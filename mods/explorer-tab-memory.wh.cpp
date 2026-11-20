// ==WindhawkMod==
// @id              explorer-tab-memory
// @name            Explorer Tab Memory Saver
// @description     Restores your tabs and active folder when you reopen File Explorer.
// @version         0.3.0
// @author          Maxxwell Quinn
// @github          https://github.com/MXXQN
// @license         MIT
// @include         windhawk.exe
// @compilerOptions -lole32 -loleaut32 -lshell32 -luuid -ladvapi32 -lcrypt32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Explorer Tab Memory Saver

Close the last File Explorer window with its tabs still open. The next time
you open Explorer to Home or This PC, the mod reopens the saved folders as tabs
and selects the one you were using. If you open a specific folder instead,
that folder stays selected alongside the restored tabs.

Windows supplies the tab order, which can differ from an order you set by
dragging tabs. Each Windows user has a separate saved session.

This mod restores the last window with Explorer's own tab commands. The
similarly named [Explorer Tabs Session Saver](https://windhawk.net/mods/explorer-tabs-session-saver)
uses shell launches and recommends a companion utility for grouping tabs.
Enable only one session-restoration mod at a time.

Requires Windows 11 File Explorer with native tabs. Tested on x64 build 26300.9974.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- accountSid: ""
  $name: Windows account SID
  $description: Leave empty for all accounts. To limit the mod to one account, enter the SID shown by whoami /user.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <sddl.h>
#include <wincrypt.h>
#include <wil/resource.h>
#include <shlobj.h>
#include <exdisp.h>
#include <wrl/client.h>
#include <algorithm>
#include <string>
#include <vector>

using Microsoft::WRL::ComPtr;
using Locations = std::vector<std::wstring>;
static constexpr wchar_t sessionKey[] = L"Software\\ExplorerTabMemory";
static constexpr UINT newTabCommand = 0xA21B;
static constexpr UINT selectTabByIndexCommand = 0xA221;
static HANDLE stopEvent, worker;

static bool hasClass(HWND window, PCWSTR name) {
    wchar_t value[64]{};
    return GetClassNameW(window, value, ARRAYSIZE(value)) && !wcscmp(value, name);
}

struct Tab {
    ComPtr<IUnknown> identity;
    ComPtr<IShellBrowser> browser;
    std::wstring path;
    HWND handle{};
};

static std::wstring location(IShellBrowser* browser) {
    ComPtr<IShellView> view;
    ComPtr<IFolderView> folderView;
    ComPtr<IPersistFolder2> folder;
    if (FAILED(browser->QueryActiveShellView(&view)) || FAILED(view.As(&folderView)) ||
        FAILED(folderView->GetFolder(IID_PPV_ARGS(&folder)))) return {};
    PIDLIST_ABSOLUTE id = nullptr;
    if (FAILED(folder->GetCurFolder(&id)) || !id) return {};
    PWSTR name = nullptr;
    SHGetNameFromIDList(id, SIGDN_DESKTOPABSOLUTEPARSING, &name);
    std::wstring result = name ? name : L"";
    CoTaskMemFree(name);
    CoTaskMemFree(id);
    return result;
}

static size_t tabCount(HWND frame) {
    size_t count = 0;
    HWND child = nullptr;
    while ((child = FindWindowExW(frame, child, L"ShellTabWindowClass", nullptr))) ++count;
    return count;
}

// Only accept a complete snapshot: COM registration and the tab HWNDs can
// appear or disappear in different orders while Explorer is building its UI.
static std::vector<Tab> readTabs(IShellWindows* windows, HWND frame,
                                 const std::vector<Tab>* known = nullptr) {
    std::vector<Tab> result;
    long count = 0;
    if (FAILED(windows->get_Count(&count))) return {};
    for (long i = 0; i < count; ++i) {
        VARIANT index{};
        index.vt = VT_I4;
        index.lVal = i;
        ComPtr<IDispatch> entry;
        ComPtr<IServiceProvider> services;
        ComPtr<IShellBrowser> browser;
        ComPtr<IWebBrowser2> web;
        ComPtr<IUnknown> identity;
        SHANDLE_PTR owner = 0;
        if (windows->Item(index, &entry) != S_OK || !entry || FAILED(entry.As(&identity))) continue;
        // When waiting for one new tab, only resolve that tab's shell folder.
        if (known) {
            auto cached = std::find_if(known->begin(), known->end(), [&](const Tab& tab) {
                return tab.identity.Get() == identity.Get();
            });
            if (cached != known->end() && cached->handle && GetParent(cached->handle) == frame) {
                result.push_back(*cached);
                continue;
            }
        }
        if (FAILED(entry.As(&web)) || FAILED(web->get_HWND(&owner)) ||
            reinterpret_cast<HWND>(owner) != frame) continue;
        if (FAILED(entry.As(&services)) ||
            FAILED(services->QueryService(SID_STopLevelBrowser, IID_PPV_ARGS(&browser)))) return {};
        auto path = location(browser.Get());
        if (path.empty()) return {};
        HWND handle = nullptr;
        if (FAILED(browser->GetWindow(&handle)) || !handle || GetParent(handle) != frame ||
            !hasClass(handle, L"ShellTabWindowClass")) handle = nullptr;
        result.push_back({identity, browser, std::move(path), handle});
    }
    if (tabCount(frame) != result.size()) return {};
    return result;
}

static bool equalPath(const std::wstring& a, const std::wstring& b) {
    return CompareStringOrdinal(a.c_str(), -1, b.c_str(), -1, TRUE) == CSTR_EQUAL;
}

constexpr size_t maxSessionBytes = 4 * 1024 * 1024;
constexpr size_t maxEncryptedBytes = maxSessionBytes + 64 * 1024;
constexpr size_t maxTabs = 4096;
constexpr size_t maxParsingName = 32767;

static std::wstring sessionFilePath() {
    alignas(TOKEN_USER) BYTE user[sizeof(TOKEN_USER) + SECURITY_MAX_SID_SIZE]{};
    DWORD length = 0;
    if (!GetTokenInformation(GetCurrentProcessToken(), TokenUser, user, sizeof(user), &length)) return {};
    wil::unique_hlocal_string sid;
    if (!ConvertSidToStringSidW(reinterpret_cast<TOKEN_USER*>(user)->User.Sid, &sid)) return {};
    std::vector<wchar_t> directory(32768);
    if (!Wh_GetModStoragePath(directory.data(), directory.size())) return {};
    return std::wstring(directory.data()) + L"\\session-" + sid.get() + L".bin";
}

// Both the old registry format and the new payload end with a MULTI_SZ.
// Bound every scan by the size of the input, including its final empty string.
static Locations parseFolders(PCWSTR text, size_t chars) {
    Locations paths;
    while (chars > 1) {
        const auto end = std::find(text, text + chars, L'\0');
        size_t length = static_cast<size_t>(end - text);
        if (!length || length == chars || length > maxParsingName || paths.size() == maxTabs) return {};
        paths.emplace_back(text, length);
        chars -= length + 1;
        text = end + 1;
    }
    if (chars != 1 || *text != L'\0') return {};
    return paths;
}

static Locations readSnapshot(HANDLE file, DWORD& activeIndex) {
    LARGE_INTEGER size;
    if (!GetFileSizeEx(file, &size) || size.QuadPart <= 0 || size.QuadPart > maxEncryptedBytes) return {};
    std::vector<BYTE> encrypted(static_cast<size_t>(size.QuadPart));
    DWORD read = 0;
    if (!ReadFile(file, encrypted.data(), static_cast<DWORD>(encrypted.size()), &read, nullptr) ||
        read != encrypted.size()) return {};
    DATA_BLOB input{read, encrypted.data()}, plain{};
    if (!CryptUnprotectData(&input, nullptr, nullptr, nullptr, nullptr, CRYPTPROTECT_UI_FORBIDDEN, &plain)) return {};
    wil::unique_hlocal_ptr<BYTE> cleanup(plain.pbData);
    // Version 1: four UTF-16 characters, a 16-bit tab index, and the folders.
    if (plain.cbData < 8 * sizeof(wchar_t) || plain.cbData > maxSessionBytes ||
        plain.cbData % sizeof(wchar_t)) return {};
    PCWSTR text = reinterpret_cast<PCWSTR>(plain.pbData);
    if (wmemcmp(text, L"ETM1", 4)) return {};
    auto paths = parseFolders(text + 5, plain.cbData / sizeof(wchar_t) - 5);
    if (text[4] >= paths.size()) return {};
    activeIndex = text[4];
    return paths;
}

static Locations loadSession(PCWSTR path, DWORD& activeIndex) {
    wil::unique_hfile file(CreateFileW(path, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_DELETE, nullptr,
        OPEN_EXISTING, FILE_FLAG_OPEN_REPARSE_POINT, nullptr));
    return file ? readSnapshot(file.get(), activeIndex) : Locations{};
}

static bool saveSession(PCWSTR path, const Locations& paths, DWORD activeIndex) {
    if (!*path || paths.empty() || paths.size() > maxTabs || activeIndex >= paths.size()) return false;
    std::vector<wchar_t> data{L'E', L'T', L'M', L'1', static_cast<wchar_t>(activeIndex)};
    for (const auto& folder : paths) {
        if (folder.empty() || folder.size() > maxParsingName || folder.find(L'\0') != folder.npos ||
            data.size() + folder.size() + 2 > maxSessionBytes / sizeof(wchar_t)) return false;
        data.insert(data.end(), folder.c_str(), folder.c_str() + folder.size() + 1);
    }
    data.push_back(0);
    DATA_BLOB plain{static_cast<DWORD>(data.size() * sizeof(wchar_t)), reinterpret_cast<BYTE*>(data.data())};
    DATA_BLOB encrypted{};
    // Windhawk can share its mod directory between Windows accounts.
    if (!CryptProtectData(&plain, nullptr, nullptr, nullptr, nullptr, CRYPTPROTECT_UI_FORBIDDEN, &encrypted)) return false;
    wil::unique_hlocal_ptr<BYTE> cleanup(encrypted.pbData);
    if (encrypted.cbData > maxEncryptedBytes) return false;

    GUID guid;
    wchar_t suffix[40];
    if (FAILED(CoCreateGuid(&guid)) || !StringFromGUID2(guid, suffix, ARRAYSIZE(suffix))) return false;
    auto temporary = std::wstring(path) + suffix;
    wil::unique_hfile file(CreateFileW(temporary.c_str(), GENERIC_WRITE, 0, nullptr, CREATE_NEW,
        FILE_ATTRIBUTE_NORMAL, nullptr));
    if (!file) return false;
    DWORD written = 0;
    bool complete = WriteFile(file.get(), encrypted.pbData, encrypted.cbData, &written, nullptr) &&
                    written == encrypted.cbData && FlushFileBuffers(file.get());
    file.reset();
    if (complete && MoveFileExW(temporary.c_str(), path, MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH)) return true;
    DeleteFileW(temporary.c_str());
    return false;
}

static void consumeSession(PCWSTR path, const Locations& expected, DWORD activeIndex) {
    // Delete only the snapshot we consumed, even if this user has two logons.
    wil::unique_hfile file(CreateFileW(path, GENERIC_READ | DELETE, FILE_SHARE_READ, nullptr,
        OPEN_EXISTING, FILE_FLAG_OPEN_REPARSE_POINT, nullptr));
    DWORD index = 0;
    if (!file || readSnapshot(file.get(), index) != expected || index != activeIndex) return;
    FILE_DISPOSITION_INFO remove{TRUE};
    SetFileInformationByHandle(file.get(), FileDispositionInfo, &remove, sizeof(remove));
}

static Locations loadLegacySession(PCWSTR key, DWORD& index) {
    DWORD bytes = 0;
    if (RegGetValueW(HKEY_CURRENT_USER, key, L"Folders", RRF_RT_REG_MULTI_SZ, nullptr, nullptr, &bytes) ||
        bytes < 3 * sizeof(wchar_t) || bytes > maxSessionBytes || bytes % sizeof(wchar_t)) return {};
    std::vector<wchar_t> data(bytes / sizeof(wchar_t));
    if (RegGetValueW(HKEY_CURRENT_USER, key, L"Folders", RRF_RT_REG_MULTI_SZ, nullptr, data.data(), &bytes) ||
        bytes % sizeof(wchar_t)) return {};
    auto paths = parseFolders(data.data(), bytes / sizeof(wchar_t));
    DWORD size = sizeof(index);
    if (RegGetValueW(HKEY_CURRENT_USER, key, L"ActiveTab", RRF_RT_REG_DWORD, nullptr, &index, &size) ||
        index >= paths.size()) index = 0;
    return paths;
}

static void clearMatchingLegacySession(PCWSTR key, const Locations& expected, DWORD activeIndex) {
    DWORD index = 0;
    if (expected.empty() || loadLegacySession(key, index) != expected || activeIndex != index) return;
    if (RegDeleteKeyValueW(HKEY_CURRENT_USER, key, L"Folders") == ERROR_SUCCESS)
        RegDeleteKeyValueW(HKEY_CURRENT_USER, key, L"ActiveTab");
}

static bool isLandingPage(const std::wstring& path) {
    return equalPath(path, L"::{679f85cb-0220-4080-b29b-5540cc05aab6}") ||
           equalPath(path, L"::{f874310e-b6b7-47dc-bc84-b9e6b38f5903}") ||
           equalPath(path, L"::{20d04fe0-3aea-1069-a2d8-08002b30309d}");
}

static bool stopped(DWORD timeout) {
    return WaitForSingleObject(stopEvent, timeout) != WAIT_TIMEOUT;
}

// Wait for the specific tab to acknowledge navigation before continuing.
static bool navigate(Tab& tab, const std::wstring& path, PCIDLIST_ABSOLUTE id) {
    if (equalPath(tab.path, path)) return true;
    if (FAILED(tab.browser->BrowseObject(id, SBSP_SAMEBROWSER | SBSP_ABSOLUTE))) return false;
    const auto deadline = GetTickCount64() + 3000;
    DWORD wait = 16;
    do {
        if (equalPath(location(tab.browser.Get()), path)) {
            tab.path = path;
            return true;
        }
        if (stopped(wait)) break;
        wait = std::min<DWORD>(wait * 2, 100);
    } while (GetTickCount64() < deadline);
    return false;
}

static bool restore(IShellWindows* windows, HWND frame, const Locations& saved,
                    std::vector<Tab> before = {}, Locations* visualOrder = nullptr,
                    HWND* lastCreatedTab = nullptr) {
    if (before.empty()) before = readTabs(windows, frame);
    if (before.empty()) return false;
    Locations existing;
    for (const auto& tab : before) existing.push_back(tab.path);
    if (visualOrder) *visualOrder = existing;
    bool useLanding = before.size() == 1 && isLandingPage(existing.front()) &&
        std::none_of(saved.begin(), saved.end(), [&](const auto& p) { return equalPath(p, existing.front()); });
    bool complete = true;
    for (const auto& path : saved) {
        if (stopped(0) || !IsWindow(frame)) return false;
        auto found = std::find_if(existing.begin(), existing.end(),
            [&](const auto& p) { return equalPath(p, path); });
        if (found != existing.end()) {
            existing.erase(found);
            continue;
        }
        PIDLIST_ABSOLUTE raw = nullptr;
        if (FAILED(SHParseDisplayName(path.c_str(), nullptr, &raw, 0, nullptr)) || !raw) {
            complete = false;
            continue;
        }
        struct ReleasePidl { PIDLIST_ABSOLUTE id; ~ReleasePidl() { CoTaskMemFree(id); } } guard{raw};
        if (useLanding) {
            useLanding = false;
            // Resolving a shell path can take time. Preserve a folder that the
            // user opened in the landing tab during that interval.
            auto current = location(before.front().browser.Get());
            if (isLandingPage(current)) {
                if (!navigate(before.front(), path, raw)) return false;
                if (visualOrder) visualOrder->front() = path;
                continue;
            }
            if (visualOrder) visualOrder->front() = current;
        }
        HWND active = FindWindowExW(frame, nullptr, L"ShellTabWindowClass", nullptr);
        if (!active || tabCount(frame) != before.size() ||
            !PostMessageW(active, WM_COMMAND, newTabCommand, 0)) return false;
        bool added = false;
        const auto deadline = GetTickCount64() + 4000;
        DWORD wait = 12;
        while (GetTickCount64() < deadline && !stopped(wait)) {
            if (!IsWindow(frame)) return false;
            wait = std::min<DWORD>(wait * 2, 100);
            size_t children = tabCount(frame);
            if (children == before.size()) continue;
            if (children != before.size() + 1) return false;
            auto after = readTabs(windows, frame, &before);
            if (after.empty()) continue;
            if (after.size() != before.size() + 1) return false;
            Tab* fresh = nullptr;
            for (auto& candidate : after) {
                if (std::any_of(before.begin(), before.end(), [&](const Tab& prev) {
                    return prev.identity.Get() == candidate.identity.Get();
                })) continue;
                // Other tabs were replaced while waiting. Leave them alone.
                if (fresh) return false;
                fresh = &candidate;
            }
            if (!fresh || !navigate(*fresh, path, raw)) return false;
            if (lastCreatedTab) *lastCreatedTab = fresh->handle;
            if (visualOrder) visualOrder->push_back(path);
            before = std::move(after);
            added = true;
            break;
        }
        if (!added) return false;
    }
    return complete;
}

static void selectRestoredTab(HWND frame, size_t index) {
    HWND previous = FindWindowExW(frame, nullptr, L"ShellTabWindowClass", nullptr);
    if (!previous || index >= tabCount(frame)) return;
    if (!SendMessageTimeoutW(frame, WM_COMMAND, selectTabByIndexCommand, index + 1,
                             SMTO_ABORTIFHUNG | SMTO_BLOCK, 300, nullptr)) return;
    // The shell can acknowledge the command before it updates the active
    // tab HWND. Let the subsequent snapshot observe the change.
    const auto deadline = GetTickCount64() + 200;
    while (FindWindowExW(frame, nullptr, L"ShellTabWindowClass", nullptr) == previous &&
           GetTickCount64() < deadline && !stopped(10)) {}
}

struct Session {
    std::wstring filePath;
    PCWSTR legacyKey;
    HWND lastFrame = nullptr;
    ULONGLONG openedAt = 0;
    Locations snapshot, pending, partialRestore;
    DWORD activeIndex = 0, savedIndex = 0;
    bool preservePending = false;
    bool restoreOnOpen;
    Session(std::wstring storage, bool startEmpty, PCWSTR legacy = sessionKey)
        : filePath(std::move(storage)), legacyKey(legacy), restoreOnOpen(startEmpty) {
        pending = loadSession(filePath.c_str(), savedIndex);
        if (pending.empty()) {
            pending = loadLegacySession(legacyKey, savedIndex);
            if (pending.empty() || !saveSession(filePath.c_str(), pending, savedIndex)) return;
            DWORD index = 0;
            if (loadSession(filePath.c_str(), index) != pending || index != savedIndex) return;
        }
        clearMatchingLegacySession(legacyKey, pending, savedIndex);
    }

    void tick(IShellWindows* windows, const std::vector<HWND>& frames) {
        if (lastFrame && std::find(frames.begin(), frames.end(), lastFrame) == frames.end()) {
            // Also catches a fast close/reopen between two observations.
            if (!preservePending && !snapshot.empty()) {
                pending = snapshot;
                savedIndex = activeIndex;
                saveSession(filePath.c_str(), pending, savedIndex);
            }
            lastFrame = nullptr;
            snapshot.clear();
            preservePending = false;
            partialRestore.clear();
            restoreOnOpen = true;
        }
        if (frames.size() != 1) {
            if (frames.size() > 1) {
                lastFrame = nullptr;
                snapshot.clear();
                preservePending = false;
                partialRestore.clear();
                restoreOnOpen = false;
            }
            return;
        }
        HWND frame = frames.front();
        if (lastFrame != frame) openedAt = GetTickCount64();
        lastFrame = frame;
        auto tabs = readTabs(windows, frame);
        if (tabs.empty()) return;
        if (restoreOnOpen && !pending.empty()) {
            // Explorer may first publish Home while opening an explicit path.
            if (tabs.size() == 1 && isLandingPage(tabs.front().path) &&
                GetTickCount64() - openedAt < 360) return;
            restoreOnOpen = false;
            const bool single = tabs.size() == 1;
            const bool explicitFolder = single && !isLandingPage(tabs.front().path);
            Locations visualOrder;
            HWND lastCreatedTab = nullptr;
            preservePending = !restore(windows, frame, pending, std::move(tabs), &visualOrder, &lastCreatedTab);
            if (!preservePending && single && visualOrder.size() > 1) {
                size_t selected = 0;
                if (!explicitFolder && savedIndex < pending.size()) {
                    // Count occurrences to distinguish two tabs in the same folder.
                    const auto matches = [&](const auto& p) { return equalPath(p, pending[savedIndex]); };
                    auto occurrence = std::count_if(pending.begin(), pending.begin() + savedIndex, matches);
                    for (size_t i = 0; i < visualOrder.size(); ++i) {
                        if (matches(visualOrder[i]) && occurrence-- == 0) { selected = i; break; }
                    }
                }
                // Leave the user's choice in place if they switched tabs during restoration.
                if (selected + 1 != visualOrder.size() && lastCreatedTab &&
                    FindWindowExW(frame, nullptr, L"ShellTabWindowClass", nullptr) == lastCreatedTab) {
                    selectRestoredTab(frame, selected);
                }
            }
            auto restoredTabs = readTabs(windows, frame);
            Locations result;
            const HWND selectedTab = FindWindowExW(frame, nullptr, L"ShellTabWindowClass", nullptr);
            activeIndex = 0;
            for (const auto& tab : restoredTabs) {
                if (selectedTab && tab.handle == selectedTab) activeIndex = static_cast<DWORD>(result.size());
                result.push_back(tab.path);
            }
            if (!preservePending) {
                // A quick second close must still retain the restored session.
                snapshot = result.empty() ? pending : std::move(result);
                consumeSession(filePath.c_str(), pending, savedIndex);
                clearMatchingLegacySession(legacyKey, pending, savedIndex);
                pending.clear();
            } else partialRestore = std::move(result);
            return;
        }
        restoreOnOpen = false;
        snapshot.clear();
        const HWND active = FindWindowExW(frame, nullptr, L"ShellTabWindowClass", nullptr);
        activeIndex = 0;
        for (const auto& tab : tabs) {
            if (active && tab.handle == active) activeIndex = static_cast<DWORD>(snapshot.size());
            snapshot.push_back(tab.path);
        }
        if (preservePending && partialRestore.empty()) partialRestore = snapshot;
        if (preservePending && partialRestore != snapshot) preservePending = false;
    }
};

static BOOL CALLBACK collectExplorerFrame(HWND window, LPARAM context) {
    if (hasClass(window, L"CabinetWClass"))
        reinterpret_cast<std::vector<HWND>*>(context)->push_back(window);
    return TRUE;
}

static std::vector<HWND> explorerFrames() {
    std::vector<HWND> frames;
    EnumWindows(collectExplorerFrame, reinterpret_cast<LPARAM>(&frames));
    return frames;
}

// Out-of-context WinEvents are delivered to the thread that registered them.
// The callback only requests a scan; COM work stays outside the callback.
struct ExplorerEvents {
    std::vector<HWND> frames;
    bool changed = false;
};
static thread_local ExplorerEvents* observedEvents;

static void CALLBACK explorerEvent(HWINEVENTHOOK, DWORD event, HWND window,
                                   LONG object, LONG child, DWORD, DWORD) {
    if (!observedEvents || !window || child != CHILDID_SELF) return;
    if (event != EVENT_SYSTEM_FOREGROUND && object != OBJID_WINDOW) return;
    const auto& frames = observedEvents->frames;
    // A destroyed HWND may already have lost its class name.
    if (std::find(frames.begin(), frames.end(), window) != frames.end() ||
        hasClass(window, L"CabinetWClass")) {
        observedEvents->changed = true;
    } else if (event != EVENT_SYSTEM_FOREGROUND &&
               (hasClass(window, L"ShellTabWindowClass") || hasClass(window, L"SHELLDLL_DefView")) &&
               hasClass(GetAncestor(window, GA_ROOT), L"CabinetWClass")) {
        observedEvents->changed = true;
    }
}

struct ProcessEvents {
    DWORD pid;
    HWINEVENTHOOK structure;
    HWINEVENTHOOK title;
    HANDLE process;
    void close() const {
        if (structure) UnhookWinEvent(structure);
        if (title) UnhookWinEvent(title);
        if (process) CloseHandle(process);
    }
};

// Explorer can host the desktop and folder windows in different processes.
// Keep hooks for a live folder host after its last window closes, so opening
// another window in the background is also detected.
static bool watchExplorerProcesses(std::vector<ProcessEvents>& hooks, DWORD shellPid,
                                   const std::vector<HWND>& frames) {
    std::vector<DWORD> owners;
    if (shellPid) owners.push_back(shellPid);
    for (HWND frame : frames) {
        DWORD pid = 0;
        GetWindowThreadProcessId(frame, &pid);
        if (pid && std::find(owners.begin(), owners.end(), pid) == owners.end()) owners.push_back(pid);
    }
    std::erase_if(hooks, [&](const auto& hook) {
        bool obsolete = hook.process ? WaitForSingleObject(hook.process, 0) == WAIT_OBJECT_0 :
            std::find(owners.begin(), owners.end(), hook.pid) == owners.end();
        if (obsolete) hook.close();
        return obsolete;
    });
    bool ready = true;
    for (auto pid : owners) {
        auto it = std::find_if(hooks.begin(), hooks.end(), [pid](const auto& h) { return h.pid == pid; });
        if (it == hooks.end()) {
            hooks.push_back({pid, nullptr, nullptr, OpenProcess(SYNCHRONIZE, FALSE, pid)});
            it = std::prev(hooks.end());
        }
        if (!it->structure) it->structure = SetWinEventHook(EVENT_OBJECT_CREATE, EVENT_OBJECT_SHOW,
            nullptr, explorerEvent, pid, 0, WINEVENT_OUTOFCONTEXT);
        if (!it->title) it->title = SetWinEventHook(EVENT_OBJECT_NAMECHANGE, EVENT_OBJECT_NAMECHANGE,
            nullptr, explorerEvent, pid, 0, WINEVENT_OUTOFCONTEXT);
        if (!it->structure || !it->title) ready = false;
    }
    return ready;
}

static DWORD WINAPI run(void*) {
    DWORD shellPid = 0;
    if (FAILED(CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED))) return 0;
    {
        ExplorerEvents events;
        observedEvents = &events;
        std::vector<ProcessEvents> hooks;
        // A new window can run in a separate Explorer process.
        auto foreground = SetWinEventHook(EVENT_SYSTEM_FOREGROUND, EVENT_SYSTEM_FOREGROUND, nullptr,
                                          explorerEvent, 0, 0, WINEVENT_OUTOFCONTEXT);
        events.frames = explorerFrames();
        Session session(sessionFilePath(), events.frames.empty());
        ComPtr<IShellWindows> windows;
        ULONGLONG due = 0;
        ULONGLONG settleUntil = GetTickCount64() + 1100;
        while (!stopped(0)) {
            MSG message;
            for (int i = 0; i < 64 && PeekMessageW(&message, nullptr, 0, 0, PM_REMOVE); ++i)
                DispatchMessageW(&message);
            auto now = GetTickCount64();
            if (events.changed) {
                events.changed = false;
                settleUntil = now + 1100;
                due = (std::min)(due, now + 65);
            }
            if (now >= due) {
                DWORD currentShellPid = 0;
                GetWindowThreadProcessId(GetShellWindow(), &currentShellPid);
                if (currentShellPid != shellPid) {
                    // The tool can outlive Explorer and its COM server.
                    windows.Reset();
                    shellPid = currentShellPid;
                }
                events.frames = explorerFrames();
                bool hooksReady = watchExplorerProcesses(hooks, shellPid, events.frames);
                if (!windows && shellPid)
                    CoCreateInstance(CLSID_ShellWindows, nullptr, CLSCTX_ALL, IID_PPV_ARGS(&windows));
                if (windows) session.tick(windows.Get(), events.frames);
                DWORD interval = events.frames.empty() ? 5000 : 3000;
                if (!windows || !hooksReady || !foreground) interval = 350;
                if (events.frames.size() == 1 && now < settleUntil) interval = 140;
                due = GetTickCount64() + interval;
            }
            now = GetTickCount64();
            DWORD timeout = now < due ? static_cast<DWORD>(due - now) : 0;
            if (MsgWaitForMultipleObjectsEx(1, &stopEvent, timeout, QS_ALLINPUT,
                                            MWMO_INPUTAVAILABLE) == WAIT_FAILED) break;
        }
        observedEvents = nullptr;
        if (foreground) UnhookWinEvent(foreground);
        for (const auto& hook : hooks) hook.close();
    }
    CoUninitialize();
    return 0;
}

static bool accountAllowed() {
    PCWSTR configuredSid = Wh_GetStringSetting(L"accountSid");
    bool allowed = configuredSid && !*configuredSid;
    PSID sid = nullptr;
    if (!allowed && configuredSid && ConvertStringSidToSidW(configuredSid, &sid)) {
        BOOL member = FALSE;
        allowed = CheckTokenMembership(nullptr, sid, &member) && member;
        LocalFree(sid);
    }
    if (configuredSid) Wh_FreeStringSetting(configuredSid);
    return allowed;
}

static DWORD WINAPI waitForAccountChange(void*) {
    WaitForSingleObject(stopEvent, INFINITE);
    return 0;
}

static bool observing;

BOOL WhTool_ModInit() {
    stopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!stopEvent) return FALSE;
    observing = accountAllowed();
    worker = CreateThread(nullptr, 0, observing ? run : waitForAccountChange, nullptr, 0, nullptr);
    if (!worker) CloseHandle(stopEvent);
    return worker != nullptr;
}

void WhTool_ModUninit() {
    SetEvent(stopEvent);
    WaitForSingleObject(worker, INFINITE);
    CloseHandle(worker);
    CloseHandle(stopEvent);
}

void WhTool_ModSettingsChanged() {
    if (accountAllowed() == observing) return;
    WhTool_ModUninit();
    if (!WhTool_ModInit()) ExitProcess(1);
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
    if (g_isToolModProcessLauncher) {
        return;
    }

    WhTool_ModSettingsChanged();
}

void Wh_ModUninit() {
    if (g_isToolModProcessLauncher) {
        return;
    }

    WhTool_ModUninit();
    ExitProcess(0);
}
