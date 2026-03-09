// ==WindhawkMod==
// @id              windhawk-translate-sync
// @name            Windhawk Translation Sync
// @description     Automatically syncs Windhawk UI translations from the official windhawk-translate GitHub repository. Backs up originals and restores on uninstall.
// @version         1.0
// @author          communism420
// @github          https://github.com/communism420
// @include         windhawk.exe
// @compilerOptions -lshlwapi -lwininet
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Windhawk Translation Sync v1.0

Keeps Windhawk's UI translations up to date by syncing them from the official
[windhawk-translate](https://github.com/ramensoftware/windhawk-translate) repository.

## How it works

Windhawk stores UI translations as JSON files at:
`C:\Program Files\Windhawk\UI\resources\app\extensions\windhawk\webview\locales\{lang}\translation.json`

The YAML files from the windhawk-translate repository contain both native C++
strings (top-level keys like `EXITDLG_CONTENT`) and UI strings (under the `app:` key).
This mod downloads the YAML, extracts only the `app:` section, converts it to JSON,
and writes it to the corresponding `translation.json` file.

## Settings

- **Update interval**: How often (in hours) to check for updates. 0 = once at startup.
- **Languages**: Comma-separated list (e.g. `ru,de,fr`). Empty = sync all existing.
- **Show notifications**: Balloon notification when translations are updated or restored.

## Safety

- Original files are backed up before modification (`.bak` suffix).
- On uninstall/disable, originals are restored from backup.
- If a download fails, the existing file is kept unchanged.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- UpdateIntervalHours: 24
  $name: Update interval (hours)
  $description: How often to sync translations. 0 = only once at startup.
- Languages: ""
  $name: Languages
  $description: Comma-separated codes (e.g. ru,de,fr). Empty = all existing languages.
- ShowNotifications: true
  $name: Show notifications
  $description: Show a tray notification when translations are updated.
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <windows.h>
#include <wininet.h>
#include <shlwapi.h>
#include <shlobj.h>

#include <string>
#include <vector>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <mutex>
#include <algorithm>

namespace fs = std::filesystem;

// ─── Globals ────────────────────────────────────────────────────────────────
static HANDLE g_syncThread = nullptr;
static volatile bool g_stopThread = false;
static std::mutex g_fileMutex;

// ─── Helpers ────────────────────────────────────────────────────────────────

static std::wstring Utf8ToWide(const std::string& s) {
    if (s.empty()) return {};
    int n = MultiByteToWideChar(CP_UTF8, 0, s.c_str(), (int)s.size(), nullptr, 0);
    std::wstring w(n, 0);
    MultiByteToWideChar(CP_UTF8, 0, s.c_str(), (int)s.size(), &w[0], n);
    return w;
}

static std::string WideToUtf8(const std::wstring& w) {
    if (w.empty()) return {};
    int n = WideCharToMultiByte(CP_UTF8, 0, w.c_str(), (int)w.size(),
                                nullptr, 0, nullptr, nullptr);
    std::string s(n, 0);
    WideCharToMultiByte(CP_UTF8, 0, w.c_str(), (int)w.size(),
                        &s[0], n, nullptr, nullptr);
    return s;
}

static std::string ReadFileStr(const fs::path& p) {
    std::ifstream f(p, std::ios::binary);
    if (!f) return {};
    return std::string((std::istreambuf_iterator<char>(f)),
                        std::istreambuf_iterator<char>());
}

static bool WriteFileStr(const fs::path& p, const std::string& data) {
    std::ofstream f(p, std::ios::binary | std::ios::trunc);
    if (!f) return false;
    f.write(data.data(), data.size());
    return f.good();
}

static void ShowBalloon(const wchar_t* msg) {
    NOTIFYICONDATAW nid = {};
    nid.cbSize = sizeof(nid);
    nid.hWnd = FindWindowW(L"WindhawkTrayWindow", nullptr);
    if (!nid.hWnd) nid.hWnd = FindWindowW(nullptr, L"Windhawk");
    nid.uFlags = NIF_INFO;
    nid.dwInfoFlags = NIIF_INFO;
    wcsncpy(nid.szInfoTitle, L"Translation Sync", ARRAYSIZE(nid.szInfoTitle));
    wcsncpy(nid.szInfo, msg, ARRAYSIZE(nid.szInfo));
    Shell_NotifyIconW(NIM_MODIFY, &nid);
}

// ─── Download ───────────────────────────────────────────────────────────────

static bool DownloadUrl(const std::wstring& url, std::string& out) {
    out.clear();
    HINTERNET hInet = InternetOpenW(L"TranslateSync/1.0", INTERNET_OPEN_TYPE_PRECONFIG,
                                     nullptr, nullptr, 0);
    if (!hInet) return false;

    HINTERNET hUrl = InternetOpenUrlW(hInet, url.c_str(), nullptr, 0,
                                       INTERNET_FLAG_RELOAD | INTERNET_FLAG_NO_CACHE_WRITE |
                                       INTERNET_FLAG_SECURE, 0);
    if (!hUrl) { InternetCloseHandle(hInet); return false; }

    char buf[8192];
    DWORD bytesRead;
    while (InternetReadFile(hUrl, buf, sizeof(buf), &bytesRead) && bytesRead > 0) {
        out.append(buf, bytesRead);
    }

    InternetCloseHandle(hUrl);
    InternetCloseHandle(hInet);
    return !out.empty();
}

// ─── YAML → JSON converter (app: section only) ─────────────────────────────

struct YamlNode {
    std::string value;
    std::vector<std::pair<std::string, YamlNode>> children;
    bool isNull = false;
};

static std::string TrimRight(const std::string& s) {
    size_t end = s.find_last_not_of(" \t\r\n");
    return (end == std::string::npos) ? "" : s.substr(0, end + 1);
}

static std::string UnquoteYaml(const std::string& s) {
    if (s.size() >= 2) {
        if ((s.front() == '"' && s.back() == '"') ||
            (s.front() == '\'' && s.back() == '\''))
            return s.substr(1, s.size() - 2);
    }
    return s;
}

static int GetIndent(const std::string& line) {
    int n = 0;
    for (char c : line) { if (c == ' ') n++; else break; }
    return n;
}

static YamlNode ParseYamlLines(const std::vector<std::string>& lines,
                                size_t& idx, int parentIndent) {
    YamlNode node;
    while (idx < lines.size()) {
        const std::string& raw = lines[idx];
        std::string line = TrimRight(raw);
        if (line.empty() || line[0] == '#') { idx++; continue; }

        int indent = GetIndent(line);
        if (indent <= parentIndent) break;

        std::string trimmed = line.substr(indent);
        size_t colon = trimmed.find(':');
        if (colon == std::string::npos) { idx++; continue; }

        std::string key = trimmed.substr(0, colon);
        std::string rest = TrimRight(trimmed.substr(colon + 1));
        if (!rest.empty() && rest[0] == ' ') rest = rest.substr(1);

        idx++;

        if (rest.empty()) {
            YamlNode child = ParseYamlLines(lines, idx, indent);
            node.children.push_back({key, child});
        } else if (rest == "~") {
            YamlNode child;
            child.isNull = true;
            node.children.push_back({key, child});
        } else {
            YamlNode child;
            child.value = UnquoteYaml(rest);
            node.children.push_back({key, child});
        }
    }
    return node;
}

static std::string JsonEscape(const std::string& s) {
    std::string out;
    out.reserve(s.size() + 8);
    for (unsigned char c : s) {
        switch (c) {
            case '"':  out += "\\\""; break;
            case '\\': out += "\\\\"; break;
            case '\n': out += "\\n";  break;
            case '\r': out += "\\r";  break;
            case '\t': out += "\\t";  break;
            default:
                if (c < 0x20) {
                    char tmp[8];
                    snprintf(tmp, sizeof(tmp), "\\u%04x", c);
                    out += tmp;
                } else {
                    out += (char)c;
                }
        }
    }
    return out;
}

static void NodeToJson(const YamlNode& node, std::string& out, int depth) {
    std::string pad(depth * 2, ' ');
    std::string padInner((depth + 1) * 2, ' ');

    if (!node.children.empty()) {
        out += "{\r\n";
        for (size_t i = 0; i < node.children.size(); i++) {
            auto& [key, child] = node.children[i];
            out += padInner + "\"" + JsonEscape(key) + "\": ";
            if (child.isNull) {
                out += "null";
            } else if (!child.children.empty()) {
                NodeToJson(child, out, depth + 1);
            } else {
                out += "\"" + JsonEscape(child.value) + "\"";
            }
            if (i + 1 < node.children.size()) out += ",";
            out += "\r\n";
        }
        out += pad + "}";
    } else if (node.isNull) {
        out += "null";
    } else {
        out += "\"" + JsonEscape(node.value) + "\"";
    }
}

static std::string YamlAppSectionToJson(const std::string& yaml) {
    std::vector<std::string> lines;
    std::istringstream iss(yaml);
    std::string line;
    while (std::getline(iss, line)) {
        lines.push_back(line);
    }

    size_t idx = 0;
    YamlNode root = ParseYamlLines(lines, idx, -1);

    for (auto& [key, child] : root.children) {
        if (key == "app") {
            std::string json;
            NodeToJson(child, json, 0);
            json += "\r\n";
            return json;
        }
    }

    Wh_Log(L"TranslateSync: 'app' section not found in YAML");
    return {};
}

// ─── Find locales directory ─────────────────────────────────────────────────

static fs::path FindLocalesDir() {
    wchar_t exePath[MAX_PATH];
    GetModuleFileNameW(nullptr, exePath, MAX_PATH);
    fs::path exeDir = fs::path(exePath).parent_path();

    fs::path locales = exeDir / L"UI" / L"resources" / L"app" / L"extensions"
                               / L"windhawk" / L"webview" / L"locales";
    Wh_Log(L"TranslateSync: Checking: %s", locales.c_str());
    if (fs::exists(locales) && fs::is_directory(locales)) {
        return locales;
    }

    // Hardcoded fallback
    locales = L"C:\\Program Files\\Windhawk\\UI\\resources\\app\\extensions\\windhawk\\webview\\locales";
    Wh_Log(L"TranslateSync: Checking fallback: %s", locales.c_str());
    if (fs::exists(locales) && fs::is_directory(locales)) {
        return locales;
    }

    Wh_Log(L"TranslateSync: Locales directory not found!");
    return {};
}

// ─── Get list of languages to sync ──────────────────────────────────────────

static std::vector<std::string> GetLanguagesToSync(const fs::path& localesDir) {
    std::vector<std::string> langs;

    auto setting = Wh_GetStringSetting(L"Languages");
    std::wstring langSetting(setting ? setting : L"");
    Wh_FreeStringSetting(setting);

    if (!langSetting.empty()) {
        std::wstringstream wss(langSetting);
        std::wstring token;
        while (std::getline(wss, token, L',')) {
            size_t start = token.find_first_not_of(L" \t");
            size_t end = token.find_last_not_of(L" \t");
            if (start != std::wstring::npos) {
                std::wstring trimmed = token.substr(start, end - start + 1);
                std::string lang = WideToUtf8(trimmed);
                if (!lang.empty() && lang != "en") {
                    langs.push_back(lang);
                }
            }
        }
    } else {
        std::error_code ec;
        for (auto& entry : fs::directory_iterator(localesDir, ec)) {
            if (!entry.is_directory()) continue;
            std::string name = entry.path().filename().string();
            if (name == "en") continue;
            if (fs::exists(entry.path() / "translation.json")) {
                langs.push_back(name);
            }
        }
        std::sort(langs.begin(), langs.end());
    }

    return langs;
}

// ─── Backup / Restore ───────────────────────────────────────────────────────

static const wchar_t* BACKUP_SUFFIX = L".wh-translate-sync.bak";

static void BackupFile(const fs::path& filePath) {
    fs::path bakPath = filePath;
    bakPath += BACKUP_SUFFIX;
    if (!fs::exists(bakPath)) {
        std::error_code ec;
        fs::copy_file(filePath, bakPath, fs::copy_options::overwrite_existing, ec);
        if (!ec) {
            Wh_Log(L"TranslateSync: Backed up: %s", filePath.c_str());
        }
    }
}

static void RestoreBackups(const fs::path& localesDir) {
    if (localesDir.empty() || !fs::exists(localesDir)) return;

    std::error_code ec;
    int restored = 0;
    for (auto& entry : fs::recursive_directory_iterator(localesDir, ec)) {
        if (!entry.is_regular_file()) continue;
        std::wstring name = entry.path().filename().wstring();
        if (name.find(BACKUP_SUFFIX) == std::wstring::npos) continue;

        std::wstring origStr = entry.path().wstring();
        size_t suffixPos = origStr.rfind(BACKUP_SUFFIX);
        if (suffixPos == std::wstring::npos) continue;
        fs::path origPath = origStr.substr(0, suffixPos);

        std::error_code ec2;
        fs::copy_file(entry.path(), origPath, fs::copy_options::overwrite_existing, ec2);
        if (!ec2) {
            fs::remove(entry.path(), ec2);
            restored++;
            Wh_Log(L"TranslateSync: Restored: %s", origPath.c_str());
        }
    }

    if (restored > 0) {
        Wh_Log(L"TranslateSync: %d file(s) restored.", restored);
    }
}

// ─── Main sync logic ────────────────────────────────────────────────────────

static void DoSync() {
    fs::path localesDir = FindLocalesDir();
    if (localesDir.empty()) {
        Wh_Log(L"TranslateSync: Locales directory not found. Cannot sync.");
        return;
    }

    Wh_Log(L"TranslateSync: Locales dir: %s", localesDir.c_str());

    auto langs = GetLanguagesToSync(localesDir);
    if (langs.empty()) {
        Wh_Log(L"TranslateSync: No languages to sync.");
        return;
    }

    Wh_Log(L"TranslateSync: %zu language(s) to sync.", langs.size());

    bool showNotify = Wh_GetIntSetting(L"ShowNotifications");
    std::vector<std::string> updatedLangs;

    for (auto& lang : langs) {
        if (g_stopThread) return;

        std::wstring yamlUrl = L"https://raw.githubusercontent.com/"
                               L"ramensoftware/windhawk-translate/main/"
                             + Utf8ToWide(lang) + L".yml";

        std::string yamlData;
        if (!DownloadUrl(yamlUrl, yamlData)) {
            Wh_Log(L"TranslateSync: Download failed: %s.yml",
                   Utf8ToWide(lang).c_str());
            continue;
        }

        Wh_Log(L"TranslateSync: Downloaded %s.yml (%zu bytes)",
               Utf8ToWide(lang).c_str(), yamlData.size());

        std::string newJson = YamlAppSectionToJson(yamlData);
        if (newJson.empty()) {
            Wh_Log(L"TranslateSync: Failed to convert YAML for %s",
                   Utf8ToWide(lang).c_str());
            continue;
        }

        fs::path langDir = localesDir / Utf8ToWide(lang);
        fs::path jsonFile = langDir / L"translation.json";

        if (!fs::exists(langDir)) {
            std::error_code ec;
            fs::create_directories(langDir, ec);
            if (ec) {
                Wh_Log(L"TranslateSync: Cannot create dir: %s",
                       langDir.c_str());
                continue;
            }
        }

        std::string existingJson;
        {
            std::lock_guard<std::mutex> lock(g_fileMutex);
            existingJson = ReadFileStr(jsonFile);
        }

        // Normalize for comparison
        std::string existNorm = existingJson;
        std::string newNorm = newJson;
        existNorm.erase(std::remove(existNorm.begin(), existNorm.end(), '\r'), existNorm.end());
        newNorm.erase(std::remove(newNorm.begin(), newNorm.end(), '\r'), newNorm.end());

        if (existNorm == newNorm) {
            Wh_Log(L"TranslateSync: %s — already up to date.",
                   Utf8ToWide(lang).c_str());
            continue;
        }

        {
            std::lock_guard<std::mutex> lock(g_fileMutex);
            if (fs::exists(jsonFile)) {
                BackupFile(jsonFile);
            }
            if (WriteFileStr(jsonFile, newJson)) {
                Wh_Log(L"TranslateSync: %s — UPDATED (%zu -> %zu bytes)",
                       Utf8ToWide(lang).c_str(), existingJson.size(), newJson.size());
                updatedLangs.push_back(lang);
            } else {
                Wh_Log(L"TranslateSync: %s — write FAILED!",
                       Utf8ToWide(lang).c_str());
            }
        }
    }

    if (!updatedLangs.empty() && showNotify) {
        std::wstring msg = L"Updated translations: ";
        for (size_t i = 0; i < updatedLangs.size(); i++) {
            if (i > 0) msg += L", ";
            msg += Utf8ToWide(updatedLangs[i]);
        }
        msg += L"\nRestart Windhawk UI to apply.";
        ShowBalloon(msg.c_str());
    }

    if (updatedLangs.empty()) {
        Wh_Log(L"TranslateSync: All translations already up to date.");
    } else {
        Wh_Log(L"TranslateSync: Sync complete. %zu language(s) updated.",
               updatedLangs.size());
    }
}

// ─── Sync thread ────────────────────────────────────────────────────────────

static DWORD WINAPI SyncThreadFunc(LPVOID) {
    Wh_Log(L"TranslateSync: Starting sync...");
    DoSync();

    int interval = Wh_GetIntSetting(L"UpdateIntervalHours");
    if (interval <= 0) {
        Wh_Log(L"TranslateSync: One-shot mode. Thread exiting.");
        return 0;
    }

    while (!g_stopThread) {
        DWORD waitMs = (DWORD)interval * 3600 * 1000;
        for (DWORD waited = 0; waited < waitMs && !g_stopThread; waited += 1000) {
            Sleep(1000);
        }
        if (g_stopThread) break;

        Wh_Log(L"TranslateSync: Periodic sync...");
        DoSync();
    }

    return 0;
}

// ─── Mod lifecycle ──────────────────────────────────────────────────────────

static DWORD WINAPI DelayedSyncThread(LPVOID) {
    Sleep(5000);
    if (!g_stopThread) {
        return SyncThreadFunc(nullptr);
    }
    return 0;
}

BOOL Wh_ModInit() {
    Wh_Log(L"TranslateSync v1.0: Initializing.");

    g_stopThread = false;

    g_syncThread = CreateThread(nullptr, 0, DelayedSyncThread, nullptr, 0, nullptr);

    Wh_Log(L"TranslateSync v1.0: Ready. Sync starts in 5s.");
    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L"TranslateSync v1.0: Uninit — restoring backups.");

    g_stopThread = true;

    if (g_syncThread) {
        WaitForSingleObject(g_syncThread, 3000);
        CloseHandle(g_syncThread);
        g_syncThread = nullptr;
    }

    fs::path localesDir = FindLocalesDir();
    if (!localesDir.empty()) {
        std::lock_guard<std::mutex> lock(g_fileMutex);
        RestoreBackups(localesDir);
    }

    bool showNotify = Wh_GetIntSetting(L"ShowNotifications");
    if (showNotify) {
        ShowBalloon(L"Original translations restored.");
    }

    Wh_Log(L"TranslateSync v1.0: Done.");
}

void Wh_ModSettingsChanged() {
    Wh_Log(L"TranslateSync v1.0: Settings changed — will apply on next sync.");
}
