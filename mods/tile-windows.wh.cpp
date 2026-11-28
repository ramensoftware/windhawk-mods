// ==WindhawkMod==
// @id              tile-windows
// @name            Tile Windows
// @description     Arrange all open windows across all monitors with one hotkey, sized by how much you use each app
// @version         1.0
// @author          laurensanp
// @github          https://github.com/laurensanp
// @homepage        https://github.com/laurensanp/tile-windows
// @include         windhawk.exe
// @compilerOptions -ldwmapi -lshell32
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Tile Windows

Arranges all your open app windows across all monitors with one hotkey, sized
by how much you use each app. Press **Ctrl+Alt+W** to arrange, **Ctrl+Alt+R**
to put every window back where it was. Nothing happens until you press a
hotkey.

![Five apps arranged by Tile Windows on one
monitor](https://raw.githubusercontent.com/laurensanp/tile-windows/95335922cbbccc911ff82c0f7f17bb88e7aae497/windhawk/screenshot.png)

* **Main monitor** gets only the apps you work in directly (weight 3 and up:
  editor, browser...). Chat, music and tools go to the other monitors.
* **Best fit, not a fixed grid:** each monitor gets the mix of side-by-side
  columns and stacked rows that gives important apps the most room while every
  window stays readable. Each press picks another good layout (columns or
  rows, mirrored or not), never the same one twice in a row.
* **Never unreadable:** windows don't get smaller than 500x300 (scaled with
  your display scaling). If too many apps are open, the least important ones
  are minimized.
* **Respects apps' own minimum sizes** (Discord, Steam, Spotify can't get very
  small) and works them into the layout.
* **Real undo:** Ctrl+Alt+R puts each window back exactly as it was before the
  first Ctrl+Alt+W: size, position, maximized or minimized. Trying several
  layouts in a row doesn't lose that.
* **Games are left alone** and **keep focus**: pressing either hotkey while in
  a game doesn't pull you out of it.
* Works with any number of monitors, resolutions and mixed display scaling.

## How it differs from other tiling mods

[Tiling Helper](https://windhawk.net/mods/tiling-helper),
[MultiWM](https://windhawk.net/mods/multiwm) and
[Adaptive Windows](https://windhawk.net/mods/adaptive-windows) arrange the
windows by a layout scheme (master and stack, grids, per-desktop layouts).
Tile Windows instead arranges all monitors at once by **how much you use each
app**:
the weight of an app decides how big its window gets, which monitor it goes
to, and which apps get minimized when not everything fits. It only acts when
you press the hotkey, so windows stay free to move in between.

## Settings

* **Weights:** process names (Task Manager > Details, without `.exe`) grouped
  by how much you use them. Higher = more space, and minimized later. Any
  number works, e.g. `4.5`. Apps not listed count as 1.
  * 3 and up: you work in it directly, main monitor.
  * below 3: other monitors (or the main monitor if you only have one).
  * 0: never touched (games, overlays).
* **Wide apps:** apps that need width (videos, websites) get wide slots instead
  of narrow columns when possible.
* **Main screen apps:** apps that get the whole main monitor to themselves when
  open (still tiled, not maximized); everything else moves to the other
  monitors.
* **Game folders:** anything started from these folders counts as a game and
  is never touched.
* **Hotkeys:** Ctrl, Alt, Shift and Win with a letter, a digit or F1-F24. A
  letter or digit needs at least one of Ctrl, Alt, Shift or Win.

Windows of apps running as administrator (e.g. Task Manager) can't be moved by
a normal app, so they are left out and the other windows are arranged without
them. Windows of apps that don't respond are left out too.

Also available as a standalone PowerShell version:
[laurensanp/tile-windows](https://github.com/laurensanp/tile-windows).
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- tileHotkey: Ctrl+Alt+W
  $name: Tile hotkey
  $description: >-
    Arranges all windows. Ctrl, Alt, Shift and Win with a letter, a digit or
    F1-F24, e.g. Ctrl+Alt+W. A letter or digit needs at least one of Ctrl,
    Alt, Shift or Win.
- restoreHotkey: Ctrl+Alt+R
  $name: Restore hotkey
  $description: >-
    Puts every window back as it was before tiling (size, position, maximized
    or minimized). Leave empty to turn it off.
- gap: 12
  $name: Gap
  $description: >-
    Space between windows and to the screen edge, in pixels at 100% display
    scaling.
- minWidth: 500
  $name: Minimum width
  $description: >-
    Narrowest allowed window, in pixels at 100% display scaling. If apps don't
    fit, the least important ones are minimized.
- minHeight: 300
  $name: Minimum height
  $description: Lowest allowed window, in pixels at 100% display scaling.
- weights:
  - - weight: "5"
      $name: Weight
      $description: >-
        Any number, e.g. 4.5. Higher = more space. 3 and up: main monitor. 0:
        never touched.
    - apps: [Code, Cursor, devenv, idea64, pycharm64, sublime_text, notepad++,
        brave, chrome, msedge, firefox, opera, vivaldi]
      $name: Apps
      $description: Process names as in Task Manager > Details, without .exe.
  - - weight: "4"
    - apps: [claude, ChatGPT, Obsidian, Notion, Figma, WINWORD, EXCEL,
        POWERPNT, ONENOTE]
  - - weight: "3"
    - apps: [MSACCESS, Acrobat, AcroRd32]
  - - weight: "2.5"
    - apps: [Discord, slack, ms-teams, Teams, Zoom]
  - - weight: "2"
    - apps: [Spotify, WhatsApp, WhatsApp.Root, Telegram, Signal, OUTLOOK, olk,
        thunderbird, WindowsTerminal, powershell, pwsh, cmd, Notepad, obs64,
        vlc]
  - - weight: "1.5"
    - apps: [explorer, ApplicationFrameHost, SystemSettings, Taskmgr]
  - - weight: "1"
    - apps: [steamwebhelper, Steam, EpicGamesLauncher, RiotClientServices,
        Battle.net, EADesktop, upc, GalaxyClient, NVIDIA App, RadeonSoftware]
  - - weight: "0"
    - apps: [javaw, RobloxPlayerBeta, Minecraft.Windows, NVIDIA Overlay,
        GameBar, LiveCaptions, osk, magnify]
  $name: App weights
  $description: >-
    How much you use each app. Apps not listed count as 1. An app listed twice
    keeps its first weight.
- wideApps: [brave, chrome, msedge, firefox, opera, vivaldi, vlc]
  $name: Wide apps
  $description: >-
    Apps that need width (videos, websites): they get wide slots (about 1280
    px) instead of narrow columns when possible.
- mainScreenApps: [""]
  $name: Main screen apps
  $description: >-
    Apps that get the whole main monitor to themselves when open (still tiled,
    not maximized); everything else moves to the other monitors.
- gameFolders: ['\steamapps\', '\Epic Games\', '\Riot Games\', '\XboxGames\',
    '\WindowsApps\Microsoft.MinecraftUWP', '\Rockstar Games\', '\EA Games\',
    '\Ubisoft Game Launcher\games\', '\GOG Galaxy\Games\', '\FiveM']
  $name: Game folders
  $description: >-
    Anything started from a path containing one of these counts as a game and
    is never touched.
*/
// ==/WindhawkModSettings==

#include <windows.h>

#include <dwmapi.h>
#include <shellapi.h>
#include <windhawk_utils.h>

#include <algorithm>
#include <array>
#include <bit>
#include <climits>
#include <cmath>
#include <cwctype>
#include <limits>
#include <map>
#include <memory>
#include <mutex>
#include <random>
#include <string>
#include <vector>

////////////////////////////////////////////////////////////////////////////////
// Settings

struct Settings {
    std::wstring tileHotkey;
    std::wstring restoreHotkey;
    int gap = 12;
    int minWidth = 500;
    int minHeight = 300;
    // Lowercase process name -> weight.
    std::map<std::wstring, double> weights;
    // All lowercase.
    std::vector<std::wstring> wideApps;
    std::vector<std::wstring> mainScreenApps;
    std::vector<std::wstring> gameFolders;
};

std::wstring ToLower(std::wstring s) {
    for (auto& c : s) {
        c = (wchar_t)towlower(c);
    }
    return s;
}

std::wstring Trim(const std::wstring& s) {
    size_t start = s.find_first_not_of(L" \t");
    if (start == std::wstring::npos) {
        return L"";
    }
    size_t end = s.find_last_not_of(L" \t");
    return s.substr(start, end - start + 1);
}

// `name` is the full setting name, e.g. weights[2].apps[0] (it has no %, so
// it's safe as the format string).
std::wstring ReadStringSetting(const std::wstring& name) {
    return Trim(WindhawkUtils::StringSetting::make(name.c_str()).get());
}

// Reads an array of strings until the first empty entry, lowercased.
std::vector<std::wstring> ReadListSetting(const std::wstring& name) {
    std::vector<std::wstring> list;
    for (int i = 0;; i++) {
        std::wstring value =
            ReadStringSetting(name + L"[" + std::to_wstring(i) + L"]");
        if (value.empty()) {
            break;
        }
        list.push_back(ToLower(value));
    }
    return list;
}

std::shared_ptr<Settings> LoadSettings() {
    auto s = std::make_shared<Settings>();
    s->tileHotkey = ReadStringSetting(L"tileHotkey");
    s->restoreHotkey = ReadStringSetting(L"restoreHotkey");
    s->gap = std::clamp(Wh_GetIntSetting(L"gap"), 0, 200);
    s->minWidth = std::clamp(Wh_GetIntSetting(L"minWidth"), 100, 4000);
    s->minHeight = std::clamp(Wh_GetIntSetting(L"minHeight"), 100, 4000);

    for (int g = 0;; g++) {
        std::wstring prefix = L"weights[" + std::to_wstring(g) + L"]";
        std::wstring weightText = ReadStringSetting(prefix + L".weight");
        if (weightText.empty()) {
            break;
        }
        std::replace(weightText.begin(), weightText.end(), L',', L'.');
        wchar_t* end = nullptr;
        double weight = wcstod(weightText.c_str(), &end);
        if (end == weightText.c_str() || !std::isfinite(weight) || weight < 0) {
            Wh_Log(L"Skipping weight group with weight \"%s\"",
                   weightText.c_str());
            continue;
        }
        for (const auto& app : ReadListSetting(prefix + L".apps")) {
            if (!s->weights.emplace(app, weight).second) {
                Wh_Log(L"%s is in two weight groups, keeping the first",
                       app.c_str());
            }
        }
    }

    s->wideApps = ReadListSetting(L"wideApps");
    s->mainScreenApps = ReadListSetting(L"mainScreenApps");
    s->gameFolders = ReadListSetting(L"gameFolders");
    return s;
}

bool Contains(const std::vector<std::wstring>& list,
              const std::wstring& value) {
    return std::find(list.begin(), list.end(), value) != list.end();
}

// "Ctrl+Alt+W" -> modifiers and virtual key. Supports Ctrl, Alt, Shift, Win
// with a letter, digit or F1-F24. A letter or digit needs at least one of Ctrl,
// Alt, Shift or Win: alone it would take that key away from every app.
bool ParseHotkey(const std::wstring& text, UINT* mods, UINT* vk) {
    *mods = MOD_NOREPEAT;
    *vk = 0;
    std::wstring upper = text;
    for (auto& c : upper) {
        c = (wchar_t)towupper(c);
    }
    size_t start = 0;
    while (start <= upper.size()) {
        size_t plus = upper.find(L'+', start);
        if (plus == std::wstring::npos) {
            plus = upper.size();
        }
        std::wstring part = Trim(upper.substr(start, plus - start));
        start = plus + 1;
        if (part == L"CTRL" || part == L"CONTROL") {
            *mods |= MOD_CONTROL;
        } else if (part == L"ALT") {
            *mods |= MOD_ALT;
        } else if (part == L"SHIFT") {
            *mods |= MOD_SHIFT;
        } else if (part == L"WIN") {
            *mods |= MOD_WIN;
        } else if (part.size() == 1 && iswalnum(part[0]) && part[0] < 128) {
            *vk = part[0];
        } else if (part.size() > 1 && part[0] == L'F') {
            int n = _wtoi(part.c_str() + 1);
            if (n < 1 || n > 24 || part != L"F" + std::to_wstring(n)) {
                return false;
            }
            *vk = VK_F1 + n - 1;
        } else {
            return false;
        }
    }
    bool fKey = *vk >= VK_F1 && *vk <= VK_F24;
    bool modifier =
        (*mods & (MOD_CONTROL | MOD_ALT | MOD_SHIFT | MOD_WIN)) != 0;
    return *vk != 0 && (fKey || modifier);
}

////////////////////////////////////////////////////////////////////////////////
// Layout math (no Windows calls)

// x, y, width, height.
using Slot = std::array<int, 4>;

struct Layout {
    std::vector<Slot> slots;
    // How many of the first apps are shown.
    int count = 0;
    // The layout as text, to avoid showing the same one twice in a row.
    std::wstring sig;
};

// Splits `length` into parts by weight; a part whose app needs more (`need`)
// keeps that minimum and the others share what's left (if even that can't
// work, plain weights). Returns the cut positions 0..length (count+1 values).
std::vector<int> SplitLength(int length,
                             const std::vector<double>& weight,
                             const std::vector<int>& need) {
    size_t n = weight.size();
    double total = 0;
    for (double v : weight) {
        total += v;
    }
    int fixedLength = 0;
    double freeTotal = 0;
    std::vector<bool> isFixed(n);
    for (size_t i = 0; i < n; i++) {
        if (need[i] > length * weight[i] / total) {
            isFixed[i] = true;
            fixedLength += need[i];
        } else {
            freeTotal += weight[i];
        }
    }
    if (fixedLength >= length || freeTotal <= 0) {
        isFixed.assign(n, false);
        fixedLength = 0;
        freeTotal = total;
    }
    std::vector<int> cuts(n + 1);
    double acc = 0;
    for (size_t i = 0; i < n; i++) {
        acc += isFixed[i] ? need[i]
                          : (length - fixedLength) * weight[i] / freeTotal;
        cuts[i + 1] = i == n - 1 ? length : (int)std::floor(acc + 0.0001);
    }
    return cuts;
}

// Slots for given columns. `cols`: index lists in screen order (left to
// right), each one column stacked top to bottom. Column width by weight (its
// biggest app + a quarter of the rest), row height by weight, both respecting
// each app's own minimum size (needW/needH).
std::vector<Slot> GetColumnSlots(const std::vector<std::vector<int>>& cols,
                                 const std::vector<double>& w,
                                 const std::vector<int>& needW,
                                 const std::vector<int>& needH,
                                 int x,
                                 int y,
                                 int width,
                                 int height) {
    size_t nc = cols.size();
    std::vector<double> colWeight(nc);
    std::vector<int> colNeed(nc);
    for (size_t c = 0; c < nc; c++) {
        double max = 0;
        double sum = 0;
        for (int i : cols[c]) {
            sum += w[i];
            max = std::max(max, w[i]);
            colNeed[c] = std::max(colNeed[c], needW[i]);
        }
        colWeight[c] = max + 0.25 * (sum - max);
    }
    std::vector<int> xc = SplitLength(width, colWeight, colNeed);
    std::vector<Slot> slots(w.size());
    for (size_t c = 0; c < nc; c++) {
        const auto& rows = cols[c];
        std::vector<double> rowWeight;
        std::vector<int> rowNeed;
        for (int i : rows) {
            rowWeight.push_back(w[i]);
            rowNeed.push_back(needH[i]);
        }
        std::vector<int> yc = SplitLength(height, rowWeight, rowNeed);
        for (size_t r = 0; r < rows.size(); r++) {
            slots[rows[r]] = {x + xc[c], y + yc[r], xc[c + 1] - xc[c],
                              yc[r + 1] - yc[r]};
        }
    }
    return slots;
}

std::wstring LayoutSig(const std::vector<Slot>& slots, int k) {
    std::wstring sig;
    for (int i = 0; i < k; i++) {
        if (i) {
            sig += L";";
        }
        sig += std::to_wstring(slots[i][0]) + L"," +
               std::to_wstring(slots[i][1]) + L"," +
               std::to_wstring(slots[i][2]) + L"," +
               std::to_wstring(slots[i][3]);
    }
    return sig;
}

struct LayoutRequest {
    // Weights sorted most important first.
    std::vector<double> w;
    // Each app's own minimum size (+gap, 0 = unknown).
    std::vector<int> needW;
    std::vector<int> needH;
    int x = 0;
    int y = 0;
    int width = 0;
    int height = 0;
    int gap = 0;
    int minW = 0;
    int minH = 0;
    // Comfortable width per app (~800, or 1280 for wide apps), 0 = 800.
    std::vector<int> comfortW;
    int comfortH = 600;
    // > 0: a random pick (same seed = same pick) among the layouts nearly as
    // good as the best, also rows instead of columns and mirrored ones, but
    // not `avoid` (the sig of the layout shown last). 0 = always the best
    // one, in columns, not mirrored: what the layout tests check against.
    unsigned seed = 0;
    std::wstring avoid;
};

// Best arrangement for one monitor: columns side by side, apps stacked inside
// them, whatever fits best. Tries every split of the first k apps into columns
// (most important in the first column) and keeps as many apps as can be shown
// readably (slot >= minW x minH). Among those it picks the highest score:
// important apps get more area, and slots below a comfortable size count less.
Layout GetBestLayout(LayoutRequest req) {
    int n = (int)req.w.size();
    req.needW.resize(n);
    req.needH.resize(n);
    req.comfortW.resize(n);
    int maxCols = std::max(1, req.width / (req.minW + req.gap));
    int maxRows = std::max(1, req.height / (req.minH + req.gap));

    // (rows instead of columns, mirrored)
    std::vector<std::pair<bool, bool>> styles = {{false, false}};
    if (req.seed) {
        styles = {{false, false}, {false, true}, {true, false}, {true, true}};
    }

    struct Candidate {
        std::vector<Slot> slots;
        double score;
        std::wstring sig;
    };

    // More than 20 apps on one monitor can't be readable anyway, and the
    // number of splits doubles with each app.
    for (int k = std::min({n, maxCols * maxRows, 20}); k >= 1; k--) {
        double bestScore = -std::numeric_limits<double>::infinity();
        std::vector<Candidate> found;
        for (auto [rows, mirrored] : styles) {
            int nCols = rows ? maxRows : maxCols;
            int nRows = rows ? maxCols : maxRows;
            for (unsigned mask = 0; mask < (1u << (k - 1)); mask++) {
                if (std::popcount(mask) + 1 > nCols) {
                    continue;
                }
                std::vector<std::vector<int>> cols;
                std::vector<int> cur = {0};
                for (int i = 1; i < k; i++) {
                    if (mask & (1u << (i - 1))) {
                        cols.push_back(cur);
                        cur.clear();
                    }
                    cur.push_back(i);
                }
                cols.push_back(cur);
                bool tooTall = false;
                for (const auto& c : cols) {
                    if ((int)c.size() > nRows) {
                        tooTall = true;
                    }
                }
                if (tooTall) {
                    continue;
                }
                if (mirrored) {
                    std::reverse(cols.begin(), cols.end());
                }

                std::vector<Slot> slots;
                if (rows) {
                    // Same as columns with x and y swapped.
                    slots = GetColumnSlots(cols, req.w, req.needH, req.needW,
                                           req.y, req.x, req.height, req.width);
                    for (int i = 0; i < k; i++) {
                        Slot s = slots[i];
                        slots[i] = {s[1], s[0], s[3], s[2]};
                    }
                } else {
                    slots = GetColumnSlots(cols, req.w, req.needW, req.needH,
                                           req.x, req.y, req.width, req.height);
                }

                double score = 0;
                bool ok = true;
                for (int i = 0; i < k; i++) {
                    const Slot& s = slots[i];
                    int sw = s[2] - req.gap;
                    int sh = s[3] - req.gap;
                    // No flat strip across the whole screen.
                    if (s[2] >= req.width && s[3] * 2 < req.height) {
                        ok = false;
                        break;
                    }
                    if (sw < req.minW || sh < req.minH ||
                        req.needW[i] > s[2] + 1 || req.needH[i] > s[3] + 1) {
                        ok = false;
                        break;
                    }
                    // Below the comfortable size counts double, so very flat
                    // or very narrow windows lose against a balanced mix.
                    double comfortW = req.comfortW[i] ? req.comfortW[i] : 800;
                    score +=
                        req.w[i] * (std::log((double)sw * sh) +
                                    2 * std::log(std::min(1.0, sw / comfortW)) +
                                    2 * std::log(std::min(
                                            1.0, (double)sh / req.comfortH)));
                }
                if (!ok) {
                    continue;
                }
                std::wstring sig = LayoutSig(slots, k);
                found.push_back({std::move(slots), score, std::move(sig)});
                bestScore = std::max(bestScore, score);
            }
        }
        if (found.empty()) {
            continue;
        }
        if (!req.seed) {
            auto it = std::max_element(
                found.begin(), found.end(),
                [](const auto& a, const auto& b) { return a.score < b.score; });
            return {it->slots, k, L""};
        }
        if (k == 2) {
            // Two apps: only side by side (they just swap sides), unless that
            // doesn't fit.
            std::vector<Candidate> side;
            for (const auto& f : found) {
                if (f.slots[0][1] == f.slots[1][1]) {
                    side.push_back(f);
                }
            }
            if (!side.empty()) {
                found = std::move(side);
                bestScore = -std::numeric_limits<double>::infinity();
                for (const auto& f : found) {
                    bestScore = std::max(bestScore, f.score);
                }
            }
        }

        double total = 0;
        for (int i = 0; i < k; i++) {
            total += req.w[i];
        }
        // 0.5: about 40% less area or comfort at most.
        std::map<std::wstring, const Candidate*> good;
        for (const auto& f : found) {
            if (f.score >= bestScore - 0.5 * total && f.sig != req.avoid) {
                good[f.sig] = &f;
            }
        }
        if (good.empty()) {
            for (const auto& f : found) {
                good[f.sig] = &f;
            }
        }
        std::mt19937 rng(req.seed);
        std::uniform_int_distribution<size_t> pickDist(0, good.size() - 1);
        auto pick = std::next(good.begin(), pickDist(rng));
        return {pick->second->slots, k, pick->second->sig};
    }

    Layout one;
    one.slots.resize(std::max(n, 1));
    one.slots[0] = {req.x, req.y, req.width, req.height};
    one.count = 1;
    return one;
}

struct Item {
    HWND hwnd = nullptr;
    double w = 1;
    bool solo = false;
    int minW = 0;
    int minH = 0;
    int comfortW = 800;
};

// Which monitor each app goes to. `items`: sorted most-used first;
// `screenCount`: monitors (0 = main). The main monitor gets the main screen
// apps alone if one is open, else only weight 3+ apps (or the most-used one if
// none); the rest spread over the other monitors by weight.
std::vector<std::vector<Item>> GetMonitorGroups(const std::vector<Item>& items,
                                                int screenCount) {
    std::vector<std::vector<Item>> groups(screenCount);
    std::vector<bool> direct(items.size());
    bool any = false;
    for (size_t i = 0; i < items.size(); i++) {
        if (items[i].solo) {
            direct[i] = any = true;
        }
    }
    if (!any) {
        for (size_t i = 0; i < items.size(); i++) {
            if (items[i].w >= 3) {
                direct[i] = any = true;
            }
        }
    }
    if (!any && !items.empty()) {
        direct[0] = true;
    }
    if (screenCount == 1) {
        direct.assign(items.size(), true);
    }
    std::vector<double> load(screenCount);
    for (size_t i = 0; i < items.size(); i++) {
        if (direct[i]) {
            groups[0].push_back(items[i]);
            continue;
        }
        int m = 1;
        for (int j = 2; j < screenCount; j++) {
            if (load[j] < load[m]) {
                m = j;
            }
        }
        groups[m].push_back(items[i]);
        load[m] += items[i].w;
    }
    return groups;
}

////////////////////////////////////////////////////////////////////////////////
// Windows

// EnumWindows callback (CALLBACK: 32-bit Windows needs stdcall, which a
// lambda can't give).
BOOL CALLBACK CollectAppWindow(HWND hwnd, LPARAM param) {
    auto& list = *(std::vector<HWND>*)param;
    if (!IsWindowVisible(hwnd) || GetWindow(hwnd, GW_OWNER) ||
        GetWindowTextLength(hwnd) == 0) {
        return TRUE;
    }
    LONG style = GetWindowLong(hwnd, GWL_STYLE);
    LONG exStyle = GetWindowLong(hwnd, GWL_EXSTYLE);
    if (!(style & WS_THICKFRAME) || (exStyle & WS_EX_TOOLWINDOW)) {
        return TRUE;
    }
    int cloaked = 0;
    if (SUCCEEDED(DwmGetWindowAttribute(hwnd, DWMWA_CLOAKED, &cloaked,
                                        sizeof(cloaked))) &&
        cloaked) {
        return TRUE;
    }
    WCHAR className[256];
    if (GetClassName(hwnd, className, ARRAYSIZE(className)) &&
        (wcscmp(className, L"Progman") == 0 ||
         wcscmp(className, L"WorkerW") == 0)) {
        return TRUE;
    }
    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    if (pid == GetCurrentProcessId()) {
        return TRUE;
    }
    // Moving or resizing a hung window blocks until it responds.
    if (IsHungAppWindow(hwnd)) {
        return TRUE;
    }
    list.push_back(hwnd);
    return TRUE;
}

// Real app windows only: visible, unowned, titled, resizable, not tool
// windows, not cloaked (hidden UWP apps, other virtual desktops), not hung. In
// z-order.
std::vector<HWND> GetAppWindows() {
    std::vector<HWND> list;
    EnumWindows(CollectAppWindow, (LPARAM)&list);
    return list;
}

struct AppInfo {
    // Lowercase process name without .exe.
    std::wstring name;
    double w = 1;
};

// Process name and weight of a window. Weight 0 for anything from a game
// folder; apps not listed count as 1.
AppInfo GetAppInfo(HWND hwnd, const Settings& settings) {
    AppInfo info;
    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    std::wstring path;
    HANDLE process = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (process) {
        WCHAR buffer[MAX_PATH * 2];
        DWORD size = ARRAYSIZE(buffer);
        if (QueryFullProcessImageName(process, 0, buffer, &size)) {
            path = ToLower(std::wstring(buffer, size));
        }
        CloseHandle(process);
    }
    std::wstring file = path.substr(path.find_last_of(L'\\') + 1);
    if (file.size() > 4 && file.ends_with(L".exe")) {
        file.resize(file.size() - 4);
    }
    info.name = file;
    auto it = settings.weights.find(file);
    if (it != settings.weights.end()) {
        info.w = it->second;
    }
    if (!path.empty()) {
        for (const auto& folder : settings.gameFolders) {
            if (path.find(folder) != std::wstring::npos) {
                info.w = 0;
                break;
            }
        }
    }
    return info;
}

// The visible frame (Windows 10/11 add invisible resize borders around most
// windows).
RECT GetVisibleRect(HWND hwnd) {
    RECT rect{};
    if (FAILED(DwmGetWindowAttribute(hwnd, DWMWA_EXTENDED_FRAME_BOUNDS, &rect,
                                     sizeof(rect)))) {
        GetWindowRect(hwnd, &rect);
    }
    return rect;
}

enum class Placed {
    kOk,
    // The window refused to shrink to the slot (app minimum size, e.g. Steam).
    kTooBig,
    // Windows doesn't let us move it (an app running as administrator).
    kCannotMove,
};

// Places the window so its *visible* frame fills the slot. Never activates
// the window, so a game or app you are in keeps focus.
Placed PlaceWindow(HWND hwnd, int x, int y, int w, int h) {
    const UINT flags = SWP_NOZORDER | SWP_NOACTIVATE | SWP_SHOWWINDOW;
    if (IsIconic(hwnd) || IsZoomed(hwnd)) {
        // Back to normal size without focusing it.
        ShowWindow(hwnd, SW_SHOWNOACTIVATE);
    }
    if (!SetWindowPos(hwnd, nullptr, x, y, w, h, flags) &&
        GetLastError() == ERROR_ACCESS_DENIED) {
        return Placed::kCannotMove;
    }
    if (IsZoomed(hwnd)) {
        // Some apps stay flagged maximized, which breaks their title bar.
        ShowWindow(hwnd, SW_SHOWNOACTIVATE);
        SetWindowPos(hwnd, nullptr, x, y, w, h, flags);
    }
    RECT win;
    RECT vis;
    GetWindowRect(hwnd, &win);
    if (FAILED(DwmGetWindowAttribute(hwnd, DWMWA_EXTENDED_FRAME_BOUNDS, &vis,
                                     sizeof(vis)))) {
        return Placed::kOk;
    }
    int l = vis.left - win.left;
    int t = vis.top - win.top;
    int r = win.right - vis.right;
    int b = win.bottom - vis.bottom;
    if (l || t || r || b) {
        SetWindowPos(hwnd, nullptr, x - l, y - t, w + l + r, h + t + b, flags);
    }
    vis = GetVisibleRect(hwnd);
    return vis.right - vis.left <= w + 2 && vis.bottom - vis.top <= h + 2
               ? Placed::kOk
               : Placed::kTooBig;
}

// Gives focus back to `hwnd` if something else took it.
void Refocus(HWND hwnd) {
    if (!hwnd || GetForegroundWindow() == hwnd) {
        return;
    }
    SetForegroundWindow(hwnd);
}

// Display scaling of the monitor: 1.0 = 100%, 1.5 = 150%.
double GetMonitorScale(HMONITOR monitor) {
    using GetDpiForMonitor_t = HRESULT(WINAPI*)(HMONITOR, int, UINT*, UINT*);
    static auto pGetDpiForMonitor = []() -> GetDpiForMonitor_t {
        HMODULE shcore =
            LoadLibraryEx(L"shcore.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
        return shcore ? (GetDpiForMonitor_t)GetProcAddress(shcore,
                                                           "GetDpiForMonitor")
                      : nullptr;
    }();
    UINT dpiX = 0;
    UINT dpiY = 0;
    if (pGetDpiForMonitor &&
        SUCCEEDED(pGetDpiForMonitor(monitor, 0 /*MDT_EFFECTIVE_DPI*/, &dpiX,
                                    &dpiY)) &&
        dpiX) {
        return dpiX / 96.0;
    }
    return 1.0;
}

struct Screen {
    HMONITOR monitor;
    RECT bounds;
    RECT work;
    bool primary;
};

// EnumDisplayMonitors callback.
BOOL CALLBACK CollectScreen(HMONITOR monitor, HDC, LPRECT, LPARAM param) {
    MONITORINFO mi{sizeof(mi)};
    if (GetMonitorInfo(monitor, &mi)) {
        ((std::vector<Screen>*)param)
            ->push_back({monitor, mi.rcMonitor, mi.rcWork,
                         (mi.dwFlags & MONITORINFOF_PRIMARY) != 0});
    }
    return TRUE;
}

// Main monitor first, then left to right.
std::vector<Screen> GetScreens() {
    std::vector<Screen> screens;
    EnumDisplayMonitors(nullptr, nullptr, CollectScreen, (LPARAM)&screens);
    std::stable_sort(screens.begin(), screens.end(),
                     [](const Screen& a, const Screen& b) {
                         if (a.primary != b.primary) {
                             return a.primary;
                         }
                         return a.bounds.left < b.bounds.left;
                     });
    return screens;
}

////////////////////////////////////////////////////////////////////////////////
// Tile and restore

// The layout shown last on each monitor, so the next press shows another one.
std::vector<std::wstring> g_lastSigs;
std::mt19937 g_rng{std::random_device{}()};

// How each window looked before the first tiling, for the restore hotkey.
// Tiling again (another layout) keeps what was saved first.
std::map<HWND, WINDOWPLACEMENT> g_undo;

void SaveForUndo(const std::vector<Item>& items) {
    std::erase_if(g_undo,
                  [](const auto& entry) { return !IsWindow(entry.first); });
    for (const auto& item : items) {
        if (g_undo.contains(item.hwnd)) {
            continue;
        }
        WINDOWPLACEMENT wp{sizeof(wp)};
        if (GetWindowPlacement(item.hwnd, &wp)) {
            g_undo[item.hwnd] = wp;
        }
    }
}

void TileWindows(const Settings& settings) {
    HWND focus = GetForegroundWindow();
    std::vector<Screen> screens = GetScreens();
    if (screens.empty()) {
        return;
    }

    // Weigh each open window; weight 0 and anything from a game folder is left
    // alone. Most-used first, ties keep the z-order (most recent first).
    std::vector<Item> items;
    for (HWND hwnd : GetAppWindows()) {
        AppInfo app = GetAppInfo(hwnd, settings);
        if (app.w <= 0) {
            continue;
        }
        Item item;
        item.hwnd = hwnd;
        item.w = app.w;
        item.solo = Contains(settings.mainScreenApps, app.name);
        item.comfortW = Contains(settings.wideApps, app.name) ? 1280 : 800;
        items.push_back(item);
    }
    if (items.empty()) {
        return;
    }
    std::stable_sort(items.begin(), items.end(),
                     [](const Item& a, const Item& b) { return a.w > b.w; });
    SaveForUndo(items);

    auto groups = GetMonitorGroups(items, (int)screens.size());
    g_lastSigs.resize(screens.size());
    std::uniform_int_distribution<unsigned> seedDist(1, INT_MAX);

    for (size_t m = 0; m < screens.size(); m++) {
        auto& g = groups[m];
        const RECT& a = screens[m].work;
        int areaW = a.right - a.left;
        int areaH = a.bottom - a.top;
        // Sizes in settings are at 100% scaling; this monitor may be scaled.
        double scale = GetMonitorScale(screens[m].monitor);
        int half = (int)std::lround(settings.gap * scale) / 2;
        int gap = 2 * half;

        LayoutRequest req;
        req.x = a.left + half;
        req.y = a.top + half;
        req.width = areaW - gap;
        req.height = areaH - gap;
        req.gap = gap;
        req.minW = (int)std::lround(settings.minWidth * scale);
        req.minH = (int)std::lround(settings.minHeight * scale);
        req.comfortH = (int)std::lround(600 * scale);
        req.seed = seedDist(g_rng);
        req.avoid = g_lastSigs[m];

        // Apps that don't fit readably are minimized (least important first).
        // If an app refuses to get as small as its slot, its real size is
        // remembered and the layout is worked out again.
        size_t count = 0;
        for (int pass = 0; pass < 8 && !g.empty(); pass++) {
            req.w.clear();
            req.needW.clear();
            req.needH.clear();
            req.comfortW.clear();
            for (const auto& it : g) {
                req.w.push_back(it.w);
                req.needW.push_back(it.minW);
                req.needH.push_back(it.minH);
                req.comfortW.push_back((int)std::lround(it.comfortW * scale));
            }
            Layout layout = GetBestLayout(req);
            g_lastSigs[m] = layout.sig;
            count = std::min((size_t)layout.count, g.size());
            bool again = false;
            std::vector<HWND> cannotMove;
            for (size_t j = 0; j < count; j++) {
                const Slot& s = layout.slots[j];
                Placed placed = PlaceWindow(g[j].hwnd, s[0] + half, s[1] + half,
                                            s[2] - gap, s[3] - gap);
                if (placed == Placed::kOk) {
                    continue;
                }
                again = true;
                if (placed == Placed::kCannotMove) {
                    // Leave it where it is and lay out the others again.
                    cannotMove.push_back(g[j].hwnd);
                    continue;
                }
                RECT v = GetVisibleRect(g[j].hwnd);
                if (v.right - v.left > s[2] - gap + 2) {
                    g[j].minW = v.right - v.left + gap;
                }
                if (v.bottom - v.top > s[3] - gap + 2) {
                    g[j].minH = v.bottom - v.top + gap;
                }
            }
            std::erase_if(g, [&](const Item& it) {
                return std::find(cannotMove.begin(), cannotMove.end(),
                                 it.hwnd) != cannotMove.end();
            });
            if (!again) {
                break;
            }
        }
        // Minimized only now that the layout is final: a slot freed by a
        // window that can't be moved goes to the next app instead.
        for (size_t j = count; j < g.size(); j++) {
            ShowWindow(g[j].hwnd, SW_SHOWMINNOACTIVE);
        }
    }

    Refocus(focus);
}

// Undoes the tiling: puts every tiled window back to the size, position and
// state (normal, maximized, minimized) it had before the first tiling. Windows
// opened since stay as they are. Nothing is activated, so a game you are in
// keeps focus: normal and minimized windows are restored without activation,
// and maximized ones are maximized by their app (WM_SYSCOMMAND), which Windows
// doesn't let take the focus from the foreground app.
void RestoreWindows() {
    HWND focus = GetForegroundWindow();
    for (auto [hwnd, wp] : g_undo) {
        // Skip closed, hidden and hung windows: SetWindowPlacement would show
        // a hidden window (an app closed to the tray, e.g. Discord) and block
        // until a hung one responds.
        if (!IsWindow(hwnd) || !IsWindowVisible(hwnd) ||
            IsHungAppWindow(hwnd)) {
            continue;
        }
        bool maximized = wp.showCmd == SW_SHOWMAXIMIZED;
        wp.showCmd = wp.showCmd == SW_SHOWMINIMIZED ? SW_SHOWMINNOACTIVE
                                                    : SW_SHOWNOACTIVATE;
        SetWindowPlacement(hwnd, &wp);
        if (maximized) {
            PostMessage(hwnd, WM_SYSCOMMAND, SC_MAXIMIZE, 0);
        }
    }
    g_undo.clear();
    Refocus(focus);
}

////////////////////////////////////////////////////////////////////////////////
// Hotkey thread

enum {
    kHotkeyTile = 1,
    kHotkeyRestore = 2,
};

constexpr UINT kMsgReloadSettings = WM_APP + 1;

std::mutex g_settingsMutex;
std::shared_ptr<Settings> g_settings;
HANDLE g_thread;
DWORD g_threadId;

std::shared_ptr<Settings> GetSettings() {
    std::lock_guard<std::mutex> lock(g_settingsMutex);
    return g_settings;
}

// The message box about hotkey problems, so a new one isn't stacked on top.
HANDLE g_problemThread;

DWORD WINAPI ShowHotkeyProblem(void* param) {
    std::unique_ptr<std::wstring> text((std::wstring*)param);
    MessageBox(nullptr, text->c_str(), L"Tile Windows",
               MB_ICONWARNING | MB_SETFOREGROUND);
    return 0;
}

void RegisterHotkeys(const Settings& settings) {
    UnregisterHotKey(nullptr, kHotkeyTile);
    UnregisterHotKey(nullptr, kHotkeyRestore);
    std::wstring problems;
    UINT tileMods = 0;
    UINT tileVk = 0;
    const std::pair<int, const std::wstring*> hotkeys[] = {
        {kHotkeyTile, &settings.tileHotkey},
        {kHotkeyRestore, &settings.restoreHotkey},
    };
    for (const auto& [id, text] : hotkeys) {
        if (text->empty()) {
            continue;
        }
        UINT mods;
        UINT vk;
        if (!ParseHotkey(*text, &mods, &vk)) {
            problems += L"Can't use the hotkey \"" + *text +
                        L"\" (use e.g. Ctrl+Alt+W: a letter or digit needs "
                        L"Ctrl, Alt, Shift or Win).\n";
        } else if (id == kHotkeyRestore && tileMods == mods && tileVk == vk) {
            problems += *text + L" is set for both hotkeys.\n";
        } else if (!RegisterHotKey(nullptr, id, mods, vk)) {
            problems += *text + L" is already used by another program.\n";
        } else {
            Wh_Log(L"Registered %s", text->c_str());
            if (id == kHotkeyTile) {
                tileMods = mods;
                tileVk = vk;
            }
        }
    }
    if (problems.empty()) {
        return;
    }
    Wh_Log(L"%s", problems.c_str());
    if (g_problemThread) {
        if (WaitForSingleObject(g_problemThread, 0) == WAIT_TIMEOUT) {
            // The last message is still open.
            return;
        }
        CloseHandle(g_problemThread);
        g_problemThread = nullptr;
    }
    // Shown on its own thread so the hotkeys that did work keep working.
    auto* text = new std::wstring(problems +
                                  L"Pick another hotkey in the mod settings.");
    g_problemThread =
        CreateThread(nullptr, 0, ShowHotkeyProblem, text, 0, nullptr);
    if (!g_problemThread) {
        delete text;
    }
}

// No DPI setup needed: the tool process (windhawk.exe in Windhawk 1.7,
// windhawk-mod.exe in 2.0) is per-monitor DPI aware, so all sizes are real
// pixels on every monitor.
DWORD WINAPI HotkeyThread(void* readyEvent) {
    MSG msg;
    // Creates the message queue before anyone posts to it.
    PeekMessage(&msg, nullptr, WM_USER, WM_USER, PM_NOREMOVE);
    // Ready once the hotkeys work.
    RegisterHotkeys(*GetSettings());
    SetEvent((HANDLE)readyEvent);

    while (GetMessage(&msg, nullptr, 0, 0) > 0) {
        if (msg.message == WM_HOTKEY && msg.wParam == kHotkeyTile) {
            TileWindows(*GetSettings());
        } else if (msg.message == WM_HOTKEY && msg.wParam == kHotkeyRestore) {
            RestoreWindows();
        } else if (msg.message == kMsgReloadSettings) {
            RegisterHotkeys(*GetSettings());
        }
    }
    UnregisterHotKey(nullptr, kHotkeyTile);
    UnregisterHotKey(nullptr, kHotkeyRestore);
    return 0;
}

BOOL WhTool_ModInit() {
    Wh_Log(L">");
    g_settings = LoadSettings();
    HANDLE readyEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);
    if (!readyEvent) {
        return FALSE;
    }
    g_thread =
        CreateThread(nullptr, 0, HotkeyThread, readyEvent, 0, &g_threadId);
    if (g_thread) {
        WaitForSingleObject(readyEvent, INFINITE);
    }
    CloseHandle(readyEvent);
    return g_thread != nullptr;
}

void WhTool_ModSettingsChanged() {
    Wh_Log(L">");
    auto settings = LoadSettings();
    {
        std::lock_guard<std::mutex> lock(g_settingsMutex);
        g_settings = settings;
    }
    PostThreadMessage(g_threadId, kMsgReloadSettings, 0, 0);
}

void WhTool_ModUninit() {
    Wh_Log(L">");
    if (g_thread) {
        PostThreadMessage(g_threadId, WM_QUIT, 0, 0);
        WaitForSingleObject(g_thread, 5000);
        CloseHandle(g_thread);
        g_thread = nullptr;
    }
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
