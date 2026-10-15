// ==WindhawkMod==
// @id              ctrlg-group
// @name            Folder Creation & File Grouping
// @description     Create single or batch folders with Ctrl+F+N, or group, copy and move selected files and folders with Ctrl+G. Includes folder templates, flexible naming, collision handling and Auto Grouping.
// @version         0.12.71
// @author          Ashix
// @github          https://github.com/k-ashix
// @twitter         https://x.com/k_ashix
// @include         windhawk.exe
// @donateUrl       https://ko-fi.com/kashix
// @compilerOptions -lole32 -loleaut32 -lshlwapi -lshell32 -luser32 -luuid -luiautomationcore -lgdi32 -ladvapi32 -ld2d1 -lcomctl32
// ==/WindhawkMod==

// For bug reports and feature requests, please open an issue here:
// https://github.com/ramensoftware/windhawk-mods/issues

// ==WindhawkModReadme==
/*
# Explorer Ctrl+G -> Group Folder/Files

Make a new folder or batch of folders in File Explorer (including sub-folder & Desktop)
(*This PC* / *Quick access* and other non-filesystem spots are skipped.)

## **Why "Ctrl + G"?** **G** for *Group*.
## **Why "Ctrl + F + N"?** **F** for *Folder*, **N** for *Name*.

> **Good to know first:** only **one** shortcut is active at a time - firing any
> other chord does nothing. Hold Ctrl for the whole gesture 
> **Esc** cancels and creates nothing.

## ⌨️ How to use
1. Open File Explorer (or click an empty Desktop spot).
2. **Hold Ctrl** the whole time -> tap **F** -> tap **N**. Type a name, press **Enter**.
3. **Hold Ctrl** the whole time -> tap **F** -> tap **N** -> tap **any digit (3, 8, 12) to create folders in batch**.
4. **Hold Ctrl** the whole time -> tap **F** -> tap **N** -> tap **you template key to create folders mentioned in you template**.

> **Ctrl must stay down for the entire shortcut.** Let go and the gesture is
> abandoned - nothing is created.

**Esc** cancels in both naming modes: the temporary folder is removed and
nothing is created. Pressing **Enter** on an empty name is what keeps the
default `New folder`. In the inline box, clicking away commits what you typed
so far (like Explorer's own rename); the popup box commits with its Create button.

#### ⌨️ Keyboard Shortcuts

| # | Shortcut | Action |
|---|---|---|
| 🖱️ | **Mouse wheel** | Scrolls whatever is under the pointer - the Selected Items rows, the summary chips, or the open Help card. |
| ↔️ | **Shift + wheel** | Pans the summary chips horizontally. |
| 🔍 | **Ctrl + wheel** | Switch the Selected Items between List, Large icons and Extra large icons. |
| ↕️ | **Ctrl + E** | Toggle the popup between compact and expanded (stays that way until you switch it back). |
| 📋 | **Copy \| Move** | Click a segment, or focus the selector and press the **Left \| Right** arrow keys. |
| 🔀 | **Tab \| Shift + Tab** | Move between controls. |
| ✅ | **Enter** | Create the folder(s), or run the group / copy / move operation. |
| ❌ | **Esc** | Cancel - nothing is created or moved. |
| 📁 | **Ctrl + F + N** | Create New Folder. |
| 📁 | **Ctrl + F + N + 9** | Creates **9** Folder. |
| 📁 | **Ctrl + F + N + i** | Creates **Images** Template (e.g). |
| 🗂️ | **Ctrl + G** | Group into Folder. |

- **🛡️ Destination folders:** Choose **Use existing**, **Numbered** or **Cancel** when the destination folder already exists. Existing folders are never overwritten.

- **📄 File conflicts:** Choose **Ask**, **Replace**, **Skip** or **Compare** when a file with the same name already exists. If an operation is cancelled or fails partway, the result is reported **honestly** (how many items actually landed) - but partial changes already made, such as a created destination folder or files already moved, are **not** automatically rolled back.

## ⚙️ Settings

| Setting | Default | What it does |
|---|---|---|
| **Name folders with a popup box** | OFF | **OFF** (default) - folder opens in Explorer's inline rename box; type + Enter. Feels native. **ON** - a themed light/dark popup box; type + Create. Reliable, same in every folder. Both modes available; only affects Ctrl+F+N. |
| **Multiple folders in one go** | ON | ON (default): type a number right after the shortcut, then Enter (e.g. `5` -> five folders). No number = one; Esc cancels. **Folder templates fire regardless of this setting.** OFF: always one folder. |
| **Max folders per run** | 25 | Cap per trigger. Range 1-100. |
| **Auto-resolve name clashes** | ON | ON: numbers past existing folders (`New folder (2)`, `(3)`...) - a folder **every** time. OFF: only the plain `New folder`, so once it exists the shortcut does nothing. If it "only works once", turn this ON. |
| **Open the new folder after creating it** | OFF | ON: browse into the newly created folder. Single-folder only|
| **Type speed** | Normal | How long the box waits before it commits/cancels on its own. **Normal** ≈ 1.2s count / 30s name, **Relaxed** ≈ 2.5s / 60s, **Patient** ≈ 4s / 120s. Pick slower if the box closes too soon. |
| **Batch numbering pattern** | `Project 1` | Number style for a named batch. All skip existing folders. |
| **Folder templates** | *Empty* | Your own fixed folder sets. Visit `Template Creation`. **Name is required** - a blank name is ignored.|
| **Template name collisions** | Skip | For templates: `Skip` existing folders, `number` a sibling, or `stop` (create nothing). Overrides *Auto-resolve name clashes*. |

## **Naming:** one folder becomes `<name>`; several become a numbered batch whose style you pick with **Batch numbering pattern** (example base `New folder`):

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

Write one template per line in the form `key=Name=Folder1|Folder2|Folder3` -
the folder list is split on the `|` pipe. These examples are shown in a code
block so the pipe renders literally:

```text
i=Image=raw|jpg|png                      press i  ->  creates raw, jpg, png
w=Work=src|docs|assets                   press w  ->  creates src, docs, assets
p=Project=01 Brief|02 Assets|03 Final    press p  ->  creates 01 Brief, 02 Assets, 03 Final
```

### Templates Creation

| # | Part / rule | What it means | Example |
|---|---|---|---|
| ✏️ | **key** | The letter you press after the trigger. One letter A-Z, **case-insensitive** (`i` = `I`, stored lowercase). Separate from the name. | `i` |
| ✏️ | **Name** | Assign a label, anything you like. Only for your reference. | `Images` |
| ✏️ | **Folders** | Everything after the 2nd `=`, split on the `\|` pipe. Each piece is one folder. | `raw\|jpg\|png` |
| ✅ | **Spaces are fine** | Spaces **inside or around** a name are kept exactly - `01 Explain & Brief`. Names are **not** trimmed,| `01 Explain & Brief` |
| 📂 | **26 templates max** | One per letter A-Z. | - |
| 📂 | **100 folders max** | Per template; each list is independent. | - |
| 📂 | **One template per key** | Same key twice = the **first** line wins, later ones ignored. Use a unique key per line. | - |
| 😈 | **Bad characters** | A name containing `\` `/` `:` `*` `?` `"` `<` `>` `\|` is skipped (same set a typed name strips). So is `.`, `..`, or any name starting with `..`. | `a\b`, `../x`, `C:\x` |
| 🌝 | **Empty / all-spaces / control chars** | Blank pieces, spaces-only names, and control characters (below ASCII 0x20) are skipped. | (empty) |
| 🥴 | **Duplicate in one line** | The same name twice in one template is kept only once, **case-insensitively** (Windows folders). | `raw\|RAW` -> `raw` |
| ⛑️ | **Never overwrites** | Existing folders follow *Template name collisions* (skip / number / stop). | - |

> Note: the `,` comma is **allowed** in a folder name - only `\` `/` `:` `*`
> `?` `"` `<` `>` `|` `..` and control characters are rejected.

## 🆚 How this is different

### Ctrl+F+N vs. the other shortcut options. All Shortcut  options create and name folders the same way 

| | **Ctrl+F+N** *(default)* | **Ctrl+Shift+N** | **Ctrl+N** | **Ctrl+N+F** | **Ctrl+Q** |
|---|---|---|---|---|---|
| Keys | Hold Ctrl, tap F, then N | Single press | Single press | Hold Ctrl, tap N, then F | Single press |
| Native in 🪟 | Ctrl+F (search) | New folder | New window | New window | (none) |
| Keeps native Explorer shortcut? | ✅ Ctrl+F search still opens if you don't press N | ➖ overlays the native New folder | ❌ replaces New window | ❌ replaces New window | ✅ uses a free key |
| Accidental-trigger risk | Low (needs the F+N chord) | Low | Higher (single press) | Medium | Higher (single press) |
| Naming + batch | ✅ | ✅ | ✅ | ✅ | ✅ |

## Ctrl+G - Group Creation 🤝

Gather the files and folders you have selected in File Explorer into one folder.

- Select anything in Explorer.
- Press **Ctrl+G**.
- In the popup, type a folder name, choose **Copy** or **Move**, and confirm.
- Virtual / non-filesystem items (*This PC*, *Quick access*, ...) and an empty selection are ignored safely - nothing happens.
- Destination - By default items are grouped under the **current parent** of the selection. 
- Set a **Fixed destination folder** in Settings to always group somewhere specific.
- If you leave the folder-name box **blank**, the last successfully used target name is reused. 

## Auto Group - Sorts the selected files into folders
- Auto Group directly group selected files into folders named on their extensions (`JPG`, `PNG`, `MD`, ...)
- Auto Group **ignores** the typed name entirely and uses its own destination policy.
- **Flat** layout creates the extension folders directly in the destination.
- **Images parent** layout nests image extensions under an `Images` folder while other extensions stay at the root.
- Files **without an extension** go to a *No Extension* folder (optional), and the originally selected folders can optionally move into an *All Folder*.

## 🧩 Custom Auto Group Rules

Add custom buckets under **Auto Grouping -> Custom Auto Group rules**. Enter one
rule per line using this format:

`FolderName=ext1|ext2|ext3`

**Examples** (shown in a code block so the `|` pipe renders literally):

```text
Images=jpg|jpeg|png|gif|webp     ->  🖼️ grouped into an Images folder
Documents=pdf|doc|docx|txt|md    ->  📄 grouped into a Documents folder
Code=cpp|tsx|md|js|ts|py         ->  💻 grouped into a Code folder
Archives=zip|7z|rar|tar|gz       ->  📦 grouped into an Archives folder
```

- **🚧 Matching and validation:** Matching is case-insensitive, and `jpeg` is normalized to `jpg`. When multiple rules match an extension, the first rule wins. Unmatched extensions retain their default extension folders.

- **⛑️ Safety:** Invalid folder names, reserved Windows device names and duplicate rule names are rejected. Under the **Images parent** layout, mixed image/non-image rules are skipped to avoid ambiguous paths.

## 📂 Ctrl+G settings reference

| Setting | Default | What it does |
|---|---|---|
| **Group Creation (Ctrl+G)** (`groupSelection`) | ON | Master on/off; OFF hands Ctrl+G back to Explorer. |
| **Group by Move or Copy** | Copy | Copy keeps originals, Move relocates. |
| **When the destination folder already exists** | Use existing | Use existing \| Numbered \| Cancel. |
| **When a file with the same name is already there** | Ask | Ask \| Replace \| Skip \| Compare. |
| **Fixed destination folder** | *(empty)* | Always group here; blank = selection's parent. |
| **Open destination after grouping** | OFF | ON opens the destination afterward. |
| **Custom Auto Group rules** | *(empty)* | Route chosen extensions into named buckets for Auto Group (one `FolderName=` rule per line). |

### vs. Windows' built-in folder creation

**Short version:** this mod creates a folder, **names it in the same motion**,
makes a **batch** at once, and lets you **pick the shortcut** - none of which
Windows' own options do.

Legend: ✅ yes · ❌ no · ➖ partial / not applicable.

| Feature | **This mod** | Win right-click | Win Ctrl+Shift+N |
|---|---|---|---|
| Creates a folder | ✅ | ✅ | ✅ |
| Keyboard-only workflow | ✅ | ❌ | ✅ |
| Name during creation | ✅ inline **or** a dedicated popup | ➖ inline rename only | ➖ inline rename only |
| Multiple folders at once | ✅ up to **100** | ❌ | ❌ |
| Configurable batch numbering | ✅ **6** (`1` `-01` `-001` `_01` `_001` `(01)`) | ❌ | ❌ |
| Handle existing | ✅ number **or** skip | ➖ always numbers | ➖ always numbers |
| Reusable folder templates | ✅ up to 26 keys | ❌ | ❌ |
| Configurable folder-creation shortcuts | ✅ **5** options | ❌ fixed | ❌ fixed |
| Group selected files into a named folder | ✅ | ❌ | ❌ |
| Group selected folders | ✅ | ❌ | ❌ |
| Choose Copy or Move while Grouping | ✅ | ❌ | ❌ |
| Automatically group files by extension | ✅ | ❌ | ❌ |
| Custom extension-based grouping rules | ✅ | ❌ | ❌ |
| Organize group extensions under a parent folder | ✅ | ❌ | ❌ |
| Reuse existing destination folders | ✅ | ❌ | ❌ |
| Theme-aware dedicated popup with Help | ✅ | ➖ | ➖ |

## 🙏 Credits

This mod grew out of two existing Windhawk mods and reuses their Explorer
hook / worker-thread design. Credit and thanks to their authors:

- **Explorer Ctrl+N to New File** — <https://github.com/ramensoftware/windhawk-mods>
- **Explorer Ctrl+Q to New Folder** — <https://github.com/ramensoftware/windhawk-mods>
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- NewFolder:
  - shortcut: "Ctrl+F+N"
    $name: Shortcut
    $description: >-
      Pick one of the supported shortcuts. Only one is active at a time.
    $options:
    - "Ctrl+F+N": Ctrl+F+N (default) - keeps every native Explorer shortcut
    - "Ctrl+Shift+N": Ctrl+Shift+N - the exact keys Windows uses for New folder
    - "Ctrl+N": Ctrl+N - single press (replaces "New window")
    - "Ctrl+N+F": Ctrl+N+F - hold Ctrl, tap N then F (replaces "New window")
    - "Ctrl+Q": Ctrl+Q - single press (alternative trigger)
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
      Off (default): type into Explorer's inline rename box (feels native). On: a popup box appears first - type the name, press Create (reliable, same in every folder). Both modes are available; this only affects Ctrl+F+N (Ctrl+G has its own popup).
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
  $name: New Folder (Ctrl+F+N)
- Group:
  - groupSelection: true
    $name: Group Creation (Ctrl+G)
    $description: >-
      On (default): press Ctrl+G in File Explorer to group the selected
      files/folders into the destination folder.
  - groupOperation: copy
    $name: Group by Move or Copy
    $description: >-
      Copy (default) leaves the originals in place; 
      Move relocates them into the destination folder.
    $options:
    - copy: Copy 
    - move: Move 
  - groupCollision: useExisting
    $name: When the destination folder already exists
    $description: >-
      This setting only matters when a folder with THAT name is already in the destination. 
      Use existing (default): drop the items into the folder that is already there (nothing is overwritten) 
      - best for grouping into the same folder again and again. 
      Numbered: leave the existing folder untouched and make a fresh one beside it ("Trip", "Trip 2", ...). Cancel: do nothing.
      Note: this is about a same-named FOLDER; a same-named FILE inside the
      destination is handled by the next setting.
    $options:
    - useExisting: Use the existing folder (default)
    - numbered: Make a new numbered folder
    - cancel: Cancel - do nothing
  - groupFileCollision: ask
    $name: When a file with the same name is already there
    $description: >-
      What to do when the destination already contains a FILE with the same name
      as one being grouped (the case Windows normally shows the "Replace or Skip
      Files" box for). Ask (default) shows that native box so you decide each
      time. Replace overwrites the existing file with no prompt. Skip keeps the
      existing file and drops the incoming one, no prompt. Compare shows the
      native box with "Compare info for both files".
    $options:
    - ask: Ask each time - show Windows' Replace/Skip box (default)
    - replace: Replace - overwrite the existing file, no prompt
    - skip: Skip - keep the existing file, no prompt
    - compare: Compare - show the native box with "Compare info for both files"
  - groupDestination: ""
    $name: Fixed destination folder (optional)
    $description: >-
      Blank (default): Ctrl+G groups into the current parent folder of the
      selection. Set a folder path: Ctrl+G always groups straight INTO that
      folder - handy for always sorting into one fixed place.
  - groupOpenAfter: false
    $name: Open destination after grouping
    $description: >-
      Off (default): nothing is opened after grouping. On: the destination
      folder is opened in Explorer once the items have been grouped.
  $name: Group Creation (Ctrl+G)
- AutoGrouping:
  - autoGroupEnabled: true
    $name: Auto Group button behaviour
    $description: >-
      Auto Group sorts the selected files into folders named based on their extensions (JPG, PNG, MD,
      ...) without you typing a folder name. Off: the button is not shown and normal Ctrl+G grouping is unchanged.
  - autoGroupLayout: flat
    $name: Auto Group folder layout
    $description: >-
      Flat (default) creates extension folders directly in the destination.
      Parent groups image extensions (JPG, PNG, GIF, WEBP, ...) beneath an
      Images folder, while other extension folders remain at the destination
      root.
    $options:
    - flat: Flat extension folders
    - imagesParent: Images parent directory
  - autoGroupImageParentName: "Images"
    $name: Images parent folder name
    $description: >-
      Parent folder for image-extension folders when the Images parent directory
      layout is selected.
  - autoGroupSelectedFolders: false
    $name: Group selected folders into All Folder
    $description: >-
      Off leave selected folders untouched. On: move the originally
      selected folders into All Folder during Auto Group only.
  - autoGroupFoldersName: "All Folder"
    $name: Directory name where existing folders will be Grouped during Auto Grouping Only
  - autoGroupNoExtension: true
    $name: Group files without extensions
    $description: >-
      On will put files without a usable extension into the No Extension
      directory. Off: leave these files untouched.
  - autoGroupNoExtensionName: "No Extension"
    $name: No Extension directory name
    $description: >-
      Folder name for files without a usable extension.
  - autoGroupCustomRules:
    - ""
    $name: Custom Auto Group rules
    $description: >-
      One rule per line as FolderName=ext1|ext2|ext3 to route those
      file extensions into a named bucket instead of the default per-extension
      folder (for example Photos=jpg|jpeg|png|gif or Documents=pdf|doc|docx|txt).
      Folder names are validated, matching is case-insensitive, the first rule
      wins when two rules claim the same extension, and files whose extension is
      not covered still go to their normal extension folder. Leave empty for the
      standard one-folder-per-extension behavior.
  $name: Auto Grouping
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <string>
#include <cwctype>
#include <vector>
#include <unordered_map>    // fix thse isseue.md item 7: one-pass Auto Group file->bucket map
#include <memory>
#include <new>
#include <d2d1.h>
#include <shlwapi.h>
#include <shlobj.h>
#include <shobjidl.h>
#include <shellapi.h>
#include <exdisp.h>
#include <oleauto.h>
#include <KnownFolders.h>
#include <ShlGuid.h>
#include <UIAutomation.h>
#include <commctrl.h>        // ILD_TRANSPARENT (image-list draw flags)
#include <commoncontrols.h>  // IImageList (Phase 1 hi-res icons); use __uuidof(IImageList) (its IID_IImageList constant is not in Windhawk clang's uuid lib)
#include <utility>           // std::pair (Phase 1 per-popup icon cache)

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
static volatile LONG g_multiFolder = 0;                 // batch toggle; PRE-LoadSettings runtime value is 0, the CONFIGURED default is ON (settings schema: multiFolder true), applied by LoadSettings
static volatile LONG g_hasTemplates = 0;                // 1 if >=1 template loaded (hook reads lock-free)
static volatile LONG g_templateKeyMask = 0;             // #1: bit (c-'a') set == letter 'c' is a bound template key (hook reads lock-free)
static volatile LONG g_maxFolders = kFolderDefaultMax;  // raw maxFolders setting
static volatile LONG g_autoResolveNames = 1;            // number clashes (default on)
static volatile LONG g_useDialog = 0;                   // Ctrl+F+N naming: 0 = inline rename (default), 1 = themed popup box. Both modes available.
static volatile LONG g_openAfterCreate = 0;             // 0 = stay, 1 = open the new folder (single create only)
static volatile LONG g_typeSpeed = 0;                   // 0=Normal, 1=Relaxed, 2=Patient (scales count + rename windows)
static volatile LONG g_namingPattern = 0;              // 0=Space "N", 1=Dash2 "-0N", 2=Dash3 "-00N", 3=Paren2 " (0N)", 4=Under2 "_0N", 5=Under3 "_00N"
// Ctrl+G "Group Selection" on/off toggle. Ctrl+G is a HARDWIRED, dedicated
// group key -- it is NOT part of the New Folder `shortcut` $options and never
// creates a folder. This gate lets the user turn the whole Ctrl+G feature off.
// Default on (1). The Windhawk $setting that drives it is wired in Task 7.
static volatile LONG g_groupSelection = 1;             // 1 = Ctrl+G groups, 0 = disabled
static volatile LONG g_groupIsMove = 0;                // groupOperation: 0 = Copy (default), 1 = Move
static volatile LONG g_groupCollision = 0;             // groupCollision: 0 = UseExisting (default), 1 = Numbered, 2 = Cancel
static volatile LONG g_groupFileCollision = 0;         // groupFileCollision: 0 = Ask (default, native prompt), 1 = Replace, 2 = Skip, 3 = Compare
// Private sentinel bit (NOT a real shell FOF_* flag; well above the defined
// range) meaning "on a name clash, skip the incoming item and keep the
// existing one." Masked off before the flags reach IFileOperation. Mirrors
// kGroupSkipExistingBit in tests/group_logic.h.
static const unsigned int kGroupSkipExistingBit = 0x40000000u;
// Compare sentinel: show the native conflict box (with Windows' own "Compare
// info for both files" choice). Masked off before the flags reach
// IFileOperation. Mirrors kGroupCompareBit in tests/group_logic.h.
static const unsigned int kGroupCompareBit = 0x20000000u;
static volatile LONG g_groupOpenAfter = 0;             // groupOpenAfter: 0 = stay (default), 1 = open destination after grouping
// Optional fixed destination folder for Ctrl+G. Empty (default) = group into
// the selection's parent using the typed name. Guarded by g_templatesCs (the
// same lock that protects the other settings-thread-written string state) so a
// settings-change write can't tear a concurrent read.
static std::wstring g_groupDestination;                // groupDestination (fixed target path; empty = none)
// ---- Auto Group (ADD-ON) settings state. ISOLATED from the Group state above:
// these affect ONLY the Auto Group button, never ordinary Ctrl+G Group or the
// folder templates. LONG flags are read via AtomicGet / written via
// InterlockedExchange like the other Group flags; the configurable NAMES are
// std::wstring guarded by the same g_templatesCs lock that protects
// g_groupDestination. The eight-second button delay is a FIXED safety rule and
// is intentionally NOT a user setting. Defaults mirror autogroup::Settings in
// tests/auto_group_logic.h.
static volatile LONG g_autoGroupEnabled = 1;           // autoGroupEnabled: 1 = show the Auto Group button (default), 0 = hidden
static volatile LONG g_autoGroupLayout = 0;            // autoGroupLayout: 0 = Flat (default), 1 = ImagesParent
static volatile LONG g_autoGroupSelectedFolders = 0;   // autoGroupSelectedFolders: 0 = leave folders untouched (default), 1 = move into All Folder
static volatile LONG g_autoGroupNoExtension = 1;       // autoGroupNoExtension: 1 = group extensionless files (default), 0 = leave untouched
static std::wstring g_autoGroupImageParentName;        // autoGroupImageParentName (default "Images")
static std::wstring g_autoGroupFoldersName;            // autoGroupFoldersName (default "All Folder")
static std::wstring g_autoGroupNoExtensionName;        // autoGroupNoExtensionName (default "No Extension")
static std::vector<std::wstring> g_autoGroupCustomRuleLines;  // autoGroupCustomRules: raw "Folder=ext|ext" lines (Phase 6), parsed at group time
// #8: the last destination Ctrl+G actually grouped into. WORKER-THREAD-LOCAL
// (only read/written inside the WM_APP_GROUP handler), so no lock is needed. A
// blank folder name in the Group popup reuses this, mirroring ResolveGroupTarget
// / ResolveGroupDestination in tests/group_logic.h.
static std::wstring g_lastGroupTarget;

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
// The hook reads the
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
// deduped. Waiting
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

// The "middle" key of a two-key chord: the single held key that is NOT a
// modifier (F for Ctrl+F+N, N for Ctrl+N+F). Returns 0 when the shortcut has no
// such key -- i.e. it is a single-press gesture (Ctrl+N, Ctrl+Q, Ctrl+G) or a
// pure modifier+trigger combo (Ctrl+Shift+N). Only a non-zero result puts the
// hook into the pending-chord path: the middle key is swallowed on its keydown
// (so it never reaches Explorer as a bare Ctrl+F search / Ctrl+N new window)
// and the gesture completes only when the trigger key follows.
static int ChordIntermediateVk(const ShortcutConfig& s) {
    int heldCount = s.heldCount;
    if (heldCount > 4) heldCount = 4;
    int found = 0;
    for (int i = 0; i < heldCount; ++i) {
        if (!IsHoldModifierVk(s.heldVks[i])) {
            if (found != 0) return 0;  // more than one non-modifier: not a simple chord
            found = s.heldVks[i];
        }
    }
    return found;
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
    PCWSTR shortcutText = Wh_GetStringSetting(L"NewFolder.shortcut");
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
                        Wh_GetIntSetting(L"NewFolder.multiFolder") != 0 ? 1 : 0);
    InterlockedExchange(&g_maxFolders,
                        static_cast<LONG>(Wh_GetIntSetting(L"NewFolder.maxFolders")));
    InterlockedExchange(&g_autoResolveNames,
                        Wh_GetIntSetting(L"NewFolder.autoResolveNames") != 0 ? 1 : 0);
    InterlockedExchange(&g_useDialog,
                        Wh_GetIntSetting(L"NewFolder.useDialog") != 0 ? 1 : 0);
    InterlockedExchange(&g_openAfterCreate,
                        Wh_GetIntSetting(L"NewFolder.openAfterCreate") != 0 ? 1 : 0);
    // Ctrl+G Group Selection on/off. Hardwired key, read only as an enable flag
    // -- Ctrl+G is NOT part of the New Folder `shortcut` $options.
    InterlockedExchange(&g_groupSelection,
                        Wh_GetIntSetting(L"Group.groupSelection") != 0 ? 1 : 0);
    // Group Move/Copy default (mirrors ParseGroupOperationIsMove): "move" ->
    // Move, anything else -> Copy (the default). Copy keeps the originals.
    PCWSTR groupOp = Wh_GetStringSetting(L"Group.groupOperation");
    LONG groupIsMove = 0;  // Copy default
    if (groupOp) {
        if (wcscmp(groupOp, L"move") == 0) {
            groupIsMove = 1;
        }
        Wh_FreeStringSetting(groupOp);
    }
    InterlockedExchange(&g_groupIsMove, groupIsMove);
    // Existing-destination behavior (mirrors ParseGroupCollision): 0=UseExisting
    // (default), 1=Numbered, 2=Cancel.
    PCWSTR groupColl = Wh_GetStringSetting(L"Group.groupCollision");
    LONG groupCollision = 0;  // UseExisting default
    if (groupColl) {
        if (wcscmp(groupColl, L"numbered") == 0) {
            groupCollision = 1;
        } else if (wcscmp(groupColl, L"cancel") == 0) {
            groupCollision = 2;
        }
        Wh_FreeStringSetting(groupColl);
    }
    InterlockedExchange(&g_groupCollision, groupCollision);
    // File-level collision behavior (Issue 1). Mirrors ParseGroupFileCollision:
    // 0=Ask (default, native Replace/Skip box), 1=Replace, 2=Skip, 3=Compare.
    PCWSTR groupFileColl = Wh_GetStringSetting(L"Group.groupFileCollision");
    LONG groupFileCollision = 0;  // Ask default
    if (groupFileColl) {
        if (wcscmp(groupFileColl, L"replace") == 0) {
            groupFileCollision = 1;
        } else if (wcscmp(groupFileColl, L"skip") == 0) {
            groupFileCollision = 2;
        } else if (wcscmp(groupFileColl, L"compare") == 0) {
            groupFileCollision = 3;
        }
        Wh_FreeStringSetting(groupFileColl);
    }
    InterlockedExchange(&g_groupFileCollision, groupFileCollision);
    // Open the destination after grouping (off by default).
    InterlockedExchange(&g_groupOpenAfter,
                        Wh_GetIntSetting(L"Group.groupOpenAfter") != 0 ? 1 : 0);
    // Optional fixed destination path. Empty = use the selection's parent.
    // Guarded by g_templatesCs to match the other settings-thread string writes.
    PCWSTR groupDest = Wh_GetStringSetting(L"Group.groupDestination");
    EnterCriticalSection(&g_templatesCs);
    g_groupDestination = groupDest ? groupDest : L"";
    LeaveCriticalSection(&g_templatesCs);
    if (groupDest) {
        Wh_FreeStringSetting(groupDest);
    }

    // ---- Auto Group settings (fix thse isseue.md Phase 7). These now live in
    // their OWN top-level Windhawk settings category "Auto Grouping" and are read
    // from the AutoGrouping.* path (NOT the legacy Group.* path). Windhawk keys a
    // stored value by its full dotted path, so relocating the category resets any
    // previously customised value to the (unchanged) documented default on first
    // load after upgrade -- that is a Windhawk settings-API limitation, not a
    // silent reset in this mod; the defaults below are identical to before.
    // ISOLATED from the Group settings above; parsing one of these never changes
    // a g_group* value, so existing Ctrl+G Group defaults/behaviour are untouched.
    // LONG flags are published with
    // InterlockedExchange; the configurable NAMES are written under g_templatesCs
    // like g_groupDestination. The eight-second delay is a fixed safety rule and
    // has no setting. Blank/whitespace names fall back to the documented default
    // so a misconfigured name can never produce an empty bucket path.
    InterlockedExchange(&g_autoGroupEnabled,
                        Wh_GetIntSetting(L"AutoGrouping.autoGroupEnabled") != 0 ? 1 : 0);
    // Layout: a string option like `groupOperation`; unknown -> Flat (0).
    LONG autoLayout = 0;  // flat default
    PCWSTR autoLay = Wh_GetStringSetting(L"AutoGrouping.autoGroupLayout");
    if (autoLay) {
        if (wcscmp(autoLay, L"imagesParent") == 0) {
            autoLayout = 1;
        }
        Wh_FreeStringSetting(autoLay);
    }
    InterlockedExchange(&g_autoGroupLayout, autoLayout);
    InterlockedExchange(&g_autoGroupSelectedFolders,
                        Wh_GetIntSetting(L"AutoGrouping.autoGroupSelectedFolders") != 0 ? 1 : 0);
    InterlockedExchange(&g_autoGroupNoExtension,
                        Wh_GetIntSetting(L"AutoGrouping.autoGroupNoExtension") != 0 ? 1 : 0);
    // Configurable folder names, each with a safe non-empty default.
    auto trimmedOrDefault = [](PCWSTR raw, const wchar_t* fallback) -> std::wstring {
        std::wstring s = raw ? raw : L"";
        std::size_t b = 0, e = s.size();
        while (b < e && (s[b] == L' ' || s[b] == L'\t')) ++b;
        while (e > b && (s[e - 1] == L' ' || s[e - 1] == L'\t')) --e;
        s = s.substr(b, e - b);
        return s.empty() ? std::wstring(fallback) : s;
    };
    PCWSTR autoImg = Wh_GetStringSetting(L"AutoGrouping.autoGroupImageParentName");
    PCWSTR autoAll = Wh_GetStringSetting(L"AutoGrouping.autoGroupFoldersName");
    PCWSTR autoNoExt = Wh_GetStringSetting(L"AutoGrouping.autoGroupNoExtensionName");
    EnterCriticalSection(&g_templatesCs);
    g_autoGroupImageParentName = trimmedOrDefault(autoImg, L"Images");
    g_autoGroupFoldersName = trimmedOrDefault(autoAll, L"All Folder");
    g_autoGroupNoExtensionName = trimmedOrDefault(autoNoExt, L"No Extension");
    LeaveCriticalSection(&g_templatesCs);
    if (autoImg) Wh_FreeStringSetting(autoImg);
    if (autoAll) Wh_FreeStringSetting(autoAll);
    if (autoNoExt) Wh_FreeStringSetting(autoNoExt);

    // Phase 6: custom Auto Group rules. Windhawk string arrays are read until
    // the first empty entry; the empty default slot yields no rules, so the
    // standard one-folder-per-extension behaviour is unchanged. The raw
    // "Folder=ext|ext" lines are validated/parsed by autogroup::ParseCustomRules
    // at Auto Group time (see the worker). Capped generously here; the parser
    // enforces the documented rule/extension limits.
    std::vector<std::wstring> customRuleLines;
    for (int i = 0; i < 1000; ++i) {
        PCWSTR cr = Wh_GetStringSetting(L"AutoGrouping.autoGroupCustomRules[%d]", i);
        if (!cr || !cr[0]) {
            if (cr) Wh_FreeStringSetting(cr);
            break;
        }
        customRuleLines.emplace_back(cr);
        Wh_FreeStringSetting(cr);
    }
    EnterCriticalSection(&g_templatesCs);
    g_autoGroupCustomRuleLines = std::move(customRuleLines);
    LeaveCriticalSection(&g_templatesCs);

    // Type-speed preset (0=Normal, 1=Relaxed, 2=Patient). Read as a string
    // option like `trigger`; any unknown value maps to Normal.
    PCWSTR typeSpeed = Wh_GetStringSetting(L"NewFolder.typeSpeed");
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

    // Batch numbering pattern (0=Space, 1=Dash2, 2=Dash3, 3=Paren2, 4=Under2, 5=Under3). Read as a
    // string option like `trigger`; any unknown value maps to Space (default).
    PCWSTR namingPattern = Wh_GetStringSetting(L"NewFolder.namingPattern");
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
    PCWSTR tc = Wh_GetStringSetting(L"NewFolder.templateCollision");
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
        PCWSTR value = Wh_GetStringSetting(L"NewFolder.folderTemplates[%d]", i);
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
           L"namingPattern=%d templateCollision=%d templates=%zu "
           L"groupSelection=%d groupIsMove=%d groupCollision=%d groupOpenAfter=%d",
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
           usableCount,
           AtomicGet(&g_groupSelection) != 0 ? 1 : 0,
           AtomicGet(&g_groupIsMove) != 0 ? 1 : 0,
           static_cast<int>(AtomicGet(&g_groupCollision)),
           AtomicGet(&g_groupOpenAfter) != 0 ? 1 : 0);
}

// Type-speed timeout mappers. Mirror of CountWindowMsForSpeed /
// ONE setting scales BOTH the count-entry window and
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
// was created -- never for a batch.
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

static int ClampFolderMax(int maxSetting) {
    if (maxSetting < 1) {
        return 1;
    }
    if (maxSetting > kFolderHardCap) {
        return kFolderHardCap;
    }
    return maxSetting;
}

// Off -> always 1; no digits
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

// Returns `count` paths
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
    // byte-identical.
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

// ---- Name-capture resolution ----
// Batch numbering pattern (
// the `namingPattern` setting). Chooses ONLY how the number is formatted; the
// base name and skip-existing collision resolution are the same for every
// pattern. Pad width only pads -- a number wider than the width is written in
// full, so the sequence never collides by truncation.
//   0 Space  : "<base> 1"
//   1 Dash2  : "<base>-01"
//   2 Dash3  : "<base>-001"
//   3 Paren2 : "<base> (01)"   |   4 Under2 : "<base>_01"   |   5 Under3 : "<base>_001"
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
        // make it fail.
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

// ---- Folder Templates: pure logic ----
// The types, constants and g_templates state are declared ABOVE LoadSettings.
// Adapted to the mod: existence via PathFileExistsW, numbering via the mod's
// FormatBatchName(LONG pattern). The shortcut key is user-assigned per
// template (the `key=` field), independent of the name. No templates are built in.

// Case-fold for case-INSENSITIVE comparison (Windows dir names). Mirror of
// #F3: "raw" and "RAW" are the same folder.
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
// control chars).
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
// #1: carrying `base` lets
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
        // fix thse isseue.md P2: a failed SetOwnerWindow means the elevation
        // prompt may not be correctly owned -- log it rather than ignore it.
        HRESULT hrOwner = fileOp->SetOwnerWindow(owner);
        if (FAILED(hrOwner))
            Wh_Log(L"CreateFolder: SetOwnerWindow failed hr=0x%08X", hrOwner);
    }

    DWORD flags =
        FOF_NOCONFIRMMKDIR |
        FOFX_SHOWELEVATIONPROMPT |
        FOFX_NOCOPYHOOKS;

    // fix thse isseue.md P2: these flags drive the no-confirm / elevation
    // behavior; if they did not take, the operation could prompt or behave
    // differently, so a failure must be surfaced, not silently dropped.
    HRESULT hrFlags = fileOp->SetOperationFlags(flags);
    if (FAILED(hrFlags))
        Wh_Log(L"CreateFolder: SetOperationFlags failed hr=0x%08X", hrFlags);

    hr = fileOp->NewItem(folderItem,
                         FILE_ATTRIBUTE_DIRECTORY,
                         name.c_str(),
                         nullptr,
                         nullptr);

    if (SUCCEEDED(hr)) {
        hr = fileOp->PerformOperations();
    }

    // fix thse isseue.md P2: if the aborted-state query itself fails we cannot
    // trust the result, so treat it as aborted -- a half-finished operation
    // must never be classified as a clean create.
    BOOL aborted = FALSE;
    HRESULT hrAbort = fileOp->GetAnyOperationsAborted(&aborted);
    if (FAILED(hrAbort)) {
        Wh_Log(L"CreateFolder: GetAnyOperationsAborted failed hr=0x%08X; "
               L"treating as aborted", hrAbort);
        aborted = TRUE;
    }

    SafeRelease(fileOp);
    SafeRelease(folderItem);

    if (FAILED(hr) || aborted) {
        Wh_Log(L"IFileOperation::NewItem failed. hr=0x%08X aborted=%d path=%ls",
               hr,
               aborted,
               fullPath.c_str());
        return false;
    }

    // fix thse isseue.md P2: confirm the result is actually a DIRECTORY, not
    // merely that *something* now occupies the path. A racing file at the
    // target must never be reported as a successfully created folder.
    return PathIsDirectoryW(fullPath.c_str()) != FALSE;
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
// Gated by ShouldUseCapturedName(): only a real commit may create folders. Do NOT
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
// ShouldUseCapturedName).
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

// fix thse isseue.md #7: is the user's "Transparency effects" setting ON?
// (Personalize\EnableTransparency == 1, the Windows 11 default). The popup only
// lets the acrylic backdrop show through when this is on; with it OFF the Win11
// backdrop does not composite, so the mod keeps the solid themed panel instead
// of a bare/white window. Missing value -> treat as ON (the default), matching
// the user's request for the transparent look.
static bool IsTransparencyEnabled() {
    DWORD val = 1;
    DWORD sz = sizeof(val);
    HKEY key = nullptr;
    if (RegOpenKeyExW(HKEY_CURRENT_USER,
            L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
            0, KEY_READ, &key) == ERROR_SUCCESS) {
        RegQueryValueExW(key, L"EnableTransparency", nullptr, nullptr,
                         reinterpret_cast<LPBYTE>(&val), &sz);
        RegCloseKey(key);
    }
    return val != 0;
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

// DPI of the monitor the popup will appear on. The tool process is DPI-aware,
// so every fixed pixel size in the popup (window, controls, rounded regions,
// font) must be scaled by this / 96 or it renders far too small at 150-200%.
// Prefer GetDpiForWindow(owner); fall back to MonitorFromWindow + the per-
// monitor GetDpiForMonitor. Both are resolved dynamically so the mod still
// links on toolchains whose import libs predate them (older Windows just
// keeps 96 = no scaling).
static UINT DpiForWindow(HWND owner) {
    if (owner) {
        if (HMODULE u32 = GetModuleHandleW(L"user32.dll")) {
            using GetDpiForWindowFn = UINT(WINAPI*)(HWND);
            auto getDpi = reinterpret_cast<GetDpiForWindowFn>(
                GetProcAddress(u32, "GetDpiForWindow"));
            if (getDpi) {
                UINT dpi = getDpi(owner);
                if (dpi != 0) return dpi;
            }
        }
        if (HMODULE shcore = LoadLibraryExW(L"shcore.dll", nullptr,
                                            LOAD_LIBRARY_SEARCH_SYSTEM32)) {
            using GetDpiForMonitorFn =
                HRESULT(WINAPI*)(HMONITOR, int, UINT*, UINT*);
            auto getDpiMon = reinterpret_cast<GetDpiForMonitorFn>(
                GetProcAddress(shcore, "GetDpiForMonitor"));
            if (getDpiMon) {
                HMONITOR mon = MonitorFromWindow(owner, MONITOR_DEFAULTTONEAREST);
                UINT dx = 0, dy = 0;
                if (SUCCEEDED(getDpiMon(mon, 0 /*MDT_EFFECTIVE_DPI*/, &dx, &dy)) &&
                    dx != 0) {
                    FreeLibrary(shcore);
                    return dx;
                }
            }
            FreeLibrary(shcore);
        }
    }
    return 96;  // no scaling
}

// Scale a 96-DPI design pixel value to the popup's monitor DPI.
static int ScaleDpi(int value96, UINT dpi) {
    return MulDiv(value96, static_cast<int>(dpi), 96);
}

// ---- Popup layout / spacing system ----
// Byte-for-byte mirror of tests/layout_logic.h (the unit-tested spacing system,
// UI spec item 9). ONE 4px scale and ONE ComputePromptLayout() that
// returns every control's 96-dpi design rectangle, so the popup no longer
// scatters absolute coordinates through WM_CREATE. The mod applies its own
// ScaleDpi() (MulDiv) to each rect when creating controls, so on-screen geometry
// matches the layout computed here. Keep this in lockstep with layout_logic.h.
namespace layout {

inline constexpr int kUnit   = 4;
inline constexpr int kSpace2 = kUnit * 2;   // 8
inline constexpr int kSpace3 = kUnit * 3;   // 12
inline constexpr int kSpace4 = kUnit * 4;   // 16
inline constexpr int kSpace5 = kUnit * 5;   // 20
inline constexpr int kSpace6 = kUnit * 6;   // 24

struct Rect {
    int x = 0, y = 0, w = 0, h = 0;
    int right()  const { return x + w; }
    int bottom() const { return y + h; }
    bool empty() const { return w <= 0 || h <= 0; }
};

struct PromptLayout {
    bool isGroup = false;
    bool expanded = false;
    int windowW = 0;
    int windowH = 0;

    // Selected Items card (group only): summary header + scrollable list.
    Rect selectedCard;
    Rect selectedHeader;
    Rect selectedList;
    Rect selectedContent;  // fix #3: padded (kSelectedPad = 20px) content rect; list HWND lives here
    Rect groupHeader;   // alias of the whole selectedCard rect (card wiring only, #3)

    // Destination card (group only): label + read-only field.
    Rect destCard;
    Rect destLabel;
    Rect destDesc;      // fix #1: supporting description BETWEEN label and field
    Rect destField;
    Rect destCopyBtn;   // fix thse isseue.md #2 (path pass): "copy path" button

    // Folder name card (group); New Folder keeps bare label/field, nameCard empty.
    Rect nameCard;
    Rect nameLabel;
    Rect nameDesc;      // fix #1: supporting description BETWEEN label and field
    Rect nameField;

    // Action card (group only, fix #6): title + description + Copy/Move
    // segments + keyboard hint. Segments live INSIDE this card.
    Rect actionCard;
    Rect actionTitle;
    Rect actionDesc;
    Rect keyboardHint;
    Rect chromeSizeBtn;  // group only (fix #5): size control in the window chrome
    Rect chromeMinBtn;   // group only (#1/#2): title-bar Minimize control
    Rect chromeCloseBtn; // group only (#2): title-bar Close control (rightmost)

    // Action row.
    Rect segCopy;       // group only: Copy segment (inside the Action card)
    Rect segMove;       // group only: Move segment (inside the Action card)
    Rect opToggle;      // group only: single Copy|Move toggle (alias of segCopy)
    Rect createButton;  // bottom action row
    Rect cancelButton;  // bottom action row
    Rect autoGroupButton; // bottom action row (ADD-ON): timed Auto Group button,
                          // left of Create/Group. Zero-size when Auto Group is
                          // disabled in settings (button not created).
    // fix thse isseue.md UI-03: footer INFO / help button (square, pinned to the
    // LEFT end of the action row). Drives the themed help panel (infohelp model).
    Rect infoButton;
};

inline int ScaleForDpi(int value96, int dpi) {
    return static_cast<int>(
        (static_cast<long long>(value96) * dpi + 48) / 96);
}

// fix thse isseue.md P2/section 3: layout::ScaleRect was an orphaned mirror of
// tests/layout_logic.h with no live call site -- removed here; the pure helper
// stays test-only in the header. (Reconciled per the dead-code review.)
inline constexpr int kWindowW        = 344;
inline constexpr int kGroupCompactW  = 410;   // fix #7: compact group width (~400-420)
inline constexpr int kGroupExpandedW = 660;   // fix #8: expanded is WIDER (~640-680)
inline constexpr int kFormWindowH    = 178;
inline constexpr int kOuterMargin    = kSpace5;  // 20
inline constexpr int kContentW       = kWindowW - kOuterMargin * 2;  // 304
inline constexpr int kLabelH         = 22;
inline constexpr int kFieldH         = 30;
inline constexpr int kNameFieldH     = 32;  // fix #7 (pass6): group Folder name field dominates (taller than kFieldH)
inline constexpr int kButtonW        = 74;
inline constexpr int kButtonH        = 30;
inline constexpr int kButtonGap      = 8;   // fix #13: 4px-grid gap (was 6)
inline constexpr int kAutoGroupBtnW  = 108; // ADD-ON: wider footer button so
                                            // "Auto Group (8)" fits; fixed width
                                            // so the row never reflows per second
inline constexpr int kLabelTop       = 18;
inline constexpr int kFieldTop       = 46;
inline constexpr int kActionRowTop   = 92;
// fix thse isseue.md E: kGroupHeaderBand (legacy fixed 150px header band) is
// unused -- the Group header is content-sized now; removed.
inline constexpr int kGroupHeaderTop       = kSpace4;  // 16
inline constexpr int kGroupHeaderBottomGap = kSpace2;  // 8
inline constexpr int kGroupHeaderExpandedExtra = 120;  // Phase 7 expanded list room

// ---- Group sectioned-card layout (Phase 3a) ----
inline constexpr int kCardInset         = kSpace4;   // 16
inline constexpr int kCardGap           = kSpace4;   // 16  (fix #7: aligned to the 4px scale)
inline constexpr int kGroupTopMargin    = 14;
inline constexpr int kGroupBottomMargin = kSpace4;   // 16
inline constexpr int kSelHeaderH        = 34;
inline constexpr int kSelListH          = 68;   // legacy fixed viewport
inline constexpr int kListCompactRows   = 4;    // fix #1: compact row cap (mode::CompactRows)
inline constexpr int kListExpandedRows  = 9;    // fix #1: expanded row cap (mode::ExpandedRows)
inline constexpr int kSelRowH           = 24;   // fix #1 (pass6): taller list row for vertical air (owner-draw S(24))
inline constexpr int kSelListPadV       = 8;    // fix #1: list top+bottom padding (legacy, superseded by kSelectedPad)
// fix thse isseue.md #3: Selected card content padding + owner-draw header
// rhythm, so the card height is DERIVED from the inset content block.
inline constexpr int kSelectedPad       = kSpace5;  // 20  content inset on all 4 sides (fix thse isseue.md #3: more breathing room, was 16/kSpace4)
inline constexpr int kSelListRightInset = 12;   // fix thse isseue.md #4: dedicated inner RIGHT inset for the selected list content (rows, size/ext metadata, chip clip, divider endpoint, scrollbar track) -- extra breathing room from the card's right edge, applied ONCE inside the list painter (NOT a second card inset)
inline constexpr int kSelTitleH         = 24;   // title row height (owner-draw S(24))
inline constexpr int kSelCountsH        = 24;   // fix thse isseue.md #1: summary-chip PILL row -- reserves the metric-derived pill height (semibold 12px + 4+4 vertical padding); was a hardcoded 16
inline constexpr int kSelTitleChipGap   = 6;    // fix thse isseue.md #1: gap between the Selected title and the chip row (6px; the general rhythm elsewhere stays kSelHdrGap=4)
inline constexpr int kSelDividerH       = 1;    // 1px hairline divider
inline constexpr int kSelHdrGap         = 4;    // even gap: title/counts/divider/rows (fix thse isseue.md #3: 4px rhythm, was 3; owner-draw S(4))
inline constexpr int kDestCardH         = 100;   // title+10 h18, desc+32 h16, field+56 h26 (ends 82) -> +12 bottom (P2 #13 rhythm)
inline constexpr int kNameCardH         = 108;  // title + desc + field (fix #1)
inline constexpr int kDescH             = 16;   // supporting description row (fix #1)
inline constexpr int kActionCardH       = 116;  // title+10..28, desc+30..46, segs+52..82, hint+86..100 (ends 100) -> 116 breathing (P0 #4)
inline constexpr int kHintH             = 14;   // keyboard-hint row
// fix thse isseue.md #3: reserved icon column at the card inner-left; section
// titles start this many px right so the icon sits INSIDE the card content.
inline constexpr int kSectionIconCol    = 24;
inline constexpr int kChromeBtnSize = 28;   // fix #6: caption hit target (glyph stays small)
inline constexpr int kChromeBtnW    = 44;   // fix thse isseue.md: caption click-area WIDTH (native ~45); buttons also span the full band height
static_assert(kChromeBtnW >= kChromeBtnSize, "caption click area must not shrink below the legacy 28px hit floor");
// fix thse isseue.md E: kChromeTop (the legacy 6px chrome-strip inset) is unused
// now that the caption buttons span the full band height -- removed.
inline constexpr int kChromeBand    = 18;   // fix thse isseue.md A: top band above card A; TitleBandH()=kGroupTopMargin(14)+18 = 32px native title-band height
// fix thse isseue.md A: caption buttons are CONTIGUOUS (native title-bar style);
// the inter-button gap is its OWN zero constant. The general kButtonGap (action
// row + copy-path button) is intentionally left untouched.
inline constexpr int kCaptionButtonGap = 0;

// fix thse isseue.md #2: a deliberate gap between the custom title band and the
// first (Selected Items) card. cardTop = TitleBandH() + this gap, so the card no
// longer butts straight up against the header band. 16px @96dpi (fix thse
// isseue.md #3: was 12/kSpace3; more separation from the title bar).
inline constexpr int kHeaderToSelectedGap = kSpace4;  // 16  title band -> first card

// fix thse isseue.md #2: borderless Group popup paints its OWN title bar across
// the top band (title + Expand/Minimize/Close); band runs y=0..title band
// bottom. The first card starts a kHeaderToSelectedGap below it (see cardTop).
inline int TitleBandH() { return kGroupTopMargin + kChromeBand; }

// ---- Client -> outer window size (Phase 3a bug fix, fix thse isseu e.md #1) ----
// Byte-for-byte mirror of tests/layout_logic.h. windowW/windowH are CLIENT
// sizes; the OUTER window (WS_CAPTION | WS_SYSMENU) must be larger by the frame
// or the bottom controls clip. See PromptOuterSize() below for the wiring.
inline int OuterWidthFromClient(int clientW, int ncExtraW) {
    return clientW + ncExtraW;
}
inline int OuterHeightFromClient(int clientH, int ncExtraH) {
    return clientH + ncExtraH;
}

inline PromptLayout ComputePromptLayout(bool isGroup, bool expanded = false,
                                        int itemCount = kListExpandedRows) {
    PromptLayout L;
    L.isGroup = isGroup;
    L.expanded = expanded;
    L.windowW = kWindowW;

    const int cancelX = kWindowW - kOuterMargin - kButtonW;
    const int createX = cancelX - kButtonW - kButtonGap;

    if (!isGroup) {
        // New Folder: original bare-form geometry, unchanged (no cards).
        L.nameLabel = Rect{kOuterMargin, kLabelTop, kContentW, kLabelH};
        L.nameField = Rect{kOuterMargin, kFieldTop, kContentW, kFieldH};
        L.createButton = Rect{createX, kActionRowTop, kButtonW, kButtonH};
        L.cancelButton = Rect{cancelX, kActionRowTop, kButtonW, kButtonH};
        L.windowH = kFormWindowH;
        return L;
    }

    // Group: sectioned card stack (top -> bottom). fix thse isseue.md #1: the
    // Selected Items list is CONTENT-SIZED (see selectedList below); cards shift
    // with it and the window grows to fit, so a small selection leaves no gap.
    // fix #9/#10: expanded mode is WIDER, not just taller. Width (and every
    // width derived from it) is chosen by mode so cards/fields/segments grow
    // horizontally. Compact group width is kGroupCompactW (410); the legacy 344px kWindowW is the base / New Folder width, not the group compact width.
    const int winW     = expanded ? kGroupExpandedW : kGroupCompactW;
    const int contentW = winW - kOuterMargin * 2;
    const int innerX   = kOuterMargin + kCardInset;    // 32
    const int innerW   = contentW - kCardInset * 2;
    L.windowW = winW;
    const int cancelX2 = winW - kOuterMargin - kButtonW;   // right-aligned
    const int createX2 = cancelX2 - kButtonW - kButtonGap; // left of Cancel

    // fix thse isseu e.md #5: chrome strip above the first card holds the size
    // control; the whole card stack drops by kChromeBand.
    // fix thse isseue.md #2/#1: the top band is a CUSTOM title bar (the Group
    // popup is WS_POPUP, no native caption). It holds three right-aligned
    // controls in the reference order Expand | Minimize | Close (Close rightmost);
    // the card stack drops by kChromeBand to clear the band.
    const int chromeBtn  = kChromeBtnW;       // wider click area (native ~45)
    const int chromeBtnH = TitleBandH();      // FULL band height: no dead strip above/below so clicks are not swallowed (fix thse isseue.md)
    const int chromeGap  = kCaptionButtonGap;   // fix thse isseue.md A: contiguous caption buttons (0 gap), NOT the general kButtonGap
    // fix thse isseue.md #5: Windows caption order Minimize | Expand/Restore |
    // Close -- Close rightmost, Expand/Restore middle, Minimize leftmost. Each
    // control fills the band top-to-bottom; the gap between them stays caption.
    const int closeX  = winW - kOuterMargin - chromeBtn;
    const int expandX = closeX - chromeGap - chromeBtn;
    const int minX    = expandX - chromeGap - chromeBtn;
    L.chromeCloseBtn = Rect{closeX,  0, chromeBtn, chromeBtnH};
    L.chromeSizeBtn  = Rect{expandX, 0, chromeBtn, chromeBtnH};
    L.chromeMinBtn   = Rect{minX,    0, chromeBtn, chromeBtnH};
    const int cardTop = TitleBandH() + kHeaderToSelectedGap;  // gap below the title band (#2)

    // fix thse isseue.md #1: content-sized list = min(itemCount, rows-for-mode)
    // rows, so a 5-item selection no longer reserves a 9-row viewport.
    const int capRows = expanded ? kListExpandedRows : kListCompactRows;
    int visRows = itemCount < capRows ? itemCount : capRows;
    if (visRows < 1) visRows = 1;   // always at least one row tall
    // fix thse isseue.md #3: the Selected card content is inset kSelectedPad
    // (20px, kSelectedPad) on ALL four sides. The content block is the owner-draw rhythm:
    //   title + gap + counts + gap + 1px divider + gap + visRows rows.
    // The card height is DERIVED from that content + the two paddings, so a
    // small selection leaves no empty space; the owner-drawn list HWND sits in
    // selectedContent, so the parent-painted rounded card border stays intact.
    const int selContentH =
        kSelTitleH + kSelTitleChipGap + kSelCountsH + kSelHdrGap +
        kSelDividerH + kSelHdrGap + visRows * kSelRowH;
    L.selectedContent = Rect{kOuterMargin + kSelectedPad, cardTop + kSelectedPad,
                             contentW - kSelectedPad * 2, selContentH};
    const int selCardH = selContentH + kSelectedPad * 2;
    L.selectedCard = Rect{kOuterMargin, cardTop, contentW, selCardH};
    L.groupHeader  = L.selectedCard;   // alias: whole card rect (card wiring only)
    L.selectedHeader = Rect{L.selectedContent.x, L.selectedContent.y,
                            L.selectedContent.w,
                            kSelTitleH + kSelTitleChipGap + kSelCountsH + kSelHdrGap +
                                kSelDividerH};
    L.selectedList   = Rect{L.selectedContent.x,
                            L.selectedHeader.bottom() + kSelHdrGap,
                            L.selectedContent.w, visRows * kSelRowH};

    // Card B -- Destination (title -> description -> read-only field, fix #1).
    const int destY = L.selectedCard.bottom() + kCardGap;
    L.destCard  = Rect{kOuterMargin, destY, contentW, kDestCardH};
    L.destLabel = Rect{innerX + kSectionIconCol, destY + 12,
                       innerW - kSectionIconCol, 18};
    L.destDesc  = Rect{innerX, L.destLabel.bottom() + 4, innerW, kDescH};
    // fix thse isseue.md #2 (path pass): the path field is a REDUCED rect that
    // leaves room for a compact "Copy path" button on its right, same row, so
    // the path reads plainly and the button never overlaps the text.
    const int copyBtnW = 54;
    L.destField   = Rect{innerX, L.destDesc.bottom() + 8, innerW - copyBtnW - kButtonGap, 26};
    L.destCopyBtn = Rect{L.destField.right() + kButtonGap, L.destField.y, copyBtnW, 26};

    // Card C -- Folder name (title -> description -> edit, fix #1).
    const int nameY = L.destCard.bottom() + kCardGap;
    L.nameCard  = Rect{kOuterMargin, nameY, contentW, kNameCardH};
    L.nameLabel = Rect{innerX + kSectionIconCol, nameY + 12,
                       innerW - kSectionIconCol, 18};
    L.nameDesc  = Rect{innerX, L.nameLabel.bottom() + 4, innerW, kDescH};
    L.nameField = Rect{innerX, L.nameDesc.bottom() + 8, innerW, kNameFieldH};

    // Card D -- Action (fix #6): title -> description -> Copy/Move segments ->
    // keyboard hint. Segments live INSIDE this card; Create/Cancel drop to the
    // bottom action row below it. fix thse isseue.md #1: the Status card was
    // removed, so Action now follows the Folder name card directly.
    const int actionY = L.nameCard.bottom() + kCardGap;
    L.actionCard  = Rect{kOuterMargin, actionY, contentW, kActionCardH};
    L.actionTitle = Rect{innerX + kSectionIconCol, actionY + 12,
                         innerW - kSectionIconCol, 18};
    L.actionDesc  = Rect{innerX, L.actionTitle.bottom() + 4, innerW, kDescH};
    const int segY = L.actionDesc.bottom() + 8;
    // fix #7: segments meet edge-to-edge (one pill, straight middle boundary).
    // fix #12: segments fill the inner width so they GROW when expanded.
    const int segHalf = innerW / 2;
    L.segCopy  = Rect{innerX, segY, segHalf, kButtonH};
    L.segMove  = Rect{innerX + segHalf, segY, innerW - segHalf, kButtonH};
    L.opToggle = Rect{innerX, segY, kButtonW, kButtonH};
    L.keyboardHint = Rect{innerX, segY + kButtonH + 6, innerW, kHintH};

    const int rowY = L.actionCard.bottom() + kSpace3;
    L.createButton = Rect{createX2, rowY, kButtonW, kButtonH};
    L.cancelButton = Rect{cancelX2, rowY, kButtonW, kButtonH};

    // ADD-ON: the timed Auto Group button sits LEFT of Group/Create, so the
    // bottom row reads [Auto Group (8)] [Group] [Cancel]. Its label is wider
    // than the 74px standard button, so give it kAutoGroupBtnW. The rect is
    // always computed (pure geometry); WM_CREATE only CREATES the control when
    // Auto Group is enabled in settings. The width is fixed and independent of
    // the countdown digit so the row never reflows each second.
    {
        const int autoW = kAutoGroupBtnW;
        const int autoX = createX2 - kButtonGap - autoW;
        L.autoGroupButton = Rect{autoX, rowY, autoW, kButtonH};
    }

    // fix thse isseue.md UI-03: the footer INFO/help button is a square control
    // pinned to the LEFT end of the action row (the actions stay right-aligned).
    L.infoButton = Rect{kOuterMargin, rowY, kButtonH, kButtonH};

    L.windowH = rowY + kButtonH + kGroupBottomMargin;
    return L;
}

// fix thse isseue.md UI-01 / UI-05: responsive reflow. ComputePromptLayout()
// returns the popup at its PREFERRED compact/expanded size; when the user
// maximizes or drag-resizes the Group popup the window procedure's WM_SIZE /
// WM_EXITSIZEMOVE handler reflows the children into the ACTUAL client rect
// through THIS pure transform, so the form never stays stranded at the fixed
// size in the upper-left corner. Contract:
//   * the content stretches to the client WIDTH (cards/fields widen; the
//     right-anchored chrome + action controls stay flush with the new edge),
//   * the Selected Items card absorbs the extra HEIGHT and every section below
//     it shifts down by the same amount, so the Destination / Folder-name /
//     Action sections and the footer track the client bottom and stay VISIBLE,
//   * the client is clamped to (minW,minH) so controls are never crushed,
//   * New Folder is never reflowed (its bare geometry is preserved).
// Byte-for-byte mirror of tests/layout_logic.h ReflowToClient (this source copy
// additionally shifts the Auto Group add-on button, which the test struct omits).
inline PromptLayout ReflowToClient(const PromptLayout& base,
                                   int clientW, int clientH,
                                   int minW, int minH,
                                   int selRowH = kSelRowH,
                                   int selCapRowsOverride = -1,
                                   int rowsNeededOverride = -1) {
    PromptLayout L = base;
    if (!base.isGroup) return L;              // New Folder keeps its fixed form
    if (clientW < minW) clientW = minW;
    if (clientH < minH) clientH = minH;
    // fix thse isseue.md Phase 2: the Selected-Items row height is MODE-AWARE.
    // List passes the 24px list row (the default); a GRID view (Large / Extra
    // large) passes its WHOLE cell height (viewmode::CellH) so the reflow floor
    // and the row-cap math below reserve a complete cell -- never a 24px row
    // that clips the icon/filename.
    if (selRowH <= 0) selRowH = kSelRowH;

    const int dx = clientW - base.windowW;    // extra width to distribute
    int dy = clientH - base.windowH;          // extra height to distribute
    // Never shrink the Selected Items card below a readable floor: keep the
    // footer reachable rather than letting the lower cards overlap it.
    // v0.12.68 #3: derive the Selected Items reflow FLOOR from its ACTUAL header
    // geometry plus one complete list row, NOT a magic 40px -- so a shrink keeps
    // the header and at least one whole row visible and the lower cards + footer
    // reachable (no overlap). Grid modes reserve their taller whole cell upstream.
    const int selHeaderBlockH = base.selectedList.y - base.selectedContent.y;
    const int kMinSelReflowH = selHeaderBlockH + selRowH;
    if (base.selectedContent.h + dy < kMinSelReflowH)
        dy = kMinSelReflowH - base.selectedContent.h;

    // fix thse isseue.md Issue 4: do NOT pour ALL the extra height into the
    // Selected Items card. Only a selection that was ROW-CAPPED in the preferred
    // layout (more items than its viewport shows) uses the extra height (and then
    // scrolls, issue 9/10); a small selection that already shows every row keeps
    // its content height, so a maximized/zoomed popup no longer leaves a big
    // blank band below the last row and the lower sections keep their natural
    // spacing. Shrink (dy < 0) is unchanged so the readable floor above still
    // holds. selGrow is what the card grows by (and what the lower cards shift).
    const int baseVisRows = (selRowH > 0) ? base.selectedList.h / selRowH : 0;
    const int selCapRows  = (selCapRowsOverride >= 0)
                                ? selCapRowsOverride
                                : (base.expanded ? kListExpandedRows : kListCompactRows);
    // fix thse isseue.md P1 "exact fit treated as capped": overflow -- the only
    // reason to pour extra height into the Selected card and start scrolling --
    // means MORE rows are needed than the preferred viewport actually shows,
    // NOT mere equality with the declared row cap. The caller supplies the true
    // rows-needed (rowsNeededOverride); fall back to the legacy cap heuristic
    // only when it is not provided.
    const bool selRowCapped = (rowsNeededOverride >= 0)
                                  ? (rowsNeededOverride > baseVisRows)
                                  : (baseVisRows >= selCapRows);
    int selGrow = dy;
    if (dy > 0 && !selRowCapped) selGrow = 0;

    L.windowW = clientW;
    L.windowH = clientH;

    const auto widen  = [dx](Rect& r) { if (r.w > 0) r.w += dx; };
    const auto moveX  = [dx](Rect& r) { if (r.w > 0) r.x += dx; };
    const auto shiftY = [selGrow](Rect& r) { if (r.w > 0) r.y += selGrow; };

    // Stretch the full-width cards / fields / descriptions to the client width.
    widen(L.selectedCard);  widen(L.selectedHeader); widen(L.selectedList);
    widen(L.selectedContent); widen(L.groupHeader);
    widen(L.destCard); widen(L.destLabel); widen(L.destDesc); widen(L.destField);
    widen(L.nameCard); widen(L.nameLabel); widen(L.nameDesc); widen(L.nameField);
    widen(L.actionCard); widen(L.actionTitle); widen(L.actionDesc); widen(L.keyboardHint);

    // Right-anchored controls move right so they stay flush with the new edge.
    moveX(L.chromeSizeBtn); moveX(L.chromeMinBtn); moveX(L.chromeCloseBtn);
    moveX(L.destCopyBtn); moveX(L.createButton); moveX(L.cancelButton);
    moveX(L.autoGroupButton);

    // Copy/Move segmented control: a LEFT-anchored, full-inner-width pair, NOT a
    // right-anchored control. It stays flush with the card's left inset and
    // STRETCHES to the new inner width (widening with the card) instead of
    // floating off to the right edge and leaving a gap. The two halves still
    // meet edge-to-edge so they read as one seamless pill. opToggle (the compact
    // single-button variant) just stays left-anchored.
    {
        const int segLeft   = base.segCopy.x;                     // card inner-left inset
        const int newInnerW = (base.segMove.right() - segLeft) + dx;
        const int newHalf   = newInnerW / 2;
        L.segCopy.x = segLeft;             L.segCopy.w = newHalf;              L.segCopy.y += selGrow;
        L.segMove.x = segLeft + newHalf;   L.segMove.w = newInnerW - newHalf;  L.segMove.y += selGrow;
        L.opToggle.x = segLeft;            L.opToggle.y += selGrow;
    }

    // The Selected Items card absorbs the (content-capped) extra height; every
    // section below it shifts down by the SAME amount so spacing stays uniform.
    L.selectedCard.h    += selGrow;
    L.selectedList.h    += selGrow;
    L.selectedContent.h += selGrow;
    L.groupHeader.h     += selGrow;
    shiftY(L.destCard); shiftY(L.destLabel); shiftY(L.destDesc);
    shiftY(L.destField); shiftY(L.destCopyBtn);
    shiftY(L.nameCard); shiftY(L.nameLabel); shiftY(L.nameDesc); shiftY(L.nameField);
    shiftY(L.actionCard); shiftY(L.actionTitle); shiftY(L.actionDesc);
    shiftY(L.keyboardHint);   // segCopy/segMove/opToggle shifted in the block above
    shiftY(L.createButton); shiftY(L.cancelButton); shiftY(L.autoGroupButton);
    shiftY(L.infoButton);   // UI-03: left-anchored, so it only tracks the footer DOWN
    return L;
}

// fix thse isseue.md Phase 4: which repaint a Ctrl+wheel view-mode switch needs.
// A switch only changes the Selected Items VIEWPORT content (grid vs list). If
// every section OUTSIDE that card -- the window size and the destination,
// folder-name, action and footer rects -- is geometrically identical between
// the OLD and NEW effective layouts, nothing around the card moved, so repaint
// JUST the Selected Items viewport (no child reposition, no full-window erase ->
// no title-bar / card / footer flash). Otherwise a section genuinely moved
// (e.g. a preferred popup must resize for a taller mode) and ONE atomic relayout
// is required. Comparing the two computed layouts keeps this robust across every
// window state instead of guessing from flags.
enum class ViewSwitchRepaint { SelectedOnly, FullRelayout };

inline bool SectionsUnchanged(const PromptLayout& a, const PromptLayout& b) {
    const auto eq = [](const Rect& p, const Rect& q) {
        return p.x == q.x && p.y == q.y && p.w == q.w && p.h == q.h;
    };
    return a.windowW == b.windowW && a.windowH == b.windowH &&
           eq(a.destCard, b.destCard)         && eq(a.destField, b.destField)     &&
           eq(a.nameCard, b.nameCard)         && eq(a.nameField, b.nameField)     &&
           eq(a.actionCard, b.actionCard)     && eq(a.keyboardHint, b.keyboardHint) &&
           eq(a.segCopy, b.segCopy)           && eq(a.segMove, b.segMove)         &&
           eq(a.createButton, b.createButton) && eq(a.cancelButton, b.cancelButton);
}

inline ViewSwitchRepaint ViewSwitchRepaintFor(const PromptLayout& oldL,
                                              const PromptLayout& newL) {
    return SectionsUnchanged(oldL, newL) ? ViewSwitchRepaint::SelectedOnly
                                         : ViewSwitchRepaint::FullRelayout;
}

}  // namespace layout

// ---- Center-preserving resize geometry (fix thse isseue.md #1) ----
// Byte-for-byte mirror of tests/center_logic.h. ApplyPromptLayout() resizes the
// popup about its CURRENT center (not its top-left) and clamps the result to the
// monitor work area, so a compact<->expanded or DPI relayout grows/shrinks
// symmetrically and never spills off-screen. Keep in lockstep with center_logic.h.
namespace center {

struct Point { int x = 0; int y = 0; };
struct Size  { int cx = 0; int cy = 0; };
struct Rect {
    int left = 0, top = 0, right = 0, bottom = 0;
    int width()  const { return right - left; }
    int height() const { return bottom - top; }
};

inline Point CenteredTopLeft(const Rect& old, const Size& outer) {
    const int cx = (old.left + old.right) / 2;
    const int cy = (old.top + old.bottom) / 2;
    return Point{cx - outer.cx / 2, cy - outer.cy / 2};
}

inline Point ClampToWorkArea(Point tl, const Size& outer, const Rect& work) {
    int left = tl.x;
    int top  = tl.y;
    if (left + outer.cx > work.right)  left = work.right  - outer.cx;
    if (top  + outer.cy > work.bottom) top  = work.bottom - outer.cy;
    if (left < work.left) left = work.left;
    if (top  < work.top)  top  = work.top;
    return Point{left, top};
}

inline Point CenteredResize(const Rect& old, const Size& outer,
                            const Rect& work) {
    return ClampToWorkArea(CenteredTopLeft(old, outer), outer, work);
}

}  // namespace center

// ---- Popup relayout REDRAW policy (fix thse isseue.md "Next exact fix order"
// #1-#4) ----
// Byte-for-byte mirror of tests/redraw_logic.h. The +/- resize must be ONE
// atomic transaction: suppress painting, move every child with no per-child
// repaint, then issue ONE final RedrawWindow reaching all children. The flag
// values below are tied to the real Win32 RDW_* constants by the static_asserts
// that follow, so the tested policy and the live wiring can never drift.
namespace redraw {

inline constexpr bool kRepaintChildOnMove = false;

inline constexpr unsigned kRdwInvalidate  = 0x0001u; // RDW_INVALIDATE
inline constexpr unsigned kRdwErase       = 0x0004u; // RDW_ERASE
inline constexpr unsigned kRdwAllChildren = 0x0080u; // RDW_ALLCHILDREN
inline constexpr unsigned kRdwUpdateNow   = 0x0100u; // RDW_UPDATENOW

inline constexpr unsigned FinalRepaintFlags() {
    return kRdwInvalidate | kRdwErase | kRdwUpdateNow | kRdwAllChildren;
}

// (fix thse isseue.md section 3) FinalRepaintCoversChildren() was an unused
// derived predicate with no production call site -- removed. The behavioural
// guarantee it described (the final repaint includes RDW_ALLCHILDREN) is still
// enforced by FinalRepaintFlags() above and pinned by tests/redraw_logic_test.cpp.

}  // namespace redraw

// Tie the tested policy constants to the real Win32 RDW_* bits so the mirror in
// tests/redraw_logic.h can never silently drift from what RedrawWindow sees.
static_assert(redraw::kRdwInvalidate  == RDW_INVALIDATE,  "RDW_INVALIDATE drift");
static_assert(redraw::kRdwErase       == RDW_ERASE,       "RDW_ERASE drift");
static_assert(redraw::kRdwAllChildren == RDW_ALLCHILDREN, "RDW_ALLCHILDREN drift");
static_assert(redraw::kRdwUpdateNow   == RDW_UPDATENOW,   "RDW_UPDATENOW drift");

// ---- Custom title-bar hit classification (fix thse isseue.md #2) ----
// Byte-for-byte mirror of tests/titlebar_logic.h. The Group popup is WS_POPUP
// (no native caption); WM_NCHITTEST feeds a client point + the band height + the
// three title-bar button boxes to Classify() to decide drag (Caption) vs a
// button vs client. Keep in lockstep with titlebar_logic.h.
namespace titlebar {

enum class Region { Client, Caption, Expand, Minimize, Close };

struct Box {
    int x = 0, y = 0, w = 0, h = 0;
    bool contains(int px, int py) const {
        return px >= x && px < x + w && py >= y && py < y + h;
    }
};

inline Region Classify(int px, int py, int bandH,
                       Box expand, Box minimize, Box close) {
    if (close.contains(px, py))    return Region::Close;
    if (minimize.contains(px, py)) return Region::Minimize;
    if (expand.contains(px, py))   return Region::Expand;
    if (py >= 0 && py < bandH)     return Region::Caption;
    return Region::Client;
}

}  // namespace titlebar

// Convert a CLIENT (content) size (96-dpi design px) to the OUTER window size
// (device px) for `dpi`, accounting for the WS_CAPTION | WS_SYSMENU non-client
// frame. ComputePromptLayout() returns CLIENT sizes; passing them straight to
// CreateWindowExW / SetWindowPos clipped the bottom action row (fix thse isseu
// e.md #1). AdjustWindowRectExForDpi (resolved dynamically -- AdjustWindowRectEx
// is the pre-Win10-1607 fallback) reports the frame; layout::Outer*FromClient
// adds it. Used at BOTH the create site and the ApplyPromptLayout relayout site
// so the two can never disagree.
static SIZE PromptOuterSize(int clientW96, int clientH96, DWORD style,
                            DWORD exStyle, UINT dpi) {
    const int cw = ScaleDpi(clientW96, dpi);
    const int ch = ScaleDpi(clientH96, dpi);
    RECT rc = {0, 0, cw, ch};
    bool adjusted = false;
    if (HMODULE u32 = GetModuleHandleW(L"user32.dll")) {
        using AdjForDpiFn = BOOL(WINAPI*)(LPRECT, DWORD, BOOL, DWORD, UINT);
        auto adj = reinterpret_cast<AdjForDpiFn>(
            GetProcAddress(u32, "AdjustWindowRectExForDpi"));
        if (adj) {
            adjusted = adj(&rc, style, FALSE, exStyle, dpi) != FALSE;
        }
    }
    if (!adjusted) {
        AdjustWindowRectEx(&rc, style, FALSE, exStyle);  // no-DPI fallback
    }
    const int ncExtraW = (rc.right - rc.left) - cw;
    const int ncExtraH = (rc.bottom - rc.top) - ch;
    SIZE out;
    out.cx = layout::OuterWidthFromClient(cw, ncExtraW);
    out.cy = layout::OuterHeightFromClient(ch, ncExtraH);
    return out;
}

// ---- Selected Items panel logic ----
// Byte-for-byte mirror of tests/selection_logic.h (UI spec item 1 / #12):
// selection summary + row decomposition (name / uppercased ext / human size).
// Keep in lockstep with selection_logic.h.
namespace sel {

struct Item {
    std::wstring name;
    bool isFolder = false;
    long long sizeBytes = -1;
    std::wstring path;   // fix thse isseue.md Issue 2: full filesystem path (empty for virtual items), used only for image-thumbnail extraction in grid views
};

inline size_t FileCount(const std::vector<Item>& items) {
    size_t n = 0;
    for (const auto& it : items) if (!it.isFolder) ++n;
    return n;
}
inline size_t FolderCount(const std::vector<Item>& items) {
    size_t n = 0;
    for (const auto& it : items) if (it.isFolder) ++n;
    return n;
}
inline std::wstring CountNoun(size_t n, const wchar_t* singular,
                              const wchar_t* plural) {
    return std::to_wstring(n) + L" " + (n == 1 ? singular : plural);
}
inline const wchar_t* CountSep() { return L" \u00b7 "; }
inline std::wstring SummaryTitle(size_t count) {
    return L"Selected items -- " + std::to_wstring(count);
}
// fix thse isseue.md P2/section 3: sel::SummaryCounts was the old plain-text
// summary builder, superseded by the live chip strip and with no call site --
// removed here; the pure helper stays test-only in tests/selection_logic.h.
// ---- Actionable type breakdown (fix thse isseu e.md #11) ----
// Byte-for-byte mirror of sel::TypeBreakdown in tests/selection_logic.h.
inline std::wstring ExtLabel(const std::wstring& name, bool isFolder);
inline std::wstring HumanSize(long long bytes);
inline std::wstring TypeBreakdown(const std::vector<Item>& items) {
    const std::wstring sep = CountSep();
    std::wstring out = CountNoun(items.size(), L"item", L"items");
    std::vector<std::wstring> order;
    std::vector<size_t>       counts;
    std::vector<bool>         isFolderGroup;
    auto bump = [&](const std::wstring& label, bool folderGroup) {
        for (size_t i = 0; i < order.size(); ++i) {
            if (order[i] == label && isFolderGroup[i] == folderGroup) {
                ++counts[i];
                return;
            }
        }
        order.push_back(label);
        counts.push_back(1);
        isFolderGroup.push_back(folderGroup);
    };
    long long totalBytes = 0;
    bool haveSize = false;
    for (const auto& it : items) {
        if (it.isFolder) {
            bump(L"folder", true);
        } else {
            std::wstring ext = ExtLabel(it.name, false);
            bump(ext.empty() ? L"FILE" : ext, false);
        }
        if (!it.isFolder && it.sizeBytes >= 0) {
            totalBytes += it.sizeBytes;
            haveSize = true;
        }
    }
    for (size_t i = 0; i < order.size(); ++i) {
        out += sep;
        if (isFolderGroup[i]) {
            out += CountNoun(counts[i], L"folder", L"folders");
        } else if (order[i] == L"FILE") {
            out += CountNoun(counts[i], L"FILE", L"FILES");
        } else {
            out += std::to_wstring(counts[i]) + L" " + order[i];
        }
    }
    if (haveSize && totalBytes > 0) {
        out += sep + HumanSize(totalBytes);
    }
    return out;
}
struct NameParts {
    std::wstring stem;
    std::wstring ext;
};
inline NameParts SplitName(const std::wstring& name, bool isFolder) {
    NameParts p;
    p.stem = name;
    if (isFolder) return p;
    const std::wstring::size_type dot = name.find_last_of(L'.');
    if (dot == std::wstring::npos || dot == 0 || dot + 1 >= name.size()) {
        return p;
    }
    p.stem = name.substr(0, dot);
    p.ext  = name.substr(dot + 1);
    return p;
}
inline std::wstring RowName(const std::wstring& name, bool isFolder) {
    return SplitName(name, isFolder).stem;
}
// The filename TEXT drawn in a Selected Items row, honoring the Show-extensions
// toggle (fix thse isseue.md Phase 1B): extensions ON renders the COMPLETE
// filename inline for files (e.g. "README.md"); OFF renders the stem. Folders
// always render their whole name regardless of the toggle. A separate extension
// pill is never drawn in either state.
inline std::wstring RowDisplayName(const std::wstring& name, bool isFolder,
                                   bool extInline) {
    if (extInline && !isFolder) return name;
    return RowName(name, isFolder);
}
inline std::wstring ExtLabel(const std::wstring& name, bool isFolder) {
    std::wstring ext = SplitName(name, isFolder).ext;
    for (auto& c : ext) c = static_cast<wchar_t>(towupper(c));
    return ext;
}
inline std::wstring HumanSize(long long bytes) {
    if (bytes < 0) return std::wstring();
    if (bytes < 1024) return std::to_wstring(bytes) + L" B";
    const wchar_t* units[] = { L"KB", L"MB", L"GB", L"TB" };
    long long unit = 1024;
    int idx = 0;
    while (idx < 3 && bytes >= unit * 1024) { unit *= 1024; ++idx; }
    const long long tenths = (bytes * 10 + unit / 2) / unit;
    if (tenths >= 100) {
        // Value is >= 10 in this unit: a whole number reads cleaner than a
        // trailing ".0" and keeps the badge short (e.g. "24 KB", "318 KB").
        const long long whole = (bytes + unit / 2) / unit;
        return std::to_wstring(whole) + L" " + units[idx];
    }
    return std::to_wstring(tenths / 10) + L"." + std::to_wstring(tenths % 10) +
           L" " + units[idx];
}

// Pick which byte count to display: prefer the filesystem's size-ON-DISK
// (allocated) metadata when available (allocated >= 0), else the logical file
// length, else unknown (-1). The actual metadata query lives in the Win32
// capture layer; this is only the selection rule so it can be unit-tested.
inline long long ChooseSizeBytes(long long logicalBytes, long long allocatedBytes) {
    if (allocatedBytes >= 0) return allocatedBytes;
    return logicalBytes;
}

}  // namespace sel

// ---- Auto Group eight-second countdown state model (ADD-ON) ----
// Byte-for-byte mirror of tests/auto_group_countdown_logic.h. Keep in lockstep.
// Pure: takes a MONOTONIC tick (the mod feeds GetTickCount64()) + the popup's
// start tick, so a stalled/busy message loop or a delayed WM_TIMER can never
// unlock the button early or late. Expiry NEVER runs the operation -- it only
// flips the button to Enabled; the user must click it.
namespace agcd {

enum class State {
    Unavailable,  // Auto Group disabled in settings -> button not shown/active
    Waiting,      // within the 8s safety delay -> disabled, shows a countdown
    Enabled,      // 8s elapsed -> clickable "Auto Group"
};

inline unsigned long long DelayMs() { return 8000; }

// fix thse isseue.md P2/section 3: agcd::TotalSeconds was an unused mirror (the
// countdown derives from DelayMs) -- removed here; it stays test-only.
inline State Resolve(bool enabledSetting, unsigned long long startTick, unsigned long long nowTick) {
    if (!enabledSetting) return State::Unavailable;
    const unsigned long long elapsed = (nowTick >= startTick) ? (nowTick - startTick) : 0;
    return (elapsed >= DelayMs()) ? State::Enabled : State::Waiting;
}

inline unsigned long long RemainingMs(bool enabledSetting, unsigned long long startTick, unsigned long long nowTick) {
    if (!enabledSetting) return 0;
    const unsigned long long elapsed = (nowTick >= startTick) ? (nowTick - startTick) : 0;
    return (elapsed >= DelayMs()) ? 0 : (DelayMs() - elapsed);
}

inline int RemainingSeconds(bool enabledSetting, unsigned long long startTick, unsigned long long nowTick) {
    const unsigned long long rem = RemainingMs(enabledSetting, startTick, nowTick);
    if (rem == 0) return 0;
    return static_cast<int>((rem + 999) / 1000);
}

inline std::wstring Label(State s, int remainingSeconds) {
    if (s == State::Waiting) {
        return L"Auto Group (" + std::to_wstring(remainingSeconds) + L")";
    }
    return L"Auto Group";
}

// fix thse isseue.md #5: STABLE M:SS countdown shown inside the button during
// the delay (minutes:seconds, seconds zero-padded, NO milliseconds). Driven by
// the whole-second RemainingSeconds value so the text changes at most once a
// second: 0:08, 0:07, 0:06, 0:05, 0:04, 0:03, 0:02, 0:01.
inline std::wstring CountdownText(int remainingSeconds) {
    if (remainingSeconds < 0) remainingSeconds = 0;
    const int m = remainingSeconds / 60;
    const int s = remainingSeconds % 60;
    std::wstring ss = std::to_wstring(s);
    if (ss.size() < 2) ss = L"0" + ss;
    return std::to_wstring(m) + L":" + ss;
}

// fix thse isseue.md Phase 4: the repaint timer must invalidate the Auto Group
// button ONLY when the VISIBLE whole-second countdown OR the enabled/disabled
// state actually changes -- never once per tick (that per-tick erase/repaint
// was the flashing cause). The WM_TIMER handler calls this before any
// InvalidateRect, so the anti-flicker contract is unit-tested, not inlined.
inline bool ShouldRepaint(int curSec, int lastSec, bool curEnabled,
                          bool lastEnabled) {
    return curSec != lastSec || curEnabled != lastEnabled;
}

inline bool Clickable(State s) { return s == State::Enabled; }

inline bool ShowOverlay(State s) { return s == State::Waiting; }

inline bool Visible(State s) { return s != State::Unavailable; }

}  // namespace agcd

// ---- Auto Group button APPEARANCE model (fix thse isseue.md top note) --------
// Byte-for-byte mirror of tests/auto_group_button_logic.h. Pure.
// Top note: "green color is not good for auto group - keep it same as group".
// So the Auto Group button NEVER uses a dedicated green surface. Once Enabled
// (8s safety delay elapsed) it is painted IDENTICALLY to the ordinary Group
// button -- the same create/primary tokens. Waiting/Unavailable -> the NEUTRAL
// (Cancel-style) secondary surface the bold M:SS countdown sits on. Caption/
// countdown ALWAYS semibold. Derived from the SAME agcd::State the clock
// resolves, so appearance and clickability never disagree.
namespace agbtn {

// GroupEnabled == the exact Group-button surface (create/primary tokens).
enum class Fill { NeutralWaiting, GroupEnabled };

// Enabled (safety delay elapsed) -> Group-button surface; Waiting/Unavailable
// -> neutral Cancel-style surface the countdown sits on.
inline Fill ResolveFill(agcd::State s) {
    return s == agcd::State::Enabled ? Fill::GroupEnabled : Fill::NeutralWaiting;
}

// True once clickable: painted with the SAME tokens as the Group button (no
// green). False while the safety delay counts down.
inline bool UsesGroupFill(agcd::State s) {
    return ResolveFill(s) == Fill::GroupEnabled;
}

// Phase 5A lists Auto Group among the semibold action labels, and the countdown
// was already bold -- so BOTH states draw their text with the semibold UI font.
inline bool CaptionSemibold(agcd::State) { return true; }

}  // namespace agbtn

// ---- Selected Items PRESENTATION-MODE model (fix thse isseue.md Phase A) -----
// Byte-for-byte mirror of tests/view_mode_logic.h. Pure. Three Explorer-like
// modes (List / Large icons / Extra large icons). Ctrl+Wheel zooms between them
// (wheel up -> larger, clamped at the ends, no wrap); normal wheel scrolls and
// Shift+wheel pans chips -- the routing helpers keep the three gestures
// mutually exclusive. Geometry is 96-DPI base; the painter DPI-scales it. The
// selection model/order never changes with the mode -- only presentation does.
namespace viewmode {

enum class Mode { List = 0, Large = 1, ExtraLarge = 2 };

inline int ModeCount() { return 3; }

inline Mode Step(Mode m, int steps) {
    int v = static_cast<int>(m) + steps;
    if (v < 0) v = 0;
    if (v > 2) v = 2;
    return static_cast<Mode>(v);
}

inline Mode OnCtrlWheel(Mode m, int wheelDelta) {
    const int notches = wheelDelta / 120;   // truncates toward zero
    return Step(m, notches);                // wheel up (+notches) -> larger
}

inline bool ZoomsOnWheel(bool ctrlDown) { return ctrlDown; }
inline bool PansChipsOnWheel(bool ctrlDown, bool shiftDown) {
    return !ctrlDown && shiftDown;
}
inline bool ScrollsListOnWheel(bool ctrlDown, bool shiftDown) {
    return !ctrlDown && !shiftDown;
}

// fix thse isseue.md Issue 5: LOCATION-aware wheel routing (byte-for-byte mirror
// of view_mode_logic.h). A PLAIN wheel over the chip strip pans WITHOUT Shift;
// Ctrl keeps zoom priority; at most ONE of {zoom, pan, scroll} per event.
inline bool WheelZooms(bool ctrlDown, bool horizontal) {
    return ctrlDown && !horizontal;
}
inline bool WheelPansChips(bool ctrlDown, bool overStrip, bool horizontal) {
    return !WheelZooms(ctrlDown, horizontal) && overStrip;
}
inline bool WheelScrollsRows(bool ctrlDown, bool overStrip, bool overRows,
                             bool horizontal) {
    if (WheelZooms(ctrlDown, horizontal)) return false;
    if (overStrip) return false;
    return overRows && !horizontal;
}

inline bool IsGrid(Mode m) { return m != Mode::List; }

inline int IconPx(Mode m) {
    switch (m) {
        case Mode::List:       return 16;
        case Mode::Large:      return 48;
        case Mode::ExtraLarge: return 96;
    }
    return 16;
}

inline int RowHeight(Mode m) { return m == Mode::List ? 24 : 0; }

inline int CellPadX(Mode m) { return IsGrid(m) ? 8 : 0; }
inline int CellPadY(Mode m) { return IsGrid(m) ? 6 : 0; }
inline int LabelBandH(Mode m) { return IsGrid(m) ? 30 : 0; }

inline int CellW(Mode m) {
    if (!IsGrid(m)) return 0;
    // Constant 8px horizontal breathing for EVERY grid mode: Ctrl+wheel zoom
    // grows only the icon (IconPx), never the surrounding padding or text area.
    return IconPx(m) + 2 * CellPadX(m) + 8;
}
inline int CellH(Mode m) {
    if (!IsGrid(m)) return 0;
    return IconPx(m) + 2 * CellPadY(m) + LabelBandH(m);
}

inline int Columns(Mode m, int availW) {
    if (!IsGrid(m)) return 1;
    const int cw = CellW(m);
    if (cw <= 0 || availW <= 0) return 1;
    const int c = availW / cw;
    return c < 1 ? 1 : c;
}

inline int GridRows(int itemCount, int cols) {
    if (cols < 1) cols = 1;
    if (itemCount <= 0) return 0;
    return (itemCount + cols - 1) / cols;
}

inline int VisibleGridRows(Mode m, int viewportH) {
    const int ch = CellH(m);
    if (ch <= 0 || viewportH <= 0) return 0;
    return viewportH / ch;
}

inline int ContentHeight(Mode m, int itemCount, int availW) {
    if (!IsGrid(m)) return itemCount * RowHeight(m);
    const int cols = Columns(m, availW);
    return GridRows(itemCount, cols) * CellH(m);
}

// fix thse isseue.md v0.12.67 (Issue #1 B): how many GRID rows the Selected
// viewport should show for `itemCount` items at `cols` columns, given a
// vertical pixel budget. Whole cells only (never a clipped row), always at
// least one row, and never more rows than the items actually need -- so a small
// selection content-sizes (no dead space) while a large one is capped to the
// budget and the remainder scrolls. List mode returns 0 (it is not a grid).
inline int SelectedGridVisibleRows(Mode m, int itemCount, int cols,
                                   int budgetPx) {
    if (!IsGrid(m)) return 0;
    const int ch = CellH(m);
    if (ch <= 0) return 0;
    int fit = budgetPx / ch;          // whole cells only -> no clipped row
    if (fit < 1) fit = 1;             // always at least one row visible
    const int need = GridRows(itemCount, cols);
    const int want = need < 1 ? 1 : need;
    return want < fit ? want : fit;
}

inline int FirstVisibleItem(Mode oldMode, int topRow, int oldCols) {
    if (oldMode == Mode::List) return topRow;
    return topRow * (oldCols < 1 ? 1 : oldCols);
}
inline int TopRowForItem(Mode newMode, int itemIndex, int newCols) {
    if (itemIndex < 0) itemIndex = 0;
    if (newMode == Mode::List) return itemIndex;
    const int c = newCols < 1 ? 1 : newCols;
    return itemIndex / c;
}

// fix thse isseue.md v0.12.68: minimum WHOLE-cell height for a mode -- one grid
// cell for the icon grids, one list row for List. Used to reserve space for at
// least one complete cell (never a blank grid) and to derive the Selected-Items
// minimum content height instead of a fixed pixel floor. Keep in lockstep with
// tests/view_mode_logic.h.
inline int MinCellH(Mode m) { return IsGrid(m) ? CellH(m) : RowHeight(m); }

// The LARGEST view mode whose one whole cell fits in `availRowsH` px of rows
// viewport. Falls back to List (which always shows at least one short row), so
// a too-small window degrades gracefully to a mode that can still show a cell
// rather than painting nothing. Order: Extra large > Large > List.
inline Mode LargestModeThatFits(int availRowsH) {
    if (availRowsH >= CellH(Mode::ExtraLarge)) return Mode::ExtraLarge;
    if (availRowsH >= CellH(Mode::Large))      return Mode::Large;
    return Mode::List;
}

// Minimum Selected-Items CONTENT height for a mode: the header block plus ONE
// whole cell/row. Replaces the old fixed 40px reflow floor so the lower cards
// and footer are never overlapped by a half-painted cell.
inline int MinSelectedContentH(Mode m, int headerBlockH) {
    return headerBlockH + MinCellH(m);
}

// Overlap guard: does one whole cell/row of this mode fit in `availRowsH` px?
inline bool CellFits(Mode m, int availRowsH) { return MinCellH(m) <= availRowsH; }

}  // namespace viewmode

// ---- Auto Group classification + destination-planning model (ADD-ON) ----
// Byte-for-byte mirror of tests/auto_group_logic.h (lines 36..497). Keep in
// lockstep. Pure planning only -- no Win32, no filesystem; the worker calls
// BuildPlan() and executes the resulting buckets via PerformGroupOperation.
namespace autogroup {

// ---------------------------------------------------------------------------
// Path helpers (pure, separator-normalised). Windows paths use backslash but
// the mod also tolerates forward slashes, so these treat both as separators.
// Kept local so this model is self-contained and testable on its own.
// ---------------------------------------------------------------------------

inline bool IsSep(wchar_t c) { return c == L'\\' || c == L'/'; }

// Strip one trailing separator ("C:\a\" -> "C:\a"); a bare drive root keeps it.
inline std::wstring TrimSep(const std::wstring& p) {
    if (p.size() > 1 && IsSep(p[p.size() - 1])) {
        if (p.size() == 3 && p[1] == L':') return p;  // keep "C:\"
        return p.substr(0, p.size() - 1);
    }
    return p;
}

// Parent directory of a path (everything before the last separator). Empty when
// there is no separator or the path is only a drive root.
inline std::wstring ParentDir(const std::wstring& path) {
    std::wstring p = TrimSep(path);
    if (p.size() <= 3 && p.size() >= 2 && p[1] == L':') return std::wstring();
    std::wstring::size_type pos = std::wstring::npos;
    for (std::wstring::size_type i = p.size(); i > 0; --i) {
        if (IsSep(p[i - 1])) { pos = i - 1; break; }
    }
    if (pos == std::wstring::npos) return std::wstring();
    if (pos == 2 && p[1] == L':') return p.substr(0, 3);  // "C:\a" -> "C:\"
    return p.substr(0, pos);
}

// Case-insensitive equality for ASCII paths (Windows paths are case-insensitive).
inline bool PathEqualCI(const std::wstring& a, const std::wstring& b) {
    if (a.size() != b.size()) return false;
    for (std::size_t i = 0; i < a.size(); ++i) {
        wchar_t ca = a[i], cb = b[i];
        if (ca >= L'A' && ca <= L'Z') ca = static_cast<wchar_t>(ca - L'A' + L'a');
        if (cb >= L'A' && cb <= L'Z') cb = static_cast<wchar_t>(cb - L'A' + L'a');
        // Normalise separators so '\\' and '/' compare equal.
        if (IsSep(ca)) ca = L'\\';
        if (IsSep(cb)) cb = L'\\';
        if (ca != cb) return false;
    }
    return true;
}

// Join a directory and a child name with a single backslash.
inline std::wstring Join(const std::wstring& dir, const std::wstring& child) {
    std::wstring d = TrimSep(dir);
    if (d.empty()) return child;
    return d + L"\\" + child;
}

// ---------------------------------------------------------------------------
// #1  Canonical extension-bucket folder name.
// Normalises an item's extension to an UPPERCASE folder name. JPG and JPEG map
// to the SAME canonical bucket "JPG" (no duplicate image folders). Works for
// new / uncommon extensions -- there is NO hardcoded whitelist. Folders and
// extensionless files return an empty string (they are not extension buckets).
// ---------------------------------------------------------------------------

// Lowercase + strip a single leading dot. "MD"/".md"/"Md" -> "md".
inline std::wstring NormKey(const std::wstring& extLabel) {
    std::wstring e = extLabel;
    std::size_t start = (!e.empty() && e[0] == L'.') ? 1u : 0u;
    std::wstring out;
    for (std::size_t i = start; i < e.size(); ++i) {
        wchar_t c = e[i];
        if (c >= L'A' && c <= L'Z') c = static_cast<wchar_t>(c - L'A' + L'a');
        out.push_back(c);
    }
    return out;
}

// Uppercase a normalized key for display. "md" -> "MD".
inline std::wstring Upper(const std::wstring& key) {
    std::wstring out;
    for (wchar_t c : key) {
        if (c >= L'a' && c <= L'z') c = static_cast<wchar_t>(c - L'a' + L'A');
        out.push_back(c);
    }
    return out;
}

// Canonical lowercase bucket KEY for an item. ".jpeg" collapses to "jpg" so a
// single image bucket holds both. Returns "" for folders / extensionless files.
inline std::wstring CanonicalKey(const std::wstring& name, bool isFolder) {
    if (isFolder) return std::wstring();
    std::wstring key = NormKey(sel::ExtLabel(name, false));
    if (key.empty()) return std::wstring();
    if (key == L"jpeg" || key == L"jpe") key = L"jpg";  // canonicalise JPEG->JPG
    return key;
}

// Canonical UPPERCASE bucket FOLDER NAME for an item (e.g. "JPG"). Empty for
// folders / extensionless files.
inline std::wstring CanonicalExtFolder(const std::wstring& name, bool isFolder) {
    return Upper(CanonicalKey(name, isFolder));
}

// ---------------------------------------------------------------------------
// #2  Image-extension category (explicit, tested). Only these go under the
// configured Images parent in Mode B. Keys are the CANONICAL keys, so "jpeg"
// is already folded into "jpg" before this is consulted.
// ---------------------------------------------------------------------------

inline bool IsImageKey(const std::wstring& canonicalKey) {
    static const wchar_t* kImageKeys[] = {
        L"jpg",  L"png",  L"gif",  L"webp", L"bmp",  L"tif",  L"tiff",
        L"heic", L"heif", L"avif", L"ico",  L"svg",  L"jfif", L"raw",
    };
    for (const wchar_t* k : kImageKeys) {
        if (canonicalKey == k) return true;
    }
    return false;
}

// Convenience overload taking a bucket FOLDER NAME (e.g. "JPG").
inline bool IsImageBucket(const std::wstring& bucketFolder) {
    return IsImageKey(NormKey(bucketFolder));
}

// ---------------------------------------------------------------------------
// #3  Folder-name validation + sanitization.
// A bucket folder name is derived from an extension (and extensions can be odd:
// long, containing invalid characters, or spelling a reserved device name).
// Reject names we cannot represent safely rather than silently producing a bad
// path. Returns true + the sanitized name when usable; false when the name must
// be rejected (so the caller can safely skip that bucket).
// ---------------------------------------------------------------------------

inline bool IsReservedDeviceName(const std::wstring& nameUpper) {
    static const wchar_t* kReserved[] = {
        L"CON", L"PRN", L"AUX", L"NUL",
        L"COM1", L"COM2", L"COM3", L"COM4", L"COM5",
        L"COM6", L"COM7", L"COM8", L"COM9",
        L"LPT1", L"LPT2", L"LPT3", L"LPT4", L"LPT5",
        L"LPT6", L"LPT7", L"LPT8", L"LPT9",
    };
    for (const wchar_t* r : kReserved) {
        if (nameUpper == r) return true;
    }
    return false;
}

inline bool IsInvalidNameChar(wchar_t c) {
    // Windows forbids these in a path component, plus control chars.
    if (c < 0x20) return true;
    switch (c) {
        case L'<': case L'>': case L':': case L'"': case L'/':
        case L'\\': case L'|': case L'?': case L'*':
            return true;
        default:
            return false;
    }
}

// Longest path component we will accept for a generated bucket (well under
// MAX_PATH component limits, leaving room for the destination root).
inline std::size_t MaxBucketNameLen() { return 64u; }

struct SanitizeResult {
    bool ok = false;             // false => reject this bucket entirely
    std::wstring name;           // sanitized name when ok
};

// Validate/sanitize a proposed bucket folder name. The input is normally an
// uppercase extension bucket ("JPG") or a configured name ("No Extension",
// "Images", "All Folder"). Trailing dots/spaces are trimmed (Windows strips
// them anyway). Any invalid character, an empty result, a reserved device name,
// or an over-long name is REJECTED (ok=false) so the caller can skip it rather
// than silently merging unrelated types.
inline SanitizeResult SanitizeBucketName(const std::wstring& proposed) {
    SanitizeResult r;
    std::wstring s = proposed;

    // Trim leading/trailing whitespace.
    std::size_t b = 0, e = s.size();
    while (b < e && (s[b] == L' ' || s[b] == L'\t')) ++b;
    while (e > b && (s[e - 1] == L' ' || s[e - 1] == L'\t')) --e;
    s = s.substr(b, e - b);

    // Windows strips trailing dots and spaces from a component; a name that is
    // ONLY dots/spaces becomes empty and must be rejected.
    while (!s.empty() && (s.back() == L'.' || s.back() == L' ')) s.pop_back();

    if (s.empty()) return r;                 // nothing usable
    if (s.size() > MaxBucketNameLen()) return r;

    for (wchar_t c : s) {
        if (IsInvalidNameChar(c)) return r;  // reject, do not substitute
    }

    // Reserved device names are compared case-insensitively, bare or with any
    // extension (e.g. "CON", "con.txt") -- here the component is a bare bucket.
    std::wstring upper = Upper(NormKey(s.find(L'.') == std::wstring::npos
                                           ? s
                                           : s.substr(0, s.find(L'.'))));
    if (IsReservedDeviceName(upper)) return r;

    r.ok = true;
    r.name = s;
    return r;
}

// ---------------------------------------------------------------------------
// #4  Dedicated Auto Group destination resolver.
// Precedence (NEVER uses g_lastGroupTarget or the last typed name):
//   1. configured Fixed destination folder, if set.
//   2. else the common filesystem parent of the selection (if they share one).
//   3. else CANNOT resolve -> safe cancel (needs a common parent or a
//      configured destination).
// ---------------------------------------------------------------------------

enum class DestStatus {
    Fixed,          // used the configured fixed destination
    CommonParent,   // used the common parent of the selection
    NoCommonParent, // selection spans multiple parents, no fixed dest -> cancel
    EmptySelection, // nothing selected -> cancel
};

struct DestResult {
    DestStatus status = DestStatus::EmptySelection;
    std::wstring root;              // destination root (empty unless resolved)
    bool resolved() const {
        return status == DestStatus::Fixed || status == DestStatus::CommonParent;
    }
};

// One captured selection entry for planning: full path + whether it is a folder.
struct SrcItem {
    std::wstring path;      // full path, e.g. "C:\pics\a.jpg"
    bool isFolder = false;
};

// Common parent of all selected items (the directory each item lives in).
// Returns empty when the items do not all share one parent directory.
inline std::wstring CommonParent(const std::vector<SrcItem>& items) {
    if (items.empty()) return std::wstring();
    std::wstring parent = ParentDir(items[0].path);
    if (parent.empty()) return std::wstring();
    for (std::size_t i = 1; i < items.size(); ++i) {
        std::wstring p = ParentDir(items[i].path);
        if (p.empty() || !PathEqualCI(p, parent)) return std::wstring();
    }
    return parent;
}

inline DestResult ResolveAutoDest(const std::vector<SrcItem>& items,
                                  const std::wstring& fixedDest) {
    DestResult out;
    if (items.empty()) {
        out.status = DestStatus::EmptySelection;
        return out;
    }
    if (!fixedDest.empty()) {
        out.status = DestStatus::Fixed;
        out.root = TrimSep(fixedDest);
        return out;
    }
    std::wstring parent = CommonParent(items);
    if (parent.empty()) {
        out.status = DestStatus::NoCommonParent;
        return out;
    }
    out.status = DestStatus::CommonParent;
    out.root = TrimSep(parent);
    return out;
}

// ---------------------------------------------------------------------------
// #5  Deterministic plan.
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Phase 6: Custom Auto Group rules. A rule routes files whose extension matches
// into a NAMED output bucket instead of the default per-extension folder. Each
// rule is one string "FolderName=ext1|ext2|ext3". Pure + unit-tested; no
// filesystem access and NO dependency on the New Folder template settings.
//   - Empty rule list -> behaviour is unchanged (no rule ever matches).
//   - Unmatched exts   -> still go to their canonical per-extension folder.
//   - Matching is case-insensitive; jpeg/jpe collapse to jpg like CanonicalKey.
//   - First valid rule wins when two rules claim the same extension.
//   - Folder names validated with SanitizeBucketName (reserved/invalid/empty
//     rejected); a duplicate rule folder name is rejected (ambiguous target).
//   - Documented caps clamp the rule and per-rule extension counts.
// ---------------------------------------------------------------------------

inline std::size_t MaxCustomRules() { return 64u; }
inline std::size_t MaxExtsPerRule() { return 64u; }

enum class RuleImageClass { AllNonImage, AllImage, Mixed };

struct CustomRule {
    std::wstring folder;                  // sanitized bucket folder name
    std::vector<std::wstring> keys;       // canonical lowercase ext keys (deduped)
    RuleImageClass imageClass = RuleImageClass::AllNonImage;
};

enum class RuleReject {
    None,
    Malformed,        // no '=', empty folder, or empty extension list
    BadFolderName,    // folder failed SanitizeBucketName
    DuplicateFolder,  // a later rule reuses an earlier rule's folder name
    LimitReached,     // rule / extension cap hit (extras dropped)
};

struct RuleRejectLog {
    std::wstring text;                    // offending raw line
    RuleReject reason = RuleReject::None;
};

struct ParsedRules {
    std::vector<CustomRule> rules;              // valid, first-seen order
    std::vector<std::wstring> dupExtDropped;    // ext keys ignored (first rule wins)
    std::vector<RuleRejectLog> rejected;        // rejected lines, for the log
    bool empty() const { return rules.empty(); }
};

// Canonicalize an extension TOKEN exactly as CanonicalKey normalizes a file's
// extension (lowercase, drop a leading dot, jpeg/jpe -> jpg) so a rule token
// matches the key a real file of that type produces.
inline std::wstring CanonicalizeExtToken(const std::wstring& token) {
    std::wstring key = NormKey(token);
    if (key == L"jpeg" || key == L"jpe") key = L"jpg";
    return key;
}

// Split "Folder=ext1|ext2" into folder + raw ext tokens. Returns false when the
// line has no '='. Blanks on the folder are left for the sanitizer; empty ext
// tokens are dropped by the caller.
inline bool SplitRuleLine(const std::wstring& line, std::wstring& folderOut,
                          std::vector<std::wstring>& extTokensOut) {
    folderOut.clear();
    extTokensOut.clear();
    const std::wstring::size_type eq = line.find(L'=');
    if (eq == std::wstring::npos) return false;
    folderOut = line.substr(0, eq);
    const std::wstring rest = line.substr(eq + 1);
    std::wstring cur;
    for (wchar_t c : rest) {
        if (c == L'|') { extTokensOut.push_back(cur); cur.clear(); }
        else cur.push_back(c);
    }
    extTokensOut.push_back(cur);
    return true;
}

// Classify a rule's canonical keys as all-image / all-non-image / mixed.
inline RuleImageClass ClassifyRuleKeys(const std::vector<std::wstring>& keys) {
    bool anyImg = false, anyNon = false;
    for (const auto& k : keys) {
        if (IsImageKey(k)) anyImg = true; else anyNon = true;
    }
    if (anyImg && anyNon) return RuleImageClass::Mixed;
    return anyImg ? RuleImageClass::AllImage : RuleImageClass::AllNonImage;
}

// Parse the raw settings array (one "Folder=ext|ext" per entry) into validated
// rules plus a log of everything ignored. Blank entries are skipped silently
// (the empty default slot). First rule wins on a duplicate extension.
inline ParsedRules ParseCustomRules(const std::vector<std::wstring>& rawLines) {
    ParsedRules out;
    auto folderTaken = [&](const std::wstring& f) {
        for (const auto& r : out.rules) if (PathEqualCI(r.folder, f)) return true;
        return false;
    };
    auto extTaken = [&](const std::wstring& k) {
        for (const auto& r : out.rules)
            for (const auto& e : r.keys) if (e == k) return true;
        return false;
    };

    for (const auto& rawLine : rawLines) {
        bool blank = true;
        for (wchar_t c : rawLine) if (c != L' ' && c != L'\t') { blank = false; break; }
        if (blank) continue;

        if (out.rules.size() >= MaxCustomRules()) {
            out.rejected.push_back({rawLine, RuleReject::LimitReached});
            continue;
        }

        std::wstring folderRaw;
        std::vector<std::wstring> tokens;
        if (!SplitRuleLine(rawLine, folderRaw, tokens)) {
            out.rejected.push_back({rawLine, RuleReject::Malformed});
            continue;
        }

        SanitizeResult fsan = SanitizeBucketName(folderRaw);
        if (!fsan.ok) {
            out.rejected.push_back({rawLine, RuleReject::BadFolderName});
            continue;
        }
        if (folderTaken(fsan.name)) {
            out.rejected.push_back({rawLine, RuleReject::DuplicateFolder});
            continue;
        }

        CustomRule rule;
        rule.folder = fsan.name;
        for (const auto& tok : tokens) {
            const std::wstring key = CanonicalizeExtToken(tok);
            if (key.empty()) continue;                 // drop empty ext token
            if (rule.keys.size() >= MaxExtsPerRule()) {
                out.rejected.push_back({rawLine, RuleReject::LimitReached});
                break;
            }
            bool already = extTaken(key);
            for (const auto& e : rule.keys) if (e == key) already = true;
            if (already) { out.dupExtDropped.push_back(key); continue; }
            rule.keys.push_back(key);
        }

        if (rule.keys.empty()) {
            out.rejected.push_back({rawLine, RuleReject::Malformed});
            continue;
        }
        rule.imageClass = ClassifyRuleKeys(rule.keys);
        out.rules.push_back(rule);
    }
    return out;
}

// Match a file's canonical key against the parsed rules. `imagesParentEnabled`
// gates the one layout-dependent safety rule: a MIXED-image rule is ambiguous
// under the Images-parent layout, so it is NOT honored there (the file falls
// back to its default per-extension bucket). In Flat mode image-ness is
// irrelevant, so a mixed rule routes normally.
struct RuleMatch {
    bool matched = false;
    std::wstring folder;
    bool isImage = false;
};
inline RuleMatch MatchCustomRule(const ParsedRules& pr,
                                 const std::wstring& canonicalKey,
                                 bool imagesParentEnabled) {
    RuleMatch m;
    for (const auto& r : pr.rules) {
        for (const auto& k : r.keys) {
            if (k != canonicalKey) continue;
            if (imagesParentEnabled && r.imageClass == RuleImageClass::Mixed) {
                return m;   // ambiguous under Images-parent -> fall back
            }
            m.matched = true;
            m.folder = r.folder;
            m.isImage = (r.imageClass == RuleImageClass::AllImage);
            return m;
        }
    }
    return m;
}

enum class Layout { Flat, ImagesParent };

// ---------------------------------------------------------------------------
// Settings-parsing helpers (pure, so the mod's LoadSettings and the unit tests
// share ONE implementation). These never touch the Group settings -- they only
// decode the Auto Group $options string and the configurable folder names.
// ---------------------------------------------------------------------------

// Map the autoGroupLayout $option identifier to the layout enum. Only the exact
// identifier "imagesParent" selects the Images-parent layout; anything else
// (including null/blank/unknown) is the safe Flat default.
inline Layout ParseLayout(const wchar_t* option) {
    if (option && std::wstring(option) == L"imagesParent") return Layout::ImagesParent;
    return Layout::Flat;
}

// Trim surrounding ASCII whitespace from a configured name; when the result is
// empty, fall back to the documented default so a blank setting can never
// produce an empty bucket path.
inline std::wstring NameOrDefault(const std::wstring& raw, const std::wstring& fallback) {
    std::size_t b = 0, e = raw.size();
    while (b < e && (raw[b] == L' ' || raw[b] == L'\t')) ++b;
    while (e > b && (raw[e - 1] == L' ' || raw[e - 1] == L'\t')) --e;
    std::wstring s = raw.substr(b, e - b);
    return s.empty() ? fallback : s;
}

struct Settings {
    Layout layout = Layout::Flat;
    std::wstring imagesParentName = L"Images";
    bool groupSelectedFolders = false;        // move selected folders -> AllFolder
    std::wstring allFolderName = L"All Folder";
    bool noExtensionEnabled = true;           // put extensionless files in a bucket
    std::wstring noExtensionName = L"No Extension";
    bool isMove = true;                        // Copy/Move for FILES
    ParsedRules customRules;                   // Phase 6: optional extension->folder rules
};

// One planned file move/copy: a source file routed into a destination directory.
struct FileOp {
    std::wstring src;        // source file path
    std::wstring destDir;    // full destination directory (bucket) path
    std::wstring bucket;     // the bucket folder name (last path component)
};

// One planned selected-folder move (only when groupSelectedFolders is on).
struct FolderOp {
    std::wstring src;        // selected folder path
    std::wstring destDir;    // the "All Folder" directory
};

enum class Skip {
    None,
    Rejected,         // bucket name failed sanitization
    NoExtDisabled,    // extensionless file, No-Extension bucket turned off
    SelfRecursive,    // folder move would be recursive / into itself
};

struct SkippedItem {
    std::wstring path;
    Skip reason = Skip::None;
};

struct Plan {
    DestResult dest;
    std::vector<FileOp> files;
    std::vector<FolderOp> folders;
    std::vector<std::wstring> dirsToCreate;   // deterministic, de-duplicated
    std::vector<SkippedItem> skipped;
    bool ok = false;    // true only when the destination resolved
};

// Resolve the full destination directory for a file bucket under the layout.
// Image buckets go under <root>\<ImagesParent>\<BUCKET> in ImagesParent mode;
// everything else (and all buckets in Flat mode) go to <root>\<BUCKET>.
inline std::wstring BucketDir(const std::wstring& root,
                              const std::wstring& bucketName,
                              bool isImage,
                              const Settings& s) {
    if (s.layout == Layout::ImagesParent && isImage) {
        return Join(Join(root, s.imagesParentName), bucketName);
    }
    return Join(root, bucketName);
}

// Build the deterministic Auto Group plan. Pure: no filesystem access. Order is
// stable -- files keep their captured (first-seen) order and each new
// destination directory is appended to dirsToCreate the first time it appears.
inline Plan BuildPlan(const std::vector<SrcItem>& items,
                      const std::wstring& fixedDest,
                      const Settings& s) {
    Plan plan;
    plan.dest = ResolveAutoDest(items, fixedDest);
    if (!plan.dest.resolved()) {
        plan.ok = false;
        return plan;
    }
    const std::wstring root = plan.dest.root;

    // Validate the configured special folder names up-front so a bad config is
    // reported once rather than per item.
    SanitizeResult imgSan = SanitizeBucketName(s.imagesParentName);
    SanitizeResult allSan = SanitizeBucketName(s.allFolderName);
    SanitizeResult noExtSan = SanitizeBucketName(s.noExtensionName);

    auto addDir = [&](const std::wstring& dir) {
        for (const auto& d : plan.dirsToCreate) {
            if (PathEqualCI(d, dir)) return;
        }
        plan.dirsToCreate.push_back(dir);
    };

    for (const auto& it : items) {
        if (it.isFolder) continue;   // folders handled separately below

        const std::wstring key = CanonicalKey(it.path, false);

        if (key.empty()) {
            // Extensionless file.
            if (!s.noExtensionEnabled || !noExtSan.ok) {
                plan.skipped.push_back({it.path,
                    noExtSan.ok ? Skip::NoExtDisabled : Skip::Rejected});
                continue;
            }
            const std::wstring dir = Join(root, noExtSan.name);
            addDir(dir);
            plan.files.push_back({it.path, dir, noExtSan.name});
            continue;
        }

        // Phase 6: a custom rule may override the default per-extension bucket
        // for this file's extension (first matching rule wins). A mixed-image
        // rule is ignored under Images-parent layout (handled in the matcher),
        // so a match here is uniformly image or non-image.
        const RuleMatch cr = MatchCustomRule(s.customRules, key,
                                             s.layout == Layout::ImagesParent);
        const std::wstring bucket = cr.matched ? cr.folder : Upper(key);
        SanitizeResult san = SanitizeBucketName(bucket);
        if (!san.ok) {
            plan.skipped.push_back({it.path, Skip::Rejected});
            continue;
        }
        const bool img = cr.matched ? cr.isImage : IsImageKey(key);
        // In ImagesParent mode an image bucket also needs the parent name valid.
        if (s.layout == Layout::ImagesParent && img && !imgSan.ok) {
            plan.skipped.push_back({it.path, Skip::Rejected});
            continue;
        }
        const std::wstring dir = BucketDir(root, san.name, img, s);
        // Make sure the Images parent itself is created before its children.
        if (s.layout == Layout::ImagesParent && img) {
            addDir(Join(root, imgSan.name));
        }
        addDir(dir);
        plan.files.push_back({it.path, dir, san.name});
    }

    // Selected folders: untouched by default. Only when grouping is enabled do
    // we move the ORIGINAL captured folders into the All Folder directory. The
    // generated output buckets above are NEVER moved into All Folder (we only
    // ever consider items from the immutable captured selection here).
    if (s.groupSelectedFolders && allSan.ok) {
        const std::wstring allDir = Join(root, allSan.name);
        bool anyFolder = false;
        for (const auto& it : items) {
            if (!it.isFolder) continue;
            // Reject a recursive / self move: source folder is the All Folder
            // target, contains it, or is contained by it.
            const std::wstring src = TrimSep(it.path);
            if (PathEqualCI(src, allDir) ||
                PathEqualCI(src, TrimSep(root)) ||
                // src is an ancestor of allDir (would move a parent into child)
                (allDir.size() > src.size() &&
                 PathEqualCI(allDir.substr(0, src.size()), src) &&
                 IsSep(allDir[src.size()]))) {
                plan.skipped.push_back({it.path, Skip::SelfRecursive});
                continue;
            }
            plan.folders.push_back({it.path, allDir});
            anyFolder = true;
        }
        if (anyFolder) addDir(allDir);
    } else if (s.groupSelectedFolders && !allSan.ok) {
        for (const auto& it : items) {
            if (it.isFolder) plan.skipped.push_back({it.path, Skip::Rejected});
        }
    }

    plan.ok = true;
    return plan;
}

}  // namespace autogroup

// ---- Selected Items list scroll model ----
// Byte-for-byte mirror of tests/list_scroll_logic.h. Keep in lockstep.
namespace listscroll {

inline int VisibleRows(int viewportH, int rowH) {
    if (rowH <= 0) return 0;
    return viewportH / rowH;
}
// Reconcile the per-mode DECLARED row cap with the TRUE capacity of the live
// viewport (fix thse isseue.md Phase 1A): never show MORE rows than physically
// fit (so the last row is never silently clipped), never exceed the declared
// cap, and fall back to the declared cap when the viewport is not yet
// measurable (height or rowH is 0) so the list never collapses to zero rows.
inline int EffectiveVisibleRows(int declaredCap, int viewportH, int rowH) {
    const int phys = VisibleRows(viewportH, rowH);
    if (phys <= 0) return declaredCap;
    return phys < declaredCap ? phys : declaredCap;
}

// fix thse isseue.md v0.12.69 spec section 2 (P0 dead space): when the popup is
// USER-SIZED (maximized or manually resized), the List viewport should show as
// many WHOLE rows as PHYSICALLY FIT -- filling the enlarged Selected Items card
// instead of leaving a blank tail below the compact 4-row / expanded 9-row cap.
// When NOT user-sized this is byte-identical to EffectiveVisibleRows (the
// declared compact/expanded cap is preserved). An unmeasured viewport (height
// or rowH 0) still falls back to the declared cap so the list never collapses.
inline int EffectiveVisibleRowsResizable(int declaredCap, int viewportH,
                                         int rowH, bool userSized) {
    const int phys = VisibleRows(viewportH, rowH);
    if (phys <= 0) return declaredCap;        // not yet measurable
    if (userSized) return phys;               // fill the enlarged viewport (no cap)
    return phys < declaredCap ? phys : declaredCap;
}
inline int MaxTopRow(int itemCount, int visibleRows) {
    int m = itemCount - visibleRows;
    return m > 0 ? m : 0;
}
inline bool NeedsScrollbar(int itemCount, int visibleRows) {
    return itemCount > visibleRows;
}
inline int ClampTopRow(int topRow, int maxTop) {
    if (topRow < 0) return 0;
    if (topRow > maxTop) return maxTop;
    return topRow;
}
enum class Key { Up, Down, Home, End, PageUp, PageDown };
inline int OnKey(Key k, int topRow, int itemCount, int visibleRows) {
    const int maxTop = MaxTopRow(itemCount, visibleRows);
    const int page = visibleRows > 0 ? visibleRows : 1;
    switch (k) {
        case Key::Up:       topRow -= 1; break;
        case Key::Down:     topRow += 1; break;
        case Key::PageUp:   topRow -= page; break;
        case Key::PageDown: topRow += page; break;
        case Key::Home:     topRow = 0; break;
        case Key::End:      topRow = maxTop; break;
    }
    return ClampTopRow(topRow, maxTop);
}
inline int OnWheel(int topRow, int wheelDelta, int itemCount, int visibleRows,
                   int rowsPerNotch) {
    const int maxTop = MaxTopRow(itemCount, visibleRows);
    const int notches = wheelDelta / 120;
    topRow -= notches * rowsPerNotch;
    return ClampTopRow(topRow, maxTop);
}
struct Range { int first; int count; };
inline Range VisibleRange(int topRow, int itemCount, int visibleRows) {
    const int maxTop = MaxTopRow(itemCount, visibleRows);
    topRow = ClampTopRow(topRow, maxTop);
    int remaining = itemCount - topRow;
    int count = remaining < visibleRows ? remaining : visibleRows;
    if (count < 0) count = 0;
    return Range{topRow, count};
}
struct Thumb { int y; int h; };
inline Thumb ThumbRect(int topRow, int itemCount, int visibleRows, int trackH,
                       int minThumbH) {
    if (itemCount <= visibleRows || itemCount <= 0) {
        return Thumb{0, trackH};
    }
    int h = static_cast<int>(
        static_cast<long long>(trackH) * visibleRows / itemCount);
    if (h < minThumbH) h = minThumbH;
    if (h > trackH) h = trackH;
    const int maxTop = MaxTopRow(itemCount, visibleRows);
    topRow = ClampTopRow(topRow, maxTop);
    int y = 0;
    if (maxTop > 0) {
        y = static_cast<int>(
            static_cast<long long>(trackH - h) * topRow / maxTop);
    }
    return Thumb{y, h};
}

// ---- Visible scrollbar geometry (fix thse isseu e.md #4) ----
// Byte-for-byte mirror of tests/list_scroll_logic.h: a reserved right-edge
// gutter for the scrollbar track so rows never underlap the thumb.
// fix thse isseue.md Phase B: a more substantial thumb (gutter 14 / pad 1 ->
// ~12px visible thumb @96dpi, spec 10-12px) + 8px breathing room between the
// rightmost size metadata and the gutter (kMetaScrollGap).
inline constexpr int kScrollbarW = 14;
inline constexpr int kScrollbarPad = 1;
inline constexpr int kMetaScrollGap = 8;
struct Box { int left; int top; int right; int bottom; };
inline Box TrackRect(int left, int top, int right, int bottom, int barW) {
    return Box{right - barW, top, right, bottom};
}
inline int ContentWidth(int viewportW, int barW, bool needsScrollbar) {
    return needsScrollbar ? (viewportW - barW) : viewportW;
}

// Input routing (#3 / #4 / #12): the list only consumes the wheel when the
// cursor is over it, and only consumes nav keys when no interactive control
// owns focus -- so buttons keep dialog nav and the edit keeps its caret.
inline bool WheelScrollsList(bool cursorOverList) {
    return cursorOverList;
}
inline bool KeysScrollList(bool focusOnEdit, bool focusOnButton,
                           bool focusOnToggle) {
    return !focusOnEdit && !focusOnButton && !focusOnToggle;
}

}  // namespace listscroll

// ---- Selected Items scrollbar AUTO-HIDE model (fix thse isseue.md Phase B) ---
// Byte-for-byte mirror of tests/scrollbar_autohide_logic.h. Pure. The scrollbar
// is a display-only indicator (wheel/keys scroll the list; no draggable thumb),
// so it auto-hides when idle and reveals while interacting / for a short window
// after the last scroll. The GUTTER stays reserved on overflow so showing or
// hiding the thumb never reflows the rows (no layout shift).
namespace sbauto {

inline constexpr unsigned kIdleHideMs = 1200;
inline constexpr int kHitSlopX = 6;

inline bool WithinRevealWindow(unsigned long long lastActivityTick,
                               unsigned long long now, unsigned idleMs) {
    if (now < lastActivityTick) return true;
    return (now - lastActivityTick) < idleMs;
}

inline bool Visible(bool needsBar, bool dragging, bool pointerInside,
                    bool withinReveal) {
    if (!needsBar) return false;
    if (dragging) return true;
    return pointerInside || withinReveal;
}

inline bool HitTestX(int px, int gutterLeft, int gutterRight, int hitSlop) {
    return px >= (gutterLeft - hitSlop) && px <= gutterRight;
}

}  // namespace sbauto

// ---- Footer INFORMATION / help button model (fix thse isseue.md Phase C) -----
// Byte-for-byte mirror of tests/info_help_logic.h. Pure. Single source of truth
// for the footer info button's accessible name, the SUMMARIZED single-line help
// content (top note: a summarized info button, 5-6 single lines), the hover +
// click-to-pin open/close state machine (sliding the pointer from the button
// into the popup must not dismiss it), and the footer-fit math so the button
// never pushes [Auto Group] [Group] [Cancel] off the popup. The live footer
// button + lightweight help popup are wired to THESE functions.
namespace infohelp {

inline const wchar_t* AccessibleName() { return L"Navigation help"; }

inline const wchar_t* Heading() { return L"Help"; }  // spec section 2: visible card title is "Help" (button accessible name stays AccessibleName()=="Navigation help")

// "Overflow and Scrollable Help Card": the card now renders the FULL navigation
// help (NAVIGATE ITEMS + New Folder + Group Creation + Auto Grouping) as single
// short lines; each line's Kind drives section/divider/body typography while the
// "Navigation help" heading stays fixed and the body scrolls on overflow. Keep
// in lockstep with tests/info_help_logic.h.
// Spec sections 3/5/6: FOUR sections whose headings are exactly the chip labels
// (Navigation / New Folder / Group Creation / Auto Group). Each line carries a
// Kind so the renderer applies the size hierarchy -- Section (13 px bold), Body
// (12 px), Bullet (indented 12 px), Note (11 px italic secondary) and Divider.
// Keyboard combinations are **`bold mono`** key tokens; action names are
// **bold**; inline examples are `mono`. Markers are NEVER drawn literally.
enum class Kind { Section, Body, Bullet, Note, Divider, Footer };
struct HelpLine { const wchar_t* text; Kind kind; };
inline const HelpLine* Lines(int* count) {
    static const HelpLine kLines[] = {
        { L"Navigation", Kind::Section },
        { L" ", Kind::Note},
        { L"**`Mouse wheel`** - Scroll Selected items", Kind::Bullet },
        { L"**`Shift + Wheel`** - Scroll Chips", Kind::Bullet },
        { L"**`Ctrl + Wheel`** - Switch between List, Large and Extra large icon views", Kind::Bullet },
        { L"**Copy | Move** - Choose, or use Arrows **←** **→** when focused", Kind::Bullet },
        { L"**`Tab`****,** **`Shift + Tab`** - Move between controls", Kind::Bullet },
        { L"**`Enter`** - Action Time", Kind::Bullet },
        { L"**`Esc`** - Cancel", Kind::Bullet },
        { L"Image previews appear only in Large and Extra large views", Kind::Note },
        { L"", Kind::Divider },
        { L"New Folder", Kind::Section },
        { L" ", Kind::Note},
        { L"**Shortcut**: **`Ctrl + F + N`**", Kind::Body },
        { L"**`Single folder`**: enter a name and confirm", Kind::Bullet },
        { L"**`Batch creation`**: when enabled, type a count after the shortcut (e.g. `**9**` for nine), up to the limit", Kind::Bullet },
        { L"**`Naming`**: inline naming is the default; popup naming is available in settings", Kind::Bullet },
        { L"**`Templates`**: press an assigned template key to create its folder set", Kind::Bullet },
        { L"", Kind::Divider },
        { L"Group Creation", Kind::Section },
        { L" ", Kind::Note},
        { L"**Shortcut**: **`Ctrl + G`**", Kind::Body },
        { L"**`Group`**: enter a destination name, choose Copy or Move, confirm", Kind::Bullet },
        { L"**`Copy`**: keep the originals", Kind::Bullet },
        { L"**`Move`**: relocate the selected items", Kind::Bullet },
        { L"**`Blank name`**: reuse the last used target name when that applies", Kind::Bullet },
        { L"**Optional quick actions**", Kind::Body },
        { L"**`Ctrl + G then C`** Copy", Kind::Bullet },
        { L"**`Ctrl + G then M`** Move to the last destination", Kind::Bullet },
        { L"Quick Copy | Move is setting-dependent", Kind::Note },
        { L"", Kind::Divider },
        { L"Auto Group", Kind::Section },
        { L" ", Kind::Note},
        { L"**`Auto Group`**: organize selected files into folders by extension", Kind::Body },
        { L"**`Safety delay`**: Wait **→** Think **→** Click Auto Group", Kind::Bullet },
        { L"**`Flat layout`**: Create extension folders directly in the destination", Kind::Bullet },
        { L"**`Parent`**: Group image-extension folders beneath the configured Images parent (beta)", Kind::Bullet },
        { L"**`Custom rules`**: Let configured rules place matching extensions into named buckets", Kind::Bullet },
        { L"The countdown never runs the operation; some features may be disabled", Kind::Note },
        { L"Visit `Ashix` _Ctrl + Left-click to open_", Kind::Footer },
    };
    if (count) *count = static_cast<int>(sizeof(kLines) / sizeof(kLines[0]));
    return kLines;
}
inline int LineCount() { int n = 0; Lines(&n); return n; }
inline const wchar_t* Line(int i) {
    int n = 0; const HelpLine* ls = Lines(&n);
    if (i < 0 || i >= n) return L"";
    return ls[i].text;
}
inline Kind LineKind(int i) {
    int n = 0; const HelpLine* ls = Lines(&n);
    if (i < 0 || i >= n) return Kind::Body;
    return ls[i].kind;
}

// fix thse isseue.md add-on section 4: the single Kind::Footer line's raw text
// (markers intact). The footer is painted in a FIXED band OUTSIDE the scrollable
// body; the renderer runs this through ParseRuns so "Visit" stays regular, the
// `Ashix` backtick span becomes the rounded key token and the _Ctrl + ..._
// underscore span is the smaller italic hint. No literal "GitHub" label.
inline const wchar_t* FooterText() {
    int n = 0; const HelpLine* ls = Lines(&n);
    for (int i = 0; i < n; ++i)
        if (ls[i].kind == Kind::Footer) return ls[i].text;
    return L"";
}

// fix thse isseue.md (nav-help typography): a run can be bold (**key**),
// MONOSPACE (`literal`, a backtick span -> a Windows mono face for keystrokes /
// literal tokens) or ITALIC (_hint_, an underscore span -> an italic face for
// secondary guidance). The three toggles are INDEPENDENT and the markers are
// NEVER rendered literally. Unmarked text is regular body.
struct HelpRun { std::wstring text; bool bold = false; bool mono = false; bool italic = false; };
inline std::vector<HelpRun> ParseRuns(const wchar_t* line) {
    std::vector<HelpRun> runs;
    std::wstring cur;
    bool bold = false, mono = false, italic = false;
    auto flush = [&]() {
        if (!cur.empty()) { runs.push_back(HelpRun{cur, bold, mono, italic}); cur.clear(); }
    };
    for (const wchar_t* p = line; *p; ) {
        if (p[0] == L'*' && p[1] == L'*') {
            flush();
            bold = !bold;
            p += 2;
        } else if (p[0] == L'`') {
            flush();
            mono = !mono;
            ++p;
        } else if (p[0] == L'_') {
            flush();
            italic = !italic;
            ++p;
        } else {
            cur.push_back(*p);
            ++p;
        }
    }
    flush();
    return runs;
}

// Issue 1: transient hover card, NO pin. Hover OR click opens; stays open while
// the pointer is on the button OR the card; leaving BOTH arms a short grace
// timer (flicker guard) that hides it; Esc / popup-close dismiss it.
enum class Vis { Hidden, Shown };

inline Vis OnHoverEnter(Vis) { return Vis::Shown; }
inline Vis OnClick(Vis) { return Vis::Shown; }
inline Vis OnDismiss(Vis) { return Vis::Hidden; }

inline bool StaysOpen(Vis v, bool pointerOnTrigger, bool pointerOnCard) {
    return v == Vis::Shown && (pointerOnTrigger || pointerOnCard);
}

inline bool ShouldArmClose(Vis v, bool pointerOnTrigger, bool pointerOnCard) {
    return v == Vis::Shown && !pointerOnTrigger && !pointerOnCard;
}

inline Vis OnCloseTimer(Vis v, bool pointerOnTrigger, bool pointerOnCard) {
    if (v == Vis::Shown && !pointerOnTrigger && !pointerOnCard) return Vis::Hidden;
    return v;
}

inline int CloseDelayMs() { return 180; }

inline bool FooterFitsWithInfo(int footerW, int infoW, int gap, int actionsW) {
    return (infoW + gap + actionsW) <= footerW;
}

inline int InfoButtonSize(int buttonH) { return buttonH; }

}  // namespace infohelp

// ---- Navigation Help card scroll + sizing model ("Overflow and Scrollable
// Help Card") ------------------------------------------------------------------
// Byte-for-byte mirror of tests/helpcard_scroll_logic.h. Pure. The help card is
// kept COMPACT: CapToWorkArea caps each dimension to the current monitor work
// area (DPI-aware -- callers pass device px) so it never enlarges past the
// available space as more content is added. When the measured content is taller
// than the content viewport, NeedsScroll turns on the internal scrollbar and
// ThumbRect gives its (theme-painted) thumb; the heading band is reserved
// OUTSIDE the viewport (ViewportHeight) so it stays fixed while the body
// scrolls. A wheel over the card scrolls ONLY the card and never the Selected
// Items list or the chips (WheelTargetsHelpCard). Keep in lockstep with the
// test header.
namespace helpscroll {

inline bool NeedsScroll(int contentH, int viewportH) {
    return viewportH > 0 && contentH > viewportH;
}

inline int MaxScroll(int contentH, int viewportH) {
    const int m = contentH - viewportH;
    return m > 0 ? m : 0;
}

inline int ClampScroll(int scrollY, int maxScroll) {
    if (scrollY < 0) return 0;
    if (scrollY > maxScroll) return maxScroll;
    return scrollY;
}

inline int OnWheel(int scrollY, int wheelDelta, int pxPerNotch, int maxScroll) {
    const int notches = wheelDelta / 120;
    scrollY -= notches * pxPerNotch;
    return ClampScroll(scrollY, maxScroll);
}

inline int CapToWorkArea(int desired, int workExtent, int marginPx) {
    int cap = workExtent - 2 * marginPx;
    if (cap < 1) cap = workExtent;
    return desired > cap ? cap : desired;
}

// Cap a desired card dimension to a FIXED maximum extent (DPI-scaled by the
// caller, so this is pure px). A zero/negative max means "no fixed cap". The
// help card applies this ALONGSIDE CapToWorkArea -- the SMALLER of the two
// always wins -- so the card has a consistent configured maximum size and never
// grows with its content past it; content beyond the resulting viewport scrolls
// INTERNALLY (NeedsScroll) instead of enlarging the card.
inline int CapToMax(int desired, int maxExtent) {
    if (maxExtent > 0 && desired > maxExtent) return maxExtent;
    return desired;
}

// Raise an (already max/work-capped) card dimension to a FIXED minimum so a
// card with little content still reads as a proper card. "Subject to available
// space": the caller passes a minExtent ALREADY clamped to the work area, so
// this never pushes the card off-screen. A zero/negative minimum means "no
// floor".
inline int CapToMin(int desired, int minExtent) {
    if (minExtent > 0 && desired < minExtent) return minExtent;
    return desired;
}

inline int ViewportHeight(int cardH, int headingBandH, int padBottom) {
    const int v = cardH - headingBandH - padBottom;
    return v > 0 ? v : 0;
}

struct Thumb { int y; int h; };
inline Thumb ThumbRect(int scrollY, int contentH, int viewportH, int trackH,
                       int minThumbH) {
    if (contentH <= viewportH || contentH <= 0 || trackH <= 0) {
        return Thumb{0, trackH};
    }
    const long long raw = static_cast<long long>(trackH) * viewportH / contentH;
    int h = static_cast<int>(raw);
    if (h < minThumbH) h = minThumbH;
    if (h > trackH)    h = trackH;
    const int maxScroll = contentH - viewportH;
    const int maxThumbY = trackH - h;
    int y = (maxScroll > 0)
        ? static_cast<int>(static_cast<long long>(scrollY) * maxThumbY / maxScroll)
        : 0;
    if (y < 0)         y = 0;
    if (y > maxThumbY) y = maxThumbY;
    return Thumb{y, h};
}

inline bool WheelTargetsHelpCard(bool cardShown, bool pointerOverCard) {
    return cardShown && pointerOverCard;
}

}  // namespace helpscroll

// ---- Navigation Help card section-navigation chips (fix thse isseue.md
// section 3 + add-on section 3) ------------------------------------------------
// Byte-for-byte mirror of tests/help_chips_logic.h. Pure. Four chips labelled
// exactly like the four section headings; a chip maps 1:1 to its infohelp
// Section line. Owns the strip layout math, hit-testing, horizontal strip
// scroll (clip + clamp + ensure-visible so the row never wraps or grows the
// card), active-section tracking from the body scroll, and the body scroll
// target when a chip is clicked. The chip strip is painted FIXED under the
// header while the body scrolls. Keep in lockstep with the test header.
namespace helpchips {

inline int Count() { return 4; }
inline const wchar_t* Label(int i) {
    switch (i) {
        case 0: return L"Navigation";
        case 1: return L"New Folder";
        case 2: return L"Group Creation";
        case 3: return L"Auto Group";
    }
    return L"";
}

struct Rect { int left; int right; };

inline std::vector<Rect> Layout(const std::vector<int>& widths, int gap,
                                int startX) {
    std::vector<Rect> out;
    int x = startX;
    for (size_t i = 0; i < widths.size(); ++i) {
        out.push_back(Rect{x, x + widths[i]});
        x += widths[i] + gap;
    }
    return out;
}

inline int TotalWidth(const std::vector<int>& widths, int gap) {
    int w = 0;
    for (size_t i = 0; i < widths.size(); ++i) {
        w += widths[i];
        if (i + 1 < widths.size()) w += gap;
    }
    return w;
}

inline int HitTest(const std::vector<Rect>& rects, int x) {
    for (size_t i = 0; i < rects.size(); ++i)
        if (x >= rects[i].left && x < rects[i].right) return static_cast<int>(i);
    return -1;
}

inline int MaxScrollX(int stripW, int visibleW) {
    const int m = stripW - visibleW;
    return m > 0 ? m : 0;
}

inline int ClampScrollX(int offset, int stripW, int visibleW) {
    const int m = MaxScrollX(stripW, visibleW);
    if (offset < 0) return 0;
    if (offset > m) return m;
    return offset;
}

inline int EnsureVisible(const std::vector<Rect>& rects, int i, int offset,
                         int visibleW, int stripW) {
    if (i < 0 || i >= static_cast<int>(rects.size()))
        return ClampScrollX(offset, stripW, visibleW);
    const int left  = rects[i].left;
    const int right = rects[i].right;
    if (left < offset)                  offset = left;
    else if (right > offset + visibleW) offset = right - visibleW;
    return ClampScrollX(offset, stripW, visibleW);
}

inline int ActiveSection(const std::vector<int>& anchors, int scrollY,
                         int viewportH) {
    if (anchors.empty()) return 0;
    const int line = scrollY + viewportH / 3;
    int active = 0;
    for (size_t i = 0; i < anchors.size(); ++i)
        if (anchors[i] <= line) active = static_cast<int>(i);
    return active;
}

inline int ScrollTargetFor(const std::vector<int>& anchors, int i,
                           int maxScrollY) {
    if (i < 0 || i >= static_cast<int>(anchors.size())) return 0;
    int t = anchors[i];
    if (t < 0) t = 0;
    if (t > maxScrollY) t = maxScrollY;
    return t;
}

inline int NextChip(int cur, int n) { return (n <= 0) ? 0 : (cur + 1) % n; }
inline int PrevChip(int cur, int n) { return (n <= 0) ? 0 : (cur - 1 + n) % n; }

}  // namespace helpchips

// ---- Navigation Help inline keyboard-token colours (fix thse isseue.md add-on
// section 6) ------------------------------------------------------------------
// Byte-for-byte mirror of tests/help_token_logic.h. Pure. The theme-aware token
// surface so a `mono` key token NEVER switches to a bright/white background in
// dark mode. Rgb() packs COLORREF (0x00BBGGRR), equal to RGB(r,g,b) on device.
namespace helptoken {

struct Colors {
    unsigned long bg;
    unsigned long text;
    unsigned long border;
    unsigned long hover;   // add-on section 2: restrained hover variant of the fill
};

inline unsigned long Rgb(int r, int g, int b) {
    return (unsigned long)(r & 0xFF)
         | ((unsigned long)(g & 0xFF) << 8)
         | ((unsigned long)(b & 0xFF) << 16);
}

// add-on section 2 FLIP: the key token is a HIGH-CONTRAST INVERSE chip. DARK
// theme -> MUTED MID-GREY (#D1CFC0, easy on the eyes) surface, DARK-GREY
// (#1F1F1F) text; LIGHT theme -> DEEP CHARCOAL (#171717) surface, WHITE
// (#FFFFFF) text. The border is a near-identical neutral (a hair off the fill)
// so the 0.5 px edge stays integrated and never reads as a doubled outline;
// `hover` is a restrained few-step variant of the fill used by the footer token.
inline Colors ForTheme(bool dark) {
    if (dark) {
        return Colors{ Rgb(209, 207, 192), Rgb(31, 31, 31),
                       Rgb(188, 186, 172), Rgb(196, 194, 180) };
    }
    return Colors{ Rgb(23, 23, 23), Rgb(255, 255, 255),
                   Rgb(44, 44, 44), Rgb(48, 48, 48) };
}

inline int R(unsigned long c)    { return (int)(c & 0xFF); }
inline int G(unsigned long c)    { return (int)((c >> 8) & 0xFF); }
inline int B(unsigned long c)    { return (int)((c >> 16) & 0xFF); }
inline int Luma(unsigned long c) { return (R(c) + G(c) + B(c)) / 3; }

}  // namespace helptoken

// ---- Navigation Help GitHub author footer (fix thse isseue.md section 6) -----
// Byte-for-byte mirror of tests/help_footer_logic.h. Pure. The author URL and
// the exact open gesture: the link opens ONLY on a Ctrl + left-click INSIDE the
// footer rect -- a plain click never navigates.
namespace helpfooter {

inline const wchar_t* Url() { return L"https://github.com/k-ashix"; }

inline bool InsideFooter(int px, int py, int left, int right, int top,
                         int bottom) {
    return px >= left && px < right && py >= top && py < bottom;
}

inline bool ShouldOpen(bool ctrlDown, bool leftClick, bool insideFooter) {
    return ctrlDown && leftClick && insideFooter;
}

// fix thse isseue.md add-on section 4: a plain DOUBLE left-click inside the
// footer ALSO opens the author link (no Ctrl needed on the double-click path).
inline bool ShouldOpenDouble(bool leftDoubleClick, bool insideFooter) {
    return leftDoubleClick && insideFooter;
}

}  // namespace helpfooter

// ---- Destination-section display model ----
// Byte-for-byte mirror of tests/destination_logic.h (UI spec item 5).
// Decides ONLY how the destination context is PRESENTED (fixed dest wins the
// field, else the effective context, else a placeholder; the recent/last
// target is a secondary line shown only when non-empty and not a duplicate).
// The resolution precedence (fixed > typed > last) stays in
// ResolveGroupDestination and is NOT changed here. Keep in lockstep with
// destination_logic.h.
namespace dest {

inline const wchar_t* SectionLabel() { return L"Destination"; }

inline std::wstring SectionDescription(bool isMove) {
    return isMove ? L"Selected items will be moved into the folder you name"
                  : L"Selected items will be copied into the folder you name";
}

inline std::wstring FieldPlaceholder() { return L"Enter folder name"; }

inline bool IsSep(wchar_t c) { return c == L'\\' || c == L'/'; }

inline std::wstring TrimWs(const std::wstring& s) {
    size_t b = 0, e = s.size();
    while (b < e && iswspace(s[b])) ++b;
    while (e > b && iswspace(s[e - 1])) --e;
    return s.substr(b, e - b);
}

inline std::wstring NormPath(const std::wstring& s) {
    std::wstring t = TrimWs(s);
    if (!t.empty() && IsSep(t[t.size() - 1])) t = t.substr(0, t.size() - 1);
    return t;
}

enum class DestSource { Fixed, Effective, None };

struct DestModel {
    DestSource source = DestSource::None;
    std::wstring fieldText;
    bool showLastTarget = false;
    std::wstring lastTargetText;
};

inline DestModel ComputeDestination(const std::wstring& fixedDest,
                                    const std::wstring& effectiveDest,
                                    const std::wstring& lastTarget) {
    DestModel m;
    const std::wstring fixed = TrimWs(fixedDest);
    const std::wstring eff = TrimWs(effectiveDest);
    if (!fixed.empty()) {
        m.source = DestSource::Fixed;
        m.fieldText = fixed;
    } else if (!eff.empty()) {
        m.source = DestSource::Effective;
        m.fieldText = eff;
    } else {
        m.source = DestSource::None;
        m.fieldText = FieldPlaceholder();
    }

    const std::wstring last = TrimWs(lastTarget);
    if (!last.empty() && NormPath(last) != NormPath(m.fieldText)) {
        m.showLastTarget = true;
        m.lastTargetText = L"Recent: " + last;
    }
    return m;
}

}  // namespace dest

// ---- Section wording (Phase 7/E) ----
// Byte-for-byte mirror of tests/section_labels_logic.h. Reworded per
// fix thse isseu e.md #6 ("Group into folder name" -> "Folder name" + helper).
namespace seclbl {

inline const wchar_t* NameLabel()       { return L"Folder name"; }
inline const wchar_t* NewFolderNameLabel() { return L"Enter your folder name"; }
inline const wchar_t* SelectedLabel() { return L"Selected items"; }

// ---- Primary (submit) button caption (fix thse isseue.md #4) ----------
// "Group" in the Ctrl+G group popup, "Create" in the New Folder popup. Both
// run the SAME IDOK command path; only the wording differs.
inline const wchar_t* SubmitLabel(bool isGroup) { return isGroup ? L"Group" : L"Create"; }

// ---- Action section (fix thse isseu e.md #6) ------------------------------
inline const wchar_t* ActionLabel()       { return L"Action for selected items"; }
inline const wchar_t* ActionDescription() { return L"Choose whether to move or copy the selected items."; }
inline const wchar_t* ActionKeyboardHint(){ return L" \u2190 \u2192"; }

}  // namespace seclbl

// Read-only Destination "information field" model (fix thse isseu e.md #2).
// Byte-for-byte mirror of tests/destfield_logic.h. Owns the field's internal
// geometry (folder icon cell + inset path-text cell) and the read-only policy
// (no keyboard focus, no caret, path ellipsis). No Browse behaviour.
namespace destfield {

struct Cell {
    int x = 0, y = 0, w = 0, h = 0;
    int right()  const { return x + w; }
    int bottom() const { return y + h; }
    bool empty() const { return w <= 0 || h <= 0; }
};

// fix thse isseue.md #2 (path pass): "no icon path". No folder-glyph column;
// the path text begins at the edge pad and the copy-path button lives OUTSIDE
// the field. kEdgePad is fed to EM_SETMARGINS on BOTH sides.
inline constexpr int kEdgePad  = 10;  // padding from the field's left/right edge
inline constexpr int kRadius   = 8;   // fix #6 (pass6): read-only field rounded-border corner radius

inline int TextLeftInset() { return kEdgePad; }

inline Cell TextCell(int fieldW, int fieldH) {
    const int x = TextLeftInset();
    const int w = fieldW - x - kEdgePad;
    return Cell{x, 0, w < 0 ? 0 : w, fieldH};
}

inline bool NoKeyboardFocus() { return true; }
inline bool ShowCaret()       { return false; }
inline bool UsePathEllipsis() { return true; }

// fix thse isseue.md #6: the field has a steady NEUTRAL border -- it is read-only
// INFO, so (unlike the editable Folder name field) it never shows a focus accent.
inline bool BorderIsAccent(bool /*focused*/) { return false; }

}  // namespace destfield

// Subclass proc for the read-only Destination info field (fix thse isseu e.md
// #2 / path pass): the field now shows the path PLAINLY -- NO folder glyph is
// drawn (fix thse isseue.md "no icon path"). Nothing is painted over the text,
// so there is no concrete artifact on the path. The subclass is retained as the
// field's native-EDIT hook; the left inset + read-only behaviour come from
// EM_SETMARGINS (destfield::TextLeftInset, now just the edge pad) + WS_DISABLED.
// Byte-for-byte mirror of tests/input_field_logic.h (fix thse isseue.md #4): the
// REUSABLE field-frame helper shared by all three prompt inputs (Group Folder
// name, Group Destination, New Folder name). The parent paints ONE rounded frame
// (fill + border); the native EDIT is an OPAQUE rectangle INSET inside that frame
// by kEditInset px on every side, so its square corners can never paint over the
// rounded border or its anti-aliased edge, and an opaque erase can never leave a
// ghost when text is deleted. One geometry, not two competing surfaces. No
// SetWindowRgn. Keep in lockstep with input_field_logic.h.
namespace inputfield {
inline constexpr int kRadius    = 8;   // outer frame corner radius (96-dpi px)
inline constexpr int kEditInset = 4;   // inner EDIT inset from the frame, all sides
enum class Field { Name, Destination, NewFolder };
struct FrameRect { int x = 0, y = 0, w = 0, h = 0;
    int right() const { return x + w; }
    int bottom() const { return y + h; } };
// fix thse isseue.md #2: the parent-painted rounded frame is EXACTLY the field's
// intended bounds -- NOT inflated outward. The old -1/+1 inflation only let the
// border spill past the field and read as a thick/doubled edge; the EDIT is
// already inset by kEditInset, so no inflation is needed. One surface, one
// border, no spill.
inline FrameRect OuterFrameRect(int fx, int fy, int fw, int fh) {
    return FrameRect{fx, fy, fw, fh};
}
inline int FrameSpillPx() { return 0; }
// The inner EDIT rect for an OUTER frame rect: inset by kEditInset on all sides
// (pure design space; the mod scales the result by DPI).
inline FrameRect InnerEditRect(int fx, int fy, int fw, int fh) {
    int iw = fw - 2 * kEditInset; int ih = fh - 2 * kEditInset;
    return FrameRect{fx + kEditInset, fy + kEditInset, iw < 0 ? 0 : iw, ih < 0 ? 0 : ih};
}
inline bool EditBackgroundIsOpaque() { return true; }  // opaque surface fill, no ghosting
inline bool ParentPaintsFrame()      { return true; }  // one rounded frame, painted by parent
inline bool UsesRegionClip()         { return false; } // never a hard-edged SetWindowRgn clip
inline bool BorderIsAccent(Field f, bool focused) {
    return (f == Field::Name || f == Field::NewFolder) && focused;
}
inline bool IsReadOnly(Field f) { return f == Field::Destination; }
inline int CornerDiameter() { return kRadius * 2; }
}  // namespace inputfield

// fix thse isseue.md #4: the EDIT no longer erases a rounded surface in its own
// DC. The field now uses ONE geometry: the parent paints a single rounded frame
// (fill + border) and the native EDIT is an OPAQUE rectangle INSET inside that
// frame (inputfield::InnerEditRect / kEditInset), so its square corners sit in
// the flat interior and never reach the rounded border. An opaque surface erase
// also means deleting text can never leave a ghost.

// True when the EDIT `hwnd` belongs to a Group popup (its parent's session is
// isGroup). Scopes the transparent/rounded field treatment to the Group popup;
// the native-caption New Folder popup is left exactly as before.
static bool EditParentIsGroup(HWND hwnd);  // fwd-decl: body deferred below struct NamePromptData (needs complete type)

static WNDPROC g_origDestFieldProc = nullptr;
// Read-only Destination field hook. fix thse isseue.md #4: the field no longer
// erases a rounded surface in its own DC -- the parent paints the single rounded
// frame and the EDIT is an OPAQUE rectangle INSET inside it (inputfield::
// InnerEditRect). The subclass is retained as the field's native-EDIT hook.
static LRESULT CALLBACK DestFieldSubclassProc(HWND hwnd, UINT msg,
                                              WPARAM wParam, LPARAM lParam) {
    return CallWindowProcW(g_origDestFieldProc, hwnd, msg, wParam, lParam);
}

// fix thse isseue.md #2 (path pass): copy the destination path to the clipboard
// (the "Copy" button, id 111). Standard CF_UNICODETEXT write via a moveable
// global; the clipboard takes ownership of the handle on success.
static void CopyPathToClipboard(HWND owner, const std::wstring& text) {
    if (text.empty()) return;
    if (!OpenClipboard(owner)) return;
    EmptyClipboard();
    const size_t bytes = (text.size() + 1) * sizeof(wchar_t);
    HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, bytes);
    if (hMem) {
        void* dst = GlobalLock(hMem);
        if (dst) {
            CopyMemory(dst, text.c_str(), bytes);
            GlobalUnlock(hMem);
            // fix thse isseue.md P2: on success the clipboard takes OWNERSHIP
            // of hMem; on failure ownership is NOT transferred, so free it here
            // instead of leaking the block.
            if (!SetClipboardData(CF_UNICODETEXT, hMem)) {
                GlobalFree(hMem);
            }
        } else {
            GlobalFree(hMem);
        }
    }
    CloseClipboard();
}

// ---- Folder name placeholder + focus-accent border (fix thse isseue.md #7) ----
// Byte-for-byte mirror of tests/namefield_logic.h.
namespace namefield {
inline constexpr int kRadius = 8;   // fix #7 (pass6): strongest input, 7-8px corner
inline const wchar_t* Placeholder() { return L"Enter folder name..."; }
inline bool BorderIsAccent(bool focused) { return focused; }
// fix thse isseue.md #5: placeholder cue shown ONLY when empty AND unfocused
// (pure predicate, mirrored in tests/namefield_logic.h and unit-tested).
inline bool ShowPlaceholder(bool isEmpty, bool focused) { return isEmpty && !focused; }
}  // namespace namefield

// Subclass the Folder name EDIT to paint the placeholder when it is empty AND
// unfocused. The edit is de-themed (for the dark fill), so EM_SETCUEBANNER is
// ignored -- we draw the cue ourselves in the secondary tone after the control's
// own paint. Mirrors the DestField subclass pattern. Group popup only.
// Forward declaration: the cached Segoe UI font helper is defined later (next
// to the theme palette), but the placeholder paint below is its FIRST use. In
// single-pass C++ that use must be preceded by a declaration, otherwise clang
// reports "undeclared identifier 'UiFont'" (and unhelpfully suggests IFont).
static HFONT UiFont(UINT dpi);
static WNDPROC g_origNameFieldProc = nullptr;
static int     g_nameFieldDpi      = 96;
static LRESULT CALLBACK NameFieldSubclassProc(HWND hwnd, UINT msg,
                                              WPARAM wParam, LPARAM lParam) {
    // fix thse isseue.md #5: the empty+unfocused placeholder cue is painted
    // INSIDE a single BeginPaint/EndPaint transaction -- NOT with a GetDC() after
    // the default WM_PAINT already ran (the old path that caused stale text /
    // flicker when content was deleted or focus changed). The EDIT stays the
    // SINGLE source of truth for its text: with text OR focus, the native EDIT
    // paints normally (caret, selection, insert/delete, clipboard, undo) -- we
    // never intercept character input and keep no shadow string. On a focus
    // change we invalidate ONLY this field so the cue toggles in step.
    switch (msg) {
    case WM_SETFOCUS:
    case WM_KILLFOCUS: {
        LRESULT r = CallWindowProcW(g_origNameFieldProc, hwnd, msg, wParam, lParam);
        if (GetWindowTextLengthW(hwnd) == 0)
            InvalidateRect(hwnd, nullptr, TRUE);
        return r;
    }
    case WM_PAINT:
        if (EditParentIsGroup(hwnd) &&
            namefield::ShowPlaceholder(GetWindowTextLengthW(hwnd) == 0,
                                       GetFocus() == hwnd)) {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);   // erases via the opaque editBg
                                               // WM_CTLCOLOREDIT brush, one pass
            RECT rc; GetClientRect(hwnd, &rc);
            const int dpi = g_nameFieldDpi;
            SetBkMode(hdc, TRANSPARENT);
            SetTextColor(hdc, IsDarkMode() ? RGB(165, 165, 165) : RGB(96, 96, 96));
            HGDIOBJ oldF = SelectObject(hdc, UiFont(static_cast<UINT>(dpi)));
            RECT tr = rc;
            tr.left += MulDiv(10, dpi, 96);   // match the EM_SETMARGINS left inset
            DrawTextW(hdc, namefield::Placeholder(), -1, &tr,
                      DT_LEFT | DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS);
            SelectObject(hdc, oldF);
            EndPaint(hwnd, &ps);
            return 0;
        }
        break;
    default:
        break;
    }
    return CallWindowProcW(g_origNameFieldProc, hwnd, msg, wParam, lParam);
}

// ---- Folder-name validation ----
// Byte-for-byte mirror of tests/name_validation_logic.h (the UI spec
// #6 / #14). Contextual, pre-Create validation that respects existing
// behavior: a blank New Folder name defaults to "New folder"; a blank Group
// name is valid when a fixed destination or remembered target still produces a
// folder. Does NOT change folder-creation semantics or the resolver. Keep in
// lockstep with name_validation_logic.h.
namespace namevalid {

enum class NameCheck {
    Ok = 0,
    EmptyNoDestination,
    IllegalCharacter,
    ReservedName,
    DotsOnly,
    TrailingDotOrSpace,
};

inline std::wstring TrimWs(const std::wstring& s) {
    size_t b = 0, e = s.size();
    while (b < e && iswspace(s[b])) ++b;
    while (e > b && iswspace(s[e - 1])) --e;
    return s.substr(b, e - b);
}

inline bool HasIllegalChar(const std::wstring& name) {
    for (wchar_t c : name) {
        if (c < 0x20) return true;
        if (c == L'<' || c == L'>' || c == L':' || c == L'"' || c == L'/' ||
            c == L'\\' || c == L'|' || c == L'?' || c == L'*') {
            return true;
        }
    }
    return false;
}

inline bool IsDotsOnly(const std::wstring& name) {
    if (name.empty()) return false;
    for (wchar_t c : name) if (c != L'.') return false;
    return true;
}

inline bool IsReservedName(const std::wstring& name) {
    std::wstring::size_type dot = name.find(L'.');
    std::wstring stem = (dot == std::wstring::npos) ? name : name.substr(0, dot);
    if (stem.empty()) return false;
    std::wstring up;
    up.reserve(stem.size());
    for (wchar_t c : stem) up.push_back(static_cast<wchar_t>(towupper(c)));
    if (up == L"CON" || up == L"PRN" || up == L"AUX" || up == L"NUL") return true;
    if (up.size() == 4) {
        std::wstring prefix = up.substr(0, 3);
        wchar_t d = up[3];
        if ((prefix == L"COM" || prefix == L"LPT") && d >= L'1' && d <= L'9') {
            return true;
        }
    }
    return false;
}

// Forbidden filename characters (mirror of IsForbiddenNameChar).
inline bool IsForbiddenNameChar(wchar_t c) {
    return c == L'<' || c == L'>' || c == L':' || c == L'"' || c == L'/' ||
           c == L'\\' || c == L'|' || c == L'?' || c == L'*';
}

// Namespace-local copy of the creation pipeline's SanitizeFolderBase(): strip
// C0 control chars and forbidden characters, then trim leading whitespace and
// trailing whitespace/dots. Validation reasons about the SANITIZED base so it
// never rejects a name the pipeline would have quietly repaired.
inline std::wstring SanitizeFolderBase(const std::wstring& raw) {
    std::wstring s;
    s.reserve(raw.size());
    for (wchar_t c : raw) {
        if (c < 0x20) continue;
        if (IsForbiddenNameChar(c)) continue;
        s.push_back(c);
    }
    size_t b = 0;
    while (b < s.size() && iswspace(s[b])) ++b;
    size_t e = s.size();
    while (e > b && (iswspace(s[e - 1]) || s[e - 1] == L'.')) --e;
    return s.substr(b, e - b);
}

// Validate a NON-EMPTY sanitized base: after sanitizing, only a reserved DOS
// device name is still un-creatable.
inline NameCheck ValidateName(const std::wstring& sanitizedBase) {
    if (IsReservedName(sanitizedBase)) return NameCheck::ReservedName;
    return NameCheck::Ok;
}

inline NameCheck ValidateNewFolderName(const std::wstring& typedName) {
    std::wstring base = SanitizeFolderBase(typedName);
    if (base.empty()) return NameCheck::Ok;
    return ValidateName(base);
}

inline NameCheck ValidateGroupName(const std::wstring& typedName,
                                   bool hasFixedDest, bool hasLastTarget) {
    if (hasFixedDest) return NameCheck::Ok;
    std::wstring base = SanitizeFolderBase(typedName);
    if (base.empty()) {
        return hasLastTarget ? NameCheck::Ok : NameCheck::EmptyNoDestination;
    }
    return ValidateName(base);
}

inline std::wstring ValidationMessage(NameCheck c) {
    switch (c) {
        case NameCheck::EmptyNoDestination:
            return L"Enter a folder name";
        case NameCheck::IllegalCharacter:
            return L"A folder name can't contain \\ / : * ? \" < > |";
        case NameCheck::ReservedName:
            return L"That name is reserved by Windows";
        case NameCheck::DotsOnly:
            return L"Enter a valid folder name";
        case NameCheck::TrailingDotOrSpace:
            return L"A folder name can't end with a space or period";
        case NameCheck::Ok:
        default:
            return std::wstring();
    }
}

}  // namespace namevalid

// ---- Copy/Move segmented-control state machine ----
// Byte-for-byte mirror of tests/segmented_logic.h (UI spec item 7 / #8).
// Decides how a keystroke/click maps to Copy/Move and that Left/Right act ONLY
// when the control owns focus (so the EDIT keeps caret behavior). d->isMove
// stays the single source of truth the worker reads back. Keep in lockstep
// with segmented_logic.h.
namespace segmented {

inline constexpr int kVkLeft  = 0x25;
inline constexpr int kVkRight = 0x27;

enum class Seg { Copy, Move };

inline Seg FromIsMove(bool isMove) { return isMove ? Seg::Move : Seg::Copy; }
inline bool ToIsMove(Seg s) { return s == Seg::Move; }

inline std::wstring SegmentLabel(Seg s) {
    return s == Seg::Move ? L"Move" : L"Copy";
}

// fix thse isseue.md #5 A: the access-key / mnemonic form of each label. The '&'
// prefix makes a prefix-aware DrawTextW UNDERLINE the next character, so Copy
// shows an underlined "C" and Move an underlined "M" -- the standard Windows
// access-key hint for the C/M keyboard mnemonics the control already handles
// (Mnemonic() below). The VISIBLE text equals SegmentLabel (the '&' is consumed,
// not drawn), so the paint keeps measuring the plain label to centre the group.
// Keep byte-identical to segmented_logic.h.
inline std::wstring MnemonicLabel(Seg s) {
    return s == Seg::Move ? L"&Move" : L"&Copy";
}
inline wchar_t MnemonicChar(Seg s) { return s == Seg::Move ? L'M' : L'C'; }

struct SegKeyResult {
    Seg seg;
    bool consumed;
};

inline SegKeyResult OnKey(Seg current, int vk, bool segFocused) {
    if (!segFocused) {
        return SegKeyResult{current, false};
    }
    if (vk == kVkLeft)  return SegKeyResult{Seg::Copy, true};
    if (vk == kVkRight) return SegKeyResult{Seg::Move, true};
    return SegKeyResult{current, false};
}

// C/M mnemonic (fix #4/#6): 'C'/'c' -> Copy, 'M'/'m' -> Move, ONLY when a
// segment owns focus so the EDIT keeps C/M as typed characters (no global
// hotkey while typing). Byte-for-byte mirror of segmented_logic.h.
inline SegKeyResult Mnemonic(Seg current, int ch, bool segFocused) {
    if (!segFocused) {
        return SegKeyResult{current, false};
    }
    if (ch == 'C' || ch == 'c') return SegKeyResult{Seg::Copy, true};
    if (ch == 'M' || ch == 'm') return SegKeyResult{Seg::Move, true};
    return SegKeyResult{current, false};
}

inline Seg OnClickSegment(Seg clicked) { return clicked; }

// One-pill corner rounding (fix #7): only the OUTER corners are rounded, the
// shared middle boundary is straight. Byte-for-byte mirror of segmented_logic.h.
struct SegCorners {
    bool topLeft, topRight, bottomRight, bottomLeft;
};
inline SegCorners CornersFor(Seg s) {
    if (s == Seg::Copy) return SegCorners{true,  false, false, true};
    return                       SegCorners{false, true,  true,  false};
}
inline bool SegmentsAreAdjacent(int leftRight, int rightLeft) {
    return leftRight == rightLeft;
}

}  // namespace segmented

// fix thse isseue.md #1: the status:: state machine was removed together with the Status/Ready feature (no status card, no live round-trip).

// ---- Create/Cancel button feedback state machine ----
// Byte-for-byte mirror of tests/button_state_logic.h (the UI spec
// #10/#11/#12). Mouse Create and Enter share one action path (as do Cancel and
// Esc); Create is accent/primary and never flashes green before a real
// success; Cancel is neutral and never permanently red; a duplicate Create
// while one is in flight is ignored. Keep in lockstep with
// button_state_logic.h.
namespace btn {

enum class Phase { Idle, Processing, Success, Warning, Failure, Cancelled };
enum class Tone { Accent, Neutral, Green, Amber, Red };
enum class Input { MouseCreate, KeyEnter, MouseCancel, KeyEsc };
enum class Action { Create, Cancel, None };

inline Action ActionFor(Input in) {
    switch (in) {
        case Input::MouseCreate:
        case Input::KeyEnter:   return Action::Create;
        case Input::MouseCancel:
        case Input::KeyEsc:     return Action::Cancel;
        default:                return Action::None;
    }
}

inline Tone CreateTone(Phase p) {
    switch (p) {
        case Phase::Success: return Tone::Green;
        case Phase::Warning: return Tone::Amber;
        case Phase::Failure: return Tone::Red;
        case Phase::Idle:
        case Phase::Processing:
        case Phase::Cancelled:
        default:             return Tone::Accent;
    }
}

inline Tone CancelTone(Phase p) {
    return p == Phase::Cancelled ? Tone::Red : Tone::Neutral;
}

inline bool CreateEnabled(Phase p) { return p != Phase::Processing; }

inline bool AcceptCreate(Phase current) { return current == Phase::Idle; }
inline Phase OnCreate(Phase current) {
    return current == Phase::Idle ? Phase::Processing : current;
}

inline Phase OnCancel(Phase /*current*/) { return Phase::Cancelled; }

inline bool IsFailure(Phase p) { return p == Phase::Failure; }
inline bool IsCancelled(Phase p) { return p == Phase::Cancelled; }

// ---- Press feedback (fix thse isseue.md #2/#5): confirm GREEN while the
// Create / Copy buttons are pressed, cancel RED for Cancel / Close. Transient
// press cue only; independent of the result-phase tones above. ----
enum class Button { Create, Copy, Cancel, Close, Other };
inline Tone PressTone(Button b) {
    switch (b) {
        case Button::Create:
        case Button::Copy:   return Tone::Green;
        case Button::Cancel:
        case Button::Close:  return Tone::Red;
        case Button::Other:
        default:             return Tone::Neutral;
    }
}

}  // namespace btn

// ---- Compact / expanded mode model ----
// Byte-for-byte mirror of tests/mode_logic.h (UI spec item 15). Remembers
// the mode for the session, picks how many Selected Items rows to show, and
// feeds `expanded` into layout::ComputePromptLayout. A large selection
// auto-opens expanded. Keep in lockstep with mode_logic.h.
namespace mode {

enum class Mode { Compact, Expanded };

inline Mode Toggle(Mode m) {
    return m == Mode::Compact ? Mode::Expanded : Mode::Compact;
}

inline bool IsExpanded(Mode m) { return m == Mode::Expanded; }

inline int CompactRows()  { return 4; }
inline int ExpandedRows() { return 9; }

inline int VisibleRows(Mode m) {
    return m == Mode::Expanded ? ExpandedRows() : CompactRows();
}

// fix thse isseue.md #1: rows drawn for `itemCount` in mode `m` -- the per-mode
// cap, but never more than there are items (content-sized list).
inline int VisibleRowsFor(size_t itemCount, Mode m) {
    const int cap = VisibleRows(m);
    return static_cast<int>(itemCount) < cap ? static_cast<int>(itemCount) : cap;
}

inline Mode AutoMode(size_t itemCount) {
    return itemCount > static_cast<size_t>(CompactRows())
               ? Mode::Expanded
               : Mode::Compact;
}

// Initial mode for a fresh popup: a remembered session choice wins, else
// auto-expand a large selection (#11 / #15).
inline Mode InitialMode(bool hasSessionMode, Mode sessionMode,
                        size_t itemCount) {
    return hasSessionMode ? sessionMode : AutoMode(itemCount);
}

}  // namespace mode

// Compact/Expanded size remembered for the current Windhawk tool-process
// session (#11). g_sessionHasMode stays false until the user first toggles, so
// the first popup still uses the auto-expand default.
static bool g_sessionHasMode = false;
static bool g_sessionExpanded = false;

// ---- Rounded-corner decisions ----
// Byte-for-byte mirror of tests/corner_logic.h (UI spec item 17). Chooses
// the native DWM corner preference on Win11 (with a region fallback), the
// DWMWCP_ROUND value, and the DPI-scaled radius. Keep in lockstep with
// corner_logic.h.
namespace corner {

enum class Method { DwmRound, RegionFallback };

inline Method ChooseMethod(bool isWin11) {
    return isWin11 ? Method::DwmRound : Method::RegionFallback;
}

inline bool UsesDwm(Method m) { return m == Method::DwmRound; }
inline bool NeedsRegionClip(Method m) { return m == Method::RegionFallback; }

inline int DwmCornerPreference() { return 2; }  // DWMWCP_ROUND

inline int CornerRadius96() { return 8; }

inline int ScaledRadius(int radius96, int dpi) {
    return static_cast<int>(
        (static_cast<long long>(radius96) * dpi + 48) / 96);
}

}  // namespace corner

// ---- Focus ring + tab order ----
// Byte-for-byte mirror of tests/focus_logic.h (UI spec item 13). The mod
// draws the ring on the owner-drawn controls and walks the same logical order
// the dialog manager uses. Keep in lockstep with focus_logic.h.
namespace focus {

enum class Ctl { Name, OpToggle, Create, Cancel };

inline Ctl First() { return Ctl::Name; }

inline Ctl Next(Ctl c, bool isGroup) {
    switch (c) {
        case Ctl::Name:     return isGroup ? Ctl::OpToggle : Ctl::Create;
        case Ctl::OpToggle: return Ctl::Create;
        case Ctl::Create:   return Ctl::Cancel;
        case Ctl::Cancel:   return Ctl::Name;
    }
    return Ctl::Name;
}

inline Ctl Prev(Ctl c, bool isGroup) {
    switch (c) {
        case Ctl::Name:     return Ctl::Cancel;
        case Ctl::OpToggle: return Ctl::Name;
        case Ctl::Create:   return isGroup ? Ctl::OpToggle : Ctl::Name;
        case Ctl::Cancel:   return Ctl::Create;
    }
    return Ctl::Name;
}

inline bool ShowsFocusRing(Ctl) { return true; }

}  // namespace focus

// Byte-for-byte mirror of tests/focus_visible_logic.h (fix thse isseue.md P0
// #1). The owner-drawn focus ring is gated by focusvis:: so a MOUSE click never
// paints an outline, while KEYBOARD navigation keeps the ring. Keep in lockstep
// with focus_visible_logic.h.
namespace focusvis {

inline constexpr int kVkTab    = 0x09;
inline constexpr int kVkReturn = 0x0D;
inline constexpr int kVkSpace  = 0x20;
inline constexpr int kVkPrior  = 0x21;  // PageUp
inline constexpr int kVkNext   = 0x22;  // PageDown
inline constexpr int kVkEnd    = 0x23;
inline constexpr int kVkHome   = 0x24;
inline constexpr int kVkLeft   = 0x25;
inline constexpr int kVkUp     = 0x26;
inline constexpr int kVkRight  = 0x27;
inline constexpr int kVkDown   = 0x28;

enum class Input { KeyboardNav, PointerPress };

inline bool FromInput(Input in) {
    return in == Input::KeyboardNav;
}

inline bool ShouldShowRing(bool hasFocus, bool focusVisible) {
    return hasFocus && focusVisible;
}

inline bool IsNavKey(int vk) {
    return vk == kVkTab    || vk == kVkReturn || vk == kVkSpace ||
           vk == kVkPrior  || vk == kVkNext   || vk == kVkEnd   ||
           vk == kVkHome   || vk == kVkLeft   || vk == kVkUp    ||
           vk == kVkRight  || vk == kVkDown;
}

}  // namespace focusvis

// ---- WM_DPICHANGED decoding ----
// Byte-for-byte mirror of tests/dpi_logic.h (UI spec item 16). Named
// `dpiutil` so it never collides with the popup's local `dpi`. Keep in lockstep
// with dpi_logic.h.
namespace dpiutil {

inline int NewDpiFromWparam(unsigned long wParam) {
    return static_cast<int>(wParam & 0xFFFFu);
}

inline bool NeedsRelayout(int oldDpi, int newDpi) {
    return newDpi > 0 && newDpi != oldDpi;
}

}  // namespace dpiutil

// Clean Segoe UI font (replaces the pixelated default System font), sized for
// the popup's monitor DPI. Cached per DPI so a move to a different-DPI monitor
// rebuilds it rather than reusing a mis-sized glyph cache.
static HFONT g_uiFont = nullptr;
static UINT g_uiFontDpi = 0;
static HFONT UiFont(UINT dpi) {
    if (g_uiFont && g_uiFontDpi == dpi) {
        return g_uiFont;
    }
    if (g_uiFont) {
        DeleteObject(g_uiFont);
        g_uiFont = nullptr;
    }
    g_uiFont = CreateFontW(ScaleDpi(-17, dpi), 0, 0, 0, FW_NORMAL, FALSE, FALSE,
                           FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
                           CLIP_DEFAULT_PRECIS, CLEARTYPE_NATURAL_QUALITY,
                           DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");
    g_uiFontDpi = dpi;
    return g_uiFont;
}

// S14 (fix thse isseue.md v0.12.54): the section TITLES ("Selected items",
// "Destination", "Folder name", "Status", "Action") + the "Group into folder"
// title-bar heading read HEAVIER than the body text in the target Win11 dialog.
// UiFontSemibold is a FW_SEMIBOLD companion to UiFont: identical Segoe UI face,
// point size and per-DPI caching -- only the weight differs. Body + helper +
// state text stays on regular UiFont. The weight policy is the unit-tested
// namespace typeweight (tests/type_weight_logic.h), mirrored here and tied to
// the real Win32 FW_* constants by the static_asserts.
namespace typeweight {
inline constexpr int kRegular  = 400;  // FW_NORMAL
inline constexpr int kSemibold = 600;  // FW_SEMIBOLD
}  // namespace typeweight
static_assert(FW_NORMAL == typeweight::kRegular,
              "UiFont regular weight must match the tested typeweight policy");
static_assert(FW_SEMIBOLD == typeweight::kSemibold,
              "UiFontSemibold weight must match the tested typeweight policy");

static HFONT g_uiFontSemibold = nullptr;
static UINT  g_uiFontSemiboldDpi = 0;
static HFONT UiFontSemibold(UINT dpi) {
    if (g_uiFontSemibold && g_uiFontSemiboldDpi == dpi) {
        return g_uiFontSemibold;
    }
    if (g_uiFontSemibold) {
        DeleteObject(g_uiFontSemibold);
        g_uiFontSemibold = nullptr;
    }
    g_uiFontSemibold = CreateFontW(ScaleDpi(-17, dpi), 0, 0, 0, FW_SEMIBOLD, FALSE,
                                   FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
                                   CLIP_DEFAULT_PRECIS, CLEARTYPE_NATURAL_QUALITY,
                                   DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");
    g_uiFontSemiboldDpi = dpi;
    return g_uiFontSemibold;
}

// fix thse isseue.md #1: the Destination PATH is drawn in a NATIVE monospace
// face (Consolas -- bundled with Windows, no external font added) so a path is
// easy to scan. DPI-scaled + natural ClearType like the other UI fonts; cached
// per-DPI and freed on unload.
static HFONT g_uiFontMono = nullptr;
static UINT  g_uiFontMonoDpi = 0;
static HFONT g_uiFontMonoBold = nullptr;   // add-on section 2: bold monospaced face for key tokens
static UINT  g_uiFontMonoBoldDpi = 0;
static HFONT UiFontMono(UINT dpi) {
    if (g_uiFontMono && g_uiFontMonoDpi == dpi) {
        return g_uiFontMono;
    }
    if (g_uiFontMono) {
        DeleteObject(g_uiFontMono);
        g_uiFontMono = nullptr;
    }
    g_uiFontMono = CreateFontW(ScaleDpi(-16, dpi), 0, 0, 0, FW_NORMAL, FALSE,
                               FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
                               CLIP_DEFAULT_PRECIS, CLEARTYPE_NATURAL_QUALITY,
                               FIXED_PITCH | FF_MODERN, L"Consolas");
    g_uiFontMonoDpi = dpi;
    return g_uiFontMono;
}

// add-on section 2: a BOLD monospaced face (Consolas, FW_SEMIBOLD) for the
// inline keyboard tokens so the key text reads bold inside its chip surface.
// Same -16 px size + FIXED_PITCH as UiFontMono so token widths stay aligned.
static HFONT UiFontMonoBold(UINT dpi) {
    if (g_uiFontMonoBold && g_uiFontMonoBoldDpi == dpi) {
        return g_uiFontMonoBold;
    }
    if (g_uiFontMonoBold) {
        DeleteObject(g_uiFontMonoBold);
        g_uiFontMonoBold = nullptr;
    }
    g_uiFontMonoBold = CreateFontW(ScaleDpi(-16, dpi), 0, 0, 0, FW_SEMIBOLD, FALSE,
                                   FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
                                   CLIP_DEFAULT_PRECIS, CLEARTYPE_NATURAL_QUALITY,
                                   FIXED_PITCH | FF_MODERN, L"Consolas");
    g_uiFontMonoBoldDpi = dpi;
    return g_uiFontMonoBold;
}

// fix thse isseue.md (nav-help typography): an ITALIC companion to UiFont --
// the same Windows-default Segoe UI face, regular weight, italic slant, same
// point size + per-DPI cache. Used for the help card's _hint_ spans so
// secondary guidance reads as softer, distinct text.
static HFONT g_uiFontItalic = nullptr;
static UINT  g_uiFontItalicDpi = 0;
static HFONT UiFontItalic(UINT dpi) {
    if (g_uiFontItalic && g_uiFontItalicDpi == dpi) {
        return g_uiFontItalic;
    }
    if (g_uiFontItalic) {
        DeleteObject(g_uiFontItalic);
        g_uiFontItalic = nullptr;
    }
    g_uiFontItalic = CreateFontW(ScaleDpi(-17, dpi), 0, 0, 0, FW_NORMAL, TRUE,
                                 FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
                                 CLIP_DEFAULT_PRECIS, CLEARTYPE_NATURAL_QUALITY,
                                 DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");
    g_uiFontItalicDpi = dpi;
    return g_uiFontItalic;
}

// Spec section 6 typography: dedicated faces so the help card renders the full
// size hierarchy instead of reusing one body face -- a ~13 px FW_BOLD (700)
// face for SECTION headings and a ~11 px italic face for SECONDARY NOTES. Same
// lazily-cached, DPI-refreshed pattern as the other UiFont* helpers (recreated
// whenever the DPI changes, so WM_DPICHANGED stays correct).
static HFONT g_uiFontSection = nullptr;
static UINT  g_uiFontSectionDpi = 0;
static HFONT UiFontSection(UINT dpi) {
    if (g_uiFontSection && g_uiFontSectionDpi == dpi) {
        return g_uiFontSection;
    }
    if (g_uiFontSection) {
        DeleteObject(g_uiFontSection);
        g_uiFontSection = nullptr;
    }
    g_uiFontSection = CreateFontW(ScaleDpi(-19, dpi), 0, 0, 0, FW_BOLD, FALSE,
                                  FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
                                  CLIP_DEFAULT_PRECIS, CLEARTYPE_NATURAL_QUALITY,
                                  DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");
    g_uiFontSectionDpi = dpi;
    return g_uiFontSection;
}

static HFONT g_uiFontNote = nullptr;
static UINT  g_uiFontNoteDpi = 0;
static HFONT UiFontNote(UINT dpi) {
    if (g_uiFontNote && g_uiFontNoteDpi == dpi) {
        return g_uiFontNote;
    }
    if (g_uiFontNote) {
        DeleteObject(g_uiFontNote);
        g_uiFontNote = nullptr;
    }
    g_uiFontNote = CreateFontW(ScaleDpi(-15, dpi), 0, 0, 0, FW_NORMAL, TRUE,
                               FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
                               CLIP_DEFAULT_PRECIS, CLEARTYPE_NATURAL_QUALITY,
                               DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");
    g_uiFontNoteDpi = dpi;
    return g_uiFontNote;
}

// ---- Popup theme palette ----
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
    COLORREF cardBg;            // section-card surface (elevated on the panel)
    COLORREF cardBorder;        // section-card hairline border
    COLORREF badgeFill;         // EXT metadata pill fill (Selected Items rows)
    COLORREF badgeBorder;       // EXT metadata pill hairline border
    // fix thse isseue.md top note: the Auto Group button reuses the shared
    // create (Group) / cancel (neutral) tokens -- it has NO dedicated green.
};

// ---- Design colour tokens ----
// Named colour tokens so NO popup colour is a bare hard-coded RGB() at the
// point of use. ResolveThemePalette() maps each semantic slot to one of these
// tokens.
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
// (fix thse isseue.md section 3) The "danger red" palette (kColDangerLight and
// its pressed/border + dark variants) was reserved for a cancelled/failure
// STATUS band that no longer exists. All six constants were [[maybe_unused]]
// and are removed; reintroduce them with the band if a status UI is ever added.
// Neutral secondary button (Cancel + Copy/Move segments) -- a gray surface that
// sits near the panel, so the primary accent Create is the only strong colour.
static const COLORREF kColNeutralLight        = RGB(251, 251, 251); // #FBFBFB
static const COLORREF kColNeutralLightPressed = RGB(240, 240, 240);
static const COLORREF kColNeutralLightBorder  = RGB(209, 209, 209); // #D1D1D1
static const COLORREF kColNeutralDark         = RGB(45, 45, 45);    // #2D2D2D
static const COLORREF kColNeutralDarkPressed  = RGB(39, 39, 39);
static const COLORREF kColNeutralDarkBorder   = RGB(74, 74, 74);    // #4A4A4A
// fix thse isseue.md #2/#5: confirm GREEN press feedback for the Create + Copy
// buttons (btn::PressTone == Green), the green counterpart to the Cancel/Close
// red press feedback. One green source of truth; a white caption reads on it.
static const COLORREF kColConfirmPressed      = RGB(16, 124, 16);   // #107C10 success green
static const COLORREF kColConfirm             = RGB(22, 150, 34);   // fix thse isseue.md Issue 1: idle success green for the '?' navigation-help button (pressed deepens to kColConfirmPressed)
// fix thse isseue.md top note ("green color is not good for auto group - keep
// it same as group"): the Auto Group button no longer has a dedicated green
// palette. Once enabled it reuses the Group button's create tokens; while the
// safety countdown runs it reuses the neutral Cancel tokens.
// Section card surface (Phase 4/B, fix thse isseu e.md #2): an elevated surface
// that sits ON the panel so each section reads as its OWN card, plus a subtle
// hairline border. Light card is near-white on the #F3F3F3 panel; dark card is
// slightly lighter than the #202020 panel.
static const COLORREF kColCardLight       = RGB(255, 255, 255);
static const COLORREF kColCardLightBorder = RGB(229, 229, 229); // #E5E5E5
static const COLORREF kColCardDark        = RGB(43, 43, 43);    // #2B2B2B
static const COLORREF kColCardDarkBorder  = RGB(58, 58, 58);    // #3A3A3A
// EXT badge pill (fix thse isseue.md #5 polish): a DEDICATED subtle neutral
// chip so the extension pill reads as quiet metadata. It used to borrow the
// edit-box groove (editBg), which in dark mode is #545454 -- a heavy slab that
// fought the row text. Light = a hair off the white card; dark = a hair above
// the #2B2B2B card, both with a matching one-tone-darker hairline border.
static const COLORREF kColBadgeLight       = RGB(241, 241, 241); // #F1F1F1
static const COLORREF kColBadgeLightBorder = RGB(224, 224, 224); // #E0E0E0
static const COLORREF kColBadgeDark        = RGB(55, 55, 55);    // #373737
static const COLORREF kColBadgeDarkBorder  = RGB(74, 74, 74);    // #4A4A4A

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
        p.cancelFill = kColNeutralDark;
        p.cancelFillPressed = kColNeutralDarkPressed;
        p.cancelText = kColTextDark;
        p.cancelBorder = kColNeutralDarkBorder;
        p.cardBg = kColCardDark;
        p.cardBorder = kColCardDarkBorder;
        p.badgeFill = kColBadgeDark;
        p.badgeBorder = kColBadgeDarkBorder;
    } else {
        p.panelBg = kColSurfaceLight;
        p.panelText = kColTextLight;
        p.editBg = kColInputLight;
        p.editText = kColInputTextLight;
        p.cancelFill = kColNeutralLight;
        p.cancelFillPressed = kColNeutralLightPressed;
        p.cancelText = kColTextLight;
        p.cancelBorder = kColNeutralLightBorder;
        p.cardBg = kColCardLight;
        p.cardBorder = kColCardLightBorder;
        p.badgeFill = kColBadgeLight;
        p.badgeBorder = kColBadgeLightBorder;
    }
    return p;
}

// fix thse isseue.md #2 (v0.12.60): ONE shared, theme-aware caption foreground
// for BOTH the "Group into folder" title text AND the three caption glyphs
// (Minimize / Expand-Restore / Close). Near-black in light, near-white in dark
// -- never a hard-coded black in dark mode. The caption buttons draw NO opaque
// backplate, so the glyph/text colour alone carries the control; it must stay
// legible over the composed title-bar material and the solid-header fallback.
// Mirrored byte-for-byte in tests/caption_fg_logic.h.
static const COLORREF kColCaptionFgLight = RGB(24, 24, 24);    // near-black
static const COLORREF kColCaptionFgDark  = RGB(245, 245, 245); // near-white
static COLORREF CaptionForeground(bool dark) {
    return dark ? kColCaptionFgDark : kColCaptionFgLight;
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

// fix thse isseue.md #2: the former ThemeFieldSurface / ThemeFieldCorner helpers
// (editBg / cardBg accessors for the EDIT's own rounded-surface erase) are gone.
// The EDIT no longer self-erases a rounded surface -- the parent paints the one
// rounded field perimeter and callers read pal.editBg / pal.cardBg directly --
// so the wrappers were dead code that tripped clang -Wunused-function. Removed.

// Rounded corners + dark title bar + acrylic backdrop (all best-effort; any
// attribute the OS does not support is silently ignored).
// Byte-for-byte mirror of tests/dwm_border_logic.h (fix thse isseue.md P0 #5):
// which DWM attribute carries the border colour (DWMWA_BORDER_COLOR == 34) and
// that the custom Group popup's border matches its OWN panel surface. Keep in
// lockstep with dwm_border_logic.h.
namespace dwmborder {
inline constexpr int kAttrBorderColor = 34;
// DWMWA_COLOR_NONE: tells DWM to draw NO border (transparent perimeter).
inline constexpr unsigned long kColorNone = 0xFFFFFFFEUL;
enum class Source { None };
inline Source BorderSource() { return Source::None; }
inline bool IsTransparent() { return BorderSource() == Source::None; }
inline bool MatchesSurface() { return false; }
inline bool AppliesTo(bool isGroup) { return isGroup; }
// fix thse isseue.md P0 #2: DWMWA_BORDER_COLOR is best-effort (older Win10
// rejects it); CHECK the HRESULT, never assume DWMWA_COLOR_NONE took effect.
// Byte-for-byte mirror of dwmborder::BorderColorAccepted in dwm_border_logic.h.
inline bool BorderColorAccepted(unsigned long hr) {
    return (hr & 0x80000000UL) == 0UL;
}
}  // namespace dwmborder

// Byte-for-byte mirror of tests/outer_outline_logic.h (fix thse isseue.md P0
// #2): the ONE shared extended-style decision. The custom borderless Group
// popup drops WS_EX_DLGMODALFRAME (the raised 3-D modal frame that painted the
// stray bright outer perimeter the issue reports); the native-caption New
// Folder popup keeps it. PromptOuterSize() and CreateWindowExW() both call
// PromptExStyle() with the same isGroup, so the frame the size math assumes can
// never disagree with the frame the window actually gets. Keep in lockstep with
// outer_outline_logic.h.
namespace outeroutline {
inline constexpr unsigned long kExDlgModalFrame = 0x00000001UL; // WS_EX_DLGMODALFRAME
inline constexpr unsigned long kExTopmost       = 0x00000008UL; // WS_EX_TOPMOST
inline unsigned long PromptExStyle(bool isGroup) {
    unsigned long ex = kExTopmost;
    if (!isGroup) {
        ex |= kExDlgModalFrame;
    }
    return ex;
}
inline bool HasDlgModalFrame(unsigned long exStyle) {
    return (exStyle & kExDlgModalFrame) != 0UL;
}
inline bool DrawsOuterModalFrame(bool isGroup) {
    return HasDlgModalFrame(PromptExStyle(isGroup));
}
}  // namespace outeroutline
// Tie the mirrored ex-style bits to the real winuser.h constants so the mirror
// in tests/outer_outline_logic.h can never silently drift from what the window
// is actually created with.
static_assert(outeroutline::kExDlgModalFrame == WS_EX_DLGMODALFRAME,
              "WS_EX_DLGMODALFRAME drift");
static_assert(outeroutline::kExTopmost == WS_EX_TOPMOST, "WS_EX_TOPMOST drift");

// Byte-for-byte mirror of tests/backdrop_logic.h (fix thse isseue.md #7): when
// may the popup drop its solid themed fill and let the Win11 acrylic backdrop
// show through (a transparent, premium, minimal window instead of a flat opaque
// "boxed" panel)? Only on Win11 + backdrop requested + Transparency effects ON;
// otherwise keep the solid panel (safe fallback). Keep in lockstep.
namespace backdrop {
inline constexpr int kBackdropAcrylic = 3;   // DWMSBT_TRANSIENTWINDOW
enum class Fill { SolidPanel, SystemBackdrop };
inline Fill ChooseFill(bool isWin11, bool backdropRequested, bool transparencyOn) {
    const bool show = isWin11 && backdropRequested && transparencyOn;
    return show ? Fill::SystemBackdrop : Fill::SolidPanel;
}
inline bool SkipSolidFill(bool isWin11, bool backdropRequested, bool transparencyOn) {
    return ChooseFill(isWin11, backdropRequested, transparencyOn) == Fill::SystemBackdrop;
}
// fix thse isseue.md #1: the HONEST, HRESULT-checked decision (mirror of
// tests/backdrop_logic.h). Skip the solid fill ONLY when the Win11 policy holds
// AND the real DwmSetWindowAttribute backdrop request SUCCEEDED (backdropApplied).
inline Fill ChooseFillChecked(bool isWin11, bool backdropRequested,
                              bool transparencyOn, bool backdropApplied) {
    const bool policyWantsBackdrop =
        ChooseFill(isWin11, backdropRequested, transparencyOn) == Fill::SystemBackdrop;
    return (policyWantsBackdrop && backdropApplied) ? Fill::SystemBackdrop
                                                    : Fill::SolidPanel;
}
inline bool SkipSolidFillChecked(bool isWin11, bool backdropRequested,
                                 bool transparencyOn, bool backdropApplied) {
    return ChooseFillChecked(isWin11, backdropRequested, transparencyOn,
                             backdropApplied) == Fill::SystemBackdrop;
}
}  // namespace backdrop

// Byte-for-byte mirror of tests/paint_mode_logic.h (fix thse isseue.md P0
// #1/#2/#3): the HONEST background-paint-mode decision. The New Folder popup
// ALWAYS paints its full themed client area (SolidTheme); the Group popup may
// leave ONLY its custom title band for the DWM backdrop (DwmTitleBand), and
// ONLY when the Win11 policy holds AND BOTH DwmSetWindowAttribute and
// DwmExtendFrameIntoClientArea actually succeeded. Any failure -> SolidTheme
// (no white body, no white title strip). Keep in lockstep with the header.
namespace paintmode {
enum class PaintMode { SolidTheme, DwmTitleBand };
inline PaintMode ChoosePaintMode(bool isGroup, bool isWin11, bool transparencyOn,
                                 bool backdropRequested, bool backdropAccepted,
                                 bool frameExtended) {
    if (!isGroup) {
        return PaintMode::SolidTheme;
    }
    const bool policy = isWin11 && backdropRequested && transparencyOn;
    const bool dwmConfirmed = backdropAccepted && frameExtended;
    return (policy && dwmConfirmed) ? PaintMode::DwmTitleBand
                                    : PaintMode::SolidTheme;
}
inline bool PaintsFullClient(PaintMode m) { return m == PaintMode::SolidTheme; }
inline bool UsesTitleBandBackdrop(PaintMode m) {
    return m == PaintMode::DwmTitleBand;
}
inline bool GroupBandCanUseBackdrop(bool isGroup, bool backdropAccepted,
                                    bool frameExtended) {
    return isGroup && backdropAccepted && frameExtended;
}
}  // namespace paintmode

// fix thse isseue.md #1/#2: the EXPLICIT, HRESULT-checked backdrop result.
// Replaces the ambiguous single bool: the caller must NOT treat the OS version
// + the Transparency-effects setting (or a successful backdrop-attribute call)
// alone as proof the backdrop composited. Every relevant DWM HRESULT is
// recorded and checked -- in particular the DwmExtendFrameIntoClientArea result
// is NO LONGER discarded -- so the Group title band only drops its solid fill
// when the custom frame genuinely extended.
struct BackdropResult {
    bool backdropAttributeAccepted = false;  // DwmSetWindowAttribute(DWMSBT) ok
    bool frameExtensionSucceeded = false;    // DwmExtendFrameIntoClientArea ok
    bool groupBandCanUseBackdrop = false;    // isGroup && both of the above
    HRESULT backdropHr = E_FAIL;             // DwmSetWindowAttribute HRESULT
    HRESULT frameExtensionHr = E_FAIL;       // DwmExtendFrameIntoClientArea HRESULT
    // fix thse isseue.md P0 #2: the DWMWA_BORDER_COLOR (DWMWA_COLOR_NONE) result
    // is CHECKED + recorded here, not assumed -- older Win10 rejects it.
    bool borderColorAttempted = false;       // DWMWA_BORDER_COLOR set attempted
    bool borderColorAccepted = false;        // DwmSetWindowAttribute(BORDER_COLOR) ok
    HRESULT borderColorHr = E_FAIL;          // DWMWA_BORDER_COLOR HRESULT
};

// Applies the dark-mode / border / rounded-corner / acrylic-backdrop DWM
// attributes (all best-effort) and returns the honest BackdropResult above. The
// frame is extended into the DPI-scaled custom title band ONLY for the Group
// popup; the native-caption New Folder popup never extends its frame (so it has
// no verified backdrop surface and always paints solid -- see paintmode::).
static BackdropResult ApplyWindowPolish(HWND hwnd, bool dark, bool glass, bool isGroup) {
    BackdropResult result;
    HMODULE dwm = LoadLibraryW(L"dwmapi.dll");
    if (!dwm) {
        return result;
    }
    typedef HRESULT(WINAPI * PFN_Set)(HWND, DWORD, LPCVOID, DWORD);
    PFN_Set set =
        reinterpret_cast<PFN_Set>(GetProcAddress(dwm, "DwmSetWindowAttribute"));
    // fix thse isseue.md #1: DwmExtendFrameIntoClientArea is resolved the SAME
    // dynamic way (no static link to dwmapi), so the mod still loads where the
    // entry point is missing. A local 4-int margins struct matches the MARGINS
    // ABI without pulling the header into this Windhawk translation unit.
    struct DwmMarginsLocal { int cxLeftWidth; int cxRightWidth;
                             int cyTopHeight; int cyBottomHeight; };
    typedef HRESULT(WINAPI * PFN_Extend)(HWND, const DwmMarginsLocal*);
    PFN_Extend extend = reinterpret_cast<PFN_Extend>(
        GetProcAddress(dwm, "DwmExtendFrameIntoClientArea"));
    if (set) {
        BOOL d = dark ? TRUE : FALSE;
        set(hwnd, 20, &d, sizeof(d));  // DWMWA_USE_IMMERSIVE_DARK_MODE
        set(hwnd, 19, &d, sizeof(d));  // older build fallback
        // fix thse isseue.md #4: give the CUSTOM borderless Group popup a
        // TRANSPARENT DWM border (DWMWA_COLOR_NONE), so the compositor draws no
        // perimeter at all -- removing the stray ~1-2px white/black outline that
        // used to track the theme (a surface-matched border still showed a faint
        // edge). The native-caption New Folder popup is left on the system
        // default (isGroup false). dwmborder:: (unit-tested) owns the attribute
        // id + the transparent policy; we do NOT draw a fake inner border.
        if (dwmborder::AppliesTo(isGroup)) {
            COLORREF border = static_cast<COLORREF>(dwmborder::kColorNone);  // DWMWA_COLOR_NONE
            // fix thse isseue.md P0 #2: CHECK + record the border HRESULT; do
            // NOT assume DWMWA_COLOR_NONE succeeded (pre-22000 Win10 rejects it).
            result.borderColorAttempted = true;
            result.borderColorHr =
                set(hwnd, dwmborder::kAttrBorderColor, &border, sizeof(border));
            result.borderColorAccepted =
                dwmborder::BorderColorAccepted(
                    static_cast<unsigned long>(result.borderColorHr));
        }
        // Rounded corners (UI spec item 17): prefer the native DWM corner
        // preference where supported (corner:: is unit-tested; `glass` is the
        // caller's Win11 gate). Older builds ignore the attribute and keep the
        // child-control region rounding, so this is behavior-preserving.
        if (corner::UsesDwm(corner::ChooseMethod(glass))) {
            DWORD round = static_cast<DWORD>(corner::DwmCornerPreference());  // DWMWCP_ROUND
            set(hwnd, 33, &round, sizeof(round));
        }
        if (glass) {
            // fix thse isseue.md #7: request the Win11 translucent acrylic
            // backdrop (DWMSBT_TRANSIENTWINDOW) via backdrop::kBackdropAcrylic.
            DWORD sbdt = static_cast<DWORD>(backdrop::kBackdropAcrylic);
            // fix thse isseue.md #1: RECORD the real HRESULT. A non-failing
            // return is the ONLY honest proof the compositor accepted the
            // backdrop -- the OS version + the Transparency setting are not.
            result.backdropHr = set(hwnd, 38, &sbdt, sizeof(sbdt));
            result.backdropAttributeAccepted = SUCCEEDED(result.backdropHr);
            // fix thse isseue.md #1: integrate the DWM frame into the CUSTOM
            // Group title band (DPI-scaled layout::TitleBandH()) so the system
            // material composites behind the borderless caption that
            // WM_ERASEBKGND leaves unpainted. Only the Group popup owns a custom
            // band; the New Folder popup keeps its native frame untouched. The
            // frame-extension HRESULT is CHECKED, not discarded (#1): the band
            // may only drop its solid fill when this call genuinely succeeded.
            if (result.backdropAttributeAccepted && isGroup && extend) {
                const int bandPx =
                    ScaleDpi(layout::TitleBandH(), DpiForWindow(hwnd));
                DwmMarginsLocal m = {0, 0, bandPx, 0};
                result.frameExtensionHr = extend(hwnd, &m);
                result.frameExtensionSucceeded =
                    SUCCEEDED(result.frameExtensionHr);
            }
        }
    }
    FreeLibrary(dwm);
    // The Group title band may composite the DWM backdrop ONLY when BOTH the
    // backdrop attribute AND the custom-frame extension succeeded (paintmode::,
    // unit-tested). The New Folder popup never qualifies (it never extends).
    result.groupBandCanUseBackdrop = paintmode::GroupBandCanUseBackdrop(
        isGroup, result.backdropAttributeAccepted,
        result.frameExtensionSucceeded);
    return result;
}

// fix thse isseue.md P0 #3: REAPPLY the DWM custom-frame extension for the Group
// title band. The band height is DPI-scaled (layout::TitleBandH()), so after a
// WM_DPICHANGED its pixel extent moves and the ORIGINAL extended margin from
// ApplyWindowPolish goes stale -- the transparent frame region would no longer
// line up with the repainted band and a white strip can creep back. Microsoft's
// "Custom Window Frame Using DWM" guidance likewise re-extends the frame on
// activation. This re-extends the frame to the CURRENT DPI-scaled band using
// the same dynamically-resolved DwmExtendFrameIntoClientArea (no static dwmapi
// link), so a missing entry point is a harmless no-op. Returns true only when
// the frame genuinely re-extended. Callers gate this on a CONFIRMED backdrop
// (d->acrylicActive), so the SolidTheme fallback paths are never touched.
static bool ReextendTitleBandFrame(HWND hwnd, UINT dpi) {
    HMODULE dwm = LoadLibraryW(L"dwmapi.dll");
    if (!dwm) {
        return false;
    }
    struct DwmMarginsLocal { int cxLeftWidth; int cxRightWidth;
                             int cyTopHeight; int cyBottomHeight; };
    typedef HRESULT(WINAPI * PFN_Extend)(HWND, const DwmMarginsLocal*);
    PFN_Extend extend = reinterpret_cast<PFN_Extend>(
        GetProcAddress(dwm, "DwmExtendFrameIntoClientArea"));
    bool ok = false;
    if (extend) {
        const int bandPx = ScaleDpi(layout::TitleBandH(), dpi);
        DwmMarginsLocal m = {0, 0, bandPx, 0};
        ok = SUCCEEDED(extend(hwnd, &m));
    }
    FreeLibrary(dwm);
    return ok;
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
    UINT dpi = 96;               // monitor DPI the popup renders at (Bug 3)
    HWND edit = nullptr;
    HWND nameLabel = nullptr;    // name-section label; a transient inline
                                 // validation message may replace its text and
                                 // is restored to origNameLabel when the user
                                 // edits the field or the input becomes valid.
    std::wstring origNameLabel;  // the section label's own text, restored after
                                 // a validation message clears (#6 / #9).
    btn::Phase phase = btn::Phase::Idle;  // Create/Cancel button state (Phase 6)
    bool expanded = false;                // compact/expanded mode (Phase 7)
    bool focusVisible = false;            // fix thse isseue.md P0 #1: ring only on
                                          // KEYBOARD focus. A nav key sets this
                                          // true; a pointer press sets it false.
                                          // Owner-draw gates the ring on it.

    // ---- Ctrl+G "Group Selection" popup extension (Task 8) --------------
    // These are populated ONLY for a Group popup. When isGroup is false the
    // popup renders EXACTLY as the New Folder prompt always has (no extra
    // rows, original geometry) -- the New Folder path is byte-for-byte
    // unchanged and never touches any field below.
    //
    // The text shown for each row MIRRORS the pure, unit-tested formatters in
    // tests/group_logic.h (FormatGroupCountLine / FormatGroupItemList /
    // FormatGroupOperationLabel / FormatGroupDestinationLine /
    // FormatGroupLastTargetLine); the mod builds the identical strings here.
    bool isGroup = false;                 // this popup is a Group Selection popup
    std::vector<sel::Item> items;         // full selection for the Selected Items panel
    int listTopRow = 0;                   // Selected Items scroll position (top row)
    int listVisibleRows = 0;              // fix thse isseue.md Phase 1A: TRUE visible-row capacity cached by the owner-draw paint (declared cap reconciled with what the viewport physically fits); 0 until first paint. Wheel/key scrolling pages by THIS so a declared-but-unfit row stays reachable.
    bool showExt = true;                  // fix thse isseue.md Issue 3: Show-extensions toggle defaults ON (mirrors showext::DefaultOn()): a new Group popup shows complete filenames; clicking toggles OFF/ON
    std::wstring destPath;                // resolved/desired destination
    std::wstring lastTarget;              // remembered last destination (may be empty)
    std::wstring effectiveDest;           // #2: captured parent for the dest preview (UI only)
    bool isMove = false;                  // false = Copy (default), true = Move
    int glideProgress = 0;                // #3: Copy|Move active-fill glide (0=Copy..100=Move)
    HWND groupOpToggle = nullptr;         // Phase 6/D: retired single toggle (kept null; segments below)
    HWND segCopyBtn = nullptr;            // Phase 6/D: Copy segment (id 107, Group only)
    HWND segMoveBtn = nullptr;            // Phase 6/D: Move segment (id 108, Group only)
    HWND groupInfo = nullptr;             // Selected Items panel STATIC (scroll target)
    HWND extToggleTip = nullptr;          // fix thse isseue.md P2 (10249-10250): tooltip giving the custom-drawn Show-extensions switch a real, state-aware accessible name
    HWND destLabel = nullptr;             // Destination card label (Group only)
    HWND destField = nullptr;             // Destination read-only field (Group only)
    HWND destCopyBtn = nullptr;           // Destination "copy path" button (id 111, Group only)
    HWND infoBtn = nullptr;               // fix thse isseue.md UI-03: footer INFO/help button (id 121, Group only)
    HWND helpCard = nullptr;              // fix thse isseue.md Issue 1: transient navigation-help card -- a SEPARATE non-activating popup (NOT a child overlay), created on demand, painted by HelpCardWndProc so the Selected Items list repaint can never cover it
    int  helpVis = 0;                     // infohelp::Vis for the help card (0=Hidden, 1=Shown)
    bool helpPtrOnBtn = false;            // Issue 1: pointer is over the '?' button (hover tracking)
    bool helpPtrOnCard = false;           // Issue 1: pointer is over the help card (hover tracking)
    unsigned long long helpCloseAtTick = 0;  // Issue 1: GetTickCount64() deadline for the grace-close (0 = not armed); infohelp::CloseDelayMs()
    int  helpScrollY = 0;                 // Overflow card: vertical scroll offset of the help content (device px); helpscroll::ClampScroll keeps it in range
    int  helpContentH = 0;                // Overflow card: measured FULL scrollable content height (device px), set by MeasureHelpCard
    int  helpViewportH = 0;               // Overflow card: scrollable viewport height (device px, heading band excluded), set by MeasureHelpCard
    // Section-navigation chips (fix thse isseue.md section 3). The chip strip is
    // painted FIXED under the header; these are its live geometry + state, set
    // by PaintHelpCard/MeasureHelpCard and read by HelpCardWndProc for clicks,
    // active tracking and horizontal strip scrolling (helpchips:: model).
    int  helpSectionY[8] = {0};           // per-section content-Y anchors (device px), from MeasureHelpCard
    int  helpSectionCount = 0;            // number of sections captured (== helpchips::Count())
    int  helpActiveChip = 0;              // currently selected/active chip (0..count-1)
    int  helpChipScrollX = 0;             // horizontal scroll offset of the (overflowing) chip strip, device px
    int  helpChipCount = 0;               // chips laid out this paint
    int  helpChipLeft[8] = {0};           // chip left edges in strip CONTENT coords (device px)
    int  helpChipRight[8] = {0};          // chip right edges in strip CONTENT coords (device px)
    int  helpChipStripW = 0;              // total chip strip width (device px)
    int  helpChipVisibleW = 0;            // visible chip-band width (content area, device px)
    int  helpChipViewX = 0;               // client-x where strip content-x 0 maps (before scroll)
    int  helpChipBandTop = 0;             // client-y of the chip band top
    int  helpChipBandBottom = 0;          // client-y of the chip band bottom
    // Auto-hiding body scrollbar (fix thse isseue.md add-on section 4): the
    // thumb reveals on interaction and hides after a short idle window via the
    // tested sbauto:: model. A ONE-SHOT hide timer repaints exactly once when
    // the window lapses -- no rapid repaint loop. The gutter stays reserved so
    // showing/hiding never reflows the body.
    unsigned long long helpSbRevealTick = 0;   // GetTickCount64() of the last scrollbar-reveal interaction
    // GitHub author footer (fix thse isseue.md section 6). The footer's content-Y
    // range (set while measuring) plus the live body-viewport client bounds (set
    // while painting) let HelpCardWndProc hit-test a Ctrl + left-click on the
    // footer link and open helpfooter::Url().
    // add-on section 4: the author footer is a FIXED band at the card bottom
    // (OUTSIDE the scrollable body). These are the Ashix TOKEN hit rect in
    // CLIENT coords, set by PaintHelpFooter and read by HelpCardWndProc so a
    // Ctrl + left-click OR a plain double left-click on the token opens the URL.
    int  helpFooterLeft = -1;             // Ashix token left (client px); -1 = none
    int  helpFooterRight = -1;            // Ashix token right (client px)
    int  helpFooterTop = -1;              // Ashix token top (client px)
    int  helpFooterBot = -1;              // Ashix token bottom (client px)
    bool helpFooterHot = false;           // pointer is over the Ashix token (hover highlight)
    int  helpBodyViewTop = 0;             // client-y of the scrollable body viewport top
    int  helpBodyViewBottom = 0;          // client-y of the scrollable body viewport bottom
    HWND createBtn = nullptr;             // Create button (relayout target, Phase 7)
    HWND cancelBtn = nullptr;             // Cancel button (relayout target, Phase 7)
    HWND sizeToggle = nullptr;            // compact/expanded titlebar toggle (id 103, Group only)
    HWND minBtn = nullptr;                // title-bar Minimize (id 104, Group only; #1/#2)
    HWND closeBtn = nullptr;              // title-bar Close (id 105, Group only; #2)
    // ---- Auto Group (ADD-ON) popup state. Only used for a Group popup when
    // autoGroupEnabled is on. The button is time-gated by the Phase C countdown
    // model (agcd::), which is driven by a repaint timer and GetTickCount64().
    HWND autoGroupBtn = nullptr;          // Auto Group button (id IDC_AUTOGROUP_BTN, Group only)
    unsigned long long autoGroupStartTick = 0;  // monotonic tick captured when THIS popup opened (fresh per popup)
    bool autoGroupVisible = false;        // Auto Group button created for this popup (settings-enabled)
    bool autoGroupRequested = false;      // set true ONLY when the user confirms Auto Group (never by normal Group)
    // fix thse isseue.md #5: last VISIBLE countdown state. The repaint timer
    // (id 112) invalidates the Auto Group button ONLY when the whole-second
    // M:SS label or the enabled/disabled state actually changes -- never once
    // per tick -- which removes the old 70ms flicker.
    int autoGroupLastSec = -1;            // last whole-second shown (agcd::RemainingSeconds); -1 = nothing drawn yet
    bool autoGroupLastEnabled = false;    // last enabled state pushed to EnableWindow()
    // ---- fix thse isseue.md #2/#3: LIVE summary-chip interaction state (Group
    // popup only). The chip strip lives in the owner-draw list (id 110); the
    // painter records each chip's hit rectangle + kind and the strip viewport,
    // the modal loop hit-tests clicks/wheel against them, and the row renderer
    // applies the active filter. Stored as primitives because the chipfilter::
    // / chipmodel:: types are defined LATER in this single-pass TU. These NEVER
    // touch the real Explorer selection or the Copy/Move/Group plan.
    int chipFilterKind = 0;               // chipfilter::Kind as int: 0 None,1 Items,2 Folders,3 NoExt,4 Ext
    std::wstring chipFilterKey;           // normalized ext key when kind==Ext
    int chipScrollOffset = 0;             // horizontal chip-strip pan (px); INDEPENDENT of listTopRow
    viewmode::Mode selViewMode = viewmode::Mode::List; // fix thse isseue.md Phase A: Selected Items presentation mode (List/Large/ExtraLarge); Ctrl+wheel cycles it, never changes the real selection
    int selGridCols = 1;                  // Phase A: last painted grid column count (cached so the modal loop can preserve the first-visible item across a mode change); 1 in List mode
    int pendingFirstItem = -1;            // v0.12.68 #2: first VISIBLE item index captured just before a Ctrl+Wheel view switch; the next paint converts it to listTopRow for the NEW mode's columns (viewmode::TopRowForItem), clamps it, then resets to -1 -- preserving the visible context across a zoom
    unsigned long long listSbActivityTick = 0; // fix thse isseue.md Phase B: GetTickCount64() of the last list scroll/interaction; the scrollbar stays visible for sbauto::kIdleHideMs after it, then a one-shot timer fades it
    bool listPointerInside = false;       // Phase B: pointer currently over the Selected Items viewport (keeps the auto-hide scrollbar revealed); cleared by the idle timer
    int chipFocusIndex = -1;              // keyboard-focused chip index (-1 = none)
    int chipNavItem = -1;                 // fix thse isseue.md Phase 5 + ADD-ON: current find-navigation match as a STABLE item index (survives a view-mode change); -1 = none. Kept SEPARATELY from the chip filter so a zoom/reflow never loses the match.
    int chipContentW = 0;                 // total chip-strip content width @ last paint (px)
    int chipViewportW = 0;                // chip-strip viewport width @ last paint (px)
    RECT chipStripRect = {0, 0, 0, 0};    // chip-strip hit/viewport area in list (id 110) client coords
    RECT listRowsRect  = {0, 0, 0, 0};    // fix thse isseue.md P1: the REAL item-row viewport (below the header/chips/divider), list (id 110) client coords; the painter caches it so the wheel router scrolls rows ONLY over the rows, not the whole control
    std::vector<RECT> chipHitRects;       // per-chip clickable rect (client coords, already scroll-adjusted)
    std::vector<int> chipHitKind;         // chipfilter::Kind as int, parallel to chipHitRects
    std::vector<std::wstring> chipHitKey; // ext key per chip (empty unless Ext), parallel to chipHitRects
    bool showExtLastPainted = false;      // last showExt state the toggle control reflected
    RECT chipToggleRect = {0, 0, 0, 0};   // fix thse isseue.md #3: Show-extensions toggle hit rect (list client coords)
    bool manualSized = false;             // fix thse isseue.md Phase 3: the user has resized or maximized the popup, so the layout must NOT force it back to the fixed compact/expanded size (density toggles then only reflow content)
    // fix thse isseue.md v0.12.67 (top note): client size latched at
    // WM_ENTERSIZEMOVE so WM_EXITSIZEMOVE can tell a pure MOVE (dragging the
    // popup by its custom header) from a real RESIZE. A move must NOT run the
    // reflow/relayout path -- that repositioned every child and repainted, so
    // the whole popup flashed / re-rendered when dragged by the header. Only a
    // genuine size change reflows now.
    int sizeMoveClientW = 0;              // client width  at the last WM_ENTERSIZEMOVE
    int sizeMoveClientH = 0;              // client height at the last WM_ENTERSIZEMOVE
    bool inSizeMove = false;              // true between WM_ENTERSIZEMOVE and WM_EXITSIZEMOVE

    // fix thse isseue.md Phase 1: per-popup cache of high-resolution Shell icon
    // handles, keyed by iconsrc::CacheKey(type, tier, dpi). Scrolling and
    // zooming reuse ONE extracted source instead of re-extracting a lower-res
    // copy every paint; every HICON is released on popup teardown so repeated
    // zoom/scroll never leaks a GDI handle. (scroll offset / view mode are NOT
    // part of the key, so they stay independent of icon resolution.)
    std::vector<std::pair<std::wstring, HICON>> iconCache;

    // fix thse isseue.md Issue 2: async image-thumbnail cache for the Large /
    // Extra large grid views. Extraction runs on a dedicated background thread
    // (IShellItemImageFactory) so opening the popup and scrolling never block;
    // results are cached by "path|px" and reused while scrolling. Only true
    // image files are ever requested (the tested thumbs:: model); everything
    // else keeps the crisp Shell icon fallback. Every bitmap + the worker thread
    // are released on teardown (StopThumbThread, then this destructor).
    struct ThumbEntry {
        HBITMAP bmp = nullptr;
        int w = 0, h = 0;
        bool done = false;
        bool failed = false;
    };
    std::vector<std::pair<std::wstring, ThumbEntry>> thumbCache;  // key = path|px
    std::vector<std::pair<std::wstring, int>> thumbQueueReq;      // pending (path, px)
    CRITICAL_SECTION thumbCs = {};
    HANDLE thumbThread = nullptr;
    HANDLE thumbWake   = nullptr;
    volatile LONG thumbStop = 0;
    bool   thumbInit = false;
    HWND   ownerHwnd = nullptr;      // for the worker's cross-thread repaint post

    ~NamePromptData() {
        for (auto& kv : iconCache)
            if (kv.second) DestroyIcon(kv.second);
        for (auto& kv : thumbCache)
            if (kv.second.bmp) DeleteObject(kv.second.bmp);
    }
};

// EditParentIsGroup definition (forward-declared above, next to the inputfield
// EDIT subclasses). Deferred to here because its body dereferences
// NamePromptData (pd->isGroup), which is only a complete type from this point
// on in this single-pass translation unit.
static bool EditParentIsGroup(HWND hwnd) {
    NamePromptData* pd = reinterpret_cast<NamePromptData*>(
        GetWindowLongPtrW(GetParent(hwnd), GWLP_USERDATA));
    return pd && pd->isGroup;
}

// Mirror of tests/group_logic.h formatters (kept byte-identical to the
// unit-tested pure logic). GroupCountLineText/GroupItemListText were retired
// once the Selected Items panel switched to the sel:: row formatters plus the
// listscroll:: viewport model, so only the still-consumed destination /
// last-target formatters remain here.
// (fix thse isseue.md section 3) GroupOperationLabelText() was removed: the
// single Copy/Move toggle it labelled was replaced by two segments driven by
// segmented::SegmentLabel, so the helper had no call site (it was
// [[maybe_unused]]) and no test or guard referenced it.
// Destination card field text (Phase 3b): the destination is its OWN section
// now, so this returns just the resolved destination for the read-only field,
// with the secondary recent/last target appended only when relevant. dest::
// (unit-tested) decides what to show; the resolver precedence (fixed > typed >
// last) stays in ResolveGroupDestination and is NOT changed here.
static std::wstring GroupDestinationFieldText(const std::wstring& destPath,
                                              const std::wstring& effectiveDest,
                                              const std::wstring& lastTarget) {
    // fix thse isseue.md #2: the effective destination (the captured parent) is
    // now threaded in instead of a hard-coded empty string, so an unconfigured
    // fixed dest shows the real parent rather than the placeholder.
    dest::DestModel dm = dest::ComputeDestination(destPath, effectiveDest,
                                                  lastTarget);
    std::wstring text = dm.fieldText;
    if (dm.showLastTarget) { text += L"   \u00b7   "; text += dm.lastTargetText; }
    return text;
}

// Apply the resolved destination to the read-only field, path-ellipsized to the
// field's text cell so a long path shows "...\leaf" instead of being clipped
// (destfield::UsePathEllipsis(), fix thse isseue.md #6). Measured with the
// field's own font + current pixel width, so it stays correct across DPI and the
// compact/expanded width change.
static void ApplyDestFieldText(HWND field, const std::wstring& destPath,
                               const std::wstring& effectiveDest,
                               const std::wstring& lastTarget, UINT dpi) {
    if (!field) return;
    std::wstring full = GroupDestinationFieldText(destPath, effectiveDest, lastTarget);
    if (destfield::UsePathEllipsis() && !full.empty()) {
        RECT rc;
        GetClientRect(field, &rc);
        auto S = [dpi](int v) {
            return static_cast<int>(MulDiv(v, static_cast<int>(dpi), 96));
        };
        const int textPx = (rc.right - rc.left)
                           - S(destfield::TextLeftInset()) - S(destfield::kEdgePad);
        if (textPx > S(24)) {
            wchar_t buf[MAX_PATH * 4] = {0};
            lstrcpynW(buf, full.c_str(), MAX_PATH * 4);
            HDC hdc = GetDC(field);
            HFONT f = reinterpret_cast<HFONT>(SendMessageW(field, WM_GETFONT, 0, 0));
            HGDIOBJ oldF = f ? SelectObject(hdc, f) : nullptr;
            PathCompactPathW(hdc, buf, static_cast<UINT>(textPx));
            if (oldF) SelectObject(hdc, oldF);
            ReleaseDC(field, hdc);
            full.assign(buf);
        }
    }
    SetWindowTextW(field, full.c_str());
}

// (fix thse isseue.md section 3/4) BuildGroupPanelText() -- the obsolete plain
// multiline-STATIC Selected Items text builder -- was removed. The Selected
// Items panel is an SS_OWNERDRAW list (WM_DRAWITEM id 110) that draws rows
// directly from the unit-tested sel:: / listscroll:: models (see
// RefreshGroupPanel below), so the text-blob builder had no call site (it was
// [[maybe_unused]]) and no test or guard referenced it.

// Re-render the Selected Items panel after the scroll position changes. The
// panel is an SS_OWNERDRAW list (WM_DRAWITEM id 110); a scroll only changes
// d->listTopRow, so this just INVALIDATES it and the owner-draw repaints the new
// visible slice from sel:: / listscroll:: -- no STATIC text is rebuilt. No-op for New Folder popups.
static void RefreshGroupPanel(NamePromptData* d) {
    if (!d || !d->isGroup || !d->groupInfo) return;
    // Phase 5/C: the Selected Items panel is now an SS_OWNERDRAW list; scrolling
    // only changes d->listTopRow (via listscroll::), so a repaint re-runs the
    // owner-draw over the new visible slice -- no text rebuild needed.
    // fix thse isseue.md #2: FALSE = no background erase before the owner-draw.
    // The list is double-buffered (WM_DRAWITEM id 110 paints a memory DC then
    // BitBlts once), so a wheel/key scroll repaint no longer flashes.
    InvalidateRect(d->groupInfo, nullptr, FALSE);
}

// Compact/Expanded relayout (UI spec item 15, Phase 7). ONE layout source
// (layout::ComputePromptLayout) drives BOTH modes: recompute for the current
// d->expanded, move every child to its new DPI-scaled rect, and grow/shrink the
// popup by the same model -- mirroring exactly how the window was first sized.
// Expanded only enlarges the Group Selected Items viewport; New Folder geometry
// is unaffected (extra == 0). Focus is left untouched (SWP_NOACTIVATE).
// fix thse isseue.md v0.12.65 Bug 1: forward declaration of the single
// authoritative "effective layout" (defined just after the winresize helpers it
// relies on). ApplyPromptLayout, WM_ERASEBKGND and WM_NCHITTEST all consume it,
// so paint, hit-test and child-move can never disagree about the live geometry.
static layout::PromptLayout ComputeEffectiveLayout(HWND hwnd, const NamePromptData* d);

// fix thse isseue.md item 6 (P1 6417-6445): deferred-move commit model, mirror
// of tests/failguard_logic.h. Defined HERE (ahead of its only consumer,
// ApplyPromptLayout below) so it is in scope at the call site; the sibling
// helpclass/gdiguard mirrors live later, each ahead of their own first use.
namespace layoutdefer {
inline bool DeferredCommitOk(bool beginOk, bool endOk) { return beginOk && endOk; }
inline bool NeedsIndividualFallback(bool beginOk, bool endOk) {
    return !DeferredCommitOk(beginOk, endOk);
}
}  // namespace layoutdefer

static void ApplyPromptLayout(HWND hwnd, NamePromptData* d,
                              const layout::PromptLayout* reflowOverride = nullptr) {
    if (!d) return;
    const UINT dpi = d->dpi;
    auto S = [dpi](int v) { return ScaleDpi(v, dpi); };
    // fix thse isseue.md UI-01 / UI-05: a WM_SIZE / WM_EXITSIZEMOVE reflow passes
    // a layout already stretched to the live client rect (layout::ReflowToClient);
    // otherwise use the PREFERRED compact/expanded model. The window is only
    // resized to the model when the popup has NOT been manually resized or
    // maximized (the guard further below), so a reflow never fights the OS size.
    // fix thse isseue.md v0.12.65 Bug 1: a WM_SIZE / WM_EXITSIZEMOVE reflow still
    // passes its reflowed layout as the override; everything else now goes
    // through the ONE effective-layout helper (preferred for a normal popup,
    // reflowed once maximized/manually resized) so a Ctrl+E density toggle on an
    // already-resized popup reflows the content into the user's size instead of
    // snapping the children back to the preferred compact/expanded rects.
    const layout::PromptLayout L = reflowOverride ? *reflowOverride :
        ComputeEffectiveLayout(hwnd, d);
    // fix thse isseue.md ("Next exact fix order" #1-#4): the +/- resize is now
    // ONE atomic transaction, not a sequence of MoveWindow(..., TRUE) that each
    // repainted mid-flight (the wiggly/staggered redraw). Flow: suppress
    // painting -> move every child atomically (BeginDeferWindowPos, no per-child
    // repaint) -> resize the window (below) -> re-enable painting -> ONE
    // RedrawWindow (RDW_ALLCHILDREN) at the end.
    SendMessageW(hwnd, WM_SETREDRAW, FALSE, 0);

    // redraw::kRepaintChildOnMove == false: DeferWindowPos carries no per-move
    // repaint. Group-only children are null on the New Folder popup, so the
    // null-handle skip reproduces the old `isGroup && handle` guards exactly.
    // fix thse isseue.md P1 (6417-6445): collect every child move FIRST so a
    // failed BeginDeferWindowPos()/EndDeferWindowPos() can replay them with
    // individual SetWindowPos() calls. DeferWindowPos only APPLIES the moves on
    // a successful EndDeferWindowPos(), so a broken batch moves ZERO children --
    // without a fallback some controls would be left at stale coordinates.
    struct PendingMove { HWND h; int x, y, w, ht; };
    PendingMove moves[16];
    int moveCount = 0;
    auto queueMove = [&](HWND h, const auto& r) {
        if (h && moveCount < 16) {
            moves[moveCount++] = PendingMove{h, S(r.x), S(r.y), S(r.w), S(r.h)};
        }
    };
    queueMove(d->groupInfo,   L.selectedContent);
    queueMove(d->destLabel,   L.destLabel);
    queueMove(d->destField,   inputfield::InnerEditRect(L.destField.x, L.destField.y, L.destField.w, L.destField.h));
    queueMove(d->destCopyBtn, L.destCopyBtn);
    queueMove(d->infoBtn,     L.infoButton);        // UI-03: footer info/help button
    // fix thse isseue.md Issue 1: the navigation-help card is a SEPARATE
    // non-activating popup window (not a child), so it is NOT laid out here;
    // PositionHelpCard() re-anchors it to the '?' button on move/resize/DPI.
    queueMove(d->nameLabel,   L.nameLabel);
    queueMove(d->edit,        inputfield::InnerEditRect(L.nameField.x, L.nameField.y, L.nameField.w, L.nameField.h));
    queueMove(d->segCopyBtn,  L.segCopy);
    queueMove(d->segMoveBtn,  L.segMove);
    queueMove(d->createBtn,   L.createButton);
    queueMove(d->cancelBtn,   L.cancelButton);
    queueMove(d->autoGroupBtn, L.autoGroupButton);  // ADD-ON: keep the Auto Group button aligned on relayout
    // fix thse isseu e.md #5: the size control stays in the tested chrome rect
    // so relayout can't drift it back onto the Selected Items card.
    queueMove(d->sizeToggle,  L.chromeSizeBtn);
    queueMove(d->minBtn,      L.chromeMinBtn);
    queueMove(d->closeBtn,    L.chromeCloseBtn);

    // Try the atomic deferred transaction first.
    HDWP hdwp = BeginDeferWindowPos(moveCount);
    const bool beginOk = (hdwp != nullptr);
    bool committed = false;
    if (beginOk) {
        bool chainOk = true;
        for (int i = 0; i < moveCount; ++i) {
            HDWP next = DeferWindowPos(hdwp, moves[i].h, nullptr,
                                       moves[i].x, moves[i].y, moves[i].w, moves[i].ht,
                                       SWP_NOZORDER | SWP_NOACTIVATE);
            if (!next) { chainOk = false; break; }  // handle is now invalid
            hdwp = next;
        }
        if (chainOk) committed = (EndDeferWindowPos(hdwp) != FALSE);
    }

    // fix thse isseue.md P1 (6417-6445): if the batch could not be started or
    // committed, no child actually moved -- replay every move individually so
    // none are left at stale coordinates, and record genuine failures.
    if (layoutdefer::NeedsIndividualFallback(beginOk, committed)) {
        int moveFailures = 0;
        for (int i = 0; i < moveCount; ++i) {
            if (!SetWindowPos(moves[i].h, nullptr,
                              moves[i].x, moves[i].y, moves[i].w, moves[i].ht,
                              SWP_NOZORDER | SWP_NOACTIVATE)) {
                ++moveFailures;
            }
        }
        Wh_Log(L"ApplyPromptLayout: deferred child-move batch failed "
               L"(beginOk=%d committed=%d); used individual SetWindowPos "
               L"fallback (%d of %d moves failed)",
               beginOk, committed, moveFailures, moveCount);
    }

    // fix thse isseue.md #6: re-ellipsize the Destination path to the field's
    // NEW width, now that the batch has resized the field.
    if (d->isGroup && d->destField) {
        ApplyDestFieldText(d->destField, d->destPath, d->effectiveDest, d->lastTarget, dpi);
    }

    // #5 DPI relayout: after a DPI change the children must (a) adopt the newly
    // scaled font and (b) get freshly scaled rounded regions -- otherwise they
    // keep the old-DPI font/clip and the exact rounded-corner + pixelation
    // artifacts this UI pass set out to remove come back on a monitor hop.
    // UiFont(dpi) atomically swaps the cached font (deletes the old, creates the
    // new); re-selecting it onto every child completes the swap so nothing still
    // references the released handle.
    HFONT font = UiFont(dpi);
    // S14 (fix thse isseue.md v0.12.54): the Destination + Folder name section
    // TITLE labels take the heavier semibold face so every section heading reads
    // consistently bold (the Action / Selected titles are drawn text
    // and get the same semibold font directly at their paint sites). Field, body
    // and state text stay on the regular UiFont.
    HFONT titleFont = UiFontSemibold(dpi);
    HWND fontTargets[] = {d->groupInfo, d->destLabel, d->destField,
                          d->nameLabel, d->edit,
                          d->segCopyBtn, d->segMoveBtn,
                          d->createBtn, d->cancelBtn,
                          d->sizeToggle, d->minBtn, d->closeBtn};
    for (HWND h : fontTargets) {
        if (!h) continue;
        HFONT use = (h == d->destLabel || h == d->nameLabel) ? titleFont : (h == d->destField ? UiFontMono(dpi) : font);
        SendMessageW(h, WM_SETFONT, reinterpret_cast<WPARAM>(use), TRUE);
    }
    // S12 (fix thse isseue.md v0.12.52): the New Folder EDIT no longer gets a
    // SetWindowRgn round-rect clip on relayout. A GDI region clip stair-steps
    // the corners at 125/150/200% DPI; instead the parent now paints the SAME
    // anti-aliased rounded field surface (fill + border) for the New Folder
    // popup in WM_ERASEBKGND that the Group popup already uses, so there is no
    // per-control region to recreate here anymore. (The Group name + Destination
    // fields and the owner-drawn Create/Cancel + Copy/Move controls were de-
    // clipped in earlier passes.)

    // L.windowW/windowH are CLIENT sizes; convert to the OUTER window size (as
    // the create site does) using THIS window's actual styles so a
    // compact/expanded or DPI relayout keeps the whole card stack inside the
    // client area instead of clipping the bottom row (fix thse isseu e.md #1).
    const DWORD style   = static_cast<DWORD>(GetWindowLongPtrW(hwnd, GWL_STYLE));
    const DWORD exStyle = static_cast<DWORD>(GetWindowLongPtrW(hwnd, GWL_EXSTYLE));
    const SIZE outer = PromptOuterSize(L.windowW, L.windowH, style, exStyle, dpi);
    // fix thse isseue.md #1: resize about the CURRENT window center (not the
    // top-left), then clamp to the monitor work area. The old no-move resize flag
    // pinned the top-left so expand/collapse grew only to the right/down; the UI
    // keeps the center fixed. Geometry is the tested center:: model; this is the
    // ONE Win32 wiring site (pinned by a source invariant).
    RECT oldRc{};
    GetWindowRect(hwnd, &oldRc);
    const center::Rect oldR{static_cast<int>(oldRc.left),
                            static_cast<int>(oldRc.top),
                            static_cast<int>(oldRc.right),
                            static_cast<int>(oldRc.bottom)};
    const center::Size newSz{static_cast<int>(outer.cx),
                             static_cast<int>(outer.cy)};
    center::Rect work = oldR;
    MONITORINFO mi;
    mi.cbSize = sizeof(mi);
    HMONITOR mon = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
    if (mon && GetMonitorInfo(mon, &mi)) {
        work = center::Rect{static_cast<int>(mi.rcWork.left),
                            static_cast<int>(mi.rcWork.top),
                            static_cast<int>(mi.rcWork.right),
                            static_cast<int>(mi.rcWork.bottom)};
    }
    const center::Point np = center::CenteredResize(oldR, newSz, work);
    Wh_Log(L"PromptResize: old=(%d,%d %dx%d) new=(%d,%d %dx%d) mode=%ls "
           L"center=(%d,%d)",
           static_cast<int>(oldRc.left), static_cast<int>(oldRc.top),
           static_cast<int>(oldRc.right - oldRc.left),
           static_cast<int>(oldRc.bottom - oldRc.top),
           np.x, np.y, static_cast<int>(outer.cx), static_cast<int>(outer.cy),
           d->expanded ? L"expanded" : L"compact",
           np.x + static_cast<int>(outer.cx) / 2,
           np.y + static_cast<int>(outer.cy) / 2);
    // fix thse isseue.md Phase 3: do NOT force the popup back to the fixed
    // compact/expanded size once the user has MAXIMIZED or hand-resized it
    // (that would snap it back / un-maximize it -- the old unconditional
    // SetWindowPos). The child controls were already positioned by the
    // queued child-move pass above regardless; only the top-level window RESIZE
    // is skipped, so Ctrl+E density then just reflows the content in place.
    if (!d->manualSized && !IsZoomed(hwnd)) {
        SetWindowPos(hwnd, nullptr, np.x, np.y, outer.cx, outer.cy,
                     SWP_NOZORDER | SWP_NOACTIVATE);
    }
    // fix thse isseue.md #3/#4: re-enable painting and issue ONE final repaint
    // reaching the window AND every child (redraw::FinalRepaintFlags() ==
    // RDW_INVALIDATE|RDW_ERASE|RDW_UPDATENOW|RDW_ALLCHILDREN), so the resize
    // resolves in a single clean frame -- no intermediate/staggered frames.
    SendMessageW(hwnd, WM_SETREDRAW, TRUE, 0);
    RedrawWindow(hwnd, nullptr, nullptr, redraw::FinalRepaintFlags());
}

// fix thse isseue.md #1: the Status/Ready feature was removed. The folder /
// Group operation now runs on the worker AFTER the popup closes and its real
// outcome is logged there, so the popup no longer has a live status band, a
// custom status app-message, or a status-setting helper.

// ---- Section card model + renderer (Phase 4/B, fix thse isseu e.md #2) ----
// Mirror of tests/card_logic.h (radius). The Group popup paints Selected /
// Destination / Folder name each as its OWN rounded card on the panel,
// instead of one flat surface. Keep kCardRadius in lockstep with card_logic.h.
namespace card { inline constexpr int kCardRadius = 8; }

// ---- Selected Items list column geometry (Phase 5/C) ----
// Mirror of tests/list_columns_logic.h. The owner-drawn Selected Items rows are
// `name .... EXT  SIZE`; these fixed right-aligned columns + NameWidth() keep
// the ellipsized name from overlapping them. Keep in lockstep with the header.
namespace listcol {
inline constexpr int kExtW     = 46;
inline constexpr int kSizeW    = 64;
inline constexpr int kColGap   = 8;
inline constexpr int kMinNameW = 60;
inline int NameWidthFrom(int contentWpx, int extWpx, int sizeWpx,
                         int colGapPx, int minNameWpx) {
    int n = contentWpx - extWpx - sizeWpx - colGapPx;
    return n < minNameWpx ? minNameWpx : n;
}
inline int NameWidth(int rowW) {
    int n = rowW - kExtW - kSizeW - kColGap;
    return n < kMinNameW ? kMinNameW : n;
}
}  // namespace listcol

// Extension-BADGE geometry for the Selected Items list (fix thse isseue.md #5).
// Byte-for-byte mirror of tests/list_badge_logic.h -- a file row's uppercased
// extension is drawn as a rounded PILL ([PDF]) instead of plain text. Unit-
// agnostic arithmetic (renderer passes device px + DPI-scaled metrics).
namespace listbadge {
inline constexpr int kPadX   = 8;   // fix #11: X padding 6 -> 8
inline constexpr int kPadY   = 3;   // fix #11: Y inset 2 -> 3
inline constexpr int kRadius = 5;   // fix #2 (pass6): pill corner radius 4 -> 5 (soft chip)
inline constexpr int kMinW   = 32;  // fix #11: min pill width 24 -> 32
struct Box {
    int left = 0, top = 0, right = 0, bottom = 0;
    int w() const { return right - left; }
    int h() const { return bottom - top; }
    bool empty() const { return right <= left || bottom <= top; }
};
inline bool ShowBadge(const std::wstring& ext) { return !ext.empty(); }
inline int BadgeWidth(int textW, int padX, int minW) {
    const int w = textW + 2 * padX;
    return w < minW ? minW : w;
}
inline Box BadgeRect(int cellRight, int rowTop, int rowH, int badgeW, int padY) {
    Box b;
    b.right  = cellRight;
    b.left   = cellRight - badgeW;
    b.top    = rowTop + padY;
    b.bottom = rowTop + rowH - padY;
    return b;
}
}  // namespace listbadge

// ---- per-extension soft badge colours (fix thse isseue.md #12) ----
// Byte-for-byte mirror of tests/badge_color_logic.h. Maps a file's uppercased
// extension label (sel::ExtLabel) to a soft category chip: a soft fill + a
// darker/lighter "ink" reused for the text AND the hairline border. Unknown
// types inherit the themed neutral chip. Never a saturated full-strength block.
namespace badgecolor {

enum class Cat { Neutral, Image, Audio, Video, Document, Archive, Code };

inline std::wstring Norm(const std::wstring& ext) {
    std::wstring e = ext;
    size_t start = (!e.empty() && e[0] == L'.') ? 1u : 0u;
    std::wstring out;
    for (size_t i = start; i < e.size(); ++i) {
        wchar_t c = e[i];
        if (c >= L'A' && c <= L'Z') c = static_cast<wchar_t>(c - L'A' + L'a');
        out.push_back(c);
    }
    return out;
}

inline Cat CategoryFor(const std::wstring& extLabel) {
    const std::wstring e = Norm(extLabel);
    auto is = [&](std::initializer_list<const wchar_t*> xs) {
        for (const wchar_t* x : xs) if (e == x) return true;
        return false;
    };
    if (is({L"jpg", L"jpeg", L"png", L"gif", L"bmp", L"webp", L"svg", L"ico",
            L"tif", L"tiff", L"heic"})) return Cat::Image;
    if (is({L"mp3", L"wav", L"flac", L"aac", L"ogg", L"m4a", L"wma"}))
        return Cat::Audio;
    if (is({L"mp4", L"mkv", L"mov", L"avi", L"wmv", L"webm", L"flv", L"m4v"}))
        return Cat::Video;
    if (is({L"pdf", L"doc", L"docx", L"txt", L"rtf", L"odt", L"xls", L"xlsx",
            L"ppt", L"pptx", L"md", L"csv"})) return Cat::Document;
    if (is({L"zip", L"rar", L"7z", L"tar", L"gz", L"bz2", L"xz", L"cab"}))
        return Cat::Archive;
    if (is({L"c", L"cpp", L"cc", L"h", L"hpp", L"cs", L"js", L"ts", L"py",
            L"java", L"rb", L"go", L"rs", L"php", L"json", L"xml", L"html",
            L"css", L"sh", L"ps1"})) return Cat::Code;
    return Cat::Neutral;
}

struct Chip {
    bool inherit;
    unsigned fill;   // 0xRRGGBB
    unsigned ink;    // 0xRRGGBB (text + border)
};

inline Chip ChipFor(Cat c, bool dark) {
    switch (c) {
        case Cat::Image:
            return dark ? Chip{false, 0x1E3346u, 0x8FC2ECu}
                        : Chip{false, 0xDCEBFBu, 0x1A4E7Au};
        case Cat::Audio:
            return dark ? Chip{false, 0x332844u, 0xC3A6E6u}
                        : Chip{false, 0xE9DEF7u, 0x5B3B86u};
        case Cat::Video:
            return dark ? Chip{false, 0x3E2838u, 0xE6A6CCu}
                        : Chip{false, 0xF7DEEEu, 0x7A2E5Bu};
        case Cat::Document:
            return dark ? Chip{false, 0x44281Eu, 0xE6A48Fu}
                        : Chip{false, 0xFBE2DCu, 0x8A2E1Au};
        case Cat::Archive:
            return dark ? Chip{false, 0x3C3418u, 0xDBC27Eu}
                        : Chip{false, 0xF7EDD2u, 0x7A5A12u};
        case Cat::Code:
            return dark ? Chip{false, 0x1E3A28u, 0x8FD6A6u}
                        : Chip{false, 0xDCF2E3u, 0x1E5F38u};
        case Cat::Neutral:
        default:
            return Chip{true, 0u, 0u};
    }
}

inline Chip ChipForLabel(const std::wstring& extLabel, bool dark) {
    return ChipFor(CategoryFor(extLabel), dark);
}

// A SOFTER hairline than the text ink (fix thse isseue.md #9/#14: "border less
// dominant", "no hard black outline"). The text keeps the full category ink;
// the border is the ink blended 55% toward the chip fill, so it reads as a
// muted edge of the same hue -- lighter than the ink in light theme, softer
// than the ink in dark theme, and never a saturated/black outline. Only
// meaningful for a tinted (non-inherit) chip; the neutral chip keeps the
// themed pal.badgeBorder.
inline unsigned SoftBorder(const Chip& c) {
    auto chan = [](unsigned v, int shift) { return (v >> shift) & 0xFFu; };
    // 45% ink + 55% fill, rounded -> biased toward the fill (less dominant).
    auto mix = [](unsigned ink, unsigned fill) {
        return (ink * 45u + fill * 55u + 50u) / 100u;
    };
    const unsigned r = mix(chan(c.ink, 16), chan(c.fill, 16));
    const unsigned g = mix(chan(c.ink, 8),  chan(c.fill, 8));
    const unsigned b = mix(chan(c.ink, 0),  chan(c.fill, 0));
    return (r << 16) | (g << 8) | b;
}

}  // namespace badgecolor

// ---- Summary chip strip + toolbar + filter models ----
// (fix thse isseue.md #2/#3/#4/#5/#6). Byte-for-byte mirrors of the tests/
// logic headers so the live popup and the unit tests share one source of
// truth. Keep each namespace in lockstep with its tests/*_logic.h.

// Byte-for-byte mirror of tests/chip_model_logic.h.
namespace chipmodel {

enum class Kind { Items, Folders, Ext, NoExt };

struct Chip {
    Kind kind = Kind::Items;
    std::wstring label;    // fully composed display text, e.g. "9 items", "3 MD"
    std::size_t count = 0;
    std::wstring extKey;   // normalized lowercase ext key ("md"); empty otherwise
};

// Lowercase + strip a single leading dot. "MD"/".md"/"Md" -> "md".
inline std::wstring NormExtKey(const std::wstring& extLabel) {
    std::wstring e = extLabel;
    std::size_t start = (!e.empty() && e[0] == L'.') ? 1u : 0u;
    std::wstring out;
    for (std::size_t i = start; i < e.size(); ++i) {
        wchar_t c = e[i];
        if (c >= L'A' && c <= L'Z') c = static_cast<wchar_t>(c - L'A' + L'a');
        out.push_back(c);
    }
    return out;
}

// Uppercase a normalized key for display. "md" -> "MD".
inline std::wstring UpperExt(const std::wstring& key) {
    std::wstring out;
    for (wchar_t c : key) {
        if (c >= L'a' && c <= L'z') c = static_cast<wchar_t>(c - L'a' + L'A');
        out.push_back(c);
    }
    return out;
}

// Build the ordered chip list from the current selection. Order: items chip,
// a folders chip (only when folders exist), then one chip per unique file
// extension in first-seen order, with extensionless files collapsed into a
// single "No extension" chip in their first-seen position.
inline std::vector<Chip> Build(const std::vector<sel::Item>& items) {
    const std::size_t total = items.size();
    std::size_t folders = 0;
    for (const auto& it : items) if (it.isFolder) ++folders;

    std::vector<Chip> out;
    {
        Chip c;
        c.kind = Kind::Items;
        c.count = total;
        c.label = std::to_wstring(total) + L" " + (total == 1 ? L"item" : L"items");
        out.push_back(c);
    }
    if (folders > 0) {
        Chip c;
        c.kind = Kind::Folders;
        c.count = folders;
        c.label = std::to_wstring(folders) + L" " +
                  (folders == 1 ? L"folder" : L"folders");
        out.push_back(c);
    }

    std::vector<std::wstring> keys;   // first-seen order; "" == no extension
    std::vector<std::size_t>  counts;
    for (const auto& it : items) {
        if (it.isFolder) continue;
        const std::wstring key = NormExtKey(sel::ExtLabel(it.name, false));
        std::size_t idx = keys.size();
        for (std::size_t i = 0; i < keys.size(); ++i) {
            if (keys[i] == key) { idx = i; break; }
        }
        if (idx == keys.size()) { keys.push_back(key); counts.push_back(1); }
        else ++counts[idx];
    }
    for (std::size_t i = 0; i < keys.size(); ++i) {
        Chip c;
        c.count = counts[i];
        if (keys[i].empty()) {
            c.kind = Kind::NoExt;
            c.label = std::to_wstring(counts[i]) + L" No extension";
        } else {
            c.kind = Kind::Ext;
            c.extKey = keys[i];
            c.label = std::to_wstring(counts[i]) + L" " + UpperExt(keys[i]);
        }
        out.push_back(c);
    }
    return out;
}

}  // namespace chipmodel

// ---- Selected Items SUMMARY-CHIP geometry model (ADD-ON) ----
// Byte-for-byte mirror of tests/sum_chip_logic.h. fix thse isseue.md #1:
// DEDICATED pill design (semibold 12px text, 8x4 padding, pill radius >=10px,
// 6px gaps) with a height DERIVED from measured text metrics -- NOT the
// per-file listbadge constants and NOT the old hardcoded 16px height. Pure; the
// painter DPI-scales each token via ScaleDpi and feeds measured text extents in.
namespace sumchip {
inline constexpr int kFontPx    = 12;  // semibold ~600 (UiFontSemibold helper)
inline constexpr int kPadX      = 8;   // horizontal padding per side
inline constexpr int kPadY      = 4;   // vertical padding per side
inline constexpr int kMinRadius = 10;  // minimum corner radius
inline constexpr int kGap       = 6;   // gap between adjacent chips
inline constexpr int kTitleGap  = 6;   // gap between the title row and chip row
inline int ChipHeight(int textH, int padY) { return textH + padY * 2; }
inline int ChipWidth(int textW, int padX) { return textW + padX * 2; }
inline int ChipRadius(int chipH, int minRadius) {
    const int r = chipH / 2;
    return (r < minRadius) ? minRadius : r;
}
inline int NextChipX(int x, int chipW, int gap) { return x + chipW + gap; }
}  // namespace sumchip

// Byte-for-byte mirror of tests/show_ext_logic.h.
namespace showext {

// The toggle starts OFF; sizes are the default per-row metadata.
inline bool DefaultOn() { return true; }  // fix thse isseue.md Issue 3: Show extensions defaults ON

// The toolbar action label.
inline const wchar_t* ToggleLabel() { return L"Show extensions"; }

// A size badge is shown for files with a known size; never for folders.
inline bool RowShowsSize(bool isFolder, bool sizeKnown) {
    return !isFolder && sizeKnown;
}

// When the toggle is ON, a file row renders its COMPLETE filename inline
// (e.g. "README.md") instead of the stem; a separate extension pill is NEVER
// drawn. Folders are never affected (always their whole name, no suffix).
inline bool RowShowsExtInline(bool toggleOn, bool isFolder) {
    return toggleOn && !isFolder;
}

// ---- Compact ON/OFF switch (fix thse isseue.md Phase 3) ----
// The toolbar control is a small theme-aware switch with NO inline text label.
// This is the accessible name used for the tooltip + assistive technology (the
// visible text label is gone, so the control still announces its purpose).
inline const wchar_t* AccessibleName() { return L"Show file extensions"; }

// State wording for assistive tech / the live tooltip (mirror of
// show_ext_logic.h). The switch has no visible text, so the tooltip announces
// whether extensions are currently shown.
inline const wchar_t* StateWord(bool on) { return on ? L"on" : L"off"; }
inline std::wstring AccessibleLabel(bool on) {
    std::wstring s = AccessibleName();
    s += L" (";
    s += StateWord(on);
    s += L")";
    return s;
}

// Switch geometry (unit-agnostic; the renderer passes device px). The switch is
// a 2:1 rounded "pill" track; the knob is a circle inset by `inset` on all
// sides and sits at the LEFT when OFF and the RIGHT when ON. No space is ever
// reserved for a text label.
inline int SwitchTrackW(int trackH) { return trackH * 2; }
inline int SwitchKnobDiameter(int trackH, int inset) {
    const int d = trackH - inset * 2;
    return d < 1 ? 1 : d;
}
inline int SwitchKnobLeft(int trackLeft, int trackW, int trackH, int inset,
                          bool on) {
    const int d = SwitchKnobDiameter(trackH, inset);
    return on ? (trackLeft + trackW - inset - d) : (trackLeft + inset);
}

}  // namespace showext

// fix thse isseue.md P2 (10249-10250): the Show-extensions switch is custom
// drawn and has NO visible text, so its accessible name used to be computed and
// immediately discarded. These helpers wire showext::AccessibleLabel() into a
// REAL standard tooltip on the Selected-Items list (groupInfo), where the
// switch is painted and hit-tested. The tooltip is a child-owned WS_POPUP, so
// it is destroyed automatically with the popup -- no explicit teardown needed.

// Push the current rect + state-aware label into the tooltip. Called on every
// repaint (after chipToggleRect is recomputed) and after the on/off flip, so
// the accessible name tracks resize, scroll and state. No-op until the tooltip
// exists.
static void SyncExtToggleTip(NamePromptData* d) {
    if (!d || !d->extToggleTip || !d->groupInfo) return;
    std::wstring label = showext::AccessibleLabel(d->showExt);
    TTTOOLINFOW ti = {};
    ti.cbSize = sizeof(ti);
    ti.hwnd = d->groupInfo;
    ti.uId = 1;  // the single rect-based tool registered in EnsureExtToggleTip
    ti.rect = d->chipToggleRect;
    SendMessageW(d->extToggleTip, TTM_NEWTOOLRECTW, 0,
                 reinterpret_cast<LPARAM>(&ti));
    ti.lpszText = const_cast<wchar_t*>(label.c_str());
    SendMessageW(d->extToggleTip, TTM_UPDATETIPTEXTW, 0,
                 reinterpret_cast<LPARAM>(&ti));
}

// Create the tooltip once and register a single rect-based tool over the switch
// (TTF_SUBCLASS lets it relay hover automatically from the owner-draw list).
// Guarded so a creation failure degrades gracefully (no tooltip, no crash).
static void EnsureExtToggleTip(NamePromptData* d, HWND groupInfo, HINSTANCE hinst) {
    if (!d || d->extToggleTip || !groupInfo) return;
    static bool iccDone = false;
    if (!iccDone) {
        INITCOMMONCONTROLSEX icc = { sizeof(icc), ICC_BAR_CLASSES };
        InitCommonControlsEx(&icc);  // ensure the TOOLTIPS_CLASS is registered
        iccDone = true;
    }
    HWND tip = CreateWindowExW(WS_EX_TOPMOST, TOOLTIPS_CLASSW, nullptr,
        WS_POPUP | TTS_ALWAYSTIP | TTS_NOPREFIX,
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
        groupInfo, nullptr, hinst, nullptr);
    if (!tip) {
        Wh_Log(L"EnsureExtToggleTip: tooltip creation failed; the "
               L"Show-extensions switch has no accessible tooltip this session");
        return;
    }
    d->extToggleTip = tip;
    std::wstring label = showext::AccessibleLabel(d->showExt);
    TTTOOLINFOW ti = {};
    ti.cbSize = sizeof(ti);
    ti.uFlags = TTF_SUBCLASS;  // auto-relay hover from the owner-draw list
    ti.hwnd = groupInfo;
    ti.uId = 1;
    ti.hinst = hinst;
    ti.rect = d->chipToggleRect;  // refined by SyncExtToggleTip on each paint
    ti.lpszText = const_cast<wchar_t*>(label.c_str());
    SendMessageW(tip, TTM_ADDTOOLW, 0, reinterpret_cast<LPARAM>(&ti));
}

// ---- Image-thumbnail model (fix thse isseue.md Issue 2) ----
// Byte-for-byte mirror of tests/thumb_logic.h. Pure. Single source of truth for
// which files are eligible for a thumbnail (raster images ONLY), the grid-only
// gating, the aspect-ratio-preserving fit into a grid cell and the request size.
// The live async IShellItemImageFactory extraction + cache + paint are wired to
// THESE functions.
namespace thumbs {

inline wchar_t Lower(wchar_t c) {
    return (c >= L'A' && c <= L'Z') ? static_cast<wchar_t>(c - L'A' + L'a') : c;
}

inline bool IsImageExtension(const wchar_t* ext) {
    if (!ext) return false;
    std::wstring e;
    for (const wchar_t* p = ext; *p; ++p) e.push_back(Lower(*p));
    if (!e.empty() && e[0] == L'.') e.erase(0, 1);
    if (e.empty()) return false;
    static const wchar_t* kImg[] = {
        L"jpg", L"jpeg", L"jpe", L"jfif", L"png", L"gif", L"bmp",
        L"dib", L"tif", L"tiff", L"webp", L"ico", L"heic", L"heif", nullptr};
    for (int i = 0; kImg[i]; ++i) if (e == kImg[i]) return true;
    return false;
}

inline std::wstring ExtOf(const std::wstring& name) {
    const size_t dot = name.find_last_of(L'.');
    if (dot == std::wstring::npos) return L"";
    const size_t slash = name.find_last_of(L"\\/");
    if (slash != std::wstring::npos && dot < slash) return L"";
    if (dot + 1 >= name.size()) return L"";
    return name.substr(dot + 1);
}

inline bool WantsThumbnail(bool isGridView, bool isFolder, bool isImageExt) {
    return isGridView && !isFolder && isImageExt;
}

inline bool WantsThumbnailFor(bool isGridView, bool isFolder,
                              const std::wstring& name) {
    return WantsThumbnail(isGridView, isFolder,
                          IsImageExtension(ExtOf(name).c_str()));
}

struct FitBox { int x = 0, y = 0, w = 0, h = 0; };
inline FitBox FitRect(int srcW, int srcH, int cellW, int cellH) {
    if (cellW < 0) cellW = 0;
    if (cellH < 0) cellH = 0;
    if (srcW <= 0 || srcH <= 0) return FitBox{0, 0, cellW, cellH};
    long long w = static_cast<long long>(cellH) * srcW / srcH;
    long long h;
    if (w <= cellW) {
        h = cellH;
    } else {
        w = cellW;
        h = static_cast<long long>(cellW) * srcH / srcW;
    }
    int iw = static_cast<int>(w);
    int ih = static_cast<int>(h);
    if (iw < 1) iw = 1;
    if (ih < 1) ih = 1;
    return FitBox{(cellW - iw) / 2, (cellH - ih) / 2, iw, ih};
}

inline int RequestPx(int cellW, int cellH) {
    return cellW < cellH ? cellW : cellH;
}

}  // namespace thumbs

// ---- Thumbnail worker lifecycle model (fix thse isseue.md P1) --------------
// Byte-for-byte mirror of tests/thumb_lifecycle_logic.h. Pure. These predicates
// own the worker lifecycle invariants that a static audit flagged as unsafe:
//   * initialization is published only when every handle the worker needs
//     exists (otherwise it stays retryable),
//   * thread-owned resources are freed only after CONFIRMED termination,
//   * an extracted bitmap is accepted only with valid, positive metadata.
// EnsureThumbThread / StopThumbThread / ThumbThreadProc below CONSUME these.
namespace thumblife {
inline bool ShouldMarkInitialized(bool eventCreated, bool threadCreated) {
    return eventCreated && threadCreated;
}
inline bool CanRetryInit(bool thumbInit) {
    return !thumbInit;
}
inline bool SafeToReleaseAfterWait(bool threadConfirmedExited) {
    return threadConfirmedExited;
}
inline bool IsValidThumbnailMetadata(bool getObjectOk, int w, int h) {
    return getObjectOk && w > 0 && h > 0;
}
// StopThumbThread waits for the worker to exit in BOUNDED slices -- NEVER an
// INFINITE wait, which a wedged Shell thumbnail provider could use to hang
// popup close / mod unload forever (fix thse isseue.md P1). The wait stops as
// soon as the worker confirms exit, or when the hard total budget elapses; if
// the budget elapses the caller must treat the worker as still-live and LEAK
// its resources rather than free them under it (see SafeToReleaseAfterWait).
inline int ExitWaitSliceMs()  { return 1000; }
inline int ExitWaitBudgetMs() { return 15000; }  // hard cap; never INFINITE
inline bool ShouldKeepWaiting(bool threadConfirmedExited, int elapsedMs) {
    return !threadConfirmedExited && elapsedMs < ExitWaitBudgetMs();
}
}  // namespace thumblife

// ---- Keyboard focus routing (fix thse isseue.md P1) ------------------------
// Byte-for-byte mirror of tests/focus_route_logic.h. ONE classification of the
// interactive controls so chip Left/Right AND list Up/Down/Home/End/Page keys
// share a single rule: a nav keystroke drives the chip strip / Selected list
// ONLY when no interactive control owns focus. The modal loop CONSUMES these.
namespace focusroute {
inline bool IsInteractiveFocus(bool edit, bool createBtn, bool cancelBtn,
                               bool copySegment, bool moveSegment,
                               bool destCopyBtn, bool autoGroupBtn,
                               bool infoBtn, bool sizeToggle,
                               bool minBtn, bool closeBtn) {
    return edit || createBtn || cancelBtn || copySegment || moveSegment ||
           destCopyBtn || autoGroupBtn || infoBtn || sizeToggle ||
           minBtn || closeBtn;
}
inline bool NavKeysDriveChipsOrList(bool interactiveFocus) {
    return !interactiveFocus;
}
}  // namespace focusroute

// ---- Honest partial Group-operation result accounting (fix thse isseue.md
//      P1, 13565-13620 / 14299-14320). Mirrors tests/group_logic.h
//      (namespace groupresult). PerformGroupOperation used to return a bare
//      bool and the Ctrl+G caller logged the ORIGINAL selection size on
//      success, over-reporting on every partial run. These helpers fold the
//      raw per-item tallies into an HONEST grouped count + outcome kind so the
//      live log never claims more than actually landed in the destination. ----
namespace groupresult {
struct GroupOpOutcome {
    int requested = 0;       // non-virtual, non-empty items considered
    int skipped = 0;         // dropped by the skip-existing collision policy
    int failedToQueue = 0;   // SHCreateItemFromParsingName / Move|Copy failed
    int queued = 0;          // operations actually handed to the shell batch
    bool performed = false;  // PerformOperations() returned SUCCEEDED(hr)
    bool aborted = false;    // GetAnyOperationsAborted() reported an abort
};
enum class GroupResultKind { NothingToDo, Complete, Partial, Failed };
struct GroupResultSummary {
    GroupResultKind kind = GroupResultKind::NothingToDo;
    int grouped = 0;         // honest count that actually landed
    bool groupedExact = true;// false only when aborted (grouped is upper bound)
    int requested = 0;
    int skipped = 0;
    int failed = 0;          // == failedToQueue
};
inline bool GroupOutcomeConsistent(const GroupOpOutcome& o) {
    return o.requested == o.queued + o.skipped + o.failedToQueue;
}
inline GroupResultSummary ClassifyGroupResult(const GroupOpOutcome& o) {
    GroupResultSummary s;
    s.requested = o.requested;
    s.skipped = o.skipped;
    s.failed = o.failedToQueue;
    if (o.requested <= 0) {
        s.kind = GroupResultKind::NothingToDo; s.grouped = 0; s.groupedExact = true;
        return s;
    }
    if (!o.performed || o.queued <= 0) {
        s.kind = GroupResultKind::Failed; s.grouped = 0; s.groupedExact = true;
        return s;
    }
    if (o.aborted) {
        s.kind = GroupResultKind::Partial; s.grouped = o.queued; s.groupedExact = false;
        return s;
    }
    s.grouped = o.queued; s.groupedExact = true;
    const bool everything =
        (o.queued == o.requested && o.skipped == 0 && o.failedToQueue == 0);
    s.kind = everything ? GroupResultKind::Complete : GroupResultKind::Partial;
    return s;
}

// Auto Group multi-bucket aggregation (fix thse isseue.md P1). Auto Group runs
// ClassifyGroupResult once PER destination bucket (and once for the All Folder
// move). Folding every per-bucket summary yields an HONEST overall count and a
// single completion verdict, so the live log can never claim "completed" while
// a bucket skipped / failed / aborted, and filesRouted / foldersMoved come from
// `grouped` -- never the requested size. Mirror of tests/group_logic.h.
struct GroupAggregate {
    int grouped = 0;          // honest items that actually landed across all buckets
    int requested = 0;        // items considered across all buckets
    int skipped = 0;          // skip-existing collisions across all buckets
    int failed = 0;           // failed-to-queue across all buckets
    int buckets = 0;          // ClassifyGroupResult summaries folded in
    bool allComplete = true;  // false as soon as one bucket is not Complete
    bool exact = true;        // false if any bucket aborted (grouped is an upper bound)
};
inline void FoldGroupResult(GroupAggregate& agg, const GroupResultSummary& s) {
    agg.grouped += s.grouped;
    agg.requested += s.requested;
    agg.skipped += s.skipped;
    agg.failed += s.failed;
    ++agg.buckets;
    if (s.kind != GroupResultKind::Complete) agg.allComplete = false;
    if (!s.groupedExact) agg.exact = false;
}
// A run is fully complete ONLY when at least one bucket ran, every folded bucket
// was Complete, and no external step failed. `externalFailure` folds in the
// non-ClassifyGroupResult failures the executor tracks (a bucket directory that
// could not be created, a blocked All Folder, etc.).
inline bool AggregateComplete(const GroupAggregate& agg, bool externalFailure) {
    return !externalFailure && agg.buckets > 0 && agg.allComplete;
}
}  // namespace groupresult

// ---- fix thse isseue.md item 6 failure-handling mirrors. Byte-for-byte with
//      tests/failguard_logic.h so a unit test pins each decision while the mod
//      uses it at the un-compilable Win32 call sites. (namespace layoutdefer is
//      defined earlier, just above ApplyPromptLayout, because that function --
//      which appears before this block -- consumes it.) ----
namespace helpclass {
inline unsigned long ClassAlreadyExistsCode() { return 1410UL; }  // ERROR_CLASS_ALREADY_EXISTS
inline bool RegistrationUsable(bool registered, unsigned long lastErr) {
    return registered || lastErr == ClassAlreadyExistsCode();
}
}  // namespace helpclass
namespace gdiguard {
inline bool ShouldDrawGlyph(bool fontCreated, bool selectOk) { return fontCreated && selectOk; }
inline bool ShouldRestoreOldFont(bool selectOk) { return selectOk; }
inline bool MustDeleteCreatedFont(bool fontCreated) { return fontCreated; }
}  // namespace gdiguard

// ---- fix thse isseue.md item 7 performance mirrors. Byte-for-byte with
//      tests/perf_logic.h. autogroupbucket:: builds the Auto Group file->bucket
//      assignment in ONE pass; thumbcache:: dedups requests and bounds the
//      async thumbnail cache. ----
namespace autogroupbucket {
inline std::wstring NormalizeDirKey(const std::wstring& dir) {
    std::wstring k;
    k.reserve(dir.size());
    for (wchar_t c : dir) {
        if (c >= L'A' && c <= L'Z') c = static_cast<wchar_t>(c - L'A' + L'a');
        else if (c == L'/')         c = L'\\';
        k.push_back(c);
    }
    return k;
}
}  // namespace autogroupbucket
namespace thumbcache {
inline int MaxEntries() { return 128; }
inline bool ShouldEvict(int cacheSize, int maxEntries) { return cacheSize > maxEntries; }
inline bool ShouldEnqueue(bool keyAlreadyPresent) { return !keyAlreadyPresent; }
}  // namespace thumbcache

// ---- Remembered Group popup placement model (fix thse isseue.md Issue 6) ----
// Byte-for-byte mirror of tests/winplace_logic.h. Pure. Clamps a remembered
// window rect onto the current monitor work area (shrink + nudge; nearest-
// monitor fallback is the caller passing that monitor's work rect), tests
// visibility, and owns the "capture NORMAL only when not maximized" rule.
namespace winplace {

struct Rect {
    int left = 0, top = 0, right = 0, bottom = 0;
    int w() const { return right - left; }
    int h() const { return bottom - top; }
};

inline Rect ClampToWork(Rect r, Rect work) {
    int w = r.w();
    int h = r.h();
    const int ww = work.w();
    const int wh = work.h();
    if (w > ww) w = ww;
    if (h > wh) h = wh;
    if (w < 1) w = 1;
    if (h < 1) h = 1;
    int x = r.left;
    int y = r.top;
    if (x + w > work.right)  x = work.right - w;
    if (y + h > work.bottom) y = work.bottom - h;
    if (x < work.left) x = work.left;
    if (y < work.top)  y = work.top;
    return Rect{x, y, x + w, y + h};
}

inline bool IntersectsWork(Rect r, Rect work) {
    return r.left < work.right && r.right > work.left &&
           r.top < work.bottom && r.bottom > work.top;
}

inline bool ShouldCaptureNormal(bool isZoomed) { return !isZoomed; }

}  // namespace winplace

// ---- session-remembered Group popup placement (Issue 6) --------------------
// Lifetime = the mod's lifetime: it survives repeated Ctrl+G opens. A Windhawk /
// Explorer restart clears it -- the mod has no writable settings store to persist
// across restarts, and the spec forbids adding a settings UI just for this, so
// cross-restart persistence is intentionally out of scope.
static volatile LONG g_groupPlaceInit   = 0;             // any user placement remembered this session
static volatile LONG g_groupMaximized   = 0;             // last user state was maximized
static volatile LONG g_groupNormalValid = 0;             // g_groupNormalRect holds a real NORMAL rect
static RECT          g_groupNormalRect  = {0, 0, 0, 0};  // last NORMAL window rect (screen px)

// Capture the CURRENT Group popup placement for the next open. The NORMAL rect
// is stored ONLY when the window is not maximized (winplace::ShouldCaptureNormal)
// so a maximize never overwrites the restore size; the maximize STATE is stored
// separately. New Folder is never a Group popup, so it is never remembered.
static void RememberGroupPlacement(HWND hwnd, NamePromptData* d) {
    if (!d || !d->isGroup) return;
    const bool zoomed = IsZoomed(hwnd) != FALSE;
    InterlockedExchange(&g_groupMaximized, zoomed ? 1 : 0);
    if (winplace::ShouldCaptureNormal(zoomed)) {
        RECT r{};
        if (GetWindowRect(hwnd, &r) && (r.right - r.left) > 0 &&
            (r.bottom - r.top) > 0) {
            g_groupNormalRect = r;
            InterlockedExchange(&g_groupNormalValid, 1);
        }
    }
    InterlockedExchange(&g_groupPlaceInit, 1);
}

// Restore the remembered NORMAL rect onto the nearest monitor's work area before
// the popup is shown (clamped so it is always visible). Returns true if a normal
// placement was applied (the caller then latches manualSized and skips the
// default centering). Maximize is applied by the caller AFTER ShowWindow.
static bool ApplyRememberedNormalPlacement(HWND hwnd, NamePromptData* d) {
    if (!d || !d->isGroup) return false;
    if (!InterlockedCompareExchange(&g_groupPlaceInit, 0, 0)) return false;
    if (!InterlockedCompareExchange(&g_groupNormalValid, 0, 0)) return false;
    HMONITOR mon = MonitorFromRect(&g_groupNormalRect, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi; mi.cbSize = sizeof(mi);
    if (!GetMonitorInfoW(mon, &mi)) return false;
    const winplace::Rect saved{g_groupNormalRect.left, g_groupNormalRect.top,
                               g_groupNormalRect.right, g_groupNormalRect.bottom};
    const winplace::Rect work{mi.rcWork.left, mi.rcWork.top,
                              mi.rcWork.right, mi.rcWork.bottom};
    const winplace::Rect c = winplace::ClampToWork(saved, work);
    SetWindowPos(hwnd, nullptr, c.left, c.top, c.right - c.left, c.bottom - c.top,
                 SWP_NOZORDER | SWP_NOACTIVATE);
    d->manualSized = true;   // reflow to the restored size, not the compact preset
    return true;
}

// True when the remembered placement for this session was maximized.
static bool GroupPlacementMaximized() {
    return InterlockedCompareExchange(&g_groupMaximized, 0, 0) != 0 &&
           InterlockedCompareExchange(&g_groupPlaceInit, 0, 0) != 0;
}

// Byte-for-byte mirror of tests/chip_scroll_logic.h.
namespace chipscroll {

// Largest horizontal offset: how far the content extends past the viewport.
inline int MaxOffset(int contentW, int viewportW) {
    const int m = contentW - viewportW;
    return m > 0 ? m : 0;
}

// Scrolling is only possible/needed when the chips overflow the viewport.
inline bool NeedsScroll(int contentW, int viewportW) {
    return contentW > viewportW;
}

// Keep an offset within [0, maxOffset].
inline int Clamp(int offset, int maxOffset) {
    if (offset < 0) return 0;
    if (offset > maxOffset) return maxOffset;
    return offset;
}

// Mouse wheel -> horizontal pan. Positive wheelDelta (wheel away from the user)
// pans toward the start (offset 0), matching the vertical list convention.
inline int OnWheel(int offset, int wheelDelta, int maxOffset, int pxPerNotch) {
    const int notches = wheelDelta / 120;
    offset -= notches * pxPerNotch;
    return Clamp(offset, maxOffset);
}

// WM_MOUSEHWHEEL / tilt wheels use the OPPOSITE sign of WM_MOUSEWHEEL (positive
// means pan right). Normalize a horizontal-wheel delta to the OnWheel sign.
inline int WheelDeltaFromHorizontal(int hwheelDelta) { return -hwheelDelta; }

// Smallest offset change that brings [chipLeft, chipRight) fully into view.
inline int BringIntoView(int chipLeft, int chipRight, int offset, int viewportW) {
    if (chipLeft < offset) return chipLeft;
    if (chipRight > offset + viewportW) return chipRight - viewportW;
    return offset;
}

// Keyboard navigation across the chips; returns the clamped active index.
enum class Key { Left, Right, Home, End };
inline int OnKey(Key k, int activeIndex, int chipCount) {
    if (chipCount <= 0) return 0;
    switch (k) {
        case Key::Left:  activeIndex -= 1; break;
        case Key::Right: activeIndex += 1; break;
        case Key::Home:  activeIndex = 0; break;
        case Key::End:   activeIndex = chipCount - 1; break;
    }
    if (activeIndex < 0) return 0;
    if (activeIndex > chipCount - 1) return chipCount - 1;
    return activeIndex;
}

}  // namespace chipscroll

// Byte-for-byte mirror of tests/chip_toolbar_logic.h.
namespace chiptoolbar {

struct Box {
    int left = 0, top = 0, right = 0, bottom = 0;
    int w() const { return right - left; }
    int h() const { return bottom - top; }
};

struct Layout {
    bool wrapped = false;   // true -> toolbar moved to a second row
    int  rows = 1;
    Box  strip;             // chip-strip viewport
    Box  toolbar;           // region holding the action buttons
};

// Lay out the summary row in [left,top,right,bottom]. toolbarW is the total
// width the toolbar buttons need (including their internal gaps), gap is the
// space between the strip and the toolbar, minStripW is the smallest usable
// chip-strip width, and rowH is the height of one summary row. The strip takes
// the remaining width on a single row when it stays >= minStripW; otherwise the
// toolbar wraps below the strip.
inline Layout Compute(int left, int top, int right, int bottom,
                      int toolbarW, int gap, int minStripW, int rowH) {
    Layout out;
    const int singleStripW = (right - left) - toolbarW - gap;
    if (singleStripW >= minStripW) {
        out.wrapped = false;
        out.rows = 1;
        out.strip = Box{left, top, left + singleStripW, bottom};
        out.toolbar = Box{left + singleStripW + gap, top, right, bottom};
    } else {
        out.wrapped = true;
        out.rows = 2;
        out.strip = Box{left, top, right, top + rowH};
        out.toolbar = Box{left, top + rowH, right, top + rowH + rowH};
    }
    return out;
}

// Place buttons left-to-right inside the toolbar box in order (Show extensions
// first, future actions after). Returns one box per width.
inline std::vector<Box> ButtonBoxes(const Box& toolbar,
                                    const std::vector<int>& widths, int gap) {
    std::vector<Box> out;
    int x = toolbar.left;
    for (std::size_t i = 0; i < widths.size(); ++i) {
        out.push_back(Box{x, toolbar.top, x + widths[i], toolbar.bottom});
        x += widths[i] + gap;
    }
    return out;
}

}  // namespace chiptoolbar

// Byte-for-byte mirror of tests/chip_filter_logic.h.
namespace chipfilter {

enum class Row { Normal, Matched, Subdued };

// Which chip is the single active filter. fix thse isseue.md #2: besides the
// per-extension chips, the Items chip restores the all-items view, the Folders
// chip highlights folder rows, and the No-extension chip highlights
// extensionless files. At most ONE is active at a time.
enum class Kind { None, Items, Folders, NoExt, Ext };

struct State {
    std::wstring activeKey;   // normalized lowercase ext key when kind==Ext; else empty
    Kind kind = Kind::None;
    // "Active" means a highlight is applied: None and the Items (all-items
    // view) chip are NOT highlighting states.
    bool active() const { return kind != Kind::None && kind != Kind::Items; }
};

// The Items chip: restore the all-items view (no row is subdued).
inline void ActivateItems(State& s) { s.kind = Kind::Items; s.activeKey.clear(); }

// The Folders chip: highlight the selected folder rows.
inline void ActivateFolders(State& s) { s.kind = Kind::Folders; s.activeKey.clear(); }

// The No-extension chip: highlight files that have no extension.
inline void ActivateNoExt(State& s) { s.kind = Kind::NoExt; s.activeKey.clear(); }

// Click an extension chip. A non-empty key becomes the single active ext
// filter, replacing any previous one; clicking the active key again leaves it
// active. An empty key is not an extension filter and does nothing.
inline void ActivateExt(State& s, const std::wstring& key) {
    if (key.empty()) return;
    s.kind = Kind::Ext;
    s.activeKey = key;
}

// Backward-compatible shorthand: Activate(key) == ActivateExt(key).
inline void Activate(State& s, const std::wstring& key) { ActivateExt(s, key); }

// The explicit reset: a deliberate click outside the chip strip.
inline void ClickOutside(State& s) {
    s.kind = Kind::None;
    s.activeKey.clear();
}

// Per-row highlight state. Normal for every row when no highlight is active
// (None / Items); otherwise the rows that match the active chip are Matched and
// every other row is Subdued (never reordered or removed).
inline Row RowFor(const State& s, const std::wstring& rowExtKey, bool isFolder) {
    switch (s.kind) {
        case Kind::None:
        case Kind::Items:   return Row::Normal;
        case Kind::Folders: return isFolder ? Row::Matched : Row::Subdued;
        case Kind::NoExt:
            return (!isFolder && rowExtKey.empty()) ? Row::Matched : Row::Subdued;
        case Kind::Ext:
            return (!isFolder && rowExtKey == s.activeKey) ? Row::Matched
                                                           : Row::Subdued;
    }
    return Row::Normal;
}

// fix thse isseue.md Phase 2A: the ACTIVE chip is indicated ONLY by a dotted
// outline (the existing focus/selection styling), never by a second accent fill
// overlay -- the chip keeps its muted/per-ext fill in every state. A chip draws
// the dotted outline when it is the active filter OR the keyboard-focused chip.
// Keyboard focus and active-filter state stay logically distinct (two inputs)
// even though they share the one compatible dotted-outline style.
inline bool ChipShowsDottedOutline(bool isActiveFilter, bool isFocused) {
    return isActiveFilter || isFocused;
}

}  // namespace chipfilter

// fix thse isseue.md Phase 5 + ADD-ON: PURE result-navigation model for the
// summary chips -- byte-for-byte mirror of tests/chip_nav_logic.h (namespace
// chipnav). A summary chip is BOTH a filter and a find-navigator: the first
// click on an extension chip reveals the first matching item and clicking the
// already-active chip again reveals the next match, wrapping at the end. It
// does NOT duplicate the match rule (delegates to chipfilter::RowFor) or the
// item<->row mapping (viewmode::TopRowForItem), so List and the two grid modes
// stay consistent with the live renderer. The navigation CURSOR is a STABLE
// ITEM INDEX kept SEPARATELY from the active filter, so it survives a view-mode
// change. Nothing here reorders items, touches the real Explorer selection or
// changes the Copy/Move plan.
namespace chipnav {

// One selected row as the navigator sees it: just enough to run the shared
// chipfilter match rule. `extKey` is the normalized lowercase extension key
// ("" for folders and extensionless files); `isFolder` marks a directory.
struct Item {
    std::wstring extKey;
    bool isFolder;
};

// The ordered list of item indices that MATCH the active chip filter, in the
// ORIGINAL item order (never reordered). Delegates to chipfilter::RowFor so the
// find-navigator and the row highlighter always agree on what "matches". When
// no highlight filter is active (None / Items) nothing matches, so the
// navigator stays idle for those chips.
inline std::vector<int> Matches(const chipfilter::State& s,
                                const std::vector<Item>& items) {
    std::vector<int> out;
    for (int i = 0; i < static_cast<int>(items.size()); ++i) {
        if (chipfilter::RowFor(s, items[i].extKey, items[i].isFolder)
            == chipfilter::Row::Matched) {
            out.push_back(i);
        }
    }
    return out;
}

// The next current-match item index.
//   matches : ordered matching item indices (from Matches()).
//   curItem : the current match's item index, or -1 when there is none.
//   reclick : true ONLY when the SAME already-active chip was clicked again.
// Rules:
//   - no matches                      -> -1 (empty results never jump).
//   - fresh activation (reclick=false)-> the FIRST match.
//   - reclick with a live cursor      -> the NEXT match, wrapping at the end.
//   - reclick with a stale cursor     -> the FIRST match.
inline int NextMatch(const std::vector<int>& matches, int curItem, bool reclick) {
    if (matches.empty()) return -1;
    if (!reclick) return matches.front();
    int pos = -1;
    for (int i = 0; i < static_cast<int>(matches.size()); ++i) {
        if (matches[i] == curItem) { pos = i; break; }
    }
    if (pos < 0) return matches.front();
    return matches[(pos + 1) % static_cast<int>(matches.size())];
}

// The MINIMAL new top row that reveals the item at `itemIndex`, given the
// current scroll (`topRow`) and how many whole rows are visible (`visibleRows`).
// The item index is mapped to its VISUAL row with viewmode::TopRowForItem using
// the ACTUAL column count, so List (cols==1) and the grid modes are handled by
// one path. If the target row is already visible the scroll is unchanged (so
// adjacent matches do not make the viewport jump); otherwise it scrolls just
// far enough -- up to the target, or down so the target becomes the last
// visible row -- then clamps to the valid range. itemIndex < 0 (empty results)
// leaves the scroll untouched.
inline int RevealTopRow(viewmode::Mode mode, int itemIndex, int itemCount,
                        int cols, int topRow, int visibleRows) {
    if (itemIndex < 0) return topRow;
    if (visibleRows < 1) visibleRows = 1;
    if (cols < 1) cols = 1;

    const int target = viewmode::TopRowForItem(mode, itemIndex, cols);
    const int totalRows = (mode == viewmode::Mode::List)
                              ? itemCount
                              : viewmode::GridRows(itemCount, cols);
    int maxTop = totalRows - visibleRows;
    if (maxTop < 0) maxTop = 0;

    int newTop = topRow;
    if (target < topRow) {
        newTop = target;                          // scroll up to the match
    } else if (target > topRow + visibleRows - 1) {
        newTop = target - visibleRows + 1;        // scroll down just enough
    }                                             // else: already visible

    if (newTop < 0) newTop = 0;
    if (newTop > maxTop) newTop = maxTop;
    return newTop;
}

// Is the item at `itemIndex` the CURRENT match? This is the extra visual state
// layered on top of chipfilter::Row::Matched: every matching row stays Matched
// (normal strength), and additionally the one current-match row gets a subtle
// accent so the user can see where they are without losing the other matches.
// A cursor of -1 means nothing is current.
inline bool IsCurrentMatch(int itemIndex, int curItem) {
    return curItem >= 0 && itemIndex == curItem;
}

}  // namespace chipnav

// fix thse isseue.md #2: bridge the PRIMITIVE chip-filter state stored on
// NamePromptData (defined earlier in this single-pass TU, so it cannot name the
// chipfilter:: types) to/from the tested chipfilter::State model. The model
// stays the single source of truth for the highlight decision.
static chipfilter::State ChipStateFrom(const NamePromptData* d) {
    chipfilter::State s;
    s.kind = static_cast<chipfilter::Kind>(d->chipFilterKind);
    s.activeKey = d->chipFilterKey;
    return s;
}
static void ChipStateInto(NamePromptData* d, const chipfilter::State& s) {
    d->chipFilterKind = static_cast<int>(s.kind);
    d->chipFilterKey = s.activeKey;
}

// ---- filesystem size resolver (fix thse isseue.md #1 / P2) ----
// Resolve a file's size for the Selected Items badge WITHOUT opening or reading
// its contents -- a metadata-only query. Prefers GetCompressedFileSizeW, which
// returns the bytes the file ACTUALLY occupies (NTFS compression and sparse
// ranges are reflected), and falls back to the logical length
// (GetFileAttributesExW) when that query is unavailable. NOTE (fix thse
// isseue.md P2): this is an APPROXIMATION of on-disk usage, NOT Explorer's
// cluster-rounded "Size on disk" -- it is not rounded up to the volume's
// allocation unit, so the badge shows a plain byte size and must not be
// presented to the user as a precise "Size on disk" figure. Folders, reparse
// points, empty paths and any metadata failure resolve to -1 ("unknown") so the
// row simply shows no size badge; it never throws or blocks the UI. Called only
// when the selection/model changes (capture), never from a paint handler.
static long long ResolveFileSizeOnDisk(const std::wstring& path) {
    if (path.empty()) return -1;

    long long logical = -1;
    WIN32_FILE_ATTRIBUTE_DATA fad = {};
    if (GetFileAttributesExW(path.c_str(), GetFileExInfoStandard, &fad)) {
        if (fad.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            return -1;  // folders never show a size badge
        }
        logical = (static_cast<long long>(fad.nFileSizeHigh) << 32) |
                  static_cast<unsigned long>(fad.nFileSizeLow);
        // Reparse points (symlinks / junctions) can target slow or absent
        // locations; keep the logical length and skip the allocation query.
        if (fad.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT) {
            return logical;
        }
    }

    long long allocated = -1;
    DWORD hi = 0;
    const DWORD lo = GetCompressedFileSizeW(path.c_str(), &hi);
    if (lo != INVALID_FILE_SIZE || GetLastError() == NO_ERROR) {
        allocated = (static_cast<long long>(hi) << 32) |
                    static_cast<unsigned long>(lo);
    }

    return sel::ChooseSizeBytes(logical, allocated);
}

// ---- real Windows Shell icon layer for file/folder ROWS (fix #9) ----
// Byte-for-byte mirror of tests/shell_icon_logic.h. The Selected Items rows
// render the SAME icon File Explorer shows (PDF/JPG/MP3/DOCX/folder) via
// SHGetFileInfoW with SHGFI_USEFILEATTRIBUTES -- a type/extension lookup that
// never touches disk -- instead of a generic custom vector. The custom vector
// layer below stays for the UI-SEMANTIC slots only.
namespace shellicon {
inline constexpr unsigned kSHGFI_ICON               = 0x000000100u;  // SHGFI_ICON
inline constexpr unsigned kSHGFI_SMALLICON          = 0x000000001u;  // SHGFI_SMALLICON
inline constexpr unsigned kSHGFI_USEFILEATTRIBUTES  = 0x000000010u;  // SHGFI_USEFILEATTRIBUTES
inline constexpr unsigned kFILE_ATTRIBUTE_DIRECTORY = 0x00000010u;   // FILE_ATTRIBUTE_DIRECTORY
inline constexpr unsigned kFILE_ATTRIBUTE_NORMAL    = 0x00000080u;   // FILE_ATTRIBUTE_NORMAL

inline bool RowUsesShellIcon(bool /*isFolder*/) { return true; }

inline unsigned IconFlags() {
    return kSHGFI_ICON | kSHGFI_SMALLICON | kSHGFI_USEFILEATTRIBUTES;
}
inline unsigned AttrFor(bool isFolder) {
    return isFolder ? kFILE_ATTRIBUTE_DIRECTORY : kFILE_ATTRIBUTE_NORMAL;
}
inline int IconPx(unsigned dpi) { return static_cast<int>((16u * dpi + 48u) / 96u); }

inline std::wstring IconKey(const std::wstring& name, bool isFolder) {
    if (isFolder) return L"<dir>";
    const size_t dot = name.find_last_of(L'.');
    if (dot == std::wstring::npos || dot + 1 >= name.size()) return L"<noext>";
    std::wstring ext = name.substr(dot);
    for (wchar_t& c : ext)
        if (c >= L'A' && c <= L'Z') c = static_cast<wchar_t>(c - L'A' + L'a');
    return ext;
}
}  // namespace shellicon

// ---- DPI-aware Shell icon SOURCE selection (fix thse isseue.md Phase 1) ----
// Byte-for-byte mirror of tests/icon_source_logic.h. The Selected Items rows
// draw each Shell icon from a system image-list TIER whose physical resolution
// is >= the painter's draw size, so Large / Extra large icons are DOWNSAMPLED
// from a crisp high-resolution source (SHIL_EXTRALARGE 48 / SHIL_JUMBO 256)
// rather than UPSCALED from a 16px small icon. The extracted HICONs are cached
// per (type, tier, dpi) so scrolling and zooming reuse one source. The real
// SHGetFileInfoW(SHGFI_SYSICONINDEX) -> SHGetImageList(SHIL_*) ->
// IImageList::GetIcon extraction + cache are below in the paint layer.
namespace iconsrc {

enum class Tier { Small = 1, ExtraLarge = 2, Jumbo = 4 };

inline int Shil(Tier t) { return static_cast<int>(t); }

inline int TierBasePx(Tier t) {
    switch (t) {
        case Tier::Small:      return 16;
        case Tier::ExtraLarge: return 48;
        case Tier::Jumbo:      return 256;
    }
    return 16;
}

inline int ScalePx(int basePx, unsigned dpi) {
    return static_cast<int>((static_cast<unsigned>(basePx) * dpi + 48u) / 96u);
}

inline int TierPx(Tier t, unsigned dpi) {
    if (t == Tier::Jumbo) return 256;
    return ScalePx(TierBasePx(t), dpi);
}

inline int DrawPx(viewmode::Mode m, unsigned dpi) {
    return ScalePx(viewmode::IconPx(m), dpi);
}

inline Tier TierForMode(viewmode::Mode m) {
    switch (m) {
        case viewmode::Mode::List:       return Tier::Small;
        case viewmode::Mode::Large:      return Tier::ExtraLarge;
        case viewmode::Mode::ExtraLarge: return Tier::Jumbo;
    }
    return Tier::Small;
}

inline Tier SelectTier(viewmode::Mode m, unsigned dpi) {
    const int need = DrawPx(m, dpi);
    if (TierPx(Tier::Small, dpi) >= need)      return Tier::Small;
    if (TierPx(Tier::ExtraLarge, dpi) >= need) return Tier::ExtraLarge;
    return Tier::Jumbo;
}

inline bool SourceCoversDraw(viewmode::Mode m, unsigned dpi) {
    return TierPx(SelectTier(m, dpi), dpi) >= DrawPx(m, dpi);
}

inline std::wstring CacheKey(const std::wstring& typeKey, Tier t, unsigned dpi) {
    return typeKey + L"|t" + std::to_wstring(static_cast<int>(t)) +
           L"|d" + std::to_wstring(dpi);
}

}  // namespace iconsrc

// ---- window maximize/restore + resize-frame model (fix thse isseue.md Phase 3) ----
// Byte-for-byte mirror of tests/win_resize_logic.h. The Group popup's title-bar
// square button is a real Maximize/Restore driven by the ACTUAL maximized state
// (IsZoomed), the borderless popup grows a resize frame (WS_THICKFRAME +
// WM_NCHITTEST), maximizing occupies the monitor WORK AREA, and Ctrl+E density
// stays independent of maximize. The Ht values equal the Win32 HT* codes so the
// WM_NCHITTEST handler returns them directly.
namespace winresize {

inline int MinClientW() { return 360; }
inline int MinClientH() { return 320; }

inline int ClampW(int w, int minW) { return w < minW ? minW : w; }
inline int ClampH(int h, int minH) { return h < minH ? minH : h; }

enum Ht {
    Client = 1, Left = 10, Right = 11, Top = 12, TopLeft = 13,
    TopRight = 14, Bottom = 15, BottomLeft = 16, BottomRight = 17
};

inline Ht HitTest(int x, int y, int clientW, int clientH, int borderPx) {
    const bool l = x >= 0 && x < borderPx;
    const bool r = x < clientW && x >= clientW - borderPx;
    const bool t = y >= 0 && y < borderPx;
    const bool b = y < clientH && y >= clientH - borderPx;
    if (t && l) return TopLeft;
    if (t && r) return TopRight;
    if (b && l) return BottomLeft;
    if (b && r) return BottomRight;
    if (l) return Left;
    if (r) return Right;
    if (t) return Top;
    if (b) return Bottom;
    return Client;
}

inline bool ShowRestoreGlyph(bool isZoomed) { return isZoomed; }

struct Rect { int left, top, right, bottom; };
inline int Width(const Rect& r)  { return r.right - r.left; }
inline int Height(const Rect& r) { return r.bottom - r.top; }
inline bool Valid(const Rect& r) { return r.right > r.left && r.bottom > r.top; }

inline Rect MaximizeTo(const Rect& workArea) { return workArea; }

inline Rect RestoreTo(const Rect& prev, const Rect& fallback) {
    return Valid(prev) ? prev : fallback;
}

// WM_GETMINMAXINFO geometry (byte-for-byte mirror of tests/win_resize_logic.h).
// A borderless WS_POPUP maximized with the DEFAULT MINMAXINFO covers the ENTIRE
// monitor (over the taskbar) and can be drag-sized into an unusable sliver.
// ComputeMinMax derives the fields the live WM_GETMINMAXINFO handler writes into
// MINMAXINFO so maximize fills the monitor WORK AREA (taskbar respected, never
// exclusive full screen) and a resize can never shrink below the minimum.
// ptMaxPosition is RELATIVE to the monitor origin (so a secondary monitor at
// negative virtual coords maximizes correctly). minTrack{W,H} are the minimum
// OUTER (device-px) window dimensions supplied by the caller.
struct SizeI  { int cx; int cy; };
struct MinMax { SizeI maxPos; SizeI maxSize; SizeI minTrack; };

inline MinMax ComputeMinMax(const Rect& monitor, const Rect& work,
                            int minTrackW, int minTrackH) {
    MinMax m;
    m.maxPos.cx   = work.left - monitor.left;   // -> MINMAXINFO.ptMaxPosition.x
    m.maxPos.cy   = work.top  - monitor.top;    // -> MINMAXINFO.ptMaxPosition.y
    m.maxSize.cx  = Width(work);                 // -> MINMAXINFO.ptMaxSize.x
    m.maxSize.cy  = Height(work);                // -> MINMAXINFO.ptMaxSize.y
    m.minTrack.cx = minTrackW;                   // -> MINMAXINFO.ptMinTrackSize.x
    m.minTrack.cy = minTrackH;                   // -> MINMAXINFO.ptMinTrackSize.y
    return m;
}

inline bool DensityMayResizeWindow(bool isZoomed) { return !isZoomed; }

}  // namespace winresize

// fix thse isseue.md v0.12.65 Bug 1: ONE authoritative "effective layout" for the
// Group popup. For a normal (not resized) popup it returns the PREFERRED compact/
// expanded layout; once the user has maximized or drag-resized it (manualSized
// latched, or IsZoomed) it returns the live REFLOWED layout -- byte-for-byte the
// SAME layout::ReflowToClient transform (and the same MulDiv device->design
// scaling) the WM_SIZE handler feeds into ApplyPromptLayout. The card paint
// (WM_ERASEBKGND), the title-bar hit-test (WM_NCHITTEST) and the child-move
// (ApplyPromptLayout) all read from THIS single result, so the painted card
// boundaries can never drift out of sync with the controls inside them. New
// Folder is never a Group popup and is never manualSized/zoomed, so it always
// gets its original untouched geometry; initial preferred sizing is unchanged.
//
// fix thse isseue.md P0 (initial-layout lifecycle): the grid-cell "effective
// preferred" sizing used to live INLINE here, so only the resize path (WM_SIZE)
// ran it; the first visible frame used the raw ComputePromptLayout preset and
// disagreed. It is now the shared, unit-tested efflayout:: model below (byte-
// for-byte mirror of tests/effective_layout_logic.h), so the first frame
// (initial outer size + WM_CREATE) and this path use ONE calculation.
namespace efflayout {

struct ReflowMetrics {
    int selRowH;     // row height handed to layout::ReflowToClient
    int selCap;      // whole-cell cap (-1 = ReflowToClient's own cap)
    int rowsNeeded;  // true content rows the reflow must know about
};

inline ReflowMetrics GridSizedPreferred(layout::PromptLayout& base, bool isGrid,
                                        int itemCount, int cellH, int visRows,
                                        int rowsNeeded) {
    ReflowMetrics m{ layout::kSelRowH, -1, itemCount };
    if (!isGrid) return m;
    const int headerBlockH = base.selectedList.y - base.selectedContent.y;
    const int listBudget   = base.selectedContent.h - headerBlockH;
    int fit = (cellH > 0) ? listBudget / cellH : 0;  // whole cells in the budget
    if (fit < 1) fit = 1;
    const int newContentH = headerBlockH + visRows * cellH;
    const int delta = newContentH - base.selectedContent.h;
    if (delta != 0) {
        base.selectedContent.h += delta;
        base.selectedList.h    += delta;
        base.selectedCard.h    += delta;
        base.groupHeader.h     += delta;
        const auto shiftY = [delta](layout::Rect& r) {
            if (r.w > 0) r.y += delta;
        };
        shiftY(base.destCard);     shiftY(base.destLabel);
        shiftY(base.destDesc);     shiftY(base.destField);
        shiftY(base.destCopyBtn);
        shiftY(base.nameCard);     shiftY(base.nameLabel);
        shiftY(base.nameDesc);     shiftY(base.nameField);
        shiftY(base.actionCard);   shiftY(base.actionTitle);
        shiftY(base.actionDesc);
        shiftY(base.segCopy);      shiftY(base.segMove);
        shiftY(base.opToggle);     shiftY(base.keyboardHint);
        shiftY(base.createButton); shiftY(base.cancelButton);
        shiftY(base.autoGroupButton); shiftY(base.infoButton);
        base.windowH += delta;
    }
    m.selRowH    = cellH;
    m.selCap     = fit;
    m.rowsNeeded = rowsNeeded;
    return m;
}

}  // namespace efflayout

static layout::PromptLayout ComputeEffectiveLayout(HWND hwnd, const NamePromptData* d) {
    layout::PromptLayout base = layout::ComputePromptLayout(
        d->isGroup, d->expanded, static_cast<int>(d->items.size()));
    if (!d->isGroup) return base;
    // fix thse isseue.md v0.12.67 (Issue #1 B) + Phase 2: make the layout
    // MODE-AWARE in EVERY window state (preferred, remembered-size, manually
    // resized and maximized). ComputePromptLayout reserves the Selected list at
    // the LIST row height (kSelRowH=24); in a GRID view the rows are much taller
    // WHOLE cells, so a list-sized card clips the cells and crams the grid.
    // FIRST re-size the Selected card to the current mode's whole grid cells --
    // bounded by the SAME vertical budget the list used -- then shift every
    // section below it by the same delta. This runs UNCONDITIONALLY (not only on
    // the preferred path) so the manual-resize / maximize reflow BELOW starts
    // from the SAME grid geometry, and it also hands that reflow the mode's cell
    // height + whole-cell cap so the resize path never falls back to a 24px List
    // row (the Phase 2 defect: clipped grid cells on resize/maximize). The
    // painted card, the owner-draw list HWND, WM_NCHITTEST and the scroll math
    // all read this one geometry. List mode is unchanged (delta 0, List defaults
    // into ReflowToClient).
    int selRowHForReflow = layout::kSelRowH;   // List default (24px row)
    int selCapForReflow  = -1;                 // List: use ReflowToClient's own cap
    // fix thse isseue.md P1 "exact fit treated as capped": hand the reflow the
    // TRUE number of rows the content needs so it grows the card only on real
    // overflow. List is one column -> one row per item.
    int rowsNeededForReflow = static_cast<int>(d->items.size());
    if (viewmode::IsGrid(d->selViewMode)) {
        const int gutter =
            listscroll::kScrollbarW + listscroll::kMetaScrollGap;
        const int availW   = base.selectedContent.w - gutter;
        const int cols     = viewmode::Columns(d->selViewMode, availW);
        const int itemCount = static_cast<int>(d->items.size());
        const int headerBlockH =
            base.selectedList.y - base.selectedContent.y;
        const int listBudget = base.selectedContent.h - headerBlockH;
        const int cellH    = viewmode::CellH(d->selViewMode);
        const int visRows = viewmode::SelectedGridVisibleRows(
            d->selViewMode, itemCount, cols, listBudget);
        // fix thse isseue.md P0 (initial-layout lifecycle): grid-cell sizing of
        // the Selected card + the matching downward shift of every section below
        // is the shared, unit-tested efflayout:: model, so the FIRST frame
        // (initial outer size + WM_CREATE) and this resize/maximize path use ONE
        // calculation -- no startup-only coordinates. The three locals keep their
        // names so they still feed layout::ReflowToClient the mode's whole-cell
        // height + cap below exactly as before.
        const efflayout::ReflowMetrics em = efflayout::GridSizedPreferred(
            base, /*isGrid=*/true, itemCount, cellH, visRows,
            viewmode::GridRows(itemCount, cols));
        selRowHForReflow    = em.selRowH;
        selCapForReflow     = em.selCap;
        rowsNeededForReflow = em.rowsNeeded;
    }

    // Preferred / remembered-size popup: the (now mode-aware) base IS the layout.
    if (!(d->manualSized || IsZoomed(hwnd))) {
        return base;
    }

    // Manually resized or maximized: stretch the SAME mode-aware base into the
    // live client rect, passing the mode's whole-cell height + cap so the grid
    // viewport and scroll capacity are derived from the owner-draw geometry.
    RECT rc{};
    GetClientRect(hwnd, &rc);
    const int cw = rc.right - rc.left;
    const int ch = rc.bottom - rc.top;
    if (cw <= 0 || ch <= 0) return base;
    const int ddpi = static_cast<int>(d->dpi ? d->dpi : 96);
    return layout::ReflowToClient(base,
        MulDiv(cw, 96, ddpi), MulDiv(ch, 96, ddpi),
        winresize::MinClientW(), winresize::MinClientH(),
        selRowHForReflow, selCapForReflow, rowsNeededForReflow);
}

// Win32 EXTRACTION + per-popup cache for the Phase 1 DPI-aware Shell icon
// source. Returns a crisp HICON from the requested system image-list tier
// (SHIL_EXTRALARGE / SHIL_JUMBO), extracted once and cached on the popup; the
// caller DOWNSAMPLES it to the draw size and must NOT free it (the popup's
// NamePromptData destructor owns teardown). Returns nullptr when no high-res
// source is available, so the caller draws a crisp vector glyph instead of
// upscaling. SHGetImageList is resolved from shell32 at runtime (no new import
// library), and uncached extraction is a pure type lookup
// (SHGFI_SYSICONINDEX | SHGFI_USEFILEATTRIBUTES -- no disk hit).
static HICON GetCachedShellIcon(NamePromptData* d, const std::wstring& name,
                                bool isFolder, iconsrc::Tier tier) {
    if (!d) return nullptr;
    const std::wstring key =
        iconsrc::CacheKey(shellicon::IconKey(name, isFolder), tier, d->dpi);
    for (auto& kv : d->iconCache)
        if (kv.first == key) return kv.second;   // reused, never re-extracted

    typedef HRESULT(WINAPI * PFN_SHGetImageList)(int, REFIID, void**);
    static PFN_SHGetImageList pSHGetImageList = []() -> PFN_SHGetImageList {
        HMODULE h = GetModuleHandleW(L"shell32.dll");
        if (!h) h = LoadLibraryW(L"shell32.dll");
        return h ? reinterpret_cast<PFN_SHGetImageList>(
                       GetProcAddress(h, "SHGetImageList"))
                 : nullptr;
    }();

    HICON hIcon = nullptr;
    SHFILEINFOW sfi = {};
    // SYSICONINDEX yields the system image-list index for this file TYPE; the
    // same index resolves across every SHIL_* tier (the lists are parallel).
    if (SHGetFileInfoW(name.c_str(), shellicon::AttrFor(isFolder), &sfi,
                       sizeof(sfi),
                       SHGFI_SYSICONINDEX | SHGFI_USEFILEATTRIBUTES)) {
        IImageList* iml = nullptr;
        if (pSHGetImageList &&
            SUCCEEDED(pSHGetImageList(iconsrc::Shil(tier), __uuidof(IImageList),
                                      reinterpret_cast<void**>(&iml))) &&
            iml) {
            // ILD_TRANSPARENT: a crisp alpha icon from the chosen tier.
            iml->GetIcon(sfi.iIcon, ILD_TRANSPARENT, &hIcon);
            iml->Release();
        }
    }
    // Only a real handle is cached; a transient failure may retry next paint
    // (and the caller falls back to the crisp vector glyph meanwhile).
    if (hIcon) d->iconCache.push_back(std::make_pair(key, hIcon));
    return hIcon;
}

// ---- production vector icon layer (fix thse isseue.md #2-#8) ----
// One DPI-aware GDI renderer keeps every popup icon crisp and independent of
// installed fonts. Callers provide the themed colour and a square destination.
namespace icons {
inline constexpr int kRowIconW   = 16;
inline constexpr int kRowIconGap = 8;   // fix #1 (pass6): icon -> text gap is 8
enum class IconId {
    // fix thse isseue.md v0.12.57: the five SECTION-heading ids (Selected,
    // Destination, Name, Action, Status) are gone -- section headings now use
    // Windows-native semantic icons (see namespace semanticicon). What remains
    // are custom GDI vector marks with NO native-icon equivalent in this popup:
    // the file-row Folder/File glyphs, the Copy/Move segment glyphs, and the
    // resize affordance (Expand/Collapse). fix thse isseue.md #1: the per-state
    // Status marks were removed with the Status/Ready feature.
    Folder, File, Copy, Move, Expand, Collapse
};

// fix thse isseue.md #6: ONE consistent stroke-width rule for every icon (keyed
// off the DPI-scaled box) instead of the old per-icon box-size threshold cliff.
// Byte-for-byte mirror of tests/icon_logic.h::IconStrokeWidth.
inline int IconStrokeWidth(int boxPx) {
    int w = (boxPx + 8) / 16;
    return w < 1 ? 1 : w;
}

// fix thse isseue.md Phase 5 + ADD-ON: the CURRENT find-navigation match gets a
// subtle, theme-aware accent OUTLINE (never a fill), layered on top of the
// normal matched-row look so the OTHER matches stay fully visible. A hollow
// brush keeps the row/cell content underneath untouched (no reorder, no
// selection change). Themed via the caller's accent colour.
static void DrawCurrentMatchOutline(HDC hdc, const RECT& rc, COLORREF accent) {
    HPEN pen = CreatePen(PS_SOLID, 1, accent);
    HGDIOBJ oldPen = SelectObject(hdc, pen);
    HGDIOBJ oldBrush = SelectObject(hdc, GetStockObject(HOLLOW_BRUSH));
    RoundRect(hdc, rc.left, rc.top, rc.right, rc.bottom, 6, 6);
    SelectObject(hdc, oldBrush);
    SelectObject(hdc, oldPen);
    DeleteObject(pen);
}

static void DrawIcon(HDC hdc, IconId id, const RECT& rc, COLORREF color) {
    const int w = rc.right - rc.left;
    const int h = rc.bottom - rc.top;
    const int u = (w < h ? w : h);
    const int x = rc.left + (w - u) / 2;
    const int y = rc.top + (h - u) / 2;
    const int p = IconStrokeWidth(u);
    HPEN pen = CreatePen(PS_SOLID, p, color);
    HGDIOBJ oldPen = SelectObject(hdc, pen);
    HGDIOBJ oldBrush = SelectObject(hdc, GetStockObject(HOLLOW_BRUSH));
    auto line = [hdc](int x1, int y1, int x2, int y2) {
        MoveToEx(hdc, x1, y1, nullptr); LineTo(hdc, x2, y2);
    };
    switch (id) {
        case IconId::Folder:
            MoveToEx(hdc, x + 1, y + 5, nullptr); LineTo(hdc, x + 5, y + 5);
            LineTo(hdc, x + 7, y + 3); LineTo(hdc, x + u - 2, y + 3);
            LineTo(hdc, x + u - 1, y + u - 2); LineTo(hdc, x + 1, y + u - 2);
            LineTo(hdc, x + 1, y + 5);
            break;
        case IconId::File:
            Rectangle(hdc, x + 3, y + 1, x + u - 3, y + u - 1);
            line(x + u - 7, y + 1, x + u - 3, y + 5);
            break;
        case IconId::Move:
            line(x + 1, y + 5, x + u - 2, y + 5);
            line(x + u - 5, y + 2, x + u - 2, y + 5);
            line(x + u - 5, y + 8, x + u - 2, y + 5);
            line(x + u - 1, y + u - 5, x + 2, y + u - 5);
            break;
        case IconId::Copy:
            Rectangle(hdc, x + 1, y + 1, x + u - 5, y + u - 5);
            Rectangle(hdc, x + 5, y + 5, x + u - 1, y + u - 1);
            break;
        case IconId::Expand: {
            // fix thse isseue.md: Maximize -- a single rounded-corner square,
            // matching the native Win11 ChromeMaximize geometry.
            const int r = u / 6 + 1;
            RoundRect(hdc, x + 3, y + 3, x + u - 3, y + u - 3, r, r);
            break;
        }
        case IconId::Collapse: {
            // fix thse isseue.md: Restore -- two overlapping rounded squares, a
            // FRONT square (bottom-left) over a BACK square (top-right) shown as
            // an L, matching the native Win11 ChromeRestore glyph.
            const int r = u / 6 + 1;
            RoundRect(hdc, x + 2, y + 5, x + u - 5, y + u - 2, r, r);  // front
            line(x + 5, y + 2, x + u - 2, y + 2);          // back: top edge
            line(x + u - 2, y + 2, x + u - 2, y + u - 5);  // back: right edge
            line(x + 5, y + 2, x + 5, y + 5);              // back: short left stub
            break;
        }
    }
    SelectObject(hdc, oldBrush);
    SelectObject(hdc, oldPen);
    DeleteObject(pen);
}

// fix thse isseue.md: size of the caption-button glyph (96-dpi px). Bigger than
// the old ~12px mark (and than native ~10px) but still a subtle caption glyph --
// "increase the icon size" while reading as a native control.
inline constexpr int kChromeGlyph = 14;

// A centered square glyph box of side `glyphPx` inside an owner-draw caption
// button cell `rc`. Replaces the old InflateRect(-8) inset so the glyph is a
// deliberate, DPI-scaled size and stays centered now that the button fills the
// full header height.
static RECT CaptionGlyphBox(const RECT& rc, int glyphPx) {
    const int cw = rc.right - rc.left;
    const int ch = rc.bottom - rc.top;
    RECT g;
    g.left   = rc.left + (cw - glyphPx) / 2;
    g.top    = rc.top  + (ch - glyphPx) / 2;
    g.right  = g.left + glyphPx;
    g.bottom = g.top  + glyphPx;
    return g;
}
}  // namespace icons

// fix thse isseue.md #7: hover/pressed state model for the custom title-bar
// chrome buttons (Expand/Minimize/Close). Byte-for-byte mirror of
// tests/chrome_button_logic.h.
namespace chromebtn {
enum class Fill { None, Hover, Pressed };
inline Fill FillFor(bool hot, bool pressed) {
    if (pressed) return Fill::Pressed;
    if (hot)     return Fill::Hover;
    return Fill::None;
}
inline bool PaintsBackground(Fill f) { return f != Fill::None; }
inline bool IsDanger(bool isClose, Fill f) {
    return isClose && f != Fill::None;
}

// fix thse isseue.md A/D: the hover/press colour feedback now fills the FULL
// caption-button rectangle (native caption-button backplate). The old 4px
// left/right inset left blank strips between the hit area and the visible fill,
// so it is removed -- the guard is now 0 (edge-to-edge, full header height).
inline constexpr int kFillGuard = 0;

// Geometry of that colour feedback inside a caption button cell: it spans the
// FULL cell height (top & bottom == header height) and insets `guard` px on the
// LEFT and RIGHT only. Cell-local coordinates (the cell origin is 0,0).
struct FillBox { int x = 0, y = 0, w = 0, h = 0; };
inline FillBox FillArea(int cellW, int cellH, int guard) {
    return FillBox{ guard, 0, cellW - 2 * guard, cellH };
}
}  // namespace chromebtn

// fix thse isseue.md #14: hover visual-state + blend for the owner-drawn
// Create/Cancel buttons and Copy/Move segments (pressed fill + focus ring are
// handled elsewhere). Byte-for-byte mirror of tests/hover_tint_logic.h.
namespace hovertint {
enum class Vis { Normal, Hover, Pressed };
inline Vis VisFor(bool hot, bool pressed) {
    if (pressed) return Vis::Pressed;
    if (hot)     return Vis::Hover;
    return Vis::Normal;
}
inline bool PaintsHover(Vis v) { return v == Vis::Hover; }
inline int BlendChannel(int c, int target, int pct) {
    if (pct < 0) pct = 0;
    if (pct > 100) pct = 100;
    int r = c + (target - c) * pct / 100;
    if (r < 0) r = 0;
    if (r > 255) r = 255;
    return r;
}
inline constexpr int kHoverPct = 12;
// fix thse isseue.md #4: keyboard-focus-visible nudges the fill toward the
// accent by this fraction -- a subtle focus COLOUR that replaces the removed
// painted focus ring. Independent of kHoverPct so focus/hover stay separate.
inline constexpr int kFocusPct = 16;
}  // namespace hovertint

// Byte-for-byte mirror of tests/seg_glide_logic.h (fix thse isseue.md #3): the
// Copy|Move active fill COLOUR glides between the two segments over a short
// eased ramp instead of flipping instantly. Pure numbers only; the GDI blend +
// SetTimer wiring live below and are pinned by source invariants.
namespace segglide {
inline constexpr int kMin  = 0;
inline constexpr int kMax  = 100;
// fix thse isseue.md #5 C: ~200ms eased Copy<->Move slide. ceil(100/kStepPct)
// ticks * kStepMs ms == ~208ms, inside the issue's 160-220ms target; the model
// is a DPI-independent 0..100 percentage. The old 20ms/5% (~400ms) read as the
// sluggish wiggly glide the issue asks to replace.
inline constexpr int kStepMs  = 16;
inline constexpr int kStepPct = 8;
inline int Advance(int cur, int target) {
    if (target < kMin) target = kMin;
    if (target > kMax) target = kMax;
    if (cur < target) { cur += kStepPct; if (cur > target) cur = target; }
    else if (cur > target) { cur -= kStepPct; if (cur < target) cur = target; }
    return cur;
}
inline int Ease(int p) {
    if (p <= kMin) return kMin;
    if (p >= kMax) return kMax;
    double t = p / 100.0;
    double e = t * t * (3.0 - 2.0 * t);
    int r = static_cast<int>(e * 100.0 + 0.5);
    if (r < kMin) r = kMin;
    if (r > kMax) r = kMax;
    return r;
}
// fix thse isseue.md #5 C: one ACTIVE PILL that slides between Copy and Move
// (replaces the old per-half AccentFracForSeg blend). Each segment is one half
// of the track; this returns the sub-span of THIS half the pill covers (0..100
// of the half width). Copy's pill hugs the seam (right) and grows left as p->0
// ([Ease(p),100]); Move's hugs the seam (left) and grows right as p->100
// ([0,Ease(p)]). The two halves' spans sum to one half-width, so it reads as
// ONE pill sliding across the seam. Keep byte-identical to seg_glide_logic.h.
struct PillBand { int startPct; int endPct; };
inline PillBand PillBandPct(int p, bool isCopySeg) {
    int e = Ease(p);
    if (isCopySeg) return PillBand{e, kMax};
    return               PillBand{kMin, e};
}
// fix thse isseue.md #6: fill the sliding pill at kFillAccentPct of the way from
// the neutral segment fill toward the Create accent -- a calm, muted accent
// tint, not a loud flash. MutedAccentFrac(kMax) == kFillAccentPct.
inline constexpr int kFillAccentPct = 65;
inline int MutedAccentFrac(int accentFrac) {
    if (accentFrac < kMin) accentFrac = kMin;
    if (accentFrac > kMax) accentFrac = kMax;
    return accentFrac * kFillAccentPct / 100;
}
inline int RestProgress(bool isMove) { return isMove ? kMax : kMin; }
}  // namespace segglide

// fix thse isseue.md v0.12.57 (Semantic Section Icon pass): the five section
// headings render a WINDOWS-NATIVE icon instead of a hand-drawn glyph inside a
// coloured tile -- Segoe Fluent Icons (Win11) -> Segoe MDL2 Assets (Win10) ->
// Shell stock icon (SHGetStockIconInfo) when no icon font is installed. This is
// the byte-for-byte (code) mirror of namespace semanticicon in
// tests/semantic_icon_logic.h; the GDI/Shell paint lives in
// DrawSemanticSectionIcon below and is pinned by source invariants.
namespace semanticicon {

// The five popup section headings that carry a semantic icon.
enum class Section { Selected, Destination, Name, Status, Action };

// Where the painted icon comes from. Fluent (Win11) is preferred, MDL2 (Win10)
// is the font fallback, and the Shell stock icon is the last-resort fallback
// when NEITHER icon font is installed (so a heading never shows a tofu box).
enum class Provider { FluentFont, Mdl2Font, ShellStock };

// A resolved icon source. For the two font providers `id` is a PUA glyph code
// point; for ShellStock `id` is a SHSTOCKICONID (SIID_*) value.
struct Spec {
    Provider provider;
    uint32_t id;
};

// Segoe Fluent Icons / Segoe MDL2 Assets shared PUA code points: List
// (Selected), Folder (Destination), Rename (Folder name), Info (Status),
// MoveToFolder (Action). Standard, Microsoft-documented symbols -- not custom.
inline constexpr uint32_t kGlyphList         = 0xE8FDu;  // List
inline constexpr uint32_t kGlyphFolder       = 0xE8B7u;  // Folder
inline constexpr uint32_t kGlyphRename       = 0xE8ACu;  // Rename / Edit
inline constexpr uint32_t kGlyphInfo         = 0xE946u;  // Info
inline constexpr uint32_t kGlyphMoveToFolder = 0xE8DEu;  // MoveToFolder

// SHSTOCKICONID (ShellApi.h) values for the no-icon-font fallback.
inline constexpr uint32_t kStockStack  = 55u;  // SIID_STACK
inline constexpr uint32_t kStockFolder = 3u;   // SIID_FOLDER
inline constexpr uint32_t kStockRename = 83u;  // SIID_RENAME
inline constexpr uint32_t kStockInfo   = 79u;  // SIID_INFO

// The icon-font glyph for a section (identical code point in Fluent & MDL2).
inline uint32_t Glyph(Section s) {
    switch (s) {
        case Section::Selected:    return kGlyphList;
        case Section::Destination: return kGlyphFolder;
        case Section::Name:        return kGlyphRename;
        case Section::Status:      return kGlyphInfo;
        case Section::Action:      return kGlyphMoveToFolder;
    }
    return kGlyphInfo;  // defensive (unreachable)
}

// The Shell stock icon for a section when NO icon font is available. Action
// (MoveToFolder) has no exact stock peer, so it falls back to the folder icon.
inline uint32_t StockId(Section s) {
    switch (s) {
        case Section::Selected:    return kStockStack;
        case Section::Destination: return kStockFolder;
        case Section::Name:        return kStockRename;
        case Section::Status:      return kStockInfo;
        case Section::Action:      return kStockFolder;
    }
    return kStockFolder;  // defensive (unreachable)
}

// Preferred provider: Segoe Fluent Icons on Windows 11, Segoe MDL2 Assets on
// Windows 10. The glyph code point is the same either way.
inline Spec Resolve(Section s, bool fluentAvailable) {
    return Spec{ fluentAvailable ? Provider::FluentFont : Provider::Mdl2Font,
                 Glyph(s) };
}

// Full resolution with the safe fallback: when NEITHER icon font is installed
// (no Segoe Fluent Icons AND no Segoe MDL2 Assets) drop to a real Shell stock
// icon so a heading never paints a missing-glyph box.
inline Spec ResolveWithFallback(Section s, bool fluentAvailable,
                                bool anyIconFontAvailable) {
    if (!anyIconFontAvailable) return Spec{ Provider::ShellStock, StockId(s) };
    return Resolve(s, fluentAvailable);
}

}  // namespace semanticicon

// Tie the SIID_* numeric mirrors above to the real ShellApi.h values so a wrong
// constant fails the live clang compile (same pattern as the FW_* mirrors).
static_assert(semanticicon::kStockStack  == static_cast<uint32_t>(SIID_STACK),  "SIID_STACK mirror drift");
static_assert(semanticicon::kStockFolder == static_cast<uint32_t>(SIID_FOLDER), "SIID_FOLDER mirror drift");
static_assert(semanticicon::kStockRename == static_cast<uint32_t>(SIID_RENAME), "SIID_RENAME mirror drift");
static_assert(semanticicon::kStockInfo   == static_cast<uint32_t>(SIID_INFO),   "SIID_INFO mirror drift");

// fix thse isseue.md #9: a SMALL leading Copy/Move glyph before each segment
// caption; the icon+label group is centred as a unit. Byte-for-byte mirror of
// tests/segicon_logic.h.
namespace segicon {
inline constexpr int kSegIcon    = 14;
inline constexpr int kSegIconGap = 6;
inline int GroupWidth(int textW) { return kSegIcon + kSegIconGap + textW; }
inline int GroupLeft(int segLeft, int segW, int groupW) {
    int x = segLeft + (segW - groupW) / 2;
    return x < segLeft ? segLeft : x;
}
inline int IconLeft(int groupLeft) { return groupLeft; }
inline int TextLeft(int groupLeft) { return groupLeft + kSegIcon + kSegIconGap; }
}  // namespace segicon

// ---- Direct2D anti-aliased rounded-surface layer (fix thse isseue.md #2/#12) --
// GDI RoundRect stair-steps the corners of the custom-painted surfaces (section
// cards, Create/Cancel, Copy/Move, extension badges, icon tiles, progress
// track, focus ring). Direct2D draws the SAME rounded rectangles with
// per-primitive anti-aliasing, so the corners are smooth at 100/125/150/200%.
// The policy (which surfaces, the float radius, the AA-mode constant, the
// stroke inset, the GDI fallback) is the unit-tested namespace aarender,
// mirrored byte-for-byte here. EDIT controls + the window frame stay pure Win32.
namespace aarender {
enum class Surface { Card, Button, Segment, Badge, Tile, Progress, FocusRing,
                     ScrollThumb, EditField, Window };
inline bool UsesAntialiasedGeometry(Surface s) {
    switch (s) {
        case Surface::Card:
        case Surface::Button:
        case Surface::Segment:
        case Surface::Badge:
        case Surface::Tile:
        case Surface::Progress:
        case Surface::FocusRing:
        case Surface::ScrollThumb:
            return true;
        case Surface::EditField:
        case Surface::Window:
        default:
            return false;
    }
}
inline constexpr int kAntialiasPerPrimitive = 0;
inline constexpr bool kFallbackToGdiOnFailure = true;
inline float DeviceRadius(int designRadiusPx, int dpi) {
    return static_cast<float>(designRadiusPx) * static_cast<float>(dpi) / 96.0f;
}
inline float StrokeInset(float strokeWidthDev) { return strokeWidthDev * 0.5f; }
}  // namespace aarender

// The tested AA-mode constant MUST equal the real Direct2D enum value.
static_assert(aarender::kAntialiasPerPrimitive ==
                  static_cast<int>(D2D1_ANTIALIAS_MODE_PER_PRIMITIVE),
              "aarender::kAntialiasPerPrimitive must match "
              "D2D1_ANTIALIAS_MODE_PER_PRIMITIVE");

namespace d2d {

// Process-lifetime factory + a reusable DC render target (re-bound per paint on
// the UI thread). Not released at unload: a SINGLE_THREADED factory is not
// agile, so a cross-thread Release() at ModUninit would be unsafe; the one-time
// leak at unload is deliberate.
static ID2D1Factory*        g_factory = nullptr;
static ID2D1DCRenderTarget* g_target  = nullptr;

inline void Release() {
    if (g_target)  { g_target->Release();  g_target  = nullptr; }
    if (g_factory) { g_factory->Release(); g_factory = nullptr; }
}

inline bool Ensure() {
    if (!g_factory) {
        HRESULT hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED,
                                       &g_factory);
        if (FAILED(hr) || !g_factory) { g_factory = nullptr; return false; }
    }
    if (!g_target) {
        const D2D1_RENDER_TARGET_PROPERTIES props = D2D1::RenderTargetProperties(
            D2D1_RENDER_TARGET_TYPE_DEFAULT,
            D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_IGNORE),
            96.0f, 96.0f);   // 1 DIP == 1 device pixel: we pass device-px rects
        HRESULT hr = g_factory->CreateDCRenderTarget(&props, &g_target);
        if (FAILED(hr) || !g_target) { g_target = nullptr; return false; }
    }
    return true;
}

inline D2D1_COLOR_F ColorOf(COLORREF c) {
    return D2D1::ColorF(GetRValue(c) / 255.0f, GetGValue(c) / 255.0f,
                        GetBValue(c) / 255.0f, 1.0f);
}

// Core: bind to `bindRc`, draw ONE rounded rect given in TARGET-LOCAL coords
// (origin at bindRc's top-left). The local coords MAY exceed [0..w]/[0..h] so
// BindDC's clip rounds only the visible side -- the Copy|Move segments use that
// to round a single side. Best-effort: returns false (painting nothing) on any
// failure so the caller falls back to GDI.
inline bool RoundRectCore(HDC hdc, const RECT& bindRc, float lx, float ly,
                          float rx, float ry, float radiusDev, bool hasFill,
                          COLORREF fill, bool hasBorder, COLORREF border,
                          float strokeDev) {
    if (!Ensure()) return false;
    if (bindRc.right <= bindRc.left || bindRc.bottom <= bindRc.top) return false;
    if (FAILED(g_target->BindDC(hdc, &bindRc))) return false;

    g_target->BeginDraw();
    g_target->SetAntialiasMode(D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);

    const D2D1_ROUNDED_RECT body =
        D2D1::RoundedRect(D2D1::RectF(lx, ly, rx, ry), radiusDev, radiusDev);
    if (hasFill) {
        ID2D1SolidColorBrush* fillBrush = nullptr;
        if (SUCCEEDED(g_target->CreateSolidColorBrush(ColorOf(fill), &fillBrush))
            && fillBrush) {
            g_target->FillRoundedRectangle(body, fillBrush);
            fillBrush->Release();
        }
    }
    if (hasBorder && strokeDev > 0.0f) {
        ID2D1SolidColorBrush* borderBrush = nullptr;
        if (SUCCEEDED(g_target->CreateSolidColorBrush(ColorOf(border),
                                                      &borderBrush))
            && borderBrush) {
            g_target->DrawRoundedRectangle(body, borderBrush, strokeDev);
            borderBrush->Release();
        }
    }

    if (FAILED(g_target->EndDraw())) {
        Release();   // device lost / invalid -> recreate next paint; GDI now
        return false;
    }
    return true;
}

// Full rounded rect: fill + optional hairline, inset so the stroke stays inside.
inline bool RoundRectAA(HDC hdc, const RECT& rc, float radiusDev, COLORREF fill,
                        bool hasBorder, COLORREF border, float strokeDev) {
    const float w = static_cast<float>(rc.right  - rc.left);
    const float h = static_cast<float>(rc.bottom - rc.top);
    if (w <= 0.0f || h <= 0.0f) return false;
    const float stroke = (hasBorder && strokeDev > 0.0f) ? strokeDev : 0.0f;
    const float ins    = aarender::StrokeInset(stroke);
    return RoundRectCore(hdc, rc, ins, ins, w - ins, h - ins, radiusDev,
                         true, fill, hasBorder, border, stroke);
}

// Stroke-only rounded outline (the focus ring) -- no fill.
inline bool RoundStrokeAA(HDC hdc, const RECT& rc, float radiusDev,
                          COLORREF border, float strokeDev) {
    const float w = static_cast<float>(rc.right  - rc.left);
    const float h = static_cast<float>(rc.bottom - rc.top);
    if (w <= 0.0f || h <= 0.0f || strokeDev <= 0.0f) return false;
    const float ins = aarender::StrokeInset(strokeDev);
    return RoundRectCore(hdc, rc, ins, ins, w - ins, h - ins, radiusDev,
                         false, 0, true, border, strokeDev);
}

// One segment of the Copy|Move pill: round only the left (roundLeft) OR right
// corners by pushing the OTHER side past the bind rect, so BindDC clips it to a
// straight seam with no border line (the same technique as the GDI version).
inline bool RoundSegmentAA(HDC hdc, const RECT& rc, float radiusDev,
                           bool roundLeft, COLORREF fill, COLORREF border,
                           float strokeDev) {
    const float w = static_cast<float>(rc.right  - rc.left);
    const float h = static_cast<float>(rc.bottom - rc.top);
    if (w <= 0.0f || h <= 0.0f) return false;
    const float ins  = aarender::StrokeInset(strokeDev);
    const float over = radiusDev + ins + 1.0f;   // push the square side outside
    const float lx = roundLeft ? ins        : -over;
    const float rx = roundLeft ? (w + over)  : (w - ins);
    return RoundRectCore(hdc, rc, lx, ins, rx, h - ins, radiusDev,
                         true, fill, true, border, strokeDev);
}

}  // namespace d2d

// Try to paint a rounded surface with Direct2D anti-aliased geometry; false
// means Direct2D is unavailable (nothing painted) so the caller GDI-fills. The
// Surface gate ties the tested aarender policy into the live paint.
// `gdiRadiusPx` is the GDI RoundRect ellipse-axis the caller already uses (==
// 2x the true corner radius), so visual parity is exact; strokePx is the
// device-pixel hairline; hasBorder=false -> borderless fill (icon tiles).
static bool DrawRoundSurfaceAA(HDC hdc, const RECT& rc, aarender::Surface surf,
                               int gdiRadiusPx, COLORREF fill, bool hasBorder,
                               COLORREF border, int strokePx) {
    if (!aarender::UsesAntialiasedGeometry(surf)) return false;
    return d2d::RoundRectAA(hdc, rc, static_cast<float>(gdiRadiusPx) * 0.5f,
                            fill, hasBorder, border,
                            static_cast<float>(strokePx));
}

// Stroke-only AA rounded outline (the focus ring).
static bool DrawRoundStrokeAA(HDC hdc, const RECT& rc, aarender::Surface surf,
                              int gdiRadiusPx, COLORREF border, int strokePx) {
    if (!aarender::UsesAntialiasedGeometry(surf)) return false;
    return d2d::RoundStrokeAA(hdc, rc, static_cast<float>(gdiRadiusPx) * 0.5f,
                              border, static_cast<float>(strokePx));
}

// One AA segment of the Copy|Move pill (rounds only the left or right corners).
static bool DrawRoundSegmentAA(HDC hdc, const RECT& rc, aarender::Surface surf,
                               int gdiRadiusPx, bool roundLeft, COLORREF fill,
                               COLORREF border, int strokePx) {
    if (!aarender::UsesAntialiasedGeometry(surf)) return false;
    return d2d::RoundSegmentAA(hdc, rc, static_cast<float>(gdiRadiusPx) * 0.5f,
                               roundLeft, fill, border,
                               static_cast<float>(strokePx));
}

// fix thse isseue.md v0.12.57: probe ONCE whether a given icon-font face is
// actually installed. GDI silently substitutes a default face for a missing
// one, so we realise the font and read GetTextFaceW back -- a mismatch means the
// face is absent and we must fall back to a Shell stock icon, not paint a tofu
// box. (Win32-only: lstrcpynW / lstrcmpiW, no secure-CRT dependency.)
static bool IconFontInstalled(const wchar_t* face) {
    HDC hdc = GetDC(nullptr);
    if (!hdc) return false;
    LOGFONTW lf = {};
    lf.lfHeight  = -16;
    lf.lfCharSet = DEFAULT_CHARSET;
    lstrcpynW(lf.lfFaceName, face, LF_FACESIZE);
    HFONT f = CreateFontIndirectW(&lf);
    bool ok = false;
    if (f) {
        HGDIOBJ of = SelectObject(hdc, f);
        wchar_t got[LF_FACESIZE] = {};
        GetTextFaceW(hdc, LF_FACESIZE, got);
        ok = (lstrcmpiW(got, face) == 0);
        SelectObject(hdc, of);
        DeleteObject(f);
    }
    ReleaseDC(nullptr, hdc);
    return ok;
}
static bool FluentIconsAvailable() {
    static int cached = -1;
    if (cached < 0) { cached = IconFontInstalled(L"Segoe Fluent Icons") ? 1 : 0; }
    return cached == 1;
}
static bool AnyIconFontAvailable() {
    static int cached = -1;
    if (cached < 0) {
        cached = (FluentIconsAvailable() ||
                  IconFontInstalled(L"Segoe MDL2 Assets")) ? 1 : 0;
    }
    return cached == 1;
}

// fix thse isseue.md C: PURE caption-button glyph model (byte-for-byte mirror of
// tests/caption_glyph_logic.h). Verified Segoe caption glyphs E921 ChromeMinimize,
// E922 ChromeMaximize, E923 ChromeRestore, E8BB ChromeClose; the provider falls
// back Fluent -> MDL2 -> geometric so a missing icon font never paints tofu.
namespace captionglyph {
enum class Button { Minimize, Expand, Close };
enum class Provider { FluentFont, Mdl2Font, Geometric };
inline unsigned Glyph(Button b, bool expanded) {
    switch (b) {
        case Button::Minimize: return 0xE921u;  // ChromeMinimize
        case Button::Close:    return 0xE8BBu;  // ChromeClose
        case Button::Expand:   return expanded ? 0xE923u   // ChromeRestore
                                               : 0xE922u;  // ChromeMaximize
    }
    return 0xE8BBu;
}
inline Provider ResolveProvider(bool fluentAvailable, bool anyFontAvailable) {
    if (fluentAvailable)  return Provider::FluentFont;
    if (anyFontAvailable) return Provider::Mdl2Font;
    return Provider::Geometric;
}
inline bool UsesFont(Provider p) { return p != Provider::Geometric; }
}  // namespace captionglyph

// fix thse isseue.md C: draw a caption glyph with the native Windows caption font
// -- Segoe Fluent Icons (Win11), Segoe MDL2 Assets (Win10) -- centered in `box`.
// Returns false when NO icon font is installed OR the specific glyph is not
// actually present in the realised font (GetGlyphIndicesW), so the caller paints
// its deliberate GEOMETRIC fallback instead of an empty / notdef box. Glyph
// support is thus VERIFIED, never assumed from a mere font install.
static bool DrawCaptionGlyph(HDC hdc, const RECT& box,
                             captionglyph::Button button, bool expanded,
                             COLORREF ink) {
    const captionglyph::Provider prov = captionglyph::ResolveProvider(
        FluentIconsAvailable(), AnyIconFontAvailable());
    if (!captionglyph::UsesFont(prov)) return false;   // no icon font -> geometric
    const int boxPx = box.bottom - box.top;
    if (boxPx <= 0) return false;
    const wchar_t* face = (prov == captionglyph::Provider::FluentFont)
                              ? L"Segoe Fluent Icons" : L"Segoe MDL2 Assets";
    LOGFONTW lf = {};
    lf.lfHeight  = -boxPx;
    lf.lfWeight  = FW_NORMAL;
    lf.lfCharSet = DEFAULT_CHARSET;
    lf.lfQuality = CLEARTYPE_NATURAL_QUALITY;
    lstrcpynW(lf.lfFaceName, face, LF_FACESIZE);
    HFONT glyphFont = CreateFontIndirectW(&lf);
    if (!glyphFont) return false;
    HGDIOBJ oldF = SelectObject(hdc, glyphFont);
    // Confirm the glyph really exists in this face; GDI would otherwise paint a
    // notdef/tofu box. If missing, restore and let the caller draw geometry.
    const wchar_t ch = static_cast<wchar_t>(captionglyph::Glyph(button, expanded));
    WORD gi = 0;
    const bool present =
        (GetGlyphIndicesW(hdc, &ch, 1, &gi, GGI_MARK_NONEXISTING_GLYPHS) != GDI_ERROR) &&
        (gi != 0xFFFF);
    if (!present) {
        SelectObject(hdc, oldF);
        DeleteObject(glyphFont);
        return false;
    }
    const int oldBk       = SetBkMode(hdc, TRANSPARENT);
    const COLORREF oldClr = SetTextColor(hdc, ink);
    const wchar_t glyph[2] = { ch, L'\0' };
    RECT dr = box;
    DrawTextW(hdc, glyph, 1, &dr,
              DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOCLIP);
    SetTextColor(hdc, oldClr);
    SetBkMode(hdc, oldBk);
    SelectObject(hdc, oldF);
    DeleteObject(glyphFont);
    return true;
}

// Paint ONE Windows-native semantic section icon in an already DEVICE-PX square
// (NO coloured tile). fix thse isseue.md v0.12.57: Segoe Fluent Icons (Win11) ->
// Segoe MDL2 Assets (Win10) -> Shell stock icon (SHGetStockIconInfo) when no
// icon font is installed. The provider/glyph decision is the tested pure model
// semanticicon::ResolveWithFallback; this routine only paints it.
static void DrawSemanticSectionIconDev(HDC hdc, semanticicon::Section section,
                                       int left, int top, int boxPx,
                                       COLORREF ink) {
    const semanticicon::Spec spec = semanticicon::ResolveWithFallback(
        section, FluentIconsAvailable(), AnyIconFontAvailable());
    if (spec.provider == semanticicon::Provider::ShellStock) {
        SHSTOCKICONINFO sii = {};
        sii.cbSize = sizeof(sii);
        if (SUCCEEDED(SHGetStockIconInfo(static_cast<SHSTOCKICONID>(spec.id),
                                         SHGSI_ICON | SHGSI_SMALLICON, &sii))) {
            DrawIconEx(hdc, left, top, sii.hIcon, boxPx, boxPx, 0, nullptr,
                       DI_NORMAL);
            DestroyIcon(sii.hIcon);
        }
        return;
    }
    const wchar_t* face = (spec.provider == semanticicon::Provider::FluentFont)
                              ? L"Segoe Fluent Icons" : L"Segoe MDL2 Assets";
    LOGFONTW lf = {};
    lf.lfHeight  = -boxPx;
    lf.lfWeight  = FW_NORMAL;
    lf.lfCharSet = DEFAULT_CHARSET;
    lf.lfQuality = CLEARTYPE_NATURAL_QUALITY;
    lstrcpynW(lf.lfFaceName, face, LF_FACESIZE);
    // fix thse isseue.md P2 (8366-8378): defensive GDI handling under resource
    // pressure. Never select a null font onto the DC, never draw with an
    // unknown font, never restore an invalid previous object, and always delete
    // a font we created.
    HFONT iconFont = CreateFontIndirectW(&lf);
    if (!gdiguard::MustDeleteCreatedFont(iconFont != nullptr)) {
        Wh_Log(L"DrawSemanticSectionIconDev: CreateFontIndirectW failed; "
               L"glyph skipped");
        return;  // no font -> nothing to select, draw, or delete
    }
    HGDIOBJ oldF = SelectObject(hdc, iconFont);
    const bool selectOk = (oldF != nullptr && oldF != HGDI_ERROR);
    if (!gdiguard::ShouldDrawGlyph(/*fontCreated*/true, selectOk)) {
        // Could not select the font: do not draw with an unknown font and do
        // not restore an invalid previous object, but still free the font.
        DeleteObject(iconFont);
        Wh_Log(L"DrawSemanticSectionIconDev: SelectObject(font) failed; "
               L"glyph skipped");
        return;
    }
    const int oldBk       = SetBkMode(hdc, TRANSPARENT);
    const COLORREF oldClr = SetTextColor(hdc, ink);
    const wchar_t glyph[2] = { static_cast<wchar_t>(spec.id), L'\0' };
    RECT dr = {left, top, left + boxPx, top + boxPx};
    DrawTextW(hdc, glyph, 1, &dr,
              DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOCLIP);
    SetTextColor(hdc, oldClr);
    SetBkMode(hdc, oldBk);
    if (gdiguard::ShouldRestoreOldFont(selectOk)) {
        SelectObject(hdc, oldF);
    }
    DeleteObject(iconFont);
}

// Label-based wrapper (96-dpi CLIENT label rect). fix thse isseue.md v0.12.57:
// the 16px icon sits in the reserved kSectionIconCol column at the card content
// inner-left (label.x already includes the column), vertically centred on the
// heading, with an 8px gap to the title text (column 24 - icon 16 = 8). No tile.
static void DrawSemanticSectionIcon(HDC hdc, semanticicon::Section section,
                                    const layout::Rect& label, UINT dpi,
                                    COLORREF ink) {
    const int boxPx = ScaleDpi(icons::kRowIconW, dpi);  // 16
    const int left  = ScaleDpi(label.x - layout::kSectionIconCol, dpi);
    const int top   = ScaleDpi(label.y, dpi) +
                      (ScaleDpi(label.h, dpi) - boxPx) / 2;
    DrawSemanticSectionIconDev(hdc, section, left, top, boxPx, ink);
}

// Paint one rounded section card: themed fill + 1px hairline border. `radiusPx`
// is already DPI-scaled by the caller so cards stay crisp at 125-200% (Phase 9
// re-checks the rounded-corner artifacts). No GDI leak: the temporary brush/pen
// are deselected and deleted before returning.
static void DrawSectionCard(HDC hdc, const RECT& rc, COLORREF fill,
                            COLORREF border, int radiusPx) {
    // #2/#12: anti-aliased rounded card via Direct2D; GDI RoundRect fallback.
    if (DrawRoundSurfaceAA(hdc, rc, aarender::Surface::Card, radiusPx, fill,
                           true, border, 1)) {
        return;
    }
    HBRUSH  br  = CreateSolidBrush(fill);
    HPEN    pen = CreatePen(PS_SOLID, 1, border);
    HGDIOBJ oldBr  = SelectObject(hdc, br);
    HGDIOBJ oldPen = SelectObject(hdc, pen);
    RoundRect(hdc, rc.left, rc.top, rc.right, rc.bottom, radiusPx, radiusPx);
    SelectObject(hdc, oldBr);
    SelectObject(hdc, oldPen);
    DeleteObject(br);
    DeleteObject(pen);
}

// ============================================================================
// fix thse isseue.md Issue 2: async image-thumbnail extraction + cache.
//
// A dedicated per-popup worker thread extracts real image thumbnails via
// IShellItemImageFactory (SIIGBF_THUMBNAILONLY, so non-image / unsupported files
// simply fail -> the crisp Shell icon fallback is kept). Results are cached by
// "path|px" so scrolling and view switches reuse them. The paint path NEVER
// blocks: it looks up the cache, enqueues a miss, draws the icon this frame and
// repaints when the worker posts WM_APP_THUMB_READY. All bitmaps + the thread
// are released by StopThumbThread() before the popup's NamePromptData dies.
// ============================================================================
static const UINT WM_APP_THUMB_READY = WM_APP + 7;

static DWORD WINAPI ThumbThreadProc(LPVOID param) {
    NamePromptData* d = reinterpret_cast<NamePromptData*>(param);
    const HRESULT coHr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    while (WaitForSingleObject(d->thumbWake, INFINITE) == WAIT_OBJECT_0) {
        if (InterlockedCompareExchange(&d->thumbStop, 0, 0)) break;
        for (;;) {
            if (InterlockedCompareExchange(&d->thumbStop, 0, 0)) break;
            std::wstring path;
            int px = 0;
            EnterCriticalSection(&d->thumbCs);
            if (d->thumbQueueReq.empty()) {
                LeaveCriticalSection(&d->thumbCs);
                break;
            }
            path = d->thumbQueueReq.back().first;   // LIFO: the most recently
            px   = d->thumbQueueReq.back().second;  // requested (visible) item first
            d->thumbQueueReq.pop_back();
            LeaveCriticalSection(&d->thumbCs);

            HBITMAP bmp = nullptr;
            int bw = 0, bh = 0;
            bool ok = false;
            IShellItemImageFactory* factory = nullptr;
            if (!path.empty() &&
                SUCCEEDED(SHCreateItemFromParsingName(
                    path.c_str(), nullptr, IID_PPV_ARGS(&factory))) && factory) {
                SIZE sz = {px, px};
                HBITMAP hb = nullptr;
                // THUMBNAILONLY => a real content thumbnail or failure (never a
                // generic icon); the caller supplies the icon fallback itself.
                if (SUCCEEDED(factory->GetImage(
                        sz, SIIGBF_THUMBNAILONLY | SIIGBF_BIGGERSIZEOK, &hb)) && hb) {
                    BITMAP bm = {};
                    // thumblife::IsValidThumbnailMetadata: accept the bitmap ONLY
                    // when GetObject succeeded with positive dimensions; drop and
                    // release anything else (fix thse isseue.md P2 / 8460-8468).
                    const bool metaOk =
                        GetObject(hb, sizeof(bm), &bm) == static_cast<int>(sizeof(bm));
                    if (thumblife::IsValidThumbnailMetadata(metaOk, bm.bmWidth, bm.bmHeight)) {
                        bmp = hb;
                        bw = bm.bmWidth;
                        bh = bm.bmHeight;
                        ok = true;
                    } else {
                        DeleteObject(hb);
                    }
                }
                factory->Release();
            }

            const std::wstring key = path + L"|" + std::to_wstring(px);
            EnterCriticalSection(&d->thumbCs);
            bool stored = false;
            for (auto& kv : d->thumbCache) {
                if (kv.first == key) {
                    if (ok) {
                        if (kv.second.bmp && kv.second.bmp != bmp)
                            DeleteObject(kv.second.bmp);
                        kv.second.bmp = bmp;
                        kv.second.w = bw;
                        kv.second.h = bh;
                    }
                    kv.second.done = true;
                    kv.second.failed = !ok;
                    stored = true;
                    break;
                }
            }
            if (!stored) {
                NamePromptData::ThumbEntry e;
                e.bmp = ok ? bmp : nullptr;
                e.w = bw;
                e.h = bh;
                e.done = true;
                e.failed = !ok;
                d->thumbCache.push_back({key, e});
            }
            // Read the owner HWND under the SAME lock GetThumbnailForPaint
            // publishes it under (fix thse isseue.md P1: synchronize ownerHwnd).
            HWND owner = d->ownerHwnd;
            LeaveCriticalSection(&d->thumbCs);
            if (!ok && bmp) DeleteObject(bmp);

            if (owner) PostMessageW(owner, WM_APP_THUMB_READY, 0, 0);
        }
    }
    if (SUCCEEDED(coHr)) CoUninitialize();
    return 0;
}

static void EnsureThumbThread(NamePromptData* d) {
    if (!d || d->thumbInit) return;  // thumblife::CanRetryInit gates re-entry:
                                     // only (re)create while init is not done.
    InitializeCriticalSection(&d->thumbCs);
    d->thumbStop = 0;
    d->thumbWake = CreateEventW(nullptr, FALSE, FALSE, nullptr);  // auto-reset
    if (!d->thumbWake) {
        // Event creation failed. Unwind the critical section and leave
        // thumbInit=false so a later paint RETRIES instead of being stuck on
        // the Shell-icon fallback forever (fix thse isseue.md P1 / 8509-8516).
        DeleteCriticalSection(&d->thumbCs);
        return;
    }
    d->thumbThread = CreateThread(nullptr, 0, ThumbThreadProc, d, 0, nullptr);
    // thumblife::ShouldMarkInitialized: publish init ONLY when BOTH the wake
    // event and the worker thread exist. Otherwise unwind and stay retryable.
    if (!thumblife::ShouldMarkInitialized(d->thumbWake != nullptr,
                                          d->thumbThread != nullptr)) {
        if (d->thumbWake) { CloseHandle(d->thumbWake); d->thumbWake = nullptr; }
        DeleteCriticalSection(&d->thumbCs);
        return;  // thumbInit stays false -> a later frame retries.
    }
    d->thumbInit = true;
}

// fix thse isseue.md Issue 2: stop + free the thumbnail worker and every cached
// bitmap. Called when the popup closes, BEFORE ~NamePromptData, so the worker
// can never post to a dead window or race the cache teardown.
static void StopThumbThread(NamePromptData* d) {
    if (!d || !d->thumbInit) return;
    InterlockedExchange(&d->thumbStop, 1);
    if (d->thumbWake) SetEvent(d->thumbWake);
    if (d->thumbThread) {
        // The worker dereferences *d (its critical section, bitmap cache and
        // ownerHwnd) and d is about to be destroyed, so freeing anything the
        // worker can still touch BEFORE it has stopped is a use-after-free.
        // thumbStop is re-checked before every queued item, so only the single
        // in-flight extraction can delay exit. Give it a grace period, then --
        // only if it has NOT confirmed termination -- wait without a premature
        // timeout. thumblife::SafeToReleaseAfterWait gates the teardown below so
        // thread-owned resources are released ONLY after a confirmed exit.
        int elapsed = 0;
        bool confirmedExit = false;
        while (thumblife::ShouldKeepWaiting(confirmedExit, elapsed)) {
            if (d->thumbWake) SetEvent(d->thumbWake);
            confirmedExit =
                (WaitForSingleObject(d->thumbThread,
                                     thumblife::ExitWaitSliceMs()) == WAIT_OBJECT_0);
            elapsed += thumblife::ExitWaitSliceMs();
        }
        if (thumblife::SafeToReleaseAfterWait(confirmedExit)) {
            CloseHandle(d->thumbThread);
            d->thumbThread = nullptr;
        } else {
            // The worker did NOT confirm exit within the bounded budget (a Shell
            // thumbnail provider is wedged). Releasing the thread handle, wake
            // event, critical section or cached bitmaps now would be a
            // use-after-free the instant the stuck extraction returns and
            // dereferences *d. Define the safe behavior: DELIBERATELY LEAK every
            // worker-owned resource and leave thumbInit set, so the still-live
            // worker only ever touches valid memory. This trades a bounded,
            // one-popup leak for guaranteed memory safety and a non-hanging
            // shutdown (reachable only if a Shell provider never returns).
            Wh_Log(L"StopThumbThread: worker did not confirm exit within the "
                   L"bounded budget; leaking thumb resources to stay "
                   L"memory-safe (wedged Shell provider?)");
            return;
        }
    }
    // Worker confirmed gone (or there was never a worker): only NOW is it safe
    // to free the thread-owned wake event, cached bitmaps and critical section
    // it could otherwise still touch -- fix thse isseue.md P1 gates ALL of this
    // teardown on the confirmed exit above, not just the thread-handle close.
    if (d->thumbWake) {
        CloseHandle(d->thumbWake);
        d->thumbWake = nullptr;
    }
    // Worker confirmed gone: only now is it safe to tear down the thread-owned
    // critical section and the cached bitmaps it could otherwise still touch.
    EnterCriticalSection(&d->thumbCs);
    for (auto& kv : d->thumbCache)
        if (kv.second.bmp) DeleteObject(kv.second.bmp);
    d->thumbCache.clear();
    d->thumbQueueReq.clear();
    LeaveCriticalSection(&d->thumbCs);
    DeleteCriticalSection(&d->thumbCs);
    d->thumbInit = false;
}

// Paint-time lookup. Returns a cached thumbnail (+ its pixel dims) when ready;
// otherwise enqueues the request, wakes the worker and returns null so the
// caller draws the Shell icon this frame. NEVER blocks on extraction.
static HBITMAP GetThumbnailForPaint(NamePromptData* d, HWND owner,
                                    const std::wstring& path, int px,
                                    int* outW, int* outH) {
    if (!d || path.empty() || px <= 0) return nullptr;
    EnsureThumbThread(d);
    if (!d->thumbInit) return nullptr;  // worker init failed -> icon fallback;
                                        // the critical section is NOT valid, so
                                        // do not touch it (fix thse isseue.md P1).
    const std::wstring key = path + L"|" + std::to_wstring(px);
    HBITMAP result = nullptr;
    bool enqueue = false;
    EnterCriticalSection(&d->thumbCs);
    d->ownerHwnd = owner;  // publish under the SAME lock the worker reads it
                           // under (fix thse isseue.md P1: synchronize ownerHwnd).
    bool found = false;
    for (auto& kv : d->thumbCache) {
        if (kv.first == key) {
            found = true;
            if (kv.second.done && !kv.second.failed && kv.second.bmp) {
                result = kv.second.bmp;
                if (outW) *outW = kv.second.w;
                if (outH) *outH = kv.second.h;
            }
            break;
        }
    }
    if (thumbcache::ShouldEnqueue(found)) {
        // fix thse isseue.md item 7: BOUND the cache. Scrolling a large image
        // selection used to grow thumbCache (and its GDI bitmaps) without limit.
        // Before adding a new entry, if we are past the cap evict the OLDEST
        // COMPLETED entry (never an in-flight one the worker may still write --
        // we hold thumbCs here, as does the worker) and free its bitmap.
        if (thumbcache::ShouldEvict(static_cast<int>(d->thumbCache.size()) + 1,
                                    thumbcache::MaxEntries())) {
            for (size_t i = 0; i < d->thumbCache.size(); ++i) {
                if (d->thumbCache[i].second.done) {  // oldest completed entry
                    if (d->thumbCache[i].second.bmp) {
                        DeleteObject(d->thumbCache[i].second.bmp);
                    }
                    d->thumbCache.erase(d->thumbCache.begin() + i);
                    break;
                }
            }
        }
        d->thumbCache.push_back({key, NamePromptData::ThumbEntry{}});  // mark requested
        d->thumbQueueReq.push_back({path, px});
        enqueue = true;
    }
    LeaveCriticalSection(&d->thumbCs);
    if (enqueue && d->thumbWake) SetEvent(d->thumbWake);
    return result;
}

// ============================================================================
// fix thse isseue.md Issue 1: transient navigation-help card.
//
// The old help surface was an SS_OWNERDRAW child (id 122) sized to the Selected
// Items area that REPLACED the item list and toggled a pinned state. It is gone
// (see the removed WM_DRAWITEM id-122 branch). Navigation help is now a small,
// self-contained, NON-ACTIVATING popup window anchored above the footer '?'
// button. Because it is a separate top-level window (not a sibling of the list),
// the Selected Items repaint can never paint over it, and because it is
// WS_EX_NOACTIVATE it never steals focus from the folder-name edit.
//
// Open/close is the TESTED infohelp:: transient model (hover OR click opens; the
// card stays while the pointer is on the button or the card; leaving BOTH arms a
// short grace close; Esc / popup-close dismiss it). All timing is driven from the
// popup's modal pump via GetTickCount64() + infohelp::CloseDelayMs(), so it does
// not depend on the card receiving its own timer messages.
// ============================================================================
static const wchar_t kHelpCardClass[] = L"CtrlgNavHelpCard";
static const UINT_PTR kHelpPumpTimer  = 114;   // wakes the modal pump so the grace-close fires even when the pointer stops moving
static const UINT_PTR kHelpSbHideTimer = 115;  // ONE-SHOT: repaints once to hide the auto-hiding body scrollbar after the idle window (no repaint loop)

// ONE authoritative pass that both MEASURES (paint=false) and PAINTS
// (paint=true) the scrollable help content, so the measured content height and
// the drawn content can never disagree ("Overflow and Scrollable Help Card").
// Returns the total content height in device px. Section headers and dividers
// get their own typography/spacing; body lines keep their bold key spans AND
// wrap naturally -- words are placed left-to-right and wrap to the next line
// when they would cross the content right edge, so long help text never spills
// out of the card. The caller clips to the viewport and offsets by the scroll.
static int LayoutHelpContent(HDC hdc, NamePromptData* d, int contentW, bool paint,
                             int originX, int originY, const ThemePalette& pal,
                             int* sectionAnchorsOut = nullptr,
                             int* sectionCountOut = nullptr) {
    const UINT dpi = d ? d->dpi : 96u;
    auto S = [dpi](int v) { return ScaleDpi(v, dpi); };
    HFONT body    = UiFont(dpi);
    HFONT bold    = UiFontSemibold(dpi);
    HFONT section = UiFontSection(dpi);
    HFONT note    = UiFontNote(dpi);
    const int bodyLineH      = S(19);
    const int noteLineH      = S(17);
    const int sectionH       = S(24);
    const int sectionGapTop  = S(10);
    const int sectionGapBottom = S(6); // add-on section 1: 6 px between a section heading and its FIRST content line
    const int dividerPad     = S(12);  // spec section 6: 12 px above AND below the 1 px separator
    const int paraGap        = S(4);   // add-on section 1: 4-6 px between related body/bullet lines (was 3)
    const int spaceFallback  = S(4);
    const int bulletIndent   = S(16);  // hanging indent for bulleted items
    const int bulletDotR     = S(2);   // small filled bullet marker radius
    const int right = originX + contentW;
    int y = originY;
    int secIdx = 0;   // section index, for recording per-section content-Y anchors

    // Shared word-wrap for Body / Bullet / Note rows: lays runs left-to-right
    // from firstX, wrapping to lineStartX (a hanging indent for bullets) when a
    // word would cross the right edge. forceFont overrides the per-run face
    // (notes render entirely in the italic note face); otherwise each run picks
    // mono / italic / bold / body. Advances the shared y and returns it.
    auto drawRuns = [&](int lineIndex, int firstX, int lineStartX, int lineH,
                        HFONT forceFont) -> int {
        int x = firstX;
        bool anyOnLine = false;
        for (const auto& run : infohelp::ParseRuns(infohelp::Line(lineIndex))) {
            if (run.text.empty()) continue;
            HFONT f = forceFont  ? forceFont
                    : run.mono   ? UiFontMonoBold(dpi)
                    : run.italic ? UiFontItalic(dpi)
                    : run.bold   ? bold
                    :              body;
            HGDIOBJ of = SelectObject(hdc, f);
            const std::wstring& t = run.text;
            // Keyboard / inline-example token: draw the WHOLE mono run as one
            // unbreakable rounded token with a THEME-AWARE surface (never white
            // in dark mode -- helptoken:: model). forceFont rows (notes) opt out.
            if (run.mono && !forceFont) {
                SIZE tsz = {0, 0};
                GetTextExtentPoint32W(hdc, t.c_str(),
                                      static_cast<int>(t.size()), &tsz);
                const int tokPadX = S(5);
                const int boxW = tsz.cx + 2 * tokPadX;
                if (anyOnLine && x + boxW > right) {
                    x = lineStartX;
                    y += lineH;
                    anyOnLine = false;
                }
                if (paint) {
                    const helptoken::Colors tc = helptoken::ForTheme(IsDarkMode());
                    const int tokH = lineH - S(2);
                    HBRUSH tbr = CreateSolidBrush(static_cast<COLORREF>(tc.bg));
                    HPEN   tpn = CreatePen(PS_SOLID, 1,
                                           static_cast<COLORREF>(tc.border));
                    HGDIOBJ obT = SelectObject(hdc, tbr);
                    HGDIOBJ opT = SelectObject(hdc, tpn);
                    const int trad = S(4) * 2;
                    RoundRect(hdc, x, y, x + boxW, y + tokH, trad, trad);
                    SelectObject(hdc, opT);
                    SelectObject(hdc, obT);
                    DeleteObject(tbr);
                    DeleteObject(tpn);
                    SetTextColor(hdc, static_cast<COLORREF>(tc.text));
                    RECT tr = {x, y, x + boxW, y + tokH};
                    DrawTextW(hdc, t.c_str(), static_cast<int>(t.size()), &tr,
                              DT_CENTER | DT_SINGLELINE | DT_VCENTER);
                }
                x += boxW + S(3);
                anyOnLine = true;
                SelectObject(hdc, of);
                continue;
            }
            size_t p = 0;
            while (p < t.size()) {
                if (t[p] == L' ') {
                    SIZE spsz = {0, 0};
                    GetTextExtentPoint32W(hdc, L" ", 1, &spsz);
                    const int sw = spsz.cx > 0 ? spsz.cx : spaceFallback;
                    if (anyOnLine) x += sw;
                    ++p;
                    continue;
                }
                size_t q = p;
                while (q < t.size() && t[q] != L' ') ++q;
                const std::wstring word = t.substr(p, q - p);
                SIZE wsz = {0, 0};
                GetTextExtentPoint32W(hdc, word.c_str(),
                                      static_cast<int>(word.size()), &wsz);
                if (anyOnLine && x + wsz.cx > right) {
                    x = lineStartX;
                    y += lineH;
                    anyOnLine = false;
                }
                if (paint) {
                    SetTextColor(hdc, pal.panelText);
                    TextOutW(hdc, x, y, word.c_str(),
                             static_cast<int>(word.size()));
                }
                x += wsz.cx;
                anyOnLine = true;
                p = q;
            }
            SelectObject(hdc, of);
        }
        return y + lineH;
    };

    for (int i = 0; i < infohelp::LineCount(); ++i) {
        const infohelp::Kind kind = infohelp::LineKind(i);
        if (kind == infohelp::Kind::Divider) {
            if (paint) {
                HPEN dpen = CreatePen(PS_SOLID, 1, pal.cancelBorder);
                HGDIOBJ opn = SelectObject(hdc, dpen);
                const int dy = y + dividerPad;
                MoveToEx(hdc, originX, dy, nullptr);
                LineTo(hdc, right, dy);
                SelectObject(hdc, opn);
                DeleteObject(dpen);
            }
            y += dividerPad * 2 + S(1);
            continue;
        }
        if (kind == infohelp::Kind::Section) {
            // Record this section's content-Y anchor (pre-gap) so a chip click
            // can scroll it to the viewport top and active tracking can match.
            if (sectionAnchorsOut && secIdx < 8) sectionAnchorsOut[secIdx] = y - originY;
            ++secIdx;
            y += sectionGapTop;
            if (paint) {
                SetTextColor(hdc, pal.panelText);
                HGDIOBJ of = SelectObject(hdc, section);
                RECT r = {originX, y, right, y + sectionH};
                DrawTextW(hdc, infohelp::Line(i), -1, &r,
                          DT_LEFT | DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS);
                SelectObject(hdc, of);
            }
            y += sectionH;
            y += sectionGapBottom;   // add-on section 1: 6 px heading -> first content line
            continue;
        }
        if (kind == infohelp::Kind::Note) {
            // Secondary note: 11 px italic, whole line in the note face.
            y = drawRuns(i, originX, originX, noteLineH, note);
            y += paraGap;
            continue;
        }
        if (kind == infohelp::Kind::Footer) {
            // add-on section 4: the author footer is NO LONGER part of the
            // scrollable body. It is painted in a FIXED band at the card bottom
            // by PaintHelpFooter, so it is EXCLUDED from this measured content
            // height / scroll range and never scrolls out of view. Skip it here.
            continue;
        }
        if (kind == infohelp::Kind::Bullet) {
            // Bulleted item: a small filled dot marker + hanging indent so
            // wrapped lines align under the text, not under the bullet.
            const int textX = originX + bulletIndent;
            if (paint) {
                HBRUSH db = CreateSolidBrush(pal.panelText);
                HGDIOBJ ob = SelectObject(hdc, db);
                HGDIOBJ op = SelectObject(hdc, GetStockObject(NULL_PEN));
                const int cx = originX + S(5);
                const int cy = y + bodyLineH / 2;
                Ellipse(hdc, cx - bulletDotR, cy - bulletDotR,
                        cx + bulletDotR, cy + bulletDotR);
                SelectObject(hdc, op);
                SelectObject(hdc, ob);
                DeleteObject(db);
            }
            y = drawRuns(i, textX, textX, bodyLineH, nullptr);
            y += paraGap;
            continue;
        }
        // Body: 12 px, per-run faces, natural word wrap.
        y = drawRuns(i, originX, originX, bodyLineH, nullptr);
        y += paraGap;
    }
    if (sectionCountOut) *sectionCountOut = secIdx;
    return y - originY;
}

// Paint the rounded, theme-aware help card. The "Navigation help" heading is
// painted FIXED at the top; the body is drawn into a CLIPPED viewport offset by
// d->helpScrollY so it scrolls underneath the heading. A theme-aware scrollbar
// thumb is drawn in the reserved right-edge gutter ONLY when the content
// overflows (helpscroll::NeedsScroll). Double buffered so scrolling never
// flashes.
// Paint the FIXED section-navigation chip strip (fix thse isseue.md section 3)
// into [left,right) x [top,top+chipH). Measures each label, lays the chips out
// with the tested helpchips:: model, persists the strip geometry onto d (for
// click hit-testing + active tracking + horizontal scroll), clips to the inner
// width and draws each chip with ONE muted neutral surface; the active chip
// gets a 1 px accent outline (NOT a different fill) per the spec. The strip is
// horizontally scrolled by d->helpChipScrollX so an overflowing row clips and
// scrolls instead of wrapping or widening the card.
static void PaintHelpChips(HDC hdc, NamePromptData* d, const ThemePalette& pal,
                           int left, int right, int top, int chipH) {
    if (!d) return;
    const UINT dpi = d->dpi;
    auto S = [dpi](int v) { return ScaleDpi(v, dpi); };
    const int n    = helpchips::Count();
    const int hpad = S(10);
    const int gap  = S(6);
    const int rad  = S(7);
    const int visibleW = right - left;

    HGDIOBJ ofChip = SelectObject(hdc, UiFontSemibold(dpi));
    std::vector<int> widths;
    widths.reserve(n);
    for (int i = 0; i < n; ++i) {
        SIZE sz = {0, 0};
        GetTextExtentPoint32W(hdc, helpchips::Label(i),
                              lstrlenW(helpchips::Label(i)), &sz);
        widths.push_back(sz.cx + 2 * hpad);
    }
    auto rects = helpchips::Layout(widths, gap, 0);
    const int stripW = helpchips::TotalWidth(widths, gap);

    // Persist geometry + clamp the horizontal offset (hit-testing / scroll).
    d->helpChipCount      = n;
    d->helpChipStripW     = stripW;
    d->helpChipVisibleW   = visibleW;
    d->helpChipViewX      = left;
    d->helpChipBandTop    = top;
    d->helpChipBandBottom = top + chipH;
    for (int i = 0; i < n && i < 8; ++i) {
        d->helpChipLeft[i]  = rects[i].left;
        d->helpChipRight[i] = rects[i].right;
    }
    d->helpChipScrollX =
        helpchips::ClampScrollX(d->helpChipScrollX, stripW, visibleW);

    const bool dark = IsDarkMode();
    const COLORREF chipBg     = dark ? RGB(62, 62, 66) : RGB(228, 228, 232);
    const COLORREF chipBorder = dark ? RGB(86, 86, 92) : RGB(198, 198, 204);
    const COLORREF chipAccent = RGB(0, 120, 215);

    HRGN clip = CreateRectRgn(left, top, right, top + chipH);
    SelectClipRgn(hdc, clip);
    for (int i = 0; i < n; ++i) {
        const int cl = left + (rects[i].left  - d->helpChipScrollX);
        const int cr = left + (rects[i].right - d->helpChipScrollX);
        if (cr <= left || cl >= right) continue;   // fully clipped off-strip
        HBRUSH br = CreateSolidBrush(chipBg);
        HPEN   pn = CreatePen(PS_SOLID, 1, chipBorder);
        HGDIOBJ ob = SelectObject(hdc, br);
        HGDIOBJ op = SelectObject(hdc, pn);
        RoundRect(hdc, cl, top, cr, top + chipH, rad, rad);
        SelectObject(hdc, op);
        SelectObject(hdc, ob);
        DeleteObject(br);
        DeleteObject(pn);
        if (i == d->helpActiveChip) {
            // Selection = a crisp 1 px accent OUTLINE (same fill as the rest).
            HPEN apn = CreatePen(PS_SOLID, 1, chipAccent);
            HGDIOBJ aop = SelectObject(hdc, apn);
            HGDIOBJ aob = SelectObject(hdc, GetStockObject(NULL_BRUSH));
            RoundRect(hdc, cl, top, cr, top + chipH, rad, rad);
            SelectObject(hdc, aob);
            SelectObject(hdc, aop);
            DeleteObject(apn);
        }
        SetTextColor(hdc, pal.panelText);
        RECT tr = {cl, top, cr, top + chipH};
        DrawTextW(hdc, helpchips::Label(i), -1, &tr,
                  DT_CENTER | DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS);
    }
    SelectClipRgn(hdc, nullptr);
    DeleteObject(clip);
    SelectObject(hdc, ofChip);
}

// add-on section 4: the FIXED author-footer band height (DPI-scaled). The band
// reserves, from the card bottom: a ~9 px gap, a 1 px divider, ~9 px of top
// padding, one token-tall row and ~9 px of bottom padding. It is excluded from
// the scrollable body viewport so the footer never scrolls out of view.
static int HelpFooterBandH(UINT dpi) {
    auto S = [dpi](int v) { return ScaleDpi(v, dpi); };
    return S(9) + S(1) + S(9) + S(20) + S(9);
}

// Paint the FIXED author footer into [left,right) x [top,bottom). A thin 1 px
// divider sits ~9 px below the scrollable body; below it the infohelp::
// FooterText() runs render as: "Visit" in regular body text, the `Ashix`
// backtick span as the SAME theme-aware rounded key token as the shortcut
// tokens (bold mono, hover-aware) and the _Ctrl + Left-click to open_ span as a
// smaller italic hint. The Ashix token's CLIENT rect is stored on d so
// HelpCardWndProc can open the author URL on a Ctrl + left-click OR a plain
// double left-click. The band stays OUTSIDE the scroll body (section 4).
static void PaintHelpFooter(HDC hdc, NamePromptData* d, const ThemePalette& pal,
                            int left, int right, int top, int bottom) {
    if (!d) return;
    (void)bottom;
    const UINT dpi = d->dpi;
    auto S = [dpi](int v) { return ScaleDpi(v, dpi); };
    const bool dark = IsDarkMode();

    // Thin 1 px divider separating the footer from the scrollable body.
    const int divY = top + S(9);
    HPEN dpen = CreatePen(PS_SOLID, 1, pal.cancelBorder);
    HGDIOBJ opn = SelectObject(hdc, dpen);
    MoveToEx(hdc, left, divY, nullptr);
    LineTo(hdc, right, divY);
    SelectObject(hdc, opn);
    DeleteObject(dpen);

    const int lineH  = S(20);
    const int rowTop = divY + S(9);
    int x = left;

    // Reset the stored token hit rect; recomputed when the token is drawn.
    d->helpFooterLeft = d->helpFooterRight = -1;
    d->helpFooterTop  = d->helpFooterBot   = -1;

    for (const auto& run : infohelp::ParseRuns(infohelp::FooterText())) {
        if (run.text.empty()) continue;
        if (run.mono) {
            // The Ashix key token: SAME surface/padding/radius/font as the body
            // key tokens (helptoken:: model), with the restrained hover fill
            // while the pointer is over it.
            HGDIOBJ ofT = SelectObject(hdc, UiFontMonoBold(dpi));
            SIZE tsz = {0, 0};
            GetTextExtentPoint32W(hdc, run.text.c_str(),
                                  static_cast<int>(run.text.size()), &tsz);
            const int tokPadX = S(5);
            const int boxW = tsz.cx + 2 * tokPadX;
            const int tokH = lineH - S(2);
            const helptoken::Colors tc = helptoken::ForTheme(dark);
            const COLORREF fill = d->helpFooterHot
                ? static_cast<COLORREF>(tc.hover)
                : static_cast<COLORREF>(tc.bg);
            HBRUSH tbr = CreateSolidBrush(fill);
            HPEN   tpn = CreatePen(PS_SOLID, 1, static_cast<COLORREF>(tc.border));
            HGDIOBJ obT = SelectObject(hdc, tbr);
            HGDIOBJ opT = SelectObject(hdc, tpn);
            const int trad = S(4) * 2;
            RoundRect(hdc, x, rowTop, x + boxW, rowTop + tokH, trad, trad);
            SelectObject(hdc, opT);
            SelectObject(hdc, obT);
            DeleteObject(tbr);
            DeleteObject(tpn);
            SetTextColor(hdc, static_cast<COLORREF>(tc.text));
            RECT tr = {x, rowTop, x + boxW, rowTop + tokH};
            DrawTextW(hdc, run.text.c_str(), static_cast<int>(run.text.size()),
                      &tr, DT_CENTER | DT_SINGLELINE | DT_VCENTER);
            d->helpFooterLeft  = x;
            d->helpFooterRight = x + boxW;
            d->helpFooterTop   = rowTop;
            d->helpFooterBot   = rowTop + tokH;
            x += boxW + S(3);
            SelectObject(hdc, ofT);
        } else {
            // "Visit " in regular body text, or the smaller italic "Ctrl +
            // Left-click to open" hint. Advance x by the measured width (so the
            // inter-run spacing is preserved even where DrawText trims spaces).
            const bool italic = run.italic;
            HFONT f = italic ? UiFontNote(dpi) : UiFont(dpi);
            HGDIOBJ of = SelectObject(hdc, f);
            SetTextColor(hdc, italic
                ? (dark ? RGB(150, 150, 150) : RGB(110, 110, 110))
                : pal.panelText);
            SIZE wsz = {0, 0};
            GetTextExtentPoint32W(hdc, run.text.c_str(),
                                  static_cast<int>(run.text.size()), &wsz);
            RECT tr = {x, rowTop, x + wsz.cx + S(2), rowTop + lineH};
            DrawTextW(hdc, run.text.c_str(), static_cast<int>(run.text.size()),
                      &tr, DT_LEFT | DT_SINGLELINE | DT_VCENTER);
            x += wsz.cx;
            SelectObject(hdc, of);
        }
    }
}

static void PaintHelpCard(HWND hwnd, HDC targetDC, NamePromptData* d) {
    RECT rc; GetClientRect(hwnd, &rc);
    const UINT dpi = d ? d->dpi : 96u;
    auto S = [dpi](int v) { return ScaleDpi(v, dpi); };
    const int w = rc.right - rc.left;
    const int h = rc.bottom - rc.top;
    HDC memDC = CreateCompatibleDC(targetDC);
    HBITMAP bmp = memDC ? CreateCompatibleBitmap(targetDC, w, h) : nullptr;
    HGDIOBJ oldBmp = (memDC && bmp) ? SelectObject(memDC, bmp) : nullptr;
    HDC hdc = (memDC && bmp) ? memDC : targetDC;

    const ThemePalette pal = ResolveThemePalette(IsDarkMode());
    HBRUSH bgBr = CreateSolidBrush(pal.panelBg);
    FillRect(hdc, &rc, bgBr);
    DeleteObject(bgBr);

    const int rad = S(8);
    HBRUSH cardBr = CreateSolidBrush(pal.cardBg);
    HPEN   pen    = CreatePen(PS_SOLID, 1, pal.cancelBorder);
    HGDIOBJ ob = SelectObject(hdc, cardBr);
    HGDIOBJ op = SelectObject(hdc, pen);
    RoundRect(hdc, rc.left, rc.top, rc.right, rc.bottom, rad, rad);
    SelectObject(hdc, op);
    SelectObject(hdc, ob);
    DeleteObject(cardBr);
    DeleteObject(pen);

    SetBkMode(hdc, TRANSPARENT);
    const int padX          = S(16);                // spec: 16 px inner padding
    const int padTop        = S(16);
    const int headerH       = S(24);                // spec section 2: ~24 px header
    const int headerToChips = S(12);                // spec: 12 px header-to-chips
    const int chipH         = S(26);                // chip row height (5+5 vpad + text)
    const int chipToContent = S(10);                // spec: 10 px chips-to-body
    const int footerBandH   = HelpFooterBandH(dpi); // add-on section 4: fixed author-footer band at the card bottom
    const int gutterW       = S(listscroll::kScrollbarW + 4);   // REUSE the Selected Items scrollbar width
    const int headingBandH  = padTop + headerH + headerToChips + chipH + chipToContent;
    const int contentX      = rc.left + padX;

    // Fixed title "Help" -- painted before the clip, so it never scrolls away.
    SetTextColor(hdc, pal.panelText);
    HGDIOBJ ofTitle = SelectObject(hdc, UiFontSemibold(dpi));
    RECT trc = {contentX, rc.top + padTop,
                rc.right - padX, rc.top + padTop + headerH};
    DrawTextW(hdc, infohelp::Heading(), -1, &trc,
              DT_LEFT | DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS);
    SelectObject(hdc, ofTitle);

    // Fixed section-navigation chip strip directly under the header. Clipped to
    // the inner width and horizontally scrolled by helpChipScrollX so it never
    // wraps or grows the card (helpchips:: model).
    PaintHelpChips(hdc, d, pal, contentX, rc.right - padX,
                   rc.top + padTop + headerH + headerToChips, chipH);

    // Scrollable content viewport (header + chips excluded at the top, the
    // FIXED author-footer band excluded at the bottom, gutter on right).
    const int viewTop    = rc.top + headingBandH;
    const int viewBottom = rc.bottom - footerBandH;
    const int viewportH  = (viewBottom > viewTop) ? (viewBottom - viewTop) : 0;
    const int contentW   = (rc.right - padX - gutterW) - contentX;
    const int scrollY    = d ? d->helpScrollY : 0;
    if (d) { d->helpBodyViewTop = viewTop; d->helpBodyViewBottom = viewBottom; }

    HRGN clip = CreateRectRgn(contentX, viewTop, rc.right - padX, viewBottom);
    SelectClipRgn(hdc, clip);
    LayoutHelpContent(hdc, d, contentW > 0 ? contentW : 1, true,
                      contentX, viewTop - scrollY, pal);
    SelectClipRgn(hdc, nullptr);
    DeleteObject(clip);

    // Theme-aware body scrollbar thumb -- shown only when the content overflows
    // AND the auto-hide reveal window is active (fix thse isseue.md add-on
    // section 4, tested sbauto:: model). The gutter stays reserved regardless,
    // so revealing/hiding the thumb never reflows the body.
    const int contentH = d ? d->helpContentH : 0;
    const bool needsBar = helpscroll::NeedsScroll(contentH, viewportH);
    const bool sbWithinReveal = d && sbauto::WithinRevealWindow(
        d->helpSbRevealTick, GetTickCount64(), sbauto::kIdleHideMs);
    if (sbauto::Visible(needsBar, /*dragging*/false,
                        /*pointerInside*/false, sbWithinReveal)) {
        const int trackLeft  = rc.right - padX - gutterW + S(2);
        const int trackRight = rc.right - padX - S(2);
        const helpscroll::Thumb th = helpscroll::ThumbRect(
            scrollY, contentH, viewportH, viewportH, S(24));
        const COLORREF thumbCol = IsDarkMode() ? RGB(165, 165, 165)
                                               : RGB(96, 96, 96);
        HBRUSH thumbBr = CreateSolidBrush(thumbCol);
        HGDIOBJ obT = SelectObject(hdc, thumbBr);
        HGDIOBJ opT = SelectObject(hdc, GetStockObject(NULL_PEN));
        const int trad = S(3) * 2;
        RoundRect(hdc, trackLeft, viewTop + th.y,
                  trackRight, viewTop + th.y + th.h, trad, trad);
        SelectObject(hdc, opT);
        SelectObject(hdc, obT);
        DeleteObject(thumbBr);
    }

    // add-on section 4: the FIXED author footer, painted LAST into the reserved
    // bottom band (below the scrollable viewport) so it is always visible and
    // never scrolls, regardless of the active section or scroll offset.
    PaintHelpFooter(hdc, d, pal, contentX, rc.right - padX,
                    rc.bottom - footerBandH, rc.bottom);

    if (memDC && bmp) {
        BitBlt(targetDC, 0, 0, w, h, memDC, 0, 0, SRCCOPY);
        SelectObject(memDC, oldBmp);
        DeleteObject(bmp);
        DeleteDC(memDC);
    } else if (memDC) {
        DeleteDC(memDC);
    }
}

static LRESULT CALLBACK HelpCardWndProc(HWND hwnd, UINT msg, WPARAM wParam,
                                        LPARAM lParam) {
    NamePromptData* d = reinterpret_cast<NamePromptData*>(
        GetWindowLongPtrW(hwnd, GWLP_USERDATA));
    // Reveal the auto-hiding body scrollbar on interaction and (re)arm the
    // ONE-SHOT idle-hide timer (fix thse isseue.md add-on section 4). The timer
    // repaints exactly once when the window lapses -- never a repaint loop.
    auto revealSb = [&]() {
        if (!d) return;
        d->helpSbRevealTick = GetTickCount64();
        SetTimer(hwnd, kHelpSbHideTimer,
                 static_cast<UINT>(sbauto::kIdleHideMs), nullptr);
    };
    switch (msg) {
        case WM_ERASEBKGND:
            return 1;   // fully painted in WM_PAINT -- no background erase flash
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            PaintHelpCard(hwnd, hdc, d);
            EndPaint(hwnd, &ps);
            return 0;
        }
        case WM_MOUSEWHEEL: {
            revealSb();
            // A wheel delivered directly to the card (e.g. "scroll inactive
            // windows under the pointer") scrolls ONLY the help body via the
            // tested helpscroll model and is consumed -- never reflowed anywhere
            // else. Shift+wheel instead pans an OVERFLOWING chip strip. After a
            // body scroll the active chip + its visibility are recomputed so the
            // selected chip follows the primary visible section (helpchips::).
            if (d) {
                const int delta =
                    static_cast<int>(static_cast<short>(HIWORD(wParam)));
                const bool shift = (LOWORD(wParam) & MK_SHIFT) != 0;
                const int step = ScaleDpi(40, d->dpi);
                if (shift && d->helpChipStripW > d->helpChipVisibleW) {
                    const int nx = helpchips::ClampScrollX(
                        d->helpChipScrollX - (delta / 120) * step,
                        d->helpChipStripW, d->helpChipVisibleW);
                    if (nx != d->helpChipScrollX) {
                        d->helpChipScrollX = nx;
                        InvalidateRect(hwnd, nullptr, FALSE);
                    }
                    return 0;
                }
                const int maxScroll =
                    helpscroll::MaxScroll(d->helpContentH, d->helpViewportH);
                const int ny = helpscroll::OnWheel(d->helpScrollY, delta,
                                                   step, maxScroll);
                if (ny != d->helpScrollY) {
                    d->helpScrollY = ny;
                    d->helpActiveChip = helpchips::ActiveSection(
                        std::vector<int>(d->helpSectionY,
                                         d->helpSectionY + d->helpSectionCount),
                        d->helpScrollY, d->helpViewportH);
                    std::vector<helpchips::Rect> cr;
                    for (int i = 0; i < d->helpChipCount && i < 8; ++i)
                        cr.push_back(helpchips::Rect{d->helpChipLeft[i],
                                                     d->helpChipRight[i]});
                    d->helpChipScrollX = helpchips::EnsureVisible(
                        cr, d->helpActiveChip, d->helpChipScrollX,
                        d->helpChipVisibleW, d->helpChipStripW);
                    InvalidateRect(hwnd, nullptr, FALSE);
                }
            }
            return 0;
        }
        case WM_MOUSEHWHEEL: {
            revealSb();
            // A horizontal wheel pans an overflowing chip strip (never the body).
            if (d && d->helpChipStripW > d->helpChipVisibleW) {
                const int delta =
                    static_cast<int>(static_cast<short>(HIWORD(wParam)));
                const int step = ScaleDpi(40, d->dpi);
                const int nx = helpchips::ClampScrollX(
                    d->helpChipScrollX + (delta / 120) * step,
                    d->helpChipStripW, d->helpChipVisibleW);
                if (nx != d->helpChipScrollX) {
                    d->helpChipScrollX = nx;
                    InvalidateRect(hwnd, nullptr, FALSE);
                }
            }
            return 0;
        }
        case WM_MOUSEMOVE: {
            // Pointer interaction reveals the auto-hiding scrollbar; the idle
            // timer hides it again a short while after movement stops.
            revealSb();
            // add-on section 2: hover-highlight the interactive Ashix footer
            // token while the pointer is over it (repaint only on state change).
            if (d && d->helpFooterLeft >= 0) {
                const int px = static_cast<int>(static_cast<short>(LOWORD(lParam)));
                const int py = static_cast<int>(static_cast<short>(HIWORD(lParam)));
                const bool hot = helpfooter::InsideFooter(
                    px, py, d->helpFooterLeft, d->helpFooterRight,
                    d->helpFooterTop, d->helpFooterBot);
                if (hot != d->helpFooterHot) {
                    d->helpFooterHot = hot;
                    InvalidateRect(hwnd, nullptr, FALSE);
                }
            }
            return 0;
        }
        case WM_TIMER:
            if (wParam == kHelpSbHideTimer) {
                KillTimer(hwnd, kHelpSbHideTimer);
                // Repaint EXACTLY ONCE to hide the thumb now the reveal window
                // has lapsed (no repaint loop; the gutter stays reserved).
                if (d && !sbauto::WithinRevealWindow(
                             d->helpSbRevealTick, GetTickCount64(),
                             sbauto::kIdleHideMs))
                    InvalidateRect(hwnd, nullptr, FALSE);
                return 0;
            }
            break;
        case WM_LBUTTONDOWN: {
            revealSb();
            // A click on a section chip jumps the body to that section and marks
            // the chip active (helpchips:: model). The card is a SEPARATE popup
            // and the message is consumed, so this never scrolls the Selected
            // Items list behind it.
            if (d) {
                const int px = static_cast<int>(static_cast<short>(LOWORD(lParam)));
                const int py = static_cast<int>(static_cast<short>(HIWORD(lParam)));
                if (py >= d->helpChipBandTop && py < d->helpChipBandBottom &&
                    d->helpChipCount > 0) {
                    const int contentXClick =
                        (px - d->helpChipViewX) + d->helpChipScrollX;
                    std::vector<helpchips::Rect> cr;
                    for (int i = 0; i < d->helpChipCount && i < 8; ++i)
                        cr.push_back(helpchips::Rect{d->helpChipLeft[i],
                                                     d->helpChipRight[i]});
                    const int chip = helpchips::HitTest(cr, contentXClick);
                    if (chip >= 0) {
                        std::vector<int> anchors(
                            d->helpSectionY,
                            d->helpSectionY + d->helpSectionCount);
                        const int maxScrollY = helpscroll::MaxScroll(
                            d->helpContentH, d->helpViewportH);
                        d->helpScrollY =
                            helpchips::ScrollTargetFor(anchors, chip, maxScrollY);
                        d->helpActiveChip = chip;
                        d->helpChipScrollX = helpchips::EnsureVisible(
                            cr, chip, d->helpChipScrollX,
                            d->helpChipVisibleW, d->helpChipStripW);
                        InvalidateRect(hwnd, nullptr, FALSE);
                    }
                }
                // add-on section 4: the author footer is a FIXED band at the
                // card bottom (always visible). Open the k-ashix profile ONLY on
                // a Ctrl + left-click directly on the Ashix TOKEN -- its client
                // rect is stored by PaintHelpFooter. A plain single click never
                // navigates (helpfooter::ShouldOpen).
                if (d->helpFooterLeft >= 0 &&
                    d->helpFooterRight > d->helpFooterLeft) {
                    const bool ctrl = (GetKeyState(VK_CONTROL) & 0x8000) != 0;
                    const bool inTok = helpfooter::InsideFooter(
                        px, py, d->helpFooterLeft, d->helpFooterRight,
                        d->helpFooterTop, d->helpFooterBot);
                    if (helpfooter::ShouldOpen(ctrl, true, inTok)) {
                        ShellExecuteW(nullptr, L"open", helpfooter::Url(),
                                      nullptr, nullptr, SW_SHOWNORMAL);
                    }
                }
            }
            return 0;
        }
        case WM_LBUTTONDBLCLK: {
            revealSb();
            // add-on section 4: a plain DOUBLE left-click on the Ashix token
            // ALSO opens the author profile (no Ctrl required on this path);
            // helpfooter::ShouldOpenDouble gates it to the token rect.
            if (d && d->helpFooterLeft >= 0 &&
                d->helpFooterRight > d->helpFooterLeft) {
                const int px = static_cast<int>(static_cast<short>(LOWORD(lParam)));
                const int py = static_cast<int>(static_cast<short>(HIWORD(lParam)));
                const bool inTok = helpfooter::InsideFooter(
                    px, py, d->helpFooterLeft, d->helpFooterRight,
                    d->helpFooterTop, d->helpFooterBot);
                if (helpfooter::ShouldOpenDouble(true, inTok)) {
                    ShellExecuteW(nullptr, L"open", helpfooter::Url(),
                                  nullptr, nullptr, SW_SHOWNORMAL);
                }
            }
            return 0;
        }
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

// Compact, capped size + scroll bookkeeping ("Overflow and Scrollable Help
// Card"). Width is the widest single line clamped to a compact band; both
// dimensions are then capped against the CURRENT monitor work area (DPI-aware
// -- all values are device px) via helpscroll::CapToWorkArea so the card never
// grows past the screen as content is added. The content height is measured at
// the final width (natural wrapping) with the SAME LayoutHelpContent used to
// paint, the card height is sized from it and capped, and the resulting content
// viewport height + scroll offset are stored on d so the painter and the wheel
// handler share one source of truth.
static SIZE MeasureHelpCard(HWND parent, NamePromptData* d) {
    const UINT dpi = d ? d->dpi : 96u;
    auto S = [dpi](int v) { return ScaleDpi(v, dpi); };
    const int padX        = S(16);   // spec: 16 px inner padding
    const int padTop      = S(16);
    const int headerH     = S(24);                 // spec section 2: ~24 px header
    const int headerToChips = S(12);               // spec: 12 px header-to-chips
    const int chipH       = S(26);                 // chip row height (matches PaintHelpCard)
    const int chipToContent = S(10);               // spec: 10 px chips-to-body
    const int footerBandH = HelpFooterBandH(dpi);  // add-on section 4: fixed author-footer band (excluded from the body viewport/scroll range)
    const int gutterW     = S(listscroll::kScrollbarW + 4);   // REUSE the Selected Items scrollbar width
    const int headingBandH = padTop + headerH + headerToChips + chipH + chipToContent;
    const int minW        = S(240);
    const int maxCardW    = S(376);   // nav-help spec (fix thse isseue.md add-on section 3): 360 + 16 = 376 logical px wide at 96 DPI; the extra width reduces wrapping and the body never widens past it
    const int maxCardH    = S(340);   // nav-help spec (fix thse isseue.md add-on section 3): 320 + 20 = 340 logical px MAX height at 96 DPI; taller content scrolls INTERNALLY, the card never grows past this
    const int margin      = S(16);

    HDC hdc = GetDC(parent);
    HFONT body = UiFont(dpi);
    HFONT bold = UiFontSemibold(dpi);

    int maxW = 0;
    {
        HGDIOBJ of = SelectObject(hdc, bold);
        SIZE sz = {0, 0};
        GetTextExtentPoint32W(hdc, infohelp::Heading(),
                              lstrlenW(infohelp::Heading()), &sz);
        if (sz.cx > maxW) maxW = sz.cx;
        SelectObject(hdc, of);
    }
    for (int i = 0; i < infohelp::LineCount(); ++i) {
        if (infohelp::LineKind(i) == infohelp::Kind::Divider) continue;
        const bool section = (infohelp::LineKind(i) == infohelp::Kind::Section);
        int lineW = 0;
        for (const auto& run : infohelp::ParseRuns(infohelp::Line(i))) {
            // Measure each run in the SAME face the renderer will draw it with
            // (mono / italic / bold / body) so the card width accounts for them.
            HFONT mf = run.mono   ? UiFontMonoBold(dpi)
                     : run.italic ? UiFontItalic(dpi)
                     : (run.bold || section) ? bold
                     :              body;
            HGDIOBJ of = SelectObject(hdc, mf);
            SIZE sz = {0, 0};
            GetTextExtentPoint32W(hdc, run.text.c_str(),
                                  static_cast<int>(run.text.size()), &sz);
            lineW += sz.cx;
            SelectObject(hdc, of);
        }
        if (lineW > maxW) maxW = lineW;
    }

    SIZE out;
    out.cx = maxW + 2 * padX + gutterW;
    if (out.cx < minW) out.cx = minW;
    out.cx = helpscroll::CapToMax(out.cx, maxCardW);

    HMONITOR mon = MonitorFromWindow(parent, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi = {}; mi.cbSize = sizeof(mi);
    int workW = 0, workH = 0;
    if (GetMonitorInfoW(mon, &mi)) {
        workW = static_cast<int>(mi.rcWork.right - mi.rcWork.left);
        workH = static_cast<int>(mi.rcWork.bottom - mi.rcWork.top);
    }
    if (workW > 0) out.cx = helpscroll::CapToWorkArea(out.cx, workW, margin);

    const int contentW = (out.cx - 2 * padX - gutterW) > 0
                       ? (out.cx - 2 * padX - gutterW) : 1;
    const ThemePalette pal = ResolveThemePalette(IsDarkMode());
    const int contentH =
        LayoutHelpContent(hdc, d, contentW, false, 0, 0, pal,
                          d ? d->helpSectionY : nullptr,
                          d ? &d->helpSectionCount : nullptr);
    ReleaseDC(parent, hdc);

    const int desiredH = headingBandH + contentH + footerBandH;
    // fix thse isseue.md P1-4: bound the card to the FIXED maximum FIRST, then
    // to the monitor work area (the SMALLER of the two always wins), so the card
    // has a consistent configured maximum and never grows with its content.
    // Content taller than the resulting viewport scrolls INTERNALLY (helpViewportH
    // / NeedsScroll below) instead of enlarging the card.
    int cardH = helpscroll::CapToMax(desiredH, maxCardH);
    if (workH > 0) cardH = helpscroll::CapToWorkArea(cardH, workH, margin);
    // fix thse isseue.md add-on section 2: keep a practical MINIMUM height (180
    // logical px) so a short card still reads as a card. The floor is itself
    // clamped to the work area ("subject to available space") so it is never
    // pushed off-screen on a tiny monitor.
    int minCardH = S(180);
    if (workH > 0) minCardH = helpscroll::CapToWorkArea(minCardH, workH, margin);
    cardH = helpscroll::CapToMin(cardH, minCardH);
    out.cy = cardH;

    if (d) {
        d->helpContentH  = contentH;
        d->helpViewportH = helpscroll::ViewportHeight(cardH, headingBandH, footerBandH);
        d->helpScrollY   = helpscroll::ClampScroll(
            d->helpScrollY, helpscroll::MaxScroll(contentH, d->helpViewportH));
        // Keep the active chip in sync with the (possibly re-clamped) scroll.
        d->helpActiveChip = helpchips::ActiveSection(
            std::vector<int>(d->helpSectionY, d->helpSectionY + d->helpSectionCount),
            d->helpScrollY, d->helpViewportH);
    }
    return out;
}

static void EnsureHelpCardClass(HINSTANCE hinst) {
    static bool reg = false;
    if (reg) return;
    WNDCLASSEXW wc = {};
    wc.cbSize = sizeof(wc);
    wc.style = CS_DBLCLKS;        // add-on section 4: deliver WM_LBUTTONDBLCLK so the Ashix footer token opens on a double-click
    wc.lpfnWndProc = HelpCardWndProc;
    wc.hInstance = hinst;
    wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    wc.hbrBackground = nullptr;   // painted fully in WM_PAINT
    wc.lpszClassName = kHelpCardClass;
    // fix thse isseue.md P2 (8929-8941): only mark the class registered when it
    // is actually usable. Previously `reg` was set unconditionally, so a failed
    // RegisterClassExW() (other than "already exists") permanently blocked every
    // retry and left EnsureHelpCard() creating windows for a missing class.
    if (RegisterClassExW(&wc) != 0) {
        reg = true;
        return;
    }
    const DWORD err = GetLastError();
    if (helpclass::RegistrationUsable(/*registered*/false, err)) {
        reg = true;  // benign: the class already exists from a prior call
    } else {
        Wh_Log(L"EnsureHelpCardClass: RegisterClassExW failed (error=%lu); "
               L"will retry on next use", err);
    }
}

static void EnsureHelpCard(HWND parent, NamePromptData* d) {
    if (!d || d->helpCard) return;
    HINSTANCE hinst = reinterpret_cast<HINSTANCE>(
        GetWindowLongPtrW(parent, GWLP_HINSTANCE));
    EnsureHelpCardClass(hinst);
    d->helpCard = CreateWindowExW(
        WS_EX_NOACTIVATE | WS_EX_TOOLWINDOW | WS_EX_TOPMOST,
        kHelpCardClass, L"", WS_POPUP,
        0, 0, 10, 10, parent, nullptr, hinst, nullptr);
    if (d->helpCard)
        SetWindowLongPtrW(d->helpCard, GWLP_USERDATA,
                          reinterpret_cast<LONG_PTR>(d));
}

// Anchor the card ABOVE the '?' button, clamped to the monitor work area (fall
// below the button only if there is not enough room above). Called whenever the
// popup moves/resizes/maximizes or the DPI changes, so the card tracks it.
static void PositionHelpCard(HWND parent, NamePromptData* d) {
    if (!d || !d->helpCard || !d->infoBtn) return;
    SIZE sz = MeasureHelpCard(parent, d);
    RECT btn; GetWindowRect(d->infoBtn, &btn);   // screen coords
    const int gap = ScaleDpi(6, d->dpi);
    int x = btn.left;
    int y = btn.top - gap - sz.cy;                // prefer above the button
    HMONITOR mon = MonitorFromWindow(parent, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi = {}; mi.cbSize = sizeof(mi);
    if (GetMonitorInfoW(mon, &mi)) {
        if (x + sz.cx > mi.rcWork.right)  x = mi.rcWork.right - sz.cx;
        if (x < mi.rcWork.left)           x = mi.rcWork.left;
        if (y < mi.rcWork.top)            y = btn.bottom + gap;   // not enough room above
        if (y + sz.cy > mi.rcWork.bottom) y = mi.rcWork.bottom - sz.cy;
    }
    SetWindowPos(d->helpCard, HWND_TOPMOST, x, y, sz.cx, sz.cy, SWP_NOACTIVATE);
}

static void ShowHelpCard(HWND parent, NamePromptData* d) {
    if (!d || !d->isGroup) return;
    EnsureHelpCard(parent, d);
    if (!d->helpCard) return;
    d->helpVis = static_cast<int>(infohelp::Vis::Shown);
    d->helpCloseAtTick = 0;
    d->helpScrollY = 0;                           // always open scrolled to the top
    PositionHelpCard(parent, d);
    ShowWindow(d->helpCard, SW_SHOWNOACTIVATE);   // never steals folder-name focus
    InvalidateRect(d->helpCard, nullptr, FALSE);
    SetTimer(parent, kHelpPumpTimer, 60, nullptr);  // drive the grace-close poll
    // Briefly reveal the auto-hiding body scrollbar on open, then let the
    // one-shot timer hide it after the idle window (add-on section 4).
    d->helpSbRevealTick = GetTickCount64();
    SetTimer(d->helpCard, kHelpSbHideTimer,
             static_cast<UINT>(sbauto::kIdleHideMs), nullptr);
}

static void HideHelpCard(HWND parent, NamePromptData* d) {
    if (!d) return;
    d->helpVis = static_cast<int>(infohelp::Vis::Hidden);
    d->helpCloseAtTick = 0;
    if (d->helpCard) {
        ShowWindow(d->helpCard, SW_HIDE);
        KillTimer(d->helpCard, kHelpSbHideTimer);   // stop the idle-hide one-shot
    }
    KillTimer(parent, kHelpPumpTimer);
}

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
            const UINT dpi = d ? d->dpi : 96;
            auto S = [dpi](int v) { return ScaleDpi(v, dpi); };
            // Spacing system (UI spec item 9): one layout, not scattered
            // coordinates. Design-space rects come from the mirrored
            // layout::ComputePromptLayout and are DPI-scaled per control via S().
            layout::PromptLayout L = layout::ComputePromptLayout(d && d->isGroup,
                                                                  d && d->expanded,
                                                                  d ? static_cast<int>(d->items.size()) : 0);

            // Group popups grow a header block ABOVE the New Folder layout:
            // count line, item list, destination + last-target lines, and a
            // Move/Copy toggle. ComputePromptLayout accounts for that band; for
            // a New Folder popup the group-only rects are empty and the form
            // keeps its original coordinates (New Folder is unchanged).
            HFONT font = UiFont(dpi);
            HWND groupInfo = nullptr;   // Phase 5/C: SS_OWNERDRAW list (id 110)
            if (d && d->isGroup) {
                // Phase 5/C: an SS_OWNERDRAW list (id 110), NOT a text STATIC.
                // WM_DRAWITEM paints title + counts + separator + the visible
                // rows (name | EXT | SIZE) from the tested sel:: / listscroll::
                // models; scrolling just repaints the new slice, so this is a
                // real structured list instead of one multi-line text blob.
                groupInfo = CreateWindowExW(0, L"STATIC", L"",
                                WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | SS_OWNERDRAW,
                                S(L.selectedContent.x), S(L.selectedContent.y), S(L.selectedContent.w), S(L.selectedContent.h),
                                hwnd, reinterpret_cast<HMENU>(static_cast<LONG_PTR>(110)), hinst, nullptr);
                SendMessageW(groupInfo, WM_SETFONT,
                             reinterpret_cast<WPARAM>(font), TRUE);
                d->groupInfo = groupInfo;   // scroll refresh target (Phase 2)
                // fix thse isseue.md P2 (10249-10250): give the custom-drawn
                // Show-extensions switch a REAL, state-aware accessible name via
                // a tooltip on this list (where it is painted + hit-tested).
                EnsureExtToggleTip(d, groupInfo, hinst);

                // Compact/Expanded size toggle button (#11, Phase 7): a small
                // owner-neutral control in the header's top-right corner. Click
                // flips the mode (same path as Ctrl+E, handled in WM_COMMAND id
                // 103); its glyph reflects the current mode. Group only.
                // fix thse isseu e.md #5: place the size control in the window
                // CHROME strip (tested layout::chromeSizeBtn), right-aligned and
                // ABOVE the first card, with BS_FLAT so it reads as a window
                // affordance rather than a push-button inside the card.
                // fix thse isseue.md E: the three caption buttons are PLAIN
                // (non-layered) owner-draw children, created with
                // CreateWindowExW(0, ...). An earlier WS_EX_LAYERED +
                // LWA_COLORKEY(black) approach was click-through on the acrylic
                // band (presses fell through to the drag-caption), so it was
                // removed -- there is NO SetLayeredWindowAttributes call. The
                // owner-draw below lays an opaque themed base fill so the whole
                // control is hit-testable (see the note after creation).
                const layout::Rect cb = L.chromeSizeBtn;
                d->sizeToggle = CreateWindowExW(0, L"BUTTON", L"",
                                WS_CHILD | WS_VISIBLE | WS_TABSTOP |
                                    BS_OWNERDRAW | BS_FLAT,
                                S(cb.x), S(cb.y), S(cb.w), S(cb.h), hwnd,
                                reinterpret_cast<HMENU>(103), hinst, nullptr);
                SendMessageW(d->sizeToggle, WM_SETFONT,
                             reinterpret_cast<WPARAM>(font), TRUE);
                // fix thse isseue.md #1/#2: Minimize (104) + Close (105) live in
                // the custom title bar beside Expand (same flat owner-draw chrome
                // style), at the tested layout::chromeMinBtn / chromeCloseBtn rects.
                const layout::Rect cbMin = L.chromeMinBtn;
                const layout::Rect cbClose = L.chromeCloseBtn;
                d->minBtn = CreateWindowExW(0, L"BUTTON", L"",
                                WS_CHILD | WS_VISIBLE | WS_TABSTOP |
                                    BS_OWNERDRAW | BS_FLAT,
                                S(cbMin.x), S(cbMin.y), S(cbMin.w), S(cbMin.h),
                                hwnd, reinterpret_cast<HMENU>(104), hinst, nullptr);
                SendMessageW(d->minBtn, WM_SETFONT,
                             reinterpret_cast<WPARAM>(font), TRUE);
                d->closeBtn = CreateWindowExW(0, L"BUTTON", L"",
                                WS_CHILD | WS_VISIBLE | WS_TABSTOP |
                                    BS_OWNERDRAW | BS_FLAT,
                                S(cbClose.x), S(cbClose.y), S(cbClose.w), S(cbClose.h),
                                hwnd, reinterpret_cast<HMENU>(105), hinst, nullptr);
                SendMessageW(d->closeBtn, WM_SETFONT,
                             reinterpret_cast<WPARAM>(font), TRUE);
                // fix thse isseue.md: the caption buttons are PLAIN (non-layered)
                // owner-draw children. A WS_EX_LAYERED + LWA_COLORKEY(black) button
                // was click-through on the acrylic band everywhere except its thin
                // glyph, so a press missed the button, fell through to the drag-
                // caption and started a window-move loop -- clicks were swallowed
                // and the window felt "stuck". An opaque themed base fill (in the
                // owner-draw below) makes the WHOLE button hit-testable; the
                // hover/pressed/red-Close fills paint over it.

                // Destination card (Phase 3b): its OWN label + read-only field
                // placed by the layout model's dest rects, instead of a text
                // line buried in the Selected Items blob. Informational only --
                // no destination-selection behaviour is added here.
                d->destLabel = CreateWindowExW(0, L"STATIC", dest::SectionLabel(),
                                WS_CHILD | WS_VISIBLE,
                                S(L.destLabel.x), S(L.destLabel.y),
                                S(L.destLabel.w), S(L.destLabel.h),
                                hwnd, nullptr, hinst, nullptr);
                SendMessageW(d->destLabel, WM_SETFONT,
                             reinterpret_cast<WPARAM>(font), TRUE);
                // fix thse isseu e.md #2: a proper read-only INFORMATION field.
                // Not focusable / no caret (informational only), a folder glyph
                // painted in the left icon cell, and the path text inset past it
                // via destfield:: so text never overlaps the glyph. Ellipsis for
                // long paths comes from the tested destfield:: policy.
                // fix thse isseue.md #4: the EDIT is INSET inside its frame rect
                // (L.destField) via the reusable inputfield helper, so its square
                // corners stay off the parent-painted rounded border/edge.
                const auto destEdit = inputfield::InnerEditRect(
                    L.destField.x, L.destField.y, L.destField.w, L.destField.h);
                d->destField = CreateWindowExW(0, L"EDIT",
                                GroupDestinationFieldText(d->destPath,
                                                          d->effectiveDest,
                                                          d->lastTarget).c_str(),
                                WS_CHILD | WS_VISIBLE | ES_READONLY | ES_AUTOHSCROLL |
                                    (destfield::NoKeyboardFocus() ? WS_DISABLED : 0),
                                S(destEdit.x), S(destEdit.y),
                                S(destEdit.w), S(destEdit.h),
                                hwnd, nullptr, hinst, nullptr);
                SendMessageW(d->destField, WM_SETFONT,
                             reinterpret_cast<WPARAM>(font), TRUE);
                // fix thse isseue.md P0 #2: NO HRGN region clipping on the
                // Destination EDIT. A clipped region gives hard-edged (non-anti-
                // aliased) rounded corners -- the exact artifact the review
                // flagged. The field stays a plain rectangle; the parent paints
                // the rounded field surface + border around it (WM_ERASEBKGND)
                // and the EDIT bg matches that surface, so it reads as one
                // rounded field with no clipping.
                // fix thse isseue.md #2 (path pass): "no icon path" -- the left
                // margin is now just the edge pad (destfield::TextLeftInset()),
                // so the path text starts right at the field edge, shown plainly.
                SendMessageW(d->destField, EM_SETMARGINS,
                             EC_LEFTMARGIN | EC_RIGHTMARGIN,
                             MAKELPARAM(S(destfield::TextLeftInset()), S(destfield::kEdgePad)));
                DisableWindowTheme(d->destField);
                // fix thse isseue.md #2 (path pass): the subclass is retained as
                // the field's native-EDIT hook but no longer paints a folder
                // glyph ("no icon path"). (fix thse isseue.md P2: the unused
                // per-field DPI "symmetry" global was removed -- it was written
                // here and never read.)
                g_origDestFieldProc = reinterpret_cast<WNDPROC>(SetWindowLongPtrW(
                    d->destField, GWLP_WNDPROC,
                    reinterpret_cast<LONG_PTR>(DestFieldSubclassProc)));
                // fix thse isseue.md #6: show the resolved path ellipsized to
                // the field width (destfield::UsePathEllipsis()).
                ApplyDestFieldText(d->destField, d->destPath, d->effectiveDest, d->lastTarget, dpi);

                // fix thse isseue.md #2 (path pass): a compact "Copy" button to
                // the RIGHT of the (now icon-less) path field copies the full
                // destination path to the clipboard. id 111 -> WM_COMMAND below.
                // BS_OWNERDRAW so it is painted by the SAME themed button
                // renderer as Create/Cancel (theme-aware, rounded, hover/focus),
                // using the secondary ("Cancel") styling -- see WM_DRAWITEM.
                d->destCopyBtn = CreateWindowExW(0, L"BUTTON", L"Copy",
                                WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_OWNERDRAW,
                                S(L.destCopyBtn.x), S(L.destCopyBtn.y),
                                S(L.destCopyBtn.w), S(L.destCopyBtn.h),
                                hwnd, reinterpret_cast<HMENU>(111), hinst, nullptr);
                SendMessageW(d->destCopyBtn, WM_SETFONT,
                             reinterpret_cast<WPARAM>(font), TRUE);

                // fix thse isseue.md Issue 1: a REAL footer '?' navigation-help
                // button (id 121), painted GREEN with a bold white "?" by the
                // themed BS_OWNERDRAW renderer. Its window text is the accessible
                // name "Navigation help" (the glyph is drawn separately). It no
                // longer owns a large child overlay: the help itself is a SEPARATE
                // non-activating popup window (d->helpCard, class kHelpCardClass)
                // created on demand by EnsureHelpCard(), so the Selected Items
                // list repaint can never paint over it. Hover OR click opens the
                // card through the tested infohelp:: transient model (NO pin).
                d->infoBtn = CreateWindowExW(0, L"BUTTON", L"?",
                                WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_OWNERDRAW,
                                S(L.infoButton.x), S(L.infoButton.y),
                                S(L.infoButton.w), S(L.infoButton.h),
                                hwnd, reinterpret_cast<HMENU>(121), hinst, nullptr);
                SendMessageW(d->infoBtn, WM_SETFONT,
                             reinterpret_cast<WPARAM>(font), TRUE);
                // Accessible name for screen readers / tooltips (the owner-draw
                // paints the "?" glyph, not this text).
                SetWindowTextW(d->infoBtn, infohelp::AccessibleName());

                // fix thse isseue.md #1: the Status card (state icon + label +
                // progress track) was removed; the Group popup no longer has a
                // status section and the operation outcome is logged by the worker.
            }

            // Phase 7/E: reworded via the tested seclbl:: model -- the group
            // popup uses the lighter "Folder name" title (helper text is drawn
            // under it in WM_DRAWITEM / paint), New Folder keeps its own label.
            const wchar_t* nameLabelText =
                (d && d->isGroup) ? seclbl::NameLabel()
                                  : seclbl::NewFolderNameLabel();
            HWND label = CreateWindowExW(0, L"STATIC",
                            nameLabelText,
                            WS_CHILD | WS_VISIBLE,
                            S(L.nameLabel.x), S(L.nameLabel.y), S(L.nameLabel.w), S(L.nameLabel.h), hwnd, nullptr, hinst, nullptr);
            if (d) {
                d->nameLabel = label;          // Phase 3: also carries inline validation
                d->origNameLabel = nameLabelText;  // restored when the message clears (#6/#9)
            }
            // fix thse isseue.md #4: the Folder-name / New Folder EDIT is INSET
            // inside its frame rect (L.nameField) via the reusable inputfield
            // helper, so its square corners stay off the rounded frame border.
            const auto nameEdit = inputfield::InnerEditRect(
                L.nameField.x, L.nameField.y, L.nameField.w, L.nameField.h);
            d->edit = CreateWindowExW(
                0, L"EDIT", L"",
                WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_AUTOHSCROLL,
                S(nameEdit.x), S(nameEdit.y), S(nameEdit.w), S(nameEdit.h), hwnd,
                reinterpret_cast<HMENU>(101), hinst, nullptr);

            // Copy | Move segmented control (Phase 6/D): TWO coordinated
            // owner-drawn segments (ids 107 Copy, 108 Move) instead of the old
            // single toggle. Clicking a segment selects EXACTLY that segment
            // (segmented::OnClickSegment); arrows move Copy<->Move only while a
            // segment owns focus (segmented::OnKey). d->isMove stays the single
            // logical state the worker reads (Copy default). The two segments
            // sit at the tested layout rects L.segCopy / L.segMove so mouse and
            // keyboard drive the same state the reference shows.
            if (d && d->isGroup) {
                d->groupOpToggle = nullptr;  // retired -- segments own the visual now
                d->segCopyBtn = CreateWindowExW(0, L"BUTTON",
                                segmented::SegmentLabel(segmented::Seg::Copy).c_str(),
                                WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_OWNERDRAW,
                                S(L.segCopy.x), S(L.segCopy.y), S(L.segCopy.w), S(L.segCopy.h), hwnd,
                                reinterpret_cast<HMENU>(107), hinst, nullptr);
                d->segMoveBtn = CreateWindowExW(0, L"BUTTON",
                                segmented::SegmentLabel(segmented::Seg::Move).c_str(),
                                WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_OWNERDRAW,
                                S(L.segMove.x), S(L.segMove.y), S(L.segMove.w), S(L.segMove.h), hwnd,
                                reinterpret_cast<HMENU>(108), hinst, nullptr);
                SendMessageW(d->segCopyBtn, WM_SETFONT,
                             reinterpret_cast<WPARAM>(font), TRUE);
                SendMessageW(d->segMoveBtn, WM_SETFONT,
                             reinterpret_cast<WPARAM>(font), TRUE);
                // fix thse isseue.md #3: the Copy/Move segments are NO longer
                // region-clipped. Their owner-draw (WM_DRAWITEM 107/108) now
                // paints a matching card-bg fill then a left/right half-pill
                // RoundRect, so the two meet at a straight seam and read as one
                // pill -- no HRGN clipping, no corner/seam artifact.
            }

            // fix thse isseue.md #4: the primary button says "Group" in the
            // Ctrl+G group popup and "Create" in the New Folder popup. Same
            // IDOK command path, validation and Enter behaviour -- only the
            // caption differs, via the tested seclbl::SubmitLabel model. The
            // owner-draw (WM_DRAWITEM) measures the LIVE GetWindowTextW, so the
            // shorter "Group" caption re-centres automatically.
            HWND ok = CreateWindowExW(0, L"BUTTON", seclbl::SubmitLabel(d && d->isGroup),
                            WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_OWNERDRAW,
                            S(L.createButton.x), S(L.createButton.y), S(L.createButton.w), S(L.createButton.h), hwnd,
                            reinterpret_cast<HMENU>(IDOK), hinst, nullptr);
            HWND cancel = CreateWindowExW(0, L"BUTTON", L"Cancel",
                            WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_OWNERDRAW,
                            S(L.cancelButton.x), S(L.cancelButton.y), S(L.cancelButton.w), S(L.cancelButton.h), hwnd,
                            reinterpret_cast<HMENU>(IDCANCEL), hinst, nullptr);
            if (d) { d->createBtn = ok; d->cancelBtn = cancel; }  // Phase 7 relayout targets

            // ADD-ON (Phase D): the timed Auto Group button, LEFT of Group, is
            // created ONLY for a Group popup and ONLY when enabled in settings.
            // Owner-draw id 120, same style as Create/Cancel. A fresh monotonic
            // start tick (GetTickCount64) is captured per popup, and a repaint
            // timer (id 112, ~100ms) refreshes the once-per-second M:SS
            // countdown. The countdown merely ENABLES the button; it never runs
            // the op.
            if (d && d->isGroup && AtomicGet(&g_autoGroupEnabled) != 0) {
                HWND autoBtn = CreateWindowExW(0, L"BUTTON", L"Auto Group",
                                WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_OWNERDRAW,
                                S(L.autoGroupButton.x), S(L.autoGroupButton.y),
                                S(L.autoGroupButton.w), S(L.autoGroupButton.h), hwnd,
                                reinterpret_cast<HMENU>(120), hinst, nullptr);
                d->autoGroupBtn = autoBtn;
                d->autoGroupVisible = (autoBtn != nullptr);
                d->autoGroupStartTick = GetTickCount64();
                if (autoBtn) {
                    SendMessageW(autoBtn, WM_SETFONT, reinterpret_cast<WPARAM>(font), TRUE);
                    // fix thse isseue.md #5: the button opens in the Waiting
                    // state, so DISABLE it for real -- EnableWindow(FALSE) blocks
                    // both mouse clicks and keyboard activation at the OS level
                    // until the 5s delay genuinely elapses. The WM_COMMAND and
                    // worker-side agcd::Clickable checks stay as a second guard.
                    // The repaint timer (id 112) only drives a once-per-second
                    // M:SS label refresh now (no 70ms flicker), so 100ms is
                    // plenty to catch each whole-second boundary promptly.
                    EnableWindow(autoBtn, FALSE);
                    d->autoGroupLastSec = -1;
                    d->autoGroupLastEnabled = false;
                    SetTimer(hwnd, 112, 100, nullptr);
                }
            }

            SendMessageW(label, WM_SETFONT, reinterpret_cast<WPARAM>(font), TRUE);
            SendMessageW(d->edit, WM_SETFONT, reinterpret_cast<WPARAM>(font), TRUE);
            SendMessageW(ok, WM_SETFONT, reinterpret_cast<WPARAM>(font), TRUE);
            SendMessageW(cancel, WM_SETFONT, reinterpret_cast<WPARAM>(font), TRUE);

            // S12 (fix thse isseue.md v0.12.52): the New Folder EDIT is no longer
            // region-clipped. A GDI HRGN clip gives hard, stair-stepped corners
            // at high DPI; instead the parent now paints an anti-aliased rounded
            // field SURFACE (fill + border) behind the New Folder EDIT in
            // WM_ERASEBKGND -- the SAME treatment the Group name + Destination
            // fields use -- so the input reads as a smooth rounded field while
            // the EDIT itself stays a plain, crisp Win32 control.
            // Keep the caret/text off the rounded edge of the input field.
            SendMessageW(d->edit, EM_SETMARGINS,
                         EC_LEFTMARGIN | EC_RIGHTMARGIN, MAKELPARAM(S(10), S(10)));
            // Detheme the edit so the dark #545454 background actually applies
            // (a themed edit ignores WM_CTLCOLOREDIT and stays white).
            DisableWindowTheme(d->edit);
            // fix thse isseue.md #7: Group popup only -- subclass the Folder name
            // EDIT so it shows the placeholder when empty+unfocused. New Folder
            // keeps its original edit untouched.
            if (d && d->isGroup) {
                g_nameFieldDpi = static_cast<int>(dpi);
                g_origNameFieldProc = reinterpret_cast<WNDPROC>(SetWindowLongPtrW(
                    d->edit, GWLP_WNDPROC,
                    reinterpret_cast<LONG_PTR>(NameFieldSubclassProc)));
            }

            SetFocus(d->edit);
            return 0;
        }
        case WM_TIMER: {
            // fix thse isseue.md #3: Copy|Move active-fill glide. Each tick eases
            // the shared progress toward the current choice and repaints BOTH
            // segments; the timer stops itself once the glide lands on target.
            if (wParam == 110) {
                if (d) {
                    const int target = (segmented::FromIsMove(d->isMove) ==
                                        segmented::Seg::Move) ? segglide::kMax
                                                              : segglide::kMin;
                    d->glideProgress = segglide::Advance(d->glideProgress, target);
                    if (d->segCopyBtn) InvalidateRect(d->segCopyBtn, nullptr, FALSE);
                    if (d->segMoveBtn) InvalidateRect(d->segMoveBtn, nullptr, FALSE);
                    if (d->glideProgress == target) KillTimer(hwnd, 110);
                } else {
                    KillTimer(hwnd, 110);
                }
                return 0;
            }
            // ADD-ON (Phase D): the Auto Group countdown repaint tick. Recompute
            // the state from the MONOTONIC clock each tick (never a tick count),
            // so a stalled/busy loop or a late WM_TIMER cannot unlock early or
            // late. We only repaint the button; the owner-draw reads the same
            // agcd:: model. Once the button is Enabled we stop the timer -- its
            // label is then static. We never run the operation here.
            if (wParam == 112) {
                if (d && d->autoGroupBtn) {
                    const bool en = AtomicGet(&g_autoGroupEnabled) != 0;
                    const ULONGLONG now = GetTickCount64();
                    const agcd::State st =
                        agcd::Resolve(en, d->autoGroupStartTick, now);
                    const int secs =
                        agcd::RemainingSeconds(en, d->autoGroupStartTick, now);
                    const bool nowEnabled = (st == agcd::State::Enabled);
                    // fix thse isseue.md Phase 4: repaint ONLY when the visible
                    // whole-second M:SS label OR the enabled/disabled state
                    // actually changes -- never once per tick. The decision is
                    // the unit-tested agcd::ShouldRepaint model, so the
                    // anti-flicker contract cannot silently regress.
                    if (agcd::ShouldRepaint(secs, d->autoGroupLastSec,
                                            nowEnabled, d->autoGroupLastEnabled)) {
                        d->autoGroupLastSec = secs;
                        if (nowEnabled != d->autoGroupLastEnabled) {
                            d->autoGroupLastEnabled = nowEnabled;
                            // Flip the REAL disabled state so clicks/keys unlock
                            // exactly at expiry (monotonic-clock driven).
                            EnableWindow(d->autoGroupBtn,
                                         nowEnabled ? TRUE : FALSE);
                        }
                        InvalidateRect(d->autoGroupBtn, nullptr, FALSE);
                    }
                    // Once Enabled the label is static -- stop the timer so no
                    // further ticks arrive.
                    if (st != agcd::State::Waiting) KillTimer(hwnd, 112);
                } else {
                    KillTimer(hwnd, 112);
                }
                return 0;
            }
            // fix thse isseue.md Phase B: one-shot Selected Items scrollbar
            // idle-hide. The reveal window after the last scroll has elapsed;
            // drop the pointer-inside flag and repaint the list so the tested
            // sbauto::Visible() hides the thumb. The gutter stays reserved, so
            // nothing reflows. One-shot (we KillTimer here) -- no repaint loop.
            if (wParam == 113) {
                KillTimer(hwnd, 113);
                if (d) {
                    d->listPointerInside = false;
                    if (d->groupInfo) InvalidateRect(d->groupInfo, nullptr, FALSE);
                }
                return 0;
            }
            break;
        }
        case WM_NCHITTEST: {
            // fix thse isseue.md #2: the Group popup is WS_POPUP (no native
            // caption), so the custom title band must drag the window. titlebar::
            // Classify (unit-tested) decides caption-vs-button; the Expand/Min/
            // Close children intercept their own clicks, so returning HTCAPTION
            // for the rest of the band moves the window. New Folder keeps its
            // native caption (handled by DefWindowProc).
            if (d && d->isGroup) {
                POINT pt = {static_cast<LONG>(static_cast<short>(LOWORD(lParam))),
                            static_cast<LONG>(static_cast<short>(HIWORD(lParam)))};
                ScreenToClient(hwnd, &pt);
                const UINT hdpi = d->dpi;
                auto HS = [hdpi](int v) { return ScaleDpi(v, hdpi); };
                // fix thse isseue.md v0.12.65 Bug 1: hit-test the title-bar
                // controls from the SAME effective layout the paint + child-move
                // use, so after a maximize/resize the caption buttons are
                // hit-tested where they were actually moved to (reflowed), not
                // at their preferred positions.
                const layout::PromptLayout L = ComputeEffectiveLayout(hwnd, d);
                auto toBox = [&](const layout::Rect& r) {
                    return titlebar::Box{HS(r.x), HS(r.y), HS(r.w), HS(r.h)};
                };
                titlebar::Region reg = titlebar::Classify(
                    pt.x, pt.y, HS(layout::TitleBandH()),
                    toBox(L.chromeSizeBtn), toBox(L.chromeMinBtn),
                    toBox(L.chromeCloseBtn));
                if (reg == titlebar::Region::Caption) {
                    return HTCAPTION;
                }
            }
            break;
        }
        case WM_ERASEBKGND: {
            // fix thse isseue.md #1/#2/#3: the background paint mode is decided
            // ONCE, honestly, from the CONFIRMED DWM state. d->acrylicActive is
            // set via paintmode:: from BOTH DWM HRESULTs, so it now means "the
            // Group title band is confirmed compositing" -- it is false for the
            // New Folder popup AND for every backdrop/frame-extension failure.
            // The only two paths are therefore:
            //   * groupBandBackdrop -> DwmTitleBand: leave ONLY the Group title
            //     band for the Win11 material, paint the themed panel BELOW it.
            //   * otherwise          -> SolidTheme : paint the WHOLE client area
            //     (title band included) with the themed panel brush.
            // The New Folder popup (native caption, never frame-extended) ALWAYS
            // takes the SolidTheme branch -- it must NEVER return 1 to skip its
            // own fill, which is what left a bare WHITE body in dark mode (#1).
            const bool groupBandBackdrop = d && d->isGroup && d->acrylicActive;
            HDC hdc = reinterpret_cast<HDC>(wParam);
            RECT rc;
            GetClientRect(hwnd, &rc);
            if (groupBandBackdrop) {
                // Group popup, backdrop CONFIRMED. fix thse isseue.md P0 #1: the
                // DWM-extended title band must be handed TRANSPARENT pixel data,
                // NOT merely left untouched. Per Microsoft's "Custom Window
                // Frame Using DWM" guidance, GDI writes no alpha, so a BLACK
                // (0,0,0) fill inside the extended frame region reads back as
                // fully transparent and the Win11 system material composites
                // through it. Leaving the band's pixels untouched is exactly
                // what left a WHITE strip in the caption. So: fill ONLY the
                // DPI-scaled title band with the stock BLACK_BRUSH while the
                // frame extension is active, then paint the themed panel BELOW
                // the band so the five cards, fields and text stay solid and
                // readable. The separator, title text and caption controls are
                // drawn on top of this frame further below.
                const int bandH = ScaleDpi(layout::TitleBandH(), d->dpi);
                const int clampedBandH = (bandH < rc.bottom) ? bandH : rc.bottom;
                RECT band = rc;
                band.bottom = clampedBandH;
                FillRect(hdc, &band,
                         static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH)));
                RECT below = rc;
                if (clampedBandH < rc.bottom) {
                    below.top = clampedBandH;
                    FillRect(hdc, &below, PanelBrush(IsDarkMode()));
                }
            } else {
                // Solid themed fallback -- the ALWAYS-readable surface. Used by
                // EVERY New Folder popup (#1: no white body) and by the Group
                // popup on Win10 / Transparency off / remote / when the backdrop
                // or the DWM frame extension failed (#3: no white title strip).
                // Paints the whole client, title band included.
                FillRect(hdc, &rc, PanelBrush(IsDarkMode()));
            }
            // S12 (fix thse isseue.md v0.12.52): the plain New Folder popup paints
            // its own AA rounded field surface -- this REPLACES the removed
            // SetWindowRgn clip on the EDIT. Same technique as the Group fields:
            // a surface 1px larger on each side, filled in editBg with a rounded
            // border, so the rectangular EDIT sits inside and only the smooth
            // rounded edge shows. GDI RoundRect stays as the per-frame fallback.
            if (d && !d->isGroup) {
                const UINT nfdpi = d->dpi;
                const layout::PromptLayout nfL = layout::ComputePromptLayout(false);
                const ThemePalette npal = ResolveThemePalette(IsDarkMode());
                const int nfRad = ScaleDpi(namefield::kRadius * 2, nfdpi);
                const bool nfFocused = (d->edit && GetFocus() == d->edit);
                const COLORREF nfBorder = namefield::BorderIsAccent(nfFocused)
                                              ? npal.createFill : npal.cardBorder;
                auto NS = [nfdpi](int v) { return ScaleDpi(v, nfdpi); };
                RECT nfr = {NS(nfL.nameField.x) - NS(1), NS(nfL.nameField.y) - NS(1),
                            NS(nfL.nameField.right()) + NS(1),
                            NS(nfL.nameField.bottom()) + NS(1)};
                if (!DrawRoundSurfaceAA(hdc, nfr, aarender::Surface::Button,
                                        nfRad, npal.editBg, false, npal.editBg, 0)) {
                    HBRUSH nsb = CreateSolidBrush(npal.editBg);
                    HPEN   nsp = CreatePen(PS_SOLID, NS(1), npal.editBg);
                    HGDIOBJ onsb = SelectObject(hdc, nsb);
                    HGDIOBJ onsp = SelectObject(hdc, nsp);
                    RoundRect(hdc, nfr.left, nfr.top, nfr.right, nfr.bottom,
                              nfRad, nfRad);
                    SelectObject(hdc, onsb);
                    SelectObject(hdc, onsp);
                    DeleteObject(nsb);
                    DeleteObject(nsp);
                }
                if (!DrawRoundStrokeAA(hdc, nfr, aarender::Surface::FocusRing,
                                       nfRad, nfBorder, NS(1))) {
                    HPEN nbp = CreatePen(PS_SOLID, NS(1), nfBorder);
                    HGDIOBJ onb = SelectObject(hdc, nbp);
                    HGDIOBJ onbr = SelectObject(hdc, GetStockObject(HOLLOW_BRUSH));
                    RoundRect(hdc, nfr.left, nfr.top, nfr.right, nfr.bottom,
                              nfRad, nfRad);
                    SelectObject(hdc, onb);
                    SelectObject(hdc, onbr);
                    DeleteObject(nbp);
                }
            }
            // Phase 4/B (fix thse isseu e.md #2): paint each Group section as
            // its OWN rounded card on top of the panel fill, so the popup reads
            // as a card stack instead of one flat surface. The card rects come
            // from the SAME tested layout model that places the controls, so the
            // surfaces line up exactly with Selected / Destination / Folder name
            // / Status. New Folder has no cards (isGroup == false).
            if (d && d->isGroup) {
                const UINT cdpi = d->dpi;
                // fix thse isseue.md v0.12.65 Bug 1: paint every card from the
                // SAME effective layout the children were moved to. Before this,
                // the cards were painted from the PREFERRED compact/expanded
                // layout while the controls had been reflowed to the live client
                // rect, so a maximized/resized popup showed card borders at the
                // old width with content spilling past them (the horizontal bars
                // outside the cards). The effective layout is the reflowed one
                // once manualSized/zoomed, so borders now track their children.
                const layout::PromptLayout L = ComputeEffectiveLayout(hwnd, d);
                const ThemePalette cpal = ResolveThemePalette(IsDarkMode());
                // fix thse isseue.md #2: custom title bar (WS_POPUP, no native
                // caption). A hairline separator under the band + the folder
                // icon and the "Group into folder" title at the left; the
                // Expand/Minimize/Close controls are owner-draw children at the
                // tested chrome rects, and the band drags via WM_NCHITTEST.
                {
                    const int bandH = ScaleDpi(layout::TitleBandH(), cdpi);
                    // fix thse isseue.md #4 (v0.12.60): the explicit hairline
                    // separator beneath the title band was REMOVED -- no border
                    // or opaque strip replaces it. The DWM backdrop, the outer
                    // rounded silhouette and the window shadow carry the band
                    // edge on their own.
                    // fix thse isseue.md #1: the window title keeps ONLY its
                    // 📂 emoji as the folder mark. The previously-drawn DUPLICATE
                    // folder vector icon was removed -- no icon on/off, the single
                    // emoji is always the title glyph -- so the caption shows one
                    // folder, not two. The text now starts at the band's left
                    // inset where the drawn icon used to sit.
                    const int tLeft = ScaleDpi(layout::kOuterMargin, cdpi);
                    SetBkMode(hdc, TRANSPARENT);
                    SetTextColor(hdc, CaptionForeground(IsDarkMode()));
                    HGDIOBJ oldTitleF = SelectObject(hdc, UiFontSemibold(cdpi));
                    RECT tText = {tLeft, 0,
                                  // fix thse isseue.md A: clip the title BEFORE the
                                  // LEFTMOST caption button (Minimize) so it never
                                  // overlaps the Minimize control once the buttons
                                  // are contiguous (was clipped to the middle Expand).
                                  ScaleDpi(L.chromeMinBtn.x, cdpi) - ScaleDpi(6, cdpi),
                                  bandH};
                    DrawTextW(hdc, L"\U0001F4C2 Group into Folder", -1, &tText,
                              DT_LEFT | DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS);
                    SelectObject(hdc, oldTitleF);
                }
                const int crad = ScaleDpi(card::kCardRadius, cdpi);
                const layout::Rect cardRects[] = {
                    L.selectedCard, L.destCard, L.nameCard,
                    L.actionCard};
                for (const layout::Rect& cr : cardRects) {
                    RECT r = {ScaleDpi(cr.x, cdpi), ScaleDpi(cr.y, cdpi),
                              ScaleDpi(cr.right(), cdpi),
                              ScaleDpi(cr.bottom(), cdpi)};
                    DrawSectionCard(hdc, r, cpal.cardBg, cpal.cardBorder, crad);
                }

                // Phase 7/E (fix thse isseu e.md #5/#6/#13): supporting
                // descriptions + small section icons, drawn ON the card surface
                // (no extra HWNDs, so the tested geometry is untouched). The
                // Destination helper reflects the Move/Copy choice via the
                // tested dest:: model; the Folder-name helper is the tested
                // seclbl:: string. Icons are simple vector glyphs (NOT emoji):
                // a folder mark before Destination, a pencil before Folder name.
                auto S2 = [cdpi](int v) { return ScaleDpi(v, cdpi); };
                SetBkMode(hdc, TRANSPARENT);
                const COLORREF descColor = IsDarkMode() ? RGB(165, 165, 165)
                                                        : RGB(96, 96, 96);
                HGDIOBJ oldF = SelectObject(hdc, UiFont(cdpi));

                // fix thse isseue.md v0.12.57: Windows-native semantic section
                // icons (Segoe Fluent Icons -> MDL2 -> Shell stock), painted in
                // the primary heading colour with NO coloured tile.
                DrawSemanticSectionIcon(hdc, semanticicon::Section::Destination,
                                        L.destLabel, cdpi, cpal.panelText);
                DrawSemanticSectionIcon(hdc, semanticicon::Section::Name,
                                        L.nameLabel, cdpi, cpal.panelText);
                DrawSemanticSectionIcon(hdc, semanticicon::Section::Action,
                                        L.actionTitle, cdpi, cpal.panelText);
                // fix thse isseue.md #7: a clear rounded border around the Folder
                // name field, accent-coloured while it has keyboard focus so the
                // input reads as unmistakable (idle = neutral card border).
                {
                    const bool nameFocused = (d->edit && GetFocus() == d->edit);
                    const COLORREF nbc = namefield::BorderIsAccent(nameFocused)
                                             ? cpal.createFill : cpal.cardBorder;
                    // fix thse isseue.md #2: frame at EXACTLY the field bounds
                    // (inputfield::OuterFrameRect / FrameSpillPx() == 0) -- no
                    // outward inflation / border spill. The accent-on-focus
                    // colour (nbc) is unchanged; only the geometry tightens.
                    RECT nfr = {S2(L.nameField.x), S2(L.nameField.y),
                                S2(L.nameField.right()),
                                S2(L.nameField.bottom())};
                    // fix thse isseue.md P0 #2: FILL the rounded field surface in
                    // the field bg BEFORE the border. The name EDIT is no longer
                    // region-clipped (it is a plain rectangle), so this filled
                    // rounded surface -- 1px larger on every side -- is what gives
                    // the field its rounded shape; the EDIT's square corners sit
                    // inside it in the same colour, so only the rounded surface
                    // edge shows against the card. No HRGN, no hard-edged clip.
                    {
                        const int nsrad = S2(namefield::kRadius * 2);
                        // S11 (fix thse isseue.md v0.12.51): route the field
                        // SURFACE fill through the Direct2D anti-aliased pipeline
                        // (Surface::Button is AA per the aarender policy) so the
                        // most-looked-at element gets smooth corners. The EDIT
                        // control itself stays pure Win32 (Surface::EditField ->
                        // not AA). GDI RoundRect is the exact fallback when D2D is
                        // unavailable, so the field degrades to today's look.
                        if (!DrawRoundSurfaceAA(hdc, nfr, aarender::Surface::Button,
                                                nsrad, cpal.editBg, false,
                                                cpal.editBg, 0)) {
                            HBRUSH nsb = CreateSolidBrush(cpal.editBg);
                            HPEN   nsp = CreatePen(PS_SOLID, S2(1), cpal.editBg);
                            HGDIOBJ onsb = SelectObject(hdc, nsb);
                            HGDIOBJ onsp = SelectObject(hdc, nsp);
                            RoundRect(hdc, nfr.left, nfr.top, nfr.right, nfr.bottom,
                                      nsrad, nsrad);
                            SelectObject(hdc, onsb);
                            SelectObject(hdc, onsp);
                            DeleteObject(nsb);
                            DeleteObject(nsp);
                        }
                    }
                    {
                        const int nrad = S2(namefield::kRadius * 2);
                        // S11: the hairline BORDER goes through the AA stroke path
                        // (Surface::FocusRing is AA); GDI hollow RoundRect is the
                        // fallback. nbc is accent while focused, neutral idle.
                        if (!DrawRoundStrokeAA(hdc, nfr, aarender::Surface::FocusRing,
                                               nrad, nbc, S2(1))) {
                            HPEN nbp = CreatePen(PS_SOLID, S2(1), nbc);
                            HGDIOBJ onb = SelectObject(hdc, nbp);
                            HGDIOBJ onbr = SelectObject(hdc, GetStockObject(HOLLOW_BRUSH));
                            RoundRect(hdc, nfr.left, nfr.top, nfr.right, nfr.bottom,
                                      nrad, nrad);
                            SelectObject(hdc, onb);
                            SelectObject(hdc, onbr);
                            DeleteObject(nbp);
                        }
                    }
                }

                // fix thse isseue.md #6: a steady NEUTRAL rounded border around
                // the read-only Destination field so it reads as a defined info
                // field. destfield::BorderIsAccent is ALWAYS false (unlike the
                // editable Folder name field), so this never shows a focus ring.
                {
                    const bool destFocused =
                        (d->destField && GetFocus() == d->destField);
                    const COLORREF dbc = destfield::BorderIsAccent(destFocused)
                                             ? cpal.createFill : cpal.cardBorder;
                    // fix thse isseue.md #2: paint the frame at EXACTLY the field
                    // bounds (inputfield::OuterFrameRect / FrameSpillPx() == 0) --
                    // no outward -1/+1 inflation, so the neutral border never
                    // spills past the field and cannot read as a thick/doubled
                    // edge. The EDIT is inset by kEditInset, so its square corners
                    // still never reach the rounded perimeter.
                    RECT dfr = {S2(L.destField.x), S2(L.destField.y),
                                S2(L.destField.right()),
                                S2(L.destField.bottom())};
                    // fix thse isseue.md P0 #2: FILL the rounded field surface in
                    // the field bg BEFORE the border. The Destination EDIT is no
                    // longer region-clipped, so this filled rounded surface gives
                    // the read-only field its rounded shape (its square corners
                    // sit inside, same colour). No HRGN, no hard-edged clip.
                    {
                        const int dsrad = S2(destfield::kRadius * 2);
                        // fix thse isseue.md #2: paint the field SURFACE and its
                        // hairline BORDER in ONE anti-aliased pass that shares a
                        // SINGLE rounded geometry, so the fill and the border are
                        // concentric and their corners coincide exactly. The old
                        // code filled the full field rect (stroke inset 0) and
                        // THEN stroked a separately-inset rounded rect of the SAME
                        // radius; two differently-inset rects with one radius have
                        // NON-concentric corner arcs, so the lighter editBg surface
                        // bulged past the neutral border at the four corners -- the
                        // "square-corner spill" / thick, doubled, bright edge the
                        // issue reports, and it grew with the stroke width at 125/
                        // 150/200% DPI. One pass == one deliberate perimeter, edges
                        // aligned, no spill. (Scoped to the Destination field per
                        // issue #2; the editable Folder name field is untouched.)
                        // S11: Direct2D AA fill+border via Surface::Button; the GDI
                        // RoundRect fallback paints the same editBg fill with the
                        // border pen in a single RoundRect. The read-only EDIT
                        // stays pure Win32, inset inside this perimeter.
                        if (!DrawRoundSurfaceAA(hdc, dfr, aarender::Surface::Button,
                                                dsrad, cpal.editBg, true,
                                                dbc, S2(1))) {
                            HBRUSH dsb = CreateSolidBrush(cpal.editBg);
                            HPEN   dsp = CreatePen(PS_SOLID, S2(1), dbc);
                            HGDIOBJ odsb = SelectObject(hdc, dsb);
                            HGDIOBJ odsp = SelectObject(hdc, dsp);
                            RoundRect(hdc, dfr.left, dfr.top, dfr.right, dfr.bottom,
                                      dsrad, dsrad);
                            SelectObject(hdc, odsb);
                            SelectObject(hdc, odsp);
                            DeleteObject(dsb);
                            DeleteObject(dsp);
                        }
                    }
                }
                
                // fix thse isseue.md (path pass): per request, the Destination
                // card paints NO supporting description row -- it is just the
                // icon + "Destination" title (destLabel STATIC) + the read-only
                // path field + the Copy button. Same for the Folder-name card.
                // fix thse isseue.md #1: the Status card + its "Status" heading
                // were removed, so nothing is painted between the Folder-name
                // card and the Action card here.
                // Action card (fix thse isseu e.md #6): a strong title, a
                // one-line description, and a dimmer keyboard hint under the
                // Copy/Move segments. The segments themselves are owner-drawn
                // child controls placed at L.segCopy / L.segMove inside the card.
                {
                    // Title -- primary weight (reuse the panel text colour).
                    const COLORREF titleColor = IsDarkMode() ? RGB(240, 240, 240)
                                                             : RGB(32, 32, 32);
                    SetTextColor(hdc, titleColor);
                    RECT at = {S2(L.actionTitle.x), S2(L.actionTitle.y),
                               S2(L.actionTitle.right()), S2(L.actionTitle.bottom())};
                    // S14: Action heading in the heavier semibold face.
                    HGDIOBJ semiAt = SelectObject(hdc, UiFontSemibold(cdpi));
                    DrawTextW(hdc, seclbl::ActionLabel(), -1, &at,
                              DT_LEFT | DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS);
                    SelectObject(hdc, semiAt);
                    // Description -- secondary.
                    SetTextColor(hdc, descColor);
                    RECT ad = {S2(L.actionDesc.x), S2(L.actionDesc.y),
                               S2(L.actionDesc.right()), S2(L.actionDesc.bottom())};
                    DrawTextW(hdc, seclbl::ActionDescription(), -1, &ad,
                              DT_LEFT | DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS);
                    // Keyboard hint -- dimmer, centred under the segments.
                    SetTextColor(hdc, descColor);
                    RECT kh = {S2(L.keyboardHint.x), S2(L.keyboardHint.y),
                               S2(L.keyboardHint.right()), S2(L.keyboardHint.bottom())};
                    DrawTextW(hdc, seclbl::ActionKeyboardHint(), -1, &kh,
                              DT_CENTER | DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS);
                }
                SelectObject(hdc, oldF);
            }
            return 1;
        }
        case WM_CTLCOLORSTATIC: {
            HDC hdc = reinterpret_cast<HDC>(wParam);
            ThemePalette pal = ResolveThemePalette(IsDarkMode());
            SetTextColor(hdc, pal.panelText);
            SetBkMode(hdc, TRANSPARENT);
            // fix thse isseue.md #1/#2: BOTH popups now paint a SOLID themed
            // surface behind their labels -- the New Folder body is always solid
            // (#1), and the Group content area is always solid with only its
            // title band translucent (#2). A label therefore always uses its
            // surface brush and NEVER a hollow brush, so it can never punch a
            // hole through to a non-existent backdrop. A Group label sits ON a
            // section card (cardBg); a New Folder label sits on the panel
            // (panelBg). (No STATIC label is ever placed on the translucent
            // Group title band -- the caption text is drawn in WM_ERASEBKGND.)
            COLORREF labelBg = (d && d->isGroup) ? pal.cardBg : pal.panelBg;
            return reinterpret_cast<LRESULT>(ThemeBrush(labelBg));
        }
        case WM_CTLCOLOREDIT: {
            HDC hdc = reinterpret_cast<HDC>(wParam);
            ThemePalette pal = ResolveThemePalette(IsDarkMode());
            SetTextColor(hdc, pal.editText);
            // fix thse isseue.md #4: ALL prompt EDITs (Group Folder name, Group
            // Destination, New Folder name) use an OPAQUE field-surface brush.
            // Each EDIT is an inset rectangle inside the parent-painted rounded
            // frame (inputfield::InnerEditRect), so its opaque square never
            // reaches the rounded border, and an opaque erase means deleting text
            // never leaves a ghost. (The old hollow brush + EDIT-self-erased
            // rounded surface is gone -- one geometry, no stale pixels.)
            SetBkColor(hdc, pal.editBg);
            return reinterpret_cast<LRESULT>(ThemeBrush(pal.editBg));
        }
        case WM_DRAWITEM: {
            DRAWITEMSTRUCT* dis = reinterpret_cast<DRAWITEMSTRUCT*>(lParam);
            if (dis->CtlType == ODT_BUTTON && dis->CtlID == 103 && d) {
                const ThemePalette pal = ResolveThemePalette(IsDarkMode());
                // fix thse isseue.md #1/#3 (v0.12.60): a TRANSPARENT caption
                // button -- NO permanent grey/black backplate. The idle surface
                // matches the band so the glyph alone reads over the DWM
                // material; hover/press/red-Close are TRANSIENT feedback painted
                // over that idle surface. The child HWND stays fully
                // hit-testable (no click-through colour-key, no overlay).
                {
                    // fix thse isseue.md #1 (v0.12.60): NO permanent grey/black
                    // backplate behind the caption glyph. The idle surface
                    // MATCHES the band beneath it so the button blends into the
                    // title-bar material -- a CONFIRMED DWM backdrop hands it the
                    // transparent BLACK brush (the Win11 material composites
                    // through, exactly like the band fill in WM_ERASEBKGND);
                    // otherwise the solid themed panel so it blends into the
                    // solid-header fallback. The child HWND stays fully
                    // hit-testable either way (no colour-key, no overlay).
                    // Mirrors tests/caption_fill_logic.h.
                    const bool bandBackdrop = d->isGroup && d->acrylicActive;
                    HBRUSH bg = bandBackdrop
                        ? static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH))
                        : CreateSolidBrush(pal.panelBg);
                    FillRect(dis->hDC, &dis->rcItem, bg);
                    if (!bandBackdrop) DeleteObject(bg);
                }
                // fix thse isseue.md #7: hover/pressed chrome background (neutral
                // tint) from the tested chromebtn:: model, so the Expand control
                // reacts like a real caption button.
                const chromebtn::Fill cf = chromebtn::FillFor(
                    (dis->itemState & ODS_HOTLIGHT) != 0,
                    (dis->itemState & ODS_SELECTED) != 0);
                if (chromebtn::PaintsBackground(cf)) {
                    HBRUSH hb = CreateSolidBrush(
                        cf == chromebtn::Fill::Pressed ? pal.cardBorder : pal.editBg);
                    // fix thse isseue.md: full header-height fill, inset by a
                    // left/right guard (tested chromebtn::FillArea) so adjacent
                    // caption fills never touch and a margin shows.
                    const chromebtn::FillBox gfb = chromebtn::FillArea(
                        dis->rcItem.right - dis->rcItem.left,
                        dis->rcItem.bottom - dis->rcItem.top,
                        ScaleDpi(chromebtn::kFillGuard, d->dpi));
                    RECT fr{dis->rcItem.left + gfb.x, dis->rcItem.top + gfb.y,
                            dis->rcItem.left + gfb.x + gfb.w,
                            dis->rcItem.top + gfb.y + gfb.h};
                    FillRect(dis->hDC, &fr, hb);
                    DeleteObject(hb);
                }
                // fix thse isseue.md: bigger, centered caption glyph (native
                // style) via the tested-size box, now the button fills the full
                // header height.
                RECT iconRc = icons::CaptionGlyphBox(
                    dis->rcItem, ScaleDpi(icons::kChromeGlyph, d->dpi));
                // fix thse isseue.md C: native Segoe caption glyph -- Maximize
                // (E922) when compact, Restore (E923) when expanded. The vector
                // icon is the DELIBERATE fallback when no icon font/glyph exists.
                if (!DrawCaptionGlyph(dis->hDC, iconRc,
                                      captionglyph::Button::Expand, d->expanded,
                                      CaptionForeground(IsDarkMode()))) {
                    icons::DrawIcon(dis->hDC,
                                    d->expanded ? icons::IconId::Collapse
                                                : icons::IconId::Expand,
                                    iconRc, CaptionForeground(IsDarkMode()));
                }
                return TRUE;
            }
            // fix thse isseue.md #1/#2: title-bar Minimize (104) + Close (105).
            // Same flat chrome background as Expand; a geometric dash / X drawn
            // directly from the panel pen (NOT a font glyph), DPI-aware.
            if (dis->CtlType == ODT_BUTTON &&
                (dis->CtlID == 104 || dis->CtlID == 105) && d) {
                const ThemePalette pal = ResolveThemePalette(IsDarkMode());
                // fix thse isseue.md #1/#2/#3 (v0.12.60): title-bar Minimize
                // (104) + Close (105) are TRANSPARENT caption buttons -- NO
                // permanent backplate. The idle surface matches the band; a
                // geometric dash / X (or the native font glyph) is drawn in the
                // shared caption foreground; hover/press tint and the red Close
                // danger fill are TRANSIENT states over the idle surface. The
                // child HWND stays fully hit-testable (no colour-key, no overlay).
                {
                    // fix thse isseue.md #1 (v0.12.60): NO permanent grey/black
                    // backplate behind the caption glyph. The idle surface
                    // MATCHES the band beneath it so the button blends into the
                    // title-bar material -- a CONFIRMED DWM backdrop hands it the
                    // transparent BLACK brush (the Win11 material composites
                    // through, exactly like the band fill in WM_ERASEBKGND);
                    // otherwise the solid themed panel so it blends into the
                    // solid-header fallback. The child HWND stays fully
                    // hit-testable either way (no colour-key, no overlay).
                    // Mirrors tests/caption_fill_logic.h.
                    const bool bandBackdrop = d->isGroup && d->acrylicActive;
                    HBRUSH bg = bandBackdrop
                        ? static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH))
                        : CreateSolidBrush(pal.panelBg);
                    FillRect(dis->hDC, &dis->rcItem, bg);
                    if (!bandBackdrop) DeleteObject(bg);
                }
                // fix thse isseue.md #7: hover/pressed chrome background; the
                // Close button (105) goes red (Win11-style). Visual state comes
                // from the tested chromebtn:: model; the mark re-tints to white
                // on the red Close fill so it stays legible.
                COLORREF markColor = CaptionForeground(IsDarkMode());
                const chromebtn::Fill cf = chromebtn::FillFor(
                    (dis->itemState & ODS_HOTLIGHT) != 0,
                    (dis->itemState & ODS_SELECTED) != 0);
                if (chromebtn::PaintsBackground(cf)) {
                    COLORREF fillCol;
                    if (chromebtn::IsDanger(dis->CtlID == 105, cf)) {
                        fillCol = (cf == chromebtn::Fill::Pressed) ? RGB(193, 15, 30)
                                                                   : RGB(232, 17, 35);
                        markColor = RGB(255, 255, 255);
                    } else {
                        fillCol = (cf == chromebtn::Fill::Pressed) ? pal.cardBorder
                                                                   : pal.editBg;
                    }
                    HBRUSH hb = CreateSolidBrush(fillCol);
                    // fix thse isseue.md: full header-height fill, inset by a
                    // left/right guard (tested chromebtn::FillArea).
                    const chromebtn::FillBox gfb = chromebtn::FillArea(
                        dis->rcItem.right - dis->rcItem.left,
                        dis->rcItem.bottom - dis->rcItem.top,
                        ScaleDpi(chromebtn::kFillGuard, d->dpi));
                    RECT fr{dis->rcItem.left + gfb.x, dis->rcItem.top + gfb.y,
                            dis->rcItem.left + gfb.x + gfb.w,
                            dis->rcItem.top + gfb.y + gfb.h};
                    FillRect(dis->hDC, &fr, hb);
                    DeleteObject(hb);
                }
                // fix thse isseue.md: bigger, centered native caption marks via
                // the tested-size glyph box; one consistent DPI stroke weight.
                RECT mk = icons::CaptionGlyphBox(
                    dis->rcItem, ScaleDpi(icons::kChromeGlyph, d->dpi));
                // fix thse isseue.md C: native Segoe caption glyph -- E921
                // ChromeMinimize (104) / E8BB ChromeClose (105). The geometric
                // dash / X is the DELIBERATE fallback when no icon font/glyph
                // is present (never an empty box).
                const captionglyph::Button cbtn =
                    (dis->CtlID == 104) ? captionglyph::Button::Minimize
                                        : captionglyph::Button::Close;
                if (!DrawCaptionGlyph(dis->hDC, mk, cbtn, d->expanded, markColor)) {
                    HPEN pen = CreatePen(PS_SOLID,
                                         icons::IconStrokeWidth(mk.right - mk.left), markColor);
                    HGDIOBJ oldPen = SelectObject(dis->hDC, pen);
                    if (dis->CtlID == 104) {
                        // Minimize: a single horizontal line, vertically centered.
                        const int my = (mk.top + mk.bottom) / 2;
                        MoveToEx(dis->hDC, mk.left, my, nullptr);
                        LineTo(dis->hDC, mk.right, my);
                    } else {
                        // Close: a clean, symmetric X (native ChromeClose).
                        MoveToEx(dis->hDC, mk.left, mk.top, nullptr);
                        LineTo(dis->hDC, mk.right, mk.bottom);
                        MoveToEx(dis->hDC, mk.left, mk.bottom, nullptr);
                        LineTo(dis->hDC, mk.right, mk.top);
                    }
                    SelectObject(dis->hDC, oldPen);
                    DeleteObject(pen);
                }
                return TRUE;
            }
            // Selected Items list (id 110, Phase 5/C): owner-drawn rows instead
            // of one flat STATIC blob. THREE visual levels -- title, counts, then
            // the scrollable rows `name .... EXT  SIZE` with real column geometry
            // (listcol::) and a header separator. The visible slice comes from
            // the SAME tested listscroll:: + mode:: model, so wheel/keys page
            // through a large selection; sel:: formats each cell. HONEST: only
            // the rows currently in the viewport are drawn.
            // fix thse isseue.md Issue 1: the old large help overlay (id 122,
            // an SS_OWNERDRAW child sized to L.selectedContent that replaced the
            // Selected Items list) is REMOVED. Navigation help now renders in a
            // separate non-activating popup window (HelpCardWndProc), so it can
            // never cover the item list and the list repaint can never erase it.
            if (dis->CtlType == ODT_STATIC && dis->CtlID == 110 && d) {
                RECT rc = dis->rcItem;
                // fix thse isseue.md #2: DOUBLE-BUFFER the whole list paint into
                // a memory DC and BitBlt it once, so wheel/key scrolling never
                // shows a mid-repaint flash (paired with the InvalidateRect
                // FALSE in RefreshGroupPanel -- no background erase before this).
                // All drawing below keeps its rc-based coordinates; a viewport
                // shift maps them into the (0,0)-origin backbuffer.
                HDC target = dis->hDC;
                const int bbW = rc.right - rc.left;
                const int bbH = rc.bottom - rc.top;
                HDC hdc = CreateCompatibleDC(target);
                HBITMAP bbBmp = CreateCompatibleBitmap(target, bbW, bbH);
                HGDIOBJ bbOld = SelectObject(hdc, bbBmp);
                SetViewportOrgEx(hdc, -rc.left, -rc.top, nullptr);
                const UINT dpi = d->dpi;
                auto S = [dpi](int v) { return ScaleDpi(v, dpi); };
                const bool dark = IsDarkMode();
                const ThemePalette pal = ResolveThemePalette(dark);
                const COLORREF secondary = dark ? RGB(165, 165, 165)
                                                : RGB(96, 96, 96);
                HBRUSH bg = CreateSolidBrush(pal.cardBg);
                FillRect(hdc, &rc, bg);
                DeleteObject(bg);
                SetBkMode(hdc, TRANSPARENT);
                HGDIOBJ oldFont = SelectObject(hdc, UiFont(dpi));

                const int left  = rc.left;   // #3: 20px inset (kSelectedPad) is the selectedContent rect, not here
                // fix thse isseue.md #4: the inner list content stops a dedicated
                // right inset short of the child's right edge, so rows, the
                // size/extension metadata, chip clipping, the divider endpoint and
                // the scrollbar track all keep balanced breathing room from the
                // card border. Applied ONCE here (the 20px card inset lives in the
                // layout -- no double padding on the outer card).
                const int right = rc.right - S(layout::kSelListRightInset);  // #4: inner right inset
                // fix thse isseue.md Phase A: the row height follows the Selected
                // Items presentation mode (tested viewmode:: model) -- compact in
                // List, taller for Large / Extra large icons so the bigger Shell
                // icon reads clearly. ALL row/scroll/scrollbar math below is
                // parameterised by rowH, so taller rows automatically show fewer
                // rows and enable the scrollbar via EffectiveVisibleRows -- no
                // separate grid code path, so the existing list render, filter
                // dimming, size badges and scroll all keep working unchanged.
                const viewmode::Mode vmode = d->selViewMode;
                const int rowH  = viewmode::IsGrid(vmode)
                    ? S(viewmode::IconPx(vmode) + 16)
                    : S(viewmode::RowHeight(vmode));
                // fix thse isseue.md #1: EVEN "breathing space" rhythm for the
                // Selected header -- air after the band, equal gaps between the
                // title, the type breakdown and a THIN hairline divider, then
                // equal air before the rows (no heavy dead space). The rows still
                // start at the same offset, so no card below moves.
                const int kSelHdrTop = 0;      // #3: 20px top padding is the selectedContent inset, not here
                const int kSelHdrGap = S(4);   // even gap around each element (fix thse isseue.md #3: 4px rhythm, was S(3))
                // fix thse isseue.md v0.12.67 (Issue #1): the TITLE band is a
                // FIXED height (kSelTitleH), NOT the item rowH. In Large / Extra
                // large modes rowH is the grid icon height (64..272px), and the
                // header previously used it for the title row -- that ballooned
                // the band and pushed the chips, divider and rows far down (the
                // "excessive space above the title / between title and chips"
                // defect). kSelTitleH mirrors the ComputePromptLayout header
                // rhythm exactly, so paint and geometry agree; List mode is
                // byte-identical (viewmode::RowHeight(List) == 24 == kSelTitleH).
                const int titleH = S(layout::kSelTitleH);
                int y = rc.top + kSelHdrTop;

                // Level 1 -- title "Selected items (N)" with the Windows-native
                // semantic section icon (fix thse isseue.md v0.12.57): the
                // Selected header uses the SAME semantic-icon renderer as the
                // Destination/Name/Status/Action cards (List/Stack glyph), with
                // NO coloured tile. The text start (left + kSectionIconCol)
                // equals the former kRowIconW(16) + kRowIconGap(8), so the title
                // text does not move.
                SetTextColor(hdc, pal.panelText);
                // fix thse isseue.md #1: Selected Items keeps its EMOJI mark (NOT
                // the Segoe semantic list glyph). Painted in the reserved icon
                // column with the UI font so the Segoe UI Emoji fallback renders
                // it; the title text start (left + kSectionIconCol) is unchanged.
                {
                    RECT selIcoR = {left, y, left + S(layout::kSectionIconCol),
                                    y + titleH};
                    DrawTextW(hdc, L"\U0001F5C2\uFE0F", -1, &selIcoR,
                              DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOCLIP);
                }
                std::wstring title = sel::SummaryTitle(d->items.size());
                RECT tr = {left + S(layout::kSectionIconCol), y,
                           right, y + titleH};
                // S14: the Selected-items heading in the heavier semibold face;
                // restore the regular font for the breakdown + rows below.
                HGDIOBJ semiSel = SelectObject(hdc, UiFontSemibold(dpi));
                DrawTextW(hdc, title.c_str(), -1, &tr,
                          DT_LEFT | DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS);
                SelectObject(hdc, semiSel);
                y += titleH;   // fix thse isseue.md v0.12.67: FIXED title height, not grid rowH
                y += S(layout::kSelTitleChipGap);   // fix thse isseue.md #1: 6px title -> chip row

                // Level 2 -- the interactive summary chip strip (fix thse
                // isseue.md #2/#3): the plain-text sel::TypeBreakdown summary is
                // REPLACED by dynamic metadata chips built from the LIVE
                // selection -- [N items] [N folders] [N EXT]... Ext chips reuse
                // the tested per-extension badgecolor table; items/folders use
                // the muted neutral chip token. The band height is UNCHANGED so
                // the divider and rows below keep their positions; overflow past
                // the card edge is clipped (wheel-pan / filtering have their own
                // tested models).
                const std::vector<chipmodel::Chip> sumChips =
                    chipmodel::Build(d->items);
                // fix thse isseue.md #1: DEDICATED summary-chip pill design via
                // sumchip:: -- semibold 12px text, 8x4 padding, pill radius
                // (>=10px), 6px gaps, border == fill (one device-pixel stroke,
                // no visible second outline), and a height DERIVED from the
                // measured text metrics (NEVER a hardcoded 16). Items/folders use
                // the muted neutral chip token with strong near-black/off-white
                // text; ext chips keep the restrained per-extension tint.
                const int chipPadX = S(sumchip::kPadX);
                const int chipPadY = S(sumchip::kPadY);
                const int chipGap  = S(sumchip::kGap);
                HGDIOBJ chipFont = SelectObject(hdc, UiFontSemibold(dpi));
                int chipTextH = 0;
                {
                    SIZE hx = {0, 0};
                    // A representative cap+descender cell so every chip shares
                    // one height regardless of its own glyphs.
                    GetTextExtentPoint32W(hdc, L"Ag", 2, &hx);
                    chipTextH = hx.cy;
                }
                const int chipH   = sumchip::ChipHeight(chipTextH, chipPadY);
                const int chipRad = sumchip::ChipRadius(chipH, S(sumchip::kMinRadius));
                const int countsH = chipH;   // reserved band tracks the pill height
                const int chipTop = y;       // chips sit at the top of the band
                // fix thse isseue.md #2/#3: the strip is a horizontally-scrolled,
                // clickable viewport [left,right]. Record the viewport + each
                // chip's hit rect (clipped to the viewport, scroll-adjusted) in
                // the list's client coords so the modal loop can hit-test clicks
                // and route the horizontal wheel. The active chip (chipfilter::)
                // gets a stronger-but-restrained accent; a clipped/offscreen chip
                // registers no clickable area. Dimensions NEVER change on state.
                const chipfilter::State chipState = ChipStateFrom(d);
                // fix thse isseue.md #3: reserve the toolbar region (Show
                // extensions toggle) at the right via the tested chiptoolbar::
                // model, so the chip strip viewport is the remaining width and
                // the toggle keeps a stable 12px gap from the strip. Future
                // toolbar actions (Grid view, Larger icons) slot beside it.
                // fix thse isseue.md Phase 3: the toolbar reserves ONLY the
                // compact switch width (a 2:1 pill from the tested showext::
                // model) -- NO space is reserved for a text label anymore.
                const int toggleW = showext::SwitchTrackW(chipH);
                const int toolbarGap = S(12);
                const chiptoolbar::Layout tbar = chiptoolbar::Compute(
                    left, chipTop, right, chipTop + chipH,
                    toggleW, toolbarGap, chipH * 3, chipH);
                const int stripRight = tbar.strip.right;
                d->chipToggleRect = RECT{tbar.toolbar.left, tbar.toolbar.top,
                                         tbar.toolbar.right, tbar.toolbar.bottom};
                // fix thse isseue.md P2 (10249-10250): keep the switch's
                // accessible tooltip aligned to its live rect + current state,
                // so the name stays correct after resize, scroll and on/off.
                SyncExtToggleTip(d);
                d->chipStripRect = RECT{left, chipTop, stripRight, chipTop + chipH};
                d->chipViewportW = stripRight - left;
                d->chipHitRects.clear();
                d->chipHitKind.clear();
                d->chipHitKey.clear();
                const int savedClip = SaveDC(hdc);
                IntersectClipRect(hdc, left, chipTop, stripRight, chipTop + chipH + 1);
                int chipX = left - d->chipScrollOffset;
                int chipIndex = 0;
                for (const chipmodel::Chip& ck : sumChips) {
                    SIZE cts = {0, 0};
                    GetTextExtentPoint32W(hdc, ck.label.c_str(),
                                          static_cast<int>(ck.label.size()), &cts);
                    const int cw = sumchip::ChipWidth(cts.cx, chipPadX);
                    const int chipLeft  = chipX;
                    const int chipRight = chipX + cw;
                    // chipfilter kind/key for THIS chip (int mirrors chipfilter::
                    // Kind: 1 Items, 2 Folders, 3 NoExt, 4 Ext).
                    int fKind = 1;
                    std::wstring fKey;
                    if (ck.kind == chipmodel::Kind::Folders)   fKind = 2;
                    else if (ck.kind == chipmodel::Kind::NoExt) fKind = 3;
                    else if (ck.kind == chipmodel::Kind::Ext) { fKind = 4; fKey = ck.extKey; }
                    // Visible (clipped) hit rect; a fully-offscreen chip gets an
                    // empty rect so a clipped part can never be clicked.
                    const int visL = chipLeft < left ? left : chipLeft;
                    const int visR = chipRight > stripRight ? stripRight : chipRight;
                    if (visR > visL) {
                        d->chipHitRects.push_back(RECT{visL, chipTop, visR, chipTop + chipH});
                    } else {
                        d->chipHitRects.push_back(RECT{0, 0, 0, 0});
                    }
                    d->chipHitKind.push_back(fKind);
                    d->chipHitKey.push_back(fKey);
                    RECT chipR = {chipLeft, chipTop, chipRight, chipTop + chipH};
                    // Default (Items/Folders/No extension): muted neutral fill
                    // with strong primary text; ext chips get the per-ext tint.
                    COLORREF cFill = pal.badgeFill;
                    COLORREF cText = pal.panelText;
                    if (ck.kind == chipmodel::Kind::Ext && !ck.extKey.empty()) {
                        const badgecolor::Chip cc =
                            badgecolor::ChipForLabel(ck.extKey, dark);
                        if (!cc.inherit) {
                            cFill = RGB((cc.fill >> 16) & 0xFF,
                                        (cc.fill >> 8) & 0xFF, cc.fill & 0xFF);
                            cText = RGB((cc.ink >> 16) & 0xFF,
                                        (cc.ink >> 8) & 0xFF, cc.ink & 0xFF);
                        }
                    }
                    // fix thse isseue.md Phase 2A: the ACTIVE filter chip keeps
                    // its muted / per-ext fill -- it is NEVER nudged toward the
                    // Create accent. The dotted outline below is the only
                    // selected-chip indicator, so clicking a chip no longer
                    // layers a second heavy accent fill over it.
                    const bool chipActive =
                        chipState.active() &&
                        static_cast<int>(chipState.kind) == fKind &&
                        (fKind != 4 || chipState.activeKey == fKey);
                    // Border is the SAME color as the fill (one device-pixel
                    // edge, no visible second outline).
                    const COLORREF cBorder = cFill;
                    const RECT chipAA = {chipR.left, chipR.top, chipR.right,
                                         chipR.bottom};
                    if (!DrawRoundSurfaceAA(hdc, chipAA, aarender::Surface::Badge,
                                            chipRad, cFill, true, cBorder, 1)) {
                        HBRUSH cbh = CreateSolidBrush(cFill);
                        HPEN   cph = CreatePen(PS_SOLID, 1, cBorder);
                        HGDIOBJ ocb = SelectObject(hdc, cbh);
                        HGDIOBJ ocp = SelectObject(hdc, cph);
                        RoundRect(hdc, chipR.left, chipR.top, chipR.right,
                                  chipR.bottom, chipRad, chipRad);
                        SelectObject(hdc, ocb);
                        SelectObject(hdc, ocp);
                        DeleteObject(cbh);
                        DeleteObject(cph);
                    }
                    // fix thse isseue.md Phase 2A: a dotted outline marks BOTH
                    // the ACTIVE filter chip and the keyboard-focused chip (the
                    // one selected-chip indicator), via the tested chipfilter::
                    // model, using the existing focus/selection colour -- no
                    // second fill, no permanent outline on inactive chips.
                    if (chipfilter::ChipShowsDottedOutline(
                            chipActive, chipIndex == d->chipFocusIndex)) {
                        HPEN fpen = CreatePen(PS_DOT, 1, pal.createFill);
                        HGDIOBJ ofp = SelectObject(hdc, fpen);
                        HGDIOBJ ofb = SelectObject(hdc, GetStockObject(NULL_BRUSH));
                        RoundRect(hdc, chipR.left, chipR.top, chipR.right,
                                  chipR.bottom, chipRad, chipRad);
                        SelectObject(hdc, ofb);
                        SelectObject(hdc, ofp);
                        DeleteObject(fpen);
                    }
                    SetTextColor(hdc, cText);
                    RECT ctr = {chipR.left, chipR.top, chipR.right, chipR.bottom};
                    DrawTextW(hdc, ck.label.c_str(), -1, &ctr,
                              DT_CENTER | DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS);
                    chipX += cw + chipGap;
                    ++chipIndex;
                }
                // Total content width (last chip's right edge minus the trailing
                // gap, measured from the unscrolled strip start) -> drives the
                // horizontal scroll clamp (chipscroll::).
                d->chipContentW = (chipX - chipGap) - (left - d->chipScrollOffset);
                RestoreDC(hdc, savedClip);
                // fix thse isseue.md Phase 3: the compact Show-extensions
                // switch, immediately after the chip-strip viewport. Default
                // OFF; ON drives the Phase 1B inline-filename rendering. It is a
                // small unlabeled ON/OFF switch (track + sliding knob) that
                // keeps a STABLE size as the state flips (never resizes on click).
                {
                    // fix thse isseue.md Phase 3: a COMPACT theme-aware ON/OFF
                    // switch with NO inline text label (the old check-box glyph
                    // + "Show extensions" text are gone). OFF = neutral track,
                    // knob LEFT; ON = accent track, knob RIGHT. The accessible
                    // name (showext::AccessibleName()) is the tooltip / assistive
                    // label since there is no visible text. Geometry comes from
                    // the tested showext:: switch model; the control keeps a
                    // STABLE size as the state flips.
                    // fix thse isseue.md P2 (10249-10250): the accessible name
                    // is no longer computed-and-discarded here -- it is wired to
                    // a real tooltip by EnsureExtToggleTip()/SyncExtToggleTip()
                    // (state-aware showext::AccessibleLabel()).
                    const RECT tr2 = d->chipToggleRect;
                    const bool on = d->showExt;
                    const int trackH = tr2.bottom - tr2.top;
                    const int trackW = tr2.right - tr2.left;
                    const int swInset = S(2);
                    const COLORREF trackFill = on
                        ? RGB(hovertint::BlendChannel(GetRValue(pal.badgeFill), GetRValue(pal.createFill), 60),
                              hovertint::BlendChannel(GetGValue(pal.badgeFill), GetGValue(pal.createFill), 60),
                              hovertint::BlendChannel(GetBValue(pal.badgeFill), GetBValue(pal.createFill), 60))
                        : pal.badgeFill;
                    const RECT tAA = {tr2.left, tr2.top, tr2.right, tr2.bottom};
                    if (!DrawRoundSurfaceAA(hdc, tAA, aarender::Surface::Badge,
                                            trackH, trackFill, true, trackFill, 1)) {
                        HBRUSH tb = CreateSolidBrush(trackFill);
                        HGDIOBJ otb = SelectObject(hdc, tb);
                        RoundRect(hdc, tr2.left, tr2.top, tr2.right, tr2.bottom,
                                  trackH, trackH);
                        SelectObject(hdc, otb);
                        DeleteObject(tb);
                    }
                    // The sliding knob: a circle at the tested position (LEFT
                    // when OFF, RIGHT when ON) -- the only state indicator, no
                    // text. A 1px border defines it against the track.
                    const int knobD = showext::SwitchKnobDiameter(trackH, swInset);
                    const int knobL = showext::SwitchKnobLeft(tr2.left, trackW,
                                                              trackH, swInset, on);
                    const int knobT = tr2.top + swInset;
                    const RECT knobR = {knobL, knobT, knobL + knobD, knobT + knobD};
                    if (!DrawRoundSurfaceAA(hdc, knobR, aarender::Surface::Badge,
                                            knobD, pal.cardBg, true, pal.cardBorder, 1)) {
                        HBRUSH kb = CreateSolidBrush(pal.cardBg);
                        HPEN   kp = CreatePen(PS_SOLID, 1, pal.cardBorder);
                        HGDIOBJ okb = SelectObject(hdc, kb);
                        HGDIOBJ okp = SelectObject(hdc, kp);
                        RoundRect(hdc, knobR.left, knobR.top, knobR.right,
                                  knobR.bottom, knobD, knobD);
                        SelectObject(hdc, okb);
                        SelectObject(hdc, okp);
                        DeleteObject(kb);
                        DeleteObject(kp);
                    }
                    d->showExtLastPainted = on;
                }
                SelectObject(hdc, chipFont);
                y += countsH;
                y += kSelHdrGap;   // even breathing: type breakdown -> divider

                // Header separator -- a THIN, LIGHT hairline (fix thse isseue.md
                // #1: a thin divider, not a heavy line). Blend the card border
                // toward the card surface so it reads as a subtle 1px rule.
                const COLORREF hairline = RGB(
                    hovertint::BlendChannel(GetRValue(pal.cardBorder), GetRValue(pal.cardBg), 45),
                    hovertint::BlendChannel(GetGValue(pal.cardBorder), GetGValue(pal.cardBg), 45),
                    hovertint::BlendChannel(GetBValue(pal.cardBorder), GetBValue(pal.cardBg), 45));
                HPEN spen = CreatePen(PS_SOLID, 1, hairline);
                HGDIOBJ opn = SelectObject(hdc, spen);
                MoveToEx(hdc, left, y, nullptr);
                LineTo(hdc, right, y);
                SelectObject(hdc, opn);
                DeleteObject(spen);
                y += 1;            // 1px THIN divider
                y += kSelHdrGap;   // even breathing: divider -> rows (lands at the same offset)

                // Level 3 -- the visible rows.
                const mode::Mode lm = d->expanded ? mode::Mode::Expanded
                                                  : mode::Mode::Compact;
                const int itemCount = static_cast<int>(d->items.size());
                const int rowsTop = y;   // top of the rows region (for the track)
                // fix thse isseue.md Phase 1A: the per-mode row count is only a
                // DECLARED maximum. Reconcile it with the TRUE number of rows
                // the remaining viewport can paint ((rc.bottom - rowsTop) at
                // rowH per row), so a declared-but-unfit row is never silently
                // clipped and the scrollbar is enabled whenever fewer rows
                // physically fit than the cap. Cache it so the modal-loop
                // wheel/key handlers page by the SAME capacity.
                // UI-02 / UI-04 (fix thse isseue.md): the Selected-items scroll
                // capacity pages by the SAME height the painter draws -- rowH for
                // the List rows, but the taller shared viewmode::CellH for the
                // Large / Extra large GRID cells (icon + padding + the fixed
                // label band). Feeding rowH for a grid would overcount how many
                // cells fit and clip the last visible row; CellH keeps layout,
                // paint and scroll in one geometry model.
                // fix thse isseue.md Phase 2 + v0.12.68 #1: Large / Extra large
                // are REAL responsive icon grids. selCols comes from the live
                // viewport width (the scrollbar gutter is reserved up front so
                // the column count stays stable) and is cached so the modal-loop
                // scroll handlers page by whole grid rows too. The grid row count
                // is viewmode::SelectedGridVisibleRows -- WHOLE cells only AND
                // never fewer than one, so a viewport too short for a full cell
                // can NEVER paint a blank grid. List keeps the physical-capacity
                // reconciliation (EffectiveVisibleRows). selCols == 1 in List.
                const bool gridMode = viewmode::IsGrid(vmode);
                const int gridGutter =
                    S(listscroll::kScrollbarW) + S(listscroll::kMetaScrollGap);
                // fix thse isseue.md P1 (DPI unit mismatch): viewmode::Columns /
                // SelectedGridVisibleRows divide by LOGICAL (96-dpi) CellW/CellH,
                // so the DEVICE-pixel viewport width/height must be converted
                // back to logical units FIRST -- the same MulDiv(.,96,dpi)
                // contract ComputeEffectiveLayout uses. Feeding device pixels to
                // logical-cell helpers over-counted columns/rows above 100% DPI.
                const int availWLogical = gridMode
                    ? MulDiv((right - left) - gridGutter, 96, static_cast<int>(dpi))
                    : 0;
                const int selCols = gridMode
                    ? viewmode::Columns(vmode, availWLogical)
                    : 1;
                d->selGridCols = selCols;
                const int rowsViewportH = rc.bottom - rowsTop;   // DEVICE px
                // fix thse isseue.md P1: cache the REAL item-row viewport (below
                // the header / chip strip / divider) in list client coords so the
                // modal-loop wheel router scrolls rows ONLY when the pointer is
                // actually over the rows -- not the title, chips or toggle.
                d->listRowsRect = RECT{left, rowsTop, right, rc.bottom};
                const int capRowH = gridMode ? S(viewmode::CellH(vmode)) : rowH;
                const int visRows = gridMode
                    ? viewmode::SelectedGridVisibleRows(
                          vmode, itemCount, selCols,
                          MulDiv(rowsViewportH, 96, static_cast<int>(dpi)))
                    : listscroll::EffectiveVisibleRowsResizable(
                          mode::VisibleRows(lm), rowsViewportH, capRowH,
                          (d->manualSized || IsZoomed(hwnd)));   // fix thse isseue.md v0.12.69 #2: a maximized/resized List fills the viewport (no 4/9-row dead space); preferred keeps its cap
                d->listVisibleRows = visRows;
                const int scrollRows = gridMode
                    ? viewmode::GridRows(itemCount, selCols)
                    : itemCount;
                // v0.12.68 #2: a pending view switch converts the captured first
                // visible item into THIS mode's top row now that selCols/visRows
                // are known, clamped to the valid maximum -- so the user's visible
                // context survives the zoom (both List->grid and grid->List).
                if (d->pendingFirstItem >= 0) {
                    const int convTop = viewmode::TopRowForItem(
                        vmode, d->pendingFirstItem, selCols);
                    d->listTopRow = listscroll::ClampTopRow(
                        convTop, listscroll::MaxTopRow(scrollRows, visRows));
                    d->pendingFirstItem = -1;
                }
                const listscroll::Range vr = listscroll::VisibleRange(
                    d->listTopRow, scrollRows, visRows);
                // fix thse isseu e.md #4: reserve a right-edge gutter for a
                // VISIBLE scrollbar when the list overflows, so rows never draw
                // under the thumb. The scroll math stays the source of truth.
                const bool needsBar =
                    listscroll::NeedsScrollbar(scrollRows, visRows);
                const int barW = S(listscroll::kScrollbarW);
                // fix thse isseue.md Phase B: reserve the gutter PLUS ~8px of
                // breathing room so the rightmost size metadata never butts
                // against the scrollbar gutter. The gutter stays reserved
                // whenever the list overflows, so toggling the auto-hide thumb
                // visibility (below) never reflows the rows (no layout shift).
                const int metaGap = needsBar ? S(listscroll::kMetaScrollGap) : 0;
                const int rowRight =
                    listscroll::ContentWidth(right, needsBar ? (barW + metaGap) : 0, needsBar);
                // fix thse isseue.md #1: the name column tracks the LIVE
                // content width (rowRight already excludes the scrollbar
                // gutter), so the genuinely-wider expanded window fills the
                // extra horizontal space instead of staying pinned to the fixed
                // compact kContentW. Columns are DPI-scaled to match the row.
                // fix thse isseue.md Phase 1B: when Show-extensions is ON the
                // extension is drawn INLINE in the filename and NO separate EXT
                // pill is painted, so the name column RECLAIMS the fixed EXT
                // column width (ext width 0). OFF keeps the reserved EXT column.
                const int extW  = d->showExt ? 0 : S(listcol::kExtW);
                const int sizeW = S(listcol::kSizeW);
                const int nameW = listcol::NameWidthFrom(
                    rowRight - left,
                    extW, sizeW, S(listcol::kColGap),
                    S(listcol::kMinNameW));
                // List mode draws one item per row here; grid modes skip this
                // loop and run the responsive cell grid below (Phase 2).
                for (int r = 0; !gridMode && r < vr.count && y + rowH <= rc.bottom; ++r) {
                    const sel::Item& it =
                        d->items[static_cast<size_t>(vr.first + r)];
                    // fix thse isseue.md #2: apply the ACTIVE chip filter's
                    // highlight to THIS row via the tested chipfilter:: model --
                    // matching rows keep full strength, non-matching rows are
                    // subtly subdued (dimmed text) but stay in their original
                    // position. NEVER changes the real selection or Copy/Move plan.
                    const std::wstring rowExtKey = it.isFolder
                        ? std::wstring()
                        : chipmodel::NormExtKey(sel::ExtLabel(it.name, false));
                    const chipfilter::Row rowFilt =
                        chipfilter::RowFor(chipState, rowExtKey, it.isFolder);
                    const bool rowSubdued = (rowFilt == chipfilter::Row::Subdued);
                    // fix thse isseue.md Phase 5 + ADD-ON: outline the CURRENT
                    // find-navigation match (List row) with the accent, leaving
                    // every OTHER matched row at full strength so the user can
                    // see where they are without losing the other matches.
                    if (chipnav::IsCurrentMatch(vr.first + r, d->chipNavItem)) {
                        const RECT mr = {left, y + S(1), rowRight, y + rowH - S(1)};
                        icons::DrawCurrentMatchOutline(hdc, mr, pal.createBorder);
                    }
                    const COLORREF rowNameColor = rowSubdued
                        ? RGB(hovertint::BlendChannel(GetRValue(pal.panelText), GetRValue(pal.cardBg), 55),
                              hovertint::BlendChannel(GetGValue(pal.panelText), GetGValue(pal.cardBg), 55),
                              hovertint::BlendChannel(GetBValue(pal.panelText), GetBValue(pal.cardBg), 55))
                        : pal.panelText;
                    const COLORREF rowMetaColor = rowSubdued
                        ? RGB(hovertint::BlendChannel(GetRValue(secondary), GetRValue(pal.cardBg), 55),
                              hovertint::BlendChannel(GetGValue(secondary), GetGValue(pal.cardBg), 55),
                              hovertint::BlendChannel(GetBValue(secondary), GetBValue(pal.cardBg), 55))
                        : secondary;
                    // Leading row icon column. fix thse isseue.md #9: draw the
                    // REAL Windows Shell icon for the item's type (folder icon,
                    // or the extension-associated file icon) so rows look just
                    // like File Explorer. SHGFI_USEFILEATTRIBUTES makes it a
                    // pure type/extension lookup (no disk hit); the icon handle
                    // is owned here, so DestroyIcon right after the blit. The
                    // custom vector is kept ONLY as a fallback.
                    // fix thse isseue.md Phase A: the leading icon column and the
                    // drawn Shell icon grow with the presentation mode (List ->
                    // Large -> Extra large) via the tested viewmode:: metrics;
                    // List keeps the exact existing small-icon size/look.
                    const int iconW   = viewmode::IsGrid(vmode)
                        ? S(viewmode::IconPx(vmode) + 8)
                        : S(icons::kRowIconW);
                    const int iconGap = S(icons::kRowIconGap);
                    RECT icr = {left, y + S(2), left + iconW, y + rowH - S(2)};
                    const int shPx = viewmode::IsGrid(vmode)
                        ? S(viewmode::IconPx(vmode))
                        : shellicon::IconPx(d->dpi);
                    // fix thse isseue.md Phase 1: in the GRID modes the icon is
                    // drawn far larger than a 16px small icon, so source it from
                    // the system image-list TIER whose resolution covers shPx
                    // (SHIL_EXTRALARGE 48 / SHIL_JUMBO 256) and DOWNSAMPLE it --
                    // never upscale a small bitmap. The extracted HICON is cached
                    // per (type,tier,dpi) on the popup (GetCachedShellIcon), so
                    // scrolling and zooming reuse ONE source and teardown frees them.
                    // fix thse isseue.md Issue 2: in a grid view, an IMAGE file
                    // shows a real thumbnail preview (async, cached) instead of
                    // its type icon. Non-image files, folders and List view keep
                    // the Shell icon. The thumbnail is aspect-fit (never
                    // stretched) and centered in the icon box; while it is still
                    // loading (or if it fails) the icon fallback below is drawn.
                    bool drewThumb = false;
                    if (viewmode::IsGrid(vmode) && !it.path.empty() &&
                        thumbs::WantsThumbnailFor(true, it.isFolder, it.name)) {
                        int tw = 0, th = 0;
                        HBITMAP tb =
                            GetThumbnailForPaint(d, hwnd, it.path, shPx, &tw, &th);
                        if (tb && tw > 0 && th > 0) {
                            const int iw = icr.right - icr.left;
                            const int ih = icr.bottom - icr.top;
                            const int boxX = icr.left + (iw - shPx) / 2;
                            const int boxY = icr.top  + (ih - shPx) / 2;
                            const thumbs::FitBox fb = thumbs::FitRect(tw, th, shPx, shPx);
                            HDC mdc = CreateCompatibleDC(hdc);
                            if (mdc) {
                                HGDIOBJ omb = SelectObject(mdc, tb);
                                const int oldStretch = SetStretchBltMode(hdc, HALFTONE);
                                SetBrushOrgEx(hdc, 0, 0, nullptr);
                                StretchBlt(hdc, boxX + fb.x, boxY + fb.y, fb.w, fb.h,
                                           mdc, 0, 0, tw, th, SRCCOPY);
                                SetStretchBltMode(hdc, oldStretch);
                                SelectObject(mdc, omb);
                                DeleteDC(mdc);
                                drewThumb = true;
                            }
                        }
                    }
                    HICON shIcon = (!drewThumb && viewmode::IsGrid(vmode))
                        ? GetCachedShellIcon(d, it.name, it.isFolder,
                                             iconsrc::SelectTier(vmode, d->dpi))
                        : nullptr;
                    if (shIcon) {
                        const int iw = icr.right - icr.left;
                        const int ih = icr.bottom - icr.top;
                        const int ix = icr.left + (iw - shPx) / 2;
                        const int iy = icr.top  + (ih - shPx) / 2;
                        // cached high-resolution source -> crisp downsample.
                        DrawIconEx(hdc, ix, iy, shIcon, shPx, shPx, 0,
                                   nullptr, DI_NORMAL);
                        // NOT destroyed here: owned by d->iconCache until the
                        // popup is torn down (no per-paint create/destroy).
                    } else if (!viewmode::IsGrid(vmode)) {
                        // List mode: the DPI-appropriate SMALL Shell icon, whose
                        // source size equals the draw size -- already crisp. The
                        // handle is owned here, so DestroyIcon right after blit.
                        SHFILEINFOW rsfi = {};
                        if (SHGetFileInfoW(it.name.c_str(),
                                           shellicon::AttrFor(it.isFolder),
                                           &rsfi, sizeof(rsfi),
                                           shellicon::IconFlags()) && rsfi.hIcon) {
                            const int iw = icr.right - icr.left;
                            const int ih = icr.bottom - icr.top;
                            const int ix = icr.left + (iw - shPx) / 2;
                            const int iy = icr.top  + (ih - shPx) / 2;
                            DrawIconEx(hdc, ix, iy, rsfi.hIcon, shPx, shPx, 0,
                                       nullptr, DI_NORMAL);
                            DestroyIcon(rsfi.hIcon);
                        } else {
                            icons::DrawIcon(hdc,
                                            it.isFolder ? icons::IconId::Folder
                                                        : icons::IconId::File,
                                            icr, secondary);
                        }
                    } else {
                        // Grid mode with no high-res source available (e.g. no
                        // SHGetImageList): draw the crisp VECTOR glyph instead of
                        // upscaling a 16px bitmap to 48/96px (fix thse isseue.md
                        // Phase 1A -- never silently upscale).
                        icons::DrawIcon(hdc,
                                        it.isFolder ? icons::IconId::Folder
                                                    : icons::IconId::File,
                                        icr, secondary);
                    }
                    // name (left of the icon column, ellipsized)
                    const int nameLeft = left + iconW + iconGap;
                    SetTextColor(hdc, rowNameColor);
                    RECT nr = {nameLeft, y, nameLeft + nameW - iconW - iconGap,
                               y + rowH};
                    // fix thse isseue.md Phase 1B: the name honors the
                    // Show-extensions toggle -- stem when OFF, the COMPLETE
                    // filename inline when ON (folders always whole-name) -- via
                    // the tested sel::RowDisplayName model. DT_END_ELLIPSIS over
                    // the (ext-reclaimed) name width keeps the filename and size
                    // badge from ever overlapping.
                    const bool extInline =
                        showext::RowShowsExtInline(d->showExt, it.isFolder);
                    std::wstring nm =
                        sel::RowDisplayName(it.name, it.isFolder, extInline);
                    DrawTextW(hdc, nm.c_str(), -1, &nr,
                              DT_LEFT | DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS);
                    // SIZE (far right, left of the scrollbar gutter). fix thse
                    // isseue.md #1: the real filesystem size is the DEFAULT
                    // right-hand metadata for files with a known size, and is
                    // NEVER drawn for folders -- the decision is the tested
                    // showext::RowShowsSize model (sizeKnown == non-empty
                    // HumanSize text).
                    std::wstring sz = sel::HumanSize(it.sizeBytes);
                    if (showext::RowShowsSize(it.isFolder, !sz.empty())) {
                        SetTextColor(hdc, rowMetaColor);
                        RECT szr = {rowRight - sizeW, y, rowRight, y + rowH};
                        DrawTextW(hdc, sz.c_str(), -1, &szr,
                                  DT_RIGHT | DT_SINGLELINE | DT_VCENTER);
                    }
                    // fix thse isseue.md Phase 1B: there is NO separate
                    // extension pill. When Show-extensions is ON the complete
                    // filename is drawn INLINE in the name cell above
                    // (sel::RowDisplayName) and the fixed EXT column width is
                    // reclaimed by the name column; when OFF the row shows the
                    // stem only. Either way the size badge keeps its own
                    // right-hand metadata column, so sizes stay visible.
                    y += rowH;
                }

                // fix thse isseue.md Phase 2: the real responsive icon GRID for
                // Large / Extra large. The single-column loop above is skipped
                // in grid mode; here each VISIBLE grid row (vr is in grid-row
                // units) is expanded into selCols cells laid left-to-right. Each
                // cell centers the crisp high-resolution Shell icon (the Phase 1
                // cache) over a centered, wrapped + ellipsized filename, with
                // consistent cell padding so icons and labels never clip or
                // collide. The active chip filter subdues non-matching cells
                // WITHOUT reordering or changing the real Explorer selection.
                if (gridMode) {
                    const int cellW   = S(viewmode::CellW(vmode));
                    const int cellH   = S(viewmode::CellH(vmode));  // UI-02: shared model = icon + padding + label band (NOT rowH, which clipped the filename beneath the icon)
                    const int gIconPx = S(viewmode::IconPx(vmode));
                    for (int gr = 0;
                         gr < vr.count &&
                         (rowsTop + (gr + 1) * cellH) <= rc.bottom;
                         ++gr) {
                        const int gridRow = vr.first + gr;
                        const int cellTop = rowsTop + gr * cellH;
                        for (int c = 0; c < selCols; ++c) {
                            const int itemIdx = gridRow * selCols + c;
                            if (itemIdx >= itemCount) break;
                            const sel::Item& git =
                                d->items[static_cast<size_t>(itemIdx)];
                            const int cellLeft = left + c * cellW;
                            // active chip filter -> subdue (never reorders)
                            const std::wstring gExtKey = git.isFolder
                                ? std::wstring()
                                : chipmodel::NormExtKey(sel::ExtLabel(git.name, false));
                            const bool gSub =
                                chipfilter::RowFor(chipState, gExtKey, git.isFolder)
                                    == chipfilter::Row::Subdued;
                            // fix thse isseue.md Phase 5 + ADD-ON: outline the
                            // CURRENT find-navigation match CELL (Large / Extra
                            // large) with the accent, leaving the other matched
                            // cells at full strength.
                            if (chipnav::IsCurrentMatch(itemIdx, d->chipNavItem)) {
                                const RECT mcr = {cellLeft + S(2), cellTop + S(2),
                                                  cellLeft + cellW - S(2),
                                                  cellTop + cellH - S(2)};
                                icons::DrawCurrentMatchOutline(hdc, mcr, pal.createBorder);
                            }
                            const COLORREF gName = gSub
                                ? RGB(hovertint::BlendChannel(GetRValue(pal.panelText), GetRValue(pal.cardBg), 55),
                                      hovertint::BlendChannel(GetGValue(pal.panelText), GetGValue(pal.cardBg), 55),
                                      hovertint::BlendChannel(GetBValue(pal.panelText), GetBValue(pal.cardBg), 55))
                                : pal.panelText;
                            // crisp high-res icon centered in the cell's top band
                            const int ix = cellLeft + (cellW - gIconPx) / 2;
                            const int iy = cellTop + S(4);
                            RECT gicr = {ix, iy, ix + gIconPx, iy + gIconPx};
                            // fix thse isseue.md v0.12.67 (Issue #2): in the
                            // GRID modes an IMAGE file shows a REAL async
                            // thumbnail preview (cached) in the cell's icon box.
                            // This is the ACTUAL grid renderer -- the previous
                            // build only ever called GetCachedShellIcon here, so
                            // the thumbnail pipeline (worker + cache + repaint,
                            // all already present) never reached the screen and
                            // every image fell back to its generic Shell icon.
                            // Non-image files, folders and List view keep the
                            // Shell icon. The thumbnail is aspect-fit (never
                            // stretched) and centered in the gIconPx box; while
                            // it is still loading (or if extraction genuinely
                            // fails) the crisp Shell-icon fallback below is drawn.
                            // Async completion posts WM_APP_THUMB_READY, which
                            // invalidates control 110 so the cell repaints.
                            bool gDrewThumb = false;
                            if (!git.isFolder && !git.path.empty() &&
                                thumbs::WantsThumbnailFor(true, git.isFolder,
                                                          git.name)) {
                                int gtw = 0, gth = 0;
                                HBITMAP gtb = GetThumbnailForPaint(
                                    d, hwnd, git.path, gIconPx, &gtw, &gth);
                                if (gtb && gtw > 0 && gth > 0) {
                                    const thumbs::FitBox gfb =
                                        thumbs::FitRect(gtw, gth, gIconPx, gIconPx);
                                    HDC gmdc = CreateCompatibleDC(hdc);
                                    if (gmdc) {
                                        HGDIOBJ gomb = SelectObject(gmdc, gtb);
                                        const int goldStretch =
                                            SetStretchBltMode(hdc, HALFTONE);
                                        SetBrushOrgEx(hdc, 0, 0, nullptr);
                                        StretchBlt(hdc, ix + gfb.x, iy + gfb.y,
                                                   gfb.w, gfb.h, gmdc, 0, 0,
                                                   gtw, gth, SRCCOPY);
                                        SetStretchBltMode(hdc, goldStretch);
                                        SelectObject(gmdc, gomb);
                                        DeleteDC(gmdc);
                                        gDrewThumb = true;
                                    }
                                }
                            }
                            HICON gIcon = (!gDrewThumb)
                                ? GetCachedShellIcon(
                                      d, git.name, git.isFolder,
                                      iconsrc::SelectTier(vmode, d->dpi))
                                : nullptr;
                            if (gIcon) {
                                DrawIconEx(hdc, ix, iy, gIcon, gIconPx, gIconPx,
                                           0, nullptr, DI_NORMAL);
                            } else if (!gDrewThumb) {
                                icons::DrawIcon(hdc,
                                                git.isFolder ? icons::IconId::Folder
                                                             : icons::IconId::File,
                                                gicr, secondary);
                            }
                            // centered, wrapped + ellipsized filename below
                            std::wstring gnm = sel::RowDisplayName(
                                git.name, git.isFolder,
                                showext::RowShowsExtInline(d->showExt, git.isFolder));
                            RECT glr = {cellLeft + S(2), iy + gIconPx + S(2),
                                        cellLeft + cellW - S(2), cellTop + cellH};
                            SetTextColor(hdc, gName);
                            DrawTextW(hdc, gnm.c_str(), -1, &glr,
                                      DT_CENTER | DT_WORDBREAK | DT_END_ELLIPSIS |
                                      DT_EDITCONTROL | DT_NOPREFIX);
                        }
                    }
                }

                // fix thse isseu e.md #4: a VISIBLE scrollbar (track + thumb)
                // in the reserved right-edge gutter, shown only when the list
                // overflows. Position/size come from the tested listscroll::
                // model (ThumbRect over the rows region), so the existing scroll
                // math remains the single source of truth.
                // fix thse isseue.md Phase B: AUTO-HIDE. The thumb is painted
                // only while the pointer is inside the viewport or within the
                // short reveal window after the last scroll (sbauto:: model); a
                // one-shot idle timer (id 113) triggers the fade repaint. The
                // gutter above stays reserved on overflow, so this never reflows
                // the rows. (The thumb is display-only -- wheel/keys scroll it --
                // so there is no drag state to keep it pinned.)
                const bool sbWithinReveal = sbauto::WithinRevealWindow(
                    d->listSbActivityTick, GetTickCount64(), sbauto::kIdleHideMs);
                const bool sbVisible = sbauto::Visible(
                    needsBar, /*dragging*/ false, d->listPointerInside, sbWithinReveal);
                if (needsBar && sbVisible) {
                    const listscroll::Box track = listscroll::TrackRect(
                        rowRight, rowsTop, right, rc.bottom, barW);
                    const int trackH = track.bottom - track.top;
                    // Track groove.
                    HBRUSH trk = CreateSolidBrush(pal.editBg);
                    RECT trc = {track.left, track.top, track.right, track.bottom};
                    FillRect(hdc, &trc, trk);
                    DeleteObject(trk);
                    // Thumb.
                    const listscroll::Thumb th = listscroll::ThumbRect(
                        d->listTopRow, scrollRows, visRows, trackH, S(16));
                    const int pad = S(listscroll::kScrollbarPad);
                    const int rad = S(3) * 2;
                    const RECT thumbRc = {track.left + pad, track.top + th.y,
                                          track.right - pad,
                                          track.top + th.y + th.h};
                    // fix thse isseue.md #2/#12 + #5 (RoundRect scan): the
                    // Selected Items scrollbar thumb was the LAST raw GDI
                    // RoundRect. Route it through the shared AA layer so its
                    // corners anti-alias like every other rounded surface; keep
                    // the GDI RoundRect as the per-frame fallback when D2D is off.
                    if (!DrawRoundSurfaceAA(hdc, thumbRc, aarender::Surface::ScrollThumb,
                                            rad, secondary, false, 0, 0)) {
                        HBRUSH thumbBr = CreateSolidBrush(secondary);
                        HGDIOBJ ob = SelectObject(hdc, thumbBr);
                        HGDIOBJ op = SelectObject(hdc, GetStockObject(NULL_PEN));
                        RoundRect(hdc, thumbRc.left, thumbRc.top,
                                  thumbRc.right, thumbRc.bottom, rad, rad);
                        SelectObject(hdc, op);
                        SelectObject(hdc, ob);
                        DeleteObject(thumbBr);
                    }
                }
                SelectObject(hdc, oldFont);
                // fix thse isseue.md #2: flush the backbuffer to the control in
                // ONE BitBlt, then tear it down (no per-row flush -> no flicker).
                SetViewportOrgEx(hdc, 0, 0, nullptr);
                BitBlt(target, rc.left, rc.top, bbW, bbH, hdc, 0, 0, SRCCOPY);
                SelectObject(hdc, bbOld);
                DeleteObject(bbBmp);
                DeleteDC(hdc);
                return TRUE;
            }
            // ADD-ON (Phase D): the timed Auto Group button (id 120). It reuses
            // the EXACT secondary-button surface (same palette tokens, same
            // rounded AA fill + 1px border) as Cancel so it never introduces a
            // new visual style. fix thse isseue.md #5: while the eight-second
            // delay is Waiting the button keeps that normal muted surface (NO
            // black lock rectangle) and shows a stable, SEMIBOLD, centered M:SS
            // countdown (agcd::CountdownText -> 0:08..0:01, no milliseconds).
            // Once Enabled it shows the plain "Auto Group" caption. The state is
            // read fresh from the monotonic clock so it matches WM_TIMER exactly.
            if (dis->CtlType == ODT_BUTTON && dis->CtlID == 120 && d) {
                // fix thse isseue.md Issue 7: DOUBLE-BUFFER the Auto Group button.
                // It composites OFF-SCREEN (panel fill -> rounded surface ->
                // countdown) and reaches the control in ONE BitBlt, so a
                // whole-second repaint never briefly shows the panel background
                // between the full-rect fill and the rounded button -- which was
                // the per-second flash. A child button's rcItem is {0,0,w,h}, so
                // the backbuffer shares the control's paint coordinate space.
                HDC realDC = dis->hDC;
                const int bbW = dis->rcItem.right - dis->rcItem.left;
                const int bbH = dis->rcItem.bottom - dis->rcItem.top;
                HDC memDC = CreateCompatibleDC(realDC);
                HBITMAP memBmp = memDC ? CreateCompatibleBitmap(realDC, bbW, bbH) : nullptr;
                HGDIOBJ oldMemBmp = (memDC && memBmp) ? SelectObject(memDC, memBmp) : nullptr;
                HDC hdc = (memDC && memBmp) ? memDC : realDC;
                const bool pressed = (dis->itemState & ODS_SELECTED) != 0;
                const UINT dpi = d->dpi;
                const int rad = ScaleDpi(14, dpi);
                ThemePalette pal = ResolveThemePalette(IsDarkMode());
                const bool en = AtomicGet(&g_autoGroupEnabled) != 0;
                const agcd::State st =
                    agcd::Resolve(en, d->autoGroupStartTick, GetTickCount64());
                RECT rc = dis->rcItem;
                // fix thse isseue.md Phase 5A/5B: appearance follows agbtn (the
                // mirrored pure model). ENABLED -> dedicated muted-GREEN tokens
                // (hover nudges, pressed deepens), so the bulk-organise action
                // reads as distinct from a normal Group; WAITING / Unavailable
                // -> the NEUTRAL Cancel-style secondary surface the bold
                // countdown sits on. No bare RGB() here -- only theme tokens.
                const bool hot = (dis->itemState & ODS_HOTLIGHT) != 0;
                COLORREF fill, border, txt;
                if (agbtn::UsesGroupFill(st)) {
                    // fix thse isseue.md top note ("green color is not good for
                    // auto group - keep it same as group"): the ENABLED Auto
                    // Group button is painted IDENTICALLY to the ordinary Group
                    // button -- the same create/primary tokens with the SAME
                    // hovertint hover nudge (never while pressed) -- so it reads
                    // as a Group action and never a dedicated green surface.
                    fill = pressed ? pal.createFillPressed : pal.createFill;
                    border = pal.createBorder;
                    txt = pal.createText;
                    if (hot && !pressed) {
                        fill = RGB(
                            hovertint::BlendChannel(GetRValue(fill), 255, hovertint::kHoverPct),
                            hovertint::BlendChannel(GetGValue(fill), 255, hovertint::kHoverPct),
                            hovertint::BlendChannel(GetBValue(fill), 255, hovertint::kHoverPct));
                    }
                } else {
                    // Waiting / Unavailable: neutral Cancel-style surface the
                    // bold M:SS countdown sits on.
                    fill = pressed ? pal.cancelFillPressed : pal.cancelFill;
                    border = pal.cancelBorder;
                    txt = pal.cancelText;
                }
                // Parent panel surface behind the square HWND corners.
                HBRUSH pbg = CreateSolidBrush(pal.panelBg);
                FillRect(hdc, &rc, pbg);
                DeleteObject(pbg);
                if (!DrawRoundSurfaceAA(hdc, rc, aarender::Surface::Button,
                                        rad, fill, true, border, 1)) {
                    HBRUSH fb = CreateSolidBrush(fill);
                    HPEN pen = CreatePen(PS_SOLID, 1, border);
                    HGDIOBJ ob = SelectObject(hdc, fb);
                    HGDIOBJ op = SelectObject(hdc, pen);
                    RoundRect(hdc, rc.left, rc.top, rc.right, rc.bottom, rad, rad);
                    SelectObject(hdc, op);
                    SelectObject(hdc, ob);
                    DeleteObject(fb);
                    DeleteObject(pen);
                }
                SetBkMode(hdc, TRANSPARENT);
                if (agcd::ShowOverlay(st)) {
                    // fix thse isseue.md #5: during the safety delay the button
                    // keeps its NORMAL muted secondary surface (already filled
                    // above) -- NO black lock rectangle -- and shows a STABLE,
                    // SEMIBOLD, centered M:SS countdown (0:08..0:01) with NO
                    // milliseconds. The text is the whole-second value, so it
                    // changes at most once per second (no flashing suffix).
                    const int secs = agcd::RemainingSeconds(
                        en, d->autoGroupStartTick, GetTickCount64());
                    std::wstring cd = agcd::CountdownText(secs);
                    HGDIOBJ ofc = SelectObject(hdc, UiFontSemibold(dpi));
                    SetTextColor(hdc, txt);
                    DrawTextW(hdc, cd.c_str(), -1, &rc,
                              DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOCLIP);
                    SelectObject(hdc, ofc);
                } else {
                    // fix thse isseue.md Phase 5A: the "Auto Group" caption is
                    // SEMIBOLD (agbtn::CaptionSemibold), matching the other
                    // action labels and the bold countdown.
                    HGDIOBJ of = SelectObject(hdc, UiFontSemibold(dpi));
                    SetTextColor(hdc, txt);
                    DrawTextW(hdc, L"Auto Group", -1, &rc,
                              DT_CENTER | DT_VCENTER | DT_SINGLELINE);
                    SelectObject(hdc, of);
                }
                // fix thse isseue.md Issue 7: flush the fully-composed button to
                // the control in ONE BitBlt (no panel-background frame between
                // the fill and the rounded surface), then tear the backbuffer
                // down. If the backbuffer could not be created we painted
                // straight to the control DC, so only clean up the DC.
                if (memDC && memBmp) {
                    BitBlt(realDC, dis->rcItem.left, dis->rcItem.top, bbW, bbH,
                           memDC, 0, 0, SRCCOPY);
                    SelectObject(memDC, oldMemBmp);
                    DeleteObject(memBmp);
                    DeleteDC(memDC);
                } else if (memDC) {
                    DeleteDC(memDC);
                }
                return TRUE;
            }
            // fix thse isseue.md #1: the progress track (id 106) was removed
            // together with the Status/Ready feature.
            if (dis->CtlType == ODT_BUTTON &&
                (dis->CtlID == IDOK || dis->CtlID == IDCANCEL ||
                 dis->CtlID == 107 || dis->CtlID == 108 ||
                 dis->CtlID == 111 || dis->CtlID == 121)) {
                bool pressed = (dis->itemState & ODS_SELECTED) != 0;
                bool isCreate = (dis->CtlID == IDOK);
                // Copy | Move segments (Phase 6/D, ids 107/108). The ACTIVE
                // segment (matching segmented::FromIsMove(d->isMove)) is filled
                // like the primary/accent so the current choice reads at a
                // glance; the inactive one is the secondary style. d->isMove is
                // the single source of truth, so the highlight can never
                // disagree with what the worker will do.
                bool isSeg = (dis->CtlID == 107 || dis->CtlID == 108);
                bool segActive = false;
                if (isSeg && d) {
                    segmented::Seg cur = segmented::FromIsMove(d->isMove);
                    segActive = (dis->CtlID == 107 && cur == segmented::Seg::Copy) ||
                                (dis->CtlID == 108 && cur == segmented::Seg::Move);
                }
                RECT rc = dis->rcItem;
                // fix thse isseue.md #5 B: double-buffer the Copy/Move segments so
                // the sliding pill composites OFF-SCREEN and reaches the control in
                // ONE blit -- no flash of the card background between the neutral
                // track fill and the accent pill. A child button's rcItem is
                // {0,0,w,h}, so the backbuffer shares the paint coordinate space.
                // Create/Cancel/Copy keep painting straight to the control DC.
                HDC realDC = dis->hDC;
                const int segBBW = rc.right - rc.left;
                const int segBBH = rc.bottom - rc.top;
                HDC segMemDC = nullptr; HBITMAP segBmp = nullptr; HGDIOBJ segOldBmp = nullptr;
                if (isSeg) {
                    segMemDC = CreateCompatibleDC(realDC);
                    if (segMemDC) {
                        segBmp = CreateCompatibleBitmap(realDC, segBBW, segBBH);
                        if (segBmp) { segOldBmp = SelectObject(segMemDC, segBmp); }
                        else { DeleteDC(segMemDC); segMemDC = nullptr; }
                    }
                }
                HDC hdc = segMemDC ? segMemDC : realDC;

                ThemePalette pal = ResolveThemePalette(IsDarkMode());
                COLORREF fill;
                COLORREF txt;
                COLORREF border;
                // fix thse isseue.md #5 C: the sliding ACTIVE PILL is drawn ON TOP
                // of the neutral track from segglide::PillBandPct; these carry the
                // eased progress + the muted accent fill into the render below.
                int segProg = 0;
                COLORREF pillFill = 0;
                if (isCreate) {
                    fill = pressed ? pal.createFillPressed : pal.createFill;
                    txt = pal.createText;
                    border = pal.createBorder;
                } else if (isSeg) {
                    // fix thse isseue.md #5 C: the TRACK is a steady neutral
                    // surface; the ACTIVE PILL (drawn on top below) is what slides.
                    // The segment's base fill/border are therefore the calm neutral
                    // secondary tokens -- hover/pressed/focus adjust THEM, never the
                    // pill, so animation and interaction feedback stay independent.
                    fill   = pressed ? pal.cancelFillPressed : pal.cancelFill;
                    border = pal.cancelBorder;
                    // Labels/icons stay put during the slide (issue #5 C): the TEXT
                    // colour tracks the COMMITTED choice (d->isMove via segActive),
                    // not the animation, so it never flickers mid-glide.
                    txt = segActive ? pal.createText : pal.cancelText;
                    // Eased progress for the pill position; the muted accent fill is
                    // kFillAccentPct of the way from neutral toward the Create accent
                    // (segglide::MutedAccentFrac(kMax)) so the pill reads calm (#6).
                    segProg = d ? d->glideProgress
                                : (segActive ? segglide::kMax : segglide::kMin);
                    const COLORREF accFill = pressed ? pal.createFillPressed
                                                     : pal.createFill;
                    const int pillPct = segglide::MutedAccentFrac(segglide::kMax);
                    pillFill = RGB(
                        hovertint::BlendChannel(GetRValue(fill), GetRValue(accFill), pillPct),
                        hovertint::BlendChannel(GetGValue(fill), GetGValue(accFill), pillPct),
                        hovertint::BlendChannel(GetBValue(fill), GetBValue(accFill), pillPct));
                } else {
                    // Cancel = secondary styling.
                    fill = pressed ? pal.cancelFillPressed : pal.cancelFill;
                    txt = pal.cancelText;
                    border = pal.cancelBorder;
                    // fix thse isseue.md Issue 1: the '?' navigation-help button
                    // (id 121) is a GREEN pill with a bold white glyph instead of
                    // the neutral Cancel styling. The hover tint below lightens
                    // this green for clear hover feedback.
                    if (dis->CtlID == 121) {
                        fill   = pressed ? kColConfirmPressed : kColConfirm;
                        txt    = kColWhite;
                        border = kColConfirmPressed;
                    }
                }

                // fix thse isseue.md #14: HOVER feedback. The caption buttons
                // already react to ODS_HOTLIGHT; give Create/Cancel + the
                // Copy/Move segments the same affordance by lightening the
                // resolved fill a touch on hover (not when pressed -- pressed
                // keeps its own fill token, and the focus ring is drawn below).
                // Single source of truth: hovertint:: (unit-tested).
                const hovertint::Vis hvis = hovertint::VisFor(
                    (dis->itemState & ODS_HOTLIGHT) != 0, pressed);
                if (hovertint::PaintsHover(hvis)) {
                    fill = RGB(
                        hovertint::BlendChannel(GetRValue(fill), 255, hovertint::kHoverPct),
                        hovertint::BlendChannel(GetGValue(fill), 255, hovertint::kHoverPct),
                        hovertint::BlendChannel(GetBValue(fill), 255, hovertint::kHoverPct));
                }

                // fix thse isseue.md: the Cancel button gives RED press feedback when
                // clicked, matching the Close (X) caption button. Scoped to
                // IDCANCEL + pressed so the inactive Copy/Move segment (which
                // shares the same secondary styling) is NOT affected.
                if (dis->CtlID == IDCANCEL && pressed) {
                    fill   = RGB(193, 15, 30);
                    txt    = RGB(255, 255, 255);
                    border = RGB(193, 15, 30);
                }

                // fix thse isseue.md #2/#5: GREEN press feedback for the Create
                // button and the Copy-path button (id 111) -- the confirm-colour
                // counterpart to the Cancel/Close red above. btn::PressTone
                // (unit-tested) owns the semantic; kColConfirmPressed is the one
                // green source. Scoped to `pressed`, so idle/hover/focus are
                // unaffected, and only to those two confirm buttons.
                {
                    const btn::Button pbtn =
                        isCreate ? btn::Button::Create
                                 : (dis->CtlID == 111 ? btn::Button::Copy
                                                      : btn::Button::Other);
                    if (pressed && btn::PressTone(pbtn) == btn::Tone::Green) {
                        fill   = kColConfirmPressed;
                        txt    = kColWhite;
                        border = kColConfirmPressed;
                    }
                }

                // fix thse isseue.md #4: focus feedback is a COLOUR change, not
                // an outline. Keyboard-focus-visible (focusvis:: gate, so a
                // MOUSE click shows nothing) recolours the control border to the
                // accent and nudges the fill toward it, so Create/Cancel and the
                // Copy/Move segments all react on the button itself. Hover
                // (above) and pressed keep their own fills -- states stay
                // separate. The old painted focus RING is gone.
                const bool focusColor = focusvis::ShouldShowRing(
                    (dis->itemState & ODS_FOCUS) != 0,
                    d ? d->focusVisible : false);
                if (focusColor) {
                    border = pal.createBorder;  // subtle accent focus colour
                    if (!pressed) {
                        fill = RGB(
                            hovertint::BlendChannel(GetRValue(fill), GetRValue(pal.createBorder), hovertint::kFocusPct),
                            hovertint::BlendChannel(GetGValue(fill), GetGValue(pal.createBorder), hovertint::kFocusPct),
                            hovertint::BlendChannel(GetBValue(fill), GetBValue(pal.createBorder), hovertint::kFocusPct));
                    }
                }

                // fix thse isseue.md #3: NO window-region clipping. Fill the
                // HWND with the MATCHING parent surface (card for the segments,
                // panel for Create/Cancel), then paint the rounded fill + border
                // INSIDE it, so the square HWND corners simply show the same
                // surface -- a rounded look with no clip and no corner artifact.
                const UINT dpi = d ? d->dpi : 96;
                const int rad = ScaleDpi(14, dpi);
                // fix thse isseue.md (path pass): the Copy button (id 111) sits
                // ON the Destination CARD, so its HWND background must be the
                // card surface (like the segments), NOT the panel -- otherwise a
                // panel-coloured patch would show behind the rounded button.
                COLORREF parentBg = (isSeg || dis->CtlID == 111) ? pal.cardBg : pal.panelBg;
                HBRUSH pbg = CreateSolidBrush(parentBg);
                FillRect(hdc, &rc, pbg);
                DeleteObject(pbg);
                HBRUSH fb = CreateSolidBrush(fill);
                HPEN pen = CreatePen(PS_SOLID, 1, border);
                HGDIOBJ ob = SelectObject(hdc, fb);
                HGDIOBJ op = SelectObject(hdc, pen);
                if (isSeg) {
                    // fix thse isseue.md #5 C: paint the neutral TRACK first --
                    // Copy (107) rounds the LEFT, Move (108) the RIGHT; the rounded
                    // side is pushed past the HWND edge so the shared middle clips
                    // to a STRAIGHT seam with no border line.
                    // #2/#12: anti-aliased segment via Direct2D; GDI fallback.
                    if (!DrawRoundSegmentAA(hdc, rc, aarender::Surface::Segment,
                                            rad * 2, dis->CtlID == 107, fill,
                                            border, 1)) {
                        if (dis->CtlID == 107)
                            RoundRect(hdc, rc.left, rc.top, rc.right + rad, rc.bottom, rad * 2, rad * 2);
                        else
                            RoundRect(hdc, rc.left - rad, rc.top, rc.right, rc.bottom, rad * 2, rad * 2);
                    }
                    // fix thse isseue.md #5 C: the sliding ACTIVE PILL on top of the
                    // track. segglide::PillBandPct (unit-tested) gives the sub-span
                    // of THIS half the pill covers at the eased progress; it hugs
                    // the seam and its moving (outer) end rounds exactly like the
                    // track, so the two halves' bands form ONE pill that slides
                    // across the seam. Width-conserving == always exactly one pill.
                    const segglide::PillBand band =
                        segglide::PillBandPct(segProg, dis->CtlID == 107);
                    if (band.endPct > band.startPct) {
                        const int halfW = rc.right - rc.left;
                        RECT pr = {rc.left + band.startPct * halfW / 100, rc.top,
                                   rc.left + band.endPct   * halfW / 100, rc.bottom};
                        HBRUSH pbr = CreateSolidBrush(pillFill);
                        HPEN   ppn = CreatePen(PS_SOLID, 1, pillFill);
                        HGDIOBJ opb = SelectObject(hdc, pbr);
                        HGDIOBJ opp = SelectObject(hdc, ppn);
                        if (!DrawRoundSegmentAA(hdc, pr, aarender::Surface::Segment,
                                                rad * 2, dis->CtlID == 107, pillFill,
                                                pillFill, 1)) {
                            if (dis->CtlID == 107)
                                RoundRect(hdc, pr.left, pr.top, pr.right + rad, pr.bottom, rad * 2, rad * 2);
                            else
                                RoundRect(hdc, pr.left - rad, pr.top, pr.right, pr.bottom, rad * 2, rad * 2);
                        }
                        SelectObject(hdc, opp);
                        SelectObject(hdc, opb);
                        DeleteObject(pbr);
                        DeleteObject(ppn);
                    }
                } else {
                    // #2/#12: anti-aliased button via Direct2D; GDI fallback.
                    if (!DrawRoundSurfaceAA(hdc, rc, aarender::Surface::Button,
                                            rad, fill, true, border, 1)) {
                        RoundRect(hdc, rc.left, rc.top, rc.right, rc.bottom, rad, rad);
                    }
                }
                SelectObject(hdc, op);
                SelectObject(hdc, ob);
                DeleteObject(fb);
                DeleteObject(pen);

                // fix thse isseue.md #1: NO internal divider line at the seam.
                // The old 1px cardBorder divider was stroked ON TOP of the
                // finished pill (seamX at rc.right-1 for Copy / rc.left for
                // Move), so whenever the sliding pill sat at or crossed the
                // seam the line cut straight through the blue pill and the
                // control read as two layered halves. It is gone: the active
                // half's accent pill vs the inactive half's neutral track give
                // the separation, and the pushed-out single-side rounding
                // already clips each half to a clean borderless straight seam,
                // so the two halves compose into ONE continuous pill with no
                // line through it. (Do not re-add a divider to "cover" a seam.)

                wchar_t caption[32] = {0};
                GetWindowTextW(dis->hwndItem, caption, ARRAYSIZE(caption));
                SetBkMode(hdc, TRANSPARENT);
                SetTextColor(hdc, txt);
                HGDIOBJ of = SelectObject(hdc, UiFont(dpi));
                if (isSeg) {
                    // fix thse isseue.md #9: a SMALL leading Copy/Move glyph
                    // before the caption; the icon+label group is centred as a
                    // unit via the tested segicon:: model. Copy (107) uses the
                    // copy glyph, Move (108) the move glyph, both in the
                    // caption's own colour (we do NOT overdo it with a 2nd accent).
                    SIZE tsz = {0, 0};
                    GetTextExtentPoint32W(hdc, caption, lstrlenW(caption), &tsz);
                    const int ico = ScaleDpi(segicon::kSegIcon, dpi);
                    const int gap = ScaleDpi(segicon::kSegIconGap, dpi);
                    const int groupW = ico + gap + tsz.cx;
                    const int gl = segicon::GroupLeft(rc.left, rc.right - rc.left,
                                                      groupW);
                    const int icoTop = rc.top + ((rc.bottom - rc.top) - ico) / 2;
                    RECT ir = {gl, icoTop, gl + ico, icoTop + ico};
                    // fix thse isseue.md #1: Copy/Move use EMOJI (not the custom
                    // vector glyphs), drawn in the same centred icon box.
                    DrawTextW(hdc,
                              dis->CtlID == 107 ? L"\U0001F4CB" : L"\U0001F4C2",
                              -1, &ir,
                              DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOCLIP);
                    RECT tr = {gl + ico + gap, rc.top, rc.right, rc.bottom};
                    // fix thse isseue.md #5 A: draw the ACCESS-KEY label (&Copy /
                    // &Move) WITH prefix processing (no DT_NOPREFIX) so the C / M
                    // mnemonic is UNDERLINED. The measurement above uses the plain
                    // caption, whose visible width is identical, so the icon+label
                    // group stays centred.
                    const std::wstring segMnemonic = segmented::MnemonicLabel(
                        dis->CtlID == 107 ? segmented::Seg::Copy
                                          : segmented::Seg::Move);
                    DrawTextW(hdc, segMnemonic.c_str(), -1, &tr,
                              DT_LEFT | DT_VCENTER | DT_SINGLELINE);
                } else if (dis->CtlID == 121) {
                    // fix thse isseue.md Issue 1: a bold white "?" glyph on the
                    // green navigation-help button -- NOT the window text (which
                    // is the accessible name "Navigation help").
                    HGDIOBJ ofq = SelectObject(hdc, UiFontSemibold(dpi));
                    DrawTextW(hdc, L"?", -1, &rc,
                              DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOCLIP);
                    SelectObject(hdc, ofq);
                } else {
                    DrawTextW(hdc, caption, -1, &rc,
                              DT_CENTER | DT_VCENTER | DT_SINGLELINE);
                }
                // fix thse isseue.md #4: the painted focus RING was removed.
                // Keyboard-focus feedback is now a COLOUR change (accent border
                // + fill nudge) applied with the fill above via focusvis::; a
                // mouse click still shows nothing. Nothing is stroked here.
                SelectObject(hdc, of);
                // fix thse isseue.md #5 B: flush the segment backbuffer to the
                // control in ONE BitBlt (no partial frames / card-bg flash), then
                // tear it down. Create/Cancel/Copy painted straight to realDC.
                if (segMemDC) {
                    BitBlt(realDC, rc.left, rc.top, segBBW, segBBH, segMemDC, 0, 0, SRCCOPY);
                    SelectObject(segMemDC, segOldBmp);
                    DeleteObject(segBmp);
                    DeleteDC(segMemDC);
                }
                return TRUE;
            }
            break;
        }
        case WM_COMMAND: {
            WORD id = LOWORD(wParam);
            // Inline validation message lives ON the name label only
            // transiently: as soon as the user edits the name, restore the
            // section label text so a stale "..." message can never stick
            // (UI spec item 6 / #9). The edit control is id 101.
            if (id == 101 && HIWORD(wParam) == EN_CHANGE && d && d->nameLabel &&
                !d->origNameLabel.empty()) {
                wchar_t cur[256] = {0};
                GetWindowTextW(d->nameLabel, cur, ARRAYSIZE(cur));
                if (d->origNameLabel != cur) {
                    SetWindowTextW(d->nameLabel, d->origNameLabel.c_str());
                    InvalidateRect(d->nameLabel, nullptr, TRUE);
                }
                return 0;
            }
            // fix thse isseue.md #7: repaint the Folder name field's focus-accent
            // border when it gains/loses keyboard focus (Group only).
            if (id == 101 && d && d->isGroup &&
                (HIWORD(wParam) == EN_SETFOCUS || HIWORD(wParam) == EN_KILLFOCUS)) {
                // fix thse isseue.md #3: the Folder name field FLASHED on every
                // click/focus because this repainted the WHOLE popup with
                // erase=TRUE -- a full WM_ERASEBKGND panel clear behind the
                // field. Repaint ONLY the field's border ring, and WITHOUT
                // erasing (FALSE), so the accent/neutral border flips with no
                // background flash.
                if (d->edit) {
                    RECT er;
                    GetWindowRect(d->edit, &er);
                    MapWindowPoints(nullptr, hwnd, reinterpret_cast<POINT*>(&er), 2);
                    const int pad = ScaleDpi(3, d->dpi);
                    InflateRect(&er, pad, pad);
                    InvalidateRect(hwnd, &er, FALSE);
                }
                return 0;
            }
            // fix thse isseue.md Phase 3: the title-bar square button (id 103)
            // is now a REAL Maximize/Restore, driven by the ACTUAL window state
            // (IsZoomed), NOT the compact/expanded content preference. Ctrl+E
            // still owns the density preference independently (see the modal
            // loop). Maximizing fills the monitor WORK AREA via WM_GETMINMAXINFO;
            // SW_RESTORE returns to the user's prior bounds (Windows tracks them).
            // manualSized latches so the layout stops forcing the preset size.
            if (id == 103 && d && d->isGroup) {
                d->manualSized = true;
                // The maximize/restore glyph repaints through the normal
                // WM_SIZE relayout path (RedrawWindow reaches every child), so
                // the previous dedicated pre-relayout caption-button repaint is
                // redundant and has been removed to kill the extra flash.
                ShowWindow(hwnd, IsZoomed(hwnd) ? SW_RESTORE : SW_MAXIMIZE);
                SetFocus(d->edit);
                return 0;
            }
            // fix thse isseue.md #1/#2: title-bar Minimize (104) sends the popup
            // to the taskbar; Close (105) dismisses exactly like Cancel/Esc
            // (routed through the IDCANCEL path so cleanup stays identical).
            if (id == 104 && d && d->isGroup) {
                ShowWindow(hwnd, SW_MINIMIZE);
                return 0;
            }
            if (id == 105 && d && d->isGroup) {
                PostMessageW(hwnd, WM_COMMAND, MAKEWPARAM(IDCANCEL, BN_CLICKED), 0);
                return 0;
            }
            // fix thse isseue.md #2 (path pass): the "Copy" button (id 111)
            // copies the resolved destination path (exactly what the read-only
            // field shows) to the clipboard. Pure clipboard write, no state change.
            if (id == 111 && d && d->isGroup) {
                CopyPathToClipboard(hwnd,
                    GroupDestinationFieldText(d->destPath, d->effectiveDest, d->lastTarget));
                return 0;
            }
            // fix thse isseue.md Issue 1: the '?' navigation-help button
            // (id 121) OPENS the transient help card through the TESTED infohelp
            // model (OnClick always -> Shown; it is NOT a pin/unpin toggle). The
            // card is a separate non-activating popup, so it never steals focus
            // from the folder-name edit; it hides again when the pointer leaves
            // both the button and the card (grace timer) or on Esc/popup-close.
            if (id == 121 && d && d->isGroup) {
                d->helpVis = static_cast<int>(
                    infohelp::OnClick(static_cast<infohelp::Vis>(d->helpVis)));
                ShowHelpCard(hwnd, d);
                return 0;
            }
            // Copy | Move segments (Phase 6/D, Group only): clicking id 107
            // selects Copy, id 108 selects Move. segmented::OnClickSegment (unit
            // tested) maps the clicked segment -> Seg -> d->isMove, the SAME
            // state the arrow keys produce, so mouse and keyboard agree. Repaint
            // BOTH segments so the active-highlight follows the new choice.
            if ((id == 107 || id == 108) && d && d->isGroup) {
                segmented::Seg clicked =
                    (id == 107) ? segmented::Seg::Copy : segmented::Seg::Move;
                d->isMove = segmented::ToIsMove(segmented::OnClickSegment(clicked));
                SetTimer(hwnd, 110, segglide::kStepMs, nullptr);  // #3: start the fill glide
                if (d->segCopyBtn) InvalidateRect(d->segCopyBtn, nullptr, FALSE);
                if (d->segMoveBtn) InvalidateRect(d->segMoveBtn, nullptr, FALSE);
                SetFocus(id == 107 ? d->segCopyBtn : d->segMoveBtn);
                return 0;
            }
            // ADD-ON (Phase D): the timed Auto Group button (id 120). It only
            // ACTS when the eight-second countdown has elapsed (agcd::Clickable).
            // On a valid click it previews the plan (destination + counts) from
            // the pure autogroup:: model, shows a concise confirmation, and --
            // on OK -- closes the popup marking the result as an Auto Group
            // request. It performs NO filesystem work here; the worker does that
            // in a later phase (and in THIS phase only logs the request). Normal
            // Group/Create/Cancel are unaffected and stay usable throughout.
            if (id == 120 && d && d->isGroup) {
                const bool en = AtomicGet(&g_autoGroupEnabled) != 0;
                const agcd::State st =
                    agcd::Resolve(en, d->autoGroupStartTick, GetTickCount64());
                if (!agcd::Clickable(st)) {
                    return 0;  // still counting down -> ignore the click entirely
                }
                // Resolve the Auto Group destination root for the PREVIEW. This
                // mirrors the worker's dedicated resolver precedence (fixed
                // destination, else the captured common parent) and NEVER reuses
                // the typed name or g_lastGroupTarget.
                std::wstring fixedDest;
                {
                    EnterCriticalSection(&g_templatesCs);
                    fixedDest = g_groupDestination;
                    LeaveCriticalSection(&g_templatesCs);
                }
                std::wstring root = !fixedDest.empty() ? fixedDest : d->effectiveDest;
                if (root.empty()) {
                    MessageBoxW(hwnd,
                        L"Auto Group needs a common parent folder for the "
                        L"selection, or a configured Fixed destination folder. "
                        L"The current selection does not share one, so nothing "
                        L"was changed.",
                        L"Auto Group", MB_OK | MB_ICONINFORMATION);
                    return 0;
                }
                // Count eligible files and folders straight from the immutable
                // selection model (no FS access). The full path-based plan is
                // built in the worker (later phase); this is only the preview.
                const bool agImages = AtomicGet(&g_autoGroupLayout) != 0;
                const bool agGroupFolders = AtomicGet(&g_autoGroupSelectedFolders) != 0;
                std::wstring agImagesName, agAllName;
                {
                    EnterCriticalSection(&g_templatesCs);
                    agImagesName = g_autoGroupImageParentName;
                    agAllName = g_autoGroupFoldersName;
                    LeaveCriticalSection(&g_templatesCs);
                }
                const bool agMove = d->isMove;
                size_t fileCount = 0, folderCount = 0;
                for (const auto& it : d->items) {
                    if (it.isFolder) ++folderCount; else ++fileCount;
                }
                const size_t foldersToMove = agGroupFolders ? folderCount : 0;
                std::wstring msg =
                    L"Auto Group will organize the selected files into "
                    L"extension folders.\n\n";
                msg += L"Destination:  " + root + L"\n";
                msg += L"Files to organize:  " + std::to_wstring(fileCount) + L"\n";
                msg += L"Operation:  " + std::wstring(agMove ? L"Move" : L"Copy") + L"\n";
                if (agGroupFolders) {
                    msg += L"Selected folders moved into \"" + agAllName +
                           L"\":  " + std::to_wstring(foldersToMove) + L"\n";
                } else {
                    msg += L"Selected folders:  left untouched\n";
                }
                if (agImages) {
                    msg += L"Image types grouped under:  \"" + agImagesName + L"\"\n";
                }
                msg += L"\nProceed?";
                const int res = MessageBoxW(hwnd, msg.c_str(), L"Auto Group",
                                            MB_OKCANCEL | MB_ICONQUESTION);
                if (res != IDOK) {
                    return 0;  // user cancelled the confirmation; popup stays open
                }
                d->accepted = true;
                d->autoGroupRequested = true;
                Wh_Log(L"AutoGroup: user confirmed; request produced (files=%zu, "
                       L"foldersToMove=%zu, move=%d) -- worker performs the operation",
                       fileCount, foldersToMove, agMove ? 1 : 0);
                DestroyWindow(hwnd);
                return 0;
            }
            if (id == IDOK && d) {
                // Shared button state machine (UI spec item 10/#12):
                // ignore a duplicate Create while one is already in flight.
                // Mouse Create and Enter share this one path (btn::AcceptCreate
                // is unit-tested in button_state_logic_test.cpp).
                if (!btn::AcceptCreate(d->phase)) {
                    return 0;
                }
                wchar_t buf[512] = {0};
                GetWindowTextW(d->edit, buf, ARRAYSIZE(buf));
                std::wstring typed = buf;
                // Contextual, pre-Create validation (UI spec item 6/#14).
                // Mirrors the resolver's precedence so a blank name stays valid
                // where a fixed destination (d->destPath) or a remembered last
                // target will still produce a folder. Unit-tested in
                // name_validation_logic_test.cpp. On a real problem we keep the
                // popup open and surface a concise message instead of failing
                // silently later.
                namevalid::NameCheck vc =
                    d->isGroup
                        ? namevalid::ValidateGroupName(typed, !d->destPath.empty(),
                                                       !d->lastTarget.empty())
                        : namevalid::ValidateNewFolderName(typed);
                if (vc != namevalid::NameCheck::Ok) {
                    if (d->nameLabel) {
                        SetWindowTextW(d->nameLabel,
                                       namevalid::ValidationMessage(vc).c_str());
                        InvalidateRect(d->nameLabel, nullptr, TRUE);
                    }
                    Wh_Log(L"PromptValidation: name rejected (check=%d); popup stays open",
                           static_cast<int>(vc));
                    SetFocus(d->edit);
                    return 0;  // do NOT accept; let the user correct the name
                }
                d->phase = btn::OnCreate(d->phase);  // Idle -> Processing; locks out duplicate Create
                d->base = typed;
                d->accepted = true;
                // fix thse isseue.md #1: close the prompt normally on Create.
                // The copy/move operation runs on the worker AFTER the popup
                // closes and its real result is logged there -- there is no
                // status band, no keep-open live round-trip, and no fabricated
                // progress. The one-submit guard above (btn::OnCreate) still
                // locks out a duplicate Create.
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
        case WM_GETMINMAXINFO: {
            // fix thse isseue.md P0 (window geometry): the Group popup is a
            // borderless WS_POPUP, so a DEFAULT maximize covers the WHOLE
            // monitor -- OVER the taskbar -- and a drag-resize could crush the
            // controls. Clamp the maximized rect to the monitor WORK AREA and
            // set the minimum track size from the tested winresize::ComputeMinMax
            // model so the popup never maximizes to exclusive full screen and
            // never shrinks into an unusable sliver. New Folder (native caption)
            // keeps the OS default handling (falls through to DefWindowProcW).
            if (d && d->isGroup) {
                MINMAXINFO* mmi = reinterpret_cast<MINMAXINFO*>(lParam);
                MONITORINFO mi;
                mi.cbSize = sizeof(mi);
                HMONITOR mon = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
                if (mmi && mon && GetMonitorInfo(mon, &mi)) {
                    const winresize::Rect monR{
                        static_cast<int>(mi.rcMonitor.left),
                        static_cast<int>(mi.rcMonitor.top),
                        static_cast<int>(mi.rcMonitor.right),
                        static_cast<int>(mi.rcMonitor.bottom)};
                    const winresize::Rect workR{
                        static_cast<int>(mi.rcWork.left),
                        static_cast<int>(mi.rcWork.top),
                        static_cast<int>(mi.rcWork.right),
                        static_cast<int>(mi.rcWork.bottom)};
                    // Minimum CLIENT (96-dpi design px) -> minimum OUTER device
                    // px via the SAME outer-size model + window styles the create
                    // site used, so the floor matches real geometry at this DPI.
                    const SIZE minOuter = PromptOuterSize(
                        winresize::MinClientW(), winresize::MinClientH(),
                        static_cast<DWORD>(GetWindowLongPtrW(hwnd, GWL_STYLE)),
                        static_cast<DWORD>(GetWindowLongPtrW(hwnd, GWL_EXSTYLE)),
                        d->dpi);
                    const winresize::MinMax mm = winresize::ComputeMinMax(
                        monR, workR, static_cast<int>(minOuter.cx),
                        static_cast<int>(minOuter.cy));
                    mmi->ptMaxPosition.x  = mm.maxPos.cx;
                    mmi->ptMaxPosition.y  = mm.maxPos.cy;
                    mmi->ptMaxSize.x      = mm.maxSize.cx;
                    mmi->ptMaxSize.y      = mm.maxSize.cy;
                    mmi->ptMinTrackSize.x = mm.minTrack.cx;
                    mmi->ptMinTrackSize.y = mm.minTrack.cy;
                    return 0;
                }
            }
            break;
        }
        case WM_SIZE: {
            // fix thse isseue.md UI-01 / UI-05: the Group popup has a real resize
            // frame (WS_THICKFRAME + WM_NCHITTEST) and a Maximize button (id 103).
            // Once it is maximized or has been manually resized, reflow the
            // children into the LIVE client rect so the form tracks the window
            // and the footer stays visible -- instead of being stranded at the
            // fixed compact/expanded size in the upper-left corner. The normal
            // (not-yet-resized) popup falls through to DefWindowProc so initial
            // sizing and the Ctrl+E density path are unchanged. New Folder is
            // never a Group popup, so it is never reflowed here.
            if (d && d->isGroup && wParam != SIZE_MINIMIZED &&
                (d->manualSized || IsZoomed(hwnd))) {
                RECT rc{};
                GetClientRect(hwnd, &rc);
                const int cw = rc.right - rc.left;
                const int ch = rc.bottom - rc.top;
                if (cw > 0 && ch > 0) {
                    // fix thse isseue.md Phase 2: route the resize/maximize
                    // relayout through the SINGLE authoritative, MODE-AWARE
                    // ComputeEffectiveLayout. It grid-sizes the Selected card to
                    // the current view mode's WHOLE cells and reflows into the
                    // live client rect with that cell height + whole-cell cap.
                    // The old inline ComputePromptLayout + ReflowToClient here
                    // bypassed the mode-aware calc and supplied a List-based
                    // (24px row) override that clipped grid cells on resize and
                    // maximize -- the exact Phase 2 defect.
                    layout::PromptLayout reflowed = ComputeEffectiveLayout(hwnd, d);
                    ApplyPromptLayout(hwnd, d, &reflowed);
                }
                // fix thse isseue.md Issue 1: keep the navigation-help card
                // anchored to the '?' button as the popup resizes / maximizes.
                if (d && d->helpCard && IsWindowVisible(d->helpCard))
                    PositionHelpCard(hwnd, d);
                // fix thse isseue.md Issue 6: remember this size + maximize state
                // for the next Ctrl+G open (the NORMAL rect is captured only
                // while NOT zoomed, so a maximize never overwrites it).
                RememberGroupPlacement(hwnd, d);
                return 0;
            }
            break;
        }
        case WM_ENTERSIZEMOVE: {
            // fix thse isseue.md v0.12.67 (top note): latch the client size at
            // the START of a move/resize drag so WM_EXITSIZEMOVE can tell a pure
            // MOVE (dragging the popup by its custom header) from a real RESIZE.
            if (d && d->isGroup) {
                RECT rc{};
                GetClientRect(hwnd, &rc);
                d->sizeMoveClientW = rc.right - rc.left;
                d->sizeMoveClientH = rc.bottom - rc.top;
                d->inSizeMove = true;
            }
            break;
        }
        case WM_EXITSIZEMOVE: {
            // fix thse isseue.md UI-05 + v0.12.67 (top note): the user finished
            // dragging. ONLY a genuine size change takes the reflow/relayout
            // path -- latch manualSized so (a) this and future size changes take
            // the reflow path above and (b) the Ctrl+E density toggle no longer
            // snaps the window back to the preferred size, then reflow once into
            // the final rect. A pure MOVE (drag by the custom header, no size
            // change) must NOT reflow: the old code unconditionally set
            // manualSized and sent WM_SIZE, which repositioned EVERY child and
            // repainted, so dragging the popup by its header flashed /
            // re-rendered the entire window. A move now only remembers the new
            // position (Issue 6) -- no manualSized latch, no WM_SIZE, no flash.
            if (d && d->isGroup) {
                RECT rc{};
                GetClientRect(hwnd, &rc);
                const int cw = rc.right - rc.left;
                const int ch = rc.bottom - rc.top;
                const bool resized =
                    !d->inSizeMove ||
                    cw != d->sizeMoveClientW || ch != d->sizeMoveClientH;
                d->inSizeMove = false;
                if (resized) {
                    d->manualSized = true;
                    SendMessageW(hwnd, WM_SIZE, SIZE_RESTORED, 0);
                }
                // fix thse isseue.md Issue 6: a drag-move or drag-resize just
                // finished -- remember the new NORMAL size/position either way.
                RememberGroupPlacement(hwnd, d);
            }
            break;
        }
        case WM_DPICHANGED: {
            // UI spec item 16: adopt the new monitor DPI. dpiutil::
            // (unit-tested) decodes it; the OS provides the suggested window
            // rectangle in lParam. Remember the DPI and take the rect so the
            // window + its DWM rounded corners follow the new monitor; child
            // metrics reuse the same layout model on the next relayout.
            const int newDpi = dpiutil::NewDpiFromWparam(
                static_cast<unsigned long>(wParam));
            if (d && dpiutil::NeedsRelayout(static_cast<int>(d->dpi), newDpi)) {
                d->dpi = static_cast<UINT>(newDpi);
                Wh_Log(L"PromptDpi: WM_DPICHANGED -> %d", newDpi);
            }
            RECT* sug = reinterpret_cast<RECT*>(lParam);
            if (sug) {
                SetWindowPos(hwnd, nullptr, sug->left, sug->top,
                             sug->right - sug->left, sug->bottom - sug->top,
                             SWP_NOZORDER | SWP_NOACTIVATE);
            }
            // #5: adopt the suggested outer rect above, THEN relayout every
            // child through the one layout model at the new DPI -- moving each
            // control, re-selecting the rescaled font, and recreating the
            // rounded regions. Without this the window resized but the children
            // kept old-DPI geometry/font/clip (the artifact we are fixing).
            if (d) {
                ApplyPromptLayout(hwnd, d);
                RefreshGroupPanel(d);
                // fix thse isseue.md add-on section 5: the Navigation Help card
                // is a SEPARATE top-level window, so ApplyPromptLayout above does
                // not touch it. If it is open, re-measure + re-anchor it at the
                // NEW DPI (MeasureHelpCard/PositionHelpCard both read d->dpi), so
                // its fonts, padding, scrollbar geometry and position all follow
                // the monitor change instead of keeping old-DPI metrics.
                if (d->helpVis) PositionHelpCard(hwnd, d);
                // fix thse isseue.md P0 #3: the title band is DPI-scaled, so the
                // DWM custom-frame extension must be re-applied at the NEW DPI or
                // the transparent extended region drifts out of sync with the
                // repainted band (a white strip creeps back). Only when the
                // backdrop is CONFIRMED compositing -- the SolidTheme fallback
                // never extends its frame, so it is left completely untouched.
                if (d->acrylicActive) {
                    ReextendTitleBandFrame(hwnd, d->dpi);
                }
            }
            return 0;
        }
        case WM_ACTIVATE: {
            // fix thse isseue.md P0 #3: re-assert the DWM custom-frame extension
            // on activation (the documented "Custom Window Frame Using DWM"
            // pattern) so the Group title band keeps its transparent extended
            // region after focus changes. No-op unless the backdrop is CONFIRMED;
            // falls through to DefWindowProcW so the normal activation/focus
            // behaviour of the modal popup is unchanged.
            if (d && d->acrylicActive) {
                ReextendTitleBandFrame(hwnd, d->dpi);
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
// A Ctrl+G group popup seeds these IN fields (isGroup + the summary rows + the
// initial Move/Copy taken from the setting) and reads the user's FINAL Move/Copy
// choice back OUT of isMove. New Folder callers pass nullptr, so isGroup stays
// false and the popup is byte-for-byte the original New Folder prompt.
struct GroupPromptSeed {
    bool isGroup = false;
    std::vector<sel::Item> items;   // full selection for the Selected Items panel
    std::wstring destPath;
    std::wstring lastTarget;
    // fix thse isseue.md #2: UI-only preview context -- the selection's captured
    // PARENT. Shown in the read-only Destination field when no fixed destination
    // is configured. Does NOT change ResolveGroupDestination precedence.
    std::wstring effectiveDest;
    bool isMove = false;  // in: default from setting; out: user's final choice
    // ADD-ON: OUT flag. The popup sets this true ONLY when the user confirmed
    // the timed Auto Group button (never on normal Group/Create). The worker
    // branches on it BEFORE any typed-name / g_lastGroupTarget resolution so an
    // Auto Group request can never reuse the last target.
    bool autoGroup = false;
    // fix thse isseue.md #1: the live worker->popup status round-trip was
    // removed. The popup always closes on Create/Cancel, so there is no live
    // HWND / session hand-off any more.
};

static bool PromptForFolderName(HWND owner, std::wstring& outBase,
                                GroupPromptSeed* group = nullptr) {
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

    // #1 PromptSession lifetime: NamePromptData is HEAP-owned, not a stack
    // local. The window stores this pointer in GWLP_USERDATA, and the review's
    // live worker->popup round-trip keeps the popup alive AFTER this function
    // returns -- a stack `NamePromptData data;` would be a use-after-return the
    // instant a late window message arrived during teardown. The unique_ptr owns the session for
    // the whole popup lifetime and frees it on return; `data` is a reference
    // alias so the rest of this function (and &data to CreateWindowExW) is
    // byte-for-byte unchanged. A unit test can't see Win32 object lifetime, so
    // scripts/regression_check.ps1 pins this (make_unique present; stack local
    // gone).
    std::unique_ptr<NamePromptData> session = std::make_unique<NamePromptData>();
    NamePromptData& data = *session;
    data.glass = IsWin11();
    data.dpi = DpiForWindow(owner);  // Bug 3: scale the popup for this monitor

    // Ctrl+G group mode: light up the popup's group header (count, item list,
    // destination + last-target rows, Move/Copy toggle) from the caller's seed.
    // New Folder callers pass group == nullptr, so isGroup stays false and the
    // prompt renders exactly as before.
    if (group && group->isGroup) {
        data.isGroup = true;
        data.items = group->items;
        data.destPath = group->destPath;
        data.lastTarget = group->lastTarget;
        data.effectiveDest = group->effectiveDest;  // #2: captured parent preview
        data.isMove = group->isMove;
        data.glideProgress = segglide::RestProgress(data.isMove);  // #3: seed glide on the correct side
        // Compact/expanded (UI spec item 11/#15): reuse the size the user
        // chose earlier this session; if they haven't chosen, auto-expand a
        // large selection so more of it is visible up front. mode::InitialMode
        // is unit-tested; the same flag sizes the window and picks the row count.
        const mode::Mode initial = mode::InitialMode(
            g_sessionHasMode,
            g_sessionExpanded ? mode::Mode::Expanded : mode::Mode::Compact,
            data.items.size());
        data.expanded = mode::IsExpanded(initial);
    }

    layout::PromptLayout PL = layout::ComputePromptLayout(data.isGroup, data.expanded,
                                                          static_cast<int>(data.items.size()));
    // PL.windowW/windowH are CLIENT (content) sizes. Convert to the OUTER window
    // size for this monitor so the WS_CAPTION title bar does not consume the
    // client area and clip the bottom action row (fix thse isseu e.md #1). The
    // style/exStyle here MUST match the CreateWindowExW call below. New Folder is
    // unchanged apart from now being correctly sized for its own title bar.
    // fix thse isseue.md #2: the Group popup is a borderless WS_POPUP so it can
    // paint its OWN title bar (title + Expand/Minimize/Close); New Folder keeps
    // the native caption. The style MUST match the CreateWindowExW call below.
    // fix thse isseue.md P0 (manual resize): the Group popup adds WS_THICKFRAME
    // (+ WS_MAXIMIZEBOX) so the OS gives it a real sizing frame -- the user can
    // drag any edge/corner to resize and the WM_SIZE handler reflows the content
    // (layout::ReflowToClient) into the live client rect. Without this bit
    // DefWindowProc ignores the frame hit-tests and the popup is stuck at its
    // fixed size. The frame is accounted for by PromptOuterSize (which uses THIS
    // same style), so the client stays the intended size (no clipping), and
    // maximize is clamped to the work area by the WM_GETMINMAXINFO handler.
    const DWORD winStyle = data.isGroup
        ? (WS_POPUP | WS_THICKFRAME | WS_MAXIMIZEBOX)
        : (WS_POPUP | WS_CAPTION | WS_SYSMENU);
    // fix thse isseue.md P0 #2: ONE shared extended style for BOTH the
    // outer-size math AND CreateWindowExW, via the unit-tested outeroutline::
    // model. The borderless Group popup drops WS_EX_DLGMODALFRAME (the raised
    // 3-D frame that painted the stray bright outer perimeter); New Folder keeps
    // it. exStyle MUST be identical at both sites.
    const DWORD exStyle =
        static_cast<DWORD>(outeroutline::PromptExStyle(data.isGroup));
    const SIZE outer = PromptOuterSize(
        PL.windowW, PL.windowH,
        winStyle,
        exStyle, data.dpi);
    const int w = outer.cx;
    const int h = outer.cy;
    int x = CW_USEDEFAULT, y = CW_USEDEFAULT;
    RECT rc;
    if (owner && GetWindowRect(owner, &rc)) {
        x = rc.left + ((rc.right - rc.left) - w) / 2;
        y = rc.top + ((rc.bottom - rc.top) - h) / 2;
    }

    // Bug 4: create the popup UNOWNED (owner = nullptr). It is already
    // WS_EX_TOPMOST and centred over the Explorer window above, and the modal
    // loop below provides modality -- so we never make another process's window
    // our owner (which would cross-attach input queues) nor disable it (which
    // would freeze that window forever if this tool process died while open).
    // fix thse isseue.md ("text feels pixelated"): create the popup PER-MONITOR
    // DPI aware (v2) so Windows renders it NATIVELY at the monitor DPI instead
    // of bitmap-stretching a 96-dpi surface (the soft/pixelated-text cause).
    // WM_CREATE builds every child control synchronously inside this
    // CreateWindowExW, so they inherit the same awareness. Resolved dynamically
    // (Win10 1703+); the previous context is restored right after.
    using SetThreadDpiCtxFn = DPI_AWARENESS_CONTEXT (WINAPI*)(DPI_AWARENESS_CONTEXT);
    HMODULE u32dpi = GetModuleHandleW(L"user32.dll");
    auto setDpiCtx = u32dpi ? reinterpret_cast<SetThreadDpiCtxFn>(
        GetProcAddress(u32dpi, "SetThreadDpiAwarenessContext")) : nullptr;
    DPI_AWARENESS_CONTEXT prevDpiCtx = nullptr;
    if (setDpiCtx) prevDpiCtx = setDpiCtx(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    HWND hwnd = CreateWindowExW(
        exStyle, kClass,
        (data.isGroup ? L"\U0001F4C2 Group into Folder" : L"New folder"),
        winStyle,
        x, y, w, h, nullptr, nullptr, HINST_THISCOMPONENT, session.get());
    if (setDpiCtx && prevDpiCtx) setDpiCtx(prevDpiCtx);
    if (!hwnd) {
        return false;
    }

    // fix thse isseue.md Issue 6: restore the remembered NORMAL size/position
    // onto the nearest monitor's work area before the popup is shown (clamped so
    // it is always visible). A maximized state is re-applied just after
    // ShowWindow below. With NO saved preference the default compact/expanded
    // centering computed above is kept. New Folder is never remembered.
    ApplyRememberedNormalPlacement(hwnd, &data);

    // fix thse isseue.md #1/#2: capture the EXPLICIT, HRESULT-checked backdrop
    // result (BOTH the DwmSetWindowAttribute AND the DwmExtendFrameIntoClientArea
    // HRESULTs) instead of a single ambiguous bool that assumed success.
    const bool isWin11 = IsWin11();
    const bool transparencyOn = IsTransparencyEnabled();
    const bool isDark = IsDarkMode();
    BackdropResult bd =
        ApplyWindowPolish(hwnd, IsDarkMode(), data.glass, data.isGroup);
    // fix thse isseue.md #1/#2/#3: ONE honest decision via paintmode:: (unit-
    // tested). The New Folder popup ALWAYS paints its full themed client area
    // (SolidTheme); only the Group popup may drop its title-band fill for the
    // backdrop (DwmTitleBand), and ONLY when Win11 + backdrop requested + the
    // user's Transparency effects are ON AND BOTH DWM calls actually succeeded.
    // The OS version + the Transparency setting + a successful attribute call
    // are NOT, on their own, treated as proof. acrylicActive therefore means
    // "the Group title band is CONFIRMED compositing"; it is false for New Folder
    // and for every failure, so WM_ERASEBKGND keeps painting the solid themed
    // panel -- the safe #202020 (dark) / #F3F3F3 (light) surface, never a bare
    // white body or white title strip, on Win10 / transparency off / remote /
    // when DWM rejected the backdrop or the frame extension failed.
    const paintmode::PaintMode paintMode = paintmode::ChoosePaintMode(
        data.isGroup, isWin11, transparencyOn, data.glass,
        bd.backdropAttributeAccepted, bd.frameExtensionSucceeded);
    data.acrylicActive = paintmode::UsesTitleBandBackdrop(paintMode);
    // fix thse isseue.md #4: one concise diagnostic per popup creation, so the
    // chosen rendering mode is explainable from the LOG ALONE -- isGroup,
    // isWin11, isDark, transparencyEnabled, the two DWM HRESULTs, acrylicActive,
    // and the explicit backgroundPaintMode (SolidTheme | DwmTitleBand).
    Wh_Log(L"NamePrompt backdrop: isGroup=%d isWin11=%d isDark=%d "
           L"transparencyEnabled=%d backdropHRESULT=0x%08X "
           L"frameExtensionHRESULT=0x%08X borderColorHRESULT=0x%08X "
           L"borderColorAccepted=%d acrylicActive=%d backgroundPaintMode=%ls",
           data.isGroup ? 1 : 0, isWin11 ? 1 : 0, isDark ? 1 : 0,
           transparencyOn ? 1 : 0,
           static_cast<unsigned>(bd.backdropHr),
           static_cast<unsigned>(bd.frameExtensionHr),
           static_cast<unsigned>(bd.borderColorHr),
           bd.borderColorAccepted ? 1 : 0,
           data.acrylicActive ? 1 : 0,
           paintMode == paintmode::PaintMode::DwmTitleBand ? L"DwmTitleBand"
                                                           : L"SolidTheme");

    // fix thse isseue.md P0 (initial-layout lifecycle): run ONE authoritative
    // layout pass BEFORE the first visible frame, via the SAME
    // ComputeEffectiveLayout + ApplyPromptLayout path a later resize uses. The
    // children were created (WM_CREATE) at the PREFERRED ComputePromptLayout
    // coordinates, which -- for a GRID Selected view -- omit the grid-cell
    // sizing + section shift the effective layout applies, so the old first
    // frame was misaligned until the user resized. Doing it here, while the
    // window is still hidden (winStyle has no WS_VISIBLE; ShowWindow is below),
    // fixes the geometry with NO flash and NO reliance on a manual resize. The
    // window is already sized (CreateWindowExW + ApplyRememberedNormalPlacement)
    // so GetClientRect is valid. This is a guarded ONE-TIME pass in the creation
    // sequence (never a message loop); ApplyPromptLayout skips the top-level
    // resize once manualSized/zoomed, so a remembered/maximized state is not
    // disturbed and no resize/relayout loop can start.
    if (data.isGroup) {
        RECT rcInit{};
        GetClientRect(hwnd, &rcInit);
        if ((rcInit.right - rcInit.left) > 0 && (rcInit.bottom - rcInit.top) > 0) {
            ApplyPromptLayout(hwnd, &data);
        }
    }

    ShowWindow(hwnd, SW_SHOW);
    // fix thse isseue.md Issue 6: if the last session state was maximized, open
    // maximized. The remembered NORMAL rect applied above is what SW_RESTORE
    // returns to, so maximize never destroys the restore size.
    if (data.isGroup && GroupPlacementMaximized()) {
        data.manualSized = true;
        ShowWindow(hwnd, SW_MAXIMIZE);
    }
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
        // fix thse isseue.md P0 #1: track focus-visible at the dialog level so
        // the owner-drawn ring shows ONLY for keyboard focus. The modal loop
        // sees every message (incl. those to child buttons) before dispatch: a
        // navigation key makes focus visible; a pointer press hides it.
        // focusvis:: (unit-tested) owns the classification; d->focusVisible is
        // the single flag the owner-draw reads back. On a transition we repaint
        // the owner-draw controls so the ring appears/disappears immediately.
        {
            bool fvNew = data.focusVisible;
            if (msg.message == WM_KEYDOWN &&
                focusvis::IsNavKey(static_cast<int>(msg.wParam))) {
                fvNew = focusvis::FromInput(focusvis::Input::KeyboardNav);
            } else if (msg.message == WM_LBUTTONDOWN ||
                       msg.message == WM_RBUTTONDOWN ||
                       msg.message == WM_MBUTTONDOWN ||
                       msg.message == WM_LBUTTONDBLCLK ||
                       msg.message == WM_NCLBUTTONDOWN) {
                fvNew = focusvis::FromInput(focusvis::Input::PointerPress);
            }
            if (fvNew != data.focusVisible) {
                data.focusVisible = fvNew;
                if (data.createBtn)  InvalidateRect(data.createBtn,  nullptr, TRUE);
                if (data.cancelBtn)  InvalidateRect(data.cancelBtn,  nullptr, TRUE);
                if (data.segCopyBtn) InvalidateRect(data.segCopyBtn, nullptr, FALSE);
                if (data.segMoveBtn) InvalidateRect(data.segMoveBtn, nullptr, FALSE);
            }
        }
        // fix thse isseue.md Issue 2: a background image thumbnail finished ->
        // repaint the Selected Items list WITHOUT erasing (no flash); the OS
        // coalesces bursts into one paint. Never changes layout or selection.
        if (msg.hwnd == hwnd && msg.message == WM_APP_THUMB_READY) {
            if (data.groupInfo) InvalidateRect(data.groupInfo, nullptr, FALSE);
            continue;
        }
        // fix thse isseue.md Issue 1: Escape dismisses the transient navigation
        // help card FIRST (the help surface owns the interaction while shown);
        // the popup itself is only cancelled by a later Esc once the card is
        // gone. infohelp::OnDismiss is the tested transition.
        if (msg.message == WM_KEYDOWN && msg.wParam == VK_ESCAPE && data.isGroup &&
            data.helpVis != static_cast<int>(infohelp::Vis::Hidden)) {
            data.helpVis = static_cast<int>(
                infohelp::OnDismiss(static_cast<infohelp::Vis>(data.helpVis)));
            HideHelpCard(hwnd, &data);
            continue;
        }
        // fix thse isseue.md Issue 1: transient navigation-help card hover.
        // Poll the pointer against the '?' button and the card every pump
        // (kHelpPumpTimer also wakes the loop while shown so the grace-close
        // still fires when the pointer stops moving). Hover opens the card; the
        // card stays while the pointer is on EITHER surface; leaving BOTH arms a
        // short grace-close (infohelp::CloseDelayMs). The decisions are the
        // tested infohelp:: predicates -- the pump only maps them to the window.
        if (data.isGroup && data.infoBtn) {
            POINT hp; GetCursorPos(&hp);
            RECT br; GetWindowRect(data.infoBtn, &br);
            const bool onBtn = PtInRect(&br, hp) != FALSE;
            bool onCard = false;
            if (data.helpCard && IsWindowVisible(data.helpCard)) {
                RECT cr; GetWindowRect(data.helpCard, &cr);
                onCard = PtInRect(&cr, hp) != FALSE;
            }
            data.helpPtrOnBtn  = onBtn;
            data.helpPtrOnCard = onCard;
            const infohelp::Vis cur = static_cast<infohelp::Vis>(data.helpVis);
            if (onBtn && cur == infohelp::Vis::Hidden) {
                data.helpVis = static_cast<int>(infohelp::OnHoverEnter(cur));
                ShowHelpCard(hwnd, &data);
            } else if (cur == infohelp::Vis::Shown) {
                if (infohelp::StaysOpen(cur, onBtn, onCard)) {
                    data.helpCloseAtTick = 0;   // on a surface -> cancel pending close
                } else if (infohelp::ShouldArmClose(cur, onBtn, onCard)) {
                    const unsigned long long now = GetTickCount64();
                    if (data.helpCloseAtTick == 0) {
                        data.helpCloseAtTick =
                            now + static_cast<unsigned long long>(infohelp::CloseDelayMs());
                    } else if (now >= data.helpCloseAtTick) {
                        if (infohelp::OnCloseTimer(cur, onBtn, onCard) ==
                            infohelp::Vis::Hidden) {
                            HideHelpCard(hwnd, &data);
                        }
                        data.helpCloseAtTick = 0;
                    }
                }
            }
        }
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
        // Compact/Expanded size toggle (UI spec item 11/#15, Phase 7):
        // Ctrl+E OR the titlebar toggle button (id 103, handled in WM_COMMAND)
        // flip the mode live through the ONE centralized layout path
        // (ApplyPromptLayout) and re-render the Selected Items panel with the
        // new row count. The chosen mode is remembered for the session so the
        // next popup opens the same way. Group only; focus is preserved.
        if (msg.message == WM_KEYDOWN && msg.wParam == 'E' &&
            (GetKeyState(VK_CONTROL) & 0x8000) && data.isGroup) {
            data.expanded = !data.expanded;
            g_sessionHasMode = true;
            g_sessionExpanded = data.expanded;
            // fix thse isseue.md ("Also remove this unnecessary repaint"): the
            // size toggle is NOT invalidated here, and the list is NOT refreshed
            // separately -- ApplyPromptLayout's single atomic RDW_ALLCHILDREN
            // repaint redraws the toggle glyph AND the Selected Items list once.
            ApplyPromptLayout(hwnd, &data);
            continue;
        }
        // Selected Items scrolling (UI spec item 1/#3/#4/#12): the panel
        // is a fixed-height viewport. The wheel scrolls it ONLY when the cursor
        // is over the list region, and the nav keys scroll it ONLY when no
        // interactive control (edit / a button / the Copy-Move toggle) owns
        // focus -- so the wheel no longer hijacks a hover over the buttons and
        // the arrow keys no longer steal dialog navigation. listscroll::
        // (unit-tested) owns both the row math AND the routing predicates;
        // d->listTopRow is the single source of truth and the selection is
        // never mutated.
        if (data.isGroup && data.groupInfo) {
            // fix thse isseue.md Phase 1A: page by the TRUE visible-row capacity
            // the owner-draw paint cached (the declared per-mode cap reconciled
            // with what the viewport physically fits), so wheel/key scrolling
            // reaches a row that would otherwise be clipped. Fall back to the
            // declared cap before the first paint has populated the cache.
            const int visRows = data.listVisibleRows > 0
                ? data.listVisibleRows
                : mode::VisibleRows(
                      data.expanded ? mode::Mode::Expanded : mode::Mode::Compact);
            const int itemCount = static_cast<int>(data.items.size());
            // fix thse isseue.md Phase 2: in a grid view (Large / Extra large)
            // one scroll "row" is a GRID ROW that is data.selGridCols cells
            // wide, so wheel/keys must page by grid rows -- not by single items.
            // Use the column count the paint cached; List mode keeps one item
            // per row (scrollCols == 1 -> scrollRows == itemCount).
            const int scrollCols = (data.selGridCols > 0) ? data.selGridCols : 1;
            const int scrollRows = viewmode::IsGrid(data.selViewMode)
                ? viewmode::GridRows(itemCount, scrollCols)
                : itemCount;
            // fix thse isseue.md #2/#3: summary-chip interactions live in the
            // same modal pump as the list. The owner-draw painter records the
            // chip strip + per-chip hit rects in list (id 110) client coords, so
            // here we hit-test the cursor against them. NONE of this touches the
            // real Explorer selection or the Copy/Move/Group plan.
            POINT cpt = {0, 0};
            GetCursorPos(&cpt);
            POINT clientPt = cpt;
            ScreenToClient(data.groupInfo, &clientPt);
            const bool overChipStrip =
                PtInRect(&data.chipStripRect, clientPt) != FALSE;
            const int chipPxPerNotch = ScaleDpi(40, data.dpi);
            // --- mouse click: Show-extensions toggle, then chip filter ---
            if (msg.message == WM_LBUTTONDOWN) {
                if (PtInRect(&data.chipToggleRect, clientPt)) {
                    // fix thse isseue.md #3: flip the per-row extension badges.
                    // Does NOT change the filter selection, source selection or
                    // Group/Copy/Move behavior; a repaint re-renders the rows via
                    // the tested showext:: model.
                    data.showExt = !data.showExt;
                    RefreshGroupPanel(&data);
                    continue;
                }
                if (overChipStrip) {
                    int hitIdx = -1;
                    for (size_t ci = 0; ci < data.chipHitRects.size(); ++ci) {
                        if (PtInRect(&data.chipHitRects[ci], clientPt)) {
                            hitIdx = static_cast<int>(ci);
                            break;
                        }
                    }
                    if (hitIdx >= 0) {
                        // Capture the PRIOR filter BEFORE activating the new one
                        // so find-navigation can tell a fresh activation from a
                        // re-click of the already-active extension chip.
                        const chipfilter::State prev = ChipStateFrom(&data);
                        chipfilter::State cs = prev;
                        const int k = data.chipHitKind[hitIdx];
                        if (k == 2) chipfilter::ActivateFolders(cs);
                        else if (k == 3) chipfilter::ActivateNoExt(cs);
                        else if (k == 4) chipfilter::ActivateExt(cs, data.chipHitKey[hitIdx]);
                        else chipfilter::ActivateItems(cs);
                        ChipStateInto(&data, cs);
                        data.chipFocusIndex = hitIdx;
                        // fix thse isseue.md Phase 5 + ADD-ON: an EXTENSION chip is
                        // ALSO a find-navigator. First click reveals the first
                        // match; clicking the same ext chip again cycles to the
                        // next match (wrapping). Items/Folders/No-extension keep
                        // their existing pure-filter behaviour (no auto-scroll),
                        // so clear the cursor for them. All via the tested
                        // chipnav:: model -- never reorders items, moves the real
                        // Explorer selection or changes the Copy/Move plan.
                        if (cs.kind == chipfilter::Kind::Ext) {
                            std::vector<chipnav::Item> navItems;
                            navItems.reserve(data.items.size());
                            for (const sel::Item& nit : data.items) {
                                navItems.push_back(chipnav::Item{
                                    nit.isFolder
                                        ? std::wstring()
                                        : chipmodel::NormExtKey(
                                              sel::ExtLabel(nit.name, false)),
                                    nit.isFolder});
                            }
                            const std::vector<int> matches =
                                chipnav::Matches(cs, navItems);
                            const bool reclick =
                                (prev.kind == chipfilter::Kind::Ext &&
                                 prev.activeKey == cs.activeKey);
                            const int nextItem = chipnav::NextMatch(
                                matches, data.chipNavItem, reclick);
                            data.chipNavItem = nextItem;
                            data.listTopRow = chipnav::RevealTopRow(
                                data.selViewMode, nextItem, itemCount,
                                scrollCols, data.listTopRow, visRows);
                        } else {
                            data.chipNavItem = -1;
                        }
                        RefreshGroupPanel(&data);
                    }
                    // A click anywhere on the strip is consumed so it can never
                    // start list scrolling or activate another control.
                    continue;
                }
                // Clicking OUTSIDE the strip clears any active highlight, but is
                // NOT consumed -- the normal button/edit action still runs.
                if (ChipStateFrom(&data).active()) {
                    chipfilter::State cs = ChipStateFrom(&data);
                    chipfilter::ClickOutside(cs);
                    ChipStateInto(&data, cs);
                    data.chipFocusIndex = -1;
                    data.chipNavItem = -1;   // Phase 5: no filter -> no current match
                    RefreshGroupPanel(&data);
                }
            }
            // --- "Overflow and Scrollable Help Card" wheel isolation: when
            // the help card is open and the pointer is over it, the wheel
            // scrolls the CARD's content and is CONSUMED here, so it never
            // scrolls the Selected Items list, pans the summary chips, or zooms
            // the view. The tested helpscroll::WheelTargetsHelpCard predicate
            // owns the decision; the trailing `continue` guarantees isolation. ---
            if (msg.message == WM_MOUSEWHEEL && data.helpCard &&
                IsWindowVisible(data.helpCard)) {
                POINT hpt = {
                    static_cast<LONG>(static_cast<short>(LOWORD(msg.lParam))),
                    static_cast<LONG>(static_cast<short>(HIWORD(msg.lParam)))};
                RECT hcr = {0};
                GetWindowRect(data.helpCard, &hcr);   // WM_MOUSEWHEEL pt is screen-space
                const bool overCard = PtInRect(&hcr, hpt) != FALSE;
                if (helpscroll::WheelTargetsHelpCard(true, overCard)) {
                    const int hdelta =
                        static_cast<int>(static_cast<short>(HIWORD(msg.wParam)));
                    const int hmax = helpscroll::MaxScroll(
                        data.helpContentH, data.helpViewportH);
                    const int hpx = ScaleDpi(40, data.dpi);
                    const int ny = helpscroll::OnWheel(
                        data.helpScrollY, hdelta, hpx, hmax);
                    if (ny != data.helpScrollY) {
                        data.helpScrollY = ny;
                        // Reveal the auto-hiding scrollbar + arm its one-shot
                        // idle-hide timer for this scroll (add-on section 4).
                        data.helpSbRevealTick = GetTickCount64();
                        SetTimer(data.helpCard, kHelpSbHideTimer,
                                 static_cast<UINT>(sbauto::kIdleHideMs), nullptr);
                        // Keep the active chip + its visibility in step with the
                        // manual scroll (section-navigation tracking).
                        data.helpActiveChip = helpchips::ActiveSection(
                            std::vector<int>(data.helpSectionY,
                                             data.helpSectionY + data.helpSectionCount),
                            data.helpScrollY, data.helpViewportH);
                        std::vector<helpchips::Rect> cr;
                        for (int i = 0; i < data.helpChipCount && i < 8; ++i)
                            cr.push_back(helpchips::Rect{data.helpChipLeft[i],
                                                         data.helpChipRight[i]});
                        data.helpChipScrollX = helpchips::EnsureVisible(
                            cr, data.helpActiveChip, data.helpChipScrollX,
                            data.helpChipVisibleW, data.helpChipStripW);
                        InvalidateRect(data.helpCard, nullptr, FALSE);
                    }
                    continue;   // isolated: never reaches the list / chips / zoom
                }
            }
            // --- fix thse isseue.md Phase A: Ctrl + Mouse Wheel over the
            // Selected Items viewport cycles the presentation mode (List ->
            // Large icons -> Extra large icons, clamped at the ends). It NEVER
            // scrolls the list or pans chips and NEVER changes the real Explorer
            // selection or the Copy/Move plan. The decision is the tested
            // viewmode:: model; listTopRow stays an item index (single-column
            // row == item), so the first visible item is preserved across the
            // mode change and the paint re-clamps to the new row-height's visible
            // capacity. Ctrl beats Shift, so Ctrl+Shift+wheel zooms (never pans). ---
            if (msg.message == WM_MOUSEWHEEL &&
                viewmode::ZoomsOnWheel((GetKeyState(VK_CONTROL) & 0x8000) != 0)) {
                POINT zpt = {
                    static_cast<LONG>(static_cast<short>(LOWORD(msg.lParam))),
                    static_cast<LONG>(static_cast<short>(HIWORD(msg.lParam)))};
                RECT zlr = {0};
                GetWindowRect(data.groupInfo, &zlr);   // WM_MOUSEWHEEL pt is screen-space
                if (PtInRect(&zlr, zpt)) {
                    const int zdelta =
                        static_cast<int>(static_cast<short>(HIWORD(msg.wParam)));
                    const viewmode::Mode oldMode = data.selViewMode;
                    const viewmode::Mode newMode =
                        viewmode::OnCtrlWheel(oldMode, zdelta);
                    if (newMode != oldMode) {
                        // fix thse isseue.md Phase 4: capture the OLD effective
                        // layout BEFORE the switch so the repaint SCOPE can be
                        // decided by comparing it with the NEW one.
                        const layout::PromptLayout oldL =
                            ComputeEffectiveLayout(hwnd, &data);
                        // v0.12.68 #2: capture the first VISIBLE item under the
                        // OLD mode + columns BEFORE switching, so the view switch
                        // preserves the user's visible context. The next paint
                        // converts it to listTopRow for the NEW mode's column
                        // count (viewmode::TopRowForItem) and clamps it.
                        data.pendingFirstItem = viewmode::FirstVisibleItem(
                            oldMode, data.listTopRow, data.selGridCols);
                        data.selViewMode = newMode;
                        data.selGridCols = 1;   // the painter recomputes the real grid column count
                        const layout::PromptLayout newL =
                            ComputeEffectiveLayout(hwnd, &data);
                        // fix thse isseue.md Phase 4: eliminate the unnecessary
                        // full-window repaint. When the surrounding sections do
                        // NOT move -- a manually sized / maximized popup whose
                        // card bounds are unchanged -- repaint ONLY the Selected
                        // Items viewport (no child reposition, no full-window
                        // RDW_ALLCHILDREN erase) so the title bar, destination
                        // card, folder-name card and footer never flash. The
                        // targeted invalidate (erase FALSE) matches the thumbnail
                        // repaint path. Only a GENUINE section move (e.g. a
                        // preferred popup must resize for a taller mode) takes the
                        // single atomic ApplyPromptLayout transaction, which still
                        // preserves the first-visible item via pendingFirstItem.
                        if (layout::ViewSwitchRepaintFor(oldL, newL) ==
                                layout::ViewSwitchRepaint::SelectedOnly) {
                            if (data.groupInfo)
                                InvalidateRect(data.groupInfo, nullptr, FALSE);
                        } else {
                            ApplyPromptLayout(hwnd, &data);
                        }
                    }
                    continue;   // Ctrl+wheel is consumed even at the clamp ends
                }
            }
            // --- horizontal wheel pan over the chip strip, independent of the
            // vertical list scroll. WM_MOUSEHWHEEL and Shift+WM_MOUSEWHEEL pan
            // horizontally via the tested chipscroll:: model. Ctrl is excluded
            // here (viewmode:: zoom owns Ctrl), matching viewmode::PansChipsOnWheel. ---
            const bool wheelCtrl  = (GetKeyState(VK_CONTROL) & 0x8000) != 0;
            const bool wheelHoriz = (msg.message == WM_MOUSEHWHEEL);
            const bool wheelVert  = (msg.message == WM_MOUSEWHEEL);
            // fix thse isseue.md Issue 5: a wheel OVER THE CHIP STRIP pans the
            // chips WITHOUT requiring Shift (Shift + wheel and WM_MOUSEHWHEEL stay
            // supported -- they are also "over the strip"); Ctrl keeps zoom
            // priority (handled by the Ctrl+wheel block above). The tested
            // viewmode::WheelPansChips model owns the decision, and the trailing
            // `continue` guarantees one event is never ALSO a vertical list scroll.
            if ((wheelVert || wheelHoriz) &&
                viewmode::WheelPansChips(wheelCtrl, overChipStrip, wheelHoriz)) {
                if (chipscroll::NeedsScroll(data.chipContentW, data.chipViewportW)) {
                    const int raw =
                        static_cast<int>(static_cast<short>(HIWORD(msg.wParam)));
                    const int delta = (msg.message == WM_MOUSEHWHEEL)
                        ? chipscroll::WheelDeltaFromHorizontal(raw) : raw;
                    const int maxOff =
                        chipscroll::MaxOffset(data.chipContentW, data.chipViewportW);
                    const int off = chipscroll::OnWheel(data.chipScrollOffset,
                                                        delta, maxOff, chipPxPerNotch);
                    if (off != data.chipScrollOffset) {
                        data.chipScrollOffset = off;
                        RefreshGroupPanel(&data);
                    }
                    continue;
                }
            }
            // --- keyboard: Left/Right navigate chips (bringing an offscreen
            // chip into view), Space activates the focused chip. Only when no
            // interactive control owns focus, so the filename field keeps its
            // own Left/Right caret movement and dialog Enter/Esc are untouched. -
            {
                const HWND cfw = GetFocus();
                // fix thse isseue.md P1: ONE focus classification (focusroute)
                // for chip + list nav keys. Chips react ONLY when no interactive
                // control owns focus -- now including destCopyBtn, autoGroupBtn,
                // infoBtn, sizeToggle, minBtn and closeBtn (previously omitted).
                const bool chipKeys = focusroute::NavKeysDriveChipsOrList(
                    focusroute::IsInteractiveFocus(
                        cfw == data.edit, cfw == data.createBtn, cfw == data.cancelBtn,
                        cfw == data.segCopyBtn, cfw == data.segMoveBtn,
                        cfw == data.destCopyBtn, cfw == data.autoGroupBtn,
                        cfw == data.infoBtn, cfw == data.sizeToggle,
                        cfw == data.minBtn, cfw == data.closeBtn));
                const int chipCount = static_cast<int>(data.chipHitRects.size());
                if (msg.message == WM_KEYDOWN && chipKeys && chipCount > 0 &&
                    (msg.wParam == VK_LEFT || msg.wParam == VK_RIGHT)) {
                    const chipscroll::Key k = (msg.wParam == VK_LEFT)
                        ? chipscroll::Key::Left : chipscroll::Key::Right;
                    const int fi = (data.chipFocusIndex < 0)
                        ? 0 : chipscroll::OnKey(k, data.chipFocusIndex, chipCount);
                    data.chipFocusIndex = fi;
                    if (fi >= 0 && fi < chipCount) {
                        const RECT& hr = data.chipHitRects[fi];
                        const int chipLeftAbs =
                            (hr.left - data.chipStripRect.left) + data.chipScrollOffset;
                        const int off = chipscroll::BringIntoView(
                            chipLeftAbs, chipLeftAbs + (hr.right - hr.left),
                            data.chipScrollOffset, data.chipViewportW);
                        data.chipScrollOffset = chipscroll::Clamp(off,
                            chipscroll::MaxOffset(data.chipContentW, data.chipViewportW));
                    }
                    RefreshGroupPanel(&data);
                    continue;
                }
                if (msg.message == WM_KEYDOWN && chipKeys && chipCount > 0 &&
                    msg.wParam == VK_SPACE && data.chipFocusIndex >= 0 &&
                    data.chipFocusIndex < chipCount) {
                    chipfilter::State cs = ChipStateFrom(&data);
                    const int k = data.chipHitKind[data.chipFocusIndex];
                    if (k == 2) chipfilter::ActivateFolders(cs);
                    else if (k == 3) chipfilter::ActivateNoExt(cs);
                    else if (k == 4) chipfilter::ActivateExt(cs, data.chipHitKey[data.chipFocusIndex]);
                    else chipfilter::ActivateItems(cs);
                    ChipStateInto(&data, cs);
                    RefreshGroupPanel(&data);
                    continue;
                }
            }
            if (msg.message == WM_MOUSEWHEEL) {
                // fix thse isseue.md P1: route the wheel by the REAL item-row
                // viewport the painter cached (data.listRowsRect), NOT the whole
                // Selected Items control. Ctrl+wheel (zoom) and a chip-strip pan
                // were already consumed above; here the tested
                // viewmode::WheelScrollsRows is authoritative, so a wheel over
                // the title / chip strip / toggle never scrolls the rows and each
                // wheel event is handled exactly once.
                const bool overRows =
                    PtInRect(&data.listRowsRect, clientPt) != FALSE;
                if (!viewmode::WheelScrollsRows(/*ctrlDown*/false, overChipStrip,
                                                overRows, /*horizontal*/false)) {
                    // Not over the item-row viewport -- let the wheel fall
                    // through (do not scroll the rows from a hover elsewhere).
                    if (!IsDialogMessageW(hwnd, &msg)) {
                        TranslateMessage(&msg);
                        DispatchMessageW(&msg);
                    }
                    continue;
                }
                const int delta =
                    static_cast<int>(static_cast<short>(HIWORD(msg.wParam)));
                // fix thse isseue.md Phase B: a wheel over the list reveals the
                // auto-hiding scrollbar and (re)arms the one-shot idle-hide
                // timer (id 113); the bar stays up for sbauto::kIdleHideMs after
                // the last notch, then fades. Repaint even at the clamp ends so
                // the reveal is visible.
                data.listPointerInside = true;
                data.listSbActivityTick = GetTickCount64();
                SetTimer(hwnd, 113, sbauto::kIdleHideMs, nullptr);
                const int top = listscroll::OnWheel(data.listTopRow, delta,
                                                    scrollRows, visRows, 1);
                data.listTopRow = top;
                RefreshGroupPanel(&data);
                continue;
            }
            const HWND fw = GetFocus();
            // fix thse isseue.md P1: list Up/Down/Home/End/PageUp/PageDown use
            // the SAME centralized focus classification as the chip keys -- the
            // list scrolls ONLY when no interactive control owns focus (now
            // including destCopyBtn/autoGroupBtn/infoBtn/sizeToggle/minBtn/closeBtn).
            const bool keysToList = focusroute::NavKeysDriveChipsOrList(
                focusroute::IsInteractiveFocus(
                    fw == data.edit, fw == data.createBtn, fw == data.cancelBtn,
                    fw == data.segCopyBtn, fw == data.segMoveBtn,
                    fw == data.destCopyBtn, fw == data.autoGroupBtn,
                    fw == data.infoBtn, fw == data.sizeToggle,
                    fw == data.minBtn, fw == data.closeBtn));
            if (msg.message == WM_KEYDOWN && keysToList) {
                bool nav = true;
                listscroll::Key k = listscroll::Key::Up;
                switch (msg.wParam) {
                    case VK_UP:    k = listscroll::Key::Up;       break;
                    case VK_DOWN:  k = listscroll::Key::Down;     break;
                    case VK_HOME:  k = listscroll::Key::Home;     break;
                    case VK_END:   k = listscroll::Key::End;      break;
                    case VK_PRIOR: k = listscroll::Key::PageUp;   break;
                    case VK_NEXT:  k = listscroll::Key::PageDown; break;
                    default:       nav = false;                   break;
                }
                if (nav) {
                    const int top = listscroll::OnKey(k, data.listTopRow,
                                                      scrollRows, visRows);
                    // fix thse isseue.md Phase B: keyboard scrolling also reveals
                    // the auto-hiding scrollbar and arms the idle-hide timer.
                    data.listSbActivityTick = GetTickCount64();
                    SetTimer(hwnd, 113, sbauto::kIdleHideMs, nullptr);
                    data.listTopRow = top;
                    RefreshGroupPanel(&data);
                    continue;
                }
            }
        }
        // Copy/Move segmented control (UI spec item 7/#8): Left -> Copy,
        // Right -> Move, but ONLY when the segmented control owns focus. When
        // the EDIT owns focus these fall through so the text caret moves
        // normally. segmented::OnKey (unit-tested) owns the decision; d->isMove
        // stays the single source of truth the worker reads back.
        // Copy | Move segmented keyboard (Phase 6/D): Left -> Copy, Right ->
        // Move, but ONLY when one of the two segments owns focus (so the EDIT
        // keeps normal caret behaviour). segmented::OnKey (unit-tested) owns
        // the decision; d->isMove is the single state and BOTH segments are
        // repainted so the active highlight follows.
        {
            const HWND kf = GetFocus();
            const bool segFocused =
                (data.segCopyBtn && kf == data.segCopyBtn) ||
                (data.segMoveBtn && kf == data.segMoveBtn);
            if (msg.message == WM_KEYDOWN && data.isGroup && segFocused &&
                (msg.wParam == VK_LEFT || msg.wParam == VK_RIGHT)) {
                segmented::SegKeyResult r = segmented::OnKey(
                    segmented::FromIsMove(data.isMove),
                    static_cast<int>(msg.wParam), true);
                if (r.consumed) {
                    data.isMove = segmented::ToIsMove(r.seg);
                    { HWND gpw = GetParent(data.segCopyBtn ? data.segCopyBtn
                                                           : data.segMoveBtn);
                      if (gpw) SetTimer(gpw, 110, segglide::kStepMs, nullptr); }  // #3 glide
                    if (data.segCopyBtn) InvalidateRect(data.segCopyBtn, nullptr, FALSE);
                    if (data.segMoveBtn) InvalidateRect(data.segMoveBtn, nullptr, FALSE);
                    // Move focus onto the now-active segment so repeated arrows
                    // keep toggling and the focus ring tracks the selection.
                    SetFocus(data.isMove ? data.segMoveBtn : data.segCopyBtn);
                    continue;
                }
            }
            // C/M mnemonics (fix #4/#6): same gate as the arrows -- only when a
            // segment owns focus, so the folder-name EDIT keeps C/M as typed
            // characters. WM_KEYDOWN delivers the uppercase VK for a letter, so
            // 'C'/'M' arrive directly; segmented::Mnemonic (unit-tested) owns the
            // decision and d->isMove stays the single source of truth.
            if (msg.message == WM_KEYDOWN && data.isGroup && segFocused &&
                (msg.wParam == 'C' || msg.wParam == 'M')) {
                segmented::SegKeyResult r = segmented::Mnemonic(
                    segmented::FromIsMove(data.isMove),
                    static_cast<int>(msg.wParam), true);
                if (r.consumed) {
                    data.isMove = segmented::ToIsMove(r.seg);
                    { HWND gpw = GetParent(data.segCopyBtn ? data.segCopyBtn
                                                           : data.segMoveBtn);
                      if (gpw) SetTimer(gpw, 110, segglide::kStepMs, nullptr); }  // #3 glide
                    if (data.segCopyBtn) InvalidateRect(data.segCopyBtn, nullptr, FALSE);
                    if (data.segMoveBtn) InvalidateRect(data.segMoveBtn, nullptr, FALSE);
                    // Mirror the arrow path: focus the now-active segment so the
                    // highlight tracks the choice.
                    SetFocus(data.isMove ? data.segMoveBtn : data.segCopyBtn);
                    continue;
                }
            }
        }
        if (!IsDialogMessageW(hwnd, &msg)) {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
    }

    // fix thse isseue.md Issue 2: stop the thumbnail worker + free every cached
    // bitmap now that the modal loop has ended (BEFORE `data` is destroyed), so
    // the worker can never post to the closing window or race the teardown.
    StopThumbThread(&data);

    // Snapshot the values we need from `data` before `session` frees it below.
    const bool  accepted  = data.accepted;
    const bool  finalMove = data.isMove;
    const bool  autoGroupReq = data.autoGroupRequested;  // ADD-ON: Auto Group confirmed
    std::wstring finalBase = data.base;

    // Bug 4: the popup was never owned or disabled, so there is nothing to
    // re-enable. Just hand focus back to the Explorer window it sat over. fix
    // thse isseue.md #1: the popup always closes before we get here, so focus
    // is always restored to Explorer.
    if (owner) {
        SetForegroundWindow(owner);
    }

    // Hand the final Move/Copy choice back to a group caller (a harmless no-op
    // for New Folder, which passed no seed). fix thse isseue.md #1: with the
    // live round-trip gone the popup is already destroyed here, so the local
    // `session` unique_ptr frees the heap state on return -- no hand-off.
    if (group && group->isGroup) {
        group->isMove = finalMove;
        group->autoGroup = autoGroupReq;  // ADD-ON: worker branches on this
    }

    if (accepted) {
        outBase = finalBase;
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
// a null, destroyed, or no-longer-Explorer target
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
    // active tab now (mirrors ShouldActOnTab). Switching
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
    // Create ONLY when we
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

    // #2 (worker-busy false-cancel fix): a late snapshot no longer cancels on
    // its own. When the worker was busy finishing a previous action the snapshot
    // resolves late, but that alone does not mean the user navigated -- the
    // authoritative drift protection is the snapshot-dir vs current-dir compare
    // and the tab-HWND (#3) guard below. Serialise on those, not on a timer, so a
    // back-to-back trigger (e.g. a template right after a folder) still runs
    // instead of being silently dropped. Mirrors ShouldRunAfterSnapshot in the
    // pure logic. snapshotFresh is now informational only.
    (void)snapshotFresh;

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
    // ShouldRetryOnCollision).
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

// Pending-chord state for two-key shortcuts (Ctrl+F+N, Ctrl+N+F) -- only touched
// on the hook thread. The intermediate non-modifier key (F for Ctrl+F+N, N for
// Ctrl+N+F) is swallowed on its keydown so it never reaches Explorer as a bare
// Ctrl+<key> (which would open search / a new window). If the final trigger key
// does not follow within the chord window, the chord is abandoned: for the
// Ctrl+F+N case the swallowed Ctrl+F is replayed so Explorer search still works.
static bool g_chordPending = false;
static int g_chordPendingVk = 0;
static ULONGLONG g_chordPendingSinceMs = 0;

static const ULONGLONG kChordTimeoutMs = 1200;  // mirrors newfolder_logic.h

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
// Worker message id for the dedicated Ctrl+G "Group Selection" action. Kept
// distinct from the New Folder action (WM_APP+1) and the browse-dir snapshot
// (WM_APP+2) so grouping never touches the folder-creation queue.
#ifndef WM_APP_GROUP
#define WM_APP_GROUP (WM_APP + 3)
#endif

// One captured selection entry (Task 5c). Mirrors the pure GroupItem in
// tests/group_logic.h (path / isFolder / isVirtual) without pulling the test
// header into the mod.
struct CapturedItem {
    std::wstring path;        // filesystem path (empty for a virtual item)
    std::wstring display;     // display name (used to log a virtual item)
    bool isFolder = false;
    bool isVirtual = false;   // no filesystem path -> unsupported for grouping
};

// Capture the CURRENT Explorer selection of `origin` ONCE, on the worker thread
// (COM already initialised there). Fills outItems and the view's folder path in
// outCommonParent. Reuses ResolveActiveShellView. The caller owns outItems and
// is expected to let it go out of scope after use so a selection is never
// reused by a later gesture. Returns true if >=1 item was captured.
static bool CaptureExplorerSelection(HWND origin,
                                     std::vector<CapturedItem>& outItems,
                                     std::wstring& outCommonParent) {
    outItems.clear();
    outCommonParent.clear();
    if (!origin) return false;

    std::wstring viewPath;
    IShellView* view = nullptr;
    if (!ResolveActiveShellView(origin, viewPath, &view) || !view) {
        return false;
    }
    outCommonParent = viewPath;

    IFolderView* folderView = nullptr;
    HRESULT hr = view->QueryInterface(IID_PPV_ARGS(&folderView));
    if (FAILED(hr) || !folderView) {
        SafeRelease(view);
        return false;
    }

    IShellItemArray* items = nullptr;
    hr = folderView->Items(SVGIO_SELECTION, IID_PPV_ARGS(&items));
    SafeRelease(folderView);
    SafeRelease(view);
    if (FAILED(hr) || !items) {
        return false;
    }

    DWORD count = 0;
    if (FAILED(items->GetCount(&count)) || count == 0) {
        SafeRelease(items);
        return false;
    }

    for (DWORD i = 0; i < count; ++i) {
        IShellItem* item = nullptr;
        if (FAILED(items->GetItemAt(i, &item)) || !item) {
            continue;
        }
        CapturedItem ci;

        PWSTR fsPath = nullptr;
        if (SUCCEEDED(item->GetDisplayName(SIGDN_FILESYSPATH, &fsPath)) && fsPath) {
            ci.path = fsPath;
            CoTaskMemFree(fsPath);
            SFGAOF attrs = 0;
            if (SUCCEEDED(item->GetAttributes(SFGAO_FOLDER, &attrs))) {
                ci.isFolder = (attrs & SFGAO_FOLDER) != 0;
            }
            ci.isVirtual = false;
        } else {
            // No filesystem path -> virtual / shell-namespace item.
            ci.isVirtual = true;
            PWSTR disp = nullptr;
            if (SUCCEEDED(item->GetDisplayName(SIGDN_NORMALDISPLAY, &disp)) && disp) {
                ci.display = disp;
                CoTaskMemFree(disp);
            }
        }
        outItems.push_back(std::move(ci));
        SafeRelease(item);
    }

    SafeRelease(items);
    return !outItems.empty();
}

// Ask (0) and Compare (3) let the shell draw its native "Replace or Skip
// Files" box; Replace (1) and Skip (2) suppress it (FOF_NOCONFIRMATION).
// Mirrors FileCollisionShowsNativePrompt in tests/group_logic.h. Used to raise
// the owner window to the foreground before PerformOperations() in exactly the
// native-prompt cases, so the modal box is not hidden behind the active window.
static bool GroupFileCollisionShowsNativePrompt(LONG policy) {
    return policy == 0 /* Ask */ || policy == 3 /* Compare */;
}

// Move or Copy the captured selection into `destFolder` via IFileOperation
// (Task 6b). Copy is the default (isMove=false). The destination folder is
// created first when it does not exist (collision default = use existing).
// Re-validates immediately before PerformOperations: the destination must
// exist and must not be one of the selected folders or nested in it (mirrors
// IsSameOrDescendant / GroupOperationPlan). Runs on the worker thread only.
// ---- Ctrl+G destination resolution (mirrors tests/group_logic.h #2/#3/#8) --
// The pure, unit-tested versions live in tests/group_logic.h; the mod TU cannot
// include that header, so the identical decisions are re-implemented here. Keep
// the two in lockstep (folder_run_tests.ps1 pins the pure side).
enum class GroupResolveKind { Create, UseExisting, Cancel };

struct GroupTargetResolutionM {
    GroupResolveKind kind = GroupResolveKind::Cancel;
    std::wstring path;  // full destination path (empty on Cancel)
};

static bool GroupIsWs(wchar_t c) {
    return c == L' ' || c == L'\t' || c == L'\n' || c == L'\r' ||
           c == L'\f' || c == L'\v';
}
static std::wstring TrimGroupWsM(const std::wstring& s) {
    size_t b = 0, e = s.size();
    while (b < e && GroupIsWs(s[b])) ++b;
    while (e > b && GroupIsWs(s[e - 1])) --e;
    return s.substr(b, e - b);
}
static std::wstring TrimTrailingSepM(const std::wstring& p) {
    if (p.size() > 1 && (p.back() == L'\\' || p.back() == L'/')) {
        if (p.size() == 3 && p[1] == L':') return p;  // keep drive root "C:\"
        return p.substr(0, p.size() - 1);
    }
    return p;
}
static std::wstring GroupJoinPath(const std::wstring& a, const std::wstring& b) {
    if (a.empty()) return b;
    std::wstring out = a;
    if (out.back() != L'\\' && out.back() != L'/') out += L'\\';
    out += b;
    return out;
}

struct GroupDestChoiceM {
    bool valid = false;
    bool useNamedInParent = false;
    std::wstring parentDir;
    std::wstring folderName;
    std::wstring wholePath;
};

// Precedence: fixed destination > typed name under common parent > last target.
static GroupDestChoiceM ResolveGroupDestination(const std::wstring& fixedDest,
                                                const std::wstring& commonParent,
                                                const std::wstring& typedName,
                                                const std::wstring& lastTarget) {
    GroupDestChoiceM out;
    std::wstring fixed = TrimTrailingSepM(TrimGroupWsM(fixedDest));
    if (!fixed.empty()) { out.valid = true; out.wholePath = fixed; return out; }
    std::wstring name = TrimGroupWsM(typedName);
    if (!name.empty()) {
        out.valid = true;
        out.useNamedInParent = true;
        out.parentDir = commonParent;
        out.folderName = name;
        return out;
    }
    std::wstring last = TrimTrailingSepM(TrimGroupWsM(lastTarget));
    if (!last.empty()) { out.valid = true; out.wholePath = last; return out; }
    return out;  // invalid
}

// Mirror of group_logic.h GroupDestinationUsable (fix thse isseue.md P1): a
// resolved/desired destination may be used ONLY when nothing occupies it (it
// will be created) OR what occupies it is a directory. A plain file there is a
// hard error that must abort BEFORE any filesystem side effect.
static bool GroupDestinationUsable(bool destExists, bool destIsDirectory) {
    return !destExists || destIsDirectory;
}

// Folder-exists collision policy (0=UseExisting, 1=Numbered, 2=Cancel), mirrors
// GroupCollisionResolve. Numbered makes "<name> 2", "<name> 3", ... siblings.
static GroupTargetResolutionM ResolveGroupTargetFolder(const std::wstring& dir,
                                                       const std::wstring& name,
                                                       LONG policy) {
    GroupTargetResolutionM out;
    std::wstring full = GroupJoinPath(dir, name);
    if (!PathFileExistsW(full.c_str())) {
        out.kind = GroupResolveKind::Create;
        out.path = full;
        return out;
    }
    if (policy == 0) {  // UseExisting
        // A file (not a directory) occupies the destination name: it cannot be
        // reused as a group folder. Cancel so the caller reports a clear error
        // instead of treating a file as a reusable target (fix thse isseue.md
        // P1 / 13448-13475). full is known to exist here, so test its type.
        if (!GroupDestinationUsable(true, PathIsDirectoryW(full.c_str()) != FALSE)) {
            out.kind = GroupResolveKind::Cancel;
            out.path.clear();
            return out;
        }
        out.kind = GroupResolveKind::UseExisting;
        out.path = full;
        return out;
    }
    if (policy == 1) {  // Numbered
        for (int i = 2; i < 1000000; ++i) {
            std::wstring cand = GroupJoinPath(dir, name + L" " + std::to_wstring(i));
            if (!PathFileExistsW(cand.c_str())) {
                out.kind = GroupResolveKind::Create;
                out.path = cand;
                return out;
            }
        }
    }
    out.kind = GroupResolveKind::Cancel;  // Cancel (2), or Numbered exhausted
    return out;
}

// Canonical comparison form for the self/descendant guard (fix thse isseue.md
// P1): unify separators to '\\', resolve '.'/'..', drop duplicate/trailing
// separators. Pure -- byte-for-byte mirror of tests/group_logic.h
// NormalizePathForCompare, so the live guard and the unit test agree on what
// "the same folder" is and alternate path syntax can never evade it.
static std::wstring NormalizePathForCompare(const std::wstring& path) {
    auto isSep = [](wchar_t ch) { return ch == L'\\' || ch == L'/'; };
    std::wstring s;
    s.reserve(path.size());
    for (wchar_t ch : path) s.push_back(isSep(ch) ? L'\\' : ch);

    std::wstring root;
    std::wstring::size_type i = 0;
    if (s.size() >= 2 && s[0] == L'\\' && s[1] == L'\\') {        // UNC
        root = L"\\\\";
        i = 2;
    } else if (s.size() >= 2 && s[1] == L':') {                  // drive letter
        root = s.substr(0, 2);
        i = 2;
        if (i < s.size() && s[i] == L'\\') { root.push_back(L'\\'); ++i; }
    } else if (!s.empty() && s[0] == L'\\') {                    // rooted
        root = L"\\";
        i = 1;
    }

    std::vector<std::wstring> parts;
    std::wstring::size_type start = i;
    auto consume = [&](std::wstring::size_type end) {
        std::wstring seg = s.substr(start, end - start);
        if (seg.empty() || seg == L".") return;
        if (seg == L"..") {
            if (!parts.empty() && parts.back() != L"..") parts.pop_back();
            else if (root.empty()) parts.push_back(seg);
            return;
        }
        parts.push_back(seg);
    };
    for (std::wstring::size_type k = i; k < s.size(); ++k) {
        if (s[k] == L'\\') { consume(k); start = k + 1; }
    }
    consume(s.size());

    std::wstring out = root;
    for (std::wstring::size_type k = 0; k < parts.size(); ++k) {
        if (!out.empty() && out.back() != L'\\') out.push_back(L'\\');
        out += parts[k];
    }
    // Drop a trailing separator unless it is a bare root ("C:\\" or "\\").
    if (out.size() > 1 && out.back() == L'\\' && !(out.size() == 3 && out[1] == L':')) {
        out.pop_back();
    }
    return out;
}

// True if `child` is `ancestor` itself or nested anywhere beneath it. Both paths
// are canonicalised by NormalizePathForCompare FIRST, so alternate separators,
// '.'/'..' and trailing separators can no longer evade this guard (fix thse
// isseue.md P1). Mirror of tests/group_logic.h IsSameOrDescendant (Win32
// case-insensitive compare, since the shell treats paths case-insensitively).
// ONE source of truth for every self/descendant check -- the per-operation
// guard AND the up-front Auto Group plan validation both call this.
static bool IsSameOrDescendant(const std::wstring& ancestor,
                               const std::wstring& child) {
    const std::wstring a = NormalizePathForCompare(ancestor);
    const std::wstring c = NormalizePathForCompare(child);
    if (a.empty() || c.size() < a.size()) return false;
    if (_wcsnicmp(c.c_str(), a.c_str(), a.size()) != 0) return false;
    return c.size() == a.size() || c[a.size()] == L'\\';
}

static bool PerformGroupOperation(const std::vector<CapturedItem>& items,
                                  const std::wstring& destFolder,
                                  bool isMove,
                                  HWND owner,
                                  groupresult::GroupResultSummary* outSummary = nullptr) {
    // Honest result accounting (fix thse isseue.md 13565-13620 / 14299-14320):
    // default to "nothing grouped" so every early validation return leaves the
    // caller with a truthful (non-inflated) summary. The main path overwrites
    // this with the real tallies.
    if (outSummary) *outSummary = groupresult::GroupResultSummary{};
    if (items.empty() || destFolder.empty()) {
        return false;
    }

    // Validate BEFORE any filesystem side effect so a rejected op never leaves
    // an empty destination directory behind (fix thse isseue.md P1 /
    // 13485-13511). Order: destination-type, then self/descendant, then create.

    // (1) Destination-type: if something already occupies the destination path
    //     it MUST be a directory. A plain file there is a hard error -- never
    //     delete it or copy into it. Mirrors group_logic.h GroupDestinationUsable.
    const bool destExists = PathFileExistsW(destFolder.c_str()) != FALSE;
    if (!GroupDestinationUsable(destExists,
                                PathIsDirectoryW(destFolder.c_str()) != FALSE)) {
        Wh_Log(L"Group: destination '%ls' is a file, not a folder -- aborting",
               destFolder.c_str());
        return false;
    }

    // (2) Self/descendant: never move/copy a selected folder into itself or one
    //     of its own descendants. Done here -- before the destination is created
    //     -- mirroring ValidateGroup's ordering.
    for (const auto& it : items) {
        // Self/descendant via the shared, canonicalising IsSameOrDescendant
        // (fix thse isseue.md P1): alternate separators, '.'/'..' and trailing
        // separators are normalised first so none can evade the guard.
        if (it.isFolder && IsSameOrDescendant(it.path, destFolder)) {
            Wh_Log(L"Group: refusing to place '%ls' into itself/descendant '%ls'",
                   it.path.c_str(), destFolder.c_str());
            return false;
        }
    }

    // (3) Only now create the destination folder if it does not yet exist
    //     (reuse the existing New Folder shell-create path). Collision default:
    //     an already-existing folder is reused, not overwritten.
    if (!destExists) {
        if (!CreateFolderWithShellElevation(destFolder, owner)) {
            Wh_Log(L"Group: failed to create destination '%ls'", destFolder.c_str());
            return false;
        }
    }

    // Re-validate immediately before the operation: the destination must now
    // exist AND be a directory (a create could race, or a file could appear).
    if (!PathIsDirectoryW(destFolder.c_str())) {
        Wh_Log(L"Group: destination not a usable folder before op '%ls'",
               destFolder.c_str());
        return false;
    }

    IShellItem* destItem = nullptr;
    HRESULT hr = SHCreateItemFromParsingName(destFolder.c_str(), nullptr,
                                             IID_PPV_ARGS(&destItem));
    if (FAILED(hr) || !destItem) {
        Wh_Log(L"Group: SHCreateItemFromParsingName(dest) failed hr=0x%08X", hr);
        return false;
    }

    IFileOperation* fileOp = nullptr;
    hr = CoCreateInstance(CLSID_FileOperation, nullptr, CLSCTX_ALL,
                          IID_PPV_ARGS(&fileOp));
    if (FAILED(hr) || !fileOp) {
        SafeRelease(destItem);
        Wh_Log(L"Group: CoCreateInstance(CLSID_FileOperation) failed hr=0x%08X", hr);
        return false;
    }
    if (owner && IsWindow(owner)) {
        fileOp->SetOwnerWindow(owner);
    }
    // Issue 1: file-level collision behavior. The pure mapping lives in
    // group_logic.h (mirrored below for the tests): Ask -> no extra flags (the
    // shell shows its native "Replace or Skip Files" box), Replace ->
    // FOF_NOCONFIRMATION (overwrite, no prompt), Skip -> FOF_NOCONFIRMATION plus
    // a private sentinel bit that we handle here (drop colliding items before
    // queueing) and then mask off before touching the shell.
    const LONG fileCollision = AtomicGet(&g_groupFileCollision);
    unsigned int policyFlags = 0;  // FileCollisionFlags(Ask)
    if (fileCollision == 1) {          // Replace
        policyFlags = FOF_NOCONFIRMATION;
    } else if (fileCollision == 2) {   // Skip
        policyFlags = FOF_NOCONFIRMATION | kGroupSkipExistingBit;
    } else if (fileCollision == 3) {   // Compare (native box, incl. compare view)
        policyFlags = kGroupCompareBit;
    }
    const bool skipExisting = (policyFlags & kGroupSkipExistingBit) != 0;
    if (policyFlags & kGroupCompareBit) {
        Wh_Log(L"Group: file collision = compare (native Replace/Skip/Compare "
               L"box will be shown for '%ls')", destFolder.c_str());
    }
    // Real shell flags only: strip BOTH private sentinels, always keep
    // NOCOPYHOOKS. Compare/Ask leave confirmation on so the native box appears.
    const DWORD shellFlags =
        FOFX_NOCOPYHOOKS |
        (policyFlags & ~(kGroupSkipExistingBit | kGroupCompareBit));
    fileOp->SetOperationFlags(shellFlags);

    int requested = 0;      // non-virtual, non-empty items we actually consider
    int queued = 0;
    int skipped = 0;
    int failedToQueue = 0;  // could not build a shell item, or Move/Copy failed
    for (const auto& it : items) {
        if (it.isVirtual || it.path.empty()) continue;  // safety - already filtered upstream
        ++requested;  // this item is in scope; it lands in exactly one bucket below

        // Skip policy: if a file/folder with this leaf name already exists in
        // the destination, keep the existing one and drop the incoming item.
        if (skipExisting) {
            PCWSTR leaf = PathFindFileNameW(it.path.c_str());
            std::wstring cand = destFolder;
            if (!cand.empty() && cand.back() != L'\\' && cand.back() != L'/') {
                cand += L'\\';
            }
            cand += leaf;
            if (PathFileExistsW(cand.c_str())) {
                ++skipped;
                continue;
            }
        }

        IShellItem* srcItem = nullptr;
        if (FAILED(SHCreateItemFromParsingName(it.path.c_str(), nullptr,
                                               IID_PPV_ARGS(&srcItem))) || !srcItem) {
            ++failedToQueue;  // could not reference the source -- NOT grouped
            continue;
        }
        HRESULT op = isMove
            ? fileOp->MoveItem(srcItem, destItem, nullptr, nullptr)
            : fileOp->CopyItem(srcItem, destItem, nullptr, nullptr);
        if (SUCCEEDED(op)) ++queued; else ++failedToQueue;
        SafeRelease(srcItem);
    }
    if (skipped > 0) {
        Wh_Log(L"Group: skipped %d item(s) already present in '%ls' "
               L"(file collision = skip)", skipped, destFolder.c_str());
    }

    bool ok = false;
    bool performed = false;
    BOOL aborted = FALSE;
    if (queued > 0) {
        // The shell's native "Replace or Skip Files" box (shown for Ask and
        // Compare) is modal to `owner`. If Explorer is not in the foreground
        // when it appears, the box is drawn BEHIND the window the user is
        // looking at and goes unnoticed. Raise the owner to the foreground
        // first so the prompt surfaces on top. Replace/Skip suppress the box,
        // so the raise is skipped for them.
        if (owner && IsWindow(owner) &&
            GroupFileCollisionShowsNativePrompt(fileCollision)) {
            ForceForeground(owner);
        }
        hr = fileOp->PerformOperations();
        fileOp->GetAnyOperationsAborted(&aborted);
        performed = SUCCEEDED(hr);
        ok = performed && !aborted;
        Wh_Log(L"Group: %s %d item(s) into '%ls' hr=0x%08X aborted=%d",
               isMove ? L"moved" : L"copied", queued, destFolder.c_str(), hr, aborted);
    } else {
        Wh_Log(L"Group: no items queued for operation");
    }

    // Fold the raw tallies into an HONEST summary (fix thse isseue.md
    // 13565-13620 / 14299-14320) so the caller reports the count that ACTUALLY
    // landed -- requested == queued + skipped + failedToQueue, and grouped
    // never exceeds queued.
    groupresult::GroupOpOutcome outcome;
    outcome.requested = requested;
    outcome.skipped = skipped;
    outcome.failedToQueue = failedToQueue;
    outcome.queued = queued;
    outcome.performed = performed;
    outcome.aborted = (aborted != FALSE);
    if (outSummary) *outSummary = groupresult::ClassifyGroupResult(outcome);

    SafeRelease(fileOp);
    SafeRelease(destItem);
    return ok;
}

// ADD-ON (Phase E): execute an Auto Group request. Pure planning lives in the
// mirrored autogroup:: model; the ACTUAL filesystem work reuses the existing,
// safe PerformGroupOperation() once PER destination bucket, so directory
// creation, self/descendant guards, IFileOperation, Copy/Move and the file
// collision policy all behave exactly like a normal Ctrl+G group. The worker
// MUST have already branched on the explicit Auto Group request before any
// typed-name / g_lastGroupTarget resolution (never reuse the last target).
//
// `fixedDest` is the configured Fixed destination (may be empty). `parent` is
// the captured common parent (may be empty when the selection spans several
// folders). isMove is the user's final Copy/Move choice. Returns true only when
// every bucket that had items reported success; reports partial results in the
// log rather than claiming a clean success.
static bool PerformAutoGroupOperation(const std::vector<CapturedItem>& items,
                                      const std::wstring& fixedDest,
                                      const std::wstring& parent,
                                      bool isMove,
                                      HWND owner) {
    // Build the immutable source list from the ORIGINAL captured selection only
    // (never an enumeration of the destination), so Auto Group's own generated
    // folders can never be swept back in.
    std::vector<autogroup::SrcItem> src;
    src.reserve(items.size());
    for (const auto& it : items) {
        if (it.isVirtual || it.path.empty()) continue;  // unsupported/virtual
        src.push_back(autogroup::SrcItem{it.path, it.isFolder});
    }

    autogroup::Settings st;
    st.layout = (AtomicGet(&g_autoGroupLayout) != 0)
                    ? autogroup::Layout::ImagesParent
                    : autogroup::Layout::Flat;
    st.groupSelectedFolders = AtomicGet(&g_autoGroupSelectedFolders) != 0;
    st.noExtensionEnabled = AtomicGet(&g_autoGroupNoExtension) != 0;
    st.isMove = isMove;
    {
        EnterCriticalSection(&g_templatesCs);
        st.imagesParentName = g_autoGroupImageParentName;
        st.allFolderName = g_autoGroupFoldersName;
        st.noExtensionName = g_autoGroupNoExtensionName;
        // Phase 6: parse the configured custom rules into a validated plan.
        // Empty config -> no rules -> unchanged default behaviour.
        st.customRules = autogroup::ParseCustomRules(g_autoGroupCustomRuleLines);
        LeaveCriticalSection(&g_templatesCs);
    }

    autogroup::Plan plan = autogroup::BuildPlan(src, fixedDest, st);
    if (!plan.ok) {
        Wh_Log(L"AutoGroup: destination could not be resolved (status=%d); the "
               L"selection needs a common parent or a configured destination -- "
               L"no filesystem change",
               static_cast<int>(plan.dest.status));
        return false;
    }
    if (plan.files.empty() && plan.folders.empty()) {
        Wh_Log(L"AutoGroup: nothing eligible to organize (skipped=%zu); no "
               L"filesystem change", plan.skipped.size());
        return false;
    }

    // fix thse isseue.md P1 (path safety): validate the self/descendant rule for
    // the WHOLE plan BEFORE creating any directory, so no destination is ever
    // created on disk only for a per-operation guard to reject the move later.
    // This matters most for the optional All Folder behavior, which MOVES the
    // originally selected folders: a configured/derived destination that is one
    // of those folders -- or sits inside one -- would be a move-into-self. Paths
    // are canonicalised by IsSameOrDescendant, so alternate separators / '.' /
    // '..' cannot slip past this up-front check either.
    for (const auto& fo : plan.folders) {
        if (IsSameOrDescendant(fo.src, fo.destDir)) {
            Wh_Log(L"AutoGroup: refusing to move selected folder '%ls' into "
                   L"itself/descendant '%ls'; aborting BEFORE any directory is "
                   L"created", fo.src.c_str(), fo.destDir.c_str());
            return false;
        }
    }

    // Phase F: create every planned destination directory UP FRONT, in the
    // plan's deterministic order, so a parent (e.g. the Images parent) always
    // exists before its child bucket -- CreateFolderWithShellElevation makes a
    // single leaf only and fails when the parent is missing. An existing
    // directory is reused; an ordinary FILE occupying a required path blocks
    // that bucket safely (never overwritten) and anything targeting it is
    // skipped rather than mis-routed.
    bool allOk = true;
    std::vector<std::wstring> blockedDirs;
    auto dirBlocked = [&](const std::wstring& dir) {
        for (const auto& b : blockedDirs) {
            if (autogroup::PathEqualCI(b, dir)) return true;
        }
        return false;
    };
    int dirsCreated = 0;
    for (const auto& dir : plan.dirsToCreate) {
        if (PathFileExistsW(dir.c_str())) {
            if (!PathIsDirectoryW(dir.c_str())) {
                blockedDirs.push_back(dir);
                Wh_Log(L"AutoGroup: a file occupies '%ls'; that bucket is "
                       L"skipped (never overwritten)", dir.c_str());
            }
            continue;  // reuse an existing directory
        }
        if (!CreateFolderWithShellElevation(dir, owner)) {
            blockedDirs.push_back(dir);
            allOk = false;
            Wh_Log(L"AutoGroup: could not create '%ls'; that bucket is skipped",
                   dir.c_str());
            continue;
        }
        ++dirsCreated;
    }

    // Route each eligible FILE into its destination bucket. Each
    // PerformGroupOperation call handles exactly one bucket (one IFileOperation
    // batch) and reuses the directory created above; a bucket blocked by a file
    // is skipped. The Copy/Move choice and the file-collision policy are the
    // same as a normal Ctrl+G group.
    // fix thse isseue.md P2 (13636-13750): build the file -> bucket assignment
    // in ONE pass instead of rescanning plan.files for every bucket (the old
    // O(buckets x files) loop). Key by the normalized dir (autogroupbucket::
    // NormalizeDirKey, mirrored + unit-tested) so each file lands in the same
    // bucket a PathEqualCI filter would have matched.
    std::unordered_map<std::wstring, std::vector<CapturedItem>> bucketMap;
    bucketMap.reserve(plan.dirsToCreate.size() * 2);
    for (const auto& f : plan.files) {
        CapturedItem ci;
        ci.path = f.src;
        ci.isFolder = false;
        bucketMap[autogroupbucket::NormalizeDirKey(f.destDir)].push_back(ci);
    }

    // fix thse isseue.md P1 (partial-success accounting): request the detailed
    // GroupResultSummary for every bucket and fold the HONEST per-bucket counts
    // into one aggregate. filesRouted now counts what ACTUALLY landed (grouped),
    // never the requested bucket size, and any bucket that is blocked or only
    // partially queued (skip / fail / abort) clears the aggregate's
    // completeness -- so the run can no longer log "completed" while items were
    // dropped.
    groupresult::GroupAggregate agg;
    int bucketsRun = 0, filesRouted = 0;
    for (const auto& dir : plan.dirsToCreate) {
        auto it = bucketMap.find(autogroupbucket::NormalizeDirKey(dir));
        if (it == bucketMap.end() || it->second.empty()) {
            continue;  // a parent-only dir (e.g. Images) -- no files routed here
        }
        const std::vector<CapturedItem>& bucketItems = it->second;
        if (dirBlocked(dir)) {
            // The bucket directory could not be created, or a plain file
            // occupies it: its files cannot be routed. Count them as failed so
            // the aggregate is honest and the run is reported PARTIAL, never
            // "completed".
            groupresult::GroupResultSummary blockedSummary;
            blockedSummary.kind = groupresult::GroupResultKind::Failed;
            blockedSummary.requested = static_cast<int>(bucketItems.size());
            blockedSummary.failed = static_cast<int>(bucketItems.size());
            groupresult::FoldGroupResult(agg, blockedSummary);
            Wh_Log(L"AutoGroup: bucket '%ls' is blocked; %zu file(s) not routed",
                   dir.c_str(), bucketItems.size());
            continue;
        }
        ++bucketsRun;
        groupresult::GroupResultSummary bucketSummary;
        const bool bucketOk =
            PerformGroupOperation(bucketItems, dir, isMove, owner, &bucketSummary);
        groupresult::FoldGroupResult(agg, bucketSummary);
        filesRouted += bucketSummary.grouped;
        if (!bucketOk ||
            bucketSummary.kind != groupresult::GroupResultKind::Complete) {
            Wh_Log(L"AutoGroup: bucket '%ls' did not fully complete "
                   L"(%d of %d grouped, %d skipped, %d failed%ls)",
                   dir.c_str(), bucketSummary.grouped, bucketSummary.requested,
                   bucketSummary.skipped, bucketSummary.failed,
                   bucketSummary.groupedExact ? L"" : L", aborted");
        }
    }

    // Phase F: selected-folder grouping. Only when the setting is on did
    // BuildPlan queue the ORIGINAL captured folders for the All Folder
    // directory (the generated output buckets are NEVER included). Selected
    // folders are MOVED into All Folder regardless of the file Copy/Move choice
    // (spec section 7), so the originals are relocated, not duplicated.
    int foldersMoved = 0;
    if (!plan.folders.empty()) {
        const std::wstring allDir = plan.folders.front().destDir;
        if (dirBlocked(allDir)) {
            allOk = false;
            Wh_Log(L"AutoGroup: '%ls' is blocked; selected folders left "
                   L"untouched", allDir.c_str());
        } else {
            std::vector<CapturedItem> folderItems;
            for (const auto& fo : plan.folders) {
                CapturedItem ci;
                ci.path = fo.src;
                ci.isFolder = true;
                folderItems.push_back(ci);
            }
            const bool kFolderMove = true;  // folders always MOVED, never copied
            groupresult::GroupResultSummary folderSummary;
            const bool folderOk = PerformGroupOperation(folderItems, allDir,
                                                        kFolderMove, owner,
                                                        &folderSummary);
            groupresult::FoldGroupResult(agg, folderSummary);
            foldersMoved = folderSummary.grouped;
            if (!folderOk ||
                folderSummary.kind != groupresult::GroupResultKind::Complete) {
                Wh_Log(L"AutoGroup: All Folder move did not fully complete for "
                       L"'%ls' (%d of %d moved%ls)",
                       allDir.c_str(), folderSummary.grouped,
                       folderSummary.requested,
                       folderSummary.groupedExact ? L"" : L", aborted");
            }
        }
    }

    // fix thse isseue.md P1: "completed" requires BOTH no external/structural
    // failure (allOk: every planned dir created, no blocked All Folder) AND an
    // aggregate where every bucket that ran was Complete. filesRouted/
    // foldersMoved are the HONEST grouped counts folded above, and agg.skipped/
    // agg.failed are logged so a partial run is never mistaken for a clean one.
    const bool completed =
        groupresult::AggregateComplete(agg, /*externalFailure=*/!allOk);
    Wh_Log(L"AutoGroup: %ls -- %d dir(s) created, %d bucket(s), %d file(s) "
           L"routed, %d folder(s) -> All Folder, %d skipped, %d failed, "
           L"%zu plan-skipped, root='%ls', fileOp=%ls",
           completed ? L"completed" : L"PARTIAL (see failures above)",
           dirsCreated, bucketsRun, filesRouted, foldersMoved,
           agg.skipped, agg.failed,
           plan.skipped.size(), plan.dest.root.c_str(),
           isMove ? L"move" : L"copy");
    return completed;
}

// Ctrl+G fired: offload a Group action to the worker. Non-blocking -- the hook
// thread never does COM / filesystem work inline (same rule as
// PostFolderActionCount). Selection capture (Task 5c) and Move/Copy via
// IFileOperation (Task 6b) run on the worker; the popup for a custom name is
// Task 8.
static void GroupTriggerFired() {
    if (!g_workerThreadId) {
        return;
    }
    if (!PostThreadMessageW(g_workerThreadId, WM_APP_GROUP, 0, 0)) {
        Wh_Log(L"GroupTriggerFired: PostThreadMessageW(WM_APP_GROUP) failed "
               L"(error=%lu)", GetLastError());
    }
}

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
    // bug). Mirrors ShouldOpenCaptureWindow.
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

    // The worker runs COM / file-system / UIA work, not latency-critical input,
    // so it stays at normal priority. (The low-level hook thread is raised.)
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

            if (msg.message == WM_APP_GROUP) {
                // Dedicated Ctrl+G "Group Selection" action. Task 5c: capture
                // the current Explorer selection ONCE, validate it, log a
                // summary. Move/Copy via IFileOperation is Task 6b.
                //
                // `sel` and `parent` are LOCAL to this handler: they are
                // destroyed when this iteration ends, so a captured selection is
                // single-use and can never be reused by a later Ctrl+G -- the
                // next gesture re-captures fresh.
                HWND groupOrigin = GetTargetExplorerHWND();
                if (!groupOrigin) {
                    Wh_Log(L"Ctrl+G: no Explorer target window; safe-cancel");
                    continue;
                }
                std::vector<CapturedItem> sel;
                std::wstring parent;
                if (!CaptureExplorerSelection(groupOrigin, sel, parent)) {
                    Wh_Log(L"Ctrl+G: could not read a selection; safe-cancel");
                    continue;
                }
                // SAFETY (mirrors ValidateGroup in tests/group_logic.h):
                //   empty selection -> do nothing.
                if (sel.empty()) {
                    Wh_Log(L"Ctrl+G: empty selection; no action");
                    continue;
                }
                //   any virtual / non-filesystem item -> cancel the whole gesture.
                bool anyVirtual = false;
                for (const auto& it : sel) {
                    if (it.isVirtual) { anyVirtual = true; break; }
                }
                if (anyVirtual) {
                    Wh_Log(L"Ctrl+G: selection has a virtual/non-filesystem item; "
                           L"cancel");
                    continue;
                }
                // Valid selection captured. Log a short summary (cap names at 5).
                Wh_Log(L"Ctrl+G: captured %zu item(s); parent='%ls'",
                       sel.size(), parent.c_str());
                {
                    size_t shown = sel.size() < 5 ? sel.size() : 5;
                    for (size_t i = 0; i < shown; ++i) {
                        Wh_Log(L"  [%zu] %ls%ls", i, sel[i].path.c_str(),
                               sel[i].isFolder ? L" (folder)" : L"");
                    }
                }

                // Task 8/9: show the Group popup FIRST so the user names the
                // folder and picks Move/Copy BEFORE anything is created, then
                // resolve the destination by precedence and honor every group
                // setting (Move/Copy default, folder-collision policy, fixed
                // destination, open-after).
                GroupPromptSeed seed;
                seed.isGroup = true;
                {
                    // Full selection for the scrollable Selected Items panel
                    // (name + folder flag + real size on disk). Sizes are read
                    // ONCE here (worker thread, metadata-only) and cached on the
                    // model so the owner-draw paint path never touches the disk.
                    for (size_t i = 0; i < sel.size(); ++i) {
                        sel::Item it;
                        it.name = sel[i].isVirtual
                                      ? sel[i].display
                                      : GetFileNamePart(sel[i].path);
                        it.isFolder = sel[i].isFolder;
                        // fix thse isseue.md Issue 2: keep the real path (empty
                        // for virtual items) so grid views can extract an image
                        // thumbnail; never used by grouping / file operations.
                        it.path = sel[i].isVirtual ? std::wstring() : sel[i].path;
                        if (!it.isFolder && !sel[i].isVirtual) {
                            it.sizeBytes = ResolveFileSizeOnDisk(sel[i].path);
                        }
                        seed.items.push_back(it);
                    }
                }
                seed.lastTarget = g_lastGroupTarget;
                seed.isMove = (AtomicGet(&g_groupIsMove) != 0);  // setting default

                // Read the optional fixed destination under the same lock the
                // settings thread uses to write it.
                std::wstring fixedDest;
                EnterCriticalSection(&g_templatesCs);
                fixedDest = g_groupDestination;
                LeaveCriticalSection(&g_templatesCs);

                // Popup preview: a fixed destination is shown up front; otherwise
                // the typed name fills it in.
                seed.destPath = TrimTrailingSepM(TrimGroupWsM(fixedDest));

                // fix thse isseue.md #2: hand the captured parent to the popup as
                // the destination PREVIEW context (shown when no fixed dest is
                // set). Resolution precedence is unchanged (ResolveGroupDestination).
                seed.effectiveDest = parent;

                std::wstring typedName;
                if (!PromptForFolderName(groupOrigin, typedName, &seed)) {
                    Wh_Log(L"Ctrl+G: popup cancelled; no action");
                    continue;
                }
                const bool isMove = seed.isMove;  // user's final Move/Copy choice

                // ADD-ON (Phase E): branch on the EXPLICIT Auto Group request
                // BEFORE any typed-name resolution and before anything that can
                // reuse g_lastGroupTarget, so Auto Group can never fall through
                // the normal blank-name path. The dedicated executor resolves
                // its own destination (fixed dest, else the captured common
                // parent) and routes each eligible file into its extension
                // bucket. g_lastGroupTarget is intentionally NOT touched here.
                if (seed.autoGroup) {
                    const bool agOk = PerformAutoGroupOperation(
                        sel, fixedDest, parent, isMove, groupOrigin);
                    if (agOk && AtomicGet(&g_groupOpenAfter) != 0) {
                        // Open the resolved destination root (fixed dest, else
                        // the captured parent) after a successful Auto Group.
                        const std::wstring openRoot =
                            !fixedDest.empty() ? fixedDest : parent;
                        if (!openRoot.empty()) {
                            NavigateActiveTabToFolder(groupOrigin, nullptr, openRoot);
                        }
                    }
                    continue;
                }

                GroupDestChoiceM choice =
                    ResolveGroupDestination(fixedDest, parent, typedName,
                                            g_lastGroupTarget);
                if (!choice.valid) {
                    Wh_Log(L"Ctrl+G: no destination (blank name, nothing "
                           L"remembered); no action");
                    continue;
                }

                std::wstring destFolder;
                if (choice.useNamedInParent) {
                    if (choice.parentDir.empty()) {
                        Wh_Log(L"Ctrl+G: selection spans multiple parents; set a "
                               L"fixed destination or select items in one folder");
                        continue;
                    }
                    const LONG collision = AtomicGet(&g_groupCollision);
                    GroupTargetResolutionM res = ResolveGroupTargetFolder(
                        choice.parentDir, choice.folderName, collision);
                    if (res.kind == GroupResolveKind::Cancel) {
                        Wh_Log(L"Ctrl+G: destination '%ls' exists and collision "
                               L"policy is Cancel; no action",
                               GroupJoinPath(choice.parentDir,
                                             choice.folderName).c_str());
                        continue;
                    }
                    destFolder = res.path;
                } else {
                    // Fixed destination or reused last target: an exact folder.
                    destFolder = choice.wholePath;
                }

                // fix thse isseue.md #5: report the HONEST grouped count, not
                // the raw selection size. PerformGroupOperation fills `summary`
                // with how many items actually landed (grouped), how many were
                // skipped (collision) and how many failed to queue.
                groupresult::GroupResultSummary summary;
                PerformGroupOperation(sel, destFolder, isMove, groupOrigin, &summary);
                const bool anyGrouped = summary.grouped > 0;
                if (summary.kind == groupresult::GroupResultKind::Complete) {
                    Wh_Log(L"Ctrl+G: grouped %d item(s) %ls -> '%ls'",
                           summary.grouped, isMove ? L"(move)" : L"(copy)",
                           destFolder.c_str());
                } else if (summary.kind == groupresult::GroupResultKind::Partial) {
                    // Partial: say how many of the selection actually landed and
                    // why the rest did not. On abort the count is an upper bound
                    // ("up to N"); never claim the full selection size.
                    Wh_Log(L"Ctrl+G: grouped %s%d of %d item(s) %ls -> '%ls' "
                           L"(%d skipped, %d failed%s)",
                           summary.groupedExact ? L"" : L"up to ",
                           summary.grouped, summary.requested,
                           isMove ? L"(move)" : L"(copy)", destFolder.c_str(),
                           summary.skipped, summary.failed,
                           summary.groupedExact ? L"" : L", aborted");
                } else {
                    Wh_Log(L"Ctrl+G: group operation did not complete "
                           L"(%d requested, 0 grouped)", summary.requested);
                }
                if (anyGrouped) {
                    g_lastGroupTarget = destFolder;  // remember for blank-name reuse
                    // "Open destination after grouping" (off by default).
                    if (AtomicGet(&g_groupOpenAfter) != 0) {
                        NavigateActiveTabToFolder(groupOrigin, nullptr, destFolder);
                    }
                }
                // fix thse isseue.md #1: the operation result was logged above; the popup already closed, so nothing more to post.
                // sel / parent go out of scope here -> capture is single-use.
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
                // DRAIN (discard) the dequeued action
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

// Runs on the hook thread and
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

    // ---- Dedicated Ctrl+G "Group Selection" (hardwired, independent) ----
    // Ctrl+G is NOT the New Folder trigger and never enters the chord/count
    // machinery below. Mirror of ClassifyTrigger (tests/group_logic.h):
    // exactly Ctrl + 'G' (VK 0x47), no other modifier, toggle on. On match we
    // offload a Group action to the worker and SWALLOW the key so Explorer
    // never sees a bare Ctrl+G. Injected input already returned above.
    if (vk == 'G' && AtomicGet(&g_groupSelection) != 0) {
        const bool ctrlDown =
            (GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0;
        const bool otherModifierDown =
            (GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0 ||
            (GetAsyncKeyState(VK_MENU) & 0x8000) != 0 ||
            (GetAsyncKeyState(VK_LWIN) & 0x8000) != 0 ||
            (GetAsyncKeyState(VK_RWIN) & 0x8000) != 0;
        if (ctrlDown && !otherModifierDown && !g_countActive) {
            Wh_Log(L"Ctrl+G pressed: posting Group Selection action");
            GroupTriggerFired();
            return 1;  // swallow Ctrl+G; Explorer never sees it
        }
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
            // BUG FIX (Ctrl+Shift+N + template letter): to type a LOWERCASE
            // template letter the user must release Shift -- which flips a
            // configured hold-modifier to released. A bound template letter
            // arriving on (or after) that release MUST still fire its template,
            // not be swallowed by the count-commit below. Otherwise the first
            // attempt committed a single default folder and the template never
            // ran (a second attempt, keys held differently, then "worked").
            // Mirrors CaptureLetterDecide(BoundLetter) -> FireTemplate,
            // independent of modifier state.
            if (vk >= 'A' && vk <= 'Z') {
                wchar_t tkey = static_cast<wchar_t>(vk - 'A' + L'a');
                if (IsTemplateKeyBound(
                        (unsigned long)AtomicGet(&g_templateKeyMask), tkey)) {
                    Wh_Log(L"Template letter '%c' pressed after modifier release; "
                           L"posting template action", tkey);
                    PostTemplateAction(tkey);
                    EndCountCapture();  // cancel count window WITHOUT count folders
                    return 1;           // swallow the letter
                }
                // Unbound letter: fall through to the count-commit below.
            }

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

    // #3: read the shortcut as ONE atomic snapshot (never the tearable struct).
    const ShortcutConfig sc = CurrentShortcut();
    const ULONGLONG now = GetTickCount64();

    // The middle key of a two-key chord (F for Ctrl+F+N, N for Ctrl+N+F), or 0
    // for a single-press shortcut. Chords need the pending-chord path so the
    // middle key is swallowed instead of leaking to Explorer as a bare
    // Ctrl+<key> (search / new window) before the trigger arrives.
    const int chordVk = ChordIntermediateVk(sc);

    // Resolve a pending chord already in progress.
    if (g_chordPending) {
        const bool expired = (now - g_chordPendingSinceMs) > kChordTimeoutMs;

        // The trigger key completed the chord in time (with modifiers held):
        // fire, and swallow the trigger key-up so no stray char reaches Explorer.
        if (!expired && fg && vk == sc.triggerVk &&
            IsGestureModifierHeldNow(sc)) {
            g_chordPending = false;
            g_chordPendingVk = 0;
            if (vk == 'F') g_swallowUpF = true;
            if (vk == 'N') g_swallowUpN = true;
            TriggerFired();
            return 1;
        }

        // Auto-repeat of the held middle key while modifiers stay down: keep
        // swallowing it and stay pending (do not leak repeats to Explorer).
        if (!expired && vk == g_chordPendingVk && IsGestureModifierHeldNow(sc)) {
            return 1;
        }

        // Chord broken or timed out: abandon. Replay the Ctrl+F we swallowed for
        // the Ctrl+F+N case so Explorer search still works; the current key then
        // falls through to Explorer normally.
        const int abandonedVk = g_chordPendingVk;
        g_chordPending = false;
        g_chordPendingVk = 0;
        if (abandonedVk == 'F') {
            ReplayCtrlF();
        }
        // fall through: let this key reach Explorer
    }

    // Start a pending chord: the middle key was pressed with the shortcut's
    // modifiers held and Explorer in front. Swallow it now; the gesture
    // completes only if the trigger key follows within the chord window.
    if (chordVk != 0 && fg && vk == chordVk && IsGestureModifierHeldNow(sc)) {
        g_chordPending = true;
        g_chordPendingVk = chordVk;
        g_chordPendingSinceMs = now;
        if (chordVk == 'F') g_swallowUpF = true;
        if (chordVk == 'N') g_swallowUpN = true;
        return 1;  // swallow the middle key
    }

    // Fire a single-press shortcut when the trigger key is pressed while every
    // preceding modifier remains held (Ctrl+N, Ctrl+Q, Ctrl+G, Ctrl+Shift+N).
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
    // above.
    Wh_Log(L"INIT: v0.12.71 ctrlg-group loaded");
    // Guard g_templates/g_templateCollision. Created BEFORE the first
    // LoadSettings() call (which publishes into them) and before the hook
    // starts, so no thread ever touches the CS uninitialised.
    InitializeCriticalSection(&g_templatesCs);
    LoadSettings();
    return KeybdHook_Init();
}

// Tool-mod boilerplate hook: Wh_ModSettingsChanged routes here so the launcher
// snippet stays identical across tool mods (it always calls
// WhTool_ModSettingsChanged, never LoadSettings directly).
void WhTool_ModSettingsChanged() {
    LoadSettings();
}

void WhTool_ModUninit() {
    Wh_Log(L"Uninit");
    KeybdHook_Exit();
    FreeThemeBrushes();
    if (g_uiFont) {
        DeleteObject(g_uiFont);
        g_uiFont = nullptr;
    }
    // fix thse isseue.md #1: release the Destination monospace path font.
    if (g_uiFontMono) {
        DeleteObject(g_uiFontMono);
        g_uiFontMono = nullptr;
    }
    // add-on section 2: release the bold monospace key-token font.
    if (g_uiFontMonoBold) {
        DeleteObject(g_uiFontMonoBold);
        g_uiFontMonoBold = nullptr;
    }
    // S14 (fix thse isseue.md v0.12.54): release the semibold title font too.
    if (g_uiFontSemibold) {
        DeleteObject(g_uiFontSemibold);
        g_uiFontSemibold = nullptr;
    }
    // fix thse isseue.md (nav-help typography): release the italic help font.
    if (g_uiFontItalic) {
        DeleteObject(g_uiFontItalic);
        g_uiFontItalic = nullptr;
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
    // Tool-mod boilerplate: never load in the session-0 (services) instance --
    // there is no interactive desktop there, so the hook/popup have nothing to
    // act on. Part of the wiki snippet that had drifted out.
    DWORD sessionId = 0;
    if (ProcessIdToSessionId(GetCurrentProcessId(), &sessionId) &&
        sessionId == 0) {
        return FALSE;
    }

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

    WhTool_ModSettingsChanged();
}

void Wh_ModUninit() {
    if (g_isToolModProcessLauncher) {
        return;
    }

    WhTool_ModUninit();
    ExitProcess(0);
}

