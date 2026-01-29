// ==WindhawkMod==
// @id              vlc-discord-rpc
// @name            VLC Discord Rich Presence
// @description     Shows your playing status, quality tags (4K/HDR), and interactive buttons on Discord.
// @version         1.0.1
// @author          ciizerr
// @github          https://github.com/ciizerr
// @include         vlc.exe
// @compilerOptions -lwinhttp
// @architecture    x86
// @architecture    x86-64
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# VLC Discord Rich Presence

Seamlessly integrates VLC Media Player with Discord to display playback status, media metadata, and resolution tags.

## Features
* **Smart Recognition:** Automatically identifies Movies, TV Shows (with Season/Episode), and Anime.
* **Quality Tags:** Displays resolution and format tags (4K, HDR, 1080p) based on the media file.
* **Interactive Buttons:** Adds a "Search This" button to your status, redirecting to Google, IMDb, or YouTube.
* **Visual Themes:** Includes options for Default and Dark Mode icon sets.
* **Playback Status:** Accurately displays elapsed time, remaining time, and pause/idle states.

## Icon Themes
Users can customize the appearance of the Rich Presence icons via the Mod Settings.

* **Default:** The standard orange VLC cone.
![default theme](https://raw.githubusercontent.com/ciizerr/vlc-discord-rpc-archive/main/screenshots/themes/default.png)

* **Dark:** A dark-mode variant for low-light aesthetics.
![dark theme](https://raw.githubusercontent.com/ciizerr/vlc-discord-rpc-archive/main/screenshots/themes/dark_.png)

**Submissions:** We are accepting community designs for new icon themes. If you have created a set (vlc, play, pause, stop), please contact `ciizerr` on Discord.

## Setup Instructions (First Run Only)
For this mod to retrieve data from VLC, the Web Interface must be enabled.

1.  Open VLC Media Player.
2.  Go to **Tools** > **Preferences** (or press `Ctrl+P`).
3.  In the bottom-left corner, under *Show settings*, select **All**.
4.  Navigate to **Interface** > **Main interfaces**.
5.  On the right panel, check the box for **Web**.

    ![Enable Web Interface](https://raw.githubusercontent.com/ciizerr/vlc-discord-rpc-archive/main/screenshots/setup/web-interface.png)

6.  In the left sidebar, expand *Main interfaces* and click on **Lua**.
7.  Under *Lua HTTP*, set the **Password** to `1234` and **Port** to `8080`.

    ![Lua Password Setup](https://raw.githubusercontent.com/ciizerr/vlc-discord-rpc-archive/main/screenshots/setup/password.png)

8.  Click **Save** and restart VLC.

## Configuration
**Search Provider:** You can change the destination of the search button (Google, Bing, IMDb) in the mod settings.

**Custom Client ID:** Power users who wish to upload their own assets can provide a custom Application ID in the settings.

## Feedback & Support
For bug reports, feature suggestions, or general feedback, please reach out via:
* **Discord:** `ciizerr`
* **GitHub:** [vlc-discord-rpc-archive](https://github.com/ciizerr/vlc-discord-rpc-archive) (contains cross-platform resources of vlc-discord-rpc)
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- ClientId: "1465711556418474148"
  $name: Discord Client ID
  $description: "The Application ID from the Discord Developer Portal. Leave default to use the official one."
- Theme: ""
  $name: Icon Theme
  $description: "Prefix for your assets. Upload images like 'dark_play_icon' to use the Dark theme."
  $options:
    - "": Default (vlc_icon)
    - "dark_": Dark Mode (dark_vlc_icon)
- Provider: Google
  $name: Search Provider
  $options:
    - Google: Google
    - Bing: Bing
    - IMDb: IMDb
    - YouTube: YouTube
    - Custom: Custom URL
- CustomUrl: ""
  $name: Custom URL
  $description: "For other sites (Yahoo, MyAnimeList), enter their search URL here. Example: https://myanimelist.net/search/all?q="
- ButtonLabel: "Search This"
  $name: Button Label
  $description: "The text displayed on the Discord button (Max 30 chars)."
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <winhttp.h>
#include <string>
#include <thread>
#include <vector>
#include <cstdio>
#include <iomanip>
#include <sstream>
#include <atomic> 

// =============================================================
// ⚙️ GLOBALS
// =============================================================
// Use std::atomic for thread safety
std::atomic<bool> g_stopThread{false};
std::thread g_workerThread;

const std::wstring VLC_PASS_BASE64 = L"OjEyMzQ="; 
const std::string SEP = " \xE2\x97\x8F ";

// =============================================================
// 1. HELPERS
// =============================================================

std::string WStrToStr(const std::wstring& wstr) {
    if (wstr.empty()) return "";
    int size = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
    std::string str(size, 0);
    WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &str[0], size, NULL, NULL);
    return str;
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

// Sanitize string: Replaces quotes with single quotes, removes weird chars.
// The "Best" Movie -> The 'Best' Movie
std::string SanitizeString(const std::string& s) {
    std::string out;
    for (char c : s) {
        if (c == '"') {
            out += '\''; // Replace double quote with single quote
        } else if (c == '\\') {
            // Skip backslash to prevent JSON breakage
        } else if ((unsigned char)c < 32) {
            // Skip control characters
        } else {
            out += c;
        }
    }
    return out;
}

std::string NumToStr(long long num) { return std::to_string(num); }

// Robust Extractor: Handles escaped quotes properly
std::string ExtractString(const std::string& json, const std::string& key) {
    std::string search = "\"" + key + "\":\"";
    size_t start = json.find(search);
    if (start == std::string::npos) return "";
    start += search.length();

    // Loop until we find a quote that is NOT escaped
    size_t current = start;
    while (current < json.length()) {
        size_t nextQuote = json.find("\"", current);
        if (nextQuote == std::string::npos) return ""; // Malformed JSON

        // Check if this quote is escaped (preceded by backslash)
        if (nextQuote > 0 && json[nextQuote - 1] == '\\') {
             current = nextQuote + 1;
        } else {
            return json.substr(start, nextQuote - start);
        }
    }
    return "";
}

long long ExtractNumber(const std::string& json, const std::string& key) {
    std::string search = "\"" + key + "\":";
    size_t start = json.find(search);
    if (start == std::string::npos) return -1;
    start += search.length();
    
    if (json[start] == '"') {
        start++;
        size_t end = json.find("\"", start);
        if (end == std::string::npos) return -1;
        try { return (long long)std::stod(json.substr(start, end - start)); } catch(...) { return -1; }
    }
    
    size_t endComma = json.find(",", start);
    size_t endBrace = json.find("}", start);
    size_t end = (endComma < endBrace) ? endComma : endBrace;
    if (end == std::string::npos) return -1;
    try { return (long long)std::stod(json.substr(start, end - start)); } catch(...) { return -1; }
}

std::string CleanString(std::string str) {
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

// =============================================================
// 2. LOGIC
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
                if (shortLang[0] >= 'a' && shortLang[0] <= 'z') shortLang[0] -= 32;
                if (shortLang[1] >= 'a' && shortLang[1] <= 'z') shortLang[1] -= 32;
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
    std::string streamKey = "\"Stream 0\":{";
    size_t start = json.find(streamKey);
    if (start == std::string::npos) return "";
    size_t end = json.find("}", start);
    if (end == std::string::npos) return "";
    std::string block = json.substr(start, end - start);

    std::string res = ExtractString(block, "Video_resolution");
    if (!res.empty()) {
        size_t xPos = res.find("x");
        if (xPos != std::string::npos) {
            try {
                long long width = std::stoll(res.substr(0, xPos));
                if (width >= 3800) tags += "4K";
                else if (width >= 2500) tags += "2K";
                else if (width >= 1900) tags += "1080p";
                else if (width >= 1200) tags += "720p";
                else tags += "SD";
            } catch(...) {}
        }
    }
    std::string color = ExtractString(block, "Color_primaries");
    std::string transfer = ExtractString(block, "Color_transfer_function");
    bool isHDR = (color.find("2020") != std::string::npos) || (transfer.find("PQ") != std::string::npos) || (transfer.find("HLG") != std::string::npos);
    if (isHDR) {
        if (!tags.empty()) tags += SEP;
        tags += "HDR";
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

// =============================================================
// 3. MAIN WORKER
// =============================================================

void Worker() {
    // 1. CLIENT ID
    std::string defaultId = "1465711556418474148"; 
    
    PCWSTR sId = Wh_GetStringSetting(L"ClientId");
    std::string userSetting = sId ? WStrToStr(sId) : "";
    Wh_FreeStringSetting(sId);
    
    std::string myClientId = userSetting.empty() ? defaultId : userSetting;

    // 2. OTHER SETTINGS
    PCWSTR sTheme = Wh_GetStringSetting(L"Theme");
    std::string myTheme = sTheme ? WStrToStr(sTheme) : "";
    Wh_FreeStringSetting(sTheme);

    PCWSTR sProv = Wh_GetStringSetting(L"Provider");
    std::string myProvider = sProv ? WStrToStr(sProv) : "Google";
    Wh_FreeStringSetting(sProv);

    PCWSTR sCust = Wh_GetStringSetting(L"CustomUrl");
    std::string myCustomUrl = sCust ? WStrToStr(sCust) : "";
    Wh_FreeStringSetting(sCust);

    PCWSTR sLbl = Wh_GetStringSetting(L"ButtonLabel");
    std::string myBtnLabel = sLbl ? WStrToStr(sLbl) : "Search This";
    Wh_FreeStringSetting(sLbl);

    if (myBtnLabel.length() > 30) myBtnLabel = myBtnLabel.substr(0, 30);
    if (myBtnLabel.empty()) myBtnLabel = "Search";
    myBtnLabel = SanitizeString(myBtnLabel);

    // ASSET NAMES
    std::string assetLarge = myTheme + "vlc_icon";
    std::string assetPlay  = myTheme + "play_icon";
    std::string assetPause = myTheme + "pause_icon";
    std::string assetStop  = myTheme + "stop_icon";

    // NETWORK HANDLES
    HINTERNET hSession = WinHttpOpen(L"VLC-CPP/1.0", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
    HINTERNET hConnect = NULL;
    HINTERNET hRequest = NULL;

    HANDLE hPipe = INVALID_HANDLE_VALUE;
    bool isConnected = false;

    // LOCAL VARS
    std::string lastTop = ""; std::string lastBot = ""; bool lastPlaying = false; 
    std::string lastState = ""; int heartbeat = 0; 
    long long anchorStart = 0; long long anchorEnd = 0;

    // MAIN LOOP
    while (!g_stopThread.load()) {
        // Robust Connection Handling: Re-connect if needed
        if (hSession && !hConnect) {
            hConnect = WinHttpConnect(hSession, L"127.0.0.1", 8080, 0);
        }

        if (hConnect) {
            hRequest = WinHttpOpenRequest(hConnect, L"GET", L"/requests/status.json", NULL, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, 0);
        }

        bool requestSuccess = false;

        if (hRequest) {
            std::wstring headers = L"Authorization: Basic " + VLC_PASS_BASE64;
            
            if (WinHttpSendRequest(hRequest, headers.c_str(), headers.length(), WINHTTP_NO_REQUEST_DATA, 0, 0, 0) &&
                WinHttpReceiveResponse(hRequest, NULL)) {
                
                requestSuccess = true;
                std::string json; DWORD dwSize = 0, dwDownloaded = 0;
                
                // ROBUST DATA READING LOOP
                do {
                    // Check availability first
                    if (!WinHttpQueryDataAvailable(hRequest, &dwSize)) {
                        requestSuccess = false; 
                        break; 
                    }
                    if (dwSize == 0) break; // End of data

                    std::vector<char> buffer(dwSize + 1);
                    // Check read success
                    if (!WinHttpReadData(hRequest, buffer.data(), dwSize, &dwDownloaded)) {
                        requestSuccess = false;
                        break;
                    }
                    json.append(buffer.data(), dwDownloaded);
                } while (dwSize > 0);

                if (requestSuccess && !json.empty()) {
                    std::string stateStr = ExtractString(json, "state");
                    
                    if (stateStr == "stopped") {
                        if (lastState != "stopped") { 
                            // IDLE
                            if (!isConnected || hPipe == INVALID_HANDLE_VALUE) {
                                for (int i=0; i<10; i++) {
                                    std::string name = "\\\\.\\pipe\\discord-ipc-" + std::to_string(i);
                                    hPipe = CreateFileA(name.c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
                                    if (hPipe != INVALID_HANDLE_VALUE) break;
                                }
                                if (hPipe != INVALID_HANDLE_VALUE) {
                                    std::string hs = "{\"v\":1,\"client_id\":\"" + myClientId + "\"}";
                                    int op=0; int l=(int)hs.length(); DWORD w; WriteFile(hPipe,&op,4,&w,NULL); WriteFile(hPipe,&l,4,&w,NULL); WriteFile(hPipe,hs.c_str(),l,&w,NULL);
                                    isConnected = true;
                                }
                            }
                            if (isConnected) {
                                std::string js = "{\"cmd\":\"SET_ACTIVITY\",\"args\":{\"pid\":" + NumToStr(GetCurrentProcessId()) + ",\"activity\":{";
                                js += "\"details\":\"Idling\",\"state\":\"Waiting for media...\",";
                                js += "\"assets\":{\"large_image\":\"" + assetLarge + "\",\"large_text\":\"VLC Media Player\",\"small_image\":\"" + assetStop + "\",\"small_text\":\"Stopped\"}";
                                js += "}},\"nonce\":\"1\"}";
                                int op=1; int l=(int)js.length(); DWORD w; WriteFile(hPipe,&op,4,&w,NULL); WriteFile(hPipe,&l,4,&w,NULL); WriteFile(hPipe,js.c_str(),l,&w,NULL);
                            }
                            lastTop = ""; lastState = "stopped"; 
                        }
                    }
                    else if (stateStr == "playing" || stateStr == "paused") {
                        std::string filename = CleanString(ExtractString(json, "filename"));
                        std::string showName = ExtractString(json, "showName");
                        std::string season = ExtractString(json, "seasonNumber");
                        std::string episode = ExtractString(json, "episodeNumber");
                        std::string title = ExtractString(json, "title");
                        
                        long long chapter = ExtractNumber(json, "chapter");
                        long long time = ExtractNumber(json, "time");
                        long long length = ExtractNumber(json, "length");
                        bool isPlaying = (stateStr == "playing");
                        
                        std::string audio = GetAudioLanguages(json);
                        std::string quality = GetQualityTags(json);

                        std::string top = ""; std::string bot = ""; std::string query = "";

                        if (!showName.empty() && !episode.empty()) {
                            top = showName;
                            if (!quality.empty()) top += SEP + quality;
                            bot = "S" + season + "E" + episode;
                            if (chapter >= 0) bot += SEP + "Ch " + NumToStr(chapter + 1);
                            if (!audio.empty()) bot += SEP + audio;
                            query = showName + " S" + season + "E" + episode;
                        } 
                        else if (!title.empty()) {
                            top = CleanString(title);
                            if (!quality.empty()) top += SEP + quality;
                            if (chapter >= 0) bot = "Ch " + NumToStr(chapter + 1); else bot = "Video";
                            if (!audio.empty()) bot += SEP + audio;
                            query = CleanString(title);
                        }
                        else {
                            top = filename;
                            if (!quality.empty()) top += SEP + quality;
                            bot = "Video";
                            query = filename;
                        }

                        auto now = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
                        long long cStart = now - (time * 1000);
                        long long cEnd = cStart + (length * 1000);

                        long long drift = cStart - anchorStart;
                        if (drift < 0) drift = -drift;

                        bool textChg = (top != lastTop || bot != lastBot);
                        bool stateChg = (isPlaying != lastPlaying);
                        bool majorDrift = (drift > 3000); 
                        bool force = (heartbeat > 30);

                        if (textChg || stateChg || majorDrift || force) {
                            anchorStart = cStart; anchorEnd = cEnd;
                            
                            // PAYLOAD
                            if (!isConnected || hPipe == INVALID_HANDLE_VALUE) {
                                for (int i=0; i<10; i++) {
                                    std::string name = "\\\\.\\pipe\\discord-ipc-" + std::to_string(i);
                                    hPipe = CreateFileA(name.c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
                                    if (hPipe != INVALID_HANDLE_VALUE) break;
                                }
                                if (hPipe != INVALID_HANDLE_VALUE) {
                                    std::string hs = "{\"v\":1,\"client_id\":\"" + myClientId + "\"}";
                                    int op=0; int l=(int)hs.length(); DWORD w; WriteFile(hPipe,&op,4,&w,NULL); WriteFile(hPipe,&l,4,&w,NULL); WriteFile(hPipe,hs.c_str(),l,&w,NULL);
                                    isConnected = true;
                                }
                            }

                            if (isConnected) {
                                std::string state = isPlaying ? "Playing" : "Paused";
                                if (query.empty()) query = "VLC Media Player";
                                std::string btnUrl = GenerateButtonUrl(query, myProvider, myCustomUrl);
                                
                                std::string js = "{\"cmd\":\"SET_ACTIVITY\",\"args\":{\"pid\":" + NumToStr(GetCurrentProcessId()) + ",\"activity\":{";
                                js += "\"details\":\"" + SanitizeString(top) + "\",";
                                js += "\"state\":\"" + SanitizeString(bot) + " (" + state + ")\",";
                                
                                // THEME ASSETS
                                js += "\"assets\":{\"large_image\":\"" + assetLarge + "\",\"large_text\":\"VLC Media Player\",\"small_image\":\"" + (isPlaying ? assetPlay : assetPause) + "\",\"small_text\":\"" + state + "\"}";
                                
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

                            lastTop = top; lastBot = bot; lastPlaying = isPlaying; heartbeat = 0; lastState = stateStr;
                        } else {
                            heartbeat++;
                        }
                    }
                }
            } 
            WinHttpCloseHandle(hRequest); hRequest = NULL;
        }

        // Error Handling: If connection failed, invalidate handle to force reconnect
        if (!requestSuccess) {
            if (hConnect) { WinHttpCloseHandle(hConnect); hConnect = NULL; }
            std::this_thread::sleep_for(std::chrono::milliseconds(2000)); // Sleep longer if VLC is closed (2s)
        } else {
            for(int k=0; k<10; k++) {
                if (g_stopThread.load()) break;
                std::this_thread::sleep_for(std::chrono::milliseconds(100)); // 1s refresh when active
            }
        }
    }
    
    if (isConnected && hPipe != INVALID_HANDLE_VALUE) CloseHandle(hPipe);
    if (hConnect) WinHttpCloseHandle(hConnect);
    if (hSession) WinHttpCloseHandle(hSession);
}

// =============================================================
// LIFECYCLE
// =============================================================

BOOL Wh_ModInit() {
    g_stopThread = false;
    g_workerThread = std::thread(Worker);
    return TRUE;
}

void Wh_ModUninit() {
    g_stopThread = true;
    if (g_workerThread.joinable()) g_workerThread.join();
}

BOOL Wh_ModSettingsChanged(BOOL* bReload) {
    *bReload = TRUE;
    return TRUE;
}