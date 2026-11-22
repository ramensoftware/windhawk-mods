// ==WindhawkMod==
// @id              win7-user-account-page-restorer
// @name            Windows 7 User Account Page Restorer
// @description     This mod restores the Windows 7 User Accounts Control Panel page on Windows 10 and 11
// @version         1.0.0
// @author          babamohammed
// @github          https://github.com/babamohammed2022
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lwinhttp -lole32 -loleaut32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Windows 7 User Account Page Restorer

## About
This mod restores the **User Accounts** page of Control Panel as it was in Windows 7: the user tile with its glass
frame, "Create a password for your account", "Change your picture", "Manage another account", "Create a new
account", the guest account pages, and the classic task list on the left.

Windows 10 and 11 still have a User Accounts page, but it is a reduced one that sends most tasks to the Settings
app. This mod puts the original Windows 7 page (NT 6.1) back in its place. The page is the real one, not a
recreation: it is Microsoft's `usercpl.dll` 6.1.7600.16385 running on its own Windows 7 DirectUI engine.

No system file is replaced or modified, and nothing is written to the registry. Disable the mod and the Windows
10/11 page is back.

## Screenshots
![User Accounts page, main view](https://raw.githubusercontent.com/babamohammed2022/babamohammed2022/main/accountpage1.PNG)

![User Accounts page, account view](https://raw.githubusercontent.com/babamohammed2022/babamohammed2022/main/accountpage2.PNG)

## What is downloaded, from where, and when
The mod contains no Microsoft binaries and never asks you for a file. The first time it runs it downloads four
files, 2 MB in total, from Microsoft's public symbol server (`https://msdl.microsoft.com/download/symbols/`):

| File | Version | Used for |
|---|---|---|
| `usercpl.dll` | 6.1.7600.16385 (Windows 7) | the page |
| `dui70.dll` | 6.1.7601.23403 (Windows 7 SP1) | the DirectUI engine the page was built for |
| `shacct.dll` | 10.0.10240.16384 (Windows 10 1507) | only the picture frame bitmap; never loaded as code |
| `duser.dll` | 6.1.7600.16385 (Windows 7) | only the text of the "common" style sheet; never loaded as code |

The size and the SHA-256 of each file are checked against values pinned in the mod; a file that does not match is
discarded. The originals are kept in the mod's storage folder (`dl`). From them the mod builds, in `bin`, a private
copy of the page for the selected language (see *How it works*). Nothing is downloaded again while `dl` is present,
so after the first run the mod works offline.

The download cannot be turned off, and the mod cannot work without it. With *Keep downloaded files when the mod is
disabled* off, the files are deleted at every unload, so the download is repeated after every disable or update. The
only thing sent is the HTTPS request for the four fixed URLs, with the user agent `Microsoft-Symbol-Server/10.0.0.0`.

**Offline at first run:** the Windows 10/11 page simply stays, and the reason is written to the mod's log. Connect
to the internet and restart `explorer.exe` (or toggle the mod).

## How it works
- **The page.** Control Panel is told to read the page definition and the layouts from the mod's private copy
  instead of `System32\usercpl.dll` (one documented function, `PSPropertyBag_ReadStr`, answers differently for that
  one value). The private copy is Microsoft's file with **no code byte changed**; only its resources are extended:
  the strings (the symbol server has no `.mui` files, so the strings are embedded in the mod, in English, Italian,
  French, Spanish, Portuguese and German), the picture frame, and the page definition.
- **The "modified page".** The original Windows 7 layouts cannot load on Windows 10/11 as they are, for two reasons,
  and both are solved inside the private copy, with nothing in `System32` involved:
  - they take the picture frame from `shacct.dll`, which no longer contains it since Windows 10 1607 (this is why
    the manual method needs a patched `shacct.dll`): the frame is put into the private copy and the layouts are
    pointed to it;
  - they use the "Common" style sheet, which DirectUI reads from the system's `DUser.dll`; since Windows 8 that
    sheet is stored in a binary format the Windows 7 DirectUI cannot read: the Windows 7 text of the sheet is put
    into each layout.
- **The accounts.** The Windows 7 page talks to the system's account objects through interfaces that changed in
  Windows 10. The mod translates between the two layouts, so the page works on the Windows 10/11 implementation
  (with the system's own elevation prompt) instead of on old code. Creating a local account also runs
  `net.exe user "<name>" /add` elevated, which can show a second UAC prompt. It creates an account without a
  password; the page then performs any further changes through its account object. If the command times out or
  the account cannot be read back, creation may already have succeeded: check the account list before retrying.
- **Scope.** The mod runs only in `explorer.exe`. Until the page is opened for the first time it loads nothing.

## Settings
- **Page language**: *Automatic* follows the Windows display language (English, Italian, French, Spanish, Portuguese or German;
  English if Windows uses another language). Applies to windows opened after the change; if the page was already used in this session, restart
  `explorer.exe`.
- **Show a generic user name**: off by default. When on, the name of the signed-in user is shown as a generic one
  ("User", "Utente", "Benutzer", ...) on this page only; Windows and the account keep the real name. Saving the generic name
  unchanged in "Change the account name" does nothing, so the account is never renamed by mistake.
- **Show legacy online ID and fingerprint links**: off by default. When enabled, the page's original visibility
  checks apply; fingerprint hardware may be required. Close and reopen the page after changing the setting. These
  links can remain unavailable or nonfunctional because their Windows 7 targets are normally absent on Windows 10/11.
- **Block account removal from the classic page**: on by default. Turn it off to use the classic page's normal
  deletion flow; see the warning under *Notes and limitations*.
- **Keep downloaded files when the mod is disabled**: on by default, so that enabling the mod again needs no
  download.

## Notes and limitations
- **Best effort.** This mod is provided on a best-effort basis. It runs a Windows 7 page on top of Windows 10/11
  components that Microsoft can change at any time, so some features may not work, or may stop working after a
  Windows update. The translations of the page (French, Spanish, Portuguese and German in particular) were also written
  without the original Microsoft files and may differ from the official wording.
- **Account removal protection** is enabled by default. For safety, the mod blocks removal from the classic page
  and displays a localized warning, to avoid ambiguous deletion. To remove an account safely through Windows, use
  Settings > Accounts > Other users, after backing up important files. Settings may also remove account data.
  You can disable **Block account removal from the classic page** to use the Windows 7 page's ordinary deletion
  flow through the Windows 10/11 account implementation, not the original Windows 7 account implementation.
  **Warning:** this path has been observed to hang after the account was deleted; its profile/file handling is not
  verified. Check whether a previous attempt already deleted the account before retrying. Protection only applies
  if the page reaches the mod's remove method.
- **The Guest account is an edge case and is not supported by modern Windows.**
  The Windows 7 page can turn the Guest account on and off, but the Guest account is
  not supported by Windows 10/11, and it has been reported to work correctly only
  with a custom shell such as Explorer7/Windows7ExplorerRestorer (or equivalent software). For normal
  (non-Guest) profiles the page is expected to work.
- The mod has been created for Windows 10 1607 or later or Windows 11; 64-bit (x64).
- The mod has been tested on Windows 10 21H2 and Windows 11 24H2. Other versions from Windows 10 1607 on may work, but have not been tested.
- This is the Windows 7 page: it does not know Microsoft accounts as such (they appear as the local accounts they
  are bound to).
- **Legacy online ID and fingerprint links are best effort.** "Link online IDs" and "Manage your fingerprint data"
  are suppressed by default and can be allowed by a setting. Their original availability checks still apply, and
  functionality depends on specific legacy components or separate restorations that this mod does not provide or
  verify. The Windows 7 targets are normally absent on Windows 10/11, and even a restored component or provider may
  not be compatible or operational. In particular, this mod removes the online IDs page definition from its private
  copy: enabling the link does not restore that page, and a test produced a file-not-found error when it was clicked.
  No working restoration is guaranteed. "Parental Controls" remains conditional on a Family Safety applet being present.
- "Change your picture" lists the pictures of `ProgramData\Microsoft\User Account Pictures\Default Pictures`,
  which Windows 10/11 do not ship; "Browse for more pictures..." works.
- When the mod is disabled or updated, it closes the Explorer windows that currently show the User Accounts page and waits
  until they are gone (a window that has moved on to another folder is left alone). The **whole window closes, including
  any other tabs in it**, and so do the dialogs the page has open; save your work in those tabs first. The close request
  is repeated every second and the wait is logged every few seconds. A pending UAC prompt cannot be closed by the mod:
  answer it. The three links of the page that the mod hides come back. The page and its DirectUI file (private copies in the
  mod's storage folder) stay loaded in `explorer.exe` until it restarts, but nothing in them points into the mod.
- If the Windows 7 page cannot be loaded for any reason, the Windows 10/11 User Accounts page is used.
- After changing a setting, close open User Accounts windows.

## Credits
- **RandomGuy5747** (WinClassic) for the German translation of the page.
- **RandomGuy5747** (WinClassic) for pointing out that the Windows 7 page can run on Windows 10 with the Windows 7
  DirectUI, in the thread "Secure restoration of Control Panel applets in Windows 10/11".
- **Brawllux** (CPL Restoration Pack) for the finding that the page needs the picture frame of `shacct.dll`.
- **AdmXP8** for the technique of running an older Control Panel page from Microsoft's symbol server
  (Language Applet Restorer, Family Safety Applet Restorer).
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- language: auto
  $name: Page language
  $name:it-IT: Lingua della pagina
  $name:de-DE: Seitensprache
  $description: This setting chooses the language of the restored page. "Automatic" follows the Windows display language.
  $description:it-IT: Questa impostazione sceglie la lingua della pagina ripristinata. "Automatica" segue la lingua di visualizzazione di Windows.
  $description:de-DE: Diese Einstellung legt die Sprache der wiederhergestellten Seite fest. "Automatisch" folgt der Windows-Anzeigesprache.
  $options:
    - auto: Automatic (Windows display language)
    - en-US: English
    - it-IT: Italiano
    - fr-FR: Français
    - es-ES: Español
    - pt-PT: Português
    - de-DE: Deutsch
- hideUserName: false
  $name: Show a generic user name
  $name:it-IT: Mostra un nome utente generico
  $name:de-DE: Generischen Benutzernamen anzeigen
  $description: This setting replaces the name of the signed-in user with a generic one ("User", localized to the page language) on this page only. Windows, the account and the other accounts are not changed. Applies to windows opened after the change.
  $description:it-IT: Questa impostazione sostituisce il nome dell'utente connesso con uno generico ("Utente", localizzato nella lingua della pagina) solo in questa pagina. Windows, l'account e gli altri account non vengono modificati. Si applica alle finestre aperte dopo la modifica.
  $description:de-DE: Diese Einstellung ersetzt den Namen des angemeldeten Benutzers nur auf dieser Seite durch einen generischen Namen ("Benutzer", in der Seitensprache). Windows, das Konto und die anderen Konten werden nicht geändert. Gilt für nach der Änderung geöffnete Fenster.
- showLegacyLinks: false
  $name: Show legacy online ID and fingerprint links
  $name:it-IT: Mostra i collegamenti precedenti per ID online e impronte
  $name:de-DE: Alte Links für Online-IDs und Fingerabdrücke anzeigen
  $description: This setting allows the classic page to show the Link online IDs and Manage your fingerprint data links instead of suppressing them. They depend on the original Windows checks, including fingerprint hardware, and might not appear. Their Windows 7 targets are normally absent on Windows 10/11, so the links may not work. Close and reopen the page after changing this setting.
  $description:it-IT: Questa impostazione consente alla pagina classica di mostrare i collegamenti per ID online e impronte invece di nasconderli. Dipendono dai controlli originali di Windows, incluso il lettore di impronte, e potrebbero non apparire. Le destinazioni di Windows 7 normalmente non sono presenti in Windows 10/11, quindi i collegamenti potrebbero non funzionare. Chiudi e riapri la pagina dopo la modifica.
  $description:de-DE: Diese Einstellung erlaubt der klassischen Seite, die alten Links für Online-IDs und Fingerabdrücke anzuzeigen, statt sie zu unterdrücken. Die ursprünglichen Windows-Prüfungen einschließlich Fingerabdruckgerät gelten weiterhin, daher erscheinen sie möglicherweise nicht. Die Windows-7-Ziele fehlen unter Windows 10/11 normalerweise, sodass die Links möglicherweise nicht funktionieren. Schließe die Seite nach der Änderung und öffne sie erneut.
- blockAccountRemoval: true
  $name: Block account removal from the classic page
  $name:it-IT: Blocca la rimozione degli account dalla pagina classica
  $name:de-DE: Kontoentfernung auf der klassischen Seite blockieren
  $description: This setting blocks account removal from the classic page and shows a warning by default. Turn it off to use the Windows 7 page's normal deletion flow through the Windows 10/11 account implementation. That path may hang even after the account is deleted; its file handling is not verified. Back up files and check whether deletion already succeeded before retrying.
  $description:it-IT: Questa impostazione blocca la rimozione dalla pagina classica e mostra un avviso per impostazione predefinita. Disattivala per usare il normale percorso di eliminazione della pagina Windows 7 tramite i componenti account di Windows 10/11. La pagina potrebbe bloccarsi anche dopo la cancellazione; la gestione dei file non è verificata. Salva i file e controlla se l'account è già stato eliminato prima di riprovare.
  $description:de-DE: Diese Einstellung blockiert standardmäßig die Kontoentfernung auf der klassischen Seite und zeigt eine Warnung. Deaktiviere sie für den normalen Löschablauf der Windows-7-Seite über die Kontokomponenten von Windows 10/11. Die Seite kann auch nach der Löschung hängen bleiben; der Umgang mit Dateien ist nicht überprüft. Sichere Dateien und prüfe vor einem erneuten Versuch, ob das Konto bereits gelöscht wurde.
- keepFilesOnDisable: true
  $name: Keep downloaded files when the mod is disabled
  $name:it-IT: Conserva i file scaricati quando la mod viene disattivata
  $name:de-DE: Heruntergeladene Dateien beim Deaktivieren der Mod beibehalten
  $description: This setting decides whether the downloaded and generated files stay in the mod's storage folder. If on, they stay, so enabling the mod again works offline. If off, they are deleted when the mod is unloaded (files still in use are left).
  $description:it-IT: Questa impostazione decide se i file scaricati e generati restano nella cartella della mod. Se attiva, restano, così la riattivazione funziona senza connessione. Se disattiva, vengono eliminati allo scaricamento della mod (i file ancora in uso restano).
  $description:de-DE: Diese Einstellung legt fest, ob die heruntergeladenen und erzeugten Dateien im Speicherordner der Mod bleiben. Wenn aktiviert, bleiben sie erhalten, sodass die erneute Aktivierung offline funktioniert. Wenn deaktiviert, werden sie beim Entladen der Mod gelöscht (Dateien, die noch verwendet werden, bleiben).
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <winhttp.h>
#include <shellapi.h>
#include <oaidl.h>
#include <ocidl.h>
#include <oleauto.h>
#include <sddl.h>
#include <wincodec.h>

#include <atomic>
#include <cstdint>
#include <cstring>
#include <cwchar>
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include <algorithm>

namespace {

using Bytes = std::vector<uint8_t>;

// =====================================================================================================================
// Constants
// =====================================================================================================================

// Bump when the generated page changes, so that a new file name is used (a loaded file cannot be replaced).
constexpr wchar_t kGeneratorRevision[] = L"r2";
constexpr wchar_t kPrivateDuiName[] = L"u7dui.dll";       // same length as "DUI70.dll"
constexpr char kDuiOld[] = "DUI70.dll";
constexpr char kDuiNew[] = "u7dui.dll";
constexpr wchar_t kSymbolHost[] = L"msdl.microsoft.com";
constexpr wchar_t kSymbolPath[] = L"/download/symbols/";
constexpr DWORD kMinBuild = 14393;                         // Windows 10 1607: first build with the known interfaces

struct FileSpec {
    const wchar_t* name;      // file name in <storage>\dl
    const wchar_t* key;       // path below the symbol server's root: name/TimeDateStamp+SizeOfImage/name
    uint32_t size;
    const char* sha256;       // lower-case hex of the exact file the server returns
};
// The hashes come from a real download of each URL (see research/NOTES.md).
constexpr FileSpec kFiles[] = {
    {L"usercpl.dll", L"usercpl.dll/4A5BE08A9c000/usercpl.dll", 625664,
     "a5b54663a4c8d4d4505da792453674b86045b44abfcde0195e616d8dbcc12c20"},
    {L"dui70.dll", L"dui70.dll/56F58A79f2000/dui70.dll", 977408,
     "7eaa6ddf25510c466d55ba84fe1ea05e2c41aad09c0a1ffa8a3d55718aad147b"},
    {L"shacct.dll", L"shacct.dll/559F39D132000/shacct.dll", 183808,
     "fad771f81439fc72f2b79a7fba451c6234eb4ff871ab876b2fd0533dfe3436ec"},
    {L"duser.dll", L"duser.dll/4A5BDF2643000/duser.dll", 260608,
     "dd663029b2eb7b12fdb00fce403d8326141e540e3b9ce84cd5871473d3e2e2cf"},
};
enum { kFileUserCpl = 0, kFileDui = 1, kFileShAcct = 2, kFileDUser = 3 };

// The two classes of the page as Windows registers them (served by System32\usercpl.dll). Control Panel looks a class
// up in the registry before it creates it, so the page definition of the private copy keeps these registered ids; the
// CoCreateInstance hook answers them from the private copy while the mod's page is in use.
constexpr GUID kClsidProviderSys = {0x9b4e5207, 0x9539, 0x4258, {0xb4, 0xa0, 0x4e, 0x70, 0xe9, 0xe5, 0x65, 0xec}};
constexpr GUID kClsidInitSys = {0x15fc1bac, 0x8d83, 0x4e87, {0x8c, 0xc2, 0xa7, 0x0c, 0x9f, 0x66, 0xf9, 0x43}};

// Account objects. Same classes in Windows 7 and Windows 10/11, different interfaces.
constexpr GUID kClsidLogonEnumUsers = {0x29D365AE, 0xD23B, 0x4004, {0x86, 0x58, 0x5E, 0xD2, 0xA5, 0x9C, 0xD7, 0x7C}};
constexpr GUID kClsidElevationFactory = {0x36F0BD14, 0xD84D, 0x468C, {0xB7, 0x9C, 0x99, 0x90, 0xF3, 0xFA, 0x89, 0x7F}};
constexpr GUID kIidEnum7 = {0xDB0C527D, 0x0868, 0x4BC4, {0x9D, 0x85, 0x86, 0x8E, 0xE1, 0x33, 0x67, 0xB4}};
constexpr GUID kIidUser7 = {0x736A9095, 0xA80E, 0x4F47, {0xA0, 0xA6, 0x87, 0xF4, 0x3D, 0x57, 0x02, 0x7C}};
constexpr GUID kIidFactory7 = {0x27A363FC, 0xA242, 0x434D, {0xBD, 0x8C, 0xF4, 0x1D, 0x32, 0x41, 0x78, 0x25}};
constexpr GUID kIidEnum10 = {0x3EE328AB, 0x8F69, 0x420A, {0x9B, 0x86, 0x70, 0x80, 0xF2, 0x2A, 0xF3, 0x8B}};
[[maybe_unused]] constexpr GUID kIidUser10 = {0x01C8AC33, 0xBDCD, 0x401B, {0xA7, 0xD8, 0xE6, 0x80, 0xF4, 0xDA, 0x87, 0xFF}};
constexpr GUID kIidFactory10 = {0x6FABDA16, 0x031E, 0x47E3, {0xB2, 0xA2, 0x23, 0x39, 0xC0, 0x5C, 0xCB, 0x9E}};
constexpr GUID kIidUnknown = {0x00000000, 0x0000, 0x0000, {0xC0, 0, 0, 0, 0, 0, 0, 0x46}};
constexpr GUID kIidDispatch = {0x00020400, 0x0000, 0x0000, {0xC0, 0, 0, 0, 0, 0, 0, 0x46}};
constexpr GUID kIidClassFactory = {0x00000001, 0x0000, 0x0000, {0xC0, 0, 0, 0, 0, 0, 0, 0x46}};

// =====================================================================================================================
// Strings of the page (usercpl.dll.mui 6.1.7600.16385)
// =====================================================================================================================

struct StringEntry {
    uint16_t id;
    const wchar_t* text;
};
// <strings>
// Generated by tools/i18n.py from lang/*.json: the 172 strings of usercpl.dll.mui 6.1.7600.16385 and, as 2000-2003,
// the four of duser.dll.mui that the common style sheet names (there they are 1100-1103).
static const StringEntry kStringsEn[] = {
    {1, L"User Accounts"},
    {2, L"Change user account settings and passwords for people who share this computer."},
    {3, L"Click here to type"},
    {4, L"Change this person's account information, such as the account type, name, password, or picture, or delete this account."},
    {5, L"Change the guest account picture or prevent guest access to this computer."},
    {6, L"Provide computer access for people without a user account on this computer."},
    {7, L"The Welcome screen shows the names of everyone with a user account for this computer. This screen appears when you start the computer or switch users."},
    {8, L"In the Start menu you can choose a computer activity, such as reading new e-mail messages or browsing the Web. This menu is personalized for everyone who shares this computer."},
    {9, L"A password hint can help you remember your password."},
    {10, L"A password hint can help this person remember his or her password."},
    {11, L"Windows cannot open the User Accounts control panel."},
    {12, L"The current user account is not recognized. Restart the computer and then open User Accounts."},
    {13, L"The passwords you typed do not match. Please retype the new password in both boxes."},
    {14, L"The password you typed is incorrect. Please retype your current password."},
    {15, L"The password you typed does not meet the password policy requirements. Check the minimum password length, password complexity and password history requirements."},
    {16, L"Windows cannot change the password."},
    {17, L"Windows cannot remove the password.  Password and/or account policies require the account to have a password."},
    {18, L"The specified account name is not valid, because account names cannot contain the following characters: /\\[]\":;|<>+=,?*\n\nPlease type a different name."},
    {19, L"The specified account name cannot be used, because it is a reserved name. Please type a different name."},
    {20, L"The user name cannot be the same as the computer name.\n\nPlease type a different name."},
    {21, L"The specified group does not exist."},
    {22, L"An account named '%s' already exists. Type a different name."},
    {23, L"The specified picture is an unknown type or is not valid. Select a different picture."},
    {24, L"No camera or scanner is available. Make sure the camera is connected and is turned on, and then try again."},
    {30, L"The account you want to delete is logged on.  Before you delete it, you should switch to that user account and log off of that account.  If you continue without doing this, there is a risk of data loss.  Do you still wish to continue?"},
    {31, L"The Guest account is logged on. Before you can turn off the Guest account, you must switch to the Guest account and log it off."},
    {34, L"Windows cannot delete the account that is currently logged in."},
    {37, L"Password protected"},
    {38, L"Guest account"},
    {39, L"Standard user"},
    {40, L"Unknown account type"},
    {41, L"Administrator"},
    {42, L"Guest account is on"},
    {43, L"Guest account is off"},
    {44, L"Windows cannot remove the password."},
    {45, L"User Accounts"},
    {46, L"This action cannot be performed due to an account restriction. Please contact your administrator."},
    {48, L"Create New Account"},
    {49, L"Change an Account"},
    {50, L"Choose Picture"},
    {51, L"Rename Account"},
    {52, L"Turn on Guest Account"},
    {53, L"Change Guest Options"},
    {54, L"Manage Accounts"},
    {55, L"Change Your Name"},
    {56, L"Delete Account"},
    {57, L"Confirm Deletion"},
    {58, L"Change Password"},
    {59, L"Change Account Type"},
    {60, L"Change Your Password"},
    {61, L"Change Your Picture"},
    {62, L"Create Password"},
    {63, L"Create Your Password"},
    {64, L"Remove Password"},
    {65, L"Remove Your Password"},
    {66, L"Change Your Account Type"},
    {67, L"Windows failed to start the advanced options dialog."},
    {68, L"The specified account name is not valid, because there is already an account with that name.\n\nPlease type a different name."},
    {69, L"User account picture"},
    {70, L"User account tile"},
    {71, L"User Accounts Control Panel"},
    {72, L"Administrator privileges required"},
    {73, L"New password"},
    {74, L"Confirm new password"},
    {75, L"Type a password hint"},
    {76, L"Current password"},
    {78, L"New account name"},
    {79, L"Help"},
    {80, L"Image Files (*.bmp,*.gif,*.jpg,*.png)|*.bmp;*.dib;*.gif;*.jpg;*.jpe;*.jpeg;*.png;*.rle|Bitmap (*.bmp,*.dib,*.rle)|*.bmp;*.dib;*.rle|GIF (*.gif)|*.gif|JPEG (*.jpg)|*.jpg|PNG (*.png)|*.png|All Files (*.*)|*"},
    {91, L"This program is blocked by group policy. For more information, contact your system administrator."},
    {92, L"User Accounts Related Tasks Pane"},
    {93, L"User Accounts Main Task Pane"},
    {94, L"List of users"},
    {95, L"User Account Control Panel"},
    {100, L"Create a password reset disk"},
    {101, L"Link online IDs"},
    {102, L"Manage your file encryption certificates"},
    {103, L"Configure advanced user profile properties"},
    {104, L"Change my environment variables"},
    {105, L"Parental Controls"},
    {106, L"Manage your credentials"},
    {107, L"Manage your fingerprint data"},
    {1100, L"&Standard user"},
    {1101, L"&Administrator"},
    {1108, L"Make changes to your user account"},
    {1109, L"Change your account name"},
    {1110, L"Create a password for your account"},
    {1111, L"Change your password"},
    {1112, L"Remove your password"},
    {1113, L"Change your picture"},
    {1114, L"Change your account type"},
    {1115, L"Manage another account"},
    {1116, L"Manage User Accounts"},
    {1117, L"To change your password, press Ctrl+Alt+Del and select Change a password."},
    {1119, L"Make changes to %username%'s account"},
    {1120, L"Change Name"},
    {1121, L"Create password"},
    {1122, L"Change password"},
    {1123, L"Remove Password"},
    {1124, L"Change the picture"},
    {1128, L"Create a new account"},
    {1129, L"What do you want to change about the guest account?"},
    {1130, L"Turn off the guest account"},
    {1131, L"Name the account and choose an account type"},
    {1133, L"This name will appear on the Welcome screen and on the Start menu."},
    {1136, L"Cancel"},
    {1137, L"Create Account"},
    {1138, L"Choose a new picture for %username%'s account"},
    {1139, L"The picture you choose will appear on the Welcome screen and on the Start menu."},
    {1140, L"Browse for more pictures..."},
    {1141, L"Change Picture"},
    {1142, L"Type a new account name for %username%'s account"},
    {1145, L"Do you want to turn on the guest account?"},
    {1146, L"If you turn on the guest account, people who do not have an account can use the guest account to log on to the computer. Password-protected files, folders, or settings are not accessible to guest users."},
    {1147, L"Turn On"},
    {1148, L"Choose the account you would like to change"},
    {1149, L"What is a user account?"},
    {1150, L"Additional things you can do"},
    {1151, L"Set up Parental Controls"},
    {1155, L"Type a new account name"},
    {1157, L"Do you want to keep %username%'s files?"},
    {1158, L"Before you delete %username%'s account, Windows can automatically save the contents of %username%'s desktop and Documents, Favorites, Music, Pictures and Videos folders to a new folder called '%username%' on your desktop. However, Windows cannot save %username%'s e-mail messages and other settings."},
    {1159, L"Delete Files"},
    {1160, L"Keep Files"},
    {1161, L"Are you sure you want to delete %username%'s account?"},
    {1162, L"You are deleting the account, but keeping the files."},
    {1163, L"%username% will no longer be able to log on, and all of %username%'s settings will be deleted. However, %username%'s files will be saved on your desktop in a folder called '%username%'"},
    {1164, L"Delete Account"},
    {1165, L"Change %username%'s password"},
    {1166, L"You are resetting the password for %username%.  If you do this, %username% will lose all personal certificates and stored passwords for Web sites or network resources."},
    {1167, L"You are changing the password for %username%.  If you do this, %username% will lose all EFS-encrypted files, personal certificates, and stored passwords for Web sites or network resources."},
    {1168, L"If %username% has forgotten the password, he or she can use the password reset floppy disk."},
    {1169, L"To avoid losing data in the future, ask %username% to make a password reset floppy disk."},
    {1172, L"If your password contains capital letters, they must be typed the same way every time you log on."},
    {1174, L"The password hint will be visible to everyone who uses this computer."},
    {1175, L"Change Account Type"},
    {1176, L"Change your password"},
    {1181, L"Choose a new picture for your account"},
    {1182, L"Create a password for %username%'s account"},
    {1183, L"You are creating a password for %username%."},
    {1184, L"If you do this, %username% will lose all EFS-encrypted files, personal certificates and stored passwords for Web sites or network resources."},
    {1185, L"To avoid losing data in the future, ask %username% to make a password reset floppy disk."},
    {1186, L"Create a password for your account"},
    {1188, L"You are removing the password for %username%.  If you do this, %username% will lose all personal certificates and stored passwords for Web sites or network resources."},
    {1189, L"You are removing the password for %username%.  If you do this, %username% will lose all EFS-encrypted files, personal certificates, and stored passwords for Web sites or network resources."},
    {1190, L"If %username% has forgotten the password, he or she can use the password reset floppy disk."},
    {1191, L"Remove your password"},
    {1192, L"Are you sure you want to remove your password?"},
    {1193, L"If you remove your password, other people can gain access to your account and change settings."},
    {1194, L"If you remove your password, other people can gain access to your account and change settings."},
    {1195, L"If you have forgotten your password, you can use the password reset floppy disk."},
    {1196, L"Remove a password"},
    {1197, L"Choose a new account type for %username%"},
    {1198, L"Standard account users can use most software and change system settings that do not affect other users or the security of the computer."},
    {1199, L"Administrators have complete access to the computer and can make any desired changes. Based on notification settings, administrators may be asked to provide their password or confirmation before making changes that affect other users.\n\nWe recommend that you protect every account with a strong password."},
    {1200, L"Select your new account type"},
    {1201, L"Go to the main User Accounts page"},
    {1203, L"You must assign another user on this computer to have an administrator account before you can change this user's account type. This ensures that there is always at least one user with a computer administrator account on this computer."},
    {1204, L"What is a password hint?"},
    {1205, L"How to create a strong password"},
    {1206, L"Why is a standard account recommended?"},
    {1207, L"Windows will delete all of %username%'s files, and then delete %username%'s account."},
    {1213, L"Change User Account Control settings"},
    {1216, L"Change the account name"},
    {1217, L"Create a password"},
    {1218, L"Change the password"},
    {1219, L"Remove the password"},
    {1220, L"Change the picture"},
    {1221, L"Change the account type"},
    {1222, L"Delete the account"},
    {1223, L"Manage another account"},
    {1226, L"If the password contains capital letters, they must be typed the same way every time."},
    {2000, L"Used to change the vertical viewing area"},
    {2001, L"Used to change the horizontal viewing area"},
    {2002, L"Horizontal"},
    {2003, L"Vertical"},
};
static const StringEntry kStringsIt[] = {
    {1, L"Account utente"},
    {2, L"Consente di cambiare le impostazioni dell'account utente e le password per gli utenti che condividono questo computer."},
    {3, L"Fare clic qui per scrivere"},
    {4, L"Modifica le informazioni sull'account di questo utente, come il tipo di account, il nome, la password o l'immagine, oppure consente di eliminare l'account."},
    {5, L"Consente di modificare l'immagine dell'account guest o impedisce l'accesso guest al computer."},
    {6, L"Fornisce l'accesso al computer a utenti che non dispongono di un account utente nel computer."},
    {7, L"Nella schermata iniziale sono visualizzati i nomi degli utenti con un account per il computer. La schermata viene visualizzata all'avvio del computer o quando si passa da un utente all'altro."},
    {8, L"Nel menu Start \u00E8 possibile scegliere un'attivit\u00E0 del computer, ad esempio, leggere i messaggi di posta oppure esplorare il Web. Questo menu \u00E8 personalizzato per ciascun utente che condivide il computer."},
    {9, L"Un suggerimento pu\u00F2 essere utile nel caso in cui la password sia stata dimenticata."},
    {10, L"Un suggerimento pu\u00F2 essere utile nel caso in cui l'utente abbia dimenticato la propria password."},
    {11, L"Impossibile aprire il pannello di controllo Account utente."},
    {12, L"L'account utente non \u00E8 riconosciuto. Riavviare il computer e quindi aprire Account utente."},
    {13, L"Le password immesse non corrispondono. Immettere la nuova password in entrambe le caselle."},
    {14, L"La password digitata non \u00E8 valida. Ridigitare la password."},
    {15, L"La password non soddisfa i requisiti dei criteri password. Verificare la lunghezza minima della password, la complessit\u00E0 della password e i requisiti della cronologia della password."},
    {16, L"Impossibile modificare la password."},
    {17, L"Impossibile rimuovere la password. I criteri di password e/o account richiedono una password per ogni account."},
    {18, L"Il nome di account specificato non \u00E8 valido. I nomi di account non possono contenere i caratteri seguenti: /\\[]\":;|<>+=,?*\n\nDigitare un nome diverso."},
    {19, L"Impossibile utilizzare il nome di account specificato. Il nome \u00E8 riservato. Digitare un nome diverso."},
    {20, L"Il nome utente non pu\u00F2 essere uguale al nome computer.\n\nDigitare un nome diverso."},
    {21, L"Il gruppo specificato non esiste."},
    {22, L"Un account denominato '%s' esiste gi\u00E0. Digitare un nome diverso."},
    {23, L"L'immagine specificata \u00E8 di tipo sconosciuto o non \u00E8 valida. Selezionare un'immagine diversa."},
    {24, L"Nessuna fotocamera o scanner disponibili. Assicurarsi che la fotocamera sia collegata e accesa, quindi riprovare."},
    {30, L"\u00C8 presente un utente connesso con l'account che si desidera eliminare. Prima di eliminare l'account, passare a tale account utente e disconnetterlo. Se si continua senza disconnettere l'utente, potrebbe verificarsi una perdita di dati. Continuare?"},
    {31, L"L'account Guest \u00E8 connesso. Prima di disattivare l'account Guest \u00E8 necessario passare all'account Guest e disconnetterlo."},
    {34, L"Impossibile eliminare l'account attualmente connesso."},
    {37, L"Protetto da password"},
    {38, L"Account Guest"},
    {39, L"Utente standard"},
    {40, L"Tipo di account sconosciuto"},
    {41, L"Administrator"},
    {42, L"Account Guest attivato"},
    {43, L"Account Guest non attivato"},
    {44, L"Impossibile rimuovere la password."},
    {45, L"Account utente"},
    {46, L"Impossibile eseguire questa azione a causa delle restrizioni sull'account. Rivolgersi all'amministratore."},
    {48, L"Crea nuovo account"},
    {49, L"Cambia un account"},
    {50, L"Seleziona immagine"},
    {51, L"Rinomina account"},
    {52, L"Attiva account Guest"},
    {53, L"Modifica opzioni Guest"},
    {54, L"Gestisci account"},
    {55, L"Modifica nome"},
    {56, L"Elimina account"},
    {57, L"Conferma eliminazione"},
    {58, L"Cambia password"},
    {59, L"Cambia tipo di account"},
    {60, L"Modifica password"},
    {61, L"Cambia immagine"},
    {62, L"Crea password"},
    {63, L"Crea password"},
    {64, L"Rimozione password"},
    {65, L"Rimuovi password"},
    {66, L"Modifica tipo di account"},
    {67, L"Impossibile avviare la finestra di dialogo Opzioni avanzate."},
    {68, L"Il nome account specificato non \u00E8 valido perch\u00E9 esiste gi\u00E0 un account con lo stesso nome.\n\nDigitare un nome diverso."},
    {69, L"Immagine account utente"},
    {70, L"File account utente"},
    {71, L"Account utente nel Pannello di controllo"},
    {72, L"Richiesti privilegi di amministrazione"},
    {73, L"Nuova password"},
    {74, L"Conferma nuova password"},
    {75, L"Digitare suggerimento password"},
    {76, L"Password corrente"},
    {78, L"Nuovo nome account"},
    {79, L"?"},
    {80, L"File di immagine (*.bmp,*.gif,*.jpg,*.png)|*.bmp;*.dib;*.gif;*.jpg;*.jpe;*.jpeg;*.png;*.rle|Bitmap (*.bmp,*.dib,*.rle)|*.bmp;*.dib;*.rle|GIF (*.gif)|*.gif|JPEG (*.jpg)|*.jpg|PNG (*.png)|*.png|Tutti i file (*.*)|*"},
    {91, L"Questo programma \u00E8 bloccato da un criterio di gruppo. Per ulteriori informazioni, contattare l'amministratore del sistema."},
    {92, L"Riquadro attivit\u00E0 correlate agli account utente"},
    {93, L"Riquadro attivit\u00E0 principale per gli account utente"},
    {94, L"Elenco di utenti"},
    {95, L"Pannello di controllo Account utente"},
    {100, L"Crea un disco di reimpostazione password"},
    {101, L"Collega ID online"},
    {102, L"Gestione dei certificati di crittografia dei file"},
    {103, L"Configura propriet\u00E0 profilo utente esperto"},
    {104, L"Modifica variabili di ambiente"},
    {105, L"Controllo genitori"},
    {106, L"Gestione delle credenziali"},
    {107, L"Gestione dati impronta digitale"},
    {1100, L"&Utente standard"},
    {1101, L"A&dministrator"},
    {1108, L"Modifica dell'account utente"},
    {1109, L"Modifica nome account"},
    {1110, L"Crea una password per l'account"},
    {1111, L"Modifica password"},
    {1112, L"Rimuovi password"},
    {1113, L"Cambia immagine"},
    {1114, L"Modifica tipo di account"},
    {1115, L"Gestisci un altro account"},
    {1116, L"Gestisci account utente"},
    {1117, L"Per cambiare la password, premere CTRL+ALT+CANC e scegliere Cambia password."},
    {1119, L"Modifica l'account di %username%"},
    {1120, L"Modifica nome"},
    {1121, L"Crea password"},
    {1122, L"Cambia password"},
    {1123, L"Rimozione password"},
    {1124, L"Cambia immagine"},
    {1128, L"Crea nuovo account"},
    {1129, L"Cosa si desidera cambiare dell'account Guest?"},
    {1130, L"Disattiva l'account Guest"},
    {1131, L"Assegnare un nome all'account e scegliere il tipo di account"},
    {1133, L"Questo nome verr\u00E0 visualizzato nella schermata iniziale e nel menu Start."},
    {1136, L"Annulla"},
    {1137, L"Crea account"},
    {1138, L"Scegliere una nuova immagine per l'account di %username%"},
    {1139, L"L'immagine selezionata verr\u00E0 visualizzata nella schermata iniziale e nel menu Start."},
    {1140, L"Cerca altre immagini..."},
    {1141, L"Cambia immagine"},
    {1142, L"Specificare un nuovo nome per l'account di %username%"},
    {1145, L"Attivare l'account Guest?"},
    {1146, L"Se si attiva l'account Guest, gli utenti che non dispongono di un account potranno accedere al computer utilizzando l'account Guest. Le cartelle, le impostazioni e i file protetti da password non sono accessibili agli utenti Guest."},
    {1147, L"Attiva"},
    {1148, L"Selezionare l'account da modificare"},
    {1149, L"Che cos'\u00E8 un account utente?"},
    {1150, L"Ulteriori operazioni"},
    {1151, L"Imposta Controllo genitori"},
    {1155, L"Immettere un nuovo nome account"},
    {1157, L"Mantenere i file di %username%?"},
    {1158, L"Prima di eliminare l'account di %username%, \u00E8 possibile salvare automaticamente il contenuto delle cartelle Desktop, Documenti, Preferiti, Musica, Immagini e Video di %username% in una nuova cartella denominata '%username%' sul desktop. Non \u00E8 tuttavia possibile salvare i messaggi di posta elettronica e altre impostazioni di %username%."},
    {1159, L"Elimina i file"},
    {1160, L"Mantieni i file"},
    {1161, L"Eliminare l'account di %username%?"},
    {1162, L"Si sta eliminando l'account, ma si stanno conservando i file."},
    {1163, L"%username% non potr\u00E0 pi\u00F9 eseguire l'accesso e tutte le impostazioni di %username% verranno eliminate. I file di %username% verranno tuttavia salvati sul desktop in una cartella denominata '%username%'."},
    {1164, L"Elimina account"},
    {1165, L"Modificare la password di %username%"},
    {1166, L"Si sta per ripristinare la password di %username%. Se si continua, %username% perder\u00E0 tutti i certificati personali e le password archiviate per siti Web o risorse di rete."},
    {1167, L"Si sta per modificare la password di %username%. Se si continua, %username% perder\u00E0 tutti i file crittografati EFS, i certificati personali e le password archiviate per siti Web o risorse di rete."},
    {1168, L"Se %username% ha dimenticato la password, potr\u00E0 utilizzare il disco floppy di reimpostazione password."},
    {1169, L"Per evitare in futuro la perdita di dati, chiedere a %username% di creare un disco floppy di reimpostazione password."},
    {1172, L"Se la password contiene lettere maiuscole, queste dovranno essere digitate come tali a ogni accesso."},
    {1174, L"Il suggerimento sar\u00E0 visibile a chiunque utilizzi il computer."},
    {1175, L"Cambia tipo di account"},
    {1176, L"Modifica password"},
    {1181, L"Scegliere una nuova immagine per l'account"},
    {1182, L"Creare una password per l'account di %username%"},
    {1183, L"Si sta per creare una password per %username%."},
    {1184, L"Se si continua, %username% perder\u00E0 tutti i file crittografati EFS, i certificati personali e le password archiviate per siti Web o risorse di rete."},
    {1185, L"Per evitare in futuro la perdita di dati, chiedere a %username% di creare un disco floppy di reimpostazione password."},
    {1186, L"Crea una password per l'account"},
    {1188, L"Si sta per rimuovere la password di %username%. Se si continua, %username% perder\u00E0 tutti i certificati personali e le password archiviate per siti Web o risorse di rete."},
    {1189, L"Si sta per rimuovere la password di %username%. Se si continua, %username% perder\u00E0 tutti i file crittografati EFS, i certificati personali e le password archiviate per siti Web o risorse di rete."},
    {1190, L"Se %username% ha dimenticato la password, potr\u00E0 utilizzare il disco floppy di reimpostazione password."},
    {1191, L"Rimuovi password"},
    {1192, L"Rimuovere la password?"},
    {1193, L"Se la password viene rimossa, altri utenti saranno in grado di accedere all'account e modificare le impostazioni."},
    {1194, L"Se la password viene rimossa, altri utenti saranno in grado di accedere all'account e modificare le impostazioni."},
    {1195, L"Se si \u00E8 dimenticata la password, sar\u00E0 possibile utilizzare il disco floppy di reimpostazione password."},
    {1196, L"Rimuovere una password"},
    {1197, L"Scegliere un nuovo tipo di account per %username%"},
    {1198, L"Gli utenti con account standard possono utilizzare la maggior parte delle applicazioni software e modificare le impostazioni di sistema che non incidono sugli altri utenti o sulla sicurezza del computer."},
    {1199, L"Gli amministratori dispongono di accesso completo al computer e possono apportare qualsiasi modifica. In base alle impostazioni di notifica, agli amministratori pu\u00F2 essere richiesto di immettere la propria password o di dare conferma prima di apportare modifiche che interessano altri utenti.\n\n\u00C8 consigliabile proteggere ogni account con una password complessa."},
    {1200, L"Selezionare il nuovo tipo di account"},
    {1201, L"Apri la finestra principale Account utente"},
    {1203, L"Per modificare il tipo di account di questo utente, \u00E8 necessario che un altro utente di questo computer disponga di un account di amministratore. In tal modo esister\u00E0 sempre almeno un utente del computer con un account di amministratore."},
    {1204, L"Che cos'\u00E8 un suggerimento password"},
    {1205, L"Come creare una password complessa"},
    {1206, L"Perch\u00E9 \u00E8 consigliabile utilizzare account standard"},
    {1207, L"Tutti i file di %username% verranno eliminati, quindi verr\u00E0 eliminato l'account di %username%."},
    {1213, L"Modifica le impostazioni di Controllo dell'account utente"},
    {1216, L"Modifica il nome account"},
    {1217, L"Crea password"},
    {1218, L"Modifica la password"},
    {1219, L"Rimuovi la password"},
    {1220, L"Cambia immagine"},
    {1221, L"Cambia tipo di account"},
    {1222, L"Elimina account"},
    {1223, L"Gestisci un altro account"},
    {1226, L"Se la password contiene lettere maiuscole, occorre digitarle sempre allo stesso modo."},
    {2000, L"Per modificare l'area di visualizzazione verticale"},
    {2001, L"Per modificare l'area di visualizzazione orizzontale"},
    {2002, L"Orizzontale"},
    {2003, L"Verticale"},
};
static const StringEntry kStringsFr[] = {
    {1, L"Comptes d'utilisateurs"},
    {2, L"Modifier les param\u00E8tres des comptes d'utilisateurs et les mots de passe des personnes qui partagent cet ordinateur."},
    {3, L"Cliquez ici pour taper"},
    {4, L"Modifier les informations du compte de cette personne, telles que le type de compte, le nom, le mot de passe ou l'image, ou supprimer ce compte."},
    {5, L"Modifier l'image du compte invit\u00E9 ou emp\u00EAcher l'acc\u00E8s invit\u00E9 \u00E0 cet ordinateur."},
    {6, L"Donner acc\u00E8s \u00E0 l'ordinateur aux personnes qui n'ont pas de compte d'utilisateur sur cet ordinateur."},
    {7, L"L'\u00E9cran d'accueil affiche les noms de toutes les personnes ayant un compte d'utilisateur sur cet ordinateur. Cet \u00E9cran appara\u00EEt au d\u00E9marrage de l'ordinateur ou lors du changement d'utilisateur."},
    {8, L"Dans le menu D\u00E9marrer, vous pouvez choisir une activit\u00E9, comme lire de nouveaux messages \u00E9lectroniques ou naviguer sur le Web. Ce menu est personnalis\u00E9 pour chaque personne qui partage cet ordinateur."},
    {9, L"Un indice de mot de passe peut vous aider \u00E0 vous souvenir de votre mot de passe."},
    {10, L"Un indice de mot de passe peut aider cette personne \u00E0 se souvenir de son mot de passe."},
    {11, L"Windows ne peut pas ouvrir le Panneau de configuration Comptes d'utilisateurs."},
    {12, L"Le compte d'utilisateur actuel n'est pas reconnu. Red\u00E9marrez l'ordinateur, puis ouvrez Comptes d'utilisateurs."},
    {13, L"Les mots de passe que vous avez tap\u00E9s ne correspondent pas. Retapez le nouveau mot de passe dans les deux zones."},
    {14, L"Le mot de passe que vous avez tap\u00E9 est incorrect. Retapez votre mot de passe actuel."},
    {15, L"Le mot de passe que vous avez tap\u00E9 ne respecte pas les exigences de la strat\u00E9gie de mot de passe. V\u00E9rifiez la longueur minimale, la complexit\u00E9 et l'historique des mots de passe."},
    {16, L"Windows ne peut pas modifier le mot de passe."},
    {17, L"Windows ne peut pas supprimer le mot de passe. Les strat\u00E9gies de mot de passe ou de compte exigent que le compte ait un mot de passe."},
    {18, L"Le nom de compte sp\u00E9cifi\u00E9 n'est pas valide, car les noms de compte ne peuvent pas contenir les caract\u00E8res suivants : /\\[]\":;|<>+=,?*\n\nTapez un autre nom."},
    {19, L"Le nom de compte sp\u00E9cifi\u00E9 ne peut pas \u00EAtre utilis\u00E9, car il s'agit d'un nom r\u00E9serv\u00E9. Tapez un autre nom."},
    {20, L"Le nom d'utilisateur ne peut pas \u00EAtre identique au nom de l'ordinateur.\n\nTapez un autre nom."},
    {21, L"Le groupe sp\u00E9cifi\u00E9 n'existe pas."},
    {22, L"Un compte nomm\u00E9 '%s' existe d\u00E9j\u00E0. Tapez un autre nom."},
    {23, L"L'image sp\u00E9cifi\u00E9e est d'un type inconnu ou n'est pas valide. S\u00E9lectionnez une autre image."},
    {24, L"Aucun appareil photo ou scanneur n'est disponible. V\u00E9rifiez que l'appareil photo est connect\u00E9 et allum\u00E9, puis r\u00E9essayez."},
    {30, L"Le compte que vous voulez supprimer est connect\u00E9. Avant de le supprimer, vous devriez passer \u00E0 ce compte d'utilisateur et le fermer. Si vous continuez sans le faire, vous risquez de perdre des donn\u00E9es. Voulez-vous quand m\u00EAme continuer ?"},
    {31, L"Le compte invit\u00E9 est connect\u00E9. Avant de pouvoir d\u00E9sactiver le compte invit\u00E9, vous devez passer au compte invit\u00E9 et fermer sa session."},
    {34, L"Windows ne peut pas supprimer le compte actuellement connect\u00E9."},
    {37, L"Prot\u00E9g\u00E9 par mot de passe"},
    {38, L"Compte invit\u00E9"},
    {39, L"Utilisateur standard"},
    {40, L"Type de compte inconnu"},
    {41, L"Administrateur"},
    {42, L"Le compte invit\u00E9 est activ\u00E9"},
    {43, L"Le compte invit\u00E9 est d\u00E9sactiv\u00E9"},
    {44, L"Windows ne peut pas supprimer le mot de passe."},
    {45, L"Comptes d'utilisateurs"},
    {46, L"Cette action ne peut pas \u00EAtre effectu\u00E9e en raison d'une restriction du compte. Contactez votre administrateur."},
    {48, L"Cr\u00E9er un nouveau compte"},
    {49, L"Modifier un compte"},
    {50, L"Choisir une image"},
    {51, L"Renommer le compte"},
    {52, L"Activer le compte invit\u00E9"},
    {53, L"Modifier les options d'invit\u00E9"},
    {54, L"G\u00E9rer les comptes"},
    {55, L"Modifier votre nom"},
    {56, L"Supprimer le compte"},
    {57, L"Confirmer la suppression"},
    {58, L"Modifier le mot de passe"},
    {59, L"Modifier le type de compte"},
    {60, L"Modifier votre mot de passe"},
    {61, L"Modifier votre image"},
    {62, L"Cr\u00E9er un mot de passe"},
    {63, L"Cr\u00E9er votre mot de passe"},
    {64, L"Supprimer le mot de passe"},
    {65, L"Supprimer votre mot de passe"},
    {66, L"Modifier votre type de compte"},
    {67, L"Windows n'a pas pu d\u00E9marrer la bo\u00EEte de dialogue des options avanc\u00E9es."},
    {68, L"Le nom de compte sp\u00E9cifi\u00E9 n'est pas valide, car un compte portant ce nom existe d\u00E9j\u00E0.\n\nTapez un autre nom."},
    {69, L"Image du compte d'utilisateur"},
    {70, L"Vignette du compte d'utilisateur"},
    {71, L"Panneau de configuration Comptes d'utilisateurs"},
    {72, L"Privil\u00E8ges d'administrateur requis"},
    {73, L"Nouveau mot de passe"},
    {74, L"Confirmer le nouveau mot de passe"},
    {75, L"Tapez un indice de mot de passe"},
    {76, L"Mot de passe actuel"},
    {78, L"Nouveau nom de compte"},
    {79, L"Aide"},
    {80, L"Fichiers image (*.bmp,*.gif,*.jpg,*.png)|*.bmp;*.dib;*.gif;*.jpg;*.jpe;*.jpeg;*.png;*.rle|Bitmap (*.bmp,*.dib,*.rle)|*.bmp;*.dib;*.rle|GIF (*.gif)|*.gif|JPEG (*.jpg)|*.jpg|PNG (*.png)|*.png|Tous les fichiers (*.*)|*"},
    {91, L"Ce programme est bloqu\u00E9 par une strat\u00E9gie de groupe. Pour plus d'informations, contactez votre administrateur syst\u00E8me."},
    {92, L"Volet des t\u00E2ches connexes des comptes d'utilisateurs"},
    {93, L"Volet des t\u00E2ches principales des comptes d'utilisateurs"},
    {94, L"Liste des utilisateurs"},
    {95, L"Panneau de configuration Compte d'utilisateur"},
    {100, L"Cr\u00E9er un disque de r\u00E9initialisation du mot de passe"},
    {101, L"Lier des ID en ligne"},
    {102, L"G\u00E9rer vos certificats de chiffrement de fichiers"},
    {103, L"Configurer les propri\u00E9t\u00E9s avanc\u00E9es du profil utilisateur"},
    {104, L"Modifier mes variables d'environnement"},
    {105, L"Contr\u00F4le parental"},
    {106, L"G\u00E9rer vos informations d'identification"},
    {107, L"G\u00E9rer vos donn\u00E9es d'empreintes digitales"},
    {1100, L"Utilisateur &standard"},
    {1101, L"&Administrateur"},
    {1108, L"Apporter des modifications \u00E0 votre compte d'utilisateur"},
    {1109, L"Modifier le nom de votre compte"},
    {1110, L"Cr\u00E9er un mot de passe pour votre compte"},
    {1111, L"Modifier votre mot de passe"},
    {1112, L"Supprimer votre mot de passe"},
    {1113, L"Modifier votre image"},
    {1114, L"Modifier le type de votre compte"},
    {1115, L"G\u00E9rer un autre compte"},
    {1116, L"G\u00E9rer les comptes d'utilisateurs"},
    {1117, L"Pour modifier votre mot de passe, appuyez sur Ctrl+Alt+Suppr et s\u00E9lectionnez Modifier un mot de passe."},
    {1119, L"Apporter des modifications au compte de %username%"},
    {1120, L"Modifier le nom"},
    {1121, L"Cr\u00E9er un mot de passe"},
    {1122, L"Modifier le mot de passe"},
    {1123, L"Supprimer le mot de passe"},
    {1124, L"Modifier l'image"},
    {1128, L"Cr\u00E9er un nouveau compte"},
    {1129, L"Que voulez-vous modifier dans le compte invit\u00E9 ?"},
    {1130, L"D\u00E9sactiver le compte invit\u00E9"},
    {1131, L"Nommez le compte et choisissez un type de compte"},
    {1133, L"Ce nom appara\u00EEtra sur l'\u00E9cran d'accueil et dans le menu D\u00E9marrer."},
    {1136, L"Annuler"},
    {1137, L"Cr\u00E9er un compte"},
    {1138, L"Choisissez une nouvelle image pour le compte de %username%"},
    {1139, L"L'image que vous choisissez appara\u00EEtra sur l'\u00E9cran d'accueil et dans le menu D\u00E9marrer."},
    {1140, L"Rechercher d'autres images..."},
    {1141, L"Modifier l'image"},
    {1142, L"Tapez un nouveau nom pour le compte de %username%"},
    {1145, L"Voulez-vous activer le compte invit\u00E9 ?"},
    {1146, L"Si vous activez le compte invit\u00E9, les personnes qui n'ont pas de compte pourront l'utiliser pour ouvrir une session sur l'ordinateur. Les fichiers, dossiers ou param\u00E8tres prot\u00E9g\u00E9s par mot de passe ne sont pas accessibles aux invit\u00E9s."},
    {1147, L"Activer"},
    {1148, L"Choisissez le compte que vous souhaitez modifier"},
    {1149, L"Qu'est-ce qu'un compte d'utilisateur ?"},
    {1150, L"Autres actions possibles"},
    {1151, L"Configurer le contr\u00F4le parental"},
    {1155, L"Tapez un nouveau nom de compte"},
    {1157, L"Voulez-vous conserver les fichiers de %username% ?"},
    {1158, L"Avant de supprimer le compte de %username%, Windows peut enregistrer automatiquement le contenu du Bureau et des dossiers Documents, Favoris, Musique, Images et Vid\u00E9os de %username% dans un nouveau dossier nomm\u00E9 '%username%' sur votre bureau. Toutefois, Windows ne peut pas enregistrer les messages \u00E9lectroniques et les autres param\u00E8tres de %username%."},
    {1159, L"Supprimer les fichiers"},
    {1160, L"Conserver les fichiers"},
    {1161, L"Voulez-vous vraiment supprimer le compte de %username% ?"},
    {1162, L"Vous supprimez le compte, mais vous conservez les fichiers."},
    {1163, L"%username% ne pourra plus ouvrir de session et tous les param\u00E8tres de %username% seront supprim\u00E9s. Toutefois, les fichiers de %username% seront enregistr\u00E9s sur votre bureau dans un dossier nomm\u00E9 '%username%'."},
    {1164, L"Supprimer le compte"},
    {1165, L"Modifier le mot de passe de %username%"},
    {1166, L"Vous r\u00E9initialisez le mot de passe de %username%. Si vous continuez, %username% perdra tous ses certificats personnels et les mots de passe enregistr\u00E9s pour les sites Web ou les ressources r\u00E9seau."},
    {1167, L"Vous modifiez le mot de passe de %username%. Si vous continuez, %username% perdra tous ses fichiers chiffr\u00E9s avec EFS, ses certificats personnels et les mots de passe enregistr\u00E9s pour les sites Web ou les ressources r\u00E9seau."},
    {1168, L"Si %username% a oubli\u00E9 son mot de passe, il ou elle peut utiliser la disquette de r\u00E9initialisation du mot de passe."},
    {1169, L"Pour \u00E9viter de perdre des donn\u00E9es \u00E0 l'avenir, demandez \u00E0 %username% de cr\u00E9er une disquette de r\u00E9initialisation du mot de passe."},
    {1172, L"Si votre mot de passe contient des majuscules, elles doivent \u00EAtre tap\u00E9es de la m\u00EAme fa\u00E7on \u00E0 chaque ouverture de session."},
    {1174, L"L'indice de mot de passe sera visible par toutes les personnes qui utilisent cet ordinateur."},
    {1175, L"Modifier le type de compte"},
    {1176, L"Modifier votre mot de passe"},
    {1181, L"Choisissez une nouvelle image pour votre compte"},
    {1182, L"Cr\u00E9er un mot de passe pour le compte de %username%"},
    {1183, L"Vous cr\u00E9ez un mot de passe pour %username%."},
    {1184, L"Si vous continuez, %username% perdra tous ses fichiers chiffr\u00E9s avec EFS, ses certificats personnels et les mots de passe enregistr\u00E9s pour les sites Web ou les ressources r\u00E9seau."},
    {1185, L"Pour \u00E9viter de perdre des donn\u00E9es \u00E0 l'avenir, demandez \u00E0 %username% de cr\u00E9er une disquette de r\u00E9initialisation du mot de passe."},
    {1186, L"Cr\u00E9er un mot de passe pour votre compte"},
    {1188, L"Vous supprimez le mot de passe de %username%. Si vous continuez, %username% perdra tous ses certificats personnels et les mots de passe enregistr\u00E9s pour les sites Web ou les ressources r\u00E9seau."},
    {1189, L"Vous supprimez le mot de passe de %username%. Si vous continuez, %username% perdra tous ses fichiers chiffr\u00E9s avec EFS, ses certificats personnels et les mots de passe enregistr\u00E9s pour les sites Web ou les ressources r\u00E9seau."},
    {1190, L"Si %username% a oubli\u00E9 son mot de passe, il ou elle peut utiliser la disquette de r\u00E9initialisation du mot de passe."},
    {1191, L"Supprimer votre mot de passe"},
    {1192, L"Voulez-vous vraiment supprimer votre mot de passe ?"},
    {1193, L"Si vous supprimez votre mot de passe, d'autres personnes pourront acc\u00E9der \u00E0 votre compte et modifier les param\u00E8tres."},
    {1194, L"Si vous supprimez votre mot de passe, d'autres personnes pourront acc\u00E9der \u00E0 votre compte et modifier les param\u00E8tres."},
    {1195, L"Si vous avez oubli\u00E9 votre mot de passe, vous pouvez utiliser la disquette de r\u00E9initialisation du mot de passe."},
    {1196, L"Supprimer un mot de passe"},
    {1197, L"Choisissez un nouveau type de compte pour %username%"},
    {1198, L"Les utilisateurs de comptes standard peuvent utiliser la plupart des logiciels et modifier les param\u00E8tres syst\u00E8me qui n'affectent pas les autres utilisateurs ni la s\u00E9curit\u00E9 de l'ordinateur."},
    {1199, L"Les administrateurs ont un acc\u00E8s complet \u00E0 l'ordinateur et peuvent effectuer toutes les modifications souhait\u00E9es. Selon les param\u00E8tres de notification, il peut \u00EAtre demand\u00E9 aux administrateurs de fournir leur mot de passe ou une confirmation avant d'effectuer des modifications qui affectent d'autres utilisateurs.\n\nNous vous recommandons de prot\u00E9ger chaque compte par un mot de passe fort."},
    {1200, L"S\u00E9lectionnez votre nouveau type de compte"},
    {1201, L"Acc\u00E9der \u00E0 la page principale Comptes d'utilisateurs"},
    {1203, L"Vous devez attribuer un compte d'administrateur \u00E0 un autre utilisateur de cet ordinateur avant de pouvoir modifier le type de compte de cet utilisateur. Cela garantit qu'il y a toujours au moins un utilisateur disposant d'un compte d'administrateur sur cet ordinateur."},
    {1204, L"Qu'est-ce qu'un indice de mot de passe ?"},
    {1205, L"Comment cr\u00E9er un mot de passe fort"},
    {1206, L"Pourquoi un compte standard est-il recommand\u00E9 ?"},
    {1207, L"Windows supprimera tous les fichiers de %username%, puis supprimera le compte de %username%."},
    {1213, L"Modifier les param\u00E8tres du Contr\u00F4le de compte d'utilisateur"},
    {1216, L"Modifier le nom du compte"},
    {1217, L"Cr\u00E9er un mot de passe"},
    {1218, L"Modifier le mot de passe"},
    {1219, L"Supprimer le mot de passe"},
    {1220, L"Modifier l'image"},
    {1221, L"Modifier le type de compte"},
    {1222, L"Supprimer le compte"},
    {1223, L"G\u00E9rer un autre compte"},
    {1226, L"Si le mot de passe contient des majuscules, elles doivent \u00EAtre tap\u00E9es de la m\u00EAme fa\u00E7on \u00E0 chaque fois."},
    {2000, L"Utilis\u00E9 pour modifier la zone d'affichage verticale"},
    {2001, L"Utilis\u00E9 pour modifier la zone d'affichage horizontale"},
    {2002, L"Horizontal"},
    {2003, L"Vertical"},
};
static const StringEntry kStringsEs[] = {
    {1, L"Cuentas de usuario"},
    {2, L"Cambiar la configuraci\u00F3n de las cuentas de usuario y las contrase\u00F1as de las personas que comparten este equipo."},
    {3, L"Haga clic aqu\u00ED para escribir"},
    {4, L"Cambiar la informaci\u00F3n de la cuenta de esta persona, como el tipo de cuenta, el nombre, la contrase\u00F1a o la imagen, o eliminar esta cuenta."},
    {5, L"Cambiar la imagen de la cuenta de invitado o impedir el acceso de invitados a este equipo."},
    {6, L"Proporcionar acceso al equipo a las personas que no tienen una cuenta de usuario en este equipo."},
    {7, L"La pantalla de bienvenida muestra los nombres de todas las personas con una cuenta de usuario en este equipo. Esta pantalla aparece al iniciar el equipo o al cambiar de usuario."},
    {8, L"En el men\u00FA Inicio puede elegir una actividad del equipo, como leer nuevos mensajes de correo electr\u00F3nico o explorar la Web. Este men\u00FA est\u00E1 personalizado para cada persona que comparte este equipo."},
    {9, L"Una sugerencia de contrase\u00F1a puede ayudarle a recordar su contrase\u00F1a."},
    {10, L"Una sugerencia de contrase\u00F1a puede ayudar a esta persona a recordar su contrase\u00F1a."},
    {11, L"Windows no puede abrir el Panel de control Cuentas de usuario."},
    {12, L"La cuenta de usuario actual no se reconoce. Reinicie el equipo y abra Cuentas de usuario."},
    {13, L"Las contrase\u00F1as que escribi\u00F3 no coinciden. Vuelva a escribir la nueva contrase\u00F1a en ambos cuadros."},
    {14, L"La contrase\u00F1a que escribi\u00F3 es incorrecta. Vuelva a escribir su contrase\u00F1a actual."},
    {15, L"La contrase\u00F1a que escribi\u00F3 no cumple los requisitos de la directiva de contrase\u00F1as. Compruebe la longitud m\u00EDnima, la complejidad y el historial de contrase\u00F1as."},
    {16, L"Windows no puede cambiar la contrase\u00F1a."},
    {17, L"Windows no puede quitar la contrase\u00F1a. Las directivas de contrase\u00F1a o de cuenta exigen que la cuenta tenga contrase\u00F1a."},
    {18, L"El nombre de cuenta especificado no es v\u00E1lido, porque los nombres de cuenta no pueden contener los caracteres siguientes: /\\[]\":;|<>+=,?*\n\nEscriba un nombre diferente."},
    {19, L"No se puede usar el nombre de cuenta especificado porque es un nombre reservado. Escriba un nombre diferente."},
    {20, L"El nombre de usuario no puede ser igual que el nombre del equipo.\n\nEscriba un nombre diferente."},
    {21, L"El grupo especificado no existe."},
    {22, L"Ya existe una cuenta con el nombre '%s'. Escriba un nombre diferente."},
    {23, L"La imagen especificada es de un tipo desconocido o no es v\u00E1lida. Seleccione una imagen diferente."},
    {24, L"No hay ninguna c\u00E1mara ni esc\u00E1ner disponible. Aseg\u00FArese de que la c\u00E1mara est\u00E9 conectada y encendida e int\u00E9ntelo de nuevo."},
    {30, L"La cuenta que desea eliminar tiene la sesi\u00F3n iniciada. Antes de eliminarla, deber\u00EDa cambiar a esa cuenta de usuario y cerrar su sesi\u00F3n. Si contin\u00FAa sin hacerlo, existe riesgo de p\u00E9rdida de datos. \u00BFDesea continuar de todos modos?"},
    {31, L"La cuenta de invitado tiene la sesi\u00F3n iniciada. Para poder desactivar la cuenta de invitado, debe cambiar a ella y cerrar su sesi\u00F3n."},
    {34, L"Windows no puede eliminar la cuenta con la que se ha iniciado sesi\u00F3n actualmente."},
    {37, L"Protegida con contrase\u00F1a"},
    {38, L"Cuenta de invitado"},
    {39, L"Usuario est\u00E1ndar"},
    {40, L"Tipo de cuenta desconocido"},
    {41, L"Administrador"},
    {42, L"La cuenta de invitado est\u00E1 activada"},
    {43, L"La cuenta de invitado est\u00E1 desactivada"},
    {44, L"Windows no puede quitar la contrase\u00F1a."},
    {45, L"Cuentas de usuario"},
    {46, L"No se puede realizar esta acci\u00F3n debido a una restricci\u00F3n de la cuenta. P\u00F3ngase en contacto con el administrador."},
    {48, L"Crear cuenta nueva"},
    {49, L"Cambiar una cuenta"},
    {50, L"Elegir imagen"},
    {51, L"Cambiar nombre de la cuenta"},
    {52, L"Activar la cuenta de invitado"},
    {53, L"Cambiar opciones de invitado"},
    {54, L"Administrar cuentas"},
    {55, L"Cambiar su nombre"},
    {56, L"Eliminar cuenta"},
    {57, L"Confirmar eliminaci\u00F3n"},
    {58, L"Cambiar contrase\u00F1a"},
    {59, L"Cambiar tipo de cuenta"},
    {60, L"Cambiar su contrase\u00F1a"},
    {61, L"Cambiar su imagen"},
    {62, L"Crear contrase\u00F1a"},
    {63, L"Crear su contrase\u00F1a"},
    {64, L"Quitar contrase\u00F1a"},
    {65, L"Quitar su contrase\u00F1a"},
    {66, L"Cambiar su tipo de cuenta"},
    {67, L"Windows no pudo iniciar el cuadro de di\u00E1logo de opciones avanzadas."},
    {68, L"El nombre de cuenta especificado no es v\u00E1lido porque ya existe una cuenta con ese nombre.\n\nEscriba un nombre diferente."},
    {69, L"Imagen de la cuenta de usuario"},
    {70, L"Icono de la cuenta de usuario"},
    {71, L"Panel de control Cuentas de usuario"},
    {72, L"Se requieren privilegios de administrador"},
    {73, L"Contrase\u00F1a nueva"},
    {74, L"Confirmar contrase\u00F1a nueva"},
    {75, L"Escriba una sugerencia de contrase\u00F1a"},
    {76, L"Contrase\u00F1a actual"},
    {78, L"Nombre de cuenta nuevo"},
    {79, L"Ayuda"},
    {80, L"Archivos de imagen (*.bmp,*.gif,*.jpg,*.png)|*.bmp;*.dib;*.gif;*.jpg;*.jpe;*.jpeg;*.png;*.rle|Mapa de bits (*.bmp,*.dib,*.rle)|*.bmp;*.dib;*.rle|GIF (*.gif)|*.gif|JPEG (*.jpg)|*.jpg|PNG (*.png)|*.png|Todos los archivos (*.*)|*"},
    {91, L"Una directiva de grupo bloquea este programa. Para obtener m\u00E1s informaci\u00F3n, p\u00F3ngase en contacto con el administrador del sistema."},
    {92, L"Panel de tareas relacionadas de Cuentas de usuario"},
    {93, L"Panel de tareas principal de Cuentas de usuario"},
    {94, L"Lista de usuarios"},
    {95, L"Panel de control de cuentas de usuario"},
    {100, L"Crear un disco de restablecimiento de contrase\u00F1a"},
    {101, L"Vincular id. en l\u00EDnea"},
    {102, L"Administrar los certificados de cifrado de archivos"},
    {103, L"Configurar las propiedades avanzadas del perfil de usuario"},
    {104, L"Cambiar mis variables de entorno"},
    {105, L"Control parental"},
    {106, L"Administrar sus credenciales"},
    {107, L"Administrar los datos de sus huellas digitales"},
    {1100, L"Usuario &est\u00E1ndar"},
    {1101, L"&Administrador"},
    {1108, L"Realizar cambios en su cuenta de usuario"},
    {1109, L"Cambiar el nombre de su cuenta"},
    {1110, L"Crear una contrase\u00F1a para su cuenta"},
    {1111, L"Cambiar su contrase\u00F1a"},
    {1112, L"Quitar su contrase\u00F1a"},
    {1113, L"Cambiar su imagen"},
    {1114, L"Cambiar su tipo de cuenta"},
    {1115, L"Administrar otra cuenta"},
    {1116, L"Administrar cuentas de usuario"},
    {1117, L"Para cambiar su contrase\u00F1a, presione Ctrl+Alt+Supr y seleccione Cambiar una contrase\u00F1a."},
    {1119, L"Realizar cambios en la cuenta de %username%"},
    {1120, L"Cambiar nombre"},
    {1121, L"Crear contrase\u00F1a"},
    {1122, L"Cambiar contrase\u00F1a"},
    {1123, L"Quitar contrase\u00F1a"},
    {1124, L"Cambiar la imagen"},
    {1128, L"Crear una cuenta nueva"},
    {1129, L"\u00BFQu\u00E9 desea cambiar de la cuenta de invitado?"},
    {1130, L"Desactivar la cuenta de invitado"},
    {1131, L"Asigne un nombre a la cuenta y elija un tipo de cuenta"},
    {1133, L"Este nombre aparecer\u00E1 en la pantalla de bienvenida y en el men\u00FA Inicio."},
    {1136, L"Cancelar"},
    {1137, L"Crear cuenta"},
    {1138, L"Elija una imagen nueva para la cuenta de %username%"},
    {1139, L"La imagen que elija aparecer\u00E1 en la pantalla de bienvenida y en el men\u00FA Inicio."},
    {1140, L"Buscar m\u00E1s im\u00E1genes..."},
    {1141, L"Cambiar imagen"},
    {1142, L"Escriba un nombre nuevo para la cuenta de %username%"},
    {1145, L"\u00BFDesea activar la cuenta de invitado?"},
    {1146, L"Si activa la cuenta de invitado, las personas que no tengan una cuenta podr\u00E1n usarla para iniciar sesi\u00F3n en el equipo. Los usuarios invitados no pueden tener acceso a los archivos, carpetas ni configuraciones protegidos con contrase\u00F1a."},
    {1147, L"Activar"},
    {1148, L"Elija la cuenta que desea cambiar"},
    {1149, L"\u00BFQu\u00E9 es una cuenta de usuario?"},
    {1150, L"Otras tareas que puede realizar"},
    {1151, L"Configurar el Control parental"},
    {1155, L"Escriba un nombre de cuenta nuevo"},
    {1157, L"\u00BFDesea conservar los archivos de %username%?"},
    {1158, L"Antes de eliminar la cuenta de %username%, Windows puede guardar autom\u00E1ticamente el contenido del escritorio y de las carpetas Documentos, Favoritos, M\u00FAsica, Im\u00E1genes y V\u00EDdeos de %username% en una carpeta nueva llamada '%username%' en el escritorio. Sin embargo, Windows no puede guardar los mensajes de correo electr\u00F3nico ni otras configuraciones de %username%."},
    {1159, L"Eliminar archivos"},
    {1160, L"Conservar archivos"},
    {1161, L"\u00BFEst\u00E1 seguro de que desea eliminar la cuenta de %username%?"},
    {1162, L"Va a eliminar la cuenta, pero conservar\u00E1 los archivos."},
    {1163, L"%username% ya no podr\u00E1 iniciar sesi\u00F3n y se eliminar\u00E1 toda la configuraci\u00F3n de %username%. Sin embargo, los archivos de %username% se guardar\u00E1n en el escritorio en una carpeta llamada '%username%'."},
    {1164, L"Eliminar cuenta"},
    {1165, L"Cambiar la contrase\u00F1a de %username%"},
    {1166, L"Va a restablecer la contrase\u00F1a de %username%. Si lo hace, %username% perder\u00E1 todos los certificados personales y las contrase\u00F1as almacenadas de sitios Web o recursos de red."},
    {1167, L"Va a cambiar la contrase\u00F1a de %username%. Si lo hace, %username% perder\u00E1 todos los archivos cifrados con EFS, los certificados personales y las contrase\u00F1as almacenadas de sitios Web o recursos de red."},
    {1168, L"Si %username% ha olvidado la contrase\u00F1a, puede usar el disquete de restablecimiento de contrase\u00F1a."},
    {1169, L"Para evitar perder datos en el futuro, pida a %username% que cree un disquete de restablecimiento de contrase\u00F1a."},
    {1172, L"Si la contrase\u00F1a contiene letras may\u00FAsculas, deber\u00E1 escribirlas de la misma forma cada vez que inicie sesi\u00F3n."},
    {1174, L"La sugerencia de contrase\u00F1a ser\u00E1 visible para todas las personas que usen este equipo."},
    {1175, L"Cambiar tipo de cuenta"},
    {1176, L"Cambiar su contrase\u00F1a"},
    {1181, L"Elija una imagen nueva para su cuenta"},
    {1182, L"Crear una contrase\u00F1a para la cuenta de %username%"},
    {1183, L"Va a crear una contrase\u00F1a para %username%."},
    {1184, L"Si lo hace, %username% perder\u00E1 todos los archivos cifrados con EFS, los certificados personales y las contrase\u00F1as almacenadas de sitios Web o recursos de red."},
    {1185, L"Para evitar perder datos en el futuro, pida a %username% que cree un disquete de restablecimiento de contrase\u00F1a."},
    {1186, L"Crear una contrase\u00F1a para su cuenta"},
    {1188, L"Va a quitar la contrase\u00F1a de %username%. Si lo hace, %username% perder\u00E1 todos los certificados personales y las contrase\u00F1as almacenadas de sitios Web o recursos de red."},
    {1189, L"Va a quitar la contrase\u00F1a de %username%. Si lo hace, %username% perder\u00E1 todos los archivos cifrados con EFS, los certificados personales y las contrase\u00F1as almacenadas de sitios Web o recursos de red."},
    {1190, L"Si %username% ha olvidado la contrase\u00F1a, puede usar el disquete de restablecimiento de contrase\u00F1a."},
    {1191, L"Quitar su contrase\u00F1a"},
    {1192, L"\u00BFEst\u00E1 seguro de que desea quitar su contrase\u00F1a?"},
    {1193, L"Si quita su contrase\u00F1a, otras personas podr\u00E1n obtener acceso a su cuenta y cambiar la configuraci\u00F3n."},
    {1194, L"Si quita su contrase\u00F1a, otras personas podr\u00E1n obtener acceso a su cuenta y cambiar la configuraci\u00F3n."},
    {1195, L"Si ha olvidado su contrase\u00F1a, puede usar el disquete de restablecimiento de contrase\u00F1a."},
    {1196, L"Quitar una contrase\u00F1a"},
    {1197, L"Elija un tipo de cuenta nuevo para %username%"},
    {1198, L"Los usuarios de cuentas est\u00E1ndar pueden usar la mayor\u00EDa del software y cambiar la configuraci\u00F3n del sistema que no afecte a otros usuarios ni a la seguridad del equipo."},
    {1199, L"Los administradores tienen acceso completo al equipo y pueden realizar cualquier cambio que deseen. Seg\u00FAn la configuraci\u00F3n de notificaci\u00F3n, se puede pedir a los administradores que proporcionen su contrase\u00F1a o una confirmaci\u00F3n antes de realizar cambios que afecten a otros usuarios.\n\nLe recomendamos que proteja todas las cuentas con una contrase\u00F1a segura."},
    {1200, L"Seleccione su nuevo tipo de cuenta"},
    {1201, L"Ir a la p\u00E1gina principal de Cuentas de usuario"},
    {1203, L"Debe asignar una cuenta de administrador a otro usuario de este equipo antes de poder cambiar el tipo de cuenta de este usuario. As\u00ED se garantiza que siempre haya al menos un usuario con una cuenta de administrador en este equipo."},
    {1204, L"\u00BFQu\u00E9 es una sugerencia de contrase\u00F1a?"},
    {1205, L"C\u00F3mo crear una contrase\u00F1a segura"},
    {1206, L"\u00BFPor qu\u00E9 se recomienda una cuenta est\u00E1ndar?"},
    {1207, L"Windows eliminar\u00E1 todos los archivos de %username% y, a continuaci\u00F3n, eliminar\u00E1 la cuenta de %username%."},
    {1213, L"Cambiar la configuraci\u00F3n del Control de cuentas de usuario"},
    {1216, L"Cambiar el nombre de la cuenta"},
    {1217, L"Crear una contrase\u00F1a"},
    {1218, L"Cambiar la contrase\u00F1a"},
    {1219, L"Quitar la contrase\u00F1a"},
    {1220, L"Cambiar la imagen"},
    {1221, L"Cambiar el tipo de cuenta"},
    {1222, L"Eliminar la cuenta"},
    {1223, L"Administrar otra cuenta"},
    {1226, L"Si la contrase\u00F1a contiene letras may\u00FAsculas, deber\u00E1 escribirlas siempre de la misma forma."},
    {2000, L"Se usa para cambiar el \u00E1rea de visualizaci\u00F3n vertical"},
    {2001, L"Se usa para cambiar el \u00E1rea de visualizaci\u00F3n horizontal"},
    {2002, L"Horizontal"},
    {2003, L"Vertical"},
};
static const StringEntry kStringsPt[] = {
    {1, L"Contas de Utilizador"},
    {2, L"Altere as defini\u00E7\u00F5es das contas de utilizador e as palavras-passe das pessoas que partilham este computador."},
    {3, L"Clique aqui para escrever"},
    {4, L"Altere as informa\u00E7\u00F5es da conta desta pessoa, como o tipo de conta, o nome, a palavra-passe ou a imagem, ou elimine esta conta."},
    {5, L"Altere a imagem da conta de convidado ou impe\u00E7a o acesso de convidados a este computador."},
    {6, L"Forne\u00E7a acesso ao computador a pessoas sem uma conta de utilizador neste computador."},
    {7, L"O ecr\u00E3 de boas-vindas mostra os nomes de todas as pessoas com uma conta de utilizador neste computador. Este ecr\u00E3 aparece quando inicia o computador ou muda de utilizador."},
    {8, L"No menu Iniciar, pode escolher uma atividade do computador, como ler novas mensagens de correio eletr\u00F3nico ou navegar na Web. Este menu \u00E9 personalizado para cada pessoa que partilha este computador."},
    {9, L"Uma sugest\u00E3o de palavra-passe pode ajud\u00E1-lo a lembrar-se da sua palavra-passe."},
    {10, L"Uma sugest\u00E3o de palavra-passe pode ajudar esta pessoa a lembrar-se da sua palavra-passe."},
    {11, L"O Windows n\u00E3o consegue abrir o painel de controlo Contas de Utilizador."},
    {12, L"A conta de utilizador atual n\u00E3o \u00E9 reconhecida. Reinicie o computador e abra novamente Contas de Utilizador."},
    {13, L"As palavras-passe que escreveu n\u00E3o coincidem. Volte a escrever a nova palavra-passe em ambas as caixas."},
    {14, L"A palavra-passe que escreveu est\u00E1 incorreta. Volte a escrever a sua palavra-passe atual."},
    {15, L"A palavra-passe que escreveu n\u00E3o cumpre os requisitos da pol\u00EDtica de palavras-passe. Verifique o comprimento m\u00EDnimo, a complexidade e o hist\u00F3rico das palavras-passe."},
    {16, L"O Windows n\u00E3o consegue alterar a palavra-passe."},
    {17, L"O Windows n\u00E3o consegue remover a palavra-passe. As pol\u00EDticas de palavras-passe ou de contas exigem que a conta tenha uma palavra-passe."},
    {18, L"O nome de conta especificado n\u00E3o \u00E9 v\u00E1lido, porque os nomes de conta n\u00E3o podem conter os seguintes carateres: /\\[]\":;|<>+=,?*\n\nEscreva um nome diferente."},
    {19, L"N\u00E3o \u00E9 poss\u00EDvel utilizar o nome de conta especificado, porque \u00E9 um nome reservado. Escreva um nome diferente."},
    {20, L"O nome de utilizador n\u00E3o pode ser igual ao nome do computador.\n\nEscreva um nome diferente."},
    {21, L"O grupo especificado n\u00E3o existe."},
    {22, L"J\u00E1 existe uma conta com o nome '%s'. Escreva um nome diferente."},
    {23, L"A imagem especificada \u00E9 de um tipo desconhecido ou n\u00E3o \u00E9 v\u00E1lida. Selecione uma imagem diferente."},
    {24, L"N\u00E3o est\u00E1 dispon\u00EDvel nenhuma c\u00E2mara ou scanner. Certifique-se de que a c\u00E2mara est\u00E1 ligada e ativada e tente novamente."},
    {30, L"A conta que pretende eliminar tem sess\u00E3o iniciada. Antes de a eliminar, deve mudar para essa conta de utilizador e terminar a respetiva sess\u00E3o. Se continuar sem o fazer, existe o risco de perda de dados. Pretende continuar mesmo assim?"},
    {31, L"A conta de convidado tem sess\u00E3o iniciada. Antes de desativar a conta de convidado, tem de mudar para a conta de convidado e terminar a respetiva sess\u00E3o."},
    {34, L"O Windows n\u00E3o consegue eliminar a conta com sess\u00E3o iniciada atualmente."},
    {37, L"Protegida por palavra-passe"},
    {38, L"Conta de convidado"},
    {39, L"Utilizador padr\u00E3o"},
    {40, L"Tipo de conta desconhecido"},
    {41, L"Administrador"},
    {42, L"A conta de convidado est\u00E1 ativada"},
    {43, L"A conta de convidado est\u00E1 desativada"},
    {44, L"O Windows n\u00E3o consegue remover a palavra-passe."},
    {45, L"Contas de Utilizador"},
    {46, L"N\u00E3o \u00E9 poss\u00EDvel efetuar esta a\u00E7\u00E3o devido a uma restri\u00E7\u00E3o da conta. Contacte o administrador."},
    {48, L"Criar Nova Conta"},
    {49, L"Alterar uma Conta"},
    {50, L"Escolher Imagem"},
    {51, L"Mudar o Nome da Conta"},
    {52, L"Ativar a Conta de Convidado"},
    {53, L"Alterar Op\u00E7\u00F5es de Convidado"},
    {54, L"Gerir Contas"},
    {55, L"Alterar o Seu Nome"},
    {56, L"Eliminar Conta"},
    {57, L"Confirmar Elimina\u00E7\u00E3o"},
    {58, L"Alterar Palavra-passe"},
    {59, L"Alterar Tipo de Conta"},
    {60, L"Alterar a Sua Palavra-passe"},
    {61, L"Alterar a Sua Imagem"},
    {62, L"Criar Palavra-passe"},
    {63, L"Criar a Sua Palavra-passe"},
    {64, L"Remover Palavra-passe"},
    {65, L"Remover a Sua Palavra-passe"},
    {66, L"Alterar o Seu Tipo de Conta"},
    {67, L"O Windows n\u00E3o conseguiu iniciar a caixa de di\u00E1logo de op\u00E7\u00F5es avan\u00E7adas."},
    {68, L"O nome de conta especificado n\u00E3o \u00E9 v\u00E1lido, porque j\u00E1 existe uma conta com esse nome.\n\nEscreva um nome diferente."},
    {69, L"Imagem da conta de utilizador"},
    {70, L"Mosaico da conta de utilizador"},
    {71, L"Painel de Controlo Contas de Utilizador"},
    {72, L"S\u00E3o necess\u00E1rios privil\u00E9gios de administrador"},
    {73, L"Nova palavra-passe"},
    {74, L"Confirmar nova palavra-passe"},
    {75, L"Escreva uma sugest\u00E3o de palavra-passe"},
    {76, L"Palavra-passe atual"},
    {78, L"Novo nome de conta"},
    {79, L"Ajuda"},
    {80, L"Ficheiros de imagem (*.bmp,*.gif,*.jpg,*.png)|*.bmp;*.dib;*.gif;*.jpg;*.jpe;*.jpeg;*.png;*.rle|Bitmap (*.bmp,*.dib,*.rle)|*.bmp;*.dib;*.rle|GIF (*.gif)|*.gif|JPEG (*.jpg)|*.jpg|PNG (*.png)|*.png|Todos os Ficheiros (*.*)|*"},
    {91, L"Este programa est\u00E1 bloqueado por uma pol\u00EDtica de grupo. Para obter mais informa\u00E7\u00F5es, contacte o administrador do sistema."},
    {92, L"Painel de Tarefas Relacionadas de Contas de Utilizador"},
    {93, L"Painel de Tarefas Principal de Contas de Utilizador"},
    {94, L"Lista de utilizadores"},
    {95, L"Painel de Controlo de Contas de Utilizador"},
    {100, L"Criar um disco de reposi\u00E7\u00E3o de palavra-passe"},
    {101, L"Ligar IDs online"},
    {102, L"Gerir os certificados de encripta\u00E7\u00E3o de ficheiros"},
    {103, L"Configurar as propriedades avan\u00E7adas do perfil de utilizador"},
    {104, L"Alterar as minhas vari\u00E1veis de ambiente"},
    {105, L"Controlo Parental"},
    {106, L"Gerir as suas credenciais"},
    {107, L"Gerir os dados das suas impress\u00F5es digitais"},
    {1100, L"Utilizador &padr\u00E3o"},
    {1101, L"&Administrador"},
    {1108, L"Efetuar altera\u00E7\u00F5es \u00E0 sua conta de utilizador"},
    {1109, L"Alterar o nome da sua conta"},
    {1110, L"Criar uma palavra-passe para a sua conta"},
    {1111, L"Alterar a sua palavra-passe"},
    {1112, L"Remover a sua palavra-passe"},
    {1113, L"Alterar a sua imagem"},
    {1114, L"Alterar o seu tipo de conta"},
    {1115, L"Gerir outra conta"},
    {1116, L"Gerir Contas de Utilizador"},
    {1117, L"Para alterar a sua palavra-passe, prima Ctrl+Alt+Del e selecione Alterar uma palavra-passe."},
    {1119, L"Efetuar altera\u00E7\u00F5es \u00E0 conta de %username%"},
    {1120, L"Alterar Nome"},
    {1121, L"Criar palavra-passe"},
    {1122, L"Alterar palavra-passe"},
    {1123, L"Remover Palavra-passe"},
    {1124, L"Alterar a imagem"},
    {1128, L"Criar uma nova conta"},
    {1129, L"O que pretende alterar na conta de convidado?"},
    {1130, L"Desativar a conta de convidado"},
    {1131, L"Atribua um nome \u00E0 conta e escolha um tipo de conta"},
    {1133, L"Este nome aparecer\u00E1 no ecr\u00E3 de boas-vindas e no menu Iniciar."},
    {1136, L"Cancelar"},
    {1137, L"Criar Conta"},
    {1138, L"Escolha uma nova imagem para a conta de %username%"},
    {1139, L"A imagem que escolher aparecer\u00E1 no ecr\u00E3 de boas-vindas e no menu Iniciar."},
    {1140, L"Procurar mais imagens..."},
    {1141, L"Alterar Imagem"},
    {1142, L"Escreva um novo nome para a conta de %username%"},
    {1145, L"Pretende ativar a conta de convidado?"},
    {1146, L"Se ativar a conta de convidado, as pessoas que n\u00E3o t\u00EAm uma conta podem utiliz\u00E1-la para iniciar sess\u00E3o no computador. Os ficheiros, pastas ou defini\u00E7\u00F5es protegidos por palavra-passe n\u00E3o est\u00E3o acess\u00EDveis aos utilizadores convidados."},
    {1147, L"Ativar"},
    {1148, L"Escolha a conta que pretende alterar"},
    {1149, L"O que \u00E9 uma conta de utilizador?"},
    {1150, L"Outras tarefas que pode efetuar"},
    {1151, L"Configurar o Controlo Parental"},
    {1155, L"Escreva um novo nome de conta"},
    {1157, L"Pretende manter os ficheiros de %username%?"},
    {1158, L"Antes de eliminar a conta de %username%, o Windows pode guardar automaticamente o conte\u00FAdo do ambiente de trabalho e das pastas Documentos, Favoritos, M\u00FAsica, Imagens e V\u00EDdeos de %username% numa nova pasta com o nome '%username%' no seu ambiente de trabalho. No entanto, o Windows n\u00E3o pode guardar as mensagens de correio eletr\u00F3nico e outras defini\u00E7\u00F5es de %username%."},
    {1159, L"Eliminar Ficheiros"},
    {1160, L"Manter Ficheiros"},
    {1161, L"Tem a certeza de que pretende eliminar a conta de %username%?"},
    {1162, L"Est\u00E1 a eliminar a conta, mas a manter os ficheiros."},
    {1163, L"%username% deixar\u00E1 de poder iniciar sess\u00E3o e todas as defini\u00E7\u00F5es de %username% ser\u00E3o eliminadas. No entanto, os ficheiros de %username% ser\u00E3o guardados no seu ambiente de trabalho numa pasta com o nome '%username%'."},
    {1164, L"Eliminar Conta"},
    {1165, L"Alterar a palavra-passe de %username%"},
    {1166, L"Est\u00E1 a repor a palavra-passe de %username%. Se o fizer, %username% perder\u00E1 todos os certificados pessoais e palavras-passe armazenadas para Web sites ou recursos de rede."},
    {1167, L"Est\u00E1 a alterar a palavra-passe de %username%. Se o fizer, %username% perder\u00E1 todos os ficheiros encriptados com EFS, certificados pessoais e palavras-passe armazenadas para Web sites ou recursos de rede."},
    {1168, L"Se %username% se esqueceu da palavra-passe, pode utilizar a disquete de reposi\u00E7\u00E3o de palavra-passe."},
    {1169, L"Para evitar a perda de dados no futuro, pe\u00E7a a %username% que crie uma disquete de reposi\u00E7\u00E3o de palavra-passe."},
    {1172, L"Se a sua palavra-passe contiver letras mai\u00FAsculas, estas t\u00EAm de ser escritas da mesma forma sempre que iniciar sess\u00E3o."},
    {1174, L"A sugest\u00E3o de palavra-passe ser\u00E1 vis\u00EDvel para todas as pessoas que utilizem este computador."},
    {1175, L"Alterar Tipo de Conta"},
    {1176, L"Alterar a sua palavra-passe"},
    {1181, L"Escolha uma nova imagem para a sua conta"},
    {1182, L"Criar uma palavra-passe para a conta de %username%"},
    {1183, L"Est\u00E1 a criar uma palavra-passe para %username%."},
    {1184, L"Se o fizer, %username% perder\u00E1 todos os ficheiros encriptados com EFS, certificados pessoais e palavras-passe armazenadas para Web sites ou recursos de rede."},
    {1185, L"Para evitar a perda de dados no futuro, pe\u00E7a a %username% que crie uma disquete de reposi\u00E7\u00E3o de palavra-passe."},
    {1186, L"Criar uma palavra-passe para a sua conta"},
    {1188, L"Est\u00E1 a remover a palavra-passe de %username%. Se o fizer, %username% perder\u00E1 todos os certificados pessoais e palavras-passe armazenadas para Web sites ou recursos de rede."},
    {1189, L"Est\u00E1 a remover a palavra-passe de %username%. Se o fizer, %username% perder\u00E1 todos os ficheiros encriptados com EFS, certificados pessoais e palavras-passe armazenadas para Web sites ou recursos de rede."},
    {1190, L"Se %username% se esqueceu da palavra-passe, pode utilizar a disquete de reposi\u00E7\u00E3o de palavra-passe."},
    {1191, L"Remover a sua palavra-passe"},
    {1192, L"Tem a certeza de que pretende remover a sua palavra-passe?"},
    {1193, L"Se remover a sua palavra-passe, outras pessoas poder\u00E3o aceder \u00E0 sua conta e alterar defini\u00E7\u00F5es."},
    {1194, L"Se remover a sua palavra-passe, outras pessoas poder\u00E3o aceder \u00E0 sua conta e alterar defini\u00E7\u00F5es."},
    {1195, L"Se se esqueceu da sua palavra-passe, pode utilizar a disquete de reposi\u00E7\u00E3o de palavra-passe."},
    {1196, L"Remover uma palavra-passe"},
    {1197, L"Escolha um novo tipo de conta para %username%"},
    {1198, L"Os utilizadores de contas padr\u00E3o podem utilizar a maioria do software e alterar as defini\u00E7\u00F5es do sistema que n\u00E3o afetem outros utilizadores nem a seguran\u00E7a do computador."},
    {1199, L"Os administradores t\u00EAm acesso total ao computador e podem efetuar qualquer altera\u00E7\u00E3o pretendida. Consoante as defini\u00E7\u00F5es de notifica\u00E7\u00E3o, pode ser pedido aos administradores que forne\u00E7am a sua palavra-passe ou uma confirma\u00E7\u00E3o antes de efetuarem altera\u00E7\u00F5es que afetem outros utilizadores.\n\nRecomendamos que proteja todas as contas com uma palavra-passe forte."},
    {1200, L"Selecione o seu novo tipo de conta"},
    {1201, L"Ir para a p\u00E1gina principal de Contas de Utilizador"},
    {1203, L"Tem de atribuir uma conta de administrador a outro utilizador deste computador antes de poder alterar o tipo de conta deste utilizador. Isto garante que existe sempre pelo menos um utilizador com uma conta de administrador neste computador."},
    {1204, L"O que \u00E9 uma sugest\u00E3o de palavra-passe?"},
    {1205, L"Como criar uma palavra-passe forte"},
    {1206, L"Porque \u00E9 recomendada uma conta padr\u00E3o?"},
    {1207, L"O Windows eliminar\u00E1 todos os ficheiros de %username% e, em seguida, eliminar\u00E1 a conta de %username%."},
    {1213, L"Alterar as defini\u00E7\u00F5es do Controlo de Conta de Utilizador"},
    {1216, L"Alterar o nome da conta"},
    {1217, L"Criar uma palavra-passe"},
    {1218, L"Alterar a palavra-passe"},
    {1219, L"Remover a palavra-passe"},
    {1220, L"Alterar a imagem"},
    {1221, L"Alterar o tipo de conta"},
    {1222, L"Eliminar a conta"},
    {1223, L"Gerir outra conta"},
    {1226, L"Se a palavra-passe contiver letras mai\u00FAsculas, estas t\u00EAm de ser escritas sempre da mesma forma."},
    {2000, L"Utilizado para alterar a \u00E1rea de visualiza\u00E7\u00E3o vertical"},
    {2001, L"Utilizado para alterar a \u00E1rea de visualiza\u00E7\u00E3o horizontal"},
    {2002, L"Horizontal"},
    {2003, L"Vertical"},
};
static const StringEntry kStringsDe[] = {
    {1, L"Benutzerkonten"},
    {2, L"Ändern Sie die Einstellungen und Kennwörter der Benutzerkonten für Personen, die diesen Computer gemeinsam nutzen."},
    {3, L"Hier klicken, um Text einzugeben"},
    {4, L"Ändern Sie die Kontoinformationen dieser Person, z. B. den Kontotyp, den Namen, das Kennwort oder das Bild, oder löschen Sie dieses Konto."},
    {5, L"Ändern Sie das Bild des Gastkontos oder verhindern Sie den Gastzugriff auf diesen Computer."},
    {6, L"Ermöglichen Sie Personen ohne Benutzerkonto den Zugriff auf diesen Computer."},
    {7, L"Auf dem Willkommensbildschirm werden die Namen aller Benutzer mit einem Konto auf diesem Computer angezeigt. Dieser Bildschirm erscheint beim Starten des Computers oder beim Wechseln des Benutzers."},
    {8, L"Im Startmenü können Sie eine Computeraktivität auswählen, z. B. neue E-Mail-Nachrichten lesen oder im Internet surfen. Dieses Menü wird für jeden Benutzer personalisiert, der diesen Computer gemeinsam nutzt."},
    {9, L"Ein Kennworthinweis kann Ihnen helfen, sich an Ihr Kennwort zu erinnern."},
    {10, L"Ein Kennworthinweis kann dieser Person helfen, sich an ihr Kennwort zu erinnern."},
    {11, L"Windows kann die Systemsteuerung für Benutzerkonten nicht öffnen."},
    {12, L"Das aktuelle Benutzerkonto wurde nicht erkannt. Starten Sie den Computer neu, und öffnen Sie anschließend die Benutzerkonten."},
    {13, L"Die eingegebenen Kennwörter stimmen nicht überein. Geben Sie das neue Kennwort bitte in beiden Feldern erneut ein."},
    {14, L"Das eingegebene Kennwort ist falsch. Geben Sie Ihr aktuelles Kennwort bitte erneut ein."},
    {15, L"Das eingegebene Kennwort entspricht nicht den Anforderungen der Kennwortrichtlinie. Überprüfen Sie die Anforderungen an die Mindestlänge, Komplexität und den Kennwortverlauf."},
    {16, L"Windows kann das Kennwort nicht ändern."},
    {17, L"Windows kann das Kennwort nicht entfernen. Die Kennwort- und/oder Kontorichtlinien erfordern, dass das Konto über ein Kennwort verfügt."},
    {18, L"Der angegebene Kontoname ist ungültig, da Kontonamen folgende Zeichen nicht enthalten dürfen: /\\[]\":;|<>+=,?*\n\nGeben Sie bitte einen anderen Namen ein."},
    {19, L"Der angegebene Kontoname kann nicht verwendet werden, da es sich um einen reservierten Namen handelt. Geben Sie bitte einen anderen Namen ein."},
    {20, L"Der Benutzername darf nicht mit dem Computernamen identisch sein.\n\nGeben Sie bitte einen anderen Namen ein."},
    {21, L"Die angegebene Gruppe ist nicht vorhanden."},
    {22, L"Ein Konto mit dem Namen '%s' ist bereits vorhanden. Geben Sie einen anderen Namen ein."},
    {23, L"Das angegebene Bild hat einen unbekannten Typ oder ist ungültig. Wählen Sie ein anderes Bild aus."},
    {24, L"Es ist keine Kamera und kein Scanner verfügbar. Stellen Sie sicher, dass die Kamera angeschlossen und eingeschaltet ist, und versuchen Sie es erneut."},
    {30, L"Der Benutzer, dessen Konto Sie löschen möchten, ist angemeldet. Bevor Sie das Konto löschen, sollten Sie zu diesem Benutzerkonto wechseln und sich dort abmelden. Wenn Sie ohne diese Schritte fortfahren, besteht die Gefahr eines Datenverlusts. Möchten Sie trotzdem fortfahren?"},
    {31, L"Das Gastkonto ist angemeldet. Bevor Sie das Gastkonto deaktivieren können, müssen Sie zum Gastkonto wechseln und sich dort abmelden."},
    {34, L"Windows kann das aktuell angemeldete Benutzerkonto nicht löschen."},
    {37, L"Kennwortgeschützt"},
    {38, L"Gastkonto"},
    {39, L"Standardbenutzer"},
    {40, L"Unbekannter Kontotyp"},
    {41, L"Administrator"},
    {42, L"Das Gastkonto ist aktiviert"},
    {43, L"Das Gastkonto ist deaktiviert"},
    {44, L"Windows kann das Kennwort nicht entfernen."},
    {45, L"Benutzerkonten"},
    {46, L"Diese Aktion kann aufgrund einer Kontoeinschränkung nicht ausgeführt werden. Wenden Sie sich an Ihren Administrator."},
    {48, L"Neues Konto erstellen"},
    {49, L"Konto ändern"},
    {50, L"Bild auswählen"},
    {51, L"Konto umbenennen"},
    {52, L"Gastkonto aktivieren"},
    {53, L"Gastoptionen ändern"},
    {54, L"Konten verwalten"},
    {55, L"Ihren Namen ändern"},
    {56, L"Konto löschen"},
    {57, L"Löschen bestätigen"},
    {58, L"Kennwort ändern"},
    {59, L"Kontotyp ändern"},
    {60, L"Ihr Kennwort ändern"},
    {61, L"Ihr Bild ändern"},
    {62, L"Kennwort erstellen"},
    {63, L"Ihr Kennwort erstellen"},
    {64, L"Kennwort entfernen"},
    {65, L"Ihr Kennwort entfernen"},
    {66, L"Ihren Kontotyp ändern"},
    {67, L"Windows konnte das Dialogfeld für die erweiterten Optionen nicht starten."},
    {68, L"Der angegebene Kontoname ist ungültig, da bereits ein Konto mit diesem Namen vorhanden ist.\n\nGeben Sie bitte einen anderen Namen ein."},
    {69, L"Benutzerkontobild"},
    {70, L"Benutzerkontokachel"},
    {71, L"Systemsteuerung für Benutzerkonten"},
    {72, L"Administratorrechte erforderlich"},
    {73, L"Neues Kennwort"},
    {74, L"Neues Kennwort bestätigen"},
    {75, L"Geben Sie einen Kennworthinweis ein"},
    {76, L"Aktuelles Kennwort"},
    {78, L"Neuer Kontoname"},
    {79, L"Hilfe"},
    {80, L"Bilddateien (*.bmp,*.gif,*.jpg,*.png)|*.bmp;*.dib;*.gif;*.jpg;*.jpe;*.jpeg;*.png;*.rle|Bitmap (*.bmp,*.dib,*.rle)|*.bmp;*.dib;*.rle|GIF (*.gif)|*.gif|JPEG (*.jpg)|*.jpg|PNG (*.png)|*.png|Alle Dateien (*.*)|*"},
    {91, L"Dieses Programm wird durch eine Gruppenrichtlinie blockiert. Weitere Informationen erhalten Sie von Ihrem Systemadministrator."},
    {92, L"Aufgabenbereich für Benutzerkonten"},
    {93, L"Hauptaufgabenbereich für Benutzerkonten"},
    {94, L"Benutzerliste"},
    {95, L"Systemsteuerung für Benutzerkonten"},
    {100, L"Kennwortrücksetzdiskette erstellen"},
    {101, L"Online-IDs verknüpfen"},
    {102, L"Dateiverschlüsselungszertifikate verwalten"},
    {103, L"Erweiterte Eigenschaften des Benutzerprofils konfigurieren"},
    {104, L"Umgebungsvariablen ändern"},
    {105, L"Jugendschutz"},
    {106, L"Anmeldeinformationen verwalten"},
    {107, L"Fingerabdruckdaten verwalten"},
    {1100, L"&Standardbenutzer"},
    {1101, L"&Administrator"},
    {1108, L"Änderungen an Ihrem Benutzerkonto vornehmen"},
    {1109, L"Ihren Kontonamen ändern"},
    {1110, L"Ein Kennwort für Ihr Konto erstellen"},
    {1111, L"Ihr Kennwort ändern"},
    {1112, L"Ihr Kennwort entfernen"},
    {1113, L"Ihr Bild ändern"},
    {1114, L"Ihren Kontotyp ändern"},
    {1115, L"Ein anderes Konto verwalten"},
    {1116, L"Benutzerkonten verwalten"},
    {1117, L"Um Ihr Kennwort zu ändern, drücken Sie STRG+ALT+ENTF und wählen Sie Kennwort ändern."},
    {1119, L"Änderungen am Konto von %username% vornehmen"},
    {1120, L"Namen ändern"},
    {1121, L"Kennwort erstellen"},
    {1122, L"Kennwort ändern"},
    {1123, L"Kennwort entfernen"},
    {1124, L"Bild ändern"},
    {1128, L"Neues Konto erstellen"},
    {1129, L"Was möchten Sie am Gastkonto ändern?"},
    {1130, L"Gastkonto deaktivieren"},
    {1131, L"Geben Sie einen Kontonamen ein und wählen Sie einen Kontotyp aus"},
    {1133, L"Dieser Name wird auf dem Willkommensbildschirm und im Startmenü angezeigt."},
    {1136, L"Abbrechen"},
    {1137, L"Konto erstellen"},
    {1138, L"Wählen Sie ein neues Bild für das Konto von %username% aus"},
    {1139, L"Das ausgewählte Bild wird auf dem Willkommensbildschirm und im Startmenü angezeigt."},
    {1140, L"Weitere Bilder suchen..."},
    {1141, L"Bild ändern"},
    {1142, L"Geben Sie einen neuen Kontonamen für das Konto von %username% ein"},
    {1145, L"Möchten Sie das Gastkonto aktivieren?"},
    {1146, L"Wenn Sie das Gastkonto aktivieren, können Personen ohne eigenes Konto das Gastkonto verwenden, um sich an diesem Computer anzumelden. Kennwortgeschützte Dateien, Ordner oder Einstellungen sind für Gastbenutzer nicht zugänglich."},
    {1147, L"Aktivieren"},
    {1148, L"Wählen Sie das Konto aus, das Sie ändern möchten"},
    {1149, L"Was ist ein Benutzerkonto?"},
    {1150, L"Weitere mögliche Aufgaben"},
    {1151, L"Jugendschutz einrichten"},
    {1155, L"Geben Sie einen neuen Kontonamen ein"},
    {1157, L"Möchten Sie die Dateien von %username% behalten?"},
    {1158, L"Bevor Sie das Konto von %username% löschen, kann Windows den Inhalt der Ordner Desktop, Dokumente, Favoriten, Musik, Bilder und Videos von %username% automatisch in einem neuen Ordner namens '%username%' auf Ihrem Desktop speichern. Windows kann jedoch die E-Mail-Nachrichten und andere Einstellungen von %username% nicht speichern."},
    {1159, L"Dateien löschen"},
    {1160, L"Dateien behalten"},
    {1161, L"Möchten Sie das Konto von %username% wirklich löschen?"},
    {1162, L"Sie löschen das Konto, behalten aber die Dateien."},
    {1163, L"%username% kann sich anschließend nicht mehr anmelden und alle Einstellungen von %username% werden gelöscht. Die Dateien von %username% werden jedoch auf Ihrem Desktop in einem Ordner namens '%username%' gespeichert."},
    {1164, L"Konto löschen"},
    {1165, L"Kennwort von %username% ändern"},
    {1166, L"Sie setzen das Kennwort für %username% zurück. Wenn Sie dies tun, verliert %username% alle persönlichen Zertifikate und gespeicherten Kennwörter für Websites oder Netzwerkressourcen."},
    {1167, L"Sie ändern das Kennwort für %username%. Wenn Sie dies tun, verliert %username% alle EFS-verschlüsselten Dateien, persönlichen Zertifikate und gespeicherten Kennwörter für Websites oder Netzwerkressourcen."},
    {1168, L"Wenn %username% das Kennwort vergessen hat, kann die Person die Kennwortrücksetzdiskette verwenden."},
    {1169, L"Um zukünftigen Datenverlust zu vermeiden, sollten Sie %username% auffordern, eine Kennwortrücksetzdiskette zu erstellen."},
    {1172, L"Wenn Ihr Kennwort Großbuchstaben enthält, müssen Sie diese bei jeder Anmeldung genauso eingeben."},
    {1174, L"Der Kennworthinweis ist für alle Benutzer dieses Computers sichtbar."},
    {1175, L"Kontotyp ändern"},
    {1176, L"Ihr Kennwort ändern"},
    {1181, L"Wählen Sie ein neues Bild für Ihr Konto aus"},
    {1182, L"Kennwort für das Konto von %username% erstellen"},
    {1183, L"Sie erstellen ein Kennwort für %username%."},
    {1184, L"Wenn Sie dies tun, verliert %username% alle EFS-verschlüsselten Dateien, persönlichen Zertifikate und gespeicherten Kennwörter für Websites oder Netzwerkressourcen."},
    {1185, L"Um zukünftigen Datenverlust zu vermeiden, sollten Sie %username% auffordern, eine Kennwortrücksetzdiskette zu erstellen."},
    {1186, L"Kennwort für Ihr Konto erstellen"},
    {1188, L"Sie entfernen das Kennwort für %username%. Wenn Sie dies tun, verliert %username% alle persönlichen Zertifikate und gespeicherten Kennwörter für Websites oder Netzwerkressourcen."},
    {1189, L"Sie entfernen das Kennwort für %username%. Wenn Sie dies tun, verliert %username% alle EFS-verschlüsselten Dateien, persönlichen Zertifikate und gespeicherten Kennwörter für Websites oder Netzwerkressourcen."},
    {1190, L"Wenn %username% das Kennwort vergessen hat, kann die Person die Kennwortrücksetzdiskette verwenden."},
    {1191, L"Ihr Kennwort entfernen"},
    {1192, L"Möchten Sie Ihr Kennwort wirklich entfernen?"},
    {1193, L"Wenn Sie Ihr Kennwort entfernen, können andere Personen auf Ihr Konto zugreifen und Einstellungen ändern."},
    {1194, L"Wenn Sie Ihr Kennwort entfernen, können andere Personen auf Ihr Konto zugreifen und Einstellungen ändern."},
    {1195, L"Wenn Sie Ihr Kennwort vergessen haben, können Sie die Kennwortrücksetzdiskette verwenden."},
    {1196, L"Kennwort entfernen"},
    {1197, L"Wählen Sie einen neuen Kontotyp für %username% aus"},
    {1198, L"Benutzer mit einem Standardkonto können die meisten Programme verwenden und Systemeinstellungen ändern, die keine Auswirkungen auf andere Benutzer oder die Sicherheit des Computers haben."},
    {1199, L"Administratoren haben uneingeschränkten Zugriff auf den Computer und können alle gewünschten Änderungen vornehmen. Abhängig von den Benachrichtigungseinstellungen müssen Administratoren möglicherweise ihr Kennwort eingeben oder eine Bestätigung erteilen, bevor sie Änderungen vornehmen können, die andere Benutzer betreffen.\n\nWir empfehlen, jedes Konto mit einem sicheren Kennwort zu schützen."},
    {1200, L"Wählen Sie Ihren neuen Kontotyp aus"},
    {1201, L"Zur Hauptseite der Benutzerkonten wechseln"},
    {1203, L"Sie müssen einem anderen Benutzer dieses Computers ein Administratorkonto zuweisen, bevor Sie den Kontotyp dieses Benutzers ändern können. Dadurch wird sichergestellt, dass auf diesem Computer immer mindestens ein Benutzer über ein Administratorkonto verfügt."},
    {1204, L"Was ist ein Kennworthinweis?"},
    {1205, L"So erstellen Sie ein sicheres Kennwort"},
    {1206, L"Warum wird ein Standardkonto empfohlen?"},
    {1207, L"Windows löscht alle Dateien von %username% und anschließend das Konto von %username%."},
    {1213, L"Einstellungen der Benutzerkontensteuerung ändern"},
    {1216, L"Kontonamen ändern"},
    {1217, L"Kennwort erstellen"},
    {1218, L"Kennwort ändern"},
    {1219, L"Kennwort entfernen"},
    {1220, L"Bild ändern"},
    {1221, L"Kontotyp ändern"},
    {1222, L"Konto löschen"},
    {1223, L"Anderes Konto verwalten"},
    {1226, L"Wenn das Kennwort Großbuchstaben enthält, müssen diese bei jeder Anmeldung genauso eingegeben werden."},
    {2000, L"Zum Ändern des vertikalen Anzeigebereichs"},
    {2001, L"Zum Ändern des horizontalen Anzeigebereichs"},
    {2002, L"Horizontal"},
    {2003, L"Vertikal"},
};
// </strings>

enum class Language { English, Italian, French, Spanish, Portuguese, German };

struct StringTable {
    const StringEntry* entries;
    size_t count;
    const wchar_t* tag;       // part of the generated file's name
    const wchar_t* generic;   // generic user name shown when "hide user name" is on
    const wchar_t* find(uint32_t id) const {
        size_t lo = 0, hi = count;
        while (lo < hi) {
            size_t mid = (lo + hi) / 2;
            if (entries[mid].id < id) lo = mid + 1; else hi = mid;
        }
        return lo < count && entries[lo].id == id ? entries[lo].text : nullptr;
    }
};

StringTable TableFor(Language language) {
    if (language == Language::Italian) return {kStringsIt, ARRAYSIZE(kStringsIt), L"it-IT", L"Utente"};
    if (language == Language::French) return {kStringsFr, ARRAYSIZE(kStringsFr), L"fr-FR", L"Utilisateur"};
    if (language == Language::Spanish) return {kStringsEs, ARRAYSIZE(kStringsEs), L"es-ES", L"Usuario"};
    if (language == Language::Portuguese) return {kStringsPt, ARRAYSIZE(kStringsPt), L"pt-PT", L"Utilizador"};
    if (language == Language::German) return {kStringsDe, ARRAYSIZE(kStringsDe), L"de-DE", L"Benutzer"};
    return {kStringsEn, ARRAYSIZE(kStringsEn), L"en-US", L"User"};
}

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
// The page generator. Pure: bytes in, bytes out; the same inputs always give the same bytes (tools/genpage.py is the
// reference implementation, and the tests compare the two).
//
// Output = Microsoft's usercpl.dll 6.1.7600.16385 with
//   * the delay-load name DUI70.dll replaced by the private name of the Windows 7 DirectUI,
//   * a new resource directory in an added section: all original resources (their data stays where it is), plus the
//     strings (one language, stored language-neutral), plus BITMAP 1 = the picture frame from shacct.dll; without the
//     "MUI" resource (the strings are inside now),
//   * the layouts no longer asking shacct.dll for the frame,
//   * the "common" style sheet inside every layout. Every layout says sheet="Common"; the DirectUI parser takes that
//     sheet from UIFILE 1010 of the system's DUser.dll, which since Windows 8 is in a binary format that the Windows 7
//     parser cannot read (the page would not load at all). The Windows 7 text of the sheet, from the Windows 7
//     duser.dll, is put into the <stylesheets> of each layout,
//   * a page definition with literal page titles and the copy's own icon (the class ids stay the registered ones),
// No code byte is changed.
// =====================================================================================================================

namespace gen {

constexpr char kFrameOld[] = "graphic(1,7,-1,0,0,1,1,library(shacct.dll))";
constexpr char kFrameNew[] = "graphic(1,7,-1,0,0,1,1)";
constexpr char kCommonOpen[] = "<style resid=\"common\">";
constexpr char kCommonClose[] = "</style>";
constexpr char kSheetsClose[] = "</stylesheets>";
constexpr uint32_t kRtBitmap = 2, kRtString = 6;

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

size_t Find(const Bytes& b, const char* what, size_t from = 0) {
    const size_t n = strlen(what);
    for (size_t i = from; i + n <= b.size(); i++)
        if (memcmp(&b[i], what, n) == 0) return i;
    return SIZE_MAX;
}

// The <style resid="common"> block of the Windows 7 DirectUser, with the four strings it names (strings of DUser.dll
// there) renumbered to strings of the page.
bool CommonSheet(const Pe& duser, const std::vector<ResItem>& resources, Bytes& block);

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

// Page definition: literal titles (they would otherwise be read from System32\usercpl.dll), own icon. The class ids are
// left as they are: Control Panel refuses a class that is not registered.
void PatchPageDefinition(Bytes& xml, const StringTable& table, const std::wstring& selfPath) {
    // The "Link online IDs" page was implemented by OnlineIDcpl.dll, which only Windows 7 has: its definition goes.
    const size_t online = Find(xml, "canonicalname=\"pageOnlineAccounts\"");
    if (online != SIZE_MAX) {
        static const char open[] = "<pagedefinition>";
        static const char close[] = "</pagedefinition>";
        size_t a = SIZE_MAX;
        for (size_t i = Find(xml, open); i != SIZE_MAX && i < online; i = Find(xml, open, i + 1)) a = i;
        const size_t b = Find(xml, close, online);
        if (a != SIZE_MAX && b != SIZE_MAX) {
            size_t end = b + sizeof(close) - 1;
            while (end < xml.size() && (xml[end] == '\r' || xml[end] == '\n')) end++;
            xml.erase(xml.begin() + a, xml.begin() + end);
        }
    }
    static const char prefix[] = "displayname=\"@usercpl.dll,-";
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
        ReplaceAll(xml, "icon=\"usercpl.dll,-1\"", "icon=\"" + XmlEscape(selfPath) + ",-1\"");
}

bool CommonSheet(const Pe& duser, const std::vector<ResItem>& resources, Bytes& block) {
    for (const ResItem& r : resources) {
        if (!r.type.is(L"UIFILE") || !r.name.is(1010u)) continue;
        const Bytes text = ResourceBytes(duser, r);
        const size_t a = Find(text, kCommonOpen);
        const size_t b = a == SIZE_MAX ? SIZE_MAX : Find(text, kCommonClose, a);
        if (b == SIZE_MAX) return false;      // a binary sheet: not the Windows 7 file
        block.assign(text.begin() + a, text.begin() + b + strlen(kCommonClose));
        block.push_back('\r');
        block.push_back('\n');
        ReplaceAll(block, "resstr(1100)", "resstr(2000)");
        ReplaceAll(block, "resstr(1101)", "resstr(2001)");
        ReplaceAll(block, "resstr(1102)", "resstr(2002)");
        ReplaceAll(block, "resstr(1103)", "resstr(2003)");
        return true;
    }
    return false;
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

bool Generate(const Bytes& usercpl, const Bytes& shacct, const Bytes& duser, const StringTable& table,
              const std::wstring& selfPath, Bytes& out, std::string& error) {
    out = usercpl;
    // 1. The private DirectUI: exactly one name to replace (the delay-load descriptor's).
    {
        const size_t n = sizeof(kDuiOld);      // with the terminating zero
        size_t found = SIZE_MAX, count = 0;
        for (size_t i = 0; i + n <= out.size(); i++)
            if (memcmp(&out[i], kDuiOld, n) == 0) { found = i; count++; }
        if (count != 1) { error = "usercpl.dll: the DUI70.dll import name was not found exactly once"; return false; }
        memcpy(&out[found], kDuiNew, n);
    }
    // 2. Resources.
    Pe pe;
    if (!pe.parse(out)) { error = "usercpl.dll: not a 64-bit PE file"; return false; }
    std::vector<ResItem> res;
    if (!ReadResources(pe, res)) { error = "usercpl.dll: unreadable resources"; return false; }
    Pe sh;
    std::vector<ResItem> shres;
    if (!sh.parse(shacct) || !ReadResources(sh, shres)) { error = "shacct.dll: unreadable resources"; return false; }
    Bytes frame;
    for (const ResItem& r : shres)
        if (r.type.is(kRtBitmap) && r.name.is(1u)) frame = ResourceBytes(sh, r);
    if (frame.empty()) { error = "shacct.dll: BITMAP 1 (the picture frame) not found"; return false; }
    Pe du;
    std::vector<ResItem> dures;
    Bytes common;
    if (!du.parse(duser) || !ReadResources(du, dures) || !CommonSheet(du, dures, common)) {
        error = "duser.dll: the text of the common style sheet (UIFILE 1010) not found";
        return false;
    }

    std::vector<ResItem> items;
    bool sawDefinition = false;
    int layouts = 0;
    for (ResItem& r : res) {
        if (r.type.is(L"MUI") || r.type.is(kRtString)) continue;
        if (r.type.is(L"XMLFILE") && r.name.is(100u)) {
            r.data = ResourceBytes(pe, r);
            PatchPageDefinition(r.data, table, selfPath);
            r.isNew = true;
            sawDefinition = true;
        } else if (r.type.is(L"UIFILE")) {
            Bytes blob = ResourceBytes(pe, r);
            bool changed = ReplaceAll(blob, kFrameOld, kFrameNew) != 0;
            const size_t at = Find(blob, kSheetsClose);
            if (at != SIZE_MAX && Find(blob, kCommonOpen) == SIZE_MAX) {
                blob.insert(blob.begin() + at, common.begin(), common.end());
                changed = true;
            }
            if (changed) {
                r.data = std::move(blob);
                r.isNew = true;
                layouts++;
            }
        }
        items.push_back(std::move(r));
    }
    if (!sawDefinition || layouts == 0) { error = "usercpl.dll: page definition or layouts not found"; return false; }
    {
        ResItem f;
        f.type.id = kRtBitmap; f.name.id = 1; f.lang = 1033; f.isNew = true; f.data = std::move(frame);
        items.push_back(std::move(f));
    }
    std::vector<std::pair<uint32_t, Bytes>> blocks;
    BuildStringBlocks(table, blocks);
    for (auto& b : blocks) {
        ResItem s;
        s.type.id = kRtString; s.name.id = b.first; s.lang = 0; s.isNew = true; s.data = std::move(b.second);
        items.push_back(std::move(s));
    }
    // 3. A new section after the last one.
    const uint32_t salign = R32(out, pe.opt + 32), falign = R32(out, pe.opt + 36);
    if (!salign || !falign) { error = "usercpl.dll: bad alignment"; return false; }
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
    if (hdr + 40 > firstRaw) { error = "usercpl.dll: no room for a section header"; return false; }
    W32(out, pe.dd + 8 * 11, 0);
    W32(out, pe.dd + 8 * 11 + 4, 0);
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

#ifdef W7UA_GENERATOR_ONLY
}  // namespace
#else

// =====================================================================================================================
// Runtime
// =====================================================================================================================

std::atomic<bool> g_stopping{false};
HANDLE g_stopEvent = nullptr;
HANDLE g_setupThread = nullptr;
std::atomic<HINTERNET> g_request{nullptr};
std::atomic<bool> g_keepFiles{true};
std::atomic<int> g_languageSetting{0};      // 0 automatic, 1 English, 2 Italian, 3 French, 4 Spanish, 5 Portuguese, 6 German
std::atomic<bool> g_hideUserName{false};
std::atomic<bool> g_blockAccountRemoval{true};
std::atomic<bool> g_showLegacyLinks{false};
std::atomic<const wchar_t*> g_genericName{L"User"};

SRWLOCK g_stateLock = SRWLOCK_INIT;         // guards the block below
std::wstring g_storage;                     // <storage>
std::wstring g_pagePath;                    // the generated page; empty = not ready, the system page is used
std::wstring g_duiPath;                     // <storage>\bin\u7dui.dll
std::string g_pageSha;                      // SHA-256 of the generated page
HMODULE g_pageModule = nullptr;             // loaded at first use; never unloaded
HMODULE g_duiModule = nullptr;

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

// ---- what must be gone before the mod's code is unloaded -----------------------------------------------------------------
// Windhawk unloads the mod right after Wh_ModUninit returns. Two things outside the mod point into its code: the three
// imports of the Windows 7 page that are patched (restored by RestorePageImports) and the adapter objects that the page
// keeps while its window is open (closed and waited for by RundownLiveObjects). Nothing pins the module.

std::atomic<LONG> g_liveObjects{0};
SRWLOCK g_threadsLock = SRWLOCK_INIT;
struct ThreadCount {
    DWORD tid;
    LONG count;
};
std::vector<ThreadCount> g_threadCounts;      // threads that hold adapters right now, with how many each (guarded by g_threadsLock)

// Records one more live adapter for `tid`. Returns false if it could not be recorded (out of memory).
bool AddLive(DWORD tid) {
    try {
        ExclusiveLock lock(g_threadsLock);
        for (ThreadCount& t : g_threadCounts)
            if (t.tid == tid) {
                t.count++;
                return true;
            }
        g_threadCounts.push_back({tid, 1});
        return true;
    } catch (...) {
        return false;
    }
}

// Forgets one adapter of `tid`; the thread leaves the list when its last adapter is gone, so a thread id that Windows
// reuses later for an unrelated window is never matched.
void RemoveLive(DWORD tid) {
    ExclusiveLock lock(g_threadsLock);
    for (size_t i = 0; i < g_threadCounts.size(); i++)
        if (g_threadCounts[i].tid == tid) {
            if (--g_threadCounts[i].count <= 0) g_threadCounts.erase(g_threadCounts.begin() + i);
            return;
        }
}

struct LiveObject {
    const DWORD tid = GetCurrentThreadId();
    const bool tracked;
    LiveObject() : tracked(AddLive(tid)) { g_liveObjects.fetch_add(1); }
    LiveObject(const LiveObject&) = delete;
    LiveObject& operator=(const LiveObject&) = delete;
    ~LiveObject() {
        if (tracked) RemoveLive(tid);
        g_liveObjects.fetch_sub(1);      // last: from here on the unload may proceed
    }
};

// Calls of the three Page_* replacements that are running right now. A call already inside one of them when the imports
// are restored still returns into the mod's code, so the unload waits for it too (RundownLiveObjects).
std::atomic<LONG> g_activeCalls{0};
struct CallGuard {
    CallGuard() { g_activeCalls.fetch_add(1); }
    CallGuard(const CallGuard&) = delete;
    CallGuard& operator=(const CallGuard&) = delete;
    ~CallGuard() { g_activeCalls.fetch_sub(1); }
};

struct SavedImport {
    void** slot;
    void* original;
    void* replacement;
};
SRWLOCK g_importsLock = SRWLOCK_INIT;
std::vector<SavedImport> g_savedImports;

void RestorePageImports() {
    ExclusiveLock lock(g_importsLock);
    for (const SavedImport& saved : g_savedImports) {
        if (*saved.slot != saved.replacement) continue;      // not ours any more
        DWORD old = 0;
        if (VirtualProtect(saved.slot, sizeof(void*), PAGE_READWRITE, &old)) {
            *saved.slot = saved.original;
            VirtualProtect(saved.slot, sizeof(void*), old, &old);
        }
    }
    g_savedImports.clear();
}

// Top-level window of a thread that holds adapters. Two kinds are closed: the Explorer window itself and the popups it
// owns (the page's modal dialogs: "Browse for more pictures...", confirmation boxes), because a modal dialog keeps its
// owner from closing. The popups go first. Closing a CabinetWClass window closes all of its tabs.
struct CloseContext {
    std::vector<HWND> popups, windows;
};

BOOL CALLBACK CollectThreadWindow(HWND hwnd, LPARAM param) {
    CloseContext* context = reinterpret_cast<CloseContext*>(param);
    try {
        wchar_t cls[32];
        if (GetClassNameW(hwnd, cls, ARRAYSIZE(cls)) && wcscmp(cls, L"CabinetWClass") == 0)
            context->windows.push_back(hwnd);
        else if (GetWindow(hwnd, GW_OWNER) && IsWindowVisible(hwnd) && IsWindowEnabled(hwnd))
            context->popups.push_back(hwnd);
    } catch (...) {
    }
    return TRUE;
}

// Posts WM_CLOSE to the windows of the threads that still hold adapters, and to the enabled popups those windows own.
void PostCloseToThreads(const std::vector<DWORD>& threads) {
    CloseContext context;
    for (DWORD tid : threads) EnumThreadWindows(tid, CollectThreadWindow, reinterpret_cast<LPARAM>(&context));
    for (HWND hwnd : context.popups) PostMessageW(hwnd, WM_CLOSE, 0, 0);
    for (HWND hwnd : context.windows) PostMessageW(hwnd, WM_CLOSE, 0, 0);
}

std::vector<DWORD> ThreadsWithObjects() {
    std::vector<DWORD> threads;
    try {
        SharedLock lock(g_threadsLock);
        for (const ThreadCount& t : g_threadCounts) threads.push_back(t.tid);
    } catch (...) {
        threads.clear();
    }
    return threads;
}

// Closes the windows of the threads that hold adapters and waits, with no time limit (the mod's code must not be unmapped
// while an adapter points into it), until the last adapter is released. WM_CLOSE is posted again about once a second,
// because a window can ignore it (a modal dialog, a pending prompt), and the remaining count is logged every 5 seconds
// so that a stuck unload can be seen in the log.
void RundownLiveObjects() {
    bool pageLoaded = false;
    try {
        SharedLock lock(g_stateLock);
        pageLoaded = g_pageModule != nullptr;
    } catch (...) {
        pageLoaded = true;
    }
    if (g_liveObjects.load() > 0)
        Wh_Log(L"Unload: closing the User Accounts windows (%ld object(s) alive)", long(g_liveObjects.load()));
    const DWORD self = GetCurrentThreadId();
    ULONGLONG lastClose = 0, lastLog = GetTickCount64();
    while (g_liveObjects.load() > 0 || g_activeCalls.load() > 0) {
        const std::vector<DWORD> threads = ThreadsWithObjects();
        const ULONGLONG now = GetTickCount64();
        if (now - lastClose >= 1000 || lastClose == 0) {
            PostCloseToThreads(threads);
            lastClose = now;
        }
        if (now - lastLog >= 5000) {
            lastLog = now;
            Wh_Log(L"Unload: still waiting for %ld object(s) held by %lu thread(s), %ld call(s) in progress%s",
                   long(g_liveObjects.load()), static_cast<unsigned long>(threads.size()), long(g_activeCalls.load()),
                   threads.empty() ? L" (none of them is on a thread the mod recorded: kept by the page after its window closed?)"
                                   : L"");
        }
        if (std::find(threads.begin(), threads.end(), self) != threads.end()) {
            // A window of this very thread can only close if its messages are processed.
            MSG msg;
            while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
                TranslateMessage(&msg);
                DispatchMessageW(&msg);
            }
        }
        Sleep(20);
    }
    // Both counts are zero, but the last thread may still be leaving Release() or a Page_* function: its decrement is
    // the last thing it counts, not the last instruction it runs. No ordering removes those final instructions, so the
    // unload waits a short grace period before the mod's code can be unmapped.
    if (pageLoaded) {
        Sleep(300);
        // A call that began in the meantime (a page still open, an import not yet restored) is waited for again.
        while (g_liveObjects.load() > 0 || g_activeCalls.load() > 0) Sleep(20);
    }
    Wh_Log(L"Unload: all objects released");
}

// ---- files ----------------------------------------------------------------------------------------------------------

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

// ---- download (WinHTTP: time-limited, size-limited, cancellable from Wh_ModBeforeUninit) ----------------------------

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

bool Matches(const FileSpec& f, const Bytes& b) { return b.size() == f.size && Sha256Hex(b) == f.sha256; }

// The verified bytes of one of the three Microsoft files: from <storage>\dl if present and intact, else downloaded.
bool GetVerifiedFile(const std::wstring& dlDir, const FileSpec& f, Bytes& data) {
    try {
        const std::wstring path = dlDir + L"\\" + f.name;
        if (ReadWholeFile(path, data) && Matches(f, data)) return true;
        const std::wstring url = std::wstring(kSymbolPath) + f.key;
        for (int attempt = 1; attempt <= 3 && !g_stopping.load(); attempt++) {
            Wh_Log(L"Downloading %s from %s (attempt %d)", f.name, kSymbolHost, attempt);
            if (Download(url, f.size, data)) {
                if (Matches(f, data)) {
                    if (!WriteFileAtomic(path, data)) Wh_Log(L"Could not save %s (continuing in memory)", f.name);
                    return true;
                }
                Wh_Log(L"%s: the downloaded file does not match the pinned size and SHA-256; discarded", f.name);
            }
            if (attempt < 3 && WaitForSingleObject(g_stopEvent, 2000 * attempt) == WAIT_OBJECT_0) break;
        }
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
    if (setting == 3) return Language::French;
    if (setting == 4) return Language::Spanish;
    if (setting == 5) return Language::Portuguese;
    if (setting == 6) return Language::German;
    switch (PRIMARYLANGID(GetUserDefaultUILanguage())) {
        case LANG_ITALIAN: return Language::Italian;
        case LANG_FRENCH: return Language::French;
        case LANG_SPANISH: return Language::Spanish;
        case LANG_PORTUGUESE: return Language::Portuguese;
        case LANG_GERMAN: return Language::German;
        default: return Language::English;
    }
}

void LoadSettings() {
    int language = 0;
    {
        StringSetting s(Wh_GetStringSetting(L"language"));
        if (s.value) {
            if (!wcscmp(s.value, L"en-US")) language = 1;
            else if (!wcscmp(s.value, L"it-IT")) language = 2;
            else if (!wcscmp(s.value, L"fr-FR")) language = 3;
            else if (!wcscmp(s.value, L"es-ES")) language = 4;
            else if (!wcscmp(s.value, L"pt-PT")) language = 5;
            else if (!wcscmp(s.value, L"de-DE")) language = 6;
        }
    }
    g_languageSetting.store(language);
    g_keepFiles.store(Wh_GetIntSetting(L"keepFilesOnDisable") != 0);
    g_hideUserName.store(Wh_GetIntSetting(L"hideUserName") != 0);
    g_blockAccountRemoval.store(Wh_GetIntSetting(L"blockAccountRemoval") != 0);
    g_showLegacyLinks.store(Wh_GetIntSetting(L"showLegacyLinks") != 0);
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

void RunSetup() {
    const std::wstring storage = StorageDir();
    if (storage.empty()) { Wh_Log(L"Setup: no storage folder"); return; }
    const std::wstring dlDir = storage + L"\\dl", binDir = storage + L"\\bin";
    EnsureDirs(dlDir);
    EnsureDirs(binDir);

    // One Explorer at a time (there can be several explorer.exe processes writing the same files).
    NamedMutex mutex(CreateMutexW(nullptr, FALSE, L"Local\\Win7UserAccountPageRestorer.Setup"));
    if (mutex.h) {
        HANDLE handles[2] = {g_stopEvent, mutex.h};
        const DWORD w = WaitForMultipleObjects(2, handles, FALSE, 300000);
        if (w != WAIT_OBJECT_0 + 1 && w != WAIT_ABANDONED_0 + 1) return;
        mutex.owned = true;
    }

    Bytes files[ARRAYSIZE(kFiles)];
    for (size_t i = 0; i < ARRAYSIZE(kFiles); i++) {
        if (g_stopping.load() || !GetVerifiedFile(dlDir, kFiles[i], files[i])) {
            if (!g_stopping.load())
                Wh_Log(L"Setup stopped: %s is not available. The Windows page stays in place. Check the internet "
                       L"connection and restart explorer.exe.", kFiles[i].name);
            return;
        }
    }

    // One copy of the page per process: if this process already runs one (loaded before a settings change or by an
    // earlier instance of the mod), that language stays until explorer.exe restarts.
    auto pathFor = [&](Language language) {
        return binDir + L"\\usercpl7." + TableFor(language).tag + L"." + kGeneratorRevision + L".dll";
    };
    Language language = CurrentLanguage();
    for (Language other : {Language::English, Language::Italian, Language::French, Language::Spanish, Language::Portuguese, Language::German}) {
        if (other != language && GetModuleHandleW(pathFor(other).c_str())) {
            Wh_Log(L"The page language applies after explorer.exe restarts (the page is in use)");
            language = other;
        }
    }
    const StringTable table = TableFor(language);
    const std::wstring pagePath = pathFor(language);
    const std::wstring duiPath = binDir + L"\\" + kPrivateDuiName;
    if (pagePath.size() >= MAX_PATH - 1) { Wh_Log(L"Setup: the storage path is too long"); return; }
    Bytes page;
    std::string error;
    if (!gen::Generate(files[kFileUserCpl], files[kFileShAcct], files[kFileDUser], table, pagePath, page, error)) {
        Wh_Log(L"Setup: could not build the page: %S", error.c_str());
        return;
    }
    if (!EnsureFile(duiPath, files[kFileDui]) || !EnsureFile(pagePath, page)) {
        Wh_Log(L"Setup: could not write the files in %s", binDir.c_str());
        return;
    }
    const std::string sha = Sha256Hex(page);

    // Pages of older generator revisions are removed (unless an Explorer has them loaded). Pages of the current revision
    // stay, whatever their language: another Explorer, or another user's, may have built one it has not opened yet.
    {
        WIN32_FIND_DATAW fd;
        FindHandle h(FindFirstFileW((binDir + L"\\usercpl7.*.dll").c_str(), &fd));
        if (h.valid()) {
            do {
                const std::wstring other = binDir + L"\\" + fd.cFileName;
                const std::wstring name = fd.cFileName;
                const std::wstring current = std::wstring(L".") + kGeneratorRevision + L".dll";
                const bool isCurrent =
                    name.size() >= current.size() &&
                    _wcsicmp(name.c_str() + name.size() - current.size(), current.c_str()) == 0;
                if (!isCurrent) DeleteFileW(other.c_str());
            } while (FindNextFileW(h.get(), &fd));
        }
    }

    {
        ExclusiveLock lock(g_stateLock);
        g_storage = storage;
        g_duiPath = duiPath;
        if (!g_pageModule) {
            g_pagePath = pagePath;
            g_pageSha = sha;
            g_genericName.store(table.generic);
        }
    }
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
}

// ---- loading the page (at first use, on the thread that opens it) ---------------------------------------------------

// Loads `path` only if its content has the expected SHA-256. The file is held open without write or delete sharing
// from before the check until after the load, so what is checked is what is loaded.
HMODULE LoadVerified(const std::wstring& path, const std::string& sha256) {
    try {
        FileHandle f(CreateFileW(path.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING,
                                 FILE_ATTRIBUTE_NORMAL, nullptr));
        if (!f.valid()) return nullptr;
        Sha256 hash;
        for (;;) {
            uint8_t buf[65536];
            DWORD got = 0;
            if (!ReadFile(f.get(), buf, sizeof(buf), &got, nullptr)) return nullptr;
            if (!got) break;
            hash.update(buf, got);
        }
        if (hash.hex() != sha256) {
            Wh_Log(L"%s does not have the expected content; not loaded", path.c_str());
            return nullptr;
        }
        // Absolute path, and the imports of the two files are searched in System32 only: <storage>\bin is shared by every
        // user's Explorer, so a DLL another user puts there must not be found by name. (u7dui.dll, which the page
        // delay-loads, is already loaded by the time the page is, and a name that is loaded is not searched again.)
        return LoadLibraryExW(path.c_str(), nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);      // the file is still held open
    } catch (...) {
        return nullptr;
    }
}

// ---- three imports of the private page answer differently (in memory, in the private copy only) ---------------------
//
// The page offers three things that Windows 10/11 no longer have a target for. The page decides about each of them by
// asking Windows; here the answer is given that makes it leave the dead link out.

// "Link online IDs" is offered on every workstation; its page was in OnlineIDcpl.dll (Windows 7 only). The page reads
// the product type for nothing else. The optional setting keeps the original answer.
BOOL WINAPI Page_GetVersionExW(LPOSVERSIONINFOW info) {
    CallGuard guard;
    const BOOL ok = GetVersionExW(info);
    if (!g_showLegacyLinks.load() && ok && info && info->dwOSVersionInfoSize >= sizeof(OSVERSIONINFOEXW))
        reinterpret_cast<OSVERSIONINFOEXW*>(info)->wProductType = VER_NT_SERVER;
    return ok;
}

// "Parental Controls" (left pane and "Manage Accounts"): offered if Windows says the feature is installed. It is
// offered here only if a Parental Controls / Family Safety applet is registered in Control Panel (Windows 10/11 have
// none; a mod that restores it makes the link come back).
HRESULT WINAPI Page_WPC_InstallState(DWORD* state) {
    CallGuard guard;
    if (!state) return E_POINTER;
    RegKey key;
    const bool present =
        RegOpenKeyExW(HKEY_LOCAL_MACHINE,
                      L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\ControlPanel\\NameSpace\\"
                      L"{96AE8D84-A250-4520-95A5-A47A7E3C548B}",
                      0, KEY_QUERY_VALUE, &key.k) == ERROR_SUCCESS;
    *state = present ? 0 : 1;      // 0 = installed
    return S_OK;
}

// "Manage your fingerprint data" is offered when a fingerprint reader is found through winbio.dll; it starts
// biocpl.dll, which Windows 10/11 do not have. By default, without that file the page is not given winbio.dll.
// The optional setting permits the original check; it does not create a missing target.
HMODULE WINAPI Page_LoadLibraryW(LPCWSTR name) {
    CallGuard guard;
    if (!g_showLegacyLinks.load() && name && _wcsicmp(name, L"winbio.dll") == 0) {
        wchar_t path[MAX_PATH];
        const UINT n = GetSystemDirectoryW(path, ARRAYSIZE(path));
        if (n && n + 12 < ARRAYSIZE(path)) {
            wcscat(path, L"\\biocpl.dll");
            if (GetFileAttributesW(path) == INVALID_FILE_ATTRIBUTES) {
                SetLastError(ERROR_MOD_NOT_FOUND);
                return nullptr;
            }
        }
    }
    return LoadLibraryW(name);
}

// Replaces one entry of the import table of `module` (by name, or by ordinal if `name` is null).
bool PatchImport(HMODULE module, const char* dll, const char* name, WORD ordinal, void* replacement) {
    BYTE* base = reinterpret_cast<BYTE*>(module);
    const auto* dos = reinterpret_cast<const IMAGE_DOS_HEADER*>(base);
    const auto* nt = reinterpret_cast<const IMAGE_NT_HEADERS*>(base + dos->e_lfanew);
    const IMAGE_DATA_DIRECTORY& dir = nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT];
    if (!dir.VirtualAddress) return false;
    for (auto* d = reinterpret_cast<const IMAGE_IMPORT_DESCRIPTOR*>(base + dir.VirtualAddress); d->Name; d++) {
        if (_stricmp(reinterpret_cast<const char*>(base + d->Name), dll) != 0 || !d->OriginalFirstThunk) continue;
        const auto* names = reinterpret_cast<const IMAGE_THUNK_DATA*>(base + d->OriginalFirstThunk);
        auto* slots = reinterpret_cast<void**>(base + d->FirstThunk);
        for (size_t i = 0; names[i].u1.AddressOfData; i++) {
            bool match;
            if (IMAGE_SNAP_BY_ORDINAL(names[i].u1.Ordinal)) {
                match = !name && IMAGE_ORDINAL(names[i].u1.Ordinal) == ordinal;
            } else {
                const auto* byName = reinterpret_cast<const IMAGE_IMPORT_BY_NAME*>(base + names[i].u1.AddressOfData);
                match = name && strcmp(reinterpret_cast<const char*>(byName->Name), name) == 0;
            }
            if (!match) continue;
            // The original value is saved first: Wh_ModUninit writes it back (RestorePageImports).
            try {
                ExclusiveLock lock(g_importsLock);
                g_savedImports.push_back({&slots[i], slots[i], replacement});
            } catch (...) {
                return false;
            }
            DWORD old = 0;
            if (!VirtualProtect(&slots[i], sizeof(void*), PAGE_READWRITE, &old)) {
                ExclusiveLock lock(g_importsLock);
                g_savedImports.pop_back();
                return false;
            }
            slots[i] = replacement;
            VirtualProtect(&slots[i], sizeof(void*), old, &old);
            return true;
        }
    }
    return false;
}

void PatchPageImports(HMODULE page) {
    const bool a = PatchImport(page, "KERNEL32.dll", "GetVersionExW", 0, reinterpret_cast<void*>(Page_GetVersionExW));
    const bool b = PatchImport(page, "SHELL32.dll", nullptr, 859, reinterpret_cast<void*>(Page_WPC_InstallState));
    const bool c = PatchImport(page, "KERNEL32.dll", "LoadLibraryW", 0, reinterpret_cast<void*>(Page_LoadLibraryW));
    if (!a || !b || !c) Wh_Log(L"Some links without a target could not be hidden (%d%d%d)", a, b, c);
}

using DllGetClassObject_t = HRESULT(STDAPICALLTYPE*)(REFCLSID, REFIID, LPVOID*);

SRWLOCK g_loadLock = SRWLOCK_INIT;          // one loader at a time; never taken by the hooks' fast paths

DllGetClassObject_t EnsurePageLoaded() {
    // Throws only std::bad_alloc (string copies); the callers are the hooks, which catch it. Every lock is a guard.
    HMODULE page = nullptr;
    std::wstring pagePath, duiPath;
    std::string pageSha;
    {
        SharedLock lock(g_stateLock);
        page = g_pageModule;
        pagePath = g_pagePath;
        duiPath = g_duiPath;
        pageSha = g_pageSha;
    }
    if (!page && !pagePath.empty()) {
        ExclusiveLock loader(g_loadLock);
        HMODULE dui = nullptr;
        {
            SharedLock lock(g_stateLock);
            page = g_pageModule;
            dui = g_duiModule;
        }
        if (!page) {
            // The DirectUI first: the page finds it by name when it first needs it.
            if (!dui) dui = LoadVerified(duiPath, kFiles[kFileDui].sha256);
            if (dui) page = LoadVerified(pagePath, pageSha);
            if (page) {
                PatchPageImports(page);
                Wh_Log(L"Windows 7 page loaded");
            } else {
                Wh_Log(L"Could not load the Windows 7 page (error %lu)", GetLastError());
            }
            ExclusiveLock lock(g_stateLock);
            g_duiModule = dui;
            g_pageModule = page;
        }
    }
    if (!page) return nullptr;
    return reinterpret_cast<DllGetClassObject_t>(reinterpret_cast<void*>(GetProcAddress(page, "DllGetClassObject")));
}

// =====================================================================================================================
// Account objects: the Windows 7 page on the Windows 10/11 implementation.
//
// The page asks COM for ShellLogonEnumUsers with the Windows 7 interface id. Windows 10 1607 ... Windows 11 26H1
// implement that class with another interface id and another virtual table (no IDispatch; three methods gone, a few
// added; the arguments of the common methods are the same). The adapters below have the Windows 7 layout and forward to
// the Windows 10 one. The layouts were read from the public symbols of 20 builds of usercpl.dll (tools/abi.py); the
// interface ids tie them down: an object that answers the Windows 10 id has the Windows 10 layout.
// =====================================================================================================================

struct IUser10 : IUnknown {
    virtual HRESULT STDMETHODCALLTYPE get_setting(BSTR name, VARIANT* value) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_setting(BSTR name, VARIANT value) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_isLoggedOn(VARIANT_BOOL* value) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_passwordRequired(VARIANT_BOOL* value) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_interactiveLogonAllowed(VARIANT_BOOL* value) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_isProfilePrivate(VARIANT_BOOL* value) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_isMemberOfDomain(VARIANT_BOOL* value) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_isLocalUserConnected(VARIANT_BOOL* value) = 0;
    virtual HRESULT STDMETHODCALLTYPE logoff(VARIANT_BOOL* result) = 0;
    virtual HRESULT STDMETHODCALLTYPE changePassword(VARIANT newPassword, VARIANT oldPassword, VARIANT_BOOL* result) = 0;
    virtual HRESULT STDMETHODCALLTYPE makeProfilePrivate(VARIANT_BOOL makePrivate) = 0;
    virtual HRESULT STDMETHODCALLTYPE getMailAccountInfo(UINT index, VARIANT* name, UINT* unread) = 0;
    // isPasswordValid follows; not used.
};

struct IEnum10 : IUnknown {
    virtual HRESULT STDMETHODCALLTYPE get_length(UINT* count) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_currentUser(IUser10** user) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_currentUserWithImpersonation(IUser10** user) = 0;
    virtual HRESULT STDMETHODCALLTYPE item(VARIANT id, IUser10** user) = 0;
    // Slot reserved: the Windows 10 create signature is not established and must not be called.
    virtual HRESULT STDMETHODCALLTYPE ReservedCreateSlot() = 0;
    virtual HRESULT STDMETHODCALLTYPE remove(VARIANT id, VARIANT backupPath, VARIANT_BOOL* result) = 0;
    virtual HRESULT STDMETHODCALLTYPE ResetList() = 0;
    // CreateConnectedUser, ConnectLocalUser, FindConnectedUserByName, SetupParentalControls follow; not used.
};

struct IFactory10 : IUnknown {
    virtual HRESULT STDMETHODCALLTYPE Reserved() = 0;      // added in Windows 10; never called here
    virtual HRESULT STDMETHODCALLTYPE Activate(HWND owner, REFGUID elevationClass) = 0;
    virtual HRESULT STDMETHODCALLTYPE CreateObject(REFCLSID clsid, REFIID iid, void** object) = 0;
};

// The Windows 7 shapes (dual interfaces: IDispatch first).
struct IUser7 : IDispatch {
    virtual HRESULT STDMETHODCALLTYPE get_setting(BSTR name, VARIANT* value) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_setting(BSTR name, VARIANT value) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_isLoggedOn(VARIANT_BOOL* value) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_passwordRequired(VARIANT_BOOL* value) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_interactiveLogonAllowed(VARIANT_BOOL* value) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_isProfilePrivate(VARIANT_BOOL* value) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_isPasswordResetAvailable(VARIANT_BOOL* value) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_isMemberOfDomain(VARIANT_BOOL* value) = 0;
    virtual HRESULT STDMETHODCALLTYPE logoff(VARIANT_BOOL* result) = 0;
    virtual HRESULT STDMETHODCALLTYPE changePassword(VARIANT newPassword, VARIANT oldPassword, VARIANT_BOOL* result) = 0;
    virtual HRESULT STDMETHODCALLTYPE makeProfilePrivate(VARIANT_BOOL makePrivate) = 0;
    virtual HRESULT STDMETHODCALLTYPE getMailAccountInfo(UINT index, VARIANT* name, UINT* unread) = 0;
};

struct IEnum7 : IDispatch {
    virtual HRESULT STDMETHODCALLTYPE get_EnumFlags(int* flags) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_EnumFlags(int flags) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_length(UINT* count) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_currentUser(IUser7** user) = 0;
    virtual HRESULT STDMETHODCALLTYPE item(VARIANT id, IUser7** user) = 0;
    virtual HRESULT STDMETHODCALLTYPE _NewEnum(IUnknown** enumerator) = 0;
    virtual HRESULT STDMETHODCALLTYPE create(BSTR loginName, IUser7** user) = 0;
    virtual HRESULT STDMETHODCALLTYPE remove(VARIANT id, VARIANT backupPath, VARIANT_BOOL* result) = 0;
    virtual HRESULT STDMETHODCALLTYPE Advise(IUnknown* callback, DWORD* cookie) = 0;
    virtual HRESULT STDMETHODCALLTYPE Unadvise(DWORD cookie) = 0;
    virtual HRESULT STDMETHODCALLTYPE ResetList() = 0;
    virtual HRESULT STDMETHODCALLTYPE get_currentUserWithImpersonation(IUser7** user) = 0;
};

struct IFactory7 : IUnknown {
    virtual HRESULT STDMETHODCALLTYPE Activate(HWND owner, REFGUID elevationClass) = 0;
    virtual HRESULT STDMETHODCALLTYPE CreateObject(REFCLSID clsid, REFIID iid, void** object) = 0;
};

bool Same(REFGUID a, const GUID& b) { return memcmp(&a, &b, sizeof(GUID)) == 0; }

// IUnknown and the empty IDispatch shared by the two adapters. The page calls the methods directly; scripting, the
// only user of IDispatch, never sees these objects.
template <class Interface, const GUID* Iid>
struct AdapterBase : Interface, LiveObject {
    std::atomic<LONG> refs{1};
    virtual ~AdapterBase() = default;
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid, void** object) override {
        if (!object) return E_POINTER;
        if (Same(iid, kIidUnknown) || Same(iid, kIidDispatch) || Same(iid, *Iid)) {
            *object = static_cast<Interface*>(this);
            AddRef();
            return S_OK;
        }
        *object = nullptr;
        return E_NOINTERFACE;
    }
    ULONG STDMETHODCALLTYPE AddRef() override { return ULONG(++refs); }
    ULONG STDMETHODCALLTYPE Release() override {
        const LONG left = --refs;
        if (!left) delete this;
        return ULONG(left);
    }
    HRESULT STDMETHODCALLTYPE GetTypeInfoCount(UINT* count) override {
        if (count) *count = 0;
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE GetTypeInfo(UINT, LCID, ITypeInfo** info) override {
        if (info) *info = nullptr;
        return E_NOTIMPL;
    }
    HRESULT STDMETHODCALLTYPE GetIDsOfNames(REFIID, LPOLESTR*, UINT, LCID, DISPID*) override { return DISP_E_UNKNOWNNAME; }
    HRESULT STDMETHODCALLTYPE Invoke(DISPID, REFIID, LCID, WORD, DISPPARAMS*, VARIANT*, EXCEPINFO*, UINT*) override {
        return DISP_E_MEMBERNOTFOUND;
    }
};

// The picture of the signed-in user as Windows 10/11 keep it. When the picture is chosen in Settings the account object
// answers "" (or a file that is not there) to "Picture", and the real files are listed in the registry, one value per
// size (Image32 ... Image1080), under AccountPicture\Users\<SID>. The smallest file of at least 192 pixels is used (the
// page draws it small, but sharp on high DPI); if there is none, the largest one.
bool CurrentUserSid(std::wstring& out) {
    HANDLE raw = nullptr;
    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &raw)) return false;
    FileHandle token(raw);
    DWORD size = 0;
    GetTokenInformation(token.get(), TokenUser, nullptr, 0, &size);
    if (!size) return false;
    std::vector<BYTE> buffer(size);
    if (!GetTokenInformation(token.get(), TokenUser, buffer.data(), size, &size)) return false;
    LPWSTR text = nullptr;
    if (!ConvertSidToStringSidW(reinterpret_cast<TOKEN_USER*>(buffer.data())->User.Sid, &text)) return false;
    out = text;
    LocalFree(text);
    return true;
}

std::wstring RegistryAccountPicture() {
    try {
        std::wstring sid;
        if (!CurrentUserSid(sid)) return std::wstring();
        const std::wstring path = L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\AccountPicture\\Users\\" + sid;
        RegKey key;
        if (RegOpenKeyExW(HKEY_LOCAL_MACHINE, path.c_str(), 0, KEY_QUERY_VALUE, &key.k) != ERROR_SUCCESS)
            return std::wstring();
        std::wstring best;
        int bestGroup = 2;      // 0: at least 192 px (smaller is better), 1: smaller than 192 px (larger is better)
        long bestSize = 0;
        for (DWORD index = 0;; index++) {
            wchar_t name[64];
            DWORD nameLength = ARRAYSIZE(name);
            wchar_t data[MAX_PATH];
            DWORD dataLength = sizeof(data) - sizeof(wchar_t), type = 0;
            const LONG result = RegEnumValueW(key.k, index, name, &nameLength, nullptr, &type,
                                              reinterpret_cast<BYTE*>(data), &dataLength);
            if (result == ERROR_NO_MORE_ITEMS) break;
            if (result != ERROR_SUCCESS || type != REG_SZ || _wcsnicmp(name, L"Image", 5) != 0) continue;
            data[dataLength / sizeof(wchar_t)] = L'\0';
            const long size = _wtol(name + 5);
            if (size <= 0 || !data[0] || GetFileAttributesW(data) == INVALID_FILE_ATTRIBUTES) continue;
            const int group = size >= 192 ? 0 : 1;
            const bool better = group < bestGroup || (group == bestGroup && (group == 0 ? size < bestSize : size > bestSize));
            if (better) {
                best = data;
                bestGroup = group;
                bestSize = size;
            }
        }
        return best;
    } catch (...) {
        return std::wstring();
    }
}

// The Windows 7 page draws its picture frame from a BMP (Windows 7 itself kept "user.bmp" and "<name>.bmp" in
// ProgramData\Microsoft\User Account Pictures); a JPEG or PNG leaves the frame empty. The pictures that Windows 10/11
// keep are JPEG or PNG, so the picture is converted to a BMP of the same size, once, in the mod's storage folder.
// WIC (the Windows Imaging Component) does the conversion; its ids are spelled out so that no extra library is needed.
constexpr GUID kClsidWicFactory = {0xcacaf262, 0x9370, 0x4615, {0xa1, 0x3b, 0x9f, 0x55, 0x39, 0xda, 0x4c, 0x0a}};
constexpr GUID kIidWicFactory = {0xec5ec8a9, 0xc395, 0x4314, {0x9c, 0x77, 0x54, 0xd7, 0xa9, 0x35, 0xff, 0x70}};
constexpr GUID kWicContainerBmp = {0x0af1d87e, 0xfcfe, 0x4188, {0xbd, 0xeb, 0xa7, 0x90, 0x64, 0x71, 0xcb, 0xe3}};
constexpr GUID kWicPixelFormat24bppBgr = {0x6fddc324, 0x4e03, 0x4bfe, {0xb1, 0x85, 0x3d, 0x77, 0x76, 0x8d, 0xc9, 0x0c}};

// Writes `source` as a 24-bit BMP to `target` (through a temporary file, renamed into place).
bool ConvertPictureToBmp(const std::wstring& source, const std::wstring& target) {
    const std::wstring temp = target + L".tmp";
    bool done = false;
    {
        IWICImagingFactory* rawFactory = nullptr;
        if (FAILED(CoCreateInstance(kClsidWicFactory, nullptr, CLSCTX_INPROC_SERVER, kIidWicFactory,
                                    reinterpret_cast<void**>(&rawFactory))) || !rawFactory)
            return false;
        std::unique_ptr<IWICImagingFactory, ComDeleter> factory(rawFactory);

        IWICBitmapDecoder* rawDecoder = nullptr;
        if (FAILED(factory->CreateDecoderFromFilename(source.c_str(), nullptr, GENERIC_READ,
                                                      WICDecodeMetadataCacheOnDemand, &rawDecoder)) || !rawDecoder)
            return false;
        std::unique_ptr<IWICBitmapDecoder, ComDeleter> decoder(rawDecoder);
        IWICBitmapFrameDecode* rawFrame = nullptr;
        if (FAILED(decoder->GetFrame(0, &rawFrame)) || !rawFrame) return false;
        std::unique_ptr<IWICBitmapFrameDecode, ComDeleter> frame(rawFrame);

        IWICFormatConverter* rawConverter = nullptr;
        if (FAILED(factory->CreateFormatConverter(&rawConverter)) || !rawConverter) return false;
        std::unique_ptr<IWICFormatConverter, ComDeleter> converter(rawConverter);
        if (FAILED(converter->Initialize(frame.get(), kWicPixelFormat24bppBgr, WICBitmapDitherTypeNone, nullptr, 0.0,
                                         WICBitmapPaletteTypeCustom)))
            return false;
        UINT width = 0, height = 0;
        if (FAILED(converter->GetSize(&width, &height)) || !width || !height || width > 4096 || height > 4096)
            return false;

        IWICStream* rawStream = nullptr;
        if (FAILED(factory->CreateStream(&rawStream)) || !rawStream) return false;
        std::unique_ptr<IWICStream, ComDeleter> stream(rawStream);
        if (FAILED(stream->InitializeFromFilename(temp.c_str(), GENERIC_WRITE))) return false;
        IWICBitmapEncoder* rawEncoder = nullptr;
        if (FAILED(factory->CreateEncoder(kWicContainerBmp, nullptr, &rawEncoder)) || !rawEncoder) return false;
        std::unique_ptr<IWICBitmapEncoder, ComDeleter> encoder(rawEncoder);
        if (FAILED(encoder->Initialize(stream.get(), WICBitmapEncoderNoCache))) return false;
        IWICBitmapFrameEncode* rawTarget = nullptr;
        if (FAILED(encoder->CreateNewFrame(&rawTarget, nullptr)) || !rawTarget) return false;
        std::unique_ptr<IWICBitmapFrameEncode, ComDeleter> out(rawTarget);
        WICPixelFormatGUID format = kWicPixelFormat24bppBgr;
        done = SUCCEEDED(out->Initialize(nullptr)) && SUCCEEDED(out->SetSize(width, height)) &&
               SUCCEEDED(out->SetPixelFormat(&format)) && Same(format, kWicPixelFormat24bppBgr) &&
               SUCCEEDED(out->WriteSource(converter.get(), nullptr)) && SUCCEEDED(out->Commit()) &&
               SUCCEEDED(encoder->Commit());
    }      // every COM object, and with them the temporary file, is released before the rename
    if (done) done = MoveFileExW(temp.c_str(), target.c_str(), MOVEFILE_REPLACE_EXISTING) != 0;
    if (!done) DeleteFileW(temp.c_str());
    return done;
}

// The BMP to give the page for the picture file `source`: the file itself if it already is a BMP, otherwise a converted
// copy named after the source (path and time of last write, so a new picture gives a new copy). Empty on failure.
std::wstring PictureAsBitmap(const std::wstring& source) {
    try {
        const size_t dot = source.rfind(L'.');
        if (dot != std::wstring::npos && _wcsicmp(source.c_str() + dot, L".bmp") == 0) return source;
        WIN32_FILE_ATTRIBUTE_DATA info{};
        if (!GetFileAttributesExW(source.c_str(), GetFileExInfoStandard, &info)) return std::wstring();
        std::wstring storage;
        {
            SharedLock lock(g_stateLock);
            storage = g_storage;
        }
        if (storage.empty()) storage = StorageDir();
        if (storage.empty()) return std::wstring();
        const std::wstring dir = storage + L"\\pic";
        Bytes key(reinterpret_cast<const uint8_t*>(source.c_str()),
                  reinterpret_cast<const uint8_t*>(source.c_str()) + source.size() * sizeof(wchar_t));
        const uint8_t* time = reinterpret_cast<const uint8_t*>(&info.ftLastWriteTime);
        key.insert(key.end(), time, time + sizeof(FILETIME));
        const std::wstring name = [&] {
            const std::string hex = Sha256Hex(key).substr(0, 24);
            return std::wstring(hex.begin(), hex.end()) + L".bmp";
        }();
        const std::wstring target = dir + L"\\" + name;
        if (GetFileAttributesW(target.c_str()) != INVALID_FILE_ATTRIBUTES) return target;      // converted earlier
        EnsureDirs(dir);
        if (!ConvertPictureToBmp(source, target)) {
            Wh_Log(L"Could not convert the account picture to a BMP: %s", source.c_str());
            return std::wstring();
        }
        // Copies of earlier pictures are removed (best effort).
        WIN32_FIND_DATAW fd;
        FindHandle h(FindFirstFileW((dir + L"\\*.bmp").c_str(), &fd));
        if (h.valid()) {
            do {
                if (_wcsicmp(fd.cFileName, name.c_str()) != 0) DeleteFileW((dir + L"\\" + fd.cFileName).c_str());
            } while (FindNextFileW(h.get(), &fd));
        }
        return target;
    } catch (...) {
        return std::wstring();
    }
}

struct UserAdapter final : AdapterBase<IUser7, &kIidUser7> {
    IUser10* inner;      // owned
    bool current;        // the account of the signed-in user (the only one whose name can be hidden)
    UserAdapter(IUser10* user, bool isCurrent) : inner(user), current(isCurrent) {}
    ~UserAdapter() override { inner->Release(); }

    HRESULT STDMETHODCALLTYPE get_setting(BSTR name, VARIANT* value) override {
        const HRESULT hr = inner->get_setting(name, value);
        // "Hide user name": the name of the signed-in user is shown as a generic one on this page only. Windows, the
        // account and every other account are not touched.
        if (SUCCEEDED(hr) && current && name && value && g_hideUserName.load()) {
            static std::atomic<int> logged{0};
            if (logged.fetch_add(1) < 100) Wh_Log(L"get_setting: %s", name);
            if (_wcsicmp(name, L"DisplayName") == 0 && value->vt == VT_BSTR) {
                if (BSTR text = SysAllocString(g_genericName.load())) {
                    VariantClear(value);
                    value->vt = VT_BSTR;
                    value->bstrVal = text;
                }
            }
        }
        // Windows 7 always answered with a picture file. Windows 10/11 answer "" while the account uses the default
        // picture (nothing was ever chosen), and the page then draws an empty frame: give it the default picture.
        // "PictureSource" (the file the picture was made from) gets the same answer, so that the page finds the
        // current picture among the ones it lists instead of adding it a second time.
        if (SUCCEEDED(hr) && name && value && (_wcsicmp(name, L"Picture") == 0 || _wcsicmp(name, L"PictureSource") == 0)) {
            const bool hasText = value->vt == VT_BSTR && value->bstrVal && value->bstrVal[0];
            if (!hasText || GetFileAttributesW(value->bstrVal) == INVALID_FILE_ATTRIBUTES) {
                // First the picture the user really has (chosen in Settings); the generic one only if there is none.
                const std::wstring real = current ? RegistryAccountPicture() : std::wstring();
                const std::wstring shown = real.empty() ? std::wstring() : PictureAsBitmap(real);
                if (!shown.empty()) {
                    if (BSTR text = SysAllocString(shown.c_str())) {
                        VariantClear(value);
                        value->vt = VT_BSTR;
                        value->bstrVal = text;
                        static std::atomic<int> loggedReal{0};
                        if (loggedReal.fetch_add(1) < 20) Wh_Log(L"Account picture (%s): %s -> %s", name, real.c_str(), shown.c_str());
                        return hr;
                    }
                }
                static const wchar_t* const candidates[] = {
                    L"%ProgramData%\\Microsoft\\User Account Pictures\\user.bmp",
                    L"%ProgramData%\\Microsoft\\User Account Pictures\\user.png",
                    L"%ProgramData%\\Microsoft\\User Account Pictures\\user-192.png",
                };
                for (const wchar_t* candidate : candidates) {
                    wchar_t path[MAX_PATH];
                    const DWORD n = ExpandEnvironmentStringsW(candidate, path, ARRAYSIZE(path));
                    if (!n || n > ARRAYSIZE(path) || GetFileAttributesW(path) == INVALID_FILE_ATTRIBUTES) continue;
                    BSTR text = SysAllocString(path);
                    if (!text) break;
                    VariantClear(value);
                    value->vt = VT_BSTR;
                    value->bstrVal = text;
                    Wh_Log(L"Default user picture (%s): %s", name, path);
                    break;
                }
            }
        }
        return hr;
    }
    HRESULT STDMETHODCALLTYPE put_setting(BSTR name, VARIANT value) override {
        // The page offers the name it was given as the starting text of "Change the account name". Saving it
        // unchanged must not rename the real account to the generic name.
        if (current && g_hideUserName.load() && name && _wcsicmp(name, L"DisplayName") == 0 && value.vt == VT_BSTR &&
            value.bstrVal && wcscmp(value.bstrVal, g_genericName.load()) == 0)
            return S_OK;
        const HRESULT hr = inner->put_setting(name, value);
        return hr;
    }
    HRESULT STDMETHODCALLTYPE get_isLoggedOn(VARIANT_BOOL* v) override { return inner->get_isLoggedOn(v); }
    HRESULT STDMETHODCALLTYPE get_passwordRequired(VARIANT_BOOL* v) override { return inner->get_passwordRequired(v); }
    HRESULT STDMETHODCALLTYPE get_interactiveLogonAllowed(VARIANT_BOOL* v) override {
        return inner->get_interactiveLogonAllowed(v);
    }
    HRESULT STDMETHODCALLTYPE get_isProfilePrivate(VARIANT_BOOL* v) override { return inner->get_isProfilePrivate(v); }
    HRESULT STDMETHODCALLTYPE get_isPasswordResetAvailable(VARIANT_BOOL* v) override {
        // Gone from Windows 10. Windows 7 answered "true" when a password reset disk could be used at the Welcome
        // screen; the page only words a hint differently.
        if (!v) return E_POINTER;
        *v = VARIANT_FALSE;
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE get_isMemberOfDomain(VARIANT_BOOL* v) override { return inner->get_isMemberOfDomain(v); }
    HRESULT STDMETHODCALLTYPE logoff(VARIANT_BOOL* r) override { return inner->logoff(r); }
    HRESULT STDMETHODCALLTYPE changePassword(VARIANT n, VARIANT o, VARIANT_BOOL* r) override {
        const HRESULT hr = inner->changePassword(n, o, r);
        return hr;
    }
    HRESULT STDMETHODCALLTYPE makeProfilePrivate(VARIANT_BOOL p) override { return inner->makeProfilePrivate(p); }
    HRESULT STDMETHODCALLTYPE getMailAccountInfo(UINT i, VARIANT* n, UINT* u) override {
        return inner->getMailAccountInfo(i, n, u);
    }
};

// Takes over the reference of `user` (also on failure).
HRESULT WrapUser(HRESULT hr, IUser10* user, IUser7** out, bool current = false) {
    if (!out) {
        if (user) user->Release();
        return E_POINTER;
    }
    *out = nullptr;
    if (FAILED(hr)) {
        if (user) user->Release();
        return hr;
    }
    if (!user) return hr;      // S_FALSE with no user is passed on as it is
    UserAdapter* adapter = new (std::nothrow) UserAdapter(user, current);
    if (!adapter) {
        user->Release();
        return E_OUTOFMEMORY;
    }
    *out = adapter;
    return hr;
}

// The native create slot has an unverified ABI. Never call it; use net.exe and
// retrieve the new user through this adapter's existing enumerator.
HRESULT CreateAccountWorkaround(IEnum10* enumerator, BSTR loginName, IUser10** user);

struct EnumAdapter final : AdapterBase<IEnum7, &kIidEnum7> {
    IEnum10* inner;      // owned
    int flags = 0;
    explicit EnumAdapter(IEnum10* e) : inner(e) {}
    ~EnumAdapter() override { inner->Release(); }

    HRESULT STDMETHODCALLTYPE get_EnumFlags(int* f) override {
        if (!f) return E_POINTER;
        *f = flags;
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE put_EnumFlags(int f) override {
        flags = f;
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE get_length(UINT* count) override { return inner->get_length(count); }
    HRESULT STDMETHODCALLTYPE get_currentUser(IUser7** user) override {
        IUser10* u = nullptr;
        const HRESULT hr = inner->get_currentUser(&u);
        return WrapUser(hr, u, user, true);
    }
    HRESULT STDMETHODCALLTYPE item(VARIANT id, IUser7** user) override {
        IUser10* u = nullptr;
        const HRESULT hr = inner->item(id, &u);
        return WrapUser(hr, u, user);
    }
    HRESULT STDMETHODCALLTYPE _NewEnum(IUnknown** enumerator) override {
        if (enumerator) *enumerator = nullptr;
        return E_NOTIMPL;
    }
    HRESULT STDMETHODCALLTYPE create(BSTR loginName, IUser7** user) override {
        if (!user) return E_POINTER;
        *user = nullptr;
        IUser10* u = nullptr;
        const HRESULT hr = CreateAccountWorkaround(inner, loginName, &u);
        return WrapUser(hr, u, user);
    }
    HRESULT STDMETHODCALLTYPE remove(VARIANT id, VARIANT backupPath, VARIANT_BOOL* result) override {
        if (!g_blockAccountRemoval.load()) {
            Wh_Log(L"[remove] protection disabled; invoking native account implementation");
            return inner->remove(id, backupPath, result);
        }
        // Fail closed: never invoke the potentially hanging private deletion method.
        // This is not a timeout or a claim that the account was removed.
        Wh_Log(L"[remove] blocked native deletion; no deletion performed by this call");
        if (!result) return E_POINTER;
        *result = VARIANT_FALSE;
        // Follow the mod's language setting; automatic uses the Windows UI language.
        const wchar_t* message;
        switch (CurrentLanguage()) {
            case Language::Italian:
                message = L"Per sicurezza, la rimozione dalla pagina classica è disattivata per evitare eliminazioni ambigue. Questa richiesta non ha eliminato alcun account. Usa Impostazioni > Account > Altri utenti. Prima verifica se un tentativo precedente ha già eliminato l'account ed esegui un backup dei file importanti: Impostazioni può eliminare anche i dati.";
                break;
            case Language::French:
                message = L"Par sécurité, la suppression depuis la page classique est désactivée afin d'éviter toute suppression ambiguë. Cette demande n'a supprimé aucun compte. Utilisez Paramètres > Comptes > Autres utilisateurs. Vérifiez d'abord si une tentative précédente a déjà supprimé le compte et sauvegardez les fichiers importants : les Paramètres peuvent aussi supprimer les données.";
                break;
            case Language::Spanish:
                message = L"Por seguridad, la eliminación desde la página clásica está desactivada para evitar eliminaciones ambiguas. Esta solicitud no ha eliminado ninguna cuenta. Usa Configuración > Cuentas > Otros usuarios. Comprueba primero si un intento anterior ya eliminó la cuenta y haz una copia de los archivos importantes: Configuración también puede eliminar los datos.";
                break;
            case Language::Portuguese:
                message = L"Por segurança, a remoção na página clássica está desativada para evitar eliminações ambíguas. Este pedido não eliminou nenhuma conta. Usa Definições > Contas > Outros utilizadores. Verifica primeiro se uma tentativa anterior já eliminou a conta e guarda uma cópia dos ficheiros importantes: as Definições também podem eliminar os dados.";
                break;
            case Language::German:
                message = L"Aus Sicherheitsgründen ist das Entfernen über die klassische Seite deaktiviert, um mehrdeutige Löschvorgänge zu vermeiden. Durch diese Anfrage wurde kein Konto gelöscht. Verwende Einstellungen > Konten > Andere Benutzer. Prüfe zunächst, ob ein früherer Versuch das Konto bereits gelöscht hat, und sichere wichtige Dateien: Die Einstellungen können auch Daten löschen.";
                break;
            default:
                message = L"For safety, removal from the classic page is disabled to avoid ambiguous deletion. This request did not delete an account. Use Settings > Accounts > Other users. First check whether an earlier attempt already deleted the account and back up important files: Settings can also delete account data.";
                break;
        }
        MessageBoxW(GetActiveWindow(), message, L"Windows 7 User Account Page Restorer", MB_OK | MB_ICONWARNING);
        return E_NOTIMPL;
    }
    HRESULT STDMETHODCALLTYPE Advise(IUnknown*, DWORD* cookie) override {
        // Change notifications (for the Windows 7 Welcome screen) are gone from Windows 10. The page refreshes its
        // list itself after every change it makes.
        if (cookie) *cookie = 1;
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE Unadvise(DWORD) override { return S_OK; }
    HRESULT STDMETHODCALLTYPE ResetList() override { return inner->ResetList(); }
    HRESULT STDMETHODCALLTYPE get_currentUserWithImpersonation(IUser7** user) override {
        IUser10* u = nullptr;
        const HRESULT hr = inner->get_currentUserWithImpersonation(&u);
        return WrapUser(hr, u, user, true);
    }
};

// Wraps a Windows 10 ILogonEnumUsers (reference taken over) and answers `iid` (the Windows 7 id).
HRESULT WrapEnum(IEnum10* e, REFIID iid, void** object) {
    EnumAdapter* adapter = new (std::nothrow) EnumAdapter(e);
    if (!adapter) {
        e->Release();
        return E_OUTOFMEMORY;
    }
    const HRESULT hr = adapter->QueryInterface(iid, object);
    adapter->Release();
    return hr;
}

// The elevation factory: the Windows 7 page asks it for the two inner objects when a page needs administrator
// rights. The system's factory (and its prompt) is used; only the account enumerator it returns is wrapped.
struct FactoryAdapter final : IFactory7, LiveObject {
    std::atomic<LONG> refs{1};
    IFactory10* inner;      // owned
    explicit FactoryAdapter(IFactory10* f) : inner(f) {}
    ~FactoryAdapter() { inner->Release(); }

    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid, void** object) override {
        if (!object) return E_POINTER;
        if (Same(iid, kIidUnknown) || Same(iid, kIidFactory7)) {
            *object = static_cast<IFactory7*>(this);
            AddRef();
            return S_OK;
        }
        *object = nullptr;
        return E_NOINTERFACE;
    }
    ULONG STDMETHODCALLTYPE AddRef() override { return ULONG(++refs); }
    ULONG STDMETHODCALLTYPE Release() override {
        const LONG left = --refs;
        if (!left) delete this;
        return ULONG(left);
    }
    HRESULT STDMETHODCALLTYPE Activate(HWND owner, REFGUID elevationClass) override {
        const HRESULT hr = inner->Activate(owner, elevationClass);
        return hr;
    }
    HRESULT STDMETHODCALLTYPE CreateObject(REFCLSID clsid, REFIID iid, void** object) override {
        if (!object) return E_POINTER;
        if (Same(clsid, kClsidLogonEnumUsers) && Same(iid, kIidEnum7)) {
            *object = nullptr;
            IEnum10* e = nullptr;
            const HRESULT hr = inner->CreateObject(clsid, kIidEnum10, reinterpret_cast<void**>(&e));
            if (FAILED(hr) || !e) return FAILED(hr) ? hr : E_NOINTERFACE;
            return WrapEnum(e, iid, object);
        }
        const HRESULT hr = inner->CreateObject(clsid, iid, object);
        return hr;
    }
};

// =====================================================================================================================
// Hooks (explorer.exe)
// =====================================================================================================================

using PSPropertyBag_ReadStr_t = HRESULT(WINAPI*)(IPropertyBag*, LPCWSTR, LPWSTR, int);
PSPropertyBag_ReadStr_t PSPropertyBag_ReadStr_Orig;

// True if `value` (as read from the registry, possibly with environment variables) names System32\usercpl.dll.
bool IsSystemUserCpl(LPCWSTR value) {
    wchar_t expanded[MAX_PATH], system[MAX_PATH];
    const DWORD n = ExpandEnvironmentStringsW(value, expanded, ARRAYSIZE(expanded));
    if (!n || n > ARRAYSIZE(expanded)) return false;
    const UINT s = GetSystemDirectoryW(system, ARRAYSIZE(system));
    if (!s || s + 13 > ARRAYSIZE(system)) return false;
    wcscat(system, L"\\usercpl.dll");
    return _wcsicmp(expanded, system) == 0;
}

// Control Panel builds a page from the "ResourceDLL" and "ResourceID" of its registration (CLayoutFolder::Load in
// shell32). For the User Accounts page the DLL becomes the private copy; everything else is left as read.
HRESULT WINAPI PSPropertyBag_ReadStr_Hook(IPropertyBag* bag, LPCWSTR name, LPWSTR value, int count) {
    const HRESULT hr = PSPropertyBag_ReadStr_Orig(bag, name, value, count);
    // A hook runs inside Explorer: whatever goes wrong here, the value as Windows read it is the answer.
    try {
        if (SUCCEEDED(hr) && name && value && count > 0 && !g_stopping.load() && _wcsicmp(name, L"ResourceDLL") == 0 &&
            IsSystemUserCpl(value)) {
            // The same test as CoCreateInstance: the private page is handed out only once it is loaded. If it cannot
            // be loaded, Windows' own page is used.
            if (EnsurePageLoaded()) {
                std::wstring page;
                {
                    SharedLock lock(g_stateLock);
                    page = g_pagePath;
                }
                if (!page.empty() && page.size() < size_t(count) &&
                    GetFileAttributesW(page.c_str()) != INVALID_FILE_ATTRIBUTES)
                    wcscpy(value, page.c_str());
            }
        }
    } catch (...) {
    }
    return hr;
}

using CoCreateInstance_t = HRESULT(WINAPI*)(REFCLSID, LPUNKNOWN, DWORD, REFIID, LPVOID*);
CoCreateInstance_t CoCreateInstance_Orig;

// Create a local account with net.exe, then use the adapter's existing
// enumerator (which may be the elevated factory proxy) to retrieve it.
HRESULT CreateAccountWorkaround(IEnum10* enumerator, BSTR loginName, IUser10** user) {
    if (user) *user = nullptr;
    if (!enumerator || !loginName || !user) return E_POINTER;
    const UINT len = SysStringLen(loginName);
    // Reject characters that cannot be safely passed as one quoted argument.
    bool valid = len >= 1 && len <= 20 && loginName[len - 1] != L'.';
    for (UINT i = 0; valid && i < len; i++) {
        const wchar_t c = loginName[i];
        if (c < 0x20 || wcschr(L"\"/\\[]:;|=,+*?<>", c)) valid = false;
    }
    if (!valid) return HRESULT_FROM_WIN32(ERROR_INVALID_NAME);

    wchar_t sysdir[MAX_PATH] = L"";
    const UINT syslen = GetSystemDirectoryW(sysdir, ARRAYSIZE(sysdir));
    if (!syslen || syslen >= ARRAYSIZE(sysdir)) return E_FAIL;
    const std::wstring exe = std::wstring(sysdir) + L"\\net.exe";
    const std::wstring params = L"user \"" + std::wstring(loginName, len) + L"\" /add";
    SHELLEXECUTEINFOW sei{};
    sei.cbSize = sizeof(sei);
    sei.fMask = SEE_MASK_NOCLOSEPROCESS | SEE_MASK_NOASYNC | SEE_MASK_FLAG_NO_UI;
    sei.lpVerb = L"runas";
    sei.lpFile = exe.c_str();
    sei.lpParameters = params.c_str();
    sei.nShow = SW_HIDE;
    if (!ShellExecuteExW(&sei)) {
        const DWORD err = GetLastError();
        Wh_Log(L"[create] could not start the elevated net.exe (%lu)", err);
        return err ? HRESULT_FROM_WIN32(err) : E_FAIL;
    }
    if (!sei.hProcess) {
        Wh_Log(L"[create] net.exe launched without a process handle; outcome unknown");
        return E_FAIL;
    }
    DWORD wait = WaitForSingleObject(sei.hProcess, 120000);
    DWORD exitCode = 1;
    const BOOL gotExitCode = wait == WAIT_OBJECT_0 && GetExitCodeProcess(sei.hProcess, &exitCode);
    CloseHandle(sei.hProcess);
    Wh_Log(L"[create] net.exe user /add finished: wait=%lu exit=%lu", wait, exitCode);
    if (wait == WAIT_TIMEOUT) {
        // The child may still create the account later; do not report a duplicate.
        Wh_Log(L"[create] net.exe timed out; account creation may still complete");
        return HRESULT_FROM_WIN32(WAIT_TIMEOUT);
    }
    if (wait != WAIT_OBJECT_0 || !gotExitCode || exitCode != 0) return E_FAIL;

    HRESULT hr = enumerator->ResetList();
    if (FAILED(hr)) {
        Wh_Log(L"[create] account created but enumerator reset failed: 0x%08lX", hr);
        return hr;
    }
    VARIANT id;
    VariantInit(&id);
    id.vt = VT_BSTR;
    id.bstrVal = loginName;  // borrowed, not freed
    hr = enumerator->item(id, user);
    Wh_Log(L"[create] account created; reading it back -> 0x%08lX", hr);
    if (SUCCEEDED(hr) && !*user) hr = E_FAIL;
    return hr;
}

HRESULT CreatePageObject(const GUID& systemClass, LPUNKNOWN outer, REFIID iid, LPVOID* object) {
    const DllGetClassObject_t getClassObject = EnsurePageLoaded();
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
    if ((first == kClsidProviderSys.Data1 || first == kClsidInitSys.Data1 || first == kClsidLogonEnumUsers.Data1 ||
         first == kClsidElevationFactory.Data1) && object && !g_stopping.load()) {
        // The two classes of the page definition: from the private copy, but only once it is ready (the same moment
        // PSPropertyBag_ReadStr starts to hand out its path); before that the system's page and classes are in use.
        if (Same(clsid, kClsidProviderSys) || Same(clsid, kClsidInitSys)) {
            // The same test as PSPropertyBag_ReadStr: redirected only once the private page is loaded. Otherwise, or
            // if the class cannot be created from it, Windows' own class (the same CLSIDs) is used.
            if (EnsurePageLoaded()) {
                const HRESULT hr = CreatePageObject(clsid, outer, iid, object);
                if (SUCCEEDED(hr)) return hr;
                Wh_Log(L"The page class could not be created (0x%08lX); Windows' own class is used", hr);
            }
            return CoCreateInstance_Orig(clsid, outer, context, iid, object);
        }
        // The Windows 7 interface ids are asked for by the Windows 7 page only.
        if (Same(clsid, kClsidLogonEnumUsers) && Same(iid, kIidEnum7)) {
            *object = nullptr;
            IEnum10* e = nullptr;
            const HRESULT hr = CoCreateInstance_Orig(clsid, nullptr, context, kIidEnum10, reinterpret_cast<void**>(&e));
            if (FAILED(hr) || !e) return FAILED(hr) ? hr : E_NOINTERFACE;
            return WrapEnum(e, iid, object);
        }
        if (Same(clsid, kClsidElevationFactory) && Same(iid, kIidFactory7)) {
            *object = nullptr;
            IFactory10* f = nullptr;
            const HRESULT hr = CoCreateInstance_Orig(clsid, nullptr, context, kIidFactory10, reinterpret_cast<void**>(&f));
            if (FAILED(hr) || !f) return FAILED(hr) ? hr : E_NOINTERFACE;
            FactoryAdapter* adapter = new (std::nothrow) FactoryAdapter(f);
            if (!adapter) {
                f->Release();
                return E_OUTOFMEMORY;
            }
            *object = static_cast<IFactory7*>(adapter);
            return S_OK;
        }
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
    if (!g_stopEvent) g_stopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (g_stopEvent) ResetEvent(g_stopEvent);
    // Download, verification and generation happen on a worker thread: Explorer never waits for them. Until they are
    // done (or if they fail) the hooks change nothing.
    StartSetup();
    return TRUE;
}

BOOL Wh_ModSettingsChanged(BOOL* reload) {
    *reload = FALSE;
    LoadSettings();
    StartSetup();
    return TRUE;
}

void Wh_ModBeforeUninit() {
    g_stopping.store(true);
    if (g_stopEvent) SetEvent(g_stopEvent);
    CancelDownload();
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
    // Nothing outside the mod may point into it when this function returns.
    RestorePageImports();
    RundownLiveObjects();
    try {
        std::wstring storage;
        {
            ExclusiveLock lock(g_stateLock);
            storage = g_storage;
            if (!g_pageModule) {
                g_pagePath.clear();
                g_pageSha.clear();
            }
        }
        if (!g_keepFiles.load()) {
            if (storage.empty()) storage = StorageDir();
            if (!storage.empty()) {
                // Files that are loaded (here or in another Explorer) cannot be deleted and stay until a later unload.
                DeleteFilesIn(storage + L"\\bin");
                DeleteFilesIn(storage + L"\\dl");
            }
        }
    } catch (...) {
        Wh_Log(L"Wh_ModUninit: unexpected exception");
    }
}

#endif  // W7UA_GENERATOR_ONLY
