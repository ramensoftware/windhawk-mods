// ==WindhawkMod==
// @id              explorer-ctrlfn-newfolder
// @name            Explorer Ctrl+F+N to New Folder
// @description     Ctrl+F+N makes a new folder and names it - inline rename (native) or a themed light/dark popup box, one setting. Batch: <name> 1..N.
// @version         0.10.0
// @author          Ashix
// @github          https://github.com/k-ashix
// @twitter         https://x.com/k_ashix
// @include         windhawk.exe
// @compilerOptions -lole32 -loleaut32 -lshlwapi -lshell32 -luser32 -luuid -luiautomationcore -lgdi32 -ladvapi32
// ==/WindhawkMod==

// For bug reports and feature requests, please open an issue here:
// https://github.com/ramensoftware/windhawk-mods/issues

// ==WindhawkModReadme==
/*
# 📁 Explorer Ctrl+F+N -> New Folder

Make a new folder or batch of folders in File Explorer (including sub-folder & Desktop)
(*This PC* / *Quick access* and other non-filesystem spots are skipped.)

## **Why "Ctrl + F + N"?** **F** for *Folder*, **N** for *Name*.

> **Good to know first:** only **one** shortcut is active at a time - firing any
> other chord does nothing. Hold Ctrl for the whole gesture 
> **Esc** cancels and creates nothing.

## ⌨️ How to use
1. Open File Explorer (or click an empty Desktop spot).
2. **Hold Ctrl** the whole time -> tap **F** -> tap **N**. Type a name, press **Enter**.
3. **Hold Ctrl** the whole time -> tap **F** -> tap **N** -> tap **any digit (3, 8, 12) to create folders in batch**.
4. **Hold Ctrl** the whole time -> tap **F** -> tap **N** -> tap **you template key to create folders mentioned in you template**.

> 🔒 **Ctrl must stay down for the entire shortcut.** Let go and the gesture is
> abandoned - nothing is created.

**Esc** keeps the default name (`New folder`). Clicking away commits what you
typed so far (like Explorer's own rename).

## ⚙️ Settings

| Setting | Default | What it does |
|---|---|---|
| **Name folders with a popup box** | OFF (inline) | **OFF** - folder opens in Explorer's inline rename box; type + Enter. Feels native. **ON** - a themed light/dark popup box; type + Create. More reliable, same in every folder. |
| **Multiple folders in one go** | ON | ON (default): type a number right after the shortcut, then Enter (e.g. `5` -> five folders). No number = one; Esc cancels. **Folder templates fire regardless of this setting.** OFF: always one folder. |
| **Max folders per run** | 25 | Cap per trigger. Range 1-100. |
| **Auto-resolve name clashes** | ON | ON: numbers past existing folders (`New folder (2)`, `(3)`...) - a folder **every** time. OFF: only the plain `New folder`, so once it exists the shortcut does nothing. If it "only works once", turn this ON. |
| **Open the new folder after creating it** | OFF | ON: browse into the new folder in the **same** window. Single-folder only|
| **Type speed** | Normal | How long the box waits before it commits/cancels on its own. **Normal** ≈ 1.2s count / 30s name, **Relaxed** ≈ 2.5s / 60s, **Patient** ≈ 4s / 120s. Pick slower if the box closes too soon. |
| **Batch numbering pattern** | `Project 1` | Number style for a named batch. All skip existing folders. |
| **Folder templates** | *(empty)* | Your own fixed folder sets. Visit `Template Creation`. **Name is required** - a blank name is ignored.|
| **Template name collisions** | Skip | For templates: `Skip` existing folders, `number` a sibling, or `stop` (create nothing). Overrides *Auto-resolve name clashes*. |

**Naming:** one folder becomes `<name>`; several become a numbered batch whose
style you pick with **Batch numbering pattern** (example base `New folder`):

| Pattern | Result for base `New folder` |
|---|---|
| `New folder 1` (default) | `New folder 1`, `New folder 2`, `New folder 3` |
| `New folder-01` | `New folder-01`, `New folder-02`, `New folder-03` |
| `New folder-001` | `New folder-001`, `New folder-002`, `New folder-003` |
| `New folder_01` | `New folder_01`, `New folder_02`, `New folder_03` |
| `New folder_001` | `New folder_001`, `New folder_002`, `New folder_003` |
| `New folder (01)` | `New folder (01)`, `New folder (02)`, `New folder (03)` |

**Smart numbering (never overwrites):** existing folders are always skipped. If
`New folder-001`, `New folder-002`, `New folder-003` already exist and you make
3 more, you get `New folder-004`, `New folder-005`, `New folder-006` - the batch
continues past the highest run instead of clobbering. A blank name falls back to
`New folder` / `New folder (2)`.

## 📁 Folder templates

Make the **same set of folders every time** just with few keypress.

**Use it:** press your trigger (`Ctrl -> F -> N`), then the template key - no
typing, no rename box. (Digits are used to create multiple folders in batch with same name while letters pick
templates where you pre-define you folder names.)

### Template examples

| # | **One template per line `key=Name=Folder1\|Folder2\|Folder3`** | Press | Will Create |
|---|---|---|---|
| 1 | `i=Image=raw\|jpg\|png` | `i` | `raw`, `jpg`, `png` |
| 2 | `w=Work=src\|docs\|assets` | `w` | `src`, `docs`, `assets` |
| 3 | `p=Project=01 Brief\|02 Assets\|03 Final` | `p` | `01 Brief`, `02 Assets`, `03 Final` |

### Templates Creation

| # | Part / rule | What it means | Example |
|---|---|---|---|
| ✏️ | **key** | The letter you press after the trigger. One letter A-Z, **case-insensitive** (`i` = `I`, stored lowercase). Separate from the name. | `i` |
| ✏️ | **Name** | Assign a label, anything you like. Only for your reference. | `Images` |
| ✏️ | **Folders** | Everything after the 2nd `=`, split on the `\|` pipe. Each piece is one folder. | `raw\|jpg\|png` |
| ✅ | **Spaces are fine** | Spaces **inside or around** a name are kept exactly - `01 Explain & Brief`. Names are **not** trimmed,| `01 Explain & Brief` |
| 📏 | **26 templates max** | One per letter A-Z. | - |
| 📏 | **100 folders max** | Per template; each list is independent. | - |
| 📏 | **One template per key** | Same key twice = the **first** line wins, later ones ignored. Use a unique key per line. | - |
| 🚫 | **Bad characters** | A name containing `\` `/` `:` `*` `?` `"` `<` `>` `\|` is skipped (same set a typed name strips). So is `.`, `..`, or any name starting with `..`. | `a\b`, `../x`, `C:\x` |
| 🚫 | **Empty / all-spaces / control chars** | Blank pieces, spaces-only names, and control characters (below ASCII 0x20) are skipped. | (empty) |
| 🚫 | **Duplicate in one line** | The same name twice in one template is kept only once, **case-insensitively** (Windows folders). | `raw\|RAW` -> `raw` |
| 🔒 | **Never overwrites** | Existing folders follow *Template name collisions* (skip / number / stop). | - |

> Note: the `,` comma is **allowed** in a folder name - only `\` `/` `:` `*`
> `?` `"` `<` `>` `|` `..` and control characters are rejected.

## 🆚 How this is different

### Ctrl+F+N vs. the other shortcut options
All four options create and name folders the same way - the difference is which
keys trigger it and what native Explorer behavior they take over.

| | **Ctrl+F+N** *(default)* | **Ctrl+N** | **Ctrl+N+F** | **Ctrl+Shift+N** |
|---|---|---|---|---|
| Keys | Hold Ctrl, tap F, then N | Single press | Hold Ctrl, tap N, then F | Single press |
| Native feature it uses | Ctrl+F (search) | New window | New window | New folder |
| Keeps native Explorer shortcut? | ✅ Ctrl+F search still opens if you don't press N | ❌ replaces New window | ❌ replaces New window | ➖ overlays the native New folder |
| Feels most native? | Adds an extra key | No | No | ✅ same keys Windows uses |
| Accidental-trigger risk | Low (needs the F+N chord) | Higher (single press) | Medium | Low |
| Naming (inline or popup) | ✅ | ✅ | ✅ | ✅ |
| Batch creation | ✅ | ✅ | ✅ | ✅ |

**In short:** pick **Ctrl+F+N** to keep every native Explorer shortcut intact,
or **Ctrl+Shift+N** if you want the exact keys Windows already uses for New
folder. **Ctrl+N** / **Ctrl+N+F** are for people who don't mind giving up the
"New window" shortcut.

### vs. Windows' built-in folder creation & other mods

**Short version:** this mod is the only one that creates a folder, **names it in
the same motion**, makes a **batch** at once, and lets you **pick the shortcut**.
The two forks below are what it grew out of; Windows' own options are shown for
reference.

Legend: ✅ yes · ❌ no · ➖ partial / not applicable.

| Feature | **This mod** | Win right-click | Win Ctrl+Shift+N | *Ctrl+N -> New File* fork | *Ctrl+Q -> New Folder* fork |
|---|---|---|---|---|---|
| Creates a folder | ✅ | ✅ | ✅ | ❌ (empty file) | ✅ |
| Keyboard-only (no mouse) | ✅ | ❌ | ✅ | ✅ | ✅ |
| Names it in the same motion | ✅ inline **or** popup | ❌ | ❌ | ➖ inline rename | ❌ unnamed |
| Batch: several at once | ✅ up to **100** | ❌ | ❌ | ❌ | ❌ |
| Batch numbering styles | ✅ **6** (`1` `-01` `-001` `_01` `_001` `(01)`) | ❌ | ❌ | ❌ | ❌ |
| Name-clash handling | ✅ number **or** skip | ➖ always numbers | ➖ always numbers | ➖ | ➖ |
| Folder templates (fixed sets) | ✅ up to 26 keys | ❌ | ❌ | ❌ | ❌ |
| Choice of shortcut | ✅ **4** options | ❌ fixed | ❌ fixed | ❌ Ctrl+N only | ❌ Ctrl+Q only |
| Keeps native Explorer shortcuts | ✅ (Ctrl+F+N default) | ✅ | ✅ | ❌ takes over New window | ➖ uses free Ctrl+Q |
| Light/dark themed popup | ✅ | ➖ | ➖ | ❌ | ❌ |
| Race-condition stability fixes | ✅ | ✅ | ✅ | ➖ | ➖ |

**Bottom line:** the built-in options can't name or batch; the *Ctrl+N -> New
File* fork makes blank files and hijacks "New window"; the *Ctrl+Q* fork makes
one unnamed folder on a fixed key. This mod adds naming (inline or themed popup),
batch creation, templates, clash handling, and a choice of shortcut - with the
Shell-notify + thread-sync work that keeps the folder fully created before the
rename fires.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- shortcut: "Ctrl+F+N"
  $name: Shortcut
  $description: >-
    Pick one of the four supported shortcuts. Only one is active at a time.
  $options:
  - "Ctrl+F+N": Ctrl+F+N (default) - keeps every native Explorer shortcut
  - "Ctrl+Shift+N": Ctrl+Shift+N - the exact keys Windows uses for New folder
  - "Ctrl+N": Ctrl+N - single press (replaces "New window")
  - "Ctrl+N+F": Ctrl+N+F - hold Ctrl, tap N then F (replaces "New window")
- multiFolder: true
  $name: Multiple folders in one go
  $description: >-
    On (default): type a number right after the trigger, must rename then Enter, to make
    that many at once (no number = one; Esc cancels). Folder templates fire
    regardless of this setting (press a template key after the trigger). Off:
    the count gesture always makes one folder.
- maxFolders: 25
  $name: Max folders per run
  $description: >-
    Cap per trigger. Range 1-100 (larger values are clamped to 100).
- autoResolveNames: true
  $name: Auto-resolve name clashes
  $description: >-
    On: number a taken name automatically (New folder (2), (3), ...). Off: use
    only "New folder" and skip a clash instead of numbering.
- useDialog: false
  $name: Name folders with a popup box
  $description: >-
    Off: type into Explorer's inline rename box (feels native). On: a popup box - type the name, press Create (more reliable).
- openAfterCreate: false
  $name: Open the new folder after creating it
  $description: >-
    Off (default): stay where you are. On: open the new folder in the SAME
    window (no new window). Single-folder only - never for a batch.
- typeSpeed: normal
  $name: Type speed
  $description: >-
    Grace period before the box gives up and commits/cancels on its own. It
    scales two windows: the batch-count entry (right after the shortcut) and
    the folder-name rename. The Ctrl+F->N key gap is fixed and not affected.
    Pick a slower preset if the box closes before you finish typing.
  $options:
  - normal: Normal (default) - about 1.2s to type the count, 30s to name
  - relaxed: Relaxed - about twice as long (about 2.5s count, 60s name)
  - patient: Patient - the most time (about 4s count, 120s name)
- namingPattern: space
  $name: Batch numbering pattern
  $description: >-
    How the number is written when you make several folders at once (example
    base "New folder"). Existing folders are always skipped, so a batch never
    overwrites and still makes the count you asked for.
  $options:
  - space: "New folder 1, New folder 2 (default)"
  - dash2: "New folder-01, New folder-02"
  - dash3: "New folder-001, New folder-002"
  - paren2: "New folder (01), New folder (02)"
  - under2: "New folder_01, New folder_02"
  - under3: "New folder_001, New folder_002"
- folderTemplates:
  - ""
  $name: Folder templates
  $description: >-
    One template per line using key=Name=Folder 1|Folder 2|Folder 3. You pick
    the key (a single letter A-Z), separate from the name. Check Templates in Readme
- templateCollision: skip
  $name: Template name collisions
  $description: >-
    What to do when a folder in the template already exists in the target directory.
  $options:
  - skip: Skip the ones that exist, create the rest
  - number: Make a numbered sibling (img, img 2, ...) using the batch pattern
  - stop: Create nothing and cancel (safest)
*/
// ==/WindhawkModSettings==

#include <sdkddkver.h>

#include <windows.h>
#include <string>
#include <cwctype>
#include <vector>
#include <memory>
#include <new>
#include <shlwapi.h>
#include <shlobj.h>
#include <shobjidl.h>
#include <shellapi.h>
#include <exdisp.h>
#include <oleauto.h>
#include <KnownFolders.h>
#include <ShlGuid.h>
#include <UIAutomation.h>

// Libraries are linked via @compilerOptions at the top of this file. Windhawk
// compiles with clang, so MSVC-style link pragmas are intentionally omitted.

extern "C" IMAGE_DOS_HEADER __ImageBase;
#define HINST_THISCOMPONENT ((HINSTANCE)&__ImageBase)

#ifndef FOFX_SHOWELEVATIONPROMPT
#define FOFX_SHOWELEVATIONPROMPT 0x00040000
#endif

#ifndef FOFX_NOCOPYHOOKS
#define FOFX_NOCOPYHOOKS 0x00800000
#endif

template <typename T>
static void SafeRelease(T*& p) {
    if (p) {
        p->Release();
        p = nullptr;
    }
}

static IUIAutomation* g_uia = nullptr;
static volatile LONG g_actionRunning = 0;

// User-configurable shortcut. The final key fires while every preceding key
// remains held. Defaults to Ctrl+F+N when the setting is invalid.
struct ShortcutConfig {
    int triggerVk;
    int heldVks[4];
    int heldCount;
};
static ShortcutConfig g_shortcut = {'N', {VK_CONTROL, 'F', 0, 0}, 2};

// #3: the SAME shortcut packed into ONE 64-bit word (see PackShortcut). The LL
// keyboard hook reads THIS via a single atomic load instead of the multi-field
// g_shortcut struct, which a settings-thread write could tear. Default Ctrl+F+N:
// triggerVk 'N' | heldCount 2 | heldVks[0]=Ctrl | heldVks[1]='F'.
static volatile LONG64 g_shortcutPacked =
    (LONG64)'N' | ((LONG64)2 << 8) |
    ((LONG64)VK_CONTROL << 16) | ((LONG64)'F' << 24);

// Multi-folder creation (v0.2.0). Default cap 25; the hard cap 100 can never be
// exceeded even if the user sets maxFolders higher.
static const int kFolderDefaultMax = 25;
static const int kFolderHardCap = 100;
static volatile LONG g_multiFolder = 0;                 // toggle (default off)
static volatile LONG g_hasTemplates = 0;                // 1 if >=1 template loaded (hook reads lock-free)
static volatile LONG g_templateKeyMask = 0;             // #1: bit (c-'a') set == letter 'c' is a bound template key (hook reads lock-free)
static volatile LONG g_maxFolders = kFolderDefaultMax;  // raw maxFolders setting
static volatile LONG g_autoResolveNames = 1;            // number clashes (default on)
static volatile LONG g_useDialog = 0;                   // 0 = inline rename, 1 = popup box
static volatile LONG g_openAfterCreate = 0;             // 0 = stay, 1 = open the new folder (single create only)
static volatile LONG g_typeSpeed = 0;                   // 0=Normal, 1=Relaxed, 2=Patient (scales count + rename windows)
static volatile LONG g_namingPattern = 0;              // 0=Space "N", 1=Dash2 "-0N", 2=Dash3 "-00N", 3=Paren2 " (0N)"

// #5: settings are written from Windhawk's settings-change callback and READ on
// the hook and worker threads. Writes already use InterlockedExchange; a bare
// `g_x` load, however, is an ordinary non-atomic read and is not a defined
// synchronisation with those writes. AtomicGet performs an interlocked (atomic,
// full-barrier) load so every read is paired with the interlocked writes and
// the settings are read consistently across threads. Used for EVERY read of the
// settings globals below -- no raw `g_multiFolder`/`g_maxFolders`/etc. loads.
static inline LONG AtomicGet(volatile LONG* p) {
    return InterlockedCompareExchange(p, 0, 0);
}

// #7: set on unload so a long-running poll (the inline rename watch) can bail
// promptly instead of blocking teardown for up to its full timeout. Read via
// AtomicGet, written via InterlockedExchange.
static volatile LONG g_unloading = 0;

// Set to 1 while a folder action is queued/in flight so only one runs at a
// time. Declared here (before the popup pump that clears it on a stale
// WM_APP+1) and used by the worker post/drain paths. Interlocked-only.
static volatile LONG g_actionPosted = 0;

// #6: deterministic Esc during inline rename. Polling GetAsyncKeyState in a
// 60 ms loop can miss a fast Esc press+release that happens entirely between
// two samples, which then gets misclassified as a focus-lost commit. The
// low-level keyboard hook (which sees EVERY keydown) LATCHES this flag the
// instant Esc is pressed while a rename watch is active, so the watcher can
// observe the cancel even if it never coincides with a poll. Both written via
// Interlocked; read via AtomicGet.
static volatile LONG g_inlineRenameActive = 0;  // a rename watch is running
static volatile LONG g_inlineEscPressed = 0;    // Esc seen during that watch

// ---- Folder Templates: shared types + load-time state ----
// Declared here (ABOVE LoadSettings) because LoadSettings reads the configured
// templates into g_templates at load time. The pure-logic functions
// (NormalizeTemplateKey, CleanTemplateFolders, ResolveTemplateKeys,
// ResolveTemplateFolders) are defined lower down -- ResolveTemplateFolders
// depends on FormatBatchName, which is also defined below.
static const int kMaxTemplates = 26;
static const int kMaxFoldersPerTemplate = 100;

enum TemplateCollision { TC_Skip = 0, TC_Number = 1, TC_Stop = 2 };

struct FolderTemplate {
    std::wstring key;                    // user-assigned single letter
    std::wstring name;
    std::vector<std::wstring> folders;
};

struct ResolvedTemplate {
    wchar_t key = 0;                     // user-assigned key, lowercased
    std::wstring name;
    std::vector<std::wstring> folders;   // validated, deduped, capped
};

// Load-time template state, read later by the hook/worker threads. A
// std::vector cannot be updated with InterlockedExchange, so g_templatesCs
// guards BOTH g_templates and g_templateCollision. The CS is created in
// WhTool_ModInit before anything can call LoadSettings and destroyed in
// WhTool_ModUninit after the hook/worker have stopped.
static std::vector<ResolvedTemplate> g_templates;
static LONG g_templateCollision = TC_Skip;
static CRITICAL_SECTION g_templatesCs;

// Defined lower down (after CleanTemplateFolders); forward-declared so
// LoadSettings can resolve templates at load time.
static std::vector<ResolvedTemplate> ResolveTemplateKeys(
        const std::vector<FolderTemplate>& raw,
        std::vector<std::wstring>* ignored);

static int ShortcutTokenToVk(std::wstring token) {
    for (wchar_t& ch : token) ch = static_cast<wchar_t>(towupper(ch));
    if (token == L"CTRL" || token == L"CONTROL") return VK_CONTROL;
    if (token == L"ALT") return VK_MENU;
    if (token == L"SHIFT") return VK_SHIFT;
    if (token == L"WIN" || token == L"WINDOWS") return VK_LWIN;
    if (token == L"ENTER") return VK_RETURN;
    if (token == L"SPACE") return VK_SPACE;
    if (token == L"TAB") return VK_TAB;
    if (token == L"ESC" || token == L"ESCAPE") return VK_ESCAPE;
    if (token == L"BACKSPACE") return VK_BACK;
    if (token == L"DELETE" || token == L"DEL") return VK_DELETE;
    if (token == L"INSERT" || token == L"INS") return VK_INSERT;
    if (token == L"HOME") return VK_HOME;
    if (token == L"END") return VK_END;
    if (token == L"PAGEUP" || token == L"PGUP") return VK_PRIOR;
    if (token == L"PAGEDOWN" || token == L"PGDN") return VK_NEXT;
    if (token == L"LEFT") return VK_LEFT;
    if (token == L"RIGHT") return VK_RIGHT;
    if (token == L"UP") return VK_UP;
    if (token == L"DOWN") return VK_DOWN;
    if (token.size() == 1 && ((token[0] >= L'A' && token[0] <= L'Z') ||
                              (token[0] >= L'0' && token[0] <= L'9'))) {
        return token[0];
    }
    if (token.size() >= 2 && token[0] == L'F') {
        int n = _wtoi(token.c_str() + 1);
        if (n >= 1 && n <= 24) return VK_F1 + n - 1;
    }
    return 0;
}

static bool ParseShortcut(PCWSTR text, ShortcutConfig* result) {
    if (!text || !result) return false;
    std::vector<int> keys;
    std::wstring input(text);
    size_t start = 0;
    while (start <= input.size()) {
        size_t end = input.find(L'+', start);
        std::wstring token = input.substr(start, end - start);
        size_t first = token.find_first_not_of(L" \t");
        size_t last = token.find_last_not_of(L" \t");
        if (first == std::wstring::npos) return false;
        token = token.substr(first, last - first + 1);
        int vk = ShortcutTokenToVk(token);
        if (!vk || keys.size() >= 5) return false;
        keys.push_back(vk);
        if (end == std::wstring::npos) break;
        start = end + 1;
    }
    if (keys.size() < 2) return false;
    ShortcutConfig parsed = {};
    parsed.triggerVk = keys.back();
    parsed.heldCount = static_cast<int>(keys.size() - 1);
    for (int i = 0; i < parsed.heldCount; ++i) {
        if (keys[i] == parsed.triggerVk) return false;
        parsed.heldVks[i] = keys[i];
    }
    *result = parsed;
    return true;
}

// #3: pack / unpack ShortcutConfig <-> one 64-bit word (mirror of
// tests/newfolder_logic.h PackShortcut/UnpackShortcut). The hook reads the
// shortcut with a single atomic load, never a torn multi-field struct read.
static LONG64 PackShortcut(const ShortcutConfig& s) {
    int heldCount = s.heldCount;
    if (heldCount < 0) heldCount = 0;
    if (heldCount > 4) heldCount = 4;
    unsigned long long p = 0;
    p |= (unsigned long long)(s.triggerVk & 0xFF);
    p |= (unsigned long long)(heldCount & 0x7) << 8;
    for (int i = 0; i < 4; ++i) {
        int vk = (i < heldCount) ? (s.heldVks[i] & 0xFF) : 0;
        p |= (unsigned long long)vk << (16 + i * 8);
    }
    return (LONG64)p;
}
static ShortcutConfig UnpackShortcut(LONG64 packed) {
    unsigned long long p = (unsigned long long)packed;
    ShortcutConfig s = {};
    s.triggerVk = (int)(p & 0xFF);
    s.heldCount = (int)((p >> 8) & 0x7);
    if (s.heldCount > 4) s.heldCount = 4;
    for (int i = 0; i < 4; ++i) {
        s.heldVks[i] = (int)((p >> (16 + i * 8)) & 0xFF);
    }
    return s;
}
// Read the published shortcut snapshot with a single interlocked load.
static ShortcutConfig CurrentShortcut() {
    return UnpackShortcut(InterlockedCompareExchange64(&g_shortcutPacked, 0, 0));
}

// #4: keys the F2-rename fallback must see released before it injects F2:
// the three standard modifiers PLUS every key in the configured shortcut,
// deduped (mirror of tests/newfolder_logic.h ShortcutReleaseWaitVks). Waiting
// for a fixed Ctrl/Alt/Shift/N/F set was wrong once the shortcut became
// configurable (e.g. Alt+F2 never waited for the F2 trigger).
static std::vector<int> ShortcutReleaseWaitVks(const ShortcutConfig& s) {
    std::vector<int> out = { VK_CONTROL, VK_MENU, VK_SHIFT };
    auto add = [&out](int vk) {
        if (vk == 0) return;
        for (int e : out) if (e == vk) return;
        out.push_back(vk);
    };
    int heldCount = s.heldCount;
    if (heldCount > 4) heldCount = 4;
    for (int i = 0; i < heldCount; ++i) add(s.heldVks[i]);
    add(s.triggerVk);
    return out;
}

// #8-2: is `vk` a modifier that can be HELD to keep a gesture alive
// (Ctrl / Shift / Alt / Win, incl. L/R variants)? Mirror of
// tests/newfolder_logic.h IsHoldModifierVk.
static bool IsHoldModifierVk(int vk) {
    return vk == VK_CONTROL || vk == VK_LCONTROL || vk == VK_RCONTROL ||
           vk == VK_SHIFT   || vk == VK_LSHIFT   || vk == VK_RSHIFT   ||
           vk == VK_MENU    || vk == VK_LMENU    || vk == VK_RMENU    ||
           vk == VK_LWIN    || vk == VK_RWIN;
}

// #8-2: the count/template capture window stays alive while the CONFIGURED
// shortcut's hold-modifier(s) are still physically down -- Ctrl for Ctrl+F+N,
// Alt for Alt+F2, Win for Win+N. The old code hard-coded VK_CONTROL, so a
// non-Ctrl shortcut committed the default folder on the first follow-up key
// (batch counts and template letters were unreachable).
// #8-3: for a MULTI-modifier shortcut (e.g. Ctrl+Shift+N) EVERY configured
// hold-modifier must remain down -- the gesture spec is "all preceding keys
// held", so releasing EITHER Ctrl OR Shift ends the window. (An OR check here
// was a bug: it kept the window alive when only one of the two was still down.)
// If the shortcut holds no modifier, there is nothing to release, so the window
// is treated as held for its duration (it ends via Enter / Escape / timeout).
// Mirror of tests/newfolder_logic.h IsGestureModifierHeld.
static bool IsGestureModifierHeldNow(const ShortcutConfig& s) {
    int heldCount = s.heldCount;
    if (heldCount > 4) heldCount = 4;
    bool anyModifier = false;
    for (int i = 0; i < heldCount; ++i) {
        int vk = s.heldVks[i];
        if (!IsHoldModifierVk(vk)) {
            continue;
        }
        anyModifier = true;
        if ((GetAsyncKeyState(vk) & 0x8000) == 0) {
            return false;   // a configured hold-modifier was released -> end
        }
    }
    // All configured hold-modifiers still down (or none configured -> not
    // modifier-gated, treat as held).
    (void)anyModifier;
    return true;
}

// #1: is `typed` bound to a template? (bit (c-'a') of the published key mask).
// The LL hook calls this with an atomic snapshot of g_templateKeyMask, so it
// never enumerates the g_templates vector on the hook thread.
static bool IsTemplateKeyBound(unsigned long mask, wchar_t typed) {
    wchar_t c = typed;
    if (c >= L'A' && c <= L'Z') c = (wchar_t)(c - L'A' + L'a');
    if (c < L'a' || c > L'z') return false;
    return (mask & (1UL << (c - L'a'))) != 0;
}

static void LoadSettings() {
    ShortcutConfig shortcut = {'N', {VK_CONTROL, 'F', 0, 0}, 2};
    PCWSTR shortcutText = Wh_GetStringSetting(L"shortcut");
    std::wstring shortcutLabel = (shortcutText && shortcutText[0]) ? shortcutText
                                                                   : L"Ctrl+F+N";
    if (!ParseShortcut(shortcutText, &shortcut)) {
        Wh_Log(L"Invalid shortcut '%ls'; using Ctrl+F+N", shortcutText ? shortcutText : L"");
        shortcutLabel = L"Ctrl+F+N";
    }
    Wh_FreeStringSetting(shortcutText);
    g_shortcut = shortcut;
    // #3: publish the packed snapshot the LL keyboard hook reads atomically.
    InterlockedExchange64(&g_shortcutPacked, PackShortcut(shortcut));

    InterlockedExchange(&g_multiFolder,
                        Wh_GetIntSetting(L"multiFolder") != 0 ? 1 : 0);
    InterlockedExchange(&g_maxFolders,
                        static_cast<LONG>(Wh_GetIntSetting(L"maxFolders")));
    InterlockedExchange(&g_autoResolveNames,
                        Wh_GetIntSetting(L"autoResolveNames") != 0 ? 1 : 0);
    InterlockedExchange(&g_useDialog,
                        Wh_GetIntSetting(L"useDialog") != 0 ? 1 : 0);
    InterlockedExchange(&g_openAfterCreate,
                        Wh_GetIntSetting(L"openAfterCreate") != 0 ? 1 : 0);

    // Type-speed preset (0=Normal, 1=Relaxed, 2=Patient). Read as a string
    // option like `trigger`; any unknown value maps to Normal.
    PCWSTR typeSpeed = Wh_GetStringSetting(L"typeSpeed");
    LONG speed = 0;  // normal
    if (typeSpeed) {
        if (wcscmp(typeSpeed, L"relaxed") == 0) {
            speed = 1;
        } else if (wcscmp(typeSpeed, L"patient") == 0) {
            speed = 2;
        } else {
            speed = 0;
        }
        Wh_FreeStringSetting(typeSpeed);
    }
    InterlockedExchange(&g_typeSpeed, speed);

    // Batch numbering pattern (0=Space, 1=Dash2, 2=Dash3, 3=Paren2). Read as a
    // string option like `trigger`; any unknown value maps to Space (default).
    PCWSTR namingPattern = Wh_GetStringSetting(L"namingPattern");
    LONG pattern = 0;  // space
    if (namingPattern) {
        if (wcscmp(namingPattern, L"dash2") == 0) {
            pattern = 1;
        } else if (wcscmp(namingPattern, L"dash3") == 0) {
            pattern = 2;
        } else if (wcscmp(namingPattern, L"paren2") == 0) {
            pattern = 3;
        } else if (wcscmp(namingPattern, L"under2") == 0) {
            pattern = 4;
        } else if (wcscmp(namingPattern, L"under3") == 0) {
            pattern = 5;
        } else {
            pattern = 0;
        }
        Wh_FreeStringSetting(namingPattern);
    }
    InterlockedExchange(&g_namingPattern, pattern);

    // Folder-template collision mode (0=Skip, 1=Number, 2=Stop). Read as a
    // string option like `trigger`; any unknown value maps to Skip (default).
    PCWSTR tc = Wh_GetStringSetting(L"templateCollision");
    LONG collision = TC_Skip;
    if (tc) {
        if (wcscmp(tc, L"number") == 0) {
            collision = TC_Number;
        } else if (wcscmp(tc, L"stop") == 0) {
            collision = TC_Stop;
        } else {
            collision = TC_Skip;
        }
        Wh_FreeStringSetting(tc);
    }

    // Windhawk supports arrays of strings. Each entry uses
    // Name=Folder 1|Folder 2|Folder 3, avoiding unsupported multi-key objects.
    std::vector<FolderTemplate> raw;
    for (int i = 0; i < 1000; ++i) {
        PCWSTR value = Wh_GetStringSetting(L"folderTemplates[%d]", i);
        if (!value || !value[0]) {
            Wh_FreeStringSetting(value);
            break;
        }
        std::wstring line(value);
        Wh_FreeStringSetting(value);
        // Format: key=Name=Folder1|Folder2. The user assigns the key; it is
        // independent of the name. Malformed lines are ignored.
        size_t firstEq = line.find(L'=');
        if (firstEq == std::wstring::npos || firstEq == 0) continue;
        size_t secondEq = line.find(L'=', firstEq + 1);
        if (secondEq == std::wstring::npos) continue;
        FolderTemplate t;
        t.key = line.substr(0, firstEq);
        t.name = line.substr(firstEq + 1, secondEq - firstEq - 1);
        size_t start = secondEq + 1;
        while (start <= line.size() &&
               t.folders.size() < kMaxFoldersPerTemplate) {
            size_t end = line.find(L'|', start);
            std::wstring folder = line.substr(start, end - start);
            if (!folder.empty()) t.folders.push_back(folder);
            if (end == std::wstring::npos) break;
            start = end + 1;
        }
        if (!t.folders.empty()) raw.push_back(std::move(t));
    }

    std::vector<std::wstring> ignored;
    std::vector<ResolvedTemplate> resolved = ResolveTemplateKeys(raw, &ignored);

    // #1: build the lock-free key mask the hook consults so ONLY a bound letter
    // fires a template (an unbound letter falls through to one default folder).
    LONG keyMask = 0;
    for (const auto& r : resolved) {
        wchar_t c = r.key;
        if (c >= L'a' && c <= L'z') keyMask |= (1L << (c - L'a'));
    }

    // Publish under the lock so the hook/worker threads see a consistent
    // (templates, collision) pair.
    EnterCriticalSection(&g_templatesCs);
    g_templates = std::move(resolved);
    g_templateCollision = collision;
    size_t usableCount = g_templates.size();
    LeaveCriticalSection(&g_templatesCs);

    // Publish a lock-free flag the keyboard hook can read: if ANY template is
    // configured, the trigger must open the key-capture window (which listens
    // for template letters) even when "Multiple folders in one go" is off.
    // Without this, template keys never fire under the default settings.
    InterlockedExchange(&g_hasTemplates, usableCount > 0 ? 1 : 0);
    InterlockedExchange(&g_templateKeyMask, keyMask);

    Wh_Log(L"Templates loaded: %zu usable, collision=%d",
           usableCount, static_cast<int>(collision));
    for (const auto& r : ignored) {
        Wh_Log(L"%ls", r.c_str());
    }

    // One concise summary per (re)load, so tuning any setting always produces a
    // single, greppable line from the mod itself -- instead of relying only on
    // Windhawk's verbose per-Wh_GetIntSetting engine trace.
    Wh_Log(L"Settings applied: shortcut='%ls' (triggerVk=%d heldKeys=%d) multiFolder=%d "
           L"maxFolders=%d autoResolve=%d useDialog=%d openAfterCreate=%d typeSpeed=%d "
           L"namingPattern=%d templateCollision=%d templates=%zu",
           shortcutLabel.c_str(),
           g_shortcut.triggerVk,
           g_shortcut.heldCount,
           AtomicGet(&g_multiFolder) != 0 ? 1 : 0,
           static_cast<int>(AtomicGet(&g_maxFolders)),
           AtomicGet(&g_autoResolveNames) != 0 ? 1 : 0,
           AtomicGet(&g_useDialog) != 0 ? 1 : 0,
           AtomicGet(&g_openAfterCreate) != 0 ? 1 : 0,
           static_cast<int>(AtomicGet(&g_typeSpeed)),
           static_cast<int>(AtomicGet(&g_namingPattern)),
           static_cast<int>(collision),
           usableCount);
}

// Type-speed timeout mappers. Mirror of CountWindowMsForSpeed /
// RenameWatchMsForSpeed in tests/newfolder_logic.h (pinned by
// tests/type_speed_test). ONE setting scales BOTH the count-entry window and
// the inline-rename watch; the chord (F->N) gap is deliberately NOT scaled.
// Three presets only, so an arbitrary value can never make the gesture
// unusable; an unknown speed falls back to Normal.
static UINT CountWindowMsForSpeed(LONG speed) {
    switch (speed) {
        case 1:  return 2500;
        case 2:  return 4000;
        case 0:
        default: return 1200;
    }
}

static int RenameWatchMsForSpeed(LONG speed) {
    switch (speed) {
        case 1:  return 60000;
        case 2:  return 120000;
        case 0:
        default: return 30000;
    }
}

// "Create and open" fires only when the setting is on AND exactly one folder
// was created -- never for a batch (mirror of tests/newfolder_logic.h
// ShouldOpenAfterCreate / tests/open_after_create_test).
static bool ShouldOpenAfterCreate(bool openAfterCreate, int createdCount) {
    return openAfterCreate && createdCount == 1;
}

static bool EnsureUIA() {
    if (g_uia) {
        return true;
    }

    HRESULT hr = CoCreateInstance(CLSID_CUIAutomation,
                                  nullptr,
                                  CLSCTX_INPROC_SERVER,
                                  IID_PPV_ARGS(&g_uia));

    if (FAILED(hr) || !g_uia) {
        Wh_Log(L"CoCreateInstance(CUIAutomation) failed. hr=0x%08X", hr);
        return false;
    }

    return true;
}

// ---------------- Basic helpers ----------------
static std::wstring JoinPath(const std::wstring& a, const std::wstring& b) {
    if (a.empty()) return b;
    if (a.back() == L'\\' || a.back() == L'/') return a + b;
    return a + L'\\' + b;
}

static std::wstring GetFileNamePart(const std::wstring& fullPath) {
    const wchar_t* p = PathFindFileNameW(fullPath.c_str());
    return p ? std::wstring(p) : L"";
}

static std::wstring GetParentDir(const std::wstring& fullPath) {
    wchar_t buf[MAX_PATH * 4] = {0};

    // Guard against paths longer than the fixed buffer: wcscpy_s would fail (and
    // previously its return was ignored, so we'd operate on an empty/garbage
    // buffer). Bail out explicitly instead so an over-long path fails cleanly
    // rather than silently producing a wrong parent.
    if (fullPath.size() >= ARRAYSIZE(buf)) {
        Wh_Log(L"GetParentDir: path too long (%zu chars) for buffer; skipping.",
               fullPath.size());
        return L"";
    }

    if (wcscpy_s(buf, ARRAYSIZE(buf), fullPath.c_str()) != 0) {
        Wh_Log(L"GetParentDir: wcscpy_s failed.");
        return L"";
    }

    if (!PathRemoveFileSpecW(buf)) {
        return L"";
    }

    return buf;
}

// Mirrors tests/newfolder_logic.h::ClampFolderMax.
static int ClampFolderMax(int maxSetting) {
    if (maxSetting < 1) {
        return 1;
    }
    if (maxSetting > kFolderHardCap) {
        return kFolderHardCap;
    }
    return maxSetting;
}

// Mirrors tests/newfolder_logic.h::ResolveFolderCount. Off -> always 1; no digits
// (requested < 1) -> 1; otherwise clamp to [1, ClampFolderMax(maxSetting)].
static int ResolveFolderCount(int requested, bool multiEnabled, int maxSetting) {
    if (!multiEnabled) {
        return 1;
    }

    int effMax = ClampFolderMax(maxSetting);
    if (requested < 1) {
        return 1;
    }
    if (requested > effMax) {
        return effMax;
    }
    return requested;
}

// Mirrors tests/newfolder_logic.h::MakeUniqueFolderNames. Returns `count` paths
// using "New folder", "New folder (2)", ..., reserving each within the batch and
// skipping anything already on disk (so making more never collides or clobbers).
static std::vector<std::wstring> MakeUniqueFolderNames(const std::wstring& dir,
                                                       int count,
                                                       bool autoResolve) {
    std::vector<std::wstring> out;
    if (count < 1) {
        return out;
    }

    // Names are emitted in strictly increasing numeric order and an index we
    // pass is never revisited, so a monotonic cursor replaces the old per-slot
    // rescan-from-(2). That turns O(count * (stat + batch)) -- thousands of
    // PathFileExistsW stats at the 100-folder cap in a busy directory -- into
    // O(count + preexisting) stats, with no per-slot rescan of `out`. Output is
    // byte-identical (multi_name_test.cpp / auto_resolve_test.cpp pin it).
    int nextIndex = 1;  // 1 == bare "New folder"; 2+ == "New folder (N)".

    auto pathForIndex = [&](int i) -> std::wstring {
        if (i <= 1) {
            return JoinPath(dir, L"New folder");
        }
        wchar_t buf[64] = {0};
        swprintf_s(buf, ARRAYSIZE(buf), L"New folder (%d)", i);
        return JoinPath(dir, buf);
    };

    for (int k = 0; k < count; ++k) {
        if (!autoResolve) {
            // Numbering disabled: only the bare "New folder" is ever a
            // candidate, and it can be claimed at most once per batch.
            std::wstring base = pathForIndex(1);
            if (nextIndex <= 1 && !PathFileExistsW(base.c_str())) {
                out.push_back(base);
            }
            nextIndex = 2;
            continue;
        }

        bool placed = false;
        for (; nextIndex < 100000; ++nextIndex) {
            std::wstring candidate = pathForIndex(nextIndex);
            if (!PathFileExistsW(candidate.c_str())) {
                out.push_back(candidate);
                ++nextIndex;  // reserve it; the next slot starts past it
                placed = true;
                break;
            }
        }
        if (!placed) {
            break;
        }
    }

    return out;
}

// ---- Name-capture resolution (mirrors tests/capture_logic.h) ----
// Batch numbering pattern (mirror of tests/newfolder_logic.h NamePattern and
// the `namingPattern` setting). Chooses ONLY how the number is formatted; the
// base name and skip-existing collision resolution are the same for every
// pattern. Pad width only pads -- a number wider than the width is written in
// full, so the sequence never collides by truncation.
//   0 Space  : "<base> 1"
//   1 Dash2  : "<base>-01"
//   2 Dash3  : "<base>-001"
//   3 Paren2 : "<base> (01)"
static std::wstring PadNumber(int n, int width) {
    std::wstring s = std::to_wstring(n);
    while (static_cast<int>(s.size()) < width) {
        s.insert(s.begin(), L'0');
    }
    return s;
}

static std::wstring FormatBatchName(const std::wstring& base, int index,
                                    LONG pattern) {
    switch (pattern) {
        case 1:  return base + L"-" + PadNumber(index, 2);       // Dash2
        case 2:  return base + L"-" + PadNumber(index, 3);       // Dash3
        case 3:  return base + L" (" + PadNumber(index, 2) + L")"; // Paren2
        case 4:  return base + L"_" + PadNumber(index, 2);       // Under2
        case 5:  return base + L"_" + PadNumber(index, 3);       // Under3
        case 0:
        default: return base + L" " + std::to_wstring(index);    // Space
    }
}

// Named batch: "<base> 1", "<base> 2", ... (or the chosen pattern), skipping
// any already on disk. Pattern defaults to the current `namingPattern` setting.
static std::vector<std::wstring> MakeNamedFolderNames(const std::wstring& dir,
                                                      const std::wstring& base,
                                                      int count,
                                                      LONG pattern) {
    std::vector<std::wstring> out;
    if (count < 1 || base.empty()) {
        return out;
    }
    int nextIndex = 1;
    for (int k = 0; k < count; ++k) {
        bool placed = false;
        for (; nextIndex < 1000000; ++nextIndex) {
            std::wstring candidate =
                JoinPath(dir, FormatBatchName(base, nextIndex, pattern));
            if (!PathFileExistsW(candidate.c_str())) {
                out.push_back(candidate);
                ++nextIndex;
                placed = true;
                break;
            }
        }
        if (!placed) {
            break;
        }
    }
    return out;
}

// Overload that uses the live `namingPattern` setting -- keeps the existing
// call sites unchanged while routing them through the selected pattern.
static std::vector<std::wstring> MakeNamedFolderNames(const std::wstring& dir,
                                                      const std::wstring& base,
                                                      int count) {
    return MakeNamedFolderNames(dir, base, count,
                                AtomicGet(&g_namingPattern));
}

// Windows-forbidden filename characters.
static bool IsForbiddenNameChar(wchar_t c) {
    return c == L'\\' || c == L'/' || c == L':' || c == L'*' ||
           c == L'?'  || c == L'"' || c == L'<' || c == L'>' || c == L'|';
}

// Sanitize typed input into a folder base: strip control + forbidden chars,
// trim whitespace, strip trailing dots/spaces. May return empty.
static std::wstring SanitizeFolderBase(const std::wstring& raw) {
    std::wstring s;
    s.reserve(raw.size());
    for (wchar_t c : raw) {
        // #12: strip ALL C0 control chars (< 0x20), covering \r \n \t plus every
        // other control char that would otherwise reach CreateDirectoryW and
        // make it fail. Mirrors tests/capture_logic.h::SanitizeFolderBase.
        if (c < 0x20) {
            continue;
        }
        if (IsForbiddenNameChar(c)) {
            continue;
        }
        s.push_back(c);
    }
    size_t b = 0;
    while (b < s.size() && iswspace(s[b])) {
        ++b;
    }
    size_t e = s.size();
    while (e > b && (iswspace(s[e - 1]) || s[e - 1] == L'.')) {
        --e;
    }
    return s.substr(b, e - b);
}

// Single named folder: "<base>", then "<base> (2)", "<base> (3)", ...
// #9: when autoResolve is off, never renumber a clashing custom name -- return
// the bare "<base>" and let the create layer report the collision (matches
// autoResolveNames=off semantics for the default name too). Mirrors
// tests/capture_logic.h::MakeUniqueNamedFolder.
static std::wstring MakeUniqueNamedFolder(const std::wstring& dir,
                                          const std::wstring& base,
                                          bool autoResolve = true) {
    std::wstring first = JoinPath(dir, base);
    if (!autoResolve || !PathFileExistsW(first.c_str())) {
        return first;
    }
    for (int i = 2; i < 100000; ++i) {
        wchar_t buf[24] = {0};
        swprintf_s(buf, ARRAYSIZE(buf), L" (%d)", i);
        std::wstring candidate = JoinPath(dir, base + buf);
        if (!PathFileExistsW(candidate.c_str())) {
            return candidate;
        }
    }
    return first;
}

// Resolve captured base + count into concrete folder paths.
//   empty base        -> default "New folder"/"New folder (N)" flow
//   base + count == 1 -> single "<base>" (with " (2)" fallback)
//   base + count > 1  -> "<base> 1".."<base> N" (skips existing)
static std::vector<std::wstring> ResolveCaptureNames(const std::wstring& dir,
                                                     const std::wstring& rawBase,
                                                     int count,
                                                     bool autoResolve) {
    std::wstring base = SanitizeFolderBase(rawBase);
    if (base.empty()) {
        return MakeUniqueFolderNames(dir, count, autoResolve);
    }
    if (count <= 1) {
        return { MakeUniqueNamedFolder(dir, base, autoResolve) };
    }
    return MakeNamedFolderNames(dir, base, count);
}

// ---- Folder Templates: pure logic (mirrors tests/capture_logic.h; pinned by
// tests/template_keys_test.cpp + tests/template_folders_test.cpp) ----
// The types, constants and g_templates state are declared ABOVE LoadSettings.
// Adapted to the mod: existence via PathFileExistsW, numbering via the mod's
// FormatBatchName(LONG pattern). The shortcut key is user-assigned per
// template (the `key=` field), independent of the name. No templates are built in.

// Case-fold for case-INSENSITIVE comparison (Windows dir names). Mirror of
// tests/capture_logic.h ToLowerFold. #F3: "raw" and "RAW" are the same folder.
static std::wstring ToLowerFold(const std::wstring& s) {
    std::wstring r;
    r.reserve(s.size());
    for (wchar_t c : s) {
        r.push_back(static_cast<wchar_t>(towlower(c)));
    }
    return r;
}

// First letter of the name, lowercased, or 0 if it is not an a-z letter.
static wchar_t NormalizeTemplateKey(const std::wstring& rawName) {
    size_t b = 0;
    while (b < rawName.size() && iswspace(rawName[b])) {
        ++b;
    }
    if (b >= rawName.size()) {
        return 0;
    }
    wchar_t c = rawName[b];
    if (c >= L'A' && c <= L'Z') {
        c = static_cast<wchar_t>(c - L'A' + L'a');
    }
    if (c >= L'a' && c <= L'z') {
        return c;
    }
    return 0;
}

// A template folder entry must be a plain leaf name (no path, no traversal, no
// control chars). Mirrors tests/capture_logic.h::IsValidTemplateFolderName.
static bool IsValidTemplateFolderName(const std::wstring& folder) {
    if (folder.empty()) {
        return false;
    }
    bool allSpace = true;
    for (wchar_t c : folder) {
        if (!iswspace(c)) {
            allSpace = false;
            break;
        }
    }
    if (allSpace) {
        return false;
    }
    for (wchar_t c : folder) {
        if (c < 0x20) {
            return false;
        }
        // #7: reject the SAME set a typed name strips (\ / : * ? " < > |), not
        // just \ / : -- so a template can't smuggle a Windows-forbidden char.
        if (IsForbiddenNameChar(c)) {
            return false;
        }
    }
    if (folder == L"." || folder == L"..") {
        return false;
    }
    if (folder.size() >= 2 && folder[0] == L'.' && folder[1] == L'.') {
        return false;
    }
    return true;
}

// Validate + dedup (first occurrence wins) + cap a template's folder list.
static std::vector<std::wstring> CleanTemplateFolders(
        const std::vector<std::wstring>& folders) {
    std::vector<std::wstring> out;
    for (const auto& f : folders) {
        if (static_cast<int>(out.size()) >= kMaxFoldersPerTemplate) {
            break;
        }
        if (!IsValidTemplateFolderName(f)) {
            continue;
        }
        // #F3: dedup case-INSENSITIVELY -- raw|RAW name the same Windows
        // folder, so keep only the first spelling.
        std::wstring fold = ToLowerFold(f);
        bool dup = false;
        for (const auto& g : out) {
            if (ToLowerFold(g) == fold) {
                dup = true;
                break;
            }
        }
        if (dup) {
            continue;
        }
        out.push_back(f);
    }
    return out;
}

// Load-time resolution: derive keys, first-in-list wins on a shared letter,
// drop invalid, enforce the 26-template cap. Ignore reasons are appended to
// `ignored` (logged by the caller). Mirrors ResolveTemplateKeys in the header.
static std::vector<ResolvedTemplate> ResolveTemplateKeys(
        const std::vector<FolderTemplate>& raw,
        std::vector<std::wstring>* ignored) {
    std::vector<ResolvedTemplate> usable;
    for (const auto& t : raw) {
        if (static_cast<int>(usable.size()) >= kMaxTemplates) {
            if (ignored) {
                ignored->push_back(L"Template '" + t.name +
                                   L"' ignored (over 26-template cap)");
            }
            continue;
        }
        if (t.name.empty()) {
            if (ignored) {
                ignored->push_back(L"Template with blank name ignored");
            }
            continue;
        }
        wchar_t key = NormalizeTemplateKey(t.key);
        if (key == 0) {
            if (ignored) {
                ignored->push_back(L"Template '" + t.name +
                                   L"' ignored (key must be a single letter A-Z)");
            }
            continue;
        }
        bool claimed = false;
        std::wstring owner;
        for (const auto& u : usable) {
            if (u.key == key) {
                claimed = true;
                owner = u.name;
                break;
            }
        }
        if (claimed) {
            if (ignored) {
                ignored->push_back(L"Template '" + t.name + L"' key '" +
                                   std::wstring(1, key) +
                                   L"' ignored (already used by '" + owner + L"')");
            }
            continue;
        }
        std::vector<std::wstring> folders = CleanTemplateFolders(t.folders);
        if (folders.empty()) {
            if (ignored) {
                ignored->push_back(L"Template '" + t.name +
                                   L"' ignored (no valid folder names)");
            }
            continue;
        }
        ResolvedTemplate rt;
        rt.key = key;
        rt.name = t.name;
        rt.folders = folders;
        usable.push_back(rt);
    }
    return usable;
}

// Fire-time: given a template's folders + collision mode + batch pattern,
// return the concrete paths to create (empty for Stop when any already exists).
// #6: first free numbered sibling of `leaf` (create-time race recovery for
// Number mode); re-scans disk so it advances past whatever just appeared.
// #8-1: also skips any path still RESERVED by other plans in this action, so a
// retry never steals a name a later plan is going to create (which would force
// that plan to renumber, e.g. [raw, "raw 3"] collapsing to "raw 3" / "raw 3 2").
// `reserved` holds the folded paths this action still owns.
// Mirror of tests/capture_logic.h NextFreeTemplateSibling.
static std::wstring NextFreeTemplateSibling(const std::wstring& dir,
                                            const std::wstring& leaf,
                                            LONG pattern,
                                            const std::vector<std::wstring>& reserved) {
    for (int i = 2; i < 1000000; ++i) {
        std::wstring candidate = JoinPath(dir, FormatBatchName(leaf, i, pattern));
        if (PathFileExistsW(candidate.c_str())) {
            continue;
        }
        std::wstring fold = ToLowerFold(candidate);
        bool isReserved = false;
        for (const auto& r : reserved) {
            if (r == fold) { isReserved = true; break; }
        }
        if (!isReserved) {
            return candidate;
        }
    }
    return JoinPath(dir, leaf);
}

// One planned template folder: the ORIGINAL base name + the resolved path.
// Mirror of tests/capture_logic.h TemplateFolderPlan. #1: carrying `base` lets
// a create-time race retry number from the original name ("raw" -> "raw 3"),
// not from an already-numbered candidate ("raw 2" -> "raw 2 2").
struct TemplateFolderPlan {
    std::wstring base;
    std::wstring path;
};

// Mirrors ResolveTemplateFolders in the header, adapted to PathFileExistsW.
static std::vector<TemplateFolderPlan> ResolveTemplateFolders(
        const std::wstring& dir, const std::vector<std::wstring>& tmplFolders,
        LONG pattern, LONG collision) {
    std::vector<std::wstring> names = CleanTemplateFolders(tmplFolders);
    std::vector<TemplateFolderPlan> out;

    // #2: reserve every path chosen THIS action (case-insensitive: raw==RAW on
    // Windows). A name is "taken" if it is on disk OR already reserved by an
    // earlier entry this run -- stops [raw, "raw 2"] planning "raw 2" twice.
    std::vector<std::wstring> planned;   // folded paths
    auto taken = [&](const std::wstring& p) {
        if (PathFileExistsW(p.c_str())) return true;
        std::wstring fold = ToLowerFold(p);
        for (const auto& q : planned) if (q == fold) return true;
        return false;
    };
    // #1: each plan carries the ORIGINAL base name for correct race-retry.
    auto reserve = [&](const std::wstring& base, const std::wstring& path) {
        planned.push_back(ToLowerFold(path));
        out.push_back(TemplateFolderPlan{ base, path });
    };

    if (collision == TC_Stop) {
        for (const auto& n : names) {
            if (PathFileExistsW(JoinPath(dir, n).c_str())) {
                return {};
            }
        }
        for (const auto& n : names) {
            reserve(n, JoinPath(dir, n));
        }
        return out;
    }

    for (const auto& n : names) {
        std::wstring path = JoinPath(dir, n);
        if (!taken(path)) {
            reserve(n, path);
            continue;
        }
        if (collision == TC_Skip) {
            continue;
        }
        // TC_Number: numbered sibling of the ORIGINAL base `n`; never overwrite
        // and never collide with a path already planned this run.
        for (int i = 2; i < 1000000; ++i) {
            std::wstring candidate =
                JoinPath(dir, FormatBatchName(n, i, pattern));
            if (!taken(candidate)) {
                reserve(n, candidate);
                break;
            }
        }
    }
    return out;
}

static std::wstring GetDesktopDir() {
    PWSTR p = nullptr;

    if (SUCCEEDED(SHGetKnownFolderPath(FOLDERID_Desktop, 0, nullptr, &p))) {
        std::wstring ret = p;
        CoTaskMemFree(p);
        return ret;
    }

    wchar_t buf[MAX_PATH] = {0};

    if (SUCCEEDED(SHGetFolderPathW(nullptr,
                                   CSIDL_DESKTOPDIRECTORY,
                                   nullptr,
                                   SHGFP_TYPE_CURRENT,
                                   buf))) {
        return buf;
    }

    return L"";
}

static bool IsPermissionError(DWORD err) {
    return err == ERROR_ACCESS_DENIED ||
           err == ERROR_PRIVILEGE_NOT_HELD ||
           err == ERROR_ELEVATION_REQUIRED;
}

// ---------------- Window helpers ----------------
static bool IsClassName(HWND hwnd, const wchar_t* expected) {
    if (!hwnd || !expected) return false;

    wchar_t cls[128] = {0};
    GetClassNameW(hwnd, cls, ARRAYSIZE(cls));

    return wcscmp(cls, expected) == 0;
}

static bool IsExplorerTopLevel(HWND hwnd) {
    return IsClassName(hwnd, L"CabinetWClass") ||
           IsClassName(hwnd, L"ExploreWClass");
}

static bool IsDescendantOrSelf(HWND parent, HWND child) {
    if (!parent || !child) return false;
    if (parent == child) return true;
    return IsChild(parent, child) != FALSE;
}

static HWND FindAncestorOrSelfByClass(HWND hwnd,
                                      const wchar_t* className,
                                      HWND stopAt) {
    for (HWND cur = hwnd; cur; cur = GetParent(cur)) {
        if (IsClassName(cur, className)) {
            return cur;
        }

        if (cur == stopAt) {
            break;
        }
    }

    return nullptr;
}

static HWND HitTestExplorerByCursor() {
    POINT pt = {};
    GetCursorPos(&pt);

    HWND h = WindowFromPoint(pt);
    if (!h) return nullptr;

    HWND root = GetAncestor(h, GA_ROOT);

    if (IsExplorerTopLevel(root)) {
        return root;
    }

    for (HWND cur = h; cur; cur = GetParent(cur)) {
        if (IsExplorerTopLevel(cur)) {
            return cur;
        }
    }

    return nullptr;
}

static HWND GetTargetExplorerHWND() {
    HWND fg = GetForegroundWindow();

    if (fg && (IsExplorerTopLevel(fg) || fg == GetShellWindow())) {
        return fg;
    }

    if (HWND h = HitTestExplorerByCursor()) {
        return h;
    }

    HWND shell = GetShellWindow();

    if (shell) {
        return shell;
    }

    return nullptr;
}

static HWND GetThreadFocusWindow(HWND topLevel) {
    if (!topLevel) return nullptr;

    DWORD tid = GetWindowThreadProcessId(topLevel, nullptr);

    GUITHREADINFO gi = {};
    gi.cbSize = sizeof(gi);

    if (!GetGUIThreadInfo(tid, &gi)) {
        return nullptr;
    }

    if (gi.hwndFocus) return gi.hwndFocus;
    if (gi.hwndActive) return gi.hwndActive;

    return nullptr;
}

struct DefViewSearchCtx {
    HWND found = nullptr;
};

static BOOL CALLBACK FindDefViewEnumProc(HWND child, LPARAM lParam) {
    DefViewSearchCtx* ctx = reinterpret_cast<DefViewSearchCtx*>(lParam);

    if (IsWindowVisible(child) && IsClassName(child, L"SHELLDLL_DefView")) {
        ctx->found = child;
        return FALSE;
    }

    return TRUE;
}

struct ShellTabSearchCtx {
    HWND bestTab = nullptr;
    LONG_PTR bestArea = 0;
};

static BOOL CALLBACK FindShellTabEnumProc(HWND hwnd, LPARAM lParam) {
    ShellTabSearchCtx* ctx = reinterpret_cast<ShellTabSearchCtx*>(lParam);

    if (!IsWindowVisible(hwnd)) return TRUE;
    if (!IsClassName(hwnd, L"ShellTabWindowClass")) return TRUE;

    DefViewSearchCtx defViewCtx;
    EnumChildWindows(hwnd,
                     FindDefViewEnumProc,
                     reinterpret_cast<LPARAM>(&defViewCtx));

    if (!defViewCtx.found) return TRUE;

    RECT rc = {};
    if (!GetWindowRect(defViewCtx.found, &rc)) return TRUE;

    LONG_PTR area =
        static_cast<LONG_PTR>(rc.right - rc.left) *
        static_cast<LONG_PTR>(rc.bottom - rc.top);

    if (area > ctx->bestArea) {
        ctx->bestArea = area;
        ctx->bestTab = hwnd;
    }

    return TRUE;
}

static HWND GetActiveShellTabHwnd(HWND explorerHwnd) {
    if (!explorerHwnd || explorerHwnd == GetShellWindow()) {
        return nullptr;
    }

    HWND focus = GetThreadFocusWindow(explorerHwnd);

    if (focus && IsDescendantOrSelf(explorerHwnd, focus)) {
        HWND tab = FindAncestorOrSelfByClass(focus,
                                             L"ShellTabWindowClass",
                                             explorerHwnd);
        if (tab) {
            return tab;
        }
    }

    // If focus is on the tab bar, command bar, or address bar, it may not be in
    // the ShellTabWindowClass subtree. Fall back to the tab holding the visible
    // SHELLDLL_DefView.
    ShellTabSearchCtx ctx;
    EnumChildWindows(explorerHwnd,
                     FindShellTabEnumProc,
                     reinterpret_cast<LPARAM>(&ctx));

    return ctx.bestTab;
}

static void ForceForeground(HWND hwnd) {
    if (!hwnd || !IsWindow(hwnd)) return;

    DWORD targetThread = GetWindowThreadProcessId(hwnd, nullptr);
    DWORD currentThread = GetCurrentThreadId();

    // Only attach when the target lives on another thread: AttachThreadInput to
    // your own thread fails, and we must only detach if the attach succeeded
    // (an unbalanced detach on a failed attach corrupts the input-queue state).
    bool attached = false;
    if (targetThread != currentThread) {
        attached = AttachThreadInput(currentThread, targetThread, TRUE) != FALSE;
    }

    if (!SetForegroundWindow(hwnd)) {
        // Foreground can be legitimately refused (foreground-lock timeout);
        // hardening only -- BringWindowToTop below still raises the window.
        Wh_Log(L"ForceForeground: SetForegroundWindow failed (error=%lu).",
               GetLastError());
    }
    BringWindowToTop(hwnd);

    if (attached) {
        AttachThreadInput(currentThread, targetThread, FALSE);
    }
}

static void SendSimpleKey(WORD key) {
    INPUT inputs[2] = {};

    inputs[0].type = INPUT_KEYBOARD;
    inputs[0].ki.wVk = key;

    inputs[1].type = INPUT_KEYBOARD;
    inputs[1].ki.wVk = key;
    inputs[1].ki.dwFlags = KEYEVENTF_KEYUP;

    UINT sent = SendInput(ARRAYSIZE(inputs), inputs, sizeof(INPUT));
    if (sent != ARRAYSIZE(inputs)) {
        // Input can be blocked (e.g. UIPI / a secure desktop). Hardening only:
        // there is no safe retry from here, so log and move on.
        Wh_Log(L"SendSimpleKey: SendInput sent %u/%u events (error=%lu).",
               sent, static_cast<UINT>(ARRAYSIZE(inputs)), GetLastError());
    }
}

static void WaitForModifierKeysReleased() {
    // #4: wait for the keys of the CONFIGURED shortcut (held keys + trigger)
    // plus the standard modifiers -- not a fixed Ctrl/Alt/Shift/N/F set. For a
    // shortcut like Alt+F2 the trigger F2 must be released too, or the injected
    // F2 merges with the user's still-held F2.
    std::vector<int> waitVks = ShortcutReleaseWaitVks(CurrentShortcut());
    for (int i = 0; i < 40; ++i) {
        bool anyDown = false;
        for (int vk : waitVks) {
            if ((GetAsyncKeyState(vk) & 0x8000) != 0) {
                anyDown = true;
                break;
            }
        }
        if (!anyDown) {
            return;
        }

        Sleep(5);
    }
}

// ---------------- ShellView resolve for rename ----------------
static bool GetPathFromShellView(IShellView* view, std::wstring& outPath) {
    outPath.clear();

    if (!view) return false;

    IFolderView* folderView = nullptr;
    HRESULT hr = view->QueryInterface(IID_PPV_ARGS(&folderView));

    if (FAILED(hr) || !folderView) {
        return false;
    }

    IPersistFolder2* persistFolder = nullptr;
    hr = folderView->GetFolder(IID_PPV_ARGS(&persistFolder));
    SafeRelease(folderView);

    if (FAILED(hr) || !persistFolder) {
        return false;
    }

    PIDLIST_ABSOLUTE pidl = nullptr;
    hr = persistFolder->GetCurFolder(&pidl);
    SafeRelease(persistFolder);

    if (FAILED(hr) || !pidl) {
        return false;
    }

    wchar_t path[MAX_PATH * 4] = {0};
    bool ok = SHGetPathFromIDListEx(pidl,
                                    path,
                                    ARRAYSIZE(path),
                                    GPFIDL_DEFAULT) &&
              path[0] != L'\0';

    CoTaskMemFree(pidl);

    if (ok) {
        outPath = path;
        return true;
    }

    return false;
}

static bool ResolveActiveShellView(HWND explorerHwnd,
                                   std::wstring& outPath,
                                   IShellView** outView) {
    if (!outView) return false;
    outPath.clear();
    *outView = nullptr;

    if (!explorerHwnd || !IsWindow(explorerHwnd)) {
        return false;
    }

    HWND activeTab = GetActiveShellTabHwnd(explorerHwnd);

    IShellWindows* shellWindows = nullptr;
    HRESULT hr = CoCreateInstance(CLSID_ShellWindows,
                                  nullptr,
                                  CLSCTX_ALL,
                                  IID_PPV_ARGS(&shellWindows));

    if (FAILED(hr) || !shellWindows) {
        return false;
    }

    long count = 0;
    shellWindows->get_Count(&count);

    for (long i = count - 1; i >= 0; --i) {
        VARIANT v;
        VariantInit(&v);
        v.vt = VT_I4;
        v.lVal = i;

        IDispatch* dispatch = nullptr;

        if (FAILED(shellWindows->Item(v, &dispatch)) || !dispatch) {
            VariantClear(&v);
            continue;
        }

        IServiceProvider* serviceProvider = nullptr;
        hr = dispatch->QueryInterface(IID_PPV_ARGS(&serviceProvider));
        SafeRelease(dispatch);

        if (FAILED(hr) || !serviceProvider) {
            VariantClear(&v);
            continue;
        }

        IShellBrowser* browser = nullptr;
        hr = serviceProvider->QueryService(SID_STopLevelBrowser,
                                           IID_PPV_ARGS(&browser));
        SafeRelease(serviceProvider);

        if (FAILED(hr) || !browser) {
            VariantClear(&v);
            continue;
        }

        IShellView* view = nullptr;
        hr = browser->QueryActiveShellView(&view);
        SafeRelease(browser);

        if (FAILED(hr) || !view) {
            VariantClear(&v);
            continue;
        }

        HWND viewHwnd = nullptr;
        hr = view->GetWindow(&viewHwnd);

        if (FAILED(hr) || !viewHwnd) {
            SafeRelease(view);
            VariantClear(&v);
            continue;
        }

        HWND viewRoot = GetAncestor(viewHwnd, GA_ROOT);
        if (viewRoot != explorerHwnd) {
            SafeRelease(view);
            VariantClear(&v);
            continue;
        }

        if (activeTab) {
            HWND viewTab = FindAncestorOrSelfByClass(viewHwnd,
                                                     L"ShellTabWindowClass",
                                                     explorerHwnd);
            if (viewTab != activeTab) {
                SafeRelease(view);
                VariantClear(&v);
                continue;
            }
        }

        std::wstring viewPath;
        if (GetPathFromShellView(view, viewPath)) {
            outPath = viewPath;
            *outView = view;
            shellWindows->Release();
            VariantClear(&v);
            return true;
        }

        SafeRelease(view);
        VariantClear(&v);
    }

    shellWindows->Release();
    return false;
}

// ---------------- PIDL / rename ----------------
static bool BindParentAndChildPIDL(const std::wstring& fullPath,
                                   PIDLIST_ABSOLUTE* outFullPidl,
                                   PCUITEMID_CHILD* outChild) {
    if (!outFullPidl || !outChild) return false;

    *outFullPidl = nullptr;
    *outChild = nullptr;

    PIDLIST_ABSOLUTE pidl = nullptr;
    SFGAOF sf = 0;

    HRESULT hr = SHParseDisplayName(fullPath.c_str(),
                                    nullptr,
                                    &pidl,
                                    0,
                                    &sf);

    if (FAILED(hr) || !pidl) {
        return false;
    }

    IShellFolder* parent = nullptr;
    PCUITEMID_CHILD child = nullptr;

    hr = SHBindToParent(pidl, IID_PPV_ARGS(&parent), &child);

    if (FAILED(hr) || !parent || !child) {
        if (parent) parent->Release();
        CoTaskMemFree(pidl);
        return false;
    }

    parent->Release();

    *outFullPidl = pidl;
    *outChild = child;
    return true;
}

static bool RenameViaShellView(IShellView* shellView,
                               const std::wstring& fullPath) {
    if (!shellView) {
        return false;
    }

    ULONGLONG t0 = GetTickCount64();

    PIDLIST_ABSOLUTE fullPidl = nullptr;
    PCUITEMID_CHILD child = nullptr;

    if (!BindParentAndChildPIDL(fullPath, &fullPidl, &child)) {
        Wh_Log(L"BindParentAndChildPIDL failed, cost=%llums",
               GetTickCount64() - t0);
        return false;
    }

    HRESULT hr = shellView->SelectItem(child,
                                       SVSI_SELECT |
                                       SVSI_ENSUREVISIBLE |
                                       SVSI_DESELECTOTHERS |
                                       SVSI_EDIT);

    CoTaskMemFree(fullPidl);

    if (SUCCEEDED(hr)) {
        Wh_Log(L"Rename via IShellView succeeded, cost=%llums",
               GetTickCount64() - t0);
        return true;
    }

    Wh_Log(L"IShellView::SelectItem failed. hr=0x%08X cost=%llums",
           hr,
           GetTickCount64() - t0);
    return false;
}

// ---------------- Folder creation ----------------
// #5: a check-then-create (PathFileExistsW then CreateDirectoryW) is not atomic
// -- another process can win the name in between, so CreateDirectoryW fails with
// ERROR_ALREADY_EXISTS. That is a collision to retry (advance to the next
// candidate name), NOT a hard failure. Mirrors
// tests/newfolder_logic.h::ShouldRetryOnCollision.
static bool ShouldRetryOnCollision(DWORD createError) {
    return createError == ERROR_ALREADY_EXISTS;
}

static bool CreateFolderDirect(const std::wstring& fullPath,
                               DWORD* pError = nullptr) {
    if (pError) {
        *pError = ERROR_SUCCESS;
    }

    if (CreateDirectoryW(fullPath.c_str(), nullptr)) {
        return true;
    }

    if (pError) {
        *pError = GetLastError();
    }
    return false;
}

static bool CreateFolderWithShellElevation(const std::wstring& fullPath,
                                           HWND owner) {
    std::wstring dir = GetParentDir(fullPath);
    std::wstring name = GetFileNamePart(fullPath);

    if (dir.empty() || name.empty()) return false;

    IShellItem* folderItem = nullptr;
    HRESULT hr = SHCreateItemFromParsingName(dir.c_str(),
                                             nullptr,
                                             IID_PPV_ARGS(&folderItem));

    if (FAILED(hr) || !folderItem) {
        Wh_Log(L"SHCreateItemFromParsingName failed. hr=0x%08X dir=%ls",
               hr,
               dir.c_str());
        return false;
    }

    IFileOperation* fileOp = nullptr;
    hr = CoCreateInstance(CLSID_FileOperation,
                          nullptr,
                          CLSCTX_ALL,
                          IID_PPV_ARGS(&fileOp));

    if (FAILED(hr) || !fileOp) {
        SafeRelease(folderItem);
        Wh_Log(L"CoCreateInstance(CLSID_FileOperation) failed. hr=0x%08X",
               hr);
        return false;
    }

    if (owner && IsWindow(owner)) {
        fileOp->SetOwnerWindow(owner);
    }

    DWORD flags =
        FOF_NOCONFIRMMKDIR |
        FOFX_SHOWELEVATIONPROMPT |
        FOFX_NOCOPYHOOKS;

    fileOp->SetOperationFlags(flags);

    hr = fileOp->NewItem(folderItem,
                         FILE_ATTRIBUTE_DIRECTORY,
                         name.c_str(),
                         nullptr,
                         nullptr);

    if (SUCCEEDED(hr)) {
        hr = fileOp->PerformOperations();
    }

    BOOL aborted = FALSE;
    fileOp->GetAnyOperationsAborted(&aborted);

    SafeRelease(fileOp);
    SafeRelease(folderItem);

    if (FAILED(hr) || aborted) {
        Wh_Log(L"IFileOperation::NewItem failed. hr=0x%08X aborted=%d path=%ls",
               hr,
               aborted,
               fullPath.c_str());
        return false;
    }

    return PathFileExistsW(fullPath.c_str()) != FALSE;
}

// ---------------- UIA rename fallback ----------------
static IUIAutomationCondition* UIA_MakeExactNameCondition(const std::wstring& name) {
    if (!EnsureUIA()) return nullptr;

    VARIANT vName;
    VariantInit(&vName);
    vName.vt = VT_BSTR;
    vName.bstrVal = SysAllocString(name.c_str());

    IUIAutomationCondition* nameCond = nullptr;
    g_uia->CreatePropertyCondition(UIA_NamePropertyId,
                                   vName,
                                   &nameCond);

    VariantClear(&vName);
    return nameCond;
}

static bool UIA_SelectItemByNameFallback(HWND explorerHwnd,
                                         const std::wstring& fileName) {
    if (!explorerHwnd || !IsWindow(explorerHwnd) || fileName.empty()) {
        return false;
    }

    if (!EnsureUIA()) {
        return false;
    }

    IUIAutomationElement* root = nullptr;
    HRESULT hr = g_uia->ElementFromHandle(explorerHwnd, &root);

    if (FAILED(hr) || !root) {
        return false;
    }

    IUIAutomationCondition* nameCond = UIA_MakeExactNameCondition(fileName);
    if (!nameCond) {
        SafeRelease(root);
        return false;
    }

    IUIAutomationElement* item = nullptr;
    hr = root->FindFirst(TreeScope_Descendants, nameCond, &item);

    SafeRelease(nameCond);
    SafeRelease(root);

    if (FAILED(hr) || !item) {
        return false;
    }

    bool ok = false;

    IUnknown* unkPattern = nullptr;
    hr = item->GetCurrentPattern(UIA_SelectionItemPatternId, &unkPattern);

    if (SUCCEEDED(hr) && unkPattern) {
        IUIAutomationSelectionItemPattern* sel = nullptr;

        if (SUCCEEDED(unkPattern->QueryInterface(IID_PPV_ARGS(&sel))) && sel) {
            ok = SUCCEEDED(sel->Select());
            SafeRelease(sel);
        }

        SafeRelease(unkPattern);
    }

    if (!ok) {
        ok = SUCCEEDED(item->SetFocus());
    } else {
        item->SetFocus();
    }

    SafeRelease(item);
    return ok;
}

// #6: arm the hook-thread Esc latch. MUST be called BEFORE StartRename so the
// hook is already recording Esc when the rename edit first appears -- closing
// the tiny pre-arm window where an extremely fast Esc pressed between
// StartRename returning and the watch loop starting would otherwise be missed.
// Clears any stale press, then marks the rename active.
static void ArmInlineEscLatch() {
    InterlockedExchange(&g_inlineEscPressed, 0);
    InterlockedExchange(&g_inlineRenameActive, 1);
}

static void StartRename(HWND explorerHwnd,
                        IShellView* shellView,
                        const std::wstring& fullPath,
                        const std::wstring& folderPath,
                        bool elevated) {
    std::wstring fileName = GetFileNamePart(fullPath);

    if (fileName.empty()) return;

    SHChangeNotify(SHCNE_MKDIR, SHCNF_PATHW, fullPath.c_str(), nullptr);
    SHChangeNotify(SHCNE_UPDATEDIR, SHCNF_PATHW, folderPath.c_str(), nullptr);

    ULONGLONG t0 = GetTickCount64();

    if (shellView) {
        bool ok = RenameViaShellView(shellView, fullPath);

        if (ok) {
            return;
        }
    } else {
        Wh_Log(L"Active ShellView for rename not available, cost=%llums",
               GetTickCount64() - t0);
    }

    ULONGLONG fallbackStart = GetTickCount64();
    const int maxWaitMs = elevated ? 900 : 250;
    const int intervalMs = 10;
    const int maxTries = maxWaitMs / intervalMs;

    bool selected = false;

    for (int i = 0; i < maxTries; ++i) {
        selected = UIA_SelectItemByNameFallback(explorerHwnd, fileName);

        if (selected) {
            break;
        }

        Sleep(intervalMs);
    }

    if (!selected) {
        Wh_Log(L"Fallback UIA failed to select item: %ls, cost=%llums",
               fileName.c_str(),
               GetTickCount64() - fallbackStart);
        return;
    }

    WaitForModifierKeysReleased();
    ForceForeground(explorerHwnd);
    SendSimpleKey(VK_F2);

    Wh_Log(L"Rename via fallback UIA+F2, cost=%llums",
           GetTickCount64() - fallbackStart);
}

// ---------------- Inline rename capture (inline) ----------------
// Read the text of the focused inline-rename EDIT box via UIA. Returns true and
// fills outText only while an EDIT control that BELONGS TO `owner` is focused.
//
// #6: the focused EDIT must be scoped to the originating Explorer window. If we
// only checked "focused control is an EDIT", then when focus drifts to another
// application mid-rename that app's EDIT text would be captured as the folder
// name. We verify the EDIT's native window is `owner` or a descendant of it and
// reject anything else.
static bool UIA_ReadFocusedEditText(std::wstring& outText, HWND owner) {
    if (!EnsureUIA()) {
        return false;
    }
    IUIAutomationElement* focused = nullptr;
    if (FAILED(g_uia->GetFocusedElement(&focused)) || !focused) {
        return false;
    }
    CONTROLTYPEID ct = 0;
    bool isEdit = SUCCEEDED(focused->get_CurrentControlType(&ct)) &&
                  ct == UIA_EditControlTypeId;
    if (!isEdit) {
        SafeRelease(focused);
        return false;
    }
    // #6: scope the EDIT to the originating Explorer window. Reject a focused
    // EDIT that lives in some other top-level window (another app that grabbed
    // focus during the rename).
    if (owner) {
        UIA_HWND editHwnd = nullptr;
        HWND edit = SUCCEEDED(focused->get_CurrentNativeWindowHandle(&editHwnd))
                        ? reinterpret_cast<HWND>(editHwnd)
                        : nullptr;
        bool belongs = edit && (GetAncestor(edit, GA_ROOT) ==
                                    GetAncestor(owner, GA_ROOT) ||
                                IsDescendantOrSelf(owner, edit));
        if (!belongs) {
            SafeRelease(focused);
            return false;
        }
    }
    bool got = false;
    IUnknown* unk = nullptr;
    if (SUCCEEDED(focused->GetCurrentPattern(UIA_ValuePatternId, &unk)) && unk) {
        IUIAutomationValuePattern* vp = nullptr;
        if (SUCCEEDED(unk->QueryInterface(IID_PPV_ARGS(&vp))) && vp) {
            BSTR bstr = nullptr;
            if (SUCCEEDED(vp->get_CurrentValue(&bstr)) && bstr) {
                outText.assign(bstr, SysStringLen(bstr));
                got = true;
                SysFreeString(bstr);
            }
            SafeRelease(vp);
        }
        SafeRelease(unk);
    }
    SafeRelease(focused);
    return got;
}

// Watch the inline rename until the EDIT box closes (Enter / focus loss) or we
// time out. outBase receives the last text seen. Returns true if an edit box
// was ever observed.
// Terminal outcome of watching the inline rename EDIT. Mirrors RenameOutcome in
// tests/capture_logic.h and is gated by ShouldUseCapturedName() (see
// tests/rename_outcome_test): only a real commit may create folders. Do NOT
// infer commit from "the EDIT disappeared" -- Esc dismisses it exactly like
// Enter, and treating that as a commit is the multi-folder "Esc still creates"
// regression.
enum class InlineRenameOutcome {
    NeverStarted,        // the rename EDIT never appeared
    Committed,           // user pressed Enter -> use the typed name
    FocusLostCommitted,  // focus left the EDIT with text intact -> commit
    Cancelled,           // user pressed Esc -> discard, cancel
    Timeout,             // watcher timed out -> discard, cancel
};

// True only when the captured name may be used to create folders (mirror of
// ShouldUseCapturedName in tests/capture_logic.h).
static bool ShouldUseCapturedName(InlineRenameOutcome outcome) {
    return outcome == InlineRenameOutcome::Committed ||
           outcome == InlineRenameOutcome::FocusLostCommitted;
}

// Watch the inline rename EDIT and report BOTH the last text seen and WHY the
// rename ended. Esc is detected explicitly (GetAsyncKeyState) so a cancel is
// never misread as a commit. Enter with text present -> Committed; the EDIT
// vanishing with text still present -> FocusLostCommitted; the whole window
// elapsing -> Timeout; the EDIT never appearing -> NeverStarted.
static InlineRenameOutcome WaitForInlineRenameResult(std::wstring& outBase,
                                                     HWND owner, int timeoutMs) {
    ULONGLONG start = GetTickCount64();
    bool sawEdit = false;
    int missAfterSeen = 0;
    std::wstring last;
    InlineRenameOutcome outcome = InlineRenameOutcome::Timeout;

    // #6: the hook-thread Esc latch is ARMED BY THE CALLER (ArmInlineEscLatch)
    // *before* StartRename, so an extremely fast Esc pressed the instant the
    // rename edit appears -- before this watch loop begins -- is still latched
    // by the hook. Do NOT clear g_inlineEscPressed here: that would erase such
    // an early press. We only (re)assert the active flag defensively, in case
    // the latch was never armed by the caller for some path.
    InterlockedExchange(&g_inlineRenameActive, 1);

    while (GetTickCount64() - start < (ULONGLONG)timeoutMs) {
        // #7: bail promptly on unload instead of blocking teardown for the full
        // timeout. Treat it as a cancel: discard the text, create nothing.
        if (AtomicGet(&g_unloading) != 0) {
            outcome = InlineRenameOutcome::Cancelled;
            last.clear();
            break;
        }

        // Esc cancels the rename: discard whatever was typed and stop. Checked
        // first so a cancel is never misclassified as a focus-lost commit.
        // #6: honour BOTH the hook's latched flag (catches a fast Esc between
        // polls) and the live key state (Esc still physically down now).
        if (AtomicGet(&g_inlineEscPressed) != 0 ||
            (GetAsyncKeyState(VK_ESCAPE) & 0x8000)) {
            outcome = sawEdit ? InlineRenameOutcome::Cancelled
                              : InlineRenameOutcome::NeverStarted;
            last.clear();
            break;
        }

        std::wstring text;
        if (UIA_ReadFocusedEditText(text, owner)) {
            sawEdit = true;
            missAfterSeen = 0;
            last = text;

            // Enter commits the rename with the current text.
            if (GetAsyncKeyState(VK_RETURN) & 0x8000) {
                outcome = InlineRenameOutcome::Committed;
                break;
            }
        } else if (sawEdit) {
            // The EDIT went away without an explicit Esc/Enter: Explorer
            // committed the rename on focus loss (click-away). The text we
            // last read is the committed name.
            if (++missAfterSeen >= 2) {
                outcome = InlineRenameOutcome::FocusLostCommitted;
                break;
            }
        }
        Sleep(60);
    }

    // #6: also honour a latched Esc that landed after the loop's last check
    // (e.g. right as the EDIT vanished) so it is never reclassified as a commit.
    if (AtomicGet(&g_inlineEscPressed) != 0) {
        outcome = sawEdit ? InlineRenameOutcome::Cancelled
                          : InlineRenameOutcome::NeverStarted;
        last.clear();
    } else if (!sawEdit) {
        outcome = InlineRenameOutcome::NeverStarted;
    }

    // #6: disarm the latch so it can't leak into a later, unrelated watch.
    InterlockedExchange(&g_inlineRenameActive, 0);
    InterlockedExchange(&g_inlineEscPressed, 0);

    outBase = last;
    return outcome;
}

// ---------------- Popup name capture (hybrid: dialog mode) ----------------
// Detect Windows "apps" dark mode from the registry (AppsUseLightTheme == 0).
static bool IsDarkMode() {
    DWORD val = 1;
    DWORD sz = sizeof(val);
    HKEY key = nullptr;
    if (RegOpenKeyExW(HKEY_CURRENT_USER,
            L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
            0, KEY_READ, &key) == ERROR_SUCCESS) {
        RegQueryValueExW(key, L"AppsUseLightTheme", nullptr, nullptr,
                         reinterpret_cast<LPBYTE>(&val), &sz);
        RegCloseKey(key);
    }
    return val == 0;
}

// True on Windows 11 (build >= 22000), where the acrylic backdrop is available.
static bool IsWin11() {
    HMODULE nt = GetModuleHandleW(L"ntdll.dll");
    if (!nt) {
        return false;
    }
    typedef LONG(WINAPI * PFN_RtlGetVersion)(OSVERSIONINFOW*);
    PFN_RtlGetVersion rtl = reinterpret_cast<PFN_RtlGetVersion>(
        GetProcAddress(nt, "RtlGetVersion"));
    if (!rtl) {
        return false;
    }
    OSVERSIONINFOW vi = {0};
    vi.dwOSVersionInfoSize = sizeof(vi);
    if (rtl(&vi) != 0) {
        return false;
    }
    return vi.dwMajorVersion > 10 ||
           (vi.dwMajorVersion == 10 && vi.dwBuildNumber >= 22000);
}

// Clean Segoe UI font (replaces the pixelated default System font).
static HFONT g_uiFont = nullptr;
static HFONT UiFont() {
    if (!g_uiFont) {
        g_uiFont = CreateFontW(-17, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                               DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
                               CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
                               DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");
    }
    return g_uiFont;
}

// ---- Popup theme palette (mirrors tests/theme_logic.h) ----
// One source of truth for every popup colour in light and dark mode. The
// WndProc colour handlers (WM_CTLCOLORSTATIC/EDIT) and the owner-draw buttons
// (WM_DRAWITEM) all read from here, so re-tuning a theme means editing exactly
// one function -- and its mirror in the tests. Keeping these in lockstep is
// what "compatible with light AND dark mode" means: no hard-coded colour is
// left behind in a handler where only one theme was considered.
struct ThemePalette {
    COLORREF panelBg;           // window / panel background
    COLORREF panelText;         // label text on the panel
    COLORREF editBg;            // edit-box background
    COLORREF editText;          // edit-box text
    COLORREF createFill;        // Create button fill (accent)
    COLORREF createFillPressed; // Create button fill while pressed
    COLORREF createText;        // Create button caption
    COLORREF createBorder;      // Create button border
    COLORREF cancelFill;        // Cancel button fill
    COLORREF cancelFillPressed; // Cancel button fill while pressed
    COLORREF cancelText;        // Cancel button caption
    COLORREF cancelBorder;      // Cancel button border
};

// ---- Design colour tokens ----
// Named colour tokens so NO popup colour is a bare hard-coded RGB() at the
// point of use. ResolveThemePalette() maps each semantic slot to one of these
// tokens; the mirror in tests/theme_logic.h defines the SAME token set/values.
static const COLORREF kColWhite            = RGB(255, 255, 255);
// Accent (Create) -- shared across both themes.
static const COLORREF kColAccent           = RGB(0, 95, 184);   // #005FB8
static const COLORREF kColAccentPressed    = RGB(0, 78, 152);
// Light-theme neutrals.
static const COLORREF kColSurfaceLight     = RGB(243, 243, 243);
static const COLORREF kColTextLight        = RGB(26, 26, 26);
static const COLORREF kColInputLight       = RGB(255, 255, 255);
static const COLORREF kColInputTextLight   = RGB(20, 20, 20);
// Dark-theme neutrals.
static const COLORREF kColSurfaceDark      = RGB(32, 32, 32);
static const COLORREF kColTextDark         = RGB(240, 240, 240);
static const COLORREF kColInputDark        = RGB(84, 84, 84);    // #545454
static const COLORREF kColInputTextDark    = RGB(240, 240, 240);
// Danger (Cancel) reds -- per theme.
static const COLORREF kColDangerLight        = RGB(196, 43, 28); // #C42B1C
static const COLORREF kColDangerLightPressed = RGB(176, 34, 22);
static const COLORREF kColDangerLightBorder  = RGB(150, 32, 20);
static const COLORREF kColDangerDark         = RGB(160, 42, 34);
static const COLORREF kColDangerDarkPressed  = RGB(130, 34, 28);
static const COLORREF kColDangerDarkBorder   = RGB(110, 28, 22);

static ThemePalette ResolveThemePalette(bool dark) {
    ThemePalette p = {};
    // Create = the muted accent blue in BOTH themes (pressed darker); white
    // caption. Every assignment below is a token, not a raw colour.
    p.createFill = kColAccent;
    p.createFillPressed = kColAccentPressed;
    p.createText = kColWhite;
    p.createBorder = kColAccent;
    if (dark) {
        p.panelBg = kColSurfaceDark;
        p.panelText = kColTextDark;
        p.editBg = kColInputDark;
        p.editText = kColInputTextDark;
        p.cancelFill = kColDangerDark;
        p.cancelFillPressed = kColDangerDarkPressed;
        p.cancelText = kColWhite;
        p.cancelBorder = kColDangerDarkBorder;
    } else {
        p.panelBg = kColSurfaceLight;
        p.panelText = kColTextLight;
        p.editBg = kColInputLight;
        p.editText = kColInputTextLight;
        p.cancelFill = kColDangerLight;
        p.cancelFillPressed = kColDangerLightPressed;
        p.cancelText = kColWhite;
        p.cancelBorder = kColDangerLightBorder;
    }
    return p;
}

// Small fixed cache of solid brushes keyed by colour, so no colour handler
// leaks a brush or re-creates one per WM_CTLCOLOR message. Freed at unload.
static HBRUSH g_themeBrushes[8] = {0};
static COLORREF g_themeBrushColors[8] = {0};
static HBRUSH ThemeBrush(COLORREF color) {
    for (int i = 0; i < 8; ++i) {
        if (g_themeBrushes[i] && g_themeBrushColors[i] == color) {
            return g_themeBrushes[i];
        }
    }
    for (int i = 0; i < 8; ++i) {
        if (!g_themeBrushes[i]) {
            g_themeBrushes[i] = CreateSolidBrush(color);
            g_themeBrushColors[i] = color;
            return g_themeBrushes[i];
        }
    }
    // Cache full (only two palettes x a few roles are ever requested, so this is
    // not expected in practice). Evict slot 0 rather than returning an UNTRACKED
    // brush: an untracked CreateSolidBrush() is never recorded in
    // g_themeBrushes[] and so FreeThemeBrushes() can never DeleteObject() it --
    // a GDI handle leak for the process lifetime. Reusing a tracked slot keeps
    // every brush we hand out owned by the cache and freed on unload.
    DeleteObject(g_themeBrushes[0]);
    g_themeBrushes[0] = CreateSolidBrush(color);
    g_themeBrushColors[0] = color;
    return g_themeBrushes[0];
}

static void FreeThemeBrushes() {
    for (int i = 0; i < 8; ++i) {
        if (g_themeBrushes[i]) {
            DeleteObject(g_themeBrushes[i]);
            g_themeBrushes[i] = nullptr;
            g_themeBrushColors[i] = 0;
        }
    }
}

// Solid panel background (used when the acrylic backdrop is unavailable).
static HBRUSH PanelBrush(bool dark) {
    return ThemeBrush(ResolveThemePalette(dark).panelBg);
}

// Rounded corners + dark title bar + acrylic backdrop (all best-effort; any
// attribute the OS does not support is silently ignored).
static void ApplyWindowPolish(HWND hwnd, bool dark, bool glass) {
    HMODULE dwm = LoadLibraryW(L"dwmapi.dll");
    if (!dwm) {
        return;
    }
    typedef HRESULT(WINAPI * PFN_Set)(HWND, DWORD, LPCVOID, DWORD);
    PFN_Set set =
        reinterpret_cast<PFN_Set>(GetProcAddress(dwm, "DwmSetWindowAttribute"));
    if (set) {
        BOOL d = dark ? TRUE : FALSE;
        set(hwnd, 20, &d, sizeof(d));  // DWMWA_USE_IMMERSIVE_DARK_MODE
        set(hwnd, 19, &d, sizeof(d));  // older build fallback
        DWORD round = 2;               // DWMWCP_ROUND
        set(hwnd, 33, &round, sizeof(round));
        if (glass) {
            DWORD backdrop = 3;        // DWMSBT_TRANSIENTWINDOW (acrylic)
            set(hwnd, 38, &backdrop, sizeof(backdrop));
        }
    }
    FreeLibrary(dwm);
}

// Force classic (non-themed) painting on a control so it honours the colours we
// hand back from WM_CTLCOLOR*. A themed EDIT in dark mode otherwise IGNORES the
// brush we return and paints its background white (#FFFFFF) -- disabling its
// theme is what makes the dark #545454 input background actually stick.
// Dynamic-loaded (like dwmapi) so there is no extra link-time dependency.
static void DisableWindowTheme(HWND h) {
    HMODULE ux = LoadLibraryW(L"uxtheme.dll");
    if (!ux) {
        return;
    }
    typedef HRESULT(WINAPI * PFN_SetWindowTheme)(HWND, LPCWSTR, LPCWSTR);
    PFN_SetWindowTheme swt = reinterpret_cast<PFN_SetWindowTheme>(
        GetProcAddress(ux, "SetWindowTheme"));
    if (swt) {
        swt(h, L"", L"");
    }
    FreeLibrary(ux);
}

struct NamePromptData {
    std::wstring base;
    bool accepted = false;
    bool glass = false;          // acrylic backdrop REQUESTED (Win11)
    bool acrylicActive = false;  // acrylic backdrop CONFIRMED compositing
    HWND edit = nullptr;
};

static LRESULT CALLBACK NamePromptWndProc(HWND hwnd, UINT msg, WPARAM wParam,
                                          LPARAM lParam) {
    NamePromptData* d = reinterpret_cast<NamePromptData*>(
        GetWindowLongPtrW(hwnd, GWLP_USERDATA));

    switch (msg) {
        case WM_CREATE: {
            CREATESTRUCTW* cs = reinterpret_cast<CREATESTRUCTW*>(lParam);
            d = reinterpret_cast<NamePromptData*>(cs->lpCreateParams);
            SetWindowLongPtrW(hwnd, GWLP_USERDATA,
                              reinterpret_cast<LONG_PTR>(d));
            HINSTANCE hinst = HINST_THISCOMPONENT;

            HWND label = CreateWindowExW(0, L"STATIC", L"Enter your folder name",
                            WS_CHILD | WS_VISIBLE,
                            20, 18, 304, 22, hwnd, nullptr, hinst, nullptr);
            d->edit = CreateWindowExW(
                0, L"EDIT", L"",
                WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_AUTOHSCROLL,
                20, 46, 304, 30, hwnd,
                reinterpret_cast<HMENU>(101), hinst, nullptr);
            HWND ok = CreateWindowExW(0, L"BUTTON", L"Create",
                            WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_OWNERDRAW,
                            170, 92, 74, 30, hwnd,
                            reinterpret_cast<HMENU>(IDOK), hinst, nullptr);
            HWND cancel = CreateWindowExW(0, L"BUTTON", L"Cancel",
                            WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_OWNERDRAW,
                            250, 92, 74, 30, hwnd,
                            reinterpret_cast<HMENU>(IDCANCEL), hinst, nullptr);

            HFONT font = UiFont();
            SendMessageW(label, WM_SETFONT, reinterpret_cast<WPARAM>(font), TRUE);
            SendMessageW(d->edit, WM_SETFONT, reinterpret_cast<WPARAM>(font), TRUE);
            SendMessageW(ok, WM_SETFONT, reinterpret_cast<WPARAM>(font), TRUE);
            SendMessageW(cancel, WM_SETFONT, reinterpret_cast<WPARAM>(font), TRUE);

            // Rounded corners on the input field AND both buttons. Clipping
            // each control HWND to a round-rect region is what removes the
            // corner artifact: the square corners fall OUTSIDE the region, so
            // the owner-draw fill can never leave an unpainted/black corner.
            SetWindowRgn(d->edit, CreateRoundRectRgn(0, 0, 305, 31, 12, 12), TRUE);
            SetWindowRgn(ok, CreateRoundRectRgn(0, 0, 75, 31, 14, 14), TRUE);
            SetWindowRgn(cancel, CreateRoundRectRgn(0, 0, 75, 31, 14, 14), TRUE);
            // Keep the caret/text off the rounded edge of the input field.
            SendMessageW(d->edit, EM_SETMARGINS,
                         EC_LEFTMARGIN | EC_RIGHTMARGIN, MAKELPARAM(10, 10));
            // Detheme the edit so the dark #545454 background actually applies
            // (a themed edit ignores WM_CTLCOLOREDIT and stays white).
            DisableWindowTheme(d->edit);

            SetFocus(d->edit);
            return 0;
        }
        case WM_ERASEBKGND: {
            // Only skip the solid fill when the acrylic backdrop is CONFIRMED to
            // be compositing. Requesting glass (d->glass) is NOT the same as it
            // working: with transparency effects off / DWM rejecting the
            // backdrop / remote sessions, nothing composited and the panel
            // showed through WHITE in dark mode. Painting the solid themed panel
            // is always safe -- real acrylic simply composites over the fill.
            bool acrylicConfirmed = d && d->glass && d->acrylicActive;
            if (acrylicConfirmed) {
                return 1;  // real acrylic will show through; skip the solid fill
            }
            HDC hdc = reinterpret_cast<HDC>(wParam);
            RECT rc;
            GetClientRect(hwnd, &rc);
            FillRect(hdc, &rc, PanelBrush(IsDarkMode()));
            return 1;
        }
        case WM_CTLCOLORSTATIC: {
            HDC hdc = reinterpret_cast<HDC>(wParam);
            ThemePalette pal = ResolveThemePalette(IsDarkMode());
            SetTextColor(hdc, pal.panelText);
            SetBkMode(hdc, TRANSPARENT);
            // Match WM_ERASEBKGND: a hollow label brush only makes sense over a
            // real acrylic backdrop. When we painted a solid panel, hand back the
            // matching solid brush so the label background is the themed surface
            // (not a stale/white one).
            bool acrylicConfirmed = d && d->glass && d->acrylicActive;
            if (acrylicConfirmed) {
                return reinterpret_cast<LRESULT>(GetStockObject(HOLLOW_BRUSH));
            }
            return reinterpret_cast<LRESULT>(ThemeBrush(pal.panelBg));
        }
        case WM_CTLCOLOREDIT: {
            HDC hdc = reinterpret_cast<HDC>(wParam);
            ThemePalette pal = ResolveThemePalette(IsDarkMode());
            SetTextColor(hdc, pal.editText);
            SetBkColor(hdc, pal.editBg);
            return reinterpret_cast<LRESULT>(ThemeBrush(pal.editBg));
        }
        case WM_DRAWITEM: {
            DRAWITEMSTRUCT* dis = reinterpret_cast<DRAWITEMSTRUCT*>(lParam);
            if (dis->CtlType == ODT_BUTTON &&
                (dis->CtlID == IDOK || dis->CtlID == IDCANCEL)) {
                bool pressed = (dis->itemState & ODS_SELECTED) != 0;
                bool isCreate = (dis->CtlID == IDOK);
                RECT rc = dis->rcItem;
                HDC hdc = dis->hDC;

                ThemePalette pal = ResolveThemePalette(IsDarkMode());
                COLORREF fill;
                COLORREF txt;
                COLORREF border;
                if (isCreate) {
                    fill = pressed ? pal.createFillPressed : pal.createFill;
                    txt = pal.createText;
                    border = pal.createBorder;
                } else {
                    fill = pressed ? pal.cancelFillPressed : pal.cancelFill;
                    txt = pal.cancelText;
                    border = pal.cancelBorder;
                }

                // The button HWND is clipped to a rounded region (set at
                // create), so fill the WHOLE item rect -- only the rounded area
                // shows and there is no unpainted corner artifact -- then trace
                // a matching rounded border with a hollow brush for definition.
                HBRUSH b = CreateSolidBrush(fill);
                FillRect(hdc, &rc, b);
                DeleteObject(b);

                HPEN pen = CreatePen(PS_SOLID, 1, border);
                HGDIOBJ op = SelectObject(hdc, pen);
                HGDIOBJ ob = SelectObject(hdc, GetStockObject(HOLLOW_BRUSH));
                RoundRect(hdc, rc.left, rc.top, rc.right, rc.bottom, 14, 14);
                SelectObject(hdc, ob);
                SelectObject(hdc, op);
                DeleteObject(pen);

                wchar_t caption[32] = {0};
                GetWindowTextW(dis->hwndItem, caption, ARRAYSIZE(caption));
                SetBkMode(hdc, TRANSPARENT);
                SetTextColor(hdc, txt);
                HGDIOBJ of = SelectObject(hdc, UiFont());
                DrawTextW(hdc, caption, -1, &rc,
                          DT_CENTER | DT_VCENTER | DT_SINGLELINE);
                SelectObject(hdc, of);
                return TRUE;
            }
            break;
        }
        case WM_COMMAND: {
            WORD id = LOWORD(wParam);
            if (id == IDOK && d) {
                wchar_t buf[512] = {0};
                GetWindowTextW(d->edit, buf, ARRAYSIZE(buf));
                d->base = buf;
                d->accepted = true;
                DestroyWindow(hwnd);
                return 0;
            }
            if (id == IDCANCEL) {
                if (d) {
                    d->accepted = false;
                }
                DestroyWindow(hwnd);
                return 0;
            }
            break;
        }
        case WM_CLOSE:
            DestroyWindow(hwnd);
            return 0;
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

// The whole trigger context posted to the worker via WM_APP+1. Declared here
// (above PromptForFolderName) because the popup's own modal loop shares the
// worker queue and can dequeue a WM_APP+1; when it does it must FREE the heap
// action carried in lParam, so it needs the full type -- not just a forward
// declaration.
struct PendingFolderAction {
    int requested = 0;          // count requested at trigger time
    HWND origin = nullptr;      // origin Explorer window (#1)
    HWND tab = nullptr;         // active shell tab at trigger time (#3)
    bool snapExpected = false;  // #2: WM_APP+2 snapshot POST succeeded for this
                                // action -> drift protection is available. If the
                                // post FAILED this stays false and the worker
                                // safe-cancels rather than running unprotected.
    ULONGLONG keypressTick = 0; // #2 (true trigger-time capture): GetTickCount64()
                                // captured on the hook thread WHEN THE SHORTCUT
                                // FIRED. Paired on the worker with the snapshot's
                                // resolve tick to prove the browse-dir snapshot
                                // was taken close enough to the keypress to be a
                                // real trigger-time capture (see
                                // kSnapshotFreshWindowMs). A late resolve (worker
                                // was busy) can't be trusted -> safe-cancel.
    wchar_t templateKey = 0;    // Folder Templates: 0 = normal count action;
                                // non-zero = template action keyed by this
                                // lowercased letter (worker looks it up in
                                // g_templates and ignores `requested`).
};

// Free a PendingFolderAction* carried in an lParam and clear the post-guard so a
// fresh trigger can queue another action. Used wherever a WM_APP+1 is CONSUMED
// (dropped) rather than acted on, so the heap allocation from
// PostFolderActionCount() is never leaked.
static void DiscardFolderAction(LPARAM lParam) {
    delete reinterpret_cast<PendingFolderAction*>(lParam);
    InterlockedExchange(&g_actionPosted, 0);
}

// Show the minimal modal prompt centered on Explorer. Returns true if the user
// pressed Create; fills outBase (the count is already known from the trigger).
// No PostQuitMessage -> the worker thread's own message loop is left intact.
static bool PromptForFolderName(HWND owner, std::wstring& outBase) {
    static const wchar_t* kClass = L"WhNewFolderHybridPrompt";
    static bool registered = false;
    if (!registered) {
        WNDCLASSEXW wc = {};
        wc.cbSize = sizeof(wc);
        wc.lpfnWndProc = NamePromptWndProc;
        wc.hInstance = HINST_THISCOMPONENT;
        wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
        wc.hbrBackground = nullptr;  // painted in WM_ERASEBKGND
        wc.lpszClassName = kClass;
        // Hardening: only latch `registered` when the class is actually usable.
        // ERROR_CLASS_ALREADY_EXISTS means a prior registration is still good.
        if (RegisterClassExW(&wc) != 0 ||
            GetLastError() == ERROR_CLASS_ALREADY_EXISTS) {
            registered = true;
        } else {
            Wh_Log(L"RegisterClassExW failed (error=%lu); popup unavailable.",
                   GetLastError());
            return false;
        }
    }

    NamePromptData data;
    data.glass = IsWin11();

    const int w = 344;
    const int h = 178;
    int x = CW_USEDEFAULT, y = CW_USEDEFAULT;
    RECT rc;
    if (owner && GetWindowRect(owner, &rc)) {
        x = rc.left + ((rc.right - rc.left) - w) / 2;
        y = rc.top + ((rc.bottom - rc.top) - h) / 2;
    }

    HWND hwnd = CreateWindowExW(
        WS_EX_DLGMODALFRAME | WS_EX_TOPMOST, kClass, L"New folder",
        WS_POPUP | WS_CAPTION | WS_SYSMENU,
        x, y, w, h, owner, nullptr, HINST_THISCOMPONENT, &data);
    if (!hwnd) {
        return false;
    }

    ApplyWindowPolish(hwnd, IsDarkMode(), data.glass);
    // acrylicActive stays false: requesting the backdrop does not guarantee it
    // composites (transparency effects off, remote session, DWM refusal). We
    // therefore always paint the solid themed panel in WM_ERASEBKGND -- if real
    // acrylic is active it simply composites over that fill, and if it is not we
    // still get the correct #202020 (dark) / #F3F3F3 (light) panel instead of a
    // bare white window. Flip this to true only behind a confirmed-acrylic probe.
    data.acrylicActive = false;

    if (owner) {
        EnableWindow(owner, FALSE);
    }
    ShowWindow(hwnd, SW_SHOW);
    if (!SetForegroundWindow(hwnd)) {
        // Foreground-lock can refuse this; SetFocus below still gives the edit
        // keyboard focus within our (enabled) window, so the prompt is usable.
        Wh_Log(L"PromptForFolderName: SetForegroundWindow(popup) failed "
               L"(error=%lu).", GetLastError());
    }
    SetFocus(data.edit);

    // Self-contained modal loop. Enter = Create, Esc = Cancel. No
    // PostQuitMessage, so the worker thread's outer loop is untouched.
    //
    // #1 (queue ownership): this loop shares the WORKER thread's message queue,
    // so it can dequeue the worker's own thread messages (msg.hwnd == nullptr):
    //   * WM_APP     -- Worker_Exit's shutdown signal. If we swallowed it the
    //                   worker would never quit and unload would hang. Re-post
    //                   it and CLOSE the popup so the outer loop handles quit.
    //   * WM_APP + 1 -- a second folder action posted while this popup is open.
    //                   CONSUME it: this popup owns the queue, so re-posting it
    //                   here would be re-dequeued on the very next GetMessageW
    //                   and spin the CPU (re-post -> receive -> re-post ...).
    //                   The action can't run nested anyway, so drop it and just
    //                   clear g_actionPosted, freeing the next trigger to post
    //                   again after the popup closes. WM_APP (shutdown) is
    //                   different: it must reach the outer loop, so re-post it
    //                   and break out of the modal loop.
    // We never DISPATCH a worker thread message from here -- only the outer
    // WorkerThread loop owns them.
    MSG msg;
    while (IsWindow(hwnd) && GetMessageW(&msg, nullptr, 0, 0) > 0) {
        if (msg.hwnd == nullptr && msg.message == WM_APP) {
            // Shutdown requested: re-post so the outer worker loop can quit,
            // then stop the modal loop.
            PostThreadMessageW(GetCurrentThreadId(), msg.message,
                               msg.wParam, msg.lParam);
            break;
        }
        if (msg.hwnd == nullptr && msg.message == WM_APP + 2) {
            // A trigger-time dir snapshot (#2) posted while this popup is open.
            // The action it belongs to will be consumed below (WM_APP+1) and
            // never acted on, so just drop the snapshot request; leaving it to
            // fall through would dispatch a thread message to no window.
            continue;
        }
        if (msg.hwnd == nullptr && msg.message == WM_APP + 1) {
            // Stale/duplicate action while this popup is open: consume it,
            // never re-post (that busy-loops). FREE the heap PendingFolderAction
            // carried in lParam (it was new'd in PostFolderActionCount and is
            // never dequeued by the worker once we drop it here) and clear the
            // post-guard so a fresh trigger can queue an action once the popup
            // closes. Dropping without deleting leaked one allocation per
            // shortcut press while the popup was open.
            DiscardFolderAction(msg.lParam);
            continue;
        }
        if (msg.message == WM_KEYDOWN && msg.wParam == VK_RETURN) {
            SendMessageW(hwnd, WM_COMMAND, IDOK, 0);
            continue;
        }
        if (msg.message == WM_KEYDOWN && msg.wParam == VK_ESCAPE) {
            SendMessageW(hwnd, WM_COMMAND, IDCANCEL, 0);
            continue;
        }
        if (!IsDialogMessageW(hwnd, &msg)) {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
    }

    if (owner) {
        EnableWindow(owner, TRUE);
        SetForegroundWindow(owner);
    }

    if (data.accepted) {
        outBase = data.base;
        return true;
    }
    return false;
}

// #1: the origin Explorer target, captured on the HOOK thread at trigger time
// and carried through WM_APP+1 so the worker acts on the window that was
// actually focused when the shortcut fired -- not on whatever is foreground
// hundreds of ms later (the multi-folder count timer alone is up to 1200ms).
// Only touched on the hook thread.
static HWND g_triggerTarget = nullptr;

// #3: the active shell TAB captured alongside g_triggerTarget at trigger time.
// The worker requires the active tab to be unchanged before creating folders,
// so switching tabs in the same Explorer window during the wait cancels the
// action instead of creating folders in the wrong tab. Only touched on the hook
// thread; snapshotted into the queued PendingFolderAction at post time (#2),
// so the worker never reads this global -- it reads its own action's copy.
static HWND g_triggerTab = nullptr;

// #2 (directory race): whether the WM_APP+2 snapshot POST succeeded for the most
// recent trigger. Set in TriggerFired, copied into the queued action by
// PostFolderActionCount, and consumed by the worker: a false value means this
// action has no drift protection, so the worker safe-cancels. Hook-thread only.
static bool g_triggerSnapPosted = false;

// #2 (true trigger-time capture): GetTickCount64() captured on the hook thread at
// the instant the shortcut fires. Carried into the queued PendingFolderAction and
// compared on the worker against g_snapResolveTick, so a snapshot resolved too
// long after the keypress (worker busy -> possible same-tab drift before capture)
// is not trusted. Hook-thread only.
static ULONGLONG g_triggerTick = 0;

// #2 (directory race): the browsing directory the user was in WHEN THE SHORTCUT
// FIRED, resolved on the worker via a WM_APP+2 snapshot posted from TriggerFired
// -- BEFORE count entry runs. Navigating the same tab during count entry (which
// keeps the tab HWND unchanged, so the #3 tab guard alone can't catch it) no
// longer moves the folders: the worker creates them in this snapshot dir, not in
// wherever the tab drifted to by the time WM_APP+1 is acted on.
//
// These three are WORKER-THREAD-LOCAL: both WM_APP+2 (snapshot) and WM_APP+1
// (act) are dispatched by the same worker message loop, serially, so there is no
// cross-thread access and no lock is needed. The hook thread only POSTS the
// messages (origin/tab travel in wParam/lParam), never touching this state.
static std::wstring g_snapDir;          // browse dir at trigger time
static HWND         g_snapOrigin = nullptr;  // origin the snapshot was taken for
static HWND         g_snapTab = nullptr;     // tab the snapshot was taken for
// #2 (true trigger-time capture): GetTickCount64() at the moment SnapshotTriggerDir
// actually RESOLVED g_snapDir on the worker. Compared on the worker against the
// keypress tick carried in PendingFolderAction to reject a snapshot that resolved
// too late to reflect keypress-time reality. Worker-thread-local; paired with
// g_snapDir (cleared alongside it).
static ULONGLONG    g_snapResolveTick = 0;

// #2 (true trigger-time capture): maximum tolerated latency between the keypress
// (stamped on the hook thread) and the worker actually resolving the browse-dir
// snapshot for it. A deliberate same-tab navigation takes far longer than this,
// so a snapshot resolved within the window reliably reflects the keypress-time
// directory; one resolved later (the worker was busy, so the user had time to
// navigate BEFORE the snapshot ran) cannot be trusted as a trigger-time capture
// and the action is cancelled. Chosen well below human navigation time and
// comfortably above an idle worker's COM resolve cost.
static const ULONGLONG kSnapshotFreshWindowMs = 250;

// Resolve and cache the current browsing directory for (origin, tab). Called on
// the worker thread from the WM_APP+2 handler at trigger time, so the captured
// path reflects where the user was before any count-entry navigation.
static void SnapshotTriggerDir(HWND origin, HWND tab) {
    // #2 (true trigger-time capture): stamp WHEN the worker actually reached this
    // snapshot. If the worker was busy the stamp lands well after the keypress;
    // the WM_APP+1 handler compares the two ticks and rejects a stale snapshot.
    // Recorded on entry so every early-return path below still carries it.
    g_snapResolveTick = GetTickCount64();
    g_snapDir.clear();
    g_snapOrigin = origin;
    g_snapTab = tab;

    if (!origin || !IsWindow(origin)) {
        return;
    }

    if (origin == GetShellWindow()) {
        g_snapDir = GetDesktopDir();
        return;
    }

    std::wstring path;
    IShellView* view = nullptr;
    if (ResolveActiveShellView(origin, path, &view) && !path.empty()) {
        g_snapDir = path;
    }
    SafeRelease(view);
}

// ---------------- Open-after-create navigation ----------------
// "Create and open" (single-folder only): after ONE folder is created, browse
// the active tab INTO it, in the SAME window (SBSP_SAMEBROWSER) -- never a new
// window. Mirrors the ResolveActiveShellView traversal but calls the browser's
// BrowseObject instead of reading the view's path. Best-effort: any failure
// just leaves the user where they were (the folder is still created + selected).
static bool NavigateActiveTabToFolder(HWND explorerHwnd,
                                      HWND capturedTab,
                                      const std::wstring& fullPath) {
    if (!explorerHwnd || !IsWindow(explorerHwnd) || fullPath.empty()) {
        return false;
    }
    // The Desktop (shell window) has no browser to navigate; skip.
    if (explorerHwnd == GetShellWindow()) {
        return false;
    }

    PIDLIST_ABSOLUTE pidl = nullptr;
    SFGAOF sf = 0;
    HRESULT hr = SHParseDisplayName(fullPath.c_str(), nullptr, &pidl, 0, &sf);
    if (FAILED(hr) || !pidl) {
        return false;
    }

    HWND activeTab = GetActiveShellTabHwnd(explorerHwnd);

    IShellWindows* shellWindows = nullptr;
    hr = CoCreateInstance(CLSID_ShellWindows, nullptr, CLSCTX_ALL,
                          IID_PPV_ARGS(&shellWindows));
    if (FAILED(hr) || !shellWindows) {
        CoTaskMemFree(pidl);
        return false;
    }

    long count = 0;
    shellWindows->get_Count(&count);
    bool navigated = false;

    for (long i = count - 1; i >= 0 && !navigated; --i) {
        VARIANT v;
        VariantInit(&v);
        v.vt = VT_I4;
        v.lVal = i;

        IDispatch* dispatch = nullptr;
        if (FAILED(shellWindows->Item(v, &dispatch)) || !dispatch) {
            VariantClear(&v);
            continue;
        }

        IServiceProvider* sp = nullptr;
        hr = dispatch->QueryInterface(IID_PPV_ARGS(&sp));
        SafeRelease(dispatch);
        if (FAILED(hr) || !sp) {
            VariantClear(&v);
            continue;
        }

        IShellBrowser* browser = nullptr;
        hr = sp->QueryService(SID_STopLevelBrowser, IID_PPV_ARGS(&browser));
        SafeRelease(sp);
        if (FAILED(hr) || !browser) {
            VariantClear(&v);
            continue;
        }

        // Match this browser to the origin window (and the captured tab when we
        // have one), so we navigate the tab the shortcut fired in -- never some
        // other window.
        HWND browserHwnd = nullptr;
        if (SUCCEEDED(browser->GetWindow(&browserHwnd)) && browserHwnd) {
            HWND browserRoot = GetAncestor(browserHwnd, GA_ROOT);
            bool sameWindow = (browserRoot == explorerHwnd);
            bool sameTab = true;
            HWND wantTab = capturedTab ? capturedTab : activeTab;
            if (sameWindow && wantTab) {
                HWND browserTab = FindAncestorOrSelfByClass(
                    browserHwnd, L"ShellTabWindowClass", explorerHwnd);
                sameTab = (browserTab == wantTab);
            }
            if (sameWindow && sameTab) {
                hr = browser->BrowseObject(pidl, SBSP_SAMEBROWSER | SBSP_ABSOLUTE);
                navigated = SUCCEEDED(hr);
                if (!navigated) {
                    Wh_Log(L"Open-after-create: BrowseObject failed hr=0x%08X",
                           hr);
                }
            }
        }

        SafeRelease(browser);
        VariantClear(&v);
    }

    shellWindows->Release();
    CoTaskMemFree(pidl);
    return navigated;
}

// ---------------- Main action ----------------
// `origin` is the Explorer window captured on the hook thread at trigger time
// (#1) and carried through WM_APP+1. We act on THAT window, not on whatever is
// foreground now, and we re-validate it here (mirrors ShouldActOnTarget in
// tests/target_capture_test): a null, destroyed, or no-longer-Explorer target
// means the context we were triggered in is gone -> cancel instead of guessing.
static void PerformNewFolderAction(int requested, HWND origin,
                                   HWND capturedTab,
                                   const std::wstring& snapshotDir,
                                   bool snapshotExpected,
                                   bool snapshotFresh,
                                   wchar_t templateKey = 0) {
    if (InterlockedCompareExchange(&g_actionRunning, 1, 0) != 0) {
        return;
    }

    ULONGLONG totalStart = GetTickCount64();

    // #1: validate the captured origin target instead of re-resolving late.
    bool stillValid = origin && IsWindow(origin);
    bool stillExplorer = stillValid &&
        (IsExplorerTopLevel(origin) || origin == GetShellWindow());
    if (!origin || !stillValid || !stillExplorer) {
        Wh_Log(L"No explorer window detected (captured origin target is gone "
               L"or no longer Explorer); cancelling.");
        InterlockedExchange(&g_actionRunning, 0);
        return;
    }

    HWND target = origin;

    std::wstring dir;

    IShellView* activeView = nullptr;

    // #3: if a tab was identified at trigger time, require it to still be the
    // active tab now (mirrors ShouldActOnTab / tests/tab_safe_test). Switching
    // tabs in the same Explorer window during the wait must CANCEL, not create
    // folders in the tab the user moved to. A zero captured tab (Desktop /
    // single-tab shell) has no tab identity to drift, so we skip the check.
    // #2: capturedTab travels in the queued action (arg), not a shared global,
    // so a later gesture's tab can't overwrite the one this action must match.
    if (capturedTab != nullptr && target != GetShellWindow()) {
        HWND currentTab = GetActiveShellTabHwnd(target);
        if (currentTab != capturedTab) {
            Wh_Log(L"Active tab changed since trigger; cancelling, "
                   L"nothing created.");
            InterlockedExchange(&g_actionRunning, 0);
            return;
        }
    }

    if (target == GetShellWindow()) {
        dir = GetDesktopDir();
    } else if (!ResolveActiveShellView(target, dir, &activeView) || dir.empty()) {
        Wh_Log(L"Failed to resolve current active tab path.");
        InterlockedExchange(&g_actionRunning, 0);
        return;
    }

    // #2 + #3 unified safety rule (mirrors ShouldActOnDir in
    // tests/capture_logic.h, pinned by tests/dir_safe_test). Create ONLY when we
    // have a snapshot we can FULLY trust AND it still matches the live dir:
    //     snapshot POST succeeded             (snapshotExpected)          [#3]
    //   + snapshot resolved near the keypress (snapshotFresh)            [#2]
    //   + snapshot actually resolved a dir    (snapshotDir non-empty)    [#3]
    //   + current dir resolved                (dir non-empty)
    //   + snapshot dir == current dir         (no same-tab drift)        [#2]
    // Anything else CANCELS. We never fall back to "create in the current dir
    // without a verified snapshot".

    // #3 (post-failure hole): a failed WM_APP+2 post means no drift protection.
    if (!snapshotExpected) {
        Wh_Log(L"Trigger-dir snapshot was not requested/failed to post; "
               L"cancelling to avoid an unprotected action.");
        SafeRelease(activeView);
        InterlockedExchange(&g_actionRunning, 0);
        return;
    }

    // #2 (true trigger-time capture): the snapshot is resolved on the worker
    // (WM_APP+2), not at the keypress. If it resolved too long after the keypress
    // the worker was busy, so the user may have navigated the same tab BEFORE the
    // snapshot ran -- meaning the snapshot itself could hold the drifted-to dir
    // and the compare below would wrongly match. A stale snapshot is not a
    // trigger-time capture, so cancel rather than trust it.
    if (!snapshotFresh) {
        Wh_Log(L"Trigger-dir snapshot resolved too late to be a true "
               L"keypress-time capture (worker was busy); cancelling.");
        SafeRelease(activeView);
        InterlockedExchange(&g_actionRunning, 0);
        return;
    }

    // #3 (resolution-failure hole): "post succeeded" != "snapshot succeeded".
    // ResolveActiveShellView can fail and leave the snapshot empty even though the
    // post went through. Treat an unresolved snapshot (or an unresolvable current
    // dir) as unsafe and CANCEL -- do NOT silently proceed with the current dir
    // and no drift check.
    if (snapshotDir.empty() || dir.empty()) {
        Wh_Log(L"Trigger-dir snapshot did not resolve (post ok but empty "
               L"snapshot/current dir); cancelling to avoid an unverified "
               L"action.");
        SafeRelease(activeView);
        InterlockedExchange(&g_actionRunning, 0);
        return;
    }

    // #2 (directory race): if the captured browse dir DIFFERS from where the
    // active view resolves NOW, the user navigated the same tab during the wait
    // (the #3 tab guard can't see a same-tab nav). Creating in the snapshot dir
    // while `activeView` still points at the drifted-to dir would also drive the
    // inline rename against the WRONG live view. The safe, provable behavior is
    // to CANCEL -- create nothing.
    {
        std::wstring snap = snapshotDir;
        while (!snap.empty() && (snap.back() == L'\\' || snap.back() == L'/')) {
            snap.pop_back();
        }
        if (snap.empty() || _wcsicmp(snap.c_str(), dir.c_str()) != 0) {
            Wh_Log(L"Browse dir changed since trigger (was '%ls', now '%ls'); "
                   L"cancelling, nothing created.", snap.c_str(), dir.c_str());
            SafeRelease(activeView);
            InterlockedExchange(&g_actionRunning, 0);
            return;
        }
    }

    while (!dir.empty() && (dir.back() == L'\\' || dir.back() == L'/')) {
        dir.pop_back();
    }

    DWORD dirAttrs = GetFileAttributesW(dir.c_str());

    if (dirAttrs == INVALID_FILE_ATTRIBUTES ||
        !(dirAttrs & FILE_ATTRIBUTE_DIRECTORY)) {
        Wh_Log(L"Unsupported target directory: %ls", dir.c_str());
        SafeRelease(activeView);
        InterlockedExchange(&g_actionRunning, 0);
        return;
    }

    // ---- Folder Templates: a template letter fired instead of a count ----
    // The hook stayed thin (it only captured the letter char); the lookup and
    // creation happen HERE on the worker. `dir` above already passed the full
    // origin/tab/snapshot drift guard, so a template gets the same protection
    // as a normal batch. templateCollision governs collisions; autoResolveNames
    // and openAfterCreate are intentionally IGNORED for templates. No rename UI,
    // no auto-navigate.
    if (templateKey != 0) {
        std::vector<std::wstring> tmplFolders;
        LONG collision = TC_Skip;
        bool found = false;
        EnterCriticalSection(&g_templatesCs);
        for (const ResolvedTemplate& rt : g_templates) {
            if (rt.key == templateKey) {
                tmplFolders = rt.folders;
                found = true;
                break;
            }
        }
        collision = g_templateCollision;
        LeaveCriticalSection(&g_templatesCs);

        if (!found) {
            Wh_Log(L"No folder template bound to key '%c'; nothing created.",
                   templateKey);
            SafeRelease(activeView);
            InterlockedExchange(&g_actionRunning, 0);
            return;
        }

        // #2 (consistency): capture the numbering pattern ONCE for this whole
        // template action so the initial plan AND every race retry use the same
        // format -- a mid-action setting change can't split one action across
        // two patterns.
        const LONG tmplPattern = AtomicGet(&g_namingPattern);
        std::vector<TemplateFolderPlan> tmplPlans = ResolveTemplateFolders(
            dir, tmplFolders, tmplPattern, collision);
        // #8-1: folded paths this action still owns, so a create-time race retry
        // never steals a name reserved for another plan (which would force that
        // plan to renumber and break the intended template set).
        std::vector<std::wstring> tmplReserved;
        tmplReserved.reserve(tmplPlans.size());
        for (const TemplateFolderPlan& q : tmplPlans) {
            tmplReserved.push_back(ToLowerFold(q.path));
        }
        int tmplCreated = 0;
        for (const TemplateFolderPlan& plan : tmplPlans) {
            const std::wstring& p = plan.path;
            DWORD err = ERROR_SUCCESS;
            bool created = CreateFolderDirect(p, &err);
            if (!created && IsPermissionError(err)) {
                created = CreateFolderWithShellElevation(p, target);
            }
            // #6: a lost check-then-create race is ERROR_ALREADY_EXISTS. In
            // Number mode, advance to the next free numbered sibling (never
            // overwrite), re-scanning the disk each try so it moves past
            // whatever just appeared. In Skip/Stop mode an existing folder is
            // left untouched, so a race just means "already there" -- no retry.
            // #1: number from the ORIGINAL base (plan.base), so a race on
            // "raw 2" retries as "raw 3", never "raw 2 2".
            if (!created && ShouldRetryOnCollision(err) && collision == TC_Number) {
                // #8-1: this plan's own path is no longer a constraint (it was
                // raced away); every OTHER reserved plan path still is.
                std::vector<std::wstring> otherReserved;
                otherReserved.reserve(tmplReserved.size());
                const std::wstring selfFold = ToLowerFold(plan.path);
                for (const auto& r : tmplReserved) {
                    if (r != selfFold) { otherReserved.push_back(r); }
                }
                int guard = 0;
                while (!created && ShouldRetryOnCollision(err) && guard++ < 64) {
                    std::wstring cand =
                        NextFreeTemplateSibling(dir, plan.base, tmplPattern,
                                                otherReserved);
                    err = ERROR_SUCCESS;
                    created = CreateFolderDirect(cand, &err);
                    if (!created && IsPermissionError(err)) {
                        created = CreateFolderWithShellElevation(cand, target);
                    }
                }
            }
            if (created) {
                ++tmplCreated;
            }
        }
        // Clear breakdown so a repeat press no longer reads as a bare "0 of 0".
        // requested = names in the template; planned = names left after the
        // collision policy (Skip drops existing ones); created = made on disk.
        const int requested = static_cast<int>(tmplFolders.size());
        const int planned   = static_cast<int>(tmplPlans.size());
        const int skippedExisting = (requested > planned) ? (requested - planned) : 0;
        const int failed = (planned > tmplCreated) ? (planned - tmplCreated) : 0;
        const wchar_t* collisionName = (collision == TC_Number) ? L"number"
                                     : (collision == TC_Stop)   ? L"stop"
                                                                : L"skip";
        if (collision == TC_Stop && tmplCreated == 0 && requested > 0) {
            // #8-2: Stop cancels the WHOLE action as soon as ANY one folder
            // already exists -- it does NOT mean all `requested` were present.
            // planned=0 here, so don't mislead the log into "all N skipped as
            // existing"; report the stop/cancel that actually happened.
            Wh_Log(L"Folder template '%c': nothing created -- stopped/cancelled "
                   L"because a folder already exists (collision=stop) in %ls.",
                   templateKey, dir.c_str());
        } else if (tmplCreated == 0 && skippedExisting == requested && requested > 0) {
            Wh_Log(L"Folder template '%c': nothing created -- all %d folder(s) "
                   L"already exist (duplicates skipped) in %ls.",
                   templateKey, requested, dir.c_str());
        } else {
            Wh_Log(L"Folder template '%c': created %d of %d requested "
                   L"(%d skipped as existing, %d failed, collision=%ls) in %ls.",
                   templateKey, tmplCreated, requested,
                   skippedExisting, failed, collisionName, dir.c_str());
        }
        SafeRelease(activeView);
        InterlockedExchange(&g_actionRunning, 0);
        return;
    }

    // #5: snapshot the settings ONCE (atomic reads) so this whole action uses a
    // single consistent view even if the user changes settings mid-run.
    const LONG multiFolder = AtomicGet(&g_multiFolder);
    const LONG maxFolders = AtomicGet(&g_maxFolders);
    const LONG autoResolveNames = AtomicGet(&g_autoResolveNames);
    const LONG useDialog = AtomicGet(&g_useDialog);

    int count = ResolveFolderCount(requested, multiFolder != 0, maxFolders);

    // Debug diagnostic (gated by Windhawk's own "Logging enabled" toggle):
    // shows exactly how the requested count was resolved for this run.
    Wh_Log(L"NewFolder: requested=%d multi=%d maxFolders=%d autoResolve=%d "
           L"-> count=%d dir=%ls",
           requested,
           multiFolder != 0 ? 1 : 0,
           static_cast<int>(maxFolders),
           autoResolveNames != 0 ? 1 : 0,
           count,
           dir.c_str());

    if (useDialog) {
        // Dialog mode: a small popup takes the base name (+ count when multi is
        // on), then we create the folders directly. No temp folder, no
        // rename-watch, and NO PostQuitMessage (that was the "popup works only
        // once" bug -- it killed the worker thread's message loop).
        std::wstring capturedBase;
        if (!PromptForFolderName(target, capturedBase)) {
            Wh_Log(L"Dialog cancelled; nothing created.");
            SafeRelease(activeView);
            InterlockedExchange(&g_actionRunning, 0);
            return;
        }

        std::vector<std::wstring> dlgFolders =
            ResolveCaptureNames(dir, capturedBase, count,
                                autoResolveNames != 0);
        if (dlgFolders.empty()) {
            Wh_Log(L"No folder names available in: %ls", dir.c_str());
            SafeRelease(activeView);
            InterlockedExchange(&g_actionRunning, 0);
            return;
        }

        std::wstring dlgFirst;
        int dlgCreated = 0;
        for (const std::wstring& folder : dlgFolders) {
            std::wstring targetName = folder;
            DWORD err = ERROR_SUCCESS;
            bool created = CreateFolderDirect(targetName, &err);
            if (!created && IsPermissionError(err)) {
                created = CreateFolderWithShellElevation(targetName, target);
            }
            // #5: a lost check-then-create race is ERROR_ALREADY_EXISTS -- treat
            // it as a collision and retry the next free name (bounded), rather
            // than reporting a spurious create failure.
            // A named batch ("<base> 1", "<base> 2", ...) keeps its numbered
            // scheme on retry: the sanitized base is non-empty AND more than one
            // folder was planned. Re-running ResolveCaptureNames(count=1) here
            // would collapse to the bare "<base>" (and even "<base> (2)"),
            // breaking the sequence into "Project 1 / Project / Project 3".
            // MakeNamedFolderNames(..., 1) returns the next free "<base> N".
            const std::wstring retryBase = SanitizeFolderBase(capturedBase);
            const bool namedBatch = !retryBase.empty() && dlgFolders.size() > 1;
            int retries = 0;
            while (!created && ShouldRetryOnCollision(err) && retries < 8) {
                ++retries;
                std::vector<std::wstring> next =
                    namedBatch
                        ? MakeNamedFolderNames(dir, retryBase, 1)
                        : ResolveCaptureNames(dir, capturedBase, 1,
                                              autoResolveNames != 0);
                if (next.empty()) {
                    break;
                }
                targetName = next[0];
                err = ERROR_SUCCESS;
                created = CreateFolderDirect(targetName, &err);
                if (!created && IsPermissionError(err)) {
                    created = CreateFolderWithShellElevation(targetName, target);
                }
            }
            if (!created) {
                Wh_Log(L"Folder create failed. error=%lu path=%ls",
                       err, targetName.c_str());
                continue;
            }
            ++dlgCreated;
            if (dlgFirst.empty()) {
                dlgFirst = targetName;
            }
            SHChangeNotify(SHCNE_MKDIR, SHCNF_PATHW, targetName.c_str(), nullptr);
        }
        SHChangeNotify(SHCNE_UPDATEDIR, SHCNF_PATHW, dir.c_str(), nullptr);
        if (!dlgFirst.empty()) {
            UIA_SelectItemByNameFallback(target, GetFileNamePart(dlgFirst));
        }

        SafeRelease(activeView);
        // "Create and open" (single-folder only): browse the active tab INTO
        // the one folder we just made, same window. Never for a batch.
        if (ShouldOpenAfterCreate(AtomicGet(&g_openAfterCreate) != 0,
                                  dlgCreated) &&
            !dlgFirst.empty()) {
            NavigateActiveTabToFolder(target, capturedTab, dlgFirst);
        }
        Wh_Log(L"Dialog capture: base='%ls' created=%d of %d",
               capturedBase.c_str(), dlgCreated, (int)dlgFolders.size());
        Wh_Log(L"Total action cost=%llums", GetTickCount64() - totalStart);
        InterlockedExchange(&g_actionRunning, 0);
        return;
    }

    // Inline mode: create ONE temp folder, drop it into native inline
    // rename (the "temp folder as input box" idea), watch the edit box for the
    // typed name, then materialise "<base> 1".."<base> N".
    std::vector<std::wstring> seed =
        MakeUniqueFolderNames(dir, 1, autoResolveNames != 0);
    if (seed.empty()) {
        Wh_Log(L"No seed folder name available in: %ls", dir.c_str());
        SafeRelease(activeView);
        InterlockedExchange(&g_actionRunning, 0);
        return;
    }

    std::wstring tempPath = seed[0];
    DWORD seedErr = ERROR_SUCCESS;
    bool seedElevated = false;
    bool seedCreated = CreateFolderDirect(tempPath, &seedErr);
    if (!seedCreated && IsPermissionError(seedErr)) {
        seedElevated = true;
        seedCreated = CreateFolderWithShellElevation(tempPath, target);
    }
    // #4: the seed create is check-then-create (MakeUniqueFolderNames picked a
    // name that looked free), so it has the same TOCTOU race as the dialog/batch
    // paths -- another process can win the name and CreateDirectoryW returns
    // ERROR_ALREADY_EXISTS. Treat that as a collision and advance to the next
    // free name ("New folder (2)", ...) instead of aborting (mirrors
    // ShouldRetryOnCollision / tests/collision_retry_test).
    int seedRetries = 0;
    while (!seedCreated && ShouldRetryOnCollision(seedErr) && seedRetries < 8) {
        ++seedRetries;
        std::vector<std::wstring> nextSeed =
            MakeUniqueFolderNames(dir, 1, autoResolveNames != 0);
        if (nextSeed.empty()) {
            break;
        }
        tempPath = nextSeed[0];
        seedErr = ERROR_SUCCESS;
        seedElevated = false;
        seedCreated = CreateFolderDirect(tempPath, &seedErr);
        if (!seedCreated && IsPermissionError(seedErr)) {
            seedElevated = true;
            seedCreated = CreateFolderWithShellElevation(tempPath, target);
        }
    }
    if (!seedCreated) {
        Wh_Log(L"Temp folder create failed. error=%lu path=%ls",
               seedErr, tempPath.c_str());
        SafeRelease(activeView);
        InterlockedExchange(&g_actionRunning, 0);
        return;
    }

    // #6: arm the Esc latch BEFORE the rename edit can appear, so a fast Esc in
    // the gap between StartRename and the watch loop is still caught by the hook.
    ArmInlineEscLatch();

    // Native inline rename on the temp folder (this IS the input box).
    StartRename(target, activeView, tempPath, dir, seedElevated);
    SafeRelease(activeView);

    // Watch the edit box until the user finishes typing (Enter / click away).
    std::wstring typed;
    InlineRenameOutcome outcome =
        WaitForInlineRenameResult(typed, target,
                                  RenameWatchMsForSpeed(AtomicGet(&g_typeSpeed)));

    // Esc / timeout / never-started must CANCEL -- never create folders from a
    // cancelled rename (mirrors ShouldUseCapturedName; the multi-folder "Esc
    // still creates" regression). Explorer removed the temp folder on Esc, so
    // there is nothing to clean up here; just stop.
    if (!ShouldUseCapturedName(outcome)) {
        Wh_Log(L"Inline rename not committed (outcome=%d); cancelling, "
               L"nothing created.",
               static_cast<int>(outcome));
        InterlockedExchange(&g_actionRunning, 0);
        return;
    }

    std::wstring base = SanitizeFolderBase(typed);

    // Locate the temp folder's committed path (Explorer may already have
    // renamed it to `base` for us).
    std::wstring committedPath = tempPath;
    if (!PathFileExistsW(tempPath.c_str())) {
        std::wstring renamed = JoinPath(dir, base);
        if (!base.empty() && PathFileExistsW(renamed.c_str())) {
            committedPath = renamed;
        } else {
            committedPath.clear();
        }
    }

    if (base.empty() || count <= 1) {
        // No usable name, or a single folder: the inline rename already did the
        // whole job, nothing more to create.
        // "Create and open" (single-folder only): browse the active tab INTO
        // the folder just committed, same window.
        if (ShouldOpenAfterCreate(AtomicGet(&g_openAfterCreate) != 0, 1) &&
            !committedPath.empty()) {
            NavigateActiveTabToFolder(target, capturedTab, committedPath);
        }
        Wh_Log(L"Inline capture: single/no-name base='%ls' count=%d",
               base.c_str(), count);
        InterlockedExchange(&g_actionRunning, 0);
        return;
    }

    // Batch of "<base> 1".."<base> N". Reuse the temp folder as slot 1 by
    // renaming it (avoids deleting anything, so no N+1 leftover). If reuse
    // fails, delete the temp folder and create slot 1 fresh -- and if that
    // delete is blocked the temp folder lingers (the N+1 edge case).
    std::vector<std::wstring> names = MakeNamedFolderNames(dir, base, count);
    if (names.empty()) {
        InterlockedExchange(&g_actionRunning, 0);
        return;
    }

    // #10: only tell Explorer about folders that ACTUALLY exist -- notifying
    // SHCNE_MKDIR for intended-but-failed names makes Explorer cache phantoms.
    std::vector<std::wstring> createdFolders;
    size_t startIdx = 0;
    if (!committedPath.empty() && committedPath == names[0]) {
        // The inline rename already produced names[0].
        createdFolders.push_back(names[0]);
        startIdx = 1;
    } else if (!committedPath.empty()) {
        // #6: try to reuse the committed temp folder as names[0] via a rename.
        // If the move fails, LEAVE the committed folder in place -- do NOT
        // delete a user-visible folder on a transient/racy move failure (the
        // old RemoveDirectoryW fallback was unsafe). We simply create names[0]
        // fresh below and the committed folder stays as-is.
        if (MoveFileW(committedPath.c_str(), names[0].c_str())) {
            createdFolders.push_back(names[0]);
            startIdx = 1;
        } else {
            Wh_Log(L"Temp folder rename to '%ls' failed; leaving it in place: %ls",
                   names[0].c_str(), committedPath.c_str());
        }
    }

    for (size_t i = startIdx; i < names.size(); ++i) {
        std::wstring targetName = names[i];
        DWORD err = ERROR_SUCCESS;
        bool created = CreateFolderDirect(targetName, &err);
        if (!created && IsPermissionError(err)) {
            created = CreateFolderWithShellElevation(targetName, target);
        }
        // #5: a lost check-then-create race shows up as ERROR_ALREADY_EXISTS.
        // Treat it as a collision: re-resolve the next free "<base> N" name and
        // retry, up to a small bounded number of attempts, instead of failing.
        int retries = 0;
        while (!created && ShouldRetryOnCollision(err) && retries < 8) {
            ++retries;
            std::vector<std::wstring> next =
                MakeNamedFolderNames(dir, base, 1);
            if (next.empty()) {
                break;
            }
            targetName = next[0];
            err = ERROR_SUCCESS;
            created = CreateFolderDirect(targetName, &err);
            if (!created && IsPermissionError(err)) {
                created = CreateFolderWithShellElevation(targetName, target);
            }
        }
        if (created) {
            createdFolders.push_back(targetName);
        } else {
            Wh_Log(L"Batch folder create failed. error=%lu path=%ls",
                   err, targetName.c_str());
        }
    }

    for (const std::wstring& f : createdFolders) {
        SHChangeNotify(SHCNE_MKDIR, SHCNF_PATHW, f.c_str(), nullptr);
    }
    SHChangeNotify(SHCNE_UPDATEDIR, SHCNF_PATHW, dir.c_str(), nullptr);
    int createdCount = (int)createdFolders.size();
    Wh_Log(L"Inline capture: base='%ls' created=%d of %d",
           base.c_str(), createdCount, (int)names.size());

    Wh_Log(L"Total action cost=%llums",
           GetTickCount64() - totalStart);

    InterlockedExchange(&g_actionRunning, 0);
}

// ---------------- Hook thread ----------------
static volatile HANDLE g_hookThread = nullptr;
static DWORD g_hookThreadId = 0;
static HHOOK g_lowLevelHook = nullptr;
static volatile HANDLE g_workerThread = nullptr;
static DWORD g_workerThreadId = 0;

// Trigger key-up swallow flags -- only touched on the hook thread. Set when a
// Ctrl+F+N gesture swallowed the F / N keydowns, so their matching key-ups are
// also swallowed (they belonged to the gesture, not to Explorer).
static bool g_swallowUpF = false;
static bool g_swallowUpN = false;

// Count-entry state (multi-folder) -- only touched on the hook thread.
static bool g_countActive = false;
static int g_countValue = 0;
static bool g_countSawDigit = false;
static UINT_PTR g_countTimer = 0;

static const UINT_PTR kCountTimerId = 0xF0F1;

static DWORD WINAPI WorkerThread(void* pParameter);
static DWORD WINAPI HookThread(void* pParameter);
static LRESULT CALLBACK LowLevelKeybdProc(int nCode,
                                          WPARAM wParam,
                                          LPARAM lParam);

// #2: every field the worker needs to act on the ORIGINAL trigger context is
// captured on the hook thread and carried in a heap-owned struct whose pointer
// rides in lParam. Nothing is read from a shared global at worker time, so a
// second gesture that overwrites g_triggerTarget / g_triggerTab while the first
// action is still queued can no longer make the first action act on the wrong
// window OR the wrong tab. The worker takes ownership and frees it.
// Post the (folder-creating) worker action. Non-blocking; the hook thread must
// never do COM / filesystem work inline.
//
// The whole trigger context (count + origin HWND + origin tab HWND) is snapshot
// into a heap PendingFolderAction and its pointer rides in lParam, so each
// posted action is fully self-contained -- NO shared mutable state between the
// hook thread and the worker.
static void PostFolderActionCount(int requested) {
    if (g_workerThreadId &&
        InterlockedCompareExchange(&g_actionPosted, 1, 0) == 0) {
        auto* action = new (std::nothrow) PendingFolderAction();
        if (!action) {
            InterlockedExchange(&g_actionPosted, 0);
            return;
        }
        action->requested = requested;
        action->origin = g_triggerTarget;
        action->tab = g_triggerTab;
        action->snapExpected = g_triggerSnapPosted;  // #2: carry post success
        action->keypressTick = g_triggerTick;        // #2: carry keypress time
        if (!PostThreadMessageW(
                g_workerThreadId, WM_APP + 1, 0,
                reinterpret_cast<LPARAM>(action))) {
            delete action;
            InterlockedExchange(&g_actionPosted, 0);
        }
    }
}

// Folder Templates: post a TEMPLATE action for the given lowercased letter key.
// Mirrors PostFolderActionCount exactly (same g_actionPosted CAS, heap alloc,
// trigger-context capture, and on-fail cleanup) but tags the action with a
// templateKey so the worker looks the letter up in g_templates instead of
// creating `requested` count folders. The hook NEVER reads g_templates -- it
// only carries the char; the worker does the lookup.
static void PostTemplateAction(wchar_t key) {
    if (g_workerThreadId &&
        InterlockedCompareExchange(&g_actionPosted, 1, 0) == 0) {
        auto* action = new (std::nothrow) PendingFolderAction();
        if (!action) {
            InterlockedExchange(&g_actionPosted, 0);
            return;
        }
        action->requested = 0;                       // count ignored for templates
        action->templateKey = key;                   // template action key
        action->origin = g_triggerTarget;
        action->tab = g_triggerTab;
        action->snapExpected = g_triggerSnapPosted;  // #2: carry post success
        action->keypressTick = g_triggerTick;        // #2: carry keypress time
        if (!PostThreadMessageW(
                g_workerThreadId, WM_APP + 1, 0,
                reinterpret_cast<LPARAM>(action))) {
            delete action;
            InterlockedExchange(&g_actionPosted, 0);
        }
    }
}

// Re-inject the Ctrl+F we swallowed so Explorer's search still opens. Injected
// input carries LLKHF_INJECTED, so our own hook ignores it (no feedback loop).
static void ReplayCtrlF() {
    bool ctrlHeld = (GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0;

    INPUT in[4] = {};
    int n = 0;

    if (!ctrlHeld) {
        in[n].type = INPUT_KEYBOARD;
        in[n].ki.wVk = VK_CONTROL;
        ++n;
    }

    in[n].type = INPUT_KEYBOARD;
    in[n].ki.wVk = 'F';
    ++n;

    in[n].type = INPUT_KEYBOARD;
    in[n].ki.wVk = 'F';
    in[n].ki.dwFlags = KEYEVENTF_KEYUP;
    ++n;

    if (!ctrlHeld) {
        in[n].type = INPUT_KEYBOARD;
        in[n].ki.wVk = VK_CONTROL;
        in[n].ki.dwFlags = KEYEVENTF_KEYUP;
        ++n;
    }

    UINT sent = SendInput(n, in, sizeof(INPUT));
    if (sent != static_cast<UINT>(n)) {
        // Replay of the swallowed Ctrl+F may be blocked (UIPI / secure desktop);
        // hardening only, nothing safe to retry here.
        Wh_Log(L"ReplayCtrlF: SendInput sent %u/%d events (error=%lu).",
               sent, n, GetLastError());
    }
}

// ---- Multi-folder count entry (hook thread only) ----
static void StartCountTimer() {
    g_countTimer = SetTimer(nullptr, kCountTimerId,
                            CountWindowMsForSpeed(AtomicGet(&g_typeSpeed)),
                            nullptr);
    // Hardening: if the count-entry timer could not be created it would never
    // auto-commit. Log it; the caller (BeginCountCapture) commits immediately
    // as a fallback so the gesture still produces a folder.
    if (g_countTimer == 0) {
        Wh_Log(L"StartCountTimer: SetTimer failed (error=%lu)", GetLastError());
    }
}

static void StopCountTimer() {
    if (g_countTimer) {
        KillTimer(nullptr, g_countTimer);
        g_countTimer = 0;
    }
}

// Clear count-entry state and stop its timer.
static void EndCountCapture() {
    StopCountTimer();
    g_countActive = false;
    g_countValue = 0;
    g_countSawDigit = false;
}

// Open a count-entry window right after a trigger fired (multi-folder on).
static void BeginCountCapture() {
    g_countActive = true;
    g_countValue = 0;
    g_countSawDigit = false;
    StartCountTimer();
    // Hardening: without the auto-commit timer, count entry could never close.
    // Fall back to creating a single folder immediately so the gesture is never
    // silently stuck.
    if (g_countTimer == 0) {
        g_countActive = false;
        PostFolderActionCount(1);
    }
}

// Commit the accumulated count (no digits -> 1) and create that many folders.
static void CommitCountCapture() {
    int requested = g_countSawDigit ? g_countValue : 0;
    Wh_Log(L"CountCapture commit: sawDigit=%d value=%d -> requested=%d",
           g_countSawDigit ? 1 : 0,
           g_countValue,
           requested);
    EndCountCapture();
    PostFolderActionCount(requested);
}

// The count-entry window elapsed: commit what was typed.
static void ResolveCountTimeout() {
    if (!g_countActive) {
        return;
    }
    CommitCountCapture();
}

// A trigger fired: open count entry when multi-folder is on, else create one now.
static void TriggerFired() {
    // #2 (true trigger-time capture): stamp the keypress instant FIRST, before any
    // other work, so it is as close as possible to the physical shortcut. The
    // worker later uses it to verify the browse-dir snapshot was resolved promptly
    // (i.e. before the user could have navigated the same tab).
    g_triggerTick = GetTickCount64();
    // #1: resolve the origin Explorer window NOW, on the hook thread, while the
    // user's focus/cursor is still where the shortcut was pressed. This HWND is
    // carried through to the worker via PostFolderActionCount so the folders
    // land in the right window even if focus moves during count entry.
    g_triggerTarget = GetTargetExplorerHWND();
    // #3: also snapshot the active tab so the worker can detect a tab switch
    // during the wait and cancel rather than create in the wrong tab.
    g_triggerTab = g_triggerTarget ? GetActiveShellTabHwnd(g_triggerTarget)
                                   : nullptr;

    // #2 (directory race): ask the worker to snapshot the CURRENT browse dir for
    // this (origin, tab) NOW, before count entry can navigate the same tab. COM
    // path resolution must not run on the hook thread, so we only POST -- the
    // worker resolves it and caches it, and later reuses it when the WM_APP+1
    // action arrives. HWNDs fit in wParam/lParam.
    //
    // Record whether the post SUCCEEDED. If it failed (or there was no target),
    // this action has NO drift protection, and PostFolderActionCount carries the
    // failure through so the worker safe-cancels instead of running unprotected.
    g_triggerSnapPosted = false;
    if (g_workerThreadId && g_triggerTarget) {
        g_triggerSnapPosted =
            PostThreadMessageW(g_workerThreadId, WM_APP + 2,
                               reinterpret_cast<WPARAM>(g_triggerTarget),
                               reinterpret_cast<LPARAM>(g_triggerTab)) != FALSE;
    }

    // Open the key-capture window when EITHER multi-folder is on OR at least
    // one template is configured. The capture window is the ONLY place a
    // template letter is caught, so gating it on multi-folder alone made
    // templates dead under the default settings (the "templates don't work"
    // bug). Mirrors ShouldOpenCaptureWindow in tests/newfolder_logic.h.
    if (AtomicGet(&g_multiFolder) || AtomicGet(&g_hasTemplates)) {
        BeginCountCapture();
    } else {
        PostFolderActionCount(1);
    }
}

static BOOL Worker_Init() {
    if (g_workerThread) {
        return TRUE;
    }

    HANDLE readyEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!readyEvent) return FALSE;

    HANDLE hThread = CreateThread(nullptr,
                                  0,
                                  WorkerThread,
                                  readyEvent,
                                  CREATE_SUSPENDED,
                                  &g_workerThreadId);

    if (!hThread) {
        CloseHandle(readyEvent);
        return FALSE;
    }

    SetThreadPriority(hThread, THREAD_PRIORITY_ABOVE_NORMAL);
    ResumeThread(hThread);

    WaitForSingleObject(readyEvent, INFINITE);
    CloseHandle(readyEvent);

    g_workerThread = hThread;
    return TRUE;
}

static void Worker_Exit() {
    // #7: signal any in-flight inline-rename poll to bail so this teardown does
    // not block on WaitForSingleObject(INFINITE) for the full rename timeout.
    InterlockedExchange(&g_unloading, 1);

    HANDLE hThread =
        (HANDLE)InterlockedExchangePointer((PVOID*)&g_workerThread, nullptr);

    if (!hThread) return;

    if (g_workerThreadId) {
        PostThreadMessageW(g_workerThreadId, WM_APP, 0, 0);
    }

    WaitForSingleObject(hThread, INFINITE);
    CloseHandle(hThread);

    g_workerThreadId = 0;
}

static BOOL KeybdHook_Init() {
    if (g_hookThread) {
        return TRUE;
    }

    if (!Worker_Init()) {
        Wh_Log(L"Worker thread initialization failed.");
        return FALSE;
    }

    HANDLE readyEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!readyEvent) {
        Worker_Exit();
        return FALSE;
    }

    HANDLE hThread = CreateThread(nullptr,
                                  0,
                                  HookThread,
                                  readyEvent,
                                  CREATE_SUSPENDED,
                                  &g_hookThreadId);

    if (!hThread) {
        CloseHandle(readyEvent);
        Worker_Exit();
        return FALSE;
    }

    SetThreadPriority(hThread, THREAD_PRIORITY_ABOVE_NORMAL);
    ResumeThread(hThread);

    WaitForSingleObject(readyEvent, INFINITE);
    CloseHandle(readyEvent);

    if (!g_lowLevelHook) {
        Wh_Log(L"SetWindowsHookEx failed.");
        WaitForSingleObject(hThread, INFINITE);
        CloseHandle(hThread);
        g_hookThreadId = 0;
        Worker_Exit();
        return FALSE;
    }

    g_hookThread = hThread;
    return TRUE;
}

static void KeybdHook_Exit() {
    HANDLE hThread =
        (HANDLE)InterlockedExchangePointer((PVOID*)&g_hookThread, nullptr);

    if (!hThread) return;

    if (g_hookThreadId) {
        PostThreadMessageW(g_hookThreadId, WM_APP, 0, 0);
    }

    WaitForSingleObject(hThread, INFINITE);
    CloseHandle(hThread);

    g_hookThreadId = 0;
    g_lowLevelHook = nullptr;

    Worker_Exit();
}

static DWORD WINAPI WorkerThread(void* pParameter) {
    HANDLE readyEvent = (HANDLE)pParameter;
    MSG msg;

    PeekMessageW(&msg, nullptr, WM_USER, WM_USER, PM_NOREMOVE);

    HRESULT hrCom = CoInitializeEx(nullptr,
                                   COINIT_APARTMENTTHREADED |
                                   COINIT_DISABLE_OLE1DDE);
    // Hardening: COM is required for shell path/view resolution. On failure the
    // action still fails safe later (ResolveActiveShellView returns false), but
    // logging makes the root cause obvious instead of a mystery "resolve failed".
    if (FAILED(hrCom)) {
        Wh_Log(L"Worker CoInitializeEx failed (hr=0x%08lX); shell resolution "
               L"will be unavailable.",
               static_cast<unsigned long>(hrCom));
    }

    SetEvent(readyEvent);

    while (true) {
        BOOL bRet = GetMessageW(&msg, nullptr, 0, 0);

        if (bRet <= 0) {
            break;
        }

        if (msg.hwnd == nullptr) {
            if (msg.message == WM_APP) {
                PostQuitMessage(0);
                continue;
            }

            if (msg.message == WM_APP + 2) {
                // #2 (directory race): trigger-time browse-dir snapshot. Resolve
                // and cache the dir for (origin, tab) NOW, before count entry
                // navigates. Worker-thread-local; paired with the WM_APP+1 that
                // follows once the count commits.
                SnapshotTriggerDir(reinterpret_cast<HWND>(msg.wParam),
                                   reinterpret_cast<HWND>(msg.lParam));
                continue;
            }

            if (msg.message == WM_APP + 1) {
                InterlockedExchange(&g_actionPosted, 0);
                // Take ownership of the heap action posted by the hook thread
                // (#2): count + origin + tab all travel WITH the message, never
                // read from a global at worker time.
                std::unique_ptr<PendingFolderAction> action(
                    reinterpret_cast<PendingFolderAction*>(msg.lParam));
                HWND origin = action ? action->origin : nullptr;
                HWND tab = action ? action->tab : nullptr;
                int requested = action ? action->requested : 0;
                // #2 (directory race): did the WM_APP+2 snapshot POST succeed?
                // If not, this action has NO drift protection and must be
                // safe-cancelled rather than run unprotected.
                bool snapExpected = action ? action->snapExpected : false;
                // #2 (directory race): pair this action with the trigger-time
                // browse-dir snapshot taken by WM_APP+2. Both messages run on
                // this same worker loop serially, so g_snap* needs no lock. Only
                // use it (dir + resolve tick) when it was resolved for THIS exact
                // (origin, tab); if anything differs we leave both empty/zero so
                // the downstream guard safe-cancels rather than acting unverified.
                std::wstring snapDir;
                ULONGLONG snapResolveTick = 0;
                if (origin && origin == g_snapOrigin && tab == g_snapTab) {
                    snapDir = g_snapDir;
                    snapResolveTick = g_snapResolveTick;
                }
                g_snapDir.clear();
                g_snapOrigin = nullptr;
                g_snapTab = nullptr;
                g_snapResolveTick = 0;
                // #2 (true trigger-time capture): the snapshot is resolved on the
                // worker when it processes WM_APP+2, NOT at the keypress. If the
                // worker was busy the resolve can land AFTER the user navigated
                // the same tab, so the snapshot itself would hold the drifted-to
                // dir and the dir compare would wrongly match. Only trust the
                // snapshot when it was resolved within a short window of the
                // keypress -- short enough that no manual same-tab navigation
                // could have completed in between. Anything slower is not a true
                // trigger-time capture, so the action safe-cancels downstream.
                ULONGLONG keypressTick = action ? action->keypressTick : 0;
                bool snapFresh =
                    snapResolveTick != 0 && keypressTick != 0 &&
                    snapResolveTick >= keypressTick &&
                    (snapResolveTick - keypressTick) <= kSnapshotFreshWindowMs;
                // Mirrors ShouldRunQueuedAction (tests/queue_drain_test,
                // tests/unload_drain_test). DRAIN (discard) the dequeued action
                // instead of running it when:
                //   * we are UNLOADING -- Worker_Exit set g_unloading; starting
                //     now would seed a temp folder mid-teardown before the
                //     rename watch could notice the unload and cancel;
                //   * an action is already in flight (its popup is pumping this
                //     same loop) -- avoids the stale duplicate; or
                //   * the message carried no captured origin (stale/legacy).
                if (AtomicGet(&g_unloading) != 0 ||
                    g_actionRunning != 0 || origin == nullptr) {
                    continue;  // action freed by unique_ptr
                }
                wchar_t templateKey = action ? action->templateKey : 0;
                PerformNewFolderAction(requested, origin, tab, snapDir,
                                       snapExpected, snapFresh, templateKey);
                continue;
            }
        }

        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    SafeRelease(g_uia);

    if (SUCCEEDED(hrCom)) {
        CoUninitialize();
    }

    return 0;
}

static DWORD WINAPI HookThread(void* pParameter) {
    HANDLE readyEvent = (HANDLE)pParameter;
    MSG msg;

    PeekMessageW(&msg, nullptr, WM_USER, WM_USER, PM_NOREMOVE);

    g_lowLevelHook = SetWindowsHookExW(WH_KEYBOARD_LL,
                                       LowLevelKeybdProc,
                                       HINST_THISCOMPONENT,
                                       0);

    SetEvent(readyEvent);

    if (!g_lowLevelHook) {
        return 0;
    }

    while (true) {
        BOOL bRet = GetMessageW(&msg, nullptr, 0, 0);

        if (bRet <= 0) {
            break;
        }

        if (msg.hwnd == nullptr) {
            if (msg.message == WM_APP) {
                PostQuitMessage(0);
                continue;
            }

            // Match the SetTimer-returned handle, not the
            // ignored constant kCountTimerId. Without this the count-entry
            // window never auto-commits, so a bare trigger creates nothing and
            // g_countActive stays stuck (the next keystroke then leaks to
            // Explorer -- search opens / Ctrl+N new window).
            if (msg.message == WM_TIMER && g_countTimer &&
                msg.wParam == g_countTimer) {
                ResolveCountTimeout();
                continue;
            }
        }

        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    UnhookWindowsHookEx(g_lowLevelHook);
    g_lowLevelHook = nullptr;

    return 0;
}

static bool IsExplorerForegroundForHotkey() {
    HWND fg = GetForegroundWindow();

    if (!fg) return false;

    return IsExplorerTopLevel(fg) || fg == GetShellWindow();
}

// Mirrors tests/newfolder_logic.h::TriggerDecide. Runs on the hook thread and
// stays O(1): it only reads async key state, updates flags, arms a timer, and
// posts the worker action. No COM / filesystem / blocking work here.
static LRESULT CALLBACK LowLevelKeybdProc(int nCode,
                                          WPARAM wParam,
                                          LPARAM lParam) {
    if (nCode != HC_ACTION) {
        return CallNextHookEx(g_lowLevelHook, nCode, wParam, lParam);
    }

    const KBDLLHOOKSTRUCT* info =
        reinterpret_cast<KBDLLHOOKSTRUCT*>(lParam);

    if (!info) {
        return CallNextHookEx(g_lowLevelHook, nCode, wParam, lParam);
    }

    // Never react to our own injected input (avoids a feedback loop).
    if (info->flags & LLKHF_INJECTED) {
        return CallNextHookEx(g_lowLevelHook, nCode, wParam, lParam);
    }

    const bool isKeyDown =
        (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN);
    const bool isKeyUp =
        (wParam == WM_KEYUP || wParam == WM_SYSKEYUP);
    const int vk = static_cast<int>(info->vkCode);

    // Swallow the key-up of a key whose key-down we swallowed (no stray char).
    if (isKeyUp) {
        if (vk == 'F' && g_swallowUpF) {
            g_swallowUpF = false;
            return 1;
        }
        if (vk == 'N' && g_swallowUpN) {
            g_swallowUpN = false;
            return 1;
        }

        // Hold-modifier-throughout: the instant the gesture's hold-modifier is
        // released, finish count entry NOW -- commit typed digits, or the
        // default single folder when none were typed. This is the responsive
        // path (no 1200 ms count-timer wait) and it clears g_countActive so the
        // next gesture starts clean instead of leaking the key into Explorer.
        // #8-2: the modifier is whatever the CONFIGURED shortcut holds (Ctrl for
        // Ctrl+F+N, Alt for Alt+F2, Win for Win+N) -- not hard-coded Ctrl. We
        // react when a hold-modifier key goes up AND no gesture modifier remains
        // down.
        if (g_countActive && IsHoldModifierVk(vk) &&
            !IsGestureModifierHeldNow(CurrentShortcut())) {
            CommitCountCapture();
        }

        return CallNextHookEx(g_lowLevelHook, nCode, wParam, lParam);
    }

    if (!isKeyDown) {
        return CallNextHookEx(g_lowLevelHook, nCode, wParam, lParam);
    }

    // #6: latch an Esc pressed while the inline-rename watcher is running so a
    // fast press+release between the watcher's 60 ms polls is still seen as a
    // cancel. We only RECORD it here -- never swallow it -- because Explorer
    // itself needs the Esc to cancel its own rename EDIT.
    if (vk == VK_ESCAPE && AtomicGet(&g_inlineRenameActive) != 0) {
        InterlockedExchange(&g_inlineEscPressed, 1);
    }

    // ---- Count entry (multi-folder): a number typed right after a trigger ----
    if (g_countActive) {
        // Never hold keys hostage if focus left Explorer/desktop.
        if (!IsExplorerForegroundForHotkey()) {
            EndCountCapture();
            return CallNextHookEx(g_lowLevelHook, nCode, wParam, lParam);
        }

        // Modifier-held-throughout: the count window only lives while the
        // gesture's hold-modifier stays down. The moment it is released, FINISH
        // entry -- commit the digits typed so far, or the default single folder
        // when none were typed (CommitCountCapture maps "no digits" -> 1). Only
        // Esc cancels (create nothing). Mirrors CountDecide(modifierHeld=false).
        // #8-2: the modifier is whatever the CONFIGURED shortcut holds (Ctrl,
        // Alt, Win, ...), not hard-coded Ctrl -- so Alt+F2 / Win+N can capture a
        // batch count or a template letter instead of committing immediately.
        // NOTE: the primary release path is the key-up handler above; this
        // keydown fallback covers a key that arrives after the modifier was let
        // go.
        const bool ctrlStillHeld = IsGestureModifierHeldNow(CurrentShortcut());
        if (!ctrlStillHeld) {
            // A DIGIT is explicit intent: the low-level hook can deliver the
            // FIRST count digit's keydown after GetAsyncKeyState(VK_CONTROL)
            // already flipped to released (observed as "Ctrl+F+N then 9"
            // logging sawDigit=0 -> requested=0 -> count=1). Fold that first
            // digit into the accumulator BEFORE committing, so the requested
            // count is honoured. Mirrors CountDecide(ctrlHeld=false, Digit).
            if (!g_countSawDigit) {
                int firstDigit = -1;
                if (vk >= '0' && vk <= '9') {
                    firstDigit = vk - '0';
                } else if (vk >= VK_NUMPAD0 && vk <= VK_NUMPAD9) {
                    firstDigit = vk - VK_NUMPAD0;
                }
                if (firstDigit >= 0) {
                    g_countValue = firstDigit;
                    g_countSawDigit = true;
                    Wh_Log(L"Ctrl read released as the first count digit "
                           L"arrived; recording digit=%d before commit",
                           firstDigit);
                    CommitCountCapture();  // create the requested count
                    return 1;              // this digit is ours; swallow it
                }
            }
            if (g_countSawDigit) {
                Wh_Log(L"Ctrl released during count entry: committing typed "
                       L"count (value=%d)", g_countValue);
                CommitCountCapture();  // create the requested count
            } else {
                Wh_Log(L"Ctrl released during count entry with no digits: "
                       L"creating the default single folder");
                CommitCountCapture();  // no digits -> 1 (default single folder)
            }
            return CallNextHookEx(g_lowLevelHook, nCode, wParam, lParam);
        }

        // Modifier auto-repeat while Ctrl is (correctly) still held: neither
        // commit nor leak a keystroke -- just keep capturing.
        if (vk == VK_CONTROL || vk == VK_LCONTROL || vk == VK_RCONTROL ||
            vk == VK_SHIFT || vk == VK_LSHIFT || vk == VK_RSHIFT ||
            vk == VK_MENU || vk == VK_LMENU || vk == VK_RMENU ||
            vk == VK_LWIN || vk == VK_RWIN) {
            return CallNextHookEx(g_lowLevelHook, nCode, wParam, lParam);
        }

        int digit = -1;
        if (vk >= '0' && vk <= '9') {
            digit = vk - '0';
        } else if (vk >= VK_NUMPAD0 && vk <= VK_NUMPAD9) {
            digit = vk - VK_NUMPAD0;
        }

        if (digit >= 0) {
            int v = g_countValue * 10 + digit;
            if (v > kFolderHardCap) {
                v = kFolderHardCap;
            }
            g_countValue = v;
            g_countSawDigit = true;
            return 1;  // swallow the digit
        }

        if (vk == VK_RETURN) {
            CommitCountCapture();
            return 1;  // swallow Enter
        }

        if (vk == VK_ESCAPE) {
            EndCountCapture();  // cancel: create nothing
            return 1;  // swallow Esc
        }

        // Folder Templates: a LETTER while the count window is active routes to
        // a template action instead of a count. Thin hook -- capture the
        // lowercased char and post; the worker does the g_templates lookup. The
        // typed count (if any) is DISCARDED: cancel the count window with
        // EndCountCapture (create nothing), not CommitCountCapture.
        if (vk >= 'A' && vk <= 'Z') {
            wchar_t key = static_cast<wchar_t>(vk - 'A' + L'a');
            // #1: only a letter BOUND to a template fires a template. An
            // unbound letter must NOT post a template the worker can't resolve
            // (which created nothing) -- it falls through to the single-folder
            // commit below. The hook reads a lock-free key mask, never the
            // g_templates vector.
            if (IsTemplateKeyBound(
                    (unsigned long)AtomicGet(&g_templateKeyMask), key)) {
                Wh_Log(L"Template letter '%c' pressed; posting template action", key);
                PostTemplateAction(key);
                EndCountCapture();  // cancel the count window WITHOUT creating count folders
                return 1;           // swallow the letter
            }
            // Unbound letter: fall through to CommitCountCapture (single folder).
        }

        // Any other key WHILE CTRL IS STILL HELD (the Ctrl-release paths above
        // already returned): commit what we have (or one) and SWALLOW it.
        // Letting it through would reach Explorer as a Ctrl+<key> shortcut --
        // Ctrl+N opens a new window, Ctrl+F opens search -- the exact "a new
        // window opens when I use Ctrl+F+N <number>" bug. Keys reach Explorer
        // only after Ctrl is released (handled above). Mirrors
        // CountDecide(CountKey::Other, ctrlHeld=true) -> swallow.
        CommitCountCapture();
        return 1;
    }

    const bool fg = IsExplorerForegroundForHotkey();

    // Fire when the configured final key is pressed while every preceding key
    // remains held. This supports user-entered shortcuts such as Ctrl+Shift+N,
    // Alt+F2, Win+N, and the default Ctrl+F+N without preset choices.
    // #3: read the shortcut as ONE atomic snapshot (never the tearable struct).
    const ShortcutConfig sc = CurrentShortcut();
    if (fg && vk == sc.triggerVk) {
        bool allHeld = true;
        for (int i = 0; i < sc.heldCount; ++i) {
            if ((GetAsyncKeyState(sc.heldVks[i]) & 0x8000) == 0) {
                allHeld = false;
                break;
            }
        }
        if (allHeld) {
            if (vk == 'F') g_swallowUpF = true;
            if (vk == 'N') g_swallowUpN = true;
            TriggerFired();
            return 1;
        }
    }

    return CallNextHookEx(g_lowLevelHook, nCode, wParam, lParam);
}

// ---------------- Tool mod entry points ----------------
BOOL WhTool_ModInit() {
    // Startup diagnostic. The version here MUST match the @version metadata
    // above (enforced by scripts\check_version_init.ps1).
    Wh_Log(L"INIT: v0.10.0 explorer-ctrlfn-newfolder loaded");
    // Guard g_templates/g_templateCollision. Created BEFORE the first
    // LoadSettings() call (which publishes into them) and before the hook
    // starts, so no thread ever touches the CS uninitialised.
    InitializeCriticalSection(&g_templatesCs);
    LoadSettings();
    return KeybdHook_Init();
}

void WhTool_ModUninit() {
    Wh_Log(L"Uninit");
    KeybdHook_Exit();
    FreeThemeBrushes();
    if (g_uiFont) {
        DeleteObject(g_uiFont);
        g_uiFont = nullptr;
    }
    // Destroyed AFTER the hook/worker have stopped (KeybdHook_Exit above), so
    // no thread can still be inside the CS when it is deleted.
    DeleteCriticalSection(&g_templatesCs);
}

////////////////////////////////////////////////////////////////////////////////
// Tool mod boilerplate

bool g_isToolModProcessLauncher = false;
HANDLE g_toolModProcessMutex = nullptr;

void WINAPI EntryPoint_Hook() {
    Wh_Log(L">");
    ExitThread(0);
}

BOOL Wh_ModInit() {
    bool isExcluded = false;
    bool isToolModProcess = false;
    bool isCurrentToolModProcess = false;

    int argc = 0;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLineW(), &argc);
    if (!argv) {
        Wh_Log(L"CommandLineToArgvW failed");
        return FALSE;
    }

    for (int i = 1; i < argc; ++i) {
        if (wcscmp(argv[i], L"-service") == 0 ||
            wcscmp(argv[i], L"-service-start") == 0 ||
            wcscmp(argv[i], L"-service-stop") == 0) {
            isExcluded = true;
            break;
        }
    }

    for (int i = 1; i < argc - 1; ++i) {
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
            CreateMutexW(nullptr, TRUE, L"windhawk-tool-mod_" WH_MOD_ID);
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
            (IMAGE_DOS_HEADER*)GetModuleHandleW(nullptr);
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
    switch (GetModuleFileNameW(nullptr,
                               currentProcessPath,
                               ARRAYSIZE(currentProcessPath))) {
        case 0:
        case ARRAYSIZE(currentProcessPath):
            Wh_Log(L"GetModuleFileName failed");
            return;
    }

    WCHAR commandLine[MAX_PATH + 256];
    swprintf_s(commandLine,
               L"\"%s\" -tool-mod \"%s\"",
               currentProcessPath,
               WH_MOD_ID);

    HMODULE kernelModule = GetModuleHandleW(L"kernelbase.dll");
    if (!kernelModule) {
        kernelModule = GetModuleHandleW(L"kernel32.dll");
        if (!kernelModule) {
            Wh_Log(L"No kernelbase.dll/kernel32.dll");
            return;
        }
    }

    using CreateProcessInternalW_t = BOOL(WINAPI*)(
        HANDLE,
        LPCWSTR,
        LPWSTR,
        LPSECURITY_ATTRIBUTES,
        LPSECURITY_ATTRIBUTES,
        WINBOOL,
        DWORD,
        LPVOID,
        LPCWSTR,
        LPSTARTUPINFOW,
        LPPROCESS_INFORMATION,
        PHANDLE);
    CreateProcessInternalW_t pCreateProcessInternalW =
        (CreateProcessInternalW_t)GetProcAddress(kernelModule,
                                                 "CreateProcessInternalW");
    if (!pCreateProcessInternalW) {
        Wh_Log(L"No CreateProcessInternalW");
        return;
    }

    STARTUPINFOW si = {};
    si.cb = sizeof(si);
    si.dwFlags = STARTF_FORCEOFFFEEDBACK;

    PROCESS_INFORMATION pi = {};
    if (!pCreateProcessInternalW(nullptr,
                                 currentProcessPath,
                                 commandLine,
                                 nullptr,
                                 nullptr,
                                 FALSE,
                                 NORMAL_PRIORITY_CLASS,
                                 nullptr,
                                 nullptr,
                                 &si,
                                 &pi,
                                 nullptr)) {
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

    LoadSettings();
}

void Wh_ModUninit() {
    if (g_isToolModProcessLauncher) {
        return;
    }

    WhTool_ModUninit();
    ExitProcess(0);
}
