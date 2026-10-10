// ==WindhawkMod==
// @id              explorer-steam-games
// @name            Steam Games in Explorer
// @description     Your Steam games in the File Explorer navigation pane: launch, open folders, playtime, collections (Epic, GOG and Xbox experimental)
// @version         1.1
// @author          HaVeN80
// @github          https://github.com/haven80
// @include         windhawk.exe
// @include         explorer.exe
// @compilerOptions -lole32 -lshell32 -lshlwapi -luuid -ladvapi32 -lwindowscodecs
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Steam Games in Explorer

![Screenshot](https://i.imgur.com/xz3s5xn.png)

Adds your installed **Steam** games to the File Explorer navigation pane, with
launch, install folder, save and screenshot folders, playtime and automatic
collections.

**Epic Games**, **GOG** and **Xbox / PC Game Pass** are supported as
**experimental** (off by default, see below).

Choose between one entry per platform or a single **Games** entry with a
subfolder per platform.

## ⚠️ Turn on the "Comments" column
Playtime, last played date, size on disk and pending updates are shown in the
**Comments** column, which File Explorer hides by default. To see them:

1. Open the **Steam** folder in the navigation pane and switch to
   **View → Details**.
2. Right-click any column header (e.g. "Name") and tick **Comments**.
   If it's not in the list, click **More...**, find **Comments** and tick it.

File Explorer remembers the choice for that folder (repeat it in the ★
collection folders if they don't pick it up). Without this column the
mod still works, but you'll only see the information in the tooltip when
hovering a game.

## Experimental launchers
Epic Games, GOG and Xbox / PC Game Pass support is built on the files those
launchers keep on disk, but it has not been tested widely yet. Turn them on in
the settings and please report any game that is missing, wrong or doesn't
launch in the mod's GitHub discussion or issues.

## Context menu
* **Double-click / Play**: launches the game.
* **Open install folder**.
* **Open save folder** (Steam games with Steam Cloud).
* **Open screenshot folder** (screenshots taken with Steam).
* **Steam ▸**: Library, Store page, Community hub, Guides, Achievements,
  Verify integrity of game files, Uninstall.
* **GOG ▸**: Open in GOG Galaxy (when Galaxy is installed).

On Windows 11 these entries are under "Show more options" (or Shift+F10),
unless you use a mod that restores the classic context menu.

## Collections
Inside each platform folder, each one can be turned on or off:
★ Recently played, ★ Never played, ★ Not played in N months, ★ Largest,
★ Update pending. A collection only appears when it is not empty (playtime and
last played date are only available for Steam).

## Columns (Details view)
* **Comments** (must be enabled, see above): playtime, last played, size on
  disk, pending updates.
* **Date modified** = last played, **Date created** = install date.

## Not installed games (optional)
A ★ Not installed collection inside Steam lists the games in your Steam
library that are not installed. Double-click (or right-click → Install) opens
Steam's install dialog. The list comes from Steam's local cache (library
artwork and `appinfo.vdf`), so no account or API key is needed; it may include
games borrowed through Steam Family and miss games you never opened in the
library.

## Covers
With "Game icons: Cover", Steam and Xbox games use their cover art as icon:
with "Large icons" view the folder looks like a game library.

## Language
Folder names, menu entries and comments follow the Windows display language:
English, Italian, Spanish, French, German, Portuguese, Polish and Russian.
Other languages use English.

## How it works
The mod has two parts:

* A background process (a dedicated Windhawk process, not Explorer) reads the
  launchers' files and keeps a real folder with the shortcuts and a hidden
  `.data` folder up to date.
* Inside `explorer.exe` the mod only hooks registry reads, so that Explorer
  sees the navigation pane entries and the games' file types (the context
  menu). Their definitions live in a private registry hive file in the mod's
  storage, loaded with `RegLoadAppKey`: it isn't part of the system registry
  and no other program can see it.

**The mod doesn't write to the system registry.** Disabling or removing it
makes the navigation pane entries and menu entries disappear right away, even
if a process crashed before. The private hive file (`<user>-shell.hiv` in the
mod's storage) is mod data like the shortcuts; if File Explorer still has it
open when the mod is removed, it may stay there until Explorer restarts and can
then be deleted by hand. These entries only appear in File Explorer windows,
not in other programs' Open/Save dialogs.

By default the shortcut folder is in the mod's own Windhawk storage (one
subfolder per Windows user), so Windhawk deletes it when the mod is removed;
you can choose another location in the settings. With "Delete the shortcut
folder when the mod is disabled or removed" (on by default) the folder is
deleted when the mod is disabled too; everything is rebuilt when the mod is
enabled again (this also happens when the mod is updated). Only files created
by the mod are deleted. If you change the shortcut folder, the old one is
cleaned up the same way.

The mod never writes into game or launcher folders.

If the entries don't show up right after enabling or updating the mod, open a
new File Explorer window (or restart File Explorer once).
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- layout: perPlatform
  $name: Navigation pane layout
  $options:
  - perPlatform: One entry per platform
  - single: A single entry with a subfolder per platform
- position: aboveThisPC
  $name: Position in the navigation pane
  $description: Open a new File Explorer window to see the change.
  $options:
  - aboveThisPC: Above This PC
  - top: At the top
  - bottom: At the bottom
- singleNodeName: ""
  $name: Name of the single entry
  $description: Leave empty for "Games" in the Windows language
- steam: true
  $name: Steam
- epic: false
  $name: Epic Games (experimental)
- gog: false
  $name: GOG (experimental)
- xbox: false
  $name: Xbox / PC Game Pass (experimental)
- iconStyle: exe
  $name: Game icons
  $options:
  - exe: Game icon
  - cover: Cover art (Steam and Xbox)
- showRecent: true
  $name: "Collection: Recently played"
- recentDays: 30
  $name: "Recently played: last N days"
- showNeverPlayed: true
  $name: "Collection: Never played"
- showStale: true
  $name: "Collection: Not played in N months"
- staleMonths: 6
  $name: "Not played in: N months"
- showBiggest: true
  $name: "Collection: Largest"
- biggestCount: 10
  $name: "Largest: number of games"
- showUpdates: true
  $name: "Collection: Update pending"
- showNotInstalled: false
  $name: "Collection: Not installed (Steam)"
  $description: Steam library games that are not installed. Double-click or right-click → Install.
- webInSteam: true
  $name: Open community pages in the Steam client
  $description: When off, they open in the default browser
- deleteOnDisable: true
  $name: Delete the shortcut folder when the mod is disabled or removed
  $description: Removes the shortcuts, covers and data created by the mod. Everything is rebuilt when the mod is enabled again.
- folderPath: ""
  $name: Shortcut folder
  $description: Leave empty to use the mod's Windhawk storage folder (recommended). If you choose a folder, it is used as is.
- excludedAppIds: "228980"
  $name: Steam AppIDs to exclude
  $description: Comma separated. 228980 = Steamworks Common Redistributables
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <propsys.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <shobjidl.h>
#include <wincodec.h>
#include <windhawk_utils.h>

#include <algorithm>
#include <cstdint>
#include <cstdlib>
#include <map>
#include <set>
#include <string>
#include <utility>
#include <vector>

namespace {

// ============================================================ constants

enum PlatformId { P_STEAM, P_EPIC, P_GOG, P_XBOX, P_COUNT };

struct PlatformInfo {
    const wchar_t* code;     // used in file extensions and ProgIDs
    const wchar_t* folder;   // folder name and navigation pane name
    const wchar_t* clsid;    // navigation pane node (per-platform layout)
    const wchar_t* setting;  // enable/disable setting
    bool launchIsUrl;        // launch file is .url (else .lnk)
};

const PlatformInfo kPlatforms[P_COUNT] = {
    {L"steam", L"Steam", L"{7C3E1B52-9A4D-4F6B-8E21-3D5A6C9B0F47}", L"steam", true},
    {L"epic", L"Epic Games", L"{7C3E1B52-9A4D-4F6B-8E21-3D5A6C9B0F48}", L"epic", true},
    {L"gog", L"GOG", L"{7C3E1B52-9A4D-4F6B-8E21-3D5A6C9B0F49}", L"gog", false},
    {L"xbox", L"Xbox", L"{7C3E1B52-9A4D-4F6B-8E21-3D5A6C9B0F4A}", L"xbox", false},
};
const wchar_t kClsidAll[] = L"{7C3E1B52-9A4D-4F6B-8E21-3D5A6C9B0F4B}";


const wchar_t kDataFolder[] = L".data";
const wchar_t kSmartPrefix[] = L"\x2605 ";  // "★ "

// Companion files next to each game's "game.<ext>" target.
const wchar_t kFolderSuffix[] = L".folder.lnk";
const wchar_t kSavesSuffix[] = L".saves.lnk";
const wchar_t kShotsSuffix[] = L".screenshots.lnk";
const wchar_t kInstallSuffix[] = L".install.url";

// Not installed Steam games use their own file type.
const wchar_t kLibExt[] = L".whsteamlib";
const wchar_t kLibProgId[] = L"WhGames.steamlib";

// Optional verbs, encoded in the target file extension.
const int kBitSaves = 1;
const int kBitShots = 2;

// Shell property keys (propkey.h).
const PROPERTYKEY kPkeyMediaDuration = {
    {0x64440490, 0x4C8B, 0x11D1, {0x8B, 0x70, 0x08, 0x00, 0x36, 0xB1, 0x1A, 0x03}},
    3};
const PROPERTYKEY kPkeyDateLastUsed = {
    {0x841E4F90, 0xFF59, 0x4D16, {0x89, 0x47, 0xE8, 0x1B, 0xBF, 0xFA, 0xB3, 0x6D}},
    16};

// ============================================================ languages

// Languages of the user-visible text; English is the fallback.
enum LangId { L_EN, L_IT, L_ES, L_FR, L_DE, L_PT, L_PL, L_RU, LANG_COUNT };

LangId LanguageFromWindows() {
    switch (PRIMARYLANGID(GetUserDefaultUILanguage())) {
        case LANG_ITALIAN: return L_IT;
        case LANG_SPANISH: return L_ES;
        case LANG_FRENCH: return L_FR;
        case LANG_GERMAN: return L_DE;
        case LANG_PORTUGUESE: return L_PT;
        case LANG_POLISH: return L_PL;
        case LANG_RUSSIAN: return L_RU;
        default: return L_EN;
    }
}

enum StrId {
    S_PLAY,
    S_OPEN_INSTALL,
    S_OPEN_SAVES,
    S_OPEN_SHOTS,
    S_GAME_TYPE,
    S_LIB_TYPE,
    S_INSTALL,
    S_STORE,
    S_LIBRARY,
    S_COMMUNITY,
    S_GUIDES,
    S_ACHIEVEMENTS,
    S_VERIFY,
    S_UNINSTALL,
    S_GALAXY,
    S_NOT_INSTALLED,
    S_PLAYTIME,
    S_LAST_PLAYED,
    S_NEVER,
    S_NEVER_PLAYED,
    S_UPDATE_PENDING,
    S_SIZE,
    S_C_RECENT,
    S_C_NEVER,
    S_C_STALE,
    S_C_LARGEST,
    S_C_UPDATES,
    S_C_NOT_INSTALLED,
    S_GAMES,
    S_COUNT
};

// Columns: English, Italian, Spanish, French, German, Portuguese, Polish, Russian.
const wchar_t* const kStrings[S_COUNT][LANG_COUNT] = {
    {L"Play", L"Gioca", L"Jugar", L"Jouer", L"Spielen", L"Jogar", L"Graj", L"Играть"},
    {L"Open install folder", L"Apri cartella di installazione", L"Abrir carpeta de instalación", L"Ouvrir le dossier d'installation", L"Installationsordner öffnen", L"Abrir pasta de instalação", L"Otwórz folder instalacji", L"Открыть папку установки"},
    {L"Open save folder", L"Apri cartella dei salvataggi", L"Abrir carpeta de partidas guardadas", L"Ouvrir le dossier des sauvegardes", L"Spielstand-Ordner öffnen", L"Abrir pasta de jogos salvos", L"Otwórz folder zapisów", L"Открыть папку сохранений"},
    {L"Open screenshot folder", L"Apri cartella degli screenshot", L"Abrir carpeta de capturas", L"Ouvrir le dossier des captures d'écran", L"Screenshot-Ordner öffnen", L"Abrir pasta de capturas de tela", L"Otwórz folder zrzutów ekranu", L"Открыть папку скриншотов"},
    {L"{0} game", L"Gioco {0}", L"Juego de {0}", L"Jeu {0}", L"{0}-Spiel", L"Jogo {0}", L"Gra {0}", L"Игра {0}"},
    {L"Steam game (not installed)", L"Gioco Steam non installato", L"Juego de Steam (no instalado)", L"Jeu Steam (non installé)", L"Steam-Spiel (nicht installiert)", L"Jogo da Steam (não instalado)", L"Gra Steam (niezainstalowana)", L"Игра Steam (не установлена)"},
    {L"Install", L"Installa", L"Instalar", L"Installer", L"Installieren", L"Instalar", L"Zainstaluj", L"Установить"},
    {L"Store page", L"Pagina dello store", L"Página de la tienda", L"Page du magasin", L"Shop-Seite", L"Página da loja", L"Strona w sklepie", L"Страница в магазине"},
    {L"Open in Steam library", L"Apri nella libreria di Steam", L"Abrir en la biblioteca de Steam", L"Ouvrir dans la bibliothèque Steam", L"In der Steam-Bibliothek öffnen", L"Abrir na biblioteca da Steam", L"Otwórz w bibliotece Steam", L"Открыть в библиотеке Steam"},
    {L"Community hub", L"Hub della community", L"Centro de la comunidad", L"Hub de la communauté", L"Community-Hub", L"Central da comunidade", L"Centrum społeczności", L"Центр сообщества"},
    {L"Guides", L"Guide", L"Guías", L"Guides", L"Anleitungen", L"Guias", L"Poradniki", L"Руководства"},
    {L"Achievements", L"Obiettivi", L"Logros", L"Succès", L"Errungenschaften", L"Conquistas", L"Osiągnięcia", L"Достижения"},
    {L"Verify integrity of game files", L"Verifica integrità dei file", L"Verificar integridad de los archivos", L"Vérifier l'intégrité des fichiers du jeu", L"Spieldateien auf Fehler überprüfen", L"Verificar integridade dos arquivos", L"Sprawdź spójność plików gry", L"Проверить целостность файлов игры"},
    {L"Uninstall...", L"Disinstalla...", L"Desinstalar...", L"Désinstaller...", L"Deinstallieren...", L"Desinstalar...", L"Odinstaluj...", L"Удалить..."},
    {L"Open in GOG Galaxy", L"Apri in GOG Galaxy", L"Abrir en GOG Galaxy", L"Ouvrir dans GOG Galaxy", L"In GOG Galaxy öffnen", L"Abrir no GOG Galaxy", L"Otwórz w GOG Galaxy", L"Открыть в GOG Galaxy"},
    {L"Not installed", L"Non installato", L"No instalado", L"Non installé", L"Nicht installiert", L"Não instalado", L"Niezainstalowana", L"Не установлена"},
    {L"Playtime: ", L"Tempo di gioco: ", L"Tiempo de juego: ", L"Temps de jeu : ", L"Spielzeit: ", L"Tempo de jogo: ", L"Czas gry: ", L"Время в игре: "},
    {L"Last played: ", L"Ultimo avvio: ", L"Última partida: ", L"Dernière partie : ", L"Zuletzt gespielt: ", L"Última sessão: ", L"Ostatnio grano: ", L"Последний запуск: "},
    {L"never", L"mai", L"nunca", L"jamais", L"nie", L"nunca", L"nigdy", L"никогда"},
    {L"never played", L"mai giocato", L"sin jugar", L"jamais joué", L"nie gespielt", L"nunca jogado", L"nigdy nie grano", L"не запускалась"},
    {L"Update pending", L"Aggiornamento in sospeso", L"Actualización pendiente", L"Mise à jour en attente", L"Update ausstehend", L"Atualização pendente", L"Oczekująca aktualizacja", L"Ожидает обновления"},
    {L"Size: ", L"Spazio: ", L"Tamaño: ", L"Taille : ", L"Größe: ", L"Tamanho: ", L"Rozmiar: ", L"Размер: "},
    {L"Recently played", L"Giocati di recente", L"Jugados recientemente", L"Joués récemment", L"Kürzlich gespielt", L"Jogados recentemente", L"Ostatnio grane", L"Недавно запущенные"},
    {L"Never played", L"Mai avviati", L"Nunca jugados", L"Jamais joués", L"Nie gespielt", L"Nunca jogados", L"Nigdy nieuruchomione", L"Ни разу не запущенные"},
    {L"Not played in {0} months", L"Non avviati da {0} mesi", L"Sin jugar en {0} meses", L"Pas joués depuis {0} mois", L"Seit {0} Monaten nicht gespielt", L"Não jogados há {0} meses", L"Nieuruchamiane od {0} miesięcy", L"Не запускались {0} мес"},
    {L"Largest", L"Più pesanti", L"Más grandes", L"Les plus volumineux", L"Größte Spiele", L"Maiores", L"Największe", L"Самые большие"},
    {L"Update pending", L"Da aggiornare", L"Por actualizar", L"À mettre à jour", L"Updates ausstehend", L"Para atualizar", L"Do aktualizacji", L"Требуют обновления"},
    {L"Not installed", L"Non installati", L"No instalados", L"Non installés", L"Nicht installiert", L"Não instalados", L"Niezainstalowane", L"Не установленные"},
    {L"Games", L"Giochi", L"Juegos", L"Jeux", L"Spiele", L"Jogos", L"Gry", L"Игры"},
};

// Set by the worker thread from the Windows display language.
LangId g_lang = L_EN;

const wchar_t* Tr(StrId id) {
    return kStrings[id][g_lang];
}

// Text with "{0}" replaced by a value.
std::wstring TrFormat(StrId id, const std::wstring& value) {
    std::wstring text = Tr(id);
    size_t p = text.find(L"{0}");
    if (p != std::wstring::npos) text.replace(p, 3, value);
    return text;
}

struct ActionDef {
    const wchar_t* suffix;
    StrId label;
};
const std::vector<ActionDef> kSteamActions = {
    {L".library.url", S_LIBRARY},
    {L".store.url", S_STORE},
    {L".community.url", S_COMMUNITY},
    {L".guides.url", S_GUIDES},
    {L".achievements.url", S_ACHIEVEMENTS},
    {L".verify.url", S_VERIFY},
    {L".uninstall.url", S_UNINSTALL},
};
const std::vector<ActionDef> kGogActions = {
    {L".galaxy.url", S_GALAXY},
};

// ============================================================ settings

struct Settings {
    bool singleNode = false;
    DWORD sortBase = 0x42;
    std::wstring singleNodeName;
    bool enabled[P_COUNT] = {};
    LangId lang = L_EN;
    bool coverIcons = false;
    bool showRecent = true;
    bool showNeverPlayed = true;
    bool showStale = true;
    bool showBiggest = true;
    bool showUpdates = true;
    bool showNotInstalled = false;
    int recentDays = 30;
    int staleMonths = 6;
    int biggestCount = 10;
    bool webInSteam = true;
    std::wstring folderPath;
    bool deleteOnDisable = true;
    std::wstring excluded;
};

Settings g_settings;
SRWLOCK g_settingsLock = SRWLOCK_INIT;
HANDLE g_stopEvent = nullptr;
HANDLE g_resyncEvent = nullptr;
HANDLE g_thread = nullptr;

// ============================================================ utilities

template <class T>
struct Com {
    T* p = nullptr;
    Com() = default;
    Com(const Com&) = delete;
    Com& operator=(const Com&) = delete;
    ~Com() {
        if (p) p->Release();
    }
    T** operator&() { return &p; }
    T* operator->() { return p; }
    explicit operator bool() const { return p != nullptr; }
};

bool Stopping() {
    return WaitForSingleObject(g_stopEvent, 0) == WAIT_OBJECT_0;
}

std::wstring Lower(std::wstring s) {
    if (!s.empty()) {
        CharLowerBuffW(&s[0], (DWORD)s.size());
    }
    return s;
}

std::string LowerA(std::string s) {
    for (auto& c : s) {
        if (c >= 'A' && c <= 'Z') c = (char)(c - 'A' + 'a');
    }
    return s;
}

bool EndsWith(const std::wstring& s, const std::wstring& suffix) {
    return s.size() >= suffix.size() &&
           s.compare(s.size() - suffix.size(), suffix.size(), suffix) == 0;
}

bool StartsWith(const std::wstring& s, const std::wstring& prefix) {
    return s.compare(0, prefix.size(), prefix) == 0;
}

bool IsNumeric(const std::string& s) {
    if (s.empty()) return false;
    for (char c : s) {
        if (c < '0' || c > '9') return false;
    }
    return true;
}

std::wstring Utf8ToWide(const std::string& s) {
    if (s.empty()) return {};
    int n = MultiByteToWideChar(CP_UTF8, 0, s.data(), (int)s.size(), nullptr, 0);
    std::wstring w(n, L'\0');
    MultiByteToWideChar(CP_UTF8, 0, s.data(), (int)s.size(), &w[0], n);
    return w;
}

std::string WideToUtf8(const std::wstring& w) {
    if (w.empty()) return {};
    int n = WideCharToMultiByte(CP_UTF8, 0, w.data(), (int)w.size(), nullptr, 0,
                                nullptr, nullptr);
    std::string s(n, '\0');
    WideCharToMultiByte(CP_UTF8, 0, w.data(), (int)w.size(), &s[0], n, nullptr,
                        nullptr);
    return s;
}

std::wstring NormalizePath(std::wstring p) {
    for (auto& c : p) {
        if (c == L'/') c = L'\\';
    }
    while (p.size() > 3 && p.back() == L'\\') p.pop_back();
    return p;
}

std::wstring ParentDir(const std::wstring& p) {
    size_t i = p.find_last_of(L'\\');
    return i == std::wstring::npos ? std::wstring() : p.substr(0, i);
}

std::wstring LeafName(const std::wstring& p) {
    size_t i = p.find_last_of(L'\\');
    return i == std::wstring::npos ? p : p.substr(i + 1);
}

bool IsAbsolute(const std::wstring& p) {
    return p.size() > 2 && (p[1] == L':' || (p[0] == L'\\' && p[1] == L'\\'));
}

std::wstring ExpandEnv(const std::wstring& s) {
    wchar_t buf[MAX_PATH * 4];
    DWORD n = ExpandEnvironmentStringsW(s.c_str(), buf, ARRAYSIZE(buf));
    if (n == 0 || n > ARRAYSIZE(buf)) return s;
    return buf;
}

bool DirExists(const std::wstring& p) {
    DWORD a = GetFileAttributesW(p.c_str());
    return a != INVALID_FILE_ATTRIBUTES && (a & FILE_ATTRIBUTE_DIRECTORY);
}

bool FileExists(const std::wstring& p) {
    DWORD a = GetFileAttributesW(p.c_str());
    return a != INVALID_FILE_ATTRIBUTES && !(a & FILE_ATTRIBUTE_DIRECTORY);
}

bool ReadFileBytes(const std::wstring& path, std::string& out) {
    HANDLE h = CreateFileW(path.c_str(), GENERIC_READ,
                           FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                           nullptr, OPEN_EXISTING, 0, nullptr);
    if (h == INVALID_HANDLE_VALUE) return false;
    LARGE_INTEGER size;
    if (!GetFileSizeEx(h, &size) || size.QuadPart > 64 * 1024 * 1024) {
        CloseHandle(h);
        return false;
    }
    out.resize((size_t)size.QuadPart);
    DWORD read = 0;
    BOOL ok = out.empty() || ReadFile(h, &out[0], (DWORD)out.size(), &read, nullptr);
    CloseHandle(h);
    if (!ok) return false;
    out.resize(read);
    return true;
}

// Reads a text file as UTF-8 (converts UTF-16 LE, strips BOM).
bool ReadTextUtf8(const std::wstring& path, std::string& out) {
    if (!ReadFileBytes(path, out)) return false;
    if (out.size() >= 2 && (unsigned char)out[0] == 0xFF &&
        (unsigned char)out[1] == 0xFE) {
        std::wstring w((out.size() - 2) / 2, L'\0');
        if (!w.empty()) memcpy(&w[0], out.data() + 2, w.size() * 2);
        out = WideToUtf8(w);
    } else if (out.size() >= 3 && (unsigned char)out[0] == 0xEF &&
               (unsigned char)out[1] == 0xBB && (unsigned char)out[2] == 0xBF) {
        out.erase(0, 3);
    }
    return true;
}

bool WriteFileBytes(const std::wstring& path, const std::string& data) {
    SetFileAttributesW(path.c_str(), FILE_ATTRIBUTE_NORMAL);
    HANDLE h = CreateFileW(path.c_str(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS,
                           FILE_ATTRIBUTE_NORMAL, nullptr);
    if (h == INVALID_HANDLE_VALUE) return false;
    DWORD written = 0;
    BOOL ok = data.empty() ||
              WriteFile(h, data.data(), (DWORD)data.size(), &written, nullptr);
    CloseHandle(h);
    return ok && written == data.size();
}

FILETIME UnixToFileTime(long long t) {
    ULONGLONG v = (ULONGLONG)(t + 11644473600LL) * 10000000ULL;
    FILETIME ft;
    ft.dwLowDateTime = (DWORD)v;
    ft.dwHighDateTime = (DWORD)(v >> 32);
    return ft;
}

ULONGLONG FileTimeValue(const FILETIME& ft) {
    return ((ULONGLONG)ft.dwHighDateTime << 32) | ft.dwLowDateTime;
}

long long UnixNow() {
    FILETIME ft;
    GetSystemTimeAsFileTime(&ft);
    return (long long)(FileTimeValue(ft) / 10000000ULL) - 11644473600LL;
}

std::wstring FormatDate(const FILETIME& utc) {
    FILETIME local;
    SYSTEMTIME st;
    if (!FileTimeToLocalFileTime(&utc, &local) || !FileTimeToSystemTime(&local, &st)) {
        return {};
    }
    wchar_t date[64] = {}, time[32] = {};
    GetDateFormatEx(LOCALE_NAME_USER_DEFAULT, DATE_SHORTDATE, &st, nullptr, date,
                    ARRAYSIZE(date), nullptr);
    GetTimeFormatEx(LOCALE_NAME_USER_DEFAULT, TIME_NOSECONDS, &st, nullptr, time,
                    ARRAYSIZE(time));
    return std::wstring(date) + L" " + time;
}

std::wstring FormatPlaytime(long long minutes) {
    if (minutes <= 0) return Tr(S_NEVER_PLAYED);
    if (minutes < 60) return std::to_wstring(minutes) + L" min";
    std::wstring s = std::to_wstring(minutes / 60) + L" h";
    if (minutes % 60) s += L" " + std::to_wstring(minutes % 60) + L" min";
    return s;
}

std::wstring FormatSize(ULONGLONG bytes) {
    wchar_t buf[64] = {};
    StrFormatByteSizeW((LONGLONG)bytes, buf, ARRAYSIZE(buf));
    return buf;
}

// Valid file name from any text.
std::wstring SanitizeName(const std::wstring& in) {
    std::wstring s;
    for (wchar_t c : in) {
        if (c == L':') {
            s += L" -";
        } else if (c < 32 || wcschr(L"<>\"/\\|?*", c)) {
            s += L' ';
        } else {
            s += c;
        }
    }
    std::wstring out;
    for (wchar_t c : s) {
        if (c == L' ' && (out.empty() || out.back() == L' ')) continue;
        out += c;
    }
    if (out.size() > 100) out.resize(100);
    while (!out.empty() && (out.back() == L' ' || out.back() == L'.')) out.pop_back();
    if (out.empty()) return L"Game";
    // Reserved device names can't be used as file names, even with an extension.
    static const wchar_t* const kReserved[] = {
        L"con", L"prn", L"aux", L"nul", L"com1", L"com2", L"com3", L"com4", L"com5",
        L"com6", L"com7", L"com8", L"com9", L"lpt1", L"lpt2", L"lpt3", L"lpt4",
        L"lpt5", L"lpt6", L"lpt7", L"lpt8", L"lpt9",
    };
    std::wstring lower = Lower(out);
    for (auto r : kReserved) {
        if (lower == r) return out + L"_";
    }
    return out;
}

// Removes control characters (CR/LF would add lines to .url files).
std::string NoControlChars(const std::string& in) {
    std::string out;
    for (char c : in) {
        if ((unsigned char)c >= 0x20 && c != 0x7F) out += c;
    }
    return out;
}

std::wstring HashString(const std::wstring& s) {
    unsigned long long h = 1469598103934665603ULL;
    for (wchar_t c : s) {
        h ^= (unsigned long long)c;
        h *= 1099511628211ULL;
    }
    wchar_t buf[17];
    swprintf(buf, 17, L"%016llx", h);
    return buf;
}

std::wstring FindFileRecursive(const std::wstring& dir, const std::wstring& pattern,
                               int depth) {
    WIN32_FIND_DATAW fd;
    HANDLE h = FindFirstFileW((dir + L"\\" + pattern).c_str(), &fd);
    if (h != INVALID_HANDLE_VALUE) {
        do {
            if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
                FindClose(h);
                return dir + L"\\" + fd.cFileName;
            }
        } while (FindNextFileW(h, &fd));
        FindClose(h);
    }
    if (depth <= 0) return {};
    h = FindFirstFileW((dir + L"\\*").c_str(), &fd);
    if (h == INVALID_HANDLE_VALUE) return {};
    std::wstring found;
    do {
        std::wstring n = fd.cFileName;
        if ((fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && n != L"." && n != L"..") {
            found = FindFileRecursive(dir + L"\\" + n, pattern, depth - 1);
            if (!found.empty()) break;
        }
    } while (FindNextFileW(h, &fd));
    FindClose(h);
    return found;
}

Settings GetSettings() {
    AcquireSRWLockShared(&g_settingsLock);
    Settings s = g_settings;
    ReleaseSRWLockShared(&g_settingsLock);
    return s;
}

// The mod's Windhawk storage folder (deleted by Windhawk when the mod is
// removed), with a subfolder per Windows user; or the folder from the settings.
// Empty if neither is available: callers must then do nothing.
std::wstring RootFolder(const Settings& s) {
    if (!s.folderPath.empty()) return NormalizePath(ExpandEnv(s.folderPath));
    WCHAR storage[MAX_PATH];
    WCHAR user[256];
    DWORD userLen = ARRAYSIZE(user);
    if (!Wh_GetModStoragePath(storage, ARRAYSIZE(storage)) ||
        !GetUserNameW(user, &userLen)) {
        return {};
    }
    return std::wstring(storage) + L"\\" + user;
}

// ============================================================ registry

// The shell entries (file types and navigation pane folders) are never
// written to the real registry. Explorer builds them in a private application
// hive (a file in the mod's storage, loaded with RegLoadAppKey, invisible to
// other processes), and the registry read hooks below make Explorer see them.
HKEY g_hive = nullptr;

// Set when a write or delete in the hive actually changed something.
bool g_regChanged = false;

// Writes a value in the hive only if it differs, so Explorer's association
// cache is invalidated only when needed.
bool RegWrite(const std::wstring& key, const wchar_t* name, DWORD type,
              const void* data, DWORD size) {
    HKEY h;
    DWORD disposition = 0;
    if (!g_hive ||
        RegCreateKeyExW(g_hive, key.c_str(), 0, nullptr, 0, KEY_SET_VALUE | KEY_QUERY_VALUE,
                        nullptr, &h, &disposition) != ERROR_SUCCESS) {
        return false;
    }
    if (disposition == REG_OPENED_EXISTING_KEY) {
        DWORD curType = 0, curSize = 0;
        if (RegQueryValueExW(h, name, nullptr, &curType, nullptr, &curSize) ==
                ERROR_SUCCESS &&
            curType == type && curSize == size) {
            std::vector<BYTE> cur(size ? size : 1);
            if (RegQueryValueExW(h, name, nullptr, nullptr, cur.data(), &curSize) ==
                    ERROR_SUCCESS &&
                memcmp(cur.data(), data, size) == 0) {
                RegCloseKey(h);
                return true;
            }
        }
    }
    LSTATUS r = RegSetValueExW(h, name, 0, type, (const BYTE*)data, size);
    RegCloseKey(h);
    if (r == ERROR_SUCCESS) g_regChanged = true;
    return r == ERROR_SUCCESS;
}

void RegDeleteKeyTree(const std::wstring& key) {
    if (g_hive && RegDeleteTreeW(g_hive, key.c_str()) == ERROR_SUCCESS) {
        g_regChanged = true;
    }
}

void NotifyAssocChanged() {
    SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST | SHCNF_FLUSHNOWAIT, nullptr, nullptr);
}

// Asks Explorer to refresh the desktop's children (navigation pane roots).
void NotifyDesktopChanged() {
    PIDLIST_ABSOLUTE desktop = nullptr;
    if (SUCCEEDED(SHGetSpecialFolderLocation(nullptr, CSIDL_DESKTOP, &desktop))) {
        SHChangeNotify(SHCNE_UPDATEDIR, SHCNF_IDLIST | SHCNF_FLUSHNOWAIT, desktop, nullptr);
        CoTaskMemFree(desktop);
    }
}

bool RegStr(const std::wstring& key, const wchar_t* name, const std::wstring& v,
            DWORD type = REG_SZ) {
    return RegWrite(key, name, type, v.c_str(), (DWORD)((v.size() + 1) * sizeof(wchar_t)));
}

bool RegDword(const std::wstring& key, const wchar_t* name, DWORD v) {
    return RegWrite(key, name, REG_DWORD, &v, sizeof(v));
}

std::wstring RegReadStr(HKEY root, const std::wstring& key, const wchar_t* name) {
    wchar_t buf[2048];
    DWORD size = sizeof(buf);
    if (RegGetValueW(root, key.c_str(), name, RRF_RT_REG_SZ | RRF_RT_REG_EXPAND_SZ,
                     nullptr, buf, &size) == ERROR_SUCCESS) {
        return buf;
    }
    return {};
}

// First token of a command line ("C:\x\y.exe" %1 -> C:\x\y.exe).
std::wstring ExeFromCommand(const std::wstring& cmd) {
    if (cmd.empty()) return {};
    if (cmd[0] == L'"') {
        size_t e = cmd.find(L'"', 1);
        return e == std::wstring::npos ? cmd.substr(1) : cmd.substr(1, e - 1);
    }
    size_t e = Lower(cmd).find(L".exe");
    return e == std::wstring::npos ? cmd : cmd.substr(0, e + 4);
}

std::wstring ProtocolHandlerExe(const wchar_t* protocol) {
    std::wstring exe = ExeFromCommand(RegReadStr(
        HKEY_CLASSES_ROOT, std::wstring(protocol) + L"\\shell\\open\\command", nullptr));
    return FileExists(exe) ? exe : std::wstring();
}

std::wstring GetSteamPath() {
    std::wstring p = RegReadStr(HKEY_CURRENT_USER, L"Software\\Valve\\Steam", L"SteamPath");
    if (p.empty()) {
        p = RegReadStr(HKEY_LOCAL_MACHINE, L"SOFTWARE\\WOW6432Node\\Valve\\Steam",
                       L"InstallPath");
    }
    if (p.empty()) {
        p = RegReadStr(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Valve\\Steam", L"InstallPath");
    }
    p = NormalizePath(p);
    if (!p.empty() && !DirExists(p)) return {};
    return p;
}

std::wstring GalaxyExe() {
    return ProtocolHandlerExe(L"goggalaxy");
}

const wchar_t kFolderIcon[] = L"%SystemRoot%\\System32\\imageres.dll,-3";

std::wstring PlatformIcon(int p) {
    std::wstring exe;
    switch (p) {
        case P_STEAM: {
            std::wstring steam = GetSteamPath();
            if (!steam.empty() && FileExists(steam + L"\\steam.exe")) {
                exe = steam + L"\\steam.exe";
            }
            break;
        }
        case P_EPIC:
            exe = ProtocolHandlerExe(L"com.epicgames.launcher");
            break;
        case P_GOG:
            exe = GalaxyExe();
            break;
        default:
            break;
    }
    return exe.empty() ? std::wstring(kFolderIcon) : exe + L",0";
}

std::wstring ExtFor(int p, int bits) {
    return std::wstring(L".wh") + kPlatforms[p].code + std::to_wstring(bits);
}

std::wstring ProgIdFor(int p, int bits) {
    return std::wstring(L"WhGames.") + kPlatforms[p].code + std::to_wstring(bits);
}

const std::vector<ActionDef>& ActionsFor(int p) {
    static const std::vector<ActionDef> kNone;
    if (p == P_STEAM) return kSteamActions;
    if (p == P_GOG) return kGogActions;
    return kNone;
}

std::wstring LaunchSuffix(int p) {
    return kPlatforms[p].launchIsUrl ? L".play.url" : L".play.lnk";
}

// File types whose verbs make up the context menu of each game.
// File type of not installed Steam games: Install and a few store links.
void RegisterLibraryType(bool enable) {
    if (!enable) {
        RegDeleteKeyTree(L"Classes\\" + std::wstring(kLibExt));
        RegDeleteKeyTree(L"Classes\\" + std::wstring(kLibProgId));
        return;
    }
    auto command = [](const std::wstring& suffix) {
        return L"\"%SystemRoot%\\explorer.exe\" \"%1" + suffix + L"\"";
    };
    std::wstring icon = PlatformIcon(P_STEAM);
    std::wstring cls = L"Classes\\" + std::wstring(kLibProgId);
    RegStr(L"Classes\\" + std::wstring(kLibExt), nullptr, kLibProgId);
    RegStr(cls, nullptr, Tr(S_LIB_TYPE));
    RegStr(cls + L"\\DefaultIcon", nullptr, icon, REG_EXPAND_SZ);
    RegStr(cls + L"\\shell", nullptr, L"open");
    RegStr(cls + L"\\shell\\open", nullptr, Tr(S_INSTALL));
    RegStr(cls + L"\\shell\\open", L"Icon", icon, REG_EXPAND_SZ);
    RegStr(cls + L"\\shell\\open\\command", nullptr, command(kInstallSuffix), REG_EXPAND_SZ);
    struct Verb {
        const wchar_t* key;
        const wchar_t* suffix;
        StrId label;
    };
    const Verb verbs[] = {
        {L"1store", L".store.url", S_STORE},
        {L"2library", L".library.url", S_LIBRARY},
        {L"3community", L".community.url", S_COMMUNITY},
    };
    for (const auto& v : verbs) {
        std::wstring k = cls + L"\\shell\\" + v.key;
        RegStr(k, nullptr, Tr(v.label));
        RegStr(k + L"\\command", nullptr, command(v.suffix), REG_EXPAND_SZ);
    }
}

// File types of the enabled platforms (the others are removed).
void RegisterFileTypes(const Settings& s) {
    bool galaxy = !GalaxyExe().empty();
    auto command = [](const std::wstring& suffix) {
        return L"\"%SystemRoot%\\explorer.exe\" \"%1" + suffix + L"\"";
    };

    for (int p = 0; p < P_COUNT; p++) {
        if (!s.enabled[p]) {
            for (int bits = 0; bits < 4; bits++) {
                RegDeleteKeyTree(L"Classes\\" + ExtFor(p, bits));
                RegDeleteKeyTree(L"Classes\\" + ProgIdFor(p, bits));
            }
            continue;
        }
        std::wstring icon = PlatformIcon(p);
        for (int bits = 0; bits < 4; bits++) {
            std::wstring cls = L"Classes\\" + ProgIdFor(p, bits);
            RegStr(L"Classes\\" + ExtFor(p, bits), nullptr, ProgIdFor(p, bits));
            RegStr(cls, nullptr, TrFormat(S_GAME_TYPE, kPlatforms[p].folder));
            RegStr(cls + L"\\DefaultIcon", nullptr, icon, REG_EXPAND_SZ);
            RegStr(cls + L"\\shell", nullptr, L"open");

            RegStr(cls + L"\\shell\\open", nullptr, Tr(S_PLAY));
            RegStr(cls + L"\\shell\\open", L"Icon", icon, REG_EXPAND_SZ);
            RegStr(cls + L"\\shell\\open\\command", nullptr, command(LaunchSuffix(p)),
                   REG_EXPAND_SZ);

            std::wstring k = cls + L"\\shell\\1installdir";
            RegStr(k, nullptr, Tr(S_OPEN_INSTALL));
            RegStr(k, L"Icon", kFolderIcon, REG_EXPAND_SZ);
            RegStr(k + L"\\command", nullptr, command(kFolderSuffix), REG_EXPAND_SZ);

            if (bits & kBitSaves) {
                k = cls + L"\\shell\\2saves";
                RegStr(k, nullptr, Tr(S_OPEN_SAVES));
                RegStr(k, L"Icon", kFolderIcon, REG_EXPAND_SZ);
                RegStr(k + L"\\command", nullptr, command(kSavesSuffix), REG_EXPAND_SZ);
            }
            if (bits & kBitShots) {
                k = cls + L"\\shell\\3screenshots";
                RegStr(k, nullptr, Tr(S_OPEN_SHOTS));
                RegStr(k, L"Icon", kFolderIcon, REG_EXPAND_SZ);
                RegStr(k + L"\\command", nullptr, command(kShotsSuffix), REG_EXPAND_SZ);
            }

            const auto& actions = ActionsFor(p);
            std::wstring menu = cls + L"\\shell\\4menu";
            if (actions.empty() || (p == P_GOG && !galaxy)) {
                RegDeleteKeyTree(menu);
            } else {
                RegStr(menu, L"MUIVerb", kPlatforms[p].folder);
                RegStr(menu, L"Icon", icon, REG_EXPAND_SZ);
                RegStr(menu, L"SubCommands", L"");
                int index = 1;
                for (const auto& a : actions) {
                    std::wstring sub = menu + L"\\shell\\" + std::to_wstring(index++);
                    RegStr(sub, L"MUIVerb", Tr(a.label));
                    RegStr(sub + L"\\command", nullptr, command(a.suffix), REG_EXPAND_SZ);
                }
            }
        }
    }
    RegisterLibraryType(s.enabled[P_STEAM] && s.showNotInstalled);
}

// Hive keys of the navigation pane folders. All of them are always present
// in the hive; the NameSpace hook decides which ones Explorer lists.
void RegisterNode(const std::wstring& clsid, const std::wstring& name,
                  const std::wstring& icon, const std::wstring& target, DWORD sortIndex) {
    std::wstring base = L"Classes\\CLSID\\" + clsid;
    RegStr(base, nullptr, name);
    RegDword(base, L"System.IsPinnedToNameSpaceTree", 1);
    RegDword(base, L"SortOrderIndex", sortIndex);
    RegStr(base + L"\\DefaultIcon", nullptr, icon, REG_EXPAND_SZ);
    RegStr(base + L"\\InProcServer32", nullptr, L"%SystemRoot%\\system32\\shell32.dll",
           REG_EXPAND_SZ);
    RegStr(base + L"\\Instance", L"CLSID", L"{0E5AAE11-A475-4c5b-AB00-C66DE400274E}");
    RegDword(base + L"\\Instance\\InitPropertyBag", L"Attributes", 0x11);
    RegStr(base + L"\\Instance\\InitPropertyBag", L"TargetFolderPath", target);
    RegDword(base + L"\\ShellFolder", L"FolderValueFlags", 0x28);
    RegDword(base + L"\\ShellFolder", L"Attributes", 0xF080004D);
    RegStr(L"NameSpaceNodes\\" + clsid, nullptr, name);
}

// Writes everything Explorer needs into the hive. Returns true if it changed.
bool BuildHive(const Settings& s) {
    g_regChanged = false;
    RegisterFileTypes(s);

    std::wstring root = RootFolder(s);
    int firstEnabled = P_STEAM;
    for (int p = P_COUNT - 1; p >= 0; p--) {
        if (s.enabled[p]) firstEnabled = p;
    }
    // SortOrderIndex sets where the entries go in the navigation pane.
    DWORD sortBase = s.sortBase;
    RegisterNode(kClsidAll, s.singleNodeName.empty() ? Tr(S_GAMES) : s.singleNodeName,
                 PlatformIcon(firstEnabled), root, sortBase);
    for (int p = 0; p < P_COUNT; p++) {
        RegisterNode(kPlatforms[p].clsid, kPlatforms[p].folder, PlatformIcon(p),
                     root + L"\\" + kPlatforms[p].folder, sortBase + p);
    }
    // Empty stand-ins, used when the real keys don't exist (see the hooks).
    HKEY h;
    for (auto key : {L"NameSpace", L"HideDesktopIcons"}) {
        if (RegCreateKeyExW(g_hive, key, 0, nullptr, 0, KEY_READ, nullptr, &h, nullptr) ==
            ERROR_SUCCESS) {
            RegCloseKey(h);
        }
    }
    return g_regChanged;
}

// ============================================================ parsers

struct VdfToken {
    int type;  // 0 = string, 1 = '{', 2 = '}'
    std::string s;
};

struct VdfKv {
    int depth;
    std::string key, value, parent, grandparent;
};

std::vector<VdfToken> VdfTokenize(const std::string& d) {
    std::vector<VdfToken> t;
    size_t i = 0, n = d.size();
    while (i < n) {
        char c = d[i];
        if (c == '"') {
            std::string s;
            i++;
            while (i < n && d[i] != '"') {
                if (d[i] == '\\' && i + 1 < n) {
                    char e = d[i + 1];
                    s += (e == 'n') ? '\n' : (e == 't') ? '\t' : e;
                    i += 2;
                } else {
                    s += d[i++];
                }
            }
            i++;
            t.push_back({0, std::move(s)});
        } else if (c == '{') {
            t.push_back({1, {}});
            i++;
        } else if (c == '}') {
            t.push_back({2, {}});
            i++;
        } else if (c == '/' && i + 1 < n && d[i + 1] == '/') {
            while (i < n && d[i] != '\n') i++;
        } else {
            i++;
        }
    }
    return t;
}

std::vector<VdfKv> VdfFlatten(const std::vector<VdfToken>& t) {
    std::vector<VdfKv> out;
    std::vector<std::string> stack;
    for (size_t i = 0; i < t.size(); i++) {
        if (t[i].type == 1) {
            stack.push_back({});
        } else if (t[i].type == 2) {
            if (!stack.empty()) stack.pop_back();
        } else if (i + 1 < t.size() && t[i + 1].type == 1) {
            stack.push_back(t[i].s);
            i++;
        } else if (i + 1 < t.size() && t[i + 1].type == 0) {
            VdfKv kv;
            kv.depth = (int)stack.size();
            kv.key = t[i].s;
            kv.value = t[i + 1].s;
            if (stack.size() >= 1) kv.parent = stack[stack.size() - 1];
            if (stack.size() >= 2) kv.grandparent = stack[stack.size() - 2];
            out.push_back(std::move(kv));
            i++;
        }
    }
    return out;
}

// Minimal JSON reader: top-level members of an object as strings.
// Arrays of scalars are joined with '|', nested objects are skipped.
class JsonReader {
public:
    explicit JsonReader(const std::string& s) : s_(s) {}

    bool ReadTopLevel(std::map<std::string, std::string>& out) {
        Ws();
        if (!Eat('{')) return false;
        Ws();
        if (Eat('}')) return true;
        for (;;) {
            Ws();
            std::string key;
            if (!String(key)) return false;
            Ws();
            if (!Eat(':')) return false;
            Ws();
            std::string value;
            if (!Value(value, 0)) return false;
            out[key] = value;
            Ws();
            if (Eat(',')) continue;
            return Eat('}');
        }
    }

private:
    void Ws() {
        while (i_ < s_.size() && (unsigned char)s_[i_] <= ' ') i_++;
    }
    bool Eat(char c) {
        if (i_ < s_.size() && s_[i_] == c) {
            i_++;
            return true;
        }
        return false;
    }
    static void AppendUtf8(std::string& out, unsigned cp) {
        if (cp < 0x80) {
            out += (char)cp;
        } else if (cp < 0x800) {
            out += (char)(0xC0 | (cp >> 6));
            out += (char)(0x80 | (cp & 0x3F));
        } else if (cp < 0x10000) {
            out += (char)(0xE0 | (cp >> 12));
            out += (char)(0x80 | ((cp >> 6) & 0x3F));
            out += (char)(0x80 | (cp & 0x3F));
        } else {
            out += (char)(0xF0 | (cp >> 18));
            out += (char)(0x80 | ((cp >> 12) & 0x3F));
            out += (char)(0x80 | ((cp >> 6) & 0x3F));
            out += (char)(0x80 | (cp & 0x3F));
        }
    }
    bool Hex4(unsigned& v) {
        if (i_ + 4 > s_.size()) return false;
        v = 0;
        for (int k = 0; k < 4; k++) {
            char c = s_[i_++];
            v <<= 4;
            if (c >= '0' && c <= '9') v |= c - '0';
            else if (c >= 'a' && c <= 'f') v |= c - 'a' + 10;
            else if (c >= 'A' && c <= 'F') v |= c - 'A' + 10;
            else return false;
        }
        return true;
    }
    bool String(std::string& out) {
        if (!Eat('"')) return false;
        while (i_ < s_.size()) {
            char c = s_[i_++];
            if (c == '"') return true;
            if (c != '\\') {
                out += c;
                continue;
            }
            if (i_ >= s_.size()) return false;
            char e = s_[i_++];
            switch (e) {
                case 'n': out += '\n'; break;
                case 't': out += '\t'; break;
                case 'r': out += '\r'; break;
                case 'b': out += '\b'; break;
                case 'f': out += '\f'; break;
                case 'u': {
                    unsigned cp;
                    if (!Hex4(cp)) return false;
                    if (cp >= 0xD800 && cp <= 0xDBFF && i_ + 1 < s_.size() &&
                        s_[i_] == '\\' && s_[i_ + 1] == 'u') {
                        i_ += 2;
                        unsigned lo;
                        if (!Hex4(lo)) return false;
                        cp = 0x10000 + ((cp - 0xD800) << 10) + (lo - 0xDC00);
                    }
                    AppendUtf8(out, cp);
                    break;
                }
                default: out += e; break;
            }
        }
        return false;
    }
    bool Value(std::string& out, int depth) {
        if (depth > 32 || i_ >= s_.size()) return false;
        char c = s_[i_];
        if (c == '"') return String(out);
        if (c == '{') {
            i_++;
            Ws();
            if (Eat('}')) return true;
            for (;;) {
                Ws();
                std::string k, v;
                if (!String(k)) return false;
                Ws();
                if (!Eat(':')) return false;
                Ws();
                if (!Value(v, depth + 1)) return false;
                Ws();
                if (Eat(',')) continue;
                return Eat('}');
            }
        }
        if (c == '[') {
            i_++;
            Ws();
            if (Eat(']')) return true;
            for (;;) {
                Ws();
                std::string v;
                if (!Value(v, depth + 1)) return false;
                if (!out.empty()) out += '|';
                out += v;
                Ws();
                if (Eat(',')) continue;
                return Eat(']');
            }
        }
        while (i_ < s_.size() && s_[i_] != ',' && s_[i_] != '}' && s_[i_] != ']' &&
               (unsigned char)s_[i_] > ' ') {
            out += s_[i_++];
        }
        return true;
    }

    const std::string& s_;
    size_t i_ = 0;
};

// Attribute of the first <tag ...> element in an XML document.
std::string XmlAttr(const std::string& xml, const std::string& tag,
                    const std::string& attr) {
    std::string open = "<" + tag;
    size_t p = 0;
    for (;;) {
        p = xml.find(open, p);
        if (p == std::string::npos) return {};
        size_t after = p + open.size();
        char next = after < xml.size() ? xml[after] : 0;
        if (next == ' ' || next == '\t' || next == '\r' || next == '\n' ||
            next == '/' || next == '>') {
            break;
        }
        p = after;
    }
    size_t end = xml.find('>', p);
    if (end == std::string::npos) return {};
    std::string text = xml.substr(p, end - p);
    std::string needle = attr + "=\"";
    size_t a = 0;
    for (;;) {
        a = text.find(needle, a);
        if (a == std::string::npos) return {};
        char before = a > 0 ? text[a - 1] : ' ';
        if (before == ' ' || before == '\t' || before == '\r' || before == '\n') break;
        a += needle.size();
    }
    a += needle.size();
    size_t q = text.find('"', a);
    return q == std::string::npos ? std::string() : text.substr(a, q - a);
}

// ============================================================ games

struct Game {
    int platform = P_STEAM;
    std::wstring id;
    std::wstring name;
    std::wstring dir;
    // Launch: URL (Steam, Epic) or executable/arguments (GOG, Xbox).
    std::string launchUrl;
    std::wstring launchExe, launchArgs, launchWorkDir;
    std::wstring iconExe;     // known icon source (empty = search the folder)
    std::wstring coverImage;  // image used for "cover" icons
    bool hasStats = false;
    long long lastPlayed = 0;  // unix time
    long long playMinutes = 0;
    ULONGLONG size = 0;
    bool updatePending = false;
    FILETIME installTime = {};
    std::wstring savesDir, shotsDir;
    std::map<std::wstring, std::string> actionUrls;  // suffix -> URL
    bool notInstalled = false;  // Steam library game that is not installed
};

bool GetDirInfo(const std::wstring& dir, FILETIME* created) {
    WIN32_FILE_ATTRIBUTE_DATA a;
    if (!GetFileAttributesExW(dir.c_str(), GetFileExInfoStandard, &a) ||
        !(a.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
        return false;
    }
    if (created) *created = a.ftCreationTime;
    return true;
}

// ---------------------------------------------------------------- Steam

std::vector<std::wstring> GetSteamLibraries(const std::wstring& steam) {
    std::vector<std::wstring> libs;
    std::set<std::wstring> seen;
    auto add = [&](std::wstring p) {
        p = NormalizePath(p);
        if (!p.empty() && seen.insert(Lower(p)).second && DirExists(p + L"\\steamapps")) {
            libs.push_back(p);
        }
    };
    add(steam);
    std::string data;
    if (ReadFileBytes(steam + L"\\steamapps\\libraryfolders.vdf", data)) {
        for (const auto& kv : VdfFlatten(VdfTokenize(data))) {
            if (_stricmp(kv.key.c_str(), "path") == 0 ||
                (IsNumeric(kv.key) && kv.value.find(':') != std::string::npos)) {
                add(Utf8ToWide(kv.value));
            }
        }
    }
    return libs;
}

// userdata\<account> of the account used most recently on this PC.
std::wstring GetSteamUserDir(const std::wstring& steam) {
    std::wstring best;
    ULONGLONG bestTime = 0;
    std::wstring userdata = steam + L"\\userdata\\";
    WIN32_FIND_DATAW fd;
    HANDLE h = FindFirstFileW((userdata + L"*").c_str(), &fd);
    if (h == INVALID_HANDLE_VALUE) return {};
    do {
        if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) || fd.cFileName[0] == L'.') {
            continue;
        }
        std::wstring dir = userdata + fd.cFileName;
        WIN32_FILE_ATTRIBUTE_DATA a;
        if (GetFileAttributesExW((dir + L"\\config\\localconfig.vdf").c_str(),
                                 GetFileExInfoStandard, &a)) {
            ULONGLONG t = FileTimeValue(a.ftLastWriteTime);
            if (t > bestTime) {
                bestTime = t;
                best = dir;
            }
        }
    } while (FindNextFileW(h, &fd));
    FindClose(h);
    return best;
}

struct SteamStats {
    long long lastPlayed = 0;
    long long playMinutes = 0;
};

std::map<std::wstring, SteamStats> ParseSteamStats(const std::wstring& userDir);

// localconfig.vdf can be several MB: parse it again only when it changes.
std::map<std::wstring, SteamStats> ReadSteamStats(const std::wstring& userDir) {
    static std::wstring cachedDir;
    static ULONGLONG cachedTime = 0;
    static std::map<std::wstring, SteamStats> cached;
    WIN32_FILE_ATTRIBUTE_DATA a;
    ULONGLONG mtime =
        GetFileAttributesExW((userDir + L"\\config\\localconfig.vdf").c_str(),
                             GetFileExInfoStandard, &a)
            ? FileTimeValue(a.ftLastWriteTime)
            : 0;
    if (mtime == 0 || mtime != cachedTime || userDir != cachedDir) {
        cached = ParseSteamStats(userDir);
        cachedTime = mtime;
        cachedDir = userDir;
    }
    return cached;
}

std::map<std::wstring, SteamStats> ParseSteamStats(const std::wstring& userDir) {
    std::map<std::wstring, SteamStats> stats;
    std::string data;
    if (userDir.empty() || !ReadFileBytes(userDir + L"\\config\\localconfig.vdf", data)) {
        return stats;
    }
    for (const auto& kv : VdfFlatten(VdfTokenize(data))) {
        if (_stricmp(kv.grandparent.c_str(), "apps") != 0 || !IsNumeric(kv.parent)) {
            continue;
        }
        if (_stricmp(kv.key.c_str(), "LastPlayed") == 0) {
            stats[Utf8ToWide(kv.parent)].lastPlayed = _atoi64(kv.value.c_str());
        } else if (_stricmp(kv.key.c_str(), "Playtime") == 0) {
            stats[Utf8ToWide(kv.parent)].playMinutes = _atoi64(kv.value.c_str());
        }
    }
    return stats;
}

std::wstring FindSteamCover(const std::wstring& steam, const std::wstring& appId) {
    std::wstring lc = steam + L"\\appcache\\librarycache\\";
    const std::wstring flat[] = {
        lc + appId + L"_library_600x900.jpg",
        lc + appId + L"\\library_600x900.jpg",
    };
    for (const auto& f : flat) {
        if (FileExists(f)) return f;
    }
    std::wstring found = FindFileRecursive(lc + appId, L"library_600x900*.jpg", 2);
    if (!found.empty()) return found;
    if (FileExists(lc + appId + L"_header.jpg")) return lc + appId + L"_header.jpg";
    return FindFileRecursive(lc + appId, L"header*.jpg", 2);
}

std::set<std::wstring> ParseExcluded(const std::wstring& s) {
    std::set<std::wstring> out;
    std::wstring cur;
    for (wchar_t c : s + L",") {
        if (c >= L'0' && c <= L'9') {
            cur += c;
        } else if (!cur.empty()) {
            out.insert(cur);
            cur.clear();
        }
    }
    return out;
}

std::string WebUrl(const Settings& s, const std::string& url) {
    return s.webInSteam ? "steam://openurl/" + url : url;
}

void ScanSteam(const Settings& s, std::vector<Game>& games) {
    std::wstring steam = GetSteamPath();
    if (steam.empty()) return;
    std::wstring userDir = GetSteamUserDir(steam);
    auto stats = ReadSteamStats(userDir);
    auto excluded = ParseExcluded(s.excluded);
    std::set<std::wstring> seenIds;

    for (const auto& lib : GetSteamLibraries(steam)) {
        std::wstring apps = lib + L"\\steamapps\\";
        WIN32_FIND_DATAW fd;
        HANDLE f = FindFirstFileW((apps + L"appmanifest_*.acf").c_str(), &fd);
        if (f == INVALID_HANDLE_VALUE) continue;
        do {
            std::string data;
            if (!ReadFileBytes(apps + fd.cFileName, data)) continue;
            std::string appId, name, installDir;
            long long flags = 0, lastPlayed = 0, size = 0, lastUpdated = 0;
            for (const auto& kv : VdfFlatten(VdfTokenize(data))) {
                if (kv.depth != 1) continue;
                const char* k = kv.key.c_str();
                if (_stricmp(k, "appid") == 0) appId = kv.value;
                else if (_stricmp(k, "name") == 0) name = kv.value;
                else if (_stricmp(k, "installdir") == 0) installDir = kv.value;
                else if (_stricmp(k, "StateFlags") == 0) flags = _atoi64(kv.value.c_str());
                else if (_stricmp(k, "LastPlayed") == 0) lastPlayed = _atoi64(kv.value.c_str());
                else if (_stricmp(k, "SizeOnDisk") == 0) size = _atoi64(kv.value.c_str());
                else if (_stricmp(k, "LastUpdated") == 0) lastUpdated = _atoi64(kv.value.c_str());
            }
            // StateFlags: 4 = fully installed, 2 = update required.
            if (appId.empty() || installDir.empty() || !(flags & 4)) continue;
            Game g;
            g.platform = P_STEAM;
            g.id = Utf8ToWide(appId);
            if (excluded.count(g.id) || !seenIds.insert(g.id).second) continue;
            std::wstring dirW = Utf8ToWide(installDir);
            g.dir = IsAbsolute(dirW) ? NormalizePath(dirW)
                                     : lib + L"\\steamapps\\common\\" + dirW;
            if (!GetDirInfo(g.dir, &g.installTime)) continue;
            g.name = Utf8ToWide(name.empty() ? installDir : name);
            g.launchUrl = "steam://rungameid/" + appId;
            g.size = size > 0 ? (ULONGLONG)size : 0;
            g.updatePending = (flags & 2) != 0;
            g.hasStats = true;
            g.lastPlayed = lastPlayed;
            auto it = stats.find(g.id);
            if (it != stats.end()) {
                g.playMinutes = it->second.playMinutes;
                // Steam also writes LastPlayed in localconfig.vdf when a game is
                // installed: ignore it when it matches the install/update time.
                long long accountLastPlayed = it->second.lastPlayed;
                bool setByInstall = lastUpdated > 0 &&
                                    std::llabs(accountLastPlayed - lastUpdated) <= 15 * 60;
                if (!setByInstall) {
                    g.lastPlayed = std::max(g.lastPlayed, accountLastPlayed);
                }
            }
            g.coverImage = FindSteamCover(steam, g.id);
            if (!userDir.empty()) {
                std::wstring saves = userDir + L"\\" + g.id + L"\\remote";
                if (DirExists(saves)) g.savesDir = saves;
                std::wstring shots = userDir + L"\\760\\remote\\" + g.id + L"\\screenshots";
                if (DirExists(shots)) g.shotsDir = shots;
            }
            g.actionUrls[L".library.url"] = "steam://nav/games/details/" + appId;
            g.actionUrls[L".store.url"] = "steam://store/" + appId;
            g.actionUrls[L".community.url"] =
                WebUrl(s, "https://steamcommunity.com/app/" + appId);
            g.actionUrls[L".guides.url"] =
                WebUrl(s, "https://steamcommunity.com/app/" + appId + "/guides/");
            g.actionUrls[L".achievements.url"] =
                WebUrl(s, "https://steamcommunity.com/stats/" + appId + "/achievements/");
            g.actionUrls[L".verify.url"] = "steam://validate/" + appId;
            g.actionUrls[L".uninstall.url"] = "steam://uninstall/" + appId;
            games.push_back(std::move(g));
        } while (FindNextFileW(f, &fd));
        FindClose(f);
    }
}

// ---------------------------------------------------------------- Steam library

// Names of the games in Steam's binary appinfo.vdf (formats v27, v28, v29).
struct AppInfoCache {
    ULONGLONG mtime = 0;
    ULONGLONG parsedAt = 0;               // GetTickCount64
    std::set<uint32_t> known;             // parsed app IDs (any type)
    std::map<uint32_t, std::wstring> gameNames;
    std::map<std::wstring, std::wstring> covers;  // appid -> cover image
};
AppInfoCache g_appInfo;

struct KvCursor {
    const uint8_t* p;
    const uint8_t* end;
    const std::vector<std::string>* strings;  // v29 key table (else null)
};

bool KvCString(KvCursor& c, std::string& out) {
    const uint8_t* z = (const uint8_t*)memchr(c.p, 0, c.end - c.p);
    if (!z) return false;
    out.assign((const char*)c.p, z - c.p);
    c.p = z + 1;
    return true;
}

bool KvKey(KvCursor& c, std::string& key) {
    if (!c.strings) return KvCString(c, key);
    if (c.end - c.p < 4) return false;
    uint32_t index;
    memcpy(&index, c.p, 4);
    c.p += 4;
    if (index >= c.strings->size()) return false;
    key = (*c.strings)[index];
    return true;
}

// Walks a binary KeyValues block, picking appinfo/common/{name,type}.
bool KvWalk(KvCursor& c, int depth, bool inCommon, std::string& name, std::string& type) {
    if (depth > 64) return false;
    while (c.p < c.end) {
        uint8_t t = *c.p++;
        if (t == 8 || t == 11) return true;
        std::string key;
        if (!KvKey(c, key)) return false;
        switch (t) {
            case 0:
                if (!KvWalk(c, depth + 1, depth == 1 && _stricmp(key.c_str(), "common") == 0,
                            name, type)) {
                    return false;
                }
                break;
            case 1: {
                std::string value;
                if (!KvCString(c, value)) return false;
                if (inCommon && _stricmp(key.c_str(), "name") == 0) name = value;
                if (inCommon && _stricmp(key.c_str(), "type") == 0) type = value;
                break;
            }
            case 5:  // wide string
                while (c.end - c.p >= 2 && (c.p[0] || c.p[1])) c.p += 2;
                if (c.end - c.p < 2) return false;
                c.p += 2;
                break;
            case 2: case 3: case 4: case 6:
                if (c.end - c.p < 4) return false;
                c.p += 4;
                break;
            case 7: case 10:
                if (c.end - c.p < 8) return false;
                c.p += 8;
                break;
            default:
                return false;
        }
    }
    return false;
}

bool ReadExact(HANDLE h, void* buf, DWORD size) {
    DWORD read = 0;
    return ReadFile(h, buf, size, &read, nullptr) && read == size;
}

void ParseAppInfo(const std::wstring& path, const std::set<uint32_t>& wanted) {
    HANDLE h = CreateFileW(path.c_str(), GENERIC_READ,
                           FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, nullptr,
                           OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, nullptr);
    if (h == INVALID_HANDLE_VALUE) return;
    uint32_t magic = 0, universe = 0;
    if (!ReadExact(h, &magic, 4) || !ReadExact(h, &universe, 4) ||
        (magic >> 8) != 0x075644 || (magic & 0xFF) < 0x27 || (magic & 0xFF) > 0x29) {
        CloseHandle(h);
        return;
    }
    int version = magic & 0xFF;
    std::vector<std::string> strings;
    if (version >= 0x29) {
        long long tableOffset = 0;
        LARGE_INTEGER pos = {}, fileSize = {}, here = {};
        if (!ReadExact(h, &tableOffset, 8) || !GetFileSizeEx(h, &fileSize) ||
            tableOffset <= 0 || tableOffset >= fileSize.QuadPart ||
            fileSize.QuadPart - tableOffset > 256 * 1024 * 1024) {
            CloseHandle(h);
            return;
        }
        SetFilePointerEx(h, {}, &here, FILE_CURRENT);
        pos.QuadPart = tableOffset;
        SetFilePointerEx(h, pos, nullptr, FILE_BEGIN);
        std::string table((size_t)(fileSize.QuadPart - tableOffset), '\0');
        if (!ReadExact(h, &table[0], (DWORD)table.size()) || table.size() < 4) {
            CloseHandle(h);
            return;
        }
        uint32_t count;
        memcpy(&count, table.data(), 4);
        size_t i = 4;
        for (uint32_t n = 0; n < count && i < table.size(); n++) {
            size_t z = table.find('\0', i);
            if (z == std::string::npos) break;
            strings.push_back(table.substr(i, z - i));
            i = z + 1;
        }
        SetFilePointerEx(h, here, nullptr, FILE_BEGIN);
    }
    const DWORD headerSize = version >= 0x28 ? 60 : 40;
    std::vector<uint8_t> buf;
    for (;;) {
        uint32_t appId = 0, size = 0;
        if (!ReadExact(h, &appId, 4) || appId == 0 || !ReadExact(h, &size, 4)) break;
        if (!wanted.count(appId) || size < headerSize || size > 64 * 1024 * 1024) {
            LARGE_INTEGER skip;
            skip.QuadPart = size;
            if (!SetFilePointerEx(h, skip, nullptr, FILE_CURRENT)) break;
            continue;
        }
        buf.resize(size);
        if (!ReadExact(h, buf.data(), size)) break;
        g_appInfo.known.insert(appId);
        KvCursor c{buf.data() + headerSize, buf.data() + size,
                   version >= 0x29 ? &strings : nullptr};
        std::string name, type;
        KvWalk(c, 0, false, name, type);
        if (!name.empty() && _stricmp(type.c_str(), "game") == 0) {
            g_appInfo.gameNames[appId] = Utf8ToWide(name);
        }
    }
    CloseHandle(h);
}

// App IDs in the user's library: Steam caches artwork for each of them.
std::set<uint32_t> SteamLibraryAppIds(const std::wstring& steam,
                                      const std::map<std::wstring, SteamStats>& stats) {
    std::set<uint32_t> ids;
    WIN32_FIND_DATAW fd;
    HANDLE h = FindFirstFileW((steam + L"\\appcache\\librarycache\\*").c_str(), &fd);
    if (h != INVALID_HANDLE_VALUE) {
        do {
            const wchar_t* n = fd.cFileName;
            if (*n < L'0' || *n > L'9') continue;
            uint32_t id = (uint32_t)wcstoul(n, nullptr, 10);
            if (id) ids.insert(id);
        } while (FindNextFileW(h, &fd));
        FindClose(h);
    }
    for (const auto& st : stats) {
        if (st.second.playMinutes > 0) ids.insert((uint32_t)wcstoul(st.first.c_str(), nullptr, 10));
    }
    ids.erase(0);
    return ids;
}

// Games the user hid in the Steam library ("Hidden" collection).
std::set<std::wstring> SteamHiddenAppIds(const std::wstring& userDir) {
    std::set<std::wstring> ids;
    std::string data;
    if (userDir.empty() ||
        !ReadFileBytes(userDir + L"\\config\\cloudstorage\\cloud-storage-namespace-1.json",
                       data)) {
        return ids;
    }
    size_t p = data.find("user-collections.hidden");
    if (p == std::string::npos) return ids;
    p = data.find("added", p);
    if (p == std::string::npos) return ids;
    p = data.find('[', p);
    size_t end = p == std::string::npos ? p : data.find(']', p);
    if (end == std::string::npos) return ids;
    std::string cur;
    for (size_t i = p + 1; i <= end; i++) {
        char c = data[i];
        if (c >= '0' && c <= '9') {
            cur += c;
        } else if (!cur.empty()) {
            ids.insert(Utf8ToWide(cur));
            cur.clear();
        }
    }
    return ids;
}

void ScanSteamLibrary(const Settings& s, std::vector<Game>& games) {
    std::wstring steam = GetSteamPath();
    if (steam.empty()) return;
    std::set<std::wstring> installed;
    for (const auto& g : games) {
        if (g.platform == P_STEAM) installed.insert(g.id);
    }
    auto excluded = ParseExcluded(s.excluded);
    std::wstring userDir = GetSteamUserDir(steam);
    for (const auto& id : SteamHiddenAppIds(userDir)) excluded.insert(id);
    auto stats = ReadSteamStats(userDir);
    auto ids = SteamLibraryAppIds(steam, stats);

    // appinfo.vdf changes often while Steam runs: re-read it at most every
    // 30 minutes, or after 5 minutes when the library has unknown games.
    std::wstring appinfo = steam + L"\\appcache\\appinfo.vdf";
    WIN32_FILE_ATTRIBUTE_DATA a;
    ULONGLONG mtime = GetFileAttributesExW(appinfo.c_str(), GetFileExInfoStandard, &a)
                          ? FileTimeValue(a.ftLastWriteTime)
                          : 0;
    bool missing = false;
    for (uint32_t id : ids) {
        if (!g_appInfo.known.count(id)) {
            missing = true;
            break;
        }
    }
    ULONGLONG now = GetTickCount64();
    ULONGLONG age = now - g_appInfo.parsedAt;
    if (g_appInfo.parsedAt == 0 || (mtime != g_appInfo.mtime && age > 30 * 60 * 1000) ||
        (missing && age > 5 * 60 * 1000)) {
        g_appInfo.known.clear();
        g_appInfo.gameNames.clear();
        g_appInfo.covers.clear();
        ParseAppInfo(appinfo, ids);
        g_appInfo.mtime = mtime;
        g_appInfo.parsedAt = now ? now : 1;
    }

    for (uint32_t id : ids) {
        if (Stopping()) return;
        auto nameIt = g_appInfo.gameNames.find(id);
        if (nameIt == g_appInfo.gameNames.end()) continue;  // not a game
        Game g;
        g.platform = P_STEAM;
        g.notInstalled = true;
        g.id = std::to_wstring(id);
        if (installed.count(g.id) || excluded.count(g.id)) continue;
        g.name = nameIt->second;
        g.hasStats = true;
        auto st = stats.find(g.id);
        if (st != stats.end()) {
            g.playMinutes = st->second.playMinutes;
            g.lastPlayed = st->second.lastPlayed;
        }
        auto cover = g_appInfo.covers.find(g.id);
        if (cover == g_appInfo.covers.end()) {
            cover = g_appInfo.covers.emplace(g.id, FindSteamCover(steam, g.id)).first;
        }
        g.coverImage = cover->second;
        std::string appId = WideToUtf8(g.id);
        g.actionUrls[kInstallSuffix] = "steam://install/" + appId;
        g.actionUrls[L".store.url"] = "steam://store/" + appId;
        g.actionUrls[L".library.url"] = "steam://nav/games/details/" + appId;
        g.actionUrls[L".community.url"] =
            WebUrl(s, "https://steamcommunity.com/app/" + appId);
        games.push_back(std::move(g));
    }
}

// ---------------------------------------------------------------- Epic

std::wstring EpicManifestsDir() {
    std::wstring data = RegReadStr(HKEY_LOCAL_MACHINE,
                                   L"SOFTWARE\\WOW6432Node\\Epic Games\\EpicGamesLauncher",
                                   L"AppDataPath");
    std::wstring dir =
        data.empty() ? ExpandEnv(L"%ProgramData%\\Epic\\EpicGamesLauncher\\Data\\Manifests")
                     : NormalizePath(data) + L"\\Manifests";
    return DirExists(dir) ? dir : std::wstring();
}

void ScanEpic(std::vector<Game>& games) {
    std::wstring dir = EpicManifestsDir();
    if (dir.empty()) return;
    std::set<std::wstring> seenIds;
    WIN32_FIND_DATAW fd;
    HANDLE f = FindFirstFileW((dir + L"\\*.item").c_str(), &fd);
    if (f == INVALID_HANDLE_VALUE) return;
    do {
        std::string data;
        if (!ReadTextUtf8(dir + L"\\" + fd.cFileName, data)) continue;
        std::map<std::string, std::string> m;
        JsonReader reader(data);
        if (!reader.ReadTopLevel(m)) continue;

        std::string appName = m["AppName"];
        std::string mainApp = m["MainGameAppName"];
        std::string categories = "|" + LowerA(m["AppCategories"]) + "|";
        if (appName.empty() || LowerA(m["bIsIncompleteInstall"]) == "true") continue;
        if (!mainApp.empty() && mainApp != appName) continue;  // DLC
        if (categories != "||" && categories.find("|games|") == std::string::npos) {
            continue;  // engines, plugins...
        }
        Game g;
        g.platform = P_EPIC;
        g.id = Utf8ToWide(appName);
        if (!seenIds.insert(g.id).second) continue;
        g.dir = NormalizePath(Utf8ToWide(m["InstallLocation"]));
        if (g.dir.empty() || !GetDirInfo(g.dir, &g.installTime)) continue;
        g.name = Utf8ToWide(m["DisplayName"].empty() ? appName : m["DisplayName"]);
        g.launchUrl = "com.epicgames.launcher://apps/" + m["CatalogNamespace"] + "%3A" +
                      m["CatalogItemId"] + "%3A" + appName + "?action=launch&silent=true";
        if (!m["LaunchExecutable"].empty()) {
            std::wstring exe =
                g.dir + L"\\" + NormalizePath(Utf8ToWide(m["LaunchExecutable"]));
            if (FileExists(exe)) g.iconExe = exe;
        }
        long long size = _atoi64(m["InstallSize"].c_str());
        g.size = size > 0 ? (ULONGLONG)size : 0;
        games.push_back(std::move(g));
    } while (FindNextFileW(f, &fd));
    FindClose(f);
}

// ---------------------------------------------------------------- GOG

const wchar_t* const kGogKeys[] = {
    L"SOFTWARE\\WOW6432Node\\GOG.com\\Games",
    L"SOFTWARE\\GOG.com\\Games",
};

void ScanGog(std::vector<Game>& games) {
    std::set<std::wstring> seenIds;
    for (auto root : kGogKeys) {
        HKEY h;
        if (RegOpenKeyExW(HKEY_LOCAL_MACHINE, root, 0, KEY_READ, &h) != ERROR_SUCCESS) {
            continue;
        }
        wchar_t sub[256];
        for (DWORD i = 0;; i++) {
            DWORD len = ARRAYSIZE(sub);
            if (RegEnumKeyExW(h, i, sub, &len, nullptr, nullptr, nullptr, nullptr) !=
                ERROR_SUCCESS) {
                break;
            }
            std::wstring key = std::wstring(root) + L"\\" + sub;
            auto val = [&](const wchar_t* name) {
                return RegReadStr(HKEY_LOCAL_MACHINE, key, name);
            };
            if (!val(L"dependsOn").empty()) continue;  // DLC
            Game g;
            g.platform = P_GOG;
            g.id = val(L"gameID");
            if (g.id.empty()) g.id = sub;
            if (!seenIds.insert(g.id).second) continue;
            g.dir = NormalizePath(val(L"path"));
            if (g.dir.empty() || !GetDirInfo(g.dir, &g.installTime)) continue;
            g.name = val(L"gameName");
            if (g.name.empty()) g.name = LeafName(g.dir);
            std::wstring exe = NormalizePath(val(L"exe"));
            if (!exe.empty() && !IsAbsolute(exe)) exe = g.dir + L"\\" + exe;
            if (!FileExists(exe)) continue;
            g.launchExe = exe;
            g.launchArgs = val(L"launchParam");
            std::wstring work = NormalizePath(val(L"workingDir"));
            if (!work.empty() && !IsAbsolute(work)) work = g.dir + L"\\" + work;
            g.launchWorkDir = DirExists(work) ? work : g.dir;
            g.iconExe = exe;
            g.actionUrls[L".galaxy.url"] = "goggalaxy://openGameView/" + WideToUtf8(g.id);
            games.push_back(std::move(g));
        }
        RegCloseKey(h);
    }
}

// ---------------------------------------------------------------- Xbox

// Folders where the Xbox app installs games (from each drive's .GamingRoot).
std::vector<std::wstring> XboxGameRoots() {
    std::vector<std::wstring> roots;
    std::set<std::wstring> seen;
    auto add = [&](const std::wstring& p) {
        std::wstring n = NormalizePath(p);
        if (DirExists(n) && seen.insert(Lower(n)).second) roots.push_back(n);
    };
    DWORD drives = GetLogicalDrives();
    for (int d = 0; d < 26; d++) {
        if (!(drives & (1u << d))) continue;
        std::wstring root = std::wstring(1, (wchar_t)(L'A' + d)) + L":\\";
        if (GetDriveTypeW(root.c_str()) != DRIVE_FIXED) continue;
        std::string data;
        if (ReadFileBytes(root + L".GamingRoot", data) && data.size() > 8 &&
            data.compare(0, 4, "RGBX") == 0) {
            std::wstring cur;
            auto flush = [&]() {
                while (!cur.empty() && cur[0] == L'\\') cur.erase(0, 1);
                if (!cur.empty()) add(IsAbsolute(cur) ? cur : root + cur);
                cur.clear();
            };
            for (size_t i = 8; i + 1 < data.size(); i += 2) {
                wchar_t c = (wchar_t)((unsigned char)data[i] |
                                      ((unsigned char)data[i + 1] << 8));
                if (c == 0) {
                    flush();
                } else if (c >= 32) {
                    cur += c;
                }
            }
            flush();
        }
        add(root + L"XboxGames");
    }
    return roots;
}

// Package name -> package family name, for packages installed for this user.
std::map<std::wstring, std::wstring> PackageFamilies() {
    std::map<std::wstring, std::wstring> out;
    HKEY h;
    if (RegOpenKeyExW(HKEY_CURRENT_USER,
                      L"Software\\Classes\\Local Settings\\Software\\Microsoft\\Windows\\"
                      L"CurrentVersion\\AppModel\\Repository\\Packages",
                      0, KEY_READ, &h) != ERROR_SUCCESS) {
        return out;
    }
    wchar_t sub[512];
    for (DWORD i = 0;; i++) {
        DWORD len = ARRAYSIZE(sub);
        if (RegEnumKeyExW(h, i, sub, &len, nullptr, nullptr, nullptr, nullptr) !=
            ERROR_SUCCESS) {
            break;
        }
        // Name_Version_Arch_ResourceId_PublisherId
        std::wstring full = sub;
        size_t first = full.find(L'_');
        size_t last = full.find_last_of(L'_');
        if (first == std::wstring::npos || last == first) continue;
        out[Lower(full.substr(0, first))] = full.substr(0, first) + full.substr(last);
    }
    RegCloseKey(h);
    return out;
}

std::wstring FindXboxLogo(const std::wstring& content, const std::string& rel) {
    if (rel.empty()) return {};
    std::wstring path = content + L"\\" + NormalizePath(Utf8ToWide(rel));
    if (FileExists(path)) return path;
    // Scaled variants: Logo.scale-200.png etc. Pick the largest.
    std::wstring dir = ParentDir(path), leaf = LeafName(path);
    size_t dot = leaf.find_last_of(L'.');
    std::wstring stem = dot == std::wstring::npos ? leaf : leaf.substr(0, dot);
    std::wstring best;
    ULONGLONG bestSize = 0;
    WIN32_FIND_DATAW fd;
    HANDLE h = FindFirstFileW((dir + L"\\" + stem + L"*.png").c_str(), &fd);
    if (h == INVALID_HANDLE_VALUE) return {};
    do {
        ULONGLONG size = ((ULONGLONG)fd.nFileSizeHigh << 32) | fd.nFileSizeLow;
        if (size > bestSize) {
            bestSize = size;
            best = dir + L"\\" + fd.cFileName;
        }
    } while (FindNextFileW(h, &fd));
    FindClose(h);
    return best;
}

void ScanXbox(std::vector<Game>& games) {
    auto families = PackageFamilies();
    std::set<std::wstring> seenIds;
    for (const auto& root : XboxGameRoots()) {
        WIN32_FIND_DATAW fd;
        HANDLE h = FindFirstFileW((root + L"\\*").c_str(), &fd);
        if (h == INVALID_HANDLE_VALUE) continue;
        do {
            std::wstring n = fd.cFileName;
            if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) || n == L"." || n == L"..") {
                continue;
            }
            std::wstring content = root + L"\\" + n + L"\\Content";
            std::string xml;
            if (!ReadTextUtf8(content + L"\\MicrosoftGame.config", xml)) continue;
            std::string identity = XmlAttr(xml, "Identity", "Name");
            std::string exeName = XmlAttr(xml, "Executable", "Name");
            std::string appId = XmlAttr(xml, "Executable", "Id");
            if (identity.empty() || appId.empty()) continue;
            auto fam = families.find(Lower(Utf8ToWide(identity)));
            if (fam == families.end()) continue;  // not registered for this user

            Game g;
            g.platform = P_XBOX;
            g.id = fam->second;
            if (!seenIds.insert(g.id).second) continue;
            g.dir = content;
            if (!GetDirInfo(g.dir, &g.installTime)) continue;
            std::string display = XmlAttr(xml, "ShellVisuals", "DefaultDisplayName");
            g.name = (display.empty() || display.rfind("ms-resource", 0) == 0)
                         ? n
                         : Utf8ToWide(display);
            g.launchExe = ExpandEnv(L"%SystemRoot%\\explorer.exe");
            g.launchArgs = L"shell:AppsFolder\\" + fam->second + L"!" + Utf8ToWide(appId);
            if (!exeName.empty()) {
                std::wstring exe = content + L"\\" + NormalizePath(Utf8ToWide(exeName));
                if (FileExists(exe)) g.iconExe = exe;
            }
            g.coverImage =
                FindXboxLogo(content, XmlAttr(xml, "ShellVisuals", "Square150x150Logo"));
            if (g.coverImage.empty()) {
                g.coverImage = FindXboxLogo(content, XmlAttr(xml, "ShellVisuals", "StoreLogo"));
            }
            games.push_back(std::move(g));
        } while (FindNextFileW(h, &fd));
        FindClose(h);
    }
}

// ============================================================ icons

bool IsUnwantedExe(const std::wstring& lowerName) {
    static const wchar_t* const kBad[] = {
        L"unins",    L"setup",      L"redist",     L"crash",
        L"report",   L"prereq",     L"dxsetup",    L"dotnet",
        L"install",  L"easyanticheat", L"start_protected_game",
        L"battleye", L"be_service", L"cefprocess", L"helper",
        L"update",   L"overlay",    L"uploader",   L"python",
        L"7z",       L"vc_",        L"diagnos",
    };
    for (auto bad : kBad) {
        if (lowerName.find(bad) != std::wstring::npos) return true;
    }
    return false;
}

// Heuristic: the shallowest, largest .exe (max 2 levels deep).
std::wstring FindGameExe(const std::wstring& dir) {
    std::wstring best;
    int bestDepth = 99;
    ULONGLONG bestSize = 0;
    std::vector<std::pair<std::wstring, int>> queue{{dir, 0}};
    size_t qi = 0;
    while (qi < queue.size() && qi < 200 && !Stopping()) {
        std::pair<std::wstring, int> item = queue[qi++];
        const std::wstring& cur = item.first;
        int depth = item.second;
        WIN32_FIND_DATAW fd;
        HANDLE h = FindFirstFileExW((cur + L"\\*").c_str(), FindExInfoBasic, &fd,
                                    FindExSearchNameMatch, nullptr,
                                    FIND_FIRST_EX_LARGE_FETCH);
        if (h == INVALID_HANDLE_VALUE) continue;
        do {
            std::wstring n = fd.cFileName;
            if (n == L"." || n == L"..") continue;
            if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                if (depth < 2 && !(fd.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT)) {
                    queue.push_back({cur + L"\\" + n, depth + 1});
                }
                continue;
            }
            std::wstring ln = Lower(n);
            if (!EndsWith(ln, L".exe") || IsUnwantedExe(ln)) continue;
            ULONGLONG size = ((ULONGLONG)fd.nFileSizeHigh << 32) | fd.nFileSizeLow;
            if (depth < bestDepth || (depth == bestDepth && size > bestSize)) {
                best = cur + L"\\" + n;
                bestDepth = depth;
                bestSize = size;
            }
        } while (FindNextFileW(h, &fd));
        FindClose(h);
        if (!best.empty() && qi < queue.size() && queue[qi].second > bestDepth) break;
    }
    return best;
}

// Converts an image (jpg/png...) into a 256x256 .ico, letterboxed.
bool MakeIconFromImage(const std::wstring& src, const std::wstring& dst) {
    const UINT S = 256;
    Com<IWICImagingFactory> factory;
    if (FAILED(CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER,
                                IID_PPV_ARGS(&factory)))) {
        return false;
    }
    Com<IWICBitmapDecoder> decoder;
    Com<IWICBitmapFrameDecode> frame;
    if (FAILED(factory->CreateDecoderFromFilename(src.c_str(), nullptr, GENERIC_READ,
                                                  WICDecodeMetadataCacheOnDemand,
                                                  &decoder)) ||
        FAILED(decoder->GetFrame(0, &frame))) {
        return false;
    }
    UINT w = 0, h = 0;
    if (FAILED(frame->GetSize(&w, &h)) || !w || !h) return false;
    double scale = std::min((double)S / w, (double)S / h);
    UINT nw = std::min(S, std::max(1u, (UINT)(w * scale + 0.5)));
    UINT nh = std::min(S, std::max(1u, (UINT)(h * scale + 0.5)));

    Com<IWICFormatConverter> converter;
    Com<IWICBitmapScaler> scaler;
    if (FAILED(factory->CreateFormatConverter(&converter)) ||
        FAILED(converter->Initialize(frame.p, GUID_WICPixelFormat32bppBGRA,
                                     WICBitmapDitherTypeNone, nullptr, 0,
                                     WICBitmapPaletteTypeCustom)) ||
        FAILED(factory->CreateBitmapScaler(&scaler)) ||
        FAILED(scaler->Initialize(converter.p, nw, nh, WICBitmapInterpolationModeFant))) {
        return false;
    }
    std::vector<BYTE> pixels(S * S * 4, 0);
    size_t offset = ((size_t)((S - nh) / 2) * S + (S - nw) / 2) * 4;
    if (FAILED(scaler->CopyPixels(nullptr, S * 4, (UINT)(pixels.size() - offset),
                                  pixels.data() + offset))) {
        return false;
    }

    Com<IWICBitmap> bitmap;
    Com<IStream> stream;
    Com<IWICBitmapEncoder> encoder;
    Com<IWICBitmapFrameEncode> frameEncode;
    WICPixelFormatGUID format = GUID_WICPixelFormat32bppBGRA;
    if (FAILED(factory->CreateBitmapFromMemory(S, S, GUID_WICPixelFormat32bppBGRA, S * 4,
                                               (UINT)pixels.size(), pixels.data(),
                                               &bitmap)) ||
        FAILED(CreateStreamOnHGlobal(nullptr, TRUE, &stream)) ||
        FAILED(factory->CreateEncoder(GUID_ContainerFormatPng, nullptr, &encoder)) ||
        FAILED(encoder->Initialize(stream.p, WICBitmapEncoderNoCache)) ||
        FAILED(encoder->CreateNewFrame(&frameEncode, nullptr)) ||
        FAILED(frameEncode->Initialize(nullptr)) || FAILED(frameEncode->SetSize(S, S)) ||
        FAILED(frameEncode->SetPixelFormat(&format)) ||
        FAILED(frameEncode->WriteSource(bitmap.p, nullptr)) ||
        FAILED(frameEncode->Commit()) || FAILED(encoder->Commit())) {
        return false;
    }

    STATSTG stat = {};
    HGLOBAL global = nullptr;
    if (FAILED(stream->Stat(&stat, STATFLAG_NONAME)) ||
        FAILED(GetHGlobalFromStream(stream.p, &global))) {
        return false;
    }
    DWORD pngSize = (DWORD)stat.cbSize.QuadPart;
    const BYTE* png = (const BYTE*)GlobalLock(global);
    if (!png) return false;

    // ICONDIR (6 bytes) + one ICONDIRENTRY (16 bytes) + PNG data.
    std::string ico(22, '\0');
    auto put16 = [&](size_t at, WORD v) { memcpy(&ico[at], &v, 2); };
    auto put32 = [&](size_t at, DWORD v) { memcpy(&ico[at], &v, 4); };
    put16(2, 1);    // type: icon
    put16(4, 1);    // image count
    put16(10, 1);   // planes
    put16(12, 32);  // bits per pixel
    put32(14, pngSize);
    put32(18, 22);
    ico.append((const char*)png, pngSize);
    GlobalUnlock(global);
    return WriteFileBytes(dst, ico);
}

// ============================================================ shortcuts

void SetFileTimes(const std::wstring& path, const FILETIME* created,
                  const FILETIME* modified) {
    HANDLE h = CreateFileW(path.c_str(), FILE_WRITE_ATTRIBUTES,
                           FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, nullptr,
                           OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, nullptr);
    if (h != INVALID_HANDLE_VALUE) {
        SetFileTime(h, created, nullptr, modified);
        CloseHandle(h);
    }
}

bool CreateLink(const std::wstring& lnkPath, const std::wstring& target,
                const std::wstring& args, const std::wstring& workDir,
                const std::wstring& icon, const std::wstring& description,
                const Game* stats) {
    Com<IShellLinkW> link;
    if (FAILED(CoCreateInstance(CLSID_ShellLink, nullptr, CLSCTX_INPROC_SERVER,
                                IID_PPV_ARGS(&link)))) {
        return false;
    }
    link->SetPath(target.c_str());
    if (!args.empty()) link->SetArguments(args.c_str());
    if (!workDir.empty()) link->SetWorkingDirectory(workDir.c_str());
    if (!icon.empty()) link->SetIconLocation(icon.c_str(), 0);
    if (!description.empty()) link->SetDescription(description.c_str());

    if (stats && stats->hasStats) {
        Com<IPropertyStore> store;
        if (SUCCEEDED(link->QueryInterface(IID_PPV_ARGS(&store)))) {
            PROPVARIANT pv;
            PropVariantInit(&pv);
            pv.vt = VT_UI8;
            pv.uhVal.QuadPart = (ULONGLONG)stats->playMinutes * 60ULL * 10000000ULL;
            store->SetValue(kPkeyMediaDuration, pv);
            if (stats->lastPlayed > 0) {
                PropVariantInit(&pv);
                pv.vt = VT_FILETIME;
                pv.filetime = UnixToFileTime(stats->lastPlayed);
                store->SetValue(kPkeyDateLastUsed, pv);
            }
            store->Commit();
        }
    }

    Com<IPersistFile> file;
    if (FAILED(link->QueryInterface(IID_PPV_ARGS(&file)))) return false;
    SetFileAttributesW(lnkPath.c_str(), FILE_ATTRIBUTE_NORMAL);
    return SUCCEEDED(file->Save(lnkPath.c_str(), TRUE));
}

// Target of a .lnk file (empty on failure).
std::wstring LinkTarget(const std::wstring& lnkPath) {
    Com<IShellLinkW> link;
    Com<IPersistFile> file;
    if (FAILED(CoCreateInstance(CLSID_ShellLink, nullptr, CLSCTX_INPROC_SERVER,
                                IID_PPV_ARGS(&link))) ||
        FAILED(link->QueryInterface(IID_PPV_ARGS(&file))) ||
        FAILED(file->Load(lnkPath.c_str(), STGM_READ))) {
        return {};
    }
    wchar_t path[MAX_PATH * 2] = {};
    if (FAILED(link->GetPath(path, ARRAYSIZE(path), nullptr, SLGP_RAWPATH))) return {};
    return path;
}

void ClearDirFiles(const std::wstring& dir, const std::wstring& keep = L"") {
    WIN32_FIND_DATAW fd;
    HANDLE h = FindFirstFileW((dir + L"\\*").c_str(), &fd);
    if (h == INVALID_HANDLE_VALUE) return;
    std::vector<std::wstring> files;
    do {
        if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
            files.push_back(dir + L"\\" + fd.cFileName);
        }
    } while (FindNextFileW(h, &fd));
    FindClose(h);
    for (const auto& p : files) {
        if (!keep.empty() && Lower(p) == Lower(keep)) continue;
        SetFileAttributesW(p.c_str(), FILE_ATTRIBUTE_NORMAL);
        DeleteFileW(p.c_str());
    }
}

void DeleteDirWithFiles(const std::wstring& dir) {
    ClearDirFiles(dir);
    SetFileAttributesW(dir.c_str(), FILE_ATTRIBUTE_NORMAL);
    RemoveDirectoryW(dir.c_str());
}

int GameBits(const Game& g) {
    return (g.savesDir.empty() ? 0 : kBitSaves) | (g.shotsDir.empty() ? 0 : kBitShots);
}

std::wstring TargetExt(const Game& g) {
    return g.notInstalled ? std::wstring(kLibExt) : ExtFor(g.platform, GameBits(g));
}

// The target file stores a hash of its data, so unchanged games are not
// rewritten after a restart.
bool GameDataUpToDate(const std::wstring& target, const std::wstring& sigHash) {
    std::string data;
    return ReadFileBytes(target, data) &&
           data.find("Sig=" + WideToUtf8(sigHash)) != std::string::npos;
}

// Writes the hidden per-game folder: the target file whose type gives the
// context menu, plus the files its verbs open.
void WriteGameData(const std::wstring& dir, const std::wstring& target, const Game& g,
                   const std::wstring& coverIcon, const std::wstring& sigHash) {
    SHCreateDirectoryExW(nullptr, dir.c_str(), nullptr);
    ClearDirFiles(dir, coverIcon);
    for (const auto& a : g.actionUrls) {
        WriteFileBytes(target + a.first,
                       "[InternetShortcut]\r\nURL=" + NoControlChars(a.second) + "\r\n");
    }
    if (!coverIcon.empty() && !FileExists(coverIcon)) {
        MakeIconFromImage(g.coverImage, coverIcon);
    }
    // Written last: its hash marks the folder as complete.
    auto writeTarget = [&]() {
        WriteFileBytes(target, "Id=" + WideToUtf8(g.id) + "\r\nSig=" + WideToUtf8(sigHash) +
                                   "\r\n");
    };
    if (g.notInstalled) {
        writeTarget();
        return;
    }

    if (!g.launchUrl.empty()) {
        WriteFileBytes(target + LaunchSuffix(g.platform),
                       "[InternetShortcut]\r\nURL=" + NoControlChars(g.launchUrl) + "\r\n");
    } else {
        CreateLink(target + LaunchSuffix(g.platform), g.launchExe, g.launchArgs,
                   g.launchWorkDir, L"", L"", nullptr);
    }
    CreateLink(target + kFolderSuffix, g.dir, L"", L"", L"", L"", nullptr);
    if (!g.savesDir.empty()) {
        CreateLink(target + kSavesSuffix, g.savesDir, L"", L"", L"", L"", nullptr);
    }
    if (!g.shotsDir.empty()) {
        CreateLink(target + kShotsSuffix, g.shotsDir, L"", L"", L"", L"", nullptr);
    }
    writeTarget();
}

std::wstring BuildComment(const Game& g) {
    std::vector<std::wstring> parts;
    if (g.notInstalled) {
        parts.push_back(Tr(S_NOT_INSTALLED));
        if (g.playMinutes > 0) {
            parts.push_back(std::wstring(Tr(S_PLAYTIME)) +
                            FormatPlaytime(g.playMinutes));
        }
        if (g.lastPlayed > 0) {
            parts.push_back(std::wstring(Tr(S_LAST_PLAYED)) +
                            FormatDate(UnixToFileTime(g.lastPlayed)));
        }
    }
    if (g.updatePending) {
        parts.push_back(std::wstring(L"\x27F3 ") +
                        Tr(S_UPDATE_PENDING));
    }
    if (g.hasStats && !g.notInstalled) {
        parts.push_back(std::wstring(Tr(S_PLAYTIME)) +
                        FormatPlaytime(g.playMinutes));
        parts.push_back(std::wstring(Tr(S_LAST_PLAYED)) +
                        (g.lastPlayed > 0 ? FormatDate(UnixToFileTime(g.lastPlayed))
                                          : std::wstring(Tr(S_NEVER))));
    }
    if (g.size) {
        parts.push_back(std::wstring(Tr(S_SIZE)) + FormatSize(g.size));
    }
    if (parts.empty()) parts.push_back(kPlatforms[g.platform].folder);
    std::wstring out;
    for (const auto& p : parts) {
        if (!out.empty()) out += L" \x00B7 ";
        out += p;
    }
    return out;
}

// ============================================================ sync

struct SyncCache {
    std::map<std::wstring, std::wstring> exeIcons;  // dir -> exe
    std::map<std::wstring, std::wstring> dataSigs;  // data dir -> signature hash
    std::map<std::wstring, std::wstring> linkSigs;  // lnk path (lower) -> hash
    std::wstring nodeSig;                           // platforms with games
    std::wstring linkSigsFile;                      // where linkSigs are saved
    bool linkSigsDirty = false;
    bool firstRun = true;
};

// Link signatures are saved in .data\links.sig ("hash<TAB>path" per line),
// so shortcuts are not all rewritten every time the tool process starts.
void LoadLinkSigs(SyncCache& cache, const std::wstring& file) {
    cache.linkSigs.clear();
    cache.linkSigsFile = file;
    cache.linkSigsDirty = false;
    std::string data;
    if (!ReadFileBytes(file, data)) return;
    size_t pos = 0;
    while (pos < data.size()) {
        size_t eol = data.find('\n', pos);
        if (eol == std::string::npos) eol = data.size();
        std::string line = data.substr(pos, eol - pos);
        size_t tab = line.find('\t');
        if (tab != std::string::npos) {
            cache.linkSigs[Utf8ToWide(line.substr(tab + 1))] = Utf8ToWide(line.substr(0, tab));
        }
        pos = eol + 1;
    }
}

void SaveLinkSigs(SyncCache& cache) {
    if (!cache.linkSigsDirty || cache.linkSigsFile.empty()) return;
    std::string data;
    for (const auto& e : cache.linkSigs) {
        data += WideToUtf8(e.second) + "\t" + WideToUtf8(e.first) + "\n";
    }
    if (WriteFileBytes(cache.linkSigsFile, data)) cache.linkSigsDirty = false;
}

// Data folders are named "<platform>-<id>" or "steamlib-<id>".
bool IsOurDataDirName(const std::wstring& name) {
    std::wstring n = Lower(name);
    if (StartsWith(n, L"steamlib-")) return true;
    for (int p = 0; p < P_COUNT; p++) {
        if (StartsWith(n, std::wstring(kPlatforms[p].code) + L"-")) return true;
    }
    return false;
}

// Removes our shortcuts that are not in "keep" (recursing into ★ folders).
// Returns true if something was deleted.
bool CleanFolder(const std::wstring& folder, const std::set<std::wstring>& keep,
                 const std::wstring& dataDirLower) {
    bool deleted = false;
    WIN32_FIND_DATAW fd;
    HANDLE h = FindFirstFileW((folder + L"\\*").c_str(), &fd);
    if (h == INVALID_HANDLE_VALUE) return false;
    std::vector<std::wstring> files, subdirs;
    do {
        std::wstring n = fd.cFileName;
        if (n == L"." || n == L"..") continue;
        if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            if (StartsWith(n, kSmartPrefix)) subdirs.push_back(folder + L"\\" + n);
        } else if (EndsWith(Lower(n), L".lnk")) {
            files.push_back(folder + L"\\" + n);
        }
    } while (FindNextFileW(h, &fd));
    FindClose(h);
    for (const auto& f : files) {
        if (keep.count(Lower(f))) continue;
        if (StartsWith(Lower(LinkTarget(f)), dataDirLower + L"\\") && DeleteFileW(f.c_str())) {
            deleted = true;
        }
    }
    for (const auto& d : subdirs) {
        if (CleanFolder(d, keep, dataDirLower)) deleted = true;
        RemoveDirectoryW(d.c_str());  // only succeeds when empty
    }
    return deleted;
}

// Deletes everything the mod created under "root" (only our files).
void CleanupRoot(const std::wstring& root) {
    if (root.empty() || !DirExists(root)) return;
    std::wstring dataDir = root + L"\\" + kDataFolder;

    std::set<std::wstring> none;
    std::wstring dataLower = Lower(dataDir);
    for (int p = 0; p < P_COUNT; p++) {
        std::wstring platformDir = root + L"\\" + kPlatforms[p].folder;
        CleanFolder(platformDir, none, dataLower);
        RemoveDirectoryW(platformDir.c_str());  // only when empty
    }

    if (DirExists(dataDir)) {
        std::vector<std::wstring> dirs;
        WIN32_FIND_DATAW fd;
        HANDLE h = FindFirstFileW((dataDir + L"\\*").c_str(), &fd);
        if (h != INVALID_HANDLE_VALUE) {
            do {
                std::wstring n = fd.cFileName;
                if ((fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && IsOurDataDirName(n)) {
                    dirs.push_back(dataDir + L"\\" + n);
                }
            } while (FindNextFileW(h, &fd));
            FindClose(h);
        }
        for (const auto& d : dirs) DeleteDirWithFiles(d);
        DeleteFileW((dataDir + L"\\links.sig").c_str());
        SetFileAttributesW(dataDir.c_str(), FILE_ATTRIBUTE_NORMAL);
        RemoveDirectoryW(dataDir.c_str());  // only when empty
    }
    RemoveDirectoryW(root.c_str());  // only when empty
    SHChangeNotify(SHCNE_UPDATEDIR, SHCNF_PATHW, ParentDir(root).c_str(), nullptr);
}

struct Placement {
    std::wstring folder;  // where the shortcut goes
    std::wstring name;    // file name without .lnk
};

void Sync(SyncCache& cache) {
    Settings s = GetSettings();
    std::wstring root = RootFolder(s);
    if (root.empty()) {
        Wh_Log(L"No folder available for the shortcuts");
        return;
    }
    std::wstring dataDir = root + L"\\" + kDataFolder;
    SHCreateDirectoryExW(nullptr, dataDir.c_str(), nullptr);
    SetFileAttributesW(dataDir.c_str(), FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM);

    std::vector<Game> games;
    if (s.enabled[P_STEAM]) ScanSteam(s, games);
    if (s.enabled[P_EPIC]) ScanEpic(games);
    if (s.enabled[P_GOG]) ScanGog(games);
    if (s.enabled[P_XBOX]) ScanXbox(games);
    if (s.enabled[P_STEAM] && s.showNotInstalled) ScanSteamLibrary(s, games);
    if (Stopping()) return;
    if (Lower(cache.linkSigsFile) != Lower(dataDir + L"\\links.sig")) {
        LoadLinkSigs(cache, dataDir + L"\\links.sig");
    }

    int counts[P_COUNT] = {};
    for (const auto& g : games) counts[g.platform]++;

    // Smart collections, per platform: name -> game indexes.
    std::wstring biggestName =
        std::wstring(kSmartPrefix) + Tr(S_C_LARGEST);
    std::map<std::wstring, std::vector<size_t>> smart[P_COUNT];
    {
        long long now = UnixNow();
        std::wstring recent =
            std::wstring(kSmartPrefix) + Tr(S_C_RECENT);
        std::wstring never = std::wstring(kSmartPrefix) + Tr(S_C_NEVER);
        std::wstring stale =
            std::wstring(kSmartPrefix) +
            TrFormat(S_C_STALE, std::to_wstring(s.staleMonths));
        std::wstring update =
            std::wstring(kSmartPrefix) + Tr(S_C_UPDATES);
        for (size_t i = 0; i < games.size(); i++) {
            const Game& g = games[i];
            if (g.notInstalled) continue;
            auto& m = smart[g.platform];
            if (g.hasStats) {
                long long age = now - g.lastPlayed;
                if (s.showRecent && g.lastPlayed > 0 &&
                    age <= (long long)s.recentDays * 86400) {
                    m[recent].push_back(i);
                }
                if (s.showNeverPlayed && g.lastPlayed == 0 && g.playMinutes == 0) {
                    m[never].push_back(i);
                }
                if (s.showStale && g.lastPlayed > 0 &&
                    age > (long long)s.staleMonths * 30 * 86400) {
                    m[stale].push_back(i);
                }
            }
            if (s.showUpdates && g.updatePending) m[update].push_back(i);
        }
        for (int p = 0; p < P_COUNT && s.showBiggest; p++) {
            std::vector<size_t> sized;
            for (size_t i = 0; i < games.size(); i++) {
                if (games[i].platform == p && games[i].size && !games[i].notInstalled) {
                    sized.push_back(i);
                }
            }
            std::sort(sized.begin(), sized.end(),
                      [&](size_t a, size_t b) { return games[a].size > games[b].size; });
            if (sized.size() > (size_t)s.biggestCount) sized.resize(s.biggestCount);
            if (!sized.empty()) smart[p][biggestName] = sized;
        }
    }

    // Shortcut names, unique per platform.
    std::vector<std::wstring> baseNames(games.size());
    std::set<std::wstring> used[P_COUNT];
    for (size_t i = 0; i < games.size(); i++) {
        std::wstring base = SanitizeName(games[i].name);
        if (!used[games[i].platform].insert(Lower(base)).second) {
            base += L" (" + SanitizeName(games[i].id) + L")";
            used[games[i].platform].insert(Lower(base));
        }
        baseNames[i] = base;
    }

    std::set<std::wstring> keepLinks, keepData;
    bool filesChanged = cache.firstRun;
    for (size_t i = 0; i < games.size(); i++) {
        if (Stopping()) return;
        const Game& g = games[i];
        std::wstring platformDir = root + L"\\" + kPlatforms[g.platform].folder;
        std::wstring gameData = dataDir + L"\\" + kPlatforms[g.platform].code +
                                (g.notInstalled ? L"lib-" : L"-") + SanitizeName(g.id);
        keepData.insert(Lower(gameData));

        // Icon: cover converted to .ico, or the game's executable.
        std::wstring coverIcon;
        if ((s.coverIcons || g.notInstalled) && !g.coverImage.empty()) {
            WIN32_FILE_ATTRIBUTE_DATA a;
            ULONGLONG stamp =
                GetFileAttributesExW(g.coverImage.c_str(), GetFileExInfoStandard, &a)
                    ? FileTimeValue(a.ftLastWriteTime)
                    : 0;
            coverIcon = gameData + L"\\cover-" +
                        HashString(g.coverImage + std::to_wstring(stamp)) + L".ico";
        }
        std::wstring exeIcon = g.iconExe;
        if (exeIcon.empty() && !g.dir.empty()) {
            auto it = cache.exeIcons.find(Lower(g.dir));
            if (it != cache.exeIcons.end()) {
                exeIcon = it->second;
            } else {
                exeIcon = FindGameExe(g.dir);
                cache.exeIcons[Lower(g.dir)] = exeIcon;
            }
        }

        std::wstring target = gameData + L"\\game" + TargetExt(g);
        std::wstring dataSig = target + L"|" + Utf8ToWide(g.launchUrl) + L"|" +
                               g.launchExe + L"|" + g.launchArgs + L"|" + g.dir + L"|" +
                               g.savesDir + L"|" + g.shotsDir + L"|" + coverIcon;
        for (const auto& a : g.actionUrls) dataSig += L"|" + Utf8ToWide(a.second);
        std::wstring dataHash = HashString(dataSig);
        bool dataOk = cache.dataSigs[gameData] == dataHash
                          ? FileExists(target)
                          : GameDataUpToDate(target, dataHash);
        if (!dataOk) {
            WriteGameData(gameData, target, g, coverIcon, dataHash);
            filesChanged = true;
        }
        cache.dataSigs[gameData] = dataHash;
        std::wstring icon = (!coverIcon.empty() && FileExists(coverIcon)) ? coverIcon : exeIcon;
        if (icon.empty()) icon = PlatformIcon(g.platform);

        // Where the game appears: its platform folder plus matching collections.
        std::vector<Placement> places;
        if (g.notInstalled) {
            places.push_back({platformDir + L"\\" + kSmartPrefix +
                                  Tr(S_C_NOT_INSTALLED),
                              baseNames[i]});
        } else {
            places.push_back({platformDir, baseNames[i]});
        }
        for (const auto& coll : smart[g.platform]) {
            const auto& list = coll.second;
            auto pos = std::find(list.begin(), list.end(), i);
            if (pos == list.end()) continue;
            std::wstring name = baseNames[i];
            if (coll.first == biggestName) {
                wchar_t rank[8];
                swprintf(rank, 8, L"%02d - ", (int)(pos - list.begin()) + 1);
                name = rank + name;
            }
            places.push_back({platformDir + L"\\" + coll.first, name});
        }

        std::wstring comment = BuildComment(g);
        for (const auto& place : places) {
            std::wstring lnk = place.folder + L"\\" + place.name + L".lnk";
            std::wstring key = Lower(lnk);
            keepLinks.insert(key);
            std::wstring sig = HashString(target + L"|" + icon + L"|" + comment + L"|" +
                                          g.dir + L"|" + std::to_wstring(g.lastPlayed) +
                                          L"|" + std::to_wstring(g.playMinutes));
            if (cache.linkSigs[key] == sig && FileExists(lnk)) continue;
            SHCreateDirectoryExW(nullptr, place.folder.c_str(), nullptr);
            if (!CreateLink(lnk, target, L"", g.dir, icon, comment, &g)) continue;
            // "Date created" = install date, "Date modified" = last launch.
            FILETIME modified = g.lastPlayed > 0 ? UnixToFileTime(g.lastPlayed) : g.installTime;
            bool hasInstall = FileTimeValue(g.installTime) != 0;
            SetFileTimes(lnk, hasInstall ? &g.installTime : nullptr,
                         (g.lastPlayed > 0 || hasInstall) ? &modified : nullptr);
            cache.linkSigs[key] = sig;
            cache.linkSigsDirty = true;
            filesChanged = true;
        }
    }

    // Remove what is no longer needed.
    std::wstring dataLower = Lower(dataDir);
    for (int p = 0; p < P_COUNT; p++) {
        std::wstring platformDir = root + L"\\" + kPlatforms[p].folder;
        if (CleanFolder(platformDir, keepLinks, dataLower)) filesChanged = true;
        if (!counts[p] && RemoveDirectoryW(platformDir.c_str())) filesChanged = true;
    }
    {
        std::vector<std::wstring> stale;
        WIN32_FIND_DATAW fd;
        HANDLE h = FindFirstFileW((dataDir + L"\\*").c_str(), &fd);
        if (h != INVALID_HANDLE_VALUE) {
            do {
                std::wstring n = fd.cFileName;
                if ((fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && IsOurDataDirName(n) &&
                    !keepData.count(Lower(dataDir + L"\\" + n))) {
                    stale.push_back(dataDir + L"\\" + n);
                }
            } while (FindNextFileW(h, &fd));
            FindClose(h);
        }
        for (const auto& d : stale) {
            DeleteDirWithFiles(d);
            cache.dataSigs.erase(d);
            filesChanged = true;
        }
    }
    for (auto it = cache.linkSigs.begin(); it != cache.linkSigs.end();) {
        if (keepLinks.count(it->first)) {
            ++it;
        } else {
            it = cache.linkSigs.erase(it);
            cache.linkSigsDirty = true;
        }
    }
    SaveLinkSigs(cache);

    // Explorer lists one navigation pane folder per platform folder (or the
    // single "Games" entry): ask it to refresh when that set changes.
    std::wstring nodeSig;
    for (int p = 0; p < P_COUNT; p++) nodeSig += counts[p] ? L"1" : L"0";
    if (nodeSig != cache.nodeSig) {
        cache.nodeSig = nodeSig;
        NotifyAssocChanged();
        NotifyDesktopChanged();
    }

    if (filesChanged) SHChangeNotify(SHCNE_UPDATEDIR, SHCNF_PATHW, root.c_str(), nullptr);
    cache.firstRun = false;
    Wh_Log(L"Synced %d games", (int)games.size());
}

// ============================================================ worker

struct Watches {
    std::vector<HANDLE> handles;  // [0] stop, [1] resync, then the rest
    size_t firstDirHandle = 2;
    size_t dirHandleCount = 0;
    HKEY gogKey = nullptr;
    HANDLE gogEvent = nullptr;

    void Close() {
        for (size_t i = 0; i < dirHandleCount; i++) {
            FindCloseChangeNotification(handles[firstDirHandle + i]);
        }
        if (gogEvent) CloseHandle(gogEvent);
        if (gogKey) RegCloseKey(gogKey);
    }
};

Watches SetUpWatches(const Settings& s) {
    Watches w;
    w.handles = {g_stopEvent, g_resyncEvent};
    std::vector<std::wstring> dirs;
    if (s.enabled[P_STEAM]) {
        std::wstring steam = GetSteamPath();
        if (!steam.empty()) {
            for (const auto& lib : GetSteamLibraries(steam)) {
                dirs.push_back(lib + L"\\steamapps");
            }
            std::wstring user = GetSteamUserDir(steam);
            if (!user.empty()) dirs.push_back(user + L"\\config");
        }
    }
    if (s.enabled[P_EPIC]) {
        std::wstring epic = EpicManifestsDir();
        if (!epic.empty()) dirs.push_back(epic);
    }
    if (s.enabled[P_XBOX]) {
        for (const auto& r : XboxGameRoots()) dirs.push_back(r);
    }
    for (const auto& d : dirs) {
        if (w.handles.size() >= MAXIMUM_WAIT_OBJECTS - 1) break;
        HANDLE c = FindFirstChangeNotificationW(
            d.c_str(), FALSE,
            FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_DIR_NAME |
                FILE_NOTIFY_CHANGE_LAST_WRITE);
        if (c != INVALID_HANDLE_VALUE) w.handles.push_back(c);
    }
    w.dirHandleCount = w.handles.size() - w.firstDirHandle;
    if (s.enabled[P_GOG]) {
        for (auto key : kGogKeys) {
            if (RegOpenKeyExW(HKEY_LOCAL_MACHINE, key, 0, KEY_NOTIFY, &w.gogKey) ==
                ERROR_SUCCESS) {
                break;
            }
            w.gogKey = nullptr;
        }
        if (w.gogKey) {
            w.gogEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
            if (w.gogEvent &&
                RegNotifyChangeKeyValue(w.gogKey, TRUE,
                                        REG_NOTIFY_CHANGE_NAME | REG_NOTIFY_CHANGE_LAST_SET,
                                        w.gogEvent, TRUE) == ERROR_SUCCESS) {
                w.handles.push_back(w.gogEvent);
            }
        }
    }
    return w;
}

DWORD WINAPI Worker(LPVOID) {
    // Don't compete with Explorer and other startup apps at sign-in.
    WaitForSingleObject(g_stopEvent, 3000);
    DWORD r = 0;

    HRESULT hrCo = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    SyncCache cache;
    bool needRegister = true;
    const DWORD kPeriodicResync = 15 * 60 * 1000;

    std::wstring lastRoot;
    while (!Stopping()) {
        g_lang = GetSettings().lang;
        std::wstring root = RootFolder(GetSettings());
        if (!lastRoot.empty() && Lower(root) != Lower(lastRoot)) {
            CleanupRoot(lastRoot);  // the shortcut folder was moved
            cache.linkSigsFile.clear();
            cache.dataSigs.clear();
        }
        lastRoot = root;
        if (needRegister) {
            cache.nodeSig.clear();
            cache.firstRun = true;
            needRegister = false;
        }
        Sync(cache);
        if (Stopping()) break;

        Watches w = SetUpWatches(GetSettings());
        DWORD count = (DWORD)w.handles.size();
        auto rearm = [&](DWORD index) {
            if (index >= w.firstDirHandle && index < w.firstDirHandle + w.dirHandleCount) {
                FindNextChangeNotification(w.handles[index]);
            }
        };
        r = WaitForMultipleObjects(count, w.handles.data(), FALSE, kPeriodicResync);
        if (r == WAIT_OBJECT_0 + 1) {
            needRegister = true;
        } else if (r >= WAIT_OBJECT_0 + 2 && r < WAIT_OBJECT_0 + count) {
            // Debounce: launchers rewrite these files often.
            rearm(r - WAIT_OBJECT_0);
            DWORD start = GetTickCount();
            for (;;) {
                DWORD r2 = WaitForMultipleObjects(count, w.handles.data(), FALSE, 3000);
                if (r2 == WAIT_OBJECT_0 + 1) {
                    needRegister = true;
                    break;
                }
                if (r2 < WAIT_OBJECT_0 + 2 || r2 >= WAIT_OBJECT_0 + count) break;
                rearm(r2 - WAIT_OBJECT_0);
                if (GetTickCount() - start > 30000) break;
            }
        } else if (r == WAIT_FAILED) {
            WaitForSingleObject(g_stopEvent, 5000);
        }
        w.Close();
    }

    Settings finalSettings = GetSettings();
    if (finalSettings.deleteOnDisable) {
        CleanupRoot(RootFolder(finalSettings));
    }
    if (SUCCEEDED(hrCo)) CoUninitialize();
    return 0;
}

std::wstring StringSetting(PCWSTR name) {
    return WindhawkUtils::StringSetting::make(name).get();
}

void LoadSettings() {
    Settings s;
    s.singleNode = StringSetting(L"layout") == L"single";
    std::wstring position = StringSetting(L"position");
    s.sortBase = position == L"top" ? 0xFF00 : position == L"bottom" ? 0x7F7F : 0x42;
    s.singleNodeName = StringSetting(L"singleNodeName");
    for (int p = 0; p < P_COUNT; p++) {
        s.enabled[p] = Wh_GetIntSetting(kPlatforms[p].setting) != 0;
    }
    s.coverIcons = StringSetting(L"iconStyle") == L"cover";
    s.lang = LanguageFromWindows();
    s.showRecent = Wh_GetIntSetting(L"showRecent") != 0;
    s.showNeverPlayed = Wh_GetIntSetting(L"showNeverPlayed") != 0;
    s.showStale = Wh_GetIntSetting(L"showStale") != 0;
    s.showBiggest = Wh_GetIntSetting(L"showBiggest") != 0;
    s.showUpdates = Wh_GetIntSetting(L"showUpdates") != 0;
    s.showNotInstalled = Wh_GetIntSetting(L"showNotInstalled") != 0;
    s.recentDays = std::max(1, Wh_GetIntSetting(L"recentDays"));
    s.staleMonths = std::max(1, Wh_GetIntSetting(L"staleMonths"));
    s.biggestCount = std::max(1, Wh_GetIntSetting(L"biggestCount"));
    s.webInSteam = Wh_GetIntSetting(L"webInSteam") != 0;
    s.folderPath = StringSetting(L"folderPath");
    s.deleteOnDisable = Wh_GetIntSetting(L"deleteOnDisable") != 0;
    s.excluded = StringSetting(L"excludedAppIds");

    AcquireSRWLockExclusive(&g_settingsLock);
    g_settings = std::move(s);
    ReleaseSRWLockExclusive(&g_settingsLock);
}

// ============================================================ explorer hooks
//
// In explorer.exe the mod only answers registry reads: the file types and the
// navigation pane folders come from the private hive (g_hive), and the
// NameSpace / HideDesktopIcons keys get the mod's entries added. Nothing is
// written to the real registry, and everything disappears when the mod is
// unloaded.

// Normalized, lowercase key paths the hooks care about.
const wchar_t kNsPath[] =
    L"hkcu\\software\\microsoft\\windows\\currentversion\\explorer\\desktop\\namespace";
const wchar_t kHidePath[] =
    L"hkcu\\software\\microsoft\\windows\\currentversion\\explorer\\hidedesktopicons\\"
    L"newstartpanel";
const wchar_t kHidePathClassic[] =
    L"hkcu\\software\\microsoft\\windows\\currentversion\\explorer\\hidedesktopicons\\"
    L"classicstartmenu";

bool IsHidePath(const std::wstring& path) {
    return path == kHidePath || path == kHidePathClassic;
}
const wchar_t kGuidPrefix[] = L"{7c3e1b52-9a4d-4f6b-8e21-3d5a6c9b0f4";

using NtQueryKey_t = LONG(NTAPI*)(HANDLE, int, PVOID, ULONG, PULONG);
NtQueryKey_t g_NtQueryKey = nullptr;

// Kernel paths of the hive's stand-in keys (lowercase).
std::wstring g_hiveNsKernel, g_hiveHideKernel;

// Handles of NameSpace / NewStartPanel keys opened through the hook. Only a
// fast pre-filter: the key path is always checked again before use.
SRWLOCK g_handlesLock = SRWLOCK_INIT;
std::set<HKEY> g_nsHandles, g_hideHandles;
volatile LONG g_trackedCount = 0;

bool ContainsI(const wchar_t* s, const wchar_t* needle) {
    size_t n = wcslen(needle);
    for (; *s; s++) {
        size_t i = 0;
        while (i < n && s[i] && towlower(s[i]) == needle[i]) i++;
        if (i == n) return true;
    }
    return false;
}

// Cheap test on a sub key or value name before doing any real work.
bool MightBeOurs(const wchar_t* sub) {
    return ContainsI(sub, L".wh") || ContainsI(sub, L"whgames.") ||
           ContainsI(sub, kGuidPrefix) || ContainsI(sub, L"namespace") ||
           ContainsI(sub, L"newstartpanel") || ContainsI(sub, L"classicstartmenu");
}

bool IsOurNodeName(const wchar_t* name) {
    if (!name || _wcsnicmp(name, kGuidPrefix, wcslen(kGuidPrefix)) != 0) return false;
    if (_wcsicmp(name, kClsidAll) == 0) return true;
    for (int p = 0; p < P_COUNT; p++) {
        if (_wcsicmp(name, kPlatforms[p].clsid) == 0) return true;
    }
    return false;
}

// "classes\..." for any classes root, "hkcu\..." for the current user's
// hive, "hklm\..." for the machine hive.
std::wstring NormalizeKeyPath(std::wstring k) {
    auto cut = [&](const std::wstring& prefix, const std::wstring& to) {
        if (k.compare(0, prefix.size(), prefix) == 0 &&
            (k.size() == prefix.size() || k[prefix.size()] == L'\\')) {
            k = to + k.substr(prefix.size());
            return true;
        }
        return false;
    };
    if (!g_hiveNsKernel.empty() && k == g_hiveNsKernel) return kNsPath;
    if (!g_hiveHideKernel.empty() && k == g_hiveHideKernel) return kHidePath;
    if (cut(L"\\registry\\machine\\software\\classes", L"classes")) return k;
    if (cut(L"\\registry\\machine", L"hklm")) return k;
    if (k.compare(0, 15, L"\\registry\\user\\") == 0) {
        size_t sidEnd = k.find(L'\\', 15);
        std::wstring sid = k.substr(15, sidEnd == std::wstring::npos ? std::wstring::npos
                                                                      : sidEnd - 15);
        std::wstring rest = sidEnd == std::wstring::npos ? L"" : k.substr(sidEnd);
        if (EndsWith(sid, L"_classes")) return L"classes" + rest;
        k = L"hkcu" + rest;
    }
    if (cut(L"hkcu\\software\\classes", L"classes")) return k;
    if (cut(L"hklm\\software\\classes", L"classes")) return k;
    return k.compare(0, 7, L"classes") == 0 || k.compare(0, 4, L"hkcu") == 0 ||
                   k.compare(0, 4, L"hklm") == 0
               ? k
               : std::wstring();
}

std::wstring KeyPath(HKEY h) {
    if (h == HKEY_CLASSES_ROOT) return L"classes";
    if (h == HKEY_CURRENT_USER) return L"hkcu";
    if (h == HKEY_LOCAL_MACHINE) return L"hklm";
    if (!g_NtQueryKey || ((ULONG_PTR)h & 0x80000000) == 0x80000000) return {};
    alignas(8) BYTE buf[4096];
    ULONG len = 0;
    if (g_NtQueryKey(h, 3 /* KeyNameInformation */, buf, sizeof(buf), &len) < 0) return {};
    ULONG nameLen = *(ULONG*)buf;
    if (nameLen > sizeof(buf) - sizeof(ULONG)) return {};
    return NormalizeKeyPath(Lower(std::wstring((wchar_t*)(buf + sizeof(ULONG)), nameLen / 2)));
}

std::wstring JoinKeyPath(const std::wstring& base, const wchar_t* sub) {
    if (base.empty()) return {};
    std::wstring s = sub ? Lower(sub) : L"";
    while (!s.empty() && s[0] == L'\\') s.erase(0, 1);
    while (!s.empty() && s.back() == L'\\') s.pop_back();
    return NormalizeKeyPath(s.empty() ? base : base + L"\\" + s);
}

// Path of the key in the hive that replaces "full", if it's one of ours.
bool MapToHive(const std::wstring& full, std::wstring& out) {
    if (full.compare(0, 8, L"classes\\") == 0) {
        std::wstring r = full.substr(8);
        std::wstring c1 = r.substr(0, r.find(L'\\'));
        bool ours = StartsWith(c1, L".whsteam") || StartsWith(c1, L".whepic") ||
                    StartsWith(c1, L".whgog") || StartsWith(c1, L".whxbox") ||
                    StartsWith(c1, L"whgames.");
        if (!ours && c1 == L"clsid" && r.size() > 6) {
            ours = IsOurNodeName(r.substr(6, 38).c_str());
        }
        if (ours) out = L"Classes\\" + r;
        return ours;
    }
    std::wstring ns = std::wstring(kNsPath) + L"\\";
    if (full.compare(0, ns.size(), ns) == 0 &&
        IsOurNodeName(full.substr(ns.size(), 38).c_str())) {
        out = L"NameSpaceNodes\\" + full.substr(ns.size());
        return true;
    }
    return false;
}

void TrackHandle(HKEY h, bool ns) {
    AcquireSRWLockExclusive(&g_handlesLock);
    (ns ? g_nsHandles : g_hideHandles).insert(h);
    g_trackedCount = (LONG)(g_nsHandles.size() + g_hideHandles.size());
    ReleaseSRWLockExclusive(&g_handlesLock);
}

// True if "h" is a NameSpace (ns) or NewStartPanel (!ns) key.
bool IsTrackedKey(HKEY h, bool ns) {
    if (!g_trackedCount) return false;
    AcquireSRWLockShared(&g_handlesLock);
    bool found = (ns ? g_nsHandles : g_hideHandles).count(h) > 0;
    ReleaseSRWLockShared(&g_handlesLock);
    if (!found) return false;
    std::wstring path = KeyPath(h);
    return ns ? path == kNsPath : IsHidePath(path);
}

// What the hooks need from the settings, computed once in ExplorerInit and
// ExplorerSettingsChanged so the hooks don't parse settings on every call.
struct NodeConfig {
    std::wstring root;
    bool enabled[P_COUNT] = {};
    bool singleNode = false;
};
SRWLOCK g_nodeConfigLock = SRWLOCK_INIT;
NodeConfig g_nodeConfig;

void UpdateNodeConfig(const Settings& s) {
    NodeConfig c;
    c.root = RootFolder(s);
    for (int p = 0; p < P_COUNT; p++) c.enabled[p] = s.enabled[p];
    c.singleNode = s.singleNode;
    AcquireSRWLockExclusive(&g_nodeConfigLock);
    g_nodeConfig = std::move(c);
    ReleaseSRWLockExclusive(&g_nodeConfigLock);
}

// Navigation pane folders to list: one per platform folder that exists (or
// the single "Games" entry).
std::vector<std::wstring> ActiveNodes() {
    AcquireSRWLockShared(&g_nodeConfigLock);
    NodeConfig c = g_nodeConfig;
    ReleaseSRWLockShared(&g_nodeConfigLock);
    std::vector<std::wstring> nodes;
    if (c.root.empty()) return nodes;
    bool any = false;
    for (int p = 0; p < P_COUNT; p++) {
        if (!c.enabled[p] || !DirExists(c.root + L"\\" + kPlatforms[p].folder)) continue;
        any = true;
        if (!c.singleNode) nodes.push_back(kPlatforms[p].clsid);
    }
    if (c.singleNode && any) nodes.push_back(kClsidAll);
    return nodes;
}

std::vector<std::wstring> AllNodes() {
    std::vector<std::wstring> nodes{kClsidAll};
    for (int p = 0; p < P_COUNT; p++) nodes.push_back(kPlatforms[p].clsid);
    return nodes;
}

LSTATUS ServeDword(LPDWORD type, LPBYTE data, LPDWORD cb) {
    if (type) *type = REG_DWORD;
    if (!cb) return data ? ERROR_INVALID_PARAMETER : ERROR_SUCCESS;
    if (data) {
        if (*cb < sizeof(DWORD)) {
            *cb = sizeof(DWORD);
            return ERROR_MORE_DATA;
        }
        *(DWORD*)data = 1;
    }
    *cb = sizeof(DWORD);
    return ERROR_SUCCESS;
}

using RegOpenKeyExW_t = decltype(&RegOpenKeyExW);
RegOpenKeyExW_t RegOpenKeyExW_Original;
using RegCloseKey_t = decltype(&RegCloseKey);
RegCloseKey_t RegCloseKey_Original;
using RegEnumKeyExW_t = decltype(&RegEnumKeyExW);
RegEnumKeyExW_t RegEnumKeyExW_Original;
using RegQueryInfoKeyW_t = decltype(&RegQueryInfoKeyW);
RegQueryInfoKeyW_t RegQueryInfoKeyW_Original;
using RegQueryValueExW_t = decltype(&RegQueryValueExW);
RegQueryValueExW_t RegQueryValueExW_Original;
using RegGetValueW_t = decltype(&RegGetValueW);
RegGetValueW_t RegGetValueW_Original;
using RegEnumValueW_t = decltype(&RegEnumValueW);
RegEnumValueW_t RegEnumValueW_Original;
using RegCreateKeyExW_t = decltype(&RegCreateKeyExW);
RegCreateKeyExW_t RegCreateKeyExW_Original;

LSTATUS WINAPI RegOpenKeyExW_Hook(HKEY hKey, LPCWSTR sub, DWORD options, REGSAM sam,
                                  PHKEY result) {
    HKEY hive = g_hive;
    if (!hive || !sub || !*sub || !result || !MightBeOurs(sub)) {
        return RegOpenKeyExW_Original(hKey, sub, options, sam, result);
    }
    std::wstring full = JoinKeyPath(KeyPath(hKey), sub);
    std::wstring hivePath;
    if (!full.empty() && MapToHive(full, hivePath)) {
        return RegOpenKeyExW_Original(hive, hivePath.c_str(), 0,
                                      sam & ~(KEY_WOW64_32KEY | KEY_WOW64_64KEY), result);
    }
    LSTATUS status = RegOpenKeyExW_Original(hKey, sub, options, sam, result);
    bool ns = full == kNsPath, hide = IsHidePath(full);
    if (!ns && !hide) return status;
    // The real key may not exist: hand out an empty read-only stand-in so the
    // mod's entries can still be added to it.
    const REGSAM kWriteAccess = KEY_SET_VALUE | KEY_CREATE_SUB_KEY | KEY_CREATE_LINK |
                                WRITE_DAC | WRITE_OWNER | DELETE | GENERIC_WRITE |
                                GENERIC_ALL | MAXIMUM_ALLOWED;
    if (status == ERROR_FILE_NOT_FOUND && !(sam & kWriteAccess)) {
        status = RegOpenKeyExW_Original(hive, ns ? L"NameSpace" : L"HideDesktopIcons", 0,
                                        KEY_READ, result);
    }
    if (status == ERROR_SUCCESS) TrackHandle(*result, ns);
    return status;
}

// Same as RegOpenKeyExW_Hook, for code that opens keys with RegCreateKeyExW.
LSTATUS WINAPI RegCreateKeyExW_Hook(HKEY hKey, LPCWSTR sub, DWORD reserved, LPWSTR cls,
                                    DWORD options, REGSAM sam,
                                    const LPSECURITY_ATTRIBUTES security, PHKEY result,
                                    LPDWORD disposition) {
    HKEY hive = g_hive;
    if (!hive || !sub || !*sub || !result || !MightBeOurs(sub)) {
        return RegCreateKeyExW_Original(hKey, sub, reserved, cls, options, sam, security,
                                        result, disposition);
    }
    std::wstring full = JoinKeyPath(KeyPath(hKey), sub);
    std::wstring hivePath;
    if (!full.empty() && MapToHive(full, hivePath)) {
        return RegCreateKeyExW_Original(hive, hivePath.c_str(), 0, nullptr, 0,
                                        sam & ~(KEY_WOW64_32KEY | KEY_WOW64_64KEY), nullptr,
                                        result, disposition);
    }
    LSTATUS status = RegCreateKeyExW_Original(hKey, sub, reserved, cls, options, sam,
                                              security, result, disposition);
    bool ns = full == kNsPath, hide = IsHidePath(full);
    if (status == ERROR_SUCCESS && (ns || hide)) TrackHandle(*result, ns);
    return status;
}

LSTATUS WINAPI RegCloseKey_Hook(HKEY hKey) {
    bool tracked = false;
    if (g_trackedCount) {
        AcquireSRWLockShared(&g_handlesLock);
        tracked = g_nsHandles.count(hKey) || g_hideHandles.count(hKey);
        ReleaseSRWLockShared(&g_handlesLock);
    }
    if (tracked) {
        AcquireSRWLockExclusive(&g_handlesLock);
        g_nsHandles.erase(hKey);
        g_hideHandles.erase(hKey);
        g_trackedCount = (LONG)(g_nsHandles.size() + g_hideHandles.size());
        ReleaseSRWLockExclusive(&g_handlesLock);
    }
    return RegCloseKey_Original(hKey);
}

// Adds the mod's folders after the real NameSpace entries.
LSTATUS WINAPI RegEnumKeyExW_Hook(HKEY hKey, DWORD index, LPWSTR name, LPDWORD nameLen,
                                  LPDWORD reserved, LPWSTR cls, LPDWORD clsLen,
                                  PFILETIME lastWrite) {
    LSTATUS status = RegEnumKeyExW_Original(hKey, index, name, nameLen, reserved, cls,
                                            clsLen, lastWrite);
    if (status != ERROR_NO_MORE_ITEMS || !g_hive || !IsTrackedKey(hKey, true)) {
        return status;
    }
    DWORD real = 0;
    if (RegQueryInfoKeyW_Original(hKey, nullptr, nullptr, nullptr, &real, nullptr, nullptr,
                                  nullptr, nullptr, nullptr, nullptr, nullptr) !=
            ERROR_SUCCESS ||
        index < real) {
        return status;
    }
    std::vector<std::wstring> nodes = ActiveNodes();
    if (index - real >= nodes.size()) return status;
    const std::wstring& clsid = nodes[index - real];
    if (!nameLen) return ERROR_INVALID_PARAMETER;
    if (!name || *nameLen <= clsid.size()) {
        *nameLen = (DWORD)clsid.size() + 1;
        return ERROR_MORE_DATA;
    }
    wcscpy_s(name, *nameLen, clsid.c_str());
    *nameLen = (DWORD)clsid.size();
    if (cls && clsLen && *clsLen) cls[0] = L'\0';
    if (clsLen) *clsLen = 0;
    if (lastWrite) GetSystemTimeAsFileTime(lastWrite);
    return ERROR_SUCCESS;
}

LSTATUS WINAPI RegQueryInfoKeyW_Hook(HKEY hKey, LPWSTR cls, LPDWORD clsLen,
                                     LPDWORD reserved, LPDWORD subKeys,
                                     LPDWORD maxSubKeyLen, LPDWORD maxClassLen,
                                     LPDWORD values, LPDWORD maxValueNameLen,
                                     LPDWORD maxValueLen, LPDWORD securityLen,
                                     PFILETIME lastWrite) {
    LSTATUS status = RegQueryInfoKeyW_Original(hKey, cls, clsLen, reserved, subKeys,
                                               maxSubKeyLen, maxClassLen, values,
                                               maxValueNameLen, maxValueLen, securityLen,
                                               lastWrite);
    if (status != ERROR_SUCCESS || !g_hive) return status;
    if (subKeys && IsTrackedKey(hKey, true)) {
        size_t extra = ActiveNodes().size();
        *subKeys += (DWORD)extra;
        if (extra && maxSubKeyLen && *maxSubKeyLen < 38) *maxSubKeyLen = 38;
    } else if (values && IsTrackedKey(hKey, false)) {
        *values += (DWORD)AllNodes().size();
        if (maxValueNameLen && *maxValueNameLen < 38) *maxValueNameLen = 38;
        if (maxValueLen && *maxValueLen < sizeof(DWORD)) *maxValueLen = sizeof(DWORD);
    }
    return status;
}

// Hides the mod's folders from the desktop (HideDesktopIcons\NewStartPanel).
LSTATUS WINAPI RegQueryValueExW_Hook(HKEY hKey, LPCWSTR valueName, LPDWORD reserved,
                                     LPDWORD type, LPBYTE data, LPDWORD cb) {
    if (g_hive && IsOurNodeName(valueName) && IsHidePath(KeyPath(hKey))) {
        return ServeDword(type, data, cb);
    }
    return RegQueryValueExW_Original(hKey, valueName, reserved, type, data, cb);
}

// Adds "{clsid}"=1 values after the real HideDesktopIcons values.
LSTATUS WINAPI RegEnumValueW_Hook(HKEY hKey, DWORD index, LPWSTR name, LPDWORD nameLen,
                                  LPDWORD reserved, LPDWORD type, LPBYTE data, LPDWORD cb) {
    LSTATUS status =
        RegEnumValueW_Original(hKey, index, name, nameLen, reserved, type, data, cb);
    if (status != ERROR_NO_MORE_ITEMS || !g_hive || !IsTrackedKey(hKey, false)) {
        return status;
    }
    DWORD real = 0;
    if (RegQueryInfoKeyW_Original(hKey, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
                                  &real, nullptr, nullptr, nullptr, nullptr) != ERROR_SUCCESS ||
        index < real) {
        return status;
    }
    std::vector<std::wstring> nodes = AllNodes();
    if (index - real >= nodes.size()) return status;
    const std::wstring& clsid = nodes[index - real];
    if (!nameLen) return ERROR_INVALID_PARAMETER;
    if (!name || *nameLen <= clsid.size()) {
        *nameLen = (DWORD)clsid.size() + 1;
        return ERROR_MORE_DATA;
    }
    wcscpy_s(name, *nameLen, clsid.c_str());
    *nameLen = (DWORD)clsid.size();
    return ServeDword(type, data, cb);
}

LSTATUS WINAPI RegGetValueW_Hook(HKEY hKey, LPCWSTR sub, LPCWSTR value, DWORD flags,
                                 LPDWORD type, PVOID data, LPDWORD cb) {
    HKEY hive = g_hive;
    bool ourValue = IsOurNodeName(value);
    if (hive && ((sub && *sub && MightBeOurs(sub)) || ourValue)) {
        std::wstring full = JoinKeyPath(KeyPath(hKey), sub);
        std::wstring hivePath;
        if (sub && *sub && !full.empty() && MapToHive(full, hivePath)) {
            HKEY key;
            LSTATUS status =
                RegOpenKeyExW_Original(hive, hivePath.c_str(), 0, KEY_QUERY_VALUE, &key);
            if (status != ERROR_SUCCESS) return status;
            status = RegGetValueW_Original(key, nullptr, value, flags, type, data, cb);
            RegCloseKey_Original(key);
            return status;
        }
        if (ourValue && IsHidePath(full)) {
            if (!(flags & RRF_RT_REG_DWORD)) return ERROR_UNSUPPORTED_TYPE;
            return ServeDword(type, (LPBYTE)data, cb);
        }
    }
    return RegGetValueW_Original(hKey, sub, value, flags, type, data, cb);
}

// Loads (or creates) the private hive in the mod's storage.
bool OpenHive() {
    WCHAR storage[MAX_PATH];
    WCHAR user[256];
    DWORD userLen = ARRAYSIZE(user);
    if (!Wh_GetModStoragePath(storage, ARRAYSIZE(storage)) || !GetUserNameW(user, &userLen)) {
        return false;
    }
    SHCreateDirectoryExW(nullptr, storage, nullptr);
    std::wstring file = std::wstring(storage) + L"\\" + user + L"-shell.hiv";
    HKEY hive = nullptr;
    LSTATUS status = RegLoadAppKeyW(file.c_str(), &hive, KEY_ALL_ACCESS, 0, 0);
    if (status != ERROR_SUCCESS) {
        Wh_Log(L"RegLoadAppKey failed: %d", (int)status);
        return false;
    }
    g_hive = hive;
    return true;
}

// Kernel paths of the stand-in keys, so the hooks recognize them.
void ReadStandInPaths() {
    for (int i = 0; i < 2; i++) {
        HKEY h;
        if (RegOpenKeyExW(g_hive, i ? L"HideDesktopIcons" : L"NameSpace", 0, KEY_READ, &h) !=
            ERROR_SUCCESS) {
            continue;
        }
        alignas(8) BYTE buf[2048];
        ULONG len = 0;
        if (g_NtQueryKey && g_NtQueryKey(h, 3, buf, sizeof(buf), &len) >= 0) {
            ULONG nameLen = *(ULONG*)buf;
            if (nameLen <= sizeof(buf) - sizeof(ULONG)) {
                std::wstring k = Lower(std::wstring((wchar_t*)(buf + sizeof(ULONG)), nameLen / 2));
                (i ? g_hiveHideKernel : g_hiveNsKernel) = k;
            }
        }
        RegCloseKey(h);
    }
}

FARPROC KernelBaseFunction(const char* name) {
    HMODULE module = GetModuleHandleW(L"kernelbase.dll");
    FARPROC p = module ? GetProcAddress(module, name) : nullptr;
    if (!p) {
        module = GetModuleHandleW(L"advapi32.dll");
        if (module) p = GetProcAddress(module, name);
    }
    return p;
}

template <typename T>
bool HookKernelBase(const char* name, T hook, T* original) {
    T target = (T)(void*)KernelBaseFunction(name);
    if (!target || !WindhawkUtils::SetFunctionHook(target, hook, original)) {
        Wh_Log(L"Failed to hook %S", name);
        return false;
    }
    return true;
}

BOOL ExplorerInit() {
    LoadSettings();
    g_lang = GetSettings().lang;
    g_NtQueryKey =
        (NtQueryKey_t)(void*)GetProcAddress(GetModuleHandleW(L"ntdll.dll"), "NtQueryKey");
    if (!g_NtQueryKey || !OpenHive()) {
        Wh_Log(L"Explorer integration disabled");
        return TRUE;
    }
    BuildHive(GetSettings());
    UpdateNodeConfig(GetSettings());
    ReadStandInPaths();

    bool ok = HookKernelBase("RegOpenKeyExW", RegOpenKeyExW_Hook, &RegOpenKeyExW_Original) &&
              HookKernelBase("RegCloseKey", RegCloseKey_Hook, &RegCloseKey_Original) &&
              HookKernelBase("RegEnumKeyExW", RegEnumKeyExW_Hook, &RegEnumKeyExW_Original) &&
              HookKernelBase("RegQueryInfoKeyW", RegQueryInfoKeyW_Hook,
                             &RegQueryInfoKeyW_Original) &&
              HookKernelBase("RegQueryValueExW", RegQueryValueExW_Hook,
                             &RegQueryValueExW_Original) &&
              HookKernelBase("RegGetValueW", RegGetValueW_Hook, &RegGetValueW_Original) &&
              HookKernelBase("RegEnumValueW", RegEnumValueW_Hook, &RegEnumValueW_Original) &&
              HookKernelBase("RegCreateKeyExW", RegCreateKeyExW_Hook,
                             &RegCreateKeyExW_Original);
    if (!ok) {
        // The mod won't load, so ExplorerUninit won't run: release the hive.
        HKEY hive = g_hive;
        g_hive = nullptr;
        RegCloseKey(hive);
        return FALSE;
    }
    return TRUE;
}

// True if this explorer.exe shows the shell (taskbar, desktop) or folder
// windows. The short-lived explorer.exe instances started by the menu verbs
// only own hidden helper windows, so they're excluded.
BOOL CALLBACK FindShellWindowProc(HWND hWnd, LPARAM param) {
    DWORD pid = 0;
    GetWindowThreadProcessId(hWnd, &pid);
    if (pid != GetCurrentProcessId()) return TRUE;
    WCHAR cls[64];
    if (!GetClassNameW(hWnd, cls, ARRAYSIZE(cls))) return TRUE;
    bool shell = _wcsicmp(cls, L"Shell_TrayWnd") == 0 || _wcsicmp(cls, L"Progman") == 0 ||
                 (_wcsicmp(cls, L"CabinetWClass") == 0 && IsWindowVisible(hWnd));
    if (!shell) return TRUE;
    *(bool*)param = true;
    return FALSE;
}

bool HasShellWindows() {
    bool found = false;
    EnumWindows(FindShellWindowProc, (LPARAM)&found);
    return found;
}

void ExplorerAfterInit() {
    // At process start nothing is cached yet, so there's nothing to refresh.
    // A refresh is only needed when the mod is enabled in an explorer.exe that
    // already shows the shell or folder windows.
    if (g_hive && HasShellWindows()) {
        NotifyAssocChanged();
        NotifyDesktopChanged();
    }
}

void ExplorerSettingsChanged() {
    LoadSettings();
    g_lang = GetSettings().lang;
    if (g_hive) {
        BuildHive(GetSettings());
        UpdateNodeConfig(GetSettings());
        NotifyAssocChanged();
        NotifyDesktopChanged();
    }
}

HKEY g_hiveToClose = nullptr;

// Called while the hooks are still active: they stop answering from now on.
void ExplorerBeforeUninit() {
    g_hiveToClose = g_hive;
    g_hive = nullptr;
}

void ExplorerUninit() {
    if (g_hive) ExplorerBeforeUninit();
    if (g_hiveToClose) {
        RegCloseKey(g_hiveToClose);
        g_hiveToClose = nullptr;
        // Only the processes showing windows need to drop the entries; the
        // short-lived ones started by the menu verbs must not refresh the
        // whole shell when they exit.
        if (HasShellWindows()) {
            NotifyAssocChanged();
            NotifyDesktopChanged();
        }
    }
}

}  // namespace

BOOL WhTool_ModInit() {
    LoadSettings();
    g_stopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_resyncEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    if (!g_stopEvent || !g_resyncEvent) return FALSE;
    g_thread = CreateThread(nullptr, 0, Worker, nullptr, 0, nullptr);
    return g_thread != nullptr;
}

void WhTool_ModUninit() {
    if (g_thread) {
        SetEvent(g_stopEvent);
        WaitForSingleObject(g_thread, INFINITE);
        CloseHandle(g_thread);
        g_thread = nullptr;
    }
    NotifyDesktopChanged();
    if (g_resyncEvent) CloseHandle(g_resyncEvent);
    if (g_stopEvent) CloseHandle(g_stopEvent);
}

void WhTool_ModSettingsChanged() {
    LoadSettings();
    SetEvent(g_resyncEvent);
}

////////////////////////////////////////////////////////////////////////////////
// Entry points. The mod runs in two kinds of processes:
// * explorer.exe: registry read hooks only (see "explorer hooks"); no file
//   parsing and no background work happen there.
// * a dedicated windhawk.exe process: the scanning and the shortcut folder,
//   started by the standard tool mod launcher below. The launcher code is the
//   snippet from the wiki, with an explorer.exe check added at the start of
//   each callback.

bool g_isExplorer = false;

bool IsExplorerProcess() {
    WCHAR path[MAX_PATH];
    DWORD len = GetModuleFileNameW(nullptr, path, ARRAYSIZE(path));
    return len && len < ARRAYSIZE(path) &&
           _wcsicmp(PathFindFileNameW(path), L"explorer.exe") == 0;
}

////////////////////////////////////////////////////////////////////////////////
// Windhawk tool mod implementation for mods which don't need to inject to other
// processes or hook other functions. Context:
// https://github.com/ramensoftware/windhawk/wiki/Mods-as-tools:-Running-mods-in-a-dedicated-process
//
// The mod will load and run in a dedicated windhawk.exe process.
//
// Paste the code below as part of the mod code, and use these callbacks:
// * WhTool_ModInit
// * WhTool_ModSettingsChanged
// * WhTool_ModUninit
//
// Currently, other callbacks are not supported.

bool g_isToolModProcessLauncher;
HANDLE g_toolModProcessMutex;

void WINAPI EntryPoint_Hook() {
    Wh_Log(L">");
    ExitThread(0);
}

BOOL Wh_ModInit() {
    if (IsExplorerProcess()) {
        g_isExplorer = true;
        return ExplorerInit();
    }

    DWORD sessionId;
    if (ProcessIdToSessionId(GetCurrentProcessId(), &sessionId) &&
        sessionId == 0) {
        return FALSE;
    }

    bool isExcluded = false;
    bool isToolModProcess = false;
    bool isCurrentToolModProcess = false;
    int argc;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLine(), &argc);
    if (!argv) {
        Wh_Log(L"CommandLineToArgvW failed");
        return FALSE;
    }

    for (int i = 1; i < argc; i++) {
        if (wcscmp(argv[i], L"-service") == 0 ||
            wcscmp(argv[i], L"-service-start") == 0 ||
            wcscmp(argv[i], L"-service-stop") == 0) {
            isExcluded = true;
            break;
        }
    }

    for (int i = 1; i < argc - 1; i++) {
        if (wcscmp(argv[i], L"-tool-mod") == 0) {
            isToolModProcess = true;
            if (wcscmp(argv[i + 1], WH_MOD_ID) == 0) {
                isCurrentToolModProcess = true;
            }
            break;
        }
    }

    LocalFree(argv);

    if (isExcluded) {
        return FALSE;
    }

    if (isCurrentToolModProcess) {
        g_toolModProcessMutex =
            CreateMutex(nullptr, TRUE, L"windhawk-tool-mod_" WH_MOD_ID);
        if (!g_toolModProcessMutex) {
            Wh_Log(L"CreateMutex failed");
            ExitProcess(1);
        }

        if (GetLastError() == ERROR_ALREADY_EXISTS) {
            Wh_Log(L"Tool mod already running (%s)", WH_MOD_ID);
            ExitProcess(1);
        }

        if (!WhTool_ModInit()) {
            ExitProcess(1);
        }

        IMAGE_DOS_HEADER* dosHeader =
            (IMAGE_DOS_HEADER*)GetModuleHandle(nullptr);
        IMAGE_NT_HEADERS* ntHeaders =
            (IMAGE_NT_HEADERS*)((BYTE*)dosHeader + dosHeader->e_lfanew);

        DWORD entryPointRVA = ntHeaders->OptionalHeader.AddressOfEntryPoint;
        void* entryPoint = (BYTE*)dosHeader + entryPointRVA;

        Wh_SetFunctionHook(entryPoint, (void*)EntryPoint_Hook, nullptr);
        return TRUE;
    }

    if (isToolModProcess) {
        return FALSE;
    }

    g_isToolModProcessLauncher = true;
    return TRUE;
}

void Wh_ModAfterInit() {
    if (g_isExplorer) {
        ExplorerAfterInit();
        return;
    }

    if (!g_isToolModProcessLauncher) {
        return;
    }

    WCHAR currentProcessPath[MAX_PATH];
    switch (GetModuleFileName(nullptr, currentProcessPath,
                              ARRAYSIZE(currentProcessPath))) {
        case 0:
        case ARRAYSIZE(currentProcessPath):
            Wh_Log(L"GetModuleFileName failed");
            return;
    }

    WCHAR
    commandLine[MAX_PATH + 2 +
                (sizeof(L" -tool-mod \"" WH_MOD_ID "\"") / sizeof(WCHAR)) - 1];
    swprintf_s(commandLine, L"\"%s\" -tool-mod \"%s\"", currentProcessPath,
               WH_MOD_ID);

    HMODULE kernelModule = GetModuleHandle(L"kernelbase.dll");
    if (!kernelModule) {
        kernelModule = GetModuleHandle(L"kernel32.dll");
        if (!kernelModule) {
            Wh_Log(L"No kernelbase.dll/kernel32.dll");
            return;
        }
    }

    using CreateProcessInternalW_t = BOOL(WINAPI*)(
        HANDLE hUserToken, LPCWSTR lpApplicationName, LPWSTR lpCommandLine,
        LPSECURITY_ATTRIBUTES lpProcessAttributes,
        LPSECURITY_ATTRIBUTES lpThreadAttributes, WINBOOL bInheritHandles,
        DWORD dwCreationFlags, LPVOID lpEnvironment, LPCWSTR lpCurrentDirectory,
        LPSTARTUPINFOW lpStartupInfo,
        LPPROCESS_INFORMATION lpProcessInformation,
        PHANDLE hRestrictedUserToken);
    CreateProcessInternalW_t pCreateProcessInternalW =
        (CreateProcessInternalW_t)GetProcAddress(kernelModule,
                                                 "CreateProcessInternalW");
    if (!pCreateProcessInternalW) {
        Wh_Log(L"No CreateProcessInternalW");
        return;
    }

    STARTUPINFO si{
        .cb = sizeof(STARTUPINFO),
        .dwFlags = STARTF_FORCEOFFFEEDBACK,
    };
    PROCESS_INFORMATION pi;
    if (!pCreateProcessInternalW(nullptr, currentProcessPath, commandLine,
                                 nullptr, nullptr, FALSE, NORMAL_PRIORITY_CLASS,
                                 nullptr, nullptr, &si, &pi, nullptr)) {
        Wh_Log(L"CreateProcess failed");
        return;
    }

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}

void Wh_ModSettingsChanged() {
    if (g_isExplorer) {
        ExplorerSettingsChanged();
        return;
    }

    if (g_isToolModProcessLauncher) {
        return;
    }

    WhTool_ModSettingsChanged();
}

void Wh_ModUninit() {
    if (g_isExplorer) {
        ExplorerUninit();
        return;
    }

    if (g_isToolModProcessLauncher) {
        return;
    }

    WhTool_ModUninit();
    ExitProcess(0);
}

void Wh_ModBeforeUninit() {
    if (g_isExplorer) ExplorerBeforeUninit();
}
