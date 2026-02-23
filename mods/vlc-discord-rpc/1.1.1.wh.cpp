// ==WindhawkMod==
// @id              vlc-discord-rpc
// @name            VLC Discord Rich Presence
// @description     Shows your playing status, quality tags (4K/HDR), and interactive buttons on Discord.
// @version         1.1.1
// @author          ciizerr
// @github          https://github.com/ciizerr
// @homepage        https://vlc-rpc.vercel.app/
// @include         vlc.exe
// @compilerOptions -lwinhttp
// @architecture    x86
// @architecture    x86-64
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# VLC Discord Rich Presence

Seamlessly integrates VLC Media Player with Discord to display playback status, media metadata, resolution tags, and **Album Artwork.** Please proceed to Setup Instructions (First Run Only).

## Features
* **Smart Cover Art Engine:** Automatically uploads local album art to `0x0.st`. If local art is missing, it intelligently scrapes high-res posters and album covers directly from the web
    * **Important:** Artwork is fetched using title-based search. If your filename does not closely match the official title, incorrect artwork may appear.
* **Metadata Scrubber:** Intelligently strips piracy site URLs, promotional phrases, bracketed tags, and scene release technical info to guarantee perfect Discord display titles.
* **Custom Junk Filter:** Define your own list of annoying tags or site names to automatically remove from media titles.
* **Smart Activity Status:** Dynamically switches between "Listening to **Song**", "Watching **Movie**", or "Playing **Video**" based on the file type.
* **Clean Metadata:** * **Music:** Displays Song Title, Artist, and Album.
    * **Video:** Displays Title, Season/Episode, Chapter, and Audio Language.
* **Quality Tags:** Displays resolution and format tags (4K, HDR, 1080p, 10-bit) based on the media file.
* **Interactive Buttons:** Adds a "Search This" button to your status, redirecting to Google, IMDb, or YouTube.
* **Visual Themes:** Includes options for Default and Dark Mode icon sets.

## Icon Themes
Users can customize the appearance of the Rich Presence icons via the Mod Settings.

* **Default:** The standard orange VLC cone.
![default theme](https://raw.githubusercontent.com/ciizerr/vlc-discord-rpc-archive/main/screenshots/themes/default.gif)

* **Dark:** A dark-mode variant for low-light aesthetics.
![dark theme](https://raw.githubusercontent.com/ciizerr/vlc-discord-rpc-archive/main/screenshots/themes/dark_.gif)

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
**Show Cover Art:** Toggle to enable/disable the fetching and uploading of cover art. If disabled, the mod will use the standard VLC icon.

**Custom Junk Words:** Add specific words or phrases to be automatically scrubbed from your media titles before displaying on Discord.

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
- ShowCoverArt: true
  $name: Show Cover Art
  $description: "If enabled, attempts to upload local art or fetch online posters via smart web search. Disable to use the standard VLC icon."
- CustomJunkWords: ""
  $name: Custom Junk Words (Comma-Separated)
  $description: "Add your own words to remove from titles before they display on Discord (e.g., mysite, ripper_name, 8k)."
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
#include <atomic>
#include <map>
#include <fstream>
#include <mutex>
#include <algorithm> 
#include <cctype>
#include <sstream>

// =============================================================
// ⚙️ GLOBALS
// =============================================================
std::atomic<bool> g_stopThread{false};
std::thread g_workerThread;
const std::wstring VLC_PASS_BASE64 = L"OjEyMzQ="; 
const std::string SEP = " \xE2\x97\x8F ";

std::map<std::string, std::string> g_imageCache;
std::mutex g_cacheMutex;

// =============================================================
// 1. STRING & METADATA HELPERS
// =============================================================

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
    std::string search = "\"" + key + "\":\"";
    size_t start = json.find(search);
    if (start == std::string::npos) return "";
    start += search.length();
    size_t current = start;
    while (current < json.length()) {
        size_t nextQuote = json.find("\"", current);
        if (nextQuote == std::string::npos) return ""; 
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

// V6 OMNI-SCRUBBER (Handles Phrases, URLs, and Scene Releases)
std::string CleanMetadata(std::string text, const std::vector<std::string>& customJunk) {
    if (text.empty()) return "";

    // 1. Strip everything inside [] only
    std::string noBrackets = "";
    bool inBracket = false;
    for (char c : text) {
        if (c == '[') inBracket = true;
        else if (c == ']') { inBracket = false; continue; }
        
        if (!inBracket) noBrackets += c;
    }

    // 2. Erase media extensions
    size_t dot = noBrackets.find_last_of(".");
    if (dot != std::string::npos) {
        std::string ext = noBrackets.substr(dot);
        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
        if (ext == ".mp3" || ext == ".mkv" || ext == ".mp4" || ext == ".avi" || ext == ".flac" || ext == ".m4a" || ext == ".wav") {
            noBrackets = noBrackets.substr(0, dot);
        }
    }

    // 3. The URL Assassin (Wipe Site + TLD combos before dots become spaces)
    std::string lowerText = noBrackets;
    std::transform(lowerText.begin(), lowerText.end(), lowerText.begin(), ::tolower);
    
    std::vector<std::string> junkSites = { 
        "olamovies", "vegamovies", "moviesmod", "katmoviehd", "mkvcinemas", 
        "filmyzilla", "filmywap", "1tamilmv", "jiorockers", "ibomma", "yts", 
        "yify", "psa", "qxr", "tigole", "rarbg", "pahe", "pagalworld", "mrjatt", 
        "djpunjab", "wapking", "songspk", "djmaza", "pendujatt", "naasongs", 
        "masstamilan", "jiosaavn"
    };
    std::vector<std::string> tlds = { 
        ".top", ".com", ".net", ".org", ".in", ".nl", ".is", ".to", ".pw", 
        ".cc", ".site", ".info", ".biz", ".co", ".nz", ".uk", ".mx", ".ws", ".pro" 
    };
    
    for (const auto& site : junkSites) {
        for (const auto& tld : tlds) {
            std::string url = site + tld;
            size_t pos;
            while ((pos = lowerText.find(url)) != std::string::npos) {
                // Replace with exact length spaces to keep indexing aligned
                noBrackets.replace(pos, url.length(), std::string(url.length(), ' '));
                lowerText.replace(pos, url.length(), std::string(url.length(), ' '));
            }
        }
    }

    // 4. Replace Periods (.) and Underscores (_) with Spaces
    for (char &c : noBrackets) {
        if (c == '.' || c == '_' || c == '~') c = ' ';
    }

    lowerText = noBrackets;
    std::transform(lowerText.begin(), lowerText.end(), lowerText.begin(), ::tolower);

    // 5. The Scene Truncator (Chop off everything after these tags)
    std::vector<std::string> truncateTags = {
        "2160p", "1080p", "720p", "480p", "4k", "bluray", "web-dl", "webrip", "hdrip", "camrip", "brrip"
    };
    for (const auto& tag : truncateTags) {
        size_t pos = lowerText.find(tag);
        if (pos != std::string::npos) {
            noBrackets = noBrackets.substr(0, pos);
            lowerText = lowerText.substr(0, pos); 
        }
    }

    // 6. Phrase & Standard Junk Word Removal
    // Note: Longer multi-word phrases MUST go first!
    std::vector<std::string> junkWords = {
        "downloaded from", "download from", "shared by", "brought to you by", "visit website",
        "downloaded", "download", "320kbps", "128kbps", "kbps", "official video", "lyric video", 
        "ringtone", "full song", "pagalworld", "mrjatt", 
        "djpunjab", "wapking", "songspk", "djmaza", "pendujatt", "naasongs", "masstamilan", 
        "jiosaavn", "olamovies", "uhdmovies", "vegamovies", "moviesmod", "katmoviehd", "mkvcinemas", 
        "filmyzilla", "filmywap", "1tamilmv", "jiorockers", "ibomma", "yts", "yify", "psa", "qxr", 
        "tigole", "rarbg", "pahe", "x264", "x265", "hevc", "10bit", "site"
    };

    junkWords.insert(junkWords.end(), customJunk.begin(), customJunk.end());

    std::string result = noBrackets;
    for (const auto& word : junkWords) {
        if (word.empty()) continue;
        size_t pos;
        while (true) {
            lowerText = result;
            std::transform(lowerText.begin(), lowerText.end(), lowerText.begin(), ::tolower);
            pos = lowerText.find(word);
            if (pos == std::string::npos) break;
            result.replace(pos, word.length(), " "); 
        }
    }
    
    // 7. Replace remaining double spaces
    std::string finalClean;
    for (char c : result) {
        if (c == ' ' && !finalClean.empty() && finalClean.back() == ' ') continue; 
        finalClean += c;
    }
    
    // 8. Trim edges
    size_t start = finalClean.find_first_not_of(" -");
    if (start == std::string::npos) return "";
    size_t end = finalClean.find_last_not_of(" -");
    return finalClean.substr(start, end - start + 1);
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
// 2. NETWORK & IMAGE API LOGIC
// =============================================================

std::string FetchHttps(const std::wstring& host, const std::wstring& path) {
    std::string result = "";
    HINTERNET hSession = WinHttpOpen(L"VLC-RPC-Mod/1.4", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
    if (!hSession) return "";

    WinHttpSetTimeouts(hSession, 3000, 3000, 3000, 3000);

    HINTERNET hConnect = WinHttpConnect(hSession, host.c_str(), INTERNET_DEFAULT_HTTPS_PORT, 0);
    if (hConnect) {
        HINTERNET hRequest = WinHttpOpenRequest(hConnect, L"GET", path.c_str(), NULL, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, WINHTTP_FLAG_SECURE);
        if (hRequest) {
            std::wstring headers = L"User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/120.0.0.0 Safari/537.36\r\n";
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
            }
            WinHttpCloseHandle(hRequest);
        }
        WinHttpCloseHandle(hConnect);
    }
    WinHttpCloseHandle(hSession);
    return result;
}

std::string FindExternalArtwork(int type, const std::string& queryTitle, const std::string& querySub) {
    if (queryTitle.empty()) return "";
    
    std::string cacheKey = "EXT_" + NumToStr(type) + "_" + queryTitle + querySub;
    {
        std::lock_guard<std::mutex> lock(g_cacheMutex);
        if (g_imageCache.find(cacheKey) != g_imageCache.end()) {
            return g_imageCache[cacheKey]; 
        }
    }

    std::string finalUrl = "";
    std::string suffix = (type == 2) ? " song cover art" : " movie poster";
    std::string term = UrlEncode(queryTitle + " " + querySub + suffix);
    
    std::wstring host = L"www.bing.com";
    std::wstring path = StrToWStr("/images/search?q=" + term);
    
    std::string html = FetchHttps(host, path);
    
    std::string searchToken = "id=OIP.";
    size_t start = html.find(searchToken);
    
    if (start != std::string::npos) {
        start += 3; 
        size_t endQuote = html.find("\"", start);
        size_t endAmp = html.find("&", start);
        size_t end = std::min(endQuote, endAmp);
        
        if (end != std::string::npos) {
            std::string imageId = html.substr(start, end - start);
            finalUrl = "https://tse1.mm.bing.net/th?id=" + imageId + "&w=512&h=512&c=1";
        }
    }

    {
        std::lock_guard<std::mutex> lock(g_cacheMutex);
        g_imageCache[cacheKey] = finalUrl; 
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

std::string UploadTo0x0st(const std::string& fileUrl) {
    std::string pathStr = UrlDecode(fileUrl);
    size_t filePrefix = pathStr.find("file:///");
    if (filePrefix != std::string::npos) pathStr = pathStr.substr(filePrefix + 8);
    for (auto &c : pathStr) if (c == '/') c = '\\';
    
    {
        std::lock_guard<std::mutex> lock(g_cacheMutex);
        if (g_imageCache.find(pathStr) != g_imageCache.end()) return g_imageCache[pathStr];
    }

    std::vector<char> fileData;
    if (!ReadFileBytes(StrToWStr(pathStr), fileData)) return "";

    HINTERNET hSession = WinHttpOpen(L"VLC-RPC-Mod/1.4", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
    if (!hSession) return "";
    HINTERNET hConnect = WinHttpConnect(hSession, L"0x0.st", INTERNET_DEFAULT_HTTPS_PORT, 0);
    if (!hConnect) { WinHttpCloseHandle(hSession); return ""; }
    
    HINTERNET hRequest = WinHttpOpenRequest(hConnect, L"POST", L"/", NULL, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, WINHTTP_FLAG_SECURE);
    if (!hRequest) { WinHttpCloseHandle(hConnect); WinHttpCloseHandle(hSession); return ""; }

    std::string boundary = "------------------------VlcRpcModBoundary";
    std::string header = "Content-Type: multipart/form-data; boundary=" + boundary;
    
    std::string bodyHead;
    bodyHead += "--" + boundary + "\r\n";
    bodyHead += "Content-Disposition: form-data; name=\"file\"; filename=\"cover.jpg\"\r\n\r\n";
    
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
            if (resultUrl.find("http") == 0) success = true;
        }
    }

    WinHttpCloseHandle(hRequest); WinHttpCloseHandle(hConnect); WinHttpCloseHandle(hSession);
    if (success) {
        std::lock_guard<std::mutex> lock(g_cacheMutex);
        g_imageCache[pathStr] = resultUrl;
        return resultUrl;
    }
    return "";
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
// 4. MAIN WORKER
// =============================================================

void Worker() {
    std::string defaultId = "1465711556418474148"; 
    PCWSTR sId = Wh_GetStringSetting(L"ClientId");
    std::string myClientId = sId ? WStrToStr(sId) : defaultId;
    if (myClientId.empty()) myClientId = defaultId;
    Wh_FreeStringSetting(sId);

    bool bShowCoverArt = Wh_GetIntSetting(L"ShowCoverArt");

    // Process Custom Junk Words Setting
    PCWSTR sJunk = Wh_GetStringSetting(L"CustomJunkWords");
    std::string customJunkStr = sJunk ? WStrToStr(sJunk) : "";
    Wh_FreeStringSetting(sJunk);
    
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
    myBtnLabel = SanitizeString(myBtnLabel);

    std::string assetLarge = myTheme + "vlc_icon";
    std::string assetPlay  = myTheme + "play_icon";
    std::string assetPause = myTheme + "pause_icon";
    std::string assetStop  = myTheme + "stop_icon";

    HINTERNET hSession = WinHttpOpen(L"VLC-RPC/1.4", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
    HINTERNET hConnect = NULL;
    HINTERNET hRequest = NULL;

    HANDLE hPipe = INVALID_HANDLE_VALUE;
    bool isConnected = false;

    std::string lastTop = ""; std::string lastBot = ""; bool lastPlaying = false; 
    std::string lastState = ""; int heartbeat = 0; 
    long long anchorStart = 0; long long anchorEnd = 0;
    int lastActivityType = 0; 
    
    std::string lastDisplayImage = ""; 

    while (!g_stopThread.load()) {
        if (hSession && !hConnect) hConnect = WinHttpConnect(hSession, L"127.0.0.1", 8080, 0);
        if (hConnect) hRequest = WinHttpOpenRequest(hConnect, L"GET", L"/requests/status.json", NULL, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, 0);

        bool requestSuccess = false;
        if (hRequest) {
            std::wstring headers = L"Authorization: Basic " + VLC_PASS_BASE64;
            if (WinHttpSendRequest(hRequest, headers.c_str(), headers.length(), WINHTTP_NO_REQUEST_DATA, 0, 0, 0) &&
                WinHttpReceiveResponse(hRequest, NULL)) {
                
                requestSuccess = true;
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
                                js += "\"details\":\"Idling\",\"state\":\"Waiting for media...\",\"type\":0,";
                                js += "\"assets\":{\"large_image\":\"" + assetLarge + "\",\"large_text\":\"VLC Media Player\",\"small_image\":\"" + assetStop + "\",\"small_text\":\"Stopped\"}";
                                js += "}},\"nonce\":\"1\"}";
                                int op=1; int l=(int)js.length(); DWORD w; WriteFile(hPipe,&op,4,&w,NULL); WriteFile(hPipe,&l,4,&w,NULL); WriteFile(hPipe,js.c_str(),l,&w,NULL);
                            }
                            lastTop = ""; lastState = "stopped"; 
                        }
                    }
                    else if (stateStr == "playing" || stateStr == "paused") {
                        std::string rawFilename = CleanString(ExtractString(json, "filename"));
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

                        // CLEAN UI STRINGS FOR DISCORD 
                        std::string filenameClean = CleanMetadata(rawFilename, customJunkList);
                        std::string filename = filenameClean.empty() ? rawFilename : filenameClean;
                        
                        std::string titleClean = CleanMetadata(rawTitle, customJunkList);
                        std::string title = titleClean.empty() ? filename : titleClean;
                        
                        std::string artistClean = CleanMetadata(rawArtist, customJunkList);
                        std::string artist = artistClean.empty() ? rawArtist : artistClean;

                        std::string top = ""; std::string bot = ""; std::string query = "";
                        std::string activityName = ""; 
                        std::string largeText = "VLC Media Player";

                        if (activityType == 2) { 
                            activityName = title.empty() ? filename : title;
                            top = activityName; 
                            query = activityName + " " + artist;

                            if (!artist.empty()) {
                                bot = "by " + artist;
                            } else if (!album.empty()) {
                                bot = album;
                            } else {
                                bot = "Music";
                            }
                            
                            if (!album.empty()) largeText = album;
                            else largeText = "Listening to VLC";

                        } else { 
                            if (!showName.empty() && !episode.empty()) {
                                std::string showClean = CleanMetadata(showName, customJunkList);
                                activityName = showClean.empty() ? showName : showClean;
                                top = activityName;
                                if (!quality.empty()) top += SEP + quality;
                                bot = "S" + season + "E" + episode;
                                if (chapter >= 0) bot += SEP + "Ch " + NumToStr(chapter + 1);
                                if (!audio.empty()) bot += SEP + audio;
                                query = activityName + " S" + season + "E" + episode;
                                largeText = "Watching TV Show";
                            } 
                            else if (!title.empty()) {
                                activityName = title;
                                top = title;
                                if (!quality.empty()) top += SEP + quality;
                                if (chapter >= 0) bot = "Ch " + NumToStr(chapter + 1); else bot = "Video";
                                if (!audio.empty()) bot += SEP + audio;
                                query = title;
                                largeText = "Watching Movie";
                            }
                            else {
                                activityName = filename;
                                top = filename;
                                if (!quality.empty()) top += SEP + quality;
                                bot = "Video";
                                query = filename;
                                largeText = "Watching Video";
                            }
                        }

                        if (activityName.empty()) activityName = "VLC Media Player";

                        std::string displayImage = assetLarge; 
                        
                        if (bShowCoverArt) {
                            if (!artworkUrl.empty()) {
                                if (artworkUrl.find("file://") == 0) {
                                    std::string uploaded = UploadTo0x0st(artworkUrl);
                                    if (!uploaded.empty() && uploaded.find("http") == 0) {
                                        displayImage = uploaded;
                                    }
                                } else if (artworkUrl.find("http") == 0) {
                                    displayImage = artworkUrl;
                                }
                            }
                            
                            if (displayImage == assetLarge && (activityType == 2 || activityType == 3)) {
                                std::string queryTitle = (activityType == 2) ? (title.empty() ? filename : title) : activityName;
                                std::string querySub = (activityType == 2) ? artist : date;
                                
                                std::string externalArt = FindExternalArtwork(activityType, queryTitle, querySub);
                                if (!externalArt.empty() && externalArt.find("http") == 0) {
                                    displayImage = externalArt;
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
                                js += "\"type\":" + NumToStr(activityType) + ",";
                                js += "\"name\":\"" + SanitizeString(activityName) + "\","; 
                                
                                js += "\"assets\":{\"large_image\":\"" + displayImage + "\",\"large_text\":\"" + SanitizeString(largeText) + "\",\"small_image\":\"" + (isPlaying ? assetPlay : assetPause) + "\",\"small_text\":\"" + state + "\"}";
                                
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
                    }
                }
            } 
            WinHttpCloseHandle(hRequest); hRequest = NULL;
        }

        if (!requestSuccess) {
            if (hConnect) { WinHttpCloseHandle(hConnect); hConnect = NULL; }
            std::this_thread::sleep_for(std::chrono::milliseconds(2000));
        } else {
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