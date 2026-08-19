// ==WindhawkMod==
// @id              resource-redirect-oneui85
// @name            Ressource Redirect (One UI 8.5)
// @description     Redirige le chargement des ressources Windows (icônes, curseurs, bitmaps, chaînes, menus, boîtes de dialogue, images GDI+) vers des fichiers alternatifs, sans modifier les fichiers système. Mode « One UI 8.5 » intégré : thèmez Windows façon Samsung Galaxy, en toute sécurité.
// @version         1.0.0
// @author          Arielle Tempest
// @include         *
// @compilerOptions -lshlwapi -lshell32
// @options {
//   "oneUi85Mode": true,
//   "themePath": "",
//   "customRules": ""
// }
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Ressource Redirect – mode One UI 8.5

Réimplémentation indépendante du concept du mod « Resource Redirect » (m417z,
GPLv3), avec un **mode One UI 8.5 intégré**.

Le mod intercepte les appels de chargement de ressources Windows (icônes,
curseurs, bitmaps, chaînes, menus, boîtes de dialogue, images GDI+) et les
redirige vers des fichiers alternatifs, **sans jamais modifier les fichiers
système**. Désactiver le mod restaure l'état d'origine en un clic.

## Mode One UI 8.5

Par défaut, le mod s'attend à trouver un dossier de thème dans
`%LOCALAPPDATA%\Windhawk\OneUI85Theme` (modifiable via le réglage
`themePath`). Y déposer le contenu d'un pack de thème compatible (par exemple
le pack d'icônes « BANAANA OneUI » du dépôt resource-redirect-icon-themes) et
redémarrer Explorer suffit.

Le dossier du thème peut contenir :
- un fichier `theme.ini` avec une section `[redirections]`
  (`%SystemRoot%\System32\imageres.dll=imageres.dll`, ...) — compatible avec
  les packs existants ;
- et/ou des fichiers reprenant les noms des fichiers système à remplacer
  (`imageres.dll`, `shell32.dll`, ...), qui seront détectés automatiquement.

## Réglages

- `oneUi85Mode` : utilise le dossier One UI 8.5 par défaut quand `themePath`
  est vide.
- `themePath` : chemin du dossier de thème (laissez vide pour le dossier par
  défaut).
- `customRules` : règles supplémentaires, une par ligne, au format
  `fichier_original=fichier_alternatif` (chemins absolus ou relatifs au
  dossier de thème, jokers `*` autorisés).

## Avertissement

Les thèmes d'icônes sont fournis par la communauté (respectez leurs licences
et crédits). Ce mod est fourni en l'état ; utilisez-le à vos risques.
*/
// ==/WindhawkModReadme==

#include <windhawk_api.h>

#include <cstddef>  // nullptr_t (requis par windhawk_utils.h avec certains compilateurs)
#include <windows.h>
#include <windhawk_utils.h>
#include <shlwapi.h>  // PathMatchSpecW

#include <algorithm>
#include <cwctype>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>
#include <shared_mutex>

//
// Réglages
//

struct Settings {
    bool oneUi85Mode = true;
    std::wstring themePath;
    std::wstring customRules;
};

static Settings g_settings;

//
// Règles de redirection
//

struct RedirectionRule {
    std::wstring pattern;      // motif (chemin ou joker), variables d'env élargies
    std::wstring replacement;  // fichier alternatif, chemin absolu élargi
};

static std::shared_mutex g_mutex;

static std::vector<RedirectionRule> g_rules;
static std::wstring g_themePath;   // chemin élargi du dossier de thème (vide = aucun)
static bool g_mirrorMode = false;  // détection automatique des fichiers jumeaux

// Cache : chemin original -> fichier alternatif (chaîne vide = pas de redirection).
static std::unordered_map<std::wstring, std::wstring> g_pathCache;
// Cache : fichier alternatif -> module chargé en tant que données (ressources).
static std::unordered_map<std::wstring, HMODULE> g_moduleCache;

// Garde anti-récursion (une fonction hookée peut en appeler une autre hookée).
static thread_local bool t_inHook = false;

class HookGuard {
   public:
    HookGuard() {
        m_wasInHook = t_inHook;
        t_inHook = true;
    }

    ~HookGuard() { t_inHook = m_wasInHook; }

    bool WasAlreadyInHook() const { return m_wasInHook; }

   private:
    bool m_wasInHook;
};

//
// Petites fonctions utilitaires
//

static std::wstring Trim(const std::wstring& s) {
    size_t a = s.find_first_not_of(L" \t\r\n");
    if (a == std::wstring::npos) {
        return {};
    }
    size_t b = s.find_last_not_of(L" \t\r\n");
    return s.substr(a, b - a + 1);
}

static std::wstring ExpandEnv(const std::wstring& s) {
    if (s.find(L'%') == std::wstring::npos) {
        return s;
    }
    DWORD n = ExpandEnvironmentStringsW(s.c_str(), nullptr, 0);
    if (n == 0) {
        return s;
    }
    std::wstring out(n, L'\0');
    n = ExpandEnvironmentStringsW(s.c_str(), &out[0], n);
    if (n == 0) {
        return s;
    }
    out.resize(n - 1);  // retire le caractère nul final
    return out;
}

static bool IsAbsolutePath(const std::wstring& p) {
    if (p.size() >= 3 && p[1] == L':' && (p[2] == L'\\' || p[2] == L'/')) {
        return true;
    }
    if (p.size() >= 2 && (p[0] == L'\\' || p[0] == L'/')) {
        return true;
    }
    return false;
}

static bool FileExists(const std::wstring& p) {
    DWORD attr = GetFileAttributesW(p.c_str());
    return attr != INVALID_FILE_ATTRIBUTES && !(attr & FILE_ATTRIBUTE_DIRECTORY);
}

static bool EqualsIgnoreCase(const std::wstring& a, const std::wstring& b) {
    if (a.size() != b.size()) {
        return false;
    }
    for (size_t i = 0; i < a.size(); i++) {
        if (towlower(a[i]) != towlower(b[i])) {
            return false;
        }
    }
    return true;
}

static bool StartsWithIgnoreCase(const std::wstring& s,
                                 const std::wstring& prefix) {
    if (s.size() < prefix.size()) {
        return false;
    }
    for (size_t i = 0; i < prefix.size(); i++) {
        if (towlower(s[i]) != towlower(prefix[i])) {
            return false;
        }
    }
    return true;
}

static std::wstring BaseName(const std::wstring& p) {
    size_t pos = p.find_last_of(L"\\/");
    return pos == std::wstring::npos ? p : p.substr(pos + 1);
}

// Chemin relatif au dossier Windows (ex. « System32\imageres.dll »), ou vide.
static std::wstring MakeRelativeToWindowsDir(const std::wstring& p) {
    static std::wstring winDir = [] {
        WCHAR buf[MAX_PATH];
        GetWindowsDirectoryW(buf, MAX_PATH);
        return std::wstring(buf);
    }();
    if (winDir.empty() || p.size() <= winDir.size() ||
        !StartsWithIgnoreCase(p, winDir) ||
        (p[winDir.size()] != L'\\' && p[winDir.size()] != L'/')) {
        return {};
    }
    return p.substr(winDir.size() + 1);
}

static std::wstring GetDefaultThemePath() {
    WCHAR buf[MAX_PATH];
    if (GetEnvironmentVariableW(L"LOCALAPPDATA", buf, MAX_PATH) != 0) {
        return std::wstring(buf) + L"\\Windhawk\\OneUI85Theme";
    }
    return {};
}

//
// Chargement des règles
//

static void AddRule(const std::wstring& key, const std::wstring& value) {
    std::wstring pattern = ExpandEnv(Trim(key));
    std::wstring replacement = ExpandEnv(Trim(value));
    if (pattern.empty() || replacement.empty()) {
        return;
    }
    if (!IsAbsolutePath(replacement) && !g_themePath.empty()) {
        replacement = g_themePath + L"\\" + replacement;
    }
    g_rules.push_back({std::move(pattern), std::move(replacement)});
}

static void ParseRuleLines(const std::wstring& text) {
    size_t start = 0;
    while (start <= text.size()) {
        size_t end = text.find(L'\n', start);
        if (end == std::wstring::npos) {
            end = text.size();
        }
        std::wstring line = Trim(text.substr(start, end - start));
        start = end + 1;
        if (line.empty() || line[0] == L'#' || line[0] == L';' ||
            line[0] == L'[') {
            continue;
        }
        size_t eq = line.find(L'=');
        if (eq == std::wstring::npos) {
            continue;
        }
        AddRule(line.substr(0, eq), line.substr(eq + 1));
    }
}

static void LoadThemeIni() {
    if (g_themePath.empty()) {
        return;
    }
    std::wstring ini = g_themePath + L"\\theme.ini";
    if (!FileExists(ini)) {
        return;
    }
    WCHAR buf[32768];
    DWORD n = GetPrivateProfileSectionW(L"redirections", buf, 32768, ini.c_str());
    for (WCHAR* p = buf; n > 0 && *p;) {
        size_t len = wcslen(p);
        std::wstring line(p, len);
        size_t eq = line.find(L'=');
        if (eq != std::wstring::npos) {
            AddRule(line.substr(0, eq), line.substr(eq + 1));
        }
        p += len + 1;
    }
}

static void RebuildRules() {
    std::unique_lock<std::shared_mutex> lock(g_mutex);
    g_rules.clear();
    g_pathCache.clear();

    // Dossier de thème effectif.
    std::wstring themePath = ExpandEnv(g_settings.themePath);
    bool themePathIsDefault = false;
    if (themePath.empty()) {
        std::wstring defaultPath = GetDefaultThemePath();
        if (g_settings.oneUi85Mode || FileExists(defaultPath)) {
            themePath = defaultPath;
            themePathIsDefault = true;
        }
    }
    g_themePath = themePath;
    // Le mode miroir (détection des fichiers jumeaux par nom) est actif dès
    // qu'un dossier de thème est configuré.
    g_mirrorMode = !g_themePath.empty();

    if (!g_themePath.empty()) {
        LoadThemeIni();
    }
    ParseRuleLines(g_settings.customRules);

    Wh_Log(L"Réglages chargés : oneUi85Mode=%d, themePath='%s'%s, %d règle(s)",
           g_settings.oneUi85Mode ? 1 : 0, g_themePath.c_str(),
           themePathIsDefault ? L" (par défaut)" : L"", (int)g_rules.size());
}

static void LoadSettings() {
    g_settings.oneUi85Mode = Wh_GetIntSetting(L"oneUi85Mode") != 0;
    {
        WindhawkUtils::StringSetting s(Wh_GetStringSetting(L"themePath"));
        g_settings.themePath = s.get() ? s.get() : L"";
    }
    {
        WindhawkUtils::StringSetting s(Wh_GetStringSetting(L"customRules"));
        g_settings.customRules = s.get() ? s.get() : L"";
    }
    RebuildRules();
}

//
// Recherche de la redirection
//

// À appeler avec le verrou partagé tenu (lecture des données globales).
static std::wstring ComputeReplacementLocked(const std::wstring& original) {
    if (original.empty()) {
        return {};
    }

    std::wstring orig = ExpandEnv(original);

    // Ne jamais rediriger des fichiers qui font déjà partie du thème.
    if (!g_themePath.empty() && StartsWithIgnoreCase(orig, g_themePath)) {
        return {};
    }

    // Règles explicites (theme.ini + réglages personnalisés).
    for (const auto& rule : g_rules) {
        if (PathMatchSpecW(orig.c_str(), rule.pattern.c_str())) {
            if (!EqualsIgnoreCase(orig, rule.replacement) &&
                FileExists(rule.replacement)) {
                return rule.replacement;
            }
        }
    }

    // Mode miroir « One UI 8.5 » : fichier du même nom dans le dossier du
    // thème, soit en miroir du dossier Windows, soit à la racine du thème.
    if (g_mirrorMode && !g_themePath.empty()) {
        std::wstring rel = MakeRelativeToWindowsDir(orig);
        if (!rel.empty()) {
            std::wstring candidate = g_themePath + L"\\" + rel;
            if (FileExists(candidate)) {
                return candidate;
            }
        }
        std::wstring base = BaseName(orig);
        if (!base.empty()) {
            std::wstring candidate = g_themePath + L"\\" + base;
            if (!EqualsIgnoreCase(orig, candidate) && FileExists(candidate)) {
                return candidate;
            }
        }
    }

    return {};
}

static std::wstring LookupReplacement(const std::wstring& original) {
    if (original.empty()) {
        return {};
    }

    std::wstring replacement;
    {
        std::shared_lock<std::shared_mutex> lock(g_mutex);
        auto it = g_pathCache.find(original);
        if (it != g_pathCache.end()) {
            return it->second;
        }
        replacement = ComputeReplacementLocked(original);
    }

    std::unique_lock<std::shared_mutex> lock(g_mutex);
    g_pathCache[original] = replacement;
    return replacement;
}

// Retourne un module chargé en tant que ressources (données) pour le module
// donné, ou nullptr s'il n'y a pas de redirection.
static HMODULE GetRedirectedModule(HMODULE hinst) {
    if (!hinst) {
        return nullptr;
    }

    WCHAR path[MAX_PATH];
    if (GetModuleFileNameW(hinst, path, MAX_PATH) == 0) {
        return nullptr;
    }

    std::wstring replacement = LookupReplacement(path);
    if (replacement.empty()) {
        return nullptr;
    }

    {
        std::shared_lock<std::shared_mutex> lock(g_mutex);
        auto it = g_moduleCache.find(replacement);
        if (it != g_moduleCache.end()) {
            return it->second;
        }
    }

    HMODULE h =
        LoadLibraryExW(replacement.c_str(), nullptr,
                       LOAD_LIBRARY_AS_DATAFILE | LOAD_LIBRARY_AS_IMAGE_RESOURCE);
    if (!h) {
        return nullptr;
    }

    std::unique_lock<std::shared_mutex> lock(g_mutex);
    g_moduleCache[replacement] = h;
    return h;
}

//
// Conversion d'index d'icônes (PrivateExtractIconsW)
//

static std::vector<WORD> EnumIconGroupIds(HMODULE hModule) {
    std::vector<WORD> ids;
    EnumResourceNamesW(
        hModule, RT_GROUP_ICON,
        [](HMODULE, LPCWSTR, LPWSTR name, LONG_PTR param) -> BOOL {
            auto* v = reinterpret_cast<std::vector<WORD>*>(param);
            if (IS_INTRESOURCE(name)) {
                v->push_back(static_cast<WORD>(
                    reinterpret_cast<ULONG_PTR>(name)));
            }
            return TRUE;
        },
        reinterpret_cast<LONG_PTR>(&ids));
    return ids;
}

// Les index d'icônes sont propres à chaque fichier : on convertit l'index du
// fichier d'origine en index du fichier alternatif via les identifiants de
// groupes d'icônes. Retourne false si la conversion est impossible.
static bool MapIconIndex(const std::wstring& origPath,
                         const std::wstring& replPath,
                         int* index) {
    if (!index || *index < 0) {
        return false;
    }

    HMODULE hOrig = LoadLibraryExW(origPath.c_str(), nullptr,
                                   LOAD_LIBRARY_AS_DATAFILE);
    HMODULE hRepl = LoadLibraryExW(replPath.c_str(), nullptr,
                                   LOAD_LIBRARY_AS_DATAFILE);
    if (!hOrig || !hRepl) {
        if (hOrig) {
            FreeLibrary(hOrig);
        }
        if (hRepl) {
            FreeLibrary(hRepl);
        }
        return false;
    }

    std::vector<WORD> origIds = EnumIconGroupIds(hOrig);
    std::vector<WORD> replIds = EnumIconGroupIds(hRepl);
    FreeLibrary(hOrig);
    FreeLibrary(hRepl);

    if (*index >= static_cast<int>(origIds.size())) {
        return false;
    }
    WORD target = origIds[*index];
    for (size_t i = 0; i < replIds.size(); i++) {
        if (replIds[i] == target) {
            *index = static_cast<int>(i);
            return true;
        }
    }
    return false;
}

//
// Signatures des fonctions hookées
//

typedef UINT(WINAPI* PrivateExtractIconsW_t)(LPCWSTR, int, int, int, HICON*,
                                             UINT*, UINT, UINT);
typedef HANDLE(WINAPI* LoadImageW_t)(HINSTANCE, LPCWSTR, UINT, int, int, UINT);
typedef HANDLE(WINAPI* LoadImageA_t)(HINSTANCE, LPCSTR, UINT, int, int, UINT);
typedef HICON(WINAPI* LoadIconW_t)(HINSTANCE, LPCWSTR);
typedef HICON(WINAPI* LoadIconA_t)(HINSTANCE, LPCSTR);
typedef HCURSOR(WINAPI* LoadCursorW_t)(HINSTANCE, LPCWSTR);
typedef HCURSOR(WINAPI* LoadCursorA_t)(HINSTANCE, LPCSTR);
typedef HBITMAP(WINAPI* LoadBitmapW_t)(HINSTANCE, LPCWSTR);
typedef HBITMAP(WINAPI* LoadBitmapA_t)(HINSTANCE, LPCSTR);
typedef HMENU(WINAPI* LoadMenuW_t)(HINSTANCE, LPCWSTR);
typedef HMENU(WINAPI* LoadMenuA_t)(HINSTANCE, LPCSTR);
typedef int(WINAPI* LoadStringW_t)(HINSTANCE, UINT, LPWSTR, int);
typedef int(WINAPI* LoadStringA_t)(HINSTANCE, UINT, LPSTR, int);
typedef INT_PTR(WINAPI* DialogBoxParamW_t)(HINSTANCE, LPCWSTR, HWND, DLGPROC,
                                           LPARAM);
typedef INT_PTR(WINAPI* DialogBoxParamA_t)(HINSTANCE, LPCSTR, HWND, DLGPROC,
                                           LPARAM);
typedef HWND(WINAPI* CreateDialogParamW_t)(HINSTANCE, LPCWSTR, HWND, DLGPROC,
                                           LPARAM);
typedef HWND(WINAPI* CreateDialogParamA_t)(HINSTANCE, LPCSTR, HWND, DLGPROC,
                                           LPARAM);

struct IStream;
typedef HRESULT(WINAPI* SHCreateStreamOnModuleResourceW_t)(HMODULE, LPCWSTR,
                                                           LPCWSTR, IStream**);

static PrivateExtractIconsW_t PrivateExtractIconsW_Original;
static LoadImageW_t LoadImageW_Original;
static LoadImageA_t LoadImageA_Original;
static LoadIconW_t LoadIconW_Original;
static LoadIconA_t LoadIconA_Original;
static LoadCursorW_t LoadCursorW_Original;
static LoadCursorA_t LoadCursorA_Original;
static LoadBitmapW_t LoadBitmapW_Original;
static LoadBitmapA_t LoadBitmapA_Original;
static LoadMenuW_t LoadMenuW_Original;
static LoadMenuA_t LoadMenuA_Original;
static LoadStringW_t LoadStringW_Original;
static LoadStringA_t LoadStringA_Original;
static DialogBoxParamW_t DialogBoxParamW_Original;
static DialogBoxParamA_t DialogBoxParamA_Original;
static CreateDialogParamW_t CreateDialogParamW_Original;
static CreateDialogParamA_t CreateDialogParamA_Original;
static SHCreateStreamOnModuleResourceW_t SHCreateStreamOnModuleResourceW_Original;

//
// Hooks
//

UINT WINAPI PrivateExtractIconsW_Hook(LPCWSTR szFileName,
                                      int nIconIndex,
                                      int cxIcon,
                                      int cyIcon,
                                      HICON* phicon,
                                      UINT* piconid,
                                      UINT nIcons,
                                      UINT flags) {
    HookGuard guard;
    if (guard.WasAlreadyInHook() || !szFileName) {
        return PrivateExtractIconsW_Original(szFileName, nIconIndex, cxIcon,
                                             cyIcon, phicon, piconid, nIcons,
                                             flags);
    }

    std::wstring replacement = LookupReplacement(szFileName);
    if (replacement.empty()) {
        return PrivateExtractIconsW_Original(szFileName, nIconIndex, cxIcon,
                                             cyIcon, phicon, piconid, nIcons,
                                             flags);
    }

    int index = nIconIndex;
    if (!MapIconIndex(szFileName, replacement, &index)) {
        // Impossible de convertir l'index : on laisse l'appel d'origine
        // s'exécuter (aucune redirection pour cette requête).
        return PrivateExtractIconsW_Original(szFileName, nIconIndex, cxIcon,
                                             cyIcon, phicon, piconid, nIcons,
                                             flags);
    }

    return PrivateExtractIconsW_Original(replacement.c_str(), index, cxIcon,
                                         cyIcon, phicon, piconid, nIcons,
                                         flags);
}

HANDLE WINAPI LoadImageW_Hook(HINSTANCE hinst,
                              LPCWSTR lpszName,
                              UINT uType,
                              int cx,
                              int cy,
                              UINT fuLoad) {
    HookGuard guard;
    if (guard.WasAlreadyInHook()) {
        return LoadImageW_Original(hinst, lpszName, uType, cx, cy, fuLoad);
    }

    if (uType == IMAGE_ICON || uType == IMAGE_CURSOR ||
        uType == IMAGE_BITMAP) {
        if (fuLoad & LR_LOADFROMFILE) {
            // Chargement depuis un fichier : on redirige le fichier.
            if (lpszName) {
                std::wstring replacement = LookupReplacement(lpszName);
                if (!replacement.empty()) {
                    HANDLE h = LoadImageW_Original(
                        nullptr, replacement.c_str(), uType, cx, cy, fuLoad);
                    if (h) {
                        return h;
                    }
                }
            }
        } else if (hinst && IS_INTRESOURCE(lpszName)) {
            // Chargement depuis les ressources d'un module.
            if (HMODULE redir = GetRedirectedModule(hinst)) {
                HANDLE h =
                    LoadImageW_Original(redir, lpszName, uType, cx, cy, fuLoad);
                if (h) {
                    return h;
                }
            }
        }
    }

    return LoadImageW_Original(hinst, lpszName, uType, cx, cy, fuLoad);
}

HANDLE WINAPI LoadImageA_Hook(HINSTANCE hinst,
                              LPCSTR lpszName,
                              UINT uType,
                              int cx,
                              int cy,
                              UINT fuLoad) {
    HookGuard guard;
    if (guard.WasAlreadyInHook()) {
        return LoadImageA_Original(hinst, lpszName, uType, cx, cy, fuLoad);
    }

    if (uType == IMAGE_ICON || uType == IMAGE_CURSOR ||
        uType == IMAGE_BITMAP) {
        if (fuLoad & LR_LOADFROMFILE) {
            if (lpszName) {
                // Conversion rapide ANSI -> Unicode pour la recherche.
                int len = MultiByteToWideChar(CP_ACP, 0, lpszName, -1, nullptr,
                                              0);
                if (len > 0) {
                    std::wstring name(len, L'\0');
                    MultiByteToWideChar(CP_ACP, 0, lpszName, -1, &name[0], len);
                    name.resize(len - 1);
                    std::wstring replacement = LookupReplacement(name);
                    if (!replacement.empty()) {
                        int len2 = WideCharToMultiByte(
                            CP_ACP, 0, replacement.c_str(), -1, nullptr, 0,
                            nullptr, nullptr);
                        std::string replA(len2 ? len2 - 1 : 0, '\0');
                        if (len2 > 1) {
                            WideCharToMultiByte(CP_ACP, 0, replacement.c_str(),
                                                -1, &replA[0], len2, nullptr,
                                                nullptr);
                        }
                        HANDLE h = LoadImageA_Original(nullptr, replA.c_str(),
                                                       uType, cx, cy, fuLoad);
                        if (h) {
                            return h;
                        }
                    }
                }
            }
        } else if (hinst && IS_INTRESOURCE(lpszName)) {
            if (HMODULE redir = GetRedirectedModule(hinst)) {
                HANDLE h =
                    LoadImageA_Original(redir, lpszName, uType, cx, cy, fuLoad);
                if (h) {
                    return h;
                }
            }
        }
    }

    return LoadImageA_Original(hinst, lpszName, uType, cx, cy, fuLoad);
}

HICON WINAPI LoadIconW_Hook(HINSTANCE hInstance, LPCWSTR lpIconName) {
    HookGuard guard;
    if (guard.WasAlreadyInHook()) {
        return LoadIconW_Original(hInstance, lpIconName);
    }

    if (hInstance && IS_INTRESOURCE(lpIconName)) {
        if (HMODULE redir = GetRedirectedModule(hInstance)) {
            if (HICON h = LoadIconW_Original(redir, lpIconName)) {
                return h;
            }
        }
    }

    return LoadIconW_Original(hInstance, lpIconName);
}

HICON WINAPI LoadIconA_Hook(HINSTANCE hInstance, LPCSTR lpIconName) {
    HookGuard guard;
    if (guard.WasAlreadyInHook()) {
        return LoadIconA_Original(hInstance, lpIconName);
    }

    if (hInstance && IS_INTRESOURCE(lpIconName)) {
        if (HMODULE redir = GetRedirectedModule(hInstance)) {
            if (HICON h = LoadIconA_Original(redir, lpIconName)) {
                return h;
            }
        }
    }

    return LoadIconA_Original(hInstance, lpIconName);
}

HCURSOR WINAPI LoadCursorW_Hook(HINSTANCE hInstance, LPCWSTR lpCursorName) {
    HookGuard guard;
    if (guard.WasAlreadyInHook()) {
        return LoadCursorW_Original(hInstance, lpCursorName);
    }

    if (hInstance && IS_INTRESOURCE(lpCursorName)) {
        if (HMODULE redir = GetRedirectedModule(hInstance)) {
            if (HCURSOR h = LoadCursorW_Original(redir, lpCursorName)) {
                return h;
            }
        }
    }

    return LoadCursorW_Original(hInstance, lpCursorName);
}

HCURSOR WINAPI LoadCursorA_Hook(HINSTANCE hInstance, LPCSTR lpCursorName) {
    HookGuard guard;
    if (guard.WasAlreadyInHook()) {
        return LoadCursorA_Original(hInstance, lpCursorName);
    }

    if (hInstance && IS_INTRESOURCE(lpCursorName)) {
        if (HMODULE redir = GetRedirectedModule(hInstance)) {
            if (HCURSOR h = LoadCursorA_Original(redir, lpCursorName)) {
                return h;
            }
        }
    }

    return LoadCursorA_Original(hInstance, lpCursorName);
}

HBITMAP WINAPI LoadBitmapW_Hook(HINSTANCE hInstance, LPCWSTR lpBitmapName) {
    HookGuard guard;
    if (guard.WasAlreadyInHook()) {
        return LoadBitmapW_Original(hInstance, lpBitmapName);
    }

    if (hInstance && IS_INTRESOURCE(lpBitmapName)) {
        if (HMODULE redir = GetRedirectedModule(hInstance)) {
            if (HBITMAP h = LoadBitmapW_Original(redir, lpBitmapName)) {
                return h;
            }
        }
    }

    return LoadBitmapW_Original(hInstance, lpBitmapName);
}

HBITMAP WINAPI LoadBitmapA_Hook(HINSTANCE hInstance, LPCSTR lpBitmapName) {
    HookGuard guard;
    if (guard.WasAlreadyInHook()) {
        return LoadBitmapA_Original(hInstance, lpBitmapName);
    }

    if (hInstance && IS_INTRESOURCE(lpBitmapName)) {
        if (HMODULE redir = GetRedirectedModule(hInstance)) {
            if (HBITMAP h = LoadBitmapA_Original(redir, lpBitmapName)) {
                return h;
            }
        }
    }

    return LoadBitmapA_Original(hInstance, lpBitmapName);
}

HMENU WINAPI LoadMenuW_Hook(HINSTANCE hInstance, LPCWSTR lpMenuName) {
    HookGuard guard;
    if (guard.WasAlreadyInHook()) {
        return LoadMenuW_Original(hInstance, lpMenuName);
    }

    if (hInstance && IS_INTRESOURCE(lpMenuName)) {
        if (HMODULE redir = GetRedirectedModule(hInstance)) {
            if (HMENU h = LoadMenuW_Original(redir, lpMenuName)) {
                return h;
            }
        }
    }

    return LoadMenuW_Original(hInstance, lpMenuName);
}

HMENU WINAPI LoadMenuA_Hook(HINSTANCE hInstance, LPCSTR lpMenuName) {
    HookGuard guard;
    if (guard.WasAlreadyInHook()) {
        return LoadMenuA_Original(hInstance, lpMenuName);
    }

    if (hInstance && IS_INTRESOURCE(lpMenuName)) {
        if (HMODULE redir = GetRedirectedModule(hInstance)) {
            if (HMENU h = LoadMenuA_Original(redir, lpMenuName)) {
                return h;
            }
        }
    }

    return LoadMenuA_Original(hInstance, lpMenuName);
}

int WINAPI LoadStringW_Hook(HINSTANCE hInstance,
                            UINT uID,
                            LPWSTR lpBuffer,
                            int cchBufferMax) {
    HookGuard guard;
    if (guard.WasAlreadyInHook()) {
        return LoadStringW_Original(hInstance, uID, lpBuffer, cchBufferMax);
    }

    if (hInstance) {
        if (HMODULE redir = GetRedirectedModule(hInstance)) {
            int n = LoadStringW_Original(redir, uID, lpBuffer, cchBufferMax);
            if (n > 0) {
                return n;
            }
        }
    }

    return LoadStringW_Original(hInstance, uID, lpBuffer, cchBufferMax);
}

int WINAPI LoadStringA_Hook(HINSTANCE hInstance,
                            UINT uID,
                            LPSTR lpBuffer,
                            int cchBufferMax) {
    HookGuard guard;
    if (guard.WasAlreadyInHook()) {
        return LoadStringA_Original(hInstance, uID, lpBuffer, cchBufferMax);
    }

    if (hInstance) {
        if (HMODULE redir = GetRedirectedModule(hInstance)) {
            int n = LoadStringA_Original(redir, uID, lpBuffer, cchBufferMax);
            if (n > 0) {
                return n;
            }
        }
    }

    return LoadStringA_Original(hInstance, uID, lpBuffer, cchBufferMax);
}

INT_PTR WINAPI DialogBoxParamW_Hook(HINSTANCE hInstance,
                                    LPCWSTR lpTemplateName,
                                    HWND hWndParent,
                                    DLGPROC lpDialogFunc,
                                    LPARAM dwInitParam) {
    HookGuard guard;
    if (guard.WasAlreadyInHook()) {
        return DialogBoxParamW_Original(hInstance, lpTemplateName, hWndParent,
                                        lpDialogFunc, dwInitParam);
    }

    if (hInstance && IS_INTRESOURCE(lpTemplateName)) {
        if (HMODULE redir = GetRedirectedModule(hInstance)) {
            INT_PTR r = DialogBoxParamW_Original(redir, lpTemplateName,
                                                 hWndParent, lpDialogFunc,
                                                 dwInitParam);
            if (r != -1) {
                return r;
            }
        }
    }

    return DialogBoxParamW_Original(hInstance, lpTemplateName, hWndParent,
                                    lpDialogFunc, dwInitParam);
}

INT_PTR WINAPI DialogBoxParamA_Hook(HINSTANCE hInstance,
                                    LPCSTR lpTemplateName,
                                    HWND hWndParent,
                                    DLGPROC lpDialogFunc,
                                    LPARAM dwInitParam) {
    HookGuard guard;
    if (guard.WasAlreadyInHook()) {
        return DialogBoxParamA_Original(hInstance, lpTemplateName, hWndParent,
                                        lpDialogFunc, dwInitParam);
    }

    if (hInstance && IS_INTRESOURCE(lpTemplateName)) {
        if (HMODULE redir = GetRedirectedModule(hInstance)) {
            INT_PTR r = DialogBoxParamA_Original(redir, lpTemplateName,
                                                 hWndParent, lpDialogFunc,
                                                 dwInitParam);
            if (r != -1) {
                return r;
            }
        }
    }

    return DialogBoxParamA_Original(hInstance, lpTemplateName, hWndParent,
                                    lpDialogFunc, dwInitParam);
}

HWND WINAPI CreateDialogParamW_Hook(HINSTANCE hInstance,
                                    LPCWSTR lpTemplateName,
                                    HWND hWndParent,
                                    DLGPROC lpDialogFunc,
                                    LPARAM dwInitParam) {
    HookGuard guard;
    if (guard.WasAlreadyInHook()) {
        return CreateDialogParamW_Original(hInstance, lpTemplateName,
                                           hWndParent, lpDialogFunc,
                                           dwInitParam);
    }

    if (hInstance && IS_INTRESOURCE(lpTemplateName)) {
        if (HMODULE redir = GetRedirectedModule(hInstance)) {
            HWND h = CreateDialogParamW_Original(redir, lpTemplateName,
                                                 hWndParent, lpDialogFunc,
                                                 dwInitParam);
            if (h) {
                return h;
            }
        }
    }

    return CreateDialogParamW_Original(hInstance, lpTemplateName, hWndParent,
                                       lpDialogFunc, dwInitParam);
}

HWND WINAPI CreateDialogParamA_Hook(HINSTANCE hInstance,
                                    LPCSTR lpTemplateName,
                                    HWND hWndParent,
                                    DLGPROC lpDialogFunc,
                                    LPARAM dwInitParam) {
    HookGuard guard;
    if (guard.WasAlreadyInHook()) {
        return CreateDialogParamA_Original(hInstance, lpTemplateName,
                                           hWndParent, lpDialogFunc,
                                           dwInitParam);
    }

    if (hInstance && IS_INTRESOURCE(lpTemplateName)) {
        if (HMODULE redir = GetRedirectedModule(hInstance)) {
            HWND h = CreateDialogParamA_Original(redir, lpTemplateName,
                                                 hWndParent, lpDialogFunc,
                                                 dwInitParam);
            if (h) {
                return h;
            }
        }
    }

    return CreateDialogParamA_Original(hInstance, lpTemplateName, hWndParent,
                                       lpDialogFunc, dwInitParam);
}

HRESULT WINAPI SHCreateStreamOnModuleResourceW_Hook(
    HMODULE hModule,
    LPCWSTR lpszName,
    LPCWSTR lpszType,
    IStream** ppStream) {
    HookGuard guard;
    if (guard.WasAlreadyInHook()) {
        return SHCreateStreamOnModuleResourceW_Original(hModule, lpszName,
                                                        lpszType, ppStream);
    }

    if (hModule) {
        if (HMODULE redir = GetRedirectedModule(hModule)) {
            HRESULT hr = SHCreateStreamOnModuleResourceW_Original(
                redir, lpszName, lpszType, ppStream);
            if (SUCCEEDED(hr)) {
                return hr;
            }
        }
    }

    return SHCreateStreamOnModuleResourceW_Original(hModule, lpszName, lpszType,
                                                    ppStream);
}

//
// Initialisation du mod
//

static BOOL HookApi(HMODULE module,
                    const char* name,
                    void* hook,
                    void** original) {
    void* target = reinterpret_cast<void*>(GetProcAddress(module, name));
    if (!target) {
        Wh_Log(L"Export introuvable : %hs — hook ignoré", name);
        return FALSE;
    }
    return Wh_SetFunctionHook(target, hook, original);
}

BOOL Wh_ModInit() {
    LoadSettings();

    HMODULE user32 = GetModuleHandleW(L"user32.dll");
    HMODULE shell32 = GetModuleHandleW(L"shell32.dll");

    HookApi(user32, "PrivateExtractIconsW", (void*)&PrivateExtractIconsW_Hook,
            (void**)&PrivateExtractIconsW_Original);
    HookApi(user32, "LoadImageW", (void*)&LoadImageW_Hook,
            (void**)&LoadImageW_Original);
    HookApi(user32, "LoadImageA", (void*)&LoadImageA_Hook,
            (void**)&LoadImageA_Original);
    HookApi(user32, "LoadIconW", (void*)&LoadIconW_Hook,
            (void**)&LoadIconW_Original);
    HookApi(user32, "LoadIconA", (void*)&LoadIconA_Hook,
            (void**)&LoadIconA_Original);
    HookApi(user32, "LoadCursorW", (void*)&LoadCursorW_Hook,
            (void**)&LoadCursorW_Original);
    HookApi(user32, "LoadCursorA", (void*)&LoadCursorA_Hook,
            (void**)&LoadCursorA_Original);
    HookApi(user32, "LoadBitmapW", (void*)&LoadBitmapW_Hook,
            (void**)&LoadBitmapW_Original);
    HookApi(user32, "LoadBitmapA", (void*)&LoadBitmapA_Hook,
            (void**)&LoadBitmapA_Original);
    HookApi(user32, "LoadMenuW", (void*)&LoadMenuW_Hook,
            (void**)&LoadMenuW_Original);
    HookApi(user32, "LoadMenuA", (void*)&LoadMenuA_Hook,
            (void**)&LoadMenuA_Original);
    HookApi(user32, "LoadStringW", (void*)&LoadStringW_Hook,
            (void**)&LoadStringW_Original);
    HookApi(user32, "LoadStringA", (void*)&LoadStringA_Hook,
            (void**)&LoadStringA_Original);
    HookApi(user32, "DialogBoxParamW", (void*)&DialogBoxParamW_Hook,
            (void**)&DialogBoxParamW_Original);
    HookApi(user32, "DialogBoxParamA", (void*)&DialogBoxParamA_Hook,
            (void**)&DialogBoxParamA_Original);
    HookApi(user32, "CreateDialogParamW", (void*)&CreateDialogParamW_Hook,
            (void**)&CreateDialogParamW_Original);
    HookApi(user32, "CreateDialogParamA", (void*)&CreateDialogParamA_Hook,
            (void**)&CreateDialogParamA_Original);
    HookApi(shell32, "SHCreateStreamOnModuleResourceW",
            (void*)&SHCreateStreamOnModuleResourceW_Hook,
            (void**)&SHCreateStreamOnModuleResourceW_Original);

    Wh_Log(L"Mod « Ressource Redirect (One UI 8.5) » initialisé");
    return TRUE;
}

void Wh_ModUninit() {
    std::unique_lock<std::shared_mutex> lock(g_mutex);
    for (auto& [path, h] : g_moduleCache) {
        if (h) {
            FreeLibrary(h);
        }
    }
    g_moduleCache.clear();
    g_pathCache.clear();
    g_rules.clear();
}

void Wh_ModSettingsChanged() {
    LoadSettings();
}

void Wh_ModAboutDialog(HWND hWnd) {
    MessageBoxW(
        hWnd,
        L"Ressource Redirect – mode One UI 8.5\n"
        L"Version 1.0.0\n\n"
        L"Réimplémentation indépendante du concept du mod « Resource Redirect » "
        L"de m417z (GPLv3).\n"
        L"Redirige le chargement des ressources Windows vers des fichiers "
        L"alternatifs, sans modifier les fichiers système.",
        L"À propos", MB_OK | MB_ICONINFORMATION);
}
