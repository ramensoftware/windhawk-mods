// ==WindhawkMod==
// @id              windows-update-control-panel-restorer
// @name            Windows Update Control Panel Page Restorer
// @description     This mod restores the Windows Update Control Panel page in Windows 10 and Windows 11
// @version         1.0.0
// @author          babamohammed
// @github          https://github.com/babamohammed2022
// @include         explorer.exe
// @include         control.exe
// @architecture    x86-64
// @compilerOptions -lwininet -ladvapi32 -lcrypt32 -lole32 -luuid -loleaut32 -lgdi32 -lcomctl32 -luser32 -lshell32
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
  $description: This setting shows the "updates available" state (amber/orange strip with an exclamation shield) when Windows reports pending available updates. Enabled by default.
- LinkSystemSettingsText: false
  $name: Link system settings text
  $description: This setting makes the "system settings" part of the recommendation text a blue link that opens Windows Update in the Settings app (ms-settings:windowsupdate). Disabled by default.
- RemoveLegacyBrokenOption: true
  $name: Remove Legacy Broken Option Fix
  $description: When Windows Update is unavailable or disabled, the restored page shows a legacy red "Check for updates for your PC" box whose button cannot work (the service is stopped). With this enabled, that broken legacy box is removed so only the "Turn on automatic updating" box remains (plus the blue settings link when the recreated interface is shown). Disable to keep the legacy box.
*/
// ==/WindhawkModSettings==

// ==WindhawkModReadme==
/*
## Windows Update Control Panel Restorer

This mod adds a best-effort classic Windows Update page back to Control Panel on
Windows 10 and Windows 11. It uses a private Windows 8.1 UI payload with a modern
Windows Update backend/status layer, without replacing system files or writing
real Control Panel registration keys.

This is a reimplementation, not the original Windows Update client. Some buttons
and small visual details are intentionally limited, and more details may be
improved in future versions.

The mod has been tested on Windows 10 21H2, Windows 11 24H2 and Windows 11 25H2.

## **Screenshot**

![Windows Update Control Panel Restorer](https://raw.githubusercontent.com/babamohammed2022/babamohammed2022/main/winupdate.PNG)

## **Features**

- Classic Windows Update-style status page with real restart/update status.
- Windows 7 or Windows 8.1 skin option for the status/app icon.
- Friendly notice when Windows Update is disabled or unavailable.
- Optional available-updates banner and optional link to Windows Update Settings.
- Optional "Remove Legacy Broken Option Fix" that hides the broken legacy
  "Check for updates for your PC" box when Windows Update is unavailable, so
  only the "Turn on automatic updating" box (and the settings link) remains.
- Restored classic "Important updates" selector on the "Change settings" page:
  the four classic options (install automatically / download / check / never)
  are shown in every supported language, the current AUOptions value is read
  from the registry and selecting an option writes it back.
- Multilingual UI: English, Italian, Spanish, French, Turkish, Russian,
  Portuguese, Chinese, Polish, Dutch, or auto-detect.

## **Notes**

- Installing updates is still handled by the modern Settings app.
- The "last checked" time is the moment this mod queried the system, so it can
  differ slightly from the Settings app.
- **Why the mod downloads a DLL:** The restored page is the real Windows 8.1
  Windows Update Control Panel UI (wucltux.dll), loaded privately from a
  verified copy obtained from the Microsoft Symbol Server. That DLL is a
  **necessary dependency**: it is what renders the classic page, and the whole
  point of this mod is to bring that original page back. Without it there is
  nothing to show, so the mod cannot function offline or without this payload.
  The download is a one-time fetch (retried up to a few times), the file is
  pinned to a known SHA-256 and its PE machine type is validated before it is
  ever loaded, and it is kept as a private copy outside System32 and nothing is
  installed to or replaced in the operating system.

**Credits**

- **Yvor** - Testing on Windows 10 21H2.
- **Cips** - Testing on Windows 11 25H2.

If you encounter issues, please report them to the author of the mod.
*/
// ==/WindhawkModReadme==

#include <windows.h>
#ifdef max
#undef max
#endif
#ifdef min
#undef min
#endif
#include <windowsx.h>  // GET_Y_LPARAM
#include <wininet.h>
#include <wincrypt.h>
#include <combaseapi.h>
#include <winnls.h>
#include <winsvc.h>
#include <shlobj.h>
#include <shellapi.h>
#include <commctrl.h>
#include <objidl.h>
#include <oaidl.h>
#include <oleauto.h>
#include <cstdlib>
#include <cstring>
#include <string>
#include <unordered_map>
#include <memory>
#include <new>
#include <mutex>
#include <shared_mutex>
#include <thread>
#include <atomic>
#include <algorithm>
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
[[maybe_unused]] static void OpenInstalledUpdates(HWND hwnd) {
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
[[maybe_unused]] static void ShowUpdateHistory(HWND hwnd) {
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

// Whether to remove the broken legacy "Check for updates for your PC" red box
// (moduleCheckForUpdates) when Windows Update is unavailable/disabled and only
// the "Turn on automatic updating" box is shown. With the service stopped that
// legacy box's button cannot work, so it is removed by default. Controlled by
// the "RemoveLegacyBrokenOption" setting ("Remove Legacy Broken Option Fix").
static std::atomic<bool> g_removeLegacyBrokenOption{true};

// Debug/preview flag: when set, the restored page renders the "pending
// updates" interface (orange strip, "Pending restart", shield icon) even when
// Windows reports no pending update. This is a developer-only diagnostic switch.
// It is intentionally compile-time only: shipped builds do not expose a setting
// which suggests that a diagnostic state can be enabled at runtime.
static constexpr bool kWuDebugForcePendingEnabled = false;
static std::atomic<bool> g_debugForcePending{false};

// Currently selected language code (default "en"). Declared early because the
// embedded string table below resolves strings per language at runtime. Loaded
// from the mod settings in LoadLanguageSetting().
// Published as an index: UI hooks may run on several Explorer threads. Never
// share a mutable std::wstring with them.
enum class Language : int { en, it, es, fr, tr, ru, pt, zh, pl, nl };
static std::atomic<Language> g_language{Language::en};
static Language LanguageFromCode(const std::wstring& value) {
    if (value == L"it") return Language::it; if (value == L"es") return Language::es;
    if (value == L"fr") return Language::fr; if (value == L"tr") return Language::tr;
    if (value == L"ru") return Language::ru; if (value == L"pt") return Language::pt;
    if (value == L"zh") return Language::zh; if (value == L"pl") return Language::pl;
    if (value == L"nl") return Language::nl; return Language::en;
}
static const wchar_t* LanguageCode() {
    static constexpr const wchar_t* kCodes[] = { L"en", L"it", L"es", L"fr", L"tr", L"ru", L"pt", L"zh", L"pl", L"nl" };
    return kCodes[static_cast<int>(g_language.load(std::memory_order_acquire))];
}
static std::wstring CurrentLanguage() { return LanguageCode(); }
static bool LanguageIs(PCWSTR code) { return wcscmp(LanguageCode(), code) == 0; }

// Whether to show the mod's "service not available" notice (the shield box).
// Controlled by the "ShowServiceNotice" setting (default on).
static std::atomic<bool> g_showServiceNotice{true};

// Cached "last check" timestamp: the moment the mod last queried the system for
// available updates (see LastCheckForUpdatesText). Kept as a formatted string.
static std::wstring g_lastQueryTimeText;
static std::mutex g_lastQueryTimeMutex;

// Cached, background-gathered status values so the Control Panel UI thread never
// has to do blocking SCM/WUA work during page rendering (see GatherBackgroundStatus).
// They are computed once on the setup thread and read from the render path.
static std::atomic<bool> g_cachedWuAvailable{false};
static std::atomic<bool> g_cachedWuServiceProbed{false};
static std::mutex g_statusMutex;
static std::wstring g_cachedLastInstall;
static bool g_lastInstallComputed = false;

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
    { 334, L"Install updates automatically (recommended)", L"Installa gli aggiornamenti automaticamente (scelta consigliata)", L"Instalar actualizaciones automáticamente (recomendado)", L"Installer automatiquement les mises à jour (recommandé)", L"Güncellemeleri otomatik olarak yükle (önerilir)", L"Автоматически устанавливать обновления (рекомендуется)", L"Instalar atualizações automaticamente (recomendado)", L"自动安装更新（推荐）", L"Automatycznie instaluj aktualizacje (zalecane)", L"Updates automatisch installeren (aanbevolen)" },
    { 335, L"Download updates but let me choose whether to install them", L"Scarica gli aggiornamenti ma consenti di scegliere se installarli", L"Descargar actualizaciones pero permitirme elegir si instalarlas", L"Télécharger les mises à jour mais me laisser choisir de les installer", L"Güncellemeleri indir, ancak bunları yükleyip yüklemeyeceğimi ben seçeyim", L"Загружать обновления, но я сам решу, устанавливать ли их", L"Baixar atualizações, mas deixar que eu escolha se desejo instalá-las", L"下载更新，但让我选择是否安装", L"Pobieraj aktualizacje, ale pozwól mi wybrać, czy je zainstalować", L"Updates downloaden, maar mij laten kiezen of ik ze wil installeren" },
    { 336, L"Check for updates but let me choose whether to download and install them", L"Verifica la disponibilità di aggiornamenti ma consenti di scegliere se scaricarli e installarli", L"Comprobar actualizaciones pero permitirme elegir si descargarlas e instalarlas", L"Rechercher les mises à jour mais me laisser choisir de les télécharger et de les installer", L"Güncellemeleri denetle, ancak bunları indirip yükleyip yüklemeyeceğimi ben seçeyim", L"Проверять обновления, но я сам решу, загружать и устанавливать ли их", L"Verificar atualizações, mas deixar que eu escolha se desejo baixá-las e instalá-las", L"检查更新，但让我选择是否下载和安装", L"Sprawdzaj aktualizacje, ale pozwól mi wybrać, czy je pobrać i zainstalować", L"Controleren op updates, maar mij laten kiezen of ik ze wil downloaden en installeren" },
    { 337, L"Never check for updates (not recommended)", L"Non verificare mai la disponibilità di aggiornamenti (scelta sconsigliata)", L"No comprobar nunca las actualizaciones (no recomendado)", L"Ne jamais rechercher les mises à jour (non recommandé)", L"Güncellemeleri hiç denetleme (önerilmez)", L"Никогда не проверять обновления (не рекомендуется)", L"Nunca verificar atualizações (não recomendado)", L"从不检查更新（不推荐）", L"Nigdy nie sprawdzaj aktualizacji (niezalecane)", L"Nooit naar updates zoeken (niet aanbevolen)" },
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
    { 1272, L"", L"", L"", L"", L"", L"", L"", L"", L"", L"" },
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
    { 1190, L"Microsoft Update", L"Microsoft Update", L"Microsoft Update", L"Microsoft Update", L"Microsoft Update", L"Центр обновления Microsoft", L"Microsoft Update", L"Microsoft 更新", L"Microsoft Update", L"Microsoft Update" },
    { 1191, L"Give me updates for other Microsoft products when I update Windows", L"Dammi aggiornamenti per altri prodotti Microsoft quando aggiorno Windows", L"Darme actualizaciones para otros productos de Microsoft cuando actualizo Windows", L"Me donner les mises à jour pour d'autres produits Microsoft quand je mets à jour Windows", L"Windows'u güncellediğimde diğer Microsoft ürünleri için güncellemeler ver", L"Предоставлять обновления для других продуктов Microsoft при обновлении Windows", L"Dar-me atualizações para outros produtos da Microsoft ao atualizar o Windows", L"更新 Windows 时为我提供其他 Microsoft 产品的更新", L"Daj mi aktualizacje dla innych produktów Microsoft, gdy aktualizuję Windows", L"Geef mij updates voor andere Microsoft-producten wanneer ik Windows bijwerk" },
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


// WinINet synchronous calls do not observe g_stopping until they return. Keep the
// active handles published so teardown can close them and break a blocked read.
static std::mutex g_downloadMutex;
static HINTERNET g_downloadInternet = nullptr;
static HINTERNET g_downloadUrl = nullptr;

static void CloseActiveDownloadHandles() {
    HINTERNET url = nullptr, internet = nullptr;
    {
        std::lock_guard<std::mutex> lock(g_downloadMutex);
        url = g_downloadUrl; g_downloadUrl = nullptr;
        internet = g_downloadInternet; g_downloadInternet = nullptr;
    }
    if (url) InternetCloseHandle(url);
    if (internet) InternetCloseHandle(internet);
}

// Removes handles only if this worker still owns them. If teardown already took
// them, it is solely responsible for closing them, avoiding a double-close race.
static void CloseOwnedDownloadHandles(HINTERNET url, HINTERNET internet) {
    bool closeUrl = false, closeInternet = false;
    {
        std::lock_guard<std::mutex> lock(g_downloadMutex);
        if (g_downloadUrl == url) { g_downloadUrl = nullptr; closeUrl = true; }
        if (g_downloadInternet == internet) { g_downloadInternet = nullptr; closeInternet = true; }
    }
    if (closeUrl && url) InternetCloseHandle(url);
    if (closeInternet && internet) InternetCloseHandle(internet);
}

static bool DownloadWithTimeout(const std::wstring& destination) {
    HINTERNET internet = InternetOpenW(L"Windhawk Windows Update Restorer",
                                       INTERNET_OPEN_TYPE_PRECONFIG, nullptr, nullptr, 0);
    if (!internet) return false;
    {
        std::lock_guard<std::mutex> lock(g_downloadMutex);
        if (g_stopping.load()) { InternetCloseHandle(internet); return false; }
        g_downloadInternet = internet;
    }
    DWORD timeout = kDownloadTimeoutMs;
    InternetSetOptionW(internet, INTERNET_OPTION_CONNECT_TIMEOUT, &timeout, sizeof(timeout));
    InternetSetOptionW(internet, INTERNET_OPTION_SEND_TIMEOUT, &timeout, sizeof(timeout));
    InternetSetOptionW(internet, INTERNET_OPTION_RECEIVE_TIMEOUT, &timeout, sizeof(timeout));
    HINTERNET url = InternetOpenUrlW(internet, kDownloadUrl, nullptr, 0,
                                     INTERNET_FLAG_RELOAD | INTERNET_FLAG_NO_CACHE_WRITE |
                                         INTERNET_FLAG_NO_UI,
                                     0);
    if (!url) { CloseOwnedDownloadHandles(nullptr, internet); return false; }
    {
        std::lock_guard<std::mutex> lock(g_downloadMutex);
        if (g_stopping.load()) {
            // The stop path either owns the parent already or will take both.
            // Publish the child before returning so it can abort it as well.
            g_downloadUrl = url;
        } else {
            g_downloadUrl = url;
        }
    }
    if (g_stopping.load()) { CloseOwnedDownloadHandles(url, internet); return false; }
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
    CloseOwnedDownloadHandles(url, internet);
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
        if (LanguageIs(L"it")) return item->it;
        if (LanguageIs(L"es")) return item->es;
        if (LanguageIs(L"fr")) return item->fr;
        if (LanguageIs(L"tr")) return item->tr;
        if (LanguageIs(L"ru")) return item->ru;
        if (LanguageIs(L"pt")) return item->pt;
        if (LanguageIs(L"zh")) return item->zh;
        if (LanguageIs(L"pl")) return item->pl;
        if (LanguageIs(L"nl")) return item->nl;
        return item->en; // default / fallback
    }
    return nullptr;
}

// Returns the translated Control Panel InfoTip (the grey tooltip shown on hover
// over the "Windows Update" item in the Control Panel) for the currently
// selected language. English is the fallback for any unknown code.
static const wchar_t* InfoTipForLanguage() {
    if (LanguageIs(L"it"))
        return L"Controlla gli aggiornamenti e visualizza la cronologia degli aggiornamenti.";
    if (LanguageIs(L"es"))
        return L"Busca actualizaciones y consulta el historial de actualizaciones.";
    if (LanguageIs(L"fr"))
        return L"Recherchez les mises à jour et consultez l'historique des mises à jour.";
    if (LanguageIs(L"tr"))
        return L"Güncellemeleri denetleyin ve güncelleme geçmişini görüntüleyin.";
    if (LanguageIs(L"ru"))
        return L"Проверьте наличие обновлений и просмотрите журнал обновлений.";
    if (LanguageIs(L"pt"))
        return L"Verifique atualizações e consulte o histórico de atualizações.";
    if (LanguageIs(L"zh"))
        return L"检查更新并查看更新历史记录。";
    if (LanguageIs(L"pl"))
        return L"Sprawdź aktualizacje i wyświetl historię aktualizacji.";
    if (LanguageIs(L"nl"))
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
        CurrentLanguage() + L"-";
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
                                     CurrentLanguage() + L"-" +
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
    g_builtLanguage = CurrentLanguage();
    Wh_Log(L"Windows Update Restorer: embedded MUI module rebuilt for language %s", LanguageCode());
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
// Embedded "updates installed / up to date" green shield-with-check icon
// (user supplied image, background removed). Used by the Win7-style banner when
// the system is fully up to date.
// -----------------------------------------------------------------------------
static const UINT kUpdatesInstalledIconId = 61003;
// The icon is stored as a raw PNG with transparency (not wrapped in an ICO) so
// GDI+ can decode it and scale it with HighQualityBicubic interpolation at the
// requested size. A single string literal (concatenated across lines).
static const char kUpdatesInstalledPngBase64[] =
    "iVBORw0KGgoAAAANSUhEUgAAAOYAAADmCAYAAADBavm7AADwqElEQVR4nOy9d9xl"
    "11nf+11rl9PfPjPvFI1mRhp1yZJl2cY2GFzAxgVT7GAILbTQTAIh98INcD+5ISSU"
    "hM9NQhLIBZIQTC82xWDiYIOL5CbJ6tM1mvb2U3dda90/1l777HPmnRlZ8mvj4PV+"
    "zuc9ZZ99dlnPetrv+T2CHRjHjx8nTVMajQZ5nmOM4ejRo895f6dOnSLLMrTWeJ6H"
    "lPKa3/E8jyzLntfvnjhxAq01vu8zGo0Iw5AwDNFao7W+6ndvuOGGq35ujEEIse1n"
    "58+fRylTvtZak2UZcRwTxzFpmvL0008bgDRN8X2fNE3J85xWq0Ucxwgh8H2f2dlZ"
    "lpeXxb59+9i1axdh6F/xmE6dOoUQAiEEeZ5f9fi/cH/t/b3SPXy+48p36fns1PcJ"
    "goAgCBBCoJR6XvvzPK+cyFJK0jQlDMNrfsfzvOf1u0opjDGEYVjeAGMMUspnNXmu"
    "Ntwk3G7kuZ0sSZIwGAzo9/sMB5EZjUbEcUyWZTQaDYQQBH6NIAjQDU2e5wgh0Aoa"
    "jQZZltHd6rO12TNPPXmcer1Op9Oh3W6Lm2+5gcXFxYnfdcIhpaRWq131+L9wf3d2"
    "7IhgulW8ugo+n5FlGVmWIaUsL2Kaplf9TpIktNvt5/W79XrdCsVwWN6oPM/xfR9j"
    "zDW+ffVxtWuSpikXL14kjmP6vaEZjUal0Pm+TxiG1Ov1cjIFQYDnecJNKK01aZoa"
    "t/JnWUaSJKRpyubmJsPh0KyuXWDXrl3iuuuuY/fu3dRqNWZmZojjmCiKrnl9v3B/"
    "d3bsiGAGQVCaAs1m83mtPsYYarUaYRiSZRme5+H7/jVNLd/38f3nfnrGGBqNBmAn"
    "gVuhhRDUarVr/v5zGVrD+vo6GxsbnHvmQilYUkparRb1ep1arSZ830cIK4BKKbTW"
    "GANKabIsN0IIAQLP86nXAzzPQylFHMckSYLWivX1dba2tsz58+dZWloShw8fJssy"
    "Op0O9XqdOI6vcaxfuL87OXbEQJ5ebdI0pd/vs7q6Sq1We1arkZSSKIrYt28fzWaT"
    "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"
    "qsaPpzzC8cMJwAFHYOUswsinZJYpdcrc/c17uGLnbl76opeKxaUlmn6LnsIGZqfn"
    "EEIr8szcLN///vf5tV/7NSGlZPPmzSkXkuu6p+0WfeTIkVMef7ryjKVLms3mCqKj"
    "KFAYwqTjB3h+iGGZ+GGAYVhs2DBCX6WX6ZNTdNo+fhBRLBY5euIooHj9q18rqqKP"
    "k49MYwUOeSuHEIpiuYAXtTEc7a5qF0krpUjItoToCtqsk+9cZ98l43yeZj7QbexM"
    "ZWBgIZQuJJZIlNAVI0DMAK57Mhoygsij4zXZcuFmvvLgV/inB/6JwnlFzCHdrj1p"
    "qoSShEISYaIwUZEEEYIItTp6PpZt0vQalGoFyMHt37+N+w8+QHm4jMxLjJyDaVtg"
    "aG5VKQSotXhfn/4+M2sp15PsHj/7mcueykpLufr76eOWkNimxHYNgqhD3s7jBi6T"
    "+2bwJwN++vVvIZ93mZ6fpFDJ0VrycaSrUymOyXe+8x21efNmtm7dSrlcplAoUKlU"
    "KBaLZ9TCPcEtny15Rjl/LEs3afF9H8cu0G57+F6oe0lKHVCRlkmr1eH8888X83OL"
    "WIaDISzcfE6MnhxXR44dZuOGTbxkz3VMHZ7Gimz6ewdRSmHn3BV4WYijpbHLKZTU"
    "6JgzbAqz1t5YCBG7mCEQlz8pGUdjDV3niU4hGIYOr/pRm47fIVQ+wtQt5Rxh0Gos"
    "ccG15/CF+7/Ee7/0PnZceQ6V3jL1hUWMSAeSFCahMHTFNRJJgCTAazcxhMS2bZqt"
    "FpW+MnPNeb7yza8gXQO75NJWAYVyAdtx8Fqd1P2UmW3Fyv332kGh7LFVz1knD3m6"
    "sfz8ZQLvtc53EhHOHpcKoiDU/VQtgR969PX14UiH2WOznLflAm68/mVivr5E229R"
    "KBeYnJjCsXXUNVQRd911F694xSuYmZlhZGSEpaWldI95pvWuP/LUIqC7TCc5Ic1G"
    "oOslXcciCn2itqfr9SJJ3s1RLlcZ3jDI3icexQ+aKufaSCGYnV2kXm/zM297m/Dr"
    "Pke+eZiRYCMlUcM0bOy8geuauMLGDlxAEsoIYXUwbR8LE0tZcR1gdt9grhgJ3N1A"
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
static const wchar_t* kAppletTasksXmlFileName = L"wuapplet-tasks-v2.xml";
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
static std::mutex g_statusIconMutex;
static HICON g_legacyWarningShield = nullptr;
static HICON GetLegacyWarningShieldIcon() {
    std::lock_guard<std::mutex> lock(g_statusIconMutex);
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
static std::mutex g_gdiPlusMutex;
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
    std::lock_guard<std::mutex> lock(g_gdiPlusMutex);
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
    std::lock_guard<std::mutex> lock(g_gdiPlusMutex);
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
    std::lock_guard<std::mutex> lock(g_statusIconMutex);
    if (g_updatesInstalledIcon) return g_updatesInstalledIcon;
    g_updatesInstalledIcon = CreateIconFromBase64PngBicubic(kUpdatesInstalledPngBase64, 48, 48);
    return g_updatesInstalledIcon;
}

static HICON g_windows81UpdateStatusIcon = nullptr;
static HICON GetWindows81UpdateStatusIcon() {
    std::lock_guard<std::mutex> lock(g_statusIconMutex);
    if (g_windows81UpdateStatusIcon) return g_windows81UpdateStatusIcon;
    g_windows81UpdateStatusIcon = CreateIconFromBase64PngBicubic(kWindows81UpdateStatusPngBase64, 48, 48);
    return g_windows81UpdateStatusIcon;
}

static HICON g_wuDisabledShieldIcon = nullptr;
static HICON GetWuDisabledShieldIcon() {
    std::lock_guard<std::mutex> lock(g_statusIconMutex);
    if (g_wuDisabledShieldIcon) return g_wuDisabledShieldIcon;
    g_wuDisabledShieldIcon = CreateIconFromBase64PngBicubic(kWuDisabledShieldPngBase64, 48, 48);
    return g_wuDisabledShieldIcon;
}

using LoadImageW_t = decltype(&LoadImageW);
static LoadImageW_t LoadImageWOriginalForLegacyWarningIcon = nullptr;
static HANDLE WINAPI LoadImageWHookForLegacyWarningIcon(HINSTANCE instance, LPCWSTR name, UINT type, int cx, int cy, UINT flags) {
    // Only substitute our private status icons when the request actually targets
    // the shell32.dll resource library, which is how our DirectUI XML references
    // them (icon(... library(shell32.dll))). This keeps other modules in
    // explorer.exe that happen to use one of the same numeric IDs unaffected.
    if (instance != GetModuleHandleW(L"shell32.dll")) return LoadImageWOriginalForLegacyWarningIcon(instance, name, type, cx, cy, flags);
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
        g_language.store(LanguageFromCode(detected.empty() ? L"en" : detected), std::memory_order_release);
    } else {
        g_language.store(LanguageFromCode(value), std::memory_order_release);
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
    g_removeLegacyBrokenOption.store(Wh_GetIntSetting(L"RemoveLegacyBrokenOption") != 0);
    // Debug: force the "pending updates" interface even without a real pending update.
    if constexpr (kWuDebugForcePendingEnabled)
        g_debugForcePending.store(true);
    else
        g_debugForcePending.store(false);
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

    auto it = kParts.find(CurrentLanguage());
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
    auto it = kTexts.find(CurrentLanguage());
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

// -----------------------------------------------------------------------------
// Blue link to the Windows Update settings child page ("Open Windows Update
// settings"). Shown only when the recreated hub cannot be built (Windows Update
// service unavailable / AU not configured) and only the red warning box remains
// on the page, so the user still gets a one-click path to the classic settings
// page (shell:::{36EEF7DB-88AD-4E81-AD49-0E313F0C35F8}\pageSettings).
//
// Additionally, there is this explorer shell:::{1138506a_b949_46a7_b6c0_ee26499fdeaf} which I don't know what was used for
// The link is NOT attached to the native moduleAUNotConfigured element: wucltux
// re-shows/re-sizes that module at runtime and overrides any XML added inside
// it (and re-appends it, pushing siblings below it). Instead, when only the red
// box would be shown, the native module is collapsed to a zero-size element and
// BuildRedBoxFallbackXml() renders a faithful recreation of the red box with
// this link directly below it - a self-contained module wucltux does not touch.
// -----------------------------------------------------------------------------
static const wchar_t* SelectOpenWindowsUpdateSettingsLinkText() {
    static const std::unordered_map<std::wstring, const wchar_t*> kTexts = {
        { L"en", L"Open Windows Update settings" },
        { L"it", L"Apri Impostazioni di Windows Update" },
        { L"es", L"Abrir la configuración de Windows Update" },
        { L"fr", L"Ouvrir les paramètres de Windows Update" },
        { L"tr", L"Windows Update ayarlarını aç" },
        { L"ru", L"Открыть параметры Центра обновления Windows" },
        { L"pt", L"Abrir as configurações do Windows Update" },
        { L"zh", L"打开 Windows 更新设置" },
        { L"pl", L"Otwórz ustawienia Windows Update" },
        { L"nl", L"Windows Update-instellingen openen" },
    };
    auto it = kTexts.find(CurrentLanguage());
    if (it == kTexts.end()) it = kTexts.find(L"en");
    return it->second;
}

// Forward declaration: defined below (used by BuildRedBoxFallbackXml).
static const wchar_t* WuOptionText(DWORD opt) {
    switch (opt) {
        case 4: return EmbeddedMuiString(334);  // Install updates automatically (recommended)
        case 3: return EmbeddedMuiString(335);  // Download updates but let me choose...
        case 2: return EmbeddedMuiString(336);  // Check for updates but let me choose...
        case 1: return EmbeddedMuiString(337);  // Never check for updates (not recommended)
        default: return EmbeddedMuiString(334);
    }
}

// Replaced with native ComboBox

static std::wstring BuildOpenWindowsUpdateSettingsLinkXml() {
    // Opens the classic settings child page via the shell URI directly
    // (shell:::{36EEF7DB-...}\pageSettings). We deliberately do NOT launch
    // "%SystemRoot%\explorer.exe" with shellexecuteparams: that spawns a NEW
    // explorer.exe process (which reloads Windhawk mods and then hands the
    // command over), which is unstable inside the shell and crashes explorer.
    // A bare shell: URI is dispatched by the existing shell instance - same
    // mechanism as ms-settings: and https:// links used elsewhere in the mod.
    return
        L"<element layoutpos=\"top\" layout=\"flowlayout()\" margin=\"rect(12rp,8rp,12rp,0)\">"
        L"<NavigateButton layout=\"flowlayout()\" shellexecute=\"shell:::"
        + std::wstring(kAppletClsid) + L"\\pageSettings\">"
        L"<Button sheet=\"wu_cp_style\" class=\"cp_content_link\" active=\"mouse | keyboard\" content=\""
        + XmlEscape(SelectOpenWindowsUpdateSettingsLinkText()) +
        L"\"/></NavigateButton></element>";
}

// -----------------------------------------------------------------------------
// Faithful recreation of the native red "Automatic updates are not configured"
// box (moduleAUNotConfigured, UIFILE 123) with the blue settings link below it.
// Used when Windows Update is unavailable / AU is not configured: the native
// module is collapsed to a zero-size element (atom stays resolvable -> no
// S_FALSE, no provider fallback re-materialization, as proven by the hub path)
// and this self-contained module is rendered instead. The inner atoms
// (actionTurnOnAU, actionAdvancedAUSettings, ...) are kept identical so
// wucltux's code-behind still finds them and the "Turn on automatic updating"
// button keeps working; the module root gets a unique atom wucltux ignores, so
// nothing is moved, resized or overridden at runtime and the link stays put.
//
// includeLink controls whether the blue "Open Windows Update settings" link is
// appended below the box: it is shown with "Show recreated interface" ON and
// omitted with it OFF. The box itself is always rendered - even with the
// recreated interface disabled, the user must still get the "Turn on automatic
// updating" box (the native one is re-shown/overridden by the provider and
// unreliable on modern builds).
// -----------------------------------------------------------------------------
static std::wstring BuildRedBoxFallbackXml(bool includeLink = true) {
    std::wstring xml =
        L"<element id=\"atom(wuamodern_redbox_fallback)\" sheet=\"wuappstyle\" layoutpos=\"top\" "
        L"layout=\"borderlayout()\" margin=\"rect(0,12rp,0,12rp)\">"
        // --- the red box itself (native moduleborder1 structure) ---
        L"<element class=\"moduleborder1\" layoutpos=\"top\" layout=\"borderlayout()\">"
        L"<element id=\"atom(areaModuleColorBox)\" layoutpos=\"left\" layout=\"borderlayout()\" "
        L"sheet=\"wuappstyle\" class=\"security_box_gradient_red\"/>"
        L"<element id=\"atom(areaModuleIcon)\" layoutpos=\"left\" layout=\"borderlayout()\" "
        L"padding=\"rect(12rp,12rp,4rp,0)\" contentalign=\"topleft\">"
        L"<element layoutpos=\"top\" layout=\"borderlayout()\" contentalign=\"topleft\">"
        L"<viewer class=\"wuapp_module_img\">"
        L"<element id=\"atom(elementRedModuleIcon)\" class=\"wuapp_module_img\" layoutpos=\"top\" "
        L"content=\"icon(105,48rp,48rp,library(imageres.dll))\"/>"
        L"</viewer></element></element>"
        L"<element layoutpos=\"top\" layout=\"flowlayout(1,0,0,2)\" padding=\"rect(0,12rp,12rp,4rp)\">"
        L"<element class=\"wuapp_module_instruction\" content=\"resstr(1149)\"/>"
        L"</element>"
        L"<element layoutpos=\"top\" layout=\"flowlayout(1,0,0,2)\" padding=\"rect(2rp,0,48rp,0)\">"
        L"<element class=\"wuapp_content_title\" content=\"resstr(1153)\"/>"
        L"</element>"
        L"<element layoutpos=\"top\" layout=\"flowlayout(1,0,0,2)\" padding=\"rect(2rp,12rp,48rp,0)\">"
        L"<element sheet=\"wu_cp_style\" class=\"cp_content_text\" content=\"resstr(1154)\"/>"
        L"</element>"
        L"<element layoutpos=\"top\" layout=\"borderlayout()\" padding=\"rect(0,7rp,18rp,18rp)\">"
        L"<element layoutpos=\"right\" layout=\"borderlayout()\" padding=\"rect(0,0,0,0)\">"
        L"<element layoutpos=\"top\" layout=\"flowlayout(0,0,2,2)\" padding=\"rect(0,0,0,0)\">"
        L"<viewer>"
        L"<CCPushButton id=\"atom(actionTurnOnAU)\" layoutpos=\"right\" active=\"mouse | keyboard\" "
        L"sheet=\"wu_cp_style\" shortcut=\"auto\" content=\"resstr(1150)\"/>"
        L"</viewer></element>"
        L"<element id=\"atom(actionAdvancedAUSettingsArea)\" layoutpos=\"top\" layout=\"flowlayout(0,0,2,2)\" "
        L"padding=\"rect(0,2rp,0,0)\">"
        L"<NavigateButton id=\"atom(actionAdvancedAUSettings)\" layoutpos=\"top\" layout=\"flowlayout()\" "
        L"padding=\"rect(0,0,0,0)\" navigationtargetroot=\"\" navigationtargetrelative=\"pageSettings\">"
        L"<Button id=\"atom(actionAdvancedAUSettingsText)\" sheet=\"wu_cp_style\" class=\"cp_content_link\" "
        L"active=\"mouse | keyboard\" content=\"resstr(1254)\"/>"
        L"</NavigateButton></element>"
        L"</element></element></element>";
    // --- our blue link, directly below the red box (only with recreated UI on) ---
    // Opens the settings page, which now shows the classic option list and the
    // blue "change update frequency" link that opens the Win32 dialog.
    if (includeLink) xml += BuildOpenWindowsUpdateSettingsLinkXml();
    xml += L"</element>";
    return xml;
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

// -----------------------------------------------------------------------------
// "Important updates" selector on the classic settings page (pageSettings).
//
// On modern Windows the legacy combobox (atom(auOptionSelectorCombobox)) is
// populated by wucltux code-behind that no longer works (the Windows Update
// service is stopped/broken), so the "Important updates" section shows an
// empty box. The atom cannot be removed (the page would fail to load), so we
// collapse it to a zero-size element (atom stays resolvable, same trick as the
// hub modules) and render our own list of the four classic options instead,
// reading the current AUOptions value from the registry. Clicking an option
// writes AUOptions through a private "wurestorer:auoptions=N" URI handled by
// the ShellExecuteExW/ShellExecuteW hooks below.
//
// AUOptions values (Windows Update Agent / classic Control Panel):
//   4 = install updates automatically (recommended)          -> resstr(334)
//   3 = download updates but let me choose whether to install -> resstr(335)
//   2 = check for updates but let me choose download/install  -> resstr(336)
//   1 = never check for updates (not recommended)             -> resstr(337)
// -----------------------------------------------------------------------------
static const wchar_t* kWuRestorerProtocol = L"wurestorer:";

static DWORD ReadAuOptionsValue() {
    // Windows 10/11 honour the Group-Policy key first; read it when present so
    // the shown selection reflects the effective state.
    HKEY hPolicy = nullptr;
    if (RegOpenKeyExW(HKEY_LOCAL_MACHINE,
            L"SOFTWARE\\Policies\\Microsoft\\Windows\\WindowsUpdate\\AU",
            0, KEY_READ, &hPolicy) == ERROR_SUCCESS) {
        DWORD noAuto = 0, size = sizeof(noAuto);
        if (RegQueryValueExW(hPolicy, L"NoAutoUpdate", nullptr, nullptr,
                             reinterpret_cast<LPBYTE>(&noAuto), &size) == ERROR_SUCCESS && noAuto != 0) {
            RegCloseKey(hPolicy);
            return 1; // never check
        }
        RegCloseKey(hPolicy);
    }

    DWORD auOptions = 4; // default: recommended
    HKEY hKey = nullptr;
    if (RegOpenKeyExW(HKEY_LOCAL_MACHINE,
            L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\WindowsUpdate\\Auto Update",
            0, KEY_READ, &hKey) == ERROR_SUCCESS) {
        DWORD noAuto = 0, size = sizeof(noAuto);
        if (RegQueryValueExW(hKey, L"NoAutoUpdate", nullptr, nullptr,
                             reinterpret_cast<LPBYTE>(&noAuto), &size) == ERROR_SUCCESS && noAuto != 0) {
            auOptions = 1; // never check
        } else {
            size = sizeof(auOptions);
            if (RegQueryValueExW(hKey, L"AUOptions", nullptr, nullptr,
                                 reinterpret_cast<LPBYTE>(&auOptions), &size) != ERROR_SUCCESS ||
                auOptions < 1 || auOptions > 4) {
                auOptions = 4;
            }
        }
        RegCloseKey(hKey);
    }
    return auOptions;
}

static bool WriteAuOptionsValue(DWORD value) {
    // The restored page is intentionally read-only. A shell extension must not
    // create or modify machine-wide Windows Update/Group Policy settings.
    Wh_Log(L"Windows Update Restorer: AUOptions change (%lu) was not applied; open Settings to change update policy.", value);
    return false;
}
static const wchar_t* SelectUpdateIntroText() {
    static const std::unordered_map<std::wstring, const wchar_t*> kTexts = {
        { L"en", L"Windows Update keeps your PC secure and reliable by installing the latest updates." },
        { L"it", L"Windows Update mantiene il PC sicuro e affidabile installando gli aggiornamenti più recenti." },
        { L"es", L"Windows Update mantiene su PC seguro y fiable instalando las actualizaciones más recientes." },
        { L"fr", L"Windows Update maintient votre PC sûr et fiable en installant les dernières mises à jour." },
        { L"tr", L"Windows Update, bilgisayarınızı en son güncellemeleri yükleyerek güvenli ve güvenilir tutar." },
        { L"ru", L"Центр обновления Windows поддерживает компьютер в безопасности и стабильной работе, устанавливая последние обновления." },
        { L"pt", L"O Windows Update mantém seu PC seguro e confiável instalando as atualizações mais recentes." },
        { L"zh", L"Windows 更新通过安装最新更新，让您的电脑保持安全和稳定。" },
        { L"pl", L"Windows Update utrzymuje komputer bezpieczny i niezawodny, instalując najnowsze aktualizacje." },
        { L"nl", L"Windows Update houdt uw pc veilig en betrouwbaar door de nieuwste updates te installeren." },
    };
    auto it = kTexts.find(CurrentLanguage());
    if (it == kTexts.end()) it = kTexts.find(L"en");
    return it->second;
}

// Renders the introduction above the native ComboBox control
// FIX: originale wucltux.dll (UIFILE 125) posiziona la combobox INLINE dentro al flowlayout delle icone (layoutpos="left", width="10rp",
// margin="rect(0,3rp,0,10rp)", classe aupsp_auComboBox). Il placeholder precedente era un elemento "top" a larghezza piena (28rp, 0,8rp,0,12rp)
// posizionato SOPRA la riga delle icone, non inline: risultava spostato (~90px in basso con fallback 180,240) e la dropdown ereditava coordinate errate
// (altezza dropdown = altezza collapsed = 28rp -> lista troncata a 1 voce). Qui si mantiene solo il testo introduttivo come riga "top"; il placeholder
// vero viene iniettato INLINE al posto del <combobox> originale (vedi PatchSettingsPageXml), usando identica posizione originale ma con width corretta (285rp)
// per contenere le 4 opzioni lunghe e altezza dropdown separata.
static std::wstring BuildUpdateIntroTextXml() {
    return
        L"<element layoutpos=\"top\" layout=\"flowlayout(0,0,0,2)\" margin=\"rect(0,0,0,6rp)\">"
        L"<element id=\"atom(auUpdateIntroText)\" sheet=\"wuappstyle\" class=\"cp_content_text\" content=\""
        + XmlEscape(SelectUpdateIntroText()) +
        L"\"/></element>";
}


// =============================================================================
// Native Win32 Drop-down ComboBox for Settings Page ("pageSettings")
// =============================================================================
#define IDC_NATIVE_AU_COMBO 0x7720

static HWND g_hwndDirectUiParent = nullptr;
static std::atomic<bool> g_isSettingsPageActive{false};

typedef void* DirectUI_Element;
typedef DirectUI_Element (*DUI_HWNDElement_GetElement_t)(HWND);
typedef DirectUI_Element (*DUI_Element_FindDescendent_t)(DirectUI_Element, unsigned short);
typedef HRESULT (*DUI_Element_GetBounds_t)(DirectUI_Element, RECT*);

static void RefreshWuPage(HWND host);
static LRESULT CALLBACK SettingsDirectUiSubclassProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);

// Subclass APIs and timers are thread-affine. This message is synchronously
// delivered by the owner thread before an any-thread subclass removal.
static const UINT g_wmUiTeardown = RegisterWindowMessageW(L"Windhawk.WUControlPanelRestorer.UiTeardown");
static std::mutex g_subclassWindowsMutex;
static std::vector<HWND> g_settingsSubclassWindows;
static std::vector<HWND> g_sidebarSubclassWindows;

// =============================================================================
// NATIVE DIRECTUI COMBOBOX REPAIR (No Win32 Overlay)
// =============================================================================

typedef ATOM (WINAPI *DirectUI_StrToID_t)(const wchar_t* str);
typedef void* (*DirectUI_FindDescendent_t)(void* element, ATOM atom);
typedef HRESULT (*DirectUI_Combobox_AddString_t)(void* combobox, const wchar_t* str);
typedef HRESULT (*DirectUI_Combobox_SetSelection_t)(void* combobox, int index);
typedef int (*DirectUI_Combobox_GetSelection_t)(void* combobox);
typedef HRESULT (*DirectUI_Element_SetEnabled_t)(void* element, bool enabled);
typedef void* (*DirectUI_Element_GetParent_t)(void* element);
// DirectUI::Element::GetID() -> the element's atom (same space as StrToID).
// Try both const and non-const x64 manglings; only one is actually exported.
typedef unsigned int (*DirectUI_Element_GetID_t)(void* element);

static DirectUI_StrToID_t pStrToID = nullptr;
static DirectUI_FindDescendent_t pFindDescendent = nullptr;
static DirectUI_Combobox_AddString_t pAddString = nullptr;
static DirectUI_Combobox_SetSelection_t pSetSelection = nullptr;
static DirectUI_Combobox_GetSelection_t pGetSelection = nullptr;
static DirectUI_Element_SetEnabled_t pSetEnabled = nullptr;
static DirectUI_Element_GetParent_t pGetParent = nullptr;
static DirectUI_Element_GetID_t pGetID = nullptr;

static bool g_nativeComboPopulated = false;
static DWORD g_lastAuOptions = 4;
static void* g_lastComboPtr = nullptr;

// -----------------------------------------------------------------------------
// DirectUI SetEnabled hook - keeps the settings combobox enabled.
// -----------------------------------------------------------------------------
// The "Important updates" combobox (atom auOptionSelectorCombobox) renders
// disabled because wucltux.dll's code-behind calls Element::SetEnabled(combo,
// false) based on its stale Win8.1 view of whether the AU option may change.
// Our 200 ms timer calling SetEnabled(combo, true) only races it - and worse,
// the pointer lookup (GetWindowLongPtrW(hwnd,0) + FindDescendent) can fail
// entirely on some builds, so the timer never even reaches the control.
//
// Fix: hook Element::SetEnabled itself and identify the combobox by its ATOM
// via Element::GetID() - this needs no root/pointer lookup at all, so it works
// even when FindDescendent never resolves the element. The hook is installed
// early (Wh_ModInit, before any page XML is parsed) so it catches the disable
// both at parse time and during wucltux's runtime state passes. As a bonus,
// when the combo pointer IS known we also protect its ancestors/children.
static UINT g_comboAtom = 0; // StrToID(L"auOptionSelectorCombobox"), resolved once
static std::atomic<void*> g_protectedCombo{nullptr};

using DirectUI_SetEnabledHook_t = HRESULT(WU_DUI_THISCALL*)(void* element, bool enabled);
static DirectUI_SetEnabledHook_t SetEnabledOriginal = nullptr;

// True when disabling 'el' would grey out the protected combobox: that is when
// el is the combobox itself (matched by atom, pointer-independently) or, when
// the pointer is known, an ancestor or descendant of it.
static bool IsProtectedComboElement(void* el) {
    if (!el) return false;
    // Primary: match the combobox by its atom. Works without ever finding the
    // element pointer, so it does not depend on GetWindowLongPtrW/FindDescendent.
    if (g_comboAtom && pGetID && pGetID(el) == g_comboAtom) return true;
    // Secondary (pointer known): protect ancestors and descendants too.
    void* combo = g_protectedCombo.load();
    if (!combo || !pGetParent) return false;
    if (el == combo) return true;
    void* cur = el;
    for (int i = 0; i < 64 && cur; ++i) { cur = pGetParent(cur); if (cur == combo) return true; } // descendant
    cur = combo;
    for (int i = 0; i < 64 && cur; ++i) { cur = pGetParent(cur); if (cur == el) return true; }    // ancestor
    return false;
}

static HRESULT WU_DUI_THISCALL SetEnabledHook(void* element, bool enabled) {
    if (!enabled) {
        // DIAGNOSTIC: log the first few SetEnabled(false) calls with the
        // element's GetID so we can see whether the combobox is disabled via
        // SetEnabled at all, and whether GetID identifies it. Throttled.
        static std::atomic<int> s_disableLogged{0};
        int n = s_disableLogged.fetch_add(1);
        if (n < 20) {
            unsigned int id = (pGetID && element) ? pGetID(element) : 0;
            Wh_Log(L"Windows Update Restorer: SetEnabled(false) el=%p id=%u comboAtom=%u getID=%hs protected=%d",
                   element, id, g_comboAtom, pGetID ? "ok" : "MISSING",
                   (int)IsProtectedComboElement(element));
        }
    }
    if (!enabled && IsProtectedComboElement(element)) {
        // Swallow the disable: keep the combobox enabled.
        return SetEnabledOriginal(element, true);
    }
    return SetEnabledOriginal(element, enabled);
}

// Resolves the dui70 exports we need and the combobox atom. Idempotent.
static void EnsureDui70ComboboxExports() {
    if (!pStrToID) {
        HMODULE dui70 = GetModuleHandleW(L"dui70.dll");
        if (!dui70) dui70 = LoadLibraryExW(L"dui70.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
        if (dui70) {
            pStrToID = (DirectUI_StrToID_t)GetProcAddress(dui70, "StrToID");
            pFindDescendent = (DirectUI_FindDescendent_t)GetProcAddress(dui70, "?FindDescendent@Element@DirectUI@@QEAAPEAV12@G@Z");
            pAddString = (DirectUI_Combobox_AddString_t)GetProcAddress(dui70, "?AddString@Combobox@DirectUI@@QEAAHPEBG@Z");
            pSetSelection = (DirectUI_Combobox_SetSelection_t)GetProcAddress(dui70, "?SetSelection@Combobox@DirectUI@@QEAAJH@Z");
            pGetSelection = (DirectUI_Combobox_GetSelection_t)GetProcAddress(dui70, "?GetSelection@Combobox@DirectUI@@QEAAHXZ");
            pSetEnabled = (DirectUI_Element_SetEnabled_t)GetProcAddress(dui70, "?SetEnabled@Element@DirectUI@@QEAAJ_N@Z");
            pGetParent = (DirectUI_Element_GetParent_t)GetProcAddress(dui70, "?GetParent@Element@DirectUI@@QEAAPEAV12@XZ");
            // GetID: try several manglings (const/non-const x UINT/int return).
            // At the x64 ABI level all of them return the value in EAX, so the
            // unsigned int typedef below reads it correctly regardless of which
            // is the true signature - we just need the right symbol string.
            const char* getIdNames[] = {
                "?GetID@Element@DirectUI@@QEBAIXZ",  // UINT, const
                "?GetID@Element@DirectUI@@QEAAIXZ",  // UINT, non-const
                "?GetID@Element@DirectUI@@QEBAHXZ",  // int, const
                "?GetID@Element@DirectUI@@QEAAHXZ",  // int, non-const
            };
            for (const char* n : getIdNames) {
                pGetID = (DirectUI_Element_GetID_t)GetProcAddress(dui70, n);
                if (pGetID) break;
            }
        }
    }
    if (!g_comboAtom && pStrToID) {
        g_comboAtom = (UINT)pStrToID(L"auOptionSelectorCombobox");
    }
}

// Installs the Element::SetEnabled hook exactly once (SetFunctionHook must not
// be called twice). Called early from Wh_ModInit and again lazily from the
// settings page timer so it is in place before the page XML is parsed.
static void EnsureSetEnabledHookInstalled() {
    EnsureDui70ComboboxExports();
    static bool s_hookInstalled = false;
    if (!s_hookInstalled && pSetEnabled) {
        if (WindhawkUtils::SetFunctionHook(
                reinterpret_cast<DirectUI_SetEnabledHook_t>(pSetEnabled),
                SetEnabledHook, &SetEnabledOriginal)) {
            s_hookInstalled = true;
            Wh_Log(L"Windows Update Restorer: hooked DirectUI Element::SetEnabled (combo atom=%u, GetID=%hs)",
                   g_comboAtom, pGetID ? "ok" : "MISSING");
        }
    }
}

static void InitDirectUIExports() {
    EnsureDui70ComboboxExports();
    EnsureSetEnabledHookInstalled();
}

static void DestroySettingsCombobox() {
    g_isSettingsPageActive.store(false);
    std::vector<HWND> windows;
    { std::lock_guard lock(g_subclassWindowsMutex); windows = g_settingsSubclassWindows; }
    // Do not hold the lock: SendMessage executes the teardown in the window's
    // owner thread, where both KillTimer and RemoveWindowSubclass are valid.
    for (HWND hwnd : windows) if (IsWindow(hwnd)) {
        SendMessageW(hwnd, g_wmUiTeardown, 0, 0);
    }
    { std::lock_guard lock(g_subclassWindowsMutex); g_settingsSubclassWindows.clear(); }
    g_hwndDirectUiParent = nullptr;
    g_nativeComboPopulated = false; g_lastComboPtr = nullptr; g_protectedCombo.store(nullptr);
}

static LRESULT CALLBACK SettingsDirectUiSubclassProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData) {
    if (!g_isSettingsPageActive.load()) {
        KillTimer(hwnd, 889);
        RemoveWindowSubclass(hwnd, SettingsDirectUiSubclassProc, uIdSubclass);
        if (g_hwndDirectUiParent == hwnd) g_hwndDirectUiParent = nullptr;
        return DefSubclassProc(hwnd, uMsg, wParam, lParam);
    }

    // RegisterWindowMessage returns a run-time value, so it cannot be a case
    // label. Handle it before the switch instead.
    if (uMsg == g_wmUiTeardown) {
        KillTimer(hwnd, 889);
        RemoveWindowSubclass(hwnd, SettingsDirectUiSubclassProc, uIdSubclass);
        {
            std::lock_guard lock(g_subclassWindowsMutex);
            auto it = std::remove(g_settingsSubclassWindows.begin(),
                                  g_settingsSubclassWindows.end(), hwnd);
            g_settingsSubclassWindows.erase(it, g_settingsSubclassWindows.end());
        }
        if (g_hwndDirectUiParent == hwnd) g_hwndDirectUiParent = nullptr;
        g_nativeComboPopulated = false;
        g_lastComboPtr = nullptr;
        g_protectedCombo.store(nullptr);
        return 0;
    }

    switch (uMsg) {
        case WM_TIMER: {
            if (wParam == 889) {
                InitDirectUIExports();
                // DIAGNOSTIC (one-shot): report what the pointer-based lookup
                // sees, so we know whether root/atom/GetID are resolving.
                static bool s_diagOnce = false;
                if (!s_diagOnce) {
                    s_diagOnce = true;
                    void* r = (void*)GetWindowLongPtrW(hwnd, 0);
                    ATOM a = pStrToID ? pStrToID(L"auOptionSelectorCombobox") : 0;
                    Wh_Log(L"Windows Update Restorer: combo diag hwnd=%p root=%p atom=%u getID=%hs findDesc=%hs",
                           hwnd, r, (unsigned)a, pGetID ? "ok" : "MISSING",
                           pFindDescendent ? "ok" : "MISSING");
                }
                if (pStrToID && pFindDescendent && pAddString && pSetSelection && pGetSelection) {
                    void* root = (void*)GetWindowLongPtrW(hwnd, 0);
                    if (root) {
                        ATOM atom = pStrToID(L"auOptionSelectorCombobox");
                        if (atom) {
                            void* combo = pFindDescendent(root, atom);
                            if (combo) {
                                if (g_lastComboPtr != combo) {
                                    g_lastComboPtr = combo;
                                    g_nativeComboPopulated = false;
                                }
                                g_protectedCombo.store(combo); // SetEnabled hook keeps it enabled
                                if (pSetEnabled) pSetEnabled(combo, true); // immediate re-enable
                                
                                if (!g_nativeComboPopulated) {
                                    pAddString(combo, WuOptionText(4));
                                    pAddString(combo, WuOptionText(3));
                                    pAddString(combo, WuOptionText(2));
                                    pAddString(combo, WuOptionText(1));
                                    
                                    DWORD current = ReadAuOptionsValue();
                                    g_lastAuOptions = current;
                                    int selIdx = (current == 4) ? 0 : (current == 3) ? 1 : (current == 2) ? 2 : 3;
                                    pSetSelection(combo, selIdx);
                                    g_nativeComboPopulated = true;
                                    Wh_Log(L"Windows Update Restorer: Native DirectUI Combobox populated (addr=%p)!", combo);
                                } else {
                                    int sel = pGetSelection(combo);
                                    if (sel >= 0 && sel <= 3) {
                                        DWORD auOpt = (sel == 0) ? 4 : (sel == 1) ? 3 : (sel == 2) ? 2 : 1;
                                        if (auOpt != g_lastAuOptions) {
                                            g_lastAuOptions = auOpt;
                                            WriteAuOptionsValue(auOpt);
                                            Wh_Log(L"Windows Update Restorer: Native combo sel changed to %d (AUOptions=%lu)", sel, auOpt);
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
            break;
        }
        case WM_DESTROY: {
            KillTimer(hwnd, 889);
            RemoveWindowSubclass(hwnd, SettingsDirectUiSubclassProc, uIdSubclass);
            if (g_hwndDirectUiParent == hwnd) g_hwndDirectUiParent = nullptr;
            g_nativeComboPopulated = false;
            g_protectedCombo.store(nullptr);
            break;
        }
    }
    return DefSubclassProc(hwnd, uMsg, wParam, lParam);
}

struct EnumSettingsDirectUiCtx {
    HWND verified = nullptr; // DirectUIHWND con atom pageSettings confermato
    HWND fallback = nullptr; // primo DirectUIHWND dimensionalmente valido, se nessuno verificato
};

static BOOL CALLBACK EnumSettingsDirectUiProc(HWND hwnd, LPARAM lParam) {
    if (!IsWindow(hwnd)) return TRUE;
    wchar_t cls[64] = {};
    GetClassNameW(hwnd, cls, ARRAYSIZE(cls));
    if (_wcsicmp(cls, L"DirectUIHWND") == 0) {
        RECT rc{};
        GetClientRect(hwnd, &rc);
        int width = rc.right - rc.left;
        int height = rc.bottom - rc.top;
        if (width > 260 && height > 150) {
            EnumSettingsDirectUiCtx* ctx = reinterpret_cast<EnumSettingsDirectUiCtx*>(lParam);
            if (!ctx->fallback) ctx->fallback = hwnd;
        }
    }
    return TRUE;
}

static HWND FindSettingsDirectUiHwnd() {
    EnumSettingsDirectUiCtx ctx;
    HWND hwndFg = GetForegroundWindow();
    if (hwndFg) {
        EnumChildWindows(hwndFg, EnumSettingsDirectUiProc, reinterpret_cast<LPARAM>(&ctx));
        if (ctx.fallback) return ctx.fallback;
    }
    HWND hwndActive = GetActiveWindow();
    if (hwndActive && hwndActive != hwndFg) {
        EnumChildWindows(hwndActive, EnumSettingsDirectUiProc, reinterpret_cast<LPARAM>(&ctx));
        if (ctx.fallback) return ctx.fallback;
    }
    HWND hwndFocus = GetFocus();
    if (hwndFocus) {
        HWND root = GetAncestor(hwndFocus, GA_ROOT);
        if (root && root != hwndFg && root != hwndActive) {
            EnumChildWindows(root, EnumSettingsDirectUiProc, reinterpret_cast<LPARAM>(&ctx));
            if (ctx.fallback) return ctx.fallback;
        }
    }
    EnumThreadWindows(GetCurrentThreadId(), [](HWND top, LPARAM lp) -> BOOL {
        EnumSettingsDirectUiCtx* pCtx = reinterpret_cast<EnumSettingsDirectUiCtx*>(lp);
        EnumChildWindows(top, EnumSettingsDirectUiProc, lp);
        if (pCtx->fallback) return FALSE;
        return TRUE;
    }, reinterpret_cast<LPARAM>(&ctx));
    
    return ctx.fallback;
}

static void InitializeNativeSettingsCombobox(HWND hwndParent) {
    g_isSettingsPageActive.store(true);
    if (!hwndParent || !IsWindow(hwndParent)) hwndParent = FindSettingsDirectUiHwnd();
    if (!hwndParent || !IsWindow(hwndParent)) return;
    g_hwndDirectUiParent = hwndParent;
    if (SetWindowSubclass(hwndParent, SettingsDirectUiSubclassProc, 888, 0)) {
        std::lock_guard lock(g_subclassWindowsMutex);
        if (std::find(g_settingsSubclassWindows.begin(), g_settingsSubclassWindows.end(), hwndParent) == g_settingsSubclassWindows.end())
            g_settingsSubclassWindows.push_back(hwndParent);
    }
    SetTimer(hwndParent, 889, 200, nullptr);
}

static void UpdateSettingsComboboxLanguage() {
    // Il testo del combobox nativo si ricaricherà alla prossima apertura della pagina
}

static std::wstring PatchSettingsPageXml(const std::wstring& input) {
    InitializeNativeSettingsCombobox(nullptr);

    std::wstring out = input;
    const size_t cbAtom = out.find(L"atom(auOptionSelectorCombobox)");
    if (cbAtom == std::wstring::npos) return input;

    // FIX NATIVO: Allarga il combobox mantenendo l'XML originale
    size_t tagStart = out.rfind(L"<combobox", cbAtom);
    if (tagStart == std::wstring::npos) tagStart = out.rfind(L"<ComboBox", cbAtom);
    if (tagStart == std::wstring::npos) tagStart = out.rfind(L"<COMBOBOX", cbAtom);
    if (tagStart != std::wstring::npos) {
        size_t widthPos = out.find(L"width=\"10rp\"", tagStart);
        if (widthPos != std::wstring::npos && widthPos < cbAtom) {
            out.replace(widthPos, 12, L"width=\"285rp\"");
            Wh_Log(L"Windows Update Restorer: Native combobox width expanded to 285rp");
        }
    }

    // Insert intro text SOPRA la flow row delle icone+combo (non dentro), come riga separata top
    // Questo preserva identica Y originale della flow row (spostata in basso di ~ altezza intro), ma il placeholder INLINE rimane nella flow originale.
    // Trova l'inizio della flow row che contiene le icone+placeholder (ora placeholder)
    const size_t placeholderPos = out.find(L"atom(auOptionSelectorPlaceholder)");
    const size_t rowStart = out.rfind(
        L"<element layoutpos=\"top\" layout=\"flowlayout(0,0,0,2)\" margin=\"rect(0,14rp,0,0)\">", placeholderPos != std::wstring::npos ? placeholderPos : cbAtom);
    if (rowStart != std::wstring::npos) {
        out.insert(rowStart, BuildUpdateIntroTextXml());
        Wh_Log(L"Windows Update Restorer: settings page - inserted intro text above icon row (placeholder now INLINE at original position)");
    }

    // Hide the red warning shield: blank the icon element INSIDE the
    // atom(auOptionSelectorWarningIcon) viewer (zero-size, empty content).
    const size_t warnAtom = out.find(L"atom(auOptionSelectorWarningIcon)");
    if (warnAtom != std::wstring::npos) {
        const size_t iconPos = out.find(L"content=\"icon(105,", warnAtom);
        if (iconPos != std::wstring::npos) {
            const size_t tagStart = out.rfind(L"<element", iconPos);
            const size_t tagEnd = out.find(L"/>", tagStart);
            if (tagStart != std::wstring::npos && tagEnd != std::wstring::npos) {
                const std::wstring hidden =
                    L"<element sheet=\"wuappstyle\" class=\"aupsp_left_img\" width=\"0rp\" height=\"0rp\" content=\"\"/>";
                out.replace(tagStart, tagEnd + 2 - tagStart, hidden);
                Wh_Log(L"Windows Update Restorer: settings page - red warning shield hidden (icon content blanked)");
            }
        }
    }
    return out;
}

// "Choose how to install updates" - group box title of the classic dialog.
static const wchar_t* SelectChooseHowToInstallUpdatesLabel() {
    static const std::unordered_map<std::wstring, const wchar_t*> kTexts = {
        { L"en", L"Choose how to install updates" },
        { L"it", L"Scegli come installare gli aggiornamenti" },
        { L"es", L"Elija cómo instalar las actualizaciones" },
        { L"fr", L"Choisissez comment installer les mises à jour" },
        { L"tr", L"Güncellemelerin nasıl yükleneceğini seçin" },
        { L"ru", L"Выберите способ установки обновлений" },
        { L"pt", L"Escolha como instalar as atualizações" },
        { L"zh", L"选择如何安装更新" },
        { L"pl", L"Wybierz sposób instalowania aktualizacji" },
        { L"nl", L"Kies hoe updates worden geïnstalleerd" },
    };
    auto it = kTexts.find(CurrentLanguage());
    if (it == kTexts.end()) it = kTexts.find(L"en");
    return it->second;
}

// Private command protocol handled by the ShellExecute hooks: the DirectUI
// NavigateButton of each classic option calls ShellExecuteExW with
// "wurestorer:auoptions=N"; we intercept it, write AUOptions and consume it
// (nothing is launched). Unknown wurestorer: commands are swallowed too.
// Forward declarations for the classic settings dialog (defined below).
static void ShowWuSettingsDialog(HWND parent);
static void RefreshWuPage(HWND host);
using ShellExecuteExW_t = BOOL(WINAPI*)(SHELLEXECUTEINFOW*);
static ShellExecuteExW_t ShellExecuteExWOriginal = nullptr;
static BOOL WINAPI ShellExecuteExWHook(SHELLEXECUTEINFOW* info) {
    if (info && info->lpFile &&
        wcsncmp(info->lpFile, kWuRestorerProtocol, wcslen(kWuRestorerProtocol)) == 0) {
        const wchar_t* p = info->lpFile + wcslen(kWuRestorerProtocol);
        if (wcsncmp(p, L"auoptions=", 10) == 0) {
            const int value = _wtoi(p + 10);
            if (value >= 1 && value <= 4) {
                WriteAuOptionsValue(static_cast<DWORD>(value));
                // The page hosting the option list re-renders so the selection
                // marker reflects the new AUOptions value.
                RefreshWuPage(info->hwnd);
                info->hInstApp = reinterpret_cast<HINSTANCE>(static_cast<UINT_PTR>(32));
                return TRUE;
            }
        } else if (wcscmp(p, L"opensettings") == 0) {
            // Open the classic settings dialog as an ADDITIONAL window on top
            // of the settings page (which stays open - we do not navigate away).
            ShowWuSettingsDialog(info->hwnd);
            info->hInstApp = reinterpret_cast<HINSTANCE>(static_cast<UINT_PTR>(32));
            return TRUE;
        }
        return TRUE; // consume unknown wurestorer: commands
    }
    return ShellExecuteExWOriginal(info);
}

using ShellExecuteW_t = HINSTANCE(WINAPI*)(HWND, LPCWSTR, LPCWSTR, LPCWSTR, LPCWSTR, INT);
static ShellExecuteW_t ShellExecuteWOriginal = nullptr;
static HINSTANCE WINAPI ShellExecuteWHook(HWND hwnd, LPCWSTR operation, LPCWSTR file,
                                          LPCWSTR parameters, LPCWSTR directory, INT show) {
    if (file && wcsncmp(file, kWuRestorerProtocol, wcslen(kWuRestorerProtocol)) == 0) {
        const wchar_t* p = file + wcslen(kWuRestorerProtocol);
        if (wcsncmp(p, L"auoptions=", 10) == 0) {
            const int value = _wtoi(p + 10);
            if (value >= 1 && value <= 4) WriteAuOptionsValue(static_cast<DWORD>(value));
        } else if (wcscmp(p, L"opensettings") == 0) {
            ShowWuSettingsDialog(hwnd);
        }
        return reinterpret_cast<HINSTANCE>(static_cast<UINT_PTR>(32));
    }
    return ShellExecuteWOriginal(hwnd, operation, file, parameters, directory, show);
}

// =============================================================================
// Classic "Change settings" dialog (Win32).
// -----------------------------------------------------------------------------
// Modeled on the classic-taskbar-properties mod: a real Win32 dialog built from
// an in-memory DLGTEMPLATE and shown with CreateDialogIndirectParamW - no
// DirectUI involved. It replaces the broken DirectUI pageSettings page: any
// navigation that would open shell:::{CLSID}\pageSettings is intercepted by the
// ShellExecute hooks above and this dialog is shown instead.
//
// The dialog offers a TrackBar (the "bar") to scroll between the four classic
// important-updates modes, plus the recommended-updates and Microsoft-products
// checkboxes. On OK the values are written to the registry (local AU key AND
// the policy key Windows 10/11 actually honour) and the Windows Update page is
// refreshed so it re-renders with the new state.
// =============================================================================
enum {
    kWuDlgSettings = 0x7701,
    kWuCtlCombo = 0x7710,
    kWuCtlOptionLabel = 0x7711,
    kWuCtlRecommended = 0x7712,
    kWuCtlMsProducts = 0x7713,
    kWuCtlAllUsers = 0x7714,
    kWuCtlNote = 0x7715,
};

static HWND g_wuSettingsDlg = nullptr;
static HWND g_wuSettingsParent = nullptr;
static DWORD g_wuDlgAuOptions = 4;
static bool g_wuDlgRecommended = false;
static bool g_wuDlgMsProducts = false;
static bool g_wuDlgAllUsers = false;

// True when the ShellExecute target is our applet's settings child page
// (shell:::{CLSID}\pageSettings), either as a bare shell: URI or as
// "%SystemRoot%\explorer.exe" + shellexecuteparams.
static const wchar_t* SelectRecommendedUpdatesLabel() {
    static const std::unordered_map<std::wstring, const wchar_t*> kTexts = {
        { L"en", L"Give me recommended updates the same way I receive important updates" },
        { L"it", L"Fornisci gli aggiornamenti consigliati nello stesso modo in cui ricevo gli aggiornamenti importanti" },
        { L"es", L"Proporcionar actualizaciones recomendadas de la misma manera que recibo las actualizaciones importantes" },
        { L"fr", L"Me donner les mises à jour recommandées de la même manière que les mises à jour importantes" },
        { L"tr", L"Önemli güncellemelerle aynı şekilde önerilen güncellemeleri de ver" },
        { L"ru", L"Предоставлять рекомендуемые обновления так же, как и важные" },
        { L"pt", L"Dar-me atualizações recomendadas da mesma forma que recebo as importantes" },
        { L"zh", L"以接收重要更新的相同方式为我提供推荐更新" },
        { L"pl", L"Zapewniaj zalecane aktualizacje w taki sam sposób, jak ważne" },
        { L"nl", L"Geef mij aanbevolen updates op dezelfde manier als belangrijke updates" },
    };
    auto it = kTexts.find(CurrentLanguage());
    if (it == kTexts.end()) it = kTexts.find(L"en");
    return it->second;
}

// Classic option text for an AUOptions value (1..4), multilingual via resstr.


static void ReadAuxAuValues(bool& recommended, bool& msProducts, bool& allUsers) {
    recommended = false;
    msProducts = false;
    allUsers = false;
    HKEY hKey = nullptr;
    if (RegOpenKeyExW(HKEY_LOCAL_MACHINE,
            L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\WindowsUpdate\\Auto Update",
            0, KEY_READ, &hKey) == ERROR_SUCCESS) {
        DWORD v = 0, sz = sizeof(v);
        if (RegQueryValueExW(hKey, L"IncludeRecommendedUpdates", nullptr, nullptr,
                             reinterpret_cast<LPBYTE>(&v), &sz) == ERROR_SUCCESS && v)
            recommended = true;
        sz = sizeof(v);
        if (RegQueryValueExW(hKey, L"MicrosoftUpdate", nullptr, nullptr,
                             reinterpret_cast<LPBYTE>(&v), &sz) == ERROR_SUCCESS && v)
            msProducts = true;
        sz = sizeof(v);
        if (RegQueryValueExW(hKey, L"AllowAllUsers", nullptr, nullptr,
                             reinterpret_cast<LPBYTE>(&v), &sz) == ERROR_SUCCESS && v)
            allUsers = true;
        RegCloseKey(hKey);
    }
}

static void WriteAuxAuValues(bool recommended, bool msProducts, bool allUsers) {
    HKEY hKey = nullptr;
    if (RegOpenKeyExW(HKEY_LOCAL_MACHINE,
            L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\WindowsUpdate\\Auto Update",
            0, KEY_SET_VALUE, &hKey) == ERROR_SUCCESS) {
        const DWORD rec = recommended ? 1 : 0;
        RegSetValueExW(hKey, L"IncludeRecommendedUpdates", 0, REG_DWORD,
                       reinterpret_cast<const BYTE*>(&rec), sizeof(rec));
        const DWORD ms = msProducts ? 1 : 0;
        RegSetValueExW(hKey, L"MicrosoftUpdate", 0, REG_DWORD,
                       reinterpret_cast<const BYTE*>(&ms), sizeof(ms));
        const DWORD au = allUsers ? 1 : 0;
        RegSetValueExW(hKey, L"AllowAllUsers", 0, REG_DWORD,
                       reinterpret_cast<const BYTE*>(&au), sizeof(au));
        RegCloseKey(hKey);
    }
}

// Asks the window hosting the Windows Update page to re-render (standard shell
// view refresh command), so the main page reflects the new AUOptions state.
// The DirectUI host may be a child of the shell view, so post to both the given
// window and its top-level ancestor.
static void RefreshWuPage(HWND host) {
    if (host && IsWindow(host)) {
        PostMessageW(host, WM_COMMAND, MAKEWPARAM(0xA220, 0), 0); // FCIDM_REFRESH
        HWND root = GetAncestor(host, GA_ROOT);
        if (root && root != host && IsWindow(root))
            PostMessageW(root, WM_COMMAND, MAKEWPARAM(0xA220, 0), 0);
    } else {
        ShellExecuteW(nullptr, L"open",
                      (L"shell:::" + std::wstring(kAppletClsid)).c_str(),
                      nullptr, nullptr, SW_SHOWNORMAL);
    }
}

static void SaveWuSettingsAndClose(HWND hwnd) {
    HWND hCombo = GetDlgItem(hwnd, kWuCtlCombo);
    int sel = hCombo ? static_cast<int>(SendMessageW(hCombo, CB_GETCURSEL, 0, 0)) : 0;
    DWORD pos = 4;
    if (sel == 0) pos = 4;
    else if (sel == 1) pos = 3;
    else if (sel == 2) pos = 2;
    else if (sel == 3) pos = 1;
    WriteAuOptionsValue(pos);
    const bool rec = IsDlgButtonChecked(hwnd, kWuCtlRecommended) == BST_CHECKED;
    const bool ms = IsDlgButtonChecked(hwnd, kWuCtlMsProducts) == BST_CHECKED;
    const bool au = IsDlgButtonChecked(hwnd, kWuCtlAllUsers) == BST_CHECKED;
    WriteAuxAuValues(rec, ms, au);
    HWND parent = g_wuSettingsParent;
    DestroyWindow(hwnd);
    RefreshWuPage(parent);
}

static std::wstring StripAmpersand(const wchar_t* s) {
    std::wstring out;
    if (!s) return out;
    for (const wchar_t* p = s; *p; ++p) {
        if (*p != L'&') out += *p;
    }
    return out;
}

static INT_PTR CALLBACK WuSettingsDlgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_INITDIALOG: {
            g_wuSettingsDlg = hwnd;
            HWND hCombo = GetDlgItem(hwnd, kWuCtlCombo);
            if (hCombo) {
                SendMessageW(hCombo, CB_RESETCONTENT, 0, 0);
                SendMessageW(hCombo, CB_ADDSTRING, 0, (LPARAM)WuOptionText(4));
                SendMessageW(hCombo, CB_ADDSTRING, 0, (LPARAM)WuOptionText(3));
                SendMessageW(hCombo, CB_ADDSTRING, 0, (LPARAM)WuOptionText(2));
                SendMessageW(hCombo, CB_ADDSTRING, 0, (LPARAM)WuOptionText(1));
                int sel = 0;
                if (g_wuDlgAuOptions == 4) sel = 0;
                else if (g_wuDlgAuOptions == 3) sel = 1;
                else if (g_wuDlgAuOptions == 2) sel = 2;
                else if (g_wuDlgAuOptions == 1) sel = 3;
                SendMessageW(hCombo, CB_SETCURSEL, sel, 0);
            }
            CheckDlgButton(hwnd, kWuCtlRecommended, g_wuDlgRecommended ? BST_CHECKED : BST_UNCHECKED);
            CheckDlgButton(hwnd, kWuCtlMsProducts, g_wuDlgMsProducts ? BST_CHECKED : BST_UNCHECKED);
            CheckDlgButton(hwnd, kWuCtlAllUsers, g_wuDlgAllUsers ? BST_CHECKED : BST_UNCHECKED);
            return TRUE;
        }
        case WM_CTLCOLORSTATIC: {
            // The privacy note is drawn in grey, like the original page.
            HDC hdc = reinterpret_cast<HDC>(wParam);
            HWND hCtl = reinterpret_cast<HWND>(lParam);
            if (hCtl == GetDlgItem(hwnd, kWuCtlNote)) {
                SetTextColor(hdc, RGB(90, 90, 90));
                SetBkColor(hdc, GetSysColor(COLOR_BTNFACE));
                return reinterpret_cast<INT_PTR>(GetSysColorBrush(COLOR_BTNFACE));
            }
            break;
        }
        case WM_COMMAND:
            switch (LOWORD(wParam)) {
                case IDOK:
                    SaveWuSettingsAndClose(hwnd);
                    return TRUE;
                case IDCANCEL:
                    DestroyWindow(hwnd);
                    return TRUE;
            }
            break;
        case WM_CLOSE:
            DestroyWindow(hwnd);
            return TRUE;
        case WM_DESTROY:
            if (g_wuSettingsDlg == hwnd) g_wuSettingsDlg = nullptr;
            return TRUE;
    }
    return FALSE;
}

static void ShowWuSettingsDialog(HWND parent) {
    if (g_wuSettingsDlg && IsWindow(g_wuSettingsDlg)) {
        SetForegroundWindow(g_wuSettingsDlg);
        return;
    }

    g_wuSettingsParent = parent;
    g_wuDlgAuOptions = ReadAuOptionsValue();
    ReadAuxAuValues(g_wuDlgRecommended, g_wuDlgMsProducts, g_wuDlgAllUsers);

    const int kControls = 10; // group, desc, label, combo, 3 checkboxes, note, OK, Cancel
    BYTE* buf = new (std::nothrow) BYTE[4096];
    if (!buf) return;
    BYTE* p = buf;
    auto align4 = [](BYTE*& ptr) { ptr = reinterpret_cast<BYTE*>((reinterpret_cast<UINT_PTR>(ptr) + 3) & ~static_cast<UINT_PTR>(3)); };

    LPDLGTEMPLATEW pDlg = reinterpret_cast<LPDLGTEMPLATEW>(p);
    pDlg->style = DS_SETFONT | DS_MODALFRAME | DS_CENTER | WS_POPUP | WS_CAPTION | WS_SYSMENU;
    pDlg->dwExtendedStyle = 0;
    pDlg->cdit = kControls;
    pDlg->x = 0; pDlg->y = 0;
    pDlg->cx = 380; pDlg->cy = 272;
    p += sizeof(DLGTEMPLATE);
    *(WORD*)p = 0; p += 2;                       // no menu
    *(WORD*)p = 0; p += 2;                       // no class
    *(WORD*)p = 0; p += 2;                       // empty title (set later)
    *(WORD*)p = 8; p += 2;                       // font point size
    const wchar_t kFont[] = L"Segoe UI";
    memcpy(p, kFont, sizeof(kFont)); p += sizeof(kFont);

    auto addCtrl = [&](DWORD style, DWORD exStyle, short x, short y, short cx, short cy,
                       WORD id, LPCWSTR cls, LPCWSTR cap) {
        align4(p);
        LPDLGITEMTEMPLATE pi = reinterpret_cast<LPDLGITEMTEMPLATE>(p);
        pi->style = WS_CHILD | WS_VISIBLE | style;
        pi->dwExtendedStyle = exStyle;
        pi->x = x; pi->y = y; pi->cx = cx; pi->cy = cy; pi->id = id;
        p += sizeof(DLGITEMTEMPLATE);
        const int clsLen = static_cast<int>(wcslen(cls));
        memcpy(p, cls, (clsLen + 1) * sizeof(wchar_t)); p += (clsLen + 1) * sizeof(wchar_t);
        const int capLen = static_cast<int>(wcslen(cap));
        memcpy(p, cap, (capLen + 1) * sizeof(wchar_t)); p += (capLen + 1) * sizeof(wchar_t);
        *(WORD*)p = 0; p += 2;                   // no creation data
    };

    // Translated texts (all with fallbacks so no label is ever empty)
    const std::wstring grp = SelectChooseHowToInstallUpdatesLabel();
    const std::wstring title = StripAmpersand(
        EmbeddedMuiString(351) ? EmbeddedMuiString(351) : L"Change settings");
    const std::wstring importantLabel = StripAmpersand(
        EmbeddedMuiString(1232) ? EmbeddedMuiString(1232) : L"Important updates");
    const wchar_t* descText = EmbeddedMuiString(1102);
    if (!descText) descText = L"When your PC is online, Windows can automatically check for important updates and install them using these settings.";
    const wchar_t* noteText = EmbeddedMuiString(1209);
    if (!noteText) noteText = L"Note: Windows Update might update itself automatically first when checking for other updates.";
    const wchar_t* allUsersText = EmbeddedMuiString(1001);
    if (!allUsersText) allUsersText = L"Allow all users to install updates on this computer";
    wchar_t okText[64] = L"OK";
    wchar_t cancelText[64] = L"Cancel";
    {
        HMODULE shell32 = GetModuleHandleW(L"shell32.dll");
        if (shell32) {
            LoadStringW(shell32, 800, okText, ARRAYSIZE(okText));       // IDS_OK
            LoadStringW(shell32, 801, cancelText, ARRAYSIZE(cancelText)); // IDS_CANCEL
        }
    }

    // --- Windows 7-style layout ---
    // "Choose how to install updates" group box with description + combobox.
    addCtrl(BS_GROUPBOX | WS_TABSTOP, 0, 8, 6, 352, 122, 0x7F00, L"Button", grp.c_str());
    addCtrl(SS_LEFT, 0, 18, 18, 332, 38, 0x7F01, L"Static", descText);              // description (wraps)
    addCtrl(SS_LEFT, 0, 18, 62, 240, 12, 0x7F02, L"Static", importantLabel.c_str()); // "Important updates:"
    addCtrl(CBS_DROPDOWNLIST | WS_VSCROLL | WS_TABSTOP, 0, 18, 76, 332, 120, kWuCtlCombo, L"ComboBox", L"");
    // Options below the group box (classic checkboxes)
    addCtrl(BS_AUTOCHECKBOX | WS_TABSTOP, 0, 16, 138, 344, 14, kWuCtlRecommended, L"Button", SelectRecommendedUpdatesLabel());
    addCtrl(BS_AUTOCHECKBOX | WS_TABSTOP, 0, 16, 156, 344, 14, kWuCtlMsProducts, L"Button",
            EmbeddedMuiString(475) ? EmbeddedMuiString(475) : L"Give me updates for other Microsoft products");
    addCtrl(BS_AUTOCHECKBOX | WS_TABSTOP, 0, 16, 174, 344, 14, kWuCtlAllUsers, L"Button", allUsersText);
    // Privacy note (grey)
    addCtrl(SS_LEFT, 0, 16, 194, 344, 28, kWuCtlNote, L"Static", noteText);
    // OK / Cancel - bottom right, classic size, always visible.
    addCtrl(BS_DEFPUSHBUTTON | WS_TABSTOP, 0, 248, 248, 60, 16, IDOK, L"Button", okText);
    addCtrl(BS_PUSHBUTTON | WS_TABSTOP, 0, 312, 248, 60, 16, IDCANCEL, L"Button", cancelText);

    HWND hwnd = CreateDialogIndirectParamW(GetModuleHandleW(nullptr),
                                           reinterpret_cast<LPDLGTEMPLATE>(buf),
                                           parent, WuSettingsDlgProc, 0);
    if (!hwnd) {
        Wh_Log(L"Windows Update Restorer: classic settings dialog creation FAILED (err=%u)", GetLastError());
    }
    delete[] buf;

    if (hwnd && IsWindow(hwnd)) {
        SetWindowTextW(hwnd, title.c_str());
        ShowWindow(hwnd, SW_SHOWNORMAL);
        SetForegroundWindow(hwnd);
    }
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
    // "Change settings" (351) opens the classic Win32 settings dialog: the
    // ShellExecute hooks intercept the pageSettings target and show it instead
    // of the broken DirectUI page.
    std::wstring cmd = BuildWuSidebarOpenCommandAttributes();
    if (stringId == 351) {
        cmd = L"shellexecute=\"shell:::" + std::wstring(kAppletClsid) + L"\\pageSettings\"";
    }
    row +=
        L"<NavigateButton layoutpos=\"left\" layout=\"flowlayout()\" " +
        cmd + L">"
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
static std::atomic<ULONGLONG> g_wuCheckedTick{static_cast<ULONGLONG>(-1)}; // -1 = never probed yet
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

static bool ProbeWindowsUpdateServiceAvailable() {
    // Called only by the setup worker; SCM RPC is not allowed on the render path.
    const ULONGLONG now = GetTickCount64();
    const ULONGLONG last = g_wuCheckedTick.load();
    if (last != static_cast<ULONGLONG>(-1) && now - last < kWuCheckIntervalMs)
        return g_wuAvailable.load();

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

// The native moduleAUNotConfigured warning ("Automatic updates are not
// configured for this computer") is driven by the Automatic Updates
// configuration (AUOptions/NoAutoUpdate), not by whether the wuauserv
// service happens to be running - wucltux re-shows/re-sizes that module at
// runtime after SetXML regardless of what the XML says, so fighting it in
// the XML patch never works (confirmed via raw XML dump: our patch is
// well-formed and still gets overridden). Instead, detect the same
// condition natively used for that box and mirror it: if AU is not
// configured, skip inserting the recreated hub entirely, exactly as if
// "Show recreated interface" were off - matching the native page and
// avoiding any conflict with the provider's own re-show logic.
static bool IsAutomaticUpdatesConfigured() {
    bool configured = true;  // default optimistic: assume configured unless proven otherwise

    // Group Policy override takes precedence when present.
    HKEY hPolicy = nullptr;
    if (RegOpenKeyExW(HKEY_LOCAL_MACHINE,
            L"SOFTWARE\\Policies\\Microsoft\\Windows\\WindowsUpdate\\AU", 0, KEY_READ, &hPolicy) == ERROR_SUCCESS) {
        DWORD noAutoUpdate = 0, size = sizeof(noAutoUpdate);
        if (RegQueryValueExW(hPolicy, L"NoAutoUpdate", nullptr, nullptr,
                             reinterpret_cast<LPBYTE>(&noAutoUpdate), &size) == ERROR_SUCCESS) {
            configured = (noAutoUpdate == 0);
        }
        RegCloseKey(hPolicy);
        return configured;
    }

    // Otherwise fall back to the local (non-policy) Automatic Updates setting.
    // AUOptions == 1 means "Never check for updates" (AU turned off), which is
    // exactly the state that makes wucltux show the native warning.
    HKEY hLocal = nullptr;
    if (RegOpenKeyExW(HKEY_LOCAL_MACHINE,
            L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\WindowsUpdate\\Auto Update", 0, KEY_READ, &hLocal) == ERROR_SUCCESS) {
        DWORD auOptions = 0, size = sizeof(auOptions);
        if (RegQueryValueExW(hLocal, L"AUOptions", nullptr, nullptr,
                             reinterpret_cast<LPBYTE>(&auOptions), &size) == ERROR_SUCCESS) {
            configured = (auOptions != 1);
        }
        RegCloseKey(hLocal);
    }
    return configured;
}

static bool IsWindowsUpdateServiceAvailable() {
    // Previously this returned a value cached once at mod startup
    // (GatherBackgroundStatus, called a single time from SetupWorker) and
    // never refreshed afterwards - so the recreated hub kept showing/hiding
    // based on whatever the service state was when Explorer/the mod first
    // loaded, regardless of later changes. ProbeWindowsUpdateServiceAvailable()
    // already self-throttles to one real SCM query per kWuCheckIntervalMs
    // (5s), so calling it directly here keeps the render path cheap while
    // actually reflecting the current state on every page (re)load.
    //
    // Additionally require Automatic Updates to actually be configured
    // (see IsAutomaticUpdatesConfigured): that is the real condition behind
    // the native moduleAUNotConfigured warning box, which wucltux re-shows
    // at runtime regardless of what our XML patch says. When AU is not
    // configured, skip the recreated hub entirely - same effect as if
    // "Show recreated interface" were off - instead of fighting a native
    // re-show we cannot suppress from the XML.
    // This function is called from DirectUI rendering. It must never make SCM RPC.
    const bool available = !g_cachedWuServiceProbed.load(std::memory_order_acquire) ||
                           g_cachedWuAvailable.load(std::memory_order_acquire);
    return available && IsAutomaticUpdatesConfigured();
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
// do not (yet) require a reboot. We do NOT treat mere existence of the Auto
// Update\Results\Download / \Install keys as "updates available" (they exist on
// essentially every install). Instead we require a real staged result: a non-empty
// LastSuccessTime or Result value recorded inside one of those keys. Only
// meaningful when Windows Update is available and no reboot is pending.
// Forward declaration (defined below).
static std::wstring ReadWuaResultString(const wchar_t* subkey, const wchar_t* valueName);

static bool ResultKeyHasStagedUpdate(const wchar_t* subkey) {
    const wchar_t* values[] = { L"LastSuccessTime", L"Success", L"Result" };
    for (const wchar_t* v : values) {
        if (!ReadWuaResultString(subkey, v).empty()) return true;
    }
    return false;
}

static bool IsUpdatesAvailable() {
    if (IsPendingWindowsUpdate()) return false;
    return ResultKeyHasStagedUpdate(L"Download") || ResultKeyHasStagedUpdate(L"Install");
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
                out.resize((size + sizeof(wchar_t) - 1) / sizeof(wchar_t));  // round up to a whole wchar
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
    std::lock_guard<std::mutex> lock(g_lastQueryTimeMutex);
    if (g_lastQueryTimeText.empty()) g_lastQueryTimeText = NowLocalTimeText();
    return g_lastQueryTimeText;
}

// Computes the "Updates were installed" value. This is the expensive part (it can
// block for seconds on a cold/unhealthy Windows Update datastore via QueryHistory),
// so it must run on a background thread, never on the Control Panel UI thread.
// The registry key is often missing on Windows 10, so we fall back to the WUA
// update history (most recent successful install), which is the reliable source.
static std::wstring ComputeLastInstallTime() {
    // 1) Registry (works on Windows 7/8).
    const wchar_t* values[] = { L"LastSuccessTime", L"Success", L"InstallTime" };
    for (const wchar_t* v : values) {
        std::wstring t = ReadWuaResultString(L"Install", v);
        if (!t.empty()) return t;
    }
    // 2) WUA history: most recent successful install. QueryHistory can block on
    // an unhealthy datastore, so never enter it once teardown has started.
    if (g_stopping.load()) return L"";
    std::vector<WuaHistoryEntry> history = GetUpdateHistory(200);
    if (g_stopping.load()) return L"";
    for (const auto& h : history) {
        if (h.resultCode == orcSucceeded && h.date > 0) {
            std::wstring s = FormatWuaDate(h.date);
            if (!s.empty()) return s;
        }
    }
    return L"";
}

// Returns the cached "Updates were installed" value computed on the background
// thread. Never does blocking work here, so it is safe to call from the render path.
static std::wstring LastInstallTimeText() {
    // Rendering must never start WUA work or wait for the worker's WUA call.
    // A blank value is neutral and the next render uses the completed cache.
    std::lock_guard<std::mutex> lock(g_statusMutex);
    return g_lastInstallComputed ? g_cachedLastInstall : std::wstring();
}

// Background gathering (called from the setup thread): probes the Windows Update
// service and pre-computes the update history, caching the results so the Control
// Panel UI thread never does blocking SCM/WUA work while rendering the page.
static void GatherBackgroundStatus() {
    // Probe the service (SCM RPC) and cache the outcome.
    const bool available = ProbeWindowsUpdateServiceAvailable();
    g_cachedWuAvailable.store(available);
    g_cachedWuServiceProbed.store(true);

    // Do the slow WUA work without the cache mutex: renderers can return their
    // neutral state immediately instead of waiting behind QueryHistory.
    if (g_stopping.load()) return;
    std::wstring lastInstall = ComputeLastInstallTime();
    if (g_stopping.load()) return;
    {
        std::lock_guard<std::mutex> lock(g_statusMutex);
        g_cachedLastInstall = std::move(lastInstall);
        g_lastInstallComputed = true;
    }
}

// =============================================================================
// Settings page ("pageSettings") patch — RESERVED FOR FUTURE USE.
// =============================================================================
// This block is intentionally NOT compiled right now (#if 0). It was an attempt
// to replace the non-functional scrolling "Important updates" module on the
// legacy "Change settings" child page with an update-history shortcut block
// (two blue links). It is kept in the source for future implementation and is
// NOT removed for simplicity: the code is simply unused NOW, not abandoned.
//
// Why it is disabled: the settings-page "Important updates" module is resolved
// by name from wucltux code-behind (atoms such as auOptionSelectorCombobox are
// looked up programmatically). Removing or hiding the module made the page fail
// to load ("impossibile caricare la pagina"). To avoid breaking the page, the
// mod currently leaves pageSettings completely untouched (see PatchModernWuPageXml).
// The helper functions below (FindModuleContaining, BuildWuSettingsReplacementXml,
// PatchWuSettingsPageXml) are kept here as a reference for a future, safe
// re-implementation. The translated strings 64540/64541/64542 in the MUI table
// are also reserved for that same feature.







// Keep every atom(module...) identifier that wucltux expects, but make the
// legacy module itself empty.  Removing an ID makes some versions of the
// provider return S_FALSE and reconstruct its stock UI, which is precisely how
// duplicate native/recreated panes appear.  This is deliberately applied only
// to the top-level WU page, never to pageSettings.
static void CollapseNativeWuModules(std::wstring& xml) {
    constexpr PCWSTR kPrefix = L"<element id=\"atom(module";
    size_t pos = 0;
    unsigned collapsed = 0;
    while ((pos = xml.find(kPrefix, pos)) != std::wstring::npos) {
        const size_t idEnd = xml.find(L"\"", pos + wcslen(L"<element id=\""));
        size_t elementEnd = 0;
        if (idEnd == std::wstring::npos || !FindElementEnd(xml, pos, elementEnd)) {
            pos += wcslen(kPrefix);
            continue;
        }

        // A stock module can also be a layout wrapper around other modules.
        // After our surface is inserted, collapsing such an ancestor would also
        // delete the recreated UI and leave the landing page completely empty.
        // Preserve every ancestor containing one of our markers, then continue
        // scanning so nested stock modules can still be collapsed individually.
        const size_t customHub = xml.find(L"atom(wuamodern_best_effort)", pos);
        const size_t customFallback = xml.find(L"atom(wuamodern_redbox_fallback)", pos);
        if ((customHub != std::wstring::npos && customHub < elementEnd) ||
            (customFallback != std::wstring::npos && customFallback < elementEnd)) {
            pos += wcslen(kPrefix);
            continue;
        }

        // Retain just id="atom(module...)". The retained atom prevents the
        // provider from falling back to its original template at runtime.
        const std::wstring idAttribute =
            xml.substr(pos + wcslen(L"<element "),
                       idEnd - (pos + wcslen(L"<element ")) + 1);
        const std::wstring empty = L"<element " + idAttribute +
            L" width=\"0rp\" height=\"0rp\" visible=\"false\"/>";
        xml.replace(pos, elementEnd - pos, empty);
        pos += empty.size();
        ++collapsed;
    }
    if (collapsed) {
        Wh_Log(L"Windows Update Restorer: suppressed %u native top-level WU module(s)", collapsed);
    }
}

// Applies the top-level Windows Update page XML patch. The settings child page
// (pageSettings) is intentionally left untouched to guarantee it always loads.
static std::wstring PatchModernWuPageXml(const std::wstring& input) {
    if (!IsWindowsUpdatePageXml(input)) {
        DestroySettingsCombobox();
        return input;
    }

    // Settings child page (pageSettings, UIFILE 125): embeds the native Win32 ComboBox
    if (input.find(L"atom(pageSettings)") != std::wstring::npos)
        return PatchSettingsPageXml(input);

    // Strictly isolate the ComboBox: destroy it immediately on any other page
    DestroySettingsCombobox();

    // Restore/normalize the Windows 7/8.1-style left navigation pane for the
    // top-level Windows Update page, including fallback layouts.
    std::wstring withNavPane = PatchWuNavigationPaneXml(input);

    if (withNavPane.find(L"wuamodern_best_effort") != std::wstring::npos)
        return withNavPane;

    // Place the new hub *after* the legacy warning module, in the normal white
    // document area. It deliberately does not alter the red legacy card. The
    // same anchor is reused below when only the red box is shown.
    const std::wstring module = L"<element id=\"atom(moduleAUNotConfigured)\"";
    const size_t moduleStart = withNavPane.find(module);

    const bool wuAvailable = IsWindowsUpdateServiceAvailable();

    // If Windows Update is unavailable (service disabled/uninstalled or AU not
    // configured), the recreated hub must disappear entirely and dynamically -
    // not via a manual setting toggle. The user only sees the red warning box,
    // so we replace it with our own faithful recreation that adds the blue link
    // to the Windows Update settings page directly below it.
    //
    // We cannot attach the link to the native module: wucltux re-shows/re-sizes
    // moduleAUNotConfigured at runtime and overrides whatever XML is added
    // inside it, and re-appending it pushes any sibling after it below the box.
    // Instead, collapse the native module to a zero-size element (its atom stays
    // resolvable, so no S_FALSE and no provider fallback re-materialization -
    // the same trick the hub path uses when the service is available) and render
    // our self-contained red box + link module (BuildRedBoxFallbackXml), which
    // wucltux does not touch: the link reliably stays directly under the box.
    if (!wuAvailable) {
        if (moduleStart == std::wstring::npos) return withNavPane;

        // Remove the broken legacy "Check for updates for your PC" red box
        // (moduleCheckForUpdates) when the setting is enabled: with the service
        // stopped its "Check for updates" button cannot work, and it would
        // duplicate the box shown below. Collapsing it to a zero-size element
        // keeps the atom resolvable (no S_FALSE) and renders nothing, exactly
        // like the moduleAUNotConfigured collapse above/below.
        std::wstring patched = withNavPane;
        if (g_removeLegacyBrokenOption.load()) {
            const std::wstring checkModule = L"<element id=\"atom(moduleCheckForUpdates)\"";
            const size_t checkStart = patched.find(checkModule);
            if (checkStart != std::wstring::npos) {
                size_t checkEnd = 0;
                if (FindElementEnd(patched, checkStart, checkEnd)) {
                    const std::wstring emptiedCheck =
                        L"<element id=\"atom(moduleCheckForUpdates)\" width=\"0rp\" height=\"0rp\" visible=\"false\"/>";
                    patched.replace(checkStart, checkEnd - checkStart, emptiedCheck);
                    Wh_Log(L"Windows Update Restorer: removed broken legacy 'Check for updates' red box (RemoveLegacyBrokenOption)");
                }
            }
        }

        // Always render our self-contained "Turn on automatic updating" box
        // recreation - both with "Show recreated interface" ON and OFF. The
        // native module (moduleAUNotConfigured) is collapsed to a zero-size
        // element (atom stays resolvable, no S_FALSE) because the provider
        // re-shows/re-sizes it at runtime and it is unreliable on modern
        // builds. With the recreated interface ON the blue settings link is
        // included below the box; with it OFF the box is shown without the
        // link (the user explicitly wants the "Turn on automatic updating"
        // box to be present in this state too).
        // moduleCheckForUpdates can appear before moduleAUNotConfigured. If it
        // was collapsed above, the replacement changed the XML length, so the
        // moduleStart offset computed from withNavPane is now stale. Re-find the
        // AU module in the modified string before replacing/inserting; otherwise
        // the recreated surface can be skipped (or the wrong element parsed).
        const size_t currentModuleStart = patched.find(module);
        if (currentModuleStart == std::wstring::npos) return patched;
        size_t moduleEnd = 0;
        if (!FindElementEnd(patched, currentModuleStart, moduleEnd)) return patched;
        const std::wstring emptiedModule =
            L"<element id=\"atom(moduleAUNotConfigured)\" width=\"0rp\" height=\"0rp\" visible=\"false\"/>";
        patched.replace(currentModuleStart, moduleEnd - currentModuleStart, emptiedModule);
        const size_t insertAt = currentModuleStart + emptiedModule.size();
        patched.insert(insertAt, BuildRedBoxFallbackXml(g_showServiceNotice.load()));
        // The replacement is the sole visible status surface. Suppress every
        // remaining stock module as well, not merely the two known red boxes.
        CollapseNativeWuModules(patched);
        Wh_Log(L"Windows Update Restorer: replaced native red box with recreation (link=%d)",
               static_cast<int>(g_showServiceNotice.load()));
        return patched;
    }

    // Only inject anything if the user has the notice enabled. Keep the sidebar
    // patch even when the recreated status box itself is disabled. (The
    // "service unavailable" case above already returned with the broken legacy
    // box removed when the setting is on.)
    if (!g_showServiceNotice.load()) return withNavPane;

    if (moduleStart == std::wstring::npos) return withNavPane;

    std::wstring patched = withNavPane;
    size_t insertAt = moduleStart;
    if (wuAvailable) {
        // When Windows Update is available and updates are applied, the native red
        // "automatic updates are off" box (moduleAUNotConfigured) is misleading and
        // must be suppressed. The DirectUI provider re-shows modules at runtime, so
        // simply adding visible="false" in XML is overridden and does NOT hide it.
        //
        // Fully erasing the <element>...</element> (previous approach) removes the
        // atom(moduleAUNotConfigured) id from the tree entirely. On Windows 11 24H2
        // the provider apparently still expects to resolve that id while finishing
        // its own setup pass; when it can't, DUISetXML returns S_FALSE (hr=1)
        // instead of S_OK, and the provider's fallback is to re-materialize the
        // native red module from its own internal template - producing the legacy
        // box duplicated alongside our recreated hub.
        //
        // Fix: keep the id present but collapse the element to an empty,
        // self-closing, zero-sized node. The atom still resolves (no S_FALSE,
        // no fallback re-show), but there is nothing left to render.
        size_t moduleEnd = 0;
        if (!FindElementEnd(patched, moduleStart, moduleEnd)) return input;
        const std::wstring emptiedModule =
            L"<element id=\"atom(moduleAUNotConfigured)\" width=\"0rp\" height=\"0rp\" visible=\"false\"/>";
        patched.replace(moduleStart, moduleEnd - moduleStart, emptiedModule);
        insertAt = moduleStart + emptiedModule.size();
    }

    std::wstring hub;
    {
        // Windows Update is available (already guaranteed above): replicate the
        // classic Windows 7 header.
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
    }

    patched.insert(insertAt, hub);
    // The recreated hub owns the top-level content. Retain native atom IDs for
    // provider compatibility, but never let their original visual modules render.
    CollapseNativeWuModules(patched);
    return patched;
}

// =============================================================================
// Windows 7-Style Custom Task Pane Section for Control Panel Sidebar
// =============================================================================










static HRESULT WU_DUI_THISCALL DUISetXMLHook(void* parser, const WCHAR* xml,
                                              HINSTANCE resourceModule,
                                              HINSTANCE hInstance) {
    if (!DUISetXMLOriginal) return E_FAIL;
    if (g_inWuXmlPatch || !xml) {
        return DUISetXMLOriginal(parser, xml, resourceModule, hInstance);
    }
    std::wstring patched = PatchModernWuPageXml(xml);
    if (patched == xml) return DUISetXMLOriginal(parser, xml, resourceModule, hInstance);
    Wh_Log(L"Windows Update Restorer: modern WUA links injected through SetXML");
    WuXmlPatchGuard guard;
    HRESULT hr = DUISetXMLOriginal(parser, patched.c_str(), resourceModule, hInstance);
    if (patched.find(L"atom(pageSettings)") != std::wstring::npos) {
        InitializeNativeSettingsCombobox(nullptr);
    } else {
        DestroySettingsCombobox();
    }
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
    if (patched == xml) return DUISetXMLFromResourceOriginal(parser, resourceName, resourceType, resourceModule,
                                                              hInstance1, hInstance2);
    WuXmlPatchGuard guard;
    const HRESULT hr = DUISetXMLOriginal(parser, patched.c_str(), reinterpret_cast<HINSTANCE>(resourceModule), hInstance1);
    if (patched.find(L"atom(pageSettings)") != std::wstring::npos) {
        InitializeNativeSettingsCombobox(nullptr);
    } else {
        DestroySettingsCombobox();
    }
    Wh_Log(L"Windows Update Restorer: modern WUA command links injected (hr=0x%08X, patchedLen=%zu, origLen=%zu, wuAvailable=%d)",
           static_cast<unsigned>(hr), patched.size(), xml.size(), static_cast<int>(IsWindowsUpdateServiceAvailable()));
    if (hr == S_FALSE) {
        Wh_Log(L"Windows Update Restorer: WARNING - SetXML returned S_FALSE, provider may reject/re-show native modules");
    }
    return hr;
}

static void InstallModernWuXmlPatchHook() {
    HMODULE dui70 = GetModuleHandleW(L"dui70.dll");
    if (!dui70) dui70 = LoadLibraryExW(L"dui70.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!dui70) return;
    for (const char* name : {
#ifdef _WIN64
#endif
             "?SetXML@DUIXmlParser@DirectUI@@QEAAJPEBGPEAUHINSTANCE__@@1@Z",

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
#endif
             "?_SetXMLFromResource@DUIXmlParser@DirectUI@@IEAAJPEBG0PEAUHINSTANCE__@@11@Z",

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

// Forward declaration (defined below, in the icon-file helper section).
static std::wstring EnsureAppletLogoIconFile(bool windows81Skin);

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
        // Private "wurestorer:" command protocol for the classic option
        // selector on the settings page (DirectUI NavigateButton -> ShellExecute).
        if (void* p = reinterpret_cast<void*>(GetProcAddress(shell32, "ShellExecuteExW"))) {
            WindhawkUtils::SetFunctionHook(reinterpret_cast<ShellExecuteExW_t>(p),
                                           ShellExecuteExWHook,
                                           &ShellExecuteExWOriginal);
        }
        if (void* p = reinterpret_cast<void*>(GetProcAddress(shell32, "ShellExecuteW"))) {
            WindhawkUtils::SetFunctionHook(reinterpret_cast<ShellExecuteW_t>(p),
                                           ShellExecuteWHook,
                                           &ShellExecuteWOriginal);
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


// Cheap allocation-free filter for the RegOpenKey* hook hot path.
static bool ContainsRelevantKeywordInsensitive(const std::wstring& path) {
    static const wchar_t* needles[] = { L"clsid", L"controlpanel" };
    for (size_t i = 0; i < path.size(); ++i) {
        for (const auto* needle : needles) {
            size_t j = 0;
            while (needle[j] && i + j < path.size() && towlower(path[i + j]) == needle[j]) ++j;
            if (!needle[j]) return true;
        }
    }
    return false;
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
    bool IsFakeAndGetPath(HKEY key, std::wstring& path) const {
        if (auto root = RootPath(key); !root.empty()) { path = std::move(root); return false; }
        std::shared_lock lock(mutex_);
        const auto found = paths_.find(key);
        path = found == paths_.end() ? std::wstring() : found->second;
        return fake_.count(key) != 0;
    }
    HKEY CreateFake(const std::wstring& path) {
        std::unique_ptr<int> owner(new (std::nothrow) int(1));
        if (!owner) return nullptr;
        HKEY key = reinterpret_cast<HKEY>(owner.get());
        std::unique_lock lock(mutex_);
        paths_[key] = path;
        fake_[key] = std::move(owner);
        return key;
    }
    void Track(HKEY key, const std::wstring& path) {
        if (!key || IsRootKey(key) || !ContainsRelevantKeywordInsensitive(path)) return;
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
        // Windows 11 25H2 no longer reliably expands %SystemRoot% in a cpltasks
        // command before splitting its executable and arguments. Use the normal
        // PATH-resolved explorer command and an explicit shell namespace target.
        L"            <sh:command>explorer.exe shell:::" + std::wstring(kAppletClsid) + L"</sh:command>\r\n"
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

// Guard + cached path so the tasks-XML file is written at most once, even though
// the hook may ask for System.Software.TasksFileUrl concurrently from multiple
// Explorer windows/threads. Writes happen under a mutex (like the applet logo
// file); after the first success we just return the cached path.
static std::mutex g_tasksXmlMutex;
static std::wstring g_tasksXmlPath;
static std::wstring EnsureControlPanelTasksXmlFile() {
    std::lock_guard<std::mutex> lock(g_tasksXmlMutex);
    if (!g_tasksXmlPath.empty()) return g_tasksXmlPath;
    const std::wstring dir = StoreDir();
    if (dir.empty()) return L"";
    const std::wstring path = dir + L"\\" + kAppletTasksXmlFileName;
    if (!WriteUtf8TextFile(path, BuildControlPanelTasksXml())) {
        Wh_Log(L"Windows Update Restorer: failed to write Control Panel task links XML (err=%u)",
               GetLastError());
        return L"";
    }
    g_tasksXmlPath = path;
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
    if (!out) return ERROR_INVALID_PARAMETER;
    try {
        std::wstring full;
        const bool isFake = g_keys.IsFakeAndGetPath(key, full);
        if (subKey && *subKey) { if (!full.empty()) full += L"\\"; full += subKey; }
        if (isFake) {
            if (!IsTarget(full)) return ERROR_FILE_NOT_FOUND;
            if (WantsWrite(access)) return ERROR_ACCESS_DENIED;
            HKEY fake = g_keys.CreateFake(full);
            if (!fake) return ERROR_OUTOFMEMORY;
            *out = fake;
            return ERROR_SUCCESS;
        }
        LSTATUS status = RegOpenKeyExWOriginal(key, subKey, options, access, out);
        if (status == ERROR_SUCCESS && *out) {
            try { g_keys.Track(*out, full); }
            catch (...) { Wh_Log(L"Windows Update Restorer: unable to track opened registry key"); }
        } else if (status == ERROR_FILE_NOT_FOUND && IsTarget(full)) {
            if (WantsWrite(access)) return ERROR_ACCESS_DENIED;
            HKEY fake = g_keys.CreateFake(full);
            if (!fake) return ERROR_OUTOFMEMORY;
            *out = fake;
            return ERROR_SUCCESS;
        }
        return status;
    } catch (...) {
        // Never unwind a C++ exception through a foreign registry API caller.
        return ERROR_NOT_ENOUGH_MEMORY;
    }
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

    std::wstring path;
    const bool isFake = g_keys.IsFakeAndGetPath(key, path);
    const std::wstring name = valueName ? valueName : L"";
    LSTATUS result = ERROR_FILE_NOT_FOUND;
    if (!path.empty() && ProvideValue(path, name, type, data, bytes, result)) return result;
    if (isFake) return ERROR_FILE_NOT_FOUND;
    return RegQueryValueExWOriginal(key, valueName, reserved, type, data, bytes);
}
static LSTATUS WINAPI RegGetValueWHook(HKEY key, LPCWSTR subKey, LPCWSTR valueName, DWORD flags,
                                       LPDWORD type, PVOID data, LPDWORD bytes) {

    std::wstring path;
    const bool isFake = g_keys.IsFakeAndGetPath(key, path);
    if (subKey && *subKey) { if (!path.empty()) path += L"\\"; path += subKey; }
    LSTATUS result = ERROR_FILE_NOT_FOUND;
    if (!path.empty() && ProvideValue(path, valueName ? valueName : L"", type,
                                      static_cast<LPBYTE>(data), bytes, result)) return result;
    if (isFake) return ERROR_FILE_NOT_FOUND;
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
    std::wstring keyPath;
    const bool isFake = g_keys.IsFakeAndGetPath(key, keyPath);
    if (isFake) {
        std::wstring sub;
        if (!VirtualSubkey(Classify(keyPath), index, sub)) return ERROR_NO_MORE_ITEMS;
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

    std::wstring keyPath;
    const bool isFake = g_keys.IsFakeAndGetPath(key, keyPath);
    if (isFake) {
        Node node = Classify(keyPath);
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
        g_builtLanguage = CurrentLanguage();
    }

    g_verified.store(true, std::memory_order_release);
    Wh_Log(L"Windows Update Restorer ready: verified Windows 8.1 wucltux.dll loaded privately");

    // Gather status (SCM probe + update history) on this worker thread so the
    // Control Panel UI thread never blocks on it during page rendering.
    if (!g_stopping.load()) GatherBackgroundStatus();
}


BOOL Wh_ModInit() {
    try {
        // TrackBar (and common controls) support for the classic settings dialog.
        INITCOMMONCONTROLSEX icc = { sizeof(icc), ICC_BAR_CLASSES };
        InitCommonControlsEx(&icc);

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

        // Install the DirectUI Element::SetEnabled hook early (before any page
        // XML is parsed) so the settings combobox disable is intercepted at the
        // source from the very first render. Idempotent; a no-op for every
        // control that is not the protected combobox.
        EnsureSetEnabledHookInstalled();

        // The requested actions live in the host Control Panel navigation pane.
        // Prepare shutdown signalling before either worker starts.
        g_stopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
        g_stopping.store(false);
        // Only patch the safe WUAppPage content anchor (never the outer Control Panel host).
        InstallModernWuXmlPatchHook();
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
    const std::wstring oldLanguage = CurrentLanguage();
    LoadLanguageSetting();
    UpdateSettingsComboboxLanguage();
    if (oldLanguage != CurrentLanguage()) {
        // Ensure a previous rebuild (if any) has finished before starting a new
        // one, so we never run two builds concurrently.
        if (g_rebuildThread && g_rebuildThread->joinable()) g_rebuildThread->join();
        g_rebuildThread.reset();
        g_rebuildThread.emplace([] {
            RebuildEmbeddedMuiForLanguage();
            UpdateSettingsComboboxLanguage();
        });
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
    // Modeless dialog proc and all subclass callbacks are mod code: make them
    // unreachable before Windhawk unloads this image.
    if (g_wuSettingsDlg && IsWindow(g_wuSettingsDlg)) DestroyWindow(g_wuSettingsDlg);
    g_wuSettingsDlg = nullptr;
    DestroySettingsCombobox();
    std::vector<HWND> sidebars;
    { std::lock_guard lock(g_subclassWindowsMutex); sidebars = g_sidebarSubclassWindows; }
    for (HWND hwnd : sidebars) if (IsWindow(hwnd))
        SendMessageW(hwnd, g_wmUiTeardown, 0, 0);
    { std::lock_guard lock(g_subclassWindowsMutex); g_sidebarSubclassWindows.clear(); }
    g_stopping.store(true);
    if (g_stopEvent) SetEvent(g_stopEvent);
    CloseActiveDownloadHandles();
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
    if (g_legacyWarningShield) { DestroyIcon(g_legacyWarningShield); g_legacyWarningShield = nullptr; }
    if (g_wuDisabledShieldIcon) { DestroyIcon(g_wuDisabledShieldIcon); g_wuDisabledShieldIcon = nullptr; }
    ShutdownGdiPlusRendering();
    g_keys.AbandonAll();
    { std::lock_guard lock(g_injectionMutex); g_injected.clear(); }
}
