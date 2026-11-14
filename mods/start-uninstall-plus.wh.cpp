// ==WindhawkMod==
// @id              start-uninstall-plus
// @name            Start Uninstall Plus
// @description     Adds "Find uninstaller..." to Start's app menu: shows which installed program matches the app, then runs its uninstaller or opens Settings filtered to it
// @version         1.1
// @author          watdafawx
// @github          https://github.com/watdafawx
// @homepage        https://github.com/watdafawx/windhawk-start-uninstall-plus
// @license         GPL-3.0
// @include         StartMenuExperienceHost.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lruntimeobject -lshell32 -luuid
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Start Uninstall Plus

For a classic (Win32) app, Start's own **Uninstall** only opens Settings >
Installed apps, unfiltered. This mod adds **Find uninstaller...** under it, which:

1. Finds the program the app belongs to among Installed apps, from the
   shortcut's target and the program's registry entry (install folder,
   icon, uninstaller location, then name).
2. Shows what it found before doing anything: the Start entry (name, app ID,
   target), the matched program (name, publisher, version, install folder,
   size, date, the exact uninstall command) and why it matched. When several
   programs fit, you pick one.
3. **Run uninstaller** starts that command (through Explorer, as a normal
   launch). **Open in Settings** opens Installed apps with the program's name
   typed into its search box.

![Find uninstaller... in Start's app menu](https://raw.githubusercontent.com/watdafawx/windhawk-start-uninstall-plus/main/screenshots/menu.png)

![Details of the matched program](https://raw.githubusercontent.com/watdafawx/windhawk-start-uninstall-plus/main/screenshots/details.png)

Packaged (Store/MSIX) apps get no extra item: Start's Uninstall already removes
them directly.

Works on pinned apps, the All apps list and Recommended, in any display
language.

Known limit: when the menu is opened from the keyboard (Shift+F10 or the menu
key) while the mouse rests over another tile, the item targets the tile under
the mouse. The details show that app's name, so check it before uninstalling.

The mod reaches Start's menu through XAML Diagnostics (a TAP and a
VisualTreeWatcher), following Windows 11 Start Menu Styler by m417z, itself
based on TranslucentTB's ExplorerTAP. It has been tested running together with
Windows 11 Start Menu Styler.
*/
// ==/WindhawkModReadme==

#include <windhawk_utils.h>
#include <xamlom.h>

#include <algorithm>
#include <atomic>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <vector>

#undef GetCurrentTime

#include <Unknwn.h>
#include <ocidl.h>
#include <exdisp.h>
#include <shldisp.h>
#include <shlobj.h>
#include <shobjidl.h>
#include <uiautomation.h>
#include <winrt/base.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.UI.Text.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.Xaml.Automation.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Controls.Primitives.h>
#include <winrt/Windows.UI.Xaml.Input.h>
#include <winrt/Windows.UI.Xaml.Interop.h>
#include <winrt/Windows.UI.Xaml.Media.h>

namespace wf = winrt::Windows::Foundation;
namespace wfc = winrt::Windows::Foundation::Collections;
namespace wux = winrt::Windows::UI::Xaml;
namespace wuxc = winrt::Windows::UI::Xaml::Controls;

std::atomic<DWORD> g_targetThreadId = 0;

// See Start Menu Styler: keeps XAML from creating composition diagnostics,
// which corrupt the heap when several UI threads add visuals.
thread_local bool g_reportCompositionDiagAsDisabled;

HMODULE GetCurrentModuleHandle() {
    HMODULE module;
    if (!GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                               GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                           L"", &module)) {
        return nullptr;
    }
    return module;
}

HWND GetCoreWnd();


// ---------------------------------------------------------------------------
// Small helpers
// ---------------------------------------------------------------------------

bool EqualsI(const std::wstring& a, const std::wstring& b) {
    return a.size() == b.size() &&
           CompareStringOrdinal(a.c_str(), -1, b.c_str(), -1, TRUE) == CSTR_EQUAL;
}

bool StartsWithI(const std::wstring& s, const std::wstring& prefix) {
    return s.size() >= prefix.size() &&
           CompareStringOrdinal(s.c_str(), (int)prefix.size(), prefix.c_str(), (int)prefix.size(), TRUE) ==
               CSTR_EQUAL;
}

// Whether path is dir or inside it.
bool IsUnder(const std::wstring& path, const std::wstring& dir) {
    if (dir.empty() || !StartsWithI(path, dir)) return false;
    return path.size() == dir.size() || path[dir.size()] == L'\\' || dir.back() == L'\\';
}

std::wstring DirOf(const std::wstring& path) {
    size_t slash = path.find_last_of(L'\\');
    return slash == std::wstring::npos ? std::wstring() : path.substr(0, slash);
}

std::wstring FileName(const std::wstring& path) {
    size_t slash = path.find_last_of(L'\\');
    return slash == std::wstring::npos ? path : path.substr(slash + 1);
}

std::wstring Expand(const std::wstring& s) {
    if (s.find(L'%') == std::wstring::npos) return s;
    WCHAR out[MAX_PATH * 2];
    DWORD n = ExpandEnvironmentStrings(s.c_str(), out, ARRAYSIZE(out));
    return n && n <= ARRAYSIZE(out) ? std::wstring(out) : s;
}

// A registry path value as a plain path: variables expanded, no quotes, no
// icon index (",0"), no trailing backslash.
std::wstring CleanPath(std::wstring s, bool iconIndex) {
    s = Expand(s);
    auto trim = [&s] {
        while (!s.empty() && (s.front() == L' ' || s.front() == L'"')) s.erase(s.begin());
        while (!s.empty() && (s.back() == L' ' || s.back() == L'"')) s.pop_back();
    };
    trim();
    if (iconIndex) {
        size_t comma = s.find_last_of(L',');
        if (comma != std::wstring::npos && comma + 1 < s.size() &&
            s.find_first_not_of(L"-0123456789 ", comma + 1) == std::wstring::npos) {
            s.resize(comma);
            trim();
        }
    }
    while (s.size() > 3 && s.back() == L'\\') s.pop_back();
    return s;
}

// Splits a command line into program and arguments: the quoted part, else up
// to the first ".exe" (uninstall strings are often unquoted paths with
// spaces), else up to the first space.
void SplitCommand(const std::wstring& line, std::wstring& file, std::wstring& params) {
    std::wstring s = line;
    while (!s.empty() && s.front() == L' ') s.erase(s.begin());
    size_t end;
    if (!s.empty() && s.front() == L'"') {
        end = s.find(L'"', 1);
        file = s.substr(1, end == std::wstring::npos ? std::wstring::npos : end - 1);
        end = end == std::wstring::npos ? s.size() : end + 1;
    } else {
        std::wstring lower = s;
        for (auto& c : lower) c = (wchar_t)towlower(c);
        size_t exe = lower.find(L".exe");
        end = exe != std::wstring::npos ? exe + 4 : s.find(L' ');
        if (end == std::wstring::npos) end = s.size();
        file = s.substr(0, end);
    }
    params = end < s.size() ? s.substr(end) : std::wstring();
    while (!params.empty() && params.front() == L' ') params.erase(params.begin());
    file = Expand(file);
}

std::wstring ReadString(HKEY key, const wchar_t* name) {
    WCHAR buf[2048];
    DWORD size = sizeof(buf) - sizeof(WCHAR), type = 0;
    if (RegQueryValueEx(key, name, nullptr, &type, (BYTE*)buf, &size) != ERROR_SUCCESS ||
        (type != REG_SZ && type != REG_EXPAND_SZ))
        return {};
    buf[size / sizeof(WCHAR)] = 0;
    return buf;
}

DWORD ReadDword(HKEY key, const wchar_t* name) {
    DWORD value = 0, size = sizeof(value), type = 0;
    if (RegQueryValueEx(key, name, nullptr, &type, (BYTE*)&value, &size) != ERROR_SUCCESS || type != REG_DWORD)
        return 0;
    return value;
}

// ---------------------------------------------------------------------------
// The app behind a Start tile
// ---------------------------------------------------------------------------

struct StartApp {
    std::wstring aumid;   // app user model ID, from the tile
    std::wstring name;    // as Start shows it
    std::wstring target;  // the program the shortcut starts
    std::wstring args;
};

// Windows' own property keys, spelled out so no extra import library is needed.
static const PROPERTYKEY kLinkTargetParsingPath = {
    {0xB9B4B3FC, 0x2B51, 0x4A42, {0xB5, 0xD8, 0x32, 0x41, 0x46, 0xAF, 0xCF, 0x25}}, 2};
static const PROPERTYKEY kLinkArguments = {
    {0x436F2667, 0x14E2, 0x4FEB, {0xB3, 0x0A, 0x14, 0x6C, 0x53, 0xB5, 0xB6, 0x74}}, 100};
static const GUID kFolderAppsFolder = {0x1e87508d, 0x89c2, 0x42f0, {0x8a, 0x7e, 0x64, 0x5a, 0x0f, 0x50, 0xca, 0x58}};

void ResolveTarget(StartApp& app) {
    winrt::com_ptr<IShellItem2> item;
    if (SUCCEEDED(SHCreateItemInKnownFolder(kFolderAppsFolder, KF_FLAG_DONT_VERIFY, app.aumid.c_str(),
                                            IID_PPV_ARGS(item.put())))) {
        PWSTR value = nullptr;
        if (SUCCEEDED(item->GetString(kLinkTargetParsingPath, &value)) && value) {
            app.target = value;
            CoTaskMemFree(value);
        }
        value = nullptr;
        if (SUCCEEDED(item->GetString(kLinkArguments, &value)) && value) {
            app.args = value;
            CoTaskMemFree(value);
        }
    }
    // An app ID that is itself a path: "{known folder GUID}\dir\app.exe".
    if (app.target.empty() && app.aumid.size() > 39 && app.aumid[0] == L'{' && app.aumid[38] == L'\\') {
        GUID id;
        if (SUCCEEDED(CLSIDFromString(app.aumid.substr(0, 38).c_str(), &id))) {
            PWSTR base = nullptr;
            if (SUCCEEDED(SHGetKnownFolderPath(id, KF_FLAG_DONT_VERIFY, nullptr, &base)) && base) {
                app.target = std::wstring(base) + app.aumid.substr(38);
            }
            CoTaskMemFree(base);
        }
    } else if (app.target.empty() && app.aumid.size() > 3 && app.aumid[1] == L':' && app.aumid[2] == L'\\') {
        app.target = app.aumid;
    }
}

// ---------------------------------------------------------------------------
// Matching it to an Installed apps entry
// ---------------------------------------------------------------------------

struct Candidate {
    int score = 0;
    std::wstring why;
    std::wstring key;  // registry key name; Settings uses it as the row's ID
    std::wstring hive;
    std::wstring name, publisher, version, location, date, size;
    std::wstring command;  // as registered
    std::wstring file, params;  // what runs
};

std::vector<std::wstring> SharedFolders() {
    static const KNOWNFOLDERID* const kShared[] = {
        &FOLDERID_ProgramFiles, &FOLDERID_ProgramFilesX86, &FOLDERID_ProgramFilesCommon,
        &FOLDERID_ProgramFilesCommonX86, &FOLDERID_LocalAppData, &FOLDERID_RoamingAppData,
        &FOLDERID_UserProgramFiles, &FOLDERID_Windows, &FOLDERID_Profile, &FOLDERID_ProgramData,
    };
    std::vector<std::wstring> folders;
    for (const KNOWNFOLDERID* id : kShared) {
        PWSTR path = nullptr;
        if (SUCCEEDED(SHGetKnownFolderPath(*id, KF_FLAG_DONT_VERIFY, nullptr, &path)) && path)
            folders.push_back(path);
        CoTaskMemFree(path);
    }
    return folders;
}

bool IsShared(const std::wstring& dir, const std::vector<std::wstring>& shared) {
    if (dir.size() <= 3) return true;  // a drive
    for (auto& f : shared)
        if (EqualsI(dir, f)) return true;
    return false;
}

// 2 same name, 1 same but for a version suffix or one is the other plus words.
int NameMatch(const std::wstring& name, const std::wstring& title) {
    if (name.empty() || title.empty()) return 0;
    if (EqualsI(name, title)) return 2;
    auto startsWithWord = [](const std::wstring& s, const std::wstring& prefix) {
        return s.size() > prefix.size() + 1 && s[prefix.size()] == L' ' && StartsWithI(s, prefix);
    };
    if (startsWithWord(name, title)) return 1;
    if (startsWithWord(title, name) && name.find(L' ') != std::wstring::npos) return 1;
    return 0;
}

std::vector<Candidate> FindCandidates(const StartApp& app) {
    std::vector<Candidate> out;
    const std::wstring program = app.target;
    WCHAR windir[MAX_PATH];
    UINT wn = GetWindowsDirectory(windir, MAX_PATH);
    if (!program.empty() && wn && wn < MAX_PATH && IsUnder(program, windir)) return out;  // part of Windows

    const std::wstring programDir = DirOf(program);
    const auto shared = SharedFolders();
    struct Root {
        HKEY hive;
        REGSAM view;
        PCWSTR label;
    };
    static const Root kRoots[] = {
        {HKEY_LOCAL_MACHINE, KEY_WOW64_64KEY, L"HKLM"},
        {HKEY_LOCAL_MACHINE, KEY_WOW64_32KEY, L"HKLM (32-bit)"},
        {HKEY_CURRENT_USER, 0, L"HKCU"},
    };
    for (const Root& root : kRoots) {
        HKEY list = nullptr;
        if (RegOpenKeyEx(root.hive, L"Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall", 0,
                         KEY_READ | root.view, &list) != ERROR_SUCCESS)
            continue;
        WCHAR sub[256];
        for (DWORD i = 0;; ++i) {
            DWORD subLen = ARRAYSIZE(sub);
            if (RegEnumKeyEx(list, i, sub, &subLen, nullptr, nullptr, nullptr, nullptr) != ERROR_SUCCESS) break;
            HKEY entry = nullptr;
            if (RegOpenKeyEx(list, sub, 0, KEY_READ | root.view, &entry) != ERROR_SUCCESS) continue;
            Candidate c;
            c.name = ReadString(entry, L"DisplayName");
            c.command = ReadString(entry, L"UninstallString");
            // As Installed apps lists them: named, removable, not an update.
            const bool listed = !c.name.empty() && !c.command.empty() && !ReadDword(entry, L"SystemComponent") &&
                                !ReadDword(entry, L"NoRemove") && ReadString(entry, L"ParentKeyName").empty();
            if (listed) {
                SplitCommand(c.command, c.file, c.params);
                const std::wstring icon = CleanPath(ReadString(entry, L"DisplayIcon"), true);
                c.location = CleanPath(ReadString(entry, L"InstallLocation"), false);
                const std::wstring uninstallerName = FileName(c.file);
                const bool sharedUninstaller =
                    EqualsI(uninstallerName, L"msiexec.exe") || EqualsI(c.file, L"msiexec") ||
                    EqualsI(uninstallerName, L"rundll32.exe");
                // A launcher removing something else: steam.exe steam://uninstall/730.
                const bool forOther = c.params.find(L"://") != std::wstring::npos;
                int rule = 0;
                if (!program.empty() && EqualsI(icon, program)) {
                    rule = 4;
                    c.why = L"Exact: the program is this entry's icon file";
                } else if (!program.empty() && !c.location.empty() && IsUnder(program, c.location) &&
                           !IsShared(c.location, shared)) {
                    rule = 3;
                    c.why = L"Strong: the program is inside this entry's install folder";
                } else if (!program.empty() && !sharedUninstaller && !forOther && !programDir.empty() &&
                           !IsShared(programDir, shared) &&
                           (IsUnder(c.file, programDir) || IsUnder(icon, programDir))) {
                    rule = 2;
                    c.why = L"Good: the uninstaller sits in the program's folder";
                }
                const int named = NameMatch(c.name, app.name);
                if (!rule && named) {
                    rule = 1;
                    c.why = L"Weak: by name only - check it is the right program";
                }
                if (rule && !forOther) {
                    c.score = rule * 4 + named;
                    c.key = sub;
                    c.hive = root.label;
                    c.publisher = ReadString(entry, L"Publisher");
                    c.version = ReadString(entry, L"DisplayVersion");
                    std::wstring d = ReadString(entry, L"InstallDate");
                    if (d.size() == 8) d = d.substr(0, 4) + L"-" + d.substr(4, 2) + L"-" + d.substr(6, 2);
                    c.date = d;
                    if (DWORD kb = ReadDword(entry, L"EstimatedSize")) {
                        WCHAR s[32];
                        if (kb >= 1024 * 1024)
                            swprintf_s(s, L"%.2f GB", kb / 1048576.0);
                        else
                            swprintf_s(s, L"%.1f MB", kb / 1024.0);
                        c.size = s;
                    }
                    // MSI's /I opens repair-or-remove; /X removes.
                    if (EqualsI(uninstallerName, L"msiexec.exe") || EqualsI(c.file, L"msiexec")) {
                        for (size_t at = 0; at + 1 < c.params.size(); ++at) {
                            if (c.params[at] == L'/' && (c.params[at + 1] == L'I' || c.params[at + 1] == L'i')) {
                                c.params[at + 1] = L'X';
                                break;
                            }
                        }
                    }
                    out.push_back(std::move(c));
                }
            }
            RegCloseKey(entry);
        }
        RegCloseKey(list);
    }
    std::stable_sort(out.begin(), out.end(), [](auto& a, auto& b) { return a.score > b.score; });
    if (out.size() > 5) out.resize(5);
    return out;
}

// ---------------------------------------------------------------------------
// Actions, run off the UI thread
// ---------------------------------------------------------------------------

// Starts a program through Explorer's Shell.Application, so it runs as a
// normal launch rather than as a child of Start (Raymond Chen's method).
bool ShellExecuteFromExplorer(const std::wstring& file, const std::wstring& params, const std::wstring& dir) {
    winrt::com_ptr<IShellWindows> windows;
    if (FAILED(CoCreateInstance(CLSID_ShellWindows, nullptr, CLSCTX_LOCAL_SERVER, IID_PPV_ARGS(windows.put()))))
        return false;
    VARIANT loc{};
    loc.vt = VT_I4;
    loc.lVal = CSIDL_DESKTOP;
    VARIANT empty{};
    long hwnd = 0;
    winrt::com_ptr<IDispatch> disp;
    if (windows->FindWindowSW(&loc, &empty, SWC_DESKTOP, &hwnd, SWFO_NEEDDISPATCH, disp.put()) != S_OK) return false;
    auto sp = disp.try_as<IServiceProvider>();
    if (!sp) return false;
    winrt::com_ptr<IShellBrowser> browser;
    if (FAILED(sp->QueryService(SID_STopLevelBrowser, IID_PPV_ARGS(browser.put())))) return false;
    winrt::com_ptr<IShellView> view;
    if (FAILED(browser->QueryActiveShellView(view.put()))) return false;
    winrt::com_ptr<IDispatch> background;
    if (FAILED(view->GetItemObject(SVGIO_BACKGROUND, IID_PPV_ARGS(background.put())))) return false;
    auto folderView = background.try_as<IShellFolderViewDual>();
    if (!folderView) return false;
    winrt::com_ptr<IDispatch> appDisp;
    if (FAILED(folderView->get_Application(appDisp.put()))) return false;
    auto shell = appDisp.try_as<IShellDispatch2>();
    if (!shell) return false;
    BSTR bFile = SysAllocString(file.c_str());
    VARIANT vParams{}, vDir{}, vOp{}, vShow{};
    vParams.vt = VT_BSTR;
    vParams.bstrVal = SysAllocString(params.c_str());
    vDir.vt = VT_BSTR;
    vDir.bstrVal = SysAllocString(dir.c_str());
    vOp.vt = VT_BSTR;
    vOp.bstrVal = SysAllocString(L"open");
    vShow.vt = VT_I4;
    vShow.lVal = SW_SHOWNORMAL;
    HRESULT hr = shell->ShellExecute(bFile, vParams, vDir, vOp, vShow);
    SysFreeString(bFile);
    VariantClear(&vParams);
    VariantClear(&vDir);
    VariantClear(&vOp);
    return SUCCEEDED(hr);
}

// Worker threads. Wh_ModUninit signals g_stopEvent and waits for all of
// them, so none still runs this DLL's code once it is unloaded.
HANDLE g_stopEvent;
SRWLOCK g_threadsLock = SRWLOCK_INIT;
std::vector<HANDLE> g_threads;

bool Stopping(DWORD waitMs = 0) {
    return WaitForSingleObject(g_stopEvent, waitMs) == WAIT_OBJECT_0;
}

void TrackThread(HANDLE t) {
    AcquireSRWLockExclusive(&g_threadsLock);
    // Forget the ones that have already finished.
    g_threads.erase(std::remove_if(g_threads.begin(), g_threads.end(),
                                   [](HANDLE h) {
                                       if (WaitForSingleObject(h, 0) != WAIT_OBJECT_0) return false;
                                       CloseHandle(h);
                                       return true;
                                   }),
                    g_threads.end());
    g_threads.push_back(t);
    ReleaseSRWLockExclusive(&g_threadsLock);
}

void JoinThreads() {
    std::vector<HANDLE> threads;
    AcquireSRWLockExclusive(&g_threadsLock);
    threads.swap(g_threads);
    ReleaseSRWLockExclusive(&g_threadsLock);
    for (HANDLE h : threads) {
        WaitForSingleObject(h, INFINITE);
        CloseHandle(h);
    }
}

template <class F>
void RunOnThread(F&& f) {
    if (Stopping()) return;
    auto* fn = new std::function<void()>(std::forward<F>(f));
    HANDLE t = CreateThread(
        nullptr, 0,
        [](LPVOID p) -> DWORD {
            auto* fn = (std::function<void()>*)p;
            HRESULT hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
            try {
                (*fn)();
            } catch (...) {
            }
            if (SUCCEEDED(hr)) CoUninitialize();
            delete fn;
            return 0;
        },
        fn, 0, nullptr);
    if (t)
        TrackThread(t);
    else
        delete fn;
}

// Closes Start, so what we launch is not left behind it: Escape while Start
// still has the foreground (our flyout is already hidden). Also lets the
// launched program take the foreground.
void CloseStart() {
    AllowSetForegroundWindow(ASFW_ANY);
    HWND fg = GetForegroundWindow();
    DWORD pid = 0;
    if (!fg || !GetWindowThreadProcessId(fg, &pid) || pid != GetCurrentProcessId()) return;
    INPUT in[2] = {};
    in[0].type = in[1].type = INPUT_KEYBOARD;
    in[0].ki.wVk = in[1].ki.wVk = VK_ESCAPE;
    in[1].ki.dwFlags = KEYEVENTF_KEYUP;
    SendInput(2, in, sizeof(INPUT));
}

void RunUninstaller(Candidate c) {
    RunOnThread([c] {
        bool ok = ShellExecuteFromExplorer(c.file, c.params, DirOf(c.file));
        Wh_Log(L"run %s %s via Explorer: %d", c.file.c_str(), c.params.c_str(), ok);
        if (!ok) {
            // Explorer not reachable: start it from here instead.
            ShellExecute(nullptr, L"open", c.file.c_str(), c.params.c_str(), DirOf(c.file).c_str(), SW_SHOWNORMAL);
        }
    });
}

// Opens Installed apps and types the name into its search box (UI Automation:
// the box's AutomationId is Settings' own, stable across builds so far).
void OpenSettingsFiltered(std::wstring filter) {
    RunOnThread([filter] {
        if (!ShellExecuteFromExplorer(L"ms-settings:appsfeatures", L"", L""))
            ShellExecute(nullptr, L"open", L"ms-settings:appsfeatures", nullptr, nullptr, SW_SHOWNORMAL);
        if (filter.empty()) return;
        winrt::com_ptr<IUIAutomation> uia;
        if (FAILED(CoCreateInstance(CLSID_CUIAutomation, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(uia.put()))))
            return;
        winrt::com_ptr<IUIAutomationElement> root;
        if (FAILED(uia->GetRootElement(root.put()))) return;
        VARIANT aid{};
        aid.vt = VT_BSTR;
        aid.bstrVal = SysAllocString(L"SystemSettings_StorageSense_AppSizesListFilter_DisplayStringValue");
        winrt::com_ptr<IUIAutomationCondition> byId;
        uia->CreatePropertyCondition(UIA_AutomationIdPropertyId, aid, byId.put());
        VARIANT cls{};
        cls.vt = VT_BSTR;
        cls.bstrVal = SysAllocString(L"ApplicationFrameWindow");
        winrt::com_ptr<IUIAutomationCondition> byClass;
        uia->CreatePropertyCondition(UIA_ClassNamePropertyId, cls, byClass.put());
        bool done = false;
        for (int tries = 0; tries < 40 && !done; ++tries) {
            if (Stopping(250)) break;
            winrt::com_ptr<IUIAutomationElementArray> frames;
            if (FAILED(root->FindAll(TreeScope_Children, byClass.get(), frames.put())) || !frames) continue;
            int n = 0;
            frames->get_Length(&n);
            for (int i = 0; i < n && !done; ++i) {
                winrt::com_ptr<IUIAutomationElement> frame, box;
                frames->GetElement(i, frame.put());
                if (!frame || FAILED(frame->FindFirst(TreeScope_Descendants, byId.get(), box.put())) || !box)
                    continue;
                winrt::com_ptr<IUIAutomationValuePattern> value;
                if (SUCCEEDED(box->GetCurrentPatternAs(UIA_ValuePatternId, IID_PPV_ARGS(value.put()))) && value) {
                    BSTR text = SysAllocString(filter.c_str());
                    done = SUCCEEDED(value->SetValue(text));
                    SysFreeString(text);
                }
            }
        }
        Wh_Log(L"settings filter '%s': %d", filter.c_str(), done);
        VariantClear(&aid);
        VariantClear(&cls);
    });
}

// ---------------------------------------------------------------------------
// The details flyout
// ---------------------------------------------------------------------------

winrt::weak_ref<wuxc::Flyout> g_detailsFlyout;

wuxc::TextBlock Text(const std::wstring& s, double size = 12, bool bold = false, double opacity = 1) {
    wuxc::TextBlock t;
    t.Text(winrt::hstring(s));
    t.FontSize(size);
    t.TextWrapping(wux::TextWrapping::Wrap);
    t.IsTextSelectionEnabled(true);
    t.Opacity(opacity);
    if (bold) t.FontWeight(winrt::Windows::UI::Text::FontWeights::SemiBold());
    return t;
}

// Label: value rows.
void AddRow(wuxc::Grid const& grid, const std::wstring& label, const std::wstring& value) {
    if (value.empty()) return;
    int row = grid.RowDefinitions().Size();
    grid.RowDefinitions().Append(wuxc::RowDefinition());
    auto l = Text(label, 12, false, 0.7);
    l.Margin(wux::ThicknessHelper::FromLengths(0, 1, 12, 1));
    wuxc::Grid::SetRow(l, row);
    auto v = Text(value);
    v.Margin(wux::ThicknessHelper::FromLengths(0, 1, 0, 1));
    wuxc::Grid::SetRow(v, row);
    wuxc::Grid::SetColumn(v, 1);
    grid.Children().Append(l);
    grid.Children().Append(v);
}

wuxc::Grid DetailsGrid() {
    wuxc::Grid g;
    wuxc::ColumnDefinition c0, c1;
    c0.Width(wux::GridLengthHelper::FromValueAndType(0, wux::GridUnitType::Auto));
    c1.Width(wux::GridLengthHelper::FromValueAndType(1, wux::GridUnitType::Star));
    g.ColumnDefinitions().Append(c0);
    g.ColumnDefinitions().Append(c1);
    return g;
}

wuxc::Grid CandidateGrid(const Candidate& c) {
    auto g = DetailsGrid();
    AddRow(g, L"Program", c.name);
    AddRow(g, L"Publisher", c.publisher);
    AddRow(g, L"Version", c.version);
    AddRow(g, L"Installed", c.date);
    AddRow(g, L"Size", c.size);
    AddRow(g, L"Folder", c.location);
    AddRow(g, L"Runs", c.file + (c.params.empty() ? L"" : L" " + c.params));
    AddRow(g, L"Registry", c.hive + L"\\...\\Uninstall\\" + c.key);
    AddRow(g, L"Match", c.why);
    return g;
}

void ShowDetails(wux::FrameworkElement const& anchor, StartApp app) {
    ResolveTarget(app);
    auto candidates = std::make_shared<std::vector<Candidate>>(FindCandidates(app));
    auto selected = std::make_shared<size_t>(0);
    Wh_Log(L"details: %s target=%s candidates=%zu", app.aumid.c_str(), app.target.c_str(), candidates->size());

    wuxc::Flyout flyout;
    wuxc::StackPanel panel;
    // Fits inside the flyout's own width limit, which is widened below.
    panel.Width(480);
    panel.Spacing(8);

    panel.Children().Append(Text(L"Uninstall " + app.name + L"?", 16, true));

    auto startGrid = DetailsGrid();
    AddRow(startGrid, L"Start entry", app.name);
    AddRow(startGrid, L"App ID", app.aumid);
    AddRow(startGrid, L"Target", app.target + (app.args.empty() ? L"" : L" " + app.args));
    panel.Children().Append(startGrid);

    wuxc::Border divider;
    divider.Height(1);
    divider.Opacity(0.2);
    divider.Background(wux::Media::SolidColorBrush(winrt::Windows::UI::Colors::Gray()));
    panel.Children().Append(divider);

    wuxc::ContentControl details;
    details.HorizontalContentAlignment(wux::HorizontalAlignment::Stretch);
    if (candidates->empty()) {
        details.Content(Text(L"No installed program matches this app, so it has no uninstaller. It is probably "
                             L"portable: delete its folder to remove it, and unpin it from Start.",
                             12, false, 0.85));
    } else {
        if (candidates->size() > 1) {
            auto note = Text(candidates->at(0).score == candidates->at(1).score
                                 ? L"Several programs match equally well. Pick the right one:"
                                 : L"Best match shown. Others that also fit:",
                             12, false, 0.85);
            panel.Children().Append(note);
            wuxc::ComboBox pick;
            pick.HorizontalAlignment(wux::HorizontalAlignment::Stretch);
            for (auto& c : *candidates) {
                pick.Items().Append(winrt::box_value(winrt::hstring(
                    c.name + (c.version.empty() ? L"" : L" " + c.version) +
                    (c.publisher.empty() ? L"" : L" (" + c.publisher + L")"))));
            }
            pick.SelectedIndex(0);
            pick.SelectionChanged([candidates, selected, weakDetails = winrt::make_weak(details),
                                   weakPick = winrt::make_weak(pick)](auto&&, auto&&) {
                auto d = weakDetails.get();
                auto p = weakPick.get();
                if (!d || !p || p.SelectedIndex() < 0) return;
                *selected = (size_t)p.SelectedIndex();
                d.Content(CandidateGrid(candidates->at(*selected)));
            });
            panel.Children().Append(pick);
        }
        details.Content(CandidateGrid(candidates->at(0)));
    }
    panel.Children().Append(details);

    wuxc::StackPanel buttons;
    buttons.Orientation(wuxc::Orientation::Horizontal);
    buttons.HorizontalAlignment(wux::HorizontalAlignment::Right);
    buttons.Spacing(8);
    buttons.Margin(wux::ThicknessHelper::FromLengths(0, 6, 0, 0));
    auto weakFlyout = winrt::make_weak(flyout);

    if (!candidates->empty()) {
        wuxc::Button run;
        run.Content(winrt::box_value(L"Run uninstaller"));
        try {
            auto res = wux::Application::Current().Resources();
            auto key = winrt::box_value(L"AccentButtonStyle");
            if (res.HasKey(key)) run.Style(res.Lookup(key).as<wux::Style>());
        } catch (...) {
        }
        run.Click([candidates, selected, weakFlyout](auto&&, auto&&) {
            if (auto f = weakFlyout.get()) f.Hide();
            CloseStart();
            RunUninstaller(candidates->at(*selected));
        });
        buttons.Children().Append(run);
    }
    wuxc::Button settings;
    settings.Content(winrt::box_value(L"Open in Settings"));
    settings.Click([candidates, selected, weakFlyout, name = app.name](auto&&, auto&&) {
        if (auto f = weakFlyout.get()) f.Hide();
        CloseStart();
        OpenSettingsFiltered(candidates->empty() ? name : candidates->at(*selected).name);
    });
    buttons.Children().Append(settings);
    if (candidates->empty() && !app.target.empty() && GetFileAttributes(app.target.c_str()) != INVALID_FILE_ATTRIBUTES) {
        wuxc::Button folder;
        folder.Content(winrt::box_value(L"Open folder"));
        folder.Click([weakFlyout, target = app.target](auto&&, auto&&) {
            if (auto f = weakFlyout.get()) f.Hide();
            CloseStart();
            RunOnThread([target] {
                if (!ShellExecuteFromExplorer(L"explorer.exe", L"/select,\"" + target + L"\"", L""))
                    ShellExecute(nullptr, L"open", L"explorer.exe", (L"/select,\"" + target + L"\"").c_str(),
                                 nullptr, SW_SHOWNORMAL);
            });
        });
        buttons.Children().InsertAt(0, folder);
    }
    wuxc::Button cancel;
    cancel.Content(winrt::box_value(L"Cancel"));
    cancel.Click([weakFlyout](auto&&, auto&&) {
        if (auto f = weakFlyout.get()) f.Hide();
    });
    buttons.Children().Append(cancel);
    panel.Children().Append(buttons);

    flyout.Content(panel);
    // FlyoutPresenter caps width at ~456 and scrolls sideways past it.
    {
        wux::Style style;
        style.TargetType(wux::Interop::TypeName{L"Windows.UI.Xaml.Controls.FlyoutPresenter",
                                                 wux::Interop::TypeKind::Metadata});
        style.Setters().Append(wux::Setter(wux::FrameworkElement::MaxWidthProperty(), winrt::box_value(560.0)));
        style.Setters().Append(wux::Setter(wuxc::ScrollViewer::HorizontalScrollBarVisibilityProperty(),
                                           winrt::box_value(wuxc::ScrollBarVisibility::Disabled)));
        style.Setters().Append(wux::Setter(wuxc::ScrollViewer::HorizontalScrollModeProperty(),
                                           winrt::box_value(wuxc::ScrollMode::Disabled)));
        flyout.FlyoutPresenterStyle(style);
    }
    // Above the tile by default; a tile in Start's top half has no room above.
    try {
        auto content = wux::Window::Current().Content();
        double tileY = anchor.TransformToVisual(content).TransformPoint(wf::Point{0, 0}).Y;
        auto bounds = wux::Window::Current().Bounds();
        if (tileY < bounds.Height / 2) flyout.Placement(wuxc::Primitives::FlyoutPlacementMode::Bottom);
    } catch (...) {
    }
    g_detailsFlyout = winrt::make_weak(flyout);
    flyout.ShowAt(anchor);
}

// ---------------------------------------------------------------------------
// Start's menu: find the tile, add the item
// ---------------------------------------------------------------------------

constexpr wchar_t kItemTag[] = L"StartUninstallPlus";

struct MenuTarget {
    StartApp app;
    winrt::weak_ref<wux::FrameworkElement> tile;
};
MenuTarget g_target;

// The items this mod put into Start's menus, with the menu they are in and
// their Click handler (code in this DLL). On unload the handler is revoked and
// the item removed, on Start's UI thread, which owns them.
struct InjectedItem {
    winrt::weak_ref<wuxc::MenuFlyoutItem> item;
    winrt::weak_ref<wuxc::ItemsControl> presenter;
    winrt::event_token click;
};
std::vector<InjectedItem> g_injected;

// Pinned/All apps: the tile's container carries "tile-W~<app ID>" (Win32) as
// its automation ID. Recommended: no automation ID; the item's Grid has
// x:Name "W~<APP ID>" (uppercased). Also accepts any automation ID with "W~".
std::optional<std::wstring> Win32AppId(wux::FrameworkElement const& fe) {
    std::wstring xname(fe.Name());
    if (xname.size() > 2 && xname.compare(0, 2, L"W~") == 0) return xname.substr(2);
    std::wstring aid(wux::Automation::AutomationProperties::GetAutomationId(fe));
    size_t at = aid.find(L"W~");
    if (at == std::wstring::npos || (at > 0 && aid[at - 1] != L'-' && aid[at - 1] != L'_')) return std::nullopt;
    std::wstring id = aid.substr(at + 2);
    if (id.empty()) return std::nullopt;
    return id;
}

bool IsAppContainer(wux::FrameworkElement const& fe) {
    std::wstring xname(fe.Name());
    if (xname.size() > 2 && xname[1] == L'~' && (xname[0] == L'W' || xname[0] == L'P')) return true;
    std::wstring aid(wux::Automation::AutomationProperties::GetAutomationId(fe));
    return aid.find(L'~') != std::wstring::npos;
}

// Recommended: the "W~..." Grid covers only the icon and text, so a click on
// the item's padding hits its GridViewItem instead. Look inside that.
wux::FrameworkElement AppContainerInside(wux::DependencyObject const& root, int depth = 8) {
    int n = wux::Media::VisualTreeHelper::GetChildrenCount(root);
    for (int i = 0; i < n; i++) {
        auto child = wux::Media::VisualTreeHelper::GetChild(root, i);
        if (auto fe = child.try_as<wux::FrameworkElement>(); fe && IsAppContainer(fe)) return fe;
        if (depth > 0) {
            if (auto found = AppContainerInside(child, depth - 1)) return found;
        }
    }
    return nullptr;
}

// The app container the menu belongs to: under the mouse (the menu opens
// where it was right-clicked), else the focused one (Shift+F10, menu key).
wux::FrameworkElement FindMenuTile() {
    auto content = wux::Window::Current().Content();
    if (!content) return nullptr;
    POINT pt;
    HWND core = GetCoreWnd();
    if (core && GetCursorPos(&pt) && ScreenToClient(core, &pt)) {
        double scale = content.XamlRoot() ? content.XamlRoot().RasterizationScale() : 1.0;
        wf::Point p{float(pt.x / scale), float(pt.y / scale)};
        for (auto const& el : wux::Media::VisualTreeHelper::FindElementsInHostCoordinates(p, content, false)) {
            if (auto fe = el.try_as<wux::FrameworkElement>(); fe && IsAppContainer(fe)) return fe;
            if (auto item = el.try_as<wuxc::Primitives::SelectorItem>()) {
                if (auto inside = AppContainerInside(item)) return inside;
                break;  // the list item under the mouse has no app ID: not an app
            }
        }
    }
    if (auto focused = wux::Input::FocusManager::GetFocusedElement().try_as<wux::DependencyObject>()) {
        for (auto node = focused; node; node = wux::Media::VisualTreeHelper::GetParent(node)) {
            if (auto fe = node.try_as<wux::FrameworkElement>(); fe && IsAppContainer(fe)) return fe;
        }
    }
    return nullptr;
}

wuxc::TextBlock TitleInside(wux::DependencyObject const& root, int depth = 6) {
    int n = wux::Media::VisualTreeHelper::GetChildrenCount(root);
    for (int i = 0; i < n; i++) {
        auto child = wux::Media::VisualTreeHelper::GetChild(root, i);
        if (auto tb = child.try_as<wuxc::TextBlock>(); tb && tb.Name() == L"Title") return tb;
        if (depth > 0) {
            if (auto tb = TitleInside(child, depth - 1)) return tb;
        }
    }
    return nullptr;
}

// Recommended items have no name on the Grid; use their "Title" text.
std::wstring TileName(wux::FrameworkElement const& tile) {
    std::wstring name(wux::Automation::AutomationProperties::GetName(tile));
    if (!name.empty()) return name;
    // Search this item's own subtree: FindName would search the whole list's
    // name scope and return another item's "Title".
    if (auto title = TitleInside(tile)) return std::wstring(title.Text());
    return name;
}

wuxc::MenuFlyoutItem MakeOurItem(winrt::event_token& click) {
    wuxc::MenuFlyoutItem item;
    // Named apart from Start's own "Uninstall", which sits right above it.
    item.Text(L"Find uninstaller…");
    item.Tag(winrt::box_value(kItemTag));
    wuxc::FontIcon icon;
    icon.Glyph(L"");  // info
    item.Icon(icon);
    click = item.Click([](auto&&, auto&&) {
        auto target = g_target;
        auto tile = target.tile.get();
        if (!tile) return;
        // After the menu has closed.
        tile.Dispatcher().RunAsync(winrt::Windows::UI::Core::CoreDispatcherPriority::Low, [target] {
            if (auto anchor = target.tile.get()) {
                try {
                    ShowDetails(anchor, target.app);
                } catch (...) {
                    Wh_Log(L"ShowDetails error %08X", winrt::to_hresult());
                }
            }
        });
    });
    return item;
}

bool IsOurs(wf::IInspectable const& o) {
    auto item = o.try_as<wuxc::MenuFlyoutItem>();
    return item && winrt::unbox_value_or<winrt::hstring>(item.Tag(), L"") == kItemTag;
}

void RemoveOurs(wfc::IVector<wuxc::MenuFlyoutItemBase> const& items) {
    for (uint32_t i = items.Size(); i-- > 0;)
        if (IsOurs(items.GetAt(i))) items.RemoveAt(i);
}

// Start's own Uninstall item has this icon in every display language.
constexpr wchar_t kStockUninstallGlyph[] = L"\uE74D";

void OnStockUninstallItem(wuxc::MenuFlyoutItem const& stock) {
    // The menu is still being laid out; change it once that is done.
    stock.Dispatcher().RunAsync(
        winrt::Windows::UI::Core::CoreDispatcherPriority::Normal, [weakStock = winrt::make_weak(stock)] {
            auto stock = weakStock.get();
            if (!stock) return;
            try {
                wuxc::ItemsControl presenter{nullptr};
                for (wux::DependencyObject p = stock; p; p = wux::Media::VisualTreeHelper::GetParent(p)) {
                    if (auto mp = p.try_as<wuxc::MenuFlyoutPresenter>()) {
                        presenter = mp;
                        break;
                    }
                }
                if (!presenter) {
                    Wh_Log(L"no presenter");
                    return;
                }
                auto items = presenter.ItemsSource().try_as<wfc::IVector<wuxc::MenuFlyoutItemBase>>();
                if (!items) {
                    Wh_Log(L"presenter items not a MenuFlyoutItemBase vector: %s",
                           presenter.ItemsSource() ? winrt::get_class_name(presenter.ItemsSource()).c_str()
                                                   : L"null");
                    return;
                }
                RemoveOurs(items);
                auto tile = FindMenuTile();
                if (!tile) {
                    Wh_Log(L"no app tile under cursor/focus");
                    return;
                }
                auto id = Win32AppId(tile);
                if (!id) {  // a packaged app: Start's own item does it
                    Wh_Log(L"not a Win32 tile: aid='%s' name='%s'",
                           std::wstring(wux::Automation::AutomationProperties::GetAutomationId(tile)).c_str(),
                           tile.Name().c_str());
                    return;
                }
                g_target.app = StartApp{*id, TileName(tile)};
                g_target.tile = winrt::make_weak(tile);
                uint32_t index = 0;
                if (!items.IndexOf(stock, index)) index = items.Size() - 1;
                winrt::event_token click{};
                auto item = MakeOurItem(click);
                items.InsertAt(index + 1, item);
                // Menus are rebuilt each time they open; forget items Start has let go of.
                g_injected.erase(std::remove_if(g_injected.begin(), g_injected.end(),
                                                [](const InjectedItem& i) { return !i.item.get(); }),
                                 g_injected.end());
                g_injected.push_back({winrt::make_weak(item), winrt::make_weak(presenter), click});
                Wh_Log(L"added item for %s (%s)", g_target.app.name.c_str(), id->c_str());
            } catch (...) {
                Wh_Log(L"menu error %08X", winrt::to_hresult());
            }
        });
}

bool IsStockUninstallItem(wuxc::MenuFlyoutItem const& item) {
    if (IsOurs(item)) return false;
    if (auto icon = item.Icon().try_as<wuxc::FontIcon>(); icon && icon.Glyph() == kStockUninstallGlyph) return true;
    return item.Text() == L"Uninstall";
}

void OnElementAdded(wux::FrameworkElement const& fe, PCWSTR type) {
    if (!type || wcscmp(type, L"Windows.UI.Xaml.Controls.MenuFlyoutItem") != 0) return;
    auto item = fe.try_as<wuxc::MenuFlyoutItem>();
    if (item && IsStockUninstallItem(item)) OnStockUninstallItem(item);
}

// Before unloading, on Start's UI thread: no item or flyout of ours may
// outlive this DLL's code.
void DetachAll() {
    for (auto& injected : g_injected) {
        try {
            auto item = injected.item.get();
            if (!item) continue;
            item.Click(injected.click);
            item.Visibility(wux::Visibility::Collapsed);
            if (auto presenter = injected.presenter.get()) {
                if (auto items = presenter.ItemsSource().try_as<wfc::IVector<wuxc::MenuFlyoutItemBase>>()) {
                    uint32_t index = 0;
                    if (items.IndexOf(item, index)) items.RemoveAt(index);
                }
            }
        } catch (...) {
        }
    }
    g_injected.clear();
    if (auto f = g_detailsFlyout.get()) {
        try {
            f.Hide();
            f.Content(nullptr);
        } catch (...) {
        }
    }
    g_target = {};
}

// ---------------------------------------------------------------------------
// XAML diagnostics TAP (from Start Menu Styler / TranslucentTB ExplorerTAP)
// ---------------------------------------------------------------------------

static constexpr GUID IID_IXamlDiagnosticsTestHooks = {
    0x735941a2, 0x3ee3, 0x495a, {0x8d, 0xa9, 0x97, 0x26, 0x27, 0x00, 0x30, 0x75}};

struct IXamlDiagnosticsTestHooks : IUnknown {
    virtual HRESULT STDMETHODCALLTYPE UnregisterInstance(InstanceHandle handle) = 0;
    virtual HRESULT STDMETHODCALLTYPE TryGetDispatcherQueueForObject(InstanceHandle handle, void** dispatcherQueue) = 0;
};

class VisualTreeWatcher
    : public winrt::implements<VisualTreeWatcher, IVisualTreeServiceCallback2, winrt::non_agile> {
public:
    VisualTreeWatcher(winrt::com_ptr<IUnknown> site)
        : m_diag(site.as<IXamlDiagnostics>()) {
        m_diag->QueryInterface(IID_IXamlDiagnosticsTestHooks, m_hooks.put_void());
        // Advising from this thread can hang; the styler does it from a new one.
        // The thread holds a reference, taken before it can run.
        AddRef();
        HANDLE thread = CreateThread(
            nullptr, 0,
            [](LPVOID p) -> DWORD {
                auto w = reinterpret_cast<VisualTreeWatcher*>(p);
                g_reportCompositionDiagAsDisabled = true;
                HRESULT hr = w->m_diag.as<IVisualTreeService3>()->AdviseVisualTreeChange(w);
                g_reportCompositionDiagAsDisabled = false;
                w->Release();
                if (FAILED(hr)) Wh_Log(L"Advise failed %08X", hr);
                return 0;
            },
            this, 0, nullptr);
        if (thread)
            TrackThread(thread);
        else
            Release();
    }

    void Unadvise() {
        m_diag.as<IVisualTreeService3>()->UnadviseVisualTreeChange(this);
    }

private:
    HRESULT STDMETHODCALLTYPE OnVisualTreeChange(ParentChildRelation relation, VisualElement element,
                                                 VisualMutationType mutationType) override try {
        if (GetCurrentThreadId() != g_targetThreadId) return S_OK;
        if (mutationType == Add) {
            try {
                wf::IInspectable obj;
                winrt::check_hresult(m_diag->GetIInspectableFromHandle(
                    element.Handle, reinterpret_cast<::IInspectable**>(winrt::put_abi(obj))));
                if (auto fe = obj.try_as<wux::FrameworkElement>()) {
                    OnElementAdded(fe, element.Type);
                }
            } catch (...) {
            }
            // Hand back the reference the diagnostics took for us.
            if (m_hooks) {
                m_hooks->UnregisterInstance(element.Handle);
                m_hooks->UnregisterInstance(relation.Parent);
            }
        }
        return S_OK;
    } catch (...) {
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE OnElementStateChanged(InstanceHandle, VisualElementState, LPCWSTR) noexcept override {
        return S_OK;
    }

    winrt::com_ptr<IXamlDiagnostics> m_diag;
    winrt::com_ptr<IXamlDiagnosticsTestHooks> m_hooks;
};

[[clang::no_destroy]] winrt::com_ptr<VisualTreeWatcher> g_watcher;

// {6F0B7C2E-3A51-4D7B-9E2C-5B1A0D4E8C71}
static constexpr CLSID CLSID_UninstallPlusTAP = {
    0x6f0b7c2e, 0x3a51, 0x4d7b, {0x9e, 0x2c, 0x5b, 0x1a, 0x0d, 0x4e, 0x8c, 0x71}};

class UninstallPlusTAP : public winrt::implements<UninstallPlusTAP, IObjectWithSite, winrt::non_agile> {
public:
    HRESULT STDMETHODCALLTYPE SetSite(IUnknown* pUnkSite) override try {
        if (g_watcher) {
            g_watcher->Unadvise();
            g_watcher = nullptr;
        }
        site.copy_from(pUnkSite);
        if (site) {
            // InitializeXamlDiagnosticsEx loaded us once more.
            FreeLibrary(GetCurrentModuleHandle());
            g_watcher = winrt::make_self<VisualTreeWatcher>(site);
        }
        return S_OK;
    } catch (...) {
        return winrt::to_hresult();
    }
    HRESULT STDMETHODCALLTYPE GetSite(REFIID riid, void** ppvSite) noexcept override {
        return site.as(riid, ppvSite);
    }

private:
    winrt::com_ptr<IUnknown> site;
};

template <class T>
struct SimpleFactory : winrt::implements<SimpleFactory<T>, IClassFactory, winrt::non_agile> {
    HRESULT STDMETHODCALLTYPE CreateInstance(IUnknown* outer, REFIID riid, void** ppv) override try {
        *ppv = nullptr;
        if (outer) return CLASS_E_NOAGGREGATION;
        return winrt::make<T>().as(riid, ppv);
    } catch (...) {
        return winrt::to_hresult();
    }
    HRESULT STDMETHODCALLTYPE LockServer(BOOL) noexcept override { return S_OK; }
};

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdll-attribute-on-redeclaration"
__declspec(dllexport) _Use_decl_annotations_ STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv) try {
    if (rclsid == CLSID_UninstallPlusTAP) {
        *ppv = nullptr;
        return winrt::make<SimpleFactory<UninstallPlusTAP>>().as(riid, ppv);
    }
    return CLASS_E_CLASSNOTAVAILABLE;
} catch (...) {
    return winrt::to_hresult();
}
__declspec(dllexport) _Use_decl_annotations_ STDAPI DllCanUnloadNow() {
    return winrt::get_module_lock() ? S_FALSE : S_OK;
}
#pragma clang diagnostic pop

using PFN_IXDE = decltype(&InitializeXamlDiagnosticsEx);

HRESULT InjectTAP() noexcept {
    HMODULE module = GetCurrentModuleHandle();
    if (!module) return HRESULT_FROM_WIN32(GetLastError());
    WCHAR location[MAX_PATH];
    switch (GetModuleFileName(module, location, ARRAYSIZE(location))) {
        case 0:
        case ARRAYSIZE(location):
            return HRESULT_FROM_WIN32(GetLastError());
    }
    HMODULE wuxDll = LoadLibraryEx(L"Windows.UI.Xaml.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!wuxDll) return HRESULT_FROM_WIN32(GetLastError());
    auto ixde = reinterpret_cast<PFN_IXDE>(GetProcAddress(wuxDll, "InitializeXamlDiagnosticsEx"));
    if (!ixde) return HRESULT_FROM_WIN32(GetLastError());
    HRESULT hr = E_FAIL;
    for (int i = 0; i < 10000; i++) {
        WCHAR name[64];
        wsprintf(name, L"VisualDiagConnection%d", i + 1);
        hr = ixde(name, GetCurrentProcessId(), L"", location, CLSID_UninstallPlusTAP, nullptr);
        if (hr != HRESULT_FROM_WIN32(ERROR_NOT_FOUND)) break;
    }
    return hr;
}

void InitializeTap() {
    DWORD none = 0;
    if (!g_targetThreadId.compare_exchange_strong(none, GetCurrentThreadId())) return;
    HRESULT hr = InjectTAP();
    Wh_Log(L"InjectTAP: %08X", hr);
}

// ---------------------------------------------------------------------------
// Hooks and lifecycle
// ---------------------------------------------------------------------------

using RegOpenKeyExW_t = decltype(&RegOpenKeyExW);
RegOpenKeyExW_t RegOpenKeyExW_Original;
LSTATUS WINAPI RegOpenKeyExW_Hook(HKEY hKey, LPCWSTR sub, DWORD opt, REGSAM sam, PHKEY out) {
    LSTATUS r = RegOpenKeyExW_Original(hKey, sub, opt, sam, out);
    if (r == ERROR_SUCCESS || !g_reportCompositionDiagAsDisabled || hKey != HKEY_LOCAL_MACHINE || !sub ||
        _wcsicmp(sub, L"Software\\Microsoft\\XAML\\Debug") != 0)
        return r;
    return RegOpenKeyExW_Original(HKEY_LOCAL_MACHINE, L"Software\\Microsoft", opt, sam, out);
}

using RegQueryValueExW_t = decltype(&RegQueryValueExW);
RegQueryValueExW_t RegQueryValueExW_Original;
LSTATUS WINAPI RegQueryValueExW_Hook(HKEY hKey, LPCWSTR name, LPDWORD res, LPDWORD type, LPBYTE data, LPDWORD cb) {
    if (!g_reportCompositionDiagAsDisabled || !name || _wcsicmp(name, L"DisableCompositionDiag") != 0)
        return RegQueryValueExW_Original(hKey, name, res, type, data, cb);
    if (type) *type = REG_DWORD;
    if (data && (!cb || *cb < sizeof(DWORD))) {
        if (cb) *cb = sizeof(DWORD);
        return ERROR_MORE_DATA;
    }
    if (data) *reinterpret_cast<DWORD*>(data) = 1;
    if (cb) *cb = sizeof(DWORD);
    return ERROR_SUCCESS;
}

using CreateWindowInBand_t = HWND(WINAPI*)(DWORD, LPCWSTR, LPCWSTR, DWORD, int, int, int, int, HWND, HMENU,
                                           HINSTANCE, PVOID, DWORD);
CreateWindowInBand_t CreateWindowInBand_Original;
HWND WINAPI CreateWindowInBand_Hook(DWORD ex, LPCWSTR cls, LPCWSTR name, DWORD style, int x, int y, int w, int h,
                                    HWND parent, HMENU menu, HINSTANCE inst, PVOID param, DWORD band) {
    HWND hWnd = CreateWindowInBand_Original(ex, cls, name, style, x, y, w, h, parent, menu, inst, param, band);
    if (hWnd && ((ULONG_PTR)cls & ~(ULONG_PTR)0xffff) && _wcsicmp(cls, L"Windows.UI.Core.CoreWindow") == 0) {
        InitializeTap();
    }
    return hWnd;
}

using CreateWindowInBandEx_t = HWND(WINAPI*)(DWORD, LPCWSTR, LPCWSTR, DWORD, int, int, int, int, HWND, HMENU,
                                             HINSTANCE, PVOID, DWORD, DWORD);
CreateWindowInBandEx_t CreateWindowInBandEx_Original;
HWND WINAPI CreateWindowInBandEx_Hook(DWORD ex, LPCWSTR cls, LPCWSTR name, DWORD style, int x, int y, int w, int h,
                                      HWND parent, HMENU menu, HINSTANCE inst, PVOID param, DWORD band, DWORD flags) {
    HWND hWnd =
        CreateWindowInBandEx_Original(ex, cls, name, style, x, y, w, h, parent, menu, inst, param, band, flags);
    if (hWnd && ((ULONG_PTR)cls & ~(ULONG_PTR)0xffff) && _wcsicmp(cls, L"Windows.UI.Core.CoreWindow") == 0) {
        InitializeTap();
    }
    return hWnd;
}

HWND GetCoreWnd() {
    HWND found = nullptr;
    EnumWindows(
        [](HWND hWnd, LPARAM lParam) -> BOOL {
            DWORD pid = 0;
            if (!GetWindowThreadProcessId(hWnd, &pid) || pid != GetCurrentProcessId()) return TRUE;
            WCHAR cls[32];
            if (!GetClassName(hWnd, cls, ARRAYSIZE(cls))) return TRUE;
            if (_wcsicmp(cls, L"Windows.UI.Core.CoreWindow") == 0) {
                *(HWND*)lParam = hWnd;
                return FALSE;
            }
            return TRUE;
        },
        (LPARAM)&found);
    return found;
}

using RunProc = void(WINAPI*)(PVOID);
bool RunFromWindowThread(HWND hWnd, RunProc proc, PVOID procParam) {
    static const UINT msg = RegisterWindowMessage(L"Windhawk_RunFromWindowThread_" WH_MOD_ID);
    struct Param {
        RunProc proc;
        PVOID procParam;
    };
    DWORD tid = GetWindowThreadProcessId(hWnd, nullptr);
    if (!tid) return false;
    if (tid == GetCurrentThreadId()) {
        proc(procParam);
        return true;
    }
    HHOOK hook = SetWindowsHookEx(
        WH_CALLWNDPROC,
        [](int code, WPARAM wParam, LPARAM lParam) -> LRESULT {
            if (code == HC_ACTION) {
                auto cwp = (const CWPSTRUCT*)lParam;
                if (cwp->message == msg) {
                    auto p = (Param*)cwp->lParam;
                    p->proc(p->procParam);
                }
            }
            return CallNextHookEx(nullptr, code, wParam, lParam);
        },
        nullptr, tid);
    if (!hook) return false;
    Param p{proc, procParam};
    SendMessage(hWnd, msg, 0, (LPARAM)&p);
    UnhookWindowsHookEx(hook);
    return true;
}

BOOL Wh_ModInit() {
    g_stopEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);
    if (!g_stopEvent) return FALSE;
    HMODULE user32 = GetModuleHandle(L"user32.dll");
    if (auto p = (CreateWindowInBand_t)GetProcAddress(user32, "CreateWindowInBand"))
        WindhawkUtils::SetFunctionHook(p, CreateWindowInBand_Hook, &CreateWindowInBand_Original);
    if (auto p = (CreateWindowInBandEx_t)GetProcAddress(user32, "CreateWindowInBandEx"))
        WindhawkUtils::SetFunctionHook(p, CreateWindowInBandEx_Hook, &CreateWindowInBandEx_Original);
    HMODULE kb = GetModuleHandle(L"kernelbase.dll");
    WindhawkUtils::SetFunctionHook((RegOpenKeyExW_t)GetProcAddress(kb, "RegOpenKeyExW"), RegOpenKeyExW_Hook,
                                   &RegOpenKeyExW_Original);
    WindhawkUtils::SetFunctionHook((RegQueryValueExW_t)GetProcAddress(kb, "RegQueryValueExW"),
                                   RegQueryValueExW_Hook, &RegQueryValueExW_Original);
    return TRUE;
}

void Wh_ModAfterInit() {
    if (HWND w = GetCoreWnd()) {
        Wh_Log(L"found core window");
        RunFromWindowThread(w, [](PVOID) { InitializeTap(); }, nullptr);
    }
}

void Wh_ModUninit() {
    // Stop and join the workers first, from this (engine) thread: they may
    // need Start's UI thread, so it must not be blocked while they finish.
    SetEvent(g_stopEvent);
    JoinThreads();
    auto drop = [](PVOID) {
        if (g_watcher) {
            g_watcher->Unadvise();
            g_watcher = nullptr;
        }
        DetachAll();
        g_targetThreadId = 0;
    };
    if (HWND w = GetCoreWnd()) {
        RunFromWindowThread(w, drop, nullptr);
    } else {
        drop(nullptr);
    }
    // A button click that raced the stop event may have started one more.
    JoinThreads();
    CloseHandle(g_stopEvent);
}
