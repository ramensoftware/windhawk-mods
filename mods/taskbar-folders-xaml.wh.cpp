// ==WindhawkMod==
// @id              taskbar-folders-xaml
// @name            Windows 11 Taskbar Folders
// @name:it-IT      Cartelle nella taskbar di Windows 11
// @name:es-ES      Carpetas en la barra de tareas
// @name:fr-FR      Dossiers dans la barre des tâches
// @name:de-DE      Ordner in der Taskleiste
// @description     Group apps into folders on the Windows 11 taskbar. Preview their icons, open a folder with one click, and drag apps to reorder them.
// @description:it-IT Raggruppa le app della taskbar di Windows 11 in cartelle con anteprime. Aprile con un clic e trascina le app per riordinarle.
// @description:es-ES Agrupa aplicaciones en carpetas en la barra de tareas de Windows 11. Abre una carpeta con un clic y arrastra las aplicaciones para reordenarlas.
// @description:fr-FR Regroupez vos applications dans des dossiers sur la barre des tâches de Windows 11. Un clic ouvre un dossier. Faites glisser les applications pour les réorganiser.
// @description:de-DE Gruppieren Sie Apps in Ordnern auf der Windows-11-Taskleiste. Ein Klick öffnet den Ordner. Ziehen Sie die Apps zum Sortieren an die gewünschte Position.
// @version         0.7.0
// @author          mamt104
// @github          https://github.com/mamt104
// @license         MIT
// @include         explorer.exe
// @include         ShellExperienceHost.exe
// @include         ShellHost.exe
// @include         ShellAppRuntime.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lruntimeobject -lshell32 -lshlwapi -luuid -lgdi32 -lcomctl32 -lcomdlg32 -ldwmapi -lmsimg32 -ladvapi32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Windows 11 Taskbar Folders (x64)

![Example folder panel above its taskbar tile](https://raw.githubusercontent.com/mamt104/windhawk-mods/375fdc4ffb3f53ef02751e3e988a92d783723d69/windows-11-taskbar-folders-preview.png)

**English** — Right-click an empty part of the taskbar and select **Create folder…**. Open a folder with one click. Drag shortcuts from the desktop or File Explorer into the open folder. Use **+** to search installed apps or choose a file. Drag apps to reorder them. In an app’s Windows menu, choose **Move to** followed by the folder name. The folder’s **⋯** menu lets you rename or remove it.

**Italiano** — Fai clic destro su uno spazio vuoto della barra e scegli **Crea cartella…**. Apri la cartella con un clic. Trascina i collegamenti dal desktop o da Esplora file nella cartella aperta. Con **+** puoi cercare le app installate o scegliere un file. Trascina le app per riordinarle. Nel menu di Windows di un’app scegli **Sposta in** seguito dal nome della cartella. Dal menu **⋯** puoi rinominare o rimuovere la cartella.

**Español** — Haz clic derecho en un espacio vacío de la barra y elige **Crear carpeta…**. Abre la carpeta con un clic. Arrastra accesos directos desde el escritorio o el Explorador de archivos a la carpeta abierta. Usa **+** para buscar aplicaciones instaladas o elegir un archivo. Arrastra las aplicaciones para reordenarlas. En el menú de Windows de una aplicación, elige **Mover a** y el nombre de la carpeta. Desde **⋯** puedes cambiar el nombre de la carpeta o quitarla.

**Français** — Faites un clic droit sur un espace vide de la barre et choisissez **Créer un dossier…**. Un clic ouvre le dossier. Faites glisser des raccourcis depuis le bureau ou l’Explorateur de fichiers vers le dossier ouvert. Avec **+**, recherchez une application installée ou choisissez un fichier. Faites glisser les applications pour les réorganiser. Dans le menu Windows d’une application, choisissez **Déplacer vers** suivi du nom du dossier. Le menu **⋯** permet de renommer ou de supprimer le dossier.

**Deutsch** — Klicken Sie mit der rechten Maustaste auf eine freie Stelle der Taskleiste und wählen Sie **Ordner erstellen…**. Ein Klick öffnet den Ordner. Ziehen Sie Verknüpfungen vom Desktop oder aus dem Datei-Explorer in den geöffneten Ordner. Mit **+** suchen Sie installierte Apps oder wählen eine Datei aus. Ziehen Sie Apps an ihre gewünschte Position. Im Windows-Menü einer App wählen Sie **Nach „Ordnername“ verschieben**. Über **⋯** können Sie den Ordner umbenennen oder entfernen.

| | English | Italiano | Español | Français | Deutsch |
|---|---|---|---|---|---|
| **PgUp / PgDn** | Change page. While dragging, pause at a side edge to change pages. | Cambia pagina. Durante un trascinamento, fermati su un bordo laterale per cambiare pagina. | Cambia de página. Al arrastrar, detente en un borde lateral para pasar de página. | Change de page. Pendant un glisser-déposer, marquez une pause sur un bord latéral pour changer de page. | Blättert eine Seite. Beim Ziehen kurz am seitlichen Rand verweilen, um umzublättern. |
### Compatibility

Folders appear on the primary Windows 11 taskbar only. The mod groups existing app buttons and adds commands to the taskbar and app context menus. Taskbar Mini Folders offers its own independent launcher tiles.

The mod keeps each Windows account's folder layout in Windhawk's mod storage. Existing folder layouts from the pre-release builds are copied into that storage on first use. Older pre-release shortcuts can be unpinned and removed in Windows.
*/
// ==/WindhawkModReadme==

#include <windhawk_utils.h>
#include <shellapi.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <sddl.h>
#include <propkey.h>
#include <array>
#include <thread>
#include <cmath>
#include <shobjidl.h>
#include <commdlg.h>
#include <commctrl.h>
#include <dwmapi.h>
#include <windowsx.h>
#include <wrl/client.h>
#include <robuffer.h>
#undef GetCurrentTime
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Data.Json.h>
#include <winrt/Windows.ApplicationModel.DataTransfer.h>
#include <winrt/Windows.Storage.h>
#include <winrt/Windows.Storage.Streams.h>
#include <winrt/Windows.UI.h>
#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.UI.Input.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Controls.Primitives.h>
#include <winrt/Windows.UI.Xaml.Input.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.Data.h>
#include <winrt/Windows.UI.Xaml.Media.Imaging.h>
#include <winrt/Windows.UI.Xaml.Automation.h>
#include <winrt/Windows.UI.Xaml.Automation.Peers.h>
#include <algorithm>
#include <atomic>
#include <filesystem>
#include <fstream>
#include <functional>
#include <map>
#include <mutex>
#include <memory>
#include <limits>
#include <strsafe.h>
#include <optional>
#include <set>
#include <string>
#include <string_view>
#include <vector>
#include <utility>

namespace fs = std::filesystem;
namespace W = winrt::Windows;
namespace X = W::UI::Xaml;
namespace C = X::Controls;
namespace P = C::Primitives;
namespace M = X::Media;
namespace I = X::Input;
namespace F = W::Foundation;
namespace J = W::Data::Json;
namespace Data = W::ApplicationModel::DataTransfer;
using X::FrameworkElement;
using C::Button;
using C::TextBlock;
using std::wstring;
using winrt::hstring;
using winrt::com_ptr;

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
    SaveFailed,
    Save,
    Cancel,
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
    MoveNamed,
    RenameTitle,
    RemovePrompt,
    RemoveApp,
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
    AppMissing,
    Count
};
static constexpr PCWSTR strings[][5] = {
    {L"Taskbar Folders", L"Cartelle nella taskbar",
     L"Carpetas en la barra de tareas", L"Dossiers dans la barre des tâches",
     L"Ordner in der Taskleiste"},  // Title
    {L"Couldn’t read cartelle.json. Check the file and its .bak copy before changing your folders.",
     L"Impossibile leggere cartelle.json. Controlla il file e la copia .bak prima di modificare le cartelle.",
     L"No se ha podido leer cartelle.json. Revisa el archivo y su copia .bak antes de cambiar las carpetas.",
     L"Impossible de lire cartelle.json. Vérifiez le fichier et sa copie .bak avant de modifier vos dossiers.",
     L"cartelle.json konnte nicht gelesen werden. Prüfen Sie die Datei und ihre .bak-Kopie, bevor Sie Ordner ändern."},  // ConfigUnreadable
    {L"Windows couldn’t save the change. Your previous folders are still active.",
     L"Windows non ha salvato la modifica. Le cartelle precedenti sono ancora attive.",
     L"Windows no ha podido guardar el cambio. Se conservan las carpetas anteriores.",
     L"Windows n’a pas pu enregistrer la modification. Vos dossiers précédents sont toujours disponibles.",
     L"Windows konnte die Änderung nicht speichern. Ihre bisherigen Ordner sind weiterhin verfügbar."},  // SaveFailed
    {L"Save", L"Salva", L"Guardar", L"Enregistrer", L"Speichern"},   // Save
    {L"Cancel", L"Annulla", L"Cancelar", L"Annuler", L"Abbrechen"},  // Cancel
    {L"Create a taskbar folder", L"Crea cartella nella taskbar",
     L"Crear una carpeta en la barra de tareas",
     L"Créer un dossier dans la barre des tâches",
     L"Taskleistenordner erstellen"},  // CreateTitle
    {L"Can’t open this folder. Check the path or choose another location.",
     L"Cartella non accessibile. Controlla il percorso o scegli un'altra posizione.",
     L"No se puede abrir esta carpeta. Comprueba la ruta o elige otra ubicación.",
     L"Impossible d’ouvrir ce dossier. Vérifiez le chemin ou choisissez un autre emplacement.",
     L"Der Ordner lässt sich nicht öffnen. Prüfen Sie den Pfad oder wählen Sie einen anderen Speicherort."},  // DirUnavailable
    {L"No folders or apps here.",
     L"Nessuna cartella o app in questa posizione.",
     L"Aquí no hay carpetas ni aplicaciones.",
     L"Aucun dossier ni aucune application à cet emplacement.",
     L"Hier gibt es keine Ordner oder Apps."},  // DirEmpty
    {L"Select apps. Use Ctrl or Shift to select more than one.",
     L"Seleziona le app. Usa Ctrl o Maiusc per sceglierne più di una.",
     L"Selecciona las aplicaciones. Usa Ctrl o Mayús para seleccionar varias.",
     L"Sélectionnez les applications. Utilisez Ctrl ou Maj pour en choisir plusieurs.",
     L"Wählen Sie Apps aus. Mit Strg oder Umschalt können Sie mehrere auswählen."},  // MultiSelect
    {L"Select an app or open a folder.",
     L"Seleziona un'app o apri una cartella.",
     L"Selecciona una aplicación o abre una carpeta.",
     L"Sélectionnez une application ou ouvrez un dossier.",
     L"Wählen Sie eine App aus oder öffnen Sie einen Ordner."},  // ChooseApp
    {L"Couldn’t read this location.", L"Impossibile leggere questa posizione.",
     L"No se ha podido leer esta ubicación.",
     L"Impossible de lire cet emplacement.",
     L"Dieser Speicherort konnte nicht gelesen werden."},  // DirReadFailed
    {L"Locations", L"Posizioni", L"Ubicaciones", L"Emplacements",
     L"Speicherorte"},                             // Places
    {L"Up", L"Su", L"Subir", L"Monter", L"Hoch"},  // Up
    {L"Go", L"Vai", L"Ir", L"Aller", L"Los"},      // Go
    {L"Folders and apps", L"Cartelle e app", L"Carpetas y aplicaciones",
     L"Dossiers et applications", L"Ordner und Apps"},            // FoldersApps
    {L"Add", L"Aggiungi", L"Añadir", L"Ajouter", L"Hinzufügen"},  // Add
    {L"This PC", L"Questo PC", L"Este equipo", L"Ce PC",
     L"Dieser PC"},                                                  // ThisPc
    {L"Desktop", L"Desktop", L"Escritorio", L"Bureau", L"Desktop"},  // Desktop
    {L"Downloads", L"Download", L"Descargas", L"Téléchargements",
     L"Downloads"},  // Downloads
    {L"My Start menu", L"Menu Start personale", L"Mi menú Inicio",
     L"Mon menu Démarrer", L"Mein Startmenü"},  // PersonalStart
    {L"Shared Start menu", L"Menu Start condiviso", L"Menú Inicio compartido",
     L"Menu Démarrer partagé", L"Gemeinsames Startmenü"},  // SharedStart
    {L"Choose from file", L"Scegli da file", L"Elegir un archivo",
     L"Choisir un fichier", L"Datei auswählen"},  // FromFile
    {L"Add apps…", L"Aggiungi app…", L"Añadir aplicaciones…",
     L"Ajouter des applications…", L"Apps hinzufügen…"},  // AddAppsMenu
    {L"Rename folder…", L"Rinomina cartella…", L"Cambiar nombre de carpeta…",
     L"Renommer le dossier…", L"Ordner umbenennen…"},  // RenameMenu
    {L"Create folder…", L"Crea cartella…", L"Crear carpeta…",
     L"Créer un dossier…", L"Ordner erstellen…"},  // CreateMenu
    {L"Remove folder…", L"Rimuovi cartella…", L"Quitar carpeta…",
     L"Supprimer le dossier…", L"Ordner entfernen…"},  // RemoveMenu
    {L"Move to “{0}”", L"Sposta in «{0}»", L"Mover a «{0}»",
     L"Déplacer vers «\u00A0{0}\u00A0»", L"Nach „{0}“ verschieben"},  // MoveNamed
    {L"Rename folder", L"Rinomina cartella", L"Cambiar nombre de carpeta",
     L"Renommer le dossier", L"Ordner umbenennen"},  // RenameTitle
    {L"Remove this folder and show its pinned apps on the taskbar again?",
     L"Rimuovere questa cartella e mostrare di nuovo i suoi pin sulla barra?",
     L"¿Quitar esta carpeta y volver a mostrar sus aplicaciones ancladas en la barra de tareas?",
     L"Supprimer ce dossier et afficher de nouveau ses applications épinglées dans la barre des tâches\u00A0?",
     L"Diesen Ordner entfernen und seine angehefteten Apps wieder in der Taskleiste anzeigen?"},  // RemovePrompt
    {L"Remove from folder", L"Rimuovi dalla cartella", L"Quitar de la carpeta",
     L"Retirer du dossier", L"Aus Ordner entfernen"},  // RemoveApp
    {L"Search installed apps", L"Cerca app installate",
     L"Buscar aplicaciones instaladas",
     L"Rechercher des applications installées",
     L"Installierte Apps suchen"},  // SearchInstalled
    {L"Search for an installed app", L"Cerca un’app installata",
     L"Busca una aplicación instalada", L"Rechercher une application installée",
     L"Eine installierte App suchen"},  // SearchHint
    {L"Previous page", L"Pagina precedente", L"Página anterior",
     L"Page précédente", L"Vorherige Seite"},  // PreviousPage
    {L"Next page", L"Pagina successiva", L"Página siguiente", L"Page suivante",
     L"Nächste Seite"},  // NextPage
    {L"Page {0} of {1}", L"Pagina {0} di {1}", L"Página {0} de {1}",
     L"Page {0} sur {1}", L"Seite {0} von {1}"},  // PageOf
    {L"Add to {0}", L"Aggiungi a {0}", L"Añadir a {0}", L"Ajouter à {0}",
     L"Zu „{0}“ hinzufügen"},                                // AddTo
    {L"Back", L"Indietro", L"Atrás", L"Retour", L"Zurück"},  // Back
    {L"More options", L"Altre opzioni", L"Más opciones", L"Autres options",
     L"Weitere Optionen"},  // Options
    {L"Add apps", L"Aggiungi app", L"Añadir aplicaciones",
     L"Ajouter des applications", L"Apps hinzufügen"},  // AddApps
    {L"Empty folder", L"Cartella vuota", L"Carpeta vacía", L"Dossier vide",
     L"Leerer Ordner"},  // Empty
    {L"Search for an app or choose a file", L"Cerca un’app o scegli un file",
     L"Busca una aplicación o elige un archivo",
     L"Recherchez une application ou choisissez un fichier",
     L"App suchen oder Datei auswählen"},  // TypeOrBrowse
    {L"Searching installed apps…", L"Ricerca delle app installate…",
     L"Buscando aplicaciones instaladas…",
     L"Recherche des applications installées…",
     L"Installierte Apps werden gesucht…"},  // Searching
    {L"App list unavailable. You can still choose a file.",
     L"Catalogo non disponibile. Puoi scegliere un file.",
     L"La lista de aplicaciones no está disponible. Puedes elegir un archivo.",
     L"La liste des applications est indisponible. Vous pouvez choisir un fichier.",
     L"Die App-Liste ist nicht verfügbar. Sie können eine Datei auswählen."},  // NoCatalog
    {L"No apps found", L"Nessuna app trovata",
     L"No se han encontrado aplicaciones", L"Aucune application trouvée",
     L"Keine Apps gefunden"},  // NoResults
    {L"Couldn’t open app search.", L"Impossibile aprire la ricerca delle app.",
     L"No se ha podido abrir la búsqueda de aplicaciones.",
     L"Impossible d’ouvrir la recherche d’applications.",
     L"Die App-Suche konnte nicht geöffnet werden."},  // SearchFailed
    {L"The selection contains no usable app or shortcut.",
     L"La selezione non contiene un’app o un collegamento utilizzabile.",
     L"La selección no contiene aplicaciones ni accesos directos válidos.",
     L"La sélection ne contient aucune application ni aucun raccourci utilisable.",
     L"Die Auswahl enthält keine verwendbare App oder Verknüpfung."},  // NoUsableFile
    {L"Windows couldn’t open the file picker.",
     L"Windows non ha potuto creare la finestra dei file.",
     L"Windows no ha podido abrir el selector de archivos.",
     L"Windows n’a pas pu ouvrir le sélecteur de fichiers.",
     L"Windows konnte die Dateiauswahl nicht öffnen."},  // PickerFailed
    {L"Shortcut unavailable. Check that the app is still installed.",
     L"Collegamento non disponibile. Controlla che l’app sia ancora installata.",
     L"Acceso directo no disponible. Comprueba que la aplicación sigue instalada.",
     L"Raccourci indisponible. Vérifiez que l’application est toujours installée.",
     L"Verknüpfung nicht verfügbar. Prüfen Sie, ob die App noch installiert ist."},  // BrokenShortcut
    {L"Applications", L"Applicazioni", L"Aplicaciones", L"Applications",
     L"Anwendungen"},  // Apps
    {L"Windows couldn’t find the shortcut for this app.",
     L"Windows non ha trovato il collegamento di questa app.",
     L"Windows no ha encontrado el acceso directo de esta aplicación.",
     L"Windows n’a pas trouvé le raccourci de cette application.",
     L"Windows konnte die Verknüpfung für diese App nicht finden."},  // AppMissing
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
    bool operator==(const App&) const = default;
};
struct AppFolder {
    wstring id, name;
    std::vector<App> apps;
    bool operator==(const AppFolder&) const = default;
};
using Model = std::vector<AppFolder>;
static Model groups;
static fs::path config;
static bool loaded = false, locked = false;
static std::atomic<bool> stopping = false;
static void note(const wchar_t* text);
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
    else if (i->id.empty() && !a.id.empty())
        *i = a;
}
static std::string encode(const Model& m) {
    J::JsonArray xs;
    for (auto& f : m) {
        J::JsonObject x;
        J::JsonArray items;
        x.SetNamedValue(L"id", J::JsonValue::CreateStringValue(f.id));
        x.SetNamedValue(L"name", J::JsonValue::CreateStringValue(f.name));
        for (auto& a : f.apps) {
            J::JsonObject v;
            v.SetNamedValue(L"id", J::JsonValue::CreateStringValue(a.id));
            v.SetNamedValue(L"name", J::JsonValue::CreateStringValue(a.name));
            v.SetNamedValue(L"target",
                            J::JsonValue::CreateStringValue(a.target));
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
        if (f.id.empty() || f.name.empty() || !ids.insert(f.id).second)
            throw std::runtime_error("folder");
        for (auto item : v.GetNamedArray(L"apps")) {
            auto a = item.GetObject();
            App app{wstring(a.GetNamedString(L"id")),
                    wstring(a.GetNamedString(L"name")),
                    wstring(a.GetNamedString(L"target"))};
            if (app.target.empty() || app.name.empty())
                throw std::runtime_error("item");
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
static fs::path knownPath(REFKNOWNFOLDERID k);

static wstring userSid() {
    winrt::handle token;
    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, token.put()))
        winrt::throw_last_error();

    DWORD size = 0;
    GetTokenInformation(token.get(), TokenUser, nullptr, 0, &size);
    if (GetLastError() != ERROR_INSUFFICIENT_BUFFER || size < sizeof(TOKEN_USER))
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

static void persist(const Model& m);
static void loadModel() {
    if (loaded)
        return;
    loaded = true;
    try {
        // The API requires a buffer, including room for the terminator.
        wstring storage(32768, L'\0');
        const auto length = Wh_GetModStoragePath(storage.data(), storage.size());
        if (!length)
            throw std::runtime_error("Windhawk storage");
        storage.resize(length);

        auto directory = fs::path(storage) / userSid();
        fs::create_directories(directory);
        config = directory / L"folders.json";
        if (fs::exists(config)) {
            groups = decode(bytes(config));
            return;
        }

        // The local pre-release used this path. Read it without changing it.
        auto legacy = knownPath(FOLDERID_LocalAppData) / L"TaskbarFolders" /
                      L"Xaml" / L"cartelle.json";
        if (fs::exists(legacy)) {
            groups = decode(bytes(legacy));
            persist(groups);
        }
    } catch (...) {
        locked = true;
        Wh_Log(L"Could not read the folder configuration; saving is disabled.");
    }
}
static void persist(const Model& m) {
    const bool exists = fs::exists(config);
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
    if (ok && exists) {
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
        note(uiText(UiString::ConfigUnreadable));
        return false;
    }
    try {
        auto next = groups;
        change(next);
        if (next == groups)
            return true;
        persist(next);
        groups = std::move(next);
        ++revision;
        requestLayout();
        return true;
    } catch (...) {
        note(uiText(UiString::SaveFailed));
        return false;
    }
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
    if (FAILED(
            SHParseDisplayName(app.target.c_str(), nullptr, &pidl, 0, nullptr)))
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
    Drops op;
};
struct Hidden {
    winrt::weak_ref<FrameworkElement> item;
    F::IInspectable old{nullptr};
};
struct Tile {
    Button button{nullptr};
    wstring id;
};
struct Faded {
    winrt::weak_ref<FrameworkElement> e;
    F::IInspectable old;
};
struct View {
    HWND primary = nullptr;
    HMONITOR monitor = nullptr;
    FrameworkElement frame{nullptr}, repeater{nullptr};
    C::Grid root{nullptr};
    C::Canvas overlay{nullptr};
    C::StackPanel strip{nullptr};
    bool layoutPending = false, rootDetached = false;
    std::vector<std::function<void()>> frameOff;
    std::vector<Tile> tiles;
    std::vector<Hidden> hidden;
    std::vector<Faded> faded;
    std::vector<DropJob> drops;
    std::vector<std::function<void()>> tileOff, menuOff;
    unsigned painted = ~0u;
    X::Thickness baseMargin{};
    bool marginSet = false;
    double reserved = 0;
    wstring dragApp;
    POINT down{};
};
// Explorer can exit without calling Wh_ModUninit. Release the XAML references
// explicitly on the thread that owns them when the mod is disabled.
[[clang::no_destroy]] static std::optional<View> ui;
static HWND controller = nullptr;
static std::atomic<HWND> published = nullptr;
static DWORD uiThread = 0;
static constexpr wchar_t kController[] = L"TaskbarFoldersXaml.Controller.065";
static constexpr wchar_t kHost[] = L"TaskbarFoldersXaml.JumpView.065";
static const UINT actionMsg =
    RegisterWindowMessageW(L"TaskbarFoldersXaml.Action.065");
static const UINT refreshMsg =
    RegisterWindowMessageW(L"TaskbarFoldersXaml.Refresh.065");
enum : UINT { kStop = WM_APP + 321, kCancelModal, kModalDrained };
enum : UINT_PTR { kLayoutTimer = 0x7466, kDropTimer, kPinDragTimer, kModalDrainTimer };
static void requestLayout() {
    if (!stopping && controller && ui && !ui->layoutPending)
        ui->layoutPending = SetTimer(controller, kLayoutTimer, 65, nullptr) != 0;
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
    if (!ui || !ui->root || ui->rootDetached || !element ||
        ui->primary != tray() ||
        ui->monitor != MonitorFromWindow(ui->primary, MONITOR_DEFAULTTONULL))
        return false;
    auto root = ui->root.XamlRoot();
    return root && element.XamlRoot() == root;
}

static void closePanel();
static HWND panelOwner();
static HINSTANCE moduleInstance();

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
        auto scaled = [&](int value) { return MulDiv(value, dpi ? dpi : 96, 96); };
        const int pad = scaled(12), height = scaled(28), button = scaled(78);
        auto addButton = [&](int id, const wchar_t* label, int x, DWORD style) {
            return CreateWindowW(
                L"BUTTON", label, WS_CHILD | WS_VISIBLE | WS_TABSTOP | style,
                x, r.bottom - pad - height, button, height, w,
                reinterpret_cast<HMENU>(static_cast<INT_PTR>(id)), nullptr, nullptr);
        };
        HWND content = nullptr;
        if (input->text) {
            content = CreateWindowW(
                L"STATIC", input->text, WS_CHILD | WS_VISIBLE | SS_LEFT | SS_NOPREFIX,
                pad, pad, r.right - 2 * pad, r.bottom - height - 3 * pad,
                w, nullptr, nullptr, nullptr);
        } else {
            content = CreateWindowExW(
                WS_EX_CLIENTEDGE, L"EDIT", input->initial.c_str(),
                WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_AUTOHSCROLL, pad, pad,
                r.right - 2 * pad, height, w, (HMENU)100, nullptr, nullptr);
        }
        const bool hasCancel = !input->text || input->confirm;
        int right = r.right - pad - button;
        HWND cancel = hasCancel ? addButton(IDCANCEL, uiText(UiString::Cancel), right, 0)
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
        auto input = reinterpret_cast<DialogInput*>(GetWindowLongPtrW(w, DWLP_USER));
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
            input->result = value.substr(first, value.find_last_not_of(L" \r\n\t") - first + 1);
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
    auto result = DialogBoxIndirectParamW(moduleInstance(), &t.d, tray(), dialogProc,
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
// Ignore folder shortcuts left by local pre-release builds.
static constexpr wchar_t kPinPrefix[] = L"TaskbarFolders.Xaml.";
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
// The pinned shortcut directory is unchanged for the duration of a typical popup.
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
    auto path = fs::path(app.target);
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
    auto item = shellItem(a.target);
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
[[clang::no_destroy]] static std::optional<std::map<wstring, Preview>> previews;
static const Preview& previewFor(const AppFolder& folder) {
    if (!previews)
        previews.emplace();
    auto& cached = (*previews)[folder.id];
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
static void createFolder() {
    loadModel();
    auto n = nameDialog(L"", uiText(UiString::CreateTitle));
    if (n.empty())
        return;
    edit([&](Model& m) { m.push_back({guid(), n, {}}); });
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
    UINT show(HWND owner = nullptr) {
        POINT pt{};
        GetCursorPos(&pt);
        if (!owner)
            owner = tray();
        if (!h || !owner)
            return 0;
        ModalScope modal(ModalKind::Menu);
        if (!modal)
            return 0;
        SetForegroundWindow(owner);
        if (stopping)
            return 0;
        auto result = TrackPopupMenuEx(
            h, TPM_RETURNCMD | TPM_NONOTIFY | TPM_RIGHTBUTTON, pt.x, pt.y, owner,
            nullptr);
        return stopping ? 0 : result;
    }
};
static void moveTo(const wstring& dest, const App& a) {
    edit([&](Model& m) { transfer(m, dest, a); });
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
// File picking runs on its own short-lived UI thread. The taskbar remains interactive.
enum : UINT { kFilesDone = WM_APP + 540, kCatalogDone };
static constexpr wchar_t kFilesClass[] = L"TaskbarFolders.Files.052";
[[clang::no_destroy]] static std::optional<std::thread> fileThread;
[[clang::no_destroy]] static std::optional<std::thread> catalogThread;
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
    if (fileThread) {
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
    fileThread.emplace([notify, anchor, title] {
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
    if (!catalogThread || !catalogReady.load())
        return false;
    catalogThread->join();
    catalogThread.reset();
    ++catalogGeneration;
    installedApps = std::move(incomingApps);
    catalogFailed = incomingFailed;
    catalogReady = false;
    catalogTime = GetTickCount64();
    return true;
}
static void wantCatalog(HWND notify) {
    collectCatalog();
    if (stopping || !notify || catalogThread ||
        (catalogTime &&
         GetTickCount64() - catalogTime < (catalogFailed ? 10000 : 120000)))
        return;
    catalogThread.emplace([notify] {
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
        size_t count =
            1;  // Key zero means an app doesn't have an explicit AppUserModelID.
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

static void stopWorkers() {
    if (HWND w = fileWindow.load())
        PostMessageW(w, WM_CLOSE, 0, 0);
    if (fileThread) {
        fileThread->join();
        fileThread.reset();
    }
    if (catalogThread) {
        catalogThread->join();
        catalogThread.reset();
    }
}

static void beginAdding(const wstring&);
enum class FolderAction { Add = 1, Rename, Create, Remove, Move };
struct MenuAction {
    FolderAction kind;
    wstring folder;
    wstring label;
};
static std::vector<MenuAction> folderActions(const wstring& dest) {
    return {{FolderAction::Add, dest, uiText(UiString::AddAppsMenu)},
            {FolderAction::Rename, dest, uiText(UiString::RenameMenu)},
            {FolderAction::Create, {}, uiText(UiString::CreateMenu)},
            {FolderAction::Remove, dest, uiText(UiString::RemoveMenu)}};
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
    wstring name = f->name;
    closePanel();
    switch (action.kind) {
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
                edit([&](Model& m) {
                    std::erase_if(m, [&](const AppFolder& folder) {
                        return folder.id == action.folder;
                    });
                });
            break;
        default:
            break;
    }
}
// This menu belongs to the three-dot button inside our own folder panel.
static void manage(wstring dest) {
    if (!lookup(groups, dest))
        return;
    auto actions = folderActions(dest);
    Menu menu;
    for (size_t i = 0; i < actions.size(); ++i) {
        if (actions[i].kind == FolderAction::Remove)
            menu.sep();
        menu.item(static_cast<UINT>(i + 1), actions[i].label);
    }
    auto n = menu.show(panelOwner());
    if (n && n <= actions.size())
        runFolderAction(actions[n - 1]);
}

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
static void appMenu(wstring dest, App a) {
    Menu menu;
    menu.item(1, uiText(UiString::RemoveApp));
    std::vector<wstring> ids;
    for (auto& f : groups)
        if (f.id != dest) {
            ids.push_back(f.id);
            menu.item(1 + (UINT)ids.size(),
                      formatUi(UiString::MoveNamed, f.name));
        }
    auto cmd = menu.show(panelOwner());
    if (!cmd)
        return;
    closePanel();
    if (cmd == 1)
        edit([&](Model& m) {
            if (auto f = lookup(m, dest))
                std::erase_if(f->apps,
                              [&](auto& old) { return sameApp(old, a); });
        });
    else if (cmd - 2 < ids.size())
        moveTo(ids[cmd - 2], a);
}
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
    kOpenApp = WM_APP + 501,
    kClosePanel,
    kCheckActivation
};
struct Ease {
    double start{}, end{};
    ULONGLONG since{};
    unsigned duration{};
    bool active(ULONGLONG now) const {
        return duration && now - since < duration;
    }
    double value(ULONGLONG now) const {
        if (!active(now))
            return end;
        double left = 1.0 - double(now - since) / duration;
        return end + (start - end) * left * left * left;
    }
    bool damage(ULONGLONG now, bool& active) {
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
    void to(double v, unsigned ms, ULONGLONG now) {
        if (v == end)
            return;
        start = value(now);
        end = v;
        since = now;
        duration = start == end ? 0 : ms;
    }
};
struct TileMotion {
    Ease hover, press, dx, dy;
};
enum : UINT_PTR { kFrameTimer = 87, kDragPageTimer = 88, kDropActivationTimer = 89 };
static bool allowMotion() {
    BOOL effects = TRUE;
    HIGHCONTRASTW hc{sizeof(hc)};
    SystemParametersInfoW(SPI_GETHIGHCONTRAST, sizeof(hc), &hc, 0);
    return !(hc.dwFlags & HCF_HIGHCONTRASTON) &&
           SystemParametersInfoW(SPI_GETCLIENTAREAANIMATION, 0, &effects, 0) &&
           effects;
}
// Four columns by four rows. Two pages fit without reloading icons when going back.
static constexpr size_t kIconCacheLimit = 32;
struct PopupIcon {
    HICON handle{};
    wstring target;
    uint64_t use{};
};
struct FolderPanel {
    HWND window{}, list{}, title{}, empty{}, add{}, more{}, hover{}, tips{},
        filter{}, lens{};
    HFONT normalFont{}, titleFont{};
    HBRUSH background{};
    COLORREF surface{}, foreground{}, buttonFace{}, rim{};
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
    bool dropReleaseSeen = false;
    int pressed = -1, dropTarget = -1;
    POINT start{};
    bool dragging = false, ghost = false, choosing = false, autoScroll = false;
    HWND prev{}, next{}, pager{};
    HBRUSH searchBrush{};
    COLORREF field{}, accent{};
    RECT rest{}, searchRect{};
    bool motion = false, frame = false, layered = false, closing = false;
    Ease reveal, arrival;
    int slide = 0, hot = -1, columns = 1, capacity = 1, page = 0, wheel = 0,
        edgeDir = 0;
    std::vector<TileMotion> tiles;
    std::map<HWND, Ease> controls;
    std::vector<PopupIcon> glyphs;
    uint64_t iconClock = 0;
    ULONGLONG powerReadAt = 0;
    UINT frameInterval = 16;
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
static FolderPanel panel;
static HWND panelOwner() {
    return panel.window ? panel.window : tray();
}
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
    return std::max(
        1, ((int)panel.apps.size() + panel.capacity - 1) / panel.capacity);
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
static void wakeMotion() {
    if (!panel.motion || panel.frame || !panel.window)
        return;
    // Sample on interaction only; a stationary panel never wakes to check power.
    auto now = GetTickCount64();
    if (!panel.powerReadAt || now - panel.powerReadAt >= 5000) {
        SYSTEM_POWER_STATUS status{};
        panel.frameInterval =
            GetSystemPowerStatus(&status) && status.SystemStatusFlag == 1 ? 32
                                                                          : 16;
        panel.powerReadAt = now;
    }
    panel.frame =
        SetTimer(panel.window, kFrameTimer, panel.frameInterval, nullptr) != 0;
}
static void target(Ease& e, double v, unsigned ms) {
    auto now = GetTickCount64();
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
static void arrangeDrag(int dest) {
    for (int local = 0; local < (int)panel.tiles.size(); ++local) {
        int n = firstItem() + local, slot = n;
        if (panel.dragging && !panel.choosing && dest >= 0 &&
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
        }
        target(panel.tiles[local].dx, dx, 155);
        target(panel.tiles[local].dy, dy, 155);
    }
    InvalidateRect(panel.list, nullptr, FALSE);
}
static void panelPosition(ULONGLONG now) {
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
        panelPosition(GetTickCount64());
        return;
    }
    layered(true);
    target(panel.reveal, 1, 165);
    panelPosition(GetTickCount64());
    if (!panel.frame)
        layered(false);
}
static void dismissPanel() {
    if (!panel.window || panel.closing)
        return;
    if (!panel.motion || panel.browsing ||
        panel.reveal.value(GetTickCount64()) < .028) {
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
    auto now = GetTickCount64();
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
        full |= dx || dy;  // Reordering can cross several cell boundaries.
        bool hover = t.hover.damage(now, active),
             press = t.press.damage(now, active);
        if (!full && (hover || press))
            redrawTile(firstItem() + (int)i);
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
    if (!active) {
        KillTimer(panel.window, kFrameTimer);
        panel.frame = false;
    }
}
static int appImage(int);
static void changePage(int, bool = false);
static void finishDrag() {
    if (panel.list && panel.autoScroll)
        KillTimer(panel.list, kDragPageTimer);
    panel.autoScroll = false;
    panel.edgeDir = 0;
    if (panel.ghost) {
        ImageList_DragLeave(nullptr);
        ImageList_EndDrag();
        panel.ghost = false;
    }
    if (onPage(panel.pressed) &&
        panel.pressed - firstItem() < (int)panel.tiles.size())
        target(panel.tiles[panel.pressed - firstItem()].press, 0, 105);
    int previous = panel.pressed;
    bool moved = panel.dragging;
    panel.pressed = -1;
    panel.dragging = false;
    panel.dropTarget = -1;
    if (moved)
        arrangeDrag(-1);
    else if (panel.list)
        redrawTile(previous);
    if (panel.list && GetCapture() == panel.list)
        ReleaseCapture();
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
static void dragAt(POINT p) {
    if (panel.pressed < 0 || panel.choosing)
        return;
    bool travelled = std::abs(p.x - panel.start.x) >=
                         GetSystemMetricsForDpi(SM_CXDRAG, panel.dpi) ||
                     std::abs(p.y - panel.start.y) >=
                         GetSystemMetricsForDpi(SM_CYDRAG, panel.dpi);
    if (!panel.dragging && travelled) {
        panel.dragging = true;
        int image = appImage(panel.pressed);
        if (image >= 0)
            panel.ghost = ImageList_BeginDrag(
                panel.icons, image, panel.iconSize / 2, panel.iconSize / 2);
        if (panel.ghost) {
            POINT s = p;
            ClientToScreen(panel.list, &s);
            ImageList_DragEnter(nullptr, s.x, s.y);
        }
    }
    if (!panel.dragging)
        return;
    int edge = edgeDirection(p);
    if (edge != panel.edgeDir) {
        if (panel.autoScroll)
            KillTimer(panel.list, kDragPageTimer);
        panel.edgeDir = edge;
        panel.autoScroll =
            edge && SetTimer(panel.list, kDragPageTimer, 420, nullptr);
    }
    int dest = hitApp(p);
    if (dest != panel.dropTarget) {
        panel.dropTarget = dest;
        arrangeDrag(dest);
    }
    if (panel.ghost) {
        POINT s = p;
        ClientToScreen(panel.list, &s);
        ImageList_DragMove(s.x, s.y);
    }
}
static void refreshPanel();
static std::vector<App> taskbarChoices(const AppFolder&);
static void moveApp(int from, int to) {
    if (from == to || !validIndex(from, panel.apps.size()) ||
        !validIndex(to, panel.apps.size())) {
        finishDrag();
        return;
    }
    auto a = panel.apps[from], b = panel.apps[to];
    wstring id = panel.folder;
    std::map<wstring, POINT> poses;
    auto now = GetTickCount64();
    for (int n = firstItem(); n < firstItem() + pageItems(); ++n) {
        AppRects r;
        appRects(n, r);
        auto& tile = panel.tiles[n - firstItem()];
        poses[panel.apps[n].target] = {
            r.cell.left + (LONG)std::lround(tile.dx.value(now)),
            r.cell.top + (LONG)std::lround(tile.dy.value(now))};
    }
    if (panel.dragging) {
        AppRects r;
        if (appRects(to, r))
            poses[a.target] = {r.cell.left, r.cell.top};
    }
    finishDrag();
    if (!edit([&](Model& m) { reorder(m, id, a, b); }) || !panel.window)
        return;
    panel.page = to / panel.capacity;
    refreshPanel();
    for (int n = firstItem(); n < firstItem() + pageItems(); ++n) {
        auto p = poses.find(panel.apps[n].target);
        if (p == poses.end())
            continue;
        AppRects r;
        appRects(n, r);
        auto& t = panel.tiles[n - firstItem()];
        t.dx.set(p->second.x - r.cell.left);
        t.dy.set(p->second.y - r.cell.top);
        target(t.dx, 0, 145);
        target(t.dy, 0, 145);
    }
    if (onPage(to))
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
        if (msg == WM_KEYDOWN && h == panel.filter) {
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
            if (panel.pressed >= 0)
                finishDrag();
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
            finishDrag();
            int n = hitApp(p);
            if (n < 0)
                return 0;
            panel.pressed = n;
            panel.start = p;
            highlight(n);
            target(panel.tiles[n - firstItem()].press, 1, 75);
            SetFocus(h);
            ListView_SetItemState(h, n - firstItem(),
                                  LVIS_SELECTED | LVIS_FOCUSED,
                                  LVIS_SELECTED | LVIS_FOCUSED);
            SetCapture(h);
            return 0;
        }
        if (msg == WM_MOUSEMOVE) {
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
        if (msg == WM_LBUTTONUP && panel.pressed >= 0) {
            dragAt(p);
            int from = panel.pressed, to = hitApp(p);
            bool moved = panel.dragging;
            if (moved && !panel.choosing && validIndex(to, panel.apps.size()) &&
                to != from)
                moveApp(from, to);
            else {
                finishDrag();
                if (!moved && from == to)
                    PostMessageW(panel.window, kOpenApp, from,
                                 panel.viewSerial);
            }
            return 0;
        }
        if (msg == WM_CAPTURECHANGED || msg == WM_CANCELMODE) {
            if (panel.pressed >= 0)
                finishDrag();
        }
        if (msg == WM_TIMER && wp == kDragPageTimer) {
            KillTimer(h, kDragPageTimer);
            panel.autoScroll = false;
            if (panel.dragging && panel.edgeDir) {
                int dir = panel.edgeDir;
                POINT pt{};
                GetCursorPos(&pt);
                ScreenToClient(h, &pt);
                if (edgeDirection(pt) == dir) {
                    changePage(panel.page + dir, true);
                    panel.edgeDir = 0;
                    dragAt(pt);
                }
            }
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
                : i < 0        ? firstItem()
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
    if (SUCCEEDED(
            SHParseDisplayName(a.target.c_str(), nullptr, &id, 0, nullptr))) {
        SHGetFileInfoW(reinterpret_cast<PCWSTR>(id), 0, &info, sizeof(info),
                       SHGFI_PIDL | SHGFI_ICON | SHGFI_LARGEICON);
        CoTaskMemFree(id);
    }
    if (!info.hIcon)
        info.hIcon = CopyIcon(LoadIconW(nullptr, IDI_APPLICATION));
    return info.hIcon;
}
static HICON panelIcon(const App& app) {
    // Request the device-pixel size once. Keep the original alpha icon for the small hover zoom.
    int size = std::clamp(panel.iconSize + px(2), 16, 160);
    if (app.target.find(L'!') == wstring::npos) {
        if (auto h = desktopIcon(app, size, &panel.pinnedIcons))
            return h;
    }
    auto item = shellItem(app.target);
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
    auto now = GetTickCount64();
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
        bool lifting =
            panel.dragging && !panel.choosing && index == panel.pressed;
        if (lifting)
            continue;
        double shade = std::max(
            {hot * .78, press, keyboard && focused == index ? .38 : 0.});
        if (shade > .005)
            rounded(dc, face,
                    blendColor(panel.surface, panel.buttonFace, shade), px(8));
        if (keyboard && focused == index) {
            RECT ring = face;
            InflateRect(&ring, -px(1), -px(1));
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
        SetTextColor(dc, panel.foreground);
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
    // BeginPaint validates the update region. Capture it before taking the paint DC.
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
        // Match the native accessibility and keyboard bounds to the painted grid.
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
        changePage(panel.page + turns);
    }
    return true;
}
static void refreshPanel() {
    auto f = lookup(groups, panel.folder);
    if (!f) {
        closePanel();
        return;
    }
    finishDrag();
    releasePreviousPage();
    ++panel.viewSerial;
    wstring query = panel.choosing ? windowText(panel.filter) : L"";
    auto terms = searchTerms(query);
    bool searching = !terms.empty();
    if (!panel.choosing)
        panel.apps = f->apps;
    else {
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
    int cols = panel.choosing ? available : std::clamp(count, 1, available);
    int rows = (count + cols - 1) / cols,
        gridTop = panel.choosing ? px(88) : px(44);
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
        panel.choosing
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
    panelPosition(GetTickCount64());
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
        ShowWindow(panel.lens, panel.choosing ? SW_SHOWNA : SW_HIDE);
        ShowWindow(panel.filter, panel.choosing ? SW_SHOWNA : SW_HIDE);
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
    PCWSTR emptyText = uiText(UiString::Empty);
    if (panel.choosing)
        emptyText = !searching                 ? uiText(UiString::TypeOrBrowse)
                    : catalogThread ? uiText(UiString::Searching)
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
        panel.pressed >= 0)
        return;
    RECT bounds{};
    GetWindowRect(panel.window, &bounds);
    if (!isIn(bounds, p))
        dismissPanel();
}
static void drawCommand(const DRAWITEMSTRUCT& di) {
    RECT r = di.rcItem;
    bool lens = di.hwndItem == panel.lens;
    FillRect(di.hDC, &r, lens ? panel.searchBrush : panel.background);
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
    double hover = panel.controls[di.hwndItem].value(GetTickCount64());
    if (pressed || focus || hover > .005) {
        COLORREF face = blendColor(lens ? panel.field : panel.surface,
                                   pressed ? panel.rim : panel.buttonFace,
                                   pressed ? 1 : hover);
        HBRUSH fill = CreateSolidBrush(face);
        HPEN pen = CreatePen(PS_SOLID, 1, focus ? panel.foreground : face);
        auto before = SelectObject(di.hDC, fill),
             line = SelectObject(di.hDC, pen);
        RoundRect(di.hDC, r.left + 1, r.top + 1, r.right - 1, r.bottom - 1,
                  px(10), px(10));
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
    if (!panel.window || panel.folder != dest)
        showFolder(dest, nullptr);
    if (!panel.window || panel.folder != dest)
        return;
    if (!ensureSearchControls()) {
        note(uiText(UiString::SearchFailed));
        return;
    }
    panel.choosing = false;
    SetWindowTextW(panel.filter, L"");
    panel.choicesRevision = ~0u;
    panel.page = 0;
    panel.wheel = 0;
    panel.choosing = true;
    refreshPanel();
    gridEntrance(1);
    SetFocus(panel.filter);
}
static bool addFilesToFolder(const wstring& dest,
                             const std::vector<wstring>& paths,
                             bool reportUnsupported) {
    if (stopping || !lookup(groups, dest) || paths.empty())
        return false;
    std::vector<App> apps;
    for (const auto& path : paths) {
        if (!appFile(path))
            continue;
        DWORD attributes = GetFileAttributesW(path.c_str());
        if (attributes == INVALID_FILE_ATTRIBUTES ||
            (attributes & FILE_ATTRIBUTE_DIRECTORY))
            continue;
        if (auto app = fromPath(path))
            apps.push_back(std::move(*app));
    }
    if (stopping)
        return false;
    if (apps.empty()) {
        if (reportUnsupported)
            note(uiText(UiString::NoUsableFile));
        return false;
    }
    return edit([&](Model& m) {
        for (const auto& app : apps)
            transfer(m, dest, app);
    });
}
static void workerFinished(UINT msg) {
    if (msg == kCatalogDone) {
        if (collectCatalog() && panel.window && panel.choosing &&
            GetWindowTextLengthW(panel.filter))
            refreshPanel();
        return;
    }
    if (!fileThread || !filesReady.load())
        return;
    fileThread->join();
    fileThread.reset();
    filesReady = false;
    auto paths = std::move(selectedPaths);
    auto dest = std::exchange(fileDestination, wstring{});
    bool added = addFilesToFolder(dest, paths, true);
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
static bool primaryMouseButtonDown() {
    int key = GetSystemMetrics(SM_SWAPBUTTON) ? VK_RBUTTON : VK_LBUTTON;
    return (GetAsyncKeyState(key) & 0x8000) != 0;
}
static void dropIntoPanel(HWND w, HDROP drop) {
    std::vector<wstring> paths;
    {
        struct DropFiles {
            HDROP handle;
            ~DropFiles() { DragFinish(handle); }
        } guard{drop};
        if (stopping || panel.window != w || panel.closing || panel.browsing)
            return;
        const UINT count = DragQueryFileW(drop, UINT_MAX, nullptr, 0);
        for (UINT i = 0; i < count; i++) {
            const UINT length = DragQueryFileW(drop, i, nullptr, 0);
            if (!length)
                continue;
            wstring path(length + 1, L'\0');
            if (DragQueryFileW(drop, i, path.data(), length + 1) == length) {
                path.resize(length);
                paths.push_back(std::move(path));
            }
        }
    }
    const auto dest = panel.folder;
    KillTimer(w, kDropActivationTimer);
    SetForegroundWindow(w);
    if (addFilesToFolder(dest, paths, false) && panel.window == w) {
        if (panel.choosing) {
            panel.choosing = false;
            panel.page = 0;
        }
        syncPanel();
    }
}
static LRESULT CALLBACK panelProc(HWND w, UINT msg, WPARAM wp, LPARAM lp) {
    try {
        switch (msg) {
            case kFilesDone:
            case kCatalogDone:
                workerFinished(msg);
                return 0;
            case WM_TIMER:
                if (wp == kFrameTimer) {
                    tickPanel();
                    return 0;
                }
                if (wp == kDropActivationTimer) {
                    if (primaryMouseButtonDown()) {
                        panel.dropReleaseSeen = false;
                    } else if (std::exchange(panel.dropReleaseSeen, true)) {
                        KillTimer(w, kDropActivationTimer);
                        PostMessageW(w, kCheckActivation, 0, 0);
                    }
                    return 0;
                }
                break;
            case WM_DROPFILES:
                dropIntoPanel(w, reinterpret_cast<HDROP>(wp));
                return 0;
            case WM_MOUSEWHEEL:
            case WM_MOUSEHWHEEL:
                if (pageWheel(msg, wp))
                    return 0;
                break;
            case WM_POWERBROADCAST:
                if (wp == PBT_APMPOWERSTATUSCHANGE ||
                    wp == PBT_POWERSETTINGCHANGE)
                    panel.powerReadAt = 0;
                break;
            case WM_SETTINGCHANGE:
                panel.powerReadAt = 0;
                if (panel.window == w) {
                    panel.motion = allowMotion();
                    if (!panel.motion && panel.frame) {
                        if (panel.closing) {
                            closePanel();
                            return 0;
                        }
                        panel.reveal.set(1);
                        panel.arrival.set(0);
                        for (auto& t : panel.tiles)
                            for (Ease* e : {&t.dx, &t.dy, &t.hover, &t.press})
                                e->set(e->end);
                        for (auto& c : panel.controls)
                            c.second.set(c.second.end);
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
                FillRect(dc, &ps.rcPaint, panel.background);
                if (panel.choosing) {
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
                if (panel.window == w && !panel.browsing) {
                    if (LOWORD(wp) == WA_INACTIVE) {
                        POINT p{};
                        GetCursorPos(&p);
                        const bool buttonDown = primaryMouseButtonDown();
                        if (!buttonDown || !isIn(panel.anchor, p))
                            PostMessageW(w, kCheckActivation, buttonDown, 0);
                    } else {
                        KillTimer(w, kDropActivationTimer);
                    }
                }
                break;
            case kCheckActivation: {
                if (panel.browsing || panel.closing)
                    return 0;
                HWND active = GetForegroundWindow();
                if (active && active != w && !IsChild(w, active) &&
                    GetAncestor(active, GA_ROOTOWNER) != w) {
                    // Explorer can activate as soon as a file drag starts.
                    // Give the shell a chance to deliver the drop after release.
                    panel.dropReleaseSeen = false;
                    if (!wp || !SetTimer(w, kDropActivationTimer, 40, nullptr))
                        dismissPanel();
                }
                return 0;
            }
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
                if (!panel.closing && panel.rendered == revision &&
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
                    dismissPanel();
                    if (!launch(a))
                        note(uiText(UiString::BrokenShortcut));
                }
                return 0;
            case WM_COMMAND:
                if (panel.closing)
                    return 0;
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
                    panel.choosing) {
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
                    if (pt.x == -1 && pt.y == -1)
                        i = selectedApp();
                    else {
                        ScreenToClient(panel.list, &pt);
                        i = hitApp(pt);
                    }
                    if (validIndex(i, panel.apps.size()))
                        appMenu(panel.folder, panel.apps[i]);
                    return 0;
                }
                break;
            case WM_DESTROY: {
                finishDrag();
                KillTimer(w, kFrameTimer);
                KillTimer(w, kDropActivationTimer);
                DragAcceptFiles(w, FALSE);
                MSG drop{};
                while (PeekMessageW(&drop, w, WM_DROPFILES, WM_DROPFILES,
                                    PM_NOREMOVE) && drop.message == WM_DROPFILES) {
                    PeekMessageW(&drop, w, WM_DROPFILES, WM_DROPFILES,
                                 PM_REMOVE);
                    DragFinish(reinterpret_cast<HDROP>(drop.wParam));
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
            }
            case WM_NCDESTROY:
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
                      bool dark) {
    if (stopping)
        return;
    if (panel.window) {
        if (panel.folder == id) {
            if (panel.closing)
                revealPanel();
            else
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
    panel.surface = dark ? RGB(39, 41, 48) : RGB(247, 248, 251);
    panel.foreground = dark ? RGB(244, 245, 250) : RGB(25, 28, 35);
    panel.buttonFace = dark ? RGB(56, 59, 69) : RGB(231, 235, 242);
    panel.rim = dark ? RGB(82, 87, 100) : RGB(194, 202, 213);
    panel.field = dark ? RGB(50, 53, 61) : RGB(235, 238, 244);
    panel.accent = dark ? RGB(143, 202, 255) : RGB(36, 102, 161);
    HIGHCONTRASTW hc{sizeof(hc)};
    SystemParametersInfoW(SPI_GETHIGHCONTRAST, sizeof(hc), &hc, 0);
    if (hc.dwFlags & HCF_HIGHCONTRASTON) {
        panel.surface = panel.field = GetSysColor(COLOR_WINDOW);
        panel.foreground = GetSysColor(COLOR_WINDOWTEXT);
        panel.buttonFace = GetSysColor(COLOR_BTNFACE);
        panel.rim = panel.accent = GetSysColor(COLOR_HIGHLIGHT);
    }
    panel.motion = allowMotion();
    panel.reveal.set(1);
    panel.searchBrush = CreateSolidBrush(panel.field);
    panel.background = CreateSolidBrush(panel.surface);
    panel.normalFont =
        CreateFontW(-px(14), 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET, 0, 0,
                    CLEARTYPE_QUALITY, 0, L"Segoe UI");
    panel.titleFont =
        CreateFontW(-px(16), 0, 0, 0, FW_SEMIBOLD, 0, 0, 0, DEFAULT_CHARSET, 0,
                    0, CLEARTYPE_QUALITY, 0, L"Segoe UI");
    DWORD corners = 2;
    DwmSetWindowAttribute(panel.window, DWMWA_WINDOW_CORNER_PREFERENCE,
                          &corners, sizeof(corners));
    panel.title =
        part(L"STATIC", L"", SS_CENTERIMAGE | SS_ENDELLIPSIS | SS_NOPREFIX, 0);
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
            ti.lpszText = const_cast<LPWSTR>(button == panel.add
                                                 ? uiText(UiString::AddApps)
                                                 : uiText(UiString::Options));
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
    DragAcceptFiles(panel.window, TRUE);
    refreshPanel();
    panel.reveal.set(panel.motion ? 0 : 1);
    revealPanel();
    gridEntrance(0);
    ShowWindow(panel.window, SW_SHOW);
    SetForegroundWindow(panel.window);
    SetFocus(panel.apps.empty() ? panel.add : panel.list);
}
static std::optional<RECT> onScreen(FrameworkElement const& element) {
    if (!isPrimaryRoot(element) ||
        element.Visibility() == X::Visibility::Collapsed)
        return std::nullopt;
    auto root = element.XamlRoot();
    if (!root)
        return std::nullopt;
    auto host = root.Content().try_as<FrameworkElement>();
    double scale = root.RasterizationScale();
    if (!host || scale <= 0)
        return std::nullopt;
    HWND bridge = FindWindowExW(
        ui->primary, nullptr,
        L"Windows.UI.Composition.DesktopWindowContentBridge", nullptr);
    POINT origin{};
    if (!ClientToScreen(bridge ? bridge : ui->primary, &origin))
        return std::nullopt;
    auto pos = element.TransformToVisual(host).TransformPoint({0, 0});
    return RECT{origin.x + lround(pos.X * scale),
                origin.y + lround(pos.Y * scale),
                origin.x + lround((pos.X + element.ActualWidth()) * scale),
                origin.y + lround((pos.Y + element.ActualHeight()) * scale)};
}
static void showFolder(const wstring& dest, FrameworkElement const& anchor) {
    if (!ui || !isPrimaryRoot(ui->frame) || (anchor && !isPrimaryRoot(anchor)))
        return;
    HWND owner = ui->primary;
    RECT bar{};
    if (!GetWindowRect(owner, &bar))
        return;
    FrameworkElement target = anchor;
    if (!target) {
        auto tile = std::find_if(ui->tiles.begin(), ui->tiles.end(),
                                 [&](auto const& t) { return t.id == dest; });
        if (tile != ui->tiles.end())
            target = tile->button;
    }
    LONG middle = bar.left + (bar.right - bar.left) / 2;
    RECT fallback{middle - 1, bar.top, middle + 1, bar.bottom};
    openPanel(dest, onScreen(target).value_or(fallback), bar, owner,
              ui->frame.ActualTheme() == X::ElementTheme::Dark);
}
static void onDrop(const wstring& dest, X::DragEventArgs const& e) {
    e.Handled(true);
    if (!ui->dragApp.empty())
        if (auto a = byId(ui->dragApp)) {
            moveTo(dest, *a);
            ui->dragApp.clear();
            return;
        }
    if (e.DataView().Contains(Data::StandardDataFormats::StorageItems())) {
        ui->drops.push_back({dest, e.DataView().GetStorageItemsAsync()});
        SetTimer(controller, kDropTimer, 75, nullptr);
    }
}
static Button makeTile(const AppFolder& f) {
    Button b;
    b.Width(40);
    b.Height(38);
    b.Padding({3, 3, 3, 3});
    b.BorderThickness({0, 0, 0, 0});
    b.Background(M::SolidColorBrush(W::UI::Color{0, 0, 0, 0}));
    b.CornerRadius({6, 6, 6, 6});
    X::Automation::AutomationProperties::SetName(b, f.name);
    C::ToolTipService::SetToolTip(b, winrt::box_value(hstring(f.name)));
    C::Image mosaic;
    mosaic.Width(30);
    mosaic.Height(30);
    mosaic.IsHitTestVisible(false);
    mosaic.Source(previewFor(f).image);
    b.Content(mosaic);
    auto id = f.id;
    click(b, ui->tileOff, [id, w = winrt::make_weak(b)]() {
        if (auto v = w.get())
            showFolder(id, v);
    });
    b.AllowDrop(true);
    auto over = b.DragOver([](auto&&, auto&& e) {
        try {
            if (!ui->dragApp.empty() ||
                e.DataView().Contains(
                    Data::StandardDataFormats::StorageItems())) {
                e.AcceptedOperation(ui->dragApp.empty()
                                        ? Data::DataPackageOperation::Copy
                                        : Data::DataPackageOperation::Move);
                e.Handled(true);
            }
        } catch (...) {
        }
    });
    auto drop = b.Drop([id](auto&&, auto&& e) {
        try {
            onDrop(id, e);
        } catch (...) {
        }
    });
    ui->tileOff.push_back([w = winrt::make_weak(b), over, drop]() {
        if (auto v = w.get()) {
            v.DragOver(over);
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
    if (!h.old || h.old == X::DependencyProperty::UnsetValue())
        e.ClearValue(X::UIElement::VisibilityProperty());
    else
        e.SetValue(X::UIElement::VisibilityProperty(), h.old);
}
static void syncOpacity(const std::vector<FrameworkElement>&);
static void clearView() {
    if (controller) {
        KillTimer(controller, kLayoutTimer);
        KillTimer(controller, kDropTimer);
        KillTimer(controller, kPinDragTimer);
    }
    closePanel();
    if (!ui)
        return;
    off(ui->frameOff);
    off(ui->tileOff);
    off(ui->menuOff);
    syncOpacity({});
    for (auto& h : ui->hidden)
        try {
            restore(h);
        } catch (...) {
        }
    for (auto& job : ui->drops)
        try {
            job.op.Cancel();
        } catch (...) {
        }
    try {
        if (ui->marginSet && ui->repeater)
            ui->repeater.Margin(ui->baseMargin);
    } catch (...) {
    }
    try {
        detach(ui->root, ui->overlay);
    } catch (...) {
    }
    try {
        detach(ui->root, ui->strip);
    } catch (...) {
    }
    ui.reset();
    previews.reset();
}
// A process can have a taskbar button without being pinned (for example a one-off file picker).
// Keep ordinary executable windows out of suggestions unless a pinned shortcut actually points there.
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
    // Resolve the current buttons when the user opens Add, even if a layout update is pending.
    std::set<wstring> seen;
    std::optional<std::set<wstring>> binaries;
    const auto pinDirectory = pinnedLinks();
    if (ui->repeater)
        visit(ui->repeater, [&](FrameworkElement const& e) {
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
        // Pinned classic shortcuts are useful when an Explorer layout does not expose app IDs.
        PWSTR roaming = nullptr;
        if (SUCCEEDED(SHGetKnownFolderPath(FOLDERID_RoamingAppData, 0, nullptr,
                                           &roaming))) {
            fs::path pins =
                fs::path(roaming) /
                L"Microsoft\\Internet Explorer\\Quick Launch\\User Pinned\\TaskBar";
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
    auto rect = onScreen(e);
    return rect && PtInRect(&*rect, pt);
}
static void reparent(Button const& b, C::Panel const& p) {
    auto old = M::VisualTreeHelper::GetParent(b).try_as<C::Panel>();
    if (old == p)
        return;
    detach(old, b);
    p.Children().Append(b);
}
static void scan() {
    if (!ui || !ui->frame || !ui->root || !ui->overlay)
        return;
    if (revision != ui->painted) {
        off(ui->tileOff);
        ui->overlay.Children().Clear();
        ui->strip.Children().Clear();
        ui->tiles.clear();
        for (auto& f : groups)
            ui->tiles.push_back({makeTile(f), f.id});
        if (previews)
            std::erase_if(*previews, [](const auto& entry) {
                return !lookup(groups, entry.first);
            });
        ui->painted = revision;
    }
    struct AppButton {
        FrameworkElement element;
        wstring id;
    };
    std::vector<AppButton> buttons;
    if (ui->repeater && !groups.empty())
        visit(ui->repeater, [&](FrameworkElement const& e) {
            if (winrt::get_class_name(e) != L"Taskbar.TaskListButton")
                return true;
            auto id = appKey(e);
            if (!id.empty())
                buttons.push_back({e, id});
            return false;
        });
    std::map<wstring, FrameworkElement> anchors;
    std::vector<FrameworkElement> childrenToHide;
    for (auto const& b : buttons) {
        if (prefix(b.id, kPinPrefix))
            continue;
        auto f =
            std::find_if(groups.begin(), groups.end(), [&](const AppFolder& x) {
                return std::any_of(
                    x.apps.begin(), x.apps.end(), [&](const App& a) {
                        return !a.id.empty() && equal(a.id, b.id);
                    });
            });
        if (f == groups.end())
            continue;
        if (!anchors.contains(f->id))
            anchors.emplace(f->id, b.element);
        else
            childrenToHide.push_back(b.element);
    }
    for (auto it = ui->hidden.begin(); it != ui->hidden.end();) {
        auto e = it->item.get();
        if (e && std::find(childrenToHide.begin(), childrenToHide.end(), e) !=
                     childrenToHide.end()) {
            ++it;
            continue;
        }
        restore(*it);
        it = ui->hidden.erase(it);
    }
    for (auto& e : childrenToHide)
        if (e.Visibility() != X::Visibility::Collapsed) {
            ui->hidden.push_back(
                {winrt::make_weak(e),
                 e.ReadLocalValue(X::UIElement::VisibilityProperty())});
            e.Visibility(X::Visibility::Collapsed);
        }
    int loose = 0;
    std::vector<FrameworkElement> hideGlyphs;
    for (auto const& tile : ui->tiles) {
        auto i = anchors.find(tile.id);
        if (i == anchors.end() || i->second.ActualWidth() < 2 ||
            i->second.Visibility() == X::Visibility::Collapsed) {
            reparent(tile.button, ui->strip);
            loose++;
            continue;
        }
        auto anchor = i->second;
        auto pos = anchor.TransformToVisual(ui->overlay).TransformPoint({0, 0});
        auto width = anchor.ActualWidth(), height = anchor.ActualHeight();
        reparent(tile.button, ui->overlay);
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
    for (auto const& tile : ui->tiles)
        if (M::VisualTreeHelper::GetParent(tile.button) == ui->strip) {
            if (tile.button.Width() != 40)
                tile.button.Width(40);
            if (tile.button.Height() != 38)
                tile.button.Height(38);
        }
    syncOpacity(hideGlyphs);
    double gap = loose * 40.;
    if (ui->repeater && gap != ui->reserved) {
        if (!ui->marginSet)
            ui->baseMargin = ui->repeater.Margin();
        auto m = ui->baseMargin;
        m.Right += gap;
        ui->repeater.Margin(m);
        ui->marginSet = gap != 0;
        ui->reserved = gap;
    }
}
static void unfade(Faded& f) {
    if (auto e = f.e.get(); e && e.Opacity() == 0) {
        if (!f.old || f.old == X::DependencyProperty::UnsetValue())
            e.ClearValue(X::UIElement::OpacityProperty());
        else
            e.SetValue(X::UIElement::OpacityProperty(), f.old);
    }
}
static void syncOpacity(const std::vector<FrameworkElement>& wanted) {
    auto& faded = ui->faded;
    for (auto i = faded.begin(); i != faded.end();) {
        auto e = i->e.get();
        if (e && std::find(wanted.begin(), wanted.end(), e) != wanted.end()) {
            ++i;
            continue;
        }
        try {
            unfade(*i);
        } catch (...) {
        }
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
static void collectDrops() {
    for (auto i = ui->drops.begin(); i != ui->drops.end();) {
        auto status = i->op.Status();
        if (status == F::AsyncStatus::Started) {
            ++i;
            continue;
        }
        try {
            if (status == F::AsyncStatus::Completed) {
                std::vector<App> add;
                for (auto s : i->op.GetResults())
                    if (auto a = fromPath(wstring(s.Path())))
                        add.push_back(*a);
                if (!add.empty())
                    edit([&](Model& m) {
                        for (auto& a : add)
                            transfer(m, i->folder, a);
                    });
            }
        } catch (...) {
            Wh_Log(L"Could not read the dropped shortcut.");
        }
        i = ui->drops.erase(i);
    }
    if (ui->drops.empty())
        KillTimer(controller, kDropTimer);
}
static void trackPinRelease() {
    if (!ui->dragApp.empty() && (GetAsyncKeyState(VK_LBUTTON) & 0x8000))
        return;
    KillTimer(controller, kPinDragTimer);
    auto id = std::exchange(ui->dragApp, wstring{});
    if (id.empty())
        return;
    POINT p{};
    GetCursorPos(&p);
    if (std::abs(p.x - ui->down.x) >= GetSystemMetrics(SM_CXDRAG) ||
        std::abs(p.y - ui->down.y) >= GetSystemMetrics(SM_CYDRAG)) {
        if (prefix(id, kPinPrefix)) {
            requestLayout();
            return;
        }
        wstring dest;
        for (auto& t : ui->tiles)
            if (M::VisualTreeHelper::GetParent(t.button) && under(t.button, p)) {
                dest = t.id;
                break;
            }
        if (!dest.empty())
            if (auto a = byId(id))
                moveTo(dest, *a);
    }
    requestLayout();
}
static void watchFrame() {
    auto root = ui->root;
    auto unload = root.Unloaded([](auto&&, auto&&) { ui->rootDetached = true; });
    ui->frameOff.push_back([root, unload] { root.Unloaded(unload); });
    auto load = root.Loaded([](auto&&, auto&&) {
        ui->rootDetached = false;
        requestLayout();
    });
    ui->frameOff.push_back([root, load] { root.Loaded(load); });
    auto size = root.SizeChanged([](auto&&, auto&&) {
        if (!groups.empty())
            requestLayout();
    });
    ui->frameOff.push_back([root, size] { root.SizeChanged(size); });
    auto repeater = ui->repeater;
    auto token = repeater.LayoutUpdated([](auto&&, auto&&) {
        if (!groups.empty())
            requestLayout();
    });
    ui->frameOff.push_back([repeater, token] { repeater.LayoutUpdated(token); });
    I::PointerEventHandler press = [](auto&&,
                                      I::PointerRoutedEventArgs const& e) {
        try {
            if (stopping || groups.empty() ||
                !e.GetCurrentPoint(nullptr).Properties().IsLeftButtonPressed())
                return;
            GetCursorPos(&ui->down);
            dismissOutside(ui->down);
            auto node = e.OriginalSource().try_as<X::DependencyObject>();
            while (node) {
                auto el = node.try_as<FrameworkElement>();
                if (el &&
                    winrt::get_class_name(el) == L"Taskbar.TaskListButton") {
                    ui->dragApp = appKey(el);
                    if (!ui->dragApp.empty())
                        SetTimer(controller, kPinDragTimer, 65, nullptr);
                    break;
                }
                node = M::VisualTreeHelper::GetParent(node);
            }
        } catch (...) {
        }
    };
    auto boxedPress = winrt::box_value(press);
    root.AddHandler(X::UIElement::PointerPressedEvent(), boxedPress, true);
    ui->frameOff.push_back([root, boxedPress] {
        root.RemoveHandler(X::UIElement::PointerPressedEvent(), boxedPress);
    });
}
// The shell host sees labels only. Command indices map to the immutable folder IDs
// kept by Explorer for this particular menu. Resolve shortcuts only on selection.
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
    // The host registers a message-only window on its XAML thread after first use.
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
    if ((id.empty() && !pidl) || prefix(id, kPinPrefix)) {
        wakeJumpHosts();
        return;
    }
    NativeSelection next;
    next.actions = appActions(groups);
    next.appId = id;
    if (pidl)
        next.shortcut.reset(ILCloneFull(pidl));
    if (id.empty() && !next.shortcut)
        return;
    nextTicket = nextTicket % 0x7ffffffeU + 1;
    next.ticket = nextTicket;
    next.opened = GetTickCount64();
    J::JsonArray labels;
    for (auto& action : next.actions)
        labels.Append(J::JsonValue::CreateStringValue(action.label));
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
    clearSelection();  // consume before any dialogue can run a nested message loop
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
        if (stopping && msg == kStop) {
            clearView();
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
        if (msg == WM_TIMER) {
            if (wp == kLayoutTimer) {
                KillTimer(h, kLayoutTimer);
                if (ui) {
                    ui->layoutPending = false;
                    scan();
                    if (!stopping)
                        syncPanel();
                }
                return 0;
            }
            if (wp == kDropTimer) {
                collectDrops();
                return 0;
            }
            if (wp == kFindJumpTimer) {
                retryJumpHost();
                return 0;
            }
            if (wp == kPinDragTimer) {
                trackPinRelease();
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
static bool isPrimaryFrame(FrameworkElement const& frame, HWND primary,
                           HMONITOR monitor) {
    if (GetSystemMetrics(SM_CMONITORS) == 1 &&
        !FindWindowW(L"Shell_SecondaryTrayWnd", nullptr))
        return true;
    // The taskbar frames share a UI thread. Use the actual screen position of
    // the frame to identify the owning taskbar window on multi-monitor setups.
    auto bounds =
        X::Automation::Peers::FrameworkElementAutomationPeer(frame)
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
    HMONITOR monitor = MonitorFromWindow(primary, MONITOR_DEFAULTTONULL);
    if (ui && ui->frame == frame && isPrimaryRoot(frame))
        return;
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
    clearView();
    ui.emplace();
    ui->primary = primary;
    ui->monitor = monitor;
    ui->frame = frame;
    ui->root = root;
    ui->repeater = repeater;
    ui->overlay = C::Canvas();
    ui->strip = C::StackPanel();
    ui->strip.Orientation(C::Orientation::Horizontal);
    ui->strip.HorizontalAlignment(X::HorizontalAlignment::Right);
    ui->strip.VerticalAlignment(X::VerticalAlignment::Center);
    C::Grid::SetColumnSpan(ui->overlay,
                           std::max(1u, root.ColumnDefinitions().Size()));
    C::Grid::SetColumnSpan(ui->strip,
                           std::max(1u, root.ColumnDefinitions().Size()));
    root.Children().Append(ui->overlay);
    root.Children().Append(ui->strip);
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
// Taskbar.View sometimes calls ShowJumpView synchronously, sometimes via a posted
// message. Stage the XAML target only as a fallback; the native group is authoritative.
static bool attachPrimaryFor(FrameworkElement const& source) {
    if (isPrimaryRoot(source))
        return true;
    for (auto node = source ? source.as<X::DependencyObject>() : nullptr;
         node;
         node = M::VisualTreeHelper::GetParent(node)) {
        auto element = node.try_as<FrameworkElement>();
        if (element && winrt::get_class_name(element) == L"Taskbar.TaskbarFrame") {
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
        if (!stopping) {
            auto e = buttonOf(sender);
            if (!e)
                e = buttonOf(
                    args.OriginalSource().try_as<X::DependencyObject>());
            if (attachPrimaryFor(e) && controller &&
                GetCurrentThreadId() == uiThread) {
                stageNative(appKey(e), nullptr);
                selection.awaitingGroup = selection.ticket != 0;
                contextActive = true;
            } else if (GetCurrentThreadId() == uiThread && selection.ticket) {
                clearSelection();
                wakeJumpHosts();
            }
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
                       : folderActions(blankFolder);
    unsigned index = 0;
    for (auto& action : actions) {
        C::MenuFlyoutItem entry;
        entry.Text(action.label);
        auto t = entry.Click([action](auto&&, auto&&) {
            try {
                if (!stopping)
                    runFolderAction(action);
            } catch (...) {
            }
        });
        ui->menuOff.push_back([items, w = winrt::make_weak(entry), t] {
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
            bool primaryContext = attachPrimaryFor(origin ? origin : sender);
            if (GetCurrentThreadId() == uiThread && selection.ticket) {
                clearSelection();
                wakeJumpHosts();
            }
            if (primaryContext && controller && ui &&
                GetCurrentThreadId() == uiThread) {
                inBlank = true;
                off(ui->menuOff);
                auto node = args.OriginalSource().try_as<X::DependencyObject>();
                while (node) {
                    auto b = node.try_as<Button>();
                    if (b) {
                        auto i = std::find_if(
                            ui->tiles.begin(), ui->tiles.end(),
                            [&](auto& t) { return t.button == b; });
                        if (i != ui->tiles.end()) {
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
    if (!inBlank || added || stopping || !ui || !ui->root ||
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
            if (!ui || !isPrimaryRoot(ui->frame))
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
            } else if (contextActive) {
                stageNative(id, pidl);
            } else if (selection.ticket) {
                clearSelection();
                wakeJumpHosts();
            }
        }
    } catch (...) {
        if (controller && GetCurrentThreadId() == uiThread)
            clearSelection();
    }
    return oldJump(self, group, item, flag);
}

// All XAML controls and registrations below belong to the shell's Jump List UI thread.
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
[[clang::no_destroy]] static thread_local std::optional<JumpFooter> footer;
static thread_local HWND hostWindow = nullptr;
static std::mutex hostsLock;
static std::vector<HWND> hosts;
static void clearFooter() {
    if (!footer)
        return;
    auto old = std::move(*footer);
    footer.reset();
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
    // A popup can already be visible while Window.Content is null or still points
    // at the previous page. Search it before the regular tree.
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
    if (footer && footer->controller == ctl && footer->ticket == seq && footer->rows &&
        footer->rows.XamlRoot())
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
    for (auto const& value : labels) {
        TextBlock line;
        line.Text(value.GetString());
        line.TextTrimming(X::TextTrimming::CharacterEllipsis);
        rows.Items().Append(line);
    }
    auto prop = header ? C::ListViewBase::HeaderProperty()
                       : C::ListViewBase::FooterProperty();
    footer.emplace(system, rows, winrt::event_token{}, winrt::event_token{},
                   ctl, seq, header, existing, !system.ReadLocalValue(prop));
    try {
        footer->clicked = rows.ItemClick(
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
        footer->unloaded =
            system.Unloaded([time](auto const& sender, auto const&) {
                try {
                    if (footer && footer->system == sender) {
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
    if (stopping && m == kStop) {
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
    WindhawkUtils::SYMBOL_HOOK hooks[] = {
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
    bool ok = WindhawkUtils::HookSymbols(m, hooks, ARRAYSIZE(hooks));
    if (!ok)
        Wh_Log(L"Taskbar symbols unavailable on this build.");
    return ok;
}
static bool wireGroups(HMODULE m) {
    if (!m || groupHooks.exchange(true))
        return false;
    WindhawkUtils::SYMBOL_HOOK taskbarDllHooks[] = {
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
    bool ok = WindhawkUtils::HookSymbols(m, taskbarDllHooks,
                                         ARRAYSIZE(taskbarDllHooks));
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
    UnregisterClassW(kFilesClass, moduleInstance());
    UnregisterClassW(kPanelClass, moduleInstance());
    UnregisterClassW(kController, moduleInstance());
    UnregisterClassW(kHost, moduleInstance());
}
