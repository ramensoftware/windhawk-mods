// ==WindhawkMod==
// @id              explorer-mouse-actions-plus
// @name            Explorer Mouse Actions Plus
// @description     在文件资源管理器中为文件项目和空白处分别设置鼠标手势，可返回上一级、复制标签页或在当前目录打开命令提示符。
// @version         0.1.0
// @author          momoca
// @github          https://github.com/momoca
// @include         explorer.exe
// @compilerOptions -lcomctl32 -loleaut32 -lole32 -lshlwapi
// @architecture    x86-64
// ==/WindhawkMod==

// Source code is published under The GNU General Public License v3.0.
//
// Modified by momoca on 2026-11-20, based on Click on Empty Explorer by LiHua81.
// Based on "Explorer Double Click Up" by wrldspawn (GPL-3.0) and
// "Explorer Middle Click Duplicate Tab" by LiHua81 (GPL-3.0).
// UIAutomation usage from "Click on empty taskbar space" (GPL-3.0).
// FileCabinet_CreateViewWindow2 hook from "Classic Explorer Treeview".
// Going up a directory referenced from Open-Shell.

// ==WindhawkModReadme==
/*
# Explorer Mouse Actions Plus

在 Windhawk 设置中添加鼠标规则。每条规则指定点击位置（空白处或文件项目）、修饰键、手势和动作。文件项目包括文件夹。动作作用于点击所在的标签页；现代 Explorer 仍会处理项目的原生点击。

默认双击空白处返回上一级，Ctrl + 双击空白处在当前目录打开 CMD。可以修改或关闭这两条规则，也可以添加 Shift + 双击打开 PowerShell、中键复制标签页等组合。相同条件取第一条启用的规则；如果同一位置和修饰键同时配置了单击、双击或三击，等待系统双击时间后执行匹配的最长手势。

“动作参数”可填写自定义快捷键（如 Ctrl+Shift+N）或文件夹背景菜单的名称或动词。CMD 和 PowerShell 需要文件系统目录；CMD 跳过 UNC 路径。

在 Windhawk 的“创建新模组”编辑器中导入 `explorer-mouse-actions-plus.wh.cpp` 并编译。使用支持 CLI 的 Windhawk 也可运行：

```powershell
D:\Windhawk\windhawk-cli.exe mod install --file .\explorer-mouse-actions-plus.wh.cpp
```

本项目由 momoca 维护，采用 GPL-3.0，沿用了 LiHua81 的 Explorer 钩子、命中检测及部分动作实现。[上游源码](https://github.com/ramensoftware/windhawk-mods/blob/main/mods/click-on-empty-explorer.wh.cpp)
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- rules:
  - - enabled: true
      $name: 启用
    - gesture: double
      $name: 手势
      $options:
        - single: 左键单击
        - triple: 左键三击
        - doubleMiddle: 中键双击
        - double: 左键双击
        - middle: 中键单击
    - modifier: ctrl
      $name: 修饰键
      $options:
        - ctrl: Ctrl
        - shift: Shift
        - alt: Alt
        - plain: 无
    - target: background
      $name: 点击位置
      $options:
        - background: 空白处
        - item: 文件或文件夹
    - action: cmd
      $name: 动作
      $options:
        - cmd: 打开 CMD
        - powershell: 打开 Windows PowerShell
        - copyPath: 复制当前目录
        - goUp: 返回上一级
        - duplicateTab: 复制标签页
        - openInVSCode: 在 VS Code 中打开
        - goBack: 后退
        - goForward: 前进
        - refresh: 刷新
        - newTab: 新建标签页
        - closeTab: 关闭标签页
        - newFolder: 新建文件夹
        - paste: 粘贴
        - customHotkey: 自定义快捷键
        - goToDesktop: 桌面
        - goToHome: 主页
        - openInTerminal: Windows Terminal
        - openWithContextMenu: 文件夹背景菜单
    - argument: ""
      $name: 动作参数
  - - enabled: true
    - gesture: double
    - modifier: plain
    - target: background
    - action: goUp
    - argument: ""
  $name: 鼠标规则
*/
// ==/WindhawkModSettings==

#include <initguid.h>
#include <windhawk_utils.h>
#include <windows.h>
#include <windowsx.h>
#include <shlobj.h>
#include <shobjidl.h>

#include <commctrl.h>
#include <shlwapi.h>
#include <UIAutomationClient.h>
#include <UIAutomationCore.h>
#include <comutil.h>
#include <winrt/base.h>

#include <atomic>
#include <cstdio>
#include <cstdlib>
#include <cwchar>
#include <cwctype>
#include <mutex>
#include <string>
#include <vector>

using bstr_ptr = _bstr_t;

// ---- Global init/shutdown guard ----

static std::atomic<bool> g_initialized = false;

#define CHECK_INIT_OR_DEFER(hWnd, uMsg, wParam, lParam) \
    if (!g_initialized) return DefSubclassProc(hWnd, uMsg, wParam, lParam);
#define CHECK_INIT_OR_RETURN_VOID()  if (!g_initialized) return

// ---- Settings cache (RAII, thread-safe against settings-changed reload) ----
// Uses WindhawkUtils::StringSetting; reloading is an assignment of a fresh
// make() result. Wh_GetStringSetting never returns NULL (it returns L"" when
// unset or on error), so .get() is always a valid pointer.

static std::mutex g_settingsMutex;
struct MouseRule {
    std::wstring gesture, modifier, target, action, argument;
};
static std::vector<MouseRule> g_rules;
static std::atomic<unsigned> g_rulesVersion;
static void LoadSettings() {
    std::lock_guard<std::mutex> lock(g_settingsMutex);
    g_rules.clear();
    ++g_rulesVersion;
    for (int i = 0; ; ++i) {
        auto field = [i](PCWSTR name) {
            std::wstring key = L"rules[" + std::to_wstring(i) + L"]." + name;
            auto value = WindhawkUtils::StringSetting::make(key.c_str());
            return std::wstring(value.get());
        };
        auto gesture = field(L"gesture");
        if (gesture.empty()) break;
        if (Wh_GetIntSetting(L"rules[%d].enabled", i))
            g_rules.push_back({gesture, field(L"modifier"), field(L"target"), field(L"action"), field(L"argument")});
    }
}

// ---- Custom hotkey parsing ----

static WORD ParseHotkeyToken(const wchar_t* token) {
    if (!token || !token[0]) return 0;
    if (_wcsicmp(token, L"Ctrl") == 0 || _wcsicmp(token, L"Control") == 0) return VK_CONTROL;
    if (_wcsicmp(token, L"Shift") == 0)                         return VK_SHIFT;
    if (_wcsicmp(token, L"Alt") == 0 || _wcsicmp(token, L"Menu") == 0) return VK_MENU;
    if (_wcsicmp(token, L"Win") == 0 || _wcsicmp(token, L"Windows") == 0) return VK_LWIN;
    if (wcslen(token) == 1) {
        if (token[0] >= L'A' && token[0] <= L'Z') return (WORD)token[0];
        if (token[0] >= L'a' && token[0] <= L'z') return (WORD)(token[0] - L'a' + L'A');
        if (token[0] >= L'0' && token[0] <= L'9') return (WORD)token[0];
    }
    if (_wcsicmp(token, L"Tab") == 0)        return VK_TAB;
    if (_wcsicmp(token, L"Enter") == 0 || _wcsicmp(token, L"Return") == 0) return VK_RETURN;
    if (_wcsicmp(token, L"Space") == 0)      return VK_SPACE;
    if (_wcsicmp(token, L"Backspace") == 0)  return VK_BACK;
    if (_wcsicmp(token, L"Delete") == 0 || _wcsicmp(token, L"Del") == 0) return VK_DELETE;
    if (_wcsicmp(token, L"Escape") == 0 || _wcsicmp(token, L"Esc") == 0) return VK_ESCAPE;
    if (_wcsicmp(token, L"Left") == 0)       return VK_LEFT;
    if (_wcsicmp(token, L"Right") == 0)      return VK_RIGHT;
    if (_wcsicmp(token, L"Up") == 0)         return VK_UP;
    if (_wcsicmp(token, L"Down") == 0)       return VK_DOWN;
    if (_wcsicmp(token, L"Home") == 0)       return VK_HOME;
    if (_wcsicmp(token, L"End") == 0)        return VK_END;
    if (_wcsicmp(token, L"PageUp") == 0)     return VK_PRIOR;
    if (_wcsicmp(token, L"PageDown") == 0)   return VK_NEXT;
    if (_wcsicmp(token, L"Insert") == 0 || _wcsicmp(token, L"Ins") == 0) return VK_INSERT;
    // F1-F24
    if ((token[0] == L'F' || token[0] == L'f') && token[1]) {
        int n = (int)wcstol(token + 1, NULL, 10);
        if (n >= 1 && n <= 24) return VK_F1 + (n - 1);
    }
    return 0;
}

// Release held side-modifiers, build combo, restore — all in one SendInput.
// Factored out so both SendKeyCombo and SendParsedHotkey use it.
static void InjectKeys(std::vector<INPUT>& in) {
    static constexpr WORD kSideMods[] = {VK_LCONTROL, VK_RCONTROL, VK_LMENU,
                                         VK_RMENU, VK_LSHIFT, VK_RSHIFT};
    std::vector<WORD> held;
    for (WORD vk : kSideMods)
        if (GetKeyState(vk) & 0x8000) held.push_back(vk);

    std::vector<INPUT> batch;
    auto Key = [&](WORD vk, DWORD flags) {
        batch.push_back(INPUT{INPUT_KEYBOARD, {.ki = {.wVk = vk, .dwFlags = flags}}});
    };
    for (WORD vk : held) Key(vk, KEYEVENTF_KEYUP);
    batch.insert(batch.end(), in.begin(), in.end());
    for (WORD vk : held) Key(vk, 0);

    SendInput((UINT)batch.size(), batch.data(), sizeof(INPUT));
}

static void SendKeyCombo(WORD vk1, WORD vk2, WORD vk3 = 0) {
    std::vector<INPUT> in;
    auto Key = [&](WORD vk, DWORD flags) {
        in.push_back(INPUT{INPUT_KEYBOARD, {.ki = {.wVk = vk, .dwFlags = flags}}});
    };
    Key(vk1, 0); Key(vk2, 0); if (vk3) Key(vk3, 0);
    if (vk3) Key(vk3, KEYEVENTF_KEYUP);
    Key(vk2, KEYEVENTF_KEYUP); Key(vk1, KEYEVENTF_KEYUP);
    InjectKeys(in);
}

static void SendParsedHotkey(const std::wstring& combo) {
    auto trim = [](std::wstring s) {
        size_t a = s.find_first_not_of(L" \t");
        if (a == std::wstring::npos) return std::wstring();
        size_t b = s.find_last_not_of(L" \t");
        return s.substr(a, b - a + 1);
    };
    std::vector<WORD> keys;
    size_t start = 0;
    while (start < combo.length()) {
        size_t end = combo.find(L'+', start);
        std::wstring token = trim((end == std::wstring::npos)
            ? combo.substr(start) : combo.substr(start, end - start));
        if (token.empty()) { if (end == std::wstring::npos) break; start = end + 1; continue; }
        WORD vk = ParseHotkeyToken(token.c_str());
        if (!vk) {
            Wh_Log(L"SendParsedHotkey: unknown key \"%s\" in combo \"%s\"",
                   token.c_str(), combo.c_str());
            return;
        }
        keys.push_back(vk);
        if (end == std::wstring::npos) break;
        start = end + 1;
    }
    if (keys.empty() || keys.size() > 4) {
        Wh_Log(L"SendParsedHotkey: unparseable hotkey combo \"%s\"", combo.c_str());
        return;
    }

    std::vector<INPUT> in;
    auto Key = [&](WORD vk, DWORD flags) {
        in.push_back(INPUT{INPUT_KEYBOARD, {.ki = {.wVk = vk, .dwFlags = flags}}});
    };
    for (size_t i = 0; i < keys.size(); i++) Key(keys[i], 0);
    for (int i = (int)keys.size() - 1; i >= 0; i--) Key(keys[i], KEYEVENTF_KEYUP);
    InjectKeys(in);
}

// ---- Invoke a folder background context menu entry (borrow the right-click menu) ----
// Reuses exactly what Explorer shows when you right-click empty space in a folder, so any
// program that registered an "Open in ..." (or similar) verb works without hard-coded paths.
// matchText is matched (case-insensitive, space-insensitive) against both the menu display
// text and the underlying verb (e.g. "VS Code" matches verb "VSCode" or text "Open with Code").
//
// SAFETY NOTE: Called from Explorer UI thread subclass proc — COM must already be initialized
// (Explorer does that internally). All PIDL/interface clean-up is manual and exception-safe
// (no C++ exceptions; early-return via if-guards). No window subclassing is used: cascading
// submenus are populated by calling IContextMenu2::HandleMenuMsg(WM_INITMENUPOPUP, ...) directly.

// Normalize: lowercase + drop whitespace + drop & accelerator markers.
// "Open Git Ba&sh here" → "opengitbashhere", so user typing "gitbash" matches.
// "通过 Code 打开" → "通过code打开", so user typing "code" or "通过code" matches.
static std::wstring NormalizeForMatch(PCWSTR s) {
    std::wstring out;
    if (!s) return out;
    out.reserve(wcslen(s) + 1);
    for (const wchar_t* p = s; *p; ++p) {
        if (iswspace((wint_t)*p) || *p == L'&') continue;
        out.push_back(*p);
    }
    // Unicode-aware, locale-independent lowercasing so exact matches also work
    // for Cyrillic/Greek/accented text (towlower only folds ASCII in the "C"
    // locale, which would silently break the case-insensitive promise).
    if (!out.empty())
        CharLowerBuffW(&out[0], (DWORD)out.size());
    return out;
}

// Case-insensitive substring test after normalization.
// This lets "VS Code" match the verb "VSCode" and "Open with Code" match "Code", etc.
static bool StrContainsNorm(PCWSTR haystack, PCWSTR needle) {
    if (!haystack || !needle || !*needle) return false;
    std::wstring h = NormalizeForMatch(haystack);
    std::wstring n = NormalizeForMatch(needle);
    // A match string that normalizes to empty (e.g. a stray space, "&", or a tab)
    // would otherwise make StrStrIW succeed for the first entry it walks — don't.
    if (n.empty()) return false;
    return StrStrIW(h.c_str(), n.c_str()) != NULL;
}

// Recursively walk the (possibly nested) context menu, invoking the first item whose
// display text or verb contains matchText. Returns true and invokes on success.
// If dumpLines is non-null, collects diagnostic lines during the walk so the caller
// doesn't need a second walk on failure (avoids re-running shell extensions).
static bool EnumContextMenuMatch(HMENU hMenu, IContextMenu* pcm, IContextMenu2* pcm2,
                                 HWND hwnd, PCWSTR matchText, int idCmdFirst,
                                 std::vector<std::wstring>* dumpLines = nullptr,
                                 int depth = 0) {
    std::wstring normMatch = NormalizeForMatch(matchText);
    if (normMatch.empty()) return false;

    int count = GetMenuItemCount(hMenu);

    // Collect this level's leaf items and submenus (in menu order) and build the
    // diagnostic dump. Matching runs afterwards in three passes so that:
    //   1) an exact (normalized text/verb) hit beats a substring hit — typing the
    //      full entry name is immune to "Encode"/"Decode"-style misfires;
    //   2) top-level entries are tested before nested ones, as the README promises
    //      ("first one in menu order"), which also avoids expanding submenus when a
    //      top-level entry already matches.
    struct LeafItem { UINT offset; std::wstring text, verb, normText, normVerb; };
    struct SubmenuItem { int index; HMENU hSubMenu; };
    std::vector<LeafItem> leaves;
    std::vector<SubmenuItem> submenus;

    for (int i = 0; i < count; i++) {
        MENUITEMINFOW mii = { sizeof(mii) };
        mii.fMask = MIIM_ID | MIIM_SUBMENU | MIIM_STATE;
        if (!GetMenuItemInfoW(hMenu, i, TRUE, &mii)) continue;

        if (mii.hSubMenu != NULL) {
            if (dumpLines) {
                wchar_t stext[MAX_PATH] = {};
                MENUITEMINFOW miiT = { sizeof(miiT), MIIM_STRING };
                miiT.dwTypeData = stext; miiT.cch = MAX_PATH;
                GetMenuItemInfoW(hMenu, i, TRUE, &miiT);
                wchar_t buf[512];
                _snwprintf_s(buf, _countof(buf), _TRUNCATE,
                    L"CMENU%s[%s] (submenu) wID=%u",
                    std::wstring(depth, L' ').c_str(),
                    stext[0] ? stext : L"(no text)", mii.wID);
                dumpLines->push_back(buf);
            }
            submenus.push_back({ i, mii.hSubMenu });
            continue;
        }

        // Leaf item: skip separators, items outside the context-menu command
        // range, and disabled/greyed entries (invoking those fails anyway, and a
        // live entry matching the same text should win).
        if (mii.wID == 0) continue;
        if (mii.wID < (UINT)idCmdFirst || mii.wID > 0x7FFF) continue;
        if (mii.fState & (MFS_DISABLED | MFS_GRAYED)) continue;

        UINT offset = mii.wID - idCmdFirst;

        wchar_t verb[MAX_PATH] = {};
        // Prefer the wide form; fall back to ANSI + CP_ACP only if the verb
        // handler doesn't support GCS_VERBW. This avoids the wide-into-ANSI
        // round trip that corrupts verbs containing non-ANSI characters.
        if (FAILED(pcm->GetCommandString(offset, GCS_VERBW, NULL, (CHAR*)verb, MAX_PATH))) {
            CHAR verbA[MAX_PATH] = {};
            if (SUCCEEDED(pcm->GetCommandString(offset, GCS_VERBA, NULL, verbA, MAX_PATH)))
                MultiByteToWideChar(CP_ACP, 0, verbA, -1, verb, MAX_PATH);
        }

        wchar_t text[MAX_PATH] = {};
        MENUITEMINFOW miiT = { sizeof(miiT), MIIM_STRING };
        miiT.dwTypeData = text; miiT.cch = MAX_PATH;
        GetMenuItemInfoW(hMenu, i, TRUE, &miiT);

        leaves.push_back({ offset, text, verb,
                           NormalizeForMatch(text), NormalizeForMatch(verb) });

        if (dumpLines) {
            wchar_t buf[640];
            _snwprintf_s(buf, _countof(buf), _TRUNCATE,
                L"CMENU%s[%s] wID=%u offset=%u verb=[%s]  → match: \"%s\" or \"%s\"",
                std::wstring(depth, L' ').c_str(),
                text[0] ? text : L"(no text)", mii.wID, offset,
                verb[0] ? verb : L"(none)",
                leaves.back().normText.c_str(),
                leaves.back().normVerb[0] ? leaves.back().normVerb.c_str() : L"<no verb>");
            dumpLines->push_back(buf);
        }
    }

    auto invokeLeaf = [&](const LeafItem& it) -> bool {
        // CMINVOKECOMMANDINFOEX with the UNICODE flag: some shell extensions
        // behave better with the wide form, and the offset verb works with it.
        CMINVOKECOMMANDINFOEX ci = { sizeof(ci) };
        ci.hwnd = hwnd;
        ci.fMask = CMIC_MASK_UNICODE;
        ci.lpVerb = MAKEINTRESOURCEA(it.offset);
        ci.lpVerbW = MAKEINTRESOURCEW(it.offset);
        ci.nShow = SW_SHOWNORMAL;
        return SUCCEEDED(pcm->InvokeCommand((CMINVOKECOMMANDINFO*)&ci));
    };

    // Pass 1: exact match on the normalized text or verb.
    for (const auto& it : leaves) {
        if (it.normText == normMatch || it.normVerb == normMatch) {
            // Once a matching entry is found, dispatch it and stop. A verb can
            // return a failure HRESULT *after* already doing work, so falling
            // through would risk running a second, unrelated command.
            if (!invokeLeaf(it))
                Wh_Log(L"EnumContextMenuMatch: InvokeCommand failed for '%s'", it.text.c_str());
            return true;
        }
    }
    // Pass 2: substring match.
    for (const auto& it : leaves) {
        if (StrContainsNorm(it.text.c_str(), matchText) ||
            StrContainsNorm(it.verb.c_str(), matchText)) {
            // Once a matching entry is found, dispatch it and stop. A verb can
            // return a failure HRESULT *after* already doing work, so falling
            // through would risk running a second, unrelated command.
            if (!invokeLeaf(it))
                Wh_Log(L"EnumContextMenuMatch: InvokeCommand failed for '%s'", it.text.c_str());
            return true;
        }
    }
    // Pass 3: descend into submenus in menu order.
    for (const auto& sm : submenus) {
        if (pcm2)
            pcm2->HandleMenuMsg(WM_INITMENUPOPUP, (WPARAM)sm.hSubMenu, MAKELPARAM(sm.index, 0));
        if (EnumContextMenuMatch(sm.hSubMenu, pcm, pcm2, hwnd, matchText, idCmdFirst,
                                 dumpLines, depth + 2))
            return true;
    }
    return false;
}

// Get the background context menu directly from the shell view.
// Works for virtual folders (This PC, Libraries, Recycle Bin, search results)
// where SHGetPathFromIDListW returns FALSE.
static bool InvokeFolderContextMenuFromBrowser(IShellBrowser* browser, HWND hwnd, PCWSTR matchText) {
    if (!browser || !matchText || !matchText[0]) return false;

    IShellView* psv = nullptr;
    if (FAILED(browser->QueryActiveShellView(&psv)) || !psv) return false;

    IContextMenu* pcm = nullptr;
    HRESULT hr = psv->GetItemObject(SVGIO_BACKGROUND, IID_IContextMenu, (void**)&pcm);
    psv->Release();
    if (FAILED(hr) || !pcm) return false;

    IContextMenu2* pcm2 = nullptr;
    pcm->QueryInterface(IID_IContextMenu2, (void**)&pcm2);

    bool found = false;
    HMENU hMenu = CreatePopupMenu();
    if (hMenu && SUCCEEDED(pcm->QueryContextMenu(hMenu, 0, 1, 0x7FFF, CMF_NORMAL))) {
        std::vector<std::wstring> dumpLines;
        found = EnumContextMenuMatch(hMenu, pcm, pcm2, hwnd, matchText, 1, &dumpLines);
        if (!found) {
            Wh_Log(L"No match for '%s' — dumping all context menu items:", matchText);
            for (auto& line : dumpLines) Wh_Log(L"%s", line.c_str());
        }
    }
    if (hMenu) DestroyMenu(hMenu);
    if (pcm2) pcm2->Release();
    pcm->Release();
    return found;
}

// ---- Duplicate Tab infrastructure ----

static thread_local wchar_t g_pendingNavPath[MAX_PATH] = {};
// Raw pointer with explicit AddRef/Release (instead of winrt::com_ptr) so the
// thread-local has a trivial destructor — a com_ptr would register a thread-exit
// destructor living in the mod image. Released on the window's own thread via
// ReleasePendingNavForThread (g_msgTeardown / WM_NCDESTROY), never from
// Wh_ModUninit's arbitrary thread.
static thread_local IShellBrowser* g_pendingNavBrowser = nullptr;

// Release the per-thread pending-nav browser and clear the pending path.
// Must run on the thread that stored them (the Explorer UI thread).
static void ReleasePendingNavForThread() {
    if (g_pendingNavBrowser) {
        g_pendingNavBrowser->Release();
        g_pendingNavBrowser = nullptr;
    }
    g_pendingNavPath[0] = L'\0';
}

static VOID CALLBACK NavigateNewTabProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime);
// Private message: dequeues action dispatch from mouse handlers to avoid
// blocking on COM activation inside WM_LBUTTONDOWN/WM_MBUTTONDOWN.
// It carries NO pointer in wParam — the action data lives in a thread-local
// queue (g_pendingActions) owned by the same thread that posts and dispatches
// it, so a message from another process or a stale post can never make
// explorer.exe free an arbitrary address.
static UINT g_msgDoAction = 0;
// Teardown message: handled on the window's own thread to kill pending timers
// (timers must be killed on their creating thread) and release per-thread COM.
static UINT g_msgTeardown = 0;

// Carries an action plus an optional context-menu match string across the
// posted-message boundary. Using an owned object (instead of a hand-packed
// wchar buffer) avoids the out-of-bounds read the previous layout had when
// no match text was present.
struct PendingAction {
    HWND target;        // the window this action was posted for
    std::wstring action;
    std::wstring match;
};

// Per-thread FIFO backing the deferred-action dispatch. The producer (mouse
// handlers) and the consumer (the g_msgDoAction handler) always run on the
// same Explorer UI thread, so a simple vector is sufficient and we never pass
// an owning pointer through a globally-visible window message. Drained in the
// g_msgTeardown handler (thread_local destructors only run at thread exit, and
// Explorer's UI threads outlive the mod), so a disable/enable cycle cannot
// leak or double-free an allocation from a previous instance.
static thread_local std::vector<PendingAction> g_pendingActions;

static bool FindShellTabAndDoAction(HWND hWnd, PCWSTR action, PCWSTR match = nullptr);

// Post an action to be handled asynchronously — avoids synchronously activating
// context-menu handlers inside the mouse-down handler.
static void PostDoAction(HWND hWnd, PCWSTR action, PCWSTR match = nullptr) {
    if (!g_msgDoAction || !action || !*action) return;
    g_pendingActions.push_back({ hWnd, action, (match && *match) ? match : L"" });
    if (!PostMessage(hWnd, g_msgDoAction, 0, 0))
        g_pendingActions.pop_back();
}

// A delayed action may fire up to GetDoubleClickTime() (~500 ms) after the
// click. SendInput injects into the *foreground* window, so by then the user
// may have moved on (Alt+Tab, notifications, a launched app). Gate the delayed
// dispatch on the clicked Explorer frame still being foreground so keys can't
// land in an unrelated window (closing a browser tab, pasting into another app).
static bool StillForeground(HWND hWnd) {
    HWND root = GetAncestor(hWnd, GA_ROOT);
    return root && root == GetForegroundWindow();
}

// ---- ExplorerWrapper ----

class ExplorerWrapper {
    winrt::com_ptr<IShellBrowser> hBrowser;

    // Helper: get current folder path via IShellView chain
    bool GetCurrentFolderPath(wchar_t* outPath) {
        IShellView* psv = nullptr;
        if (FAILED(hBrowser->QueryActiveShellView(&psv)) || !psv)
            return false;
        IFolderView* pfv = nullptr;
        if (FAILED(psv->QueryInterface(IID_PPV_ARGS(&pfv)))) {
            psv->Release();
            return false;
        }
        IPersistIDList* pidlList = nullptr;
        if (FAILED(pfv->GetFolder(IID_PPV_ARGS(&pidlList)))) {
            pfv->Release();
            psv->Release();
            return false;
        }
        PIDLIST_ABSOLUTE pidl = nullptr;
        bool ok = SUCCEEDED(pidlList->GetIDList(&pidl)) && pidl
                  && SHGetPathFromIDListW(pidl, outPath) && outPath[0];
        if (pidl) CoTaskMemFree(pidl);
        pidlList->Release();
        pfv->Release();
        psv->Release();
        return ok;
    }

public:
    HWND hShellTab = NULL;
    HWND m_timerHwnd = NULL;  // subclassed HWND for timer messages

    ExplorerWrapper(HWND shellTab, IShellBrowser* hShellBrowser, HWND timerHwnd) {
        hShellTab = shellTab;
        hBrowser.copy_from(hShellBrowser);
        m_timerHwnd = timerHwnd;
    }

    // ---- Navigation ----

    void GoUp() {
        hBrowser->BrowseObject(NULL, SBSP_SAMEBROWSER | SBSP_PARENT);
    }

    void GoBack() {
        hBrowser->BrowseObject(NULL, SBSP_SAMEBROWSER | SBSP_NAVIGATEBACK);
    }

    void GoForward() {
        hBrowser->BrowseObject(NULL, SBSP_SAMEBROWSER | SBSP_NAVIGATEFORWARD);
    }

    void GoToDesktop() {
        PIDLIST_ABSOLUTE pidl = nullptr;
        if (SUCCEEDED(SHGetFolderLocation(NULL, CSIDL_DESKTOP, NULL, 0, &pidl)) && pidl) {
            hBrowser->BrowseObject(pidl, SBSP_SAMEBROWSER | SBSP_ABSOLUTE);
            CoTaskMemFree(pidl);
        }
    }

    void GoToHome() {
        PIDLIST_ABSOLUTE pidl = nullptr;
        if (SUCCEEDED(SHParseDisplayName(
                L"shell:::{679f85cb-0220-4080-b29b-5540cc05aab6}",
                NULL, &pidl, 0, NULL)) && pidl) {
            hBrowser->BrowseObject(pidl, SBSP_SAMEBROWSER | SBSP_ABSOLUTE);
            CoTaskMemFree(pidl);
        }
    }

    // ---- View ----

    void Refresh() {
        IShellView* psv = nullptr;
        if (SUCCEEDED(hBrowser->QueryActiveShellView(&psv)) && psv) {
            psv->Refresh();
            psv->Release();
        }
    }

    // ---- Tab operations ----

    void NewTab() {
        SendKeyCombo(VK_CONTROL, 'T');
    }

    void DuplicateTab() {
        wchar_t path[MAX_PATH] = {};
        if (!GetCurrentFolderPath(path)) return;
        wcsncpy(g_pendingNavPath, path, MAX_PATH - 1);
        g_pendingNavPath[MAX_PATH - 1] = L'\0';
        if (g_pendingNavBrowser) {  // release a previously pending browser, if any
            g_pendingNavBrowser->Release();
            g_pendingNavBrowser = nullptr;
        }
        SendKeyCombo(VK_CONTROL, 'T');
        SetTimer(m_timerHwnd, 0x4D43, 500, nullptr);
    }

    void CloseTab() {
        SendKeyCombo(VK_CONTROL, 'W');
    }

    // ---- File operations ----

    void NewFolder() {
        SendKeyCombo(VK_CONTROL, VK_SHIFT, 'N');
    }

    void Paste() {
        SendKeyCombo(VK_CONTROL, 'V');
    }

    void CopyPath() {
        wchar_t path[MAX_PATH] = {};
        if (!GetCurrentFolderPath(path)) return;
        size_t len = wcslen(path) + 1;
        HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, len * sizeof(wchar_t));
        if (!hMem) return;
        wchar_t* pMem = (wchar_t*)GlobalLock(hMem);
        if (!pMem) { GlobalFree(hMem); return; }
        wcsncpy(pMem, path, len);
        GlobalUnlock(hMem);
        if (OpenClipboard(NULL)) {
            EmptyClipboard();
            if (!SetClipboardData(CF_UNICODETEXT, hMem))
                GlobalFree(hMem);
            CloseClipboard();
        } else {
            GlobalFree(hMem);
        }
    }

    // ---- External program launchers ----

    void OpenInVSCode(PCWSTR match) {
        // "Code" is a loose substring match — entries like "Encode with HandBrake"
        // also contain "code" and may be invoked instead. Route through the generic
        // path so a per-trigger Context Menu Match can override the default; see the
        // README note under "Open in VS Code".
        OpenWithContextMenu(match && *match ? match : L"Code");
    }

    void OpenInTerminal() {
        wchar_t path[MAX_PATH] = {};
        if (!GetCurrentFolderPath(path)) {
            Wh_Log(L"OpenInTerminal: no real folder path (virtual folder?), nothing to open");
            return;
        }
        // Launch Windows Terminal directly in the current folder. Avoids the fragile
        // context-menu text/verb matching (which breaks on non-English Windows / updates).
        // A drive root (C:\) ends with a backslash; double it so the closing quote
        // isn't escaped by command-line parsing ("C:\" -> "C:\\").
        std::wstring dir = path;
        if (!dir.empty() && dir.back() == L'\\')
            dir.push_back(L'\\');
        std::wstring cmdLine = L"wt.exe -d \"" + dir + L"\"";
        STARTUPINFOW si = { sizeof(si) };
        PROCESS_INFORMATION pi = {};
        if (CreateProcessW(NULL, cmdLine.data(), NULL, NULL, FALSE, 0, NULL, path, &si, &pi)) {
            CloseHandle(pi.hThread);
            CloseHandle(pi.hProcess);
        } else {
            Wh_Log(L"OpenInTerminal: failed to launch wt.exe (err=%lu)", GetLastError());
        }
    }

    void OpenWithContextMenu(PCWSTR match = nullptr) {
        std::wstring m = match ? match : L"";
        if (m.empty()) {
            Wh_Log(L"OpenWithContextMenu: no match text configured (Context Menu Match setting)");
            return;
        }
        // Use browser directly — works for virtual folders too (This PC, Libraries, etc.)
        // Own any verb UI (Properties, extension dialogs) with the top-level Explorer
        // frame rather than the ShellTabWindowClass child, so the dialog can't end up
        // behind the frame.
        HWND hwndOwner = GetAncestor(hShellTab, GA_ROOT);
        if (!hwndOwner) hwndOwner = hShellTab;
        if (!InvokeFolderContextMenuFromBrowser(hBrowser.get(), hwndOwner, m.c_str()))
            Wh_Log(L"OpenWithContextMenu: no context menu entry matching '%s'", m.c_str());
    }

    void OpenConsole(bool powershell) {
        wchar_t path[MAX_PATH]{}, system[MAX_PATH]{};
        if (!GetCurrentFolderPath(path) || !PathIsDirectoryW(path)) return;
        if (!powershell && PathIsUNCW(path)) {
            Wh_Log(L"CMD cannot retain a UNC working directory");
            return;
        }
        if (!GetSystemDirectoryW(system, ARRAYSIZE(system))) return;
        std::wstring exe = std::wstring(system) + (powershell ?
            L"\\WindowsPowerShell\\v1.0\\powershell.exe" : L"\\cmd.exe");
        std::wstring args = L"\"" + exe + (powershell ? L"\" -NoProfile" : L"\" /D");
        STARTUPINFOW startup{sizeof(startup)};
        PROCESS_INFORMATION process{};
        if (CreateProcessW(exe.c_str(), args.data(), nullptr, nullptr, FALSE,
                           CREATE_NEW_CONSOLE, nullptr, path, &startup, &process)) {
            CloseHandle(process.hThread);
            CloseHandle(process.hProcess);
        } else Wh_Log(L"CreateProcessW failed: %u", GetLastError());
    }

    void DoAction(PCWSTR action, PCWSTR match = nullptr) {
        if (!action || !hBrowser || !IsWindow(hShellTab)) return;

        if (!wcscmp(action, L"customHotkey")) SendParsedHotkey(match ? match : L"");
        else if (!wcscmp(action, L"cmd")) OpenConsole(false);
        else if (!wcscmp(action, L"powershell")) OpenConsole(true);
        else if (wcscmp(action, L"goUp") == 0)            GoUp();
        else if (wcscmp(action, L"goBack") == 0)     GoBack();
        else if (wcscmp(action, L"goForward") == 0)  GoForward();
        else if (wcscmp(action, L"goToDesktop") == 0) GoToDesktop();
        else if (wcscmp(action, L"goToHome") == 0)    GoToHome();
        else if (wcscmp(action, L"refresh") == 0)     Refresh();
        else if (wcscmp(action, L"newTab") == 0)      NewTab();
        else if (wcscmp(action, L"duplicateTab") == 0) DuplicateTab();
        else if (wcscmp(action, L"closeTab") == 0)    CloseTab();
        else if (wcscmp(action, L"newFolder") == 0)   NewFolder();
        else if (wcscmp(action, L"copyPath") == 0)    CopyPath();
        else if (wcscmp(action, L"paste") == 0)       Paste();
        else if (wcscmp(action, L"openInVSCode") == 0)  OpenInVSCode(match);
        else if (wcscmp(action, L"openInTerminal") == 0) OpenInTerminal();
        else if (wcscmp(action, L"openWithContextMenu") == 0) OpenWithContextMenu(match);
        // "none" or unknown — do nothing
    }
};

// ---- Globals ----

// Track subclassed windows so Wh_ModUninit can remove subclasses
// for windows created both during and after init.
struct SubclassEntry { HWND hWnd; bool isListView; };
static std::vector<SubclassEntry> g_subclassed;
static std::mutex g_subclassMutex;

// Record a subclassed window, skipping duplicates (e.g. a view that was
// recreated after a view-mode switch, or a window already picked up by both
// the CreateWindowExW hook and the Wh_ModAfterInit enumeration).
static void TrackSubclassedWindow(HWND hWnd, bool isListView) {
    std::lock_guard<std::mutex> lk(g_subclassMutex);
    for (auto& e : g_subclassed)
        if (e.hWnd == hWnd) return;
    g_subclassed.push_back({ hWnd, isListView });
}

// Per-thread UIAutomation — each Explorer window runs on its own STA thread.
// Stored thread_local and released on the window's own thread (see
// ReleaseUIAutomationForThread / g_msgTeardown): a COM object has thread
// affinity, so it must not be released from Wh_ModUninit's arbitrary thread,
// and Releasing during DLL_PROCESS_DETACH is unsafe too.
static thread_local IUIAutomation* g_pUIAutomation = nullptr;

static IUIAutomation* GetUIAutomation() {
    if (!g_pUIAutomation) {
        CoCreateInstance(CLSID_CUIAutomation, NULL, CLSCTX_INPROC_SERVER,
                         __uuidof(IUIAutomation), (void**)&g_pUIAutomation);
    }
    return g_pUIAutomation;
}

// Release the per-thread UIAutomation. Must be called on the thread that
// created it (the window's own thread), never from Wh_ModUninit.
static void ReleaseUIAutomationForThread() {
    if (g_pUIAutomation) {
        g_pUIAutomation->Release();
        g_pUIAutomation = nullptr;
    }
}

// ---- NavigateNewTabProc (timer callback for duplicate tab) ----

static VOID CALLBACK NavigateNewTabProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime) {
    KillTimer(hwnd, 0x4D43);
    CHECK_INIT_OR_RETURN_VOID();

    if (!g_pendingNavPath[0] || !g_pendingNavBrowser) {
        ReleasePendingNavForThread();
        return;
    }

    // Hold an owning reference across BrowseObject: it navigates and pumps
    // messages, so a nested WM_NCDESTROY / g_msgTeardown on this thread could
    // call ReleasePendingNavForThread() mid-call and drop g_pendingNavBrowser's
    // reference. Holding our own keeps the object alive for the call; the nested
    // null-out still does the right thing for the next duplicate.
    winrt::com_ptr<IShellBrowser> browser;
    browser.copy_from(g_pendingNavBrowser);

    PIDLIST_ABSOLUTE pidl = nullptr;
    if (SUCCEEDED(SHParseDisplayName(g_pendingNavPath, NULL, &pidl, 0, NULL)) && pidl) {
        browser->BrowseObject(pidl, SBSP_SAMEBROWSER | SBSP_ABSOLUTE);
        CoTaskMemFree(pidl);
    }

    ReleasePendingNavForThread();
}

// ---- Helper: find ExplorerWrapper by shellTab HWND and run action ----

static bool FindShellTabAndDoAction(HWND hWnd, PCWSTR action, PCWSTR match) {
    if (!hWnd || !IsWindow(hWnd) || !action) {
        Wh_Log(L"FindShellTabAndDoAction: invalid args hWnd=%p action=%s", hWnd, action ? action : L"null");
        return false;
    }
    int limit = 16;
    HWND parent = GetParent(hWnd);
    while (parent && limit-- > 0) {
        wchar_t className[256];
        if (!GetClassName(parent, className, 256)) break;
        if (wcscmp(className, L"ShellTabWindowClass") == 0) {
            HWND shellTab = parent;
            // Query the browser on-demand instead of caching in g_Wrappers
            // (WM_USER+7 returns a borrowed pointer, so copy_from to AddRef)
            winrt::com_ptr<IShellBrowser> browser;
            browser.copy_from(reinterpret_cast<IShellBrowser*>(
                (void*)SendMessage(shellTab, WM_USER + 7, 0, 0)));
            if (browser) {
                ExplorerWrapper tmp(shellTab, browser.get(), hWnd);
                tmp.DoAction(action, match);
                return true;
            }
            Wh_Log(L"FindShellTabAndDoAction: no browser for shellTab=%p, action=%s", shellTab, action);
            break;
        }
        parent = GetParent(parent);
    }
    Wh_Log(L"FindShellTabAndDoAction: ShellTabWindowClass not found from hWnd=%p, action=%s", hWnd, action);
    return false;
}

// ---- SysListView32 subclass ----

struct ClickSequence {
    HWND hwnd{};
    POINT point{};
    DWORD time{};
    int mask{}, item{}, count{};
    bool middle{};
    unsigned version{};
    MouseRule pending;
};
static thread_local ClickSequence sequence;
static constexpr UINT_PTR kClickTimer = 0x4D44;

static void FinishSequence(bool dispatch) {
    if (!sequence.hwnd) return;
    KillTimer(sequence.hwnd, kClickTimer);
    if (dispatch && sequence.version == g_rulesVersion && StillForeground(sequence.hwnd))
        PostDoAction(sequence.hwnd, sequence.pending.action.c_str(), sequence.pending.argument.c_str());
    sequence = {};
}

static bool HandleRule(HWND hwnd, UINT msg, LPARAM pos, bool list) {
    bool middle = msg == WM_MBUTTONDOWN || msg == WM_MBUTTONDBLCLK;
    int mask = (!!(GetKeyState(VK_CONTROL)&0x8000)) |
               ((!!(GetKeyState(VK_SHIFT)&0x8000)) << 1) |
               ((!!(GetKeyState(VK_MENU)&0x8000)) << 2);
    PCWSTR mod = mask == 0 ? L"plain" : mask == 1 ? L"ctrl" :
                 mask == 2 ? L"shift" : mask == 4 ? L"alt" : L"";
    std::vector<MouseRule> candidates;
    unsigned version;
    {
        std::lock_guard lock(g_settingsMutex);
        version = g_rulesVersion;
        for (const auto& r : g_rules)
            if (r.modifier == mod && middle == (r.gesture == L"middle" || r.gesture == L"doubleMiddle"))
                candidates.push_back(r);
    }
    if (candidates.empty()) { FinishSequence(true); return false; }
    POINT pt{GET_X_LPARAM(pos), GET_Y_LPARAM(pos)};
    int item = -1;
    if (list) {
        LVHITTESTINFO hit{};
        hit.pt = pt;
        item = ListView_SubItemHitTest(hwnd, &hit);
    } else {
        if (!ClientToScreen(hwnd, &pt)) return false;
        winrt::com_ptr<IUIAutomation> automation;
        automation.copy_from(GetUIAutomation());
        winrt::com_ptr<IUIAutomationElement> el;
        if (!automation || FAILED(automation->ElementFromPoint(pt, el.put())) || !el)
            return false;
        winrt::com_ptr<IUIAutomationTreeWalker> walker;
        automation->get_ControlViewWalker(walker.put());
        bool found = false;
        for (int depth = 0; el && depth < 8; ++depth) {
            bstr_ptr cls;
            CONTROLTYPEID type{};
            el->get_CurrentControlType(&type);
            el->get_CurrentClassName(cls.GetAddress());
            PCWSTR name = cls.GetBSTR();
            if (type == UIA_ListItemControlTypeId) { item = 0; found = true; break; }
            if (name && (!wcscmp(name,L"UIItemsView") || !wcscmp(name,L"UIGroupItem"))) {
                found = depth == 0;
                break;
            }
            winrt::com_ptr<IUIAutomationElement> parent;
            if (!walker || FAILED(walker->GetParentElement(el.get(),parent.put()))) break;
            el = std::move(parent);
        }
        if (!found) return false;
    }
    DWORD now = GetMessageTime();
    bool continues = sequence.hwnd == hwnd && sequence.mask == mask &&
        sequence.version == version && sequence.middle == middle && sequence.item == item &&
        now - sequence.time <= GetDoubleClickTime() &&
        abs(pt.x-sequence.point.x) <= GetSystemMetrics(SM_CXDOUBLECLK)/2 &&
        abs(pt.y-sequence.point.y) <= GetSystemMetrics(SM_CYDOUBLECLK)/2;
    if (!continues) FinishSequence(true);
    int count = sequence.count + 1;
    const MouseRule* exact = nullptr;
    bool longer = false;
    for (const auto& rule : candidates) {
        if (rule.target != (item < 0 ? L"background" : L"item")) continue;
        int length = rule.gesture == L"triple" ? 3 :
            (rule.gesture == L"double" || rule.gesture == L"doubleMiddle") ? 2 : 1;
        if (length == count && !exact) exact = &rule;
        longer |= length > count;
    }
    if (!exact && !longer) { FinishSequence(true); return false; }
    MouseRule pending = exact ? *exact : sequence.pending;
    sequence = {hwnd, pt, now, mask, item, count, middle, version, std::move(pending)};
    if (!longer || !SetTimer(hwnd, kClickTimer, GetDoubleClickTime(), nullptr))
        FinishSequence(true);
    return true;
}

LRESULT CALLBACK ViewSubclass(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam,
                                      DWORD_PTR dwRefData) {
    // Teardown on the window's own thread, before the init guard so it still
    // runs while Wh_ModUninit is tearing down (g_initialized is already 0 then).
    // Kills pending timers (must be killed on their creating thread) and
    // releases the per-thread UIAutomation.
    if (g_msgTeardown && uMsg == g_msgTeardown) {
        KillTimer(hWnd, 0x4D43);
        KillTimer(hWnd, 0x4D44);
        KillTimer(hWnd, 0x4D45);
        ReleaseUIAutomationForThread();
        ReleasePendingNavForThread();
        // Drop any not-yet-dispatched actions and pending click state. These
        // thread_local objects outlive the mod on Explorer's long-lived UI
        // threads, so without this they would keep holding their state for the
        // rest of the process's life.
        g_pendingActions.clear();
        FinishSequence(false);
        return 0;
    }

    // Remove from subclass tracking on destroy and release per-thread COM.
    // Handled before the init guard so it runs even during teardown.
    if (uMsg == WM_NCDESTROY) {
        if (sequence.hwnd == hWnd) FinishSequence(false);
        {
            std::lock_guard<std::mutex> lk(g_subclassMutex);
            std::erase_if(g_subclassed, [hWnd](const SubclassEntry& e) { return e.hWnd == hWnd; });
        }
        ReleaseUIAutomationForThread();
        ReleasePendingNavForThread();
        return DefSubclassProc(hWnd, uMsg, wParam, lParam);
    }

    // Deferred action dispatch (posted from mouse handlers to avoid blocking).
    // Handled before the init guard so a posted action that arrives after the
    // mod was disabled is still drained (the per-thread queue below owns it).
    // The dispatch itself is gated on g_initialized.
    if (g_msgDoAction && uMsg == g_msgDoAction) {
        // Drop stale entries whose target window is gone (e.g. the tab was
        // closed before its posted message was dispatched) so they can't be
        // mis-dispatched to a different window on the same thread.
        while (!g_pendingActions.empty() &&
               !IsWindow(g_pendingActions.front().target))
            g_pendingActions.erase(g_pendingActions.begin());
        if (!g_pendingActions.empty() && g_pendingActions.front().target == hWnd) {
            PendingAction a = std::move(g_pendingActions.front());
            g_pendingActions.erase(g_pendingActions.begin());
            // The posted message can be dispatched late if the Explorer UI thread
            // is stuck, so guard the SendInput-based actions the same way as the
            // timer paths — only run while the clicked frame is still foreground.
            if (g_initialized && !a.action.empty() && StillForeground(hWnd))
                FindShellTabAndDoAction(hWnd, a.action.c_str(), a.match.c_str());
        }
        return 0;
    }

    CHECK_INIT_OR_DEFER(hWnd, uMsg, wParam, lParam);

    if (uMsg == WM_TIMER && wParam == 0x4D43) {
        NavigateNewTabProc(hWnd,uMsg,wParam,0);
        return 0;
    }
    if (uMsg == WM_TIMER && wParam == kClickTimer && sequence.hwnd == hWnd) {
        if (GetTickCount() - sequence.time >= GetDoubleClickTime()) FinishSequence(true);
        return 0;
    }
    bool list = dwRefData != 0;
    UINT button = list ? uMsg : (uMsg == WM_PARENTNOTIFY ? LOWORD(wParam) : 0);
    if (button == WM_LBUTTONDOWN || button == WM_LBUTTONDBLCLK ||
        button == WM_MBUTTONDOWN || button == WM_MBUTTONDBLCLK) {
        if (HandleRule(hWnd,button,lParam,list) && list && button != WM_LBUTTONDOWN)
            return 0;
    }
    return DefSubclassProc(hWnd,uMsg,wParam,lParam);
}

// ---- CreateWindowExW hook ----

using CreateWindowExW_t = decltype(&CreateWindowExW);
CreateWindowExW_t CreateWindowExW_original;

HWND WINAPI CreateWindowExW_hook(DWORD dwExStyle, LPCWSTR lpClassName,
                                  LPCWSTR lpWindowName, DWORD dwStyle,
                                  int X, int Y, int nWidth, int nHeight,
                                  HWND hWndParent, HMENU hMenu,
                                  HINSTANCE hInstance, LPVOID lpParam) {
    HWND hWnd = CreateWindowExW_original(dwExStyle, lpClassName, lpWindowName,
                                          dwStyle, X, Y, nWidth, nHeight,
                                          hWndParent, hMenu, hInstance, lpParam);
    if (!hWnd || !g_initialized) return hWnd;

    wchar_t className[256];
    if (!GetClassName(hWnd, className, 256)) return hWnd;
    if (wcscmp(className, L"SysListView32") != 0 &&
        wcscmp(className, L"DirectUIHWND") != 0) return hWnd;

    HWND shellTab = NULL, defView = NULL;
    HWND p = GetParent(hWnd);
    int limit = 16;
    while (p && limit-- > 0) {
        wchar_t pc[256];
        if (!GetClassName(p, pc, 256)) break;
        if (wcscmp(pc, L"SHELLDLL_DefView") == 0) defView = p;
        if (wcscmp(pc, L"ShellTabWindowClass") == 0) { shellTab = p; break; }
        p = GetParent(p);
    }
    if (!shellTab || !defView || !IsWindow(shellTab)) return hWnd;

    if (wcscmp(className, L"SysListView32") == 0) {
        if (WindhawkUtils::SetWindowSubclassFromAnyThread(hWnd, ViewSubclass, 1))
            TrackSubclassedWindow(hWnd, true);
    } else {
        if (IsWindow(defView) &&
            WindhawkUtils::SetWindowSubclassFromAnyThread(defView, ViewSubclass, 0))
            TrackSubclassedWindow(defView, false);
    }
    return hWnd;
}

// ---- FileCabinet_CreateViewWindow2 hook ----

typedef HRESULT (*__cdecl FileCabinet_CreateViewWindow2_t)(
    IShellBrowser*, void*, IShellView*, IShellView*, void*, HWND*);
FileCabinet_CreateViewWindow2_t FileCabinet_CreateViewWindow2Original;
// Loaded in Wh_ModInit to install the symbol hook, kept loaded for the hook's
// lifetime, freed in Wh_ModUninit to balance the reference.
static HMODULE g_hExplorerFrame = nullptr;

HRESULT __cdecl FileCabinet_CreateViewWindow2Hook(
    IShellBrowser* pBrowser, void* var1, IShellView* psv1,
    IShellView* psv2, void* var2, HWND* hWnd) {
    HRESULT hRes = FileCabinet_CreateViewWindow2Original(pBrowser, var1, psv1, psv2, var2, hWnd);
    if (!g_initialized || FAILED(hRes) || !hWnd || !*hWnd) return hRes;

    HWND shellTab = GetParent(*hWnd);
    if (shellTab && IsWindow(shellTab)) {
        if (g_pendingNavPath[0] && !g_pendingNavBrowser && pBrowser) {
            g_pendingNavBrowser = pBrowser;
            g_pendingNavBrowser->AddRef();
        }
    }
    return hRes;
}

// ---- Enumeration for already-open Explorer windows ----

BOOL CALLBACK InitEnumChildWindowsProc(HWND hWnd, LPARAM lParam) {
    DWORD pid = 0;
    GetWindowThreadProcessId(hWnd, &pid);
    if (pid == GetCurrentProcessId()) {
        wchar_t className[256];
        if (!GetClassName(hWnd, className, 256)) return TRUE;
        if (wcscmp(className, L"SHELLDLL_DefView") == 0) {
            HWND lv = FindWindowEx(hWnd, NULL, L"SysListView32", NULL);
            HWND dui = FindWindowEx(hWnd, NULL, L"DirectUIHWND", NULL);
            if (lv) {
                if (WindhawkUtils::SetWindowSubclassFromAnyThread(lv, ViewSubclass, 1)) {
                    Wh_Log(L"SysListView32 Subclassed %p", lv);
                    TrackSubclassedWindow(lv, true);
                }
            } else if (dui) {
                if (WindhawkUtils::SetWindowSubclassFromAnyThread(hWnd, ViewSubclass, 0)) {
                    Wh_Log(L"DirectUIHWND Subclassed %p", hWnd);
                    TrackSubclassedWindow(hWnd, false);
                }
            }
            return FALSE;
        }
    }
    return TRUE;
}

BOOL CALLBACK InitEnumWindowsProc(HWND hWnd, LPARAM lParam) {
    DWORD pid = 0;
    GetWindowThreadProcessId(hWnd, &pid);
    if (pid == GetCurrentProcessId()) {
        wchar_t className[256];
        if (!GetClassName(hWnd, className, 256)) return TRUE;
        if (wcscmp(className, L"CabinetWClass") == 0) {
            for (HWND shellTab = FindWindowEx(hWnd, NULL, L"ShellTabWindowClass", NULL);
                 shellTab;
                 shellTab = FindWindowEx(hWnd, shellTab, L"ShellTabWindowClass", NULL)) {
                EnumChildWindows(shellTab, InitEnumChildWindowsProc, (LPARAM)shellTab);
            }
        }
    }
    return TRUE;
}

// ---- Windhawk lifecycle ----

BOOL Wh_ModInit() {
    Wh_Log(L"Click on Empty Explorer Init");

    g_msgDoAction = RegisterWindowMessage(L"ClickOnEmptyExplorer_DoAction_" WH_MOD_ID);
    g_msgTeardown = RegisterWindowMessage(L"ClickOnEmptyExplorer_Teardown_" WH_MOD_ID);
    LoadSettings();

    g_hExplorerFrame = LoadLibraryExW(L"explorerframe.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!g_hExplorerFrame) {
        Wh_Log(L"Failed to load explorerframe.dll");
        return FALSE;
    }

    WindhawkUtils::SYMBOL_HOOK explorerframe_dll_hooks[] = {
        {{
            L"long __cdecl FileCabinet_CreateViewWindow2(struct IShellBrowser *,struct tagFolderSetDataBase *,struct IShellView *,struct IShellView *,struct tagRECT *,struct HWND__ * *)"
        },
        (void**)&FileCabinet_CreateViewWindow2Original,
        (void*)FileCabinet_CreateViewWindow2Hook,
        FALSE}
    };
    if (!WindhawkUtils::HookSymbols(g_hExplorerFrame, explorerframe_dll_hooks, ARRAYSIZE(explorerframe_dll_hooks))) {
        // FileCabinet_CreateViewWindow2 is only used to capture the new tab's
        // IShellBrowser for the Duplicate Tab action. If the symbol can't be
        // resolved (new Windows build, symbol server down), don't disable the
        // whole mod — only Duplicate Tab should degrade. The hook simply isn't
        // installed, so FileCabinet_CreateViewWindow2Original stays null and is
        // never called.
        Wh_Log(L"Failed to hook ExplorerFrame.dll — Duplicate Tab will be unavailable, other actions still work");
        FreeLibrary(g_hExplorerFrame);
        g_hExplorerFrame = nullptr;
    }
    // On success, keep explorerframe.dll loaded so the installed hook stays valid;
    // the reference is released in Wh_ModUninit.

    WindhawkUtils::SetFunctionHook(CreateWindowExW, CreateWindowExW_hook, &CreateWindowExW_original);

    g_initialized.store(true);
    return TRUE;
}

void Wh_ModAfterInit() {
    EnumWindows(InitEnumWindowsProc, 0);
}

void Wh_ModSettingsChanged() {
    LoadSettings();
}

void Wh_ModUninit() {
    // Block subclass callbacks and hook code before cleanup
    g_initialized.store(false);

    // g_pendingNav* are thread_local and owned by the Explorer UI threads; they
    // are released on those threads via the g_msgTeardown / WM_NCDESTROY paths
    // below, not here (this runs on Windhawk's arbitrary thread).

    // Collect subclassed HWNDs under lock, then teardown + remove subclasses
    // outside the lock. Timers use nullptr callback now (WM_TIMER handled in
    // the subclass proc), so no mod-image callback can fire after unload.
    // Send the teardown message so KillTimer + per-thread COM release happen on
    // the window's own thread (a timer must be killed on its creating thread,
    // and the UIAutomation COM object has thread affinity).
    std::vector<SubclassEntry> toRemove;
    {
        std::lock_guard<std::mutex> lk(g_subclassMutex);
        std::swap(toRemove, g_subclassed);
    }
    for (auto& e : toRemove) {
        if (e.hWnd && IsWindow(e.hWnd)) {
            if (g_msgTeardown) SendMessage(e.hWnd, g_msgTeardown, 0, 0);
            WindhawkUtils::RemoveWindowSubclassFromAnyThread(
                e.hWnd, ViewSubclass);
        }
    }

    // Balance the LoadLibraryExW reference taken in Wh_ModInit. explorerframe.dll
    // stays loaded regardless (Explorer itself holds a reference), so this only
    // releases the mod's own reference — the hooks are gone by now.
    if (g_hExplorerFrame) {
        FreeLibrary(g_hExplorerFrame);
        g_hExplorerFrame = nullptr;
    }
}
