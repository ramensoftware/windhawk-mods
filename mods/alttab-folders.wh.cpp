// ==WindhawkMod==
// @id              alttab-folders
// @name            Alt+Tab Folders
// @description     Group windows into named folders; Alt+Tab cycles only the windows of the active folder
// @version         1.0.0
// @a160k          you
// @include         explorer.exe
// @compilerOptions -ldwmapi -lgdi32 -luser32 -ladvapi32
// ==/WindhawkMod==
// ==WindhawkModReadme==
/*
# Alt+Tab Folders

Group open windows into named folders and limit **Alt+Tab** navigation exclusively to windows within the active folder.

## Hotkeys (Default: Ctrl + Alt + Key)
- **Ctrl + Alt + N:** Create a new folder and add the foreground window
- **Ctrl + Alt + A:** Add the foreground window to the active folder
- **Ctrl + Alt + R:** Remove the foreground window from the active folder
- **Ctrl + Alt + P:** Pin / unpin the process (.exe) to the active folder
- **Ctrl + Alt + F:** Switch to the next folder
- **Ctrl + Alt + G:** Switch to the previous folder
- **Ctrl + Alt + 0:** Show all windows (deactivate folder filter)
- **Ctrl + Alt + D:** Delete the active folder
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- modifiers: ctrlalt
  $name: Hotkey modifiers
  $options:
  - ctrlalt: Ctrl+Alt
  - ctrlshift: Ctrl+Shift
  - winalt: Win+Alt
- overlayMs: 1200
  $name: Toast duration (ms)
- followFocus: true
  $name: Switch active folder when focus moves to a window of another folder
- folders:
  - - name: Work
      $name: Folder name
    - processes: code.exe|chrome.exe
      $name: Process names separated by |
  $name: Rule based folders
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <shellapi.h>
#include <dwmapi.h>
#include <winuser.h>

#include <algorithm>
#include <atomic>
#include <map>
#include <mutex>
#include <set>
#include <string>
#include <vector>

#include <windhawk_api.h>
#include <windhawk_utils.h>

namespace {

enum class Mods { CtrlAlt, CtrlShift, WinAlt };

struct Config {
    Mods mods = Mods::CtrlAlt;
    int overlayMs = 1200;
    bool followFocus = true;
} g_cfg;

std::recursive_mutex g_mu;
std::map<std::string, std::vector<HWND>> g_folders;
std::vector<std::string> g_order;
std::map<std::string, std::set<std::wstring>> g_rules;
std::map<std::string, std::set<std::wstring>> g_pins;
std::string g_active;
std::vector<HWND> g_mru;

bool g_session = false;
std::vector<HWND> g_list;
int g_sel = 0;

std::wstring g_toast;

std::atomic<bool> g_ready{false};
HWND g_overlay = nullptr;
HHOOK g_kbHook = nullptr;
HWINEVENTHOOK g_winEvt = nullptr;
HANDLE g_thread = nullptr;
HANDLE g_startEvt = nullptr;
DWORD g_uiThreadId = 0;
int g_dpi = 96;
HFONT g_font = nullptr;
HFONT g_fontBold = nullptr;

constexpr UINT WM_TOAST = WM_APP + 1;
constexpr UINT_PTR TIMER_HIDE = 1;
constexpr wchar_t kOverlayClass[] = L"WH_AltTabFoldersOverlay";

typedef BOOL(WINAPI* SetForegroundWindow_t)(HWND);
typedef void(WINAPI* SwitchToThisWindow_t)(HWND, BOOL);
SetForegroundWindow_t SetForegroundWindow_Original = nullptr;
SwitchToThisWindow_t SwitchToThisWindow_Original = nullptr;

int S(int v) { return MulDiv(v, g_dpi, 96); }

std::string W2U(const std::wstring& w) {
    if (w.empty()) return {};
    int n = WideCharToMultiByte(CP_UTF8, 0, w.data(), (int)w.size(), nullptr, 0, nullptr, nullptr);
    std::string s(n, '\0');
    WideCharToMultiByte(CP_UTF8, 0, w.data(), (int)w.size(), &s[0], n, nullptr, nullptr);
    return s;
}

std::wstring U2W(const std::string& s) {
    if (s.empty()) return {};
    int n = MultiByteToWideChar(CP_UTF8, 0, s.data(), (int)s.size(), nullptr, 0);
    std::wstring w(n, L'\0');
    MultiByteToWideChar(CP_UTF8, 0, s.data(), (int)s.size(), &w[0], n);
    return w;
}

bool KeyDown(int vk) { return (GetAsyncKeyState(vk) & 0x8000) != 0; }

HWND RootOf(HWND h) {
    if (!h) return nullptr;
    HWND r = GetAncestor(h, GA_ROOT);
    return r ? r : h;
}

std::wstring ExeName(HWND h) {
    DWORD pid = 0;
    GetWindowThreadProcessId(h, &pid);
    if (!pid) return L"";
    HANDLE p = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (!p) return L"";
    wchar_t buf[MAX_PATH * 2];
    DWORD n = ARRAYSIZE(buf);
    std::wstring out;
    if (QueryFullProcessImageNameW(p, 0, buf, &n)) {
        out.assign(buf, n);
        size_t pos = out.find_last_of(L'\\');
        if (pos != std::wstring::npos) out = out.substr(pos + 1);
        CharLowerBuffW(&out[0], (DWORD)out.size());
    }
    CloseHandle(p);
    return out;
}

std::set<std::wstring> SplitProcesses(const std::wstring& s) {
    std::set<std::wstring> out;
    std::wstring cur;
    auto flush = [&] {
        size_t a = cur.find_first_not_of(L" \t");
        size_t b = cur.find_last_not_of(L" \t");
        if (a != std::wstring::npos) {
            std::wstring t = cur.substr(a, b - a + 1);
            CharLowerBuffW(&t[0], (DWORD)t.size());
            out.insert(t);
        }
        cur.clear();
    };
    for (wchar_t c : s) {
        if (c == L'|') flush();
        else cur += c;
    }
    flush();
    return out;
}

bool IsAltTabWindow(HWND h) {
    if (!h || h == g_overlay || !IsWindow(h) || !IsWindowVisible(h)) return false;
    LONG_PTR ex = GetWindowLongPtrW(h, GWL_EXSTYLE);
    bool appWindow = (ex & WS_EX_APPWINDOW) != 0;
    if ((ex & WS_EX_TOOLWINDOW) && !appWindow) return false;
    if ((ex & WS_EX_NOACTIVATE) && !appWindow) return false;
    if (GetWindow(h, GW_OWNER) && !appWindow) return false;
    DWORD cloaked = 0;
    if (SUCCEEDED(DwmGetWindowAttribute(h, 14, &cloaked, sizeof(cloaked))) && cloaked)
        return false;
    if (GetWindowTextLengthW(h) == 0) return false;
    return true;
}

void EnsureFolder(const std::string& name) {
    if (g_folders.find(name) == g_folders.end()) {
        g_folders[name];
        g_order.push_back(name);
    }
}

bool IsMember(const std::string& folder, HWND h) {
    auto it = g_folders.find(folder);
    if (it == g_folders.end()) return false;
    if (std::find(it->second.begin(), it->second.end(), h) != it->second.end()) return true;
    auto r = g_rules.find(folder);
    auto p = g_pins.find(folder);
    bool hasRules = (r != g_rules.end() && !r->second.empty()) || (p != g_pins.end() && !p->second.empty());
    if (!hasRules) return false;
    std::wstring exe = ExeName(h);
    if (exe.empty()) return false;
    if (r != g_rules.end() && r->second.count(exe)) return true;
    if (p != g_pins.end() && p->second.count(exe)) return true;
    return false;
}

std::string FolderOf(HWND h) {
    for (auto& name : g_order)
        if (IsMember(name, h)) return name;
    return {};
}

BOOL CALLBACK EnumWindowsProc(HWND h, LPARAM lp) {
    if (IsAltTabWindow(h)) ((std::vector<HWND>*)lp)->push_back(h);
    return TRUE;
}

std::vector<HWND> LiveMembers(const std::string& folder) {
    std::vector<HWND> all;
    EnumWindows(EnumWindowsProc, (LPARAM)&all);

    std::vector<HWND> out;
    for (HWND h : all)
        if (IsMember(folder, h)) out.push_back(h);

    auto rank = [&](HWND h) {
        auto it = std::find(g_mru.begin(), g_mru.end(), h);
        return it == g_mru.end() ? (size_t)100000 : (size_t)(it - g_mru.begin());
    };
    std::stable_sort(out.begin(), out.end(), [&](HWND a, HWND b) { return rank(a) < rank(b); });

    auto it = g_folders.find(folder);
    if (it != g_folders.end()) {
        auto& v = it->second;
        v.erase(std::remove_if(v.begin(), v.end(), [](HWND h) { return !IsWindow(h); }), v.end());
    }
    return out;
}

void SaveState() {
    std::lock_guard<std::recursive_mutex> lk(g_mu);
    std::wstring s = L"A\t" + U2W(g_active) + L"\n";
    for (auto& n : g_order)
        if (!g_rules.count(n)) s += L"F\t" + U2W(n) + L"\n";
    for (auto& kv : g_pins)
        for (auto& e : kv.second) s += L"P\t" + U2W(kv.first) + L"\t" + e + L"\n";
    
    HKEY hKey;
    if (RegCreateKeyExW(HKEY_CURRENT_USER, L"Software\\WindhawkMods\\AltTabFolders", 0, nullptr, 0, KEY_WRITE, nullptr, &hKey, nullptr) == ERROR_SUCCESS) {
        RegSetValueExW(hKey, L"State", 0, REG_SZ, (const BYTE*)s.c_str(), (DWORD)((s.size() + 1) * sizeof(wchar_t)));
        RegCloseKey(hKey);
    }
}

std::vector<std::wstring> SplitChar(const std::wstring& s, wchar_t sep) {
    std::vector<std::wstring> out;
    std::wstring cur;
    for (wchar_t c : s) {
        if (c == sep) { out.push_back(cur); cur.clear(); }
        else cur += c;
    }
    out.push_back(cur);
    return out;
}

void LoadState() {
    std::wstring buf;
    HKEY hKey;
    if (RegOpenKeyExW(HKEY_CURRENT_USER, L"Software\\WindhawkMods\\AltTabFolders", 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
        DWORD cbData = 0;
        if (RegQueryValueExW(hKey, L"State", nullptr, nullptr, nullptr, &cbData) == ERROR_SUCCESS && cbData > 0) {
            buf.resize(cbData / sizeof(wchar_t));
            RegQueryValueExW(hKey, L"State", nullptr, nullptr, (LPBYTE)&buf[0], &cbData);
            while (!buf.empty() && buf.back() == L'\0') buf.pop_back();
        }
        RegCloseKey(hKey);
    }

    std::lock_guard<std::recursive_mutex> lk(g_mu);
    std::string active;
    for (auto& line : SplitChar(buf, L'\n')) {
        auto f = SplitChar(line, L'\t');
        if (f.size() >= 2 && f[0] == L"A") active = W2U(f[1]);
        else if (f.size() >= 2 && f[0] == L"F" && !f[1].empty()) EnsureFolder(W2U(f[1]));
        else if (f.size() >= 3 && f[0] == L"P" && !f[1].empty() && !f[2].empty()) {
            std::string n = W2U(f[1]);
            EnsureFolder(n);
            g_pins[n].insert(f[2]);
        }
    }
    g_active = g_folders.count(active) ? active : std::string();
}

void LoadSettings() {
    std::lock_guard<std::recursive_mutex> lk(g_mu);

    PCWSTR m = Wh_GetStringSetting(L"modifiers");
    std::wstring ms = m ? m : L"ctrlalt";
    Wh_FreeStringSetting(m);
    g_cfg.mods = ms == L"ctrlshift" ? Mods::CtrlShift : ms == L"winalt" ? Mods::WinAlt : Mods::CtrlAlt;
    g_cfg.overlayMs = std::max(200, Wh_GetIntSetting(L"overlayMs"));
    g_cfg.followFocus = Wh_GetIntSetting(L"followFocus") != 0;

    g_rules.clear();
    for (int i = 0;; i++) {
        PCWSTR n = Wh_GetStringSetting(L"folders[%d].name", i);
        bool end = !n || !*n;
        std::wstring name = n ? n : L"";
        Wh_FreeStringSetting(n);
        if (end) break;
        PCWSTR p = Wh_GetStringSetting(L"folders[%d].processes", i);
        std::wstring procs = p ? p : L"";
        Wh_FreeStringSetting(p);
        std::string key = W2U(name);
        EnsureFolder(key);
        g_rules[key] = SplitProcesses(procs);
    }
    if (!g_active.empty() && !g_folders.count(g_active)) g_active.clear();
}

void PlaceOverlay(int w, int h) {
    POINT pt;
    GetCursorPos(&pt);
    MONITORINFO mi = {sizeof(mi)};
    GetMonitorInfoW(MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST), &mi);
    int x = mi.rcMonitor.left + (mi.rcMonitor.right - mi.rcMonitor.left - w) / 2;
    int y = mi.rcMonitor.top + (mi.rcMonitor.bottom - mi.rcMonitor.top - h) / 2;
    SetWindowPos(g_overlay, HWND_TOPMOST, x, y, w, h, SWP_NOACTIVATE | SWP_SHOWWINDOW);
    SetWindowRgn(g_overlay, CreateRoundRectRgn(0, 0, w + 1, h + 1, S(16), S(16)), TRUE);
    InvalidateRect(g_overlay, nullptr, FALSE);
}

constexpr int kMaxRows = 12;

int RowsVisible() { return std::min<int>((int)g_list.size(), kMaxRows); }

void ShowSwitcher() {
    int w = S(560);
    int h = S(52) + RowsVisible() * S(38) + S(12);
    PlaceOverlay(w, h);
}

void ShowToastOnUi(const std::wstring& text) {
    if (g_session) return;
    {
        std::lock_guard<std::recursive_mutex> lk(g_mu);
        g_toast = text;
    }
    PlaceOverlay(S(460), S(56));
    SetTimer(g_overlay, TIMER_HIDE, g_cfg.overlayMs, nullptr);
}

void ShowToast(const std::wstring& text) {
    if (!g_overlay) return;
    {
        std::lock_guard<std::recursive_mutex> lk(g_mu);
        g_toast = text;
    }
    PostMessageW(g_overlay, WM_TOAST, 0, 0);
}

HICON IconOf(HWND h) {
    DWORD_PTR r = 0;
    if (SendMessageTimeoutW(h, WM_GETICON, ICON_SMALL2, 0, SMTO_ABORTIFHUNG | SMTO_BLOCK, 40, &r) && r)
        return (HICON)r;
    if (SendMessageTimeoutW(h, WM_GETICON, ICON_BIG, 0, SMTO_ABORTIFHUNG | SMTO_BLOCK, 40, &r) && r)
        return (HICON)r;
    HICON ic = (HICON)GetClassLongPtrW(h, GCLP_HICONSM);
    if (!ic) ic = (HICON)GetClassLongPtrW(h, GCLP_HICON);
    return ic;
}

void Paint(HWND hw) {
    PAINTSTRUCT ps;
    HDC dc = BeginPaint(hw, &ps);
    RECT rc;
    GetClientRect(hw, &rc);
    int W = rc.right, H = rc.bottom;

    HDC dcm = CreateCompatibleDC(dc);
    HBITMAP bmp = CreateCompatibleBitmap(dc, W, H);
    HGDIOBJ oldBmp = SelectObject(dcm, bmp);

    HBRUSH bg = CreateSolidBrush(RGB(32, 32, 38));
    FillRect(dcm, &rc, bg);
    DeleteObject(bg);
    SetBkMode(dcm, TRANSPARENT);
    HGDIOBJ oldFont = SelectObject(dcm, g_font);

    std::lock_guard<std::recursive_mutex> lk(g_mu);

    if (!g_session) {
        SetTextColor(dcm, RGB(240, 240, 240));
        SelectObject(dcm, g_fontBold);
        RECT tr = rc;
        DrawTextW(dcm, g_toast.c_str(), -1, &tr, DT_SINGLELINE | DT_VCENTER | DT_CENTER | DT_END_ELLIPSIS | DT_NOPREFIX);
    } else {
        std::wstring title = L"Folder: " + (g_active.empty() ? std::wstring(L"All") : U2W(g_active)) +
                             L"   (" + std::to_wstring(g_list.size()) + L")";
        SetTextColor(dcm, RGB(120, 190, 255));
        SelectObject(dcm, g_fontBold);
        RECT hr = {S(18), 0, W - S(18), S(48)};
        DrawTextW(dcm, title.c_str(), -1, &hr, DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS | DT_NOPREFIX);
        SelectObject(dcm, g_font);

        int n = (int)g_list.size();
        int start = std::max(0, std::min(g_sel - kMaxRows / 2, n - kMaxRows));
        int y = S(52);
        for (int i = start; i < n && i < start + kMaxRows; i++, y += S(38)) {
            RECT row = {S(8), y, W - S(8), y + S(36)};
            if (i == g_sel) {
                HBRUSH sel = CreateSolidBrush(RGB(0, 95, 184));
                HGDIOBJ ob = SelectObject(dcm, sel);
                HPEN np = (HPEN)SelectObject(dcm, GetStockObject(NULL_PEN));
                RoundRect(dcm, row.left, row.top, row.right, row.bottom, S(10), S(10));
                SelectObject(dcm, np);
                SelectObject(dcm, ob);
                DeleteObject(sel);
            }
            HWND t = g_list[i];
            if (HICON ic = IconOf(t))
                DrawIconEx(dcm, row.left + S(10), row.top + S(8), ic, S(20), S(20), 0, nullptr, DI_NORMAL);
            wchar_t txt[256] = L"";
            GetWindowTextW(t, txt, ARRAYSIZE(txt));
            SetTextColor(dcm, RGB(245, 245, 245));
            RECT tr = {row.left + S(42), row.top, row.right - S(10), row.bottom};
            DrawTextW(dcm, txt, -1, &tr, DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS | DT_NOPREFIX);
        }
    }

    BitBlt(dc, 0, 0, W, H, dcm, 0, 0, SRCCOPY);
    SelectObject(dcm, oldFont);
    SelectObject(dcm, oldBmp);
    DeleteObject(bmp);
    DeleteDC(dcm);
    EndPaint(hw, &ps);
}

LRESULT CALLBACK OverlayProc(HWND h, UINT m, WPARAM w, LPARAM l) {
    switch (m) {
        case WM_PAINT: Paint(h); return 0;
        case WM_ERASEBKGND: return 1;
        case WM_MOUSEACTIVATE: return MA_NOACTIVATE;
        case WM_NCHITTEST: return HTTRANSPARENT;
        case WM_TOAST: {
            std::wstring t;
            {
                std::lock_guard<std::recursive_mutex> lk(g_mu);
                t = g_toast;
            }
            ShowToastOnUi(t);
            return 0;
        }
        case WM_TIMER:
            if (w == TIMER_HIDE) {
                KillTimer(h, TIMER_HIDE);
                if (!g_session) ShowWindow(h, SW_HIDE);
            }
            return 0;
    }
    return DefWindowProcW(h, m, w, l);
}

void FocusWindow(HWND h) {
    if (!IsWindow(h)) return;
    if (IsIconic(h)) ShowWindow(h, SW_RESTORE);
    BOOL ok = SetForegroundWindow_Original ? SetForegroundWindow_Original(h) : SetForegroundWindow(h);
    if (!ok || RootOf(GetForegroundWindow()) != h) {
        if (SwitchToThisWindow_Original) SwitchToThisWindow_Original(h, TRUE);
    }
}

void SendDummyKey() {
    INPUT in[2] = {};
    in[0].type = INPUT_KEYBOARD;
    in[0].ki.wVk = 0xE8;
    in[1] = in[0];
    in[1].ki.dwFlags = KEYEVENTF_KEYUP;
    SendInput(2, in, sizeof(INPUT));
}

std::wstring FolderLabel(const std::string& f) {
    if (f.empty()) return L"All windows";
    return L"Folder: " + U2W(f) + L"  (" + std::to_wstring(LiveMembers(f).size()) + L" windows)";
}

HWND TargetWindow() {
    HWND fg = RootOf(GetForegroundWindow());
    if (fg && fg != g_overlay) return fg;
    return nullptr;
}

void ActNewFolder() {
    std::lock_guard<std::recursive_mutex> lk(g_mu);
    int i = 1;
    std::string name;
    do { name = "Folder " + std::to_string(i++); } while (g_folders.count(name));
    EnsureFolder(name);
    g_active = name;
    if (HWND t = TargetWindow()) g_folders[name].push_back(t);
    SaveState();
    ShowToastOnUi(L"Created " + FolderLabel(name));
}

void ActAssign() {
    HWND t = TargetWindow();
    if (!t) return;
    {
        std::lock_guard<std::recursive_mutex> lk(g_mu);
        if (g_active.empty()) { ActNewFolder(); return; }
        for (auto& kv : g_folders) {
            auto& v = kv.second;
            v.erase(std::remove(v.begin(), v.end(), t), v.end());
        }
        g_folders[g_active].push_back(t);
        SaveState();
        ShowToastOnUi(L"Added to " + FolderLabel(g_active));
    }
}

void ActRemove() {
    HWND t = TargetWindow();
    std::lock_guard<std::recursive_mutex> lk(g_mu);
    if (!t || g_active.empty()) return;
    auto& v = g_folders[g_active];
    v.erase(std::remove(v.begin(), v.end(), t), v.end());
    if (IsMember(g_active, t))
        ShowToastOnUi(L"Matched by a process rule/pin (use P to unpin)");
    else
        ShowToastOnUi(L"Removed from " + FolderLabel(g_active));
}

void ActPin() {
    HWND t = TargetWindow();
    std::lock_guard<std::recursive_mutex> lk(g_mu);
    if (!t || g_active.empty()) return;
    std::wstring exe = ExeName(t);
    if (exe.empty()) return;
    auto& pins = g_pins[g_active];
    if (pins.count(exe)) {
        pins.erase(exe);
        ShowToastOnUi(L"Unpinned " + exe);
    } else {
        for (auto& kv : g_pins) kv.second.erase(exe);
        pins.insert(exe);
        ShowToastOnUi(L"Pinned " + exe + L" to " + U2W(g_active));
    }
    SaveState();
}

void ActDelete() {
    std::lock_guard<std::recursive_mutex> lk(g_mu);
    if (g_active.empty()) return;
    std::string n = g_active;
    g_folders.erase(n);
    g_pins.erase(n);
    g_order.erase(std::remove(g_order.begin(), g_order.end(), n), g_order.end());
    g_active.clear();
    SaveState();
    ShowToastOnUi(L"Deleted " + U2W(n) + L" - showing all windows");
}

void ActSetActive(const std::string& f) {
    std::lock_guard<std::recursive_mutex> lk(g_mu);
    g_active = f;
    SaveState();
    ShowToastOnUi(FolderLabel(g_active));
}

void ActCycle(int dir) {
    std::string next;
    {
        std::lock_guard<std::recursive_mutex> lk(g_mu);
        int n = (int)g_order.size() + 1;
        int cur = 0;
        for (size_t i = 0; i < g_order.size(); i++)
            if (g_order[i] == g_active) cur = (int)i + 1;
        int idx = ((cur + dir) % n + n) % n;
        next = idx == 0 ? std::string() : g_order[idx - 1];
    }
    ActSetActive(next);
}

bool StartSession() {
    std::lock_guard<std::recursive_mutex> lk(g_mu);
    if (g_active.empty() || !g_folders.count(g_active)) return false;
    std::vector<HWND> list = LiveMembers(g_active);
    if (list.empty()) return false;
    g_list = list;
    g_sel = (list.size() > 1 && RootOf(GetForegroundWindow()) == list[0]) ? 1 : 0;
    g_session = true;
    SendDummyKey();
    ShowSwitcher();
    return true;
}

void StepSession(int dir) {
    std::lock_guard<std::recursive_mutex> lk(g_mu);
    int n = (int)g_list.size();
    if (!n) return;
    g_sel = ((g_sel + dir) % n + n) % n;
    InvalidateRect(g_overlay, nullptr, FALSE);
}

void EndSession(bool commit) {
    HWND target = nullptr;
    {
        std::lock_guard<std::recursive_mutex> lk(g_mu);
        if (!g_session) return;
        if (commit && g_sel >= 0 && g_sel < (int)g_list.size()) target = g_list[g_sel];
        g_session = false;
        g_list.clear();
    }
    ShowWindow(g_overlay, SW_HIDE);
    if (target) FocusWindow(target);
}

bool ModsHeld() {
    bool c = KeyDown(VK_CONTROL), a = KeyDown(VK_MENU), w = KeyDown(VK_LWIN) || KeyDown(VK_RWIN);
    switch (g_cfg.mods) {
        case Mods::CtrlShift: return c && KeyDown(VK_SHIFT) && !a && !w;
        case Mods::WinAlt: return w && a && !c;
        default: return c && a && !w;
    }
}

bool IsAltKey(DWORD vk) { return vk == VK_MENU || vk == VK_LMENU || vk == VK_RMENU; }

LRESULT CALLBACK KeyboardProc(int code, WPARAM wp, LPARAM lp) {
    if (code != HC_ACTION || !g_ready) return CallNextHookEx(nullptr, code, wp, lp);
    auto* k = (KBDLLHOOKSTRUCT*)lp;
    if (k->flags & LLKHF_INJECTED) return CallNextHookEx(nullptr, code, wp, lp);

    bool down = wp == WM_KEYDOWN || wp == WM_SYSKEYDOWN;
    bool up = wp == WM_KEYUP || wp == WM_SYSKEYUP;
    DWORD vk = k->vkCode;

    if (g_session) {
        if (down && vk == VK_ESCAPE) { EndSession(false); return 1; }
        if (up && IsAltKey(vk)) { EndSession(true); return CallNextHookEx(nullptr, code, wp, lp); }
        if (down && vk == VK_TAB) { StepSession(KeyDown(VK_SHIFT) ? -1 : 1); return 1; }
        if (up && vk == VK_TAB) return 1;
        return CallNextHookEx(nullptr, code, wp, lp);
    }

    if (down && vk == VK_TAB && (k->flags & LLKHF_ALTDOWN) && !KeyDown(VK_CONTROL) &&
        !KeyDown(VK_LWIN) && !KeyDown(VK_RWIN)) {
        bool hasFolder;
        {
            std::lock_guard<std::recursive_mutex> lk(g_mu);
            hasFolder = !g_active.empty();
        }
        if (hasFolder && StartSession()) {
            if (KeyDown(VK_SHIFT)) StepSession(-1);
            return 1;
        }
        return CallNextHookEx(nullptr, code, wp, lp);
    }

    if (down && ModsHeld()) {
        bool handled = true;
        switch (vk) {
            case 'N': ActNewFolder(); break;
            case 'A': ActAssign(); break;
            case 'R': ActRemove(); break;
            case 'P': ActPin(); break;
            case 'D': ActDelete(); break;
            case 'F': ActCycle(+1); break;
            case 'G': ActCycle(-1); break;
            case '0': ActSetActive(std::string()); break;
            default: handled = false;
        }
        if (handled) {
            SendDummyKey();
            return 1;
        }
    }
    return CallNextHookEx(nullptr, code, wp, lp);
}

void OnFocusRequest(HWND h) {
    if (!g_ready || !g_cfg.followFocus || !h) return;
    HWND root = RootOf(h);
    std::string follow;
    {
        std::lock_guard<std::recursive_mutex> lk(g_mu);
        if (g_active.empty() || IsMember(g_active, root)) return;
        follow = FolderOf(root);
        if (follow.empty()) return;
        g_active = follow;
        SaveState();
    }
    ShowToast(L"Switched to " + FolderLabel(follow));
}

BOOL WINAPI SetForegroundWindow_Hook(HWND h) {
    OnFocusRequest(h);
    return SetForegroundWindow_Original(h);
}

void WINAPI SwitchToThisWindow_Hook(HWND h, BOOL altTab) {
    OnFocusRequest(h);
    SwitchToThisWindow_Original(h, altTab);
}

void CALLBACK WinEventProc(HWINEVENTHOOK, DWORD event, HWND hwnd, LONG idObject, LONG, DWORD, DWORD) {
    if (event != EVENT_SYSTEM_FOREGROUND || idObject != OBJID_WINDOW || !hwnd) return;
    HWND root = RootOf(hwnd);
    if (root == g_overlay) return;
    std::lock_guard<std::recursive_mutex> lk(g_mu);
    g_mru.erase(std::remove(g_mru.begin(), g_mru.end(), root), g_mru.end());
    g_mru.insert(g_mru.begin(), root);
    if (g_mru.size() > 256) g_mru.resize(256);
}

DWORD WINAPI UiThread(LPVOID) {
    g_uiThreadId = GetCurrentThreadId();
    
    HMODULE hUser32 = GetModuleHandleW(L"user32.dll");
    if (hUser32) {
        typedef UINT(WINAPI* GetDpiForSystem_t)();
        auto pGetDpiForSystem = (GetDpiForSystem_t)GetProcAddress(hUser32, "GetDpiForSystem");
        if (pGetDpiForSystem) g_dpi = (int)pGetDpiForSystem();
        else g_dpi = 96;
    } else {
        g_dpi = 96;
    }

    WNDCLASSW wc = {};
    wc.lpfnWndProc = OverlayProc;
    wc.hInstance = GetModuleHandleW(nullptr);
    wc.lpszClassName = kOverlayClass;
    wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    RegisterClassW(&wc);

    g_font = CreateFontW(-S(15), 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET, 0, 0, CLEARTYPE_QUALITY, 0, L"Segoe UI");
    g_fontBold = CreateFontW(-S(17), 0, 0, 0, FW_SEMIBOLD, 0, 0, 0, DEFAULT_CHARSET, 0, 0, CLEARTYPE_QUALITY, 0, L"Segoe UI");

    g_overlay = CreateWindowExW(WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE | WS_EX_LAYERED | WS_EX_TRANSPARENT,
                                kOverlayClass, L"AltTabFolders", WS_POPUP, 0, 0, 10, 10, nullptr, nullptr,
                                wc.hInstance, nullptr);
    if (g_overlay) SetLayeredWindowAttributes(g_overlay, 0, 238, LWA_ALPHA);

    g_kbHook = SetWindowsHookExW(WH_KEYBOARD_LL, KeyboardProc, GetModuleHandleW(nullptr), 0);
    g_winEvt = SetWinEventHook(EVENT_SYSTEM_FOREGROUND, EVENT_SYSTEM_FOREGROUND, nullptr, WinEventProc, 0, 0,
                               WINEVENT_OUTOFCONTEXT);
    HWND fg = RootOf(GetForegroundWindow());
    if (fg) g_mru.push_back(fg);

    if (!g_kbHook) Wh_Log(L"SetWindowsHookEx(WH_KEYBOARD_LL) failed: %u", GetLastError());
    SetEvent(g_startEvt);

    MSG msg;
    while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    if (g_kbHook) UnhookWindowsHookEx(g_kbHook);
    if (g_winEvt) UnhookWinEvent(g_winEvt);
    g_kbHook = nullptr;
    g_winEvt = nullptr;
    if (g_overlay) DestroyWindow(g_overlay);
    g_overlay = nullptr;
    UnregisterClassW(kOverlayClass, wc.hInstance);
    if (g_font) DeleteObject(g_font);
    if (g_fontBold) DeleteObject(g_fontBold);
    return 0;
}

}

BOOL Wh_ModInit() {
    Wh_Log(L"Alt+Tab Folders init");
    LoadSettings();
    LoadState();

    HMODULE user32 = GetModuleHandleW(L"user32.dll");
    if (user32) {
        void* setFg = (void*)GetProcAddress(user32, "SetForegroundWindow");
        void* switchTo = (void*)GetProcAddress(user32, "SwitchToThisWindow");
        if (setFg)
            Wh_SetFunctionHook(setFg, (void*)SetForegroundWindow_Hook, (void**)&SetForegroundWindow_Original);
        if (switchTo)
            Wh_SetFunctionHook(switchTo, (void*)SwitchToThisWindow_Hook, (void**)&SwitchToThisWindow_Original);
    }

    g_startEvt = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_thread = CreateThread(nullptr, 0, UiThread, nullptr, 0, nullptr);
    if (!g_thread) return FALSE;
    WaitForSingleObject(g_startEvt, 5000);
    CloseHandle(g_startEvt);
    g_startEvt = nullptr;

    g_ready = true;
    return TRUE;
}

void Wh_ModSettingsChanged() {
    LoadSettings();
}

void Wh_ModUninit() {
    g_ready = false;
    if (g_thread) {
        PostThreadMessageW(g_uiThreadId, WM_QUIT, 0, 0);
        WaitForSingleObject(g_thread, 3000);
        CloseHandle(g_thread);
        g_thread = nullptr;
    }
    SaveState();
}
