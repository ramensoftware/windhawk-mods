// ==WindhawkMod==
// @id              everything-explorer-search
// @name            Everything Explorer Search
// @description     Everything search inside File Explorer (native search box via UI Automation and a hotkey) and Win+S to EverythingToolbar
// @version         0.8.0
// @author          Frixed
// @include         explorer.exe
// @compilerOptions -lole32 -loleaut32 -lshell32 -lshlwapi -lcomctl32 -luuid -lpropsys -ladvapi32 -lgdi32 -ldwmapi -luser32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Everything Explorer Search

Replaces File Explorer search results with results from
[Everything](https://www.voidtools.com/) and, optionally, redirects Win+S to
EverythingToolbar.

## Requirements
- [Everything](https://www.voidtools.com/) running (app or service). If it
  doesn't respond, the search falls back to Windows Search with the same text
  and folder.
- [EverythingToolbar](https://github.com/srwi/EverythingToolbar) installed. It
  ships `Everything64.dll`, which this mod uses to query Everything. If you
  installed it somewhere else, update the DLL path in the settings.
- For Win+S: EverythingToolbar running, with the same global shortcut set in
  this mod.

## How it works
- **Native search box:** a UI Automation thread detects when focus enters the
  Explorer search box (an Edit whose Name or AutomationId contains one of the
  words in `searchBoxHints`), takes its text, clears it and opens the mod's own
  search box right on top of it, so Windows Search never starts.
- **Hotkey:** with an Explorer window in the foreground, the hotkey (Ctrl+F by
  default) opens the same search box.
- **Search as you type:** the tab shows results while you type. Enter
  confirms, Escape goes back to the original folder.
- **Classic search box:** if your Explorer still uses the Win32 search box,
  pressing Enter in it does the same (hooked when the window is created).
- The virtual folder (a shell folder registered in HKCU that lives inside this
  DLL) lists real files: context menu, drag and drop, icons and properties are
  delegated to the shell. It refreshes by itself when a result is deleted or
  renamed.
- The current folder is used as the search scope: `"C:\folder\" text`.
- **Win+S:** a low-level keyboard hook in the taskbar's explorer.exe detects
  Win+S (without Shift/Ctrl/Alt, so Win+Shift+S still takes screenshots),
  swallows it and sends the configured EverythingToolbar shortcut.

## Try the folder without the hotkey
With the mod enabled, type in the address bar:
`::{7E3C2A10-4B5D-4F0E-9A57-3D6E8B2C1F44}\text to search`

## Known limitations
- The classic search box is only hooked in windows opened AFTER enabling the
  mod.
- Searching again from inside the results folder uses the last valid scope.
- The results view shows the old Windows 7 style command bar (Organize,
  Include in library...) instead of the Windows 11 one. It stays in that
  window until you open another tab or window.
- After updating the mod, restart Windows Explorer so the results folder loads
  the new version.
- The index warning option writes `WholeFileSystem = 1` to
  `HKCU\Software\Microsoft\Windows\CurrentVersion\Explorer\Search\Preferences`
  and does not revert it when the mod is disabled.

## Support the project

If you enjoy using this mod and would like to support its development, you can make a donation through PayPal. Every contribution is appreciated!

[**Donate with PayPal**](https://www.paypal.com/donate?hosted_button_id=W7BHDHGZP6CHY)
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- everythingDll: '%LocalAppData%\Programs\EverythingToolbar\Everything64.dll'
  $name: Everything64.dll path
  $description: Everything SDK DLL that ships with EverythingToolbar
- maxResults: 1000
  $name: Maximum results
- interceptSearchBox: true
  $name: Replace the native search box (UI Automation)
- searchBoxHints: search,buscar
  $name: Search box keywords
  $description: Words that identify the search box in its Name or AutomationId, comma separated
- hotkey: ctrlF
  $name: Hotkey to open the Everything search
  $options:
  - ctrlF: Ctrl+F (replaces Explorer's)
  - ctrlShiftF: Ctrl+Shift+F
  - none: Disabled
- liveSearch: true
  $name: Show results as you type
- disableIndexWarning: true
  $name: Disable the Windows Search indexing warning
  $description: Writes WholeFileSystem = 1 to the user's search preferences
- winSEnabled: true
  $name: Win+S opens EverythingToolbar
- winSShortcut: Win+Alt+S
  $name: EverythingToolbar shortcut
  $description: Shortcut sent when you press Win+S. Must include Win and match EverythingToolbar's setting.
- winSRequiredProcess: ""
  $name: Required process for Win+S (optional)
  $description: If set and not running, Win+S opens the native search. Leave empty to always redirect.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <exdisp.h>
#include <knownfolders.h>
#include <propsys.h>
#include <propvarutil.h>
#include <commctrl.h>
#include <dwmapi.h>
#include <uiautomation.h>
#include <tlhelp32.h>

#include <atomic>
#include <cwctype>
#include <mutex>
#include <string>
#include <unordered_set>
#include <vector>

#ifndef SBSP_WRITENOHISTORY
#define SBSP_WRITENOHISTORY 0x08000000
#endif
#ifndef SFVM_WINDOWCREATED
#define SFVM_WINDOWCREATED 15
#endif

// ---------------------------------------------------------------------------
// Constantes y GUIDs (definidos acá para no depender de libuuid)
// ---------------------------------------------------------------------------

// {7E3C2A10-4B5D-4F0E-9A57-3D6E8B2C1F44}
static const CLSID CLSID_EvFolder = {0x7e3c2a10, 0x4b5d, 0x4f0e, {0x9a, 0x57, 0x3d, 0x6e, 0x8b, 0x2c, 0x1f, 0x44}};
static const wchar_t kClsidStr[] = L"{7E3C2A10-4B5D-4F0E-9A57-3D6E8B2C1F44}";

static const GUID kBHID_DataObject = {0xB8C0BD9F, 0xED24, 0x455C, {0x83, 0xE6, 0xD5, 0x39, 0x0C, 0x4F, 0xE8, 0xC4}};
static const GUID kBHID_SFUIObject = {0x3981E225, 0xF559, 0x11D3, {0x8E, 0x3A, 0x00, 0xC0, 0x4F, 0x68, 0x37, 0xD5}};
static const GUID kSID_STopLevelBrowser = {0x4C96BE40, 0x915C, 0x11CF, {0x99, 0xD3, 0x00, 0xAA, 0x00, 0x4A, 0xE8, 0x37}};
static const GUID kCLSID_ShellWindows = {0x9BA05972, 0xF6A8, 0x11CF, {0xA4, 0x42, 0x00, 0xA0, 0xC9, 0x0A, 0x8F, 0x39}};
static const GUID kIID_IShellFolderViewCB = {0x2047E320, 0xF2A9, 0x11CE, {0xAE, 0x65, 0x08, 0x00, 0x2B, 0x2E, 0x12, 0x62}};
static const GUID kFOLDERID_ComputerFolder = {0x0AC0837C, 0xBBF8, 0x452A, {0x85, 0x0D, 0x79, 0xD0, 0x8E, 0x66, 0x7C, 0xA7}};

#define MKKEY(a, b, c, d0, d1, d2, d3, d4, d5, d6, d7, pid) \
    {{a, b, c, {d0, d1, d2, d3, d4, d5, d6, d7}}, pid}
static const PROPERTYKEY kPkName   = MKKEY(0xB725F130, 0x47EF, 0x101A, 0xA5, 0xF1, 0x02, 0x60, 0x8C, 0x9E, 0xEB, 0xAC, 10);
static const PROPERTYKEY kPkType   = MKKEY(0xB725F130, 0x47EF, 0x101A, 0xA5, 0xF1, 0x02, 0x60, 0x8C, 0x9E, 0xEB, 0xAC, 4);
static const PROPERTYKEY kPkSize   = MKKEY(0xB725F130, 0x47EF, 0x101A, 0xA5, 0xF1, 0x02, 0x60, 0x8C, 0x9E, 0xEB, 0xAC, 12);
static const PROPERTYKEY kPkDate   = MKKEY(0xB725F130, 0x47EF, 0x101A, 0xA5, 0xF1, 0x02, 0x60, 0x8C, 0x9E, 0xEB, 0xAC, 14);
static const PROPERTYKEY kPkFolder = MKKEY(0xE3E0584C, 0xB788, 0x4A5A, 0xBB, 0x20, 0x7F, 0x5A, 0x44, 0xC9, 0xAC, 0xDD, 6);

// ---------------------------------------------------------------------------
// Ajustes
// ---------------------------------------------------------------------------

static std::wstring g_dllPath;
static DWORD g_maxResults = 1000;
enum Hotkey { HK_NONE, HK_CTRL_F, HK_CTRL_SHIFT_F };
static Hotkey g_hotkey = HK_CTRL_F;
static HMODULE g_self = nullptr;
static std::atomic<bool> g_intercept{true};
static std::atomic<bool> g_live{true};
static bool g_disableIndexWarning = true;
static std::mutex g_hintMutex;
static std::vector<std::wstring> g_hints;

// Win+S -> EverythingToolbar
struct WinSConfig {
    bool enabled = false;
    bool valid = false;
    std::vector<WORD> keys;  // sin Win: modificadores primero, tecla principal al final
    std::wstring requiredProcess;
};
static WinSConfig g_winS;
static std::mutex g_winSMutex;

static std::wstring Trim(const std::wstring& s) {
    size_t a = s.find_first_not_of(L" \t\r\n");
    if (a == std::wstring::npos) return L"";
    size_t b = s.find_last_not_of(L" \t\r\n");
    return s.substr(a, b - a + 1);
}

// "Win+Alt+S", "Win+Ctrl+Space", "Win+Alt+F9"... Tiene que incluir Win.
static bool ParseShortcut(const std::wstring& text, std::vector<WORD>& keys) {
    bool hasWin = false;
    std::vector<WORD> mods;
    WORD mainKey = 0;
    size_t start = 0;
    while (start <= text.size()) {
        size_t end = text.find(L'+', start);
        if (end == std::wstring::npos) end = text.size();
        std::wstring tok = Trim(text.substr(start, end - start));
        for (auto& c : tok) c = (wchar_t)towlower(c);
        if (tok.empty()) return false;
        if (tok == L"win") {
            hasWin = true;
        } else if (tok == L"ctrl") {
            mods.push_back(VK_CONTROL);
        } else if (tok == L"alt") {
            mods.push_back(VK_MENU);
        } else if (tok == L"shift") {
            mods.push_back(VK_SHIFT);
        } else if (tok == L"space") {
            mainKey = VK_SPACE;
        } else if (tok.size() == 1 && iswalnum(tok[0])) {
            mainKey = (WORD)towupper(tok[0]);  // los VK de A-Z / 0-9 coinciden con ASCII
        } else if (tok.size() >= 2 && tok[0] == L'f') {
            int n = _wtoi(tok.c_str() + 1);
            if (n < 1 || n > 24) return false;
            mainKey = (WORD)(VK_F1 + n - 1);
        } else {
            return false;
        }
        start = end + 1;
    }
    if (!hasWin || !mainKey) return false;
    keys = mods;
    keys.push_back(mainKey);
    return true;
}

static void LoadSettings() {
    PCWSTR p = Wh_GetStringSetting(L"everythingDll");
    // La ruta admite variables de entorno (%LocalAppData%, etc.).
    WCHAR expanded[MAX_PATH] = {};
    DWORD n = ExpandEnvironmentStringsW(p ? p : L"", expanded, ARRAYSIZE(expanded));
    g_dllPath = (n && n <= ARRAYSIZE(expanded)) ? expanded : (p ? p : L"");
    Wh_FreeStringSetting(p);
    int m = Wh_GetIntSetting(L"maxResults");
    g_maxResults = m > 0 ? (DWORD)m : 1000;
    PCWSTR hk = Wh_GetStringSetting(L"hotkey");
    g_hotkey = HK_CTRL_F;
    if (hk && !wcscmp(hk, L"ctrlShiftF")) g_hotkey = HK_CTRL_SHIFT_F;
    else if (hk && !wcscmp(hk, L"none")) g_hotkey = HK_NONE;
    Wh_FreeStringSetting(hk);

    g_intercept = Wh_GetIntSetting(L"interceptSearchBox") != 0;
    g_live = Wh_GetIntSetting(L"liveSearch") != 0;
    g_disableIndexWarning = Wh_GetIntSetting(L"disableIndexWarning") != 0;

    PCWSTR hs = Wh_GetStringSetting(L"searchBoxHints");
    std::vector<std::wstring> hints;
    std::wstring cur;
    for (const wchar_t* c = hs ? hs : L""; ; c++) {
        if (*c == L',' || *c == 0) {
            std::wstring t = Trim(cur);
            if (!t.empty()) {
                for (auto& ch : t) ch = (wchar_t)towlower(ch);
                hints.push_back(t);
            }
            cur.clear();
            if (!*c) break;
        } else {
            cur += *c;
        }
    }
    Wh_FreeStringSetting(hs);
    {
        std::lock_guard<std::mutex> lk(g_hintMutex);
        g_hints = hints;
    }

    WinSConfig ws;
    ws.enabled = Wh_GetIntSetting(L"winSEnabled") != 0;
    PCWSTR sc = Wh_GetStringSetting(L"winSShortcut");
    std::wstring shortcut = sc ? sc : L"";
    Wh_FreeStringSetting(sc);
    PCWSTR rp = Wh_GetStringSetting(L"winSRequiredProcess");
    ws.requiredProcess = rp ? Trim(rp) : L"";
    Wh_FreeStringSetting(rp);
    ws.valid = ParseShortcut(shortcut, ws.keys);
    std::lock_guard<std::mutex> lk(g_winSMutex);
    g_winS = ws;
}

// Evita el aviso del Explorador cuando el indexado de Windows Search está desactivado.
static void DisableIndexWarning() {
    DWORD one = 1;
    RegSetKeyValueW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Search\\Preferences",
                    L"WholeFileSystem", REG_DWORD, &one, sizeof(one));
}

// ---------------------------------------------------------------------------
// Everything SDK (Everything64.dll), cargado dinámicamente
// ---------------------------------------------------------------------------

struct EvApi {
    HMODULE h = nullptr;
    void(WINAPI* SetSearchW)(LPCWSTR) = nullptr;
    void(WINAPI* SetMax)(DWORD) = nullptr;
    BOOL(WINAPI* QueryW)(BOOL) = nullptr;
    DWORD(WINAPI* GetNumResults)() = nullptr;
    DWORD(WINAPI* GetResultFullPathNameW)(DWORD, LPWSTR, DWORD) = nullptr;
    DWORD(WINAPI* GetMajorVersion)() = nullptr;
    void(WINAPI* Reset)() = nullptr;
};
static EvApi g_ev;
static std::mutex g_evMutex;  // el SDK2 usa estado global: serializar todo

static bool LoadEverything() {
    std::lock_guard<std::mutex> lk(g_evMutex);
    if (g_ev.h) {
        FreeLibrary(g_ev.h);
        g_ev = EvApi{};
    }
    g_ev.h = LoadLibraryW(g_dllPath.c_str());
    if (!g_ev.h) return false;
#define LOADFN(n) g_ev.n = reinterpret_cast<decltype(g_ev.n)>(GetProcAddress(g_ev.h, "Everything_" #n))
    LOADFN(SetSearchW);
    LOADFN(SetMax);
    LOADFN(QueryW);
    LOADFN(GetNumResults);
    LOADFN(GetResultFullPathNameW);
    LOADFN(GetMajorVersion);
    LOADFN(Reset);
#undef LOADFN
    return g_ev.SetSearchW && g_ev.SetMax && g_ev.QueryW && g_ev.GetNumResults && g_ev.GetResultFullPathNameW;
}

// Everything tiene que estar corriendo para que el IPC responda.
static bool EvAvailable() {
    std::lock_guard<std::mutex> lk(g_evMutex);
    if (!g_ev.h || !g_ev.QueryW) return false;
    if (g_ev.GetMajorVersion) return g_ev.GetMajorVersion() > 0;
    return true;
}

// ---------------------------------------------------------------------------
// PIDLs propios
//   SIG_QUERY: hijo de la raíz; texto = consulta completa de Everything
//   SIG_FILE : hijo de una carpeta-consulta; datos = PIDL absoluto real
// ---------------------------------------------------------------------------

#pragma pack(push, 1)
struct EvId {
    USHORT cb;
    USHORT sig;
};
#pragma pack(pop)

static const USHORT SIG_QUERY = 0x5145;
static const USHORT SIG_FILE = 0x4546;

static PITEMID_CHILD MakeItem(USHORT sig, const void* data, UINT bytes) {
    UINT cb = sizeof(EvId) + bytes;
    if (cb > 0xFFFF - sizeof(USHORT)) return nullptr;
    BYTE* p = (BYTE*)CoTaskMemAlloc(cb + sizeof(USHORT));
    if (!p) return nullptr;
    EvId* id = (EvId*)p;
    id->cb = (USHORT)cb;
    id->sig = sig;
    memcpy(p + sizeof(EvId), data, bytes);
    *(USHORT*)(p + cb) = 0;
    return (PITEMID_CHILD)p;
}

// Item de resultado: PIDL absoluto real seguido de la ruta (para ordenar por nombre
// y carpeta sin pedirle nada al shell).
static PITEMID_CHILD MakeFileItem(PCIDLIST_ABSOLUTE abs, const wchar_t* path) {
    UINT pidlBytes = ILGetSize(abs);
    UINT pathBytes = (UINT)((wcslen(path) + 1) * sizeof(WCHAR));
    std::vector<BYTE> data(pidlBytes + pathBytes);
    memcpy(data.data(), abs, pidlBytes);
    memcpy(data.data() + pidlBytes, path, pathBytes);
    return MakeItem(SIG_FILE, data.data(), (UINT)data.size());
}

static PITEMID_CHILD MakeQueryItem(const std::wstring& q) {
    return MakeItem(SIG_QUERY, q.c_str(), (UINT)((q.size() + 1) * sizeof(WCHAR)));
}

static const EvId* AsEv(const ITEMIDLIST* pidl, USHORT sig) {
    const EvId* id = (const EvId*)pidl;
    return (id && id->cb >= sizeof(EvId) && id->sig == sig) ? id : nullptr;
}

static std::wstring QueryText(const EvId* id) {
    return std::wstring((const WCHAR*)((const BYTE*)id + sizeof(EvId)));
}

static PCIDLIST_ABSOLUTE RealPidl(const EvId* id) {
    return (PCIDLIST_ABSOLUTE)((const BYTE*)id + sizeof(EvId));
}

// Ruta guardada detrás del PIDL real; nullptr en items de versiones anteriores.
static const wchar_t* ItemPath(const EvId* id) {
    UINT used = sizeof(EvId) + ILGetSize(RealPidl(id));
    if (id->cb < used + 2 * sizeof(WCHAR)) return nullptr;
    const wchar_t* p = (const wchar_t*)((const BYTE*)id + used);
    return p[(id->cb - used) / sizeof(WCHAR) - 1] == 0 ? p : nullptr;
}

// Resuelve el item real detrás de un item SIG_FILE.
struct Real {
    IShellFolder* parent = nullptr;
    PCUITEMID_CHILD child = nullptr;
    PCIDLIST_ABSOLUTE abs = nullptr;
    ~Real() {
        if (parent) parent->Release();
    }
};

static HRESULT BindReal(PCUITEMID_CHILD pidl, Real& r) {
    const EvId* id = AsEv(pidl, SIG_FILE);
    if (!id) return E_INVALIDARG;
    r.abs = RealPidl(id);
    return SHBindToParent(r.abs, IID_PPV_ARGS(&r.parent), &r.child);
}

static HRESULT GetProp(PCUITEMID_CHILD pidl, const PROPERTYKEY& key, PROPVARIANT* pv) {
    const EvId* id = AsEv(pidl, SIG_FILE);
    if (!id) return E_INVALIDARG;
    IShellItem2* si = nullptr;
    HRESULT hr = SHCreateItemFromIDList(RealPidl(id), IID_PPV_ARGS(&si));
    if (FAILED(hr)) return hr;
    hr = si->GetProperty(key, pv);
    si->Release();
    return hr;
}

static HRESULT StringToStrRet(PCWSTR s, STRRET* psr) {
    psr->uType = STRRET_WSTR;
    return SHStrDupW(s, &psr->pOleStr);
}

static HRESULT Cmp(int r) {
    return MAKE_HRESULT(SEVERITY_SUCCESS, 0, (USHORT)(short)(r < 0 ? -1 : (r > 0 ? 1 : 0)));
}

static std::wstring Lower(std::wstring s) {
    if (!s.empty()) CharLowerBuffW(&s[0], (DWORD)s.size());
    return s;
}

// "C:\carpeta\" -> "C:\carpeta"; las raíces de unidad ("C:\") quedan igual.
static std::wstring StripSlash(std::wstring s) {
    while (s.size() > 3 && s.back() == L'\\') s.pop_back();
    return s;
}

// La consulta guardada tiene la forma `"C:\carpeta\" texto` (o solo `texto` sin alcance).
static void SplitQuery(const std::wstring& full, std::wstring& scope, std::wstring& text) {
    scope.clear();
    text = full;
    if (full.size() > 2 && full[0] == L'"') {
        size_t e = full.find(L"\" ", 1);
        if (e != std::wstring::npos) {
            scope = full.substr(1, e - 1);
            text = full.substr(e + 2);
        }
    }
}

// Nombre que el shell muestra para la carpeta (localizado: "Descargas", "Este equipo"...).
static std::wstring FolderLabel(const std::wstring& path) {
    static std::mutex m;
    static std::wstring lastPath, lastLabel;
    {
        std::lock_guard<std::mutex> lk(m);
        if (path == lastPath) return lastLabel;
    }
    std::wstring clean = StripSlash(path);
    std::wstring label;
    PIDLIST_ABSOLUTE p = nullptr;
    if (SUCCEEDED(SHParseDisplayName(clean.c_str(), nullptr, &p, 0, nullptr)) && p) {
        PWSTR s = nullptr;
        if (SUCCEEDED(SHGetNameFromIDList(p, SIGDN_NORMALDISPLAY, &s))) {
            label = s;
            CoTaskMemFree(s);
        }
        ILFree(p);
    }
    if (label.empty()) label = PathFindFileNameW(clean.c_str());
    std::lock_guard<std::mutex> lk(m);
    lastPath = path;
    lastLabel = label;
    return label;
}

// Título de la pestaña: "texto — en Descargas".
static std::wstring SearchTitle(const std::wstring& text, const std::wstring& scope) {
    return scope.empty() ? text : text + L" \u2014 en " + FolderLabel(scope);
}

// ---------------------------------------------------------------------------
// Columnas
// ---------------------------------------------------------------------------

struct Col {
    const wchar_t* name;
    const PROPERTYKEY* key;
    int cx;
    SHCOLSTATEF state;
};
static const Col kCols[] = {
    {L"Nombre", &kPkName, 32, SHCOLSTATE_TYPE_STR | SHCOLSTATE_ONBYDEFAULT},
    {L"Carpeta", &kPkFolder, 44, SHCOLSTATE_TYPE_STR | SHCOLSTATE_ONBYDEFAULT},
    {L"Tipo", &kPkType, 20, SHCOLSTATE_TYPE_STR | SHCOLSTATE_ONBYDEFAULT},
    {L"Tamaño", &kPkSize, 12, SHCOLSTATE_TYPE_INT | SHCOLSTATE_ONBYDEFAULT},
    {L"Fecha de modificación", &kPkDate, 22, SHCOLSTATE_TYPE_DATE | SHCOLSTATE_ONBYDEFAULT},
};

// ---------------------------------------------------------------------------
// IEnumIDList sobre un vector
// ---------------------------------------------------------------------------

class EvEnum : public IEnumIDList {
    LONG m_ref = 1;
    std::vector<PITEMID_CHILD> m_items;
    size_t m_pos = 0;

public:
    explicit EvEnum(std::vector<PITEMID_CHILD>&& v) : m_items(std::move(v)) {}
    virtual ~EvEnum() {
        for (auto p : m_items) CoTaskMemFree(p);
    }
    STDMETHODIMP QueryInterface(REFIID riid, void** ppv) override {
        if (riid == IID_IUnknown || riid == __uuidof(IEnumIDList)) {
            *ppv = static_cast<IEnumIDList*>(this);
            AddRef();
            return S_OK;
        }
        *ppv = nullptr;
        return E_NOINTERFACE;
    }
    STDMETHODIMP_(ULONG) AddRef() override { return InterlockedIncrement(&m_ref); }
    STDMETHODIMP_(ULONG) Release() override {
        ULONG r = InterlockedDecrement(&m_ref);
        if (!r) delete this;
        return r;
    }
    STDMETHODIMP Next(ULONG celt, PITEMID_CHILD* rgelt, ULONG* pf) override {
        ULONG n = 0;
        while (n < celt && m_pos < m_items.size()) rgelt[n++] = (PITEMID_CHILD)ILClone(m_items[m_pos++]);
        if (pf) *pf = n;
        return n == celt ? S_OK : S_FALSE;
    }
    STDMETHODIMP Skip(ULONG c) override {
        m_pos = (m_pos + c > m_items.size()) ? m_items.size() : m_pos + c;
        return m_pos < m_items.size() ? S_OK : S_FALSE;
    }
    STDMETHODIMP Reset() override {
        m_pos = 0;
        return S_OK;
    }
    STDMETHODIMP Clone(IEnumIDList**) override { return E_NOTIMPL; }
};

// ---------------------------------------------------------------------------
// La carpeta virtual
//   Raíz (::{CLSID}): vacía; ParseDisplayName(texto) -> item SIG_QUERY
//   BindToObject(SIG_QUERY) -> carpeta-consulta, que lista resultados de Everything
// ---------------------------------------------------------------------------

class EvFolder : public IShellFolder2, public IPersistFolder2 {
    LONG m_ref = 1;
    PIDLIST_ABSOLUTE m_pidl = nullptr;
    std::wstring m_query;
    bool m_isQuery = false;
    // Rutas (en minúsculas) de la última enumeración, para saber si un aviso
    // de cambio del shell afecta a algún resultado. EnumObjects puede correr
    // en un hilo de fondo de la vista.
    std::mutex m_resMutex;
    std::unordered_set<std::wstring> m_paths;

public:
    EvFolder() {}
    virtual ~EvFolder() {
        if (m_pidl) ILFree(m_pidl);
    }

    const std::wstring& Query() const { return m_query; }

    // folder = true: también coincide si `lower` es una carpeta que contiene algún resultado.
    bool Affects(const std::wstring& lower, bool folder) {
        std::lock_guard<std::mutex> lk(m_resMutex);
        if (m_paths.count(lower)) return true;
        if (!folder) return false;
        std::wstring prefix = lower + L"\\";
        for (auto& p : m_paths)
            if (!p.compare(0, prefix.size(), prefix)) return true;
        return false;
    }

    // IUnknown
    STDMETHODIMP QueryInterface(REFIID riid, void** ppv) override {
        *ppv = nullptr;
        if (riid == IID_IUnknown || riid == __uuidof(IShellFolder) || riid == __uuidof(IShellFolder2))
            *ppv = static_cast<IShellFolder2*>(this);
        else if (riid == __uuidof(IPersist) || riid == __uuidof(IPersistFolder) || riid == __uuidof(IPersistFolder2))
            *ppv = static_cast<IPersistFolder2*>(this);
        else
            return E_NOINTERFACE;
        AddRef();
        return S_OK;
    }
    STDMETHODIMP_(ULONG) AddRef() override { return InterlockedIncrement(&m_ref); }
    STDMETHODIMP_(ULONG) Release() override {
        ULONG r = InterlockedDecrement(&m_ref);
        if (!r) delete this;
        return r;
    }

    // IPersist / IPersistFolder / IPersistFolder2
    STDMETHODIMP GetClassID(CLSID* p) override {
        *p = CLSID_EvFolder;
        return S_OK;
    }
    STDMETHODIMP Initialize(PCIDLIST_ABSOLUTE pidl) override {
        if (m_pidl) ILFree(m_pidl);
        m_pidl = ILCloneFull(pidl);
        return m_pidl ? S_OK : E_OUTOFMEMORY;
    }
    STDMETHODIMP GetCurFolder(PIDLIST_ABSOLUTE* ppidl) override {
        *ppidl = m_pidl ? ILCloneFull(m_pidl) : nullptr;
        return *ppidl ? S_OK : E_FAIL;
    }

    // IShellFolder
    STDMETHODIMP ParseDisplayName(HWND, IBindCtx*, LPWSTR name, ULONG* eaten, PIDLIST_RELATIVE* ppidl, ULONG* attr) override {
        *ppidl = nullptr;
        if (m_isQuery || !name || !*name) return E_INVALIDARG;
        *ppidl = (PIDLIST_RELATIVE)MakeQueryItem(name);
        if (eaten) *eaten = (ULONG)wcslen(name);
        if (attr) *attr &= (SFGAO_FOLDER | SFGAO_BROWSABLE);
        return *ppidl ? S_OK : E_OUTOFMEMORY;
    }

    STDMETHODIMP EnumObjects(HWND, SHCONTF, IEnumIDList** ppenum) override {
        std::vector<PITEMID_CHILD> items;
        std::unordered_set<std::wstring> paths;
        std::vector<std::wstring> found;
        if (m_isQuery && !m_query.empty()) {
            // Bajo el lock solo la consulta IPC: armar los PIDLs toca el disco y
            // no debe bloquear a las demás ventanas que consultan Everything.
            std::lock_guard<std::mutex> lk(g_evMutex);
            if (g_ev.h && g_ev.QueryW) {
                g_ev.SetSearchW(m_query.c_str());
                g_ev.SetMax(g_maxResults);
                if (g_ev.QueryW(TRUE)) {
                    DWORD n = g_ev.GetNumResults();
                    found.reserve(n);
                    std::vector<WCHAR> buf(32768);
                    for (DWORD i = 0; i < n; i++)
                        if (g_ev.GetResultFullPathNameW(i, buf.data(), (DWORD)buf.size())) found.emplace_back(buf.data());
                }
                if (g_ev.Reset) g_ev.Reset();
            }
        }
        items.reserve(found.size());
        for (auto& path : found) {
            PIDLIST_ABSOLUTE abs = ILCreateFromPathW(path.c_str());
            if (!abs) continue;  // se borró entre la consulta y ahora
            PITEMID_CHILD it = MakeFileItem(abs, path.c_str());
            ILFree(abs);
            if (!it) continue;
            items.push_back(it);
            paths.insert(Lower(path));
        }
        {
            std::lock_guard<std::mutex> lk(m_resMutex);
            m_paths.swap(paths);
        }
        *ppenum = new EvEnum(std::move(items));
        return S_OK;
    }

    STDMETHODIMP BindToObject(PCUIDLIST_RELATIVE pidl, IBindCtx* pbc, REFIID riid, void** ppv) override {
        *ppv = nullptr;
        if (const EvId* q = AsEv(pidl, SIG_QUERY)) {
            if (m_isQuery) return E_INVALIDARG;
            EvFolder* f = new EvFolder;
            PIDLIST_RELATIVE first = ILCloneFirst(pidl);
            f->m_pidl = first ? ILCombine(m_pidl, first) : nullptr;
            if (first) ILFree(first);
            f->m_isQuery = true;
            f->m_query = QueryText(q);
            HRESULT hr = f->QueryInterface(riid, ppv);
            f->Release();
            return hr;
        }
        // Un item real dentro de los resultados: delegar a la carpeta real.
        Real r;
        HRESULT hr = BindReal(pidl, r);
        if (FAILED(hr)) return hr;
        return r.parent->BindToObject(r.child, pbc, riid, ppv);
    }

    STDMETHODIMP BindToStorage(PCUIDLIST_RELATIVE, IBindCtx*, REFIID, void** ppv) override {
        *ppv = nullptr;
        return E_NOTIMPL;
    }

    STDMETHODIMP CompareIDs(LPARAM lParam, PCUIDLIST_RELATIVE a, PCUIDLIST_RELATIVE b) override {
        UINT col = (UINT)(lParam & SHCIDS_COLUMNMASK);
        const EvId* fa = AsEv(a, SIG_FILE);
        const EvId* fb = AsEv(b, SIG_FILE);
        if (!fa || !fb) {
            const EvId* qa = AsEv(a, SIG_QUERY);
            const EvId* qb = AsEv(b, SIG_QUERY);
            if (qa && qb) return Cmp(StrCmpIW(QueryText(qa).c_str(), QueryText(qb).c_str()));
            return E_INVALIDARG;
        }
        int r = 0;
        const wchar_t* pathA = ItemPath(fa);
        const wchar_t* pathB = ItemPath(fb);
        if (pathA && pathB && (col == 0 || col == 1)) {
            const wchar_t* na = PathFindFileNameW(pathA);
            const wchar_t* nb = PathFindFileNameW(pathB);
            if (col == 0) {
                r = StrCmpLogicalW(na, nb);  // mismo orden "natural" que el Explorador
            } else {
                std::wstring da(pathA, na - pathA), db(pathB, nb - pathB);
                r = StrCmpLogicalW(da.c_str(), db.c_str());
            }
        } else if (col < ARRAYSIZE(kCols)) {
            PROPVARIANT pa, pb;
            PropVariantInit(&pa);
            PropVariantInit(&pb);
            if (SUCCEEDED(GetProp(a, *kCols[col].key, &pa)) && SUCCEEDED(GetProp(b, *kCols[col].key, &pb)))
                r = PropVariantCompareEx(pa, pb, PVCU_DEFAULT, PVCF_DEFAULT);
            PropVariantClear(&pa);
            PropVariantClear(&pb);
        }
        if (r == 0) {  // desempate estable
            size_t n = (fa->cb < fb->cb ? fa->cb : fb->cb) - sizeof(EvId);
            r = memcmp((const BYTE*)fa + sizeof(EvId), (const BYTE*)fb + sizeof(EvId), n);
            if (r == 0) r = (int)fa->cb - (int)fb->cb;
        }
        return Cmp(r);
    }

    STDMETHODIMP CreateViewObject(HWND, REFIID riid, void** ppv) override;

    STDMETHODIMP GetAttributesOf(UINT cidl, PCUITEMID_CHILD_ARRAY apidl, SFGAOF* pa) override {
        const SFGAOF kFolderish = SFGAO_FOLDER | SFGAO_BROWSABLE;
        if (cidl == 0) {
            *pa &= kFolderish;
            return S_OK;
        }
        SFGAOF want = *pa, out = want;
        for (UINT i = 0; i < cidl; i++) {
            if (AsEv(apidl[i], SIG_QUERY)) {
                out &= (want & kFolderish);
                continue;
            }
            Real r;
            HRESULT hr = BindReal(apidl[i], r);
            if (FAILED(hr)) return hr;
            SFGAOF a = want;
            hr = r.parent->GetAttributesOf(1, &r.child, &a);
            if (FAILED(hr)) return hr;
            // Los resultados NO son subcarpetas navegables de esta carpeta:
            // abrir una carpeta resultado pasa por el menú contextual real.
            a &= ~(SFGAO_FOLDER | SFGAO_BROWSABLE | SFGAO_HASSUBFOLDER | SFGAO_CANRENAME);
            out &= a;
        }
        *pa = out;
        return S_OK;
    }

    STDMETHODIMP GetUIObjectOf(HWND hwnd, UINT cidl, PCUITEMID_CHILD_ARRAY apidl, REFIID riid, UINT*, void** ppv) override {
        *ppv = nullptr;
        if (cidl == 0 || !AsEv(apidl[0], SIG_FILE)) return E_NOINTERFACE;

        if (riid == __uuidof(IDataObject) || cidl > 1) {
            std::vector<PCIDLIST_ABSOLUTE> abs;
            for (UINT i = 0; i < cidl; i++) {
                const EvId* id = AsEv(apidl[i], SIG_FILE);
                if (!id) return E_INVALIDARG;
                abs.push_back(RealPidl(id));
            }
            IShellItemArray* sia = nullptr;
            HRESULT hr = SHCreateShellItemArrayFromIDLists(cidl, abs.data(), &sia);
            if (FAILED(hr)) return hr;
            hr = sia->BindToHandler(nullptr, riid == __uuidof(IDataObject) ? kBHID_DataObject : kBHID_SFUIObject, riid, ppv);
            sia->Release();
            return hr;
        }

        Real r;
        HRESULT hr = BindReal(apidl[0], r);
        if (FAILED(hr)) return hr;
        PCUITEMID_CHILD c = r.child;
        return r.parent->GetUIObjectOf(hwnd, 1, &c, riid, nullptr, ppv);
    }

    STDMETHODIMP GetDisplayNameOf(PCUITEMID_CHILD pidl, SHGDNF flags, STRRET* psr) override {
        if (const EvId* q = AsEv(pidl, SIG_QUERY)) {
            std::wstring t = QueryText(q);
            if (!(flags & SHGDN_FORPARSING)) {
                std::wstring scope, text;
                SplitQuery(t, scope, text);
                t = SearchTitle(text, scope);
            }
            return StringToStrRet(t.c_str(), psr);
        }
        Real r;
        HRESULT hr = BindReal(pidl, r);
        if (FAILED(hr)) return hr;
        STRRET sr;
        hr = r.parent->GetDisplayNameOf(r.child, flags, &sr);
        if (FAILED(hr)) return hr;
        PWSTR s = nullptr;
        hr = StrRetToStrW(&sr, r.child, &s);  // STRRET_OFFSET apuntaría al PIDL real, no al nuestro
        if (FAILED(hr)) return hr;
        psr->uType = STRRET_WSTR;
        psr->pOleStr = s;
        return S_OK;
    }

    STDMETHODIMP SetNameOf(HWND, PCUITEMID_CHILD, LPCWSTR, SHGDNF, PITEMID_CHILD*) override { return E_NOTIMPL; }

    // IShellFolder2
    STDMETHODIMP GetDefaultSearchGUID(GUID*) override { return E_NOTIMPL; }
    STDMETHODIMP EnumSearches(IEnumExtraSearch**) override { return E_NOTIMPL; }
    STDMETHODIMP GetDefaultColumn(DWORD, ULONG* pSort, ULONG* pDisplay) override {
        *pSort = 0;
        *pDisplay = 0;
        return S_OK;
    }
    STDMETHODIMP GetDefaultColumnState(UINT i, SHCOLSTATEF* pcs) override {
        if (i >= ARRAYSIZE(kCols)) return E_INVALIDARG;
        *pcs = kCols[i].state;
        return S_OK;
    }
    STDMETHODIMP GetDetailsEx(PCUITEMID_CHILD pidl, const SHCOLUMNID* pscid, VARIANT* pv) override {
        PROPVARIANT p;
        PropVariantInit(&p);
        HRESULT hr = GetProp(pidl, *pscid, &p);
        if (SUCCEEDED(hr)) {
            // mingw no declara PropVariantToVariant; se resuelve en runtime desde propsys.dll
            using Fn = HRESULT(WINAPI*)(const PROPVARIANT*, VARIANT*);
            static Fn fn = (Fn)GetProcAddress(LoadLibraryW(L"propsys.dll"), "PropVariantToVariant");
            hr = fn ? fn(&p, pv) : E_NOTIMPL;
        }
        PropVariantClear(&p);
        return hr;
    }
    STDMETHODIMP GetDetailsOf(PCUITEMID_CHILD pidl, UINT i, SHELLDETAILS* psd) override {
        if (i >= ARRAYSIZE(kCols)) return E_FAIL;
        psd->fmt = (i == 3) ? LVCFMT_RIGHT : LVCFMT_LEFT;
        psd->cxChar = kCols[i].cx;
        if (!pidl) return StringToStrRet(kCols[i].name, &psd->str);
        if (!AsEv(pidl, SIG_FILE)) return StringToStrRet(L"", &psd->str);
        PROPVARIANT pv;
        PropVariantInit(&pv);
        PWSTR s = nullptr;
        HRESULT hr = GetProp(pidl, *kCols[i].key, &pv);
        if (SUCCEEDED(hr)) hr = PSFormatForDisplayAlloc(*kCols[i].key, pv, PDFF_DEFAULT, &s);
        PropVariantClear(&pv);
        if (FAILED(hr)) return StringToStrRet(L"", &psd->str);
        psd->str.uType = STRRET_WSTR;
        psd->str.pOleStr = s;
        return S_OK;
    }
    STDMETHODIMP MapColumnToSCID(UINT i, SHCOLUMNID* pscid) override {
        if (i >= ARRAYSIZE(kCols)) return E_FAIL;
        *pscid = *kCols[i].key;
        return S_OK;
    }
};

// ---------------------------------------------------------------------------
// Refresco automático de la vista de resultados
//   El callback de la vista crea una ventana hija oculta dentro de la vista
//   (muere con ella) que escucha avisos de cambio del shell. Si se borra o
//   renombra un resultado, se repite la consulta con un pequeño retardo.
// ---------------------------------------------------------------------------

class EvViewCB : public IShellFolderViewCB {
    LONG m_ref = 1;

public:
    EvFolder* folder;             // referencia propia
    IShellView* view = nullptr;   // débil: la vista es dueña de este callback

    explicit EvViewCB(EvFolder* f) : folder(f) { folder->AddRef(); }
    virtual ~EvViewCB() { folder->Release(); }

    STDMETHODIMP QueryInterface(REFIID riid, void** ppv) override {
        if (riid == IID_IUnknown || riid == kIID_IShellFolderViewCB) {
            *ppv = static_cast<IShellFolderViewCB*>(this);
            AddRef();
            return S_OK;
        }
        *ppv = nullptr;
        return E_NOINTERFACE;
    }
    STDMETHODIMP_(ULONG) AddRef() override { return InterlockedIncrement(&m_ref); }
    STDMETHODIMP_(ULONG) Release() override {
        ULONG r = InterlockedDecrement(&m_ref);
        if (!r) delete this;
        return r;
    }
    STDMETHODIMP MessageSFVCB(UINT msg, WPARAM w, LPARAM l) noexcept override;
};

static const UINT kMsgShellChange = WM_APP + 1;
static const UINT_PTR kRefreshTimer = 1;
static const wchar_t kNotifyClass[] = L"EverythingExplorerSearchNotify";
static std::once_flag g_notifyClassOnce;
static bool g_notifyClassRegistered = false;

struct NotifyData {
    EvViewCB* cb;
    ULONG reg;
};

static void RegisterChangeNotify(HWND h, NotifyData* d) {
    std::wstring scope, text;
    SplitQuery(d->cb->folder->Query(), scope, text);
    PIDLIST_ABSOLUTE p = nullptr;
    if (!scope.empty()) SHParseDisplayName(StripSlash(scope).c_str(), nullptr, &p, 0, nullptr);
    bool scoped = p != nullptr;
    if (!p) {  // sin alcance: todo el espacio de nombres (PIDL vacío = Escritorio)
        p = (PIDLIST_ABSOLUTE)CoTaskMemAlloc(sizeof(USHORT));
        if (!p) return;
        *(USHORT*)p = 0;
    }
    SHChangeNotifyEntry e = {p, TRUE};
    // El nivel de interrupción (cambios hechos fuera del shell) solo con un alcance
    // que no sea la raíz de una unidad, para no vigilar recursivamente discos enteros.
    int src = SHCNRF_ShellLevel | SHCNRF_NewDelivery | (scoped && !PathIsRootW(StripSlash(scope).c_str()) ? SHCNRF_InterruptLevel : 0);
    d->reg = SHChangeNotifyRegister(h, src, SHCNE_DELETE | SHCNE_RMDIR | SHCNE_RENAMEITEM | SHCNE_RENAMEFOLDER, kMsgShellChange, 1, &e);
    ILFree(p);
}

static LRESULT CALLBACK NotifyWndProc(HWND h, UINT m, WPARAM w, LPARAM l) {
    NotifyData* d = (NotifyData*)GetWindowLongPtrW(h, GWLP_USERDATA);
    switch (m) {
        case WM_NCCREATE: {
            auto* cs = (CREATESTRUCTW*)l;
            d = new NotifyData{(EvViewCB*)cs->lpCreateParams, 0};
            d->cb->AddRef();
            SetWindowLongPtrW(h, GWLP_USERDATA, (LONG_PTR)d);
            break;
        }
        case WM_CREATE:
            RegisterChangeNotify(h, d);
            return 0;
        case kMsgShellChange: {
            PIDLIST_ABSOLUTE* pidls = nullptr;
            LONG ev = 0;
            HANDLE lock = SHChangeNotification_Lock((HANDLE)w, (DWORD)l, &pidls, &ev);
            if (!lock) return 0;
            bool hit = false;
            if (pidls && pidls[0]) {
                PWSTR s = nullptr;
                if (SUCCEEDED(SHGetNameFromIDList(pidls[0], SIGDN_FILESYSPATH, &s))) {
                    hit = d->cb->folder->Affects(Lower(s), (ev & (SHCNE_RMDIR | SHCNE_RENAMEFOLDER)) != 0);
                    CoTaskMemFree(s);
                }
            }
            SHChangeNotification_Unlock(lock);
            // Retardo: Everything tarda unos milisegundos en reflejar el cambio
            // y un borrado múltiple genera un aviso por archivo.
            if (hit) SetTimer(h, kRefreshTimer, 500, nullptr);
            return 0;
        }
        case WM_TIMER:
            if (w == kRefreshTimer) {
                KillTimer(h, kRefreshTimer);
                if (d && d->cb->view) d->cb->view->Refresh();
                return 0;
            }
            break;
        case WM_DESTROY:
            if (d && d->reg) SHChangeNotifyDeregister(d->reg);
            break;
        case WM_NCDESTROY:
            if (d) {
                d->cb->Release();
                delete d;
                SetWindowLongPtrW(h, GWLP_USERDATA, 0);
            }
            break;
    }
    return DefWindowProcW(h, m, w, l);
}

STDMETHODIMP EvViewCB::MessageSFVCB(UINT msg, WPARAM w, LPARAM) noexcept {
    if (msg != SFVM_WINDOWCREATED) return E_NOTIMPL;
    std::call_once(g_notifyClassOnce, [] {
        WNDCLASSEXW wc = {};
        wc.cbSize = sizeof(wc);
        wc.lpfnWndProc = NotifyWndProc;
        wc.hInstance = g_self;
        wc.lpszClassName = kNotifyClass;
        g_notifyClassRegistered = RegisterClassExW(&wc) != 0;
    });
    if (g_notifyClassRegistered)
        CreateWindowExW(0, kNotifyClass, L"", WS_CHILD, 0, 0, 0, 0, (HWND)w, nullptr, g_self, this);
    return S_OK;
}

STDMETHODIMP EvFolder::CreateViewObject(HWND, REFIID riid, void** ppv) {
    *ppv = nullptr;
    if (riid != __uuidof(IShellView)) return E_NOINTERFACE;
    EvViewCB* cb = m_isQuery ? new EvViewCB(this) : nullptr;
    SFV_CREATE sfv = {sizeof(sfv), static_cast<IShellFolder*>(this), nullptr, cb};
    IShellView* sv = nullptr;
    HRESULT hr = SHCreateShellFolderView(&sfv, &sv);
    if (cb) {
        if (SUCCEEDED(hr)) cb->view = sv;
        cb->Release();
    }
    *ppv = sv;
    return hr;
}

// ---------------------------------------------------------------------------
// Class factory + export (el shell instancia la carpeta vía registro HKCU,
// que apunta a este mismo DLL)
// ---------------------------------------------------------------------------

class EvFactory : public IClassFactory {
public:
    STDMETHODIMP QueryInterface(REFIID riid, void** ppv) override {
        if (riid == IID_IUnknown || riid == __uuidof(IClassFactory)) {
            *ppv = static_cast<IClassFactory*>(this);
            return S_OK;
        }
        *ppv = nullptr;
        return E_NOINTERFACE;
    }
    STDMETHODIMP_(ULONG) AddRef() override { return 2; }
    STDMETHODIMP_(ULONG) Release() override { return 1; }
    STDMETHODIMP CreateInstance(IUnknown* outer, REFIID riid, void** ppv) override {
        *ppv = nullptr;
        if (outer) return CLASS_E_NOAGGREGATION;
        EvFolder* f = new EvFolder;
        HRESULT hr = f->QueryInterface(riid, ppv);
        f->Release();
        return hr;
    }
    STDMETHODIMP LockServer(BOOL) override { return S_OK; }
};
static EvFactory g_factory;

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdll-attribute-on-redeclaration"
#endif
extern "C" __declspec(dllexport) HRESULT STDAPICALLTYPE DllGetClassObject(REFCLSID clsid, REFIID riid, void** ppv) {
    if (clsid == CLSID_EvFolder) return g_factory.QueryInterface(riid, ppv);
    *ppv = nullptr;
    return CLASS_E_CLASSNOTAVAILABLE;
}
#ifdef __clang__
#pragma clang diagnostic pop
#endif

static const wchar_t kHideNewStart[] = L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\HideDesktopIcons\\NewStartPanel";
static const wchar_t kHideClassic[] = L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\HideDesktopIcons\\ClassicStartMenu";

static std::wstring NameSpaceKey() {
    return std::wstring(L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Desktop\\NameSpace\\") + kClsidStr;
}

static std::wstring ClsidKey() { return std::wstring(L"Software\\Classes\\CLSID\\") + kClsidStr; }

static void RegisterFolderClass() {
    GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                       (LPCWSTR)&DllGetClassObject, &g_self);
    WCHAR path[MAX_PATH] = {};
    GetModuleFileNameW(g_self, path, MAX_PATH);

    std::wstring base = ClsidKey();
    static const wchar_t kName[] = L"Everything";
    static const wchar_t kBoth[] = L"Both";
    DWORD attr = SFGAO_FOLDER | SFGAO_BROWSABLE;
    RegSetKeyValueW(HKEY_CURRENT_USER, base.c_str(), nullptr, REG_SZ, kName, sizeof(kName));
    RegSetKeyValueW(HKEY_CURRENT_USER, (base + L"\\InprocServer32").c_str(), nullptr, REG_SZ, path, (DWORD)((wcslen(path) + 1) * sizeof(WCHAR)));
    RegSetKeyValueW(HKEY_CURRENT_USER, (base + L"\\InprocServer32").c_str(), L"ThreadingModel", REG_SZ, kBoth, sizeof(kBoth));
    RegSetKeyValueW(HKEY_CURRENT_USER, (base + L"\\ShellFolder").c_str(), L"Attributes", REG_DWORD, &attr, sizeof(attr));

    // Sin esta entrada "::{CLSID}" no se puede parsear (E_INVALIDARG). Se oculta del
    // escritorio y del panel de navegación.
    DWORD zero = 0, one = 1;
    RegSetKeyValueW(HKEY_CURRENT_USER, base.c_str(), L"System.IsPinnedToNameSpaceTree", REG_DWORD, &zero, sizeof(zero));
    for (const wchar_t* k : {kHideNewStart, kHideClassic}) RegSetKeyValueW(HKEY_CURRENT_USER, k, kClsidStr, REG_DWORD, &one, sizeof(one));
    RegSetKeyValueW(HKEY_CURRENT_USER, NameSpaceKey().c_str(), nullptr, REG_SZ, kName, sizeof(kName));
}

static void UnregisterFolderClass() {
    RegDeleteTreeW(HKEY_CURRENT_USER, NameSpaceKey().c_str());
    for (const wchar_t* k : {kHideNewStart, kHideClassic}) RegDeleteKeyValueW(HKEY_CURRENT_USER, k, kClsidStr);
    RegDeleteTreeW(HKEY_CURRENT_USER, ClsidKey().c_str());
}

// ---------------------------------------------------------------------------
// Navegación
// ---------------------------------------------------------------------------

static std::wstring g_lastScope;
static std::mutex g_scopeMutex;  // cada ventana del Explorador corre en su propio hilo
static UINT g_msgUnsub = 0;
static bool g_swallowChar = false;
static std::mutex g_listMutex;
static std::vector<HWND> g_subclassed;

static HWND FindAncestorByClass(HWND h, PCWSTR cls) {
    WCHAR buf[128];
    for (HWND p = GetAncestor(h, GA_PARENT); p; p = GetAncestor(p, GA_PARENT)) {
        if (GetClassNameW(p, buf, ARRAYSIZE(buf)) && !_wcsicmp(buf, cls)) return p;
    }
    return nullptr;
}

// IShellBrowser de la pestaña/ventana que contiene la caja de búsqueda.
static IShellBrowser* FindShellBrowser(HWND frame, HWND tab) {
    IShellWindows* psw = nullptr;
    if (FAILED(CoCreateInstance(kCLSID_ShellWindows, nullptr, CLSCTX_ALL, IID_PPV_ARGS(&psw)))) return nullptr;
    IShellBrowser* best = nullptr;
    IShellBrowser* fallback = nullptr;
    long n = 0;
    psw->get_Count(&n);
    for (long i = 0; i < n && !best; i++) {
        VARIANT v;
        VariantInit(&v);
        v.vt = VT_I4;
        v.lVal = i;
        IDispatch* disp = nullptr;
        if (FAILED(psw->Item(v, &disp)) || !disp) continue;
        IServiceProvider* sp = nullptr;
        if (SUCCEEDED(disp->QueryInterface(IID_PPV_ARGS(&sp)))) {
            IShellBrowser* sb = nullptr;
            if (SUCCEEDED(sp->QueryService(kSID_STopLevelBrowser, IID_PPV_ARGS(&sb)))) {
                HWND h = nullptr;
                sb->GetWindow(&h);
                if (tab && h == tab) {
                    best = sb;
                } else if (h == frame || h == tab) {
                    if (!fallback) {
                        fallback = sb;
                        sb = nullptr;
                    }
                }
                if (sb && sb != best) sb->Release();
            }
            sp->Release();
        }
        disp->Release();
    }
    psw->Release();
    if (best) {
        if (fallback) fallback->Release();
        return best;
    }
    return fallback;
}

static bool LooksLikeFsPath(const std::wstring& s) {
    return (s.size() >= 3 && iswalpha(s[0]) && s[1] == L':' && s[2] == L'\\') || (s.size() > 2 && s[0] == L'\\' && s[1] == L'\\');
}

// PIDL de la carpeta que muestra la pestaña (hay que liberarlo con ILFree).
static PIDLIST_ABSOLUTE CurrentFolder(IShellBrowser* sb) {
    PIDLIST_ABSOLUTE cur = nullptr;
    IShellView* sv = nullptr;
    if (SUCCEEDED(sb->QueryActiveShellView(&sv))) {
        IFolderView* fv = nullptr;
        if (SUCCEEDED(sv->QueryInterface(IID_PPV_ARGS(&fv)))) {
            IPersistFolder2* pf = nullptr;
            if (SUCCEEDED(fv->GetFolder(IID_PPV_ARGS(&pf)))) {
                if (FAILED(pf->GetCurFolder(&cur))) cur = nullptr;
                pf->Release();
            }
            fv->Release();
        }
        sv->Release();
    }
    return cur;
}

static bool IsInsideRoot(PCIDLIST_ABSOLUTE root, PCIDLIST_ABSOLUTE cur) {
    return root && cur && (ILIsEqual(root, cur) || ILIsParent(root, cur, FALSE));
}

// Carpeta actual como alcance de búsqueda (vacío = todo Everything).
static std::wstring GetScope(IShellBrowser* sb, PCIDLIST_ABSOLUTE root) {
    std::wstring scope;
    bool inOurFolder = false;
    PIDLIST_ABSOLUTE cur = CurrentFolder(sb);
    if (cur) {
        inOurFolder = IsInsideRoot(root, cur);
        PWSTR s = nullptr;
        if (SUCCEEDED(SHGetNameFromIDList(cur, SIGDN_FILESYSPATH, &s))) {
            scope = s;
            CoTaskMemFree(s);
        } else if (SUCCEEDED(SHGetNameFromIDList(cur, SIGDN_URL, &s))) {
            std::wstring url = s;
            CoTaskMemFree(s);
            size_t p = url.find(L"crumb=location:");
            if (p != std::wstring::npos) {
                p += 15;
                size_t e = url.find(L'&', p);
                std::wstring enc = url.substr(p, e == std::wstring::npos ? e : e - p);
                std::vector<WCHAR> b(enc.begin(), enc.end());
                b.push_back(0);
                UrlUnescapeW(b.data(), nullptr, nullptr, URL_UNESCAPE_INPLACE);
                scope = b.data();
            }
        }
        ILFree(cur);
    }
    std::lock_guard<std::mutex> lk(g_scopeMutex);
    if (LooksLikeFsPath(scope)) {
        g_lastScope = scope;
        return scope;
    }
    return inOurFolder ? g_lastScope : L"";
}

#pragma pack(push, 1)
struct RegItemPidl {
    USHORT cb;
    BYTE type;
    BYTE order;
    GUID clsid;
    USHORT term;
};
#pragma pack(pop)

// PIDL de un "regitem" (carpeta identificada por CLSID) hijo del Escritorio, armado a mano.
static PIDLIST_ABSOLUTE MakeRootPidl() {
    RegItemPidl* r = (RegItemPidl*)CoTaskMemAlloc(sizeof(RegItemPidl));
    if (!r) return nullptr;
    r->cb = sizeof(RegItemPidl) - sizeof(USHORT);
    r->type = 0x1F;
    r->order = 0x50;
    r->clsid = CLSID_EvFolder;
    r->term = 0;
    return (PIDLIST_ABSOLUTE)r;
}

static PIDLIST_ABSOLUTE GetRootPidl() {
    PIDLIST_ABSOLUTE root = nullptr;
    std::wstring rootName = std::wstring(L"::") + kClsidStr;
    HRESULT hr = SHParseDisplayName(rootName.c_str(), nullptr, &root, 0, nullptr);
    return SUCCEEDED(hr) && root ? root : MakeRootPidl();
}

static std::wstring UrlEncode(const std::wstring& s) {
    int n = WideCharToMultiByte(CP_UTF8, 0, s.c_str(), (int)s.size(), nullptr, 0, nullptr, nullptr);
    std::string u(n > 0 ? n : 0, '\0');
    if (n > 0) WideCharToMultiByte(CP_UTF8, 0, s.c_str(), (int)s.size(), &u[0], n, nullptr, nullptr);
    static const wchar_t hex[] = L"0123456789ABCDEF";
    std::wstring r;
    for (unsigned char c : u) {
        bool plain = (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9') || c == '-' || c == '_' || c == '.' ||
                     c == '~';
        if (plain) {
            r += (wchar_t)c;
        } else {
            r += L'%';
            r += hex[c >> 4];
            r += hex[c & 15];
        }
    }
    return r;
}

// Everything no responde: misma búsqueda con Windows Search.
static bool NavigateWindowsSearch(IShellBrowser* sb, const std::wstring& scope, const std::wstring& q) {
    std::wstring url = L"search-ms:displayname=" + UrlEncode(SearchTitle(q, scope)) + L"&query=" + UrlEncode(q);
    if (!scope.empty()) url += L"&crumb=location:" + UrlEncode(StripSlash(scope));
    PIDLIST_ABSOLUTE p = nullptr;
    HRESULT hr = SHParseDisplayName(url.c_str(), nullptr, &p, 0, nullptr);
    if (FAILED(hr) || !p) return false;
    hr = sb->BrowseObject(p, SBSP_SAMEBROWSER | SBSP_ABSOLUTE);
    ILFree(p);
    return SUCCEEDED(hr);
}

// fallback: si Everything no responde, buscar con Windows Search en vez de devolver false.
// sbspExtra: flags adicionales para BrowseObject (p. ej. SBSP_WRITENOHISTORY).
static bool NavigateSearch(HWND frame, HWND tab, const std::wstring& q, bool fallback, UINT sbspExtra) {
    if (q.empty()) return false;
    bool ev = EvAvailable();
    if (!ev && !fallback) return false;
    IShellBrowser* sb = FindShellBrowser(frame, tab);
    if (!sb) return false;
    PIDLIST_ABSOLUTE root = GetRootPidl();
    if (!root) {
        sb->Release();
        return false;
    }

    std::wstring scope = GetScope(sb, root);
    bool ok = false;
    if (!ev) {
        ok = NavigateWindowsSearch(sb, scope, q);
    } else {
        std::wstring full = q;
        if (!scope.empty()) {
            if (scope.back() != L'\\') scope += L'\\';
            full = L"\"" + scope + L"\" " + q;
        }
        PITEMID_CHILD item = MakeQueryItem(full);
        if (item) {
            PIDLIST_ABSOLUTE abs = ILCombine(root, item);
            if (abs) {
                ok = SUCCEEDED(sb->BrowseObject(abs, SBSP_SAMEBROWSER | SBSP_ABSOLUTE | sbspExtra));
                ILFree(abs);
            }
            CoTaskMemFree(item);
        }
    }
    ILFree(root);
    sb->Release();
    return ok;
}

// Enter dentro de la caja de búsqueda clásica (Edit Win32). Si Everything no
// responde, se deja seguir a la búsqueda nativa de esa misma caja.
static bool HandleEnter(HWND edit) {
    WCHAR text[2048] = {};
    GetWindowTextW(edit, text, ARRAYSIZE(text));
    return NavigateSearch(GetAncestor(edit, GA_ROOT), FindAncestorByClass(edit, L"ShellTabWindowClass"), Trim(text), false, 0);
}

// Antes de descargar el mod: las pestañas que muestran resultados usan código
// de este DLL, así que se las manda a la carpeta de origen (o a Este equipo).
static void EvacuateThreadBrowsers() {
    PIDLIST_ABSOLUTE root = GetRootPidl();
    if (!root) return;
    IShellWindows* psw = nullptr;
    if (FAILED(CoCreateInstance(kCLSID_ShellWindows, nullptr, CLSCTX_ALL, IID_PPV_ARGS(&psw)))) {
        ILFree(root);
        return;
    }
    long n = 0;
    psw->get_Count(&n);
    for (long i = 0; i < n; i++) {
        VARIANT v;
        VariantInit(&v);
        v.vt = VT_I4;
        v.lVal = i;
        IDispatch* disp = nullptr;
        if (FAILED(psw->Item(v, &disp)) || !disp) continue;
        IServiceProvider* sp = nullptr;
        IShellBrowser* sb = nullptr;
        if (SUCCEEDED(disp->QueryInterface(IID_PPV_ARGS(&sp)))) {
            sp->QueryService(kSID_STopLevelBrowser, IID_PPV_ARGS(&sb));
            sp->Release();
        }
        disp->Release();
        if (!sb) continue;
        HWND h = nullptr;
        sb->GetWindow(&h);
        PIDLIST_ABSOLUTE cur = (h && GetWindowThreadProcessId(h, nullptr) == GetCurrentThreadId()) ? CurrentFolder(sb) : nullptr;
        if (IsInsideRoot(root, cur)) {
            PIDLIST_ABSOLUTE target = nullptr;
            if (const EvId* q = AsEv(ILFindLastID(cur), SIG_QUERY)) {
                std::wstring scope, text;
                SplitQuery(QueryText(q), scope, text);
                if (!scope.empty()) SHParseDisplayName(StripSlash(scope).c_str(), nullptr, &target, 0, nullptr);
            }
            if (!target) SHGetKnownFolderIDList(kFOLDERID_ComputerFolder, 0, nullptr, &target);
            if (target) {
                sb->BrowseObject(target, SBSP_SAMEBROWSER | SBSP_ABSOLUTE | SBSP_WRITENOHISTORY);
                ILFree(target);
            }
        }
        if (cur) ILFree(cur);
        sb->Release();
    }
    psw->Release();
    ILFree(root);
}

// ---------------------------------------------------------------------------
// Atajo + cuadro de texto propio (no depende de la caja de búsqueda de Windows)
// ---------------------------------------------------------------------------

static HWND g_popup = nullptr;      // ventana contenedora (la que se ve)
static HWND g_popupEdit = nullptr;  // Edit hijo
static HWND g_popupFrame = nullptr;
static std::atomic<bool> g_uiaMatched{false};

// Búsqueda mientras se escribe
static PIDLIST_ABSOLUTE g_popupOrigin = nullptr;  // carpeta a la que vuelve Escape
static bool g_liveNavigated = false;
static std::wstring g_liveText;
static ULONGLONG g_liveGuardUntil = 0;  // hasta cuándo una pérdida de foco se atribuye a la navegación
static const UINT_PTR kLiveTimer = 1;
static const UINT kMsgRefocus = WM_APP + 2;

struct PopupReq {
    RECT rc;
    bool hasRect;
    std::wstring text;
};
static UINT g_msgShowPopup = 0;
static UINT g_msgEvacuate = 0;
static std::atomic<ULONGLONG> g_popupClosedTick{0};
static std::atomic<LONG> g_evacPending{0};
static HANDLE g_evacDone = nullptr;

static BOOL CALLBACK FindTabProc(HWND h, LPARAM l) {
    WCHAR cls[64];
    if (GetClassNameW(h, cls, ARRAYSIZE(cls)) && !_wcsicmp(cls, L"ShellTabWindowClass") && IsWindowVisible(h)) {
        *(HWND*)l = h;
        return FALSE;
    }
    return TRUE;
}

static HWND FindActiveTab(HWND frame) {
    HWND r = nullptr;
    EnumChildWindows(frame, FindTabProc, (LPARAM)&r);
    return r;
}

static void RestoreOrigin() {
    if (g_popupOrigin && g_popupFrame && IsWindow(g_popupFrame)) {
        if (IShellBrowser* sb = FindShellBrowser(g_popupFrame, FindActiveTab(g_popupFrame))) {
            sb->BrowseObject(g_popupOrigin, SBSP_SAMEBROWSER | SBSP_ABSOLUTE | SBSP_WRITENOHISTORY);
            sb->Release();
        }
    }
    g_liveNavigated = false;
    g_liveText.clear();
}

static void LiveSearch() {
    if (!g_popupEdit || !g_popupFrame || !IsWindow(g_popupFrame)) return;
    WCHAR text[2048] = {};
    GetWindowTextW(g_popupEdit, text, ARRAYSIZE(text));
    std::wstring q = Trim(text);
    if (q == g_liveText) return;
    if (q.empty()) {
        if (g_liveNavigated) RestoreOrigin();
        return;
    }
    if (!EvAvailable()) return;  // Windows Search es lento para buscar en cada tecla: espera al Enter
    g_liveGuardUntil = GetTickCount64() + 1500;
    // La primera navegación queda en el historial (Atrás vuelve al origen); las siguientes no.
    if (NavigateSearch(g_popupFrame, FindActiveTab(g_popupFrame), q, false, g_liveNavigated ? SBSP_WRITENOHISTORY : 0)) {
        g_liveNavigated = true;
        g_liveText = q;
    }
    if (g_popup) PostMessageW(g_popup, kMsgRefocus, 0, 0);
}

// --- Apariencia: colores del tema, fuentes y pinceles del cuadro de texto ---

struct PopupTheme {
    COLORREF bg, text, border, hint, accent;
};
struct PopupState {
    PopupTheme th;
    HBRUSH bg = nullptr;
    HFONT font = nullptr;
    HFONT icon = nullptr;
    int dpi = 96;
};
static PopupState g_ps;
static const wchar_t kPopupClass[] = L"EverythingExplorerSearchPopup";
static bool g_popupClassRegistered = false;

static PopupTheme MakeTheme() {
    DWORD light = 1, cb = sizeof(light);
    RegGetValueW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize", L"AppsUseLightTheme",
                 RRF_RT_REG_DWORD, nullptr, &light, &cb);
    PopupTheme t;
    if (!light) {
        t.bg = RGB(45, 45, 45);
        t.text = RGB(255, 255, 255);
        t.border = RGB(70, 70, 70);
        t.hint = RGB(190, 190, 190);
    } else {
        t.bg = RGB(255, 255, 255);
        t.text = RGB(27, 27, 27);
        t.border = RGB(224, 224, 224);
        t.hint = RGB(96, 94, 92);
    }
    DWORD col = 0;
    BOOL opaque = FALSE;
    if (SUCCEEDED(DwmGetColorizationColor(&col, &opaque)))
        t.accent = RGB((col >> 16) & 0xFF, (col >> 8) & 0xFF, col & 0xFF);
    else
        t.accent = RGB(0, 120, 212);
    return t;
}

static void LayoutPopup(HWND h) {
    if (!g_popupEdit || !g_ps.font) return;
    RECT rc;
    GetClientRect(h, &rc);
    int padL = MulDiv(12, g_ps.dpi, 96);
    int padR = MulDiv(38, g_ps.dpi, 96);
    HDC dc = GetDC(h);
    HGDIOBJ old = SelectObject(dc, g_ps.font);
    TEXTMETRICW tm = {};
    GetTextMetricsW(dc, &tm);
    SelectObject(dc, old);
    ReleaseDC(h, dc);
    int th = tm.tmHeight;
    int y = (rc.bottom - th) / 2;
    if (y < 0) y = 0;
    int w = rc.right - padL - padR;
    if (w < 10) w = 10;
    MoveWindow(g_popupEdit, padL, y, w, th, TRUE);
}

static LRESULT CALLBACK PopupEditProc(HWND h, UINT m, WPARAM w, LPARAM l, UINT_PTR id, DWORD_PTR) {
    switch (m) {
        case WM_KEYDOWN:
            if (w == VK_ESCAPE) {
                if (g_liveNavigated) RestoreOrigin();
                DestroyWindow(g_popup);
                return 0;
            }
            if (w == L'A' && (GetKeyState(VK_CONTROL) & 0x8000)) {
                SendMessageW(h, EM_SETSEL, 0, -1);
                return 0;
            }
            if (w == VK_RETURN) {
                WCHAR text[2048] = {};
                GetWindowTextW(h, text, ARRAYSIZE(text));
                std::wstring q = Trim(text);
                HWND frame = g_popupFrame;
                bool shown = g_liveNavigated && q == g_liveText;
                UINT extra = g_liveNavigated ? SBSP_WRITENOHISTORY : 0;
                if (q.empty() && g_liveNavigated) RestoreOrigin();
                DestroyWindow(g_popup);
                if (frame && IsWindow(frame)) {
                    SetForegroundWindow(frame);
                    if (!shown) NavigateSearch(frame, FindActiveTab(frame), q, true, extra);
                }
                return 0;
            }
            break;
        case WM_CHAR:
            if (w == VK_RETURN || w == VK_ESCAPE || w == 1) return 0;  // sin beep (1 = Ctrl+A)
            break;
        case WM_NCDESTROY:
            RemoveWindowSubclass(h, PopupEditProc, id);
            break;
    }
    return DefSubclassProc(h, m, w, l);
}

static LRESULT CALLBACK PopupWndProc(HWND h, UINT m, WPARAM w, LPARAM l) {
    switch (m) {
        case WM_CREATE: {
            HWND e = CreateWindowExW(0, L"Edit", L"", WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL, 0, 0, 10, 10, h, nullptr, g_self, nullptr);
            if (!e) return -1;
            g_popupEdit = e;
            SendMessageW(e, WM_SETFONT, (WPARAM)g_ps.font, TRUE);
            SendMessageW(e, EM_SETCUEBANNER, TRUE, (LPARAM)L"Buscar con Everything…");
            SetWindowSubclass(e, PopupEditProc, 1, 0);
            return 0;
        }
        case WM_COMMAND:
            if (HIWORD(w) == EN_CHANGE && (HWND)l == g_popupEdit && g_live) SetTimer(h, kLiveTimer, 200, nullptr);
            return 0;
        case WM_TIMER:
            if (w == kLiveTimer) {
                KillTimer(h, kLiveTimer);
                LiveSearch();
                return 0;
            }
            break;
        case kMsgRefocus:
            SetForegroundWindow(h);
            if (g_popupEdit) SetFocus(g_popupEdit);
            return 0;
        case WM_SIZE:
            LayoutPopup(h);
            return 0;
        case WM_ERASEBKGND: {
            RECT rc;
            GetClientRect(h, &rc);
            FillRect((HDC)w, &rc, g_ps.bg);
            return 1;
        }
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC dc = BeginPaint(h, &ps);
            RECT rc;
            GetClientRect(h, &rc);
            FillRect(dc, &rc, g_ps.bg);
            // borde fino
            HPEN pen = CreatePen(PS_SOLID, 1, g_ps.th.border);
            HGDIOBJ op = SelectObject(dc, pen);
            HGDIOBJ ob = SelectObject(dc, GetStockObject(NULL_BRUSH));
            Rectangle(dc, 0, 0, rc.right, rc.bottom);
            SelectObject(dc, ob);
            SelectObject(dc, op);
            DeleteObject(pen);
            // subrayado con el color de acento (como la caja nativa con foco)
            int t = MulDiv(2, g_ps.dpi, 96);
            RECT ul = {1, rc.bottom - 1 - t, rc.right - 1, rc.bottom - 1};
            HBRUSH ab = CreateSolidBrush(g_ps.th.accent);
            FillRect(dc, &ul, ab);
            DeleteObject(ab);
            // lupa
            SetBkMode(dc, TRANSPARENT);
            SetTextColor(dc, g_ps.th.hint);
            HGDIOBJ of = SelectObject(dc, g_ps.icon);
            RECT ir = {rc.right - MulDiv(38, g_ps.dpi, 96), 0, rc.right - MulDiv(8, g_ps.dpi, 96), rc.bottom};
            DrawTextW(dc, L"\uE721", 1, &ir, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
            SelectObject(dc, of);
            EndPaint(h, &ps);
            return 0;
        }
        case WM_CTLCOLOREDIT:
            SetTextColor((HDC)w, g_ps.th.text);
            SetBkColor((HDC)w, g_ps.th.bg);
            return (LRESULT)g_ps.bg;
        case WM_SETFOCUS:
            if (g_popupEdit) SetFocus(g_popupEdit);
            return 0;
        case WM_ACTIVATE:
            if (LOWORD(w) == WA_INACTIVE) {
                // Si la vista nueva se quedó con el foco justo después de una navegación
                // en vivo, se devuelve el foco al cuadro. Un click del usuario cancela.
                bool click = ((GetAsyncKeyState(VK_LBUTTON) | GetAsyncKeyState(VK_RBUTTON) | GetAsyncKeyState(VK_MBUTTON)) & 0x8000) != 0;
                HWND to = (HWND)l;
                if (!click && GetTickCount64() < g_liveGuardUntil && to && GetAncestor(to, GA_ROOT) == g_popupFrame)
                    PostMessageW(h, kMsgRefocus, 0, 0);
                else
                    PostMessageW(h, WM_CLOSE, 0, 0);  // click afuera = cerrar (los resultados quedan)
            }
            break;
        case WM_CLOSE:
            DestroyWindow(h);
            return 0;
        case WM_NCDESTROY:
            g_popup = nullptr;
            g_popupEdit = nullptr;
            g_popupClosedTick = GetTickCount64();
            if (g_popupOrigin) ILFree(g_popupOrigin);
            g_popupOrigin = nullptr;
            g_liveNavigated = false;
            g_liveText.clear();
            g_liveGuardUntil = 0;
            if (g_ps.bg) DeleteObject(g_ps.bg);
            if (g_ps.font) DeleteObject(g_ps.font);
            if (g_ps.icon) DeleteObject(g_ps.icon);
            g_ps.bg = nullptr;
            g_ps.font = nullptr;
            g_ps.icon = nullptr;
            break;
    }
    return DefWindowProcW(h, m, w, l);
}

static bool EnsurePopupClass() {
    if (g_popupClassRegistered) return true;
    WNDCLASSEXW wc = {};
    wc.cbSize = sizeof(wc);
    wc.lpfnWndProc = PopupWndProc;
    wc.hInstance = g_self;
    wc.hCursor = LoadCursorW(nullptr, MAKEINTRESOURCEW(32512));
    wc.lpszClassName = kPopupClass;
    if (!RegisterClassExW(&wc)) return false;
    g_popupClassRegistered = true;
    return true;
}

static void ShowPopup(HWND frame, const PopupReq* req = nullptr) {
    if (g_popup) {
        SetFocus(g_popupEdit);
        return;
    }
    if (!EnsurePopupClass()) return;

    UINT dpi = GetDpiForWindow(frame);
    if (!dpi) dpi = 96;
    g_ps.dpi = (int)dpi;
    g_ps.th = MakeTheme();
    g_ps.bg = CreateSolidBrush(g_ps.th.bg);
    int fpx = MulDiv(14, (int)dpi, 96);
    g_ps.font = CreateFontW(-fpx, 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET, 0, 0, CLEARTYPE_QUALITY, 0, L"Segoe UI");
    g_ps.icon = CreateFontW(-fpx, 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET, 0, 0, CLEARTYPE_QUALITY, 0, L"Segoe Fluent Icons");

    RECT fr;
    GetWindowRect(frame, &fr);
    int x, y, w, h;
    if (req && req->hasRect && req->rc.right - req->rc.left > 20) {
        // Mismo rectángulo que la caja de búsqueda nativa.
        x = req->rc.left;
        y = req->rc.top;
        w = req->rc.right - req->rc.left;
        h = req->rc.bottom - req->rc.top;
        if (w < MulDiv(200, (int)dpi, 96)) w = MulDiv(200, (int)dpi, 96);
        if (h < MulDiv(28, (int)dpi, 96)) h = MulDiv(28, (int)dpi, 96);
    } else {
        int fw = fr.right - fr.left;
        w = MulDiv(640, (int)dpi, 96);
        if (w > fw - 40) w = fw - 40;
        h = MulDiv(40, (int)dpi, 96);
        x = fr.left + (fw - w) / 2;
        y = fr.top + MulDiv(90, (int)dpi, 96);
    }

    HWND hw = CreateWindowExW(WS_EX_TOPMOST | WS_EX_TOOLWINDOW, kPopupClass, L"", WS_POPUP, x, y, w, h, frame, nullptr, g_self, nullptr);
    if (!hw) return;
    // Esquinas redondeadas chicas de Win11 (DWMWCP_ROUNDSMALL) y borde del color del tema.
    int pref = 3;
    DwmSetWindowAttribute(hw, 33, &pref, sizeof(pref));
    COLORREF bc = g_ps.th.border;
    DwmSetWindowAttribute(hw, 34, &bc, sizeof(bc));

    g_popup = hw;
    g_popupFrame = frame;
    if (g_live) {
        if (IShellBrowser* sb = FindShellBrowser(frame, FindActiveTab(frame))) {
            g_popupOrigin = CurrentFolder(sb);
            sb->Release();
        }
    }
    LayoutPopup(hw);
    if (req && !req->text.empty()) {
        SetWindowTextW(g_popupEdit, req->text.c_str());
        SendMessageW(g_popupEdit, EM_SETSEL, 0x7FFF, 0x7FFF);
    }
    ShowWindow(hw, SW_SHOW);
    SetForegroundWindow(hw);
    SetFocus(g_popupEdit);
}

static bool MatchesHotkey(WPARAM vk, LPARAM lp) {
    if (vk != 'F' || g_hotkey == HK_NONE) return false;
    bool ctrl = (GetKeyState(VK_CONTROL) & 0x8000) != 0;
    bool shift = (GetKeyState(VK_SHIFT) & 0x8000) != 0;
    bool alt = (lp & (1 << 29)) != 0;
    if (!ctrl || alt) return false;
    return g_hotkey == HK_CTRL_F ? !shift : shift;
}

// Hook de teclado por hilo (corre en el hilo de UI de cada ventana del Explorador).
static LRESULT CALLBACK KbProc(int code, WPARAM w, LPARAM l) {
    if (code == HC_ACTION && !(l & 0x80000000) && !(l & 0x40000000) && MatchesHotkey(w, l)) {
        HWND fg = GetForegroundWindow();
        WCHAR cls[64];
        if (fg && GetWindowThreadProcessId(fg, nullptr) == GetCurrentThreadId() && GetClassNameW(fg, cls, ARRAYSIZE(cls)) &&
            !_wcsicmp(cls, L"CabinetWClass")) {
            // Si UIA ya reconoció la caja nativa, se deja que el Explorador enfoque su propia caja:
            // el cuadro de texto aparece justo encima de ella, en su posición exacta.
            if (g_intercept && g_uiaMatched) return CallNextHookEx(nullptr, code, w, l);
            ShowPopup(fg);
            return 1;  // no llega al Explorador
        }
    }
    return CallNextHookEx(nullptr, code, w, l);
}

// Se ejecuta fuera del hook, desde el bucle de mensajes del hilo.
static VOID CALLBACK EvacuateTimerProc(HWND, UINT, UINT_PTR id, DWORD) {
    KillTimer(nullptr, id);
    EvacuateThreadBrowsers();
    if (--g_evacPending == 0 && g_evacDone) SetEvent(g_evacDone);
}

// Mensajes que llegan al hilo de UI del Explorador desde otros hilos del mod.
static LRESULT CALLBACK GetMsgProc(int code, WPARAM w, LPARAM l) {
    if (code == HC_ACTION && w == PM_REMOVE) {
        MSG* m = (MSG*)l;
        if (g_msgShowPopup && m->message == g_msgShowPopup) {
            PopupReq* req = (PopupReq*)m->lParam;
            m->message = WM_NULL;
            m->lParam = 0;
            HWND fg = GetForegroundWindow();
            WCHAR cls[64];
            if (req && fg && GetWindowThreadProcessId(fg, nullptr) == GetCurrentThreadId() &&
                GetClassNameW(fg, cls, ARRAYSIZE(cls)) && !_wcsicmp(cls, L"CabinetWClass"))
                ShowPopup(fg, req);
            delete req;
            return 0;
        }
        if (g_msgEvacuate && m->message == g_msgEvacuate) {
            m->message = WM_NULL;
            if (!SetTimer(nullptr, 0, 0, EvacuateTimerProc) && --g_evacPending == 0 && g_evacDone) SetEvent(g_evacDone);
            return 0;
        }
    }
    return CallNextHookEx(nullptr, code, w, l);
}

struct HookSet {
    DWORD tid;
    HHOOK kb;
    HHOOK gm;
};
static std::mutex g_hookMutex;
static std::vector<HookSet> g_kbHooks;

static void InstallKbHook(DWORD tid) {
    std::lock_guard<std::mutex> lk(g_hookMutex);
    for (size_t i = 0; i < g_kbHooks.size(); i++) {
        if (g_kbHooks[i].tid == tid) {  // el tid pudo reutilizarse: reinstalar
            UnhookWindowsHookEx(g_kbHooks[i].kb);
            UnhookWindowsHookEx(g_kbHooks[i].gm);
            g_kbHooks.erase(g_kbHooks.begin() + i);
            break;
        }
    }
    HHOOK kb = SetWindowsHookExW(WH_KEYBOARD, KbProc, g_self, tid);
    HHOOK gm = SetWindowsHookExW(WH_GETMESSAGE, GetMsgProc, g_self, tid);
    if (kb && gm) {
        g_kbHooks.push_back({tid, kb, gm});
    } else {
        if (kb) UnhookWindowsHookEx(kb);
        if (gm) UnhookWindowsHookEx(gm);
    }
}

static BOOL CALLBACK EnumExplorerProc(HWND h, LPARAM) {
    WCHAR cls[64];
    DWORD pid = 0;
    DWORD tid = GetWindowThreadProcessId(h, &pid);
    if (pid == GetCurrentProcessId() && GetClassNameW(h, cls, ARRAYSIZE(cls)) && !_wcsicmp(cls, L"CabinetWClass")) InstallKbHook(tid);
    return TRUE;
}

// Manda cada hilo de ventanas del Explorador a sacar sus pestañas de la carpeta
// de resultados y espera a que terminen.
static void EvacuateAll() {
    std::vector<DWORD> tids;
    {
        std::lock_guard<std::mutex> lk(g_hookMutex);
        for (auto& p : g_kbHooks) tids.push_back(p.tid);
    }
    if (tids.empty()) return;
    g_evacDone = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_evacPending = 1;  // reserva para que ningún hilo dispare el evento antes de terminar de postear
    for (DWORD tid : tids) {
        g_evacPending++;
        if (!PostThreadMessageW(tid, g_msgEvacuate, 0, 0)) g_evacPending--;
    }
    if (--g_evacPending == 0) SetEvent(g_evacDone);
    WaitForSingleObject(g_evacDone, 3000);
    HANDLE ev = g_evacDone;
    g_evacDone = nullptr;
    CloseHandle(ev);
}

static LRESULT CALLBACK EditSubclassProc(HWND h, UINT m, WPARAM w, LPARAM l, UINT_PTR id, DWORD_PTR) {
    if (m == g_msgUnsub) {
        RemoveWindowSubclass(h, EditSubclassProc, id);
        return 0;
    }
    switch (m) {
        case WM_KEYDOWN:
            if (w == VK_RETURN && !(GetKeyState(VK_SHIFT) & 0x8000) && HandleEnter(h)) {
                g_swallowChar = true;
                return 0;
            }
            break;
        case WM_CHAR:
            if (w == L'\r' && g_swallowChar) {
                g_swallowChar = false;
                return 0;
            }
            break;
        case WM_NCDESTROY: {
            RemoveWindowSubclass(h, EditSubclassProc, id);
            std::lock_guard<std::mutex> lk(g_listMutex);
            for (size_t i = 0; i < g_subclassed.size(); i++)
                if (g_subclassed[i] == h) {
                    g_subclassed.erase(g_subclassed.begin() + i);
                    break;
                }
            break;
        }
    }
    return DefSubclassProc(h, m, w, l);
}

static bool InSearchBox(HWND parent) {
    WCHAR cls[128];
    bool hit = false;
    int d = 0;
    for (HWND p = parent; p && d < 8; p = GetAncestor(p, GA_PARENT), d++) {
        if (!GetClassNameW(p, cls, ARRAYSIZE(cls))) break;
        if (!_wcsicmp(cls, L"UniversalSearchBand") || !_wcsicmp(cls, L"Search Box") || wcsstr(cls, L"SearchEditBox")) hit = true;
    }
    return hit;
}

// ---------------------------------------------------------------------------
// UI Automation: detectar cuándo el foco entra a la caja de búsqueda nativa (XAML)
// ---------------------------------------------------------------------------

static bool HintMatches(const std::wstring& lowered) {
    std::lock_guard<std::mutex> lk(g_hintMutex);
    for (auto& h : g_hints)
        if (!h.empty() && lowered.find(h) != std::wstring::npos) return true;
    return false;
}

// Corre en un hilo interno de UIA (MTA), no en el hilo de UI.
static void OnFocus(IUIAutomationElement* el) {
    if (!g_intercept || !el) return;
    HWND fg = GetForegroundWindow();
    if (!fg) return;
    DWORD pid = 0;
    DWORD tid = GetWindowThreadProcessId(fg, &pid);
    if (pid != GetCurrentProcessId()) return;
    WCHAR cls[64];
    if (!GetClassNameW(fg, cls, ARRAYSIZE(cls)) || _wcsicmp(cls, L"CabinetWClass")) return;

    CONTROLTYPEID ct = 0;
    if (FAILED(el->get_CurrentControlType(&ct)) || ct != UIA_EditControlTypeId) return;

    BSTR name = nullptr, aid = nullptr;
    el->get_CurrentName(&name);
    el->get_CurrentAutomationId(&aid);
    RECT rc = {};
    el->get_CurrentBoundingRectangle(&rc);

    bool match = HintMatches(Lower(name ? name : L"")) || HintMatches(Lower(aid ? aid : L""));

    if (match) g_uiaMatched = true;
    if (match && GetTickCount64() - g_popupClosedTick > 800 && g_msgShowPopup) {
        std::wstring text;
        IUIAutomationValuePattern* vp = nullptr;
        if (SUCCEEDED(el->GetCurrentPatternAs(UIA_ValuePatternId, IID_PPV_ARGS(&vp))) && vp) {
            BSTR v = nullptr;
            if (SUCCEEDED(vp->get_CurrentValue(&v)) && v) {
                text = v;
                SysFreeString(v);
            }
            BSTR empty = SysAllocString(L"");
            vp->SetValue(empty);  // que la caja nativa no arranque Windows Search
            SysFreeString(empty);
            vp->Release();
        }
        PopupReq* req = new PopupReq{rc, rc.right > rc.left, text};
        if (!PostThreadMessageW(tid, g_msgShowPopup, 0, (LPARAM)req)) delete req;
    }
    SysFreeString(name);
    SysFreeString(aid);
}

class FocusHandler : public IUIAutomationFocusChangedEventHandler {
    LONG m_ref = 1;

public:
    virtual ~FocusHandler() {}
    STDMETHODIMP QueryInterface(REFIID riid, void** ppv) override {
        if (riid == IID_IUnknown || riid == __uuidof(IUIAutomationFocusChangedEventHandler)) {
            *ppv = static_cast<IUIAutomationFocusChangedEventHandler*>(this);
            AddRef();
            return S_OK;
        }
        *ppv = nullptr;
        return E_NOINTERFACE;
    }
    STDMETHODIMP_(ULONG) AddRef() override { return InterlockedIncrement(&m_ref); }
    STDMETHODIMP_(ULONG) Release() override {
        ULONG r = InterlockedDecrement(&m_ref);
        if (!r) delete this;
        return r;
    }
    STDMETHODIMP HandleFocusChangedEvent(IUIAutomationElement* sender) override {
        OnFocus(sender);
        return S_OK;
    }
};

static HANDLE g_uiaThread = nullptr;
static HANDLE g_uiaStop = nullptr;

static DWORD WINAPI UiaThreadProc(LPVOID) {
    CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    IUIAutomation* uia = nullptr;
    HRESULT hr = CoCreateInstance(__uuidof(CUIAutomation), nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&uia));
    if (SUCCEEDED(hr) && uia) {
        FocusHandler* h = new FocusHandler();
        uia->AddFocusChangedEventHandler(nullptr, h);
        WaitForSingleObject(g_uiaStop, INFINITE);
        uia->RemoveFocusChangedEventHandler(h);
        h->Release();
        uia->Release();
    }
    CoUninitialize();
    return 0;
}

static void StartUia() {
    if (g_uiaThread) return;
    g_uiaStop = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_uiaThread = CreateThread(nullptr, 0, UiaThreadProc, nullptr, 0, nullptr);
}

static void StopUia() {
    if (!g_uiaThread) return;
    SetEvent(g_uiaStop);
    // Sin límite: si se cerrara el handle con el hilo vivo, seguiría ejecutando
    // código de un DLL ya descargado.
    WaitForSingleObject(g_uiaThread, INFINITE);
    CloseHandle(g_uiaThread);
    CloseHandle(g_uiaStop);
    g_uiaThread = nullptr;
    g_uiaStop = nullptr;
}

// ---------------------------------------------------------------------------
// Win+S -> EverythingToolbar (hook de teclado de bajo nivel en un hilo propio)
// ---------------------------------------------------------------------------

static const ULONG_PTR kInjectedMarker = 0x57534554;  // marca las teclas que envía el mod
static HHOOK g_llHook = nullptr;
static HANDLE g_llThread = nullptr;
static HANDLE g_llReady = nullptr;
static DWORD g_llThreadId = 0;
static bool g_sSwallowed = false;

static bool IsProcessRunning(const std::wstring& name) {
    HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snap == INVALID_HANDLE_VALUE) return false;
    PROCESSENTRY32W pe = {};
    pe.dwSize = sizeof(pe);
    bool found = false;
    if (Process32FirstW(snap, &pe)) {
        do {
            if (!_wcsicmp(pe.szExeFile, name.c_str())) {
                found = true;
                break;
            }
        } while (Process32NextW(snap, &pe));
    }
    CloseHandle(snap);
    return found;
}

// Solo reacciona el explorer.exe dueño de la barra de tareas; si hay otros
// procesos explorer.exe (ventanas en proceso separado) no deben dispararlo de nuevo.
static bool IsShellProcess() {
    HWND taskbar = FindWindowW(L"Shell_TrayWnd", nullptr);
    if (!taskbar) return false;
    DWORD pid = 0;
    GetWindowThreadProcessId(taskbar, &pid);
    return pid == GetCurrentProcessId();
}

static void SendShortcut(const std::vector<WORD>& keys) {
    std::vector<INPUT> inputs;
    auto add = [&](WORD vk, bool up) {
        INPUT in = {};
        in.type = INPUT_KEYBOARD;
        in.ki.wVk = vk;
        in.ki.dwFlags = up ? KEYEVENTF_KEYUP : 0;
        in.ki.dwExtraInfo = kInjectedMarker;
        inputs.push_back(in);
    };
    // Win ya está apretada físicamente: solo se envía el resto.
    for (size_t i = 0; i < keys.size(); i++) add(keys[i], false);
    for (size_t i = keys.size(); i-- > 0;) add(keys[i], true);
    SendInput((UINT)inputs.size(), inputs.data(), sizeof(INPUT));
}

static bool IsDown(int vk) {
    return (GetAsyncKeyState(vk) & 0x8000) != 0;
}

// El hook de bajo nivel tiene que responder en pocos milisegundos: si se pasa del
// tiempo límite del sistema, Windows deja pasar la tecla y se abre la búsqueda
// nativa encima de EverythingToolbar. Por eso el hook solo decide y descarta la S;
// el envío de teclas y la búsqueda del proceso requerido se hacen después, en el
// bucle de mensajes del mismo hilo.
static const UINT kMsgSendWinS = WM_APP + 3;
static DWORD g_shellPid = 0;

static bool IsShellProcessCached() {
    if (!g_shellPid && IsShellProcess()) g_shellPid = GetCurrentProcessId();
    return g_shellPid != 0;
}

static bool ShouldHandleWinS() {
    if (!(IsDown(VK_LWIN) || IsDown(VK_RWIN))) return false;
    if (IsDown(VK_SHIFT) || IsDown(VK_CONTROL) || IsDown(VK_MENU)) return false;
    {
        std::lock_guard<std::mutex> lk(g_winSMutex);
        if (!g_winS.enabled || !g_winS.valid) return false;
    }
    return IsShellProcessCached();
}

static void SendWinS() {
    WinSConfig cfg;
    {
        std::lock_guard<std::mutex> lk(g_winSMutex);
        cfg = g_winS;
    }
    // Sin el proceso requerido se reenvía la S marcada: Win sigue apretada y
    // Windows abre su búsqueda como si nada.
    if (!cfg.requiredProcess.empty() && !IsProcessRunning(cfg.requiredProcess))
        SendShortcut({'S'});
    else
        SendShortcut(cfg.keys);
}

static LRESULT CALLBACK LowLevelKeyboardProc(int code, WPARAM wParam, LPARAM lParam) {
    if (code == HC_ACTION) {
        const KBDLLHOOKSTRUCT* kb = (const KBDLLHOOKSTRUCT*)lParam;
        if (kb->dwExtraInfo != kInjectedMarker && kb->vkCode == 'S') {
            bool down = wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN;
            bool up = wParam == WM_KEYUP || wParam == WM_SYSKEYUP;
            if (down) {
                if (g_sSwallowed) return 1;  // autorrepetición de la S descartada
                if (ShouldHandleWinS()) {
                    g_sSwallowed = true;
                    PostThreadMessageW(g_llThreadId, kMsgSendWinS, 0, 0);
                    return 1;
                }
            } else if (up && g_sSwallowed) {
                g_sSwallowed = false;
                return 1;
            }
        }
    }
    return CallNextHookEx(nullptr, code, wParam, lParam);
}

static DWORD WINAPI LowLevelHookThread(LPVOID) {
    MSG msg;
    PeekMessageW(&msg, nullptr, WM_USER, WM_USER, PM_NOREMOVE);  // crea la cola de mensajes
    g_llThreadId = GetCurrentThreadId();
    g_llHook = SetWindowsHookExW(WH_KEYBOARD_LL, LowLevelKeyboardProc, GetModuleHandleW(nullptr), 0);
    SetEvent(g_llReady);
    if (!g_llHook) return 1;
    while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
        if (!msg.hwnd && msg.message == kMsgSendWinS) {
            SendWinS();
            continue;
        }
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
    UnhookWindowsHookEx(g_llHook);
    g_llHook = nullptr;
    return 0;
}

static void StartWinS() {
    if (g_llThread) return;
    g_llReady = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_llThread = CreateThread(nullptr, 0, LowLevelHookThread, nullptr, 0, nullptr);
    if (!g_llThread) {
        CloseHandle(g_llReady);
        g_llReady = nullptr;
        return;
    }
    WaitForSingleObject(g_llReady, 2000);
}

static void StopWinS() {
    if (!g_llThread) return;
    if (g_llThreadId) PostThreadMessageW(g_llThreadId, WM_QUIT, 0, 0);
    WaitForSingleObject(g_llThread, 3000);
    CloseHandle(g_llThread);
    CloseHandle(g_llReady);
    g_llThread = nullptr;
    g_llReady = nullptr;
    g_llThreadId = 0;
    g_sSwallowed = false;
}

static void ApplyWinS() {
    bool on;
    {
        std::lock_guard<std::mutex> lk(g_winSMutex);
        on = g_winS.enabled && g_winS.valid;
    }
    if (on)
        StartWinS();
    else
        StopWinS();
}

// ---------------------------------------------------------------------------
// Enganche de la caja de búsqueda clásica y de las ventanas nuevas
// ---------------------------------------------------------------------------

using CreateWindowExW_t = decltype(&CreateWindowExW);
static CreateWindowExW_t CreateWindowExW_Original;

static HWND WINAPI CreateWindowExW_Hook(DWORD ex, LPCWSTR cls, LPCWSTR name, DWORD style, int x, int y, int w, int h,
                                        HWND parent, HMENU menu, HINSTANCE inst, LPVOID param) {
    HWND hwnd = CreateWindowExW_Original(ex, cls, name, style, x, y, w, h, parent, menu, inst, param);
    if (hwnd && !parent && !IS_INTRESOURCE(cls) && !_wcsicmp(cls, L"CabinetWClass")) InstallKbHook(GetCurrentThreadId());
    if (hwnd && parent && !IS_INTRESOURCE(cls) && !_wcsicmp(cls, L"Edit")) {
        if (InSearchBox(parent)) {
            if (SetWindowSubclass(hwnd, EditSubclassProc, 1, 0)) {
                std::lock_guard<std::mutex> lk(g_listMutex);
                g_subclassed.push_back(hwnd);
            }
        }
    }
    return hwnd;
}

// ---------------------------------------------------------------------------
// Ciclo de vida del mod
// ---------------------------------------------------------------------------

BOOL Wh_ModInit() {
    LoadSettings();
    g_msgUnsub = RegisterWindowMessageW(L"EverythingExplorerSearch_Unsub");
    g_msgShowPopup = RegisterWindowMessageW(L"EverythingExplorerSearch_ShowPopup");
    g_msgEvacuate = RegisterWindowMessageW(L"EverythingExplorerSearch_Evacuate");
    if (g_disableIndexWarning) DisableIndexWarning();
    LoadEverything();
    RegisterFolderClass();
    Wh_SetFunctionHook((void*)CreateWindowExW, (void*)CreateWindowExW_Hook, (void**)&CreateWindowExW_Original);
    EnumWindows(EnumExplorerProc, 0);  // ventanas que ya estaban abiertas
    if (g_intercept) StartUia();
    ApplyWinS();
    return TRUE;
}

void Wh_ModSettingsChanged() {
    LoadSettings();
    if (g_disableIndexWarning) DisableIndexWarning();
    LoadEverything();
    if (g_intercept)
        StartUia();
    else
        StopUia();
    ApplyWinS();
}

void Wh_ModUninit() {
    StopUia();
    StopWinS();
    if (g_popup) {
        DWORD_PTR res = 0;
        SendMessageTimeoutW(g_popup, WM_CLOSE, 0, 0, SMTO_ABORTIFHUNG, 1000, &res);
    }
    // La carpeta virtual vive en este DLL: ninguna pestaña puede quedar mostrándola.
    EvacuateAll();
    // Quitar el subclass en el hilo dueño de cada ventana antes de que se descargue el código.
    std::vector<HWND> list;
    {
        std::lock_guard<std::mutex> lk(g_listMutex);
        list = g_subclassed;
    }
    for (HWND h : list) {
        DWORD_PTR res = 0;
        SendMessageTimeoutW(h, g_msgUnsub, 0, 0, SMTO_ABORTIFHUNG, 1000, &res);
    }
    {
        std::lock_guard<std::mutex> lk(g_hookMutex);
        for (auto& p : g_kbHooks) {
            UnhookWindowsHookEx(p.kb);
            UnhookWindowsHookEx(p.gm);
        }
        g_kbHooks.clear();
    }
    if (g_popupClassRegistered) {
        UnregisterClassW(kPopupClass, g_self);
        g_popupClassRegistered = false;
    }
    if (g_notifyClassRegistered) UnregisterClassW(kNotifyClass, g_self);
    UnregisterFolderClass();
    std::lock_guard<std::mutex> lk(g_evMutex);
    if (g_ev.h) FreeLibrary(g_ev.h);
    g_ev = EvApi{};
}
