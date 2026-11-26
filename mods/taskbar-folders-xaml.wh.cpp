// ==WindhawkMod==
// @id              taskbar-folders-xaml
// @name            Windows 11 Taskbar Folders
// @name:it-IT      Cartelle nella barra delle applicazioni di Windows 11
// @name:es-ES      Carpetas en la barra de tareas
// @name:fr-FR      Dossiers dans la barre des tâches
// @name:de-DE      Ordner in der Taskleiste
// @description     Group apps in folders on your Windows 11 taskbar. Click a folder to see and open its apps.
// @description:it-IT Raggruppa le app in cartelle sulla barra delle applicazioni di Windows 11. Apri una cartella e fai clic sull’app che vuoi avviare.
// @description:es-ES Agrupa aplicaciones en carpetas en la barra de tareas de Windows 11. Abre una carpeta y haz clic en la aplicación que quieras iniciar.
// @description:fr-FR Regroupez vos applications dans des dossiers sur la barre des tâches de Windows 11. Ouvrez un dossier et cliquez sur une application pour la lancer.
// @description:de-DE Gruppieren Sie Apps in Ordnern auf der Windows-11-Taskleiste. Öffnen Sie einen Ordner und klicken Sie auf die gewünschte App.
// @version         1.0
// @author          mamt104
// @github          https://github.com/mamt104
// @license         MIT
// @include         explorer.exe
// @include         ShellExperienceHost.exe
// @include         ShellHost.exe
// @include         ShellAppRuntime.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -loleacc -lruntimeobject -lshell32 -lshlwapi -luuid -lgdi32 -lcomctl32 -lcomdlg32 -ldwmapi -lmsimg32 -ladvapi32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Windows 11 Taskbar Folders

Keep apps you use together under one icon on your taskbar. Click a folder to see what’s inside, then click an app to open it.

![An open folder with Notepad, Character Map and Task Manager](https://raw.githubusercontent.com/mamt104/windhawk-mods/94fc1a4a8a3459191976ffa62a93263b4537df9c/windows-11-taskbar-folders-preview.png)

1. Right-click an empty spot on the taskbar and choose **Create folder…**.
2. Drag shortcuts from your desktop or File Explorer onto the folder. For an app already on the taskbar, right-click its icon and choose **Move to** with the folder name.
3. Drag apps to change their order. Hold the first app, click others, then drag them together. Drop an app on the desktop to put a shortcut there.

In Windhawk settings, choose a folder design and place folders on either side of Start. Open apps keep their taskbar buttons and show a dot inside the folder.

![A folder to the left of Start with a thin divider between them](https://raw.githubusercontent.com/mamt104/windhawk-mods/94fc1a4a8a3459191976ffa62a93263b4537df9c/taskbar-folders-start.png)

Folders appear on the main taskbar. Each Windows account has its own layout and copies of imported shortcuts. To pin a folder among your apps, choose **Pin to taskbar** in its menu and confirm in File Explorer. If you disable the mod, Windows keeps that pin. You can remove it with **Unpin from taskbar**.

Developed together by mamt104 and ChatGPT (Codex). mamt104 designed the interactions, tested on Windows and reported the bugs that guided the fixes.

### Italiano

Riunisci le app che usi insieme sotto una sola icona. Fai clic destro su uno spazio vuoto della barra e scegli **Crea cartella…**. Trascinaci i collegamenti dal desktop o da Esplora file. Per un’app già sulla barra, fai clic destro sulla sua icona e scegli **Sposta in** con il nome della cartella. Apri la cartella e fai clic su un’app per avviarla.

Trascina le app per riordinarle. Per spostarne più di una, tieni premuta la prima e fai clic sulle altre. Trascina un’app sul desktop per crearvi un collegamento. Nelle impostazioni puoi scegliere lo stile e mettere le cartelle a sinistra o a destra di Start. Le app aperte mantengono il proprio pulsante sulla barra e hanno un puntino nella cartella.

Le cartelle compaiono sulla barra principale. Ogni account Windows conserva il proprio ordine e una copia dei collegamenti importati. Per fissare una cartella tra le app, scegli **Integra nella barra** e conferma in Esplora file. Se disattivi la mod, puoi rimuovere l’icona fissata dal menu di Windows.

Progetto sviluppato insieme da mamt104 e ChatGPT (Codex). mamt104 ha progettato le interazioni, provato la mod su Windows e segnalato i bug che hanno guidato le correzioni.

### Español

Reúne las aplicaciones que usas juntas en un solo icono. Haz clic derecho en un espacio vacío de la barra y elige **Crear carpeta…**. Arrastra accesos directos desde el escritorio o el Explorador de archivos. Para una aplicación que ya está en la barra, haz clic derecho en su icono y elige **Mover a** con el nombre de la carpeta. Abre la carpeta y haz clic en una aplicación para iniciarla.

Arrastra las aplicaciones para ordenarlas. Para mover varias, mantén pulsada la primera y haz clic en las demás. Arrastra una al escritorio para crear un acceso directo. En los ajustes puedes elegir el diseño y colocar las carpetas a la izquierda o a la derecha de Inicio. Las aplicaciones abiertas mantienen su botón en la barra y muestran un punto en la carpeta.

Las carpetas aparecen en la barra principal. Cada cuenta de Windows guarda su propia distribución y copias de los accesos directos importados. Para anclar una carpeta entre las aplicaciones, elige **Anclar a la barra de tareas** y confirma en el Explorador. Si desactivas el mod, puedes desanclar el icono desde el menú de Windows.

Proyecto desarrollado por mamt104 y ChatGPT (Codex). mamt104 diseñó las interacciones, probó la mod en Windows y comunicó los fallos que guiaron las correcciones.

### Français

Regroupez les applications que vous utilisez ensemble sous une seule icône. Faites un clic droit sur un espace vide de la barre des tâches et choisissez **Créer un dossier…**. Glissez-y des raccourcis depuis le bureau ou l’Explorateur de fichiers. Pour une application déjà dans la barre, faites un clic droit sur son icône et choisissez **Déplacer vers** avec le nom du dossier. Ouvrez le dossier et cliquez sur une application pour la lancer.

Faites glisser les applications pour les réorganiser. Pour en déplacer plusieurs, maintenez le bouton enfoncé sur la première, puis cliquez sur les autres. Glissez une application sur le bureau pour y placer un raccourci. Les paramètres permettent de choisir l’apparence des dossiers et de les placer de part et d’autre de Démarrer. Une application ouverte conserve son bouton dans la barre et porte un point dans le dossier.

Les dossiers apparaissent sur la barre principale. Chaque compte Windows conserve sa propre disposition et une copie des raccourcis importés. Pour épingler un dossier parmi les applications, choisissez **Épingler à la barre des tâches** et confirmez dans l’Explorateur. Si vous désactivez le mod, vous pouvez détacher l’icône depuis le menu Windows.

Projet développé par mamt104 et ChatGPT (Codex). mamt104 a conçu les interactions, testé le mod sous Windows et signalé les bugs qui ont guidé les corrections.

### Deutsch

Fassen Sie häufig gemeinsam genutzte Apps unter einem Symbol zusammen. Klicken Sie mit der rechten Maustaste auf eine freie Stelle der Taskleiste und wählen Sie **Ordner erstellen…**. Ziehen Sie Verknüpfungen vom Desktop oder aus dem Datei-Explorer auf den Ordner. Ist eine App bereits in der Taskleiste, klicken Sie mit der rechten Maustaste auf ihr Symbol und wählen Sie den Eintrag mit dem gewünschten Ordnernamen. Öffnen Sie den Ordner und klicken Sie auf eine App, um sie zu starten.

Ziehen Sie Apps, um sie zu sortieren. Um mehrere zu verschieben, halten Sie die erste kurz gedrückt und klicken Sie dann auf die weiteren Apps. Ziehen Sie eine App auf den Desktop, um dort eine Verknüpfung anzulegen. In den Einstellungen wählen Sie das Design und die Position links oder rechts von Start. Geöffnete Apps behalten ihre Schaltfläche in der Taskleiste und sind im Ordner mit einem Punkt markiert.

Die Ordner erscheinen in der Haupttaskleiste. Jedes Windows-Konto speichert seine eigene Anordnung und Kopien der importierten Verknüpfungen. Um einen Ordner zwischen den Apps anzuheften, wählen Sie **An Taskleiste anheften** und bestätigen Sie im Explorer. Wenn Sie die Mod deaktivieren, können Sie das angeheftete Symbol über das Windows-Menü entfernen.

Gemeinsam entwickelt von mamt104 und ChatGPT (Codex). mamt104 hat die Bedienung entworfen, die Mod unter Windows getestet und Fehler gemeldet, die anschließend behoben wurden.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- runningAppClick: launch
  $name: Click on an already open app
  $options:
  - launch: Launch the app as usual
  - activate: Switch to its foremost window
  $name:it-IT: Clic su un’app già aperta
  $options:it-IT:
  - launch: Avvia l’app come al solito
  - activate: Porta in primo piano la sua finestra più recente
  $name:es-ES: Clic en una aplicación abierta
  $options:es-ES:
  - launch: Iniciar la aplicación como siempre
  - activate: Mostrar su ventana en primer plano
  $name:fr-FR: Clic sur une application ouverte
  $options:fr-FR:
  - launch: Lancer l’application comme d’habitude
  - activate: Afficher sa fenêtre de premier plan
  $name:de-DE: Klick auf eine bereits geöffnete App
  $options:de-DE:
  - launch: App wie gewohnt starten
  - activate: Zum vordersten Fenster der App wechseln
- folderPlacement: normal
  $name: Folder position on the taskbar
  $options:
  - normal: Normal taskbar order
  - left: Immediately left of Start
  - right: Immediately right of Start
  $name:it-IT: Posizione delle cartelle sulla barra
  $options:it-IT:
  - normal: Ordine normale della barra
  - left: Subito a sinistra di Start
  - right: Subito a destra di Start
  $name:es-ES: Posición de las carpetas en la barra
  $options:es-ES:
  - normal: Orden normal de la barra
  - left: A la izquierda de Inicio
  - right: A la derecha de Inicio
  $name:fr-FR: Position des dossiers dans la barre
  $options:fr-FR:
  - normal: Ordre habituel de la barre
  - left: À gauche de Démarrer
  - right: À droite de Démarrer
  $name:de-DE: Ordnerposition auf der Taskleiste
  $options:de-DE:
  - normal: Normale Reihenfolge der Taskleiste
  - left: Links neben Start
  - right: Rechts neben Start
- panelPreset: windows
  $name: Folder design
  $description: "Default design for folders. Open a folder and choose Appearance from its options menu to override it."
  $options:
  - windows: Windows
  - prism: Prism
  - nord: Nord
  - mocha: Mocha
  - oled: Midnight OLED
  - paper: Paper
  $name:it-IT: Design delle cartelle
  $description:it-IT: "Design predefinito. Dal menu Aspetto di ogni cartella puoi sceglierne uno diverso."
  $options:it-IT:
  - windows: Windows
  - prism: Prisma
  - nord: Nord
  - mocha: Mocha
  - oled: Mezzanotte OLED
  - paper: Carta
  $name:es-ES: Diseño de las carpetas
  $description:es-ES: "Diseño predeterminado. Puedes cambiarlo en el menú Apariencia de cada carpeta."
  $options:es-ES:
  - windows: Windows
  - prism: Prisma
  - nord: Nord
  - mocha: Mocha
  - oled: Medianoche OLED
  - paper: Papel
  $name:fr-FR: Style des dossiers
  $description:fr-FR: "Style par défaut. Le menu Apparence de chaque dossier permet de le personnaliser."
  $options:fr-FR:
  - windows: Windows
  - prism: Prisme
  - nord: Nord
  - mocha: Mocha
  - oled: Minuit OLED
  - paper: Papier
  $name:de-DE: Ordnerdesign
  $description:de-DE: "Standarddesign. Im Menü Darstellung lässt sich das Design jedes Ordners ändern."
  $options:de-DE:
  - windows: Windows
  - prism: Prisma
  - nord: Nord
  - mocha: Mocha
  - oled: Mitternacht OLED
  - paper: Papier
- panelAccent: ""
  $name: Accent color
  $description: "Selection follows the Windows accent. Other details use the folder design. Enter a color such as #76B5F5 to override both."
  $name:it-IT: Colore di accento
  $description:it-IT: "La selezione segue il colore di Windows. Gli altri dettagli usano il colore del design. Inserisci un colore come #76B5F5 per personalizzare entrambi."
  $name:es-ES: Color de énfasis
  $description:es-ES: "La selección usa el color de Windows. Los demás detalles usan el color del diseño. Introduce un color como #76B5F5 para personalizar ambos."
  $name:fr-FR: Couleur d’accentuation
  $description:fr-FR: "La sélection suit la couleur de Windows. Les autres détails utilisent celle du style. Saisissez une couleur comme #76B5F5 pour personnaliser les deux."
  $name:de-DE: Akzentfarbe
  $description:de-DE: "Die Auswahl verwendet die Windows-Akzentfarbe. Andere Details folgen dem Ordnerdesign. Mit einer Farbe wie #76B5F5 lassen sich beide anpassen."
*/
// ==/WindhawkModSettings==

#include <appmodel.h>
#include <commctrl.h>
#include <commdlg.h>
#include <dwmapi.h>
#include <exdisp.h>
#include <ole2.h>
#include <oleacc.h>
#include <propkey.h>
#include <robuffer.h>
#include <sddl.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <shobjidl.h>
#include <windhawk_utils.h>
#include <windowsx.h>
#include <wrl/client.h>
#include <array>
#include <chrono>
#include <cmath>
#include <condition_variable>
#include <thread>
#undef GetCurrentTime
#include <strsafe.h>
#include <winrt/Windows.ApplicationModel.DataTransfer.h>
#include <winrt/Windows.Data.Json.h>
#include <winrt/Windows.Devices.Input.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Storage.Streams.h>
#include <winrt/Windows.Storage.h>
#include <winrt/Windows.UI.Composition.h>
#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.UI.Input.h>
#include <winrt/Windows.UI.ViewManagement.h>
#include <winrt/Windows.UI.Xaml.Automation.h>
#include <winrt/Windows.UI.Xaml.Controls.Primitives.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Data.h>
#include <winrt/Windows.UI.Xaml.Hosting.h>
#include <winrt/Windows.UI.Xaml.Input.h>
#include <winrt/Windows.UI.Xaml.Markup.h>
#include <winrt/Windows.UI.Xaml.Media.Imaging.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.h>
#include <algorithm>
#include <atomic>
#include <deque>
#include <filesystem>
#include <fstream>
#include <functional>
#include <limits>
#include <map>
#include <memory>
#include <mutex>
#include <optional>
#include <set>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace fs = std::filesystem;
namespace W = winrt::Windows;
namespace X = W::UI::Xaml;
namespace C = X::Controls;
namespace M = X::Media;
namespace I = X::Input;
namespace F = W::Foundation;
namespace J = W::Data::Json;
namespace Data = W::ApplicationModel::DataTransfer;
using C::Button;
using C::TextBlock;
using std::wstring;
using winrt::com_ptr;
using winrt::hstring;
using X::FrameworkElement;

enum class UiLanguage : size_t { English, Italian, Spanish, French, German };
static std::optional<UiLanguage> languageForTag(std::wstring_view tag) {
    auto base = tag.substr(0, tag.find_first_of(L"-_"));
    if (base.size() != 2)
        return {};
    auto lower = [](wchar_t c) {
        return c >= L'A' && c <= L'Z' ? wchar_t(c + (L'a' - L'A')) : c;
    };
    const wchar_t a = lower(base[0]), b = lower(base[1]);
    if (a == L'e' && b == L'n')
        return UiLanguage::English;
    if (a == L'i' && b == L't')
        return UiLanguage::Italian;
    if (a == L'e' && b == L's')
        return UiLanguage::Spanish;
    if (a == L'f' && b == L'r')
        return UiLanguage::French;
    if (a == L'd' && b == L'e')
        return UiLanguage::German;
    return {};
}
static UiLanguage userLanguage() {
    ULONG count = 0, size = 0;
    if (GetUserPreferredUILanguages(MUI_LANGUAGE_NAME, &count, nullptr,
                                    &size) &&
        size) {
        std::vector<wchar_t> names(size);
        if (GetUserPreferredUILanguages(MUI_LANGUAGE_NAME, &count, names.data(),
                                        &size)) {
            for (PCWSTR p = names.data(); *p; p += wcslen(p) + 1)
                if (auto lang = languageForTag(p))
                    return *lang;
        }
    }
    wchar_t tag[LOCALE_NAME_MAX_LENGTH]{};
    if (LCIDToLocaleName(MAKELCID(GetUserDefaultUILanguage(), SORT_DEFAULT),
                         tag, ARRAYSIZE(tag), 0))
        if (auto lang = languageForTag(tag))
            return *lang;
    return UiLanguage::English;
}

enum class UiString : size_t {
    Title,
    ConfigUnreadable,
    StorageUnavailable,
    SaveFailed,
    Save,
    Cancel,
    PinUnavailable,
    PinInstructions,
    PinFailed,
    CreateTitle,
    DirUnavailable,
    DirEmpty,
    MultiSelect,
    ChooseApp,
    DirReadFailed,
    Places,
    Up,
    Go,
    FoldersApps,
    Add,
    ThisPc,
    Desktop,
    Downloads,
    PersonalStart,
    SharedStart,
    FromFile,
    AddAppsMenu,
    RenameMenu,
    CreateMenu,
    RemoveMenu,
    Integrate,
    MoveNamed,
    RenameTitle,
    RemovePrompt,
    RemoveApp,
    RemoveSelected,
    SearchInstalled,
    SearchHint,
    PreviousPage,
    NextPage,
    PageOf,
    AddTo,
    Back,
    Options,
    AddApps,
    Empty,
    TypeOrBrowse,
    Searching,
    NoCatalog,
    NoResults,
    SearchFailed,
    NoUsableFile,
    PickerFailed,
    BrokenShortcut,
    Apps,
    UnpinFirst,
    AppMissing,
    Appearance,
    FindInFolder,
    UndoMove,
    OpenFolder,
    MoreApps,
    DropDesktop,
    DropDirectory,
    ExportFailed,
    Count
};
static constexpr PCWSTR
    strings[]
           [5] = {
               {L"Taskbar Folders", L"Cartelle nella taskbar",
                L"Carpetas en la barra de tareas",
                L"Dossiers dans la barre des tâches",
                L"Ordner in der Taskleiste"},  // Title
               {L"Couldn’t read the folder configuration. Check these files "
                L"before changing your folders.\n\n{0}\n{0}.bak",
                L"Impossibile leggere la configurazione delle cartelle. "
                L"Controlla questi file prima di modificare le "
                L"cartelle.\n\n{0}\n{0}.bak",
                L"No se pudo leer la configuración de las carpetas. Revisa "
                L"estos archivos antes de modificar las "
                L"carpetas.\n\n{0}\n{0}.bak",
                L"Impossible de lire la configuration des dossiers. Vérifiez "
                L"ces fichiers avant de modifier vos dossiers.\n\n{0}\n{0}.bak",
                L"Die Ordnerkonfiguration konnte nicht gelesen werden. Prüfen "
                L"Sie diese Dateien, bevor Sie Ihre Ordner "
                L"ändern.\n\n{0}\n{0}.bak"},  // ConfigUnreadable
               {L"Windhawk’s mod storage is unavailable. Folder changes are "
                L"disabled.",
                L"L’archivio della mod di Windhawk non è disponibile. Le "
                L"modifiche alle cartelle sono disattivate.",
                L"El almacenamiento de la mod en Windhawk no está disponible. "
                L"Los cambios en las carpetas están desactivados.",
                L"Le stockage du mod dans Windhawk est indisponible. La "
                L"modification des dossiers est désactivée.",
                L"Der Mod-Speicher von Windhawk ist nicht verfügbar. "
                L"Ordneränderungen sind deaktiviert."},  // StorageUnavailable
               {L"Windows couldn’t save the change. Your previous folders are "
                L"still active.",
                L"Windows non ha salvato la modifica. Le cartelle precedenti "
                L"sono ancora attive.",
                L"Windows no ha podido guardar el cambio. Se conservan las "
                L"carpetas anteriores.",
                L"Windows n’a pas pu enregistrer la modification. Vos dossiers "
                L"précédents sont toujours disponibles.",
                L"Windows konnte die Änderung nicht speichern. Ihre bisherigen "
                L"Ordner sind weiterhin verfügbar."},  // SaveFailed
               {L"Save", L"Salva", L"Guardar", L"Enregistrer",
                L"Speichern"},  // Save
               {L"Cancel", L"Annulla", L"Cancelar", L"Annuler",
                L"Abbrechen"},  // Cancel
               {L"This version of Explorer doesn’t support native folder pins. "
                L"You can still open the folder from the taskbar.",
                L"Questa build di Explorer non espone il comando di avvio del "
                L"pin. La cartella resta accessibile dalla barra.",
                L"Esta versión del Explorador no admite el anclaje nativo. "
                L"Puedes seguir abriendo la carpeta desde la barra.",
                L"Cette version de l’Explorateur ne prend pas en charge "
                L"l’épinglage natif. Le dossier reste accessible dans la barre "
                L"des tâches.",
                L"Diese Explorer-Version unterstützt das native Anheften "
                L"nicht. Der Ordner lässt sich weiterhin über die Taskleiste "
                L"öffnen."},  // PinUnavailable
               {L"To pin “{0}”, right-click the selected shortcut in File "
                L"Explorer and choose “Pin to taskbar”.",
                L"Per fissare «{0}», fai clic destro sul collegamento "
                L"selezionato in Esplora file e scegli «Aggiungi alla barra "
                L"delle applicazioni».",
                L"Para anclar «{0}», haz clic derecho en el acceso directo "
                L"seleccionado en el Explorador y elige «Anclar a la barra de "
                L"tareas».",
                L"Pour épingler « {0} », faites un clic droit sur le raccourci "
                L"sélectionné dans l’Explorateur et choisissez « Épingler à la "
                L"barre des tâches ».",
                L"Um „{0}“ anzuheften, klicken Sie im Explorer mit der rechten "
                L"Maustaste auf die ausgewählte Verknüpfung und wählen Sie „An "
                L"Taskleiste anheften“."},  // PinInstructions
               {L"Windows couldn’t prepare the shortcut. Try again from the "
                L"folder menu.",
                L"Windows non ha completato la preparazione del pin. Puoi "
                L"riprovare dal menu della cartella.",
                L"Windows no ha podido preparar el acceso directo. Inténtalo "
                L"de nuevo desde el menú de la carpeta.",
                L"Windows n’a pas pu préparer le raccourci. Réessayez depuis "
                L"le menu du dossier.",
                L"Windows konnte die Verknüpfung nicht vorbereiten. Versuchen "
                L"Sie es über das Ordnermenü erneut."},  // PinFailed
               {L"Create a taskbar folder", L"Crea cartella nella taskbar",
                L"Crear una carpeta en la barra de tareas",
                L"Créer un dossier dans la barre des tâches",
                L"Taskleistenordner erstellen"},  // CreateTitle
               {L"Can’t open this folder. Check the path or choose another "
                L"location.",
                L"Cartella non accessibile. Controlla il percorso o scegli "
                L"un'altra posizione.",
                L"No se puede abrir esta carpeta. Comprueba la ruta o elige "
                L"otra ubicación.",
                L"Impossible d’ouvrir ce dossier. Vérifiez le chemin ou "
                L"choisissez un autre emplacement.",
                L"Der Ordner lässt sich nicht öffnen. Prüfen Sie den Pfad oder "
                L"wählen Sie einen anderen Speicherort."},  // DirUnavailable
               {L"No folders or apps here.",
                L"Nessuna cartella o app in questa posizione.",
                L"Aquí no hay carpetas ni aplicaciones.",
                L"Aucun dossier ni aucune application à cet emplacement.",
                L"Hier gibt es keine Ordner oder Apps."},  // DirEmpty
               {L"Select apps. Use Ctrl or Shift to select more than one.",
                L"Seleziona le app. Usa Ctrl o Maiusc per sceglierne più di "
                L"una.",
                L"Selecciona las aplicaciones. Usa Ctrl o Mayús para "
                L"seleccionar varias.",
                L"Sélectionnez les applications. Utilisez Ctrl ou Maj pour en "
                L"choisir plusieurs.",
                L"Wählen Sie Apps aus. Mit Strg oder Umschalt können Sie "
                L"mehrere auswählen."},  // MultiSelect
               {L"Select an app or open a folder.",
                L"Seleziona un'app o apri una cartella.",
                L"Selecciona una aplicación o abre una carpeta.",
                L"Sélectionnez une application ou ouvrez un dossier.",
                L"Wählen Sie eine App aus oder öffnen Sie einen Ordner."},  // ChooseApp
               {L"Couldn’t read this location.",
                L"Impossibile leggere questa posizione.",
                L"No se ha podido leer esta ubicación.",
                L"Impossible de lire cet emplacement.",
                L"Dieser Speicherort konnte nicht gelesen werden."},  // DirReadFailed
               {L"Locations", L"Posizioni", L"Ubicaciones", L"Emplacements",
                L"Speicherorte"},                             // Places
               {L"Up", L"Su", L"Subir", L"Monter", L"Hoch"},  // Up
               {L"Go", L"Vai", L"Ir", L"Aller", L"Los"},      // Go
               {L"Folders and apps", L"Cartelle e app",
                L"Carpetas y aplicaciones", L"Dossiers et applications",
                L"Ordner und Apps"},  // FoldersApps
               {L"Add", L"Aggiungi", L"Añadir", L"Ajouter",
                L"Hinzufügen"},  // Add
               {L"This PC", L"Questo PC", L"Este equipo", L"Ce PC",
                L"Dieser PC"},  // ThisPc
               {L"Desktop", L"Desktop", L"Escritorio", L"Bureau",
                L"Desktop"},  // Desktop
               {L"Downloads", L"Download", L"Descargas", L"Téléchargements",
                L"Downloads"},  // Downloads
               {L"My Start menu", L"Menu Start personale", L"Mi menú Inicio",
                L"Mon menu Démarrer", L"Mein Startmenü"},  // PersonalStart
               {L"Shared Start menu", L"Menu Start condiviso",
                L"Menú Inicio compartido", L"Menu Démarrer partagé",
                L"Gemeinsames Startmenü"},  // SharedStart
               {L"Choose from file", L"Scegli da file", L"Elegir un archivo",
                L"Choisir un fichier", L"Datei auswählen"},  // FromFile
               {L"Add apps…", L"Aggiungi app…", L"Añadir aplicaciones…",
                L"Ajouter des applications…",
                L"Apps hinzufügen…"},  // AddAppsMenu
               {L"Rename folder…", L"Rinomina cartella…",
                L"Cambiar nombre de carpeta…", L"Renommer le dossier…",
                L"Ordner umbenennen…"},  // RenameMenu
               {L"Create folder…", L"Crea cartella…", L"Crear carpeta…",
                L"Créer un dossier…", L"Ordner erstellen…"},  // CreateMenu
               {L"Delete folder…", L"Elimina cartella…", L"Eliminar carpeta…",
                L"Supprimer le dossier…", L"Ordner löschen…"},  // RemoveMenu
               {L"Pin to taskbar", L"Integra nella barra",
                L"Anclar a la barra de tareas",
                L"Épingler à la barre des tâches",
                L"An Taskleiste anheften"},  // Integrate
               {L"Move to “{0}”", L"Sposta in «{0}»", L"Mover a «{0}»",
                L"Déplacer vers « {0} »",
                L"Nach „{0}“ verschieben"},  // MoveNamed
               {L"Rename folder", L"Rinomina cartella",
                L"Cambiar nombre de carpeta", L"Renommer le dossier",
                L"Ordner umbenennen"},  // RenameTitle
               {L"Delete this folder? Its apps and their original shortcuts "
                L"will remain available.",
                L"Eliminare questa cartella? Le app e i loro collegamenti "
                L"originali resteranno disponibili.",
                L"¿Eliminar esta carpeta? Sus aplicaciones y sus accesos "
                L"directos originales seguirán disponibles.",
                L"Supprimer ce dossier ? Ses applications et leurs raccourcis "
                L"d’origine resteront disponibles.",
                L"Diesen Ordner löschen? Seine Apps und ihre ursprünglichen "
                L"Verknüpfungen bleiben verfügbar."},  // RemovePrompt
               {L"Remove from folder", L"Rimuovi dalla cartella",
                L"Quitar de la carpeta", L"Retirer du dossier",
                L"Aus Ordner entfernen"},  // RemoveApp
               {L"Remove {0} apps from folder",
                L"Rimuovi {0} app dalla cartella",
                L"Quitar {0} aplicaciones de la carpeta",
                L"Retirer {0} applications du dossier",
                L"{0} Apps aus dem Ordner entfernen"},
               {L"Search installed apps", L"Cerca app installate",
                L"Buscar aplicaciones instaladas",
                L"Rechercher des applications installées",
                L"Installierte Apps suchen"},  // SearchInstalled
               {L"Search for an installed app", L"Cerca un’app installata",
                L"Busca una aplicación instalada",
                L"Rechercher une application installée",
                L"Eine installierte App suchen"},  // SearchHint
               {L"Previous page", L"Pagina precedente", L"Página anterior",
                L"Page précédente", L"Vorherige Seite"},  // PreviousPage
               {L"Next page", L"Pagina successiva", L"Página siguiente",
                L"Page suivante", L"Nächste Seite"},  // NextPage
               {L"Page {0} of {1}", L"Pagina {0} di {1}", L"Página {0} de {1}",
                L"Page {0} sur {1}", L"Seite {0} von {1}"},  // PageOf
               {L"Add to {0}", L"Aggiungi a {0}", L"Añadir a {0}",
                L"Ajouter à {0}", L"Zu „{0}“ hinzufügen"},              // AddTo
               {L"Back", L"Indietro", L"Atrás", L"Retour", L"Zurück"},  // Back
               {L"More options", L"Altre opzioni", L"Más opciones",
                L"Autres options", L"Weitere Optionen"},  // Options
               {L"Add apps", L"Aggiungi app", L"Añadir aplicaciones",
                L"Ajouter des applications", L"Apps hinzufügen"},  // AddApps
               {L"Empty folder", L"Cartella vuota", L"Carpeta vacía",
                L"Dossier vide", L"Leerer Ordner"},  // Empty
               {L"Search for an app or choose a file",
                L"Cerca un’app o scegli un file",
                L"Busca una aplicación o elige un archivo",
                L"Recherchez une application ou choisissez un fichier",
                L"App suchen oder Datei auswählen"},  // TypeOrBrowse
               {L"Searching installed apps…", L"Ricerca delle app installate…",
                L"Buscando aplicaciones instaladas…",
                L"Recherche des applications installées…",
                L"Installierte Apps werden gesucht…"},  // Searching
               {L"App list unavailable. You can still choose a file.",
                L"Catalogo non disponibile. Puoi scegliere un file.",
                L"La lista de aplicaciones no está disponible. Puedes elegir "
                L"un archivo.",
                L"La liste des applications est indisponible. Vous pouvez "
                L"choisir un fichier.",
                L"Die App-Liste ist nicht verfügbar. Sie können eine Datei "
                L"auswählen."},  // NoCatalog
               {L"No apps found", L"Nessuna app trovata",
                L"No se han encontrado aplicaciones",
                L"Aucune application trouvée",
                L"Keine Apps gefunden"},  // NoResults
               {L"Couldn’t open app search.",
                L"Impossibile aprire la ricerca delle app.",
                L"No se ha podido abrir la búsqueda de aplicaciones.",
                L"Impossible d’ouvrir la recherche d’applications.",
                L"Die App-Suche konnte nicht geöffnet werden."},  // SearchFailed
               {
                   L"The selection contains no usable app or shortcut.",
                   L"La selezione non contiene un’app o un collegamento "
                   L"utilizzabile.",
                   L"La selección no contiene aplicaciones ni accesos directos "
                   L"válidos.",
                   L"La sélection ne contient aucune application ni aucun "
                   L"raccourci utilisable.",
                   L"Die Auswahl enthält keine verwendbare App oder "
                   L"Verknüpfung."},  // NoUsableFile
               {L"Windows couldn’t open the file picker.",
                L"Windows non ha potuto creare la finestra dei file.",
                L"Windows no ha podido abrir el selector de archivos.",
                L"Windows n’a pas pu ouvrir le sélecteur de fichiers.",
                L"Windows konnte die Dateiauswahl nicht öffnen."},  // PickerFailed
               {L"Shortcut unavailable. Check that the app is still installed.",
                L"Collegamento non disponibile. Controlla che l’app sia ancora "
                L"installata.",
                L"Acceso directo no disponible. Comprueba que la aplicación "
                L"sigue instalada.",
                L"Raccourci indisponible. Vérifiez que l’application est "
                L"toujours installée.",
                L"Verknüpfung nicht verfügbar. Prüfen Sie, ob die App noch "
                L"installiert ist."},  // BrokenShortcut
               {L"Applications", L"Applicazioni", L"Aplicaciones",
                L"Applications", L"Anwendungen"},  // Apps
               {L"Hold Shift and right-click the pin. Choose “Unpin from "
                L"taskbar”, then remove the folder from its three-dot menu.",
                L"Tieni premuto Maiusc e fai clic destro sul pin. Scegli "
                L"«Rimuovi dalla barra delle applicazioni», poi elimina la "
                L"cartella dai tre puntini.",
                L"Mantén pulsada Mayús y haz clic derecho en el icono. Elige "
                L"«Desanclar de la barra de tareas» y quita la carpeta desde "
                L"su menú de tres puntos.",
                L"Maintenez Maj enfoncée et faites un clic droit sur l’icône. "
                L"Choisissez « Détacher de la barre des tâches », puis "
                L"supprimez le dossier depuis son menu à trois points.",
                L"Halten Sie die Umschalttaste gedrückt und klicken Sie mit "
                L"der rechten Maustaste auf das Symbol. Wählen Sie „Von "
                L"Taskleiste lösen“ und entfernen Sie den Ordner anschließend "
                L"über sein Dreipunktmenü."},  // UnpinFirst
               {
                   L"Windows couldn’t find the shortcut for this app.",
                   L"Windows non ha trovato il collegamento di questa app.",
                   L"Windows no ha encontrado el acceso directo de esta "
                   L"aplicación.",
                   L"Windows n’a pas trouvé le raccourci de cette application.",
                   L"Windows konnte die Verknüpfung für diese App nicht "
                   L"finden."},  // AppMissing
               {L"Appearance", L"Aspetto", L"Apariencia", L"Apparence",
                L"Darstellung"},
               {L"Search this folder", L"Cerca in questa cartella",
                L"Buscar en esta carpeta", L"Rechercher dans ce dossier",
                L"Diesen Ordner durchsuchen"},
               {L"Undo move", L"Annulla spostamento", L"Deshacer movimiento",
                L"Annuler le déplacement", L"Verschieben rückgängig machen"},
               {L"Open folder", L"Apri cartella", L"Abrir carpeta",
                L"Ouvrir le dossier", L"Ordner öffnen"},
               {L"More apps", L"Altre app", L"Más aplicaciones",
                L"Autres applications", L"Weitere Apps"},
               {L"Place shortcut on desktop",
                L"Metti il collegamento sul desktop",
                L"Colocar acceso directo en el escritorio",
                L"Placer le raccourci sur le bureau",
                L"Verknüpfung auf dem Desktop ablegen"},
               {L"Place shortcut in this folder",
                L"Metti il collegamento in questa cartella",
                L"Colocar acceso directo en esta carpeta",
                L"Placer le raccourci dans ce dossier",
                L"Verknüpfung in diesem Ordner ablegen"},
               {L"Windows couldn’t place every shortcut here. The remaining "
                L"apps are still in your taskbar folder.",
                L"Windows non ha potuto salvare tutti i collegamenti qui. Le "
                L"app rimaste sono ancora nella cartella della barra.",
                L"Windows no pudo guardar todos los accesos directos aquí. Las "
                L"aplicaciones restantes siguen en la carpeta de la barra.",
                L"Windows n’a pas pu enregistrer tous les raccourcis ici. Les "
                L"applications restantes sont dans le dossier de la barre.",
                L"Windows konnte hier nicht alle Verknüpfungen ablegen. Die "
                L"übrigen Apps befinden sich weiterhin im Taskleistenordner."},
};
static_assert(std::size(strings) == static_cast<size_t>(UiString::Count));
static PCWSTR uiText(UiString id) {
    static const auto language = userLanguage();
    return strings[static_cast<size_t>(id)][static_cast<size_t>(language)];
}
static wstring formatUi(UiString id,
                        std::wstring_view first,
                        std::wstring_view second = {}) {
    std::wstring_view text = uiText(id);
    wstring out;
    out.reserve(text.size() + first.size() + second.size());
    for (size_t i = 0; i < text.size();) {
        if (text[i] == L'{' && i + 2 < text.size() && text[i + 2] == L'}' &&
            (text[i + 1] == L'0' || text[i + 1] == L'1')) {
            out.append(text[i + 1] == L'0' ? first : second);
            i += 3;
        } else
            out.push_back(text[i++]);
    }
    return out;
}

static bool equal(const wstring& a, const wstring& b) {
    return !_wcsicmp(a.c_str(), b.c_str());
}
static bool prefix(const wstring& s, const wchar_t* p) {
    return !_wcsnicmp(s.c_str(), p, wcslen(p));
}
static wstring guid() {
    GUID id{};
    winrt::check_hresult(CoCreateGuid(&id));
    wchar_t s[40];
    StringFromGUID2(id, s, 40);
    return s;
}
struct App {
    wstring id, name, target;
    bool groupTaskbarButton = true;
    wstring savedShortcut{};
    bool operator==(const App&) const = default;
};
enum class FolderStyle : uint32_t {
    Inherit,
    Windows,
    Prism,
    Nord,
    Mocha,
    Oled,
    Paper,
    Count
};
struct StyleLabel {
    PCWSTR key;
    std::array<PCWSTR, 5> names;
};
static constexpr StyleLabel styleLabels[] = {
    {L"inherit",
     {L"Use Windhawk setting", L"Usa le impostazioni di Windhawk",
      L"Usar el ajuste de Windhawk", L"Utiliser le réglage de Windhawk",
      L"Windhawk-Einstellung verwenden"}},
    {L"windows", {L"Windows", L"Windows", L"Windows", L"Windows", L"Windows"}},
    {L"prism", {L"Prism", L"Prisma", L"Prisma", L"Prisme", L"Prisma"}},
    {L"nord", {L"Nord", L"Nord", L"Nord", L"Nord", L"Nord"}},
    {L"mocha", {L"Mocha", L"Mocha", L"Mocha", L"Mocha", L"Mocha"}},
    {L"oled",
     {L"Midnight OLED", L"Mezzanotte OLED", L"Medianoche OLED", L"Minuit OLED",
      L"Mitternacht OLED"}},
    {L"paper", {L"Paper", L"Carta", L"Papel", L"Papier", L"Papier"}},
};
static_assert(std::size(styleLabels) ==
              static_cast<size_t>(FolderStyle::Count));
static FolderStyle parseStyle(std::wstring_view key, FolderStyle fallback) {
    for (size_t i = 0; i < std::size(styleLabels); ++i)
        if (key == styleLabels[i].key)
            return static_cast<FolderStyle>(i);
    return fallback;
}
static COLORREF parseAccent(std::wstring_view text) {
    if (text.starts_with(L'#'))
        text.remove_prefix(1);
    if (text.size() != 6)
        return CLR_INVALID;
    unsigned value = 0;
    for (wchar_t c : text) {
        unsigned digit;
        if (c >= L'0' && c <= L'9')
            digit = c - L'0';
        else if (c >= L'a' && c <= L'f')
            digit = c - L'a' + 10;
        else if (c >= L'A' && c <= L'F')
            digit = c - L'A' + 10;
        else
            return CLR_INVALID;
        value = (value << 4) | digit;
    }
    return RGB(value >> 16, (value >> 8) & 255, value & 255);
}
struct AppearanceSettings {
    FolderStyle preset = FolderStyle::Windows;
    COLORREF accent = CLR_INVALID;
    bool operator==(const AppearanceSettings&) const = default;
};
static std::atomic<AppearanceSettings> appearanceSettings{AppearanceSettings{}};
enum class FolderPlacement { Normal, LeftOfStart, RightOfStart };
static std::atomic<FolderPlacement> folderPlacement = FolderPlacement::Normal;
static std::atomic<bool> preferOpenWindow = false;
static void readAppearanceSettings() {
    auto position = WindhawkUtils::StringSetting::make(L"folderPlacement");
    folderPlacement.store(position.get() && equal(position.get(), L"left")
                              ? FolderPlacement::LeftOfStart
                          : position.get() && equal(position.get(), L"right")
                              ? FolderPlacement::RightOfStart
                              : FolderPlacement::Normal);
    auto click = WindhawkUtils::StringSetting::make(L"runningAppClick");
    preferOpenWindow.store(click.get() && equal(click.get(), L"activate"));
    auto preset = WindhawkUtils::StringSetting::make(L"panelPreset");
    auto accent = WindhawkUtils::StringSetting::make(L"panelAccent");
    FolderStyle style =
        parseStyle(preset.get() ? preset.get() : L"", FolderStyle::Windows);
    appearanceSettings.store(
        {style == FolderStyle::Inherit ? FolderStyle::Windows : style,
         parseAccent(accent.get() ? accent.get() : L"")});
}
struct AppFolder {
    wstring id, name;
    std::vector<App> apps;
    FolderStyle style = FolderStyle::Inherit;
    bool operator==(const AppFolder&) const = default;
};
using Model = std::vector<AppFolder>;
static Model groups;
static fs::path config, failedReadPath;
static bool loaded = false, locked = false;
static std::atomic<bool> stopping = false;
static void note(const wchar_t* s);
static AppFolder* lookup(Model& m, const wstring& id) {
    auto i =
        std::find_if(m.begin(), m.end(), [&](auto& f) { return f.id == id; });
    return i == m.end() ? nullptr : &*i;
}
static bool sameApp(const App& a, const App& b) {
    return (!a.id.empty() && !b.id.empty() && equal(a.id, b.id)) ||
           equal(a.target, b.target);
}
static void transfer(Model& m, const wstring& dest, const App& a) {
    if (!lookup(m, dest) || a.target.empty())
        return;
    for (auto& f : m)
        if (f.id != dest)
            std::erase_if(f.apps, [&](auto& old) { return sameApp(old, a); });
    auto& items = lookup(m, dest)->apps;
    auto i = std::find_if(items.begin(), items.end(),
                          [&](auto& old) { return sameApp(old, a); });
    if (i == items.end())
        items.push_back(a);
    else if ((i->id.empty() && !a.id.empty()) ||
             (!i->groupTaskbarButton && a.groupTaskbarButton)) {
        bool grouped = i->groupTaskbarButton || a.groupTaskbarButton;
        *i = a;
        i->groupTaskbarButton = grouped;
    }
}
static void removeApp(Model& model, const wstring& folder, const App& app) {
    if (auto f = lookup(model, folder))
        std::erase_if(f->apps,
                      [&](const auto& entry) { return sameApp(entry, app); });
}
static fs::path knownPath(REFKNOWNFOLDERID);
static bool validFolderId(const wstring&);
static bool shortcutExtension(const wstring& ext) {
    return equal(ext, L".lnk") || equal(ext, L".url") ||
           equal(ext, L".appref-ms");
}

static fs::path savedShortcutPath(const App& app) {
    return config.parent_path() / L"Shortcuts" /
           (app.savedShortcut + fs::path(app.target).extension().wstring());
}

static wstring launchTarget(const App& app) {
    if (!app.savedShortcut.empty()) {
        auto saved = savedShortcutPath(app);
        std::error_code ec;
        if (fs::is_regular_file(saved, ec))
            return saved.wstring();
    }
    return app.target;
}

static bool legacyGroupedShortcut(const wstring& target) {
    fs::path path(target);
    if (!path.is_absolute() || !equal(path.extension().wstring(), L".lnk"))
        return true;
    try {
        const auto directory = path.parent_path().lexically_normal().wstring();
        return !equal(
                   directory,
                   knownPath(FOLDERID_Desktop).lexically_normal().wstring()) &&
               !equal(directory, knownPath(FOLDERID_PublicDesktop)
                                     .lexically_normal()
                                     .wstring());
    } catch (...) {
        return true;
    }
}
static std::string encode(const Model& m) {
    J::JsonArray xs;
    for (auto& f : m) {
        J::JsonObject x;
        J::JsonArray items;
        x.SetNamedValue(L"id", J::JsonValue::CreateStringValue(f.id));
        x.SetNamedValue(L"name", J::JsonValue::CreateStringValue(f.name));
        if (f.style != FolderStyle::Inherit)
            x.SetNamedValue(L"panelStyle",
                            J::JsonValue::CreateStringValue(
                                styleLabels[static_cast<size_t>(f.style)].key));
        for (auto& a : f.apps) {
            J::JsonObject v;
            v.SetNamedValue(L"id", J::JsonValue::CreateStringValue(a.id));
            v.SetNamedValue(L"name", J::JsonValue::CreateStringValue(a.name));
            v.SetNamedValue(L"target",
                            J::JsonValue::CreateStringValue(a.target));
            v.SetNamedValue(
                L"groupTaskbarButton",
                J::JsonValue::CreateBooleanValue(a.groupTaskbarButton));
            if (!a.savedShortcut.empty())
                v.SetNamedValue(
                    L"savedShortcut",
                    J::JsonValue::CreateStringValue(a.savedShortcut));
            items.Append(v);
        }
        x.SetNamedValue(L"apps", items);
        xs.Append(x);
    }
    J::JsonObject doc;
    doc.SetNamedValue(L"version", J::JsonValue::CreateNumberValue(1));
    doc.SetNamedValue(L"folders", xs);
    return winrt::to_string(doc.Stringify());
}
static Model decode(const std::string& raw) {
    auto doc = J::JsonObject::Parse(winrt::to_hstring(raw));
    if (doc.GetNamedNumber(L"version") != 1)
        throw std::runtime_error("version");
    Model out;
    std::set<wstring> ids;
    for (auto entry : doc.GetNamedArray(L"folders")) {
        auto v = entry.GetObject();
        AppFolder f{wstring(v.GetNamedString(L"id")),
                    wstring(v.GetNamedString(L"name")),
                    {}};
        if (auto style = v.TryLookup(L"panelStyle");
            style && style.ValueType() == J::JsonValueType::String)
            f.style = parseStyle(std::wstring_view(style.GetString()),
                                 FolderStyle::Inherit);
        if (f.id.empty() || f.name.empty() || !ids.insert(f.id).second)
            throw std::runtime_error("folder");
        for (auto item : v.GetNamedArray(L"apps")) {
            auto a = item.GetObject();
            App app{wstring(a.GetNamedString(L"id")),
                    wstring(a.GetNamedString(L"name")),
                    wstring(a.GetNamedString(L"target"))};
            if (app.target.empty() || app.name.empty())
                throw std::runtime_error("item");
            app.groupTaskbarButton =
                a.HasKey(L"groupTaskbarButton")
                    ? a.GetNamedBoolean(L"groupTaskbarButton")
                    : legacyGroupedShortcut(app.target);
            if (auto saved = a.GetNamedString(L"savedShortcut", L"");
                !saved.empty()) {
                if (!validFolderId(wstring(saved)) ||
                    !shortcutExtension(
                        fs::path(app.target).extension().wstring()))
                    throw std::runtime_error("shortcut reference");
                app.savedShortcut = saved;
            }
            f.apps.push_back(app);
        }
        out.push_back(std::move(f));
    }
    return out;
}
static std::string bytes(const fs::path& path) {
    std::ifstream in(path, std::ios::binary);
    if (!in)
        throw std::runtime_error("open");
    return std::string(std::istreambuf_iterator<char>(in), {});
}
struct IgnoreCase {
    bool operator()(const wstring& a, const wstring& b) const {
        return _wcsicmp(a.c_str(), b.c_str()) < 0;
    }
};
struct AppIdentitySet {
    std::set<wstring, IgnoreCase> targets, ids;
    bool contains(const App& app) const {
        return targets.contains(app.target) ||
               (!app.id.empty() && ids.contains(app.id));
    }
    void add(const App& app) {
        targets.insert(app.target);
        if (!app.id.empty())
            ids.insert(app.id);
    }
};
using PinNames = std::map<wstring, wstring, IgnoreCase>;
static std::mutex pinNameLock;
static PinNames pinNames;
static PinNames indexPinNames(const Model&);
static void publishPinNames(PinNames next) {
    std::lock_guard lock(pinNameLock);
    pinNames.swap(next);
}
static wstring userSid() {
    winrt::handle token;
    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, token.put()))
        winrt::throw_last_error();

    DWORD size = 0;
    GetTokenInformation(token.get(), TokenUser, nullptr, 0, &size);
    if (GetLastError() != ERROR_INSUFFICIENT_BUFFER ||
        size < sizeof(TOKEN_USER))
        winrt::throw_last_error();

    std::vector<std::byte> data(size);
    if (!GetTokenInformation(token.get(), TokenUser, data.data(), size, &size))
        winrt::throw_last_error();
    auto user = reinterpret_cast<const TOKEN_USER*>(data.data());
    PWSTR sid = nullptr;
    if (!ConvertSidToStringSidW(user->User.Sid, &sid))
        winrt::throw_last_error();

    std::unique_ptr<wchar_t, decltype(&LocalFree)> sidText(sid, LocalFree);
    return sidText.get();
}

static void persist(const Model&);

// Capture shortcuts only when committing an import or upgrading an old layout.
// Keeping the original Shell file also retains browser profiles, icons and
// PIDLs.
struct ShortcutCopies {
    std::vector<fs::path> created;
    bool committed = false;
    ~ShortcutCopies() {
        if (!committed)
            for (const auto& path : created) {
                std::error_code ec;
                fs::remove(path, ec);
            }
    }
    void capture(Model& model, const Model* previous = nullptr) {
        std::optional<std::map<wstring, const App*, IgnoreCase>> existing;
        for (auto& folder : model)
            for (auto& app : folder.apps) {
                if (!app.savedShortcut.empty())
                    continue;
                fs::path source(app.target);
                if (!source.is_absolute() ||
                    !shortcutExtension(source.extension().wstring()))
                    continue;
                if (previous) {
                    if (!existing) {
                        existing.emplace();
                        for (const auto& f : *previous)
                            for (const auto& old : f.apps)
                                existing->try_emplace(old.target, &old);
                    }
                    if (auto it = existing->find(app.target);
                        it != existing->end() &&
                        !it->second->savedShortcut.empty()) {
                        app.savedShortcut = it->second->savedShortcut;
                        continue;
                    }
                }
                std::error_code ec;
                if (!fs::is_regular_file(source, ec))
                    continue;
                try {
                    auto id = guid();
                    auto directory = config.parent_path() / L"Shortcuts";
                    fs::create_directories(directory);
                    auto dest = directory / (id + source.extension().wstring());
                    if (!CopyFileW(source.c_str(), dest.c_str(), TRUE))
                        winrt::throw_last_error();
                    // The private copy is owned by the mod even if the source
                    // was read-only.
                    auto attrs = GetFileAttributesW(dest.c_str());
                    if (attrs != INVALID_FILE_ATTRIBUTES &&
                        (attrs & FILE_ATTRIBUTE_READONLY))
                        SetFileAttributesW(dest.c_str(),
                                           attrs & ~FILE_ATTRIBUTE_READONLY);
                    created.push_back(std::move(dest));
                    app.savedShortcut = std::move(id);
                } catch (...) {
                    if (previous)
                        throw;
                }
            }
    }
};

static void pruneSavedShortcuts() {
    // Keep the previous on-disk revision recoverable. Only run at cold start,
    // before undo records, previews or export workers can reference older
    // files.
    try {
        auto dir = config.parent_path() / L"Shortcuts";
        if (!fs::is_directory(dir))
            return;
        std::set<wstring, IgnoreCase> used;
        auto retain = [&](const Model& model) {
            for (const auto& f : model)
                for (const auto& a : f.apps)
                    if (!a.savedShortcut.empty())
                        used.insert(savedShortcutPath(a).filename().wstring());
        };
        retain(groups);
        auto backup = config;
        backup += L".bak";
        if (fs::exists(backup))
            retain(decode(bytes(backup)));
        for (const auto& item : fs::directory_iterator(dir)) {
            const auto& path = item.path();
            if (!validFolderId(path.stem().wstring()) ||
                !shortcutExtension(path.extension().wstring()) ||
                used.contains(path.filename().wstring()) ||
                !fs::is_regular_file(item.symlink_status()))
                continue;
            std::error_code ec;
            fs::remove(path, ec);
        }
    } catch (...) {
        // If a backup cannot be inspected, leave the recovery files in place.
    }
}

static void cleanUnusedPins();

// A pre-release layout can already contain private shortcuts, including a
// shortcut whose original was deleted. Migrate those files before the layout.
static void loadUserFolders(const fs::path& directory, const fs::path& legacy) {
    config = directory / L"folders.json";
    fs::create_directories(directory);
    bool migrate = !fs::exists(config) && fs::exists(legacy);
    auto source = migrate ? legacy : config;
    if (fs::exists(source)) {
        failedReadPath = source;
        groups = decode(bytes(source));
        failedReadPath.clear();
    }
    auto upgraded = groups;
    ShortcutCopies copies;
    if (migrate) {
        for (auto& folder : upgraded)
            for (auto& app : folder.apps) {
                if (app.savedShortcut.empty())
                    continue;
                auto dest = savedShortcutPath(app);
                auto from =
                    legacy.parent_path() / L"Shortcuts" / dest.filename();
                if (fs::is_regular_file(dest))
                    continue;
                if (!fs::is_regular_file(from)) {
                    app.savedShortcut.clear();
                    continue;
                }
                fs::create_directories(dest.parent_path());
                if (!CopyFileW(from.c_str(), dest.c_str(), TRUE))
                    winrt::throw_last_error();
                copies.created.push_back(dest);
            }
    }
    copies.capture(upgraded);
    if (migrate || !copies.created.empty()) {
        persist(upgraded);
        copies.committed = true;
        groups = std::move(upgraded);
    }
    pruneSavedShortcuts();
    cleanUnusedPins();
    publishPinNames(indexPinNames(groups));
}

static void loadModel() {
    if (loaded)
        return;
    loaded = true;
    try {
        wstring storage(32768, L'\0');
        auto length = Wh_GetModStoragePath(storage.data(), storage.size());
        if (!length)
            throw std::runtime_error("Windhawk storage");
        storage.resize(length);
        auto directory = fs::path(storage) / userSid();
        auto legacy = knownPath(FOLDERID_LocalAppData) / L"TaskbarFolders" /
                      L"Xaml" / L"cartelle.json";
        loadUserFolders(directory, legacy);
    } catch (...) {
        locked = true;
        Wh_Log(L"Could not read the folder configuration; saving is disabled.");
    }
}
static void persist(const Model& m) {
    auto data = encode(m);
    auto tmp = config.parent_path() / guid();
    HANDLE h = CreateFileW(tmp.c_str(), GENERIC_WRITE, 0, nullptr, CREATE_NEW,
                           FILE_ATTRIBUTE_NORMAL, nullptr);
    if (h == INVALID_HANDLE_VALUE)
        winrt::throw_last_error();
    DWORD n = 0;
    bool ok = data.size() <= MAXDWORD &&
              WriteFile(h, data.data(), (DWORD)data.size(), &n, nullptr) &&
              n == data.size() && FlushFileBuffers(h);
    CloseHandle(h);
    if (ok && fs::exists(config)) {
        auto backup = config;
        backup += L".bak";
        ok = ReplaceFileW(config.c_str(), tmp.c_str(), backup.c_str(), 0,
                          nullptr, nullptr);
    } else if (ok)
        ok = MoveFileExW(tmp.c_str(), config.c_str(), MOVEFILE_WRITE_THROUGH);
    if (!ok) {
        DeleteFileW(tmp.c_str());
        throw std::runtime_error("save");
    }
}
static unsigned revision = 0;
static void requestLayout();
template <typename T>
static bool edit(T change) {
    loadModel();
    if (locked) {
        const auto& path = failedReadPath.empty() ? config : failedReadPath;
        if (path.empty())
            note(uiText(UiString::StorageUnavailable));
        else
            note(formatUi(UiString::ConfigUnreadable, path.wstring()).c_str());
        return false;
    }
    try {
        auto next = groups;
        change(next);
        ShortcutCopies copies;
        copies.capture(next, &groups);
        if (next == groups)
            return true;
        auto names = indexPinNames(next);
        persist(next);
        copies.committed = true;
        groups = std::move(next);
        ++revision;
        publishPinNames(std::move(names));
        requestLayout();
        return true;
    } catch (...) {
        note(uiText(UiString::SaveFailed));
        return false;
    }
}
// An undo record covers only the app lists changed by the last move.
struct FolderOrder {
    wstring id;
    std::vector<App> before, after;
};
static std::vector<FolderOrder> lastMove;
template <class Change>
static bool editMove(Change change) {
    std::vector<FolderOrder> record;
    // These edits only move apps. Capture the affected folders before
    // committing to disk.
    if (!edit([&](Model& next) {
            change(next);
            for (size_t i = 0; i < groups.size(); ++i)
                if (groups[i].apps != next[i].apps)
                    record.push_back({groups[i].id, groups[i].apps, {}});
        }))
        return false;
    if (!record.empty()) {
        for (auto& order : record)
            order.after = lookup(groups, order.id)->apps;
        lastMove = std::move(record);
    }
    return true;
}
static bool canUndoMove() {
    if (lastMove.empty())
        return false;
    for (const auto& order : lastMove) {
        auto folder = lookup(groups, order.id);
        if (!folder || folder->apps != order.after)
            return false;
    }
    // Respect a later move of a removed item into a folder outside this undo
    // record.
    AppIdentitySet otherApps;
    for (const auto& folder : groups) {
        if (std::any_of(lastMove.begin(), lastMove.end(),
                        [&](const FolderOrder& order) {
                            return order.id == folder.id;
                        }))
            continue;
        for (const auto& app : folder.apps)
            otherApps.add(app);
    }
    for (const auto& order : lastMove)
        for (const auto& app : order.before)
            if (otherApps.contains(app))
                return false;
    return true;
}
static bool undoMove() {
    if (!canUndoMove())
        return false;
    if (!edit([&](Model& m) {
            for (const auto& item : lastMove)
                lookup(m, item.id)->apps = item.before;
        }))
        return false;
    lastMove.clear();
    return true;
}
static com_ptr<IShellItem> shellItem(const wstring& target) {
    com_ptr<IShellItem> item;
    if (!target.empty())
        SHCreateItemFromParsingName(target.c_str(), nullptr,
                                    IID_PPV_ARGS(item.put()));
    return item;
}
static wstring display(IShellItem* item, SIGDN mode) {
    PWSTR name = nullptr;
    if (!item || FAILED(item->GetDisplayName(mode, &name)))
        return {};
    wstring text = name;
    CoTaskMemFree(name);
    return text;
}
static wstring appKey(FrameworkElement const& e) {
    wstring v(X::Automation::AutomationProperties::GetAutomationId(e));
    return prefix(v, L"Appid: ") ? v.substr(7) : L"";
}
static std::optional<App> byId(const wstring& id) {
    if (id.empty())
        return {};
    wstring target = L"shell:AppsFolder\\" + id;
    auto item = shellItem(target);
    if (!item) {
        PWSTR expanded = nullptr;
        if (id.front() == L'{') {
            auto end = id.find(L"}\\");
            GUID folder{};
            if (end != wstring::npos &&
                SUCCEEDED(
                    CLSIDFromString(id.substr(0, end + 1).c_str(), &folder)) &&
                SUCCEEDED(
                    SHGetKnownFolderPath(folder, 0, nullptr, &expanded))) {
                target = (fs::path(expanded) / id.substr(end + 2)).wstring();
                CoTaskMemFree(expanded);
                item = shellItem(target);
            }
        } else if (fs::path(id).is_absolute()) {
            target = id;
            item = shellItem(target);
        }
    }
    if (!item)
        return {};
    auto name = display(item.get(), SIGDN_NORMALDISPLAY);
    return App{id, name.empty() ? id : name, target};
}
static bool launch(const App& app) {
    PIDLIST_ABSOLUTE pidl = nullptr;
    auto target = launchTarget(app);
    fs::path path(target);
    std::error_code ec;
    if (path.is_absolute() && shortcutExtension(path.extension().wstring()) &&
        !fs::is_regular_file(path, ec)) {
        if (app.id.empty())
            return false;
        auto installed = byId(app.id);
        if (!installed || equal(installed->target, target))
            return false;
        target = installed->target;
    }
    if (FAILED(SHParseDisplayName(target.c_str(), nullptr, &pidl, 0, nullptr)))
        return false;
    SHELLEXECUTEINFOW s{sizeof(s)};
    s.fMask = SEE_MASK_IDLIST | SEE_MASK_FLAG_NO_UI;
    s.lpIDList = pidl;
    s.nShow = SW_SHOWNORMAL;
    bool ok = ShellExecuteExW(&s);
    CoTaskMemFree(pidl);
    return ok;
}
template <class Fn>
static void visit(X::DependencyObject const& root, Fn&& fn, int level = 0) {
    if (!root || level > 40)
        return;
    auto e = root.try_as<FrameworkElement>();
    if (e && !fn(e))
        return;
    int n = M::VisualTreeHelper::GetChildrenCount(root);
    for (int i = 0; i < n; ++i)
        visit(M::VisualTreeHelper::GetChild(root, i), fn, level + 1);
}

using Drops =
    F::IAsyncOperation<F::Collections::IVectorView<W::Storage::IStorageItem>>;
struct DropJob {
    wstring folder;
    Drops storage{nullptr};
    std::vector<wstring> paths;
    std::vector<App> apps;
    std::optional<App> before;
    uint64_t previewToken{};
};
struct TaskbarDrag {
    wstring app;
    POINT start{};
    uint32_t pointerId{};
    std::optional<App> resolved;
    bool resolvedOnce = false;
    wstring previewFolder;
    uint64_t token{};
};
struct TaskbarDrop {
    wstring app, folder;
    std::optional<App> before;
    uint64_t token{};
    std::optional<App> resolved;
};
struct Hidden {
    winrt::weak_ref<FrameworkElement> item;
    F::IInspectable old{nullptr};
    X::Data::BindingBase binding{nullptr};
};
struct Tile {
    Button button{nullptr};
    wstring id;
    FrameworkElement motionHost{nullptr};
    FrameworkElement plateHost{nullptr}, nativePlate{nullptr};
    C::Border plate{nullptr};
    X::ElementTheme plateTheme = X::ElementTheme::Default;
    bool plateSearched = false;
};
struct View {
    HWND primary{};
    HMONITOR monitor{};
    FrameworkElement frame{nullptr}, repeater{nullptr};
    C::Grid root{nullptr};
    C::Canvas overlay{nullptr};
    C::StackPanel strip{nullptr};
    C::ControlTemplate tileTemplate{nullptr};
    bool layoutPending = false, rootDetached = false;
    std::vector<std::function<void()>> frameOff;
    std::vector<Tile> tiles;
    std::vector<Hidden> hidden;
    std::vector<DropJob> drops;
    std::vector<std::function<void()>> tileOff, menuOff;
    unsigned painted = ~0u;
    X::Thickness baseMargin{};
    bool marginSet = false;
    double reserved = 0;
    TaskbarDrag drag;
    std::deque<TaskbarDrop> releasedButtons;
    std::map<wstring, wstring, IgnoreCase> groupByApp;
};
[[clang::no_destroy]] static View ui;
static HWND controller = nullptr;
static std::atomic<HWND> published = nullptr;
static DWORD uiThread = 0;
static constexpr wchar_t kController[] = L"TaskbarFoldersXaml.Controller.065";
static constexpr wchar_t kHost[] = L"TaskbarFoldersXaml.JumpView.065";
static const UINT actionMsg =
    RegisterWindowMessageW(L"TaskbarFoldersXaml.Action.065");
static const UINT refreshMsg =
    RegisterWindowMessageW(L"TaskbarFoldersXaml.Refresh.065");
static constexpr UINT kStop = WM_APP + 321, kCancelModal = WM_APP + 330,
                      kModalDrained = WM_APP + 331;
enum : UINT_PTR {
    kLayoutTimer = 0x7466,
    kDropTimer,
    kIconHoverTimer,
    kExitTimer,
    kFileSurfaceTimer,
    kNativeDropTimer,
    kModalDrainTimer
};
static constexpr UINT kExportReady = WM_APP + 326;
static constexpr UINT kButtonRunningChanged = WM_APP + 325;
static constexpr UINT kTaskbarDrop = WM_APP + 322,
                      kAppearanceChanged = WM_APP + 323,
                      kExitFrame = WM_APP + 324;
static bool queueDrop(DropJob job) {
    if (stopping || !controller || !lookup(groups, job.folder))
        return false;
    ui.drops.push_back(std::move(job));
    if (SetTimer(controller, kDropTimer, 45, nullptr))
        return true;
    ui.drops.pop_back();
    return false;
}
static void requestLayout() {
    if (!stopping && controller && !ui.layoutPending)
        ui.layoutPending = SetTimer(controller, kLayoutTimer, 65, nullptr) != 0;
}
static void off(std::vector<std::function<void()>>& v) {
    for (auto& f : v)
        try {
            f();
        } catch (...) {
        }
    v.clear();
}
static HWND tray() {
    return FindWindowW(L"Shell_TrayWnd", nullptr);
}
static bool isPrimaryRoot(FrameworkElement const& element) {
    if (!ui.root || ui.rootDetached || !element || ui.primary != tray() ||
        ui.monitor != MonitorFromWindow(ui.primary, MONITOR_DEFAULTTONULL))
        return false;
    auto root = ui.root.XamlRoot();
    return root && element.XamlRoot() == root;
}

static void closePanel();
static HINSTANCE moduleInstance();

static wstring folderName(wstring value) {
    auto first = value.find_first_not_of(L" \r\n\t");
    return first == wstring::npos
               ? wstring{}
               : value.substr(first,
                              value.find_last_not_of(L" \r\n\t") - first + 1);
}
enum class ModalKind { Dialog, Menu };
static std::optional<ModalKind> activeModal;
static HWND modalDialog = nullptr;
static bool drainModal = false;
static HANDLE modalDrained = nullptr;

class ModalScope {
   public:
    explicit ModalScope(ModalKind kind) : entered(!stopping && !activeModal) {
        if (entered)
            activeModal = kind;
    }
    ~ModalScope() {
        if (!entered)
            return;
        modalDialog = nullptr;
        activeModal.reset();
        // Let the enclosing XAML/Win32 handler return before unloading.
        if (drainModal && !PostMessageW(controller, kModalDrained, 0, 0) &&
            !SetTimer(controller, kModalDrainTimer, 1, nullptr)) {
            // The worker still synchronizes with this thread through kStop.
            SetEvent(modalDrained);
        }
    }
    explicit operator bool() const { return entered; }
    ModalScope(const ModalScope&) = delete;
    ModalScope& operator=(const ModalScope&) = delete;

   private:
    bool entered;
};

struct DialogInput {
    const wchar_t* title;
    const wchar_t* text = nullptr;
    bool confirm = false;
    wstring initial, result;
};
static INT_PTR CALLBACK dialogProc(HWND w, UINT msg, WPARAM wp, LPARAM lp) {
    if (msg == WM_INITDIALOG) {
        modalDialog = w;
        if (stopping) {
            EndDialog(w, IDCANCEL);
            return TRUE;
        }
        auto input = reinterpret_cast<DialogInput*>(lp);
        SetWindowLongPtrW(w, DWLP_USER, lp);
        SetWindowTextW(w, input->title);
        RECT r{};
        GetClientRect(w, &r);
        const int dpi = GetDpiForWindow(w);
        auto scaled = [&](int value) {
            return MulDiv(value, dpi ? dpi : 96, 96);
        };
        const int pad = scaled(12), height = scaled(28), button = scaled(78);
        auto addButton = [&](int id, const wchar_t* label, int x, DWORD style) {
            return CreateWindowW(
                L"BUTTON", label, WS_CHILD | WS_VISIBLE | WS_TABSTOP | style, x,
                r.bottom - pad - height, button, height, w,
                reinterpret_cast<HMENU>(static_cast<INT_PTR>(id)), nullptr,
                nullptr);
        };
        HWND content = nullptr;
        if (input->text) {
            content = CreateWindowW(
                L"STATIC", input->text,
                WS_CHILD | WS_VISIBLE | SS_LEFT | SS_NOPREFIX, pad, pad,
                r.right - 2 * pad, r.bottom - height - 3 * pad, w, nullptr,
                nullptr, nullptr);
        } else {
            content = CreateWindowExW(
                WS_EX_CLIENTEDGE, L"EDIT", input->initial.c_str(),
                WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_AUTOHSCROLL, pad, pad,
                r.right - 2 * pad, height, w, (HMENU)100, nullptr, nullptr);
        }
        const bool hasCancel = !input->text || input->confirm;
        int right = r.right - pad - button;
        HWND cancel =
            hasCancel ? addButton(IDCANCEL, uiText(UiString::Cancel), right, 0)
                      : nullptr;
        HWND ok = addButton(IDOK, input->text ? L"OK" : uiText(UiString::Save),
                            hasCancel ? right - button - scaled(8) : right,
                            BS_DEFPUSHBUTTON);
        for (auto child : {content, ok, cancel})
            if (child)
                SendMessageW(child, WM_SETFONT,
                             (WPARAM)GetStockObject(DEFAULT_GUI_FONT), TRUE);
        if (!input->text) {
            SendMessageW(content, EM_SETLIMITTEXT, 80, 0);
            SendMessageW(content, EM_SETSEL, 0, -1);
        }
        SendMessageW(w, DM_SETDEFID, IDOK, 0);
        SetFocus(input->text ? ok : content);
        return FALSE;
    }
    if (msg == WM_COMMAND && LOWORD(wp) == IDOK) {
        auto input =
            reinterpret_cast<DialogInput*>(GetWindowLongPtrW(w, DWLP_USER));
        if (!input)
            return FALSE;
        if (!input->text) {
            wchar_t buf[82]{};
            GetDlgItemTextW(w, 100, buf, ARRAYSIZE(buf));
            wstring value = buf;
            auto first = value.find_first_not_of(L" \r\n\t");
            if (first == wstring::npos) {
                SetFocus(GetDlgItem(w, 100));
                return TRUE;
            }
            input->result = value.substr(
                first, value.find_last_not_of(L" \r\n\t") - first + 1);
        }
        EndDialog(w, IDOK);
        return TRUE;
    }
    if (msg == WM_CLOSE || (msg == WM_COMMAND && LOWORD(wp) == IDCANCEL)) {
        EndDialog(w, IDCANCEL);
        return TRUE;
    }
    if (msg == WM_NCDESTROY && modalDialog == w)
        modalDialog = nullptr;
    return FALSE;
}

static INT_PTR showDialog(DialogInput& input) {
    ModalScope modal(ModalKind::Dialog);
    if (!modal)
        return IDCANCEL;
    struct alignas(DWORD) Template {
        DLGTEMPLATE d;
        WORD menu, cls, title;
    } t{};
    t.d.style = WS_POPUP | WS_CAPTION | WS_SYSMENU | DS_MODALFRAME | DS_CENTER;
    t.d.cx = input.text ? 240 : 200;
    t.d.cy = input.text ? 90 : 64;
    auto result =
        DialogBoxIndirectParamW(moduleInstance(), &t.d, tray(), dialogProc,
                                reinterpret_cast<LPARAM>(&input));
    return stopping ? IDCANCEL : result;
}

static wstring nameDialog(wstring initial, const wchar_t* title) {
    DialogInput input{.title = title, .initial = std::move(initial)};
    if (showDialog(input) == IDOK)
        return std::move(input.result);
    return {};
}

static void note(const wchar_t* text) {
    DialogInput input{.title = uiText(UiString::Title), .text = text};
    showDialog(input);
}

static bool confirmRemove(const wstring& name) {
    DialogInput input{.title = name.c_str(),
                      .text = uiText(UiString::RemovePrompt),
                      .confirm = true};
    return showDialog(input) == IDOK;
}
// Native shortcuts have their own AppUserModelID. Windows owns the button,
// its drag gesture, ordering, shell visual states and transitions.
static constexpr wchar_t kPinPrefix[] = L"TaskbarFolders.Xaml.";
static std::atomic<bool> nativeReady = false;
static bool validFolderId(const wstring& id) {
    GUID g{};
    return id.size() == 38 && id.front() == L'{' && id.back() == L'}' &&
           SUCCEEDED(CLSIDFromString(id.c_str(), &g));
}
static wstring pinId(const wstring& id) {
    return validFolderId(id) ? wstring(kPinPrefix) + id.substr(1, 36) : L"";
}
static const AppFolder* folderForPin(const wstring& id) {
    if (!prefix(id, kPinPrefix))
        return nullptr;
    for (const auto& f : groups)
        if (equal(pinId(f.id), id))
            return &f;
    return nullptr;
}
// Shell name queries can arrive off the UI thread. They see only committed
// names.
static PinNames indexPinNames(const Model& model) {
    PinNames next;
    for (const auto& f : model) {
        auto id = pinId(f.id);
        if (!id.empty())
            next.emplace(std::move(id), f.name);
    }
    return next;
}
static fs::path knownPath(REFKNOWNFOLDERID k) {
    PWSTR p = nullptr;
    winrt::check_hresult(SHGetKnownFolderPath(k, 0, nullptr, &p));
    fs::path path(p);
    CoTaskMemFree(p);
    return path;
}
static constexpr PROPERTYKEY kModelId = {
    {0x9f4c2855,
     0x9f79,
     0x4b39,
     {0xa8, 0xd0, 0xe1, 0xd4, 0x2d, 0xe1, 0xd5, 0xf3}},
    5};
static constexpr PROPERTYKEY kPinTitle = {
    {0xf29f85e0,
     0x4ff9,
     0x1068,
     {0xab, 0x91, 0x08, 0x00, 0x2b, 0x27, 0xb3, 0xd9}},
    2};
static wstring shortcutId(const fs::path& p) {
    if (!equal(p.extension().wstring(), L".lnk"))
        return {};
    com_ptr<IShellLinkW> link;
    if (FAILED(CoCreateInstance(CLSID_ShellLink, nullptr, CLSCTX_INPROC_SERVER,
                                IID_PPV_ARGS(link.put()))) ||
        FAILED(link.as<IPersistFile>()->Load(p.c_str(), STGM_READ)))
        return {};
    auto props = link.as<IPropertyStore>();
    PROPVARIANT v{};
    if (FAILED(props->GetValue(kModelId, &v)))
        return {};
    wstring id = v.vt == VT_LPWSTR && v.pwszVal ? v.pwszVal : L"";
    PropVariantClear(&v);
    return id;
}
static wstring linkTitle(wstring name) {
    for (auto& c : name)
        if (c < 32 || wcschr(L"<>:\"/\\|?*", c))
            c = L'_';
    while (!name.empty() && (name.back() == L' ' || name.back() == L'.'))
        name.pop_back();
    if (name.empty())
        return uiText(UiString::Title);
    auto stem = name.substr(0, name.find(L'.'));
    if (equal(stem, L"CON") || equal(stem, L"PRN") || equal(stem, L"AUX") ||
        equal(stem, L"NUL") ||
        (stem.size() == 4 && (prefix(stem, L"COM") || prefix(stem, L"LPT")) &&
         iswdigit(stem[3])))
        name = L"_" + name;
    return name;
}
struct PinPaths {
    fs::path art, links, link, fallback;
};
static PinPaths pinPaths(const AppFolder& f) {
    if (!validFolderId(f.id))
        throw std::runtime_error("folder id");
    auto dir = config.parent_path() / L"Pin" / f.id;
    auto links = dir / L"Links";
    return {dir / L"preview.ico", links, links / (linkTitle(f.name) + L".lnk"),
            dir / L"Folder"};
}
static fs::path pinnedLinks() {
    return knownPath(FOLDERID_RoamingAppData) /
           L"Microsoft\\Internet Explorer\\Quick Launch\\User Pinned\\TaskBar";
}
static std::vector<fs::path> ownLinks(const fs::path& directory,
                                      const wstring& id) {
    std::vector<fs::path> paths;
    std::error_code ec;
    for (fs::directory_iterator i(directory, ec), end; !ec && i != end;
         i.increment(ec))
        if (equal(i->path().extension().wstring(), L".lnk") &&
            equal(shortcutId(i->path()), id))
            paths.push_back(i->path());
    return paths;
}
using Pixel = std::array<BYTE, 4>;
struct Pixels {
    int w = 0, h = 0;
    std::vector<Pixel> data;
};
static HICON shellIcon(const App&);
static HICON resourceIcon(const fs::path& location,
                          int index = 0,
                          int size = 0) {
    if (!location.is_absolute())
        return nullptr;
    HICON icon = nullptr;
    if (size > 0 &&
        SUCCEEDED(SHDefExtractIconW(location.c_str(), index, 0, &icon, nullptr,
                                    MAKELONG(size, size))) &&
        icon)
        return icon;
    auto count = ExtractIconExW(location.c_str(), index, &icon, nullptr, 1);
    return count == 1 ? icon : nullptr;
}
static HICON shortcutIcon(const fs::path& path, int size = 0) {
    com_ptr<IShellLinkW> link;
    if (FAILED(CoCreateInstance(CLSID_ShellLink, nullptr, CLSCTX_INPROC_SERVER,
                                IID_PPV_ARGS(link.put()))) ||
        FAILED(link.as<IPersistFile>()->Load(path.c_str(), STGM_READ)))
        return nullptr;
    wchar_t file[32768]{}, expanded[32768]{};
    int index = 0;
    if (SUCCEEDED(link->GetIconLocation(file, ARRAYSIZE(file), &index)) &&
        *file) {
        DWORD length =
            ExpandEnvironmentStringsW(file, expanded, ARRAYSIZE(expanded));
        if (length && length <= ARRAYSIZE(expanded))
            if (auto h = resourceIcon(expanded, index, size))
                return h;
    }
    if (SUCCEEDED(
            link->GetPath(file, ARRAYSIZE(file), nullptr, SLGP_RAWPATH)) &&
        equal(fs::path(file).extension().wstring(), L".exe"))
        return resourceIcon(file, 0, size);
    return nullptr;
}
// The pinned shortcut directory is unchanged for the duration of a typical
// popup.
using PinnedIconLinks = std::map<wstring, std::vector<fs::path>, IgnoreCase>;
static PinnedIconLinks readPinnedIconLinks(const fs::path& dir) {
    PinnedIconLinks links;
    std::error_code ec;
    for (fs::directory_iterator i(dir, ec), end; !ec && i != end;
         i.increment(ec)) {
        if (!equal(i->path().extension().wstring(), L".lnk"))
            continue;
        auto id = shortcutId(i->path());
        if (!id.empty())
            links[id].push_back(i->path());
    }
    return links;
}
static HICON desktopIcon(const App& app,
                         int size = 0,
                         std::optional<PinnedIconLinks>* cache = nullptr) {
    auto path = fs::path(launchTarget(app));
    if (equal(path.extension().wstring(), L".lnk"))
        return shortcutIcon(path, size);
    if (equal(path.extension().wstring(), L".exe") && path.is_absolute())
        return resourceIcon(path, 0, size);
    if (!prefix(app.target, L"shell:AppsFolder\\"))
        return nullptr;
    auto id = app.id.empty() ? app.target.substr(wcslen(L"shell:AppsFolder\\"))
                             : app.id;
    if (cache) {
        if (!*cache)
            cache->emplace(readPinnedIconLinks(pinnedLinks()));
        auto links = (*cache)->find(id);
        if (links != (*cache)->end())
            for (const auto& link : links->second)
                if (auto h = shortcutIcon(link, size))
                    return h;
    } else
        for (const auto& link : ownLinks(pinnedLinks(), id))
            if (auto h = shortcutIcon(link, size))
                return h;
    auto item = shellItem(app.target);
    auto details = item ? item.try_as<IShellItem2>() : nullptr;
    static constexpr PROPERTYKEY targetKey = {
        {0xb9b4b3fc,
         0x2b51,
         0x4a42,
         {0xb5, 0xd8, 0x32, 0x41, 0x46, 0xaf, 0xcf, 0x25}},
        2};
    PWSTR raw = nullptr;
    if (details && SUCCEEDED(details->GetString(targetKey, &raw)) && raw) {
        fs::path file(raw);
        CoTaskMemFree(raw);
        if (equal(file.extension().wstring(), L".exe"))
            return resourceIcon(file, 0, size);
    }
    return nullptr;
}
static Pixels shellBitmap(const App& a) {
    auto item = shellItem(launchTarget(a));
    if (!item)
        return {};
    auto factory = item.try_as<IShellItemImageFactory>();
    HBITMAP image = nullptr;
    if (!factory ||
        FAILED(factory->GetImage(
            {48, 48}, SIIGBF_ICONONLY | SIIGBF_BIGGERSIZEOK, &image)))
        return {};
    struct Guard {
        HBITMAP h;
        ~Guard() { DeleteObject(h); }
    } guard{image};
    BITMAP bm{};
    if (!GetObjectW(image, sizeof(bm), &bm) || bm.bmWidth < 1 ||
        bm.bmHeight < 1 || bm.bmWidth > 512 || bm.bmHeight > 512)
        return {};
    Pixels result{(int)bm.bmWidth, (int)bm.bmHeight, {}};
    result.data.resize(result.w * result.h);
    BITMAPINFO info{};
    info.bmiHeader = {
        sizeof(BITMAPINFOHEADER), bm.bmWidth, -bm.bmHeight, 1, 32, BI_RGB};
    auto dc = GetDC(nullptr);
    if (!dc)
        return {};
    int rows = GetDIBits(dc, image, 0, bm.bmHeight, result.data.data(), &info,
                         DIB_RGB_COLORS);
    ReleaseDC(nullptr, dc);
    if (rows != bm.bmHeight ||
        std::none_of(result.data.begin(), result.data.end(),
                     [](const Pixel& p) { return p[3] > 0; }))
        return {};
    return result;
}
static Pixels iconPixels(const App& a,
                         std::optional<PinnedIconLinks>* pins = nullptr) {
    constexpr int n = 48;
    Pixels result;
    HICON ico = desktopIcon(a, 0, pins);
    if (!ico) {
        auto bitmap = shellBitmap(a);
        if (!bitmap.data.empty())
            return bitmap;
        ico = shellIcon(a);
    }
    if (!ico)
        return result;
    struct IconGuard {
        HICON h;
        ~IconGuard() { DestroyIcon(h); }
    } iconGuard{ico};
    BITMAPINFO bi{};
    bi.bmiHeader = {sizeof(BITMAPINFOHEADER), n, -n, 1, 32, BI_RGB};
    HDC dc = CreateCompatibleDC(nullptr);
    if (!dc)
        return result;
    BYTE* bits = nullptr;
    HBITMAP dib =
        CreateDIBSection(dc, &bi, DIB_RGB_COLORS, (void**)&bits, nullptr, 0);
    if (!dib) {
        DeleteDC(dc);
        return result;
    }
    auto old = SelectObject(dc, dib);
    struct CanvasGuard {
        HDC dc;
        HGDIOBJ old;
        HBITMAP dib;
        ~CanvasGuard() {
            SelectObject(dc, old);
            DeleteObject(dib);
            DeleteDC(dc);
        }
    } guard{dc, old, dib};
    std::vector<Pixel> dark(n * n);
    memset(bits, 0, n * n * 4);
    if (!DrawIconEx(dc, 0, 0, ico, n, n, 0, nullptr, DI_NORMAL))
        return result;
    GdiFlush();
    memcpy(dark.data(), bits, n * n * 4);
    memset(bits, 255, n * n * 4);
    if (!DrawIconEx(dc, 0, 0, ico, n, n, 0, nullptr, DI_NORMAL))
        return result;
    GdiFlush();
    // Two backgrounds also preserve transparency for classic AND-mask icons.
    for (int i = 0; i < n * n; i++) {
        auto& px = dark[i];
        int delta = 0;
        for (int ch = 0; ch < 3; ch++)
            delta += std::max(0, (int)bits[i * 4 + ch] - px[ch]);
        px[3] = static_cast<BYTE>(255 - std::min(255, (delta + 1) / 3));
        for (int ch = 0; ch < 3; ch++)
            px[ch] = std::min(px[ch], px[3]);
    }
    result.w = n;
    result.h = n;
    result.data = std::move(dark);
    return result;
}
static std::vector<Pixel> mosaicPixels(int n, const std::vector<Pixels>& apps) {
    std::vector<Pixel> pixels(n * n);
    for (int y = 0; y < n; y++)
        for (int x = 0; x < n; x++) {
            double u = (x + .5) / n, v = (y + .5) / n;
            double edge =
                .20 - std::hypot(std::max(std::abs(u - .5) - .26, 0.),
                                 std::max(std::abs(v - .5) - .26, 0.));
            if (edge <= 0)
                continue;
            Pixel& p = pixels[y * n + x];
            p = edge < .030 ? Pixel{186, 162, 143, 225}
                            : Pixel{134, 113, 101, 94};
            p[3] = (BYTE)std::lround(p[3] * std::min(1., edge * n));
            size_t count = std::min<size_t>(apps.size(), 4);
            for (size_t i = 0; i < count; i++) {
                double side = count == 1 ? .76 : .40;
                double cx = count == 1 || (count == 3 && i == 2)
                                ? .5
                                : (i % 2 ? .715 : .285);
                double cy = count <= 2 ? .5 : (i / 2 ? .715 : .285);
                double sx = (u - cx) / side + .5, sy = (v - cy) / side + .5;
                if (sx < 0 || sx >= 1 || sy < 0 || sy >= 1)
                    continue;
                const auto& im = apps[i];
                if (im.data.empty()) {
                    if (sx > .16 && sx < .84 && sy > .16 && sy < .84)
                        p = {215, 198, 180, 255};
                    break;
                }
                const auto& q =
                    im.data[(int)(sy * im.h) * im.w + (int)(sx * im.w)];
                double alpha = q[3] / 255., back = (1 - alpha) * p[3] / 255.,
                       total = alpha + back;
                if (total > 0)
                    for (int ch = 0; ch < 3; ch++)
                        p[ch] = (BYTE)std::clamp(
                            std::lround((q[ch] + p[ch] * back) / total), 0L,
                            255L);
                p[3] = (BYTE)std::lround(total * 255);
                break;
            }
        }
    return pixels;
}
// An executable contains its own icon. Shortcuts and shell items may delegate
// to other files, so resolve them again when the visible membership changes.
struct ExeStamp {
    ULONGLONG written, size;
    bool operator==(const ExeStamp&) const = default;
};
static std::optional<ExeStamp> executableStamp(const wstring& target) {
    if (target.size() < 5 ||
        _wcsicmp(target.c_str() + target.size() - 4, L".exe") ||
        !fs::path(target).is_absolute())
        return {};
    WIN32_FILE_ATTRIBUTE_DATA info{};
    if (!GetFileAttributesExW(target.c_str(), GetFileExInfoStandard, &info))
        return {};
    const auto written =
        (ULONGLONG(info.ftLastWriteTime.dwHighDateTime) << 32) |
        info.ftLastWriteTime.dwLowDateTime;
    const auto size = (ULONGLONG(info.nFileSizeHigh) << 32) | info.nFileSizeLow;
    return ExeStamp{written, size};
}
struct Preview {
    std::array<std::optional<ExeStamp>, 4> stamps{};
    std::vector<wstring> targets;
    std::vector<Pixels> icons;
    M::Imaging::WriteableBitmap image{nullptr};
};
[[clang::no_destroy]] static std::map<wstring, Preview> previews;
static const Preview& previewFor(const AppFolder& folder) {
    auto& cached = previews[folder.id];
    const size_t count = std::min<size_t>(4, folder.apps.size());
    if (cached.image && cached.targets.size() == count &&
        std::equal(cached.targets.begin(), cached.targets.end(),
                   folder.apps.begin(), [](const wstring& path, const App& a) {
                       return equal(path, a.target);
                   }))
        return cached;
    Preview next;
    next.targets.reserve(count);
    next.icons.reserve(count);
    std::optional<PinnedIconLinks> pins;
    for (size_t i = 0; i < count; ++i) {
        const auto& app = folder.apps[i];
        next.targets.push_back(app.target);
        auto& stamp = next.stamps[i];
        stamp = executableStamp(app.target);
        if (stamp) {
            auto old = std::find_if(
                cached.targets.begin(), cached.targets.end(),
                [&](const auto& t) { return equal(t, app.target); });
            const auto slot = static_cast<size_t>(old - cached.targets.begin());
            if (old != cached.targets.end() && cached.stamps[slot] == stamp &&
                !cached.icons[slot].data.empty()) {
                next.icons.push_back(cached.icons[slot]);
                continue;
            }
        }
        next.icons.push_back(iconPixels(app, &pins));
    }
    constexpr int n = 80;
    auto frame = mosaicPixels(n, next.icons);
    for (auto& px : frame)
        for (int c = 0; c < 3; c++)
            px[c] = (BYTE)((px[c] * px[3] + 127) / 255);
    next.image = M::Imaging::WriteableBitmap(n, n);
    auto buf = next.image.PixelBuffer()
                   .as<::Windows::Storage::Streams::IBufferByteAccess>();
    BYTE* dest = nullptr;
    winrt::check_hresult(buf->Buffer(&dest));
    memcpy(dest, frame.data(), n * n * 4);
    next.image.Invalidate();
    cached = std::move(next);
    return cached;
}
#pragma pack(push, 1)
struct IcoHeader {
    WORD reserved, type, count;
};
struct IcoEntry {
    BYTE w, h, colors, reserved;
    WORD planes, depth;
    DWORD size, offset;
};
#pragma pack(pop)
static void writeMosaic(const AppFolder& f, const fs::path& target) {
    const auto& apps = previewFor(f).icons;
    constexpr int sizes[] = {24, 32, 48, 64};
    std::vector<IcoEntry> entries;
    DWORD off = sizeof(IcoHeader) + ARRAYSIZE(sizes) * sizeof(IcoEntry);
    for (int n : sizes) {
        DWORD len =
            sizeof(BITMAPINFOHEADER) + n * n * 4 + ((n + 31) / 32) * 4 * n;
        entries.push_back({(BYTE)n, (BYTE)n, 0, 0, 1, 32, len, off});
        off += len;
    }
    auto temp = target.parent_path() / (guid() + L".tmp");
    try {
        std::ofstream os(temp, std::ios::binary);
        os.exceptions(std::ios::badbit | std::ios::failbit);
        auto put = [&](const void* p, size_t size) {
            os.write((const char*)p, size);
        };
        IcoHeader head{0, 1, ARRAYSIZE(sizes)};
        put(&head, sizeof(head));
        put(entries.data(), entries.size() * sizeof(IcoEntry));
        for (int n : sizes) {
            auto px = mosaicPixels(n, apps);
            BITMAPINFOHEADER hdr{sizeof(hdr), n, 2 * n, 1, 32, BI_RGB};
            put(&hdr, sizeof(hdr));
            for (int y = n - 1; y >= 0; y--)
                put(px.data() + y * n, 4 * n);
            int stride = ((n + 31) / 32) * 4;
            std::vector<BYTE> mask(stride * n);
            for (int y = 0; y < n; y++)
                for (int x = 0; x < n; x++)
                    if (!px[(n - y - 1) * n + x][3])
                        mask[y * stride + x / 8] |= (BYTE)(128 >> (x % 8));
            put(mask.data(), mask.size());
        }
        os.close();
        if (!MoveFileExW(temp.c_str(), target.c_str(),
                         MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH))
            winrt::throw_last_error();
    } catch (...) {
        DeleteFileW(temp.c_str());
        throw;
    }
}
static void notifyLink(const fs::path& p, LONG what = SHCNE_UPDATEITEM) {
    SHChangeNotify(what, SHCNF_PATHW | SHCNF_FLUSHNOWAIT, p.c_str(), nullptr);
}
static void writePinLink(const AppFolder& f,
                         const PinPaths& p,
                         const fs::path& dest) {
    com_ptr<IShellLinkW> link;
    winrt::check_hresult(CoCreateInstance(CLSID_ShellLink, nullptr,
                                          CLSCTX_INPROC_SERVER,
                                          IID_PPV_ARGS(link.put())));
    winrt::check_hresult(
        link->SetPath((knownPath(FOLDERID_Windows) / L"explorer.exe").c_str()));
    winrt::check_hresult(
        link->SetArguments((L"\"" + p.fallback.wstring() + L"\"").c_str()));
    winrt::check_hresult(link->SetDescription(f.name.c_str()));
    winrt::check_hresult(link->SetIconLocation(p.art.c_str(), 0));
    auto props = link.as<IPropertyStore>();
    auto setText = [&](REFPROPERTYKEY k, const wstring& s) {
        PROPVARIANT v{};
        v.vt = VT_LPWSTR;
        v.pwszVal = const_cast<PWSTR>(s.c_str());
        winrt::check_hresult(props->SetValue(k, v));
    };
    setText(kModelId, pinId(f.id));
    setText(kPinTitle, f.name);
    winrt::check_hresult(props->Commit());
    auto temp = dest.parent_path() / (guid() + L".lnk");
    try {
        winrt::check_hresult(link.as<IPersistFile>()->Save(temp.c_str(), TRUE));
        if (!MoveFileExW(temp.c_str(), dest.c_str(),
                         MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH))
            winrt::throw_last_error();
    } catch (...) {
        DeleteFileW(temp.c_str());
        throw;
    }
}

struct PinStamp {
    wstring name;
    std::vector<std::pair<wstring, wstring>> icons;
};
static std::map<wstring, PinStamp> pinStamps;

static bool isRealDirectory(const fs::path& directory) {
    DWORD attr = GetFileAttributesW(directory.c_str());
    return attr != INVALID_FILE_ATTRIBUTES &&
           (attr & FILE_ATTRIBUTE_DIRECTORY) &&
           !(attr & FILE_ATTRIBUTE_REPARSE_POINT);
}

// Only clean files generated by this mod, inside its own private directory.
static void cleanPinFiles(const AppFolder& folder, bool removeFolder) {
    auto p = pinPaths(folder);
    auto dir = p.art.parent_path();
    if (!isRealDirectory(dir.parent_path()) || !isRealDirectory(dir))
        return;
    std::error_code ec;
    auto cleanTemporary = [&](const fs::path& directory, PCWSTR extension) {
        for (fs::directory_iterator it(directory, ec), end; !ec && it != end;
             it.increment(ec)) {
            auto path = it->path();
            if (validFolderId(path.stem().wstring()) &&
                equal(path.extension().wstring(), extension) &&
                fs::is_regular_file(it->symlink_status(ec)))
                fs::remove(path, ec);
        }
        ec.clear();
    };
    cleanTemporary(dir, L".tmp");
    if (isRealDirectory(p.links)) {
        // A user is allowed to give the folder a GUID as its display name.
        for (fs::directory_iterator it(p.links, ec), end; !ec && it != end;
             it.increment(ec)) {
            auto path = it->path();
            if (!equal(path.extension().wstring(), L".lnk") ||
                !fs::is_regular_file(it->symlink_status(ec)))
                continue;
            if (!removeFolder && equal(path.wstring(), p.link.wstring()))
                continue;
            auto id = shortcutId(path);
            if (equal(id, pinId(folder.id)) ||
                (id.empty() && validFolderId(path.stem().wstring())))
                fs::remove(path, ec);
        }
    }
    if (removeFolder) {
        if (isRealDirectory(p.links))
            fs::remove(p.links, ec);
        if (isRealDirectory(p.fallback))
            fs::remove(p.fallback, ec);
        fs::remove(p.art, ec);
        fs::remove(dir, ec);
    }
}

static void cleanUnusedPins() {
    try {
        auto directory = config.parent_path() / L"Pin";
        if (!isRealDirectory(directory))
            return;
        for (const auto& entry : fs::directory_iterator(directory)) {
            auto id = entry.path().filename().wstring();
            if (!validFolderId(id) || !isRealDirectory(entry.path()))
                continue;
            if (auto folder = lookup(groups, id))
                cleanPinFiles(*folder, false);
            else {
                AppFolder removed{id, uiText(UiString::Title), {}};
                cleanPinFiles(removed, true);
            }
        }
    } catch (...) {
        Wh_Log(L"Folder shortcut cleanup deferred.");
    }
}

static void ensureAssets(const AppFolder& folder,
                         const PinPaths& p,
                         bool existingOnly = false) {
    const bool hasArt = fs::exists(p.art);
    if (existingOnly && !hasArt)
        return;
    const bool priorLink = fs::exists(p.link);
    const auto count = std::min<size_t>(4, folder.apps.size());
    auto previous = pinStamps.find(folder.id);
    const bool sameName =
        previous != pinStamps.end() && previous->second.name == folder.name;
    const bool sameIcons =
        previous != pinStamps.end() && previous->second.icons.size() == count &&
        std::equal(previous->second.icons.begin(), previous->second.icons.end(),
                   folder.apps.begin(), [](const auto& icon, const App& app) {
                       return icon.first == app.target &&
                              icon.second == app.savedShortcut;
                   });
    if (sameName && sameIcons && priorLink && hasArt)
        return;
    fs::create_directories(p.links);
    fs::create_directories(p.fallback);
    if (!sameIcons || !hasArt) {
        writeMosaic(folder, p.art);
        notifyLink(p.art);
    }
    if (!priorLink || !sameName) {
        writePinLink(folder, p, p.link);
        notifyLink(p.link, priorLink ? SHCNE_UPDATEITEM : SHCNE_CREATE);
        cleanPinFiles(folder, false);
    }
    auto& stamp = pinStamps[folder.id];
    stamp.name = folder.name;
    if (!sameIcons) {
        stamp.icons.clear();
        stamp.icons.reserve(count);
        for (size_t i = 0; i < count; ++i)
            stamp.icons.emplace_back(folder.apps[i].target,
                                     folder.apps[i].savedShortcut);
    }
}
static void integrate(const wstring& id) {
    try {
        auto f = lookup(groups, id);
        if (!f)
            return;
        if (!nativeReady.load()) {
            note(uiText(UiString::PinUnavailable));
            return;
        }
        const auto name = f->name;
        auto p = pinPaths(*f);
        ensureAssets(*f, p);
        note(formatUi(UiString::PinInstructions, name).c_str());
        if (stopping)
            return;
        PIDLIST_ABSOLUTE item = nullptr;
        winrt::check_hresult(
            SHParseDisplayName(p.link.c_str(), nullptr, &item, 0, nullptr));
        auto hr = SHOpenFolderAndSelectItems(item, 0, nullptr, 0);
        CoTaskMemFree(item);
        winrt::check_hresult(hr);
    } catch (...) {
        note(uiText(UiString::PinFailed));
    }
}
static void createFolder() {
    loadModel();
    auto n = nameDialog(L"", uiText(UiString::CreateTitle));
    if (n.empty())
        return;
    auto id = guid();
    edit([&](Model& m) { m.push_back({id, n, {}}); });
}
struct Menu {
    HMENU h = CreatePopupMenu();
    ~Menu() {
        if (h)
            DestroyMenu(h);
    }
    void item(UINT id, const wstring& s) {
        wstring escaped;
        for (auto c : s) {
            escaped += c;
            if (c == L'&')
                escaped += c;
        }
        AppendMenuW(h, MF_STRING, id, escaped.c_str());
    }
    void sep() { AppendMenuW(h, MF_SEPARATOR, 0, nullptr); }
    UINT show(HWND owner = nullptr,
              TPMPARAMS* placement = nullptr,
              const POINT* position = nullptr) {
        ModalScope modal(ModalKind::Menu);
        if (!modal)
            return 0;
        POINT pt{};
        if (position)
            pt = *position;
        else if (placement)
            pt = {placement->rcExclude.right, placement->rcExclude.top};
        else
            GetCursorPos(&pt);
        if (!owner)
            owner = tray();
        SetForegroundWindow(owner);
        UINT flags = TPM_RETURNCMD | TPM_RIGHTBUTTON;
        if (!placement)
            flags |= TPM_NONOTIFY;
        if (GetSystemMetrics(SM_MENUDROPALIGNMENT))
            flags |= TPM_RIGHTALIGN;
        const auto result =
            TrackPopupMenuEx(h, flags, pt.x, pt.y, owner, placement);
        return stopping ? 0 : result;
    }
};
static void moveTo(const wstring& dest, const App& a) {
    editMove([&](Model& m) { transfer(m, dest, a); });
}
static bool appFile(const fs::path&);
static std::optional<App> fromPath(wstring path) {
    path = fs::path(path).make_preferred().wstring();
    auto ext = fs::path(path).extension().wstring();
    if (!appFile(path))
        return {};
    wstring id = equal(ext, L".lnk") ? shortcutId(path) : L"";
    if (prefix(id, kPinPrefix))
        return {};
    auto item = shellItem(path);
    if (!item)
        return {};
    wstring name = display(item.get(), SIGDN_NORMALDISPLAY);
    return App{id, name.empty() ? fs::path(path).stem().wstring() : name, path};
}
// File picking runs on its own short-lived UI thread. The taskbar remains
// interactive.
enum : UINT { kFilesDone = WM_APP + 540, kCatalogDone };
static constexpr wchar_t kFilesClass[] = L"TaskbarFolders.Files.052";
[[clang::no_destroy]] static std::thread fileThread, catalogThread;
static std::atomic<HWND> fileWindow{};
static std::atomic<bool> filesReady = false, catalogReady = false;
static std::vector<wstring> selectedPaths;
static wstring fileDestination;
static std::vector<App> installedApps, incomingApps;
static ULONGLONG catalogTime = 0;
static uint64_t catalogGeneration = 0;
static bool catalogFailed = false, incomingFailed = false;
static bool appFile(const fs::path& file) {
    auto ext = file.extension().wstring();
    return equal(ext, L".lnk") || equal(ext, L".exe") || equal(ext, L".url") ||
           equal(ext, L".appref-ms");
}
static wstring windowText(HWND w) {
    wstring s(GetWindowTextLengthW(w) + 1, L'\0');
    int n = GetWindowTextW(w, s.data(), (int)s.size());
    s.resize(n);
    return s;
}
struct FileEntry {
    fs::path path;
    bool directory;
};
struct FilesWindow {
    HWND window{}, places{}, address{}, up{}, go{}, list{}, status{}, accept{},
        cancel{};
    HFONT font{};
    HIMAGELIST icons{};
    UINT dpi = 96;
    fs::path directory;
    std::vector<fs::path> roots;
    std::vector<FileEntry> entries;
    std::vector<wstring> picked;
};
enum : int { kPlaces = 450, kAddress, kUp, kGo, kFilesList, kPick, kCancel };
static void fileStatus(FilesWindow& w, const wchar_t* s) {
    SetWindowTextW(w.status, s);
}
static void loadDirectory(FilesWindow& w, fs::path path) {
    std::vector<FileEntry> entries;
    if (path.empty()) {
        DWORD drives = GetLogicalDrives();
        for (int i = 0; i < 26; i++)
            if (drives & (1u << i)) {
                wstring drive = L"A:\\";
                drive[0] = (wchar_t)(L'A' + i);
                entries.push_back({drive, true});
            }
    } else {
        path = path.lexically_normal().make_preferred();
        std::error_code ec;
        for (fs::directory_iterator it(path, ec), end; !ec && it != end;
             it.increment(ec)) {
            if (stopping)
                return;
            std::error_code attr;
            bool directory = it->is_directory(attr);
            if (!attr && (directory || appFile(it->path())))
                entries.push_back({it->path(), directory});
        }
        if (ec) {
            fileStatus(w, uiText(UiString::DirUnavailable));
            return;
        }
        std::sort(entries.begin(), entries.end(),
                  [](const FileEntry& a, const FileEntry& b) {
                      if (a.directory != b.directory)
                          return a.directory;
                      return _wcsicmp(a.path.c_str(), b.path.c_str()) < 0;
                  });
    }
    w.directory = path;
    w.entries = std::move(entries);
    SendMessageW(w.list, WM_SETREDRAW, FALSE, 0);
    ListView_DeleteAllItems(w.list);
    for (size_t i = 0; i < w.entries.size(); ++i) {
        const auto& e = w.entries[i];
        wstring name =
            path.empty() ? e.path.wstring() : e.path.filename().wstring();
        LVITEMW row{};
        row.mask = LVIF_TEXT | LVIF_IMAGE;
        row.iItem = (int)i;
        row.iImage = e.directory ? 0 : 1;
        row.pszText = name.data();
        ListView_InsertItem(w.list, &row);
    }
    SendMessageW(w.list, WM_SETREDRAW, TRUE, 0);
    InvalidateRect(w.list, nullptr, TRUE);
    SetWindowTextW(w.address, path.c_str());
    auto place = std::find(w.roots.begin(), w.roots.end(), path);
    SendMessageW(w.places, CB_SETCURSEL,
                 place == w.roots.end() ? -1 : place - w.roots.begin(), 0);
    EnableWindow(w.up, !path.empty());
    fileStatus(w, w.entries.empty() ? uiText(UiString::DirEmpty)
                                    : uiText(UiString::MultiSelect));
}
static void fileAccept(FilesWindow& w, int activated = -1) {
    if (activated >= 0 && activated < (int)w.entries.size() &&
        w.entries[activated].directory) {
        loadDirectory(w, w.entries[activated].path);
        return;
    }
    std::vector<wstring> files;
    fs::path sub;
    int dirs = 0;
    for (int n = ListView_GetNextItem(w.list, -1, LVNI_SELECTED); n >= 0;
         n = ListView_GetNextItem(w.list, n, LVNI_SELECTED)) {
        if (n >= (int)w.entries.size())
            continue;
        auto& entry = w.entries[n];
        if (entry.directory) {
            sub = entry.path;
            ++dirs;
        } else
            files.push_back(entry.path.wstring());
    }
    if (files.empty()) {
        if (dirs == 1)
            loadDirectory(w, sub);
        else
            fileStatus(w, uiText(UiString::ChooseApp));
        return;
    }
    w.picked = std::move(files);
    DestroyWindow(w.window);
}
static void fileGo(FilesWindow& w) {
    wstring value = windowText(w.address);
    if (value.size() >= 2 && value.front() == L'"' && value.back() == L'"')
        value = value.substr(1, value.size() - 2);
    DWORD count = ExpandEnvironmentStringsW(value.c_str(), nullptr, 0);
    if (count && count < 32768) {
        wstring expanded(count, L'\0');
        if (ExpandEnvironmentStringsW(value.c_str(), expanded.data(), count) ==
            count) {
            expanded.pop_back();
            value = std::move(expanded);
        }
    }
    if (value.empty()) {
        loadDirectory(w, {});
        return;
    }
    fs::path p(value);
    if (p.is_relative())
        p = w.directory / p;
    std::error_code ec;
    if (fs::is_regular_file(p, ec) && appFile(p)) {
        w.picked = {p.make_preferred().wstring()};
        DestroyWindow(w.window);
        return;
    }
    loadDirectory(w, p);
}
static void fileUp(FilesWindow& w) {
    if (!w.directory.empty())
        loadDirectory(w, w.directory == w.directory.root_path()
                             ? fs::path{}
                             : w.directory.parent_path());
}
static int fp(const FilesWindow& w, int dip) {
    return MulDiv(dip, w.dpi, 96);
}
static void layoutFiles(FilesWindow& w) {
    RECT r{};
    GetClientRect(w.window, &r);
    int pad = fp(w, 12), gap = fp(w, 6), hh = fp(w, 27), ww = r.right,
        hhClient = r.bottom;
    int y = pad;
    MoveWindow(w.places, pad, y, ww - 2 * pad, fp(w, 190), TRUE);
    y += hh + gap;
    int side = fp(w, 64);
    MoveWindow(w.up, pad, y, side, hh, TRUE);
    MoveWindow(w.go, ww - pad - side, y, side, hh, TRUE);
    int x = pad + side + gap;
    MoveWindow(w.address, x, y, std::max(1, ww - 2 * x), hh, TRUE);
    y += hh + gap;
    int footer = hhClient - pad - hh;
    MoveWindow(w.list, pad, y, std::max(1, ww - 2 * pad),
               std::max(1, footer - fp(w, 27) - y), TRUE);
    RECT client{};
    GetClientRect(w.list, &client);
    ListView_SetColumnWidth(w.list, 0, std::max(1L, client.right - 2));
    MoveWindow(w.status, pad, footer - fp(w, 23), std::max(1, ww - 2 * pad),
               fp(w, 22), TRUE);
    int bw = fp(w, 86);
    MoveWindow(w.cancel, ww - pad - bw, footer, bw, hh, TRUE);
    MoveWindow(w.accept, ww - pad - 2 * bw - gap, footer, bw, hh, TRUE);
}
static LRESULT CALLBACK fileProc(HWND h, UINT msg, WPARAM wp, LPARAM lp) {
    auto* w = (FilesWindow*)GetWindowLongPtrW(h, GWLP_USERDATA);
    if (msg == WM_NCCREATE) {
        w = (FilesWindow*)((CREATESTRUCTW*)lp)->lpCreateParams;
        w->window = h;
        SetWindowLongPtrW(h, GWLP_USERDATA, (LONG_PTR)w);
    }
    if (!w)
        return DefWindowProcW(h, msg, wp, lp);
    try {
        switch (msg) {
            case WM_GETMINMAXINFO: {
                auto m = (MINMAXINFO*)lp;
                m->ptMinTrackSize = {fp(*w, 345), fp(*w, 285)};
                return 0;
            }
            case WM_SIZE:
                if (w->cancel)
                    layoutFiles(*w);
                return 0;
            case WM_COMMAND:
                switch (LOWORD(wp)) {
                    case kPlaces:
                        if (HIWORD(wp) == CBN_SELCHANGE) {
                            auto i =
                                SendMessageW(w->places, CB_GETCURSEL, 0, 0);
                            if (i >= 0 && (size_t)i < w->roots.size())
                                loadDirectory(*w, w->roots[i]);
                        }
                        return 0;
                    case kGo:
                        fileGo(*w);
                        return 0;
                    case kUp:
                        fileUp(*w);
                        return 0;
                    case IDOK:
                        if (GetFocus() == w->address)
                            fileGo(*w);
                        else
                            fileAccept(*w);
                        return 0;
                    case kPick:
                        fileAccept(*w);
                        return 0;
                    case kCancel:
                    case IDCANCEL:
                        DestroyWindow(h);
                        return 0;
                }
                break;
            case WM_NOTIFY: {
                auto* n = (NMHDR*)lp;
                if (n->hwndFrom == w->list && n->code == NM_DBLCLK) {
                    auto a = (NMITEMACTIVATE*)lp;
                    if (a->iItem >= 0)
                        fileAccept(*w, a->iItem);
                    return 0;
                }
                break;
            }
            case WM_CLOSE:
                DestroyWindow(h);
                return 0;
            case WM_DESTROY:
                PostQuitMessage(0);
                return 0;
            case WM_NCDESTROY:
                w->window = nullptr;
                return DefWindowProcW(h, msg, wp, lp);
        }
    } catch (...) {
        fileStatus(*w, uiText(UiString::DirReadFailed));
    }
    return DefWindowProcW(h, msg, wp, lp);
}
static void runFiles(FilesWindow& w, const wstring& title, RECT anchor) {
    WNDCLASSW cls{};
    cls.hInstance = moduleInstance();
    cls.lpfnWndProc = fileProc;
    cls.lpszClassName = kFilesClass;
    cls.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    cls.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    if (!RegisterClassW(&cls) && GetLastError() != ERROR_CLASS_ALREADY_EXISTS)
        winrt::throw_last_error();
    INITCOMMONCONTROLSEX cc{sizeof(cc), ICC_LISTVIEW_CLASSES};
    InitCommonControlsEx(&cc);
    MONITORINFO mi{sizeof(mi)};
    GetMonitorInfoW(MonitorFromRect(&anchor, MONITOR_DEFAULTTOPRIMARY), &mi);
    HWND h = CreateWindowExW(WS_EX_TOOLWINDOW, kFilesClass, title.c_str(),
                             WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
                             mi.rcWork.left + 32, mi.rcWork.top + 32, 550, 410,
                             nullptr, nullptr, cls.hInstance, &w);
    if (!h)
        winrt::throw_last_error();
    struct Cleanup {
        FilesWindow& w;
        ~Cleanup() {
            fileWindow = nullptr;
            if (w.window)
                DestroyWindow(w.window);
            if (w.icons)
                ImageList_Destroy(w.icons);
            if (w.font)
                DeleteObject(w.font);
        }
    } cleanup{w};
    w.dpi = GetDpiForWindow(h);
    if (!w.dpi)
        w.dpi = 96;
    w.font =
        CreateFontW(-fp(w, 14), 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                    DEFAULT_CHARSET, 0, 0, CLEARTYPE_QUALITY, 0, L"Segoe UI");
    auto child = [&](PCWSTR c, PCWSTR t, DWORD style, int id,
                     DWORD ex = 0) -> HWND {
        auto control =
            CreateWindowExW(ex, c, t, WS_CHILD | WS_VISIBLE | style, 0, 0, 1, 1,
                            h, (HMENU)(INT_PTR)id, cls.hInstance, nullptr);
        if (!control)
            winrt::throw_last_error();
        SendMessageW(control, WM_SETFONT, (WPARAM)w.font, TRUE);
        return control;
    };
    w.places = child(WC_COMBOBOXW, uiText(UiString::Places),
                     CBS_DROPDOWNLIST | WS_TABSTOP | WS_VSCROLL, kPlaces);
    w.up = child(L"BUTTON", uiText(UiString::Up), WS_TABSTOP, kUp);
    w.address = child(L"EDIT", L"", WS_TABSTOP | ES_AUTOHSCROLL, kAddress,
                      WS_EX_CLIENTEDGE);
    w.go = child(L"BUTTON", uiText(UiString::Go), WS_TABSTOP, kGo);
    w.list = child(WC_LISTVIEWW, uiText(UiString::FoldersApps),
                   WS_TABSTOP | LVS_REPORT | LVS_NOCOLUMNHEADER |
                       LVS_SHOWSELALWAYS | LVS_SHAREIMAGELISTS,
                   kFilesList, WS_EX_CLIENTEDGE);
    w.status = child(L"STATIC", L"", SS_CENTERIMAGE | SS_ENDELLIPSIS, 0);
    w.accept = child(L"BUTTON", uiText(UiString::Add),
                     WS_TABSTOP | BS_DEFPUSHBUTTON, kPick);
    w.cancel = child(L"BUTTON", uiText(UiString::Cancel), WS_TABSTOP, kCancel);
    ListView_SetExtendedListViewStyle(
        w.list, LVS_EX_FULLROWSELECT | LVS_EX_DOUBLEBUFFER);
    LVCOLUMNW col{};
    col.mask = LVCF_WIDTH;
    col.cx = 400;
    ListView_InsertColumn(w.list, 0, &col);
    int size = fp(w, 18);
    w.icons = ImageList_Create(size, size, ILC_COLOR32 | ILC_MASK, 2, 0);
    if (w.icons) {
        for (auto sid : {SIID_FOLDER, SIID_APPLICATION}) {
            SHSTOCKICONINFO info{sizeof(info)};
            if (SUCCEEDED(SHGetStockIconInfo(sid, SHGSI_ICON | SHGSI_SMALLICON,
                                             &info))) {
                ImageList_AddIcon(w.icons, info.hIcon);
                DestroyIcon(info.hIcon);
            }
        }
        ListView_SetImageList(w.list, w.icons, LVSIL_SMALL);
    }
    auto place = [&](PCWSTR name, fs::path dir) {
        w.roots.push_back(std::move(dir));
        SendMessageW(w.places, CB_ADDSTRING, 0, (LPARAM)name);
    };
    place(uiText(UiString::ThisPc), {});
    auto addKnown = [&](PCWSTR name, REFKNOWNFOLDERID id) {
        PWSTR raw = nullptr;
        if (SUCCEEDED(SHGetKnownFolderPath(id, 0, nullptr, &raw))) {
            fs::path d(raw);
            CoTaskMemFree(raw);
            place(name, d);
        }
    };
    addKnown(uiText(UiString::Desktop), FOLDERID_Desktop);
    addKnown(uiText(UiString::Downloads), FOLDERID_Downloads);
    addKnown(uiText(UiString::PersonalStart), FOLDERID_Programs);
    addKnown(uiText(UiString::SharedStart), FOLDERID_CommonPrograms);
    int width = std::min<LONG>(fp(w, 560), mi.rcWork.right - mi.rcWork.left),
        height = std::min<LONG>(fp(w, 420), mi.rcWork.bottom - mi.rcWork.top);
    SetWindowPos(
        h, nullptr,
        mi.rcWork.left + (mi.rcWork.right - mi.rcWork.left - width) / 2,
        mi.rcWork.top + (mi.rcWork.bottom - mi.rcWork.top - height) / 2, width,
        height, SWP_NOZORDER | SWP_NOACTIVATE);
    loadDirectory(w, w.roots.size() > 1 ? w.roots[1] : fs::path{});
    fileWindow.store(h);
    if (stopping)
        return;
    ShowWindow(h, SW_SHOW);
    SetForegroundWindow(h);
    SetFocus(w.list);
    MSG m{};
    while (w.window && !stopping && GetMessageW(&m, nullptr, 0, 0) > 0) {
        if (m.message == WM_KEYDOWN || m.message == WM_SYSKEYDOWN) {
            if (m.wParam == VK_ESCAPE) {
                DestroyWindow(h);
                continue;
            }
            if (m.wParam == 'L' && (GetKeyState(VK_CONTROL) & 0x8000)) {
                SetFocus(w.address);
                SendMessageW(w.address, EM_SETSEL, 0, -1);
                continue;
            }
            if (m.wParam == VK_UP && (GetKeyState(VK_MENU) & 0x8000)) {
                fileUp(w);
                continue;
            }
            if (m.wParam == VK_RETURN &&
                (m.hwnd == w.address || m.hwnd == w.list)) {
                if (m.hwnd == w.address)
                    fileGo(w);
                else
                    fileAccept(w);
                continue;
            }
        }
        if (!IsDialogMessageW(h, &m)) {
            TranslateMessage(&m);
            DispatchMessageW(&m);
        }
    }
}
static HRESULT filesStatus = S_OK;
static bool browse(const wstring& dest, HWND notify, RECT anchor) {
    if (stopping || !notify)
        return false;
    if (fileThread.joinable()) {
        if (HWND h = fileWindow.load())
            SetForegroundWindow(h);
        return false;
    }
    fileDestination = dest;
    filesReady = false;
    auto f = lookup(groups, dest);
    wstring title = uiText(UiString::FromFile);
    if (f)
        title += L" — " + f->name;
    fileThread = std::thread([notify, anchor, title] {
        FilesWindow state;
        auto hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
        HRESULT status = S_OK;
        try {
            if (!stopping)
                runFiles(state, title, anchor);
        } catch (...) {
            status = winrt::to_hresult();
        }
        if (SUCCEEDED(hr))
            CoUninitialize();
        selectedPaths = std::move(state.picked);
        filesStatus = status;
        filesReady.store(true);
        if (!stopping)
            PostMessageW(notify, kFilesDone, 0, 0);
    });
    return true;
}

static std::vector<App> readAppCatalog() {
    std::vector<App> result;
    com_ptr<IShellItem> folder;
    com_ptr<IEnumShellItems> items;
    if (SUCCEEDED(SHGetKnownFolderItem(FOLDERID_AppsFolder, KF_FLAG_DEFAULT,
                                       nullptr, IID_PPV_ARGS(folder.put()))) &&
        SUCCEEDED(folder->BindToHandler(nullptr, BHID_EnumItems,
                                        IID_PPV_ARGS(items.put())))) {
        while (!stopping) {
            com_ptr<IShellItem> entry;
            ULONG count = 0;
            if (items->Next(1, entry.put(), &count) != S_OK || !count)
                break;
            auto id = display(entry.get(), SIGDN_PARENTRELATIVEPARSING);
            if (id.empty() || prefix(id, kPinPrefix))
                continue;
            auto name = display(entry.get(), SIGDN_NORMALDISPLAY);
            if (!name.empty())
                result.push_back({id, name, L"shell:AppsFolder\\" + id});
        }
    }
    if (result.empty() && !stopping) {
        for (const auto& root : {FOLDERID_Programs, FOLDERID_CommonPrograms}) {
            PWSTR raw = nullptr;
            if (FAILED(SHGetKnownFolderPath(root, 0, nullptr, &raw)))
                continue;
            fs::path path(raw);
            CoTaskMemFree(raw);
            std::error_code ec;
            for (fs::recursive_directory_iterator it(
                     path, fs::directory_options::skip_permission_denied, ec),
                 end;
                 !ec && it != end && !stopping; it.increment(ec)) {
                if (!equal(it->path().extension().wstring(), L".lnk"))
                    continue;
                if (auto a = fromPath(it->path().wstring()))
                    result.push_back(std::move(*a));
            }
        }
    }
    std::sort(result.begin(), result.end(), [](const App& a, const App& b) {
        return _wcsicmp(a.name.c_str(), b.name.c_str()) < 0;
    });
    return result;
}
static bool collectCatalog() {
    if (!catalogThread.joinable() || !catalogReady.load())
        return false;
    catalogThread.join();
    ++catalogGeneration;
    installedApps = std::move(incomingApps);
    catalogFailed = incomingFailed;
    catalogReady = false;
    catalogTime = GetTickCount64();
    return true;
}
static void wantCatalog(HWND notify) {
    collectCatalog();
    if (stopping || !notify || catalogThread.joinable() ||
        (catalogTime &&
         GetTickCount64() - catalogTime < (catalogFailed ? 10000 : 120000)))
        return;
    catalogThread = std::thread([notify] {
        auto init = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
        std::vector<App> apps;
        try {
            if (SUCCEEDED(init) && !stopping)
                apps = readAppCatalog();
        } catch (...) {
        }
        if (SUCCEEDED(init))
            CoUninitialize();
        incomingFailed = apps.empty();
        incomingApps = std::move(apps);
        catalogReady = true;
        if (!stopping)
            PostMessageW(notify, kCatalogDone, 0, 0);
    });
}
using TermList = std::vector<std::wstring_view>;
static TermList searchTerms(const wstring& query) {
    TermList terms;
    for (size_t i = query.find_first_not_of(L" \t"); i != wstring::npos;) {
        auto end = query.find_first_of(L" \t", i);
        terms.emplace_back(query.data() + i,
                           (end == wstring::npos ? query.size() : end) - i);
        i = end == wstring::npos ? end : query.find_first_not_of(L" \t", end);
    }
    return terms;
}
static bool matchName(const wstring& name, const TermList& terms) {
    for (auto term : terms)
        if (FindNLSStringEx(
                LOCALE_NAME_USER_DEFAULT,
                FIND_FROMSTART | NORM_IGNORECASE | NORM_IGNORENONSPACE,
                name.data(), (int)name.size(), term.data(), (int)term.size(),
                nullptr, nullptr, nullptr, 0) < 0)
            return false;
    return true;
}
struct AppSearch {
    struct Entry {
        const App* app;
        size_t id, path;
    };
    std::vector<Entry> entries;
    std::vector<BYTE> used;
    std::optional<uint64_t> generation;

    void build(const std::vector<App>& members,
               const std::vector<App>& pins,
               const std::vector<App>& catalog,
               uint64_t version) {
        generation.reset();
        entries.clear();
        std::map<wstring, size_t, IgnoreCase> ids, paths;
        size_t count = 1;  // Key zero means an app doesn't have an explicit
                           // AppUserModelID.
        auto key = [&](auto& map, const wstring& value) {
            auto [pos, inserted] = map.try_emplace(value, count);
            if (inserted)
                ++count;
            return pos->second;
        };
        auto identify = [&](const App& a) {
            return Entry{&a, a.id.empty() ? 0 : key(ids, a.id),
                         key(paths, a.target)};
        };
        for (const auto& a : members)
            identify(a);
        size_t firstFree = count;
        auto add = [&](const std::vector<App>& apps) {
            for (const auto& a : apps) {
                auto e = identify(a);
                if ((!e.id || e.id >= firstFree) && e.path >= firstFree)
                    entries.push_back(e);
            }
        };
        add(pins);
        add(catalog);
        used.resize(count);
        generation = version;
    }
    void filter(const TermList& terms, std::vector<App>& result) {
        result.clear();
        std::fill(used.begin(), used.end(), 0);
        for (const auto& e : entries) {
            if (used[e.path] || (e.id && used[e.id]) ||
                !matchName(e.app->name, terms))
                continue;
            result.push_back(*e.app);
            used[e.path] = 1;
            if (e.id)
                used[e.id] = 1;
        }
    }
};

static void stopRunningWorker();
static void stopWorkers() {
    stopRunningWorker();
    if (HWND w = fileWindow.load())
        PostMessageW(w, WM_CLOSE, 0, 0);
    if (fileThread.joinable())
        fileThread.join();
    if (catalogThread.joinable())
        catalogThread.join();
}

static void beginAdding(const wstring&);
static bool pinnedFolder(const wstring&);
static void removeFolderPin(wstring);
enum class FolderAction {
    Add = 1,
    Rename,
    Create,
    Remove,
    Integrate,
    Move,
    Open,
    Launch
};
struct MenuAction {
    FolderAction kind;
    wstring folder;
    wstring label;
    wstring target{};
};
static C::Symbol menuSymbol(FolderAction action) {
    using S = C::Symbol;
    switch (action) {
        case FolderAction::Add:
            return S::Add;
        case FolderAction::Rename:
            return S::Rename;
        case FolderAction::Create:
            return S::NewFolder;
        case FolderAction::Remove:
            return S::Delete;
        case FolderAction::Integrate:
            return S::Pin;
        case FolderAction::Move:
            return S::MoveToFolder;
        case FolderAction::Open:
            return S::Folder;
        case FolderAction::Launch:
            return S::OpenFile;
    }
    return S::Folder;
}
static std::vector<MenuAction> taskbarFolderActions(const AppFolder& folder,
                                                    bool offerPin) {
    std::vector<MenuAction> result;
    result.reserve(folder.apps.size() + 5);
    result.push_back(
        {FolderAction::Open, folder.id, uiText(UiString::OpenFolder)});
    for (const auto& app : folder.apps)
        result.push_back(
            {FolderAction::Launch, folder.id, app.name, app.target});
    result.push_back(
        {FolderAction::Add, folder.id, uiText(UiString::AddAppsMenu)});
    result.push_back(
        {FolderAction::Rename, folder.id, uiText(UiString::RenameMenu)});
    if (offerPin)
        result.push_back(
            {FolderAction::Integrate, folder.id, uiText(UiString::Integrate)});
    result.push_back(
        {FolderAction::Remove, folder.id, uiText(UiString::RemoveMenu)});
    return result;
}
static const App* menuMember(const MenuAction& action) {
    const auto* folder = lookup(groups, action.folder);
    if (!folder)
        return nullptr;
    auto it = std::find_if(
        folder->apps.begin(), folder->apps.end(),
        [&](const App& a) { return equal(a.target, action.target); });
    return it != folder->apps.end() ? &*it : nullptr;
}
static void openMenuFolder(const wstring&);
static void launchMenuMember(const MenuAction&);
static std::vector<MenuAction> folderActions(const wstring& dest,
                                             bool offerPin) {
    std::vector<MenuAction> result{
        {FolderAction::Add, dest, uiText(UiString::AddAppsMenu)},
        {FolderAction::Rename, dest, uiText(UiString::RenameMenu)},
        {FolderAction::Create, {}, uiText(UiString::CreateMenu)},
        {FolderAction::Remove, dest, uiText(UiString::RemoveMenu)}};
    if (offerPin)
        result.push_back(
            {FolderAction::Integrate, dest, uiText(UiString::Integrate)});
    return result;
}
static std::vector<MenuAction> appActions(const Model& folders) {
    std::vector<MenuAction> result;
    result.reserve(folders.size());
    for (const auto& f : folders)
        result.push_back(
            {FolderAction::Move, f.id, formatUi(UiString::MoveNamed, f.name)});
    if (result.empty())
        result.push_back(
            {FolderAction::Create, {}, uiText(UiString::CreateMenu)});
    return result;
}
static void runFolderAction(MenuAction action) {
    if (action.kind == FolderAction::Create) {
        closePanel();
        createFolder();
        return;
    }
    auto f = lookup(groups, action.folder);
    if (!f)
        return;
    if (action.kind == FolderAction::Add) {
        beginAdding(action.folder);
        return;
    }
    if (action.kind == FolderAction::Open) {
        openMenuFolder(action.folder);
        return;
    }
    if (action.kind == FolderAction::Launch) {
        launchMenuMember(action);
        return;
    }
    wstring name = f->name;
    closePanel();
    switch (action.kind) {
        case FolderAction::Integrate:
            integrate(action.folder);
            break;
        case FolderAction::Rename: {
            auto n = nameDialog(name, uiText(UiString::RenameTitle));
            if (!n.empty())
                edit([&](Model& m) {
                    if (auto g = lookup(m, action.folder))
                        g->name = n;
                });
            break;
        }
        case FolderAction::Remove:
            if (confirmRemove(name))
                removeFolderPin(action.folder);
            break;
        default:
            break;
    }
}
// This menu belongs to the three-dot button inside our own folder panel.
static void manage(wstring dest);

static void click(Button const& b,
                  std::vector<std::function<void()>>& bag,
                  std::function<void()> fn) {
    auto t = b.Click([fn](auto&&, auto&&) {
        try {
            if (!stopping)
                fn();
        } catch (...) {
            Wh_Log(L"Folder action failed.");
        }
    });
    bag.push_back([w = winrt::make_weak(b), t]() {
        if (auto v = w.get())
            v.Click(t);
    });
}
static void showFolder(const wstring&, FrameworkElement const&);
// The taskbar's XAML island can be only as tall as the taskbar itself.
static constexpr wchar_t kPanelClass[] = L"TaskbarFoldersXaml.AppPanel.04";
enum : UINT {
    kApps = 101,
    kAdd,
    kMore,
    kFilter,
    kLens,
    kPrev,
    kNext,
    kPager,
    kTitle,
    kNameEdit,
    kOpenApp = WM_APP + 501,
    kClosePanel,
    kCheckActivation,
    kPanelFrame,
    kRunningReady,
    kMenuLaunchReady
};
static double motionTime() {
    using namespace std::chrono;
    return duration<double, std::milli>(steady_clock::now().time_since_epoch())
        .count();
}
struct Ease {
    double start{}, end{};
    double since{};
    unsigned duration{};
    bool active(double now) const { return duration && now - since < duration; }
    double value(double now) const {
        if (!active(now))
            return end;
        double left = 1.0 - std::max(0., now - since) / duration;
        return end + (start - end) * left * left * left;
    }
    bool damage(double now, bool& active) {
        if (!duration)
            return false;
        if (this->active(now))
            active = true;
        else
            set(end);
        return true;
    }
    void set(double v) {
        start = end = v;
        duration = 0;
    }
    void to(double v, unsigned ms, double now) {
        if (v == end)
            return;
        start = value(now);
        end = v;
        since = now;
        duration = start == end ? 0 : ms;
    }
};
// A critically damped spring carries both position and signed velocity across
// reversals.
struct Slide : Ease {
    double speed{};
    static constexpr unsigned settleMs = 240;
    static constexpr double damping = .040;
    double value(double now) const {
        if (!active(now))
            return end;
        double t = std::max(0., now - since), offset = start - end;
        return end + (offset + (speed + damping * offset) * t) *
                         std::exp(-damping * t);
    }
    double velocity(double now) const {
        if (!active(now))
            return 0;
        double t = std::max(0., now - since);
        return (speed - damping * (speed + damping * (start - end)) * t) *
               std::exp(-damping * t);
    }
    void from(double position, double rate, double destination, double now) {
        start = position;
        speed = rate;
        end = destination;
        since = now;
        duration = start != end || std::abs(speed) > .001 ? settleMs : 0;
    }
    void to(double v, unsigned, double now) {
        if (v == end)
            return;
        from(value(now), velocity(now), v, now);
    }
};
struct TileMotion {
    Ease hover, press;
    Slide dx, dy;
    POINT lastOffset{};
};
struct IncomingPreview {
    uint64_t token{};
    std::vector<App> apps;
    int slot = -1;
    int originalPage = 0, lastPage = -1;
    POINT lastPoint{};
};
struct DragIcon {
    HWND window{};
    HDC dc{};
    HBITMAP bitmap{};
    HGDIOBJ stock{};
    DWORD* pixels{};
    int side{};
    POINT position{}, samplePosition{};
    double sampledAt{}, vx{}, vy{};
    static constexpr BYTE dragOpacity = 245;
    BYTE opacity = dragOpacity;
    ~DragIcon() {
        if (window)
            DestroyWindow(window);
        if (stock)
            SelectObject(dc, stock);
        if (bitmap)
            DeleteObject(bitmap);
        if (dc)
            DeleteDC(dc);
    }
};
struct ExitEffect {
    std::unique_ptr<DragIcon> icon;
    std::unique_ptr<DWORD[]> original;
    POINT origin{};
    int sourceSide{}, padding{};
    COLORREF accent{};
    double since{}, dx{}, dy{}, frameInterval{}, nextFrame{};
    static constexpr double duration = 360;
};
enum : UINT_PTR {
    kFrameTimer = 87,
    kDragPageTimer,
    kPanelActivationTimer,
    kPeerTimer,
    kRunningTimer,
    kSelectHoldTimer
};
static bool allowMotion() {
    BOOL effects = TRUE;
    HIGHCONTRASTW hc{sizeof(hc)};
    SystemParametersInfoW(SPI_GETHIGHCONTRAST, sizeof(hc), &hc, 0);
    return !(hc.dwFlags & HCF_HIGHCONTRASTON) &&
           SystemParametersInfoW(SPI_GETCLIENTAREAANIMATION, 0, &effects, 0) &&
           effects;
}
// Four columns by four rows. Two pages fit without reloading icons when going
// back.
static constexpr size_t kIconCacheLimit = 32;
struct PopupIcon {
    HICON handle{};
    wstring target;
    uint64_t use{};
};
enum class TileFinish { Plain, Prism, Soft, Line };
struct FolderPalette {
    COLORREF surface{}, foreground{}, buttonFace{}, rim{}, field{}, accent{};
    COLORREF headerLeft{}, headerRight{}, tile{}, tileEdge{};
    COLORREF selectionMark{}, selectionFace{}, selectionHot{};
    TileFinish finish = TileFinish::Plain;
    int radius = 8;
    bool dark = false, highContrast = false;
};
struct AppWindow {
    HWND window;
    DWORD pid;
    bool operator==(const AppWindow&) const = default;
};
using WindowMatches = std::map<wstring, std::vector<AppWindow>, IgnoreCase>;
struct FolderPanel : FolderPalette {
    HWINEVENTHOOK windowHook{};
    std::shared_ptr<const std::vector<App>> windowMembers;
    unsigned windowRevision = ~0u, clickSerial{};
    uint64_t windowRequest{};
    bool runningDirty = true, windowTimer = false;
    wstring windowFolder;
    WindowMatches openWindows;
    std::optional<App> pendingClick;
    bool appearanceReady = false, systemDark = false;
    FolderStyle shownStyle = FolderStyle::Inherit,
                requestedStyle = FolderStyle::Inherit;
    AppearanceSettings appliedSettings{};
    HMENU designMenu{};
    HWND window{}, list{}, title{}, empty{}, add{}, more{}, hover{}, tips{},
        filter{}, lens{};
    HFONT normalFont{}, titleFont{}, paperFont{};
    HBRUSH background{};
    HIMAGELIST icons{};
    std::map<wstring, int> imageSlots;
    UINT dpi = 96;
    RECT anchor{}, bar{};
    wstring folder;
    std::vector<App> apps;
    unsigned rendered = ~0u, viewSerial = 0, choicesRevision = ~0u;
    std::vector<App> pins;
    AppSearch search;
    int cellWidth = 0, cellHeight = 0, iconSize = 0, lineHeight = 0;
    bool browsing = false;
    bool dropRegistered = false, dropHover = false, waitForDrag = false;
    int pressed = -1, dropTarget = -1, contextItem = -1;
    POINT start{};
    bool dragging = false, choosing = false, autoScroll = false,
         filtering = false, toggleOnRelease = false;
    ULONGLONG holdDue = 0;
    bool selectedOnHold = false;
    int searchOrigin = 0;
    std::set<wstring, IgnoreCase> selection;
    std::vector<int> dragIndices;
    std::vector<App> dragApps;
    std::unique_ptr<DragIcon> dragImage;
    POINT dragGrip{};
    int exportHint = 0;
    HWND prev{}, next{}, pager{}, nameEdit{};
    bool renaming = false;
    IncomingPreview incoming;
    struct TransferDrag {
        wstring source;
        std::vector<App> apps;
        RECT anchor;
        int page;
        uint64_t token{};
    };
    std::optional<TransferDrag> transfer;
    wstring hoverFolder;
    POINT dragPoint{};
    HBRUSH searchBrush{};
    RECT rest{}, searchRect{};
    bool motion = false, frame = false, layered = false, closing = false;
    Ease reveal, arrival;
    int slide = 0, hot = -1, columns = 1, capacity = 1, page = 0, wheel = 0,
        edgeDir = 0;
    std::vector<TileMotion> tiles;
    std::map<HWND, Ease> controls;
    std::vector<PopupIcon> glyphs;
    uint64_t iconClock = 0;
    ULONGLONG timingReadAt = 0;
    HMONITOR frameMonitor{};
    double displayPhase = 0;
    double frameInterval = 1000. / 60, frameDue = 0, frameSample = 0;
    HANDLE frameTimer{};
    PTP_WAIT frameWait{};
    bool fallbackTimer = false;
    std::set<wstring> failedIcons;
    std::optional<PinnedIconLinks> pinnedIcons;
    HDC previousPage{};
    HBITMAP previousBitmap{};
    HGDIOBJ previousStock{};
    int previousWidth = 0, previousHeight = 0;
    HRGN paintRegion{};
    HDC buffer{};
    HBITMAP bitmap{};
    HGDIOBJ stockBitmap{};
    int bufferWidth = 0, bufferHeight = 0;
};
[[clang::no_destroy]] static FolderPanel panel;
static uint64_t nextPreviewToken = 0;
static void refreshPanel();
static void findInFolder();
static void requestWindowScan(bool forClick = false);
static void dismissPanel();
static void scheduleWindowScan();
static void endFolderSearch();
static void releasePreviousPage();
static void positionIncoming(POINT screen);
static bool primaryMouseDown();
static void trackPeer(POINT screen);
static void enterPeer(const wstring& dest, POINT screen);
static void endPeer(std::optional<POINT> drop);
static wstring dropFolderAt(POINT);
static void beginRename();
static void endRename(bool save, bool restoreFocus);
static void closePanel() {
    if (panel.window)
        DestroyWindow(panel.window);
}
static int px(int dip) {
    return MulDiv(dip, panel.dpi, 96);
}
static bool isIn(RECT r, POINT p) {
    return PtInRect(&r, p) != FALSE;
}
static bool validIndex(int i, size_t count) {
    return i >= 0 && static_cast<size_t>(i) < count;
}
static bool pastDragThreshold(POINT start, POINT current, UINT dpi) {
    return std::abs(current.x - start.x) >=
               GetSystemMetricsForDpi(SM_CXDRAG, dpi) ||
           std::abs(current.y - start.y) >=
               GetSystemMetricsForDpi(SM_CYDRAG, dpi);
}
static bool shellFileWindow(HWND window, bool includeExplorer) {
    if (!window)
        return false;
    if (window == GetShellWindow())
        return true;
    wchar_t name[32]{};
    GetClassNameW(window, name, ARRAYSIZE(name));
    if (includeExplorer && !wcscmp(name, L"CabinetWClass"))
        return true;
    if (wcscmp(name, L"WorkerW") && wcscmp(name, L"Progman"))
        return false;
    DWORD shellPid{}, windowPid{};
    GetWindowThreadProcessId(GetShellWindow(), &shellPid);
    GetWindowThreadProcessId(window, &windowPid);
    return shellPid && windowPid == shellPid &&
           FindWindowExW(window, nullptr, L"SHELLDLL_DefView", nullptr);
}
static void moveAppOutside(int index, POINT screenPoint);
enum class FileSurface { None, Desktop, Folder };
static bool windowClass(HWND w, PCWSTR expected) {
    wchar_t name[64]{};
    return w && GetClassNameW(w, name, ARRAYSIZE(name)) &&
           !wcscmp(name, expected);
}
static FileSurface fileSurface(HWND hit, HWND root) {
    if (!root)
        return FileSurface::None;
    if (shellFileWindow(root, false))
        return FileSurface::Desktop;
    if (!windowClass(root, L"CabinetWClass"))
        return FileSurface::None;
    for (auto child = hit; child && child != root; child = GetParent(child))
        if (windowClass(child, L"SHELLDLL_DefView"))
            return FileSurface::Folder;
    return FileSurface::None;
}
// Resolve the view under the pointer, including the selected Explorer tab. A
// toolbar or a virtual location such as Home is not a filesystem destination.
static com_ptr<IFolderView> fileView(IDispatch* entry, HWND hit, bool desktop) {
    com_ptr<::IServiceProvider> services;
    com_ptr<IShellBrowser> browser;
    com_ptr<IShellView> view;
    HWND window{};
    if (!entry || FAILED(entry->QueryInterface(IID_PPV_ARGS(services.put()))) ||
        FAILED(services->QueryService(SID_STopLevelBrowser,
                                      IID_PPV_ARGS(browser.put()))) ||
        FAILED(browser->QueryActiveShellView(view.put())) ||
        FAILED(view->GetWindow(&window)) ||
        (!desktop && window != hit && !IsChild(window, hit)))
        return {};
    return view.try_as<IFolderView>();
}
static com_ptr<IFolderView> findFileView(HWND hit, FileSurface surface) {
    com_ptr<IShellWindows> windows;
    if (FAILED(CoCreateInstance(CLSID_ShellWindows, nullptr,
                                CLSCTX_LOCAL_SERVER,
                                IID_PPV_ARGS(windows.put()))))
        return {};
    VARIANT loc{}, empty{};
    com_ptr<IDispatch> entry;
    if (surface == FileSurface::Desktop) {
        loc.vt = VT_I4;
        loc.lVal = CSIDL_DESKTOP;
        long window{};
        if (windows->FindWindowSW(&loc, &empty, SWC_DESKTOP, &window,
                                  SWFO_NEEDDISPATCH, entry.put()) != S_OK)
            return {};
        return fileView(entry.get(), hit, true);
    }
    long count{};
    if (FAILED(windows->get_Count(&count)))
        return {};
    for (long i = 0; i < count; ++i) {
        VARIANT index{};
        index.vt = VT_I4;
        index.lVal = i;
        entry = nullptr;
        if (windows->Item(index, entry.put()) == S_OK)
            if (auto view = fileView(entry.get(), hit, false))
                return view;
    }
    return {};
}
struct FileDestination {
    fs::path directory;
    com_ptr<IFolderView> desktopView;
    HWND list{};
};
// Modern Explorer's items have no individual HWND. Only hit-test the contents
// pane once on release, and ask its own accessibility provider for the item.
static std::optional<wstring> accessibleItemName(IAccessible* provider,
                                                 VARIANT child) {
    VARIANT role{};
    provider->get_accRole(child, &role);
    bool item = role.vt == VT_I4 && (role.lVal == ROLE_SYSTEM_LISTITEM ||
                                     role.lVal == ROLE_SYSTEM_OUTLINEITEM ||
                                     role.lVal == ROLE_SYSTEM_ROW);
    VariantClear(&role);
    if (!item)
        return {};
    BSTR text{};
    provider->get_accName(child, &text);
    wstring name = text ? wstring(text, SysStringLen(text)) : wstring{};
    SysFreeString(text);
    return name;
}
static std::optional<wstring> explorerItemAt(HWND contents, POINT screen) {
    com_ptr<IAccessible> current;
    if (FAILED(AccessibleObjectFromWindow(contents, OBJID_CLIENT,
                                          IID_PPV_ARGS(current.put()))))
        return {};
    for (int depth = 0; depth < 12; ++depth) {
        VARIANT child{};
        if (FAILED(current->accHitTest(screen.x, screen.y, &child)))
            return {};
        if (child.vt == VT_DISPATCH && child.pdispVal) {
            com_ptr<IAccessible> next;
            auto hr = child.pdispVal->QueryInterface(IID_PPV_ARGS(next.put()));
            VariantClear(&child);
            if (FAILED(hr) || !next)
                return {};
            VARIANT self{};
            self.vt = VT_I4;
            self.lVal = CHILDID_SELF;
            // A row can itself contain a text label, an icon, and several
            // columns.
            if (auto name = accessibleItemName(next.get(), self))
                return name;
            if (next.get() == current.get())
                return {};
            current = std::move(next);
            continue;
        }
        if (child.vt != VT_I4) {
            VariantClear(&child);
            return {};
        }
        return accessibleItemName(current.get(), child);
    }
    return {};
}
static FileDestination childDirectory(IShellItem* item) {
    SFGAOF flags{};
    if (!item ||
        FAILED(item->GetAttributes(
            SFGAO_FOLDER | SFGAO_FILESYSTEM | SFGAO_STREAM, &flags)) ||
        (flags & (SFGAO_FOLDER | SFGAO_FILESYSTEM | SFGAO_STREAM)) !=
            (SFGAO_FOLDER | SFGAO_FILESYSTEM))
        return {};
    return {fs::path(display(item, SIGDN_FILESYSPATH))};
}
static FileDestination dropDirectory(HWND hit, HWND root, POINT screen) {
    auto surface = fileSurface(hit, root);
    if (surface == FileSurface::None)
        return {};
    auto view = findFileView(hit, surface);
    FileDestination destination;
    if (surface == FileSurface::Desktop) {
        destination.directory = knownPath(FOLDERID_Desktop);
        destination.desktopView = view;
    } else {
        com_ptr<IShellFolder> folder;
        com_ptr<IShellItem> item;
        if (!view || FAILED(view->GetFolder(IID_PPV_ARGS(folder.put()))) ||
            FAILED(SHGetItemFromObject(folder.get(), IID_PPV_ARGS(item.put()))))
            return {};
        destination.directory = display(item.get(), SIGDN_FILESYSPATH);
    }
    if (destination.directory.empty())
        return {};
    // Resolve a child under the pointer. Dropping on an ordinary file cancels.
    DWORD pid{};
    GetWindowThreadProcessId(hit, &pid);
    if (view && windowClass(hit, WC_LISTVIEWW) &&
        pid == GetCurrentProcessId()) {
        destination.list = hit;
        LVHITTESTINFO info{};
        info.pt = screen;
        ScreenToClient(hit, &info.pt);
        int index = ListView_HitTest(hit, &info);
        if (index >= 0 && (info.flags & LVHT_ONITEM)) {
            auto version = view.try_as<IFolderView2>();
            com_ptr<IShellItem> item;
            if (!version ||
                FAILED(version->GetItem(index, IID_PPV_ARGS(item.put()))))
                return {};
            return childDirectory(item.get());
        }
    } else if (view) {
        if (auto name = explorerItemAt(hit, screen)) {
            if (name->empty() || *name == L"." || *name == L".." ||
                name->find_first_of(L"\\/:") != wstring::npos)
                return {};
            com_ptr<IShellFolder> folder;
            com_ptr<IShellItem> item;
            PITEMID_CHILD child{};
            if (FAILED(view->GetFolder(IID_PPV_ARGS(folder.put()))) ||
                FAILED(folder->ParseDisplayName(nullptr, nullptr, name->data(),
                                                nullptr, &child, nullptr)))
                return {};
            auto hr = SHCreateItemWithParent(nullptr, folder.get(), child,
                                             IID_PPV_ARGS(item.put()));
            CoTaskMemFree(child);
            if (FAILED(hr))
                return {};
            return childDirectory(item.get());
        }
    }
    return destination;
}
static bool identicalShortcut(const fs::path& a, const fs::path& b) {
    std::error_code ea, eb;
    auto size = fs::file_size(a, ea);
    if (ea || size != fs::file_size(b, eb) || eb)
        return false;
    std::ifstream x(a, std::ios::binary), y(b, std::ios::binary);
    std::array<char, 4096> left{}, right{};
    while (size) {
        auto count = static_cast<std::streamsize>(
            std::min<uintmax_t>(size, left.size()));
        if (!x.read(left.data(), count) || !y.read(right.data(), count) ||
            memcmp(left.data(), right.data(), count))
            return false;
        size -= count;
    }
    return x.is_open() && y.is_open();
}
// Publish a completed shortcut without replacing an existing file. The app
// itself always stays at its original location, including when it is a
// standalone .exe.
static fs::path placeShortcut(const App& app, const fs::path& directory) {
    fs::path source(launchTarget(app));
    auto ext = source.extension().wstring();
    bool copy = shortcutExtension(ext);
    std::error_code ec;
    if (!copy && !prefix(app.target, L"shell:AppsFolder\\") &&
        (!equal(ext, L".exe") || !source.is_absolute() ||
         !fs::is_regular_file(source, ec)))
        return {};
    if (copy && fs::is_regular_file(source, ec) &&
        fs::equivalent(source.parent_path(), directory, ec))
        return source;
    if (!fs::is_directory(directory, ec))
        return {};
    auto temp = directory / (L".TaskbarFolders-" + guid() + L".tmp");
    HANDLE reservation =
        CreateFileW(temp.c_str(), GENERIC_WRITE, 0, nullptr, CREATE_NEW,
                    FILE_ATTRIBUTE_NORMAL, nullptr);
    if (reservation == INVALID_HANDLE_VALUE)
        return {};
    CloseHandle(reservation);
    struct Temporary {
        const fs::path& path;
        bool owned = true;
        ~Temporary() {
            if (owned && !DeleteFileW(path.c_str()) &&
                GetLastError() == ERROR_ACCESS_DENIED) {
                SetFileAttributesW(path.c_str(), FILE_ATTRIBUTE_NORMAL);
                DeleteFileW(path.c_str());
            }
        }
    } cleanup{temp};
    if (copy) {
        if (!CopyFileW(source.c_str(), temp.c_str(), FALSE))
            return {};
    } else {
        com_ptr<IShellLinkW> link;
        auto target = shellItem(app.target);
        if (!target || FAILED(CoCreateInstance(CLSID_ShellLink, nullptr,
                                               CLSCTX_INPROC_SERVER,
                                               IID_PPV_ARGS(link.put()))))
            return {};
        PIDLIST_ABSOLUTE id{};
        if (FAILED(SHGetIDListFromObject(target.get(), &id)))
            return {};
        auto hr = link->SetIDList(id);
        CoTaskMemFree(id);
        if (FAILED(hr))
            return {};
        if (source.is_absolute())
            link->SetWorkingDirectory(source.parent_path().c_str());
        if (FAILED(link.as<IPersistFile>()->Save(temp.c_str(), TRUE)))
            return {};
        ext = L".lnk";
    }
    auto name =
        linkTitle(app.name.empty() ? source.stem().wstring() : app.name);
    if (copy && equal(fs::path(name).extension().wstring(), ext) &&
        name.size() > ext.size())
        name.resize(name.size() - ext.size());
    if (name.size() > 190) {
        name.resize(190);
        if (IS_HIGH_SURROGATE(name.back()))
            name.pop_back();
    }
    for (unsigned i = 1; i <= 9999; ++i) {
        auto leaf = name +
                    (i == 1 ? wstring{} : L" (" + std::to_wstring(i) + L")") +
                    ext;
        auto output = directory / leaf;
        if (MoveFileExW(temp.c_str(), output.c_str(), 0)) {
            cleanup.owned = false;
            notifyLink(output, SHCNE_CREATE);
            return output;
        }
        auto error = GetLastError();
        if (error != ERROR_ALREADY_EXISTS && error != ERROR_FILE_EXISTS)
            return {};
        if (identicalShortcut(temp, output))
            return output;
    }
    return {};
}
static void revealDesktopShortcuts(const FileDestination& target,
                                   const std::vector<fs::path>& files,
                                   POINT screen) {
    const auto& view = target.desktopView;
    if (!view || std::none_of(files.begin(), files.end(),
                              [](const auto& path) { return !path.empty(); }))
        return;
    com_ptr<IShellFolder> folder;
    view->GetFolder(IID_PPV_ARGS(folder.put()));
    const bool position = target.list && view->GetAutoArrange() == S_FALSE &&
                          ScreenToClient(target.list, &screen);
    POINT spacing{72, 80};
    if (position)
        view->GetSpacing(&spacing);
    std::vector<std::unique_ptr<ITEMIDLIST, decltype(&CoTaskMemFree)>> owned;
    std::vector<PCUITEMID_CHILD> children;
    std::vector<POINT> points;
    bool missing = false;
    for (const auto& path : files) {
        if (path.empty())
            continue;
        auto leaf = path.filename().wstring();
        PITEMID_CHILD id{};
        auto hr = folder
                      ? folder->ParseDisplayName(nullptr, nullptr, leaf.data(),
                                                 nullptr, &id, nullptr)
                      : E_FAIL;
        owned.emplace_back(id, &CoTaskMemFree);
        if (FAILED(hr) || !id) {
            missing = true;
            continue;
        }
        POINT current{};
        if (!missing && FAILED(view->GetItemPosition(id, &current)))
            missing = true;
        if (position) {
            auto i = static_cast<LONG>(points.size());
            children.push_back(id);
            points.push_back(
                {screen.x + i % 4 * spacing.x, screen.y + i / 4 * spacing.y});
        }
    }
    // Explorer can still have the directory change queued when the worker
    // finishes. Refresh that view once for the whole drop, before asking it to
    // position the icons.
    if (missing) {
        auto shell = view.try_as<IShellView>();
        if (!shell || FAILED(shell->Refresh()))
            notifyLink(target.directory, SHCNE_UPDATEDIR);
    }
    if (!children.empty())
        view->SelectAndPositionItems(static_cast<UINT>(children.size()),
                                     children.data(), points.data(),
                                     SVSI_POSITIONITEM | SVSI_NOSTATECHANGE);
}
static void dragExportHint(FileSurface surface, POINT screen = {}) {
    if (!panel.tips)
        return;
    auto next = static_cast<int>(surface);
    if (!next && !panel.exportHint)
        return;
    TOOLINFOW tool{sizeof(tool)};
    tool.hwnd = panel.window;
    tool.uId = kApps;
    tool.uFlags = TTF_TRACK | TTF_ABSOLUTE | TTF_TRANSPARENT;
    if (panel.exportHint != next) {
        if (panel.exportHint)
            SendMessageW(panel.tips, TTM_TRACKACTIVATE, FALSE, (LPARAM)&tool);
        if (next) {
            tool.lpszText = const_cast<PWSTR>(uiText(
                surface == FileSurface::Desktop ? UiString::DropDesktop
                                                : UiString::DropDirectory));
            if (!panel.exportHint)
                SendMessageW(panel.tips, TTM_ADDTOOLW, 0, (LPARAM)&tool);
            else
                SendMessageW(panel.tips, TTM_UPDATETIPTEXTW, 0, (LPARAM)&tool);
        } else
            SendMessageW(panel.tips, TTM_DELTOOLW, 0, (LPARAM)&tool);
    }
    if (next) {
        SendMessageW(panel.tips, TTM_TRACKPOSITION, 0,
                     MAKELPARAM(screen.x + px(18), screen.y + px(26)));
        if (next != panel.exportHint)
            SendMessageW(panel.tips, TTM_TRACKACTIVATE, TRUE, (LPARAM)&tool);
    }
    panel.exportHint = next;
}
static void updateExportHint(POINT screen) {
    auto hit = WindowFromPoint(screen);
    dragExportHint(fileSurface(hit, GetAncestor(hit, GA_ROOT)), screen);
}
static bool reorder(Model& m,
                    const wstring& id,
                    const App& source,
                    const App& target) {
    auto f = lookup(m, id);
    if (!f)
        return false;
    auto& apps = f->apps;
    auto locate = [&](const App& app) {
        return std::find_if(apps.begin(), apps.end(),
                            [&](const App& x) { return sameApp(x, app); });
    };
    auto a = locate(source), b = locate(target);
    if (a == apps.end() || b == apps.end() || a == b)
        return false;
    if (a < b)
        std::rotate(a, a + 1, b + 1);
    else
        std::rotate(b, a, a + 1);
    return true;
}
static RECT placePanel(RECT work,
                       RECT bar,
                       RECT anchor,
                       int width,
                       int height,
                       int gap) {
    width = std::clamp<int>(width, 1, std::max(1L, work.right - work.left));
    auto above =
        std::clamp(std::min(anchor.top, bar.top) - gap, work.top, work.bottom);
    auto below = std::clamp(std::max(anchor.bottom, bar.bottom) + gap, work.top,
                            work.bottom);
    auto roomAbove = above - work.top, roomBelow = work.bottom - below;
    bool top = height <= roomAbove || roomAbove >= roomBelow;
    height =
        std::min<int>(height, std::max<LONG>(1, top ? roomAbove : roomBelow));
    auto left = std::clamp((anchor.left + anchor.right - width) / 2, work.left,
                           work.right - width);
    auto y = top ? above - height : below;
    return {left, y, left + width, y + height};
}
struct AppRects {
    RECT cell{}, icon{}, label{};
};
static int firstItem() {
    return panel.page * panel.capacity;
}
static int pageCount() {
    int pages = ((int)panel.apps.size() + panel.capacity - 1) / panel.capacity;
    return std::max(1, pages);
}
static int pageItems() {
    return std::clamp((int)panel.apps.size() - firstItem(), 0, panel.capacity);
}
static bool onPage(int n) {
    return n >= firstItem() && n < firstItem() + pageItems();
}
static int selectedApp() {
    int local = ListView_GetNextItem(panel.list, -1, LVNI_FOCUSED);
    return local < 0 ? -1 : firstItem() + local;
}
static bool appRects(int index, AppRects& out) {
    if (!onPage(index) || !panel.cellWidth)
        return false;
    int local = index - firstItem(),
        x = (local % panel.columns) * panel.cellWidth,
        y = (local / panel.columns) * panel.cellHeight;
    int cx = x + panel.cellWidth / 2;
    out.cell = {x, y, x + panel.cellWidth, y + panel.cellHeight};
    out.icon = {cx - panel.iconSize / 2, y + px(5),
                cx + (panel.iconSize + 1) / 2, y + px(5) + panel.iconSize};
    int top = out.icon.bottom + px(5);
    out.label = {x + px(3), top, x + panel.cellWidth - px(3),
                 top + 2 * panel.lineHeight};
    return true;
}
static int hitApp(POINT p) {
    RECT bounds{};
    GetClientRect(panel.list, &bounds);
    if (!isIn(bounds, p) || !panel.cellWidth || !panel.cellHeight)
        return -1;
    int col = p.x / panel.cellWidth,
        idx = firstItem() + (p.y / panel.cellHeight) * panel.columns + col;
    return col < panel.columns && onPage(idx) ? idx : -1;
}
static int reorderSlot(POINT p) {
    int next = hitApp(p);
    if (next < 0 && panel.dragging && panel.page == pageCount() - 1 &&
        pageItems()) {
        RECT area{};
        GetClientRect(panel.list, &area);
        if (isIn(area, p) && panel.cellWidth && panel.cellHeight &&
            p.x / panel.cellWidth < panel.columns &&
            (p.y / panel.cellHeight) * panel.columns + p.x / panel.cellWidth >=
                pageItems())
            next = int(panel.apps.size()) - 1;
    }
    AppRects previous;
    if (next >= 0 && next != panel.dropTarget &&
        appRects(panel.dropTarget, previous)) {
        // Crossing a boundary deliberately changes slots. A few pixels of hand
        // jitter don't swap them back.
        InflateRect(&previous.cell, std::min(px(5), panel.cellWidth / 8),
                    std::min(px(5), panel.cellHeight / 8));
        if (isIn(previous.cell, p))
            return panel.dropTarget;
    }
    return next;
}
struct MonitorTiming {
    double interval = 1000. / 60, phase = 0;
};
static double refreshPeriod(DISPLAYCONFIG_RATIONAL rate) {
    return rate.Numerator && rate.Denominator
               ? 1000. * rate.Denominator / rate.Numerator
               : 0;
}
static MonitorTiming monitorTiming(HMONITOR display) {
    MONITORINFOEXW monitor{};
    monitor.cbSize = sizeof(monitor);
    if (!GetMonitorInfoW(display, &monitor))
        return {};
    DEVMODEW mode{};
    mode.dmSize = sizeof(mode);
    double nominal = 0;
    if (EnumDisplaySettingsW(monitor.szDevice, ENUM_CURRENT_SETTINGS, &mode) &&
        mode.dmDisplayFrequency > 1)
        nominal = 1000. / mode.dmDisplayFrequency;
    DWM_TIMING_INFO timing{};
    timing.cbSize = sizeof(timing);
    LARGE_INTEGER frequency{}, counter{};
    if ((monitor.dwFlags & MONITORINFOF_PRIMARY) &&
        SUCCEEDED(DwmGetCompositionTimingInfo(nullptr, &timing)) &&
        timing.qpcRefreshPeriod && timing.qpcVBlank &&
        QueryPerformanceFrequency(&frequency) &&
        QueryPerformanceCounter(&counter)) {
        double unit = 1000. / frequency.QuadPart,
               period = timing.qpcRefreshPeriod * unit;
        if (!nominal || std::abs(period - nominal) < nominal * .025)
            return {period, motionTime() - (double(counter.QuadPart) -
                                            double(timing.qpcVBlank)) *
                                               unit};
    }
    // A second monitor can have a different (including fractional) refresh
    // rate.
    constexpr UINT32 flags = QDC_ONLY_ACTIVE_PATHS;
    for (int attempt = 0; attempt < 2; ++attempt) {
        UINT32 count{}, modes{};
        if (GetDisplayConfigBufferSizes(flags, &count, &modes) !=
                ERROR_SUCCESS ||
            !count || !modes)
            break;
        std::vector<DISPLAYCONFIG_PATH_INFO> paths(count);
        std::vector<DISPLAYCONFIG_MODE_INFO> modeInfo(modes);
        auto result = QueryDisplayConfig(flags, &count, paths.data(), &modes,
                                         modeInfo.data(), nullptr);
        if (result == ERROR_INSUFFICIENT_BUFFER)
            continue;
        if (result != ERROR_SUCCESS)
            break;
        for (UINT32 i = 0; i < count; ++i) {
            auto& path = paths[i];
            DISPLAYCONFIG_SOURCE_DEVICE_NAME source{};
            source.header = {DISPLAYCONFIG_DEVICE_INFO_GET_SOURCE_NAME,
                             sizeof(source), path.sourceInfo.adapterId,
                             path.sourceInfo.id};
            if (DisplayConfigGetDeviceInfo(&source.header) == ERROR_SUCCESS &&
                !_wcsicmp(source.viewGdiDeviceName, monitor.szDevice))
                if (double interval =
                        refreshPeriod(path.targetInfo.refreshRate))
                    return {interval, 0};
        }
        break;
    }
    return nominal ? MonitorTiming{nominal, 0} : MonitorTiming{};
}
static void syncPanelTiming(bool starting) {
    auto coarse = GetTickCount64();
    bool expired = !panel.timingReadAt || coarse - panel.timingReadAt >= 2000;
    if (!starting && !expired)
        return;
    auto display = MonitorFromWindow(panel.window, MONITOR_DEFAULTTONEAREST);
    if (expired || display != panel.frameMonitor) {
        auto timing = monitorTiming(display);
        bool sameCadence = display == panel.frameMonitor &&
                           std::abs(panel.frameInterval - timing.interval) <
                               timing.interval * .0001;
        if (panel.fallbackTimer && panel.frameInterval != timing.interval)
            SetTimer(panel.window, kFrameTimer,
                     std::max(1u, (UINT)std::lround(timing.interval)), nullptr);
        panel.frameInterval = timing.interval;
        panel.displayPhase = timing.phase;
        panel.frameMonitor = display;
        panel.timingReadAt = coarse;
        if (!starting && sameCadence)
            return;
    }
    double now = panel.frameDue = motionTime();
    if (panel.displayPhase) {
        // Reserve a fraction of one refresh for painting, including on
        // high-refresh displays.
        double lead = std::min(1.5, panel.frameInterval * .10);
        panel.frameDue -=
            std::fmod(now - panel.displayPhase + lead, panel.frameInterval);
        if (panel.frameDue > now)
            panel.frameDue -= panel.frameInterval;
    }
}
static void CALLBACK postPanelFrame(PTP_CALLBACK_INSTANCE,
                                    void* window,
                                    PTP_WAIT,
                                    TP_WAIT_RESULT) {
    PostMessageW(static_cast<HWND>(window), kPanelFrame, 0, 0);
}
static bool schedulePanelFrame() {
    syncPanelTiming(false);
    if (panel.fallbackTimer)
        return true;
    if (!panel.frameTimer) {
        panel.frameTimer = CreateWaitableTimerExW(
            nullptr, nullptr, CREATE_WAITABLE_TIMER_HIGH_RESOLUTION,
            SYNCHRONIZE | TIMER_MODIFY_STATE);
        if (!panel.frameTimer)
            panel.frameTimer = CreateWaitableTimerW(nullptr, FALSE, nullptr);
        if (panel.frameTimer)
            panel.frameWait =
                CreateThreadpoolWait(postPanelFrame, panel.window, nullptr);
    }
    if (panel.frameWait) {
        double now = motionTime();
        panel.frameDue += panel.frameInterval;
        if (panel.frameDue <= now)
            panel.frameDue +=
                (std::floor((now - panel.frameDue) / panel.frameInterval) + 1) *
                panel.frameInterval;
        LARGE_INTEGER relative{};
        relative.QuadPart =
            -std::max(1LL, std::llround((panel.frameDue - now) * 10000));
        SetThreadpoolWait(panel.frameWait, panel.frameTimer, nullptr);
        if (SetWaitableTimer(panel.frameTimer, &relative, 0, nullptr, nullptr,
                             FALSE))
            return true;
        SetThreadpoolWait(panel.frameWait, nullptr, nullptr);
    }
    panel.fallbackTimer =
        SetTimer(panel.window, kFrameTimer,
                 (UINT)std::lround(panel.frameInterval), nullptr) != 0;
    return panel.fallbackTimer;
}
static void stopPanelFrames() {
    if (panel.fallbackTimer) {
        KillTimer(panel.window, kFrameTimer);
        panel.fallbackTimer = false;
    }
    if (panel.frameWait) {
        SetThreadpoolWait(panel.frameWait, nullptr, nullptr);
        CancelWaitableTimer(panel.frameTimer);
        WaitForThreadpoolWaitCallbacks(panel.frameWait, TRUE);
        MSG old{};
        while (PeekMessageW(&old, panel.window, kPanelFrame, kPanelFrame,
                            PM_REMOVE)) {
        }
    }
    panel.frame = false;
}
static void wakeMotion() {
    if (!panel.motion || panel.frame || !panel.window)
        return;
    syncPanelTiming(true);
    panel.frameSample = motionTime();
    panel.frame = schedulePanelFrame();
}
template <class Motion>
static void target(Motion& e,
                   double v,
                   unsigned ms,
                   double now = motionTime()) {
    if (!panel.motion)
        e.set(v);
    else {
        e.to(v, ms, now);
        if (e.active(now)) {
            wakeMotion();
            if (!panel.frame)
                e.set(v);
        }
    }
}
static COLORREF blendColor(COLORREF a, COLORREF b, double f) {
    auto mix = [f](int x, int y) {
        return int(std::lround(x + (y - x) * std::clamp(f, 0.0, 1.0)));
    };
    return RGB(mix(GetRValue(a), GetRValue(b)), mix(GetGValue(a), GetGValue(b)),
               mix(GetBValue(a), GetBValue(b)));
}
static void rounded(HDC dc, RECT r, COLORREF face, int radius) {
    auto pen = SelectObject(dc, GetStockObject(NULL_PEN)),
         brush = SelectObject(dc, GetStockObject(DC_BRUSH));
    auto old = SetDCBrushColor(dc, face);
    RoundRect(dc, r.left, r.top, r.right + 1, r.bottom + 1, radius * 2,
              radius * 2);
    SetDCBrushColor(dc, old);
    SelectObject(dc, pen);
    SelectObject(dc, brush);
}
static constexpr COLORREF swatch(unsigned rgb) {
    return RGB(rgb >> 16, (rgb >> 8) & 255, rgb & 255);
}
static double colorLuminance(COLORREF color) {
    auto linear = [](int channel) {
        double s = channel / 255.;
        return s <= .04045 ? s / 12.92 : std::pow((s + .055) / 1.055, 2.4);
    };
    return .2126 * linear(GetRValue(color)) + .7152 * linear(GetGValue(color)) +
           .0722 * linear(GetBValue(color));
}
static double contrast(COLORREF a, COLORREF b) {
    double x = colorLuminance(a), y = colorLuminance(b);
    return (std::max(x, y) + .05) / (std::min(x, y) + .05);
}
static COLORREF windowsAccent() {
    using namespace W::UI::ViewManagement;
    try {
        auto color = UISettings().GetColorValue(UIColorType::Accent);
        return RGB(color.R, color.G, color.B);
    } catch (const winrt::hresult_error&) {
    }
    DWORD argb{};
    BOOL opaque{};
    if (SUCCEEDED(DwmGetColorizationColor(&argb, &opaque)))
        return swatch(argb & 0xffffff);
    return GetSysColor(COLOR_HIGHLIGHT);
}
static FolderPalette paletteFor(FolderStyle style,
                                bool systemDark,
                                COLORREF accent,
                                bool highContrast,
                                COLORREF selection = CLR_INVALID) {
    FolderPalette p;
    if (highContrast) {
        p.highContrast = true;
        p.surface = p.field = p.tile = p.headerLeft = p.headerRight =
            GetSysColor(COLOR_WINDOW);
        p.foreground = GetSysColor(COLOR_WINDOWTEXT);
        p.buttonFace = GetSysColor(COLOR_BTNFACE);
        p.rim = p.tileEdge = p.accent = GetSysColor(COLOR_HIGHLIGHT);
        p.dark = colorLuminance(p.surface) < .2;
        p.selectionMark = GetSysColor(COLOR_HIGHLIGHTTEXT);
        p.selectionFace = p.selectionHot = p.accent;
        return p;
    }
    // Canvas, text, emphasis, controls and header are tuned together for each
    // material.
    unsigned canvas, ink, focus, control, edge, field, left, right, tile,
        tileEdge;
    switch (style) {
        case FolderStyle::Prism:
            if (systemDark) {
                canvas = 0x202733;
                ink = 0xf3f8ff;
                focus = 0x8ee5f7;
                control = 0x3c5368;
                edge = 0x64768f;
                field = 0x2c3948;
                left = 0x383348;
                right = 0x234653;
                tile = 0x293443;
                tileEdge = 0x49576c;
            } else {
                canvas = 0xeef3fa;
                ink = 0x17273b;
                focus = 0x006e83;
                control = 0xc5ddeb;
                edge = 0x8fa7bf;
                field = 0xe4edf6;
                left = 0xe7dff7;
                right = 0xcceef0;
                tile = 0xf6f9fd;
                tileEdge = 0xb2c3d5;
            }
            p.finish = TileFinish::Prism;
            p.radius = 11;
            break;
        case FolderStyle::Nord:
            canvas = 0x292f3b;
            ink = 0xeceff4;
            focus = 0x8fbcbb;
            control = 0x455368;
            edge = 0x596779;
            field = 0x333d4e;
            left = 0x374355;
            right = 0x2f414a;
            tile = 0x323b4a;
            tileEdge = 0x4a5869;
            p.finish = TileFinish::Soft;
            p.radius = 6;
            break;
        case FolderStyle::Mocha:
            canvas = 0x23222e;
            ink = 0xf0e9fb;
            focus = 0xcba6f7;
            control = 0x504358;
            edge = 0x6d6074;
            field = 0x34303e;
            left = 0x443448;
            right = 0x322d43;
            tile = 0x302b3a;
            tileEdge = 0x433a4e;
            p.finish = TileFinish::Soft;
            p.radius = 13;
            break;
        case FolderStyle::Oled:
            canvas = 0x000000;
            ink = 0xf4f5f7;
            focus = 0x79ddbc;
            control = 0x1b2524;
            edge = 0x495250;
            field = 0x101514;
            left = right = tile = canvas;
            tileEdge = 0x252b2a;
            p.finish = TileFinish::Line;
            p.radius = 5;
            break;
        case FolderStyle::Paper:
            canvas = 0xf7f1e7;
            ink = 0x332b25;
            focus = 0x8b4129;
            control = 0xe7d8c7;
            edge = 0xb5a28d;
            field = 0xf1e8dc;
            left = 0xf1e5d4;
            right = 0xf7f1e7;
            tile = 0xfbf7ef;
            tileEdge = 0xd9cbbb;
            p.finish = TileFinish::Line;
            p.radius = 4;
            break;
        default:
            canvas = systemDark ? 0x272930 : 0xf7f8fb;
            ink = systemDark ? 0xf4f5fa : 0x191c23;
            focus = systemDark ? 0x8fcaff : 0x2466a1;
            control = systemDark ? 0x383b45 : 0xe7ebf2;
            edge = systemDark ? 0x525764 : 0xc2cad5;
            field = systemDark ? 0x32353d : 0xebeef4;
            left = right = tile = tileEdge = canvas;
            break;
    }
    p.surface = swatch(canvas);
    p.foreground = swatch(ink);
    p.accent = accent == CLR_INVALID ? swatch(focus) : accent;
    p.buttonFace = swatch(control);
    p.rim = swatch(edge);
    p.field = swatch(field);
    p.headerLeft = swatch(left);
    p.headerRight = swatch(right);
    p.tile = swatch(tile);
    p.tileEdge = swatch(tileEdge);
    p.dark = colorLuminance(p.surface) < .25;
    COLORREF inkColor = accent != CLR_INVALID
                            ? accent
                            : (selection != CLR_INVALID ? selection : p.accent);
    p.selectionFace = blendColor(p.tile, inkColor, .15);
    p.selectionHot = blendColor(p.buttonFace, inkColor, .12);
    p.selectionMark = inkColor;
    for (int i = 0;
         i < 12 && (contrast(p.selectionMark, p.selectionFace) < 3.0 ||
                    contrast(p.selectionMark, p.selectionHot) < 3.0);
         ++i)
        p.selectionMark = blendColor(p.selectionMark, p.foreground, .20);
    for (int i = 0; i < 8 && (contrast(p.accent, p.surface) < 3.0 ||
                              contrast(p.accent, p.tile) < 3.0);
         ++i)
        p.accent = blendColor(p.accent, p.foreground, .24);
    return p;
}
static void applyPanelStyle(FolderStyle choice, bool force = false) {
    if (!panel.window)
        return;
    auto settings = appearanceSettings.load();
    panel.requestedStyle = choice;
    auto style = choice == FolderStyle::Inherit ? settings.preset : choice;
    if (!force && panel.appearanceReady && panel.shownStyle == style &&
        panel.appliedSettings == settings)
        return;
    HIGHCONTRASTW hc{sizeof(hc)};
    SystemParametersInfoW(SPI_GETHIGHCONTRAST, sizeof(hc), &hc, 0);
    bool highContrast = (hc.dwFlags & HCF_HIGHCONTRASTON) != 0;
    COLORREF selection = !highContrast && settings.accent == CLR_INVALID
                             ? windowsAccent()
                             : settings.accent;
    auto next = paletteFor(style, panel.systemDark, settings.accent,
                           highContrast, selection);
    HBRUSH background = CreateSolidBrush(next.surface),
           search = CreateSolidBrush(next.field);
    if (!background || !search) {
        if (background)
            DeleteObject(background);
        if (search)
            DeleteObject(search);
        return;
    }
    auto oldBackground = std::exchange(panel.background, background);
    auto oldSearch = std::exchange(panel.searchBrush, search);
    static_cast<FolderPalette&>(panel) = next;
    panel.shownStyle = style;
    panel.appearanceReady = true;
    panel.appliedSettings = settings;
    if (oldBackground)
        DeleteObject(oldBackground);
    if (oldSearch)
        DeleteObject(oldSearch);
    BOOL dark = next.dark;
    DwmSetWindowAttribute(panel.window, DWMWA_USE_IMMERSIVE_DARK_MODE, &dark,
                          sizeof(dark));
    DWORD corners = style == FolderStyle::Oled || style == FolderStyle::Paper
                        ? DWMWCP_ROUNDSMALL
                        : DWMWCP_ROUND;
    DwmSetWindowAttribute(panel.window, DWMWA_WINDOW_CORNER_PREFERENCE,
                          &corners, sizeof(corners));
    COLORREF border =
        next.finish == TileFinish::Plain ? DWMWA_COLOR_DEFAULT : next.rim;
    DwmSetWindowAttribute(panel.window, DWMWA_BORDER_COLOR, &border,
                          sizeof(border));
    if (panel.list) {
        ListView_SetBkColor(panel.list, next.surface);
        ListView_SetTextColor(panel.list, next.foreground);
        releasePreviousPage();
        RedrawWindow(panel.window, nullptr, nullptr,
                     RDW_INVALIDATE | RDW_ALLCHILDREN);
    }
}
static void restorePanelStyle(bool force = false) {
    if (auto f = lookup(groups, panel.folder))
        applyPanelStyle(f->style, force);
}
static void fillColor(HDC dc, const RECT& r, COLORREF color) {
    auto old = SetDCBrushColor(dc, color);
    FillRect(dc, &r, (HBRUSH)GetStockObject(DC_BRUSH));
    SetDCBrushColor(dc, old);
}
static void paintBackdrop(HDC dc, HWND child, const RECT& bounds) {
    FillRect(dc, &bounds, panel.background);
    if (panel.finish == TileFinish::Plain || !panel.window)
        return;
    POINT start{};
    if (child != panel.window)
        MapWindowPoints(panel.window, child, &start, 1);
    RECT client{};
    GetClientRect(panel.window, &client);
    RECT header{start.x, start.y, start.x + client.right, start.y + px(41)},
        clip{};
    if (!IntersectRect(&clip, &header, &bounds))
        return;
    int saved = SaveDC(dc);
    IntersectClipRect(dc, clip.left, clip.top, clip.right, clip.bottom);
    auto vertex = [](LONG x, LONG y, COLORREF c) {
        return TRIVERTEX{x,
                         y,
                         (COLOR16)(GetRValue(c) * 257),
                         (COLOR16)(GetGValue(c) * 257),
                         (COLOR16)(GetBValue(c) * 257),
                         0};
    };
    if (panel.headerLeft == panel.headerRight)
        fillColor(dc, header, panel.headerLeft);
    else {
        TRIVERTEX stops[] = {
            vertex(header.left, header.top, panel.headerLeft),
            vertex(header.right, header.bottom, panel.headerRight)};
        GRADIENT_RECT pair{0, 1};
        GradientFill(dc, stops, 2, &pair, 1, GRADIENT_FILL_RECT_H);
    }
    RECT seam{header.left + px(12), header.bottom - px(1),
              header.right - px(12), header.bottom};
    fillColor(dc, seam, panel.tileEdge);
    RestoreDC(dc, saved);
}
static void outlineTile(HDC dc,
                        RECT r,
                        COLORREF edge,
                        COLORREF fill,
                        int radius) {
    rounded(dc, r, edge, radius);
    InflateRect(&r, -px(1), -px(1));
    if (r.right > r.left && r.bottom > r.top)
        rounded(dc, r, fill, std::max(1, radius - px(1)));
}
static constexpr UINT firstStyleCommand = 300;
static std::optional<FolderStyle> styleCommand(UINT id) {
    if (id < firstStyleCommand ||
        id - firstStyleCommand >= std::size(styleLabels))
        return {};
    return static_cast<FolderStyle>(id - firstStyleCommand);
}
static HBITMAP menuSwatch(const FolderPalette& p, int size) {
    BITMAPINFO info{};
    info.bmiHeader = {sizeof(BITMAPINFOHEADER), size, -size, 1, 32, BI_RGB};
    void* memory = nullptr;
    auto bmp =
        CreateDIBSection(nullptr, &info, DIB_RGB_COLORS, &memory, nullptr, 0);
    if (!bmp)
        return nullptr;
    auto pixels = static_cast<DWORD*>(memory);
    for (int y = 0; y < size; ++y)
        for (int x = 0; x < size; ++x) {
            double u = (x + .5) / size, v = (y + .5) / size;
            double dx = std::max({.20 - u, u - .80, 0.}),
                   dy = std::max({.20 - v, v - .80, 0.});
            bool in = dx * dx + dy * dy <= .028;
            auto c = (u > .63 && v > .25 && v < .75) ? p.accent
                     : v < .36                       ? p.headerLeft
                                                     : p.tile;
            if (in && (u < .11 || v < .11 || u > .89 || v > .89))
                c = p.rim;
            pixels[y * size + x] = in ? 0xff000000u | (GetRValue(c) << 16) |
                                            (GetGValue(c) << 8) | GetBValue(c)
                                      : 0;
        }
    return bmp;
}
static void previewMenuChoice(HMENU selected, UINT id, UINT flags) {
    if (!panel.designMenu || flags == 0xffff)
        return;
    if (selected == panel.designMenu && !(flags & MF_POPUP)) {
        if (auto style = styleCommand(id)) {
            applyPanelStyle(*style);
            return;
        }
    }
    restorePanelStyle();
}
static void choosePanelStyle(const wstring& dest, FolderStyle selected) {
    const HWND window = panel.window;
    const auto previous = revision;
    if (!edit([&](Model& model) {
            if (auto f = lookup(model, dest))
                f->style = selected;
        }))
        return;
    if (window && panel.window == window && panel.folder == dest &&
        panel.rendered == previous)
        panel.rendered = revision;
}
struct RunIdentity {
    wstring key, id, exe;
    bool allowExplicitPath = false;
};
static wstring itemString(IShellItem2* item, REFPROPERTYKEY property) {
    PWSTR value = nullptr;
    if (!item || FAILED(item->GetString(property, &value)) || !value)
        return {};
    wstring s(value);
    CoTaskMemFree(value);
    return s;
}
// Resolve a shortcut once per folder snapshot, always off the panel thread.
static RunIdentity runningIdentity(const App& app) {
    RunIdentity key{app.target, app.id};
    auto path = fs::path(launchTarget(app));
    auto ext = path.extension().wstring();
    if (equal(ext, L".lnk")) {
        com_ptr<IShellLinkW> link;
        if (SUCCEEDED(CoCreateInstance(CLSID_ShellLink, nullptr,
                                       CLSCTX_INPROC_SERVER,
                                       IID_PPV_ARGS(link.put()))) &&
            SUCCEEDED(link.as<IPersistFile>()->Load(path.c_str(), STGM_READ))) {
            auto props = link.try_as<IPropertyStore>();
            PROPVARIANT v{};
            if (key.id.empty() && props &&
                SUCCEEDED(props->GetValue(kModelId, &v)) && v.vt == VT_LPWSTR &&
                v.pwszVal)
                key.id = v.pwszVal;
            PropVariantClear(&v);
            wchar_t target[32768]{}, args[2048]{};
            if (SUCCEEDED(
                    link->GetPath(target, ARRAYSIZE(target), nullptr, 0)) &&
                SUCCEEDED(link->GetArguments(args, ARRAYSIZE(args))) && !*args)
                key.exe = target;
        }
    } else if (prefix(app.target, L"shell:AppsFolder\\")) {
        if (key.id.empty())
            key.id = app.target.substr(17);
        auto shell = shellItem(app.target);
        auto item =
            shell ? shell.try_as<IShellItem2>() : com_ptr<IShellItem2>{};
        if (item) {
            auto shellId = itemString(item.get(), kModelId);
            if (!shellId.empty())
                key.id = std::move(shellId);
            key.exe = itemString(item.get(), PKEY_Link_TargetParsingPath);
        }
        if (key.exe.empty() && fs::path(app.id).is_absolute())
            key.exe = app.id;
    } else if (equal(ext, L".exe"))
        key.exe = app.target;
    if (!key.exe.empty()) {
        auto executable = fs::path(key.exe);
        if (!executable.is_absolute() ||
            !equal(executable.extension().wstring(), L".exe"))
            key.exe.clear();
        else {
            // A web app can share its browser executable with many unrelated
            // windows.
            auto file = executable.filename().wstring();
            if (StrStrIW(key.id.c_str(), L"_crx_"))
                key.exe.clear();
            key.allowExplicitPath =
                key.id.empty() && !equal(file, L"chrome.exe") &&
                !equal(file, L"msedge.exe") && !equal(file, L"brave.exe") &&
                !equal(file, L"firefox.exe") && !equal(file, L"opera.exe") &&
                !equal(file, L"vivaldi.exe");
        }
    }
    return key;
}
static bool appWindowVisible(HWND w) {
    if (!IsWindowVisible(w) || w == GetShellWindow())
        return false;
    auto ex = GetWindowLongPtrW(w, GWL_EXSTYLE);
    if ((ex & WS_EX_TOOLWINDOW) ||
        ((ex & WS_EX_NOACTIVATE) && !(ex & WS_EX_APPWINDOW)) ||
        (GetWindow(w, GW_OWNER) && !(ex & WS_EX_APPWINDOW)))
        return false;
    wchar_t name[64]{};
    GetClassNameW(w, name, ARRAYSIZE(name));
    if (!wcscmp(name, L"WorkerW") || !wcscmp(name, L"Progman") ||
        !wcscmp(name, L"Shell_TrayWnd") ||
        !wcscmp(name, L"Shell_SecondaryTrayWnd"))
        return false;
    DWORD cloaked = 0;
    DwmGetWindowAttribute(w, DWMWA_CLOAKED, &cloaked, sizeof(cloaked));
    return !cloaked;
}
struct ProcessIdentity {
    wstring id, exe;
};
static ProcessIdentity processIdentity(DWORD pid) {
    ProcessIdentity out;
    HANDLE process = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (!process)
        return out;
    wchar_t exe[32768]{}, id[256]{};
    DWORD count = ARRAYSIZE(exe);
    UINT32 length = ARRAYSIZE(id);
    if (QueryFullProcessImageNameW(process, 0, exe, &count))
        out.exe.assign(exe, count);
    if (GetApplicationUserModelId(process, &length, id) == ERROR_SUCCESS)
        out.id = id;
    CloseHandle(process);
    return out;
}
static wstring explicitWindowId(HWND w) {
    com_ptr<IPropertyStore> props;
    if (FAILED(SHGetPropertyStoreForWindow(w, IID_PPV_ARGS(props.put()))))
        return {};
    PROPVARIANT v{};
    wstring id;
    if (SUCCEEDED(props->GetValue(kModelId, &v)) && v.vt == VT_LPWSTR &&
        v.pwszVal)
        id = v.pwszVal;
    PropVariantClear(&v);
    return id;
}
static WindowMatches matchOpenWindows(const std::vector<RunIdentity>& keys) {
    WindowMatches result;
    if (keys.empty())
        return result;
    struct Enumeration {
        std::vector<HWND> windows;
        bool failed = false;
    } list;
    EnumWindows(
        [](HWND w, LPARAM lp) -> BOOL {
            auto& found = *reinterpret_cast<Enumeration*>(lp);
            try {
                if (appWindowVisible(w))
                    found.windows.push_back(w);
                return TRUE;
            } catch (...) {
                found.failed = true;
                return FALSE;
            }
        },
        reinterpret_cast<LPARAM>(&list));
    if (list.failed)
        return result;
    std::map<wstring, wstring, IgnoreCase> pathOwners;
    std::set<wstring, IgnoreCase> sharedPaths;
    for (const auto& key : keys)
        if (!key.exe.empty() && !key.id.empty()) {
            auto [it, inserted] = pathOwners.try_emplace(key.exe, key.id);
            if (!inserted && !equal(it->second, key.id))
                sharedPaths.insert(key.exe);
        }
    std::map<DWORD, ProcessIdentity> processes;
    auto process = [&](DWORD pid) -> const ProcessIdentity& {
        auto [it, inserted] = processes.try_emplace(pid);
        if (inserted)
            it->second = processIdentity(pid);
        return it->second;
    };
    for (HWND w : list.windows) {
        DWORD pid = 0;
        GetWindowThreadProcessId(w, &pid);
        if (!pid)
            continue;
        const auto& owner = process(pid);
        auto id = explicitWindowId(w);
        if (id.empty())
            id = owner.id;
        if (id.empty())
            if (HWND child = FindWindowExW(
                    w, nullptr, L"Windows.UI.Core.CoreWindow", nullptr)) {
                DWORD childPid = 0;
                GetWindowThreadProcessId(child, &childPid);
                if (childPid)
                    id = process(childPid).id;
            }
        for (const auto& key : keys) {
            bool idMatch = !id.empty() && !key.id.empty() && equal(id, key.id);
            bool pathMatch =
                !key.exe.empty() && equal(key.exe, owner.exe) &&
                (id.empty() ? key.id.empty() || !sharedPaths.contains(key.exe)
                            : key.allowExplicitPath);
            if (idMatch || pathMatch)
                result[key.key].push_back({w, pid});
        }
    }
    return result;
}
struct WindowScanJob {
    HWND notify{};
    uint64_t request{};
    std::shared_ptr<const std::vector<App>> apps;
    std::optional<MenuAction> click{};
};
struct WindowScanner {
    std::mutex lock;
    std::condition_variable changed;
    std::thread thread;
    bool stopping = false;
    std::optional<WindowScanJob> next;
    std::deque<WindowScanJob> clicks;
    std::deque<std::pair<MenuAction, std::vector<AppWindow>>> readyClicks;
    uint64_t ready = 0;
    WindowMatches result;
};
[[clang::no_destroy]] static WindowScanner windowScanner;
static uint64_t windowSequence = 0;
static void runningWorker() {
    auto& worker = windowScanner;
    auto init = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    std::shared_ptr<const std::vector<App>> cached;
    std::vector<RunIdentity> keys;
    for (;;) {
        WindowScanJob job;
        {
            std::unique_lock guard(worker.lock);
            worker.changed.wait(guard, [&] {
                return worker.stopping || worker.next.has_value() ||
                       !worker.clicks.empty();
            });
            if (worker.stopping)
                break;
            if (!worker.clicks.empty()) {
                job = std::move(worker.clicks.front());
                worker.clicks.pop_front();
            } else {
                job = std::move(*worker.next);
                worker.next.reset();
            }
        }
        WindowMatches result;
        try {
            if (cached != job.apps) {
                keys.clear();
                for (const auto& a : *job.apps)
                    keys.push_back(runningIdentity(a));
                cached = job.apps;
            }
            result = matchOpenWindows(keys);
        } catch (...) {
            cached.reset();
        }
        std::lock_guard guard(worker.lock);
        if (worker.stopping)
            break;
        if (job.click) {
            auto windows = result.find(job.click->target);
            worker.readyClicks.emplace_back(std::move(*job.click),
                                            windows == result.end()
                                                ? std::vector<AppWindow>{}
                                                : std::move(windows->second));
            if (!PostMessageW(job.notify, kMenuLaunchReady, 0, 0))
                worker.readyClicks.pop_back();
        } else if (!worker.next) {
            worker.ready = job.request;
            worker.result = std::move(result);
            PostMessageW(job.notify, kRunningReady, WPARAM(job.request), 0);
        }
    }
    if (SUCCEEDED(init))
        CoUninitialize();
}
static void stopRunningWorker() {
    auto& worker = windowScanner;
    {
        std::lock_guard guard(worker.lock);
        worker.stopping = true;
        worker.next.reset();
        worker.clicks.clear();
        worker.readyClicks.clear();
    }
    worker.changed.notify_one();
    if (worker.thread.joinable())
        worker.thread.join();
}
static void requestWindowScan(bool forClick) {
    if (!panel.window || panel.closing || panel.choosing || stopping)
        return;
    if (!forClick && (panel.dragging || panel.pressed >= 0 || panel.transfer ||
                      panel.incoming.token)) {
        panel.runningDirty = true;
        return;
    }
    auto f = lookup(groups, panel.folder);
    if (!f)
        return;
    if (panel.windowFolder != f->id || panel.windowRevision != revision ||
        !panel.windowMembers) {
        panel.windowMembers = std::make_shared<const std::vector<App>>(f->apps);
        panel.windowFolder = f->id;
        panel.windowRevision = revision;
    }
    auto& worker = windowScanner;
    {
        std::lock_guard guard(worker.lock);
        if (worker.stopping)
            return;
        if (!worker.thread.joinable())
            worker.thread = std::thread(runningWorker);
        panel.windowRequest = ++windowSequence;
        panel.runningDirty = false;
        worker.next.emplace(WindowScanJob{panel.window, panel.windowRequest,
                                          panel.windowMembers});
    }
    worker.changed.notify_one();
}
static void scheduleWindowScan() {
    if (!panel.window || panel.closing || panel.choosing)
        return;
    panel.runningDirty = true;
    if (!panel.windowTimer && panel.pressed < 0 && !panel.transfer &&
        !panel.incoming.token)
        panel.windowTimer =
            SetTimer(panel.window, kRunningTimer, 90, nullptr) != 0;
}
static void CALLBACK runningWindowEvent(HWINEVENTHOOK hook,
                                        DWORD event,
                                        HWND window,
                                        LONG object,
                                        LONG child,
                                        DWORD,
                                        DWORD) {
    if (hook != panel.windowHook || !window || window == panel.window ||
        object != OBJID_WINDOW || child != CHILDID_SELF)
        return;
    if (event == EVENT_OBJECT_DESTROY) {
        bool tracked = std::any_of(
            panel.openWindows.begin(), panel.openWindows.end(),
            [&](const auto& entry) {
                return std::any_of(
                    entry.second.begin(), entry.second.end(),
                    [&](const AppWindow& ref) { return ref.window == window; });
            });
        if (!tracked)
            return;
    } else if (GetAncestor(window, GA_ROOT) != window ||
               (GetWindowLongPtrW(window, GWL_EXSTYLE) & WS_EX_TOOLWINDOW))
        return;
    scheduleWindowScan();
}
static bool liveAppWindow(const AppWindow& ref) {
    DWORD pid = 0;
    return GetWindowThreadProcessId(ref.window, &pid) && pid == ref.pid &&
           appWindowVisible(ref.window);
}
static void openFromFolder(const App& app, std::vector<AppWindow> matches) {
    if (GetKeyState(VK_CONTROL) & 0x8000)
        return;
    std::erase_if(matches,
                  [](const AppWindow& ref) { return !liveAppWindow(ref); });
    if (!preferOpenWindow || matches.empty()) {
        dismissPanel();
        if (!launch(app))
            note(uiText(UiString::BrokenShortcut));
        return;
    }
    // EnumWindows supplies Z order. Prefer the foremost visible, unminimized
    // window.
    auto front = std::find_if(
        matches.begin(), matches.end(),
        [](const AppWindow& ref) { return !IsIconic(ref.window); });
    const auto& ref = front == matches.end() ? matches.front() : *front;
    HWND target = GetLastActivePopup(ref.window);
    if (!target || !IsWindowVisible(target))
        target = ref.window;
    if (IsIconic(ref.window))
        ShowWindowAsync(ref.window, SW_RESTORE);
    dismissPanel();
    if (!SetForegroundWindow(target)) {
        FLASHWINFO flash{sizeof(flash), target, FLASHW_TRAY, 2, 0};
        FlashWindowEx(&flash);
    }
}
static void launchMenuMember(const MenuAction& command) {
    const auto* app = menuMember(command);
    if (!app || !controller || (GetKeyState(VK_CONTROL) & 0x8000))
        return;
    if (!preferOpenWindow) {
        auto chosen = *app;
        openFromFolder(chosen, {});
        return;
    }
    const auto* folder = lookup(groups, command.folder);
    auto members = panel.windowMembers && panel.windowFolder == folder->id &&
                           panel.windowRevision == revision
                       ? panel.windowMembers
                       : std::make_shared<const std::vector<App>>(folder->apps);
    auto& worker = windowScanner;
    {
        std::lock_guard guard(worker.lock);
        if (worker.stopping)
            return;
        if (!worker.thread.joinable())
            worker.thread = std::thread(runningWorker);
        worker.clicks.push_back({controller, 0, std::move(members), command});
    }
    worker.changed.notify_one();
}
static void acceptMenuLaunches() {
    decltype(windowScanner.readyClicks) ready;
    {
        std::lock_guard guard(windowScanner.lock);
        ready.swap(windowScanner.readyClicks);
    }
    for (auto& [command, windows] : ready)
        if (const auto* app = menuMember(command)) {
            auto chosen = *app;
            openFromFolder(chosen, std::move(windows));
        }
}
static void acceptWindowScan(uint64_t serial) {
    if (!serial || serial != panel.windowRequest || panel.closing)
        return;
    WindowMatches next;
    {
        std::lock_guard guard(windowScanner.lock);
        if (serial != windowScanner.ready)
            return;
        next.swap(windowScanner.result);
        windowScanner.ready = 0;
    }
    if (panel.windowFolder != panel.folder ||
        panel.windowRevision != revision) {
        scheduleWindowScan();
        return;
    }
    for (auto it = next.begin(); it != next.end();) {
        std::erase_if(it->second, [](const AppWindow& ref) {
            DWORD pid = 0;
            return !GetWindowThreadProcessId(ref.window, &pid) ||
                   ref.pid != pid || !IsWindowVisible(ref.window);
        });
        if (it->second.empty())
            it = next.erase(it);
        else
            ++it;
    }
    if (panel.openWindows != next) {
        panel.openWindows = std::move(next);
        if (!panel.dragging && !panel.transfer)
            InvalidateRect(panel.list, nullptr, FALSE);
    }
    if (panel.pendingClick) {
        auto app = std::move(*panel.pendingClick);
        panel.pendingClick.reset();
        if (panel.clickSerial == panel.viewSerial && !panel.choosing &&
            !panel.dragging && !panel.transfer && panel.pressed < 0 &&
            !panel.incoming.token) {
            auto it = panel.openWindows.find(app.target);
            openFromFolder(app, it == panel.openWindows.end()
                                    ? std::vector<AppWindow>{}
                                    : it->second);
        }
    }
}
static void manage(wstring dest) {
    auto folder = lookup(groups, dest);
    if (!folder || panel.folder != dest || !panel.window)
        return;
    const HWND window = panel.window;
    const auto active = folder->style;
    const auto actions = folderActions(dest, !pinnedFolder(dest));
    struct Swatches {
        std::array<HBITMAP, std::size(styleLabels)> images{};
        ~Swatches() {
            for (auto bmp : images)
                if (bmp)
                    DeleteObject(bmp);
        }
    } swatches;
    Menu menu, styles;
    if (!menu.h || !styles.h)
        return;
    static const auto language = static_cast<size_t>(userLanguage());
    auto settings = appearanceSettings.load();
    for (size_t i = 0; i < std::size(styleLabels); ++i) {
        if (i == 1)
            styles.sep();
        UINT command = firstStyleCommand + static_cast<UINT>(i);
        styles.item(command, styleLabels[i].names[language]);
        if (i && !panel.highContrast) {
            auto palette = paletteFor(static_cast<FolderStyle>(i),
                                      panel.systemDark, settings.accent, false);
            if (auto bmp = menuSwatch(palette, std::clamp(px(16), 12, 48))) {
                swatches.images[i] = bmp;
                MENUITEMINFOW item{sizeof(item)};
                item.fMask = MIIM_BITMAP;
                item.hbmpItem = bmp;
                SetMenuItemInfoW(styles.h, command, FALSE, &item);
            }
        }
    }
    CheckMenuRadioItem(
        styles.h, firstStyleCommand,
        firstStyleCommand + static_cast<UINT>(std::size(styleLabels)) - 1,
        firstStyleCommand + static_cast<UINT>(active), MF_BYCOMMAND);
    HMENU submenu = styles.h;
    bool linked = false;
    for (size_t i = 0; i < actions.size(); ++i) {
        if (actions[i].kind == FolderAction::Remove)
            menu.sep();
        menu.item(static_cast<UINT>(i + 1), actions[i].label);
        if (actions[i].kind == FolderAction::Rename) {
            linked = AppendMenuW(menu.h, MF_POPUP,
                                 reinterpret_cast<UINT_PTR>(submenu),
                                 uiText(UiString::Appearance)) != FALSE;
            if (linked)
                styles.h = nullptr;  // The parent now owns the submenu.
        }
    }
    constexpr UINT findCommand = 0x5230, undoCommand = 0x5231;
    menu.sep();
    menu.item(findCommand,
              wstring(uiText(UiString::FindInFolder)) + L"\tCtrl+F");
    if (canUndoMove())
        menu.item(undoCommand,
                  wstring(uiText(UiString::UndoMove)) + L"\tCtrl+Z");
    panel.designMenu = linked ? submenu : nullptr;
    TPMPARAMS placement{sizeof(placement)};
    GetWindowRect(window, &placement.rcExclude);
    auto cmd = menu.show(window, &placement);
    if (panel.window != window || panel.folder != dest)
        return;
    panel.designMenu = nullptr;
    if (auto selected = styleCommand(cmd))
        choosePanelStyle(dest, *selected);
    if (panel.window != window || panel.folder != dest)
        return;
    restorePanelStyle();
    if (cmd == findCommand)
        findInFolder();
    else if (cmd == undoCommand) {
        if (undoMove())
            refreshPanel();
    } else if (cmd > 0 && cmd <= actions.size())
        runFolderAction(actions[cmd - 1]);
}
static void redrawTile(int n) {
    AppRects r;
    if (appRects(n, r))
        InvalidateRect(panel.list, &r.cell, FALSE);
}
static void highlight(int n) {
    if (panel.hot == n)
        return;
    if (onPage(panel.hot))
        target(panel.tiles[panel.hot - firstItem()].hover, 0, 125);
    redrawTile(panel.hot);
    panel.hot = n;
    if (onPage(n))
        target(panel.tiles[n - firstItem()].hover, 1, 105);
    redrawTile(n);
}
static void clearPanelSelection() {
    if (panel.selection.empty())
        return;
    panel.selection.clear();
    InvalidateRect(panel.list, nullptr, FALSE);
}
static void toggleSelection(int n) {
    const auto& key = panel.apps[n].target;
    if (!panel.selection.erase(key))
        panel.selection.insert(key);
    redrawTile(n);
}
static void cancelSelectionHold() {
    if (panel.holdDue && panel.list)
        KillTimer(panel.list, kSelectHoldTimer);
    panel.holdDue = 0;
}
static void acceptSelectionHold() {
    if (!panel.holdDue || GetTickCount64() < panel.holdDue)
        return;
    cancelSelectionHold();
    if (!onPage(panel.pressed) || panel.dragging ||
        GetCapture() != panel.list || (GetKeyState(VK_CONTROL) & 0x8000))
        return;
    panel.pendingClick.reset();
    panel.selectedOnHold = true;
    toggleSelection(panel.pressed);
}
static void collectDragApps(int pressed) {
    panel.dragIndices.clear();
    panel.dragApps.clear();
    if (!panel.selection.contains(panel.apps[pressed].target)) {
        panel.dragIndices.push_back(pressed);
        panel.dragApps.push_back(panel.apps[pressed]);
        return;
    }
    for (int i = 0; i < (int)panel.apps.size(); ++i)
        if (panel.selection.contains(panel.apps[i].target)) {
            panel.dragIndices.push_back(i);
            panel.dragApps.push_back(panel.apps[i]);
        }
}
static int groupInsertion(const std::vector<int>& selected, int from, int to) {
    const int boundary = to + (to > from);
    return boundary -
           int(std::lower_bound(selected.begin(), selected.end(), boundary) -
               selected.begin());
}
static void arrangeDrag(int dest) {
    auto now = motionTime();
    const int insertion =
        panel.dragIndices.size() > 1 && dest >= 0
            ? groupInsertion(panel.dragIndices, panel.pressed, dest)
            : 0;
    for (int local = 0; local < (int)panel.tiles.size(); ++local) {
        int n = firstItem() + local, slot = n;
        if (panel.dragging && panel.dragIndices.size() > 1) {
            if (dest >= 0 && dest != panel.pressed) {
                const auto& indices = panel.dragIndices;
                auto at = std::lower_bound(indices.begin(), indices.end(), n);
                if (at == indices.end() || *at != n) {
                    int rank = n - int(at - indices.begin());
                    slot = rank + (rank >= insertion ? int(indices.size()) : 0);
                }
            }
        } else if (panel.dragging && !panel.choosing && dest >= 0 &&
                   n != panel.pressed) {
            if (n > panel.pressed && n <= dest)
                slot = n - 1;
            if (n < panel.pressed && n >= dest)
                slot = n + 1;
        }
        int dx = 0, dy = 0;
        if (onPage(slot)) {
            int t = slot - firstItem();
            dx = ((t % panel.columns) - (local % panel.columns)) *
                 panel.cellWidth;
            dy = ((t / panel.columns) - (local / panel.columns)) *
                 panel.cellHeight;
        } else if (slot != n)
            dy = slot < firstItem() ? -panel.cellHeight : panel.cellHeight;
        target(panel.tiles[local].dx, dx, Slide::settleMs, now);
        target(panel.tiles[local].dy, dy, Slide::settleMs, now);
    }
    if (!panel.motion || !panel.frame)
        InvalidateRect(panel.list, nullptr, FALSE);
}
static void panelPosition(double now) {
    RECT r = panel.rest;
    if (r.right <= r.left)
        return;
    int sign = panel.rest.bottom <= panel.anchor.top ? 1 : -1;
    double reveal = panel.reveal.value(now);
    int shift = int(std::lround(sign * px(7) * (1 - reveal)));
    // Keep the surface outside the taskbar even during the first frame.
    SetWindowPos(panel.window, nullptr, r.left, r.top + shift, r.right - r.left,
                 r.bottom - r.top, SWP_NOZORDER | SWP_NOACTIVATE);
    if (panel.layered)
        SetLayeredWindowAttributes(
            panel.window, 0,
            (BYTE)std::lround(255 * std::clamp(reveal, 0.0, 1.0)), LWA_ALPHA);
}
static void layered(bool enabled) {
    if (panel.layered == enabled)
        return;
    auto style = GetWindowLongPtrW(panel.window, GWL_EXSTYLE);
    SetWindowLongPtrW(
        panel.window, GWL_EXSTYLE,
        enabled ? (style | WS_EX_LAYERED) : (style & ~WS_EX_LAYERED));
    panel.layered = enabled;
}
static void revealPanel() {
    panel.closing = false;
    if (!panel.motion) {
        panel.reveal.set(1);
        panelPosition(motionTime());
        return;
    }
    layered(true);
    target(panel.reveal, 1, 165);
    panelPosition(motionTime());
    if (!panel.frame)
        layered(false);
}
static void dismissPanel() {
    if (!panel.window || panel.closing)
        return;
    if (!panel.motion || panel.browsing ||
        panel.reveal.value(motionTime()) < .028) {
        closePanel();
        return;
    }
    panel.closing = true;
    layered(true);
    target(panel.reveal, 0, 110);
    if (!panel.frame)
        closePanel();
}
static void gridEntrance(int dir) {
    panel.arrival.set(panel.motion ? 1 : 0);
    panel.slide = dir;
    target(panel.arrival, 0, 175);
}
static void releasePreviousPage() {
    if (!panel.previousPage)
        return;
    SelectObject(panel.previousPage, panel.previousStock);
    DeleteObject(panel.previousBitmap);
    DeleteDC(panel.previousPage);
    panel.previousPage = nullptr;
    panel.previousBitmap = nullptr;
    panel.previousStock = nullptr;
}
static void tickPanel() {
    auto now = panel.frameSample = motionTime();
    bool active = panel.reveal.active(now);
    if (panel.layered) {
        panelPosition(now);
        if (!active) {
            if (panel.closing) {
                closePanel();
                return;
            }
            layered(false);
        }
    }
    bool full = panel.arrival.damage(now, active);
    if (!panel.arrival.active(now))
        releasePreviousPage();
    for (size_t i = 0; i < panel.tiles.size(); ++i) {
        auto& t = panel.tiles[i];
        bool dx = t.dx.damage(now, active), dy = t.dy.damage(now, active);
        bool hover = t.hover.damage(now, active),
             press = t.press.damage(now, active);
        if (dx || dy || hover || press) {
            AppRects r;
            if (!appRects(firstItem() + (int)i, r))
                continue;
            RECT old = r.cell;
            OffsetRect(&old, t.lastOffset.x, t.lastOffset.y);
            t.lastOffset = {(LONG)std::lround(t.dx.value(now)),
                            (LONG)std::lround(t.dy.value(now))};
            OffsetRect(&r.cell, t.lastOffset.x, t.lastOffset.y);
            bool moved = !EqualRect(&old, &r.cell);
            if (!full && (moved || hover || press)) {
                if (moved)
                    InvalidateRect(panel.list, &old, FALSE);
                InvalidateRect(panel.list, &r.cell, FALSE);
            }
        }
    }
    if (full && panel.list)
        InvalidateRect(panel.list, nullptr, FALSE);
    for (auto& pair : panel.controls) {
        if (pair.second.active(now) || pair.second.duration) {
            InvalidateRect(pair.first, nullptr, FALSE);
            if (!pair.second.active(now))
                pair.second.set(pair.second.end);
            else
                active = true;
        }
    }
    if (!active || !schedulePanelFrame())
        stopPanelFrames();
}
static int appImage(int);
static void changePage(int, bool = false);
static void startDragImage(POINT screen);
static void moveDragImage(POINT screen) {
    if (!panel.dragImage)
        return;
    auto& icon = *panel.dragImage;
    POINT next{screen.x - panel.dragGrip.x, screen.y - panel.dragGrip.y};
    if (next.x == icon.position.x && next.y == icon.position.y)
        return;
    SetWindowPos(icon.window, nullptr, next.x, next.y, 0, 0,
                 SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
    icon.position = next;
    const double now = motionTime(), dt = now - icon.sampledAt;
    if (dt >= 2) {
        double blend = 1 - std::exp(-dt / 28);
        icon.vx += blend * ((next.x - icon.samplePosition.x) / dt - icon.vx);
        icon.vy += blend * ((next.y - icon.samplePosition.y) / dt - icon.vy);
        icon.samplePosition = next;
        icon.sampledAt = now;
    }
}
static void finishDrag(bool returnToSlot = false) {
    dragExportHint(FileSurface::None);
    cancelSelectionHold();
    panel.selectedOnHold = false;
    const int previous = panel.pressed;
    AppRects original{};
    const bool returning = returnToSlot && panel.motion && panel.dragImage &&
                           appRects(previous, original);
    POINT imageOrigin = returning ? panel.dragImage->position : POINT{};
    if (returning)
        ScreenToClient(panel.list, &imageOrigin);
    if (!panel.hoverFolder.empty() && panel.list)
        KillTimer(panel.list, kPeerTimer);
    panel.hoverFolder.clear();
    if (panel.list && panel.autoScroll)
        KillTimer(panel.list, kDragPageTimer);
    panel.autoScroll = false;
    panel.edgeDir = 0;
    panel.contextItem = -1;
    panel.dragImage.reset();
    if (onPage(panel.pressed) &&
        panel.pressed - firstItem() < (int)panel.tiles.size())
        target(panel.tiles[panel.pressed - firstItem()].press, 0, 105);
    bool moved = panel.dragging;
    if (moved && panel.dragIndices.size() > 1)
        InvalidateRect(panel.list, nullptr, FALSE);
    panel.dragIndices.clear();
    panel.dragApps.clear();
    panel.toggleOnRelease = false;
    panel.pressed = -1;
    panel.dragging = false;
    panel.dropTarget = -1;
    if (moved)
        arrangeDrag(-1);
    if (returning) {
        auto& tile = panel.tiles[previous - firstItem()];
        auto now = motionTime();
        tile.dx.from(imageOrigin.x - original.icon.left, 0, 0, now);
        tile.dy.from(imageOrigin.y - original.icon.top, 0, 0, now);
        if (tile.dx.duration || tile.dy.duration) {
            wakeMotion();
            if (!panel.frame) {
                tile.dx.set(0);
                tile.dy.set(0);
            }
        }
        tile.lastOffset = {(LONG)std::lround(tile.dx.value(now)),
                           (LONG)std::lround(tile.dy.value(now))};
        OffsetRect(&original.cell, tile.lastOffset.x, tile.lastOffset.y);
        InvalidateRect(panel.list, &original.cell, FALSE);
    }
    if (panel.list)
        redrawTile(previous);
    if (!panel.transfer && panel.list && GetCapture() == panel.list)
        ReleaseCapture();
    if (panel.runningDirty && previous >= 0)
        scheduleWindowScan();
}
static int edgeDirection(POINT pt) {
    RECT r{};
    GetClientRect(panel.list, &r);
    if (!isIn(r, pt))
        return 0;
    if (panel.page > 0 && pt.x < px(14))
        return -1;
    if (panel.page + 1 < pageCount() && pt.x >= r.right - px(14))
        return 1;
    return 0;
}
static void updateDragEdge(POINT p) {
    int edge = edgeDirection(p);
    if (edge == panel.edgeDir)
        return;
    if (panel.autoScroll)
        KillTimer(panel.list, kDragPageTimer);
    panel.edgeDir = edge;
    panel.autoScroll =
        edge && SetTimer(panel.list, kDragPageTimer, 420, nullptr) != 0;
}
static void dragAt(POINT p) {
    if (panel.pressed < 0 || panel.choosing)
        return;
    POINT screen = p;
    ClientToScreen(panel.list, &screen);
    if (!panel.dragging && pastDragThreshold(panel.start, p, panel.dpi)) {
        cancelSelectionHold();
        if (panel.toggleOnRelease &&
            !panel.selection.contains(panel.apps[panel.pressed].target))
            toggleSelection(panel.pressed);
        collectDragApps(panel.pressed);
        panel.dragging = true;
        panel.dropTarget = panel.pressed;
        if (panel.dragIndices.size() > 1)
            InvalidateRect(panel.list, nullptr, FALSE);
        startDragImage(screen);
        redrawTile(panel.pressed);
    }
    if (!panel.dragging)
        return;
    panel.dragPoint = p;
    updateDragEdge(p);
    int dest = reorderSlot(p);
    if (dest != panel.dropTarget) {
        panel.dropTarget = dest;
        arrangeDrag(dest);
    }
    if (dest < 0 && !isIn(panel.bar, screen))
        updateExportHint(screen);
    else
        dragExportHint(FileSurface::None);
    moveDragImage(screen);
    trackPeer(screen);
}
static void advanceDragPage(POINT screen) {
    KillTimer(panel.list, kDragPageTimer);
    panel.autoScroll = false;
    int direction = std::exchange(panel.edgeDir, 0);
    if (!direction || (!panel.dragging && !panel.incoming.token))
        return;
    POINT pt = screen;
    ScreenToClient(panel.list, &pt);
    if (edgeDirection(pt) != direction)
        return;
    changePage(panel.page + direction, true);
    if (panel.incoming.token)
        positionIncoming(screen);
    else
        dragAt(pt);
}
static void refreshPanel();
static std::vector<App> taskbarChoices(const AppFolder&);
struct TilePose {
    wstring key;
    double x, y, vx, vy;
};
using TilePoses = std::vector<TilePose>;
static TilePoses visiblePoses() {
    TilePoses poses;
    if (!panel.motion || !panel.list)
        return poses;
    auto now = motionTime();
    POINT origin{};
    ClientToScreen(panel.list, &origin);
    poses.reserve(panel.tiles.size());
    for (int i = 0; i < (int)panel.tiles.size(); ++i) {
        int n = firstItem() + i;
        AppRects r;
        if (!appRects(n, r))
            continue;
        auto& t = panel.tiles[i];
        poses.push_back({panel.apps[n].target,
                         origin.x + r.cell.left + t.dx.value(now),
                         origin.y + r.cell.top + t.dy.value(now),
                         t.dx.velocity(now), t.dy.velocity(now)});
    }
    return poses;
}
static void settleTiles(const TilePoses& poses) {
    if (poses.empty())
        return;
    POINT origin{};
    ClientToScreen(panel.list, &origin);
    auto now = motionTime();
    for (int i = 0; i < (int)panel.tiles.size(); ++i) {
        int n = firstItem() + i;
        AppRects r;
        if (!appRects(n, r))
            continue;
        auto p = std::find_if(poses.begin(), poses.end(), [&](const auto& v) {
            return v.key == panel.apps[n].target;
        });
        if (p == poses.end())
            continue;
        auto& t = panel.tiles[i];
        double x = p->x - origin.x - r.cell.left,
               y = p->y - origin.y - r.cell.top;
        t.dx.from(x, p->vx, 0, now);
        t.dy.from(y, p->vy, 0, now);
        t.lastOffset = {(LONG)std::lround(x), (LONG)std::lround(y)};
        if (t.dx.duration || t.dy.duration) {
            wakeMotion();
            if (!panel.frame) {
                t.dx.set(0);
                t.dy.set(0);
                t.lastOffset = {};
            }
        }
    }
}
// Slot changes keep the native list, icon cache, window bounds and accessible
// item positions.
static void updatePageRange(int first, int last) {
    int begin = std::max(firstItem(), first),
        end = std::min(firstItem() + pageItems(), last + 1);
    for (int n = begin; n < end; ++n) {
        auto local = n - firstItem();
        ListView_SetItemText(panel.list, local, 0,
                             const_cast<PWSTR>(panel.apps[n].name.c_str()));
        panel.tiles[local] = {};
        redrawTile(n);
    }
}
static void populatePage();
static void moveApp(int from, int to) {
    if (panel.rendered != revision || !validIndex(from, panel.apps.size()) ||
        !validIndex(to, panel.apps.size())) {
        finishDrag();
        return;
    }
    if (from == to) {
        finishDrag(true);
        return;
    }
    if (panel.dragApps.empty())
        collectDragApps(from);
    if (panel.dragApps.size() > 1 || panel.filtering) {
        auto poses = visiblePoses();
        const int slot = groupInsertion(panel.dragIndices, from, to);
        std::vector<App> order;
        order.reserve(panel.apps.size());
        for (int n = 0; n < (int)panel.apps.size(); ++n)
            if (!std::binary_search(panel.dragIndices.begin(),
                                    panel.dragIndices.end(), n))
                order.push_back(panel.apps[n]);
        order.insert(order.begin() + slot, panel.dragApps.begin(),
                     panel.dragApps.end());
        if (order == panel.apps) {
            finishDrag(true);
            return;
        }
        const auto id = panel.folder;
        const auto lead = panel.apps[from].target;
        if (panel.dragImage && panel.motion && onPage(to)) {
            AppRects rect;
            appRects(to, rect);
            std::erase_if(
                poses, [&](const TilePose& pose) { return pose.key == lead; });
            poses.push_back({lead,
                             double(panel.dragImage->position.x -
                                    (rect.icon.left - rect.cell.left)),
                             double(panel.dragImage->position.y -
                                    (rect.icon.top - rect.cell.top)),
                             0, 0});
        }
        std::set<wstring, IgnoreCase> visible;
        if (panel.filtering)
            for (const auto& item : panel.apps)
                visible.insert(item.target);
        finishDrag();
        if (!editMove([&](Model& m) {
                auto& members = lookup(m, id)->apps;
                if (visible.empty())
                    members = order;
                else {
                    auto next = order.begin();
                    for (auto& a : members)
                        if (visible.contains(a.target))
                            a = *next++;
                }
            }) ||
            !panel.window)
            return;
        panel.apps = std::move(order);
        panel.rendered = revision;
        ++panel.viewSerial;
        int leadIndex =
            int(std::find_if(panel.apps.begin(), panel.apps.end(),
                             [&](const App& a) { return a.target == lead; }) -
                panel.apps.begin());
        if (!onPage(leadIndex)) {
            panel.page = leadIndex / panel.capacity;
            populatePage();
        } else
            updatePageRange(firstItem(), firstItem() + pageItems() - 1);
        settleTiles(poses);
        ListView_SetItemState(panel.list, leadIndex - firstItem(),
                              LVIS_SELECTED | LVIS_FOCUSED,
                              LVIS_SELECTED | LVIS_FOCUSED);
        return;
    }
    auto a = panel.apps[from], b = panel.apps[to];
    wstring id = panel.folder;
    auto poses = visiblePoses();
    if (panel.dragImage && panel.motion && onPage(to)) {
        POINT pointer = panel.dragPoint;
        ClientToScreen(panel.list, &pointer);
        std::erase_if(poses, [&](const auto& v) { return v.key == a.target; });
        AppRects dest;
        appRects(to, dest);
        poses.push_back({a.target,
                         double(pointer.x - panel.dragGrip.x -
                                (dest.icon.left - dest.cell.left)),
                         double(pointer.y - panel.dragGrip.y -
                                (dest.icon.top - dest.cell.top)),
                         0, 0});
    }
    finishDrag();
    if (!editMove([&](Model& m) { reorder(m, id, a, b); }) || !panel.window)
        return;
    auto& apps = panel.apps;
    if (from < to)
        std::rotate(apps.begin() + from, apps.begin() + from + 1,
                    apps.begin() + to + 1);
    else
        std::rotate(apps.begin() + to, apps.begin() + from,
                    apps.begin() + from + 1);
    panel.rendered = revision;
    ++panel.viewSerial;
    if (panel.page != to / panel.capacity) {
        panel.page = to / panel.capacity;
        populatePage();
    } else
        updatePageRange(std::min(from, to), std::max(from, to));
    settleTiles(poses);
    ListView_SetItemState(panel.list, to - firstItem(),
                          LVIS_SELECTED | LVIS_FOCUSED,
                          LVIS_SELECTED | LVIS_FOCUSED);
}
static void drawGrid(HDC);
static void paintGrid(HWND);
static bool pageWheel(UINT, WPARAM);
static LRESULT CALLBACK
panelChild(HWND h, UINT msg, WPARAM wp, LPARAM lp, UINT_PTR sub, DWORD_PTR) {
    try {
        if (msg == WM_NCDESTROY) {
            RemoveWindowSubclass(h, panelChild, sub);
            return DefSubclassProc(h, msg, wp, lp);
        }
        if ((msg == WM_MOUSEWHEEL || msg == WM_MOUSEHWHEEL) &&
            pageWheel(msg, wp))
            return 0;
        if (panel.closing && ((msg >= WM_MOUSEFIRST && msg <= WM_MOUSELAST) ||
                              msg == WM_KEYDOWN))
            return 0;
        if (msg == WM_KEYDOWN && panel.choosing && wp == 'F' &&
            (GetKeyState(VK_CONTROL) & 0x8000)) {
            SetFocus(panel.filter);
            SendMessageW(panel.filter, EM_SETSEL, 0, -1);
            return 0;
        }
        if (h == panel.nameEdit) {
            if (msg == WM_KEYDOWN && (wp == VK_RETURN || wp == VK_ESCAPE)) {
                endRename(wp == VK_RETURN, true);
                return 0;
            }
            if (msg == WM_CHAR &&
                (wp == VK_RETURN || wp == VK_ESCAPE || wp == VK_TAB))
                return 0;
            if (msg == WM_KEYDOWN && wp == VK_TAB) {
                endRename(true, true);
                return 0;
            }
            return DefSubclassProc(h, msg, wp, lp);
        }
        if (!panel.choosing && !panel.renaming && !panel.transfer &&
            panel.pressed < 0 && !panel.incoming.token) {
            if (msg == WM_KEYDOWN && wp == 'F' &&
                (GetKeyState(VK_CONTROL) & 0x8000)) {
                findInFolder();
                return 0;
            }
            if (h != panel.filter && msg == WM_KEYDOWN && wp == 'Z' &&
                (GetKeyState(VK_CONTROL) & 0x8000)) {
                if (undoMove())
                    refreshPanel();
                return 0;
            }
            if (h == panel.list && msg == WM_CHAR && wp == VK_SPACE)
                return 0;
            if (h == panel.list && msg == WM_KEYDOWN && wp == VK_SPACE) {
                int i = selectedApp();
                if (!(lp & (1L << 30)) && validIndex(i, panel.apps.size()))
                    toggleSelection(i);
                return 0;
            }
            if (msg == WM_KEYDOWN && wp == VK_ESCAPE &&
                !panel.selection.empty()) {
                clearPanelSelection();
                return 0;
            }
            if (h == panel.list && msg == WM_CHAR && wp >= 32 && wp != 127 &&
                !(GetKeyState(VK_CONTROL) & 0x8000)) {
                findInFolder();
                if (panel.filtering)
                    SendMessageW(panel.filter, WM_CHAR, wp, lp);
                return 0;
            }
            if (panel.filtering && msg == WM_KEYDOWN && wp == VK_ESCAPE) {
                endFolderSearch();
                return 0;
            }
        }
        if (msg == WM_KEYDOWN && wp == VK_F2 && !panel.choosing) {
            beginRename();
            return 0;
        }
        if (msg == WM_KEYDOWN && h == panel.filter) {
            if (wp == VK_RETURN && !pageItems())
                return 0;
            if (wp == VK_ESCAPE && GetWindowTextLengthW(h)) {
                SetWindowTextW(h, L"");
                return 0;
            }
            if ((wp == VK_DOWN || wp == VK_RETURN) && pageItems()) {
                if (wp == VK_RETURN)
                    PostMessageW(panel.window, kOpenApp, firstItem(),
                                 panel.viewSerial);
                else {
                    SetFocus(panel.list);
                    ListView_SetItemState(panel.list, 0,
                                          LVIS_SELECTED | LVIS_FOCUSED,
                                          LVIS_SELECTED | LVIS_FOCUSED);
                }
                return 0;
            }
        }
        if (msg == WM_KEYDOWN && wp == VK_ESCAPE) {
            if (panel.transfer) {
                endPeer({});
                return 0;
            }
            if (panel.pressed >= 0)
                finishDrag(true);
            else if (!panel.selection.empty())
                clearPanelSelection();
            else
                PostMessageW(panel.window, kClosePanel, 0, 0);
            return 0;
        }
        if (msg == WM_KEYDOWN && wp == VK_TAB) {
            if (HWND next = GetNextDlgTabItem(
                    panel.window, h, (GetKeyState(VK_SHIFT) & 0x8000) != 0))
                SetFocus(next);
            return 0;
        }
        if (msg == WM_KEYDOWN && (wp == VK_PRIOR || wp == VK_NEXT)) {
            changePage(panel.page + (wp == VK_PRIOR ? -1 : 1));
            return 0;
        }
        if (h == panel.filter)
            return DefSubclassProc(h, msg, wp, lp);
        if (h != panel.list) {
            if (msg == WM_MOUSEMOVE && panel.hover != h) {
                panel.hover = h;
                TRACKMOUSEEVENT track{sizeof(track), TME_LEAVE, h, 0};
                TrackMouseEvent(&track);
                target(panel.controls[h], 1, 100);
                InvalidateRect(h, nullptr, FALSE);
            } else if (msg == WM_MOUSELEAVE) {
                if (panel.hover == h)
                    panel.hover = nullptr;
                target(panel.controls[h], 0, 125);
                InvalidateRect(h, nullptr, FALSE);
            }
            return DefSubclassProc(h, msg, wp, lp);
        }
        POINT p{GET_X_LPARAM(lp), GET_Y_LPARAM(lp)};
        if (panel.transfer) {
            if (msg == WM_CAPTURECHANGED || msg == WM_CANCELMODE) {
                endPeer({});
                return 0;
            }
            if (msg == WM_MOUSEMOVE || msg == WM_LBUTTONUP) {
                POINT screen = p;
                ClientToScreen(h, &screen);
                if (GetCapture() != h ||
                    (msg == WM_MOUSEMOVE && !(wp & MK_LBUTTON)))
                    endPeer({});
                else if (msg == WM_LBUTTONUP)
                    endPeer(screen);
                else {
                    moveDragImage(screen);
                    positionIncoming(screen);
                    trackPeer(screen);
                    if (!isIn(panel.bar, screen))
                        updateExportHint(screen);
                    else
                        dragExportHint(FileSurface::None);
                }
                return 0;
            }
        }
        if (msg == WM_TIMER && wp == kSelectHoldTimer) {
            acceptSelectionHold();
            return 0;
        }
        if (msg == WM_TIMER && wp == kPeerTimer) {
            KillTimer(h, kPeerTimer);
            auto dest = panel.hoverFolder;
            POINT screen{};
            GetCursorPos(&screen);
            if (!dest.empty() && GetCapture() == h && primaryMouseDown() &&
                isIn(panel.bar, screen) && dropFolderAt(screen) == dest)
                enterPeer(dest, screen);
            return 0;
        }
        if (msg == WM_PAINT) {
            paintGrid(h);
            return 0;
        }
        if (msg == WM_PRINTCLIENT && wp) {
            drawGrid((HDC)wp);
            return 0;
        }
        if (msg == WM_ERASEBKGND)
            return 1;
        if (msg == WM_MOUSELEAVE) {
            highlight(-1);
            return 0;
        }
        if (msg == WM_LBUTTONDOWN || msg == WM_LBUTTONDBLCLK) {
            panel.pendingClick.reset();
            if (panel.incoming.token)
                return 0;
            finishDrag();
            if (wp & MK_CONTROL)
                return 0;
            int n = hitApp(p);
            if (n < 0) {
                clearPanelSelection();
                return 0;
            }
            panel.toggleOnRelease = !panel.choosing && !panel.selection.empty();
            panel.pressed = n;
            panel.start = p;
            highlight(n);
            target(panel.tiles[n - firstItem()].press, 1, 75);
            SetFocus(h);
            ListView_SetItemState(h, n - firstItem(),
                                  LVIS_SELECTED | LVIS_FOCUSED,
                                  LVIS_SELECTED | LVIS_FOCUSED);
            SetCapture(h);
            if (!panel.choosing && !panel.toggleOnRelease &&
                GetCapture() == h) {
                const UINT delay =
                    std::clamp<UINT>(GetDoubleClickTime(), 350, 650);
                if (SetTimer(h, kSelectHoldTimer, delay, nullptr))
                    panel.holdDue = GetTickCount64() + delay;
            }
            return 0;
        }
        if (msg == WM_MOUSEMOVE) {
            if (panel.pressed >= 0 && (!(wp & MK_LBUTTON) || GetCapture() != h))
                finishDrag(true);
            if (panel.pressed >= 0)
                dragAt(p);
            else {
                if (panel.hot < 0) {
                    TRACKMOUSEEVENT track{sizeof(track), TME_LEAVE, h, 0};
                    TrackMouseEvent(&track);
                }
                highlight(hitApp(p));
            }
            return 0;
        }
        if (msg == WM_LBUTTONUP) {
            if (panel.pressed < 0 || GetCapture() != h) {
                finishDrag(true);
                return 0;
            }
            if (panel.dragging) {
                POINT screen = p;
                ClientToScreen(h, &screen);
                moveDragImage(screen);
            }
            int from = panel.pressed,
                to = panel.dragging ? reorderSlot(p) : hitApp(p);
            bool moved =
                panel.dragging || pastDragThreshold(panel.start, p, panel.dpi);
            if (moved && !panel.choosing) {
                panel.dragPoint = p;
                if (validIndex(to, panel.apps.size()))
                    moveApp(from, to);
                else if (ClientToScreen(h, &p))
                    moveAppOutside(from, p);
                else
                    finishDrag();
            } else {
                if (!moved && !(wp & MK_CONTROL))
                    acceptSelectionHold();
                const bool held = panel.selectedOnHold,
                           toggle = panel.toggleOnRelease;
                finishDrag();
                if (held || (wp & MK_CONTROL) ||
                    (GetKeyState(VK_CONTROL) & 0x8000))
                    return 0;
                if (!moved && from == to && toggle) {
                    toggleSelection(from);
                    return 0;
                }
                if (!moved && from == to)
                    PostMessageW(panel.window, kOpenApp, from,
                                 panel.viewSerial);
            }
            return 0;
        }
        if (msg == WM_RBUTTONDOWN || msg == WM_RBUTTONDBLCLK) {
            finishDrag();
            ++panel.viewSerial;
            if (!panel.choosing && (panel.contextItem = hitApp(p)) >= 0)
                SetCapture(h);
            return 0;
        }
        if (msg == WM_RBUTTONUP) {
            int item = std::exchange(panel.contextItem, -1);
            if (GetCapture() != h || panel.pressed >= 0)
                return 0;
            ReleaseCapture();
            if (item >= 0 && item == hitApp(p) && ClientToScreen(h, &p))
                SendMessageW(panel.window, WM_CONTEXTMENU, (WPARAM)h,
                             MAKELPARAM(p.x, p.y));
            return 0;
        }
        if (msg == WM_CAPTURECHANGED || msg == WM_CANCELMODE) {
            if (panel.pressed >= 0 || panel.contextItem >= 0)
                finishDrag(true);
        }
        if (msg == WM_TIMER && wp == kDragPageTimer) {
            POINT screen{};
            if (GetCursorPos(&screen))
                advanceDragPage(screen);
            return 0;
        }
        if (msg == WM_KEYDOWN && wp == VK_RETURN) {
            int i = selectedApp();
            if (i >= 0)
                PostMessageW(panel.window, kOpenApp, i, panel.viewSerial);
            return 0;
        }
        if (msg == WM_KEYDOWN && pageItems() &&
            (wp == VK_HOME || wp == VK_END || wp == VK_UP || wp == VK_DOWN ||
             wp == VK_LEFT || wp == VK_RIGHT)) {
            int i = selectedApp(), step = wp == VK_LEFT    ? -1
                                          : wp == VK_RIGHT ? 1
                                          : wp == VK_UP    ? -panel.columns
                                                           : panel.columns;
            int dest =
                wp == VK_HOME  ? 0
                : wp == VK_END ? (int)panel.apps.size() - 1
                : i < 0 ? firstItem()
                        : std::clamp(i + step, 0, (int)panel.apps.size() - 1);
            if (!panel.choosing && (GetKeyState(VK_CONTROL) & 0x8000) &&
                (wp == VK_LEFT || wp == VK_RIGHT)) {
                moveApp(i, dest);
                return 0;
            }
            changePage(dest / panel.capacity);
            ListView_SetItemState(h, dest - firstItem(),
                                  LVIS_SELECTED | LVIS_FOCUSED,
                                  LVIS_SELECTED | LVIS_FOCUSED);
            InvalidateRect(h, nullptr, FALSE);
            return 0;
        }
    } catch (...) {
        finishDrag();
    }
    return DefSubclassProc(h, msg, wp, lp);
}
static HICON shellIcon(const App& a) {
    PIDLIST_ABSOLUTE id = nullptr;
    SHFILEINFOW info{};
    if (SUCCEEDED(SHParseDisplayName(launchTarget(a).c_str(), nullptr, &id, 0,
                                     nullptr))) {
        SHGetFileInfoW(reinterpret_cast<PCWSTR>(id), 0, &info, sizeof(info),
                       SHGFI_PIDL | SHGFI_ICON | SHGFI_LARGEICON);
        CoTaskMemFree(id);
    }
    if (!info.hIcon)
        info.hIcon = CopyIcon(LoadIconW(nullptr, IDI_APPLICATION));
    return info.hIcon;
}
static HICON panelIcon(const App& app) {
    // Request the device-pixel size once. Keep the original alpha icon for the
    // small hover zoom.
    int size = std::clamp(panel.iconSize + px(2), 16, 160);
    if (app.target.find(L'!') == wstring::npos) {
        if (auto h = desktopIcon(app, size, &panel.pinnedIcons))
            return h;
    }
    auto item = shellItem(launchTarget(app));
    auto factory = item ? item.try_as<IShellItemImageFactory>() : nullptr;
    HBITMAP color = nullptr;
    HICON result = nullptr;
    if (factory &&
        SUCCEEDED(factory->GetImage(
            {size, size}, SIIGBF_ICONONLY | SIIGBF_SCALEUP, &color)) &&
        color) {
        BITMAP bm{};
        if (GetObjectW(color, sizeof(bm), &bm) && bm.bmWidth > 0 &&
            bm.bmHeight > 0 && bm.bmWidth <= 512 && bm.bmHeight <= 512) {
            std::vector<BYTE> bits(((bm.bmWidth + 15) / 16) * 2 * bm.bmHeight,
                                   0);
            if (HBITMAP mask =
                    CreateBitmap(bm.bmWidth, bm.bmHeight, 1, 1, bits.data())) {
                ICONINFO info{TRUE, 0, 0, mask, color};
                result = CreateIconIndirect(&info);
                DeleteObject(mask);
            }
        }
        DeleteObject(color);
    }
    return result ? result : shellIcon(app);
}
static int appImage(int index) {
    if (!panel.icons || !validIndex(index, panel.apps.size()))
        return -1;
    const auto& app = panel.apps[index];
    auto found = panel.imageSlots.find(app.target);
    if (found != panel.imageSlots.end()) {
        auto& entry = panel.glyphs[found->second];
        entry.use = ++panel.iconClock;
        return found->second;
    }
    if (panel.failedIcons.contains(app.target))
        return -1;
    HICON icon = panelIcon(app);
    if (!icon) {
        if (panel.failedIcons.size() >= kIconCacheLimit)
            panel.failedIcons.clear();
        panel.failedIcons.insert(app.target);
        return -1;
    }
    bool full = panel.glyphs.size() >= kIconCacheLimit;
    int victim = -1;
    if (full) {
        auto oldest = std::min_element(
            panel.glyphs.begin(), panel.glyphs.end(),
            [](const auto& a, const auto& b) { return a.use < b.use; });
        victim = (int)(oldest - panel.glyphs.begin());
    }
    int slot = ImageList_ReplaceIcon(panel.icons, victim, icon);
    if (slot < 0) {
        DestroyIcon(icon);
        return -1;
    }
    if ((size_t)slot >= panel.glyphs.size())
        panel.glyphs.resize(slot + 1);
    auto& entry = panel.glyphs[slot];
    if (entry.handle) {
        panel.imageSlots.erase(entry.target);
        DestroyIcon(entry.handle);
    }
    entry = {icon, app.target, ++panel.iconClock};
    panel.imageSlots.emplace(entry.target, slot);
    return slot;
}
static void drawGrid(HDC dc) {
    RECT client{};
    GetClientRect(panel.list, &client);
    FillRect(dc, &client, panel.background);
    auto now = panel.frame ? panel.frameSample : motionTime();
    double enter = panel.arrival.value(now);
    bool keyboard =
        GetFocus() == panel.list &&
        !(SendMessageW(panel.list, WM_QUERYUISTATE, 0, 0) & UISF_HIDEFOCUS);
    int focused = keyboard ? selectedApp() : -1;
    RECT clip{};
    int clipType = GetClipBox(dc, &clip);
    int saved = SaveDC(dc);
    SetBkMode(dc, TRANSPARENT);
    SelectObject(dc, panel.normalFont);
    for (int i = 0; i < (int)panel.tiles.size(); ++i) {
        int index = firstItem() + i;
        AppRects r;
        if (!appRects(index, r))
            continue;
        auto& t = panel.tiles[i];
        int dx =
            int(std::lround(t.dx.value(now) + enter * panel.slide * px(15)));
        int dy = int(
            std::lround(t.dy.value(now) + (panel.slide ? 0 : enter * px(6))));
        OffsetRect(&r.cell, dx, dy);
        OffsetRect(&r.label, dx, dy);
        OffsetRect(&r.icon, dx, dy);
        RECT visible{};
        if (!IntersectRect(&visible, &clip, &r.cell))
            continue;
        if (clipType == COMPLEXREGION && !RectVisible(dc, &visible))
            continue;
        RECT face = r.cell;
        InflateRect(&face, -px(2), -px(2));
        double hot = t.hover.value(now), press = t.press.value(now);
        bool lifting = panel.dragImage &&
                       (index == panel.pressed ||
                        (panel.dragIndices.size() > 1 &&
                         std::binary_search(panel.dragIndices.begin(),
                                            panel.dragIndices.end(), index)));
        if (lifting)
            continue;
        bool incoming =
            panel.incoming.token && index >= panel.incoming.slot &&
            index - panel.incoming.slot < (int)panel.incoming.apps.size();
        bool selected = !panel.choosing &&
                        panel.selection.contains(panel.apps[index].target);
        bool focusedTile = keyboard && focused == index;
        double shade = std::max({hot * .78, press, focusedTile ? .38 : 0.});
        bool highlighted = panel.highContrast &&
                           (incoming || selected || focusedTile || press > .4);
        if (highlighted) {
            rounded(dc, face, GetSysColor(COLOR_HIGHLIGHT), px(4));
        } else if (panel.finish != TileFinish::Plain) {
            auto base =
                selected ? panel.selectionFace
                         : (incoming ? blendColor(panel.tile, panel.accent, .22)
                                     : panel.tile);
            auto fill = blendColor(
                base, selected ? panel.selectionHot : panel.buttonFace, shade);
            auto edge = selected ? panel.selectionMark
                                 : (incoming || focusedTile
                                        ? panel.accent
                                        : blendColor(panel.tileEdge,
                                                     panel.accent, hot * .38));
            outlineTile(dc, face, edge, fill, px(panel.radius));
            if (panel.finish == TileFinish::Prism) {
                RECT sheen{face.left + px(10), face.top + px(1),
                           face.right - px(10), face.top + px(2)};
                fillColor(dc, sheen, blendColor(fill, panel.foreground, .20));
            }
        } else {
            if (selected)
                rounded(
                    dc, face,
                    blendColor(panel.selectionFace, panel.selectionHot, shade),
                    px(8));
            else if (shade > .005)
                rounded(dc, face,
                        blendColor(panel.surface, panel.buttonFace, shade),
                        px(8));
            else if (incoming)
                rounded(dc, face, blendColor(panel.surface, panel.accent, .22),
                        px(8));
        }
        if (selected) {
            RECT mark{face.left + px(4), face.top + px(5), face.left + px(7),
                      face.bottom - px(5)};
            rounded(dc, mark, panel.selectionMark, px(2));
        }
        if (focusedTile) {
            RECT ring = face;
            InflateRect(&ring, -px(2), -px(2));
            DrawFocusRect(dc, &ring);
        }
        int size = std::max(1, (int)std::lround(panel.iconSize *
                                                (1 + .055 * hot - .125 * press -
                                                 .045 * enter)));
        int image = appImage(index);
        if (image >= 0 && image < (int)panel.glyphs.size() &&
            panel.glyphs[image].handle)
            DrawIconEx(dc, (r.icon.left + r.icon.right - size) / 2,
                       (r.icon.top + r.icon.bottom - size) / 2,
                       panel.glyphs[image].handle, size, size, 0, nullptr,
                       DI_NORMAL);
        if (!panel.choosing && !panel.openWindows.empty() &&
            panel.openWindows.contains(panel.apps[index].target)) {
            int x = (r.icon.left + r.icon.right) / 2, half = std::max(2, px(4));
            RECT dot{x - half, r.icon.bottom + px(1), x + half,
                     r.icon.bottom + px(4)};
            rounded(dc, dot,
                    panel.highContrast
                        ? GetSysColor(highlighted ? COLOR_HIGHLIGHTTEXT
                                                  : COLOR_WINDOWTEXT)
                        : panel.accent,
                    px(2));
        }
        SetTextColor(dc, highlighted ? GetSysColor(COLOR_HIGHLIGHTTEXT)
                                     : panel.foreground);
        const auto& label = panel.apps[index].name;
        int length = (int)label.size();
        if (length > 4 && !_wcsicmp(label.c_str() + length - 4, L".exe"))
            length -= 4;
        DrawTextW(dc, label.c_str(), length, &r.label,
                  DT_CENTER | DT_WORDBREAK | DT_EDITCONTROL | DT_END_ELLIPSIS |
                      DT_NOPREFIX);
    }
    RestoreDC(dc, saved);
    if (panel.previousPage && enter > 0.001 && panel.slide) {
        int x = int(std::lround(-panel.slide * px(18) * (1 - enter)));
        BLENDFUNCTION alpha{AC_SRC_OVER, 0, (BYTE)std::lround(255 * enter), 0};
        AlphaBlend(dc, x, 0, panel.previousWidth, panel.previousHeight,
                   panel.previousPage, 0, 0, panel.previousWidth,
                   panel.previousHeight, alpha);
    }
}
static HBITMAP iconBitmap32(HDC dc, HICON icon, int side, DWORD*& pixelsOut) {
    if (!icon || side <= 0)
        return nullptr;
    BITMAPINFO info{};
    info.bmiHeader = {sizeof(BITMAPINFOHEADER), side, -side, 1, 32, BI_RGB};
    void* pixels = nullptr;
    HBITMAP bmp =
        CreateDIBSection(dc, &info, DIB_RGB_COLORS, &pixels, nullptr, 0);
    if (!bmp)
        return nullptr;
    auto stock = SelectObject(dc, bmp);
    auto bits = static_cast<DWORD*>(pixels);
    std::fill_n(bits, side * side, 0u);
    bool ok =
        DrawIconEx(dc, 0, 0, icon, side, side, 0, nullptr, DI_NORMAL) != FALSE;
    GdiFlush();
    if (ok && std::none_of(bits, bits + side * side,
                           [](DWORD p) { return p >> 24; })) {
        // Classic mask-based icons have no alpha channel. Recover their
        // silhouette on white.
        std::unique_ptr<DWORD[]> black(new (std::nothrow) DWORD[side * side]);
        ok = bool(black);
        if (ok) {
            std::copy_n(bits, side * side, black.get());
            std::fill_n(bits, side * side, 0x00ffffffu);
            ok = DrawIconEx(dc, 0, 0, icon, side, side, 0, nullptr,
                            DI_NORMAL) != FALSE;
            GdiFlush();
            for (int i = 0; i < side * side; ++i) {
                DWORD alpha =
                    255 - std::clamp<int>(
                              int(bits[i] & 255) - int(black[i] & 255), 0, 255);
                bits[i] = (black[i] & 0x00ffffffu) | (alpha << 24);
            }
        }
    }
    SelectObject(dc, stock);
    if (!ok) {
        DeleteObject(bmp);
        return nullptr;
    }
    pixelsOut = bits;
    return bmp;
}
static bool presentIcon(DragIcon& icon, POINT position, BYTE opacity) {
    POINT origin{};
    SIZE size{icon.side, icon.side};
    BLENDFUNCTION blend{AC_SRC_OVER, 0, opacity, AC_SRC_ALPHA};
    if (!UpdateLayeredWindow(icon.window, nullptr, &position, &size, icon.dc,
                             &origin, 0, &blend, ULW_ALPHA))
        return false;
    icon.position = position;
    icon.opacity = opacity;
    return true;
}
// Stamp the count into the existing premultiplied surface once when picking up
// the group.
static void stampDragCount(DragIcon& icon, size_t count) {
    if (count < 2)
        return;
    const auto label = count > 99 ? wstring(L"99+") : std::to_wstring(count);
    int height = std::min(icon.side, px(16)),
        width = std::min(icon.side, height + (int(label.size()) - 1) * px(6));
    int left = icon.side - width, top = icon.side - height;
    double radius = height * .5;
    auto color =
        panel.highContrast ? GetSysColor(COLOR_HIGHLIGHT) : panel.accent;
    auto ink =
        panel.highContrast ? GetSysColor(COLOR_HIGHLIGHTTEXT)
        : contrast(color, RGB(0, 0, 0)) > contrast(color, RGB(255, 255, 255))
            ? RGB(0, 0, 0)
            : RGB(255, 255, 255);
    auto coverage = [&](int x, int y) {
        double cx = std::clamp(x + .5, left + radius, icon.side - radius);
        return std::clamp(
            radius + .5 - std::hypot(x + .5 - cx, y + .5 - (top + radius)), 0.,
            1.);
    };
    for (int y = top; y < icon.side; ++y)
        for (int x = left; x < icon.side; ++x) {
            double a = coverage(x, y);
            DWORD& p = icon.pixels[y * icon.side + x];
            auto mix = [&](BYTE channel, int shift) {
                return DWORD(std::lround(channel * a +
                                         ((p >> shift) & 255) * (1 - a)))
                       << shift;
            };
            p = mix(255, 24) | mix(GetRValue(color), 16) |
                mix(GetGValue(color), 8) | mix(GetBValue(color), 0);
        }
    HFONT font =
        CreateFontW(-px(11), 0, 0, 0, FW_BOLD, 0, 0, 0, DEFAULT_CHARSET, 0, 0,
                    ANTIALIASED_QUALITY, 0, L"Segoe UI");
    if (!font)
        return;
    auto old = SelectObject(icon.dc, font);
    SetBkMode(icon.dc, TRANSPARENT);
    SetTextColor(icon.dc, ink);
    RECT rect{left, top, icon.side, icon.side};
    DrawTextW(icon.dc, label.c_str(), -1, &rect,
              DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
    GdiFlush();
    SelectObject(icon.dc, old);
    DeleteObject(font);
    for (int y = top; y < icon.side; ++y)
        for (int x = left; x < icon.side; ++x)
            if (coverage(x, y) >= 1.)
                icon.pixels[y * icon.side + x] |= 0xff000000u;
}
static std::unique_ptr<DragIcon> makeDragIcon(int app, POINT position) {
    const int image = appImage(app);
    if (image < 0)
        return {};
    std::unique_ptr<DragIcon> icon(new (std::nothrow) DragIcon);
    if (!icon)
        return {};
    icon->side = panel.iconSize;
    icon->dc = CreateCompatibleDC(nullptr);
    if (!icon->dc)
        return {};
    icon->bitmap = iconBitmap32(icon->dc, panel.glyphs[image].handle,
                                icon->side, icon->pixels);
    if (!icon->bitmap)
        return {};
    icon->stock = SelectObject(icon->dc, icon->bitmap);
    stampDragCount(*icon, panel.dragApps.size());
    icon->window = CreateWindowExW(
        WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW |
            WS_EX_NOACTIVATE | WS_EX_TOPMOST,
        L"STATIC", L"", WS_POPUP | WS_DISABLED, 0, 0, icon->side, icon->side,
        panel.window, nullptr, moduleInstance(), nullptr);
    if (!icon->window || !presentIcon(*icon, position, DragIcon::dragOpacity))
        return {};
    icon->samplePosition = position;
    icon->sampledAt = motionTime();
    ShowWindow(icon->window, SW_SHOWNOACTIVATE);
    return icon;
}
static void startDragImage(POINT screen) {
    AppRects r;
    if (!appRects(panel.pressed, r))
        return;
    const auto& tile = panel.tiles[panel.pressed - firstItem()];
    const auto now = panel.frame ? panel.frameSample : motionTime();
    panel.dragGrip = {
        panel.start.x - r.icon.left - (LONG)std::lround(tile.dx.value(now)),
        panel.start.y - r.icon.top - (LONG)std::lround(tile.dy.value(now))};
    panel.dragImage = makeDragIcon(
        panel.pressed,
        {screen.x - panel.dragGrip.x, screen.y - panel.dragGrip.y});
}
// Interpolate premultiplied BGRA channels in pairs, including alpha at soft
// edges.
static DWORD mixIconPixels(DWORD a, DWORD b, unsigned weight) {
    constexpr DWORD mask = 0x00ff00ff;
    auto pair = [weight](DWORD left, DWORD right) {
        return (
            (((left & mask) * (256 - weight) + (right & mask) * weight) >> 8) &
            mask);
    };
    return pair(a, b) | (pair(a >> 8, b >> 8) << 8);
}
// Expand the existing drag surface around its original pixels so release has no
// visual jump.
static bool prepareExitSurface(ExitEffect& effect) {
    auto& icon = *effect.icon;
    const int source = icon.side, pad = (source + 1) / 2,
              side = source + 2 * pad;
    auto original =
        std::unique_ptr<DWORD[]>(new (std::nothrow) DWORD[source * source]);
    if (!original)
        return false;
    std::copy_n(icon.pixels, source * source, original.get());
    BITMAPINFO info{};
    info.bmiHeader = {sizeof(BITMAPINFOHEADER), side, -side, 1, 32, BI_RGB};
    void* bits = nullptr;
    HBITMAP surface =
        CreateDIBSection(icon.dc, &info, DIB_RGB_COLORS, &bits, nullptr, 0);
    if (!surface)
        return false;
    auto previous = SelectObject(icon.dc, surface);
    if (!previous || previous == HGDI_ERROR) {
        DeleteObject(surface);
        return false;
    }
    DeleteObject(icon.bitmap);
    icon.bitmap = surface;
    icon.pixels = static_cast<DWORD*>(bits);
    icon.side = side;
    effect.original = std::move(original);
    effect.padding = pad;
    std::fill_n(icon.pixels, side * side, 0u);
    for (int y = 0; y < source; ++y)
        std::copy_n(effect.original.get() + y * source, source,
                    icon.pixels + (y + pad) * side + pad);
    return true;
}
static double exitEase(double t) {
    t = std::clamp(t, 0., 1.);
    return t * t * (3 - 2 * t);
}
static DWORD pixelOver(DWORD front, DWORD back) {
    unsigned alpha = front >> 24;
    if (!alpha)
        return back;
    if (alpha == 255 || !back)
        return front;
    return front + mixIconPixels(0, back, 256 - (alpha * 256 + 127) / 255);
}
static void drawExit(ExitEffect& effect, double t) {
    auto& icon = *effect.icon;
    const int side = icon.side, source = effect.sourceSide;
    const double center = (side - 1) * .5, dip = source / 36.;
    std::fill_n(icon.pixels, side * side, 0u);
    // A single outlined ripple stays legible on both light and dark
    // backgrounds.
    const double ripple = exitEase(t / .23) * (1 - exitEase((t - .48) / .52));
    if (ripple > 0) {
        const double radius = source * (.28 + .49 * (1 - (1 - t) * (1 - t)));
        const double line = std::max(.9, dip * (1.8 - .55 * t)),
                     rim = line + std::max(.9, 1.1 * dip);
        const double outer = radius + rim + .5,
                     inner = std::max(0., radius - rim - .5);
        const int left = std::max(0, int(std::floor(center - outer))),
                  right = std::min(side - 1, int(std::ceil(center + outer)));
        const DWORD tint = 0xff000000u | (GetRValue(effect.accent) << 16) |
                           (GetGValue(effect.accent) << 8) |
                           GetBValue(effect.accent);
        for (int y = left; y <= right; ++y) {
            const double dy = y - center;
            for (int x = left; x <= right; ++x) {
                double dx = x - center, rr = dx * dx + dy * dy;
                if (rr > outer * outer || rr < inner * inner)
                    continue;
                double dist = std::abs(std::sqrt(rr) - radius);
                unsigned shadow = unsigned(136 * ripple *
                                           std::clamp(rim + .5 - dist, 0., 1.));
                unsigned light = unsigned(231 * ripple *
                                          std::clamp(line + .5 - dist, 0., 1.));
                icon.pixels[y * side + x] =
                    pixelOver(mixIconPixels(0, tint, light),
                              mixIconPixels(0, 0xff17202au, shadow));
            }
        }
    }
    const unsigned opacity =
        unsigned(std::lround(256 * (1 - exitEase((t - .33) / .49))));
    if (!opacity)
        return;
    const double scale = 1 - .82 * exitEase((t - .025) / .80), step = 1 / scale;
    const double low = center - scale * (source + 1) * .5,
                 high = center + scale * (source + 1) * .5;
    const int left = std::max(0, int(std::ceil(low))),
              right = std::min(side - 1, int(std::floor(high)));
    auto pixel = [&](int x, int y) -> DWORD {
        return unsigned(x) < unsigned(source) && unsigned(y) < unsigned(source)
                   ? effect.original[y * source + x]
                   : 0;
    };
    for (int y = left; y <= right; ++y) {
        double sy = (y - center) * step + (source - 1) * .5;
        int iy = int(std::floor(sy));
        unsigned fy = unsigned((sy - iy) * 256);
        for (int x = left; x <= right; ++x) {
            double sx = (x - center) * step + (source - 1) * .5;
            int ix = int(std::floor(sx));
            unsigned fx = unsigned((sx - ix) * 256);
            DWORD top = mixIconPixels(pixel(ix, iy), pixel(ix + 1, iy), fx);
            DWORD bottom =
                mixIconPixels(pixel(ix, iy + 1), pixel(ix + 1, iy + 1), fx);
            DWORD front =
                mixIconPixels(0, mixIconPixels(top, bottom, fy), opacity);
            auto& out = icon.pixels[y * side + x];
            out = pixelOver(front, out);
        }
    }
}
static bool advanceExit(ExitEffect& effect, double now) {
    double t = std::clamp((now - effect.since) / ExitEffect::duration, 0., 1.);
    if (t >= 1)
        return false;
    const double travel = 1 - (1 - t) * (1 - t) * (1 - t);
    POINT position{effect.origin.x - effect.padding +
                       LONG(std::lround(effect.dx * travel)),
                   effect.origin.y - effect.padding +
                       LONG(std::lround(effect.dy * travel))};
    BYTE opacity = DragIcon::dragOpacity;
    if (effect.original)
        drawExit(effect, t);
    else
        opacity = BYTE(std::lround(opacity * (1 - exitEase((t - .28) / .72))));
    return presentIcon(*effect.icon, position, opacity);
}
// The release belongs to the gesture, not the popup. Its window can outlive the
// folder.
struct ExitClock {
    using WaitForFrame = DWORD(WINAPI*)(UINT, const HANDLE*, DWORD);
    std::vector<ExitEffect> effects;
    HWND window{};
    HMODULE dcomp{};
    WaitForFrame waitForFrame{};
    PTP_WORK work{};
    HANDLE cancel{}, timer{};
    std::atomic<bool> posted{false};
    std::atomic<unsigned> fallbackMicros{16667};
    bool windowTimer{}, mixedDisplays{};
};
[[clang::no_destroy]] static ExitClock exitClock;
static void CALLBACK waitForExitFrames(PTP_CALLBACK_INSTANCE,
                                       void* context,
                                       PTP_WORK) {
    auto& clock = *static_cast<ExitClock*>(context);
    auto waitForFrame = clock.waitForFrame;
    double due = motionTime();
    for (;;) {
        if (waitForFrame) {
            DWORD result = waitForFrame(1, &clock.cancel, 80);
            if (result != WAIT_OBJECT_0 + 1) {
                if (WaitForSingleObject(clock.cancel, 0) == WAIT_OBJECT_0)
                    return;
                // Unavailable composition (e.g. a disconnected display). Still
                // finish on time.
                waitForFrame = nullptr;
                due = motionTime();
                continue;
            }
        } else {
            double interval = clock.fallbackMicros.load() / 1000.,
                   now = motionTime();
            due += interval;
            if (due <= now)
                due += (std::floor((now - due) / interval) + 1) * interval;
            LARGE_INTEGER relative{};
            relative.QuadPart =
                -std::max(1LL, std::llround((due - now) * 10000));
            if (clock.timer && SetWaitableTimer(clock.timer, &relative, 0,
                                                nullptr, nullptr, FALSE)) {
                HANDLE waits[] = {clock.cancel, clock.timer};
                if (WaitForMultipleObjects(2, waits, FALSE, INFINITE) !=
                    WAIT_OBJECT_0 + 1)
                    return;
            } else if (WaitForSingleObject(clock.cancel,
                                           DWORD(std::ceil(due - now))) !=
                       WAIT_TIMEOUT)
                return;
        }
        // A busy UI samples the latest time once instead of replaying queued,
        // obsolete frames.
        if (!clock.posted.exchange(true) &&
            !PostMessageW(clock.window, kExitFrame, 0, 0))
            return;
    }
}
static void stopExitFrames() {
    auto& clock = exitClock;
    if (clock.cancel)
        SetEvent(clock.cancel);
    if (clock.work) {
        WaitForThreadpoolWorkCallbacks(clock.work, TRUE);
        CloseThreadpoolWork(clock.work);
        clock.work = nullptr;
    }
    if (clock.windowTimer) {
        KillTimer(clock.window, kExitTimer);
        clock.windowTimer = false;
    }
    if (clock.window) {
        MSG old{};
        while (PeekMessageW(&old, clock.window, kExitFrame, kExitFrame,
                            PM_REMOVE)) {
        }
        clock.window = nullptr;
    }
    if (clock.timer) {
        CloseHandle(clock.timer);
        clock.timer = nullptr;
    }
    if (clock.cancel) {
        CloseHandle(clock.cancel);
        clock.cancel = nullptr;
    }
    if (clock.dcomp) {
        FreeLibrary(clock.dcomp);
        clock.dcomp = nullptr;
    }
    clock.waitForFrame = nullptr;
    clock.posted = false;
    clock.effects.clear();
}
static bool scheduleExitFrames() {
    auto& clock = exitClock;
    double interval = clock.effects.front().frameInterval;
    for (const auto& effect : clock.effects)
        interval = std::min(interval, effect.frameInterval);
    clock.fallbackMicros = std::max(1u, unsigned(std::lround(interval * 1000)));
    if (clock.work || clock.windowTimer)
        return true;
    clock.window = controller;
    clock.mixedDisplays = GetSystemMetrics(SM_CMONITORS) > 1;
    clock.dcomp =
        LoadLibraryExW(L"dcomp.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (clock.dcomp)
        clock.waitForFrame = reinterpret_cast<ExitClock::WaitForFrame>(
            GetProcAddress(clock.dcomp, "DCompositionWaitForCompositorClock"));
    clock.cancel = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    clock.timer = CreateWaitableTimerExW(nullptr, nullptr,
                                         CREATE_WAITABLE_TIMER_HIGH_RESOLUTION,
                                         SYNCHRONIZE | TIMER_MODIFY_STATE);
    if (clock.cancel)
        clock.work = CreateThreadpoolWork(waitForExitFrames, &clock, nullptr);
    if (clock.work) {
        SubmitThreadpoolWork(clock.work);
        return true;
    }
    clock.windowTimer =
        SetTimer(clock.window, kExitTimer,
                 std::max(1u, unsigned(std::lround(interval))), nullptr) != 0;
    return clock.windowTimer;
}
static void tickExits() {
    auto& clock = exitClock;
    if (!clock.window)
        return;
    clock.posted = false;
    const double now = motionTime();
    std::erase_if(clock.effects, [&](ExitEffect& effect) {
        if (now - effect.since >= ExitEffect::duration)
            return true;
        // The compositor can wake for another monitor between two refreshes of
        // this one.
        if (clock.mixedDisplays) {
            const double ahead =
                now + effect.frameInterval * .12 - effect.nextFrame;
            if (ahead < 0)
                return false;
            effect.nextFrame += (std::floor(ahead / effect.frameInterval) + 1) *
                                effect.frameInterval;
        }
        return !advanceExit(effect, now);
    });
    if (clock.effects.empty())
        stopExitFrames();
}
static void startExit(std::unique_ptr<DragIcon> icon, POINT release) {
    if (!panel.motion || !controller || !icon)
        return;
    ExitEffect effect;
    effect.origin = icon->position;
    effect.sourceSide = icon->side;
    effect.accent = blendColor(panel.accent, RGB(255, 255, 255), .52);
    const HMONITOR monitor =
        MonitorFromPoint(release, MONITOR_DEFAULTTONEAREST);
    effect.frameInterval = monitorTiming(monitor).interval;
    double carry = 18 * std::exp(-(motionTime() - icon->sampledAt) / 35);
    const double speed = std::hypot(icon->vx, icon->vy);
    if (speed > 0)
        carry = std::min(carry, px(12) / speed);
    effect.dx = icon->vx * carry;
    effect.dy = icon->vy * carry;
    // Bring the shrinking icon gently into view when released at the taskbar or
    // a screen corner.
    MONITORINFO info{sizeof(info)};
    if (GetMonitorInfoW(monitor, &info)) {
        const double half = icon->side * .5, margin = icon->side * .86 + 2;
        auto inside = [&](double start, double offset, LONG low, LONG high) {
            if (high - low < 2 * margin)
                return (low + double(high)) * .5 - start;
            return std::clamp(start + offset, low + margin, high - margin) -
                   start;
        };
        effect.dx = inside(effect.origin.x + half, effect.dx,
                           info.rcMonitor.left, info.rcMonitor.right);
        effect.dy = inside(effect.origin.y + half, effect.dy,
                           info.rcMonitor.top, info.rcMonitor.bottom);
    }
    SetWindowLongPtrW(icon->window, GWLP_HWNDPARENT, 0);
    SetWindowPos(icon->window, HWND_TOPMOST, 0, 0, 0, 0,
                 SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE);
    effect.icon = std::move(icon);
    if (prepareExitSurface(effect) &&
        !presentIcon(*effect.icon,
                     {effect.origin.x - effect.padding,
                      effect.origin.y - effect.padding},
                     DragIcon::dragOpacity))
        return;
    effect.since = motionTime();
    effect.nextFrame = effect.since;
    exitClock.effects.push_back(std::move(effect));
    if (!scheduleExitFrames())
        stopExitFrames();
}
static void capturePreviousPage() {
    if (!panel.motion || !IsWindowVisible(panel.list)) {
        releasePreviousPage();
        return;
    }
    RECT r{};
    GetClientRect(panel.list, &r);
    if (r.right <= 0 || r.bottom <= 0)
        return;
    HDC screen = GetDC(panel.list), dc = CreateCompatibleDC(screen);
    HBITMAP bmp = CreateCompatibleBitmap(screen, r.right, r.bottom);
    ReleaseDC(panel.list, screen);
    if (!dc || !bmp) {
        if (dc)
            DeleteDC(dc);
        if (bmp)
            DeleteObject(bmp);
        releasePreviousPage();
        return;
    }
    auto stock = SelectObject(dc, bmp);
    drawGrid(dc);
    releasePreviousPage();
    panel.previousPage = dc;
    panel.previousBitmap = bmp;
    panel.previousStock = stock;
    panel.previousWidth = r.right;
    panel.previousHeight = r.bottom;
}
static void paintGrid(HWND h) {
    // BeginPaint validates the update region. Capture it before taking the
    // paint DC.
    if (!panel.paintRegion)
        panel.paintRegion = CreateRectRgn(0, 0, 0, 0);
    int regionType =
        panel.paintRegion ? GetUpdateRgn(h, panel.paintRegion, FALSE) : ERROR;
    PAINTSTRUCT ps{};
    HDC screen = BeginPaint(h, &ps);
    RECT r{};
    GetClientRect(h, &r);
    if (r.right > 0 && r.bottom > 0) {
        if (!panel.buffer)
            panel.buffer = CreateCompatibleDC(screen);
        if (panel.buffer && (!panel.bitmap || panel.bufferWidth != r.right ||
                             panel.bufferHeight != r.bottom)) {
            if (auto bmp = CreateCompatibleBitmap(screen, r.right, r.bottom)) {
                auto old = SelectObject(panel.buffer, bmp);
                if (panel.bitmap)
                    DeleteObject(old);
                else
                    panel.stockBitmap = old;
                panel.bitmap = bmp;
                panel.bufferWidth = r.right;
                panel.bufferHeight = r.bottom;
            }
        }
        if (panel.bitmap && panel.bufferWidth == r.right &&
            panel.bufferHeight == r.bottom) {
            int saved = SaveDC(panel.buffer);
            if (saved) {
                if (regionType != COMPLEXREGION ||
                    SelectClipRgn(panel.buffer, panel.paintRegion) == ERROR)
                    IntersectClipRect(panel.buffer, ps.rcPaint.left,
                                      ps.rcPaint.top, ps.rcPaint.right,
                                      ps.rcPaint.bottom);
            }
            drawGrid(panel.buffer);
            if (saved)
                RestoreDC(panel.buffer, saved);
            BitBlt(screen, ps.rcPaint.left, ps.rcPaint.top,
                   ps.rcPaint.right - ps.rcPaint.left,
                   ps.rcPaint.bottom - ps.rcPaint.top, panel.buffer,
                   ps.rcPaint.left, ps.rcPaint.top, SRCCOPY);
        } else
            drawGrid(screen);
    }
    EndPaint(h, &ps);
}
static HWND part(PCWSTR cls,
                 PCWSTR title,
                 DWORD style,
                 int id,
                 bool visible = true) {
    return CreateWindowExW(
        0, cls, title, WS_CHILD | (visible ? WS_VISIBLE : 0) | style, 0, 0, 1,
        1, panel.window, (HMENU)(INT_PTR)id, moduleInstance(), nullptr);
}
static bool wirePanelChild(HWND h) {
    if (!h || !SetWindowSubclass(h, panelChild, 1, 0))
        return false;
    SendMessageW(h, WM_SETFONT, (WPARAM)panel.normalFont, FALSE);
    return true;
}
static void beginRename() {
    if (panel.choosing || panel.renaming || panel.closing || panel.dragging ||
        panel.incoming.token)
        return;
    auto f = lookup(groups, panel.folder);
    if (!f)
        return;
    if (!panel.nameEdit) {
        HWND input =
            part(L"EDIT", L"", WS_TABSTOP | ES_AUTOHSCROLL, kNameEdit, false);
        if (!wirePanelChild(input)) {
            if (input)
                DestroyWindow(input);
            return;
        }
        panel.nameEdit = input;
        SendMessageW(input, WM_SETFONT, (WPARAM)panel.titleFont, FALSE);
        SendMessageW(input, EM_SETLIMITTEXT, 80, 0);
        SendMessageW(input, EM_SETMARGINS, EC_LEFTMARGIN | EC_RIGHTMARGIN,
                     MAKELPARAM(px(2), px(2)));
        SendMessageW(input, EM_SETCUEBANNER, TRUE,
                     (LPARAM)uiText(UiString::RenameTitle));
    }
    RECT r{};
    GetWindowRect(panel.title, &r);
    MapWindowPoints(nullptr, panel.window, (POINT*)&r, 2);
    MoveWindow(panel.nameEdit, r.left, r.top + px(3), r.right - r.left,
               r.bottom - r.top - px(4), FALSE);
    SetWindowTextW(panel.nameEdit, f->name.c_str());
    panel.renaming = true;
    ShowWindow(panel.title, SW_HIDE);
    ShowWindow(panel.nameEdit, SW_SHOWNA);
    SetFocus(panel.nameEdit);
    SendMessageW(panel.nameEdit, EM_SETSEL, 0, -1);
}
static void endRename(bool save, bool restoreFocus) {
    if (!std::exchange(panel.renaming, false))
        return;
    auto id = panel.folder;
    auto name = save ? folderName(windowText(panel.nameEdit)) : wstring{};
    auto f = lookup(groups, id);
    if (f && !name.empty() && f->name != name) {
        unsigned previous = revision;
        if (edit([&](Model& m) {
                if (auto dest = lookup(m, id))
                    dest->name = name;
            }) &&
            panel.window) {
            SetWindowTextW(panel.title, name.c_str());
            SetWindowTextW(panel.window, name.c_str());
            if (panel.rendered == previous)
                panel.rendered = revision;
        }
    }
    if (!panel.window)
        return;
    ShowWindow(panel.title, SW_SHOWNA);
    ShowWindow(panel.nameEdit, SW_HIDE);
    if (restoreFocus)
        SetFocus(panel.apps.empty() ? panel.add : panel.list);
}
static bool ensureSearchControls() {
    if (panel.filter)
        return true;
    panel.lens = part(L"BUTTON", uiText(UiString::SearchInstalled),
                      WS_TABSTOP | BS_OWNERDRAW, kLens, false);
    panel.filter =
        part(L"EDIT", L"", WS_TABSTOP | ES_AUTOHSCROLL, kFilter, false);
    if (!wirePanelChild(panel.lens) || !wirePanelChild(panel.filter)) {
        if (panel.lens)
            DestroyWindow(panel.lens);
        if (panel.filter)
            DestroyWindow(panel.filter);
        panel.lens = panel.filter = nullptr;
        return false;
    }
    SendMessageW(panel.filter, EM_LIMITTEXT, 160, 0);
    SendMessageW(panel.filter, EM_SETCUEBANNER, TRUE,
                 (LPARAM)uiText(UiString::SearchHint));
    SendMessageW(panel.filter, EM_SETMARGINS, EC_LEFTMARGIN | EC_RIGHTMARGIN,
                 MAKELPARAM(px(4), px(4)));
    return true;
}
static void findInFolder() {
    if (panel.choosing || panel.closing || panel.renaming || panel.transfer ||
        panel.pressed >= 0 || panel.incoming.token || !ensureSearchControls())
        return;
    if (!panel.filtering) {
        panel.searchOrigin = panel.page;
        clearPanelSelection();
        SetWindowTextW(panel.filter, L"");
        panel.filtering = true;
        panel.page = 0;
        panel.wheel = 0;
        SetWindowTextW(panel.lens, uiText(UiString::FindInFolder));
        SendMessageW(panel.filter, EM_SETCUEBANNER, TRUE,
                     (LPARAM)uiText(UiString::FindInFolder));
        refreshPanel();
    }
    SetFocus(panel.filter);
}
static void endFolderSearch() {
    if (!panel.filtering)
        return;
    panel.filtering = false;
    clearPanelSelection();
    SetWindowTextW(panel.filter, L"");
    panel.page = panel.searchOrigin;
    panel.wheel = 0;
    refreshPanel();
    if (panel.window)
        SetFocus(panel.apps.empty() ? panel.add : panel.list);
}
static bool ensurePager() {
    if (panel.pager)
        return true;
    panel.prev = part(L"BUTTON", uiText(UiString::PreviousPage),
                      WS_TABSTOP | BS_OWNERDRAW, kPrev, false);
    panel.next = part(L"BUTTON", uiText(UiString::NextPage),
                      WS_TABSTOP | BS_OWNERDRAW, kNext, false);
    panel.pager = part(L"STATIC", L"", SS_OWNERDRAW, kPager, false);
    if (panel.pager && wirePanelChild(panel.prev) && wirePanelChild(panel.next))
        return true;
    for (HWND h : {panel.prev, panel.next, panel.pager})
        if (h)
            DestroyWindow(h);
    panel.prev = panel.next = panel.pager = nullptr;
    return false;
}

static void populatePage() {
    HWND h = panel.list;
    panel.hot = -1;
    panel.tiles.assign(pageItems(), {});
    SendMessageW(h, WM_SETREDRAW, FALSE, 0);
    ListView_DeleteAllItems(h);
    ListView_SetIconSpacing(h, panel.cellWidth, panel.cellHeight);
    for (int i = 0; i < pageItems(); ++i) {
        int n = firstItem() + i;
        const auto& a = panel.apps[n];
        LVITEMW item{};
        item.mask = LVIF_TEXT | LVIF_IMAGE;
        item.iItem = i;
        item.pszText = const_cast<PWSTR>(a.name.c_str());
        item.iImage = I_IMAGECALLBACK;
        ListView_InsertItem(h, &item);
        // Match the native accessibility and keyboard bounds to the painted
        // grid.
        AppRects r{};
        appRects(n, r);
        POINT old{};
        RECT icon{};
        if (ListView_GetItemPosition(h, i, &old) &&
            ListView_GetItemRect(h, i, &icon, LVIR_ICON)) {
            int x = old.x +
                    (r.icon.left + r.icon.right - icon.left - icon.right) / 2;
            int y = old.y +
                    (r.icon.top + r.icon.bottom - icon.top - icon.bottom) / 2;
            ListView_SetItemPosition32(h, i, x, y);
        }
    }
    SendMessageW(h, WM_SETREDRAW, TRUE, 0);
    ShowWindow(h, pageItems() ? SW_SHOWNA : SW_HIDE);
    int pages = pageCount();
    if (panel.pager) {
        for (HWND control : {panel.prev, panel.pager, panel.next})
            ShowWindow(control, pages > 1 ? SW_SHOWNA : SW_HIDE);
        EnableWindow(panel.prev, panel.page > 0);
        EnableWindow(panel.next, panel.page + 1 < pages);
        wstring desc =
            formatUi(UiString::PageOf, std::to_wstring(panel.page + 1),
                     std::to_wstring(pages));
        SetWindowTextW(panel.pager, desc.c_str());
        InvalidateRect(panel.pager, nullptr, FALSE);
    }
    InvalidateRect(h, nullptr, FALSE);
}
static void changePage(int page, bool keepDrag) {
    page = std::clamp(page, 0, pageCount() - 1);
    if (page == panel.page)
        return;
    int old = panel.page;
    int local = ListView_GetNextItem(panel.list, -1, LVNI_FOCUSED);
    if (!keepDrag)
        finishDrag();
    else
        panel.dropTarget = -1;
    capturePreviousPage();
    panel.page = page;
    ++panel.viewSerial;
    populatePage();
    gridEntrance(page > old ? 1 : -1);
    if (!keepDrag && local >= 0 && pageItems())
        ListView_SetItemState(panel.list, std::min(local, pageItems() - 1),
                              LVIS_SELECTED | LVIS_FOCUSED,
                              LVIS_SELECTED | LVIS_FOCUSED);
}
static bool pageWheel(UINT msg, WPARAM wp) {
    if (pageCount() < 2)
        return false;
    int delta = GET_WHEEL_DELTA_WPARAM(wp);
    panel.wheel += (msg == WM_MOUSEHWHEEL ? delta : -delta);
    int turns = panel.wheel / WHEEL_DELTA;
    if (turns) {
        panel.wheel %= WHEEL_DELTA;
        int old = panel.page;
        const bool incoming = panel.incoming.token != 0;
        changePage(old + turns, panel.dragging || panel.transfer || incoming);
        if (panel.page != old) {
            if (panel.dragging)
                dragAt(panel.dragPoint);
            else if (incoming) {
                POINT pointer{};
                if (GetCursorPos(&pointer))
                    positionIncoming(pointer);
            }
        }
    }
    return true;
}
static void refreshPanel() {
    panel.incoming.lastPage = -1;
    auto f = lookup(groups, panel.folder);
    if (!f) {
        closePanel();
        return;
    }
    if (panel.windowFolder != f->id)
        panel.openWindows.clear();
    if (!panel.choosing &&
        (panel.windowFolder != f->id || panel.windowRevision != revision))
        scheduleWindowScan();
    if (!panel.designMenu)
        applyPanelStyle(f->style);
    finishDrag();
    releasePreviousPage();
    ++panel.viewSerial;
    const bool searchField = panel.choosing || panel.filtering;
    wstring query = searchField ? windowText(panel.filter) : L"";
    auto terms = searchTerms(query);
    bool searching = !terms.empty();
    if (!panel.choosing) {
        if (panel.filtering && searching) {
            panel.apps.clear();
            for (const auto& app : f->apps)
                if (matchName(app.name, terms))
                    panel.apps.push_back(app);
        } else
            panel.apps = f->apps;
        if (panel.incoming.token) {
            if (panel.rendered != revision)
                panel.incoming = {};
            else {
                auto& preview = panel.incoming;
                preview.slot =
                    std::clamp(preview.slot, 0, (int)panel.apps.size());
                panel.apps.insert(panel.apps.begin() + preview.slot,
                                  preview.apps.begin(), preview.apps.end());
            }
        }
    } else {
        if (panel.choicesRevision != revision) {
            panel.search.generation.reset();
            panel.pins = taskbarChoices(*f);
            panel.choicesRevision = revision;
        }
        if (!searching)
            panel.apps = panel.pins;
        else {
            wantCatalog(controller ? controller : panel.window);
            if (panel.search.generation != catalogGeneration)
                panel.search.build(f->apps, panel.pins, installedApps,
                                   catalogGeneration);
            panel.search.filter(terms, panel.apps);
        }
    }
    MONITORINFO mi{sizeof(mi)};
    GetMonitorInfoW(MonitorFromRect(&panel.anchor, MONITOR_DEFAULTTONEAREST),
                    &mi);
    HDC dc = GetDC(panel.list);
    auto before = SelectObject(dc, panel.normalFont);
    TEXTMETRICW tm{};
    GetTextMetricsW(dc, &tm);
    SelectObject(dc, before);
    ReleaseDC(panel.list, dc);
    panel.lineHeight = std::max<int>(1, tm.tmHeight + tm.tmExternalLeading);
    panel.iconSize = px(36);
    panel.cellHeight =
        px(5) + panel.iconSize + px(5) + 2 * panel.lineHeight + px(9);
    int monitorW = mi.rcWork.right - mi.rcWork.left, pad = px(12);
    int cellW = std::min(px(88), std::max(1, monitorW - 2 * pad));
    int available = std::clamp((monitorW - 2 * pad) / cellW, 1, 4),
        count = (int)panel.apps.size();
    int cols = searchField ? available : std::clamp(count, 1, available);
    int rows = (count + cols - 1) / cols,
        gridTop = searchField ? px(88) : px(44);
    int room =
        (int)std::max(
            mi.rcWork.bottom - std::max(panel.bar.bottom, panel.anchor.bottom),
            std::min(panel.bar.top, panel.anchor.top) - mi.rcWork.top) -
        px(7);
    int capRows =
        std::clamp((room - gridTop - px(12)) / panel.cellHeight, 1, 4);
    if (rows > capRows)
        capRows =
            std::clamp((room - gridTop - px(39)) / panel.cellHeight, 1, 4);
    int shown =
        panel.filtering
            ? std::min(capRows,
                       std::max(1, ((int)f->apps.size() + cols - 1) / cols))
        : panel.choosing
            ? std::min(
                  capRows,
                  std::max({2, rows,
                            std::min(4, ((int)panel.pins.size() + cols - 1) /
                                            cols)}))
            : std::min(rows, capRows);
    panel.cellWidth = cellW;
    panel.columns = cols;
    panel.capacity = std::max(1, cols * capRows);
    panel.page = std::clamp(panel.page, 0, pageCount() - 1);
    int width = std::min(monitorW, std::max(px(190), 2 * pad + cols * cellW));
    int wanted = gridTop + (shown ? shown * panel.cellHeight : px(41)) +
                 (pageCount() > 1 ? px(36) : px(10));
    panel.rest =
        placePanel(mi.rcWork, panel.bar, panel.anchor, width, wanted, px(8));
    panelPosition(motionTime());
    RECT client{};
    GetClientRect(panel.window, &client);
    int w = client.right, h = client.bottom, side = px(28), gap = px(4);
    int last = w - pad - side, addX = last - side - gap;
    MoveWindow(panel.title, pad + px(2), px(8), std::max(1, addX - pad - gap),
               side, TRUE);
    MoveWindow(panel.add, addX, px(8), side, side, TRUE);
    MoveWindow(panel.more, last, px(8), side, side, TRUE);
    panel.searchRect = {pad, px(44), w - pad, px(76)};
    if (panel.filter) {
        MoveWindow(panel.lens, pad + px(4), px(47), px(27), px(26), TRUE);
        MoveWindow(panel.filter, pad + px(33),
                   px(44) + (px(32) - panel.lineHeight) / 2,
                   std::max(1, w - 2 * pad - px(42)), panel.lineHeight + 1,
                   TRUE);
        ShowWindow(panel.lens, searchField ? SW_SHOWNA : SW_HIDE);
        ShowWindow(panel.filter, searchField ? SW_SHOWNA : SW_HIDE);
    }
    int gridHeight =
        std::max(1, h - gridTop - (pageCount() > 1 ? px(36) : px(10)));
    MoveWindow(panel.empty, pad, gridTop, std::max(1, w - 2 * pad), gridHeight,
               TRUE);
    int listWidth = std::max(1, std::min(w, cols * cellW));
    MoveWindow(panel.list, (w - listWidth) / 2, gridTop, listWidth, gridHeight,
               TRUE);
    if (pageCount() > 1 && ensurePager()) {
        int footerY = h - px(32), ind = px(76), btn = px(26);
        MoveWindow(panel.prev, (w - ind) / 2 - btn, footerY, btn, btn, TRUE);
        MoveWindow(panel.pager, (w - ind) / 2, footerY, ind, btn, TRUE);
        MoveWindow(panel.next, (w + ind) / 2, footerY, btn, btn, TRUE);
    }
    auto heading =
        panel.choosing ? formatUi(UiString::AddTo, f->name) : f->name;
    SetWindowTextW(panel.window, heading.c_str());
    SetWindowTextW(panel.title, heading.c_str());
    SetWindowTextW(panel.more, panel.choosing ? uiText(UiString::Back)
                                              : uiText(UiString::Options));
    SetWindowTextW(panel.add, panel.choosing ? uiText(UiString::FromFile)
                                             : uiText(UiString::AddApps));
    PCWSTR emptyText = uiText(panel.filtering && searching ? UiString::NoResults
                                                           : UiString::Empty);
    if (panel.choosing)
        emptyText = !searching                 ? uiText(UiString::TypeOrBrowse)
                    : catalogThread.joinable() ? uiText(UiString::Searching)
                    : catalogFailed            ? uiText(UiString::NoCatalog)
                                               : uiText(UiString::NoResults);
    SetWindowTextW(panel.empty, emptyText);
    if (panel.tips)
        for (HWND button : {panel.add, panel.more}) {
            TOOLINFOW ti{sizeof(ti)};
            ti.uFlags = TTF_IDISHWND | TTF_SUBCLASS;
            ti.hwnd = panel.window;
            ti.uId = (UINT_PTR)button;
            ti.lpszText = const_cast<LPWSTR>(
                button == panel.add
                    ? (panel.choosing ? uiText(UiString::FromFile)
                                      : uiText(UiString::AddApps))
                    : (panel.choosing ? uiText(UiString::Back)
                                      : uiText(UiString::Options)));
            SendMessageW(panel.tips, TTM_UPDATETIPTEXTW, 0, (LPARAM)&ti);
        }

    if (!panel.icons) {
        int size = px(36);
        panel.icons =
            ImageList_Create(size, size, ILC_COLOR32 | ILC_MASK, 4, 4);
        ListView_SetImageList(panel.list, panel.icons, LVSIL_NORMAL);
    }
    populatePage();
    ShowWindow(panel.empty, rows ? SW_HIDE : SW_SHOWNA);
    panel.rendered = revision;
    RedrawWindow(panel.window, nullptr, nullptr,
                 RDW_INVALIDATE | RDW_ALLCHILDREN | RDW_ERASE);
}
static void syncPanel() {
    if (panel.window && panel.rendered != revision)
        refreshPanel();
}
static void dismissOutside(POINT p) {
    if (!panel.window || panel.browsing || isIn(panel.anchor, p) ||
        panel.pressed >= 0 || panel.transfer)
        return;
    RECT bounds{};
    GetWindowRect(panel.window, &bounds);
    if (!isIn(bounds, p))
        dismissPanel();
}
static void drawCommand(const DRAWITEMSTRUCT& di) {
    RECT r = di.rcItem;
    bool lens = di.hwndItem == panel.lens;
    if (lens)
        FillRect(di.hDC, &r, panel.searchBrush);
    else
        paintBackdrop(di.hDC, di.hwndItem, r);
    if (di.hwndItem == panel.title) {
        int saved = SaveDC(di.hDC);
        HFONT font = panel.shownStyle == FolderStyle::Paper &&
                             !panel.highContrast && panel.paperFont
                         ? panel.paperFont
                         : panel.titleFont;
        SelectObject(di.hDC, font);
        SetBkMode(di.hDC, TRANSPARENT);
        SetTextColor(di.hDC, panel.foreground);
        if (panel.finish != TileFinish::Plain) {
            int height = px(panel.shownStyle == FolderStyle::Oled ? 5 : 14),
                top = (r.bottom - height) / 2;
            RECT mark{r.left, top, r.left + px(3), top + height};
            rounded(di.hDC, mark, panel.accent, px(2));
            r.left += px(9);
        }
        auto title = windowText(di.hwndItem);
        DrawTextW(di.hDC, title.c_str(), (int)title.size(), &r,
                  DT_LEFT | DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS |
                      DT_NOPREFIX);
        RestoreDC(di.hDC, saved);
        return;
    }
    if (di.hwndItem == panel.pager) {
        int pages = pageCount(), dots = std::min(7, pages),
            first = std::clamp(panel.page - 3, 0, pages - dots);
        int x = (r.right - (dots - 1) * px(9) - px(14)) / 2, cy = r.bottom / 2;
        for (int i = 0; i < dots; i++) {
            bool current = first + i == panel.page;
            int w = px(current ? 14 : 4), height = px(4);
            RECT dot{x, cy - height / 2, x + w, cy + (height + 1) / 2};
            rounded(di.hDC, dot, current ? panel.accent : panel.rim, px(2));
            x += w + px(5);
        }
        return;
    }
    bool pressed = (di.itemState & ODS_SELECTED) != 0;
    bool focus =
        (di.itemState & ODS_FOCUS) && !(di.itemState & ODS_NOFOCUSRECT);
    int saved = SaveDC(di.hDC);
    double hover = panel.controls[di.hwndItem].value(
        panel.frame ? panel.frameSample : motionTime());
    if ((panel.finish != TileFinish::Plain && !lens) || pressed || focus ||
        hover > .005) {
        auto base = (lens || panel.finish != TileFinish::Plain) ? panel.field
                                                                : panel.surface;
        COLORREF face = blendColor(base, pressed ? panel.rim : panel.buttonFace,
                                   pressed ? 1 : hover);
        HBRUSH fill = CreateSolidBrush(face);
        HPEN pen =
            CreatePen(PS_SOLID, 1,
                      focus                               ? panel.foreground
                      : panel.finish != TileFinish::Plain ? panel.tileEdge
                                                          : face);
        auto before = SelectObject(di.hDC, fill),
             line = SelectObject(di.hDC, pen);
        int diameter =
            px(panel.finish == TileFinish::Plain ? 10 : 2 * panel.radius);
        RoundRect(di.hDC, r.left + 1, r.top + 1, r.right - 1, r.bottom - 1,
                  diameter, diameter);
        SelectObject(di.hDC, before);
        SelectObject(di.hDC, line);
        DeleteObject(fill);
        DeleteObject(pen);
    }
    int x = (r.left + r.right) / 2, y = (r.top + r.bottom) / 2,
        arm = px(pressed ? 4 : 5);
    COLORREF ink = IsWindowEnabled(di.hwndItem) ? panel.foreground : panel.rim;
    HPEN pen = CreatePen(PS_SOLID, std::max(1, px(1)), ink);
    HBRUSH fill = CreateSolidBrush(panel.foreground);
    SelectObject(di.hDC, pen);
    SelectObject(di.hDC, fill);
    auto line = [&](int x1, int y1, int x2, int y2) {
        MoveToEx(di.hDC, x1, y1, nullptr);
        LineTo(di.hDC, x2, y2);
    };
    if (di.hwndItem == panel.prev || di.hwndItem == panel.next) {
        int dir = di.hwndItem == panel.next ? 1 : -1;
        line(x - dir * arm / 2, y - arm, x + dir * arm / 2, y);
        line(x + dir * arm / 2, y, x - dir * arm / 2, y + arm);
    } else if (di.hwndItem == panel.lens) {
        auto b = SelectObject(di.hDC, GetStockObject(NULL_BRUSH));
        Ellipse(di.hDC, x - arm, y - arm, x + arm / 2 + 1, y + arm / 2 + 1);
        SelectObject(di.hDC, b);
        line(x + arm / 2 - 1, y + arm / 2 - 1, x + arm + 2, y + arm + 2);
    } else if (di.hwndItem == panel.add) {
        if (panel.choosing) {
            POINT outline[] = {
                {x - arm - 1, y - arm}, {x - 1, y - arm},
                {x + 1, y - arm / 2},   {x + arm + 1, y - arm / 2},
                {x + arm + 1, y + arm}, {x - arm - 1, y + arm},
                {x - arm - 1, y - arm}};
            Polyline(di.hDC, outline, ARRAYSIZE(outline));
        } else {
            line(x - arm, y, x + arm + 1, y);
            line(x, y - arm, x, y + arm + 1);
        }
    } else if (panel.choosing) {
        line(x + arm / 2, y - arm, x - arm / 2, y);
        line(x - arm / 2, y, x + arm / 2, y + arm);
    } else {
        int dot = std::max(2, px(2));
        for (int i = -1; i <= 1; i++) {
            int left = x + i * px(4) - dot / 2;
            Ellipse(di.hDC, left, y - dot / 2, left + dot, y - dot / 2 + dot);
        }
    }
    RestoreDC(di.hDC, saved);
    DeleteObject(fill);
    DeleteObject(pen);
}

static void beginAdding(const wstring& dest) {
    if (panel.incoming.token)
        return;
    if (!panel.window || panel.folder != dest || panel.closing)
        openMenuFolder(dest);
    if (!panel.window || panel.folder != dest)
        return;
    if (!ensureSearchControls()) {
        note(uiText(UiString::SearchFailed));
        return;
    }
    panel.filtering = false;
    clearPanelSelection();
    panel.choosing = false;
    SetWindowTextW(panel.filter, L"");
    panel.choicesRevision = ~0u;
    SetWindowTextW(panel.lens, uiText(UiString::SearchInstalled));
    SendMessageW(panel.filter, EM_SETCUEBANNER, TRUE,
                 (LPARAM)uiText(UiString::SearchHint));
    panel.page = 0;
    panel.wheel = 0;
    panel.choosing = true;
    refreshPanel();
    gridEntrance(1);
    SetFocus(panel.filter);
}
static void workerFinished(UINT msg) {
    if (msg == kCatalogDone) {
        if (collectCatalog() && panel.window && panel.choosing &&
            GetWindowTextLengthW(panel.filter))
            refreshPanel();
        return;
    }
    if (!fileThread.joinable() || !filesReady.load())
        return;
    fileThread.join();
    filesReady = false;
    auto paths = std::move(selectedPaths);
    auto dest = std::exchange(fileDestination, wstring{});
    bool added = false;
    if (!stopping && lookup(groups, dest) && !paths.empty()) {
        std::vector<App> apps;
        for (auto& path : paths)
            if (auto app = fromPath(path))
                apps.push_back(std::move(*app));
        if (!apps.empty())
            added = edit([&](Model& m) {
                for (const auto& a : apps)
                    transfer(m, dest, a);
            });
        else
            note(uiText(UiString::NoUsableFile));
    }
    if (!stopping && panel.window && panel.browsing && panel.folder == dest) {
        panel.browsing = false;
        if (added) {
            panel.choosing = false;
            panel.page = 0;
        }
        refreshPanel();
        gridEntrance(0);
        ShowWindow(panel.window, SW_SHOW);
        SetForegroundWindow(panel.window);
        SetFocus(panel.choosing ? panel.filter : panel.list);
    }
    if (FAILED(filesStatus) && !stopping)
        note(uiText(UiString::PickerFailed));
}
static void stopPanelActivationCheck(HWND window) {
    panel.waitForDrag = false;
    KillTimer(window, kPanelActivationTimer);
}
static bool primaryMouseDown() {
    int button = GetSystemMetrics(SM_SWAPBUTTON) ? VK_RBUTTON : VK_LBUTTON;
    return (GetAsyncKeyState(button) & 0x8000) != 0;
}
static void checkPanelActivation(HWND window, bool fromTimer) {
    if (panel.window != window || panel.browsing || panel.closing ||
        panel.transfer || panel.dragging)
        return;
    HWND foreground = GetForegroundWindow();
    if (!foreground || foreground == window || IsChild(window, foreground) ||
        GetAncestor(foreground, GA_ROOTOWNER) == window) {
        stopPanelActivationCheck(window);
        return;
    }
    if (panel.dropRegistered && !panel.choosing &&
        (panel.dropHover ||
         ((!ui.drag.app.empty() || shellFileWindow(foreground, true)) &&
          primaryMouseDown()))) {
        if (fromTimer || SetTimer(window, kPanelActivationTimer, 35, nullptr)) {
            panel.waitForDrag = true;
            return;
        }
    }
    if (panel.waitForDrag) {
        // Give OLE the next turn after button-up to finish the accepted drop.
        panel.waitForDrag = false;
        if (fromTimer)
            return;
    }
    stopPanelActivationCheck(window);
    dismissPanel();
}
static std::optional<App> droppedShortcut(const wstring&);
static void stopIncomingEdge() {
    if (panel.autoScroll && panel.list)
        KillTimer(panel.list, kDragPageTimer);
    panel.autoScroll = false;
    panel.edgeDir = 0;
}
static void clearIncoming(uint64_t token, bool committed = false) {
    if (!token || token != panel.incoming.token)
        return;
    stopIncomingEdge();
    int page = panel.incoming.originalPage;
    auto f = lookup(groups, panel.folder);
    if (committed && f && f->apps == panel.apps) {
        int first = panel.incoming.slot,
            last = first + (int)panel.incoming.apps.size();
        panel.incoming = {};
        panel.rendered = revision;
        scheduleWindowScan();
        for (int n = std::max(first, firstItem());
             n < std::min(last, firstItem() + pageItems()); ++n)
            redrawTile(n);
        return;
    }
    auto poses = visiblePoses();
    panel.incoming = {};
    if (panel.window) {
        if (!committed)
            panel.page = page;
        refreshPanel();
        if (panel.window) {
            settleTiles(poses);
            if (panel.runningDirty)
                scheduleWindowScan();
        }
    }
}
static int incomingSlot(POINT p, int count) {
    RECT grid{};
    GetClientRect(panel.list, &grid);
    if (isIn(grid, p) && panel.cellWidth > 0 && panel.cellHeight > 0) {
        int column = std::min<int>(p.x / panel.cellWidth, panel.columns - 1);
        return std::clamp(firstItem() +
                              (int)(p.y / panel.cellHeight) * panel.columns +
                              column,
                          0, count);
    }
    return panel.incoming.token ? panel.incoming.slot : count;
}
static uint64_t beginIncoming(const std::vector<App>& candidates,
                              POINT screen,
                              bool atEnd = false) {
    if (!panel.window || !panel.list || panel.choosing || panel.closing ||
        panel.browsing || panel.dragging)
        return 0;
    if (panel.filtering)
        endFolderSearch();
    if (panel.rendered != revision)
        refreshPanel();
    auto f = lookup(groups, panel.folder);
    if (!f || !panel.window)
        return 0;
    std::vector<App> fresh;
    if (candidates.size() == 1) {
        const auto& app = candidates.front();
        if (std::none_of(f->apps.begin(), f->apps.end(),
                         [&](const App& old) { return sameApp(old, app); }))
            fresh.push_back(app);
    } else {
        AppIdentitySet present;
        for (const auto& app : f->apps)
            present.add(app);
        fresh.reserve(candidates.size());
        for (const auto& app : candidates)
            if (!present.contains(app)) {
                present.add(app);
                fresh.push_back(app);
            }
    }
    if (panel.incoming.token && !fresh.empty() &&
        fresh == panel.incoming.apps) {
        // The shell button and the popup are separate drop targets for the same
        // gesture. Hand off the live layout and make any pending leave from the
        // old target harmless.
        panel.incoming.token = ++nextPreviewToken;
        if (!atEnd)
            positionIncoming(screen);
        return panel.incoming.token;
    }
    clearIncoming(panel.incoming.token);
    if (fresh.empty())
        return 0;
    auto poses = visiblePoses();
    int count = (int)f->apps.size();
    if (panel.renaming)
        endRename(true, false);
    POINT local = screen;
    ScreenToClient(panel.list, &local);
    int slot = atEnd ? count : incomingSlot(local, count);
    auto token = ++nextPreviewToken;
    panel.incoming = {token, std::move(fresh), slot, panel.page};
    // Full pages keep their capacity when room for the incoming item is
    // reserved.
    if (count >= panel.capacity)
        panel.page = slot / panel.capacity;
    refreshPanel();
    if (panel.window)
        settleTiles(poses);
    return token;
}
static void positionIncoming(POINT screen) {
    auto& preview = panel.incoming;
    if (!preview.token || !panel.list)
        return;
    if (preview.lastPage == panel.page && preview.lastPoint.x == screen.x &&
        preview.lastPoint.y == screen.y)
        return;
    preview.lastPage = panel.page;
    preview.lastPoint = screen;
    POINT p = screen;
    ScreenToClient(panel.list, &p);
    updateDragEdge(p);
    int size = (int)preview.apps.size(), old = preview.slot;
    int slot = incomingSlot(p, (int)panel.apps.size() - size);
    if (slot == old)
        return;
    auto poses = visiblePoses();
    auto it = panel.apps.begin();
    if (slot < old)
        std::rotate(it + slot, it + old, it + old + size);
    else
        std::rotate(it + old, it + old + size, it + slot + size);
    preview.slot = slot;
    updatePageRange(std::min(old, slot), std::max(old, slot) + size - 1);
    settleTiles(poses);
}
static std::optional<App> incomingAnchor(uint64_t token) {
    if (token && token == panel.incoming.token) {
        int after = panel.incoming.slot + (int)panel.incoming.apps.size();
        if (validIndex(after, panel.apps.size()))
            return panel.apps[after];
    }
    return {};
}
static void insertDropped(Model& model,
                          const wstring& folder,
                          const std::vector<App>& incoming,
                          const std::optional<App>& before) {
    auto dest = lookup(model, folder);
    if (!dest)
        return;
    auto& apps = dest->apps;
    auto count = apps.size(), slot = count;
    if (before) {
        auto anchor = std::find_if(apps.begin(), apps.end(), [&](const App& a) {
            return sameApp(a, *before);
        });
        slot = static_cast<size_t>(anchor - apps.begin());
    }
    if (incoming.size() > 1) {
        AppIdentitySet moving;
        bool allNew =
            std::all_of(incoming.begin(), incoming.end(), [&](const App& a) {
                if (a.target.empty() || moving.contains(a))
                    return false;
                moving.add(a);
                return true;
            });
        if (allNew && std::none_of(apps.begin(), apps.end(), [&](const App& a) {
                return moving.contains(a);
            })) {
            for (auto& other : model)
                if (other.id != folder)
                    std::erase_if(other.apps, [&](const App& a) {
                        return moving.contains(a);
                    });
            apps.insert(apps.begin() + slot, incoming.begin(), incoming.end());
            return;
        }
    }
    for (const auto& app : incoming)
        transfer(model, folder, app);
    std::rotate(apps.begin() + slot, apps.begin() + count, apps.end());
}
// Move the selected members in folder order. A batch reads each folder once.
static void moveFolderMembers(Model& model,
                              const wstring& from,
                              const wstring& to,
                              const std::vector<App>& batch,
                              const std::optional<App>& before = {}) {
    auto source = lookup(model, from);
    auto dest = to.empty() ? nullptr : lookup(model, to);
    if (!source || from == to || (!to.empty() && !dest) || batch.empty())
        return;
    AppIdentitySet moving;
    for (const auto& a : batch)
        moving.add(a);
    auto member = [&](const App& a) { return moving.contains(a); };
    if (std::count_if(source->apps.begin(), source->apps.end(), member) !=
        static_cast<ptrdiff_t>(batch.size()))
        return;
    if (!dest) {
        std::erase_if(source->apps, member);
        return;
    }
    if (std::any_of(dest->apps.begin(), dest->apps.end(), member)) {
        insertDropped(model, to, batch, before);
        return;
    }
    for (auto& folder : model)
        if (folder.id != to)
            std::erase_if(folder.apps, member);
    auto insertAt = dest->apps.end();
    if (before)
        insertAt =
            std::find_if(dest->apps.begin(), insertAt,
                         [&](const App& a) { return sameApp(a, *before); });
    dest->apps.insert(insertAt, batch.begin(), batch.end());
}
static void appMenu(wstring dest, App app, const POINT* position = nullptr) {
    auto folder = lookup(groups, dest);
    if (!folder)
        return;
    std::vector<App> batch;
    if (panel.selection.contains(app.target)) {
        for (const auto& member : folder->apps)
            if (panel.selection.contains(member.target))
                batch.push_back(member);
    }
    if (batch.empty())
        batch.push_back(app);
    HWND owner = panel.window;
    auto version = revision;
    Menu menu;
    menu.item(1, batch.size() == 1 ? wstring(uiText(UiString::RemoveApp))
                                   : formatUi(UiString::RemoveSelected,
                                              std::to_wstring(batch.size())));
    if (menu.show(owner, nullptr, position) != 1 || owner != panel.window ||
        panel.closing || panel.folder != dest || revision != version)
        return;
    if (editMove([&](Model& model) {
            if (batch.size() == 1)
                removeApp(model, dest, app);
            else
                moveFolderMembers(model, dest, L"", batch);
        }))
        refreshPanel();
}
// A single desktop gesture can cross several closed tiles and the open panel.
// Resolve shortcut metadata once per data object, only for the life of that
// drag.
static bool fileDragActive = false;
static void finishFileGesture();
[[clang::no_destroy]] static com_ptr<::IUnknown> fileDataIdentity;
static std::vector<App> fileDataApps;
struct PanelDropTarget
    : winrt::implements<PanelDropTarget, IDropTarget, winrt::non_agile> {
    HWND window;
    std::vector<App> items;
    uint64_t token{};

    explicit PanelDropTarget(HWND host) : window(host) {}
    bool open() const {
        return panel.window == window && !stopping && !panel.closing &&
               !panel.browsing && !panel.choosing && !panel.transfer;
    }
    DWORD accept(DWORD offered) const {
        if (!open() || items.empty())
            return DROPEFFECT_NONE;
        if (offered & DROPEFFECT_COPY)
            return DROPEFFECT_COPY;
        if (offered & DROPEFFECT_LINK)
            return DROPEFFECT_LINK;
        return DROPEFFECT_NONE;
    }
    static FORMATETC fileFormat() {
        return {CF_HDROP, nullptr, DVASPECT_CONTENT, -1, TYMED_HGLOBAL};
    }

    static std::vector<App> readItems(IDataObject* data) {
        std::vector<App> apps;
        if (!data)
            return apps;
        com_ptr<::IUnknown> identity;
        if (fileDragActive &&
            SUCCEEDED(data->QueryInterface(IID_PPV_ARGS(identity.put()))) &&
            identity == fileDataIdentity)
            return fileDataApps;
        auto format = fileFormat();
        struct FileMedium {
            STGMEDIUM value{};
            ~FileMedium() { ReleaseStgMedium(&value); }
        } medium;
        if (FAILED(data->GetData(&format, &medium.value)) ||
            medium.value.tymed != TYMED_HGLOBAL)
            return apps;
        auto files = reinterpret_cast<HDROP>(medium.value.hGlobal);
        if (!files)
            return apps;
        UINT count = DragQueryFileW(files, UINT_MAX, nullptr, 0);
        for (UINT i = 0; i < count; ++i) {
            UINT length = DragQueryFileW(files, i, nullptr, 0);
            if (!length)
                continue;
            wstring path(length + 1, L'\0');
            if (DragQueryFileW(files, i, path.data(), length + 1) != length)
                continue;
            path.resize(length);
            if (auto app = droppedShortcut(path))
                apps.push_back(std::move(*app));
        }
        if (fileDragActive && identity) {
            fileDataApps = apps;
            fileDataIdentity = std::move(identity);
        }
        return apps;
    }
    HRESULT STDMETHODCALLTYPE DragEnter(IDataObject* data,
                                        DWORD keys,
                                        POINTL point,
                                        DWORD* effect) noexcept final {
        if (!effect)
            return E_POINTER;
        try {
            DragLeave();
            if (open() && (*effect & (DROPEFFECT_COPY | DROPEFFECT_LINK))) {
                items = readItems(data);
                if (!items.empty())
                    token = beginIncoming(items, {point.x, point.y});
            }
            return DragOver(keys, point, effect);
        } catch (...) {
            DragLeave();
            *effect = DROPEFFECT_NONE;
            return S_OK;
        }
    }
    HRESULT STDMETHODCALLTYPE DragOver(DWORD,
                                       POINTL point,
                                       DWORD* effect) noexcept final {
        if (!effect)
            return E_POINTER;
        try {
            *effect = accept(*effect);
            if (panel.window == window) {
                panel.dropHover = *effect != DROPEFFECT_NONE;
                if (panel.dropHover && token && token == panel.incoming.token)
                    positionIncoming({point.x, point.y});
            }
        } catch (...) {
            DragLeave();
            *effect = DROPEFFECT_NONE;
        }
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE DragLeave() noexcept final {
        items.clear();
        auto previous = std::exchange(token, 0);
        if (panel.window == window) {
            panel.dropHover = false;
            try {
                clearIncoming(previous);
            } catch (...) {
            }
        }
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE Drop(IDataObject* data,
                                   DWORD,
                                   POINTL point,
                                   DWORD* effect) noexcept final {
        if (!effect)
            return E_POINTER;
        DWORD operation = accept(*effect);
        *effect = DROPEFFECT_NONE;
        try {
            if (operation != DROPEFFECT_NONE && data) {
                if (token && token == panel.incoming.token)
                    positionIncoming({point.x, point.y});
                DropJob job{panel.folder};
                job.before = incomingAnchor(token);
                job.apps = std::move(items);
                job.previewToken = token;
                if (queueDrop(std::move(job))) {
                    token = 0;
                    stopIncomingEdge();
                    stopPanelActivationCheck(window);
                    PostMessageW(window, kCheckActivation, 1, 0);
                    *effect = operation;
                }
            }
        } catch (...) {
            Wh_Log(L"Could not accept the dropped shortcut.");
        }
        DragLeave();
        finishFileGesture();
        return S_OK;
    }
};
static void registerPanelDrop(HWND window) {
    auto target = winrt::make_self<PanelDropTarget>(window);
    if (FAILED(OleInitialize(nullptr)))
        return;
    if (SUCCEEDED(RegisterDragDrop(window, target.get())))
        panel.dropRegistered = true;
    else
        OleUninitialize();
}
static LRESULT CALLBACK panelProc(HWND w, UINT msg, WPARAM wp, LPARAM lp) {
    try {
        switch (msg) {
            case kFilesDone:
            case kCatalogDone:
                workerFinished(msg);
                return 0;
            case kRunningReady:
                if (w == panel.window)
                    acceptWindowScan(wp);
                return 0;
            case kPanelFrame:
            case WM_TIMER:
                if (msg == kPanelFrame || wp == kFrameTimer) {
                    if (w != panel.window || !panel.frame ||
                        panel.fallbackTimer != (msg == WM_TIMER))
                        return 0;
                    tickPanel();
                    if (panel.window == w && panel.list)
                        UpdateWindow(panel.list);
                    return 0;
                }
                if (wp == kPanelActivationTimer) {
                    checkPanelActivation(w, true);
                    return 0;
                }
                if (wp == kRunningTimer) {
                    KillTimer(w, kRunningTimer);
                    panel.windowTimer = false;
                    requestWindowScan();
                    return 0;
                }
                break;
            case WM_MOUSEWHEEL:
            case WM_MOUSEHWHEEL:
                if (pageWheel(msg, wp))
                    return 0;
                break;
            case WM_DISPLAYCHANGE:
                panel.timingReadAt = 0;
                break;
            case WM_WINDOWPOSCHANGED:
                if (w == panel.window && lp && panel.frameMonitor &&
                    !(reinterpret_cast<WINDOWPOS*>(lp)->flags & SWP_NOMOVE) &&
                    MonitorFromWindow(w, MONITOR_DEFAULTTONEAREST) !=
                        panel.frameMonitor)
                    panel.timingReadAt = 0;
                break;
            case WM_POWERBROADCAST:
                if (wp == PBT_APMPOWERSTATUSCHANGE ||
                    wp == PBT_POWERSETTINGCHANGE)
                    panel.timingReadAt = 0;
                break;
            case WM_MENUSELECT:
                previewMenuChoice((HMENU)lp, LOWORD(wp), HIWORD(wp));
                break;
            case WM_THEMECHANGED:
            case WM_SYSCOLORCHANGE:
            case WM_DWMCOLORIZATIONCOLORCHANGED:
            case WM_SETTINGCHANGE:
                if (panel.window == w) {
                    try {
                        if (ui.root)
                            panel.systemDark =
                                ui.root.ActualTheme() == X::ElementTheme::Dark;
                    } catch (...) {
                    }
                    if (panel.designMenu)
                        applyPanelStyle(panel.requestedStyle, true);
                    else
                        restorePanelStyle(true);
                }
                if (msg != WM_SETTINGCHANGE)
                    break;
                panel.timingReadAt = 0;
                if (panel.window == w) {
                    panel.motion = allowMotion();
                    if (!panel.motion)
                        stopExitFrames();
                    if (!panel.motion && panel.frame) {
                        if (panel.closing) {
                            closePanel();
                            return 0;
                        }
                        panel.reveal.set(1);
                        panel.arrival.set(0);
                        for (auto& t : panel.tiles)
                            for (Ease* e : std::array<Ease*, 4>{
                                     &t.dx, &t.dy, &t.hover, &t.press})
                                e->set(e->end);
                        for (auto& c : panel.controls)
                            c.second.set(c.second.end);
                        InvalidateRect(panel.list, nullptr, FALSE);
                        tickPanel();
                    }
                }
                break;
            case WM_CTLCOLOREDIT: {
                auto dc = (HDC)wp;
                SetTextColor(dc, panel.foreground);
                SetBkColor(dc, panel.field);
                return (LRESULT)panel.searchBrush;
            }
            case WM_NOTIFY: {
                auto head = (NMHDR*)lp;
                if (panel.tips && head->hwndFrom == panel.tips &&
                    head->code == TTN_GETDISPINFOW) {
                    auto& tip = *reinterpret_cast<NMTTDISPINFOW*>(lp);
                    auto button = reinterpret_cast<HWND>(head->idFrom);
                    if (button == panel.add || button == panel.more) {
                        GetWindowTextW(button, tip.szText,
                                       ARRAYSIZE(tip.szText));
                        tip.lpszText = tip.szText;
                    }
                    return 0;
                }
                if (head->hwndFrom != panel.list)
                    break;
                if (head->code == LVN_GETDISPINFOW) {
                    auto& info = *(NMLVDISPINFOW*)lp;
                    if (info.item.mask & LVIF_IMAGE)
                        info.item.iImage =
                            appImage(firstItem() + info.item.iItem);
                    return 0;
                }
                break;
            }
            case WM_ERASEBKGND:
                return 1;
            case WM_PAINT: {
                PAINTSTRUCT ps{};
                HDC dc = BeginPaint(w, &ps);
                paintBackdrop(dc, w, ps.rcPaint);
                if (panel.choosing || panel.filtering) {
                    rounded(dc, panel.searchRect, panel.field, px(7));
                    RECT line = panel.searchRect;
                    line.left += px(7);
                    line.right -= px(7);
                    line.top = line.bottom - std::max(1, px(1));
                    auto old = SetDCBrushColor(dc, GetFocus() == panel.filter
                                                       ? panel.accent
                                                       : panel.rim);
                    FillRect(dc, &line, (HBRUSH)GetStockObject(DC_BRUSH));
                    SetDCBrushColor(dc, old);
                }
                EndPaint(w, &ps);
                return 0;
            }
            case WM_CTLCOLORSTATIC: {
                auto dc = reinterpret_cast<HDC>(wp);
                SetTextColor(dc, panel.foreground);
                SetBkMode(dc, TRANSPARENT);
                return (LRESULT)panel.background;
            }
            case WM_DRAWITEM:
                drawCommand(*reinterpret_cast<DRAWITEMSTRUCT*>(lp));
                return TRUE;
            case WM_ACTIVATE:
                if (panel.window != w)
                    break;
                if (LOWORD(wp) != WA_INACTIVE) {
                    stopPanelActivationCheck(w);
                    break;
                }
                if (!panel.browsing) {
                    POINT p{};
                    GetCursorPos(&p);
                    if (!(isIn(panel.anchor, p) && primaryMouseDown()))
                        PostMessageW(w, kCheckActivation, 0, 0);
                }
                break;
            case kCheckActivation:
                if (wp && panel.window == w && !panel.closing &&
                    shellFileWindow(GetForegroundWindow(), true))
                    if (SetForegroundWindow(w))
                        SetFocus(panel.apps.empty() ? panel.add : panel.list);
                checkPanelActivation(w, false);
                return 0;
            case kClosePanel:
            case WM_CLOSE:
                dismissPanel();
                return 0;
            case WM_KEYDOWN:
                if (wp == VK_ESCAPE) {
                    dismissPanel();
                    return 0;
                }
                break;
            case kOpenApp:
                if (GetKeyState(VK_CONTROL) & 0x8000)
                    return 0;
                if (!panel.incoming.token && !panel.closing &&
                    panel.rendered == revision &&
                    static_cast<unsigned>(lp) == panel.viewSerial &&
                    validIndex((int)wp, panel.apps.size())) {
                    auto a = panel.apps[wp];
                    if (panel.choosing) {
                        auto id = panel.folder;
                        if (edit([&](Model& m) { transfer(m, id, a); }) &&
                            panel.window) {
                            panel.choosing = false;
                            panel.page = 0;
                            refreshPanel();
                            auto f = std::find_if(
                                panel.apps.begin(), panel.apps.end(),
                                [&](auto& x) { return sameApp(a, x); });
                            if (f != panel.apps.end())
                                changePage(int(f - panel.apps.begin()) /
                                           panel.capacity);
                            gridEntrance(-1);
                        }
                        return 0;
                    }
                    if (preferOpenWindow) {
                        panel.pendingClick = a;
                        panel.clickSerial = panel.viewSerial;
                        requestWindowScan(true);
                    } else
                        openFromFolder(a, {});
                }
                return 0;
            case WM_COMMAND:
                if (panel.closing)
                    return 0;
                if (LOWORD(wp) == kTitle && HIWORD(wp) == STN_CLICKED) {
                    beginRename();
                    return 0;
                }
                if (LOWORD(wp) == kNameEdit && HIWORD(wp) == EN_KILLFOCUS) {
                    endRename(true, false);
                    return 0;
                }
                if (HIWORD(wp) == BN_CLICKED &&
                    (LOWORD(wp) == kPrev || LOWORD(wp) == kNext)) {
                    changePage(panel.page + (LOWORD(wp) == kNext ? 1 : -1));
                    return 0;
                }
                if (LOWORD(wp) == kFilter &&
                    (HIWORD(wp) == EN_SETFOCUS || HIWORD(wp) == EN_KILLFOCUS)) {
                    InvalidateRect(w, &panel.searchRect, FALSE);
                    return 0;
                }
                if (LOWORD(wp) == kFilter && HIWORD(wp) == EN_CHANGE &&
                    (panel.choosing || panel.filtering)) {
                    clearPanelSelection();
                    panel.page = 0;
                    panel.wheel = 0;
                    refreshPanel();
                    return 0;
                }
                if (LOWORD(wp) == kLens && HIWORD(wp) == BN_CLICKED) {
                    SetFocus(panel.filter);
                    return 0;
                }
                if (HIWORD(wp) == BN_CLICKED && LOWORD(wp) == kAdd) {
                    if (panel.choosing) {
                        if (browse(panel.folder, controller ? controller : w,
                                   panel.anchor)) {
                            panel.browsing = true;
                            ShowWindow(w, SW_HIDE);
                        }
                    } else
                        beginAdding(panel.folder);
                    return 0;
                }
                if (HIWORD(wp) == BN_CLICKED && LOWORD(wp) == kMore) {
                    if (panel.choosing) {
                        panel.choosing = false;
                        panel.page = 0;
                        refreshPanel();
                        gridEntrance(-1);
                        SetFocus(panel.apps.empty() ? panel.add : panel.list);
                    } else
                        manage(panel.folder);
                    return 0;
                }
                break;
            case WM_CONTEXTMENU:
                if (!panel.choosing &&
                    reinterpret_cast<HWND>(wp) == panel.list) {
                    POINT pt{GET_X_LPARAM(lp), GET_Y_LPARAM(lp)};
                    int i;
                    if (pt.x == -1 && pt.y == -1) {
                        i = selectedApp();
                        RECT icon{};
                        if (i < 0 ||
                            !ListView_GetItemRect(panel.list, i - firstItem(),
                                                  &icon, LVIR_ICON))
                            return 0;
                        pt = {(icon.left + icon.right) / 2, icon.bottom};
                        ClientToScreen(panel.list, &pt);
                    } else {
                        auto client = pt;
                        ScreenToClient(panel.list, &client);
                        i = hitApp(client);
                    }
                    if (validIndex(i, panel.apps.size()))
                        appMenu(panel.folder, panel.apps[i], &pt);
                    return 0;
                }
                break;
            case WM_DESTROY:
                panel.closing = true;
                panel.renaming = false;
                panel.transfer.reset();
                panel.incoming = {};
                stopPanelActivationCheck(w);
                KillTimer(w, kRunningTimer);
                if (auto hook = std::exchange(panel.windowHook, nullptr))
                    UnhookWinEvent(hook);
                if (std::exchange(panel.dropRegistered, false)) {
                    RevokeDragDrop(w);
                    OleUninitialize();
                }
                finishDrag();
                stopPanelFrames();
                if (panel.frameWait) {
                    CloseThreadpoolWait(panel.frameWait);
                    panel.frameWait = nullptr;
                }
                if (panel.frameTimer) {
                    CloseHandle(panel.frameTimer);
                    panel.frameTimer = nullptr;
                }
                releasePreviousPage();
                for (const auto& icon : panel.glyphs)
                    if (icon.handle)
                        DestroyIcon(icon.handle);
                if (panel.paintRegion)
                    DeleteObject(panel.paintRegion);
                if (panel.buffer) {
                    if (panel.stockBitmap)
                        SelectObject(panel.buffer, panel.stockBitmap);
                    if (panel.bitmap)
                        DeleteObject(panel.bitmap);
                    DeleteDC(panel.buffer);
                }
                if (panel.searchBrush)
                    DeleteObject(panel.searchBrush);
                if (panel.list && IsWindow(panel.list))
                    ListView_SetImageList(panel.list, nullptr, LVSIL_NORMAL);
                if (panel.icons) {
                    ImageList_Destroy(panel.icons);
                    panel.icons = nullptr;
                }
                return 0;
            case WM_NCDESTROY:
                if (panel.paperFont)
                    DeleteObject(panel.paperFont);
                if (panel.background)
                    DeleteObject(panel.background);
                if (panel.normalFont)
                    DeleteObject(panel.normalFont);
                if (panel.titleFont)
                    DeleteObject(panel.titleFont);
                panel = FolderPanel{};
                break;
        }
    } catch (...) {
        Wh_Log(L"Folder panel error: %08X", winrt::to_hresult());
    }
    return DefWindowProcW(w, msg, wp, lp);
}
static void openPanel(const wstring& id,
                      RECT anchor,
                      RECT bar,
                      HWND owner,
                      bool dark,
                      bool forDrop = false) {
    if (stopping)
        return;
    if (panel.window) {
        if (panel.folder == id) {
            if (panel.closing)
                revealPanel();
            else if (!forDrop)
                dismissPanel();
            return;
        }
        closePanel();
    }
    if (!lookup(groups, id))
        return;
    WNDCLASSW wc{};
    wc.lpfnWndProc = panelProc;
    wc.hInstance = moduleInstance();
    wc.lpszClassName = kPanelClass;
    wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    if (!RegisterClassW(&wc) && GetLastError() != ERROR_CLASS_ALREADY_EXISTS)
        return;
    INITCOMMONCONTROLSEX init{sizeof(init), ICC_LISTVIEW_CLASSES};
    if (!InitCommonControlsEx(&init))
        return;
    panel.window =
        CreateWindowExW(WS_EX_TOOLWINDOW | WS_EX_TOPMOST, kPanelClass, L"",
                        WS_POPUP | WS_CLIPCHILDREN, anchor.left, anchor.top, 1,
                        1, owner, nullptr, moduleInstance(), nullptr);
    if (!panel.window)
        return;
    panel.anchor = anchor;
    panel.bar = bar;
    panel.folder = id;
    panel.dpi = GetDpiForWindow(owner ? owner : panel.window);
    if (!panel.dpi)
        panel.dpi = 96;
    panel.systemDark = dark;
    restorePanelStyle();
    if (!panel.appearanceReady) {
        closePanel();
        return;
    }
    panel.motion = allowMotion();
    panel.reveal.set(1);
    panel.normalFont =
        CreateFontW(-px(14), 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET, 0, 0,
                    CLEARTYPE_QUALITY, 0, L"Segoe UI");
    panel.titleFont =
        CreateFontW(-px(16), 0, 0, 0, FW_SEMIBOLD, 0, 0, 0, DEFAULT_CHARSET, 0,
                    0, CLEARTYPE_QUALITY, 0, L"Segoe UI");
    panel.paperFont =
        CreateFontW(-px(17), 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET, 0, 0,
                    CLEARTYPE_QUALITY, 0, L"Georgia");
    panel.title = part(L"STATIC", L"", SS_OWNERDRAW | SS_NOTIFY, kTitle);
    panel.more = part(L"BUTTON", L"⋯", WS_TABSTOP | BS_OWNERDRAW, kMore);
    panel.empty = part(L"STATIC", uiText(UiString::Empty),
                       SS_CENTER | SS_CENTERIMAGE | SS_NOPREFIX, 0);
    panel.list =
        part(WC_LISTVIEWW, uiText(UiString::Apps),
             WS_TABSTOP | LVS_ICON | LVS_NOSCROLL | LVS_NOLABELWRAP |
                 LVS_SINGLESEL | LVS_SHOWSELALWAYS | LVS_SHAREIMAGELISTS,
             kApps);
    panel.add = part(L"BUTTON", uiText(UiString::AddAppsMenu),
                     WS_TABSTOP | BS_OWNERDRAW, kAdd);
    if (!panel.list || !panel.title || !panel.add || !panel.more ||
        !panel.empty) {
        closePanel();
        return;
    }
    panel.tips =
        CreateWindowExW(WS_EX_TOPMOST, TOOLTIPS_CLASSW, nullptr,
                        WS_POPUP | TTS_ALWAYSTIP | TTS_NOPREFIX, 0, 0, 0, 0,
                        panel.window, nullptr, moduleInstance(), nullptr);
    if (panel.tips)
        for (HWND button : {panel.add, panel.more}) {
            TOOLINFOW ti{sizeof(ti)};
            ti.uFlags = TTF_IDISHWND | TTF_SUBCLASS;
            ti.hwnd = panel.window;
            ti.uId = (UINT_PTR)button;
            ti.lpszText = LPSTR_TEXTCALLBACKW;
            SendMessageW(panel.tips, TTM_ADDTOOLW, 0, (LPARAM)&ti);
        }
    for (auto h : {panel.more, panel.add, panel.list})
        if (!wirePanelChild(h)) {
            closePanel();
            return;
        }
    SendMessageW(panel.empty, WM_SETFONT, (WPARAM)panel.normalFont, TRUE);
    SendMessageW(panel.title, WM_SETFONT, (WPARAM)panel.titleFont, TRUE);
    ListView_SetExtendedListViewStyle(panel.list, LVS_EX_DOUBLEBUFFER);
    ListView_SetBkColor(panel.list, panel.surface);
    ListView_SetTextBkColor(panel.list, CLR_NONE);
    ListView_SetTextColor(panel.list, panel.foreground);
    panel.windowHook =
        SetWinEventHook(EVENT_OBJECT_DESTROY, EVENT_OBJECT_HIDE, nullptr,
                        runningWindowEvent, 0, 0, WINEVENT_OUTOFCONTEXT);
    refreshPanel();
    registerPanelDrop(panel.window);
    panel.reveal.set(panel.motion ? 0 : 1);
    revealPanel();
    gridEntrance(0);
    ShowWindow(panel.window, forDrop ? SW_SHOWNOACTIVATE : SW_SHOW);
    if (!forDrop) {
        SetForegroundWindow(panel.window);
        SetFocus(panel.apps.empty() ? panel.add : panel.list);
    }
}
static bool anchorScreenRect(HWND owner,
                             FrameworkElement const& anchor,
                             RECT& rect) {
    if (auto root = anchor ? anchor.XamlRoot() : nullptr) {
        auto host = root.Content().try_as<FrameworkElement>();
        if (host) {
            auto size = anchor.ActualSize();
            auto box = anchor.TransformToVisual(host).TransformBounds(
                {0, 0, size.x, size.y});
            double scale = root.RasterizationScale();
            HWND bridge = FindWindowExW(
                owner, nullptr,
                L"Windows.UI.Composition.DesktopWindowContentBridge", nullptr);
            POINT start{};
            if (ClientToScreen(bridge ? bridge : owner, &start)) {
                rect = {start.x + lround(box.X * scale),
                        start.y + lround(box.Y * scale),
                        start.x + lround((box.X + box.Width) * scale),
                        start.y + lround((box.Y + box.Height) * scale)};
                return !IsRectEmpty(&rect);
            }
        }
    }
    return false;
}
static void displayFolder(const wstring& dest,
                          FrameworkElement const& anchor,
                          bool forDrop) {
    if (!isPrimaryRoot(ui.frame) || (anchor && !isPrimaryRoot(anchor)))
        return;
    HWND owner = ui.primary;
    RECT bar{};
    if (!GetWindowRect(owner, &bar))
        return;
    LONG middle = bar.left + (bar.right - bar.left) / 2;
    RECT rect{middle - 1, bar.top, middle + 1, bar.bottom};
    anchorScreenRect(owner, anchor, rect);
    openPanel(dest, rect, bar, owner,
              !anchor || anchor.ActualTheme() == X::ElementTheme::Dark,
              forDrop);
}
static void showFolder(const wstring& dest, FrameworkElement const& anchor) {
    displayFolder(dest, anchor, false);
}
struct IconHover {
    wstring folder;
    FrameworkElement anchor{nullptr};
    Drops storage{nullptr};
    std::vector<App> apps;
    uint64_t token{};
    bool leaving = false, opened = false, ready = false, native = false,
         fileTarget = false;
};
[[clang::no_destroy]] static IconHover iconHover;
static void resetIconHover(bool committed = false) {
    if (controller)
        KillTimer(controller, kIconHoverTimer);
    auto hover = std::exchange(iconHover, IconHover{});
    if (!committed)
        clearIncoming(hover.token);
    if (!committed && hover.opened && panel.window &&
        panel.folder == hover.folder && !panel.dropHover)
        PostMessageW(panel.window, kCheckActivation, 0, 0);
}
static void leaveIcon(const wstring& folder) {
    if (iconHover.folder != folder)
        return;
    if (!iconHover.opened && !iconHover.token) {
        resetIconHover();
        return;
    }
    iconHover.leaving = true;
    // Bridge the small gap between the taskbar button and the popup's OLE
    // target.
    if (!SetTimer(controller, kIconHoverTimer, 140, nullptr))
        resetIconHover();
}
static void previewIconDrop() {
    KillTimer(controller, kIconHoverTimer);
    if (iconHover.leaving) {
        resetIconHover();
        return;
    }
    if (iconHover.ready || iconHover.folder.empty() || !iconHover.anchor)
        return;
    if (!iconHover.native &&
        (!iconHover.fileTarget || iconHover.apps.empty())) {
        if (!iconHover.storage)
            return;
        auto status = iconHover.storage.Status();
        if (status == F::AsyncStatus::Started) {
            if (!SetTimer(controller, kIconHoverTimer, 55, nullptr))
                resetIconHover();
            return;
        }
        if (status != F::AsyncStatus::Completed)
            return;
        for (auto item : iconHover.storage.GetResults()) {
            if (item.IsOfType(W::Storage::StorageItemTypes::File))
                if (auto app = droppedShortcut(wstring(item.Path())))
                    iconHover.apps.push_back(std::move(*app));
        }
    } else if (iconHover.native &&
               (ui.drag.app.empty() || !primaryMouseDown())) {
        resetIconHover();
        return;
    }
    iconHover.ready = true;
    if (iconHover.apps.empty())
        return;
    iconHover.opened = !panel.window || panel.folder != iconHover.folder;
    displayFolder(iconHover.folder, iconHover.anchor, true);
    if (panel.window && panel.folder == iconHover.folder) {
        POINT screen{};
        GetCursorPos(&screen);
        iconHover.token = beginIncoming(iconHover.apps, screen, true);
        if (iconHover.native) {
            ui.drag.previewFolder = iconHover.folder;
            ui.drag.token = iconHover.token;
        }
    }
}
static void hoverShortcut(const wstring& folder,
                          FrameworkElement const& anchor,
                          const Data::DataPackageView& view) {
    if (iconHover.folder != folder || iconHover.native) {
        resetIconHover();
        iconHover.folder = folder;
        iconHover.anchor = anchor;
        iconHover.storage = view.GetStorageItemsAsync();
        if (!SetTimer(controller, kIconHoverTimer, 330, nullptr))
            resetIconHover();
    } else if (iconHover.leaving) {
        iconHover.leaving = false;
        KillTimer(controller, kIconHoverTimer);
    }
}
// A folder has its own OLE drop window for this file drag. When the pointer
// crosses onto it Windows ends the previous taskbar target's insertion preview.
static constexpr wchar_t kFileSurfaceClass[] = L"TaskbarFolders.FileDrop";
struct FileDropWindow {
    HWND window;
    wstring folder;
};
[[clang::no_destroy]] static std::vector<FileDropWindow> fileWindows;
[[clang::no_destroy]] static Data::DataPackageView filePackage{nullptr};
static double fileLayoutAfter = 0, fileReleasedAt = 0;
static bool fileDropFinished = false;
static void finishFileGesture() {
    // Release the OLE windows on the next message, after Drop returns to
    // Windows.
    if (fileDragActive && !std::exchange(fileDropFinished, true) && controller)
        PostMessageW(controller, WM_TIMER, kFileSurfaceTimer, 0);
}
static void resetFileWindows() {
    if (controller)
        KillTimer(controller, kFileSurfaceTimer);
    auto old = std::exchange(fileWindows, {});
    filePackage = nullptr;
    fileDropFinished = false;
    fileReleasedAt = 0;
    fileDataIdentity = nullptr;
    std::vector<App>{}.swap(fileDataApps);
    for (const auto& entry : old) {
        RevokeDragDrop(entry.window);
        DestroyWindow(entry.window);
        OleUninitialize();
    }
    if (std::exchange(fileDragActive, false)) {
        fileLayoutAfter = motionTime() + 140;
        requestLayout();
    }
}
static DWORD fileEffect(DWORD offered) {
    if (offered & DROPEFFECT_COPY)
        return DROPEFFECT_COPY;
    return offered & DROPEFFECT_LINK;
}
struct IconFileTarget
    : winrt::implements<IconFileTarget, IDropTarget, winrt::non_agile> {
    wstring folder;
    FrameworkElement anchor{nullptr};
    bool entered = false;
    IconFileTarget(const wstring& id, FrameworkElement const& tile)
        : folder(id), anchor(tile) {}
    bool current() const {
        return entered && !stopping && iconHover.fileTarget &&
               iconHover.folder == folder;
    }
    HRESULT STDMETHODCALLTYPE DragEnter(IDataObject* data,
                                        DWORD,
                                        POINTL,
                                        DWORD* effect) noexcept final {
        if (!effect)
            return E_POINTER;
        DWORD allowed = fileEffect(*effect);
        *effect = DROPEFFECT_NONE;
        try {
            if (stopping || !allowed || !lookup(groups, folder))
                return S_OK;
            auto items = PanelDropTarget::readItems(data);
            Drops storage{nullptr};
            if (items.empty() && filePackage) {
                auto format = PanelDropTarget::fileFormat();
                if (!data || FAILED(data->QueryGetData(&format)))
                    storage = filePackage.GetStorageItemsAsync();
            }
            if (items.empty() && !storage)
                return S_OK;
            resetIconHover();
            iconHover.folder = folder;
            iconHover.anchor = anchor;
            iconHover.fileTarget = true;
            iconHover.apps = std::move(items);
            iconHover.storage = std::move(storage);
            entered = true;
            SetTimer(controller, kIconHoverTimer, 330, nullptr);
            *effect = allowed;
        } catch (...) {
            DragLeave();
        }
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE DragOver(DWORD,
                                       POINTL,
                                       DWORD* effect) noexcept final {
        if (!effect)
            return E_POINTER;
        *effect = current() ? fileEffect(*effect) : DROPEFFECT_NONE;
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE DragLeave() noexcept final {
        try {
            if (current())
                leaveIcon(folder);
        } catch (...) {
        }
        if (entered && fileDragActive && !primaryMouseDown() && controller)
            PostMessageW(controller, WM_TIMER, kFileSurfaceTimer, 0);
        entered = false;
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE Drop(IDataObject*,
                                   DWORD,
                                   POINTL,
                                   DWORD* effect) noexcept final {
        if (!effect)
            return E_POINTER;
        DWORD allowed = fileEffect(*effect);
        *effect = DROPEFFECT_NONE;
        try {
            if (current() && allowed) {
                DropJob job{folder};
                job.before = incomingAnchor(iconHover.token);
                job.previewToken = iconHover.token;
                if (!iconHover.apps.empty())
                    job.apps = std::move(iconHover.apps);
                else
                    job.storage = iconHover.storage;
                bool queued = (!job.apps.empty() || job.storage) &&
                              queueDrop(std::move(job));
                resetIconHover(queued);
                if (queued)
                    *effect = allowed;
            } else if (current())
                resetIconHover();
        } catch (...) {
            DragLeave();
        }
        entered = false;
        finishFileGesture();
        return S_OK;
    }
};
static bool registerFileWindow(HWND owner,
                               const RECT& r,
                               const wstring& id,
                               FrameworkElement const& element) {
    if (std::any_of(fileWindows.begin(), fileWindows.end(),
                    [&](const auto& v) { return v.folder == id; }))
        return true;
    if (IsRectEmpty(&r))
        return false;
    auto target = winrt::make_self<IconFileTarget>(id, element);
    WNDCLASSW wc{};
    wc.hInstance = moduleInstance();
    wc.lpszClassName = kFileSurfaceClass;
    wc.lpfnWndProc = DefWindowProcW;
    if (!RegisterClassW(&wc) && GetLastError() != ERROR_CLASS_ALREADY_EXISTS)
        return false;
    FileDropWindow entry{nullptr, id};
    if (FAILED(OleInitialize(nullptr)))
        return false;
    auto w = CreateWindowExW(
        WS_EX_NOACTIVATE | WS_EX_TOOLWINDOW | WS_EX_TOPMOST | WS_EX_LAYERED,
        kFileSurfaceClass, L"", WS_POPUP, r.left, r.top, r.right - r.left,
        r.bottom - r.top, owner, nullptr, wc.hInstance, nullptr);
    if (!w || !SetLayeredWindowAttributes(w, 0, 1, LWA_ALPHA) ||
        FAILED(RegisterDragDrop(w, target.get()))) {
        if (w)
            DestroyWindow(w);
        OleUninitialize();
        return false;
    }
    entry.window = w;
    fileWindows.push_back(std::move(entry));
    ShowWindow(w, SW_SHOWNOACTIVATE);
    return true;
}
static void prepareFileWindows(const Data::DataPackageView& view);
static void checkFileWindows() {
    if (!fileDragActive)
        return;
    if (fileDropFinished) {
        resetFileWindows();
        return;
    }
    POINT p{};
    RECT r{};
    auto bar = tray();
    if (GetCursorPos(&p) && GetWindowRect(bar, &r) && isIn(r, p)) {
        if (primaryMouseDown()) {
            fileReleasedAt = 0;
            return;
        }
        // A timer may run after the physical release but before OLE dispatches
        // Drop.
        if (iconHover.fileTarget && !iconHover.leaving) {
            auto now = motionTime();
            if (!fileReleasedAt)
                fileReleasedAt = now;
            if (now - fileReleasedAt < 350)
                return;
        }
    }
    if (iconHover.fileTarget && !iconHover.leaving)
        leaveIcon(iconHover.folder);
    resetFileWindows();
}
static void shortcutDrop(const wstring& folder,
                         FrameworkElement const& anchor,
                         X::DragEventArgs const& e,
                         bool received) {
    if (stopping || !lookup(groups, folder))
        return;
    using Operation = Data::DataPackageOperation;
    auto allowed = e.AllowedOperations();
    auto operation = Operation::None;
    if ((allowed & Operation::Copy) != Operation::None)
        operation = Operation::Copy;
    else if ((allowed & Operation::Link) != Operation::None)
        operation = Operation::Link;
    if (operation == Operation::None || !controller || !ui.drag.app.empty())
        return;
    auto view = e.DataView();
    if (!view.Contains(Data::StandardDataFormats::StorageItems()))
        return;
    if (!received)
        prepareFileWindows(view);
    e.Handled(true);
    {
        if (!received) {
            if (!iconHover.fileTarget)
                hoverShortcut(folder, anchor, view);
        } else {
            DropJob job{folder};
            if (iconHover.folder == folder && !iconHover.native) {
                if (iconHover.ready || !iconHover.apps.empty())
                    job.apps = std::move(iconHover.apps);
                else
                    job.storage = iconHover.storage;
                job.before = incomingAnchor(iconHover.token);
                job.previewToken = iconHover.token;
            } else
                job.storage = view.GetStorageItemsAsync();
            bool queued =
                (job.storage || !job.apps.empty()) && queueDrop(std::move(job));
            resetIconHover(queued);
            if (!queued)
                operation = Operation::None;
        }
    }
    e.AcceptedOperation(operation);
    if (received)
        finishFileGesture();
}
static bool suppressFolderClick(const wstring& id);
struct TaskbarFolderMenu {
    C::MenuFlyout flyout{nullptr};
    winrt::event_token closed{};
    std::vector<std::function<void()>> handlers;
};
[[clang::no_destroy]] static TaskbarFolderMenu folderMenu;
static void closeTaskbarFolderMenu(bool hide = true) {
    auto old = std::exchange(folderMenu, TaskbarFolderMenu{});
    if (!old.flyout)
        return;
    if (old.closed.value)
        try {
            old.flyout.Closed(old.closed);
        } catch (...) {
        }
    off(old.handlers);
    if (hide)
        try {
            old.flyout.Hide();
        } catch (...) {
        }
}
static void fillTaskbarFolderMenu(TaskbarFolderMenu& menu,
                                  const AppFolder& folder,
                                  bool offerPin) {
    auto items = menu.flyout.Items();
    C::MenuFlyoutSubItem more{nullptr};
    size_t apps = 0;
    for (auto& action : taskbarFolderActions(folder, offerPin)) {
        auto rows = items;
        if (action.kind == FolderAction::Launch) {
            if (!apps)
                items.Append(C::MenuFlyoutSeparator());
            if (++apps > 5) {
                if (!more) {
                    more = C::MenuFlyoutSubItem();
                    more.Text(uiText(UiString::MoreApps));
                    more.Icon(C::SymbolIcon(C::Symbol::AllApps));
                    items.Append(more);
                }
                rows = more.Items();
            }
        } else if (action.kind == FolderAction::Add ||
                   action.kind == FolderAction::Remove)
            items.Append(C::MenuFlyoutSeparator());
        C::MenuFlyoutItem row;
        row.Text(action.label);
        row.Icon(C::SymbolIcon(menuSymbol(action.kind)));
        auto token = row.Click([action = std::move(action)](auto&&, auto&&) {
            try {
                if (!stopping)
                    runFolderAction(action);
            } catch (...) {
                Wh_Log(L"Folder menu action failed.");
            }
        });
        menu.handlers.push_back([w = winrt::make_weak(row), token] {
            if (auto row = w.get())
                row.Click(token);
        });
        rows.Append(row);
    }
}
static bool showTaskbarFolderMenu(const wstring& id,
                                  FrameworkElement const& anchor) {
    auto* folder = lookup(groups, id);
    if (!folder || !anchor || !anchor.XamlRoot() || stopping)
        return false;
    try {
        closeTaskbarFolderMenu();
        auto flyout = folderMenu.flyout = C::MenuFlyout();
        flyout.ShouldConstrainToRootBounds(false);
        flyout.Placement(C::Primitives::FlyoutPlacementMode::Top);
        folderMenu.closed = flyout.Closed([](auto const& sender, auto&&) {
            try {
                if (sender == folderMenu.flyout)
                    closeTaskbarFolderMenu(false);
            } catch (...) {
            }
        });
        fillTaskbarFolderMenu(folderMenu, *folder, !pinnedFolder(id));
        flyout.ShowAt(anchor);
        return true;
    } catch (...) {
        closeTaskbarFolderMenu();
        return false;
    }
}
static C::ControlTemplate folderButtonTemplate() {
    if (!ui.tileTemplate) {
        ui.tileTemplate = X::Markup::XamlReader::Load(
                              LR"(
<ControlTemplate xmlns="http://schemas.microsoft.com/winfx/2006/xaml/presentation"
                 xmlns:x="http://schemas.microsoft.com/winfx/2006/xaml" TargetType="Button">
  <Grid Background="Transparent">
    <VisualStateManager.VisualStateGroups>
      <VisualStateGroup x:Name="CommonStates">
        <VisualState x:Name="Normal"/>
        <VisualState x:Name="PointerOver">
          <VisualState.Setters>
            <Setter Target="FolderPlate.Background" Value="{ThemeResource ButtonBackgroundPointerOver}"/>
            <Setter Target="FolderPlate.BorderBrush" Value="{ThemeResource ButtonBorderBrushPointerOver}"/>
          </VisualState.Setters>
        </VisualState>
        <VisualState x:Name="Pressed">
          <VisualState.Setters>
            <Setter Target="FolderPlate.Background" Value="{ThemeResource ButtonBackgroundPressed}"/>
            <Setter Target="FolderPlate.BorderBrush" Value="{ThemeResource ButtonBorderBrushPressed}"/>
          </VisualState.Setters>
        </VisualState>
        <VisualState x:Name="Disabled"/>
      </VisualStateGroup>
    </VisualStateManager.VisualStateGroups>
    <Border x:Name="FolderPlate" Margin="2" CornerRadius="4" BorderThickness="1"
            IsHitTestVisible="False"/>
    <ContentPresenter Content="{TemplateBinding Content}" HorizontalAlignment="Center"
                      VerticalAlignment="Center" IsHitTestVisible="False"/>
  </Grid>
</ControlTemplate>)")
                              .as<C::ControlTemplate>();
    }
    return ui.tileTemplate;
}
static Button makeTile(const AppFolder& f) {
    Button b;
    b.Width(40);
    b.Height(38);
    b.Padding({3, 3, 3, 3});
    b.BorderThickness({0, 0, 0, 0});
    b.Background(M::SolidColorBrush(W::UI::Color{0, 0, 0, 0}));
    try {
        b.Template(folderButtonTemplate());
    } catch (...) {
        b.CornerRadius({4, 4, 4, 4});
    }
    b.UseSystemFocusVisuals(true);
    X::Automation::AutomationProperties::SetName(b, f.name);
    C::ToolTipService::SetToolTip(b, winrt::box_value(hstring(f.name)));
    C::Image mosaic;
    mosaic.Width(30);
    mosaic.Height(30);
    mosaic.IsHitTestVisible(false);
    mosaic.Source(previewFor(f).image);
    b.Content(mosaic);
    auto id = f.id;
    click(b, ui.tileOff, [id, w = winrt::make_weak(b)]() {
        if (auto v = w.get(); v && !suppressFolderClick(id))
            showFolder(id, v);
    });
    auto context =
        b.ContextRequested([id](X::UIElement const& sender,
                                I::ContextRequestedEventArgs const& args) {
            try {
                if (!args.Handled() &&
                    showTaskbarFolderMenu(id, sender.as<FrameworkElement>()))
                    args.Handled(true);
            } catch (...) {
            }
        });
    ui.tileOff.push_back([w = winrt::make_weak(b), context] {
        if (auto v = w.get())
            v.ContextRequested(context);
    });
    b.AllowDrop(true);
    auto hover = [id](F::IInspectable const& sender,
                      X::DragEventArgs const& e) {
        try {
            shortcutDrop(id, sender.as<FrameworkElement>(), e, false);
        } catch (...) {
        }
    };
    auto enter = b.DragEnter(hover);
    auto over = b.DragOver(hover);
    auto leave =
        b.DragLeave([id](F::IInspectable const&, X::DragEventArgs const&) {
            try {
                if (!iconHover.fileTarget)
                    leaveIcon(id);
            } catch (...) {
            }
        });
    auto drop =
        b.Drop([id](F::IInspectable const& sender, X::DragEventArgs const& e) {
            try {
                shortcutDrop(id, sender.as<FrameworkElement>(), e, true);
            } catch (...) {
            }
        });
    ui.tileOff.push_back([w = winrt::make_weak(b), enter, over, leave, drop]() {
        if (auto v = w.get()) {
            v.DragEnter(enter);
            v.DragOver(over);
            v.DragLeave(leave);
            v.Drop(drop);
        }
    });
    return b;
}

static void detach(C::Panel const& panel, X::UIElement const& child) {
    if (!panel || !child)
        return;
    uint32_t i = 0;
    if (panel.Children().IndexOf(child, i))
        panel.Children().RemoveAt(i);
}
static void restore(Hidden& h) {
    auto e = h.item.get();
    if (!e || e.Visibility() != X::Visibility::Collapsed)
        return;
    if (h.binding)
        e.SetBinding(X::UIElement::VisibilityProperty(), h.binding);
    else if (!h.old || h.old == X::DependencyProperty::UnsetValue())
        e.ClearValue(X::UIElement::VisibilityProperty());
    else
        e.SetValue(X::UIElement::VisibilityProperty(), h.old);
}
static void resetNative();
static void clearFolderPlacement();
static void clearView() {
    closeTaskbarFolderMenu();
    resetFileWindows();
    resetIconHover();
    resetNative();
    clearFolderPlacement();
    if (controller) {
        KillTimer(controller, kLayoutTimer);
        KillTimer(controller, kDropTimer);
        KillTimer(controller, kNativeDropTimer);
    }
    off(ui.frameOff);
    closePanel();
    off(ui.tileOff);
    off(ui.menuOff);
    for (auto& h : ui.hidden)
        try {
            restore(h);
        } catch (...) {
        }
    if (ui.marginSet && ui.repeater)
        ui.repeater.Margin(ui.baseMargin);
    detach(ui.root, ui.overlay);
    detach(ui.root, ui.strip);
    ui = View{};
    previews.clear();
}
struct NativeButton {
    FrameworkElement element;
    wstring id;
};
[[clang::no_destroy]] static std::vector<NativeButton> buttons;
static bool pinnedFolder(const wstring& id) {
    auto key = pinId(id);
    if (key.empty())
        return false;
    return std::any_of(buttons.begin(), buttons.end(),
                       [&](const auto& b) { return equal(key, b.id); });
}

static void removeFolderPin(wstring dest) {
    auto f = lookup(groups, dest);
    if (!f)
        return;
    const auto old = *f;
    if (validFolderId(dest) &&
        (pinnedFolder(dest) || !ownLinks(pinnedLinks(), pinId(dest)).empty())) {
        note(uiText(UiString::UnpinFirst));
        return;
    }
    if (!edit([&](Model& m) {
            std::erase_if(m, [&](const AppFolder& g) { return g.id == dest; });
        }))
        return;
    if (validFolderId(dest))
        cleanPinFiles(old, true);
    pinStamps.erase(dest);
}
[[clang::no_destroy]] static std::vector<std::function<void()>> dropOff;
[[clang::no_destroy]] static std::vector<NativeButton> dropHosts;
static void prepareFileWindows(const Data::DataPackageView& view) {
    if (fileDragActive || !controller || stopping || !ui.drag.app.empty() ||
        ui.tiles.empty())
        return;
    RECT bar{};
    auto owner = tray();
    if (!owner || !GetWindowRect(owner, &bar))
        return;
    fileWindows.reserve(ui.tiles.size() + dropHosts.size());
    if (!SetTimer(controller, kFileSurfaceTimer, 75, nullptr))
        return;
    fileDragActive = true;
    filePackage = view;
    auto add = [&](const wstring& id, FrameworkElement const& e) {
        RECT r{}, clipped{};
        if (e && e.Visibility() == X::Visibility::Visible &&
            anchorScreenRect(owner, e, r) && IntersectRect(&clipped, &r, &bar))
            registerFileWindow(owner, clipped, id, e);
    };
    for (const auto& t : ui.tiles)
        add(t.id, t.button);
    for (const auto& b : dropHosts)
        if (auto f = folderForPin(b.id))
            add(f->id, b.element);
}
struct LiveIcon {
    FrameworkElement host{nullptr};
    C::Image glyph{nullptr};
    wstring id;
    M::ImageSource drawn{nullptr};
    F::IInspectable oldLocal{nullptr};
    X::Data::BindingBase binding{nullptr};
    int64_t token = 0;
};
[[clang::no_destroy]] static std::vector<LiveIcon> liveIcons;
static C::Image pinGlyph(const FrameworkElement& host) {
    C::Image exact{nullptr}, inPanel{nullptr};
    int candidates = 0;
    visit(host, [&](const FrameworkElement& e) {
        if (auto img = e.try_as<C::Image>()) {
            if (e.Name() == L"Icon") {
                exact = img;
                return false;
            }
            auto parent =
                M::VisualTreeHelper::GetParent(e).try_as<FrameworkElement>();
            if (parent && parent.Name() == L"IconPanel") {
                ++candidates;
                inPanel = img;
            }
        }
        return !exact;
    });
    return exact ? exact : (candidates == 1 ? inPanel : nullptr);
}
static void restoreGlyph(LiveIcon& live) {
    auto dp = C::Image::SourceProperty();
    live.glyph.UnregisterPropertyChangedCallback(dp, live.token);
    if (live.glyph.Source() != live.drawn)
        return;
    if (live.binding)
        live.glyph.SetBinding(dp, live.binding);
    else if (live.oldLocal == X::DependencyProperty::UnsetValue())
        live.glyph.ClearValue(dp);
    else
        live.glyph.SetValue(dp, live.oldLocal);
}
static void syncGlyphs(const std::vector<NativeButton>& hosts) {
    struct Target {
        const NativeButton* b;
        C::Image glyph;
    };
    std::vector<Target> targets;
    for (const auto& b : hosts)
        if (auto g = pinGlyph(b.element))
            targets.push_back({&b, g});
    for (auto it = liveIcons.begin(); it != liveIcons.end();) {
        if (std::any_of(targets.begin(), targets.end(), [&](const Target& t) {
                return t.glyph == it->glyph && equal(t.b->id, it->id);
            })) {
            ++it;
            continue;
        }
        restoreGlyph(*it);
        it = liveIcons.erase(it);
    }
    for (const auto& t : targets) {
        auto f = folderForPin(t.b->id);
        if (!f)
            continue;
        auto it =
            std::find_if(liveIcons.begin(), liveIcons.end(),
                         [&](const LiveIcon& l) { return l.glyph == t.glyph; });
        auto dp = C::Image::SourceProperty();
        bool fresh = it == liveIcons.end();
        if (fresh) {
            LiveIcon state;
            state.host = t.b->element;
            state.glyph = t.glyph;
            state.id = t.b->id;
            state.token = t.glyph.RegisterPropertyChangedCallback(
                dp, [](auto&&, auto&&) { requestLayout(); });
            liveIcons.push_back(std::move(state));
            it = std::prev(liveIcons.end());
        }
        auto wanted = previewFor(*f).image;
        auto shown = t.glyph.Source();
        if (fresh || shown != it->drawn) {
            auto expression = t.glyph.GetBindingExpression(dp);
            it->binding = expression ? expression.ParentBinding() : nullptr;
            it->oldLocal = t.glyph.ReadLocalValue(dp);
        }
        if (shown != wanted) {
            t.glyph.Source(wanted);
        }
        it->drawn = wanted;
    }
}
// The taskbar can keep the same XAML button and tooltip after its .lnk is
// renamed. Keep the native tooltip container (placement, theme and open/close
// animations).
struct PinCaption {
    FrameworkElement host{nullptr}, owner{nullptr}, target{nullptr};
    C::ToolTip tip{nullptr};
    X::DependencyProperty property{nullptr};
    wstring id, name;
    bool accessible = false;
    F::IInspectable original{nullptr}, written{nullptr};
    X::Data::BindingBase binding{nullptr};
    int64_t changed{}, replaced{};
    winrt::event_token opened{};
};
struct CaptionTarget {
    FrameworkElement host, owner;
    C::ToolTip tip;
    wstring id;
    bool accessible = false;
};
[[clang::no_destroy]] static std::vector<PinCaption> pinCaptions;
static bool ownsCaption(const PinCaption& c, const F::IInspectable& value) {
    if (!c.written)
        return false;
    if (c.written == value)
        return true;
    // A string-typed dependency property can rebox its effective value on each
    // read.
    auto text = value.try_as<F::IReference<hstring>>();
    return c.accessible && text && text.Value() == c.name;
}
static void applyCaption(PinCaption& c, const wstring& name) {
    auto current = c.target.GetValue(c.property);
    bool ours = ownsCaption(c, current);
    if (ours && name == c.name)
        return;
    if (!ours) {
        auto expr = c.target.GetBindingExpression(c.property);
        c.binding = expr ? expr.ParentBinding() : nullptr;
        c.original = c.target.ReadLocalValue(c.property);
        c.written = nullptr;
        if (auto str = current.try_as<F::IReference<hstring>>();
            str && str.Value() == name)
            return;
    }
    c.name = name;
    c.written = winrt::box_value(hstring(name));
    c.target.SetValue(c.property, c.written);
}
static void restoreCaption(PinCaption& c) {
    c.target.UnregisterPropertyChangedCallback(c.property, c.changed);
    if (c.tip) {
        c.tip.Opened(c.opened);
        c.owner.UnregisterPropertyChangedCallback(
            C::ToolTipService::ToolTipProperty(), c.replaced);
    }
    if (!ownsCaption(c, c.target.GetValue(c.property)))
        return;
    if (c.binding)
        c.target.SetBinding(c.property, c.binding);
    else if (c.original == X::DependencyProperty::UnsetValue())
        c.target.ClearValue(c.property);
    else
        c.target.SetValue(c.property, c.original);
}
static void syncPinCaptions(const std::vector<NativeButton>& hosts) {
    std::vector<CaptionTarget> targets;
    for (const auto& b : hosts) {
        if (!folderForPin(b.id))
            continue;
        size_t before = targets.size();
        visit(b.element, [&](const FrameworkElement& owner) {
            auto value = C::ToolTipService::GetToolTip(owner);
            if (value) {
                // If the template acquired an inner tooltip, retire our
                // synthetic root label.
                bool synthetic = std::any_of(
                    pinCaptions.begin(), pinCaptions.end(), [&](const auto& c) {
                        return !c.accessible && c.owner == owner && !c.tip &&
                               c.written == value &&
                               (!c.original ||
                                c.original ==
                                    X::DependencyProperty::UnsetValue());
                    });
                if (!synthetic)
                    targets.push_back(
                        {b.element, owner, value.try_as<C::ToolTip>(), b.id});
            }
            return true;
        });
        if (targets.size() == before)
            targets.push_back({b.element, b.element, nullptr, b.id});
        targets.push_back({b.element, b.element, nullptr, b.id, true});
    }
    auto matches = [](const PinCaption& c, const CaptionTarget& t) {
        return c.host == t.host && c.owner == t.owner && c.tip == t.tip &&
               c.accessible == t.accessible && equal(c.id, t.id);
    };
    for (auto i = pinCaptions.begin(); i != pinCaptions.end();) {
        if (std::any_of(targets.begin(), targets.end(),
                        [&](const auto& t) { return matches(*i, t); })) {
            ++i;
            continue;
        }
        restoreCaption(*i);
        i = pinCaptions.erase(i);
    }
    for (auto& t : targets) {
        auto f = folderForPin(t.id);
        if (!f)
            continue;
        auto i = std::find_if(pinCaptions.begin(), pinCaptions.end(),
                              [&](const auto& c) { return matches(c, t); });
        if (i == pinCaptions.end()) {
            PinCaption c;
            c.host = t.host;
            c.owner = t.owner;
            c.tip = t.tip;
            c.id = t.id;
            c.accessible = t.accessible;
            c.target = t.tip ? t.tip.as<FrameworkElement>() : t.owner;
            if (t.accessible)
                c.property =
                    X::Automation::AutomationProperties::NameProperty();
            else
                c.property = t.tip ? C::ContentControl::ContentProperty()
                                   : C::ToolTipService::ToolTipProperty();
            auto changed = [](auto&&, auto&&) { requestLayout(); };
            c.changed =
                c.target.RegisterPropertyChangedCallback(c.property, changed);
            if (t.tip) {
                c.replaced = t.owner.RegisterPropertyChangedCallback(
                    C::ToolTipService::ToolTipProperty(), changed);
                c.opened =
                    t.tip.Opened([](const F::IInspectable& sender, auto&&) {
                        if (stopping)
                            return;
                        try {
                            for (auto& entry : pinCaptions)
                                if (entry.tip == sender &&
                                    equal(appKey(entry.host), entry.id)) {
                                    if (auto folder = folderForPin(entry.id))
                                        applyCaption(entry, folder->name);
                                }
                        } catch (...) {
                        }
                    });
            }
            pinCaptions.push_back(std::move(c));
            i = std::prev(pinCaptions.end());
        }
        applyCaption(*i, f->name);
    }
}

static void resetNative() {
    syncPinCaptions({});
    syncGlyphs({});
    off(dropOff);
    dropHosts.clear();
    buttons.clear();
}
static void setNativeDrops(const std::vector<NativeButton>& hosts) {
    if (hosts.size() == dropHosts.size() &&
        std::equal(hosts.begin(), hosts.end(), dropHosts.begin(),
                   [](const auto& a, const auto& b) {
                       return a.element == b.element && a.id == b.id;
                   }))
        return;
    off(dropOff);
    dropHosts = hosts;
    for (const auto& b : hosts) {
        auto e = b.element;
        auto f = folderForPin(b.id);
        if (!f)
            continue;
        auto old = e.ReadLocalValue(X::UIElement::AllowDropProperty());
        auto over = winrt::box_value(
            X::DragEventHandler([id = f->id](F::IInspectable const& sender,
                                             X::DragEventArgs const& e) {
                try {
                    shortcutDrop(id, sender.as<FrameworkElement>(), e, false);
                } catch (...) {
                }
            }));
        auto leave = winrt::box_value(X::DragEventHandler(
            [id = f->id](F::IInspectable const&, X::DragEventArgs const&) {
                try {
                    if (!iconHover.fileTarget)
                        leaveIcon(id);
                } catch (...) {
                }
            }));
        auto drop = winrt::box_value(
            X::DragEventHandler([id = f->id](F::IInspectable const& sender,
                                             X::DragEventArgs const& e) {
                try {
                    shortcutDrop(id, sender.as<FrameworkElement>(), e, true);
                } catch (...) {
                }
            }));
        e.AllowDrop(true);
        e.AddHandler(X::UIElement::DragEnterEvent(), over, true);
        e.AddHandler(X::UIElement::DragOverEvent(), over, true);
        e.AddHandler(X::UIElement::DragLeaveEvent(), leave, true);
        e.AddHandler(X::UIElement::DropEvent(), drop, true);
        dropOff.push_back([e, old, over, leave, drop] {
            e.RemoveHandler(X::UIElement::DragEnterEvent(), over);
            e.RemoveHandler(X::UIElement::DragOverEvent(), over);
            e.RemoveHandler(X::UIElement::DragLeaveEvent(), leave);
            e.RemoveHandler(X::UIElement::DropEvent(), drop);
            if (e.AllowDrop()) {
                if (old == X::DependencyProperty::UnsetValue())
                    e.ClearValue(X::UIElement::AllowDropProperty());
                else
                    e.SetValue(X::UIElement::AllowDropProperty(), old);
            }
        });
    }
}
// A process can have a taskbar button without being pinned (for example a
// one-off file picker). Keep ordinary executable windows out of suggestions
// unless a pinned shortcut actually points there.
static std::set<wstring> pinnedExecutables(const fs::path& directory) {
    std::set<wstring> paths;
    std::error_code ec;
    for (fs::directory_iterator i(directory, ec), end; !ec && i != end;
         i.increment(ec)) {
        if (!equal(i->path().extension().wstring(), L".lnk"))
            continue;
        com_ptr<IShellLinkW> l;
        if (FAILED(CoCreateInstance(CLSID_ShellLink, nullptr,
                                    CLSCTX_INPROC_SERVER,
                                    IID_PPV_ARGS(l.put()))) ||
            FAILED(l.as<IPersistFile>()->Load(i->path().c_str(), STGM_READ)))
            continue;
        wchar_t target[MAX_PATH]{};
        if (SUCCEEDED(l->GetPath(target, ARRAYSIZE(target), nullptr, 0)) &&
            *target)
            paths.insert(target);
    }
    return paths;
}
static bool suggestWindow(const App& app,
                          std::optional<std::set<wstring>>& pins,
                          const fs::path& directory) {
    fs::path dest(app.target);
    bool rawExe =
        dest.is_absolute() && equal(dest.extension().wstring(), L".exe");
    bool rawLabel = equal(fs::path(app.name).extension().wstring(), L".exe");
    if (!rawExe && !rawLabel)
        return true;
    if (!rawExe) {
        if (auto item = shellItem(app.target))
            dest = display(item.get(), SIGDN_FILESYSPATH);
    }
    if (!pins)
        pins = pinnedExecutables(directory);
    auto target = dest.lexically_normal().wstring();
    return std::any_of(pins->begin(), pins->end(),
                       [&](const wstring& p) { return equal(p, target); });
}
static std::vector<App> taskbarChoices(const AppFolder& folder) {
    std::vector<App> choices;
    auto add = [&](const App& app) {
        auto matches = [&](const App& x) { return sameApp(x, app); };
        if (std::none_of(folder.apps.begin(), folder.apps.end(), matches) &&
            std::none_of(choices.begin(), choices.end(), matches))
            choices.push_back(app);
    };
    // Resolve the current buttons when the user opens Add, even if a layout
    // update is pending.
    std::set<wstring> seen;
    std::optional<std::set<wstring>> binaries;
    const auto pinDirectory = pinnedLinks();
    if (ui.repeater)
        visit(ui.repeater, [&](FrameworkElement const& e) {
            if (winrt::get_class_name(e) != L"Taskbar.TaskListButton")
                return true;
            auto id = appKey(e);
            if (!id.empty() && !prefix(id, kPinPrefix) &&
                seen.insert(id).second)
                if (auto app = byId(id);
                    app && suggestWindow(*app, binaries, pinDirectory))
                    add(*app);
            return false;
        });
    if (choices.empty()) {
        // Pinned classic shortcuts are useful when an Explorer layout does not
        // expose app IDs.
        PWSTR roaming = nullptr;
        if (SUCCEEDED(SHGetKnownFolderPath(FOLDERID_RoamingAppData, 0, nullptr,
                                           &roaming))) {
            fs::path pins = fs::path(roaming) /
                            L"Microsoft\\Internet Explorer\\Quick Launch\\User "
                            L"Pinned\\TaskBar";
            CoTaskMemFree(roaming);
            std::error_code ec;
            for (fs::directory_iterator it(pins, ec), end; !ec && it != end;
                 it.increment(ec)) {
                if (equal(it->path().extension().wstring(), L".lnk"))
                    if (auto app = fromPath(it->path().wstring()))
                        add(*app);
            }
        }
    }
    return choices;
}
static bool under(FrameworkElement const& e, POINT pt) {
    if (!e || !ui.root || e.Visibility() == X::Visibility::Collapsed)
        return false;
    RECT rect{};
    return e.XamlRoot() == ui.root.XamlRoot() &&
           anchorScreenRect(tray(), e, rect) && isIn(rect, pt);
}
static Button projectedButton(const wstring& id);
static wstring dropFolderAt(POINT point, FrameworkElement* anchor) {
    for (const auto& b : buttons)
        if (auto folder = folderForPin(b.id);
            folder && !projectedButton(folder->id) && under(b.element, point)) {
            if (anchor)
                *anchor = b.element;
            return folder->id;
        }
    for (const auto& tile : ui.tiles)
        if (M::VisualTreeHelper::GetParent(tile.button) &&
            under(tile.button, point)) {
            if (anchor)
                *anchor = tile.button;
            return tile.id;
        }
    return {};
}
static wstring dropFolderAt(POINT point) {
    return dropFolderAt(point, nullptr);
}
static void cancelTaskbarPreview() {
    clearIncoming(ui.drag.token);
    ui.drag = {};
    if (iconHover.native)
        resetIconHover();
}
static void previewTaskbarApp(POINT screen) {
    auto& drag = ui.drag;
    HWND window = GetAncestor(WindowFromPoint(screen), GA_ROOT);
    bool inPanel = window && window == panel.window && !panel.choosing &&
                   !panel.browsing && !panel.closing;
    FrameworkElement anchor{nullptr};
    auto folder = inPanel            ? panel.folder
                  : window == tray() ? dropFolderAt(screen, &anchor)
                                     : wstring{};
    if (iconHover.native && iconHover.folder != folder)
        resetIconHover();
    if (drag.previewFolder != folder) {
        clearIncoming(std::exchange(drag.token, 0));
        drag.previewFolder.clear();
    }
    if (folder.empty())
        return;
    if (!drag.resolvedOnce) {
        drag.resolved = byId(drag.app);
        drag.resolvedOnce = true;
    }
    if (!drag.resolved)
        return;
    if (inPanel) {
        if (drag.previewFolder != folder ||
            (drag.token && drag.token != panel.incoming.token)) {
            drag.previewFolder = folder;
            drag.token = beginIncoming({*drag.resolved}, screen);
        }
        if (drag.token == panel.incoming.token)
            positionIncoming(screen);
        return;
    }
    if (!iconHover.native || iconHover.folder != folder) {
        resetIconHover();
        iconHover.folder = folder;
        iconHover.anchor = anchor;
        iconHover.apps = {*drag.resolved};
        iconHover.native = true;
        if (!SetTimer(controller, kIconHoverTimer, 330, nullptr))
            resetIconHover();
    }
}
static void trackPeer(POINT screen) {
    wstring dest;
    if (isIn(panel.bar, screen))
        dest = dropFolderAt(screen);
    if (dest == panel.folder)
        dest.clear();
    if (dest == panel.hoverFolder)
        return;
    KillTimer(panel.list, kPeerTimer);
    panel.hoverFolder = dest;
    if (!dest.empty())
        SetTimer(panel.list, kPeerTimer, 330, nullptr);
}
static void enterPeer(const wstring& dest, POINT screen) {
    if (!lookup(groups, dest) || dest == panel.folder ||
        GetCapture() != panel.list || panel.rendered != revision)
        return;
    if (!panel.transfer) {
        if (!panel.dragging || !validIndex(panel.pressed, panel.apps.size()))
            return;
        if (panel.dragApps.empty())
            collectDragApps(panel.pressed);
        panel.transfer = FolderPanel::TransferDrag{
            panel.folder, panel.dragApps, panel.anchor,
            panel.filtering ? panel.searchOrigin : panel.page};
    }
    auto floating = std::move(panel.dragImage);
    finishDrag();
    panel.filtering = false;
    clearPanelSelection();
    panel.incoming = {};
    panel.transfer->token = 0;
    panel.folder = dest;
    panel.anchor = {screen.x - px(18), panel.bar.top, screen.x + px(18),
                    panel.bar.bottom};
    panel.page = 0;
    refreshPanel();
    if (panel.window && panel.transfer && dest != panel.transfer->source)
        panel.transfer->token =
            beginIncoming(panel.transfer->apps, screen, true);
    if (panel.window && panel.transfer) {
        panel.dragImage = std::move(floating);
        moveDragImage(screen);
    }
}
static void endPeer(std::optional<POINT> drop) {
    if (!panel.transfer)
        return;
    auto origin = *panel.transfer;
    auto dest = panel.folder;
    auto anchor = incomingAnchor(origin.token);
    if (drop) {
        auto hit = WindowFromPoint(*drop);
        auto root = GetAncestor(hit, GA_ROOT);
        if (root == panel.window) {
            positionIncoming(*drop);
            anchor = incomingAnchor(origin.token);
        } else if (root == tray()) {
            dest = dropFolderAt(*drop);
            if (dest != panel.folder)
                anchor.reset();
        } else if (fileSurface(hit, root) != FileSurface::None) {
            dest.clear();
            anchor.reset();
        } else
            drop.reset();
    }
    auto floating = std::move(panel.dragImage);
    panel.transfer.reset();
    finishDrag();
    if (drop && !dest.empty() && dest != origin.source) {
        bool saved = editMove([&](Model& m) {
            moveFolderMembers(m, origin.source, dest, origin.apps, anchor);
        });
        clearIncoming(origin.token, saved);
        syncPanel();
        return;
    }
    panel.incoming = {};
    panel.folder = origin.source;
    panel.anchor = origin.anchor;
    panel.page = origin.page;
    refreshPanel();
    if (panel.window && origin.apps.size() > 1)
        for (const auto& app : origin.apps)
            panel.selection.insert(app.target);
    if (drop && dest.empty() && panel.window) {
        panel.dragImage = std::move(floating);
        panel.dragApps = origin.apps;
        auto it = std::find_if(
            panel.apps.begin(), panel.apps.end(),
            [&](const App& a) { return sameApp(a, origin.apps.front()); });
        if (it != panel.apps.end())
            moveAppOutside(int(it - panel.apps.begin()), *drop);
        else
            finishDrag();
    }
}
struct ShortcutExport {
    wstring source;
    FileDestination target;
    POINT release{};
    HWND owner{};
    std::vector<App> apps;
    std::vector<fs::path> files;
    PTP_WORK work{};
    ~ShortcutExport() {
        if (work)
            CloseThreadpoolWork(work);
    }
};
[[clang::no_destroy]] static std::vector<std::unique_ptr<ShortcutExport>>
    exports;
static void CALLBACK writeShortcuts(PTP_CALLBACK_INSTANCE,
                                    void* context,
                                    PTP_WORK) noexcept {
    auto& job = *static_cast<ShortcutExport*>(context);
    auto apartment = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    if (SUCCEEDED(apartment) || apartment == RPC_E_CHANGED_MODE) {
        for (size_t i = 0; i < job.apps.size(); ++i) {
            try {
                job.files[i] = placeShortcut(job.apps[i], job.target.directory);
            } catch (...) {
            }
        }
    }
    if (SUCCEEDED(apartment))
        CoUninitialize();
    PostMessageW(job.owner, kExportReady, reinterpret_cast<WPARAM>(&job), 0);
}
static bool startExport(const wstring& source,
                        FileDestination dest,
                        std::vector<App> apps,
                        POINT release) {
    if (!controller || stopping || apps.empty() || dest.directory.empty())
        return false;
    auto job = std::make_unique<ShortcutExport>();
    job->source = source;
    job->target = std::move(dest);
    job->release = release;
    job->owner = controller;
    job->apps = std::move(apps);
    job->files.resize(job->apps.size());
    job->work = CreateThreadpoolWork(writeShortcuts, job.get(), nullptr);
    if (!job->work)
        return false;
    exports.push_back(std::move(job));
    SubmitThreadpoolWork(exports.back()->work);
    return true;
}
static void finishExport(ShortcutExport* result, bool reportFailure = true) {
    auto it =
        std::find_if(exports.begin(), exports.end(),
                     [&](const auto& entry) { return entry.get() == result; });
    if (it == exports.end())
        return;
    auto job = std::move(*it);
    exports.erase(it);
    WaitForThreadpoolWorkCallbacks(job->work, FALSE);
    std::map<wstring, const App*, IgnoreCase> successful;
    if (!stopping)
        revealDesktopShortcuts(job->target, job->files, job->release);
    size_t failures = 0;
    for (size_t i = 0; i < job->apps.size(); ++i) {
        if (job->files[i].empty()) {
            ++failures;
            continue;
        }
        successful.emplace(job->apps[i].target, &job->apps[i]);
    }
    // Shell calls above may dispatch messages that edit the open folder.
    std::vector<App> placed;
    if (auto source = lookup(groups, job->source))
        for (const auto& app : source->apps) {
            auto match = successful.find(app.target);
            if (match != successful.end() && app == *match->second)
                placed.push_back(app);
        }
    if (!placed.empty()) {
        bool visible = !stopping && panel.window &&
                       panel.folder == job->source && !panel.dragging &&
                       !panel.transfer;
        auto poses = visible ? visiblePoses() : decltype(visiblePoses()){};
        int page = panel.page;
        if (editMove([&](Model& m) {
                moveFolderMembers(m, job->source, L"", placed);
            }) &&
            visible) {
            for (const auto& a : placed)
                panel.selection.erase(a.target);
            syncPanel();
            if (panel.window && panel.folder == job->source) {
                if (panel.page != page && pageItems())
                    gridEntrance(-1);
                else
                    settleTiles(poses);
            }
        }
    }
    if (failures && reportFailure && !stopping)
        note(uiText(UiString::ExportFailed));
}
static void moveAppOutside(int index, POINT screenPoint) {
    if (panel.rendered != revision || !validIndex(index, panel.apps.size())) {
        finishDrag();
        return;
    }
    if (panel.dragApps.empty())
        collectDragApps(index);
    auto apps = panel.dragApps;
    auto from = panel.folder;
    HWND hit = WindowFromPoint(screenPoint), target = GetAncestor(hit, GA_ROOT);
    wstring to;
    FileDestination files;
    if (target && target == tray())
        to = dropFolderAt(screenPoint);
    else {
        auto window = panel.window;
        auto serial = revision;
        int pressed = panel.pressed;
        files = dropDirectory(hit, target, screenPoint);
        if (panel.window != window || panel.folder != from ||
            revision != serial || panel.pressed != pressed) {
            if (panel.window == window && panel.folder == from &&
                panel.pressed == pressed)
                finishDrag(true);
            return;
        }
        if (files.directory.empty()) {
            finishDrag(true);
            return;
        }
    }
    if (from == to) {
        finishDrag(true);
        return;
    }
    if (!files.directory.empty()) {
        if (!startExport(from, std::move(files), std::move(apps),
                         screenPoint)) {
            finishDrag(true);
            note(uiText(UiString::ExportFailed));
            return;
        }
        auto image = std::move(panel.dragImage);
        finishDrag();
        startExit(std::move(image), screenPoint);
        return;
    }
    auto poses = visiblePoses();
    auto icon = std::move(panel.dragImage);
    if (!icon && panel.motion) {
        const int half = panel.iconSize / 2;
        icon =
            makeDragIcon(index, {screenPoint.x - half, screenPoint.y - half});
    }
    int oldPage = panel.page;
    finishDrag();
    if (!editMove([&](Model& m) { moveFolderMembers(m, from, to, apps); }) ||
        !panel.window || panel.folder != from)
        return;

    clearPanelSelection();
    startExit(std::move(icon), screenPoint);
    syncPanel();
    if (!panel.window)
        return;
    if (panel.page != oldPage && pageItems())
        gridEntrance(-1);
    else
        settleTiles(poses);
}
static void reparent(X::UIElement const& b, C::Panel const& p) {
    auto old = M::VisualTreeHelper::GetParent(b).try_as<C::Panel>();
    if (old == p)
        return;
    detach(old, b);
    p.Children().Append(b);
}
// The shell and other taskbar extensions animate the real button. Share its
// transform so both visuals sample exactly the same animation at composition
// time.
static void followNativeMotion(Tile& tile, FrameworkElement const& host) {
    if (tile.motionHost == host)
        return;
    auto follow = [&](X::DependencyProperty const& property, PCWSTR path) {
        if (!host) {
            tile.button.ClearValue(property);
            return;
        }
        X::Data::Binding binding;
        binding.Source(host);
        binding.Path(X::PropertyPath(path));
        binding.Mode(X::Data::BindingMode::OneWay);
        tile.button.SetBinding(property, binding);
    };
    follow(X::UIElement::RenderTransformProperty(), L"RenderTransform");
    follow(X::UIElement::RenderTransformOriginProperty(),
           L"RenderTransformOrigin");
    follow(FrameworkElement::FlowDirectionProperty(), L"FlowDirection");
    tile.motionHost = host;
}
static F::Rect layoutBoundsIn(FrameworkElement const& element,
                              X::UIElement const& relative) {
    auto size = element.ActualSize();
    if (element == relative)
        return {0, 0, size.x, size.y};
    auto parent = M::VisualTreeHelper::GetParent(element).as<X::UIElement>();
    auto offset = element.ActualOffset();
    return parent.TransformToVisual(relative).TransformBounds(
        {offset.x, offset.y, size.x, size.y});
}
static void matchTaskbarPlate(Tile& tile, FrameworkElement sample) {
    if (!ui.tileTemplate || tile.button.Template() != ui.tileTemplate)
        return;
    if (!tile.plate) {
        tile.button.ApplyTemplate();
        visit(tile.button, [&](FrameworkElement const& e) {
            if (e.Name() == L"FolderPlate")
                tile.plate = e.try_as<C::Border>();
            return !tile.plate;
        });
        if (!tile.plate)
            return;
    }
    if (!sample && !buttons.empty())
        sample = buttons.front().element;
    bool changed = tile.plateHost != sample;
    if (changed) {
        tile.plateHost = sample;
        tile.nativePlate = nullptr;
        tile.plateSearched = false;
    }
    if (tile.nativePlate && sample.IsLoaded() && !tile.nativePlate.IsLoaded()) {
        tile.nativePlate = nullptr;
        tile.plateSearched = false;
    }
    if (sample && !tile.plateSearched &&
        M::VisualTreeHelper::GetChildrenCount(sample) > 0) {
        visit(sample, [&](FrameworkElement const& e) {
            if (e.Name() == L"BackgroundElement")
                tile.nativePlate = e;
            return !tile.nativePlate;
        });
        tile.plateSearched = bool(tile.nativePlate) || sample.IsLoaded();
    }
    X::Thickness inset{2, 2, 2, 2}, stroke{1, 1, 1, 1};
    X::CornerRadius corners{4, 4, 4, 4};
    if (auto plate = tile.nativePlate;
        plate && plate.ActualWidth() > 0 && plate.ActualHeight() > 0) {
        auto bounds = layoutBoundsIn(plate, sample);
        inset = {
            std::max(0.f, bounds.X), std::max(0.f, bounds.Y),
            std::max(0., sample.ActualWidth() - bounds.X - bounds.Width),
            std::max(0., sample.ActualHeight() - bounds.Y - bounds.Height)};
        if (auto border = plate.try_as<C::Border>()) {
            corners = border.CornerRadius();
            stroke = border.BorderThickness();
        }
    }
    if (tile.plate.Margin() != inset)
        tile.plate.Margin(inset);
    if (tile.plate.CornerRadius() != corners)
        tile.plate.CornerRadius(corners);
    if (tile.plate.BorderThickness() != stroke)
        tile.plate.BorderThickness(stroke);
    auto theme = tile.button.ActualTheme();
    if (!changed && tile.plateTheme == theme)
        return;
    tile.plateTheme = theme;
    const std::pair<PCWSTR, PCWSTR> brushes[] = {
        {L"ButtonBackgroundPointerOver",
         L"ShellTaskbarItemFillColorSecondaryBrush"},
        {L"ButtonBackgroundPressed", L"ShellTaskbarItemFillColorTertiaryBrush"},
        {L"ButtonBorderBrushPointerOver",
         L"ShellTaskbarItemGradientStrokeColorSecondaryBrush"},
        {L"ButtonBorderBrushPressed",
         L"ShellTaskbarItemStrokeColorTertiaryBrush"}};
    auto resources = tile.button.Resources();
    for (const auto& [target, source] : brushes) {
        auto key = winrt::box_value(hstring(source)),
             local = winrt::box_value(hstring(target));
        M::Brush found{nullptr};
        for (auto e = sample ? sample : ui.root.try_as<FrameworkElement>();
             e && !found;
             e = M::VisualTreeHelper::GetParent(e).try_as<FrameworkElement>()) {
            auto dictionary = e.Resources();
            if (dictionary.HasKey(key))
                found = dictionary.Lookup(key).try_as<M::Brush>();
        }
        if (!found)
            if (auto app = X::Application::Current();
                app && app.Resources().HasKey(key))
                found = app.Resources().Lookup(key).try_as<M::Brush>();
        if (found)
            resources.Insert(local, found);
        else if (resources.HasKey(local))
            resources.Remove(local);
    }
}
// Use the native button state even when the folder flyout is closed.
static HRESULT(WINAPI* getButtonRunning)(void*, bool*) = nullptr;
static HRESULT(WINAPI* setButtonRunning)(void*, bool) = nullptr;
static HRESULT WINAPI onButtonRunning(void* button, bool value) {
    HRESULT result = setButtonRunning(button, value);
    if (SUCCEEDED(result) && !stopping)
        if (auto host = published.load())
            PostMessageW(host, kButtonRunningChanged, 0, 0);
    return result;
}
// This method belongs to ITaskListButton, which can have a different COM
// address from the object's IUnknown or its public XAML interface.
static constexpr winrt::guid taskListButtonIid{
    0x6d8e265b,
    0x363c,
    0x5067,
    {0x93, 0x85, 0x11, 0xd9, 0x4d, 0xff, 0xbe, 0x04}};
static bool canGroupClosedButton(FrameworkElement const& button) {
    if (!getButtonRunning)
        return false;
    winrt::com_ptr<::IUnknown> taskButton;
    if (FAILED(winrt::get_unknown(button)->QueryInterface(
            taskListButtonIid, taskButton.put_void())))
        return false;
    bool running = true;
    return SUCCEEDED(getButtonRunning(taskButton.get(), &running)) && !running;
}
struct PlacedButton {
    FrameworkElement element{nullptr};
    X::Thickness base{}, applied{};
    F::IInspectable local{nullptr};
    X::Data::BindingBase binding{nullptr};
};
[[clang::no_destroy]] static std::map<void*, PlacedButton> placedButtons;
[[clang::no_destroy]] static C::Border folderDivider{nullptr};
struct ProjectedTile {
    Button button{nullptr};
    C::Canvas layer{nullptr};
    M::TranslateTransform translation{nullptr};
    F::Rect slot{};
    Slide x;
};
struct FolderGesture {
    wstring id;
    uint32_t pointer{};
    F::Point origin{}, point{};
    bool moved{}, mouse{};
    double missedRelease{};
};
struct DividerWatch {
    X::DependencyObject object;
    X::DependencyProperty property;
    int64_t token;
};
namespace Comp = W::UI::Composition;
// Keep the slot geometry in layout coordinates. TransformToVisual across the
// two branches can include a compositor animation that the overlay already
// follows.
struct RowSpace {
    float x{}, y{};
    bool mirrored{};
    F::Rect map(F::Rect r) const {
        r.X = x + (mirrored ? -r.X - r.Width : r.X);
        r.Y += y;
        return r;
    }
};
static RowSpace rowLayoutSpace() {
    RowSpace space;
    auto mirrors = [](FrameworkElement const& child,
                      FrameworkElement const& parent) {
        if (child.FlowDirection() == parent.FlowDirection())
            return false;
        auto map = child.TransformToVisual(parent);
        return map.TransformPoint({1, 0}).X < map.TransformPoint({0, 0}).X;
    };
    auto el = ui.repeater;
    while (el && el != ui.root) {
        auto parent =
            M::VisualTreeHelper::GetParent(el).try_as<FrameworkElement>();
        if (!parent)
            break;
        auto offset = el.ActualOffset();
        if (mirrors(el, parent)) {
            space.x = float(el.ActualWidth()) - space.x;
            space.mirrored = !space.mirrored;
        }
        space.x += offset.x;
        space.y += offset.y;
        el = parent;
    }
    auto offset = ui.overlay.ActualOffset();
    space.x -= offset.x;
    space.y -= offset.y;
    if (mirrors(ui.overlay, ui.root)) {
        space.x = float(ui.overlay.ActualWidth()) - space.x;
        space.mirrored = !space.mirrored;
    }
    return space;
}
struct ShellOffset {
    FrameworkElement element{nullptr};
    hstring key;
    F::Numerics::float3 base{}, layout{};
};
struct ShellMotion {
    FrameworkElement anchor{nullptr};
    Comp::Visual overlay{nullptr};
    Comp::CompositionPropertySet bases{nullptr};
    std::vector<ShellOffset> offsets;
    M::TransformGroup sharedMoves{nullptr};
};
struct StartProjection {
    FolderPlacement side = FolderPlacement::Normal;
    FrameworkElement start{nullptr};
    std::map<wstring, ProjectedTile> tiles;
    std::vector<wstring> order;
    M::TranslateTransform line{nullptr};
    ShellMotion shell;
    FrameworkElement watchedStart{nullptr}, watchedFolder{nullptr};
    std::vector<DividerWatch> dividerWatches;
    bool rebindDivider{}, dividerChanged{};
    F::Rect startLayout{};
    RowSpace rowSpace{};
    double watchUntil{}, invalidSince{}, releaseAfter{};
    bool motion{}, rendering{}, pointerDown{};
    winrt::event_token frame{};
    FolderGesture gesture;
    wstring ignoreClick;
    double ignoreUntil{};
};
[[clang::no_destroy]] static StartProjection projection;
static void stopShellMotion() {
    auto old = std::exchange(projection.shell, ShellMotion{});
    try {
        if (old.sharedMoves && ui.overlay &&
            ui.overlay.RenderTransform() == old.sharedMoves)
            ui.overlay.ClearValue(X::UIElement::RenderTransformProperty());
    } catch (...) {
    }
    if (old.overlay)
        try {
            old.overlay.StopAnimation(L"Translation");
            old.overlay.Properties().InsertVector3(L"Translation", {});
            X::Hosting::ElementCompositionPreview::SetIsTranslationEnabled(
                ui.overlay, false);
        } catch (...) {
        }
}
// Use the same XAML translation objects for the native row and the overlay.
// TransformToVisual can still describe the preceding frame inside a property
// callback.
static bool isRowTranslation(M::Transform const& transform) {
    if (transform.try_as<M::TranslateTransform>())
        return true;
    auto group = transform.try_as<M::TransformGroup>();
    if (!group)
        return false;
    for (auto const& child : group.Children())
        if (!isRowTranslation(child))
            return false;
    return true;
}
static void bindRowTranslations() {
    auto& moves = projection.shell.sharedMoves;
    if (moves && ui.overlay.RenderTransform() != moves) {
        moves = nullptr;
        return;
    }
    if (!moves) {
        auto local =
            ui.overlay.ReadLocalValue(X::UIElement::RenderTransformProperty());
        if (local && local != X::DependencyProperty::UnsetValue())
            return;
    }
    std::vector<M::Transform> sources;
    auto parent = M::VisualTreeHelper::GetParent(projection.start)
                      .try_as<FrameworkElement>();
    while (parent && parent != ui.root) {
        auto move = parent.RenderTransform();
        if (move && isRowTranslation(move))
            sources.push_back(move);
        parent =
            M::VisualTreeHelper::GetParent(parent).try_as<FrameworkElement>();
    }
    if (sources.empty()) {
        if (moves) {
            ui.overlay.ClearValue(X::UIElement::RenderTransformProperty());
            moves = nullptr;
        }
        return;
    }
    M::TransformGroup group;
    for (auto& source : sources)
        group.Children().Append(source);
    ui.overlay.RenderTransform(group);
    moves = group;
}
// The shell can move its visuals on the compositor before or after layout
// reaches the UI thread. Apply only that difference to our own overlay.
static void syncShellOffsets() {
    if (!projection.start)
        return;
    projection.rowSpace = rowLayoutSpace();
    auto& shell = projection.shell;
    for (auto& part : shell.offsets) {
        auto layout = part.element.ActualOffset();
        if (part.layout != layout) {
            auto target = part.base + (layout - part.layout);
            shell.bases.InsertVector3(part.key, target);
            part.base = target;
            part.layout = layout;
        }
    }
}
static void bindShellMotion() {
    if (projection.shell.anchor == projection.start)
        return;
    stopShellMotion();
    auto& shell = projection.shell;
    shell.anchor = projection.start;
    try {
        using Preview = X::Hosting::ElementCompositionPreview;
        auto output = Preview::GetElementVisual(ui.overlay);
        auto compositor = output.Compositor();
        auto expression = compositor.CreateExpressionAnimation();
        shell.bases = compositor.CreatePropertySet();
        expression.SetReferenceParameter(L"bases", shell.bases);
        wstring formula;
        // When the taskbar recenters, Explorer animates Offset on each button
        // with its own delay. Use Start's presented offset as well as the
        // row's. The folder's own Dock hover stays on its separate XAML
        // transform.
        auto current = projection.start;
        while (current && current != ui.root) {
            auto parent = M::VisualTreeHelper::GetParent(current)
                              .try_as<FrameworkElement>();
            if (!parent)
                return;
            auto visual = Preview::GetElementVisual(current);
            auto index = std::to_wstring(shell.offsets.size());
            hstring ref(L"v" + index), key(L"p" + index);
            const auto layout = current.ActualOffset();
            auto base = current == projection.start ? layout : visual.Offset();
            shell.bases.InsertVector3(key, base);
            expression.SetReferenceParameter(ref, visual);
            auto transform = parent.TransformToVisual(ui.overlay);
            const bool mirrored = transform.TransformPoint({1, 0}).X <
                                  transform.TransformPoint({0, 0}).X;
            if (!formula.empty())
                formula += L" + ";
            wstring delta =
                L"(" + wstring(ref) + L".Offset - bases." + wstring(key) + L")";
            if (current == projection.start)
                formula +=
                    wstring(mirrored ? L"Vector3(-1,0,0)" : L"Vector3(1,0,0)") +
                    L" * " + delta;
            else
                formula += mirrored ? L"Vector3(-1,1,1) * " + delta : delta;
            shell.offsets.push_back({current, key, base, layout});
            current = parent;
        }
        if (!current || formula.empty())
            return;
        expression.Expression(formula);
        shell.overlay = output;
        Preview::SetIsTranslationEnabled(ui.overlay, true);
        output.StartAnimation(L"Translation", expression);
        bindRowTranslations();
    } catch (...) {
        stopShellMotion();
        projection.shell.anchor = projection.start;
    }
}
static Button projectedButton(const wstring& id) {
    auto it = projection.tiles.find(id);
    return it != projection.tiles.end() ? it->second.button : Button{nullptr};
}
struct HeldProperty {
    FrameworkElement host;
    X::DependencyProperty property;
    F::IInspectable prior;
    X::Data::BindingBase binding{nullptr};
};
[[clang::no_destroy]] static std::vector<HeldProperty> proxyProperties;
static void restoreProxy(HeldProperty const& p) {
    auto value = p.host.ReadLocalValue(p.property);
    if (!value || value == X::DependencyProperty::UnsetValue())
        return;
    if (p.property == X::UIElement::OpacityProperty()) {
        if (winrt::unbox_value<double>(value) != 0)
            return;
    } else if (p.property == X::Automation::AutomationProperties::
                                 AccessibilityViewProperty()) {
        if (winrt::unbox_value<X::Automation::Peers::AccessibilityView>(
                value) != X::Automation::Peers::AccessibilityView::Raw)
            return;
    } else if (winrt::unbox_value<bool>(value))
        return;
    if (p.binding)
        p.host.SetBinding(p.property, p.binding);
    else if (p.prior == X::DependencyProperty::UnsetValue())
        p.host.ClearValue(p.property);
    else
        p.host.SetValue(p.property, p.prior);
}
static void syncProxies(const std::map<wstring, FrameworkElement>& hosts) {
    for (auto i = proxyProperties.begin(); i != proxyProperties.end();) {
        if (std::any_of(hosts.begin(), hosts.end(),
                        [&](auto& entry) { return entry.second == i->host; })) {
            ++i;
            continue;
        }
        try {
            restoreProxy(*i);
        } catch (...) {
        }
        i = proxyProperties.erase(i);
    }
    for (const auto& [id, host] : hosts) {
        auto hold = [&](X::DependencyProperty const& property,
                        F::IInspectable const& value) {
            if (std::any_of(proxyProperties.begin(), proxyProperties.end(),
                            [&](auto& p) {
                                return p.host == host && p.property == property;
                            }))
                return;
            auto prior = host.ReadLocalValue(property);
            auto expr = host.GetBindingExpression(property);
            host.SetValue(property, value);
            proxyProperties.push_back(
                {host, property, prior, expr ? expr.ParentBinding() : nullptr});
        };
        hold(X::UIElement::OpacityProperty(), winrt::box_value(0.0));
        hold(X::UIElement::IsHitTestVisibleProperty(), winrt::box_value(false));
        if (host.try_as<C::Control>())
            hold(C::Control::IsTabStopProperty(), winrt::box_value(false));
        hold(X::Automation::AutomationProperties::AccessibilityViewProperty(),
             winrt::box_value(X::Automation::Peers::AccessibilityView::Raw));
    }
}
static void stopPlacementFrames() {
    if (!std::exchange(projection.rendering, false))
        return;
    try {
        M::CompositionTarget::Rendering(projection.frame);
    } catch (...) {
    }
}
static void unwatchDivider() {
    for (const auto& watch : projection.dividerWatches) {
        try {
            watch.object.UnregisterPropertyChangedCallback(watch.property,
                                                           watch.token);
        } catch (...) {
        }
    }
    projection.dividerWatches.clear();
    projection.watchedStart = nullptr;
    projection.watchedFolder = nullptr;
}
static bool suppressFolderClick(const wstring& id) {
    return (projection.gesture.id == id && projection.gesture.moved) ||
           (projection.ignoreClick == id &&
            motionTime() < projection.ignoreUntil);
}
static void endFolderGesture(bool commit);
static void clearProjectedTiles() {
    stopPlacementFrames();
    stopShellMotion();
    unwatchDivider();
    for (auto& [id, t] : projection.tiles) {
        try {
            if (t.layer) {
                detach(t.layer, t.button);
                detach(ui.overlay, t.layer);
            }
        } catch (...) {
        }
    }
    for (auto& tile : ui.tiles)
        try {
            followNativeMotion(tile, nullptr);
        } catch (...) {
        }
    projection = StartProjection{};
}
static void restorePosition(const PlacedButton& p) {
    if (p.element.Margin() != p.applied)
        return;
    auto property = FrameworkElement::MarginProperty();
    if (p.binding)
        p.element.SetBinding(property, p.binding);
    else if (!p.local || p.local == X::DependencyProperty::UnsetValue())
        p.element.ClearValue(property);
    else
        p.element.SetValue(property, p.local);
}
static void clearFolderPlacement() {
    if (!projection.start && projection.tiles.empty() &&
        placedButtons.empty() && proxyProperties.empty() && !folderDivider)
        return;
    clearProjectedTiles();
    syncProxies({});
    for (auto& [key, p] : placedButtons)
        try {
            restorePosition(p);
        } catch (...) {
        }
    placedButtons.clear();
    if (folderDivider) {
        detach(ui.overlay, folderDivider);
        folderDivider = nullptr;
    }
}
struct BarSlot {
    FrameworkElement element{nullptr};
    wstring folder;
    F::Rect rect{};
    double x{}, target{};
    bool start = false;
};
// Slot widths and existing gaps come from Windows. Only the order changes.
static bool orderFolderSlots(std::vector<BarSlot>& slots,
                             FolderPlacement side,
                             double& boundary) {
    if (side == FolderPlacement::Normal || slots.empty())
        return false;
    std::stable_sort(
        slots.begin(), slots.end(),
        [](const BarSlot& a, const BarSlot& b) { return a.x < b.x; });
    std::vector<size_t> folders, order;
    auto start = slots.size();
    for (size_t i = 0; i < slots.size(); ++i) {
        if (slots[i].start) {
            if (start != slots.size())
                return false;
            start = i;
        }
        if (!slots[i].folder.empty())
            folders.push_back(i);
        if (i && slots[i].x < slots[i - 1].x + slots[i - 1].rect.Width - 1)
            return false;
    }
    if (start == slots.size() || folders.empty())
        return false;
    std::map<wstring, size_t> rank;
    for (size_t i = 0; i < ui.tiles.size(); ++i)
        rank.emplace(ui.tiles[i].id, i);
    std::stable_sort(folders.begin(), folders.end(), [&](size_t a, size_t b) {
        return rank.at(slots[a].folder) < rank.at(slots[b].folder);
    });
    for (size_t i = 0; i < slots.size(); ++i) {
        if (!slots[i].folder.empty())
            continue;
        if (i == start && side == FolderPlacement::LeftOfStart)
            order.insert(order.end(), folders.begin(), folders.end());
        order.push_back(i);
        if (i == start && side == FolderPlacement::RightOfStart)
            order.insert(order.end(), folders.begin(), folders.end());
    }
    double cursor = slots.front().x;
    for (size_t i = 0; i < order.size(); ++i) {
        if (i)
            cursor += slots[i].x - slots[i - 1].x - slots[i - 1].rect.Width;
        auto& slot = slots[order[i]];
        slot.target = cursor;
        cursor += slot.rect.Width;
    }
    const auto& first = slots[folders.front()];
    const auto& last = slots[folders.back()];
    boundary =
        side == FolderPlacement::LeftOfStart
            ? (last.target + last.rect.Width + slots[start].target) / 2
            : (slots[start].target + slots[start].rect.Width + first.target) /
                  2;
    return true;
}
static FrameworkElement repeaterChild(FrameworkElement child) {
    while (child) {
        auto parent = M::VisualTreeHelper::GetParent(child);
        if (parent == ui.repeater)
            return child;
        child = parent.try_as<FrameworkElement>();
    }
    return nullptr;
}
static void shiftButton(FrameworkElement const& e, double offset) {
    auto key = winrt::get_abi(e);
    auto [it, added] = placedButtons.try_emplace(key);
    auto& p = it->second;
    auto current = e.Margin();
    if (added || current != p.applied) {
        auto property = FrameworkElement::MarginProperty();
        auto expression = e.GetBindingExpression(property);
        p = {e, current, current, e.ReadLocalValue(property),
             expression ? expression.ParentBinding() : nullptr};
    }
    auto next = p.base;
    next.Left += offset;
    next.Right -= offset;
    if (current != next)
        e.Margin(next);
    p.applied = next;
}
static bool usableAnchor(FrameworkElement const& e) {
    return e && e.ActualWidth() >= 2 &&
           e.Visibility() == X::Visibility::Visible;
}
static double centerX(F::Rect rect) {
    return rect.X + rect.Width / 2.;
}

// The slot can include margins or contain a smaller button. Preserve the
// native pivot inside that slot even when Explorer wraps the button in a panel.
static F::Rect folderContentBounds(const BarSlot& slot,
                                   FrameworkElement const& host,
                                   const RowSpace& toOverlay,
                                   double logicalX,
                                   bool rtl) {
    auto item = slot.element;
    auto content = layoutBoundsIn(host, item);
    auto placed = placedButtons.find(winrt::get_abi(item));
    auto margin =
        placed == placedButtons.end() ? item.Margin() : placed->second.base;
    double spare = std::max(
        0., slot.rect.Width - margin.Left - margin.Right - item.ActualWidth());
    auto align = item.HorizontalAlignment();
    double left =
        margin.Left + (align == X::HorizontalAlignment::Left    ? 0
                       : align == X::HorizontalAlignment::Right ? spare
                                                                : spare / 2);
    if ((item.FlowDirection() == X::FlowDirection::RightToLeft) != rtl)
        content.X = float(item.ActualWidth() - content.X - content.Width);
    content.X += float(logicalX + left);
    content.Y += item.ActualOffset().y;
    return toOverlay.map(content);
}

static std::vector<wstring> draggedFolderOrder() {
    auto order = projection.order;
    auto& g = projection.gesture;
    if (!g.moved || !projection.tiles.contains(g.id))
        return order;
    double center =
        centerX(projection.tiles.at(g.id).slot) + g.point.X - g.origin.X;
    std::erase(order, g.id);
    auto next = std::find_if(order.begin(), order.end(), [&](const auto& id) {
        return center < centerX(projection.tiles.at(id).slot);
    });
    order.insert(next, g.id);
    return order;
}
static void setFolderTargets(const std::vector<wstring>& order,
                             double now,
                             bool drag,
                             bool animate = true) {
    if (order.empty())
        return;
    const auto& slots = projection.order;
    double x = projection.tiles.at(slots.front()).slot.X;
    const auto last = projection.tiles.at(slots.back()).slot;
    const double right = last.X + last.Width;
    for (size_t i = 0; i < order.size(); ++i) {
        auto& tile = projection.tiles.at(order[i]);
        if (drag && order[i] == projection.gesture.id) {
            double wanted = tile.slot.X + projection.gesture.point.X -
                            projection.gesture.origin.X;
            double bounded = std::clamp(
                wanted, double(projection.tiles.at(slots.front()).slot.X),
                std::max(double(projection.tiles.at(slots.front()).slot.X),
                         right - tile.slot.Width));
            // A little resistance at the edge also gives a single folder
            // feedback.
            tile.x.set(bounded + 12 * std::tanh((wanted - bounded) / 42.));
        } else if (tile.x.end != x || (!animate && tile.x.duration)) {
            if (animate && projection.motion)
                tile.x.to(x, Slide::settleMs, now);
            else
                tile.x.set(x);
        }
        x += tile.slot.Width;
        if (i + 1 < slots.size()) {
            auto a = projection.tiles.at(slots[i]).slot;
            x += projection.tiles.at(slots[i + 1]).slot.X - a.X - a.Width;
        }
    }
}
static void watchPlacementMotion(double milliseconds = 0);
static void alignFolderDivider(FrameworkElement const& folder);
static bool paintProjection(double now, bool bindObservers = true);
static void watchDividerProperty(X::DependencyObject const& object,
                                 X::DependencyProperty const& property,
                                 bool rebind = false,
                                 bool rowMotion = false) {
    auto token = object.RegisterPropertyChangedCallback(
        property, [rebind, rowMotion](auto&&, auto&&) {
            if (stopping || ui.rootDetached || !projection.line)
                return;
            projection.rebindDivider |= rebind;
            projection.dividerChanged = true;
            try {
                if (rowMotion && rebind)
                    bindRowTranslations();
                if (rowMotion || projection.pointerDown)
                    paintProjection(motionTime(), false);
                else if (projection.watchedFolder)
                    alignFolderDivider(projection.watchedFolder);
                watchPlacementMotion();
            } catch (...) {
            }
        });
    projection.dividerWatches.push_back({object, property, token});
}
static void watchDividerTransform(M::Transform const& transform,
                                  bool rowMotion = false) {
    if (!transform)
        return;
    auto watch = [&](const auto& object, const auto& property,
                     bool rebind = false) {
        watchDividerProperty(object, property, rebind, rowMotion);
    };
    if (auto group = transform.try_as<M::TransformGroup>()) {
        watch(group, M::TransformGroup::ChildrenProperty(), true);
        for (const auto& child : group.Children())
            watchDividerTransform(child, rowMotion);
    } else if (auto move = transform.try_as<M::TranslateTransform>()) {
        watch(move, M::TranslateTransform::XProperty());
        watch(move, M::TranslateTransform::YProperty());
    } else if (auto scale = transform.try_as<M::ScaleTransform>()) {
        watch(scale, M::ScaleTransform::ScaleXProperty());
        watch(scale, M::ScaleTransform::ScaleYProperty());
        watch(scale, M::ScaleTransform::CenterXProperty());
        watch(scale, M::ScaleTransform::CenterYProperty());
    } else if (auto composite = transform.try_as<M::CompositeTransform>()) {
        for (auto property : {M::CompositeTransform::ScaleXProperty(),
                              M::CompositeTransform::ScaleYProperty(),
                              M::CompositeTransform::TranslateXProperty(),
                              M::CompositeTransform::TranslateYProperty(),
                              M::CompositeTransform::CenterXProperty(),
                              M::CompositeTransform::CenterYProperty(),
                              M::CompositeTransform::RotationProperty(),
                              M::CompositeTransform::SkewXProperty(),
                              M::CompositeTransform::SkewYProperty()})
            watch(composite, property);
    } else if (auto matrix = transform.try_as<M::MatrixTransform>()) {
        watch(matrix, M::MatrixTransform::MatrixProperty());
    }
}
static void watchDividerEdges(FrameworkElement const& folder) {
    if (!projection.rebindDivider &&
        projection.start == projection.watchedStart &&
        folder == projection.watchedFolder)
        return;
    unwatchDivider();
    projection.rebindDivider = false;
    for (const auto& edge : {projection.start, folder}) {
        watchDividerProperty(edge, X::UIElement::RenderTransformProperty(),
                             true);
        watchDividerProperty(edge,
                             X::UIElement::RenderTransformOriginProperty());
        watchDividerTransform(edge.RenderTransform());
    }
    auto ancestor = M::VisualTreeHelper::GetParent(projection.start)
                        .try_as<FrameworkElement>();
    while (ancestor && ancestor != ui.root) {
        watchDividerProperty(ancestor, X::UIElement::RenderTransformProperty(),
                             true, true);
        auto transform = ancestor.RenderTransform();
        if (transform &&
            (!projection.shell.sharedMoves || !isRowTranslation(transform))) {
            watchDividerProperty(ancestor,
                                 X::UIElement::RenderTransformOriginProperty(),
                                 false, true);
            watchDividerTransform(transform, true);
        }
        ancestor =
            M::VisualTreeHelper::GetParent(ancestor).try_as<FrameworkElement>();
    }
    projection.watchedStart = projection.start;
    projection.watchedFolder = folder;
}
static F::Rect visibleButtonBounds(FrameworkElement const& element) {
    const auto size = element.ActualSize();
    F::Rect r{0, 0, size.x, size.y};
    if (element != projection.start)
        return element.TransformToVisual(ui.overlay).TransformBounds(r);
    if (element.FlowDirection() == ui.repeater.FlowDirection()) {
        // Read the live Dock transform around the XAML pivot. Start's
        // compositor Offset already moves the entire overlay and must not move
        // the line twice.
        auto move = element.RenderTransform();
        if (move) {
            auto origin = element.RenderTransformOrigin();
            auto x = origin.X * size.x, y = origin.Y * size.y;
            r = move.TransformBounds({-x, -y, size.x, size.y});
            r.X += x;
            r.Y += y;
        }
        auto offset = element.ActualOffset();
        r.X += offset.x;
        r.Y += offset.y;
    } else {
        r = element.TransformToVisual(ui.repeater).TransformBounds(r);
    }
    return projection.rowSpace.map(r);
}
static void alignFolderDivider(FrameworkElement const& button) {
    auto start = visibleButtonBounds(projection.start),
         folder = visibleButtonBounds(button);
    const bool left = projection.side == FolderPlacement::LeftOfStart;
    double x = ((left ? start.X + folder.X + folder.Width
                      : start.X + start.Width + folder.X) -
                folderDivider.Width()) /
               2.;
    double y =
        (start.Y + start.Height / 2. + folder.Y + folder.Height / 2.) / 2. -
        C::Canvas::GetTop(folderDivider) - folderDivider.Height() / 2.;
    if (std::abs(projection.line.X() - x) > .005)
        projection.line.X(x);
    if (std::abs(projection.line.Y() - y) > .005)
        projection.line.Y(y);
}
static bool paintProjection(double now, bool bindObservers) {
    if (!projection.start || !projection.line || projection.tiles.empty())
        return false;
    const bool left = projection.side == FolderPlacement::LeftOfStart;
    auto actual =
        projection.rowSpace.map(layoutBoundsIn(projection.start, ui.repeater));
    double shift = actual.X - projection.startLayout.X;
    double shiftY = actual.Y - projection.startLayout.Y;
    bool nativeDrag = projection.pointerDown || now < projection.releaseAfter;
    if (nativeDrag) {
        auto rendered = visibleButtonBounds(projection.start);
        shift +=
            left ? rendered.X - actual.X
                 : (rendered.X + rendered.Width) - (actual.X + actual.Width);
    }
    bool active = false;
    double adjacent = left ? std::numeric_limits<double>::lowest()
                           : std::numeric_limits<double>::max();
    const ProjectedTile* nearest = nullptr;
    for (auto& [id, t] : projection.tiles) {
        if (!projection.motion && t.x.duration)
            t.x.set(t.x.end);
        double x = t.x.value(now) + shift;
        if (std::abs(t.translation.X() - x) > .005)
            t.translation.X(x);
        if (std::abs(t.translation.Y() - shiftY) > .005)
            t.translation.Y(shiftY);
        double facing = left ? x + t.slot.Width : x;
        if (!nearest || (left ? facing > adjacent : facing < adjacent)) {
            nearest = &t;
            adjacent = facing;
        }
        active |= t.x.active(now);
    }
    if (bindObservers)
        watchDividerEdges(nearest->button);
    alignFolderDivider(nearest->button);
    bool changed = std::exchange(projection.dividerChanged, false);
    return changed || active || projection.pointerDown ||
           now < projection.watchUntil;
}
static void watchPlacementMotion(double milliseconds) {
    if (!projection.start || (projection.rendering && milliseconds == 0))
        return;
    projection.watchUntil =
        std::max(projection.watchUntil, motionTime() + milliseconds);
    if (projection.rendering)
        return;
    projection.frame = M::CompositionTarget::Rendering([](auto&&, auto&&) {
        try {
            if (!stopping && !ui.rootDetached && paintProjection(motionTime()))
                return;
        } catch (...) {
        }
        stopPlacementFrames();
    });
    projection.rendering = true;
}
static void beginFolderGesture(const wstring& id,
                               uint32_t pointer,
                               F::Point origin,
                               bool mouse = false) {
    if (!projection.tiles.contains(id) || !projection.gesture.id.empty())
        return;
    projection.ignoreClick.clear();
    projection.gesture = {id, pointer, origin, origin, false, mouse};
}
static void moveFolderGesture(uint32_t pointer, F::Point point) {
    auto& g = projection.gesture;
    if (g.id.empty() || g.pointer != pointer)
        return;
    g.point = point;
    auto scale = ui.root.XamlRoot().RasterizationScale();
    UINT dpi = UINT(std::lround(96 * scale));
    POINT delta{LONG(std::lround((point.X - g.origin.X) * scale)),
                LONG(std::lround((point.Y - g.origin.Y) * scale))};
    if (!g.moved && !pastDragThreshold({}, delta, dpi))
        return;
    g.moved = true;
    setFolderTargets(draggedFolderOrder(), motionTime(), true);
    watchPlacementMotion();
}
static void reorderFolder(Model& model,
                          const wstring& id,
                          const std::vector<wstring>& order) {
    auto item = std::find_if(model.begin(), model.end(),
                             [&](auto& f) { return f.id == id; });
    auto position = std::find(order.begin(), order.end(), id);
    if (item == model.end() || position == order.end() || order.size() < 2)
        return;
    bool last = std::next(position) == order.end();
    auto& neighbor = last ? *std::prev(position) : *std::next(position);
    auto to = std::find_if(model.begin(), model.end(),
                           [&](auto& f) { return f.id == neighbor; });
    if (to == model.end())
        return;
    if (last)
        ++to;
    if (item < to)
        std::rotate(item, std::next(item), to);
    else if (to < item)
        std::rotate(to, item, std::next(item));
}
static void endFolderGesture(bool commit) {
    if (projection.gesture.id.empty())
        return;
    auto order = draggedFolderOrder();
    auto g = std::exchange(projection.gesture, FolderGesture{});
    if (g.moved || !commit) {
        projection.ignoreClick = g.id;
        projection.ignoreUntil = motionTime() + 400;
    }
    if (!g.moved)
        return;
    auto bounds = projection.tiles.at(g.id).slot;
    if (g.point.Y < bounds.Y - 5 || g.point.Y > bounds.Y + bounds.Height + 5)
        commit = false;
    if (!commit || order == projection.order ||
        !edit([&](Model& m) { reorderFolder(m, g.id, order); }))
        order = projection.order;
    setFolderTargets(order, motionTime(), false);
    watchPlacementMotion();
    requestLayout();
}
static bool positionByStart(const std::map<wstring, FrameworkElement>& hosts) {
    auto side = folderPlacement.load();
    if (side == FolderPlacement::Normal || !ui.repeater || ui.tiles.empty())
        return false;
    auto width = ui.repeater.ActualWidth();
    if (width < 2)
        return false;
    const auto toOverlay = rowLayoutSpace();
    const bool rtl = toOverlay.mirrored;
    const bool rowRtl =
        ui.repeater.FlowDirection() == X::FlowDirection::RightToLeft;
    std::map<void*, wstring> members;
    for (const auto& [id, button] : hosts) {
        if (!usableAnchor(button))
            return false;
        auto child = repeaterChild(button);
        if (!child || !members.emplace(winrt::get_abi(child), id).second)
            return false;
    }
    std::vector<BarSlot> slots;
    std::optional<F::Rect> start;
    auto count = M::VisualTreeHelper::GetChildrenCount(ui.repeater);
    for (int i = 0; i < count; ++i) {
        auto e = M::VisualTreeHelper::GetChild(ui.repeater, i)
                     .try_as<FrameworkElement>();
        if (!usableAnchor(e) || e.ActualHeight() < 2)
            continue;
        auto r = C::Primitives::LayoutInformation::GetLayoutSlot(e);
        if (r.Width < 2 || r.Height < 2 || r.X + r.Width <= 0 ||
            r.Y + r.Height <= 0 || r.X >= width + 1)
            continue;
        bool isStart = X::Automation::AutomationProperties::GetAutomationId(
                           e) == L"StartButton";
        if (isStart)
            start = r;
        double x = rtl ? width - r.X - r.Width : r.X;
        auto found = members.find(winrt::get_abi(e));
        slots.push_back({e, found == members.end() ? wstring{} : found->second,
                         r, x, x, isStart});
    }
    if (!start)
        return false;
    const auto row = start->Y + start->Height / 2;
    std::erase_if(slots, [&](const BarSlot& s) {
        return row < s.rect.Y || row >= s.rect.Y + s.rect.Height;
    });
    if (slots.empty())
        return false;
    double begin = slots.front().x, end = begin;
    for (const auto& s : slots) {
        begin = std::min(begin, s.x);
        end = std::max(end, s.x + s.rect.Width);
    }
    for (const auto& tile : ui.tiles) {
        if (hosts.contains(tile.id)) {
            if (std::none_of(slots.begin(), slots.end(),
                             [&](auto& s) { return s.folder == tile.id; }))
                return false;
        } else {
            double x = rtl ? begin - 40 : end;
            slots.push_back({nullptr,
                             tile.id,
                             {0, start->Y, 40, start->Height},
                             x,
                             x,
                             false});
            if (rtl)
                begin = x;
            else
                end = x + 40;
        }
    }
    double boundary = 0;
    if (!orderFolderSlots(slots, side, boundary))
        return false;
    auto mapped = [&](double x, float y, float w, float h) {
        return toOverlay.map({float(rtl ? width - x - w : x), y, w, h});
    };
    // An overflowed or clipped row belongs to the shell. Never cover the
    // notification area.
    auto rowBounds = mapped(slots.front().x, start->Y,
                            float(end - slots.front().x), start->Height);
    if (rowBounds.X < -1 ||
        rowBounds.X + rowBounds.Width > ui.overlay.ActualWidth() + 1)
        return false;
    bool first = !projection.start;
    bool moved = first || projection.side != side;
    projection.motion = allowMotion();
    projection.side = side;
    projection.order.clear();
    std::set<void*> shifted;
    for (const auto& slot : slots) {
        const auto offset = (slot.target - slot.x) * (rtl ? -1 : 1);
        if (slot.element && std::abs(offset) >= .01) {
            shifted.insert(winrt::get_abi(slot.element));
            shiftButton(slot.element, offset);
        }
        if (!slot.folder.empty()) {
            auto tile =
                std::find_if(ui.tiles.begin(), ui.tiles.end(),
                             [&](auto& t) { return t.id == slot.folder; });
            auto& t = projection.tiles[slot.folder];
            const float h = slot.element ? slot.rect.Height
                                         : std::min(38.f, slot.rect.Height);
            auto rect = mapped(slot.target, row - h / 2, slot.rect.Width, h);
            if (!t.button) {
                t.button = tile->button;
                t.layer = C::Canvas();
                t.translation = M::TranslateTransform();
                t.x.set(rect.X);
                t.layer.RenderTransform(t.translation);
                t.translation.X(rect.X);
            }
            moved |= rect != t.slot;
            t.slot = rect;
            reparent(t.button, t.layer);
            reparent(t.layer, ui.overlay);
            auto native = hosts.find(tile->id);
            auto host = native == hosts.end() ? FrameworkElement{nullptr}
                                              : native->second;
            followNativeMotion(*tile, host);
            auto content = host
                               ? folderContentBounds(
                                     slot, host, toOverlay,
                                     rtl ? width - slot.target - slot.rect.Width
                                         : slot.target,
                                     rowRtl)
                               : rect;
            if (t.layer.Width() != rect.Width)
                t.layer.Width(rect.Width);
            if (t.layer.Height() != rect.Height)
                t.layer.Height(rect.Height);
            if (t.button.Width() != content.Width)
                t.button.Width(content.Width);
            if (t.button.Height() != content.Height)
                t.button.Height(content.Height);
            if (C::Canvas::GetLeft(t.button) != content.X - rect.X)
                C::Canvas::SetLeft(t.button, content.X - rect.X);
            if (C::Canvas::GetTop(t.button) != content.Y - rect.Y)
                C::Canvas::SetTop(t.button, content.Y - rect.Y);
            if (C::Canvas::GetLeft(t.layer) != 0)
                C::Canvas::SetLeft(t.layer, 0);
            if (C::Canvas::GetTop(t.layer) != rect.Y)
                C::Canvas::SetTop(t.layer, rect.Y);
            projection.order.push_back(slot.folder);
            // Explorer keeps its real slot for pin management. The visible
            // button owns pointer capture and can never start a native pin
            // reorder.
            continue;
        }
        if (slot.start) {
            projection.start = slot.element;
            projection.startLayout = folderContentBounds(
                slot, slot.element, toOverlay,
                rtl ? width - slot.target - slot.rect.Width : slot.target,
                rowRtl);
        }
    }
    for (auto i = placedButtons.begin(); i != placedButtons.end();) {
        if (shifted.contains(i->first)) {
            ++i;
            continue;
        }
        restorePosition(i->second);
        i = placedButtons.erase(i);
    }
    std::sort(projection.order.begin(), projection.order.end(),
              [](const auto& a, const auto& b) {
                  return projection.tiles.at(a).slot.X <
                         projection.tiles.at(b).slot.X;
              });
    setFolderTargets(projection.order, motionTime(), false, false);
    if (!folderDivider) {
        folderDivider = C::Border();
        folderDivider.IsHitTestVisible(false);
        folderDivider.Width(1);
        projection.line = M::TranslateTransform();
        folderDivider.RenderTransform(projection.line);
        C::Canvas::SetLeft(folderDivider, 0);
        X::Automation::AutomationProperties::SetAccessibilityView(
            folderDivider, X::Automation::Peers::AccessibilityView::Raw);
    }
    uint32_t index = 0;
    if (!ui.overlay.Children().IndexOf(folderDivider, index))
        ui.overlay.Children().Append(folderDivider);
    auto theme = ui.root.ActualTheme();
    bool dark = theme == X::ElementTheme::Dark;
    W::UI::Color color =
        dark ? W::UI::Color{68, 255, 255, 255} : W::UI::Color{62, 0, 0, 0};
    HIGHCONTRASTW contrast{sizeof(contrast)};
    if (SystemParametersInfoW(SPI_GETHIGHCONTRAST, sizeof(contrast), &contrast,
                              0) &&
        (contrast.dwFlags & HCF_HIGHCONTRASTON)) {
        auto ink = GetSysColor(COLOR_WINDOWTEXT);
        color = {255, GetRValue(ink), GetGValue(ink), GetBValue(ink)};
    }
    auto brush = folderDivider.Background().try_as<M::SolidColorBrush>();
    if (!brush || brush.Color() != color)
        folderDivider.Background(M::SolidColorBrush(color));
    const float h = std::min(18.f, start->Height);
    auto at = mapped(boundary - .5, row - h / 2, 1, h);
    if (folderDivider.Height() != h)
        folderDivider.Height(h);
    if (first)
        projection.line.X(at.X);
    if (C::Canvas::GetTop(folderDivider) != at.Y)
        C::Canvas::SetTop(folderDivider, at.Y);
    bindShellMotion();
    syncShellOffsets();
    if (paintProjection(motionTime()) || moved)
        watchPlacementMotion();
    return true;
}
static void syncOpacity(const std::vector<FrameworkElement>&);
static bool deferPlacementScan(double now, bool down) {
    if (!projection.start || folderPlacement.load() != projection.side ||
        ui.tiles.size() != groups.size())
        return false;
    if (fileDragActive || now < fileLayoutAfter)
        return true;
    auto& gesture = projection.gesture;
    if (!gesture.id.empty()) {
        if (!gesture.mouse || down) {
            gesture.missedRelease = 0;
            return true;
        }
        // A shell capture transfer can consume the final mouse event.
        if (!gesture.missedRelease)
            gesture.missedRelease = now;
        if (now - gesture.missedRelease < 140)
            return true;
        endFolderGesture(false);
    }
    if (projection.pointerDown) {
        if (down)
            return true;
        projection.pointerDown = false;
        projection.releaseAfter = now + 140;
    }
    return now < projection.releaseAfter;
}
static bool holdTransientLayout(double now) {
    if (!projection.start || folderPlacement.load() != projection.side)
        return false;
    if (!projection.invalidSince)
        projection.invalidSince = now;
    return now - projection.invalidSince < 420;
}
static FrameworkElement groupedSlot(FrameworkElement button) {
    auto current = button;
    while (auto parent = M::VisualTreeHelper::GetParent(current)
                             .try_as<FrameworkElement>()) {
        if (parent == ui.repeater)
            return current;
        if (M::VisualTreeHelper::GetChildrenCount(parent) != 1)
            break;
        current = parent;
    }
    return button;
}
static bool syncHiddenSlots(const std::vector<FrameworkElement>& needed) {
    bool changed = false;
    for (auto it = ui.hidden.begin(); it != ui.hidden.end();) {
        auto e = it->item.get();
        if (e && std::find(needed.begin(), needed.end(), e) != needed.end()) {
            ++it;
            continue;
        }
        restore(*it);
        it = ui.hidden.erase(it);
        changed = true;
    }
    for (auto& e : needed)
        if (e.Visibility() != X::Visibility::Collapsed) {
            auto prop = X::UIElement::VisibilityProperty();
            auto binding = e.GetBindingExpression(prop);
            ui.hidden.push_back({winrt::make_weak(e), e.ReadLocalValue(prop),
                                 binding ? binding.ParentBinding() : nullptr});
            e.Visibility(X::Visibility::Collapsed);
            changed = true;
        }
    return changed;
}
static void scan() {
    if (stopping || ui.rootDetached || !ui.frame || !ui.root || !ui.overlay)
        return;
    if (projection.start && fileDragActive)
        return;
    if (deferPlacementScan(motionTime(), primaryMouseDown())) {
        requestLayout();
        return;
    }
    if (revision != ui.painted) {
        bool reuse =
            ui.tiles.size() == groups.size() &&
            std::all_of(groups.begin(), groups.end(), [](auto& f) {
                return std::any_of(ui.tiles.begin(), ui.tiles.end(),
                                   [&](auto& t) { return t.id == f.id; });
            });
        if (!reuse) {
            clearFolderPlacement();
            off(ui.tileOff);
            ui.overlay.Children().Clear();
            ui.strip.Children().Clear();
            ui.tiles.clear();
        } else {
            for (size_t i = 0; i < groups.size(); ++i) {
                auto next =
                    std::find_if(ui.tiles.begin() + i, ui.tiles.end(),
                                 [&](auto& t) { return t.id == groups[i].id; });
                std::iter_swap(ui.tiles.begin() + i, next);
            }
        }
        for (size_t i = 0; i < groups.size(); ++i) {
            const auto& f = groups[i];
            if (validFolderId(f.id))
                try {
                    auto p = pinPaths(f);
                    ensureAssets(f, p, true);
                } catch (...) {
                    Wh_Log(L"Folder preview update deferred.");
                }
            if (!reuse) {
                ui.tiles.push_back({makeTile(f), f.id});
                continue;
            }
            auto b = ui.tiles[i].button;
            if (X::Automation::AutomationProperties::GetName(b) != f.name) {
                X::Automation::AutomationProperties::SetName(b, f.name);
                C::ToolTipService::SetToolTip(
                    b, winrt::box_value(hstring(f.name)));
            }
            if (auto image = b.Content().try_as<C::Image>()) {
                const auto& preview = previewFor(f).image;
                if (image.Source() != preview)
                    image.Source(preview);
            }
        }
        std::erase_if(previews, [](const auto& entry) {
            return !lookup(groups, entry.first);
        });
        ui.groupByApp.clear();
        for (const auto& folder : groups)
            for (const auto& app : folder.apps)
                if (app.groupTaskbarButton && !app.id.empty())
                    ui.groupByApp.try_emplace(app.id, folder.id);
        ui.painted = revision;
    }
    buttons.clear();
    if (ui.repeater && !groups.empty())
        visit(ui.repeater, [](FrameworkElement const& e) {
            if (winrt::get_class_name(e) != L"Taskbar.TaskListButton")
                return true;
            auto id = appKey(e);
            if (!id.empty())
                buttons.push_back({e, id});
            return false;
        });
    std::map<wstring, FrameworkElement> anchors;
    std::vector<FrameworkElement> childrenToHide;
    std::vector<NativeButton> hosts;
    std::set<wstring> pinned;
    if (nativeReady.load())
        for (auto& b : buttons)
            if (auto f = folderForPin(b.id)) {
                hosts.push_back(b);
                pinned.insert(f->id);
            }
    for (auto const& b : buttons) {
        if (prefix(b.id, kPinPrefix))
            continue;
        auto group = ui.groupByApp.find(b.id);
        if (group == ui.groupByApp.end() || !canGroupClosedButton(b.element))
            continue;
        const auto& id = group->second;
        if (!pinned.contains(id) && !anchors.contains(id))
            anchors.emplace(id, b.element);
        else
            childrenToHide.push_back(groupedSlot(b.element));
    }
    if (syncHiddenSlots(childrenToHide) && projection.start) {
        requestLayout();
        return;
    }
    auto folderHosts = anchors;
    for (auto& host : hosts)
        if (auto f = folderForPin(host.id))
            folderHosts.insert_or_assign(f->id, host.element);
    bool positioned = false;
    try {
        positioned = positionByStart(folderHosts);
    } catch (...) {
        positioned = false;
    }
    if (!positioned) {
        if (holdTransientLayout(motionTime())) {
            requestLayout();
            return;
        }
        clearFolderPlacement();
    } else
        projection.invalidSince = 0;
    if (positioned) {
        syncOpacity({});
        syncPinCaptions({});
        syncGlyphs({});
        setNativeDrops({});
    } else {
        syncPinCaptions(hosts);
        syncGlyphs(hosts);
        setNativeDrops(hosts);
    }
    syncProxies(positioned ? folderHosts
                           : std::map<wstring, FrameworkElement>{});
    int loose = 0;
    std::vector<FrameworkElement> hideGlyphs;
    for (auto& tile : ui.tiles) {
        auto sample = folderHosts.find(tile.id);
        try {
            matchTaskbarPlate(tile, sample == folderHosts.end()
                                        ? FrameworkElement{nullptr}
                                        : sample->second);
        } catch (...) {
        }
        if (positioned) {
            if (!folderHosts.contains(tile.id))
                ++loose;
            continue;
        }
        if (pinned.contains(tile.id)) {
            followNativeMotion(tile, nullptr);
            detach(
                M::VisualTreeHelper::GetParent(tile.button).try_as<C::Panel>(),
                tile.button);
            continue;
        }
        auto i = anchors.find(tile.id);
        if (i == anchors.end() || !usableAnchor(i->second)) {
            ++loose;
            followNativeMotion(tile, nullptr);
            reparent(tile.button, ui.strip);
            continue;
        }
        auto anchor = i->second;
        auto width = anchor.ActualWidth(), height = anchor.ActualHeight();
        auto pos = layoutBoundsIn(anchor, ui.overlay);
        reparent(tile.button, ui.overlay);
        followNativeMotion(tile, anchor);
        if (tile.button.Width() != width)
            tile.button.Width(width);
        if (tile.button.Height() != std::max(1., height))
            tile.button.Height(std::max(1., height));
        if (C::Canvas::GetLeft(tile.button) != pos.X)
            C::Canvas::SetLeft(tile.button, pos.X);
        if (C::Canvas::GetTop(tile.button) != pos.Y)
            C::Canvas::SetTop(tile.button, pos.Y);
        hideGlyphs.push_back(anchor);
    }
    for (auto const& tile : ui.tiles)
        if (M::VisualTreeHelper::GetParent(tile.button) == ui.strip) {
            if (tile.button.Width() != 40)
                tile.button.Width(40);
            if (tile.button.Height() != 38)
                tile.button.Height(38);
        }
    syncOpacity(hideGlyphs);
    double gap = loose * 40.;
    if (ui.repeater && gap != ui.reserved) {
        if (!ui.marginSet)
            ui.baseMargin = ui.repeater.Margin();
        auto m = ui.baseMargin;
        m.Right += gap;
        ui.repeater.Margin(m);
        ui.marginSet = gap != 0;
        ui.reserved = gap;
    }
}
struct Faded {
    winrt::weak_ref<FrameworkElement> e;
    F::IInspectable old;
};
[[clang::no_destroy]] static std::vector<Faded> faded;
static void unfade(Faded& f) {
    if (auto e = f.e.get(); e && e.Opacity() == 0) {
        if (!f.old || f.old == X::DependencyProperty::UnsetValue())
            e.ClearValue(X::UIElement::OpacityProperty());
        else
            e.SetValue(X::UIElement::OpacityProperty(), f.old);
    }
}
static void syncOpacity(const std::vector<FrameworkElement>& wanted) {
    for (auto i = faded.begin(); i != faded.end();) {
        auto e = i->e.get();
        if (e && std::find(wanted.begin(), wanted.end(), e) != wanted.end()) {
            ++i;
            continue;
        }
        unfade(*i);
        i = faded.erase(i);
    }
    for (auto const& e : wanted)
        if (e.Opacity() != 0) {
            faded.push_back(
                {winrt::make_weak(e),
                 e.ReadLocalValue(X::UIElement::OpacityProperty())});
            e.Opacity(0);
        }
}
static std::optional<App> droppedShortcut(const wstring& name) {
    fs::path path(name);
    std::error_code error;
    if (!path.is_absolute() || !appFile(path) ||
        !fs::is_regular_file(path, error))
        return {};
    auto app = fromPath(name);
    if (app)
        app->groupTaskbarButton = false;
    return app;
}
static void collectDrops() {
    std::vector<DropJob> completed;
    for (auto job = ui.drops.begin(); job != ui.drops.end();) {
        try {
            if (job->storage) {
                auto status = job->storage.Status();
                if (status == F::AsyncStatus::Started) {
                    ++job;
                    continue;
                }
                if (status == F::AsyncStatus::Completed)
                    for (auto item : job->storage.GetResults())
                        if (item.IsOfType(W::Storage::StorageItemTypes::File))
                            job->paths.emplace_back(item.Path());
            }
        } catch (...) {
            Wh_Log(L"Could not read the dropped shortcut.");
        }
        completed.push_back(std::move(*job));
        job = ui.drops.erase(job);
    }
    if (ui.drops.empty() && controller)
        KillTimer(controller, kDropTimer);
    for (auto& job : completed) {
        bool saved = false;
        if (!stopping && lookup(groups, job.folder)) {
            for (const auto& path : job.paths)
                if (auto app = droppedShortcut(path))
                    job.apps.push_back(std::move(*app));
            if (!job.apps.empty())
                saved = editMove([&](Model& m) {
                    insertDropped(m, job.folder, job.apps, job.before);
                });
        }
        clearIncoming(job.previewToken, saved);
    }
    syncPanel();
}
static FrameworkElement buttonOf(X::DependencyObject);
static wstring projectedFolderAt(X::DependencyObject node) {
    while (node && node != ui.root) {
        if (auto b = node.try_as<Button>())
            for (const auto& [id, tile] : projection.tiles)
                if (tile.button == b)
                    return id;
        node = M::VisualTreeHelper::GetParent(node);
    }
    return {};
}
static void watchFrame() {
    auto root = ui.root;
    auto enterEvent = X::UIElement::DragEnterEvent();
    auto enter = winrt::box_value(X::DragEventHandler(
        [](F::IInspectable const&, X::DragEventArgs const& e) {
            try {
                if (stopping || fileDragActive || !ui.drag.app.empty() ||
                    ui.tiles.empty())
                    return;
                auto view = e.DataView();
                if (view.Contains(Data::StandardDataFormats::StorageItems()))
                    prepareFileWindows(view);
            } catch (...) {
            }
        }));
    root.AddHandler(enterEvent, enter, true);
    ui.frameOff.push_back(
        [root, enterEvent, enter] { root.RemoveHandler(enterEvent, enter); });
    auto unload = root.Unloaded([](auto&&, auto&&) {
        ui.rootDetached = true;
        projection.gesture = {};
        projection.pointerDown = false;
        stopPlacementFrames();
    });
    ui.frameOff.push_back([root, unload] { root.Unloaded(unload); });
    auto load = root.Loaded([](auto&&, auto&&) {
        ui.rootDetached = false;
        requestLayout();
    });
    ui.frameOff.push_back([root, load] { root.Loaded(load); });
    auto size = root.SizeChanged([](auto&&, auto&&) {
        if (!groups.empty())
            requestLayout();
    });
    ui.frameOff.push_back([root, size] { root.SizeChanged(size); });
    auto repeater = ui.repeater;
    auto token = repeater.LayoutUpdated([](auto&&, auto&&) {
        if (projection.start && !ui.rootDetached && !stopping)
            try {
                syncShellOffsets();
                if (paintProjection(motionTime()))
                    watchPlacementMotion();
            } catch (...) {
            }
        if (!groups.empty())
            requestLayout();
    });
    ui.frameOff.push_back([repeater, token] { repeater.LayoutUpdated(token); });
    using Update = W::UI::Input::PointerUpdateKind;
    auto attach = [&](auto event, I::PointerEventHandler handler) {
        auto boxed = winrt::box_value(handler);
        root.AddHandler(event, boxed, true);
        ui.frameOff.push_back(
            [root, event, boxed] { root.RemoveHandler(event, boxed); });
    };
    attach(X::UIElement::PointerPressedEvent(),
           [](F::IInspectable const&, I::PointerRoutedEventArgs const& e) {
               try {
                   if (stopping || groups.empty())
                       return;
                   auto p = e.GetCurrentPoint(nullptr);
                   if (p.Properties().PointerUpdateKind() !=
                       Update::LeftButtonPressed)
                       return;
                   cancelTaskbarPreview();
                   POINT screen{};
                   if (!GetCursorPos(&screen))
                       return;
                   auto source =
                       e.OriginalSource().try_as<X::DependencyObject>();
                   if (projection.start) {
                       auto id = projectedFolderAt(source);
                       if (!id.empty()) {
                           beginFolderGesture(
                               id, p.PointerId(),
                               e.GetCurrentPoint(ui.overlay).Position(),
                               e.Pointer().PointerDeviceType() ==
                                   W::Devices::Input::PointerDeviceType::Mouse);
                           requestLayout();
                           return;
                       }
                   }
                   auto button = buttonOf(source);
                   if (button)
                       ui.drag = {appKey(button), screen, p.PointerId()};
                   if (projection.start &&
                       (button ||
                        repeaterChild(source.try_as<FrameworkElement>()))) {
                       projection.pointerDown = true;
                       watchPlacementMotion(Slide::settleMs);
                       requestLayout();
                   }
                   if (ui.drag.app.empty() || prefix(ui.drag.app, kPinPrefix))
                       dismissOutside(screen);
               } catch (...) {
               }
           });
    attach(X::UIElement::PointerMovedEvent(),
           [](F::IInspectable const&, I::PointerRoutedEventArgs const& e) {
               try {
                   if (stopping ||
                       (ui.drag.app.empty() && projection.gesture.id.empty() &&
                        !projection.pointerDown))
                       return;
                   if (projection.start) {
                       auto p = e.GetCurrentPoint(ui.overlay);
                       if (!projection.gesture.id.empty() &&
                           projection.gesture.pointer == p.PointerId()) {
                           if (p.Properties().IsLeftButtonPressed())
                               moveFolderGesture(p.PointerId(), p.Position());
                           return;
                       }
                       if (projection.pointerDown &&
                           p.Properties().IsLeftButtonPressed())
                           watchPlacementMotion(Slide::settleMs);
                   }
                   if (ui.drag.app.empty() || prefix(ui.drag.app, kPinPrefix) ||
                       e.Pointer().PointerId() != ui.drag.pointerId)
                       return;
                   if (!e.GetCurrentPoint(nullptr)
                            .Properties()
                            .IsLeftButtonPressed()) {
                       cancelTaskbarPreview();
                       return;
                   }
                   POINT screen{};
                   if (!GetCursorPos(&screen))
                       return;
                   UINT dpi = GetDpiForWindow(tray());
                   if (pastDragThreshold(ui.drag.start, screen, dpi ? dpi : 96))
                       previewTaskbarApp(screen);
               } catch (...) {
               }
           });
    attach(
        X::UIElement::PointerReleasedEvent(),
        [](F::IInspectable const&, I::PointerRoutedEventArgs const& e) {
            try {
                if (!projection.gesture.id.empty() &&
                    projection.gesture.pointer == e.Pointer().PointerId()) {
                    auto p = e.GetCurrentPoint(ui.overlay);
                    if (p.Properties().PointerUpdateKind() ==
                        Update::LeftButtonReleased) {
                        moveFolderGesture(p.PointerId(), p.Position());
                        endFolderGesture(true);
                    }
                    return;
                }
                if (projection.pointerDown && !primaryMouseDown()) {
                    projection.pointerDown = false;
                    projection.releaseAfter = motionTime() + 140;
                    requestLayout();
                }
                if (stopping || ui.drag.app.empty())
                    return;
                auto p = e.GetCurrentPoint(nullptr);
                if (p.PointerId() != ui.drag.pointerId ||
                    p.Properties().PointerUpdateKind() !=
                        Update::LeftButtonReleased)
                    return;
                POINT screen{};
                HWND owner = tray();
                UINT dpi = owner ? GetDpiForWindow(owner) : 96;
                if (!GetCursorPos(&screen)) {
                    cancelTaskbarPreview();
                    return;
                }
                if (!pastDragThreshold(ui.drag.start, screen, dpi ? dpi : 96) ||
                    prefix(ui.drag.app, kPinPrefix)) {
                    cancelTaskbarPreview();
                    dismissOutside(screen);
                    return;
                }
                auto root = GetAncestor(WindowFromPoint(screen), GA_ROOT);
                bool inPanel = root && root == panel.window &&
                               !panel.choosing && !panel.closing &&
                               !panel.browsing;
                auto folder = inPanel                  ? panel.folder
                              : owner && root == owner ? dropFolderAt(screen)
                                                       : wstring{};
                if (folder.empty()) {
                    cancelTaskbarPreview();
                    return;
                }
                auto drag = std::exchange(ui.drag, TaskbarDrag{});
                if (inPanel && drag.token && drag.token == panel.incoming.token)
                    positionIncoming(screen);
                ui.releasedButtons.push_back(
                    {std::move(drag.app), folder, incomingAnchor(drag.token),
                     drag.token, std::move(drag.resolved)});
                if (iconHover.native)
                    resetIconHover(true);
                if (!SetTimer(controller, kNativeDropTimer, 140, nullptr) &&
                    !PostMessageW(controller, kTaskbarDrop, 0, 0)) {
                    ui.releasedButtons.pop_back();
                    clearIncoming(drag.token);
                }
                requestLayout();
            } catch (...) {
            }
        });
    I::PointerEventHandler cancel = [](F::IInspectable const&,
                                       I::PointerRoutedEventArgs const& e) {
        try {
            if (!projection.gesture.id.empty() &&
                projection.gesture.pointer == e.Pointer().PointerId())
                endFolderGesture(false);
            if (e.Pointer().PointerId() == ui.drag.pointerId)
                cancelTaskbarPreview();
        } catch (...) {
        }
    };
    attach(X::UIElement::PointerCanceledEvent(), cancel);
    attach(X::UIElement::PointerCaptureLostEvent(),
           [](F::IInspectable const&, I::PointerRoutedEventArgs const& e) {
               try {
                   if (!projection.gesture.id.empty() &&
                       projection.gesture.pointer == e.Pointer().PointerId()) {
                       auto point = e.GetCurrentPoint(ui.overlay);
                       bool release = point.Properties().PointerUpdateKind() ==
                                      Update::LeftButtonReleased;
                       if (release)
                           moveFolderGesture(point.PointerId(),
                                             point.Position());
                       endFolderGesture(release);
                   }
                   if (e.Pointer().PointerId() == ui.drag.pointerId)
                       cancelTaskbarPreview();
               } catch (...) {
               }
           });
    I::KeyEventHandler key = [](F::IInspectable const&,
                                I::KeyRoutedEventArgs const& e) {
        try {
            if (!projection.gesture.id.empty() &&
                e.Key() == W::System::VirtualKey::Escape) {
                endFolderGesture(false);
                e.Handled(true);
            }
        } catch (...) {
        }
    };
    auto keyHandler = winrt::box_value(key);
    auto keyEvent = X::UIElement::KeyDownEvent();
    root.AddHandler(keyEvent, keyHandler, true);
    ui.frameOff.push_back([root, keyEvent, keyHandler] {
        root.RemoveHandler(keyEvent, keyHandler);
    });
}
// The shell host sees labels only. Command indices map to the immutable folder
// IDs kept by Explorer for this particular menu. Resolve shortcuts only on
// selection.
struct NativeSelection {
    uint32_t ticket{};
    ULONGLONG opened{};
    bool awaitingGroup{};
    wstring appId;
    std::unique_ptr<ITEMIDLIST, decltype(&CoTaskMemFree)> shortcut{
        nullptr, &CoTaskMemFree};
    std::vector<MenuAction> actions;
};
static NativeSelection selection;
static uint32_t nextTicket{};
static void clearSelection() {
    selection = NativeSelection{};
    if (controller)
        SetWindowTextW(controller, L"");
}
static constexpr UINT_PTR kFindJumpTimer = 0x7470;
static unsigned findAttempts = 0;
static bool isJumpThread(DWORD tid) {
    using Describe = HRESULT(WINAPI*)(HANDLE, PWSTR*);
    static auto describe = reinterpret_cast<Describe>(GetProcAddress(
        GetModuleHandleW(L"kernel32.dll"), "GetThreadDescription"));
    if (!describe || !tid)
        return false;
    auto thread = OpenThread(THREAD_QUERY_LIMITED_INFORMATION, FALSE, tid);
    if (!thread)
        return false;
    PWSTR name = nullptr;
    auto hr = describe(thread, &name);
    CloseHandle(thread);
    bool found = SUCCEEDED(hr) && name && _wcsicmp(name, L"JumpViewUI") == 0;
    if (name)
        LocalFree(name);
    return found;
}
static bool notifyJumpHost() {
    // The host registers a message-only window on its XAML thread after first
    // use.
    bool posted = false;
    HWND h = nullptr;
    while ((h = FindWindowExW(HWND_MESSAGE, h, kHost, nullptr)))
        posted = PostMessageW(h, refreshMsg, 0, 0) || posted;
    if (posted)
        return true;
    // On cold start, activation/show messages can be sent directly rather than
    // dispatched. Post one identifiable message to the real Jump List window.
    while (
        (h = FindWindowExW(nullptr, h, L"Windows.UI.Core.CoreWindow", nullptr)))
        if (isJumpThread(GetWindowThreadProcessId(h, nullptr)) &&
            PostMessageW(h, refreshMsg, 0, 0))
            return true;
    return false;
}
static void wakeJumpHosts() {
    if (findAttempts) {
        KillTimer(controller, kFindJumpTimer);
        findAttempts = 0;
    }
    if (!notifyJumpHost() && selection.ticket &&
        SetTimer(controller, kFindJumpTimer, 120, nullptr))
        findAttempts = 7;
}
static void retryJumpHost() {
    if (!selection.ticket || notifyJumpHost() || !findAttempts ||
        !--findAttempts) {
        KillTimer(controller, kFindJumpTimer);
        findAttempts = 0;
    }
}

static void stageNative(const wstring& id, const ITEMIDLIST* pidl) {
    clearSelection();
    if (id.empty() && !pidl) {
        wakeJumpHosts();
        return;
    }
    NativeSelection next;
    auto f = folderForPin(id);
    next.actions = f ? taskbarFolderActions(*f, false) : appActions(groups);
    next.appId = id;
    if (!f && pidl)
        next.shortcut.reset(ILCloneFull(pidl));
    if (!f && id.empty() && !next.shortcut)
        return;
    nextTicket = nextTicket % 0x7ffffffeU + 1;
    next.ticket = nextTicket;
    next.opened = GetTickCount64();
    J::JsonArray labels;
    for (const auto& action : next.actions) {
        if (!f) {
            labels.Append(J::JsonValue::CreateStringValue(action.label));
            continue;
        }
        J::JsonArray row;
        row.Append(J::JsonValue::CreateStringValue(action.label));
        row.Append(
            J::JsonValue::CreateNumberValue(static_cast<int>(action.kind)));
        labels.Append(row);
    }
    auto title = std::to_wstring(next.ticket) + L"|" +
                 std::to_wstring(next.opened) + L"|" +
                 wstring(labels.Stringify());
    if (SetWindowTextW(controller, title.c_str())) {
        selection = std::move(next);
        wakeJumpHosts();
    }
}
static void invokeNativeAction(WPARAM seq, LPARAM ordinal) {
    if (!selection.ticket || seq != selection.ticket || ordinal < 1 ||
        static_cast<size_t>(ordinal) > selection.actions.size())
        return;
    auto command = std::move(selection.actions[ordinal - 1]);
    auto id = std::move(selection.appId);
    auto shortcut = std::move(selection.shortcut);
    clearSelection();  // consume before any dialogue can run a nested message
                       // loop
    if (command.kind != FolderAction::Move) {
        runFolderAction(command);
        return;
    }
    if (!lookup(groups, command.folder))
        return;
    std::optional<App> app;
    if (shortcut) {
        com_ptr<IShellItem> si;
        if (SUCCEEDED(SHCreateItemFromIDList(shortcut.get(),
                                             IID_PPV_ARGS(si.put())))) {
            auto path = display(si.get(), SIGDN_FILESYSPATH);
            if (!path.empty())
                app = fromPath(path);
            if (app && !id.empty())
                app->id = id;
        }
    }
    if (!app)
        app = byId(id);
    if (app)
        moveTo(command.folder, *app);
    else
        note(uiText(UiString::AppMissing));
}

static LRESULT CALLBACK controlProc(HWND h, UINT msg, WPARAM wp, LPARAM lp) {
    try {
        if (stopping && msg == kCancelModal) {
            if (!activeModal)
                return 0;
            drainModal = true;
            if (activeModal == ModalKind::Menu)
                EndMenu();
            else if (modalDialog)
                EndDialog(modalDialog, IDCANCEL);
            return 1;
        }
        if (stopping && (msg == kModalDrained ||
                         (msg == WM_TIMER && wp == kModalDrainTimer))) {
            KillTimer(h, kModalDrainTimer);
            if (drainModal && !activeModal)
                SetEvent(modalDrained);
            return 0;
        }
        if (msg == WM_DESTROY) {
            stopExitFrames();
            return 0;
        }
        if (msg == kMenuLaunchReady) {
            if (!stopping)
                acceptMenuLaunches();
            return 0;
        }
        if (msg == kExitFrame || (msg == WM_TIMER && wp == kExitTimer)) {
            tickExits();
            return 0;
        }
        if (msg == kButtonRunningChanged) {
            if (!groups.empty())
                requestLayout();
            return 0;
        }
        if (msg == kAppearanceChanged) {
            requestLayout();
            if (panel.window) {
                if (panel.designMenu)
                    applyPanelStyle(panel.requestedStyle, true);
                else
                    restorePanelStyle();
            }
            return 0;
        }
        if (msg == kExportReady) {
            finishExport(reinterpret_cast<ShortcutExport*>(wp));
            return 0;
        }
        if (msg == kStop) {
            while (!exports.empty())
                finishExport(exports.back().get(), false);
            syncOpacity({});
            clearView();
            decltype(buttons){}.swap(buttons);
            decltype(dropHosts){}.swap(dropHosts);
            decltype(liveIcons){}.swap(liveIcons);
            decltype(pinCaptions){}.swap(pinCaptions);
            decltype(dropOff){}.swap(dropOff);
            decltype(proxyProperties){}.swap(proxyProperties);
            decltype(faded){}.swap(faded);
            decltype(fileWindows){}.swap(fileWindows);
            decltype(exports){}.swap(exports);
            DestroyWindow(h);
            controller = nullptr;
            published = nullptr;
            return 0;
        }
        if (stopping)
            return DefWindowProcW(h, msg, wp, lp);
        if (msg == kFilesDone || msg == kCatalogDone) {
            workerFinished(msg);
            return 0;
        }
        if (msg == kTaskbarDrop ||
            (msg == WM_TIMER && wp == kNativeDropTimer)) {
            KillTimer(h, kNativeDropTimer);
            auto released = std::exchange(ui.releasedButtons, {});
            for (auto& pending : released) {
                bool saved = false;
                if (lookup(groups, pending.folder)) {
                    auto app = pending.resolved ? std::move(pending.resolved)
                                                : byId(pending.app);
                    if (app)
                        saved = editMove([&](Model& m) {
                            insertDropped(m, pending.folder, {*app},
                                          pending.before);
                        });
                }
                clearIncoming(pending.token, saved);
            }
            syncPanel();
            requestLayout();
            return 0;
        }
        if (msg == WM_TIMER) {
            if (wp == kLayoutTimer) {
                KillTimer(h, kLayoutTimer);
                ui.layoutPending = false;
                scan();
                syncPanel();
                return 0;
            }
            if (wp == kDropTimer) {
                collectDrops();
                return 0;
            }
            if (wp == kFileSurfaceTimer) {
                checkFileWindows();
                return 0;
            }
            if (wp == kIconHoverTimer) {
                previewIconDrop();
                return 0;
            }
            if (wp == kFindJumpTimer) {
                retryJumpHost();
                return 0;
            }
        }
        if (msg == actionMsg) {
            invokeNativeAction(wp, lp);
            return 0;
        }
    } catch (...) {
        Wh_Log(L"Taskbar action failed: %08X", winrt::to_hresult());
    }
    return DefWindowProcW(h, msg, wp, lp);
}
static HINSTANCE moduleInstance() {
    HMODULE h = nullptr;
    GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                           GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                       (PCWSTR)&controlProc, &h);
    return h;
}
static bool ensureControl() {
    if (controller)
        return uiThread == GetCurrentThreadId();
    WNDCLASSW wc{};
    wc.lpfnWndProc = controlProc;
    wc.hInstance = moduleInstance();
    wc.lpszClassName = kController;
    if (!RegisterClassW(&wc) && GetLastError() != ERROR_CLASS_ALREADY_EXISTS)
        return false;
    controller = CreateWindowExW(0, kController, L"", 0, 0, 0, 0, 0,
                                 HWND_MESSAGE, nullptr, wc.hInstance, nullptr);
    if (!controller)
        return false;
    uiThread = GetCurrentThreadId();
    published = controller;
    ChangeWindowMessageFilterEx(controller, actionMsg, MSGFLT_ALLOW, nullptr);
    loadModel();
    return true;
}
#include <winrt/Windows.UI.Xaml.Automation.Peers.h>

static bool isPrimaryFrame(FrameworkElement const& frame,
                           HWND primary,
                           HMONITOR monitor) {
    if (GetSystemMetrics(SM_CMONITORS) == 1 &&
        !FindWindowW(L"Shell_SecondaryTrayWnd", nullptr))
        return true;
    // The taskbar frames share a UI thread. Use the actual screen position of
    // the frame to identify the owning taskbar window on multi-monitor setups.
    auto bounds = X::Automation::Peers::FrameworkElementAutomationPeer(frame)
                      .GetBoundingRectangle();
    if (bounds.Width <= 0 || bounds.Height <= 0)
        return false;
    POINT center{lround(bounds.X + bounds.Width / 2),
                 lround(bounds.Y + bounds.Height / 2)};
    RECT bar{};
    return MonitorFromPoint(center, MONITOR_DEFAULTTONULL) == monitor &&
           GetWindowRect(primary, &bar) && PtInRect(&bar, center);
}
static void ensureFrame(FrameworkElement const& frame) {
    if (stopping || !frame || !frame.XamlRoot())
        return;
    auto primary = tray();
    if (!primary ||
        GetWindowThreadProcessId(primary, nullptr) != GetCurrentThreadId())
        return;
    if (ui.frame == frame && isPrimaryRoot(frame))
        return;
    auto monitor = MonitorFromWindow(primary, MONITOR_DEFAULTTONULL);
    if (!monitor || !isPrimaryFrame(frame, primary, monitor))
        return;
    C::Grid root{nullptr};
    FrameworkElement repeater{nullptr};
    visit(frame, [&](FrameworkElement const& e) {
        if (e.Name() == L"RootGrid" && !root)
            root = e.try_as<C::Grid>();
        if (e.Name() == L"TaskbarFrameRepeater")
            repeater = e;
        return !(root && repeater);
    });
    if (!root || !repeater || !ensureControl())
        return;
    syncOpacity({});
    clearView();
    ui.primary = primary;
    ui.monitor = monitor;
    ui.frame = frame;
    ui.root = root;
    ui.repeater = repeater;
    ui.overlay = C::Canvas();
    ui.strip = C::StackPanel();
    ui.strip.Orientation(C::Orientation::Horizontal);
    ui.strip.HorizontalAlignment(X::HorizontalAlignment::Right);
    ui.strip.VerticalAlignment(X::VerticalAlignment::Center);
    ui.overlay.FlowDirection(X::FlowDirection::LeftToRight);
    C::Grid::SetColumnSpan(ui.overlay,
                           std::max(1u, root.ColumnDefinitions().Size()));
    C::Grid::SetColumnSpan(ui.strip,
                           std::max(1u, root.ColumnDefinitions().Size()));
    root.Children().Append(ui.overlay);
    root.Children().Append(ui.strip);
    watchFrame();
    scan();
}

using TaskContext = void(__fastcall*)(void*,
                                      X::UIElement const&,
                                      I::ContextRequestedEventArgs const&);
static TaskContext oldContext;
using BlankContext = void(__fastcall*)(FrameworkElement const&,
                                       void*,
                                       I::ContextRequestedEventArgs const&,
                                       unsigned long long);
static BlankContext oldBlank;
using Append = void(__fastcall*)(void*, C::MenuFlyoutItemBase const&);
static Append oldAppend;
using AbiPointer = int(WINAPI*)(void*, void*);
using AbiApply = int(WINAPI*)(void*);
static AbiPointer oldPointer;
static AbiApply oldApply;
static thread_local bool inBlank = false, added = false;
static thread_local wstring blankFolder;
static FrameworkElement buttonOf(X::DependencyObject e) {
    while (e) {
        auto f = e.try_as<FrameworkElement>();
        if (f && winrt::get_class_name(f) == L"Taskbar.TaskListButton")
            return f;
        e = M::VisualTreeHelper::GetParent(e);
    }
    return nullptr;
}
// Taskbar.View sometimes calls ShowJumpView synchronously, sometimes via a
// posted message. Stage the XAML target only as a fallback; the native group is
// authoritative.
static bool attachPrimaryFor(FrameworkElement const& source) {
    if (isPrimaryRoot(source))
        return true;
    for (auto node = source ? source.as<X::DependencyObject>() : nullptr; node;
         node = M::VisualTreeHelper::GetParent(node)) {
        auto element = node.try_as<FrameworkElement>();
        if (element &&
            winrt::get_class_name(element) == L"Taskbar.TaskbarFrame") {
            ensureFrame(element);
            return isPrimaryRoot(source);
        }
    }
    return false;
}
static thread_local bool contextActive = false;
static void __fastcall taskContext(void* self,
                                   X::UIElement const& sender,
                                   I::ContextRequestedEventArgs const& args) {
    struct Scope {
        bool previous = std::exchange(contextActive, false);
        ~Scope() { contextActive = previous; }
    } scope;
    try {
        if (!stopping && attachPrimaryFor(sender.try_as<FrameworkElement>()) &&
            controller && GetCurrentThreadId() == uiThread) {
            auto e = buttonOf(sender);
            if (!e)
                e = buttonOf(
                    args.OriginalSource().try_as<X::DependencyObject>());
            auto id = e ? appKey(e) : L"";
            if (auto f = folderForPin(id)) {
                F::Point pos{};
                bool extended = (GetKeyState(VK_SHIFT) & 0x8000) &&
                                args.TryGetPosition(e, pos);
                if (!extended && showTaskbarFolderMenu(f->id, e)) {
                    clearSelection();
                    args.Handled(true);
                    return;
                }
            }
            stageNative(id, nullptr);
            selection.awaitingGroup = selection.ticket != 0;
            contextActive = true;
        }
    } catch (...) {
        if (controller && GetCurrentThreadId() == uiThread)
            clearSelection();
    }
    oldContext(self, sender, args);
}
static void addBlankCommands(
    F::Collections::IVector<C::MenuFlyoutItemBase> const& items) {
    auto actions = blankFolder.empty()
                       ? std::vector<MenuAction>{{FolderAction::Create,
                                                  {},
                                                  uiText(UiString::CreateMenu)}}
                       : folderActions(blankFolder, !pinnedFolder(blankFolder));
    unsigned index = 0;
    for (auto& action : actions) {
        C::MenuFlyoutItem entry;
        entry.Text(action.label);
        entry.Icon(C::SymbolIcon(menuSymbol(action.kind)));
        auto t = entry.Click([action](auto&&, auto&&) {
            try {
                if (!stopping)
                    runFolderAction(action);
            } catch (...) {
            }
        });
        ui.menuOff.push_back([items, w = winrt::make_weak(entry), t] {
            if (auto v = w.get()) {
                v.Click(t);
                uint32_t position{};
                if (items.IndexOf(v, position))
                    items.RemoveAt(position);
            }
        });
        items.InsertAt(index++, entry);
    }
}

static void __fastcall blank(FrameworkElement const& sender,
                             void* settings,
                             I::ContextRequestedEventArgs const& args,
                             unsigned long long flags) {
    struct Scope {
        bool active = std::exchange(inBlank, false),
             inserted = std::exchange(added, false);
        wstring folder = std::exchange(blankFolder, wstring{});
        ~Scope() {
            inBlank = active;
            added = inserted;
            blankFolder = std::move(folder);
        }
    } scope;
    try {
        if (!stopping) {
            auto origin = args.OriginalSource().try_as<FrameworkElement>();
            if (attachPrimaryFor(origin ? origin : sender) && controller &&
                GetCurrentThreadId() == uiThread) {
                inBlank = true;
                clearSelection();
                off(ui.menuOff);
                auto node = args.OriginalSource().try_as<X::DependencyObject>();
                while (node) {
                    auto b = node.try_as<Button>();
                    if (b) {
                        auto i = std::find_if(
                            ui.tiles.begin(), ui.tiles.end(),
                            [&](auto& t) { return t.button == b; });
                        if (i != ui.tiles.end()) {
                            if (args.Handled() ||
                                showTaskbarFolderMenu(i->id, b)) {
                                args.Handled(true);
                                return;
                            }
                            blankFolder = i->id;
                            break;
                        }
                    }
                    node = M::VisualTreeHelper::GetParent(node);
                }
            }
        }
    } catch (...) {
    }
    oldBlank(sender, settings, args, flags);
    // Some taskbar versions expose the menu directly on its owner.
    if (inBlank && !added && sender)
        try {
            auto ownerFlyout = sender.ContextFlyout();
            auto flyout = ownerFlyout ? ownerFlyout.try_as<C::MenuFlyout>()
                                      : C::MenuFlyout{nullptr};
            if (flyout && flyout.IsOpen() && flyout.Target() == sender) {
                added = true;
                addBlankCommands(flyout.Items());
            }
        } catch (...) {
        }
}
static void __fastcall append(void* self,
                              C::MenuFlyoutItemBase const& original) {
    oldAppend(self, original);
    if (!inBlank || added || stopping || !ui.root ||
        GetCurrentThreadId() != uiThread)
        return;
    added = true;
    try {
        auto items =
            reinterpret_cast<F::Collections::IVector<C::MenuFlyoutItemBase>*>(
                self);
        addBlankCommands(*items);
    } catch (...) {
    }
}

static FrameworkElement fromAbi(void* self) {
    FrameworkElement frame{nullptr};
    static_cast<IUnknown*>(self)->QueryInterface(
        winrt::guid_of<FrameworkElement>(), winrt::put_abi(frame));
    return frame;
}
static int WINAPI frameMove(void* self, void* args) {
    auto hr = oldPointer(self, args);
    if (!stopping && (!published.load() || GetCurrentThreadId() == uiThread))
        try {
            if (!ui.root || ui.rootDetached)
                ensureFrame(fromAbi(self));
        } catch (...) {
        }
    return hr;
}
static int WINAPI frameApply(void* self) {
    auto hr = oldApply(self);
    if (!stopping)
        try {
            ensureFrame(fromAbi(self));
        } catch (...) {
        }
    return hr;
}
using Jump = HRESULT(WINAPI*)(void*, void*, void*, bool);
using GetId = PCWSTR(WINAPI*)(void*);
using GetShortcut = const ITEMIDLIST*(WINAPI*)(void*);
static Jump oldJump;
static GetId groupId;
static GetShortcut groupShortcut;
using GroupTip = HRESULT(WINAPI*)(void*, void*, PWSTR, int);
using LauncherName = HRESULT(WINAPI*)(void*, PWSTR*);
static GroupTip oldGroupTip = nullptr;
static LauncherName oldLauncherName = nullptr;
static std::optional<wstring> currentPinName(void* group) {
    if (stopping || !group || !groupId)
        return {};
    auto raw = groupId(group);
    if (!raw || _wcsnicmp(raw, kPinPrefix, ARRAYSIZE(kPinPrefix) - 1))
        return {};
    std::lock_guard lock(pinNameLock);
    auto i = pinNames.find(raw);
    if (i == pinNames.end())
        return {};
    return i->second;
}
static HRESULT WINAPI groupTip(void* group,
                               void* item,
                               PWSTR buffer,
                               int length) {
    try {
        if (buffer && length > 0)
            if (auto name = currentPinName(group))
                return StringCchCopyW(buffer, length, name->c_str());
    } catch (...) {
    }
    return oldGroupTip(group, item, buffer, length);
}
static HRESULT WINAPI launcherName(void* group, PWSTR* result) {
    try {
        if (result)
            if (auto name = currentPinName(group))
                return SHStrDupW(name->c_str(), result);
    } catch (...) {
    }
    return oldLauncherName(group, result);
}
using NativeClick = HRESULT(WINAPI*)(void*, void*, void*, const void*);
using NativeLaunch = HRESULT(WINAPI*)(void*, void*, const POINT&, int);
static NativeClick oldClick = nullptr;
static NativeLaunch oldLaunch = nullptr;
static bool openPin(void* group) {
    if (stopping || !controller || GetCurrentThreadId() != uiThread || !group ||
        !groupId)
        return false;
    auto raw = groupId(group);
    if (!raw || !prefix(raw, kPinPrefix))
        return false;
    auto f = folderForPin(raw);
    if (!f)
        return false;
    FrameworkElement anchor = projectedButton(f->id);
    if (!anchor)
        for (const auto& b : buttons)
            if (equal(b.id, raw)) {
                anchor = b.element;
                break;
            }
    if (!anchor || !isPrimaryRoot(anchor))
        return false;
    showFolder(f->id, anchor);
    return true;
}
static void openMenuFolder(const wstring& id) {
    if (panel.window && panel.folder == id) {
        if (panel.browsing)
            return;
        if (panel.closing)
            revealPanel();
        if (panel.choosing) {
            panel.choosing = false;
            panel.page = 0;
            refreshPanel();
        }
        SetForegroundWindow(panel.window);
        SetFocus(panel.apps.empty() ? panel.add : panel.list);
        return;
    }
    FrameworkElement anchor = projectedButton(id);
    if (!anchor)
        for (const auto& b : buttons)
            if (equal(b.id, pinId(id))) {
                anchor = b.element;
                break;
            }
    if (!anchor)
        for (const auto& t : ui.tiles)
            if (t.id == id) {
                anchor = t.button;
                break;
            }
    showFolder(id, anchor);
}
static HRESULT WINAPI clickPin(void* self,
                               void* group,
                               void* item,
                               const void* options) {
    try {
        if (openPin(group))
            return S_OK;
    } catch (...) {
    }
    return oldClick(self, group, item, options);
}
static HRESULT WINAPI launchPin(void* self,
                                void* group,
                                const POINT& pt,
                                int flags) {
    try {
        if (openPin(group))
            return S_OK;
    } catch (...) {
    }
    return oldLaunch(self, group, pt, flags);
}
static HRESULT WINAPI jump(void* self, void* group, void* item, bool flag) {
    try {
        if (!stopping && controller && GetCurrentThreadId() == uiThread) {
            auto raw = group && groupId ? groupId(group) : nullptr;
            wstring id = raw ? raw : L"";
            auto pidl = group && groupShortcut ? groupShortcut(group) : nullptr;
            bool sameContext = selection.awaitingGroup &&
                               GetTickCount64() - selection.opened < 1200 &&
                               ((!id.empty() && equal(id, selection.appId)) ||
                                (id.empty() && contextActive));
            if (sameContext) {
                selection.awaitingGroup = false;
                if (pidl)
                    selection.shortcut.reset(ILCloneFull(pidl));
            } else
                stageNative(id, pidl);
        }
    } catch (...) {
        if (controller && GetCurrentThreadId() == uiThread)
            clearSelection();
    }
    return oldJump(self, group, item, flag);
}

// All XAML controls and registrations below belong to the shell's Jump List UI
// thread.
struct JumpFooter {
    C::ListViewBase system{nullptr};
    C::ListView rows{nullptr};
    winrt::event_token clicked{}, unloaded{};
    HWND controller{};
    uint32_t ticket{};
    bool header{};
    C::StackPanel existing{nullptr};
    bool hadLocalNull{};
};
[[clang::no_destroy]] static thread_local JumpFooter footer;
static thread_local HWND hostWindow = nullptr;
static std::mutex hostsLock;
static std::vector<HWND> hosts;
static void clearFooter() {
    auto old = std::exchange(footer, JumpFooter{});
    if (!old.rows)
        return;
    try {
        old.rows.ItemClick(old.clicked);
    } catch (...) {
    }
    try {
        old.system.Unloaded(old.unloaded);
    } catch (...) {
    }
    if (old.existing) {
        uint32_t index{};
        if (old.existing.Children().IndexOf(old.rows, index))
            old.existing.Children().RemoveAt(index);
        return;
    }
    if ((old.header ? old.system.Header() : old.system.Footer()) != old.rows)
        return;
    auto prop = old.header ? C::ListViewBase::HeaderProperty()
                           : C::ListViewBase::FooterProperty();
    if (old.hadLocalNull)
        old.system.SetValue(prop, nullptr);
    else
        old.system.ClearValue(prop);
}
// The system list has a distinctive runtime class. Template variants also
// use the name SystemItemList under SystemItemsContainer.
static C::ListViewBase findSystemList(X::DependencyObject const& node,
                                      bool section = false,
                                      int depth = 0) {
    if (!node || depth > 48)
        return nullptr;
    if (auto element = node.try_as<FrameworkElement>()) {
        if (element.Visibility() == X::Visibility::Collapsed)
            return nullptr;
        auto name = element.Name();
        section = section || name == L"SystemItemsContainer";
        if (auto list = element.try_as<C::ListViewBase>()) {
            if ((section && name == L"SystemItemList") ||
                winrt::get_class_name(element) ==
                    L"JumpViewUI.SystemItemListView")
                return list;
        }
    }
    int count = M::VisualTreeHelper::GetChildrenCount(node);
    for (int i = 0; i < count; ++i)
        if (auto found = findSystemList(M::VisualTreeHelper::GetChild(node, i),
                                        section, depth + 1))
            return found;
    return nullptr;
}
static C::ListViewBase currentSystemList() {
    auto window = X::Window::Current();
    if (!window)
        return nullptr;
    auto root = window.Content();
    // A popup can already be visible while Window.Content is null or still
    // points at the previous page. Search it before the regular tree.
    if (root)
        if (auto element = root.try_as<FrameworkElement>();
            element && element.XamlRoot()) {
            try {
                for (auto p : M::VisualTreeHelper::GetOpenPopupsForXamlRoot(
                         element.XamlRoot()))
                    if (auto list = findSystemList(p.Child()))
                        return list;
            } catch (...) {
            }
        }
    try {
        for (auto p : M::VisualTreeHelper::GetOpenPopups(window))
            if (auto list = findSystemList(p.Child()))
                return list;
    } catch (...) {
    }
    return findSystemList(root);
}
using ReadCaption = int(WINAPI*)(HWND, LPWSTR, int);
static ReadCaption readCaption = reinterpret_cast<ReadCaption>(
    GetProcAddress(GetModuleHandleW(L"user32.dll"), "InternalGetWindowText"));
static bool parseMenuStamp(const wchar_t* text,
                           uint32_t& seq,
                           ULONGLONG& time,
                           const wchar_t*& payload) {
    wchar_t* end = nullptr;
    auto value = wcstoull(text, &end, 10);
    if (!value || value > 0x7ffffffe || *end != L'|')
        return false;
    auto begin = end + 1;
    time = wcstoull(begin, &end, 10);
    if (end == begin || *end != L'|')
        return false;
    seq = static_cast<uint32_t>(value);
    payload = end + 1;
    return true;
}
static C::Grid jumpCommandRow(J::IJsonValue const& value) {
    hstring label;
    FolderAction kind = FolderAction::Move;
    if (value.ValueType() == J::JsonValueType::Array) {
        auto row = value.GetArray();
        label = row.GetStringAt(0);
        auto code = row.GetNumberAt(1);
        if (code >= int(FolderAction::Add) && code <= int(FolderAction::Launch))
            kind = static_cast<FolderAction>(int(code));
    } else {
        label = value.GetString();
        if (label == uiText(UiString::CreateMenu))
            kind = FolderAction::Create;
    }
    C::Grid row;
    C::ColumnDefinition glyph, text;
    glyph.Width({32, X::GridUnitType::Pixel});
    text.Width({1, X::GridUnitType::Star});
    row.ColumnDefinitions().Append(glyph);
    row.ColumnDefinitions().Append(text);
    C::SymbolIcon icon(menuSymbol(kind));
    icon.HorizontalAlignment(X::HorizontalAlignment::Left);
    icon.VerticalAlignment(X::VerticalAlignment::Center);
    X::Automation::AutomationProperties::SetAccessibilityView(
        icon, X::Automation::Peers::AccessibilityView::Raw);
    TextBlock line;
    line.Text(label);
    line.TextTrimming(X::TextTrimming::CharacterEllipsis);
    line.VerticalAlignment(X::VerticalAlignment::Center);
    C::Grid::SetColumn(line, 1);
    row.Children().Append(icon);
    row.Children().Append(line);
    X::Automation::AutomationProperties::SetName(row, label);
    return row;
}
static bool attachJump(HWND ctl) {
    wchar_t prefix[96]{};
    uint32_t seq{};
    ULONGLONG time{};
    const wchar_t* payload = nullptr;
    if (!ctl || !readCaption || !readCaption(ctl, prefix, ARRAYSIZE(prefix)) ||
        !parseMenuStamp(prefix, seq, time, payload)) {
        clearFooter();
        return true;
    }
    if (footer.controller == ctl && footer.ticket == seq && footer.rows &&
        footer.rows.XamlRoot())
        return true;
    clearFooter();
    if (GetTickCount64() - time > 7000)
        return true;
    auto system = currentSystemList();
    if (!system)
        return false;
    auto freeSlot = [&](auto prop) {
        auto local = system.ReadLocalValue(prop);
        return !system.GetValue(prop) &&
               (!local || local == X::DependencyProperty::UnsetValue());
    };
    bool header = !freeSlot(C::ListViewBase::FooterProperty());
    C::StackPanel existing{nullptr};
    if (header && !freeSlot(C::ListViewBase::HeaderProperty())) {
        for (auto slot : {system.Footer(), system.Header()})
            if (slot)
                if (auto stack = slot.try_as<C::StackPanel>()) {
                    if (stack.Orientation() == C::Orientation::Vertical) {
                        existing = stack;
                        break;
                    }
                }
        if (!existing)
            return true;
    }
    std::vector<wchar_t> data(512);
    for (;;) {
        int n = readCaption(ctl, data.data(), static_cast<int>(data.size()));
        if (!n)
            return false;
        if (static_cast<size_t>(n + 1) < data.size())
            break;
        if (data.size() >
            static_cast<size_t>(std::numeric_limits<int>::max() / 2))
            return true;
        data.resize(data.size() * 2);
    }
    uint32_t actual = 0;
    if (!parseMenuStamp(data.data(), actual, time, payload) || actual != seq)
        return false;
    auto labels = J::JsonArray::Parse(payload);
    if (!labels.Size())
        return true;
    C::ListView rows;
    rows.SelectionMode(C::ListViewSelectionMode::None);
    rows.IsItemClickEnabled(true);
    rows.MaxHeight(180);
    rows.HorizontalAlignment(X::HorizontalAlignment::Stretch);
    X::Automation::AutomationProperties::SetName(rows, uiText(UiString::Title));
    for (auto const& value : labels)
        rows.Items().Append(jumpCommandRow(value));
    auto prop = header ? C::ListViewBase::HeaderProperty()
                       : C::ListViewBase::FooterProperty();
    footer = {system, rows,     {},
              {},     ctl,      seq,
              header, existing, !system.ReadLocalValue(prop)};
    try {
        footer.clicked = rows.ItemClick(
            [ctl, seq](auto const& sender, C::ItemClickEventArgs const& args) {
                try {
                    if (stopping)
                        return;
                    auto list = sender.template as<C::ListView>();
                    uint32_t index{};
                    if (list.Items().IndexOf(args.ClickedItem(), index)) {
                        SetForegroundWindow(tray());
                        PostMessageW(ctl, actionMsg, seq,
                                     static_cast<LPARAM>(index) + 1);
                    }
                } catch (...) {
                }
            });
        footer.unloaded =
            system.Unloaded([time](auto const& sender, auto const&) {
                try {
                    if (footer.system == sender) {
                        clearFooter();
                        if (hostWindow && !stopping &&
                            GetTickCount64() - time < 450)
                            PostMessageW(hostWindow, refreshMsg, 0, 0);
                    }
                } catch (...) {
                }
            });
        if (existing)
            existing.Children().Append(rows);
        else if (header)
            system.Header(rows);
        else
            system.Footer(rows);
    } catch (...) {
        clearFooter();
        throw;
    }
    return true;
}
static constexpr UINT_PTR kJumpRetry = 1;
static thread_local unsigned attempts = 0;
static thread_local HWND searchController = nullptr;
static void stopJumpProbe() {
    if (attempts && hostWindow)
        KillTimer(hostWindow, kJumpRetry);
    attempts = 0;
}
static void probeJump() {
    if (!attempts)
        return;
    KillTimer(hostWindow, kJumpRetry);
    bool done = false;
    try {
        done = attachJump(searchController);
    } catch (...) {
    }
    // Attaching a XAML control can pump messages and tear down this host.
    if (stopping || !attempts || !hostWindow)
        return;
    --attempts;
    if (done || !attempts) {
        stopJumpProbe();
        return;
    }
    const UINT delay = attempts > 4 ? 70 : 260;
    if (!SetTimer(hostWindow, kJumpRetry, delay, nullptr))
        stopJumpProbe();
}
static void startJumpProbe() {
    searchController =
        FindWindowExW(HWND_MESSAGE, nullptr, kController, nullptr);
    if (!searchController) {
        stopJumpProbe();
        clearFooter();
        return;
    }
    attempts = 9;
    probeJump();
}
static LRESULT CALLBACK hostProc(HWND w, UINT m, WPARAM wp, LPARAM lp) {
    if (m == kStop) {
        stopJumpProbe();
        try {
            clearFooter();
        } catch (...) {
        }
        DestroyWindow(w);
        hostWindow = nullptr;
        return 0;
    }
    if (!stopping && m == refreshMsg) {
        try {
            startJumpProbe();
        } catch (...) {
        }
        return 0;
    }
    if (!stopping && m == WM_TIMER && wp == kJumpRetry) {
        probeJump();
        return 0;
    }
    if (m == WM_NCDESTROY) {
        std::lock_guard guard(hostsLock);
        std::erase(hosts, w);
    }
    return DefWindowProcW(w, m, wp, lp);
}
static bool ensureHost() {
    if (hostWindow)
        return true;
    WNDCLASSW cls{};
    cls.lpfnWndProc = hostProc;
    cls.hInstance = moduleInstance();
    cls.lpszClassName = kHost;
    if (!RegisterClassW(&cls) && GetLastError() != ERROR_CLASS_ALREADY_EXISTS)
        return false;
    hostWindow = CreateWindowExW(0, kHost, L"", 0, 0, 0, 0, 0, HWND_MESSAGE,
                                 nullptr, cls.hInstance, nullptr);
    if (hostWindow) {
        std::lock_guard guard(hostsLock);
        hosts.push_back(hostWindow);
    }
    return hostWindow != nullptr;
}
static decltype(&DispatchMessageW) oldDispatch;
static LRESULT WINAPI dispatch(const MSG* msg) {
    auto result = oldDispatch(msg);
    if (stopping || !msg || !msg->hwnd || msg->hwnd == hostWindow)
        return result;
    if (msg->message != refreshMsg) {
        switch (msg->message) {
            case WM_SETFOCUS:
                break;
            case WM_SHOWWINDOW:
                if (!msg->wParam)
                    return result;
                break;
            case WM_ACTIVATE:
                if (LOWORD(msg->wParam) == WA_INACTIVE)
                    return result;
                break;
            default:
                return result;
        }
        if (attempts || !GetModuleHandleW(L"JumpViewUI.dll"))
            return result;
        if (!hostWindow && !isJumpThread(GetCurrentThreadId()))
            return result;
    }
    try {
        if (ensureHost())
            startJumpProbe();
    } catch (...) {
    }
    return result;
}

static std::atomic<bool> viewHooks = false, groupHooks = false;
static HMODULE viewModule() {
    auto m = GetModuleHandleW(L"Taskbar.View.dll");
    return m ? m : GetModuleHandleW(L"ExplorerExtensions.dll");
}
static bool wireView(HMODULE m) {
    if (!m || viewHooks.exchange(true))
        return false;
    // Taskbar.View.dll, ExplorerExtensions.dll
    WindhawkUtils::SYMBOL_HOOK viewSymbolHooks[] = {
        {{LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskListButton,struct winrt::Taskbar::ITaskListButton>::get_IsRunning(bool *))"},
         &getButtonRunning,
         nullptr,
         true},
        {{LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskListButton,struct winrt::Taskbar::ITaskListButton>::put_IsRunning(bool))"},
         &setButtonRunning,
         onButtonRunning,
         true},
        {{LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskbarFrame,struct winrt::Windows::UI::Xaml::Controls::IControlOverrides>::OnPointerMoved(void *))"},
         &oldPointer,
         frameMove},
        {{LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskbarFrame,struct winrt::Windows::UI::Xaml::IFrameworkElementOverrides>::OnApplyTemplate(void))"},
         &oldApply,
         frameApply,
         true},
        {{LR"(private: void __cdecl winrt::Taskbar::implementation::TaskListButton::OnContextRequested(struct winrt::Windows::UI::Xaml::UIElement const &,struct winrt::Windows::UI::Xaml::Input::ContextRequestedEventArgs const &))"},
         &oldContext,
         taskContext,
         true},
        {{LR"(void __cdecl winrt::Taskbar::implementation::ContextMenus::ShowTaskbarSettingsContextMenu(struct winrt::Windows::UI::Xaml::FrameworkElement const &,struct winrt::WindowsUdk::UI::Shell::TaskbarSettings const &,struct winrt::Windows::UI::Xaml::Input::ContextRequestedEventArgs const &,unsigned __int64))"},
         &oldBlank,
         blank,
         true},
        {{LR"(public: __cdecl winrt::impl::consume_Windows_Foundation_Collections_IVector<struct winrt::Windows::Foundation::Collections::IVector<struct winrt::Windows::UI::Xaml::Controls::MenuFlyoutItemBase>,struct winrt::Windows::UI::Xaml::Controls::MenuFlyoutItemBase>::Append(struct winrt::Windows::UI::Xaml::Controls::MenuFlyoutItemBase const &)const )"},
         &oldAppend,
         append,
         true}};
    bool ok = WindhawkUtils::HookSymbols(m, viewSymbolHooks,
                                         ARRAYSIZE(viewSymbolHooks));
    if (!ok)
        Wh_Log(L"Taskbar symbols unavailable on this build.");
    return ok;
}
static bool wireGroups(HMODULE m) {
    if (!m || groupHooks.exchange(true))
        return false;
    // taskbar.dll
    WindhawkUtils::SYMBOL_HOOK groupSymbolHooks[] = {
        {{LR"(public: virtual long __cdecl CTaskGroup::GetToolTipText(struct ITaskItem *,unsigned short *,int))"},
         &oldGroupTip,
         groupTip,
         true},
        {{LR"(public: virtual long __cdecl CTaskGroup::GetLauncherName(unsigned short * *))"},
         &oldLauncherName,
         launcherName,
         true},
        {{LR"(public: virtual long __cdecl CTaskListWnd::HandleClick(struct ITaskGroup *,struct ITaskItem *,struct winrt::Windows::System::LauncherOptions const &))"},
         &oldClick,
         clickPin,
         true},
        {{LR"(public: virtual long __cdecl CTaskBand::Launch(struct ITaskGroup *,struct tagPOINT const &,enum LaunchFromTaskbarOptions))"},
         &oldLaunch,
         launchPin,
         true},
        {{LR"(public: virtual unsigned short const * __cdecl CTaskGroup::GetAppID(void))"},
         &groupId},
        {{LR"(public: virtual struct _ITEMIDLIST_ABSOLUTE const * __cdecl CTaskGroup::GetShortcutIDList(void))"},
         &groupShortcut,
         nullptr,
         true},
        {{LR"(public: virtual long __cdecl CTaskListWnd::ShowJumpView(struct ITaskGroup *,struct ITaskItem *,bool))"},
         &oldJump,
         jump,
         true}};
    bool ok = WindhawkUtils::HookSymbols(m, groupSymbolHooks,
                                         ARRAYSIZE(groupSymbolHooks));
    nativeReady = ok && groupId && (oldClick || oldLaunch);
    return ok;
}
static decltype(&LoadLibraryExW) oldLoad;
static HMODULE WINAPI onLoad(LPCWSTR name, HANDLE file, DWORD flags) {
    auto m = oldLoad(name, file, flags);
    if (m && !stopping && !viewHooks && m == viewModule() && wireView(m))
        Wh_ApplyHookOperations();
    if (m && !stopping && !groupHooks &&
        m == GetModuleHandleW(L"taskbar.dll") && wireGroups(m))
        Wh_ApplyHookOperations();
    return m;
}
static bool explorer = false;
BOOL Wh_ModInit() {
    readAppearanceSettings();
    wchar_t path[MAX_PATH]{};
    GetModuleFileNameW(nullptr, path, MAX_PATH);
    explorer = equal(fs::path(path).filename().wstring(), L"explorer.exe");
    if (explorer) {
        modalDrained = CreateEventW(nullptr, TRUE, FALSE, nullptr);
        if (!modalDrained)
            return FALSE;
    }
    bool messages = WindhawkUtils::SetFunctionHook(DispatchMessageW, dispatch,
                                                   &oldDispatch);
    if (!explorer)
        return messages;
    auto kb = GetModuleHandleW(L"kernelbase.dll");
    auto load = reinterpret_cast<decltype(&LoadLibraryExW)>(
        GetProcAddress(kb, "LoadLibraryExW"));
    bool loader =
        load && WindhawkUtils::SetFunctionHook(load, onLoad, &oldLoad);
    bool ok = wireView(viewModule());
    wireGroups(GetModuleHandleW(L"taskbar.dll"));
    if (!ok && !loader) {
        CloseHandle(std::exchange(modalDrained, nullptr));
        return FALSE;
    }
    return TRUE;
}
void Wh_ModSettingsChanged() {
    readAppearanceSettings();
    if (auto w = published.load(); w && !stopping)
        PostMessageW(w, kAppearanceChanged, 0, 0);
}
void Wh_ModAfterInit() {
    if (explorer) {
        if (wireView(viewModule()))
            Wh_ApplyHookOperations();
        if (wireGroups(GetModuleHandleW(L"taskbar.dll")))
            Wh_ApplyHookOperations();
    }
}
void Wh_ModBeforeUninit() {
    stopping = true;
    if (auto w = published.load()) {
        if (SendMessageW(w, kCancelModal, 0, 0))
            WaitForSingleObject(modalDrained, INFINITE);
        SendMessageW(w, kStop, 0, 0);
    }
    stopWorkers();
    std::vector<HWND> copy;
    {
        std::lock_guard guard(hostsLock);
        copy = hosts;
    }
    for (auto w : copy)
        if (IsWindow(w))
            SendMessageW(w, kStop, 0, 0);
}
void Wh_ModUninit() {
    if (modalDrained)
        CloseHandle(std::exchange(modalDrained, nullptr));
    UnregisterClassW(kFileSurfaceClass, moduleInstance());
    UnregisterClassW(kFilesClass, moduleInstance());
    UnregisterClassW(kPanelClass, moduleInstance());
    UnregisterClassW(kController, moduleInstance());
    UnregisterClassW(kHost, moduleInstance());
}
