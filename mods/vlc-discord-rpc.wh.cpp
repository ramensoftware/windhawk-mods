// ==WindhawkMod==
// @id              vlc-discord-rpc
// @name            VLC Discord Rich Presence
// @description     Shows your currently playing media on Discord — with cover art, quality tags, and a search button.
// @version         1.2.0
// @author          ciizerr
// @github          https://github.com/ciizerr
// @homepage        https://vlc-rpc.vercel.app/
// @include         vlc.exe
// @compilerOptions -lwinhttp -lshell32 -lgdiplus -lole32
// @architecture    x86
// @architecture    x86-64
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# VLC Discord Rich Presence

Shows what you're watching or listening to on your Discord profile — including cover art, quality tags, ⭐ ratings, genres, runtime, and a search button.

## Features

| Feature | Description |
|---|---|
| 🖼️ **Cover Art** | Uploads local album art automatically. Fallbacks to searching official databases and the web for a matching poster. |
| 🧹 **Smart Title Cleaning** | Strips scene release tags (e.g., IMAX, UHD, x265, HDR), piracy site URLs, and release info from filenames so titles look clean. |
| 🍿 **Multi-Engine Scraper** | Automatically fetches official titles, episode names, ⭐ ratings, genres, and exact runtimes from **TMDB** and **TVMaze**. Prioritizes TVMaze for TV series to save TMDB rate limits, with persistent 30-day disk caching. |
| 🎮 **Activity Type** | Automatically shows "Listening to", "Watching", or "Playing" based on the file type. |
| 🎵 **Rich Metadata** | Music shows artist/album. Video shows season, episode, chapter, audio language, ratings, and genres. |
| 📺 **Quality Tags** | Displays resolution and format badges like 4K, HDR, or 1080p next to the title. |
| 🔍 **Search Button** | Adds an interactive button linking to Google, IMDb, YouTube, or a custom URL. |
| 🎨 **Themes** | Choose between a classic Default or sleek Dark icon set. |
| 📉 **Minimal Mode** | Hides the small play/pause badge so your cover art takes center stage. |

## New Look in v1.2.0

| Movies | TV Series |
| :---: | :---: |
| ![movies](https://raw.githubusercontent.com/ciizerr/vlc-discord-rpc-archive/main/screenshots/themes/movies.png) | ![series](https://raw.githubusercontent.com/ciizerr/vlc-discord-rpc-archive/main/screenshots/themes/series.png) |

## Icon Themes

| Default (Orange VLC Cone) | Dark (Low-light setups) |
| :---: | :---: |
| ![default theme](https://raw.githubusercontent.com/ciizerr/vlc-discord-rpc-archive/main/screenshots/themes/default.gif) | ![dark theme](https://raw.githubusercontent.com/ciizerr/vlc-discord-rpc-archive/main/screenshots/themes/dark_.gif) |


Community icon submissions are welcome — reach out to `ciizerr` on Discord with a set of four icons (vlc, play, pause, stop).

## First-Time Setup

This mod reads playback data from VLC's built-in HTTP interface. You only need to do this once.

1. Open VLC and go to **Tools → Preferences** (`Ctrl+P`).
2. In the bottom-left, switch *Show settings* to **All**.
3. Go to **Interface → Main interfaces** and check **Web**.

   ![Enable Web Interface](https://raw.githubusercontent.com/ciizerr/vlc-discord-rpc-archive/main/screenshots/setup/web-interface.png)

4. Expand *Main interfaces* in the sidebar and click **Lua**.
5. Under *Lua HTTP*, set the **Password** to `1234`. Leave the port to default.

   ![Lua Password Setup](https://raw.githubusercontent.com/ciizerr/vlc-discord-rpc-archive/main/screenshots/setup/password.png)

6. Click **Save** and restart VLC.

## Settings Overview

| Setting | What it does |
|---|---|
| **Cover Art** | Fetches and shows artwork on your status |
| **Quality Tags** | Shows 4K / HDR / 1080p badges |
| **Minimal Mode** | Hides the small play/pause corner badge |
| **Clean Media Titles** | Removes scene tags, IMAX/UHD tags, and URLs from filenames |
| **TMDB Scraper** | Primary metadata engine for movies & TV series. Includes option for custom API key |
| **TVMaze Scraper** | Secondary fallback engine prioritized for TV shows |
| **⭐ Rating / Genres / Runtime / Type** | Toggles controlling shared display details across TMDB and TVMaze |
| **Title Filters** | Choose between live community filters or local-only |
| **Custom Filter Words** | Add your own words to strip from titles |
| **Notifications** | Shows a Windows toast when a new track starts |
| **Search Button** | Picks which site the status button links to |
| **"Listening to..." label** | Controls what shows in the activity name for music |

## Filter List

To keep title cleaning accurate, the mod can download a small community‑maintained filter list from GitHub.  
This list is refreshed every 6 hours.  

If you enable **Local Filters Only** in settings, the mod will stop downloading the online filter list and instead use:  
- The built‑in word list  
- Any custom filters you’ve added yourself  

## Troubleshooting

| Problem / Log Message | Solution |
|---|---|
| **Before trying fixes** | Check the Windhawk mod logs (`...`) first to identify what the worker is doing. |
| **Log: `Port X responded with status code 404`** | Another application on your computer is occupying port `8080` (or `X`) instead of VLC, or VLC's `status.json` file is missing. The mod will automatically check candidate fallback ports (`9080`, `9090`, etc.). |
| **Log: `Auth failed (401) on port X`** | Wrong VLC password. Ensure your Lua HTTP password in VLC preferences or `%APPDATA%\vlc\vlcrc` (`http-password=`) is set exactly to `1234`. Restart VLC afterward. |
| **Log: `Could not reach VLC on port X (WinHttp error ...)`** | VLC Web Interface is either disabled, closed, or listening on a different port. The mod will automatically test candidate fallback ports (`8080`, `9080`, `9090`, `7080`, `4212`, `8081`, `8088`). To find VLC's exact port manually: open **Resource Monitor** → **Network** → **Listening Ports**, find `vlc.exe`, and enter that port into **Custom Port** (`CustomPort`) in the mod settings. |
| **Log: `Checked all candidate ports. Still searching...`** | The mod tested all candidate ports and could not find VLC's Web Interface. Make sure VLC is running and the **Web** (`Lua HTTP`) checkbox is checked under **Main interfaces**. If VLC is using a non-standard port, use **Resource Monitor** → **Network** → **Listening Ports** to find `vlc.exe`'s port and enter it in **Custom Port** in the mod settings. |
| **Connection still fails (rare)** | Open `%APPDATA%\vlc\vlcrc`, find `#http-port=8080` (or whichever port you want to use), change it to `http-port=8080` (removing the `#`), save, and restart VLC. |
| **Connected to VLC, but not showing on Discord** | Enable **Display current activity** (`Activity Privacy`) in Discord settings. Make sure Discord is open and running properly. |
| **Wrong poster/artwork** | Enable **Clean Media Titles** for better filename cleanup, and enable **TMDB & TVMaze Scrapers** for official titles and high-resolution posters. |

## External Requests

The mod makes external requests for certain features:

- **`api.themoviedb.org`** — used to fetch official movie/TV titles, release years, ⭐ ratings, genres, exact runtimes, and high-res poster URLs (`FetchTmdbMetadata`)  
- **`api.tvmaze.com`** — used to fetch rich TV show titles, episode names, ratings, and poster URLs (`FetchTvMazeMetadata`)  
- **`www.bing.com`** — used to search for fallback poster and artwork URLs (`FindExternalArtwork`)  
- **`uguu.se`** — used to upload local album art (`UploadToUguu`)  
- **`raw.githubusercontent.com`** — used to download `vlc-discord-icon.ico` and `filters.txt` for notifications and media title cleaning  

These requests are only used for artwork, metadata scraping, uploads, notifications, and media title cleaning.

## Support & Feedback

Found a bug, have a suggestion, or need help?

- **Discord:** `ciizerr` ([Windhawk Server](https://discord.com/servers/windhawk-923944342991818753))
- **GitHub:** [vlc-discord-rpc-archive](https://github.com/ciizerr/vlc-discord-rpc-archive)
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- DisplayGroup:
  - ShowCoverArt: true
    $name: Cover Art
    $description: "Fetches and displays artwork on your Discord status. Uses local album art if available, otherwise searches official databases and the web for a matching poster."
  - ShowQualityTags: true
    $name: Quality Tags
    $description: "Shows resolution and format badges like 4K, HDR, or 1080p next to the title."
  - Theme: ""
    $name: Icon Theme
    $description: "Visual style of the main icon on your Discord status."
    $options:
      - "": Default
      - "dark_": Dark Mode
  - MinimalMode: false
    $name: Minimal Mode
    $description: "Hides the small play/pause/stop badge in the corner of your status image."
  - ShowNotifications: false
    $name: Toast Notifications
    $description: "Shows a Windows notification when a new file starts playing."
  - ShowChapter: false
    $name: Show Chapter Number
    $description: "Displays the current chapter alongside the episode or track info."
  - ShowAudioLanguage: false
    $name: Show Audio Language
    $description: "Shows the active audio track language (e.g. EN, JP) in your status."
  - MusicActivityName: "Title"
    $name: "'Listening to...' Label"
    $description: "What to show in the Discord activity name when playing music."
    $options:
      - Title: Song Title
      - Artist: Artist Name
      - Album: Album Name
  $name: "Appearance & Display"

- ScraperGroup:
  - EnableTmdbScraper: true
    $name: "Enable TMDB Scraper (Primary - Movies & TV)"
    $description: "Primary metadata engine. Automatically fetches official titles, release years, posters, ⭐ ratings, genres, and runtimes from The Movie Database (TMDB)."
  - CustomTmdbApiKey: ""
    $name: "Custom TMDB API Key (Optional)"
    $description: "Enter your own TMDB API key if you have one. If left blank, the built-in community key is used automatically."
  - EnableTvMazeScraper: true
    $name: "Enable TVMaze Scraper (Fallback - TV Only)"
    $description: "Secondary fallback for TV series. If watching TV, prioritizes TVMaze first to conserve TMDB rate limits."
  - ShowRating: true
    $name: "Show ⭐ Rating"
    $description: "Displays average rating (e.g. ⭐ 8.4/10) from TMDB or TVMaze next to the details."
  - ShowGenres: true
    $name: "Show Genres"
    $description: "Displays official genres (e.g. Action, Sci-Fi) in your Discord status."
  - ShowRuntime: false
    $name: "Show Runtime/Length"
    $description: "Displays official duration or runtime when available."
  - ShowMediaType: true
    $name: "Show Media Type inside Hover Text"
    $description: "Shows official classification label inside large image hover text (e.g. 'Watching Movie' or 'Watching TV Show')."
  $name: "Media Scrapers & Metadata Display"

- FilterGroup:
  - EnableMetadataCleaner: true
    $name: Clean Media Titles
    $description: "Strips scene release tags (e.g. WEB-DL, x265, IMAX), piracy site URLs, and other clutter from filenames before showing them on Discord."
  - StrictLocalMode: true
    $name: Local Filters Only
    $description: "Keeps title cleaning offline. When off, the mod downloads an updated community filter list from GitHub every 6 hours. When on, it only uses the built-in filters and your custom words below."
  - CustomJunkWords: ""
    $name: Custom Words to Remove
    $description: "Extra words or site names to strip from titles, separated by commas. Example: toonworld4all.com, mytag. Requires 'Clean Media Titles' to be enabled."
  $name: "Smart Title Cleaning & Filters"

- ButtonGroup:
  - Provider: Google
    $name: Search Button Provider
    $description: "The site your Discord status button links to when clicked."
    $options:
      - Google: Google
      - Bing: Bing
      - IMDb: IMDb
      - YouTube: YouTube
      - Custom: Custom URL
  - ButtonLabel: "Search This"
    $name: Button Label
    $description: "The text shown on the Discord status button. Maximum 30 characters."
  - CustomUrl: ""
    $name: Custom Search URL
    $description: "Only used when Search Button is set to Custom. Paste the base search URL here. Example: https://myanimelist.net/search/all?q="
  $name: "Search Button Customization"

- AdvancedGroup:
  - CustomPort: 0
    $name: Custom VLC Port
    $description: "Only needed if VLC is running on a non-standard HTTP port. Leave at 0 to detect automatically."
  - ClientId: "1465711556418474148"
    $name: Discord Application ID
    $description: "The Client ID from the Discord Developer Portal. Only change this if you've set up your own Discord application with custom assets."
  $name: "Advanced & Connectivity"
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <shellapi.h>
#include <objbase.h>
#include <gdiplus.h>
#include <winhttp.h>
#include <string>
#include <thread>
#include <vector>
#include <cstdio>
#include <atomic>
#include <map>
#include <fstream>
#include <mutex>
#include <algorithm> 
#include <cctype>
#include <sstream>
#include <shlobj.h>
#include <sys/stat.h>
#include <optional>
#include <chrono>
#include <ctime>

// =============================================================
// ⚙️ GLOBALS
// =============================================================
ULONG_PTR g_gdiplusToken = 0;
std::atomic<bool> g_stopThread{false};
[[clang::no_destroy]] std::optional<std::thread> g_workerThread;
std::mutex g_asyncMutex;

struct AsyncTask {
    std::thread thread;
    std::shared_ptr<std::atomic<bool>> done;
};
[[clang::no_destroy]] std::optional<std::vector<AsyncTask>> g_asyncTasks{std::in_place};

template <typename F>
void RunAsync(F&& f) {
    auto done = std::make_shared<std::atomic<bool>>(false);
    std::thread t([f = std::forward<F>(f), done]() mutable {
        f();
        done->store(true);
    });
    std::lock_guard<std::mutex> lock(g_asyncMutex);
    for (auto it = g_asyncTasks->begin(); it != g_asyncTasks->end();) {
        if (it->done->load()) {
            if (it->thread.joinable()) it->thread.join();
            it = g_asyncTasks->erase(it);
        } else {
            ++it;
        }
    }
    g_asyncTasks->push_back({std::move(t), std::move(done)});
}
const std::string SEP = " \xE2\x97\x8F ";

std::map<std::string, std::string> g_imageCache;
std::map<std::string, bool> g_fetchInProgress;
std::mutex g_cacheMutex;
std::mutex g_diskMutex;
std::mutex g_sessionMutex;
HINTERNET g_hWinHttpSession = NULL;

HINTERNET GetSharedWinHttpSession() {
    std::lock_guard<std::mutex> lock(g_sessionMutex);
    if (!g_hWinHttpSession) {
        g_hWinHttpSession = WinHttpOpen(L"VLC-RPC-Mod/2.0", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
        if (g_hWinHttpSession) {
            WinHttpSetTimeouts(g_hWinHttpSession, 3000, 3000, 3000, 3000);
        }
    }
    return g_hWinHttpSession;
}

struct TvShowMetadata {
    bool valid = false;
    std::string showName;
    std::string episodeTitle;
    std::string posterUrl;
    std::string rating;
    std::string genres;
    std::string runtime;
    std::string showType;
};

std::map<std::string, TvShowMetadata> g_tvMetadataCache;
std::map<std::string, bool> g_tvFetchInProgress;

// =============================================================
// 🔥 DYNAMIC JUNK FILTER GLOBALS 🔥
// =============================================================
std::vector<std::string> g_junkSites;
std::vector<std::string> g_tlds;
std::vector<std::string> g_truncateTags;
std::vector<std::string> g_junkWords;
std::mutex g_filterMutex;
bool g_filtersLoaded = false;

// =============================================================
// 1. STRING & METADATA HELPERS
// =============================================================

std::string Base64Encode(const std::string& in) {
    std::string out;
    int val = 0, valb = -6;
    for (unsigned char c : in) {
        val = (val << 8) + c;
        valb += 8;
        while (valb >= 0) {
            out.push_back("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"[(val >> valb) & 0x3F]);
            valb -= 6;
        }
    }
    if (valb > -6) out.push_back("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"[((val << 8) >> (valb + 8)) & 0x3F]);
    while (out.size() % 4) out.push_back('=');
    return out;
}

std::string Base64Decode(const std::string& in) {
    std::string out;
    std::vector<int> T(256, -1);
    const std::string b64 = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    for (int i = 0; i < 64; i++) T[b64[i]] = i;

    int val = 0, valb = -8;
    for (unsigned char c : in) {
        if (T[c] == -1) break;
        val = (val << 6) + T[c];
        valb += 6;
        if (valb >= 0) {
            out.push_back(char((val >> valb) & 0xFF));
            valb -= 8;
        }
    }
    return out;
}

std::string GetActiveTmdbApiKey(const std::string& customKey) {
    if (!customKey.empty()) return customKey;
    // Default community API key (obfuscated)
    return Base64Decode("ZGI0MDU0MjQwOGJlODk3ZDcwMDI4YmY2OGZlNjBmYTY=");
}

std::string WStrToStr(const std::wstring& wstr) {
    if (wstr.empty()) return "";
    int size = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
    std::string str(size, 0);
    WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &str[0], size, NULL, NULL);
    return str;
}

std::wstring StrToWStr(const std::string& str) {
    if (str.empty()) return L"";
    int size = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
    std::wstring wstr(size, 0);
    MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstr[0], size);
    return wstr;
}

std::string GetStringSettingSafe(PCWSTR name) {
    PCWSTR val = Wh_GetStringSetting(name);
    if (!val) return "";
    std::wstring wval(val);
    Wh_FreeStringSetting(val);
    return WStrToStr(wval);
}

void ReadVlcConfig(int& port, std::wstring& authBase64) {
    port = 0;
    std::string password = "";
    
    char appDataPath[MAX_PATH];
    if (SUCCEEDED(SHGetFolderPathA(NULL, CSIDL_APPDATA, NULL, 0, appDataPath))) {
        std::string vlcrcPath = std::string(appDataPath) + "\\vlc\\vlcrc";
        std::ifstream file(vlcrcPath);
        if (file.is_open()) {
            std::string line;
            while (std::getline(file, line)) {
                if (!line.empty() && line.back() == '\r') line.pop_back();
                
                if (line.find("http-port=") == 0) {
                    try { port = std::stoi(line.substr(10)); } catch(...) {}
                }
                else if (line.find("http-password=") == 0) {
                    password = line.substr(14);
                }
            }
        } else {
            Wh_Log(L"Could not open vlcrc at %S — using defaults.", vlcrcPath.c_str());
        }
    } else {
        Wh_Log(L"Could not resolve %%APPDATA%% path.");
    }
    
    if (password.empty()) {
        password = "1234";
    }
    
    std::string authStr = ":" + password;
    std::string b64 = Base64Encode(authStr);
    authBase64 = StrToWStr(b64);
}

std::string UrlEncode(const std::string &value) {
    static const char hex[] = "0123456789ABCDEF";
    std::string result;
    for (char c : value) {
        if (isalnum((unsigned char)c) || c == '-' || c == '_' || c == '.' || c == '~') {
            result += c;
        } else {
            result += '%';
            result += hex[(c >> 4) & 0xF];
            result += hex[c & 0xF];
        }
    }
    return result;
}

std::string UrlDecode(const std::string &value) {
    std::string result;
    result.reserve(value.length());
    for (size_t i = 0; i < value.length(); ++i) {
        if (value[i] == '%') {
            if (i + 2 < value.length()) {
                int hex1 = value[i + 1];
                int hex2 = value[i + 2];
                hex1 = (hex1 >= '0' && hex1 <= '9') ? (hex1 - '0') : ((hex1 & 0xDF) - 'A' + 10);
                hex2 = (hex2 >= '0' && hex2 <= '9') ? (hex2 - '0') : ((hex2 & 0xDF) - 'A' + 10);
                result += static_cast<char>((hex1 << 4) | hex2);
                i += 2;
            }
        } else if (value[i] == '+') {
            result += ' ';
        } else {
            result += value[i];
        }
    }
    return result;
}

std::string SanitizeString(const std::string& s) {
    std::string out;
    for (char c : s) {
        if (c == '"') out += '\'';
        else if (c == '\\') {} 
        else if ((unsigned char)c < 32) {} 
        else out += c;
    }
    return out;
}

std::string NumToStr(long long num) { return std::to_string(num); }

std::string ExtractString(const std::string& json, const std::string& key) {
    std::string searchKey = "\"" + key + "\"";
    size_t pos = 0;
    while ((pos = json.find(searchKey, pos)) != std::string::npos) {
        size_t afterKey = pos + searchKey.length();
        while (afterKey < json.length() && isspace((unsigned char)json[afterKey])) afterKey++;
        if (afterKey < json.length() && json[afterKey] == ':') {
            afterKey++;
            while (afterKey < json.length() && isspace((unsigned char)json[afterKey])) afterKey++;
            if (afterKey < json.length() && json[afterKey] == '"') {
                size_t start = afterKey + 1;
                std::string result;
                for (size_t i = start; i < json.length(); ++i) {
                    if (json[i] == '\\' && i + 1 < json.length()) {
                        if (json[i + 1] == '/') { result += '/'; i++; }
                        else if (json[i + 1] == '"') { result += '"'; i++; }
                        else if (json[i + 1] == '\\') { result += '\\'; i++; }
                        else { result += json[i]; }
                    } else if (json[i] == '"') {
                        return result;
                    } else {
                        result += json[i];
                    }
                }
                return result;
            }
        }
        pos += searchKey.length();
    }
    return "";
}

long long ExtractNumber(const std::string& json, const std::string& key) {
    std::string searchKey = "\"" + key + "\"";
    size_t pos = 0;
    while ((pos = json.find(searchKey, pos)) != std::string::npos) {
        size_t afterKey = pos + searchKey.length();
        while (afterKey < json.length() && isspace((unsigned char)json[afterKey])) afterKey++;
        if (afterKey < json.length() && json[afterKey] == ':') {
            afterKey++;
            while (afterKey < json.length() && isspace((unsigned char)json[afterKey])) afterKey++;
            if (afterKey < json.length() && (isdigit((unsigned char)json[afterKey]) || json[afterKey] == '-' || json[afterKey] == '"')) {
                if (json[afterKey] == '"') afterKey++;
                size_t end = afterKey;
                while (end < json.length() && (isdigit((unsigned char)json[end]) || json[end] == '-' || json[end] == '.')) end++;
                try { return (long long)std::stod(json.substr(afterKey, end - afterKey)); } catch(...) {}
            }
        }
        pos += searchKey.length();
    }
    return -1;
}

std::string ExtractDoubleStr(const std::string& json, const std::string& key) {
    std::string searchKey = "\"" + key + "\"";
    size_t pos = 0;
    while ((pos = json.find(searchKey, pos)) != std::string::npos) {
        size_t afterKey = pos + searchKey.length();
        while (afterKey < json.length() && isspace((unsigned char)json[afterKey])) afterKey++;
        if (afterKey < json.length() && json[afterKey] == ':') {
            afterKey++;
            while (afterKey < json.length() && isspace((unsigned char)json[afterKey])) afterKey++;
            if (afterKey < json.length() && (isdigit((unsigned char)json[afterKey]) || json[afterKey] == '-' || json[afterKey] == '"')) {
                if (json[afterKey] == '"') afterKey++;
                size_t end = afterKey;
                while (end < json.length() && (isdigit((unsigned char)json[end]) || json[end] == '-' || json[end] == '.')) end++;
                std::string valStr = json.substr(afterKey, end - afterKey);
                try {
                    double d = std::stod(valStr);
                    if (d > 0.0) {
                        char buf[32];
                        snprintf(buf, sizeof(buf), "%.1f", d);
                        std::string res = buf;
                        if (res.length() > 2 && res.substr(res.length() - 2) == ".0") {
                            res = res.substr(0, res.length() - 2);
                        }
                        return res;
                    }
                } catch(...) {}
            }
        }
        pos += searchKey.length();
    }
    return "";
}

std::string ExtractGenresStr(const std::string& json) {
    size_t pos = json.find("\"genres\"");
    if (pos == std::string::npos) return "";
    pos = json.find('[', pos);
    if (pos == std::string::npos) return "";
    size_t end = json.find(']', pos);
    if (end == std::string::npos) return "";
    
    std::string arr = json.substr(pos + 1, end - pos - 1);
    std::vector<std::string> genres;

    if (arr.find("\"name\"") != std::string::npos) {
        size_t npos = 0;
        while ((npos = arr.find("\"name\"", npos)) != std::string::npos) {
            npos = arr.find(':', npos);
            if (npos == std::string::npos) break;
            npos = arr.find('"', npos);
            if (npos == std::string::npos) break;
            size_t endq = arr.find('"', npos + 1);
            if (endq == std::string::npos) break;
            std::string g = arr.substr(npos + 1, endq - npos - 1);
            if (!g.empty()) genres.push_back(g);
            npos = endq + 1;
            if (genres.size() >= 3) break;
        }
    } else {
        size_t q1 = 0;
        while ((q1 = arr.find('"', q1)) != std::string::npos) {
            size_t q2 = arr.find('"', q1 + 1);
            if (q2 == std::string::npos) break;
            std::string g = arr.substr(q1 + 1, q2 - q1 - 1);
            if (!g.empty()) genres.push_back(g);
            q1 = q2 + 1;
            if (genres.size() >= 3) break;
        }
    }
    
    std::string out;
    for (size_t i = 0; i < genres.size(); ++i) {
        out += genres[i];
        if (i + 1 < genres.size()) out += ", ";
    }
    return out;
}

std::string CleanString(const std::string& str) {
    std::string out;
    for (size_t i = 0; i < str.length(); ++i) {
        if (str[i] == '%' && i + 2 < str.length()) {
            if (str.substr(i, 3) == "%20") { out += ' '; i += 2; continue; }
            if (str.substr(i, 3) == "%5B") { out += '['; i += 2; continue; }
            if (str.substr(i, 3) == "%5D") { out += ']'; i += 2; continue; }
        }
        out += str[i];
    }
    return out;
}

std::string CleanShowTitle(const std::string& input);

std::string CleanMetadata(std::string text, const std::vector<std::string>& customJunk) {
    if (text.empty()) return "";

    std::string noBrackets = "";
    bool inBracket = false;
    for (char c : text) {
        if (c == '[') inBracket = true;
        else if (c == ']') { inBracket = false; continue; }
        
        if (!inBracket) noBrackets += c;
    }

    size_t dot = noBrackets.find_last_of(".");
    if (dot != std::string::npos) {
        std::string ext = noBrackets.substr(dot);
        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
        if (ext == ".mp3" || ext == ".mkv" || ext == ".mp4" || ext == ".avi" || ext == ".flac" || ext == ".m4a" || ext == ".wav") {
            noBrackets = noBrackets.substr(0, dot);
        }
    }

    std::string lowerText = noBrackets;
    std::transform(lowerText.begin(), lowerText.end(), lowerText.begin(), ::tolower);

    std::vector<std::string> activeSites;
    std::vector<std::string> activeTlds;
    std::vector<std::string> activeTags;
    std::vector<std::string> activeWords;
    
    {
        std::lock_guard<std::mutex> lock(g_filterMutex);
        if (g_filtersLoaded) {
            activeSites = g_junkSites;
            activeTlds = g_tlds;
            activeTags = g_truncateTags;
            activeWords = g_junkWords;
        } else {
            activeSites = { 
                "olamovies", "vegamovies", "moviesmod", "katmoviehd", "mkvcinemas", 
                "filmyzilla", "filmywap", "1tamilmv", "jiorockers", "ibomma", "yts", 
                "yify", "psa", "qxr", "tigole", "rarbg", "pahe", "pagalworld", "mrjatt", 
                "djpunjab", "wapking", "songspk", "djmaza", "pendujatt", "naasongs", 
                "masstamilan", "jiosaavn", "moviesverse"
            };
            activeTlds = { 
                ".top", ".com", ".net", ".org", ".in", ".nl", ".is", ".to", ".pw", 
                ".cc", ".site", ".info", ".biz", ".co", ".nz", ".uk", ".mx", ".ws", ".pro" 
            };
            activeTags = {
                "2160p", "1080p", "720p", "480p", "4k", "uhd", "imax", "bluray", "web-dl", "webrip", "hdrip", "camrip", "brrip", "hdr", "10bit", "x264", "x265", "hevc", "remastered", "extended", "unrated"
            };
            activeWords = {
                "downloaded from", "download from", "shared by", "brought to you by", "visit website",
                "downloaded", "download", "320kbps", "128kbps", "kbps", "official video", "lyric video", 
                "ringtone", "full song", "pagalworld", "mrjatt", 
                "djpunjab", "wapking", "songspk", "djmaza", "pendujatt", "naasongs", "masstamilan", 
                "jiosaavn", "olamovies", "uhdmovies", "vegamovies", "moviesmod", "katmoviehd", "mkvcinemas", 
                "filmyzilla", "filmywap", "1tamilmv", "jiorockers", "ibomma", "yts", "yify", "psa", "qxr", 
                "tigole", "rarbg", "pahe", "x264", "x265", "hevc", "10bit"
            };
        }
    }
    
    activeSites.insert(activeSites.end(), customJunk.begin(), customJunk.end());
    
    for (const auto& site : activeSites) {
        if (site.empty()) continue;
        for (const auto& tld : activeTlds) {
            std::string url = site + tld;
            size_t pos;
            while ((pos = lowerText.find(url)) != std::string::npos) {
                noBrackets.replace(pos, url.length(), std::string(url.length(), ' '));
                lowerText.replace(pos, url.length(), std::string(url.length(), ' '));
            }
        }
    }

    for (char &c : noBrackets) {
        if (c == '.' || c == '_' || c == '~') c = ' ';
    }

    lowerText = noBrackets;
    std::transform(lowerText.begin(), lowerText.end(), lowerText.begin(), ::tolower);

    for (const auto& tag : activeTags) {
        size_t pos = lowerText.find(tag);
        if (pos != std::string::npos) {
            noBrackets = noBrackets.substr(0, pos);
            lowerText = lowerText.substr(0, pos); 
        }
    }

    activeWords.insert(activeWords.end(), customJunk.begin(), customJunk.end());

    std::string result = noBrackets;
    for (const auto& word : activeWords) {
        if (word.empty()) continue;
        size_t pos = 0;
        while (true) {
            lowerText = result;
            std::transform(lowerText.begin(), lowerText.end(), lowerText.begin(), ::tolower);
            pos = lowerText.find(word, pos);
            if (pos == std::string::npos) break;
            
            bool isWordBoundary = true;
            if (pos > 0 && isalnum((unsigned char)lowerText[pos - 1])) isWordBoundary = false;
            if (pos + word.length() < lowerText.length() && isalnum((unsigned char)lowerText[pos + word.length()])) isWordBoundary = false;
            
            if (isWordBoundary) {
                result.replace(pos, word.length(), std::string(word.length(), ' '));
                pos += word.length();
            } else {
                pos += 1;
            }
        }
    }
    
    std::string finalClean;
    for (char c : result) {
        if (c == ' ' && !finalClean.empty() && finalClean.back() == ' ') continue; 
        finalClean += c;
    }
    
    size_t start = finalClean.find_first_not_of(" -");
    if (start == std::string::npos) return "";
    size_t end = finalClean.find_last_not_of(" -");
    std::string cleaned = finalClean.substr(start, end - start + 1);
    return CleanShowTitle(cleaned);
}

std::string ExtractYear(const std::string& filename) {
    for (size_t i = 0; i + 3 < filename.length(); i++) {
        if ((filename[i] == '1' && filename[i+1] == '9') || 
            (filename[i] == '2' && filename[i+1] == '0')) {
            if (isdigit(filename[i+2]) && isdigit(filename[i+3])) {
                bool validStart = (i == 0 || !isdigit(filename[i-1]));
                bool validEnd = (i + 4 == filename.length() || !isdigit(filename[i+4]));
                if (validStart && validEnd) {
                    return filename.substr(i, 4);
                }
            }
        }
    }
    return "";
}

// =============================================================
// 2. ASYNC NETWORK & IMAGE API LOGIC
// =============================================================

std::string FetchHttps(const std::wstring& host, const std::wstring& path) {
    if (g_stopThread.load()) return "";
    std::string result = "";
    HINTERNET hSession = GetSharedWinHttpSession();
    if (!hSession) return "";

    HINTERNET hConnect = WinHttpConnect(hSession, host.c_str(), INTERNET_DEFAULT_HTTPS_PORT, 0);
    if (hConnect) {
        HINTERNET hRequest = WinHttpOpenRequest(hConnect, L"GET", path.c_str(), NULL, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, WINHTTP_FLAG_SECURE);
        if (hRequest) {
            std::wstring headers = L"User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/120.0.0.0 Safari/537.36\r\n";
            if (g_stopThread.load()) { WinHttpCloseHandle(hRequest); WinHttpCloseHandle(hConnect); return ""; }
            if (WinHttpSendRequest(hRequest, headers.c_str(), headers.length(), WINHTTP_NO_REQUEST_DATA, 0, 0, 0) &&
                WinHttpReceiveResponse(hRequest, NULL)) {
                
                DWORD dwSize = 0;
                DWORD dwDownloaded = 0;
                do {
                    if (!WinHttpQueryDataAvailable(hRequest, &dwSize)) break;
                    if (dwSize == 0) break;
                    std::vector<char> buffer(dwSize + 1);
                    if (WinHttpReadData(hRequest, buffer.data(), dwSize, &dwDownloaded)) {
                        result.append(buffer.data(), dwDownloaded);
                    }
                } while (dwSize > 0);
            } else {
                DWORD err = GetLastError();
                std::wstring cleanPath = path;
                size_t apiKeyPos = cleanPath.find(L"api_key=");
                if (apiKeyPos != std::wstring::npos) {
                    size_t ampPos = cleanPath.find(L"&", apiKeyPos);
                    if (ampPos == std::wstring::npos) {
                        cleanPath.replace(apiKeyPos + 8, std::wstring::npos, L"***");
                    } else {
                        cleanPath.replace(apiKeyPos + 8, ampPos - (apiKeyPos + 8), L"***");
                    }
                }
                Wh_Log(L"FetchHttps failed for %s%s (WinHttp error: %lu)", host.c_str(), cleanPath.c_str(), err);
            }
            WinHttpCloseHandle(hRequest);
        }
        WinHttpCloseHandle(hConnect);
    }
    return result;
}

std::string FindExternalArtwork(int type, const std::string& queryTitle, const std::string& querySub, bool isTvShow = false) {
    if (queryTitle.empty()) return "";
    std::string finalUrl = "";
    std::string suffix = (type == 2) ? " song cover art" : (isTvShow ? " show poster" : " movie poster");
    std::string term = UrlEncode(queryTitle + " " + querySub + suffix);
    std::wstring host = L"www.bing.com";
    std::wstring path = StrToWStr("/images/async?q=" + term + "&first=0&count=1&adlt=strict");
    std::string html = FetchHttps(host, path);
    
    std::string searchToken = "&quot;turl&quot;:&quot;";
    size_t start = html.find(searchToken);
    
    if (start == std::string::npos) {
        searchToken = "\"turl\":\"";
        start = html.find(searchToken);
    }
    
    if (start != std::string::npos) {
        std::string idToken = "id=OIP.";
        size_t idStart = html.find(idToken, start);
        
        if (idStart != std::string::npos && idStart < start + 500) {
            idStart += 3; 
            size_t endAmp = html.find("&", idStart);
            size_t endQuote = html.find("\"", idStart);
            size_t end = std::min(endAmp, endQuote);
            if (end != std::string::npos) {
                std::string imageId = html.substr(idStart, end - idStart);
                finalUrl = "https://tse1.mm.bing.net/th?id=" + imageId;
                Wh_Log(L"Found external artwork via async: %S", finalUrl.c_str());
            }
        }
    }
    return finalUrl;
}

bool ReadFileBytes(const std::wstring& path, std::vector<char>& data) {
    std::ifstream file(path.c_str(), std::ios::binary | std::ios::ate);
    if (!file) return false;
    std::streamsize size = file.tellg();
    if (size <= 0 || size > 10 * 1024 * 1024) return false; 
    file.seekg(0, std::ios::beg);
    data.resize(size);
    if (!file.read(data.data(), size)) return false;
    return true;
}

std::string UploadToUguu(const std::string& fileUrl) {
    std::string pathStr = UrlDecode(fileUrl);
    size_t filePrefix = pathStr.find("file:///");
    if (filePrefix != std::string::npos) pathStr = pathStr.substr(filePrefix + 8);
    for (auto &c : pathStr) if (c == '/') c = '\\';

    std::vector<char> fileData;
    if (!ReadFileBytes(StrToWStr(pathStr), fileData)) return "";

    HINTERNET hSession = GetSharedWinHttpSession();
    if (!hSession) return "";
    
    HINTERNET hConnect = WinHttpConnect(hSession, L"uguu.se", INTERNET_DEFAULT_HTTPS_PORT, 0);
    if (!hConnect) return "";
    
    HINTERNET hRequest = WinHttpOpenRequest(hConnect, L"POST", L"/upload.php", NULL, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, WINHTTP_FLAG_SECURE);
    if (!hRequest) { WinHttpCloseHandle(hConnect); return ""; }

    std::string boundary = "------------------------VlcRpcModBoundary";
    std::string header = "Content-Type: multipart/form-data; boundary=" + boundary;
    
    std::string bodyHead;
    bodyHead += "--" + boundary + "\r\n";
    bodyHead += "Content-Disposition: form-data; name=\"files[]\"; filename=\"cover.jpg\"\r\n\r\n";
    
    std::string bodyTail;
    bodyTail += "\r\n--" + boundary + "--\r\n";

    DWORD totalSize = (DWORD)(bodyHead.size() + fileData.size() + bodyTail.size());
    bool success = false;
    std::string resultUrl = "";

    if (WinHttpAddRequestHeaders(hRequest, StrToWStr(header).c_str(), (DWORD)-1L, WINHTTP_ADDREQ_FLAG_ADD) &&
        WinHttpSendRequest(hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0, WINHTTP_NO_REQUEST_DATA, 0, totalSize, 0)) {
        
        DWORD bytesWritten;
        WinHttpWriteData(hRequest, bodyHead.c_str(), (DWORD)bodyHead.size(), &bytesWritten);
        WinHttpWriteData(hRequest, fileData.data(), (DWORD)fileData.size(), &bytesWritten);
        WinHttpWriteData(hRequest, bodyTail.c_str(), (DWORD)bodyTail.size(), &bytesWritten);

        if (WinHttpReceiveResponse(hRequest, NULL)) {
            DWORD dwSize = 0, dwDownloaded = 0;
            do {
                if (!WinHttpQueryDataAvailable(hRequest, &dwSize)) break;
                if (dwSize == 0) break;
                std::vector<char> respBuf(dwSize + 1);
                if (WinHttpReadData(hRequest, respBuf.data(), dwSize, &dwDownloaded)) {
                    resultUrl.append(respBuf.data(), dwDownloaded);
                }
            } while (dwSize > 0);
            
            while (!resultUrl.empty() && (resultUrl.back() == '\n' || resultUrl.back() == '\r')) resultUrl.pop_back();
            
            std::string extractedUrl = "";
            size_t urlPos = resultUrl.find("\"url\"");
            if (urlPos != std::string::npos) {
                size_t colonPos = resultUrl.find(":", urlPos);
                if (colonPos != std::string::npos) {
                    size_t quote1 = resultUrl.find("\"", colonPos);
                    if (quote1 != std::string::npos) {
                        size_t quote2 = resultUrl.find("\"", quote1 + 1);
                        if (quote2 != std::string::npos) {
                            extractedUrl = resultUrl.substr(quote1 + 1, quote2 - quote1 - 1);
                        }
                    }
                }
            }

            if (!extractedUrl.empty() && extractedUrl.find("http") == 0) {
                resultUrl = extractedUrl;
                success = true;
            } else if (resultUrl.find("http") == 0) {
                success = true;
            }
        }
    }

    WinHttpCloseHandle(hRequest); WinHttpCloseHandle(hConnect);
    
    if (success) {
        std::string finalCleanUrl;
        for (char c : resultUrl) { if (c != '\\') finalCleanUrl += c; }
        Wh_Log(L"Uploaded local cover art to Uguu: %S", finalCleanUrl.c_str());
        return finalCleanUrl;
    }
    return "";
}

std::string CleanShowTitle(const std::string& input) {
    std::string res = input;
    
    // 1. Strip leading whitespace and release group tags (e.g. "[ANK] ", "[SubsPlease] ", "(HorribleSubs) ")
    while (!res.empty()) {
        size_t first = res.find_first_not_of(" .-_");
        if (first != std::string::npos && first > 0) {
            res = res.substr(first);
        } else if (!res.empty() && (res[0] == '[' || res[0] == '(')) {
            char closeChar = (res[0] == '[') ? ']' : ')';
            size_t closeIdx = res.find(closeChar);
            if (closeIdx != std::string::npos && closeIdx > 0 && closeIdx < 30 && closeIdx + 1 < res.length()) {
                res = res.substr(closeIdx + 1);
                continue;
            }
            break;
        } else {
            break;
        }
    }

    // 2. Helper to strip trailing junk characters like ' ', '.', '-', '_', '(', '[', '~', ':'
    auto stripTrailingJunk = [](std::string& s) {
        while (!s.empty() && (isspace((unsigned char)s.back()) || s.back() == '.' || s.back() == '-' || s.back() == '_' || s.back() == '(' || s.back() == '[' || s.back() == '~' || s.back() == ':' || s.back() == ';')) {
            s.pop_back();
        }
    };
    stripTrailingJunk(res);
    
    // 3. Check for embedded quality/release tags (e.g. IMAX, UHD, 1080p, BluRay, etc.) and truncate
    static const std::vector<std::string> releaseTags = {
        "2160p", "1080p", "720p", "480p", "4k", "uhd", "imax", "bluray", "web-dl", "webrip", "hdrip", "camrip", "brrip", "hdr", "10bit", "x264", "x265", "hevc", "remastered", "extended", "unrated", "proper", "repack", "aac", "ddp", "dts"
    };
    std::string lowerRes = res;
    std::transform(lowerRes.begin(), lowerRes.end(), lowerRes.begin(), ::tolower);
    size_t earliestTag = std::string::npos;
    for (const auto& tag : releaseTags) {
        size_t pos = lowerRes.find(tag);
        if (pos != std::string::npos && pos > 0) {
            char prev = lowerRes[pos - 1];
            if (prev == ' ' || prev == '.' || prev == '-' || prev == '_' || prev == '(' || prev == '[') {
                if (earliestTag == std::string::npos || pos < earliestTag) {
                    earliestTag = pos;
                }
            }
        }
    }
    if (earliestTag != std::string::npos) {
        res = res.substr(0, earliestTag);
        stripTrailingJunk(res);
    }
    
    // 4. Check if ends with (19xx)/(20xx) or [19xx]/[20xx]
    if (res.length() >= 6 && (res.back() == ')' || res.back() == ']')) {
        char open = (res.back() == ')') ? '(' : '[';
        if (res[res.length() - 6] == open && isdigit((unsigned char)res[res.length() - 5]) && isdigit((unsigned char)res[res.length() - 4]) && isdigit((unsigned char)res[res.length() - 3]) && isdigit((unsigned char)res[res.length() - 2])) {
            int yr = 0;
            try { yr = std::stoi(res.substr(res.length() - 5, 4)); } catch(...) {}
            if (yr >= 1900 && yr <= 2100) {
                res = res.substr(0, res.length() - 6);
                stripTrailingJunk(res);
            }
        }
    }
    
    // 5. Check if ends with 4 digits preceded by space, dot, minus, underscore, or open paren/bracket (e.g. "Scarecrow 2026", "Scarecrow.2026")
    if (res.length() >= 5 && isdigit((unsigned char)res[res.length() - 1]) && isdigit((unsigned char)res[res.length() - 2]) && isdigit((unsigned char)res[res.length() - 3]) && isdigit((unsigned char)res[res.length() - 4])) {
        char sep = res[res.length() - 5];
        if (sep == ' ' || sep == '.' || sep == '-' || sep == '_' || sep == '(' || sep == '[') {
            int yr = 0;
            try { yr = std::stoi(res.substr(res.length() - 4)); } catch(...) {}
            if (yr >= 1900 && yr <= 2100) {
                res = res.substr(0, res.length() - 5);
                stripTrailingJunk(res);
            }
        }
    }

    return res;
}

bool ParseTvShowPattern(const std::string& input, std::string& outShow, int& outSeason, int& outEpisode) {
    if (input.empty()) return false;
    
    // Check for S<season>E<episode> (e.g. S01E01, s1e1, S01.E01, S01_E01, S1 - E1)
    for (size_t i = 0; i < input.length(); ++i) {
        if (input[i] == 'S' || input[i] == 's') {
            size_t sIdx = i + 1;
            std::string sNum;
            while (sIdx < input.length() && isdigit((unsigned char)input[sIdx]) && sNum.length() < 3) {
                sNum += input[sIdx++];
            }
            if (!sNum.empty()) {
                size_t eIdx = sIdx;
                while (eIdx < input.length() && (input[eIdx] == '.' || input[eIdx] == '-' || input[eIdx] == '_' || input[eIdx] == ' ')) {
                    eIdx++;
                }
                if (eIdx < input.length() && (input[eIdx] == 'E' || input[eIdx] == 'e')) {
                    eIdx++;
                    std::string eNum;
                    while (eIdx < input.length() && isdigit((unsigned char)input[eIdx]) && eNum.length() < 3) {
                        eNum += input[eIdx++];
                    }
                    if (!eNum.empty()) {
                        std::string rawShow = input.substr(0, i);
                        // Clean trailing separators
                        while (!rawShow.empty() && (rawShow.back() == '.' || rawShow.back() == '-' || rawShow.back() == '_' || rawShow.back() == ' ' || rawShow.back() == '(' || rawShow.back() == '[' || rawShow.back() == '~')) {
                            rawShow.pop_back();
                        }
                        for (char& c : rawShow) {
                            if (c == '.' || c == '_') c = ' ';
                        }
                        size_t first = rawShow.find_first_not_of(' ');
                        if (first != std::string::npos) rawShow = rawShow.substr(first);
                        else rawShow.clear();
                        
                        if (!rawShow.empty()) {
                            outShow = CleanShowTitle(rawShow);
                            try {
                                outSeason = std::stoi(sNum);
                                outEpisode = std::stoi(eNum);
                                return true;
                            } catch (...) {}
                        }
                    }
                }
            }
        }
    }
    
    // Check for <season>x<episode> (e.g. 1x01, 01x01)
    for (size_t i = 1; i + 1 < input.length(); ++i) {
        if (input[i] == 'x' || input[i] == 'X') {
            if (isdigit((unsigned char)input[i - 1]) && isdigit((unsigned char)input[i + 1])) {
                size_t sStart = i - 1;
                while (sStart > 0 && isdigit((unsigned char)input[sStart - 1]) && (i - sStart) < 3) {
                    sStart--;
                }
                if (sStart == 0 || input[sStart - 1] == ' ' || input[sStart - 1] == '.' || input[sStart - 1] == '-' || input[sStart - 1] == '_') {
                    size_t eEnd = i + 1;
                    while (eEnd < input.length() && isdigit((unsigned char)input[eEnd]) && (eEnd - (i + 1)) < 3) {
                        eEnd++;
                    }
                    if (eEnd == input.length() || input[eEnd] == ' ' || input[eEnd] == '.' || input[eEnd] == '-' || input[eEnd] == '_' || !isalnum((unsigned char)input[eEnd])) {
                        std::string sNum = input.substr(sStart, i - sStart);
                        std::string eNum = input.substr(i + 1, eEnd - (i + 1));
                        std::string rawShow = input.substr(0, sStart);
                        while (!rawShow.empty() && (rawShow.back() == '.' || rawShow.back() == '-' || rawShow.back() == '_' || rawShow.back() == ' ' || rawShow.back() == '(' || rawShow.back() == '[' || rawShow.back() == '~')) {
                            rawShow.pop_back();
                        }
                        for (char& c : rawShow) {
                            if (c == '.' || c == '_') c = ' ';
                        }
                        size_t first = rawShow.find_first_not_of(' ');
                        if (first != std::string::npos) rawShow = rawShow.substr(first);
                        else rawShow.clear();
                        
                        if (!rawShow.empty()) {
                            outShow = CleanShowTitle(rawShow);
                            try {
                                outSeason = std::stoi(sNum);
                                outEpisode = std::stoi(eNum);
                                return true;
                            } catch (...) {}
                        }
                    }
                }
            }
        }
    }
    
    return false;
}

std::wstring GetMetadataDiskCachePathW() {
    WCHAR storagePath[MAX_PATH];
    if (Wh_GetModStoragePath(storagePath, ARRAYSIZE(storagePath))) {
        return std::wstring(storagePath) + L"\\metadata_cache.txt";
    }
    return L"";
}

bool LoadMetadataFromDisk(const std::string& cacheKey, TvShowMetadata& outMeta) {
    std::wstring path = GetMetadataDiskCachePathW();
    if (path.empty()) return false;
    std::lock_guard<std::mutex> lock(g_diskMutex);
    std::ifstream file(path.c_str());
    if (!file.is_open()) return false;
    std::string line;
    std::string prefix = cacheKey + "|||";
    time_t now = time(nullptr);
    while (std::getline(file, line)) {
        if (line.find(prefix) == 0) {
            std::vector<std::string> parts;
            size_t pos = 0, next;
            while ((next = line.find("|||", pos)) != std::string::npos) {
                parts.push_back(line.substr(pos, next - pos));
                pos = next + 3;
            }
            parts.push_back(line.substr(pos));
            if (parts.size() >= 9) {
                long long ts = 0;
                try { ts = std::stoll(parts[1]); } catch (...) {}
                if (ts > 0 && difftime(now, (time_t)ts) >= 2592000) {
                    Wh_Log(L"Disk cache expired (>30 days) for %S", cacheKey.c_str());
                    continue;
                }
                outMeta.valid = true;
                outMeta.showName = parts[2];
                outMeta.episodeTitle = parts[3];
                outMeta.posterUrl = parts[4];
                outMeta.rating = parts[5];
                outMeta.genres = parts[6];
                outMeta.runtime = parts[7];
                outMeta.showType = parts[8];
                Wh_Log(L"Loaded metadata from disk cache for %S", cacheKey.c_str());
                return true;
            }
        }
    }
    return false;
}

void SaveMetadataToDisk(const std::string& cacheKey, const TvShowMetadata& meta) {
    if (!meta.valid) return;
    std::wstring path = GetMetadataDiskCachePathW();
    if (path.empty()) return;
    std::lock_guard<std::mutex> lock(g_diskMutex);
    std::ofstream file(path.c_str(), std::ios::app);
    if (file.is_open()) {
        time_t now = time(nullptr);
        file << cacheKey << "|||" << (long long)now << "|||" << meta.showName << "|||" << meta.episodeTitle << "|||"
             << meta.posterUrl << "|||" << meta.rating << "|||" << meta.genres << "|||"
             << meta.runtime << "|||" << meta.showType << "\n";
        file.close();
    }
}

void PruneMetadataDiskCache() {
    std::wstring path = GetMetadataDiskCachePathW();
    if (path.empty()) return;
    std::lock_guard<std::mutex> lock(g_diskMutex);
    std::ifstream file(path.c_str());
    if (!file.is_open()) return;
    
    std::map<std::string, std::string> latestValidLines;
    std::string line;
    time_t now = time(nullptr);
    
    while (std::getline(file, line)) {
        size_t firstSep = line.find("|||");
        if (firstSep != std::string::npos) {
            std::string key = line.substr(0, firstSep);
            size_t secondSep = line.find("|||", firstSep + 3);
            if (secondSep != std::string::npos) {
                long long ts = 0;
                try { ts = std::stoll(line.substr(firstSep + 3, secondSep - (firstSep + 3))); } catch (...) {}
                if (ts > 0 && difftime(now, (time_t)ts) < 2592000) {
                    latestValidLines[key] = line;
                }
            }
        }
    }
    file.close();
    
    std::ofstream out(path.c_str(), std::ios::trunc);
    if (out.is_open()) {
        for (const auto& kv : latestValidLines) {
            out << kv.second << "\n";
        }
        out.close();
    }
}


TvShowMetadata FetchTmdbMetadata(const std::string& showName, const std::string& year, bool isTv, int season, int episode, const std::string& apiKey, const std::string& cacheKey) {
    TvShowMetadata meta;
    Wh_Log(L"FetchTmdbMetadata starting: show='%S', year='%S', isTv=%d, S=%d, E=%d (apiKey len=%d)", showName.c_str(), year.c_str(), isTv ? 1 : 0, season, episode, (int)apiKey.length());
    if (showName.empty() || apiKey.empty()) {
        Wh_Log(L"TMDB skipped: showName or apiKey is empty!");
        return meta;
    }

    std::string cleanName = CleanShowTitle(showName);
    std::string extractedYr = year.empty() ? ExtractYear(showName) : year;
    if (extractedYr.length() > 4) extractedYr = ExtractYear(extractedYr);

    long long tmdbId = 0;
    std::string searchJson;

    if (!isTv) {
        std::string q = UrlEncode(cleanName);
        std::string path = "/3/search/movie?api_key=" + apiKey + "&query=" + q + (!extractedYr.empty() ? "&year=" + extractedYr : "");
        Wh_Log(L"TMDB Movie search query: '%S' (year='%S')", cleanName.c_str(), extractedYr.c_str());
        searchJson = FetchHttps(L"api.themoviedb.org", StrToWStr(path));
        tmdbId = ExtractNumber(searchJson, "id");
        if (tmdbId <= 0 && !extractedYr.empty()) {
            Wh_Log(L"TMDB Movie search with year returned no ID, retrying without year...");
            path = "/3/search/movie?api_key=" + apiKey + "&query=" + q;
            searchJson = FetchHttps(L"api.themoviedb.org", StrToWStr(path));
            tmdbId = ExtractNumber(searchJson, "id");
        }
        if (tmdbId <= 0 && cleanName != showName) {
            Wh_Log(L"TMDB Movie clean name search returned no ID, retrying raw name: '%S'", showName.c_str());
            path = "/3/search/movie?api_key=" + apiKey + "&query=" + UrlEncode(showName);
            searchJson = FetchHttps(L"api.themoviedb.org", StrToWStr(path));
            tmdbId = ExtractNumber(searchJson, "id");
        }
        if (tmdbId <= 0) {
            Wh_Log(L"TMDB Movie search failed or no ID found. Response (first 150 chars): '%S'", searchJson.substr(0, 150).c_str());
        } else {
            Wh_Log(L"TMDB Movie matched ID=%lld", tmdbId);
            std::string detailsPath = "/3/movie/" + NumToStr(tmdbId) + "?api_key=" + apiKey;
            std::string detailsJson = FetchHttps(L"api.themoviedb.org", StrToWStr(detailsPath));
            
            std::string canonTitle = ExtractString(detailsJson, "title");
            if (canonTitle.empty()) canonTitle = ExtractString(searchJson, "title");
            if (canonTitle.empty()) canonTitle = showName;

            std::string posterPath = ExtractString(searchJson, "poster_path");
            if (posterPath.empty()) {
                size_t colPos = detailsJson.find("\"belongs_to_collection\"");
                std::string cleanDetails = detailsJson;
                if (colPos != std::string::npos) {
                    size_t endCol = detailsJson.find('}', colPos);
                    if (endCol != std::string::npos) {
                        cleanDetails.erase(colPos, endCol - colPos + 1);
                    }
                }
                posterPath = ExtractString(cleanDetails, "poster_path");
            }
            
            meta.valid = true;
            meta.showName = canonTitle;
            if (!posterPath.empty()) meta.posterUrl = "https://image.tmdb.org/t/p/w500" + posterPath;

            std::string avgRating = ExtractDoubleStr(detailsJson, "vote_average");
            if (avgRating.empty() || avgRating == "0") avgRating = ExtractDoubleStr(searchJson, "vote_average");
            if (!avgRating.empty() && avgRating != "0") meta.rating = "\xE2\xAD\x90 " + avgRating;

            meta.genres = ExtractGenresStr(detailsJson);

            long long runMin = ExtractNumber(detailsJson, "runtime");
            if (runMin > 0) {
                if (runMin >= 60) {
                    long long h = runMin / 60;
                    long long m = runMin % 60;
                    meta.runtime = NumToStr(h) + "h" + (m > 0 ? " " + NumToStr(m) + "m" : "");
                } else {
                    meta.runtime = NumToStr(runMin) + "m";
                }
            }
            meta.showType = "Movie";
            if (!cacheKey.empty()) SaveMetadataToDisk(cacheKey, meta);
            Wh_Log(L"TMDB Movie success: Title='%S', Rating='%S', Genres='%S', Runtime='%S'", meta.showName.c_str(), meta.rating.c_str(), meta.genres.c_str(), meta.runtime.c_str());
            return meta;
        }
    } else {
        std::string q = UrlEncode(cleanName);
        std::string path = "/3/search/tv?api_key=" + apiKey + "&query=" + q + (!extractedYr.empty() ? "&first_air_date_year=" + extractedYr : "");
        Wh_Log(L"TMDB TV search query: '%S' (year='%S')", cleanName.c_str(), extractedYr.c_str());
        searchJson = FetchHttps(L"api.themoviedb.org", StrToWStr(path));
        tmdbId = ExtractNumber(searchJson, "id");
        if (tmdbId <= 0 && !extractedYr.empty()) {
            Wh_Log(L"TMDB TV search with year returned no ID, retrying without year...");
            path = "/3/search/tv?api_key=" + apiKey + "&query=" + q;
            searchJson = FetchHttps(L"api.themoviedb.org", StrToWStr(path));
            tmdbId = ExtractNumber(searchJson, "id");
        }
        if (tmdbId <= 0 && cleanName != showName) {
            Wh_Log(L"TMDB TV clean name search returned no ID, retrying raw name: '%S'", showName.c_str());
            path = "/3/search/tv?api_key=" + apiKey + "&query=" + UrlEncode(showName);
            searchJson = FetchHttps(L"api.themoviedb.org", StrToWStr(path));
            tmdbId = ExtractNumber(searchJson, "id");
        }
        if (tmdbId <= 0) {
            Wh_Log(L"TMDB TV search failed or no ID found. Response (first 150 chars): '%S'", searchJson.substr(0, 150).c_str());
        } else {
            Wh_Log(L"TMDB TV matched ID=%lld", tmdbId);
            std::string canonName = ExtractString(searchJson, "name");
            if (canonName.empty()) canonName = showName;

            std::string showPosterPath = ExtractString(searchJson, "poster_path");

            std::string epPath = "/3/tv/" + NumToStr(tmdbId) + "/season/" + NumToStr(season) + "/episode/" + NumToStr(episode) + "?api_key=" + apiKey;
            std::string epJson = FetchHttps(L"api.themoviedb.org", StrToWStr(epPath));

            std::string epName = ExtractString(epJson, "name");
            std::string epPosterPath = ExtractString(epJson, "still_path");

            std::string showPath = "/3/tv/" + NumToStr(tmdbId) + "?api_key=" + apiKey;
            std::string showDetailsJson = FetchHttps(L"api.themoviedb.org", StrToWStr(showPath));

            meta.valid = true;
            meta.showName = canonName;
            meta.episodeTitle = epName;
            
            std::string bestPoster = !showPosterPath.empty() ? showPosterPath : epPosterPath;
            if (!bestPoster.empty()) meta.posterUrl = "https://image.tmdb.org/t/p/w500" + bestPoster;

            std::string avgRating = ExtractDoubleStr(epJson, "vote_average");
            if (avgRating.empty() || avgRating == "0") avgRating = ExtractDoubleStr(showDetailsJson, "vote_average");
            if (!avgRating.empty() && avgRating != "0") meta.rating = "\xE2\xAD\x90 " + avgRating;

            meta.genres = ExtractGenresStr(showDetailsJson);

            long long runMin = ExtractNumber(epJson, "runtime");
            if (runMin <= 0) {
                size_t ert = showDetailsJson.find("\"episode_run_time\"");
                if (ert != std::string::npos) {
                    size_t p1 = showDetailsJson.find('[', ert);
                    if (p1 != std::string::npos) {
                        size_t p2 = showDetailsJson.find_first_of(",]", p1 + 1);
                        if (p2 != std::string::npos) {
                            try { runMin = (long long)std::stod(showDetailsJson.substr(p1 + 1, p2 - p1 - 1)); } catch(...) {}
                        }
                    }
                }
            }
            if (runMin > 0) {
                if (runMin >= 60) {
                    long long h = runMin / 60;
                    long long m = runMin % 60;
                    meta.runtime = NumToStr(h) + "h" + (m > 0 ? " " + NumToStr(m) + "m" : "");
                } else {
                    meta.runtime = NumToStr(runMin) + "m";
                }
            }
            meta.showType = "TV Show";
            if (!cacheKey.empty()) SaveMetadataToDisk(cacheKey, meta);
            Wh_Log(L"TMDB TV success: Show='%S', Ep='%S', Rating='%S', Genres='%S', Runtime='%S'", meta.showName.c_str(), meta.episodeTitle.c_str(), meta.rating.c_str(), meta.genres.c_str(), meta.runtime.c_str());
            return meta;
        }
    }

    return meta;
}

TvShowMetadata FetchTvMazeMetadata(const std::string& showName, int season, int episode, const std::string& year = "", const std::string& cacheKey = "") {
    TvShowMetadata meta;
    if (showName.empty() || season <= 0 || episode <= 0) return meta;

    std::string cleanName = CleanShowTitle(showName);
    std::string extractedYr = year.empty() ? ExtractYear(showName) : year;
    
    long long showId = 0;
    std::string showJson;
    
    if (!extractedYr.empty()) {
        std::string queryWithYear = cleanName + " (" + extractedYr + ")";
        std::string q = UrlEncode(queryWithYear);
        std::wstring showPath = StrToWStr("/singlesearch/shows?q=" + q);
        showJson = FetchHttps(L"api.tvmaze.com", showPath);
        showId = ExtractNumber(showJson, "id");
        if (showId > 0) {
            Wh_Log(L"TVMaze matched show with year: %S", queryWithYear.c_str());
        }
    }

    if (showId <= 0) {
        std::string q = UrlEncode(cleanName);
        std::wstring showPath = StrToWStr("/singlesearch/shows?q=" + q);
        showJson = FetchHttps(L"api.tvmaze.com", showPath);
        showId = ExtractNumber(showJson, "id");
    }

    if (showId <= 0 && cleanName != showName) {
        Wh_Log(L"TVMaze show search failed for '%S', retrying raw name: '%S'", cleanName.c_str(), showName.c_str());
        std::string q = UrlEncode(showName);
        std::wstring showPath = StrToWStr("/singlesearch/shows?q=" + q);
        showJson = FetchHttps(L"api.tvmaze.com", showPath);
        showId = ExtractNumber(showJson, "id");
    }

    if (showId <= 0) {
        Wh_Log(L"TVMaze show search failed or not found for: %S", showName.c_str());
        return meta;
    }

    std::string canonName = ExtractString(showJson, "name");
    if (canonName.empty()) canonName = showName;

    std::string showPoster = ExtractString(showJson, "original");
    if (showPoster.empty() || showPoster.find("http") != 0) {
        showPoster = ExtractString(showJson, "medium");
    }

    std::string epPathStr = "/shows/" + NumToStr(showId) + "/episodebynumber?season=" + NumToStr(season) + "&number=" + NumToStr(episode);
    std::string epJson = FetchHttps(L"api.tvmaze.com", StrToWStr(epPathStr));
    
    std::string epName = ExtractString(epJson, "name");
    std::string epPoster = ExtractString(epJson, "original");
    if (epPoster.empty() || epPoster.find("http") != 0) {
        epPoster = ExtractString(epJson, "medium");
    }

    meta.valid = true;
    meta.showName = canonName;
    meta.episodeTitle = epName;
    meta.posterUrl = !showPoster.empty() ? showPoster : epPoster;
    
    std::string avgRating = ExtractDoubleStr(showJson, "average");
    if (!avgRating.empty() && avgRating != "0") {
        meta.rating = "\xE2\xAD\x90 " + avgRating;
    }
    meta.genres = ExtractGenresStr(showJson);
    
    long long runMin = ExtractNumber(showJson, "runtime");
    if (runMin <= 0) runMin = ExtractNumber(showJson, "averageRuntime");
    if (runMin > 0) {
        if (runMin >= 60) {
            long long h = runMin / 60;
            long long m = runMin % 60;
            meta.runtime = NumToStr(h) + "h" + (m > 0 ? " " + NumToStr(m) + "m" : "");
        } else {
            meta.runtime = NumToStr(runMin) + "m";
        }
    }
    meta.showType = ExtractString(showJson, "type");
    if (!cacheKey.empty()) SaveMetadataToDisk(cacheKey, meta);
    Wh_Log(L"TVMaze success: Show='%S', Episode='%S', Rating='%S', Genres='%S', Runtime='%S', Type='%S'", meta.showName.c_str(), meta.episodeTitle.c_str(), meta.rating.c_str(), meta.genres.c_str(), meta.runtime.c_str(), meta.showType.c_str());
    return meta;
}

// =============================================================
// 3. LOGIC HELPERS
// =============================================================

std::string GetAudioLanguages(const std::string& json) {
    std::vector<std::string> activeLangs;
    std::vector<std::string> allLangs;
    for (int i = 0; i < 60; i++) {
        std::string streamKey = "\"Stream " + std::to_string(i) + "\":{";
        size_t start = json.find(streamKey);
        if (start == std::string::npos) continue;
        size_t end = json.find("}", start);
        if (end == std::string::npos) continue;
        std::string block = json.substr(start, end - start);

        if (block.find("\"Type\":\"Audio\"") != std::string::npos) {
            std::string lang = ExtractString(block, "Language");
            if (!lang.empty()) {
                std::string shortLang = lang.substr(0, 2);
                if (shortLang.length() > 0 && shortLang[0] >= 'a' && shortLang[0] <= 'z') shortLang[0] -= 32;
                if (shortLang.length() > 1 && shortLang[1] >= 'a' && shortLang[1] <= 'z') shortLang[1] -= 32;
                bool existsAll = false;
                for (const auto& l : allLangs) if (l == shortLang) existsAll = true;
                if (!existsAll) allLangs.push_back(shortLang);
                bool isDecoded = (block.find("Decoded_format") != std::string::npos) || 
                                 (block.find("Decoded_channels") != std::string::npos);
                if (isDecoded) {
                    bool existsActive = false;
                    for (const auto& l : activeLangs) if (l == shortLang) existsActive = true;
                    if (!existsActive) activeLangs.push_back(shortLang);
                }
            }
        }
    }
    std::vector<std::string>* targetList = (activeLangs.size() > 0) ? &activeLangs : &allLangs;
    std::string result = "";
    for (size_t i = 0; i < targetList->size(); i++) {
        if (i > 0) result += " | ";
        result += (*targetList)[i];
    }
    return result;
}

std::string GetQualityTags(const std::string& json) {
    std::string tags = "";
    for (int i = 0; i < 10; i++) {
        std::string streamKey = "\"Stream " + std::to_string(i) + "\":{";
        size_t start = json.find(streamKey);
        if (start == std::string::npos) continue;
        size_t end = json.find("}", start);
        if (end == std::string::npos) continue;
        std::string block = json.substr(start, end - start);

        if (block.find("\"Type\":\"Video\"") != std::string::npos) {
            std::string res = ExtractString(block, "Video_resolution");
            if (!res.empty()) {
                size_t xPos = res.find("x");
                if (xPos != std::string::npos) {
                    try {
                        long long width = std::stoll(res.substr(0, xPos));
                        if (width >= 3800) tags = "4K";
                        else if (width >= 2500) tags = "2K";
                        else if (width >= 1900) tags = "1080p";
                        else if (width >= 1200) tags = "720p";
                        else tags = "SD";
                    } catch(...) {}
                }
            }

            std::string color = ExtractString(block, "Color_primaries");
            std::string transfer = ExtractString(block, "Color_transfer_function");
            
            bool isHDR = false;
            if (color.find("2020") != std::string::npos) isHDR = true; 
            if (transfer.find("PQ") != std::string::npos) isHDR = true; 
            if (transfer.find("HLG") != std::string::npos) isHDR = true; 
            if (transfer.find("2084") != std::string::npos) isHDR = true;

            if (isHDR) {
                if (!tags.empty()) tags += SEP;
                tags += "HDR";
            }
            break; 
        }
    }
    return tags;
}

std::string GenerateButtonUrl(std::string query, const std::string& provider, const std::string& customUrl) {
    std::string base = "";
    if (provider == "Google") base = "https://www.google.com/search?q=";
    else if (provider == "Bing") base = "https://www.bing.com/search?q=";
    else if (provider == "IMDb") base = "https://www.imdb.com/find/?q=";
    else if (provider == "YouTube") base = "https://www.youtube.com/results?search_query=";
    else if (provider == "Custom") base = customUrl;

    if (base.empty()) base = "https://www.google.com/search?q=";
    if (query.empty()) query = "VLC Media Player";
    return base + UrlEncode(query);
}

int DetectActivityType(const std::string& filename, const std::string& quality) {
    if (!quality.empty()) return 3; // Watching

    std::string ext = "";
    size_t dot = filename.rfind(".");
    if (dot != std::string::npos) {
        ext = filename.substr(dot);
        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    }

    if (ext == ".mp3" || ext == ".flac" || ext == ".wav" || ext == ".m4a" || ext == ".aac" || ext == ".ogg" || ext == ".wma" || ext == ".opus") {
        return 2; // Listening
    }
    if (ext == ".mkv" || ext == ".mp4" || ext == ".avi" || ext == ".mov" || ext == ".wmv" || ext == ".webm" || ext == ".m4v") {
        return 3; // Watching
    }
    return 0; // Playing
}

// =============================================================
// 4. REMOTE FILTER UPDATER & CACHING
// =============================================================

std::wstring GetCacheFilePathW() {
    WCHAR storagePath[MAX_PATH];
    if (Wh_GetModStoragePath(storagePath, ARRAYSIZE(storagePath))) {
        return std::wstring(storagePath) + L"\\junklist.txt";
    }
    return L"";
}

bool IsCacheValid(const std::wstring& path) {
    struct _stat result;
    if (_wstat(path.c_str(), &result) == 0) {
        time_t now = time(nullptr);
        if (difftime(now, result.st_mtime) < 21600) {
            return true;
        }
    }
    return false;
}

struct FilterData {
    std::vector<std::string> sites;
    std::vector<std::string> tlds;
    std::vector<std::string> tags;
    std::vector<std::string> words;
    bool isValid = false;
};

FilterData ParseFilters(const std::string& text) {
    FilterData data;
    if (text.empty()) return data;

    std::stringstream ss(text);
    std::string line;
    int currentSection = 0;

    while (std::getline(ss, line)) {
        if (!line.empty() && line.back() == '\r') line.pop_back();
        if (line.empty()) continue;

        size_t first = line.find_first_not_of(" \t");
        if (first == std::string::npos) continue;
        size_t last = line.find_last_not_of(" \t");
        line = line.substr(first, last - first + 1);

        if (line == "[SITES]") { currentSection = 1; continue; }
        if (line == "[TLDS]") { currentSection = 2; continue; }
        if (line == "[SCENE]") { currentSection = 3; continue; }
        if (line == "[WORDS]") { currentSection = 4; continue; }

        std::string lowerLine = line;
        std::transform(lowerLine.begin(), lowerLine.end(), lowerLine.begin(), ::tolower);

        if (currentSection == 1) data.sites.push_back(lowerLine);
        else if (currentSection == 2) data.tlds.push_back(lowerLine);
        else if (currentSection == 3) data.tags.push_back(lowerLine);
        else if (currentSection == 4) data.words.push_back(lowerLine);
    }

    if (!data.sites.empty() && !data.tlds.empty() && !data.tags.empty() && !data.words.empty()) {
        data.isValid = true;
    }
    return data;
}

FilterData LoadFiltersFromFile(const std::wstring& path) {
    std::ifstream file(path.c_str());
    if (!file.is_open()) return FilterData();
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    return ParseFilters(content);
}

void FetchRemoteFilters(bool strictLocalMode) {
    std::wstring cachePath = GetCacheFilePathW();
    
    if (strictLocalMode) return;

    if (!cachePath.empty() && IsCacheValid(cachePath)) {
        FilterData localData = LoadFiltersFromFile(cachePath);
        if (localData.isValid) {
            std::lock_guard<std::mutex> lock(g_filterMutex);
            g_junkSites = localData.sites;
            g_tlds = localData.tlds;
            g_truncateTags = localData.tags;
            g_junkWords = localData.words;
            g_filtersLoaded = true;
            Wh_Log(L"Loaded filter cache from disk (%s).", cachePath.c_str());
            return;
        }
    }

    PCWSTR url = L"https://raw.githubusercontent.com/ciizerr/vlc-discord-rpc-archive/main/assets/filters.txt";
    const WH_URL_CONTENT* content = Wh_GetUrlContent(url, nullptr);
    if (content) {
        std::string result(content->data, content->length);
        Wh_FreeUrlContent(content);

        FilterData remoteData = ParseFilters(result);
        if (remoteData.isValid) {
            if (!cachePath.empty()) {
                std::ofstream out(cachePath.c_str(), std::ios::trunc);
                if (out.is_open()) {
                    out << result;
                    out.close();
                }
            }
            std::lock_guard<std::mutex> lock(g_filterMutex);
            g_junkSites = remoteData.sites;
            g_tlds = remoteData.tlds;
            g_truncateTags = remoteData.tags;
            g_junkWords = remoteData.words;
            g_filtersLoaded = true;
            Wh_Log(L"Downloaded and applied updated filters from GitHub.");
            return;
        }
    }

    if (!cachePath.empty()) {
        FilterData staleData = LoadFiltersFromFile(cachePath);
        if (staleData.isValid) {
            std::lock_guard<std::mutex> lock(g_filterMutex);
            g_junkSites = staleData.sites;
            g_tlds = staleData.tlds;
            g_truncateTags = staleData.tags;
            g_junkWords = staleData.words;
            g_filtersLoaded = true;
        }
    }
}

// =============================================================
// 5. NOTIFICATIONS
// =============================================================

void ShowSystemToast(const std::wstring& title, const std::wstring& message, const std::string& imageUrl) {
    RunAsync([title, message, imageUrl]() {
        HICON hDynamicIcon = NULL;

        if (!imageUrl.empty() && imageUrl.find("http") == 0) {
            std::wstring wideUrl = StrToWStr(imageUrl);
            const WH_URL_CONTENT* content = Wh_GetUrlContent(wideUrl.c_str(), nullptr);
            if (content && content->length > 0) {
                IStream* pStream = nullptr;
                HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, content->length);
                if (hMem) {
                    void* pData = GlobalLock(hMem);
                    memcpy(pData, content->data, content->length);
                    GlobalUnlock(hMem);
                    CreateStreamOnHGlobal(hMem, TRUE, &pStream);
                }
                if (pStream) {
                    Gdiplus::Bitmap* sourceBmp = Gdiplus::Bitmap::FromStream(pStream);
                    if (sourceBmp && sourceBmp->GetLastStatus() == Gdiplus::Ok) {
                        UINT w = sourceBmp->GetWidth();
                        UINT h = sourceBmp->GetHeight();
                        UINT size = (w < h) ? w : h;

                        Gdiplus::Bitmap* squareBmp = new Gdiplus::Bitmap(size, size, PixelFormat32bppARGB);
                        if (squareBmp && squareBmp->GetLastStatus() == Gdiplus::Ok) {
                            Gdiplus::Graphics graphics(squareBmp);
                            graphics.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);
                            
                            UINT x = (w > h) ? (w - h) / 2 : 0;
                            UINT y = (h > w) ? (h - w) / 2 : 0;
                            
                            graphics.DrawImage(sourceBmp, 
                                              Gdiplus::Rect(0, 0, size, size), 
                                              x, y, size, size, 
                                              Gdiplus::UnitPixel);
                                              
                            squareBmp->GetHICON(&hDynamicIcon);
                        }
                        delete squareBmp;
                    }
                    delete sourceBmp;
                    pStream->Release();
                }
                Wh_FreeUrlContent(content);
            }
        }

        WCHAR storagePath[MAX_PATH];
        std::wstring defaultIconPath = L"";
        if (Wh_GetModStoragePath(storagePath, ARRAYSIZE(storagePath))) {
            defaultIconPath = std::wstring(storagePath) + L"\\vlc-discord-icon.ico";
            struct _stat result;
            if (_wstat(defaultIconPath.c_str(), &result) != 0) {
                const WH_URL_CONTENT* content = Wh_GetUrlContent(L"https://raw.githubusercontent.com/ciizerr/vlc-discord-rpc-archive/main/assets/vlc-discord-icon.ico", nullptr);
                if (content && content->length > 0) {
                    std::ofstream out(defaultIconPath.c_str(), std::ios::binary | std::ios::trunc);
                    if (out.is_open()) {
                        out.write((const char*)content->data, content->length);
                        out.close();
                    }
                }
                if (content) Wh_FreeUrlContent(content);
            }
        }
        
        HWND hwnd = CreateWindowExW(0, L"STATIC", L"VLC-RPC-Toast", WS_OVERLAPPED, 0, 0, 0, 0, NULL, NULL, NULL, NULL);
        if (!hwnd) {
            if (hDynamicIcon) DestroyIcon(hDynamicIcon);
            return;
        }

        HICON hAppIcon = NULL;
        if (!defaultIconPath.empty()) {
            hAppIcon = (HICON)LoadImageW(NULL, defaultIconPath.c_str(), IMAGE_ICON, GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON), LR_LOADFROMFILE);
        }
        if (!hAppIcon) hAppIcon = LoadIcon(NULL, IDI_INFORMATION);

        SendMessageW(hwnd, WM_SETICON, ICON_SMALL, (LPARAM)hAppIcon);
        SendMessageW(hwnd, WM_SETICON, ICON_BIG, (LPARAM)hAppIcon);

        NOTIFYICONDATAW nid = {0};
        nid.cbSize = sizeof(NOTIFYICONDATAW);
        nid.hWnd = hwnd;
        nid.uID = 1338;
        nid.uFlags = NIF_ICON | NIF_TIP | NIF_INFO;
        nid.hIcon = hAppIcon;
        wcscpy_s(nid.szTip, L"VLC Discord RPC");
        wcsncpy_s(nid.szInfoTitle, title.c_str(), 63);
        wcsncpy_s(nid.szInfo, message.c_str(), 255);
        
        if (hDynamicIcon) {
            nid.dwInfoFlags = NIIF_USER | NIIF_LARGE_ICON | NIIF_RESPECT_QUIET_TIME;
            nid.hBalloonIcon = hDynamicIcon;
        } else {
            nid.dwInfoFlags = NIIF_USER | NIIF_RESPECT_QUIET_TIME;
        }
        
        Shell_NotifyIconW(NIM_ADD, &nid);
        
        DWORD start = GetTickCount();
        MSG msg;
        while (GetTickCount() - start < 5000 && !g_stopThread) {
            if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            } else {
                Sleep(50);
            }
        }
        
        Shell_NotifyIconW(NIM_DELETE, &nid);
        if (hAppIcon) DestroyIcon(hAppIcon); 
        if (hDynamicIcon) DestroyIcon(hDynamicIcon);
        DestroyWindow(hwnd);
    });
}

// =============================================================
// 6. MAIN WORKER
// =============================================================

void Worker() {
    bool bStrictLocalMode = Wh_GetIntSetting(L"FilterGroup.StrictLocalMode");
    FetchRemoteFilters(bStrictLocalMode);
    
    std::string defaultId = "1465711556418474148"; 
    std::string myClientId = GetStringSettingSafe(L"AdvancedGroup.ClientId");
    if (myClientId.empty()) myClientId = defaultId;
    
    PruneMetadataDiskCache();

    bool bShowCoverArt = Wh_GetIntSetting(L"DisplayGroup.ShowCoverArt");
    bool bShowChapter = Wh_GetIntSetting(L"DisplayGroup.ShowChapter");
    bool bShowQualityTags = Wh_GetIntSetting(L"DisplayGroup.ShowQualityTags");
    bool bShowAudioLanguage = Wh_GetIntSetting(L"DisplayGroup.ShowAudioLanguage");
    bool bEnableMetadataCleaner = Wh_GetIntSetting(L"FilterGroup.EnableMetadataCleaner");
    
    bool bEnableTmdbScraper = Wh_GetIntSetting(L"ScraperGroup.EnableTmdbScraper");
    bool bEnableTvMazeScraper = Wh_GetIntSetting(L"ScraperGroup.EnableTvMazeScraper");
    bool bShowRating = Wh_GetIntSetting(L"ScraperGroup.ShowRating");
    bool bShowGenres = Wh_GetIntSetting(L"ScraperGroup.ShowGenres");
    bool bShowRuntime = Wh_GetIntSetting(L"ScraperGroup.ShowRuntime");
    bool bShowMediaType = Wh_GetIntSetting(L"ScraperGroup.ShowMediaType");

    std::string customTmdbKey = GetStringSettingSafe(L"ScraperGroup.CustomTmdbApiKey");
    std::string activeTmdbKey = GetActiveTmdbApiKey(customTmdbKey);
    
    bool bMinimalMode = Wh_GetIntSetting(L"DisplayGroup.MinimalMode");
    bool bShowNotifications = Wh_GetIntSetting(L"DisplayGroup.ShowNotifications");

    std::string customJunkStr = GetStringSettingSafe(L"FilterGroup.CustomJunkWords");
    
    std::vector<std::string> customJunkList;
    std::stringstream ss(customJunkStr);
    std::string token;
    while (std::getline(ss, token, ',')) {
        size_t start = token.find_first_not_of(" ");
        if (start != std::string::npos) {
            token = token.substr(start, token.find_last_not_of(" ") - start + 1);
            std::transform(token.begin(), token.end(), token.begin(), ::tolower);
            customJunkList.push_back(token);
        }
    }

    std::string myTheme = GetStringSettingSafe(L"DisplayGroup.Theme");
    std::string assetLarge = myTheme + "vlc_icon";
    std::string assetPlay  = myTheme + "play_icon";
    std::string assetPause = myTheme + "pause_icon";
    std::string assetStop  = myTheme + "stop_icon";

    std::string myProvider = GetStringSettingSafe(L"ButtonGroup.Provider");
    if (myProvider.empty()) myProvider = "Google";
    
    std::string myCustomUrl = GetStringSettingSafe(L"ButtonGroup.CustomUrl");
    
    std::string myBtnLabel = GetStringSettingSafe(L"ButtonGroup.ButtonLabel");
    if (myBtnLabel.empty()) myBtnLabel = "Search This";
    myBtnLabel = SanitizeString(myBtnLabel);

    std::string myMusicNameSetting = GetStringSettingSafe(L"DisplayGroup.MusicActivityName");
    if (myMusicNameSetting.empty()) myMusicNameSetting = "Title";

    int vlcPort = 0;
    std::wstring vlcAuthBase64 = L"OjEyMzQ=";
    ReadVlcConfig(vlcPort, vlcAuthBase64);

    int customPort = Wh_GetIntSetting(L"AdvancedGroup.CustomPort");
    if (customPort > 0) {
        vlcPort = customPort;
    }
    
    std::vector<int> candidatePorts;
    if (vlcPort > 0) candidatePorts.push_back(vlcPort);
    
    int fallbacks[] = {8080, 9080, 9090, 7080, 4212, 8081, 8088};
    for (int p : fallbacks) {
        if (std::find(candidatePorts.begin(), candidatePorts.end(), p) == candidatePorts.end()) {
            candidatePorts.push_back(p);
        }
    }
    int currentPortIndex = 0;

    Wh_Log(L"Worker started. Port=%d, AuthConfigLoaded=%d", vlcPort, vlcAuthBase64.length() > 0 ? 1 : 0);

    HINTERNET hSession = WinHttpOpen(L"VLC-RPC/1.5", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
    if (hSession) {
        WinHttpSetTimeouts(hSession, 1500, 1500, 1500, 1500);
    }
    HINTERNET hConnect = NULL;
    HINTERNET hRequest = NULL;

    HANDLE hPipe = INVALID_HANDLE_VALUE;
    bool isConnected = false;

    std::string lastTop = ""; std::string lastBot = ""; bool lastPlaying = false; 
    std::string lastState = ""; int heartbeat = 0; 
    long long anchorStart = 0; long long anchorEnd = 0;
    int lastActivityType = 0; 
    std::string lastDisplayImage = ""; 

    std::string lastRawFilename = "";
    std::string lastRawTitle = "";
    std::string lastRawArtist = "";
    std::string lastShowName = "";
    std::string cachedFilename = "";
    std::string cachedTitle = "";
    std::string cachedArtist = "";
    std::string cachedShowName = "";

    std::string lastToastMediaKey = "";
    int toastTimer = 0;
    bool toastFired = false;

    bool logged401 = false;
    bool logged200 = false;
    bool loggedFailure = false;

    while (!g_stopThread.load()) {
        int activePort = candidatePorts[currentPortIndex];
        if (hSession && !hConnect) {
            hConnect = WinHttpConnect(hSession, L"127.0.0.1", activePort, 0);
        }
        if (hConnect) hRequest = WinHttpOpenRequest(hConnect, L"GET", L"/requests/status.json", NULL, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, 0);

        bool requestSuccess = false;
        DWORD savedErr = 0;
        if (hRequest) {
            // Always send the Authorization header upfront. VLC returns 401 with wrong
            // password and 200 on success. Any valid HTTP response means we reached VLC.
            std::wstring authHeaders = L"Authorization: Basic " + vlcAuthBase64;
            if (WinHttpSendRequest(hRequest, authHeaders.c_str(), authHeaders.length(), WINHTTP_NO_REQUEST_DATA, 0, 0, 0) &&
                WinHttpReceiveResponse(hRequest, NULL)) {

                DWORD statusCode = 0;
                DWORD dwSizeStatus = sizeof(statusCode);
                WinHttpQueryHeaders(hRequest, WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER, WINHTTP_HEADER_NAME_BY_INDEX, &statusCode, &dwSizeStatus, WINHTTP_NO_HEADER_INDEX);

                if (statusCode == 401) {
                    if (!logged401) {
                        Wh_Log(L"Auth failed (401) on port %d. Check the Lua HTTP password in %%APPDATA%%\\vlc\\vlcrc.", activePort);
                        logged401 = true;
                    }
                    requestSuccess = true;
                } else if (statusCode == 200) {
                    if (!logged200) {
                        Wh_Log(L"Connected to VLC on port %d.", activePort);
                        logged200 = true;
                    }
                    requestSuccess = true;
                } else {
                    static DWORD lastBadCode = 0;
                    static int lastBadPort = 0;
                    if (statusCode != lastBadCode || activePort != lastBadPort) {
                        Wh_Log(L"Port %d responded with status code %lu (not VLC / status.json missing). Checking other candidate ports...", activePort, statusCode);
                        lastBadCode = statusCode;
                        lastBadPort = activePort;
                    }
                    requestSuccess = false;
                }
                std::string json; DWORD dwSize = 0, dwDownloaded = 0;
                do {
                    if (!WinHttpQueryDataAvailable(hRequest, &dwSize)) break;
                    if (dwSize == 0) break;
                    std::vector<char> buffer(dwSize + 1);
                    if (WinHttpReadData(hRequest, buffer.data(), dwSize, &dwDownloaded)) {
                        json.append(buffer.data(), dwDownloaded);
                    }
                } while (dwSize > 0);

                if (!json.empty()) {
                    std::string stateStr = ExtractString(json, "state");

                    if (stateStr == "stopped") {
                         if (lastState != "stopped") { 
                            if (!isConnected || hPipe == INVALID_HANDLE_VALUE) {
                                for (int i=0; i<10; i++) {
                                    std::string name = "\\\\.\\pipe\\discord-ipc-" + std::to_string(i);
                                    hPipe = CreateFileA(name.c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
                                    if (hPipe != INVALID_HANDLE_VALUE) {
                                        Wh_Log(L"Connected to Discord IPC (pipe %d).", i);
                                        break;
                                    }
                                }
                                if (hPipe != INVALID_HANDLE_VALUE) {
                                    std::string hs = "{\"v\":1,\"client_id\":\"" + myClientId + "\"}";
                                    int op=0; int l=(int)hs.length(); DWORD w; WriteFile(hPipe,&op,4,&w,NULL); WriteFile(hPipe,&l,4,&w,NULL); WriteFile(hPipe,hs.c_str(),l,&w,NULL);
                                    isConnected = true;
                                } else {
                                    static bool loggedIpcFail1 = false;
                                    if (!loggedIpcFail1) {
                                        Wh_Log(L"Could not connect to Discord IPC. Is Discord running?");
                                        loggedIpcFail1 = true;
                                    }
                                }
                            }
                            if (isConnected) {
                                std::string js = "{\"cmd\":\"SET_ACTIVITY\",\"args\":{\"pid\":" + NumToStr(GetCurrentProcessId()) + ",\"activity\":{";
                                js += "\"details\":\"Idling\",\"state\":\"Waiting for media...\",\"type\":0,";
                                
                                js += "\"assets\":{\"large_image\":\"" + assetLarge + "\",\"large_text\":\"VLC Media Player\"";
                                if (!bMinimalMode) {
                                    js += ",\"small_image\":\"" + assetStop + "\",\"small_text\":\"Stopped\"";
                                }
                                js += "}";
                                
                                js += "}},\"nonce\":\"1\"}";
                                int op=1; int l=(int)js.length(); DWORD w; WriteFile(hPipe,&op,4,&w,NULL); WriteFile(hPipe,&l,4,&w,NULL); WriteFile(hPipe,js.c_str(),l,&w,NULL);
                            }
                            lastTop = ""; lastState = "stopped"; 
                        }
                    }
                    else if (stateStr == "playing" || stateStr == "paused") {
                        std::string rawFilename = CleanString(ExtractString(json, "filename"));
                        
                        if (rawFilename != lastToastMediaKey) {
                            lastToastMediaKey = rawFilename;
                            toastTimer = 0;
                            toastFired = false;
                        }
                        
                        if (stateStr == "playing" && !toastFired) {
                            toastTimer++;
                        }
                        
                        std::string rawTitle = ExtractString(json, "title");
                        std::string showName = ExtractString(json, "showName");
                        std::string season = ExtractString(json, "seasonNumber");
                        std::string episode = ExtractString(json, "episodeNumber");
                        std::string rawArtist = ExtractString(json, "artist"); 
                        std::string album = ExtractString(json, "album");   
                        std::string artworkUrl = ExtractString(json, "artwork_url");
                        
                        std::string date = ExtractString(json, "date");
                        if (date.empty()) date = ExtractYear(rawFilename); 
                        
                        long long chapter = ExtractNumber(json, "chapter");
                        long long time = ExtractNumber(json, "time");
                        long long length = ExtractNumber(json, "length");
                        bool isPlaying = (stateStr == "playing");
                        
                        std::string quality = GetQualityTags(json);
                        std::string audio = GetAudioLanguages(json);
                        
                        int activityType = DetectActivityType(rawFilename, quality);

                        if (rawFilename != lastRawFilename || rawTitle != lastRawTitle || rawArtist != lastRawArtist || showName != lastShowName) {
                            std::string filenameClean = bEnableMetadataCleaner ? CleanMetadata(rawFilename, customJunkList) : rawFilename;
                            std::string newCachedFilename = filenameClean.empty() ? rawFilename : filenameClean;
                            
                            std::string titleClean = bEnableMetadataCleaner ? CleanMetadata(rawTitle, customJunkList) : rawTitle;
                            std::string newCachedTitle = titleClean.empty() ? newCachedFilename : titleClean;
                            
                            std::string artistClean = bEnableMetadataCleaner ? CleanMetadata(rawArtist, customJunkList) : rawArtist;
                            std::string newCachedArtist = artistClean.empty() ? rawArtist : artistClean;
                            
                            std::string showClean = bEnableMetadataCleaner ? CleanMetadata(showName, customJunkList) : showName;
                            std::string newCachedShowName = showClean.empty() ? showName : showClean;

                            if (rawFilename != lastRawFilename || newCachedFilename != cachedFilename || newCachedTitle != cachedTitle || newCachedArtist != cachedArtist || newCachedShowName != cachedShowName) {
                                if (rawFilename != lastRawFilename || newCachedTitle != cachedTitle || newCachedShowName != cachedShowName) {
                                    Wh_Log(L"Media changed: %S", rawFilename.c_str());
                                }
                                cachedFilename = newCachedFilename;
                                cachedTitle = newCachedTitle;
                                cachedArtist = newCachedArtist;
                                cachedShowName = newCachedShowName;
                            }

                            lastRawFilename = rawFilename;
                            lastRawTitle = rawTitle;
                            lastRawArtist = rawArtist;
                            lastShowName = showName;
                        }

                        std::string filename = cachedFilename;
                        std::string title = cachedTitle;
                        std::string artist = cachedArtist;

                        std::string top = ""; std::string bot = ""; std::string query = "";
                        std::string activityName = ""; 
                        std::string largeText = "VLC Media Player";
                        std::string activeMediaCacheKey = "";

                        if (activityType == 2) { 
                            std::string defaultName = title.empty() ? filename : title;
                            if (myMusicNameSetting == "Artist" && !artist.empty()) {
                                activityName = artist;
                            } else if (myMusicNameSetting == "Album" && !album.empty()) {
                                activityName = album;
                            } else {
                                activityName = defaultName;
                            }
                            top = defaultName; 
                            query = defaultName + " " + artist;

                            if (!artist.empty()) {
                                bot = "by " + artist;
                            } else if (!album.empty()) {
                                bot = album;
                            } else {
                                bot = "Music";
                            }
                            
                            if (!album.empty()) largeText = album;
                            else largeText = "Listening to Music";

                        } else { 
                            bool isTvDetected = false;
                            std::string tvShowName = cachedShowName;
                            int tvSeason = 0;
                            int tvEpisode = 0;

                            if (!showName.empty() && !episode.empty()) {
                                isTvDetected = true;
                                tvShowName = cachedShowName;
                                try { tvSeason = std::stoi(season); } catch(...) { tvSeason = 1; }
                                try { tvEpisode = std::stoi(episode); } catch(...) { tvEpisode = 1; }
                            } else if (bEnableMetadataCleaner || bEnableTvMazeScraper || bEnableTmdbScraper) {
                                std::string parsedShow;
                                int parsedSeason = 0, parsedEpisode = 0;
                                if (ParseTvShowPattern(cachedFilename, parsedShow, parsedSeason, parsedEpisode) ||
                                    ParseTvShowPattern(cachedTitle, parsedShow, parsedSeason, parsedEpisode)) {
                                    isTvDetected = true;
                                    tvShowName = parsedShow;
                                    tvSeason = parsedSeason;
                                    tvEpisode = parsedEpisode;
                                }
                            }

                            activeMediaCacheKey = "";
                            if (bEnableTmdbScraper || (isTvDetected && bEnableTvMazeScraper)) {
                                if (isTvDetected) {
                                    std::string seasonStr = NumToStr(tvSeason);
                                    std::string episodeStr = NumToStr(tvEpisode);
                                    activeMediaCacheKey = "MEDIA_TV_" + tvShowName + (date.empty() ? "" : "_" + date) + "_S" + seasonStr + "_E" + episodeStr;
                                } else if (!title.empty() || !filename.empty()) {
                                    std::string movieName = !title.empty() ? title : filename;
                                    activeMediaCacheKey = "MEDIA_MOV_" + movieName + (date.empty() ? "" : "_" + date);
                                }
                            }

                            bool foundValidMediaMeta = false;
                            bool alreadyAttemptedMediaMeta = false;
                            TvShowMetadata activeMeta;

                            if (!activeMediaCacheKey.empty()) {
                                {
                                    std::lock_guard<std::mutex> lock(g_cacheMutex);
                                    auto it = g_tvMetadataCache.find(activeMediaCacheKey);
                                    if (it != g_tvMetadataCache.end()) {
                                        alreadyAttemptedMediaMeta = true;
                                        if (it->second.valid) {
                                            activeMeta = it->second;
                                            foundValidMediaMeta = true;
                                        }
                                    }
                                }

                                if (!alreadyAttemptedMediaMeta) {
                                    TvShowMetadata diskMeta;
                                    if (LoadMetadataFromDisk(activeMediaCacheKey, diskMeta)) {
                                        std::lock_guard<std::mutex> lock(g_cacheMutex);
                                        g_tvMetadataCache[activeMediaCacheKey] = diskMeta;
                                        if (!diskMeta.posterUrl.empty()) {
                                            g_imageCache[activeMediaCacheKey] = diskMeta.posterUrl;
                                        }
                                        alreadyAttemptedMediaMeta = true;
                                        if (diskMeta.valid) {
                                            activeMeta = diskMeta;
                                            foundValidMediaMeta = true;
                                        }
                                    }
                                }
                            }

                            if (foundValidMediaMeta) {
                                if (isTvDetected) {
                                    std::string seasonStr = NumToStr(tvSeason);
                                    std::string episodeStr = NumToStr(tvEpisode);
                                    activityName = !activeMeta.showName.empty() ? activeMeta.showName : tvShowName;
                                    top = activityName;
                                    if (bShowQualityTags && !quality.empty()) top += SEP + quality;

                                    bot = "S" + seasonStr + " E" + episodeStr;
                                    if (!activeMeta.episodeTitle.empty()) bot += " - " + activeMeta.episodeTitle;
                                    if (bShowRating && !activeMeta.rating.empty()) bot += " | " + activeMeta.rating;
                                    if (bShowGenres && !activeMeta.genres.empty()) bot += " | " + activeMeta.genres;
                                    if (bShowRuntime && !activeMeta.runtime.empty()) bot += " | " + activeMeta.runtime;
                                    if (bShowChapter && chapter >= 0) bot += SEP + "Ch " + NumToStr(chapter + 1);
                                    if (bShowAudioLanguage && !audio.empty()) bot += SEP + audio;

                                    if (bShowMediaType && !activeMeta.showType.empty()) largeText = "Watching " + activeMeta.showType;
                                    else largeText = "Watching TV Show";

                                    query = activityName + " S" + seasonStr + " E" + episodeStr;
                                } else {
                                    activityName = !activeMeta.showName.empty() ? activeMeta.showName : (!title.empty() ? title : filename);
                                    top = activityName;
                                    if (bShowQualityTags && !quality.empty()) top += SEP + quality;

                                    bot = "";
                                    if (bShowRating && !activeMeta.rating.empty()) bot = activeMeta.rating;
                                    if (bShowGenres && !activeMeta.genres.empty()) bot += (bot.empty() ? "" : " | ") + activeMeta.genres;
                                    if (bShowRuntime && !activeMeta.runtime.empty()) bot += (bot.empty() ? "" : " | ") + activeMeta.runtime;
                                    if (bShowChapter && chapter >= 0) {
                                        if (!bot.empty()) bot += SEP;
                                        bot += "Ch " + NumToStr(chapter + 1);
                                    }
                                    if (bShowAudioLanguage && !audio.empty()) {
                                        if (!bot.empty()) bot += SEP;
                                        bot += audio;
                                    }

                                    if (bShowMediaType && !activeMeta.showType.empty()) largeText = "Watching " + activeMeta.showType;
                                    else largeText = "Watching Movie";

                                    query = activityName;
                                }
                            } else {
                                if (!alreadyAttemptedMediaMeta && !activeMediaCacheKey.empty()) {
                                    bool alreadyFetchingMedia = false;
                                    {
                                        std::lock_guard<std::mutex> lock(g_cacheMutex);
                                        if (g_tvFetchInProgress[activeMediaCacheKey]) alreadyFetchingMedia = true;
                                        else g_tvFetchInProgress[activeMediaCacheKey] = true;
                                    }
                                    if (!alreadyFetchingMedia) {
                                        std::string mediaTitle = isTvDetected ? tvShowName : (!title.empty() ? title : filename);
                                        std::string dateCopy = date;
                                        bool isTvCopy = isTvDetected;
                                        int sNumCopy = tvSeason;
                                        int eNumCopy = tvEpisode;
                                        std::string tmdbKeyCopy = activeTmdbKey;
                                        std::string cacheKeyCopy = activeMediaCacheKey;
                                        RunAsync([cacheKeyCopy, mediaTitle, dateCopy, isTvCopy, sNumCopy, eNumCopy, bEnableTmdbScraper, bEnableTvMazeScraper, tmdbKeyCopy]() {
                                            Wh_Log(L"Scraper thread started: media='%S', isTv=%d, TMDB=%d, TVMaze=%d", mediaTitle.c_str(), isTvCopy ? 1 : 0, bEnableTmdbScraper ? 1 : 0, bEnableTvMazeScraper ? 1 : 0);
                                            TvShowMetadata res;
                                            if (isTvCopy && bEnableTvMazeScraper) {
                                                res = FetchTvMazeMetadata(mediaTitle, sNumCopy, eNumCopy, dateCopy, cacheKeyCopy);
                                                if (!res.valid && bEnableTmdbScraper) {
                                                    Wh_Log(L"TVMaze did not return valid metadata. Falling back to TMDB...");
                                                    res = FetchTmdbMetadata(mediaTitle, dateCopy, isTvCopy, sNumCopy, eNumCopy, tmdbKeyCopy, cacheKeyCopy);
                                                }
                                            } else if (bEnableTmdbScraper) {
                                                res = FetchTmdbMetadata(mediaTitle, dateCopy, isTvCopy, sNumCopy, eNumCopy, tmdbKeyCopy, cacheKeyCopy);
                                            }
                                            std::lock_guard<std::mutex> lock(g_cacheMutex);
                                            g_tvMetadataCache[cacheKeyCopy] = res;
                                            if (res.valid && !res.posterUrl.empty()) {
                                                g_imageCache[cacheKeyCopy] = res.posterUrl;
                                            }
                                            g_tvFetchInProgress[cacheKeyCopy] = false;
                                        });
                                    }
                                }

                                if (isTvDetected) {
                                    std::string seasonStr = NumToStr(tvSeason);
                                    std::string episodeStr = NumToStr(tvEpisode);
                                    activityName = tvShowName;
                                    top = activityName;
                                    if (bShowQualityTags && !quality.empty()) top += SEP + quality;
                                    
                                    bot = "S" + seasonStr + " E" + episodeStr;
                                    if (bShowChapter && chapter >= 0) bot += SEP + "Ch " + NumToStr(chapter + 1);
                                    if (bShowAudioLanguage && !audio.empty()) bot += SEP + audio;
                                    query = activityName + " S" + seasonStr + " E" + episodeStr;
                                    largeText = "Watching TV Show";
                                } else if (!title.empty()) {
                                    activityName = title;
                                    top = title;
                                    if (bShowQualityTags && !quality.empty()) top += SEP + quality;
                                    bot = "";
                                    if (bShowChapter && chapter >= 0) bot = "Ch " + NumToStr(chapter + 1);
                                    if (bShowAudioLanguage && !audio.empty()) {
                                        if (!bot.empty()) bot += SEP;
                                        bot += audio;
                                    }
                                    query = title;
                                    largeText = "Watching Movie";
                                } else {
                                    activityName = filename;
                                    top = filename;
                                    if (bShowQualityTags && !quality.empty()) top += SEP + quality;
                                    bot = "";
                                    query = filename;
                                    largeText = "Watching Video";
                                }
                            }
                        }

                        if (activityName.empty()) activityName = "VLC Media Player";

                        std::string displayImage = assetLarge; 
                        std::string targetCacheKey = "";
                        
                        if (bShowCoverArt) {
                            bool isUpload = false;
                            bool isMediaScraperTarget = false;

                            if (!artworkUrl.empty() && artworkUrl.find("file://") == 0) {
                                targetCacheKey = UrlDecode(artworkUrl);
                                isUpload = true;
                            } else if (!activeMediaCacheKey.empty()) {
                                targetCacheKey = activeMediaCacheKey;
                                isMediaScraperTarget = true;
                            } else if (activityType == 2 || activityType == 3 || activityType == 0) {
                                bool isTv = (!showName.empty() && !episode.empty());
                                std::string queryTitle = (activityType == 2) ? (title.empty() ? filename : title) : activityName;
                                std::string querySub = (activityType == 2) ? artist : date;
                                targetCacheKey = "EXT_" + NumToStr(activityType) + "_" + queryTitle + querySub + (isTv ? "_TV" : "");
                            }

                            if (!targetCacheKey.empty()) {
                                bool foundInCache = false;
                                {
                                    std::lock_guard<std::mutex> lock(g_cacheMutex);
                                    if (g_imageCache.find(targetCacheKey) != g_imageCache.end()) {
                                        displayImage = g_imageCache[targetCacheKey];
                                        if (displayImage.empty() || displayImage.find("http") != 0) displayImage = assetLarge; 
                                        foundInCache = true;
                                    }
                                }

                                if (!foundInCache && !isMediaScraperTarget) {
                                    bool alreadyFetching = false;
                                    {
                                        std::lock_guard<std::mutex> lock(g_cacheMutex);
                                        if (g_fetchInProgress[targetCacheKey]) alreadyFetching = true;
                                        else g_fetchInProgress[targetCacheKey] = true;
                                    }

                                    if (!alreadyFetching) {
                                        if (isUpload) {
                                            std::string artUrl = artworkUrl;
                                            RunAsync([targetCacheKey, artUrl]() {
                                                std::string result = UploadToUguu(artUrl);
                                                std::lock_guard<std::mutex> lock(g_cacheMutex);
                                                g_imageCache[targetCacheKey] = result;
                                                g_fetchInProgress[targetCacheKey] = false;
                                            });
                                        } else {
                                            int aType = activityType;
                                            std::string qTitle = (activityType == 2) ? (title.empty() ? filename : title) : activityName;
                                            std::string qSub = (activityType == 2) ? artist : date;
                                            bool isTv = (!showName.empty() && !episode.empty()) || (bEnableTvMazeScraper && targetCacheKey.find("MEDIA_TV_") == 0);
                                            RunAsync([targetCacheKey, aType, qTitle, qSub, isTv]() {
                                                std::string result = FindExternalArtwork(aType, qTitle, qSub, isTv);
                                                std::lock_guard<std::mutex> lock(g_cacheMutex);
                                                g_imageCache[targetCacheKey] = result;
                                                g_fetchInProgress[targetCacheKey] = false;
                                            });
                                        }
                                    }
                                } else if (!foundInCache && isMediaScraperTarget) {
                                    bool mediaFinishedWithoutPoster = false;
                                    {
                                        std::lock_guard<std::mutex> lock(g_cacheMutex);
                                        auto it = g_tvMetadataCache.find(targetCacheKey);
                                        if (it != g_tvMetadataCache.end() && !g_tvFetchInProgress[targetCacheKey]) {
                                            if (!it->second.valid || it->second.posterUrl.empty()) {
                                                mediaFinishedWithoutPoster = true;
                                            }
                                        }
                                    }
                                    if (mediaFinishedWithoutPoster) {
                                        std::string fallbackKey = "EXT_" + NumToStr(activityType) + "_" + activityName + date + (targetCacheKey.find("MEDIA_TV_") == 0 ? "_TV" : "");
                                        bool shouldSpawnFallback = false;
                                        int aType = activityType;
                                        std::string qTitle = activityName;
                                        std::string qSub = date;
                                        bool isTvFallback = (targetCacheKey.find("MEDIA_TV_") == 0);
                                        {
                                            std::lock_guard<std::mutex> lock(g_cacheMutex);
                                            if (g_imageCache.find(fallbackKey) != g_imageCache.end()) {
                                                displayImage = g_imageCache[fallbackKey];
                                                if (displayImage.empty() || displayImage.find("http") != 0) displayImage = assetLarge; 
                                            } else if (!g_fetchInProgress[fallbackKey]) {
                                                g_fetchInProgress[fallbackKey] = true;
                                                shouldSpawnFallback = true;
                                            }
                                        }
                                        if (shouldSpawnFallback) {
                                            RunAsync([fallbackKey, aType, qTitle, qSub, isTvFallback]() {
                                                std::string result = FindExternalArtwork(aType, qTitle, qSub, isTvFallback);
                                                std::lock_guard<std::mutex> lock(g_cacheMutex);
                                                g_imageCache[fallbackKey] = result;
                                                g_fetchInProgress[fallbackKey] = false;
                                            });
                                        }
                                    }
                                }
                            }
                        }

                        auto now = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
                        long long cStart = now - (time * 1000);
                        long long cEnd = cStart + (length * 1000);

                        long long drift = cStart - anchorStart;
                        if (drift < 0) drift = -drift;

                        bool textChg = (top != lastTop || bot != lastBot);
                        bool stateChg = (isPlaying != lastPlaying);
                        bool typeChg = (activityType != lastActivityType);
                        bool artChg = (displayImage != lastDisplayImage);
                        bool majorDrift = (drift > 3000); 
                        bool force = (heartbeat > 30);

                        if (textChg || stateChg || typeChg || artChg || majorDrift || force) { 
                            anchorStart = cStart; anchorEnd = cEnd;
                            
                            if (!isConnected || hPipe == INVALID_HANDLE_VALUE) {
                                for (int i=0; i<10; i++) {
                                    std::string name = "\\\\.\\pipe\\discord-ipc-" + std::to_string(i);
                                    hPipe = CreateFileA(name.c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
                                    if (hPipe != INVALID_HANDLE_VALUE) {
                                        Wh_Log(L"Connected to Discord IPC (pipe %d).", i);
                                        break;
                                    }
                                }
                                if (hPipe != INVALID_HANDLE_VALUE) {
                                    std::string hs = "{\"v\":1,\"client_id\":\"" + myClientId + "\"}";
                                    int op=0; int l=(int)hs.length(); DWORD w; WriteFile(hPipe,&op,4,&w,NULL); WriteFile(hPipe,&l,4,&w,NULL); WriteFile(hPipe,hs.c_str(),l,&w,NULL);
                                    isConnected = true;
                                } else {
                                    static bool loggedIpcFail2 = false;
                                    if (!loggedIpcFail2) {
                                        Wh_Log(L"Could not connect to Discord IPC. Is Discord running?");
                                        loggedIpcFail2 = true;
                                    }
                                }
                            }

                            if (isConnected) {
                                DWORD bytesAvail = 0;
                                while (PeekNamedPipe(hPipe, NULL, 0, NULL, &bytesAvail, NULL) && bytesAvail > 0) {
                                    char sink[1024];
                                    DWORD toRead = (DWORD)std::min((size_t)bytesAvail, sizeof(sink));
                                    DWORD br;
                                    if (!ReadFile(hPipe, sink, toRead, &br, NULL) || br == 0) break;
                                }

                                std::string state = isPlaying ? "Playing" : "Paused";
                                if (query.empty()) query = "VLC Media Player";
                                std::string btnUrl = GenerateButtonUrl(query, myProvider, myCustomUrl);
                                
                                std::string js = "{\"cmd\":\"SET_ACTIVITY\",\"args\":{\"pid\":" + NumToStr(GetCurrentProcessId()) + ",\"activity\":{";
                                js += "\"details\":\"" + SanitizeString(top) + "\",";
                                if (bot.empty()) {
                                    js += "\"state\":\"" + state + "\",";
                                } else {
                                    js += "\"state\":\"" + SanitizeString(bot) + SEP + state + "\",";
                                }
                                js += "\"type\":" + NumToStr(activityType) + ",";
                                js += "\"name\":\"" + SanitizeString(activityName) + "\","; 
                                
                                js += "\"assets\":{\"large_image\":\"" + SanitizeString(displayImage) + "\",\"large_text\":\"" + SanitizeString(largeText) + "\"";
                                if (!bMinimalMode) {
                                    js += ",\"small_image\":\"" + (isPlaying ? assetPlay : assetPause) + "\",\"small_text\":\"" + state + "\"";
                                }
                                js += "}";
                                
                                if (isPlaying && anchorEnd > 0) {
                                    js += ",\"timestamps\":{\"start\":" + NumToStr(anchorStart) + ",\"end\":" + NumToStr(anchorEnd) + "}";
                                }
                                js += ",\"buttons\":[{\"label\":\"" + myBtnLabel + "\",\"url\":\"" + btnUrl + "\"}]";
                                
                                js += "}},\"nonce\":\"1\"}";
                                
                                int op=1; int l=(int)js.length(); DWORD w;
                                bool s1 = WriteFile(hPipe,&op,4,&w,NULL);
                                bool s2 = WriteFile(hPipe,&l,4,&w,NULL);
                                bool s3 = WriteFile(hPipe,js.c_str(),l,&w,NULL);
                                
                                if (!s1 || !s2 || !s3) { CloseHandle(hPipe); hPipe = INVALID_HANDLE_VALUE; isConnected = false; }
                            }
                            
                            lastTop = top; lastBot = bot; lastPlaying = isPlaying; lastActivityType = activityType; 
                            lastDisplayImage = displayImage; 
                            heartbeat = 0; lastState = stateStr;
                        } else {
                            heartbeat++;
                        }
                        
                        if (bShowNotifications && isPlaying && !toastFired && toastTimer >= 3) {
                            if (!top.empty()) {
                                ShowSystemToast(StrToWStr(top), StrToWStr(bot), displayImage);
                            }
                            toastFired = true;
                        }
                    }
                }
            } else {
                savedErr = GetLastError(); // capture before any WinHttpCloseHandle clears it
            }
            WinHttpCloseHandle(hRequest); hRequest = NULL;
        }

        if (!requestSuccess) {
            if (!loggedFailure) {
                Wh_Log(L"Could not reach VLC on port %d (WinHttp error %lu). Trying fallback ports...", activePort, savedErr);
                loggedFailure = true;
            }
            if (hConnect) { WinHttpCloseHandle(hConnect); hConnect = NULL; }
            currentPortIndex = (currentPortIndex + 1) % candidatePorts.size();
            if (currentPortIndex == 0 && loggedFailure) {
                Wh_Log(L"Checked all candidate ports. Still searching for VLC Web Interface... (Is VLC Web Interface enabled and running?)");
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        } else {
            loggedFailure = false;
            for(int k=0; k<10; k++) {
                if (g_stopThread.load()) break;
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
        }
    }
    
    if (isConnected && hPipe != INVALID_HANDLE_VALUE) CloseHandle(hPipe);
    if (hConnect) WinHttpCloseHandle(hConnect);
    if (hSession) WinHttpCloseHandle(hSession);
}

BOOL Wh_ModInit() {
    Gdiplus::GdiplusStartupInput gdiplusStartupInput;
    Gdiplus::GdiplusStartup(&g_gdiplusToken, &gdiplusStartupInput, NULL);
    g_stopThread = false;
    g_workerThread.emplace(Worker);
    return TRUE;
}

void Wh_ModUninit() {
    g_stopThread = true;

    // Close the shared WinHttp session first — this aborts any in-flight async
    // HTTP requests (scraper, artwork) immediately, so the thread joins below
    // complete in milliseconds rather than waiting out 3-second per-request timeouts.
    {
        std::lock_guard<std::mutex> lock(g_sessionMutex);
        if (g_hWinHttpSession) {
            WinHttpCloseHandle(g_hWinHttpSession);
            g_hWinHttpSession = NULL;
        }
    }

    if (g_workerThread && g_workerThread->joinable()) {
        g_workerThread->join();
    }
    g_workerThread.reset();

    {
        std::lock_guard<std::mutex> lock(g_asyncMutex);
        if (g_asyncTasks.has_value()) {
            for (auto& t : *g_asyncTasks) {
                if (t.thread.joinable()) {
                    t.thread.join();
                }
            }
            g_asyncTasks.reset();
        }
    }

    Gdiplus::GdiplusShutdown(g_gdiplusToken);
}

BOOL Wh_ModSettingsChanged(BOOL* bReload) {
    *bReload = TRUE;
    return TRUE;
}
