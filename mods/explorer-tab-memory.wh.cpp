// ==WindhawkMod==
// @id              explorer-tab-memory
// @name            Explorer Tab Memory Saver
// @description     Restores your tabs and active folder when you reopen File Explorer.
// @version         0.2.1
// @author          Maxxwell Quinn
// @github          https://github.com/MXXQN
// @license         MIT
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lshell32 -luuid -ladvapi32
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

Tested on Windows 11 x64, build 26300.9974, with native File Explorer tabs.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- accountSid: ""
  $name: Windows account SID
  $description: Leave empty for all accounts, or enter the SID of one user.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <sddl.h>
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

static Locations loadSession(PCWSTR key) {
    DWORD bytes = 0;
    if (RegGetValueW(HKEY_CURRENT_USER, key, L"Folders", RRF_RT_REG_MULTI_SZ,
            nullptr, nullptr, &bytes) || bytes > 4 * 1024 * 1024) return {};
    std::vector<wchar_t> data(bytes / sizeof(wchar_t) + 2);
    if (RegGetValueW(HKEY_CURRENT_USER, key, L"Folders", RRF_RT_REG_MULTI_SZ,
            nullptr, data.data(), &bytes)) return {};
    Locations result;
    for (PCWSTR item = data.data(); *item; item += wcslen(item) + 1) result.emplace_back(item);
    return result;
}

static bool saveSession(PCWSTR key, const Locations& paths) {
    if (paths.empty()) return false;
    std::vector<wchar_t> data;
    for (const auto& path : paths) data.insert(data.end(), path.c_str(), path.c_str() + path.size() + 1);
    data.push_back(0);
    return RegSetKeyValueW(HKEY_CURRENT_USER, key, L"Folders", REG_MULTI_SZ,
        data.data(), static_cast<DWORD>(data.size() * sizeof(wchar_t))) == ERROR_SUCCESS;
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
    PCWSTR key;
    HWND lastFrame = nullptr;
    ULONGLONG openedAt = 0;
    Locations snapshot, pending, partialRestore;
    DWORD activeIndex = 0, savedIndex = 0;
    bool preservePending = false;
    bool restoreOnOpen;
    Session(PCWSTR storage, bool startEmpty) : key(storage), pending(loadSession(storage)), restoreOnOpen(startEmpty) {
        DWORD bytes = sizeof(savedIndex);
        RegGetValueW(HKEY_CURRENT_USER, key, L"ActiveTab", RRF_RT_REG_DWORD,
                     nullptr, &savedIndex, &bytes);
    }

    void tick(IShellWindows* windows, const std::vector<HWND>& frames) {
        if (lastFrame && std::find(frames.begin(), frames.end(), lastFrame) == frames.end()) {
            // Also catches a fast close/reopen between two observations.
            if (!preservePending && !snapshot.empty()) {
                pending = snapshot;
                savedIndex = activeIndex;
                if (saveSession(key, pending))
                    RegSetKeyValueW(HKEY_CURRENT_USER, key, L"ActiveTab", REG_DWORD,
                                    &savedIndex, sizeof(savedIndex));
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
                RegDeleteKeyValueW(HKEY_CURRENT_USER, key, L"Folders");
                RegDeleteKeyValueW(HKEY_CURRENT_USER, key, L"ActiveTab");
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

static std::vector<HWND> explorerFrames() {
    std::vector<HWND> frames;
    EnumWindows([](HWND window, LPARAM context) -> BOOL {
        if (hasClass(window, L"CabinetWClass"))
            reinterpret_cast<std::vector<HWND>*>(context)->push_back(window);
        return TRUE;
    }, reinterpret_cast<LPARAM>(&frames));
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
    std::vector<DWORD> owners{shellPid};
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
    // At sign-in Windhawk may initialize before the desktop HWND exists.
    DWORD shellPid = 0;
    while (!shellPid && !stopped(100))
        GetWindowThreadProcessId(GetShellWindow(), &shellPid);
    if (shellPid != GetCurrentProcessId()) return 0;
    if (FAILED(CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED))) return 0;
    {
        ExplorerEvents events;
        observedEvents = &events;
        std::vector<ProcessEvents> hooks;
        // A new window can run in a separate Explorer process.
        auto foreground = SetWinEventHook(EVENT_SYSTEM_FOREGROUND, EVENT_SYSTEM_FOREGROUND, nullptr,
                                          explorerEvent, 0, 0, WINEVENT_OUTOFCONTEXT);
        events.frames = explorerFrames();
        Session session(sessionKey, events.frames.empty());
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
                events.frames = explorerFrames();
                bool hooksReady = watchExplorerProcesses(hooks, shellPid, events.frames);
                if (!windows) CoCreateInstance(CLSID_ShellWindows, nullptr, CLSCTX_ALL, IID_PPV_ARGS(&windows));
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

BOOL Wh_ModInit() {
    // Run one observer in the desktop shell for each interactive user.
    DWORD shellPid = 0;
    GetWindowThreadProcessId(GetShellWindow(), &shellPid);
    if (shellPid && shellPid != GetCurrentProcessId()) return FALSE;
    PCWSTR configuredSid = Wh_GetStringSetting(L"accountSid");
    bool allowed = configuredSid && !*configuredSid;
    PSID sid = nullptr;
    if (!allowed && configuredSid && ConvertStringSidToSidW(configuredSid, &sid)) {
        BOOL member = FALSE;
        allowed = CheckTokenMembership(nullptr, sid, &member) && member;
        LocalFree(sid);
    }
    if (configuredSid) Wh_FreeStringSetting(configuredSid);
    if (!allowed) return FALSE;
    stopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!stopEvent) return FALSE;
    worker = CreateThread(nullptr, 0, run, nullptr, 0, nullptr);
    if (!worker) CloseHandle(stopEvent);
    return worker != nullptr;
}

void Wh_ModUninit() {
    SetEvent(stopEvent);
    WaitForSingleObject(worker, INFINITE);
    CloseHandle(worker);
    CloseHandle(stopEvent);
}
