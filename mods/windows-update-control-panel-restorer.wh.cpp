// ==WindhawkMod==
// @id              windows-update-control-panel-restorer
// @name            Windows Update Control Panel Restorer
// @description     This mod restores the Windows Update Control Panel page in Windows 10 and Windows 11
// @version         1.0.0
// @author          babamohammed
// @github          https://github.com/babamohammed2022
// @include         explorer.exe
// @include         control.exe
// @architecture    x86-64
// @compilerOptions -lwininet -ladvapi32 -lcrypt32 -lole32 -luuid -loleaut32 -lgdi32
// ==/WindhawkMod==

// ==WindhawkModSettings==
/*
- Language: auto
  $name: Language
  $description: This setting changes the language shown on the restored page. "Auto" detects the system language automatically; otherwise English is used as the fallback if a language is not recognized.
  $options:
    - auto: Auto (detect system language)
    - en: English
    - it: Italiano
    - es: Español
    - fr: Français
    - tr: Türkçe
    - ru: Русский
    - pt: Português
    - zh: 中文
    - pl: Polski
    - nl: Nederlands
- ShowServiceNotice: true
  $name: Show recreated interface
  $description: This setting shows the mod's recreated interface, a best-effort recreation of the classic Windows Update Control Panel page. Disable this to hide the recreated interface.
- UpdatePageSkin: windows7
  $name: Update page skin
  $description: This setting chooses the status banner and applet icon skin. Windows 7 keeps the current shield-style status icons and uses the supplied applet logo; Windows 8.1 uses the included Windows Update icon. The available-updates and disabled-service/fallback notices are unchanged.
  $options:
    - windows7: Windows 7 (current)
    - windows81: Windows 8.1
- ShowAvailableUpdates: true
  $name: Show available-updates banner
  $description: This setting shows the "updates available" state (amber/orange strip with an exclamation shield) when Windows reports pending available updates. Disabled by default.
- LinkSystemSettingsText: false
  $name: Link system settings text
  $description: This setting makes the "system settings" part of the recommendation text a blue link that opens Windows Update in the Settings app (ms-settings:windowsupdate). Disabled by default.
- DebugForcePendingUpdate: false
  $name: Debug - force pending-updates state
  $description: This setting is a debug/preview option that forces the "pending updates" interface even when Windows reports no pending update. Disabled by default.
*/
// ==/WindhawkModSettings==

// ==WindhawkModReadme==
/*
# Windows Update Control Panel Restorer

This mod adds a best-effort classic Windows Update page back to Control Panel on
Windows 10 and Windows 11. It uses a private Windows 8.1 UI payload with a modern
Windows Update backend/status layer, without replacing system files or writing
real Control Panel registration keys.

This is a reimplementation, not the original Windows Update client. Some buttons
and small visual details are intentionally limited, and more details may be
improved in future versions.

**Features**

- Classic Windows Update-style status page with real restart/update status.
- Windows 7 or Windows 8.1 skin option for the status/app icon.
- Friendly notice when Windows Update is disabled or unavailable.
- Optional available-updates banner and optional link to Windows Update Settings.
- Multilingual UI: English, Italian, Spanish, French, Turkish, Russian,
  Portuguese, Chinese, Polish, Dutch, or auto-detect.

**Notes**

- Installing updates is still handled by the modern Settings app.
- The "last checked" time is the moment this mod queried the system, so it can
  differ slightly from the Settings app.
- The Windows 8.1 UI DLL is downloaded from Microsoft Symbol Server and verified
  before use.

**Credits**

- **Yvor** - Testing on Windows 10 21H2.
- **Cips** - Testing on Windows 11 25H2.

If you encounter issues, please report them to the author of the mod.
*/
// ==/WindhawkModReadme==

#include <windows.h>
#include <windowsx.h>  // GET_Y_LPARAM
#include <wininet.h>
#include <wincrypt.h>
#include <combaseapi.h>
#include <winnls.h>
#include <winsvc.h>
#include <shlobj.h>
#include <shellapi.h>
#include <objidl.h>
#include <oaidl.h>
#include <oleauto.h>
#include <cstdlib>
#include <cstring>
#include <string>
#include <unordered_map>
#include <memory>
#include <mutex>
#include <shared_mutex>
#include <thread>
#include <atomic>
#include <optional>
#include <vector>
#include <windhawk_utils.h>

// =============================================================================
// Windows Update API (WUA) Headers and Implementation
// =============================================================================
#include <wuapi.h>
#include <wuerror.h>
#include <comdef.h>

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunknown-pragmas"
#pragma comment(lib, "ole32.lib")
#pragma comment(lib, "uuid.lib")
#pragma comment(lib, "oleaut32.lib")
#pragma clang diagnostic pop

// -----------------------------------------------------------------------------
// WUA Data Structures
// -----------------------------------------------------------------------------
struct WuaUpdateInfo {
    std::wstring title;
    std::wstring kb;
    std::wstring description;
    std::wstring msrcSeverity;
    ULONGLONG maxDownloadSize = 0;
    bool mandatory = false;
    bool isImportant = false;
    bool isInstalled = false;
    bool isOptional = false;
};

struct WuaSearchResult {
    HRESULT hr = E_FAIL;
    std::vector<WuaUpdateInfo> updates;
    std::wstring lastChecked;
    int importantCount = 0;
    int optionalCount = 0;
};

struct WuaHistoryEntry {
    std::wstring title;
    DATE date = 0;
    OperationResultCode resultCode = orcNotStarted;
    int errorCode = 0;
};

// -----------------------------------------------------------------------------
// WUA Helper Functions
// -----------------------------------------------------------------------------
static std::wstring BstrToWString(BSTR value) {
    return value ? std::wstring(value, SysStringLen(value)) : L"";
}

static std::wstring GetKbList(IUpdate* update) {
    IStringCollection* kbIds = nullptr;
    if (FAILED(update->get_KBArticleIDs(&kbIds)) || !kbIds) {
        return L"";
    }
    LONG count = 0;
    kbIds->get_Count(&count);
    std::wstring result;
    for (LONG i = 0; i < count; ++i) {
        BSTR kb = nullptr;
        if (SUCCEEDED(kbIds->get_Item(i, &kb)) && kb) {
            if (!result.empty()) {
                result += L", ";
            }
            result += L"KB";
            result += BstrToWString(kb);
            SysFreeString(kb);
        }
    }
    kbIds->Release();
    return result;
}

static std::wstring GetMsrcSeverity(IUpdate* update) {
    BSTR severity = nullptr;
    if (SUCCEEDED(update->get_MsrcSeverity(&severity)) && severity) {
        std::wstring result = BstrToWString(severity);
        SysFreeString(severity);
        return result;
    }
    return L"";
}

static bool IsUpdateImportant(IUpdate* update) {
    ICategoryCollection* categories = nullptr;
    if (FAILED(update->get_Categories(&categories)) || !categories) {
        return false;
    }
    
    LONG count = 0;
    categories->get_Count(&count);
    bool isImportant = false;
    
    for (LONG i = 0; i < count && !isImportant; ++i) {
        ICategory* category = nullptr;
        if (SUCCEEDED(categories->get_Item(i, &category)) && category) {
            BSTR name = nullptr;
            if (SUCCEEDED(category->get_Name(&name)) && name) {
                std::wstring categoryName = BstrToWString(name);
                // Check for common important update category patterns
                if (categoryName.find(L"Security") != std::wstring::npos ||
                    categoryName.find(L"Critical") != std::wstring::npos ||
                    categoryName.find(L"Important") != std::wstring::npos ||
                    categoryName.find(L"Update") != std::wstring::npos) {
                    isImportant = true;
                }
                SysFreeString(name);
            }
            category->Release();
        }
    }
    categories->Release();
    return isImportant;
}

// -----------------------------------------------------------------------------
// Search for Available Updates using WUA
// -----------------------------------------------------------------------------
// Conservative RAII guard for a short-lived COM apartment on this thread.
// CoInitializeEx can return S_FALSE / RPC_E_CHANGED_MODE when the thread is
// already in an apartment — in that case we must NOT call CoUninitialize. The
// guard tracks that and uninitializes only when it actually initialized.
class ComGuard {
public:
    ComGuard() {
        HRESULT hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
        mustUninit_ = SUCCEEDED(hr);
    }
    ComGuard(const ComGuard&) = delete;
    ComGuard& operator=(const ComGuard&) = delete;
    ~ComGuard() {
        if (mustUninit_) CoUninitialize();
    }
private:
    bool mustUninit_ = false;
};

[[maybe_unused]] static WuaSearchResult SearchAvailableUpdates() {
    WuaSearchResult result;

    ComGuard comGuard; // RAII: uninitializes COM on scope exit (only if we initialized it)
    
    IUpdateSession* session = nullptr;
    IUpdateSearcher* searcher = nullptr;
    ISearchResult* searchResult = nullptr;
    
    result.hr = CoCreateInstance(
        __uuidof(UpdateSession),
        nullptr,
        CLSCTX_INPROC_SERVER,
        __uuidof(IUpdateSession),
        reinterpret_cast<void**>(&session));
    
    if (FAILED(result.hr)) {
        return result;
    }
    
    // Set client application ID for tracking
    BSTR clientId = SysAllocString(L"Windhawk Windows Update Control Panel");
    session->put_ClientApplicationID(clientId);
    SysFreeString(clientId);
    
    result.hr = session->CreateUpdateSearcher(&searcher);
    if (FAILED(result.hr)) {
        session->Release();
        return result;
    }
    
    // Search for updates that are not installed and not hidden
    BSTR criteria = SysAllocString(L"IsInstalled=0 and IsHidden=0");
    result.hr = searcher->Search(criteria, &searchResult);
    SysFreeString(criteria);
    
    if (FAILED(result.hr)) {
        searcher->Release();
        session->Release();
        return result;
    }
    
    // Note: ISearchResult doesn't have get_LastSearchTime in the standard API
    // The last check time would need to be obtained from registry or another source
    
    IUpdateCollection* collection = nullptr;
    result.hr = searchResult->get_Updates(&collection);
    
    if (SUCCEEDED(result.hr) && collection) {
        LONG count = 0;
        collection->get_Count(&count);
        
        for (LONG i = 0; i < count; ++i) {
            IUpdate* update = nullptr;
            if (FAILED(collection->get_Item(i, &update)) || !update) {
                continue;
            }
            
            WuaUpdateInfo info;
            
            // Get title
            BSTR title = nullptr;
            if (SUCCEEDED(update->get_Title(&title)) && title) {
                info.title = BstrToWString(title);
                SysFreeString(title);
            }
            
            // Get KB article IDs
            info.kb = GetKbList(update);
            
            // Get MSRC severity (for important updates)
            info.msrcSeverity = GetMsrcSeverity(update);
            
            // Get download size (returns DECIMAL, convert to bytes)
            DECIMAL sizeDec;
            sizeDec.Lo64 = 0;
            if (SUCCEEDED(update->get_MaxDownloadSize(&sizeDec))) {
                info.maxDownloadSize = sizeDec.Lo64;
            }
            
            // Check if mandatory
            VARIANT_BOOL mandatory = VARIANT_FALSE;
            update->get_IsMandatory(&mandatory);
            info.mandatory = mandatory == VARIANT_TRUE;
            
            // Determine if important or optional
            info.isImportant = IsUpdateImportant(update);
            if (!info.isImportant) {
                info.isOptional = true;
                result.optionalCount++;
            } else {
                result.importantCount++;
            }
            
            // Get description
            BSTR description = nullptr;
            if (SUCCEEDED(update->get_Description(&description)) && description) {
                info.description = BstrToWString(description);
                SysFreeString(description);
            }
            
            result.updates.push_back(std::move(info));
            update->Release();
        }
        collection->Release();
    }
    
    searchResult->Release();
    searcher->Release();
    session->Release();
    
    return result;
}

// -----------------------------------------------------------------------------
// Get Update History using WUA
// -----------------------------------------------------------------------------
static std::vector<WuaHistoryEntry> GetUpdateHistory(int maxEntries = 100) {
    std::vector<WuaHistoryEntry> history;
    
    ComGuard comGuard; // RAII: uninitializes COM on scope exit (only if we initialized it)

    IUpdateSession* session = nullptr;
    IUpdateSearcher* searcher = nullptr;
    IUpdateHistoryEntryCollection* entries = nullptr;
    
    HRESULT hr = CoCreateInstance(
        __uuidof(UpdateSession),
        nullptr,
        CLSCTX_INPROC_SERVER,
        __uuidof(IUpdateSession),
        reinterpret_cast<void**>(&session));
    
    if (FAILED(hr)) {
        return history;
    }
    
    hr = session->CreateUpdateSearcher(&searcher);
    if (FAILED(hr)) {
        session->Release();
        return history;
    }
    
    LONG total = 0;
    hr = searcher->GetTotalHistoryCount(&total);
    
    if (SUCCEEDED(hr) && total > 0) {
        LONG requestCount = total > maxEntries ? maxEntries : total;
        hr = searcher->QueryHistory(0, requestCount, &entries);
        
        if (SUCCEEDED(hr) && entries) {
            LONG count = 0;
            entries->get_Count(&count);
            
            for (LONG i = 0; i < count; ++i) {
                IUpdateHistoryEntry* entry = nullptr;
                if (FAILED(entries->get_Item(i, &entry)) || !entry) {
                    continue;
                }
                
                WuaHistoryEntry hist;
                
                BSTR title = nullptr;
                if (SUCCEEDED(entry->get_Title(&title)) && title) {
                    hist.title = BstrToWString(title);
                    SysFreeString(title);
                }
                
                entry->get_Date(&hist.date);
                entry->get_ResultCode(&hist.resultCode);
                
                // Get error code if failed
                if (hist.resultCode == orcFailed) {
                    LONG error = 0;
                    entry->get_HResult(&error);
                    hist.errorCode = error;
                }
                
                history.push_back(std::move(hist));
                entry->Release();
            }
            entries->Release();
        }
    }
    
    searcher->Release();
    session->Release();
    
    return history;
}

// -----------------------------------------------------------------------------
// Open Installed Updates Page
// -----------------------------------------------------------------------------
static void OpenInstalledUpdates(HWND hwnd) {
    ShellExecuteW(
        hwnd,
        L"open",
        L"explorer.exe",
        L"shell:::{D450A8A1-9568-45C7-9C0E-B4F9FB4537BD}",
        nullptr,
        SW_SHOWNORMAL);
}

// -----------------------------------------------------------------------------
// Format file size for display
// -----------------------------------------------------------------------------
[[maybe_unused]] static std::wstring FormatFileSize(ULONGLONG bytes) {
    if (bytes == 0) return L"0 B";
    
    const double kb = static_cast<double>(bytes) / 1024.0;
    const double mb = kb / 1024.0;
    const double gb = mb / 1024.0;
    
    wchar_t buffer[64];
    if (gb >= 1.0) {
        swprintf_s(buffer, L"%.1f GB", gb);
    } else if (mb >= 1.0) {
        swprintf_s(buffer, L"%.1f MB", mb);
    } else if (kb >= 1.0) {
        swprintf_s(buffer, L"%.1f KB", kb);
    } else {
        swprintf_s(buffer, L"%llu B", bytes);
    }
    return std::wstring(buffer);
}

// -----------------------------------------------------------------------------
// Show Update Search Results in a Message Box (for testing/debugging)
// -----------------------------------------------------------------------------
static void ShowUpdateHistory(HWND hwnd) {
    std::vector<WuaHistoryEntry> history = GetUpdateHistory(50);
    
    if (history.empty()) {
        MessageBoxW(hwnd, L"No update history found.", L"Windows Update History", 
                    MB_ICONINFORMATION | MB_OK);
        return;
    }
    
    std::wstring text = L"Recent Update History:\n\n";
    
    for (const WuaHistoryEntry& entry : history) {
        SYSTEMTIME st{};
        VariantTimeToSystemTime(entry.date, &st);
        
        wchar_t dateBuf[64];
        swprintf_s(dateBuf, L"%04u-%02u-%02u", st.wYear, st.wMonth, st.wDay);
        
        text += dateBuf;
        text += L" - ";
        
        // Result code description
        switch (entry.resultCode) {
            case orcNotStarted: text += L"Not started"; break;
            case orcInProgress: text += L"In progress"; break;
            case orcSucceeded: text += L"Succeeded"; break;
            case orcSucceededWithErrors: text += L"Succeeded with errors"; break;
            case orcFailed: 
                text += L"Failed";
                if (entry.errorCode != 0) {
                    wchar_t errBuf[32];
                    swprintf_s(errBuf, L" (0x%08X)", entry.errorCode);
                    text += errBuf;
                }
                break;
            case orcAborted: text += L"Aborted"; break;
            default: text += L"Unknown"; break;
        }
        
        text += L"\n  " + entry.title + L"\n\n";
    }
    
    MessageBoxW(hwnd, text.c_str(), L"Windows Update History", 
                MB_ICONINFORMATION | MB_OK);
}

// -----------------------------------------------------------------------------
// Private, verified Windows 8.1 UI payload.
// wucltux.dll 7.9.9600.17415 (winblue_r4.141028-1500), x64.
// Source: Microsoft Symbol Server. The blob redirect is normal for msdl URLs.
// -----------------------------------------------------------------------------
static const wchar_t* kDllName = L"wucltux.dll";
static const wchar_t* kDownloadUrl =
    L"https://msdl.microsoft.com/download/symbols/wucltux.dll/"
    L"54503A411a9000/wucltux.dll";
static const wchar_t* kExpectedSha256 =
    L"2B9928A0928D73786F68166B3EF785C0055BD6E73C5583913703A5D8DF61BE4C";
static const DWORD kMinDllSize = 65536;
static const DWORD kDownloadTimeoutMs = 20000;
static const int kMaxDownloadAttempts = 3;
static const DWORD kRetryDelayMs = 3000;

// Windows Update's classic Control Panel namespace item.
static const wchar_t* kAppletClsid = L"{36eef7db-88ad-4e81-ad49-0e313f0c35f8}";
static const wchar_t* kLayoutFolderClsid = L"{328B0346-7EAF-4BBE-A479-7CB88A095F5B}";
static const wchar_t* kDisplayName = L"Windows Update";
static const wchar_t* kApplicationName = L"Microsoft.WindowsUpdate";
// XMLFILE resource 100 names this COM element provider. It is required to
// construct the legacy DirectUI page; omitting it produces "Unable to load page".
static const wchar_t* kElementProviderClsid = L"{cfbc05bc-1b9e-4693-a49c-4e7181d69e0a}";
static const DWORD kShellFolderAttributes = 0xA0000000;
static const DWORD kInitResourceId = 100;

static const GUID kAppletFolderGuid = {0x36eef7db, 0x88ad, 0x4e81,
                                       {0xad, 0x49, 0x0e, 0x31, 0x3f, 0x0c, 0x35, 0xf8}};
static const GUID kElementProviderGuid = {0xcfbc05bc, 0x1b9e, 0x4693,
                                         {0xa4, 0x9c, 0x4e, 0x71, 0x81, 0xd6, 0x9e, 0x0a}};
static const IID IID_IClassFactory_GUID = {0x00000001, 0x0000, 0x0000,
                                           {0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46}};

static std::atomic<bool> g_verified{false};
static std::atomic<HMODULE> g_module{nullptr};
static std::atomic<const std::wstring*> g_dllPath{nullptr};
static std::atomic<bool> g_stopping{false};
static HANDLE g_stopEvent = nullptr;
// DirectUI resolves resstr(...) via XResourceProvider, bypassing LoadStringW.
// This private resource copy supplies the embedded MUI string blocks to it.
static std::mutex g_resourceMutex;
static std::wstring g_resourcePath;
static std::atomic<HMODULE> g_resourceModule{nullptr};
[[clang::no_destroy]] static std::optional<std::thread> g_setupThread;

// The language the currently-loaded embedded MUI resource module was built for.
// When the user changes the language, the module must be rebuilt and reloaded so
// the classic page actually shows the new strings (it is built once at startup).
static std::wstring g_builtLanguage = L"en";
static std::mutex g_rebuildMutex;
[[clang::no_destroy]] static std::optional<std::thread> g_rebuildThread;

// Which icon skin to use for the normal Windows Update status banner.
// 0 = Windows 7/current shield/check icons, 1 = Windows 8.1 update icon.
static constexpr int kUpdatePageSkinWindows7 = 0;
static constexpr int kUpdatePageSkinWindows81 = 1;
static std::atomic<int> g_updatePageSkin{kUpdatePageSkinWindows7};
static bool IsWindows81Skin() {
    return g_updatePageSkin.load() == kUpdatePageSkinWindows81;
}

// Whether to show the "updates available" (amber) banner when Windows reports
// pending available updates. Controlled only by the "ShowAvailableUpdates"
// setting (default off); no global hotkey is registered.
static std::atomic<bool> g_showAvailableUpdates{false};

// Optional, conservative bridge to the modern Settings app. When enabled, only
// the translated "system settings" phrase in the up-to-date recommendation is
// made clickable; the text is otherwise unchanged. Disabled by default.
static std::atomic<bool> g_linkSystemSettingsText{false};

// Debug/preview flag: when set, the restored page renders the "pending
// updates" interface (orange strip, "Pending restart", shield icon) even when
// Windows reports no pending update. Controlled by the
// "DebugForcePendingUpdate" setting (default off).
static std::atomic<bool> g_debugForcePending{false};

// Currently selected language code (default "en"). Declared early because the
// embedded string table below resolves strings per language at runtime. Loaded
// from the mod settings in LoadLanguageSetting().
static std::wstring g_language = L"en";

// Whether to show the mod's "service not available" notice (the shield box).
// Controlled by the "ShowServiceNotice" setting (default on).
static std::atomic<bool> g_showServiceNotice{true};

// Cached "last check" timestamp: the moment the mod last queried the system for
// available updates (see LastCheckForUpdatesText). Kept as a formatted string.
static std::wstring g_lastQueryTimeText;

// Cache of the most recent DirectUI parser + base XML for the Windows Update
// page, used to re-render it in place with updated status data (so "Check for
// updates" refreshes the box instead of popping a debug dialog).
static void* g_wuParser = nullptr;
static HINSTANCE g_wuResModule = nullptr;
static HINSTANCE g_wuHInstance = nullptr;
static std::wstring g_wuBaseXml;
static std::mutex g_wuRenderMutex;

// Embedded from the matching Windows 8.1 en-US wucltux.dll.mui string table,
// expanded to ten languages: en, it, es, fr, tr, ru, pt, zh, pl, nl.
struct WucltuxEmbeddedString {
    UINT id;
    const wchar_t* en;
    const wchar_t* it;
    const wchar_t* es;
    const wchar_t* fr;
    const wchar_t* tr;
    const wchar_t* ru;
    const wchar_t* pt;
    const wchar_t* zh;
    const wchar_t* pl;
    const wchar_t* nl;
};
static const WucltuxEmbeddedString kWucltuxMuiStrings[] = {
    { 1, L"Windows Update", L"Windows Update", L"Windows Update", L"Windows Update", L"Windows Update", L"Центр обновления Windows", L"Windows Update", L"Windows 更新", L"Windows Update", L"Windows Update" },
    { 2, L"Delivers software updates and drivers, and provides automatic updating options.", L"Fornisce aggiornamenti software e driver e offre opzioni di aggiornamento automatico.", L"Proporciona actualizaciones de software y controladores, y ofrece opciones de actualización automática.", L"Fournit les mises à jour logicielles et les pilotes, et propose des options de mise à jour automatique.", L"Yazılım güncellemeleri ve sürücüler sunar ve otomatik güncelleme seçenekleri sağlar.", L"Поставляет обновления программного обеспечения и драйверов, а также предоставляет параметры автоматического обновления.", L"Fornece atualizações de software e drivers e oferece opções de atualização automática.", L"提供软件更新和驱动程序，并提供自动更新选项。", L"Dostarcza aktualizacje oprogramowania i sterowników oraz udostępnia opcje automatycznej aktualizacji.", L"Levert software-updates en stuurprogramma's en biedt opties voor automatische updates." },
    { 3, L"Windows Update", L"Windows Update", L"Windows Update", L"Windows Update", L"Windows Update", L"Центр обновления Windows", L"Windows Update", L"Windows 更新", L"Windows Update", L"Windows Update" },
    { 4, L"Check for software and driver updates, choose automatic updating settings, or view installed updates.", L"Cerca aggiornamenti di software e driver, scegli le impostazioni di aggiornamento automatico o visualizza gli aggiornamenti installati.", L"Busca actualizaciones de software y controladores, elige la configuración de actualización automática o consulta las actualizaciones instaladas.", L"Recherchez les mises à jour logicielles et de pilotes, choisissez les paramètres de mise à jour automatique ou consultez les mises à jour installées.", L"Yazılım ve sürücü güncellemelerini denetleyin, otomatik güncelleme ayarlarını seçin veya yüklü güncellemeleri görüntüleyin.", L"Проверьте наличие обновлений программного обеспечения и драйверов, выберите параметры автоматического обновления или просмотрите установленные обновления.", L"Verifique atualizações de software e drivers, escolha as configurações de atualização automática ou consulte as atualizações instaladas.", L"检查软件和驱动程序更新、选择自动更新设置或查看已安装的更新。", L"Sprawdź aktualizacje oprogramowania i sterowników, wybierz ustawienia automatycznej aktualizacji lub wyświetl zainstalowane aktualizacje.", L"Controleer op software- en stuurprogramma-updates, kies automatische update-instellingen of bekijk geïnstalleerde updates." },
    { 71, L"Windows Update", L"Windows Update", L"Windows Update", L"Windows Update", L"Windows Update", L"Центр обновления Windows", L"Windows Update", L"Windows 更新", L"Windows Update", L"Windows Update" },
    { 73, L"Change settings", L"Cambia impostazioni", L"Cambiar la configuración", L"Modifier les paramètres", L"Ayarları değiştir", L"Изменить параметры", L"Alterar configurações", L"更改设置", L"Zmień ustawienia", L"Instellingen wijzigen" },
    { 74, L"View update history", L"Visualizza cronologia aggiornamenti", L"Ver historial de actualizaciones", L"Afficher l'historique des mises à jour", L"Güncelleme geçmişini görüntüle", L"Просмотр журнала обновлений", L"Exibir histórico de atualizações", L"查看更新历史记录", L"Wyświetl historię aktualizacji", L"Updategeschiedenis weergeven" },
    { 75, L"Restore hidden updates", L"Ripristina aggiornamenti nascosti", L"Restaurar actualizaciones ocultas", L"Restaurer les mises à jour masquées", L"Gizli güncellemeleri geri yükle", L"Восстановить скрытые обновления", L"Restaurar atualizações ocultas", L"还原隐藏的更新", L"Przywróć ukryte aktualizacje", L"Verborgen updates herstellen" },
    { 78, L"Select updates to install", L"Seleziona gli aggiornamenti da installare", L"Seleccionar las actualizaciones que se van a instalar", L"Sélectionner les mises à jour à installer", L"Yüklenecek güncellemeleri seçin", L"Выбор обновлений для установки", L"Selecione as atualizações a instalar", L"选择要安装的更新", L"Wybierz aktualizacje do zainstalowania", L"Updates selecteren om te installeren" },
    { 81, L"1 important update", L"1 aggiornamento importante", L"1 actualización importante", L"1 mise à jour importante", L"1 önemli güncelleme", L"1 важное обновление", L"1 atualização importante", L"1 个重要更新", L"1 ważna aktualizacja", L"1 belangrijke update" },
    { 82, L"%d important updates", L"%d aggiornamenti importanti", L"%d actualizaciones importantes", L"%d mises à jour importantes", L"%d önemli güncelleme", L"%d важных обновлений", L"%d atualizações importantes", L"%d 个重要更新", L"%d ważnych aktualizacji", L"%d belangrijke updates" },
    { 83, L"%1, %2", L"%1, %2", L"%1, %2", L"%1, %2", L"%1, %2", L"%1, %2", L"%1, %2", L"%1，%2", L"%1, %2", L"%1, %2" },
    { 84, L"1 update", L"1 aggiornamento", L"1 actualización", L"1 mise à jour", L"1 güncelleme", L"1 обновление", L"1 atualização", L"1 个更新", L"1 aktualizacja", L"1 update" },
    { 85, L"%d updates", L"%d aggiornamenti", L"%d actualizaciones", L"%d mises à jour", L"%d güncelleme", L"%d обновлений", L"%d atualizações", L"%d 个更新", L"%d aktualizacje", L"%d updates" },
    { 86, L"%s MB", L"%s MB", L"%s MB", L"%s Mo", L"%s MB", L"%s МБ", L"%s MB", L"%s MB", L"%s MB", L"%s MB" },
    { 87, L"%s KB", L"%s KB", L"%s KB", L"%s Ko", L"%s KB", L"%s КБ", L"%s KB", L"%s KB", L"%s KB", L"%s KB" },
    { 88, L"1 hour", L"1 ora", L"1 hora", L"1 heure", L"1 saat", L"1 час", L"1 hora", L"1 小时", L"1 godzina", L"1 uur" },
    { 89, L"%1!lu! hours", L"%1!lu! ore", L"%1!lu! horas", L"%1!lu! heures", L"%1!lu! saat", L"%1!lu! ч.", L"%1!lu! horas", L"%1!lu! 小时", L"%1!lu! godz.", L"%1!lu! uur" },
    { 92, L"%1!lu! minutes", L"%1!lu! minuti", L"%1!lu! minutos", L"%1!lu! minutes", L"%1!lu! dakika", L"%1!lu! мин.", L"%1!lu! minutos", L"%1!lu! 分钟", L"%1!lu! min.", L"%1!lu! minuten" },
    { 93, L"%1 %2", L"%1 %2", L"%1 %2", L"%1 %2", L"%1 %2", L"%1 %2", L"%1 %2", L"%1 %2", L"%1 %2", L"%1 %2" },
    { 94, L"Download and install updates for your PC", L"Scarica e installa aggiornamenti per il tuo PC", L"Descargar e instalar actualizaciones para su PC", L"Télécharger et installer les mises à jour de votre PC", L"Bilgisayarınız için güncellemeleri indirip yükleyin", L"Загрузить и установить обновления для компьютера", L"Baixar e instalar atualizações para seu PC", L"下载并安装适用于你的电脑的更新", L"Pobierz i zainstaluj aktualizacje dla komputera", L"Updates voor uw pc downloaden en installeren" },
    { 95, L"Install updates for your PC", L"Installa aggiornamenti per il tuo PC", L"Instalar actualizaciones para su PC", L"Installer les mises à jour de votre PC", L"Bilgisayarınız için güncellemeleri yükleyin", L"Установить обновления для компьютера", L"Instalar atualizações para seu PC", L"安装适用于你的电脑的更新", L"Zainstaluj aktualizacje dla komputera", L"Updates voor uw pc installeren" },
    { 96, L"Not configured (not recommended)", L"Non configurato (sconsigliato)", L"No configurado (no recomendado)", L"Non configuré (non recommandé)", L"Yapılandırılmadı (önerilmez)", L"Не настроено (не рекомендуется)", L"Não configurado (não recomendado)", L"未配置（不推荐）", L"Nieskonfigurowano (niezalecane)", L"Niet geconfigureerd (niet aanbevolen)" },
    { 97, L"Never check for updates (not recommended)", L"Non controllare mai gli aggiornamenti (sconsigliato)", L"No comprobar nunca las actualizaciones (no recomendado)", L"Ne jamais rechercher les mises à jour (non recommandé)", L"Güncellemeleri hiç denetleme (önerilmez)", L"Никогда не проверять обновления (не рекомендуется)", L"Nunca verificar atualizações (não recomendado)", L"从不检查更新（不推荐）", L"Nigdy nie sprawdzaj aktualizacji (niezalecane)", L"Nooit naar updates zoeken (niet aanbevolen)" },
    { 98, L"Notify you to download and install new updates", L"Ti informa prima di scaricare e installare nuovi aggiornamenti", L"Notificarle para descargar e instalar nuevas actualizaciones", L"Vous avertir pour télécharger et installer les nouvelles mises à jour", L"Yeni güncellemeleri indirmek ve yüklemek için sizi bilgilendirir", L"Уведомлять о необходимости загрузки и установки новых обновлений", L"Notificá-lo para baixar e instalar novas atualizações", L"通知你下载并安装新更新", L"Powiadamiaj przed pobraniem i zainstalowaniem nowych aktualizacji", L"U op de hoogte stellen om nieuwe updates te downloaden en te installeren" },
    { 99, L"Notify you to install new updates", L"Ti informa prima di installare nuovi aggiornamenti", L"Notificarle para instalar nuevas actualizaciones", L"Vous avertir pour installer les nouvelles mises à jour", L"Yeni güncellemeleri yüklemek için sizi bilgilendirir", L"Уведомлять о необходимости установки новых обновлений", L"Notificá-lo para instalar novas atualizações", L"通知你安装新更新", L"Powiadamiaj przed zainstalowaniem nowych aktualizacji", L"U op de hoogte stellen om nieuwe updates te installeren" },
    { 100, L"Automatically install new updates every day at %s (recommended)", L"Installa automaticamente nuovi aggiornamenti ogni giorno alle ore %s (consigliato)", L"Instalar automáticamente nuevas actualizaciones todos los días a las %s (recomendado)", L"Installer automatiquement les nouvelles mises à jour chaque jour à %s (recommandé)", L"Yeni güncellemeleri her gün %s saatinde otomatik olarak yükle (önerilir)", L"Автоматически устанавливать новые обновления каждый день в %s (рекомендуется)", L"Instalar automaticamente novas atualizações todos os dias às %s (recomendado)", L"每天在 %s 自动安装新更新（推荐）", L"Automatycznie instaluj nowe aktualizacje codziennie o %s (zalecane)", L"Installeer nieuwe updates elke dag om %s automatisch (aanbevolen)" },
    { 101, L"Automatically install new updates every Sunday at %s", L"Installa automaticamente nuovi aggiornamenti ogni domenica alle ore %s", L"Instalar automáticamente nuevas actualizaciones cada domingo a las %s", L"Installer automatiquement les nouvelles mises à jour chaque dimanche à %s", L"Yeni güncellemeleri her Pazar %s saatinde otomatik olarak yükle", L"Автоматически устанавливать новые обновления каждое воскресенье в %s", L"Instalar automaticamente novas atualizações todos os domingos às %s", L"每星期日 %s 自动安装新更新", L"Automatycznie instaluj nowe aktualizacje w każdą niedzielę o %s", L"Installeer nieuwe updates elke zondag om %s automatisch" },
    { 102, L"Automatically install new updates every Monday at %s", L"Installa automaticamente nuovi aggiornamenti ogni lunedì alle ore %s", L"Instalar automáticamente nuevas actualizaciones cada lunes a las %s", L"Installer automatiquement les nouvelles mises à jour chaque lundi à %s", L"Yeni güncellemeleri her Pazartesi %s saatinde otomatik olarak yükle", L"Автоматически устанавливать новые обновления каждый понедельник в %s", L"Instalar automaticamente novas atualizações todas as segundas-feiras às %s", L"每星期一 %s 自动安装新更新", L"Automatycznie instaluj nowe aktualizacje w każdy poniedziałek o %s", L"Installeer nieuwe updates elke maandag om %s automatisch" },
    { 103, L"Automatically install new updates every Tuesday at %s", L"Installa automaticamente nuovi aggiornamenti ogni martedì alle ore %s", L"Instalar automáticamente nuevas actualizaciones cada martes a las %s", L"Installer automatiquement les nouvelles mises à jour chaque mardi à %s", L"Yeni güncellemeleri her Salı %s saatinde otomatik olarak yükle", L"Автоматически устанавливать новые обновления каждый вторник в %s", L"Instalar automaticamente novas atualizações todas as terças-feiras às %s", L"每星期二 %s 自动安装新更新", L"Automatycznie instaluj nowe aktualizacje w każdy wtorek o %s", L"Installeer nieuwe updates elke dinsdag om %s automatisch" },
    { 104, L"Automatically install new updates every Wednesday at %s", L"Installa automaticamente nuovi aggiornamenti ogni mercoledì alle ore %s", L"Instalar automáticamente nuevas actualizaciones cada miércoles a las %s", L"Installer automatiquement les nouvelles mises à jour chaque mercredi à %s", L"Yeni güncellemeleri her Çarşamba %s saatinde otomatik olarak yükle", L"Автоматически устанавливать новые обновления каждую среду в %s", L"Instalar automaticamente novas atualizações todas as quartas-feiras às %s", L"每星期三 %s 自动安装新更新", L"Automatycznie instaluj nowe aktualizacje w każdą środę o %s", L"Installeer nieuwe updates elke woensdag om %s automatisch" },
    { 105, L"Automatically install new updates every Thursday at %s", L"Installa automaticamente nuovi aggiornamenti ogni giovedì alle ore %s", L"Instalar automáticamente nuevas actualizaciones cada jueves a las %s", L"Installer automatiquement les nouvelles mises à jour chaque jeudi à %s", L"Yeni güncellemeleri her Perşembe %s saatinde otomatik olarak yükle", L"Автоматически устанавливать новые обновления каждый четверг в %s", L"Instalar automaticamente novas atualizações todas as quintas-feiras às %s", L"每星期四 %s 自动安装新更新", L"Automatycznie instaluj nowe aktualizacje w każdy czwartek o %s", L"Installeer nieuwe updates elke donderdag om %s automatisch" },
    { 106, L"Automatically install new updates every Friday at %s", L"Installa automaticamente nuovi aggiornamenti ogni venerdì alle ore %s", L"Instalar automáticamente nuevas actualizaciones cada viernes a las %s", L"Installer automatiquement les nouvelles mises à jour chaque vendredi à %s", L"Yeni güncellemeleri her Cuma %s saatinde otomatik olarak yükle", L"Автоматически устанавливать новые обновления каждую пятницу в %s", L"Instalar automaticamente novas atualizações todas as sextas-feiras às %s", L"每星期五 %s 自动安装新更新", L"Automatycznie instaluj nowe aktualizacje w każdy piątek o %s", L"Installeer nieuwe updates elke vrijdag om %s automatisch" },
    { 107, L"Automatically install new updates every Saturday at %s", L"Installa automaticamente nuovi aggiornamenti ogni sabato alle ore %s", L"Instalar automáticamente nuevas actualizaciones cada sábado a las %s", L"Installer automatiquement les nouvelles mises à jour chaque samedi à %s", L"Yeni güncellemeleri her Cumartesi %s saatinde otomatik olarak yükle", L"Автоматически устанавливать новые обновления каждую субботу в %s", L"Instalar automaticamente novas atualizações todos os sábados às %s", L"每星期六 %s 自动安装新更新", L"Automatycznie instaluj nowe aktualizacje w każdą sobotę o %s", L"Installeer nieuwe updates elke zaterdag om %s automatisch" },
    { 109, L"Today at %s", L"Oggi alle ore %s", L"Hoy a las %s", L"Aujourd'hui à %s", L"Bugün %s", L"Сегодня в %s", L"Hoje às %s", L"今天 %s", L"Dziś o %s", L"Vandaag om %s" },
    { 110, L"Yesterday at %s", L"Ieri alle ore %s", L"Ayer a las %s", L"Hier à %s", L"Dün %s", L"Вчера в %s", L"Ontem às %s", L"昨天 %s", L"Wczoraj o %s", L"Gisteren om %s" },
    { 111, L"%1 at %2", L"%1 alle ore %2", L"%1 a las %2", L"%1 à %2", L"%1 %2", L"%1 в %2", L"%1 às %2", L"%1 %2", L"%1 o %2", L"%1 om %2" },
    { 112, L"Error code %X", L"Codice di errore %X", L"Código de error %X", L"Code d'erreur %X", L"Hata kodu %X", L"Код ошибки %X", L"Código de erro %X", L"错误代码 %X", L"Kod błędu %X", L"Foutcode %X" },
    { 113, L"Without the latest updates, your PC is more vulnerable to security attacks and performance problems.", L"Senza gli aggiornamenti più recenti, il tuo PC è più vulnerabile ad attacchi alla sicurezza e a problemi di prestazioni.", L"Sin las actualizaciones más recientes, su PC es más vulnerable a ataques de seguridad y problemas de rendimiento.", L"Sans les dernières mises à jour, votre PC est plus vulnérable aux attaques de sécurité et aux problèmes de performances.", L"En son güncellemeler olmadan bilgisayarınız güvenlik saldırılarına ve performans sorunlarına karşı daha savunmasızdır.", L"Без последних обновлений компьютер более уязвим к атакам и проблемам с производительностью.", L"Sem as atualizações mais recentes, seu PC fica mais vulnerável a ataques de segurança e problemas de desempenho.", L"没有最新更新，你的电脑更容易受到安全攻击和性能问题的影响。", L"Bez najnowszych aktualizacji komputer jest bardziej narażony na ataki i problemy z wydajnością.", L"Zonder de nieuwste updates is uw pc kwetsbaarder voor beveiligingsaanvallen en prestatieproblemen." },
    { 114, L"%1 - %2", L"%1 - %2", L"%1 - %2", L"%1 - %2", L"%1 - %2", L"%1 — %2", L"%1 - %2", L"%1 - %2", L"%1 - %2", L"%1 - %2" },
    { 115, L"Recommended", L"Consigliato", L"Recomendado", L"Recommandé", L"Önerilen", L"Рекомендуется", L"Recomendado", L"推荐", L"Zalecane", L"Aanbevolen" },
    { 116, L"Name", L"Nome", L"Nombre", L"Nom", L"Ad", L"Имя", L"Nome", L"名称", L"Nazwa", L"Naam" },
    { 117, L"Type", L"Tipo", L"Tipo", L"Type", L"Tür", L"Тип", L"Tipo", L"类型", L"Typ", L"Type" },
    { 118, L"Published", L"Pubblicato", L"Publicado", L"Publiée", L"Yayınlanma", L"Опубликовано", L"Publicado", L"已发布", L"Opublikowano", L"Gepubliceerd" },
    { 119, L"Important", L"Importante", L"Importante", L"Importante", L"Önemli", L"Важное", L"Importante", L"重要", L"Ważne", L"Belangrijk" },
    { 120, L"Optional", L"Facoltativo", L"Opcional", L"Facultatif", L"İsteğe bağlı", L"Необязательное", L"Opcional", L"可选", L"Opcjonalne", L"Optioneel" },
    { 121, L"Today", L"Oggi", L"Hoy", L"Aujourd'hui", L"Bugün", L"Сегодня", L"Hoje", L"今天", L"Dziś", L"Vandaag" },
    { 122, L"Yesterday", L"Ieri", L"Ayer", L"Hier", L"Dün", L"Вчера", L"Ontem", L"昨天", L"Wczoraj", L"Gisteren" },
    { 123, L"Status", L"Stato", L"Estado", L"État", L"Durum", L"Состояние", L"Status", L"状态", L"Stan", L"Status" },
    { 124, L"Date installed", L"Data di installazione", L"Fecha de instalación", L"Date d'installation", L"Yükleme tarihi", L"Дата установки", L"Data de instalação", L"安装日期", L"Data instalacji", L"Installatiedatum" },
    { 125, L"Succeeded", L"Riuscito", L"Correcta", L"Réussie", L"Başarılı", L"Успешно", L"Bem-sucedido", L"成功", L"Powodzenie", L"Geslaagd" },
    { 126, L"Failed", L"Non riuscito", L"Error", L"Échec", L"Başarısız", L"Неудачно", L"Falhou", L"失败", L"Niepowodzenie", L"Mislukt" },
    { 127, L"Canceled", L"Annullato", L"Cancelada", L"Annulée", L"İptal edildi", L"Отменено", L"Cancelada", L"已取消", L"Anulowano", L"Geannuleerd" },
    { 128, L"Downloading updates...", L"Download degli aggiornamenti in corso...", L"Descargando actualizaciones...", L"Téléchargement des mises à jour...", L"Güncellemeler indiriliyor...", L"Загрузка обновлений...", L"Baixando atualizações...", L"正在下载更新...", L"Pobieranie aktualizacji...", L"Updates downloaden..." },
    { 129, L"Downloading %1!lu! updates (%2 total, %3!lu!%% complete)", L"Download di %1!lu! aggiornamenti (%2 in totale, %3!lu!%% completato)", L"Descargando %1!lu! actualizaciones (%2 en total, %3!lu!%% completadas)", L"Téléchargement de %1!lu! mises à jour (%2 au total, %3!lu!%% effectué)", L"%1!lu! güncelleme indiriliyor (toplam %2, %3!lu!%% tamamlandı)", L"Загрузка обновлений: %1!lu! (%2 всего, выполнено %3!lu!%%)", L"Baixando %1!lu! atualizações (%2 no total, %3!lu!%% concluído)", L"正在下载 %1!lu! 个更新（共 %2 个，已完成 %3!lu!%%）", L"Pobieranie %1!lu! aktualizacji (łącznie %2, %3!lu!%% ukończono)", L"%1!lu! updates downloaden (%2 totaal, %3!lu!%% voltooid)" },
    { 130, L"Downloading 1 update (%2 total, %3!lu!%% complete)", L"Download di 1 aggiornamento (%2 in totale, %3!lu!%% completato)", L"Descargando 1 actualización (%2 en total, %3!lu!%% completada)", L"Téléchargement de 1 mise à jour (%2 au total, %3!lu!%% effectué)", L"1 güncelleme indiriliyor (toplam %2, %3!lu!%% tamamlandı)", L"Загрузка 1 обновления (%2 всего, выполнено %3!lu!%%)", L"Baixando 1 atualização (%2 no total, %3!lu!%% concluído)", L"正在下载 1 个更新（共 %2 个，已完成 %3!lu!%%）", L"Pobieranie 1 aktualizacji (łącznie %2, %3!lu!%% ukończono)", L"1 update downloaden (%2 totaal, %3!lu!%% voltooid)" },
    { 131, L"&Stop download", L"&Interrompi download", L"&Detener descarga", L"&Arrêter le téléchargement", L"İndirmeyi &durdur", L"&Остановить загрузку", L"&Parar download", L"&停止下载", L"&Zatrzymaj pobieranie", L"Download &stoppen" },
    { 132, L"Installing updates...", L"Installazione aggiornamenti in corso...", L"Instalando actualizaciones...", L"Installation des mises à jour...", L"Güncellemeler yükleniyor...", L"Установка обновлений...", L"Instalando atualizações...", L"正在安装更新...", L"Instalowanie aktualizacji...", L"Updates installeren..." },
    { 133, L"Installing update %1!lu! of %2!lu!...", L"Installazione aggiornamento %1!lu! di %2!lu!...", L"Instalando actualización %1!lu! de %2!lu!...", L"Installation de la mise à jour %1!lu! sur %2!lu!...", L"%2!lu! güncellemeden %1!lu! yükleniyor...", L"Установка обновления %1!lu! из %2!lu!...", L"Instalando atualização %1!lu! de %2!lu!...", L"正在安装 %2!lu! 个更新中的第 %1!lu! 个...", L"Instalowanie aktualizacji %1!lu! z %2!lu!...", L"Update %1!lu! van %2!lu! installeren..." },
    { 134, L"Preparing to install...", L"Preparazione installazione...", L"Preparando la instalación...", L"Préparation de l'installation...", L"Yüklemeye hazırlanıyor...", L"Подготовка к установке...", L"Preparando a instalação...", L"正在准备安装...", L"Przygotowywanie do instalacji...", L"Installatie voorbereiden..." },
    { 135, L"(Uninstall:) %s", L"(Disinstallazione:) %s", L"(Desinstalar:) %s", L"(Désinstaller:) %s", L"(Kaldır:) %s", L"(Удаление:) %s", L"(Desinstalar:) %s", L"（卸载：）%s", L"(Odinstaluj:) %s", L"(Verwijderen:) %s" },
    { 136, L"%1 (%2!lu!)", L"%1 (%2!lu!)", L"%1 (%2!lu!)", L"%1 (%2!lu!)", L"%1 (%2!lu!)", L"%1 (%2!lu!)", L"%1 (%2!lu!)", L"%1（%2!lu!）", L"%1 (%2!lu!)", L"%1 (%2!lu!)" },
    { 137, L"Updates", L"Aggiornamenti", L"Actualizaciones", L"Mises à jour", L"Güncellemeler", L"Обновления", L"Atualizações", L"更新", L"Aktualizacje", L"Updates" },
    { 138, L"Total missing updates: %1!lu!", L"Aggiornamenti mancanti totali: %1!lu!", L"Actualizaciones que faltan en total: %1!lu!", L"Mises à jour manquantes au total : %1!lu!", L"Toplam eksik güncelleme: %1!lu!", L"Всего отсутствует обновлений: %1!lu!", L"Atualizações ausentes no total: %1!lu!", L"缺少的更新总数：%1!lu!", L"Łącznie brakujących aktualizacji: %1!lu!", L"Totaal ontbrekende updates: %1!lu!" },
    { 139, L"Code %1!X!", L"Codice %1!X!", L"Código %1!X!", L"Code %1!X!", L"Kod %1!X!", L"Код %1!X!", L"Código %1!X!", L"代码 %1!X!", L"Kod %1!X!", L"Code %1!X!" },
    { 140, L"%1 %2", L"%1 %2", L"%1 %2", L"%1 %2", L"%1 %2", L"%1 %2", L"%1 %2", L"%1 %2", L"%1 %2", L"%1 %2" },
    { 141, L"Succeeded: 1 update", L"Riuscito: 1 aggiornamento", L"Correctas: 1 actualización", L"Réussie : 1 mise à jour", L"Başarılı: 1 güncelleme", L"Успешно: 1 обновление", L"Bem-sucedida: 1 atualização", L"成功：1 个更新", L"Powodzenie: 1 aktualizacja", L"Geslaagd: 1 update" },
    { 142, L"Succeeded: %1!lu! updates", L"Riuscito: %1!lu! aggiornamenti", L"Correctas: %1!lu! actualizaciones", L"Réussies : %1!lu! mises à jour", L"Başarılı: %1!lu! güncelleme", L"Успешно: %1!lu! обновлений", L"Bem-sucedidas: %1!lu! atualizações", L"成功：%1!lu! 个更新", L"Powodzenie: %1!lu! aktualizacji", L"Geslaagd: %1!lu! updates" },
    { 143, L"Failed: 1 update", L"Non riuscito: 1 aggiornamento", L"Error: 1 actualización", L"Échec : 1 mise à jour", L"Başarısız: 1 güncelleme", L"Неудачно: 1 обновление", L"Falhou: 1 atualização", L"失败：1 个更新", L"Niepowodzenie: 1 aktualizacja", L"Mislukt: 1 update" },
    { 144, L"Failed: %1!lu! updates", L"Non riuscito: %1!lu! aggiornamenti", L"Error: %1!lu! actualizaciones", L"Échec : %1!lu! mises à jour", L"Başarısız: %1!lu! güncelleme", L"Неудачно: %1!lu! обновлений", L"Falhou: %1!lu! atualizações", L"失败：%1!lu! 个更新", L"Niepowodzenie: %1!lu! aktualizacji", L"Mislukt: %1!lu! updates" },
    { 145, L"Canceled: 1 update", L"Annullato: 1 aggiornamento", L"Cancelada: 1 actualización", L"Annulée : 1 mise à jour", L"İptal edildi: 1 güncelleme", L"Отменено: 1 обновление", L"Cancelada: 1 atualização", L"已取消：1 个更新", L"Anulowano: 1 aktualizacja", L"Geannuleerd: 1 update" },
    { 146, L"Canceled: %1!lu! updates", L"Annullato: %1!lu! aggiornamenti", L"Canceladas: %1!lu! actualizaciones", L"Annulées : %1!lu! mises à jour", L"İptal edildi: %1!lu! güncelleme", L"Отменено: %1!lu! обновлений", L"Canceladas: %1!lu! atualizações", L"已取消：%1!lu! 个更新", L"Anulowano: %1!lu! aktualizacji", L"Geannuleerd: %1!lu! updates" },
    { 147, L"Not needed: 1 update", L"Non necessario: 1 aggiornamento", L"No necesario: 1 actualización", L"Non nécessaire : 1 mise à jour", L"Gerekli değil: 1 güncelleme", L"Не требуется: 1 обновление", L"Não necessária: 1 atualização", L"不需要：1 个更新", L"Niepotrzebne: 1 aktualizacja", L"Niet nodig: 1 update" },
    { 148, L"Not needed: %1!lu! updates", L"Non necessario: %1!lu! aggiornamenti", L"No necesarios: %1!lu! actualizaciones", L"Non nécessaires : %1!lu! mises à jour", L"Gerekli değil: %1!lu! güncelleme", L"Не требуется: %1!lu! обновлений", L"Não necessárias: %1!lu! atualizações", L"不需要：%1!lu! 个更新", L"Niepotrzebne: %1!lu! aktualizacji", L"Niet nodig: %1!lu! updates" },
    { 152, L"Windows could not search for new updates", L"Windows non ha potuto cercare nuovi aggiornamenti", L"Windows no pudo buscar nuevas actualizaciones", L"Windows n'a pas pu rechercher de nouvelles mises à jour", L"Windows yeni güncellemeleri arayamadı", L"Windows не удалось найти новые обновления", L"O Windows não pôde procurar novas atualizações", L"Windows 无法搜索新更新", L"System Windows nie mógł wyszukać nowych aktualizacji", L"Windows kon geen nieuwe updates zoeken" },
    { 157, L"&Restore update", L"&Ripristina aggiornamento", L"&Restaurar actualización", L"&Restaurer la mise à jour", L"Güncellemeyi &geri yükle", L"&Восстановить обновление", L"&Restaurar atualização", L"&还原更新", L"&Przywróć aktualizację", L"Update &herstellen" },
    { 158, L"&Hide update", L"&Nascondi aggiornamento", L"&Ocultar actualización", L"&Masquer la mise à jour", L"Güncellemeyi &gizle", L"&Скрыть обновление", L"&Ocultar atualização", L"&隐藏更新", L"&Ukryj aktualizację", L"Update &verbergen" },
    { 159, L"Downloading and installing updates...", L"Download e installazione aggiornamenti in corso...", L"Descargando e instalando actualizaciones...", L"Téléchargement et installation des mises à jour...", L"Güncellemeler indiriliyor ve yükleniyor...", L"Загрузка и установка обновлений...", L"Baixando e instalando atualizações...", L"正在下载并安装更新...", L"Pobieranie i instalowanie aktualizacji...", L"Updates downloaden en installeren..." },
    { 160, L"If you opt out, you won't receive updates from %1 anymore. Do you want to continue?", L"Se annulli l'iscrizione, non riceverai più aggiornamenti da %1. Continuare?", L"Si opta por no participar, dejará de recibir actualizaciones de %1. ¿Desea continuar?", L"Si vous vous désabonnez, vous ne recevrez plus de mises à jour de %1. Voulez-vous continuer ?", L"Abone olmaktan çıkarsanız artık %1 güncellemelerini alamazsınız. Devam etmek istiyor musunuz?", L"Если отказаться, вы больше не будете получать обновления от %1. Продолжить?", L"Se você desativar a assinatura, não receberá mais atualizações de %1. Deseja continuar?", L"如果选择退出，你将不再收到来自 %1 的更新。是否要继续？", L"Jeśli zrezygnujesz, nie będziesz już otrzymywać aktualizacji od %1. Czy chcesz kontynuować?", L"Als u zich afmeldt, ontvangt u geen updates meer van %1. Wilt u doorgaan?" },
    { 161, L"Your administrator requires this update to be installed by %1 at %2.", L"L'amministratore richiede l'installazione di questo aggiornamento entro %1 alle ore %2.", L"El administrador exige que esta actualización se instale antes del %1 a las %2.", L"Votre administrateur exige que cette mise à jour soit installée d'ici le %1 à %2.", L"Yöneticiniz bu güncellemenin %2 tarihinde %1 saatine kadar yüklenmesini gerektiriyor.", L"Администратор требует установить это обновление до %1 в %2.", L"Seu administrador exige que esta atualização seja instalada até %1 às %2.", L"管理员要求此更新必须在 %1 %2 之前安装。", L"Administrator wymaga zainstalowania tej aktualizacji do %1 o %2.", L"Uw beheerder vereist dat deze update vóór %1 om %2 is geïnstalleerd." },
    { 162, L"Your administrator requires this update to be installed by today at %s.", L"L'amministratore richiede l'installazione di questo aggiornamento entro oggi alle ore %s.", L"El administrador exige que esta actualización se instale hoy a las %s.", L"Votre administrateur exige que cette mise à jour soit installée aujourd'hui à %s.", L"Yöneticiniz bu güncellemenin bugün %s saatine kadar yüklenmesini gerektiriyor.", L"Администратор требует установить это обновление сегодня в %s.", L"Seu administrador exige que esta atualização seja instalada hoje às %s.", L"管理员要求此更新必须在今天 %s 之前安装。", L"Administrator wymaga zainstalowania tej aktualizacji dziś o %s.", L"Uw beheerder vereist dat deze update vandaag om %s is geïnstalleerd." },
    { 163, L"Windows Update", L"Windows Update", L"Windows Update", L"Windows Update", L"Windows Update", L"Центр обновления Windows", L"Windows Update", L"Windows 更新", L"Windows Update", L"Windows Update" },
    { 164, L"Please read and accept the license terms (%1!lu! of %2!lu!)", L"Leggere e accettare i termini di licenza (%1!lu! di %2!lu!)", L"Lea y acepte los términos de licencia (%1!lu! de %2!lu!)", L"Veuillez lire et accepter les termes du contrat de licence (%1!lu! sur %2!lu!)", L"Lisans koşullarını okuyun ve kabul edin (%2!lu! sözleşmeden %1!lu!)", L"Прочтите и примите условия лицензии (%1!lu! из %2!lu!)", L"Leia e aceite os termos da licença (%1!lu! de %2!lu!)", L"请阅读并接受许可条款（共 %2!lu! 项，第 %1!lu! 项）", L"Przeczytaj i zaakceptuj postanowienia licencyjne (%1!lu! z %2!lu!)", L"Lees en accepteer de licentievoorwaarden (%1!lu! van %2!lu!)" },
    { 165, L"Windows Update", L"Windows Update", L"Windows Update", L"Windows Update", L"Windows Update", L"Центр обновления Windows", L"Windows Update", L"Windows 更新", L"Windows Update", L"Windows Update" },
    { 166, L"These updates won't be installed.", L"Questi aggiornamenti non verranno installati.", L"Estas actualizaciones no se instalarán.", L"Ces mises à jour ne seront pas installées.", L"Bu güncellemeler yüklenmeyecek.", L"Эти обновления не будут установлены.", L"Estas atualizações não serão instaladas.", L"这些更新将不会安装。", L"Te aktualizacje nie zostaną zainstalowane.", L"Deze updates worden niet geïnstalleerd." },
    { 167, L"&Don't ask me again to install these updates", L"&Non chiedermi più di installare questi aggiornamenti", L"No &volver a preguntar si deseo instalar estas actualizaciones", L"Ne plus &demander d'installer ces mises à jour", L"Bu güncellemeleri yüklememi bir daha &sorma", L"&Больше не спрашивать об установке этих обновлений", L"Não &perguntar novamente sobre estas atualizações", L"&不再询问是否安装这些更新", L"&Nie pytaj ponownie o instalowanie tych aktualizacji", L"Niet &opnieuw vragen deze updates te installeren" },
    { 168, L"%1. ", L"%1. ", L"%1. ", L"%1. ", L"%1. ", L"%1. ", L"%1. ", L"%1.", L"%1. ", L"%1. " },
    { 169, L"You need to provide administrator permission", L"È necessario fornire l'autorizzazione di amministratore", L"Debe proporcionar permiso de administrador", L"Vous devez fournir une autorisation d'administrateur", L"Yönetici izni sağlamanız gerekir", L"Требуется разрешение администратора", L"Você precisa fornecer permissão de administrador", L"你需要提供管理员权限", L"Musisz podać uprawnienia administratora", L"U moet beheerdersmachtiging verstrekken" },
    { 170, L"To complete this task, you need to sign in with an administrator account or ask an administrator to complete the task for you.", L"Per completare l'attività, devi accedere con un account amministratore o chiedere a un amministratore di completarla per te.", L"Para completar esta tarea, debe iniciar sesión con una cuenta de administrador o pedirle a un administrador que la complete por usted.", L"Pour terminer cette tâche, vous devez vous connecter avec un compte administrateur ou demander à un administrateur de la terminer pour vous.", L"Bu görevi tamamlamak için bir yönetici hesabıyla oturum açmanız veya bir yöneticiden bu görevi sizin için tamamlamasını istemeniz gerekir.", L"Для выполнения этой задачи необходимо войти с учетной записью администратора или попросить администратора выполнить ее за вас.", L"Para concluir esta tarefa, você precisa entrar com uma conta de administrador ou pedir a um administrador que a conclua para você.", L"要完成此任务，你需要使用管理员帐户登录，或请求管理员为你完成此任务。", L"Aby ukończyć to zadanie, musisz zalogować się na konto administratora lub poprosić administratora o ukończenie go za Ciebie.", L"Om deze taak te voltooien moet u zich aanmelden met een beheerdersaccount of een beheerder vragen deze taak voor u te voltooien." },
    { 171, L"Restart now to finish installing updates.", L"Riavvia ora per terminare l'installazione degli aggiornamenti.", L"Reinicie ahora para terminar de instalar las actualizaciones.", L"Redémarrez maintenant pour terminer l'installation des mises à jour.", L"Güncellemelerin yüklenmesini bitirmek için şimdi yeniden başlatın.", L"Перезапустите компьютер сейчас, чтобы завершить установку обновлений.", L"Reinicie agora para concluir a instalação das atualizações.", L"立即重启以完成更新安装。", L"Uruchom ponownie teraz, aby zakończyć instalowanie aktualizacji.", L"Start nu opnieuw op om de installatie van updates te voltooien." },
    { 173, L"1 optional update", L"1 aggiornamento facoltativo", L"1 actualización opcional", L"1 mise à jour facultative", L"1 isteğe bağlı güncelleme", L"1 необязательное обновление", L"1 atualização opcional", L"1 个可选更新", L"1 opcjonalna aktualizacja", L"1 optionele update" },
    { 174, L"%1!lu! optional updates", L"%1!lu! aggiornamenti facoltativi", L"%1!lu! actualizaciones opcionales", L"%1!lu! mises à jour facultatives", L"%1!lu! isteğe bağlı güncelleme", L"%1!lu! необязательных обновлений", L"%1!lu! atualizações opcionais", L"%1!lu! 个可选更新", L"%1!lu! opcjonalnych aktualizacji", L"%1!lu! optionele updates" },
    { 178, L"For Windows and other products from %s", L"Per Windows e altri prodotti di %s", L"Para Windows y otros productos de %s", L"Pour Windows et d'autres produits de %s", L"Windows ve %s diğer ürünleri için", L"Для Windows и других продуктов %s", L"Para Windows e outros produtos da %s", L"适用于 %s 的 Windows 及其他产品", L"Dla systemu Windows i innych produktów firmy %s", L"Voor Windows en andere producten van %s" },
    { 179, L"From %s", L"Da %s", L"De %s", L"De %s", L"%s sürümünden", L"От %s", L"Da %s", L"来自 %s", L"Od %s", L"Van %s" },
    { 180, L"You will receive updates from %1.", L"Riceverai aggiornamenti da %1.", L"Recibirá actualizaciones de %1.", L"Vous recevrez des mises à jour de %1.", L"%1 güncellemelerini alacaksınız.", L"Вы будете получать обновления от %1.", L"Você receberá atualizações de %1.", L"你将收到来自 %1 的更新。", L"Będziesz otrzymywać aktualizacje od %1.", L"U ontvangt updates van %1." },
    { 182, L"Managed by your system administrator", L"Gestito dall'amministratore di sistema", L"Administrado por el administrador del sistema", L"Géré par votre administrateur système", L"Sistem yöneticiniz tarafından yönetiliyor", L"Управляется системным администратором", L"Gerenciado pelo administrador do sistema", L"由你的系统管理员管理", L"Zarządzane przez administratora systemu", L"Beheerd door uw systeembeheerder" },
    { 183, L"More updates are available.", L"Sono disponibili altri aggiornamenti.", L"Hay más actualizaciones disponibles.", L"D'autres mises à jour sont disponibles.", L"Daha fazla güncelleme kullanılabilir.", L"Доступно больше обновлений.", L"Há mais atualizações disponíveis.", L"还有更多更新可用。", L"Dostępnych jest więcej aktualizacji.", L"Er zijn meer updates beschikbaar." },
    { 184, L"%1 (Failed)", L"%1 (non riuscito)", L"%1 (Error)", L"%1 (échec)", L"%1 (başarısız)", L"%1 (сбой)", L"%1 (falhou)", L"%1（失败）", L"%1 (niepowodzenie)", L"%1 (mislukt)" },
    { 185, L"Pending restart", L"Riavvio in sospeso", L"Reinicio pendiente", L"Redémarrage en attente", L"Yeniden başlatma bekleniyor", L"Ожидается перезапуск", L"Reinicialização pendente", L"待重启", L"Oczekiwanie na ponowne uruchomienie", L"Opnieuw opstarten in behandeling" },
    { 187, L"For Windows only.", L"Solo per Windows.", L"Solo para Windows.", L"Windows uniquement.", L"Yalnızca Windows için.", L"Только для Windows.", L"Somente para Windows.", L"仅适用于 Windows。", L"Tylko dla systemu Windows.", L"Alleen voor Windows." },
    { 188, L"Windows Update", L"Windows Update", L"Windows Update", L"Windows Update", L"Windows Update", L"Центр обновления Windows", L"Windows Update", L"Windows 更新", L"Windows Update", L"Windows Update" },
    { 189, L"Check for updates managed by your system administrator", L"Controlla gli aggiornamenti gestiti dall'amministratore di sistema", L"Comprobar actualizaciones administradas por el administrador del sistema", L"Rechercher les mises à jour gérées par votre administrateur système", L"Sistem yöneticiniz tarafından yönetilen güncellemeleri denetleyin", L"Проверить обновления, управляемые системным администратором", L"Verificar atualizações gerenciadas pelo administrador do sistema", L"检查由系统管理员管理的更新", L"Sprawdź aktualizacje zarządzane przez administratora systemu", L"Controleer op updates die door uw systeembeheerder worden beheerd" },
    { 191, L"Windows Update ran into a problem.", L"Windows Update ha riscontrato un problema.", L"Windows Update tuvo un problema.", L"Windows Update a rencontré un problème.", L"Windows Update bir sorunla karşılaştı.", L"Центр обновления Windows столкнулся с проблемой.", L"O Windows Update encontrou um problema.", L"Windows 更新遇到问题。", L"Windows Update napotkał problem.", L"Windows Update heeft een probleem ondervonden." },
    { 222, L"Restarting in: %1!d! min, %2!d! sec", L"Riavvio tra: %1!d! min, %2!d! sec", L"Reiniciando en: %1!d! min, %2!d! seg", L"Redémarrage dans : %1!d! min, %2!d! s", L"Şu süre içinde yeniden başlatılıyor: %1!d! dk, %2!d! sn", L"Перезапуск через: %1!d! мин, %2!d! с", L"Reiniciando em: %1!d! min, %2!d! seg", L"正在重启：%1!d! 分 %2!d! 秒", L"Uruchamianie ponownie za: %1!d! min, %2!d! s", L"Opnieuw starten over: %1!d! min, %2!d! sec" },
    { 223, L"&Restart", L"&Riavvia", L"&Reiniciar", L"&Redémarrer", L"Yeniden &başlat", L"&Перезапустить", L"&Reiniciar", L"&重启", L"&Uruchom ponownie", L"&Opnieuw starten" },
    { 226, L"Windows can't update important files and services while the system is using them. Make sure to save your files before restarting.", L"Windows non può aggiornare file e servizi importanti mentre il sistema li sta utilizzando. Salva i file prima di riavviare.", L"Windows no puede actualizar archivos y servicios importantes mientras el sistema los está usando. Asegúrese de guardar sus archivos antes de reiniciar.", L"Windows ne peut pas mettre à jour des fichiers et services importants pendant que le système les utilise. Veillez à enregistrer vos fichiers avant de redémarrer.", L"Sistem bunları kullanırken Windows önemli dosyaları ve hizmetleri güncelleyemez. Yeniden başlatmadan önce dosyalarınızı kaydettiğinizden emin olun.", L"Windows не может обновить важные файлы и службы, пока они используются системой. Перед перезапуском обязательно сохраните файлы.", L"O Windows não pode atualizar arquivos e serviços importantes enquanto o sistema os está usando. Salve seus arquivos antes de reiniciar.", L"系统正在使用重要文件和服务时，Windows 无法更新它们。请确保在重启前保存文件。", L"System Windows nie może zaktualizować ważnych plików i usług, gdy są używane. Przed ponownym uruchomieniem zapisz pliki.", L"Windows kan geen belangrijke bestanden en services bijwerken terwijl het systeem ze gebruikt. Sla uw bestanden op voordat u opnieuw opstart." },
    { 227, L"Your PC needs to restart to finish installing important updates. If you've already saved everything, you can restart now. Otherwise, you should take a moment to save your work.", L"Il tuo PC deve essere riavviato per completare l'installazione degli aggiornamenti importanti. Se hai già salvato tutto, puoi riavviare ora. In caso contrario, prenditi un momento per salvare il lavoro.", L"Su PC debe reiniciarse para terminar de instalar las actualizaciones importantes. Si ya guardó todo, puede reiniciar ahora. De lo contrario, debe tomarse un momento para guardar su trabajo.", L"Votre PC doit redémarrer pour terminer l'installation des mises à jour importantes. Si vous avez déjà tout enregistré, vous pouvez redémarrer maintenant. Sinon, prenez un moment pour enregistrer votre travail.", L"Bilgisayarınızın önemli güncellemelerin yüklenmesini bitirmek için yeniden başlatılması gerekiyor. Her şeyi kaydettiyseniz şimdi yeniden başlatabilirsiniz. Aksi takdirde, çalışmanızı kaydetmek için bir dakikanızı ayırın.", L"Компьютер необходимо перезапустить, чтобы завершить установку важных обновлений. Если вы уже сохранили все, можно перезапустить сейчас. В противном случае сохраните свою работу.", L"Seu PC precisa ser reiniciado para concluir a instalação de atualizações importantes. Se já salvou tudo, reinicie agora. Caso contrário, reserve um momento para salvar seu trabalho.", L"你的电脑需要重启才能完成重要更新的安装。如果已保存所有内容，现在即可重启。否则，请花点时间保存你的工作。", L"Komputer należy ponownie uruchomić, aby zakończyć instalowanie ważnych aktualizacji. Jeśli wszystko zapisałeś, uruchom ponownie teraz. W przeciwnym razie poświęć chwilę na zapisanie pracy.", L"Uw pc moet opnieuw worden gestart om de installatie van belangrijke updates te voltooien. Als u alles al hebt opgeslagen, kunt u nu opnieuw starten. Anders neemt u even de tijd om uw werk op te slaan." },
    { 235, L"Restarting in %1!d! minutes, %2!d! seconds", L"Riavvio tra %1!d! minuti e %2!d! secondi", L"Reiniciando en %1!d! minutos y %2!d! segundos", L"Redémarrage dans %1!d! minutes et %2!d! secondes", L"%1!d! dakika %2!d! saniye içinde yeniden başlatılıyor", L"Перезапуск через %1!d! мин и %2!d! с", L"Reiniciando em %1!d! minutos e %2!d! segundos", L"将在 %1!d! 分 %2!d! 秒后重启", L"Uruchamianie ponowne za %1!d! minut i %2!d! sekund", L"Opnieuw starten over %1!d! minuten en %2!d! seconden" },
    { 236, L"Restarting in %1!d! seconds", L"Riavvio tra %1!d! secondi", L"Reiniciando en %1!d! segundos", L"Redémarrage dans %1!d! secondes", L"%1!d! saniye içinde yeniden başlatılıyor", L"Перезапуск через %1!d! с", L"Reiniciando em %1!d! segundos", L"将在 %1!d! 秒后重启", L"Uruchamianie ponowne za %1!d! sekund", L"Opnieuw starten over %1!d! seconden" },
    { 237, L"&Close", L"&Chiudi", L"&Cerrar", L"&Fermer", L"&Kapat", L"&Закрыть", L"&Fechar", L"&关闭", L"&Zamknij", L"&Sluiten" },
    { 238, L"Updates are available", L"Sono disponibili aggiornamenti", L"Hay actualizaciones disponibles", L"Des mises à jour sont disponibles", L"Güncellemeler kullanılabilir", L"Доступны обновления", L"Há atualizações disponíveis", L"有可用更新", L"Dostępne są aktualizacje", L"Er zijn updates beschikbaar" },
    { 239, L"Restart to finish updating your PC", L"Riavvia per completare l'aggiornamento del PC", L"Reinicie para terminar de actualizar su PC", L"Redémarrez pour terminer la mise à jour de votre PC", L"Bilgisayarınızın güncellemesini bitirmek için yeniden başlatın", L"Перезапустите компьютер, чтобы завершить обновление", L"Reinicie para concluir a atualização do PC", L"重启以完成电脑更新", L"Uruchom ponownie, aby zakończyć aktualizację komputera", L"Start opnieuw op om het bijwerken van uw pc te voltooien" },
    { 240, L"Go to Windows Update to install the updates now.", L"Vai a Windows Update per installare gli aggiornamenti ora.", L"Vaya a Windows Update para instalar las actualizaciones ahora.", L"Accédez à Windows Update pour installer les mises à jour maintenant.", L"Güncellemeleri şimdi yüklemek için Windows Update'e gidin.", L"Перейдите в Центр обновления Windows, чтобы установить обновления сейчас.", L"Vá para Windows Update para instalar as atualizações agora.", L"转到 Windows 更新以立即安装更新。", L"Przejdź do Windows Update, aby teraz zainstalować aktualizacje.", L"Ga naar Windows Update om de updates nu te installeren." },
    { 241, L"Windows Update couldn't install the updates automatically. Go to Windows Update to install them now.", L"Windows Update non ha potuto installare automaticamente gli aggiornamenti. Vai a Windows Update per installarli ora.", L"Windows Update no pudo instalar las actualizaciones automáticamente. Vaya a Windows Update para instalarlas ahora.", L"Windows Update n'a pas pu installer automatiquement les mises à jour. Accédez à Windows Update pour les installer maintenant.", L"Windows Update güncellemeleri otomatik olarak yükleyemedi. Bunları şimdi yüklemek için Windows Update'e gidin.", L"Центру обновления Windows не удалось установить обновления автоматически. Перейдите в Центр обновления Windows, чтобы установить их сейчас.", L"O Windows Update não conseguiu instalar as atualizações automaticamente. Vá para Windows Update para instalá-las agora.", L"Windows 更新无法自动安装更新。请转到 Windows 更新立即安装。", L"Windows Update nie mógł automatycznie zainstalować aktualizacji. Przejdź do Windows Update, aby je teraz zainstalować.", L"Windows Update kon de updates niet automatisch installeren. Ga naar Windows Update om ze nu te installeren." },
    { 242, L"Save your work, and restart your PC now to finish installing important updates. If you choose 'Later', your PC will automatically restart in 1 day.", L"Salva il lavoro e riavvia il PC ora per completare l'installazione degli aggiornamenti importanti. Se scegli 'Dopo', il PC si riavvierà automaticamente entro 1 giorno.", L"Guarde su trabajo y reinicie su PC ahora para terminar de instalar las actualizaciones importantes. Si elige 'Más tarde', su PC se reiniciará automáticamente en 1 día.", L"Enregistrez votre travail et redémarrez votre PC maintenant pour terminer l'installation des mises à jour importantes. Si vous choisissez 'Plus tard', votre PC redémarrera automatiquement dans 1 jour.", L"Çalışmanızı kaydedin ve önemli güncellemelerin yüklenmesini bitirmek için bilgisayarınızı şimdi yeniden başlatın. 'Sonra' seçerseniz bilgisayarınız 1 gün içinde otomatik olarak yeniden başlatılır.", L"Сохраните работу и перезапустите компьютер, чтобы завершить установку важных обновлений. Если вы выберете «Позже», компьютер будет автоматически перезапущен через 1 день.", L"Salve seu trabalho e reinicie o PC agora para concluir a instalação de atualizações importantes. Se escolher 'Mais tarde', seu PC reiniciará automaticamente em 1 dia.", L"保存你的工作，然后立即重启电脑以完成重要更新的安装。如果选择“稍后”，你的电脑将在 1 天后自动重启。", L"Zapisz swoją pracę i uruchom ponownie komputer, aby zakończyć instalowanie ważnych aktualizacji. Jeśli wybierzesz 'Później', komputer uruchomi się ponownie automatycznie za 1 dzień.", L"Sla uw werk op en start uw pc nu opnieuw op om de installatie van belangrijke updates te voltooien. Als u 'Later' kiest, wordt uw pc automatisch binnen 1 dag opnieuw gestart." },
    { 243, L"&Install", L"&Installa", L"&Instalar", L"&Installer", L"&Yükle", L"&Установить", L"&Instalar", L"&安装", L"&Zainstaluj", L"&Installeren" },
    { 244, L"Windows Update needs your help", L"Windows Update richiede il tuo intervento", L"Windows Update necesita su ayuda", L"Windows Update a besoin de votre aide", L"Windows Update yardımınızı gerektiriyor", L"Центру обновления Windows нужна ваша помощь", L"O Windows Update precisa da sua ajuda", L"Windows 更新需要你的帮助", L"Windows Update wymaga Twojej pomocy", L"Windows Update heeft uw hulp nodig" },
    { 245, L"Windows Update hasn't been able to check for new updates for the last 30 days. Go to Windows Update to resolve this issue.", L"Windows Update non è riuscito a cercare nuovi aggiornamenti negli ultimi 30 giorni. Vai a Windows Update per risolvere il problema.", L"Windows Update no ha podido buscar nuevas actualizaciones en los últimos 30 días. Vaya a Windows Update para resolver este problema.", L"Windows Update n'a pas pu rechercher de nouvelles mises à jour pendant les 30 derniers jours. Accédez à Windows Update pour résoudre ce problème.", L"Windows Update son 30 gündür yeni güncellemeleri denetleyemedi. Bu sorunu çözmek için Windows Update'e gidin.", L"Центр обновления Windows не мог проверять наличие новых обновлений последние 30 дней. Перейдите в Центр обновления Windows, чтобы решить эту проблему.", L"O Windows Update não consegue verificar novas atualizações há 30 dias. Vá para Windows Update para resolver esse problema.", L"Windows 更新在过去 30 天内无法检查新更新。请转到 Windows 更新以解决此问题。", L"Windows Update nie mógł sprawdzać nowych aktualizacji przez ostatnie 30 dni. Przejdź do Windows Update, aby rozwiązać ten problem.", L"Windows Update kan al 30 dagen geen nieuwe updates controleren. Ga naar Windows Update om dit probleem op te lossen." },
    { 246, L"Go to &Windows Update", L"Vai a &Windows Update", L"Ir a &Windows Update", L"Accéder à &Windows Update", L"&Windows Update'e git", L"Перейти в &Центр обновления Windows", L"Ir para &Windows Update", L"转到 &Windows 更新", L"Przejdź do &Windows Update", L"Naar &Windows Update gaan" },
    { 247, L"Your PC needs to restart to install a firmware update. If you've already saved everything, you can restart now. Otherwise, you should take a moment to save your work.", L"Il tuo PC deve essere riavviato per installare un aggiornamento del firmware. Se hai già salvato tutto, puoi riavviare ora. In caso contrario, prenditi un momento per salvare il lavoro.", L"Su PC debe reiniciarse para instalar una actualización de firmware. Si ya guardó todo, puede reiniciar ahora. De lo contrario, debe tomarse un momento para guardar su trabajo.", L"Votre PC doit redémarrer pour installer une mise à jour du firmware. Si vous avez déjà tout enregistré, vous pouvez redémarrer maintenant. Sinon, prenez un moment pour enregistrer votre travail.", L"Bilgisayarınızın bir üretici yazılımı güncellemesi yüklemek için yeniden başlatılması gerekiyor. Her şeyi kaydettiyseniz şimdi yeniden başlatabilirsiniz. Aksi takdirde, çalışmanızı kaydetmek için bir dakikanızı ayırın.", L"Компьютер необходимо перезапустить, чтобы установить обновление встроенного ПО. Если вы уже сохранили все, можно перезапустить сейчас. В противном случае сохраните свою работу.", L"Seu PC precisa ser reiniciado para instalar uma atualização de firmware. Se já salvou tudo, reinicie agora. Caso contrário, reserve um momento para salvar seu trabalho.", L"你的电脑需要重启才能安装固件更新。如果已保存所有内容，现在即可重启。否则，请花点时间保存你的工作。", L"Komputer należy ponownie uruchomić, aby zainstalować aktualizację oprogramowania układowego. Jeśli wszystko zapisałeś, uruchom ponownie teraz. W przeciwnym razie poświęć chwilę na zapisanie pracy.", L"Uw pc moet opnieuw worden gestart om een firmware-update te installeren. Als u alles al hebt opgeslagen, kunt u nu opnieuw starten. Anders neemt u even de tijd om uw werk op te slaan." },
    { 248, L"This will update your PC's hardware to help make it more stable. You can install this update by going to Windows Update.", L"Questo aggiornerà l'hardware del tuo PC per renderlo più stabile. Puoi installare questo aggiornamento andando in Windows Update.", L"Esto actualizará el hardware de su PC para ayudarlo a ser más estable. Puede instalar esta actualización yendo a Windows Update.", L"Cela mettra à jour le matériel de votre PC pour le rendre plus stable. Vous pouvez installer cette mise à jour en accédant à Windows Update.", L"Bu, bilgisayarınızın donanımını daha kararlı hale getirmek için güncelleyecektir. Bu güncellemeyi Windows Update'e giderek yükleyebilirsiniz.", L"Это обновит оборудование компьютера, чтобы сделать его более стабильным. Вы можете установить это обновление, перейдя в Центр обновления Windows.", L"Isso atualizará o hardware do seu PC para torná-lo mais estável. Você pode instalar esta atualização indo para Windows Update.", L"这将更新你电脑的硬件以使其更稳定。你可以通过转到 Windows 更新来安装此更新。", L"To zaktualizuje sprzęt komputera, aby był bardziej stabilny. Tę aktualizację możesz zainstalować, przechodząc do Windows Update.", L"Dit werkt de hardware van uw pc bij om deze stabieler te maken. U kunt deze update installeren door naar Windows Update te gaan." },
    { 249, L"Restarting in %1!d! minute, %2!d! seconds", L"Riavvio tra %1!d! minuto e %2!d! secondi", L"Reiniciando en %1!d! minuto y %2!d! segundos", L"Redémarrage dans %1!d! minute et %2!d! secondes", L"%1!d! dakika %2!d! saniye içinde yeniden başlatılıyor", L"Перезапуск через %1!d! мин и %2!d! с", L"Reiniciando em %1!d! minuto e %2!d! segundos", L"将在 %1!d! 分 %2!d! 秒后重启", L"Uruchamianie ponowne za %1!d! minutę i %2!d! sekund", L"Opnieuw starten over %1!d! minuut en %2!d! seconden" },
    { 250, L"Restarting in %1!d! minutes, %2!d! second", L"Riavvio tra %1!d! minuti e %2!d! secondo", L"Reiniciando en %1!d! minutos y %2!d! segundo", L"Redémarrage dans %1!d! minutes et %2!d! seconde", L"%1!d! dakika %2!d! saniye içinde yeniden başlatılıyor", L"Перезапуск через %1!d! мин и %2!d! с", L"Reiniciando em %1!d! minutos e %2!d! segundo", L"将在 %1!d! 分 %2!d! 秒后重启", L"Uruchamianie ponowne za %1!d! minut i %2!d! sekundę", L"Opnieuw starten over %1!d! minuten en %2!d! seconde" },
    { 251, L"Restarting in %1!d! minute, %2!d! second", L"Riavvio tra %1!d! minuto e %2!d! secondo", L"Reiniciando en %1!d! minuto y %2!d! segundo", L"Redémarrage dans %1!d! minute et %2!d! seconde", L"%1!d! dakika %2!d! saniye içinde yeniden başlatılıyor", L"Перезапуск через %1!d! мин и %2!d! с", L"Reiniciando em %1!d! minuto e %2!d! segundo", L"将在 %1!d! 分 %2!d! 秒后重启", L"Uruchamianie ponowne za %1!d! minutę i %2!d! sekundę", L"Opnieuw starten over %1!d! minuut en %2!d! seconde" },
    { 252, L"Restarting in %1!d! second", L"Riavvio tra %1!d! secondo", L"Reiniciando en %1!d! segundo", L"Redémarrage dans %1!d! seconde", L"%1!d! saniye içinde yeniden başlatılıyor", L"Перезапуск через %1!d! с", L"Reiniciando em %1!d! segundo", L"将在 %1!d! 秒后重启", L"Uruchamianie ponowne za %1!d! sekundę", L"Opnieuw starten over %1!d! seconde" },
    { 253, L"A firmware update is available", L"È disponibile un aggiornamento del firmware", L"Hay una actualización de firmware disponible", L"Une mise à jour du firmware est disponible", L"Bir üretici yazılımı güncellemesi kullanılabilir", L"Доступно обновление встроенного ПО", L"Há uma atualização de firmware disponível", L"有可用的固件更新", L"Dostępna jest aktualizacja oprogramowania układowego", L"Er is een firmware-update beschikbaar" },
    { 254, L"&Later", L"&Dopo", L"&Más tarde", L"&Plus tard", L"&Sonra", L"&Позже", L"&Mais tarde", L"&稍后", L"&Później", L"&Later" },
    { 300, L"%1 is also available", L"È disponibile anche %1", L"%1 también está disponible", L"%1 est également disponible", L"%1 de kullanılabilir", L"%1 также доступно", L"%1 também está disponível", L"%1 也可用", L"%1 jest również dostępne", L"%1 is ook beschikbaar" },
    { 301, L"%1 are also available", L"%1 sono anche disponibili", L"%1 también están disponibles", L"%1 sont également disponibles", L"%1 de kullanılabilir", L"%1 также доступны", L"%1 também estão disponíveis", L"%1 也可用", L"%1 są również dostępne", L"%1 zijn ook beschikbaar" },
    { 302, L"%1 is available", L"%1 è disponibile", L"%1 está disponible", L"%1 est disponible", L"%1 kullanılabilir", L"%1 доступно", L"%1 está disponível", L"%1 可用", L"%1 jest dostępne", L"%1 is beschikbaar" },
    { 303, L"%1 are available", L"%1 sono disponibili", L"%1 están disponibles", L"%1 sont disponibles", L"%1 kullanılabilir", L"%1 доступны", L"%1 estão disponíveis", L"%1 可用", L"%1 są dostępne", L"%1 zijn beschikbaar" },
    { 304, L"No important updates available", L"Nessun aggiornamento importante disponibile", L"No hay actualizaciones importantes disponibles", L"Aucune mise à jour importante disponible", L"Önemli güncelleme yok", L"Нет доступных важных обновлений", L"Nenhuma atualização importante disponível", L"没有可用的重要更新", L"Brak ważnych aktualizacji", L"Geen belangrijke updates beschikbaar" },
    { 305, L"There are no important updates available for your PC.", L"Non sono disponibili aggiornamenti importanti per il tuo PC.", L"No hay actualizaciones importantes disponibles para su PC.", L"Aucune mise à jour importante n'est disponible pour votre PC.", L"Bilgisayarınız için önemli güncelleme yok.", L"Нет доступных важных обновлений для вашего компьютера.", L"Não há atualizações importantes disponíveis para seu PC.", L"没有适用于你电脑的重要更新。", L"Brak ważnych aktualizacji dla Twojego komputera.", L"Er zijn geen belangrijke updates beschikbaar voor uw pc." },
    { 306, L"There are no updates available for your PC.", L"Non sono disponibili aggiornamenti per il tuo PC.", L"No hay actualizaciones disponibles para su PC.", L"Aucune mise à jour n'est disponible pour votre PC.", L"Bilgisayarınız için güncelleme yok.", L"Нет доступных обновлений для вашего компьютера.", L"Não há atualizações disponíveis para seu PC.", L"没有适用于你电脑的更新。", L"Brak aktualizacji dla Twojego komputera.", L"Er zijn geen updates beschikbaar voor uw pc." },
    { 308, L"%d optional updates", L"%d aggiornamenti facoltativi", L"%d actualizaciones opcionales", L"%d mises à jour facultatives", L"%d isteğe bağlı güncelleme", L"%d необязательных обновлений", L"%d atualizações opcionais", L"%d 个可选更新", L"%d opcjonalnych aktualizacji", L"%d optionele updates" },
    { 318, L"Important updates", L"Aggiornamenti importanti", L"Actualizaciones importantes", L"Mises à jour importantes", L"Önemli güncellemeler", L"Важные обновления", L"Atualizações importantes", L"重要更新", L"Ważne aktualizacje", L"Belangrijke updates" },
    { 319, L"Important and recommended updates", L"Aggiornamenti importanti e consigliati", L"Actualizaciones importantes y recomendadas", L"Mises à jour importantes et recommandées", L"Önemli ve önerilen güncellemeler", L"Важные и рекомендуемые обновления", L"Atualizações importantes e recomendadas", L"重要和推荐更新", L"Ważne i zalecane aktualizacje", L"Belangrijke en aanbevolen updates" },
    { 320, L"Optional updates", L"Aggiornamenti facoltativi", L"Actualizaciones opcionales", L"Mises à jour facultatives", L"İsteğe bağlı güncellemeler", L"Необязательные обновления", L"Atualizações opcionais", L"可选更新", L"Opcjonalne aktualizacje", L"Optionele updates" },
    { 321, L"Recommended and optional updates", L"Aggiornamenti consigliati e facoltativi", L"Actualizaciones recomendadas y opcionales", L"Mises à jour recommandées et facultatives", L"Önerilen ve isteğe bağlı güncellemeler", L"Рекомендуемые и необязательные обновления", L"Atualizações recomendadas e opcionais", L"推荐和可选更新", L"Zalecane i opcjonalne aktualizacje", L"Aanbevolen en optionele updates" },
    { 323, L"Total selected: %s, %s (%s)", L"Selezionati in totale: %s, %s (%s)", L"Seleccionados en total: %s, %s (%s)", L"Sélectionnés au total : %s, %s (%s)", L"Toplam seçilen: %s, %s (%s)", L"Выбрано всего: %s, %s (%s)", L"Selecionadas no total: %s, %s (%s)", L"总选定的项：%s、%s（%s）", L"Łącznie wybrano: %s, %s (%s)", L"Totaal geselecteerd: %s, %s (%s)" },
    { 324, L"It is recommended to use the system settings to configure updates.", L"Si consiglia di utilizzare le impostazioni del sistema per configurare gli aggiornamenti.", L"Se recomienda usar la configuración del sistema para configurar las actualizaciones.", L"Il est recommandé d'utiliser les paramètres du système pour configurer les mises à jour.", L"Güncellemeleri yapılandırmak için sistem ayarlarını kullanmanız önerilir.", L"Рекомендуется использовать параметры системы для настройки обновлений.", L"Recomenda-se usar as configurações do sistema para configurar as atualizações.", L"建议使用系统设置来配置更新。", L"Zaleca się korzystanie z ustawień systemowych w celu skonfigurowania aktualizacji.", L"Het wordt aanbevolen om de systeeminstellingen te gebruiken om updates te configureren." },
    { 325, L"Importance", L"Importanza", L"Importancia", L"Importance", L"Önem", L"Важность", L"Importância", L"重要性", L"Ważność", L"Belangrijkheid" },
    { 326, L"Size", L"Dimensione", L"Tamaño", L"Taille", L"Boyut", L"Размер", L"Tamanho", L"大小", L"Rozmiar", L"Grootte" },
    { 327, L"Select optional updates to install", L"Seleziona gli aggiornamenti facoltativi da installare", L"Seleccionar las actualizaciones opcionales que se van a instalar", L"Sélectionner les mises à jour facultatives à installer", L"Yüklenecek isteğe bağlı güncellemeleri seçin", L"Выбор необязательных обновлений для установки", L"Selecione as atualizações opcionais a instalar", L"选择要安装的可选更新", L"Wybierz opcjonalne aktualizacje do zainstalowania", L"Selecteer optionele updates om te installeren" },
    { 328, L"Review optional updates", L"Rivedi gli aggiornamenti facoltativi", L"Revisar las actualizaciones opcionales", L"Passer en revue les mises à jour facultatives", L"İsteğe bağlı güncellemeleri gözden geçirin", L"Просмотр необязательных обновлений", L"Revisar atualizações opcionais", L"查看可选更新", L"Przejrzyj opcjonalne aktualizacje", L"Optionele updates controleren" },
    { 329, L"Select important updates to install", L"Seleziona gli aggiornamenti importanti da installare", L"Seleccionar las actualizaciones importantes que se van a instalar", L"Sélectionner les mises à jour importantes à installer", L"Yüklenecek önemli güncellemeleri seçin", L"Выбор важных обновлений для установки", L"Selecione as atualizações importantes a instalar", L"选择要安装的重要更新", L"Wybierz ważne aktualizacje do zainstalowania", L"Selecteer belangrijke updates om te installeren" },
    { 330, L"Review important updates", L"Rivedi gli aggiornamenti importanti", L"Revisar las actualizaciones importantes", L"Passer en revue les mises à jour importantes", L"Önemli güncellemeleri gözden geçirin", L"Просмотр важных обновлений", L"Revisar atualizações importantes", L"查看重要更新", L"Przejrzyj ważne aktualizacje", L"Belangrijke updates controleren" },
    { 331, L"Review all important updates", L"Rivedi tutti gli aggiornamenti importanti", L"Revisar todas las actualizaciones importantes", L"Passer en revue toutes les mises à jour importantes", L"Tüm önemli güncellemeleri gözden geçirin", L"Просмотр всех важных обновлений", L"Revisar todas as atualizações importantes", L"查看所有重要更新", L"Przejrzyj wszystkie ważne aktualizacje", L"Alle belangrijke updates controleren" },
    { 333, L"Downloaded", L"Scaricato", L"Descargada", L"Téléchargée", L"İndirildi", L"Загружено", L"Baixado", L"已下载", L"Pobrano", L"Gedownload" },
    { 334, L"Install updates automatically (recommended)", L"Installa aggiornamenti automaticamente (consigliato)", L"Instalar actualizaciones automáticamente (recomendado)", L"Installer automatiquement les mises à jour (recommandé)", L"Güncellemeleri otomatik olarak yükle (önerilir)", L"Автоматически устанавливать обновления (рекомендуется)", L"Instalar atualizações automaticamente (recomendado)", L"自动安装更新（推荐）", L"Automatycznie instaluj aktualizacje (zalecane)", L"Updates automatisch installeren (aanbevolen)" },
    { 335, L"Download updates but let me choose whether to install them", L"Scarica gli aggiornamenti ma lascia che sia io a decidere se installarli", L"Descargar actualizaciones pero permitirme elegir si instalarlas", L"Télécharger les mises à jour mais me laisser choisir de les installer", L"Güncellemeleri indir, ancak bunları yükleyip yüklemeyeceğimi ben seçeyim", L"Загружать обновления, но я сам решу, устанавливать ли их", L"Baixar atualizações, mas deixar que eu escolha se desejo instalá-las", L"下载更新，但让我选择是否安装", L"Pobieraj aktualizacje, ale pozwól mi wybrać, czy je zainstalować", L"Updates downloaden, maar mij laten kiezen of ik ze wil installeren" },
    { 336, L"Check for updates but let me choose whether to download and install them", L"Controlla gli aggiornamenti ma lascia che sia io a decidere se scaricarli e installarli", L"Comprobar actualizaciones pero permitirme elegir si descargarlas e instalarlas", L"Rechercher les mises à jour mais me laisser choisir de les télécharger et de les installer", L"Güncellemeleri denetle, ancak bunları indirip yükleyip yüklemeyeceğimi ben seçeyim", L"Проверять обновления, но я сам решу, загружать и устанавливать ли их", L"Verificar atualizações, mas deixar que eu escolha se desejo baixá-las e instalá-las", L"检查更新，但让我选择是否下载和安装", L"Sprawdzaj aktualizacje, ale pozwól mi wybrać, czy je pobrać i zainstalować", L"Controleren op updates, maar mij laten kiezen of ik ze wil downloaden en installeren" },
    { 337, L"Never check for updates (not recommended)", L"Non controllare mai gli aggiornamenti (sconsigliato)", L"No comprobar nunca las actualizaciones (no recomendado)", L"Ne jamais rechercher les mises à jour (non recommandé)", L"Güncellemeleri hiç denetleme (önerilmez)", L"Никогда не проверять обновления (не рекомендуется)", L"Nunca verificar atualizações (não recomendado)", L"从不检查更新（不推荐）", L"Nigdy nie sprawdzaj aktualizacji (niezalecane)", L"Nooit naar updates zoeken (niet aanbevolen)" },
    { 338, L"Please select an option:", L"Seleziona un'opzione:", L"Seleccione una opción:", L"Veuillez sélectionner une option :", L"Lütfen bir seçenek seçin:", L"Выберите вариант:", L"Selecione uma opção:", L"请选择选项：", L"Wybierz opcję:", L"Selecteer een optie:" },
    { 339, L"Download and install your selected updates", L"Scarica e installa gli aggiornamenti selezionati", L"Descargar e instalar las actualizaciones seleccionadas", L"Télécharger et installer les mises à jour sélectionnées", L"Seçilen güncellemeleri indirip yükleyin", L"Загрузить и установить выбранные обновления", L"Baixar e instalar as atualizações selecionadas", L"下载并安装所选更新", L"Pobierz i zainstaluj wybrane aktualizacje", L"Download de geselecteerde updates en installeer ze" },
    { 340, L"Install your selected updates", L"Installa gli aggiornamenti selezionati", L"Instalar las actualizaciones seleccionadas", L"Installer les mises à jour sélectionnées", L"Seçilen güncellemeleri yükleyin", L"Установить выбранные обновления", L"Instalar as atualizações selecionadas", L"安装所选更新", L"Zainstaluj wybrane aktualizacje", L"Installeer de geselecteerde updates" },
    { 341, L"%1!lu! important updates", L"%1!lu! aggiornamenti importanti", L"%1!lu! actualizaciones importantes", L"%1!lu! mises à jour importantes", L"%1!lu! önemli güncelleme", L"%1!lu! важных обновлений", L"%1!lu! atualizações importantes", L"%1!lu! 个重要更新", L"%1!lu! ważnych aktualizacji", L"%1!lu! belangrijke updates" },
    { 345, L"Windows Update can't check for updates because the service is not running. You may need to restart your PC.", L"Windows Update non può cercare aggiornamenti perché il servizio non è in esecuzione. Potrebbe essere necessario riavviare il PC.", L"Windows Update no puede buscar actualizaciones porque el servicio no se está ejecutando. Es posible que deba reiniciar su PC.", L"Windows Update ne peut pas rechercher les mises à jour car le service n'est pas en cours d'exécution. Vous devrez peut-être redémarrer votre PC.", L"Windows Update, hizmet çalışmadığı için güncellemeleri denetleyemiyor. Bilgisayarınızı yeniden başlatmanız gerekebilir.", L"Центр обновления Windows не может проверить наличие обновлений, так как служба не запущена. Возможно, потребуется перезапустить компьютер.", L"O Windows Update não pode verificar atualizações porque o serviço não está em execução. Talvez seja necessário reiniciar o PC.", L"Windows 更新无法检查更新，因为服务未运行。你可能需要重启电脑。", L"Windows Update nie może sprawdzać aktualizacji, ponieważ usługa nie jest uruchomiona. Może być konieczne ponowne uruchomienie komputera.", L"Windows Update kan geen updates controleren omdat de service niet wordt uitgevoerd. Mogelijk moet u uw pc opnieuw starten." },
    { 347, L"Before Windows Update can check for updates, you must first configure Windows Update's settings. You can do this using the 'Change settings' link located below the 'Check for updates' link.", L"Prima che Windows Update possa cercare aggiornamenti, devi configurare le impostazioni di Windows Update. Puoi farlo usando il collegamento 'Cambia impostazioni' situato sotto il collegamento 'Controlla aggiornamenti'.", L"Antes de que Windows Update pueda buscar actualizaciones, primero debe configurar los parámetros de Windows Update. Puede hacerlo mediante el vínculo 'Cambiar la configuración' situado debajo del vínculo 'Buscar actualizaciones'.", L"Avant que Windows Update puisse rechercher des mises à jour, vous devez d'abord configurer les paramètres de Windows Update. Vous pouvez le faire à l'aide du lien « Modifier les paramètres » situé sous le lien « Rechercher les mises à jour ».", L"Windows Update güncellemeleri denetlemeden önce Windows Update ayarlarını yapılandırmanız gerekir. Bunu 'Denetle' bağlantısının altında bulunan 'Ayarları değiştir' bağlantısını kullanarak yapabilirsiniz.", L"Прежде чем Центр обновления Windows сможет проверить наличие обновлений, необходимо настроить его параметры. Это можно сделать с помощью ссылки «Изменить параметры», расположенной ниже ссылки «Проверить наличие обновлений».", L"Antes que o Windows Update possa verificar atualizações, você deve primeiro configurar as configurações do Windows Update. Você pode fazer isso usando o link 'Alterar configurações' localizado abaixo do link 'Verificar atualizações'.", L"在 Windows 更新可以检查更新之前，必须先配置 Windows 更新的设置。你可以使用位于“检查更新”链接下方的“更改设置”链接进行配置。", L"Zanim Windows Update będzie mógł sprawdzać aktualizacje, musisz najpierw skonfigurować ustawienia Windows Update. Możesz to zrobić, używając łącza 'Zmień ustawienia' znajdującego się poniżej łącza 'Sprawdź aktualizacje'.", L"Voordat Windows Update updates kan controleren, moet u eerst de instellingen van Windows Update configureren. U kunt dit doen met de koppeling 'Instellingen wijzigen' onder de koppeling 'Controleren op updates'." },
    { 348, L"Windows Update is already checking for, downloading, or installing updates.", L"Windows Update sta già cercando, scaricando o installando aggiornamenti.", L"Windows Update ya está comprobando, descargando o instalando actualizaciones.", L"Windows Update recherche, télécharge ou installe déjà des mises à jour.", L"Windows Update zaten güncellemeleri denetliyor, indiriyor veya yüklüyor.", L"Центр обновления Windows уже проверяет, загружает или устанавливает обновления.", L"O Windows Update já está verificando, baixando ou instalando atualizações.", L"Windows 更新已在检查、下载或安装更新。", L"Windows Update już sprawdza, pobiera lub instaluje aktualizacje.", L"Windows Update controleert al op updates, downloadt of installeert deze." },
    { 349, L"Windows Update can't check for updates because settings on this PC are controlled by your system administrator.", L"Windows Update non può cercare aggiornamenti perché le impostazioni di questo PC sono controllate dall'amministratore di sistema.", L"Windows Update no puede buscar actualizaciones porque la configuración de este PC está controlada por el administrador del sistema.", L"Windows Update ne peut pas rechercher les mises à jour car les paramètres de ce PC sont contrôlés par votre administrateur système.", L"Windows Update, bu bilgisayardaki ayarlar sistem yöneticiniz tarafından denetlendiği için güncellemeleri denetleyemiyor.", L"Центр обновления Windows не может проверить наличие обновлений, так как параметры этого компьютера управляются системным администратором.", L"O Windows Update não pode verificar atualizações porque as configurações deste PC são controladas pelo administrador do sistema.", L"Windows 更新无法检查更新，因为此电脑上的设置由你的系统管理员控制。", L"Windows Update nie może sprawdzać aktualizacji, ponieważ ustawienia tego komputera są kontrolowane przez administratora systemu.", L"Windows Update kan geen updates controleren omdat de instellingen op deze pc door uw systeembeheerder worden beheerd." },
    { 350, L"Check for updates", L"Controlla aggiornamenti", L"Buscar actualizaciones", L"Rechercher des mises à jour", L"Güncellemeleri denetle", L"Проверить наличие обновлений", L"Verificar atualizações", L"检查更新", L"Sprawdź aktualizacje", L"Controleren op updates" },
    { 351, L"Change settings", L"Cambia impostazioni", L"Cambiar la configuración", L"Modifier les paramètres", L"Ayarları değiştir", L"Изменить параметры", L"Alterar configurações", L"更改设置", L"Zmień ustawienia", L"Instellingen wijzigen" },
    { 352, L"View update history", L"Visualizza cronologia aggiornamenti", L"Ver historial de actualizaciones", L"Afficher l'historique des mises à jour", L"Güncelleme geçmişini görüntüle", L"Просмотр журнала обновлений", L"Exibir histórico de atualizações", L"查看更新历史记录", L"Wyświetl historię aktualizacji", L"Updategeschiedenis weergeven" },
    { 353, L"Restore hidden updates", L"Ripristina aggiornamenti nascosti", L"Restaurar actualizaciones ocultas", L"Restaurer les mises à jour masquées", L"Gizli güncellemeleri geri yükle", L"Восстановить скрытые обновления", L"Restaurar atualizações ocultas", L"还原隐藏的更新", L"Przywróć ukryte aktualizacje", L"Verborgen updates herstellen" },
    { 355, L"Security Center", L"Centro sicurezza", L"Centro de seguridad", L"Centre de sécurité", L"Güvenlik Merkezi", L"Центр безопасности", L"Central de Segurança", L"安全中心", L"Centrum zabezpieczeń", L"Beveiligingscentrum" },
    { 356, L"Installed Updates", L"Aggiornamenti installati", L"Actualizaciones instaladas", L"Mises à jour installées", L"Yüklü Güncellemeler", L"Установленные обновления", L"Atualizações Instaladas", L"已安装的更新", L"Zainstalowane aktualizacje", L"Geïnstalleerde updates" },
    { 358, L"Add features to %WINDOWS_SHORT%", L"Aggiungi funzionalità a %WINDOWS_SHORT%", L"Agregar características a %WINDOWS_SHORT%", L"Ajouter des fonctionnalités à %WINDOWS_SHORT%", L"%WINDOWS_SHORT% için özellik ekle", L"Добавить компоненты в %WINDOWS_SHORT%", L"Adicionar recursos ao %WINDOWS_SHORT%", L"向 %WINDOWS_SHORT% 添加功能", L"Dodaj funkcje do %WINDOWS_SHORT%", L"Functies toevoegen aan %WINDOWS_SHORT%" },
    { 371, L"%1 selected", L"%1 selezionati", L"%1 seleccionados", L"%1 sélectionnés", L"%1 seçildi", L"Выбрано: %1", L"%1 selecionadas", L"已选定 %1", L"Wybrano: %1", L"%1 geselecteerd" },
    { 372, L"%1 selected", L"%1 selezionati", L"%1 seleccionados", L"%1 sélectionnés", L"%1 seçildi", L"Выбрано: %1", L"%1 selecionadas", L"已选定 %1", L"Wybrano: %1", L"%1 geselecteerd" },
    { 373, L"The updates were installed", L"Gli aggiornamenti sono stati installati", L"Las actualizaciones se instalaron", L"Les mises à jour ont été installées", L"Güncellemeler yüklendi", L"Обновления установлены", L"As atualizações foram instaladas", L"更新已安装", L"Aktualizacje zostały zainstalowane", L"De updates zijn geïnstalleerd" },
    { 374, L"Some updates were not installed", L"Alcuni aggiornamenti non sono stati installati", L"Algunas actualizaciones no se instalaron", L"Certaines mises à jour n'ont pas été installées", L"Bazı güncellemeler yüklenmedi", L"Некоторые обновления не были установлены", L"Algumas atualizações não foram instaladas", L"某些更新未安装", L"Niektóre aktualizacje nie zostały zainstalowane", L"Sommige updates zijn niet geïnstalleerd" },
    { 375, L"1 pending important update", L"1 aggiornamento importante in sospeso", L"1 actualización importante pendiente", L"1 mise à jour importante en attente", L"1 bekleyen önemli güncelleme", L"1 ожидающее важное обновление", L"1 atualização importante pendente", L"1 个待处理的重要更新", L"1 oczekująca ważna aktualizacja", L"1 lopende belangrijke update" },
    { 376, L"%d pending important updates", L"%d aggiornamenti importanti in sospeso", L"%d actualizaciones importantes pendientes", L"%d mises à jour importantes en attente", L"%d bekleyen önemli güncelleme", L"%d ожидающих важных обновлений", L"%d atualizações importantes pendentes", L"%d 个待处理的重要更新", L"%d oczekujących ważnych aktualizacji", L"%d lopende belangrijke updates" },
    { 377, L"%1!lu! pending important updates", L"%1!lu! aggiornamenti importanti in sospeso", L"%1!lu! actualizaciones importantes pendientes", L"%1!lu! mises à jour importantes en attente", L"%1!lu! bekleyen önemli güncelleme", L"%1!lu! ожидающих важных обновлений", L"%1!lu! atualizações importantes pendentes", L"%1!lu! 个待处理的重要更新", L"%1!lu! oczekujących ważnych aktualizacji", L"%1!lu! lopende belangrijke updates" },
    { 378, L"There was a problem checking for updates.", L"Si è verificato un problema durante la ricerca degli aggiornamenti.", L"Hubo un problema al buscar actualizaciones.", L"Un problème est survenu lors de la recherche des mises à jour.", L"Güncellemeler denetlenirken bir sorun oluştu.", L"При проверке обновлений возникла проблема.", L"Houve um problema ao verificar atualizações.", L"检查更新时出现问题。", L"Wystąpił problem podczas sprawdzania aktualizacji.", L"Er is een probleem opgetreden bij het controleren op updates." },
    { 380, L"&Hide updates", L"&Nascondi aggiornamenti", L"&Ocultar actualizaciones", L"&Masquer les mises à jour", L"Güncellemeleri &gizle", L"&Скрыть обновления", L"&Ocultar atualizações", L"&隐藏更新", L"&Ukryj aktualizacje", L"Updates &verbergen" },
    { 381, L"&Restore updates", L"&Ripristina aggiornamenti", L"&Restaurar actualizaciones", L"&Restaurer les mises à jour", L"Güncellemeleri &geri yükle", L"&Восстановить обновления", L"&Restaurar atualizações", L"&还原更新", L"&Przywróć aktualizacje", L"Updates &herstellen" },
    { 382, L"Updates are available for your PC", L"Sono disponibili aggiornamenti per il tuo PC", L"Hay actualizaciones disponibles para su PC", L"Des mises à jour sont disponibles pour votre PC", L"Bilgisayarınız için güncellemeler kullanılabilir", L"Для вашего компьютера доступны обновления", L"Há atualizações disponíveis para seu PC", L"有适用于你电脑的更新", L"Dostępne są aktualizacje dla Twojego komputera", L"Er zijn updates beschikbaar voor uw pc" },
    { 383, L"Important updates are available for your PC", L"Sono disponibili aggiornamenti importanti per il tuo PC", L"Hay actualizaciones importantes disponibles para su PC", L"Des mises à jour importantes sont disponibles pour votre PC", L"Bilgisayarınız için önemli güncellemeler kullanılabilir", L"Для вашего компьютера доступны важные обновления", L"Há atualizações importantes disponíveis para seu PC", L"有适用于你电脑的重要更新", L"Dostępne są ważne aktualizacje dla Twojego komputera", L"Er zijn belangrijke updates beschikbaar voor uw pc" },
    { 384, L"%1 selected, %2", L"%1 selezionati, %2", L"%1 seleccionados, %2", L"%1 sélectionnés, %2", L"%1 seçildi, %2", L"Выбрано: %1, %2", L"%1 selecionadas, %2", L"已选定 %1，%2", L"Wybrano: %1, %2", L"%1 geselecteerd, %2" },
    { 385, L"There was a problem getting the list of updates for your PC. To continue, please reopen Windows Update. (Error code: %1!X!)", L"Si è verificato un problema durante il recupero dell'elenco degli aggiornamenti per il tuo PC. Per continuare, riapri Windows Update. (Codice di errore: %1!X!)", L"Hubo un problema al obtener la lista de actualizaciones para su PC. Para continuar, vuelva a abrir Windows Update. (Código de error: %1!X!)", L"Un problème est survenu lors de l'obtention de la liste des mises à jour pour votre PC. Pour continuer, rouvrez Windows Update. (Code d'erreur : %1!X!)", L"Bilgisayarınız için güncelleme listesi alınırken bir sorun oluştu. Devam etmek için Windows Update'i yeniden açın. (Hata kodu: %1!X!)", L"При получении списка обновлений для компьютера возникла проблема. Чтобы продолжить, снова откройте Центр обновления Windows. (Код ошибки: %1!X!)", L"Houve um problema ao obter a lista de atualizações para seu PC. Para continuar, reabra o Windows Update. (Código de erro: %1!X!)", L"获取你电脑的更新列表时出现问题。若要继续，请重新打开 Windows 更新。（错误代码：%1!X!）", L"Wystąpił problem podczas pobierania listy aktualizacji dla Twojego komputera. Aby kontynuować, otwórz ponownie Windows Update. (Kod błędu: %1!X!)", L"Er is een probleem opgetreden bij het ophalen van de updatelijst voor uw pc. Open Windows Update opnieuw om verder te gaan. (Foutcode: %1!X!)" },
    { 386, L"System Firmware Update - %s", L"Aggiornamento firmware di sistema - %s", L"Actualización de firmware del sistema - %s", L"Mise à jour du firmware système - %s", L"Sistem Üretici Yazılımı Güncellemesi - %s", L"Обновление системного встроенного ПО - %s", L"Atualização de firmware do sistema - %s", L"系统固件更新 - %s", L"Aktualizacja oprogramowania układowego systemu - %s", L"Systeemfirmware-update - %s" },
    { 387, L"System Hardware Update - %s", L"Aggiornamento hardware di sistema - %s", L"Actualización de hardware del sistema - %s", L"Mise à jour du matériel système - %s", L"Sistem Donanımı Güncellemesi - %s", L"Обновление системного оборудования - %s", L"Atualização de hardware do sistema - %s", L"系统硬件更新 - %s", L"Aktualizacja sprzętu systemu - %s", L"Systeemhardware-update - %s" },
    { 388, L"These updates are released by PC manufacturers to help improve the stability and performance of PC hardware.", L"Questi aggiornamenti vengono rilasciati dai produttori di PC per migliorare stabilità e prestazioni dell'hardware del PC.", L"Estas actualizaciones las publican los fabricantes de PC para ayudar a mejorar la estabilidad y el rendimiento del hardware del PC.", L"Ces mises à jour sont publiées par les fabricants de PC pour améliorer la stabilité et les performances du matériel du PC.", L"Bu güncellemeler, bilgisayar donanımının kararlılığını ve performansını iyileştirmek için PC üreticileri tarafından yayımlanır.", L"Эти обновления выпускаются производителями ПК для повышения стабильности и производительности оборудования.", L"Estas atualizações são lançadas pelos fabricantes de PCs para melhorar a estabilidade e o desempenho do hardware.", L"这些更新由电脑制造商发布，有助于提高电脑硬件的稳定性和性能。", L"Te aktualizacje są publikowane przez producentów komputerów, aby poprawić stabilność i wydajność sprzętu.", L"Deze updates worden door pc-fabrikanten uitgebracht om de stabiliteit en prestaties van pc-hardware te verbeteren." },
    { 450, L"Check", L"Controlla", L"Marcar", L"Cocher", L"İşaretle", L"Отметить", L"Marcar", L"选中", L"Zaznacz", L"Aanvinken" },
    { 451, L"Uncheck", L"Deseleziona", L"Desmarcar", L"Décocher", L"İşareti kaldır", L"Снять отметку", L"Desmarcar", L"取消选中", L"Odznacz", L"Uitvinken" },
    { 452, L"%1, %2", L"%1, %2", L"%1, %2", L"%1, %2", L"%1, %2", L"%1, %2", L"%1, %2", L"%1，%2", L"%1, %2", L"%1, %2" },
    { 453, L"Important update list, 1 update", L"Elenco aggiornamenti importanti, 1 aggiornamento", L"Lista de actualizaciones importantes, 1 actualización", L"Liste des mises à jour importantes, 1 mise à jour", L"Önemli güncelleme listesi, 1 güncelleme", L"Список важных обновлений: 1 обновление", L"Lista de atualizações importantes, 1 atualização", L"重要更新列表，1 个更新", L"Lista ważnych aktualizacji, 1 aktualizacja", L"Belangrijke updatelijst, 1 update" },
    { 454, L"Important update list, %d updates", L"Elenco aggiornamenti importanti, %d aggiornamenti", L"Lista de actualizaciones importantes, %d actualizaciones", L"Liste des mises à jour importantes, %d mises à jour", L"Önemli güncelleme listesi, %d güncelleme", L"Список важных обновлений: %d обновлений", L"Lista de atualizações importantes, %d atualizações", L"重要更新列表，%d 个更新", L"Lista ważnych aktualizacji, %d aktualizacji", L"Belangrijke updatelijst, %d updates" },
    { 455, L"Optional update list, 1 update", L"Elenco aggiornamenti facoltativi, 1 aggiornamento", L"Lista de actualizaciones opcionales, 1 actualización", L"Liste des mises à jour facultatives, 1 mise à jour", L"İsteğe bağlı güncelleme listesi, 1 güncelleme", L"Список необязательных обновлений: 1 обновление", L"Lista de atualizações opcionais, 1 atualização", L"可选更新列表，1 个更新", L"Lista opcjonalnych aktualizacji, 1 aktualizacja", L"Optionele updatelijst, 1 update" },
    { 456, L"Optional update list, %d updates", L"Elenco aggiornamenti facoltativi, %d aggiornamenti", L"Lista de actualizaciones opcionales, %d actualizaciones", L"Liste des mises à jour facultatives, %d mises à jour", L"İsteğe bağlı güncelleme listesi, %d güncelleme", L"Список необязательных обновлений: %d обновлений", L"Lista de atualizações opcionais, %d atualizações", L"可选更新列表，%d 个更新", L"Lista opcjonalnych aktualizacji, %d aktualizacji", L"Optionele updatelijst, %d updates" },
    { 457, L"Important update list, 1 update", L"Elenco aggiornamenti importanti, 1 aggiornamento", L"Lista de actualizaciones importantes, 1 actualización", L"Liste des mises à jour importantes, 1 mise à jour", L"Önemli güncelleme listesi, 1 güncelleme", L"Список важных обновлений: 1 обновление", L"Lista de atualizações importantes, 1 atualização", L"重要更新列表，1 个更新", L"Lista ważnych aktualizacji, 1 aktualizacja", L"Belangrijke updatelijst, 1 update" },
    { 458, L"Important update list, %d updates", L"Elenco aggiornamenti importanti, %d aggiornamenti", L"Lista de actualizaciones importantes, %d actualizaciones", L"Liste des mises à jour importantes, %d mises à jour", L"Önemli güncelleme listesi, %d güncelleme", L"Список важных обновлений: %d обновлений", L"Lista de atualizações importantes, %d atualizações", L"重要更新列表，%d 个更新", L"Lista ważnych aktualizacji, %d aktualizacji", L"Belangrijke updatelijst, %d updates" },
    { 459, L"Optional update list, 1 update", L"Elenco aggiornamenti facoltativi, 1 aggiornamento", L"Lista de actualizaciones opcionales, 1 actualización", L"Liste des mises à jour facultatives, 1 mise à jour", L"İsteğe bağlı güncelleme listesi, 1 güncelleme", L"Список необязательных обновлений: 1 обновление", L"Lista de atualizações opcionais, 1 atualização", L"可选更新列表，1 个更新", L"Lista opcjonalnych aktualizacji, 1 aktualizacja", L"Optionele updatelijst, 1 update" },
    { 460, L"Optional update list, %d updates", L"Elenco aggiornamenti facoltativi, %d aggiornamenti", L"Lista de actualizaciones opcionales, %d actualizaciones", L"Liste des mises à jour facultatives, %d mises à jour", L"İsteğe bağlı güncelleme listesi, %d güncelleme", L"Список необязательных обновлений: %d обновлений", L"Lista de atualizações opcionais, %d atualizações", L"可选更新列表，%d 个更新", L"Lista opcjonalnych aktualizacji, %d aktualizacji", L"Optionele updatelijst, %d updates" },
    { 475, L"&Give me updates for other Microsoft products when I update Windows", L"&Dammi aggiornamenti per altri prodotti Microsoft quando aggiorno Windows", L"&Darme actualizaciones para otros productos de Microsoft cuando actualizo Windows", L"Me donner les mises à jour pour d'autres produits Microsoft quand je mets à jour Windows", L"Windows'u güncellediğimde diğer Microsoft ürünleri için güncellemeler &ver", L"&Предоставлять обновления для других продуктов Microsoft при обновлении Windows", L"&Dar-me atualizações para outros produtos da Microsoft ao atualizar o Windows", L"更新 Windows 时为我提供其他 Microsoft 产品的更新（&G）", L"&Daj mi aktualizacje dla innych produktów Microsoft, gdy aktualizuję Windows", L"Geef mij updates voor andere Microsoft-producten wanneer ik Windows bijwerk (&G)" },
    { 480, L"Download is pending — select this update to start downloading it", L"Download in sospeso: seleziona questo aggiornamento per avviare il download", L"La descarga está pendiente: seleccione esta actualización para empezar a descargarla", L"Téléchargement en attente — sélectionnez cette mise à jour pour commencer le téléchargement", L"İndirme bekleniyor — indirmeye başlamak için bu güncellemeyi seçin", L"Загрузка ожидает — выберите это обновление, чтобы начать загрузку", L"Download pendente — selecione esta atualização para iniciar o download", L"下载处于挂起状态 — 选择此更新以开始下载", L"Pobieranie w toku — wybierz tę aktualizację, aby rozpocząć pobieranie", L"Download in behandeling — selecteer deze update om het downloaden te starten" },
    { 481, L"Total selected: %s (%s)", L"Selezionati in totale: %s (%s)", L"Seleccionados en total: %s (%s)", L"Sélectionnés au total : %s (%s)", L"Toplam seçilen: %s (%s)", L"Выбрано всего: %s (%s)", L"Selecionadas no total: %s (%s)", L"总选定的项：%s（%s）", L"Łącznie wybrano: %s (%s)", L"Totaal geselecteerd: %s (%s)" },
    { 482, L"See all available updates", L"Visualizza tutti gli aggiornamenti disponibili", L"Ver todas las actualizaciones disponibles", L"Voir toutes les mises à jour disponibles", L"Tüm kullanılabilir güncellemeleri gör", L"Просмотреть все доступные обновления", L"Ver todas as atualizações disponíveis", L"查看所有可用更新", L"Zobacz wszystkie dostępne aktualizacje", L"Alle beschikbare updates bekijken" },
    { 483, L"%1 will be installed.", L"%1 verrà installato.", L"%1 se instalará.", L"%1 sera installée.", L"%1 yüklenecek.", L"%1 будет установлено.", L"%1 será instalada.", L"将安装 %1。", L"%1 zostanie zainstalowana.", L"%1 wordt geïnstalleerd." },
    { 1000, L"Standard User Control", L"Controllo utente standard", L"Control de usuario estándar", L"Contrôle d'utilisateur standard", L"Standart Kullanıcı Denetimi", L"Стандартный контроль пользователя", L"Controle de usuário padrão", L"标准用户控制", L"Kontrola standardowego użytkownika", L"Standaardgebruikersbeheer" },
    { 1001, L"Allow standard users to install programs and updates with Windows Update.", L"Consenti agli utenti standard di installare programmi e aggiornamenti con Windows Update.", L"Permitir que los usuarios estándar instalen programas y actualizaciones con Windows Update.", L"Permettre aux utilisateurs standard d'installer des programmes et des mises à jour avec Windows Update.", L"Standart kullanıcıların Windows Update ile program ve güncelleme yüklemesine izin verin.", L"Разрешить стандартным пользователям устанавливать программы и обновления с помощью Центра обновления Windows.", L"Permitir que usuários padrão instalem programas e atualizações com o Windows Update.", L"允许标准用户使用 Windows 更新安装程序和更新。", L"Zezwól standardowym użytkownikom na instalowanie programów i aktualizacji za pomocą Windows Update.", L"Sta standaardgebruikers programma's en updates installeren met Windows Update." },
    { 1100, L"Choose your Windows Update settings", L"Scegli le impostazioni di Windows Update", L"Elija la configuración de Windows Update", L"Choisissez vos paramètres Windows Update", L"Windows Update ayarlarınızı seçin", L"Выбор параметров Центра обновления Windows", L"Escolha suas configurações do Windows Update", L"选择 Windows 更新设置", L"Wybierz ustawienia Windows Update", L"Kies uw Windows Update-instellingen" },
    { 1102, L"When your PC is online, Windows can automatically check for important updates and install them using these settings. When new updates are available, you can also choose to install them when you shut down your PC.", L"Quando il PC è online, Windows può cercare automaticamente gli aggiornamenti importanti e installarli usando queste impostazioni. Quando sono disponibili nuovi aggiornamenti, puoi anche scegliere di installarli all'arresto del PC.", L"Cuando su PC esté en línea, Windows puede buscar automáticamente actualizaciones importantes e instalarlas con esta configuración. Cuando haya nuevas actualizaciones disponibles, también puede elegir instalarlas al apagar su PC.", L"Lorsque votre PC est en ligne, Windows peut rechercher automatiquement les mises à jour importantes et les installer à l'aide de ces paramètres. Lorsque de nouvelles mises à jour sont disponibles, vous pouvez également choisir de les installer à l'arrêt de votre PC.", L"Bilgisayarınız çevrimiçiyken Windows önemli güncellemeleri otomatik olarak denetleyebilir ve bu ayarları kullanarak yükleyebilir. Yeni güncellemeler kullanılabilir olduğunda, bilgisayarınızı kapattığınızda bunları yüklemeyi de seçebilirsiniz.", L"Когда компьютер подключен к Интернету, Windows может автоматически проверять наличие важных обновлений и устанавливать их с помощью этих параметров. Когда доступны новые обновления, вы также можете установить их при завершении работы компьютера.", L"Quando seu PC estiver online, o Windows pode verificar automaticamente atualizações importantes e instalá-las usando estas configurações. Quando novas atualizações estiverem disponíveis, você também pode escolher instalá-las ao desligar o PC.", L"当你的电脑在线时，Windows 可以自动检查重要更新并使用这些设置进行安装。当有新更新可用时，你也可以选择在关闭电脑时安装它们。", L"Gdy komputer jest online, system Windows może automatycznie sprawdzać ważne aktualizacje i instalować je przy użyciu tych ustawień. Gdy dostępne są nowe aktualizacje, możesz również wybrać ich instalację podczas zamykania komputera.", L"Wanneer uw pc online is, kan Windows automatisch controleren op belangrijke updates en deze installeren met deze instellingen. Wanneer nieuwe updates beschikbaar zijn, kunt u er ook voor kiezen ze te installeren wanneer u uw pc afsluit." },
    { 1105, L"Check the Status column to ensure all important updates were successful. To remove an update, see <a id=\\\"actionViewInstalledUpdates\\\">Installed Updates</a>.", L"Controllare la colonna Stato per assicurarsi che tutti gli aggiornamenti importanti siano riusciti. Per rimuovere un aggiornamento, vedere <a id=\\\"actionViewInstalledUpdates\\\">Aggiornamenti installati</a>.", L"Compruebe la columna Estado para asegurarse de que todas las actualizaciones importantes fueron correctas. Para quitar una actualización, vea <a id=\\\"actionViewInstalledUpdates\\\">Actualizaciones instaladas</a>.", L"Vérifiez la colonne État pour vous assurer que toutes les mises à jour importantes ont réussi. Pour supprimer une mise à jour, consultez <a id=\\\"actionViewInstalledUpdates\\\">Mises à jour installées</a>.", L"Tüm önemli güncellemelerin başarılı olduğundan emin olmak için Durum sütununu denetleyin. Bir güncellemeyi kaldırmak için <a id=\\\"actionViewInstalledUpdates\\\">Yüklü Güncellemeler</a> bölümüne bakın.", L"Проверьте столбец «Состояние», чтобы убедиться, что все важные обновления установлены успешно. Чтобы удалить обновление, см. <a id=\\\"actionViewInstalledUpdates\\\">Установленные обновления</a>.", L"Verifique a coluna Status para garantir que todas as atualizações importantes foram concluídas. Para remover uma atualização, veja <a id=\\\"actionViewInstalledUpdates\\\">Atualizações instaladas</a>.", L"检查“状态”列以确保所有重要更新均成功。若要删除某个更新，请参阅<a id=\\\"actionViewInstalledUpdates\\\">已安装的更新</a>。", L"Sprawdź kolumnę Stan, aby upewnić się, że wszystkie ważne aktualizacje zakończyły się pomyślnie. Aby usunąć aktualizację, zobacz <a id=\\\"actionViewInstalledUpdates\\\">Zainstalowane aktualizacje</a>.", L"Controleer de kolom Status om ervoor te zorgen dat alle belangrijke updates zijn geslaagd. Raadpleeg <a id=\\\"actionViewInstalledUpdates\\\">Geïnstalleerde updates</a> om een update te verwijderen." },
    { 1107, L"Recommended updates", L"Aggiornamenti consigliati", L"Actualizaciones recomendadas", L"Mises à jour recommandées", L"Önerilen güncellemeler", L"Рекомендуемые обновления", L"Atualizações recomendadas", L"推荐更新", L"Zalecane aktualizacje", L"Aanbevolen updates" },
    { 1108, L"Windows can't update important files and services while the system is using them. Save any open files, and then restart the PC.", L"Windows non può aggiornare file e servizi importanti mentre il sistema li sta utilizzando. Salva i file aperti e riavvia il PC.", L"Windows no puede actualizar archivos y servicios importantes mientras el sistema los está usando. Guarde los archivos abiertos y reinicie el PC.", L"Windows ne peut pas mettre à jour des fichiers et services importants pendant que le système les utilise. Enregistrez les fichiers ouverts, puis redémarrez le PC.", L"Sistem bunları kullanırken Windows önemli dosyaları ve hizmetleri güncelleyemez. Açık dosyaları kaydedin ve bilgisayarı yeniden başlatın.", L"Windows не может обновить важные файлы и службы, пока они используются системой. Сохраните открытые файлы и перезапустите компьютер.", L"O Windows não pode atualizar arquivos e serviços importantes enquanto o sistema os está usando. Salve os arquivos abertos e reinicie o PC.", L"系统正在使用重要文件和服务时，Windows 无法更新它们。请保存打开的文件，然后重启电脑。", L"System Windows nie może zaktualizować ważnych plików i usług, gdy są używane. Zapisz otwarte pliki i uruchom ponownie komputer.", L"Windows kan geen belangrijke bestanden en services bijwerken terwijl het systeem ze gebruikt. Sla geopende bestanden op en start de pc opnieuw op." },
    { 1110, L"Windows can't update important files and services while the system is using them. Make sure to save your files before restarting.", L"Windows non può aggiornare file e servizi importanti mentre il sistema li sta utilizzando. Salva i file prima di riavviare.", L"Windows no puede actualizar archivos y servicios importantes mientras el sistema los está usando. Asegúrese de guardar sus archivos antes de reiniciar.", L"Windows ne peut pas mettre à jour des fichiers et services importants pendant que le système les utilise. Veillez à enregistrer vos fichiers avant de redémarrer.", L"Sistem bunları kullanırken Windows önemli dosyaları ve hizmetleri güncelleyemez. Yeniden başlatmadan önce dosyalarınızı kaydettiğinizden emin olun.", L"Windows не может обновить важные файлы и службы, пока они используются системой. Перед перезапуском обязательно сохраните файлы.", L"O Windows não pode atualizar arquivos e serviços importantes enquanto o sistema os está usando. Salve seus arquivos antes de reiniciar.", L"系统正在使用重要文件和服务时，Windows 无法更新它们。请确保在重启前保存文件。", L"System Windows nie może zaktualizować ważnych plików i usług, gdy są używane. Przed ponownym uruchomieniem zapisz pliki.", L"Windows kan geen belangrijke bestanden en services bijwerken terwijl het systeem ze gebruikt. Sla uw bestanden op voordat u opnieuw opstart." },
    { 1112, L"Update type: ", L"Tipo di aggiornamento: ", L"Tipo de actualización: ", L"Type de mise à jour : ", L"Güncelleme türü: ", L"Тип обновления: ", L"Tipo de atualização: ", L"更新类型：", L"Typ aktualizacji: ", L"Updatetype: " },
    { 1117, L"Give me &recommended updates the same way I receive important updates", L"Dammi gli aggiornamenti &consigliati come ricevo gli aggiornamenti importanti", L"Darme actualizaciones &recomendadas de la misma manera que recibo las importantes", L"Donnez-moi les mises à jour &recommandées de la même manière que je reçois les importantes", L"Önemli güncellemeleri aldığım şekilde &önerilen güncellemeleri de ver", L"Предоставлять &рекомендуемые обновления так же, как важные", L"Dar-me atualizações &recomendadas da mesma forma que recebo as importantes", L"以接收重要更新的相同方式为我提供推荐更新（&r）", L"Daj mi &zalecane aktualizacje w taki sam sposób, jak otrzymuję ważne", L"Geef mij &aanbevolen updates op dezelfde manier als ik belangrijke ontvang" },
    { 1118, L"Restore hidden updates", L"Ripristina aggiornamenti nascosti", L"Restaurar actualizaciones ocultas", L"Restaurer les mises à jour masquées", L"Gizli güncellemeleri geri yükle", L"Восстановить скрытые обновления", L"Restaurar atualizações ocultas", L"还原隐藏的更新", L"Przywróć ukryte aktualizacje", L"Verborgen updates herstellen" },
    { 1119, L"Cancel", L"Annulla", L"Cancelar", L"Annuler", L"İptal", L"Отмена", L"Cancelar", L"取消", L"Anuluj", L"Annuleren" },
    { 1121, L"Installation date: ", L"Data di installazione: ", L"Fecha de instalación: ", L"Date d'installation : ", L"Yükleme tarihi: ", L"Дата установки: ", L"Data de instalação: ", L"安装日期：", L"Data instalacji: ", L"Installatiedatum: " },
    { 1124, L"Installation status: ", L"Stato dell'installazione: ", L"Estado de instalación: ", L"État de l'installation : ", L"Yükleme durumu: ", L"Состояние установки: ", L"Status de instalação: ", L"安装状态：", L"Stan instalacji: ", L"Installatiestatus: " },
    { 1125, L"Error details: ", L"Dettagli errore: ", L"Detalles del error: ", L"Détails de l'erreur : ", L"Hata ayrıntıları: ", L"Сведения об ошибке: ", L"Detalhes do erro: ", L"错误详细信息：", L"Szczegóły błędu: ", L"Foutdetails: " },
    { 1128, L"See also", L"Vedi anche", L"Ver también", L"Voir aussi", L"Ayrıca bakınız", L"См. также", L"Consulte também", L"另请参阅", L"Zobacz też", L"Zie ook" },
    { 1131, L"Windows Update", L"Windows Update", L"Windows Update", L"Windows Update", L"Windows Update", L"Центр обновления Windows", L"Windows Update", L"Windows 更新", L"Windows Update", L"Windows Update" },
    { 1132, L"&Install updates", L"&Installa aggiornamenti", L"&Instalar actualizaciones", L"&Installer les mises à jour", L"Güncellemeleri &yükle", L"&Установить обновления", L"&Instalar atualizações", L"&安装更新", L"&Zainstaluj aktualizacje", L"Updates &installeren" },
    { 1135, L"Select the updates you want to install", L"Seleziona gli aggiornamenti da installare", L"Seleccione las actualizaciones que desea instalar", L"Sélectionnez les mises à jour à installer", L"Yüklemek istediğiniz güncellemeleri seçin", L"Выберите обновления для установки", L"Selecione as atualizações que deseja instalar", L"选择要安装的更新", L"Wybierz aktualizacje do zainstalowania", L"Selecteer de updates die u wilt installeren" },
    { 1136, L"Try &again", L"Riprova", L"&Reintentar", L"&Réessayer", L"&Yeniden dene", L"&Повторить", L"&Tentar novamente", L"&重试", L"&Spróbuj ponownie", L"&Opnieuw proberen" },
    { 1140, L"Checking for updates...", L"Ricerca aggiornamenti in corso...", L"Buscando actualizaciones...", L"Recherche des mises à jour...", L"Güncellemeler denetleniyor...", L"Проверка наличия обновлений...", L"Verificando atualizações...", L"正在检查更新...", L"Sprawdzanie aktualizacji...", L"Controleren op updates..." },
    { 1141, L"&Stop installation", L"&Interrompi installazione", L"&Detener instalación", L"&Arrêter l'installation", L"Yüklemeyi &durdur", L"&Остановить установку", L"&Parar instalação", L"&停止安装", L"&Zatrzymaj instalację", L"Installatie &stoppen" },
    { 1144, L"Most recent check for updates:", L"Ultima ricerca aggiornamenti:", L"Última búsqueda de actualizaciones:", L"Dernière recherche des mises à jour :", L"En son güncelleme denetimi:", L"Последняя проверка обновлений:", L"Verificação mais recente de atualizações:", L"最近检查更新：", L"Ostatnie sprawdzanie aktualizacji:", L"Meest recente controle op updates:" },
    { 1145, L"Updates were installed:", L"Aggiornamenti installati:", L"Actualizaciones instaladas:", L"Mises à jour installées :", L"Yüklenen güncellemeler:", L"Установленные обновления:", L"Atualizações instaladas:", L"已安装更新：", L"Zainstalowane aktualizacje:", L"Geïnstalleerde updates:" },
    { 1146, L"%lastSuccessfulInstallTime%", L"%lastSuccessfulInstallTime%", L"%lastSuccessfulInstallTime%", L"%lastSuccessfulInstallTime%", L"%lastSuccessfulInstallTime%", L"%lastSuccessfulInstallTime%", L"%lastSuccessfulInstallTime%", L"%lastSuccessfulInstallTime%", L"%lastSuccessfulInstallTime%", L"%lastSuccessfulInstallTime%" },
    { 1149, L"Turn on automatic updating", L"Attiva l'aggiornamento automatico", L"Activar la actualización automática", L"Activer la mise à jour automatique", L"Otomatik güncellemeyi aç", L"Включить автоматическое обновление", L"Ativar a atualização automática", L"启用自动更新", L"Włącz automatyczną aktualizację", L"Automatische update inschakelen" },
    { 1150, L"Turn on &automatic updates", L"Attiva gli aggiornamenti &automatici", L"Activar las actualizaciones &automáticas", L"Activer les mises à jour &automatiques", L"&Otomatik güncellemeleri aç", L"Включить &автоматические обновления", L"Ativar atualizações &automáticas", L"启用自动更新（&a）", L"Włącz &automatyczne aktualizacje", L"Automatische updates inschakelen (&a)" },
    { 1152, L"Always install the latest updates to enhance your PC's security and performance.", L"Installa sempre gli aggiornamenti più recenti per migliorare sicurezza e prestazioni del tuo PC.", L"Instale siempre las actualizaciones más recientes para mejorar la seguridad y el rendimiento de su PC.", L"Installez toujours les dernières mises à jour pour améliorer la sécurité et les performances de votre PC.", L"Bilgisayarınızın güvenliğini ve performansını artırmak için en son güncellemeleri her zaman yükleyin.", L"Всегда устанавливайте последние обновления для повышения безопасности и производительности компьютера.", L"Instale sempre as atualizações mais recentes para melhorar a segurança e o desempenho do seu PC.", L"始终安装最新更新以增强电脑的安全性和性能。", L"Zawsze instaluj najnowsze aktualizacje, aby zwiększyć bezpieczeństwo i wydajność komputera.", L"Installeer altijd de nieuwste updates om de beveiliging en prestaties van uw pc te verbeteren." },
    { 1153, L"Updates are not being installed automatically", L"Gli aggiornamenti non vengono installati automaticamente", L"Las actualizaciones no se están instalando automáticamente", L"Les mises à jour ne sont pas installées automatiquement", L"Güncellemeler otomatik olarak yüklenmiyor", L"Обновления не устанавливаются автоматически", L"As atualizações não estão sendo instaladas automaticamente", L"更新未自动安装", L"Aktualizacje nie są instalowane automatycznie", L"Updates worden niet automatisch geïnstalleerd" },
    { 1154, L"Turn on automatic updating to help improve the security and performance of your PC and allow standard users to install updates on this PC.", L"Attiva l'aggiornamento automatico per migliorare sicurezza e prestazioni del tuo PC e consentire agli utenti standard di installare aggiornamenti su questo PC.", L"Active la actualización automática para mejorar la seguridad y el rendimiento de su PC y permitir que los usuarios estándar instalen actualizaciones en este PC.", L"Activez la mise à jour automatique pour améliorer la sécurité et les performances de votre PC et permettre aux utilisateurs standard d'installer des mises à jour sur ce PC.", L"Bilgisayarınızın güvenliğini ve performansını artırmak ve standart kullanıcıların bu bilgisayara güncelleme yüklemesine izin vermek için otomatik güncellemeyi açın.", L"Включите автоматическое обновление, чтобы повысить безопасность и производительность компьютера и разрешить стандартным пользователям устанавливать обновления.", L"Ative a atualização automática para melhorar a segurança e o desempenho do seu PC e permitir que usuários padrão instalem atualizações neste PC.", L"启用自动更新以帮助提高电脑的安全性和性能，并允许标准用户在此电脑上安装更新。", L"Włącz automatyczną aktualizację, aby poprawić bezpieczeństwo i wydajność komputera i zezwolić standardowym użytkownikom na instalowanie aktualizacji.", L"Schakel automatische updates in om de beveiliging en prestaties van uw pc te verbeteren en standaardgebruikers updates op deze pc te laten installeren." },
    { 1156, L"&Restore", L"&Ripristina", L"&Restaurar", L"&Restaurer", L"&Geri yükle", L"&Восстановить", L"&Restaurar", L"&还原", L"&Przywróć", L"&Herstellen" },
    { 1157, L"Review your update history", L"Rivedi la cronologia degli aggiornamenti", L"Revise su historial de actualizaciones", L"Passez en revue votre historique des mises à jour", L"Güncelleme geçmişinizi gözden geçirin", L"Просмотрите журнал обновлений", L"Revise seu histórico de atualizações", L"查看你的更新历史记录", L"Przejrzyj historię aktualizacji", L"Bekijk uw updategeschiedenis" },
    { 1158, L"OK", L"OK", L"Aceptar", L"OK", L"Tamam", L"ОК", L"OK", L"确定", L"OK", L"OK" },
    { 1162, L"Let me choose", L"Lascia decidere a me", L"Permítame elegir", L"Laissez-moi choisir", L"Ben seçeyim", L"Я выбираю сам", L"Deixe-me escolher", L"让我选择", L"Pozwól mi wybrać", L"Laat mij kiezen" },
    { 1163, L"You decide which updates are installed automatically, when they happen, and who can install updates.", L"Sei tu a decidere quali aggiornamenti vengono installati automaticamente, quando e chi può installarli.", L"Usted decide qué actualizaciones se instalan automáticamente, cuándo y quién puede instalarlas.", L"Vous décidez quelles mises à jour sont installées automatiquement, quand et qui peut les installer.", L"Hangi güncellemelerin otomatik olarak yükleneceğine, ne zaman yükleneceğine ve kimlerin yükleyebileceğine siz karar verirsiniz.", L"Вы решаете, какие обновления устанавливать автоматически, когда и кто может их устанавливать.", L"Você decide quais atualizações são instaladas automaticamente, quando e quem pode instalá-las.", L"你决定自动安装哪些更新、何时安装以及谁可以安装更新。", L"Ty decydujesz, które aktualizacje są instalowane automatycznie, kiedy i kto może je instalować.", L"U bepaalt welke updates automatisch worden geïnstalleerd, wanneer en wie ze kan installeren." },
    { 1170, L"Error(s) found:", L"Errore/i trovato/i:", L"Error(es) encontrado(s):", L"Erreur(s) détectée(s) :", L"Bulunan hata(lar):", L"Найдены ошибки:", L"Erro(s) encontrado(s):", L"找到的错误：", L"Znaleziono błędy:", L"Fout(en) gevonden:" },
    { 1173, L"Get help with this error", L"Ottieni assistenza per questo errore", L"Obtener ayuda con este error", L"Obtenir de l'aide sur cette erreur", L"Bu hata için yardım alın", L"Получить справку об этой ошибке", L"Obter ajuda com este erro", L"获取此错误的帮助", L"Uzyskaj pomoc dotyczącą tego błędu", L"Help bij deze fout krijgen" },
    { 1174, L"Some settings are managed by your system administrator. ", L"Alcune impostazioni sono gestite dall'amministratore di sistema. ", L"Algunos parámetros los administra el administrador del sistema. ", L"Certains paramètres sont gérés par votre administrateur système. ", L"Bazı ayarlar sistem yöneticiniz tarafından yönetiliyor. ", L"Некоторые параметры управляются системным администратором. ", L"Algumas configurações são gerenciadas pelo administrador do sistema. ", L"某些设置由你的系统管理员管理。 ", L"Niektóre ustawienia są zarządzane przez administratora systemu. ", L"Sommige instellingen worden door uw systeembeheerder beheerd. " },
    { 1175, L"More information.", L"Maggiori informazioni.", L"Más información.", L"Plus d'informations.", L"Daha fazla bilgi.", L"Дополнительные сведения.", L"Mais informações.", L"更多信息。", L"Więcej informacji.", L"Meer informatie." },
    { 1176, L"After you restore updates, you can install them. We recommend restoring all important updates.", L"Dopo aver ripristinato gli aggiornamenti, puoi installarli. Si consiglia di ripristinare tutti gli aggiornamenti importanti.", L"Después de restaurar las actualizaciones, puede instalarlas. Se recomienda restaurar todas las actualizaciones importantes.", L"Après avoir restauré les mises à jour, vous pouvez les installer. Nous recommandons de restaurer toutes les mises à jour importantes.", L"Güncellemeleri geri yükledikten sonra bunları yükleyebilirsiniz. Tüm önemli güncellemelerin geri yüklenmesini öneririz.", L"После восстановления обновлений вы можете установить их. Рекомендуем восстановить все важные обновления.", L"Após restaurar as atualizações, você pode instalá-las. Recomendamos restaurar todas as atualizações importantes.", L"还原更新后，你可以安装它们。我们建议还原所有重要更新。", L"Po przywróceniu aktualizacji możesz je zainstalować. Zalecamy przywrócenie wszystkich ważnych aktualizacji.", L"Nadat u updates hebt hersteld, kunt u ze installeren. We raden aan alle belangrijke updates te herstellen." },
    { 1177, L"Updates help improve the security and performance of your computer.  It's important to install them as soon as they become available.", L"Gli aggiornamenti aiutano a migliorare sicurezza e prestazioni del computer. È importante installarli appena disponibili.", L"Las actualizaciones ayudan a mejorar la seguridad y el rendimiento del equipo. Es importante instalarlas tan pronto como estén disponibles.", L"Les mises à jour améliorent la sécurité et les performances de votre ordinateur. Il est important de les installer dès qu'elles sont disponibles.", L"Güncellemeler bilgisayarınızın güvenliğini ve performansını artırmaya yardımcı olur. Kullanılabilir olduklarında bunları yüklemek önemlidir.", L"Обновления помогают повысить безопасность и производительность компьютера. Важно устанавливать их сразу после появления.", L"As atualizações ajudam a melhorar a segurança e o desempenho do computador. É importante instalá-las assim que estiverem disponíveis.", L"更新有助于提高电脑的安全性和性能。一旦可用，立即安装非常重要。", L"Aktualizacje pomagają poprawić bezpieczeństwo i wydajność komputera. Ważne jest, aby instalować je, gdy tylko będą dostępne.", L"Updates verbeteren de beveiliging en prestaties van uw computer. Het is belangrijk ze te installeren zodra ze beschikbaar zijn." },
    { 1178, L"Install updates automatically (recommended)", L"Installa aggiornamenti automaticamente (consigliato)", L"Instalar actualizaciones automáticamente (recomendado)", L"Installer automatiquement les mises à jour (recommandé)", L"Güncellemeleri otomatik olarak yükle (önerilir)", L"Автоматически устанавливать обновления (рекомендуется)", L"Instalar atualizações automaticamente (recomendado)", L"自动安装更新（推荐）", L"Automatycznie instaluj aktualizacje (zalecane)", L"Updates automatisch installeren (aanbevolen)" },
    { 1181, L"Install new Windows Update software", L"Installa nuovo software di Windows Update", L"Instalar software nuevo de Windows Update", L"Installer un nouveau logiciel Windows Update", L"Yeni Windows Update yazılımını yükle", L"Установить новое программное обеспечение Центра обновления Windows", L"Instalar novo software do Windows Update", L"安装新的 Windows 更新软件", L"Zainstaluj nowe oprogramowanie Windows Update", L"Nieuw Windows Update-software installeren" },
    { 1182, L"&Install now", L"&Installa ora", L"&Instalar ahora", L"&Installer maintenant", L"Şimdi &yükle", L"&Установить сейчас", L"&Instalar agora", L"&立即安装", L"&Zainstaluj teraz", L"Nu &installeren" },
    { 1183, L"Sometimes, Windows Update itself needs to be updated. To continue, you'll need to do this now. Your automatic update settings won't change at all.", L"A volte è necessario aggiornare Windows Update stesso. Per continuare, dovrai farlo ora. Le impostazioni di aggiornamento automatico non cambieranno.", L"A veces es necesario actualizar el propio Windows Update. Para continuar, tendrá que hacerlo ahora. La configuración de actualización automática no cambiará.", L"Parfois, Windows Update lui-même doit être mis à jour. Pour continuer, vous devrez le faire maintenant. Vos paramètres de mise à jour automatique ne changeront pas.", L"Bazen Windows Update'in kendisinin güncellenmesi gerekir. Devam etmek için bunu şimdi yapmanız gerekir. Otomatik güncelleme ayarlarınız hiç değişmeyecek.", L"Иногда сам Центр обновления Windows нуждается в обновлении. Чтобы продолжить, сделайте это сейчас. Ваши параметры автоматического обновления не изменятся.", L"Às vezes, o próprio Windows Update precisa ser atualizado. Para continuar, você precisará fazer isso agora. Suas configurações de atualização automática não mudarão.", L"有时 Windows 更新本身需要更新。若要继续，你现在需要执行此操作。自动更新设置将完全不变。", L"Czasami sam Windows Update wymaga aktualizacji. Aby kontynuować, musisz to zrobić teraz. Ustawienia automatycznej aktualizacji w żaden sposób się nie zmienią.", L"Soms moet Windows Update zelf worden bijgewerkt. Om verder te gaan, moet u dit nu doen. Uw instellingen voor automatische updates veranderen niet." },
    { 1184, L"To finish installing this update, Windows Update will automatically close and reopen.", L"Per completare l'installazione di questo aggiornamento, Windows Update si chiuderà e riaprirà automaticamente.", L"Para terminar de instalar esta actualización, Windows Update se cerrará y volverá a abrir automáticamente.", L"Pour terminer l'installation de cette mise à jour, Windows Update se fermera et se rouvrira automatiquement.", L"Bu güncellemenin yüklenmesini bitirmek için Windows Update otomatik olarak kapanıp yeniden açılacaktır.", L"Чтобы завершить установку этого обновления, Центр обновления Windows автоматически закроется и снова откроется.", L"Para concluir a instalação desta atualização, o Windows Update fechará e reabrirá automaticamente.", L"若要完成此更新的安装，Windows 更新将自动关闭并重新打开。", L"Aby zakończyć instalowanie tej aktualizacji, Windows Update zamknie się i otworzy ponownie automatycznie.", L"Om de installatie van deze update te voltooien, wordt Windows Update automatisch gesloten en opnieuw geopend." },
    { 1185, L"Check for updates for your PC", L"Controlla gli aggiornamenti per il tuo PC", L"Buscar actualizaciones para su PC", L"Rechercher des mises à jour pour votre PC", L"Bilgisayarınız için güncellemeleri denetleyin", L"Проверить наличие обновлений для компьютера", L"Verificar atualizações para seu PC", L"检查适用于你电脑的更新", L"Sprawdź aktualizacje dla swojego komputera", L"Controleren op updates voor uw pc" },
    { 1186, L"&Check for updates", L"&Controlla aggiornamenti", L"&Buscar actualizaciones", L"&Rechercher des mises à jour", L"Güncellemeleri &denetle", L"&Проверить наличие обновлений", L"&Verificar atualizações", L"&检查更新", L"&Sprawdź aktualizacje", L"&Controleren op updates" },
    { 1188, L"&Restart now", L"&Riavvia ora", L"&Reiniciar ahora", L"&Redémarrer maintenant", L"Şimdi yeniden &başlat", L"&Перезапустить сейчас", L"&Reiniciar agora", L"&立即重启", L"&Uruchom ponownie teraz", L"Nu &opnieuw starten" },
    { 1195, L"More information: ", L"Maggiori informazioni: ", L"Más información: ", L"Plus d'informations : ", L"Daha fazla bilgi: ", L"Дополнительные сведения: ", L"Mais informações: ", L"更多信息：", L"Więcej informacji: ", L"Meer informatie: " },
    { 1196, L"Help and Support: ", L"Guida e supporto: ", L"Ayuda y soporte: ", L"Aide et support : ", L"Yardım ve Destek: ", L"Справка и поддержка: ", L"Ajuda e Suporte: ", L"帮助和支持：", L"Pomoc i obsługa techniczna: ", L"Help en ondersteuning: " },
    { 1197, L"Download size: ", L"Dimensioni download: ", L"Tamaño de descarga: ", L"Taille du téléchargement : ", L"İndirme boyutu: ", L"Размер загрузки: ", L"Tamanho do download: ", L"下载大小：", L"Rozmiar pobierania: ", L"Downloadgrootte: " },
    { 1198, L"You may need to restart your computer for this update to take effect.", L"Potrebbe essere necessario riavviare il computer affinché questo aggiornamento abbia effetto.", L"Es posible que deba reiniciar el equipo para que esta actualización surta efecto.", L"Vous devrez peut-être redémarrer votre ordinateur pour que cette mise à jour prenne effet.", L"Bu güncellemenin geçerli olması için bilgisayarınızı yeniden başlatmanız gerekebilir.", L"Возможно, потребуется перезапустить компьютер, чтобы это обновление вступило в силу.", L"Talvez seja necessário reiniciar o computador para que esta atualização entre em vigor.", L"可能需要重启电脑才能应用此更新。", L"Może być konieczne ponowne uruchomienie komputera, aby ta aktualizacja została zastosowana.", L"Mogelijk moet u uw computer opnieuw starten voordat deze update van kracht wordt." },
    { 1199, L"To continue using Windows Update, you need to install this update. After installing it, you might still need to install other important updates for your computer.", L"Per continuare a usare Windows Update, devi installare questo aggiornamento. Dopo l'installazione, potresti dover installare altri aggiornamenti importanti per il computer.", L"Para seguir usando Windows Update, debe instalar esta actualización. Después de instalarla, es posible que deba instalar otras actualizaciones importantes para su equipo.", L"Pour continuer à utiliser Windows Update, vous devez installer cette mise à jour. Après l'installation, vous devrez peut-être encore installer d'autres mises à jour importantes pour votre ordinateur.", L"Windows Update'i kullanmaya devam etmek için bu güncellemeyi yüklemeniz gerekir. Yükledikten sonra bilgisayarınız için başka önemli güncellemeler yüklemeniz gerekebilir.", L"Чтобы продолжить использовать Центр обновления Windows, установите это обновление. После установки могут потребоваться другие важные обновления для компьютера.", L"Para continuar usando o Windows Update, você precisa instalar esta atualização. Após instalá-la, talvez você ainda precise instalar outras atualizações importantes para seu computador.", L"若要继续使用 Windows 更新，你需要安装此更新。安装后，你可能仍需要为你的电脑安装其他重要更新。", L"Aby nadal korzystać z Windows Update, musisz zainstalować tę aktualizację. Po jej zainstalowaniu może być konieczne zainstalowanie innych ważnych aktualizacji dla komputera.", L"Om Windows Update te blijven gebruiken, moet u deze update installeren. Na de installatie moet u mogelijk nog andere belangrijke updates voor uw computer installeren." },
    { 1201, L"Print", L"Stampa", L"Imprimir", L"Imprimer", L"Yazdır", L"Печать", L"Imprimir", L"打印", L"Drukuj", L"Afdrukken" },
    { 1202, L"I &accept the license terms", L"I &accetto i termini di licenza", L"Acepto los términos de licencia", L"J'&accepte les termes du contrat de licence", L"Lisans koşullarını &kabul ediyorum", L"Я принимаю условия лицензии", L"&Aceito os termos da licença", L"我接受许可条款（&a）", L"&Akceptuję postanowienia licencyjne", L"Ik ga akkoord met de licentievoorwaarden (&a)" },
    { 1203, L"I &decline", L"&Non accetto", L"&Rechazo", L"Je &refuse", L"&Reddediyorum", L"&Не принимаю", L"&Recuso", L"我拒绝（&d）", L"&Odrzucam", L"Ik &weiger" },
    { 1204, L"Help", L"Guida", L"Ayuda", L"Aide", L"Yardım", L"Справка", L"Ajuda", L"帮助", L"Pomoc", L"Help" },
    { 1205, L"Install important and recommended updates as they become available. Allow standard users to install updates on this computer.", L"Installa gli aggiornamenti importanti e consigliati appena disponibili. Consenti agli utenti standard di installare aggiornamenti su questo computer.", L"Instale las actualizaciones importantes y recomendadas tan pronto como estén disponibles. Permita que los usuarios estándar instalen actualizaciones en este equipo.", L"Installez les mises à jour importantes et recommandées dès qu'elles sont disponibles. Autorisez les utilisateurs standard à installer des mises à jour sur cet ordinateur.", L"Önemli ve önerilen güncellemeleri kullanılabilir olduklarında yükleyin. Standart kullanıcıların bu bilgisayara güncelleme yüklemesine izin verin.", L"Устанавливайте важные и рекомендуемые обновления по мере их появления. Разрешите стандартным пользователям устанавливать обновления на этом компьютере.", L"Instale atualizações importantes e recomendadas assim que estiverem disponíveis. Permita que usuários padrão instalem atualizações neste computador.", L"及时安装重要和推荐更新。允许标准用户在此电脑上安装更新。", L"Instaluj ważne i zalecane aktualizacje, gdy tylko będą dostępne. Zezwól standardowym użytkownikom na instalowanie aktualizacji na tym komputerze.", L"Installeer belangrijke en aanbevolen updates zodra ze beschikbaar zijn. Sta standaardgebruikers toe updates op deze computer te installeren." },
    { 1209, L"Note: Windows Update might update itself automatically first when checking for other updates.  You can visit the Microsoft website to read the privacy statement online.", L"Nota: Windows Update potrebbe aggiornarsi automaticamente prima di controllare altri aggiornamenti.  È possibile visitare il sito Web di Microsoft per leggere l'informativa sulla privacy online.", L"Nota: Windows Update podría actualizarse automáticamente antes de buscar otras actualizaciones.  Puede visitar el sitio web de Microsoft para leer la declaración de privacidad en línea.", L"Remarque : Windows Update peut d'abord se mettre à jour automatiquement lors de la recherche d'autres mises à jour.  Vous pouvez visiter le site Web de Microsoft pour lire la déclaration de confidentialité en ligne.", L"Not: Diğer güncellemeleri denetlerken Windows Update önce kendini otomatik olarak güncelleyebilir.  Gizlilik bildirimini çevrimiçi okumak için Microsoft web sitesini ziyaret edebilirsiniz.", L"Примечание. При проверке других обновлений Центр обновления Windows может сначала обновиться автоматически.  Вы можете посетить веб-сайт Майкрософт, чтобы прочитать политику конфиденциальности в Интернете.", L"Observação: o Windows Update pode se atualizar automaticamente antes de verificar outras atualizações.  Você pode visitar o site da Microsoft para ler a declaração de privacidade online.", L"注意：检查其他更新时，Windows 更新可能会先自动更新自身。您可以访问 Microsoft 网站阅读在线隐私声明。", L"Uwaga: podczas sprawdzania innych aktualizacji Windows Update może najpierw zaktualizować się automatycznie.  Możesz odwiedzić witrynę firmy Microsoft, aby przeczytać politykę prywatności online.", L"Opmerking: Windows Update kan zichzelf eerst automatisch bijwerken bij het controleren op andere updates.  U kunt de Microsoft-website bezoeken om de privacyverklaring online te lezen." },
    { 1210, L"There aren't any hidden updates.", L"Non ci sono aggiornamenti nascosti.", L"No hay actualizaciones ocultas.", L"Il n'y a aucune mise à jour masquée.", L"Gizli güncelleme yok.", L"Нет скрытых обновлений.", L"Não há atualizações ocultas.", L"没有任何隐藏的更新。", L"Nie ma ukrytych aktualizacji.", L"Er zijn geen verborgen updates." },
    { 1211, L"You have not tried to install any updates for your computer.", L"Non hai provato a installare alcun aggiornamento per il computer.", L"No ha intentado instalar ninguna actualización para su equipo.", L"Vous n'avez pas essayé d'installer de mises à jour pour votre ordinateur.", L"Bilgisayarınız için herhangi bir güncelleme yüklemeyi denemediniz.", L"Вы не пытались установить обновления для компьютера.", L"Você não tentou instalar atualizações para seu computador.", L"你尚未尝试为你的电脑安装任何更新。", L"Nie próbowałeś zainstalować żadnych aktualizacji dla swojego komputera.", L"U hebt niet geprobeerd updates voor uw computer te installeren." },
    { 1213, L"Troubleshoot problems with installing updates", L"Risolvi i problemi di installazione degli aggiornamenti", L"Solucionar problemas con la instalación de actualizaciones", L"Résoudre les problèmes d'installation des mises à jour", L"Güncellemelerin yüklenmesiyle ilgili sorunları giderin", L"Устранение проблем с установкой обновлений", L"Solucionar problemas com a instalação de atualizações", L"解决更新安装问题", L"Rozwiązywanie problemów z instalacją aktualizacji", L"Problemen met het installeren van updates oplossen" },
    { 1218, L"Learn about installing Windows updates", L"Scopri come installare gli aggiornamenti di Windows", L"Obtener información sobre la instalación de actualizaciones de Windows", L"En savoir plus sur l'installation des mises à jour Windows", L"Windows güncellemelerini yükleme hakkında bilgi edinin", L"Подробнее об установке обновлений Windows", L"Saiba mais sobre como instalar atualizações do Windows", L"了解如何安装 Windows 更新", L"Dowiedz się, jak instalować aktualizacje systemu Windows", L"Meer informatie over het installeren van Windows-updates" },
    { 1224, L"Help", L"Guida", L"Ayuda", L"Aide", L"Yardım", L"Справка", L"Ajuda", L"帮助", L"Pomoc", L"Help" },
    { 1225, L"You receive updates: ", L"Ricevi aggiornamenti: ", L"Recibe actualizaciones: ", L"Vous recevez des mises à jour : ", L"Güncellemeleri şuradan alırsınız: ", L"Вы получаете обновления: ", L"Você recebe atualizações: ", L"你收到的更新：", L"Otrzymujesz aktualizacje: ", L"U ontvangt updates: " },
    { 1226, L"You checked online for updates from %1.", L"Hai controllato online la presenza di aggiornamenti da %1.", L"Comprobó en línea si había actualizaciones de %1.", L"Vous avez recherché en ligne les mises à jour de %1.", L"%1 güncellemeleri için çevrimiçi denetim yaptınız.", L"Вы проверили наличие обновлений от %1 в Интернете.", L"Você verificou online atualizações de %1.", L"你已在线检查来自 %1 的更新。", L"Sprawdziłeś online aktualizacje od %1.", L"U hebt online gecontroleerd op updates van %1." },
    { 1227, L"Check online for updates from %1", L"Controlla online gli aggiornamenti da %1", L"Comprobar en línea las actualizaciones de %1", L"Rechercher en ligne les mises à jour de %1", L"%1 güncellemeleri için çevrimiçi denetleyin", L"Проверить в Интернете наличие обновлений от %1", L"Verificar online as atualizações de %1", L"在线检查来自 %1 的更新", L"Sprawdź online aktualizacje od %1", L"Online controleren op updates van %1" },
    { 1232, L"&Important updates", L"Aggiornamenti &importanti", L"Actualizaciones &importantes", L"Mises à jour &importantes", L"&Önemli güncellemeler", L"&Важные обновления", L"Atualizações &importantes", L"重要更新（&I）", L"&Ważne aktualizacje", L"Belangrijke updates (&I)" },
    { 1233, L"&I", L"&I", L"&Yo", L"&J", L"&B", L"&Я", L"&E", L"&我", L"&Ja", L"&I" },
    { 1234, L"Cancel", L"Annulla", L"Cancelar", L"Annuler", L"İptal", L"Отмена", L"Cancelar", L"取消", L"Anuluj", L"Annuleren" },
    { 1235, L"Install", L"Installa", L"Instalar", L"Installer", L"Yükle", L"Установить", L"Instalar", L"安装", L"Zainstaluj", L"Installeren" },
    { 1236, L"Updates will be automatically downloaded in the background when your PC is not on a metered Internet connection.", L"Gli aggiornamenti verranno scaricati automaticamente in background quando il PC non è connesso a una connessione Internet a consumo.", L"Las actualizaciones se descargarán automáticamente en segundo plano cuando su PC no tenga una conexión a Internet con datos limitados.", L"Les mises à jour seront automatiquement téléchargées en arrière-plan lorsque votre PC n'est pas sur une connexion Internet limitée.", L"Bilgisayarınız ölçümlemeli bir İnternet bağlantısında değilken güncellemeler arka planda otomatik olarak indirilir.", L"Обновления будут автоматически загружаться в фоновом режиме, когда компьютер не подключен к лимитируемому подключению к Интернету.", L"As atualizações serão baixadas automaticamente em segundo plano quando seu PC não estiver em uma conexão à Internet medida.", L"当你的电脑未使用按流量计费的 Internet 连接时，更新将在后台自动下载。", L"Aktualizacje będą automatycznie pobierane w tle, gdy komputer nie korzysta z taryfowanego połączenia internetowego.", L"Updates worden automatisch op de achtergrond gedownload wanneer uw pc geen datalimiet-verbinding heeft." },
    { 1246, L"More information", L"Maggiori informazioni", L"Más información", L"Plus d'informations", L"Daha fazla bilgi", L"Дополнительные сведения", L"Mais informações", L"更多信息", L"Więcej informacji", L"Meer informatie" },
    { 1247, L"More information (2)", L"Maggiori informazioni (2)", L"Más información (2)", L"Plus d'informations (2)", L"Daha fazla bilgi (2)", L"Дополнительные сведения (2)", L"Mais informações (2)", L"更多信息 (2)", L"Więcej informacji (2)", L"Meer informatie (2)" },
    { 1248, L"More information (3)", L"Maggiori informazioni (3)", L"Más información (3)", L"Plus d'informations (3)", L"Daha fazla bilgi (3)", L"Дополнительные сведения (3)", L"Mais informações (3)", L"更多信息 (3)", L"Więcej informacji (3)", L"Meer informatie (3)" },
    { 1249, L"Support information", L"Informazioni sul supporto", L"Información de soporte", L"Informations de support", L"Destek bilgileri", L"Сведения о поддержке", L"Informações de suporte", L"支持信息", L"Informacje o pomocy", L"Ondersteuningsinformatie" },
    { 1250, L"Horizontal", L"Orizzontale", L"Horizontal", L"Horizontal", L"Yatay", L"Горизонтальный", L"Horizontal", L"水平", L"Poziomy", L"Horizontaal" },
    { 1251, L"Used to change horizontal viewing area", L"Usato per modificare l'area di visualizzazione orizzontale", L"Se usa para cambiar el área de visualización horizontal", L"Utilisé pour modifier la zone d'affichage horizontale", L"Yatay görüntüleme alanını değiştirmek için kullanılır", L"Используется для изменения горизонтальной области просмотра", L"Usado para alterar a área de exibição horizontal", L"用于更改水平查看区域", L"Używane do zmiany poziomego obszaru wyświetlania", L"Wordt gebruikt om het horizontale weergavegebied te wijzigen" },
    { 1252, L"Vertical", L"Verticale", L"Vertical", L"Vertical", L"Dikey", L"Вертикальный", L"Vertical", L"垂直", L"Pionowy", L"Verticaal" },
    { 1253, L"Used to change vertical viewing area", L"Usato per modificare l'area di visualizzazione verticale", L"Se usa para cambiar el área de visualización vertical", L"Utilisé pour modifier la zone d'affichage verticale", L"Dikey görüntüleme alanını değiştirmek için kullanılır", L"Используется для изменения вертикальной области просмотра", L"Usado para alterar a área de exibição vertical", L"用于更改垂直查看区域", L"Używane do zmiany pionowego obszaru wyświetlania", L"Wordt gebruikt om het verticale weergavegebied te wijzigen" },
    { 1254, L"Let me choose my settings", L"Lasciami scegliere le impostazioni", L"Permítame elegir mi configuración", L"Laissez-moi choisir mes paramètres", L"Ayarlarımı ben seçeyim", L"Я сам выберу параметры", L"Deixe-me escolher minhas configurações", L"让我选择我的设置", L"Pozwól mi wybrać ustawienia", L"Laat mij mijn instellingen kiezen" },
    { 1255, L"No updates are selected.", L"Nessun aggiornamento selezionato.", L"No hay actualizaciones seleccionadas.", L"Aucune mise à jour sélectionnée.", L"Hiçbir güncelleme seçilmedi.", L"Не выбрано ни одного обновления.", L"Nenhuma atualização selecionada.", L"未选择任何更新。", L"Nie wybrano żadnych aktualizacji.", L"Er zijn geen updates geselecteerd." },
    { 1256, L"There are no updates available for your PC.", L"Non sono disponibili aggiornamenti per il tuo PC.", L"No hay actualizaciones disponibles para su PC.", L"Aucune mise à jour n'est disponible pour votre PC.", L"Bilgisayarınız için güncelleme yok.", L"Нет доступных обновлений для вашего компьютера.", L"Não há atualizações disponíveis para seu PC.", L"没有适用于你电脑的更新。", L"Brak aktualizacji dla Twojego komputera.", L"Er zijn geen updates beschikbaar voor uw pc." },
    { 1259, L"Update is ready to install", L"L'aggiornamento è pronto per l'installazione", L"La actualización está lista para instalarse", L"La mise à jour est prête à être installée", L"Güncelleme yüklenmeye hazır", L"Обновление готово к установке", L"A atualização está pronta para instalação", L"更新已准备好安装", L"Aktualizacja jest gotowa do instalacji", L"De update is klaar om te worden geïnstalleerd" },
    { 1260, L"Update is ready to download", L"L'aggiornamento è pronto per il download", L"La actualización está lista para descargarse", L"La mise à jour est prête à être téléchargée", L"Güncelleme indirilmeye hazır", L"Обновление готово к загрузке", L"A atualização está pronta para download", L"更新已准备好下载", L"Aktualizacja jest gotowa do pobrania", L"De update is klaar om te worden gedownload" },
    { 1264, L"You may need to restart your PC after installing this update.", L"Potrebbe essere necessario riavviare il PC dopo l'installazione di questo aggiornamento.", L"Es posible que deba reiniciar su PC después de instalar esta actualización.", L"Vous devrez peut-être redémarrer votre PC après l'installation de cette mise à jour.", L"Bu güncellemeyi yükledikten sonra bilgisayarınızı yeniden başlatmanız gerekebilir.", L"Возможно, потребуется перезапустить компьютер после установки этого обновления.", L"Talvez seja necessário reiniciar o PC após instalar esta atualização.", L"安装此更新后，你可能需要重启电脑。", L"Po zainstalowaniu tej aktualizacji może być konieczne ponowne uruchomienie komputera.", L"Mogelijk moet u uw pc opnieuw starten na het installeren van deze update." },
    { 1265, L"You will need to restart your PC after installing this update.", L"Dovrai riavviare il PC dopo l'installazione di questo aggiornamento.", L"Tendrá que reiniciar su PC después de instalar esta actualización.", L"Vous devrez redémarrer votre PC après l'installation de cette mise à jour.", L"Bu güncellemeyi yükledikten sonra bilgisayarınızı yeniden başlatmanız gerekecek.", L"Вам потребуется перезапустить компьютер после установки этого обновления.", L"Você precisará reiniciar o PC após instalar esta atualização.", L"安装此更新后，你将需要重启电脑。", L"Będziesz musiał ponownie uruchomić komputer po zainstalowaniu tej aktualizacji.", L"U moet uw pc opnieuw starten na het installeren van deze update." },
    { 1266, L"Published: ", L"Pubblicato: ", L"Publicado: ", L"Publiée : ", L"Yayınlanma: ", L"Опубликовано: ", L"Publicado: ", L"已发布：", L"Opublikowano: ", L"Gepubliceerd: " },
    { 1267, L"Important", L"Importante", L"Importante", L"Importante", L"Önemli", L"Важное", L"Importante", L"重要", L"Ważne", L"Belangrijk" },
    { 1268, L"Optional", L"Facoltativo", L"Opcional", L"Facultatif", L"İsteğe bağlı", L"Необязательное", L"Opcional", L"可选", L"Opcjonalne", L"Optioneel" },
    { 1269, L"Recommended Update", L"Aggiornamento consigliato", L"Actualización recomendada", L"Mise à jour recommandée", L"Önerilen Güncelleme", L"Рекомендуемое обновление", L"Atualização recomendada", L"推荐更新", L"Zalecana aktualizacja", L"Aanbevolen update" },
    { 1270, L"No updates are available.", L"Nessun aggiornamento disponibile.", L"No hay actualizaciones disponibles.", L"Aucune mise à jour disponible.", L"Kullanılabilir güncelleme yok.", L"Нет доступных обновлений.", L"Nenhuma atualização disponível.", L"没有可用更新。", L"Brak dostępnych aktualizacji.", L"Er zijn geen updates beschikbaar." },
    { 1272, L"Windows will never check for, download, or install updates.", L"Windows non controllerà, scaricherà né installerà mai gli aggiornamenti.", L"Windows nunca comprobará, descargará ni instalará actualizaciones.", L"Windows ne recherchera, ne téléchargera ni n'installera jamais les mises à jour.", L"Windows güncellemeleri asla denetlemeyecek, indirmeyecek veya yüklemeyecek.", L"Windows никогда не будет проверять, загружать или устанавливать обновления.", L"O Windows nunca verificará, baixará nem instalará atualizações.", L"Windows 将永不检查、下载或安装更新。", L"System Windows nigdy nie będzie sprawdzać, pobierać ani instalować aktualizacji.", L"Windows zal nooit controleren op, downloaden of installeren van updates." },
    { 1273, L"%rebootPendingForInstall%", L"%rebootPendingForInstall%", L"%rebootPendingForInstall%", L"%rebootPendingForInstall%", L"%rebootPendingForInstall%", L"%rebootPendingForInstall%", L"%rebootPendingForInstall%", L"%rebootPendingForInstall%", L"%rebootPendingForInstall%", L"%rebootPendingForInstall%" },
    { 1279, L"There was a problem getting the list of updates for your PC. To continue, please reopen Windows Update", L"Si è verificato un problema durante il recupero dell'elenco degli aggiornamenti per il tuo PC. Per continuare, riapri Windows Update", L"Hubo un problema al obtener la lista de actualizaciones para su PC. Para continuar, vuelva a abrir Windows Update", L"Un problème est survenu lors de l'obtention de la liste des mises à jour pour votre PC. Pour continuer, rouvrez Windows Update", L"Bilgisayarınız için güncelleme listesi alınırken bir sorun oluştu. Devam etmek için Windows Update'i yeniden açın", L"При получении списка обновлений для компьютера возникла проблема. Чтобы продолжить, снова откройте Центр обновления Windows", L"Houve um problema ao obter a lista de atualizações para seu PC. Para continuar, reabra o Windows Update", L"获取你电脑的更新列表时出现问题。若要继续，请重新打开 Windows 更新", L"Wystąpił problem podczas pobierania listy aktualizacji dla Twojego komputera. Aby kontynuować, otwórz ponownie Windows Update", L"Er is een probleem opgetreden bij het ophalen van de updatelijst voor uw pc. Open Windows Update opnieuw om verder te gaan" },
    { 1281, L"Time until reboot", L"Tempo prima del riavvio", L"Tiempo hasta el reinicio", L"Temps avant redémarrage", L"Yeniden başlatmaya kalan süre", L"Время до перезагрузки", L"Tempo até a reinicialização", L"距重启的时间", L"Czas do ponownego uruchomienia", L"Tijd tot opnieuw opstarten" },
    { 1284, L"Res&ume", L"&Riprendi", L"&Reanudar", L"&Reprendre", L"&Sürdür", L"&Возобновить", L"&Retomar", L"&继续", L"&Wznów", L"&Hervatten" },
    { 1288, L"Updates will be automatically installed during the maintenance window.", L"Gli aggiornamenti verranno installati automaticamente durante la finestra di manutenzione.", L"Las actualizaciones se instalarán automáticamente durante la ventana de mantenimiento.", L"Les mises à jour seront automatiquement installées pendant la fenêtre de maintenance.", L"Güncellemeler bakım penceresi sırasında otomatik olarak yüklenecek.", L"Обновления будут автоматически установлены в окно обслуживания.", L"As atualizações serão instaladas automaticamente durante a janela de manutenção.", L"更新将在维护时段自动安装。", L"Aktualizacje będą automatycznie instalowane w oknie konserwacji.", L"Updates worden automatisch geïnstalleerd tijdens het onderhoudsvenster." },
    { 64501, L"Every day", L"Ogni giorno", L"Todos los días", L"Tous les jours", L"Her gün", L"Каждый день", L"Todos os dias", L"每天", L"Codziennie", L"Elke dag" },
    { 64502, L"Every Sunday", L"Ogni domenica", L"Todos los domingos", L"Tous les dimanches", L"Her Pazar", L"Каждое воскресенье", L"Todos os domingos", L"每星期日", L"W każdą niedzielę", L"Elke zondag" },
    { 64503, L"Every Monday", L"Ogni lunedì", L"Todos los lunes", L"Tous les lundis", L"Her Pazartesi", L"Каждый понедельник", L"Todas as segundas-feiras", L"每星期一", L"W każdy poniedziałek", L"Elke maandag" },
    { 64504, L"Every Tuesday", L"Ogni martedì", L"Todos los martes", L"Tous les mardis", L"Her Salı", L"Каждый вторник", L"Todas as terças-feiras", L"每星期二", L"W każdy wtorek", L"Elke dinsdag" },
    { 64505, L"Every Wednesday", L"Ogni mercoledì", L"Todos los miércoles", L"Tous les mercredis", L"Her Çarşamba", L"Каждую среду", L"Todas as quartas-feiras", L"每星期三", L"W każdą środę", L"Elke woensdag" },
    { 64506, L"Every Thursday", L"Ogni giovedì", L"Todos los jueves", L"Tous les jeudis", L"Her Perşembe", L"Каждый четверг", L"Todas as quintas-feiras", L"每星期四", L"W każdy czwartek", L"Elke donderdag" },
    { 64507, L"Every Friday", L"Ogni venerdì", L"Todos los viernes", L"Tous les vendredis", L"Her Cuma", L"Каждую пятницу", L"Todas as sextas-feiras", L"每星期五", L"W każdy piątek", L"Elke vrijdag" },
    { 64508, L"Every Saturday", L"Ogni sabato", L"Todos los sábados", L"Tous les samedis", L"Her Cumartesi", L"Каждую субботу", L"Todos os sábados", L"每星期六", L"W każdą sobotę", L"Elke zaterdag" },
    { 64531, L"Enter the credentials for proxy authentication", L"Immetti le credenziali per l'autenticazione proxy", L"Especifique las credenciales para la autenticación de proxy", L"Saisissez les informations d'identification pour l'authentification du proxy", L"Proxy kimlik doğrulaması için kimlik bilgilerini girin", L"Введите учетные данные для проверки подлинности прокси-сервера", L"Insira as credenciais para a autenticação de proxy", L"输入代理身份验证的凭据", L"Wprowadź poświadczenia do uwierzytelniania serwera proxy", L"Voer de referenties in voor proxyverificatie" },
    { 64532, L"Password Required", L"Password richiesta", L"Se requiere contraseña", L"Mot de passe requis", L"Parola gerekli", L"Требуется пароль", L"Senha necessária", L"需要密码", L"Wymagane hasło", L"Wachtwoord vereist" },
    { 20000, L"Modern Update Status", L"Stato aggiornamenti moderno", L"Estado de actualizaciones modernas", L"État des mises à jour moderne", L"Modern Güncelleme Durumu", L"Состояние современных обновлений", L"Status de atualizações modernas", L"现代更新状态", L"Nowoczesny stan aktualizacji", L"Moderne updatestatus" },
    { 20001, L"Available Updates", L"Aggiornamenti disponibili", L"Actualizaciones disponibles", L"Mises à jour disponibles", L"Kullanılabilir Güncellemeler", L"Доступные обновления", L"Atualizações disponíveis", L"可用更新", L"Dostępne aktualizacje", L"Beschikbare updates" },
    { 20002, L"Update History", L"Cronologia aggiornamenti", L"Historial de actualizaciones", L"Historique des mises à jour", L"Güncelleme Geçmişi", L"Журнал обновлений", L"Histórico de atualizações", L"更新历史记录", L"Historia aktualizacji", L"Updategeschiedenis" },
    { 20003, L"Check for Updates", L"Controlla aggiornamenti", L"Buscar actualizaciones", L"Rechercher des mises à jour", L"Güncellemeleri Denetle", L"Проверить наличие обновлений", L"Verificar atualizações", L"检查更新", L"Sprawdź aktualizacje", L"Controleren op updates" },
    { 20004, L"View Installed Updates", L"Visualizza aggiornamenti installati", L"Ver actualizaciones instaladas", L"Afficher les mises à jour installées", L"Yüklü Güncellemeleri Görüntüle", L"Просмотр установленных обновлений", L"Exibir atualizações instaladas", L"查看已安装的更新", L"Wyświetl zainstalowane aktualizacje", L"Geïnstalleerde updates bekijken" },
    { 20005, L"Last Check: %s", L"Ultimo controllo: %s", L"Última comprobación: %s", L"Dernière vérification : %s", L"Son denetim: %s", L"Последняя проверка: %s", L"Última verificação: %s", L"上次检查：%s", L"Ostatnie sprawdzenie: %s", L"Laatste controle: %s" },
    { 20006, L"Important: %d", L"Importanti: %d", L"Importantes: %d", L"Importantes : %d", L"Önemli: %d", L"Важные: %d", L"Importantes: %d", L"重要：%d", L"Ważne: %d", L"Belangrijk: %d" },
    { 20007, L"Optional: %d", L"Facoltativi: %d", L"Opcionales: %d", L"Facultatifs : %d", L"İsteğe bağlı: %d", L"Необязательные: %d", L"Opcionais: %d", L"可选：%d", L"Opcjonalne: %d", L"Optioneel: %d" },
    { 20008, L"Your PC is up to date!", L"Il tuo PC è aggiornato!", L"Su PC está actualizado.", L"Votre PC est à jour !", L"Bilgisayarınız güncel!", L"Ваш компьютер обновлен!", L"Seu PC está atualizado!", L"你的电脑已是最新！", L"Twój komputer jest aktualny!", L"Uw pc is up-to-date!" },
    { 20009, L"Checking for updates...", L"Ricerca aggiornamenti in corso...", L"Buscando actualizaciones...", L"Recherche des mises à jour...", L"Güncellemeler denetleniyor...", L"Проверка наличия обновлений...", L"Verificando atualizações...", L"正在检查更新...", L"Sprawdzanie aktualizacji...", L"Controleren op updates..." },
    { 20010, L"No updates found.", L"Nessun aggiornamento trovato.", L"No se encontraron actualizaciones.", L"Aucune mise à jour trouvée.", L"Güncelleme bulunamadı.", L"Обновления не найдены.", L"Nenhuma atualização encontrada.", L"未找到更新。", L"Nie znaleziono aktualizacji.", L"Geen updates gevonden." },
    { 20020, L"Get updates for other Microsoft products.", L"Ottieni aggiornamenti per altri prodotti Microsoft.", L"Obtén actualizaciones para otros productos de Microsoft.", L"Obtenez des mises à jour pour d'autres produits Microsoft.", L"Diğer Microsoft ürünleri için güncellemeleri alın.", L"Получайте обновления для других продуктов Microsoft.", L"Obtenha atualizações para outros produtos da Microsoft.", L"获取其他 Microsoft 产品的更新。", L"Pobierz aktualizacje dla innych produktów Microsoft.", L"Ontvang updates voor andere Microsoft-producten." },
    { 20021, L"Find out more", L"Scopri di più", L"Obtén más información", L"En savoir plus", L"Daha fazla bilgi edinin", L"Узнать больше", L"Saiba mais", L"了解更多信息", L"Dowiedz się więcej", L"Meer informatie" },
    { 20022, L"There are updates available", L"Sono disponibili aggiornamenti", L"Hay actualizaciones disponibles", L"Des mises à jour sont disponibles", L"Güncellemeler mevcut", L"Доступны обновления", L"Há atualizações disponíveis", L"有可用更新", L"Są dostępne aktualizacje", L"Er zijn updates beschikbaar" },
    { 20023, L"Go to Windows Settings to install them", L"Vai alle impostazioni di Windows per installarli", L"Ve a la configuración de Windows para instalarlas", L"Accédez aux paramètres Windows pour les installer", L"Bunları yüklemek için Windows Ayarları'na gidin", L"Перейдите в параметры Windows, чтобы установить их", L"Vá para as configurações do Windows para instalá-las", L"转到 Windows 设置以安装它们", L"Przejdź do ustawień systemu Windows, aby je zainstalować", L"Ga naar Windows-instellingen om ze te installeren" },
    { 64540, L"To view the update history, choose one of the following settings:", L"Per visualizzare la cronologia degli aggiornamenti, scegliere una delle seguenti impostazioni:", L"Para ver el historial de actualizaciones, elija una de las siguientes opciones:", L"Pour afficher l'historique des mises à jour, choisissez l'une des options suivantes :", L"Güncelleme geçmişini görüntülemek için aşağıdaki seçeneklerden birini seçin:", L"Чтобы просмотреть журнал обновлений, выберите один из следующих параметров:", L"Para ver o histórico de atualizações, escolha uma das seguintes opções:", L"要查看更新历史记录，请选择以下选项之一：", L"Aby wyświetlić historię aktualizacji, wybierz jedną z następujących opcji:", L"Om de updategeschiedenis weer te geven, kiest u een van de volgende opties:" },
    { 64541, L"View update history", L"Visualizza cronologia aggiornamenti", L"Ver historial de actualizaciones", L"Afficher l'historique des mises à jour", L"Güncelleme geçmişini görüntüle", L"Просмотреть журнал обновлений", L"Ver histórico de atualizações", L"查看更新历史记录", L"Wyświetl historię aktualizacji", L"Updategeschiedenis weergeven" },
    { 64542, L"Manage updates from the system settings", L"Gestisci gli aggiornamenti dalle impostazioni di sistema", L"Administrar las actualizaciones desde la configuración del sistema", L"Gérer les mises à jour à partir des paramètres du système", L"Güncellemeleri sistem ayarlarından yönetin", L"Управление обновлениями из параметров системы", L"Gerenciar atualizações nas configurações do sistema", L"在系统设置中管理更新", L"Zarządzaj aktualizacjami w ustawieniach systemu", L"Updates beheren via de systeeminstellingen" },
    { 0, nullptr }
};


static wchar_t HexUpper(BYTE nibble) {
    return nibble < 10 ? static_cast<wchar_t>(L'0' + nibble)
                       : static_cast<wchar_t>(L'A' + nibble - 10);
}


static bool ComputeSha256(const std::wstring& path, BYTE digest[32]) {
    HANDLE file = CreateFileW(path.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr,
                              OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (file == INVALID_HANDLE_VALUE) return false;
    HCRYPTPROV provider = 0;
    HCRYPTHASH hash = 0;
    bool ok = false;
    if (CryptAcquireContextW(&provider, nullptr, nullptr, PROV_RSA_AES,
                             CRYPT_VERIFYCONTEXT) &&
        CryptCreateHash(provider, CALG_SHA_256, 0, 0, &hash)) {
        BYTE buffer[65536];
        DWORD read = 0;
        bool readOk = true;
        while (ReadFile(file, buffer, sizeof(buffer), &read, nullptr) && read) {
            if (!CryptHashData(hash, buffer, read, 0)) { readOk = false; break; }
        }
        DWORD size = 32;
        ok = readOk && CryptGetHashParam(hash, HP_HASHVAL, digest, &size, 0) && size == 32;
    }
    if (hash) CryptDestroyHash(hash);
    if (provider) CryptReleaseContext(provider, 0);
    CloseHandle(file);
    return ok;
}


static bool IsValidPayload(const std::wstring& path) {
    HANDLE file = CreateFileW(path.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr,
                              OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (file == INVALID_HANDLE_VALUE) return false;
    LARGE_INTEGER fileSize{};
    IMAGE_DOS_HEADER dos{};
    DWORD read = 0;
    bool ok = GetFileSizeEx(file, &fileSize) && fileSize.QuadPart >= kMinDllSize &&
              ReadFile(file, &dos, sizeof(dos), &read, nullptr) &&
              read == sizeof(dos) && dos.e_magic == IMAGE_DOS_SIGNATURE;
    DWORD signature = 0;
    WORD machine = 0;
    if (ok) {
        LARGE_INTEGER offset{};
        offset.QuadPart = dos.e_lfanew;
        ok = SetFilePointerEx(file, offset, nullptr, FILE_BEGIN) &&
             ReadFile(file, &signature, sizeof(signature), &read, nullptr) &&
             read == sizeof(signature) && signature == IMAGE_NT_SIGNATURE &&
             ReadFile(file, &machine, sizeof(machine), &read, nullptr) &&
             read == sizeof(machine) && machine == IMAGE_FILE_MACHINE_AMD64;
    }
    CloseHandle(file);
    if (!ok) return false;
    BYTE digest[32];
    if (!ComputeSha256(path, digest)) return false;
    for (int i = 0; i < 32; ++i) {
        if (kExpectedSha256[i * 2] != HexUpper(digest[i] >> 4) ||
            kExpectedSha256[i * 2 + 1] != HexUpper(digest[i] & 15)) return false;
    }
    return true;
}


static std::wstring StoreDir() {
    wchar_t path[32768] = {};
    size_t length = Wh_GetModStoragePath(path, ARRAYSIZE(path));
    if (length == 0 || length >= ARRAYSIZE(path)) {
        Wh_Log(L"Windows Update Restorer: Wh_GetModStoragePath failed");
        return {};
    }
    return path;
}


static bool DownloadWithTimeout(const std::wstring& destination) {
    HINTERNET internet = InternetOpenW(L"Windhawk Windows Update Restorer",
                                       INTERNET_OPEN_TYPE_PRECONFIG, nullptr, nullptr, 0);
    if (!internet) return false;
    DWORD timeout = kDownloadTimeoutMs;
    InternetSetOptionW(internet, INTERNET_OPTION_CONNECT_TIMEOUT, &timeout, sizeof(timeout));
    InternetSetOptionW(internet, INTERNET_OPTION_SEND_TIMEOUT, &timeout, sizeof(timeout));
    InternetSetOptionW(internet, INTERNET_OPTION_RECEIVE_TIMEOUT, &timeout, sizeof(timeout));
    HINTERNET url = InternetOpenUrlW(internet, kDownloadUrl, nullptr, 0,
                                     INTERNET_FLAG_RELOAD | INTERNET_FLAG_NO_CACHE_WRITE |
                                         INTERNET_FLAG_NO_UI,
                                     0);
    if (!url) { InternetCloseHandle(internet); return false; }
    bool ok = false;
    DWORD status = 0, statusLength = sizeof(status), headerIndex = 0;
    if (HttpQueryInfoW(url, HTTP_QUERY_STATUS_CODE | HTTP_QUERY_FLAG_NUMBER, &status,
                       &statusLength, &headerIndex) && status == HTTP_STATUS_OK) {
        HANDLE file = CreateFileW(destination.c_str(), GENERIC_WRITE, FILE_SHARE_READ, nullptr,
                                  CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
        if (file != INVALID_HANDLE_VALUE) {
            BYTE buffer[65536];
            DWORD available = 0, read = 0, written = 0;
            const ULONGLONG started = GetTickCount64();
            ok = true;
            for (;;) {
                if (g_stopping.load() || GetTickCount64() - started > kDownloadTimeoutMs ||
                    !InternetQueryDataAvailable(url, &available, 0, 0)) { ok = false; break; }
                if (!available) break;
                if (available > sizeof(buffer)) available = sizeof(buffer);
                if (!InternetReadFile(url, buffer, available, &read) || !read ||
                    !WriteFile(file, buffer, read, &written, nullptr) || written != read) {
                    ok = false; break;
                }
            }
            CloseHandle(file);
        }
    }
    InternetCloseHandle(url);
    InternetCloseHandle(internet);
    return ok;
}


static bool EnsurePayload(std::wstring& outPath) {
    std::wstring dir = StoreDir();
    if (dir.empty()) return false;
    const std::wstring finalPath = dir + L"\\" + kDllName;
    if (GetFileAttributesW(finalPath.c_str()) != INVALID_FILE_ATTRIBUTES &&
        IsValidPayload(finalPath)) {
        outPath = finalPath;
        return true;
    }
    const std::wstring temporaryPath = finalPath + L".tmp";
    for (int attempt = 1; attempt <= kMaxDownloadAttempts && !g_stopping.load(); ++attempt) {
        DeleteFileW(temporaryPath.c_str());
        Wh_Log(L"Downloading verified Windows 8.1 wucltux.dll, attempt %d/%d", attempt,
               kMaxDownloadAttempts);
        if (DownloadWithTimeout(temporaryPath) && IsValidPayload(temporaryPath) &&
            MoveFileExW(temporaryPath.c_str(), finalPath.c_str(),
                        MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH)) {
            outPath = finalPath;
            return true;
        }
        DeleteFileW(temporaryPath.c_str());
        if (attempt < kMaxDownloadAttempts && g_stopEvent &&
            WaitForSingleObject(g_stopEvent, kRetryDelayMs) == WAIT_OBJECT_0) break;
    }
    return false;
}


// -----------------------------------------------------------------------------
// Private resource-module builder (adapted from the Performance Information and
// Tools Restorer). UpdateResource cannot normally modify a MUI-configured PE,
// hence DisableMuiConfigInPrivateCopy is applied only to the private copy.
// -----------------------------------------------------------------------------
class UniqueWinHandle {
public:
    UniqueWinHandle() = default;
    explicit UniqueWinHandle(HANDLE handle) : handle_(handle) {}
    ~UniqueWinHandle() { Reset(); }

    UniqueWinHandle(const UniqueWinHandle&) = delete;
    UniqueWinHandle& operator=(const UniqueWinHandle&) = delete;

    UniqueWinHandle(UniqueWinHandle&& other) noexcept
        : handle_(other.Release()) {}
    UniqueWinHandle& operator=(UniqueWinHandle&& other) noexcept {
        if (this != &other) Reset(other.Release());
        return *this;
    }


    bool IsValid() const { return handle_ && handle_ != INVALID_HANDLE_VALUE; }

    HANDLE Get() const { return handle_; }
    HANDLE Release() {

        HANDLE result = handle_;
        handle_ = INVALID_HANDLE_VALUE;
        return result;
    }

    void Reset(HANDLE handle = INVALID_HANDLE_VALUE) {

        if (IsValid()) CloseHandle(handle_);
        handle_ = handle;
    }


private:
    HANDLE handle_ = INVALID_HANDLE_VALUE;
};

class ScopedTemporaryFile {

public:
    explicit ScopedTemporaryFile(std::wstring path) : path_(std::move(path)) {}
    ~ScopedTemporaryFile() {

        if (!committed_ && !path_.empty()) DeleteFileW(path_.c_str());
    }

    ScopedTemporaryFile(const ScopedTemporaryFile&) = delete;
    ScopedTemporaryFile& operator=(const ScopedTemporaryFile&) = delete;
    void Commit() { committed_ = true; }


private:
    std::wstring path_;
    bool committed_ = false;
};

class ResourceUpdateTransaction {

public:
    explicit ResourceUpdateTransaction(const std::wstring& path)
        : update_(BeginUpdateResourceW(path.c_str(), FALSE)) {}
    ~ResourceUpdateTransaction() {

        if (update_) EndUpdateResourceW(update_, TRUE);

    }

    ResourceUpdateTransaction(const ResourceUpdateTransaction&) = delete;
    ResourceUpdateTransaction& operator=(const ResourceUpdateTransaction&) = delete;


    bool IsValid() const { return update_ != nullptr; }

    HANDLE Get() const { return update_; }

    bool Commit() {

        if (!update_) return false;
        HANDLE update = update_;
        update_ = nullptr;
        return EndUpdateResourceW(update, FALSE) != FALSE;
    }


private:
    HANDLE update_ = nullptr;
};

template <typename T>
static bool ReadPeValue(const std::vector<BYTE>& file, size_t offset, T& value) {

    if (offset > file.size() || file.size() - offset < sizeof(T)) return false;
    memcpy(&value, file.data() + offset, sizeof(T));
    return true;
}


// UpdateResource intentionally restricts LN/MUI binaries. Rename the private
// copy's named "MUI" RC-config resource to the unused name "CUI" first. This
// changes only the copy and makes it a normal resource PE. Note: this depends on
// undocumented layout details of the specific Microsoft binary at the symbol
// server URL; it may need updating if that binary ever changes. It is confined
// to a private copy, so the blast radius is small.
static bool DisableMuiConfigInPrivateCopy(const std::wstring& path) {
    UniqueWinHandle file(CreateFileW(path.c_str(), GENERIC_READ | GENERIC_WRITE,
                                     FILE_SHARE_READ, nullptr, OPEN_EXISTING,
                                     FILE_ATTRIBUTE_NORMAL, nullptr));
    if (!file.IsValid()) return false;


    LARGE_INTEGER size = {};
    if (!GetFileSizeEx(file.Get(), &size) || size.QuadPart <= 0 ||
        size.QuadPart > 64 * 1024 * 1024) {

        return false;
    }


    std::vector<BYTE> bytes(static_cast<size_t>(size.QuadPart));
    DWORD bytesRead = 0;
    if (!ReadFile(file.Get(), bytes.data(), static_cast<DWORD>(bytes.size()),
                  &bytesRead, nullptr) ||
        bytesRead != bytes.size()) {

        return false;
    }


    IMAGE_DOS_HEADER dos = {};
    if (!ReadPeValue(bytes, 0, dos) || dos.e_magic != IMAGE_DOS_SIGNATURE ||
        dos.e_lfanew < 0) {

        return false;
    }


    const size_t ntOffset = static_cast<size_t>(dos.e_lfanew);
    DWORD signature = 0;
    IMAGE_FILE_HEADER fileHeader = {};
    if (!ReadPeValue(bytes, ntOffset, signature) ||
        signature != IMAGE_NT_SIGNATURE ||
        !ReadPeValue(bytes, ntOffset + sizeof(DWORD), fileHeader)) {

        return false;
    }


    const size_t optionalOffset = ntOffset + sizeof(DWORD) + sizeof(IMAGE_FILE_HEADER);
    WORD optionalMagic = 0;
    if (!ReadPeValue(bytes, optionalOffset, optionalMagic)) return false;


    DWORD resourceRva = 0;
    if (optionalMagic == IMAGE_NT_OPTIONAL_HDR64_MAGIC) {
        IMAGE_OPTIONAL_HEADER64 optional = {};
        if (!ReadPeValue(bytes, optionalOffset, optional) ||
            optional.NumberOfRvaAndSizes <= IMAGE_DIRECTORY_ENTRY_RESOURCE) {

            return false;
        }
        resourceRva =
            optional.DataDirectory[IMAGE_DIRECTORY_ENTRY_RESOURCE].VirtualAddress;
    } else if (optionalMagic == IMAGE_NT_OPTIONAL_HDR32_MAGIC) {
        IMAGE_OPTIONAL_HEADER32 optional = {};
        if (!ReadPeValue(bytes, optionalOffset, optional) ||
            optional.NumberOfRvaAndSizes <= IMAGE_DIRECTORY_ENTRY_RESOURCE) {

            return false;
        }
        resourceRva =
            optional.DataDirectory[IMAGE_DIRECTORY_ENTRY_RESOURCE].VirtualAddress;
    } else {

        return false;
    }
    if (!resourceRva) return false;


    const size_t sectionOffset = optionalOffset + fileHeader.SizeOfOptionalHeader;
    DWORD resourceRaw = 0;
    for (WORD i = 0; i < fileHeader.NumberOfSections; ++i) {
        IMAGE_SECTION_HEADER section = {};
        if (!ReadPeValue(bytes, sectionOffset + i * sizeof(IMAGE_SECTION_HEADER),
                         section)) {

            return false;
        }
        const DWORD virtualSize = section.Misc.VirtualSize;
        const DWORD span = virtualSize > section.SizeOfRawData ? virtualSize
                                                               : section.SizeOfRawData;
        if (resourceRva >= section.VirtualAddress &&
            resourceRva - section.VirtualAddress < span) {

            resourceRaw = section.PointerToRawData +
                          (resourceRva - section.VirtualAddress);

            break;
        }
    }
    if (!resourceRaw || resourceRaw >= bytes.size()) return false;


    IMAGE_RESOURCE_DIRECTORY root = {};
    if (!ReadPeValue(bytes, resourceRaw, root)) return false;
    const DWORD entryCount = static_cast<DWORD>(root.NumberOfNamedEntries) +
                             root.NumberOfIdEntries;
    const size_t entriesOffset = resourceRaw + sizeof(root);


    size_t muiFirstCharacterOffset = 0;
    for (DWORD i = 0; i < entryCount; ++i) {
        IMAGE_RESOURCE_DIRECTORY_ENTRY entry = {};
        if (!ReadPeValue(bytes, entriesOffset + i * sizeof(entry), entry)) {

            return false;
        }
        DWORD nameField = 0;
        memcpy(&nameField, &entry, sizeof(nameField));
        if (!(nameField & 0x80000000u)) continue;
        const size_t stringOffset = resourceRaw + (nameField & 0x7FFFFFFFu);

        WORD length = 0;
        if (!ReadPeValue(bytes, stringOffset, length) || length != 3) continue;
        WCHAR name[3] = {};
        if (stringOffset + sizeof(WORD) > bytes.size() ||
            bytes.size() - (stringOffset + sizeof(WORD)) < sizeof(name)) {

            return false;
        }

        memcpy(name, bytes.data() + stringOffset + sizeof(WORD), sizeof(name));
        if (name[0] == L'M' && name[1] == L'U' && name[2] == L'I') {
            muiFirstCharacterOffset = stringOffset + sizeof(WORD);
            break;
        }
    }
    if (!muiFirstCharacterOffset) return false;


    LARGE_INTEGER position = {};
    position.QuadPart = static_cast<LONGLONG>(muiFirstCharacterOffset);

    if (!SetFilePointerEx(file.Get(), position, nullptr, FILE_BEGIN)) return false;
    const WCHAR replacement = L'C';
    DWORD written = 0;
    return WriteFile(file.Get(), &replacement, sizeof(replacement), &written,
                     nullptr) &&
           written == sizeof(replacement);

}



// -----------------------------------------------------------------------------
// Embedded MUI strings. The matching MUI file is not downloaded: the classic
// page receives its strings from this table, keeping the mod self-contained.
// -----------------------------------------------------------------------------
using LoadStringW_t = int(WINAPI*)(HINSTANCE, UINT, LPWSTR, int);
static LoadStringW_t LoadStringWOriginal = nullptr;

// Returns the string for the currently selected language (g_language), falling
// back to English for any unknown code.
static const wchar_t* EmbeddedMuiString(UINT id) {
    for (const auto* item = kWucltuxMuiStrings; item->en; ++item) {
        if (item->id != id) continue;
        if (g_language == L"it") return item->it;
        if (g_language == L"es") return item->es;
        if (g_language == L"fr") return item->fr;
        if (g_language == L"tr") return item->tr;
        if (g_language == L"ru") return item->ru;
        if (g_language == L"pt") return item->pt;
        if (g_language == L"zh") return item->zh;
        if (g_language == L"pl") return item->pl;
        if (g_language == L"nl") return item->nl;
        return item->en; // default / fallback
    }
    return nullptr;
}

// Returns the translated Control Panel InfoTip (the grey tooltip shown on hover
// over the "Windows Update" item in the Control Panel) for the currently
// selected language. English is the fallback for any unknown code.
static const wchar_t* InfoTipForLanguage() {
    if (g_language == L"it")
        return L"Controlla gli aggiornamenti e visualizza la cronologia degli aggiornamenti.";
    if (g_language == L"es")
        return L"Busca actualizaciones y consulta el historial de actualizaciones.";
    if (g_language == L"fr")
        return L"Recherchez les mises à jour et consultez l'historique des mises à jour.";
    if (g_language == L"tr")
        return L"Güncellemeleri denetleyin ve güncelleme geçmişini görüntüleyin.";
    if (g_language == L"ru")
        return L"Проверьте наличие обновлений и просмотрите журнал обновлений.";
    if (g_language == L"pt")
        return L"Verifique atualizações e consulte o histórico de atualizações.";
    if (g_language == L"zh")
        return L"检查更新并查看更新历史记录。";
    if (g_language == L"pl")
        return L"Sprawdź aktualizacje i wyświetl historię aktualizacji.";
    if (g_language == L"nl")
        return L"Controleer op updates en bekijk de updategeschiedenis.";
    return L"Check for updates and view update history."; // en / fallback
}

static bool IsWucltuxInstance(HINSTANCE instance) {
    if (!instance) return false;
    const ULONG_PTR raw = reinterpret_cast<ULONG_PTR>(instance);
    return reinterpret_cast<HMODULE>(raw & ~static_cast<ULONG_PTR>(3)) == g_module.load();
}
static int CopyEmbeddedString(const wchar_t* text, LPWSTR buffer, int bufferChars) {
    if (!text) return 0;
    const int length = static_cast<int>(wcslen(text));
    if (bufferChars == 0) {
        if (!buffer) return 0;
        *reinterpret_cast<LPCWSTR*>(buffer) = text;
        return length;
    }
    if (!buffer || bufferChars < 1) return 0;
    const int copied = length < bufferChars - 1 ? length : bufferChars - 1;
    if (copied) memcpy(buffer, text, static_cast<size_t>(copied) * sizeof(wchar_t));
    buffer[copied] = 0;
    return copied;
}
static int WINAPI LoadStringWHook(HINSTANCE instance, UINT id, LPWSTR buffer, int bufferChars) {
    if (IsWucltuxInstance(instance)) {
        if (const wchar_t* text = EmbeddedMuiString(id))
            return CopyEmbeddedString(text, buffer, bufferChars);
    }
    return LoadStringWOriginal(instance, id, buffer, bufferChars);
}


// Build an RT_STRING payload block (16 consecutive string IDs).
static bool BuildWucltuxStringBlock(UINT blockId, std::vector<BYTE>& output) {
    output.clear();
    bool hasText = false;
    for (UINT index = 0; index < 16; ++index) {
        const UINT id = (blockId - 1) * 16 + index;
        const wchar_t* text = EmbeddedMuiString(id);
        const WORD length = text ? static_cast<WORD>(wcslen(text)) : 0;
        const BYTE* lengthBytes = reinterpret_cast<const BYTE*>(&length);
        output.insert(output.end(), lengthBytes, lengthBytes + sizeof(length));
        if (length) {
            const BYTE* textBytes = reinterpret_cast<const BYTE*>(text);
            output.insert(output.end(), textBytes, textBytes + length * sizeof(wchar_t));
            hasText = true;
        }
    }
    return hasText;
}


// Verifies that a candidate .mres file at `path` actually contains our
// embedded strings (loads it as a data/image resource module and checks a
// known string id resolves to the expected text). Returns the loaded module
// on success (caller takes ownership) or nullptr on any failure, in which
// case any partially-loaded module is freed.
static HMODULE ValidateEmbeddedMuiResourceModule(const std::wstring& path) {
    HMODULE module = LoadLibraryExW(path.c_str(), nullptr,
                                    LOAD_LIBRARY_AS_DATAFILE | LOAD_LIBRARY_AS_IMAGE_RESOURCE);
    if (!module) return nullptr;
    // Pick a stable, always-present id from the table (id 1, "Windows Update")
    // and confirm it resolves via a real resource lookup, not just that the
    // file loaded.
    const wchar_t* expected = EmbeddedMuiString(1);
    wchar_t buffer[64] = {};
    const int copied = LoadStringW(module, 1, buffer, ARRAYSIZE(buffer));
    if (copied <= 0 || !expected || wcscmp(buffer, expected) != 0) {
        FreeLibrary(module);
        return nullptr;
    }
    return module;
}

// Scans the storage directory for an already-built, still-valid embedded-mui
// module from an earlier generation in this same process and reuses it
// instead of building a new one. This matters because we deliberately never
// FreeLibrary the modules we load (a Control Panel page can keep a live
// reference into one), so every rebuild leaves the previous file locked in
// memory forever - repeatedly creating fresh files on every mod
// enable/disable cycle is exactly the kind of rapid file churn that trips
// ransomware-protection heuristics in AV/EDR products. Reusing a known-good
// existing file avoids that churn entirely.
static bool ReuseExistingEmbeddedMuiResourceModule(const std::wstring& sourceDir) {
    // Only reuse files built for the current language. The filename embeds the
    // language code so a module built for one language is never mistaken for
    // another (their ID-1 string is often identical, so validating only against
    // ID-1 is not enough to tell them apart).
    const std::wstring prefix =
        L"wucltux.embedded-mui-" + std::to_wstring(GetCurrentProcessId()) + L"-" +
        g_language + L"-";
    const std::wstring pattern = sourceDir + L"\\" + prefix + L"*.mres";

    WIN32_FIND_DATAW findData{};
    HANDLE find = FindFirstFileW(pattern.c_str(), &findData);
    if (find == INVALID_HANDLE_VALUE) return false;
    bool reused = false;
    do {
        if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) continue;
        const std::wstring candidate = sourceDir + L"\\" + findData.cFileName;
        if (HMODULE module = ValidateEmbeddedMuiResourceModule(candidate)) {
            std::lock_guard<std::mutex> lock(g_resourceMutex);
            g_resourcePath = candidate;
            g_resourceModule.store(module);
            reused = true;
            Wh_Log(L"Windows Update Restorer: reusing existing embedded MUI resource module: %s",
                   candidate.c_str());
            break;
        }
    } while (!reused && FindNextFileW(find, &findData));
    FindClose(find);
    return reused;
}

static bool BuildEmbeddedMuiResourceModule(const std::wstring& sourcePath) {
    const size_t sourceSlash = sourcePath.find_last_of(L"\\/");
    if (sourceSlash == std::wstring::npos) return false;
    if (ReuseExistingEmbeddedMuiResourceModule(sourcePath.substr(0, sourceSlash))) return true;
    // Do the actual file work without holding g_resourceMutex: it can take
    // several seconds now (see the retry loop below), and holding the lock
    // that long would stall the UI thread if it calls
    // EmbeddedMuiResourceModule() (via XResourceProviderCreateHook) while a
    // rebuild is in progress. We only touch g_resourcePath - the one piece
    // of shared state - briefly, under the lock, at the very end. We also
    // deliberately do NOT clear g_resourcePath up front: if a previous
    // build already succeeded, its (still-loaded) file stays usable for
    // string lookups while this rebuild attempt is in flight.
    const size_t slash = sourcePath.find_last_of(L"\\/");
    if (slash == std::wstring::npos) return false;
    // The destination filename must be unique per *build attempt*, not just
    // per process: if the mod is reloaded without restarting explorer.exe,
    // the previous build's file can still be mapped in memory (loaded via
    // LOAD_LIBRARY_AS_DATAFILE) and reusing its name makes MoveFileExW fail
    // with ERROR_ACCESS_DENIED (5).
    static std::atomic<uint32_t> generation{0};
    // Deliberately NOT a .dll extension: several AV/EDR products (Defender
    // Controlled Folder Access, Attack Surface Reduction "block unknown
    // executables" rules, etc.) can permanently deny creation/rename of
    // newly-written .dll/.exe files by a process like explorer.exe - not a
    // transient lock, so no amount of retrying helps. LoadLibraryExW with
    // LOAD_LIBRARY_AS_DATAFILE|LOAD_LIBRARY_AS_IMAGE_RESOURCE doesn't care
    // about the extension, only the PE content, so give the file a
    // non-executable-looking extension to sidestep extension-based heuristics.
    const std::wstring destination = sourcePath.substr(0, slash + 1) +
                                     L"wucltux.embedded-mui-" +
                                     std::to_wstring(GetCurrentProcessId()) + L"-" +
                                     g_language + L"-" +
                                     std::to_wstring(generation.fetch_add(1) + 1) + L".mres";
    const std::wstring temporary = destination + L".tmp";
    ScopedTemporaryFile temporaryGuard(temporary);
    DeleteFileW(temporary.c_str());
    if (!CopyFileW(sourcePath.c_str(), temporary.c_str(), FALSE)) return false;
    if (!DisableMuiConfigInPrivateCopy(temporary)) {
        Wh_Log(L"Windows Update Restorer: could not neutralize MUI config in private copy");
        return false;
    }
    ResourceUpdateTransaction update(temporary);
    if (!update.IsValid()) {
        Wh_Log(L"Windows Update Restorer: BeginUpdateResource failed (%u)", GetLastError());
        return false;
    }
    std::vector<BYTE> block;
    // en-US plus it-IT: the latter prevents the Italian Control Panel resource
    // lookup from missing the module before it can fall back to English.
    static const WORD languages[] = {0x0000, 0x0409, 0x0410};
    int blocksWritten = 0;
    int blocksFailed = 0;
    for (UINT blockId = 1; blockId <= 1251; ++blockId) {  // Extended to include WUA strings
        if (!BuildWucltuxStringBlock(blockId, block)) continue;
        for (WORD language : languages) {
            if (!UpdateResourceW(update.Get(), RT_STRING, MAKEINTRESOURCEW(blockId), language,
                                 block.data(), static_cast<DWORD>(block.size()))) {
                // Don't abort the whole build on a single failed block: log it
                // and keep going, so a transient/localized failure (e.g. AV
                // briefly locking the temp file) doesn't leave the page with
                // zero embedded strings.
                Wh_Log(L"Windows Update Restorer: UpdateResource failed (block=%u lang=%04X err=%u)",
                       blockId, language, GetLastError());
                ++blocksFailed;
                continue;
            }
            ++blocksWritten;
        }
    }
    if (blocksWritten == 0) {
        Wh_Log(L"Windows Update Restorer: no string blocks could be written, aborting embedded MUI build");
        return false;
    }
    if (blocksFailed > 0) {
        Wh_Log(L"Windows Update Restorer: embedded MUI build had %d failed block writes (continuing with %d successful)",
               blocksFailed, blocksWritten);
    }
    if (!update.Commit()) {
        Wh_Log(L"Windows Update Restorer: EndUpdateResource failed (%u)", GetLastError());
        return false;
    }
    DeleteFileW(destination.c_str());
    // MoveFileExW can fail with ERROR_ACCESS_DENIED (5) right after the
    // resource-patched file is written, typically because AV/EDR real-time
    // protection holds it open for an on-write scan. That scan can take
    // several seconds (cloud lookups, sandboxing), not milliseconds, so use
    // a longer exponential backoff. This runs on the background setup
    // thread, so a multi-second wait here does not block the UI.
    static const int kMaxMoveAttempts = 10;
    static const DWORD kInitialMoveRetryDelayMs = 200;
    static const DWORD kMaxMoveRetryDelayMs = 2000;
    bool moved = false;
    DWORD lastMoveError = 0;
    DWORD retryDelayMs = kInitialMoveRetryDelayMs;
    for (int attempt = 1; attempt <= kMaxMoveAttempts; ++attempt) {
        if (MoveFileExW(temporary.c_str(), destination.c_str(),
                        MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH)) {
            moved = true;
            break;
        }
        lastMoveError = GetLastError();
        if (lastMoveError != ERROR_ACCESS_DENIED && lastMoveError != ERROR_SHARING_VIOLATION) {
            break;  // Not a transient-lock error; retrying won't help.
        }
        if (g_stopping.load()) break;
        if (attempt < kMaxMoveAttempts) {
            Wh_Log(L"Windows Update Restorer: activating embedded MUI module failed (%u), retrying in %ums (%d/%d)",
                   lastMoveError, retryDelayMs, attempt, kMaxMoveAttempts);
            if (g_stopEvent && WaitForSingleObject(g_stopEvent, retryDelayMs) == WAIT_OBJECT_0) break;
            retryDelayMs = retryDelayMs < kMaxMoveRetryDelayMs / 2 ? retryDelayMs * 2 : kMaxMoveRetryDelayMs;
        }
    }
    if (!moved) {
        Wh_Log(L"Windows Update Restorer: activating embedded MUI module failed (%u) after %d attempts",
               lastMoveError, kMaxMoveAttempts);
        return false;
    }
    temporaryGuard.Commit();
    {
        std::lock_guard<std::mutex> lock(g_resourceMutex);
        g_resourcePath = destination;
    }
    Wh_Log(L"Windows Update Restorer: embedded MUI resource module ready (%d blocks): %s",
           blocksWritten, destination.c_str());
    return true;
}


static HMODULE EmbeddedMuiResourceModule() {
    std::lock_guard<std::mutex> lock(g_resourceMutex);
    if (HMODULE module = g_resourceModule.load()) return module;
    if (g_resourcePath.empty()) return nullptr;
    HMODULE module = LoadLibraryExW(g_resourcePath.c_str(), nullptr,
                                   LOAD_LIBRARY_AS_DATAFILE | LOAD_LIBRARY_AS_IMAGE_RESOURCE);
    if (!module) Wh_Log(L"Windows Update Restorer: loading private MUI resource module failed (%u)", GetLastError());
    g_resourceModule.store(module);
    return module;
}

// Rebuilds the embedded MUI resource module for the currently selected language
// and reloads it. The classic page reads its strings from this module, which is
// built once at startup; changing the language therefore requires a rebuild. We
// run this on the background rebuild thread so it does not block the UI.
static void RebuildEmbeddedMuiForLanguage() {
    std::lock_guard<std::mutex> lock(g_rebuildMutex);
    if (g_stopping.load()) return;
    const std::wstring* path = g_dllPath.load();
    if (!path || path->empty()) {
        Wh_Log(L"Windows Update Restorer: no wucltux.dll path available for language rebuild");
        return;
    }
    if (!BuildEmbeddedMuiResourceModule(*path)) {
        Wh_Log(L"Windows Update Restorer: language rebuild of embedded MUI module failed");
        return;
    }
    // Point the cached module handle at the newly built file. We deliberately
    // never FreeLibrary the old module (a page may still reference it); it is
    // simply replaced as the source for future lookups.
    {
        std::lock_guard<std::mutex> rl(g_resourceMutex);
        g_resourceModule.store(nullptr);
    }
    EmbeddedMuiResourceModule();
    g_builtLanguage = g_language;
    Wh_Log(L"Windows Update Restorer: embedded MUI module rebuilt for language %s", g_language.c_str());
}


// -----------------------------------------------------------------------------
// Embedded warning shield (user supplied image, white outer background removed,
// converted to a small multi-size ICO and stored as base64 in this source).
// -----------------------------------------------------------------------------
static const UINT kLegacyWarningShieldIconId = 61002;
static const char kLegacyWarningShieldIcoBase64[] =
    "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"
    "5gAAAABJRU5ErkJggg==";


// -----------------------------------------------------------------------------
// Embedded "updates installed / up to date" check icon (user supplied image,
// stored as a multi-size ICO in base64). Used by the Win7-style banner when the
// system is fully up to date.
// -----------------------------------------------------------------------------
static const UINT kUpdatesInstalledIconId = 61003;
// The icon is stored as a raw PNG (not wrapped in an ICO) so GDI+ can
// decode it and scale it with HighQualityBicubic interpolation at the
// requested size. A single string literal (concatenated across lines).
static const char kUpdatesInstalledPngBase64[] =
    "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";

// -----------------------------------------------------------------------------
// Embedded Windows 8.1-style Windows Update status icon (user supplied PNG).
// Used when UpdatePageSkin is set to windows81. Stored as a raw PNG so GDI+
// can scale it with HighQualityBicubic interpolation at render time.
// -----------------------------------------------------------------------------
static const UINT kWindows81UpdateStatusIconId = 61005;
static const char kWindows81UpdateStatusPngBase64[] =
    "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";
// -----------------------------------------------------------------------------
// Embedded Control Panel applet logos. The Windows 7 logo is the user supplied
// image with the edge-connected white background removed. Both ICO payloads are
// multi-size and were resampled bicubically, so the shell can pick a matching
// size instead of scaling a single bitmap. The Windows 8.1 ICO is generated from
// the already embedded Windows 8.1 status PNG.
// -----------------------------------------------------------------------------
static const wchar_t* kAppletLogoWin7FileName = L"wuapplet-win7.ico";
static const wchar_t* kAppletLogoWin81FileName = L"wuapplet-win81.ico";
static const wchar_t* kAppletTasksXmlFileName = L"wuapplet-tasks.xml";
static const char kAppletLogoWin7IcoBase64[] =
    "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"
    "v0+Ae1XW/weVWA8bq2S85gAAAABJRU5ErkJggg==";
static const char kAppletLogoWin81IcoBase64[] =
    "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"
    "FKGUT5AAAAAASUVORK5CYII=";


// -----------------------------------------------------------------------------
// Embedded "service unavailable" shield (user supplied image, white background
// removed -> transparent). Shown when the Windows Update service is disabled or
// missing, so the user still gets a friendly, translated notice instead of the
// native (often cryptic) red "automatic updates are off" box. Stored as a raw
// PNG so GDI+ can decode it and scale it with HighQualityBicubic at render time.
static const UINT kWuDisabledShieldIconId = 61004;
static const char kWuDisabledShieldPngBase64[] =
    "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"
    "vt36/wHH7B4fpaoEyQAAAABJRU5ErkJggg=="
    "";


static int Base64Digit(char c) {
    if (c >= 'A' && c <= 'Z') return c - 'A'; if (c >= 'a' && c <= 'z') return c - 'a' + 26;
    if (c >= '0' && c <= '9') return c - '0' + 52; if (c == '+') return 62; if (c == '/') return 63; return -1;
}
static std::vector<BYTE> DecodeBase64Icon(const char* source) {
    std::vector<BYTE> out; int value = 0, bits = -8;
    for (const char* p = source; p && *p; ++p) { if (*p == '=') break; int d = Base64Digit(*p); if (d < 0) continue; value = (value << 6) + d; bits += 6; if (bits >= 0) { out.push_back(static_cast<BYTE>((value >> bits) & 0xFF)); bits -= 8; } }
    return out;
}
#pragma pack(push, 1)
struct EmbeddedIconHeader { WORD reserved, type, count; };
struct EmbeddedIconEntry { BYTE width, height, colors, reserved; WORD planes, bitCount; DWORD bytes, offset; };
#pragma pack(pop)
static HICON g_legacyWarningShield = nullptr;
static HICON GetLegacyWarningShieldIcon() {
    if (g_legacyWarningShield) return g_legacyWarningShield;
    std::vector<BYTE> ico = DecodeBase64Icon(kLegacyWarningShieldIcoBase64);
    if (ico.size() < sizeof(EmbeddedIconHeader) + sizeof(EmbeddedIconEntry)) return nullptr;
    const auto* header = reinterpret_cast<const EmbeddedIconHeader*>(ico.data());
    if (header->type != 1 || !header->count) return nullptr;
    const auto* entry = reinterpret_cast<const EmbeddedIconEntry*>(ico.data() + sizeof(EmbeddedIconHeader));
    if (entry->offset > ico.size() || entry->bytes > ico.size() - entry->offset) return nullptr;
    g_legacyWarningShield = CreateIconFromResourceEx(ico.data() + entry->offset, entry->bytes, TRUE, 0x00030000, 48, 48, LR_DEFAULTCOLOR);
    return g_legacyWarningShield;
}

static HICON g_updatesInstalledIcon = nullptr;

// -----------------------------------------------------------------------------
// GDI+ HighQualityBicubic icon rendering (mirrors win7-network-flyout-recreation)
// -----------------------------------------------------------------------------
// Instead of loading the PNG as a fixed-size icon and letting DirectUI upscale it
// (which blurs it), we decode the PNG with GDI+ and scale it to the requested size
// using InterpolationModeHighQualityBicubic (mode 7). This gives a crisp icon at
// any DPI. We resolve gdiplus.dll once and reuse the pointers/startup token.
static HMODULE g_hGdiPlus = NULL;
static ULONG_PTR g_gdiplusToken = 0;
typedef int (WINAPI *GdipCreateBitmapFromStreamFunc)(IStream*, void**);
typedef int (WINAPI *GdipCreateBitmapFromScan0Func)(int, int, int, int, const void*, void**);
typedef int (WINAPI *GdipGetImageGraphicsContextFunc)(void*, void**);
typedef int (WINAPI *GdipSetInterpolationModeFunc)(void*, int);
typedef int (WINAPI *GdipSetPixelOffsetModeFunc)(void*, int);
typedef int (WINAPI *GdipGraphicsClearFunc)(void*, unsigned int);
typedef int (WINAPI *GdipDrawImageRectIFunc)(void*, void*, int, int, int, int);
typedef int (WINAPI *GdipCreateHICONFromBitmapFunc)(void*, HICON*);
typedef int (WINAPI *GdipDeleteGraphicsFunc)(void*);
typedef int (WINAPI *GdipDisposeImageFunc)(void*);
static GdipCreateBitmapFromStreamFunc pGdipCreateBitmapFromStream = NULL;
static GdipCreateBitmapFromScan0Func pGdipCreateBitmapFromScan0 = NULL;
static GdipGetImageGraphicsContextFunc pGdipGetImageGraphicsContext = NULL;
static GdipSetInterpolationModeFunc pGdipSetInterpolationMode = NULL;
static GdipSetPixelOffsetModeFunc pGdipSetPixelOffsetMode = NULL;
static GdipGraphicsClearFunc pGdipGraphicsClear = NULL;
static GdipDrawImageRectIFunc pGdipDrawImageRectI = NULL;
static GdipCreateHICONFromBitmapFunc pGdipCreateHICONFromBitmap = NULL;
static GdipDeleteGraphicsFunc pGdipDeleteGraphics = NULL;
static GdipDisposeImageFunc pGdipDisposeImage = NULL;

static BOOL InitGdiPlusRendering() {
    if (g_hGdiPlus) return TRUE;
    g_hGdiPlus = LoadLibraryExW(L"gdiplus.dll", NULL, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!g_hGdiPlus) { Wh_Log(L"Windows Update Restorer: GDI+ failed to load"); return FALSE; }
    pGdipCreateBitmapFromStream = (GdipCreateBitmapFromStreamFunc)GetProcAddress(g_hGdiPlus, "GdipCreateBitmapFromStream");
    pGdipCreateBitmapFromScan0 = (GdipCreateBitmapFromScan0Func)GetProcAddress(g_hGdiPlus, "GdipCreateBitmapFromScan0");
    pGdipGetImageGraphicsContext = (GdipGetImageGraphicsContextFunc)GetProcAddress(g_hGdiPlus, "GdipGetImageGraphicsContext");
    pGdipSetInterpolationMode = (GdipSetInterpolationModeFunc)GetProcAddress(g_hGdiPlus, "GdipSetInterpolationMode");
    pGdipSetPixelOffsetMode = (GdipSetPixelOffsetModeFunc)GetProcAddress(g_hGdiPlus, "GdipSetPixelOffsetMode");
    pGdipGraphicsClear = (GdipGraphicsClearFunc)GetProcAddress(g_hGdiPlus, "GdipGraphicsClear");
    pGdipDrawImageRectI = (GdipDrawImageRectIFunc)GetProcAddress(g_hGdiPlus, "GdipDrawImageRectI");
    pGdipCreateHICONFromBitmap = (GdipCreateHICONFromBitmapFunc)GetProcAddress(g_hGdiPlus, "GdipCreateHICONFromBitmap");
    pGdipDeleteGraphics = (GdipDeleteGraphicsFunc)GetProcAddress(g_hGdiPlus, "GdipDeleteGraphics");
    pGdipDisposeImage = (GdipDisposeImageFunc)GetProcAddress(g_hGdiPlus, "GdipDisposeImage");
    if (!pGdipCreateBitmapFromStream || !pGdipCreateBitmapFromScan0 || !pGdipGetImageGraphicsContext ||
        !pGdipSetInterpolationMode || !pGdipSetPixelOffsetMode || !pGdipGraphicsClear ||
        !pGdipDrawImageRectI || !pGdipCreateHICONFromBitmap || !pGdipDeleteGraphics || !pGdipDisposeImage) {
        Wh_Log(L"Windows Update Restorer: GDI+ missing function pointers");
        FreeLibrary(g_hGdiPlus); g_hGdiPlus = NULL; return FALSE;
    }
    typedef int (WINAPI *GdiplusStartupFunc)(ULONG_PTR*, const void*, void*);
    GdiplusStartupFunc pStartup = (GdiplusStartupFunc)GetProcAddress(g_hGdiPlus, "GdiplusStartup");
    if (!pStartup) { FreeLibrary(g_hGdiPlus); g_hGdiPlus = NULL; return FALSE; }
    struct { DWORD Version; void* Callback; BOOL Suppress; } si = {1, NULL, FALSE};
    if (pStartup(&g_gdiplusToken, &si, NULL) != 0) {
        FreeLibrary(g_hGdiPlus); g_hGdiPlus = NULL; return FALSE;
    }
    Wh_Log(L"Windows Update Restorer: GDI+ initialized for bicubic icons");
    return TRUE;
}

static void ShutdownGdiPlusRendering() {
    if (g_hGdiPlus) {
        typedef void (WINAPI *GdiplusShutdownFunc)(ULONG_PTR);
        GdiplusShutdownFunc pShutdown = (GdiplusShutdownFunc)GetProcAddress(g_hGdiPlus, "GdiplusShutdown");
        if (pShutdown && g_gdiplusToken) pShutdown(g_gdiplusToken);
        FreeLibrary(g_hGdiPlus);
        g_hGdiPlus = NULL; g_gdiplusToken = 0;
    }
}

// Decodes a base64 PNG and returns an HICON scaled to targetWidth/Height using
// GDI+ HighQualityBicubic interpolation. Returns NULL on any failure.
static HICON CreateIconFromBase64PngBicubic(const char* base64Str, int targetWidth, int targetHeight) {
    if (!InitGdiPlusRendering() || !pGdipCreateBitmapFromStream || !pGdipCreateHICONFromBitmap)
        return NULL;

    std::vector<BYTE> data = DecodeBase64Icon(base64Str);
    if (data.empty()) return NULL;

    HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, data.size());
    if (!hMem) return NULL;
    void* pMem = GlobalLock(hMem);
    if (!pMem) { GlobalFree(hMem); return NULL; }
    memcpy(pMem, data.data(), data.size());
    GlobalUnlock(hMem);
    IStream* stream = NULL;
    CreateStreamOnHGlobal(hMem, TRUE, &stream);
    if (!stream) { GlobalFree(hMem); return NULL; }

    HICON hIcon = NULL;
    void* srcBitmap = NULL;
    if (pGdipCreateBitmapFromStream(stream, &srcBitmap) == 0 && srcBitmap) {
        bool scaled = false;
        if (targetWidth > 0 && targetHeight > 0 && pGdipCreateBitmapFromScan0 &&
            pGdipGetImageGraphicsContext && pGdipSetInterpolationMode &&
            pGdipSetPixelOffsetMode && pGdipGraphicsClear && pGdipDrawImageRectI &&
            pGdipDeleteGraphics) {
            void* dstBitmap = NULL;
            if (pGdipCreateBitmapFromScan0(targetWidth, targetHeight, 0, 0x00E200B, NULL, &dstBitmap) == 0 && dstBitmap) {
                void* graphics = NULL;
                if (pGdipGetImageGraphicsContext(dstBitmap, &graphics) == 0 && graphics) {
                    pGdipSetInterpolationMode(graphics, 7);   // HighQualityBicubic
                    pGdipSetPixelOffsetMode(graphics, 3);     // HalfPixel
                    pGdipGraphicsClear(graphics, 0);
                    scaled = pGdipDrawImageRectI(graphics, srcBitmap, 0, 0, targetWidth, targetHeight) == 0;
                    pGdipDeleteGraphics(graphics);
                }
                if (scaled) pGdipCreateHICONFromBitmap(dstBitmap, &hIcon);
                pGdipDisposeImage(dstBitmap);
            }
        }
        if (!scaled) pGdipCreateHICONFromBitmap(srcBitmap, &hIcon);
        pGdipDisposeImage(srcBitmap);
    }
    stream->Release();
    return hIcon;
}

static HICON GetUpdatesInstalledIcon() {
    if (g_updatesInstalledIcon) return g_updatesInstalledIcon;
    g_updatesInstalledIcon = CreateIconFromBase64PngBicubic(kUpdatesInstalledPngBase64, 48, 48);
    return g_updatesInstalledIcon;
}

static HICON g_windows81UpdateStatusIcon = nullptr;
static HICON GetWindows81UpdateStatusIcon() {
    if (g_windows81UpdateStatusIcon) return g_windows81UpdateStatusIcon;
    g_windows81UpdateStatusIcon = CreateIconFromBase64PngBicubic(kWindows81UpdateStatusPngBase64, 48, 48);
    return g_windows81UpdateStatusIcon;
}

static HICON g_wuDisabledShieldIcon = nullptr;
static HICON GetWuDisabledShieldIcon() {
    if (g_wuDisabledShieldIcon) return g_wuDisabledShieldIcon;
    g_wuDisabledShieldIcon = CreateIconFromBase64PngBicubic(kWuDisabledShieldPngBase64, 48, 48);
    return g_wuDisabledShieldIcon;
}

using LoadImageW_t = decltype(&LoadImageW);
static LoadImageW_t LoadImageWOriginalForLegacyWarningIcon = nullptr;
static HANDLE WINAPI LoadImageWHookForLegacyWarningIcon(HINSTANCE instance, LPCWSTR name, UINT type, int cx, int cy, UINT flags) {
    if (type == IMAGE_ICON && IS_INTRESOURCE(name)) {
        const UINT id = static_cast<UINT>(reinterpret_cast<UINT_PTR>(name));
        if (id == kLegacyWarningShieldIconId) {
            if (HICON icon = GetLegacyWarningShieldIcon()) return CopyIcon(icon);
        } else if (id == kUpdatesInstalledIconId) {
            if (HICON icon = GetUpdatesInstalledIcon()) return CopyIcon(icon);
        } else if (id == kWindows81UpdateStatusIconId) {
            if (HICON icon = GetWindows81UpdateStatusIcon()) return CopyIcon(icon);
        } else if (id == kWuDisabledShieldIconId) {
            if (HICON icon = GetWuDisabledShieldIcon()) return CopyIcon(icon);
        }
    }
    return LoadImageWOriginalForLegacyWarningIcon(instance, name, type, cx, cy, flags);
}
static void InstallLegacyWarningIconHook() {
    HMODULE user32 = GetModuleHandleW(L"user32.dll");
    if (!user32) user32 = LoadLibraryExW(L"user32.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (user32) if (void* p = reinterpret_cast<void*>(GetProcAddress(user32, "LoadImageW")))
        WindhawkUtils::SetFunctionHook(reinterpret_cast<LoadImageW_t>(p), LoadImageWHookForLegacyWarningIcon, &LoadImageWOriginalForLegacyWarningIcon);
}

// -----------------------------------------------------------------------------
// DirectUI XML patch.  The legacy page is still rendered by wucltux.dll, but
// its original commands are no longer reliable on modern Windows.  We add a
// small, in-page command group whose NavigateButton routes through our private
// ShellExecute command protocol below.  Nothing is written to the DLL on disk.
// -----------------------------------------------------------------------------
#ifdef _WIN64
#define WU_DUI_THISCALL __cdecl
#else
#define WU_DUI_THISCALL __thiscall
#endif

using DUISetXML_t = HRESULT(WU_DUI_THISCALL*)(void*, const WCHAR*, HINSTANCE, HINSTANCE);
using DUISetXMLFromResource_t = HRESULT(WU_DUI_THISCALL*)(
    void*, PCWSTR, PCWSTR, HMODULE, HINSTANCE, HINSTANCE);
static DUISetXML_t DUISetXMLOriginal = nullptr;
static DUISetXMLFromResource_t DUISetXMLFromResourceOriginal = nullptr;
static thread_local int g_inWuXmlPatch = 0;

// Conservative RAII guard for g_inWuXmlPatch: increments on construction and
// guarantees the decrement on destruction, so the re-entrancy guard is released
// even if the body returns early or throws. Prevents double-patching.
class WuXmlPatchGuard {
public:
    WuXmlPatchGuard() { ++g_inWuXmlPatch; }
    WuXmlPatchGuard(const WuXmlPatchGuard&) = delete;
    WuXmlPatchGuard& operator=(const WuXmlPatchGuard&) = delete;
    ~WuXmlPatchGuard() { --g_inWuXmlPatch; }
};

static std::wstring LoadDirectUiResourceXml(HMODULE module, PCWSTR name, PCWSTR type) {
    if (!module || !name || !type) return {};
    HRSRC resource = FindResourceW(module, name, type);
    if (!resource) return {};
    HGLOBAL loaded = LoadResource(module, resource);
    const DWORD bytes = loaded ? SizeofResource(module, resource) : 0;
    const char* data = loaded ? static_cast<const char*>(LockResource(loaded)) : nullptr;
    if (!data || !bytes) return {};
    int chars = MultiByteToWideChar(CP_UTF8, 0, data, static_cast<int>(bytes), nullptr, 0);
    UINT cp = CP_UTF8;
    if (chars <= 0) { cp = CP_ACP; chars = MultiByteToWideChar(cp, 0, data, static_cast<int>(bytes), nullptr, 0); }
    if (chars <= 0) return {};
    std::wstring xml(static_cast<size_t>(chars), L'\0');
    MultiByteToWideChar(cp, 0, data, static_cast<int>(bytes), &xml[0], chars);
    while (!xml.empty() && (xml.back() == L'\0' || xml.back() == L'\r' || xml.back() == L'\n')) xml.pop_back();
    return xml;
}

// Finds the closing tag for the first <element>. DirectUI's XML uses nested
// elements, so inserting before the final tag keeps the added group inside the
// original page instead of creating a second root.
[[maybe_unused]] static bool FindRootElementEnd(const std::wstring& xml, size_t& end) {
    const size_t root = xml.find(L"<element");
    if (root == std::wstring::npos) return false;
    int depth = 0;
    for (size_t i = root; i < xml.size();) {
        if (xml.compare(i, 8, L"<element") == 0) {
            const size_t gt = xml.find(L'>', i);
            if (gt == std::wstring::npos) return false;
            if (gt == i || xml[gt - 1] != L'/') ++depth;
            i = gt + 1;
        } else if (xml.compare(i, 10, L"</element>") == 0) {
            if (--depth == 0) { end = i; return true; }
            i += 10;
        } else {
            ++i;
        }
    }
    return false;
}

// -----------------------------------------------------------------------------
// Multilingual service message (10 languages + English fallback)
// -----------------------------------------------------------------------------
// The currently selected language (a Windhawk setting). The default "auto"
// detects the system language automatically; any specific code overrides it.
// English is always the fallback for any unknown code. Declared near the top of
// the file (see g_language above), where the embedded string table uses it.

// Maps a Windows LANGID (its primary language) to one of the supported codes.
// Returns L"" when the language is not one of the ten supported ones.
static std::wstring LanguageCodeFromLangId(USHORT langId) {
    switch (PRIMARYLANGID(langId)) {
        case LANG_ITALIAN: return L"it";
        case LANG_SPANISH: return L"es";
        case LANG_FRENCH: return L"fr";
        case LANG_TURKISH: return L"tr";
        case LANG_RUSSIAN: return L"ru";
        case LANG_PORTUGUESE: return L"pt";
        case LANG_CHINESE: return L"zh";
        case LANG_POLISH: return L"pl";
        case LANG_DUTCH: return L"nl";
        case LANG_ENGLISH: return L"en";
        default: return L"";
    }
}

// Detects the user's UI language (falls back to the system default UI language,
// then to the default locale). Returns a supported code, or L"" if none matches.
static std::wstring DetectSystemLanguage() {
    std::wstring code = LanguageCodeFromLangId(GetUserDefaultUILanguage());
    if (!code.empty()) return code;
    code = LanguageCodeFromLangId(GetSystemDefaultUILanguage());
    if (!code.empty()) return code;
    code = LanguageCodeFromLangId(GetUserDefaultLCID());
    return code;
}

// Loads the message language from the mod settings. The "auto" value (or an
// empty one) triggers automatic system-language detection. English is the final
// fallback if nothing matches.
static void LoadLanguageSetting() {
    PCWSTR lang = Wh_GetStringSetting(L"Language");
    std::wstring value = (lang && *lang) ? lang : L"auto";
    if (lang) Wh_FreeStringSetting(lang);
    for (auto& c : value) c = towlower(c);
    if (value == L"auto" || value.empty()) {
        std::wstring detected = DetectSystemLanguage();
        g_language = detected.empty() ? L"en" : detected;
    } else {
        g_language = value;
    }

    PCWSTR skin = Wh_GetStringSetting(L"UpdatePageSkin");
    std::wstring skinValue = (skin && *skin) ? skin : L"windows7";
    if (skin) Wh_FreeStringSetting(skin);
    for (auto& c : skinValue) c = towlower(c);
    g_updatePageSkin.store(
        (skinValue == L"windows81" || skinValue == L"windows8.1")
            ? kUpdatePageSkinWindows81
            : kUpdatePageSkinWindows7);

    // Whether the "service not available" shield notice is shown.
    g_showServiceNotice.store(Wh_GetIntSetting(L"ShowServiceNotice") != 0);
    g_showAvailableUpdates.store(Wh_GetIntSetting(L"ShowAvailableUpdates") != 0);
    g_linkSystemSettingsText.store(Wh_GetIntSetting(L"LinkSystemSettingsText") != 0);
    // Debug: force the "pending updates" interface even without a real pending update.
    g_debugForcePending.store(Wh_GetIntSetting(L"DebugForcePendingUpdate") != 0);
}

// Escapes XML special characters so a translation string is safe to embed in the
// DirectUI document.
static std::wstring XmlEscape(const wchar_t* s) {
    std::wstring out;
    if (!s) return out;
    for (const wchar_t* p = s; *p; ++p) {
        switch (*p) {
            case L'&': out += L"&amp;"; break;
            case L'<': out += L"&lt;"; break;
            case L'>': out += L"&gt;"; break;
            case L'"': out += L"&quot;"; break;
            default: out += *p; break;
        }
    }
    return out;
}

// Returns the translated "bold" title and the "normal" explanation for the
// currently selected language. English is used as the fallback.
static void SelectServiceMessage(const wchar_t*& title, const wchar_t*& text) {
    struct Msg { const wchar_t* title; const wchar_t* text; };

    static const std::unordered_map<std::wstring, Msg> kMessages = {
        { L"en", { L"The legacy Windows Update service is currently not available",
                   L"The classic service is not available on this version of Windows. It is recommended to use the modern Settings app to do operations with updates" } },
        { L"it", { L"Il servizio Windows Update legacy non è al momento disponibile",
                   L"Il servizio classico non è disponibile in questa versione di Windows. Si consiglia di usare l'app Impostazioni moderna per eseguire operazioni con gli aggiornamenti" } },
        { L"es", { L"El servicio heredado de Windows Update no está disponible actualmente",
                   L"El servicio clásico no está disponible en esta versión de Windows. Se recomienda usar la aplicación Configuración moderna para realizar operaciones con las actualizaciones" } },
        { L"fr", { L"Le service Windows Update hérité n'est actuellement pas disponible",
                   L"Le service classique n'est pas disponible sur cette version de Windows. Il est recommandé d'utiliser l'application Paramètres moderne pour effectuer des opérations avec les mises à jour" } },
        { L"tr", { L"Eski Windows Update hizmeti şu anda kullanılamıyor",
                   L"Klasik hizmet bu Windows sürümünde kullanılamıyor. Güncellemelerle işlem yapmak için modern Ayarlar uygulamasını kullanmanız önerilir" } },
        { L"ru", { L"Устаревшая служба Windows Update в настоящее время недоступна",
                   L"Классическая служба недоступна в этой версии Windows. Рекомендуется использовать современное приложение «Параметры» для операций с обновлениями" } },
        { L"pt", { L"O serviço herdado do Windows Update não está disponível no momento",
                   L"O serviço clássico não está disponível nesta versão do Windows. Recomenda-se usar o aplicativo Configurações moderno para fazer operações com as atualizações" } },
        { L"zh", { L"旧版 Windows 更新服务当前不可用",
                   L"此版本的 Windows 中经典服务不可用。建议使用现代“设置”应用执行更新操作" } },
        { L"pl", { L"Starsza usługa Windows Update jest obecnie niedostępna",
                   L"Klasyczna usługa nie jest dostępna w tej wersji systemu Windows. Zaleca się korzystanie z nowoczesnej aplikacji Ustawienia w celu wykonywania operacji na aktualizacjach" } },
        { L"nl", { L"De verouderde Windows Update-service is momenteel niet beschikbaar",
                   L"De klassieke service is niet beschikbaar in deze versie van Windows. Het wordt aanbevolen om de moderne app Instellingen te gebruiken om bewerkingen met updates uit te voeren" } },
    };

    auto it = kMessages.find(g_language);
    if (it == kMessages.end()) it = kMessages.find(L"en"); // fallback
    const Msg& m = it->second;
    title = m.title;
    text = m.text;
}


// Returns the three pieces of string 324 ("It is recommended to use the system
// settings to configure updates.") so the middle phrase can become a blue link
// without changing the translated words. The suffix is rendered as its own row
// to avoid DirectUI wrapping only the final word in narrow Control Panel windows.
struct SettingsRecommendationLinkParts {
    const wchar_t* before;
    const wchar_t* link;
    const wchar_t* after;
};

static SettingsRecommendationLinkParts SelectSettingsRecommendationLinkParts() {
    static const std::unordered_map<std::wstring, SettingsRecommendationLinkParts> kParts = {
        { L"en", { L"It is recommended to use the ", L"system settings", L"to configure updates." } },
        { L"it", { L"Si consiglia di utilizzare le ", L"impostazioni del sistema", L"per configurare gli aggiornamenti." } },
        { L"es", { L"Se recomienda usar la ", L"configuración del sistema", L"para configurar las actualizaciones." } },
        { L"fr", { L"Il est recommandé d'utiliser les ", L"paramètres du système", L"pour configurer les mises à jour." } },
        { L"tr", { L"Güncellemeleri yapılandırmak için ", L"sistem ayarlarını", L"kullanmanız önerilir." } },
        { L"ru", { L"Рекомендуется использовать ", L"параметры системы", L"для настройки обновлений." } },
        { L"pt", { L"Recomenda-se usar as ", L"configurações do sistema", L"para configurar atualizações." } },
        { L"zh", { L"建议使用", L"系统设置", L"来配置更新。" } },
        { L"pl", { L"Zaleca się korzystanie z ", L"ustawień systemowych", L"w celu skonfigurowania aktualizacji." } },
        { L"nl", { L"Het wordt aanbevolen om de ", L"systeeminstellingen", L"te gebruiken om updates te configureren." } },
    };

    auto it = kParts.find(g_language);
    if (it == kParts.end()) it = kParts.find(L"en");
    return it->second;
}

static std::wstring BuildPlainStatusDescriptionXml(const wchar_t* desc) {
    return L"<element sheet=\"wuappstyle\" class=\"cp_content_text\" contentalign=\"wrapleft\" content=\""
        + XmlEscape(desc ? desc : L"") +
        L"\"/>";
}

static std::wstring BuildLinkedSettingsRecommendationXml() {
    const SettingsRecommendationLinkParts parts = SelectSettingsRecommendationLinkParts();
    std::wstring xml =
        L"<element layout=\"flowlayout(1)\" contentalign=\"wrapleft\">"
        L"<element layout=\"flowlayout(0,0,0,2)\" contentalign=\"wrapleft\">";
    if (parts.before && *parts.before) {
        xml +=
            L"<element sheet=\"wuappstyle\" class=\"cp_content_text\" content=\""
            + XmlEscape(parts.before) +
            L"\"/>";
    }
    xml +=
        L"<NavigateButton layout=\"flowlayout()\" shellexecute=\"ms-settings:windowsupdate\">"
        L"<Button sheet=\"wu_cp_style\" class=\"cp_content_link\" active=\"mouse | keyboard\" content=\""
        + XmlEscape(parts.link ? parts.link : L"system settings") +
        L"\"/></NavigateButton></element>";
    if (parts.after && *parts.after) {
        xml +=
            L"<element sheet=\"wuappstyle\" class=\"cp_content_text\" contentalign=\"wrapleft\" margin=\"rect(0,1rp,0,0)\" content=\""
            + XmlEscape(parts.after) +
            L"\"/>";
    }
    xml += L"</element>";
    return xml;
}


static const wchar_t* SelectChangeWindowsUpdateSettingsLinkText() {
    static const std::unordered_map<std::wstring, const wchar_t*> kTexts = {
        { L"en", L"Change Windows Update settings" },
        { L"it", L"Cambia impostazioni di Windows Update" },
        { L"es", L"Cambiar la configuración de Windows Update" },
        { L"fr", L"Modifier les paramètres de Windows Update" },
        { L"tr", L"Windows Update ayarlarını değiştir" },
        { L"ru", L"Изменить параметры Центра обновления Windows" },
        { L"pt", L"Alterar configurações do Windows Update" },
        { L"zh", L"更改 Windows 更新设置" },
        { L"pl", L"Zmień ustawienia Windows Update" },
        { L"nl", L"Windows Update-instellingen wijzigen" },
    };
    auto it = kTexts.find(g_language);
    if (it == kTexts.end()) it = kTexts.find(L"en");
    return it->second;
}

static std::wstring BuildWindowsUpdateSettingsPageParams() {
    return L"shell:::" + std::wstring(kAppletClsid) + L"\\pageSettings";
}

static std::wstring BuildChangeWindowsUpdateSettingsLinkXml() {
    return
        L"<NavigateButton layout=\"flowlayout()\" margin=\"rect(0,5rp,0,0)\" "
        L"shellexecute=\"%SystemRoot%\\explorer.exe\" shellexecuteparams=\"" +
        BuildWindowsUpdateSettingsPageParams() +
        L"\">"
        L"<Button sheet=\"wu_cp_style\" class=\"cp_content_link\" active=\"mouse | keyboard\" content=\"" +
        XmlEscape(SelectChangeWindowsUpdateSettingsLinkText()) +
        L"\"/></NavigateButton>";
}

static bool IsWindowsUpdatePageXml(const std::wstring& xml) {
    // The start/status and automatic-update pages do not all share one action
    // name. These are stable string/action references in the Win 8.1 wucltux
    // XMLFILE resource and cover both the normal landing page and the
    // "Turn on automatic updating" page shown on modern Windows.
    return xml.find(L"actionCheckForUpdates") != std::wstring::npos ||
           xml.find(L"actionViewInstalledUpdates") != std::wstring::npos ||
           xml.find(L"resstr(1100)") != std::wstring::npos ||
           xml.find(L"resstr(1149)") != std::wstring::npos ||
           xml.find(L"resstr(1150)") != std::wstring::npos ||
           xml.find(L"resstr(1153)") != std::wstring::npos;
}

static bool FindElementEnd(const std::wstring& xml, size_t start, size_t& end) {
    int depth = 0;
    for (size_t pos = start; pos < xml.size();) {
        if (xml.compare(pos, 8, L"<element") == 0) {
            size_t gt = xml.find(L'>', pos); if (gt == std::wstring::npos) return false;
            if (gt == pos || xml[gt - 1] != L'/') ++depth;
            pos = gt + 1;
        } else if (xml.compare(pos, 10, L"</element>") == 0) {
            if (--depth == 0) { end = pos + 10; return true; }
            pos += 10;
        } else ++pos;
    }
    return false;
}


static std::wstring BuildWuSidebarOpenCommandAttributes() {
    return L"shellexecute=\"%SystemRoot%\\explorer.exe\" shellexecuteparams=\"shell:::" +
           std::wstring(kAppletClsid) + L"\"";
}

static std::wstring BuildWuSidebarLinkRow(UINT stringId, bool withIcon) {
    std::wstring row =
        L"<element class=\"cp_nav_row\" layout=\"borderlayout()\" layoutpos=\"top\">";
    if (withIcon) {
        row += L"<element class=\"cp_nav_img\" layoutpos=\"left\" content=\"resbmp(10,2,255,16rp,16rp,0,0)\"/>";
    } else {
        row += L"<element class=\"cp_nav_h_spacer\" layoutpos=\"left\"/>";
    }
    wchar_t content[32];
    swprintf_s(content, L"resstr(%u)", stringId);
    row +=
        L"<NavigateButton layoutpos=\"left\" layout=\"flowlayout()\" " +
        BuildWuSidebarOpenCommandAttributes() + L">"
        L"<Button class=\"cp_nav_link\" sheet=\"wu_cp_style\" active=\"mouse | keyboard\" content=\"" +
        std::wstring(content) +
        L"\"/></NavigateButton>"
        L"</element>";
    return row;
}

static std::wstring BuildWuNavigationPaneXml() {
    // Reconstructed from the Windows 8.1 wucltux.dll UIFILE 123 navigation pane.
    // The original buttons depended on legacy page actions; here every visible
    // sidebar link conservatively reopens this restored Control Panel page.
    return
        L"<element class=\"cp_nav_pane\" id=\"atom(VistaNavigationPane)\" layout=\"filllayout()\" layoutpos=\"left\">"
        L"<viewer><element id=\"atom(NavPanelWatermark)\" sheet=\"WUCommonNavPanelStyle\"/></viewer>"
        L"<element class=\"cp_nav_list\" layout=\"borderlayout()\">"
        L"<element class=\"cp_nav_task_box\" layout=\"borderlayout()\" layoutpos=\"top\">" +
        BuildWuSidebarLinkRow(350, true) +
        BuildWuSidebarLinkRow(351, false) +
        BuildWuSidebarLinkRow(352, false) +
        BuildWuSidebarLinkRow(353, false) +
        L"</element>"
        L"<element class=\"cp_nav_link_box\" layoutpos=\"bottom\" layout=\"borderlayout()\">"
        L"<element class=\"cp_nav_row\" layout=\"borderlayout()\" layoutpos=\"top\">"
        L"<element class=\"cp_nav_h_spacer\" layoutpos=\"left\"/>"
        L"<element class=\"cp_nav_label\" layoutpos=\"left\" content=\"resstr(1128)\"/>"
        L"</element>" +
        BuildWuSidebarLinkRow(355, false) +
        BuildWuSidebarLinkRow(356, false) +
        L"</element>"
        L"</element>"
        L"</element>";
}

static bool IsWuTopLevelPageXml(const std::wstring& xml) {
    return xml.find(L"atom(toplevel)") != std::wstring::npos ||
           xml.find(L"atom(moduleAUNotConfigured)") != std::wstring::npos;
}

static std::wstring PatchWuNavigationPaneXml(const std::wstring& input) {
    // Only the top-level Windows Update page gets this pane. Child pages such as
    // pageSettings have their own legacy command surface; injecting the pane
    // there can make the legacy DirectUI provider reject the page.
    if (!IsWuTopLevelPageXml(input)) return input;

    std::wstring out = input;
    const std::wstring navMarker = L"id=\"atom(VistaNavigationPane)\"";
    const size_t navId = out.find(navMarker);
    const std::wstring navPane = BuildWuNavigationPaneXml();
    if (navId != std::wstring::npos) {
        const size_t navStart = out.rfind(L"<element", navId);
        size_t navEnd = 0;
        if (navStart != std::wstring::npos && FindElementEnd(out, navStart, navEnd)) {
            out.replace(navStart, navEnd - navStart, navPane);
        }
        return out;
    }

    const size_t wuPageEnd = out.rfind(L"</WUAppPage>");
    if (wuPageEnd != std::wstring::npos) {
        out.insert(wuPageEnd, navPane);
    }
    return out;
}

// -----------------------------------------------------------------------------
// Windows Update service availability
// -----------------------------------------------------------------------------
// The classic page is restored on systems where the legacy service is broken,
// but on systems where Windows Update is actually running we should NOT show the
// obsolete "service not available" notice. We detect this by checking the
// Windows Update service (wuauserv): if it exists and is not disabled, updates
// can still be processed, so the warning is suppressed. If the service is
// missing (uninstalled) or disabled, the notice is shown. The result is cached
// briefly so we do not hit the service manager on every page render.
static std::atomic<bool> g_wuAvailable{false};
static std::atomic<ULONGLONG> g_wuCheckedTick{0};
static constexpr ULONGLONG kWuCheckIntervalMs = 5000;

// RAII wrapper for Service Control Manager handles. This keeps the service
// detection path exception/early-return safe and avoids repeating
// CloseServiceHandle on every branch.
class ScopedServiceHandle {
public:
    ScopedServiceHandle() = default;
    explicit ScopedServiceHandle(SC_HANDLE handle) : handle_(handle) {}
    ~ScopedServiceHandle() { Reset(); }

    ScopedServiceHandle(const ScopedServiceHandle&) = delete;
    ScopedServiceHandle& operator=(const ScopedServiceHandle&) = delete;

    ScopedServiceHandle(ScopedServiceHandle&& other) noexcept
        : handle_(other.Release()) {}
    ScopedServiceHandle& operator=(ScopedServiceHandle&& other) noexcept {
        if (this != &other) Reset(other.Release());
        return *this;
    }

    bool IsValid() const { return handle_ != nullptr; }
    SC_HANDLE Get() const { return handle_; }

    SC_HANDLE Release() {
        SC_HANDLE result = handle_;
        handle_ = nullptr;
        return result;
    }

    void Reset(SC_HANDLE handle = nullptr) {
        if (handle_) CloseServiceHandle(handle_);
        handle_ = handle;
    }

private:
    SC_HANDLE handle_ = nullptr;
};

// Detects whether this is Windows 10 (build < 22000) rather than Windows 11.
// Used to show the "View update history" link only on Windows 10, like the
// classic Control Panel page did. Reads the CurrentBuildNumber once and caches it.
static bool IsWindows10() {
    static int cached = -1;  // -1 = not yet detected
    if (cached != -1) return cached == 1;
    cached = 0;
    HKEY hKey = nullptr;
    if (RegOpenKeyExW(HKEY_LOCAL_MACHINE,
            L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
        wchar_t buf[64] = {};
        DWORD size = sizeof(buf);
        if (RegQueryValueExW(hKey, L"CurrentBuildNumber", nullptr, nullptr,
                             reinterpret_cast<LPBYTE>(buf), &size) == ERROR_SUCCESS) {
            long build = wcstol(buf, nullptr, 10);
            if (build >= 10000 && build < 22000) cached = 1;
        }
        RegCloseKey(hKey);
    }
    return cached == 1;
}

static bool IsWindowsUpdateServiceAvailable() {
    const ULONGLONG now = GetTickCount64();
    const ULONGLONG last = g_wuCheckedTick.load();
    if (now - last < kWuCheckIntervalMs) return g_wuAvailable.load();

    bool available = false;
    ScopedServiceHandle scm(OpenSCManagerW(nullptr, SERVICES_ACTIVE_DATABASE, SC_MANAGER_CONNECT));
    if (scm.IsValid()) {
        ScopedServiceHandle svc(OpenServiceW(scm.Get(), L"wuauserv", SERVICE_QUERY_CONFIG));
        if (svc.IsValid()) {
            DWORD needed = 0;
            if (!QueryServiceConfigW(svc.Get(), nullptr, 0, &needed) &&
                GetLastError() == ERROR_INSUFFICIENT_BUFFER && needed != 0) {
                std::vector<BYTE> buffer(needed);
                if (QueryServiceConfigW(svc.Get(), reinterpret_cast<QUERY_SERVICE_CONFIGW*>(buffer.data()),
                                        needed, &needed)) {
                    const auto* cfg = reinterpret_cast<const QUERY_SERVICE_CONFIGW*>(buffer.data());
                    // Available unless explicitly disabled.
                    available = (cfg->dwStartType != SERVICE_DISABLED);
                }
            }
        }
    }

    g_wuAvailable.store(available);
    g_wuCheckedTick.store(now);
    return available;
}

// -----------------------------------------------------------------------------
// Pending-update detection (mirrors the Win7 Action Center Recreation mod)
// -----------------------------------------------------------------------------
// Like that mod, we do NOT run a full WUA COM search (which can be slow or fail
// on these systems). Instead we read the standard registry flags that Windows
// sets when updates have been downloaded and a reboot is required to apply them.
// Returns true if there are pending updates that need a restart.
static bool IsPendingWindowsUpdate() {    // Key 1: CBS / component-based servicing reboot pending.
    {
        HKEY hKey = nullptr;
        if (RegOpenKeyExW(HKEY_LOCAL_MACHINE,
                L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Component Based Servicing\\RebootPending",
                0, KEY_READ, &hKey) == ERROR_SUCCESS) {
            RegCloseKey(hKey);
            return true;
        }
    }
    // Key 2: Windows Update auto-update reboot required.
    {
        HKEY hKey = nullptr;
        if (RegOpenKeyExW(HKEY_LOCAL_MACHINE,
                L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\WindowsUpdate\\Auto Update\\RebootRequired",
                0, KEY_READ, &hKey) == ERROR_SUCCESS) {
            RegCloseKey(hKey);
            return true;
        }
    }
    return false;
}

// Returns true when Windows Update has downloaded/awaiting-install updates that
// do not (yet) require a reboot. Mirrors the Win7 Action Center approach: the
// "Auto Update\\Results\\Download" key is present after updates have been staged.
// Only meaningful when Windows Update is available and no reboot is pending.
static bool IsUpdatesAvailable() {
    if (IsPendingWindowsUpdate()) return false;
    HKEY hKey = nullptr;
    const wchar_t* keys[] = {
        L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\WindowsUpdate\\Auto Update\\Results\\Download",
        L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\WindowsUpdate\\Auto Update\\Results\\Install",
    };
    for (const wchar_t* k : keys) {
        if (RegOpenKeyExW(HKEY_LOCAL_MACHINE, k, 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
            // A non-empty key with pending update data indicates staged updates.
            RegCloseKey(hKey);
            return true;
        }
    }
    return false;
}

// Reads a REG_SZ value from the Windows Update results registry (Auto Update).
// Used to fill the Win7-style info lines below the status banner.
static std::wstring ReadWuaResultString(const wchar_t* subkey, const wchar_t* valueName) {
    HKEY hKey = nullptr;
    std::wstring out;
    const std::wstring path =
        L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\WindowsUpdate\\Auto Update\\Results\\"
        + std::wstring(subkey);
    if (RegOpenKeyExW(HKEY_LOCAL_MACHINE, path.c_str(), 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
        DWORD type = 0, size = 0;
        if (RegQueryValueExW(hKey, valueName, nullptr, &type, nullptr, &size) == ERROR_SUCCESS &&
            size > 0 && size < 4096) {
            if (type == REG_SZ || type == REG_EXPAND_SZ) {
                out.resize(size / sizeof(wchar_t));
                DWORD written = size;
                if (RegQueryValueExW(hKey, valueName, nullptr, &type,
                                     reinterpret_cast<LPBYTE>(&out[0]), &written) == ERROR_SUCCESS) {
                    while (!out.empty() && out.back() == L'\0') out.pop_back();
                } else {
                    out.clear();
                }
            } else if (type == REG_BINARY && size == sizeof(FILETIME)) {
                FILETIME ft{};
                DWORD written = size;
                if (RegQueryValueExW(hKey, valueName, nullptr, &type,
                                     reinterpret_cast<LPBYTE>(&ft), &written) == ERROR_SUCCESS) {
                    SYSTEMTIME st{};
                    FILETIME local{};
                    if (FileTimeToLocalFileTime(&ft, &local) && FileTimeToSystemTime(&local, &st)) {
                        wchar_t buf[64];
                        swprintf_s(buf, L"%04u/%02u/%02u %02u:%02u",
                                   st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute);
                        out = buf;
                    }
                }
            }
        }
        RegCloseKey(hKey);
    }
    return out;
}

// Builds the "Most recent check for updates" value from the registry. Windows
// Formats a WUA DATE (VT_DATE / OLE automation date) as "YYYY/MM/DD HH:MM".
static std::wstring FormatWuaDate(DATE date) {
    SYSTEMTIME st{};
    if (VariantTimeToSystemTime(date, &st) && st.wYear >= 1601) {
        wchar_t buf[64];
        swprintf_s(buf, L"%04u/%02u/%02u %02u:%02u",
                   st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute);
        return buf;
    }
    return L"";
}

// Returns the current local date/time as "YYYY/MM/DD HH:MM". Used as the "last
// check" timestamp: it reflects the moment the mod queried the system for update
// availability (not Windows' own recorded scan time).
static std::wstring NowLocalTimeText() {
    SYSTEMTIME st{};
    GetLocalTime(&st);
    wchar_t buf[64];
    swprintf_s(buf, L"%04u/%02u/%02u %02u:%02u",
               st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute);
    return buf;
}

// Returns the "last check" timestamp: the moment the mod queried the system for
// update availability. This is NOT Windows' own recorded scan time — it is the
// time this mod asked the system (see the README limitation note). Returns the
// cached value, or refreshes it to now if not set yet.
static std::wstring LastCheckForUpdatesText() {
    if (g_lastQueryTimeText.empty()) g_lastQueryTimeText = NowLocalTimeText();
    return g_lastQueryTimeText;
}

// Builds the "Updates were installed" value. The registry key is often missing on
// Windows 10, so we fall back to the WUA update history (most recent successful
// install), which is the reliable source. Returns a readable string, or empty.
static std::wstring LastInstallTimeText() {
    // 1) Registry (works on Windows 7/8).
    const wchar_t* values[] = { L"LastSuccessTime", L"Success", L"InstallTime" };
    for (const wchar_t* v : values) {
        std::wstring t = ReadWuaResultString(L"Install", v);
        if (!t.empty()) return t;
    }
    // 2) WUA history: most recent successful install.
    std::vector<WuaHistoryEntry> history = GetUpdateHistory(200);
    for (const auto& h : history) {
        if (h.resultCode == orcSucceeded && h.date > 0) {
            std::wstring s = FormatWuaDate(h.date);
            if (!s.empty()) return s;
        }
    }
    return L"";
}

// -----------------------------------------------------------------------------
// Settings page ("pageSettings") patch: the "Change settings" child page.
// -----------------------------------------------------------------------------
// The classic settings page is otherwise left untouched. We only replace the
// non-functional scrolling "Important updates" list module with a small,
// conservative update-history shortcut block (two blue links) that routes to the
// Installed Updates page (Win10) / modern Settings (Win11) and to the modern
// Windows Update settings page.
// Finds the innermost <element id="atom(...)"> containing marker and returns the
// outer element boundaries (start, end) plus the inner content range (innerStart,
// innerEnd) so callers can replace just the children while keeping the id-bearing
// module element intact (DirectUI fails with "cannot find element" if a module
// referenced by id is removed entirely).
static bool FindAtomModuleContaining(const std::wstring& xml, const std::wstring& marker,
                                     size_t& start, size_t& end,
                                     size_t& innerStart, size_t& innerEnd) {
    const size_t m = xml.find(marker);
    if (m == std::wstring::npos) return false;
    bool found = false;
    for (size_t i = 0; i < xml.size();) {
        const size_t lt = xml.find(L"<element", i);
        if (lt == std::wstring::npos || lt > m) break;
        const size_t gt = xml.find(L'>', lt);
        if (gt == std::wstring::npos) break;
        if (gt == lt || xml[gt - 1] == L'/') { i = gt + 1; continue; } // self-closing
        size_t e = 0;
        if (!FindElementEnd(xml, lt, e)) break;
        const std::wstring tag = xml.substr(lt, gt - lt);
        if (tag.find(L"id=\"atom(") != std::wstring::npos && lt < m && e > m) {
            if (!found || lt > start) {
                start = lt; end = e; found = true; // innermost
                innerStart = gt + 1; innerEnd = e - (size_t)9; // strip trailing </element>
            }
        }
        i = gt + 1;
    }
    return found;
}

static std::wstring BuildWuSettingsReplacementXml() {
    const wchar_t* heading = EmbeddedMuiString(64540);
    const wchar_t* linkHistory = EmbeddedMuiString(64541);
    const wchar_t* linkSettings = EmbeddedMuiString(64542);
    if (!heading) heading = L"To view the update history, choose one of the following settings:";
    if (!linkHistory) linkHistory = L"View update history";
    if (!linkSettings) linkSettings = L"Manage updates from the system settings";

    // Link 1 opens the Installed Updates page on Windows 10 (classic CLSID) or
    // the modern Settings update-history page on Windows 11.
    std::wstring cmdHistory;
    if (IsWindows10()) {
        cmdHistory = L"shellexecute=\"%SystemRoot%\\explorer.exe\" shellexecuteparams=\"shell:::{d450a8a1-9568-45c7-9c0e-b4f9fb4537bd}\"";
    } else {
        cmdHistory = L"shellexecute=\"ms-settings:windowsupdate-history\"";
    }

    std::wstring xml =
        L"<element id=\"atom(wusettings_best_effort)\" layoutpos=\"top\" layout=\"borderlayout()\" margin=\"rect(12rp,14rp,12rp,0)\">"
        L"<element sheet=\"wuappstyle\" class=\"moduleborder1\" layoutpos=\"top\" layout=\"borderlayout()\">"
        L"<element layoutpos=\"client\" layout=\"flowlayout(1)\" contentalign=\"wrapleft\" padding=\"rect(12rp,15rp,12rp,15rp)\">"
        L"<element sheet=\"wuappstyle\" class=\"wuapp_content_title\" foreground=\"gtc(CONTROLPANELSTYLE,10,1,3803)\" margin=\"rect(0,-3rp,0,0)\" contentalign=\"wrapleft\" content=\""
        + XmlEscape(heading) + L"\"/>"
        L"<element layout=\"flowlayout(0,0,0,2)\" contentalign=\"wrapleft\">"
        L"<NavigateButton layout=\"flowlayout()\" " + cmdHistory + L">"
        L"<Button sheet=\"wu_cp_style\" class=\"cp_content_link\" active=\"mouse | keyboard\" content=\""
        + XmlEscape(linkHistory) + L"\"/></NavigateButton></element>"
        L"<element layout=\"flowlayout(0,0,0,2)\" contentalign=\"wrapleft\">"
        L"<NavigateButton layout=\"flowlayout()\" shellexecute=\"ms-settings:windowsupdate\">"
        L"<Button sheet=\"wu_cp_style\" class=\"cp_content_link\" active=\"mouse | keyboard\" content=\""
        + XmlEscape(linkSettings) + L"\"/></NavigateButton></element>"
        L"</element></element></element>";
    return xml;
}

static std::wstring PatchWuSettingsPageXml(const std::wstring& input) {
    if (input.find(L"atom(pageSettings)") == std::wstring::npos)
        return input;
    // Replace the non-functional scrolling "Important updates" list (its heading
    // is the "&Important updates" string, resstr 1232) with the update-history
    // shortcut block. We keep the module element (and its id) and swap only its
    // children so DirectUI never loses a referenced element.
    size_t modStart = 0, modEnd = 0, innerStart = 0, innerEnd = 0;
    if (FindAtomModuleContaining(input, L"resstr(1232)", modStart, modEnd, innerStart, innerEnd)) {
        Wh_Log(L"Windows Update Restorer: replaced Important updates list children on settings page");
        std::wstring out = input;
        out.replace(innerStart, innerEnd - innerStart, BuildWuSettingsReplacementXml());
        return out;
    }
    Wh_Log(L"Windows Update Restorer: settings page patched but Important updates marker not found");
    return input;
}

// Applies the top-level Windows Update page XML patch. The settings child page
// (pageSettings) gets its own targeted patch (the Important updates list swap).
static std::wstring PatchModernWuPageXml(const std::wstring& input) {
    if (!IsWindowsUpdatePageXml(input)) return input;

    // The legacy Windows Update settings child page only gets the small
    // "Important updates" list replacement (see PatchWuSettingsPageXml); the
    // rest of it is left to the original legacy provider.
    if (input.find(L"atom(pageSettings)") != std::wstring::npos)
        return PatchWuSettingsPageXml(input);

    // Restore/normalize the Windows 7/8.1-style left navigation pane for the
    // top-level Windows Update page, including fallback layouts.
    std::wstring withNavPane = PatchWuNavigationPaneXml(input);

    if (withNavPane.find(L"wuamodern_best_effort") != std::wstring::npos)
        return withNavPane;

    // Only inject anything if the user has the notice enabled. Keep the sidebar
    // patch even when the recreated status box itself is disabled.
    if (!g_showServiceNotice.load()) return withNavPane;

    // Place the new hub *after* the legacy warning module, in the normal white
    // document area. It deliberately does not alter the red legacy card.
    const std::wstring module = L"<element id=\"atom(moduleAUNotConfigured)\"";
    const size_t moduleStart = withNavPane.find(module);
    if (moduleStart == std::wstring::npos) return withNavPane;

    // Decide what to show:
    //  - If Windows Update is actually available (service running), show a live
    //    status box based on the standard pending-update registry flags (the same
    //    approach as the Win7 Action Center Recreation mod). If updates are
    //    pending, prompt to restart; otherwise show "up to date". The obsolete
    //    "service not available" warning is suppressed in this case.
    //  - Otherwise (service disabled/uninstalled) show the shield warning.
    const bool wuAvailable = IsWindowsUpdateServiceAvailable();

    std::wstring patched = withNavPane;
    size_t insertAt = moduleStart;
    if (wuAvailable) {
        // When Windows Update is available and updates are applied, the native red
        // "automatic updates are off" box (moduleAUNotConfigured) is misleading and
        // must be suppressed. The DirectUI provider re-shows modules at runtime, so
        // simply adding visible="false" in XML is overridden and does NOT hide it.
        // Instead we remove the whole module element from the XML; the provider then
        // has nothing to show. The position it occupied becomes our insertion point.
        size_t moduleEnd = 0;
        if (!FindElementEnd(patched, moduleStart, moduleEnd)) return input;
        patched.erase(moduleStart, moduleEnd - moduleStart);
        insertAt = moduleStart;
    }

    std::wstring hub;
    if (wuAvailable) {
        // Windows Update is available: replicate the classic Windows 7 header.
        //  - A colored rectangle (green = up to date, orange = pending updates
        //    or updates available) with a shield/check icon, the "Windows Update"
        //    title and a status line ("No important updates available", etc.).
        //  - Below it, three info lines: most recent check, updates installed,
        //    and which updates you receive.
        // DebugForcePendingUpdate forces the "pending updates" state so the
        // interface can be previewed even when Windows reports nothing pending.
        const bool pending =
            IsPendingWindowsUpdate() || g_debugForcePending.load();
        // Only surface the "updates available" (amber) state when the user
        // enabled it (default off) — otherwise treat available updates as
        // "up to date".
        const bool updatesAvailable = IsUpdatesAvailable() && g_showAvailableUpdates.load();
        // Icon skin: Windows 7/current uses the existing green/check and warning
        // shields. Windows 8.1 uses the supplied Windows Update icon instead
        // of those two embedded status shields. The amber available-updates
        // shield and the disabled-service/fallback notice are intentionally unchanged.
        UINT iconId = kLegacyWarningShieldIconId;
        if (updatesAvailable) {
            iconId = 105;
        } else if (IsWindows81Skin()) {
            iconId = kWindows81UpdateStatusIconId;
        } else if (!pending) {
            iconId = kUpdatesInstalledIconId;
        }
        const wchar_t* statusText = nullptr;
        const wchar_t* desc = L"";
        if (pending) {
            statusText = EmbeddedMuiString(185);       // "Pending restart"
            desc = EmbeddedMuiString(226);
        } else if (updatesAvailable) {
            statusText = EmbeddedMuiString(20022);     // "There are updates available"
            desc = EmbeddedMuiString(20023);           // "Go to Windows Settings to install them"
            if (!desc) desc = L"Go to Windows Settings to install them";
        } else {
            statusText = EmbeddedMuiString(304);       // "No important updates available"
            desc = EmbeddedMuiString(324);             // "It is recommended to use the system settings..."
        }
        if (!statusText) statusText = L"";
        if (!desc) desc = L"";
        const bool linkSettingsRecommendation =
            !pending && !updatesAvailable && g_linkSystemSettingsText.load();
        const std::wstring descXml = linkSettingsRecommendation
            ? BuildLinkedSettingsRecommendationXml()
            : BuildPlainStatusDescriptionXml(desc);

        wchar_t iconSpec[64];
        swprintf_s(iconSpec, L"icon(%u,48rp,48rp,library(shell32.dll))", iconId);

        // Info lines below the banner, stacked as a vertical column (each line is
        // a layoutpos="top" row inside the borderlayout parent, like perfcenter).
        std::wstring lastCheck = LastCheckForUpdatesText();
        std::wstring lastInstall = LastInstallTimeText();
        // "You receive updates:" + "For Windows only."
        const wchar_t* recvLabel = EmbeddedMuiString(1225);   // "You receive updates: "
        const wchar_t* winOnly = EmbeddedMuiString(187);      // "For Windows only."
        std::wstring recvVal = (winOnly ? winOnly : L"For Windows only.");

        std::wstring infoBlock;
        auto addInfoLine = [&](const wchar_t* label, const std::wstring& value) {
            // Always render the label row; append the value only when available.
            infoBlock +=
                L"<element layoutpos=\"top\" layout=\"flowlayout()\" margin=\"rect(0,5rp,0,0)\">"
                L"<element sheet=\"wuappstyle\" class=\"cp_content_text\" content=\""
                + XmlEscape(label) +
                L"\"/>";
            if (!value.empty()) {
                infoBlock +=
                    L"<element sheet=\"wuappstyle\" class=\"cp_content_text\" content=\""
                    + XmlEscape(value.c_str()) +
                    L"\"/>";
            }
            infoBlock += L"</element>";
        };
        if (!lastCheck.empty() && lastCheck.back() != L'.') lastCheck += L".";
        addInfoLine(EmbeddedMuiString(1144), lastCheck); // "Most recent check for updates:"
        // "Updates were installed:" row, always shown, with the optional Windows 10
        // "View update history" blue link that opens the Installed Updates page.
        {
            infoBlock +=
                L"<element layoutpos=\"top\" layout=\"flowlayout()\" margin=\"rect(0,5rp,0,0)\">"
                L"<element sheet=\"wuappstyle\" class=\"cp_content_text\" content=\""
                + XmlEscape(EmbeddedMuiString(1145)) +
                L"\"/>";
            if (!lastInstall.empty()) {
                std::wstring installVal = lastInstall;
                if (installVal.back() != L'.') installVal += L".";
                infoBlock +=
                    L"<element sheet=\"wuappstyle\" class=\"cp_content_text\" content=\""
                    + XmlEscape(installVal.c_str()) +
                    L"\"/>";
            }
            if (IsWindows10()) {
                infoBlock +=
                    L"<NavigateButton layout=\"flowlayout()\" shellexecute=\"%SystemRoot%\\explorer.exe\" "
                    L"shellexecuteparams=\"shell:::{d450a8a1-9568-45c7-9c0e-b4f9fb4537bd}\">"
                    L"<Button sheet=\"wu_cp_style\" class=\"cp_content_link\" active=\"mouse | keyboard\" content=\""
                    + std::wstring(EmbeddedMuiString(74) ? EmbeddedMuiString(74) : L"View update history") +
                    L"\"/></NavigateButton>";
            }
            infoBlock += L"</element>";
        }
        {
            infoBlock +=
                L"<element layoutpos=\"top\" layout=\"flowlayout()\" margin=\"rect(0,5rp,0,0)\">"
                L"<element sheet=\"wuappstyle\" class=\"cp_content_text\" content=\""
                + XmlEscape(recvLabel) +
                L"\"/><element sheet=\"wuappstyle\" class=\"cp_content_text\" content=\""
                + XmlEscape(recvVal.c_str()) +
                L"\"/></element>";
        }

        // Footer: "Get updates for other Microsoft products." + azzurro
        // "Find out more" link (opens microsoft.com), inside a light-blue bordered box.
        {
            infoBlock +=
                L"<element layoutpos=\"top\" layout=\"borderlayout()\" bordercolor=\"RGB(163,207,245)\" "
                L"borderthickness=\"rect(1rp,1rp,1rp,1rp)\" padding=\"rect(12rp,10rp,12rp,10rp)\" margin=\"rect(0,14rp,0,0)\">"
                L"<element layoutpos=\"top\" layout=\"flowlayout(0,0,0,2)\">"
                L"<element sheet=\"wuappstyle\" class=\"cp_content_text\" content=\""
                + std::wstring(EmbeddedMuiString(20020) ? EmbeddedMuiString(20020) : L"Get updates for other Microsoft products.") +
                L"\"/>"
                L"<NavigateButton layout=\"flowlayout()\" shellexecute=\"https://www.microsoft.com\">"
                L"<Button sheet=\"wu_cp_style\" class=\"cp_content_link\" active=\"mouse | keyboard\" content=\""
                + std::wstring(EmbeddedMuiString(20021) ? EmbeddedMuiString(20021) : L"Find out more") +
                L"\"/></NavigateButton>"
                L"</element></element>";
        }

        // The status rectangle: bordered module (grey border) with a colored strip,
        // icon and a large azzurro (light-blue) status title. Everything sits in a
        // borderlayout parent so children with layoutpos="top" stack in a column.
        // The side strip is orange when there are pending updates (restart
        // pending) or available updates, green when the PC is up to date.
        std::wstring colorClass =
            (pending || updatesAvailable) ? L"orange" : L"green";
        hub =
            L"<element id=\"atom(wuamodern_best_effort)\" layoutpos=\"top\" layout=\"borderlayout()\" "
            L"margin=\"rect(12rp,14rp,12rp,0)\">"
            // --- status rectangle (grey border from wuappstyle moduleborder1) ---
            L"<element sheet=\"wuappstyle\" class=\"moduleborder1\" layoutpos=\"top\" layout=\"borderlayout()\">";
        if (colorClass == L"orange") {
            hub +=
                L"<element layoutpos=\"left\" background=\"RGB(240,145,10)\" width=\"16rp\"/>";
        } else {
            hub +=
                L"<element layoutpos=\"left\" sheet=\"wuappstyle\" class=\"security_box_gradient_"
                + colorClass +
                L"\" width=\"16rp\"/>";
        }
        hub +=
            L"<element layoutpos=\"client\" layout=\"borderlayout()\" padding=\"rect(12rp,15rp,12rp,15rp)\">"
            L"<element layoutpos=\"top\" layout=\"borderlayout()\">"
            L"<viewer layoutpos=\"left\" padding=\"rect(0,0,12rp,0)\">"
            L"<element content=\"" + std::wstring(iconSpec) + L"\"/></viewer>"
            L"<element layoutpos=\"client\" layout=\"flowlayout(1)\" contentalign=\"wrapleft\">"
            L"<element sheet=\"wuappstyle\" class=\"wuapp_content_title\" foreground=\"gtc(CONTROLPANELSTYLE,10,1,3803)\" margin=\"rect(0,-3rp,0,0)\" contentalign=\"wrapleft\" content=\""
            + XmlEscape(statusText) +
            L"\"/>"
            + descXml +
            BuildChangeWindowsUpdateSettingsLinkXml() +
            L"</element></element>"
            L"</element>"
            L"</element>"
            // --- info lines below the rectangle, each its own column row ---
            + infoBlock +
            L"</element>";
    } else {
        // Windows Update is disabled/missing. Instead of leaving the user with
        // the cryptic native red box alone, show a friendly, fully translated
        // notice (the "service not available" message) with the embedded shield
        // icon. It is inserted just above the native box, which is still
        // accurate, so the user gets a clear explanation plus the system's own
        // indicator.
        const wchar_t* title = nullptr;
        const wchar_t* text = nullptr;
        SelectServiceMessage(title, text);
        if (!title) title = L"Windows Update service is not available";
        if (!text) text = L"";
        wchar_t iconSpec[64];
        swprintf_s(iconSpec, L"icon(%u,48rp,48rp,library(shell32.dll))",
                   static_cast<unsigned>(kWuDisabledShieldIconId));
        hub =
            L"<element id=\"atom(wuamodern_best_effort)\" layoutpos=\"top\" layout=\"borderlayout()\" "
            L"margin=\"rect(12rp,14rp,12rp,0)\">"
            L"<element sheet=\"wuappstyle\" class=\"moduleborder1\" layoutpos=\"top\" layout=\"borderlayout()\">"
            L"<element layoutpos=\"left\" sheet=\"wuappstyle\" class=\"security_box_gradient_red\" width=\"16rp\"/>"
            L"<element layoutpos=\"client\" layout=\"borderlayout()\" padding=\"rect(12rp,15rp,12rp,15rp)\">"
            L"<element layoutpos=\"top\" layout=\"borderlayout()\">"
            L"<viewer layoutpos=\"left\" padding=\"rect(0,0,12rp,0)\">"
            L"<element content=\"" + std::wstring(iconSpec) + L"\"/></viewer>"
            L"<element layoutpos=\"client\" layout=\"flowlayout(1)\" contentalign=\"wrapleft\">"
            L"<element sheet=\"wuappstyle\" class=\"wuapp_content_title\" foreground=\"gtc(CONTROLPANELSTYLE,10,1,3803)\" margin=\"rect(0,-3rp,0,0)\" contentalign=\"wrapleft\" content=\""
            + XmlEscape(title) +
            L"\"/><element sheet=\"wuappstyle\" class=\"cp_content_text\" contentalign=\"wrapleft\" content=\""
            + XmlEscape(text) +
            L"\"/></element></element>"
            L"</element>"
            L"</element>"
            L"</element>";
    }

    patched.insert(insertAt, hub);
    return patched;
}

// Re-runs the XML patch on the cached page and calls SetXML again so the page
// shows the latest status. Must run on the page's owning (GUI) thread; it is
// called from the ShellExecute hook, which runs on the Control Panel GUI thread.
static void ReRenderWuPage() {
    std::lock_guard<std::mutex> lock(g_wuRenderMutex);
    if (!DUISetXMLOriginal || !g_wuParser || g_wuBaseXml.empty()) return;
    const std::wstring patched = PatchModernWuPageXml(g_wuBaseXml);
    if (patched.empty() || patched == g_wuBaseXml) return;
    WuXmlPatchGuard guard;
    DUISetXMLOriginal(g_wuParser, patched.c_str(), g_wuResModule, g_wuHInstance);
}

// Runs a synchronous WUA search, updates the cached status and re-renders the
// page in place. Called on the GUI thread when the user clicks the in-page
// "Check for updates" link (instead of opening a debug message box).
// Re-checks Windows Update state (pending-update registry flags) and re-renders
// the status box in place. Runs on the GUI thread via the ShellExecute hook.
static void DoCheckForUpdatesInPage() {
    ReRenderWuPage();
}
// Debug helper: writes the raw settings-page XML to the mod storage folder so
// the real element structure can be inspected. Remove in production.
static void DumpSettingsXmlIfNeeded(const std::wstring& xml, const wchar_t* tag) {
    if (xml.find(L"atom(pageSettings)") == std::wstring::npos) return;
    std::wstring dir = StoreDir();
    if (dir.empty()) return;
    std::wstring path = dir + L"\\wusettings_dump_" + std::wstring(tag) + L".xml";
    int need = WideCharToMultiByte(CP_UTF8, 0, xml.c_str(), (int)xml.size(), nullptr, 0, nullptr, nullptr);
    std::string utf8;
    if (need > 0) { utf8.resize(need); WideCharToMultiByte(CP_UTF8, 0, xml.c_str(), (int)xml.size(), &utf8[0], need, nullptr, nullptr); }
    HANDLE file = CreateFileW(path.c_str(), GENERIC_WRITE, FILE_SHARE_READ, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (file == INVALID_HANDLE_VALUE) return;
    DWORD written = 0;
    WriteFile(file, utf8.data(), (DWORD)utf8.size(), &written, nullptr);
    CloseHandle(file);
    Wh_Log(L"Windows Update Restorer: settings XML dumped to %s", path.c_str());
}

static HRESULT WU_DUI_THISCALL DUISetXMLHook(void* parser, const WCHAR* xml,
                                              HINSTANCE resourceModule,
                                              HINSTANCE hInstance) {
    if (!DUISetXMLOriginal) return E_FAIL;
    if (g_inWuXmlPatch || !xml) {
        return DUISetXMLOriginal(parser, xml, resourceModule, hInstance);
    }
    std::wstring patched = PatchModernWuPageXml(xml);
    DumpSettingsXmlIfNeeded(xml, L"setxml");
    if (patched == xml) return DUISetXMLOriginal(parser, xml, resourceModule, hInstance);
    {
        std::lock_guard<std::mutex> lock(g_wuRenderMutex);
        g_wuParser = parser;
        g_wuResModule = resourceModule;
        g_wuHInstance = hInstance;
        g_wuBaseXml = xml;
    }
    Wh_Log(L"Windows Update Restorer: modern WUA links injected through SetXML");
    WuXmlPatchGuard guard;
    HRESULT hr = DUISetXMLOriginal(parser, patched.c_str(), resourceModule, hInstance);
    return hr;
}

static HRESULT WU_DUI_THISCALL DUISetXMLFromResourceHook(
    void* parser, PCWSTR resourceName, PCWSTR resourceType, HMODULE resourceModule,
    HINSTANCE hInstance1, HINSTANCE hInstance2) {
    if (!DUISetXMLFromResourceOriginal) return E_FAIL;
    if (!DUISetXMLOriginal || g_inWuXmlPatch) {
        return DUISetXMLFromResourceOriginal(parser, resourceName, resourceType, resourceModule,
                                             hInstance1, hInstance2);
    }
    std::wstring xml = LoadDirectUiResourceXml(resourceModule, resourceName, resourceType);
    // Do not rely solely on a numeric resource ID: different legacy builds use
    // different XMLFILE IDs. The two stock action names identify the WU landing page.
    if (xml.empty() || !IsWindowsUpdatePageXml(xml)) {
        return DUISetXMLFromResourceOriginal(parser, resourceName, resourceType, resourceModule,
                                             hInstance1, hInstance2);
    }
    std::wstring patched = PatchModernWuPageXml(xml);
    DumpSettingsXmlIfNeeded(xml, L"fromres");
    if (patched == xml) return DUISetXMLFromResourceOriginal(parser, resourceName, resourceType, resourceModule,
                                                              hInstance1, hInstance2);
    {
        std::lock_guard<std::mutex> lock(g_wuRenderMutex);
        g_wuParser = parser;
        g_wuResModule = reinterpret_cast<HINSTANCE>(resourceModule);
        g_wuHInstance = hInstance1;
        g_wuBaseXml = xml;
    }
    WuXmlPatchGuard guard;
    const HRESULT hr = DUISetXMLOriginal(parser, patched.c_str(), reinterpret_cast<HINSTANCE>(resourceModule), hInstance1);
    Wh_Log(L"Windows Update Restorer: modern WUA command links injected (hr=0x%08X)",
           static_cast<unsigned>(hr));
    return hr;
}

static void InstallModernWuXmlPatchHook() {
    HMODULE dui70 = GetModuleHandleW(L"dui70.dll");
    if (!dui70) dui70 = LoadLibraryExW(L"dui70.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!dui70) return;
    for (const char* name : {
#ifdef _WIN64
             "?SetXML@DUIXmlParser@DirectUI@@QEAAJPEBGPEAUHINSTANCE__@@1@Z",
#endif
             "?SetXML@DUIXmlParser@DirectUI@@QAAJPBGPAUHINSTANCE__@@1@Z" }) {
        if (FARPROC proc = GetProcAddress(dui70, name)) { DUISetXMLOriginal = reinterpret_cast<DUISetXML_t>(proc); break; }
    }
    if (!DUISetXMLOriginal) { Wh_Log(L"Windows Update Restorer: DirectUI SetXML not found"); return; }
    // Some wucltux pages call SetXML directly rather than _SetXMLFromResource.
    // Hook both paths; the thread-local guard prevents double patching.
    DUISetXML_t setXmlTarget = DUISetXMLOriginal;
    WindhawkUtils::SetFunctionHook(setXmlTarget, DUISetXMLHook, &DUISetXMLOriginal);
    for (const char* name : {
#ifdef _WIN64
             "?_SetXMLFromResource@DUIXmlParser@DirectUI@@IEAAJPEBG0PEAUHINSTANCE__@@11@Z",
#endif
             "?_SetXMLFromResource@DUIXmlParser@DirectUI@@IAEJPBG0PAUHINSTANCE__@@11@Z" }) {
        if (FARPROC proc = GetProcAddress(dui70, name)) {
            WindhawkUtils::SetFunctionHook(reinterpret_cast<DUISetXMLFromResource_t>(proc),
                                           DUISetXMLFromResourceHook,
                                           &DUISetXMLFromResourceOriginal);
            break;
        }
    }
    if (!DUISetXMLFromResourceOriginal)
        Wh_Log(L"Windows Update Restorer: DirectUI _SetXMLFromResource hook failed");
}

using XResourceProviderCreate_t = HRESULT(*)(HINSTANCE, LPCWSTR, LPCWSTR, LPCWSTR, void**);
static XResourceProviderCreate_t XResourceProviderCreateOriginal = nullptr;
static HRESULT XResourceProviderCreateHook(HINSTANCE instance, LPCWSTR resourceName,
                                           LPCWSTR resourceType, LPCWSTR stylesheetName,
                                           void** provider) {
    HINSTANCE resourceInstance = instance;
    if (IsWucltuxInstance(instance)) {
        if (HMODULE embedded = EmbeddedMuiResourceModule())
            resourceInstance = reinterpret_cast<HINSTANCE>(embedded);
    }
    return XResourceProviderCreateOriginal(resourceInstance, resourceName, resourceType,
                                           stylesheetName, provider);
}

// -----------------------------------------------------------------------------
// ShellExecute Hook for Modern WUA Integration
// -----------------------------------------------------------------------------
using ShellExecuteW_t = decltype(&ShellExecuteW);
static ShellExecuteW_t ShellExecuteWOriginal = nullptr;

static std::wstring EnsureAppletLogoIconFile(bool windows81Skin);

static bool IsModernWuAction(PCWSTR params, const wchar_t* action) {
    if (!params || !action) return false;
    std::wstring value = params;
    for (auto& c : value) c = towlower(c);
    std::wstring expected = L"wuamodern:";
    expected += action;
    for (auto& c : expected) c = towlower(c);
    return value == expected;
}

static bool HandleModernWuAction(HWND hwnd, PCWSTR params) {
    if (IsModernWuAction(params, L"checkupdates")) {
        DoCheckForUpdatesInPage();
        return true;
    }
    if (IsModernWuAction(params, L"history")) {
        ShowUpdateHistory(hwnd);
        return true;
    }
    if (IsModernWuAction(params, L"installed")) {
        OpenInstalledUpdates(hwnd);
        return true;
    }
    if (IsModernWuAction(params, L"settings")) {
        if (ShellExecuteWOriginal)
            ShellExecuteWOriginal(hwnd, L"open", L"ms-settings:windowsupdate", nullptr, nullptr, SW_SHOWNORMAL);
        return true;
    }
    return false;
}

static HINSTANCE WINAPI ShellExecuteWHook(HWND hwnd, LPCWSTR operation, LPCWSTR file,
                                          LPCWSTR parameters, LPCWSTR directory,
                                          INT showCmd) {
    if (HandleModernWuAction(hwnd, parameters)) {
        return reinterpret_cast<HINSTANCE>(static_cast<INT_PTR>(33));
    }
    return ShellExecuteWOriginal(hwnd, operation, file, parameters, directory, showCmd);
}



// -----------------------------------------------------------------------------
// Shell presentation hooks for the restored Control Panel page.
// -----------------------------------------------------------------------------
// The legacy page definition uses indirect strings/icons such as
// "@wucltux.dll,-73" and "wucltux.dll,-1" for child pages (for example
// shell:::{36EEF7DB-88AD-4E81-AD49-0E313F0C35F8}\pageSettings). On modern
// systems those shell-level lookups can fail or use the wrong icon, even though
// DirectUI's in-page strings are already supplied by our embedded MUI table.
// These hooks keep the breadcrumb/page title and page icon consistent with the
// selected skin without modifying the verified wucltux.dll payload.
static bool IsWucltuxPathString(PCWSTR path) {
    if (!path || !*path) return false;
    std::wstring value = path;
    for (auto& c : value) c = towlower(c);
    return value.find(L"wucltux.dll") != std::wstring::npos;
}

static bool TryParseWucltuxIndirectStringId(PCWSTR source, UINT& id) {
    id = 0;
    if (!IsWucltuxPathString(source)) return false;
    const wchar_t* comma = wcsrchr(source, L',');
    if (!comma || !comma[1]) return false;
    int parsed = _wtoi(comma + 1);
    if (parsed < 0) parsed = -parsed;
    if (parsed <= 0) return false;
    id = static_cast<UINT>(parsed);
    return true;
}

using SHLoadIndirectString_t = HRESULT(WINAPI*)(PCWSTR, PWSTR, UINT, void**);
static SHLoadIndirectString_t SHLoadIndirectStringOriginal = nullptr;
static HRESULT WINAPI SHLoadIndirectStringHook(PCWSTR source, PWSTR outBuf,
                                               UINT outChars, void** reserved) {
    UINT id = 0;
    if (TryParseWucltuxIndirectStringId(source, id)) {
        if (const wchar_t* text = EmbeddedMuiString(id)) {
            if (outBuf && outChars) {
                CopyEmbeddedString(text, outBuf, static_cast<int>(outChars));
            }
            return S_OK;
        }
    }
    return SHLoadIndirectStringOriginal(source, outBuf, outChars, reserved);
}

static HICON LoadAppletLogoIconForShell(int size) {
    if (size <= 0) size = GetSystemMetrics(SM_CXICON);
    const std::wstring iconPath = EnsureAppletLogoIconFile(IsWindows81Skin());
    if (iconPath.empty()) return nullptr;
    return reinterpret_cast<HICON>(LoadImageW(nullptr, iconPath.c_str(), IMAGE_ICON,
                                              size, size,
                                              LR_LOADFROMFILE | LR_DEFAULTCOLOR));
}

using ExtractIconExW_t = UINT(WINAPI*)(LPCWSTR, int, HICON*, HICON*, UINT);
static ExtractIconExW_t ExtractIconExWOriginal = nullptr;
static UINT WINAPI ExtractIconExWHook(LPCWSTR file, int iconIndex,
                                      HICON* largeIcons, HICON* smallIcons,
                                      UINT icons) {
    if (IsWucltuxPathString(file)) {
        if (iconIndex == -1 || icons == 0) return 1;
        UINT loaded = 0;
        if (largeIcons) {
            largeIcons[0] = LoadAppletLogoIconForShell(GetSystemMetrics(SM_CXICON));
            if (largeIcons[0]) loaded = 1;
        }
        if (smallIcons) {
            smallIcons[0] = LoadAppletLogoIconForShell(GetSystemMetrics(SM_CXSMICON));
            if (smallIcons[0]) loaded = 1;
        }
        return loaded;
    }
    return ExtractIconExWOriginal(file, iconIndex, largeIcons, smallIcons, icons);
}

using PrivateExtractIconsW_t = UINT(WINAPI*)(LPCWSTR, int, int, int, HICON*, UINT*, UINT, UINT);
static PrivateExtractIconsW_t PrivateExtractIconsWOriginal = nullptr;
static UINT WINAPI PrivateExtractIconsWHook(LPCWSTR file, int iconIndex,
                                            int cxIcon, int cyIcon, HICON* icons,
                                            UINT* iconIds, UINT iconCount,
                                            UINT flags) {
    if (IsWucltuxPathString(file)) {
        if (iconIndex == -1 || iconCount == 0) return 1;
        const int size = cxIcon > 0 ? cxIcon : (cyIcon > 0 ? cyIcon : GetSystemMetrics(SM_CXICON));
        UINT loaded = 0;
        if (icons) {
            icons[0] = LoadAppletLogoIconForShell(size);
            if (icons[0]) loaded = 1;
        }
        if (iconIds) iconIds[0] = 0;
        return loaded;
    }
    return PrivateExtractIconsWOriginal(file, iconIndex, cxIcon, cyIcon, icons,
                                        iconIds, iconCount, flags);
}

using SHDefExtractIconW_t = HRESULT(WINAPI*)(LPCWSTR, int, UINT, HICON*, HICON*, UINT);
static SHDefExtractIconW_t SHDefExtractIconWOriginal = nullptr;
static HRESULT WINAPI SHDefExtractIconWHook(LPCWSTR iconFile, int iconIndex,
                                            UINT flags, HICON* largeIcon,
                                            HICON* smallIcon, UINT iconSize) {
    if (IsWucltuxPathString(iconFile)) {
        const int largeSize = LOWORD(iconSize) ? LOWORD(iconSize) : GetSystemMetrics(SM_CXICON);
        const int smallSize = HIWORD(iconSize) ? HIWORD(iconSize) : GetSystemMetrics(SM_CXSMICON);
        bool loaded = false;
        if (largeIcon) {
            *largeIcon = LoadAppletLogoIconForShell(largeSize);
            loaded = loaded || *largeIcon;
        }
        if (smallIcon) {
            *smallIcon = LoadAppletLogoIconForShell(smallSize);
            loaded = loaded || *smallIcon;
        }
        return loaded ? S_OK : E_FAIL;
    }
    return SHDefExtractIconWOriginal(iconFile, iconIndex, flags, largeIcon,
                                     smallIcon, iconSize);
}

static void InstallShellPresentationHooks() {
    HMODULE shlwapi = GetModuleHandleW(L"shlwapi.dll");
    if (!shlwapi) shlwapi = LoadLibraryExW(L"shlwapi.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (shlwapi) {
        if (void* p = reinterpret_cast<void*>(GetProcAddress(shlwapi, "SHLoadIndirectString"))) {
            WindhawkUtils::SetFunctionHook(reinterpret_cast<SHLoadIndirectString_t>(p),
                                           SHLoadIndirectStringHook,
                                           &SHLoadIndirectStringOriginal);
        }
    }

    HMODULE shell32 = GetModuleHandleW(L"shell32.dll");
    if (!shell32) shell32 = LoadLibraryExW(L"shell32.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (shell32) {
        if (void* p = reinterpret_cast<void*>(GetProcAddress(shell32, "ExtractIconExW"))) {
            WindhawkUtils::SetFunctionHook(reinterpret_cast<ExtractIconExW_t>(p),
                                           ExtractIconExWHook,
                                           &ExtractIconExWOriginal);
        }
        if (void* p = reinterpret_cast<void*>(GetProcAddress(shell32, "SHDefExtractIconW"))) {
            WindhawkUtils::SetFunctionHook(reinterpret_cast<SHDefExtractIconW_t>(p),
                                           SHDefExtractIconWHook,
                                           &SHDefExtractIconWOriginal);
        }
    }

    HMODULE user32 = GetModuleHandleW(L"user32.dll");
    if (!user32) user32 = LoadLibraryExW(L"user32.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (user32) {
        if (void* p = reinterpret_cast<void*>(GetProcAddress(user32, "PrivateExtractIconsW"))) {
            WindhawkUtils::SetFunctionHook(reinterpret_cast<PrivateExtractIconsW_t>(p),
                                           PrivateExtractIconsWHook,
                                           &PrivateExtractIconsWOriginal);
        }
    }
}

static void InstallModernWuHooks() {
    HMODULE shell32 = GetModuleHandleW(L"shell32.dll");
    if (!shell32) {
        shell32 = LoadLibraryExW(L"shell32.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    }
    if (shell32) {
        if (void* p = reinterpret_cast<void*>(GetProcAddress(shell32, "ShellExecuteW"))) {
            WindhawkUtils::SetFunctionHook(reinterpret_cast<ShellExecuteW_t>(p),
                                           ShellExecuteWHook, &ShellExecuteWOriginal);
        }
    }
}


// -----------------------------------------------------------------------------
// In-memory Control Panel registration. This is the same conservative design as
// the Performance Information and Tools mod: no RegSetValue and no real CLSID.
// -----------------------------------------------------------------------------
static std::wstring ToLower(std::wstring text) {
    for (auto& c : text) c = towlower(c);
    return text;
}
static bool EndsWith(const std::wstring& value, const std::wstring& suffix) {
    return value.size() >= suffix.size() &&
           value.compare(value.size() - suffix.size(), suffix.size(), suffix) == 0;
}
static bool IsRootKey(HKEY key) {
    uintptr_t value = reinterpret_cast<uintptr_t>(key);
    return value >= 0x80000000 && value <= 0x80000004;
}
static std::wstring RootPath(HKEY key) {
    switch (reinterpret_cast<uintptr_t>(key)) {
        case 0x80000000: return L"HKEY_CLASSES_ROOT";
        case 0x80000001: return L"HKEY_CURRENT_USER";
        case 0x80000002: return L"HKEY_LOCAL_MACHINE";
        default: return {};
    }
}


class KeyTracker {

public:
    std::wstring Path(HKEY key) const {
        if (auto root = RootPath(key); !root.empty()) return root;
        std::shared_lock lock(mutex_);
        auto found = paths_.find(key);
        return found == paths_.end() ? std::wstring() : found->second;
    }
    bool IsFake(HKEY key) const {
        std::shared_lock lock(mutex_);
        return fake_.count(key) != 0;
    }
    HKEY CreateFake(const std::wstring& path) {
        auto owner = std::make_unique<int>(1);
        HKEY key = reinterpret_cast<HKEY>(owner.get());
        std::unique_lock lock(mutex_);
        paths_[key] = path;
        fake_[key] = std::move(owner);
        return key;
    }
    void Track(HKEY key, const std::wstring& path) {
        if (!key || IsRootKey(key)) return;
        std::unique_lock lock(mutex_);
        paths_[key] = path;
    }
    void Close(HKEY key) {
        std::unique_lock lock(mutex_);
        paths_.erase(key);
        fake_.erase(key);
    }
    void AbandonAll() {
        std::unique_lock lock(mutex_);
        paths_.clear();
        for (auto& item : fake_) {
            int* ptr = item.second.release();
            (void)ptr;
        }
        fake_.clear();
    }

private:
    mutable std::shared_mutex mutex_;
    std::unordered_map<HKEY, std::wstring> paths_;
    std::unordered_map<HKEY, std::unique_ptr<int>> fake_;
};
static KeyTracker g_keys;

// A namespace enumeration can ask for ERROR_NO_MORE_ITEMS more than once.
// Inject the virtual item once per enumeration pass only; otherwise callers that
// increment the index after the injected item would see it repeatedly.
static std::mutex g_injectionMutex;
static std::unordered_map<HKEY, bool> g_injected;
static bool ShouldInject(HKEY key, DWORD index) {
    std::lock_guard lock(g_injectionMutex);
    if (index == 0) g_injected[key] = false;
    bool& injected = g_injected[key];
    if (injected) return false;
    injected = true;
    return true;
}
static void ClearInjection(HKEY key) {
    std::lock_guard lock(g_injectionMutex);
    g_injected.erase(key);
}


static std::wstring g_clsidSuffix;
static std::wstring g_defaultIconSuffix;
static std::wstring g_inprocSuffix;
static std::wstring g_instanceSuffix;
static std::wstring g_propertyBagSuffix;
static std::wstring g_shellFolderSuffix;
static std::wstring g_namespaceSuffix;
static std::wstring g_providerSuffix;
static std::wstring g_providerInprocSuffix;

static void InitPaths() {
    const std::wstring clsid = ToLower(kAppletClsid);
    g_clsidSuffix = L"clsid\\" + clsid;
    g_defaultIconSuffix = g_clsidSuffix + L"\\defaulticon";
    g_inprocSuffix = g_clsidSuffix + L"\\inprocserver32";
    g_instanceSuffix = g_clsidSuffix + L"\\instance";
    g_propertyBagSuffix = g_instanceSuffix + L"\\initpropertybag";
    g_shellFolderSuffix = g_clsidSuffix + L"\\shellfolder";
    g_namespaceSuffix = L"controlpanel\\namespace\\" + clsid;
    g_providerSuffix = L"clsid\\" + ToLower(kElementProviderClsid);
    g_providerInprocSuffix = g_providerSuffix + L"\\inprocserver32";
}


enum class Node { None, Root, Icon, Inproc, Instance, Bag, ShellFolder, Namespace, Provider, ProviderInproc };
static Node Classify(const std::wstring& path) {

    const auto lower = ToLower(path);
    if (EndsWith(lower, g_namespaceSuffix)) return Node::Namespace;
    if (EndsWith(lower, g_propertyBagSuffix)) return Node::Bag;
    if (EndsWith(lower, g_instanceSuffix)) return Node::Instance;
    if (EndsWith(lower, g_shellFolderSuffix)) return Node::ShellFolder;
    if (EndsWith(lower, g_inprocSuffix)) return Node::Inproc;
    if (EndsWith(lower, g_defaultIconSuffix)) return Node::Icon;
    if (EndsWith(lower, g_clsidSuffix)) return Node::Root;
    if (EndsWith(lower, g_providerInprocSuffix)) return Node::ProviderInproc;
    if (EndsWith(lower, g_providerSuffix)) return Node::Provider;
    return Node::None;
}
static bool IsNamespaceParent(const std::wstring& path) {

    return EndsWith(ToLower(path), L"controlpanel\\namespace");
}
static bool IsTarget(const std::wstring& path) { return Classify(path) != Node::None; }


static LSTATUS PutString(LPBYTE data, LPDWORD bytes, const std::wstring& text) {

    if (!bytes) return ERROR_INVALID_PARAMETER;
    DWORD needed = static_cast<DWORD>((text.size() + 1) * sizeof(wchar_t));
    if (!data) { *bytes = needed; return ERROR_SUCCESS; }
    if (*bytes < needed) { *bytes = needed; return ERROR_MORE_DATA; }
    memcpy(data, text.c_str(), needed); *bytes = needed; return ERROR_SUCCESS;
}
static LSTATUS PutDword(LPBYTE data, LPDWORD bytes, DWORD value) {
    if (!bytes) return ERROR_INVALID_PARAMETER;
    if (!data) { *bytes = sizeof(value); return ERROR_SUCCESS; }
    if (*bytes < sizeof(value)) { *bytes = sizeof(value); return ERROR_MORE_DATA; }
    *reinterpret_cast<DWORD*>(data) = value; *bytes = sizeof(value); return ERROR_SUCCESS;
}
static std::wstring ShdocvwPath() {
    wchar_t system[MAX_PATH] = {};
    GetSystemDirectoryW(system, ARRAYSIZE(system));
    return std::wstring(system) + L"\\shdocvw.dll";
}

static std::mutex g_appletLogoIconMutex;

static bool FileHasExpectedSize(const std::wstring& path, DWORD expectedSize) {
    HANDLE file = CreateFileW(path.c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE,
                              nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (file == INVALID_HANDLE_VALUE) return false;
    LARGE_INTEGER size{};
    const bool ok = GetFileSizeEx(file, &size) && size.QuadPart == expectedSize;
    CloseHandle(file);
    return ok;
}

static bool WriteBinaryFile(const std::wstring& path, const std::vector<BYTE>& data) {
    HANDLE file = CreateFileW(path.c_str(), GENERIC_WRITE, FILE_SHARE_READ, nullptr,
                              CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (file == INVALID_HANDLE_VALUE) return false;
    DWORD written = 0;
    const bool ok = WriteFile(file, data.data(), static_cast<DWORD>(data.size()), &written, nullptr) &&
                    written == static_cast<DWORD>(data.size());
    CloseHandle(file);
    return ok;
}

static std::wstring EnsureAppletLogoIconFile(bool windows81Skin) {
    std::lock_guard<std::mutex> lock(g_appletLogoIconMutex);
    const std::wstring dir = StoreDir();
    if (dir.empty()) return L"";
    const wchar_t* fileName = windows81Skin ? kAppletLogoWin81FileName : kAppletLogoWin7FileName;
    const char* base64 = windows81Skin ? kAppletLogoWin81IcoBase64 : kAppletLogoWin7IcoBase64;
    std::vector<BYTE> data = DecodeBase64Icon(base64);
    if (data.empty()) return L"";
    const std::wstring path = dir + L"\\" + fileName;
    if (!FileHasExpectedSize(path, static_cast<DWORD>(data.size()))) {
        if (!WriteBinaryFile(path, data)) {
            Wh_Log(L"Windows Update Restorer: failed to write applet logo icon %s (err=%u)",
                   fileName, GetLastError());
            return L"";
        }
    }
    return path;
}

static std::wstring AppletDefaultIconValue(const std::wstring& fallbackPayload) {
    const std::wstring iconPath = EnsureAppletLogoIconFile(IsWindows81Skin());
    if (iconPath.empty()) return fallbackPayload + L",-1";
    return L"\"" + iconPath + L"\",0";
}

static void CleanupAppletLogoIconFiles() {
    const std::wstring dir = StoreDir();
    if (dir.empty()) return;
    DeleteFileW((dir + L"\\" + kAppletLogoWin7FileName).c_str());
    DeleteFileW((dir + L"\\" + kAppletLogoWin81FileName).c_str());
}


static bool WriteUtf8TextFile(const std::wstring& path, const std::wstring& text) {
    int bytesNeeded = WideCharToMultiByte(CP_UTF8, 0, text.c_str(), -1, nullptr, 0, nullptr, nullptr);
    if (bytesNeeded <= 1) return false;
    std::string utf8(static_cast<size_t>(bytesNeeded - 1), '\0');
    WideCharToMultiByte(CP_UTF8, 0, text.c_str(), -1, &utf8[0], bytesNeeded, nullptr, nullptr);

    HANDLE file = CreateFileW(path.c_str(), GENERIC_WRITE, FILE_SHARE_READ, nullptr,
                              CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (file == INVALID_HANDLE_VALUE) return false;
    DWORD written = 0;
    const bool ok = WriteFile(file, utf8.data(), static_cast<DWORD>(utf8.size()), &written, nullptr) &&
                    written == static_cast<DWORD>(utf8.size());
    CloseHandle(file);
    return ok;
}

static const wchar_t* TaskStringOrFallback(UINT id, const wchar_t* fallback) {
    if (const wchar_t* text = EmbeddedMuiString(id)) return text;
    return fallback ? fallback : L"";
}

static void AppendControlPanelTaskXml(std::wstring& xml, const wchar_t* id,
                                      const wchar_t* name, const wchar_t* keywords) {
    xml +=
        L"        <sh:task id=\"" + std::wstring(id) + L"\">\r\n"
        L"            <sh:name>" + XmlEscape(name) + L"</sh:name>\r\n"
        L"            <sh:keywords>" + XmlEscape(keywords) + L"</sh:keywords>\r\n"
        // All classic blue task links open the restored Windows Update page.
        // Use explorer shell:::{CLSID} explicitly, as requested, rather than
        // relying on canonical Control Panel task resolution.
        L"            <sh:command>%SystemRoot%\\explorer.exe shell:::" + std::wstring(kAppletClsid) + L"</sh:command>\r\n"
        L"        </sh:task>\r\n";
}

static std::wstring BuildControlPanelTasksXml() {
    // These are the classic Windows 7/8.1 blue task links shown under the
    // Control Panel item in category/search views. Labels come from the same
    // multilingual MUI table used by the page. Every link conservatively opens
    // our restored Windows Update Control Panel page.
    struct TaskDef { const wchar_t* id; UINT labelId; const wchar_t* fallback; const wchar_t* keywords; };
    static const TaskDef tasks[] = {
        { L"{72F890EA-C723-4B30-B990-69897A70E42D}", 350, L"Check for updates", L"windows update;check;updates;scan;" },
        { L"{0B11B6C6-4E9C-4E92-A3E4-1D31584546BA}", 351, L"Change settings", L"windows update;change settings;automatic updates;" },
        { L"{B3F9A7F2-C2FA-42B5-9D4A-473C66A3D4C0}", 352, L"View update history", L"windows update;history;installed updates;" },
        { L"{AC39E470-04F0-45C0-87D9-2B0C0B197350}", 353, L"Restore hidden updates", L"windows update;restore hidden updates;hidden;" },
        { L"{5D0F2DCB-6393-4E76-998B-2DA9A2F04AA1}", 20004, L"View Installed Updates", L"windows update;view installed updates;uninstall;" },
    };

    std::wstring xml =
        L"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\r\n"
        L"<applications xmlns=\"http://schemas.microsoft.com/windows/cpltasks/v1\"\r\n"
        L"              xmlns:sh=\"http://schemas.microsoft.com/windows/tasks/v1\">\r\n"
        L"    <application id=\"" + std::wstring(kAppletClsid) + L"\">\r\n";
    for (const auto& task : tasks) {
        AppendControlPanelTaskXml(xml, task.id,
                                  TaskStringOrFallback(task.labelId, task.fallback),
                                  task.keywords);
    }
    static const wchar_t* categories[] = { L"5", L"10" };
    for (const wchar_t* category : categories) {
        xml += L"        <category id=\"" + std::wstring(category) + L"\">\r\n";
        for (const auto& task : tasks) {
            xml += L"            <sh:task idref=\"" + std::wstring(task.id) + L"\"/>\r\n";
        }
        xml += L"        </category>\r\n";
    }
    xml +=
        L"    </application>\r\n"
        L"</applications>\r\n";
    return xml;
}

static std::wstring EnsureControlPanelTasksXmlFile() {
    const std::wstring dir = StoreDir();
    if (dir.empty()) return L"";
    const std::wstring path = dir + L"\\" + kAppletTasksXmlFileName;
    if (!WriteUtf8TextFile(path, BuildControlPanelTasksXml())) {
        Wh_Log(L"Windows Update Restorer: failed to write Control Panel task links XML (err=%u)",
               GetLastError());
        return L"";
    }
    return path;
}

static void CleanupControlPanelTasksXmlFile() {
    const std::wstring dir = StoreDir();
    if (dir.empty()) return;
    DeleteFileW((dir + L"\\" + kAppletTasksXmlFileName).c_str());
}



static bool ProvideValue(const std::wstring& path, const std::wstring& name,
                         LPDWORD type, LPBYTE data, LPDWORD bytes, LSTATUS& result) {
    if (!g_verified.load()) return false;
    const std::wstring* payload = g_dllPath.load();
    if (!payload || payload->empty()) return false;
    const Node node = Classify(path);
    std::wstring value;
    DWORD number = 0;
    bool isDword = false;
    switch (node) {
        case Node::Namespace:
            if (name.empty()) value = kDisplayName; else return false;
            break;
        case Node::Root:
            if (name.empty()) value = kDisplayName;
            else if (name == L"LocalizedString") value = L"@" + *payload + L",-1";
            else if (name == L"InfoTip") value = InfoTipForLanguage();
            else if (name == L"System.ApplicationName") value = kApplicationName;
            else if (name == L"System.ControlPanel.Category") value = L"5,10";
            else if (name == L"System.Software.TasksFileUrl") {
                value = EnsureControlPanelTasksXmlFile();
                if (value.empty()) return false;
            }
            else return false;
            break;
        case Node::Icon:
            if (!name.empty()) return false;
            value = AppletDefaultIconValue(*payload); break;
        case Node::Inproc:
            if (name.empty()) value = ShdocvwPath();
            else if (name == L"ThreadingModel") value = L"Apartment";
            else return false;
            break;
        case Node::Instance:
            if (name != L"CLSID") return false;
            value = kLayoutFolderClsid; break;
        case Node::Bag:
            if (name == L"ResourceDLL") value = *payload;
            else if (name == L"ResourceID") { isDword = true; number = kInitResourceId; }
            else return false;
            break;
        case Node::ShellFolder:
            if (name == L"Attributes") { isDword = true; number = kShellFolderAttributes; }
            else if (name == L"WantsParseDisplayName") value.clear();
            else return false;
            break;
        case Node::Provider:
            if (!name.empty()) return false;
            value.clear();
            break;
        case Node::ProviderInproc:
            if (name.empty()) value = *payload;
            else if (name == L"ThreadingModel") value = L"Apartment";
            else return false;
            break;
        default: return false;
    }
    if (type) *type = isDword ? REG_DWORD : REG_SZ;
    result = isDword ? PutDword(data, bytes, number) : PutString(data, bytes, value);
    return true;
}


using RegOpenKeyExW_t = decltype(&RegOpenKeyExW);
using RegOpenKeyW_t = decltype(&RegOpenKeyW);
using RegCloseKey_t = decltype(&RegCloseKey);
using RegQueryValueExW_t = decltype(&RegQueryValueExW);
using RegGetValueW_t = decltype(&RegGetValueW);
using RegEnumKeyExW_t = decltype(&RegEnumKeyExW);
using RegQueryInfoKeyW_t = decltype(&RegQueryInfoKeyW);
static RegOpenKeyExW_t RegOpenKeyExWOriginal;
static RegOpenKeyW_t RegOpenKeyWOriginal;
static RegCloseKey_t RegCloseKeyOriginal;
static RegQueryValueExW_t RegQueryValueExWOriginal;
static RegGetValueW_t RegGetValueWOriginal;
static RegEnumKeyExW_t RegEnumKeyExWOriginal;
static RegQueryInfoKeyW_t RegQueryInfoKeyWOriginal;


static bool WantsWrite(REGSAM access) {

    return (access & (KEY_SET_VALUE | KEY_CREATE_SUB_KEY | KEY_CREATE_LINK)) != 0;
}
static LSTATUS OpenVirtual(HKEY key, LPCWSTR subKey, DWORD options, REGSAM access, PHKEY out) {
    std::wstring base = g_keys.Path(key);
    std::wstring full = base;
    if (subKey && *subKey) { if (!full.empty()) full += L"\\"; full += subKey; }
    if (g_keys.IsFake(key)) {
        if (!IsTarget(full)) return ERROR_FILE_NOT_FOUND;
        if (WantsWrite(access)) return ERROR_ACCESS_DENIED;
        *out = g_keys.CreateFake(full); return ERROR_SUCCESS;
    }
    LSTATUS status = RegOpenKeyExWOriginal(key, subKey, options, access, out);
    if (status == ERROR_SUCCESS && out && *out) g_keys.Track(*out, full);
    else if (status == ERROR_FILE_NOT_FOUND && IsTarget(full)) {
        if (WantsWrite(access)) return ERROR_ACCESS_DENIED;
        *out = g_keys.CreateFake(full); return ERROR_SUCCESS;
    }
    return status;
}
static LSTATUS WINAPI RegOpenKeyExWHook(HKEY key, LPCWSTR subKey, DWORD options,
                                        REGSAM access, PHKEY out) {

    return OpenVirtual(key, subKey, options, access, out);
}
static LSTATUS WINAPI RegOpenKeyWHook(HKEY key, LPCWSTR subKey, PHKEY out) {

    return OpenVirtual(key, subKey, 0, MAXIMUM_ALLOWED, out);
}
static LSTATUS WINAPI RegCloseKeyHook(HKEY key) {

    if (g_keys.IsFake(key)) { g_keys.Close(key); ClearInjection(key); return ERROR_SUCCESS; }
    LSTATUS status = RegCloseKeyOriginal(key); g_keys.Close(key); ClearInjection(key); return status;
}
static LSTATUS WINAPI RegQueryValueExWHook(HKEY key, LPCWSTR valueName, LPDWORD reserved,
                                            LPDWORD type, LPBYTE data, LPDWORD bytes) {

    std::wstring path = g_keys.Path(key), name = valueName ? valueName : L"";
    LSTATUS result = ERROR_FILE_NOT_FOUND;
    if (!path.empty() && ProvideValue(path, name, type, data, bytes, result)) return result;
    if (g_keys.IsFake(key)) return ERROR_FILE_NOT_FOUND;
    return RegQueryValueExWOriginal(key, valueName, reserved, type, data, bytes);
}
static LSTATUS WINAPI RegGetValueWHook(HKEY key, LPCWSTR subKey, LPCWSTR valueName, DWORD flags,
                                       LPDWORD type, PVOID data, LPDWORD bytes) {

    std::wstring path = g_keys.Path(key);
    if (subKey && *subKey) { if (!path.empty()) path += L"\\"; path += subKey; }
    LSTATUS result = ERROR_FILE_NOT_FOUND;
    if (!path.empty() && ProvideValue(path, valueName ? valueName : L"", type,
                                      static_cast<LPBYTE>(data), bytes, result)) return result;
    if (g_keys.IsFake(key)) return ERROR_FILE_NOT_FOUND;
    return RegGetValueWOriginal(key, subKey, valueName, flags, type, data, bytes);
}


static bool VirtualSubkey(Node node, DWORD index, std::wstring& name) {
    if (node == Node::Root) {
        static const wchar_t* names[] = {L"DefaultIcon", L"InProcServer32", L"Instance", L"ShellFolder"};
        if (index < ARRAYSIZE(names)) { name = names[index]; return true; }
    } else if (node == Node::Instance && index == 0) { name = L"InitPropertyBag"; return true; }
    else if (node == Node::Provider && index == 0) { name = L"InProcServer32"; return true; }
    return false;
}
static LSTATUS WINAPI RegEnumKeyExWHook(HKEY key, DWORD index, LPWSTR name, LPDWORD chars,
                                        LPDWORD reserved, LPWSTR cls, LPDWORD classChars,
                                        PFILETIME time) {
    if (g_keys.IsFake(key)) {
        std::wstring sub;
        if (!VirtualSubkey(Classify(g_keys.Path(key)), index, sub)) return ERROR_NO_MORE_ITEMS;
        if (!name || !chars) return ERROR_INVALID_PARAMETER;
        if (*chars <= sub.size()) { *chars = static_cast<DWORD>(sub.size() + 1); return ERROR_MORE_DATA; }
        wcscpy_s(name, *chars, sub.c_str()); *chars = static_cast<DWORD>(sub.size());
        if (time) GetSystemTimeAsFileTime(time); return ERROR_SUCCESS;
    }
    LSTATUS status = RegEnumKeyExWOriginal(key, index, name, chars, reserved, cls, classChars, time);
    if (status != ERROR_NO_MORE_ITEMS || !IsNamespaceParent(g_keys.Path(key)) || !g_verified.load()) return status;
    // Validate the output buffer BEFORE latching the injection flag. If the
    // caller's buffer is too small we return ERROR_MORE_DATA and it retries at
    // the same index; consuming the flag here would make that retry return
    // ERROR_NO_MORE_ITEMS and the injected applet would silently not appear.
    if (!name || !chars) return ERROR_INVALID_PARAMETER;
    if (*chars <= wcslen(kAppletClsid)) { *chars = static_cast<DWORD>(wcslen(kAppletClsid) + 1); return ERROR_MORE_DATA; }
    if (!ShouldInject(key, index)) return ERROR_NO_MORE_ITEMS;
    wcscpy_s(name, *chars, kAppletClsid); *chars = static_cast<DWORD>(wcslen(kAppletClsid));
    if (time) GetSystemTimeAsFileTime(time); return ERROR_SUCCESS;
}
static LSTATUS WINAPI RegQueryInfoKeyWHook(HKEY key, LPWSTR cls, LPDWORD classChars, LPDWORD reserved,
                                           LPDWORD subKeys, LPDWORD maxSubKey, LPDWORD maxClass,
                                           LPDWORD values, LPDWORD maxValueName, LPDWORD maxValueData,
                                           LPDWORD security, PFILETIME time) {

    if (g_keys.IsFake(key)) {
        Node node = Classify(g_keys.Path(key));
        if (subKeys) *subKeys = node == Node::Root ? 4 :
                                (node == Node::Instance || node == Node::Provider) ? 1 : 0;
        if (values) *values = 8; // Sizing hint; values are supplied on query.
        if (maxSubKey) *maxSubKey = 32; if (maxClass) *maxClass = 0;
        if (maxValueName) *maxValueName = 40; if (maxValueData) *maxValueData = 1024;
        if (cls && classChars && *classChars) cls[0] = 0;
        if (classChars) *classChars = 0; if (time) GetSystemTimeAsFileTime(time);
        return ERROR_SUCCESS;
    }
    LSTATUS status = RegQueryInfoKeyWOriginal(key, cls, classChars, reserved, subKeys, maxSubKey,
                                              maxClass, values, maxValueName, maxValueData, security, time);
    if (status == ERROR_SUCCESS && IsNamespaceParent(g_keys.Path(key)) && g_verified.load() && subKeys) {
        ++*subKeys;
        if (maxSubKey && *maxSubKey < wcslen(kAppletClsid)) *maxSubKey = static_cast<DWORD>(wcslen(kAppletClsid));
    }
    return status;
}


// shdocvw.dll implements the standard layout-folder class used by old CPL items.
using CoCreateInstance_t = HRESULT(WINAPI*)(REFCLSID, LPUNKNOWN, DWORD, REFIID, LPVOID*);
static CoCreateInstance_t CoCreateInstanceOriginal = nullptr;
// Defined later (before SetupWorker); declared here so the CoCreateInstance
// hook can lazily load wucltux.dll if the page is constructed in a process that
// skipped the eager heavy setup (e.g. a shell/explorer.exe fallback).
static HMODULE EnsurePrivateModuleLoaded();
static HRESULT WINAPI CoCreateInstanceHook(REFCLSID clsid, LPUNKNOWN outer, DWORD context,
                                           REFIID iid, LPVOID* result) {
    if (!IsEqualGUID(clsid, kAppletFolderGuid) && !IsEqualGUID(clsid, kElementProviderGuid))
        return CoCreateInstanceOriginal(clsid, outer, context, iid, result);
    const bool isElementProvider = IsEqualGUID(clsid, kElementProviderGuid);
    if (!g_verified.load()) {
        if (isElementProvider)
            Wh_Log(L"Windows Update Restorer: WUAppElementProvider requested before wucltux.dll was verified/ready");
        return REGDB_E_CLASSNOTREG;
    }


    // The namespace folder itself comes from shdocvw. The XMLFILE resource in
    // wucltux.dll then creates cfbc05bc-... (WUAppElementProvider), which must
    // come from the private legacy module.
    HMODULE server = nullptr;
    if (IsEqualGUID(clsid, kAppletFolderGuid)) {
        server = GetModuleHandleW(L"shdocvw.dll");
        if (!server) server = LoadLibraryExW(L"shdocvw.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    } else {
        // Defensive fallback: normally SetupWorker has already loaded
        // wucltux.dll into g_module. If for any reason it has not (e.g. the
        // element provider is requested before the background setup finished),
        // load it on demand so the page can still be constructed.
        server = g_module.load();
        if (!server) server = EnsurePrivateModuleLoaded();
    }
    if (!server) {
        if (isElementProvider)
            Wh_Log(L"Windows Update Restorer: WUAppElementProvider requested but private wucltux.dll module is not loaded");
        return REGDB_E_CLASSNOTREG;
    }
    auto getClassObject = reinterpret_cast<HRESULT(WINAPI*)(REFCLSID, REFIID, LPVOID*)>(
        GetProcAddress(server, "DllGetClassObject"));
    if (!getClassObject) {
        if (isElementProvider)
            Wh_Log(L"Windows Update Restorer: WUAppElementProvider requested but DllGetClassObject export was not found in wucltux.dll");
        return REGDB_E_CLASSNOTREG;
    }
    IClassFactory* factory = nullptr;
    HRESULT hr = getClassObject(clsid, IID_IClassFactory_GUID, reinterpret_cast<void**>(&factory));
    if (FAILED(hr)) {
        if (isElementProvider)
            Wh_Log(L"Windows Update Restorer: WUAppElementProvider DllGetClassObject failed (hr=0x%08X)",
                   static_cast<unsigned>(hr));
        return hr;
    }
    hr = factory->CreateInstance(outer, iid, result);
    if (isElementProvider) {
        if (FAILED(hr)) {
            Wh_Log(L"Windows Update Restorer: WUAppElementProvider CreateInstance failed (hr=0x%08X)",
                   static_cast<unsigned>(hr));
        } else {
            Wh_Log(L"Windows Update Restorer: WUAppElementProvider created successfully (hr=0x%08X)",
                   static_cast<unsigned>(hr));
        }
    }
    factory->Release();
    return hr;
}


static void* RegistryFunction(const char* name) {
    HMODULE module = GetModuleHandleW(L"kernelbase.dll");
    void* function = module ? reinterpret_cast<void*>(GetProcAddress(module, name)) : nullptr;
    if (!function) {
        module = GetModuleHandleW(L"advapi32.dll");
        if (!module) module = LoadLibraryW(L"advapi32.dll");
        if (module) function = reinterpret_cast<void*>(GetProcAddress(module, name));
    }
    return function;
}


// Loads the private wucltux.dll module on demand from g_dllPath. Used as a
// defensive fallback from the CoCreateInstance hook: normally SetupWorker loads
// the module eagerly into g_module; if the element provider is requested before
// that finishes, this loads it right away so the page can still be constructed.
// Fast (a plain module load), so it is safe to call from the CoCreateInstance
// hook. Returns the module or nullptr.
static HMODULE EnsurePrivateModuleLoaded() {
    if (HMODULE m = g_module.load()) return m;
    const std::wstring* path = g_dllPath.load();
    if (!path || path->empty()) return nullptr;
    HMODULE module = LoadLibraryExW(path->c_str(), nullptr, 0);
    if (!module) {
        Wh_Log(L"Windows Update Restorer: lazy LoadLibraryEx of wucltux failed (%u)", GetLastError());
        return nullptr;
    }
    HMODULE expected = nullptr;
    if (g_module.compare_exchange_strong(expected, module)) return module;
    FreeLibrary(module);  // another thread won the race; do not leak ours
    return g_module.load();
}

static void SetupWorker() {
    std::wstring path;
    if (!EnsurePayload(path) || g_stopping.load()) {
        Wh_Log(L"Windows Update Restorer: wucltux.dll was not downloaded or failed verification");
        return;
    }
    // The page's XMLFILE creates WUAppElementProvider through DllGetClassObject,
    // so this must be an executable module load, not LOAD_LIBRARY_AS_DATAFILE.
    // It remains a private copy outside System32.
    HMODULE module = LoadLibraryExW(path.c_str(), nullptr, 0);
    if (!module) {
        Wh_Log(L"Windows Update Restorer: LoadLibraryEx failed (%u)", GetLastError());
        return;
    }
    g_module.store(module);
    g_dllPath.store(new std::wstring(path));
    if (!BuildEmbeddedMuiResourceModule(path)) {
        Wh_Log(L"Windows Update Restorer: embedded MUI resource module could not be built");
    } else {
        g_builtLanguage = g_language;
    }

    g_verified.store(true, std::memory_order_release);
    Wh_Log(L"Windows Update Restorer ready: verified Windows 8.1 wucltux.dll loaded privately");
}


BOOL Wh_ModInit() {
    try {
        LoadLanguageSetting();
        InitPaths();
        void* openEx = RegistryFunction("RegOpenKeyExW");
        void* open = RegistryFunction("RegOpenKeyW");
        void* close = RegistryFunction("RegCloseKey");
        void* query = RegistryFunction("RegQueryValueExW");
        void* get = RegistryFunction("RegGetValueW");
        void* enumerate = RegistryFunction("RegEnumKeyExW");
        void* info = RegistryFunction("RegQueryInfoKeyW");
        if (!openEx || !open || !close || !query || !get || !enumerate || !info) return FALSE;
        WindhawkUtils::SetFunctionHook(reinterpret_cast<RegOpenKeyExW_t>(openEx), RegOpenKeyExWHook, &RegOpenKeyExWOriginal);
        WindhawkUtils::SetFunctionHook(reinterpret_cast<RegOpenKeyW_t>(open), RegOpenKeyWHook, &RegOpenKeyWOriginal);
        WindhawkUtils::SetFunctionHook(reinterpret_cast<RegCloseKey_t>(close), RegCloseKeyHook, &RegCloseKeyOriginal);
        WindhawkUtils::SetFunctionHook(reinterpret_cast<RegQueryValueExW_t>(query), RegQueryValueExWHook, &RegQueryValueExWOriginal);
        WindhawkUtils::SetFunctionHook(reinterpret_cast<RegGetValueW_t>(get), RegGetValueWHook, &RegGetValueWOriginal);
        WindhawkUtils::SetFunctionHook(reinterpret_cast<RegEnumKeyExW_t>(enumerate), RegEnumKeyExWHook, &RegEnumKeyExWOriginal);
        WindhawkUtils::SetFunctionHook(reinterpret_cast<RegQueryInfoKeyW_t>(info), RegQueryInfoKeyWHook, &RegQueryInfoKeyWOriginal);


        HMODULE combase = GetModuleHandleW(L"combase.dll");
        if (!combase) combase = LoadLibraryExW(L"combase.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
        if (combase) {
            if (void* create = reinterpret_cast<void*>(GetProcAddress(combase, "CoCreateInstance")))
                WindhawkUtils::SetFunctionHook(reinterpret_cast<CoCreateInstance_t>(create), CoCreateInstanceHook, &CoCreateInstanceOriginal);

        }
        // wucltux.dll imports LoadStringW through the normal User32 API. Its
        // original MUI is embedded above, so no external .mui file is needed.
        HMODULE user32 = GetModuleHandleW(L"user32.dll");
        if (!user32) user32 = LoadLibraryExW(L"user32.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
        if (user32) {
            if (void* loadString = reinterpret_cast<void*>(GetProcAddress(user32, "LoadStringW")))
                WindhawkUtils::SetFunctionHook(reinterpret_cast<LoadStringW_t>(loadString),
                                               LoadStringWHook, &LoadStringWOriginal);
        }


        // DirectUI's resstr(...) goes through XResourceProvider rather than
        // LoadStringW. Redirect it to the private DLL carrying the embedded MUI.
        HMODULE dui70 = GetModuleHandleW(L"dui70.dll");
        if (!dui70) dui70 = LoadLibraryExW(L"dui70.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
        if (dui70) {
            void* providerCreate = reinterpret_cast<void*>(GetProcAddress(
                dui70, "?Create@XResourceProvider@DirectUI@@SAJPEAUHINSTANCE__@@PEBG11PEAPEAV12@@Z"));
            if (providerCreate) {
                WindhawkUtils::SetFunctionHook(
                    reinterpret_cast<XResourceProviderCreate_t>(providerCreate),
                    XResourceProviderCreateHook, &XResourceProviderCreateOriginal);
            } else {

                Wh_Log(L"Windows Update Restorer: XResourceProvider::Create was not found");
            }
        }

        // The requested actions live in the host Control Panel navigation pane.
        // Prepare shutdown signalling before either worker starts.
        g_stopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
        g_stopping.store(false);
        // Only patch the safe WUAppPage content anchor (never the outer Control Panel host).
        InstallModernWuXmlPatchHook();
        InstallModernWuHooks();
        InstallShellPresentationHooks();
        InstallLegacyWarningIconHook();
        g_setupThread.emplace(SetupWorker);
        return TRUE;
    } catch (...) { return FALSE; }
}

// Called whenever the user changes the mod settings (e.g. picks a different
// language). We just reload the flag; the next DirectUI page render uses it, so
// no full mod restart is needed.
// Called whenever the user changes the mod settings (e.g. picks a different
// language). We reload the flag and, if the language actually changed, rebuild
// the embedded MUI module in the background so the classic page reflects the
// new language without a full mod restart.
void Wh_ModSettingsChanged() {
    const std::wstring oldLanguage = g_language;
    LoadLanguageSetting();
    if (oldLanguage != g_language) {
        // Ensure a previous rebuild (if any) has finished before starting a new
        // one, so we never run two builds concurrently.
        if (g_rebuildThread && g_rebuildThread->joinable()) g_rebuildThread->join();
        g_rebuildThread.reset();
        g_rebuildThread.emplace([] { RebuildEmbeddedMuiForLanguage(); });
    }
}


// Best-effort cleanup of leftover embedded-mui generation files from this
// process (see BuildEmbeddedMuiResourceModule). We deliberately never call
// FreeLibrary on g_resourceModule/g_module while the mod is loaded, since a
// Control Panel page may still hold a reference into them - so on disable we
// can only try to delete the *files*. A copy still mapped by an open page
// simply fails to delete here (harmless) and Windows will let it go once the
// last handle to it closes, at the latest when explorer.exe restarts.
// Deletes every embedded-mui resource file this process has created, including
// the currently active one. Called when the mod is disabled so no stale or
// partially-written .mres is left behind that a later start could reuse as if
// it were valid (which is what causes "corrupted" strings). The module is
// already mapped into memory at this point, so deleting the file on disk is
// safe: Windows keeps the mapping alive until the last reference to it closes.
static void CleanupGeneratedResourceModuleFiles() {
    const std::wstring dir = StoreDir();
    if (dir.empty()) return;
    const std::wstring prefix =
        L"wucltux.embedded-mui-" + std::to_wstring(GetCurrentProcessId()) + L"-";
    const std::wstring pattern = dir + L"\\" + prefix + L"*";

    WIN32_FIND_DATAW findData{};
    HANDLE find = FindFirstFileW(pattern.c_str(), &findData);
    if (find == INVALID_HANDLE_VALUE) return;
    do {
        if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) continue;
        const std::wstring fullPath = dir + L"\\" + findData.cFileName;
        if (DeleteFileW(fullPath.c_str())) {
            Wh_Log(L"Windows Update Restorer: deleted embedded MUI resource file: %s",
                   findData.cFileName);
        } else if (GetLastError() != ERROR_FILE_NOT_FOUND) {
            // The file is still mapped by an open page; Windows will remove it
            // once the last handle to it closes (harmless to leave for now).
            Wh_Log(L"Windows Update Restorer: resource file in use, removal deferred: %s (err=%u)",
                   findData.cFileName, GetLastError());
        }
    } while (FindNextFileW(find, &findData));
    FindClose(find);
}

void Wh_ModUninit() {
    g_stopping.store(true);
    if (g_stopEvent) SetEvent(g_stopEvent);
    if (g_setupThread && g_setupThread->joinable()) g_setupThread->join();
    g_setupThread.reset();
    if (g_rebuildThread && g_rebuildThread->joinable()) g_rebuildThread->join();
    g_rebuildThread.reset();
    if (g_stopEvent) { CloseHandle(g_stopEvent); g_stopEvent = nullptr; }
    g_verified.store(false);
    delete g_dllPath.exchange(nullptr);
    // Deliberately do not unload the datafile while a Control Panel page can cache it.
    g_module.store(nullptr);
    {
        std::lock_guard<std::mutex> lock(g_resourceMutex);
        CleanupGeneratedResourceModuleFiles();
    }
    CleanupAppletLogoIconFiles();
    CleanupControlPanelTasksXmlFile();
    if (g_updatesInstalledIcon) { DestroyIcon(g_updatesInstalledIcon); g_updatesInstalledIcon = nullptr; }
    if (g_windows81UpdateStatusIcon) { DestroyIcon(g_windows81UpdateStatusIcon); g_windows81UpdateStatusIcon = nullptr; }
    ShutdownGdiPlusRendering();
    g_keys.AbandonAll();
    { std::lock_guard lock(g_injectionMutex); g_injected.clear(); }
}
