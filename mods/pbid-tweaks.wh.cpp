// ==WindhawkMod==
// @id              pbid-tweaks
// @name            Skins & Tools for Power BI
// @description     Skins for the Power BI Desktop window, plus layout, filter, theme, interaction, color and mock-up tools, injected from inside Power BI with no extra programs
// @version         1.2.2
// @author          Analytic Endeavors
// @github          https://github.com/ae-reid-havens
// @include         PBIDesktop.exe
// @architecture    x86-64
// @license         CC-BY-ND-4.0
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Skins & Tools for Power BI

Restyles the Power BI Desktop window (title bar, ribbon, panes, page tabs, status bar, every
view) with one of several skins. The skins never touch your report: the report page and its
visuals look exactly as your report theme says. Two tools can change a report, only when you
press their buttons: Filter map's hide, show, lock and unlock menu, and Interactions' bar.

**Restart Power BI after enabling, updating or changing the settings of this mod.** A window
already open keeps what it was given when it opened; disabling the mod takes the skin and the
tools out of it at once.

Power BI is a trademark of Microsoft. This mod is not affiliated with or endorsed by Microsoft.
Licensed CC BY-ND 4.0: share unchanged copies with credit; modified versions may not be shared.
By Reid Havens, Analytic Endeavors (fingerprint AE-PBT-29A1ED34-2026).

![The 13 skins, one after another](https://raw.githubusercontent.com/ae-reid-havens/skins-tools-for-power-bi/main/media/skins-v1.webp)

![Layout QA and Colors staying on while the skin changes](https://raw.githubusercontent.com/ae-reid-havens/skins-tools-for-power-bi/main/media/skin-tools-v1.webp)

More demos of each tool, and every release: https://github.com/ae-reid-havens/skins-tools-for-power-bi

**Skins:** Blue Hills, Classic Grey, Stronghold, Gunmetal Console, Hazard Suit, Analytic Endeavors, Starship Panel, Phosphor Terminal, Neon Grid, High Contrast Ink, Graph Paper, Midnight Orchid, Arctic Frost.

**How to pick one:** open the **View** tab; a **Skins** group sits at the end of the ribbon.
The status bar also gets a small chip (the skin name and the page's visual count) that opens
the menu, or press **Ctrl+Alt+K**. **Ctrl+Alt+]** and **Ctrl+Alt+[** step through the skins
(on keyboards that type [ and ] with AltGr, such as German or French, use the ribbon or the
menu). The keys leave typing alone: in a text box or the formula bar only the letter keys work.
"None" puts Power BI back exactly as it was. The choice is remembered; changing this mod's
settings resets it to the settings (Stencil blocks and templates are kept). Classic has two
styles, picked beside the gallery while it is chosen: Clean, and Power BI 95 (silver panes, heavy
bevels, a teal desktop around the page and a taskbar with a clock).

**Tools for report authors** (switches at the top of the menu, grouped by job: Plan, Layout,
Check, While editing):
- **Stencil (Ctrl+Alt+S):** a see-through mock-up layer on the page. Draw cards, charts, tables,
  slicers, titles and text on the grid, in report pixels; blocks are saved per report and page.
  The Build menu copies a PowerShell script that writes them into the report project folder as
  real visuals with no fields (new files only); save the report, paste the script, then click Apply external changes in the bar Power BI shows.
  Build also saves a page's blocks as a named template to start other pages from, copies a page's
  visuals (and their groups, which then move together) into the stencil, and turns the blocks into
  card frames drawn as one page background image.
- **Layout QA (Ctrl+Alt+G):** a grid and rulers in report pixels over the page, a label on
  every visual with its position and size, red marks where an edge is 1 to 8 px off a line
  other visuals share, where a gap is almost the page's usual gap, or where a visual runs past
  the page edge, amber marks where an edge is off the grid (what Snap to grid keeps), and a
  strip under the page with the counts, a grid picker and the grid check switch. Hover a label
  for the visual's card: its figures, each finding in words and the value that fixes it; click
  copies x, y, w, h, Shift+click copies the suggested values. Hover a visual for its distances
  in four directions and the edges it shares. The grid, the near-miss range and the amber marks
  are settings below.
- **Filter map (Ctrl+Alt+F):** a badge on every visual with its visual, page and report filters
  and its slicers & cross-filters, spelled out on hover; hover it for the list, hidden and locked
  filters included. A menu (in the strip, and beside "Filters on this visual" in the Filters pane) hides,
  shows, locks or unlocks every visual-level filter on the selected visual, the page or every
  page, by pressing Power BI's own buttons visual by visual. It updates by itself when a filter
  changes; to read each visual's slicers and cross-filters it opens Power BI's filter list for
  that visual out of sight (if that selects the visual, your selection is put back).
- **Theme check (Ctrl+Alt+T):** a badge on every visual that sets formatting differently from
  the report theme ("3 differ from theme"), colors outside the theme included; settings that
  differ from most of the page are grey information, off by default. The card gives the value
  each should have. Reads the open report's project files (see "What it reads and keeps"); for
  a .pbix, compares what each visual draws with most of the page.
- **Colors (Ctrl+Alt+L, or Ctrl+Alt+A):** one badge on every visual whose text, bars, lines or
  points are hard to read against what is behind them (WCAG AA or AAA, picked in its strip), or
  that draws colors off the palette (the report theme's colors when the report is a Power BI
  project, else Power BI's default theme colors, unless you set your own in this mod's settings).
  Hover it for each finding, numbered and outlined on the page; click a swatch to copy its hex.
- **Interactions (Ctrl+Alt+I, on by default; shows in Format > Edit interactions):** the active icon on
  every visual is colored (None red, Filter blue, Highlight green), and a bar above the selected
  visual sets every other visual to None, Filter or Highlight in one click, by pressing each
  visual's own icon (Ctrl+Z undoes them one at a time).

Badge colors mean severity only (red to fix, amber to look at, grey for information), and each
badge leads with its tool's letter (F, T, C).

**Help (Ctrl+Alt+H, or "Help and keys" in the menu):** what every mark the tools draw means,
the extras, every key, and where choices are kept.

**Extras (menu):** a splash when a skin is picked, short notes on visual errors and finished
refreshes, confetti when a refresh finishes, optional sounds; each has a Try button.

**Hidden surprises:** there is an Easter egg in here, Chaos Mode, found with a classic cheat
code, and the Power BI 95 style has a Begin menu. Chaos Mode is a mode: once on, it stays on (an
assistant called Barry, the Power BI 95 look, jokes and click gags) until you leave it by pressing
**Esc twice**, or by typing the code again. A small note at the bottom of the window says so for
as long as it is on. While it is on, each page with a pie or donut chart shows a fake blue crash
screen once; any key or click brings the report back. A plain key only does that, so Delete or
an arrow key never touches the selected visual, while a key with Ctrl, Alt or Win still does its
usual job (Ctrl+S still saves). The Begin menu's programs animate and draw over the window for a few
seconds. Neither changes anything in the report, and their sounds play only when Skin sounds is on
(it is off by default). The "Hidden surprises" setting turns both off.

**What it reads and keeps:**
- **Files:** for a report saved as a Power BI project (.pbip), the mod reads that report's own
  JSON files (every .json under its .Report folder's definition and StaticResources folders),
  with no setting: the .pbip Power BI was started with, then any .pbip it opens or saves (File >
  Open, Recent, Save As), and again after each save, so the tools compare with what was just
  saved. To see which report that is, it watches the names of the files Power BI opens and moves
  into place when it saves; it reads nothing else. "Report project folder" is only a fallback for a window where no .pbip was seen,
  and "Sound file for the refresh confetti" reads that one audio file when Power BI starts and
  when the settings are saved. The mod itself writes no files.
- **Stencil's Build** copies a PowerShell script to the clipboard and changes nothing by itself.
  Only when you paste and run it yourself does it write into the report project folder. Each file
  that exists is copied to a .stencil-backup folder beside the .Report folder before it changes:
  - Building visuals writes new visual files only (a visual file that exists stops the script
    before it writes anything), and for a new page its page.json, with pages.json backed up and
    then updated to list the page.
  - Card frames (Convert to background, or "with frames as background") write one new SVG under
    StaticResources\RegisteredResources, then rewrite definition\report.json (to register it)
    and the page's page.json (to make it the page background). Both are backed up first and
    written through temporary files, so a failed write leaves the originals.
  - With -Replace, on a page that already has a frames image, the old frames image is moved into
    the backups (unless something else in the report uses it), and a script run again with
    -Replace writes its own image again.
- **Browser storage:** choices made inside Power BI (the skin and Classic's style, the switches,
  each tool's settings, Stencil blocks and templates) are kept in the Power BI page's own localStorage under the key `pbid-skins`;
  conveniences for the viewer (folded menu sections, the last help tab, the dock tab, the
  one-time menu hint) under `pbid-skins-ui`. Both are kept per Windows user, in Power BI's own
  WebView2 profile. Changing this mod's settings resets the choices and keeps Stencil blocks and templates.
  Disabling the mod takes the skin and tools out of the open window and leaves both keys for
  next time.

Works with Power BI's light and dark modes. Power BI changes its interface often; if an update
leaves something looking wrong, pick "None", which puts Power BI back exactly as it was.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- skin: off
  $name: Skin
  $description: The skin to use until you pick another inside Power BI. Restart Power BI after changing any of these settings
  $options:
  - off: None (Power BI as set)
  - luna: Blue Hills
  - classic: Classic Grey
  - stronghold: Stronghold
  - gunmetal-console: Gunmetal Console
  - hazard-suit: Hazard Suit
  - ae: Analytic Endeavors
  - starship-panel: Starship Panel
  - phosphor-terminal: Phosphor Terminal
  - grid: Neon Grid
  - ink: High Contrast Ink
  - graphpaper: Graph Paper
  - orchid: Midnight Orchid
  - frost: Arctic Frost
- splash: true
  $name: Splash when a skin is picked
- notes: true
  $name: Notes on visual errors and finished refreshes
- confetti: true
  $name: Confetti when a refresh finishes
- sounds: false
  $name: Skin sounds
  $description: A few synthesized notes when a skin is picked or a refresh finishes. The sounds of the hidden surprises play only while this is on too
- cheer: ""
  $name: Sound file for the refresh confetti
  $description: Full path to an .mp3, .wav, .ogg or .m4a under 2 MB, played with the confetti when a refresh finishes; empty for none. Read when Power BI starts and when the settings are saved. A "with the cheer sound" switch appears under Confetti in the menu
- project: ""
  $name: Report project folder (optional)
  $description: Leave empty; the mod finds the report Power BI has open by itself. Only used for a window where no .pbip was opened or saved; then the .Report folder (or .pbip file) named here is read instead
- visualCount: true
  $name: Visual count in the status bar
- surprises: true
  $name: Hidden surprises
  $description: A hidden Easter egg, found with a classic cheat code, and the Begin menu of Classic's Power BI 95 style. The Easter egg is a mode that stays on until you press Esc twice or type the code again; everything it does is on screen only, and its sounds follow Skin sounds. Off leaves out both, and its key watcher
- keepReportFilterColors: false
  $name: Keep the report's Filters pane colors
  $description: Skins paint the Filters pane like the other panes; turn this on while designing the report's own filter pane formatting
- grid: 8
  $name: Layout QA grid, in report pixels
  $description: The grid Layout QA draws and checks edges against; Power BI's Snap to grid uses 8
- snap: true
  $name: Layout QA marks edges off the grid
  $description: Amber marks on visuals with an edge off the grid; turn off for pages that are not built on a grid
- nearMiss: 8
  $name: Layout QA near-miss range, in report pixels
  $description: An edge this close to a neighbour's matching edge, or a gap this close to the page's usual gap, is marked as off
- palette: ""
  $name: Palette for Colors
  $description: "Hex colors separated by commas or spaces, for example #118DFF, #12239E, #E66C37. Empty means the report theme's colors when the report is a Power BI project, else Power BI's default theme colors"
- tolerance: 12
  $name: Palette tolerance
  $description: How far each color channel (0 to 255) may stray from a palette color and still count as on the palette
*/
// ==/WindhawkModSettings==

// Skins & Tools for Power BI, by Reid Havens, Analytic Endeavors.
// Fingerprint: AE-PBT-29A1ED34-2026. Licensed CC BY-ND 4.0.
// The same fingerprint is in the readme above, the page script (pbt.fingerprint), the base
// stylesheet and the help panel; scripts/build-mod.mjs refuses a build that loses any of them.

#include <windows.h>
#include <unknwn.h>
#include <windhawk_utils.h>
#include <atomic>
#include <memory>
#include <mutex>
#include <string>
#include <vector>
#include <cwctype>

// ---- the skins and tools: generated by scripts/build-mod.mjs from the pbid-tweaks repo ----
static const wchar_t kPayload[] =
LR"PBT((() => {
const PBT_HOST = 'mod';
const __defaults = {"skin":"off","features":{"statusChip":true,"visualCount":true,"notes":true,"confetti":true,"sounds":false,"splash":true,"reportFilterPane":false,"refreshCheer":true,"interactions":true,"surprises":true},"qa":{"grid":8,"nearMiss":8,"snap":true},"lint":{"palette":["#118DFF","#12239E","#E66C37","#6B007B","#E044A7","#744EC2","#D9B300","#D64550","#197278","#1AAB40","#15C6F4","#4092FF","#FFA058","#BE5DC9","#F472D0","#B5A1FF","#C4A200","#FF8080","#00DBBC","#5BD667"],"tolerance":12},"pageSize":null};
const __settings = (typeof __PBT_SETTINGS__ !== 'undefined' && __PBT_SETTINGS__) || {};
const PBT_SETTINGS_KEY = JSON.stringify(__settings);
// The sound file the mod's "cheer" setting names, embedded by the mod as a data URL before this
// runs; else, in a local build made with --cheer only, that build's own sound; else none.
const PBT_CHEER = (typeof __PBT_CHEER__ !== 'undefined' && __PBT_CHEER__) || null;
// The report project files the mod's "project" setting names, read by the mod when Power BI
// starts or the settings change: { root, files: [{ path, text }] }, or none.
const PBT_PROJECT = (typeof __PBT_PROJECT__ !== 'undefined' && __PBT_PROJECT__) || (typeof __PBT_PROJECT_JSON__ === 'string' && window === window.top ? (() => { try { return JSON.parse(__PBT_PROJECT_JSON__); } catch { return null; } })() : null);
let __stored = null, __stencil = null, __templates = null;
try {
  const s = JSON.parse(localStorage.getItem('pbid-skins') || 'null');
  const st = s && s.cfg && s.cfg.stencil;
  if (st && typeof st === 'object' && !Array.isArray(st)) {
    __stencil = {};
    for (const [rk, pages] of Object.entries(st)) {
      if (!pages || typeof pages !== 'object' || Array.isArray(pages)) continue;
      const keep = {};
      for (const [pk, list] of Object.entries(pages)) if (Array.isArray(list)) keep[pk] = list.filter(b => b && typeof b === 'object');
      if (Object.keys(keep).length) __stencil[rk] = keep;
    }
  }
  // Saved Stencil templates are the user's work too: a list of { id, name, blocks } objects.
  const tp = s && s.cfg && s.cfg.stencilTemplates;
  if (Array.isArray(tp)) __templates = tp.filter(t => t && typeof t === 'object' && Array.isArray(t.blocks));
  if (s && s.settings === PBT_SETTINGS_KEY && s.cfg && typeof s.cfg === 'object') __stored = s.cfg;
} catch {}
const PBT_CONFIG = __stored || { ...__defaults, ...__settings, features: { ...__defaults.features, ...(__settings.features || {}) }, qa: { ...__defaults.qa, ...(__settings.qa || {}) }, lint: { ...__defaults.lint, ...(__settings.lint || {}) }, ...(__stencil ? { stencil: __stencil } : {}), ...(__templates && __templates.length ? { stencilTemplates: __templates } : {}) };
const PBT_SKINS = {"luna":{"short":"Blue","name":"Blue Hills","order":1,"splash":["Welcome","Blue sky, green hills."],"confetti":{"shapes":["square"],"colors":["#f25022","#7fba00","#00a4ef","#ffb900"]},"sound":{"wave":"sine","notes":[659,784,988,1319],"step":0.12,"len":0.5},"palette":{"ramp":[["#000000","#ece9d8"],["#141414","#f1efe2"],["#292929","#ffffff"],["#3d3d3d","#d6d2c2"],["#5c5c5c","#aca899"],["#8f8f8f","#716f64"],["#d6d6d6","#1f1f1f"],["#ffffff","#000000"]],"accent":"#316ac5","vars":{"font":"Tahoma, \"Segoe UI\", sans-serif","title-font":"\"Trebuchet MS\", Tahoma, sans-serif","titlebar-bg":"linear-gradient(180deg, #0997ff 0%, #0053ee 9%, #0050ee 40%, #0066ff 86%, #005bff 94%, #003dd7 100%)","titlebar-fg":"#ffffff","titlebar-hover":"rgba(255, 255, 255, .25)","icon":"#ffffff","icon-alt":"#cfe0ff","search-bg":"rgba(255, 255, 255, .92)","icon-fill":"#ffffff","tabs-bg":"#ece9d8","tab-fg":"#000000","tab-active-bg":"#fcfcfe","tab-active-fg":"#000000","tab-hover-bg":"#fff4d1","tab-radius":"3px","file-bg":"linear-gradient(180deg, #3c9a3c, #2d7d2d)","file-fg":"#ffffff","ribbon-bg":"linear-gradient(180deg, #fcfcfe 0%, #f4f3ee 100%)","ribbon-border":"1px solid #919b9c","group-fg":"#215dc6","nav-bg":"linear-gradient(180deg, #5f8ee0, #4a5fc8)","icon-nav":"#ffffff","icon-active":"#ffffff","nav-active-bg":"rgba(0, 20, 90, .35)","pagetabs-bg":"#ece9d8","pagetab-bg":"linear-gradient(180deg, #ffffff, #ecebe5)","pagetab-active-bg":"#fcfcfe","panehead-bg":"linear-gradient(90deg, #ffffff 0%, #c6d3f7 100%)","panehead-fg":"#215dc6","status-bg":"linear-gradient(180deg, #f0eee3, #e3dfcd)","status-fg":"#000000","gutter":"radial-gradient(120% 60% at 30% 108%, #5fa83a 0 40%, transparent 41%), radial-gradient(90% 50% at 85% 112%, #4c9a2a 0 45%, transparent 46%), linear-gradient(180deg, #2f6fd8 0%, #6aa5ee 55%, #bcd9f7 80%, #5fa83a 81%)","ui-bg":"#ffffff","ui-fg":"#000000","ui-border":"#7f9db9","ui-font":"Tahoma, sans-serif","ui-radius":"4px","hover-bg":"#fff4d1","hover-ring":"#ffc83c","hover-radius":"3px","avatar-bg":"rgba(255, 255, 255, .2)","nav-hover-bg":"rgba(255, 255, 255, .2)","wc-glyph":"14px","search-fg":"#3a4a6b"}},"css":"/* Luna: glossy blue title bar, beige chrome, white tabs with the orange hover line. */\nhtml[data-pbt-skin=\"luna\"] div.titlebar .title { font: bold 13px \"Trebuchet MS\", Tahoma, sans-serif !important; text-shadow: 1px 1px #0a1f8a; }\nhtml[data-pbt-skin=\"luna\"] div.titlebar .timestamp { text-shadow: 1px 1px #0a1f8a; }\n\n\n/* XP tabs: raised white tab, orange line along the top when hovered or selected. */\nhtml[data-pbt-skin=\"luna\"] #tablist button[role=\"tab\"] { border: 1px solid transparent !important; }\nhtml[data-pbt-skin=\"luna\"] #tablist button[role=\"tab\"]:hover { box-shadow: inset 0 3px 0 #ffc83c; border-color: #919b9c !important; }\nhtml[data-pbt-skin=\"luna\"] #tablist button[role=\"tab\"][aria-selected=\"true\"] {\n  border-color: #919b9c !important; border-bottom-color: #fcfcfe !important; box-shadow: inset 0 3px 0 #e68b2c;\n}\nhtml[data-pbt-skin=\"luna\"] #tablist button[role=\"tab\"]::after { display: none !important; }\nhtml[data-pbt-skin=\"luna\"] [id=\"Ribbon-file\"] { border-radius: 0 8px 8px 0 !important; font-style: italic; font-weight: bold; }\n\n/* Ribbon: rounded pa)PBT"
LR"PBT(nel; buttons get the XP hover. */\nhtml[data-pbt-skin=\"luna\"] .ribbonContainer .bottomBarContainer { border-radius: 0 0 4px 4px !important; }\n\n/* Explorer style task panes. */\nhtml[data-pbt-skin=\"luna\"] .sidePane, html[data-pbt-skin=\"luna\"] .outspacePane { border-left: 1px solid #7f9db9 !important; }\nhtml[data-pbt-skin=\"luna\"] :is(.pane-header, .outspacePane .paneHeader) { border-radius: 4px 4px 0 0; font-weight: bold; }\n\n/* Page tabs */\nhtml[data-pbt-skin=\"luna\"] .carouselScrollPane li.section, html[data-pbt-skin=\"luna\"] button.section.static.create .thumbnail-container { border: 1px solid #919b9c !important; border-bottom: 0 !important; }\nhtml[data-pbt-skin=\"luna\"] .carouselScrollPane li.section.selected { box-shadow: inset 0 3px 0 #e68b2c; }\n\n/* Scrollbars */\nhtml[data-pbt-skin=\"luna\"] ::-webkit-scrollbar { width: 16px; height: 16px; background: #f4f3ee; }\nhtml[data-pbt-skin=\"luna\"] ::-webkit-scrollbar-thumb { background: linear-gradient(90deg, #c8d6fb, #b2c8f8); border: 1px solid #fff; border-radius: 3px; box-shadow: inset 0 0 0 1px #7f9db9; }\n\n/* Office icons draw their outlines light grey for dark ribbons; darken them on this light one. */\nhtml[data-pbt-skin=\"luna\"] .ribbonContainer svg .OfficeIconColors_m22 { fill: #5f6b7a !important; }\n\n/* The search magnifier on the pale search field. */\nhtml[data-pbt-skin=\"luna\"] div.ribbonSearchOuter svg, html[data-pbt-skin=\"luna\"] div.ribbonSearchOuter svg path { fill: #3a4a6b !important; color: #3a4a6b !important; }\n\n/* Window controls: rounded glossy squares inside Power BI's own button boxes; Close is orange-red. */\nhtml[data-pbt-skin=\"luna\"] {\n  --pbt-wc-inset: 5px 9px; --pbt-wc-radius: 3px; --pbt-wc-fg: #fff;\n  --pbt-wc-face: radial-gradient(circle at 30% 25%, #7fb0ff, #2463e0 60%, #1546b8);\n  --pbt-wc-close-face: radial-gradient(circle at 30% 25%, #f6a28a, #e0491f 60%, #b5320f);\n  --pbt-wc-ring: inset 0 0 0 1px #fff, inset 0 0 3px rgba(0, 0, 0, .45);\n  --pbt-wc-hover-bg: transparent; --pbt-wc-close-hover-bg: transparent;\n}\n"},"classic":{"short":"Classic","name":"Classic Grey","order":2,"splash":["Classic","Please wait while your settings are applied..."],"confetti":{"shapes":["square"],"colors":["#0a246a","#a6caf0","#d4d0c8","#808080","#3a6ea5"]},"sound":{"wave":"square","notes":[880,660],"step":0.09,"len":0.15},"palette":{"ramp":[["#000000","#d4d0c8"],["#141414","#d4d0c8"],["#292929","#ffffff"],["#3d3d3d","#c0c0c0"],["#5c5c5c","#808080"],["#8f8f8f","#404040"],["#d6d6d6","#000000"],["#ffffff","#000000"]],"accent":"#0a246a","vars":{"font":"Tahoma, \"Microsoft Sans Serif\", sans-serif","title-font":"Tahoma, sans-serif","titlebar-bg":"linear-gradient(90deg, #0a246a 0%, #a6caf0 100%)","titlebar-fg":"#ffffff","titlebar-hover":"rgba(255, 255, 255, .2)","icon":"#ffffff","icon-alt":"#d4d0c8","search-bg":"#ffffff","icon-fill":"#ffffff","tabs-bg":"#d4d0c8","tab-fg":"#000000","tab-active-bg":"#d4d0c8","tab-active-fg":"#000000","tab-hover-bg":"#d4d0c8","file-bg":"#d4d0c8","file-fg":"#000000","ribbon-bg":"#d4d0c8","group-fg":"#000000","nav-bg":"#d4d0c8","icon-nav":"#000000","icon-active":"#000000","nav-active-bg":"#ffffff","pagetabs-bg":"#d4d0c8","pagetab-bg":"#d4d0c8","pagetab-active-bg":"#d4d0c8","panehead-bg":"linear-gradient(90deg, #0a246a, #4a7cc7)","panehead-fg":"#ffffff","status-bg":"#d4d0c8","status-fg":"#000000","gutter":"#3a6ea5","ui-bg":"#d4d0c8","ui-fg":"#000000","ui-border":"#808080","ui-font":"Tahoma, sans-serif","ui-radius":"0","ui-shadow":"inset -1px -1px #404040, inset 1px 1px #ffffff, inset -2px -2px #808080, inset 2px 2px #dfdfdf, 2px 2px 0 rgba(0,0,0,.35)","avatar-bg":"transparent","wc-glyph":"12px","hover-radius":"0"}},"css":"/* Classic: grey 3D bevels everywhere, navy gradient title bar, square corners. */\nhtml[data-pbt-skin=\"classic\"] {\n  --pbt-outset: inset -1px -1px #404040, inset 1px 1px #ffffff, inset -2px -2px #808080, inset 2px 2px #dfdfdf;\n  --pbt-inset: inset -1px -1px #ffffff, inset 1px )PBT"
LR"PBT(1px #808080, inset -2px -2px #dfdfdf, inset 2px 2px #404040;\n}\nhtml[data-pbt-skin=\"classic\"] div.titlebar .title { font-weight: bold !important; }\nhtml[data-pbt-skin=\"classic\"] * { border-radius: 0 !important; }\nhtml[data-pbt-skin=\"classic\"] .displayArea * { border-radius: revert !important; }\n\n\n/* Tabs as property-sheet tabs on one baseline, File included. The open tab is bold, a shade\n   lighter, and open at the bottom into the panel instead of jumping up. */\nhtml[data-pbt-skin=\"classic\"] :is(#tablist button[role=\"tab\"], [id=\"Ribbon-file\"]) { box-shadow: inset 1px 1px #fff, inset -1px 0 #404040, inset -2px 0 #808080 !important; margin-top: 3px !important; height: 28px !important; }\nhtml[data-pbt-skin=\"classic\"] #tablist button[role=\"tab\"][aria-selected=\"true\"] { font-weight: bold; background: #e0ddd6 !important; box-shadow: inset 1px 1px #fff, inset -1px 0 #404040, inset -2px 0 #808080, 0 2px 0 #e0ddd6 !important; }\nhtml[data-pbt-skin=\"classic\"] #tablist button[role=\"tab\"]::after { display: none !important; }\n\n/* Ribbon panel and buttons bevel. */\nhtml[data-pbt-skin=\"classic\"] .ribbonContainer .bottomBarContainer { box-shadow: var(--pbt-outset); }\nhtml[data-pbt-skin=\"classic\"][data-pbt-skin] .ribbonContainer .bottomBarContainer button:not(:disabled):not(.is-disabled):not(.pbt-rbtn):hover { box-shadow: var(--pbt-outset) !important; background: #d4d0c8 !important; }\nhtml[data-pbt-skin=\"classic\"] .ribbonContainer .bottomBarContainer button:not(:disabled):active { box-shadow: var(--pbt-inset); }\n\n/* Panes are sunken wells; inputs are white wells. */\nhtml[data-pbt-skin=\"classic\"] :is(.sidePane, .outspacePane) { box-shadow: var(--pbt-outset); }\nhtml[data-pbt-skin=\"classic\"] :is(.sidePane, .outspacePane, .ribbonSearchOuter) input { box-shadow: var(--pbt-inset); background: #fff !important; }\nhtml[data-pbt-skin=\"classic\"] :is(.pane-header, .outspacePane .paneHeader) { font-weight: bold; }\nhtml[data-pbt-skin=\"classic\"] view-navigator button[aria-selected=\"true\"] { box-shadow: var(--pbt-inset); }\n\n/* Page tabs */\nhtml[data-pbt-skin=\"classic\"] .carouselScrollPane li.section, html[data-pbt-skin=\"classic\"] button.section.static.create .thumbnail-container { box-shadow: inset 1px 0 #fff, inset -1px 0 #404040, inset 0 -1px #fff; }\nhtml[data-pbt-skin=\"classic\"] .carouselScrollPane li.section.selected { box-shadow: inset 1px 0 #fff, inset -1px 0 #404040, inset 0 -2px #d4d0c8; }\n\n/* Status bar: sunken panels. */\nhtml[data-pbt-skin=\"classic\"] section.statusBar > div { box-shadow: inset -1px -1px #fff, inset 1px 1px #808080; margin: 1px; }\n\n/* Our menu and toasts get the bevel too. */\nhtml[data-pbt-skin=\"classic\"] .pbt-menu button:hover { background: #0a246a !important; color: #fff; --pbt-ui-fg: #fff; --pbt-ui-bg: #0a246a; --pbt-accent: #fff; }\nhtml[data-pbt-skin=\"classic\"] .pbt-toast { border: 0; border-left: 0; }\n\nhtml[data-pbt-skin=\"classic\"] ::-webkit-scrollbar { width: 16px; height: 16px; background: repeating-conic-gradient(#d4d0c8 0 25%, #fff 0 50%) 0 0 / 2px 2px; }\nhtml[data-pbt-skin=\"classic\"] ::-webkit-scrollbar-thumb { background: #d4d0c8; box-shadow: var(--pbt-outset); }\n\n/* The title gradient ends light blue; the account sits on that end, so it takes dark ink. */\nhtml[data-pbt-skin=\"classic\"] div.titlebar .accountNameLabel { color: #0a246a !important; }\nhtml[data-pbt-skin=\"classic\"] titlebar account .imageAvatarIcon { color: #0a246a !important; }\n\n/* Office icons draw their outlines light grey for dark ribbons; darken them on this light one. */\nhtml[data-pbt-skin=\"classic\"] .ribbonContainer svg .OfficeIconColors_m22 { fill: #404040 !important; }\n\n/* Share is a square raised button like the window controls. */\nhtml[data-pbt-skin=\"classic\"] .farPeripheralControls button { box-shadow: var(--pbt-outset) !important; border: 0 !important; background: #d4d0c8 !important; }\n/* Search fields: one sunken well around the wh)PBT"
LR"PBT(ole field, not a second one around the text. */\nhtml[data-pbt-skin=\"classic\"] :is(.outspacePane, .ribbonSearchOuter) input { box-shadow: none !important; }\nhtml[data-pbt-skin=\"classic\"]:not(.pbt-report-filters) .outspacePane search-box-modern { box-shadow: var(--pbt-inset) !important; background-color: #fff !important; }\n\nhtml[data-pbt-skin=\"classic\"] .sidePane input { box-shadow: none !important; }\n\n/* Window controls: small bevelled grey buttons inside Power BI's own button boxes. */\nhtml[data-pbt-skin=\"classic\"] {\n  --pbt-wc-inset: 6px 5px; --pbt-wc-radius: 0; --pbt-wc-fg: #000; --pbt-wc-close-hover-fg: #000;\n  --pbt-wc-face: #d4d0c8; --pbt-wc-ring: inset -1px -1px #404040, inset 1px 1px #ffffff, inset -2px -2px #808080, inset 2px 2px #dfdfdf;\n  --pbt-wc-hover-bg: transparent; --pbt-wc-close-hover-bg: transparent;\n}\nhtml[data-pbt-skin=\"classic\"] .windowControls button:hover::before { filter: none; }\nhtml[data-pbt-skin=\"classic\"] .windowControls button:active::before { box-shadow: inset -1px -1px #ffffff, inset 1px 1px #808080, inset -2px -2px #dfdfdf, inset 2px 2px #404040; }\n"},"stronghold":{"short":"Stronghold","name":"Stronghold","order":3,"splash":["Stronghold Ready","The war room is open. Bring data, chieftain."],"confetti":{"shapes":["coin"],"colors":["#f2c94c","#d4a531","#b8860b","#ffe08a"]},"sound":{"wave":"triangle","notes":[110,110,147,98],"step":0.16,"len":0.25},"palette":{"ramp":[["#000000","#0e0b09"],["#141414","#1b1612"],["#292929","#2a221b"],["#3d3d3d","#40342a"],["#5c5c5c","#5e4c3b"],["#8f8f8f","#a08a6a"],["#d6d6d6","#efdcae"],["#ffffff","#fff1cc"]],"accent":"#b3121b","vars":{"font":"\"Segoe UI\", sans-serif","title-font":"\"Book Antiqua\", \"Palatino Linotype\", Georgia, serif","titlebar-bg":"radial-gradient(circle at 50% 50%, #f2d27a 0 1.4px, #6b4a14 2px, transparent 2.6px) 12px calc(100% - 1px) / 48px 7px repeat-x, radial-gradient(circle at 50% 50%, #f2d27a 0 1.4px, #6b4a14 2px, transparent 2.6px) 12px 1px / 48px 7px repeat-x, linear-gradient(180deg, #9a6a1c 0 1px, #3a332d 1px, #24201c 50%, #171411 calc(100% - 2px), #9a6a1c calc(100% - 2px))","titlebar-fg":"#f2d27a","titlebar-hover":"rgba(242, 210, 122, .2)","icon":"#f2d27a","icon-alt":"#a08a6a","avatar-bg":"rgba(0, 0, 0, .35)","search-bg":"rgba(14, 11, 9, .75)","tabs-bg":"linear-gradient(180deg, rgba(0,0,0,.15), rgba(0,0,0,.45)), repeating-linear-gradient(0deg, #15110e 0 2px, transparent 2px 18px), repeating-linear-gradient(90deg, #15110e 0 2px, transparent 2px 46px) 0 0 / 46px 36px, linear-gradient(135deg, #4a423a, #332d28)","tab-fg":"#e6cf96","tab-active-bg":"linear-gradient(180deg, #c4161f 0%, #8e0d14 70%, #5e070c 100%)","tab-active-fg":"#ffe9a8","tab-hover-bg":"rgba(242, 210, 122, .16)","file-bg":"linear-gradient(180deg, #f2d27a, #b98a2e 50%, #7a5414)","file-fg":"#2a1606","ribbon-bg":"radial-gradient(circle at 25% 20%, rgba(255, 220, 150, .06), transparent 45%), repeating-linear-gradient(45deg, rgba(0, 0, 0, .12) 0 2px, transparent 2px 6px), linear-gradient(180deg, #33281f, #221a14)","group-fg":"#e0b44a","hover-bg":"rgba(179, 18, 27, .28)","hover-ring":"#e0b44a","hover-radius":"2px","nav-bg":"linear-gradient(90deg, #171411, #24201c 60%, #171411), linear-gradient(#9a6a1c, #9a6a1c)","icon-nav":"#cdb47a","icon-active":"#ffe9a8","nav-active-bg":"linear-gradient(90deg, rgba(179, 18, 27, .7), rgba(179, 18, 27, .2))","nav-hover-bg":"rgba(242, 210, 122, .14)","pagetabs-bg":"linear-gradient(180deg, #24201c, #171411)","pagetab-bg":"linear-gradient(180deg, #40342a, #2a221b)","pagetab-fg":"#e6cf96","pagetab-active-bg":"linear-gradient(180deg, #c4161f, #6e0a10)","pagetab-active-fg":"#ffe9a8","panehead-bg":"linear-gradient(180deg, #8e0d14, #4a060a)","panehead-fg":"#f2d27a","status-bg":"radial-gradient(circle at 50% 50%, #e0b44a 0 1.2px, #5a3a0c 1.8px, transparent 2.4px) 8px 0 / 96px 6px repeat-x, linear-gradient(180deg, #3a332d, #171411)","status-fg":"#cdb47a","ui-bg":"radial-gradient(120% 120% at 50% 40%, #f6e8c3 0%, #ead5a2 )PBT"
LR"PBT(60%, #cfae72 100%)","ui-fg":"#3a2410","ui-border":"#8a5a14","ui-font":"\"Segoe UI\", sans-serif","ui-radius":"3px","ui-shadow":"0 0 0 1px #3a2410, 0 10px 28px rgba(0, 0, 0, .6)","wc-glyph":"12px","share-bg":"linear-gradient(180deg, #3a332d, #24201c)"}},"css":"/* Stronghold: an iron band with bronze rivets across the top, a stone wall behind red banner\n   tabs, the ribbon in a bronze frame with spiked corner bosses, stitched leather panes and\n   parchment for every message. All drawn with gradients; no images. */\nhtml[data-pbt-skin=\"stronghold\"] {\n  --pbt-bronze: linear-gradient(180deg, #f7dd8c 0%, #c1913a 30%, #6b4a14 52%, #a8782a 75%, #e8c36a 100%);\n  /* A boss: bronze stud with a dark ring, and two tusks (curved spikes) pointing out of the corner. */\n  --pbt-boss: radial-gradient(circle, #fff0b8 0 2px, #d9a640 3px 5px, #5a3a0c 6px 7px, transparent 7.5px);\n}\n\n/* Type: the chrome reads in Segoe UI (palette.vars font), so ribbon captions, field names and\n   filter labels stay easy to scan; the display face is kept for headings only: the title, the\n   ribbon tabs and File, the pane headers and the menu's section names. The base font rule reads\n   --pbt-font on each element, so setting it on a heading reaches every span inside. */\nhtml[data-pbt-skin=\"stronghold\"] :is(div.titlebar .title, #tablist, [id=\"Ribbon-file\"], .pane-header, .outspacePane .paneHeader, .pbt-menu h4) { --pbt-font: var(--pbt-title-font); font-family: var(--pbt-title-font); }\n\n/* Title: engraved gold, crossed blades before the file name. */\nhtml[data-pbt-skin=\"stronghold\"] div.titlebar .title { font: bold 15px \"Book Antiqua\", \"Palatino Linotype\", serif !important; letter-spacing: .05em; text-shadow: 0 1px 0 #000, 0 -1px 0 rgba(255, 240, 184, .25), 0 0 8px rgba(242, 201, 76, .35); }\nhtml[data-pbt-skin=\"stronghold\"] div.titlebar .title::before { content: \"\\2694\"; margin-right: 7px; color: #e0b44a; text-shadow: 0 1px 0 #000; font-family: \"Segoe UI Symbol\", serif; }\n\n\n/* Banner tabs: the chosen tab hangs as a red pennant with a gold edge and a cut point. */\nhtml[data-pbt-skin=\"stronghold\"] #tablist button[role=\"tab\"] { text-shadow: 0 1px 0 #000; letter-spacing: .04em; font-variant: small-caps; font-size: 15px !important; }\nhtml[data-pbt-skin=\"stronghold\"] #tablist button[role=\"tab\"]::after { display: none !important; }\nhtml[data-pbt-skin=\"stronghold\"] #tablist button[role=\"tab\"][aria-selected=\"true\"] {\n  clip-path: polygon(0 0, 100% 0, 100% 78%, 50% 100%, 0 78%); padding-bottom: 4px;\n  box-shadow: inset 2px 0 0 #e0b44a, inset -2px 0 0 #e0b44a, inset 0 2px 0 #f7dd8c;\n}\nhtml[data-pbt-skin=\"stronghold\"] [id=\"Ribbon-file\"] { font-weight: bold; text-shadow: 0 1px 0 rgba(255, 240, 184, .6); box-shadow: inset 0 0 0 1px #3a2408; clip-path: polygon(0 0, calc(100% - 10px) 0, 100% 50%, calc(100% - 10px) 100%, 0 100%); padding-right: 10px; }\n\n/* Ribbon: bronze frame, dark inner edge, a boss at each corner. */\nhtml[data-pbt-skin=\"stronghold\"] .ribbonContainer .bottomBarContainer {\n  border: 3px solid transparent !important; border-image: var(--pbt-bronze) 1 !important; border-radius: 0 !important;\n  box-shadow: inset 0 0 0 1px #000, inset 0 0 18px rgba(0, 0, 0, .75), 0 3px 8px rgba(0, 0, 0, .7);\n  background:\n    var(--pbt-boss) 0 0 / 16px 16px no-repeat,\n    var(--pbt-boss) 100% 0 / 16px 16px no-repeat,\n    var(--pbt-boss) 0 100% / 16px 16px no-repeat,\n    var(--pbt-boss) 100% 100% / 16px 16px no-repeat,\n    var(--pbt-ribbon-bg) !important;\n}\nhtml[data-pbt-skin=\"stronghold\"] #RibbonModeToggle:hover { box-shadow: inset 0 0 0 1px #e0b44a; }\nhtml[data-pbt-skin=\"stronghold\"] .ribbonContainer [role=\"group\"] label[class*=\"groupLabel\"] { letter-spacing: .03em; text-shadow: 0 1px 0 #000; }\n/* Group dividers become thin bronze posts. */\nhtml[data-pbt-skin=\"stronghold\"] .ribbonContainer [class*=\"groupContainer\"] + [class*=\"groupContainer\"] { border-left: 1px solid #6b4a14 !importa)PBT"
LR"PBT(nt; box-shadow: inset 1px 0 0 rgba(255, 240, 184, .12); }\n\n/* Panes: leather with a stitched edge behind a bronze rail. */\nhtml[data-pbt-skin=\"stronghold\"] .sidePane { background: repeating-linear-gradient(45deg, rgba(0, 0, 0, .1) 0 2px, transparent 2px 6px), linear-gradient(180deg, #2a221b, #1b1612) !important; }\nhtml[data-pbt-skin=\"stronghold\"] :is(.sidePane, .outspacePane) { border-left: 3px solid transparent !important; border-image: var(--pbt-bronze) 1 !important; }\nhtml[data-pbt-skin=\"stronghold\"] .sidePane .pane-container { outline: 1px dashed rgba(224, 180, 74, .35); outline-offset: -5px; }\nhtml[data-pbt-skin=\"stronghold\"] :is(.pane-header, .outspacePane .paneHeader) { border-bottom: 2px solid #e0b44a; font-variant: small-caps; letter-spacing: .06em; text-shadow: 0 1px 0 #000; }\nhtml[data-pbt-skin=\"stronghold\"] view-navigator { border-right: 3px solid transparent; border-image: var(--pbt-bronze) 1; }\n\n/* Page tabs: small red shields with a gold rim. */\nhtml[data-pbt-skin=\"stronghold\"] .carouselScrollPane li.section, html[data-pbt-skin=\"stronghold\"] button.section.static.create .thumbnail-container { border: 1px solid #6b4a14 !important; border-top: 0 !important; border-radius: 0 0 10px 10px !important; }\nhtml[data-pbt-skin=\"stronghold\"] .carouselScrollPane li.section.selected { border-color: #e0b44a !important; box-shadow: inset 0 0 0 1px #3a2408; }\nhtml[data-pbt-skin=\"stronghold\"] button.section.static.create { color: #f2d27a !important; }\nhtml[data-pbt-skin=\"stronghold\"] button.section.static.create .thumbnail-container { box-shadow: none; }\n\n/* Parchment for menus and toasts, with a wax seal on toasts; the splash is a scroll. */\nhtml[data-pbt-skin=\"stronghold\"] .pbt-toast { border: 1px solid #8a5a14; border-left: 1px solid #8a5a14; padding-left: 40px; position: relative; }\nhtml[data-pbt-skin=\"stronghold\"] .pbt-toast::before { content: \"\"; position: absolute; left: 10px; top: 50%; width: 20px; height: 20px; margin-top: -10px; border-radius: 50%; background: radial-gradient(circle at 40% 35%, #e04a4a, #9e1015 60%, #5e070c); box-shadow: 0 1px 2px rgba(0, 0, 0, .5); }\nhtml[data-pbt-skin=\"stronghold\"] .pbt-menu button:hover { background: rgba(179, 18, 27, .18) !important; }\nhtml[data-pbt-skin=\"stronghold\"] .pbt-menu h4 { color: #8e0d14; opacity: 1; font-variant: small-caps; }\nhtml[data-pbt-skin=\"stronghold\"] .pbt-splash > div { border: 0; color: #3a2410; padding: 34px 64px; box-shadow: 0 0 0 1px #3a2410, 0 -10px 0 -4px #8a5a14, 0 10px 0 -4px #8a5a14, 0 18px 40px rgba(0, 0, 0, .7); }\n\nhtml[data-pbt-skin=\"stronghold\"] ::-webkit-scrollbar { width: 14px; height: 14px; background: #1b1612; }\nhtml[data-pbt-skin=\"stronghold\"] ::-webkit-scrollbar-thumb { background: var(--pbt-bronze); border-radius: 7px; border: 3px solid #1b1612; }\n\n/* The Filters pane matches the side panes: leather behind a stitched inner frame. */\nhtml[data-pbt-skin=\"stronghold\"]:not(.pbt-report-filters) .outspacePane {\n  background: repeating-linear-gradient(45deg, rgba(0, 0, 0, .1) 0 2px, transparent 2px 6px), linear-gradient(180deg, #2a221b, #1b1612) !important;\n  outline: 1px dashed rgba(224, 180, 74, .35); outline-offset: -5px;\n}\nhtml[data-pbt-skin=\"stronghold\"] .farPeripheralControls button { border-color: #6b4a14 !important; }\n\n/* Window controls: round bronze rivets inside Power BI's own button boxes; Close glows red. */\nhtml[data-pbt-skin=\"stronghold\"] {\n  --pbt-wc-inset: 5px 10px; --pbt-wc-radius: 50%; --pbt-wc-fg: #2a1606; --pbt-wc-close-fg: #ffe9a8; --pbt-wc-close-hover-fg: #ffe9a8;\n  --pbt-wc-face: radial-gradient(circle at 38% 30%, #fff0b8, #d9a640 45%, #7a5414 80%, #3a2408);\n  --pbt-wc-close-face: radial-gradient(circle at 38% 30%, #ff9a8a, #c4161f 45%, #6e0a10 85%);\n  --pbt-wc-ring: 0 0 0 1px #000, 0 1px 2px rgba(0, 0, 0, .8);\n  --pbt-wc-hover-bg: transparent; --pbt-wc-close-hover-bg: transparent;\n}\nhtml[data-pbt-skin=\"stronghold\"] .windowControls )PBT"
LR"PBT(button:hover::before { box-shadow: 0 0 0 1px #000, 0 0 8px rgba(255, 220, 120, .8); }\nhtml[data-pbt-skin=\"stronghold\"] .windowControls button.closeButton:hover::before { box-shadow: 0 0 0 1px #000, 0 0 10px #ff3b30; }\n"},"gunmetal-console":{"short":"Gunmetal","name":"Gunmetal Console","order":4,"splash":["Console Online","Systems nominal. Awaiting orders, commander."],"confetti":{"shapes":["crystal"],"colors":["#ffb020","#8fb2cc","#d4dee5","#ffd27a"]},"sound":{"wave":"sawtooth","notes":[1200,1600,1200,2000],"step":0.06,"len":0.07},"palette":{"ramp":[["#000000","#05090b"],["#141414","#0b1419"],["#292929","#13212a"],["#3d3d3d","#1d3240"],["#5c5c5c","#2e4d5f"],["#8f8f8f","#7f97a6"],["#d6d6d6","#d4dee5"],["#ffffff","#f0f5f8"]],"accent":"#ffb020","vars":{"font":"Bahnschrift, \"Agency FB\", \"Segoe UI\", sans-serif","title-font":"\"Agency FB\", Bahnschrift, sans-serif","titlebar-bg":"linear-gradient(180deg, #2a3740 0%, #141c21 55%, #0b1014 100%)","titlebar-fg":"#d4dee5","titlebar-hover":"rgba(143, 178, 204, .2)","icon":"#c4d3dc","icon-alt":"#7f97a6","search-bg":"rgba(5, 9, 11, .8)","tabs-bg":"linear-gradient(180deg, #1a252c, #0f171c)","tab-fg":"#9fb4c2","tab-active-bg":"linear-gradient(180deg, rgba(255, 176, 32, .22), rgba(255, 176, 32, .04))","tab-active-fg":"#ffc04d","tab-hover-bg":"rgba(143, 178, 204, .16)","file-bg":"linear-gradient(180deg, #3b5568 0%, #253747 55%, #182531 100%)","file-fg":"#ffd27a","ribbon-bg":"linear-gradient(180deg, #24323b 0%, #18232a 50%, #121a1f 100%)","ribbon-border":"1px solid #3d5666","group-fg":"#9ec3db","nav-bg":"linear-gradient(90deg, #121a1f, #1d2a32)","icon-nav":"#9fb4c2","icon-active":"#ffc04d","nav-active-bg":"rgba(255, 176, 32, .14)","pagetabs-bg":"#0b1419","pagetab-bg":"linear-gradient(180deg, #1d3240, #13212a)","pagetab-fg":"#9fb4c2","pagetab-active-bg":"linear-gradient(180deg, rgba(255, 176, 32, .28), #13212a)","pagetab-active-fg":"#ffc04d","panehead-bg":"linear-gradient(90deg, #1d3240, transparent)","panehead-fg":"#ffc04d","status-bg":"linear-gradient(180deg, #1a252c, #0b1014)","status-fg":"#ffc04d","gutter":"radial-gradient(1px 1px at 20px 30px, #fff, transparent) 0 0 / 220px 200px, radial-gradient(1px 1px at 110px 80px, #9ff7ff, transparent) 0 0 / 220px 200px, radial-gradient(1.5px 1.5px at 160px 20px, #fff, transparent) 0 0 / 220px 200px, radial-gradient(1px 1px at 60px 140px, #cfe, transparent) 0 0 / 220px 200px, radial-gradient(1px 1px at 190px 170px, #fff, transparent) 0 0 / 220px 200px, radial-gradient(1px 1px at 37px 95px, #fff, transparent) 0 0 / 310px 260px, radial-gradient(120% 100% at 80% 10%, #10243a, #03060a 70%)","ui-bg":"#0f171c","ui-fg":"#d4dee5","ui-border":"#3d5666","ui-font":"Bahnschrift, \"Segoe UI\", sans-serif","ui-radius":"0","hover-bg":"rgba(143, 178, 204, .14)","hover-ring":"rgba(255, 176, 32, .7)","hover-radius":"0","avatar-bg":"rgba(255, 176, 32, .1)","panehead-case":"uppercase","panehead-tracking":".1em"}},"css":"/* Gunmetal Console: blue-grey steel plates with chamfered corners and rivets, pale steel text,\n   amber readouts for whatever is lit or chosen, scanlines on the panes, a starfield behind the page. */\nhtml[data-pbt-skin=\"gunmetal-console\"] { --pbt-chamfer: polygon(10px 0, calc(100% - 10px) 0, 100% 10px, 100% calc(100% - 10px), calc(100% - 10px) 100%, 10px 100%, 0 calc(100% - 10px), 0 10px); }\nhtml[data-pbt-skin=\"gunmetal-console\"] div.titlebar { border-bottom: 1px solid #5f7f94; box-shadow: 0 1px 8px rgba(143, 178, 204, .3); }\nhtml[data-pbt-skin=\"gunmetal-console\"] div.titlebar .title { font: 600 16px \"Agency FB\", Bahnschrift, sans-serif !important; letter-spacing: .12em; text-transform: uppercase; text-shadow: 0 0 6px rgba(143, 178, 204, .6); }\n\n\nhtml[data-pbt-skin=\"gunmetal-console\"] #tablist button[role=\"tab\"] { text-transform: uppercase; letter-spacing: .08em; font-size: 12px !important; }\nhtml[data-pbt-skin=\"gunmetal-console\"] #tablist button[role=\"tab\"][aria-selected=\"true\"] { text-shadow: 0 0 6px rgba(255,)PBT"
LR"PBT( 176, 32, .6); box-shadow: inset 0 -2px 0 #ffb020; }\nhtml[data-pbt-skin=\"gunmetal-console\"] #tablist button[role=\"tab\"]::after { display: none !important; }\n/* File: a lit console key, chamfered, with an amber edge and a sweep of light on hover. */\nhtml[data-pbt-skin=\"gunmetal-console\"] [id=\"Ribbon-file\"] {\n  clip-path: polygon(8px 0, 100% 0, 100% calc(100% - 8px), calc(100% - 8px) 100%, 0 100%, 0 8px);\n  box-shadow: inset 0 0 0 1px #ffb020, inset 0 0 12px rgba(255, 176, 32, .35) !important;\n  text-transform: uppercase; letter-spacing: .14em; font-weight: 600;\n  text-shadow: 0 0 6px rgba(255, 176, 32, .7);\n}\nhtml[data-pbt-skin=\"gunmetal-console\"] [id=\"Ribbon-file\"]:hover {\n  background: linear-gradient(100deg, transparent 30%, rgba(255, 228, 176, .3) 50%, transparent 70%) -100% 0 / 200% 100% no-repeat, var(--pbt-file-bg) !important;\n  animation: pbt-gunmetal-console-sweep .6s ease-out;\n}\n@keyframes pbt-gunmetal-console-sweep { from { background-position: -100% 0, 0 0; } to { background-position: 100% 0, 0 0; } }\n\n/* Ribbon plate: chamfered, riveted, with a thin cyan edge light. */\nhtml[data-pbt-skin=\"gunmetal-console\"] .ribbonContainer .bottomBarContainer {\n  clip-path: var(--pbt-chamfer);\n  background-image:\n    radial-gradient(circle at 16px 7px, #8fa7b3 0 1.5px, #2a3740 2.5px, transparent 3px),\n    radial-gradient(circle at calc(100% - 16px) 7px, #8fa7b3 0 1.5px, #2a3740 2.5px, transparent 3px),\n    radial-gradient(circle at 16px calc(100% - 7px), #8fa7b3 0 1.5px, #2a3740 2.5px, transparent 3px),\n    radial-gradient(circle at calc(100% - 16px) calc(100% - 7px), #8fa7b3 0 1.5px, #2a3740 2.5px, transparent 3px),\n    linear-gradient(90deg, rgba(90, 209, 255, .35), transparent 3px, transparent calc(100% - 3px), rgba(90, 209, 255, .35)),\n    var(--pbt-ribbon-bg) !important;\n}\nhtml[data-pbt-skin=\"gunmetal-console\"] .ribbonContainer [role=\"group\"] label[class*=\"groupLabel\"] { text-transform: uppercase; letter-spacing: .1em; font-size: 10px !important; }\n\n/* Panes: scanlines and a steel edge. The Filters pane keeps its colors: the report sets them. */\nhtml[data-pbt-skin=\"gunmetal-console\"] .sidePane { background-image: repeating-linear-gradient(180deg, rgba(0, 0, 0, .22) 0 1px, transparent 1px 3px) !important; }\nhtml[data-pbt-skin=\"gunmetal-console\"] :is(.sidePane, .outspacePane) { border-left: 1px solid #2e4d5f !important; box-shadow: inset 1px 0 0 rgba(90, 209, 255, .25); }\nhtml[data-pbt-skin=\"gunmetal-console\"] :is(.pane-header, .outspacePane .paneHeader) { text-transform: uppercase; letter-spacing: .1em; border-bottom: 1px solid #2e4d5f; }\nhtml[data-pbt-skin=\"gunmetal-console\"] .carouselScrollPane li.section, html[data-pbt-skin=\"gunmetal-console\"] button.section.static.create .thumbnail-container { clip-path: polygon(8px 0, calc(100% - 8px) 0, 100% 100%, 0 100%); padding: 0 6px; }\nhtml[data-pbt-skin=\"gunmetal-console\"] section.statusBar { border-top: 1px solid #2e4d5f; text-shadow: 0 0 4px rgba(255, 176, 32, .5); }\n\nhtml[data-pbt-skin=\"gunmetal-console\"] ::-webkit-scrollbar { width: 12px; height: 12px; background: #0b1419; }\nhtml[data-pbt-skin=\"gunmetal-console\"] ::-webkit-scrollbar-thumb { background: linear-gradient(90deg, #2e4d5f, #3d6a80); border: 2px solid #0b1419; }\nhtml[data-pbt-skin=\"gunmetal-console\"] .pbt-toast, html[data-pbt-skin=\"gunmetal-console\"] .pbt-menu { clip-path: var(--pbt-chamfer); }\n\n/* The Filters pane matches the side panes. */\nhtml[data-pbt-skin=\"gunmetal-console\"]:not(.pbt-report-filters) .outspacePane { background-image: repeating-linear-gradient(180deg, rgba(0, 0, 0, .22) 0 1px, transparent 1px 3px) !important; }\n/* New page: as tall as the page tabs, with the + centered. */\nhtml[data-pbt-skin=\"gunmetal-console\"] button.section.static.create .thumbnail-container { height: 35px !important; display: flex !important; align-items: center; justify-content: center; }\n\n/* The + centered in the New page button)PBT"
LR"PBT(. */\nhtml[data-pbt-skin=\"gunmetal-console\"] button.section.static.create .thumbnail { display: flex !important; align-items: center; justify-content: center; height: 100% !important; }\nhtml[data-pbt-skin=\"gunmetal-console\"] button.section.static.create svg-use-symbol { display: flex !important; align-items: center; justify-content: center; height: 100% !important; }\n\n/* Window controls: chamfered gunmetal plates inside Power BI's own button boxes. */\nhtml[data-pbt-skin=\"gunmetal-console\"] {\n  --pbt-wc-inset: 5px 4px; --pbt-wc-radius: 0; --pbt-wc-fg: #d4dee5; --pbt-wc-close-fg: #ffb3b3; --pbt-wc-close-hover-fg: #ffd6d6;\n  --pbt-wc-face: linear-gradient(180deg, #2e3d47, #141c21); --pbt-wc-close-face: linear-gradient(180deg, #7a1f1f, #3d0d0d);\n}\nhtml[data-pbt-skin=\"gunmetal-console\"] .windowControls button::before { clip-path: polygon(6px 0, 100% 0, 100% calc(100% - 6px), calc(100% - 6px) 100%, 0 100%, 0 6px); }\nhtml[data-pbt-skin=\"gunmetal-console\"] .windowControls button:hover::before { filter: brightness(1.45); }\n"},"hazard-suit":{"short":"Hazard","name":"Hazard Suit","order":5,"splash":["Hazard Suit Online","Biomonitor active. Data integrity nominal."],"confetti":{"shapes":["lambda"],"colors":["#ff9d00","#ffb43a","#ffd27a","#ff6a00"]},"sound":{"wave":"square","notes":[523,784],"step":0.14,"len":0.12},"palette":{"ramp":[["#000000","#070604"],["#141414","#0f0c07"],["#292929","#19140b"],["#3d3d3d","#2a2111"],["#5c5c5c","#4a3a1c"],["#8f8f8f","#c8963c"],["#d6d6d6","#ffb43a"],["#ffffff","#ffd27a"]],"accent":"#ff9d00","vars":{"font":"Bahnschrift, \"Trebuchet MS\", sans-serif","title-font":"Bahnschrift, \"Trebuchet MS\", sans-serif","titlebar-bg":"linear-gradient(180deg, #14110c, #050403)","titlebar-fg":"#ffb43a","titlebar-hover":"rgba(255, 157, 0, .2)","icon":"#ffb43a","icon-alt":"#c8963c","search-bg":"rgba(0, 0, 0, .6)","tabs-bg":"#0a0805","tab-fg":"#e6a43c","tab-active-bg":"rgba(255, 157, 0, .16)","tab-active-fg":"#ffd27a","tab-hover-bg":"rgba(255, 157, 0, .1)","file-bg":"#120e08","file-fg":"#ffb43a","ribbon-bg":"linear-gradient(180deg, #17130c, #0d0b07)","ribbon-border":"1px solid #6b4b12","group-fg":"#ff9d00","nav-bg":"#0a0805","icon-nav":"#e6a43c","icon-active":"#ffd27a","nav-active-bg":"rgba(255, 157, 0, .14)","pagetabs-bg":"#070604","pagetab-bg":"transparent","pagetab-fg":"#e6a43c","pagetab-active-bg":"rgba(255, 157, 0, .18)","pagetab-active-fg":"#ffd27a","panehead-bg":"transparent","panehead-fg":"#ffb43a","status-bg":"#070604","status-fg":"#ffb43a","gutter":"linear-gradient(rgba(255, 157, 0, .07) 1px, transparent 1px) 0 0 / 40px 40px, linear-gradient(90deg, rgba(255, 157, 0, .07) 1px, transparent 1px) 0 0 / 40px 40px, radial-gradient(120% 90% at 50% 40%, #1d1810, #050403 75%)","ui-bg":"#0c0905","ui-fg":"#ffb43a","ui-border":"#6b4b12","ui-font":"Bahnschrift, sans-serif","ui-radius":"2px","hover-bg":"rgba(255, 157, 0, .12)","hover-ring":"rgba(255, 157, 0, .6)","hover-radius":"0","avatar-bg":"rgba(255, 157, 0, .1)","panehead-case":"uppercase","panehead-tracking":".1em"}},"css":"/* Hazard Suit: amber HUD on black, glowing text, HUD corner brackets on the panes,\n   a hazard-stripe rail along the status bar, a lambda before the file name. */\nhtml[data-pbt-skin=\"hazard-suit\"] { --pbt-glow: 0 0 6px rgba(255, 157, 0, .65); }\nhtml[data-pbt-skin=\"hazard-suit\"] div.titlebar .title { font: 600 14px Bahnschrift, sans-serif !important; letter-spacing: .08em; text-shadow: var(--pbt-glow); }\nhtml[data-pbt-skin=\"hazard-suit\"] div.titlebar .title::before { content: \"\u03bb\"; margin-right: 8px; font-weight: 700; color: #ff9d00; }\nhtml[data-pbt-skin=\"hazard-suit\"] div.titlebar { border-bottom: 1px solid #6b4b12; }\n\nhtml[data-pbt-skin=\"hazard-suit\"] #tablist button[role=\"tab\"] { text-transform: uppercase; letter-spacing: .07em; font-size: 12px !important; }\nhtml[data-pbt-skin=\"hazard-suit\"] #tablist button[role=\"tab\"][aria-selected=\"true\"] { text-shadow: var(--pbt-glow); box-shadow: ins)PBT"
LR"PBT(et 0 -2px 0 #ff9d00; }\nhtml[data-pbt-skin=\"hazard-suit\"] #tablist button[role=\"tab\"]::after { display: none !important; }\nhtml[data-pbt-skin=\"hazard-suit\"] [id=\"Ribbon-file\"] { text-shadow: 0 0 3px #000, 0 0 3px #000; font-weight: bold; }\n\n/* HUD brackets: four corner marks drawn with gradients. */\nhtml[data-pbt-skin=\"hazard-suit\"] {\n  --pbt-brackets:\n    linear-gradient(#ff9d00, #ff9d00) top left / 14px 2px, linear-gradient(#ff9d00, #ff9d00) top left / 2px 14px,\n    linear-gradient(#ff9d00, #ff9d00) top right / 14px 2px, linear-gradient(#ff9d00, #ff9d00) top right / 2px 14px,\n    linear-gradient(#ff9d00, #ff9d00) bottom left / 14px 2px, linear-gradient(#ff9d00, #ff9d00) bottom left / 2px 14px,\n    linear-gradient(#ff9d00, #ff9d00) bottom right / 14px 2px, linear-gradient(#ff9d00, #ff9d00) bottom right / 2px 14px;\n}\nhtml[data-pbt-skin=\"hazard-suit\"] .ribbonContainer .bottomBarContainer { background: var(--pbt-brackets), var(--pbt-ribbon-bg) !important; background-repeat: no-repeat !important; }\n/* The Filters pane keeps its colors (the report sets them), so only the side panes get brackets. */\nhtml[data-pbt-skin=\"hazard-suit\"] .sidePane { background: var(--pbt-brackets), #0f0c07 !important; background-repeat: no-repeat !important; }\nhtml[data-pbt-skin=\"hazard-suit\"] :is(.sidePane, .outspacePane) { border-left: 1px solid #2a2111 !important; }\nhtml[data-pbt-skin=\"hazard-suit\"] .ribbonContainer [role=\"group\"] label[class*=\"groupLabel\"] { text-transform: uppercase; letter-spacing: .1em; font-size: 10px !important; text-shadow: var(--pbt-glow); }\nhtml[data-pbt-skin=\"hazard-suit\"] :is(.pane-header, .outspacePane .paneHeader) { text-transform: uppercase; letter-spacing: .1em; text-shadow: var(--pbt-glow); }\nhtml[data-pbt-skin=\"hazard-suit\"] .carouselScrollPane li.section.selected { box-shadow: inset 0 2px 0 #ff9d00; }\n\n/* Hazard rail above the status bar. */\nhtml[data-pbt-skin=\"hazard-suit\"] section.statusBar { border-top: 4px solid transparent; border-image: repeating-linear-gradient(-45deg, #ff9d00 0 8px, #111 8px 16px) 4; text-shadow: var(--pbt-glow); }\n\nhtml[data-pbt-skin=\"hazard-suit\"] ::-webkit-scrollbar { width: 10px; height: 10px; background: #070604; }\nhtml[data-pbt-skin=\"hazard-suit\"] ::-webkit-scrollbar-thumb { background: #6b4b12; border: 1px solid #ff9d00; }\nhtml[data-pbt-skin=\"hazard-suit\"] .pbt-toast { text-shadow: var(--pbt-glow); }\n\n/* File: an upright amber label on a dark plate inside a solid amber outline (striped, it broke\n   up into dashes at this size and read as disabled). */\nhtml[data-pbt-skin=\"hazard-suit\"] [id=\"Ribbon-file\"] { border: 2px solid #ff9d00 !important; border-image: none !important; }\nhtml[data-pbt-skin=\"hazard-suit\"] [id=\"Ribbon-file\"], html[data-pbt-skin=\"hazard-suit\"] [id=\"Ribbon-file\"] * { font-style: normal !important; text-shadow: var(--pbt-glow) !important; }\n/* The Filters pane gets the same HUD brackets as the side panes. */\nhtml[data-pbt-skin=\"hazard-suit\"]:not(.pbt-report-filters) .outspacePane { background: var(--pbt-brackets), #0f0c07 !important; background-repeat: no-repeat !important; }\n\n/* File matches its neighbours: capitals at the tab size. */\nhtml[data-pbt-skin=\"hazard-suit\"] [id=\"Ribbon-file\"] { text-transform: uppercase; letter-spacing: .07em; font-size: 12px !important; }\nhtml[data-pbt-skin=\"hazard-suit\"] [id=\"Ribbon-file\"] * { font-size: 12px !important; }\n/* Painted Filters pane: let the brackets show through its header, as on the side panes. */\nhtml[data-pbt-skin=\"hazard-suit\"]:not(.pbt-report-filters) .outspacePane .paneHeader { background-color: transparent !important; }\n\n/* Window controls: no face, an amber hover inside the box; Close turns red. */\nhtml[data-pbt-skin=\"hazard-suit\"] { --pbt-wc-radius: 0; --pbt-wc-hover-face: rgba(255, 157, 0, .2); --pbt-wc-close-hover-face: #b3261e; }\n"},"ae":{"short":"AE","name":"Analytic Endeavors","order":6,"splash":[")PBT"
LR"PBT(Analytic Endeavors","On brand. Navy ground, teal action, orange accent."],"confetti":{"shapes":["square","ring"],"colors":["#009999","#00B8B8","#FF8C42","#00587C","#8474B2"]},"sound":{"wave":"sine","notes":[784,1047,1319],"step":0.1,"len":0.35},"palette":{"ramp":[["#000000","#0a1628"],["#141414","#0d0d1a"],["#292929","#1a1a2e"],["#3d3d3d","#2a3a5a"],["#5c5c5c","#3d4f73"],["#8f8f8f","#8d8d9e"],["#d6d6d6","#d6dbe6"],["#ffffff","#ffffff"]],"accent":"#009999","vars":{"font":"Inter, \"Segoe UI\", sans-serif","title-font":"Inter, \"Segoe UI\", sans-serif","titlebar-bg":"linear-gradient(90deg, #0a1628 0%, #003d56 100%)","titlebar-fg":"#ffffff","icon":"#00b8b8","icon-alt":"#b0b0b0","tabs-bg":"#0a1628","tab-fg":"#b0b0b0","tab-active-fg":"#ffffff","file-bg":"#009999","file-fg":"#ffffff","ribbon-bg":"#1a1a2e","ribbon-border":"1px solid #2a3a5a","group-fg":"#00b8b8","nav-bg":"#0d0d1a","icon-nav":"#b0b0b0","icon-active":"#00b8b8","pagetabs-bg":"#0d0d1a","pagetab-active-bg":"#1a1a2e","panehead-fg":"#ffffff","status-bg":"#0a1628","status-fg":"#b0b0b0","gutter":"linear-gradient(160deg, #0a1628 0%, #0d0d1a 55%, #0a1628 100%)","ui-bg":"#1a1a2e","ui-fg":"#ffffff","ui-border":"#2a3a5a","ui-radius":"8px","hover-bg":"rgba(0, 153, 153, .18)","hover-ring":"#009999","hover-radius":"6px"}},"css":"/* Analytic Endeavors: navy ground, teal action color, orange accent line under the chosen tab. */\nhtml[data-pbt-skin=\"ae\"] #tablist button[role=\"tab\"][aria-selected=\"true\"] { box-shadow: inset 0 -3px 0 #ff8c42; }\nhtml[data-pbt-skin=\"ae\"] #tablist button[role=\"tab\"]::after { display: none !important; }\nhtml[data-pbt-skin=\"ae\"] .ribbonContainer .bottomBarContainer { border-radius: 8px !important; }\nhtml[data-pbt-skin=\"ae\"] .carouselScrollPane li.section.selected { box-shadow: inset 0 -3px 0 #009999; }\nhtml[data-pbt-skin=\"ae\"] ::-webkit-scrollbar { width: 10px; height: 10px; background: #0d0d1a; }\nhtml[data-pbt-skin=\"ae\"] ::-webkit-scrollbar-thumb { background: #2a3a5a; border-radius: 5px; }\nhtml[data-pbt-skin=\"ae\"] ::-webkit-scrollbar-thumb:hover { background: #009999; }\n"},"starship-panel":{"short":"Starship","name":"Starship Panel","order":7,"splash":["Computer Ready","All decks report nominal. Awaiting your report."],"confetti":{"shapes":["pill"],"colors":["#ff9966","#cc99cc","#9999ff","#ffcc99","#cc6666","#ffcc66"]},"sound":{"wave":"sine","notes":[1046,1397],"step":0.07,"len":0.09},"palette":{"ramp":[["#000000","#000000"],["#141414","#05050a"],["#292929","#101018"],["#3d3d3d","#22222e"],["#5c5c5c","#3a3a4a"],["#8f8f8f","#9a8fb0"],["#d6d6d6","#f0d8b0"],["#ffffff","#ffe6c8"]],"accent":"#ff9966","vars":{"font":"Bahnschrift, \"Agency FB\", \"Segoe UI\", sans-serif","title-font":"Bahnschrift, \"Agency FB\", sans-serif","titlebar-bg":"#000000","titlebar-fg":"#ff9966","titlebar-hover":"rgba(255, 153, 102, .22)","icon":"#ff9966","icon-alt":"#cc99cc","search-bg":"#101018","tabs-bg":"#000000","tab-fg":"#000000","tab-active-bg":"#000000","tab-active-fg":"#ffcc66","tab-hover-bg":"#ffcc99","file-bg":"#cc6666","file-fg":"#000000","ribbon-bg":"#000000","group-fg":"#ffcc99","hover-bg":"rgba(255, 153, 102, .22)","hover-ring":"#ff9966","hover-radius":"14px","nav-bg":"#cc99cc","icon-nav":"#000000","icon-active":"#000000","nav-active-bg":"#ff9966","nav-hover-bg":"rgba(0, 0, 0, .15)","pagetabs-bg":"#000000","pagetab-bg":"#9999ff","pagetab-fg":"#000000","pagetab-active-bg":"#ffcc66","pagetab-active-fg":"#000000","panehead-bg":"linear-gradient(90deg, #ff9966, #ff9966)","panehead-fg":"#000000","status-bg":"#000000","status-fg":"#ffcc99","wc-glyph":"12px","ui-bg":"#000000","ui-fg":"#ffcc99","ui-border":"#cc99cc","ui-font":"Bahnschrift, sans-serif","ui-radius":"18px","share-fg":"#ff9966","panehead-case":"uppercase","panehead-tracking":".06em"}},"css":"/* Starship Panel: black ground, rounded pill buttons in orange, lilac, periwinkle and peach,\n   an elbow bar sweeping around the ribbon, condensed capitals everywhere. */\nhtml[data-pbt-skin=\")PBT"
LR"PBT(starship-panel\"] {\n  --pbt-lc-orange: #ff9966; --pbt-lc-lilac: #cc99cc; --pbt-lc-blue: #9999ff; --pbt-lc-peach: #ffcc99; --pbt-lc-red: #cc6666; --pbt-lc-gold: #ffcc66;\n}\nhtml[data-pbt-skin=\"starship-panel\"] :is(#tablist button[role=\"tab\"], [id=\"Ribbon-file\"], .ribbonContainer label[class*=\"groupLabel\"], .pane-header h2, .outspacePane .paneHeader, section.statusBar, .carouselScrollPane li.section, div.titlebar .title, .pbt-rgroup-label) {\n  text-transform: uppercase; letter-spacing: .06em; font-stretch: condensed;\n}\n\n/* Title: an orange cap before the name. */\nhtml[data-pbt-skin=\"starship-panel\"] div.titlebar .title { font-weight: 600 !important; }\nhtml[data-pbt-skin=\"starship-panel\"] div.titlebar .title::before {\n  content: \"\"; display: inline-block; width: 44px; height: 14px; margin-right: 10px; vertical-align: -2px;\n  border-radius: 999px 0 0 999px; background: var(--pbt-lc-orange);\n}\n\n\n/* Tabs: pills, each in the next console color. The open one is the only hollow pill: black with\n   a gold rim and gold letters, so it never reads as one more color in the row. */\nhtml[data-pbt-skin=\"starship-panel\"] #tablist button[role=\"tab\"] { border-radius: 999px !important; margin: 3px 2px !important; height: 25px !important; padding: 0 14px !important; font-weight: 600; }\nhtml[data-pbt-skin=\"starship-panel\"] #tablist button[role=\"tab\"]::after { display: none !important; }\n/* Any tab outside the usual tab list (contextual tabs such as Format) still gets a pill to sit on. */\nhtml[data-pbt-skin=\"starship-panel\"] .ribbonContainer button[role=\"tab\"]:not([aria-selected=\"true\"]) { background: var(--pbt-lc-peach) !important; }\nhtml[data-pbt-skin=\"starship-panel\"] #tablist .ms-OverflowSet-item:nth-child(5n+1) button[role=\"tab\"]:not([aria-selected=\"true\"]) { background: var(--pbt-lc-orange) !important; }\nhtml[data-pbt-skin=\"starship-panel\"] #tablist .ms-OverflowSet-item:nth-child(5n+2) button[role=\"tab\"]:not([aria-selected=\"true\"]) { background: var(--pbt-lc-lilac) !important; }\nhtml[data-pbt-skin=\"starship-panel\"] #tablist .ms-OverflowSet-item:nth-child(5n+3) button[role=\"tab\"]:not([aria-selected=\"true\"]) { background: var(--pbt-lc-blue) !important; }\nhtml[data-pbt-skin=\"starship-panel\"] #tablist .ms-OverflowSet-item:nth-child(5n+4) button[role=\"tab\"]:not([aria-selected=\"true\"]) { background: var(--pbt-lc-peach) !important; }\nhtml[data-pbt-skin=\"starship-panel\"] #tablist .ms-OverflowSet-item:nth-child(5n+5) button[role=\"tab\"]:not([aria-selected=\"true\"]) { background: var(--pbt-lc-red) !important; }\nhtml[data-pbt-skin=\"starship-panel\"] #tablist button[role=\"tab\"]:hover { filter: brightness(1.15); }\nhtml[data-pbt-skin=\"starship-panel\"] #tablist button[role=\"tab\"][aria-selected=\"true\"] { box-shadow: inset 0 0 0 2px var(--pbt-lc-gold); }\nhtml[data-pbt-skin=\"starship-panel\"] [id=\"Ribbon-file\"] { border-radius: 999px !important; margin: 3px 4px 3px 6px !important; height: 25px !important; padding: 0 16px !important; font-weight: 700; }\n\n/* Ribbon: the elbow. A thick lilac bar runs along the top and down the left with a rounded corner. */\nhtml[data-pbt-skin=\"starship-panel\"] .ribbonContainer .bottomBarContainer {\n  border-style: solid !important; border-color: var(--pbt-lc-lilac) !important;\n  border-width: 7px 0 0 41px !important; border-top-left-radius: 48px 30px !important; border-radius: 48px 0 0 0 / 30px 0 0 0 !important;\n}\nhtml[data-pbt-skin=\"starship-panel\"] .ribbonContainer .bottomBarContainer button:not(:disabled):hover { filter: brightness(1.15); }\nhtml[data-pbt-skin=\"starship-panel\"] .ribbonContainer [role=\"group\"] label[class*=\"groupLabel\"] { font-weight: 600; }\nhtml[data-pbt-skin=\"starship-panel\"] .ribbonContainer [class*=\"groupContainer\"] + [class*=\"groupContainer\"] { border-left: 2px solid var(--pbt-lc-blue) !important; }\n\n/* View switcher: a lilac column ending in a rounded foot. */\nhtml[data-pbt-s)PBT"
LR"PBT(kin=\"starship-panel\"] .viewNavigatorContainer { border-radius: 0 0 0 26px; }\nhtml[data-pbt-skin=\"starship-panel\"] view-navigator button { margin: 2px 0; border-radius: 0 14px 14px 0; }\nhtml[data-pbt-skin=\"starship-panel\"] view-navigator button[aria-selected=\"true\"] { box-shadow: none; }\n\n/* Panes: an orange header pill, black body. */\nhtml[data-pbt-skin=\"starship-panel\"] :is(.pane-header, .outspacePane .paneHeader) { border-radius: 999px 0 0 999px; margin: 4px 0 4px 4px; font-weight: 600; }\nhtml[data-pbt-skin=\"starship-panel\"] .outspacePane.outspacePaneCollapsed { border-radius: 0 0 0 22px; }\nhtml[data-pbt-skin=\"starship-panel\"] .sidePane { border-left: 4px solid #000 !important; }\n\n/* Page tabs: pills; the New page button is one too. */\nhtml[data-pbt-skin=\"starship-panel\"] .carouselScrollPane li.section, html[data-pbt-skin=\"starship-panel\"] button.section.static.create .thumbnail-container { border-radius: 999px !important; margin: 3px 2px; }\nhtml[data-pbt-skin=\"starship-panel\"] button.section.static.create .thumbnail-container { background: var(--pbt-pagetab-bg) !important; box-shadow: none; }\nhtml[data-pbt-skin=\"starship-panel\"] button.section.static.create { color: #000 !important; }\n\n/* Status bar: a lilac segment leading the readouts. */\nhtml[data-pbt-skin=\"starship-panel\"] section.statusBar .leftPane::before {\n  content: \"\"; display: inline-block; width: 60px; height: 12px; margin-right: 8px; vertical-align: middle; border-radius: 999px; background: var(--pbt-lc-lilac);\n}\n\n/* Share, menu and toasts as console panels. */\nhtml[data-pbt-skin=\"starship-panel\"] .farPeripheralControls button { border-radius: 999px !important; }\nhtml[data-pbt-skin=\"starship-panel\"] .pbt-toast { border: 0; border-left: 22px solid var(--pbt-lc-orange); border-radius: 22px 6px 6px 22px; }\nhtml[data-pbt-skin=\"starship-panel\"] .pbt-menu h4 { color: var(--pbt-lc-orange); opacity: 1; text-transform: uppercase; }\nhtml[data-pbt-skin=\"starship-panel\"] .pbt-menu button:hover { background: var(--pbt-lc-orange) !important; color: #000; border-radius: 999px; --pbt-ui-fg: #000; --pbt-ui-bg: var(--pbt-lc-orange); --pbt-accent: #000; }\nhtml[data-pbt-skin=\"starship-panel\"] .pbt-menu .pbt-skins button:hover { border-radius: 6px; }\nhtml[data-pbt-skin=\"starship-panel\"] ::-webkit-scrollbar { width: 12px; height: 12px; background: #000; }\nhtml[data-pbt-skin=\"starship-panel\"] ::-webkit-scrollbar-thumb { background: var(--pbt-lc-lilac); border-radius: 999px; border: 2px solid #000; }\n\n/* The elbow bar takes 7px off the panel; lift the Skins group label clear of the bottom edge. */\nhtml[data-pbt-skin=\"starship-panel\"] .pbt-rgroup { padding-bottom: 8px; }\n\n/* A page tab's close x sits inside the pill, clear of its curved end. */\n/* The tab's close \"x\" (shown on hover and focus) gets its own room at the pill's right end, so it\n   never runs into the label (\"SALES ANALYSISX\"); lowercase and dimmed, like Power BI's own. */\nhtml[data-pbt-skin=\"starship-panel\"] .carouselScrollPane li.section .sectionDeleteIcon { position: absolute !important; top: 50% !important; right: -18px !important; transform: translateY(-50%); text-transform: none !important; opacity: .7; }\nhtml[data-pbt-skin=\"starship-panel\"] .carouselScrollPane li.section { padding: 0 26px 0 10px !important; }\n\n/* Window controls: colored pills inside Power BI's own button boxes. */\nhtml[data-pbt-skin=\"starship-panel\"] {\n  --pbt-wc-inset: 7px 5px; --pbt-wc-radius: 999px; --pbt-wc-fg: #000; --pbt-wc-close-hover-fg: #000;\n  --pbt-wc-face: var(--pbt-lc-blue); --pbt-wc-close-face: var(--pbt-lc-red);\n  --pbt-wc-hover-bg: transparent; --pbt-wc-close-hover-bg: transparent;\n}\nhtml[data-pbt-skin=\"starship-panel\"] .windowControls button:nth-child(2)::before { background: var(--pbt-lc-lilac); }\n\n/* The 8 px gutter the ribbon row keeps at its left showed black beside the lilac elbow and ended where the wider lilac rail )PBT"
LR"PBT(begins; the elbow runs to the window edge instead. */\nhtml[data-pbt-skin=\"starship-panel\"] .ribbonContainer div:has(> .bottomBarContainer) { padding-left: 0 !important; }\n"},"phosphor-terminal":{"short":"Phosphor","name":"Phosphor Terminal","order":8,"splash":["TERMINAL READY","> LOAD REPORT ... OK"],"confetti":{"shapes":["bits"],"colors":["#33ff77","#66ff99","#1fbf55","#b8ffcc"]},"sound":{"wave":"square","notes":[440,880,440],"step":0.05,"len":0.05},"palette":{"mono":true,"ramp":[["#000000","#020a04"],["#141414","#041208"],["#292929","#072010"],["#3d3d3d","#0c3318"],["#5c5c5c","#145226"],["#8f8f8f","#2f9a50"],["#d6d6d6","#6dff9a"],["#ffffff","#b8ffcc"]],"accent":"#33ff77","vars":{"font":"Consolas, \"Lucida Console\", monospace","title-font":"Consolas, \"Lucida Console\", monospace","titlebar-bg":"repeating-linear-gradient(180deg, rgba(0, 0, 0, .3) 0 1px, transparent 1px 3px), linear-gradient(180deg, #06200f, #031208)","titlebar-fg":"#66ff99","titlebar-hover":"rgba(51, 255, 119, .18)","icon":"#66ff99","icon-alt":"#2f9a50","avatar-bg":"transparent","search-bg":"#020a04","tabs-bg":"repeating-linear-gradient(180deg, rgba(0, 0, 0, .3) 0 1px, transparent 1px 3px), #031208","tab-fg":"#3ee673","tab-active-bg":"#33ff77","tab-active-fg":"#021006","tab-hover-bg":"rgba(51, 255, 119, .15)","file-bg":"transparent","file-fg":"#66ff99","ribbon-bg":"repeating-linear-gradient(180deg, rgba(0, 0, 0, .28) 0 1px, transparent 1px 3px), radial-gradient(120% 140% at 50% 40%, #0a2a14, #020a04 80%)","ribbon-border":"1px solid #1d6b36","group-fg":"#3ee673","hover-bg":"rgba(51, 255, 119, .14)","hover-ring":"#33ff77","hover-radius":"0","nav-bg":"repeating-linear-gradient(180deg, rgba(0, 0, 0, .3) 0 1px, transparent 1px 3px), #031208","icon-nav":"#3ee673","icon-active":"#021006","nav-active-bg":"#33ff77","nav-hover-bg":"rgba(51, 255, 119, .15)","pagetabs-bg":"#020a04","pagetab-bg":"transparent","pagetab-fg":"#3ee673","pagetab-active-bg":"#33ff77","pagetab-active-fg":"#021006","panehead-bg":"linear-gradient(90deg, #0c3318, #072010)","panehead-fg":"#66ff99","status-bg":"repeating-linear-gradient(180deg, rgba(0, 0, 0, .3) 0 1px, transparent 1px 3px), #031208","status-fg":"#3ee673","ui-bg":"#020a04","ui-fg":"#66ff99","ui-border":"#1d6b36","ui-font":"Consolas, monospace","ui-radius":"0","panehead-case":"uppercase","panehead-tracking":"0"}},"css":"/* Phosphor Terminal: one-color green phosphor screen. Scanlines, a soft glow on every letter, a dark\n   vignette at the edges, Office icons tinted green, inverted highlight for whatever is selected,\n   and a blinking cursor after the file name. */\nhtml[data-pbt-skin=\"phosphor-terminal\"] :is(titlebar, ribbon-report-view, view-navigator, .sidePane, .outspacePane .paneHeader, status-bar, exploration-footer-modern) {\n  text-shadow: 0 0 4px rgba(51, 255, 119, .55);\n}\n/* Selected items are inverted (dark text on green); no glow there. */\nhtml[data-pbt-skin=\"phosphor-terminal\"] :is(#tablist button[role=\"tab\"][aria-selected=\"true\"], .carouselScrollPane li.section.selected) * { text-shadow: none !important; }\n\n/* Title: a prompt and a blinking cursor. */\nhtml[data-pbt-skin=\"phosphor-terminal\"] div.titlebar .title::before { content: \"> \"; }\nhtml[data-pbt-skin=\"phosphor-terminal\"] div.titlebar .title::after { content: \"_\"; margin-left: 2px; animation: pbt-phosphor-terminal-blink 1.1s steps(1) infinite; }\n@keyframes pbt-phosphor-terminal-blink { 50% { opacity: 0; } }\n@media (prefers-reduced-motion: reduce) { html[data-pbt-skin=\"phosphor-terminal\"] div.titlebar .title::after { animation: none; } }\n\n\n/* Tabs and File in capitals; File in brackets. */\nhtml[data-pbt-skin=\"phosphor-terminal\"] #tablist button[role=\"tab\"] { text-transform: uppercase; letter-spacing: .04em; font-size: 12px !important; }\nhtml[data-pbt-skin=\"phosphor-terminal\"] #tablist button[role=\"tab\"]::after { display: none !important; }\nhtml[data-pbt-skin=\"phosphor-terminal\"] [id=\"Ribbon-file\"] { text-tr)PBT"
LR"PBT(ansform: uppercase; box-shadow: inset 0 0 0 1px #1d6b36; }\nhtml[data-pbt-skin=\"phosphor-terminal\"] [id=\"Ribbon-file\"] .ms-Button-label::before { content: \"[ \"; }\nhtml[data-pbt-skin=\"phosphor-terminal\"] [id=\"Ribbon-file\"] .ms-Button-label::after { content: \" ]\"; }\n\n/* The screen: rounded glass with a vignette. Office icons go green, except the report theme\n   previews in the Themes gallery, which must keep their real colors to be chosen. */\nhtml[data-pbt-skin=\"phosphor-terminal\"] .ribbonContainer .bottomBarContainer { border-radius: 10px !important; box-shadow: inset 0 0 40px rgba(0, 0, 0, .85), 0 0 10px rgba(51, 255, 119, .15); }\nhtml[data-pbt-skin=\"phosphor-terminal\"] .ribbonContainer .bottomBarContainer :is(svg, img):not(.pbt-sw):not(#themesGroup-group *) { filter: grayscale(1) sepia(1) hue-rotate(75deg) saturate(4) brightness(1.15); }\nhtml[data-pbt-skin=\"phosphor-terminal\"] .ribbonContainer [role=\"group\"] label[class*=\"groupLabel\"] { text-transform: uppercase; letter-spacing: .06em; }\nhtml[data-pbt-skin=\"phosphor-terminal\"] .sidePane { box-shadow: inset 0 0 30px rgba(0, 0, 0, .8); background-image: repeating-linear-gradient(180deg, rgba(0, 0, 0, .22) 0 1px, transparent 1px 3px) !important; }\nhtml[data-pbt-skin=\"phosphor-terminal\"] :is(.pane-header, .outspacePane .paneHeader) { border-bottom: 1px solid #1d6b36; }\nhtml[data-pbt-skin=\"phosphor-terminal\"] :is(.pane-header, .outspacePane .paneHeader) :is(h2, .pane-title) { letter-spacing: 0; font-size: 13px !important; }\nhtml[data-pbt-skin=\"phosphor-terminal\"] :is(.sidePane, .outspacePane) { border-left: 1px solid #1d6b36 !important; }\n\n/* Page tabs: plain text until chosen, then inverted. */\nhtml[data-pbt-skin=\"phosphor-terminal\"] .carouselScrollPane li.section, html[data-pbt-skin=\"phosphor-terminal\"] button.section.static.create .thumbnail-container { box-shadow: inset 0 0 0 1px #145226 !important; }\nhtml[data-pbt-skin=\"phosphor-terminal\"] section.statusBar { border-top: 1px solid #1d6b36; }\n\nhtml[data-pbt-skin=\"phosphor-terminal\"] .pbt-menu button:hover { background: #33ff77 !important; color: #021006; text-shadow: none; --pbt-ui-fg: #021006; --pbt-ui-bg: #33ff77; --pbt-accent: #021006; }\nhtml[data-pbt-skin=\"phosphor-terminal\"] .pbt-toast { text-shadow: 0 0 4px rgba(51, 255, 119, .55); border-left-color: #33ff77; }\nhtml[data-pbt-skin=\"phosphor-terminal\"] ::-webkit-scrollbar { width: 12px; height: 12px; background: #020a04; }\nhtml[data-pbt-skin=\"phosphor-terminal\"] ::-webkit-scrollbar-thumb { background: #145226; border: 1px solid #33ff77; }\n\n/* Visual type icons in the Visualizations pane carry fixed colors (dark lines, white and blue\n   fills, the same in both modes). Inverted first, so lines turn bright and white fills go dark,\n   then tinted green by brightness: bright lines over dim fills, two tones like the ribbon. */\nhtml[data-pbt-skin=\"phosphor-terminal\"] .sidePane .visual-types-container :is(svg, img) { filter: invert(1) grayscale(1) sepia(1) hue-rotate(75deg) saturate(4) brightness(1.05); }\n/* The Filters pane matches the side panes. */\nhtml[data-pbt-skin=\"phosphor-terminal\"]:not(.pbt-report-filters) .outspacePane { box-shadow: inset 0 0 30px rgba(0, 0, 0, .8); background-image: repeating-linear-gradient(180deg, rgba(0, 0, 0, .22) 0 1px, transparent 1px 3px) !important; }\n\n/* Window controls: bracketed terminal keys inside Power BI's own button boxes; hover inverts. */\nhtml[data-pbt-skin=\"phosphor-terminal\"] {\n  --pbt-wc-inset: 5px 4px; --pbt-wc-radius: 0; --pbt-wc-ring: inset 0 0 0 1px #1d6b36;\n  --pbt-wc-hover-face: #33ff77; --pbt-wc-close-hover-face: #33ff77; --pbt-wc-close-hover-fg: #021006;\n}\nhtml[data-pbt-skin=\"phosphor-terminal\"] .windowControls button:hover { color: #021006 !important; }\n"},"grid":{"short":"Neon","name":"Neon Grid","order":9,"splash":["PROGRAM LOADED","The grid is yours. Bring your data."],"confetti":{"shapes":["streak"],"colors":["#00e5ff","#7)PBT"
LR"PBT(ff6ff","#ff7a00","#ffffff"]},"sound":{"wave":"sawtooth","notes":[220,440,880],"step":0.05,"len":0.12},"palette":{"ramp":[["#000000","#000308"],["#141414","#02070f"],["#292929","#061224"],["#3d3d3d","#0b1f3a"],["#5c5c5c","#123158"],["#8f8f8f","#4a7fb0"],["#d6d6d6","#bff4ff"],["#ffffff","#e8fdff"]],"accent":"#00e5ff","vars":{"font":"Bahnschrift, \"Segoe UI\", sans-serif","title-font":"Bahnschrift, sans-serif","titlebar-bg":"linear-gradient(180deg, #02070f, #000308)","titlebar-fg":"#9ff4ff","titlebar-hover":"rgba(0, 229, 255, .16)","icon":"#00e5ff","icon-alt":"#4a7fb0","search-bg":"#02070f","tabs-bg":"#000308","tab-fg":"#7fdfff","tab-active-bg":"transparent","tab-active-fg":"#ffb066","tab-hover-bg":"rgba(0, 229, 255, .1)","file-bg":"transparent","file-fg":"#ff9a3c","ribbon-bg":"linear-gradient(180deg, #030b18, #01040a)","ribbon-border":"1px solid #00e5ff","group-fg":"#00e5ff","hover-bg":"rgba(0, 229, 255, .12)","hover-ring":"#00e5ff","hover-radius":"2px","nav-bg":"#000308","icon-nav":"#4fd8ff","icon-active":"#ff9a3c","nav-active-bg":"rgba(255, 122, 0, .12)","pagetabs-bg":"#000308","pagetab-bg":"transparent","pagetab-fg":"#7fdfff","pagetab-active-bg":"rgba(255, 122, 0, .12)","pagetab-active-fg":"#ffb066","panehead-bg":"linear-gradient(180deg, #061224, #02070f)","panehead-fg":"#9ff4ff","status-bg":"#000308","status-fg":"#7fdfff","ui-bg":"#02070f","ui-fg":"#bff4ff","ui-border":"#00e5ff","ui-font":"Bahnschrift, sans-serif","ui-radius":"2px","panehead-case":"uppercase","panehead-tracking":".1em","gutter":"linear-gradient(rgba(0, 229, 255, .07) 1px, transparent 1px) 0 0 / 24px 24px, linear-gradient(90deg, rgba(0, 229, 255, .07) 1px, transparent 1px) 0 0 / 24px 24px, radial-gradient(120% 100% at 50% 0%, #04142a, #000308 70%)"}},"css":"/* Grid Runner: black space, thin neon cyan edges that glow, orange for whatever is chosen, a faint\n   grid on the canvas around the page (palette.vars gutter; never behind controls, where its lines\n   read as separators or input edges), and a light trail running along the bottom of the title bar. */\nhtml[data-pbt-skin=\"grid\"] { --pbt-glow-c: 0 0 6px rgba(0, 229, 255, .8), 0 0 14px rgba(0, 229, 255, .35); --pbt-glow-o: 0 0 6px rgba(255, 122, 0, .85), 0 0 14px rgba(255, 122, 0, .35); }\nhtml[data-pbt-skin=\"grid\"] :is(#tablist button[role=\"tab\"], .ribbonContainer label[class*=\"groupLabel\"], .pane-header h2, .outspacePane .paneHeader, div.titlebar .title) { text-transform: uppercase; letter-spacing: .1em; }\n\n/* Title bar: neon base line with a light trail running along it. The trail is a full-width strip\n   whose background moves, so nothing ever extends past the window edge. */\nhtml[data-pbt-skin=\"grid\"] div.titlebar { position: relative; box-shadow: inset 0 -1px 0 rgba(0, 229, 255, .7); }\nhtml[data-pbt-skin=\"grid\"] div.titlebar::after {\n  content: \"\"; position: absolute; left: 0; right: 0; bottom: 0; height: 2px; pointer-events: none;\n  background: linear-gradient(90deg, transparent, #00e5ff 70%, #fff) no-repeat -160px 0 / 160px 2px;\n  filter: drop-shadow(0 0 3px #00e5ff);\n  animation: pbt-grid-trail 7s linear infinite;\n}\n@keyframes pbt-grid-trail { from { background-position: -160px 0; } to { background-position: calc(100% + 160px) 0; } }\n@media (prefers-reduced-motion: reduce) { html[data-pbt-skin=\"grid\"] div.titlebar::after { animation: none; display: none; } }\nhtml[data-pbt-skin=\"grid\"] div.titlebar .title { text-shadow: 0 0 6px rgba(0, 229, 255, .8); font-weight: 600 !important; }\n\n\n/* Tabs: cyan text, the open tab orange with a glowing underline. */\nhtml[data-pbt-skin=\"grid\"] #tablist button[role=\"tab\"] { text-shadow: 0 0 4px rgba(0, 229, 255, .5); font-size: 12px !important; }\nhtml[data-pbt-skin=\"grid\"] #tablist button[role=\"tab\"]::after { display: none !important; }\nhtml[data-pbt-skin=\"grid\"] #tablist button[role=\"tab\"][aria-selected=\"true\"] { box-shadow: inset 0 -2px 0 #ff7a00, 0 8px 10px -8px #ff7a00; text-shadow: 0 0 6px rgba(255, )PBT"
LR"PBT(122, 0, .8); }\nhtml[data-pbt-skin=\"grid\"] [id=\"Ribbon-file\"] { box-shadow: inset 0 0 0 1px #ff7a00; text-shadow: 0 0 6px rgba(255, 122, 0, .8); text-transform: uppercase; letter-spacing: .1em; }\nhtml[data-pbt-skin=\"grid\"] .ribbonTopBarContainer { box-shadow: inset 0 -1px 0 rgba(0, 229, 255, .25); }\n\n/* Ribbon: a glowing frame. */\nhtml[data-pbt-skin=\"grid\"] .ribbonContainer .bottomBarContainer { border-radius: 4px !important; box-shadow: var(--pbt-glow-c), inset 0 0 14px rgba(0, 229, 255, .12); }\nhtml[data-pbt-skin=\"grid\"] .ribbonContainer .bottomBarContainer button:not(:disabled):hover { box-shadow: inset 0 0 0 1px #00e5ff, var(--pbt-glow-c) !important; }\nhtml[data-pbt-skin=\"grid\"] .ribbonContainer [role=\"group\"] label[class*=\"groupLabel\"] { font-size: 10px !important; text-shadow: 0 0 4px rgba(0, 229, 255, .6); }\nhtml[data-pbt-skin=\"grid\"] .ribbonContainer [class*=\"groupContainer\"] + [class*=\"groupContainer\"] { border-left: 1px solid rgba(0, 229, 255, .35) !important; }\n\n/* Panes and the view switcher: neon edges. */\nhtml[data-pbt-skin=\"grid\"] :is(.sidePane, .outspacePane) { border-left: 1px solid #00e5ff !important; box-shadow: -2px 0 8px -2px rgba(0, 229, 255, .6); }\nhtml[data-pbt-skin=\"grid\"] :is(.pane-header, .outspacePane .paneHeader) { box-shadow: inset 0 -1px 0 rgba(0, 229, 255, .6); text-shadow: 0 0 4px rgba(0, 229, 255, .6); }\nhtml[data-pbt-skin=\"grid\"] .viewNavigatorContainer { box-shadow: inset -1px 0 0 rgba(0, 229, 255, .6); }\nhtml[data-pbt-skin=\"grid\"] view-navigator button[aria-selected=\"true\"] { box-shadow: inset 3px 0 0 #ff7a00; }\n\n/* Page tabs: outlined; the open one orange. */\nhtml[data-pbt-skin=\"grid\"] .carouselScrollPane li.section, html[data-pbt-skin=\"grid\"] button.section.static.create .thumbnail-container { box-shadow: inset 0 0 0 1px rgba(0, 229, 255, .5) !important; }\nhtml[data-pbt-skin=\"grid\"] .carouselScrollPane li.section.selected { box-shadow: inset 0 0 0 1px #ff7a00, var(--pbt-glow-o) !important; }\nhtml[data-pbt-skin=\"grid\"] section.statusBar { box-shadow: inset 0 1px 0 rgba(0, 229, 255, .7), 0 -2px 8px -2px rgba(0, 229, 255, .5); }\n\nhtml[data-pbt-skin=\"grid\"] .pbt-toast, html[data-pbt-skin=\"grid\"] .pbt-menu { box-shadow: var(--pbt-glow-c); }\nhtml[data-pbt-skin=\"grid\"] .pbt-toast { border-left-color: #ff7a00; }\nhtml[data-pbt-skin=\"grid\"] ::-webkit-scrollbar { width: 10px; height: 10px; background: #000308; }\nhtml[data-pbt-skin=\"grid\"] ::-webkit-scrollbar-thumb { background: #0b1f3a; border: 1px solid #00e5ff; border-radius: 5px; }\n\n/* The open page tab is marked in orange, like the open ribbon tab. */\nhtml[data-pbt-skin=\"grid\"] .carouselScrollPane li.section.selected { border-bottom-color: #ff7a00 !important; }\n\n/* Window controls: neon outlines inside Power BI's own button boxes; Close burns orange. */\nhtml[data-pbt-skin=\"grid\"] {\n  --pbt-wc-inset: 5px 4px; --pbt-wc-radius: 2px; --pbt-wc-ring: inset 0 0 0 1px rgba(0, 229, 255, .7);\n  --pbt-wc-hover-face: rgba(0, 229, 255, .12); --pbt-wc-close-hover-face: rgba(255, 122, 0, .14); --pbt-wc-close-hover-fg: #ffb066;\n}\nhtml[data-pbt-skin=\"grid\"] .windowControls button:hover::before { box-shadow: inset 0 0 0 1px #00e5ff, var(--pbt-glow-c); }\nhtml[data-pbt-skin=\"grid\"] .windowControls button.closeButton:hover::before { box-shadow: inset 0 0 0 1px #ff7a00, var(--pbt-glow-o); }\n\n/* Disabled Undo and Redo: the cyan icons sit at the edge of visibility at the base opacity. */\nhtml[data-pbt-skin=\"grid\"] titlebar-toolbar button.is-disabled { opacity: .82; }\n"},"ink":{"short":"Ink","name":"High Contrast Ink","order":10,"splash":["High Contrast Ink","Black on white. Every word at 7:1 or better."],"confetti":{"shapes":["square"],"colors":["#000000","#ffd400","#0033a0","#595959"]},"sound":{"wave":"triangle","notes":[880,660,880],"step":0.08,"len":0.12},"palette":{"ramp":[["#000000","#ffffff"],["#141414","#ffffff"],["#292929","#f5f5f5"],["#3d3d3d","#e0e0e0)PBT"
LR"PBT("],["#5c5c5c","#6e6e6e"],["#8f8f8f","#262626"],["#d6d6d6","#000000"],["#ffffff","#000000"]],"accent":"#0033a0","vars":{"font":"\"Segoe UI\", sans-serif","title-font":"\"Segoe UI\", sans-serif","titlebar-bg":"#ffffff","titlebar-fg":"#000000","titlebar-hover":"#e0e0e0","icon":"#000000","icon-alt":"#000000","icon-fill":"#ffffff","icon-line":"#000000","search-bg":"#ffffff","search-fg":"#000000","tabs-bg":"#ffffff","tab-fg":"#000000","tab-active-bg":"#000000","tab-active-fg":"#ffffff","tab-hover-bg":"#e0e0e0","file-bg":"#ffffff","file-fg":"#000000","ribbon-bg":"#ffffff","ribbon-border":"2px solid #000000","ribbon-fg":"#000000","ribbon-fg-disabled":"#525252","group-fg":"#000000","nav-bg":"#ffffff","icon-nav":"#000000","icon-active":"#ffffff","nav-active-bg":"#000000","nav-hover-bg":"#e0e0e0","pagetabs-bg":"#ffffff","pagetab-bg":"#ffffff","pagetab-fg":"#000000","pagetab-active-bg":"#000000","pagetab-active-fg":"#ffffff","panehead-bg":"#ffffff","panehead-solid":"#ffffff","panehead-fg":"#000000","status-bg":"#ffffff","status-fg":"#000000","share-fg":"#000000","gutter":"#ececec","ui-bg":"#ffffff","ui-fg":"#000000","ui-border":"#000000","ui-font":"\"Segoe UI\", sans-serif","ui-radius":"0","ui-shadow":"0 0 0 1px #000000, 0 6px 18px rgba(0, 0, 0, .35)","hover-bg":"#e0e0e0","hover-ring":"#000000","hover-radius":"0","avatar-bg":"transparent"}},"css":"/* High Contrast Ink: black on white, every word at 7:1 or better, 2 px black rules, a yellow focus\n   ring, no gradients, glow or shadows on the chrome, and UI text a size up. The open tab, view and\n   page are inverted (white on black), one marker each. */\n\n/* Focus: a yellow ring with a black edge outside it, so it shows on white and on black alike.\n   Fluent draws its own focus outline from these tokens. */\nhtml[data-pbt-skin=\"ink\"][data-theme], html[data-pbt-skin=\"ink\"] .fui-FluentProvider { --colorStrokeFocus1: #000000 !important; --colorStrokeFocus2: #ffd400 !important; }\nhtml[data-pbt-skin=\"ink\"] :focus-visible { outline: 3px solid #ffd400 !important; outline-offset: 0 !important; box-shadow: 0 0 0 5px #000 !important; }\n\n/* Disabled text stays at 7:1 too, on white and on the grey panels: a dark grey in place of the\n   half-strength mix the base pins and of the half opacity the Filters pane's Apply button takes. */\nhtml[data-pbt-skin=\"ink\"][data-theme] { --colorNeutralForegroundDisabled: #525252 !important; --colorNeutralForegroundDisabled2: #525252 !important; --colorNeutralForeground4: #525252 !important; }\nhtml[data-pbt-skin=\"ink\"] .outspacePane button.applyFilter:disabled { opacity: 1 !important; color: #525252 !important; }\n\n/* Title bar and tabs: a black rule under each row; text a size up. */\nhtml[data-pbt-skin=\"ink\"] div.titlebar { border-bottom: 2px solid #000; }\nhtml[data-pbt-skin=\"ink\"] div.titlebar .title { font-weight: 700 !important; }\nhtml[data-pbt-skin=\"ink\"] #tablist button[role=\"tab\"] { font-size: 14px !important; font-weight: 600; }\nhtml[data-pbt-skin=\"ink\"] #tablist button[role=\"tab\"]::after { display: none !important; }\nhtml[data-pbt-skin=\"ink\"] #tablist button[role=\"tab\"]:hover:not([aria-selected=\"true\"]) { box-shadow: inset 0 0 0 2px #000; }\nhtml[data-pbt-skin=\"ink\"] [id=\"Ribbon-file\"] { box-shadow: inset 0 0 0 2px #000; font-size: 14px !important; font-weight: 700; }\nhtml[data-pbt-skin=\"ink\"] [id=\"Ribbon-file\"] * { font-size: 14px !important; }\n\n/* Ribbon: a plain white panel inside a 2 px black frame; group labels a size up, divided by rules. */\nhtml[data-pbt-skin=\"ink\"] .ribbonContainer .bottomBarContainer { border-radius: 0 !important; box-shadow: none !important; }\nhtml[data-pbt-skin=\"ink\"] .ribbonContainer [role=\"group\"] label[class*=\"groupLabel\"] { font-size: 12px !important; font-weight: 600; }\nhtml[data-pbt-skin=\"ink\"] .ribbonContainer [class*=\"groupContainer\"] + [class*=\"groupContainer\"] { border-left: 2px solid #000 !important; }\nhtml[data-pbt-skin=\"ink\"] .ribbonCo)PBT"
LR"PBT(ntainer .bottomBarContainer button:not(:disabled):not(.is-disabled):not(.pbt-rbtn):hover { box-shadow: inset 0 0 0 2px #000 !important; }\n\n/* Panes: 2 px rules; titles a size up. */\nhtml[data-pbt-skin=\"ink\"] :is(.sidePane, .outspacePane) { border-left: 2px solid #000 !important; }\nhtml[data-pbt-skin=\"ink\"] :is(.pane-header, .outspacePane .paneHeader) { border-bottom: 2px solid #000; }\nhtml[data-pbt-skin=\"ink\"] :is(.pane-header, .outspacePane .paneHeader) :is(h2, .pane-title) { font-size: 16px !important; font-weight: 700 !important; }\nhtml[data-pbt-skin=\"ink\"] .viewNavigatorContainer { box-shadow: inset -2px 0 0 #000; }\nhtml[data-pbt-skin=\"ink\"] view-navigator button[aria-selected=\"true\"] { box-shadow: none; }\n\n/* Page tabs: 2 px outlined boxes; the open one inverted. Status bar under a rule. */\nhtml[data-pbt-skin=\"ink\"] .carouselScrollPane li.section, html[data-pbt-skin=\"ink\"] button.section.static.create .thumbnail-container { box-shadow: inset 0 0 0 2px #000 !important; }\nhtml[data-pbt-skin=\"ink\"] .carouselScrollPane li.section * { font-size: 13px !important; }\nhtml[data-pbt-skin=\"ink\"] section.statusBar { border-top: 2px solid #000; font-size: 13px; }\n\n/* Our own UI a size up, square, with a 2 px black edge. */\nhtml[data-pbt-skin=\"ink\"] :is(.pbt-menu, .pbt-help) { font-size: 14px; border-width: 2px !important; }\nhtml[data-pbt-skin=\"ink\"] .pbt-tip { font-size: 13px; border-width: 2px !important; }\nhtml[data-pbt-skin=\"ink\"] .pbt-toast { border: 2px solid #000; border-left-width: 6px; box-shadow: none; }\nhtml[data-pbt-skin=\"ink\"] .pbt-menu button:hover { background: #000 !important; color: #fff; --pbt-ui-fg: #fff; --pbt-accent: #ffd400; }\nhtml[data-pbt-skin=\"ink\"] .pbt-chip:is(:hover, :focus-visible) { background: #000 !important; color: #fff !important; }\n\nhtml[data-pbt-skin=\"ink\"] ::-webkit-scrollbar { width: 14px; height: 14px; background: #fff; }\nhtml[data-pbt-skin=\"ink\"] ::-webkit-scrollbar-thumb { background: #000; border: 3px solid #fff; }\n\n/* Window controls: black outlined squares inside Power BI's own button boxes; hover inverts. */\nhtml[data-pbt-skin=\"ink\"] {\n  --pbt-wc-inset: 5px 6px; --pbt-wc-radius: 0; --pbt-wc-fg: #000; --pbt-wc-close-fg: #000;\n  --pbt-wc-ring: inset 0 0 0 2px #000; --pbt-wc-hover-face: #000; --pbt-wc-close-hover-face: #b00020; --pbt-wc-close-hover-fg: #fff;\n  --pbt-wc-hover-bg: transparent; --pbt-wc-close-hover-bg: transparent;\n}\nhtml[data-pbt-skin=\"ink\"] .windowControls button:hover { color: #fff !important; }\n"},"graphpaper":{"short":"Paper","name":"Graph Paper","order":11,"splash":["Graph Paper","Sharp pencil, red pen, a fresh pad."],"confetti":{"shapes":["square","streak"],"colors":["#c8102e","#3a6aa8","#45484d","#7fa7d9"]},"sound":{"wave":"triangle","notes":[523,659,784],"step":0.09,"len":0.18},"palette":{"ramp":[["#000000","#f3eee1"],["#141414","#f7f3e8"],["#292929","#fffdf7"],["#3d3d3d","#e4dccb"],["#5c5c5c","#b5ab98"],["#8f8f8f","#66625a"],["#d6d6d6","#45484d"],["#ffffff","#33363a"]],"accent":"#c8102e","vars":{"font":"\"Segoe UI\", sans-serif","title-font":"\"Segoe Print\", \"Segoe UI\", sans-serif","titlebar-bg":"#efe9da","titlebar-fg":"#33363a","titlebar-hover":"rgba(58, 106, 168, .14)","icon":"#33363a","icon-alt":"#66625a","icon-fill":"#fffdf7","search-bg":"#fffdf7","search-fg":"#33363a","tabs-bg":"#efe9da","tab-fg":"#45484d","tab-active-bg":"#fffdf7","tab-active-fg":"#b00d26","tab-hover-bg":"rgba(58, 106, 168, .12)","file-bg":"#fffdf7","file-fg":"#b00d26","ribbon-bg":"#fffdf7","ribbon-border":"1px solid #c9d8ea","group-fg":"#3a6aa8","nav-bg":"#efe9da","icon-nav":"#45484d","icon-active":"#b00d26","nav-active-bg":"rgba(200, 16, 46, .08)","nav-hover-bg":"rgba(58, 106, 168, .12)","pagetabs-bg":"#efe9da","pagetab-bg":"#f7f3e8","pagetab-fg":"#45484d","pagetab-active-bg":"#fffdf7","pagetab-active-fg":"#b00d26","panehead-bg":"#efe9da","panehead-solid":"#efe9da","panehead-fg":"#33363a","status-bg":"#efe9da","st)PBT"
LR"PBT(atus-fg":"#45484d","gutter":"#e6dfcd","ui-bg":"#fffdf7","ui-fg":"#33363a","ui-border":"#b5ab98","ui-font":"\"Segoe UI\", sans-serif","ui-radius":"2px","hover-bg":"rgba(58, 106, 168, .12)","hover-ring":"#3a6aa8","hover-radius":"2px","avatar-bg":"transparent"}},"css":"/* Graph Paper: warm off-white paper, pencil-grey text, a red pen for whatever is chosen, and a\n   faint blue engineering grid in the panes. The grid is paper, never a control: inputs, lists,\n   field wells, filter cards and the visual icons sit on plain sheets above it, so its lines never\n   read as separators or input edges. Flat; no gradients on the chrome. */\nhtml[data-pbt-skin=\"graphpaper\"] {\n  --pbt-gp-sheet: #fffdf7;\n  --pbt-gp-grid:\n    linear-gradient(rgba(58, 106, 168, .2) 1px, transparent 1px) 0 0 / 80px 80px,\n    linear-gradient(90deg, rgba(58, 106, 168, .2) 1px, transparent 1px) 0 0 / 80px 80px,\n    linear-gradient(rgba(58, 106, 168, .09) 1px, transparent 1px) 0 0 / 16px 16px,\n    linear-gradient(90deg, rgba(58, 106, 168, .09) 1px, transparent 1px) 0 0 / 16px 16px;\n}\n\n/* Title: handwritten. */\nhtml[data-pbt-skin=\"graphpaper\"] div.titlebar { border-bottom: 1px solid #c9d8ea; }\nhtml[data-pbt-skin=\"graphpaper\"] div.titlebar .title { font: 15px \"Segoe Print\", \"Segoe UI\", sans-serif !important; }\n\n/* Tabs: the open one underlined in red pen. File is written in red inside a pencil box. */\nhtml[data-pbt-skin=\"graphpaper\"] #tablist button[role=\"tab\"]::after { display: none !important; }\nhtml[data-pbt-skin=\"graphpaper\"] #tablist button[role=\"tab\"][aria-selected=\"true\"] { box-shadow: inset 0 -2px 0 #c8102e, inset 1px 0 0 #c9d8ea, inset -1px 0 0 #c9d8ea; font-weight: 600; }\nhtml[data-pbt-skin=\"graphpaper\"] [id=\"Ribbon-file\"] { box-shadow: inset 0 0 0 1px #b00d26; font-weight: 600; }\n\n/* Ribbon: a clean sheet with blue rules between groups. */\nhtml[data-pbt-skin=\"graphpaper\"] .ribbonContainer .bottomBarContainer { border-radius: 2px !important; }\nhtml[data-pbt-skin=\"graphpaper\"] .ribbonContainer [class*=\"groupContainer\"] + [class*=\"groupContainer\"] { border-left: 1px solid #c9d8ea !important; }\nhtml[data-pbt-skin=\"graphpaper\"] .ribbonContainer svg .OfficeIconColors_m22 { fill: #45484d !important; }\n\n/* Panes: the grid on the pane itself, a red margin rule under each title. */\nhtml[data-pbt-skin=\"graphpaper\"] .sidePane,\nhtml[data-pbt-skin=\"graphpaper\"]:not(.pbt-report-filters) .outspacePane { background: var(--pbt-gp-grid), #f5f1e5 !important; }\nhtml[data-pbt-skin=\"graphpaper\"] :is(.sidePane, .outspacePane) { border-left: 1px solid #c9d8ea !important; }\nhtml[data-pbt-skin=\"graphpaper\"] :is(.pane-header, .outspacePane .paneHeader) { border-bottom: 1px solid rgba(200, 16, 46, .55); }\nhtml[data-pbt-skin=\"graphpaper\"] :is(.pane-header, .outspacePane .paneHeader) :is(h2, .pane-title) { font-weight: 600 !important; }\n/* Pane headers and the Filters pane's label rows (\"Filters on this visual\") on plain paper, so no\n   grid line runs through their words. */\nhtml[data-pbt-skin=\"graphpaper\"] :is(.pane-header, .outspacePane .paneHeader) { background-image: none !important; }\nhtml[data-pbt-skin=\"graphpaper\"]:not(.pbt-report-filters) .outspacePane section.bucket > .header { background-color: #f5f1e5 !important; box-shadow: 0 0 0 4px #f5f1e5; }\n/* Plain sheets for everything a person clicks or types in, so the grid only shows on empty paper.\n   Hovered and chosen items keep their own highlight. */\nhtml[data-pbt-skin=\"graphpaper\"] .sidePane :is(input, .field-list-search-container, .visual-types-container, field-well-bucket-modern, li.fieldWellProperty, cdk-tree-node, [role=\"tablist\"], [role=\"textbox\"], [role=\"combobox\"], [role=\"switch\"]):not(:hover):not([aria-selected=\"true\"]):not([aria-checked=\"true\"]),\nhtml[data-pbt-skin=\"graphpaper\"]:not(.pbt-report-filters) .outspacePane :is(.card, mat-form-field.searchBox, search-box-modern, input, mat-select):not(:hover) {\n)PBT"
LR"PBT(  background-color: var(--pbt-gp-sheet) !important;\n}\nhtml[data-pbt-skin=\"graphpaper\"] .sidePane .visual-types-container { box-shadow: 0 0 0 4px var(--pbt-gp-sheet); }\n\n/* Page tabs: index cards; the open one underlined in red. */\nhtml[data-pbt-skin=\"graphpaper\"] .carouselScrollPane li.section, html[data-pbt-skin=\"graphpaper\"] button.section.static.create .thumbnail-container { box-shadow: inset 0 0 0 1px #c9d8ea !important; }\nhtml[data-pbt-skin=\"graphpaper\"] .carouselScrollPane li.section.selected { box-shadow: inset 0 0 0 1px #c9d8ea, inset 0 -2px 0 #c8102e !important; }\nhtml[data-pbt-skin=\"graphpaper\"] section.statusBar { border-top: 1px solid #c9d8ea; }\n\n/* Our own UI: a sheet of the same paper, red pen headings. */\nhtml[data-pbt-skin=\"graphpaper\"] .pbt-menu h4 { color: #b00d26; opacity: 1; }\nhtml[data-pbt-skin=\"graphpaper\"] .pbt-toast { border-left-color: #c8102e; }\nhtml[data-pbt-skin=\"graphpaper\"] .pbt-splash > div { background: var(--pbt-gp-grid), #fffdf7; color: #33363a; }\n\nhtml[data-pbt-skin=\"graphpaper\"] ::-webkit-scrollbar { width: 12px; height: 12px; background: #f3eee1; }\nhtml[data-pbt-skin=\"graphpaper\"] ::-webkit-scrollbar-thumb { background: #c4bba8; border: 3px solid #f3eee1; border-radius: 6px; }\n\n/* Window controls: pencil outlines inside Power BI's own button boxes; Close turns red pen. */\nhtml[data-pbt-skin=\"graphpaper\"] {\n  --pbt-wc-inset: 5px 6px; --pbt-wc-radius: 2px; --pbt-wc-fg: #33363a; --pbt-wc-close-fg: #33363a;\n  --pbt-wc-ring: inset 0 0 0 1px #b5ab98; --pbt-wc-hover-face: rgba(58, 106, 168, .14); --pbt-wc-close-hover-face: #c8102e; --pbt-wc-close-hover-fg: #fff;\n  --pbt-wc-hover-bg: transparent; --pbt-wc-close-hover-bg: transparent;\n}\n\n/* Disabled Undo and Redo: pencil grey at the base opacity sits at the edge of visibility. */\nhtml[data-pbt-skin=\"graphpaper\"] titlebar-toolbar button.is-disabled { opacity: .86; }\n"},"orchid":{"short":"Midnight","name":"Midnight Orchid","order":12,"splash":["Midnight Orchid","Night shift. Pink and purple on deep blue-grey."],"confetti":{"shapes":["ring","square"],"colors":["#ff79c6","#bd93f9","#8be9fd","#50fa7b","#f1fa8c"]},"sound":{"wave":"sine","notes":[587,740,880,1175],"step":0.1,"len":0.3},"palette":{"ramp":[["#000000","#21222c"],["#141414","#282a36"],["#292929","#343746"],["#3d3d3d","#44475a"],["#5c5c5c","#565a72"],["#8f8f8f","#a4abcf"],["#d6d6d6","#e6e6e0"],["#ffffff","#f8f8f2"]],"accent":"#bd93f9","vars":{"font":"\"Segoe UI\", sans-serif","title-font":"\"Segoe UI\", sans-serif","titlebar-bg":"#191a21","titlebar-fg":"#f8f8f2","titlebar-hover":"rgba(189, 147, 249, .18)","icon":"#f8f8f2","icon-alt":"#bd93f9","search-bg":"#282a36","search-fg":"#f8f8f2","tabs-bg":"#191a21","tab-fg":"#d0d2e4","tab-active-bg":"#282a36","tab-active-fg":"#ff79c6","tab-hover-bg":"rgba(98, 114, 164, .3)","file-bg":"#bd93f9","file-fg":"#191a21","ribbon-bg":"#282a36","ribbon-border":"1px solid #44475a","group-fg":"#bd93f9","nav-bg":"#21222c","icon-nav":"#a4abcf","icon-active":"#ff79c6","nav-active-bg":"rgba(255, 121, 198, .12)","nav-hover-bg":"rgba(98, 114, 164, .3)","pagetabs-bg":"#21222c","pagetab-bg":"#282a36","pagetab-fg":"#d0d2e4","pagetab-active-bg":"#44475a","pagetab-active-fg":"#f8f8f2","panehead-bg":"#21222c","panehead-solid":"#21222c","panehead-fg":"#bd93f9","status-bg":"#191a21","status-fg":"#d0d2e4","gutter":"radial-gradient(120% 90% at 50% 0%, #2e3040, #191a21 75%)","ui-bg":"#282a36","ui-fg":"#f8f8f2","ui-border":"#6272a4","ui-font":"\"Segoe UI\", sans-serif","ui-radius":"6px","hover-bg":"rgba(189, 147, 249, .16)","hover-ring":"#bd93f9","hover-radius":"4px","avatar-bg":"rgba(189, 147, 249, .15)"}},"css":"/* Midnight Orchid: a deep blue-grey night (#282A36) with soft white text, purple for structure\n   (group labels, pane titles, File) and pink for whatever is open or chosen. Flat panels, rounded\n   corners, no texture. */\n\n/* Title: a pink dot before the file name. */\nhtml[data-pbt-skin=\"orchid\"] div.title)PBT"
LR"PBT(bar { border-bottom: 1px solid #44475a; }\nhtml[data-pbt-skin=\"orchid\"] div.titlebar .title { font-weight: 600 !important; }\nhtml[data-pbt-skin=\"orchid\"] div.titlebar .title::before { content: \"\"; display: inline-block; width: 8px; height: 8px; margin-right: 8px; border-radius: 50%; background: #ff79c6; vertical-align: 1px; }\n\n/* Tabs: the open one with a pink underline. File is a purple pill. */\nhtml[data-pbt-skin=\"orchid\"] #tablist button[role=\"tab\"]::after { display: none !important; }\nhtml[data-pbt-skin=\"orchid\"] #tablist button[role=\"tab\"] { border-radius: 6px 6px 0 0; }\nhtml[data-pbt-skin=\"orchid\"] #tablist button[role=\"tab\"][aria-selected=\"true\"] { box-shadow: inset 0 -2px 0 #ff79c6; font-weight: 600; }\nhtml[data-pbt-skin=\"orchid\"] [id=\"Ribbon-file\"] { border-radius: 6px !important; margin-right: 4px !important; font-weight: 600; }\n\n/* Ribbon: a rounded panel; group rules in the comment blue-grey. */\nhtml[data-pbt-skin=\"orchid\"] .ribbonContainer .bottomBarContainer { border-radius: 8px !important; }\nhtml[data-pbt-skin=\"orchid\"] .ribbonContainer [class*=\"groupContainer\"] + [class*=\"groupContainer\"] { border-left: 1px solid #44475a !important; }\n\n/* Panes: a quiet edge; titles purple with a thin pink rule. */\nhtml[data-pbt-skin=\"orchid\"] :is(.sidePane, .outspacePane) { border-left: 1px solid #44475a !important; }\nhtml[data-pbt-skin=\"orchid\"] :is(.pane-header, .outspacePane .paneHeader) { border-bottom: 1px solid rgba(255, 121, 198, .45); }\nhtml[data-pbt-skin=\"orchid\"] :is(.pane-header, .outspacePane .paneHeader) :is(h2, .pane-title) { font-weight: 600 !important; }\nhtml[data-pbt-skin=\"orchid\"] view-navigator button[aria-selected=\"true\"] { box-shadow: inset 3px 0 0 #ff79c6; }\n\n/* Page tabs: rounded tops; the open one marked in pink. */\nhtml[data-pbt-skin=\"orchid\"] .carouselScrollPane li.section, html[data-pbt-skin=\"orchid\"] button.section.static.create .thumbnail-container { border-radius: 6px 6px 0 0 !important; }\nhtml[data-pbt-skin=\"orchid\"] .carouselScrollPane li.section.selected { box-shadow: inset 0 -2px 0 #ff79c6 !important; }\nhtml[data-pbt-skin=\"orchid\"] section.statusBar { border-top: 1px solid #44475a; }\n\nhtml[data-pbt-skin=\"orchid\"] .pbt-menu h4 { color: #bd93f9; opacity: 1; }\nhtml[data-pbt-skin=\"orchid\"] .pbt-toast { border-left-color: #ff79c6; }\nhtml[data-pbt-skin=\"orchid\"] ::-webkit-scrollbar { width: 10px; height: 10px; background: #21222c; }\nhtml[data-pbt-skin=\"orchid\"] ::-webkit-scrollbar-thumb { background: #44475a; border-radius: 5px; }\nhtml[data-pbt-skin=\"orchid\"] ::-webkit-scrollbar-thumb:hover { background: #6272a4; }\n\n/* Window controls: soft rounded faces inside Power BI's own button boxes; Close turns red. */\nhtml[data-pbt-skin=\"orchid\"] {\n  --pbt-wc-inset: 6px 8px; --pbt-wc-radius: 6px; --pbt-wc-fg: #f8f8f2; --pbt-wc-close-fg: #f8f8f2;\n  --pbt-wc-hover-face: #44475a; --pbt-wc-close-hover-face: #ff5555; --pbt-wc-close-hover-fg: #191a21;\n  --pbt-wc-hover-bg: transparent; --pbt-wc-close-hover-bg: transparent;\n}\n"},"frost":{"short":"Frost","name":"Arctic Frost","order":13,"splash":["Arctic Frost","Polar night, snow-white text, frost-blue edges."],"confetti":{"shapes":["crystal","square"],"colors":["#88c0d0","#81a1c1","#eceff4","#8fbcbb","#b48ead"]},"sound":{"wave":"sine","notes":[988,1319,1568],"step":0.11,"len":0.4},"palette":{"ramp":[["#000000","#272c36"],["#141414","#2e3440"],["#292929","#3b4252"],["#3d3d3d","#434c5e"],["#5c5c5c","#4c566a"],["#8f8f8f","#aab4c8"],["#d6d6d6","#d8dee9"],["#ffffff","#eceff4"]],"accent":"#88c0d0","vars":{"font":"\"Segoe UI\", sans-serif","title-font":"\"Segoe UI\", sans-serif","titlebar-bg":"#242933","titlebar-fg":"#eceff4","titlebar-hover":"rgba(136, 192, 208, .16)","icon":"#d8dee9","icon-alt":"#88c0d0","search-bg":"#2e3440","search-fg":"#eceff4","tabs-bg":"#242933","tab-fg":"#d8dee9","tab-active-bg":"#2e3440","tab-active-fg":"#88c0d0","tab-hover-bg":"rgba(136, )PBT"
LR"PBT(192, 208, .12)","file-bg":"#88c0d0","file-fg":"#242933","ribbon-bg":"#2e3440","ribbon-border":"1px solid #434c5e","group-fg":"#88c0d0","nav-bg":"#242933","icon-nav":"#aab4c8","icon-active":"#88c0d0","nav-active-bg":"rgba(136, 192, 208, .14)","nav-hover-bg":"rgba(136, 192, 208, .12)","pagetabs-bg":"#242933","pagetab-bg":"#2e3440","pagetab-fg":"#d8dee9","pagetab-active-bg":"#3b4252","pagetab-active-fg":"#eceff4","panehead-bg":"#2e3440","panehead-solid":"#2e3440","panehead-fg":"#eceff4","status-bg":"#242933","status-fg":"#d8dee9","gutter":"radial-gradient(120% 80% at 80% 0%, #354152, #242933 70%)","ui-bg":"#2e3440","ui-fg":"#eceff4","ui-border":"#4c566a","ui-font":"\"Segoe UI\", sans-serif","ui-radius":"4px","hover-bg":"rgba(136, 192, 208, .14)","hover-ring":"#88c0d0","hover-radius":"4px","avatar-bg":"rgba(136, 192, 208, .12)"}},"css":"/* Arctic Frost: polar-night panes (#2E3440), snow-white text, frost blue (#88C0D0) for whatever is\n   open or chosen and for the structure, a thin frost line along the top of each panel. Flat and\n   quiet; no texture. */\n\n/* Title bar: a frost line along the bottom. */\nhtml[data-pbt-skin=\"frost\"] div.titlebar { border-bottom: 1px solid #3b4252; box-shadow: inset 0 -1px 0 rgba(136, 192, 208, .25); }\nhtml[data-pbt-skin=\"frost\"] div.titlebar .title { font-weight: 600 !important; letter-spacing: .02em; }\n\n/* Tabs: the open one with a frost-blue underline. File is a frost-blue key. */\nhtml[data-pbt-skin=\"frost\"] #tablist button[role=\"tab\"]::after { display: none !important; }\nhtml[data-pbt-skin=\"frost\"] #tablist button[role=\"tab\"][aria-selected=\"true\"] { box-shadow: inset 0 -2px 0 #88c0d0; font-weight: 600; }\nhtml[data-pbt-skin=\"frost\"] [id=\"Ribbon-file\"] { border-radius: 4px !important; margin-right: 4px !important; font-weight: 600; }\n\n/* Ribbon: a panel with a frost line along its top edge. */\nhtml[data-pbt-skin=\"frost\"] .ribbonContainer .bottomBarContainer { border-radius: 6px !important; box-shadow: inset 0 1px 0 rgba(136, 192, 208, .35); }\nhtml[data-pbt-skin=\"frost\"] .ribbonContainer [class*=\"groupContainer\"] + [class*=\"groupContainer\"] { border-left: 1px solid #434c5e !important; }\n\n/* Panes: a frost line at the top, a quiet edge. */\nhtml[data-pbt-skin=\"frost\"] :is(.sidePane, .outspacePane) { border-left: 1px solid #3b4252 !important; }\nhtml[data-pbt-skin=\"frost\"] :is(.pane-header, .outspacePane .paneHeader) { border-top: 2px solid #88c0d0; border-bottom: 1px solid #3b4252; }\nhtml[data-pbt-skin=\"frost\"] :is(.pane-header, .outspacePane .paneHeader) :is(h2, .pane-title) { font-weight: 600 !important; }\n\n/* Page tabs: the open one marked in frost blue. */\nhtml[data-pbt-skin=\"frost\"] .carouselScrollPane li.section, html[data-pbt-skin=\"frost\"] button.section.static.create .thumbnail-container { border-radius: 4px 4px 0 0 !important; }\nhtml[data-pbt-skin=\"frost\"] .carouselScrollPane li.section.selected { box-shadow: inset 0 -2px 0 #88c0d0 !important; }\nhtml[data-pbt-skin=\"frost\"] section.statusBar { border-top: 1px solid #3b4252; }\n\nhtml[data-pbt-skin=\"frost\"] .pbt-menu h4 { color: #88c0d0; opacity: 1; }\nhtml[data-pbt-skin=\"frost\"] .pbt-toast { border-left-color: #88c0d0; }\nhtml[data-pbt-skin=\"frost\"] ::-webkit-scrollbar { width: 10px; height: 10px; background: #272c36; }\nhtml[data-pbt-skin=\"frost\"] ::-webkit-scrollbar-thumb { background: #434c5e; border-radius: 5px; }\nhtml[data-pbt-skin=\"frost\"] ::-webkit-scrollbar-thumb:hover { background: #5e81ac; }\n\n/* Window controls: frosted faces inside Power BI's own button boxes; Close turns aurora red. */\nhtml[data-pbt-skin=\"frost\"] {\n  --pbt-wc-inset: 6px 8px; --pbt-wc-radius: 4px; --pbt-wc-fg: #eceff4; --pbt-wc-close-fg: #eceff4;\n  --pbt-wc-hover-face: #3b4252; --pbt-wc-close-hover-face: #bf616a; --pbt-wc-close-hover-fg: #eceff4;\n  --pbt-wc-hover-bg: transparent; --pbt-wc-close-hover-bg: transparent;\n}\n"}};
const PBT_BASE_CSS = "/* Skins & Tools for Power BI, by Reid Havens, Analytic Endeavors. Licensed CC BY-ND 4.0.\n   Fingerprint: AE-PBT-29A1ED34-2026 (also --ae-fp below; nothing reads it).\n   Shared by every skin, and by our own UI when no skin is on.\n   Our UI reads --pbt-* variables; each skin's palette.vars sets its own. */\n:root {\n  --ae-fp: \"AE-PBT-29A1ED34-2026\";\n  --pbt-ui-bg: #1f1f1f;\n  --pbt-ui-fg: #f0f0f0;\n  --pbt-ui-border: #444;\n  --pbt-ui-font: \"Segoe UI\", sans-serif;\n  --pbt-ui-radius: 6px;\n  --pbt-accent: #2aac94;\n  --pbt-ui-shadow: 0 8px 28px rgba(0, 0, 0, .45);\n}\n\n/* Chrome font: everything outside the report page, except icon fonts. Elements that draw an icon\n   glyph through a pseudo-element are named by what they are (arrow, chevron, caret) rather than\n   \"icon\" or \"glyph\"; the dropdown arrow in the Filters pane became a missing-glyph box when the\n   font rule reached it. */\nhtml[data-pbt-skin] :is(titlebar, ribbon-report-view, view-navigator, .sidePane, .outspacePane, status-bar, exploration-footer-modern, .cdk-overlay-container, .ms-Layer)\n  :is(span, div, button, label, h1, h2, h3, h4, input, li, a, p):not([class*=\"glyph\" i]):not([class*=\"icon\" i]):not([class*=\"arrow\" i]):not([class*=\"chevron\" i]):not([class*=\"caret\" i]):not(i):not(.zoomButton):not(.displayArea *) {\n  font-family: var(--pbt-font, inherit) !important;\n}\n\n/* ---- shared skin hooks ----\n   Each reads a --pbt-* variable a skin may set in palette.vars; unset ones fall back to\n   whatever the color remap produced, so a skin only names what it wants to change. */\nhtml[data-pbt-skin] div.titlebar {\n  background: var(--pbt-titlebar-bg, var(--titlebar-background)) !important;\n  color: var(--pbt-titlebar-fg, var(--titlebar-foreground)) !important;\n}\nhtml[data-pbt-skin] div.titlebar :is(.title, .timestamp, .dotSeparator, .accountNameLabel, .caretdownIcon, .titlebarControl) {\n  color: var(--pbt-titlebar-fg, inherit) !important;\n}\n/* Power BI's own tooltips (Fluent v9) carry a fixed charcoal or whi)PBT"
LR"PBT(te face per mode. */\nhtml[data-pbt-skin] .fui-Tooltip__content {\n  background: var(--pbt-ui-bg) !important; color: var(--pbt-ui-fg) !important; border: 1px solid var(--pbt-ui-border) !important;\n  border-radius: var(--pbt-ui-radius) !important; box-shadow: var(--pbt-ui-shadow) !important; font-family: var(--pbt-ui-font) !important;\n}\nhtml[data-pbt-skin] .fui-Tooltip__content * { color: inherit !important; }\n/* Quick access icons (Save, Undo, Redo) are Office SVGs with fills fixed for a dark title bar.\n   Repaint them from the skin so they read on any title bar. */\nhtml[data-pbt-skin] titlebar-toolbar button { color: var(--pbt-icon, var(--pbt-titlebar-fg)) !important; }\nhtml[data-pbt-skin] titlebar-toolbar svg .OfficeIconColors_HighContrast,\nhtml[data-pbt-skin] titlebar-toolbar svg .OfficeIconColors_DynamicColor { fill: var(--pbt-icon, var(--pbt-titlebar-fg, currentColor)) !important; }\nhtml[data-pbt-skin] titlebar-toolbar svg .OfficeIconColors_m22 { fill: var(--pbt-icon-alt, var(--pbt-icon, currentColor)) !important; }\nhtml[data-pbt-skin] titlebar-toolbar svg .OfficeIconColors_m20 { fill: var(--pbt-icon-fill, transparent) !important; }\nhtml[data-pbt-skin] titlebar-toolbar button.is-disabled { opacity: .72; }\n/* Office icons everywhere else in the chrome: their fill class (m20) is transparent in Power BI's\n   dark mode and white in its light mode, so under a skin the same icon drew hollow in one mode\n   and filled in the other. Pinned here; light skins set --pbt-icon-fill to white. */\nhtml[data-pbt-skin] :is(.ribbonContainer, div.ribbonSearchOuter, .sidePane, .outspacePane, section.statusBar, exploration-footer-modern) svg .OfficeIconColors_m20 { fill: var(--pbt-icon-fill, transparent) !important; }\n/* Their line greys (m22 the main stroke, m23 the secondary, m21 the light one, m252 black) are\n   fixed per mode as well and sat too close to a dark skin's ground. They become shades of the\n   skin's text color (--pbt-icon-line for the main stroke), lighter on a dark skin, darker on a\n   light one; the \"d\" classes are the same strokes on disabled buttons. */\nhtml[data-pbt-skin] { --pbt-icon-line-default: color-mix(in srgb, var(--pbt-ribbon-fg, var(--colorNeutralForeground1)) 82%, transparent); }\nhtml[data-pbt-skin] :is(.ribbonContainer, div.ribbonSearchOuter, .sidePane, .outspacePane, section.statusBar, exploration-footer-modern) svg :is(.OfficeIconColors_m22, .OfficeIconColors_m252) { fill: var(--pbt-icon-line, var(--pbt-icon-line-default)) !important; }\nhtml[data-pbt-skin] :is(.ribbonContainer, div.ribbonSearchOuter, .sidePane, .outspacePane, section.statusBar, exploration-footer-modern) svg .OfficeIconColors_m23 { fill: color-mix(in srgb, var(--pbt-icon-line, var(--pbt-icon-line-default)) 72%, transparent) !important; }\nhtml[data-pbt-skin] :is(.ribbonContainer, div.ribbonSearchOuter, .sidePane, .outspacePane, section.statusBar, exploration-footer-modern) svg .OfficeIconColors_m21 { fill: color-mix(in srgb, var(--pbt-icon-line, var(--pbt-icon-line-default)) 40%, transparent) !important; }\nhtml[data-pbt-skin] :is(.ribbonContainer, .sidePane, .outspacePane) svg :is(.OfficeIconColors_d22, .OfficeIconColors_d252, .OfficeIconColors_d30, .OfficeIconColors_d31, .OfficeIconColors_d32) { fill: color-mix(in srgb, var(--pbt-icon-line, var(--pbt-icon-line-default)) 55%, transparent) !important; }\nhtml[data-pbt-skin] :is(.ribbonContainer, .sidePane, .outspacePane) svg :is(.OfficeIconColors_d23, .OfficeIconColors_d24, .OfficeIconColors_d210, .OfficeIconColors_d211) { fill: color-mix(in srgb, var(--pbt-icon-line, var(--pbt-icon-line-default)) 40%, transparent) !important; }\nhtml[data-pbt-skin] :is(.ribbonContainer, .sidePane, .outspacePane) svg :is(.OfficeIconColors_d20, .OfficeIconColors_d21, .OfficeIconColors_d212) { fill: var(--pbt-icon-fill, transparent) !important; }\nhtml[data-pbt-skin] titlebar-toolbar button:not(.is-disabled):hover { background: var(--pbt-titlebar-hover, rgba(255, 255, 255, .18))PBT"
LR"PBT() !important; }\n/* Window controls keep Power BI's exact size and position. Windows decides what a click there\n   means from fixed button rectangles (Snap layouts depend on them), so a resized or shifted\n   button would maximize when you meant to minimize. A skin draws its own look as a face inside\n   the box instead, through these variables:\n     --pbt-wc-inset      where the face sits inside the 44 x 34 box (default 4px 4px, so no face\n                         or hover ever touches the window frame; keep at least 4px each way)\n     --pbt-wc-radius     its corners\n     --pbt-wc-face       its paint; --pbt-wc-close-face for Close\n     --pbt-wc-ring       its box-shadow (bevels, outlines, glow)\n     --pbt-wc-fg         glyph color; --pbt-wc-close-fg for Close\n     --pbt-wc-hover-face the face's paint under the pointer (default: the face, brightened; a\n                         grey tint when there is no face); --pbt-wc-close-hover-face for Close\n                         (default: the close face, else red)\n     --pbt-wc-hover-bg   whole-box hover tint, transparent by default\n     --pbt-wc-glyph      glyph size */\nhtml[data-pbt-skin] .windowControls button {\n  color: var(--pbt-wc-fg, var(--pbt-titlebar-fg, var(--titlebar-foreground))) !important;\n  position: relative; display: inline-grid !important; place-items: center; transition: background-color .12s;\n}\nhtml[data-pbt-skin] .windowControls button.closeButton { color: var(--pbt-wc-close-fg, var(--pbt-wc-fg, var(--pbt-titlebar-fg, var(--titlebar-foreground)))) !important; }\nhtml[data-pbt-skin] .windowControls button::before {\n  content: \"\"; position: absolute; inset: var(--pbt-wc-inset, 4px 4px); pointer-events: none;\n  border-radius: var(--pbt-wc-radius, 3px); background: var(--pbt-wc-face, transparent); box-shadow: var(--pbt-wc-ring, none);\n  transition: filter .12s, box-shadow .12s, background-color .12s;\n}\nhtml[data-pbt-skin] .windowControls button.closeButton::before { background: var(--pbt-wc-close-face, var(--pbt-wc-face, transparent)); }\nhtml[data-pbt-skin] .windowControls button:hover::before { background: var(--pbt-wc-hover-face, var(--pbt-wc-face, var(--pbt-titlebar-hover, rgba(127, 127, 127, .3)))); filter: brightness(1.18); }\nhtml[data-pbt-skin] .windowControls button.closeButton:hover::before { background: var(--pbt-wc-close-hover-face, var(--pbt-wc-close-face, var(--pbt-wc-face, #c42b1c))); }\n/* The button sizes itself around this 16 px box, so the box stays 16 px and only the drawing\n   inside it scales to --pbt-wc-glyph. */\nhtml[data-pbt-skin] .windowControls button pbi-office-icon { position: relative; z-index: 1; width: 16px !important; height: 16px !important; display: grid !important; place-items: center; margin: 0 !important; }\nhtml[data-pbt-skin] .windowControls button pbi-office-icon svg { display: block; width: var(--pbt-wc-glyph, 16px) !important; height: var(--pbt-wc-glyph, 16px) !important; }\nhtml[data-pbt-skin] .windowControls svg [class^=\"OfficeIconColors\"] { fill: currentColor !important; }\nhtml[data-pbt-skin] .windowControls button:hover { background-color: var(--pbt-wc-hover-bg, transparent) !important; }\nhtml[data-pbt-skin] .windowControls button.closeButton:hover { background-color: var(--pbt-wc-close-hover-bg, var(--pbt-wc-hover-bg, transparent)) !important; color: var(--pbt-wc-close-hover-fg, #fff) !important; }\n\n/* Account: avatar and hover follow the title bar instead of Fluent's white. */\nhtml[data-pbt-skin] titlebar account .header { border-radius: 4px; transition: background-color .12s; }\nhtml[data-pbt-skin] titlebar account .header:hover { background: var(--pbt-titlebar-hover, rgba(127, 127, 127, .2)) !important; }\nhtml[data-pbt-skin] titlebar account .imageAvatarIcon { background: var(--pbt-avatar-bg, transparent) !important; color: var(--pbt-icon, var(--pbt-titlebar-fg, currentColor)) !important; box-shadow: 0 0 0 1px currentColor; border-radius: 50%; }\nhtml[data-pbt-skin] titleb)PBT"
LR"PBT(ar account .imageAvatarIcon svg { fill: currentColor; color: inherit; }\n\n/* One hover language for the chrome; skins tune it with --pbt-hover-bg and --pbt-hover-ring. */\nhtml[data-pbt-skin] {\n  --pbt-hover-bg-default: color-mix(in srgb, var(--pbt-accent) 16%, transparent);\n  --pbt-hover-ring-default: color-mix(in srgb, var(--pbt-accent) 70%, transparent);\n}\nhtml[data-pbt-skin] .ribbonContainer .bottomBarContainer button:not(:disabled):not(.is-disabled):not(.pbt-rbtn):hover {\n  background-color: var(--pbt-hover-bg, var(--pbt-hover-bg-default)) !important;\n  box-shadow: inset 0 0 0 1px var(--pbt-hover-ring, var(--pbt-hover-ring-default));\n  border-radius: var(--pbt-hover-radius, 4px) !important;\n}\nhtml[data-pbt-skin] .ribbonContainer .bottomBarContainer button { transition: background-color .12s, box-shadow .12s; }\n/* Skin fonts run wider than Segoe UI; a little side room keeps neighbouring button labels apart. */\nhtml[data-pbt-skin] .ribbonContainer .bottomBarContainer [role=\"group\"] button.ms-ButtonNext { padding-inline: 4px !important; margin-inline: 1px !important; }\nhtml[data-pbt-skin] :is(.carouselScrollPane li.section, .pane-header button, .outspacePane .paneHeader button):hover { filter: brightness(1.08); }\nhtml[data-pbt-skin] .carouselScrollPane li.section:not(.selected):hover { box-shadow: inset 0 -2px 0 var(--pbt-hover-ring, var(--pbt-hover-ring-default)) !important; }\n\n/* ribbon tab row and panel */\nhtml[data-pbt-skin] .ribbonTopBarContainer { background: var(--pbt-tabs-bg, var(--pbt-titlebar-bg, transparent)) !important; }\nhtml[data-pbt-skin] .ribbonTopBarContainer > div,\nhtml[data-pbt-skin] .ribbonTopBarContainer > div > div { background: transparent !important; }\nhtml[data-pbt-skin] #tablist button[role=\"tab\"] {\n  color: var(--pbt-tab-fg, var(--colorNeutralForeground1)) !important; border-radius: var(--pbt-tab-radius, 0) var(--pbt-tab-radius, 0) 0 0;\n}\nhtml[data-pbt-skin] #tablist button[role=\"tab\"] * { color: inherit !important; }\nhtml[data-pbt-skin] #tablist button[role=\"tab\"]:hover { background: var(--pbt-tab-hover-bg, rgba(127, 127, 127, .18)) !important; }\nhtml[data-pbt-skin] #tablist button[role=\"tab\"][aria-selected=\"true\"] {\n  background: var(--pbt-tab-active-bg, transparent) !important; color: var(--pbt-tab-active-fg, var(--colorNeutralForeground1)) !important;\n}\n/* Power BI's sliding underline is placed from the tabs' measured widths, so a skin's font leaves\n   it under the wrong tab (and on top of the skin's own marker). Every skin marks the open tab on\n   the tab itself, so the underline goes. */\nhtml[data-pbt-skin] #selectedPivotIndicator { display: none !important; }\nhtml[data-pbt-skin] [id=\"Ribbon-file\"] {\n  background: var(--pbt-file-bg, transparent) !important; color: var(--pbt-file-fg, var(--pbt-tab-fg, inherit)) !important;\n}\nhtml[data-pbt-skin] [id=\"Ribbon-file\"] * { color: inherit !important; }\nhtml[data-pbt-skin] .ribbonContainer div:has(> .bottomBarContainer) { background: var(--pbt-tabs-bg, var(--pbt-titlebar-bg, transparent)) !important; }\nhtml[data-pbt-skin] .ribbonContainer .bottomBarContainer {\n  background: var(--pbt-ribbon-bg, var(--colorNeutralBackground1)) !important;\n  border: var(--pbt-ribbon-border, 0) !important;\n}\nhtml[data-pbt-skin] .ribbonContainer [role=\"group\"] label[class*=\"groupLabel\"] { color: var(--pbt-group-fg, inherit) !important; }\n/* The title bar search box text and magnifier; skins with a light search field set --pbt-search-fg. */\nhtml[data-pbt-skin] div.ribbonSearchOuter :is(input, svg, span, i) { color: var(--pbt-search-fg, inherit) !important; }\nhtml[data-pbt-skin] div.ribbonSearchOuter input::placeholder { color: var(--pbt-search-fg, inherit) !important; opacity: .8; }\n/* Ribbon buttons sit flat on the panel (textured panels would otherwise show white boxes);\n   pressed toggles such as Filters keep their state color. */\nhtml[data-pbt-skin] .ribbonContainer .bottomBarContainer butto)PBT"
LR"PBT(n:not([aria-pressed=\"true\"]):not([aria-checked=\"true\"]):not(.is-checked):not(:hover):not(.pbt-rbtn) { background-color: transparent !important; background-image: none !important; }\n/* Fluent v8 parts of the ribbon (the External tools buttons, split-button chevrons) bake in text\n   colors for Power BI's own light or dark mode instead of reading the theme variables, so they\n   flipped with the mode underneath a skin. Skins set them directly, so a skin looks the same in\n   either mode. Disabled ones are the skin text at half strength: Power BI's own disabled grey\n   differs by mode and lands at different points on a skin's palette. */\nhtml[data-pbt-skin] .ribbonContainer .bottomBarContainer :is(.ms-Button-label, .ms-Button-textContainer, .ms-Button-description, .ms-Button-menuIcon, .ms-Button-icon) {\n  color: var(--pbt-ribbon-fg, var(--colorNeutralForeground1)) !important;\n}\n/* Fluent v9 buttons (Paste, Cut, New visual calculation, ...) color their text on the button\n   itself, so the disabled color goes on the button and its text inherits it. */\nhtml[data-pbt-skin] .ribbonContainer .bottomBarContainer :is(button:disabled, button[aria-disabled=\"true\"], .is-disabled),\nhtml[data-pbt-skin] .ribbonContainer .bottomBarContainer :is(button:disabled, button[aria-disabled=\"true\"], .is-disabled) :is(.ms-Button-label, .ms-Button-textContainer, .ms-Button-description, .ms-Button-menuIcon, span:not(svg *)) {\n  color: var(--pbt-ribbon-fg-disabled, color-mix(in srgb, var(--pbt-ribbon-fg, var(--colorNeutralForeground1)) 55%, transparent)) !important;\n}\n/* Fluent v8 dropdown menus (ribbon menus, context menus) have the same baked-in colors. */\nhtml[data-pbt-skin] :is(.ms-ContextualMenu, .ms-Callout-main) { background-color: var(--colorNeutralBackground1) !important; color: var(--colorNeutralForeground1) !important; }\nhtml[data-pbt-skin] .ms-ContextualMenu-link { background-color: transparent !important; color: var(--colorNeutralForeground1) !important; }\nhtml[data-pbt-skin] .ms-ContextualMenu-link:hover:not(.is-disabled) { background-color: var(--colorNeutralBackground1Hover, var(--pbt-hover-bg-default)) !important; }\nhtml[data-pbt-skin] .ms-ContextualMenu-link :is(.ms-ContextualMenu-itemText, .ms-ContextualMenu-icon, .ms-ContextualMenu-submenuIcon, .ms-ContextualMenu-secondaryText) { color: inherit !important; }\nhtml[data-pbt-skin] .ms-ContextualMenu-link.is-disabled { color: color-mix(in srgb, var(--colorNeutralForeground1) 55%, transparent) !important; }\nhtml[data-pbt-skin] .ms-ContextualMenu-divider { background-color: var(--colorNeutralStroke2) !important; }\n\n/* The collapse chevron at the panel's bottom right sits in a container with a fixed dark fill. */\nhtml[data-pbt-skin] [class*=\"chevronContainer\"] { background: transparent !important; }\nhtml[data-pbt-skin] #RibbonModeToggle { background: transparent !important; color: var(--pbt-group-fg, var(--colorNeutralForeground1)) !important; border-radius: var(--pbt-hover-radius, 4px) !important; }\nhtml[data-pbt-skin] #RibbonModeToggle svg [class^=\"OfficeIconColors\"] { fill: currentColor !important; }\n\n/* view switcher (Report, Table, Model, DAX, TMDL) */\nhtml[data-pbt-skin] .viewNavigatorContainer { background: var(--pbt-nav-bg, var(--colorPaneBackground)) !important; }\n/* The icons are strokes in currentColor; the buttons' own (remapped) fill would hide the rail behind them. */\nhtml[data-pbt-skin] view-navigator button { color: var(--pbt-icon-nav, var(--pbt-icon, inherit)) !important; background: transparent !important; }\nhtml[data-pbt-skin] view-navigator button:hover { background: var(--pbt-nav-hover-bg, rgba(127, 127, 127, .2)) !important; }\nhtml[data-pbt-skin] view-navigator button[aria-selected=\"true\"] {\n  color: var(--pbt-icon-active, var(--pbt-accent)) !important; background: var(--pbt-nav-active-bg, transparent) !important;\n  box-shadow: inset 3px 0 0 var(--pbt-accent);\n}\n\n/* page tabs */\nhtml[data-pbt-skin] ul.pane.secti)PBT"
LR"PBT(ons, html[data-pbt-skin] .explorationNavigationContent { background: var(--pbt-pagetabs-bg, var(--pane-background-color)) !important; }\nhtml[data-pbt-skin] .carouselScrollPane li.section { background: var(--pbt-pagetab-bg, transparent) !important; border-radius: var(--pbt-tab-radius, 0) var(--pbt-tab-radius, 0) 0 0; }\nhtml[data-pbt-skin] .carouselScrollPane li.section * { color: var(--pbt-pagetab-fg, inherit) !important; }\nhtml[data-pbt-skin] .carouselScrollPane li.section.selected { background: var(--pbt-pagetab-active-bg, var(--section-background-selected-color)) !important; }\nhtml[data-pbt-skin] .carouselScrollPane li.section.selected * { color: var(--pbt-pagetab-active-fg, inherit) !important; }\n/* New page (+) follows the page tabs instead of keeping its accent-filled square. */\nhtml[data-pbt-skin] button.section.static.create { background: transparent !important; color: var(--pbt-pagetab-fg, var(--pbt-tab-fg, var(--colorNeutralForeground1))) !important; }\nhtml[data-pbt-skin] button.section.static.create .thumbnail-container {\n  background: var(--pbt-pagetab-bg, transparent) !important; border-radius: var(--pbt-tab-radius, 0) var(--pbt-tab-radius, 0) 0 0;\n  box-shadow: inset 0 0 0 1px color-mix(in srgb, currentColor 30%, transparent);\n}\nhtml[data-pbt-skin] button.section.static.create:hover .thumbnail-container { background: var(--pbt-hover-bg, var(--pbt-hover-bg-default)) !important; }\n/* The + symbol paints with these variables, not currentColor. */\nhtml[data-pbt-skin] button.section.static.create {\n  --add-new-page-icon-color: currentColor; --add-new-page-icon-color-hover: currentColor; --add-new-page-icon-color-pressed: currentColor;\n}\n\n/* side panes and status bar */\nhtml[data-pbt-skin] .pane-header, html[data-pbt-skin] .outspacePane .paneHeader {\n  background: var(--pbt-panehead-bg, transparent) !important; color: var(--pbt-panehead-fg, inherit) !important;\n}\n/* The Filters pane body keeps the report's colors (often white), so its header gets a solid chrome\n   color under the skin's header paint: the same color the side panes show behind theirs. */\nhtml[data-pbt-skin] .outspacePane .paneHeader { background-color: var(--pbt-panehead-solid, var(--exploreUIPane-colorBackground)) !important; }\n/* Collapsed, the pane is a thin strip with the title turned on its side and running below the\n   header block; the strip shows no report content, so it takes the header paint end to end. */\nhtml[data-pbt-skin] .outspacePane.outspacePaneCollapsed {\n  background: var(--pbt-panehead-bg, transparent) !important;\n  background-color: var(--pbt-panehead-solid, var(--exploreUIPane-colorBackground)) !important;\n}\nhtml[data-pbt-skin] .outspacePane.outspacePaneCollapsed .paneHeader { background: transparent !important; }\n/* Every part of a pane header takes the header color, including the Filters title and glyph,\n   which otherwise keep the report's inline filter-pane text color (dark on a skinned header). */\nhtml[data-pbt-skin] :is(.pane-header, .outspacePane .paneHeader) :is(h2, button, span, div, .glyphicon) { color: var(--pbt-panehead-fg, inherit) !important; }\n/* Pane titles set their own letter case; a skin sets --pbt-panehead-case once and every title follows. */\nhtml[data-pbt-skin] :is(.pane-header, .outspacePane .paneHeader) :is(h2, .pane-title, .sidePaneTitle) { text-transform: var(--pbt-panehead-case, none) !important; letter-spacing: var(--pbt-panehead-tracking, normal) !important; }\n\n/* The Filters pane body: the report sets its colors (white by default), which leaves a white\n   block beside dark skinned panes. Skins paint it like the other panes; the menu's \"Keep the\n   report's Filters pane colors\" (html.pbt-report-filters) shows the report's own formatting. */\nhtml[data-pbt-skin]:not(.pbt-report-filters) .outspacePane {\n  background-color: var(--exploreUIPane-colorBackground) !important; color: var(--colorNeutralForeground1) !important;\n  border-color: var(--pbt-line-so)PBT"
LR"PBT(ft) !important;\n}\nhtml[data-pbt-skin]:not(.pbt-report-filters) .outspacePane .paneContents :is(div, span, section, label, p, h3, h4, button, a):not([role=\"checkbox\"]):not([class*=\"checkbox\" i]):not([class*=\"slider\" i]):not([class*=\"toggle\" i]) {\n  background-color: transparent !important; color: inherit !important;\n}\n/* Borders come from the text color, not a mid-grey token: a mid-grey lands on a different spot of\n   the skin's ramp under Power BI's light and dark modes, while the text end is the same in both. */\nhtml[data-pbt-skin] { --pbt-line: color-mix(in srgb, var(--colorNeutralForeground1) 32%, transparent); --pbt-line-soft: color-mix(in srgb, var(--colorNeutralForeground1) 18%, transparent); }\nhtml[data-pbt-skin]:not(.pbt-report-filters) .outspacePane .paneContents :is([class*=\"card\" i], [class*=\"drop\" i], [class*=\"well\" i]) { border-color: var(--pbt-line) !important; }\nhtml[data-pbt-skin]:not(.pbt-report-filters) .outspacePane input { background: var(--colorNeutralBackground1) !important; color: var(--colorNeutralForeground1) !important; border-color: var(--pbt-line) !important; }\n/* The filter card's controls are Angular Material parts, not plain elements, so they are named:\n   the Filter type and condition dropdowns, their open list (in the overlay layer), and the\n   \"Require single selection\" check box. */\nhtml[data-pbt-skin]:not(.pbt-report-filters) .outspacePane mat-select {\n  background-color: var(--colorNeutralBackground1) !important; color: var(--colorNeutralForeground1) !important; border-color: var(--pbt-line) !important;\n}\nhtml[data-pbt-skin]:not(.pbt-report-filters) .outspacePane mat-select :is(.mat-mdc-select-value, .mat-mdc-select-value-text, .mat-mdc-select-placeholder, .mat-mdc-select-arrow) { color: inherit !important; }\nhtml[data-pbt-skin]:not(.pbt-report-filters) .outspacePane mat-select .mat-mdc-select-placeholder { opacity: .7; }\nhtml[data-pbt-skin]:not(.pbt-report-filters) .cdk-overlay-container .mat-mdc-select-panel {\n  background-color: var(--colorNeutralBackground1) !important; color: var(--colorNeutralForeground1) !important; border: 1px solid var(--pbt-line); box-shadow: var(--pbt-ui-shadow) !important;\n}\nhtml[data-pbt-skin]:not(.pbt-report-filters) .cdk-overlay-container .mat-mdc-option { background-color: transparent !important; color: var(--colorNeutralForeground1) !important; font-family: var(--pbt-font, inherit) !important; }\nhtml[data-pbt-skin]:not(.pbt-report-filters) .cdk-overlay-container .mat-mdc-option :is(.mdc-list-item__primary-text, span) { color: inherit !important; }\nhtml[data-pbt-skin]:not(.pbt-report-filters) .cdk-overlay-container .mat-mdc-option:is(:hover, .mat-mdc-option-active, .mdc-list-item--selected) { background-color: color-mix(in srgb, var(--pbt-accent) 22%, transparent) !important; }\nhtml[data-pbt-skin]:not(.pbt-report-filters) .outspacePane .mdc-checkbox__background { border-color: var(--colorNeutralForeground1) !important; background-color: transparent !important; }\nhtml[data-pbt-skin]:not(.pbt-report-filters) .outspacePane .mdc-checkbox__native-control:checked ~ .mdc-checkbox__background { background-color: var(--pbt-accent) !important; border-color: var(--pbt-accent) !important; }\nhtml[data-pbt-skin]:not(.pbt-report-filters) .outspacePane .mdc-checkbox__checkmark-path { stroke: var(--colorNeutralBackground1) !important; }\nhtml[data-pbt-skin]:not(.pbt-report-filters) .outspacePane .mdc-checkbox__ripple { display: none; }\n/* The search field inside a filter card sits on its own row with the magnifier; a filled input\n   there showed as a lighter block behind the words. */\nhtml[data-pbt-skin]:not(.pbt-report-filters) .outspacePane .filterContent input.searchInput { background: transparent !important; }\n/* The search box sits beside the contents with its own inline report colors. */\nhtml[data-pbt-skin]:not(.pbt-report-filters) .outspacePane search-box-modern {\n  background-color: var(--colorNeutralBackgro)PBT"
LR"PBT(und1) !important; color: var(--colorNeutralForeground1) !important;\n}\nhtml[data-pbt-skin]:not(.pbt-report-filters) .outspacePane search-box-modern * { color: inherit !important; background-color: transparent !important; }\nhtml[data-pbt-skin]:not(.pbt-report-filters) .outspacePane .searchFiltersLineBreak { border-color: var(--colorNeutralStroke2) !important; background: transparent !important; }\nhtml[data-pbt-skin] section.statusBar { background: var(--pbt-status-bg, var(--colorBackground)) !important; color: var(--pbt-status-fg, var(--colorNeutralForeground2)) !important; }\nhtml[data-pbt-skin] section.statusBar :is(div, span, button):not(.pbt-chip, .pbt-chip *) { color: inherit !important; }\n/* Share (top right) is a Fluent v8 button with fixed dark colors. */\nhtml[data-pbt-skin] .farPeripheralControls button {\n  background: var(--pbt-share-bg, transparent) !important; color: var(--pbt-share-fg, var(--pbt-tab-fg, var(--colorNeutralForeground1))) !important;\n  border: 1px solid color-mix(in srgb, currentColor 45%, transparent) !important; border-radius: var(--pbt-hover-radius, 4px) !important;\n  width: auto !important; padding: 0 10px !important;\n}\nhtml[data-pbt-skin] .farPeripheralControls button * { color: inherit !important; }\nhtml[data-pbt-skin] .farPeripheralControls button .ms-Button-flexContainer { gap: 6px; }\nhtml[data-pbt-skin] .farPeripheralControls svg :is(path, [class^=\"OfficeIconColors\"]) { fill: currentColor !important; }\n/* Disabled until signed in: dim the whole button evenly instead of fading only the icon. */\nhtml[data-pbt-skin] .farPeripheralControls button.is-disabled { opacity: .85; }\nhtml[data-pbt-skin] .farPeripheralControls button.is-disabled * { opacity: 1 !important; }\nhtml[data-pbt-skin] .farPeripheralControls button:not(.is-disabled):hover {\n  background: var(--pbt-hover-bg, color-mix(in srgb, var(--pbt-accent) 28%, transparent)) !important;\n  box-shadow: inset 0 0 0 1px var(--pbt-hover-ring, var(--pbt-hover-ring-default));\n}\n\n/* canvas gutter around the report page */\nhtml[data-pbt-skin] .explorationContainer .exploration { background: var(--pbt-gutter, var(--white-color)) !important; }\n\n/* ---- our UI ---- */\n.pbt-toasts {\n  position: fixed; right: 24px; bottom: 44px; z-index: 2147483000;\n  display: flex; flex-direction: column; gap: 8px; align-items: flex-end; pointer-events: none;\n}\n.pbt-toast {\n  max-width: 380px; padding: 10px 14px; border-radius: var(--pbt-ui-radius);\n  background: var(--pbt-ui-bg); color: var(--pbt-ui-fg); border: 1px solid var(--pbt-ui-border);\n  border-left: 4px solid var(--pbt-accent);\n  font: 13px/1.4 var(--pbt-ui-font); box-shadow: var(--pbt-ui-shadow);\n  opacity: 0; transform: translateY(8px); transition: opacity .25s, transform .25s;\n  display: flex; flex-direction: column; gap: 2px;\n}\n.pbt-toast.pbt-in { opacity: 1; transform: none; }\n.pbt-toast b { font-weight: 700; }\n/* Tone wins over a skin's own toast edge color, so an error never looks like good news. */\n.pbt-toast.pbt-bad, html[data-pbt-skin] .pbt-toast.pbt-bad { border-left-color: #e74c3c !important; }\n.pbt-toast.pbt-good, html[data-pbt-skin] .pbt-toast.pbt-good { border-left-color: #2ecc71 !important; }\n.pbt-toast.pbt-warn, html[data-pbt-skin] .pbt-toast.pbt-warn { border-left-color: #f39c12 !important; }\n\n/* status bar chip and menu */\n.pbt-chip {\n  display: inline-flex; align-items: center; gap: 6px; height: 18px; margin: 0 6px; padding: 0 8px;\n  border: 1px solid transparent; border-radius: 3px; background: transparent; color: inherit;\n  font: inherit; font-size: 12px; cursor: pointer; white-space: nowrap;\n}\n/* Hover and keyboard focus: a tint plus the outline, at a selector strong enough that no skin's\n   status bar rules flatten it. */\n.pbt-chip:is(:hover, :focus-visible), html[data-pbt-skin] section.statusBar .pbt-chip:is(:hover, :focus-visible) {\n  border-color: currentColor !important; background: color-mix(in srgb, currentColor 16%, )PBT"
LR"PBT(transparent) !important; outline: none;\n}\n/* The separator belongs to the count, so a hidden or empty count leaves no stray dot. */\n.pbt-chip .pbt-count:not(:empty)::before { content: \"\\00b7\"; margin-right: 6px; }\n.pbt-chip .pbt-dot { width: 8px; height: 8px; border-radius: 50%; background: var(--pbt-accent); }\n/* The caret: an up chevron drawn from two borders, as tall as the chip's text (the menu opens upward). */\n.pbt-chip .pbt-caret { flex: none; width: 7px; height: 7px; margin: 4px 2px 0; box-sizing: border-box; border: solid currentColor; border-width: 1.6px 1.6px 0 0; transform: rotate(-45deg); opacity: .85; }\n.pbt-count.pbt-warn { color: #f39c12; font-weight: 600; }\n/* The one-time hint beside the chip (page/20-menu.js, hint). */\n.pbt-hint {\n  display: inline-flex; align-items: center; gap: 4px; height: 18px; margin: 0 4px; padding: 0 2px 0 8px; box-sizing: border-box;\n  border: 1px dashed currentColor; border-radius: 9px; font-size: 11px; white-space: nowrap; opacity: .9;\n}\n.pbt-hint .pbt-hint-x { border: 0; background: transparent; color: inherit; font: inherit; font-size: 13px; line-height: 1; padding: 0 5px; cursor: pointer; border-radius: 7px; }\n.pbt-hint .pbt-hint-x:hover, .pbt-hint .pbt-hint-x:focus-visible { background: color-mix(in srgb, currentColor 18%, transparent); outline: none; }\n\n/* The menu: skins as tiles, then tools and extras as switches with a one-line description. */\n.pbt-menu {\n  position: fixed; z-index: 2147483001; width: 310px; max-height: 88vh; overflow: auto; padding: 6px; box-sizing: border-box;\n  background: var(--pbt-ui-bg); color: var(--pbt-ui-fg); border: 1px solid var(--pbt-ui-border);\n  border-radius: var(--pbt-ui-radius); box-shadow: var(--pbt-ui-shadow); font: 13px/1.3 var(--pbt-ui-font);\n}\n.pbt-menu h4 { display: flex; align-items: baseline; gap: 8px; margin: 6px 8px 2px; font-size: 11px; text-transform: uppercase; letter-spacing: .06em; font-weight: 600; }\n.pbt-menu h4 .pbt-key { text-transform: none; letter-spacing: 0; }\n.pbt-menu h4 button.pbt-sect { display: inline-flex; width: auto; align-items: center; gap: 6px; padding: 2px 4px; margin: 0 0 0 -4px; opacity: .75; font: inherit; text-transform: inherit; letter-spacing: inherit; }\n.pbt-menu h4 button.pbt-sect:hover, .pbt-menu h4 button.pbt-sect:focus-visible { opacity: 1; }\n/* A long skin name in the Skins heading ends in an ellipsis (the full name is in its tooltip); the key hint keeps its words. */\n.pbt-menu h4 button.pbt-sect { min-width: 0; }\n.pbt-menu h4 button.pbt-sect > span:last-child { min-width: 0; overflow: hidden; text-overflow: ellipsis; white-space: nowrap; }\n.pbt-menu h4 .pbt-key { flex: none; white-space: nowrap; }\n.pbt-menu .pbt-chev { display: inline-block; width: 10px; font-size: 10px; transition: transform .12s; }\n.pbt-menu .pbt-section.pbt-folded .pbt-chev { transform: rotate(-90deg); }\n.pbt-menu .pbt-section.pbt-folded .pbt-body { display: none; }\n.pbt-menu .pbt-blurb { margin: 0 8px 6px; font-size: 11px; line-height: 1.35; opacity: .8; }\n/* Compact (short windows): descriptions live in the tooltips only. */\n/* A drag handle at the bottom right; the menu is anchored by its bottom edge, so it grows upward. */\n.pbt-menu { resize: vertical; min-height: 140px; }\n.pbt-menu hr { border: 0; border-top: 1px solid var(--pbt-ui-border); margin: 6px 4px; }\n.pbt-menu button {\n  display: flex; width: 100%; align-items: center; gap: 8px; padding: 5px 8px; border: 0; border-radius: 4px; box-sizing: border-box;\n  background: transparent; color: inherit; font: inherit; text-align: left; cursor: pointer;\n}\n.pbt-menu button:hover, .pbt-menu button:focus-visible { background: color-mix(in srgb, var(--pbt-accent) 25%, transparent); outline: none; }\n.pbt-menu .pbt-key { margin-left: auto; opacity: .75; font-size: 11px; white-space: nowrap; }\n.pbt-skins { display: grid; grid-template-columns: repeat(3, 1fr); gap: 3px; padding: 0 4px; }\n.pbt-skins button { flex-direction)PBT"
LR"PBT(: column; align-items: flex-start; gap: 3px; padding: 5px 7px; border: 1px solid transparent; font-size: 12px; }\n.pbt-skins button[aria-checked=\"true\"] { border-color: var(--pbt-accent); background: color-mix(in srgb, var(--pbt-accent) 16%, transparent); }\n.pbt-skins .pbt-sw { width: 100%; height: 10px; border-radius: 3px; border: 1px solid rgba(128, 128, 128, .5); box-sizing: border-box; }\n.pbt-skins .pbt-name { display: flex; align-items: center; gap: 4px; width: 100%; }\n.pbt-skins .pbt-name::after { content: \"\"; margin-left: auto; }\n.pbt-skins button[aria-checked=\"true\"] .pbt-name::after { content: \"\\2713\"; color: var(--pbt-accent); font-weight: 700; }\n.pbt-menu .pbt-item { display: grid; grid-template-columns: 30px 1fr auto; grid-template-areas: \"sw name key\" \". desc desc\"; column-gap: 8px; row-gap: 1px; align-items: center; }\n.pbt-menu .pbt-item .pbt-switch { grid-area: sw; }\n.pbt-menu .pbt-item .pbt-name { grid-area: name; }\n.pbt-menu .pbt-item .pbt-key, .pbt-menu .pbt-item .pbt-pills { grid-area: key; margin-left: 0; }\n.pbt-menu .pbt-pills { display: inline-flex; gap: 4px; }\n.pbt-menu .pbt-item .pbt-desc { grid-area: desc; font-size: 11px; line-height: 1.3; opacity: .72; }\n.pbt-switch { position: relative; width: 28px; height: 15px; border-radius: 8px; background: color-mix(in srgb, var(--pbt-ui-fg) 25%, transparent); box-shadow: inset 0 0 0 1px color-mix(in srgb, var(--pbt-ui-fg) 40%, transparent); transition: background .12s; }\n.pbt-switch::after { content: \"\"; position: absolute; top: 2px; left: 2px; width: 11px; height: 11px; border-radius: 50%; background: var(--pbt-ui-fg); transition: left .12s; }\n[aria-checked=\"true\"] > .pbt-switch { background: var(--pbt-accent); box-shadow: none; }\n[aria-checked=\"true\"] > .pbt-switch::after { left: 15px; background: var(--pbt-ui-bg); }\n.pbt-menu .pbt-try { padding: 1px 9px; border: 1px solid color-mix(in srgb, var(--pbt-ui-fg) 45%, transparent); border-radius: 10px; font-size: 11px; cursor: pointer; }\n.pbt-menu .pbt-try:hover, .pbt-menu .pbt-try:focus-visible { border-color: var(--pbt-accent); background: color-mix(in srgb, var(--pbt-accent) 25%, transparent); outline: none; }\n.pbt-menu .pbt-try .pbt-pill-icon { width: 11px; height: 11px; margin-right: 4px; vertical-align: -1px; fill: none; stroke: currentColor; stroke-width: 1.5; stroke-linejoin: round; stroke-linecap: round; }\n.pbt-menu .pbt-try[aria-pressed=\"true\"] { border-color: var(--pbt-accent); background: var(--pbt-accent); color: var(--pbt-ui-bg); font-weight: 600; }\n.pbt-menu .pbt-help-item .pbt-q { width: 30px; text-align: center; font-weight: 700; }\n\n/* tooltips: the shared bubble pbt.tip draws, title line then explanation lines */\n.pbt-tip {\n  position: fixed; z-index: 2147483004; max-width: 300px; padding: 6px 9px; pointer-events: none; box-sizing: border-box;\n  background: var(--pbt-ui-bg); color: var(--pbt-ui-fg); border: 1px solid var(--pbt-ui-border);\n  border-radius: var(--pbt-ui-radius); box-shadow: var(--pbt-ui-shadow); font: 12px/1.35 var(--pbt-ui-font);\n}\n.pbt-tip b { display: block; font-weight: 700; }\n.pbt-tip span { display: block; }\n.pbt-tip::after {\n  content: \"\"; position: absolute; left: var(--pbt-tip-arrow, 18px); bottom: -6px; width: 10px; height: 10px; transform: translateX(-50%) rotate(45deg);\n  background: var(--pbt-ui-bg); border-right: 1px solid var(--pbt-ui-border); border-bottom: 1px solid var(--pbt-ui-border);\n}\n.pbt-tip.pbt-tip-below::after { bottom: auto; top: -6px; border: 0; border-left: 1px solid var(--pbt-ui-border); border-top: 1px solid var(--pbt-ui-border); }\n\n/* help page */\n.pbt-help-backdrop { position: fixed; inset: 0; z-index: 2147483001; display: grid; place-items: center; background: rgba(0, 0, 0, .35); }\n.pbt-help {\n  width: min(680px, 92vw); max-height: 88vh; overflow: auto; outline: none; box-sizing: border-box;\n  background: var(--pbt-ui-bg); color: var(--pbt-ui-fg); border: 1px solid var(--pbt)PBT"
LR"PBT(-ui-border); border-radius: var(--pbt-ui-radius); box-shadow: var(--pbt-ui-shadow);\n  font: 13px/1.45 var(--pbt-ui-font); text-align: left;\n}\n.pbt-help header { position: sticky; top: 0; z-index: 1; display: flex; align-items: center; gap: 10px; padding: 10px 14px; background: var(--pbt-ui-bg); border-bottom: 1px solid var(--pbt-ui-border); }\n.pbt-help header h2 { margin: 0; font-size: 15px; font-weight: 700; }\n.pbt-help header nav { display: flex; flex-wrap: wrap; gap: 2px; margin-left: 8px; font-size: 12px; }\n.pbt-help header .pbt-tab { border: 0; border-bottom: 2px solid transparent; background: transparent; color: inherit; font: inherit; font-size: 12px; padding: 3px 7px; opacity: .75; cursor: pointer; }\n.pbt-help header .pbt-tab:hover, .pbt-help header .pbt-tab:focus-visible { opacity: 1; outline: none; background: color-mix(in srgb, var(--pbt-accent) 18%, transparent); }\n.pbt-help header .pbt-tab[aria-selected=\"true\"] { opacity: 1; border-bottom-color: var(--pbt-accent); font-weight: 700; }\n.pbt-help .pbt-tabpanel[hidden] { display: none; }\n/* A drag handle at the bottom right; the page is centered, so it grows from the middle. */\n.pbt-help { resize: both; min-width: 360px; min-height: 160px; }\n.pbt-help header .pbt-x { margin-left: auto; border: 0; background: transparent; color: inherit; font: 18px/1 var(--pbt-ui-font); cursor: pointer; padding: 0 4px; }\n.pbt-help section { padding: 10px 14px; border-bottom: 1px solid color-mix(in srgb, var(--pbt-ui-border) 50%, transparent); }\n.pbt-help section:last-child { border-bottom: 0; }\n.pbt-help h3 { display: flex; align-items: baseline; gap: 8px; margin: 0 0 4px; font-size: 13px; font-weight: 700; }\n.pbt-help h3 .pbt-key { margin-left: auto; font-size: 11px; opacity: .75; font-weight: 400; white-space: nowrap; }\n.pbt-help p { margin: 0 0 6px; opacity: .92; }\n.pbt-help ul { margin: 0 0 6px; padding-left: 18px; }\n.pbt-help li { margin: 2px 0; }\n.pbt-help li .pbt-state { margin-left: 6px; font-size: 11px; opacity: .6; }\n.pbt-help table { border-collapse: collapse; font-size: 12px; }\n.pbt-help td { padding: 2px 10px 2px 0; vertical-align: top; }\n.pbt-help td:first-child { white-space: nowrap; opacity: .8; }\n.pbt-help .pbt-legend { display: grid; grid-template-columns: 24px 1fr; gap: 4px 10px; align-items: center; margin: 4px 0 6px; }\n.pbt-help .pbt-lg-box { width: 22px; height: 14px; outline: 1px solid rgba(0, 153, 255, .9); background: rgba(0, 153, 255, .08); }\n.pbt-help .pbt-lg-bad { width: 22px; height: 14px; outline: 2px solid #e5383b; background: rgba(229, 56, 59, .1); }\n.pbt-help .pbt-lg-guide { width: 22px; height: 0; border-top: 1px dashed #e5383b; }\n.pbt-help .pbt-lg-spill { width: 22px; height: 14px; outline: 2px solid #e5383b; background: rgba(229, 56, 59, .1); clip-path: inset(0 0 0 40%); margin-left: -8px; }\n.pbt-help .pbt-help-skins { display: grid; grid-template-columns: repeat(2, 1fr); gap: 3px 14px; margin: 2px 0 6px; font-size: 12px; }\n.pbt-help .pbt-help-skin { display: flex; align-items: center; gap: 6px; }\n.pbt-help .pbt-help-skin span:not(.pbt-sw) { opacity: .7; overflow: hidden; text-overflow: ellipsis; }\n.pbt-help .pbt-sw { flex: none; width: 24px; height: 11px; border-radius: 3px; border: 1px solid rgba(128, 128, 128, .5); display: inline-block; vertical-align: middle; }\n.pbt-help .pbt-help-palette { display: flex; flex-wrap: wrap; align-items: center; gap: 4px; margin: 0 0 6px; }\n.pbt-help .pbt-help-palette .pbt-sw { width: 16px; height: 16px; }\n.pbt-help .pbt-btn { padding: 1px 10px; border: 1px solid color-mix(in srgb, var(--pbt-ui-fg) 45%, transparent); border-radius: 10px; background: transparent; color: inherit; font: 11px var(--pbt-ui-font); cursor: pointer; }\n.pbt-help .pbt-btn:hover { border-color: var(--pbt-accent); background: color-mix(in srgb, var(--pbt-accent) 25%, transparent); }\n/* The footer under every tab, like a card's footer: the credit line, then the fingerprint, more muted. */\n.pbt)PBT"
LR"PBT(-help .pbt-help-foot { display: flex; flex-wrap: wrap; gap: 2px 12px; padding: 8px 14px; border-top: 1px solid var(--pbt-ui-border); font-size: 11px; opacity: .85; }\n.pbt-help .pbt-help-foot .pbt-help-fp { margin-left: auto; opacity: .7; }\n\n/* splash shown when a skin is chosen */\n.pbt-splash {\n  position: fixed; inset: 0; z-index: 2147483002; display: grid; place-items: center; pointer-events: none;\n  background: radial-gradient(circle at 50% 45%, rgba(0, 0, 0, .35), rgba(0, 0, 0, .75));\n  animation: pbt-splash 1.9s ease forwards;\n}\n.pbt-splash > div {\n  padding: 28px 48px; text-align: center; color: var(--pbt-ui-fg); font: 600 34px/1.2 var(--pbt-title-font, var(--pbt-ui-font));\n  background: var(--pbt-ui-bg); border: 2px solid var(--pbt-accent); border-radius: var(--pbt-ui-radius); box-shadow: var(--pbt-ui-shadow);\n}\n.pbt-splash small { display: block; margin-top: 8px; font: 14px/1.3 var(--pbt-ui-font); opacity: .8; }\n@keyframes pbt-splash { 0% { opacity: 0; } 12% { opacity: 1; } 75% { opacity: 1; } 100% { opacity: 0; } }\n\n/* confetti canvas */\n.pbt-confetti { position: fixed; inset: 0; z-index: 2147483003; pointer-events: none; }\n\n/* Konami code: the report page does a little dance */\n@keyframes pbt-wobble { 0%, 100% { transform: none; } 25% { transform: rotate(-1.2deg); } 75% { transform: rotate(1.2deg); } }\n.pbt-dance .displayArea .visualContainer { animation: pbt-wobble .35s ease-in-out 6; }\n\n/* ---- layout QA overlay ---- */\n.pbt-qa { position: fixed; z-index: 2147482000; pointer-events: none; }\n.pbt-qa {\n  background-image:\n    linear-gradient(to right, rgba(0, 153, 255, .22) 1px, transparent 1px),\n    linear-gradient(to bottom, rgba(0, 153, 255, .22) 1px, transparent 1px),\n    linear-gradient(to right, rgba(0, 153, 255, .10) 1px, transparent 1px),\n    linear-gradient(to bottom, rgba(0, 153, 255, .10) 1px, transparent 1px);\n  background-size: var(--pbt-major) var(--pbt-major), var(--pbt-major) var(--pbt-major), var(--pbt-grid) var(--pbt-grid), var(--pbt-grid) var(--pbt-grid);\n  outline: 1px dashed rgba(0, 153, 255, .8);\n}\n.pbt-ruler { position: absolute; font: 10px/1 Consolas, monospace; color: #0a84ff; }\n.pbt-ruler-x { left: 0; right: 0; top: -14px; height: 14px; }\n.pbt-ruler-y { top: 0; bottom: 0; left: -30px; width: 30px; }\n.pbt-ruler span { position: absolute; background: rgba(255, 255, 255, .85); padding: 1px 2px; border-radius: 2px; }\n.pbt-ruler-x span { transform: translateX(-50%); }\n.pbt-ruler-y span { right: 2px; transform: translateY(-50%); }\n.pbt-box { position: absolute; outline: 1px solid rgba(0, 153, 255, .9); background: rgba(0, 153, 255, .05); }\n.pbt-box.pbt-bad { outline: 2px solid #e5383b; background: rgba(229, 56, 59, .07); }\n.pbt-label {\n  position: absolute; left: -1px; bottom: 100%; pointer-events: auto; cursor: copy;\n  display: flex; gap: 6px; align-items: center; max-width: max(100%, 160px); overflow: hidden; white-space: nowrap;\n  padding: 1px 5px; border: 0; border-radius: 3px 3px 0 0; background: #0a84ff; color: #fff; font: 10px/1.4 Consolas, monospace;\n}\n.pbt-box.pbt-bad .pbt-label { background: #e5383b; }\n.pbt-label em { font-style: normal; font-weight: 700; }\n.pbt-guide { position: absolute; pointer-events: none; }\n.pbt-guide-x { top: 0; bottom: 0; border-left: 1px dashed #e5383b; }\n.pbt-guide-y { left: 0; right: 0; border-top: 1px dashed #e5383b; }\n\n/* ---- Skins group added to the View ribbon ---- */\n/* Sized to the row it sits in (skins with thick frames make that row shorter), and kept clear\n   of the ribbon's collapse chevron at the far right. */\n.pbt-rgroup {\n  display: flex; flex-direction: column; align-self: stretch; box-sizing: border-box; max-height: 100%; overflow: hidden;\n  margin-right: 36px; padding: 0 8px; border-left: 1px solid var(--colorNeutralStroke2, rgba(127, 127, 127, .4)); color: var(--colorNeutralForeground1, inherit);\n}\n.pbt-rgroup-body { flex: 1 1 auto; min-height: 0; display: flex; align-ite)PBT"
LR"PBT(ms: center; }\n.pbt-rgroup-label { display: block; flex: none; text-align: center; font-size: 11px; line-height: 14px; margin-bottom: 2px; color: var(--pbt-group-fg, inherit); }\n/* Two rows that grow sideways, however many skins there are. */\n.pbt-rgallery { display: grid; grid-template-rows: repeat(2, auto); grid-auto-flow: column; gap: 2px 3px; }\n.pbt-rbtn {\n  display: flex; flex-direction: column; align-items: center; gap: 0; min-width: 50px; padding: 1px 4px; line-height: 1.15;\n  border: 1px solid transparent; border-radius: 4px; background: transparent; color: inherit; font: inherit; font-size: 11px; cursor: pointer;\n}\n.pbt-rbtn:hover { border-color: var(--pbt-hover-ring, var(--colorNeutralStroke1, #888)); background: var(--pbt-hover-bg, transparent); }\n.pbt-rbtn[aria-pressed=\"true\"] { border-color: var(--pbt-accent); background: color-mix(in srgb, var(--pbt-accent) 18%, transparent); }\n.pbt-rbtn .pbt-sw { width: 34px; height: 10px; border-radius: 3px; border: 1px solid rgba(127, 127, 127, .6); }\n/* Classic's two styles, a column beside the gallery while Classic is picked. */\n.pbt-rstyle { display: flex; flex-direction: column; gap: 3px; margin-left: 6px; padding-left: 6px; border-left: 1px solid var(--colorNeutralStroke2, rgba(127, 127, 127, .4)); }\n.pbt-rstyle .pbt-rbtn { min-width: 76px; padding: 3px 6px; }\n\n/* An edge off the grid: amber, apart from the red marks, because leaving the grid can be a choice. */\n.pbt-box.pbt-off { outline: 2px dashed #d98e04; background: rgba(217, 142, 4, .06); }\n.pbt-box.pbt-off .pbt-label { background: #b8780a; }\n/* A Stencil block checked with the visuals: a dashed frame and an outlined label, so it never reads\n   as a built visual; the red and amber findings still color it. */\n.pbt-box.pbt-box-block:not(.pbt-bad):not(.pbt-off) { outline-style: dashed; background: transparent; }\n.pbt-box.pbt-box-block .pbt-label { font-style: italic; box-shadow: inset 0 0 0 1px rgba(255, 255, 255, .55); }\n.pbt-box.pbt-box-block:not(.pbt-bad):not(.pbt-off) .pbt-label { background: #5f6b78; }\n.pbt-help .pbt-lg-off { width: 22px; height: 14px; outline: 2px dashed #d98e04; background: rgba(217, 142, 4, .1); }\n\n/* A sub-switch under its parent (the cheer under Confetti). */\n.pbt-menu .pbt-item.pbt-sub { padding-left: 34px; }\n/* The tools' jobs (Plan, Layout, Check, While editing) as small labels inside the Tools section. */\n.pbt-menu .pbt-group { margin: 6px 8px 1px 46px; font-size: 10px; letter-spacing: .04em; opacity: .7; }\n/* A skin whose hover paint is its accent would hide the \"on\" track: a ring in the row's own text color keeps it. */\n.pbt-menu button:hover > .pbt-switch, .pbt-menu button:focus-visible > .pbt-switch { box-shadow: inset 0 0 0 1px currentColor; }\n\n/* ---- Layout QA: card, guides with tags, hover distances, shared edges, status strip ---- */\n.pbt-tip.pbt-tip-rich { padding: 0; max-width: 360px; }\n.pbt-card { width: 340px; font: 12px/1.35 var(--pbt-ui-font); }\n.pbt-card header { display: flex; align-items: center; gap: 8px; padding: 8px 10px; border-bottom: 1px solid var(--pbt-ui-border); }\n.pbt-card header b { font-size: 13px; }\n.pbt-card .pbt-card-type { opacity: .7; }\n.pbt-card .pbt-card-badge { margin-left: auto; padding: 1px 8px; border-radius: 10px; font-weight: 700; font-size: 11px; color: #fff; white-space: nowrap; }\n/* Severity, the only thing a badge's color means, for every check tool's badges and cards alike:\n   red to fix, amber to look at, grey for information, green for a pass. */\n.pbt-sev-bad, .pbt-badge-bad { background: #c4314b; }\n.pbt-sev-warn, .pbt-badge-off { background: #9c6609; }\n.pbt-sev-info { background: #5f6b78; }\n.pbt-sev-ok, .pbt-badge-ok { background: #1c8353; }\n.pbt-card-kpi { display: grid; grid-template-columns: repeat(4, 1fr); gap: 1px; background: var(--pbt-ui-border); border-bottom: 1px solid var(--pbt-ui-border); }\n.pbt-card-kpi div { background: var(--pbt-ui-bg); padding: 5px 10px; }\n.pbt-c)PBT"
LR"PBT(ard-kpi small { display: block; opacity: .6; font-size: 10px; text-transform: uppercase; letter-spacing: .06em; }\n.pbt-card-kpi strong { font: 700 15px Consolas, monospace; }\n.pbt-card-kpi strong i { font: 10px var(--pbt-ui-font); opacity: .75; margin-left: 3px; font-style: normal; color: #f39c12; }\n/* What a counter counts, one tiny line under it. */\n.pbt-card-kpi .pbt-card-kpi-note { display: block; margin-top: 1px; font-size: 9.5px; line-height: 1.25; opacity: .6; }\n/* A pinned card (pbt.tip): it takes the pointer, scrolls when taller than the window, and its\n   counters filter the list (the one pressed is marked) while its rows outline their parts. */\n.pbt-tip.pbt-tip-pinned { pointer-events: auto; max-height: calc(100vh - 16px); overflow: auto; outline: 2px solid var(--pbt-accent); outline-offset: -1px; }\n.pbt-tip-pinned .pbt-card-kpi .pbt-kpi-toggle { cursor: pointer; }\n.pbt-tip-pinned .pbt-card-kpi .pbt-kpi-toggle:hover, .pbt-tip-pinned .pbt-card-kpi .pbt-kpi-toggle:focus-visible { background: color-mix(in srgb, var(--pbt-accent) 10%, var(--pbt-ui-bg)); outline: none; }\n.pbt-tip-pinned .pbt-card-kpi .pbt-kpi-toggle[aria-pressed=\"true\"] { background: color-mix(in srgb, var(--pbt-accent) 22%, var(--pbt-ui-bg)); box-shadow: inset 0 -2px 0 var(--pbt-accent); }\n.pbt-tip-pinned .pbt-card-row-pin { cursor: pointer; border-radius: 3px; }\n.pbt-tip-pinned .pbt-card-row-pin:hover, .pbt-tip-pinned .pbt-card-row-pin.pbt-on { background: color-mix(in srgb, var(--pbt-accent) 12%, transparent); }\n.pbt-tip .pbt-card-rows > [hidden] { display: none; }\n.pbt-card-rows { padding: 6px 10px; display: grid; gap: 5px; }\n.pbt-card-row { display: grid; grid-template-columns: 14px 1fr auto; gap: 8px; align-items: start; }\n.pbt-card-dot { display: inline-block; width: 10px; height: 10px; border-radius: 2px; margin-top: 3px; flex: none; }\n.pbt-dot-edge { background: #e5383b; } .pbt-dot-gap { background: repeating-linear-gradient(45deg, #e5383b 0 2px, #f5a4a0 2px 4px); }\n.pbt-dot-grid { background: #d98e04; } .pbt-dot-ok { background: #1e8e5a; } .pbt-dot-dist { background: #0a84ff; } .pbt-dot-vis { background: #0a84ff; }\n.pbt-card-fix { color: var(--pbt-accent); font: 11px/1.4 Consolas, monospace; white-space: pre-line; text-align: right; }\n.pbt-card footer { padding: 6px 10px; border-top: 1px solid var(--pbt-ui-border); display: flex; gap: 12px; font-size: 11px; opacity: .85; }\n.pbt-card footer b { color: var(--pbt-accent); font-weight: 600; }\n.pbt-label .pbt-pill { padding: 0 5px; border-radius: 8px; background: rgba(255, 255, 255, .25); font-weight: 700; }\n.pbt-box.pbt-hovered { outline-width: 2px; background: rgba(0, 153, 255, .12); }\n.pbt-box.pbt-bad.pbt-hovered { background: rgba(229, 56, 59, .14); }\n.pbt-guide-tag { position: absolute; padding: 1px 5px; background: #e5383b; color: #fff; font: 10px/1.4 Consolas, monospace; border-radius: 3px; white-space: nowrap; }\n.pbt-hover { position: absolute; inset: 0; pointer-events: none; }\n.pbt-dist-line { position: absolute; background: #0a84ff; }\n.pbt-dist-line.pbt-h { height: 1px; } .pbt-dist-line.pbt-v { width: 1px; }\n.pbt-dist-line::before, .pbt-dist-line::after { content: \"\"; position: absolute; background: #0a84ff; }\n.pbt-dist-line.pbt-h::before { left: 0; top: -3px; width: 1px; height: 7px; } .pbt-dist-line.pbt-h::after { right: 0; top: -3px; width: 1px; height: 7px; }\n.pbt-dist-line.pbt-v::before { top: 0; left: -3px; height: 1px; width: 7px; } .pbt-dist-line.pbt-v::after { bottom: 0; left: -3px; height: 1px; width: 7px; }\n.pbt-dist { position: absolute; transform: translate(-50%, -50%); padding: 2px 5px; background: #0a84ff; color: #fff; font: 10px/1 Consolas, monospace; border-radius: 3px; white-space: nowrap; }\n.pbt-dist.pbt-dist-page { background: #5a6b7a; }\n.pbt-shared { position: absolute; pointer-events: none; }\n.pbt-shared-x { top: 0; bottom: 0; border-left: 1px solid rgba(30, 142, 90, .85); }\n.pbt-shared-y { left: 0; right: 0; border-)PBT"
LR"PBT(top: 1px solid rgba(30, 142, 90, .85); }\n.pbt-shared-tag { position: absolute; padding: 1px 5px; background: #1e8e5a; color: #fff; font: 10px/1.4 Consolas, monospace; border-radius: 3px; white-space: nowrap; }\n.pbt-shared-x .pbt-shared-tag { left: 3px; bottom: 4px; }\n.pbt-shared-y .pbt-shared-tag { left: 6px; top: -9px; }\n.pbt-qa-strip {\n  position: absolute; left: 0; right: 0; display: flex; flex-wrap: wrap; align-items: center; gap: 4px 12px; padding: 4px 10px; pointer-events: auto; box-sizing: border-box;\n  background: var(--pbt-ui-bg); color: var(--pbt-ui-fg); border: 1px solid var(--pbt-ui-border); border-radius: var(--pbt-ui-radius); font: 12px/1.3 var(--pbt-ui-font); box-shadow: var(--pbt-ui-shadow);\n}\n.pbt-qa-strip .pbt-strip-item { display: inline-flex; align-items: center; gap: 5px; white-space: nowrap; }\n.pbt-qa-strip .pbt-strip-item b { font: 700 13px Consolas, monospace; }\n.pbt-qa-strip .pbt-strip-seg { margin-left: auto; display: inline-flex; gap: 2px; align-items: center; }\n.pbt-qa-strip .pbt-strip-label { opacity: .7; margin-right: 4px; }\n.pbt-qa-strip button { border: 1px solid color-mix(in srgb, var(--pbt-ui-fg) 35%, transparent); border-radius: 3px; background: transparent; color: inherit; font: 11px var(--pbt-ui-font); padding: 1px 7px; cursor: pointer; }\n.pbt-qa-strip button.pbt-on { background: var(--pbt-accent); border-color: var(--pbt-accent); color: var(--pbt-ui-bg); font-weight: 700; }\n.pbt-qa-strip button:hover { border-color: var(--pbt-accent); }\n.pbt-qa-strip .pbt-strip-help { width: 20px; height: 20px; padding: 0; border-radius: 50%; font-weight: 700; }\n.pbt-help .pbt-lg-shared { width: 22px; height: 0; border-top: 1px solid rgba(30, 142, 90, .9); }\n.pbt-help .pbt-lg-dist { width: 22px; height: 12px; background: #0a84ff; border-radius: 2px; }\n\n/* ---- Filter map (page/35-filters.js) ---- */\n.pbt-fm { position: fixed; z-index: 2147482990; pointer-events: none; }\n.pbt-fm-badge {\n  position: absolute; display: flex; gap: 3px; align-items: center; pointer-events: auto; cursor: default;\n  padding: 2px 3px; border-radius: 3px; box-shadow: 0 1px 3px rgba(0, 0, 0, .3);\n  font: 600 10px/1.3 Consolas, monospace; color: #fff; white-space: nowrap;\n}\n.pbt-fm-badge.pbt-fm-has { outline: 1px solid rgba(255, 255, 255, .75); outline-offset: -2px; }\n/* Chips are a letter and a count, all one neutral tint (the badge's color is its severity); the\n   words take their place while the pointer is on the badge. */\n.pbt-fm-chip { padding: 0 4px; border-radius: 2px; background: rgba(255, 255, 255, .16); }\n/* The words, while the pointer is on the badge or its card is open: a panel raised over the badge,\n   growing up and to the left from its bottom-right corner, wrapping onto lines above within its own\n   visual's width (--pbt-fm-max, set on hover). The chips stay in place, so the badge keeps its size. */\n.pbt-rail > .pbt-fm-badge[data-rail-tool] { position: relative; }\n.pbt-fm-words { display: none; }\n.pbt-fm-badge:is(:hover, .pbt-tip-open) > .pbt-fm-words {\n  position: absolute; right: 0; bottom: 0; z-index: 1; display: flex; flex-wrap: wrap-reverse; justify-content: flex-end; gap: 3px;\n  width: max-content; max-width: var(--pbt-fm-max, 240px); box-sizing: border-box; padding: 2px 3px; border-radius: 3px;\n  background: inherit; box-shadow: 0 2px 8px rgba(0, 0, 0, .45); white-space: nowrap;\n}\n/* The rail of a badge in use is drawn over the other rails. */\n.pbt-rail:hover, .pbt-rail:has(> .pbt-tip-open) { z-index: 1; }\n.pbt-dot-fvisual { background: #8a4dff; }\n.pbt-dot-fpage { background: #0a84ff; }\n.pbt-dot-freport { background: #12957f; }\n.pbt-dot-fother { background: #d98e04; }\n.pbt-fm-hidden { background: transparent; outline: 1px dashed #c9b3ff; color: #e3d6ff; }\n.pbt-fm-none { background: transparent; color: #c8ced6; font-weight: 400; }\n.pbt-fm .pbt-fm-strip { position: absolute; left: 0; right: 0; pointer-events: auto; }\n.pbt-fm-pane { margin-right: 10px; })PBT"
LR"PBT(\n.pbt-fm-menu {\n  position: fixed; z-index: 2147483000; min-width: 230px; padding: 4px 0; display: grid;\n  background: var(--pbt-fm-bg, var(--pbt-ui-bg, #fff)); color: var(--pbt-fm-fg, var(--pbt-ui-fg, #252423)); border: 1px solid color-mix(in srgb, currentColor 22%, transparent);\n  border-radius: 4px; box-shadow: 0 0 2px rgba(0, 0, 0, .24), 0 4px 8px rgba(0, 0, 0, .28); font: 12px/1.35 var(--pbt-ui-font, \"Segoe UI\", sans-serif);\n}\n.pbt-fm-menu:focus { outline: none; }\n.pbt-fm-menu-head { padding: 6px 12px 4px; font-size: 11px; font-weight: 600; opacity: .7; }\n.pbt-fm-item {\n  display: flex; align-items: center; gap: 10px; width: 100%; min-height: 26px; padding: 4px 12px; border: 0; border-radius: 0;\n  background: transparent; color: inherit; font: inherit; text-align: left; cursor: pointer;\n}\n.pbt-fm-item:hover:not(:disabled), .pbt-fm-item:focus-visible { background: color-mix(in srgb, currentColor 12%, transparent); outline: none; }\n.pbt-fm-item:disabled { opacity: .45; cursor: default; }\n.pbt-fm-icon { flex: none; width: 16px; text-align: center; font-size: 13px; }\n.pbt-fm-sep { height: 1px; margin: 4px 0; background: color-mix(in srgb, currentColor 16%, transparent); }\n.pbt-help .pbt-lg-fvisual, .pbt-help .pbt-lg-fpage, .pbt-help .pbt-lg-freport, .pbt-help .pbt-lg-fother { width: 22px; height: 14px; border-radius: 3px; }\n.pbt-help .pbt-lg-fvisual { background: #8a4dff; } .pbt-help .pbt-lg-fpage { background: #0a84ff; } .pbt-help .pbt-lg-freport { background: #12957f; } .pbt-help .pbt-lg-fother { background: #d98e04; }\n\n/* ---- Theme check (page/45-theme.js) ---- */\n.pbt-th { position: fixed; z-index: 2147482980; pointer-events: none; }\n.pbt-th-badge {\n  position: absolute; pointer-events: auto; cursor: default; padding: 1px 6px; border-radius: 3px;\n  font: 600 10px/1.4 Consolas, monospace; color: #fff; white-space: nowrap; box-shadow: 0 1px 3px rgba(0, 0, 0, .3);\n}\n.pbt-th .pbt-th-strip { position: absolute; left: 0; right: 0; pointer-events: auto; }\n/* One line in the dock, however many tabs lead it: the counts and buttons keep their size and the\n   words beside the counts give way (cut with an ellipsis; each item's tooltip has them in full),\n   the theme check's \"most often\" list first. */\n.pbt-dock .pbt-qa-strip { flex-wrap: nowrap; }\n.pbt-dock .pbt-qa-strip > * { flex-shrink: 0; white-space: nowrap; }\n.pbt-dock .pbt-qa-strip > .pbt-strip-item { flex-shrink: 1; min-width: 0; overflow: hidden; }\n.pbt-dock .pbt-qa-strip > .pbt-th-top { flex-shrink: 1000; }\n.pbt-dock .pbt-qa-strip > .pbt-strip-item > span:last-child { min-width: 0; overflow: hidden; text-overflow: ellipsis; }\n.pbt-dock .pbt-qa-strip > .pbt-strip-item > :is(b, .pbt-card-dot) { flex-shrink: 0; }\n/* Too wide for the dock (page/00-core.js, pbt.dock.place): the extras go, each also in a tooltip. */\n.pbt-dock .pbt-qa-strip.pbt-strip-tight [data-opt] { display: none; }\n.pbt-card-sub { padding: 6px 0 0; font-size: 10px; text-transform: uppercase; letter-spacing: .06em; opacity: .6; }\n/* A heading's note on the right names what the blue values are (the theme check's \"theme value\"). */\n.pbt-card-sub:has(> .pbt-card-fix) { display: flex; justify-content: space-between; gap: 8px; }\n.pbt-card-sub > .pbt-card-fix { text-transform: none; letter-spacing: 0; font-size: 10px; }\n\n/* While the filter map reads Power BI's own filter lists, the popup and the visual header icons\n   it opens are kept out of sight (page/35-filters.js, quiet). */\n/* Only the overlay box that holds the filter list, never another overlay of yours. The class sits\n   on the overlay container and the page itself, not on html: a class change on html restyles the\n   whole document (the rules end in a bare *), twice per pass. html carries it only before Power BI\n   has made its overlay container. */\n.cdk-overlay-container.pbt-fm-quiet > :has(filter-restatement-overlay),\n.cdk-overlay-container.pbt-fm-quiet > :has(filter-restatement-overlay) )PBT"
LR"PBT(*,\n.displayArea.pbt-fm-quiet .visualHeaderItemContainer,\nhtml.pbt-fm-quiet .cdk-overlay-container > :has(filter-restatement-overlay),\nhtml.pbt-fm-quiet .cdk-overlay-container > :has(filter-restatement-overlay) *,\nhtml.pbt-fm-quiet .displayArea .visualHeaderItemContainer { opacity: 0 !important; }\n/* ...and lets clicks through, so a click of yours during a pass reaches the report. */\n.cdk-overlay-container.pbt-fm-quiet > :has(filter-restatement-overlay),\n.cdk-overlay-container.pbt-fm-quiet > :has(filter-restatement-overlay) *,\nhtml.pbt-fm-quiet .cdk-overlay-container > :has(filter-restatement-overlay),\nhtml.pbt-fm-quiet .cdk-overlay-container > :has(filter-restatement-overlay) * { pointer-events: none !important; }\n\n/* The visual a hovered badge belongs to (pbt.ownBadge). */\n.pbt-owner { position: fixed; z-index: 2147482970; pointer-events: none; outline: 2px dashed #0a84ff; outline-offset: 1px; background: rgba(10, 132, 255, .06); border-radius: 2px; }\n\n/* ---- Interactions helper (page/36-interactions.js), only while Edit interactions is on ---- */\n/* The active interaction icon on each visual: None red, Filter blue, Highlight green. */\nhtml.pbt-ia .displayArea .visualHeaderItemContainer:has(> [aria-label=\"None\"][class*=\"filled\"]) { background: #d13438 !important; border-color: #d13438 !important; border-radius: 3px; }\nhtml.pbt-ia .displayArea .visualHeaderItemContainer:has(> [aria-label=\"Filter\"][class*=\"filled\"]) { background: #0f6cbd !important; border-color: #0f6cbd !important; border-radius: 3px; }\nhtml.pbt-ia .displayArea .visualHeaderItemContainer:has(> [aria-label=\"Highlight\"][class*=\"filled\"]) { background: #107c41 !important; border-color: #107c41 !important; border-radius: 3px; }\nhtml.pbt-ia .displayArea .visualHeaderItemContainer:has(> [class*=\"filled\"][aria-label]) > * { color: #fff !important; opacity: 1 !important; }\n.pbt-ia-host { position: fixed; z-index: 2147482995; pointer-events: none; }\n.pbt-ia-bar {\n  position: absolute; display: flex; align-items: center; gap: 4px; padding: 3px; pointer-events: auto; white-space: nowrap;\n  background: var(--pbt-ui-bg, #fff); color: var(--pbt-ui-fg, #252423); border: 1px solid color-mix(in srgb, currentColor 22%, transparent);\n  border-radius: 4px; box-shadow: 0 0 2px rgba(0, 0, 0, .24), 0 4px 8px rgba(0, 0, 0, .28); font: 12px/1.3 var(--pbt-ui-font, \"Segoe UI\", sans-serif);\n}\n.pbt-ia-btn {\n  display: inline-flex; align-items: center; gap: 6px; padding: 3px 9px; border: 1px solid color-mix(in srgb, currentColor 35%, transparent); border-radius: 3px;\n  background: color-mix(in srgb, currentColor 6%, transparent); color: inherit; font: inherit; cursor: pointer; box-shadow: 0 1px 0 color-mix(in srgb, currentColor 14%, transparent);\n}\n.pbt-ia-btn:hover, .pbt-ia-btn:focus-visible { border-color: var(--pbt-accent, currentColor); background: color-mix(in srgb, currentColor 14%, transparent); outline: none; }\n.pbt-ia-btn:active { box-shadow: none; transform: translateY(1px); }\n.pbt-ia-btn[aria-disabled=\"true\"] { opacity: .45; cursor: default; box-shadow: none; }\n.pbt-ia-btn[aria-disabled=\"true\"]:hover { border-color: color-mix(in srgb, currentColor 35%, transparent); background: color-mix(in srgb, currentColor 6%, transparent); }\n.pbt-ia-btn[aria-disabled=\"true\"]:active { transform: none; }\n.pbt-ia-dot { width: 10px; height: 10px; border-radius: 2px; }\n.pbt-ia-none .pbt-ia-dot { background: #d13438; } .pbt-ia-filter .pbt-ia-dot { background: #0f6cbd; } .pbt-ia-highlight .pbt-ia-dot { background: #107c41; }\n.pbt-ia-count { padding: 0 8px 0 6px; opacity: .75; border-left: 1px solid color-mix(in srgb, currentColor 22%, transparent); margin-left: 2px; }\n\n/* ---- Stencil (page/38-stencil.js) ---- */\n/* Under the other tools' layers, so their badges and strips stay reachable over it. */\n.pbt-st { position: fixed; z-index: 2147481900; pointer-events: none; }\n.pbt-st-edit {\n  pointer-events: auto; cursor: cross)PBT"
LR"PBT(hair; outline: 1px dashed color-mix(in srgb, var(--pbt-accent) 80%, transparent);\n  background-image: linear-gradient(to right, color-mix(in srgb, var(--pbt-accent) 16%, transparent) 1px, transparent 1px), linear-gradient(to bottom, color-mix(in srgb, var(--pbt-accent) 16%, transparent) 1px, transparent 1px);\n  background-size: var(--pbt-grid) var(--pbt-grid);\n}\n.pbt-st .pbt-st-strip { position: absolute; left: 0; right: 0; pointer-events: auto; cursor: default; }\n.pbt-st-block {\n  position: absolute; box-sizing: border-box; border: 1.5px dashed var(--pbt-accent); border-radius: 4px;\n  background: color-mix(in srgb, var(--pbt-accent) 9%, transparent); color: var(--pbt-ui-fg, #252423);\n}\n.pbt-st-edit .pbt-st-block { cursor: move; background: color-mix(in srgb, var(--pbt-accent) 14%, transparent); }\n.pbt-st-block.pbt-st-sel { border-style: solid; box-shadow: 0 0 0 1px var(--pbt-accent); }\n.pbt-st-block.pbt-st-built { border-style: solid; border-color: rgba(30, 142, 90, .9); background: transparent; }\n.pbt-st-block.pbt-st-new { border-style: solid; }\n/* Over an empty page, opaque: Power BI's empty-canvas art does not show through the blocks. */\n.pbt-st-empty .pbt-st-block:not(.pbt-st-built) { background: color-mix(in srgb, var(--pbt-accent) 12%, var(--pbt-ui-bg, #fff)); }\n.pbt-st-empty.pbt-st-edit .pbt-st-block:not(.pbt-st-built) { background: color-mix(in srgb, var(--pbt-accent) 18%, var(--pbt-ui-bg, #fff)); }\n/* No blocks yet: one line centred on the page just above the dock, as wide as its text. */\n.pbt-st-hint {\n  position: absolute; left: 50%; bottom: 8px; transform: translateX(-50%); width: max-content; max-width: 80%; box-sizing: border-box; padding: 6px 12px; pointer-events: none;\n  background: var(--pbt-ui-bg, #fff); color: var(--pbt-ui-fg, #252423); border: 1px dashed var(--pbt-accent); border-radius: 4px;\n  box-shadow: 0 1px 3px rgba(0, 0, 0, .2); font: 12px/1.4 var(--pbt-ui-font, \"Segoe UI\", sans-serif); text-align: center;\n}\n.pbt-st-tag {\n  position: absolute; left: 4px; top: 4px; max-width: calc(100% - 8px); display: inline-flex; align-items: center; gap: 5px; overflow: hidden;\n  padding: 1px 6px; border-radius: 3px; background: var(--pbt-ui-bg, #fff); color: var(--pbt-ui-fg, #252423); font: 11px/1.4 var(--pbt-ui-font, \"Segoe UI\", sans-serif);\n  white-space: nowrap; text-overflow: ellipsis; box-shadow: 0 1px 2px rgba(0, 0, 0, .2);\n}\n.pbt-st-tag .glyphicon { font-size: 12px; color: var(--pbt-accent); }\n.pbt-st-check { color: rgb(30, 142, 90); font-weight: 700; }\n.pbt-st-size {\n  position: absolute; right: 4px; bottom: 4px; padding: 1px 5px; border-radius: 3px; background: var(--pbt-accent); color: var(--pbt-ui-bg, #fff);\n  font: 600 10px/1.4 Consolas, monospace; white-space: nowrap; pointer-events: none;\n}\n.pbt-st-h { position: absolute; width: 8px; height: 8px; box-sizing: border-box; background: var(--pbt-ui-bg, #fff); border: 1.5px solid var(--pbt-accent); border-radius: 1px; }\n.pbt-st-h-n, .pbt-st-h-s { left: calc(50% - 4px); cursor: ns-resize; } .pbt-st-h-e, .pbt-st-h-w { top: calc(50% - 4px); cursor: ew-resize; }\n.pbt-st-h-n, .pbt-st-h-ne, .pbt-st-h-nw { top: -5px; } .pbt-st-h-s, .pbt-st-h-se, .pbt-st-h-sw { bottom: -5px; }\n.pbt-st-h-e, .pbt-st-h-ne, .pbt-st-h-se { right: -5px; } .pbt-st-h-w, .pbt-st-h-nw, .pbt-st-h-sw { left: -5px; }\n.pbt-st-h-ne, .pbt-st-h-sw { cursor: nesw-resize; } .pbt-st-h-nw, .pbt-st-h-se { cursor: nwse-resize; }\n.pbt-st-strip .pbt-st-name { font-weight: 600; }\n.pbt-st-strip .pbt-st-palette { margin-left: 0; }\n.pbt-st-strip .pbt-st-type { width: 26px; height: 22px; padding: 0; display: inline-flex; align-items: center; justify-content: center; }\n.pbt-st-strip .pbt-st-type .glyphicon { font-size: 13px; }\n.pbt-st-strip .pbt-st-typename { min-width: 76px; font-size: 11px; font-weight: 600; white-space: nowrap; }\n.pbt-st-strip .pbt-st-typename.pbt-st-dim { font-weight: 400; opacity: .7; }\n.pbt-st-strip .pbt-st-fields { margin-left: )PBT"
LR"PBT(0; gap: 6px; }\n.pbt-st-field { display: inline-flex; align-items: center; gap: 3px; }\n.pbt-st-field span { opacity: .7; }\n.pbt-st-strip input {\n  box-sizing: border-box; height: 22px; border: 1px solid color-mix(in srgb, var(--pbt-ui-fg) 35%, transparent); border-radius: 3px;\n  background: transparent; color: inherit; font: 11px var(--pbt-ui-font); padding: 0 5px;\n}\n.pbt-st-strip input:focus { outline: none; border-color: var(--pbt-accent); }\n.pbt-st-strip .pbt-st-num { width: 56px; font-family: Consolas, monospace; }\n.pbt-st-strip .pbt-st-label-input { width: 150px; }\n.pbt-st-strip .pbt-st-mode { margin-left: auto; }\n.pbt-st-confirm { max-width: 260px; padding: 2px 12px 6px; font-size: 11px; opacity: .85; }\n\n/* Visualizations pane, drill-through well: Power BI fills it with its own light drop-target color and\n   a dark dashed border, unreadable under any dark skin. Same look as the \"Add data fields here\"\n   wells above it: no fill, a border in the pane's text color at 32%. */\nhtml[data-pbt-skin] .drillthroughPane .cards.droppableElement {\n  background: transparent !important; border: 1px solid color-mix(in srgb, currentColor 32%, transparent) !important;\n}\n/* The empty report's \"Add data to your report\" screen (heading, four source cards and a link) sits\n   on the report page, which skins leave alone, yet takes its colors from Power BI's theme tokens. A\n   skin's remap turned some card pictures black, others pale blue or white, and the heading pale on\n   the white page. It keeps Power BI's own light colors (measured 2026-11-09) under every skin and\n   under the Power BI 95 look. Hover only deepens the card shadow, which stays Power BI's. */\nhtml:is([data-pbt-skin], .pbt-retro95) .canvasWatermark :is(.heading, .subHeading, .subHeading *, .subText) { color: #252423 !important; }\nhtml:is([data-pbt-skin], .pbt-retro95) .canvasWatermark .dataSourceImage.excel { background-color: #caead8 !important; }\nhtml:is([data-pbt-skin], .pbt-retro95) .canvasWatermark .dataSourceImage.sql { background-color: #eff6fc !important; }\nhtml:is([data-pbt-skin], .pbt-retro95) .canvasWatermark .dataSourceImage.inline { background-color: #fefdf5 !important; }\nhtml:is([data-pbt-skin], .pbt-retro95) .canvasWatermark .dataSourceImage.sample { background-color: #f3f2f1 !important; }\nhtml:is([data-pbt-skin], .pbt-retro95) .canvasWatermark button.link { color: #0078d4 !important; }\n/* Bottom right and click-through: the visual's own header icons sit top right. */\n.pbt-st-tag.pbt-st-tag-built { left: auto; top: auto; right: 4px; bottom: 4px; padding: 0 5px; pointer-events: none; }\n/* Stencil layouts in the menu: tiles with a small picture of each layout. */\n.pbt-st-tiles { display: grid; grid-template-columns: repeat(3, 132px); gap: 4px; padding: 2px 8px 6px; }\n.pbt-st-tile {\n  display: grid; gap: 4px; padding: 6px; border: 1px solid transparent; border-radius: 4px; background: transparent; color: inherit;\n  font: 11px/1.3 inherit; text-align: left; cursor: pointer;\n}\n.pbt-st-tile:hover, .pbt-st-tile:focus-visible { background: color-mix(in srgb, currentColor 10%, transparent); border-color: color-mix(in srgb, currentColor 22%, transparent); outline: none; }\n.pbt-st-thumb { width: 118px; height: auto; aspect-ratio: 16 / 9; display: block; }\n.pbt-st-thumb-page { fill: color-mix(in srgb, currentColor 6%, transparent); stroke: color-mix(in srgb, currentColor 25%, transparent); stroke-width: 8; }\n.pbt-st-thumb rect:not(.pbt-st-thumb-page) { fill: color-mix(in srgb, var(--pbt-accent) 28%, transparent); stroke: var(--pbt-accent); stroke-width: 6; }\n.pbt-st-thumb rect.pbt-st-thumb-title, .pbt-st-thumb rect.pbt-st-thumb-text { fill: color-mix(in srgb, currentColor 30%, transparent); stroke: none; }\n.pbt-st-thumb rect.pbt-st-thumb-card { fill: color-mix(in srgb, var(--pbt-accent) 50%, transparent); }\n/* Stencil layout picker and saved templates: a search box over a list that scrolls at a fixed\n   height (so t)PBT"
LR"PBT(he menu keeps its place above the strip while a search narrows it), built-in layouts\n   first, then \"Your templates\" with rename and delete on each tile, shown on hover or focus. */\n.pbt-st-search {\n  box-sizing: border-box; height: 26px; margin: 2px 12px 6px; padding: 0 8px; border: 1px solid color-mix(in srgb, currentColor 35%, transparent); border-radius: 3px;\n  background: transparent; color: inherit; font: 12px var(--pbt-ui-font, \"Segoe UI\", sans-serif);\n}\n.pbt-st-search:focus { outline: none; border-color: var(--pbt-accent); }\n.pbt-st-pick { height: min(56vh, 440px); overflow-y: auto; overscroll-behavior: contain; }\n.pbt-st-tile-wrap { position: relative; display: grid; }\n.pbt-st-tile-name { overflow: hidden; text-overflow: ellipsis; white-space: nowrap; }\n.pbt-st-tile-acts { position: absolute; top: 8px; right: 8px; display: none; gap: 2px; }\n.pbt-st-tile-wrap:hover .pbt-st-tile-acts, .pbt-st-tile-wrap:focus-within .pbt-st-tile-acts { display: inline-flex; }\n.pbt-st-tile-act {\n  width: 20px; height: 20px; padding: 0; display: inline-flex; align-items: center; justify-content: center; cursor: pointer;\n  border: 1px solid color-mix(in srgb, currentColor 25%, transparent); border-radius: 3px; background: var(--pbt-fm-bg, var(--pbt-ui-bg, #fff)); color: inherit;\n}\n.pbt-st-tile-act:hover, .pbt-st-tile-act:focus-visible { border-color: var(--pbt-accent); color: var(--pbt-accent); outline: none; }\n.pbt-st-tile.pbt-st-tile-edit { cursor: default; border-color: color-mix(in srgb, currentColor 22%, transparent); align-content: start; }\n.pbt-st-tile-input {\n  box-sizing: border-box; width: 100%; height: 22px; padding: 0 5px; border: 1px solid var(--pbt-accent); border-radius: 3px;\n  background: transparent; color: inherit; font: 11px var(--pbt-ui-font, \"Segoe UI\", sans-serif);\n}\n.pbt-st-tile-input:focus { outline: none; }\n.pbt-st-tile-confirm { display: flex; gap: 4px; }\n.pbt-st-tile-yes, .pbt-st-tile-no {\n  flex: 1; height: 24px; padding: 0 6px; border: 1px solid color-mix(in srgb, currentColor 30%, transparent); border-radius: 3px;\n  background: transparent; color: inherit; font: 11px var(--pbt-ui-font, \"Segoe UI\", sans-serif); cursor: pointer;\n}\n.pbt-st-tile-yes { background: #c4314b; border-color: #c4314b; color: #fff; }\n.pbt-st-tile-yes:focus-visible, .pbt-st-tile-no:focus-visible { outline: 2px solid var(--pbt-accent); outline-offset: 1px; }\n/* Stencil groups: a thin dashed outline a few pixels outside each group's blocks, its name above\n   the top left corner; under the blocks and click-through. */\n.pbt-st-group { position: absolute; margin: -4px; padding: 4px; border: 1px dashed color-mix(in srgb, var(--pbt-ui-fg, #252423) 55%, transparent); border-radius: 5px; pointer-events: none; }\n.pbt-st-group-name {\n  position: absolute; left: 4px; bottom: 100%; max-width: calc(100% - 8px); padding: 0 5px; overflow: hidden; text-overflow: ellipsis; white-space: nowrap;\n  border-radius: 3px 3px 0 0; background: var(--pbt-ui-bg, #fff); color: var(--pbt-ui-fg, #252423); font: 10px/1.5 var(--pbt-ui-font, \"Segoe UI\", sans-serif); opacity: .9;\n}\n\n/* ---- Colors (page/46-colors.js): hard to read and off palette in one badge per visual ---- */\n.pbt-co { position: fixed; z-index: 2147482970; pointer-events: none; }\n.pbt-co-badge {\n  position: absolute; display: flex; gap: 3px; align-items: center; pointer-events: auto; cursor: default; padding: 1px 6px; border-radius: 3px;\n  font: 600 10px/1.4 Consolas, monospace; color: #fff; white-space: nowrap; box-shadow: 0 1px 3px rgba(0, 0, 0, .3);\n}\n.pbt-co-badge b { font-weight: 600; }\n.pbt-co-badge .pbt-sw { width: 11px; height: 11px; padding: 0; border: 1px solid #fff; border-radius: 2px; cursor: copy; }\n.pbt-co .pbt-co-strip { position: absolute; left: 0; right: 0; pointer-events: auto; }\n/* \"bars, lines and points\": a switch with a track, apart from the AA and AAA buttons. */\n.pbt-qa-strip button.pbt-strip-switch { display: in)PBT"
LR"PBT(line-flex; align-items: center; gap: 6px; border: 0; padding: 1px 2px; background: transparent; color: inherit; font-weight: 400; }\n.pbt-qa-strip button.pbt-strip-switch.pbt-on { background: transparent; color: inherit; font-weight: 400; }\n.pbt-switch-track { position: relative; flex: none; width: 24px; height: 12px; border-radius: 6px; box-sizing: border-box; border: 1px solid color-mix(in srgb, var(--pbt-ui-fg) 45%, transparent); }\n.pbt-switch-track::after { content: \"\"; position: absolute; left: 2px; top: 2px; width: 6px; height: 6px; border-radius: 50%; background: color-mix(in srgb, var(--pbt-ui-fg) 60%, transparent); transition: left .12s; }\n.pbt-strip-switch.pbt-on .pbt-switch-track { background: var(--pbt-accent); border-color: var(--pbt-accent); }\n.pbt-strip-switch.pbt-on .pbt-switch-track::after { left: 14px; background: var(--pbt-ui-bg); }\n.pbt-qa-strip button.pbt-strip-switch:hover .pbt-switch-track, .pbt-qa-strip button.pbt-strip-switch:focus-visible .pbt-switch-track { border-color: var(--pbt-accent); }\n.pbt-ct-pair { display: inline-flex; align-items: center; gap: 4px; min-width: 0; }\n.pbt-ct-pair > span:last-child { overflow: hidden; text-overflow: ellipsis; white-space: nowrap; }\n.pbt-ct-sw { flex: none; width: 12px; height: 12px; border-radius: 2px; box-shadow: inset 0 0 0 1px rgba(127, 127, 127, .6); }\n/* While a contrast scan hit-tests the canvas, every part of it answers, so a data label's bar is\n   found under it (Power BI turns pointer events off on many SVG parts). */\nhtml.pbt-ct-probe .displayArea * { pointer-events: auto !important; }\n.pbt-help .pbt-lg-ctbad, .pbt-help .pbt-lg-ctwarn, .pbt-help .pbt-lg-ctok { width: 30px; height: 14px; border-radius: 3px; }\n.pbt-help .pbt-lg-ctbad { background: #c4314b; } .pbt-help .pbt-lg-ctwarn { background: #9c6609; } .pbt-help .pbt-lg-ctok { background: rgba(28, 131, 83, .9); }\n/* The tooltip turns spans into lines; a contrast pair stays on one: swatches, then the text. */\n.pbt-tip .pbt-ct-pair { display: flex; }\n.pbt-tip .pbt-ct-pair > span { display: inline-block; }\n\n/* ---- The tools' dock (page/00-core.js, pbt.dock): one bar under the page for every tool's\n   strip, a tab row first when two or more tools are on. Never below the visible canvas. ---- */\n.pbt-dock {\n  position: fixed; z-index: 2147482995; display: flex; align-items: stretch; pointer-events: auto; box-sizing: border-box;\n  background: var(--pbt-ui-bg); color: var(--pbt-ui-fg); border: 1px solid var(--pbt-ui-border); border-radius: var(--pbt-ui-radius);\n  box-shadow: var(--pbt-ui-shadow); font: 12px/1.3 var(--pbt-ui-font);\n}\n.pbt-dock-tabs { flex: none; display: flex; align-items: center; gap: 2px; padding: 4px 8px; border-right: 1px solid color-mix(in srgb, var(--pbt-ui-fg) 18%, transparent); }\n.pbt-dock-tab {\n  display: inline-flex; align-items: center; gap: 5px; border: 1px solid transparent; border-bottom-width: 2px; border-radius: 3px 3px 0 0;\n  background: transparent; color: inherit; font: 11px var(--pbt-ui-font); padding: 1px 7px; cursor: pointer; white-space: nowrap;\n}\n.pbt-dock-tab:hover, .pbt-dock-tab:focus-visible { border-color: color-mix(in srgb, var(--pbt-accent) 60%, transparent); outline: none; }\n/* The dot: this tool is on (drawing on the page). The underline: its strip is the one shown. */\n.pbt-dock-dot { flex: none; width: 6px; height: 6px; border-radius: 50%; background: #1e8e5a; box-shadow: 0 0 0 1px color-mix(in srgb, var(--pbt-ui-bg) 70%, transparent); }\n.pbt-dock-tab.pbt-on { border-bottom-color: var(--pbt-accent); font-weight: 700; background: color-mix(in srgb, var(--pbt-accent) 14%, transparent); }\n.pbt-dock-body { flex: 1; min-width: 0; display: flex; }\n/* A strip in the dock is the dock's content: no frame or position of its own. */\n.pbt-dock .pbt-qa-strip { position: static; flex: 1; min-width: 0; border: 0; box-shadow: none; border-radius: 0; background: transparent; }\n/* Every strip in the dock is as tall as the ta)PBT"
LR"PBT(llest (Stencil's, with its 22 px inputs and its 4 px\n   padding), contents centred, so the page above does not jump when another tab is shown. */\n.pbt-dock .pbt-qa-strip { min-height: 30px; align-items: center; }\n.pbt-dock .pbt-qa-strip input { height: 22px; }\n/* Power BI's page has no [hidden] rule of its own, and the strips are display: flex. */\n.pbt-dock [hidden] { display: none !important; }\n\n/* ---- Badge rails (page/00-core.js, pbt.rail): one row of check badges per visual, just outside its\n   bottom-right corner, tethered to its bottom edge in the worst badge's severity color (inside the\n   corner when the gap below is too short); Filter map, Theme, then Colors. ---- */\n.pbt-rails { position: fixed; left: 0; top: 0; width: 0; height: 0; z-index: 2147482990; pointer-events: none; }\n.pbt-rail { position: absolute; transform: translateX(-100%); display: flex; align-items: center; gap: 3px; white-space: nowrap; pointer-events: none; }\n.pbt-rail[hidden] { display: none; }\n.pbt-rail-tether { position: absolute; left: 3px; right: 3px; bottom: 100%; height: 4px; box-sizing: border-box; border: 2px solid rgba(90, 107, 122, .9); border-bottom: 0; pointer-events: none; }\n.pbt-rail[data-sev=\"bad\"] .pbt-rail-tether { border-color: #c4314b; }\n.pbt-rail[data-sev=\"warn\"] .pbt-rail-tether { border-color: #9c6609; }\n.pbt-rail[data-sev=\"info\"] .pbt-rail-tether { border-color: #5f6b78; }\n.pbt-rail[data-sev=\"ok\"] .pbt-rail-tether { border-color: #1c8353; }\n.pbt-rail.pbt-rail-in .pbt-rail-tether { display: none; }\n.pbt-rail > [data-rail-tool] { position: static; transform: none; margin: 0; }\n/* Every badge leads with its tool's letter (F, T, C), so its color can mean severity alone. */\n.pbt-rail > [data-rail-tool]::before {\n  content: attr(data-glyph); display: inline-block; margin-right: 4px; padding-right: 4px; border-right: 1px solid rgba(255, 255, 255, .45);\n  font: 700 9px/1 Consolas, monospace; color: #fff; opacity: .85;\n}\n.pbt-rail > .pbt-fm-badge::before { margin-right: 1px; }\n.pbt-rail > [data-rail-tool=\"filters\"] { order: 1; }\n.pbt-rail > [data-rail-tool=\"theme\"] { order: 2; }\n.pbt-rail > [data-rail-tool=\"colors\"] { order: 3; }\n/* Focus mode: a tool whose strip the dock does not show draws a dot with its letter; full on hover\n   and while its card is open. */\n.pbt-rail > .pbt-dot:not(:hover):not(.pbt-tip-open) {\n  width: 15px; height: 15px; padding: 0; gap: 0; box-sizing: border-box; border-radius: 50%; outline: 0;\n  display: inline-flex; align-items: center; justify-content: center; font-size: 0;\n}\n.pbt-rail > .pbt-dot:not(:hover):not(.pbt-tip-open) > * { display: none; }\n.pbt-rail > .pbt-dot:not(:hover):not(.pbt-tip-open)::before { content: attr(data-glyph); margin: 0; padding: 0; border: 0; opacity: 1; font: 700 9px/1 Consolas, monospace; color: #fff; }\n/* Layout QA's guide tags while a check tool is on: kept in place (the rails avoid them) but shown\n   only for the hovered visual. */\n.pbt-qa.pbt-qa-quiet .pbt-guide-tag:not(.pbt-hov) { visibility: hidden; }\n\n/* Pinpointing (pbt.pinpoint): the parts a finding is about, outlined and numbered while its badge\n   (or swatch) is hovered; the numbers match the card's rows. Under the card, over the page. */\n.pbt-hit { position: fixed; z-index: 2147482996; box-sizing: border-box; border: 2px solid #c4314b; border-radius: 3px; box-shadow: 0 0 0 1px rgba(255, 255, 255, .85); pointer-events: none; }\n.pbt-hit-pin, .pbt-hit-num {\n  display: inline-flex; align-items: center; justify-content: center; min-width: 16px; height: 16px; padding: 0 3px; box-sizing: border-box;\n  border-radius: 8px; background: #c4314b; color: #fff; font: 700 10px/1 Consolas, monospace;\n}\n.pbt-hit-pin { position: absolute; top: -10px; left: -10px; box-shadow: 0 1px 3px rgba(0, 0, 0, .35); }\n/* The pins' own layer, drawn after every outline so no outline crosses a pin; each pin is placed\n   in window coordinates. One red for every pin: color mea)PBT"
LR"PBT(ns severity only. */\n.pbt-hit.pbt-hit-pins { left: 0; top: 0; width: 0; height: 0; border: 0; border-radius: 0; box-shadow: none; }\n/* A pinned card's row brought forward: the other outlines and pins fade. */\n.pbt-hit.pbt-hit-dim, .pbt-hit-pin.pbt-hit-dim { opacity: .2; }\n.pbt-tip .pbt-hit-num { display: inline-flex; }\n\n/* Help panel: the header is the drag handle that moves the panel (page/70-help.js), and each tab\n   reads as a one-line summary followed by short points, each led by a bold term. The tab buttons\n   and the close button keep their pointer cursor from their own rules above. */\n.pbt-help header { cursor: move; user-select: none; touch-action: none; }\n.pbt-help .pbt-help-lead { opacity: 1; margin-bottom: 4px; }\n.pbt-help .pbt-help-pts li { margin: 3px 0; }\n\n/* Interactions bar: run feedback and the Highlight preview (page/36-interactions.js). */\n/* The button whose run is going shows its count; the others wait, greyed by aria-disabled. After\n   a run the button says what happened for a few seconds: a green edge when it all went through,\n   a red one when it stopped or some visuals did not change. */\n.pbt-ia-btn[aria-busy=\"true\"] { cursor: progress; border-color: var(--pbt-accent, currentColor); background: color-mix(in srgb, currentColor 14%, transparent); }\n.pbt-ia-btn.pbt-ia-done { border-color: #107c41; box-shadow: inset 0 0 0 1px #107c41; }\n.pbt-ia-btn.pbt-ia-warn { border-color: #d13438; box-shadow: inset 0 0 0 1px #d13438; }\n/* While the pointer is on the Highlight button: an outlined visual changes, a dashed one is\n   already set to Highlight, a dimmed one cannot be highlighted. The marks never take the pointer. */\n.pbt-ia-marks { position: absolute; inset: 0; pointer-events: none; }\n.pbt-ia-mark { position: absolute; box-sizing: border-box; border-radius: 2px; pointer-events: none; }\n.pbt-ia-mark-yes { outline: 2px solid #107c41; outline-offset: 1px; background: rgba(16, 124, 65, .06); }\n.pbt-ia-mark-same { outline: 2px dashed #107c41; outline-offset: 1px; }\n.pbt-ia-mark-no { outline: 1px dashed rgba(96, 94, 92, .7); outline-offset: 1px; background: rgba(255, 255, 255, .6); }\n\n/* Stencil strip and menus, round 3 (page/38-stencil.js): the \"\u2026\" button after the common types,\n   the menu of every other visual type (scrolls when taller than the window allows, the current\n   type in bold), the neutral square for a type with no Power BI icon, the delete button and the\n   Convert to visuals button (dimmed while no report files are loaded; a click says why). */\n.pbt-st-strip .pbt-st-more-btn { width: 26px; height: 22px; padding: 0; display: inline-flex; align-items: center; justify-content: center; font-weight: 700; line-height: 1; }\n.pbt-fm-menu.pbt-st-more { max-height: min(70vh, 560px); overflow-y: auto; }\n.pbt-st-more .pbt-fm-item[aria-current=\"true\"] { font-weight: 600; }\n.glyphicon.pbt-st-glyph-any::before { content: ''; display: inline-block; width: .85em; height: .85em; box-sizing: border-box; border: 1.5px solid currentColor; border-radius: 2px; vertical-align: -.1em; }\n.pbt-st-strip .pbt-st-del { width: 26px; height: 22px; padding: 0; display: inline-flex; align-items: center; justify-content: center; }\n.pbt-st-strip .pbt-st-trash { display: block; }\n.pbt-st-strip .pbt-st-convert { white-space: nowrap; font-weight: 600; }\n.pbt-st-strip .pbt-st-convert[aria-disabled=\"true\"] { opacity: .55; }\n";
// ---- 00-core.js ----
// Runs inside Power BI Desktop's report page. inject.mjs concatenates every page/*.js file
// into one function with PBT_CONFIG, PBT_SKINS and PBT_BASE_CSS in scope, then calls pbt.start().
// The report page itself (.displayArea and the visuals in it) is never restyled: skins
// change the app around it, and the tools only draw overlays on top of it.

// Hidden subframes get nothing; only the top window is ours.
if (window.top !== window) return 'skipped subframe';
// Each install replaces the previous one (hot reload, a restarted injector).
if (window.__pbt) { try { window.__pbt.remove(); } catch {} }
const gen = window.__pbtGen = (window.__pbtGen || 0) + 1;

// Provenance fingerprint: marks this code as Skins & Tools for Power BI, by Reid Havens, Analytic
// Endeavors, licensed CC BY-ND 4.0. The same code is in the mod's header comment and readme and in
// skins/_base.css, sits on our overlay hosts as data-ae-fp and shows in the help panel's footer;
// scripts/build-mod.mjs fails a build that loses any copy. credit is the line the menu and the
// help panel show. Frozen, and exposed as pbt.fingerprint.
const FINGERPRINT = Object.freeze({ code: 'AE-PBT-29A1ED34-2026', owner: 'Reid Havens, Analytic Endeavors', license: 'CC BY-ND 4.0', credit: 'Skins & Tools for Power BI by Reid Havens, Analytic Endeavors' });

const pbt = window.__pbt = {
  fingerprint: FINGERPRINT,
  cfg: PBT_CONFIG,
  skins: PBT_SKINS,
  cleanups: [],
  modules: [],
  hotkeys: [],
  current: () => window.__pbtGen === gen,
};

// ---- helpers ----
// Elements are built with createElement only, in case the app turns on Trusted Types.
pbt.el = (tag, props = {}, ...children) => {
  const e = document.createElement(tag);
  for (const [k, v] of Object.entries(props)) {
    if (v == null) continue; // an attribute left out
    if (k === 'class') e.className = v;
    else if (k === 'text') e.textContent = v;
    else if (k === 'style') e.style.cssText = v;
    else if (k.startsWith('on')) e.addEventListener(k.slice(2), v);
    else if (k === 'dataset') Object.assign(e.dataset, v);
    else e.setAttribute(k, v);
  }
  for (const c of children.flat()) if (c != null) e.append(c);
  return e;
};

pbt.style = (id, css) => {
  let s = document.getElementById(id);
  if (!s) { s = document.createElement('style'); s.id = id; s.dataset.pbt = ''; document.head.append(s); }
  if (s.textContent !== css) s.textContent = css;
  return s;
};

pbt.on = (target, type, fn, opts) => {
  target.addEventListener(type, fn, opts);
  pbt.cleanups.push(() => targe)PBT"
LR"PBT(t.removeEventListener(type, fn, opts));
};

pbt.every = (ms, fn) => {
  const t = setInterval(() => { if (pbt.current()) fn(); }, ms);
  pbt.cleanups.push(() => clearInterval(t));
};

// Two hosts, named by PBT_HOST:
//   'mod'       the Windhawk mod inside Power BI. Choices live in this origin's localStorage,
//               which every Power BI view shares and which survives restarts; views follow
//               each other through the storage event. Never WebView2's message channel:
//               Power BI's own bridge parses every message there as one of its commands.
//   'devtools'  the development injector: DevTools bindings pbtSave and pbtLog.
const HOST = typeof PBT_HOST !== 'undefined' ? PBT_HOST : 'devtools';
const STORE_KEY = 'pbid-skins';
pbt.log = msg => {
  try {
    if (HOST === 'devtools' && window.pbtLog) window.pbtLog(String(msg));
    else console.debug('[pbid-skins]', String(msg));
  } catch {}
};

// The skin as loaded, kept as a value: pbt.cfg is the loaded config object itself, and the skin
// setter changes it before saving, so comparing against PBT_CONFIG.skin would miss the first save.
const START_SKIN = PBT_CONFIG.skin;
pbt.save = () => {
  // Development trace for the stray skin save (a test run once left the applied test skin in
  // tweaks.json): log where a save that changes the skin came from.
  const was = pbt.savedSkin !== undefined ? pbt.savedSkin : START_SKIN;
  if (HOST === 'devtools' && pbt.cfg.skin !== was) pbt.log(`skin saved as ${pbt.cfg.skin} (was ${was}) by ${(new Error().stack || '').split('\n').slice(2, 7).map(s => s.trim()).join(' < ')}`);
  pbt.savedSkin = pbt.cfg.skin;
  try {
    if (HOST === 'mod') localStorage.setItem(STORE_KEY, JSON.stringify({ settings: typeof PBT_SETTINGS_KEY !== 'undefined' ? PBT_SETTINGS_KEY : '', cfg: pbt.cfg }));
    else if (window.pbtSave) window.pbtSave(JSON.stringify(pbt.cfg));
  } catch (e) {
    // A full or blocked store (QuotaExceededError) would otherwise look saved and be gone at the
    // next start. Said once per page; the choices still apply for this session.
    pbt.log(`save failed: ${e && e.message}`);
    if (!pbt.saveFailed) {
      pbt.saveFailed = true;
      try { pbt.toast('Choices and Stencil blocks could not be saved; they last until Power BI closes.', { tone: 'warn', title: 'Skins & Tools', ms: 8000 }); } catch {}
    }
  }
};

// A choice made in another view (Report, Table, DAX query, ...) arrives as a storage event.
pbt.receive = cfg => {
  if (!cfg || typeof cfg.skin !== 'string' || typeof cfg.features !== 'object') return;
  pbt.cfg = cfg;
  if (pbt.skin) pbt.skin.apply(cfg.skin);
  pbt.modules.forEach(m => m.onConfig && m.onConfig());
};
if (HOST === 'mod') {
  pbt.on(window, 'storage', e => {
    if (e.key !== STORE_KEY || !e.newValue) return;
    try { pbt.receive(JSON.parse(e.newValue).cfg); } catch {}
  });
}

// Per-viewer conveniences (collapsed menu sections, the last help tab) live in the page's own
// localStorage under one key; never the choices themselves, which have their store above.
const UI_KEY = 'pbid-skins-ui';
pbt.ui = {
  read() { try { return JSON.parse(localStorage.getItem(UI_KEY) || '{}') || {}; } catch { return {}; } },
  get(key, dflt) { const v = this.read()[key]; return v === undefined ? dflt : v; },
  set(key, value) { try { const s = this.read(); s[key] = value; localStorage.setItem(UI_KEY, JSON.stringify(s)); } catch {} },
  reset() { try { localStorage.removeItem(UI_KEY); } catch {} },
};

pbt.feature = name => !!(pbt.cfg.features || {})[name];
pbt.setFeature = (name, on) => {
  pbt.cfg.features[name] = on;
  pbt.save();
  pbt.modules.forEach(m => m.onConfig && m.onConfig());
};

// One body observer shared by every module, batched with a short timer. Not animation frames:
// a covered or minimized window gets none, so the batch would wait until it paints again (the
// Skins group then stayed on Home after a tab switch).
const watchers = new Set();
pbt.watch = fn => { watchers.add(fn); pbt.cleanups.push(() => watchers.delete(fn)); };
let watchQueued = false;
const observer = new MutationObserver(muts => {
  if (watchQueued || !pbt.current()) return;
  // Mutations inside our own nodes do not count; otherwise drawing an overlay would wake the observer again.
  if (muts.every(m => m.target.closest && m.target.closest('[data-pbt]'))) return;
  watchQueued = true;
  setTimeout(() => { watchQueued = false; watchers.forEach(fn => { try { fn(); } catch (e) { pbt.log(e.stack); } }); }, 30);
});
pbt.cleanups.push(() => observer.disconnect());

// ---- hotkeys (Ctrl+Alt+<key>, capture phase so Power BI does not see the ones we take) ----
// extra.tool lists the key under Tools in the menu, with extra.active() as its check mark.
// Typed text is never taken. On many layouts (German, French, Nordic, Spanish, Italian, Czech, ...)
// AltGr types [ and ], and Windows reports AltGr as Ctrl+Alt. So the bracket keys never fire while
// the focus is in a text field or an editor (the formula bar, DAX query and TMDL views are Monaco),
// nor when the browser flags AltGraph. A letter hotkey still fires in a text field when the key
// typed is an ASCII letter, which no layout types with AltGr.
// Match order: the ASCII letter typed; else the physical letter key (Cyrillic, Greek, Polish AltGr letters),
// outside text fields only; else the character itself (the brackets). Physical keys come second so
// AZERTY's A stays on A, and before the brackets so Czech Ctrl+Alt+F (which yields "[") is F.
pbt.hotkey = (key, label, fn, extra = {}) => pbt.hotkeys.push({ key: key.toLowerCase(), label, fn, ...extra });
// A switch, check box or button with the focus types nothing, so it does not count as a field.
const NO_TEXT = /^(checkbox|radio|range|button|submit|reset|color|file|image|hidden)$/i;
const typingIn = n => !!n && n.nodeType === 1 && (n.isContentEditable || /^(TEXTAREA|SELECT)$/.test(n.tagName) || (n.tagName === 'INPUT' && !NO_TEXT.test(n.type || '')) || !!(n.closest && n.)PBT"
LR"PBT(closest('.monaco-editor')));
pbt.on(window, 'keydown', e => {
  if (!e.ctrlKey || !e.altKey || e.metaKey) return;
  const inField = typingIn((e.composedPath && e.composedPath()[0]) || e.target);
  const altGraph = !!(e.getModifierState && e.getModifierState('AltGraph'));
  const k = (e.key || '').toLowerCase();
  const letter = /^[a-z]$/.test(k) ? k : !inField && /^Key[A-Z]$/.test(e.code || '') ? e.code.slice(3).toLowerCase() : '';
  const hk = letter ? pbt.hotkeys.find(h => h.key === letter)
    : inField || altGraph ? null : pbt.hotkeys.find(h => h.key === k);
  if (!hk) return;
  e.preventDefault();
  e.stopImmediatePropagation();
  hk.fn();
}, true);

// ---- badges and the visual they belong to ----
// A tool's badge on a visual carries the visual's index (data-vi), and while the pointer is on
// it, an outline marks that visual: a text box or an empty-looking visual has no frame of its own,
// and its badge can otherwise look as if it belonged to a neighbour.
pbt.ownBadge = (badge, v) => {
  badge.dataset.vi = String(v.i);
  badge.classList.add('pbt-nudge');
  const show = () => {
    // A pinned card keeps its own visual outlined (pbt.tip).
    if (pbt.tipPinned()) return;
    pbt.hideOwner();
    const r = v.el.isConnected ? v.el.getBoundingClientRect() : null;
    if (!r || !r.width) return;
    document.body.append(pbt.el('div', { class: 'pbt-owner', 'data-pbt': '', style: `left:${r.left}px;top:${r.top}px;width:${r.width}px;height:${r.height}px` }));
  };
  badge.addEventListener('pointerenter', show);
  badge.addEventListener('pointerleave', pbt.hideOwner);
  return badge;
};
pbt.hideOwner = force => { if (pbt.tipPinned() && force !== true) return; document.querySelectorAll('.pbt-owner').forEach(e => e.remove()); };

// ---- passes: a check tool badges only the visuals with something to report, unless the shared
// "show passes" switch is on (overlays.passes; one setting for every check tool) ----
pbt.passes = () => !!(pbt.cfg.overlays || {}).passes;
pbt.checkTools = () => ({ filters: pbt.filters, theme: pbt.themecheck, colors: pbt.colors });
// Every check tool that is on draws again at once (its signature keys carry the setting too, so a
// change made in another view is picked up on the tools' own timers).
pbt.redrawChecks = () => {
  const t = pbt.checkTools();
  if (t.filters && t.filters.on && t.filters.result) { t.filters.drawKey = ''; t.filters.draw(); }
  for (const k of ['theme', 'colors']) if (t[k] && t[k].on) { t[k].sig = ''; t[k].dirty = true; t[k].draw(false); }
};
pbt.setPasses = on => { pbt.cfg.overlays = { ...(pbt.cfg.overlays || {}), passes: !!on }; pbt.save(); pbt.redrawChecks(); };
// The two strip controls every check tool shares: how many visuals are fine, and the switch.
pbt.passItems = (pass, total, what) => {
  const el = pbt.el, on = pbt.passes();
  const fine = pbt.tip(el('span', { class: 'pbt-strip-item', dataset: { count: 'ok' } }, el('span', { class: 'pbt-card-dot pbt-dot-ok' }), el('b', { text: `${pass} of ${total}` }), el('span', { text: 'fine' })),
    `${pass} of ${total} visuals with nothing to report\n${what}${on ? '' : '\nThey get no badge while "show passes" is off.'}`);
  const sw = pbt.tip(el('button', { class: `pbt-strip-grid pbt-strip-passes${on ? ' pbt-on' : ''}`, text: 'show passes', 'aria-pressed': String(on),
    onpointerdown: e => { e.stopPropagation(); e.preventDefault(); }, onclick: e => { e.stopPropagation(); pbt.setPasses(!on); } }),
  'Show passes\nA badge on every visual, the fine ones too. Off, only visuals with something to report get one. One setting for every check tool.');
  return { fine, sw };
};

// ---- badge rails: one row per visual for every check tool's badge ----
// Just outside the visual's bottom-right corner, in a fixed order (Filter map, Theme, then Colors),
// with a tether in the worst badge's severity color up to the visual's bottom edge so the row
// cannot be read as the next visual's (inside the corner when the gap below is too short). Rails live in one host in window coordinates and are placed from their visual's
// own box on every nudge, so they follow a move, an undo, a pane collapsing or a resize even
// before the tools redraw.
const RAIL_GAP = 4;
// Inside its visual, a rail keeps this far from the frame, clear of Power BI's resize handles.
const RAIL_INSET = 8;
const RAIL_GLYPH = { filters: 'F', theme: 'T', colors: 'C' };
pbt.railGap = RAIL_GAP;
pbt.rails = {
  host: null,
  byEl: new WeakMap(),
  ensureHost() {
    if (!this.host || !this.host.isConnected) { this.host = pbt.el('div', { class: 'pbt-rails', 'data-pbt': '', 'data-ae-fp': FINGERPRINT.code }); document.body.append(this.host); }
    return this.host;
  },
  // The rail's resting place from its visual's drawn box; false when the visual is gone.
  place(rail) {
    const el = rail._vel, r = el && el.isConnected ? el.getBoundingClientRect() : null;
    if (!r || !r.width) return false;
    const left = `${r.right}px`, top = `${r.bottom + RAIL_GAP}px`;
    if (rail.style.left !== left) rail.style.left = left;
    if (rail.style.top !== top) rail.style.top = top;
    return true;
  },
  all() { return this.host && this.host.isConnected ? [...this.host.querySelectorAll(':scope > .pbt-rail')] : []; },
};
pbt.rail = v => {
  const host = pbt.rails.ensureHost();
  let rail = pbt.rails.byEl.get(v.el);
  if (!rail || !rail.isConnected) {
    rail = pbt.el('div', { class: 'pbt-rail', dataset: { vi: String(v.i) } }, pbt.el('span', { class: 'pbt-rail-tether' }));
    rail._vel = v.el;
    pbt.rails.byEl.set(v.el, rail);
    host.append(rail);
  }
  pbt.rails.place(rail);
  return rail;
};
// A tool's badge for a visual into that visual's rail, replacing the tool's previous one there.
let nudgeSoon = false;
pbt.railPut = (badge, v, tool) => {
  const rail = pbt.rail(v);
  rail.querySelectorAll(`:scope > [data-rail-tool="${tool}"]`).forEach(b => b.remove());
  // The badge replaced may be the one whose card is pinned.
  if (pbt.checkPin) pbt.chec)PBT"
LR"PBT(kPin();
  badge.dataset.railTool = tool;
  badge.dataset.glyph = RAIL_GLYPH[tool] || '?';
  badge.classList.toggle('pbt-dot', !pbt.focusFull(tool));
  // A badge with a card: a click pins the card open (pbt.tip).
  if (badge._pbtTipBuild) badge.dataset.pbtPin = '';
  rail.append(badge);
  // Placed among the other marks right away, not on the next 400 ms tick.
  if (!nudgeSoon) { nudgeSoon = true; setTimeout(() => { nudgeSoon = false; if (pbt.current()) pbt.nudgeBadges(); }, 0); }
  return badge;
};
// A tool's badges leave every rail (the tool turned off or redraws); empty rails go.
pbt.railClear = tool => {
  for (const rail of pbt.rails.all()) {
    rail.querySelectorAll(`:scope > [data-rail-tool="${tool}"]`).forEach(b => b.remove());
    if (!rail.querySelector(':scope > [data-rail-tool]')) rail.remove();
  }
  // A card pinned from one of the badges just removed goes with it.
  if (pbt.checkPin) pbt.checkPin();
};

// ---- focus mode: with two or more check tools on, the tool whose strip the dock shows draws full
// badges and the others shrink to a dot with the tool's letter, full again under the pointer.
// While the dock shows another kind of strip (Layout QA, Stencil), the check tool shown last
// keeps its full badges. ----
pbt.focusFull = tool => {
  const t = pbt.checkTools(), on = Object.keys(t).filter(n => t[n] && t[n].on);
  if (on.length < 2) return true;
  const d = pbt.dock, lead = on.includes(d.shown) ? d.shown : on.includes(d.lastCheck) ? d.lastCheck : on[on.length - 1];
  return tool === lead;
};
pbt.applyFocus = () => {
  for (const b of document.querySelectorAll('.pbt-rail > [data-rail-tool]')) b.classList.toggle('pbt-dot', !pbt.focusFull(b.dataset.railTool));
  // Layout QA's guide tags show only for the hovered visual while a check tool is on as well.
  const t = pbt.checkTools();
  // When that changes, which tags show changes too, so Layout QA places its tags again.
  if (pbt.qa && pbt.qa.host) {
    const h = pbt.qa.host, was = h.classList.contains('pbt-qa-quiet');
    if (h.classList.toggle('pbt-qa-quiet', Object.values(t).some(x => x && x.on)) !== was && pbt.qa.declutter) pbt.qa.declutter();
  }
};

// ---- the nudger ----
const hit = (a, b) => a.left < b.right - 1 && b.left < a.right - 1 && a.top < b.bottom - 1 && b.top < a.bottom - 1;
const overlapArea = (a, b) => Math.max(0, Math.min(a.right, b.right) - Math.max(a.left, b.left)) * Math.max(0, Math.min(a.bottom, b.bottom) - Math.max(a.top, b.top));
// What a rail must never cover: Layout QA's labels and red guide tags (also the tags hidden until
// hover, so a rail does not jump when one shows), every visual's title band, the dock, the page
// tabs and the status bar. The blue hover distances are left out: they come and go with the pointer.
const railObstacles = () => {
  const out = [...document.querySelectorAll('.pbt-qa .pbt-label, .pbt-qa .pbt-guide-tag, .pbt-dock, .carouselScrollPane, section.statusBar')].map(e => e.getBoundingClientRect()).filter(r => r.width);
  for (const el of document.querySelectorAll('.displayArea .visualContainer')) {
    const r = el.getBoundingClientRect();
    if (!r.width) continue;
    const t = el.querySelector('.visualTitle'), tr = t && t.getBoundingClientRect();
    const bottom = Math.min(r.bottom, Math.max(r.top + 12, tr && tr.height ? tr.bottom : 0));
    out.push({ left: r.left, right: r.right, top: r.top, bottom });
  }
  return out;
};
// Each rail rests below its visual; when that spot is taken it slides left along the visual's
// bottom edge (never past it, so never toward a neighbour), then tries the same inside the
// visual's bottom edge, and failing all that takes the spot that covers least. When the gap below
// the visual (to the next visual under it, or the page's edge) is too short for a rail, a rail
// there would read as the next visual's, so it goes inside the bottom-right corner at once.
const RAIL_SEV = ['bad', 'warn', 'info', 'ok'];
const placeRails = placed => {
  const g = pbt.page.geometry();
  const boxes = [...document.querySelectorAll('.displayArea .visualContainer')].map(el => ({ el, r: el.getBoundingClientRect() })).filter(b => b.r.width);
  for (const rail of pbt.rails.all()) {
    if (!rail.querySelector(':scope > [data-rail-tool]')) { rail.remove(); continue; }
    const shown = pbt.rails.place(rail);
    rail.hidden = !shown;
    if (!shown) continue;
    const wasX = +rail.dataset.nx || 0, wasY = +rail.dataset.ny || 0, r = rail.getBoundingClientRect();
    const base = { left: r.left - wasX, right: r.right - wasX, top: r.top - wasY, bottom: r.bottom - wasY };
    const at = (dx, dy) => ({ left: base.left + dx, right: base.right + dx, top: base.top + dy, bottom: base.bottom + dy });
    // A rail under the pointer, or with a badge whose card is open, stays put: its badge may have
    // grown from a dot, and moving it would pull it from under the pointer.
    if (rail.matches(':hover') || rail.querySelector(':scope > .pbt-tip-open')) { placed.push(at(wasX, wasY)); continue; }
    // The tether wears the worst severity among the rail's badges.
    const sev = RAIL_SEV.find(s => rail.querySelector(`:scope > .pbt-sev-${s}`)) || '';
    if ((rail.dataset.sev || '') !== sev) rail.dataset.sev = sev;
    const vr = rail._vel.getBoundingClientRect();
    const slide = Math.max(0, vr.width - r.width), inside = -(r.height + RAIL_GAP + RAIL_INSET);
    let room = g ? g.rect.bottom - vr.bottom : Infinity;
    for (const b of boxes) if (b.el !== rail._vel && b.r.top >= vr.bottom - 1 && b.r.left < vr.right - 1 && vr.left < b.r.right - 1) room = Math.min(room, b.r.top - vr.bottom);
    // Short: less than the rail plus its own height of air below it, so it would sit about as near
    // the next visual as its own.
    const short = room < r.height * 2 + RAIL_GAP;
    const tries = [];
    for (const dy of short ? [inside] : [0, inside]) {
      // Inside, clear of both side frames by RAIL_INSET; with less room than that, centered.
      const lo = dy <)PBT"
LR"PBT( 0 ? (slide < 2 * RAIL_INSET ? Math.round(slide / 2) : RAIL_INSET) : 0, hi = dy < 0 ? Math.max(lo, slide - RAIL_INSET) : slide;
      for (let dx = lo; dx < hi; dx += 8) tries.push([-dx, dy]);
      tries.push([-hi, dy]);
    }
    let best = null;
    for (const [dx, dy] of tries) {
      const q = at(dx, dy), cost = placed.reduce((s, p) => s + (hit(q, p) ? overlapArea(q, p) : 0), 0);
      if (!best || cost < best.cost) best = { dx, dy, cost };
      if (!cost) break;
    }
    const { dx, dy } = best;
    if (dx !== wasX || dy !== wasY) { rail.style.translate = dx || dy ? `${dx}px ${dy}px` : ''; rail.dataset.nx = String(dx); rail.dataset.ny = String(dy); }
    rail.classList.toggle('pbt-rail-in', dy < 0);
    placed.push(at(dx, dy));
  }
};
// Rails first (above); then any other badge that lands on something is slid sideways to the nearest
// free spot, Layout QA's labels staying put. Offsets are kept in data-nx and data-ny so a badge's
// or rail's own position can still be measured.
pbt.nudgeBadges = () => {
  const placed = railObstacles();
  placeRails(placed);
  const g = pbt.page.geometry();
  for (const e of document.querySelectorAll('.pbt-nudge')) {
    if (e.closest('.pbt-rail')) continue;
    if (!e.isConnected || !e.offsetParent) continue;
    const wasX = +e.dataset.nx || 0, wasY = +e.dataset.ny || 0, r = e.getBoundingClientRect();
    const base = { left: r.left - wasX, right: r.right - wasX, top: r.top - wasY, bottom: r.bottom - wasY, width: r.width, height: r.height };
    const at = (dx, dy) => ({ left: base.left + dx, right: base.right + dx, top: base.top + dy, bottom: base.bottom + dy });
    let best = [0, 0];
    if (placed.some(p => hit(at(0, 0), p))) {
      // Sideways first; when the gap is full, up into its own visual's bottom edge.
      const tries = [];
      for (const dy of [0, -(base.height + 4)]) { if (dy) tries.push([0, dy]); for (let k = 1; k <= 8; k++) for (const s of [-1, 1]) tries.push([s * k * (base.width + 4), dy]); }
      best = tries.find(([dx, dy]) => { const q = at(dx, dy); return !(g && (q.left < g.rect.left || q.right > g.rect.left + g.rect.width)) && !placed.some(p => hit(q, p)); }) || [0, 0];
    }
    const [dx, dy] = best;
    if (dx !== wasX || dy !== wasY) { e.style.translate = dx || dy ? `${dx}px ${dy}px` : ''; e.dataset.nx = String(dx); e.dataset.ny = String(dy); }
    placed.push(at(dx, dy));
  }
};
// Pinpointing: the parts a finding is about, outlined in red on the page with a numbered pin on
// the first of each group (the number matches the row in the card); one call replaces the last.
// groups: [{ els: [elements], n: number or '' }].
// While a card is pinned open (pbt.tip), its outlines stay: other badges' hovers neither replace
// nor clear them, and they go when the card is unpinned (force).
pbt.pinpoint = (groups, force) => {
  if (pbt.tipPinned() && force !== true) return;
  pbt.unpinpoint(true);
  // Each pin's spot: just outside its outline's top-left corner.
  const pins = [];
  for (const grp of groups) {
    let first = true;
    const n = grp.n !== '' && grp.n != null ? String(grp.n) : '';
    for (const e of (grp.els || []).slice(0, 40)) {
      if (!e || !e.isConnected) continue;
      const r = e.getBoundingClientRect();
      if (r.width < 1 || r.height < 1) continue;
      document.body.append(pbt.el('div', { class: 'pbt-hit', 'data-pbt': '', dataset: { n }, style: `left:${r.left - 2}px;top:${r.top - 2}px;width:${r.width + 4}px;height:${r.height + 4}px` }));
      if (first && n) pins.push({ n, left: r.left - 12, top: r.top - 12 });
      first = false;
    }
  }
  if (!pins.length) return;
  // The pins live in one layer drawn after every outline. A pin inside its own outline box was
  // covered by any outline drawn after it (two findings on the whole visual: outline 2's border ran
  // across pin 1), and pins were set apart only when their corners matched to the pixel. Now pins
  // are placed in number order, and a pin that would touch one already placed moves right of it,
  // so 1 is leftmost and none covers another.
  const layer = pbt.el('div', { class: 'pbt-hit pbt-hit-pins', 'data-pbt': '' });
  document.body.append(layer);
  const placed = [];
  for (const p of pins.sort((a, b) => (+a.n || 0) - (+b.n || 0))) {
    const pin = pbt.el('span', { class: 'pbt-hit-pin', dataset: { n: p.n }, text: p.n });
    layer.append(pin);
    const w = pin.offsetWidth || 16, h = pin.offsetHeight || 16;
    let x = p.left;
    for (let guard = 0; guard < 60; guard++) {
      const over = placed.find(q => x < q.right + 2 && q.left < x + w + 2 && p.top < q.bottom && q.top < p.top + h);
      if (!over) break;
      x = over.right + 2;
    }
    pin.style.left = `${x}px`;
    pin.style.top = `${p.top}px`;
    placed.push({ left: x, right: x + w, top: p.top, bottom: p.top + h });
  }
};
pbt.unpinpoint = force => { if (pbt.tipPinned() && force !== true) return; document.querySelectorAll('.pbt-hit').forEach(e => e.remove()); };
// One finding brought forward from a pinned card's row: its outline and pin stay, the others dim.
// n is the finding's number; els (for a row with no number) are parts outlined just for the
// moment. pbt.pinFocus() with nothing puts every outline back.
pbt.pinFocus = (n, els) => {
  document.querySelectorAll('.pbt-hit-extra').forEach(e => e.remove());
  const want = n != null && n !== '' ? String(n) : null;
  const dim = want != null || !!(els && els.length);
  for (const e of document.querySelectorAll('.pbt-hit:not(.pbt-hit-pins), .pbt-hit-pins > .pbt-hit-pin')) e.classList.toggle('pbt-hit-dim', dim && e.dataset.n !== want);
  for (const e of (els || []).slice(0, 40)) {
    if (!e || !e.isConnected) continue;
    const r = e.getBoundingClientRect();
    if (r.width < 1 || r.height < 1) continue;
    document.body.append(pbt.el('div', { class: 'pbt-hit pbt-hit-extra', 'data-pbt': '', style: `left:${r.left - 2}px;top:${r.top - 2}px;width:${r.width + 4}px;height:${r.height + 4}px` }));
  }
};
// A )PBT"
LR"PBT(number for a card row, matching the pin on the page.
pbt.pinNum = n => pbt.el('span', { class: 'pbt-hit-num', text: String(n) });

// ---- the tools' strips: one dock under the page ----
// Every tool's strip lives in a single dock: just below the page, but never lower than the
// visible canvas, so it never covers the page tabs or the status bar however many tools are on.
// With one tool on, the dock is that tool's strip. With several, a row of tabs (one per tool)
// leads the bar and the chosen tool's strip fills the rest of the same line: one line, always.
// The newest tool turned on is shown; a tab picks another, and the choice is remembered.
// The tools by job, in the one order the menu, the dock, the help and the docs all use.
pbt.JOBS = [['plan', 'Plan', ['stencil']], ['layout', 'Layout', ['qa']], ['check', 'Check', ['filters', 'theme', 'colors']], ['editing', 'While editing', ['interactions']]];
pbt.toolOrder = pbt.JOBS.flatMap(j => j[2]);
pbt.stripOrder = pbt.toolOrder.filter(n => n !== 'interactions');
pbt.stripTools = () => ({ qa: pbt.qa, filters: pbt.filters, theme: pbt.themecheck, colors: pbt.colors, stencil: pbt.stencil });
const DOCK_NAMES = { qa: 'Layout QA', filters: 'Filter map', theme: 'Theme check', colors: 'Colors', stencil: 'Stencil' };
pbt.DOCK_NAMES = DOCK_NAMES;
pbt.dock = {
  el: null, tabs: null, body: null, bars: {}, shown: '', lastCheck: '',
  ensure() {
    if (this.el && this.el.isConnected) return this.el;
    const el = pbt.el;
    this.tabs = el('div', { class: 'pbt-dock-tabs', role: 'tablist', 'aria-label': 'Tools' });
    this.body = el('div', { class: 'pbt-dock-body' });
    this.el = el('div', { class: 'pbt-dock', 'data-pbt': '', 'data-ae-fp': FINGERPRINT.code }, this.tabs, this.body);
    document.body.append(this.el);
    return this.el;
  },
  on() { const tools = pbt.stripTools(); return pbt.stripOrder.filter(n => tools[n] && tools[n].on && this.bars[n]); },
  // A tool's fresh strip for its slot (the old one goes). The tool appends its bar to its own
  // overlay right after placing it, so the move into the dock waits for that to finish.
  pending: [],
  adopt(name, bar) {
    this.pending.push([name, bar, !this.bars[name] || !this.bars[name].isConnected]);
    if (this.pending.length === 1) queueMicrotask(() => this.flush());
  },
  // Moves the strips placed since the last move into the dock (also callable at once, by tests).
  flush() {
    const todo = this.pending.splice(0);
    for (const [name, bar, fresh] of todo) {
      if (!bar.parentNode) continue;
      this.ensure();
      if (this.bars[name] && this.bars[name] !== bar) this.bars[name].remove();
      this.bars[name] = bar;
      bar.dataset.dockTool = name;
      this.body.append(bar);
      if (fresh) this.shown = name;
    }
    if (todo.length) this.layout();
  },
  pick(name, remember = true) { this.shown = name; if (remember) pbt.ui.set('dock.tab', name); this.layout(); },
  // Focus mode follows the strip shown, so every layout (a tab picked, a tool on or off) applies it.
  layout() { try { this.place(); } finally { if (pbt.checkTools()[this.shown]) this.lastCheck = this.shown; pbt.applyFocus(); } },
  place() {
    if (!this.el) return;
    // Strips of tools turned off go.
    const tools = pbt.stripTools();
    for (const [n, b] of Object.entries(this.bars)) if (!tools[n] || !tools[n].on) { b.remove(); delete this.bars[n]; }
    const on = this.on();
    if (!on.length) { this.el.remove(); this.el = null; return; }
    if (!on.includes(this.shown)) this.shown = on.includes(pbt.ui.get('dock.tab')) ? pbt.ui.get('dock.tab') : on[on.length - 1];
    for (const [n, b] of Object.entries(this.bars)) b.hidden = n !== this.shown;
    // The tab row, only with two or more tools on. A dot marks each tool as on (drawing on the
    // page); the shown tab is underlined, apart from that.
    const el = pbt.el;
    this.tabs.replaceChildren(...(on.length > 1 ? on.map(n => pbt.tip(el('button', { class: `pbt-dock-tab${n === this.shown ? ' pbt-on' : ''}`, role: 'tab', 'aria-selected': String(n === this.shown), dataset: { tool: n },
      onpointerdown: e => { e.stopPropagation(); e.preventDefault(); }, onclick: e => { e.stopPropagation(); this.pick(n); } }, el('span', { class: 'pbt-dock-dot', 'aria-hidden': 'true' }), el('span', { text: DOCK_NAMES[n] || n })),
    `${DOCK_NAMES[n] || n}: on${n === this.shown ? ', strip shown' : ''}\nThe dot means the tool is on and drawing on the page. Click to show its strip here; only one strip shows at a time.`)) : []));
    this.tabs.hidden = on.length < 2;
    // Just below the page when the visible canvas has room, else inside its bottom edge.
    const g = pbt.page.geometry();
    if (!g) return;
    const vpEl = g.da.closest('.displayAreaViewport') || g.da.parentElement;
    const vp = vpEl ? vpEl.getBoundingClientRect() : { left: 0, top: 0, right: innerWidth, bottom: innerHeight };
    const h = this.el.offsetHeight || 34;
    const left = Math.max(g.rect.left, vp.left + 4), right = Math.min(g.rect.left + g.rect.width, vp.right - 4);
    const top = Math.min(g.rect.top + g.rect.height + 4, vp.bottom - h - 4);
    this.el.style.cssText = `left:${left}px;top:${Math.max(vp.top + 4, top)}px;width:${Math.max(200, right - left)}px`;
    // A strip too wide for the dock drops its extras (data-opt: a palette or theme name, the
    // setting most often off, a label), each also in a tooltip, before a count's words get cut.
    const bar = this.bars[this.shown];
    if (bar) {
      bar.classList.remove('pbt-strip-tight');
      const cut = () => bar.scrollWidth > bar.clientWidth + 1 || [...bar.querySelectorAll('.pbt-strip-item > span:last-child')].some(s => s.scrollWidth > s.clientWidth + 1);
      if (bar.querySelector('[data-opt]') && cut()) bar.classList.add('pbt-strip-tight');
    }
  },
};
pbt.placeStrip = (bar, name) => pbt.dock.adopt(name, bar);
// A tool turned on or off: every other tool redraws on its next tick and the dock lays out again.
// )PBT"
LR"PBT(Only the redraw keys are reset (sig, drawKey); never a name a tool uses for a method.
pbt.restack = () => { for (const t of Object.values(pbt.stripTools())) if (t) { t.sig = ''; t.drawKey = ''; } queueMicrotask(() => pbt.dock.layout()); };

// ---- toasts ----
pbt.toast = (text, { tone = 'info', ms = 4000, title } = {}) => {
  let host = document.querySelector('.pbt-toasts');
  if (!host) { host = pbt.el('div', { class: 'pbt-toasts', 'data-pbt': '', 'data-ae-fp': FINGERPRINT.code }); document.body.append(host); }
  const t = pbt.el('div', { class: `pbt-toast pbt-${tone}`, role: 'status' },
    title ? pbt.el('b', { text: title }) : null,
    pbt.el('span', { text }));
  host.append(t);
  requestAnimationFrame(() => t.classList.add('pbt-in'));
  setTimeout(() => { t.classList.remove('pbt-in'); setTimeout(() => t.remove(), 400); }, ms);
  return t;
};

// ---- tooltips: one shared bubble for everything we add. pbt.tip(el, text) marks an element; the
// text's first line is the title, the rest the explanation. Shown 350 ms after the pointer
// arrives or at once on keyboard focus, above the element (below when there is no room). ----
// text is a string (title line, then explanation lines) or a function returning an element (a
// card); a function's element is built fresh each time the tip shows.
pbt.tip = (el, text) => {
  if (typeof text === 'function') { el._pbtTipBuild = text; el.dataset.pbtTip = 'card'; }
  else { delete el._pbtTipBuild; if (text) el.dataset.pbtTip = text; else delete el.dataset.pbtTip; }
  el.removeAttribute('title');
  return el;
};
// The tip's text for tests and copies: a card's text content, else the text itself.
pbt.tipText = el => el._pbtTipBuild ? el._pbtTipBuild().textContent : (el.dataset.pbtTip || '');
// Opening and closing go by where the pointer really is. The tip used to close on the first
// pointerout, pointerover or focusout anywhere in the document, so events that do not mean the
// pointer left closed it: the pointer events Filter map dispatches on visuals while it reads their
// lists, a node rebuilt under a resting pointer, a focus change elsewhere in Power BI, a leave and
// re-enter pair from the host window. The re-entry then started a fresh 350 ms wait, so the tip
// showed, closed and showed again. Now only real (trusted) pointer events count, a close waits
// CLOSE_MS and happens only if the element is no longer under the pointer, focus closes only a tip
// that focus opened, and while one tip is open the next element's tip shows at once (an element
// rebuilt under the pointer gets its tip back without a gap).
// A card tip on an element marked data-pbt-pin (the check tools' badges) pins open on a click: it
// then takes the pointer, so its rows and counters can be used, and it stays until a click
// elsewhere, Esc, or a second click on the badge. The page outlines stay with it (pbt.pinpoint).
const CLOSE_MS = 200;
{
  let box = null, timer = 0, closeTimer = 0, target = null, open = false, byFocus = false, pinned = null;
  pbt.tipPinned = () => !!pinned;
  const hide = () => {
    clearTimeout(timer); clearTimeout(closeTimer); timer = 0; closeTimer = 0;
    const wasPinned = !!pinned;
    if (target) target.classList.remove('pbt-tip-open');
    target = null; open = false; byFocus = false; pinned = null;
    if (box) { box.hidden = true; box.classList.remove('pbt-tip-pinned'); }
    if (wasPinned) { pbt.pinFocus(); pbt.unpinpoint(true); pbt.hideOwner(true); }
  };
  const keep = () => { clearTimeout(closeTimer); closeTimer = 0; };
  const closeSoon = () => {
    if (pinned || closeTimer) return;
    closeTimer = setTimeout(() => {
      closeTimer = 0;
      if (target && target.isConnected && target.matches(':hover')) return;
      hide();
    }, CLOSE_MS);
  };
  const show = el => {
    clearTimeout(timer); timer = 0;
    const text = el.dataset.pbtTip;
    if (!text || !el.isConnected) return;
    if (target && target !== el) target.classList.remove('pbt-tip-open');
    target = el;
    open = true;
    // The element keeps its open look (a Filter map badge its words) while its tip shows.
    el.classList.add('pbt-tip-open');
    if (!box || !box.isConnected) { box = pbt.el('div', { class: 'pbt-tip', 'data-pbt': '', 'data-ae-fp': FINGERPRINT.code, role: 'tooltip' }); document.body.append(box); }
    if (el._pbtTipBuild) { box.replaceChildren(el._pbtTipBuild()); box.classList.add('pbt-tip-rich'); }
    else {
      const [first, ...rest] = text.split('\n');
      box.replaceChildren(pbt.el('b', { text: first }), ...rest.map(line => pbt.el('span', { text: line })));
      box.classList.remove('pbt-tip-rich');
    }
    box.hidden = false;
    const r = el.getBoundingClientRect(), b = box.getBoundingClientRect();
    const left = Math.max(8, Math.min(r.left, innerWidth - b.width - 8));
    let top = r.top - b.height - 9;
    const below = top < 8;
    if (below) top = r.bottom + 9;
    box.classList.toggle('pbt-tip-below', below);
    box.style.left = `${left}px`;
    box.style.top = `${top}px`;
    box.style.setProperty('--pbt-tip-arrow', `${Math.max(10, Math.min(r.left + r.width / 2 - left, b.width - 14))}px`);
  };
  // A pinned card's counters filter its list to their category (click again for all), and a row
  // under the pointer, or clicked to keep it, brings its part on the page forward: a numbered row
  // its own outline and pin (pbt.pinFocus), a row with known parts (row._pbtEls) those parts.
  // Counters carry data-cat, rows the categories they belong to (space separated).
  const wire = () => {
    const card = box.querySelector('.pbt-card');
    if (!card) return;
    const rows = card.querySelector('.pbt-card-rows');
    const kpis = [...card.querySelectorAll('.pbt-card-kpi > [data-cat]')];
    for (const k of kpis) {
      k.classList.add('pbt-kpi-toggle');
      k.setAttribute('role', 'button');
      k.setAttribute('aria-pressed', 'false');
      k.tabIndex = 0;
      const pick = () => {
        const on = k.getAttribut)PBT"
LR"PBT(e('aria-pressed') !== 'true';
        for (const o of kpis) o.setAttribute('aria-pressed', String(on && o === k));
        const cat = on ? k.dataset.cat : '';
        if (rows) for (const r of rows.children) r.hidden = !!cat && !(r.dataset.cat || '').split(' ').includes(cat);
      };
      k.addEventListener('click', e => { e.stopPropagation(); pick(); });
      k.addEventListener('keydown', e => { if (e.key === 'Enter' || e.key === ' ') { e.preventDefault(); e.stopPropagation(); pick(); } });
    }
    if (!rows) return;
    for (const r of rows.querySelectorAll('.pbt-card-row')) {
      const num = r.querySelector(':scope > .pbt-hit-num');
      if (num) r.dataset.pin = num.textContent;
      if (num || (r._pbtEls && r._pbtEls.length)) r.classList.add('pbt-card-row-pin');
    }
    let kept = null;
    const bring = r => {
      if (r && r.dataset.pin) pbt.pinFocus(r.dataset.pin);
      else if (r && r._pbtEls && r._pbtEls.length) pbt.pinFocus(null, r._pbtEls);
      else pbt.pinFocus();
    };
    rows.addEventListener('pointerover', e => { const r = e.target.closest('.pbt-card-row'); bring(r && r.classList.contains('pbt-card-row-pin') ? r : kept); });
    rows.addEventListener('pointerleave', () => bring(kept));
    rows.addEventListener('click', e => {
      const r = e.target.closest('.pbt-card-row-pin');
      if (!r) return;
      e.stopPropagation();
      for (const o of rows.querySelectorAll('.pbt-card-row.pbt-on')) if (o !== r) o.classList.remove('pbt-on');
      kept = r.classList.toggle('pbt-on') ? r : null;
      bring(kept || r);
    });
  };
  pbt.on(document, 'pointerover', e => {
    if (!e.isTrusted || pinned) return;
    const el = e.target.closest && e.target.closest('[data-pbt-tip]');
    if (el && el === target) { keep(); return; }
    if (!el) { if (target) closeSoon(); return; }
    keep();
    if (open) { show(el); return; }
    clearTimeout(timer);
    if (target) target.classList.remove('pbt-tip-open');
    target = el; byFocus = false;
    timer = setTimeout(() => { timer = 0; if (target === el && el.isConnected && el.matches(':hover')) show(el); }, 350);
  }, true);
  pbt.on(document, 'pointerout', e => {
    if (!e.isTrusted || !target || pinned) return;
    if (e.relatedTarget && target.contains(e.relatedTarget)) return;
    closeSoon();
  }, true);
  pbt.on(document, 'focusin', e => {
    if (pinned) return;
    const el = e.target.closest && e.target.closest('[data-pbt-tip]');
    if (!el || el === target) return;
    hide();
    show(el);
    byFocus = true;
  }, true);
  pbt.on(document, 'focusout', e => { if (byFocus && !pinned && target && (e.target === target || target.contains(e.target))) hide(); }, true);
  // A press closes the tip (a pressed button's tip has done its job); a pinned card stays for a
  // press inside it or on its badge (the click that follows unpins it).
  pbt.on(document, 'pointerdown', e => {
    if (!e.isTrusted) return;
    if (pinned && ((box && box.contains(e.target)) || pinned.contains(e.target))) return;
    hide();
  }, true);
  // The badge's own hover handlers, run again on purpose. Power BI's Report view page has no
  // PointerEvent constructor (Model and Table view have one), so a MouseEvent stands in there;
  // without it the click threw before show(el) and the card never pinned.
  const pointerEnter = el => { const E = typeof PointerEvent === 'function' ? PointerEvent : MouseEvent; el.dispatchEvent(new E('pointerenter', { bubbles: false })); };
  pbt.on(document, 'click', e => {
    if (!e.isTrusted || !e.target.closest) return;
    const el = e.target.closest('[data-pbt-pin]');
    // A control inside the badge with a tip of its own (a Colors swatch) keeps its own click.
    if (!el || !el._pbtTipBuild || e.target.closest('[data-pbt-tip]') !== el) return;
    // Unpinned under the pointer: the badge's own hover outlines come back at once.
    if (pinned === el) { hide(); if (el.matches(':hover')) pointerEnter(el); return; }
    if (pinned) hide();
    // Its outlines and its visual's, drawn again in case a pinned card held them back.
    pointerEnter(el);
    show(el);
    if (!open) return;
    pinned = el;
    box.classList.add('pbt-tip-pinned');
    wire();
  }, true);
  pbt.on(document, 'keydown', e => { if (e.key === 'Escape') hide(); }, true);
  pbt.on(window, 'blur', () => hide());
  pbt.hideTip = hide;
  // A pinned card whose badge is gone (its tool cleared the rails, or a redraw replaced the badge)
  // or whose tool is off unpins by itself: its card, outlines and pins go, and the other tips work
  // again. Every pinned badge sits in a rail (pbt.railPut marks it), so this is called when a tool
  // clears or replaces its badges there, and on the 400 ms nudge timer for anything else that
  // removes one.
  pbt.checkPin = () => {
    if (!pinned) return;
    const tool = pinned.dataset.railTool, t = tool ? pbt.checkTools()[tool] : null;
    if (!pinned.isConnected || (tool && !(t && t.on))) hide();
  };
}

// ---- lifecycle ----
pbt.module = m => pbt.modules.push(m);

pbt.remove = () => {
  window.__pbtGen++;
  pbt.modules.slice().reverse().forEach(m => { try { m.stop && m.stop(); } catch {} });
  pbt.cleanups.splice(0).forEach(fn => { try { fn(); } catch {} });
  document.querySelectorAll('[data-pbt]').forEach(n => n.remove());
  document.documentElement.removeAttribute('data-pbt-skin');
  if (window.__pbt === pbt) delete window.__pbt;
  return 'removed';
};

// The start script runs before Power BI has drawn anything. The report view waits for its title
// bar; the other views (DAX query, TMDL) are pages of their own with no title bar, and only need
// a body. Modules skip whatever a page does not have.
pbt.start = () => {
  const isReport = /\/reportView\.html$/.test(location.pathname);
  const boot = () => {
    if (!pbt.current()) return;
    if (!document.body || (isReport && !document.querySelector('titlebar'))) return void setTimeout(boot, 250);
    pbt.style('pbt-base', PBT_BASE_CSS);
    f)PBT"
LR"PBT(or (const m of pbt.modules) { try { m.start && m.start(); } catch (e) { pbt.log(`${m.name} start: ${e.stack}`); } }
    if (isReport) pbt.every(400, () => { pbt.checkPin(); if (document.querySelector('.pbt-nudge, .pbt-rail')) pbt.nudgeBadges(); });
    observer.observe(document.body, { childList: true, subtree: true });
  };
  boot();
  return `pbid-tweaks gen ${gen} (${pbt.modules.map(m => m.name).join(', ')})`;
};

// ---- 05-project.js ----
// The report's saved project files (PBIR), for the tools that compare the page with what the
// report defines: the theme, each page and each visual's own formatting and filters. Both hosts
// hand over the same shape, PBT_PROJECT = { root, files: [{ path, text }] } with paths relative
// to the .Report folder: inject.mjs reads tweaks.json "project" on every reload; the mod finds the
// report Power BI has open (its "project" setting is only a fallback) and hands the files over
// again through set() when another report is opened or this one is saved. Nothing here touches
// the page; everything is pure so scripts/tool-check.mjs can run it on fixtures.

const project = {
  raw: typeof PBT_PROJECT !== 'undefined' ? PBT_PROJECT : null,
  data: null,

  // A PBIR literal ("'Net Revenue'", "11D", "true", "'#FFFFFF'", "12L") as a plain value.
  literal(v) {
    if (typeof v !== 'string') return v;
    if (/^'.*'$/s.test(v)) return v.slice(1, -1).replace(/''/g, "'");
    if (v === 'true' || v === 'false') return v === 'true';
    if (/^-?\d+(\.\d+)?[DLM]?$/.test(v)) return +v.replace(/[DLM]$/, '');
    if (v === 'null') return null;
    return v;
  },

  // One property value from a visual.json (an "expr") or a theme (plain JSON), as something
  // comparable: a literal, a hex color, "theme color 3" or a short description of an expression.
  value(p) {
    if (p == null || typeof p !== 'object') return p;
    if (p.solid && p.solid.color !== undefined) return this.value(p.solid.color);
    if (p.expr) {
      const e = p.expr;
      if (e.Literal) return this.literal(e.Literal.Value);
      if (e.ThemeDataColor) return `theme color ${e.ThemeDataColor.ColorId}${e.ThemeDataColor.Percent ? ` ${e.ThemeDataColor.Percent > 0 ? '+' : ''}${Math.round(e.ThemeDataColor.Percent * 100)}%` : ''}`;
      if (e.Measure || e.Aggregation || e.Conditional || e.FillRule) return 'set by a rule or measure';
      if (e.ResourcePackageItem) return 'an image';
      return 'an expression';
    }
    if (p.color !== undefined) return this.value(p.color);
    return JSON.stringify(p);
  },

  // { title: [{ properties: { fontSize: {...} } }] } (a visual's objects) or
  // { title: [{ fontSize: 12 }] } (a theme's visual style) as { 'title.fontSize': 12 }.
  // Entries with a selector (per series, per category) are kept under 'object[selector].prop'.
  flat(objects, fromTheme = false) {
    const out = {};
    for (const [obj, entries] of Object.entries(objects || {})) {
      for (const entry of [].concat(entries || [])) {
        if (!entry || typeof entry !== 'object') continue;
        const props = fromTheme ? entry : entry.properties || {};
        const sel = entry.selector ? `[${entry.selector.metadata || entry.selector.id || (entry.selector.data ? 'data' : 'selector')}]` : '';
        for (const [k, v] of Object.entries(props)) {
          if (fromTheme && k === '$id') continue;
          out[`${obj}${sel}.${k}`] = this.value(v);
        }
      }
    }
    return out;
  },

  // A field reference ({ Column | Measure | Aggregation | HierarchyLevel }) as "Table[Field]".
  field(f) {
    if (!f) return '';
    const inner = f.Column || f.Measure || (f.Aggregation && (f.Aggregation.Expression.Column || f.Aggregation.Expression.Measure)) || (f.HierarchyLevel && { Expression: f.HierarchyLevel.Expression.Hierarchy.Expression, Property: f.HierarchyLevel.Level });
    if (!inner) return 'a field';
    const src = inner.Expression && inner.Expression.SourceRef;
    return `${src ? `${src.Entity || src.Source}` : ''}[${inner.Property}]`;
  },

  // A filterConfig as [{ name, field, property, kind, hidden, locked, howCreated, applied, text }];
  // text states the filter the way the Filters pane does ("Category is Sports", "Year is 2024 or
  // 2025") for a basic (In / Not In) filter, else names the field and the filter type.
  filters(cfg) {
    return ((cfg && cfg.filters) || []).map(f => {
      const field = this.field(f.field);
      const property = (/\[([^\]]*)\]$/.exec(field) || [])[1] || field;
      return {
        name: f.name, field, property,
        kind: f.type || 'Categorical',
        hidden: !!f.isHiddenInViewMode,
        locked: !!f.isLockedInViewMode,
        howCreated: f.howCreated || '',
        applied: !!f.filter,
        text: f.filter ? this.restate(property, f.filter) : `${property} is (All)`,
      };
    });
  },
  restate(property, filter) {
    const where = (filter.Where || [])[0];
    const cond = where && where.Condition;
    const list = c => c && c.In && c.In.Values ? c.In.Values.map(row => row.map(x => String(this.literal(x.Literal ? x.Literal.Value : ''))).join(', ')) : null;
    let vals = list(cond);
    if (vals) return `${property} is ${vals.join(' or ')}`;
    vals = cond && cond.Not && list(cond.Not.Expression);
    if (vals) return `${property} is not ${vals.join(' or ')}`;
    return `${property} (filtered)`;
  },

  // The raw files as { report, theme, themeName, pages: [{ name, displayName, width, height,
  // filters, visuals: [{ name, type, x, y (page positions; rx, ry as saved), w, h, z, title, objects,
  // containerObjects, filters, parent }] }] }.
  parse(raw) {
    if (!raw || !Array.isArray(raw.files)) return null;
    const json = new Map();
    for (const f of raw.files) { try { json.set(f.path, JSON.parse(f.text)); } catch {} }
    const report = json.get('definition/report.json') || {};
    const tc = report.themeCollection || {};
    // The custom theme over the base theme, when the project carries the base theme's fil)PBT"
LR"PBT(e
    // (StaticResources/SharedResources/BaseThemes); either alone when only one is there.
    const find = name => name && [...json.keys()].find(p => p.startsWith('StaticResources/') && (p.endsWith(`/${name}`) || p.endsWith(`/${name}.json`)));
    const customName = tc.customTheme && tc.customTheme.name;
    const baseName = tc.baseTheme && tc.baseTheme.name;
    const custom = find(customName) ? json.get(find(customName)) : null;
    const base = find(baseName) ? json.get(find(baseName)) : null;
    const theme = custom && base ? this.mergeTheme(base, custom) : custom || base || null;
    const themeName = custom ? customName : base ? baseName : customName || baseName || '';
    const pagesMeta = json.get('definition/pages/pages.json') || {};
    const order = pagesMeta.pageOrder || [];
    const pages = [];
    for (const [path, page] of json) {
      const m = /^definition\/pages\/([^/]+)\/page\.json$/.exec(path);
      if (!m) continue;
      const visuals = [];
      for (const [vp, v] of json) {
        const vm = new RegExp(`^definition/pages/${m[1].replace(/[.*+?^${}()|[\]\\]/g, '\\$&')}/visuals/([^/]+)/visual\\.json$`).exec(vp);
        if (!vm || !v.position) continue;
        const vis = v.visual || {};
        const containerObjects = this.flat(vis.visualContainerObjects);
        visuals.push({
          name: v.name || vm[1],
          type: vis.visualType || (v.visualGroup ? 'group' : ''),
          x: v.position.x, y: v.position.y, w: v.position.width, h: v.position.height, z: v.position.z || 0,
          title: containerObjects['title.text'] != null ? String(containerObjects['title.text']) : '',
          objects: this.flat(vis.objects),
          containerObjects,
          filters: this.filters(v.filterConfig),
          parent: v.parentGroupName || '',
        });
      }
      this.placeGroups(visuals);
      pages.push({ name: page.name || m[1], displayName: page.displayName || m[1], width: page.width, height: page.height, filters: this.filters(page.filterConfig), visuals });
    }
    pages.sort((a, b) => (order.indexOf(a.name) + 1 || 1e6) - (order.indexOf(b.name) + 1 || 1e6));
    return { root: raw.root || '', report, themeName: themeName || '', theme, filters: this.filters(report.filterConfig), pages, activePage: pagesMeta.activePageName || '' };
  },

  // A grouped visual's saved x and y are relative to its group (parentGroupName), and a group's to
  // its own group. Each becomes a page position, with the saved values kept as rx and ry; the
  // matcher, Theme check and Filter map compare page positions.
  placeGroups(visuals) {
    const by = new Map(visuals.map(v => [v.name, v]));
    const at = new Map();
    const abs = (v, depth = 0) => {
      if (at.has(v.name)) return at.get(v.name);
      const p = v.parent && by.get(v.parent);
      const [px, py] = p && p !== v && depth < 16 ? abs(p, depth + 1) : [0, 0];
      const r = [px + v.x, py + v.y];
      at.set(v.name, r);
      return r;
    };
    for (const v of visuals) abs(v);
    for (const v of visuals) { v.rx = v.x; v.ry = v.y; [v.x, v.y] = at.get(v.name); }
    return visuals;
  },

  // A theme over another: objects merge key by key, the visual style arrays entry by entry, and
  // anything else (data colors, a color) is replaced by the custom theme's value.
  mergeTheme(base, over) {
    const isObj = x => x && typeof x === 'object' && !Array.isArray(x);
    const merge = (a, b) => {
      if (Array.isArray(a) && Array.isArray(b) && a.every(isObj) && b.every(isObj)) return b.map((x, i) => (a[i] ? merge(a[i], x) : x)).concat(a.slice(b.length));
      if (!isObj(a) || !isObj(b)) return b === undefined ? a : b;
      const out = { ...a };
      for (const [k, v] of Object.entries(b)) out[k] = k === 'dataColors' ? v : merge(a[k], v);
      return out;
    };
    return merge(base, over);
  },

  // A report opened or saved while the page is open: the mod hands its files over again (null when
  // it has none), and the tools that compare with them draw again on their next pass.
  // The last few hand-overs ({ at, root, files, seen }), for checking which folder the mod read.
  log: [],
  set(raw, info) {
    this.raw = raw && Array.isArray(raw.files) ? raw : null;
    this.data = null;
    this.log = [...this.log.slice(-9), { at: new Date().toISOString(), root: this.raw ? this.raw.root : null, files: this.raw ? this.raw.files.length : 0, seen: info && info.seen || '' }];
    if (pbt.restack) pbt.restack();
    if (pbt.redrawChecks) pbt.redrawChecks();
    // Whatever waits for a save to be read (Stencil's builds) hears it here.
    try { window.dispatchEvent(new CustomEvent('pbt-project')); } catch {}
  },

  get() {
    if (this.data === null && this.raw) this.data = this.parse(this.raw) || false;
    return this.data || null;
  },
  theme() { const d = this.get(); return d && d.theme; },

  // The page open in Power BI, by the display name on its selected page tab.
  page(displayName) {
    const d = this.get();
    if (!d) return null;
    if (displayName == null) {
      const li = document.querySelector('.carouselScrollPane li.section.selected');
      const t = li && (li.querySelector('[class*="title" i], [class*="label" i], span') || li);
      displayName = t ? t.textContent.trim() : '';
    }
    return d.pages.find(p => p.displayName === displayName) || null;
  },

  // The saved visual for one on the page ({ kind, title, x, y, w, h } from pbt.page.visuals()):
  // same type (letters only: "Donut chart" and "donutChart") at the same position; else the same
  // type and title, reported as moved since the last save; else none, with the reason.
  letters: s => String(s || '').toLowerCase().replace(/[^a-z]/g, '').replace(/visual$/, ''),
  // tol: how far (report px) a reading may be from the saved value and still be the same place;
  // below 100% zoom the caller passes about one screen pixel.
  // The page's role description (v.kind) first; where that finds nothing exact, the visual's class)PBT"
LR"PBT(
  // name (v.typeName, "visual-clusteredColumnChart" read as clusteredColumnChart), which names the
  // saved type more often (a shape's role description is "img", a card in a group can read as a
  // container).
  match(v, page = this.page(), tol = 0.5) {
    const m = this.matchKind(v, page, tol);
    if (page && (!m.file || m.reason) && v.typeName && v.typeName !== 'visual' && this.letters(v.typeName) !== this.letters(v.kind)) {
      const k = this.matchKind({ ...v, kind: v.typeName }, page, tol);
      if (k.file && (!m.file || !k.reason)) return k;
    }
    return m;
  },
  matchKind(v, page, tol) {
    if (!page) return { file: null, reason: 'no page of that name in the project files' };
    // "Clustered bar chart" on the page is "barChart" in the file, "Table visual" is "tableEx".
    // The page's name for a type ("Clustered bar chart") against the file's visualType
    // ("clusteredBarChart", or the older "barChart"): exact after letters only, or a known alias.
    // A prefix match is used only when exactly one saved visual on the page fits it, so a line
    // chart is never paired with a line and column combo chart.
    const ALIAS = { tableex: 'table', pivottable: 'matrix', barchart: 'stackedbarchart', columnchart: 'stackedcolumnchart', cardvisual: 'card', multirowcard: 'multirowcard', basicshape: 'shape', actionbutton: 'button', textbox: 'textbox', img: 'shape' };
    const norm = s => ALIAS[s] || s;
    const kind = norm(this.letters(v.kind));
    let typeOk = f => kind && norm(this.letters(f.type)) === kind;
    if (!page.visuals.some(typeOk)) {
      const pre = f => { const t = norm(this.letters(f.type)); return kind && t && (t.startsWith(kind.slice(0, 6)) || kind.startsWith(t.slice(0, 6))); };
      if (page.visuals.filter(pre).length === 1) typeOk = pre;
    }
    const same = f => Math.abs(f.x - v.x) <= tol && Math.abs(f.y - v.y) <= tol && Math.abs(f.w - v.w) <= tol && Math.abs(f.h - v.h) <= tol;
    const typed = page.visuals.filter(typeOk);
    const exact = typed.filter(same);
    if (exact.length === 1) return { file: exact[0], reason: '' };
    if (exact.length > 1) return { file: exact.find(f => f.title === v.title) || exact[0], reason: exact.find(f => f.title === v.title) ? '' : 'two saved visuals of this type sit here' };
    const byTitle = typed.filter(f => f.title && f.title === v.title);
    if (byTitle.length === 1) return { file: byTitle[0], reason: 'moved or resized since the last save' };
    return { file: null, reason: typed.length ? 'moved or resized since the last save' : 'not in the saved project (added since the last save?)' };
  },
};
pbt.project = project;

// ---- 10-skins.js ----
// Skins. Power BI's chrome takes its colors from about 1400 CSS variables that the app
// theme sets on [data-theme="..."] and on .fui-FluentProvider classes. A skin remaps every
// one of those colors through a small palette (skins/<id>.json), then layers its own
// character CSS (skins/<id>.css) on top. Switching Power BI between light and dark simply
// regenerates the remap from the new theme.
//
// Palette fields:
//   ramp    [sourceGrey, skinColor] pairs. A neutral source color is placed on the grey
//           scale (0 = the theme's background end, 1 = its text end) and takes the
//           interpolated skin color, so backgrounds, surfaces, borders and text all move together.
//   accent  replaces the theme's brand hue family, keeping each shade's relative lightness.
//   mono    true for one-color screens: every colored variable takes the accent hue.
//   vars    extra --pbt-* values the skin CSS and our own UI read (font, title bar, etc.).

const color = (() => {
  const parse = s => {
    s = String(s).trim().toLowerCase();
    if (s === 'transparent') return { r: 0, g: 0, b: 0, a: 0 };
    let m = s.match(/^#([0-9a-f]{3,8})$/);
    if (m) {
      let h = m[1];
      if (h.length === 3 || h.length === 4) h = [...h].map(c => c + c).join('');
      if (h.length !== 6 && h.length !== 8) return null;
      return { r: parseInt(h.slice(0, 2), 16), g: parseInt(h.slice(2, 4), 16), b: parseInt(h.slice(4, 6), 16), a: h.length === 8 ? parseInt(h.slice(6, 8), 16) / 255 : 1 };
    }
    m = s.match(/^rgba?\(\s*([\d.]+)[\s,]+([\d.]+)[\s,]+([\d.]+)(?:[\s,/]+([\d.]+%?))?\s*\)$/);
    if (m) return { r: +m[1], g: +m[2], b: +m[3], a: m[4] == null ? 1 : m[4].endsWith('%') ? parseFloat(m[4]) / 100 : +m[4] };
    if (s === 'white') return { r: 255, g: 255, b: 255, a: 1 };
    if (s === 'black') return { r: 0, g: 0, b: 0, a: 1 };
    if (s === 'silver') return { r: 192, g: 192, b: 192, a: 1 };
    return null;
  };
  const hex = ({ r, g, b, a = 1 }) => {
    const h = v => Math.round(Math.max(0, Math.min(255, v))).toString(16).padStart(2, '0');
    return a >= 1 ? `#${h(r)}${h(g)}${h(b)}` : `rgba(${Math.round(r)}, ${Math.round(g)}, ${Math.round(b)}, ${+a.toFixed(3)})`;
  };
  const hsl = ({ r, g, b }) => {
    r /= 255; g /= 255; b /= 255;
    const max = Math.max(r, g, b), min = Math.min(r, g, b), l = (max + min) / 2, d = max - min;
    let h = 0, s = 0;
    if (d) {
      s = d / (1 - Math.abs(2 * l - 1));
      h = max === r ? ((g - b) / d) % 6 : max === g ? (b - r) / d + 2 : (r - g) / d + 4;
      h = (h * 60 + 360) % 360;
    }
    return { h, s, l };
  };
  const fromHsl = ({ h, s, l }, a = 1) => {
    const c = (1 - Math.abs(2 * l - 1)) * s, x = c * (1 - Math.abs((h / 60) % 2 - 1)), m = l - c / 2;
    const [r, g, b] = h < 60 ? [c, x, 0] : h < 120 ? [x, c, 0] : h < 180 ? [0, c, x] : h < 240 ? [0, x, c] : h < 300 ? [x, 0, c] : [c, 0, x];
    return { r: (r + m) * 255, g: (g + m) * 255, b: (b + m) * 255, a };
  };
  const grey = c => (c.r + c.g + c.b) / 765;
  const chroma = c => (Math.max(c.r, c.g, c.b) - Math.min(c.r, c.g, c.b)) / 255;
  const mix = (a, b, t) => ({ r: a.r + (b.r - a.r) * t, g: a.g + (b.g - a.g) * t, b: a.b + (b.b - a.b) * t, a: 1 });
  // WCAG 2.1 relative luminance and contrast ratio (opaque colors).
  const lum = c => { const f = v => { v /)PBT"
LR"PBT(= 255; return v <= 0.03928 ? v / 12.92 : ((v + 0.055) / 1.055) ** 2.4; }; return 0.2126 * f(c.r) + 0.7152 * f(c.g) + 0.0722 * f(c.b); };
  const contrast = (a, b) => { const x = lum(a), y = lum(b); return (Math.max(x, y) + 0.05) / (Math.min(x, y) + 0.05); };
  return { parse, hex, hsl, fromHsl, grey, chroma, mix, lum, contrast };
})();
pbt.color = color;

// The theme's variables, as the browser would resolve them for <html>, read from one reference
// theme whatever mode Power BI is in. Every [data-theme="..."] block is in the page's style
// sheets at all times, so the dark block (the fullest, and the one the skins were drawn against)
// is read even while light mode is on: the same token then maps to the same skin color in both
// modes. Power BI's light and dark palettes are not mirror images (its dark background is not
// its darkest grey), so reading each mode's own block put inputs, cards and selection marks on
// different ramp spots per mode. The current mode's block is the fallback when no dark block exists.
function themeVariables() {
  const current = document.documentElement.getAttribute('data-theme') || '';
  const base = new Map(), byTheme = new Map(), fui = new Map();
  const take = (rule, map) => {
    for (let i = 0; i < rule.style.length; i++) {
      const p = rule.style[i];
      if (p.startsWith('--')) map.set(p, rule.style.getPropertyValue(p).trim());
    }
  };
  for (const sheet of document.styleSheets) {
    if (sheet.ownerNode && sheet.ownerNode.dataset && 'pbt' in sheet.ownerNode.dataset) continue;
    let list; try { list = sheet.cssRules; } catch { continue; }
    const walk = l => {
      for (const r of l) {
        if (r.selectorText && r.style) {
          const parts = r.selectorText.split(',').map(s => s.trim());
          const themed = parts.map(p => (p.match(/^\[data-theme="([^"]+)"\]$/) || [])[1]).filter(Boolean);
          if (themed.length) { for (const t of themed) { if (!byTheme.has(t)) byTheme.set(t, new Map()); take(r, byTheme.get(t)); } }
          else if (parts.includes(':root')) take(r, base);
          else if (parts.every(p => /^\.fui-FluentProvider[\w-]*$/.test(p))) take(r, fui);
        }
        if (r.cssRules && r.cssRules.length) walk(r.cssRules);
      }
    };
    walk(list);
  }
  const theme = byTheme.has('dark') ? 'dark' : current;
  const allCurrent = new Map([...base, ...(byTheme.get(current) || new Map())]);
  // Two kinds of token. "ref": the reference block gives the name a color, the same in both
  // modes. "own": everything else (the :root defaults, the rules Fluent inserts at run time for
  // the current mode, provider tokens the reference only aliases), which are current-mode
  // colors and are remapped with the current mode's polarity; they behave as before.
  const concrete = v => v && !v.startsWith('var(');
  const refBlock = byTheme.get(theme) || new Map();
  const ref = new Map(), own = new Map(), fuiRef = new Map(), fuiOwn = new Map();
  for (const [k, v] of allCurrent) { const r = refBlock.get(k); if (concrete(r)) ref.set(k, r); else own.set(k, v); }
  for (const [k, v] of fui) { const r = refBlock.get(k); if (concrete(r)) fuiRef.set(k, r); else fuiOwn.set(k, v); }
  // What the remappers resolve names against: the reference view and the current-mode view.
  const all = new Map([...allCurrent, ...ref]);
  return { theme, current, all, allCurrent, ref, own, fui: fuiRef, fuiOwn, fuiAll: fui };
}

// Builds the remap function for one skin against the reference theme. srcDarkOverride names the
// source's polarity when it is not the theme's (the Monaco editor follows the mode, not the theme).
function remapper(skin, vars, srcDarkOverride) {
  const resolve = name => {
    let v = vars.all.get(name) || vars.fui.get(name);
    for (let i = 0; i < 5 && v && v.startsWith('var('); i++) v = vars.all.get(v.slice(4).split(/[,)]/)[0].trim());
    return v && color.parse(v);
  };
  const bg = resolve('--colorNeutralBackground1') || { r: 41, g: 41, b: 41 };
  const fg = resolve('--colorNeutralForeground1') || { r: 255, g: 255, b: 255 };
  const srcDark = srcDarkOverride === undefined ? color.grey(bg) < color.grey(fg) : srcDarkOverride;
  const brand = resolve('--colorBrandBackground') || { r: 12, g: 105, b: 90 };
  const brandHsl = color.hsl(brand);
  const ramp = skin.palette.ramp.map(([s, d]) => [color.grey(color.parse(s)), color.parse(d)]).sort((a, b) => a[0] - b[0]);
  const skinDark = color.grey(ramp[0][1]) < color.grey(ramp[ramp.length - 1][1]);
  const accent = color.hsl(color.parse(skin.palette.accent));
  const onRamp = t => {
    if (t <= ramp[0][0]) return ramp[0][1];
    for (let i = 1; i < ramp.length; i++) {
      if (t <= ramp[i][0]) return color.mix(ramp[i - 1][1], ramp[i][1], (t - ramp[i - 1][0]) / (ramp[i][0] - ramp[i - 1][0]));
    }
    return ramp[ramp.length - 1][1];
  };
  const map = value => {
    const c = color.parse(value);
    if (!c || c.a === 0) return null;
    if (color.chroma(c) < 0.12) {
      // Neutral: position on the theme's grey scale, background end first.
      const t = srcDark ? color.grey(c) : 1 - color.grey(c);
      return color.hex({ ...onRamp(t), a: c.a });
    }
    const h = color.hsl(c);
    // One-color screens (palette.mono): every colored variable takes the accent hue, keeping its lightness.
    if (skin.palette.mono) return color.hex(color.fromHsl({ h: accent.h, s: accent.s * 0.85, l: Math.max(0.12, Math.min(0.9, srcDark === skinDark ? h.l : 1 - h.l)) }, c.a));
    const dh = Math.min(Math.abs(h.h - brandHsl.h), 360 - Math.abs(h.h - brandHsl.h));
    if (dh <= 40) {
      // Brand family: the skin accent at the same lightness offset from the brand color.
      const l = Math.max(0.04, Math.min(0.96, accent.l + (h.l - brandHsl.l)));
      return color.hex(color.fromHsl({ h: accent.h, s: accent.s, l }, c.a));
    }
    // Status and category colors keep their hue; flip lightness when the skin flips polarity.
    if (srcDark !== skinDark) return color.hex(color.fromHsl({)PBT"
LR"PBT( ...h, l: Math.max(0.2, Math.min(0.8, 1 - h.l)) }, c.a));
    return null;
  };
  map.flips = srcDark !== skinDark; // a dark source under a light skin, or the reverse
  map.srcDark = srcDark;
  return map;
}

function skinCss(id, skin) {
  const vars = themeVariables();
  const map = remapper(skin, vars);
  // Tokens the reference theme does not color are current-mode values: their own remap.
  const mapMode = vars.theme === vars.current ? map : remapper(skin, { all: vars.allCurrent, fui: vars.fuiAll });
  const decl = (m, mapper = map) => {
    const out = [];
    for (const [k, v] of m) {
      if (v.startsWith('var(')) continue;
      const mapped = mapper(v);
      if (mapped) out.push(`${k}: ${mapped};`);
    }
    return out.join(' ');
  };
  const own = Object.entries(skin.palette.vars || {}).map(([k, v]) => `--pbt-${k}: ${v};`).join(' ');
  const sel = `html[data-pbt-skin="${id}"]`;
  // Tokens that must not follow the remap. Disabled text is a mid-grey in both of Power BI's
  // modes, and a mid-grey lands on a different spot of the ramp under each mode (too dark on a
  // dark skin in light mode); it is pinned halfway between the skin's ground and text instead.
  // Text on the brand color is white in both modes, which a light accent cannot carry.
  const ramp = skin.palette.ramp.map(([s, d]) => [color.grey(color.parse(s)), color.parse(d)]).sort((a, b) => a[0] - b[0]);
  const ground = ramp[0][1], text = ramp[ramp.length - 1][1];
  const disabled = color.hex(color.mix(text, ground, 0.45)), faint = color.hex(color.mix(text, ground, 0.7));
  const onBrand = color.hsl(color.parse(skin.palette.accent)).l > 0.6 ? '#111111' : '#ffffff';
  const pinned = `--colorNeutralForegroundDisabled: ${disabled}; --colorNeutralForegroundDisabled2: ${disabled}; --colorNeutralForeground4: ${disabled}; --colorNeutralStrokeDisabled: ${faint}; --colorNeutralForegroundOnBrand: ${onBrand}; --colorBrandForegroundOnBrand: ${onBrand};`;
  // The editor follows the mode (vs-dark or vs), not the reference theme, so its tokens get a
  // remap of their own polarity when the two differ.
  const editor = document.querySelector('.monaco-editor');
  const editorDark = editor ? editor.classList.contains('vs-dark') : map.srcDark;
  const mapEditor = editorDark === map.srcDark ? map : remapper(skin, vars, editorDark);
  return [
    `/* generated from data-theme="${vars.theme}" (mode ${vars.current}): ${vars.ref.size} reference + ${vars.own.size} mode theme variables, ${vars.fui.size} + ${vars.fuiOwn.size} Fluent */`,
    `${sel}[data-theme] { ${decl(vars.ref)} ${decl(vars.own, mapMode)} --pbt-accent: ${skin.palette.accent}; ${own} ${pinned} }`,
    `${sel} .fui-FluentProvider { ${decl(vars.fui)} ${decl(vars.fuiOwn, mapMode)} }`,
    officeIconCss(sel, skin, vars),
    monacoCss(sel, mapEditor),
    skin.css,
  ].join('\n');
}

// Office icons' colored classes (the Power Apps purple, the Excel green, the orange and yellow
// accents) are fixed per mode too: Power BI's dark mode uses lighter versions, so a dark skin
// over light mode got the dark-for-white purple at 2:1. Every colored class is read from the
// mode-independent defaults (the plain .OfficeIconColors_mNNN rules) and goes through the remap
// as a status color from a light source: a light skin keeps it, a dark skin gets its lightness
// flipped, the same in both modes.
function officeIconCss(sel, skin, vars) {
  const fromLight = remapper(skin, vars, false);
  const out = [];
  for (const sheet of document.styleSheets) {
    if (sheet.ownerNode && sheet.ownerNode.dataset && 'pbt' in sheet.ownerNode.dataset) continue;
    let list; try { list = sheet.cssRules; } catch { continue; }
    for (const r of list) {
      const m = r.selectorText && r.selectorText.match(/^\.(OfficeIconColors_m(?:2\d\d|3\d))$/);
      if (!m || !r.style.fill) continue;
      const c = color.parse(r.style.fill);
      if (!c || color.chroma(c) < 0.12) continue;
      // A light skin keeps the default itself (pinned, so dark mode's lighter version never shows).
      const mapped = fromLight(r.style.fill) || r.style.fill;
      out.push(`${sel} :is(.ribbonContainer, div.ribbonSearchOuter, .sidePane, .outspacePane, section.statusBar) svg .${m[1]} { fill: ${mapped} !important; }`);
    }
  }
  return out.join('\n');
}

// The DAX query and TMDL views hold a Monaco editor, which is themed by Power BI's editor theme
// (vs-dark or vs by mode), not by the variables: fixed backgrounds and one .mtkN rule per token
// color. The surfaces take the skin's ground and the token colors go through the same remap as
// the theme (neutrals onto the ramp; colored tokens keep their hue and flip lightness when the
// skin's polarity differs from the mode's), so code reads the same over either mode.
function monacoCss(sel, map) {
  const tokens = [];
  for (const sheet of document.styleSheets) {
    if (sheet.ownerNode && sheet.ownerNode.dataset && 'pbt' in sheet.ownerNode.dataset) continue;
    let list; try { list = sheet.cssRules; } catch { continue; }
    for (const r of list) {
      if (!r.selectorText || !/^\.mtk\d+$/.test(r.selectorText) || !r.style.color) continue;
      const mapped = map(r.style.color);
      if (mapped) tokens.push(`${sel} .monaco-editor ${r.selectorText} { color: ${mapped} !important; }`);
    }
  }
  return [
    ...tokens,
    `${sel} :is(.monaco-editor, .monaco-editor .margin, .monaco-editor .monaco-editor-background, .monaco-editor .minimap, .monaco-editor .lines-content, .monaco-editor .overflow-guard) { background-color: var(--colorNeutralBackground1) !important; }`,
    `${sel} .monaco-editor .line-numbers { color: var(--colorNeutralForegroundDisabled) !important; }`,
    `${sel} .monaco-editor .current-line { border-color: var(--pbt-line-soft) !important; }`,
    `${sel} .monaco-editor .selected-text { background-color: color-mix(in srgb, var(--pbt-accent) 35%, transparent) !important; }`,
    `${sel} .monaco-editor .cursor { background-color: var(--colorNeutral)PBT"
LR"PBT(Foreground1) !important; }`,
    `${sel} .monaco-editor .minimap-shadow-visible { box-shadow: none !important; }`,
    // The minimap is a canvas painted in the editor theme's colors; when the skin's polarity
    // differs from the mode's, invert it so it reads like the remapped code beside it.
    map.flips ? `${sel} .monaco-editor .minimap canvas { filter: invert(1) hue-rotate(180deg); }` : '',
  ].join('\n');
}

const skins = {
  name: 'skins',
  theme: null,
  ids() {
    return Object.keys(pbt.skins).sort((a, b) => (pbt.skins[a].order || 99) - (pbt.skins[b].order || 99));
  },
  apply(id, { announce = false, relayout = true } = {}) {
    const skin = pbt.skins[id];
    const html = document.documentElement;
    html.classList.toggle('pbt-report-filters', pbt.feature('reportFilterPane'));
    this.current = id; // what is drawn now; the smoke test applies skins without saving them
    if (!skin) {
      html.removeAttribute('data-pbt-skin');
      pbt.style('pbt-skin', '');
      this.theme = null;
    } else {
      this.theme = html.getAttribute('data-theme');
      pbt.style('pbt-skin', skinCss(id, skin));
      html.setAttribute('data-pbt-skin', id);
      if (announce) pbt.modules.forEach(m => m.onSkin && m.onSkin(id, skin));
    }
    // Whatever shows the skin's name or marks the chosen one (chip, menu, Skins group) follows
    // what is drawn, so a skin applied for a test shows as itself.
    pbt.modules.forEach(m => m.onApplied && m.onApplied(id));
    if (!relayout) return;
    // Fonts and paddings just changed. A resize lets Power BI re-measure its ribbon groups, but
    // not its tab underline (placed from tab widths measured when the tab was opened), so the
    // underline is moved back under the open tab directly. Skins hide it; this matters for None.
    // Timers rather than animation frames, which a covered window does not get. A copy stopped in
    // the meantime leaves the rest to its stop().
    setTimeout(() => { if (pbt.current()) window.dispatchEvent(new Event('resize')); }, 30);
    for (const ms of [60, 400]) setTimeout(() => { if (pbt.current()) this.alignUnderline(); }, ms);
  },
  alignUnderline() {
    const bar = document.querySelector('#selectedPivotIndicator');
    const open = document.querySelector('#tablist button[role="tab"][aria-selected="true"]');
    if (!bar || !open) return;
    const b = bar.getBoundingClientRect(), o = open.getBoundingClientRect();
    if (!b.width) return;
    const shift = (o.left + (o.width - b.width) / 2) - b.left;
    if (Math.abs(shift) <= 0.5) return;
    if (this.barLeft === undefined) this.barLeft = bar.style.left;  // Power BI's own, for stop()
    bar.style.left = `${(parseFloat(getComputedStyle(bar).left) || 0) + shift}px`;
  },
  onConfig() { document.documentElement.classList.toggle('pbt-report-filters', pbt.feature('reportFilterPane')); },
  set(id) {
    pbt.cfg.skin = id;
    pbt.save();
    this.apply(id, { announce: true });
    pbt.modules.forEach(m => m.onConfig && m.onConfig());
  },
  cycle(step = 1) {
    const ids = ['off', ...this.ids()];
    const i = Math.max(0, ids.indexOf(pbt.cfg.skin));
    this.set(ids[(i + step + ids.length) % ids.length]);
  },
  start() {
    this.apply(pbt.cfg.skin);
    // Power BI switched between light and dark: rebuild the remap from the new theme.
    const mo = new MutationObserver(() => {
      if (pbt.skins[this.current] && document.documentElement.getAttribute('data-theme') !== this.theme) this.apply(this.current);
    });
    mo.observe(document.documentElement, { attributes: true, attributeFilter: ['data-theme'] });
    pbt.cleanups.push(() => mo.disconnect());
    // Power BI keeps adding theme rules after the page starts: Fluent's style engine inserts its
    // color tokens only when its controls first render, through insertRule, which no DOM
    // observer sees. A skin generated at startup missed them (the ribbon kept Power BI's colors
    // until the skin was picked again), so the skin is regenerated whenever Power BI's style
    // sheets grow. Our own sheets are left out of the count, or regenerating would trigger itself.
    const sheetsSize = () => {
      let n = 0;
      for (const s of document.styleSheets) {
        if (s.ownerNode && s.ownerNode.dataset && 'pbt' in s.ownerNode.dataset) continue;
        try { n += s.cssRules.length; } catch {}
      }
      return n;
    };
    let size = sheetsSize();
    pbt.every(1000, () => {
      const now = sheetsSize();
      if (now === size) return;
      size = now;
      if (pbt.skins[this.current]) this.apply(this.current, { relayout: false });
    });
    pbt.hotkey(']', 'Next skin', () => this.cycle(1));
    pbt.hotkey('[', 'Previous skin', () => this.cycle(-1));
  },
  // Teardown (pbt.remove): the class apply() and onConfig() set on <html> goes too; the skin's
  // style sheet and data-pbt-skin are taken away by pbt.remove itself, after this.
  // Then Power BI re-measures its ribbon for its own fonts, and the tab underline goes back: with no
  // inline position of Power BI's own it is simply cleared; else it is moved under the open tab
  // once the sheet is gone, because the value from before may name another tab by now. A newer
  // copy (a reload) that has put a skin on again is left alone.
  stop() {
    const html = document.documentElement;
    html.classList.remove('pbt-report-filters');
    const skinned = html.hasAttribute('data-pbt-skin');
    const moved = this.barLeft !== undefined, own = this.barLeft;
    this.barLeft = undefined;
    const free = () => !html.hasAttribute('data-pbt-skin');
    if (skinned) setTimeout(() => { if (free()) window.dispatchEvent(new Event('resize')); }, 30);
    const bar = document.querySelector('#selectedPivotIndicator');
    if (moved && own === '' && bar) bar.style.left = '';
    else if (moved || skinned) for (const ms of [60, 400]) setTimeout(() => { if (free()) this.alignUnderline(); }, ms);
  },
};
pbt.skin = skins;
pbt.module(skins);

// )PBT"
LR"PBT(---- 20-menu.js ----
// Status bar chip ("Starship Panel \u00b7 9 visuals") and the menu it opens: the tools grouped by job,
// then skins as tiles (folded once one is picked), then the extras as switches with a one-line
// description each, then Help and keys.
// Ctrl+Alt+K opens the same menu from anywhere. Tools and extras switch in place; picking a
// skin closes the menu (its splash follows).

// The extras, in menu order: label, one-line description, and for some a Try button.
const FEATURES = {
  splash: { label: 'Splash when a skin changes', desc: 'A card with the skin\'s name over the window, for two seconds.' },
  notes: { label: 'Comments', desc: 'A short note when a visual shows an error, a refresh finishes, or a tool reports.' },
  confetti: { label: 'Refresh confetti', desc: 'Confetti when a refresh finishes, in the skin\'s shapes and colors. The finish is read from the page re-rendering after Refresh. With a cheer sound set, the Cheer pill plays it with the confetti.', toggle: 'refreshCheer', tryLabel: 'Try', helpTry: 'Try confetti', tryTip: 'Throw confetti now, with the cheer when that switch is on\nCtrl+Alt+C does the same.', run: () => { if (pbt.fun) { pbt.fun.confetti(); if (pbt.feature('refreshCheer')) pbt.fun.cheer(true); } } },
  // A sound with the confetti: the file built into the mod or named in its settings (or in
  // tweaks.json "cheer"), embedded as PBT_CHEER. The switch shows only when a file is there;
  // Confetti's Try plays both when it is on.
  // Shown as the Cheer pill on the confetti row (toggle), not as a row of its own.
  refreshCheer: { label: 'Cheer', inline: true, sound: true, pillTip: 'Play a cheer with the confetti', desc: 'The cheer plays with the confetti. Confetti alone when this is off.', available: () => typeof PBT_CHEER !== 'undefined' && !!PBT_CHEER },
  sounds: { label: 'Skin sounds', desc: 'A few synthesized notes when a skin is picked or a refresh finishes, and the sounds of the hidden surprises. No audio files.', tryLabel: 'Try', helpTry: 'Play this skin\'s notes', tryTip: 'Play this skin\'s notes now', run: () => pbt.fun && pbt.fun.play(pbt.skins[pbt.skin.current] || pbt.skins[pbt.cfg.skin], true) },
  visualCount: { label: 'Visual count in the status bar', desc: 'The chip shows how many visuals the open page has; more than 20 turns it amber.' },
  reportFilterPane: { label: "Keep the report's Filters pane colors", desc: "Show the report's own filter pane formatting while you design it, instead of the skin's paint." },
};
pbt.FEATURES = FEATURES;
pbt.featureList = () => Object.entries(FEATURES).filter(([, def]) => !def.inline && (!def.available || def.available()));

// Names for the menu, the chip and the ribbon group.
const skinLabel = id => (pbt.skins[id] ? pbt.skins[id].name : '');
// Three-band swatch: the skin's surface, its accent, its text.
const swatch = s => {
  const r = s.palette.ramp;
  return `linear-gradient(90deg, ${r[1][1]} 0 40%, ${s.palette.accent} 40% 70%, ${r[r.length - 1][1]} 70%)`;
};
const NONE_SWATCH = 'repeating-linear-gradient(45deg, #888 0 3px, transparent 3px 6px)';
pbt.skinLabel = skinLabel;
pbt.swatch = swatch;
pbt.skinTip = id => {
  if (!pbt.skins[id]) return 'None\nPower BI exactly as it is set, light or dark.';
  const s = pbt.skins[id];
  return `${s.name}${s.inspiredBy ? `\nInspired by ${s.inspiredBy}.` : ''}\nCtrl+Alt+[ / ] switch skins.`;
};

const menu = {
  name: 'menu',
  chip: null,
  open: null,
  // The skin drawn right now (a test may apply one without saving it), else the saved one.
  shownSkin() {
    // Classic in its 95 style draws the Power BI 95 look in place of the skin; it still shows as Classic.
    if (pbt.retro95 && pbt.retro95.classicOn && pbt.retro95.classicOn()) return 'classic';
    return pbt.skin && pbt.skin.current !== undefined ? pbt.skin.current : pbt.cfg.skin;
  },
  build() {
    const bar = document.querySelector('section.statusBar .leftPane');
    if (!bar || !pbt.feature('statusChip')) return;
    if (this.chip && this.chip.isConnected) return this.refresh();
    // A chip left behind by an earlier copy of the script never stays beside ours.
    document.querySelectorAll('.pbt-chip').forEach(n => n.remove());
    this.chip = pbt.el('button', { class: 'pbt-chip', 'data-pbt': '', 'aria-haspopup': 'menu', onclick: e => { e.stopPropagation(); this.toggle(this.chip); } },
      pbt.el('span', { class: 'pbt-dot' }), pbt.el('span', { class: 'pbt-skin-name' }), pbt.el('span', { class: 'pbt-count' }), pbt.el('span', { class: 'pbt-caret', 'aria-hidden': 'true' }));
    bar.append(this.chip);
    this.refresh();
    this.hint();
  },
  // A one-time pill beside the chip naming the menu's key: gone for good once it is closed, the
  // menu opens, or 20 seconds pass (remembered per viewer).
  hint() {
    clearTimeout(this.hintTimer);
    document.querySelectorAll('.pbt-hint').forEach(n => n.remove());
    if (!this.chip || !this.chip.isConnected || pbt.ui.get('hint.menu')) return null;
    const done = () => { clearTimeout(this.hintTimer); pbt.ui.set('hint.menu', true); h.remove(); };
    const h = pbt.el('span', { class: 'pbt-hint', 'data-pbt': '', role: 'note' },
      pbt.el('span', { text: 'Ctrl+Alt+K: tools and skins' }),
      pbt.tip(pbt.el('button', { class: 'pbt-hint-x', 'aria-label': 'Dismiss', text: '\u00d7', onclick: e => { e.stopPropagation(); done(); } }), 'Dismiss\nThis hint does not come back.'));
    this.hintTimer = setTimeout(done, 20000);
    this.hintDone = done;
    this.chip.after(h);
    return h;
  },
  refresh() {
    if (!this.chip) return;
    const name = skinLabel(this.shownSkin()) || 'No skin';
    this.chip.querySelector('.pbt-skin-name').textContent = name;
    const count = this.chip.querySelector('.pbt-count');
    const n = document.querySelectorAll('.displayArea .visualContainer').length;
    count.hidden = !pbt.feature('visualCount');
    // The "\u00b7" before the count is drawn by the stylesheet, so it goes away with the count.
    co)PBT"
LR"PBT(unt.textContent = `${n} visual${n === 1 ? '' : 's'}`;
    // More than about 20 visuals on a page is where render time usually starts to hurt.
    const many = n > 20;
    count.classList.toggle('pbt-warn', many);
    pbt.tip(this.chip, `Skins and tools\n${name} \u00b7 ${n} visual${n === 1 ? '' : 's'} on this page${many ? ' (more than 20: consider fewer, or combine cards)' : ''}\nClick or press Ctrl+Alt+K for the menu.`);
    this.sync();
  },
  // Switch states follow the config without rebuilding the open menu.
  sync() {
    if (!this.open) return;
    this.open.querySelectorAll('[data-pbt-state]').forEach(b => { b.setAttribute('aria-checked', String(!!b._pbtState())); });
  },
  close() { if (this.open) { this.open.remove(); this.open = null; } },
  toggle(anchor) {
    if (this.open) return this.close();
    pbt.hideTip && pbt.hideTip();
    if (document.querySelector('.pbt-hint') && this.hintDone) this.hintDone();
    // Descriptions live in the tooltips (hover or focus an item), so the list stays short.
    const m = pbt.el('div', { class: 'pbt-menu', 'data-pbt': '', 'data-ae-fp': pbt.fingerprint.code, role: 'menu', 'aria-label': 'Skins and tools' });
    // A section: a heading that folds its body (remembered per viewer), a key hint, a blurb in
    // the heading's tooltip.
    const section = (id, title, key, blurb, ...body) => {
      // Skins fold by themselves once a skin has been picked (unfolding is remembered as well).
      const folded = !!pbt.ui.get(`menu.${id}`, id === 'skins' && !!pbt.skins[this.shownSkin()]);
      const s = pbt.el('div', { class: `pbt-section${folded ? ' pbt-folded' : ''}`, dataset: { section: id } });
      const btn = pbt.tip(pbt.el('button', { class: 'pbt-sect', 'aria-expanded': String(!folded), onclick: e => {
        e.stopPropagation();
        const now = s.classList.toggle('pbt-folded');
        btn.setAttribute('aria-expanded', String(!now));
        pbt.ui.set(`menu.${id}`, now);
      } }, pbt.el('span', { class: 'pbt-chev', 'aria-hidden': 'true', text: '\u25be' }), pbt.el('span', { text: title })), `${title}\n${blurb ? `${blurb}\n` : ''}Click to fold or unfold this section; the menu remembers it.`);
      s.append(pbt.el('h4', {}, btn, key ? pbt.el('span', { class: 'pbt-key', text: key }) : null), pbt.el('div', { class: 'pbt-body' }, ...body));
      return s;
    };
    // A tool or an extra: switch, name, key or Try button; the description is its tooltip.
    // A pill on the row: a button inside the row's button, so it keeps the click to itself.
    // sound: a small speaker before the words, so the pill reads as a sound switch (the Cheer).
    const pill = (text, act, tipText, pressed, sound) => {
      const p = pbt.tip(pbt.el('span', { class: 'pbt-try', role: 'button', tabindex: '0', text, onclick: e => { e.stopPropagation(); act(p); }, onkeydown: e => { if (e.key === 'Enter' || e.key === ' ') { e.preventDefault(); e.stopPropagation(); act(p); } } }), tipText);
      if (pressed != null) p.setAttribute('aria-pressed', String(pressed));
      if (sound) {
        const ns = 'http://www.w3.org/2000/svg', svg = document.createElementNS(ns, 'svg'), path = document.createElementNS(ns, 'path');
        svg.setAttribute('viewBox', '0 0 16 16');
        svg.setAttribute('class', 'pbt-pill-icon');
        svg.setAttribute('aria-hidden', 'true');
        path.setAttribute('d', 'M2 6h3l4-3v10l-4-3H2z M11 5.5c1.2 1.3 1.2 3.7 0 5 M13 3.5c2.3 2.4 2.3 6.6 0 9');
        svg.append(path);
        p.prepend(svg);
      }
      return p;
    };
    const item = ({ label, desc, key, state, onclick, tip, tryLabel, tryTip, run, sub, toggle, note }) => {
      // toggle: a feature shown as an on/off pill on this row (the confetti's Cheer).
      const extra = toggle && FEATURES[toggle] && (!FEATURES[toggle].available || FEATURES[toggle].available()) ? FEATURES[toggle] : null;
      const pills = [
        extra ? pill(extra.label, p => { pbt.setFeature(toggle, !pbt.feature(toggle)); p.setAttribute('aria-pressed', String(pbt.feature(toggle))); }, `${extra.pillTip || extra.label}\n${extra.desc} Optional; click to switch it on or off.`, pbt.feature(toggle), extra.sound) : null,
        run ? pill(tryLabel || 'Try', () => run(), tryTip) : null,
      ].filter(Boolean);
      const b = pbt.el('button', { class: `pbt-item${sub ? ' pbt-sub' : ''}`, role: 'menuitemcheckbox', 'aria-checked': String(!!state()), dataset: { pbtState: '' }, onclick: e => { e.stopPropagation(); onclick(); this.sync(); } },
        pbt.el('span', { class: 'pbt-switch', 'aria-hidden': 'true' }),
        pbt.el('span', { class: 'pbt-name', text: label }),
        key ? pbt.el('span', { class: 'pbt-key', text: key }) : null,
        pills.length ? pbt.el('span', { class: 'pbt-pills' }, ...pills) : null,
        note ? pbt.el('span', { class: 'pbt-desc', text: note }) : null);
      b._pbtState = state;
      return pbt.tip(b, `${label}\n${desc || ''}${tip ? `\n${tip.split('\n').slice(1).join('\n')}` : ''}`.replace(/\n+$/, ''));
    };

    // Tools first, grouped by job (pbt.JOBS: the order the dock, the help and the docs use too).
    const tools = pbt.hotkeys.filter(h => h.tool);
    const toolItem = hk => item({ label: hk.label, desc: hk.desc, note: hk.note, key: `Ctrl+Alt+${hk.key.toUpperCase()}`, state: () => !!(hk.active && hk.active()), onclick: () => hk.fn(), tip: hk.tip });
    const grouped = pbt.JOBS.flatMap(([job, title, ids]) => {
      const list = ids.map(id => tools.find(h => h.id === id)).filter(Boolean);
      return list.length ? [pbt.el('div', { class: 'pbt-group', dataset: { job }, text: title }), ...list.map(toolItem)] : [];
    });
    const rest = tools.filter(h => !pbt.toolOrder.includes(h.id)).map(toolItem);
    m.append(section('tools', 'Tools', null, null, ...grouped, ...rest), pbt.el('hr'));

    const shown = this.shownSkin();
    const tiles = pbt.el('div', { class: 'pbt-skins' });
    const tile = (id, label, sw) => pbt.tip(pbt.el('button', { role: 'menuit)PBT"
LR"PBT(emradio', 'aria-checked': String(shown === id), onclick: e => { e.stopPropagation(); this.close(); if (id !== 'off') pbt.ui.set('menu.skins', true); pbt.skin.set(id); } },
      pbt.el('span', { class: 'pbt-sw', style: `background:${sw}` }), pbt.el('span', { class: 'pbt-name', text: label })), pbt.skinTip(id));
    tiles.append(tile('off', 'None', NONE_SWATCH));
    for (const id of pbt.skin.ids()) tiles.append(tile(id, pbt.skins[id].short || pbt.skins[id].name, swatch(pbt.skins[id])));
    // Folded, the heading still names the skin drawn.
    m.append(section('skins', skinLabel(shown) ? `Skins: ${skinLabel(shown)}` : 'Skins', 'Ctrl+Alt+[ / ] to switch', 'Restyle the window around your report. The report page itself is never changed.', tiles), pbt.el('hr'));
    m.append(section('extras', 'Extras', null, null, ...pbt.featureList().map(([f, def]) =>
      item({ ...def, state: () => pbt.feature(f), onclick: () => pbt.setFeature(f, !pbt.feature(f)) }))), pbt.el('hr'));
    m.append(pbt.tip(pbt.el('button', { role: 'menuitem', class: 'pbt-help-item', onclick: e => { e.stopPropagation(); this.close(); if (pbt.help) pbt.help.open(); } },
      pbt.el('span', { class: 'pbt-q', text: '?' }), pbt.el('span', { class: 'pbt-name', text: 'Help and keys' }), pbt.el('span', { class: 'pbt-key', text: 'Ctrl+Alt+H' })),
      'Help and keys\nWhat the skins, the tools and the extras do, every key, and where choices are kept.'));
    // The credit line under everything (pbt.fingerprint, page/00-core.js).
    m.append(pbt.el('div', { class: 'pbt-blurb', text: pbt.fingerprint.credit }));

    document.body.append(m);
    // Anchored above the chip by its bottom edge, so folding a section or dragging the handle
    // grows or shrinks it upward; never taller than the room above the chip.
    const a = (anchor && anchor.isConnected ? anchor : document.querySelector('section.statusBar') || document.body).getBoundingClientRect();
    const r = m.getBoundingClientRect();
    m.style.left = `${Math.max(8, Math.min(a.left, innerWidth - r.width - 8))}px`;
    m.style.bottom = `${Math.max(8, innerHeight - a.top + 6)}px`;
    m.style.maxHeight = `${Math.max(200, a.top - 14)}px`;
    this.open = m;
    // Focus the menu itself, not its first tile: focus would raise the tile's tooltip at once.
    m.tabIndex = -1;
    m.focus({ preventScroll: true });
  },
  start() {
    this.build();
    pbt.watch(() => { this.build(); this.refresh(); });
    pbt.on(document, 'pointerdown', e => { if (this.open && !this.open.contains(e.target) && !(this.chip && this.chip.contains(e.target))) this.close(); }, true);
    pbt.on(document, 'keydown', e => { if (e.key === 'Escape' && this.open) { e.stopPropagation(); this.close(); } }, true);
    pbt.hotkey('k', 'Open the menu', () => this.toggle(this.chip), { desc: 'Skins, tools, extras and help, from anywhere in the window.' });
  },
  onConfig() { this.refresh(); },
  onApplied() { this.refresh(); if (this.open) this.open.querySelectorAll('.pbt-skins button').forEach((b, i) => b.setAttribute('aria-checked', String((i ? pbt.skin.ids()[i - 1] : 'off') === this.shownSkin()))); },
  // The hint's timer goes too, so a disabled copy never marks the hint as seen.
  stop() { this.close(); clearTimeout(this.hintTimer); document.querySelectorAll('.pbt-hint').forEach(n => n.remove()); },
};
pbt.menu = menu;
pbt.module(menu);

// ---- 30-qa.js ----
// Layout QA overlay (Ctrl+Alt+G): a grid and rulers in report pixels, every visual's type,
// position and size, and findings about each visual's edges, gaps and grid fit, drawn as
// marks on the page and explained in a card on each label. Hovering a visual shows its
// distances to the neighbours and the edges it shares with them. A status strip summarises
// the page and sets the grid. The overlay never takes clicks meant for the canvas.
//
// The findings engine (pbt.qa.analyse) is a pure function of the visual list, so
// scripts/qa-check.mjs can run fixtures through it from outside the page.

pbt.page = {
  // Report-pixel page size. Power BI scales the page to the zoom level, so the size comes from
  // the displayed width and the status bar zoom, snapped to the nearest whole-pixel size.
  geometry() {
    const da = document.querySelector('.displayArea');
    if (!da) return null;
    const rect = da.getBoundingClientRect();
    const dw = parseFloat(da.style.width) || rect.width;
    const dh = parseFloat(da.style.height) || rect.height;
    let w, h;
    if (Array.isArray(pbt.cfg.pageSize)) [w, h] = pbt.cfg.pageSize;
    else {
      const zoom = parseFloat((document.querySelector('#zoomValueButton') || {}).textContent) / 100 || 1;
      const ratio = dh / dw;
      let best = null;
      for (let cand = Math.floor(dw / (zoom + 0.006)); cand <= Math.ceil(dw / Math.max(0.05, zoom - 0.006)); cand++) {
        const ch = cand * ratio;
        const score = Math.abs(ch - Math.round(ch)) * 10 + (cand % 10 ? 0.5 : 0) + (Math.round(ch) % 10 ? 0.5 : 0) + Math.abs(cand - dw / zoom) / 50;
        if (!best || score < best.score) best = { cand, h: Math.round(ch), score };
      }
      [w, h] = best ? [best.cand, best.h] : [Math.round(dw / zoom), Math.round(dh / zoom)];
    }
    // The scale from the page's style width (871.111 px), not its laid-out rectangle (871.109):
    // Power BI positions visuals with the style's figure, and the difference flips edges that
    // fall on a half pixel.
    return { da, rect, w, h, scale: dw / w };
  },
  // { raw: true } skips the saved-file step below (scripts/tool-check.mjs measures the raw reading).
  visuals(g = this.geometry(), opts = {}) {
    if (!g) return [];
    // An edge on an exact half pixel can land either side by float noise; accept both.
    const draws = (c, s) => Math.abs(c * g.scale - s) <= 0.5 + 0.002;
    const list = [...g.da.querySelectorAll('.visualContainer')].map((el, i) => {
      const r = el.getBoundingClientRect();
      const type = el.querySelector('[class*="visual-"]');
      con)PBT"
LR"PBT(st typeName = (String(type && type.className).match(/\bvisual-(\w+)/) || [])[1] || 'visual';
      const kind = el.getAttribute('aria-roledescription') || typeName;
      const title = (el.getAttribute('aria-label') || '').trim();
      // Power BI rounds each edge to a whole screen pixel at the current zoom. Below 100% one
      // screen pixel covers more than one report pixel (at 68%, 339 and 340 both draw at 231), so
      // every whole report pixel that rounds to the edge seen is a candidate; the one on the
      // coarsest grid (8, then 4, then 2) wins, since that is where snapped visuals sit, then the
      // nearest. At 100% and above there is only one candidate.
      const edge = screen => {
        const s = Math.round(screen), guess = s / g.scale;
        const cands = [];
        for (let c = Math.floor((s - 0.5) / g.scale) - 1; c <= Math.ceil((s + 0.5) / g.scale) + 1; c++) if (draws(c, s)) cands.push(c);
        if (!cands.length) return Math.round(guess * 10) / 10;
        const rank = c => (c % 8 === 0 ? 0 : c % 4 === 0 ? 1 : c % 2 === 0 ? 2 : 3) * 10 + Math.abs(c - guess);
        return cands.sort((a, b) => rank(a) - rank(b))[0];
      };
      // Power BI rounds each edge (its container gets translate(round(x * s)) and width
      // round((x + w) * s) - round(x * s)), so both edges are read back and the size is their
      // difference. The scale must be Power BI's own (see geometry), or edges near a half pixel
      // read one off.
      const x = edge(r.left - g.rect.left), y = edge(r.top - g.rect.top);
      const right = edge(r.right - g.rect.left), bottom = edge(r.bottom - g.rect.top);
      return { i, el, r, kind, typeName, title, x, y, w: +(right - x).toFixed(1), h: +(bottom - y).toFixed(1) };
    }).filter(v => v.w > 0 && v.h > 0);
    // Below 100% zoom two report pixels can draw at the same screen pixel, so the reading can be
    // 1 px off. With the report's project files loaded, a visual whose saved position and size
    // draw exactly where it is drawn takes the saved values (v.fromFile); one moved since the save
    // keeps the reading.
    if (!opts.raw && g.scale < 0.999 && pbt.project && pbt.project.get()) {
      // The tools call this on timers; the matching is redone only when the page, the zoom or a
      // visual's drawn box changes.
      const page = pbt.project.page();
      const key = page && JSON.stringify([page.name, g.scale, g.rect.left, g.rect.top, list.map(v => [v.kind, v.title, Math.round(v.r.left), Math.round(v.r.top), Math.round(v.r.width), Math.round(v.r.height)])]);
      if (page && key !== this.snapKey) {
        this.snapKey = key;
        this.snap = list.map(v => {
          const f = pbt.project.match(v, page, Math.max(0.5, 1 / g.scale + 0.01)).file;
          if (!f) return null;
          const sx = Math.round(v.r.left - g.rect.left), sy = Math.round(v.r.top - g.rect.top);
          const sr = Math.round(v.r.right - g.rect.left), sb = Math.round(v.r.bottom - g.rect.top);
          return draws(f.x, sx) && draws(f.y, sy) && draws(f.x + f.w, sr) && draws(f.y + f.h, sb) ? { x: f.x, y: f.y, w: f.w, h: f.h, fromFile: true } : null;
        });
      }
      if (page) list.forEach((v, k) => { if (this.snap && this.snap[k]) Object.assign(v, this.snap[k]); });
    }
    return list;
  },
};

// ---- the findings engine ----
// analyse(visuals, { grid, nearMiss, snap, pageW, pageH }) returns, per visual, its findings
// (kind edge | gap | grid | spill, with the edge, how far, what it refers to and the suggested
// value), the edges it shares with others, its distances to the nearest neighbour or page edge
// in each direction, and a suggested x, y, w, h; plus the guides to draw and a summary.
const EDGE_NAMES = { x: ['left', 'right'], y: ['top', 'bottom'] };
const name = v => v.title || v.kind;
const edgeValue = (v, e) => e === 'left' ? v.x : e === 'right' ? v.x + v.w : e === 'top' ? v.y : e === 'bottom' ? v.y + v.h : e === 'cx' ? v.x + v.w / 2 : v.y + v.h / 2;
const listNames = vs => { const n = vs.map(name); return n.length <= 2 ? n.join(' and ') : `${n.slice(0, 2).join(', ')} and ${n.length - 2} more`; };

function analyse(vs, cfg = {}) {
  const grid = cfg.grid || 8, nearMiss = cfg.nearMiss || 8, snap = cfg.snap !== false;
  const pageW = cfg.pageW || 0, pageH = cfg.pageH || 0;
  const out = vs.map(v => ({ v, findings: [], shared: [], dist: {}, suggest: { x: v.x, y: v.y, w: v.w, h: v.h }, onGrid: true }));
  const guides = new Map(); // key axis:value -> { axis, value, edge, members: [v], delta }

  // Edges: shared and near misses, same edge name against every other visual.
  for (const axis of ['x', 'y']) {
    for (const e of EDGE_NAMES[axis]) {
      for (const a of out) {
        const va = edgeValue(a.v, e);
        const exact = [], near = new Map();
        for (const b of out) {
          if (b === a) continue;
          const d = va - edgeValue(b.v, e);
          if (Math.abs(d) < 0.5) exact.push(b.v);
          else if (Math.abs(d) >= 1 && Math.abs(d) <= nearMiss) { const k = String(edgeValue(b.v, e)); if (!near.has(k)) near.set(k, []); near.get(k).push(b.v); }
        }
        if (exact.length) a.shared.push({ axis, edge: e, value: va, members: exact });
        if (!near.size) continue;
        // A near miss counts when the other value is a line two or more visuals already share,
        // or when the other visual sits in the same row or column (both of its edges on this
        // axis within nearMiss of ours). A card's right edge happening to fall 4 px from a
        // chart's in another row is a coincidence, not a finding.
        const inLine = (p, q) => EDGE_NAMES[axis].every(ed => Math.abs(edgeValue(p, ed) - edgeValue(q, ed)) <= nearMiss);
        const groups = [...near.entries()].filter(([, refs]) => refs.length >= 2 || refs.some(b => inLine(a.v, b))).sort((p, q) => q[1].length - p[1].length);
        if (!groups.length) continue;
        const [refValue, refs] = groups[0];
        if (exact.le)PBT"
LR"PBT(ngth >= refs.length) continue; // it sits with as many as it misses: aligned
        const ref = +refValue, delta = va - ref;
        a.findings.push({ kind: 'edge', edge: e, axis, delta, ref, refs, text: `${e[0].toUpperCase()}${e.slice(1)} edge is ${Math.abs(delta)} px ${axis === 'x' ? (delta > 0 ? 'right of' : 'left of') : (delta > 0 ? 'below' : 'above')} ${listNames(refs)} (${ref}).` });
        if (e === 'left') { a.suggest.x = ref; a.suggest.w = a.v.x + a.v.w - ref; }
        else if (e === 'right') a.suggest.w = ref - a.suggest.x;
        else if (e === 'top') { a.suggest.y = ref; a.suggest.h = a.v.y + a.v.h - ref; }
        else a.suggest.h = ref - a.suggest.y;
        const key = `${axis}:${ref}`;
        if (!guides.has(key)) guides.set(key, { axis, value: ref, edge: e, members: [], delta, items: [] });
        guides.get(key).members.push(a.v);
        guides.get(key).items.push({ v: a.v, edge: e, delta });
      }
    }
    // Centers only matter when neither edge on this axis lines up or nearly lines up.
    const c = axis === 'x' ? 'cx' : 'cy';
    const loose = out.filter(a => !a.shared.some(s => s.axis === axis) && !a.findings.some(f => f.axis === axis));
    for (const a of loose) {
      const vc = edgeValue(a.v, c);
      const exact = loose.filter(b => b !== a && Math.abs(vc - edgeValue(b.v, c)) < 1).map(b => b.v);
      if (exact.length) a.shared.push({ axis, edge: c, value: vc, members: exact });
    }
  }

  // Distances: the nearest neighbour that overlaps on the other axis, else the page edge.
  for (const a of out) {
    const v = a.v;
    const dirs = {
      left: b => (v.y < b.y + b.h && b.y < v.y + v.h) && b.x + b.w <= v.x ? v.x - (b.x + b.w) : null,
      right: b => (v.y < b.y + b.h && b.y < v.y + v.h) && b.x >= v.x + v.w ? b.x - (v.x + v.w) : null,
      up: b => (v.x < b.x + b.w && b.x < v.x + v.w) && b.y + b.h <= v.y ? v.y - (b.y + b.h) : null,
      down: b => (v.x < b.x + b.w && b.x < v.x + v.w) && b.y >= v.y + v.h ? b.y - (v.y + v.h) : null,
    };
    const toPage = { left: v.x, right: pageW ? pageW - (v.x + v.w) : null, up: v.y, down: pageH ? pageH - (v.y + v.h) : null };
    for (const [dir, fn] of Object.entries(dirs)) {
      let best = null;
      for (const b of out) { if (b === a) continue; const d = fn(b.v); if (d != null && (!best || d < best.d)) best = { d: +d.toFixed(1), to: b.v }; }
      if (!best && toPage[dir] != null) best = { d: +toPage[dir].toFixed(1), to: null };
      if (best) a.dist[dir] = best;
    }
  }

  // Gaps: the page's usual gap between neighbours, and the ones 1 px to nearMiss off it. The
  // finding goes on the visual that comes second (right or below), with the position that
  // restores the usual gap.
  const gapList = [];
  for (const a of out) for (const dir of ['right', 'down']) { const d = a.dist[dir]; if (d && d.to && d.d >= 0 && d.d < 120) gapList.push({ a: a.v, b: d.to, dir, g: d.d }); }
  let usual = null;
  if (gapList.length >= 3) {
    const counts = new Map();
    gapList.forEach(x => counts.set(Math.round(x.g), (counts.get(Math.round(x.g)) || 0) + 1));
    usual = [...counts].sort((p, q) => q[1] - p[1] || p[0] - q[0])[0][0];
    for (const x of gapList) {
      const delta = x.g - usual;
      if (Math.abs(delta) < 1 || Math.abs(delta) > nearMiss) continue;
      const b = out.find(o => o.v === x.b);
      const from = name(x.a);
      // The move keeps any edge an edge finding already placed on a shared line: a near edge placed
      // there stays (the line outranks the gap), a far edge placed there keeps its value by resizing.
      const placed = edge => b.findings.find(f => f.kind === 'edge' && f.edge === edge);
      if (x.dir === 'right') {
        b.findings.push({ kind: 'gap', axis: 'x', delta, usual, text: `Gap to ${from} on its left is ${x.g} px; the page's usual gap is ${usual} px.`, refs: [x.a] });
        if (!placed('left')) { b.suggest.x = x.a.x + x.a.w + usual; const r = placed('right'); if (r) b.suggest.w = r.ref - b.suggest.x; }
      } else {
        b.findings.push({ kind: 'gap', axis: 'y', delta, usual, text: `Gap to ${from} above is ${x.g} px; the page's usual gap is ${usual} px.`, refs: [x.a] });
        if (!placed('top')) { b.suggest.y = x.a.y + x.a.h + usual; const bt = placed('bottom'); if (bt) b.suggest.h = bt.ref - b.suggest.y; }
      }
    }
  }

  // Grid and the page edge.
  let onGridCount = 0;
  for (const a of out) {
    const v = a.v;
    const off = [['left', v.x], ['top', v.y], ['right', v.x + v.w], ['bottom', v.y + v.h]].map(([e, val]) => [e, val - Math.round(val / grid) * grid]).filter(([, d]) => Math.abs(d) > 0.25);
    a.onGrid = !off.length;
    if (a.onGrid) onGridCount++;
    if (snap && off.length) {
      a.findings.push({ kind: 'grid', edges: off, text: `Off the ${grid} px grid: ${off.map(([e, d]) => `${e} by ${Math.abs(+d.toFixed(1))} px`).join(', ')}.`, suggestEdges: off.map(([e, d]) => [e, (e === 'left' || e === 'right' ? (e === 'left' ? v.x : v.x + v.w) : (e === 'top' ? v.y : v.y + v.h)) - d]) });
      // Snap the suggestion too, except edges an edge finding placed and both edges on an axis a gap
      // finding moved (that move is a translation, so the far edge goes with it); the finding lists
      // only the snaps applied, so the card and the figures agree.
      const moved = new Set(a.findings.filter(f => f.kind === 'edge').map(f => f.edge));
      for (const f of a.findings) if (f.kind === 'gap') (f.axis === 'x' ? ['left', 'right'] : ['top', 'bottom']).forEach(e => moved.add(e));
      const gridFinding = a.findings[a.findings.length - 1];
      gridFinding.suggestEdges = gridFinding.suggestEdges.filter(([e]) => !moved.has(e));
      for (const [e, val] of gridFinding.suggestEdges) {
        if (e === 'left') { a.suggest.w = a.suggest.x + a.suggest.w - val; a.suggest.x = val; }
        else if (e === 'right') a.suggest.w = val - a.suggest.x;
        else if (e === 'top') { a.suggest.h = a.suggest.y + a.suggest.h - val; a.suggest.y = val; })PBT"
LR"PBT(
        else a.suggest.h = val - a.suggest.y;
      }
    }
    if (v.x < 0 || v.y < 0 || (pageW && v.x + v.w > pageW + 0.25) || (pageH && v.y + v.h > pageH + 0.25)) a.findings.push({ kind: 'spill', text: 'Runs past the page edge.' });
  }

  const summary = {
    visuals: vs.length,
    edges: out.filter(a => a.findings.some(f => f.kind === 'edge')).length,
    gaps: out.filter(a => a.findings.some(f => f.kind === 'gap')).length,
    spills: out.filter(a => a.findings.some(f => f.kind === 'spill')).length,
    onGrid: onGridCount, usual, grid, snap,
  };
  return { visuals: out, guides: [...guides.values()], summary };
}

const qa = {
  name: 'qa',
  on: false,
  host: null,
  sig: '',
  result: null,
  geometry: null,
  hover: -1,
  analyse,
  cfgNow() { const q = pbt.cfg.qa || {}; return { grid: q.grid || 8, nearMiss: q.nearMiss || 8, snap: q.snap !== false }; },
  setQa(key, value) { pbt.cfg.qa = { ...(pbt.cfg.qa || {}), [key]: value }; pbt.save(); this.sig = ''; this.draw(false); },
  toggle() {
    this.on = !this.on;
    pbt.restack();
    if (!this.on) { this.clear(); pbt.toast('Layout QA off'); return; }
    this.draw(true);
  },
  clear() { if (this.host) { this.host.remove(); this.host = null; } this.sig = ''; this.hover = -1; this.result = null; },

  // The card on a label: what the visual is, where it sits, what to look at, what to do.
  card(a, g) {
    const { v, findings, shared, suggest } = a;
    const el = pbt.el;
    const edgeFindings = findings.filter(f => f.kind === 'edge'), gapFindings = findings.filter(f => f.kind === 'gap'), gridFinding = findings.find(f => f.kind === 'grid');
    const badge = edgeFindings.length ? ['bad', `${edgeFindings.length} edge${edgeFindings.length === 1 ? '' : 's'} off`] : gapFindings.length ? ['bad', `${gapFindings.length} odd gap${gapFindings.length === 1 ? '' : 's'}`] : findings.some(f => f.kind === 'spill') ? ['bad', 'off the page'] : gridFinding ? ['off', 'off the grid'] : ['ok', 'aligned'];
    const delta = (now, want) => want === now ? null : el('i', { text: `${want > now ? '\u25bc' : '\u25b2'}${Math.abs(+(want - now).toFixed(1))}` });
    const kpi = (label, now, want) => el('div', {}, el('small', { text: label }), el('strong', {}, String(now), delta(now, want)));
    const rows = [];
    const row = (kind, text, fix) => rows.push(el('div', { class: 'pbt-card-row' }, el('span', { class: `pbt-card-dot pbt-dot-${kind}` }), el('span', { text }), el('span', { class: 'pbt-card-fix', text: fix || '' })));
    for (const f of edgeFindings) row('edge', f.text, `${f.edge} \u2192 ${f.ref}`);
    for (const f of gapFindings) row('gap', f.text, f.axis === 'x' ? `x \u2192 ${suggest.x}` : `y \u2192 ${suggest.y}`);
    for (const f of findings.filter(x => x.kind === 'spill')) row('edge', f.text, '');
    if (gridFinding) row('grid', gridFinding.text, gridFinding.suggestEdges.map(([e, val]) => `${e} \u2192 ${val}`).join('\n'));
    else if (this.cfgNow().snap) row('ok', `On the ${this.cfgNow().grid} px grid.`);
    if (shared.length) {
      const parts = shared.map(s => `${s.edge === 'cx' ? 'center' : s.edge === 'cy' ? 'middle' : s.edge} with ${listNames(s.members)}`);
      row('ok', `Shares its ${parts.join('; ')}.`);
    } else if (!edgeFindings.length) row('ok', 'No neighbour shares an edge with it.');
    const d = a.dist;
    const dists = ['left', 'up', 'right', 'down'].filter(k => d[k]).map(k => `${k} ${d[k].d}${d[k].to ? '' : ' (page)'}`).join(' \u00b7 ');
    if (dists) row('dist', `Gaps: ${dists}.`);
    return el('div', { class: 'pbt-card' },
      el('header', {}, el('b', { text: v.title || v.kind }), v.title ? el('span', { class: 'pbt-card-type', text: v.kind }) : null, el('span', { class: `pbt-card-badge pbt-badge-${badge[0]}`, text: badge[1] })),
      el('div', { class: 'pbt-card-kpi' }, kpi('x', v.x, suggest.x), kpi('y', v.y, suggest.y), kpi('width', v.w, suggest.w), kpi('height', v.h, suggest.h)),
      el('div', { class: 'pbt-card-rows' }, ...rows),
      el('footer', {}, el('span', {}, el('b', { text: 'Click' }), ' copies x, y, w, h'), (suggest.x !== v.x || suggest.y !== v.y || suggest.w !== v.w || suggest.h !== v.h) ? el('span', {}, el('b', { text: 'Shift+click' }), ' copies the suggested values') : null));
  },

  // The status strip under the page (inside its bottom edge when there is no room below).
  strip(g, s) {
    const el = pbt.el;
    const cfg = this.cfgNow();
    const item = (cls, n, text, tip) => pbt.tip(el('span', { class: 'pbt-strip-item', dataset: { count: cls } }, cls ? el('span', { class: `pbt-card-dot pbt-dot-${cls}` }) : null, n != null ? el('b', { text: String(n) }) : null, el('span', { text })), tip);
    // The pointer press stays out of Power BI's hands (no canvas selection change); the action runs
    // on click, which Enter and Space raise as well.
    const swallow = e => { e.stopPropagation(); e.preventDefault(); };
    const button = (attrs, run, tip) => pbt.tip(el('button', { ...attrs, onpointerdown: swallow, onclick: e => { e.stopPropagation(); run(); } }), tip);
    const gridBtn = n => button({ class: `pbt-strip-grid${cfg.grid === n ? ' pbt-on' : ''}`, text: String(n) }, () => this.setQa('grid', n), `${n} px grid\nDraws the grid every ${n} px and checks edges against it (qa.grid).`);
    const snapBtn = button({ class: `pbt-strip-grid${cfg.snap ? ' pbt-on' : ''}`, text: 'grid check' }, () => this.setQa('snap', !cfg.snap), 'Grid check\nAmber marks on visuals with an edge off the grid (qa.snap). Snap to grid in the View ribbon places edges on it while you drag.');
    const helpBtn = button({ class: 'pbt-strip-help', text: '?' }, () => { if (pbt.help) { pbt.help.close(); pbt.help.open(); pbt.help.show('qa'); } }, 'What the marks mean\nOpens the help page at Layout QA.');
    const bar = el('div', { class: 'pbt-qa-strip' },
      item('', null, `${g.w} \u00d7 ${g.h} page${g.scale < 0.999 ? ` \u00b7 \u00b11 px at ${Math.round(g.scale * 100)}%` : ''}`, `Page size in report pixels\nRead fro)PBT"
LR"PBT(m the zoom level and the page's shape; pageSize in the settings pins it.${g.scale < 0.999 ? `\nBelow 100% zoom one screen pixel covers ${(1 / g.scale).toFixed(1)} report pixels, so an edge can read 1 px off; where two values draw the same, the one on the grid is shown. Zoom to 100% (or wider) for exact values.` : ''}`),
      item('vis', s.visuals, `visual${s.visuals === 1 ? '' : 's'}`, 'Visuals on this page\nEach has a label; hover a label for its card.'),
      item('edge', s.edges + s.spills, s.edges + s.spills === 1 ? 'edge off' : 'edges off', `Edges 1 to ${cfg.nearMiss} px off a neighbour's matching edge, and visuals past the page edge\nRed boxes; the dashed guide shows where the edge belongs.`),
      item('gap', s.gaps, s.usual != null ? `odd gap${s.gaps === 1 ? '' : 's'} \u00b7 usual ${s.usual}` : 'odd gaps', `Gaps between neighbours 1 to ${cfg.nearMiss} px off the page's usual gap${s.usual != null ? ` (${s.usual} px here)` : ''}\nHover a visual to read its gaps.`),
      cfg.snap ? item('grid', `${s.onGrid} of ${s.visuals}`, 'on the grid', `Visuals with all four edges on the ${cfg.grid} px grid\nAmber boxes are off it; their card says which edge and by how much.`) : null,
      el('span', { class: 'pbt-strip-seg' }, el('span', { class: 'pbt-strip-label', text: 'grid' }), ...[4, 8, 10, 16, 20].map(gridBtn), snapBtn),
      helpBtn);
    // Below the page when the window has room there, else inside its bottom edge.
    pbt.placeStrip(bar, 'qa', g);
    return bar;
  },

  // Stencil's blocks on this page, while Stencil is on, checked with the visuals by the same rules.
  // A block a built visual already covers is left out (pbt.stencil.qaBlocks), so the same
  // rectangle is not counted twice. Each reads as a block: its type plus "block", its label with
  // "(block)". Their places are in the draw's signature, so a block added, moved or removed is
  // picked up on the next tick.
  blocks(g) {
    const st = pbt.stencil;
    if (!st || !st.on || !st.qaBlocks) return [];
    return st.qaBlocks(g).map(b => ({ i: `st-${b.id}`, x: b.x, y: b.y, w: b.w, h: b.h, kind: `${b.typeLabel} block`, title: b.label ? `${b.label} (block)` : '', block: true }));
  },

  draw(announce) {
    const g = pbt.page.geometry();
    if (!g) { this.clear(); return; }
    const vs = pbt.page.visuals(g).concat(this.blocks(g));
    const cfg = this.cfgNow();
    const sig = JSON.stringify([g.rect.left, g.rect.top, g.rect.width, g.w, g.h, cfg, vs.map(v => [v.x, v.y, v.w, v.h, v.title])]);
    if (sig === this.sig && this.host && this.host.isConnected) return;
    this.sig = sig;
    if (this.host) this.host.remove();
    this.geometry = g;
    const hoverI = this.hover >= 0 && this.result && this.result.visuals[this.hover] ? this.result.visuals[this.hover].v.i : null;
    const res = this.result = analyse(vs, { ...cfg, pageW: g.w, pageH: g.h });
    const px = v => `${v * g.scale}px`;
    const host = this.host = pbt.el('div', { class: 'pbt-qa', 'data-pbt': '', style: `left:${g.rect.left}px;top:${g.rect.top}px;width:${g.rect.width}px;height:${g.rect.height}px;--pbt-grid:${cfg.grid * g.scale}px;--pbt-major:${cfg.grid * 5 * g.scale}px` });
    host.dataset.onGrid = cfg.snap ? `${res.summary.onGrid}/${vs.length}` : '';
    // Rulers every 50 report px, labelled every 100.
    const top = pbt.el('div', { class: 'pbt-ruler pbt-ruler-x' }), left = pbt.el('div', { class: 'pbt-ruler pbt-ruler-y' });
    for (let x = 0; x <= g.w; x += 100) top.append(pbt.el('span', { style: `left:${px(x)}`, text: String(x) }));
    for (let y = 0; y <= g.h; y += 100) left.append(pbt.el('span', { style: `top:${px(y)}`, text: String(y) }));
    host.append(top, left);

    // Guides: where a near-missed edge belongs, tagged with what is off and by how much.
    for (const gd of res.guides) {
      const line = pbt.el('div', { class: `pbt-guide pbt-guide-${gd.axis}`, style: gd.axis === 'x' ? `left:${px(gd.value)}` : `top:${px(gd.value)}` });
      // A vertical guide's tag sits in the first visual's bottom corner, a horizontal one's at its
      // right end, clear of Power BI's title row. The tag carries a distance and a direction only
      // when every visual on the line is off by the same amount the same way; otherwise it counts
      // them and each visual's card has its own numbers.
      const items = gd.items, m = items[0].v;
      const place = gd.axis === 'x'
        ? `top:${px(m.y + m.h - 4)};transform:translateY(-100%);${items[0].edge === 'right' ? 'right' : 'left'}:4px`
        : `right:${px(g.w - (m.x + m.w))};${items[0].edge === 'bottom' ? 'bottom' : 'top'}:4px`;
      const same = items.every(it => it.edge === items[0].edge && it.delta === items[0].delta);
      const edge = items[0].edge[0].toUpperCase() + items[0].edge.slice(1);
      const text = same
        ? `${edge} edge${items.length > 1 ? `s of ${items.length} visuals` : ''} ${Math.abs(items[0].delta)} px off the line at ${gd.value}`
        : `${items.length} edges off the line at ${gd.value}`;
      // The visuals on the line, so the tag can show only for a hovered one (pbt.applyFocus).
      line.append(pbt.el('span', { class: 'pbt-guide-tag', style: place, text, dataset: { vis: items.map(it => it.v.i).join(',') } }));
      host.append(line);
    }

    // Boxes and labels.
    this.hostHover = pbt.el('div', { class: 'pbt-hover' });
    for (const a of res.visuals) {
      const v = a.v;
      const bad = a.findings.some(f => f.kind === 'edge' || f.kind === 'gap' || f.kind === 'spill');
      const offGrid = a.findings.some(f => f.kind === 'grid');
      const box = pbt.el('div', { class: `pbt-box${bad ? ' pbt-bad' : offGrid ? ' pbt-off' : ''}${v.block ? ' pbt-box-block' : ''}`, dataset: { index: String(a.v.i) }, style: `left:${px(v.x)};top:${px(v.y)};width:${px(v.w)};height:${px(v.h)}` });
      const dims = `${v.x}, ${v.y} \u00b7 ${v.w} \u00d7 ${v.h}`;
      const n = a.findings.filter(f => f.kind !== 'grid').length;
      const label = pbt.tip(pbt.el('bu)PBT"
LR"PBT(tton', {
        class: 'pbt-label',
        onpointerdown: e => {
          e.stopPropagation(); e.preventDefault();
          const s = e.shiftKey ? a.suggest : v;
          pbt.copy(`${s.x}, ${s.y}, ${s.w}, ${s.h}`, `Copied ${e.shiftKey ? 'suggested ' : ''}${name(v)}: ${s.x}, ${s.y} \u00b7 ${s.w} \u00d7 ${s.h}`);
        },
      }, pbt.el('b', { text: v.kind }), pbt.el('span', { text: dims }), n ? pbt.el('em', { class: 'pbt-pill', text: `${n} ${n === 1 ? 'issue' : 'issues'}` }) : offGrid ? pbt.el('em', { text: '\u25a6' }) : null), () => this.card(a, g));
      box.append(label);
      host.append(box);
    }
    host.append(this.hostHover, this.strip(g, res.summary));
    document.body.append(host);
    pbt.applyFocus();
    this.declutter();
    // The hover layer is new, so the hovered visual (by its place in the page) is drawn again.
    this.hover = -1;
    if (hoverI != null) { const idx = res.visuals.findIndex(a => a.v.i === hoverI); if (idx >= 0) { this.hover = idx; this.drawHover(idx); } }
    if (announce) {
      const s = res.summary;
      const bad = s.edges + s.gaps + s.spills;
      const parts = [`${s.visuals} visuals`, bad ? `${bad} with an edge or gap to look at` : 'edges and gaps in order', s.usual != null ? `usual gap ${s.usual} px` : null, s.snap ? `${s.onGrid} of ${s.visuals} on the ${s.grid} px grid` : null];
      pbt.toast(`${parts.filter(Boolean).join(', ')}. Hover a label for its card; hover a visual for its gaps.`, { title: 'Layout QA', tone: bad ? 'bad' : 'good', ms: 6000 });
      if (pbt.fun) pbt.fun.say(bad ? 'qaBad' : 'qaGood');
    }
  },

  // Hover: the visual under the pointer shows its distances and the edges it shares.
  onMove(e) {
    if (!this.on || !this.host || !this.result || !this.geometry) return;
    const g = this.geometry;
    const rx = (e.clientX - g.rect.left) / g.scale, ry = (e.clientY - g.rect.top) / g.scale;
    let idx = -1;
    // A label belongs to its visual even where it hangs over another one (a text box behind a row of cards).
    const over = e.target && e.target.closest && e.target.closest('.pbt-box[data-index]');
    if (over) idx = this.result.visuals.findIndex(a => String(a.v.i) === over.dataset.index);
    if (idx < 0) this.result.visuals.forEach((a, i) => { const v = a.v; if (rx >= v.x && rx <= v.x + v.w && ry >= v.y && ry <= v.y + v.h && (idx < 0 || v.w * v.h < this.result.visuals[idx].v.w * this.result.visuals[idx].v.h)) idx = i; });
    if (idx === this.hover) return;
    this.hover = idx;
    this.drawHover(idx);
  },
  drawHover(idx) {
    const layer = this.hostHover;
    if (!layer) return;
    layer.replaceChildren();
    this.host.querySelectorAll('.pbt-box.pbt-hovered').forEach(b => b.classList.remove('pbt-hovered'));
    const hv = idx < 0 ? null : String(this.result.visuals[idx].v.i);
    for (const t of this.host.querySelectorAll('.pbt-guide-tag')) t.classList.toggle('pbt-hov', !!hv && t.dataset.vis.split(',').includes(hv));
    // A guide tag shown only for the hovered visual may land on a label, so the tags are placed again.
    if (idx < 0) { this.declutter(); return; }
    const g = this.geometry, a = this.result.visuals[idx], v = a.v;
    const px = n => `${n * g.scale}px`;
    const box = this.host.querySelector(`.pbt-box[data-index="${v.i}"]`);
    if (box) box.classList.add('pbt-hovered');
    const el = pbt.el;
    // Distances in the four directions, drawn from the visual's mid line to the neighbour or the page edge.
    const midX = v.x + v.w / 2, midY = v.y + v.h / 2;
    const dist = (dir, d) => {
      if (!d || d.d <= 0) return;
      const to = d.to;
      let x1, y1, x2, y2;
      if (dir === 'left') { y1 = y2 = midY; x1 = v.x - d.d; x2 = v.x; }
      else if (dir === 'right') { y1 = y2 = midY; x1 = v.x + v.w; x2 = x1 + d.d; }
      else if (dir === 'up') { x1 = x2 = midX; y1 = v.y - d.d; y2 = v.y; }
      else { x1 = x2 = midX; y1 = v.y + v.h; y2 = y1 + d.d; }
      const horizontal = dir === 'left' || dir === 'right';
      layer.append(el('div', { class: `pbt-dist-line ${horizontal ? 'pbt-h' : 'pbt-v'}`, style: horizontal ? `left:${px(x1)};top:${px(y1)};width:${px(x2 - x1)}` : `left:${px(x1)};top:${px(y1)};height:${px(y2 - y1)}` }));
      layer.append(el('div', { class: `pbt-dist${to ? '' : ' pbt-dist-page'}`, style: `left:${px((x1 + x2) / 2)};top:${px((y1 + y2) / 2)}`, text: `${d.d}${to ? '' : ' to page'}` }));
    };
    for (const dir of ['left', 'right', 'up', 'down']) dist(dir, a.dist[dir]);
    // Edges shared with others: a green line across the page with a tag.
    for (const s of a.shared) {
      if (s.edge === 'cx' || s.edge === 'cy') continue;
      const line = el('div', { class: `pbt-shared pbt-shared-${s.axis}`, style: s.axis === 'x' ? `left:${px(s.value)}` : `top:${px(s.value)}` });
      line.append(el('span', { class: 'pbt-shared-tag', text: `${s.edge} shared by ${s.members.length + 1}` }));
      layer.append(line);
    }
    this.declutter();
  },

  // Tags (the red notes on guides, the green shared-edge tags and the blue distances) never sit on
  // a label or on each other. A tag that does tries the other side of its own line first, then
  // slides along the line in steps of half its size, both ways and on both sides, and takes the
  // first free spot inside the overlay; a distance has no line of its own, so it tries small moves
  // up, down, left and right. Labels never move. Hidden tags (a guide tag waiting for its visual's
  // hover) are skipped. The offset goes in the translate property, so a tag's own transform (the
  // guide tag's lift, the distance's centering) stays as it is. Runs after a draw and a hover only.
  declutter() {
    const host = this.host;
    if (!host || !host.isConnected) return;
    const tags = [...host.querySelectorAll('.pbt-guide-tag, .pbt-shared-tag, .pbt-dist')];
    for (const t of tags) if (t.style.translate) t.style.translate = '';
    const shown = e => { const r = e.getBoundingClientRect(); return r.width > 0 && r.hei)PBT"
LR"PBT(ght > 0 && getComputedStyle(e).visibility !== 'hidden' ? r : null; };
    const placed = [...host.querySelectorAll('.pbt-label')].map(shown).filter(Boolean);
    const hb = host.getBoundingClientRect();
    const hit = (a, b) => a.left < b.right - 1 && b.left < a.right - 1 && a.top < b.bottom - 1 && b.top < a.bottom - 1;
    const inside = q => q.left >= hb.left && q.right <= hb.right && q.top >= hb.top && q.bottom <= hb.bottom;
    const GAP = 3;
    for (const t of tags) {
      const r = shown(t);
      if (!r) continue;
      const at = (dx, dy) => ({ left: r.left + dx, right: r.right + dx, top: r.top + dy, bottom: r.bottom + dy });
      const free = q => !placed.some(p => hit(q, p));
      let best = [0, 0];
      if (!free(at(0, 0))) {
        const tries = [];
        const line = t.parentElement, cls = line ? line.classList : null;
        const horizontal = !!cls && (cls.contains('pbt-guide-y') || cls.contains('pbt-shared-y'));
        const vertical = !!cls && (cls.contains('pbt-guide-x') || cls.contains('pbt-shared-x'));
        if (horizontal || vertical) {
          // across: the move that puts the tag on the other side of its line, a small gap away;
          // along: steps of half the tag's length.
          const lr = line.getBoundingClientRect();
          const across = horizontal
            ? ((r.top + r.bottom) / 2 >= lr.top ? lr.top - GAP - r.bottom : lr.top + GAP - r.top)
            : ((r.left + r.right) / 2 >= lr.left ? lr.left - GAP - r.right : lr.left + GAP - r.left);
          const step = Math.max(6, (horizontal ? r.width : r.height) / 2);
          const span = horizontal ? hb.width : hb.height;
          const move = (along, side) => horizontal ? [along, side] : [side, along];
          tries.push(move(0, across));
          for (let k = 1; k * step <= span; k++) for (const s of [1, -1]) for (const side of [0, across]) tries.push(move(s * k * step, side));
        } else {
          const sx = r.width / 2 + GAP, sy = r.height + GAP;
          for (let k = 1; k <= 4; k++) tries.push([0, -k * sy], [0, k * sy], [-k * sx, 0], [k * sx, 0]);
        }
        best = tries.find(([dx, dy]) => { const q = at(dx, dy); return inside(q) && free(q); }) || [0, 0];
      }
      if (best[0] || best[1]) t.style.translate = `${Math.round(best[0])}px ${Math.round(best[1])}px`;
      placed.push(at(best[0], best[1]));
    }
  },

  start() {
    pbt.hotkey('g', 'Layout QA', () => this.toggle(), {
      id: 'qa',
      tool: true, active: () => this.on,
      desc: 'Grid, rulers and every visual\'s position and size in report pixels. Red marks edges off a neighbour and odd gaps, amber marks edges off the grid; hover a label for the card, hover a visual for its gaps.',
      tip: 'Layout QA\nAn overlay on the report page; it takes no clicks meant for the canvas and redraws as visuals move. Each label opens a card that says what to fix and gives the values; Shift+click copies them.',
    });
    // Cheap while on: redraws only when page geometry or a visual moves.
    pbt.every(300, () => { if (this.on) this.draw(false); });
    pbt.on(document, 'pointermove', e => this.onMove(e), { capture: true, passive: true });
  },
  stop() { this.clear(); },
};

// Clipboard: the async API when the page allows it, else the old textarea route.
pbt.copy = (text, msg) => {
  const done = () => pbt.toast(msg || `Copied ${text}`, { ms: 2500 });
  const fallback = () => {
    const t = pbt.el('textarea', { 'data-pbt': '', style: 'position:fixed;left:-9999px' });
    t.value = text;
    document.body.append(t);
    t.select();
    try { document.execCommand('copy'); done(); } catch { pbt.toast(text, { title: 'Copy this', ms: 6000 }); }
    t.remove();
  };
  if (navigator.clipboard && navigator.clipboard.writeText) navigator.clipboard.writeText(text).then(done, fallback);
  else fallback();
};

pbt.qa = qa;
pbt.module(qa);

// ---- 35-filters.js ----
// Filter map (Ctrl+Alt+F): which visuals carry filters and where each comes from, plus bulk hide,
// show, lock and unlock of visual-level filters for one visual, the page or every page.
//
// Sources (probed against Power BI Desktop, 2026-10-30):
// - The Filters pane: sections captioned "Filters on this visual" (only while a visual is
//   selected), "Filters on this page" and "Filters on all pages". Each .card has a title button
//   whose aria-label states the filter ("Region is East"; field-well cards read "X is (All)" and
//   filter nothing) and buttons .filterPaneIcon.hide (aria "Hide filter" / "Show filter", glyph
//   pbi-glyph-view / pbi-glyph-hide3) and .filterPaneIcon.lock (glyph pbi-glyph-unlock /
//   pbi-glyph-lock).
// - Each visual's header button .vcFilterRestatementBtn opens Power BI's own list of what affects
//   the visual ("Filters and slicers affecting this visual"): one .filterRestatementCard per
//   source, a <strong> heading (the field, or "Included (1)" for a data selection) and an li per
//   condition ("is East"). Visual-level filters hidden in view mode are left out of it, so the
//   pane is the source for visual-level filters and the popup supplies the rest: slicers, cross-
//   filters and selections from other visuals ("from another element").
// - A synthetic click selects a visual and keeps slicer selections; a click on a page tab's
//   [role="tab"] switches pages. The header button exists on the selected or hovered visual.
// Nothing here writes to the report except the bulk actions, which press the pane's own buttons.

const fsleep = ms => new Promise(r => setTimeout(r, ms));
const fwait = async (test, ms = 2000, step = 80) => { const end = Date.now() + ms; let v; while (!(v = test()) && Date.now() < end) await fsleep(step); return v; };
const LEVELS = {
  visual: { label: 'Visual', short: 'V', words: 'visual', tip: 'A visual-level filter, set on this visual in the Filters pane.' },
  page: { label: 'Page', short: 'P', words: 'page', tip: 'A page-level filter: every visual on this page.' },
  report: { la)PBT"
LR"PBT(bel: 'Report', short: 'R', words: 'report', tip: 'A report-level filter: every visual on every page.' },
  other: { label: 'Slicers & cross-filters', short: 'S', words: 'from slicers & cross-filters', tip: 'From slicers & cross-filters: a slicer, a cross-filter or a selection in another visual, a drillthrough or a tooltip page.' },
};

// What each counter on the card counts, in a line under it.
const KPI_NOTES = {
  visual: 'filters set on this visual itself',
  page: 'page filters, on every visual here',
  report: 'report filters, on every page',
  other: 'from slicers, clicks in other visuals, drillthrough',
};
// How long, in ms, one reading of where a badge's cross-filters come from is reused: long enough
// for a hover and the card it opens 350 ms later to share it, short enough that the next hover,
// after a click on another chart, reads the canvas again.
const SOURCES_MS = 600;
// After a page change, the map waits for the canvas to go quiet this long (no visual added or
// removed) before it reads the new page, and never longer than SETTLE_MAX in all.
const SETTLE_QUIET = 500;
const SETTLE_MAX = 3000;
// A list read is trusted for FRESH_MS at most, whatever else happens: some changes are never seen
// here (a slicer inside a custom visual's frame, a refresh finishing, a model edit), so a kept list
// is read again on the next pass once it is this old.
const FRESH_MS = 10000;
// Power BI applies a slicer pick or a filter change a moment after the click. A read counts as
// fresh only when it began at least DIRTY_SETTLE ms after the last change, and the pass a change
// schedules runs DIRTY_PASS ms after it (each new change moves it later).
const DIRTY_SETTLE = 1500;
const DIRTY_PASS = 2000;
const filtermap = {
  name: 'filters',
  on: false,
  host: null,
  result: null,
  busy: false,
  cancel: false,
  sig: '',

  // ---- the Filters pane ----
  sections() {
    const out = {};
    for (const cap of document.querySelectorAll('.outspacePane .caption')) {
      const t = cap.textContent.trim().toLowerCase();
      const header = cap.closest('.header');
      const box = header && header.parentElement;
      if (!box) continue;
      const key = /this visual/.test(t) ? 'visual' : /this page/.test(t) ? 'page' : /all pages/.test(t) ? 'report' : null;
      if (key) out[key] = { header, box };
    }
    return out;
  },
  readCards(section) {
    if (!section) return [];
    return [...section.box.querySelectorAll('.card')].map(el => {
      const titleEl = el.querySelector('.title');
      const title = ((titleEl && (titleEl.getAttribute('aria-label') || titleEl.textContent)) || '').trim();
      const hideB = el.querySelector('.filterPaneIcon.hide'), lockB = el.querySelector('.filterPaneIcon.lock');
      return {
        el, title,
        field: title.replace(/\s+is\s+.*$/, ''),
        active: !!title && !/\sis \(All\)$/.test(title),
        hidden: !!hideB && (/show filter/i.test(hideB.getAttribute('aria-label') || '') || /pbi-glyph-hide/.test(hideB.className)),
        locked: !!lockB && (/glyph-lock\b/.test(lockB.className) || /locked/i.test(lockB.getAttribute('aria-label') || '')),
      };
    });
  },

  // ---- the canvas ----
  selectedIndex() { return pbt.page.visuals().findIndex(v => v.el.classList.contains('selected')); },
  // A click near the visual's top-left corner, inside its frame: selects it without hitting a
  // data point (a column, a table row) that would cross-filter the page.
  select(v) {
    const r = v.el.getBoundingClientRect();
    for (const [x, y] of [[r.left + 4, r.top + 4], [r.right - 4, r.bottom - 4], [r.left + r.width / 2, r.top + 6]]) {
      const t = document.elementFromPoint(x, y);
      if (!t || !v.el.contains(t)) continue;
      for (const type of ['pointerdown', 'mousedown', 'pointerup', 'mouseup', 'click']) t.dispatchEvent(new MouseEvent(type, { bubbles: true, cancelable: true, clientX: x, clientY: y, button: 0, view: window }));
      return true;
    }
    return false;
  },
  // select(), then wait for Power BI to take it. Right after a click elsewhere (a slicer pick) it
  // can ignore a synthetic select for a moment: clear the selection and try once more. alive() is
  // the operation's own check (stopped, Esc pressed); nothing is clicked once it fails.
  async selectNow(v, alive = () => true, tries = 2) {
    for (let k = 0; k < tries && alive(); k++) {
      if (k) { this.deselect(); await fsleep(400); if (!alive()) return false; }
      if (!this.select(v)) return false;
      if (await fwait(() => v.el.classList.contains('selected'), 1200)) return alive();
    }
    return false;
  },
  // A click on the canvas around the page (.displayAreaViewport) clears the selection; a click on
  // the page itself does not when synthetic (probed 2026-10-30).
  deselect() {
    const g = pbt.page.geometry();
    if (!g) return false;
    const vp = g.da.closest('.displayAreaViewport') || document.querySelector('.displayAreaViewport');
    const midX = g.rect.left + g.rect.width / 2, midY = g.rect.top + g.rect.height / 2;
    for (const [x, y] of [[g.rect.left - 8, midY], [g.rect.right + 8, midY], [midX, g.rect.top - 8], [midX, g.rect.bottom + 8]]) {
      const t = document.elementFromPoint(x, y);
      if (!t || (vp && !vp.contains(t)) || g.da.contains(t) || t.closest('[data-pbt]')) continue;
      for (const type of ['pointerdown', 'mousedown', 'pointerup', 'mouseup', 'click']) t.dispatchEvent(new MouseEvent(type, { bubbles: true, cancelable: true, clientX: x, clientY: y, button: 0, view: window }));
      return true;
    }
    return false;
  },
  // Ends a synthetic hover, so Power BI drops the visual's header and its hover popup.
  unhover(t, x, y) {
    if (!t) return;
    for (const type of ['pointerout', 'mouseout', 'pointerleave', 'mouseleave']) t.dispatchEvent(new MouseEvent(type, { bubbles: !/leave/.test(type), clientX: x, clientY: y, relatedTarget: document.body, view: window }));
  },
  // Closes the filter list this tool op)PBT"
LR"PBT(ened, and nothing else: the Escape goes to that popup and
  // the backdrop clicked is the one in its own overlay box, never a menu or dropdown of yours.
  closePopup() {
    const box = document.querySelector('filter-restatement');
    if (!box) return;
    box.dispatchEvent(new KeyboardEvent('keydown', { key: 'Escape', code: 'Escape', bubbles: true }));
    const wrap = box.closest('.cdk-overlay-popover, .cdk-overlay-connected-position-bounding-box, .cdk-global-overlay-wrapper');
    const bd = wrap && wrap.querySelector(':scope > .cdk-overlay-backdrop');
    if (bd && document.querySelector('filter-restatement')) bd.click();
  },
  // Something of yours is open or being typed in: a dropdown, a menu, a dialog (an overlay that
  // is not this tool's), or a text field with focus. A pass then waits.
  userBusy() {
    const a = document.activeElement;
    if (a && (a.isContentEditable || /^(INPUT|TEXTAREA|SELECT)$/.test(a.tagName)) && !a.closest('[data-pbt]')) return true;
    // Menus, dropdowns and dialogs open with a backdrop; tooltips (a chart's data tooltip under a
    // resting pointer) have none and never hold a pass back.
    return [...document.querySelectorAll('.cdk-overlay-container .cdk-overlay-backdrop')].some(bd => !(bd.parentElement && bd.parentElement.querySelector('filter-restatement-overlay, filter-restatement')));
  },
  // Power BI's own list of what affects the visual, as [{ heading, items }]; null when the
  // visual has no filter button (shapes, some text boxes). The hover goes to the visual's top-left
  // corner, its title area, so no data point opens a tooltip; the caller hides the popup and the
  // header icons while it runs (quiet).
  async popup(v) {
    const r = v.el.getBoundingClientRect(), x = r.left + Math.min(10, r.width / 4), y = r.top + Math.min(8, r.height / 4);
    const t = document.elementFromPoint(x, y);
    if (t) for (const type of ['pointerover', 'mouseover', 'pointermove', 'mousemove']) t.dispatchEvent(new MouseEvent(type, { bubbles: true, clientX: x, clientY: y, view: window }));
    const b = await fwait(() => v.el.querySelector('.vcFilterRestatementBtn'), 800);
    if (!b) { this.unhover(t, x, y); return null; }
    // The list takes keyboard focus when it opens and hands it back on closing, and that focus event
    // raises the tooltip of whatever control gets it (the page tab, the left rail, Switch ribbons).
    // While the list is read, a focus call on anything inside its overlay is ignored, so focus never
    // moves and handing it back changes nothing. Undone in finally, whatever happens.
    const focus = HTMLElement.prototype.focus;
    HTMLElement.prototype.focus = function (o) { return this.closest && this.closest('.cdk-overlay-container') ? undefined : focus.call(this, o); };
    try {
      // A click that does not bubble: the button opens its list, but the visual never hears the
      // click, so it is not selected (a bubbling click selects it and the Filters pane follows).
      b.dispatchEvent(new MouseEvent('click', { bubbles: false, cancelable: true, view: window }));
      const box = await fwait(() => document.querySelector('filter-restatement'), 1500);
      await fsleep(120);
      const cards = box ? [...box.querySelectorAll('.filterRestatementCard')].map(cd => ({
        heading: ((cd.querySelector('strong') || {}).textContent || '').trim(),
        items: [...cd.querySelectorAll('li')].map(li => (li.getAttribute('title') || li.textContent).trim()).filter(Boolean),
      })) : [];
      this.closePopup();
      await fwait(() => !document.querySelector('filter-restatement'), 800);
      this.unhover(t, x, y);
      return cards;
    } finally { HTMLElement.prototype.focus = focus; }
  },

  // ---- the pure core: one visual's filters by level ----
  // cards: { visual, page, report } as read from the pane; popup: [{ heading, items }] or null.
  // A popup condition that states a pane card ("Region" + "is East" = "Region is East") is that
  // card; the rest come from other elements.
  parse(cards, popup) {
    const items = [];
    const known = new Set();
    // Visual-level filters read from the saved files may be worded differently from Power BI's
    // list (an advanced filter, a date); a popup entry on the same field is that filter.
    // Only visible ones count (hidden filters are not in Power BI's list), and only those whose
    // own wording is not in the list.
    const listed = new Set((popup || []).flatMap(p => (p.items.length ? p.items : ['']).map(cond => `${p.heading}${cond ? ` ${cond}` : ''}`.trim().toLowerCase())));
    const visualFields = new Map();
    for (const level of ['visual', 'page', 'report']) {
      for (const c of (cards[level] || [])) {
        if (!c.active) continue;
        known.add(c.title.toLowerCase());
        if (level === 'visual' && !c.hidden && !listed.has(c.title.toLowerCase())) visualFields.set(c.field.toLowerCase(), (visualFields.get(c.field.toLowerCase()) || 0) + 1);
        items.push({ level, field: c.field, text: c.title, hidden: !!c.hidden, locked: !!c.locked, saved: !!c.fromFile });
      }
    }
    for (const p of popup || []) {
      const conds = p.items.length ? p.items : [''];
      for (const cond of conds) {
        const text = `${p.heading}${cond ? ` ${cond}` : ''}`.trim();
        if (known.has(text.toLowerCase())) continue;
        const f = p.heading.toLowerCase();
        if (visualFields.get(f)) { visualFields.set(f, visualFields.get(f) - 1); continue; }
        items.push({ level: 'other', field: p.heading, text: /^included|^excluded/i.test(p.heading) ? `${p.heading}: ${cond}` : text, hidden: false, locked: false });
        known.add(text.toLowerCase());
      }
    }
    return items;
  },

  // ---- what is known about each visual ----
  // live: visual element -> its "Filters on this visual" cards, read from the pane whenever the
  // visual is selected (by you, or by a bulk action); popups: visual element -> Power BI's list.
  // Keyed b)PBT"
LR"PBT(y page, type and position, not by element: Power BI builds new elements when you
  // leave a page and come back.
  live: new Map(),
  popups: new Map(),
  key(v) { return `${this.pageNow() || ''}|${v.kind}|${v.x},${v.y},${v.w},${v.h}`; },
  // The open page, as its saved internal name (when the project files have it), its place among
  // the page tabs and the tab's text: two pages of the same name are never taken for one another.
  // key() calls this for every visual, inside sorts too, so the answer is kept for 100 ms: a pass
  // still sees a page change at its next check.
  pageNow() {
    const t = performance.now();
    if (this.pageAt !== undefined && t - this.pageAt < 100) return this.pageKey;
    this.pageKey = this.readPage();
    this.pageAt = t;
    return this.pageKey;
  },
  readPage() {
    const tabs = document.querySelectorAll('.carouselScrollPane li.section');
    let i = -1;
    for (let k = 0; k < tabs.length; k++) if (tabs[k].classList.contains('selected')) { i = k; break; }
    if (i < 0) return undefined;
    const p = pbt.project && pbt.project.get() ? pbt.project.page() : null;
    let name = p ? p.name : '';
    // page() finds a page by its display name; when several share it, the one at this tab's place.
    if (p) { const pages = pbt.project.get().pages; if (pages.filter(x => x.displayName === p.displayName).length > 1) name = pages[i] && pages[i].displayName === p.displayName ? pages[i].name : ''; }
    return `${name}|${i}|${tabs[i].textContent}`;
  },
  // Everything kept belongs to one set of project files: when the mod hands over new ones (another
  // report opened, or a save), every list read so far goes stale and is read again; for another
  // report (a new folder, or files appearing or going) what was kept is dropped as well.
  cacheRaw: undefined,
  syncRaw() {
    const raw = (pbt.project && pbt.project.raw) || null;
    if (raw === this.cacheRaw) return;
    const same = !!raw && !!this.cacheRaw && raw.root === this.cacheRaw.root;
    this.cacheRaw = raw;
    this.readAt.clear(); this.round = null;
    this.paneSigs.clear();
    if (!same) { this.popups.clear(); this.live.clear(); }
    this.dirty();
  },
  // A visual's visual-level filters: read live if it has been selected since the page opened,
  // else from the saved project files (hidden and locked included), else unknown (null).
  visualCards(v, page, tol) {
    if (this.live.has(this.key(v))) return this.live.get(this.key(v));
    if (!page) return null;
    const m = pbt.project.match(v, page, tol);
    if (!m.file) return null;
    return m.file.filters.map(f => ({ title: f.text, field: f.property, active: f.applied, hidden: f.hidden, locked: f.locked, fromFile: true }));
  },
  compose() {
    this.syncRaw();
    const g = pbt.page.geometry();
    if (!g) { this.result = null; return; }
    const vs = pbt.page.visuals(g);
    const sec = this.sections();
    const page = this.readCards(sec.page), report = this.readCards(sec.report);
    const proj = pbt.project && pbt.project.get() ? pbt.project.page() : null;
    const tol = Math.max(0.5, 1 / g.scale + 0.01);
    const rows = vs.map(v => {
      const visual = this.visualCards(v, proj, tol);
      const popup = this.popups.has(this.key(v)) ? this.popups.get(this.key(v)) : undefined;
      return { v, items: this.parse({ visual: visual || [], page, report }, popup || []), noButton: popup === null, unread: popup === undefined, unknown: visual === null, live: this.live.has(this.key(v)) };
    });
    this.result = { at: Date.now(), page: page.filter(c => c.active).length, report: report.filter(c => c.active).length, rows, project: !!proj };
  },
  // The cheap refresh, nothing clicked: page and report filters from the pane, and the selected
  // visual's own cards. Runs whenever the Filters pane changes.
  // The selected visual's own cards, kept even while the map is off, so a visual edited then is
  // not shown with old cards later.
  capturePane() {
    const el = document.querySelector('.displayArea .visualContainer.selected');
    const sec = el && this.sections().visual;
    if (!sec) return;
    const sel = pbt.page.visuals().find(v => v.el === el);
    if (sel) this.live.set(this.key(sel), this.readCards(sec));
  },
  // A change in the page or report filters the Filters pane shows (one added, edited, hidden,
  // locked, even with no click of yours, as when a refresh ends) can change Power BI's lists: the
  // lists read so far go stale. Compared per page, so the pane changing with the page tab does not
  // count. "Filters on this visual" is left out: it changes with every selection, this tool's own
  // included, and an edit of yours there comes with a click or key, which marks the lists stale
  // already. Noted only while the map is on and its page has settled with nothing running, so the
  // pane read is the one for the page it is filed under (during a page change, or a run over
  // every page, the pane can still show the page before).
  paneSigs: new Map(),
  notePane() {
    const page = this.pageNow() || '';
    if (!this.on || this.settling || this.busy || page !== (this.pageName || '')) return;
    const sec = this.sections();
    const sig = JSON.stringify(['page', 'report'].map(k => this.readCards(sec[k]).map(c => [c.title, c.hidden, c.locked])));
    const was = this.paneSigs.get(page);
    if (was !== undefined && was !== sig) this.dirty();
    this.paneSigs.set(page, sig);
  },
  refreshPane() {
    this.capturePane();
    this.checkPage();
    this.notePane();
    // While a page is still being drawn, nothing is composed: the page watcher in start() scans
    // once it has settled.
    if (this.on && !this.settling) { this.compose(); this.sig = this.layoutSig(); this.draw(); }
  },
  // The popup and the visual headers out of sight (skins/_base.css), for one read at a time: on
  // just before a popup opens, off as soon as it has closed, never across the waits between reads.
  // The class goes on every ove)PBT"
LR"PBT(rlay container and every page area there is at that moment, never
  // on html while a container exists: a class change on html restyles the whole document. Before
  // Power BI has made its container (no menu or popup opened yet), html carries it for that read.
  quiet(on) {
    if (!on) { for (const e of document.querySelectorAll('.pbt-fm-quiet')) e.classList.remove('pbt-fm-quiet'); document.documentElement.classList.remove('pbt-fm-quiet'); return; }
    const boxes = document.querySelectorAll('.cdk-overlay-container');
    if (!boxes.length) { document.documentElement.classList.add('pbt-fm-quiet'); return; }
    for (const e of [...boxes, ...document.querySelectorAll('.displayArea')]) e.classList.add('pbt-fm-quiet');
  },
  // Power BI's list for every visual (slicers, cross-filters), read out of sight: the popup and
  // the visual headers are hidden while it runs, nothing is selected.
  // One operation at a time (a pass, a bulk action, reading each visual): each waits its turn in
  // a queue instead of being refused or running beside another. gen changes when the module
  // stops, and every loop checks it, so nothing keeps clicking after a stop or a reload.
  chain: Promise.resolve(),
  pending: 0,
  gen: 0,
  exclusive(fn) {
    const gen = this.gen;
    this.pending++;
    // An operation skipped because the map stopped or was turned off still leaves the count, or
    // idle() would wait for it forever.
    const run = this.chain.then(async () => {
      if (gen !== this.gen) { this.pending--; return undefined; }
      this.busy = true;
      try { return await fn(gen); } finally { this.busy = false; this.pending--; }
    }, () => { this.pending--; });
    this.chain = run.catch(() => {});
    return run;
  },
  // Resolves when nothing is running or queued.
  async idle() { while (this.pending) await this.chain; },
  // Power BI's list for every visual, out of sight. Requests while one is queued join it; a pass
  // waits while a dropdown, menu or text field of yours is open, and stops after 15 s, carrying on
  // from the next unread visual in a follow-up pass.
  // A list read is kept until something could have changed it (dirtyAt): a real click or key on
  // the report canvas or in the Filters pane, an undo or redo (touchesFilters), a change in the
  // Filters pane's page or report filters, a bulk action, new project files, Rescan or turning the
  // map on; and never longer than FRESH_MS. A read
  // that began within DIRTY_SETTLE ms of a change may hold the list from before it (Power BI was
  // still applying it), so it is shown but read again. A pass reads only the visuals whose list is
  // not fresh, so coming back soon to a page you only looked at reads nothing.
  readAt: new Map(),
  dirtyAt: 0,
  // While the map is on, a change also schedules a pass for when Power BI has applied it.
  dirty() { this.dirtyAt = performance.now(); this.round = null; if (this.on) this.later(DIRTY_PASS); },
  later(ms) {
    clearTimeout(this.refreshTimer);
    this.refreshTimer = setTimeout(() => { if (this.on) this.refreshPopups(); }, ms);
  },
  fresh(v) {
    const k = this.key(v), at = this.readAt.get(k);
    return at !== undefined && this.popups.has(k) && at - this.dirtyAt >= DIRTY_SETTLE && performance.now() - at < FRESH_MS;
  },
  refreshPopups() {
    // A copy that was stopped (the mod disabled, a reload) starts nothing.
    if (!pbt.current()) return Promise.resolve();
    // A minimized or hidden window throttles timers (a 100 ms wait takes most of a second), so
    // the lists are read when it is shown again.
    if (document.hidden) { this.stale = true; return Promise.resolve(); }
    if (this.passQueued) return this.passPromise;
    this.passQueued = true;
    this.passPromise = this.exclusive(async gen => {
      this.passQueued = false;
      // Nothing is read or drawn with the map off, or for a page still being drawn or not yet
      // taken up by the page watcher (which settles it and then scans it).
      const page = this.pageNow();
      const ok = () => gen === this.gen && this.on && !this.settling;
      if (!ok() || (page || '') !== (this.pageName || '')) return;
      if (this.userBusy()) { this.retryPass(1500); return; }
      this.syncRaw();
      const until = Date.now() + 15000;
      let resumed = false, left = false, early = false;
      const vs = pbt.page.visuals();
      // A round is the run of passes that carries on after a budget stop. Visuals read earlier in
      // the round are not read again, even when they have gone stale meanwhile, so every visual is
      // read once per round whatever the page size. Unread visuals go first, then the oldest reads.
      const round = this.round || (this.round = new Set());
      const age = v => { const at = this.readAt.get(this.key(v)); return at === undefined || !this.popups.has(this.key(v)) ? -1 : at; };
      const todo = vs.filter(v => !this.fresh(v) && !round.has(this.key(v))).sort((x, y) => age(x) - age(y));
      for (let i = 0; i < todo.length; i++) {
        if (!ok()) return;
        if (Date.now() > until || document.hidden || this.userBusy()) { resumed = true; break; }
        // The page changed under the pass (its visuals are gone): stop; the new page is read
        // once it has settled, and this one carries on from here when you come back.
        const v = todo[i];
        if (this.pageNow() !== page || !v.el.isConnected) { left = true; break; }
        // Power BI's own work (drawing, a click of yours) goes first: one visual per idle moment.
        if (i) await new Promise(r => (window.requestIdleCallback ? requestIdleCallback(r, { timeout: 300 }) : setTimeout(r, 0)));
        if (!ok()) return;
        if (this.pageNow() !== page || !v.el.isConnected) { left = true; break; }
        const prevEl = document.querySelector('.displayArea .visualContainer.selected');
        // Stamped with the time the read began, so a change during the read makes it stale.
        const at = perf)PBT"
LR"PBT(ormance.now();
        let cards;
        this.quiet(true);
        try { cards = await this.popup(v); } finally { this.quiet(false); }
        if (!ok()) return;
        // A list read after the page changed would belong to another page's key: dropped.
        if (this.pageNow() !== page) { left = true; break; }
        this.popups.set(this.key(v), cards);
        this.readAt.set(this.key(v), at);
        round.add(this.key(v));
        if (at - this.dirtyAt < DIRTY_SETTLE) early = true;
        // Safety net, narrow on purpose: if reading this visual selected it, undo that one
        // selection; a selection you make meanwhile is left alone.
        if (v.el.classList.contains('selected') && prevEl !== v.el) {
          const prev = prevEl && prevEl.isConnected && vs.find(x => x.el === prevEl);
          if (prev) this.select(prev); else this.deselect();
        }
      }
      // A page change is picked up by the page watcher in start(), which redraws for the new page.
      if (!ok() || this.pageNow() !== page) return;
      this.compose(); this.sig = this.layoutSig(); this.draw();
      if (!resumed) this.round = null;
      if (resumed || left) this.retryPass(document.hidden ? 0 : 400);
      // Lists read too soon after a change are read again once Power BI has applied it.
      if (early) this.later(Math.max(400, this.dirtyAt + DIRTY_PASS - performance.now()));
    });
    return this.passPromise;
  },
  retryPass(ms) {
    if (document.hidden) { this.stale = true; return; }
    clearTimeout(this.retryTimer);
    this.retryTimer = setTimeout(() => { if (this.on) this.refreshPopups(); }, ms);
  },

  // ---- scan the page ----
  // Everything, with nothing selected or clicked on the canvas: the pane, then Power BI's list
  // for each visual out of sight.
  // announce (turning the map on, Rescan) reads every list again; otherwise lists still fresh
  // are kept.
  // A scan whose map is turned off (or stopped) meanwhile ends at its next step.
  async scan(announce) {
    const gen = this.gen, alive = () => gen === this.gen && this.on;
    this.pageName = this.pageNow();
    if (announce) { this.readAt.clear(); this.round = null; }
    await this.idle();
    if (!alive()) return;
    this.refreshPane();
    await this.refreshPopups();
    await this.idle();
    if (!alive()) return;
    if (!this.result) this.compose();
    if (announce && this.result) {
      const rows = this.result.rows;
      const n = rows.filter(r => r.items.some(i => i.level === 'visual')).length;
      const unknown = rows.filter(r => r.unknown).length;
      pbt.toast(`${n} of ${rows.length} visuals carry visual-level filters; hover a badge for the list.${unknown ? ` ${unknown} visual${unknown === 1 ? '' : 's'} not in the project files: select ${unknown === 1 ? 'it' : 'them'} (or use "Read each visual") to see hidden filters.` : ''}`, { title: 'Filter map', ms: 6000 });
    }
  },
  // Without project files: select each visual in turn to read its cards (hidden ones included),
  // then put the selection back. Only on request, from the strip.
  async readEachVisual() {
    await this.exclusive(async gen => {
      const vs = pbt.page.visuals();
      const before = this.selectedIndex();
      const alive = () => gen === this.gen;
      for (const v of vs) {
        if (!alive()) return;
        if (!await this.selectNow(v, alive)) continue;
        if (await fwait(() => v.el.classList.contains('selected') && this.sections().visual, 1200)) this.live.set(this.key(v), this.readCards(this.sections().visual));
      }
      if (!alive()) return;
      if (before >= 0 && vs[before]) await this.selectNow(vs[before], alive); else this.deselect();
      await fsleep(300);
    });
    this.refreshPane();
  },
  layoutSig() { const g = pbt.page.geometry(); return g ? JSON.stringify([g.rect.left, g.rect.top, g.rect.width, pbt.page.visuals(g).map(v => [v.x, v.y, v.w, v.h, v.title])]) : ''; },

  // ---- drawing ----
  clear() { if (this.host) { this.host.remove(); this.host = null; } pbt.railClear('filters'); },
  // Something to report: a visual-level filter, a hidden or locked one, or a slicer or cross-filter.
  // Page and report filters reach every visual on the page alike, so they alone are a pass. A
  // visual whose own filters could not be read is not a pass either.
  reportable(row) { return row.unknown || row.items.some(i => i.level === 'visual' || i.level === 'other' || i.hidden || i.locked); },
  counts(items) { const n = { visual: 0, page: 0, report: 0, other: 0, hidden: 0, locked: 0 }; for (const i of items) { n[i.level]++; if (i.hidden) n.hidden++; if (i.locked) n.locked++; } return n; },
  // Where a filter from another element comes from, as [{ el, name, how }]: the slicers on the
  // page for that field (by the slicer's own header or title). Power BI does not name the visual a
  // cross-filter comes from, so that one is inferred, cautiously: the other visuals that use the
  // filtered field and show a selection right now; only when exactly one does is it named. Zero or
  // several give none: no outline is better than a wrong one. others: the page's visuals without
  // the one described. Runs when a card or the outlines are first wanted, not on every pass.
  sourcesFor(item, others, ctx = {}) {
    const f = String(item.field || '').replace(/^(included|excluded)\b.*$/i, '').trim().toLowerCase() || String(item.text || '').split(/\s+is\s+/i)[0].trim().toLowerCase();
    const slicers = f ? others.filter(v => /slicer/i.test(v.kind) && ((v.title || '').toLowerCase().includes(f) || [...v.el.querySelectorAll('.slicer-header-text, [class*="headerText" i]')].some(h => h.textContent.trim().toLowerCase() === f))) : [];
    if (slicers.length) return slicers.map(v => ({ el: v.el, name: v.title || v.kind, how: 'slicer' }));
    // A cross-filter reads "Included (1): Apparel (Category)": the field is in the last brackets.
    const m = /^(included|excluded)\b/i.test(String(item.field || '')) ? /\(([^()]+)\)\)PBT"
LR"PBT(s*$/.exec(String(item.text || '')) : null;
    const field = m ? m[1].trim().toLowerCase() : f;
    if (!field) return [];
    const hits = others.filter(v => !/slicer/i.test(v.kind) && this.usesField(v, field, ctx) && this.showsSelection(v.el));
    return hits.length === 1 ? [{ el: hits[0].el, name: hits[0].title || hits[0].kind, how: 'click' }] : [];
  },
  // Whether a visual uses a field (its name, lowercased): from its saved visual.json when the
  // project has the page and the visual (the query's fields), else from what it draws (axis and
  // legend titles, or the field's name as a word in its title).
  usesField(v, field, ctx) {
    const saved = this.savedFields(v, ctx);
    if (saved) return saved.has(field);
    const word = new RegExp(`(^|[^a-z0-9])${field.replace(/[.*+?^${}()|[\]\\]/g, '\\$&')}([^a-z0-9]|$)`, 'i');
    if (word.test(v.title || '')) return true;
    return [...v.el.querySelectorAll('.xAxisLabel, .yAxisLabel, .legendTitle, [class*="axisTitle" i], [class*="legendTitle" i]')].some(t => t.textContent.trim().toLowerCase() === field);
  },
  // The fields each saved visual uses, by its visual.json path. The cache belongs to one set of
  // project files (fieldCacheRaw): when the mod hands over new ones (another report opened, or a
  // save), pbt.project.raw is a new object and the cache starts again.
  fieldCache: new Map(),
  fieldCacheRaw: null,
  savedFields(v, ctx) {
    const p = pbt.project, raw = p && p.raw;
    if (!ctx.page || !raw || !Array.isArray(raw.files)) return null;
    if (this.fieldCacheRaw !== raw) { this.fieldCache = new Map(); this.fieldCacheRaw = raw; }
    const hit = p.match(v, ctx.page, ctx.tol);
    if (!hit.file) return null;
    // The full path on the visual's own page, so a visual of the same name on another page is
    // never read in its place.
    const path = `definition/pages/${ctx.page.name}/visuals/${hit.file.name}/visual.json`;
    if (this.fieldCache.has(path)) return this.fieldCache.get(path);
    const file = raw.files.find(x => x.path === path);
    let json = null;
    try { json = file ? JSON.parse(file.text) : null; } catch {}
    if (!json) return null;
    const out = new Set();
    const state = (json.visual && json.visual.query && json.visual.query.queryState) || {};
    for (const role of Object.values(state)) {
      for (const pr of (role && role.projections) || []) {
        const name = /\[([^\]]+)\]$/.exec(p.field(pr.field));
        if (name) out.add(name[1].toLowerCase());
        if (pr.nativeQueryRef) out.add(String(pr.nativeQueryRef).toLowerCase());
        if (pr.displayName) out.add(String(pr.displayName).toLowerCase());
      }
    }
    this.fieldCache.set(path, out);
    return out;
  },
  // A visual with a selection: Power BI dims the data points that are not selected, so among one
  // kind of mark (the same tag and class) some are drawn full and some faded; or a data point
  // carries aria-selected. A kind of mark that is faded throughout (an area's fill) does not count.
  showsSelection(el) {
    if (el.querySelector('svg [aria-selected="true"], [role="option"][aria-selected="true"], [role="gridcell"][aria-selected="true"]')) return true;
    const kinds = new Map();
    const nodes = el.querySelectorAll('svg rect, svg path, svg circle');
    for (let i = 0, seen = 0; i < nodes.length && seen < 400; i++) {
      const n = nodes[i];
      if (n.closest('.axis, [class*="legend" i], [class*="gridline" i], [data-pbt]')) continue;
      const cs = getComputedStyle(n);
      if (!cs.fill || cs.fill === 'none' || cs.display === 'none' || cs.visibility === 'hidden') continue;
      seen++;
      const a = (cs.fillOpacity === '' ? 1 : parseFloat(cs.fillOpacity)) * (cs.opacity === '' ? 1 : parseFloat(cs.opacity));
      const key = `${n.tagName}.${n.getAttribute('class') || ''}`, k = kinds.get(key) || { full: 0, faded: 0 };
      if (a >= 0.9) k.full++; else if (a > 0.05 && a <= 0.6) k.faded++;
      kinds.set(key, k);
    }
    return [...kinds.values()].some(k => k.full && k.faded);
  },
  card(row, sources = new Map()) {
    const el = pbt.el, v = row.v, n = this.counts(row.items);
    const total = row.items.length;
    let pin = 0;
    // Each row carries its level (data-cat), so a pinned card's counters can filter the list.
    const from = i => { const s = (sources.get(i) || []).find(x => x.how === 'click'); return s ? ` \u00b7 from ${s.name} (a click)` : ''; };
    const rows = row.items.map(i => el('div', { class: 'pbt-card-row', dataset: { cat: i.level } },
      i.level === 'other' && (sources.get(i) || []).length ? pbt.pinNum(++pin) : el('span', { class: `pbt-card-dot pbt-dot-f${i.level}` }),
      el('span', { text: `${LEVELS[i.level].label}: ${i.text}${from(i)}` }),
      el('span', { class: 'pbt-card-fix', text: [i.hidden ? 'hidden' : '', i.locked ? 'locked' : ''].filter(Boolean).join(' \u00b7 ') })));
    if (!rows.length && !row.unknown) rows.push(el('div', { class: 'pbt-card-row' }, el('span', { class: 'pbt-card-dot pbt-dot-ok' }), el('span', { text: 'No filters affect this visual.' }), el('span')));
    if (row.unknown) rows.push(el('div', { class: 'pbt-card-row' }, el('span', { class: 'pbt-card-dot pbt-dot-grid' }), el('span', { text: 'Not in the project files, so its own filters (hidden ones included) are unknown: select it, or use "Read each visual" on the strip.' }), el('span')));
    if (row.noButton) rows.push(el('div', { class: 'pbt-card-row' }, el('span', { class: 'pbt-card-dot pbt-dot-grid' }), el('span', { text: 'This visual has no filter list of its own, so slicers and cross-filters are not shown for it.' }), el('span')));
    return el('div', { class: 'pbt-card' },
      el('header', {}, el('b', { text: v.title || v.kind }), v.title ? el('span', { class: 'pbt-card-type', text: v.kind }) : null,
        el('span', { class: 'pbt-card-badge pbt-sev-info', text: total ? `${total} filter${total === 1 ? '' : 's'}` : 'none' })),
      el('div', { class: 'pbt-card-kpi' }, ...[')PBT"
LR"PBT(visual', 'page', 'report', 'other'].map(k => el('div', { dataset: { cat: k } }, el('small', { text: LEVELS[k].label }), el('strong', {}, String(n[k]), k === 'visual' && (n.hidden || n.locked) ? el('i', { text: [n.hidden ? `${n.hidden} hidden` : '', n.locked ? `${n.locked} locked` : ''].filter(Boolean).join(', ') }) : null), el('span', { class: 'pbt-card-kpi-note', text: KPI_NOTES[k] })))),
      el('div', { class: 'pbt-card-rows' }, ...rows),
      el('footer', {}, el('span', {}, el('b', { text: 'Visual' }), ' from the Filters pane'), el('span', {}, el('b', { text: 'Slicers & cross-filters' }), ' from Power BI\'s list for the visual')));
  },
  draw() {
    const g = pbt.page.geometry();
    if (!g || !this.result) { this.clear(); this.drawKey = ''; return; }
    // Redraw only when something shown changed (the pane updates often; a redraw would close an
    // open card and flicker the badges).
    const key = JSON.stringify([g.rect.left, g.rect.top, g.rect.width, pbt.passes(), this.result.page, this.result.report, this.result.rows.map(r => [r.v.x, r.v.y, r.v.w, r.v.h, r.unknown, r.items.map(i => [i.level, i.text, i.hidden, i.locked])])]);
    if (key === this.drawKey && this.host && this.host.isConnected) return;
    this.drawKey = key;
    this.clear();
    const el = pbt.el;
    const host = this.host = el('div', { class: 'pbt-fm', 'data-pbt': '', style: `left:${g.rect.left}px;top:${g.rect.top}px;width:${g.rect.width}px;height:${g.rect.height}px` });
    const live = pbt.page.visuals(g);
    const passes = pbt.passes();
    for (const row of this.result.rows) {
      if (!passes && !this.reportable(row)) continue;
      const v = live.find(x => x.el === row.v.el) || row.v;
      const n = this.counts(row.items);
      // A chip is a letter and a count ("V1"); the badge is grey information like every finding
      // that is not a fault. While the pointer is on the badge (or its card is open) the words
      // ("1 visual") show in a panel raised over the badge: it grows up and to the left from the
      // badge's bottom-right corner, wraps onto lines above within its own visual's width, and
      // leaves the chips in place. The badge itself never changes size on hover, so nothing moves
      // under the pointer and no neighbour's rail is covered.
      const parts = ['visual', 'page', 'report', 'other'].filter(k => n[k]).map(k => [k, `${LEVELS[k].short}${n[k]}`, `${n[k]} ${LEVELS[k].words}`]);
      if (row.unknown) parts.unshift(['unknown', 'V?', 'own filters unknown']);
      if (n.hidden) parts.push(['hidden', `${n.hidden} hidden`, `${n.hidden} hidden`]);
      const chips = parts.map(([k, short]) => el('span', { class: `pbt-fm-chip pbt-fm-${k}`, text: short }));
      const words = parts.length ? el('span', { class: 'pbt-fm-words', 'aria-hidden': 'true' }, ...parts.map(([k, , long]) => el('span', { class: `pbt-fm-chip pbt-fm-${k}`, text: long }))) : null;
      const badge = pbt.tip(el('div', { class: `pbt-fm-badge pbt-sev-info${n.visual ? ' pbt-fm-has' : ''}` },
        ...(chips.length ? chips : [el('span', { class: 'pbt-fm-chip pbt-fm-none', text: 'no filters' })]), words), () => this.card(row, sourcesNow()));
      badge.dataset.fmVisual = row.v.title || row.v.kind;
      pbt.ownBadge(badge, v);
      // Kept beside the items, never on them: the items are plain data (tests serialize them).
      // Worked out when the card or the outlines are wanted, not on every draw, and again for each
      // hover or opening of the card: the visual a cross-filter comes from is read from which
      // visual shows a selection, and that changes with every click on the canvas, so a source
      // kept from an earlier hover would still name the chart clicked before. One reading serves
      // a hover and the card it opens (SOURCES_MS apart at most), so each costs one pass.
      let sources = new Map(), sourcesAt = -Infinity;
      const sourcesNow = () => {
        const now = performance.now();
        if (now - sourcesAt < SOURCES_MS) return sources;
        sourcesAt = now;
        sources = new Map();
        const others = pbt.page.visuals().filter(x => x.el !== v.el);
        const gg = pbt.page.geometry();
        const ctx = { page: pbt.project && pbt.project.get() ? pbt.project.page() : null, tol: gg ? Math.max(0.5, 1 / gg.scale + 0.01) : 0.5 };
        for (const i of row.items) if (i.level === 'other') sources.set(i, this.sourcesFor(i, others, ctx));
        return sources;
      };
      badge.addEventListener('pointerenter', () => {
        // The words panel may reach left as far as its own visual's left edge.
        const br = badge.getBoundingClientRect(), vr = v.el.isConnected ? v.el.getBoundingClientRect() : null;
        if (vr && vr.width) badge.style.setProperty('--pbt-fm-max', `${Math.max(120, Math.round(br.right - vr.left))}px`);
        let n = 0;
        const found = sourcesNow();
        pbt.pinpoint(row.items.filter(i => (found.get(i) || []).length).map(i => ({ els: found.get(i).map(s => s.el), n: ++n })));
      });
      badge.addEventListener('pointerleave', () => pbt.unpinpoint());
      pbt.railPut(badge, v, 'filters');
    }
    host.append(this.strip(g));
    document.body.append(host);
  },
  strip(g) {
    const el = pbt.el, rows = this.result.rows;
    const withV = rows.filter(r => r.items.some(i => i.level === 'visual')).length;
    const hidden = rows.reduce((s, r) => s + r.items.filter(i => i.hidden).length, 0);
    const locked = rows.reduce((s, r) => s + r.items.filter(i => i.locked).length, 0);
    const others = rows.filter(r => r.items.some(i => i.level === 'other')).length;
    const passes = pbt.passItems(rows.filter(r => !this.reportable(r)).length, rows.length, 'No visual-level filter, nothing hidden or locked, and no slicer or cross-filter: only the page and report filters every visual shares.');
    const item = (cls, n, text, tip) => pbt.tip(el('span', { class: 'pbt-strip-item', dataset: { count: cls } }, el('span', { class: `pbt)PBT"
LR"PBT(-card-dot pbt-dot-f${cls}` }), el('b', { text: String(n) }), el('span', { text })), tip);
    const swallow = e => { e.stopPropagation(); e.preventDefault(); };
    const button = (attrs, run, tip) => pbt.tip(el('button', { ...attrs, onpointerdown: swallow, onclick: e => { e.stopPropagation(); run(e); } }), tip);
    const bar = el('div', { class: 'pbt-qa-strip pbt-fm-strip' },
      // Short words so the strip stays on one line in a wide font; each tooltip spells the count out.
      item('visual', withV, 'visual', `${withV} of ${rows.length} visuals with visual filters\nVisuals with at least one visual-level filter that filters something. Field-well cards ("is (All)") are not counted.`),
      item('visual', hidden, 'hidden', `${hidden} visual-level filter${hidden === 1 ? '' : 's'} hidden in view mode\nReport readers do not see them in the Filters pane, and Power BI's own list for the visual leaves them out.`),
      item('visual', locked, 'locked', `${locked} visual-level filter${locked === 1 ? '' : 's'} locked in view mode`),
      item('page', this.result.page, 'page', `${this.result.page} filter${this.result.page === 1 ? '' : 's'} on this page`),
      item('report', this.result.report, 'report', `${this.result.report} report filter${this.result.report === 1 ? '' : 's'}, on all pages`),
      item('other', others, 'slicers', `${others} of ${rows.length} visuals filtered by slicers & cross-filters\nA slicer, a cross-filter or a selection in another visual.`),
      passes.fine,
      el('span', { class: 'pbt-strip-seg' }, passes.sw,
        button({ class: 'pbt-strip-grid', text: 'Rescan' }, () => this.scan(true), 'Rescan\nReads every visual again, out of sight. The map also updates by itself when a filter changes or you click a slicer or a visual.'),
        rows.some(r => r.unknown) ? button({ class: 'pbt-strip-grid', text: 'Read each visual' }, () => this.readEachVisual(), 'Read each visual\nSome visuals are not in the project files, so their hidden filters are unknown. This selects each visual in turn to read its cards, then puts the selection back. Setting the report project folder avoids it.') : null,
        button({ class: 'pbt-strip-grid', text: 'Visual filters \u25be' }, e => this.menu(e.currentTarget), 'Hide, show, lock or unlock visual-level filters\nFor the selected visual, this page or every page.'),
        button({ class: 'pbt-strip-help', text: '?' }, () => { if (pbt.help) { pbt.help.close(); pbt.help.open(); pbt.help.show('filters'); } }, 'What the badges mean\nOpens the help page at Filter map.')));
    pbt.placeStrip(bar, 'filters', g);
    return bar;
  },

  // ---- bulk hide, show, lock, unlock of visual-level filters ----
  // The menu, built like Power BI's own pbi-menu (a compact list, one glyph per item from Power
  // BI's icon font, a soft two-layer shadow), so it reads as part of the Filters pane.
  menu(anchor) {
    document.querySelectorAll('.pbt-fm-menu').forEach(m => m.remove());
    const el = pbt.el;
    const sel = this.selectedIndex() >= 0;
    const groups = [['hide', 'Hide', 'pbi-glyph-hide3'], ['show', 'Show', 'pbi-glyph-view'], ['lock', 'Lock', 'pbi-glyph-lock'], ['unlock', 'Unlock', 'pbi-glyph-unlock']];
    const scopes = [['visual', 'on the selected visual'], ['page', 'on this page'], ['report', 'on every page']];
    const close = () => { m.remove(); document.removeEventListener('pointerdown', outside, true); };
    const outside = e => { if (!m.contains(e.target) && e.target !== anchor && !anchor.contains(e.target)) close(); };
    const items = [];
    const m = el('div', { class: 'pbt-fm-menu', 'data-pbt': '', role: 'menu', 'aria-label': 'Visual-level filters',
      onkeydown: e => {
        const live = items.filter(b => !b.disabled), i = live.indexOf(document.activeElement);
        if (e.key === 'Escape') { e.preventDefault(); e.stopPropagation(); close(); anchor.focus && anchor.focus(); }
        else if (e.key === 'ArrowDown' || e.key === 'ArrowUp') { e.preventDefault(); e.stopPropagation(); const down = e.key === 'ArrowDown'; live[i < 0 ? (down ? 0 : live.length - 1) : (i + (down ? 1 : live.length - 1)) % live.length].focus(); }
      } },
      el('div', { class: 'pbt-fm-menu-head', text: 'Visual-level filters' }),
      ...groups.flatMap(([a, label, glyph], gi) => [
        gi ? el('div', { class: 'pbt-fm-sep', role: 'separator' }) : null,
        ...scopes.map(([s, where]) => {
          const b = el('button', { class: 'pbt-fm-item', role: 'menuitem', dataset: { scope: s, action: a },
            onpointerdown: e => { e.stopPropagation(); e.preventDefault(); },
            onclick: e => { e.stopPropagation(); close(); this.bulk(a, s); } },
          el('span', { class: `glyphicon glyph-small ${glyph} pbt-fm-icon`, 'aria-hidden': 'true' }),
          el('span', { text: `${label} ${where}` }));
          if (s === 'visual' && !sel) b.disabled = true;
          items.push(b);
          return pbt.tip(b, `${label} every visual-level filter ${where}${s === 'visual' && !sel ? '\nSelect a visual first.' : ''}${s !== 'visual' ? '\nSelects each visual in turn and presses its card buttons in the Filters pane; Esc stops it.' : ''}\nField-well cards count too; page and report filters are left alone.`);
        })]));
    // Power BI paints its menus inline from the theme; this one takes the colors of the Filters
    // pane it opens from (the first painted background up from the pane), so it matches Power
    // BI's own look and every skin.
    const paint = start => { for (let e = start; e && e !== document.documentElement; e = e.parentElement) { const cs = getComputedStyle(e); if (cs.backgroundColor && !/rgba\(0, 0, 0, 0\)|transparent/.test(cs.backgroundColor)) return cs; } return null; };
    const pane = paint(document.querySelector('.outspacePane .cards, .outspacePane'));
    if (pane) { m.style.setProperty('--pbt-fm-bg', pane.backgroundColor); m.style.setProperty('--pbt-fm-fg', getComputedStyle(document.querySelector('.outspaceP)PBT"
LR"PBT(ane')).color); }
    document.body.append(m);
    const r = anchor.getBoundingClientRect(), mr = m.getBoundingClientRect();
    m.style.left = `${Math.max(8, Math.min(innerWidth - mr.width - 8, r.right - mr.width))}px`;
    m.style.top = `${r.bottom + 4 + mr.height < innerHeight - 8 ? r.bottom + 4 : Math.max(8, r.top - mr.height - 4)}px`;
    // Focus the menu itself (arrow keys reach the items), so no tooltip opens until you point at one.
    m.tabIndex = -1;
    m.focus();
    setTimeout(() => document.addEventListener('pointerdown', outside, true), 0);
  },
  // Presses the matching card buttons in "Filters on this visual" for the selected visual.
  async applyToSelected(action) {
    const sec = this.sections().visual;
    if (!sec) return 0;
    const want = { hide: c => !c.hidden, show: c => c.hidden, lock: c => !c.locked, unlock: c => c.locked }[action];
    const btn = action === 'hide' || action === 'show' ? '.filterPaneIcon.hide' : '.filterPaneIcon.lock';
    let n = 0;
    for (const c of this.readCards(sec)) {
      if (!want(c)) continue;
      const b = c.el.querySelector(btn);
      if (!b) continue;
      b.click();
      n++;
      await fsleep(120);
    }
    return n;
  },
  async onPage(action, progress, live = () => !this.cancel) {
    let n = 0;
    const vs = pbt.page.visuals();
    for (let i = 0; i < vs.length && live(); i++) {
      if (!await this.selectNow(vs[i], live)) continue;
      if (!await fwait(() => vs[i].el.classList.contains('selected') && this.sections().visual, 1200)) continue;
      if (!live()) break;
      n += await this.applyToSelected(action);
      this.live.set(this.key(vs[i]), this.readCards(this.sections().visual));
      progress(i + 1, vs.length, n);
    }
    return n;
  },
  tabs() { return [...document.querySelectorAll('.carouselScrollPane li.section')].filter(li => li.offsetParent !== null); },
  bulk(action, scope) { return this.exclusive(gen => this.bulkRun(action, scope, gen)); },
  async bulkRun(action, scope, gen) {
    this.cancel = false;
    const verb = { hide: 'Hid', show: 'Showed', lock: 'Locked', unlock: 'Unlocked' }[action];
    const ing = { hide: 'Hiding', show: 'Showing', lock: 'Locking', unlock: 'Unlocking' }[action];
    const stopKey = e => { if (e.key === 'Escape') this.cancel = true; };
    const live = () => !this.cancel && gen === this.gen;
    document.addEventListener('keydown', stopKey, true);
    let n = 0, pages = 0, stuck = '';
    try {
      if (scope === 'visual') { n = await this.applyToSelected(action); pages = 1; const sel = pbt.page.visuals().find(v => v.el.classList.contains('selected')); if (sel && this.sections().visual) this.live.set(this.key(sel), this.readCards(this.sections().visual)); }
      else if (scope === 'page') {
        const before = this.selectedIndex();
        n = await this.onPage(action, (i, of, k) => pbt.toast(`${ing} visual filters: visual ${i} of ${of}, ${k} so far. Esc stops.`, { title: 'Filter map', ms: 1500 }), live);
        const vs = pbt.page.visuals();
        // Your selection goes back after Esc too; only a stop (gen moved on) leaves it.
        if (before >= 0 && vs[before]) await this.selectNow(vs[before], () => gen === this.gen); else if (gen === this.gen) this.deselect();
        pages = 1;
      } else {
        const tabs = this.tabs();
        const start = tabs.findIndex(li => li.classList.contains('selected'));
        for (let p = 0; p < tabs.length && live(); p++) {
          const li = this.tabs()[p];
          if (!li) break;
          if (!li.classList.contains('selected')) {
            (li.querySelector('[role="tab"]') || li).click();
            // A page that does not open in time ends the run: going on would act on the page
            // still showing, which is not the one named in the progress. The tab is looked up again on
            // each check, in case Power BI draws the tabs anew when the page changes.
            if (!await fwait(() => { const t = this.tabs()[p]; return !!t && t.classList.contains('selected'); }, 4000)) { stuck = (li.querySelector('.textLabel') || li).textContent.trim(); break; }
            // Wait for the page's visuals to settle: the same count twice, 400 ms apart.
            let last = -1;
            for (let k = 0; k < 15; k++) { await fsleep(400); const c = pbt.page.visuals().length; if (c && c === last) break; last = c; }
          }
          const name = (li.querySelector('.textLabel') || li).textContent.trim();
          n += await this.onPage(action, (i, of, k) => pbt.toast(`${ing} visual filters: page ${p + 1} of ${tabs.length} (${name}), visual ${i} of ${of}, ${n + k} so far. Esc stops.`, { title: 'Filter map', ms: 1500 }), live);
          this.deselect();
          pages++;
        }
        // A stop (the map turned off, or the mod disabled: gen moved on) leaves the page where it is
        // and says nothing, as the this-page branch does; Esc still goes back to your page.
        const back = gen === this.gen ? this.tabs()[start] : null;
        if (back && !back.classList.contains('selected')) { (back.querySelector('[role="tab"]') || back).click(); await fwait(() => { const t = this.tabs()[start]; return !!t && t.classList.contains('selected'); }, 4000); await fsleep(800); }
      }
      if (gen !== this.gen) return n;
      pbt.toast(`${verb} ${n} visual-level filter${n === 1 ? '' : 's'}${scope === 'report' ? ` on ${pages} page${pages === 1 ? '' : 's'}` : ''}${this.cancel ? ' (stopped)' : stuck ? ` (stopped: page ${stuck} did not open)` : ''}. Save to keep it.`, { title: 'Filter map', ms: 6000, tone: this.cancel || stuck ? 'warn' : 'info' });
      return n;
    } finally {
      document.removeEventListener('keydown', stopKey, true);
      // Hiding or showing a filter changes Power BI's lists (a hidden one leaves them), and the
      // clicks were this tool's own, which the click watcher ignores.
      this.dirty();
      if (this.on) setTimeout(() => this.refreshPane(), 300);
    }
  },

  )PBT"
LR"PBT(// The same menu in the Filters pane, beside the "Filters on this visual" caption (or "Filters
  // on this page" when no visual is selected): a small button that opens it.
  inject() {
    const sec = this.sections();
    const target = sec.visual || sec.page;
    for (const b of document.querySelectorAll('.pbt-fm-pane')) if (!target || !target.header.contains(b)) b.remove();
    if (!target || target.header.querySelector('.pbt-fm-pane')) return;
    // A copy of the header's own "More options" button (button.optionsMenu > span.glyphicon), with
    // its Angular style scope attribute, so it gets Power BI's size, color and alignment; the
    // glyph is the eye Power BI uses on filter cards.
    const more = [...target.header.querySelectorAll('button')].find(x => /more options/i.test(x.getAttribute('aria-label') || ''));
    const b = document.createElement('button');
    const glyph = document.createElement('span');
    if (more) {
      for (const a of more.attributes) if (a.name.startsWith('_ngcontent')) { b.setAttribute(a.name, ''); glyph.setAttribute(a.name, ''); }
    }
    b.className = 'optionsMenu pbt-fm-pane';
    b.setAttribute('data-pbt', '');
    b.setAttribute('aria-label', 'Hide, show, lock or unlock visual-level filters');
    b.setAttribute('aria-haspopup', 'true');
    glyph.className = 'glyphicon pbi-glyph-view glyph-mini';
    b.append(glyph);
    b.addEventListener('pointerdown', e => { e.stopPropagation(); e.preventDefault(); });
    b.addEventListener('click', e => { e.stopPropagation(); this.menu(b); });
    pbt.tip(b, 'Visual-level filters, all at once\nHide, show, lock or unlock them on the selected visual, this page or every page.');
    if (more) more.before(b); else target.header.append(b);
  },

  toggle() {
    this.on = !this.on;
    pbt.restack && pbt.restack();
    if (!this.on) { this.halt(); this.result = null; pbt.toast('Filter map off'); return; }
    this.scan(true);
  },
  // Whether a real click or key can have changed what filters a visual, so that every list read so
  // far goes stale: one on the report canvas (a slicer, a cross-filter, a bookmark or drill
  // button, a click on the empty canvas that clears a cross-filter), one in the Filters pane, one
  // in the Bookmarks pane or its View bar (applying a bookmark changes filters and slicers), and an
  // undo or redo (Ctrl+Z or Ctrl+Y on any keyboard layout, or Power BI's Undo and Redo buttons).
  // A popup (a dropdown
  // slicer's list, a filter card's dropdown, a visual's or a bookmark's menu) counts when it was
  // opened from one of those places: the last click outside any popup said where (fromZone).
  // Everything else changes no filter: the ribbon, the Format, Visualizations, Fields and Selection
  // panes, a save. A change those make to the page
  // or report filters still shows in the Filters pane (notePane), and a save brings new project
  // files (syncRaw); a field dragged into "Filters on this visual" is dropped in the Filters pane.
  // Never counted: synthetic events (this tool's own), its own controls and Ctrl+Alt keys, a
  // modifier key on its own, the page tabs (a page change is read once the new page has settled),
  // and typing in a text field until Enter commits it (the formula bar, a search box, a title).
  // Where those places are, from Power BI's own scripts (desktop.min.js, Power BI Desktop of
  // 2026-10): the Bookmarks pane is the Angular component <bookmarks-pane-content> (its list is
  // <bookmark-tree> of <bookmark-node>), and the bar shown by the pane's View button is
  // <bookmark-play>. The title bar's Undo and Redo are ribbon controls with the ids "undo" and
  // "redo" (TitlebarToolbarFactory, rendered by the ribbon's React toolbar, which passes the
  // control id on as the element id); the Home ribbon's split button is "undoRedoSplit". Seen live
  // on 2026-11-10: button#undo and button#redo inside titlebar-toolbar, with data-unique-id
  // "Ribbon-undo" and "Ribbon-redo". Those are matched first; a button labelled Undo or Redo, or whose tooltip names Ctrl+Z or Ctrl+Y, is
  // the fallback for a build that renders them differently.
  FILTER_ZONES: '.displayAreaViewport, .displayArea, .outspacePane, bookmarks-pane-content, bookmark-tree, bookmark-play',
  UNDO_IDS: '#undo, #redo, [id^="undoRedoSplit"], [data-unique-id="Ribbon-undo"], [data-unique-id="Ribbon-redo"]',
  fromZone: false,
  touchesFilters(e) {
    const t = e && e.target;
    if (!e.isTrusted || !t || !t.closest || t.closest('[data-pbt]') || t.closest('.carouselScrollPane')) return false;
    const zone = n => !!n.closest(this.FILTER_ZONES);
    const inPopup = !!t.closest('.cdk-overlay-container, .ms-Layer');
    if (e.type === 'pointerdown') { if (!inPopup) this.fromZone = zone(t); return false; }
    const typing = () => typingIn((e.composedPath && e.composedPath()[0]) || t);
    // Undo and redo count as the key goes down, so letting go of Ctrl before the Z comes up still
    // counts. The letter is read as the core's hotkeys read it: the Latin letter typed, else the
    // physical key (KeyZ, KeyY), so a Cyrillic or Greek layout, whose Z key types its own letter,
    // counts too, and a French Ctrl+W on the physical Z key does not. In a text field Ctrl+Z undoes
    // the typing, not a filter.
    const k = (e.key || '').toLowerCase();
    const letter = /^[a-z]$/.test(k) ? k : /^Key[A-Z]$/.test(e.code || '') ? e.code.slice(3).toLowerCase() : '';
    const undoKey = !!(e.ctrlKey || e.metaKey) && !e.altKey && (letter === 'z' || letter === 'y');
    if (e.type === 'keydown') return undoKey && !typing();
    if (e.type === 'keyup') {
      if (undoKey) return false;  // counted on its keydown
      if (/^(Control|Alt|Shift|Meta|AltGraph)$/.test(e.key) || (e.ctrlKey && e.altKey && pbt.hotkeys.some(h => h.key === (e.key || '').toLowerCase()))) return false;
      if (typing() && e.key !== 'Enter') return false;
    } else {
      if (t.closest(this.UNDO_IDS)) return true;
    )PBT"
LR"PBT(  const b = t.closest('button, [role="button"], [role="menuitem"]');
      const label = b ? `${b.getAttribute('aria-label') || ''} ${b.getAttribute('title') || ''}`.trim() : '';
      if (/^(undo|redo)\b|ctrl\s*\+\s*[zy]\b/i.test(label)) return true;
    }
    return zone(t) || (inPopup && this.fromZone);
  },
  start() {
    pbt.hotkey('f', 'Filter map', () => this.toggle(), {
      id: 'filters',
      tool: true, active: () => this.on,
      desc: 'A badge on every visual: its visual, page and report filters and what else filters it (slicers, cross-filters). Hover a badge for the list; hide or lock visual filters on a visual, a page or every page.',
      tip: 'Filter map\nReads visual filters from the report project folder and the Filters pane, and slicers and cross-filters from Power BI\'s own list for each visual, out of sight; it updates itself when a filter changes.',
    });
    // Updates by themselves: a change in the Filters pane (a filter added, edited, hidden, a visual
    // selected) refreshes from the pane at once; a real click or key that can change a filter
    // (touchesFilters: a slicer pick, a cross-filter, an undo) refreshes Power BI's lists once
    // Power BI has applied it (dirty). Synthetic events (this tool's own) are ignored.
    this.syncRaw();
    let paneTimer = 0;
    pbt.cleanups.push(() => { clearTimeout(paneTimer); clearTimeout(this.refreshTimer); });
    // The pane is always read (the selected visual's cards are kept even during a pass); the
    // redraw waits for a running pass, which redraws when it ends.
    const paneObserver = new MutationObserver(() => { clearTimeout(paneTimer); paneTimer = setTimeout(() => this.refreshPane(), 250); });
    pbt.cleanups.push(() => paneObserver.disconnect());
    // Even while the map is off, so the lists it has kept are not shown as fresh when it comes back;
    // with the map on, a pass follows once Power BI has applied the change. A pass started while a
    // dropdown or text field of yours is open waits for it.
    const canvasChange = e => { if (this.touchesFilters(e)) this.dirty(); };
    pbt.on(document, 'visibilitychange', () => { if (!document.hidden && this.stale && this.on) { this.stale = false; this.refreshPopups(); } });
    pbt.on(document, 'pointerdown', canvasChange, true);
    pbt.on(document, 'pointerup', canvasChange, true);
    pbt.on(document, 'keydown', canvasChange, true);
    pbt.on(document, 'keyup', canvasChange, true);
    // Keep the overlay on its visuals while the page moves, rescan after a page change, and keep
    // the pane button in place (the pane re-renders its sections often).
    pbt.every(500, () => {
      this.inject();
      this.syncRaw();
      const pane = document.querySelector('.outspacePane');
      if (pane && pane !== this.observed) { paneObserver.disconnect(); paneObserver.observe(pane, { subtree: true, childList: true, characterData: true, attributes: true, attributeFilter: ['aria-label', 'class'] }); this.observed = pane; }
      if (!this.on || this.busy || this.settling) return;
      if (!this.result) return;
      if (this.checkPage()) return;
      const sig = this.layoutSig();
      if (!sig) return;
      if (sig !== this.sig) { this.sig = sig; this.compose(); this.draw(); }
    });
  },
  // A page change seen (by the 500 ms watcher or a pane change): wait for Power BI to finish
  // drawing the new page, then scan it once. The lists are not read inside the page switch, where
  // every hover and popup would make Power BI lay the page out again mid-draw, and nothing is
  // composed or drawn for the half-built page. True when a page change was taken up.
  checkPage() {
    if (!this.on || this.busy || this.settling || !this.result) return false;
    const pageNow = this.pageNow();
    if (pageNow === this.pageName) return false;
    this.pageName = pageNow;
    if (!this.result.rows.length) return false;
    const token = this.settling = {};
    // The old page's badges go now: Power BI may reuse a visual's element for the new page.
    pbt.railClear('filters');
    this.drawKey = '';
    // The wait takes its turn in the queue like a pass, so idle() (and a test waiting for the map)
    // covers it, and the scan follows it without a gap.
    const gen = this.gen;
    const alive = () => this.settling === token && gen === this.gen && this.on && this.pageNow() === pageNow;
    this.exclusive(() => this.settle(alive)).then(() => {
      if (this.settling !== token) return;
      this.settling = null;
      // Changed page again meanwhile: the watcher takes the newer page up on its next tick.
      if (this.on && gen === this.gen && this.pageNow() === pageNow) this.scan(false);
    });
    return true;
  },
  // Resolves once the canvas has had no element added or removed for SETTLE_QUIET ms with the
  // same number of visuals, or after SETTLE_MAX ms (a visual that keeps redrawing never holds
  // the map back longer). Only childList changes count, so an animation does not.
  async settle(alive) {
    const end = performance.now() + SETTLE_MAX;
    let da = null, mo = null, last = performance.now(), count = -1;
    try {
      while (alive() && performance.now() < end) {
        const d = document.querySelector('.displayArea');
        if (d !== da) {
          if (mo) mo.disconnect();
          da = d; mo = null; last = performance.now();
          if (d) { mo = new MutationObserver(() => { last = performance.now(); }); mo.observe(d, { childList: true, subtree: true }); }
        }
        await fsleep(150);
        const c = da ? da.querySelectorAll('.visualContainer').length : -1;
        if (c !== count) { count = c; last = performance.now(); }
        if (c > 0 && performance.now() - last >= SETTLE_QUIET) return;
      }
    } finally { if (mo) mo.disconnect(); }
  },
  // Turning the map off, or stopping, ends running work: the generation changes, queued
  // operations skip, loops return at their next check, bulk runs see cancel, and no tim)PBT"
LR"PBT(er starts
  // another pass.
  halt() {
    this.gen++;
    this.cancel = true;
    this.on = false;
    this.settling = null;
    // A pass queued but skipped never cleared its flag: cleared here, so the next one starts.
    this.passQueued = false;
    this.stale = false;
    clearTimeout(this.retryTimer);
    clearTimeout(this.refreshTimer);
    // Pane readings are compared only while the map is on: one taken before it went off would
    // make the first change seen after it comes back on look like a new one.
    this.paneSigs.clear();
    this.quiet(false);
    this.clear();
  },
  // Stopping (the injector removed or reloaded): everything turning the map off stops, and the
  // pane button and menu go too.
  stop() {
    this.halt();
    document.querySelectorAll('.pbt-fm-pane, .pbt-fm-menu').forEach(e => e.remove());
  },
};
pbt.filters = filtermap;
pbt.module(filtermap);

// ---- 36-interactions.js ----
// Interactions (Ctrl+Alt+I): while Edit interactions is on (Format tab, a visual selected),
// the active interaction icon on every other visual gets a color (None red, Filter blue,
// Highlight green), and a bar above the selected visual sets them all at once.
//
// Probed against Power BI Desktop, 2026-10-30: in Edit interactions each target visual's header
// holds one .visualHeaderItemContainer per choice, whose glyph carries aria-label "Filter",
// "Highlight" (charts) or "None". The active one's glyph class ends in "filled"
// (pbi-glyph-crossfilterfilled, pbi-glyph-disablefilled); the others are outlines in the same
// grey. A plain DOM click on a glyph changes the interaction, like a click of yours.
// The tool is on by default and remembered (features.interactions); it draws nothing outside
// Edit interactions.
//
// A "for all" run presses one icon at a time. While it runs, its button counts the presses
// ("Setting 2 of 4...") and the other buttons wait; at the end the button says what happened for
// a few seconds ("Done: 4 set to None", or how many did not change). Only one run goes at once.
// The Highlight button counts the visuals that offer Highlight ("Highlight 2 of 4") and presses
// only those; while the pointer is on it, those visuals are outlined on the page and the ones that
// cannot be highlighted are dimmed.

const CHOICES = [
  { id: 'none', aria: /^none$/i, label: 'None', verb: 'No interaction', tip: 'The selected visual does not filter or highlight it.' },
  { id: 'filter', aria: /^filter$/i, label: 'Filter', verb: 'Filter', tip: 'The selected visual filters it.' },
  { id: 'highlight', aria: /^highlight$/i, label: 'Highlight', verb: 'Highlight', tip: 'The selected visual highlights its matching part (charts only).' },
];

// Whether a choice's icon can be pressed. Power BI leaves the Highlight icon out on visuals that
// cannot be highlighted (cards, slicers, tables); should it ever show one greyed out instead, the
// usual disabled markers on the icon or on its container also count as "cannot".
const pressable = (...els) => !els.some(e => e && (e.getAttribute('aria-disabled') === 'true' || e.hasAttribute('disabled') || e.classList.contains('disabled')));
const wait = ms => new Promise(r => setTimeout(r, ms));
const plural = (n, word) => `${n} ${word}${n === 1 ? '' : 's'}`;

// Small setters that touch the DOM only when the value really changes, so the bar can be updated
// on every pass without disturbing a button under the pointer.
const setText = (e, v) => { if (e.textContent !== v) e.textContent = v; };
const setAttr = (e, k, v) => {
  if (v == null) { if (e.hasAttribute(k)) e.removeAttribute(k); }
  else if (e.getAttribute(k) !== v) e.setAttribute(k, v);
};
const setTip = (e, v) => { if (e.dataset.pbtTip !== v) pbt.tip(e, v); };
const setStyle = (e, k, v) => { if (e.style[k] !== v) e.style[k] = v; };

const interactions = {
  name: 'interactions',
  host: null,
  ui: null,
  run: null,
  flash: null,
  flashTimer: 0,
  preview: false,
  markSig: '',
  get on() { return pbt.feature('interactions'); },

  // The Edit interactions state of the page: the source (selected) visual and, for every other
  // visual, its choices with the active one. null when Edit interactions is off.
  state() {
    const vs = pbt.page.visuals();
    const targets = [];
    for (const v of vs) {
      const choices = [];
      for (const c of v.el.querySelectorAll('.visualHeaderItemContainer')) {
        const g = c.firstElementChild;
        const aria = g && (g.getAttribute('aria-label') || '');
        const choice = CHOICES.find(x => x.aria.test(aria));
        if (!choice || !/pbi-glyph-(crossfilter|disable|highlight)/.test(String(g.className))) continue;
        choices.push({ id: choice.id, glyph: g, box: c, active: /filled\b/.test(String(g.className)), enabled: pressable(g, c) });
      }
      if (choices.length) targets.push({ v, choices, active: (choices.find(c => c.active) || {}).id || '' });
    }
    if (!targets.length) return null;
    const source = vs.find(v => v.el.classList.contains('selected')) || null;
    return { source, targets };
  },

  // Sets every target that offers the choice to it by clicking its icon, one at a time (so Ctrl+Z
  // undoes them one at a time); returns how many were pressed. A second call while a run is
  // going (a double click, Enter held down, a script) returns 0 at once, so two passes never
  // press the same icons together.
  async setAll(id) {
    if (this.run) return 0;
    const choice = CHOICES.find(c => c.id === id);
    const st = choice ? this.state() : null;
    if (!st) return 0;
    const wants = c => !!c && c.enabled && !c.active;
    const pick = t => t.choices.find(x => x.id === id);
    const offered = st.targets.filter(t => { const c = pick(t); return c && c.enabled; }).length;
    const todo = st.targets.filter(t => wants(pick(t))).map(t => t.v.el);
    const before = st.targets.filter(t => t.active === id).length;
    const sourceEl = st.source && st.source.el;
    const run = this.run = { id, done)PBT"
LR"PBT(: 0, total: todo.length };
    clearTimeout(this.flashTimer);
    this.flash = null;
    // The button keeps at least its width, so the bar does not shrink under the pointer while the
    // label changes to the count and back.
    const btn = this.ui && this.ui.btns[id];
    if (btn) btn.style.minWidth = `${btn.offsetWidth}px`;
    this.draw();
    let pressed = 0, stopped = '';
    try {
      for (const el of todo) {
        // The page is read again before each press: a press can redraw the other visuals'
        // headers, and the run stops if Edit interactions closes, the tool is turned off or
        // another visual is selected. A disable (pbt.remove) ends the run on the spot, with no bar
        // drawn again and no pbt-ia class put back: this copy of the page code is no longer current.
        if (!pbt.current()) return pressed;
        const now = this.on ? this.state() : null;
        if (!this.on) { stopped = 'the Interactions tool was turned off'; break; }
        if (!now) { stopped = 'Edit interactions was closed'; break; }
        if ((now.source && now.source.el) !== sourceEl) { stopped = 'another visual was selected'; break; }
        const t = now.targets.find(x => x.v.el === el);
        const c = t && pick(t);
        if (!wants(c)) continue; // set by hand meanwhile, or no longer on the page
        run.done++;
        this.draw();
        c.glyph.click();
        pressed++;
        await wait(60);
      }
      // Time for Power BI to redraw the icons before they are counted.
      await wait(250);
    } finally {
      this.run = null;
    }
    if (!pbt.current()) return pressed;
    // The result is checked by counting, not by trusting the clicks: every visual that was set
    // before plus every press should now show the choice.
    const after = this.state();
    const now = after ? after.targets.filter(t => t.active === id).length : before;
    const missed = after ? Math.max(0, before + pressed - now) : 0;
    let text, warn = false, detail = '';
    if (stopped) {
      warn = true;
      text = `Stopped after ${pressed} of ${run.total}`;
      detail = `Stopped after ${pressed} of ${run.total} because ${stopped}. Ctrl+Z undoes the ones already set.`;
    } else if (!offered) {
      warn = true;
      text = `No visual offers ${choice.label}`;
    } else if (!run.total) {
      text = `All ${offered} already ${choice.label}`;
    } else if (missed) {
      warn = true;
      text = `${pressed - missed} of ${run.total} set, ${missed} failed`;
      detail = `${plural(missed, 'visual')} did not change to ${choice.label}. Try its icon by hand; Ctrl+Z undoes the ones that did change.`;
    } else {
      text = `Done: ${pressed} set to ${choice.label}`;
    }
    this.flash = { id, text, warn };
    this.flashTimer = setTimeout(() => { this.flash = null; this.draw(); }, warn ? 4500 : 2500);
    // The button itself confirms a good run; a stopped or failed one also gets a toast, which
    // has room for the reason.
    if (detail) pbt.toast(detail, { title: 'Interactions' });
    this.draw();
    return pressed;
  },

  clear() {
    if (this.host) this.host.remove();
    this.host = null; this.ui = null; this.preview = false; this.markSig = '';
  },

  // Builds the bar once. Later passes only update it (render), so a pointer resting on a button
  // keeps its hover look and its tooltip. The bar used to be torn down and rebuilt whenever
  // anything in it changed: on every 400 ms pass while a run changed the counts, and once more
  // after the run, each time under the pointer that had just clicked, which made the buttons
  // blink.
  build() {
    this.clear();
    const el = pbt.el;
    const swallow = e => { e.stopPropagation(); e.preventDefault(); };
    const btns = {}, labels = {};
    for (const c of CHOICES) {
      labels[c.id] = el('span', { class: 'pbt-ia-label' });
      btns[c.id] = el('button', { type: 'button', class: `pbt-ia-btn pbt-ia-${c.id}`, dataset: { set: c.id }, onpointerdown: swallow,
        onclick: e => { e.stopPropagation(); if (!this.run && btns[c.id].getAttribute('aria-disabled') !== 'true') this.setAll(c.id); } },
      el('span', { class: 'pbt-ia-dot' }), labels[c.id]);
    }
    // The Highlight preview follows the pointer or keyboard focus on the Highlight button.
    const preview = on => { if (this.preview !== on) { this.preview = on; this.draw(); } };
    const hb = btns.highlight;
    hb.addEventListener('pointerenter', () => preview(true));
    hb.addEventListener('pointerleave', () => preview(false));
    hb.addEventListener('focus', () => preview(true));
    hb.addEventListener('blur', () => preview(false));
    const count = el('span', { class: 'pbt-ia-count' });
    const marks = el('div', { class: 'pbt-ia-marks' });
    const bar = el('div', { class: 'pbt-ia-bar' }, btns.none, btns.filter, btns.highlight, count);
    // The marks go under the bar, so the bar stays readable over an outlined visual.
    const host = this.host = el('div', { class: 'pbt-ia-host', 'data-pbt': '' }, marks, bar);
    this.ui = { bar, btns, labels, count, marks };
    document.body.append(host);
  },

  // Brings the bar up to date with the page: labels, disabled states, tips, position and the
  // Highlight preview, each written only when it changed.
  render(st, g) {
    const { bar, btns, labels, count, marks } = this.ui;
    const s = st.source, total = st.targets.length;
    const counts = { none: 0, filter: 0, highlight: 0 };
    for (const t of st.targets) if (counts[t.active] !== undefined) counts[t.active]++;
    const offers = id => st.targets.filter(t => t.choices.some(c => c.id === id && c.enabled)).length;
    const hl = offers('highlight');
    const run = this.run, flash = this.flash;
    for (const c of CHOICES) {
      const b = btns[c.id];
      const n = offers(c.id);
      const how = `Clicks each visual's ${c.label} icon, the same as doing it by hand; Ctrl+Z undoes them one at a time.`;
      let label = `${c.lab)PBT"
LR"PBT(el} for all`, off = n === 0;
      let tip = `${c.verb} for every other visual\n${c.tip} ${how}${counts[c.id] === total ? `\nAll ${total} are already set to ${c.label}.` : ''}`;
      if (c.id === 'highlight' && n && n < total) {
        label = `Highlight ${n} of ${total}`;
        tip = `Highlight for ${n} of ${total} visuals\nOnly ${n} of the ${total} visuals offer Highlight; the other ${total - n} (cards and other visuals without highlight) stay as they are. While the pointer is here, the visuals that change are outlined and the ones that cannot are dimmed. ${how}`;
      } else if (c.id === 'highlight' && !n) {
        tip = `Highlight is not available here\nCards and other visuals without highlight cannot be highlighted, and none of the ${total} visuals here offer it (only charts do). Set them to None or Filter instead.`;
      } else if (!n) {
        tip = `${c.verb} is not available here\nNone of the ${total} visuals here offer ${c.label}.`;
      }
      const running = !!run && run.id === c.id;
      const flashing = !run && !!flash && flash.id === c.id;
      if (running) label = `Setting ${run.done} of ${run.total}...`;
      else if (flashing) label = flash.text;
      if (run && !running) { off = true; tip = `${tip.split('\n')[0]}\nWait for the current run to finish.`; }
      if (running) off = false;
      setText(labels[c.id], label);
      setAttr(b, 'aria-disabled', off ? 'true' : null);
      setAttr(b, 'aria-busy', running ? 'true' : null);
      b.classList.toggle('pbt-ia-done', flashing && !flash.warn);
      b.classList.toggle('pbt-ia-warn', flashing && flash.warn);
      if (!running && !flashing) setStyle(b, 'minWidth', '');
      setTip(b, tip);
    }
    setText(count, `${counts.none} of ${total} set to None`);
    setTip(count, `Interactions from ${s.title || s.kind}\nNone ${counts.none} \u00b7 Filter ${counts.filter}${hl ? ` \u00b7 Highlight ${counts.highlight}` : ''} of ${total} visuals; ${hl} of ${total} offer Highlight. The active icon on each visual is colored: None red, Filter blue, Highlight green.`);
    // The host covers the page; the bar sits above the selected visual, inside the page, or just
    // below its top edge when there is no room above.
    const host = this.host;
    setStyle(host, 'left', `${g.rect.left}px`);
    setStyle(host, 'top', `${g.rect.top}px`);
    setStyle(host, 'width', `${g.rect.width}px`);
    setStyle(host, 'height', `${g.rect.height}px`);
    const top = s.y * g.scale - 34;
    setStyle(bar, 'left', `${Math.max(0, s.x * g.scale)}px`);
    setStyle(bar, 'top', `${top >= 0 ? top : s.y * g.scale + 4}px`);
    // The Highlight preview: outlined visuals change, dashed ones are already Highlight, dimmed
    // ones cannot be highlighted. Hidden during a run.
    const list = this.preview && !run ? st.targets.map(t => {
      const c = t.choices.find(x => x.id === 'highlight');
      const kind = !c || !c.enabled ? 'no' : c.active ? 'same' : 'yes';
      return [kind, t.v.x * g.scale, t.v.y * g.scale, t.v.w * g.scale, t.v.h * g.scale];
    }) : [];
    const sig = JSON.stringify(list);
    if (sig !== this.markSig) {
      this.markSig = sig;
      marks.replaceChildren(...list.map(([kind, x, y, w, h]) => pbt.el('div', { class: `pbt-ia-mark pbt-ia-mark-${kind}`, style: `left:${x}px;top:${y}px;width:${w}px;height:${h}px` })));
    }
  },

  draw() {
    const st = this.on ? this.state() : null;
    document.documentElement.classList.toggle('pbt-ia', !!st);
    if (!st || !st.source) { this.clear(); return; }
    const g = pbt.page.geometry();
    if (!g) { this.clear(); return; }
    if (!this.host || !this.host.isConnected || !this.ui) this.build();
    this.render(st, g);
  },

  toggle() {
    pbt.setFeature('interactions', !this.on);
    this.draw();
    pbt.toast(this.on ? 'Interactions on: it shows in Format > Edit interactions' : 'Interactions off', { title: 'Interactions' });
  },
  start() {
    pbt.hotkey('i', 'Interactions', () => this.toggle(), {
      id: 'interactions', note: 'shows in Format > Edit interactions',
      tool: true, active: () => this.on,
      desc: 'In Edit interactions (Format tab): the active icon on every visual is colored (None red, Filter blue, Highlight green), and a bar above the selected visual sets them all to None, Filter or Highlight at once.',
      tip: 'Interactions\nOn by default; it draws only while Edit interactions is on. Each "for all" button clicks the visuals\' own icons, so Ctrl+Z undoes them.',
    });
    pbt.every(400, () => this.draw());
  },
  stop() { clearTimeout(this.flashTimer); this.flash = null; this.clear(); document.documentElement.classList.remove('pbt-ia'); },
};
pbt.interactions = interactions;
pbt.module(interactions);

// ---- 38-stencil.js ----
// Stencil (Ctrl+Alt+S): a see-through mock-up layer over the report page. In edit mode you draw,
// move and resize blocks (card, charts, table, slicer, title, text) on the Layout QA grid, in
// report pixels; they are saved per report and page with the other choices. "Build" writes each
// block as a real visual with no fields into the report's project files (dev injector: the
// pbtBuild binding; the mod: a PowerShell script on the clipboard), so the layout comes back as
// placeholders to drag fields into.
//
// Probed against Power BI Desktop, 2026-10-30: the title bar holds the report name in
// .titleStatusContainer h1.title and always reads "Last saved: ..." (no unsaved marker), so a
// build always asks whether the report is saved: a reload drops unsaved work, and the next save
// would overwrite the new files. Desktop-saved visuals use visualContainer schema 2.5.0, 16-hex
// visual names and 2-space JSON.

const ST_TYPES = [
  { id: 'card', label: 'Card', visual: 'card', glyph: 'pbi-glyph-contactcard', w: 200, h: 120 },
  { id: 'column', label: 'Column chart', visual: 'clusteredColumnChart', glyph: 'pbi-glyph-barchartvertical', w: 400, h: 280 },
  { id: 'bar', label: 'Bar chart', visual: 'clusteredBarChar)PBT"
LR"PBT(t', glyph: 'pbi-glyph-barchart', w: 400, h: 280 },
  { id: 'line', label: 'Line chart', visual: 'lineChart', glyph: 'pbi-glyph-chart', w: 400, h: 280 },
  { id: 'table', label: 'Table', visual: 'tableEx', glyph: 'pbi-glyph-table', w: 400, h: 280 },
  { id: 'slicer', label: 'Slicer', visual: 'slicer', glyph: 'pbi-glyph-checkbox', w: 200, h: 100 },
  { id: 'title', label: 'Title', visual: 'textbox', glyph: 'pbi-glyph-title', w: 600, h: 48, text: { fontSize: '20pt', fontWeight: 'bold' } },
  { id: 'text', label: 'Text', visual: 'textbox', glyph: 'pbi-glyph-inserttextbox', w: 300, h: 80, text: { fontSize: '11pt' } },
];
// The rest of Power BI's own visuals, picked from the "\u2026" menu in the strip. Each is defined the
// way the common types above are, so a build writes it as its own visualType. A type without a
// fitting Power BI icon gets a neutral square (pbt-st-glyph-any, drawn in skins/_base.css). Smart
// narrative has no visualType a build can write safely, so it builds as a text box holding its
// label. group heads the menu section a type is listed under.
const ST_ANY = 'pbt-st-glyph-any';
const ST_MORE = [
  { id: 'stackedBar', group: 'Bar and column', label: 'Stacked bar chart', visual: 'barChart', glyph: 'pbi-glyph-barchart', w: 400, h: 280 },
  { id: 'stackedBar100', group: 'Bar and column', label: '100% stacked bar chart', visual: 'hundredPercentStackedBarChart', glyph: 'pbi-glyph-barchart', w: 400, h: 280 },
  { id: 'stackedColumn', group: 'Bar and column', label: 'Stacked column chart', visual: 'columnChart', glyph: 'pbi-glyph-barchartvertical', w: 400, h: 280 },
  { id: 'stackedColumn100', group: 'Bar and column', label: '100% stacked column chart', visual: 'hundredPercentStackedColumnChart', glyph: 'pbi-glyph-barchartvertical', w: 400, h: 280 },
  { id: 'ribbon', group: 'Bar and column', label: 'Ribbon chart', visual: 'ribbonChart', glyph: 'pbi-glyph-barchartvertical', w: 400, h: 280 },
  { id: 'waterfall', group: 'Bar and column', label: 'Waterfall chart', visual: 'waterfallChart', glyph: 'pbi-glyph-barchartvertical', w: 400, h: 280 },
  { id: 'funnel', group: 'Bar and column', label: 'Funnel', visual: 'funnel', glyph: 'pbi-glyph-barchart', w: 400, h: 280 },
  { id: 'area', group: 'Line and area', label: 'Area chart', visual: 'areaChart', glyph: 'pbi-glyph-chart', w: 400, h: 280 },
  { id: 'stackedArea', group: 'Line and area', label: 'Stacked area chart', visual: 'stackedAreaChart', glyph: 'pbi-glyph-chart', w: 400, h: 280 },
  { id: 'lineStackedColumn', group: 'Line and area', label: 'Line and stacked column chart', visual: 'lineStackedColumnComboChart', glyph: 'pbi-glyph-chart', w: 400, h: 280 },
  { id: 'lineClusteredColumn', group: 'Line and area', label: 'Line and clustered column chart', visual: 'lineClusteredColumnComboChart', glyph: 'pbi-glyph-chart', w: 400, h: 280 },
  { id: 'scatter', group: 'Other charts', label: 'Scatter chart', visual: 'scatterChart', glyph: ST_ANY, w: 400, h: 280 },
  { id: 'pie', group: 'Other charts', label: 'Pie chart', visual: 'pieChart', glyph: ST_ANY, w: 320, h: 280 },
  { id: 'donut', group: 'Other charts', label: 'Donut chart', visual: 'donutChart', glyph: ST_ANY, w: 320, h: 280 },
  { id: 'treemap', group: 'Other charts', label: 'Treemap', visual: 'treemap', glyph: ST_ANY, w: 400, h: 280 },
  { id: 'gauge', group: 'Other charts', label: 'Gauge', visual: 'gauge', glyph: ST_ANY, w: 320, h: 200 },
  { id: 'map', group: 'Maps', label: 'Map', visual: 'map', glyph: ST_ANY, w: 400, h: 280 },
  { id: 'filledMap', group: 'Maps', label: 'Filled map', visual: 'filledMap', glyph: ST_ANY, w: 400, h: 280 },
  { id: 'multiRowCard', group: 'Cards and tables', label: 'Multi-row card', visual: 'multiRowCard', glyph: 'pbi-glyph-contactcard', w: 300, h: 200 },
  { id: 'kpi', group: 'Cards and tables', label: 'KPI', visual: 'kpi', glyph: 'pbi-glyph-contactcard', w: 240, h: 160 },
  { id: 'matrix', group: 'Cards and tables', label: 'Matrix', visual: 'pivotTable', glyph: 'pbi-glyph-table', w: 400, h: 280 },
  { id: 'button', group: 'Elements', label: 'Button', visual: 'actionButton', glyph: ST_ANY, w: 160, h: 48 },
  { id: 'image', group: 'Elements', label: 'Image', visual: 'image', glyph: ST_ANY, w: 200, h: 120 },
  { id: 'shape', group: 'Elements', label: 'Shape', visual: 'basicShape', glyph: ST_ANY, w: 200, h: 120 },
  { id: 'decompositionTree', group: 'AI visuals', label: 'Decomposition tree', visual: 'decompositionTreeVisual', glyph: ST_ANY, w: 600, h: 320 },
  { id: 'keyInfluencers', group: 'AI visuals', label: 'Key influencers', visual: 'keyDriversVisual', glyph: ST_ANY, w: 600, h: 320 },
  { id: 'qna', group: 'AI visuals', label: 'Q&A', visual: 'qnaVisual', glyph: ST_ANY, w: 400, h: 280 },
  { id: 'smartNarrative', group: 'AI visuals', label: 'Smart narrative', visual: 'textbox', glyph: 'pbi-glyph-inserttextbox', w: 400, h: 160, text: { fontSize: '11pt' } },
];
const ST_ALL = [...ST_TYPES, ...ST_MORE];
const ST_TYPE = Object.fromEntries(ST_ALL.map(t => [t.id, t]));
const ST_SCHEMA = 'https://developer.microsoft.com/json-schemas/fabric/item/report/definition/visualContainer/2.5.0/schema.json';
const ST_PAGE_SCHEMA = 'https://developer.microsoft.com/json-schemas/fabric/item/report/definition/page/2.0.0/schema.json';
// The only files a build may write, relative to the .Report folder (the injector and the build
// script check the same rule).
const ST_PATH = /^definition\/pages\/[A-Za-z0-9_-]{1,64}\/(visuals\/[A-Za-z0-9_-]{1,64}\/visual\.json|page\.json)$/;
const ST_MAX = 200;
// How far, in screen pixels, the pointer must travel from the press before a drag draws a block.
const ST_DRAG_PX = 4;
// Converting blocks into real visuals with the build script, in the user's three steps (the
// tooltips and the toast after a click say the same), and what it needs when no report files are
// loaded.
const ST_CONVERT_STEPS = '1. This copies a script. 2. Paste it into PowerShell and press Enter. 3. Close and reopen the report in Power BI to see the visua)PBT"
LR"PBT(ls.';
const ST_NEEDS_PBIP = 'The report must be a .pbip project that Power BI opened; the mod finds its files by itself.';
// Converting blocks into a background of frames (rounded card shapes drawn in one SVG that is the
// page's background image), in the same three steps.
const ST_FRAMES_STEPS = '1. This copies a script. 2. Paste it into PowerShell and press Enter. 3. Close and reopen the report in Power BI to see the frames.';
// The frames' card look when the report theme sets none: white, a light grey 1 px border, 8 px
// corners.
const ST_FRAME_LOOK = { fill: '#FFFFFF', stroke: '#E1E5EA', width: 1, radius: 8 };
// The name a frames image is registered under: StencilFrames_, the page's name, 16 digits.
const ST_FRAMES_ITEM = /^StencilFrames_[A-Za-z0-9_-]{1,96}\.svg$/;
// Starter layouts, drawn for a 1280 x 720 page on an 8 px grid (32 px margins, 16 px gaps, a title
// across the top) and scaled to the page they land on. Each block: [type, label, x, y, w, h].
const ST_TITLE = ['title', 'Page title', 32, 24, 1216, 48];
const ST_PRESETS = [
  { id: 'kpi-two', name: 'KPI row, two charts', desc: 'Four cards across the top, a column chart and a trend side by side.', blocks: [ST_TITLE,
    ['card', 'KPI 1', 32, 88, 292, 120], ['card', 'KPI 2', 340, 88, 292, 120], ['card', 'KPI 3', 648, 88, 292, 120], ['card', 'KPI 4', 956, 88, 292, 120],
    ['column', 'By category', 32, 224, 600, 464], ['line', 'Trend', 648, 224, 600, 464]] },
  { id: 'executive', name: 'Executive summary', desc: 'Three cards, a wide trend and two smaller charts beside it.', blocks: [ST_TITLE,
    ['card', 'KPI 1', 32, 88, 392, 120], ['card', 'KPI 2', 440, 88, 392, 120], ['card', 'KPI 3', 848, 88, 400, 120],
    ['line', 'Trend', 32, 224, 800, 464], ['bar', 'Top items', 848, 224, 400, 224], ['column', 'By segment', 848, 464, 400, 224]] },
  { id: 'sidebar', name: 'Slicer sidebar', desc: 'Three slicers down the left; cards, a chart and a table to the right.', blocks: [ST_TITLE,
    ['slicer', 'Slicer 1', 32, 88, 224, 192], ['slicer', 'Slicer 2', 32, 296, 224, 192], ['slicer', 'Slicer 3', 32, 504, 224, 184],
    ['card', 'KPI 1', 272, 88, 312, 120], ['card', 'KPI 2', 600, 88, 312, 120], ['card', 'KPI 3', 928, 88, 320, 120],
    ['column', 'By category', 272, 224, 480, 464], ['table', 'Details', 768, 224, 480, 464]] },
  { id: 'slicer-bar', name: 'Slicer bar', desc: 'Slicers in a row under the title, then cards and two charts.', blocks: [ST_TITLE,
    ['slicer', 'Slicer 1', 32, 88, 292, 64], ['slicer', 'Slicer 2', 340, 88, 292, 64], ['slicer', 'Slicer 3', 648, 88, 292, 64], ['slicer', 'Slicer 4', 956, 88, 292, 64],
    ['card', 'KPI 1', 32, 168, 292, 112], ['card', 'KPI 2', 340, 168, 292, 112], ['card', 'KPI 3', 648, 168, 292, 112], ['card', 'KPI 4', 956, 168, 292, 112],
    ['column', 'By category', 32, 296, 600, 392], ['line', 'Trend', 648, 296, 600, 392]] },
  { id: 'detail', name: 'Detail table', desc: 'Filters across the top and one large table for the rows behind the numbers.', blocks: [ST_TITLE,
    ['slicer', 'Slicer 1', 32, 88, 392, 64], ['slicer', 'Slicer 2', 440, 88, 392, 64], ['slicer', 'Slicer 3', 848, 88, 400, 64],
    ['table', 'Details', 32, 168, 1216, 520]] },
  { id: 'grid4', name: 'Four-chart grid', desc: 'Two by two: column, trend, bar and a table, all the same size.', blocks: [ST_TITLE,
    ['column', 'By category', 32, 88, 600, 292], ['line', 'Trend', 648, 88, 600, 292], ['bar', 'Top items', 32, 396, 600, 292], ['table', 'Details', 648, 396, 600, 292]] },
  { id: 'hero', name: 'Hero chart, side KPIs', desc: 'One large trend with four cards stacked down the right.', blocks: [ST_TITLE,
    ['line', 'Trend', 32, 88, 880, 600],
    ['card', 'KPI 1', 928, 88, 320, 136], ['card', 'KPI 2', 928, 240, 320, 136], ['card', 'KPI 3', 928, 392, 320, 136], ['card', 'KPI 4', 928, 544, 320, 144]] },
  { id: 'ranking', name: 'Two rankings', desc: 'Two tall bar charts side by side, for top and bottom lists.', blocks: [ST_TITLE,
    ['bar', 'Top items', 32, 88, 600, 600], ['bar', 'Bottom items', 648, 88, 600, 600]] },
  { id: 'trend', name: 'Trend focus', desc: 'Cards, a full-width trend and a short table under it.', blocks: [ST_TITLE,
    ['card', 'KPI 1', 32, 88, 292, 120], ['card', 'KPI 2', 340, 88, 292, 120], ['card', 'KPI 3', 648, 88, 292, 120], ['card', 'KPI 4', 956, 88, 292, 120],
    ['line', 'Trend', 32, 224, 1216, 288], ['table', 'Details', 32, 528, 1216, 160]] },
  { id: 'scorecard', name: 'Scorecard', desc: 'Eight cards in two rows over a table.', blocks: [ST_TITLE,
    ...[0, 1].flatMap(r => [0, 1, 2, 3].map(c => ['card', `KPI ${r * 4 + c + 1}`, 32 + c * 308, 88 + r * 156, 292, 140])),
    ['table', 'Details', 32, 400, 1216, 288]] },
  { id: 'compare', name: 'Side-by-side comparison', desc: 'Two matching columns, each with a heading, a chart and a table.', blocks: [ST_TITLE,
    ['text', 'This year', 32, 88, 600, 48], ['text', 'Last year', 648, 88, 600, 48],
    ['column', 'This year by category', 32, 152, 600, 272], ['column', 'Last year by category', 648, 152, 600, 272],
    ['table', 'This year details', 32, 440, 600, 248], ['table', 'Last year details', 648, 440, 600, 248]] },
  { id: 'notes', name: 'Chart with notes', desc: 'A large chart with a text column for the story beside it.', blocks: [ST_TITLE,
    ['column', 'Main chart', 32, 88, 816, 600], ['text', 'What it shows', 864, 88, 384, 288], ['card', 'Headline number', 864, 392, 384, 136], ['slicer', 'Slicer', 864, 544, 384, 144]] },
];

const stencil = {
  name: 'stencil',
  on: false,
  mode: 'view',
  type: 'card',
  sel: null,
  host: null,
  layer: null,
  sig: '',
  drag: null,
  work: null,
  undo: [],
  redo: [],
  undoKey: '',
  // The block copied or cut with Ctrl+C or Ctrl+X, kept here in memory (never on the system
  // clipboard) so Ctrl+V can paste it on this page or another one.
  clip: null,
  // Every type a block can have: the common ones in the strip, then the "\u2026" menu's.
  types: ST_ALL,

  // ---- pure core (fixtures in scri)PBT"
LR"PBT(pts/tool-check.mjs) ----
  // A value to the nearest line within tol, else to the grid.
  snapTo(v, lines, grid, tol) {
    let best = null;
    for (const l of lines) { const d = Math.abs(l - v); if (d <= tol && (best === null || d < Math.abs(best - v))) best = l; }
    return best !== null ? best : Math.round(v / grid) * grid;
  },
  // A moved span (start, size): whichever edge is nearer a line lands on it, else the start
  // lands on the grid.
  snapSpan(start, size, lines, grid, tol) {
    const near = v => { let best = null; for (const l of lines) { const d = Math.abs(l - v); if (d <= tol && (!best || d < best.d)) best = { l, d }; } return best; };
    const a = near(start), b = near(start + size);
    if (a && (!b || a.d <= b.d)) return a.l;
    if (b) return b.l - size;
    return Math.round(start / grid) * grid;
  },
  // What a Power BI visual type (aria role description or visualType) is as a stencil block.
  kindToType(kind, fallback = 'column') {
    const k = String(kind || '').toLowerCase();
    if (/slicer/.test(k)) return 'slicer';
    if (/card|kpi|multi.?row/.test(k)) return 'card';
    if (/text ?box/.test(k)) return 'text';
    if (/table|matrix|pivot/.test(k)) return 'table';
    if (/line|area/.test(k)) return 'line';
    if (/bar/.test(k)) return 'bar';
    if (/column/.test(k)) return 'column';
    return fallback;
  },
  literal(s) { return { expr: { Literal: { Value: `'${String(s).replace(/'/g, "''")}'` } } }; },
  // The visual.json for one block.
  visualJson(b, name, z, schema) {
    const t = ST_TYPE[b.type] || ST_TYPE.card;
    const visual = { visualType: t.visual };
    if (t.text) visual.objects = { general: [{ properties: { paragraphs: [{ textRuns: [{ value: b.label || t.label, textStyle: { ...t.text } }] }] } }] };
    else visual.visualContainerObjects = { title: [{ properties: { show: { expr: { Literal: { Value: 'true' } } }, text: this.literal(b.label || t.label) } }] };
    visual.drillFilterOtherVisuals = true;
    const r = v => Math.round(v);
    return { $schema: schema || ST_SCHEMA, name, position: { x: r(b.x), y: r(b.y), z, height: r(b.h), width: r(b.w), tabOrder: z }, visual };
  },
  // The files a build writes: { files: [{ path, text }], page, appendPage }. spec: { project
  // (pbt.project.get()), page (an existing page's name) or newPage ({ displayName, width,
  // height }), blocks, schema, rand (0..1, for tests) }.
  files(spec) {
    const { project, blocks } = spec;
    const rand = spec.rand || Math.random;
    const taken = new Set();
    for (const p of project.pages) { taken.add(p.name); for (const v of p.visuals) taken.add(v.name); }
    const hex = n => { for (;;) { let s = ''; for (let i = 0; i < n; i++) s += Math.floor(rand() * 16).toString(16); if (!taken.has(s)) { taken.add(s); return s; } } };
    const out = [];
    let pageName, appendPage = null, z = 0;
    if (spec.newPage) {
      pageName = hex(20);
      appendPage = pageName;
      const names = new Set(project.pages.map(p => p.displayName));
      let display = spec.newPage.displayName, n = 2;
      while (names.has(display)) display = `${spec.newPage.displayName} ${n++}`;
      out.push({ path: `definition/pages/${pageName}/page.json`, text: JSON.stringify({ $schema: ST_PAGE_SCHEMA, name: pageName, displayName: display, displayOption: 'FitToPage', height: spec.newPage.height || 720, width: spec.newPage.width || 1280 }, null, 2) });
    } else {
      const page = project.pages.find(p => p.name === spec.page);
      if (!page) throw new Error(`page ${spec.page} is not in the project files`);
      pageName = page.name;
      z = page.visuals.reduce((m, v) => Math.max(m, v.z || 0), 0);
    }
    // Top to bottom, left to right, so the tab order reads like the page.
    const order = [...blocks].sort((a, b) => a.y - b.y || a.x - b.x);
    for (const b of order) {
      z += 1000;
      const name = hex(16);
      out.push({ path: `definition/pages/${pageName}/visuals/${name}/visual.json`, text: JSON.stringify(this.visualJson(b, name, z, spec.schema), null, 2) });
    }
    for (const f of out) if (!ST_PATH.test(f.path)) throw new Error(`refused path ${f.path}`);
    return { files: out, page: pageName, appendPage };
  },
  // A paste-ready PowerShell script that writes a build, a frames background, or both (Windows
  // PowerShell 5.1 or 7). build: files() with the plan's fields, or null; frames: framesSpec(), or
  // null. Every check runs before anything is written, so a refusal leaves the project as it was.
  script(root, build, frames) {
    // The project folder travels inside the base64 data, never as PowerShell source: a folder name
    // can hold a typographic quote (U+2018 to U+201B), which PowerShell also reads as a quote.
    const payload = { files: build ? build.files : [], appendPage: build ? build.appendPage : null, root };
    if (frames) payload.frames = { page: frames.page, item: frames.item, svg: frames.svg, image: frames.image, transparency: frames.transparency };
    const data = btoa(unescape(encodeURIComponent(JSON.stringify(payload))));
    const visuals = build ? build.files.filter(f => f.path.endsWith('/visual.json')).length : 0;
    const what = [
      build ? `${visuals} placeholder visual${visuals === 1 ? '' : 's'}${build.appendPage ? ' on a new page' : ''}` : '',
      frames ? `${frames.count} frame${frames.count === 1 ? '' : 's'} as the page background` : '',
    ].filter(Boolean).join(' and ');
    const lines = [
      `# Stencil ${build ? 'build' : 'frames'}: ${what} for ${root}`,
      build ? '# Writes only new files under definition\\pages (never one that exists); pages.json is backed up before a new page joins it.' : null,
      frames ? '# Frames: one new SVG under StaticResources\\RegisteredResources, registered in report.json and set as the page background in page.json; both are backed up first.' : null,
      frames && !build ? '# A page that already has a background image is left alone: change the last line from } to })PBT"
LR"PBT( -Replace to replace it.' : null,
      frames && build ? '# A page that already has a background image keeps it: the frames are skipped and the visuals are still built.' : null,
      '# Everything runs as one block, so a refusal stops it all even when pasted line by line into a console.',
      frames && !build ? '& { param([switch]$Replace)' : '& {',
      "$ErrorActionPreference = 'Stop'",
      `$b = ConvertFrom-Json ([Text.Encoding]::UTF8.GetString([Convert]::FromBase64String('${data}')))`,
      '$root = $b.root',
      '$enc = New-Object Text.UTF8Encoding $false',
      `$ok = '${ST_PATH.source}'`,
      "$pj = Join-Path $root 'definition/pages/pages.json'",
      'if (-not (Test-Path -LiteralPath $pj)) { throw "Not a report folder (no definition/pages/pages.json): $root" }',
    ];
    if (build) lines.push(
      '$seen = @{}',
      'foreach ($f in $b.files) {',
      '  if ($f.path -cnotmatch $ok) { throw "Refused: $($f.path)" }',
      '  if ($seen.ContainsKey($f.path)) { throw "Listed twice: $($f.path)" }',
      '  $seen[$f.path] = $true',
      '  $p = Join-Path $root $f.path',
      '  if (Test-Path -LiteralPath $p) { throw "Already exists: $($f.path)" }',
      "  $page = ($f.path -split '/')[2]",
      "  if ($f.path -like '*/page.json') { if (Test-Path -LiteralPath (Split-Path $p)) { throw \"Page folder already exists: $($f.path)\" } }",
      "  elseif ($page -cne $b.appendPage -and -not (Test-Path -LiteralPath (Join-Path $root \"definition/pages/$page/page.json\"))) { throw \"No such page: $page\" }",
      '}',
    );
    if (frames) lines.push(
      // Windows PowerShell 5.1 and 7 both read JSON this way; 7.5 and later would otherwise turn
      // text that looks like a date into a date.
      '$cj = @{}',
      "if ((Get-Command ConvertFrom-Json).Parameters.ContainsKey('DateKind')) { $cj.DateKind = 'String' }",
      'function Set-Prop($o, $n, $v) { if ($o.PSObject.Properties[$n]) { $o.$n = $v } else { $o | Add-Member -NotePropertyName $n -NotePropertyValue $v } }',
      '$fr = $b.frames',
      "if ($fr.page -cnotmatch '^[A-Za-z0-9_-]{1,64}$') { throw \"Refused page: $($fr.page)\" }",
      `if ($fr.item -cnotmatch '${ST_FRAMES_ITEM.source}') { throw "Refused image name: $($fr.item)" }`,
      "$rj = Join-Path $root 'definition/report.json'",
      '$gj = Join-Path $root "definition/pages/$($fr.page)/page.json"',
      '$img = Join-Path $root "StaticResources/RegisteredResources/$($fr.item)"',
      'if (-not (Test-Path -LiteralPath $rj)) { throw "Not a report folder (no definition/report.json): $root" }',
      '$rep = [IO.File]::ReadAllText($rj) | ConvertFrom-Json @cj',
      // A page this script adds has no background yet; an existing one is checked now, before
      // anything is written. With -Replace, the frames image it replaces (one this script made
      // before, by its name) is unregistered and moved into the backups, unless anything else in
      // the report uses it too. Convert to visuals never replaces: on a page with a background
      // image it skips the frames and still builds the visuals.
      '$oldItem = $null',
      '$skipFrames = $false',
      'if ($fr.page -cne $b.appendPage) {',
      '  if (-not (Test-Path -LiteralPath $gj)) { throw "No such page: $($fr.page)" }',
      '  $pg = [IO.File]::ReadAllText($gj) | ConvertFrom-Json @cj',
      '  $had = @($pg.objects.background | Where-Object { $_ -and $_.properties -and $_.properties.image })',
      build ? '  if ($had.Count) { $skipFrames = $true }'
        : '  if ($had.Count -and -not $Replace) { throw "Nothing written: this page already has a background image. To replace it, paste the script again with its last line changed from } to } -Replace" }',
      build ? null : '  if ($had.Count) { $oldItem = $had[0].properties.image.image.url.expr.ResourcePackageItem.ItemName }',
      '}',
      // The same script run again with -Replace writes its image again; otherwise an image of
      // that name is never overwritten.
      'if (-not $skipFrames -and (Test-Path -LiteralPath $img) -and -not ($Replace -and $oldItem -ceq $fr.item)) { throw "Already exists: StaticResources/RegisteredResources/$($fr.item)" }',
    );
    if (build) lines.push(
      'foreach ($f in $b.files) {',
      '  $p = Join-Path $root $f.path',
      '  [void](New-Item -ItemType Directory -Force -Path (Split-Path $p))',
      '  [IO.File]::WriteAllText($p, $f.text, $enc)',
      '}',
    );
    if (build || frames) lines.push(
      "$bk = Join-Path (Split-Path $root) '.stencil-backup'",
      "$stamp = Get-Date -Format 'yyyyMMdd-HHmmss'",
    );
    if (build) lines.push(
      'if ($b.appendPage) {',
      '  [void](New-Item -ItemType Directory -Force -Path $bk)',
      "  Copy-Item -LiteralPath $pj (Join-Path $bk ('pages-' + $stamp + '.json'))",
      '  $m = [IO.File]::ReadAllText($pj) | ConvertFrom-Json',
      '  $m.pageOrder = @($m.pageOrder) + $b.appendPage',
      '  [IO.File]::WriteAllText($pj, ($m | ConvertTo-Json -Depth 10), $enc)',
      '}',
    );
    // The frames: on a page that keeps its own background image (Convert to visuals only), a line
    // says they were skipped; everything else in the script still runs.
    if (frames) lines.push(
      'if ($skipFrames) {',
      '  "Frames skipped: this page already has a background image, so it was left as it is. To put the frames there in its place, use Convert to background and paste its script with the last line changed from } to } -Replace."',
      '} else {',
      '[void](New-Item -ItemType Directory -Force -Path $bk)',
      '$rb = Join-Path $bk (\'report-\' + $stamp + \'.json\')',
      'Copy-Item -LiteralPath $rj $rb',
      "if ($fr.page -cne $b.appendPage) { Copy-Item -LiteralPath $gj (Join-Path $bk ('page-' + $fr.page + '-' + $stamp + '.json')) }",
      // Whether this run creates the image file, so a failed write can remove it again.
      '$imgNew = -not (Test-Path -LiteralPath $img)',
      // The image joins the RegisteredResou)PBT"
LR"PBT(rces package (made when the report has none yet).
      "$pk = @($rep.resourcePackages | Where-Object { $_ -and $_.name -ceq 'RegisteredResources' })[0]",
      'if (-not $pk) {',
      "  $pk = [pscustomobject]@{ name = 'RegisteredResources'; type = 'RegisteredResources'; items = @() }",
      '  Set-Prop $rep resourcePackages (@($rep.resourcePackages | Where-Object { $_ }) + $pk)',
      '}',
      "Set-Prop $pk items (@($pk.items | Where-Object { $_ -and $_.name -cne $fr.item }) + [pscustomobject]@{ name = $fr.item; path = $fr.item; type = 'Image' })",
      // The page background: the image, fully opaque in a background entry this script makes; an
      // entry the page already has keeps its own color and transparency.
      '$pg = [IO.File]::ReadAllText($gj) | ConvertFrom-Json @cj',
      'if (-not $pg.objects) { Set-Prop $pg objects ([pscustomobject]@{}) }',
      '$bg = @($pg.objects.background | Where-Object { $_ })',
      'if ($bg.Count) {',
      '  if (-not $bg[0].properties) { Set-Prop $bg[0] properties ([pscustomobject]@{}) }',
      '  Set-Prop $bg[0].properties image $fr.image',
      // Power BI applies the background transparency to the image too, so a fully transparent entry
      // would hide the frames: it is made opaque (the page color then shows) and the script says so.
      "  $tp = $bg[0].properties.transparency",
      "  $tv = if ($tp -and $tp.expr -and $tp.expr.Literal) { [string]$tp.expr.Literal.Value } else { '' }",
      "  if ($tv -cmatch '^(\\d+(\\.\\d+)?)D$' -and [double]$Matches[1] -gt 90) {",
      "    Set-Prop $bg[0].properties transparency $fr.transparency",
      "    'The page background was fully transparent, which would hide the frames, so its transparency is now 0 and the page color shows (it was see-through before).'",
      '  }',
      '} else {',
      '  Set-Prop $pg.objects background @([pscustomobject]@{ properties = [pscustomobject]@{ image = $fr.image; transparency = $fr.transparency } })',
      '}',
      '$pgText = $pg | ConvertTo-Json -Depth 100',
      // The frames image this one replaces is unregistered and moved into the backups only when
      // nothing else uses it: no other JSON file under definition (other pages, visuals, bookmarks),
      // nothing in report.json beyond its registration (a wallpaper, say), and nothing left on this
      // page. Otherwise it stays registered and on disk, and only this page's background changes.
      '$oldPath = $null',
      `if ($oldItem -and $oldItem -cne $fr.item -and $oldItem -cmatch '${ST_FRAMES_ITEM.source}') {`,
      '  $here = @([IO.Path]::GetFullPath($rj), [IO.Path]::GetFullPath($gj))',
      "  $others = @(Get-ChildItem -LiteralPath (Join-Path $root 'definition') -Recurse -File -Filter *.json | Where-Object { $here -notcontains $_.FullName } | Select-String -SimpleMatch -List -Pattern $oldItem)",
      '  $repRest = $rep | Select-Object -Property * -ExcludeProperty resourcePackages | ConvertTo-Json -Depth 100',
      '  if (-not $others.Count -and -not $repRest.Contains($oldItem) -and -not $pgText.Contains($oldItem)) {',
      '    $pk.items = @($pk.items | Where-Object { $_.name -cne $oldItem })',
      '    $oldPath = Join-Path $root "StaticResources/RegisteredResources/$oldItem"',
      '  } else { "The old frames image $oldItem is used elsewhere in the report, so it stays registered and in place." }',
      '}',
      // Both JSON files are written to a temporary file first and then moved into place, so a
      // failed write leaves the original. When page.json cannot be moved in after report.json was,
      // report.json comes back from its backup. The old image moves only after both are in place.
      "$tr = $rj + '.stencil-tmp'",
      "$tg = $gj + '.stencil-tmp'",
      '$repDone = $false',
      'try {',
      '  [void](New-Item -ItemType Directory -Force -Path (Split-Path $img))',
      '  [IO.File]::WriteAllText($img, $fr.svg, $enc)',
      '  [IO.File]::WriteAllText($tr, ($rep | ConvertTo-Json -Depth 100), $enc)',
      '  [IO.File]::WriteAllText($tg, $pgText, $enc)',
      '  Move-Item -LiteralPath $tr -Destination $rj -Force',
      '  $repDone = $true',
      '  Move-Item -LiteralPath $tg -Destination $gj -Force',
      '} catch {',
      '  if ($repDone) { Copy-Item -LiteralPath $rb $rj -Force }',
      '  if ($imgNew -and (Test-Path -LiteralPath $img)) { Remove-Item -LiteralPath $img -Force }',
      '  throw',
      '} finally {',
      '  foreach ($t in @($tr, $tg)) { if (Test-Path -LiteralPath $t -PathType Leaf) { Remove-Item -LiteralPath $t -Force } }',
      '}',
      "if ($oldPath -and (Test-Path -LiteralPath $oldPath)) { Move-Item -LiteralPath $oldPath (Join-Path $bk ($stamp + '-' + $oldItem)) }",
      '}',
    );
    lines.push(
      build && frames ? 'if ($skipFrames) { "Built $($b.files.Count) file(s) under $root. In Power BI, click Apply external changes in the bar at the top (or reopen the report) to see them." }'
        + ' else { "Built $($b.files.Count) file(s) and the page\'s frames background under $root (backups in $bk). Close and reopen the report in Power BI to see them." }'
        : frames ? '"Frames background written under $root (backups in $bk). Close and reopen the report in Power BI to see it."'
          : '"Built $($b.files.Count) file(s) under $root. In Power BI, click Apply external changes in the bar at the top (or reopen the report) to see them."',
      '}',
      '',
    );
    return lines.filter(l => l !== null).join('\r\n');
  },
  // The stencil as an SVG at report size (blocks as labelled boxes), for Figma and the like.
  svg(blocks, w, h) {
    const esc = s => String(s).replace(/[&<>"]/g, c => ({ '&': '&amp;', '<': '&lt;', '>': '&gt;', '"': '&quot;' }[c]));
    const rects = blocks.map(b => `  <g><rect x="${b.x}" y="${b.y}" width="${b.w}" height="${b.h}" rx="4" fill="#e8f1fb" stroke="#4a7fc1" stroke-dasharray="6 4"/><text x="${b.x + 8}" y="${b.y + 18}" font-family="Segoe UI, sans-serif" font-size="12" fill=")PBT"
LR"PBT(#1f3b5c">${esc(b.label || ST_TYPE[b.type].label)}</text><text x="${b.x + 8}" y="${b.y + 34}" font-family="Segoe UI, sans-serif" font-size="10" fill="#5b7390">${esc(ST_TYPE[b.type].label)} \u00b7 ${b.w} \u00d7 ${b.h}</text></g>`);
    return `<svg xmlns="http://www.w3.org/2000/svg" width="${w}" height="${h}" viewBox="0 0 ${w} ${h}">\n  <rect width="${w}" height="${h}" fill="#ffffff"/>\n${rects.join('\n')}\n</svg>\n`;
  },

  // Frames: a background image of card shapes behind the blocks.
  // The rectangles to frame: one per group, exactly the group's outline as the stencil draws it,
  // and one per block in no group. A group's frame is not padded, so it never runs into the gap
  // to a neighbour and the spacing stays what the blocks say; groups come first so a larger frame
  // sits under smaller ones (pure, for fixtures).
  frameRects(list) {
    const r = v => Math.round(v);
    const groups = this.groupBoxes(list).map(b => ({ x: r(b.x), y: r(b.y), w: r(b.w), h: r(b.h) }));
    const singles = list.filter(b => !b.group).map(b => ({ x: r(b.x), y: r(b.y), w: r(b.w), h: r(b.h) }));
    return [...groups, ...singles].filter(f => f.w > 0 && f.h > 0);
  },
  // The card look for frames from the report theme's visual style for every visual type: its
  // background color, border color, width and corner radius, each where the theme sets it, else
  // the default look (pure, for fixtures). A border the theme hides is still drawn in its color,
  // since a frame needs an edge to be seen against the page.
  frameLook(theme) {
    const vs = theme && theme.visualStyles && theme.visualStyles['*'] && theme.visualStyles['*']['*'];
    const flat = vs ? pbt.project.flat(vs, true) : {};
    const hex = v => (typeof v === 'string' && /^#([0-9a-f]{3}|[0-9a-f]{6}|[0-9a-f]{8})$/i.test(v) ? v.toUpperCase() : null);
    const num = (v, min, max) => (typeof v === 'number' && Number.isFinite(v) && v >= min && v <= max ? v : null);
    const fill = hex(flat['background.color']), stroke = hex(flat['border.color']);
    const width = num(flat['border.width'], 1, 10), radius = num(flat['border.radius'], 0, 100);
    return {
      fill: fill || ST_FRAME_LOOK.fill,
      stroke: stroke || ST_FRAME_LOOK.stroke,
      width: width !== null ? width : ST_FRAME_LOOK.width,
      radius: radius !== null ? radius : ST_FRAME_LOOK.radius,
      fromTheme: !!(fill || stroke),
    };
  },
  // The frames as one SVG at the page's size, to map 1:1 onto it: each frame a rounded rectangle
  // with its border inside its edges, over a soft shadow drawn by an SVG filter, on a transparent
  // page so the page's own color shows between the frames (pure, for fixtures).
  framesSvg(rects, w, h, look) {
    const sw = look.width, half = sw / 2;
    const shapes = rects.map(f => `    <rect x="${f.x + half}" y="${f.y + half}" width="${Math.max(0, f.w - sw)}" height="${Math.max(0, f.h - sw)}" rx="${look.radius}" ry="${look.radius}"/>`);
    return [
      `<svg xmlns="http://www.w3.org/2000/svg" width="${w}" height="${h}" viewBox="0 0 ${w} ${h}">`,
      '  <defs>',
      `    <filter id="pbt-frame-shadow" filterUnits="userSpaceOnUse" x="0" y="0" width="${w}" height="${h}" color-interpolation-filters="sRGB">`,
      '      <feDropShadow dx="0" dy="1" stdDeviation="1" flood-color="#000000" flood-opacity="0.08"/>',
      '      <feDropShadow dx="0" dy="2" stdDeviation="3" flood-color="#000000" flood-opacity="0.08"/>',
      '    </filter>',
      '  </defs>',
      `  <g fill="${look.fill}" stroke="${look.stroke}" stroke-width="${sw}" filter="url(#pbt-frame-shadow)">`,
      ...shapes,
      '  </g>',
      '</svg>',
      '',
    ].join('\n');
  },
  // What the script needs to make one page's frames background: { page, item, svg, image,
  // transparency }. image and transparency are the page.json background properties, in the shape
  // Power BI Desktop saves for a page background image (scaled to fit, so the SVG at page size
  // maps 1:1). spec: { page (its name), display, width, height, blocks, theme, rand } (pure).
  framesSpec(spec) {
    const rand = spec.rand || Math.random;
    let digits = '';
    for (let i = 0; i < 16; i++) digits += Math.floor(rand() * 10);
    const item = `StencilFrames_${spec.page}${digits}.svg`;
    if (!ST_FRAMES_ITEM.test(item) || !/^[A-Za-z0-9_-]{1,64}$/.test(spec.page)) throw new Error(`refused page ${spec.page}`);
    const w = Math.round(spec.width || 1280), h = Math.round(spec.height || 720);
    const rects = this.frameRects(spec.blocks);
    const lit = v => ({ expr: { Literal: { Value: v } } });
    return {
      page: spec.page, item, count: rects.length,
      svg: this.framesSvg(rects, w, h, this.frameLook(spec.theme)),
      image: {
        image: {
          name: lit(`'Stencil frames (${String(spec.display || spec.page).replace(/'/g, "''")}).svg'`),
          url: { expr: { ResourcePackageItem: { PackageName: 'RegisteredResources', PackageType: 1, ItemName: item } } },
          scaling: lit("'Fit'"),
        },
      },
      transparency: lit('0D'),
    };
  },

  presets: ST_PRESETS,
  // A layout's blocks on a page of w x h: scaled from 1280 x 720, every edge on the grid, inside the
  // page (pure, for fixtures).
  presetBlocks(preset, w, h, grid, newId = (i => `p${i}`)) {
    const sx = w / 1280, sy = h / 720, g = Math.max(1, grid || 1);
    const snap = v => Math.round(v / g) * g;
    // A saved template's row can carry a seventh item, the block's group.
    return preset.blocks.map(([type, label, x, y, bw, bh, group], i) => {
      // The start is clamped inside the page before it snaps (leaving room for one grid step), then
      // the end is at least one grid step further and inside the page, on both axes.
      const edge = (v, size, span) => {
        const a = Math.min(Math.max(0, snap(Math.min(Math.max(0, v), Math.max(0, span - g)))), Math.max(0, span - g));
        return [a, Math.min(span, Math.max(a + g, snap(v + size)))];
      };
      const [x0, x1] = edge(x * sx, )PBT"
LR"PBT(bw * sx, w), [y0, y1] = edge(y * sy, bh * sy, h);
      const b = { id: newId(i), type, label, x: x0, y: y0, w: x1 - x0, h: y1 - y0 };
      if (group) b.group = group;
      return b;
    });
  },
  // A page's blocks as a template's rows, page independent: each position and size as a fraction
  // of the page, kept on the 1280 x 720 basis the starter layouts use, so presetBlocks scales a
  // template onto any page exactly as it scales a layout (pure, for fixtures).
  templateBlocks(list, w, h) {
    const r = v => Math.round(v * 100) / 100;
    return list.map(b => {
      const row = [b.type, b.label || '', r(b.x * 1280 / w), r(b.y * 720 / h), r(b.w * 1280 / w), r(b.h * 720 / h)];
      if (b.group) row.push(b.group);
      return row;
    });
  },
  // The saved templates: pbt.cfg.stencilTemplates, a list of { id, name, blocks } with blocks in
  // the layouts' row form. Anything malformed is left out, and rows of a type this version does
  // not know are dropped.
  templates() {
    const all = Array.isArray(pbt.cfg.stencilTemplates) ? pbt.cfg.stencilTemplates : [];
    return all.filter(t => t && typeof t.id === 'string' && typeof t.name === 'string' && Array.isArray(t.blocks))
      .map(t => ({ id: t.id, name: t.name, blocks: t.blocks.filter(row => Array.isArray(row) && ST_TYPE[row[0]] && row.slice(2, 6).every(Number.isFinite)) }))
      .filter(t => t.blocks.length);
  },
  storeTemplates(list) {
    pbt.cfg.stencilTemplates = list;
    pbt.save();
  },
  // The stored list as it is, every entry kept. Saving, renaming and deleting work on this list and
  // change only the template they target, so an entry templates() leaves out (written by a newer
  // version, say) is never lost; the filtered list is for showing and applying only.
  rawTemplates() {
    return Array.isArray(pbt.cfg.stencilTemplates) ? [...pbt.cfg.stencilTemplates] : [];
  },
  // The stored template (other than exceptId) already named name in any case, or null.
  templateNamed(name, exceptId) {
    const n = String(name || '').trim().toLowerCase();
    return this.rawTemplates().find(t => t && typeof t.name === 'string' && t.id !== exceptId && t.name.trim().toLowerCase() === n) || null;
  },
  // Saves this page's blocks as a template named name; a template of the same name (any case) is
  // replaced and keeps its place in the list. Returns the template, or null with nothing saved.
  saveTemplate(name) {
    const g = this.g || this.geometry(), list = this.blocks();
    name = String(name || '').trim().slice(0, 80);
    if (!g || !list.length || !name) return null;
    const all = this.rawTemplates();
    const same = this.templateNamed(name), i = same ? all.indexOf(same) : -1;
    const t = { id: same && typeof same.id === 'string' ? same.id : `u${Date.now().toString(36)}${Math.floor(Math.random() * 1e4).toString(36)}`, name, blocks: this.templateBlocks(list, g.w, g.h) };
    if (i >= 0) all[i] = t; else all.push(t);
    this.storeTemplates(all);
    pbt.toast(`${name}: ${list.length} block${list.length === 1 ? '' : 's'} saved as a template${i >= 0 ? ', in place of the one of that name' : ''}. Build \u25be > Start from a layout puts it on any page.`, { title: 'Stencil', ms: 5000 });
    return t;
  },
  // Renames a template; false when there is no such template, the name is empty, or another
  // template already has the name (any case).
  renameTemplate(id, name) {
    name = String(name || '').trim().slice(0, 80);
    const all = this.rawTemplates(), i = all.findIndex(x => x && x.id === id);
    if (i < 0 || !name || this.templateNamed(name, id)) return false;
    all[i] = { ...all[i], name };
    this.storeTemplates(all);
    return true;
  },
  deleteTemplate(id) {
    const all = this.rawTemplates();
    if (!all.some(t => t && t.id === id)) return false;
    this.storeTemplates(all.filter(t => !(t && t.id === id)));
    return true;
  },
  // Built-in layouts first, then the saved templates, each kept when its name holds the search
  // text (any case).
  filterLayouts(query, custom) {
    const q = String(query || '').trim().toLowerCase();
    const hit = p => !q || p.name.toLowerCase().includes(q);
    return { builtIn: ST_PRESETS.filter(hit), custom: custom.filter(hit) };
  },

  // ---- groups: an optional group name on a block, from the saved visuals' groups ----
  // The saved visuals of a page with their place on the page and the outermost group each sits in:
  // [{ name, type, x, y, w, h, group }], group '' when ungrouped. A visual in a group is saved
  // relative to its group (parentGroupName), so the groups' offsets are added up; groupNames maps
  // a group visual's name to the name shown. A visual hidden in the saved files (its name, or a
  // group it sits in, in hidden) is not on the page and is left out, so a visual stacked over a
  // hidden twin (a bookmark's other state) takes its own group (pure, for fixtures).
  savedPlaces(page, groupNames = {}, hidden = new Set()) {
    const vs = (page && page.visuals) || [];
    const by = new Map(vs.map(v => [v.name, v]));
    const out = [];
    for (const v of vs) {
      if (v.type === 'group' || hidden.has(v.name)) continue;
      // Saved positions are relative to the group (rx, ry from page/05-project.js; x, y in a
      // fixture page that has no rx), summed up through the groups.
      const sx = q => (q.rx !== undefined ? q.rx : q.x), sy = q => (q.ry !== undefined ? q.ry : q.y);
      let x = sx(v), y = sy(v), top = null, p = by.get(v.parent), off = false;
      const seen = new Set();
      while (p && !seen.has(p.name)) { seen.add(p.name); if (hidden.has(p.name)) off = true; x += sx(p); y += sy(p); top = p; p = by.get(p.parent); }
      if (!off) out.push({ name: v.name, type: v.type, x, y, w: v.w, h: v.h, group: top ? (groupNames[top.name] || top.name) : '' });
    }
    return out;
  },
  // The names of a page's visuals and groups saved hidden (isHidden in their visual.json).
  hiddenNames(page, files) {
    const out = new )PBT"
LR"PBT(Set(), pre = `definition/pages/${(page && page.name) || ''}/visuals/`;
    for (const f of files || []) {
      if (!f.path.startsWith(pre) || !f.path.endsWith('/visual.json') || !/"isHidden"\s*:\s*true/.test(f.text)) continue;
      try { const v = JSON.parse(f.text); if (v.isHidden === true) out.add(v.name || f.path.slice(pre.length).split('/')[0]); } catch {}
    }
    return out;
  },
  // Each outermost group on a page by its visual's name, as the name the stencil shows: its
  // display name in the saved files (visualGroup.displayName), else "Group"; two groups of the
  // same name are told apart with a number (pure, for fixtures).
  groupNames(page, files) {
    const vs = (page && page.visuals) || [], out = {}, used = new Set();
    for (const v of vs) {
      if (v.type !== 'group' || (v.parent && vs.some(p => p.name === v.parent))) continue;
      let display = '';
      const f = (files || []).find(x => x.path === `definition/pages/${page.name}/visuals/${v.name}/visual.json`);
      try { display = String(((JSON.parse(f.text) || {}).visualGroup || {}).displayName || ''); } catch {}
      const base = display.trim() || 'Group';
      let n = base, k = 2;
      while (used.has(n)) n = `${base} ${k++}`;
      used.add(n);
      out[v.name] = n;
    }
    return out;
  },
  // The group a visual on the page (pbt.page.visuals()) sits in, from the saved places: the saved
  // visual at the same place (tol report px), of the same type when more than one is there.
  groupAt(v, places, tol = 1) {
    const at = places.filter(a => Math.abs(a.x - v.x) <= tol && Math.abs(a.y - v.y) <= tol && Math.abs(a.w - v.w) <= tol && Math.abs(a.h - v.h) <= tol);
    const typed = at.filter(a => this.typeMatchKey(a.type) === this.typeMatchKey(v.kind) || this.typeMatchKey(a.type) === this.typeMatchKey(v.typeName));
    const hit = typed.length ? typed[0] : at.length === 1 ? at[0] : null;
    return hit ? hit.group : '';
  },
  // Each group's box around its blocks: [{ name, x, y, w, h }] (pure, for fixtures).
  groupBoxes(list) {
    const boxes = new Map();
    for (const b of list) {
      if (!b.group) continue;
      const r = boxes.get(b.group);
      if (!r) { boxes.set(b.group, { name: b.group, x: b.x, y: b.y, r: b.x + b.w, b: b.y + b.h }); continue; }
      r.x = Math.min(r.x, b.x); r.y = Math.min(r.y, b.y); r.r = Math.max(r.r, b.x + b.w); r.b = Math.max(r.b, b.y + b.h);
    }
    return [...boxes.values()].map(r => ({ name: r.name, x: r.x, y: r.y, w: r.r - r.x, h: r.b - r.y }));
  },
  // A small picture of a layout for the menu.
  thumb(preset) {
    const ns = 'http://www.w3.org/2000/svg';
    const svg = document.createElementNS(ns, 'svg');
    svg.setAttribute('viewBox', '0 0 1280 720');
    svg.setAttribute('class', 'pbt-st-thumb');
    svg.setAttribute('aria-hidden', 'true');
    const page = document.createElementNS(ns, 'rect');
    for (const [k, v] of Object.entries({ width: 1280, height: 720, class: 'pbt-st-thumb-page' })) page.setAttribute(k, v);
    svg.append(page);
    for (const [type, , x, y, w, h] of preset.blocks) {
      const r = document.createElementNS(ns, 'rect');
      for (const [k, v] of Object.entries({ x: x + 6, y: y + 6, width: Math.max(4, w - 12), height: Math.max(4, h - 12), rx: 10, class: `pbt-st-thumb-${type}` })) r.setAttribute(k, v);
      svg.append(r);
    }
    return svg;
  },
  // A starter layout or a saved template (by id) in place of this page's blocks, as one undo step.
  applyPreset(id) {
    const preset = ST_PRESETS.find(p => p.id === id) || this.templates().find(t => t.id === id), g = this.g;
    if (!preset || !g) return;
    const n = Date.now().toString(36);
    const list = this.presetBlocks(preset, g.w, g.h, this.grid(), i => `l${n}${i}`);
    const had = this.blocks().length;
    this.sel = null;
    this.commit(list);
    if (this.mode !== 'edit') this.setMode('edit');
    pbt.toast(`${preset.name}: ${list.length} blocks${had ? `, in place of the ${had} there were` : ''}. Rename and resize them, then build; Ctrl+Z undoes the layout.`, { title: 'Stencil', ms: 6000 });
  },

  // ---- where the blocks live: pbt.cfg.stencil[report][page] ----
  reportName() { const t = document.querySelector('.titleStatusContainer h1.title'); return t ? t.textContent.trim() : ''; },
  // The project files, only when they belong to this window's report: the title bar's name and the
  // .Report folder's name agree. Another report open beside it must never build into them.
  project() {
    const p = pbt.project && pbt.project.get();
    if (!p || !p.root) return null;
    const norm = s => String(s).toLowerCase().replace(/[^a-z0-9]/g, '');
    // Either separator: the setting is typed by hand, and Explorer copies paths with backslashes.
    const folder = p.root.split(/[\\/]/).pop().replace(/\.Report$/i, '');
    const title = this.reportName();
    return !title || norm(title) === norm(folder) ? p : null;
  },
  reportKey() {
    const p = this.project();
    if (p) { this.adoptBlocks(p.root); return p.root; }
    return this.reportName() || 'report';
  },
  // The mod finds the project folder by itself, so the key can appear or change while the report
  // is open: a first save as .pbip (the blocks were drawn under the report's name), a Save As to
  // another folder, or a path that differs only in case from a hand-typed setting. When the
  // current root holds no blocks, the blocks this report has under another key come to it, once,
  // saved as a commit saves them: a root that is the same path in another case, else the report's
  // name, moves; else a root with the same .Report folder name, when exactly one holds blocks, is
  // copied, not moved. A folder of the same name elsewhere can be another report (a copy of this
  // one, or the scratch copy scripts/tool-check.mjs builds into), and its blocks must stay where
  // they are. Two sets that both hold blocks are never merged, and the generic 'report' key (no
  // title) is never taken, since it could)PBT"
LR"PBT( be any report's.
  adoptBlocks(root) {
    const all = pbt.cfg.stencil || {};
    // Checked again only when the stored blocks or the root change; reportKey runs on every draw.
    if (this.adoptSeen === all && this.adoptRoot === root) return;
    this.adoptSeen = all; this.adoptRoot = root;
    // A root takes blocks once per session, so clearing every block it holds does not bring a
    // copied set straight back.
    this.adopted = this.adopted || new Set();
    if (this.adopted.has(root)) return;
    const has = k => !!all[k] && typeof all[k] === 'object' && Object.keys(all[k]).length > 0;
    if (has(root)) return;
    const isPath = s => /[\\/]/.test(s);
    const path = s => String(s).replace(/\\/g, '/').replace(/\/+$/, '').toLowerCase();
    const folder = s => (String(s).split(/[\\/]/).filter(Boolean).pop() || '').replace(/\.Report$/i, '').toLowerCase().replace(/[^a-z0-9]/g, '');
    const keys = Object.keys(all).filter(k => k !== root && k !== 'report' && has(k));
    const name = this.reportName();
    let from = keys.find(k => isPath(k) && path(k) === path(root)) || null, move = true;
    if (!from && name && !isPath(name) && keys.includes(name)) from = name;
    if (!from && folder(root)) {
      const same = keys.filter(k => isPath(k) && folder(k) === folder(root));
      if (same.length === 1) { from = same[0]; move = false; }
    }
    if (!from) return;
    const next = { ...all, [root]: JSON.parse(JSON.stringify(all[from])) };
    if (move) delete next[from];
    pbt.cfg.stencil = next;
    this.adoptSeen = next;
    this.adopted.add(root);
    pbt.save();
  },
  tabName() {
    const li = document.querySelector('.carouselScrollPane li.section.selected');
    const t = li && (li.querySelector('[class*="title" i], [class*="label" i], span') || li);
    return t ? t.textContent.trim() : '';
  },
  projectPage() { return this.project() ? pbt.project.page() : null; },
  pageKey() { const p = this.projectPage(); return p ? p.name : this.tabName(); },
  // A page drawn on before the project files knew it sits under its tab name; once they do, the
  // blocks are found there still and move to the page's name on the next change.
  blocks() {
    if (this.work) return this.work;
    const rep = (pbt.cfg.stencil || {})[this.reportKey()] || {};
    return (rep[this.pageKey()] || rep[this.tabName()] || []).map(b => ({ ...b }));
  },
  commit(list, { undoable = true } = {}) {
    const rk = this.reportKey(), pk = this.pageKey(), tab = this.tabName();
    if (!pk) return;
    if (undoable) { this.undo.push(JSON.stringify(this.blocks())); if (this.undo.length > 100) this.undo.shift(); this.redo = []; }
    const all = { ...(pbt.cfg.stencil || {}) };
    const page = { ...(all[rk] || {}) };
    if (tab && tab !== pk) delete page[tab];
    // group only on a grouped block, so an ungrouped one is stored exactly as before groups.
    if (list.length) page[pk] = list.map(b => ({ id: b.id, type: b.type, label: b.label, x: b.x, y: b.y, w: b.w, h: b.h, ...(b.group ? { group: b.group } : {}) }));
    else delete page[pk];
    if (Object.keys(page).length) all[rk] = page; else delete all[rk];
    pbt.cfg.stencil = all;
    pbt.save();
    if (list.length > ST_MAX) pbt.toast(`${list.length} blocks on this page; the stencil is meant for a layout, not a full report`, { title: 'Stencil' });
    this.sig = '';
    this.draw();
  },
  history(back) {
    const from = back ? this.undo : this.redo, to = back ? this.redo : this.undo;
    if (!from.length) return;
    to.push(JSON.stringify(this.blocks()));
    this.commit(JSON.parse(from.pop()), { undoable: false });
  },
  newId() { return `b${Date.now().toString(36)}${Math.floor(Math.random() * 1e6).toString(36)}`; },
  defaultLabel(type, list) {
    const t = ST_TYPE[type];
    let n = list.filter(b => b.type === type).length + 1;
    while (list.some(b => b.label === `${t.label} ${n}`)) n++;
    return `${t.label} ${n}`;
  },

  // ---- geometry ----
  geometry() {
    const g = pbt.page.geometry();
    if (!g) return null;
    const p = this.projectPage();
    // The saved page size is exact; the zoom-derived one is the fallback.
    if (p && p.width && p.height && Math.abs(p.width * g.scale - parseFloat(g.da.style.width)) < 2) return { ...g, w: p.width, h: p.height };
    return g;
  },
  grid() { return pbt.qa ? pbt.qa.cfgNow().grid : 8; },
  pt(e, g) { return { x: (e.clientX - g.rect.left) / g.scale, y: (e.clientY - g.rect.top) / g.scale }; },
  // skip: a block's id, or a Set of ids (a group being moved), whose edges are not lines.
  lines(g, skip) {
    const xs = [0, g.w], ys = [0, g.h];
    const skipped = id => (skip instanceof Set ? skip.has(id) : id === skip);
    for (const v of pbt.page.visuals(g)) { xs.push(v.x, v.x + v.w); ys.push(v.y, v.y + v.h); }
    for (const b of this.blocks()) if (!skipped(b.id)) { xs.push(b.x, b.x + b.w); ys.push(b.y, b.y + b.h); }
    return { xs, ys };
  },
  // Blocks with a real visual of the same type at the same place: built. A block matches only the
  // visual it would build (its own visualType), so a pie block over a column chart, or a matrix
  // block over a table, is not built and Convert to visuals still writes it. kindToType is not used
  // here: it folds every type it does not know into a column chart.
  // typeKey reads a Power BI type, as the page names it ("100% stacked bar chart", its aria role
  // description) or as the file does ("hundredPercentStackedBarChart"), as one comparable name:
  // letters only, "100%" spelled out, a trailing "visual" dropped, and the few types whose two names
  // differ in more than spelling (TYPE_ALIAS) under one of them.
  typeKey(kind) {
    const TYPE_ALIAS = {
      tableex: 'table', pivottable: 'matrix', barchart: 'stackedbarchart', columnchart: 'stackedcolumnchart',
      cardnew: 'card', basicshape: 'shape', actionbutton: 'button', keydrivers: 'keyinfluencers', qna: 'qa',
      linestackedcolumncombochart: 'lineandstackedcolumnchart', lineclusteredc)PBT"
LR"PBT(olumncombochart: 'lineandclusteredcolumnchart',
    };
    const k = String(kind || '').toLowerCase().replace(/100\s*%/g, 'hundredpercent').replace(/[^a-z]/g, '').replace(/visual$/, '');
    return TYPE_ALIAS[k] || k;
  },
  // The same name with the equivalences the stencil always had: clustered, stacked and 100% stacked
  // bars are one bar chart (and the same for columns), every kind of slicer is a slicer, and a
  // title, a text and a smart narrative block all build a text box.
  typeMatchKey(kind) {
    const k = this.typeKey(kind);
    if (/slicer/.test(k)) return 'slicer';
    if (/^(clustered|stacked|hundredpercentstacked)barchart$/.test(k)) return 'barchart';
    if (/^(clustered|stacked|hundredpercentstacked)columnchart$/.test(k)) return 'columnchart';
    return k;
  },
  // Whether a Power BI visual (its aria role description or visualType) is what a block of this
  // type builds.
  sameType(kind, type) {
    const t = ST_TYPE[type], k = this.typeMatchKey(kind);
    return !!(t && k) && k === this.typeMatchKey(t.visual);
  },
  // The block type that builds exactly this Power BI visual, or null when none does (a custom
  // visual). A text box is a text block; "Copy page into stencil" makes a short one at the top a
  // title.
  blockType(kind) {
    const k = this.typeKey(kind);
    if (!k) return null;
    if (k === 'textbox') return 'text';
    const t = ST_ALL.find(x => x.visual !== 'textbox' && this.typeKey(x.visual) === k);
    return t ? t.id : null;
  },
  builtIds(g, list) {
    const vs = pbt.page.visuals(g);
    // The aria role description first, then the visual-* class name Power BI puts on the visual.
    return new Set(list.filter(b => vs.some(v => Math.abs(v.x - b.x) <= 1 && Math.abs(v.y - b.y) <= 1 && Math.abs(v.w - b.w) <= 1 && Math.abs(v.h - b.h) <= 1 && (this.sameType(v.kind, b.type) || this.sameType(v.typeName, b.type)))).map(b => b.id));
  },
  // Read only, for Layout QA: this page's blocks that no built visual covers, each with its type's
  // name ({ id, type, typeLabel, label, x, y, w, h }).
  qaBlocks(g) {
    const list = this.blocks(), built = g ? this.builtIds(g, list) : new Set();
    return list.filter(b => !built.has(b.id)).map(b => ({ ...b, typeLabel: (ST_TYPE[b.type] || {}).label || b.type || 'Block' }));
  },
  // The same against the saved files: visuals a build wrote that the page does not show until the
  // report reopens.
  savedIds(page, list) {
    const vs = (page && page.visuals) || [];
    return new Set(list.filter(b => vs.some(v => Math.abs(v.x - b.x) <= 1 && Math.abs(v.y - b.y) <= 1 && Math.abs(v.w - b.w) <= 1 && Math.abs(v.h - b.h) <= 1 && this.sameType(v.type, b.type))).map(b => b.id));
  },

  // ---- pointer editing (edit mode) ----
  onDown(e) {
    if (this.mode !== 'edit' || e.button !== 0 || e.target.closest('.pbt-st-strip')) return;
    const g = this.g;
    if (!g) return;
    e.preventDefault();
    e.stopPropagation();
    document.querySelectorAll('.pbt-fm-menu').forEach(m => m.remove());
    // The keys follow the press: the stencil holds the focus while you edit on it.
    if (document.activeElement !== this.host) this.host.focus({ preventScroll: true });
    const p = this.pt(e, g);
    const handle = e.target.closest('[data-h]'), blockEl = e.target.closest('.pbt-st-block');
    this.work = this.blocks();
    const before = JSON.stringify(this.work);
    const free = e.altKey;
    if (handle && this.sel) {
      const b = this.work.find(x => x.id === this.sel);
      if (b) this.drag = { kind: 'resize', h: handle.dataset.h, p0: p, r0: { ...b }, before, lines: this.lines(g, b.id) };
    } else if (blockEl) {
      this.sel = blockEl.dataset.id;
      // A second press on the same block within 450 ms is a double-click: rename it. (The block is
      // redrawn between the presses, so the browser's own dblclick never reaches it.)
      const last = this.lastDown, now = Date.now();
      this.lastDown = { id: this.sel, t: now };
      if (last && last.id === this.sel && now - last.t < 450) { this.lastDown = null; this.work = null; this.sig = ''; this.draw(); this.focusLabel(); return; }
      const b = this.work.find(x => x.id === this.sel);
      // A block in a group moves its whole group: the group's box snaps and stays on the page, and
      // every member moves by as much.
      const members = b && b.group ? this.work.filter(x => x.group === b.group) : [];
      if (b && members.length > 1) {
        const box = this.groupBoxes(members)[0];
        this.drag = { kind: 'move', p0: p, r0: { ...b }, before, box, members: members.map(x => ({ id: x.id, x: x.x, y: x.y })), lines: this.lines(g, new Set(members.map(x => x.id))) };
      } else if (b) this.drag = { kind: 'move', p0: p, r0: { ...b }, before, lines: this.lines(g, b.id) };
    } else {
      this.sel = null;
      const grid = free ? 1 : this.grid();
      const lines = this.lines(g);
      const tol = free ? 0 : grid / 2;
      // c0 is the press in screen pixels: only a drag past a few of them draws a block, so a plain
      // click on the empty page just clears the selection.
      this.drag = { kind: 'draw', p0: { x: this.snapTo(p.x, lines.xs, grid, tol), y: this.snapTo(p.y, lines.ys, grid, tol) }, c0: { x: e.clientX, y: e.clientY }, before, lines };
    }
    if (!this.drag) { this.work = null; return; }
    this.host.setPointerCapture(e.pointerId);
    this.sig = '';
    this.draw();
  },
  onMove(e) {
    const d = this.drag, g = this.g;
    if (!d || !g) return;
    e.preventDefault();
    const p = this.pt(e, g);
    const free = e.altKey, grid = free ? 1 : this.grid(), tol = free ? 0 : grid / 2;
    const min = Math.max(grid, 8);
    const clamp = (v, lo, hi) => Math.max(lo, Math.min(hi, v));
    if (d.kind === 'move' && d.box) {
      const nx = clamp(this.snapSpan(d.box.x + p.x - d.p0.x, d.box.w, d.lines.xs, grid, tol), 0, g.w - d.box.w);
      const ny = clamp(this.snapSpan(d.box.y + p.y - d.p0.y, d.box.h, d.lines.ys, grid, tol), 0, g.h )PBT"
LR"PBT(- d.box.h);
      for (const m of d.members) { const x = this.work.find(o => o.id === m.id); if (x) { x.x = m.x + nx - d.box.x; x.y = m.y + ny - d.box.y; } }
    } else if (d.kind === 'move') {
      const b = this.work.find(x => x.id === d.r0.id);
      b.x = clamp(this.snapSpan(d.r0.x + p.x - d.p0.x, b.w, d.lines.xs, grid, tol), 0, g.w - b.w);
      b.y = clamp(this.snapSpan(d.r0.y + p.y - d.p0.y, b.h, d.lines.ys, grid, tol), 0, g.h - b.h);
    } else if (d.kind === 'resize') {
      const b = this.work.find(x => x.id === d.r0.id), r = d.r0;
      let l = r.x, t = r.y, rt = r.x + r.w, bt = r.y + r.h;
      const sx = v => clamp(this.snapTo(v, d.lines.xs, grid, tol), 0, g.w), sy = v => clamp(this.snapTo(v, d.lines.ys, grid, tol), 0, g.h);
      if (d.h.includes('w')) l = Math.min(sx(r.x + p.x - d.p0.x), rt - min);
      if (d.h.includes('e')) rt = Math.max(sx(r.x + r.w + p.x - d.p0.x), l + min);
      if (d.h.includes('n')) t = Math.min(sy(r.y + p.y - d.p0.y), bt - min);
      if (d.h.includes('s')) bt = Math.max(sy(r.y + r.h + p.y - d.p0.y), t + min);
      Object.assign(b, { x: l, y: t, w: rt - l, h: bt - t });
    } else {
      if (!d.moved && Math.hypot(e.clientX - d.c0.x, e.clientY - d.c0.y) >= ST_DRAG_PX) d.moved = true;
      if (!d.moved) return;
      const x1 = clamp(this.snapTo(p.x, d.lines.xs, grid, tol), 0, g.w), y1 = clamp(this.snapTo(p.y, d.lines.ys, grid, tol), 0, g.h);
      d.rect = { x: Math.min(d.p0.x, x1), y: Math.min(d.p0.y, y1), w: Math.abs(x1 - d.p0.x), h: Math.abs(y1 - d.p0.y) };
    }
    this.sig = '';
    this.draw();
  },
  onUp(e) {
    const d = this.drag, g = this.g;
    if (!d) return;
    this.drag = null;
    try { this.host.releasePointerCapture(e.pointerId); } catch {}
    let list = this.work;
    this.work = null;
    if (d.kind === 'draw' && g && d.moved && d.rect) {
      // Only a drag draws a block (a click makes none). A drag thinner than the smallest block in
      // either direction still draws one, grown to that size and kept inside the page.
      const min = Math.max(this.grid(), 8);
      const w = Math.min(g.w, Math.max(min, d.rect.w)), h = Math.min(g.h, Math.max(min, d.rect.h));
      const r = { x: Math.max(0, Math.min(d.rect.x, g.w - w)), y: Math.max(0, Math.min(d.rect.y, g.h - h)), w, h };
      const type = ST_TYPE[this.type] ? this.type : 'card';
      const b = { id: this.newId(), type, label: this.defaultLabel(type, list), ...r };
      list = [...list, b];
      this.sel = b.id;
    }
    if (JSON.stringify(list) === d.before) { this.sig = ''; this.draw(); return; }
    this.undo.push(d.before);
    this.redo = [];
    this.commit(list, { undoable: false });
  },
  onKey(e) {
    if (!this.on || this.mode !== 'edit' || (e.ctrlKey && e.altKey)) return;
    // The keys are the stencil's only while nothing else has them: no help page or skins menu open,
    // and the focus on the page itself or on the stencil (not a pane, a menu, a Power BI field or
    // another tool's control).
    if (document.querySelector('.pbt-help-backdrop, .pbt-menu')) return;
    const t = document.activeElement;
    const free = !t || t === document.body || t === document.documentElement || (this.host && this.host.contains(t) && !/^(INPUT|TEXTAREA|SELECT)$/.test(t.tagName));
    if (!free) return;
    const list = this.blocks();
    const b = list.find(x => x.id === this.sel);
    const k = e.key, ctrl = e.ctrlKey || e.metaKey;
    let done = true;
    if (k === 'Escape') { if (this.sel) this.sel = null; else this.setMode('view'); this.sig = ''; this.draw(); }
    else if (ctrl && (k === 'z' || k === 'Z') && !e.shiftKey) this.history(true);
    else if (ctrl && (k === 'y' || ((k === 'z' || k === 'Z') && e.shiftKey))) this.history(false);
    else if (b && (k === 'Delete' || k === 'Backspace')) this.remove(b.id);
    // Copy, cut and paste one block, held in memory by the stencil (never the system clipboard).
    // Ctrl+C and Ctrl+V with nothing to copy or paste fall through to Power BI.
    else if (b && ctrl && !e.shiftKey && (k === 'c' || k === 'C')) this.copyBlock(b, false);
    else if (b && ctrl && !e.shiftKey && (k === 'x' || k === 'X')) this.copyBlock(b, true);
    else if (this.clip && ctrl && !e.shiftKey && (k === 'v' || k === 'V')) this.paste();
    else if (b && ctrl && (k === 'd' || k === 'D')) {
      const g = this.g, step = this.grid();
      const c = { ...b, id: this.newId(), label: this.defaultLabel(b.type, list), x: Math.min(b.x + step, g ? g.w - b.w : b.x), y: Math.min(b.y + step, g ? g.h - b.h : b.y) };
      // A duplicate is a block of its own, as a pasted one is: in no group.
      delete c.group;
      this.sel = c.id;
      this.commit([...list, c]);
    } else if (b && /^Arrow/.test(k)) {
      const g = this.g, step = e.shiftKey ? this.grid() : 1;
      const dx = k === 'ArrowLeft' ? -step : k === 'ArrowRight' ? step : 0, dy = k === 'ArrowUp' ? -step : k === 'ArrowDown' ? step : 0;
      b.x = Math.max(0, Math.min(g ? g.w - b.w : 1e5, b.x + dx));
      b.y = Math.max(0, Math.min(g ? g.h - b.h : 1e5, b.y + dy));
      this.commit(list);
    } else done = false;
    if (done) { e.preventDefault(); e.stopImmediatePropagation(); }
  },
  focusLabel() { setTimeout(() => { const i = document.querySelector('.pbt-st-strip .pbt-st-label-input'); if (i) { i.focus(); i.select(); } }, 0); },

  // ---- actions ----
  setMode(mode) {
    this.mode = mode;
    if (mode === 'view') { this.sel = null; this.drag = null; this.work = null; }
    this.sig = '';
    this.draw();
  },
  // A type picked in the strip or the "\u2026" menu. With a block selected it changes that block's type
  // in place; with none it is the type the next block drawn takes. Either way it is also the type
  // the next drawing uses.
  pick(type) {
    if (!ST_TYPE[type]) return;
    this.type = type;
    const list = this.blocks(), b = this.mode === 'edit' && list.find(x => x.id === this.sel);
    if (b) { if (b.type !== type) this.retype(list, b, type); else { th)PBT"
LR"PBT(is.sig = ''; this.draw(); } return; }
    if (this.mode !== 'edit') this.setMode('edit'); else { this.sig = ''; this.draw(); }
  },
  // Whether a block's label is still the name the stencil gave it ("Card 3" for a card).
  isDefaultLabel(b) {
    const t = ST_TYPE[b.type];
    if (!b.label) return true;
    return !!t && b.label.startsWith(`${t.label} `) && /^\d+$/.test(b.label.slice(t.label.length + 1));
  },
  // One block becomes another type, as one undo step: same place and size, and a name the user
  // typed stays; a default name becomes the new type's default name.
  retype(list, b, type) {
    if (this.isDefaultLabel(b)) b.label = this.defaultLabel(type, list.filter(x => x !== b));
    b.type = type;
    this.commit(list);
  },
  // Removes one block, as one undo step.
  remove(id) {
    const list = this.blocks();
    if (!list.some(x => x.id === id)) return;
    if (this.sel === id) this.sel = null;
    this.commit(list.filter(x => x.id !== id));
  },
  // Ctrl+C (cut false) or Ctrl+X (cut true): the block goes into the stencil's own clipboard; a cut
  // also removes it, as one undo step.
  copyBlock(b, cut) {
    this.clip = { type: b.type, label: b.label, x: b.x, y: b.y, w: b.w, h: b.h };
    if (cut) this.remove(b.id);
    else pbt.toast(`${b.label || (ST_TYPE[b.type] || ST_TYPE.card).label} copied; Ctrl+V pastes it here or on another page`, { title: 'Stencil', ms: 2500 });
  },
  // Ctrl+V: the copied block lands where it was copied from, or one grid step down and right of it
  // when a block already sits there (each paste then steps on from the last), kept inside the page
  // and selected. A name already used on this page gets a number (a default name, the type's next
  // one). One undo step.
  paste() {
    const c = this.clip, g = this.g;
    if (!c || !ST_TYPE[c.type]) return;
    const list = this.blocks(), step = this.grid();
    const w = g ? Math.min(c.w, g.w) : c.w, h = g ? Math.min(c.h, g.h) : c.h;
    const inside = (x, y) => ({ x: Math.max(0, Math.min(x, g ? g.w - w : x)), y: Math.max(0, Math.min(y, g ? g.h - h : y)) });
    let at = inside(c.x, c.y);
    for (let i = 0; i < 50 && list.some(b => b.x === at.x && b.y === at.y); i++) at = inside(at.x + step, at.y + step);
    let label = c.label || '';
    if (!label || list.some(b => b.label === label)) {
      if (this.isDefaultLabel(c)) label = this.defaultLabel(c.type, list);
      else { let n = 2; while (list.some(b => b.label === `${c.label} ${n}`)) n++; label = `${c.label} ${n}`; }
    }
    const b = { id: this.newId(), type: c.type, label, x: at.x, y: at.y, w, h };
    this.clip = { ...c, x: at.x, y: at.y };
    this.sel = b.id;
    this.commit([...list, b]);
  },
  copyPage() {
    const g = this.g;
    if (!g) return;
    const list = this.blocks();
    // The groups come from the saved files: a visual on the page in a saved group joins a block
    // group of that group's name. A visual moved since the last save matches no saved place and
    // copies ungrouped; a saved group's own box drawn on the page is not a visual to copy.
    const page = this.projectPage();
    const hasGroups = !!(page && page.visuals.some(v => v.type === 'group'));
    const files = (pbt.project.raw || {}).files, hidden = hasGroups ? this.hiddenNames(page, files) : new Set();
    const places = hasGroups ? this.savedPlaces(page, this.groupNames(page, files), hidden) : [];
    // The groups' own boxes on the page: savedPlaces passes over groups, so here they go in under
    // another type.
    const groupBoxes = hasGroups ? this.savedPlaces({ visuals: page.visuals.map(v => (v.type === 'group' ? { ...v, type: 'groupbox' } : v)) }, {}, hidden).filter(a => a.type === 'groupbox') : [];
    const tol = g.scale < 0.999 ? Math.max(1, 1 / g.scale) : 1;
    const add = pbt.page.visuals(g).filter(v => !list.some(b => b.x === v.x && b.y === v.y && b.w === v.w && b.h === v.h)).map(v => {
      const group = hasGroups ? this.groupAt(v, places, tol) : '';
      if (hasGroups && !group && groupBoxes.some(a => Math.abs(a.x - v.x) <= tol && Math.abs(a.y - v.y) <= tol && Math.abs(a.w - v.w) <= tol && Math.abs(a.h - v.h) <= tol)) return null;
      // The exact block type when one builds this visual, so the copy shows as built; else the
      // nearest common type.
      let type = this.blockType(v.kind) || this.blockType(v.typeName) || this.kindToType(v.kind);
      if (type === 'text' && v.h <= 80 && v.y < 120) type = 'title';
      const b = { id: this.newId(), type, label: v.title || v.kind || ST_TYPE[type].label, x: v.x, y: v.y, w: v.w, h: v.h };
      if (group) b.group = group;
      return b;
    }).filter(Boolean);
    if (!add.length) { pbt.toast('Every visual on this page is already a block', { title: 'Stencil' }); return; }
    // A copied group whose name the page's blocks already use gets a name of its own ("Name 2"),
    // so two unrelated sets never merge into one group (one drag, one outline, one frame).
    const used = new Set(list.map(b => b.group).filter(Boolean)), rename = new Map();
    for (const b of add) {
      if (!b.group) continue;
      if (!rename.has(b.group)) {
        let n = b.group, k = 2;
        while (used.has(n)) n = `${b.group} ${k++}`;
        used.add(n);
        rename.set(b.group, n);
      }
      b.group = rename.get(b.group);
    }
    this.commit([...list, ...add]);
    const groups = new Set(add.map(b => b.group).filter(Boolean)).size;
    pbt.toast(`${add.length} visual${add.length === 1 ? '' : 's'} copied into the stencil${groups ? `, keeping ${groups} group${groups === 1 ? '' : 's'}` : ''}`, { title: 'Stencil' });
  },
  clear(builtOnly) {
    const g = this.g, list = this.blocks();
    const built = g ? this.builtIds(g, list) : new Set();
    const keep = builtOnly ? list.filter(b => !built.has(b.id)) : [];
    this.sel = null;
    this.commit(keep);
    pbt.toast(builtOnly ? `${list.length - keep.length} built block${list.length - keep.length === 1 ? '' : 's'} cleared` : 'Stencil c)PBT"
LR"PBT(leared for this page; Ctrl+Z in edit mode brings it back', { title: 'Stencil' });
  },
  // The build for this page or a new one, or { error } when it cannot be built. withFrames (the
  // copied script with "with frames as background" on) adds the page's frames; the development
  // build never asks for them, so a frames problem cannot stop it.
  plan(newPage, withFrames) {
    const project = this.project();
    const list = this.blocks();
    const g = this.g;
    if (!project) {
      const p = pbt.project && pbt.project.get();
      if (p) return { error: `This window shows ${this.reportName()}, but the report project folder is ${p.root.split(/[\\/]/).pop()}, so building here would write into another report.` };
      return { error: `No report files are loaded, so there is nowhere to write. ${ST_NEEDS_PBIP}` };
    }
    if (!list.length) return { error: 'Nothing to build: draw some blocks first.' };
    const page = this.projectPage();
    if (!newPage && !page) return { error: 'This page is not in the saved report files yet. Save the report (Ctrl+S) and try again, or build as a new page.', unsaved: true };
    // On this page, blocks whose visual is already there (drawn, or written by a build the page has
    // not reloaded yet) are skipped; a new page gets every block.
    let todo = list;
    if (!newPage) {
      const built = g ? this.builtIds(g, list) : new Set(), saved = this.savedIds(page, list);
      todo = list.filter(b => !built.has(b.id) && !saved.has(b.id));
      if (!todo.length) return { error: 'Every block already has its visual on this page (or in its saved files, waiting for a reopen).' };
    }
    // The page's own visuals name the schema version Desktop writes; the default otherwise.
    let schema = ST_SCHEMA;
    if (page && pbt.project.raw) {
      const f = pbt.project.raw.files.find(x => x.path.startsWith(`definition/pages/${page.name}/visuals/`) && x.path.endsWith('/visual.json'));
      const m = f && /"\$schema"\s*:\s*"([^"]*visualContainer[^"]*)"/.exec(f.text);
      if (m) schema = m[1];
    }
    // "Sales (stencil)" built again is "Sales (stencil) 2", not "Sales (stencil) (stencil)".
    const display = ((page && page.displayName) || this.tabName() || 'Page').replace(/ \(stencil\)( \d+)?$/, '');
    try {
      const build = this.files({ project, page: page && page.name, newPage: newPage ? { displayName: `${display} (stencil)`, width: g ? g.w : 1280, height: g ? g.h : 720 } : null, blocks: todo, schema });
      // "with frames as background" on: every block on the page (built or not) gets its frame on
      // the page the visuals go to, in the same script.
      let frames = null;
      if (withFrames) {
        const np = newPage ? JSON.parse(build.files[0].text) : null;
        frames = this.framesSpec({ page: build.page, display: np ? np.displayName : page.displayName, width: np ? np.width : page.width || (g ? g.w : 1280), height: np ? np.height : page.height || (g ? g.h : 720), blocks: list, theme: pbt.project.theme() });
      }
      return { ...build, root: project.root, count: todo.length, ids: todo.map(b => b.id), frames };
    } catch (err) { return { error: err.message }; }
  },
  // The frames background for this page, or { error }: one frame per group and per block in no
  // group, whether or not its visual is built yet.
  planFrames() {
    const project = this.project();
    const list = this.blocks();
    const g = this.g;
    if (!project) {
      const p = pbt.project && pbt.project.get();
      if (p) return { error: `This window shows ${this.reportName()}, but the report project folder is ${p.root.split(/[\\/]/).pop()}, so converting here would write into another report.` };
      return { error: `No report files are loaded, so there is nowhere to write. ${ST_NEEDS_PBIP}` };
    }
    if (!list.length) return { error: 'Nothing to frame: draw some blocks first.' };
    const page = this.projectPage();
    if (!page) return { error: 'This page is not in the saved report files yet. Save the report (Ctrl+S) and try again.', unsaved: true };
    try {
      const frames = this.framesSpec({ page: page.name, display: page.displayName, width: page.width || (g ? g.w : 1280), height: page.height || (g ? g.h : 720), blocks: list, theme: pbt.project.theme() });
      return { root: project.root, frames };
    } catch (err) { return { error: err.message }; }
  },
  // A page saved a moment ago reaches the files the tools read about 2 s later (the mod reads them
  // again after each save). A build asked for in between waits for that read, up to 4 s, and tries
  // once more; only then is the page reported as not saved. stop() calls endSaveWait with no retry,
  // so a copy of the page code that has been stopped never builds, copies or toasts afterwards (a
  // new copy sends pbt-project while it boots, which would otherwise finish this wait at once).
  waitForSave(retry) {
    if (this.saveWait) return true;
    pbt.toast('Reading the saved report\u2026', { title: 'Stencil', ms: 2500 });
    const done = () => { this.endSaveWait(); retry(); };
    this.saveDone = done;
    window.addEventListener('pbt-project', done);
    this.saveWait = setTimeout(done, 4000);
    return true;
  },
  endSaveWait() {
    if (this.saveDone) window.removeEventListener('pbt-project', this.saveDone);
    clearTimeout(this.saveWait);
    this.saveWait = 0;
    this.saveDone = null;
  },
  build(newPage, given, waited) {
    const b = given || this.plan(newPage);
    if (b.unsaved && !given && !waited) return this.waitForSave(() => this.build(newPage, null, true));
    if (b.error) { pbt.toast(b.error, { title: 'Stencil', tone: 'bad', ms: 7000 }); return false; }
    if (typeof window.pbtBuild !== 'function') return this.copyScript(newPage);
    if (this.building) { pbt.toast('A build is still being written; wait for its answer', { title: 'Stencil' }); return false; }
    this.building = true;
    clearTimeout(this.buildTimer);
    this.buildTimer = setTimeo)PBT"
LR"PBT(ut(() => { this.building = false; }, 60000);
    // The injector re-runs the page code with fresh project files before it answers, so what the
    // answer needs travels with the request. noReload (tests) leaves the open report alone.
    const move = b.appendPage ? { appendPage: b.appendPage, report: this.reportKey(), from: this.pageKey(), ids: b.ids } : null;
    window.pbtBuild(JSON.stringify({ files: b.files, appendPage: b.appendPage, move, noReload: !!b.noReload }));
    pbt.toast(`Writing ${b.count} placeholder visual${b.count === 1 ? '' : 's'}\u2026`, { title: 'Stencil' });
    return true;
  },
  // The injector's answer to pbtBuild.
  built(result) {
    this.building = false;
    if (!result || !result.ok) { pbt.toast(`Nothing written: ${(result && result.error) || 'no answer from the injector'}`, { title: 'Stencil', tone: 'bad', ms: 8000 }); return; }
    // Blocks built onto a new page move there with their visuals, so they show as built.
    const lb = result.move;
    if (lb && lb.appendPage && Array.isArray(lb.ids)) {
      const all = { ...(pbt.cfg.stencil || {}) }, rep = { ...(all[lb.report] || {}) };
      const from = rep[lb.from] || [];
      const moved = from.filter(b => lb.ids.includes(b.id));
      if (moved.length) {
        rep[lb.appendPage] = [...(rep[lb.appendPage] || []), ...moved];
        const left = from.filter(b => !lb.ids.includes(b.id));
        if (left.length) rep[lb.from] = left; else delete rep[lb.from];
        all[lb.report] = rep;
        pbt.cfg.stencil = all;
        pbt.save();
      }
    }
    pbt.toast(`${result.written} file${result.written === 1 ? '' : 's'} written. ${result.reloaded ? 'The report reloaded from its files.' : 'Power BI shows a bar at the top: click Apply external changes to load them (or reopen the report).'}`, { title: 'Stencil', tone: 'good', ms: 8000 });
    if (!this.on) { this.on = true; pbt.restack(); }
    this.setMode('view');
  },
  // "Convert to visuals": copies the build script, then says in three steps what to do with it.
  copyScript(newPage, waited) {
    const b = this.plan(newPage, !!pbt.cfg.stencilFrames);
    if (b.unsaved && !waited) return this.waitForSave(() => this.copyScript(newPage, true));
    if (b.error) { pbt.toast(b.error, { title: 'Stencil', tone: 'bad', ms: 7000 }); return false; }
    pbt.copy(this.script(b.root, b, b.frames), `Convert to visuals${b.frames ? ' with frames' : ''}: script copied`);
    pbt.toast(ST_CONVERT_STEPS, { title: `Convert to visuals${newPage ? ': new page' : ''}${b.frames ? ', with frames' : ''}`, ms: 15000 });
    return true;
  },
  // "Convert to background": copies the frames script, then says in three steps what to do with it.
  copyBackground(waited) {
    const b = this.planFrames();
    if (b.unsaved && !waited) return this.waitForSave(() => this.copyBackground(true));
    if (b.error) { pbt.toast(b.error, { title: 'Stencil', tone: 'bad', ms: 7000 }); return false; }
    pbt.copy(this.script(b.root, null, b.frames), 'Convert to background: script copied');
    pbt.toast(ST_FRAMES_STEPS, { title: 'Convert to background', ms: 15000 });
    return true;
  },
  copySvg() {
    const g = this.g, list = this.blocks();
    if (!list.length) { pbt.toast('Nothing to copy: draw some blocks first', { title: 'Stencil' }); return; }
    pbt.copy(this.svg(list, g ? g.w : 1280, g ? g.h : 720), 'Stencil copied as SVG at report size; paste it into Figma as a frame.');
  },

  // The Build menu (Power BI's menu look, as the filter map's).
  menu(anchor, confirm) {
    document.querySelectorAll('.pbt-fm-menu').forEach(m => m.remove());
    const el = pbt.el;
    const dev = typeof window.pbtBuild === 'function';
    const project = this.project();
    const list = this.blocks();
    // beforeClose: the layout picker's hook that keeps a template name still being typed.
    let beforeClose = null;
    const close = () => { if (beforeClose) { const f = beforeClose; beforeClose = null; f(); } m.remove(); document.removeEventListener('pointerdown', outside, true); };
    const outside = e => { if (!m.contains(e.target) && e.target !== anchor && !anchor.contains(e.target)) close(); };
    const items = [];
    const item = (glyph, text, run, tip, disabled) => {
      const b = el('button', { class: 'pbt-fm-item', role: 'menuitem', onpointerdown: e => { e.stopPropagation(); e.preventDefault(); }, onclick: e => { e.stopPropagation(); run(); } },
        el('span', { class: `glyphicon glyph-small ${glyph} pbt-fm-icon`, 'aria-hidden': 'true' }), el('span', { text }));
      if (disabled) b.disabled = true;
      items.push(b);
      return pbt.tip(b, tip);
    };
    const sep = () => el('div', { class: 'pbt-fm-sep', role: 'separator' });
    let body;
    if (confirm === 'types') {
      // The "\u2026" menu: every other Power BI visual a block can stand for, by group. Picking one does
      // what a type button in the strip does.
      const b = this.mode === 'edit' && list.find(x => x.id === this.sel);
      const cur = b ? b.type : this.type;
      body = [el('div', { class: 'pbt-fm-menu-head', text: b ? `Change ${b.label || 'the block'} to` : 'Draw a block of type' })];
      let group = '';
      for (const t of ST_MORE) {
        if (t.group !== group) { if (group) body.push(sep()); group = t.group; body.push(el('div', { class: 'pbt-fm-menu-head', text: group })); }
        const built = t.visual === 'textbox' ? 'a text box' : `a ${t.visual} visual`;
        const it = item(t.glyph, t.label, () => { close(); this.pick(t.id); },
          `${t.label}\n${b ? `Changes the selected block to a ${t.label.toLowerCase()} block, keeping its place and size; Ctrl+Z undoes it.` : `Draw a ${t.label.toLowerCase()} block: drag on the page.`} Builds as ${built} with no fields.`);
        if (t.id === cur) it.setAttribute('aria-current', 'true');
        body.push(it);
      }
    } else if (confirm === 'presets') {
      // The layout picker: a search box over a scrolling list, the built-in l)PBT"
LR"PBT(ayouts first and then
      // "Your templates", each of those with rename and delete. Typing filters by name; rename and
      // delete happen inside the picker (the host may block window.prompt and window.confirm).
      const swallow = e => { e.stopPropagation(); e.preventDefault(); };
      const replaces = list.length ? `\nReplaces the ${list.length} block${list.length === 1 ? '' : 's'} on this page; Ctrl+Z in edit mode brings them back.` : '';
      const search = el('input', { class: 'pbt-st-search', type: 'search', placeholder: 'Search layouts and templates', 'aria-label': 'Search layouts and templates' });
      const pane = el('div', { class: 'pbt-st-pick' });
      // editing: { id, mode: 'rename' | 'delete' } for the one saved template being renamed or
      // deleted, else null. pending: the open rename box's finish, so a name still being typed is
      // kept before any other picker action and when the picker closes (a click on a tile takes
      // no focus, so the box would not blur). settle() runs it and is false when the name was
      // refused; the other action then waits.
      let editing = null, pending = null;
      const settle = () => !pending || pending(true);
      beforeClose = () => { if (pending) pending(true, true); };
      const msg = el('div', { class: 'pbt-st-confirm', role: 'status' });
      const say = text => { msg.textContent = text; msg.hidden = !text; };
      say('');
      const back = item('pbi-glyph-pageleft', 'Back', () => { if (settle()) this.menu(anchor); }, 'Back\nThe Build menu.');
      const tile = (p, custom) => {
        const b = el('button', { class: 'pbt-st-tile', role: 'menuitem', dataset: { preset: p.id }, onpointerdown: swallow, onclick: e => { e.stopPropagation(); if (!settle()) return; close(); this.applyPreset(p.id); } },
          this.thumb(p), el('span', { class: 'pbt-st-tile-name', text: p.name }));
        pbt.tip(b, `${p.name}\n${custom ? 'Your template.' : p.desc} ${p.blocks.length} block${p.blocks.length === 1 ? '' : 's'}, scaled to this page and placed on the grid.${replaces}`);
        if (!custom) return [b, [b]];
        const icon = (ds, label, run) => {
          const ns = 'http://www.w3.org/2000/svg', svg = document.createElementNS(ns, 'svg');
          for (const [k, v] of Object.entries({ viewBox: '0 0 16 16', width: 12, height: 12, 'aria-hidden': 'true' })) svg.setAttribute(k, v);
          for (const dd of ds) {
            const path = document.createElementNS(ns, 'path');
            for (const [k, v] of Object.entries({ d: dd, fill: 'none', stroke: 'currentColor', 'stroke-width': '1.3', 'stroke-linecap': 'round', 'stroke-linejoin': 'round' })) path.setAttribute(k, v);
            svg.append(path);
          }
          return pbt.tip(el('button', { class: 'pbt-st-tile-act', 'aria-label': `${label} ${p.name}`, onpointerdown: swallow, onclick: e => { e.stopPropagation(); if (settle()) run(); } }, svg), label);
        };
        const ren = icon(['M10.5 2.5l3 3L6 13H3v-3z', 'M9 4l3 3'], 'Rename', () => { editing = { id: p.id, mode: 'rename' }; render(); });
        const del = icon(['M2.5 4h11', 'M6 4V2.5h4V4', 'M3.8 4l.8 9.5h6.8l.8-9.5'], 'Delete', () => { editing = { id: p.id, mode: 'delete' }; render(); });
        if (editing && editing.id === p.id && editing.mode === 'rename') {
          const input = el('input', { class: 'pbt-st-tile-input', type: 'text', 'aria-label': `New name for ${p.name}`, value: p.name, maxlength: '80' });
          let done = false;
          // Enter, leaving the box, another picker action or closing the picker keeps the new name;
          // Esc keeps the old one. A name another template has (any case) is refused with a line
          // in the picker and the box stays; when the picker is closing, a toast says so instead.
          // Returns false when the name was refused and the box stays.
          const finish = (ok, closing) => {
            if (done) return true;
            const name = input.value.trim();
            if (ok && name && name !== p.name) {
              const other = this.templateNamed(name, p.id);
              if (other) {
                if (!closing) { say(`"${other.name}" is already the name of a template. Type another name, or press Esc to keep "${p.name}".`); return false; }
                pbt.toast(`${p.name} was not renamed: "${other.name}" is already the name of a template.`, { title: 'Stencil', tone: 'bad', ms: 6000 });
              } else this.renameTemplate(p.id, name);
            }
            done = true;
            pending = null;
            say('');
            if (closing || !input.isConnected || !editing || editing.id !== p.id || editing.mode !== 'rename') return true;
            editing = null;
            render();
            const again = pane.querySelector(`[data-preset="${CSS.escape(p.id)}"]`);
            if (again) again.focus();
            return true;
          };
          pending = finish;
          input.addEventListener('keydown', e => { e.stopPropagation(); if (e.key === 'Enter') { e.preventDefault(); finish(true); } if (e.key === 'Escape') { e.preventDefault(); finish(false); } });
          input.addEventListener('input', () => say(''));
          input.addEventListener('blur', () => finish(true));
          setTimeout(() => { input.focus(); input.select(); }, 0);
          return [el('div', { class: 'pbt-st-tile pbt-st-tile-edit' }, this.thumb(p), input), [input]];
        }
        if (editing && editing.id === p.id && editing.mode === 'delete') {
          const yes = el('button', { class: 'pbt-st-tile-yes', onpointerdown: swallow, onclick: e => { e.stopPropagation(); this.deleteTemplate(p.id); editing = null; render(); search.focus(); } }, 'Delete');
          const no = el('button', { class: 'pbt-st-tile-no', onpointerdown: swallow, onclick: e => { e.stopPropagation(); editing = null; render(); } }, 'Cancel');
          for (const x of [yes, no]) x.addEventListener('keydown', e => { if (e.key ===)PBT"
LR"PBT( 'Escape') { e.preventDefault(); e.stopPropagation(); editing = null; render(); } });
          setTimeout(() => no.focus(), 0);
          return [el('div', { class: 'pbt-st-tile pbt-st-tile-edit', role: 'group', 'aria-label': `Delete ${p.name}?` }, el('span', { class: 'pbt-st-tile-name', text: `Delete "${p.name}"?` }), el('span', { class: 'pbt-st-tile-confirm' }, yes, no)), [yes, no]];
        }
        return [el('div', { class: 'pbt-st-tile-wrap' }, b, el('span', { class: 'pbt-st-tile-acts' }, ren, del)), [b, ren, del]];
      };
      const render = () => {
        // A rename box drawn again sets pending again; one that goes is dropped.
        pending = null;
        const custom = this.templates();
        const { builtIn, custom: mine } = this.filterLayouts(search.value, custom);
        const focusable = [];
        const grid = rows => { const kids = rows.map(([node, keys]) => { focusable.push(...keys); return node; }); return el('div', { class: 'pbt-st-tiles' }, ...kids); };
        const parts = [];
        if (builtIn.length) parts.push(el('div', { class: 'pbt-fm-menu-head', text: 'Built-in layouts' }), grid(builtIn.map(p => tile(p, false))));
        parts.push(el('div', { class: 'pbt-fm-menu-head', text: `Your templates${custom.length ? ` (${mine.length === custom.length ? custom.length : `${mine.length} of ${custom.length}`})` : ''}` }));
        if (mine.length) parts.push(grid(mine.map(p => tile(p, true))));
        else parts.push(el('div', { class: 'pbt-st-confirm', text: custom.length ? 'No template matches the search.' : 'None yet. "Save as template" in the Build menu saves this page\'s blocks as one, for any page.' }));
        if (!builtIn.length && !mine.length) parts.unshift(el('div', { class: 'pbt-st-confirm', text: `Nothing is named like "${search.value.trim()}".` }));
        const top = pane.scrollTop;
        pane.replaceChildren(...parts);
        pane.scrollTop = top;
        // The arrow keys walk the search box, the tiles shown and Back.
        items.length = 0;
        items.push(search, ...focusable, back);
      };
      search.addEventListener('keydown', e => {
        e.stopPropagation();
        if (e.key === 'Escape') { e.preventDefault(); if (search.value) { search.value = ''; render(); } else { close(); if (anchor.focus) anchor.focus(); } }
        else if (e.key === 'ArrowDown') { e.preventDefault(); const first = items.find(x => x !== search && !x.disabled); if (first) first.focus(); }
        else if (e.key === 'Enter') { e.preventDefault(); const first = pane.querySelector('.pbt-st-tile[data-preset]'); if (first) first.click(); }
      });
      search.addEventListener('input', () => { editing = null; pane.scrollTop = 0; render(); });
      render();
      setTimeout(() => search.focus(), 0);
      body = [
        el('div', { class: 'pbt-fm-menu-head', text: 'Start from a layout' }),
        search,
        msg,
        pane,
        sep(),
        back,
      ];
    } else if (confirm === 'save') {
      // Save as template: the name typed here, inside the menu (the host may block window.prompt).
      const all = this.templates();
      const input = el('input', { class: 'pbt-st-search', type: 'text', 'aria-label': 'Template name', maxlength: '80', value: (this.tabName() || 'Template').slice(0, 80) });
      const note = el('div', { class: 'pbt-st-confirm' });
      const save = item('pbi-glyph-checkmark', 'Save', () => { if (this.saveTemplate(input.value)) close(); }, 'Save\nKeeps the template with your choices on this computer; it fits any page size.');
      const sync = () => {
        const name = input.value.trim(), same = all.find(t => t.name.toLowerCase() === name.toLowerCase());
        save.disabled = !name;
        note.textContent = same ? `Replaces your template "${same.name}".` : `${list.length} block${list.length === 1 ? '' : 's'} from this page, as a layout for any report and page size.`;
      };
      input.addEventListener('keydown', e => {
        e.stopPropagation();
        if (e.key === 'Enter') { e.preventDefault(); if (!save.disabled) save.click(); }
        else if (e.key === 'Escape') { e.preventDefault(); close(); if (anchor.focus) anchor.focus(); }
      });
      input.addEventListener('input', sync);
      sync();
      items.unshift(input);
      setTimeout(() => { input.focus(); input.select(); }, 0);
      body = [
        el('div', { class: 'pbt-fm-menu-head', text: 'Save as template' }),
        input,
        note,
        save,
        item('pbi-glyph-cancel', 'Cancel', () => close(), 'Cancel\nNothing is saved.'),
      ];
    } else if (confirm) {
      const newPage = confirm === 'new';
      body = [
        el('div', { class: 'pbt-fm-menu-head', text: 'Is the report saved?' }),
        el('div', { class: 'pbt-st-confirm', text: 'Building writes new files into the project and reloads the report from them. Unsaved changes in Power BI would be lost, so save first (Ctrl+S).' }),
        item('pbi-glyph-checkmark', newPage ? 'Saved: build as a new page' : 'Saved: build on this page', () => { close(); this.build(newPage); }, 'Build now\nWrites one new visual.json per block; existing files are never changed.'),
        item('pbi-glyph-cancel', 'Cancel', () => close(), 'Cancel\nNothing is written.'),
      ];
    } else {
      const noProject = !project ? `\nNot available yet: no report files are loaded. ${ST_NEEDS_PBIP}` : '';
      const saved = this.templates().length;
      // "with frames as background": a toggle remembered with the choices; the menu stays open and
      // draws again with its check mark moved.
      const framesOn = !!pbt.cfg.stencilFrames;
      const frameToggle = item(framesOn ? 'pbi-glyph-checkmark' : 'pbt-st-glyph-none', 'with frames as background', () => { pbt.cfg.stencilFrames = !framesOn; pbt.save(); this.menu(anchor); },
        `with frames as background: ${framesOn ? 'on' : 'off'}\nWhen on, Convert to visuals also draws the frames Convert to background makes, in the s)PBT"
LR"PBT(ame script.`);
      frameToggle.setAttribute('role', 'menuitemcheckbox');
      frameToggle.setAttribute('aria-checked', String(framesOn));
      body = [
        el('div', { class: 'pbt-fm-menu-head', text: 'Stencil' }),
        item('pbi-glyph-fittopage', 'Start from a layout', () => this.menu(anchor, 'presets'), `Start from a layout\n${ST_PRESETS.length} starter layouts (KPI rows, slicer sidebars, chart grids, detail tables and more)${saved ? ` and your ${saved} saved template${saved === 1 ? '' : 's'}` : ''}, scaled to this page; adjust them, then build.`),
        item('pbi-glyph-add', 'Save as template', () => this.menu(anchor, 'save'), 'Save as template\nKeeps this page\'s blocks as a named layout, listed under "Your templates" in Start from a layout and scaled to fit any page.', !list.length),
        sep(),
        dev ? item('pbi-glyph-add', 'Build on this page', () => this.menu(anchor, 'page'), `Build on this page\nEach block not built yet becomes a real visual with no fields, titled with its label, at its exact position.${noProject}`, !project || !list.length) : null,
        dev ? item('pbi-glyph-page', 'Build as a new page', () => this.menu(anchor, 'new'), `Build as a new page\nThe same visuals on a new page named after this one; pages.json is backed up first.${noProject}`, !project || !list.length) : null,
        item('pbi-glyph-code', 'Convert to visuals: this page', () => { close(); this.copyScript(false); }, `Convert to visuals: this page\nTurns each block not built yet into a real visual with no fields, on this page. Save the report first.\n${ST_CONVERT_STEPS}${noProject}`, !project || !list.length),
        item('pbi-glyph-code', 'Convert to visuals: new page', () => { close(); this.copyScript(true); }, `Convert to visuals: new page\nTurns every block into a real visual with no fields, on a new page named after this one. Save the report first.\n${ST_CONVERT_STEPS}${noProject}`, !project || !list.length),
        frameToggle,
        item('pbi-glyph-code', 'Convert to background', () => { close(); this.copyBackground(); }, `Convert to background\nDraws a card-style frame behind each group of blocks (and each block in no group) as one SVG, the page's background image: pixel exact and nothing extra to render. Styled from the report theme when it sets a visual background or border. Save the report first.\n${ST_FRAMES_STEPS}${noProject}`, !project || !list.length),
        sep(),
        item('pbi-glyph-download', 'Copy page into stencil', () => { close(); this.copyPage(); }, 'Copy page into stencil\nEvery visual on this page becomes a block at its position, to redesign from.'),
        item('pbi-glyph-copy', 'Copy as SVG', () => { close(); this.copySvg(); }, 'Copy as SVG\nThe blocks as labelled boxes at report size, to paste into Figma or another design tool.', !list.length),
        sep(),
        item('pbi-glyph-checkmark', 'Clear built blocks', () => { close(); this.clear(true); }, 'Clear built blocks\nRemoves the blocks whose visual now sits on the page.', !list.length),
        item('pbi-glyph-eraser', 'Clear this page\'s stencil', () => { close(); this.clear(false); }, 'Clear this page\'s stencil\nRemoves every block on this page; Ctrl+Z in edit mode brings them back.', !list.length),
      ];
    }
    const m = el('div', { class: `pbt-fm-menu pbt-st-menu${confirm === 'types' ? ' pbt-st-more' : confirm === 'presets' ? ' pbt-st-picker' : ''}`, 'data-pbt': '', role: 'menu', 'aria-label': confirm === 'types' ? 'More visual types' : 'Stencil',
      onkeydown: e => {
        const live = items.filter(b => !b.disabled), i = live.indexOf(document.activeElement);
        if (e.key === 'Escape') { e.preventDefault(); e.stopPropagation(); close(); anchor.focus && anchor.focus(); }
        else if (e.key === 'ArrowDown' || e.key === 'ArrowUp') { e.preventDefault(); e.stopPropagation(); const down = e.key === 'ArrowDown'; if (live.length) live[i < 0 ? (down ? 0 : live.length - 1) : (i + (down ? 1 : live.length - 1)) % live.length].focus(); }
      } }, ...body);
    document.body.append(m);
    const r = anchor.getBoundingClientRect(), mr = m.getBoundingClientRect();
    m.style.left = `${Math.max(8, Math.min(innerWidth - mr.width - 8, r.right - mr.width))}px`;
    m.style.top = `${r.top - mr.height - 4 > 8 ? r.top - mr.height - 4 : Math.min(innerHeight - mr.height - 8, r.bottom + 4)}px`;
    m.tabIndex = -1;
    m.focus();
    setTimeout(() => document.addEventListener('pointerdown', outside, true), 0);
  },

  // ---- drawing ----
  strip(g, list, built) {
    const el = pbt.el;
    const swallow = e => { e.stopPropagation(); e.preventDefault(); };
    const button = (attrs, run, tip, ...kids) => pbt.tip(el('button', { ...attrs, onpointerdown: swallow, onclick: e => { e.stopPropagation(); run(e); } }, ...kids), tip);
    const edit = this.mode === 'edit';
    const b = list.find(x => x.id === this.sel);
    // The type lit in the strip: the selected block's (a type click changes it), else the type the
    // next block drawn takes.
    const cur = b && edit ? b.type : this.type;
    const palette = ST_TYPES.map(t => button({ class: `pbt-strip-grid pbt-st-type${cur === t.id && edit ? ' pbt-on' : ''}`, dataset: { type: t.id }, 'aria-label': t.label }, () => this.pick(t.id),
      `${t.label}\n${b && edit ? `Changes the selected block to a ${t.label.toLowerCase()} block, keeping its place and size; Ctrl+Z undoes it.` : `Draw a ${t.label.toLowerCase()} block: drag on the page.`} Builds as ${t.visual === 'textbox' ? 'a text box' : `a ${t.visual} visual`} with no fields.`,
      el('span', { class: `glyphicon glyph-small ${t.glyph}`, 'aria-hidden': 'true' })));
    // "\u2026" right after the common types opens the menu of every other Power BI visual; it is lit
    // while the current type is one of those.
    const moreOn = edit && !ST_TYPES.some(t => t.id === cur);
    palette.push(button({ class: `pbt-strip-grid pbt-st-more-btn${moreOn ? ' pbt-on' : ''}`, text: '\u2026', 'ar)PBT"
LR"PBT(ia-label': 'More visual types', 'aria-haspopup': 'menu' }, e => this.menu(e.currentTarget, 'types'),
      `More visual types\n${ST_MORE.length} more Power BI visuals: stacked and 100% bar and column charts, area, combo, ribbon, waterfall, funnel, scatter, pie, donut, treemap, maps, gauge, multi-row card, KPI, matrix, button, image, shape and the AI visuals.${b && edit ? ' Picking one changes the selected block.' : ''}`));
    let fields = null;
    if (b && edit) {
      const num = (key, label) => {
        const i = el('input', { class: 'pbt-st-num', type: 'number', step: '1', 'aria-label': label, value: String(b[key]) });
        i.addEventListener('keydown', e => { e.stopPropagation(); if (e.key === 'Enter') i.blur(); if (e.key === 'Escape') { i.value = String(b[key]); i.blur(); } });
        i.addEventListener('change', () => { const v = Math.round(Number(i.value)); if (!Number.isFinite(v)) return; const l = this.blocks(), x = l.find(y => y.id === b.id); if (!x) return; x[key] = Math.max(key === 'w' || key === 'h' ? 8 : 0, v); this.commit(l); });
        return pbt.tip(el('label', { class: 'pbt-st-field' }, el('span', { text: label }), i), `${label === 'w' ? 'Width' : label === 'h' ? 'Height' : label} in report pixels\nType a value and press Enter.`);
      };
      const name = el('input', { class: 'pbt-st-label-input', type: 'text', 'aria-label': 'Label', value: b.label || '' });
      name.addEventListener('keydown', e => { e.stopPropagation(); if (e.key === 'Enter') name.blur(); if (e.key === 'Escape') { name.value = b.label || ''; name.blur(); } });
      name.addEventListener('change', () => { const l = this.blocks(), x = l.find(y => y.id === b.id); if (!x) return; x.label = name.value.trim() || this.defaultLabel(x.type, l); this.commit(l); });
      // A trash can drawn inline, so it shows whatever icons Power BI's font has.
      const ns = 'http://www.w3.org/2000/svg', trash = document.createElementNS(ns, 'svg');
      for (const [k, v] of Object.entries({ viewBox: '0 0 16 16', width: 14, height: 14, 'aria-hidden': 'true', class: 'pbt-st-trash' })) trash.setAttribute(k, v);
      for (const dd of ['M2.5 4h11', 'M6 4V2.5h4V4', 'M3.8 4l.8 9.5h6.8l.8-9.5', 'M6.5 6.5v5', 'M9.5 6.5v5']) {
        const p = document.createElementNS(ns, 'path');
        for (const [k, v] of Object.entries({ d: dd, fill: 'none', stroke: 'currentColor', 'stroke-width': '1.2', 'stroke-linecap': 'round', 'stroke-linejoin': 'round' })) p.setAttribute(k, v);
        trash.append(p);
      }
      const del = button({ class: 'pbt-strip-grid pbt-st-del', 'aria-label': 'Delete block' }, () => this.remove(b.id), 'Delete block (Delete)\nRemoves the selected block; Ctrl+Z brings it back.', trash);
      fields = el('span', { class: 'pbt-strip-seg pbt-st-fields' }, pbt.tip(name, 'Label\nThe visual\'s title when built (a text box\'s text). Double-click a block to edit it.'), num('x', 'x'), num('y', 'y'), num('w', 'w'), num('h', 'h'), del);
    }
    const count = pbt.tip(el('span', { class: 'pbt-strip-item' }, el('b', { text: String(list.length) }), el('span', { text: `block${list.length === 1 ? '' : 's'}${built.size ? ` \u00b7 ${built.size} built` : ''}` })),
      `Stencil blocks on this page\nSaved with your choices, per report and page.${built.size ? ` ${built.size} already have their visual on the page (a check mark).` : ''}`);
    const modeBtn = button({ class: `pbt-strip-grid pbt-st-mode${edit ? ' pbt-on' : ''}`, text: edit ? 'Done' : 'Edit' }, () => this.setMode(edit ? 'view' : 'edit'),
      edit ? 'Done editing (Esc)\nThe stencil stays visible and every click goes to Power BI again.' : 'Edit the stencil\nWhile editing, the page takes no clicks: drag to draw, drag a block to move it, its handles to resize. Alt snaps to 1 px; Esc ends.');
    const menuBtn = button({ class: 'pbt-strip-grid pbt-st-menu-btn', text: 'Build \u25be' }, e => this.menu(e.currentTarget), 'Build, convert, copy and clear\nConvert the blocks to real visuals, copy an SVG, copy the page into the stencil, clear blocks.\nConvert to background draws a card-style frame behind each group and lone block, as the page\'s background image.');
    // Convert to visuals, beside Build, while the page has blocks with no visual yet: the same as
    // the Build menu's "Convert to visuals: this page". Without report files it stays visible but
    // dimmed, and a click says what it needs.
    const project = this.project();
    const convertBtn = list.length > built.size ? button({ class: 'pbt-strip-grid pbt-st-convert', text: 'Convert to visuals', 'aria-disabled': project ? 'false' : 'true' }, () => this.copyScript(false),
      `Convert to visuals\nTurns each block not built yet into a real visual with no fields, on this page. Save the report first.\n${ST_CONVERT_STEPS}\nFrames as background: ${pbt.cfg.stencilFrames ? 'on, so the script also draws a frame behind each group and lone block.' : 'off (turn it on in the Build menu to add a frame behind each group and lone block).'}${project ? '' : `\nNot available yet: no report files are loaded. ${ST_NEEDS_PBIP}`}`) : null;
    const helpBtn = button({ class: 'pbt-strip-help', text: '?' }, () => { if (pbt.help) { pbt.help.close(); pbt.help.open(); pbt.help.show('stencil'); } }, 'How the stencil works\nOpens the help page at Stencil.');
    const bar = el('div', { class: 'pbt-qa-strip pbt-st-strip' },
      pbt.tip(el('span', { class: 'pbt-strip-item pbt-st-name', text: 'Stencil' }), 'Stencil (Ctrl+Alt+S)\nA mock-up layer on this page: draw blocks in Edit, build them as real visuals from the Build menu.'), count,
      el('span', { class: 'pbt-strip-seg pbt-st-palette' }, ...palette),
      // The lit type in words beside its icon (the selected block's, else the next block's).
      el('span', { class: `pbt-st-typename${edit ? '' : ' pbt-st-dim'}`, text: (ST_TYPE[cur] || ST_TYPE.card).label }),
      fields, modeBtn, convertBtn, menuBtn, helpBtn);
    pbt.placeStrip(bar, 'stencil', g);
    return b)PBT"
LR"PBT(ar;
  },

  clearHost() { if (this.host) { this.host.remove(); this.host = null; this.layer = null; } this.sig = ''; },
  draw() {
    if (!this.on) { this.clearHost(); return; }
    const g = this.geometry();
    if (!g) { this.clearHost(); return; }
    this.g = g;
    const pk = this.pageKey();
    if (pk !== this.undoKey) { this.undoKey = pk; this.undo = []; this.redo = []; if (!this.drag) this.sel = null; }
    const list = this.blocks();
    const built = this.drag ? new Set() : this.builtIds(g, list);
    const d = this.drag;
    const grid = this.grid();
    const empty = !document.querySelector('.displayArea .visualContainer');
    const sig = JSON.stringify([g.rect.left, g.rect.top, g.rect.width, g.rect.height, g.w, g.h, empty, this.mode, this.type, this.sel, list, [...built], d && d.rect, grid, pbt.stripOrder.filter(n => (pbt.stripTools()[n] || {}).on), !!pbt.cfg.stencilFrames]);
    if (sig === this.sig && this.host && this.host.isConnected) return;
    this.sig = sig;
    const el = pbt.el;
    // One host for as long as the stencil is on, so a drag keeps its pointer capture.
    if (!this.host || !this.host.isConnected) {
      this.host = el('div', { class: 'pbt-st', 'data-pbt': '', tabindex: '-1',
        onpointerdown: e => this.onDown(e), onpointermove: e => this.onMove(e), onpointerup: e => this.onUp(e), onpointercancel: e => this.onUp(e) });
      document.body.append(this.host);
    }
    const host = this.host;
    // Over an empty page the blocks are opaque, so Power BI's empty-canvas art does not show
    // through them; over visuals they stay see-through.
    host.className = `pbt-st${this.mode === 'edit' ? ' pbt-st-edit' : ''}${empty ? ' pbt-st-empty' : ''}`;
    host.style.cssText = `left:${g.rect.left}px;top:${g.rect.top}px;width:${g.rect.width}px;height:${g.rect.height}px;--pbt-grid:${grid * g.scale}px`;
    const px = v => `${v * g.scale}px`;
    const kids = [];
    // A thin dashed outline around each group's blocks, its name above it, under the blocks and
    // click-through.
    for (const r of this.groupBoxes(list)) kids.push(el('div', { class: 'pbt-st-group', dataset: { group: r.name }, style: `left:${px(r.x)};top:${px(r.y)};width:${px(r.w)};height:${px(r.h)}` },
      el('span', { class: 'pbt-st-group-name', text: r.name })));
    for (const b of list) {
      const t = ST_TYPE[b.type] || ST_TYPE.card;
      const sel = this.mode === 'edit' && b.id === this.sel;
      const node = el('div', { class: `pbt-st-block${sel ? ' pbt-st-sel' : ''}${built.has(b.id) ? ' pbt-st-built' : ''}`, dataset: { id: b.id, type: b.type }, style: `left:${px(b.x)};top:${px(b.y)};width:${px(b.w)};height:${px(b.h)}` },
        // A built block's visual says what it is; the block keeps only a check mark in its corner.
        built.has(b.id) ? el('span', { class: 'pbt-st-tag pbt-st-tag-built' }, el('span', { class: 'pbt-st-check', text: '\u2713' }))
          : el('span', { class: 'pbt-st-tag' }, el('span', { class: `glyphicon glyph-small ${t.glyph}`, 'aria-hidden': 'true' }), el('span', { text: b.label || t.label })),
        sel || (d && d.r0 && d.r0.id === b.id) ? el('span', { class: 'pbt-st-size', text: `${Math.round(b.x)}, ${Math.round(b.y)} \u00b7 ${Math.round(b.w)} \u00d7 ${Math.round(b.h)}${b.group ? ` \u00b7 ${b.group}` : ''}` }) : null,
        ...(sel ? ['n', 'ne', 'e', 'se', 's', 'sw', 'w', 'nw'].map(h => el('span', { class: `pbt-st-h pbt-st-h-${h}`, dataset: { h } })) : []));
      kids.push(node);
    }
    if (d && d.kind === 'draw' && d.rect) kids.push(el('div', { class: 'pbt-st-block pbt-st-new', style: `left:${px(d.rect.x)};top:${px(d.rect.y)};width:${px(d.rect.w)};height:${px(d.rect.h)}` },
      el('span', { class: 'pbt-st-size', text: `${Math.round(d.rect.w)} \u00d7 ${Math.round(d.rect.h)}` })));
    // No blocks yet: one line on the page, just above the dock, says how to start. The dock sits
    // below the page when there is room, else inside its bottom edge; the hint stays above it.
    const dock = document.querySelector('.pbt-dock'), dockTop = dock ? dock.getBoundingClientRect().top : Infinity;
    const lift = dockTop < g.rect.top + g.rect.height ? g.rect.top + g.rect.height - dockTop + 8 : 8;
    if (!list.length && !(d && d.kind === 'draw')) kids.push(el('div', { class: 'pbt-st-hint', style: `bottom:${lift}px` }, this.mode === 'edit'
      ? `Drag on the page to draw a ${(ST_TYPE[this.type] || ST_TYPE.card).label.toLowerCase()} block. Pick another type in the strip below, or \u2026 for more.`
      : 'No stencil blocks on this page yet. Click Edit in the strip below to draw some, or Build \u25be for a ready-made layout.'));
    host.replaceChildren(...kids, this.strip(g, list, built));
  },

  toggle() {
    this.on = !this.on;
    pbt.restack();
    if (!this.on) { this.mode = 'view'; this.drag = null; this.work = null; this.sel = null; this.clearHost(); document.querySelectorAll('.pbt-st-menu').forEach(m => m.remove()); pbt.toast('Stencil off', { title: 'Stencil' }); return; }
    // Straight into edit mode on a page with no blocks yet.
    this.mode = this.blocks().length ? 'view' : 'edit';
    this.sig = '';
    this.draw();
    pbt.toast(this.mode === 'edit' ? 'Stencil: drag on the page to draw a block; Esc when done' : 'Stencil on: Edit to change it, Build \u25be to build it', { title: 'Stencil' });
  },
  start() {
    pbt.hotkey('s', 'Stencil', () => this.toggle(), {
      id: 'stencil',
      tool: true, active: () => this.on,
      desc: 'A see-through mock-up layer on the page: draw cards, charts, tables, slicers, titles and text on the grid, in report pixels, then build them as real visuals with no fields.',
      tip: 'Stencil\nBlocks are saved per report and page. Build writes them into the report\'s project files (development) or copies a PowerShell script that does (the mod).',
    });
    pbt.on(window, 'keydown', e => this.onKey(e), { capture: true });
    pbt.every(300, () => { if (this.on && !this.drag && !(document.activeEle)PBT"
LR"PBT(ment && document.activeElement.tagName === 'INPUT' && document.activeElement.closest('.pbt-st-strip'))) this.draw(); });
  },
  stop() { this.endSaveWait(); this.clearHost(); document.querySelectorAll('.pbt-st-menu').forEach(m => m.remove()); },
};
pbt.stencil = stencil;
pbt.module(stencil);

// ---- 45-theme.js ----
// Theme check (Ctrl+Alt+T): which visuals set their own formatting instead of taking the report
// theme's, and what differs.
//
// With the report's project files (page/05-project.js), each visual's saved formatting (its
// objects and container objects) is compared with the theme: the visual style for its type, then
// for every visual ("*"), then the title text class for title font, size and color. Three kinds
// of finding:
// - theme: the visual sets a property the theme sets, to a different value;
// - palette: the visual sets a color that is not one of the theme's colors;
// - page: the visual sets a property the theme leaves open, to a value most visuals on the page
//   that set it do not use (one title hidden among shown ones).
// Properties set to the theme's own value, and ones with no theme default that agree with the
// page, are listed as information. Text content (titles, alt text) is never a finding.
//
// Without project files it falls back to what is drawn: each visual's title (shown, font, size,
// weight, color), background and border against the page's most common value.

const tsleep = ms => new Promise(r => setTimeout(r, ms));
const CONTENT = /\.(text|paragraphs|altText|titleText|webURL|bookmark|tooltip)$/i;
const LABELS = {
  'title.show': 'Title shown', 'title.fontSize': 'Title font size', 'title.fontFamily': 'Title font', 'title.fontColor': 'Title color',
  'title.bold': 'Title bold', 'title.alignment': 'Title alignment', 'title.background': 'Title background',
  'background.show': 'Background shown', 'background.color': 'Background color', 'background.transparency': 'Background transparency',
  'border.show': 'Border shown', 'border.color': 'Border color', 'border.radius': 'Corner radius', 'border.width': 'Border width',
  'dropShadow.show': 'Shadow shown', 'visualHeader.show': 'Header shown', 'subTitle.show': 'Subtitle shown',
  'labels.fontSize': 'Data label size', 'labels.color': 'Data label color', 'labels.fontFamily': 'Data label font',
  'dataPoint.fill': 'Data color', 'legend.show': 'Legend shown',
};
const label = key => LABELS[key] || key.replace(/\[[^\]]*\]/, '').replace(/\./, ' \u203a ');

const theme = {
  name: 'theme',
  on: false,
  host: null,
  sig: '',
  result: null,

  // ---- the pure core ----
  norm(v) {
    if (typeof v === 'string') {
      const s = v.trim();
      if (/^#[0-9a-f]{3,8}$/i.test(s)) { const h = s.length === 4 ? '#' + [...s.slice(1)].map(c => c + c).join('') : s.slice(0, 7); return h.toUpperCase(); }
      return s.replace(/^['"]|['"]$/g, '').split(',')[0].trim().toLowerCase();
    }
    if (typeof v === 'number') return Math.round(v * 100) / 100;
    return v;
  },
  same(a, b) { return JSON.stringify(this.norm(a)) === JSON.stringify(this.norm(b)); },
  // The theme's value for one object.property on a visual type, or undefined.
  flatCache: new WeakMap(),
  flatScopes(t, type) {
    let byType = this.flatCache.get(t);
    if (!byType) { byType = new Map(); this.flatCache.set(t, byType); }
    if (!byType.has(type)) {
      const vs = t.visualStyles || {};
      byType.set(type, [vs[type] && vs[type]['*'], vs['*'] && vs['*']['*']].filter(Boolean).map(scope => pbt.project.flat(scope, true)));
    }
    return byType.get(type);
  },
  themeValue(t, type, key) {
    if (!t) return undefined;
    const plain = key.replace(/\[[^\]]*\]/, '');
    for (const flat of this.flatScopes(t, type)) if (flat[plain] !== undefined) return flat[plain];
    const tc = t.textClasses || {};
    const title = { 'title.fontSize': 'fontSize', 'title.fontFamily': 'fontFace', 'title.fontColor': 'color' }[plain];
    if (title && tc.title && tc.title[title] !== undefined) return tc.title[title];
    return undefined;
  },
  themeColors(t) {
    if (!t) return [];
    const keys = ['good', 'neutral', 'bad', 'maximum', 'center', 'minimum', 'foreground', 'foregroundNeutralSecondary', 'foregroundNeutralDark', 'background', 'backgroundLight', 'backgroundNeutral', 'backgroundDark', 'tableAccent', 'hyperlink', 'visitedHyperlink'];
    return [...(t.dataColors || []), ...keys.map(k => t[k])].filter(c => typeof c === 'string').map(c => this.norm(c));
  },
  onPalette(hex, colors, tol = 12) {
    const p = pbt.color.parse(hex);
    if (!p) return true;
    if (pbt.color.chroma(p) < 0.1) return true; // greys, white and black always pass
    return colors.some(c => { const q = pbt.color.parse(c); return q && Math.abs(q.r - p.r) <= tol && Math.abs(q.g - p.g) <= tol && Math.abs(q.b - p.b) <= tol; });
  },
  // files: the page's saved visuals ({ name, type, objects, containerObjects }); t: the theme.
  // Returns Map(name -> [{ kind: theme|palette|page|info, key, label, value, want, note }]).
  compare(files, t) {
    const colors = this.themeColors(t);
    const props = f => Object.entries({ ...f.containerObjects, ...f.objects }).filter(([k]) => !CONTENT.test(k));
    // For properties the theme leaves open: what most visuals on the page that set them use.
    const tally = new Map();
    for (const f of files) for (const [k, v] of props(f)) {
      if (this.themeValue(t, f.type, k) !== undefined) continue;
      const m = tally.get(k) || new Map();
      const n = JSON.stringify(this.norm(v));
      m.set(n, (m.get(n) || 0) + 1);
      tally.set(k, m);
    }
    const out = new Map();
    for (const f of files) {
      const rows = [];
      for (const [k, v] of props(f)) {
        const want = this.themeValue(t, f.type, k);
        const isColor = typeof v === 'string' && /^#[0-9a-f]{3,8}$/i.test(v.trim());
        if (want !== undefined) {
          if (!this.same(v, want)) rows.push({ kind: 'theme', key: k, label: label(k), value: v, want }))PBT"
LR"PBT(;
          else rows.push({ kind: 'info', key: k, label: label(k), value: v, note: 'repeats the theme' });
        } else {
          const m = tally.get(k);
          const setters = m ? [...m.values()].reduce((a, b) => a + b, 0) : 0;
          const top = m ? [...m.entries()].sort((a, b) => b[1] - a[1])[0] : null;
          if (setters >= 3 && top && top[1] / setters >= 0.6 && top[0] !== JSON.stringify(this.norm(v))) rows.push({ kind: 'page', key: k, label: label(k), value: v, want: JSON.parse(top[0]), note: `${top[1]} of ${setters} visuals on the page that set it` });
          else rows.push({ kind: 'info', key: k, label: label(k), value: v, note: 'no theme default' });
        }
        if (isColor && colors.length && !this.onPalette(v, colors)) rows.push({ kind: 'palette', key: k, label: label(k), value: v, note: 'not one of the theme\'s colors' });
      }
      out.set(f.name, rows);
    }
    return out;
  },

  // Without project files: what is drawn, against the page's most common value.
  drawn(v) {
    const t = v.el.querySelector('.visualTitle');
    const tt = t && ([...t.querySelectorAll('*')].find(e => e.children.length === 0 && e.textContent.trim()) || t);
    const cs = tt && getComputedStyle(tt);
    const wrap = v.el.querySelector('.visualWrapper') || v.el.querySelector('.vcBody');
    const ws = wrap && getComputedStyle(wrap);
    const hex = c => { const p = pbt.color.parse(c); return p && p.a > 0.05 ? pbt.color.hex({ ...p, a: p.a > 0.95 ? 1 : p.a }).toUpperCase() : 'none'; };
    const bw = ws ? parseFloat(ws.borderTopWidth) || 0 : 0;
    return {
      'title.show': !!t,
      'title.fontFamily': cs ? cs.fontFamily.split(',')[0].replace(/["']/g, '').trim() : undefined,
      'title.fontSize': cs ? Math.round(parseFloat(cs.fontSize) * 0.75 * 10) / 10 : undefined,
      'title.bold': cs ? +cs.fontWeight >= 600 : undefined,
      'title.fontColor': cs ? hex(cs.color) : undefined,
      'background.color': ws ? hex(ws.backgroundColor) : undefined,
      'border.show': ws ? bw > 0 && ws.borderTopStyle !== 'none' && hex(ws.borderTopColor) !== 'none' : undefined,
      'border.radius': ws ? Math.round(parseFloat(ws.borderTopLeftRadius) || 0) : undefined,
    };
  },
  majority(list) {
    const out = new Map(list.map(x => [x.v, []]));
    const keys = Object.keys(list[0] ? list[0].d : {});
    for (const k of keys) {
      const have = list.filter(x => x.d[k] !== undefined);
      if (have.length < 3) continue;
      const m = new Map();
      for (const x of have) { const n = JSON.stringify(this.norm(x.d[k])); m.set(n, (m.get(n) || 0) + 1); }
      const [top, n] = [...m.entries()].sort((a, b) => b[1] - a[1])[0];
      if (n / have.length < 0.6) continue;
      for (const x of have) if (JSON.stringify(this.norm(x.d[k])) !== top) out.get(x.v).push({ kind: 'page', key: k, label: label(k), value: x.d[k], want: JSON.parse(top), note: `${n} of ${have.length} visuals on the page` });
    }
    return out;
  },

  // ---- reading the page ----
  analyse() {
    const g = pbt.page.geometry();
    if (!g) return null;
    const vs = pbt.page.visuals(g);
    const proj = pbt.project && pbt.project.get();
    const page = proj && pbt.project.page();
    if (page) {
      const t = proj.theme;
      const found = this.compare(page.visuals, t);
      const rows = vs.map(v => {
        const m = pbt.project.match(v, page, Math.max(0.5, 1 / g.scale + 0.01));
        return { v, file: m.file, stale: !!m.reason && !!m.file, missing: !m.file, items: m.file ? found.get(m.file.name) || [] : [] };
      });
      return { mode: 'file', source: t ? (proj.themeName || 'the report theme').replace(/\.json$/i, '') : 'none in the project files', hasTheme: !!t, rows };
    }
    const list = vs.map(v => ({ v, d: this.drawn(v) }));
    const found = this.majority(list);
    return { mode: 'drawn', source: 'the page majority', hasTheme: false, rows: list.map(x => ({ v: x.v, items: found.get(x.v) || [] })) };
  },

  // What the visuals were compared with, in words.
  against(res, long) {
    if (res.mode === 'file' && res.hasTheme) return `the theme ${res.source}${long ? ', from the saved files' : ''}`;
    if (res.mode === 'file') return `the rest of the page${long ? ' (no theme file in the project; the report uses Power BI\'s built-in theme)' : ''}`;
    return long ? 'what most visuals on the page draw' : 'the page majority (no project folder set)';
  },

  // ---- drawing ----
  clear() { if (this.host) { this.host.remove(); this.host = null; } pbt.railClear('theme'); this.sig = ''; },
  // "Differs from most of the page" is grey information, shown on the badges only when the strip's
  // switch is on (themeCheck.page, off by default). Without a theme to compare with (no project
  // files, or no theme file in them) it is the whole check, so it always shows then.
  pageShown(res = this.result) { return !!res && (!res.hasTheme || !!(pbt.cfg.themeCheck || {}).page); },
  findings(row, res = this.result) { const page = this.pageShown(res); return row.items.filter(i => i.kind === 'theme' || i.kind === 'palette' || (page && i.kind === 'page')); },
  // Something to report: a finding, or a visual missing from the saved files (it could not be checked).
  reportable(row, res = this.result) { return row.missing || this.findings(row, res).length > 0; },
  // The badge's words and severity: amber for differences from the theme file, grey for the page
  // and for a visual not in the saved files, green for a pass.
  verdict(row, res = this.result) {
    const f = this.findings(row, res), th = f.filter(i => i.kind !== 'page').length, pg = f.length - th;
    const differ = (n, what) => `${n} differ${n === 1 ? 's' : ''} from ${what}`;
    if (th) return { sev: 'warn', text: pg ? `${differ(th, 'theme')} \u00b7 ${pg} from page` : differ(th, 'theme') };
    if (pg) return { sev: 'info', text: differ(pg, 'the page') };
    if (row.missing) return { sev: 'info', text: 'not saved' };
    return { sev: 'ok', text: )PBT"
LR"PBT(res.hasTheme ? '\u2713 theme' : '\u2713 page' };
  },
  fmt(v) { return typeof v === 'boolean' ? (v ? 'on' : 'off') : v == null ? 'not set' : String(v); },
  // The parts of a visual a setting shows on: the title, the axes, the legend, data labels, the
  // marks in a color, or the visual's box for its background, border and shadow.
  elsFor(v, item) {
    const el = v.el, k = String(item.key || '').toLowerCase(), q = sel => [...el.querySelectorAll(sel)];
    if (k.startsWith('subtitle')) return q('[class*="subTitle" i], [class*="subtitle" i]').slice(0, 1).concat(q('.visualTitle')).slice(0, 1);
    if (k.startsWith('title')) return q('.visualTitle').slice(0, 1);
    if (k.startsWith('categoryaxis')) return q('g.x.axis, .xAxisLabel').slice(0, 2);
    if (k.startsWith('valueaxis')) return q('g.y.axis, .yAxisLabel').slice(0, 2);
    if (k.startsWith('legend')) return q('[class*="legend" i]').slice(0, 1);
    if (k.startsWith('labels') || k.startsWith('datalabels')) return q('.labelGraphicsContext text, .label-container text, [class*="dataLabel" i]').slice(0, 20);
    if (/(color|fill)$/.test(k) && typeof item.value === 'string') {
      const want = pbt.color.parse(item.value);
      const near = c => c && want && Math.abs(c.r - want.r) + Math.abs(c.g - want.g) + Math.abs(c.b - want.b) <= 9;
      const marks = q('svg rect, svg path, svg circle').filter(n => near(pbt.color.parse(getComputedStyle(n).fill)));
      if (marks.length && !/^(background|border|dropshadow)/.test(k)) return marks.slice(0, 40);
    }
    return [el];
  },
  card(row, res) {
    const el = pbt.el, v = row.v;
    const off = this.findings(row, res);
    const info = row.items.filter(i => i.kind === 'info');
    // Page outliers while the switch is off: listed, not numbered or counted on the badge.
    const quiet = row.items.filter(i => i.kind === 'page' && !off.includes(i));
    const dot = { theme: 'grid', palette: 'edge', page: 'gap', info: 'dist' };
    // Each row carries its categories (data-cat: its kind, and "set" since every one is a setting
    // on the visual), so a pinned card's counters can filter the list. A row without a number
    // keeps the part it is about (row._pbtEls) when that is known, so pointing at it in a pinned
    // card outlines that part; the whole visual is not a known part, so it gets none.
    const line = (i, n) => {
      const r = line0(i, n);
      r.dataset.cat = `${i.kind} set`;
      if (!n) { const els = this.elsFor(v, i); if (!(els.length === 1 && els[0] === v.el)) r._pbtEls = els; }
      return r;
    };
    const line0 = (i, n) => el('div', { class: 'pbt-card-row' },
      n ? pbt.pinNum(n) : el('span', { class: `pbt-card-dot pbt-dot-${dot[i.kind]}` }),
      el('span', { text: i.kind === 'palette' ? `${i.label} ${this.fmt(i.value)}: ${i.note}.` : i.kind === 'info' ? `${i.label}: ${this.fmt(i.value)} (${i.note})` : `${i.label}: ${this.fmt(i.value)} here, ${i.kind === 'theme' ? 'theme' : 'most'} ${this.fmt(i.want)}${i.kind === 'page' ? ` (${i.note})` : ''}` }),
      i.kind === 'theme' || i.kind === 'page' ? el('span', { class: 'pbt-card-fix', text: `\u2192 ${this.fmt(i.want)}`, 'aria-label': `${i.kind === 'theme' ? 'Theme value' : 'Most of the page'}: ${this.fmt(i.want)}` }) : el('span', { class: 'pbt-card-fix' }));
    const rows = off.map((i, n) => line(i, n + 1));
    // The blue values are named in the heading, not in tooltips of their own: the card shows while
    // its badge is hovered, and a pinned card (a click on the badge) has no tooltips inside it.
    const valueIs = vals => el('span', { class: 'pbt-card-fix', text: vals.some(i => i.kind === 'theme') ? '\u2192 theme value' : vals.some(i => i.kind === 'page') ? '\u2192 most of the page' : '' });
    if (off.length) rows.unshift(el('div', { class: 'pbt-card-sub', dataset: { cat: [...new Set(off.map(i => i.kind)), 'set'].join(' ') } }, el('span', { text: 'Differences (numbered on the page)' }), valueIs(off)));
    if (!off.length) rows.push(el('div', { class: 'pbt-card-row' }, el('span', { class: 'pbt-card-dot pbt-dot-ok' }), el('span', { text: res.hasTheme ? 'Takes the theme for everything the theme sets.' : 'Matches most visuals on the page.' }), el('span')));
    if (row.missing) rows.push(el('div', { class: 'pbt-card-row' }, el('span', { class: 'pbt-card-dot pbt-dot-grid' }), el('span', { text: 'Not in the saved project files (added since the last save?); save to check it.' }), el('span')));
    if (row.stale) rows.push(el('div', { class: 'pbt-card-row' }, el('span', { class: 'pbt-card-dot pbt-dot-grid' }), el('span', { text: 'Moved since the last save; the formatting read is the saved one.' }), el('span')));
    const infoShown = info.slice(0, 4).map(i => line(i, 0));
    if (info.length > 4) infoShown.push(el('div', { class: 'pbt-card-row', dataset: { cat: 'info set' } }, el('span'), el('span', { text: `and ${info.length - 4} more set on the visual without a finding` }), el('span')));
    const n = { theme: 0, palette: 0, page: quiet.length, info: info.length };
    for (const i of off) n[i.kind]++;
    const vd = this.verdict(row, res);
    return el('div', { class: 'pbt-card' },
      el('header', {}, el('b', { text: v.title || v.kind }), v.title ? el('span', { class: 'pbt-card-type', text: v.kind }) : null,
        el('span', { class: `pbt-card-badge pbt-sev-${vd.sev}`, text: vd.text })),
      el('div', { class: 'pbt-card-kpi' }, ...[
        ['Differ from theme', n.theme, 'theme', 'settings set to a value other than the theme gives'],
        ['Off palette', n.palette, 'palette', 'colors here that are not theme colors'],
        ['Differ from page', n.page, 'page', 'settings unlike most visuals on the page (information)'],
        ['Set on this visual', n.info + off.length + quiet.length, 'set', 'every formatting setting the visual sets itself'],
      ].map(([k, x, cat, note]) => el('div', { dataset: { cat } }, el('small', { text: k }), el('strong', { text: String(x) }), el('span', { class: 'pbt-car)PBT"
LR"PBT(d-kpi-note', text: note })))),
      el('div', { class: 'pbt-card-rows' }, ...rows,
        ...(quiet.length ? [el('div', { class: 'pbt-card-sub', dataset: { cat: 'page set' }, text: 'Differs from most of the page (information)' }), ...quiet.map(i => line(i, 0))] : []),
        ...(infoShown.length ? [el('div', { class: 'pbt-card-sub', dataset: { cat: 'info set' }, text: 'Also set on this visual' }), ...infoShown] : [])),
      el('footer', {}, el('span', {}, el('b', { text: 'Compared with' }), ` ${this.against(res, true)}`)));
  },
  draw(announce) {
    const g = pbt.page.geometry();
    if (!g) { this.clear(); return; }
    const res = this.analyse();
    if (!res) { this.clear(); return; }
    const sig = JSON.stringify([g.rect.left, g.rect.top, g.rect.width, pbt.passes(), this.pageShown(res), res.mode, res.rows.map(r => [r.v.x, r.v.y, r.v.w, r.v.h, r.missing, r.stale, r.items.map(i => [i.kind, i.key, i.value, i.want])])]);
    if (sig === this.sig && this.host && this.host.isConnected) return;
    this.sig = sig;
    if (this.host) this.host.remove();
    pbt.railClear('theme');
    this.result = res;
    const el = pbt.el;
    const host = this.host = el('div', { class: 'pbt-th', 'data-pbt': '', style: `left:${g.rect.left}px;top:${g.rect.top}px;width:${g.rect.width}px;height:${g.rect.height}px` });
    const passes = pbt.passes();
    for (const row of res.rows) {
      if (!passes && !this.reportable(row, res)) continue;
      const v = row.v, vd = this.verdict(row, res);
      const b = pbt.tip(el('div', { class: `pbt-th-badge pbt-sev-${vd.sev}`, text: vd.text }), () => this.card(row, res));
      b.dataset.thVisual = v.title || v.kind;
      pbt.ownBadge(b, v);
      const findings = this.findings(row, res);
      b.addEventListener('pointerenter', () => pbt.pinpoint(findings.map((i, n) => ({ els: this.elsFor(v, i), n: n + 1 }))));
      b.addEventListener('pointerleave', () => pbt.unpinpoint());
      pbt.railPut(b, v, 'theme');
    }
    host.append(this.strip(g, res));
    document.body.append(host);
    if (announce) {
      const n = res.rows.filter(r => this.findings(r, res).length).length;
      pbt.toast(`${n} of ${res.rows.length} visuals differ from ${this.against(res)}; hover a badge for what differs`, { title: 'Theme check' });
    }
  },
  strip(g, res) {
    const el = pbt.el;
    const pageOnly = !res.hasTheme;
    const offRows = res.rows.filter(r => r.items.some(i => pageOnly ? i.kind === 'page' : i.kind === 'theme' || i.kind === 'palette'));
    const pageRows = pageOnly ? [] : res.rows.filter(r => r.items.some(i => i.kind === 'page'));
    const counts = new Map();
    // Visuals per setting (a background that is both off theme and off palette counts once).
    for (const r of res.rows) for (const l of new Set(this.findings(r, res).map(i => i.label))) counts.set(l, (counts.get(l) || 0) + 1);
    // The setting most often off fits the strip; the tooltip lists up to six with their counts.
    const order = [...counts.entries()].sort((a, b) => b[1] - a[1]);
    const ranked = order.map(([k, n]) => `${k} (${n} visual${n === 1 ? '' : 's'})`);
    const top = order.length ? order[0][0] : '';
    // The page outliers' switch, with a theme file only (without one the page is the check).
    const pageOn = this.pageShown(res);
    const pageSw = pageOnly ? null : pbt.tip(el('button', { class: `pbt-strip-switch pbt-th-page${pageOn ? ' pbt-on' : ''}`, role: 'switch', 'aria-checked': String(pageOn), onpointerdown: e => { e.stopPropagation(); e.preventDefault(); },
      onclick: e => { e.stopPropagation(); pbt.cfg.themeCheck = { ...(pbt.cfg.themeCheck || {}), page: !pageOn }; pbt.save(); this.sig = ''; this.draw(false); } },
    el('span', { class: 'pbt-switch-track' }), el('span', { text: `${pageRows.length} differ${pageRows.length === 1 ? 's' : ''} from the page` })),
    'Differs from most of the page\nSettings the theme leaves open that most visuals on the page set differently (one hidden title among shown ones). Grey information, off by default; on, they count on the badges. Saved as themeCheck.page.');
    const item = (cls, n, text, tip) => pbt.tip(el('span', { class: 'pbt-strip-item', dataset: { count: cls } }, el('span', { class: `pbt-card-dot pbt-dot-${cls}` }), n != null ? el('b', { text: String(n) }) : null, el('span', { text })), tip);
    const swallow = e => { e.stopPropagation(); e.preventDefault(); };
    const help = pbt.tip(el('button', { class: 'pbt-strip-help', text: '?', onpointerdown: swallow, onclick: e => { e.stopPropagation(); if (pbt.help) { pbt.help.close(); pbt.help.open(); pbt.help.show('theme'); } } }), 'What the badges mean\nOpens the help page at Theme check.');
    const topItem = top ? item('gap', null, `most often: ${top}`, ['The settings visuals on this page most often set differently', ...ranked.slice(0, 6)].join('\n')) : null;
    if (topItem) topItem.classList.add('pbt-th-top');
    const passes = pbt.passItems(res.rows.filter(r => !this.reportable(r, res)).length, res.rows.length, `Nothing set differently from ${this.against(res)}.`);
    const bar = el('div', { class: 'pbt-qa-strip pbt-th-strip' },
      item('grid', `${offRows.length} of ${res.rows.length}`, pageOnly ? 'differ from the page' : 'differ from theme', `Visuals that set something differently from ${pageOnly ? 'most of the page' : 'the theme file'}\nHover a visual's badge for the list.`),
      item('dist', null, res.mode === 'file' && res.hasTheme ? `theme: ${res.source}` : 'page majority', res.mode === 'file' && res.hasTheme ? 'Compared with the report theme, from the saved project files\nSave the report to check changes made since.' : res.mode === 'file' ? 'The project files have no theme file (the report uses Power BI\'s built-in theme), so each visual\'s saved formatting is compared with the rest of the page.' : 'No project folder set, so each visual is compared with what most visuals on the page draw\nSet the report project folder for a check against the theme its)PBT"
LR"PBT(elf.'),
      topItem,
      passes.fine,
      pageSw,
      el('span', { class: 'pbt-strip-seg' }, passes.sw, help));
    // On a narrow dock the theme's name and the setting most often off give way first (pbt.dock.place).
    for (const e of bar.querySelectorAll('[data-count="dist"], .pbt-th-top')) e.dataset.opt = '';
    pbt.placeStrip(bar, 'theme', g);
    return bar;
  },

  toggle() {
    this.on = !this.on;
    pbt.restack();
    if (!this.on) { this.clear(); this.result = null; pbt.toast('Theme check off'); return; }
    this.draw(true);
  },
  start() {
    pbt.hotkey('t', 'Theme check', () => this.toggle(), {
      id: 'theme',
      tool: true, active: () => this.on,
      desc: 'A badge on every visual that sets formatting differently from the report theme ("3 differ from theme"), colors outside the theme included; settings that differ from most of the page are grey information, off by default. Uses the report project folder; without it, compares visuals with each other.',
      tip: 'Theme check\nReads each visual\'s saved formatting from the report project folder and compares it with the theme; without the folder it compares what each visual draws with most of the page.',
    });
    pbt.every(600, () => { if (this.on) this.draw(false); });
  },
  stop() { this.clear(); },
};
pbt.themecheck = theme;
pbt.module(theme);

// ---- 46-colors.js ----
// Colors (Ctrl+Alt+L, Ctrl+Alt+A opens it too): the two questions a page's colors raise, in one
// badge per visual, one card and one strip.
//
// Hard to read: every run of text in a visual against the color actually behind it (WCAG 2.1,
// 1.4.3: AA 4.5:1, or 3:1 for large text; AAA 7:1 and 4.5:1), and bars, lines and points against
// the visual's background (1.4.11, 3:1).
// Off palette: the colors each visual actually drew (SVG fills and strokes, text, backgrounds)
// against the palette: the report theme's colors when the project files are there, else
// lint.palette, each channel within lint.tolerance. Greys, white and black always pass. Fonts are
// listed too, and checked when lint.fonts names the allowed families.
//
// What is behind a piece of text is found by hit-testing under its first line of glyphs: every
// painted layer down to the first opaque one counts (the text's own cell fill, the bar under a
// data label, the visual's background, the page), on the fill or the outline the point falls on,
// composited as the browser paints them (a see-through ancestor shared with the text blends the
// two together). Power BI turns pointer events off on many SVG parts, which hit-testing would
// skip, so the scan switches them on inside the canvas for its duration (html.pbt-ct-probe).
// Text over an image, a canvas or a gradient, or outside the window, is listed as not checked,
// never guessed; a visual with nothing measured gets no contrast verdict.
//
// The palette scan reads only what the report paints: the visual header (its icons take the
// app's colors, which the skin repaints), our own overlays, Power BI's invisible hit targets and
// anything drawn fully see-through are left out, so the verdict is the same in every skin.

const CT_LARGE_PX = 24, CT_LARGE_BOLD_PX = 18.66;
const CT_SHAPES = /^(rect|path|circle|ellipse|polygon|polyline|line)$/i;
// Power BI's clearCatcher: an invisible rect over a plot that takes its clicks.
const CT_CATCHER = /clearCatcher/;
// Elements read per visual; a larger one (a long table) is checked in part and says so.
const CT_NODES = 20000;
// Elements the palette scan reads per visual.
const CO_NODES = 4000;
// Card rows per section; the rest are counted.
const CO_ROWS = 8;
// The visual header's own parts (its icons and the "Press Enter to edit" hint). The container
// itself carries classes like visualHeaderAbove, so only the exact token counts.
const CT_HEADER = /(^|\s)(visualHeader|vcHeader|visualsEnterHint)(\s|$)/;
const ctHeaderEl = e => CT_HEADER.test(typeof e.className === 'string' ? e.className : '') || e.tagName === 'VISUAL-HEADER';
// Inside a visual's header, up to the visual.
const ctInHeader = (n, root) => { for (let e = n; e && e !== root; e = e.parentElement) if (ctHeaderEl(e)) return true; return false; };
// Not a chart mark: axes, ticks, gridlines, the legend and data labels, by exact class token (the
// whole plot sits in g.axisGraphicsContext, so a substring would drop the bars too).
const ctNotMark = (n, root) => {
  for (let e = n; e && e !== root; e = e.parentElement) {
    const cls = (e.getAttribute && e.getAttribute('class')) || '';
    if (cls.split(/\s+/).some(t => /^(axis|tick|ticks|gridlines?|legend.*|labels?|label-container|data-?labels?)$/i.test(t))) return true;
  }
  return false;
};
// Left out of the palette scan with everything inside: the header, our overlays, tooltips held in
// <title>, the filter icon Power BI adds, and its invisible hit targets (the line chart's
// interactivity line is a 10 px stroke at opacity 0 in the app's white, which a skin recolours).
const coSkip = n => ctHeaderEl(n) || n.hasAttribute('data-pbt') || n.tagName.toLowerCase() === 'title'
  || /(^|\s)(clearCatcher|interactivity-line|vcFilterRestatementBtn)(\s|$)/.test(n.getAttribute('class') || '');
// A CSS number, with a default only when it is missing (an opacity of 0 is 0).
const ctNum = (v, d) => { const n = parseFloat(v); return Number.isNaN(n) ? d : n; };
const coFont = cs => cs.fontFamily.split(',')[0].replace(/["']/g, '').trim();

const CO_SEV = { bad: 'bad', warn: 'warn', ok: 'ok', none: 'info' };
const colors = {
  name: 'colors',
  on: false,
  host: null,
  sig: '',
  result: null,

  // ---- pure core (fixtures in scripts/tool-check.mjs) ----
  cfgNow() { const c = pbt.cfg.contrast || {}; return { level: c.level === 'AAA' ? 'AAA' : 'AA', marks: c.marks !== false }; },
  ratio(a, b) { return pbt.color.contrast(a, b); },
  // Layers from the bottom up ({ r, g, b, a }), the bottom one taken as opaque: what the eye sees.
  composite(layers) {
    if (!layers.leng)PBT"
LR"PBT(th) return { r: 255, g: 255, b: 255, a: 1 };
    let out = { ...layers[0], a: 1 };
    for (const l of layers.slice(1)) out = { r: l.r * l.a + out.r * (1 - l.a), g: l.g * l.a + out.g * (1 - l.a), b: l.b * l.a + out.b * (1 - l.a), a: 1 };
    return out;
  },
  isLarge(px, weight) { return px >= CT_LARGE_PX || (px >= CT_LARGE_BOLD_PX && weight >= 700); },
  target(level, large, kind) {
    if (kind === 'mark') return 3;
    return level === 'AAA' ? (large ? 4.5 : 7) : (large ? 3 : 4.5);
  },
  // One visual's findings: each text pair or mark color (fg and bg as the eye sees them) with its
  // ratio and target, failing first; items that could not be measured are left out.
  judge(items, cfg) {
    const out = items.filter(i => !i.skip && (i.kind !== 'mark' || cfg.marks)).map(i => {
      const r = this.ratio(i.fg, i.bg);
      const need = this.target(cfg.level, i.large, i.kind);
      return { ...i, ratio: r, need, pass: r >= need };
    });
    return out.sort((a, b) => (a.pass - b.pass) || (a.ratio / a.need) - (b.ratio / b.need));
  },

  // ---- the palette ----
  // The report theme's colors when the project files are there (its data colors and the good,
  // neutral, bad, foreground, background and accent colors), unless lint.fromTheme is false;
  // else the palette in the settings.
  themePalette() {
    const t = pbt.project && (pbt.cfg.lint || {}).fromTheme !== false && pbt.project.theme();
    if (!t) return null;
    const keys = ['good', 'neutral', 'bad', 'maximum', 'center', 'minimum', 'foreground', 'foregroundNeutralSecondary', 'foregroundNeutralDark', 'background', 'backgroundLight', 'backgroundNeutral', 'backgroundDark', 'tableAccent', 'hyperlink', 'visitedHyperlink'];
    const list = [...(t.dataColors || []), ...keys.map(k => t[k]).filter(c => typeof c === 'string')];
    return list.length ? list : null;
  },
  themeName() { return pbt.project.get().themeName || 'custom theme'; },
  source() { return this.themePalette() ? `the report theme (${this.themeName()})` : 'the palette in the settings'; },
  // The same in the strip's few words.
  sourceShort() { return this.themePalette() ? `theme: ${this.themeName().replace(/\.json$/i, '')}` : 'palette in the settings'; },
  palette() {
    const cfg = pbt.cfg.lint || {};
    return (this.themePalette() || cfg.palette || []).map(pbt.color.parse).filter(Boolean);
  },
  tolerance() { return (pbt.cfg.lint && pbt.cfg.lint.tolerance) || 12; },
  // Colors and fonts one visual draws: { colors: Map(hex -> Set of uses), fonts, els: Map(hex ->
  // elements) }. Parents come before children in document order, so whether a subtree is left out
  // and how see-through it is are carried down without walking up from every element.
  scan(el) {
    const found = new Map(), fonts = new Set(), els = new Map();
    const add = (value, alpha, where, node) => {
      const c = pbt.color.parse(value);
      if (!c || c.a * alpha < 0.15) return;
      if (pbt.color.chroma(c) < 0.1) return; // neutral
      const key = pbt.color.hex({ ...c, a: 1 });
      if (!found.has(key)) { found.set(key, new Set()); els.set(key, []); }
      found.get(key).add(where);
      if (node && els.get(key).length < 40) els.get(key).push(node);
    };
    const state = new Map([[el, 1]]); // element -> opacity, 0 when left out or not drawn
    const nodes = el.querySelectorAll('*');
    for (let i = 0; i < nodes.length && i < CO_NODES; i++) {
      const n = nodes[i];
      const up = state.has(n.parentElement) ? state.get(n.parentElement) : 1;
      if (!up || coSkip(n)) { state.set(n, 0); continue; }
      const cs = getComputedStyle(n);
      const op = cs.display === 'none' ? 0 : up * ctNum(cs.opacity, 1);
      state.set(n, op);
      if (op < 0.05 || cs.visibility === 'hidden') continue;
      if (n instanceof SVGElement) {
        if (/^(path|rect|circle|ellipse|line|polyline|polygon|text|tspan)$/.test(n.tagName)) {
          if (cs.fill && cs.fill !== 'none' && !cs.fill.startsWith('url')) add(cs.fill, op * ctNum(cs.fillOpacity, 1), 'fill', n);
          if (cs.stroke && cs.stroke !== 'none' && !cs.stroke.startsWith('url') && ctNum(cs.strokeWidth, 1) > 0) add(cs.stroke, op * ctNum(cs.strokeOpacity, 1), 'stroke', n);
        }
        if (n.tagName === 'text' && n.textContent.trim()) fonts.add(coFont(cs));
        continue;
      }
      if ([...n.childNodes].some(c => c.nodeType === 3 && c.textContent.trim())) {
        add(cs.color, op, 'text', n);
        fonts.add(coFont(cs));
      }
      if (cs.backgroundColor !== 'rgba(0, 0, 0, 0)') add(cs.backgroundColor, op, 'background', n);
    }
    return { colors: found, fonts, els };
  },
  near(c, pal = this.palette(), tol = this.tolerance()) { return pal.some(p => Math.max(Math.abs(p.r - c.r), Math.abs(p.g - c.g), Math.abs(p.b - c.b)) <= tol); },
  offPalette(found) {
    const pal = this.palette(), tol = this.tolerance();
    return [...found.keys()].filter(k => !this.near(pbt.color.parse(k), pal, tol));
  },
  // The palette color closest to a stray one (largest channel difference), for the card. A stray
  // color is never neutral, so the palette's own colors are offered before its greys.
  nearest(hex) {
    const c = pbt.color.parse(hex), all = this.palette(), hued = all.filter(p => pbt.color.chroma(p) >= 0.1), pal = hued.length ? hued : all;
    let best = null, d = Infinity;
    for (const p of pal) { const x = Math.max(Math.abs(p.r - c.r), Math.abs(p.g - c.g), Math.abs(p.b - c.b)); if (x < d) { d = x; best = p; } }
    return best ? pbt.color.hex({ ...best, a: 1 }) : '';
  },
  allowedFonts() { return ((pbt.cfg.lint && pbt.cfg.lint.fonts) || []).map(f => f.toLowerCase()); },

  // ---- reading the page for contrast ----
  // One painted color over another, both possibly see-through (source over).
  over(top, bottom) {
    const a = top.a + bottom.a * (1 - top.a);
    if (a <= 0) return { r: 0, g: 0, b: 0, a: 0 };
    const ch = k => (top[k] * top.a + bottom[k] * bottom.a * (1 - top.a)) / a;
    return {)PBT"
LR"PBT( r: ch('r'), g: ch('g'), b: ch('b'), a };
  },
  // The two colors the eye compares: the text (or mark) over its background, and the background
  // alone. Layers come top first; those inside `group` (a see-through ancestor shared with the
  // text) are composited with the text first and take the group's opacity together, as the
  // browser paints them.
  paint(top, layers, groupAlpha = 1) {
    let outer = { r: 255, g: 255, b: 255, a: 1 }, inner = { r: 0, g: 0, b: 0, a: 0 };
    for (const l of [...layers].reverse()) {
      if (l.inner) inner = this.over(l, inner);
      else outer = this.over(l, outer);
    }
    const flat = c => this.over({ ...c, a: c.a * groupAlpha }, outer);
    return { fg: flat(this.over(top, inner)), bg: flat(inner) };
  },
  opacity(el, stop) {
    let o = 1;
    for (let n = el; n && n !== stop && n.nodeType === 1; n = n.parentElement) o *= ctNum(getComputedStyle(n).opacity, 1);
    return o;
  },
  // The outermost see-through ancestor of `el` inside the visual, if any: its content (the text
  // and what it sits on) is blended as one with what lies under it.
  group(el, stop) {
    let g = null;
    for (let n = el.parentElement; n && n !== stop; n = n.parentElement) if (ctNum(getComputedStyle(n).opacity, 1) < 0.999) g = n;
    return g;
  },
  // The painted layers under a point, top first, skipping `skip(el)` elements and our own
  // overlays: { layers } or { image: true } (an image, a canvas or a gradient is painted there) or
  // { unseen: true } (the point is outside the window, so nothing can be read there).
  behind(x, y, skip, container, group) {
    if (x < 0 || y < 0 || x >= innerWidth || y >= innerHeight) return { unseen: true };
    const layers = [];
    const alpha = e => (group && group.contains(e) ? this.opacity(e, group) : this.opacity(e, container));
    for (const e of document.elementsFromPoint(x, y)) {
      if (e.closest('[data-pbt]') || skip(e) || CT_CATCHER.test(e.getAttribute('class') || '')) continue;
      const cs = getComputedStyle(e), tag = e.tagName.toLowerCase();
      let c = null, a = 1;
      if (e instanceof SVGElement) {
        if (tag === 'image') return { image: true };
        if (!CT_SHAPES.test(tag)) continue;
        // The point may be on the shape's fill or on its outline (a gridline is all outline).
        let inFill = tag !== 'line' && tag !== 'polyline', inStroke = false;
        try {
          const m = e.getScreenCTM(), p = m && new DOMPoint(x, y).matrixTransform(m.inverse());
          if (p && e.isPointInFill) inFill = tag !== 'line' && e.isPointInFill(p);
          if (p && e.isPointInStroke && cs.stroke !== 'none') inStroke = e.isPointInStroke(p);
        } catch {}
        const paint = inStroke && (!inFill || cs.fill === 'none') ? cs.stroke : inFill ? cs.fill : 'none';
        if (paint === 'none') continue;
        if (/url\(/.test(paint)) return { image: true };
        c = pbt.color.parse(paint);
        a = paint === cs.stroke && inStroke && !inFill ? ctNum(cs.strokeOpacity, 1) : ctNum(cs.fillOpacity, 1);
      } else {
        if (tag === 'img' || tag === 'canvas' || tag === 'video' || (cs.backgroundImage && cs.backgroundImage !== 'none')) return { image: true };
        c = pbt.color.parse(cs.backgroundColor);
      }
      if (!c) continue;
      const layer = { ...c, a: c.a * a * alpha(e), inner: !!(group && group.contains(e)) };
      if (layer.a < 0.02) continue;
      layers.push(layer);
      if (layer.a >= 0.99 && !layer.inner) break;
    }
    return { layers };
  },
  // Where a piece of text is drawn: the first line box of its own text, so a short label in a wide
  // cell is sampled under its glyphs, not at the middle of the cell.
  textBox(n) {
    if (n instanceof SVGElement) return n.getBoundingClientRect();
    const t = [...n.childNodes].find(c => c.nodeType === 3 && c.textContent.trim());
    if (!t) return n.getBoundingClientRect();
    const range = document.createRange();
    range.selectNodeContents(t);
    const rects = [...range.getClientRects()].filter(r => r.width >= 1 && r.height >= 1);
    return rects[0] || n.getBoundingClientRect();
  },
  // How much larger than its CSS size an element is drawn (Power BI scales the canvas): the size
  // the eye sees, in screen pixels, for one CSS pixel of it.
  drawScale(n) {
    if (n instanceof SVGElement) { const m = n.getScreenCTM(); return m ? Math.hypot(m.a, m.b) || 1 : 1; }
    return n.offsetWidth ? n.getBoundingClientRect().width / n.offsetWidth || 1 : 1;
  },
  // What a run of text is, in plain words, from the classes around it (Power BI names its parts:
  // legend, axis, labels, title, cells).
  roleOf(n, v, px) {
    const toks = [];
    for (let e = n; e && e !== v.el; e = e.parentElement) toks.push(...String((e.getAttribute && e.getAttribute('class')) || '').toLowerCase().split(/\s+/));
    const has = re => toks.some(t => re.test(t));
    const kind = String(v.kind || '').toLowerCase();
    if (has(/legend/)) return 'Legend';
    if (has(/^(xaxislabel|yaxislabel|axistitle|axis-title)$/) || (has(/axis/) && has(/title/))) return 'Axis title';
    if (has(/axis|tick/)) return 'Axis label';
    if (has(/^(labels?|label-container|data-?labels?)$/)) return 'Data label';
    if (has(/title/)) return 'Title';
    if (/table|matrix/.test(kind)) return has(/header/) ? 'Table header' : 'Table cell';
    if (/slicer/.test(kind)) return 'Slicer item';
    if (/card|kpi/.test(kind)) return px >= 20 ? 'Card value' : 'Card label';
    if (/text ?box/.test(kind)) return 'Text';
    return 'Text';
  },
  // What a chart's marks are called, from the visual's type and the shape.
  markRole(v, n, count) {
    const k = String(v.kind || '').toLowerCase(), many = count !== 1;
    const name = /column/.test(k) ? 'column' : /bar/.test(k) ? 'bar' : /area/.test(k) ? 'area' : /line/.test(k) ? 'line' : /scatter|point|bubble/.test(k) ? 'point' : /pie|donut/.test(k) ? 'slice' : /map/.test(k) ? 'shape' : 'shape';
    return `${count} ${name})PBT"
LR"PBT(${many ? 's' : ''}`;
  },
  clampTo(r, vr) {
    return [Math.min(Math.max(r.left + r.width / 2, vr.left + 1), vr.right - 1), Math.min(Math.max(r.top + r.height / 2, vr.top + 1), vr.bottom - 1)];
  },
  // Every visible run of text in a visual: HTML elements with their own text, SVG <text> and
  // <tspan>. { items, truncated } (a visual larger than CT_NODES elements is checked in part).
  texts(v, g) {
    const items = [], vr = v.el.getBoundingClientRect(), stop = v.el.parentElement;
    const nodes = v.el.querySelectorAll('*');
    for (let i = 0; i < nodes.length && i < CT_NODES; i++) {
      const n = nodes[i];
      const svg = n instanceof SVGElement, tag = n.tagName.toLowerCase();
      // An element's own text: HTML elements, SVG <text> and <tspan> (Power BI puts a label's color
      // on the tspan); a <title> is a tooltip, not drawn. The cheap tests come first.
      if (svg && tag !== 'text' && tag !== 'tspan') continue;
      const text = [...n.childNodes].filter(c => c.nodeType === 3).map(c => c.textContent).join('').trim();
      if (!text || /^Select or drag fields/i.test(text) || n.closest('title, [data-pbt], .vcFilterRestatementBtn') || ctInHeader(n, v.el)) continue;
      const cs = getComputedStyle(n);
      if (cs.visibility === 'hidden' || cs.display === 'none') continue;
      const r = this.textBox(n);
      if (r.width < 2 || r.height < 2 || r.right < vr.left || r.left > vr.right || r.bottom < vr.top || r.top > vr.bottom) continue;
      const fg = pbt.color.parse(svg ? cs.fill : cs.color);
      if (!fg || (svg && cs.fill === 'none') || /url\(/.test(cs.fill)) continue;
      const grp = this.group(n, stop);
      const a = fg.a * (svg ? ctNum(cs.fillOpacity, 1) : 1) * this.opacity(n, grp || stop);
      if (a < 0.05) continue;
      // Large text is judged at the report's own size: the drawn size over the page zoom, to 0.1 px
      // (the two scales differ in the 7th decimal, which made a 24 px title 23.99999 px).
      const px = Math.round((parseFloat(cs.fontSize) || 12) * this.drawScale(n) / (g.scale || 1) * 10) / 10, weight = parseInt(cs.fontWeight, 10) || 400;
      const [cx, cy] = this.clampTo(r, vr);
      // The text's own element counts as background (a table cell's fill); other text does not.
      const back = this.behind(cx, cy, e => (e !== n && n.contains(e)) || (e instanceof SVGElement && /^(text|tspan)$/i.test(e.tagName)), stop, grp);
      const label = text.length > 40 ? `${text.slice(0, 39)}\u2026` : text;
      if (back.image || back.unseen) { items.push({ kind: 'text', text: label, skip: back.image ? 'image' : 'unseen' }); continue; }
      const { fg: fgEff, bg } = this.paint({ ...fg, a }, back.layers, grp ? this.opacity(grp, stop) : 1);
      items.push({ kind: 'text', text: label, fg: fgEff, bg, large: this.isLarge(px, weight), px, weight, role: this.roleOf(n, v, px), els: [n] });
    }
    return { items, truncated: nodes.length > CT_NODES };
  },
  // Chart marks: SVG shapes with a fill or a stroke, of some size, outside axes, gridlines, the
  // legend and labels; one entry per color, against what is behind the largest visible mark of it.
  // A shape covering most of its plot is the plot's background, not a mark.
  marks(v, g) {
    const vr = v.el.getBoundingClientRect(), stop = v.el.parentElement, s2 = (g.scale || 1) ** 2, byColor = new Map(), all = new Set();
    for (const n of v.el.querySelectorAll('svg *')) {
      if (!CT_SHAPES.test(n.tagName) || CT_CATCHER.test(n.getAttribute('class') || '') || n.closest('[data-pbt]') || ctNotMark(n, v.el) || ctInHeader(n, v.el)) continue;
      const cs = getComputedStyle(n);
      if (cs.visibility === 'hidden' || cs.display === 'none') continue;
      const r = n.getBoundingClientRect();
      if (r.right < vr.left || r.left > vr.right || r.bottom < vr.top || r.top > vr.bottom) continue;
      const plot = n.ownerSVGElement && n.ownerSVGElement.getBoundingClientRect();
      if (plot && plot.width * plot.height > 0 && r.width * r.height >= 0.85 * plot.width * plot.height) continue;
      const fill = cs.fill !== 'none' && !/url\(/.test(cs.fill) && ctNum(cs.fillOpacity, 1) > 0.05 && pbt.color.parse(cs.fill);
      const stroke = cs.stroke !== 'none' && !/url\(/.test(cs.stroke) && ctNum(cs.strokeWidth, 0) >= 1 && ctNum(cs.strokeOpacity, 1) > 0.05 && pbt.color.parse(cs.stroke);
      const filled = fill && fill.a > 0.05 && r.width * r.height >= 36 * s2 && r.width >= 2 && r.height >= 2;
      const c = filled ? fill : stroke && stroke.a > 0.05 && (r.width >= 8 * Math.sqrt(s2) || r.height >= 8 * Math.sqrt(s2)) ? stroke : null;
      if (!c) continue;
      all.add(n);
      const key = pbt.color.hex({ ...c, a: 1 });
      if (!byColor.has(key)) byColor.set(key, []);
      byColor.get(key).push({ n, r, c, own: c.a * (filled ? ctNum(cs.fillOpacity, 1) : ctNum(cs.strokeOpacity, 1)) });
    }
    const items = [];
    for (const [key, shapes] of byColor) {
      // The largest shape of this color that is actually visible (an opacity of 0 anywhere up its
      // ancestors hides it); opacity is read only for these few candidates.
      shapes.sort((p, q) => q.r.width * q.r.height - p.r.width * p.r.height);
      let pick = null;
      for (const s of shapes.slice(0, 6)) { const a = s.own * this.opacity(s.n, stop); if (a >= 0.05) { pick = { ...s, a }; break; } }
      if (!pick) continue;
      const [cx, cy] = this.clampTo(pick.r, vr);
      const back = this.behind(cx, cy, e => all.has(e), stop, null);
      if (back.image || back.unseen) { items.push({ kind: 'mark', text: key, skip: back.image ? 'image' : 'unseen' }); continue; }
      const { fg, bg } = this.paint({ ...pick.c, a: pick.a }, back.layers);
      items.push({ kind: 'mark', text: key, fg, bg, large: false, role: this.markRole(v, pick.n, shapes.length), els: shapes.map(x => x.n) });
    }
    return { items };
  },
  // Every visual on the page: its contrast findings and what it draws against the palette.
  analyse() {
    const g = )PBT"
LR"PBT(pbt.page.geometry();
    if (!g) return null;
    const cfg = this.cfgNow(), allowed = this.allowedFonts();
    const root = document.documentElement;
    root.classList.add('pbt-ct-probe');
    try {
      const rows = pbt.page.visuals(g).map(v => {
        const t = this.texts(v, g), m = cfg.marks ? this.marks(v, g) : { items: [] };
        const items = [...t.items, ...m.items];
        const pal = this.scan(v.el);
        return {
          v, judged: this.judge(items, cfg), truncated: t.truncated,
          skipped: items.filter(i => i.skip === 'image').length, unseen: items.filter(i => i.skip === 'unseen').length,
          texts: t.items.length,
          colors: pal.colors, fonts: pal.fonts, els: pal.els,
          off: this.offPalette(pal.colors),
          fontsOff: allowed.length ? [...pal.fonts].filter(f => !allowed.includes(f.toLowerCase())) : [],
        };
      });
      return { g, cfg, rows };
    } finally { root.classList.remove('pbt-ct-probe'); }
  },

  // ---- drawing ----
  // The contrast half of a visual's verdict: how many fall short, or "meets", or nothing measured
  // ("not checked"), or measured in part (a long table, text outside the window).
  verdict(row, cfg) {
    const fails = row.judged.filter(j => !j.pass).length;
    if (fails) return `${fails} below ${cfg.level}`;
    if (!row.judged.length) return 'not checked';
    return row.truncated || row.unseen || row.skipped ? `partly checked` : `meets ${cfg.level}`;
  },
  fails(row) { return row.judged.filter(j => !j.pass); },
  // Something to report: a pair below the level, a color off the palette, or a font outside the
  // allowed families. Not checked or partly checked is told on the card.
  reportable(row) { return this.fails(row).length > 0 || row.off.length > 0 || row.fontsOff.length > 0; },
  // The numbered findings, one numbering across both sections (the card's rows and the pins).
  numbered(row) {
    const hard = this.fails(row).slice(0, CO_ROWS).map((j, i) => ({ j, n: i + 1 }));
    const off = row.off.slice(0, CO_ROWS).map((hex, i) => ({ hex, n: hard.length + i + 1 }));
    return { hard, off };
  },
  // The badge's words: "1 hard to read \u00b7 2 off palette", or what is fine when there is nothing.
  label(row, cfg) {
    const fails = this.fails(row).length, parts = [];
    if (fails) parts.push(`${fails} hard to read`);
    if (row.off.length) parts.push(`${row.off.length} off palette`);
    if (row.fontsOff.length) parts.push(`${row.fontsOff.length} other font${row.fontsOff.length === 1 ? '' : 's'}`);
    if (parts.length) return parts.join(' \u00b7 ');
    const v = this.verdict(row, cfg);
    return v === 'not checked' ? 'contrast not checked' : v === 'partly checked' ? 'partly checked' : '\u2713 colors';
  },
  // Red when anything is hard to read, amber when only off palette, green when all is measured and
  // fine, grey when the text could not be measured (or only in part). CO_SEV maps the tone to the
  // shared severity classes every check tool's badges use.
  tone(row, cfg) {
    if (this.fails(row).length) return 'bad';
    if (row.off.length || row.fontsOff.length) return 'warn';
    return this.label(row, cfg) === '\u2713 colors' ? 'ok' : 'none';
  },
  highlight(row) {
    const { hard, off } = this.numbered(row);
    pbt.pinpoint([...hard.map(x => ({ els: x.j.els, n: x.n })), ...off.map(x => ({ els: row.els.get(x.hex) || [], n: x.n }))]);
  },
  unhighlight() { pbt.unpinpoint(); },
  swatch(c) { return pbt.el('span', { class: 'pbt-ct-sw', style: `background:${pbt.color.hex({ ...c, a: 1 })}` }); },
  card(row, cfg) {
    const el = pbt.el, v = row.v;
    const fails = this.fails(row), { hard, off } = this.numbered(row);
    const lowest = row.judged.length ? Math.min(...row.judged.map(j => j.ratio)) : null;
    const hex = c => pbt.color.hex({ ...c, a: 1 });
    // Rows carry their section (data-cat: hard or off), so a pinned card's counters can filter the list.
    const note = (dot, text, cat) => el('div', { class: 'pbt-card-row', dataset: cat ? { cat } : {} }, el('span', { class: `pbt-card-dot pbt-dot-${dot}` }), el('span', { text }), el('span'));
    const tone = this.tone(row, cfg);
    const rows = [el('div', { class: 'pbt-card-sub', dataset: { cat: 'hard' }, text: 'Hard to read' })];
    for (const { j, n } of hard) rows.push(el('div', { class: 'pbt-card-row', dataset: { cat: 'hard' } }, pbt.pinNum(n),
      el('span', { class: 'pbt-ct-pair' }, this.swatch(j.fg), this.swatch(j.bg),
        el('span', { text: j.kind === 'mark' ? `${j.role[0].toUpperCase()}${j.role.slice(1)} in ${j.text} on ${hex(j.bg)}` : `${j.role} "${j.text}"${j.large ? ' (large)' : ''} on ${hex(j.bg)}` })),
      el('span', { class: 'pbt-card-fix', text: `${j.ratio.toFixed(2)}:1 \u2192 ${j.need}:1` })));
    if (fails.length > CO_ROWS) rows.push(note('grid', `and ${fails.length - CO_ROWS} more`, 'hard'));
    if (!fails.length) rows.push(note(row.judged.length ? 'ok' : 'grid', row.judged.length ? `Every text${cfg.marks ? ' and bar, line and point' : ''} measured meets ${cfg.level}.` : 'Nothing measurable here (text drawn as an image or in a canvas cannot be read).', 'hard'));
    if (row.skipped) rows.push(note('grid', `${row.skipped} over an image, a canvas or a gradient: not checked.`));
    if (row.unseen) rows.push(note('grid', `${row.unseen} outside the window: scroll them into view to check.`));
    if (row.truncated) rows.push(note('grid', `A large visual: its first ${CT_NODES.toLocaleString()} elements were checked.`));
    rows.push(el('div', { class: 'pbt-card-sub', dataset: { cat: 'off' }, text: 'Off palette' }));
    for (const { hex: h, n } of off) {
      const near = this.nearest(h);
      const nearC = near && pbt.color.parse(near);
      rows.push(el('div', { class: 'pbt-card-row', dataset: { cat: 'off' } }, pbt.pinNum(n),
        el('span', { class: 'pbt-ct-pair' }, this.swatch(pbt.color.parse(h)), el('span', { text: `${h} as ${[...row.colors.get(h)].join(' and ')}` })),
      )PBT"
LR"PBT(  // The nearest palette color as a swatch too, so the two can be compared at a glance.
        el('span', { class: 'pbt-card-fix' }, ...(near ? [nearC ? this.swatch(nearC) : null, ` nearest ${near}`] : []))));
    }
    if (row.off.length > CO_ROWS) rows.push(note('grid', `and ${row.off.length - CO_ROWS} more`, 'off'));
    if (!row.off.length) rows.push(note('ok', `Every color is within ${this.tolerance()} of a palette color.`, 'off'));
    if (row.fontsOff.length) rows.push(note('edge', `Fonts outside ${(pbt.cfg.lint.fonts || []).join(', ')}: ${row.fontsOff.join(', ')}`));
    const drawn = [...row.colors.keys()];
    rows.push(note('grid', `Drawn: ${drawn.length ? drawn.slice(0, 10).join(', ') + (drawn.length > 10 ? ` and ${drawn.length - 10} more` : '') : 'neutrals only'}; fonts ${[...row.fonts].join(', ') || 'none'}`));
    return el('div', { class: 'pbt-card' },
      el('header', {}, el('b', { text: v.title || v.kind }), v.title ? el('span', { class: 'pbt-card-type', text: v.kind }) : null,
        el('span', { class: `pbt-card-badge pbt-sev-${CO_SEV[tone]}`, text: this.label(row, cfg) })),
      el('div', { class: 'pbt-card-kpi' }, ...[
        ['Hard to read', fails.length, 'hard', `below the ${cfg.level} contrast it needs`],
        ['Off palette', row.off.length, 'off', 'colors not within reach of a palette color'],
        ['Lowest', lowest == null ? '-' : `${lowest.toFixed(1)}:1`, '', 'lowest contrast measured here'],
        ['Texts', row.texts, '', 'runs of text found and read'],
      ].map(([k, x, cat, about]) => el('div', { dataset: cat ? { cat } : {} }, el('small', { text: k }), el('strong', { text: String(x) }), el('span', { class: 'pbt-card-kpi-note', text: about })))),
      el('div', { class: 'pbt-card-rows' }, ...rows),
      el('footer', {}, el('span', {}, el('b', { text: cfg.level }), cfg.level === 'AAA' ? ' text 7:1, large text 4.5:1' : ' text 4.5:1, large text 3:1', cfg.marks ? ', bars, lines and points 3:1' : '', `; palette: ${this.sourceShort()}, within ${this.tolerance()}`)));
  },
  strip(g, res) {
    const el = pbt.el, cfg = res.cfg;
    const hard = res.rows.reduce((n, r) => n + this.fails(r).length, 0);
    const hardVis = res.rows.filter(r => this.fails(r).length).length;
    const textFails = res.rows.reduce((n, r) => n + r.judged.filter(j => !j.pass && j.kind === 'text').length, 0);
    const offCount = res.rows.reduce((n, r) => n + r.off.length, 0);
    const offVis = res.rows.filter(r => r.off.length).length;
    const swallow = e => { e.stopPropagation(); e.preventDefault(); };
    const button = (attrs, run, tip) => pbt.tip(el('button', { ...attrs, onpointerdown: swallow, onclick: e => { e.stopPropagation(); run(); } }), tip);
    const item = (cls, n, text, tip) => pbt.tip(el('span', { class: 'pbt-strip-item', dataset: { count: cls } }, el('span', { class: `pbt-card-dot pbt-dot-${cls}` }), n != null ? el('b', { text: String(n) }) : null, el('span', { text })), tip);
    const passes = pbt.passItems(res.rows.filter(r => !this.reportable(r)).length, res.rows.length, `Nothing measured falls below ${cfg.level} and every color is on the palette. Visuals not checked or checked in part say so on their card.`);
    const set = (k, val) => { pbt.cfg.contrast = { ...cfg, [k]: val }; pbt.save(); this.sig = ''; this.draw(false); };
    const pal = this.palette();
    // The counts are of pairs and colors, so the visuals they fall in are named beside them.
    const inVis = (n, v) => n ? ` in ${v} visual${v === 1 ? '' : 's'}` : '';
    const bar = el('div', { class: 'pbt-qa-strip pbt-co-strip' },
      item('vis', null, this.themePalette() ? 'theme palette' : 'settings palette', `Palette: ${this.source()}\n${pal.length} color${pal.length === 1 ? '' : 's'}, each channel within ${this.tolerance()}; greys, white and black always pass. ${this.themePalette() ? 'lint.fromTheme false uses the palette in the settings instead.' : 'With the report project folder set, the theme\'s colors are used.'}`),
      item('edge', hard, `hard to read${inVis(hard, hardVis)}`, `Texts${cfg.marks ? ', bars, lines and points' : ''} below ${cfg.level}, in ${hardVis} of ${res.rows.length} visuals\n${textFails} text${textFails === 1 ? '' : 's'} below ${cfg.level === 'AAA' ? '7:1 (large text 4.5:1)' : '4.5:1 (large text 3:1)'}${cfg.marks ? `, ${hard - textFails} bar, line or point color${hard - textFails === 1 ? '' : 's'} below 3:1` : ''}. Hover a visual's badge for each pair.`),
      item('grid', offCount, `off palette${inVis(offCount, offVis)}`, `Colors outside the palette, in ${offVis} of ${res.rows.length} visuals\nHover a visual's badge for each color and where it is used; point at a swatch to outline every part drawn in it.`),
      passes.fine,
      el('span', { class: 'pbt-strip-seg' }, passes.sw, el('span', { class: 'pbt-strip-label', text: 'level' }),
        button({ class: `pbt-strip-grid${cfg.level === 'AA' ? ' pbt-on' : ''}`, text: 'AA' }, () => set('level', 'AA'), 'AA\nText 4.5:1, large text 3:1: the level most accessibility policies ask for.'),
        button({ class: `pbt-strip-grid${cfg.level === 'AAA' ? ' pbt-on' : ''}`, text: 'AAA' }, () => set('level', 'AAA'), 'AAA\nText 7:1, large text 4.5:1: the stricter level.')),
      button({ class: `pbt-strip-switch${cfg.marks ? ' pbt-on' : ''}`, role: 'switch', 'aria-checked': String(cfg.marks) }, () => set('marks', !cfg.marks),
        'Bars, lines and points\nAlso check chart marks against the visual\'s background at 3:1 (WCAG 1.4.11).'),
      button({ class: 'pbt-strip-help', text: '?' }, () => { if (pbt.help) { pbt.help.close(); pbt.help.open(); pbt.help.show('colors'); } }, 'How Colors works\nOpens the help page at Colors.'));
    // The switch's track, then its words (built here so the button keeps one tooltip).
    bar.querySelector('.pbt-strip-switch').append(el('span', { class: 'pbt-switch-track' }), el('span', { text: 'bars, lines and points' }));
    // On a narrow dock the palette's name and the "level" label)PBT"
LR"PBT( give way first (pbt.dock.place).
    bar.querySelector('[data-count="vis"]').dataset.opt = '';
    bar.querySelector('.pbt-strip-label').dataset.opt = '';
    pbt.placeStrip(bar, 'colors', g);
    return bar;
  },
  clear() { if (this.host) { this.host.remove(); this.host = null; } pbt.railClear('colors'); this.unhighlight(); this.sig = ''; },
  draw(announce) {
    // The scan is the heavy part: on the timer it runs only after the page changed (a mutation,
    // a move, a zoom, a scroll, another tool's strip) or the settings did.
    const g0 = pbt.page.geometry();
    const cheap = g0 && JSON.stringify([g0.rect.left, g0.rect.top, g0.rect.width, g0.rect.height, g0.scale, this.cfgNow(), pbt.cfg.lint || {}, this.sourceShort(), pbt.passes(), pbt.stripOrder.filter(n => (pbt.stripTools()[n] || {}).on)]);
    if (!announce && !this.dirty && cheap === this.cheap && this.host && this.host.isConnected) return;
    this.dirty = false;
    this.cheap = cheap;
    const res = this.analyse();
    if (!res) { this.clear(); return; }
    const g = res.g;
    // A color changed in place (a data color, conditional formatting) changes attributes, not
    // nodes, so the page itself is watched for those as well.
    if (g.da !== this.watched) { this.recolor.disconnect(); this.recolor.observe(g.da, { subtree: true, attributes: true, attributeOldValue: true, attributeFilter: ['fill', 'stroke', 'style', 'opacity', 'fill-opacity', 'stroke-opacity', 'color'] }); this.watched = g.da; }
    const sig = JSON.stringify([g.rect.left, g.rect.top, g.rect.width, g.rect.height, res.cfg, pbt.cfg.lint || {}, this.sourceShort(), pbt.passes(), pbt.stripOrder.filter(n => (pbt.stripTools()[n] || {}).on),
      res.rows.map(r => [r.v.x, r.v.y, r.v.w, r.v.h, r.skipped, r.unseen, r.truncated, r.judged.map(j => [j.kind, j.text, Math.round(j.ratio * 100), j.need]), [...r.colors.keys()], [...r.fonts]])]);
    if (sig === this.sig && this.host && this.host.isConnected) return;
    this.sig = sig;
    this.result = res;
    if (this.host) this.host.remove();
    pbt.railClear('colors');
    const el = pbt.el;
    const host = this.host = el('div', { class: 'pbt-co', 'data-pbt': '', style: `left:${g.rect.left}px;top:${g.rect.top}px;width:${g.rect.width}px;height:${g.rect.height}px` });
    const passes = pbt.passes(), tol = this.tolerance();
    for (const row of res.rows) {
      if (!passes && !this.reportable(row)) continue;
      const v = row.v, { off } = this.numbered(row);
      const tone = this.tone(row, res.cfg);
      const b = pbt.tip(el('div', { class: `pbt-co-badge pbt-co-${tone} pbt-sev-${CO_SEV[tone]}` }, el('b', { text: this.label(row, res.cfg) })), () => this.card(row, res.cfg));
      // Each stray color as a swatch: pointing at one outlines every part drawn in it (with its
      // number from the card), clicking copies its hex.
      for (const { hex, n } of off) {
        b.append(pbt.tip(el('button', {
          class: 'pbt-sw', style: `background:${hex}`,
          onpointerdown: e => { e.stopPropagation(); e.preventDefault(); pbt.copy(hex); },
          onpointerenter: () => pbt.pinpoint([{ els: row.els.get(hex) || [], n }]),
          // Back on the badge: its own outlines again (leaving the badge too clears them after).
          onpointerleave: () => this.highlight(row),
        }), `${hex} (${n} on the card)\nUsed as ${[...row.colors.get(hex)].join(' and ')}; no palette color within ${tol} per channel; outlined on the page while you point here.\nClick to copy the hex`));
      }
      b.dataset.coVisual = v.title || v.kind;
      pbt.ownBadge(b, v);
      b.addEventListener('pointerenter', () => this.highlight(row));
      b.addEventListener('pointerleave', () => this.unhighlight());
      pbt.railPut(b, v, 'colors');
    }
    host.append(this.strip(g, res));
    document.body.append(host);
    if (announce) {
      const hardN = res.rows.filter(r => this.fails(r).length).length;
      const offN = res.rows.filter(r => r.off.length).length;
      const fontN = res.rows.filter(r => r.fontsOff.length).length;
      const unchecked = res.rows.filter(r => !r.judged.length).length;
      const parts = [];
      if (hardN) parts.push(`${hardN} with something hard to read (below ${res.cfg.level})`);
      if (offN) parts.push(`${offN} off palette`);
      if (fontN) parts.push(`${fontN} in other fonts`);
      const rest = unchecked ? ` ${unchecked} with no text measurable.` : '';
      pbt.toast(parts.length ? `Of ${res.rows.length} visuals: ${parts.join(', ')}. Hover a badge for the details.${rest}` : `All ${res.rows.length} visuals are easy to read at ${res.cfg.level} and on the palette.${rest}`,
        { title: 'Colors', tone: hardN ? 'bad' : offN || fontN ? 'warn' : 'good', ms: 6000 });
      if (pbt.fun) pbt.fun.say(parts.length ? 'lintBad' : 'lintGood');
    }
  },
  toggle() {
    this.on = !this.on;
    this.dirty = true;
    // Focus mode counts the check tools on, so the dock lays out (and applies it) again.
    pbt.restack();
    if (!this.on) { this.clear(); this.result = null; this.recolor.disconnect(); this.watched = null; pbt.toast('Colors off'); return; }
    this.draw(true);
  },
  start() {
    const extra = {
      active: () => this.on,
      desc: 'A badge on every visual with text hard to read against what is behind it (WCAG AA or AAA, bars, lines and points at 3:1) or colors off the palette; greys, white and black always pass.',
      tip: 'Colors\nReads text and chart colors as drawn and the color behind each, including the bar under a data label, and every color a visual draws against the theme or your palette. Hover a badge for the card; point at a swatch to outline every part in that color.',
    };
    pbt.hotkey('l', 'Colors', () => this.toggle(), { ...extra, tool: true, id: 'colors' });
    // The contrast check's old key opens the same tool; only one entry in the Tools menu.
    pbt.hotkey('a', 'Colors (as Ctrl+Alt+L)', () => this.toggle(), extra);
    // Any change on the ca)PBT"
LR"PBT(nvas marks the scan stale; the 2-second tick rescans only then. The shared
    // watcher sees nodes come and go; this one sees colors change in place (watched from draw()).
    pbt.watch(() => { this.dirty = true; });
    // A style change counts only when its paint changed: Power BI restyles a visual's header on
    // every hover (the Filter map's own hovers too), which is no reason for a rescan.
    const paint = s => (String(s || '').match(/(?:^|;)\s*(?:fill|stroke|color|opacity|fill-opacity|stroke-opacity|background(?:-color)?)\s*:[^;]*/g) || []).join(';');
    this.recolor = new MutationObserver(ms => { if (ms.some(m => m.attributeName !== 'style' || paint(m.oldValue) !== paint(m.target.getAttribute('style')))) this.dirty = true; });
    pbt.cleanups.push(() => this.recolor.disconnect());
    pbt.every(2000, () => { if (this.on) this.draw(false); });
  },
  stop() { this.clear(); document.documentElement.classList.remove('pbt-ct-probe'); },
};
pbt.colors = colors;
pbt.module(colors);

// ---- 50-fun.js ----
// Extras: a splash when a skin is picked, short comments (visual errors, finished
// refreshes, QA and lint results), confetti when a refresh finishes (shapes and colors per
// skin), short synthesized sounds per skin (off by default), and the Konami code.
// Nothing here touches the report; confetti and toasts draw on top and take no clicks.

const VOICE = {"title":"Skins","dance":"Dance break!","error":["A visual on this page shows an error.","A visual could not render."],"refresh":["Refresh finished.","Data refreshed."],"qaGood":["Every edge lines up.","Layout is clean."],"qaBad":["Some edges are a few pixels off.","A few visuals are slightly out of line."],"lintGood":["Every color is on the palette.","Colors: nothing off the palette."],"lintBad":["Some colors are off the palette.","Colors: some colors are off the palette."]};
const pickLine = list => list[Math.floor(Math.random() * list.length)];

const fun = {
  name: 'fun',
  errors: new WeakSet(),
  say(kind, extra) {
    if (!pbt.feature('notes')) return;
    pbt.toast(`${pickLine(VOICE[kind])}${extra ? ` ${extra}` : ''}`, { title: VOICE.title, tone: /error|Bad/.test(kind) ? 'bad' : 'good', ms: 5000 });
  },

  // ---- splash and sound when a skin is chosen ----
  onSkin(id, skin) {
    if (pbt.feature('splash') && skin.splash) {
      document.querySelectorAll('.pbt-splash').forEach(n => n.remove());
      const s = pbt.el('div', { class: 'pbt-splash', 'data-pbt': '' }, pbt.el('div', {}, skin.splash[0], pbt.el('small', { text: skin.splash[1] || '' })));
      document.body.append(s);
      setTimeout(() => s.remove(), 2000);
    }
    this.play(skin);
  },

  // ---- sounds: a few oscillator notes, no audio files ----
  audio: null,
  play(skin, force = false) {
    if ((!force && !pbt.feature('sounds')) || !skin || !skin.sound) return;
    try {
      const ctx = this.audio || (this.audio = new AudioContext());
      const { wave = 'sine', notes = [660], step = 0.1, len = 0.2 } = skin.sound;
      notes.forEach((f, i) => {
        const o = ctx.createOscillator(), g = ctx.createGain();
        const t = ctx.currentTime + i * step;
        o.type = wave; o.frequency.value = f;
        g.gain.setValueAtTime(0.0001, t);
        g.gain.exponentialRampToValueAtTime(0.08, t + 0.01);
        g.gain.exponentialRampToValueAtTime(0.0001, t + len);
        o.connect(g).connect(ctx.destination);
        o.start(t); o.stop(t + len + 0.05);
      });
    } catch {}
  },

  // ---- a cheer of your own with the refresh confetti: an audio file embedded as PBT_CHEER, by the
  // injector from tweaks.json "cheer" or by the mod from its "Sound file for the refresh confetti"
  // setting (the mod never carries a sound of its own). It rides on the confetti switch. ----
  cheer(force = false) {
    const src = typeof PBT_CHEER !== 'undefined' ? PBT_CHEER : null;
    if (!src || (!force && !(pbt.feature('confetti') && pbt.feature('refreshCheer')))) return false;
    try { const a = new Audio(src); a.volume = 0.7; a.play().catch(() => {}); } catch {}
    return true;
  },

  // ---- confetti ----
  confetti(skin = pbt.skins[pbt.cfg.skin]) {
    const style = (skin && skin.confetti) || { shapes: ['square'], colors: ['#2aac94', '#f2c94c', '#e5383b', '#5ad1ff', '#8474b2'] };
    const canvas = pbt.el('canvas', { class: 'pbt-confetti', 'data-pbt': '' });
    const dpr = devicePixelRatio || 1;
    canvas.width = innerWidth * dpr; canvas.height = innerHeight * dpr;
    document.body.append(canvas);
    const ctx = canvas.getContext('2d');
    ctx.scale(dpr, dpr);
    const parts = Array.from({ length: 160 }, (_, i) => ({
      x: innerWidth / 2 + (Math.random() - 0.5) * innerWidth * 0.3, y: innerHeight * 0.35,
      vx: (Math.random() - 0.5) * 16, vy: -Math.random() * 14 - 4,
      r: Math.random() * Math.PI, vr: (Math.random() - 0.5) * 0.3, s: 6 + Math.random() * 7,
      c: style.colors[i % style.colors.length], shape: style.shapes[i % style.shapes.length],
    }));
    const draw = p => {
      ctx.save(); ctx.translate(p.x, p.y); ctx.rotate(p.r); ctx.fillStyle = p.c; ctx.strokeStyle = p.c;
      switch (p.shape) {
        case 'coin': // gold coin seen edge-on as it spins
          ctx.scale(Math.abs(Math.cos(p.r * 2)) + 0.15, 1);
          ctx.beginPath(); ctx.arc(0, 0, p.s / 1.6, 0, Math.PI * 2); ctx.fill();
          ctx.strokeStyle = 'rgba(90, 60, 0, .6)'; ctx.lineWidth = 1.2; ctx.beginPath(); ctx.arc(0, 0, p.s / 2.6, 0, Math.PI * 2); ctx.stroke();
          break;
        case 'crystal': // mineral shard
          ctx.beginPath(); ctx.moveTo(0, -p.s); ctx.lineTo(p.s / 2.4, 0); ctx.lineTo(0, p.s); ctx.lineTo(-p.s / 2.4, 0); ctx.closePath(); ctx.fill();
          ctx.fillStyle = 'rgba(255, 255, 255, .5)'; ctx.beginPath(); ctx.moveTo(0, -p.s); ctx.lineTo(p.s / 2.4, 0); ctx.lineTo(0, 0); ctx.fill();
          break;
        case 'lambda':
          ctx.font = `bold ${p.s * 2}px Bahnschrift, sans-serif`; ctx.textAlign = 'center'; ctx.textBasel)PBT"
LR"PBT(ine = 'middle'; ctx.fillText('\u03bb', 0, 0);
          break;
        case 'ring':
          ctx.lineWidth = 2; ctx.beginPath(); ctx.arc(0, 0, p.s / 2, 0, Math.PI * 2); ctx.stroke();
          break;
        case 'pill': // console button
          ctx.beginPath(); ctx.roundRect(-p.s, -p.s / 3, p.s * 2, p.s / 1.5, p.s / 3); ctx.fill();
          break;
        case 'bits': // a 0 or a 1, fixed per particle
          ctx.rotate(-p.r); ctx.font = `bold ${p.s * 1.8}px "Lucida Console", monospace`; ctx.textAlign = 'center'; ctx.textBaseline = 'middle';
          ctx.fillText(p.s > 9.5 ? '1' : '0', 0, 0);
          break;
        case 'streak': // light trail along the direction of travel: a faint wide band and a bright core (no canvas blur, which is slow)
          ctx.rotate(Math.atan2(p.vy, p.vx) - p.r);
          ctx.globalAlpha = 0.3; ctx.fillRect(-p.s * 2.5, -2.5, p.s * 2.5, 5);
          ctx.globalAlpha = 1; ctx.fillRect(-p.s * 2.5, -1, p.s * 2.5, 2);
          break;
        default:
          ctx.fillRect(-p.s / 2, -p.s / 4, p.s, p.s / 2);
      }
      ctx.restore();
    };
    const t0 = performance.now();
    const frame = now => {
      if (!canvas.isConnected) return;
      ctx.clearRect(0, 0, innerWidth, innerHeight);
      for (const p of parts) { p.vy += 0.35; p.vx *= 0.99; p.x += p.vx; p.y += p.vy; p.r += p.vr; draw(p); }
      if (now - t0 < 4500) requestAnimationFrame(frame); else canvas.remove();
    };
    requestAnimationFrame(frame);
    this.play(skin);
  },

  // ---- visual errors: Power BI's error state shows inside the visual's container ----
  checkErrors() {
    const found = [];
    for (const v of document.querySelectorAll('.displayArea .visualContainer')) {
      const err = v.querySelector('[class*="error" i]:not(svg *), .visualError, [data-testid*="error" i]');
      const text = (v.textContent || '').slice(0, 400);
      const broken = (err && err.offsetParent !== null && err.getBoundingClientRect().width > 0) || /can.t display (this|the) visual|couldn.t (load|retrieve) the data|something.s wrong with one or more fields/i.test(text);
      if (broken && !this.errors.has(v)) { this.errors.add(v); found.push(v.getAttribute('aria-label') || v.getAttribute('aria-roledescription') || 'a visual'); }
    }
    if (found.length) this.say('error', `(${found.map(s => s.trim()).join(', ')})`);
  },

  // ---- refresh: the progress window is native, so watch for the click, then for the
  // burst of re-rendering in the report page and the quiet after it ----
  refresh: null,
  refreshEvent: 'pbt-refreshed',
  armRefresh() {
    const da = document.querySelector('.displayArea');
    if (!da) return;
    if (this.refresh) this.refresh.mo.disconnect();
    const state = this.refresh = { count: 0, last: 0, t0: Date.now(), mo: null };
    state.mo = new MutationObserver(m => { state.count += m.length; state.last = Date.now(); });
    state.mo.observe(da, { childList: true, subtree: true, attributes: true, characterData: true });
  },
  pollRefresh() {
    const s = this.refresh;
    if (!s) return;
    const quiet = Date.now() - s.last;
    if (s.count > 60 && quiet > 1500) {
      s.mo.disconnect(); this.refresh = null;
      if (pbt.feature('confetti')) { this.confetti(); this.cheer(); }
      this.say('refresh');
      // Other modules (Chaos Mode, page/80-chaos.js) hear about a finished refresh through this event.
      try { window.dispatchEvent(new CustomEvent(this.refreshEvent)); } catch {}
    } else if (Date.now() - s.t0 > 10 * 60000) { s.mo.disconnect(); this.refresh = null; }
  },

  start() {
    pbt.watch(() => this.checkErrors());
    pbt.on(document, 'click', e => {
      const b = e.target.closest && e.target.closest('button');
      if (b && /^Refresh$/i.test((b.getAttribute('aria-label') || b.textContent || '').trim()) && b.closest('.ribbonContainer, .ms-Callout, .ms-Layer')) this.armRefresh();
    }, true);
    pbt.every(500, () => this.pollRefresh());
    pbt.hotkey('c', 'Throw confetti', () => this.confetti(), { desc: 'Confetti in the skin\'s shapes, right now.' });
    // Konami code: the visuals dance for two seconds.
    const code = ['ArrowUp', 'ArrowUp', 'ArrowDown', 'ArrowDown', 'ArrowLeft', 'ArrowRight', 'ArrowLeft', 'ArrowRight', 'b', 'a'];
    let at = 0;
    // The code is part of the "Hidden surprises" setting: off, no key is watched for it at all.
    if (pbt.feature('surprises')) pbt.on(window, 'keydown', e => {
      at = e.key === code[at] || e.key.toLowerCase() === code[at] ? at + 1 : (e.key === code[0] ? 1 : 0);
      if (at === code.length) {
        at = 0;
        // Chaos Mode (page/80-chaos.js) owns the code once it has started.
        try { if (pbt.chaos && pbt.chaos.state().started) return; } catch {}
        document.documentElement.classList.add('pbt-dance');
        setTimeout(() => document.documentElement.classList.remove('pbt-dance'), 2200);
        pbt.toast(VOICE.dance, { title: VOICE.title });
        this.confetti();
      }
    }, true);
  },
  stop() {
    if (this.refresh) this.refresh.mo.disconnect();
    document.documentElement.classList.remove('pbt-dance');
    if (this.audio) this.audio.close().catch(() => {});
  },
};
pbt.fun = fun;
pbt.module(fun);

// ---- 60-ribbon.js ----
// A "Skins" group at the end of the View ribbon, beside Power BI's own Themes. "None" puts
// Power BI back exactly as it was set. Each button carries a tooltip with the skin's name.

const ribbon = {
  name: 'ribbon',
  group: null,
  sig: '',
  // Classic in its 95 style draws the Power BI 95 look in place of the skin; it still shows as Classic.
  shownSkin() {
    if (pbt.retro95 && pbt.retro95.classicOn && pbt.retro95.classicOn()) return 'classic';
    return pbt.skin && pbt.skin.current !== undefined ? pbt.skin.current : pbt.cfg.skin;
  },
  classicStyle() { return pbt.retro95 && pbt.retro95.classicStyle ? pbt.retro95.classicStyle() : ''; },
  build() {
    const viewOpen = document.querySelector('#tablist button#view[role="tab"][)PBT"
LR"PBT(aria-selected="true"]');
    const groups = viewOpen && [...document.querySelectorAll('.ribbonContainer .bottomBarContainer [role="group"][id$="-group"]')];
    if (!groups || !groups.length) {
      // Another tab is open. The ribbon re-renders its row in place and would keep our group in
      // it (it showed up at the start of Home), so take it out.
      document.querySelectorAll('.pbt-rgroup').forEach(g => g.remove());
      this.group = null;
      return;
    }
    // Each group sits in a "groupContainer" inside one flex row; ours goes at the end of that row.
    const lastWrap = groups[groups.length - 1].closest('[class*="groupContainer"]');
    const row = lastWrap && lastWrap.parentElement;
    if (!row) return;
    // A group left behind by an earlier copy of the script never stays beside ours.
    document.querySelectorAll('.pbt-rgroup').forEach(g => { if (g !== this.group) g.remove(); });
    const shown = this.shownSkin();
    const style = shown === 'classic' ? this.classicStyle() : '';
    const sig = `${shown}|${style}`;
    if (this.group && this.group.isConnected && this.group.parentElement === row && row.lastElementChild === this.group && sig === this.sig) return;
    if (this.group) this.group.remove();
    this.sig = sig;

    const btn = (id, label, sw) => pbt.tip(pbt.el('button', {
      class: 'pbt-rbtn', 'aria-pressed': String(shown === id || (id === 'off' && !pbt.skins[shown])),
      onpointerdown: e => e.stopPropagation(),
      onclick: e => { e.stopPropagation(); pbt.skin.set(id); },
    }, pbt.el('span', { class: 'pbt-sw', style: `background:${sw}` }), pbt.el('span', { text: label })), pbt.skinTip(id));

    // While Classic is picked, its two styles sit beside the gallery.
    const STYLES = [['clean', 'Clean', 'Clean\nClassic as it is: grey bevels on Power BI\'s own layout.'],
      ['95', 'Power BI 95', 'Power BI 95\nClassic gone full 90s desktop: silver panes, heavy bevels, a teal desktop and a Begin button.']];
    const styles = style && pbt.el('div', { class: 'pbt-rstyle', role: 'group', 'aria-label': 'Classic style' },
      STYLES.map(([v, label, tip]) => pbt.tip(pbt.el('button', {
        class: 'pbt-rbtn', 'aria-pressed': String(style === v),
        onpointerdown: e => e.stopPropagation(),
        onclick: e => { e.stopPropagation(); pbt.retro95.setClassicStyle(v); },
      }, pbt.el('span', { text: label })), tip)));

    this.group = pbt.el('div', { class: 'pbt-rgroup', 'data-pbt': '', role: 'group', 'aria-label': 'Skins' },
      pbt.el('div', { class: 'pbt-rgroup-body' },
        pbt.el('div', { class: 'pbt-rgallery' },
          btn('off', 'None', 'repeating-linear-gradient(45deg, #888 0 3px, transparent 3px 6px)'),
          pbt.skin.ids().map(id => btn(id, pbt.skins[id].short || pbt.skins[id].name, pbt.swatch(pbt.skins[id])))),
        styles || null),
      pbt.tip(pbt.el('label', { class: 'pbt-rgroup-label pbt-groupLabel', text: 'Skins' }), 'Skins\nRestyle the window around your report; the report page itself is never changed. The status bar chip and Ctrl+Alt+K open the full menu.'));
    row.append(this.group);
  },
  start() {
    this.build();
    pbt.watch(() => this.build());
  },
  onConfig() { this.sig = ''; this.build(); },
  onApplied() { this.sig = ''; this.build(); },
  stop() { if (this.group) this.group.remove(); },
};
pbt.module(ribbon);

// ---- 70-help.js ----
// In-app help (Ctrl+Alt+H, or "Help and keys" in the menu): what the skins, the tools and the
// extras do, a legend for every mark the tools draw, every key, and where choices are kept.
// A panel in the middle of the window; Esc, its close button or a click outside closes it.

const help = {
  name: 'help',
  panel: null,
  backdrop: null,
  open() {
    if (this.panel) return this.close();
    if (pbt.menu) pbt.menu.close();
    pbt.hideTip && pbt.hideTip();
    const el = pbt.el;
    const p = text => el('p', { text });
    const key = text => el('span', { class: 'pbt-key', text });
    const h3 = (text, k) => el('h3', {}, el('span', { text }), k ? key(k) : null);
    const section = (id, ...kids) => el('section', { id: `pbt-help-${id}` }, ...kids);
    const button = (text, tip, run) => pbt.tip(el('button', { class: 'pbt-btn', text, onclick: e => { e.stopPropagation(); run(); } }), tip);
    const hk = k => { const h = pbt.hotkeys.find(x => x.key === k); return h ? `Ctrl+Alt+${h.key.toUpperCase()}` : ''; };
    const lintCfg = pbt.cfg.lint || {}, palette = lintCfg.palette || [];
    const mod = typeof PBT_HOST !== 'undefined' && PBT_HOST === 'mod';
    // Each tab reads as a one-line summary, then short points each led by a bold term. The points
    // are built from text nodes only (no innerHTML), so nothing in a string is ever read as markup.
    const lead = text => el('p', { class: 'pbt-help-lead', text });
    const pts = (...items) => el('ul', { class: 'pbt-help-pts' }, ...items.map(([term, text]) => el('li', {}, el('b', { text: `${term}: ` }), text)));

    const intro = section('intro',
      lead('Restyles the title bar, ribbon, panes, page tabs, status bar and every view; never your report page or its visuals.'),
      pts(
        ['Tools', 'Plan (Stencil), Layout (Layout QA), Check (Filter map, Theme check, Colors), While editing (Interactions). Extras are for fun.'],
        ['Badges', 'the check tools share one row under each visual\'s bottom right corner, each led by its letter (F, T, C).'],
        ['Badge color', 'severity only: red to fix, amber to look at, grey for information, green for a pass ("show passes").'],
        ['Dock', 'with two or more tools on, one tab per tool under the page; a dot marks tools that are on, the underlined tab\'s strip shows.']));

    const skins = section('skins', h3('Skins', `${hk(']')} next \u00b7 ${hk('[')} previous`),
      lead('Your pick is remembered and follows every open view and dialog.'),
      pts(
        ['Pick', 'the Skins group at the end of the View ribbon, the status bar chip, or the keys.)PBT"
LR"PBT('],
        ['None', 'puts Power BI back exactly as it was set.'],
        ['AltGr keyboards', '(German, French, Nordic, Spanish, Italian, Czech and many more) [ and ] are left for typing and may not step skins; use the ribbon, chip or menu.'],
        ['Light and dark', 'skins look the same in both; the title bar\'s theme button still switches the mode underneath.']),
      el('div', { class: 'pbt-help-skins' }, ...pbt.skin.ids().map(id => {
        const s = pbt.skins[id];
        return el('div', { class: 'pbt-help-skin' }, el('span', { class: 'pbt-sw', style: `background:${pbt.swatch(s)}` }), el('b', { text: s.name }), s.inspiredBy ? el('span', { text: ` ${s.inspiredBy}` }) : null);
      })));

    const legend = rows => el('div', { class: 'pbt-legend' }, ...rows.flatMap(([cls, text]) => [el('span', { class: cls }), el('span', { text })]));
    const qaCfg = pbt.cfg.qa || {}, gridNow = qaCfg.grid || 8, nearNow = qaCfg.nearMiss || 8;
    const qa = section('qa', h3('Layout QA', hk('g')),
      lead('A grid, rulers and a label on every visual; counts in a strip under the page.'),
      pts(
        ['Grid', `${gridNow} report px, every fifth line stronger.`],
        ['Labels', 'type and x, y \u00b7 w \u00d7 h in exact report pixels at any zoom (as in the Format pane).'],
        ['Overlay', 'takes no canvas clicks; redraws as visuals move.']),
      legend([
        ['pbt-lg-box', 'A visual in order: its edges line up with its neighbours and its gaps match the page.'],
        ['pbt-lg-bad', `A visual to look at: an edge 1 to ${nearNow} px off a neighbour's matching edge, a gap that far off the page's usual gap, or it runs past the page edge.`],
        ['pbt-lg-guide', 'Where a near-missed edge belongs; the tag says which edge, how far off and where the line is ("Right edge 4 px off the line at 1248").'],
        ['pbt-lg-off', `A visual with an edge off the ${gridNow} px grid (amber, dashed), when the grid check is on.`],
        ['pbt-lg-shared', 'Shown while you hover a visual: an edge it shares with others, tagged with how many.'],
        ['pbt-lg-dist', 'Shown while you hover a visual: its gap to the nearest neighbour, or to the page edge, in each direction.'],
      ]),
      pts(
        ['Hover a label', 'position, size, each finding with the neighbours involved, and the fixing value (an arrow on x, y, width or height marks the change).'],
        ['Click a label', 'copies x, y, w, h; Shift+click, the suggested values for the Format pane\'s Properties.'],
        ['Aligned', 'an edge as many neighbours share as nearly miss, so one visual off a row is reported, not the row. Centers count only if neither edge on that axis lines up.'],
        ['Strip', 'page size, visual count, edges off, odd gaps vs the usual gap, visuals on the grid, grid picker (4 to 20 px, qa.grid), grid check (qa.snap), this help.'],
        ['Settings', `qa.nearMiss (${nearNow}), how far off is a miss; pageSize pins the page size.`]),
      h3('When to snap'),
      pts(
        ['Snap to grid', '(View ribbon) holds edges to Power BI\'s grid as you drag; amber marks read that back.'],
        ['On the grid', 'the skeleton (cards, charts, their rows and columns, the page gutter), so gaps repeat and columns line up unmeasured.'],
        ['Off the grid', 'content-sized things where a pixel or two of optical alignment matters more: text-sized slicers and buttons, text boxes, legends, a title aligned to a chart\'s plot area, not its frame.'],
        ['Odd gap', 'page built on a gap that is not a grid multiple? Set the grid to it (qa.grid). qa.snap false hides amber marks.']));

    const fm = section('filters', h3('Filter map', hk('f')),
      lead('A grey badge on each visual with something worth knowing says what filters it, by level.'),
      pts(
        ['Badge', 'letter and count per level, spelled out under the pointer ("V1 S2": "1 visual \u00b7 2 from slicers & cross-filters").'],
        ['Hover', 'each filter in words; for visual-level ones, whether hidden or locked in view mode.'],
        ['Strip', 'counts, Rescan, and the visual-filters menu.']),
      legend([
        ['pbt-lg-fvisual', 'V: visual-level filters, from the Filters pane (hidden ones included, which Power BI\'s own list for the visual leaves out). An outline on the badge marks a visual that has any.'],
        ['pbt-lg-fpage', 'P: filters on this page.'],
        ['pbt-lg-freport', 'R: filters on all pages.'],
        ['pbt-lg-fother', 'S: slicers & cross-filters: a slicer, a cross-filter or a selection in another visual, read from Power BI\'s "Filters and slicers affecting this visual".'],
      ]),
      p('Read without clicking anything you can see:'),
      pts(
        ['Page, report', 'from the Filters pane.'],
        ['Visual', 'from the project files (Settings), hidden and locked included, or live from the pane once selected.'],
        ['Slicers, cross-filters', 'from Power BI\'s list for each visual, opened out of sight.'],
        ['Updates', 'when a filter changes in the pane or you click a slicer or visual.'],
        ['No project files', '"Read each visual" (strip) selects each visual in turn to read cards it cannot see otherwise.']),
      h3('Hide, show, lock or unlock visual filters at once'),
      pts(
        ['Open', '"Visual filters" in the strip, or the small button by "Filters on this visual" in the Filters pane ("Filters on this page" with nothing selected).'],
        ['Menu', 'Hide, Show, Lock or Unlock for the selected visual, this page or every page.'],
        ['How', 'presses the eye and lock buttons you would, card by card, field-well cards included; page and report filters are left alone.'],
        ['Esc', 'stops a run. Save the report to keep the result.']));

    const th = section('theme', h3('Theme check', hk('t')),
      lead('Badges each visual that sets something differently from the theme file.'),
      pts(
        ['Badge', 'amber, "3 differ from theme".'],
        ['Page', 'grey infor)PBT"
LR"PBT(mation on settings unlike most of the page ("1 differs from the page"); off until the strip\'s "differ from the page" switch (themeCheck.page).'],
        ['Pass', '"\u2713 theme" while "show passes" is on.'],
        ['Hover', 'each setting with this visual\'s value and the expected one, plus what it sets itself without a finding; each finding\'s part (title, axes, columns in a color, the box for background or border) is outlined and numbered.']),
      legend([
        ['pbt-lg-off', 'Theme: the visual sets something the theme sets, to another value (a title size of 16 where the theme says 14).'],
        ['pbt-lg-bad', 'Palette: the visual sets a color that is not one of the theme\'s colors.'],
        ['pbt-lg-guide', 'Page (information): the theme leaves it open, and most visuals on the page that set it use another value (one hidden title among shown ones).'],
      ]),
      pts(
        ['Project files', '(Settings): saved formatting vs the theme file\'s style for the visual type, then for every visual, then the title text class. Save to check later changes.'],
        ['Without them', 'what each visual draws (title, color, weight, background, border) vs most of the page; those differences always show.'],
        ['Strip', 'names the setting most often off ("Title font size"); its tooltip lists the top six with counts.']));

    const ia = section('interactions', h3('Interactions', hk('i')),
      lead('While Edit interactions is on (select a visual, then Format > Edit interactions), colors every other visual\'s active icon so the page reads at a glance.'),
      pts(
        ['Colors', 'None red, Filter blue, Highlight green.'],
        ['Bar', 'above the selected visual: "None for all", "Filter for all", "Highlight for all" (greyed when no visual offers it; only charts do), with a count of those set to None.'],
        ['Undo', 'it clicks each visual\'s own icon, so Ctrl+Z undoes one at a time.'],
        ['Switch', 'on by default; Ctrl+Alt+I switches it off and on.']));

    const st = section('stencil', h3('Stencil', hk('s')),
      lead('A see-through mock-up layer: draw where visuals go, build the blocks as real visuals with no fields, then drag the fields in.'),
      pts(
        ['Blocks', 'report pixels on the Layout QA grid, saved per report and page.'],
        ['Edit', 'in the strip makes the page a drawing surface that takes no clicks until Done or Esc.'],
        ['Draw', 'pick a type in the strip (card, column, bar or line chart, table, slicer, title, text), or "\u2026" for every other Power BI visual (stacked and 100% charts, area, combo, pie, donut, map, gauge, KPI, matrix, button, image, shape, the AI visuals and more), then drag on the page. A click without a drag draws nothing; on empty page it just deselects. Drag a block to move, its handles to resize.'],
        ['Change type', 'with a block selected, picking a type (strip or "\u2026") changes that block in place: same place and size, and a name you typed stays. One Ctrl+Z undoes it.'],
        ['Snap', 'to the grid and to edges of visuals and blocks; hold Alt for 1 px steps.'],
        ['Exact', 'type the selected block\'s x, y, width, height and label in the strip.']),
      el('table', {}, ...[
        ['Arrows', 'Move the selected block 1 px (Shift: one grid step)'],
        ['Delete', 'Remove the selected block (or the trash button in the strip)'],
        ['Ctrl+C, Ctrl+X', 'Copy or cut the selected block (kept by the stencil, not the system clipboard)'],
        ['Ctrl+V', 'Paste it on this page or another one, selected, a grid step off when its spot is taken'],
        ['Ctrl+D', 'Duplicate it'],
        ['Ctrl+Z, Ctrl+Y', 'Undo and redo stencil changes (Power BI\'s own undo is not touched)'],
        ['Double-click', 'Rename a block (its title when built; a text box\'s text)'],
        ['Esc', 'Deselect, then leave edit mode'],
      ].map(([k, what]) => el('tr', {}, el('td', { text: k }), el('td', { text: what })))),
      pts(...(mod
        ? [['Convert to visuals', 'the strip button, or "Convert to visuals: this page" or "Convert to visuals: new page" in the Build menu, copies a PowerShell script writing one new visual.json per block into the open report\'s files (a .pbip project that Power BI opened; the mod finds it by itself).'],
          ['Load', 'save the report, paste the script into PowerShell and press Enter, then close and reopen the report to see the visuals.'],
          ['Safe', 'only new files under definition\\pages; pages.json is backed up before a new page is added.']]
        : [['Build', 'the Build menu writes one new visual.json per block into the report project folder ("project"), on this page or a new one, after asking whether the report is saved.'],
          ['Load', 'click Apply external changes in Power BI\'s top bar (with the Power BI Desktop bridge library the report reloads by itself).'],
          ['Safe', 'only new files under definition/pages; pages.json is backed up before a new page is added.'],
          ['Convert to visuals', 'the strip button, or "Convert to visuals: this page" or "Convert to visuals: new page" in the Build menu, gives the same build as a PowerShell script: paste it into PowerShell and press Enter, then close and reopen the report.']]),
        ['Built', 'blocks get a check mark once their visual sits there; "Clear built blocks" removes them.'],
        ['Redesign', '"Copy page into stencil" turns the page\'s visuals into blocks.'],
        ['Templates', '"Save as template" in the Build menu keeps this page\'s blocks as a named layout that fits any page size. "Start from a layout" lists the built-in layouts, then "Your templates"; type in its search box to filter by name, and point to a template for rename and delete. A template replaces the page\'s blocks as one step; Ctrl+Z brings them back.'],
        ['Groups', 'visuals grouped in Power BI keep their group when copied into the stencil (from the saved report files, so save first): a dashe)PBT"
LR"PBT(d outline with the group\'s name goes around them, dragging one block moves the whole group, and templates keep the groups.'],
        ['Frames', '"Convert to background" in the Build menu copies a script that draws a card-style frame behind each group and each lone block, as one SVG that becomes the page\'s background image (theme colors when the theme sets them; report.json and page.json are backed up; an existing background image is replaced only with -Replace). "with frames as background" adds the same frames to Convert to visuals.'],
        ['SVG', '"Copy as SVG" copies the blocks at report size, for Figma or another design tool.']));

    const co = section('colors', h3('Colors', `${hk('l')} or ${hk('a')}`),
      lead('One badge per visual with something to fix, in plain words: "1 hard to read", "2 off palette", or both.'),
      pts(
        ['Badge color', 'red if anything is hard to read; amber if the only finding is a color off the palette.'],
        ['Hover', 'one section per question; each numbered finding\'s part is outlined on the page with its number.'],
        ['Swatches', 'one per stray color on the badge: point to outline every part drawn in it, click to copy its hex.']),
      legend([
        ['pbt-lg-ctbad', 'Hard to read: text, or a bar, line or point, that does not stand out enough from what is behind it.'],
        ['pbt-lg-ctwarn', 'Off palette: the visual draws a color outside the palette (or a font outside the allowed families), and nothing is hard to read.'],
        ['pbt-lg-ctok', 'With "show passes" on: every text is measured and meets the level, and every color is on the palette.'],
      ]),
      h3('Hard to read'),
      pts(
        ['Text', 'against the color actually behind it: the bar under a data label, the visual\'s background, the page.'],
        ['Levels', 'AA 4.5:1, or 3:1 for large text (24 px or more, or 18.66 px and bold); AAA 7:1 and 4.5:1 (WCAG 2.1, 1.4.3). Pick AA or AAA in the strip.'],
        ['Bars, lines and points', '3:1 against the visual\'s background (1.4.11); the strip\'s "bars, lines and points" switch turns this off.'],
        ['As drawn', 'colors are read as Power BI draws them, so conditional formatting and the theme count.'],
        ['Not checked', 'text over an image or gradient (listed as not checked), text a custom visual draws into a canvas (unreadable), and the rest of accessibility: alt text, tab order, meaning carried by color alone.']),
      h3('Off palette'),
      pts(
        ['Reads', 'the colors each visual drew (fills, strokes, text, backgrounds); lists those off the palette with where used and the nearest palette color.'],
        ['Always pass', 'greys, white and black.'],
        ['Not read', 'the visual header, Power BI\'s invisible hover targets, anything fully see-through; so every skin gives the same verdict.'],
        ['Palette', `the report theme's colors when project files are loaded (data colors plus good, neutral, bad, foreground, background and accents), else the settings palette; the strip names which. Now: ${pbt.colors.source()}.`]),
      el('div', { class: 'pbt-help-palette' },
        el('span', { text: `The palette in the settings: ${palette.length} color${palette.length === 1 ? '' : 's'}, each channel within ${lintCfg.tolerance || 12}. ` }),
        ...palette.slice(0, 24).map(hex => pbt.tip(el('span', { class: 'pbt-sw', style: `background:${hex}` }), String(hex)))),
      p(lintCfg.fonts && lintCfg.fonts.length ? `Fonts outside ${lintCfg.fonts.join(', ')} are flagged as well.` : 'Fonts are listed on each card and checked when the settings name the allowed families.'));

    const extras = section('extras', h3('Extras'),
      el('ul', {}, ...(pbt.featureList ? pbt.featureList() : Object.entries(pbt.FEATURES || {})).map(([f, def]) => el('li', {},
        el('b', { text: def.label }), el('span', { text: `: ${def.desc} ` }),
        def.run ? button(def.helpTry || def.tryLabel || 'Try', def.tryTip, def.run) : null,
        el('span', { class: 'pbt-state', text: pbt.feature(f) ? 'on' : 'off' }))),
        el('li', {}, el('b', { text: 'The Konami code' }), el('span', { text: ' does something to the page until you press Esc twice or type the code again (the Hidden surprises setting turns it off; its sounds follow Skin sounds).' }))),
      p('Switch each one in the menu (status bar chip or Ctrl+Alt+K).'));

    const keys = section('keys', h3('Keys'),
      el('table', {}, ...[
        ...pbt.hotkeys.map(h => [`Ctrl+Alt+${h.key.toUpperCase()}`, h.label]),
        ['Esc', 'Close the menu, this help, or a tooltip'],
      ].map(([k, what]) => el('tr', {}, el('td', { text: k }), el('td', { text: what })))),
      pts(
        ['Typing', 'in a text box or editor (the formula bar, DAX query view, TMDL view) the letter keys still work; [ and ] are left for typing.'],
        ['AltGr keyboards', 'where [ and ] need AltGr, step skins from the ribbon, chip or menu.'],
        ['This help', 'drag its header to move it, double-click the header to center it; drag its bottom right corner to resize.']));

    const settings = section('settings', h3('Settings'),
      mod
        ? pts(
          ['Where', 'Windhawk > Skins & Tools for Power BI > Settings: the starting skin, the extras, the palette and tolerance for Colors, and "Report project folder".'],
          ['When', 'read when Power BI starts; restart Power BI after enabling, updating or changing the mod.'],
          ['Project files', 'the mod finds the open report\'s project folder by itself: the report Power BI opened as a .pbip, also after File > Open, Recent or Save As, and again after each save.'],
          ['Report project folder', 'only a fallback, for a window where no .pbip was seen: the .Report folder of a report saved as a Power BI project.'],
          ['Choices', 'made in Power BI last until the mod\'s settings change (the project folder and sound file aside), then the settings apply again; Stencil blocks are kept.'])
        : )PBT"
LR"PBT(pts(
          ['tweaks.json', 'beside inject.mjs: the skin, the extras (features), the Layout QA grid and threshold (qa.grid, qa.nearMiss), the palette, tolerance and fonts for Colors (lint.palette, lint.tolerance, lint.fonts, lint.fromTheme), its contrast level and switch (contrast.level, contrast.marks), the report project folder (project, read on every reload) and pageSize.'],
          ['Choices', 'made in Power BI are written back to it.']),
      p(pbt.project && pbt.project.get() ? `Project files: ${pbt.project.get().root} (${pbt.project.get().pages.length} pages, theme ${pbt.project.get().themeName || 'none'}).` : 'Project files: none loaded.'));

    // Tabs, one section at a time, so the page stays short on a laptop; the last tab is remembered.
    // The tools in the one order the menu and the dock use (pbt.JOBS).
    this.tabs = [['skins', 'Skins', [intro, skins]], ['stencil', 'Stencil', [st]], ['qa', 'Layout QA', [qa]], ['filters', 'Filter map', [fm]], ['theme', 'Theme check', [th]], ['colors', 'Colors', [co]], ['interactions', 'Interactions', [ia]], ['extras', 'Extras', [extras]], ['keys', 'Keys', [keys]], ['settings', 'Settings', [settings]]];
    const current = this.tabs.some(t => t[0] === pbt.ui.get('help.tab')) ? pbt.ui.get('help.tab') : 'skins';
    const nav = el('nav', { role: 'tablist' }, ...this.tabs.map(([id, text]) =>
      pbt.tip(el('button', { class: 'pbt-tab', role: 'tab', 'aria-selected': String(id === current), dataset: { tab: id }, text, onclick: e => { e.stopPropagation(); this.show(id); } }), `${text}\nLeft and right arrow keys move between the tabs.`)));
    const panels = this.tabs.map(([id, , parts]) => { const p = el('div', { class: 'pbt-tabpanel', role: 'tabpanel', dataset: { tab: id } }, ...parts); p.hidden = id !== current; return p; });
    const header = el('header', {}, el('h2', { text: 'Skins & Tools for Power BI' }), nav,
      pbt.tip(el('button', { class: 'pbt-x', 'aria-label': 'Close', text: '\u00d7', onclick: e => { e.stopPropagation(); this.close(); } }), 'Close\nEsc does the same.'));
    // The header is the drag handle (its buttons excepted); a double-click on it centers the panel again.
    header.addEventListener('pointerdown', e => this.drag(e));
    header.addEventListener('dblclick', e => { if (e.target.closest('button')) return; this.place(null); pbt.ui.set('help.pos', null); });

    // The footer under every tab: whose work this is, then the provenance fingerprint, small and
    // muted (pbt.fingerprint, page/00-core.js).
    const footer = el('footer', { class: 'pbt-help-foot' }, el('span', { text: pbt.fingerprint.credit }), el('span', { class: 'pbt-help-fp', text: pbt.fingerprint.code }));

    this.backdrop = el('div', { class: 'pbt-help-backdrop', 'data-pbt': '', 'data-ae-fp': pbt.fingerprint.code, onpointerdown: e => { if (e.target === this.backdrop) this.close(); } });
    this.panel = el('div', { class: 'pbt-help', 'data-pbt': '', role: 'dialog', 'aria-modal': 'true', 'aria-label': 'Help and keys', tabindex: '-1',
      onkeydown: e => {
        if (e.key !== 'ArrowLeft' && e.key !== 'ArrowRight') return;
        const ids = this.tabs.map(t => t[0]), i = ids.indexOf(this.current);
        e.preventDefault(); e.stopPropagation();
        this.show(ids[(i + (e.key === 'ArrowRight' ? 1 : ids.length - 1)) % ids.length]);
      } }, header, ...panels, footer);
    this.current = current;
    this.backdrop.append(this.panel);
    document.body.append(this.backdrop);
    // Where it was last dragged to, kept with the last tab; placed after it is in the page, so its
    // size is known. On opening, the whole panel is brought inside the window when it fits, so a
    // panel left half off the edge comes back whole (and the smoke check's bounds still hold).
    this.pos = null;
    this.place(pbt.ui.get('help.pos'), true);
    this.panel.focus();
  },
  // Moves the panel so its top left corner is at pos ({ x, y } in CSS pixels), or back to the
  // middle of the window when pos is null. While dragging, at least 40 px of it stays inside the
  // window on the left, right and bottom, and its top never goes above the window, so the header
  // can always be grabbed again. With whole set, the entire panel is kept inside instead, as far
  // as it fits.
  place(pos, whole) {
    const s = this.panel && this.panel.style;
    if (!s) return;
    if (!pos || !Number.isFinite(pos.x) || !Number.isFinite(pos.y)) {
      s.position = s.left = s.top = s.margin = '';
      this.pos = null;
      return;
    }
    const keep = 40, w = this.panel.offsetWidth, h = this.panel.offsetHeight;
    const minX = whole ? 0 : keep - w, maxX = whole ? window.innerWidth - w : window.innerWidth - keep;
    const maxY = whole ? window.innerHeight - h : window.innerHeight - keep;
    const x = Math.round(Math.max(Math.min(pos.x, maxX), minX));
    const y = Math.round(Math.max(0, Math.min(pos.y, maxY)));
    s.position = 'fixed'; s.margin = '0'; s.left = `${x}px`; s.top = `${y}px`;
    this.pos = { x, y };
  },
  // A drag on the header moves the panel. Pointer capture keeps the moves coming while the pointer
  // is outside the header or the window, and a few pixels of slack let a plain click stay a click.
  // The tab buttons and the close button are left to their own clicks.
  drag(e) {
    if (e.button !== 0 || !this.panel || e.target.closest('button')) return;
    const handle = e.currentTarget, r = this.panel.getBoundingClientRect();
    const offX = e.clientX - r.left, offY = e.clientY - r.top, startX = e.clientX, startY = e.clientY;
    let moved = false, done = false;
    const move = ev => {
      if (!moved && Math.abs(ev.clientX - startX) < 3 && Math.abs(ev.clientY - startY) < 3) return;
      moved = true;
      this.place({ x: ev.clientX - offX, y: ev.clientY - offY });
    };
    const end = () => {
      if (done) return;
      done = true;
      handle.removeEventListener('pointermove', move);
      handle.removeEventListener('pointerup',)PBT"
LR"PBT( end);
      handle.removeEventListener('pointercancel', end);
      handle.removeEventListener('lostpointercapture', end);
      if (moved && this.pos) pbt.ui.set('help.pos', this.pos);
    };
    handle.addEventListener('pointermove', move);
    handle.addEventListener('pointerup', end);
    handle.addEventListener('pointercancel', end);
    handle.addEventListener('lostpointercapture', end);
    try { handle.setPointerCapture(e.pointerId); } catch {}
  },
  show(id) {
    if (!this.panel) return;
    this.current = id;
    this.panel.querySelectorAll('[role="tab"]').forEach(t => t.setAttribute('aria-selected', String(t.dataset.tab === id)));
    this.panel.querySelectorAll('.pbt-tabpanel').forEach(p => { p.hidden = p.dataset.tab !== id; });
    this.panel.scrollTop = 0;
    pbt.ui.set('help.tab', id);
  },
  close() {
    if (this.backdrop) this.backdrop.remove();
    this.backdrop = this.panel = null;
  },
  start() {
    pbt.hotkey('h', 'Help and keys', () => this.open(), { desc: 'This page: skins, tools, extras, keys and settings.' });
    pbt.on(document, 'keydown', e => { if (e.key === 'Escape' && this.panel) { e.stopPropagation(); this.close(); } }, true);
    // A moved panel is pulled back inside the window when the window gets smaller.
    pbt.on(window, 'resize', () => { if (this.panel && this.pos) this.place(this.pos); });
  },
  stop() { this.close(); },
};
pbt.help = help;
pbt.module(help);

// ---- 80-chaos.js ----
// Chaos Mode: the hidden Easter egg (the "Hidden surprises" setting, on by default). The Konami code (Up Up Down Down Left
// Right Left Right B A) turns it on and off; Esc twice within a second also turns it off. On, it
// shows a banner with an 8-bit fanfare, switches on the Power BI 95 look (pbt.retro95, a separate
// module) when that is loaded, and brings in Barry, an unhelpful bar chart assistant with googly
// eyes. It stays on until it is left, and a small note at the bottom of the window says how for as
// long as it is on. A pie or donut chart on the page gets a fake blue crash screen once per page
// (any key or click dismisses it; a key with Ctrl, Alt or Win still reaches Power BI, a plain key
// does not), G drops every visual to
// the bottom of the canvas and floats it back, and a finished refresh plays a dial-up handshake,
// confetti and an achievement. Everything is drawn on top of the report: visuals move only by a
// Web Animations translate that is cancelled afterwards, nothing in the report or its files
// changes, every sound is synthesized with WebAudio and plays only while the Skin sounds switch
// is on (as the Begin-menu programs in page/82 to 84 do), and off() leaves the page as it was.
//
// Contract, so the lead can inject this file on its own into a page where the mod already runs:
// - It needs only `pbt` (window.__pbt) in scope. Everything else sits inside one block, so its
//   names never meet another page file's.
// - It registers as a module (pbt.chaos, then pbt.module): the core calls start() at boot and
//   stop() from pbt.remove(). Evaluated after the core has booted, call window.__pbt.chaos.start()
//   (start() is safe to call twice). Evaluating the file again replaces the previous copy.
// - Test hook, the same object: window.__pbt.chaos.on(), off(), say(text), crash(), gravity(),
//   finale(), slot(), fedUp(), beginMenu(), balloon(), runaway(), state() and audit(). The hooks
//   need no keyboard. say() and fedUp() need Chaos Mode on, beginMenu() the Power BI 95 look; the
//   others run either way and are cleared by off(). audit() returns { styles, pbtNodes, ... } for
//   a before-and-after comparison on the same page.
// - Its styles come with it: a <style data-pbt> element added by on() (or a hook that draws) and
//   removed by off() and stop().
//
// Click eggs, only while Chaos Mode is on, none of which stops a click from reaching Power BI:
// a card's big number spins a slot reel; five clicks on Barry within 4 s make him fed up (he
// turns into a pie and crashes the report himself); a triple-clicked column or bar inflates and
// pops; eight clicks on one slicer within 3 s make it dodge the pointer for 5 s. Power BI's own
// elements are only ever moved, scaled or faded by Web Animations, all cancelled by off().
// - While Chaos Mode is on, backup keys fire each beat at once: Ctrl+Alt+1 Barry's next line,
//   Ctrl+Alt+2 the crash screen, Ctrl+Alt+3 gravity, Ctrl+Alt+4 the refresh finale.
{
  if (pbt.chaos && typeof pbt.chaos.remove === 'function') { try { pbt.chaos.remove(); } catch {} }

  // ---- lines ----
  const CHAOS_LINES = {
    joke: [
      'Have you tried CALCULATE? It\'s in everything. Like glitter.',
      'Fun fact: "it depends" is the correct answer to 90% of Power BI questions.',
      'Pie chart detected. I have alerted the authorities.',
      'I see a measure called "Measure 7". Bold.',
      'Every visual is a KPI if you believe in yourself.',
      'Tip: if the numbers look wrong, add more decimals.',
      'Bidirectional filters: because one direction was too easy.',
      'Your star schema looks a bit like a snowflake today.',
      'A table with 40 columns is just a spreadsheet in disguise.',
      'Have you tried turning the filter context off and on again?',
      'Sorting months alphabetically? April first it is.',
      'Pro tip: rename "Sheet1" before anyone sees it.',
      'Is this a dashboard or a mosaic?',
      'I counted the colors on this page. You have... many.',
      'Behind every great report is a SUMX nobody understands.',
      'Gauges are just pie charts with ambition.',
      'Is that Comic Sans? No? Would you like it to be?',
      'BLANK() is my favorite value. So honest.',
      'An analyst once said: "Why is this total wrong?"',
      'Add a card. Then another card. Cards all the way down.',
      'Many-to-many? I respect the chaos.',
      'My cousin, the scatter chart, is all over the place.',
      'Your date table called. It wants to be marked.',
      ')PBT"
LR"PBT(This report needs more drop shadows. Said nobody.',
      'I would explain row context, but I am just three bars.',
    ],
    page: ['New page! Same data, new mysteries.', 'Ooh, a fresh page. Shall I ruin it?', 'Page switch! I had grown attached to that one.'],
    visual: ['A new visual! Is it a pie chart? Please?', 'Another visual. The canvas grows ever fuller.', 'New visual spotted. Want me to rotate it 45 degrees?'],
    slicer: ['This slicer slices. Amaze.', 'Slicer clicked. Filter context has entered the chat.', 'Careful, that slicer is load-bearing.'],
    dax: ['Back from DAX query view! Did EVALUATE evaluate?', 'DAX query view: where measures go to be judged.'],
    model: ['Back from Model view. Mind the spaghetti.', 'Model view! So many lines. So few of them mine.'],
    idle: ['Still there? I could make it worse while you wait.', 'Thirty seconds of silence. Is the refresh still going?', 'Thinking about pie charts? Me too.'],
    refresh: ['Refreshing... a good time for coffee.', 'Refresh started. Pray to the gateway.', 'Refresh! Hold on, I\'m dialing in.'],
    achievement: ['Used CALCULATE without crying', 'Refreshed on a Friday', 'Survived a many-to-many', 'Renamed Sheet1', 'Zero pie charts harmed', 'Got the filter context right first time', 'Turned off a bidirectional filter'],
  };
  const CHAOS_INTRO = 'Hi! I\'m Barry. It looks like you\'re building a report. Would you like me to make it worse?';
  const CHAOS_PIE = 'We don\'t talk about pie charts.';

  // ---- styles ----
  const CHAOS_CSS = `
.pbt-chaos-banner { position: fixed; inset: 0; z-index: 2147483005; display: grid; place-items: center; pointer-events: none; background: radial-gradient(circle at 50% 50%, rgba(0, 0, 0, .25), rgba(0, 0, 0, .7)); animation: pbt-chaos-fade 2s ease forwards; }
.pbt-chaos-banner > div { text-align: center; animation: pbt-chaos-zoom 2s cubic-bezier(.2, 1.4, .4, 1) forwards; }
.pbt-chaos-banner b { display: block; font: 900 min(14vw, 150px)/1 Impact, "Arial Black", sans-serif; letter-spacing: .04em; color: #fff; background: linear-gradient(90deg, #ff3b3b, #ffb000, #ffe600, #2bd96b, #2aa8ff, #b84dff, #ff3b3b); background-size: 200% 100%; -webkit-background-clip: text; background-clip: text; -webkit-text-fill-color: transparent; filter: drop-shadow(4px 4px 0 #000); animation: pbt-chaos-rainbow .6s linear infinite, pbt-chaos-shake .12s linear infinite; }
.pbt-chaos-banner small { display: block; margin-top: 10px; font: 700 22px/1.2 "Lucida Console", Consolas, monospace; color: #fff; text-shadow: 2px 2px 0 #000; }
@keyframes pbt-chaos-fade { 0% { opacity: 0; } 8% { opacity: 1; } 80% { opacity: 1; } 100% { opacity: 0; } }
@keyframes pbt-chaos-zoom { 0% { transform: scale(.2) rotate(-12deg); } 30% { transform: scale(1.08) rotate(2deg); } 100% { transform: scale(1) rotate(0); } }
@keyframes pbt-chaos-rainbow { to { background-position: 200% 0; } }
@keyframes pbt-chaos-shake { 0% { transform: translate(0, 0); } 25% { transform: translate(2px, -1px); } 50% { transform: translate(-1px, 2px); } 75% { transform: translate(-2px, -1px); } 100% { transform: translate(0, 0); } }

.pbt-toast.pbt-chaos-exit { position: fixed; left: 50%; bottom: 76px; z-index: 2147483001; max-width: min(420px, 80vw); padding: 6px 12px; font-size: 12px; pointer-events: none; transform: translateX(-50%); }
.pbt-toast.pbt-chaos-exit.pbt-in { transform: translateX(-50%); }

.pbt-chaos-barry { position: fixed; right: 28px; bottom: 40px; z-index: 2147483003; width: 128px; height: 141px; pointer-events: none; animation: pbt-chaos-in .8s cubic-bezier(.3, 1.6, .5, 1) both; }
.pbt-chaos-barry > svg { position: absolute; inset: 0; width: 128px; height: 141px; pointer-events: auto; cursor: pointer; overflow: visible; animation: pbt-chaos-bob 1.6s ease-in-out infinite alternate; }
.pbt-chaos-eyes { transform-box: fill-box; transform-origin: center; animation: pbt-chaos-blink 4.2s infinite; }
@keyframes pbt-chaos-in { 0% { transform: translateY(170px) scale(.5); opacity: 0; } 60% { opacity: 1; } 100% { transform: none; opacity: 1; } }
@keyframes pbt-chaos-bob { from { transform: translateY(0); } to { transform: translateY(-4px); } }
@keyframes pbt-chaos-blink { 0%, 93%, 100% { transform: scaleY(1); } 96% { transform: scaleY(.1); } }
.pbt-chaos-x { position: absolute; top: -6px; right: -6px; width: 20px; height: 20px; padding: 0; border: 1px solid #333; border-radius: 50%; background: #fff; color: #222; font: 700 12px/18px "Segoe UI", sans-serif; text-align: center; cursor: pointer; pointer-events: auto; box-shadow: 0 1px 3px rgba(0, 0, 0, .3); }
.pbt-chaos-x:hover { background: #ffe0e0; }
.pbt-chaos-bubble { position: absolute; right: 18px; bottom: calc(100% + 14px); box-sizing: border-box; width: max-content; min-width: 170px; max-width: 300px; padding: 10px 13px; border: 2px solid #222; border-radius: 14px; background: #fffbe6; color: #111; font: 600 15px/1.35 "Segoe UI", sans-serif; text-align: left; pointer-events: auto; box-shadow: 3px 3px 0 rgba(0, 0, 0, .35); }
.pbt-chaos-bubble[hidden] { display: none; }
.pbt-chaos-bubble::after { content: ""; position: absolute; right: 26px; bottom: -12px; border: 6px solid transparent; border-top: 8px solid #222; border-bottom: 0; transform: translateY(2px); }
.pbt-chaos-text { min-height: 1.35em; white-space: pre-wrap; }
.pbt-chaos-actions { display: flex; flex-wrap: wrap; gap: 6px; margin-top: 8px; }
.pbt-chaos-actions:empty { display: none; }
.pbt-chaos-actions button { padding: 3px 10px; border: 1px solid #333; border-radius: 6px; background: #fff; color: #111; font: 600 12px/1.4 "Segoe UI", sans-serif; cursor: pointer; }
.pbt-chaos-actions button:hover { background: #2aac94; color: #fff; }

.pbt-chaos-bsod { position: fixed; inset: 0; z-index: 2147483646; box-sizing: border-box; padding: 9vh 10vw; background: #0000aa; color: #fff; font: 20px/1.5 "Lucida Console", Consolas, monospace; cursor: default; user-select: none; }
.pbt-chaos-bsod h1 { margin: 0 0 4vh; font: 400 min(18vh, 140px))PBT"
LR"PBT(/1 "Segoe UI", sans-serif; color: #fff; }
.pbt-chaos-bsod p { margin: 0 0 2.6vh; max-width: 900px; color: #fff; }
.pbt-chaos-bsod .pbt-chaos-row { display: flex; gap: 24px; align-items: center; margin-top: 4vh; }
.pbt-chaos-bsod svg { width: 96px; height: 96px; flex: none; }
.pbt-chaos-bsod .pbt-chaos-blinky { animation: pbt-chaos-cursor 1s steps(1) infinite; }
@keyframes pbt-chaos-cursor { 50% { opacity: 0; } }

.pbt-chaos-ach { position: fixed; left: 50%; bottom: 64px; z-index: 2147483004; display: flex; gap: 14px; align-items: center; box-sizing: border-box; min-width: 360px; padding: 12px 22px 12px 14px; border: 2px solid #c9a227; border-radius: 40px; background: linear-gradient(180deg, #2a2a2a, #111); color: #fff; font: 14px/1.3 "Segoe UI", sans-serif; box-shadow: 0 6px 24px rgba(0, 0, 0, .5); pointer-events: none; transform: translate(-50%, 0); animation: pbt-chaos-ach 5s ease forwards; }
.pbt-chaos-ach svg { width: 44px; height: 44px; flex: none; }
.pbt-chaos-ach small { display: block; color: #f2c94c; font: 700 11px/1.2 "Segoe UI", sans-serif; letter-spacing: .14em; }
.pbt-chaos-ach b { display: block; font: 700 17px/1.3 "Segoe UI", sans-serif; color: #fff; }
@keyframes pbt-chaos-ach { 0% { opacity: 0; transform: translate(-50%, 60px); } 10% { opacity: 1; transform: translate(-50%, 0); } 88% { opacity: 1; transform: translate(-50%, 0); } 100% { opacity: 0; transform: translate(-50%, 20px); } }

.pbt-chaos-bars, .pbt-chaos-pie { transform-box: fill-box; transform-origin: 50% 50%; }
.pbt-chaos-slot { position: fixed; z-index: 2147483002; box-sizing: border-box; overflow: hidden; pointer-events: none; text-align: center; white-space: nowrap; -webkit-mask-image: linear-gradient(transparent, #000 18%, #000 82%, transparent); mask-image: linear-gradient(transparent, #000 18%, #000 82%, transparent); }
.pbt-chaos-slot > div > div { display: block; overflow: hidden; }
.pbt-chaos-bit { position: fixed; z-index: 2147483002; width: 7px; height: 7px; border-radius: 50%; pointer-events: none; }
`;

  // ---- small helpers ----
  const SVG_NS = 'http://www.w3.org/2000/svg';
  const svgEl = (tag, attrs = {}, ...kids) => {
    const e = document.createElementNS(SVG_NS, tag);
    for (const [k, v] of Object.entries(attrs)) if (v != null) e.setAttribute(k, v);
    for (const c of kids.flat()) if (c != null) e.append(c);
    return e;
  };
  // Built with createElement only, like pbt.el, in case the app turns on Trusted Types.
  const htmlEl = (tag, props = {}, ...kids) => {
    const e = document.createElement(tag);
    for (const [k, v] of Object.entries(props)) {
      if (v == null) continue;
      if (k === 'class') e.className = v;
      else if (k === 'text') e.textContent = v;
      else if (k.startsWith('on')) e.addEventListener(k.slice(2), v);
      else e.setAttribute(k, v);
    }
    for (const c of kids.flat()) if (c != null) e.append(c);
    return e;
  };
  const safe = fn => function (...args) { try { return fn.apply(this, args); } catch (e) { try { pbt.log && pbt.log(`chaos: ${e && e.stack}`); } catch {} } };
  // The same test as the core's hotkeys: a text field or an editor (the formula bar and the DAX
  // and TMDL editors are Monaco) is typing, a switch or a button is not.
  const NO_TEXT = /^(checkbox|radio|range|button|submit|reset|color|file|image|hidden)$/i;
  const typingIn = n => !!n && n.nodeType === 1 && (n.isContentEditable || /^(TEXTAREA|SELECT)$/.test(n.tagName) || (n.tagName === 'INPUT' && !NO_TEXT.test(n.type || '')) || !!(n.closest && n.closest('.monaco-editor, [role="textbox"]')));
  const KONAMI = ['arrowup', 'arrowup', 'arrowdown', 'arrowdown', 'arrowleft', 'arrowright', 'arrowleft', 'arrowright', 'b', 'a'];

  // Everything that runs while the module is started (base) or while Chaos Mode is on (live).
  const S = {
    started: false, on: false, base: [], live: [], timers: new Set(), els: new Set(),
    konami: 0, escAt: 0, ctx: null, master: null, ctxT: null, bags: {},
    barry: null, bubble: null, text: null, actions: null, pupils: [], typing: null, hideAt: null,
    lastSaid: 0, closed: false, crashed: new Set(), crashEl: null, exitEl: null, crashPct: 0,
    pageKey: '', count: 0, pageAt: 0, lastActive: 0, idleSaid: false, pendingView: null,
    grav: null, mouse: null, eyeQueued: false,
    // The click eggs: Web Animations on Power BI's own elements (all cancelled by off()), cards
    // spinning a slot reel, bars being popped, clicks on Barry and on slicers, the runaway slicer.
    anims: new Set(), slots: new Set(), popping: new Set(), hits: new Map(), tri: null, pokes: [], fed: null, run: null, measure: null,
  };
  const listen = (list, target, type, fn, opts) => {
    if (!target) return;
    const h = safe(fn);
    target.addEventListener(type, h, opts);
    list.push(() => target.removeEventListener(type, h, opts));
  };
  const later = (fn, ms) => {
    const t = setTimeout(() => { S.timers.delete(t); safe(fn)(); }, ms);
    S.timers.add(t);
    return t;
  };
  const cancel = t => { if (t != null) { clearTimeout(t); S.timers.delete(t); } };
  const track = el => { el.setAttribute('data-pbt', ''); S.els.add(el); return el; };
  const drop = el => { if (el) { S.els.delete(el); el.remove(); } };
  // A Web Animation that off() can find and cancel. One that keeps no fill forgets itself when done.
  const anim = (el, frames, opts) => {
    if (!el || typeof el.animate !== 'function') return null;
    const a = el.animate(frames, opts);
    S.anims.add(a);
    a.addEventListener('cancel', () => S.anims.delete(a));
    if (!opts.fill || opts.fill === 'none') a.addEventListener('finish', () => S.anims.delete(a));
    return a;
  };
  const unanim = a => { if (a) { S.anims.delete(a); try { a.cancel(); } catch {} } };
  // A random line from a pool, without repeats until every line in it has been used.
  const nextLine = kind => {
    const pool = CHAOS_LINES[kind] || CHAOS_LINES.joke;
    let bag = S.bags[kind];
    if (!bag || !bag.length) {
      bag =)PBT"
LR"PBT( S.bags[kind] = pool.map((_, i) => i);
      for (let i = bag.length - 1; i > 0; i -= 1) { const j = Math.floor(Math.random() * (i + 1)); [bag[i], bag[j]] = [bag[j], bag[i]]; }
    }
    return pool[bag.pop()];
  };

  // ---- sounds: oscillators and filtered noise, no audio files ----
  const audio = () => {
    // Silent unless Skin sounds is on (off by default), like every other sound of the mod.
    if (!pbt.feature('sounds')) return null;
    try {
      if (!S.ctx || S.ctx.state === 'closed') {
        S.ctx = new AudioContext();
        S.master = S.ctx.createGain();
        S.master.gain.value = 0.9;
        S.master.connect(S.ctx.destination);
      }
      if (S.ctx.state === 'suspended') S.ctx.resume().catch(() => {});
      return S.ctx;
    } catch { return null; }
  };
  const tone = ({ type = 'square', f = 440, f2, at = 0, len = 0.1, vol = 0.04 }) => {
    const ctx = audio();
    if (!ctx) return;
    try {
      const o = ctx.createOscillator(), g = ctx.createGain();
      const t = ctx.currentTime + at;
      o.type = type;
      o.frequency.setValueAtTime(f, t);
      if (f2) o.frequency.exponentialRampToValueAtTime(f2, t + len);
      g.gain.setValueAtTime(0.0001, t);
      g.gain.exponentialRampToValueAtTime(vol, t + 0.008);
      g.gain.setValueAtTime(vol, t + Math.max(0.01, len - 0.03));
      g.gain.exponentialRampToValueAtTime(0.0001, t + len);
      o.connect(g).connect(S.master);
      o.start(t); o.stop(t + len + 0.05);
    } catch {}
  };
  // A burst of filtered white noise that dies away.
  const noise = ({ at = 0, len = 0.1, vol = 0.1, type = 'bandpass', f = 2000, q = 1 }) => {
    const ctx = audio();
    if (!ctx) return;
    try {
      const t = ctx.currentTime + at, buf = ctx.createBuffer(1, Math.ceil(ctx.sampleRate * len), ctx.sampleRate), data = buf.getChannelData(0);
      for (let i = 0; i < data.length; i += 1) data[i] = Math.random() * 2 - 1;
      const n = ctx.createBufferSource(), flt = ctx.createBiquadFilter(), g = ctx.createGain();
      n.buffer = buf; flt.type = type; flt.frequency.value = f; flt.Q.value = q;
      g.gain.setValueAtTime(vol, t);
      g.gain.exponentialRampToValueAtTime(0.0001, t + len);
      n.connect(flt).connect(g).connect(S.master);
      n.start(t); n.stop(t + len + 0.02);
    } catch {}
  };
  const sound = {
    // One click of a slot reel, at seconds from now.
    tick(at = 0) { tone({ f: 1900, len: 0.014, vol: 0.03, at }); noise({ at, len: 0.02, vol: 0.05, type: 'highpass', f: 3000 }); },
    // A bell and a shimmer of coins.
    kaching() {
      noise({ len: 0.3, vol: 0.05, type: 'highpass', f: 6000 });
      tone({ type: 'triangle', f: 1975.5, at: 0.02, len: 0.16, vol: 0.07 });
      tone({ type: 'triangle', f: 2637, at: 0.13, len: 0.55, vol: 0.07 });
      tone({ type: 'sine', f: 3951, at: 0.13, len: 0.5, vol: 0.02 });
    },
    pop() { noise({ len: 0.08, vol: 0.3, type: 'lowpass', f: 2200 }); tone({ type: 'sine', f: 560, f2: 90, len: 0.09, vol: 0.12 }); },
    angry() { tone({ type: 'sawtooth', f: 110, f2: 92, len: 0.4, vol: 0.05 }); tone({ type: 'sawtooth', f: 117, f2: 95, len: 0.4, vol: 0.04 }); },
    // A rising arpeggio and a held chord, in square waves.
    fanfare() {
      [523.25, 659.25, 783.99, 1046.5, 783.99, 1046.5].forEach((f, i) => tone({ f, at: i * 0.085, len: 0.09, vol: 0.035 }));
      [783.99, 1046.5, 1318.5].forEach(f => tone({ f, at: 0.55, len: 0.6, vol: 0.025 }));
    },
    off() {
      tone({ f: 880, f2: 330, len: 0.18, vol: 0.035 });
      tone({ f: 440, f2: 110, at: 0.16, len: 0.22, vol: 0.035 });
    },
    blip() { tone({ f: 520 + Math.random() * 380, len: 0.025, vol: 0.008 }); },
    boop() { tone({ type: 'sine', f: 620, f2: 260, len: 0.24, vol: 0.09 }); },
    chime() { tone({ type: 'triangle', f: 987.77, len: 0.14, vol: 0.06 }); tone({ type: 'triangle', f: 1318.5, at: 0.12, len: 0.4, vol: 0.06 }); },
    // A slide whistle: a sine glide with a little vibrato.
    whistle(from, to, len) {
      const ctx = audio();
      if (!ctx) return;
      try {
        const t = ctx.currentTime, o = ctx.createOscillator(), g = ctx.createGain(), lfo = ctx.createOscillator(), depth = ctx.createGain();
        o.type = 'sine';
        o.frequency.setValueAtTime(from, t);
        o.frequency.exponentialRampToValueAtTime(to, t + len);
        lfo.frequency.value = 7; depth.gain.value = 14;
        lfo.connect(depth).connect(o.frequency);
        g.gain.setValueAtTime(0.0001, t);
        g.gain.exponentialRampToValueAtTime(0.06, t + 0.04);
        g.gain.setValueAtTime(0.06, t + len - 0.08);
        g.gain.exponentialRampToValueAtTime(0.0001, t + len);
        o.connect(g).connect(S.master);
        o.start(t); lfo.start(t); o.stop(t + len + 0.05); lfo.stop(t + len + 0.05);
      } catch {}
    },
    // A dial-up handshake, about 3.3 s: DTMF digits, the answer tone, a warbling carrier, then
    // bursts of band-passed noise over a high whine. Returns its length in seconds.
    modem() {
      const ctx = audio();
      if (!ctx) return 0;
      try {
        const DTMF = { 1: [697, 1209], 2: [697, 1336], 3: [697, 1477], 4: [770, 1209], 5: [770, 1336], 6: [770, 1477], 7: [852, 1209], 8: [852, 1336], 9: [852, 1477], 0: [941, 1336] };
        [...'5551234'].forEach((d, i) => DTMF[d].forEach(f => tone({ type: 'sine', f, at: i * 0.11, len: 0.075, vol: 0.03 })));
        tone({ type: 'sine', f: 2100, at: 0.9, len: 0.5, vol: 0.03 });
        const t0 = ctx.currentTime;
        const w = ctx.createOscillator(), wg = ctx.createGain();
        w.type = 'square';
        for (let k = 0; k < 20; k += 1) w.frequency.setValueAtTime(k % 2 ? 1180 : 980, t0 + 1.45 + k * 0.025);
        wg.gain.setValueAtTime(0.0001, t0 + 1.45);
        wg.gain.exponentialRampToValueAtTime(0.018, t0 + 1.47);
        wg.gain.setValueAtTime(0.018, t0 + 1.92);
        wg.gain.exponentialRampToValueAtTime(0.0001, t0 + 1.96);
        w.connect(wg).connect(S.master);
        w.start(t0 + 1.45); w.stop(t0 + 2);
        const len = )PBT"
LR"PBT(1.35, buf = ctx.createBuffer(1, Math.ceil(ctx.sampleRate * len), ctx.sampleRate), data = buf.getChannelData(0);
        for (let i = 0; i < data.length; i += 1) data[i] = Math.random() * 2 - 1;
        const n = ctx.createBufferSource(), bp = ctx.createBiquadFilter(), ng = ctx.createGain();
        n.buffer = buf; bp.type = 'bandpass'; bp.frequency.value = 1800; bp.Q.value = 0.8;
        ng.gain.setValueAtTime(0.0001, t0 + 1.95);
        for (let k = 0; k < 13; k += 1) {
          const tk = t0 + 1.98 + k * 0.1, peak = 0.05 + Math.random() * 0.05;
          ng.gain.setValueAtTime(0.0001, tk);
          ng.gain.exponentialRampToValueAtTime(peak, tk + 0.012);
          ng.gain.setValueAtTime(peak, tk + 0.05 + Math.random() * 0.02);
          ng.gain.exponentialRampToValueAtTime(0.0001, tk + 0.09);
        }
        n.connect(bp).connect(ng).connect(S.master);
        n.start(t0 + 1.95); n.stop(t0 + 1.95 + len);
        tone({ type: 'sine', f: 2400, f2: 2250, at: 2.0, len: 1.25, vol: 0.012 });
        return 3.35;
      } catch { return 0; }
    },
  };

  // ---- Barry ----
  const buildBarry = () => {
    const eye = (cx, cy) => {
      const white = svgEl('circle', { cx, cy, r: 7, fill: '#fff', stroke: '#1d1d1d', 'stroke-width': 1.4 });
      const pupil = svgEl('circle', { cx, cy, r: 3.1, fill: '#111' });
      S.pupils.push({ white, pupil, cx, cy });
      return [white, pupil];
    };
    // The pie he turns into when he is fed up: bigger, googlier eyes that wobble further.
    const googly = (cx, cy) => {
      const white = svgEl('circle', { cx, cy, r: 8.5, fill: '#fff', stroke: '#1d1d1d', 'stroke-width': 1.4 });
      const pupil = svgEl('circle', { cx, cy, r: 4, fill: '#111' });
      S.pupils.push({ white, pupil, cx, cy, m: 4.5 });
      return [white, pupil];
    };
    S.pupils = [];
    const line = { stroke: '#1d1d1d', 'stroke-width': 1.6 };
    const body = svgEl('svg', { viewBox: '0 0 100 110', 'aria-label': 'Barry, the unhelpful assistant', role: 'img' },
      svgEl('ellipse', { cx: 50, cy: 104, rx: 42, ry: 4.5, fill: 'rgba(0,0,0,.2)' }),
      svgEl('g', { class: 'pbt-chaos-bars' },
        svgEl('rect', { class: 'pbt-chaos-bar', x: 9, y: 56, width: 22, height: 44, rx: 3, fill: '#f2c94c', ...line }),
        svgEl('rect', { class: 'pbt-chaos-bar', x: 69, y: 38, width: 22, height: 62, rx: 3, fill: '#5ad1ff', ...line }),
        svgEl('rect', { class: 'pbt-chaos-bar', x: 38, y: 12, width: 25, height: 88, rx: 3, fill: '#2aac94', ...line }),
        svgEl('path', { d: 'M44 47 Q50.5 54 57 47', fill: 'none', stroke: '#0d3b33', 'stroke-width': 2.4, 'stroke-linecap': 'round' }),
        svgEl('g', { class: 'pbt-chaos-eyes' }, eye(44, 29), eye(57.5, 29))),
      svgEl('g', { class: 'pbt-chaos-pie', opacity: 0 },
        svgEl('circle', { cx: 50, cy: 62, r: 36, fill: '#2aac94', ...line }),
        svgEl('path', { d: 'M50 62 L50 26 A36 36 0 0 1 81.18 80 Z', fill: '#f2c94c', ...line }),
        svgEl('path', { d: 'M50 62 L81.18 80 A36 36 0 0 1 22.42 85.14 Z', fill: '#5ad1ff', ...line }),
        googly(40, 52), googly(61, 52),
        svgEl('path', { d: 'M38 75 q4 -4 8 0 t8 0 t8 0', fill: 'none', stroke: '#1d1d1d', 'stroke-width': 2.2, 'stroke-linecap': 'round' })),
      svgEl('rect', { x: 5, y: 99, width: 90, height: 3.5, rx: 1.5, fill: '#1d1d1d' }));
    S.text = htmlEl('div', { class: 'pbt-chaos-text' });
    S.actions = htmlEl('div', { class: 'pbt-chaos-actions' });
    S.bubble = htmlEl('div', { class: 'pbt-chaos-bubble', role: 'status' }, S.text, S.actions);
    S.bubble.hidden = true;
    const x = htmlEl('button', { class: 'pbt-chaos-x', type: 'button', 'aria-label': 'Close Barry', title: 'Close Barry', text: 'x' });
    const root = track(htmlEl('div', { class: 'pbt-chaos-barry', 'data-ae-fp': (pbt.fingerprint || {}).code || '' }, S.bubble, body, x));
    // Clicks on Barry stay with Barry; Power BI never sees them.
    for (const type of ['pointerdown', 'mousedown', 'click', 'dblclick', 'contextmenu']) root.addEventListener(type, e => e.stopPropagation());
    x.addEventListener('click', safe(() => { S.closed = true; hideBarry(); }));
    body.addEventListener('click', safe(pokeBarry));
    document.body.append(root);
    S.barry = root;
    place();
  };
  const hideBarry = () => {
    unFed();
    stopTyping();
    cancel(S.hideAt);
    drop(S.barry);
    S.barry = S.bubble = S.text = S.actions = null;
    S.pupils = [];
  };
  // Above Power BI's status bar, at the right.
  const place = () => {
    if (!S.barry) return;
    const sb = document.querySelector('section.statusBar');
    const r = sb && sb.getBoundingClientRect();
    const bottom = r && r.height && r.top > innerHeight / 2 ? innerHeight - r.top + 10 : 16;
    S.barry.style.bottom = `${Math.round(bottom)}px`;
  };
  // The pupils look at the pointer, at most 3 units off the middle of each eye.
  const look = () => {
    S.eyeQueued = false;
    if (!S.mouse || !S.pupils.length) return;
    for (const p of S.pupils) {
      const r = p.white.getBoundingClientRect();
      if (!r.width) continue;
      const dx = S.mouse[0] - (r.left + r.width / 2), dy = S.mouse[1] - (r.top + r.height / 2);
      const dist = Math.hypot(dx, dy) || 1, k = Math.min(p.m || 3, dist / 25) / dist;
      p.pupil.setAttribute('cx', (p.cx + dx * k).toFixed(2));
      p.pupil.setAttribute('cy', (p.cy + dy * k).toFixed(2));
    }
  };
  const stopTyping = () => { if (S.typing) { clearInterval(S.typing); S.typing = null; } };
  // Barry says a line, typed out with a quiet blip per character. Reactions keep a 6 s cooldown
  // and wait while the intro question is fresh (9 s) or Barry is closed; force skips all three
  // (the intro, the pie line, a click on Barry, the test hook and the backup key).
  const say = (text, opts = {}) => {
    if (!S.on || !text) return false;
    const now = Date.now();
    if (!opts.force && (S.closed || now - S.lastSaid < 6000 || now < S.choosing)) return false;
    if (opts.force) S.closed = false;
    if )PBT"
LR"PBT((!S.barry) buildBarry();
    S.lastSaid = now;
    S.choosing = opts.buttons ? now + 9000 : 0;
    stopTyping();
    cancel(S.hideAt);
    S.bubble.hidden = false;
    S.text.textContent = '';
    const buttons = opts.buttons || [['Gravity!', () => gravity()]];
    S.actions.replaceChildren(...buttons.map(([label, fn]) => htmlEl('button', { type: 'button', text: label, onclick: safe(e => { e.stopPropagation(); fn(); }) })));
    let i = 0;
    S.typing = setInterval(safe(() => {
      if (!S.text) return stopTyping();
      i += 1;
      S.text.textContent = text.slice(0, i);
      if (i % 2 && text[i - 1] !== ' ') sound.blip();
      if (i >= text.length) stopTyping();
    }), 24);
    S.hideAt = later(() => { if (S.bubble) S.bubble.hidden = true; }, opts.hold || text.length * 24 + 5000);
    return true;
  };
  const react = kind => say(nextLine(kind));
  const intro = () => say(CHAOS_INTRO, {
    force: true, hold: 20000, buttons: [
      ['Yes', () => { say('Great! Press G for gravity. You\'re welcome.', { force: true }); }],
      ['Also yes', () => { say('I like your energy. Worse it is.', { force: true }); }],
    ],
  });

  // ---- Barry gets fed up: five clicks within 4 s turn him red, then into a pie, then crash ----
  const pokeBarry = () => {
    if (S.fed) return;
    const now = Date.now();
    S.pokes = S.pokes.filter(t => now - t < 4000);
    S.pokes.push(now);
    if (S.on && S.pokes.length >= 5) { S.pokes = []; fedUp(); return; }
    say(nextLine('joke'), { force: true });
  };
  const fedUp = () => {
    if (!S.on || S.fed || S.crashEl) return false;
    const f = S.fed = { stage: 'red', anims: [] };
    const keep = a => { if (a) f.anims.push(a); return a; };
    say('I\'m a bar chart, not a button!', { force: true, hold: 9000, buttons: [] });
    if (!S.barry) { unFed(); return false; }
    sound.angry();
    S.barry.querySelectorAll('.pbt-chaos-bar').forEach(r => keep(anim(r, [{ fill: '#e5383b' }], { duration: 250, fill: 'forwards' })));
    keep(anim(S.barry, [{ translate: '0 0' }, { translate: '-6px 0' }, { translate: '6px 0' }, { translate: '-4px 0' }, { translate: '4px 0' }, { translate: '0 0' }], { duration: 400, iterations: 3 }));
    // Over about 2 s the bars spin away to nothing while a pie spins up out of nothing.
    later(() => {
      if (S.fed !== f || !S.barry) return;
      f.stage = 'morph';
      sound.whistle(1200, 300, 1.9);
      keep(anim(S.barry.querySelector('.pbt-chaos-bars'), [
        { transform: 'none', opacity: 1 },
        { transform: 'rotate(120deg) scale(.6, .3)', opacity: 0.85, offset: 0.5 },
        { transform: 'rotate(300deg) scale(0)', opacity: 0 },
      ], { duration: 1500, easing: 'ease-in', fill: 'forwards' }));
      keep(anim(S.barry.querySelector('.pbt-chaos-pie'), [
        { transform: 'rotate(-270deg) scale(0)', opacity: 0 },
        { transform: 'rotate(-90deg) scale(.4)', opacity: 1, offset: 0.35 },
        { transform: 'rotate(20deg) scale(1.12)', opacity: 1, offset: 0.8 },
        { transform: 'none', opacity: 1 },
      ], { duration: 2000, easing: 'ease-out', fill: 'forwards' }));
      later(() => {
        if (S.fed !== f) return;
        f.stage = 'pie';
        say('Oh no. Oh no no no.', { force: true, hold: 8000, buttons: [] });
        // His own crash screen; dismissing it (dismissCrash) turns him back into bars.
        later(() => { if (S.fed === f) { f.stage = 'crash'; crash(); } }, 1900);
      }, 2050);
    }, 1500);
    return true;
  };
  // Back to bars: every animation of the fed-up run cancelled.
  const unFed = () => {
    const f = S.fed;
    if (!f) return false;
    S.fed = null;
    f.anims.forEach(unanim);
    return true;
  };

  // ---- the page: which page is open, how many visuals, any pie or donut ----
  const pageKey = () => {
    try {
      const li = document.querySelector('.carouselScrollPane li.section.selected');
      if (li) return li.textContent.trim();
      const p = pbt.project && pbt.project.page && pbt.project.page();
      return (p && p.name) || 'page';
    } catch { return 'page'; }
  };
  const containers = () => [...document.querySelectorAll('.displayArea .visualContainer')].filter(el => !(el.parentElement && el.parentElement.closest('.visualContainer')));
  const kinds = () => {
    try {
      if (pbt.page && typeof pbt.page.visuals === 'function') return pbt.page.visuals().map(v => [v.kind || '', v.typeName || '']);
    } catch {}
    return containers().map(el => {
      const type = el.querySelector('[class*="visual-"]');
      return [el.getAttribute('aria-roledescription') || '', (((type && type.getAttribute('class')) || '').match(/\bvisual-(\w+)/) || [])[1] || ''];
    });
  };
  const visualCount = () => {
    try { if (pbt.page && typeof pbt.page.visuals === 'function') return pbt.page.visuals().length; } catch {}
    return containers().length;
  };
  const hasPie = () => kinds().some(([kind, typeName]) => /\b(pie|donut|doughnut)\b/i.test(kind) || /^(pie|donut|doughnut)/i.test(typeName));
  // Once per page per Chaos Mode, and only when a pie or donut is really there.
  const maybeCrash = () => {
    if (!S.on || S.crashEl || !hasPie()) return;
    const key = pageKey();
    if (S.crashed.has(key)) return;
    S.crashed.add(key);
    crash();
  };

  // ---- the blue crash screen ----
  const crash = () => {
    if (S.crashEl) return false;
    ensureStyle();
    const pct = htmlEl('span', { text: '0' });
    // The pie with its missing slice (from 315 to 360 degrees, top left) pulled out along the
    // slice's middle, so it reads as an exploded pie.
    const pie = svgEl('svg', { viewBox: '-8 -10 116 116', 'aria-hidden': 'true' },
      svgEl('path', { d: 'M50 50 L50 6 A44 44 0 1 1 18.9 18.9 Z', fill: '#fff' }),
      svgEl('path', { d: 'M50 50 L18.9 18.9 A44 44 0 0 1 50 6 Z', fill: '#fff', transform: 'translate(-3 -7.4)' }));
    const el = track(htmlEl('div', { class: 'pbt-chaos-bsod', role: 'alertdialog', 'aria-label': 'Fake crash screen. Press an)PBT"
LR"PBT(y key to continue.' },
      htmlEl('h1', { text: ':(' }),
      htmlEl('p', { text: 'Power BI ran into a problem it couldn\'t handle.' }),
      htmlEl('p', {}, pct, '% complete, collecting your regrets'),
      htmlEl('div', { class: 'pbt-chaos-row' }, pie, htmlEl('div', {},
        htmlEl('p', { text: 'STOP CODE: PIE_CHART_DETECTED' }),
        htmlEl('p', { text: 'What failed: humans.cannot.compare.angles.sys' }))),
      htmlEl('p', {}, 'Press any key to continue', htmlEl('span', { class: 'pbt-chaos-blinky', text: '_' }))));
    el.addEventListener('pointerdown', safe(e => { e.preventDefault(); e.stopPropagation(); dismissCrash(); }));
    document.body.append(el);
    S.crashEl = el;
    S.crashPct = 0;
    const step = () => {
      if (S.crashEl !== el) return;
      S.crashPct = Math.min(100, S.crashPct + 1 + Math.floor(Math.random() * 7));
      pct.textContent = String(S.crashPct);
      if (S.crashPct < 100) later(step, 70 + Math.random() * 160);
    };
    later(step, 400);
    return true;
  };
  const dismissCrash = () => {
    if (!S.crashEl) return false;
    drop(S.crashEl);
    S.crashEl = null;
    sound.boop();
    // A crash Barry caused himself (fedUp) ends with him a bar chart again.
    const fed = unFed();
    if (S.on) later(() => say(fed ? 'Let us never speak of this.' : CHAOS_PIE, { force: true }), 450);
    return true;
  };

  // ---- gravity: a Web Animations translate on each visual container, cancelled afterwards ----
  const gravity = () => {
    if (S.grav) return false;
    const da = document.querySelector('.displayArea');
    if (!da) return false;
    const floor = Math.min(da.getBoundingClientRect().bottom, innerHeight);
    const items = containers().map(el => ({ el, r: el.getBoundingClientRect() })).filter(v => v.r.width > 0 && v.r.height > 0 && typeof v.el.animate === 'function');
    if (!items.length) return false;
    items.sort((a, b) => a.r.left - b.r.left);
    const g = S.grav = { anims: [] };
    sound.whistle(1500, 260, 0.9);
    items.forEach((v, i) => {
      // The page is scaled to the zoom, so screen pixels become the container's own pixels.
      const scale = v.el.offsetHeight ? v.r.height / v.el.offsetHeight : 1;
      v.d = Math.max(0, floor - v.r.bottom) / (scale || 1);
      v.tilt = ((Math.random() - 0.5) * 8).toFixed(1);
      const b = Math.min(v.d * 0.22, 60), b2 = b / 3, d = v.d;
      g.anims.push(v.el.animate([
        { translate: '0 0', rotate: '0deg', easing: 'cubic-bezier(.55, 0, 1, .45)' },
        { translate: `0 ${d}px`, rotate: `${v.tilt}deg`, offset: 0.55, easing: 'cubic-bezier(0, .55, .45, 1)' },
        { translate: `0 ${d - b}px`, rotate: `${v.tilt}deg`, offset: 0.72, easing: 'cubic-bezier(.55, 0, 1, .45)' },
        { translate: `0 ${d}px`, rotate: `${v.tilt}deg`, offset: 0.86, easing: 'cubic-bezier(0, .55, .45, 1)' },
        { translate: `0 ${d - b2}px`, rotate: `${v.tilt}deg`, offset: 0.93, easing: 'cubic-bezier(.55, 0, 1, .45)' },
        { translate: `0 ${d}px`, rotate: `${v.tilt}deg` },
      ], { duration: 1000, delay: i * 70, fill: 'forwards' }));
    });
    const landed = 1000 + items.length * 70;
    g.timer = later(() => {
      if (S.grav !== g) return;
      sound.whistle(260, 1500, 1.1);
      items.forEach((v, i) => {
        if (!v.el.isConnected) return;
        g.anims.push(v.el.animate([
          { translate: `0 ${v.d}px`, rotate: `${v.tilt}deg` },
          { translate: '0 0', rotate: '0deg' },
        ], { duration: 1100, delay: i * 50, easing: 'cubic-bezier(.3, 0, .2, 1)', fill: 'forwards' }));
      });
      g.timer = later(() => endGravity(g), 1150 + items.length * 50);
    }, landed + 3000);
    return true;
  };
  // Every animation cancelled: the containers are back where Power BI put them, with no style of ours.
  const endGravity = g => {
    if (!g) return;
    cancel(g.timer);
    g.anims.forEach(a => { try { a.cancel(); } catch {} });
    if (S.grav === g) S.grav = null;
  };

  // ---- what a visual container is ----
  const typeOf = v => (((v.querySelector('[class*="visual-"]') || { getAttribute: () => '' }).getAttribute('class') || '').match(/\bvisual-(\w+)/) || [])[1] || '';
  const isCard = v => /^card\b/i.test(v.getAttribute('aria-roledescription') || '') || /^card/i.test(typeOf(v));
  const isSlicer = v => /slicer/i.test(`${v.getAttribute('aria-roledescription') || ''} ${typeOf(v)}`);
  const containerOf = n => (n && n.closest && n.closest('.displayArea .visualContainer')) || null;
  // Screen pixels per container pixel: the page is scaled to the zoom.
  const scaleOf = v => { const r = v.getBoundingClientRect(); return v.offsetHeight ? r.height / v.offsetHeight || 1 : 1; };

  // ---- card slot machine: the big number spins a reel and lands on a non-answer ----
  const SLOT_ENDS = ['42', 'NaN', 'It depends', '\u00af\\_(\u30c4)_/\u00af', '#REF!', 'BLANK()'];
  const textWidth = (text, font) => {
    const c = S.measure || (S.measure = document.createElement('canvas').getContext('2d'));
    c.font = font;
    return c.measureText(text).width;
  };
  // The card's value: the text with the biggest font in the container (a new card can hold
  // several; then the one under the pointer). Its title, hints and our own nodes are skipped.
  const cardValue = (v, pt) => {
    const vr = v.getBoundingClientRect(), found = [];
    for (const n of v.querySelectorAll('text, tspan, div, span, p')) {
      if (n.closest('[data-pbt], .visualsEnterHint, .visualHeader, [class*="visualTitle" i]')) continue;
      if (n.namespaceURI === SVG_NS ? n.tagName === 'text' && n.querySelector('tspan') : n.children.length) continue;
      const txt = (n.textContent || '').trim();
      if (!txt) continue;
      const r = n.getBoundingClientRect();
      if (r.width < 2 || r.height < 2 || r.right < vr.left || r.left > vr.right || r.bottom < vr.top || r.top > vr.bottom) continue;
      const cs = getComputedStyle(n);
      if (cs.visibility === 'hidden' || cs.display === 'none') continue;
      )PBT"
LR"PBT(found.push({ n, r, cs, txt, size: parseFloat(cs.fontSize) || 0 });
    }
    if (!found.length) return null;
    const big = Math.max(...found.map(c => c.size));
    const values = found.filter(c => c.size >= big * 0.8).sort((a, b) => b.size - a.size);
    if (!pt) return values[0];
    return values.find(c => pt[0] >= c.r.left - 12 && pt[0] <= c.r.right + 12 && pt[1] >= c.r.top - 12 && pt[1] <= c.r.bottom + 12) || null;
  };
  // Random values shaped like the card's own ("$73,458" gives "$12,907", "$5,311", ...).
  const lookalike = sample => {
    const m = /^(\D*?)([\d.,]+)(\D*)$/.exec(sample);
    const pre = m ? m[1] : '', suf = m ? m[3] : '', digits = Math.min(7, Math.max(1, m ? m[2].replace(/\D/g, '').length : 4));
    return () => `${pre}${Math.floor(Math.random() * 10 ** digits).toLocaleString('en-US')}${suf}`;
  };
  // Where a cubic-bezier easing passes each item of the reel, in seconds, for the ticks.
  const reelTicks = (items, secs, x1, y1, x2, y2) => {
    const bez = (s, a, b) => 3 * a * s * (1 - s) ** 2 + 3 * b * s * s * (1 - s) + s ** 3;
    const out = [];
    let last = 0;
    for (let k = 1; k <= 400; k += 1) {
      const s = k / 400, idx = Math.floor(bez(s, y1, y2) * (items - 1) + 0.5);
      if (idx > last) { last = idx; out.push(bez(s, x1, x2) * secs); }
    }
    return out;
  };
  const slot = (v, pt) => {
    if (!v || S.slots.has(v)) return false;
    const val = cardValue(v, pt);
    if (!val) return false;
    ensureStyle();
    S.slots.add(v);
    const { n, r, cs, txt } = val, vr = v.getBoundingClientRect(), svg = n.namespaceURI === SVG_NS;
    // The value's drawn font size: its CSS size through the zoom (and any SVG scaling), checked
    // against the height it really takes.
    let size = (parseFloat(cs.fontSize) || 16) * scaleOf(v);
    if (svg && n.getScreenCTM) { try { const m = n.getScreenCTM(); if (m) size = (parseFloat(cs.fontSize) || 16) * Math.hypot(m.a, m.b); } catch {} }
    if (!(size > r.height * 0.4 && size < r.height * 1.2)) size = r.height * 0.72;
    const end = SLOT_ENDS[Math.floor(Math.random() * SLOT_ENDS.length)];
    const fontAt = px => `${cs.fontStyle} ${cs.fontWeight} ${px.toFixed(1)}px ${cs.fontFamily}`;
    const room = Math.max(r.width, vr.width - 12), wEnd = textWidth(end, fontAt(size));
    if (wEnd > room) size *= room / wEnd;
    const w = Math.min(room, Math.max(r.width, textWidth(end, fontAt(size)) + 8)), h = Math.max(r.height, size * 1.25);
    const make = lookalike(txt), items = [txt];
    for (let i = 0; i < 16; i += 1) items.push(i % 6 === 4 ? SLOT_ENDS[Math.floor(Math.random() * SLOT_ENDS.length)] : make());
    items.push(end);
    const reel = htmlEl('div', {}, items.map(t => htmlEl('div', { text: t, style: `height:${h}px` })));
    const box = track(htmlEl('div', { class: 'pbt-chaos-slot', 'aria-hidden': 'true' }, reel));
    box.style.cssText = `left:${r.left + r.width / 2 - w / 2}px;top:${r.top + r.height / 2 - h / 2}px;width:${w}px;height:${h}px;font:${fontAt(size)};line-height:${h}px;color:${svg ? cs.fill : cs.color}`;
    document.body.append(box);
    // The real number fades out under the reel (an SVG tspan through its text element).
    const real = (svg && n.tagName === 'tspan' && n.closest('text')) || n;
    const hide = anim(real, [{ opacity: 1 }, { opacity: 0 }], { duration: 90, fill: 'forwards' });
    const spin = 1.7;
    const roll = anim(reel, [{ transform: 'translateY(0)' }, { transform: `translateY(${-(items.length - 1) * h}px)` }], { duration: spin * 1000, easing: 'cubic-bezier(.12, .6, .22, 1)', fill: 'forwards' });
    reelTicks(items.length, spin, 0.12, 0.6, 0.22, 1).forEach(t => sound.tick(t));
    later(() => sound.kaching(), spin * 1000);
    later(() => {
      const fade = anim(box, [{ opacity: 1 }, { opacity: 0 }], { duration: 350, fill: 'forwards' });
      unanim(hide);
      anim(real, [{ opacity: 0 }, { opacity: 1 }], { duration: 350 });
      later(() => { unanim(roll); unanim(fade); drop(box); S.slots.delete(v); }, 380);
    }, spin * 1000 + 2500);
    return true;
  };

  // ---- balloon bars: a triple-clicked column inflates, pops, and grows back ----
  const BAR_SEL = 'rect.column, rect.bar';
  const isBar = n => !!(n && n.matches && n.matches(BAR_SEL) && containerOf(n) && !n.closest('[data-pbt]'));
  const barAt = e => {
    if (isBar(e.target)) return e.target;
    try { for (const n of document.elementsFromPoint(e.clientX, e.clientY)) if (isBar(n)) return n; } catch {}
    return null;
  };
  // Clicks pass straight through to Power BI (its own selection still happens); only the count is kept.
  const barClick = e => {
    const bar = barAt(e);
    if (!bar) { S.tri = null; return; }
    const v = containerOf(bar), i = [...v.querySelectorAll(BAR_SEL)].indexOf(bar), now = Date.now(), t = S.tri;
    if (t && t.v === v && t.i === i && now - t.at < 600) t.n += 1; else S.tri = { v, i, n: 1 };
    S.tri.at = now;
    if (S.tri.n >= 3) { S.tri = null; balloon(bar); }
  };
  const bits = (r, color) => {
    const cx = r.left + r.width / 2, cy = r.top + r.height / 2, c = /^(rgb|#)/.test(color || '') ? color : '#888';
    for (let i = 0; i < 12; i += 1) {
      const el = track(htmlEl('div', { class: 'pbt-chaos-bit', style: `left:${cx - 3.5}px;top:${cy - 3.5}px;background:${c}` }));
      document.body.append(el);
      const ang = (i / 12) * Math.PI * 2 + Math.random() * 0.5, d = 30 + Math.random() * 45;
      el.animate([{ transform: 'translate(0, 0) scale(1)', opacity: 1 }, { transform: `translate(${(Math.cos(ang) * d).toFixed(1)}px, ${(Math.sin(ang) * d + 14).toFixed(1)}px) scale(.4)`, opacity: 0 }],
        { duration: 550 + Math.random() * 250, easing: 'cubic-bezier(.2, .7, .3, 1)', fill: 'forwards' });
      later(() => drop(el), 850);
    }
  };
  // Only CSS transform and opacity, by Web Animations, all cancelled at the end: Power BI's own
  // attributes on the rect are never written. A CSS transform replaces the SVG transform
  // attribute while it runs, so the attrib)PBT"
LR"PBT(ute's matrix (read, never changed) goes first.
  const balloon = bar => {
    if (!bar || S.popping.has(bar) || typeof bar.animate !== 'function') return false;
    let bb;
    try { bb = bar.getBBox(); } catch { return false; }
    if (!bb.width || !bb.height) return false;
    ensureStyle();
    S.popping.add(bar);
    let pre = '';
    try {
      const list = bar.transform && bar.transform.baseVal;
      if (list && list.numberOfItems) {
        let m = new DOMMatrix();
        for (let i = 0; i < list.numberOfItems; i += 1) { const t = list.getItem(i).matrix; m = m.multiply(DOMMatrix.fromMatrix({ a: t.a, b: t.b, c: t.c, d: t.d, e: t.e, f: t.f })); }
        pre = `matrix(${m.a}, ${m.b}, ${m.c}, ${m.d}, ${m.e}, ${m.f}) `;
      }
    } catch {}
    const across = /\bbar\b/.test(bar.getAttribute('class') || '');
    const cx = bb.x + bb.width / 2, cy = bb.y + bb.height / 2;
    const at = (sx, sy, ox = cx, oy = cy) => `${pre}translate(${ox}px, ${oy}px) scale(${sx}, ${sy}) translate(${-ox}px, ${-oy}px)`;
    const color = getComputedStyle(bar).fill;
    sound.whistle(330, 900, 0.7);
    const a1 = anim(bar, [
      { transform: at(1, 1) }, { transform: at(1.12, 1.08), offset: 0.3 }, { transform: at(1.05, 1.03), offset: 0.45 }, { transform: at(1.3, 1.2) },
    ], { duration: 750, easing: 'ease-in', fill: 'forwards' });
    later(() => {
      if (!S.popping.has(bar)) return;
      const r = bar.getBoundingClientRect();
      const a2 = anim(bar, [{ transform: at(1.3, 1.2), opacity: 1 }, { transform: at(1.7, 1.5), opacity: 0 }], { duration: 110, fill: 'forwards' });
      sound.pop();
      bits(r, color);
      // Two seconds later it grows back from its base: up for a column, out from the axis for a bar.
      later(() => {
        if (!S.popping.has(bar)) return;
        const bx = across ? bb.x : cx, by = across ? cy : bb.y + bb.height;
        const a3 = anim(bar, [
          { transform: across ? at(0, 1, bx, by) : at(1, 0, bx, by), opacity: 1 },
          { transform: across ? at(1.1, 1, bx, by) : at(1, 1.1, bx, by), opacity: 1, offset: 0.7 },
          { transform: at(1, 1), opacity: 1 },
        ], { duration: 650, easing: 'ease-out', fill: 'forwards' });
        sound.whistle(300, 620, 0.5);
        later(() => { [a1, a2, a3].forEach(unanim); S.popping.delete(bar); }, 680);
      }, 2000);
    }, 750);
    return true;
  };

  // ---- runaway slicer: eight clicks in 3 s and it dodges the pointer for 5 s ----
  const slicerHit = v => {
    const now = Date.now(), list = (S.hits.get(v) || []).filter(t => now - t < 3000);
    list.push(now);
    S.hits.set(v, list);
    if (list.length >= 8) { S.hits.delete(v); runaway(v); }
  };
  // The container's current translate (Web Animations included), in screen pixels.
  const offsetOf = run => {
    const m = String(getComputedStyle(run.v).translate || '').match(/-?[\d.]+(?:e-?\d+)?/g);
    return m ? [(+m[0] || 0) * run.scale, (+m[1] || 0) * run.scale] : [0, 0];
  };
  const slide = (run, x, y, ms) => {
    const [fx, fy] = offsetOf(run), s = run.scale;
    run.off = [x, y];
    const a = anim(run.v, [{ translate: `${fx / s}px ${fy / s}px` }, { translate: `${x / s}px ${y / s}px` }], { duration: ms, easing: 'cubic-bezier(.2, .8, .3, 1)', fill: 'forwards' });
    run.anims.splice(0).forEach(unanim);
    if (a) run.anims.push(a);
  };
  // A pointer within 80 px of the slicer sends it the way that leaves the most room, inside the
  // visible part of the report page.
  const dodge = (run, x, y) => {
    if (S.run !== run || run.ending || !run.v.isConnected) return;
    const h = run.home, [ox, oy] = run.off;
    const away = (dx, dy) => Math.hypot(Math.max(h.left + dx - x, 0, x - h.right - dx), Math.max(h.top + dy - y, 0, y - h.bottom - dy));
    if (away(ox, oy) >= 80) return;
    const da = document.querySelector('.displayArea'), p = da ? da.getBoundingClientRect() : { left: 0, top: 0, right: innerWidth, bottom: innerHeight };
    const b = { left: Math.max(0, p.left), top: Math.max(0, p.top), right: Math.min(innerWidth, p.right), bottom: Math.min(innerHeight, p.bottom) };
    const clamp = (val, lo, hi) => Math.min(Math.max(val, lo), Math.max(lo, hi));
    let best = null;
    for (let k = 0; k < 16; k += 1) {
      const ang = (k * Math.PI) / 8;
      const nx = clamp(ox + Math.cos(ang) * 150, b.left - h.left, b.right - h.right);
      const ny = clamp(oy + Math.sin(ang) * 150, b.top - h.top, b.bottom - h.bottom);
      const score = away(nx, ny) - Math.hypot(nx - ox, ny - oy) * 0.05;
      if (!best || score > best.score) best = { nx, ny, score };
    }
    slide(run, best.nx, best.ny, 260);
  };
  const runaway = v => {
    if (!v || S.run || S.grav || typeof v.animate !== 'function') return false;
    const run = S.run = { v, scale: scaleOf(v), home: v.getBoundingClientRect(), off: [0, 0], anims: [], pt: null, queued: false, ending: false };
    say('Please stop.', { force: true, buttons: [] });
    sound.whistle(500, 1400, 0.35);
    const onMove = e => {
      run.pt = [e.clientX, e.clientY];
      if (run.queued) return;
      run.queued = true;
      requestAnimationFrame(safe(() => { run.queued = false; if (run.pt) dodge(run, run.pt[0], run.pt[1]); }));
    };
    document.addEventListener('pointermove', onMove, { capture: true, passive: true });
    run.unlisten = () => document.removeEventListener('pointermove', onMove, { capture: true, passive: true });
    if (S.mouse) dodge(run, S.mouse[0], S.mouse[1]);
    later(() => endRun(run, true), 5000);
    return true;
  };
  // Glides home, then every animation is cancelled: the container is where Power BI put it.
  const endRun = (run, glide) => {
    if (!run || S.run !== run) return;
    run.ending = true;
    if (run.unlisten) { run.unlisten(); run.unlisten = null; }
    const done = () => { run.anims.splice(0).forEach(unanim); if (S.run === run) S.run = null; };
    if (glide && run.v.isConnected && (run.off[0] || run.off[1])) { slide(run, 0, 0, 700); later(done, 750); } el)PBT"
LR"PBT(se done();
  };

  // ---- the refresh finale ----
  const finale = () => {
    ensureStyle();
    const secs = sound.modem();
    later(() => {
      try { if (pbt.fun && typeof pbt.fun.confetti === 'function') pbt.fun.confetti(); } catch {}
      achievement(nextLine('achievement'));
    }, Math.max(300, secs * 1000));
    return true;
  };
  const achievement = line => {
    const cup = svgEl('svg', { viewBox: '0 0 48 48', 'aria-hidden': 'true' },
      svgEl('circle', { cx: 24, cy: 24, r: 23, fill: '#3a3a3a', stroke: '#c9a227', 'stroke-width': 2 }),
      svgEl('path', { d: 'M15 12 H33 V20 A9 9 0 0 1 15 20 Z', fill: '#f2c94c' }),
      svgEl('path', { d: 'M15 14 H11 A5 5 0 0 0 16 22 M33 14 H37 A5 5 0 0 1 32 22', fill: 'none', stroke: '#f2c94c', 'stroke-width': 2 }),
      svgEl('rect', { x: 22, y: 28, width: 4, height: 6, fill: '#f2c94c' }),
      svgEl('rect', { x: 17, y: 34, width: 14, height: 4, rx: 1, fill: '#f2c94c' }));
    document.querySelectorAll('.pbt-chaos-ach').forEach(n => drop(n));
    const el = track(htmlEl('div', { class: 'pbt-chaos-ach', role: 'status' }, cup, htmlEl('div', {}, htmlEl('small', { text: 'ACHIEVEMENT UNLOCKED' }), htmlEl('b', { text: line }))));
    document.body.append(el);
    sound.chime();
    later(() => drop(el), 5100);
  };

  // ---- the banner ----
  const banner = () => {
    const el = track(htmlEl('div', { class: 'pbt-chaos-banner', 'aria-hidden': 'true' }, htmlEl('div', {}, htmlEl('b', { text: 'CHAOS MODE' }), htmlEl('small', { text: 'you asked for this' }))));
    document.body.append(el);
    later(() => drop(el), 2000);
  };

  // ---- how to leave: a small note in the mod's toast style, on screen for as long as Chaos Mode is
  // on (it goes with the rest of S.els in off()). It takes no clicks. ----
  const exitNote = () => {
    if (S.exitEl && S.exitEl.isConnected) return;
    const el = S.exitEl = track(htmlEl('div', { class: 'pbt-toast pbt-chaos-exit', role: 'status' },
      htmlEl('span', { text: 'Chaos Mode is on. To leave it, press Esc twice, or type the code again.' })));
    document.body.append(el);
    requestAnimationFrame(() => el.classList.add('pbt-in'));
  };

  const ensureStyle = () => {
    let s = document.getElementById('pbt-chaos-css');
    if (!s) { s = document.createElement('style'); s.id = 'pbt-chaos-css'; s.setAttribute('data-pbt', ''); (document.head || document.documentElement).append(s); }
    if (s.textContent !== CHAOS_CSS) s.textContent = CHAOS_CSS;
  };
  const retro = how => { try { if (pbt.retro95 && typeof pbt.retro95[how] === 'function') pbt.retro95[how]('chaos'); } catch {} };
  // Ends every registered real-visual effect (page/82 to 84) that is running.
  const stopFx = () => {
    const fx = pbt.chaosFx || {};
    for (const id of Object.keys(fx)) { const f = fx[id]; if (f && typeof f.stop === 'function') { try { f.stop(); } catch {} } }
  };

  // ---- one tick a second while Chaos Mode is on: page switches, new visuals, idle ----
  const tick = () => {
    // Page switches and new visuals are tracked even while the window is covered (a demo window
    // behind the presenter's notes still switches pages); only the idle line waits to be seen.
    if (!S.on) return;
    place();
    const now = Date.now(), key = pageKey(), n = visualCount();
    if (key !== S.pageKey) {
      const first = !S.pageKey;
      S.pageKey = key; S.count = n; S.pageAt = now;
      if (!first) react('page');
      maybeCrash();
      return;
    }
    if (n !== S.count) {
      const up = n > S.count;
      S.count = n;
      // Right after a page switch the visuals arrive one by one; that is not a new visual.
      if (up && now - S.pageAt > 3000) react('visual');
      maybeCrash();
    }
    if (!document.hidden && !S.idleSaid && now - S.lastActive > 30000) { S.idleSaid = true; react('idle'); }
  };
  const active = () => { S.lastActive = Date.now(); S.idleSaid = false; };

  const BACKUP = { 1: () => say(nextLine('joke'), { force: true }), 2: () => crash(), 3: () => gravity(), 4: () => finale() };

  // ---- keys: the Konami code, Esc twice, G, the backup keys, and any key on the crash screen ----
  const onKey = e => {
    if (S.crashEl) {
      // Any key dismisses the crash screen. A key with Ctrl, Alt or Win goes on to Power BI as usual
      // (Ctrl+S saves, a shortcut runs). A plain key is only the "any key" the screen asks for: it is
      // swallowed, its keyup too (eatUp), so Delete does not remove the selected visual and an
      // arrow does not nudge it. Either way it counts for nothing here (neither half of Esc twice
      // nor G).
      if (!(e.ctrlKey || e.altKey || e.metaKey)) {
        e.preventDefault(); e.stopImmediatePropagation();
        S.eatUp = e.code || e.key || null;
      }
      if (!e.repeat) dismissCrash();
      return;
    }
    if (S.on) active();
    const target = (e.composedPath && e.composedPath()[0]) || e.target;
    if (S.on && e.ctrlKey && e.altKey && !e.metaKey && !e.shiftKey) {
      const digit = (/^(?:Digit|Numpad)([1-4])$/.exec(e.code || '') || [])[1];
      // The core's rule for Ctrl+Alt keys: never in a text field, never when AltGraph is flagged,
      // since Windows reports AltGr as Ctrl+Alt and some layouts type #, $, ~ or ^ with it.
      const altGraph = !!(e.getModifierState && e.getModifierState('AltGraph'));
      if (digit && !typingIn(target) && !altGraph) {
        e.preventDefault(); e.stopImmediatePropagation();
        if (!e.repeat) BACKUP[digit]();
        return;
      }
    }
    if (e.ctrlKey || e.altKey || e.metaKey || typingIn(target)) { S.konami = 0; return; }
    const k = (e.key || '').toLowerCase();
    if (k === KONAMI[S.konami]) S.konami += 1;
    else S.konami = k === 'arrowup' ? (S.konami === 2 ? 2 : 1) : 0;
    if (S.konami === KONAMI.length) {
      S.konami = 0;
      // The extras' own Konami dance steps aside for Chaos Mode (an older page/50-fun.js still
      // starts it on the same key).
      document.documentElement.classL)PBT"
LR"PBT(ist.remove('pbt-dance');
      if (S.on) chaos.off(); else chaos.on();
      return;
    }
    if (!S.on) return;
    if (k === 'escape') {
      // An Esc that closes the Power BI 95 Begin menu is that menu's, not half of Esc twice.
      if (pbt.retro95 && pbt.retro95.menuEl) return;
      if (Date.now() - S.escAt < 1000) { S.escAt = 0; chaos.off(); } else S.escAt = Date.now();
      return;
    }
    if (k === 'g' && !e.shiftKey && !e.repeat) { e.preventDefault(); e.stopImmediatePropagation(); gravity(); }
  };

  const chaos = {
    name: 'chaos',
    start() {
      if (S.started) return 'already started';
      // The "Hidden surprises" setting off: no key watcher, nothing at all (settings apply when
      // Power BI starts, so nothing needs undoing here).
      if (!pbt.feature('surprises')) return 'off (Hidden surprises setting)';
      S.started = true;
      listen(S.base, window, 'keydown', onKey, true);
      // The keyup of a plain key that dismissed the crash screen goes with its keydown.
      listen(S.base, window, 'keyup', e => {
        if (!S.eatUp || (e.code || e.key) !== S.eatUp) return;
        S.eatUp = null;
        e.preventDefault(); e.stopImmediatePropagation();
      }, true);
      // A finished refresh: page/50-fun.js dispatches this event. An older copy without it is
      // heard through its "refresh" comment instead, put back on stop().
      listen(S.base, window, 'pbt-refreshed', () => { if (S.on) finale(); });
      const fun = pbt.fun;
      if (fun && !fun.refreshEvent && typeof fun.say === 'function') {
        const orig = fun.say;
        const wrapped = function (kind, ...rest) {
          if (kind === 'refresh') { try { window.dispatchEvent(new CustomEvent('pbt-refreshed')); } catch {} }
          return orig.call(this, kind, ...rest);
        };
        fun.say = wrapped;
        S.base.push(() => { if (fun.say === wrapped) fun.say = orig; });
      }
      return 'started';
    },
    on() {
      if (S.on) return false;
      if (!S.started) this.start();
      if (!S.started || !document.body) return false;
      ensureStyle();
      clearTimeout(S.ctxT);
      S.on = true;
      S.closed = false; S.choosing = 0; S.crashed.clear(); S.pageKey = ''; S.lastSaid = 0; S.escAt = 0;
      active();
      banner();
      sound.fanfare();
      // After the banner: the 95 look, Barry, and the first look at the page.
      later(() => {
        if (!S.on) return;
        retro('on');
        exitNote();
        intro();
        S.pageKey = pageKey(); S.count = visualCount(); S.pageAt = Date.now();
        const iv = setInterval(safe(tick), 1000);
        S.live.push(() => clearInterval(iv));
        // A pie or donut on this page crashes once Barry's intro has been read, never before.
        later(() => { if (S.on && S.pageKey === pageKey()) maybeCrash(); }, CHAOS_INTRO.length * 24 + 2400);
      }, 2000);
      listen(S.live, document, 'pointermove', e => {
        active();
        S.mouse = [e.clientX, e.clientY];
        if (!S.eyeQueued && S.pupils.length) { S.eyeQueued = true; requestAnimationFrame(safe(look)); }
      }, { capture: true, passive: true });
      listen(S.live, document, 'wheel', active, { capture: true, passive: true });
      listen(S.live, document, 'pointerdown', e => {
        active();
        const v = containerOf(e.target);
        if (!v || !isSlicer(v)) return;
        react('slicer');
        slicerHit(v);
      }, true);
      // The click eggs on the report page. Nothing here stops a click: Power BI still selects and
      // cross-filters as usual.
      listen(S.live, document, 'click', e => {
        const v = containerOf(e.target);
        if (!v || !S.on) return;
        if (isCard(v)) slot(v, [e.clientX, e.clientY]);
        else barClick(e);
      }, true);
      listen(S.live, document, 'click', e => {
        const b = e.target && e.target.closest && e.target.closest('button');
        if (!b) return;
        const label = (b.getAttribute('aria-label') || b.textContent || '').trim();
        if (/^Refresh$/i.test(label) && b.closest('.ribbonContainer, .ms-Callout, .ms-Layer')) { react('refresh'); return; }
        if (b.closest('view-navigator')) {
          const view = /dax query/i.test(label) ? 'dax' : /^model/i.test(label) ? 'model' : null;
          if (!view) return;
          // The other views are pages of their own, so this page hides; Barry speaks on the way back.
          S.pendingView = view;
          later(() => { if (S.pendingView && !document.hidden) { react(S.pendingView); S.pendingView = null; } }, 1500);
        }
      }, true);
      listen(S.live, document, 'visibilitychange', () => {
        if (document.hidden || !S.pendingView) return;
        const view = S.pendingView;
        S.pendingView = null;
        later(() => react(view), 700);
      });
      listen(S.live, window, 'resize', place);
      return true;
    },
    off() {
      const was = S.on;
      S.on = false;
      S.timers.forEach(t => clearTimeout(t)); S.timers.clear();
      S.live.splice(0).forEach(fn => { try { fn(); } catch {} });
      endGravity(S.grav);
      endRun(S.run, false);
      hideBarry();
      // Every Web Animation on a card value, a bar or a slicer goes, so Power BI's elements are
      // back exactly as they were (no inline style was ever written to them).
      [...S.anims].forEach(unanim);
      S.slots.clear(); S.popping.clear(); S.hits.clear(); S.tri = null; S.pokes = [];
      S.crashEl = null; S.exitEl = null;
      [...S.els].forEach(drop);
      S.konami = 0; S.pendingView = null; S.choosing = 0;
      // Programs launched from the Begin menu end with Chaos Mode, so no visual stays moved,
      // hidden or catching clicks after the panic switch.
      if (was) stopFx();
      if (was) { retro('off'); sound.off(); }
      // The audio device is let go a few seconds later, once the power-down sound has played.
      clearTimeout(S.ctxT);
      if (S.ctx) S.ctxT = setTimeout(() => { )PBT"
LR"PBT(if (S.on || !S.ctx) return; try { S.ctx.close().catch(() => {}); } catch {} S.ctx = null; S.master = null; }, 3000);
      // Our style element too, so the page has exactly the [data-pbt] nodes it had before; on()
      // and the test hooks put it back when they need it.
      const s = document.getElementById('pbt-chaos-css');
      if (s) s.remove();
      return was;
    },
    toggle() { return S.on ? this.off() : this.on(); },
    // The test hook. say() needs Chaos Mode on; the rest run either way.
    say(text) { return say(String(text || nextLine('joke')), { force: true }); },
    crash() { if (!S.started) this.start(); return crash(); },
    dismiss() { return dismissCrash(); },
    gravity() { if (!S.started) this.start(); return gravity(); },
    finale() { if (!S.started) this.start(); return finale(); },
    // The click eggs, without the clicks: the first card, bar or slicer on the page.
    slot() { const v = containers().find(isCard); return v ? slot(v, null) : 'no card on this page'; },
    fedUp() { return S.on ? fedUp() : 'Chaos Mode is off'; },
    beginMenu() {
      const r = pbt.retro95;
      if (!r || typeof r.menu !== 'function') return 'no retro95';
      if (!r.active) return 'retro95 is off';
      return r.menuEl ? true : r.menu();
    },
    balloon() {
      const bar = [...document.querySelectorAll(`.displayArea .visualContainer :is(${BAR_SEL})`)].find(b => { const r = b.getBoundingClientRect(); return r.width > 0 && r.height > 0; });
      return bar ? balloon(bar) : 'no column or bar on this page';
    },
    // Starts the run and takes one dodge from a pointer at the slicer's middle, so a screenshot
    // shows it on the move.
    runaway() {
      const v = containers().find(isSlicer);
      if (!v) return 'no slicer on this page';
      if (!runaway(v)) return false;
      const r = v.getBoundingClientRect();
      dodge(S.run, r.left + r.width / 2, r.top + r.height / 2);
      return true;
    },
    // For the lead's before-and-after check: the visual containers' inline styles and our nodes,
    // read the same way the live run reads them. Compare on the same page: another page has
    // another number of containers (each adds a "|") and other tools' badges.
    audit() {
      const vc = [...document.querySelectorAll('.visualContainer')];
      const styleText = vc.map(e => e.getAttribute('style')).join('|');
      const nodes = [...document.querySelectorAll('[data-pbt]')].map(e => `${e.tagName.toLowerCase()}${e.id ? `#${e.id}` : ''}${(e.getAttribute('class') || '').trim().split(/\s+/).filter(Boolean).map(c => `.${c}`).join('')}`);
      const anims = document.getAnimations().filter(a => a.effect && a.effect.target && a.effect.target.closest && a.effect.target.closest('.displayArea')).length;
      return { styles: styleText.length, pbtNodes: nodes.length, page: pageKey(), containers: vc.length, styleText, nodes, anims };
    },
    // Whether Chaos Mode is on: one flag, cheap enough for a click handler (state() is not).
    isOn() { return S.on; },
    state() {
      return {
        started: S.started, on: S.on,
        barry: !!(S.barry && S.barry.isConnected), closed: S.closed,
        bubble: S.bubble && !S.bubble.hidden ? (S.text && S.text.textContent) || '' : null,
        crash: !!S.crashEl, crashPct: S.crashEl ? S.crashPct : null,
        exitNote: !!(S.exitEl && S.exitEl.isConnected),
        gravity: !!S.grav, banner: !!document.querySelector('.pbt-chaos-banner'),
        achievement: (document.querySelector('.pbt-chaos-ach b') || {}).textContent || null,
        page: S.pageKey, visuals: S.count, pie: (() => { try { return hasPie(); } catch { return null; } })(),
        crashedPages: [...S.crashed], retro95: !!pbt.retro95,
        fedUp: S.fed ? S.fed.stage : null, slots: S.slots.size, balloons: S.popping.size, runaway: !!S.run, anims: S.anims.size,
        beginMenu: !!(pbt.retro95 && pbt.retro95.menuEl),
        audio: S.ctx ? S.ctx.state : null,
      };
    },
    stop() {
      this.off();
      S.base.splice(0).forEach(fn => { try { fn(); } catch {} });
      S.started = false;
      const s = document.getElementById('pbt-chaos-css');
      if (s) s.remove();
      clearTimeout(S.ctxT); S.ctxT = null;
      if (S.ctx) { try { S.ctx.close().catch(() => {}); } catch {} S.ctx = null; S.master = null; }
    },
    // Takes this copy out completely, so a fresh evaluation of the file can replace it.
    remove() {
      this.stop();
      const i = pbt.modules ? pbt.modules.indexOf(this) : -1;
      if (i >= 0) pbt.modules.splice(i, 1);
      if (pbt.chaos === this) delete pbt.chaos;
    },
  };
  pbt.chaos = chaos;
  if (typeof pbt.module === 'function') pbt.module(chaos);
}

// ---- 81-retro95.js ----
// Power BI 95: a mid-90s desktop look for Power BI's window, switched on and off by Chaos Mode
// (page/80-chaos.js). Silver chrome with raised and sunken bevels, navy title strips on the panes,
// a teal desktop around the report page, chunky grey scrollbars and a taskbar along the status bar
// with a "Begin" button and a clock. All of it is original and generic: no logos from any real
// operating system.
//
// Contract:
//   pbt.retro95.on(owner)  turns the look on for an owner, 'chaos' by default (Chaos Mode) or
//                          'classic' (the Classic skin with its 95 style); a second owner only joins.
//   pbt.retro95.off(owner) lets one owner go; the look stays while another still wants it. With no
//                          owner it takes every trace away at once: its style element, the html
//                          class, the taskbar pieces, its timer and its observer.
//   pbt.retro95.setClassicStyle('clean' | '95')  Classic's style, remembered; the Skins group
//                          shows the switch while Classic is picked.
//   pbt.retro95.active     true while the look is on.
//   pbt.retro95.toggle()   on or off, whichever it is not.
//   pbt.retro95.preview()  a one-line description of what the )PBT"
LR"PBT(look matched, for tests.
//   pbt.retro95.start()    what the core calls at boot; it turns nothing on by itself.
//   pbt.retro95.menu()     opens (or closes) the Begin menu, as a click on Begin does; the test
//                          hook window.__pbt.chaos.beginMenu() calls it. menuEl is the open menu.
//   pbt.retro95.shutdown() the fake "safe to turn off" screen, for 3 s.
// This file also works on its own: evaluate its body with pbt = window.__pbt in scope (the
// installed mod already running), then call window.__pbt.retro95.on(). Evaluating it again
// replaces the earlier copy cleanly.
//
// The look sits on top of whatever skin is active. Every rule is scoped under html.pbt-retro95 with
// two :not(#id) clauses, which gives it ID-level specificity, so it outranks the skins' attribute
// selectors without editing them. The report page itself (.displayArea and its visuals) keeps its
// own look, with nothing added.

if (pbt.retro95) {
  try { pbt.retro95.off(); } catch {}
  const old = pbt.modules.indexOf(pbt.retro95);
  if (old >= 0) pbt.modules.splice(old, 1);
}

const R95_SEL = 'html.pbt-retro95:not(#pbt-r95-a):not(#pbt-r95-b)';
const R95_FONT = 'Tahoma, Verdana, "MS Sans Serif", "Microsoft Sans Serif", sans-serif';

// Power BI's chrome takes its colors from theme variables. The neutral ones (backgrounds, text,
// borders) are remapped onto a silver scale while the look is on, the same way a skin remaps them,
// so text inside the panes stays dark on silver in light and dark mode alike. Positions run from 0
// (the theme's background end) to 1 (its text end); a dark theme's background1 (#292929, about 0.16)
// lands on white, which is where Fluent puts its input wells and cards.
const R95_RAMP = [[0, '#c0c0c0'], [0.078, '#c0c0c0'], [0.161, '#ffffff'], [0.239, '#c0c0c0'], [0.361, '#808080'], [0.561, '#404040'], [0.839, '#000000'], [1, '#000000']];

function r95ThemeCss() {
  const c = pbt.color;
  if (!c) return { css: '', count: 0 };
  const ramp = R95_RAMP.map(([t, h]) => [t, c.parse(h)]);
  const onRamp = t => {
    if (t <= ramp[0][0]) return ramp[0][1];
    for (let i = 1; i < ramp.length; i++) if (t <= ramp[i][0]) return c.mix(ramp[i - 1][1], ramp[i][1], (t - ramp[i - 1][0]) / (ramp[i][0] - ramp[i - 1][0]));
    return ramp[ramp.length - 1][1];
  };
  const map = (value, srcDark) => {
    const col = c.parse(value);
    if (!col || col.a === 0 || c.chroma(col) >= 0.12) return null;
    const t = srcDark ? c.grey(col) : 1 - c.grey(col);
    return c.hex({ ...onRamp(t), a: col.a });
  };
  // Every theme block in the page's own style sheets (ours are skipped). The dark block is the
  // reference wherever it names a concrete color, as the skins do, so a token lands on the same
  // silver in both modes; everything else is read in the current mode.
  const current = document.documentElement.getAttribute('data-theme') || '';
  const root = new Map(), dark = new Map(), mode = new Map(), fui = new Map();
  const take = (rule, m) => { for (let i = 0; i < rule.style.length; i++) { const p = rule.style[i]; if (p.startsWith('--')) m.set(p, rule.style.getPropertyValue(p).trim()); } };
  const walk = list => {
    for (const r of list) {
      if (r.selectorText && r.style) {
        const parts = r.selectorText.split(',').map(s => s.trim());
        for (const p of parts) {
          const th = (p.match(/^\[data-theme="([^"]+)"\]$/) || [])[1];
          if (th === 'dark') take(r, dark);
          if (th && th === current) take(r, mode);
        }
        if (parts.includes(':root')) take(r, root);
        else if (parts.every(p => /^\.fui-FluentProvider[\w-]*$/.test(p))) take(r, fui);
      }
      if (r.cssRules && r.cssRules.length) walk(r.cssRules);
    }
  };
  for (const sheet of document.styleSheets) {
    if (sheet.ownerNode && sheet.ownerNode.dataset && 'pbt' in sheet.ownerNode.dataset) continue;
    let list; try { list = sheet.cssRules; } catch { continue; }
    walk(list);
  }
  const cs = getComputedStyle(document.documentElement);
  const bg = c.parse(cs.getPropertyValue('--colorNeutralBackground1').trim()), fg = c.parse(cs.getPropertyValue('--colorNeutralForeground1').trim());
  const curDark = bg && fg ? c.grey(bg) < c.grey(fg) : current === 'dark';
  const decl = (m, useRef) => {
    const out = [];
    for (const [k, v] of m) {
      if (!v || v.startsWith('var(')) continue;
      const r = useRef && dark.get(k);
      const mapped = r && !r.startsWith('var(') ? map(r, true) : map(v, curDark);
      if (mapped) out.push(`${k}: ${mapped} !important;`);
    }
    return out;
  };
  const top = decl(new Map([...root, ...mode, ...dark]), true);
  const inner = decl(fui, true);
  // Pinned: disabled and faint text stay readable on silver (at least 4.5:1), text on the brand
  // color stays white.
  const pinned = '--colorNeutralForegroundDisabled: #4a4a4a !important; --colorNeutralForegroundDisabled2: #4a4a4a !important; --colorNeutralForeground4: #4a4a4a !important; --colorNeutralForegroundOnBrand: #ffffff !important; --colorBrandForegroundOnBrand: #ffffff !important;';
  return {
    css: `${R95_SEL} { ${top.join(' ')} ${pinned} }\n${R95_SEL} .fui-FluentProvider { ${inner.join(' ')} ${pinned} }`,
    count: top.length + inner.length,
  };
}

// Begin menu icons drawn as small SVGs (22 x 22), in the bright, outlined style of mid-90s game
// and program icons. All original drawings.
const R95_SVG = {
  folder: `<path d='M1 5.5h7l2 2h11v12H1z' fill='#e8c020' stroke='#000' stroke-width='.8'/><path d='M1 9.5h20v10H1z' fill='#fff08a' stroke='#000' stroke-width='.8'/>`,
  programs: `<path d='M1 5.5h7l2 2h11v12H1z' fill='#e8c020' stroke='#000' stroke-width='.8'/><path d='M1 9.5h20v10H1z' fill='#fff08a' stroke='#000' stroke-width='.8'/><rect x='8.5' y='11.5' width='10' height='7' fill='#fff' stroke='#000' stroke-width='.7'/><rect x='8.5' y='11.5' width='10' height='2' fill='#000080'/>`,
  documents: `<path d='M4 1.5h9l5 5v14H4z' fill='#fff' stroke='#000' stroke-width='.8'/><)PBT"
LR"PBT(path d='M13 1.5v5h5' fill='#d0d0d0' stroke='#000' stroke-width='.8'/><path d='M6.5 10h9M6.5 13h9M6.5 16h7' stroke='#000080'/>`,
  settings: `<rect x='2' y='3' width='18' height='16' fill='#c0c0c0' stroke='#000'/><path d='M6 6v10M11 6v10M16 6v10' stroke='#404040'/><rect x='4.5' y='8' width='3' height='2.5' fill='#000080'/><rect x='9.5' y='12' width='3' height='2.5' fill='#c00000'/><rect x='14.5' y='7' width='3' height='2.5' fill='#008000'/>`,
  find: `<path d='M3 1.5h9l3 3v14H3z' fill='#fff' stroke='#000' stroke-width='.8'/><circle cx='12' cy='12' r='4.5' fill='#bfefff' stroke='#000' stroke-width='1.4'/><path d='M15.3 15.3l5 5' stroke='#000' stroke-width='2.4' stroke-linecap='round'/>`,
  coaster: `<rect x='.5' y='.5' width='21' height='21' fill='#9ad8ff' stroke='#000'/><rect x='1' y='17' width='20' height='4' fill='#2e8b2e'/><path d='M3 17V8M6 17V6M9 17V10M15 17V9M18 17V5.5' stroke='#7a4a1a'/><path d='M1 13C3 4 7 3 9 9S12 16 14 10S18 3 21 6' fill='none' stroke='#c00000' stroke-width='1.6'/><rect x='3.2' y='2.6' width='3.8' height='2.6' rx='.5' fill='#ffd800' stroke='#000' stroke-width='.6'/>`,
  jukebox: `<path d='M3.5 21.5V9a7.5 7.5 0 0 1 15 0v12.5z' fill='#b03000' stroke='#000'/><path d='M6 21V9.5a5 5 0 0 1 10 0V21' fill='none' stroke='#ffd800' stroke-width='1.4'/><rect x='8.5' y='10.5' width='5' height='3.5' fill='#80ffff' stroke='#000' stroke-width='.5'/><path d='M8.5 16.5h5M8.5 18.5h5M8.5 20.5h5' stroke='#400000'/>`,
  wheel: `<path d='M11 12L20 12A9 9 0 0 1 15.5 19.79Z' fill='#e00000'/><path d='M11 12L15.5 19.79A9 9 0 0 1 6.5 19.79Z' fill='#ffd800'/><path d='M11 12L6.5 19.79A9 9 0 0 1 2 12Z' fill='#00a000'/><path d='M11 12L2 12A9 9 0 0 1 6.5 4.21Z' fill='#0050e0'/><path d='M11 12L6.5 4.21A9 9 0 0 1 15.5 4.21Z' fill='#c000c0'/><path d='M11 12L15.5 4.21A9 9 0 0 1 20 12Z' fill='#ff8000'/><circle cx='11' cy='12' r='9' fill='none' stroke='#000'/><circle cx='11' cy='12' r='1.6' fill='#fff' stroke='#000' stroke-width='.6'/><path d='M8.5 .8h5L11 4.5z' fill='#fff' stroke='#000' stroke-width='.8'/>`,
  rave: `<rect x='.5' y='.5' width='21' height='21' fill='#200040' stroke='#000'/><path d='M5 7.5L12 21M5 7.5L20 16' stroke='#fff' stroke-width='.6' opacity='.7'/><rect x='2' y='14' width='3' height='7' fill='#ff00ff'/><rect x='6.5' y='11' width='3' height='10' fill='#00ffff'/><rect x='11' y='16' width='3' height='5' fill='#80ff00'/><rect x='15.5' y='12' width='3' height='9' fill='#ffff00'/><circle cx='5' cy='4.5' r='3' fill='#d8d8d8' stroke='#fff' stroke-width='.5'/><path d='M2 4.5h6M5 1.5v6' stroke='#808080' stroke-width='.5'/>`,
  blackhole: `<rect x='.5' y='.5' width='21' height='21' fill='#000020' stroke='#000'/><path d='M3 3h.01M18 4h.01M4 18h.01M19 17h.01M15 2h.01' stroke='#fff' stroke-width='1.2' stroke-linecap='round'/><ellipse cx='11' cy='11' rx='9' ry='3.5' fill='none' stroke='#ff9000' stroke-width='1.6' transform='rotate(-20 11 11)'/><circle cx='11' cy='11' r='4.5' fill='#000' stroke='#a060ff'/>`,
  hammer: `<rect x='1.5' y='1.5' width='14' height='14' fill='#c8f0ff' stroke='#000'/><path d='M8 8L1.5 5M8 8L5 1.5M8 8L13 1.5M8 8L15.5 10M8 8L10 15.5M8 8L1.5 13M4 4.5L6.5 3M11.5 4L14 6M12 12L9 13' stroke='#000' stroke-width='.7' fill='none'/><path d='M20.5 20.5l-8-8' stroke='#8b4513' stroke-width='2.2' stroke-linecap='round'/><path d='M13.9 9L16 11.1L11.1 16L9 13.9Z' fill='#808080' stroke='#000' stroke-width='.7'/>`,
  cups: `<rect x='.5' y='17' width='21' height='4.5' fill='#2e7d32' stroke='#000'/><circle cx='11' cy='15.3' r='1.7' fill='#fff' stroke='#000' stroke-width='.5'/><path d='M1 17L2.5 9h3L7 17z' fill='#d00000' stroke='#000' stroke-width='.7'/><path d='M8 12L9.5 4h3L14 12z' fill='#d00000' stroke='#000' stroke-width='.7'/><path d='M15 17L16.5 9h3L21 17z' fill='#d00000' stroke='#000' stroke-width='.7'/><path d='M1.8 14h4.4M8.8 9h4.4M15.8 14h4.4' stroke='#ff9090'/>`,
};
const R95_ICON_CSS = Object.entries(R95_SVG).map(([id, body]) =>
  `.pbt-r95-ico-${id} { background: url("data:image/svg+xml,${encodeURIComponent(`<svg xmlns='http://www.w3.org/2000/svg' viewBox='0 0 22 22'>${body}</svg>`)}") center / contain no-repeat; }`);

// Where each real-visual effect sits under Programs, and its icon. An effect not listed here shows
// straight under Programs with the plain program icon.
const R95_FX_PLACE = {
  coaster: ['Games', 'coaster'], wheel: ['Games', 'wheel'], shellgame: ['Games', 'cups'],
  jukebox: ['Multimedia', 'jukebox'], rave: ['Multimedia', 'rave'],
  shatter: ['Accessories', 'hammer'], blackhole: ['Screen Savers', 'blackhole'],
};
const R95_FX_ORDER = ['coaster', 'wheel', 'shellgame', 'jukebox', 'rave', 'shatter', 'blackhole'];
const R95_FOLDERS = ['Games', 'Multimedia', 'Accessories', 'Screen Savers'];

// The fixed part of the look. P is the scoped prefix; OUT and IN are the raised and sunken bevels.
const R95_CSS = (() => {
  const P = R95_SEL;
  // The Filters pane body follows "Keep the report's Filters pane colors", as the skins do.
  const PF = `${R95_SEL}:not(.pbt-report-filters)`;
  const chrome = 'titlebar, ribbon-report-view, view-navigator, .sidePane, .outspacePane, status-bar, exploration-footer-modern, .cdk-overlay-container, .ms-Layer';
  const iconHosts = '.ribbonContainer, div.ribbonSearchOuter, .sidePane, .outspacePane, section.statusBar, exploration-footer-modern, .viewNavigatorContainer';
  return `
${P} {
  --r95-out: inset -1px -1px #000000, inset 1px 1px #ffffff, inset -2px -2px #808080, inset 2px 2px #dfdfdf;
  --r95-in: inset -1px -1px #ffffff, inset 1px 1px #000000, inset -2px -2px #dfdfdf, inset 2px 2px #808080;
  --r95-title: linear-gradient(90deg, #000080 0%, #1060b0 100%);
  /* The skins' shared hooks (skins/_base.css), so the parts a skin draws through them follow too. */
  --pbt-font: ${R95_FONT}; --pbt-ui-font: ${R95_FONT};
  --pbt-titlebar-bg: var(--r95-title); --pbt-titlebar-fg: #ffffff; --pbt-titlebar-hover: rgba(255, 255, 255, .2);
  --pbt-icon: #ffffff; --pbt-icon-alt: #c0c0c0; --pbt-icon-fill: #ffffff; --pbt-icon-line: #212121; --pbt-icon)PBT"
LR"PBT(-nav: #000000; --pbt-icon-active: #000000;
  --pbt-search-fg: #000000; --pbt-tabs-bg: #c0c0c0; --pbt-tab-fg: #000000; --pbt-tab-active-bg: #c0c0c0; --pbt-tab-active-fg: #000000; --pbt-tab-hover-bg: #c0c0c0;
  --pbt-file-bg: #c0c0c0; --pbt-file-fg: #000000; --pbt-ribbon-bg: #c0c0c0; --pbt-group-fg: #000000; --pbt-nav-bg: #c0c0c0; --pbt-nav-active-bg: #ffffff;
  --pbt-pagetabs-bg: #c0c0c0; --pbt-pagetab-bg: #c0c0c0; --pbt-pagetab-active-bg: #e0e0e0;
  --pbt-panehead-bg: var(--r95-title); --pbt-panehead-solid: #000080; --pbt-panehead-fg: #ffffff; --pbt-panehead-case: none; --pbt-panehead-tracking: normal;
  --pbt-status-bg: #c0c0c0; --pbt-status-fg: #000000; --pbt-gutter: #008080; --pbt-accent: #000080;
  --pbt-hover-bg: #d4d4d4; --pbt-hover-ring: #000000; --pbt-hover-radius: 0;
  --pbt-share-bg: #c0c0c0; --pbt-share-fg: #000000;
  --pbt-wc-inset: 6px 5px; --pbt-wc-radius: 0; --pbt-wc-fg: #000000; --pbt-wc-close-fg: #000000; --pbt-wc-close-hover-fg: #000000;
  --pbt-wc-face: #c0c0c0; --pbt-wc-close-face: #c0c0c0; --pbt-wc-hover-face: #d4d4d4; --pbt-wc-close-hover-face: #d4d4d4;
  --pbt-wc-ring: var(--r95-out); --pbt-wc-hover-bg: transparent; --pbt-wc-close-hover-bg: transparent;
  --pbt-ui-bg: #c0c0c0; --pbt-ui-fg: #000000; --pbt-ui-border: #808080; --pbt-ui-radius: 0;
}

/* Square corners on the chrome; the report page keeps its own. */
${P} :is(${chrome}) :not(.displayArea, .displayArea *) { border-radius: 0 !important; }

/* System fonts of the era, 11 px, on the chrome only (icon fonts excluded, as in the skins). */
${P} :is(${chrome}) :is(span, div, button, label, h1, h2, h3, h4, input, li, a, p):not([class*="glyph" i]):not([class*="icon" i]):not([class*="arrow" i]):not([class*="chevron" i]):not([class*="caret" i]):not(i):not(.zoomButton):not(.displayArea *) {
  font-family: ${R95_FONT} !important; font-size: 11px !important; letter-spacing: normal !important; font-stretch: normal !important;
}

/* Title bar: the navy strip, white bold title. */
${P} div.titlebar { background: var(--r95-title) !important; color: #ffffff !important; }
${P} div.titlebar :is(.title, .timestamp, .dotSeparator, .accountNameLabel, .caretdownIcon, .titlebarControl) { color: #ffffff !important; }
${P} div.titlebar .title { font-weight: bold !important; }
${P} div.titlebar .title::before { display: none !important; }
${P} titlebar-toolbar button { color: #ffffff !important; }
${P} titlebar-toolbar svg :is(.OfficeIconColors_HighContrast, .OfficeIconColors_DynamicColor, .OfficeIconColors_m22, .OfficeIconColors_m252) { fill: #ffffff !important; }
${P} titlebar-toolbar svg .OfficeIconColors_m20 { fill: transparent !important; }
${P} titlebar account .imageAvatarIcon { color: #ffffff !important; background: transparent !important; }
${P}:not([data-pbt-skin]) .windowControls button { color: #ffffff !important; }
${P}:not([data-pbt-skin]) .windowControls svg [class^="OfficeIconColors"] { fill: currentColor !important; }
${P} div.ribbonSearchOuter { background: #ffffff !important; box-shadow: var(--r95-in) !important; border: 0 !important; }
${P} div.ribbonSearchOuter * { background-color: transparent !important; border-color: transparent !important; }
${P} div.ribbonSearchOuter :is(input, svg, span, i) { color: #000000 !important; }
${P} div.ribbonSearchOuter input::placeholder { color: #4a4a4a !important; opacity: 1 !important; }
${P} .farPeripheralControls button { background: #c0c0c0 !important; color: #000000 !important; border: 0 !important; box-shadow: var(--r95-out) !important; }
${P} .farPeripheralControls button * { color: #000000 !important; }
${P} .farPeripheralControls svg :is(path, [class^="OfficeIconColors"]) { fill: currentColor !important; }

/* Ribbon: silver tab row, property-sheet tabs, a raised panel and raised buttons. */
${P} :is(.ribbonTopBarContainer, .ribbonContainer, .ribbonContainer div:has(> .bottomBarContainer)) { background: #c0c0c0 !important; }
${P} .ribbonTopBarContainer > div, ${P} .ribbonTopBarContainer > div > div { background: transparent !important; }
${P} :is(.ribbonContainer button[role="tab"], [id="Ribbon-file"]) {
  background: #c0c0c0 !important; color: #000000 !important; filter: none !important; border-radius: 0 !important;
  box-shadow: inset 1px 1px #ffffff, inset -1px 0 #000000, inset -2px 0 #808080 !important; margin-top: 3px !important; height: 26px !important;
}
${P} :is(.ribbonContainer button[role="tab"], [id="Ribbon-file"]) * { color: #000000 !important; }
${P} .ribbonContainer button[role="tab"]::after { display: none !important; }
/* Power BI's sliding underline is placed from the tab widths and would sit under the wrong tab. */
${P} #selectedPivotIndicator { display: none !important; }
${P} .ribbonContainer button[role="tab"][aria-selected="true"] { font-weight: bold !important; background: #c0c0c0 !important; box-shadow: inset 1px 1px #ffffff, inset -1px 0 #000000, inset -2px 0 #808080, 0 2px 0 #c0c0c0 !important; }
${P} .ribbonContainer .bottomBarContainer { background: #c0c0c0 !important; border: 0 !important; box-shadow: var(--r95-out) !important; border-radius: 0 !important; }
${P} .ribbonContainer :is(span, label, div, button):not([class*="icon" i]) { color: #000000; }
${P} .ribbonContainer [role="group"] label[class*="groupLabel"] { color: #000000 !important; }
${P} .ribbonContainer .bottomBarContainer [role="group"] button:not(:disabled):not(.is-disabled) {
  background: #c0c0c0 !important; color: #000000 !important; box-shadow: var(--r95-out) !important; border-radius: 0 !important; filter: none !important;
}
${P} .ribbonContainer .bottomBarContainer [role="group"] button:not(:disabled):not(.is-disabled):hover { background: #d4d4d4 !important; box-shadow: var(--r95-out) !important; }
${P} .ribbonContainer .bottomBarContainer [role="group"] button:not(:disabled):not(.is-disabled):is(:active, [aria-pressed="true"], [aria-expanded="true"], .is-checked) { background: #dfdfdf !important; box-shadow: var(--r95-in) !important; }
${P} .ribbonContainer .bottomBa)PBT"
LR"PBT(rContainer [role="group"] button:is(:disabled, .is-disabled) { background: transparent !important; box-shadow: none !important; color: #4a4a4a !important; }
${P} .ribbonContainer .bottomBarContainer [role="group"] button:is(:disabled, .is-disabled) * { color: #4a4a4a !important; }
${P} .ribbonContainer [class*="groupContainer"] + [class*="groupContainer"] { border-left: 1px solid #808080 !important; box-shadow: inset 1px 0 #ffffff; }

/* Office icons drawn for a light surface, whatever the mode or skin. */
${P} :is(${iconHosts}) svg :is(.OfficeIconColors_HighContrast, .OfficeIconColors_m22, .OfficeIconColors_m252) { fill: #212121 !important; }
${P} :is(${iconHosts}) svg .OfficeIconColors_m20 { fill: #ffffff !important; }
${P} :is(${iconHosts}) svg .OfficeIconColors_m23 { fill: #616161 !important; }
${P} :is(${iconHosts}) svg .OfficeIconColors_m21 { fill: #9e9e9e !important; }
${P} :is(.pane-header, .outspacePane .paneHeader) svg :is(.OfficeIconColors_HighContrast, .OfficeIconColors_m22, .OfficeIconColors_m252, .OfficeIconColors_m23) { fill: #ffffff !important; }
${P} :is(.pane-header, .outspacePane .paneHeader) svg :is(.OfficeIconColors_m20, .OfficeIconColors_m21) { fill: transparent !important; }

/* View switcher on the left: a silver column, the open view a sunken white well. */
${P} .viewNavigatorContainer { background: #c0c0c0 !important; box-shadow: var(--r95-out) !important; }
${P} view-navigator button { color: #000000 !important; background: transparent !important; }
${P} view-navigator button[aria-selected="true"] { background: #ffffff !important; box-shadow: var(--r95-in) !important; }

/* Panes (Filters, Visualizations, Data and the rest): raised silver windows with navy title strips. */
:is(${P} .sidePane, ${PF} .outspacePane) { background-color: #c0c0c0 !important; color: #000000 !important; box-shadow: var(--r95-out) !important; border-color: #808080 !important; }
${P} :is(.pane-header, .outspacePane .paneHeader) { background: var(--r95-title) !important; color: #ffffff !important; text-transform: none !important; letter-spacing: normal !important; }
${P} :is(.pane-header, .outspacePane .paneHeader)::before { display: none !important; }
${P} :is(.pane-header, .outspacePane .paneHeader) :is(h2, h3, button, span, div, .glyphicon, i) { color: #ffffff !important; background-color: transparent !important; text-transform: none !important; }
${P} :is(.pane-header, .outspacePane .paneHeader) :is(h2, .pane-title, .sidePaneTitle) { font-weight: bold !important; }
${P} .outspacePane.outspacePaneCollapsed { background: var(--r95-title) !important; color: #ffffff !important; }
${P} .outspacePane.outspacePaneCollapsed * { color: #ffffff !important; }
${P} .outspacePane.outspacePaneCollapsed .paneHeader { background: transparent !important; margin: 0 !important; }
:is(${P} .sidePane, ${PF} .outspacePane) .paneContents :is(div, span, section, label, p, h3, h4, a):not([class*="glyph" i]):not([class*="icon" i]) { color: #000000; }
:is(${P} .sidePane, ${PF} .outspacePane) :is(input, textarea, search-box-modern) { background: #ffffff !important; color: #000000 !important; box-shadow: var(--r95-in) !important; border-color: transparent !important; }
:is(${P} .sidePane, ${PF} .outspacePane) search-box-modern input { box-shadow: none !important; }
:is(${P} .sidePane, ${PF} .outspacePane) :is(input, textarea)::placeholder { color: #4a4a4a !important; opacity: 1 !important; }
/* Filter cards and the "Add data fields here" wells: Power BI draws them with light grey 1px borders
   (#c8c8c8 and #b4b4b4) that vanish on the silver pane. Cards become raised panels, wells sunken
   white fields, both with the black and white bevel edges. Only the well boxes themselves: the card
   wrappers, field chips and the strip under them are drop targets too. */
${PF} .outspacePane .paneContents .card { background-color: #c0c0c0 !important; border-color: transparent !important; box-shadow: var(--r95-out) !important; }
:is(${PF} .outspacePane .dropzone, ${P} .sidePane ol.properties, ${P} .sidePane .cards).droppableElement { background-color: #ffffff !important; border-color: transparent !important; box-shadow: var(--r95-in) !important; }
:is(${PF} .outspacePane .dropzone, ${P} .sidePane ol.properties, ${P} .sidePane .cards).droppableElement.ui-droppable-hover { background-color: #ffffe1 !important; }
${PF} .outspacePane mat-select { background-color: #ffffff !important; border-color: transparent !important; box-shadow: var(--r95-in) !important; }

/* Page tabs at the bottom: raised tabs on a silver bar, the open one bold and lighter. */
${P} :is(exploration-footer-modern, ul.pane.sections, .explorationNavigationContent) { background: #c0c0c0 !important; }
${P} .carouselScrollPane li.section { background: #c0c0c0 !important; color: #000000 !important; box-shadow: var(--r95-out) !important; filter: none !important; border-radius: 0 !important; }
${P} .carouselScrollPane li.section * { color: #000000 !important; }
${P} .carouselScrollPane li.section.selected { background: #e0e0e0 !important; font-weight: bold !important; box-shadow: inset 1px 0 #ffffff, inset -1px 0 #000000, inset -2px 0 #808080, inset 0 -2px #e0e0e0 !important; }
${P} .carouselScrollPane li.section:not(.selected):hover { background: #d4d4d4 !important; box-shadow: var(--r95-out) !important; }
${P} button.section.static.create { color: #000000 !important; --add-new-page-icon-color: #000000; --add-new-page-icon-color-hover: #000000; --add-new-page-icon-color-pressed: #000000; }
${P} button.section.static.create .thumbnail-container { background: #c0c0c0 !important; box-shadow: var(--r95-out) !important; border-radius: 0 !important; }

/* Status bar: the taskbar. Silver with a white top edge; the Begin button and the clock are ours. */
${P} section.statusBar { background: #c0c0c0 !important; color: #000000 !important; box-shadow: inset 0 1px #dfdfdf, inset 0 2px #ffffff !important; }
${P} section.statusBar :is(div, span, button, label):not(.pbt-chip, .pb)PBT"
LR"PBT(t-chip *) { color: #000000 !important; }
${P} section.statusBar .leftPane::before { display: none !important; }
${P} .pbt-r95-begin {
  display: inline-flex !important; align-items: center; gap: 4px; height: 20px; margin: 1px 6px 1px 2px; padding: 0 7px 0 5px; flex: none;
  background: #c0c0c0 !important; color: #000000 !important; box-shadow: var(--r95-out) !important; border: 0; border-radius: 0;
  font: bold 11px ${R95_FONT} !important; cursor: pointer; vertical-align: middle;
}
${P} .pbt-r95-begin:active { box-shadow: var(--r95-in) !important; padding: 1px 6px 0 6px; }
${P} .pbt-r95-logo { position: relative; display: inline-block; width: 11px; height: 11px; flex: none; }
/* The logo: four plain squares in a staggered stack, drawn with box shadows. */
${P} .pbt-r95-logo::before {
  content: ""; position: absolute; left: 0; top: 6px; width: 5px; height: 5px; background: #000080;
  box-shadow: 6px 0 0 #008080, 0 -6px 0 #800000, 6px -6px 0 #808000;
}
${P} .pbt-r95-clock {
  display: inline-block !important; height: 18px; line-height: 18px; margin: 1px 4px 1px 6px; padding: 0 10px; flex: none; order: 999;
  background: #c0c0c0 !important; color: #000000 !important; box-shadow: inset -1px -1px #ffffff, inset 1px 1px #808080 !important;
  font: 11px ${R95_FONT} !important; white-space: nowrap; vertical-align: middle;
}

/* The teal desktop around the report page. The visuals keep their own look, with no added frame or shadow. */
${P} .explorationContainer .exploration { background: #008080 !important; }

/* The classic dotted focus rectangle on the chrome. */
${P} :is(${chrome}) :focus-visible, ${P} .pbt-r95-begin:focus-visible { outline: 1px dotted #000000 !important; outline-offset: -4px !important; }
${P} titlebar :focus-visible, ${P} :is(.pane-header, .outspacePane .paneHeader) :focus-visible { outline-color: #ffffff !important; }

/* Chunky grey scrollbars everywhere but inside the report page. */
${P} *:not(.displayArea, .displayArea *)::-webkit-scrollbar { width: 16px; height: 16px; background: repeating-conic-gradient(#c0c0c0 0 25%, #ffffff 0 50%) 0 0 / 2px 2px; }
${P} *:not(.displayArea, .displayArea *)::-webkit-scrollbar-thumb { background: #c0c0c0; box-shadow: var(--r95-out); border-radius: 0; border: 0; }
${P} *:not(.displayArea, .displayArea *)::-webkit-scrollbar-button:single-button { display: block; width: 16px; height: 16px; background: #c0c0c0; box-shadow: var(--r95-out); }
${P} *:not(.displayArea, .displayArea *)::-webkit-scrollbar-button:single-button:active { box-shadow: var(--r95-in); }
${P} *:not(.displayArea, .displayArea *)::-webkit-scrollbar-corner { background: #c0c0c0; }

/* The Begin menu: a raised panel above the Begin button, a navy band down its left side. */
${P} .pbt-r95-menu {
  position: fixed; z-index: 2147483004; display: flex; box-sizing: border-box; min-width: 280px; padding: 3px;
  background: #c0c0c0 !important; color: #000000 !important; box-shadow: var(--r95-out) !important; font: 11px ${R95_FONT} !important; user-select: none;
}
${P} .pbt-r95-band { display: flex; align-items: flex-end; justify-content: center; flex: none; width: 24px; padding-bottom: 6px; background: linear-gradient(0deg, #000080, #1084d0); }
${P} .pbt-r95-band span { writing-mode: vertical-rl; transform: rotate(180deg); color: #c0c0c0; font: 18px ${R95_FONT} !important; white-space: nowrap; }
${P} .pbt-r95-band b { color: #ffffff; font-weight: bold; }
${P} .pbt-r95-items { display: flex; flex-direction: column; flex: 1; min-width: 0; }
${P} .pbt-r95-item {
  display: flex; align-items: center; gap: 10px; height: 34px; padding: 0 20px 0 8px; border: 0; border-radius: 0;
  background: transparent !important; color: #000000 !important; font: 11px ${R95_FONT} !important; text-align: left; white-space: nowrap; cursor: pointer;
}
${P} .pbt-r95-item:is(:hover, :focus-visible) { background: #000080 !important; color: #ffffff !important; outline: 0 !important; }
${P} .pbt-r95-sep { height: 0; margin: 3px 2px; border-top: 1px solid #808080; border-bottom: 1px solid #ffffff; }
/* Small icons, drawn with CSS only. */
${P} .pbt-r95-ico { position: relative; display: inline-block; flex: none; box-sizing: border-box; width: 22px; height: 22px; }
${P} .pbt-r95-ico::before, ${P} .pbt-r95-ico::after { content: ""; position: absolute; box-sizing: border-box; }
/* A spreadsheet: a white grid under a green header row. */
${P} .pbt-r95-ico-sheet {
  border: 1px solid #000000; background-color: #ffffff;
  background-image: linear-gradient(#008000, #008000), repeating-linear-gradient(0deg, transparent 0 4px, #808080 4px 5px), repeating-linear-gradient(90deg, transparent 0 6px, #808080 6px 7px);
  background-size: 100% 5px, auto, auto; background-repeat: no-repeat;
}
/* A gold coin with a dollar sign. */
${P} .pbt-r95-ico-coin { border: 1px solid #000000; border-radius: 50%; background: radial-gradient(circle at 35% 35%, #ffff9c, #c8a000); }
${P} .pbt-r95-ico-coin::after { content: "$"; inset: 0; display: grid; place-items: center; color: #5a3d00; font: bold 13px ${R95_FONT}; }
/* A sheet with lines and a pencil. */
${P} .pbt-r95-ico-tab { width: 18px; margin: 0 2px; border: 1px solid #000000; background: #ffffff; }
${P} .pbt-r95-ico-tab::before { left: 2px; right: 2px; top: 4px; height: 2px; background: #000080; box-shadow: 0 5px #808080, 0 10px #808080; }
${P} .pbt-r95-ico-tab::after { right: -4px; bottom: -3px; width: 4px; height: 13px; background: linear-gradient(#ff8080 0 15%, #ffd800 15% 78%, #000000 78%); transform: rotate(35deg); }
/* A grey server box with two lights. */
${P} .pbt-r95-ico-server { width: 14px; margin: 0 4px; background: #c0c0c0; box-shadow: var(--r95-out); }
${P} .pbt-r95-ico-server::before { left: 3px; top: 4px; width: 8px; height: 2px; background: #404040; box-shadow: 0 4px #404040; }
${P} .pbt-r95-ico-server::after { left: 3px; bottom: 3px; width: 3px; height: 3px; background: #00e000; box-shadow: 4px 0 #ff0000; }
/* Two tables and the line between them. */
${P} .pbt-r95-ic)PBT"
LR"PBT(o-link { background: linear-gradient(to top right, transparent calc(50% - 1px), #000000 calc(50% - 1px) calc(50% + 1px), transparent calc(50% + 1px)); }
${P} .pbt-r95-ico-link::before { left: 0; bottom: 0; width: 10px; height: 10px; border: 1px solid #000000; background: #ffff80; }
${P} .pbt-r95-ico-link::after { right: 0; top: 0; width: 10px; height: 10px; border: 1px solid #000000; background: #80ffff; }
/* A monitor on its stand. */
${P} .pbt-r95-ico-monitor { height: 16px; margin-bottom: 6px; border: 1px solid #000000; background: #000080; box-shadow: inset 0 0 0 2px #c0c0c0; }
${P} .pbt-r95-ico-monitor::after { left: 5px; bottom: -6px; width: 10px; height: 5px; border: 1px solid #000000; background: #808080; }
/* A program in the Begin menu (the real-visual effects): a little window with a navy title strip. */
${P} .pbt-r95-ico-app { height: 16px; margin-bottom: 6px; border: 1px solid #000000; background: linear-gradient(#000080 0 4px, #ffffff 4px 100%); }
/* The SVG icons: folders, programs and games. */
${R95_ICON_CSS.map(rule => `${P} ${rule}`).join('\n')}
/* Folders open a submenu to the right on hover (or click), as the mid-90s Start menu did. The open
   folder's row stays highlighted while the pointer is in its submenu. */
${P} .pbt-r95-folder { position: relative; display: flex; flex-direction: column; }
${P} .pbt-r95-folder > .pbt-r95-item { padding-right: 8px; }
${P} .pbt-r95-folder.pbt-r95-open > .pbt-r95-item { background: #000080 !important; color: #ffffff !important; }
${P} .pbt-r95-arrow { flex: none; width: 0; height: 0; margin-left: auto; padding: 0; border-left: 4px solid currentColor; border-top: 4px solid transparent; border-bottom: 4px solid transparent; }
${P} .pbt-r95-sub {
  position: absolute; left: 100%; top: -3px; z-index: 1; display: none; flex-direction: column; box-sizing: border-box; min-width: 200px; padding: 3px;
  background: #c0c0c0 !important; color: #000000 !important; box-shadow: var(--r95-out) !important;
}
${P} .pbt-r95-folder.pbt-r95-open > .pbt-r95-sub { display: flex; }
${P} .pbt-r95-sub .pbt-r95-item { height: 30px; gap: 8px; padding-left: 6px; }
${P} .pbt-r95-empty { color: #808080 !important; cursor: default; padding-left: 36px !important; }
${P} .pbt-r95-empty:hover { background: transparent !important; color: #808080 !important; }
/* The joke items' little dialogs: a raised window with a navy title strip, progress blocks, a
   text box with a blinking caret, and buttons. None of it takes clicks. */
${P} .pbt-r95-dlg {
  position: fixed; left: 50%; top: 45%; z-index: 2147483004; box-sizing: border-box; padding: 3px; transform: translate(-50%, -50%); pointer-events: none;
  background: #c0c0c0 !important; color: #000000 !important; box-shadow: var(--r95-out) !important; font: 11px ${R95_FONT} !important;
}
${P} .pbt-r95-dlg-title { height: 20px; line-height: 20px; padding: 0 6px; background: var(--r95-title); color: #ffffff; font-weight: bold; }
${P} .pbt-r95-dlg-body { display: grid; gap: 10px; padding: 12px 12px 10px; }
${P} .pbt-r95-prog { display: flex; gap: 2px; height: 20px; box-sizing: border-box; padding: 3px; background: #ffffff; box-shadow: var(--r95-in); }
${P} .pbt-r95-prog i { flex: 1; background: #000080; }
${P} .pbt-r95-prog-green i { background: #008000; }
${P} .pbt-r95-field { display: flex; align-items: center; height: 22px; padding: 0 5px; background: #ffffff; box-shadow: var(--r95-in); font: 12px ${R95_FONT}; white-space: pre; }
${P} .pbt-r95-caret { width: 1px; height: 14px; margin-left: 1px; background: #000000; animation: pbt-r95-blink 1s steps(1) infinite; }
@keyframes pbt-r95-blink { 50% { opacity: 0; } }
${P} .pbt-r95-btns { display: flex; justify-content: flex-end; gap: 6px; }
${P} .pbt-r95-btn { min-width: 70px; padding: 4px 0; text-align: center; background: #c0c0c0; box-shadow: var(--r95-out); }
${P} .pbt-r95-btn-down { box-shadow: var(--r95-in); }
${P} .pbt-r95-fly { position: fixed; left: 0; top: 0; z-index: 2147483005; width: 44px !important; height: 44px !important; pointer-events: none; }
${P} .pbt-r95-glass { position: fixed; left: 0; top: 0; z-index: 2147483005; width: 72px; height: 72px; pointer-events: none; }
${P} .pbt-r95-rel { position: fixed; inset: 0; z-index: 2147483004; pointer-events: none; }
/* The screen after Shut Down: black, with orange text in the middle. */
${P} .pbt-r95-off { position: fixed; inset: 0; z-index: 2147483646; display: grid; place-items: center; padding: 0 8vw; background: #000000 !important; cursor: none; }
${P} .pbt-r95-off div { color: #ff8c00 !important; font: bold min(5vw, 40px)/1.3 ${R95_FONT} !important; text-align: center; }

/* Our own menu and toasts join in. */
${P} .pbt-toast { border: 0 !important; border-radius: 0 !important; }
${P} .pbt-menu button:hover { background: #000080 !important; color: #ffffff !important; --pbt-ui-fg: #ffffff; --pbt-ui-bg: #000080; --pbt-accent: #ffffff; }
`;
})();

// What preview() counts: a label per part of the window and the selector it matched.
const R95_PARTS = [
  ['title bar', 'div.titlebar'],
  ['ribbon tabs', '.ribbonContainer button[role="tab"], [id="Ribbon-file"]'],
  ['ribbon panel', '.ribbonContainer .bottomBarContainer'],
  ['ribbon buttons', '.ribbonContainer .bottomBarContainer [role="group"] button'],
  ['panes', '.sidePane, .outspacePane'],
  ['pane title strips', '.pane-header, .outspacePane .paneHeader'],
  ['view switcher', '.viewNavigatorContainer'],
  ['page tabs', '.carouselScrollPane li.section'],
  ['status bar', 'section.statusBar'],
  ['desktop', '.explorationContainer .exploration'],
];

const retro95 = {
  name: 'retro95',
  active: false,
  timer: null,
  themeMo: null,
  remapped: 0,
  where: '',
  menuEl: null,
  menuOff: null,
  shutTimer: null,

  // Who wants the look on: 'chaos' (Chaos Mode) and 'classic' (the Classic skin with its 95 style
  // picked). It goes off when the last one lets go; off() with no owner turns it off outright.
  owners: new Set(),

  // The core calls this at boot. Th)PBT"
LR"PBT(e look waits for on(); Chaos Mode turns it on, and so does the
  // Classic skin when its style is set to 95.
  start() { this.syncClassic(); },
  stop() { this.off(); },
  onConfig() { this.syncClassic(); },

  // Classic's two styles: 'clean' (the Classic skin itself) and '95' (this look in its place). Kept
  // with the other choices (pbt.cfg, saved under pbid-skins), so every Power BI view follows a
  // change through the storage event, and reset with them when the mod's settings change.
  classicStyle() { return pbt.cfg && pbt.cfg.classicStyle === '95' ? '95' : 'clean'; },
  classicOn() { return this.owners.has('classic'); },
  setClassicStyle(style) {
    pbt.cfg.classicStyle = style === '95' ? '95' : 'clean';
    pbt.save();
    this.syncClassic(true);
  },
  syncClassic(force = false) {
    const want = pbt.cfg.skin === 'classic' && this.classicStyle() === '95';
    const had = this.owners.has('classic');
    if (want && !had) this.on('classic');
    else if (!want && had) this.off('classic');
    // Classic picked again, or a choice arriving from another view, draws the Classic skin again
    // under the look: set it aside at once rather than at the next tick.
    else if (want && had && this.active) this.noSkin();
    // The Skins group and the menu mark the chosen skin and show the style switch; redraw them.
    if (force || want !== had) pbt.modules.forEach(m => { if (m !== this && m.onApplied) try { m.onApplied(pbt.skin && pbt.skin.current); } catch {} });
  },

  on(owner = 'chaos') {
    this.owners.add(owner);
    if (this.active) return 'retro95 already on';
    this.active = true;
    // The look replaces any skin rather than mixing with one: the skin drawn now is set aside
    // (not saved, so the saved choice stays) and comes back on off().
    this.prevSkin = this.noSkin();
    document.documentElement.classList.add('pbt-retro95');
    this.paint();
    this.taskbar();
    // One tick keeps the clock right to the minute and puts the taskbar pieces back if Power BI
    // redraws its status bar. It only reads the clock and checks two nodes, so it costs nothing.
    // A skin picked meanwhile (Ctrl+Alt+] or the menu) is set aside the same way.
    this.timer = setInterval(() => {
      if (!this.active) return;
      const picked = this.noSkin();
      if (picked) this.prevSkin = picked;
      this.taskbar();
    }, 1000);
    // Switching Power BI between light and dark brings new theme values; remap them again.
    this.themeMo = new MutationObserver(() => { if (this.active) this.paint(); });
    this.themeMo.observe(document.documentElement, { attributes: true, attributeFilter: ['data-theme'] });
    window.dispatchEvent(new Event('resize'));
    return 'retro95 on';
  },

  // Draws no skin if one is drawn; returns the id it took off, else ''.
  noSkin() {
    const s = pbt.skin;
    const now = s && s.current;
    if (!s || !now || now === 'off' || !pbt.skins[now]) return '';
    s.apply('off');
    return now;
  },

  off(owner) {
    if (owner) {
      this.owners.delete(owner);
      if (this.owners.size && this.active) return `retro95 still on for ${[...this.owners].join(', ')}`;
    }
    this.owners.clear();
    const was = this.active;
    // The programs from the Begin menu go with the look.
    if (was) this.stopFx();
    // The skin to draw again is the saved choice, not the one that was set aside: None picked
    // while the look was on stays None, and a skin picked meanwhile comes back.
    const back = was && pbt.cfg.skin;
    this.prevSkin = '';
    if (back && pbt.skins[back] && pbt.skin && (!pbt.skin.current || pbt.skin.current === 'off')) pbt.skin.apply(back);
    this.active = false;
    this.closeMenu();
    this.endGag();
    clearTimeout(this.shutTimer); this.shutTimer = null;
    clearInterval(this.timer); this.timer = null;
    if (this.themeMo) { this.themeMo.disconnect(); this.themeMo = null; }
    document.querySelectorAll('[data-pbt-r95]').forEach(n => n.remove());
    const html = document.documentElement;
    html.classList.remove('pbt-retro95');
    // An html element that had no class at all gets no empty class attribute left behind.
    if (html.getAttribute('class') === '') html.removeAttribute('class');
    this.where = '';
    // Power BI re-measures its layout on a resize (about 12 ms); only when the look was on.
    if (was) window.dispatchEvent(new Event('resize'));
    return 'retro95 off';
  },

  toggle() { return this.active ? this.off() : this.on(); },

  // The style element goes last in <head>, after every skin sheet, and carries data-pbt so the
  // skins' own theme reader skips it.
  paint() {
    let s = document.getElementById('pbt-retro95');
    if (!s) {
      s = document.createElement('style');
      s.id = 'pbt-retro95'; s.dataset.pbt = ''; s.dataset.pbtR95 = '';
    }
    let theme = { css: '', count: 0 };
    try { theme = r95ThemeCss(); } catch (e) { pbt.log && pbt.log(`retro95 theme: ${e.message}`); }
    this.remapped = theme.count;
    s.textContent = `${theme.css}\n${R95_CSS}`;
    document.head.append(s);
  },

  clockText() {
    const d = new Date(), h = d.getHours();
    return `${h % 12 || 12}:${String(d.getMinutes()).padStart(2, '0')} ${h < 12 ? 'AM' : 'PM'}`;
  },

  // The Begin button goes first in the status bar's left pane; the clock goes at the end of the
  // bar's last pane (its far right), else at the end of the left pane. Neither covers anything:
  // both take their own room in the bar, and neither adds a pane that would shift the others.
  taskbar() {
    const bar = document.querySelector('section.statusBar');
    if (!bar) return;
    const left = bar.querySelector('.leftPane') || bar;
    let begin = bar.querySelector('.pbt-r95-begin');
    // The Begin menu and its programs are part of the "Hidden surprises" setting; off, the look
    // keeps its taskbar and clock without it.
    if (!begin && pbt.feature('surprises')) {
      begin = pbt.el('button', {
        class: 'pbt-r95-)PBT"
LR"PBT(begin', type: 'button', 'data-pbt': '', 'data-pbt-r95': '', 'aria-label': 'Begin',
        onpointerdown: e => e.stopPropagation(),
        'aria-haspopup': 'menu', 'aria-expanded': 'false',
        onclick: e => { e.stopPropagation(); this.menu(); },
      }, pbt.el('span', { class: 'pbt-r95-logo', 'aria-hidden': 'true' }), pbt.el('span', { text: 'Begin' }));
      left.prepend(begin);
    }
    let clock = bar.querySelector('.pbt-r95-clock');
    const panes = [...bar.children].filter(c => !('pbtR95' in c.dataset));
    const right = panes.length > 1 ? panes[panes.length - 1] : null;
    if (!clock) {
      clock = pbt.el('span', { class: 'pbt-r95-clock', 'data-pbt': '', 'data-pbt-r95': '', role: 'timer', 'aria-label': 'Clock' });
      (right && right !== left ? right : left).append(clock);
    }
    const t = this.clockText();
    if (clock.textContent !== t) clock.textContent = t;
    this.where = clock.parentElement === left ? 'left pane' : `last status pane (${clock.parentElement.className || clock.parentElement.tagName.toLowerCase()})`;
  },

  // The Begin menu, opened and closed by the Begin button. A click outside it or Esc closes it;
  // each item closes it and tells a joke (Barry's line while Chaos Mode is on, else a toast).
  menu() {
    if (!this.active) return false;
    if (this.menuEl) { this.closeMenu(); return false; }
    const begin = document.querySelector('.pbt-r95-begin');
    if (!begin) return false;
    const ico = icon => pbt.el('span', { class: `pbt-r95-ico pbt-r95-ico-${icon}`, 'aria-hidden': 'true' });
    // Entering a row opens it when it is a folder and closes any other open folder beside it, after
    // a short pause, so a diagonal move into a submenu does not open the rows it crosses.
    const hover = row => { clearTimeout(this.menuHover); this.menuHover = setTimeout(() => this.openRow(row), 200); };
    const item = (icon, label, fn) => {
      const b = pbt.el('button', {
        class: 'pbt-r95-item', type: 'button', role: 'menuitem',
        onclick: e => { e.stopPropagation(); this.closeMenu(); fn(); },
      }, ico(icon), pbt.el('span', { text: label }));
      b.addEventListener('mouseenter', () => hover(b));
      return b;
    };
    const folder = (icon, label, children) => {
      const row = pbt.el('button', {
        class: 'pbt-r95-item', type: 'button', role: 'menuitem', 'aria-haspopup': 'menu', 'aria-expanded': 'false',
        onclick: e => { e.stopPropagation(); clearTimeout(this.menuHover); this.openRow(f); },
      }, ico(icon), pbt.el('span', { text: label }), pbt.el('span', { class: 'pbt-r95-arrow', 'aria-hidden': 'true' }));
      const f = pbt.el('div', { class: 'pbt-r95-folder' }, row,
        pbt.el('div', { class: 'pbt-r95-sub', role: 'menu', 'aria-label': label },
          ...(children.length ? children : [pbt.el('div', { class: 'pbt-r95-item pbt-r95-empty', text: '(Empty)' })])));
      row.addEventListener('mouseenter', () => hover(f));
      return f;
    };
    const joke = text => () => this.joke(text);
    // The real-visual effects (page/82 to 84) register themselves in pbt.chaosFx; each is a program
    // in its folder under Programs, and launching one first stops any effect still running.
    const fx = pbt.chaosFx || {};
    const ids = [...R95_FX_ORDER.filter(id => fx[id]), ...Object.keys(fx).filter(id => !R95_FX_ORDER.includes(id) && fx[id] && typeof fx[id].run === 'function')];
    const program = id => item((R95_FX_PLACE[id] || [])[1] || 'app', fx[id].name || id, () => this.launch(id));
    const programs = [
      ...R95_FOLDERS.map(name => [name, ids.filter(id => (R95_FX_PLACE[id] || [])[0] === name)])
        .filter(([, list]) => list.length).map(([name, list]) => folder('folder', name, list.map(program))),
      ...ids.filter(id => !R95_FX_PLACE[id]).map(program),
    ];
    const m = pbt.el('div', {
      class: 'pbt-r95-menu', role: 'menu', 'aria-label': 'Begin', 'data-pbt': '', 'data-pbt-r95': '',
      onpointerdown: e => e.stopPropagation(), onmousedown: e => e.stopPropagation(), onclick: e => e.stopPropagation(),
    },
    pbt.el('div', { class: 'pbt-r95-band', 'aria-hidden': 'true' }, pbt.el('span', {}, pbt.el('b', { text: 'Power BI' }), ' 95')),
    pbt.el('div', { class: 'pbt-r95-items' },
      folder('programs', 'Programs', programs),
      folder('documents', 'Documents', [
        item('sheet', 'Export to Excel (we know you will)', () => this.gag('excel')),
        item('tab', 'Rename Sheet1', () => this.gag('rename')),
      ]),
      folder('settings', 'Settings', [item('server', 'Reboot the gateway', () => this.gag('reboot'))]),
      folder('find', 'Find', [item('link', 'The missing relationship', () => this.gag('find'))]),
      item('coin', 'Ask the CFO', joke('The CFO asks: can we get this in Excel?')),
      pbt.el('div', { class: 'pbt-r95-sep', role: 'separator' }),
      item('monitor', 'Shut Down...', () => this.shutdown())));
    for (const sep of m.querySelectorAll('.pbt-r95-sep')) sep.addEventListener('mouseenter', () => clearTimeout(this.menuHover));
    const r = begin.getBoundingClientRect();
    m.style.left = `${Math.max(0, Math.round(r.left))}px`;
    m.style.bottom = `${Math.round(innerHeight - r.top + 2)}px`;
    document.body.append(m);
    this.menuEl = m;
    begin.setAttribute('aria-expanded', 'true');
    const outside = e => { if (!m.contains(e.target) && !begin.contains(e.target)) this.closeMenu(); };
    const key = e => { if (e.key === 'Escape') { e.preventDefault(); e.stopPropagation(); this.closeMenu(); } };
    document.addEventListener('pointerdown', outside, true);
    window.addEventListener('keydown', key, true);
    this.menuOff = () => { document.removeEventListener('pointerdown', outside, true); window.removeEventListener('keydown', key, true); };
    return true;
  },

  // Runs one registered effect, stopping any other that is still running first.
  launch(id) {
    const fx = pbt.chaosFx || {};
    const f = fx[id];
    if (!f || typeof f.run)PBT"
LR"PBT( !== 'function') return false;
    const running = fx.__running;
    if (running && running !== id && fx[running] && typeof fx[running].stop === 'function') { try { fx[running].stop(); } catch {} }
    Object.defineProperty(fx, '__running', { value: id, writable: true, configurable: true, enumerable: false });
    // Each launch gets its own token: a relaunched effect's earlier run settling later must not
    // clear the mark of the run that replaced it.
    const token = (this.launchToken || 0) + 1;
    this.launchToken = token;
    let out;
    try { out = f.run(); } catch { out = false; }
    Promise.resolve(out).finally(() => { if (this.launchToken === token && fx.__running === id) fx.__running = ''; });
    return out;
  },

  // Ends every registered effect that is running (page/82 to 84).
  stopFx() {
    const fx = pbt.chaosFx || {};
    for (const id of Object.keys(fx)) { const f = fx[id]; if (f && typeof f.stop === 'function') { try { f.stop(); } catch {} } }
    if (Object.prototype.hasOwnProperty.call(fx, '__running')) fx.__running = '';
  },

  // Opens a folder row (closing the other open folders beside it), or for a plain row only closes
  // them. A submenu that would run past the bottom of the window is moved up to fit.
  openRow(row) {
    if (!row || !row.isConnected || !this.menuEl || !this.menuEl.contains(row)) return;
    const shut = f => {
      for (const x of [f, ...f.querySelectorAll('.pbt-r95-open')]) { x.classList.remove('pbt-r95-open'); x.firstElementChild.setAttribute('aria-expanded', 'false'); }
    };
    for (const sib of row.parentElement.children) if (sib !== row && sib.classList.contains('pbt-r95-open')) shut(sib);
    if (!row.classList.contains('pbt-r95-folder') || row.classList.contains('pbt-r95-open')) return;
    row.classList.add('pbt-r95-open');
    row.firstElementChild.setAttribute('aria-expanded', 'true');
    const sub = row.lastElementChild;
    sub.style.top = '';
    const over = sub.getBoundingClientRect().bottom - (innerHeight - 4);
    if (over > 0) sub.style.top = `${-3 - Math.ceil(over)}px`;
  },

  closeMenu() {
    clearTimeout(this.menuHover);
    if (this.menuOff) { this.menuOff(); this.menuOff = null; }
    if (this.menuEl) { this.menuEl.remove(); this.menuEl = null; }
    const begin = document.querySelector('.pbt-r95-begin');
    if (begin) begin.setAttribute('aria-expanded', 'false');
  },

  joke(text) {
    try { if (pbt.chaos && typeof pbt.chaos.say === 'function' && pbt.chaos.say(text)) return; } catch {}
    if (pbt.toast) pbt.toast(text, { title: 'Power BI 95', ms: 4000 });
  },

  // The Begin menu's joke items, each a short animation and then its line: 'excel' (an export
  // dialog whose progress bar stalls, then the sheet flies into Begin), 'rename' (a rename box
  // that types FINAL, FINAL_v2, FINAL_v2_REAL and settles on Sheet1 (2)), 'reboot' (every visual
  // powers down to grey, a reboot dialog runs, the visuals flash back on one by one) and 'find' (a
  // magnifying glass searches the page and draws a relationship line between two visuals).
  // Overlays carry data-pbt-r95 and animations on Power BI's visuals are Web Animations only, all
  // ended by endGag(), which off() calls. One gag at a time; a new one ends the last.
  gag(id) {
    if (!this.active) return false;
    this.endGag();
    const G = this.gagRun = { timers: new Set(), anims: new Set(), els: new Set() };
    const later = (ms, fn) => {
      const t = setTimeout(() => { G.timers.delete(t); if (this.gagRun === G) { try { fn(); } catch {} } }, ms);
      G.timers.add(t);
    };
    const anim = (el, frames, opts) => { const a = el.animate(frames, opts); G.anims.add(a); return a; };
    const mk = (cls, parent = document.body) => {
      const e = pbt.el('div', { class: cls, 'data-pbt': '', 'data-pbt-r95': '' });
      parent.append(e); G.els.add(e); return e;
    };
    const done = (text, ms) => later(ms, () => { this.endGag(); this.joke(text); });
    const dialog = (title, width) => {
      const d = mk('pbt-r95-dlg');
      d.style.width = `${width}px`;
      const body = pbt.el('div', { class: 'pbt-r95-dlg-body' });
      d.append(pbt.el('div', { class: 'pbt-r95-dlg-title', text: title }), body);
      anim(d, [{ transform: 'translate(-50%, -50%) scale(.6)', opacity: 0 }, { transform: 'translate(-50%, -50%) scale(1)', opacity: 1 }], { duration: 180, easing: 'ease-out', fill: 'forwards' });
      return { d, body };
    };
    // A Windows 95 progress bar of 20 blocks; when(i) says after how many ms block i appears.
    const progress = (body, green, when) => {
      const bar = pbt.el('div', { class: `pbt-r95-prog${green ? ' pbt-r95-prog-green' : ''}` });
      body.append(bar);
      for (let i = 0; i < 20; i += 1) {
        const c = pbt.el('i');
        c.style.visibility = 'hidden';
        bar.append(c);
        later(when(i), () => { c.style.visibility = 'visible'; });
      }
    };
    const visuals = () => [...document.querySelectorAll('.displayArea .visualContainer')]
      .filter(v => !(v.parentElement && v.parentElement.closest('.visualContainer')) && v.getBoundingClientRect().width > 40);

    if (id === 'excel') {
      const { d, body } = dialog('Exporting to Excel', 330);
      body.append(pbt.el('div', { text: 'Copying 1,048,576 rows nobody will look at...' }));
      // Quick to 75%, then the famous last-block stall.
      progress(body, true, i => 150 + i * 70 + (i > 14 ? (i - 14) * 160 : 0));
      later(2700, () => {
        const r = d.getBoundingClientRect();
        const begin = document.querySelector('.pbt-r95-begin');
        const t = begin ? begin.getBoundingClientRect() : { left: 12, top: innerHeight - 24, width: 0, height: 0 };
        const x0 = r.left + r.width / 2 - 22, y0 = r.top + r.height / 2 - 22, x1 = t.left + t.width / 2 - 22, y1 = t.top + t.height / 2 - 22;
        const ico = mk('pbt-r95-ico pbt-r95-ico-sheet pbt-r95-fly');
        anim(d, [{ opacity: 1 }, { opacity: 0 }], { duration: 200, fill)PBT"
LR"PBT(: 'forwards' });
        anim(ico, [
          { transform: `translate(${x0}px, ${y0}px) scale(1) rotate(0deg)` },
          { transform: `translate(${(x0 + x1) / 2}px, ${Math.min(y0, y1) - 140}px) scale(1.5) rotate(-200deg)`, offset: 0.5 },
          { transform: `translate(${x1}px, ${y1}px) scale(.25) rotate(-360deg)` },
        ], { duration: 900, easing: 'ease-in', fill: 'forwards' });
      });
      done('Exported. Excel says welcome back.', 3700);
      return true;
    }

    if (id === 'rename') {
      const { body } = dialog('Rename', 300);
      body.append(pbt.el('div', { text: 'New name for Sheet1:' }));
      const txt = pbt.el('span', { text: 'Sheet1' });
      const field = pbt.el('div', { class: 'pbt-r95-field' }, txt, pbt.el('b', { class: 'pbt-r95-caret' }));
      const ok = pbt.el('span', { class: 'pbt-r95-btn', text: 'OK' });
      body.append(field, pbt.el('div', { class: 'pbt-r95-btns' }, ok, pbt.el('span', { class: 'pbt-r95-btn', text: 'Cancel' })));
      // Every keystroke, with its pause: type to a name, deleting back to the shared start first.
      let cur = 'Sheet1', at = 450;
      const show = (s, ms) => { at += ms; later(at, () => { txt.textContent = s; }); };
      const typeTo = target => {
        let p = 0;
        while (p < cur.length && p < target.length && cur[p] === target[p]) p += 1;
        while (cur.length > p) { cur = cur.slice(0, -1); show(cur, 40); }
        while (cur.length < target.length) { cur = target.slice(0, cur.length + 1); show(cur, 85); }
      };
      typeTo('Sheet1 FINAL'); at += 350;
      typeTo('Sheet1 FINAL_v2'); at += 350;
      typeTo('Sheet1 FINAL_v2_REAL'); at += 550;
      typeTo('Sheet1 (2)'); at += 300;
      later(at, () => ok.classList.add('pbt-r95-btn-down'));
      done('Renamed to Sheet1 (2). Progress!', at + 350);
      return true;
    }

    if (id === 'reboot') {
      const vis = visuals();
      const step = 60, off = vis.length * step + 260;
      const dark = { filter: 'grayscale(1) brightness(.35)', opacity: 0.5 };
      vis.forEach((v, i) => anim(v, [{ filter: 'none', opacity: 1 }, dark], { duration: 220, delay: i * step, fill: 'forwards' }));
      let box = null;
      later(off, () => {
        box = dialog('On-premises data gateway', 320);
        box.body.append(pbt.el('div', { text: 'Restarting... please do not unplug the cloud.' }));
        progress(box.body, false, i => 80 + i * 70);
      });
      later(off + 1750, () => {
        if (box) anim(box.d, [{ opacity: 1 }, { opacity: 0 }], { duration: 200, fill: 'forwards' });
        vis.forEach((v, i) => anim(v, [dark, { filter: 'brightness(1.8)', opacity: 1, offset: 0.6 }, { filter: 'none', opacity: 1 }], { duration: 380, delay: i * step, fill: 'forwards' }));
      });
      done('Gateway rebooted. Back sometime this quarter.', off + 1750 + vis.length * step + 500);
      return true;
    }

    if (id === 'find') {
      const rects = visuals().map(v => v.getBoundingClientRect());
      if (rects.length < 2) { this.endGag(); this.joke('Found it. It was hiding behind the date table.'); return true; }
      const mid = r => ({ x: r.left + r.width / 2, y: r.top + r.height / 2 });
      // Two visuals to join, and a search path over up to four others that ends between them.
      const order = rects.map((_, i) => i).sort(() => Math.random() - 0.5);
      const A = mid(rects[order[0]]), B = mid(rects[order[1]]);
      const path = [...order.slice(2, 6).map(i => mid(rects[i])), A, { x: (A.x + B.x) / 2, y: (A.y + B.y) / 2 }];
      const glass = mk('pbt-r95-glass');
      glass.innerHTML = "<svg viewBox='0 0 64 64' width='72' height='72'><circle cx='26' cy='26' r='18' fill='rgba(191,239,255,.35)' stroke='#000' stroke-width='5'/><path d='M39 39l18 18' stroke='#000' stroke-width='8' stroke-linecap='round'/></svg>";
      anim(glass, path.map(p => ({ transform: `translate(${p.x - 26}px, ${p.y - 26}px)` })), { duration: 2200, easing: 'ease-in-out', fill: 'forwards' });
      later(2300, () => {
        const len = Math.hypot(B.x - A.x, B.y - A.y);
        const rel = mk('pbt-r95-rel');
        const tag = (p, q, t) => `<g transform='translate(${p.x + (q.x - p.x) * 0.12 - 9} ${p.y + (q.y - p.y) * 0.12 - 9})'><rect width='18' height='18' fill='#ffff80' stroke='#000'/><text x='9' y='14' text-anchor='middle' font-family='Tahoma, sans-serif' font-size='13' font-weight='bold'>${t}</text></g>`;
        rel.innerHTML = `<svg width='100%' height='100%'><line x1='${A.x}' y1='${A.y}' x2='${B.x}' y2='${B.y}' stroke='#000080' stroke-width='4' stroke-linecap='round' stroke-dasharray='${len}' stroke-dashoffset='${len}'/>${tag(A, B, '1')}${tag(B, A, '*')}</svg>`;
        const line = rel.querySelector('line');
        anim(line, [{ strokeDashoffset: len }, { strokeDashoffset: 0 }], { duration: 600, easing: 'ease-out', fill: 'forwards' });
        rel.querySelectorAll('g').forEach((g, i) => anim(g, [{ opacity: 0 }, { opacity: 1 }], { duration: 250, delay: 500 + i * 150, fill: 'both' }));
        anim(glass, [{ opacity: 1 }, { opacity: 0 }], { duration: 300, fill: 'forwards' });
      });
      done('Found it. It was hiding behind the date table.', 2300 + 1700);
      return true;
    }
    this.endGag();
    return false;
  },

  endGag() {
    const G = this.gagRun;
    if (!G) return;
    this.gagRun = null;
    G.timers.forEach(t => clearTimeout(t)); G.timers.clear();
    G.anims.forEach(a => { try { a.cancel(); } catch {} }); G.anims.clear();
    G.els.forEach(e => e.remove()); G.els.clear();
  },

  // Shut Down...: a black screen with orange text for 3 s, then the report again.
  shutdown() {
    this.closeMenu();
    document.querySelectorAll('.pbt-r95-off').forEach(n => n.remove());
    const s = pbt.el('div', {
      class: 'pbt-r95-off', role: 'alert', 'data-pbt': '', 'data-pbt-r95': '',
      onpointerdown: e => { e.preventDefault(); e.stopPropagation(); },
    }, pbt.el('div', { text: 'It is now safe to turn off your report' }));
    document.body.append)PBT"
LR"PBT((s);
    clearTimeout(this.shutTimer);
    this.shutTimer = setTimeout(() => { this.shutTimer = null; s.remove(); }, 3000);
    return true;
  },

  // For the lead's test: what the look matched, part by part, in one line.
  preview() {
    const counts = R95_PARTS.map(([label, sel]) => `${label} ${document.querySelectorAll(sel).length}`);
    const begin = !!document.querySelector('.pbt-r95-begin'), clock = document.querySelector('.pbt-r95-clock');
    return `retro95 ${this.active ? 'on' : 'off'}: ${counts.join(', ')}; ${this.remapped} theme colors remapped; `
      + `taskbar ${begin ? 'Begin' : 'no Begin'} + ${clock ? `clock ${clock.textContent}` : 'no clock'}${this.where ? ` in ${this.where}` : ''}`;
  },
};
pbt.retro95 = retro95;
pbt.module(retro95);

// ---- 82-fx-sound.js ----
// Chaos Mode effects that ride and play the report's own line chart (part of the "Hidden
// surprises" setting). "Rollercoaster.exe" sends a little googly-eyed cart along the real line of the
// first line chart on the page (slow uphill with a clack-clack, fast downhill with a whistle) and
// off the right edge in an arc. "Data Jukebox" plays that line as a melody, one pentatonic note per
// data point, while a glowing playhead and a dot sweep across the chart. While Chaos Mode is on, a
// click on a pie or donut slice also plays a drum (kick, snare or hi-hat by slice index) with a
// small pulse of the slice. Everything is drawn on top of the report or done with Web Animations
// that are cancelled afterwards; Power BI's own attributes are only read, never written, and every
// sound is synthesized with WebAudio.
//
// Contract, so the lead can inject this file on its own into a page where the mod already runs:
// - It needs only `pbt` (window.__pbt) in scope. Everything else sits inside one block, so its
//   names never meet another page file's.
// - Effects register in the shared registry the Begin menu reads:
//   pbt.chaosFx.coaster and pbt.chaosFx.jukebox, each { name, run(), stop() }. run() returns a
//   Promise that resolves when the effect is done (true, or 'stopped'), or false when the page has
//   no line chart (Barry says why). stop() ends it at once and removes every trace.
//   Test calls: window.__pbt.chaosFx.coaster.run(), window.__pbt.chaosFx.jukebox.run().
// - The file registers as a module (pbt.fxSound, then pbt.module): the core calls start() at boot,
//   which adds the slice drum listener, and stop() from pbt.remove(). Evaluated after the core has
//   booted, it starts itself when Chaos Mode's module is already started; otherwise call
//   window.__pbt.fxSound.start(). Evaluating the file again replaces the previous copy.
// - It needs no style element: every node it adds is inline styled, marked data-pbt, and removed
//   when its effect ends, so window.__pbt.chaos.audit() reads the same before and after.
{
  if (pbt.fxSound && typeof pbt.fxSound.remove === 'function') { try { pbt.fxSound.remove(); } catch {} }

  const SVG_NS = 'http://www.w3.org/2000/svg';
  const svgEl = (tag, attrs = {}, ...kids) => {
    const e = document.createElementNS(SVG_NS, tag);
    for (const [k, v] of Object.entries(attrs)) if (v != null) e.setAttribute(k, v);
    for (const c of kids.flat()) if (c != null) e.append(c);
    return e;
  };
  // Built with createElement only, like pbt.el, in case the app turns on Trusted Types.
  const htmlEl = (tag, props = {}, text) => {
    const e = document.createElement(tag);
    for (const [k, v] of Object.entries(props)) if (v != null) e.setAttribute(k, v);
    if (text != null) e.textContent = text;
    e.setAttribute('data-pbt', '');
    return e;
  };
  const safe = fn => function (...args) { try { return fn.apply(this, args); } catch (e) { try { pbt.log && pbt.log(`fx-sound: ${e && e.stack}`); } catch {} } };
  const clamp = (v, lo, hi) => Math.min(Math.max(v, lo), hi);

  const S = { started: false, base: [], ctx: null, master: null, closeT: null, runs: {}, pulses: new Map() };

  // ==== sound: one AudioContext, made when first needed, closed 3 s after the last sound ends
  // (a drum hit, or the last running effect) and by stop(), so no audio device stays open ====
  const closeAudio = () => {
    clearTimeout(S.closeT); S.closeT = null;
    if (S.ctx) { try { S.ctx.close().catch(() => {}); } catch {} S.ctx = null; S.master = null; }
  };
  const closeSoon = () => {
    clearTimeout(S.closeT);
    S.closeT = setTimeout(() => { S.closeT = null; if (!Object.keys(S.runs).length) closeAudio(); }, 3000);
  };
  const audio = () => {
    // Silent unless Skin sounds is on (off by default), like every other sound of the mod.
    if (!pbt.feature('sounds')) return null;
    clearTimeout(S.closeT); S.closeT = null;
    try {
      if (!S.ctx || S.ctx.state === 'closed') {
        S.ctx = new AudioContext();
        S.master = S.ctx.createGain();
        S.master.gain.value = 0.8;
        S.master.connect(S.ctx.destination);
      }
      if (S.ctx.state === 'suspended') S.ctx.resume().catch(() => {});
      return S.ctx;
    } catch { return null; }
  };
  // Each run of an effect plays through its own gain, so stopping it silences notes already queued.
  const newBus = () => {
    const ctx = audio();
    if (!ctx) return null;
    try { const g = ctx.createGain(); g.gain.value = 1; g.connect(S.master); return g; } catch { return null; }
  };
  const tone = ({ dest, type = 'square', f = 440, f2, at = 0, len = 0.1, vol = 0.04, attack = 0.008, decay = false, lp }) => {
    const ctx = audio();
    if (!ctx || !dest) return;
    try {
      const o = ctx.createOscillator(), g = ctx.createGain();
      const t = ctx.currentTime + at;
      o.type = type;
      o.frequency.setValueAtTime(f, t);
      if (f2) o.frequency.exponentialRampToValueAtTime(f2, t + len);
      g.gain.setValueAtTime(0.0001, t);
      g.gain.exponentialRampToValueAtTime(vol, t + attack);
      if (!decay) g.gain.setValueAtTime(vol, t + Math.max(attack + 0.002, len - 0.03));
      g.gain.expon)PBT"
LR"PBT(entialRampToValueAtTime(0.0001, t + len);
      let out = o;
      if (lp) { const flt = ctx.createBiquadFilter(); flt.type = 'lowpass'; flt.frequency.value = lp; out = o.connect(flt); }
      out.connect(g).connect(dest);
      o.start(t); o.stop(t + len + 0.05);
    } catch {}
  };
  // A burst of filtered white noise that dies away.
  const noise = ({ dest, at = 0, len = 0.1, vol = 0.1, type = 'bandpass', f = 2000, q = 1 }) => {
    const ctx = audio();
    if (!ctx || !dest) return;
    try {
      const t = ctx.currentTime + at, buf = ctx.createBuffer(1, Math.ceil(ctx.sampleRate * len), ctx.sampleRate), data = buf.getChannelData(0);
      for (let i = 0; i < data.length; i += 1) data[i] = Math.random() * 2 - 1;
      const n = ctx.createBufferSource(), flt = ctx.createBiquadFilter(), g = ctx.createGain();
      n.buffer = buf; flt.type = type; flt.frequency.value = f; flt.Q.value = q;
      g.gain.setValueAtTime(vol, t);
      g.gain.exponentialRampToValueAtTime(0.0001, t + len);
      n.connect(flt).connect(g).connect(dest);
      n.start(t); n.stop(t + len + 0.02);
    } catch {}
  };
  const sound = {
    // One click of the lift chain, alternating two pitches for the clack-clack.
    clack(dest, alt) {
      noise({ dest, len: 0.03, vol: 0.14, type: 'highpass', f: alt ? 2600 : 3600 });
      tone({ dest, f: alt ? 150 : 185, len: 0.03, vol: 0.05, attack: 0.002, decay: true });
    },
    // A whistle that rises and falls, with a little vibrato: the "wheee".
    whee(dest, len = 0.8) {
      const ctx = audio();
      if (!ctx || !dest) return;
      try {
        const t = ctx.currentTime, o = ctx.createOscillator(), g = ctx.createGain(), lfo = ctx.createOscillator(), depth = ctx.createGain();
        o.type = 'sine';
        o.frequency.setValueAtTime(620, t);
        o.frequency.exponentialRampToValueAtTime(1550, t + len * 0.3);
        o.frequency.exponentialRampToValueAtTime(460, t + len);
        lfo.frequency.value = 9; depth.gain.value = 22;
        lfo.connect(depth).connect(o.frequency);
        g.gain.setValueAtTime(0.0001, t);
        g.gain.exponentialRampToValueAtTime(0.06, t + 0.05);
        g.gain.setValueAtTime(0.06, t + len * 0.7);
        g.gain.exponentialRampToValueAtTime(0.0001, t + len);
        o.connect(g).connect(dest);
        o.start(t); lfo.start(t); o.stop(t + len + 0.05); lfo.stop(t + len + 0.05);
      } catch {}
    },
    // A soft plucked note: a triangle with a quiet filtered square an octave below.
    note(dest, f, at) {
      tone({ dest, type: 'triangle', f, at, len: 0.3, vol: 0.09, attack: 0.008, decay: true });
      tone({ dest, type: 'square', f: f / 2, at, len: 0.22, vol: 0.018, attack: 0.008, decay: true, lp: 1400 });
    },
    kick(dest, at = 0, vol = 0.32) { tone({ dest, type: 'sine', f: 150, f2: 42, at, len: 0.26, vol, attack: 0.003, decay: true }); noise({ dest, at, len: 0.015, vol: vol * 0.3, type: 'lowpass', f: 1500 }); },
    snare(dest, at = 0, vol = 0.2) { noise({ dest, at, len: 0.17, vol, type: 'bandpass', f: 1900, q: 0.7 }); tone({ dest, type: 'triangle', f: 200, f2: 160, at, len: 0.09, vol: vol * 0.5, attack: 0.002, decay: true }); },
    hat(dest, at = 0, vol = 0.1) { noise({ dest, at, len: 0.05, vol, type: 'highpass', f: 7500 }); },
  };

  // ==== Barry and Chaos Mode ====
  const barry = text => { try { if (pbt.chaos && typeof pbt.chaos.say === 'function') pbt.chaos.say(text); } catch {} };
  // isOn() is one flag; state() builds a full report (it scans the page for pies), too much per click.
  const chaosOn = () => { try { return !!(pbt.chaos && (typeof pbt.chaos.isOn === 'function' ? pbt.chaos.isOn() : pbt.chaos.state().on)); } catch { return false; } };

  // ==== finding the first line chart's series line ====
  // Top-level visual containers only, as page/80-chaos.js finds them, in reading order. Custom
  // visuals draw inside iframes, which these queries never enter.
  const containers = () => [...document.querySelectorAll('.displayArea .visualContainer')]
    .filter(el => !(el.parentElement && el.parentElement.closest('.visualContainer')))
    .map(el => ({ el, r: el.getBoundingClientRect() }))
    .filter(v => v.r.width > 0 && v.r.height > 0)
    .sort((a, b) => (Math.abs(a.r.top - b.r.top) > 8 ? a.r.top - b.r.top : a.r.left - b.r.left))
    .map(v => v.el);
  const containerOf = n => (n && n.closest && n.closest('.displayArea .visualContainer')) || null;
  const typeOf = v => (((v.querySelector('[class*="visual-"]') || { getAttribute: () => '' }).getAttribute('class') || '').match(/\bvisual-(\w+)/) || [])[1] || '';
  const isLineVisual = v => /line|area/i.test(`${v.getAttribute('aria-roledescription') || ''} ${typeOf(v)}`);
  const cls = p => p.getAttribute('class') || '';
  const usable = p => {
    try {
      const r = p.getBoundingClientRect();
      return r.width > 30 && r.bottom > 0 && r.top < innerHeight && r.right > 0 && r.left < innerWidth && p.getTotalLength() > 40;
    } catch { return false; }
  };
  // The series path: one whose class says line (not an axis, a gridline or the wide invisible
  // hit area), or in a line chart, failing that, the widest stroked path with no fill.
  const seriesPath = (v, loose) => {
    const paths = [...v.querySelectorAll('svg path')].filter(p => !p.closest('[data-pbt]'));
    let found = paths.filter(p => /line/i.test(cls(p)) && !/domain|axis|grid|interactiv|reference|trend|marker|label|legend/i.test(cls(p)) && usable(p));
    if (!found.length && loose) {
      found = paths.filter(p => {
        if (/domain|axis|grid|area|legend/i.test(cls(p))) return false;
        const cs = getComputedStyle(p);
        return cs.fill === 'none' && cs.stroke && cs.stroke !== 'none' && usable(p);
      });
    }
    let best = null, bw = 0;
    for (const p of found) { const w = p.getBoundingClientRect().width; if (w > bw + 1) { best = p; bw = w; } }
    return best;
  };
  const findLine = () => {
    const all = containers();
    for (const v of all.filter(isLineVisual)) { )PBT"
LR"PBT(const p = seriesPath(v, true); if (p) return { v, path: p }; }
    for (const v of all.filter(v => !isLineVisual(v))) { const p = seriesPath(v, false); if (p) return { v, path: p }; }
    return null;
  };
  const titleOf = v => {
    const t = v.querySelector('[class*="visualTitle"], [data-testid*="visual-title"]');
    const s = ((t && t.textContent) || '').replace(/\s+/g, ' ').trim();
    return s && s.length <= 50 ? s : '';
  };

  // ==== from the path's own coordinates to the screen ====
  // Points are kept in the path's user space and mapped every frame, so a scroll or a zoom
  // during the effect is followed. getScreenCTM gives the map; it is checked against the path's
  // real box on screen and corrected when the two disagree (a CSS zoom on an ancestor).
  const samplePath = (path, n) => {
    const len = path.getTotalLength(), out = [];
    for (let i = 0; i <= n; i += 1) { const p = path.getPointAtLength((len * i) / n); out.push([p.x, p.y]); }
    return out;
  };
  const screenMap = (path, dense) => {
    const m = path.getScreenCTM();
    if (!m) return null;
    const raw = ([x, y]) => [m.a * x + m.c * y + m.e, m.b * x + m.d * y + m.f];
    let x0 = Infinity, y0 = Infinity, x1 = -Infinity, y1 = -Infinity;
    for (const pt of dense) { const [x, y] = raw(pt); x0 = Math.min(x0, x); x1 = Math.max(x1, x); y0 = Math.min(y0, y); y1 = Math.max(y1, y); }
    const r = path.getBoundingClientRect();
    if (Math.abs(r.left - x0) < 3 && Math.abs(r.top - y0) < 3 && Math.abs(r.right - x1) < 3 && Math.abs(r.bottom - y1) < 3) return raw;
    const sx = x1 - x0 > 1 ? r.width / (x1 - x0) : 1, sy = y1 - y0 > 1 ? r.height / (y1 - y0) : 1;
    return pt => { const [x, y] = raw(pt); return [r.left + (x - x0) * sx, r.top + (y - y0) * sy]; };
  };
  // The vertices of the path data: the end point of every segment, which on a Power BI line is
  // one point per data point. Relative commands and implicit repeats are followed.
  const ARGS = { m: 2, l: 2, h: 1, v: 1, c: 6, s: 4, q: 4, t: 2, a: 7, z: 0 };
  const vertices = d => {
    const tokens = String(d || '').match(/[MLHVCSQTAZmlhvcsqtaz]|-?(?:\d+\.?\d*|\.\d+)(?:[eE][-+]?\d+)?/g) || [];
    const out = [];
    let cmd = '', x = 0, y = 0, sx = 0, sy = 0, i = 0;
    while (i < tokens.length) {
      if (/^[a-z]$/i.test(tokens[i])) { cmd = tokens[i]; i += 1; if (/z/i.test(cmd)) { x = sx; y = sy; continue; } }
      const lc = cmd.toLowerCase(), k = ARGS[lc];
      if (!k) return out;
      const a = tokens.slice(i, i + k).map(Number);
      if (a.length < k || a.some(n => !Number.isFinite(n))) return out;
      i += k;
      const rel = cmd !== cmd.toUpperCase();
      if (lc === 'h') x = rel ? x + a[0] : a[0];
      else if (lc === 'v') y = rel ? y + a[0] : a[0];
      else { x = rel ? x + a[k - 2] : a[k - 2]; y = rel ? y + a[k - 1] : a[k - 1]; }
      if (lc === 'm') { sx = x; sy = y; cmd = rel ? 'l' : 'L'; }
      out.push([x, y]);
    }
    return out;
  };

  // ==== one run of an effect: its frame, timers, nodes, animations and sound, all undone by end ====
  const newRun = id => {
    const r = { id, raf: 0, timers: new Set(), els: new Set(), anims: new Set(), bus: newBus(), done: false, resolve: null, chaos: chaosOn(), checkedAt: 0 };
    r.promise = new Promise(res => { r.resolve = res; });
    return r;
  };
  const later = (r, fn, ms) => { const t = setTimeout(() => { r.timers.delete(t); safe(fn)(); }, ms); r.timers.add(t); return t; };
  const add = (r, el) => { r.els.add(el); document.body.append(el); return el; };
  const anim = (r, el, frames, opts) => {
    if (!el || typeof el.animate !== 'function') return null;
    const a = el.animate(frames, opts);
    r.anims.add(a);
    a.addEventListener('finish', () => r.anims.delete(a));
    return a;
  };
  const endRun = (r, result) => {
    if (!r || r.done) return;
    r.done = true;
    if (r.raf) cancelAnimationFrame(r.raf);
    r.timers.forEach(t => clearTimeout(t)); r.timers.clear();
    r.anims.forEach(a => { try { a.cancel(); } catch {} }); r.anims.clear();
    r.els.forEach(el => el.remove()); r.els.clear();
    if (r.bus) { try { r.bus.disconnect(); } catch {} r.bus = null; }
    if (S.runs[r.id] === r) delete S.runs[r.id];
    if (!Object.keys(S.runs).length && S.ctx) closeSoon();
    r.resolve(result);
  };
  // Chaos Mode switched off in the middle of an effect ends the effect too (checked twice a second).
  const chaosGone = (r, now) => {
    if (!r.chaos || now - r.checkedAt < 500) return false;
    r.checkedAt = now;
    return !chaosOn();
  };
  // A short text that pops up and floats away, for "Wheee!" and the jukebox's notes.
  const floater = (r, x, y, text, color, size, rise) => {
    const el = add(r, htmlEl('div', { 'aria-hidden': 'true', style: `position:fixed;left:${x}px;top:${y}px;z-index:2147483002;pointer-events:none;font:900 ${size}px/1 "Comic Sans MS","Segoe UI",sans-serif;color:${color};text-shadow:2px 2px 0 rgba(0,0,0,.45);white-space:nowrap;transform:translate(-50%,-50%)` }, text));
    const a = anim(r, el, [
      { transform: 'translate(-50%, -50%) scale(.6)', opacity: 0 },
      { transform: 'translate(-50%, -80%) scale(1.1)', opacity: 1, offset: 0.2 },
      { transform: `translate(-50%, calc(-50% - ${rise}px)) scale(1)`, opacity: 0 },
    ], { duration: 950, easing: 'ease-out', fill: 'forwards' });
    later(r, () => { if (a) { r.anims.delete(a); a.cancel(); } r.els.delete(el); el.remove(); }, 1000);
  };

  // ==== Rollercoaster.exe ====
  // The cart, 60 by 48 with the bottom of its wheels at (30, 47): that point rides on the line.
  const buildCart = () => {
    const ink = { stroke: '#1d1d1d', 'stroke-width': 1.6 };
    const eye = (cx, cy) => {
      const pupil = svgEl('circle', { cx, cy: cy + 1, r: 3.1, fill: '#111' });
      return { g: svgEl('g', {}, svgEl('circle', { cx, cy, r: 7, fill: '#fff', ...ink }), pupil), pupil, cx, cy: cy + 1 };
    };
    const wheel = cx => svgEl('g', {},
      svgEl('circle', { cx, cy: 41, r: 6, fill: '#333)PBT"
LR"PBT(', stroke: '#111', 'stroke-width': 1.4 }),
      svgEl('line', { x1: cx - 4.5, y1: 41, x2: cx + 4.5, y2: 41, stroke: '#bbb', 'stroke-width': 1.4 }),
      svgEl('circle', { cx, cy: 41, r: 1.8, fill: '#ddd' }));
    const e1 = eye(28, 12), e2 = eye(42, 12), w1 = wheel(16), w2 = wheel(44);
    const svg = svgEl('svg', { viewBox: '0 0 60 48', width: 60, height: 48, overflow: 'visible', 'aria-hidden': 'true' },
      svgEl('path', { d: 'M14 18 V9 H20', fill: 'none', stroke: '#555', 'stroke-width': 2.4, 'stroke-linecap': 'round' }),
      svgEl('path', { d: 'M6 18 H50 Q57 18 57 25 V33 Q57 38 52 38 H9 Q4 38 4 33 V20 Q4 18 6 18 Z', fill: '#e5383b', ...ink }),
      svgEl('rect', { x: 5, y: 27, width: 51, height: 4, fill: '#f2c94c' }),
      svgEl('circle', { cx: 52, cy: 23, r: 2, fill: '#fff6c2', stroke: '#1d1d1d', 'stroke-width': 1 }),
      w1, w2, e1.g, e2.g);
    return { svg, eyes: [e1, e2], wheels: [[w1, 16], [w2, 44]] };
  };
  const coasterRun = () => {
    if (S.runs.coaster) return S.runs.coaster.promise;
    const line = findLine();
    if (!line) { barry('No line chart here. The ride is closed for maintenance.'); return false; }
    const { path } = line;
    let user, scale;
    try {
      const m = path.getScreenCTM();
      scale = m ? Math.hypot(m.a, m.b) || 1 : 1;
      const screenLen = path.getTotalLength() * scale;
      user = samplePath(path, clamp(Math.ceil(screenLen / 3), 60, 900));
    } catch { return false; }
    let map = screenMap(path, user);
    if (!map) return false;
    let P = user.map(map);
    // Line charts run left to right; if this one was drawn the other way, ride it reversed.
    if (P[0][0] > P[P.length - 1][0]) { user.reverse(); P.reverse(); }
    const r = S.runs.coaster = newRun('coaster');
    barry('Our revenue has been a wild ride.');
    const cart = buildCart();
    const box = add(r, htmlEl('div', { 'aria-hidden': 'true', style: 'position:fixed;left:0;top:0;width:60px;height:48px;z-index:2147483002;pointer-events:none;transform-origin:30px 47px;will-change:transform;filter:drop-shadow(2px 3px 2px rgba(0,0,0,.35))' }));
    box.append(cart.svg);
    let total = 0;
    for (let i = 1; i < P.length; i += 1) total += Math.hypot(P[i][0] - P[i - 1][0], P[i][1] - P[i - 1][1]);
    // About seven seconds for the ride itself, faster down and slower up.
    const base = clamp(total / 7, 70, 320);
    const c = { f: 0, v: base * 0.6, ang: null, climb: 0, alt: false, wheeAt: 0, last: 0, spin: 0, pv: null, eye: [0, 0, 0, 0], fly: null };
    const place = (x, y, deg) => { box.style.transform = `translate(${(x - 30).toFixed(1)}px, ${(y - 47).toFixed(1)}px) rotate(${deg.toFixed(1)}deg)`; };
    // Googly pupils on a spring that lags behind the cart's acceleration; wheels turn with distance.
    const jiggle = (ax, ay, dt) => {
      const e = c.eye, k = 0.0025;
      const tx = clamp(-ax * k, -2.6, 2.6), ty = clamp(-ay * k, -2.6, 2.6);
      e[2] += ((tx - e[0]) * 120 - e[2] * 9) * dt; e[3] += ((ty - e[1]) * 120 - e[3] * 9) * dt;
      e[0] = clamp(e[0] + e[2] * dt, -3, 3); e[1] = clamp(e[1] + e[3] * dt, -3, 3);
      cart.eyes.forEach(ey => { ey.pupil.setAttribute('cx', (ey.cx + e[0]).toFixed(2)); ey.pupil.setAttribute('cy', (ey.cy + e[1]).toFixed(2)); });
    };
    const roll = d => { c.spin = (c.spin + (d / 6) * (180 / Math.PI)) % 360; cart.wheels.forEach(([w, cx]) => w.setAttribute('transform', `rotate(${c.spin.toFixed(1)} ${cx} 41)`)); };
    const fly = (x, y, now) => {
      const a = (c.ang || 0) * Math.PI / 180, sp = Math.max(c.v, 320);
      c.fly = { x, y, vx: Math.max(340, sp * Math.cos(a)), vy: Math.min(-280, sp * Math.sin(a) - 320), t0: now };
      sound.whee(r.bus, 1.1);
      floater(r, x, y - 50, 'Wheee!', '#ffe600', 26, 50);
    };
    const frame = safe(now => {
      if (r.done) return;
      r.raf = requestAnimationFrame(frame);
      if (chaosGone(r, now)) { endRun(r, 'stopped'); return; }
      const dt = c.last ? clamp((now - c.last) / 1000, 0, 0.05) : 0;
      c.last = now;
      if (c.fly) {
        const f = c.fly;
        f.vy += 1100 * dt; f.x += f.vx * dt; f.y += f.vy * dt;
        const deg = Math.atan2(f.vy, f.vx) * 180 / Math.PI;
        place(f.x, f.y, deg);
        roll(f.vx * dt);
        jiggle(0, -1100, dt);
        if (!f.gone && (f.x > innerWidth + 80 || f.y > innerHeight + 100 || now - f.t0 > 4000)) {
          f.gone = true;
          box.style.visibility = 'hidden';
          later(r, () => endRun(r, true), 1000);
        }
        return;
      }
      if (!path.isConnected) { endRun(r, 'stopped'); return; }
      const m2 = screenMap(path, user);
      if (m2) { map = m2; P = user.map(map); }
      const n = P.length;
      let i = Math.floor(c.f), frac = c.f - i, d = c.v * dt;
      while (d > 0 && i < n - 1) {
        const L = Math.hypot(P[i + 1][0] - P[i][0], P[i + 1][1] - P[i][1]) || 0.0001, rem = (1 - frac) * L;
        if (d < rem) { frac += d / L; d = 0; } else { d -= rem; i += 1; frac = 0; }
      }
      c.f = i + frac;
      const j = Math.min(i + 1, n - 1);
      const x = P[i][0] + (P[j][0] - P[i][0]) * frac, y = P[i][1] + (P[j][1] - P[i][1]) * frac;
      const a = P[Math.max(0, i - 3)], b = P[Math.min(n - 1, i + 4)];
      const dx = b[0] - a[0], dy = b[1] - a[1], h = Math.hypot(dx, dy) || 1;
      const down = dy / h;
      const ang = Math.atan2(dy, dx) * 180 / Math.PI;
      c.ang = c.ang == null ? ang : c.ang + (ang - c.ang) * Math.min(1, dt * 14);
      // Gravity, more or less: the target speed follows the slope, and the cart eases toward it.
      const target = clamp(base * (1 + 2.4 * down), base * 0.32, base * 3.6);
      const v0 = c.v;
      c.v += (target - c.v) * Math.min(1, dt * (target > c.v ? 2.2 : 3.5));
      const travelled = c.v * dt;
      roll(travelled);
      const ax = dt ? ((c.v - v0) / dt) * Math.cos(c.ang * Math.PI / 180) : 0;
      const ay = dt ? ((c.v - v0) / dt) * Math.sin(c.ang * Math.PI / 180) : 0;
      jiggle(ax, ay, dt);
      place(x, y, c.ang);
      // The )PBT"
LR"PBT(lift chain clacks while climbing; a steep drop gets a wheee.
      if (down < -0.12) {
        c.climb += travelled;
        if (c.climb > 15) { c.climb = 0; c.alt = !c.alt; sound.clack(r.bus, c.alt); }
      } else c.climb = 0;
      if (down > 0.45 && now - c.wheeAt > 1400) { c.wheeAt = now; sound.whee(r.bus, 0.8); floater(r, x, y - 46, 'Wheee!', '#ffe600', 20, 36); }
      if (i >= n - 1) fly(x, y, now);
    });
    place(P[0][0], P[0][1], 0);
    r.raf = requestAnimationFrame(frame);
    // A safety net: no ride lasts longer than 20 s.
    later(r, () => endRun(r, true), 20000);
    return r.promise;
  };

  // ==== Data Jukebox ====
  // C major pentatonic over two octaves and one more C, from C4.
  const SCALE = [0, 2, 4, 7, 9, 12, 14, 16, 19, 21, 24].map(s => 261.63 * 2 ** (s / 12));
  const STEP = 1 / 6;
  const jukeboxRun = () => {
    if (S.runs.jukebox) return S.runs.jukebox.promise;
    const line = findLine();
    if (!line) { barry('No line chart here. The band refuses to play a pie.'); return false; }
    const { v, path } = line;
    let dense, verts, map;
    try {
      dense = samplePath(path, 240);
      verts = vertices(path.getAttribute('d'));
      // Too few points to be data (or path data this parser does not know): sample the line instead.
      if (verts.length < 3) verts = samplePath(path, 24);
      map = screenMap(path, dense);
    } catch { return false; }
    if (!map) return false;
    // Neighbours at the same spot (a repeated move or a closing segment) are one point.
    verts = verts.filter((p, k) => !k || Math.hypot(p[0] - verts[k - 1][0], p[1] - verts[k - 1][1]) > 0.01);
    if (map(verts[0])[0] > map(verts[verts.length - 1])[0]) verts.reverse();
    if (verts.length > 96) { const src = verts; verts = Array.from({ length: 96 }, (_, k) => src[Math.round((k * (src.length - 1)) / 95)]); }
    const n = verts.length;
    if (n < 2) return false;
    // Higher on screen is a higher value, so a higher note.
    const ys = verts.map(p => map(p)[1]), top = Math.min(...ys), bottom = Math.max(...ys);
    const notes = ys.map(y => SCALE[bottom - top > 0.5 ? Math.round(((bottom - y) / (bottom - top)) * (SCALE.length - 1)) : 4]);
    // A short line plays more than once, the repeats as the remix: drums, then an octave up.
    const passes = n >= 30 ? 1 : n >= 12 ? 2 : 3;
    const passLen = n * STEP, total = passes * passLen;
    const r = S.runs.jukebox = newRun('jukebox');
    barry(`Now playing: ${titleOf(v) || 'Net Revenue by Month'} (Remix).`);
    const lead = 0.25;
    for (let p = 0; p < passes; p += 1) {
      notes.forEach((f, k) => {
        const at = lead + p * passLen + k * STEP;
        sound.note(r.bus, p === 2 ? f * 2 : f, at);
        if (p >= 1) {
          if (k % 2 === 0) sound.kick(r.bus, at, 0.2); else sound.hat(r.bus, at, 0.06);
          if (p === 2 && k % 4 === 2) sound.snare(r.bus, at, 0.12);
        }
      });
    }
    const glow = '#5ad1ff';
    const head = add(r, htmlEl('div', { 'aria-hidden': 'true', style: `position:fixed;left:0;top:0;width:3px;height:10px;z-index:2147483002;pointer-events:none;border-radius:2px;background:linear-gradient(${glow}00, ${glow}, ${glow}00);box-shadow:0 0 10px 3px ${glow}99, 0 0 22px 6px ${glow}55;will-change:transform` }));
    const dot = add(r, htmlEl('div', { 'aria-hidden': 'true', style: `position:fixed;left:0;top:0;width:14px;height:14px;margin:-7px 0 0 -7px;z-index:2147483002;pointer-events:none;box-sizing:border-box;border-radius:50%;background:#fff;border:3px solid ${glow};box-shadow:0 0 12px 4px ${glow}aa;will-change:translate,scale` }));
    const t0 = performance.now() + lead * 1000;
    let lastNote = -1;
    const SYMBOLS = ['\u266a', '\u266b', '\u266a', '\u2669'];
    const COLORS = ['#5ad1ff', '#f2c94c', '#2bd96b', '#ff7ab6', '#b84dff'];
    const frame = safe(now => {
      if (r.done) return;
      r.raf = requestAnimationFrame(frame);
      if (chaosGone(r, now)) { endRun(r, 'stopped'); return; }
      if (!path.isConnected) { endRun(r, 'stopped'); return; }
      const m2 = screenMap(path, dense);
      if (m2) map = m2;
      const el = Math.max(0, (now - t0) / 1000);
      if (el >= total + 0.35) { endRun(r, true); return; }
      const pass = Math.min(passes - 1, Math.floor(el / passLen)), within = Math.min(el - pass * passLen, passLen - 0.0001);
      const k = Math.min(n - 1, Math.floor(within / STEP)), frac = k < n - 1 ? (within - k * STEP) / STEP : 0;
      const a = map(verts[k]), b = map(verts[Math.min(k + 1, n - 1)]);
      // The dot rests on each point while its note sounds, then glides to the next.
      const ease = frac < 0.55 ? 0 : (frac - 0.55) / 0.45, e = ease * ease * (3 - 2 * ease);
      const x = a[0] + (b[0] - a[0]) * e, y = a[1] + (b[1] - a[1]) * e;
      const pr = path.getBoundingClientRect(), vr = v.getBoundingClientRect();
      const hTop = Math.max(vr.top, pr.top - 16), hBot = Math.min(vr.bottom, pr.bottom + 16);
      head.style.height = `${Math.max(30, hBot - hTop).toFixed(1)}px`;
      head.style.transform = `translate(${(x - 1.5).toFixed(1)}px, ${hTop.toFixed(1)}px)`;
      dot.style.translate = `${x.toFixed(1)}px ${y.toFixed(1)}px`;
      const idx = pass * n + k;
      if (idx !== lastNote && now >= t0) {
        lastNote = idx;
        anim(r, dot, [{ scale: '1.9' }, { scale: '1' }], { duration: 160, easing: 'ease-out' });
        if (idx % 2 === 0) floater(r, a[0] + (Math.random() - 0.5) * 16, a[1] - 18, SYMBOLS[idx % SYMBOLS.length], COLORS[idx % COLORS.length], 20, 40);
      }
    });
    const start = map(verts[0]);
    dot.style.translate = `${start[0].toFixed(1)}px ${start[1].toFixed(1)}px`;
    head.style.transform = `translate(${(start[0] - 1.5).toFixed(1)}px, ${start[1].toFixed(1)}px)`;
    r.raf = requestAnimationFrame(frame);
    later(r, () => endRun(r, true), (total + lead + 2) * 1000);
    return r.promise;
  };

  // ==== drum slices: a click on a pie or donut slice while Chaos Mode is on ====
  const SLICE_SEL = 'path[class*="s)PBT"
LR"PBT(lice"]';
  const isSlice = n => !!(n && n.matches && n.matches(SLICE_SEL) && containerOf(n) && !n.closest('[data-pbt]'));
  const sliceAt = e => {
    if (isSlice(e.target)) return e.target;
    try { for (const n of document.elementsFromPoint(e.clientX, e.clientY)) if (isSlice(n)) return n; } catch {}
    return null;
  };
  // A CSS transform scale about the slice's middle, by Web Animations, gone when it finishes.
  // A CSS transform replaces the SVG transform attribute while it runs, so the attribute's matrix
  // (read, never changed) goes first, as page/80-chaos.js does for its balloon bars.
  const pulse = p => {
    if (typeof p.animate !== 'function') return;
    let bb;
    try { bb = p.getBBox(); } catch { return; }
    if (!bb.width || !bb.height) return;
    let pre = '';
    try {
      const list = p.transform && p.transform.baseVal;
      if (list && list.numberOfItems) {
        let m = new DOMMatrix();
        for (let i = 0; i < list.numberOfItems; i += 1) { const t = list.getItem(i).matrix; m = m.multiply(DOMMatrix.fromMatrix({ a: t.a, b: t.b, c: t.c, d: t.d, e: t.e, f: t.f })); }
        pre = `matrix(${m.a}, ${m.b}, ${m.c}, ${m.d}, ${m.e}, ${m.f}) `;
      }
    } catch {}
    const cx = bb.x + bb.width / 2, cy = bb.y + bb.height / 2;
    const at = s => `${pre}translate(${cx}px, ${cy}px) scale(${s}) translate(${-cx}px, ${-cy}px)`;
    const old = S.pulses.get(p);
    if (old) { try { old.cancel(); } catch {} }
    const a = p.animate([{ transform: at(1) }, { transform: at(1.12), offset: 0.3 }, { transform: at(1) }], { duration: 300, easing: 'ease-out' });
    S.pulses.set(p, a);
    const forget = () => { if (S.pulses.get(p) === a) S.pulses.delete(p); };
    a.addEventListener('finish', forget);
    a.addEventListener('cancel', forget);
  };
  // Never prevents or stops the event: Power BI still selects and cross-filters as usual.
  const onPointer = e => {
    // Chaos Mode is checked first: finding the slice under the pointer costs about 0.1 ms a click,
    // which every click paid while Chaos Mode was off.
    if (e.button !== 0 || S.runs.jukebox || !chaosOn()) return;
    const p = sliceAt(e);
    if (!p) return;
    const v = containerOf(p), idx = Math.max(0, [...v.querySelectorAll(SLICE_SEL)].indexOf(p));
    const dest = audio() && S.master;
    if (dest) [sound.kick, sound.snare, sound.hat][idx % 3](dest);
    if (!Object.keys(S.runs).length) closeSoon();
    pulse(p);
  };

  // ==== the registry and the module ====
  const stopAll = () => Object.values(S.runs).forEach(r => endRun(r, 'stopped'));
  const coaster = { name: 'Rollercoaster.exe', run: coasterRun, stop() { endRun(S.runs.coaster, 'stopped'); return true; } };
  const jukebox = { name: 'Data Jukebox', run: jukeboxRun, stop() { endRun(S.runs.jukebox, 'stopped'); return true; } };
  pbt.chaosFx = pbt.chaosFx || {};
  pbt.chaosFx.coaster = coaster;
  pbt.chaosFx.jukebox = jukebox;

  const fxSound = {
    name: 'fxSound',
    start() {
      if (S.started) return 'already started';
      // Off with the "Hidden surprises" setting: no listener at all.
      if (!pbt.feature('surprises')) return 'off (Hidden surprises setting)';
      S.started = true;
      const h = safe(onPointer);
      document.addEventListener('pointerdown', h, { capture: true, passive: true });
      S.base.push(() => document.removeEventListener('pointerdown', h, { capture: true, passive: true }));
      return 'started';
    },
    stop() {
      stopAll();
      S.pulses.forEach(a => { try { a.cancel(); } catch {} }); S.pulses.clear();
      S.base.splice(0).forEach(fn => { try { fn(); } catch {} });
      S.started = false;
      closeAudio();
    },
    // Takes this copy out completely, so a fresh evaluation of the file can replace it.
    remove() {
      this.stop();
      const i = pbt.modules ? pbt.modules.indexOf(this) : -1;
      if (i >= 0) pbt.modules.splice(i, 1);
      if (pbt.chaosFx && pbt.chaosFx.coaster === coaster) delete pbt.chaosFx.coaster;
      if (pbt.chaosFx && pbt.chaosFx.jukebox === jukebox) delete pbt.chaosFx.jukebox;
      if (pbt.fxSound === this) delete pbt.fxSound;
    },
    // For tests: what is running and what the line finder sees on this page.
    state() {
      const line = (() => { try { return findLine(); } catch { return null; } })();
      return {
        started: S.started, running: Object.keys(S.runs), chaos: chaosOn(), pulses: S.pulses.size,
        line: line ? { type: typeOf(line.v), cls: cls(line.path), title: titleOf(line.v), vertices: vertices(line.path.getAttribute('d')).length } : null,
      };
    },
  };
  pbt.fxSound = fxSound;
  if (typeof pbt.module === 'function') pbt.module(fxSound);
  // Injected after the core booted (Chaos Mode's module already started): start now.
  try { if (pbt.chaos && typeof pbt.chaos.state === 'function' && pbt.chaos.state().started) fxSound.start(); } catch {}
}

// ---- 83-fx-space.js ----
// Chaos Mode effects, part of the "Hidden surprises" setting with page/80-chaos.js:
// Black Hole.scr, Shatter.exe and Shell Game. Each acts on the real visual containers on the
// report canvas. A visual is only ever moved by a Web Animation (an additive transform, so Power
// BI's own transform stays underneath) or faded by one, and every animation, overlay and clone is
// cancelled or removed when the effect ends, so the page is exactly as it was. Every sound is
// synthesized with WebAudio on one AudioContext of this file's own, closed a few seconds after
// the last effect ends and at stop().
//
// Contract, so the lead can inject this file on its own into a page where the mod already runs:
// - It needs only `pbt` (window.__pbt) in scope. Everything else sits inside one block, so its
//   names never meet another page file's. Evaluating the file again replaces the previous copy.
// - It registers as a module (pbt.fxSpace, then pbt.module): start() (also called when the file
//   is evaluated, and safe to call twice) watches which visual was cl)PBT"
LR"PBT(icked last; stop() ends any
//   running effect, removes the listener and closes the audio; pbt.remove() calls stop().
// - Each effect sits in the shared registry, pbt.chaosFx.<id> = { name, run(), stop() }: run()
//   returns a Promise that resolves when the effect is over (true, or false when it was stopped)
//   or false at once when it cannot run on this page (another effect of this file running, or
//   no visual it needs; Barry says so when Chaos Mode is on). Test calls:
//   window.__pbt.chaosFx.blackhole.run(), .shatter.run(), .shellgame.run(); the shell game also
//   takes .shellgame.guess(index) (the index of a card in .shellgame.state().cards) to finish
//   without a real click.
// - Barry (pbt.chaos.say) comments when Chaos Mode is on; the shell game's question and verdict
//   fall back to a pbt.toast when it is off, since the game needs them.
{
  if (pbt.fxSpace && typeof pbt.fxSpace.remove === 'function') { try { pbt.fxSpace.remove(); } catch {} }

  const F = { started: false, base: [], cur: null, last: null, ctx: null, master: null, noiseBuf: null, closeT: null };
  const log = e => { try { pbt.log && pbt.log(`fx-space: ${(e && e.stack) || e}`); } catch {} };
  const safe = fn => function (...args) { try { return fn.apply(this, args); } catch (e) { log(e); return undefined; } };
  const r2 = n => Math.round(n * 100) / 100;
  const r3 = n => Math.round(n * 1000) / 1000;
  const rand = (a, b) => a + Math.random() * (b - a);
  const clamp = (v, a, b) => Math.max(a, Math.min(b, v));

  // == one running effect at a time: its timers, animations, our nodes and its sounds ==
  const begin = id => {
    if (F.cur) return null;
    if (!F.started) fxSpace.start();
    const run = { id, anims: new Set(), els: new Set(), timers: new Set(), nodes: new Set(), offs: [], ended: false, data: {} };
    run.done = new Promise(res => { run.resolve = res; });
    F.cur = run;
    cancelClose();
    return run;
  };
  // Everything the effect made goes: timers, listeners, every Web Animation (so Power BI's
  // elements are back exactly as they were) and our nodes. A stopped effect also cuts its sounds.
  const end = (run, result = true) => {
    if (!run || run.ended) return;
    run.ended = true;
    run.timers.forEach(t => clearTimeout(t)); run.timers.clear();
    run.offs.splice(0).forEach(fn => { try { fn(); } catch {} });
    run.anims.forEach(a => { try { a.cancel(); } catch {} }); run.anims.clear();
    run.els.forEach(el => el.remove()); run.els.clear();
    if (result !== true) run.nodes.forEach(n => { try { n.stop(); } catch {} });
    run.nodes.clear();
    if (F.cur === run) F.cur = null;
    closeSoon();
    run.resolve(result);
  };
  // A pause inside an effect. Once the effect has ended it never resolves, so the rest of a
  // stopped effect's script simply never runs.
  const wait = (run, ms) => new Promise(res => {
    if (run.ended) return;
    const t = setTimeout(() => { run.timers.delete(t); if (!run.ended) res(); }, ms);
    run.timers.add(t);
  });
  const at = (run, ms, fn) => {
    const t = setTimeout(() => { run.timers.delete(t); if (!run.ended) safe(fn)(); }, ms);
    run.timers.add(t);
    return t;
  };
  const ani = (run, el, frames, opts) => {
    if (!el || typeof el.animate !== 'function' || run.ended) return null;
    try { const a = el.animate(frames, opts); run.anims.add(a); return a; } catch (e) { log(e); return null; }
  };
  const drop = (run, a) => { if (a) { run.anims.delete(a); try { a.cancel(); } catch {} } };
  const own = (run, el) => { el.setAttribute('data-pbt', ''); run.els.add(el); return el; };
  // Runs an effect's script; a failure ends it cleanly.
  const go = (run, script) => {
    Promise.resolve().then(() => script()).catch(e => { log(e); end(run, false); });
    return run.done;
  };

  // == Barry, when Chaos Mode is on ==
  const barry = text => { try { const c = pbt.chaos; return !!(c && typeof c.say === 'function' && c.say(text) === true); } catch { return false; } };
  const speak = (text, ms) => {
    if (barry(text)) return true;
    try { if (typeof pbt.toast === 'function') { pbt.toast(text, { title: 'Barry', ms }); return true; } } catch {}
    return false;
  };

  // == sounds: oscillators and filtered noise, no audio files ==
  const audio = () => {
    // Silent unless Skin sounds is on (off by default), like every other sound of the mod.
    if (!pbt.feature('sounds')) return null;
    try {
      if (!F.ctx || F.ctx.state === 'closed') {
        F.ctx = new AudioContext();
        F.master = F.ctx.createGain();
        F.master.gain.value = 0.7;
        F.master.connect(F.ctx.destination);
        F.noiseBuf = null;
      }
      if (F.ctx.state === 'suspended') F.ctx.resume().catch(() => {});
      return F.ctx;
    } catch { return null; }
  };
  const cancelClose = () => { if (F.closeT) { clearTimeout(F.closeT); F.closeT = null; } };
  const closeAudio = () => {
    cancelClose();
    const c = F.ctx;
    F.ctx = null; F.master = null; F.noiseBuf = null;
    if (c) { try { c.close().catch(() => {}); } catch {} }
  };
  // The context closes 3 s after the last effect, once its sound tails have played.
  const closeSoon = () => { cancelClose(); F.closeT = setTimeout(() => { F.closeT = null; if (!F.cur) closeAudio(); }, 3000); };
  const keep = n => { if (F.cur) F.cur.nodes.add(n); return n; };
  const noiseBuf = ctx => {
    if (!F.noiseBuf) {
      const len = ctx.sampleRate * 2, b = ctx.createBuffer(1, len, ctx.sampleRate), d = b.getChannelData(0);
      for (let i = 0; i < len; i += 1) d[i] = Math.random() * 2 - 1;
      F.noiseBuf = b;
    }
    return F.noiseBuf;
  };
  const tone = ({ type = 'sine', f = 440, f2, at: when = 0, len = 0.1, vol = 0.05, attack = 0.008 }) => {
    const ctx = audio();
    if (!ctx) return;
    try {
      const o = ctx.createOscillator(), g = ctx.createGain(), t = ctx.currentTime + when;
      o.type = type;
      o.frequency.setValueAtTime(f, t);
      if (f2) o.frequency.expone)PBT"
LR"PBT(ntialRampToValueAtTime(f2, t + len);
      g.gain.setValueAtTime(0.0001, t);
      g.gain.exponentialRampToValueAtTime(vol, t + Math.min(attack, len * 0.5));
      g.gain.exponentialRampToValueAtTime(0.0001, t + len);
      o.connect(g).connect(F.master);
      o.start(t); o.stop(t + len + 0.05);
      keep(o);
    } catch {}
  };
  // Filtered white noise with an attack and a decay; f2 sweeps the filter.
  const noise = ({ at: when = 0, len = 0.1, vol = 0.1, type = 'bandpass', f = 2000, f2, q = 1, attack = 0.004 }) => {
    const ctx = audio();
    if (!ctx) return;
    try {
      const t = ctx.currentTime + when, n = ctx.createBufferSource(), flt = ctx.createBiquadFilter(), g = ctx.createGain();
      n.buffer = noiseBuf(ctx); n.loop = true;
      flt.type = type; flt.Q.value = q;
      flt.frequency.setValueAtTime(f, t);
      if (f2) flt.frequency.exponentialRampToValueAtTime(f2, t + len);
      g.gain.setValueAtTime(0.0001, t);
      g.gain.exponentialRampToValueAtTime(vol, t + Math.min(attack, len * 0.9));
      g.gain.exponentialRampToValueAtTime(0.0001, t + len);
      n.connect(flt).connect(g).connect(F.master);
      n.start(t, Math.random() * 1.5); n.stop(t + len + 0.02);
      keep(n);
    } catch {}
  };
  // A deep rumble that rises in pitch for len seconds: detuned saws and noise under a lowpass.
  const rumble = len => {
    const ctx = audio();
    if (!ctx) return;
    try {
      len = Math.max(2, len);
      const t = ctx.currentTime, g = ctx.createGain(), lp = ctx.createBiquadFilter();
      lp.type = 'lowpass'; lp.Q.value = 6;
      lp.frequency.setValueAtTime(110, t); lp.frequency.exponentialRampToValueAtTime(480, t + len);
      g.gain.setValueAtTime(0.0001, t);
      g.gain.exponentialRampToValueAtTime(0.22, t + 1.2);
      g.gain.setValueAtTime(0.22, t + len - 0.4);
      g.gain.exponentialRampToValueAtTime(0.0001, t + len);
      lp.connect(g).connect(F.master);
      [[29, 72, 'sawtooth'], [30.5, 75, 'sawtooth'], [58, 146, 'triangle']].forEach(([a, b, type]) => {
        const o = ctx.createOscillator();
        o.type = type;
        o.frequency.setValueAtTime(a, t); o.frequency.exponentialRampToValueAtTime(b, t + len);
        o.connect(lp); o.start(t); o.stop(t + len + 0.05);
        keep(o);
      });
      const n = ctx.createBufferSource(), ng = ctx.createGain();
      n.buffer = noiseBuf(ctx); n.loop = true; ng.gain.value = 0.6;
      n.connect(ng).connect(lp); n.start(t); n.stop(t + len + 0.05);
      keep(n);
    } catch {}
  };
  // A burp: a falling saw with a fast wobble, through two vowel-ish bandpass filters.
  const burp = () => {
    const ctx = audio();
    if (!ctx) return;
    try {
      const t = ctx.currentTime, o = ctx.createOscillator(), lfo = ctx.createOscillator(), depth = ctx.createGain();
      const g = ctx.createGain(), f1 = ctx.createBiquadFilter(), f2 = ctx.createBiquadFilter(), mix = ctx.createGain();
      o.type = 'sawtooth';
      o.frequency.setValueAtTime(96, t); o.frequency.linearRampToValueAtTime(70, t + 0.75);
      lfo.frequency.value = 23; depth.gain.value = 16;
      lfo.connect(depth).connect(o.frequency);
      f1.type = 'bandpass'; f1.frequency.value = 520; f1.Q.value = 3;
      f2.type = 'bandpass'; f2.frequency.value = 1150; f2.Q.value = 5;
      o.connect(f1).connect(mix); o.connect(f2).connect(mix);
      g.gain.setValueAtTime(0.0001, t);
      g.gain.exponentialRampToValueAtTime(0.8, t + 0.04);
      g.gain.setValueAtTime(0.65, t + 0.5);
      g.gain.exponentialRampToValueAtTime(0.0001, t + 0.8);
      mix.connect(g).connect(F.master);
      o.start(t); lfo.start(t); o.stop(t + 0.85); lfo.stop(t + 0.85);
      keep(o); keep(lfo);
    } catch {}
  };
  const sfx = {
    open() { tone({ f: 95, f2: 26, len: 1.4, vol: 0.22, attack: 0.05 }); noise({ len: 1.3, vol: 0.12, type: 'lowpass', f: 300, f2: 80, attack: 0.3 }); },
    slurp() {
      tone({ f: 1300, f2: 170, len: 0.24, vol: 0.05 });
      noise({ len: 0.22, vol: 0.12, type: 'bandpass', f: 2600, f2: 380, q: 4 });
      tone({ f: 150, f2: 55, at: 0.17, len: 0.16, vol: 0.12 });
    },
    spit() { tone({ type: 'triangle', f: 200, f2: 900, len: 0.1, vol: 0.06 }); noise({ len: 0.08, vol: 0.1, type: 'highpass', f: 1600 }); },
    close() { tone({ f: 420, f2: 38, len: 0.55, vol: 0.12 }); noise({ len: 0.4, vol: 0.06, type: 'bandpass', f: 1800, f2: 200, q: 2 }); },
    crack() { noise({ len: 0.05, vol: 0.3, type: 'highpass', f: 3000 }); tone({ type: 'square', f: 2200, f2: 700, len: 0.035, vol: 0.03 }); },
    // Breaking glass: a bright noise burst, a thud, and a shower of tinkles that thins out.
    crash() {
      noise({ len: 0.6, vol: 0.35, type: 'highpass', f: 1800 });
      noise({ len: 1, vol: 0.16, type: 'bandpass', f: 6000, q: 1.5 });
      tone({ f: 140, f2: 45, len: 0.22, vol: 0.22 });
      for (let i = 0; i < 16; i += 1) tone({ type: 'triangle', f: rand(2600, 7200), at: 0.02 + Math.random() ** 2 * 0.8, len: rand(0.05, 0.18), vol: rand(0.015, 0.04), attack: 0.003 });
    },
    // The crash played backwards, roughly: a swell of noise and rising tinkles.
    rewind(len) {
      noise({ len, vol: 0.14, type: 'bandpass', f: 700, f2: 5200, q: 2, attack: len * 0.92 });
      for (let i = 0; i < 10; i += 1) tone({ type: 'triangle', f: 2400 + i * 420, at: len * (0.3 + i * 0.065), len: 0.07, vol: 0.02, attack: 0.003 });
    },
    clink() { tone({ type: 'triangle', f: 2093, len: 0.3, vol: 0.06, attack: 0.003 }); tone({ type: 'triangle', f: 3136, at: 0.04, len: 0.45, vol: 0.05, attack: 0.003 }); },
    whoosh(len) { noise({ len, vol: 0.18, type: 'bandpass', f: 350, f2: 2200, q: 1.4, attack: len * 0.55 }); },
    hop() { tone({ f: 480, f2: 960, len: 0.12, vol: 0.06 }); tone({ f: 640, f2: 1280, at: 0.14, len: 0.12, vol: 0.06 }); },
    ding() {
      tone({ type: 'triangle', f: 1568, len: 0.8, vol: 0.08, attack: 0.003 });
      tone({ type: 'triangle', f: 2093, at: 0.12, len: 0.9, vol: 0.07, attack: 0.003 });
      tone({ f: 4186, at: 0.12, len: 0.5, vol: 0.02 });)PBT"
LR"PBT(
    },
    buzzer() { tone({ type: 'sawtooth', f: 98, len: 0.75, vol: 0.07, attack: 0.01 }); tone({ type: 'square', f: 104, len: 0.75, vol: 0.04, attack: 0.01 }); },
  };

  // == the visuals on the canvas ==
  // Top-level containers only (a group's members move with the group), never our own clones.
  const containers = () => [...document.querySelectorAll('.displayArea .visualContainer')].filter(el => !el.closest('[data-pbt]') && !(el.parentElement && el.parentElement.closest('.visualContainer')));
  const topOf = el => {
    let c = el;
    for (let p = c.parentElement && c.parentElement.closest('.visualContainer'); p; p = c.parentElement && c.parentElement.closest('.visualContainer')) c = p;
    return c;
  };
  const typeOf = v => (((v.querySelector('[class*="visual-"]') || { getAttribute: () => '' }).getAttribute('class') || '').match(/\bvisual-(\w+)/) || [])[1] || '';
  const kindOf = v => `${v.getAttribute('aria-roledescription') || ''} ${typeOf(v)}`;
  // A container's geometry: its screen box, its own size, the screen pixels per container pixel
  // (the page is scaled to the zoom) and its centre measured from its transform origin, so a
  // rotation or scale of ours turns it about its own middle.
  const box = el => {
    if (!el || typeof el.animate !== 'function') return null;
    const r = el.getBoundingClientRect(), w = el.offsetWidth, h = el.offsetHeight;
    if (!(r.width > 4 && r.height > 4 && w > 0 && h > 0)) return null;
    const o = getComputedStyle(el).transformOrigin.split(' ').map(parseFloat);
    const ox = o[0] || 0, oy = o[1] || 0;
    return { el, r, w, h, k: r.width / w, ox, oy, cx: r.left + r.width / 2, cy: r.top + r.height / 2, c: { x: w / 2 - ox, y: h / 2 - oy } };
  };
  const onScreen = b => b.r.bottom > 0 && b.r.top < innerHeight && b.r.right > 0 && b.r.left < innerWidth;
  // The transform each effect adds on top of Power BI's own (composite: 'add'): a move in the
  // container's own pixels, then a turn and a scale about the point c (from the transform origin).
  // Every keyframe has the same list of functions, so they interpolate one by one.
  const X = (c, dx, dy, deg = 0, s = 1) => `translate(${r2(dx)}px, ${r2(dy)}px) translate(${r2(c.x)}px, ${r2(c.y)}px) rotate(${r2(deg)}deg) scale(${r3(s)}) translate(${r2(-c.x)}px, ${r2(-c.y)}px)`;

  // == Black Hole.scr ==
  const blackhole = () => {
    const run = begin('blackhole');
    if (!run) return false;
    const da = document.querySelector('.displayArea');
    const items = containers().map(box).filter(b => b && onScreen(b));
    if (!da || !items.length) {
      barry('No visuals here. The black hole goes hungry.');
      end(run, false);
      return false;
    }
    // The hole opens in the middle of the part of the canvas that is on screen.
    const dr = da.getBoundingClientRect();
    const L = Math.max(dr.left, 0), T = Math.max(dr.top, 0), RT = Math.min(dr.right, innerWidth), BT = Math.min(dr.bottom, innerHeight);
    const hx = (L + RT) / 2, hy = (T + BT) / 2;
    const R = clamp(Math.min(RT - L, BT - T) * 0.1, 36, 120), S = R * 6;
    const layer = (inset, css) => {
      const d = document.createElement('div');
      d.style.cssText = `position:absolute;inset:${inset}%;border-radius:50%;pointer-events:none;${css}`;
      return d;
    };
    const ringMask = (a, b, c, d) => {
      const g = `radial-gradient(circle closest-side,transparent ${a}%,#000 ${b}%,#000 ${c}%,transparent ${d}%)`;
      return `-webkit-mask-image:${g};mask-image:${g};`;
    };
    // Layers, outside in: a lens ring that bends the report behind it (a backdrop filter), the glowing
    // accretion ring and its brighter streaks (both spinning), and the event horizon.
    const wrap = own(run, document.createElement('div'));
    wrap.style.cssText = `position:fixed;left:${r2(hx - S / 2)}px;top:${r2(hy - S / 2)}px;width:${r2(S)}px;height:${r2(S)}px;z-index:2147482900;pointer-events:none;transform:scale(0.001)`;
    const pulse = layer(0, '');
    const lens = layer(0, `backdrop-filter:blur(2.5px) saturate(1.7) hue-rotate(-30deg) contrast(1.15);-webkit-backdrop-filter:blur(2.5px) saturate(1.7) hue-rotate(-30deg) contrast(1.15);background:radial-gradient(circle closest-side,rgba(110,50,190,0) 30%,rgba(110,50,190,.16) 52%,rgba(110,50,190,0) 100%);${ringMask(28, 40, 60, 100)}`);
    const ring = layer(20, `background:conic-gradient(from 0deg,#ffb347,#ff4f7b,#8a3dff,#3fa9ff,#ffd36b,#ff6a2b,#ffb347);filter:blur(3px);opacity:.95;${ringMask(46, 56, 70, 100)}`);
    const streaks = layer(14, `background:repeating-conic-gradient(from 0deg,rgba(255,236,200,0) 0deg 9deg,rgba(255,236,200,.75) 11deg 13deg,rgba(255,236,200,0) 15deg 24deg);filter:blur(1.2px);${ringMask(44, 54, 66, 92)}`);
    const disc = layer(33, `background:radial-gradient(circle closest-side,#000 0 80%,#120600 90%,rgba(0,0,0,0) 100%);box-shadow:0 0 ${r2(R * 0.4)}px ${r2(R * 0.08)}px rgba(255,150,60,.7),inset 0 0 ${r2(R * 0.3)}px rgba(255,120,40,.35)`);
    pulse.append(lens, ring, streaks, disc);
    wrap.append(pulse);
    document.body.append(wrap);

    // Each visual's spiral into the hole: two and a quarter turns, the radius shrinking faster
    // near the end, turning and shrinking to almost nothing. The nearest go first. The spiral is
    // an ellipse shaped like the visible canvas, so a visual swinging round strays past the
    // canvas edges as little as possible (a wide page would otherwise send them off the top).
    items.sort((a, b) => Math.hypot(a.cx - hx, a.cy - hy) - Math.hypot(b.cx - hx, b.cy - hy));
    const n = items.length, gap = clamp(2600 / n, 140, 380), IN = 1900, OUT = 1300, N = 36, TURNS = 2.25;
    const dir = Math.random() < 0.5 ? 1 : -1, inEase = 'cubic-bezier(.5, 0, .85, .45)';
    const halfW = Math.max(1, (RT - L) / 2), halfH = Math.max(1, (BT - T) / 2);
    items.forEach(b => {
      const ex = (b.cx - hx) / halfW, ey = (b.cy - hy) / halfH, s0 = Math.hypot(ex, ey), th0 = Math.atan2(ey, ex);
      b.frames = [];
      for (let j )PBT"
LR"PBT(= 0; j <= N; j += 1) {
        const t = j / N, rho = s0 * (1 - t) ** 1.5, th = th0 + dir * TURNS * 2 * Math.PI * t;
        const sc = Math.max(0.02, (1 - t) ** 1.35);
        let px = hx + rho * halfW * Math.cos(th), py = hy + rho * halfH * Math.sin(th);
        // The turning box reaches out to its half diagonal once it has turned a little; it is
        // kept inside the visible canvas (or no further out than where it started), so it never
        // stretches the report's scroll area.
        const grow = Math.min(1, t * 6), diag = Math.hypot(b.r.width, b.r.height) / 2;
        const ax = sc * (b.r.width / 2 + (diag - b.r.width / 2) * grow), ay = sc * (b.r.height / 2 + (diag - b.r.height / 2) * grow);
        const fit = (v, lo, hi, v0) => (lo <= hi ? clamp(v, Math.min(lo, v0), Math.max(hi, v0)) : v);
        px = fit(px, L + ax, RT - ax, b.cx); py = fit(py, T + ay, BT - ay, b.cy);
        b.frames.push({ transform: X(b.c, (px - b.cx) / b.k, (py - b.cy) / b.k, dir * 600 * t ** 1.3, sc) });
      }
    });
    const gulp = (s, ms) => ani(run, pulse, [{ scale: 1 }, { scale: s }, { scale: 1 }], { duration: ms, easing: 'ease-out' });

    return go(run, async () => {
      sfx.open();
      ani(run, ring, [{ rotate: '0deg' }, { rotate: '360deg' }], { duration: 1500, iterations: Infinity });
      ani(run, streaks, [{ rotate: '0deg' }, { rotate: '360deg' }], { duration: 950, iterations: Infinity });
      ani(run, wrap, [{ transform: 'scale(0.001) rotate(-120deg)' }, { transform: 'scale(1) rotate(0deg)' }], { duration: 1400, easing: 'cubic-bezier(.2, .8, .2, 1)', fill: 'forwards' });
      await wait(run, 1300);

      const inEnd = (n - 1) * gap + IN;
      rumble((inEnd + 1200) / 1000);
      items.forEach((b, i) => at(run, i * gap, () => {
        b.a = ani(run, b.el, b.frames, { duration: IN, easing: inEase, fill: 'forwards', composite: 'add' });
        b.fade = ani(run, b.el, [{ opacity: 1 }, { opacity: 1, offset: 0.82 }, { opacity: 0 }], { duration: IN, easing: inEase, fill: 'forwards' });
        at(run, IN * 0.92, () => { sfx.slurp(); gulp(1.1, 260); });
      }));
      await wait(run, inEnd + 250);

      // Full. It pulses, burps, and spits them back out, the last one in first.
      ani(run, pulse, [{ scale: 1 }, { scale: 1.28 }, { scale: 0.88 }, { scale: 1.2 }, { scale: 0.94 }, { scale: 1.08 }, { scale: 1 }], { duration: 1500, easing: 'ease-in-out' });
      await wait(run, 900);
      burp();
      await wait(run, 800);
      barry("I'm not saying it's a bad report, but even the black hole spat it out.");
      const back = items.slice().reverse(), g2 = clamp(gap * 0.7, 110, 260);
      back.forEach((b, i) => at(run, i * g2, () => {
        // The way out is the way in, backwards. The new animation starts where the old one
        // holds, so cancelling the old one in the same task shows no jump.
        b.a2 = ani(run, b.el, b.frames.slice().reverse(), { duration: OUT, easing: 'cubic-bezier(.15, .55, .35, 1)', fill: 'forwards', composite: 'add' });
        drop(run, b.a); drop(run, b.fade);
        sfx.spit();
        gulp(0.9, 200);
      }));
      await wait(run, (back.length - 1) * g2 + OUT + 200);

      sfx.close();
      ani(run, wrap, [{ transform: 'scale(1) rotate(0deg)' }, { transform: 'scale(0.001) rotate(200deg)' }], { duration: 650, easing: 'cubic-bezier(.6, 0, .9, .4)', fill: 'forwards' });
      await wait(run, 700);
      end(run, true);
    });
  };

  // == Shatter.exe ==
  const NOT_CHART = /\b(card|slicer|textbox|image|shape|basicShape|actionButton|button|pageNavigator|bookmarkNavigator)/i;
  // The visual clicked last (if it is still on the page and has not moved), else the largest chart.
  // Custom visuals live in iframes, which a clone would reload, so they are skipped.
  // Shatter clones the visual once per shard (6 to 18 times) in one task, so a visual larger than
  // this many elements (a long table or matrix) is never picked: cloning it would freeze Power BI.
  const SHATTER_MAX_NODES = 5000;
  const pickShatter = () => {
    const usable = el => (el && el.isConnected && !el.querySelector('iframe') && el.getElementsByTagName('*').length <= SHATTER_MAX_NODES ? box(el) : null);
    const last = F.last;
    if (last) {
      const b = usable(last.el);
      if (b && onScreen(b) && last.x >= b.r.left && last.x <= b.r.right && last.y >= b.r.top && last.y <= b.r.bottom) return { b, pt: [last.x, last.y] };
    }
    const all = containers().map(usable).filter(b => b && onScreen(b));
    const charts = all.filter(b => !NOT_CHART.test(kindOf(b.el)) && b.el.querySelector('svg path, svg rect'));
    const list = (charts.length ? charts : all).sort((a, b) => b.r.width * b.r.height - a.r.width * a.r.height);
    return list[0] ? { b: list[0], pt: null } : null;
  };
  // Triangles on a jittered grid that covers the container exactly: 12 to 18 shards, shaped to
  // the container. Points on the border slide only along it. Each triangle grows by 0.75 px from
  // its middle so the assembled shards show no hairline seams.
  const cutShards = (w, h, few) => {
    const a = w / h;
    const [cols, rows] = few ? (a >= 1 ? [3, 2] : [2, 3]) : a > 1.6 ? [4, 2] : a < 0.62 ? [2, 4] : [3, 3];
    const pts = [];
    for (let j = 0; j <= rows; j += 1) {
      const row = [];
      for (let i = 0; i <= cols; i += 1) {
        let x = (i / cols) * w, y = (j / rows) * h;
        if (i > 0 && i < cols) x += rand(-0.32, 0.32) * (w / cols);
        if (j > 0 && j < rows) y += rand(-0.32, 0.32) * (h / rows);
        row.push([x, y]);
      }
      pts.push(row);
    }
    const tris = [];
    for (let j = 0; j < rows; j += 1) {
      for (let i = 0; i < cols; i += 1) {
        const p00 = pts[j][i], p10 = pts[j][i + 1], p01 = pts[j + 1][i], p11 = pts[j + 1][i + 1];
        if (Math.random() < 0.5) tris.push([p00, p10, p11], [p00, p11, p01]);
        else tris.push([p00, p10, p01], [p10, p11, p01]);
      }
    }
    return tris.map(t => {
      const gx = (t[0][0] + t[1][0)PBT"
LR"PBT(] + t[2][0]) / 3, gy = (t[0][1] + t[1][1] + t[2][1]) / 3;
      const grown = t.map(([x, y]) => { const d = Math.hypot(x - gx, y - gy) || 1; return [x + ((x - gx) / d) * 0.75, y + ((y - gy) / d) * 0.75]; });
      return { pts: grown, edges: t, gx, gy };
    });
  };
  // Canvases come out of cloneNode blank; copy what they show (a WebGL one may stay blank).
  const copyCanvases = (from, to) => {
    const a = from.querySelectorAll('canvas'), b = to.querySelectorAll('canvas');
    a.forEach((c, i) => { try { if (b[i] && c.width && c.height) b[i].getContext('2d').drawImage(c, 0, 0); } catch {} });
  };
  const shatter = () => {
    const run = begin('shatter');
    if (!run) return false;
    const pick = pickShatter();
    if (!pick) {
      barry('Nothing here to break. I am almost disappointed.');
      end(run, false);
      return false;
    }
    const { b } = pick;
    const few = b.el.getElementsByTagName('*').length > 3000;
    const shards = cutShards(b.w, b.h, few);
    const ix = pick.pt ? clamp((pick.pt[0] - b.r.left) / b.k, 0, b.w) : b.w * rand(0.35, 0.65);
    const iy = pick.pt ? clamp((pick.pt[1] - b.r.top) / b.k, 0, b.h) : b.h * rand(0.35, 0.65);

    // The visible part of the canvas: the shards fly inside it and are clipped to it.
    const da = document.querySelector('.displayArea'), dr = da ? da.getBoundingClientRect() : { left: 0, top: 0, right: innerWidth, bottom: innerHeight };
    const wallL = Math.max(dr.left, 0), wallT = Math.max(dr.top, 0), wallR = Math.min(dr.right, innerWidth), floor = Math.min(dr.bottom, innerHeight);
    // Moves an absolutely placed element of ours until its top left corner is at (x, y) on screen.
    const nudge = (el, x, y) => {
      let left = 0, top = 0;
      for (let k = 0; k < 3; k += 1) {
        const cr = el.getBoundingClientRect(), dx = (x - cr.left) / b.k, dy = (y - cr.top) / b.k;
        if (Math.abs(dx) < 0.3 && Math.abs(dy) < 0.3) break;
        left += dx; top += dy;
        el.style.left = `${r2(left)}px`; el.style.top = `${r2(top)}px`;
      }
      return [left, top];
    };
    // A clipping layer over the visible canvas, in the visual's own parent so Power BI's styles
    // still apply to the clones inside it. A clip-path does not shrink a clone's layout box, so
    // without this layer a flying clone would stretch the report's scroll area.
    const stage = own(run, document.createElement('div'));
    stage.style.cssText = `position:absolute;left:0px;top:0px;width:${r2((wallR - wallL) / b.k)}px;height:${r2((floor - wallT) / b.k)}px;margin:0;padding:0;border:0;overflow:hidden;pointer-events:none;z-index:99999`;
    b.el.after(stage);
    nudge(stage, wallL, wallT);
    // One clone per shard, laid exactly over the visual. The first clone is measured and nudged
    // into place; the rest take its offsets.
    const proto = b.el.cloneNode(true);
    proto.setAttribute('aria-hidden', 'true');
    proto.setAttribute('inert', '');
    proto.removeAttribute('tabindex');
    Object.assign(proto.style, { position: 'absolute', left: '0px', top: '0px', margin: '0', width: `${b.w}px`, height: `${b.h}px`, boxSizing: 'border-box', pointerEvents: 'none' });
    const clones = shards.map((s, i) => {
      const c = i ? proto.cloneNode(true) : proto;
      c.setAttribute('data-pbt', '');
      copyCanvases(b.el, c);
      c.style.clipPath = `polygon(${s.pts.map(([x, y]) => `${r2(x)}px ${r2(y)}px`).join(', ')})`;
      return c;
    });
    stage.append(clones[0]);
    const [left, top] = nudge(clones[0], b.r.left, b.r.top);
    clones.slice(1).forEach(c => { c.style.left = `${r2(left)}px`; c.style.top = `${r2(top)}px`; });
    clones[0].after(...clones.slice(1));
    // The visual hides under its clones, and its wrapper is lifted so the shards fly over the
    // other visuals. Both are animations, cancelled at the end.
    ani(run, b.el, [{ opacity: 0 }, { opacity: 0 }], { duration: 1, fill: 'forwards' });
    const lift = b.el.parentElement;
    if (lift && lift !== document.querySelector('.displayArea') && lift.closest('.displayArea')) ani(run, lift, [{ zIndex: 99999 }, { zIndex: 99999 }], { duration: 1, fill: 'forwards' });

    // The cracks, drawn over the visual for a moment before it breaks.
    const SVG_NS = 'http://www.w3.org/2000/svg';
    const crack = own(run, document.createElementNS(SVG_NS, 'svg'));
    crack.setAttribute('viewBox', `0 0 ${r2(b.w)} ${r2(b.h)}`);
    crack.setAttribute('preserveAspectRatio', 'none');
    crack.style.cssText = `position:fixed;left:${r2(b.r.left)}px;top:${r2(b.r.top)}px;width:${r2(b.r.width)}px;height:${r2(b.r.height)}px;z-index:2147482900;pointer-events:none;overflow:visible`;
    const d = shards.map(s => `M${s.edges.map(([x, y]) => `${r2(x)} ${r2(y)}`).join('L')}Z`).join('') + shards.map(s => `M${r2(ix)} ${r2(iy)}L${r2(s.gx)} ${r2(s.gy)}`).join('');
    [['rgba(0,0,0,.45)', 3], ['rgba(255,255,255,.95)', 1.2]].forEach(([stroke, sw]) => {
      const p = document.createElementNS(SVG_NS, 'path');
      p.setAttribute('d', d); p.setAttribute('fill', 'none'); p.setAttribute('stroke', stroke);
      p.setAttribute('stroke-width', String(sw)); p.setAttribute('vector-effect', 'non-scaling-stroke'); p.setAttribute('stroke-linejoin', 'round');
      crack.append(p);
    });
    document.body.append(crack);

    return go(run, async () => {
      sfx.crack();
      barry("Don't worry, it was already broken.");
      clones.forEach(c => ani(run, c, [{ transform: 'translate(0px, 0px)' }, { transform: 'translate(3px, -2px)' }, { transform: 'translate(-3px, 2px)' }, { transform: 'translate(0px, 0px)' }], { duration: 160, iterations: 2, composite: 'add' }));
      await wait(run, 420);
      run.els.delete(crack); crack.remove();

      // Each shard flies away from the point of impact with a kick upward, spins, and falls
      // under gravity. Screen pixels per second become the container's own pixels. Shards stay
      // inside the visible canvas: they stop at its side walls and land on its )PBT"
LR"PBT(floor, so nothing
      // pokes out of the report's scroll area and gives it scroll bars for a moment.
      sfx.crash();
      const OUTMS = 1150, G = 1500, STEPS = 14;
      const flights = clones.map((c, i) => {
        const s = shards[i];
        let ux = s.gx - ix, uy = s.gy - iy;
        const dist = Math.hypot(ux, uy) || 1;
        ux /= dist; uy /= dist;
        const sp = rand(380, 820), vx = ux * sp + rand(-60, 60), vy = uy * sp - rand(250, 520);
        const spin = rand(200, 720) * (Math.random() < 0.5 ? -1 : 1), c0 = { x: s.gx - b.ox, y: s.gy - b.oy };
        // The shard's reach from its middle, on screen, and where its middle starts on screen.
        const reach = Math.max(...s.edges.map(([x, y]) => Math.hypot(x - s.gx, y - s.gy))) * b.k;
        const sx0 = b.r.left + s.gx * b.k, sy0 = b.r.top + s.gy * b.k;
        const lo = wallL + reach, hi = wallR - reach, low = floor - reach;
        const frames = [];
        let landed = null;
        for (let j = 0; j <= STEPS; j += 1) {
          const t = (j / STEPS) * (OUTMS / 1000);
          let x = sx0 + vx * t, y = sy0 + vy * t + 0.5 * G * t * t, deg = spin * t;
          if (j && y > low) { if (!landed) landed = { deg }; y = Math.max(sy0, low); }
          if (landed) deg = landed.deg;
          if (j) x = lo < hi ? clamp(x, Math.min(lo, sx0), Math.max(hi, sx0)) : sx0;
          frames.push({ transform: X(c0, (x - sx0) / b.k, (y - sy0) / b.k, deg, 1) });
        }
        return ani(run, c, frames, { duration: OUTMS, delay: rand(0, 60), fill: 'forwards', composite: 'add' });
      });
      await wait(run, OUTMS + 80 + 1400);

      // Rewind: every flight plays backwards, so each shard retraces its path to its exact place.
      sfx.rewind(1.15);
      flights.forEach(a => { try { if (a) a.reverse(); } catch {} });
      await wait(run, OUTMS + 160);
      sfx.clink();
      end(run, true);
    });
  };

  // == Shell Game ==
  const isCardKind = s => /^card/i.test((s || '').trim());
  // Up to three card visuals (Card or Card (new)), in reading order.
  const findCards = () => {
    let list = [];
    try {
      if (pbt.page && typeof pbt.page.visuals === 'function') {
        list = pbt.page.visuals().filter(v => v.el && (isCardKind(v.kind) || isCardKind(v.typeName)) && !(v.el.parentElement && v.el.parentElement.closest('.visualContainer'))).map(v => ({ el: v.el, title: v.title }));
      }
    } catch {}
    if (!list.length) list = containers().filter(el => isCardKind(el.getAttribute('aria-roledescription')) || isCardKind(typeOf(el))).map(el => ({ el, title: (el.getAttribute('aria-label') || '').trim() }));
    // A card without a title still names itself: the label under its number ("Net Revenue"), the
    // first text on it with letters and no digits.
    const labelOf = el => {
      for (const t of el.querySelectorAll('text, tspan, .caption, .label, [class*="label" i]')) {
        const s = (t.textContent || '').replace(/\s+/g, ' ').trim();
        if (s && /[A-Za-z]{3}/.test(s) && !/\d/.test(s) && t.children.length === 0) return s;
      }
      return '';
    };
    return list
      .map(c => { const b = c.el.querySelector('iframe') ? null : box(c.el); return b && onScreen(b) ? Object.assign(b, { title: c.title || labelOf(c.el) }) : null; })
      .filter(Boolean)
      .sort((a, b) => (Math.abs(a.r.top - b.r.top) > 8 ? a.r.top - b.r.top : a.r.left - b.r.left))
      .slice(0, 3);
  };
  const cleanTitle = t => { const s = String(t || '').replace(/\s+/g, ' ').trim(); return s.length > 40 ? `${s.slice(0, 38).trim()}...` : s; };
  const shellgame = () => {
    const run = begin('shellgame');
    if (!run) return false;
    const cards = findCards();
    if (cards.length < 2) {
      barry(cards.length ? 'One card? I need at least two to cheat properly.' : 'No cards on this page. Nothing to shuffle.');
      end(run, false);
      return false;
    }
    const n = cards.length, slots = cards.map(c => [c.cx, c.cy]), pos = cards.map((_, i) => i);
    const off = cards.map(() => [0, 0]), cur = cards.map(() => null);
    const target = Math.floor(Math.random() * n);
    const name = cleanTitle(cards[target].title) || 'the card';
    const G = run.data = { phase: 'show', cards, target, name, guess: null, answer: null };
    // A card glides from where it is to slot s along an arc (bend +1 or -1, so two swapping
    // cards pass on opposite sides), growing a little on the way like a lifted cup.
    const move = (i, s, ms, bend) => {
      const c = cards[i], from = off[i], to = [(slots[s][0] - c.cx) / c.k, (slots[s][1] - c.cy) / c.k];
      const mx = (from[0] + to[0]) / 2 - (to[1] - from[1]) * 0.28 * bend, my = (from[1] + to[1]) / 2 + (to[0] - from[0]) * 0.28 * bend;
      const a = ani(run, c.el, [{ transform: X(c.c, from[0], from[1]) }, { transform: X(c.c, mx, my, 0, 1.06) }, { transform: X(c.c, to[0], to[1]) }], { duration: ms, easing: 'ease-in-out', fill: 'forwards', composite: 'add' });
      drop(run, cur[i]); cur[i] = a; off[i] = to;
    };
    const hop = i => {
      const c = cards[i], u = 18 / c.k;
      ani(run, c.el, [0, -u, 0, -u * 0.55, 0].map(y => ({ transform: X(c.c, 0, y) })), { duration: 650, easing: 'ease-in-out', composite: 'add' });
    };
    const wiggle = i => {
      const c = cards[i];
      ani(run, c.el, [0, 9, -9, 7, -7, 4, -4, 0].map(d => ({ transform: X(c.c, 0, 0, d, 1.04) })), { duration: 900, easing: 'ease-in-out', composite: 'add' });
    };
    // Identical card backs over every card while they shuffle, so no title or value gives the
    // answer away. Each back sits inside its card's container (it moves with the card), takes no
    // pointer events (the guess is read from the card underneath) and goes with end().
    const backs = cards.map(c => {
      const d = own(run, document.createElement('div'));
      const set = { position: 'absolute', inset: '0', 'z-index': '2147483000', 'pointer-events': 'none', 'box-sizing': 'border-box',
        'border-radius': '8px', display:)PBT"
LR"PBT( 'grid', 'place-items': 'center', color: '#ffffff', font: '700 34px/1 "Segoe UI", system-ui, sans-serif',
        background: 'repeating-linear-gradient(45deg, #00587c 0 9px, #004766 9px 18px)',
        'box-shadow': 'inset 0 0 0 3px #ffffff, inset 0 0 0 5px #009999, 0 2px 6px rgba(0, 0, 0, .25)' };
      for (const [k, v] of Object.entries(set)) d.style.setProperty(k, v, 'important');
      d.style.opacity = '0';
      d.textContent = '?';
      c.el.append(d);
      return d;
    });
    const backAnims = backs.map(() => null);
    const flip = show => backs.forEach((d, i) => {
      const a = ani(run, d, show ? [{ opacity: 0, transform: 'scaleY(.15)' }, { opacity: 1, transform: 'none' }] : [{ opacity: 1, transform: 'none' }, { opacity: 0, transform: 'scaleY(.15)' }],
        { duration: 320, delay: i * 70, easing: 'ease-out', fill: 'forwards' });
      drop(run, backAnims[i]); backAnims[i] = a;
    });

    // The guess: only while Barry waits for it, only clicks on the cards, and the rest of that
    // click's events (up to 800 ms) are kept from Power BI too. Everything else passes.
    let swallowUntil = 0;
    G.answer = null;
    const pick = i => {
      if (G.phase !== 'guess' || !G.answer) return false;
      G.phase = 'result'; G.guess = i;
      swallowUntil = Date.now() + 800;
      G.answer(i);
      return true;
    };
    const block = e => {
      const t = e.target;
      const i = t && t.nodeType === 1 ? cards.findIndex(c => c.el.contains(t)) : -1;
      if (i < 0) return;
      if (G.phase !== 'guess' && Date.now() > swallowUntil) return;
      e.stopImmediatePropagation();
      e.preventDefault();
      if (G.phase === 'guess' && /^(pointerdown|mousedown|click)$/.test(e.type)) pick(i);
    };
    run.data.pick = pick;

    return go(run, async () => {
      sfx.hop();
      hop(target);
      barry(`Keep your eye on ${name}.`);
      await wait(run, 1700);

      // Face down: every card gets the same back before the shuffle.
      flip(true);
      sfx.whoosh(0.3);
      await wait(run, 320 + n * 70 + 250);

      G.phase = 'shuffle';
      const swaps = 6 + Math.floor(Math.random() * 3);
      for (let s = 0; s < swaps; s += 1) {
        const ms = Math.round(560 - (330 * s) / (swaps - 1));
        const a = Math.floor(Math.random() * n);
        let b = Math.floor(Math.random() * (n - 1));
        if (b >= a) b += 1;
        [pos[a], pos[b]] = [pos[b], pos[a]];
        move(a, pos[a], ms, 1);
        move(b, pos[b], ms, -1);
        sfx.whoosh(ms / 1000);
        await wait(run, ms + 50);
      }

      ['pointerdown', 'mousedown', 'pointerup', 'mouseup', 'click', 'dblclick', 'contextmenu'].forEach(type => {
        const h = safe(block);
        window.addEventListener(type, h, true);
        run.offs.push(() => window.removeEventListener(type, h, true));
      });
      const guessed = await new Promise(res => {
        G.answer = res;
        G.phase = 'guess';
        speak(`Where's ${name}?`, 6000);
        // Nobody guesses within 15 s: Barry shows them anyway.
        at(run, 15000, () => { if (G.phase === 'guess') { G.phase = 'result'; res(-1); } });
      });

      // Face up again, then the verdict.
      flip(false);
      await wait(run, 320 + n * 70);
      if (guessed === target) {
        sfx.ding();
        speak('Lucky guess.', 2500);
        hop(target);
      } else {
        sfx.buzzer();
        speak(guessed < 0 ? "Too slow. It's this one." : "Wrong. It's this one.", 2500);
        wiggle(target);
      }
      await wait(run, 2000);

      // Every card glides home; then end() cancels every animation.
      cards.forEach((c, i) => {
        const from = off[i];
        const a = ani(run, c.el, [{ transform: X(c.c, from[0], from[1]) }, { transform: X(c.c, 0, 0) }], { duration: 750, easing: 'cubic-bezier(.3, 0, .2, 1)', fill: 'forwards', composite: 'add' });
        drop(run, cur[i]); cur[i] = a; off[i] = [0, 0];
      });
      sfx.whoosh(0.7);
      await wait(run, 800);
      end(run, true);
    });
  };

  // == the registry the Begin menu and the backup keys launch from ==
  const runFx = fn => { try { return fn(); } catch (e) { log(e); if (F.cur) end(F.cur, false); return false; } };
  const stopFx = id => { if (F.cur && F.cur.id === id) { end(F.cur, false); return true; } return false; };
  const fx = {
    blackhole: { name: 'Black Hole.scr', run: () => runFx(blackhole), stop: () => stopFx('blackhole') },
    shatter: { name: 'Shatter.exe', run: () => runFx(shatter), stop: () => stopFx('shatter') },
    shellgame: {
      name: 'Shell Game',
      run: () => runFx(shellgame),
      stop: () => stopFx('shellgame'),
      // The lead's way to finish the game without a click: index into state().cards.
      guess(index) {
        const r = F.cur;
        if (!r || r.id !== 'shellgame' || !r.data.pick) return 'no shell game running';
        if (r.data.phase !== 'guess') return `not guessing yet (${r.data.phase})`;
        const i = Number(index);
        if (!(i >= 0 && i < r.data.cards.length)) return `index must be 0 to ${r.data.cards.length - 1}`;
        return r.data.pick(i);
      },
      state() {
        const r = F.cur;
        if (!r || r.id !== 'shellgame' || !r.data.cards) return { running: false };
        return { running: true, phase: r.data.phase, cards: r.data.cards.map(c => cleanTitle(c.title) || 'card'), answer: r.data.target, guess: r.data.guess };
      },
    },
  };

  const fxSpace = {
    name: 'fxSpace',
    effects: fx,
    // Remembers the top-level visual under the last pointer press, for Shatter.exe. It only
    // reads the event; nothing is stopped. Shatter is launched from the Begin menu, which exists
    // only with the Power BI 95 look on, so other clicks return at once and nothing is kept.
    start() {
      if (F.started) return true;
      // Off with the "Hidden surprises" setting: no listener at all.
      if (!pbt.feature('surprises')) return false;
      F.started = true;
      const h = saf)PBT"
LR"PBT(e(e => {
        if (!(pbt.retro95 && pbt.retro95.active)) { F.last = null; return; }
        const t = e.target;
        const el = t && t.closest ? t.closest('.displayArea .visualContainer') : null;
        if (!el || el.closest('[data-pbt]')) return;
        F.last = { el: topOf(el), x: e.clientX, y: e.clientY, t: Date.now() };
      });
      window.addEventListener('pointerdown', h, { capture: true, passive: true });
      F.base.push(() => window.removeEventListener('pointerdown', h, { capture: true, passive: true }));
      return true;
    },
    stop() {
      if (F.cur) end(F.cur, false);
      F.base.splice(0).forEach(fn => { try { fn(); } catch {} });
      F.started = false;
      F.last = null;
      closeAudio();
    },
    // Takes this copy out completely, so a fresh evaluation of the file can replace it.
    remove() {
      this.stop();
      const i = pbt.modules ? pbt.modules.indexOf(this) : -1;
      if (i >= 0) pbt.modules.splice(i, 1);
      if (pbt.chaosFx) Object.keys(fx).forEach(k => { if (pbt.chaosFx[k] === fx[k]) delete pbt.chaosFx[k]; });
      if (pbt.fxSpace === this) delete pbt.fxSpace;
    },
  };
  pbt.chaosFx = pbt.chaosFx || {};
  Object.assign(pbt.chaosFx, fx);
  pbt.fxSpace = fxSpace;
  if (typeof pbt.module === 'function') pbt.module(fxSpace);
  fxSpace.start();
}

// ---- 84-fx-party.js ----
// Party effects for Chaos Mode (part of the "Hidden surprises" setting). Two effects act on the
// report's real native visuals. "Prize Wheel" spins the first pie or donut chart like a game-show
// wheel under a pointer, ticking at every slice edge it passes, and Barry announces the slice it
// lands on. "Bar Rave" makes every bar and column dance to a synthesized 120 bpm beat with cycling
// colors under a soft disco light. Power BI's own elements are only ever moved, scaled, faded or
// filtered by Web Animations that are cancelled at the end; their attributes are read, never
// written. Overlays of our own (the pointer, the disco light) are removed at the end, and every
// sound is synthesized with WebAudio.
//
// Contract, so the lead can inject this file on its own into a page where the mod already runs:
// - It needs only `pbt` (window.__pbt) in scope. Everything else sits inside one block, so its
//   names never meet another page file's.
// - It registers as a module (pbt.fxParty, then pbt.module): start() does nothing (the effects run
//   on demand), stop() ends any running effect at once and closes the audio, and pbt.remove()
//   calls stop(). Evaluating the file again replaces the previous copy.
// - The effects register in the shared registry the Begin menu reads:
//   pbt.chaosFx.wheel and pbt.chaosFx.rave, each { name, run(), stop() }. run() returns a Promise
//   that resolves when the effect has finished and cleaned up, or false when the page has no
//   visual it can use. stop() ends it at once. Escape also ends a running effect.
// - Test calls: window.__pbt.chaosFx.wheel.run(), window.__pbt.chaosFx.rave.run().
//   rave.run({ hue: 'svg' }) puts the color cycle on each chart's svg instead of on each bar, in
//   case the host draws no CSS filter on SVG shapes.
// - Barry (pbt.chaos.say) comments when Chaos Mode is on; otherwise the effects run silently.
{
  if (pbt.fxParty && typeof pbt.fxParty.remove === 'function') { try { pbt.fxParty.remove(); } catch {} }

  const SVG_NS = 'http://www.w3.org/2000/svg';
  const safe = fn => function (...args) { try { return fn.apply(this, args); } catch (e) { try { pbt.log && pbt.log(`fx-party: ${e && e.stack}`); } catch {} } };
  const say = text => { try { const c = pbt.chaos; if (c && typeof c.say === 'function') return c.say(text); } catch {} return false; };

  // The top-level visual containers on the page, as 80-chaos.js finds them, and a container's type.
  const containers = () => [...document.querySelectorAll('.displayArea .visualContainer')].filter(el => !(el.parentElement && el.parentElement.closest('.visualContainer')));
  const typeOf = v => (((v.querySelector('[class*="visual-"]') || { getAttribute: () => '' }).getAttribute('class') || '').match(/\bvisual-(\w+)/) || [])[1] || '';
  const kindOf = v => `${v.getAttribute('aria-roledescription') || ''} ${typeOf(v)}`;
  const shown = el => { const r = el.getBoundingClientRect(); return r.width >= 1 && r.height >= 1; };
  // An SVG element's transform attribute as one CSS matrix, read and never changed. A CSS transform
  // replaces the attribute while it runs, so every transform we animate starts with this.
  const preOf = el => {
    try {
      const list = el.transform && el.transform.baseVal;
      if (list && list.numberOfItems) {
        let m = new DOMMatrix();
        for (let i = 0; i < list.numberOfItems; i += 1) { const t = list.getItem(i).matrix; m = m.multiply(new DOMMatrix([t.a, t.b, t.c, t.d, t.e, t.f])); }
        return `matrix(${m.a}, ${m.b}, ${m.c}, ${m.d}, ${m.e}, ${m.f}) `;
      }
    } catch {}
    return '';
  };
  const ctm = el => { const m = el.getCTM(); return m ? new DOMMatrix([m.a, m.b, m.c, m.d, m.e, m.f]) : new DOMMatrix(); };
  const div = css => { const e = document.createElement('div'); e.setAttribute('data-pbt', ''); e.setAttribute('aria-hidden', 'true'); e.style.cssText = css; return e; };
  const svgEl = (tag, attrs = {}) => { const e = document.createElementNS(SVG_NS, tag); for (const [k, v] of Object.entries(attrs)) e.setAttribute(k, v); return e; };

  // ==== runs: everything an effect starts, so stop() can take it all back ====
  const F = { runs: new Map(), ctx: null, master: null, noiseBuf: null, closeTimer: null, base: [] };
  const newRun = id => {
    const run = { id, anims: [], els: [], timers: [], cleanup: [], bus: null, ended: false };
    run.promise = new Promise(res => { run.resolve = res; });
    F.runs.set(id, run);
    return run;
  };
  const endRun = run => {
    if (!run || run.ended) return;
    run.ended = true;
    run.timers.forEach(t => clearTimeout(t));
    run.anims.forEach(a)PBT"
LR"PBT( => { try { a.cancel(); } catch {} });
    run.els.forEach(e => { try { e.remove(); } catch {} });
    run.cleanup.forEach(fn => { try { fn(); } catch {} });
    if (run.bus) { try { run.bus.disconnect(); } catch {} }
    if (F.runs.get(run.id) === run) F.runs.delete(run.id);
    if (!F.runs.size) closeAudioSoon();
    run.resolve(true);
  };
  const stopFx = id => endRun(F.runs.get(id));
  const later = (run, fn, ms) => { run.timers.push(setTimeout(() => { if (!run.ended) safe(fn)(); }, ms)); };
  const animate = (run, el, frames, opts) => {
    if (!el || typeof el.animate !== 'function') return null;
    const a = el.animate(frames, opts);
    run.anims.push(a);
    return a;
  };

  // ==== sound: one AudioContext made when first needed, a gain bus per run, closed when idle ====
  const audio = () => {
    // Silent unless Skin sounds is on (off by default), like every other sound of the mod.
    if (!pbt.feature('sounds')) return null;
    try {
      clearTimeout(F.closeTimer);
      if (!F.ctx || F.ctx.state === 'closed') {
        F.ctx = new AudioContext();
        F.master = F.ctx.createGain();
        F.master.gain.value = 0.5;
        F.master.connect(F.ctx.destination);
        const len = F.ctx.sampleRate, buf = F.ctx.createBuffer(1, len, F.ctx.sampleRate), d = buf.getChannelData(0);
        for (let i = 0; i < len; i += 1) d[i] = Math.random() * 2 - 1;
        F.noiseBuf = buf;
      }
      if (F.ctx.state === 'suspended') F.ctx.resume().catch(() => {});
      return F.ctx;
    } catch { return null; }
  };
  const closeAudio = () => {
    clearTimeout(F.closeTimer);
    if (F.ctx) { try { F.ctx.close().catch(() => {}); } catch {} }
    F.ctx = null; F.master = null; F.noiseBuf = null;
  };
  const closeAudioSoon = () => { clearTimeout(F.closeTimer); F.closeTimer = setTimeout(() => { if (!F.runs.size) closeAudio(); }, 1500); };
  // The run's bus; disconnecting it silences everything the run scheduled.
  const busOf = run => {
    if (run.bus) return run.bus;
    const ctx = audio();
    if (!ctx) return null;
    run.bus = ctx.createGain();
    run.bus.connect(F.master);
    return run.bus;
  };
  // A tone at absolute audio time t, with a short attack and an exponential release.
  const tone = (run, { t, type = 'square', f = 440, f2, len = 0.1, vol = 0.04, lp }) => {
    const bus = busOf(run);
    if (!bus) return;
    try {
      const ctx = F.ctx, o = ctx.createOscillator(), g = ctx.createGain();
      o.type = type;
      o.frequency.setValueAtTime(f, t);
      if (f2) o.frequency.exponentialRampToValueAtTime(f2, t + len);
      g.gain.setValueAtTime(0.0001, t);
      g.gain.exponentialRampToValueAtTime(vol, t + 0.006);
      g.gain.exponentialRampToValueAtTime(0.0001, t + len);
      let out = o;
      if (lp) { const flt = ctx.createBiquadFilter(); flt.type = 'lowpass'; flt.frequency.setValueAtTime(lp[0], t); flt.frequency.exponentialRampToValueAtTime(lp[1], t + len); flt.Q.value = lp[2] || 1; out = o.connect(flt); }
      out.connect(g).connect(bus);
      o.start(t); o.stop(t + len + 0.03);
    } catch {}
  };
  // A filtered noise burst at absolute audio time t, from a random point of the shared buffer.
  const noise = (run, { t, len = 0.05, vol = 0.05, type = 'highpass', f = 6000, q = 0.7 }) => {
    const bus = busOf(run);
    if (!bus || !F.noiseBuf) return;
    try {
      const ctx = F.ctx, n = ctx.createBufferSource(), flt = ctx.createBiquadFilter(), g = ctx.createGain();
      n.buffer = F.noiseBuf; flt.type = type; flt.frequency.value = f; flt.Q.value = q;
      g.gain.setValueAtTime(vol, t);
      g.gain.exponentialRampToValueAtTime(0.0001, t + len);
      n.connect(flt).connect(g).connect(bus);
      n.start(t, Math.random() * Math.max(0, 0.95 - len)); n.stop(t + len + 0.02);
    } catch {}
  };
  const now = () => (F.ctx ? F.ctx.currentTime : 0);
  // A short brass fanfare in sawtooth through a lowpass: three pickups and a held chord.
  const fanfare = run => {
    if (!audio()) return;
    const t = now() + 0.03;
    [[392, 0, 0.11], [392, 0.13, 0.11], [392, 0.26, 0.11], [523.25, 0.39, 0.42]].forEach(([f, at, len]) => tone(run, { t: t + at, type: 'sawtooth', f, len, vol: 0.05, lp: [2600, 1400] }));
    [523.25, 659.25, 783.99, 1046.5].forEach(f => tone(run, { t: t + 0.82, type: 'sawtooth', f, len: 1.1, vol: 0.03, lp: [3000, 900] }));
    noise(run, { t: t + 0.82, len: 0.9, vol: 0.04, f: 5000 });
  };

  // ==== Prize Wheel ====
  const SLICE_SEL = 'path[class*="slice" i]';
  const isSlice = p => p.ownerSVGElement && !/highlight/i.test(p.getAttribute('class') || '') && !p.closest('[data-pbt], [class*="legend" i]') && shown(p);
  // The first pie or donut on the page (any other container with slices after those), its
  // slices in one svg, and the g that holds them all.
  const findWheel = () => {
    const all = containers();
    const pies = all.filter(v => /pie|donut|doughnut/i.test(kindOf(v)));
    for (const v of [...pies, ...all.filter(v => !pies.includes(v))]) {
      const found = [...v.querySelectorAll(SLICE_SEL)].filter(isSlice);
      if (found.length < 2) continue;
      const svg = found[0].ownerSVGElement, slices = found.filter(p => p.ownerSVGElement === svg);
      let g = slices[0].parentNode;
      while (g && g !== svg && !slices.every(p => g.contains(p))) g = g.parentNode;
      if (!g || g === svg || g.tagName.toLowerCase() !== 'g' || typeof g.animate !== 'function') continue;
      return { v, svg, g, slices };
    }
    return null;
  };
  const norm = a => ((a % 360) + 360) % 360;
  // Each slice's angular range in the group's own coordinates: points sampled along its outline,
  // the centre from the bounding box of all of them, then the largest empty gap in a slice's
  // angles is what lies outside it. Angles are in degrees, clockwise from 3 o'clock (y points down).
  const wheelGeometry = (g, slices) => {
    const gi = ctm(g).inverse(), N = 72;
    const sampled = slices.map(p => {
      let m = new DOMMatrix();
      try )PBT"
LR"PBT({ m = gi.multiply(ctm(p)); } catch {}
      const L = p.getTotalLength(), pts = [];
      if (!(L > 0)) return { p, pts };
      for (let i = 0; i < N; i += 1) { const q = p.getPointAtLength((L * i) / N); pts.push(new DOMPoint(q.x, q.y).matrixTransform(m)); }
      return { p, pts };
    });
    const xs = sampled.flatMap(s => s.pts.map(q => q.x)), ys = sampled.flatMap(s => s.pts.map(q => q.y));
    if (!xs.length) return null;
    const minX = Math.min(...xs), maxX = Math.max(...xs), minY = Math.min(...ys), maxY = Math.max(...ys);
    const cx = (minX + maxX) / 2, cy = (minY + maxY) / 2, R = Math.max(maxX - minX, maxY - minY) / 2;
    if (!(R > 2)) return null;
    const parts = [];
    sampled.forEach(({ p, pts }, idx) => {
      const angs = pts.filter(q => Math.hypot(q.x - cx, q.y - cy) > R * 0.3).map(q => norm((Math.atan2(q.y - cy, q.x - cx) * 180) / Math.PI)).sort((a, b) => a - b);
      if (angs.length < 3) return;
      let gap = angs[0] + 360 - angs[angs.length - 1], start = angs[0], end = angs[angs.length - 1];
      for (let i = 1; i < angs.length; i += 1) if (angs[i] - angs[i - 1] > gap) { gap = angs[i] - angs[i - 1]; start = angs[i]; end = angs[i - 1]; }
      const span = 360 - gap;
      if (span < 0.5) return;
      parts.push({ p, idx, start, end, span, mid: norm(start + span / 2) });
    });
    if (parts.length < 2) return null;
    const edges = [];
    parts.forEach(s => [s.start, s.end].forEach(e => { if (!edges.some(x => Math.abs(norm(x - e + 180) - 180) < 1)) edges.push(e); }));
    return { cx, cy, R, parts, edges };
  };
  const cleanAria = s => {
    let first = String(s || '').replace(/\s+/g, ' ').trim().split(/[,;]\s*/)[0];
    if (/:\s/.test(first)) first = first.split(/:\s*/).pop();
    return first.trim();
  };
  // The winning slice's name: its own aria-label or title, then the legend entry in the same
  // order, then the data label nearest the slice, then just its number.
  const sliceLabel = (v, slices, part, screen) => {
    const p = part.p;
    const own = p.getAttribute('aria-label') || (p.querySelector('title') || {}).textContent
      || (p.parentNode && p.parentNode.children.length === 1 && p.parentNode.getAttribute && p.parentNode.getAttribute('aria-label'));
    if (cleanAria(own)) return cleanAria(own).slice(0, 48);
    const items = [...v.querySelectorAll('.legendItem, [class*="legendItem" i]')].filter(it => !it.parentElement.closest('.legendItem, [class*="legendItem" i]'));
    const names = items.map(it => ((it.querySelector('title') || {}).textContent || (it.querySelector('.legendText, text, span') || it).textContent || '').trim());
    const k = slices.indexOf(p);
    if (names.length && k >= 0 && k < names.length && names[k]) return names[k].slice(0, 48);
    const rad = (part.mid * Math.PI) / 180, tx = screen.x + Math.cos(rad) * screen.r * 1.2, ty = screen.y + Math.sin(rad) * screen.r * 1.2;
    let best = null;
    for (const t of v.querySelectorAll('svg text')) {
      if (t.closest('[class*="legend" i], [data-pbt]')) continue;
      const txt = (t.textContent || '').trim(), r = t.getBoundingClientRect();
      if (!txt || !r.width) continue;
      const d = Math.hypot(r.left + r.width / 2 - tx, r.top + r.height / 2 - ty);
      if (d < screen.r * 0.9 && (!best || d < best.d)) best = { d, txt };
    }
    return best ? best.txt.slice(0, 48) : `Slice ${k + 1}`;
  };
  // A pointer of our own: a red flap on a gold hub, hinged at the hub, tip down on the rim.
  const makePointer = (x, y, k) => {
    const w = 28 * k, h = 38 * k;
    const el = div(`position:fixed;left:${x - w / 2}px;top:${y - h}px;width:${w}px;height:${h}px;z-index:2147483002;pointer-events:none;transform-origin:50% ${(8 / 38) * 100}%;filter:drop-shadow(1px 2px 1px rgba(0,0,0,.45))`);
    const svg = svgEl('svg', { viewBox: '0 0 28 38', width: w, height: h, style: 'display:block;overflow:visible' });
    svg.append(svgEl('path', { d: 'M4 8 L24 8 L14 36 Z', fill: '#e5383b', stroke: '#222', 'stroke-width': 2, 'stroke-linejoin': 'round' }));
    svg.append(svgEl('circle', { cx: 14, cy: 8, r: 6, fill: '#f2c94c', stroke: '#222', 'stroke-width': 2 }));
    el.append(svg);
    return el;
  };
  const WIND = 450, SPIN = 6500, HOLD = 3000, BACK = 900, LEAD = 60;
  // The spin's easing: a quartic ease-out, so the wheel starts fast and creeps to a stop.
  const ease = x => 1 - (1 - x) ** 4;
  const unease = y => 1 - (1 - Math.min(Math.max(y, 0), 1)) ** 0.25;
  const wheel = () => {
    stopFx('wheel');
    const found = findWheel();
    const geo = found && wheelGeometry(found.g, found.slices);
    if (!geo) { say('No wheel on this page. Try Sales Analysis.'); return false; }
    const { v, svg, g, slices } = found, { cx, cy, parts, edges } = geo;
    const run = newRun('wheel');
    // Where the wheel is on screen: the union of the slices' boxes.
    const boxes = slices.map(p => p.getBoundingClientRect());
    const sl = Math.min(...boxes.map(b => b.left)), st = Math.min(...boxes.map(b => b.top)), sr = Math.max(...boxes.map(b => b.right)), sb = Math.max(...boxes.map(b => b.bottom));
    const screen = { x: (sl + sr) / 2, y: (st + sb) / 2, r: Math.max(sr - sl, sb - st) / 2 };
    // The winner, by slice size like a real wheel, and a landing point inside it away from its edges.
    let pick = Math.random() * parts.reduce((s, q) => s + q.span, 0), win = parts[parts.length - 1];
    for (const q of parts) { if ((pick -= q.span) <= 0) { win = q; break; } }
    const margin = Math.min(win.span * 0.2, 6);
    const land = win.start + margin + Math.random() * Math.max(0, win.span - margin * 2);
    const label = sliceLabel(v, slices, win, screen);
    // The pointer is at 12 o'clock (-90 degrees); turning by theta moves angle a to a + theta.
    const turns = 5 + Math.floor(Math.random() * 2), back = -12;
    const total = norm(-90 - land) + 360 * turns;
    const pre = preOf(g);
    const rot = a => `${pre}translate(${cx}px, ${cy}px) rotate(${a.toFixed(3)}deg) translate(${-cx}px, ${-cy)PBT"
LR"PBT(}px)`;
    // The spin as keyframes: a wind-up back, then the ease-out sampled finely.
    const D = WIND + SPIN, frames = [{ transform: rot(0), offset: 0, easing: 'ease-in-out' }, { transform: rot(back), offset: WIND / D }];
    for (let i = 1; i <= 96; i += 1) { const x = i / 96; frames.push({ transform: rot(back + (total - back) * ease(x)), offset: (WIND + SPIN * x) / D }); }
    // A tick each time an edge passes the pointer: the pointer's angle in the wheel's own
    // coordinates is -90 - theta, so edge e passes at theta = -90 - e (plus whole turns).
    const ticks = [];
    edges.forEach(e => {
      for (let th = norm(-90 - e); th <= total; th += 360) if (th > back) ticks.push(WIND + SPIN * unease((th - back) / (total - back)));
    });
    ticks.sort((a, b) => a - b);
    const kept = ticks.filter((t, i) => i === 0 || t - ticks[i - 1] > 22);

    const ptr = makePointer(screen.x, Math.max(0, st) + 5, Math.min(1.6, Math.max(0.8, screen.r / 90)));
    document.body.append(ptr);
    run.els.push(ptr);
    animate(run, ptr, [{ translate: '0 -40px', opacity: 0 }, { translate: '0 0', opacity: 1 }], { duration: 350, easing: 'cubic-bezier(.3, 1.5, .5, 1)', fill: 'both' });
    say('Spin to win! Results not statistically significant.');
    // Data labels and leader lines stay where Power BI drew them, so they fade while the wheel turns.
    const labels = [...svg.querySelectorAll('text, polyline, line')].filter(n => !g.contains(n) && !n.closest('[class*="legend" i], [data-pbt]'));
    labels.forEach(n => animate(run, n, [{ opacity: 1 }, { opacity: 0 }], { duration: 250, delay: LEAD, fill: 'forwards' }));
    animate(run, g, frames, { duration: D, delay: LEAD, fill: 'forwards' });
    const ctx = audio();
    if (ctx) {
      const t0 = ctx.currentTime + LEAD / 1000;
      kept.forEach(ms => {
        const t = t0 + ms / 1000;
        tone(run, { t, f: 2000 + Math.random() * 300, len: 0.014, vol: 0.05 });
        noise(run, { t, len: 0.018, vol: 0.09, f: 3500 });
      });
    }
    kept.forEach(ms => animate(run, ptr, [{ rotate: '0deg' }, { rotate: '-26deg', offset: 0.25 }, { rotate: '0deg' }], { duration: 150, delay: LEAD + ms, easing: 'ease-out' }));

    const landed = LEAD + D;
    // The winner pops out a little along its own middle, in the turning group's coordinates.
    const rad = (win.mid * Math.PI) / 180, wpre = preOf(win.p);
    const popIn = `translate(0px, 0px) ${wpre}`, popOut = `translate(${(Math.cos(rad) * geo.R * 0.07).toFixed(2)}px, ${(Math.sin(rad) * geo.R * 0.07).toFixed(2)}px) ${wpre}`;
    const others = parts.filter(q => q !== win);
    later(run, () => {
      others.forEach(q => animate(run, q.p, [{ opacity: 1 }, { opacity: 0.35 }], { duration: 300, fill: 'forwards' }));
      animate(run, win.p, [{ transform: popIn }, { transform: popOut }], { duration: 300, easing: 'cubic-bezier(.3, 1.6, .5, 1)', fill: 'forwards' });
      animate(run, ptr, [{ scale: '1' }, { scale: '1.25' }, { scale: '1' }], { duration: 400, iterations: 2 });
      fanfare(run);
      say(`${label} wins the pizza party!`);
      try {
        if (pbt.fun && typeof pbt.fun.confetti === 'function') {
          pbt.fun.confetti();
          const c = [...document.querySelectorAll('canvas.pbt-confetti')].pop();
          if (c) run.els.push(c);
        }
      } catch {}
    }, landed + 150);
    // Back to where Power BI drew it: the nearest whole turn, then every animation is cancelled.
    later(run, () => {
      const r = norm(total), home = r > 180 ? total + (360 - r) : total - r;
      animate(run, g, [{ transform: rot(total) }, { transform: rot(home) }], { duration: BACK, easing: 'ease-in-out', fill: 'forwards' });
      others.forEach(q => animate(run, q.p, [{ opacity: 0.35 }, { opacity: 1 }], { duration: BACK, fill: 'forwards' }));
      animate(run, win.p, [{ transform: popOut }, { transform: popIn }], { duration: BACK, easing: 'ease-in-out', fill: 'forwards' });
      labels.forEach(n => animate(run, n, [{ opacity: 0 }, { opacity: 1 }], { duration: 300, delay: BACK - 200, fill: 'forwards' }));
      animate(run, ptr, [{ opacity: 1 }, { opacity: 0 }], { duration: 300, delay: BACK - 300, fill: 'forwards' });
    }, landed + 150 + HOLD);
    // The confetti canvas removes itself after 4.5 s; the run ends after both.
    later(run, () => endRun(run), Math.max(landed + 150 + HOLD + BACK + 100, landed + 150 + 4700));
    return run.promise;
  };

  // ==== Bar Rave ====
  const BPM = 120, BEAT = 60000 / BPM, BEATS = 20;
  // Every visible bar and column rect in the native charts, with which way it grows.
  const findBars = () => {
    const out = [];
    for (const v of containers()) {
      const kind = kindOf(v);
      let i = 0;
      for (const r of v.querySelectorAll('rect')) {
        const cls = r.getAttribute('class') || '';
        if (!/bar|column/i.test(cls) || /scroll|background|axis|legend|grid/i.test(cls)) continue;
        if (r.closest('[data-pbt], [class*="legend" i]') || !shown(r) || typeof r.animate !== 'function') continue;
        let bb;
        try { bb = r.getBBox(); } catch { continue; }
        if (!bb.width || !bb.height) continue;
        const across = /\bbar\b/i.test(cls) ? true : /\bcolumn\b/i.test(cls) ? false : /bar/i.test(kind) && !/column/i.test(kind);
        out.push({ r, bb, across, i, svg: r.ownerSVGElement });
        i += 1;
        if (out.length >= 600) return out;
      }
    }
    return out;
  };
  // The beat, scheduled ahead on the audio clock: a kick on every beat, a clap on 2 and 4, hats
  // on the off-beats, an octave-bouncing bass line (A, A, F, G), and a cymbal at the end.
  const beat = (run, t0) => {
    const roots = [55, 55, 43.65, 49];
    for (let b = 0; b < BEATS; b += 1) {
      const t = t0 + (b * BEAT) / 1000, half = BEAT / 2000, q = BEAT / 4000;
      tone(run, { t, type: 'sine', f: 150, f2: 42, len: 0.3, vol: 0.5 });
      if (b % 2 === 1) { noise(run, { t, len: 0.16, vol: 0.16, type: 'bandpass', f: 1800, q: 0.8 }); tone(r)PBT"
LR"PBT(un, { t, type: 'triangle', f: 190, f2: 150, len: 0.08, vol: 0.05 }); }
      noise(run, { t: t + half, len: 0.05, vol: 0.06, f: 8000 });
      noise(run, { t: t + q, len: 0.025, vol: 0.02, f: 9000 });
      noise(run, { t: t + half + q, len: 0.025, vol: 0.02, f: 9000 });
      for (let e = 0; e < 2; e += 1) {
        const root = roots[Math.floor(b / 4) % 4];
        tone(run, { t: t + e * half, type: 'sawtooth', f: e ? root * 2 : root, len: 0.2, vol: 0.07, lp: [900, 260, 4] });
      }
    }
    const end = t0 + (BEATS * BEAT) / 1000;
    tone(run, { t: end, type: 'sine', f: 150, f2: 42, len: 0.4, vol: 0.5 });
    noise(run, { t: end, len: 1.0, vol: 0.08, f: 5000 });
  };
  const rave = (opts = {}) => {
    stopFx('rave');
    const bars = findBars();
    if (!bars.length) { say('No bars on this page. This party is bring your own column chart.'); return false; }
    const run = newRun('rave');
    say('Drop the bass. And the decimal places.');
    const lead = 120, total = BEATS * BEAT;
    const ctx = audio();
    if (ctx) beat(run, ctx.currentTime + lead / 1000);
    // Each bar scales along its value axis from its baseline (the bottom of a column, the left of
    // a bar), one bounce per beat, in a ripple of four phases and with its own height.
    bars.forEach(({ r, bb, across, i }) => {
      const pre = preOf(r), bx = across ? bb.x : bb.x + bb.width / 2, by = across ? bb.y + bb.height / 2 : bb.y + bb.height;
      const sc = s => `${pre}translate(${bx}px, ${by}px) scale(${across ? s : 1}, ${across ? 1 : s}) translate(${-bx}px, ${-by}px)`;
      const amp = 0.18 + Math.random() * 0.27;
      animate(run, r, [
        { transform: sc(1), easing: 'cubic-bezier(.2, .9, .3, 1)' },
        { transform: sc(1 + amp), offset: 0.16, easing: 'ease-in-out' },
        { transform: sc(1 - amp * 0.5), offset: 0.7, easing: 'ease-in-out' },
        { transform: sc(1) },
      ], { duration: BEAT, iterations: BEATS, delay: lead + (i % 4) * (BEAT / 4) });
    });
    // Party colors by a CSS hue rotation (the fill attribute is never touched): on each bar, or on
    // each chart's svg with { hue: 'svg' }. One cycle per bar of four beats, ending where it began.
    const hueTargets = opts.hue === 'svg' ? [...new Set(bars.map(b => b.svg).filter(Boolean))].map((r, i) => ({ r, i })) : bars;
    hueTargets.forEach(({ r, i }) => animate(run, r, [
      { filter: 'hue-rotate(0deg) saturate(1)' },
      { filter: 'hue-rotate(180deg) saturate(1.7)', offset: 0.5 },
      { filter: 'hue-rotate(360deg) saturate(1)' },
    ], { duration: BEAT * 4, iterations: BEATS / 4, delay: lead + (i % 8) * 90 }));
    // The disco light: slow beams from the top of the canvas and drifting sparkles, screen-blended
    // at low opacity, taking no clicks.
    const da = document.querySelector('.displayArea');
    const p = da ? da.getBoundingClientRect() : { left: 0, top: 0, right: innerWidth, bottom: innerHeight };
    const L = Math.max(0, p.left), T = Math.max(0, p.top), W = Math.min(innerWidth, p.right) - L, H = Math.min(innerHeight, p.bottom) - T;
    if (W > 0 && H > 0) {
      const light = div(`position:fixed;left:${L}px;top:${T}px;width:${W}px;height:${H}px;overflow:hidden;pointer-events:none;z-index:2147483001;mix-blend-mode:screen;opacity:0`);
      const beams = div('position:absolute;left:-100%;top:-5%;width:300%;height:260%;transform-origin:50% 0;background:repeating-conic-gradient(from 0deg at 50% 0%, rgba(255,40,190,.6) 0deg 5deg, transparent 5deg 16deg, rgba(0,210,255,.6) 16deg 21deg, transparent 21deg 32deg, rgba(255,225,0,.55) 32deg 37deg, transparent 37deg 48deg)');
      const dots = div('position:absolute;inset:0;background-image:radial-gradient(circle, rgba(255,255,255,.9) 0 1.5px, transparent 2.5px), radial-gradient(circle, rgba(255,130,230,.8) 0 1.5px, transparent 2.5px);background-size:110px 80px, 150px 110px;background-position:0 0, 40px 30px');
      light.append(beams, dots);
      document.body.append(light);
      run.els.push(light);
      animate(run, light, [{ opacity: 0 }, { opacity: 0.26 }], { duration: 600, delay: lead, fill: 'forwards' });
      animate(run, light, [{ opacity: 0.26 }, { opacity: 0 }], { duration: 600, delay: lead + total - 300, fill: 'forwards' });
      animate(run, beams, [{ transform: 'rotate(-22deg)' }, { transform: 'rotate(22deg)' }], { duration: BEAT * 4, iterations: BEATS / 4, direction: 'alternate', easing: 'ease-in-out', delay: lead });
      animate(run, beams, [{ filter: 'hue-rotate(0deg)' }, { filter: 'hue-rotate(360deg)' }], { duration: BEAT * 8, iterations: Infinity });
      animate(run, dots, [{ backgroundPosition: '0px 0px, 40px 30px' }, { backgroundPosition: '330px 80px, -260px 140px' }], { duration: total, delay: lead });
    }
    // The bars finish their last bounce back at rest; the cymbal rings out, then everything goes.
    later(run, () => endRun(run), lead + total + BEAT * 0.75 + 900);
    return run.promise;
  };

  // ==== registry, Escape, module ====
  // Added by start() and taken off by stop(), which pbt.remove() calls, so a removed or reloaded
  // copy leaves no listener behind.
  const onKey = e => { if (e.key === 'Escape' && F.runs.size) [...F.runs.values()].forEach(endRun); };
  let keyOn = false;

  // An effect that throws while it sets up is ended at once, so nothing of it stays on the page.
  const guarded = (id, fn) => {
    try { return fn(); } catch (e) {
      try { pbt.log && pbt.log(`fx-party ${id}: ${e && e.stack}`); } catch {}
      stopFx(id);
      return false;
    }
  };
  pbt.chaosFx = pbt.chaosFx || {};
  const entries = {
    wheel: { name: 'Prize Wheel', run: () => guarded('wheel', wheel), stop: () => stopFx('wheel') },
    rave: { name: 'Bar Rave', run: opts => guarded('rave', () => rave(opts)), stop: () => stopFx('rave') },
  };
  Object.assign(pbt.chaosFx, entries);

  const fxParty = {
    name: 'fxParty',
    start() {
      // Off with the "Hidden surprises" setting: no key listener at all.
      if (!pbt.featu)PBT"
LR"PBT(re('surprises')) return false;
      if (!keyOn) { window.addEventListener('keydown', onKey, true); keyOn = true; }
      return true;
    },
    // Ends every running effect at once, closes the audio and takes the Escape listener off.
    stop() {
      [...F.runs.values()].forEach(endRun);
      closeAudio();
      if (keyOn) { window.removeEventListener('keydown', onKey, true); keyOn = false; }
    },
    state() { return { running: [...F.runs.keys()], audio: F.ctx ? F.ctx.state : null }; },
    // Takes this copy out completely, so a fresh evaluation of the file can replace it.
    remove() {
      this.stop();
      F.base.splice(0).forEach(fn => { try { fn(); } catch {} });
      Object.entries(entries).forEach(([id, e]) => { if (pbt.chaosFx && pbt.chaosFx[id] === e) delete pbt.chaosFx[id]; });
      const i = pbt.modules ? pbt.modules.indexOf(this) : -1;
      if (i >= 0) pbt.modules.splice(i, 1);
      if (pbt.fxParty === this) delete pbt.fxParty;
    },
  };
  pbt.fxParty = fxParty;
  if (typeof pbt.module === 'function') pbt.module(fxParty);
}

return pbt.start();
})();
)PBT";

// ---- WebView2 pieces used here (layouts from WebView2.h, SDK 1.0.2903.40) ----
// Defined here: the IID_IUnknown symbol lives in uuid.lib, which Windhawk does not link.
static const GUID kIID_IUnknown = {0x00000000, 0x0000, 0x0000, {0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46}};
static const GUID IID_EnvCompleted = {0x4e8a3389, 0xc9d8, 0x4bd2, {0xb6, 0xb5, 0x12, 0x4f, 0xee, 0x6c, 0xc1, 0x4d}};
static const GUID IID_CtrlCompleted = {0x6c4819f3, 0xc9b7, 0x4260, {0x81, 0x27, 0xc9, 0xf5, 0xbd, 0xe7, 0xf6, 0x8c}};
static const GUID IID_AddScriptCompleted = {0xb99369f3, 0x9b11, 0x47b5, {0xbc, 0x6f, 0x8e, 0x78, 0x95, 0xfc, 0xea, 0x17}};
static const GUID IID_Environment10 = {0xee0eb9df, 0x6f12, 0x46ce, {0xb5, 0x3f, 0x3f, 0x47, 0xb9, 0xc9, 0x28, 0xe0}};

constexpr int kEnvCreateController = 3;                 // ICoreWebView2Environment
constexpr int kEnv10CreateControllerWithOptions = 21;   // ICoreWebView2Environment10
constexpr int kCtrlGetCoreWebView2 = 25;                // ICoreWebView2Controller
constexpr int kWebViewAddScriptOnDocumentCreated = 27;  // ICoreWebView2
constexpr int kWebViewRemoveScriptOnDocumentCreated = 28;
constexpr int kWebViewExecuteScript = 29;

template <typename T>
static T Slot(IUnknown* obj, int index) {
    return reinterpret_cast<T>((*reinterpret_cast<void***>(obj))[index]);
}

// ---- settings ----
// The choice made inside Power BI lives in the page's own localStorage, which every Power BI
// view shares. The mod never uses WebView2's message channel: Power BI's own bridge parses
// every message there as one of its commands and reports an error for anything else.
static std::mutex g_lock;
static std::wstring g_settingsJson;  // the mod's settings, as the page's config overrides
static std::wstring g_cheerJs;       // "var __PBT_CHEER__ = 'data:...';" when a sound file is set
static std::wstring g_projectJs;     // "var __PBT_PROJECT_JSON__ = '...';" when report files were found
static std::wstring g_projectObject; // the same files as the JSON object the page parses (under g_lock)

// ---- the report Power BI has open, found without a setting ----
// The tools that compare the page with the saved report (Theme check, Filter map's own, hidden and
// locked filters, the palette from the theme, Stencil's built blocks) need its project files. The
// mod finds them by itself: the .pbip Power BI was started with, then the report Power BI loads or
// saves while it runs, seen as its .Report folder's definition.pbir, which Power BI opens (in
// CreateFileW) to load a report and moves into place (in MoveFileWithProgressW) on every save, Save
// As included. Other .pbip files Power BI touches (its Recent list) are not taken for the open
// report. A save also moves files under the .Report folder, which the same watch sees, so the tools
// compare with what was just saved. The "project" setting is only a fallback for a window where no
// report at all was found (not for a .pbix, which has no project files).
static std::mutex g_reportLock;      // guards the four below; held only for a copy or a compare
static std::wstring g_reportPbip;    // the .pbip on Power BI's command line
static std::wstring g_reportSeen;    // the .Report folder whose definition.pbir Power BI read or wrote last
static bool g_reportPbix = false;    // Power BI was started with a .pbix: no project files, no fallback
static bool g_rootFromSetting = false;  // the folder read last came from the "project" setting
static std::wstring g_tempDir;          // the user's temp folder, long form, with its trailing '\' (set in Wh_ModInit)
static std::wstring g_reportRoot;    // the .Report folder read last, with a trailing '\'
static HANDLE g_wake = nullptr;      // auto-reset: a .pbip or a save was seen, or the settings changed
static HANDLE g_worker = nullptr;    // reads the files and hands them to the open pages
static DWORD g_workerId = 0;
static std::atomic<bool> g_stopWorker{false};
static std::atomic<ULONGLONG> g_dueAt{0};  // when to read again: a save writes many files in a row

static std::string Base64(const std::vector<unsigned char>& in) {
    static const char* t = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    std::string out;
    out.reserve((in.size() + 2) / 3 * 4);
    size_t i = 0;
    for (; i + 2 < in.size(); i += 3) {
        unsigned v = ((unsigned)in[i] << 16) | ((unsigned)in[i + 1] << 8) | (unsigned)in[i + 2];
        out += t[(v >> 18) & 63]; out += t[(v >> 12) & 63]; out += t[(v >> 6) & 63]; out += t[v & 63];
    }
    if (i < in.size()) {
        unsigned v = (unsigned)in[i] << 16;
        if (i + 1 < in.size()) v |= (unsigned)in[i + 1] << 8;
        out += t[(v >> 18) & 63]; out += t[(v >> 12) & 63];
        out += (i + 1 < in.size()) ? t[(v >> 6) & 63] : '=';
        out += '=';
    }
    return out;
}

// The sound file named in the settings (an .mp3, .wav, .ogg or .m4a under 2 MB), embedded as a
// data URL for the page. Nothing is embedded when the setting is empty or the file cannot be read.
static std::wstring CheerScript() {
    std::wstring path = WindhawkUtils::StringSetting::make(L"cheer").get();
    while (!path.empty() && (path.back() == L' ' || path.back() == L'"')) path.pop_back();
    while (!path.empty() && (path.front() == L' ' || path.front() == L'"')) path.erase(path.begin());
    if (path.empty()) return L"";
    HANDLE h = CreateFileW(path.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (h == INVALID_HANDLE_VALUE) { Wh_Log(L"cheer: cannot open %s", path.c_str()); return L""; }
    LARGE_INTEGER size = {};
    GetFileSizeEx(h, &size);
    if (size.QuadPart <= 0 || size.QuadPart > 2 * 1024 * 1024) { CloseHandle(h); Wh_Log(L"cheer: file empty or over 2 MB"); return L""; }
    std::vector<unsigned char> bytes((size_t)size.QuadPart);
    DWORD got = 0;
    BOOL ok = ReadFile(h, bytes.data(), (DWORD)bytes.size(), &got, nullptr);
    CloseHandle(h);
    if (!ok || got != bytes.size()) { Wh_Log(L"cheer: cannot read %s", path.c_str()); return L""; }
    std::string b64 = Base64(bytes);
    std::wstring ext = path.size() > 4 ? path.substr(path.size() - 4) : L"";
    for (size_t i = 0; i < ext.size(); i++) ext[i] = (wchar_t)towlower(ext[i]);
    const wchar_t* mime = ext == L".wav" ? L"audio/wav" : ext == L".ogg" ? L"audio/ogg" : ext == L".m4a" ? L"audio/mp4" : L"audio/mpeg";
    std::wstring s = L"var __PBT_CHEER__ = \"data:";
    s += mime;
    s += L";base64,";
    s.append(b64.begin(), b64.end());
    s += L"\";\n";
    return s;
}

// A JSON string literal (quotes included) for UTF-16 text: quotes, backslashes, control
// characters and U+2028/U+2029 escaped, so the result is safe inside a script.
static void AppendJsonString(std::wstring& out, const std::wstring& text) {
    out += L'"';
    wchar_t buf[8];
    for (wchar_t ch : text) {
        if (ch == L'"') out += L"\\\"";
        else if (ch == L'\\') out += L"\\\\";
        else if (ch < 0x20 || ch == 0x2028 || ch == 0x2029) { swprintf(buf, 8, L"\\u%04x", (unsigned)ch); out += buf; }
        else out += ch;
    }
    out += L'"';
}

// The report's project files (a .Report folder, found as described above, or the "project"
// setting), every .json under definition and StaticResources, as { root, files: [{ path, text }] }
// for the page.
// Caps: 1 MB a file, 8 MB in all.
static void ProjectWalk(const std::wstring& root, const std::wstring& rel, std::wstring& files, size_t& total, int& count) {
    WIN32_FIND_DATAW fd;
    std::wstring pattern = root + L"\\" + rel + L"\\*";
    HANDLE f = FindFirstFileW(pattern.c_str(), &fd);
    if (f == INVALID_HANDLE_VALUE) return;
    do {
        std::wstring name = fd.cFileName;
        if (name == L"." || name == L"..") continue;
        std::wstring sub = rel + L"/" + name;
        if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            // Junctions and directory symlinks (name surrogates) can point anywhere, a large tree or
            // back up this one, so they are not followed. Other reparse points are ordinary folders:
            // OneDrive and synced SharePoint libraries mark every folder as one.
            bool link = (fd.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT) && IsReparseTagNameSurrogate(fd.dwReserved0);
            if (!link && name != L".pbi" && name != L"CustomVisuals") ProjectWalk(root, sub, files, total, count);
            continue;
        }
        if (name.size() < 5 || _wcsicmp(name.c_str() + name.size() - 5, L".json") != 0) continue;
        if (fd.nFileSizeHigh || fd.nFileSizeLow > 1024 * 1024 || total + fd.nFileSizeLow > 8 * 1024 * 1024) continue;
        std::wstring full = root + L"\\" + sub;
        for (auto& c : full) if (c == L'/') c = L'\\';
        HANDLE h = CreateFileW(full.c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
        if (h == INVALID_HANDLE_VALUE) continue;
        std::string bytes(fd.nFileSizeLow, '\0');
        DWORD got = 0;
        BOOL ok = ReadFile(h, bytes.data(), (DWORD)bytes.size(), &got, nullptr);
        CloseHandle(h);
        if (!ok || got != bytes.size()) continue;
        size_t skip = bytes.size() >= 3 && (unsigned char)bytes[0] == 0xEF && (unsigned char)bytes[1] == 0xBB && (unsigned char)bytes[2] == 0xBF ? 3 : 0;
        int n = MultiByteToWideChar(CP_UTF8, 0, bytes.data() + skip, (int)(bytes.size() - skip), nullptr, 0);
        std::wstring text(n, L'\0');
        if (n) MultiByteToWideChar(CP_UTF8, 0, bytes.data() + skip, (int)(bytes.size() - skip), text.data(), n);
        total += bytes.size();
        if (count++) files += L",";
        files += L"{\"path\":";
        AppendJsonString(files, sub.substr(1));
        files += L",\"text\":";
        AppendJsonString(files, text);
        files += L"}";
    } while (FindNextFileW(f, &fd));
    FindClose(f);
}

// The report folder a .pbip names (artifacts[].report.path, relative to the .pbip), found with a
// plain text search; else the .pbip's own name with .Report.
static std::wstring PbipReportFolder(const std::wstring& pbip) {
    std::wstring dir = pbip.substr(0, pbip.find_last_of(L"\\/") + 1);
    std::wstring fallback = pbip.substr(0, pbip.size() - 5) + L".Report";
    HANDLE h = CreateFileW(pbip.c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (h == INVALID_HANDLE_VALUE) return fallback;
    char buf[8192];
    DWORD got = 0;
    BOOL ok = ReadFile(h, buf, sizeof(buf) - 1, &got, nullptr);
    CloseHandle(h);
    if (!ok) return fallback;
    std::string text(buf, got);
    size_t r = text.find("\"report\"");
    size_t k = r == std::string::npos ? r : text.find("\"path\"", r);
    size_t q1 = k == std::string::npos ? k : text.find('"', text.find(':', k) + 1);
    size_t q2 = q1 == std::string::npos ? q1 : text.find('"', q1 + 1);
    if (q2 == std::string::npos) return fallback;
    std::string rel = text.substr(q1 + 1, q2 - q1 - 1);
    // The .pbip is UTF-8: a folder name with accents or in another script reads back as written.
    int n = MultiByteToWideChar(CP_UTF8, 0, rel.data(), (int)rel.size(), nullptr, 0);
    std::wstring wrel(n > 0 ? n : 0, L'\0');
    if (n > 0) MultiByteToWideChar(CP_UTF8, 0, rel.data(), (int)rel.size(), wrel.data(), n);
    for (auto& c : wrel) if (c == L'/') c = L'\\';
    return wrel.empty() ? fallback : dir + wrel;
}

static bool EndsWithPbip(const wchar_t* s, size_t n) { return n > 5 && _wcsicmp(s + n - 5, L".pbip") == 0; }

// A .Report folder from a path to one or to a .pbip (quotes and trailing slashes allowed), made
// absolute and normalized, without a trailing slash; empty for an empty path.
static std::wstring ReportFolder(std::wstring path) {
    while (!path.empty() && (path.back() == L' ' || path.back() == L'"' || path.back() == L'\\' || path.back() == L'/')) path.pop_back();
    while (!path.empty() && (path.front() == L' ' || path.front() == L'"')) path.erase(path.begin());
    if (path.empty()) return L"";
    if (EndsWithPbip(path.c_str(), path.size())) path = PbipReportFolder(path);
    wchar_t full[1024];
    DWORD k = GetFullPathNameW(path.c_str(), 1024, full, nullptr);
    if (k && k < 1024) path.assign(full, k);
    while (!path.empty() && (path.back() == L'\\' || path.back() == L'/')) path.pop_back();
    return path;
}

// The report's files as the JSON object the page parses, { root, files }; empty when none.
static std::wstring ProjectObject(const std::wstring& path) {
    if (path.empty()) return L"";
    std::wstring files;
    size_t total = 0;
    int count = 0;
    // Only what the tools read, as the development injector does.
    ProjectWalk(path, L"/definition", files, total, count);
    ProjectWalk(path, L"/StaticResources", files, total, count);
    if (!count) { Wh_Log(L"project: no report files under %s", path.c_str()); return L""; }
    std::wstring rootJson;
    AppendJsonString(rootJson, path);
    return L"{\"root\":" + rootJson + L",\"files\":[" + files + L"]}";
}

// Handed over as one string that only the top-level page parses: every other frame (custom visual
// sandboxes too) leaves the script's function before this line, and a string literal costs those
// frames far less to compile than a multi-megabyte object literal would.
static std::wstring ProjectScriptFrom(const std::wstring& object) {
    if (object.empty()) return L"";
    std::wstring s = L"var __PBT_PROJECT_JSON__ = ";
    AppendJsonString(s, object);
    s += L";\n";
    return s;
}

// The files for the report Power BI has open, else for the "project" setting; notes the folder
// read, so that a save into it is seen.
// Tried in order: the folder seen last, the folder read last (a save may write through a folder
// that is gone by the time it is read, or be mid-move), the command line's report. `seen` reports
// what was tried first, for the page's log.
static std::wstring CurrentProjectObject(std::wstring* seenOut = nullptr, bool* seenOk = nullptr) {
    std::wstring pbip, seen, last;
    bool pbix;
    {
        std::lock_guard<std::mutex> lock(g_reportLock);
        pbip = g_reportPbip;
        seen = g_reportSeen;
        last = g_reportRoot;
        pbix = g_reportPbix;
    }
    if (seenOut) *seenOut = seen;
    // A folder that came from the setting is not a report Power BI showed: the setting is read
    // again instead, so a changed or cleared setting takes effect.
    bool fromSetting;
    {
        std::lock_guard<std::mutex> lock(g_reportLock);
        fromSetting = g_rootFromSetting;
    }
    if (fromSetting) last.clear();
    std::wstring root, object;
    for (const std::wstring* c : {&seen, &last, &pbip}) {
        if (c->empty()) continue;
        root = ReportFolder(*c);
        object = ProjectObject(root);
        if (!object.empty()) { if (seenOk) *seenOk = c == &seen; break; }
    }
    bool viaSetting = false;
    if (object.empty() && seen.empty() && last.empty() && pbip.empty() && !pbix) {
        root = ReportFolder(WindhawkUtils::StringSetting::make(L"project").get());
        object = ProjectObject(root);
        viaSetting = true;
    }
    std::lock_guard<std::mutex> lock(g_reportLock);
    if (!object.empty()) {
        g_reportRoot = root + L"\\";
        g_rootFromSetting = viaSetting;
    } else if (viaSetting) {
        g_reportRoot.clear();
        g_rootFromSetting = false;
    }
    return object;
}

// Power BI's own data folders (%LOCALAPPDATA%\Microsoft\Power BI Desktop, with TempSaves and
// AutoRecovery, or the Store app's ...\Microsoft\Power BI Desktop Store App) hold copies of
// reports that are never the report open in the window.
static bool InPowerBiData(const wchar_t* p, size_t k) {
    static const wchar_t kDir[] = L"\\microsoft\\power bi desktop";
    const size_t m = ARRAYSIZE(kDir) - 1;
    for (size_t i = 0; i + m <= k; i++) if (_wcsnicmp(p + i, kDir, m) == 0) return true;
    return false;
}

// Calls still running inside this module's hooks (a CreateFileW can block for seconds on a slow
// or online-only path) or its hand-over windows; Wh_ModUninit waits for them before the image may go.
static std::atomic<long> g_hookCalls{0};
struct HookCall {
    HookCall() { g_hookCalls++; }
    ~HookCall() { g_hookCalls--; }
};

// The .pbip on Power BI's command line (a double-clicked report, or "Open with"); `pbix` tells
// whether a .pbix was named there instead.
static std::wstring PbipFromCommandLine(bool& pbix) {
    std::wstring found, arg;
    bool quoted = false;
    pbix = false;
    for (const wchar_t* c = GetCommandLineW();; c++) {
        if (*c == L'"') { quoted = !quoted; continue; }
        if (!*c || (!quoted && (*c == L' ' || *c == L'\t'))) {
            if (EndsWithPbip(arg.c_str(), arg.size())) found = arg;
            else if (arg.size() > 5 && _wcsicmp(arg.c_str() + arg.size() - 5, L".pbix") == 0) pbix = true;
            arg.clear();
            if (!*c) break;
            continue;
        }
        arg += *c;
    }
    if (found.empty()) return L"";
    wchar_t full[1024];
    DWORD k = GetFullPathNameW(found.c_str(), 1024, full, nullptr);
    return k && k < 1024 ? std::wstring(full, k) : found;
}

static void ScheduleProjectRead(DWORD delayMs) {
    g_dueAt = GetTickCount64() + delayMs;
    if (g_wake) SetEvent(g_wake);
}

// From the CreateFileW and move hooks, on whichever Power BI thread opened or moved the file: one
// suffix test for nearly every file, and no reading here. A .Report folder's definition.pbir,
// read, written or moved there, makes that folder the report; a write or move under the report's
// .Report folder (a save) asks for the files again. Both are read 1.5 s later, after the last one.
static void NoteFile(LPCWSTR name, bool writing) {
    static const wchar_t kPbir[] = L"definition.pbir";
    const size_t m = ARRAYSIZE(kPbir) - 1;
    size_t n = wcslen(name);
    bool pbir = n > m && _wcsicmp(name + n - m, kPbir) == 0 && (name[n - m - 1] == L'\\' || name[n - m - 1] == L'/');
    if (!pbir && !writing) return;
    wchar_t full[1024];
    DWORD k = GetFullPathNameW(name, 1024, full, nullptr);
    if (!k || k >= 1024 || k <= m) return;
    const wchar_t* p = full;
    if (k > 6 && wcsncmp(p, L"\\\\?\\", 4) == 0 && p[5] == L':') { p += 4; k -= 4; }
    bool read;
    {
        std::lock_guard<std::mutex> lock(g_reportLock);
        // A save is written under Power BI's TempSaves folder first and then moved into place (the
        // move hooks note where it lands), and AutoRecovery keeps copies there too: those only ask
        // for a read of the report already known, they are never taken for the report.
        // A copy under the user's temp folder is a staging copy too, never the report. Any other
        // folder is (a .pbip may name its report folder freely, not only *.Report).
        bool staging = InPowerBiData(p, k) || (!g_tempDir.empty() && k > g_tempDir.size() && _wcsnicmp(p, g_tempDir.c_str(), g_tempDir.size()) == 0);
        // Files moved after a Save As's definition.pbir land under the new folder, which the
        // reader has not read yet: they count as writes under the report too, so the read waits
        // for the last of them.
        auto under = [&](const std::wstring& root) { return !root.empty() && k > root.size() && _wcsnicmp(p, root.c_str(), root.size()) == 0; };
        // The prefix strip above shortens the path, so a definition.pbir is checked again here.
        if (pbir && k > m) { if (!staging) g_reportSeen.assign(p, k - m); read = true; }  // the folder, with its trailing '\'
        else read = under(g_reportRoot) || under(g_reportSeen);
    }
    if (read) ScheduleProjectRead(1500);
}

using CreateFileW_t = HANDLE(WINAPI*)(LPCWSTR, DWORD, DWORD, LPSECURITY_ATTRIBUTES, DWORD, DWORD, HANDLE);
CreateFileW_t CreateFileW_Original;
HANDLE WINAPI CreateFileW_Hook(LPCWSTR name, DWORD access, DWORD share, LPSECURITY_ATTRIBUTES sa, DWORD disposition, DWORD flags, HANDLE templ) {
    HookCall in;
    HANDLE h = CreateFileW_Original(name, access, share, sa, disposition, flags, templ);
    // The reader's own opens are skipped, or reading a .pbip would ask for another read. A disable
    // sets g_stopWorker first, so nothing is noted from then on.
    if (h != INVALID_HANDLE_VALUE && name && !g_stopWorker && GetCurrentThreadId() != g_workerId) {
        // CreateFileW sets the last error on success too (ERROR_ALREADY_EXISTS); the caller gets it back as it was.
        DWORD err = GetLastError();
        NoteFile(name, (access & (GENERIC_WRITE | GENERIC_ALL | FILE_WRITE_DATA | FILE_APPEND_DATA)) != 0);
        SetLastError(err);
    }
    return h;
}

// Every save, Save As included, is written under Power BI's TempSaves folder and then moved into
// place file by file (MoveFileWithProgressW, measured on Power BI 2.159), so CreateFileW never sees
// the final folder's definition.pbir. The move's destination is noted as a file written there: a
// .Report folder's definition.pbir landing outside Power BI's own data folders makes that folder
// the report (a Save As to a new folder), and the files moved after it ask for the read again.
// MoveFileExW is watched too, for a Power BI that moves its files with that call instead.
// A move that waits for a restart (MOVEFILE_DELAY_UNTIL_REBOOT) has not landed and is not noted. A
// whole .Report folder moved into place is noted as its definition.pbir.
static void NoteMove(LPCWSTR to, DWORD flags) {
    if (!to || (flags & MOVEFILE_DELAY_UNTIL_REBOOT) || g_stopWorker || GetCurrentThreadId() == g_workerId) return;
    DWORD err = GetLastError();
    size_t n = wcslen(to);
    while (n && (to[n - 1] == L'\\' || to[n - 1] == L'/')) n--;
    if (n > 7 && _wcsnicmp(to + n - 7, L".Report", 7) == 0) {
        DWORD a = GetFileAttributesW(to);
        if (a != INVALID_FILE_ATTRIBUTES && (a & FILE_ATTRIBUTE_DIRECTORY)) {
            std::wstring pbir(to, n);
            pbir += L"\\definition.pbir";
            NoteFile(pbir.c_str(), true);
            SetLastError(err);
            return;
        }
    }
    NoteFile(to, true);
    SetLastError(err);
}

using MoveFileWithProgressW_t = BOOL(WINAPI*)(LPCWSTR, LPCWSTR, LPPROGRESS_ROUTINE, LPVOID, DWORD);
MoveFileWithProgressW_t MoveFileWithProgressW_Original;
BOOL WINAPI MoveFileWithProgressW_Hook(LPCWSTR from, LPCWSTR to, LPPROGRESS_ROUTINE progress, LPVOID data, DWORD flags) {
    HookCall in;
    BOOL ok = MoveFileWithProgressW_Original(from, to, progress, data, flags);
    if (ok) NoteMove(to, flags);
    return ok;
}

using MoveFileExW_t = BOOL(WINAPI*)(LPCWSTR, LPCWSTR, DWORD);
MoveFileExW_t MoveFileExW_Original;
BOOL WINAPI MoveFileExW_Hook(LPCWSTR from, LPCWSTR to, DWORD flags) {
    HookCall in;
    BOOL ok = MoveFileExW_Original(from, to, flags);
    if (ok) NoteMove(to, flags);
    return ok;
}

static void LoadSettings() {
    std::wstring cheer = CheerScript();
    auto skin = WindhawkUtils::StringSetting::make(L"skin");
    std::wstring s = L"{\"skin\":\"";
    for (const wchar_t* c = skin.get(); *c; c++) if (iswalnum(*c) || *c == L'-' || *c == L'_') s += *c;
    auto flag = [](PCWSTR name) { return Wh_GetIntSetting(name) ? L"true" : L"false"; };
    s += L"\",\"features\":{";
    s += L"\"splash\":";           s += flag(L"splash");
    s += L",\"notes\":";           s += flag(L"notes");
    s += L",\"confetti\":";        s += flag(L"confetti");
    s += L",\"sounds\":";          s += flag(L"sounds");
    s += L",\"visualCount\":";     s += flag(L"visualCount");
    s += L",\"surprises\":";       s += flag(L"surprises");
    s += L",\"reportFilterPane\":"; s += flag(L"keepReportFilterColors");
    // The palette for Colors: hex codes in any separators, with or without '#'. Anything
    // else is dropped; an empty list leaves the page's default palette in place.
    auto pal = WindhawkUtils::StringSetting::make(L"palette");
    std::wstring palJson, tok;
    auto flush = [&]() {
        if (tok.size() == 4 || tok.size() == 7) { if (!palJson.empty()) palJson += L","; palJson += L"\"" + tok + L"\""; }
        tok.clear();
    };
    for (const wchar_t* c = pal.get(); *c; c++) {
        if (*c == L'#') { flush(); tok = L"#"; }
        else if (iswxdigit(*c)) { if (tok.empty()) tok = L"#"; tok += *c; }
        else flush();
    }
    flush();
    int tol = Wh_GetIntSetting(L"tolerance");
    if (tol < 0) tol = 0;
    if (tol > 128) tol = 128;
    int grid = Wh_GetIntSetting(L"grid");
    if (grid < 1) grid = 8;
    if (grid > 200) grid = 200;
    int nearMiss = Wh_GetIntSetting(L"nearMiss");
    if (nearMiss < 1) nearMiss = 8;
    if (nearMiss > 100) nearMiss = 100;
    s += L"},\"qa\":{\"grid\":" + std::to_wstring(grid) + L",\"nearMiss\":" + std::to_wstring(nearMiss) + L",\"snap\":" + flag(L"snap");
    s += L"},\"lint\":{\"tolerance\":" + std::to_wstring(tol);
    // A palette typed here wins over the report theme's colors (from the project folder).
    if (!palJson.empty()) s += L",\"palette\":[" + palJson + L"],\"fromTheme\":false";
    s += L"}}";
    std::lock_guard<std::mutex> lock(g_lock);
    g_settingsJson = s;
    g_cheerJs = cheer;
}

// WebView2 runs the script in every frame of every page the app opens: custom visual sandboxes,
// sign-in pages, anything embedded. All of it, the settings and project files included, sits in
// one function that returns at once unless this is the top-level page of Power BI's own origin
// (its views are https://ms-pbi.pbi.microsoft.com/minerva/...), so no other frame gets a global,
// the report's files or any of the code.
static const wchar_t kPowerBiOrigin[] = L"https://ms-pbi.pbi.microsoft.com";

static std::wstring ScriptForNewPage() {
    std::wstring settings, cheer, project;
    {
        std::lock_guard<std::mutex> lock(g_lock);
        settings = g_settingsJson;
        cheer = g_cheerJs;
        project = g_projectJs;
    }
    std::wstring s = L"(function () {\nif (window.top !== window || location.origin !== '";
    s += kPowerBiOrigin;
    s += L"') return;\n";
    s += L"var __PBT_SETTINGS__ = " + settings + L";\n";
    s += cheer;
    s += project;
    s += kPayload;
    s += L"\n})();\n";
    return s;
}

// What disabling the mod runs in each page it gave the script to: the page's own teardown
// (pbt.remove in page/00-core.js, exposed as window.__pbt), which undoes every skin and tool.
static const wchar_t kTeardown[] = L"window.__pbt && typeof window.__pbt.remove === 'function' && window.__pbt.remove();";

// ---- a minimal COM object for the completion and event handlers ----
// Each handler is IUnknown + Invoke(two arguments); `invoke` receives them.
struct Handler {
    void** vtbl;
    std::atomic<ULONG> refs{1};
    GUID iid;
    IUnknown* inner;  // the app's own handler, when wrapping one
    HRESULT (*invoke)(Handler*, void*, void*);
    void* ctx;        // what `invoke` works on: the controller's parent window, a tracked web view
};

static HRESULT STDMETHODCALLTYPE H_QueryInterface(Handler* self, REFIID riid, void** out) {
    if (IsEqualGUID(riid, kIID_IUnknown) || IsEqualGUID(riid, self->iid)) {
        *out = self;
        self->refs++;
        return S_OK;
    }
    *out = nullptr;
    return E_NOINTERFACE;
}
// Handlers WebView2 still holds (their code is in this module); Wh_ModUninit waits for them to go.
static std::atomic<long> g_liveHandlers{0};
static ULONG STDMETHODCALLTYPE H_AddRef(Handler* self) { return ++self->refs; }
static ULONG STDMETHODCALLTYPE H_Release(Handler* self) {
    ULONG n = --self->refs;
    if (!n) {
        if (self->inner) self->inner->Release();
        delete self;
        g_liveHandlers--;
    }
    return n;
}
static HRESULT STDMETHODCALLTYPE H_Invoke(Handler* self, void* a, void* b) { return self->invoke(self, a, b); }
static void* g_handlerVtbl[] = {(void*)H_QueryInterface, (void*)H_AddRef, (void*)H_Release, (void*)H_Invoke};

static IUnknown* MakeHandler(const GUID& iid, IUnknown* inner, HRESULT (*invoke)(Handler*, void*, void*), void* ctx = nullptr) {
    auto* h = new Handler();
    h->vtbl = g_handlerVtbl;
    h->iid = iid;
    h->inner = inner;
    h->invoke = invoke;
    h->ctx = ctx;
    if (inner) inner->AddRef();
    g_liveHandlers++;
    return reinterpret_cast<IUnknown*>(h);
}

// Forward a completion to the handler we wrapped.
static HRESULT Forward(Handler* self, void* a, void* b) {
    using Invoke_t = HRESULT(STDMETHODCALLTYPE*)(IUnknown*, void*, void*);
    return self->inner ? Slot<Invoke_t>(self->inner, 3)(self->inner, a, b) : S_OK;
}

// ---- the web views given the script, so that disabling the mod can take it back out ----
// Each keeps one reference to its ICoreWebView2, the id AddScriptToExecuteOnDocumentCreated
// reported, and the thread and window it was made on: WebView2 refuses calls from any thread but
// the one that created it, and a message sent to that window reaches that thread.
// A record is freed only on its web view's thread, by TakeBack or by OnScriptAdded, never on
// Windhawk's thread: a registration still pending when the mod is disabled has its callback to come.
struct Tracked {
    IUnknown* webview;
    HWND window;
    DWORD thread;
    std::wstring scriptId;
    bool settled = false;     // OnScriptAdded has run (the id, or a failure, is known)
    bool handedOver = false;  // TakeBack ran first and left the rest to OnScriptAdded
    HWND push = nullptr;      // a message-only window of this thread: the reader posts new report files to it
};
static std::vector<Tracked*> g_tracked;  // under g_lock

// ---- the reader's hand-overs: posted, never sent ----
// The reader never waits on Power BI's threads: it posts each web view's new files to a
// message-only window this mod made on that web view's thread, and the thread runs the work from its
// own message loop, where any posted message is handled. The window class itself uses the
// system's DefWindowProcW, and each window gets this module's procedure only once it exists: a
// window or class left behind after a disable then never points into code that is gone (see
// Wh_ModUninit), and a later copy of the mod that finds the class still registered makes working
// windows from it all the same.
static const wchar_t kPushClass[] = L"pbid-tweaks report hand-over";
constexpr UINT kPushMsg = WM_APP + 0x5A1;
// Every hand-over window made, for Wh_ModUninit to find any still there. Windows go with
// DropPushWindow, or with their thread when it ends; a window taken down with its thread gets no
// WM_NCDESTROY, so Wh_ModUninit checks each handle for a window that still exists.
static std::mutex g_pushLock;
static std::vector<HWND> g_pushList;
static LRESULT CALLBACK PushWndProc(HWND w, UINT msg, WPARAM wp, LPARAM lp);
static HWND MakePushWindow() {
    HMODULE self = nullptr;
    GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, reinterpret_cast<LPCWSTR>(&PushWndProc), &self);
    WNDCLASSW wc = {};
    wc.lpfnWndProc = DefWindowProcW;
    wc.hInstance = self;
    wc.lpszClassName = kPushClass;
    RegisterClassW(&wc);  // fails harmlessly when the class is registered already
    HWND w = CreateWindowExW(0, kPushClass, L"", 0, 0, 0, 0, 0, HWND_MESSAGE, nullptr, self, nullptr);
    if (w) {
        SetWindowLongPtrW(w, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(PushWndProc));
        std::lock_guard<std::mutex> lock(g_pushLock);
        g_pushList.push_back(w);
    }
    return w;
}
// Set by Wh_ModBeforeUninit, in the same g_lock block that takes g_tracked: from then on no web
// view is given the script and no new handler is made, so nothing can join the list too late.
static std::atomic<bool> g_unloading{false};

using RemoveScript_t = HRESULT(STDMETHODCALLTYPE*)(IUnknown*, LPCWSTR);
// On the web view's thread: no script for the documents to come, and the record goes.
static void Retire(Tracked* t) {
    if (!t->scriptId.empty()) Slot<RemoveScript_t>(t->webview, kWebViewRemoveScriptOnDocumentCreated)(t->webview, t->scriptId.c_str());
    t->webview->Release();
    delete t;
}

static HRESULT OnScriptAdded(Handler* self, void* hr, void* id) {
    auto* t = static_cast<Tracked*>(self->ctx);
    if (FAILED((HRESULT)(INT_PTR)hr)) Wh_Log(L"AddScriptToExecuteOnDocumentCreated failed: 0x%08X", (unsigned)(INT_PTR)hr);
    bool release;
    {
        std::lock_guard<std::mutex> lock(g_lock);
        if (SUCCEEDED((HRESULT)(INT_PTR)hr) && id) t->scriptId = static_cast<LPCWSTR>(id);
        t->settled = true;
        release = t->handedOver;
    }
    // The mod was disabled while this registration was pending: TakeBack has run the page's
    // teardown and left the script and the record to this moment.
    if (release) Retire(t);
    return S_OK;
}

// ---- a controller exists: give its web view the skins ----
// Runs in the controller's completion handler, on the web view's own thread.
static void Attach(IUnknown* controller, HWND window) {
    using GetCoreWebView2_t = HRESULT(STDMETHODCALLTYPE*)(IUnknown*, IUnknown**);
    using AddScript_t = HRESULT(STDMETHODCALLTYPE*)(IUnknown*, LPCWSTR, IUnknown*);
    IUnknown* webview = nullptr;
    if (FAILED(Slot<GetCoreWebView2_t>(controller, kCtrlGetCoreWebView2)(controller, &webview)) || !webview) return;
    std::wstring script = ScriptForNewPage();
    // The record keeps the reference GetCoreWebView2 returned; TakeBack or OnScriptAdded releases it.
    auto* t = new Tracked{webview, window, GetCurrentThreadId(), L""};
    // Made before the record joins the list, so the reader never finds it without its window.
    t->push = MakePushWindow();
    bool unloading;
    {
        // Tested under the lock: a disable that started after OnControllerCreated looked has either
        // taken the list already (then this view is left alone) or will take this record with it.
        std::lock_guard<std::mutex> lock(g_lock);
        unloading = g_unloading;
        if (!unloading) g_tracked.push_back(t);
    }
    if (unloading) {
        if (t->push) DestroyWindow(t->push);  // its WM_NCDESTROY takes it off the list
        webview->Release();
        delete t;
        return;
    }
    IUnknown* done = MakeHandler(IID_AddScriptCompleted, nullptr, OnScriptAdded, t);
    Slot<AddScript_t>(webview, kWebViewAddScriptOnDocumentCreated)(webview, script.c_str(), done);
    done->Release();
}

static HRESULT OnControllerCreated(Handler* self, void* hr, void* controller) {
    if (SUCCEEDED((HRESULT)(INT_PTR)hr) && controller && !g_unloading) Attach(static_cast<IUnknown*>(controller), static_cast<HWND>(self->ctx));
    return Forward(self, hr, controller);
}

// ---- the environment's controller factories ----
using CreateController_t = HRESULT(STDMETHODCALLTYPE*)(IUnknown*, HWND, IUnknown*);
using CreateControllerWithOptions_t = HRESULT(STDMETHODCALLTYPE*)(IUnknown*, HWND, IUnknown*, IUnknown*);
CreateController_t CreateController_Original;
CreateControllerWithOptions_t CreateControllerWithOptions_Original;

// While the mod is being disabled, the app's own handler goes through untouched, so WebView2 is
// handed no new object of this module to hold.
HRESULT STDMETHODCALLTYPE CreateController_Hook(IUnknown* env, HWND parent, IUnknown* handler) {
    if (g_unloading) return CreateController_Original(env, parent, handler);
    IUnknown* wrapped = MakeHandler(IID_CtrlCompleted, handler, OnControllerCreated, parent);
    HRESULT hr = CreateController_Original(env, parent, wrapped);
    wrapped->Release();
    return hr;
}
HRESULT STDMETHODCALLTYPE CreateControllerWithOptions_Hook(IUnknown* env, HWND parent, IUnknown* options, IUnknown* handler) {
    if (g_unloading) return CreateControllerWithOptions_Original(env, parent, options, handler);
    IUnknown* wrapped = MakeHandler(IID_CtrlCompleted, handler, OnControllerCreated, parent);
    HRESULT hr = CreateControllerWithOptions_Original(env, parent, options, wrapped);
    wrapped->Release();
    return hr;
}

static std::atomic<bool> g_envHooked{false};
static HRESULT OnEnvironmentCreated(Handler* self, void* hr, void* envPtr) {
    IUnknown* env = static_cast<IUnknown*>(envPtr);
    if (SUCCEEDED((HRESULT)(INT_PTR)hr) && env && !g_unloading && !g_envHooked.exchange(true)) {
        // Every environment shares one implementation, so hooking its methods once covers all.
        WindhawkUtils::SetFunctionHook(Slot<CreateController_t>(env, kEnvCreateController), CreateController_Hook, &CreateController_Original);
        IUnknown* env10 = nullptr;
        if (SUCCEEDED(env->QueryInterface(IID_Environment10, (void**)&env10)) && env10) {
            WindhawkUtils::SetFunctionHook(Slot<CreateControllerWithOptions_t>(env10, kEnv10CreateControllerWithOptions), CreateControllerWithOptions_Hook, &CreateControllerWithOptions_Original);
            env10->Release();
        }
        Wh_ApplyHookOperations();
    }
    return Forward(self, hr, envPtr);
}

// ---- WebView2Loader's environment factory, hooked when the loader loads ----
using CreateEnv_t = HRESULT(STDAPICALLTYPE*)(PCWSTR, PCWSTR, IUnknown*, IUnknown*);
CreateEnv_t CreateEnv_Original;
HRESULT STDAPICALLTYPE CreateEnv_Hook(PCWSTR browserFolder, PCWSTR userDataFolder, IUnknown* options, IUnknown* handler) {
    if (g_unloading) return CreateEnv_Original(browserFolder, userDataFolder, options, handler);
    IUnknown* wrapped = MakeHandler(IID_EnvCompleted, handler, OnEnvironmentCreated);
    HRESULT hr = CreateEnv_Original(browserFolder, userDataFolder, options, wrapped);
    wrapped->Release();
    return hr;
}

static std::atomic<bool> g_loaderHooked{false};
static void HookLoader(HMODULE loader, bool apply) {
    if (!loader || g_loaderHooked.exchange(true)) return;
    auto target = reinterpret_cast<CreateEnv_t>(GetProcAddress(loader, "CreateCoreWebView2EnvironmentWithOptions"));
    if (!target) {
        Wh_Log(L"CreateCoreWebView2EnvironmentWithOptions not found in WebView2Loader.dll");
        return;
    }
    WindhawkUtils::SetFunctionHook(target, CreateEnv_Hook, &CreateEnv_Original);
    if (apply) Wh_ApplyHookOperations();
}

using LoadLibraryExW_t = HMODULE(WINAPI*)(LPCWSTR, HANDLE, DWORD);
LoadLibraryExW_t LoadLibraryExW_Original;
HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR name, HANDLE file, DWORD flags) {
    HookCall in;
    HMODULE m = LoadLibraryExW_Original(name, file, flags);
    if (m && name && !g_loaderHooked && !g_unloading) {
        const wchar_t* base = wcsrchr(name, L'\\');
        base = base ? base + 1 : name;
        if (_wcsicmp(base, L"WebView2Loader.dll") == 0) HookLoader(m, true);
    }
    return m;
}

static DWORD WINAPI ProjectWorker(void*);

BOOL Wh_ModInit() {
    // A fresh image starts with these false; they are set here all the same, so a start never
    // depends on how an earlier copy of the mod ended.
    g_unloading = false;
    g_loaderHooked = false;
    g_envHooked = false;
    g_stopWorker = false;
    g_dueAt = 0;
    {
        // Long form, so it compares with the full paths CreateFileW and the moves are given.
        wchar_t tmp[MAX_PATH + 1], full[1024];
        DWORD tn = GetTempPathW(MAX_PATH + 1, tmp);
        DWORD fn = tn && tn <= MAX_PATH ? GetLongPathNameW(tmp, full, 1024) : 0;
        std::lock_guard<std::mutex> lock(g_reportLock);
        g_tempDir = fn && fn < 1024 ? std::wstring(full, fn) : tn && tn <= MAX_PATH ? std::wstring(tmp, tn) : L"";
        if (!g_tempDir.empty() && g_tempDir.back() != L'\\') g_tempDir += L'\\';
    }
    {
        // A report opened before the mod was switched on, other than the one Power BI was started
        // with, is found at its next save or open.
        bool pbix;
        std::wstring pbip = PbipFromCommandLine(pbix);
        std::lock_guard<std::mutex> lock(g_reportLock);
        g_reportPbip = pbip;
        g_reportPbix = pbix;
        g_reportSeen.clear();
        g_reportRoot.clear();
    }
    LoadSettings();
    // Read here, before any web view exists and before the hooks are on, so the first page has it.
    std::wstring object = CurrentProjectObject();
    {
        std::lock_guard<std::mutex> lock(g_lock);
        g_projectObject = object;
        g_projectJs = ProjectScriptFrom(object);
    }
    g_wake = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    g_worker = g_wake ? CreateThread(nullptr, 0, ProjectWorker, nullptr, 0, &g_workerId) : nullptr;
    HMODULE already = GetModuleHandleW(L"WebView2Loader.dll");
    if (already) HookLoader(already, false);
    HMODULE kernelBase = GetModuleHandleW(L"kernelbase.dll");
    WindhawkUtils::SetFunctionHook(reinterpret_cast<LoadLibraryExW_t>(GetProcAddress(kernelBase, "LoadLibraryExW")), LoadLibraryExW_Hook, &LoadLibraryExW_Original);
    if (g_worker) {
        WindhawkUtils::SetFunctionHook(reinterpret_cast<CreateFileW_t>(GetProcAddress(kernelBase, "CreateFileW")), CreateFileW_Hook, &CreateFileW_Original);
        WindhawkUtils::SetFunctionHook(reinterpret_cast<MoveFileWithProgressW_t>(GetProcAddress(kernelBase, "MoveFileWithProgressW")), MoveFileWithProgressW_Hook, &MoveFileWithProgressW_Original);
        WindhawkUtils::SetFunctionHook(reinterpret_cast<MoveFileExW_t>(GetProcAddress(kernelBase, "MoveFileExW")), MoveFileExW_Hook, &MoveFileExW_Original);
    }
    return TRUE;
}

// New settings win over the choice remembered inside Power BI (it is tagged with the settings it
// was made under) and apply to pages opened from now on: the readme asks for a restart of Power BI,
// because the open views keep the script they were given. The report's files are the exception:
// a changed "project" setting reaches the open pages at once, through the reader.
void Wh_ModSettingsChanged() {
    LoadSettings();
    ScheduleProjectRead(0);
}

// ---- disabling the mod: undo the page and take the script back out ----
// Windhawk calls Wh_ModBeforeUninit on a thread of its own, while the hooks are still on, and
// WebView2 only answers the thread that made each web view. A thread-local WH_CALLWNDPROC hook sees
// a message sent to one of that thread's windows and runs the work there before the window gets
// it; SendMessage returns once the work is done, and the hook goes again. A plain SendMessage
// waits as long as that thread is busy: the work item lives on this stack, so nothing runs after
// it is gone, and Windhawk's thread holds no lock a Power BI thread could be waiting for.
struct ThreadWork {
    void (*fn)(void*);
    void* arg;
    bool ran;
};
static UINT g_workMsg;
static LRESULT CALLBACK WorkHook(int code, WPARAM wParam, LPARAM lParam) {
    if (code == HC_ACTION) {
        const CWPSTRUCT* m = reinterpret_cast<const CWPSTRUCT*>(lParam);
        if (g_workMsg && m->message == g_workMsg) {
            auto* w = reinterpret_cast<ThreadWork*>(m->lParam);
            w->fn(w->arg);
            w->ran = true;
        }
    }
    return CallNextHookEx(nullptr, code, wParam, lParam);
}
// True only when the work really ran: a window destroyed after it was found takes the message
// with it, and SendMessage then returns without the hook ever seeing it.
static bool RunOnWindowThread(HWND window, void (*fn)(void*), void* arg) {
    DWORD thread = GetWindowThreadProcessId(window, nullptr);
    if (!thread) return false;
    if (thread == GetCurrentThreadId()) { fn(arg); return true; }
    if (!g_workMsg) g_workMsg = RegisterWindowMessageW(L"pbid-tweaks: run on the web view thread");
    if (!g_workMsg) return false;
    HHOOK hook = SetWindowsHookExW(WH_CALLWNDPROC, WorkHook, nullptr, thread);
    if (!hook) return false;
    ThreadWork work{fn, arg, false};
    SendMessageW(window, g_workMsg, 0, reinterpret_cast<LPARAM>(&work));
    UnhookWindowsHookEx(hook);
    return work.ran;
}

// On the web view's own thread: the page's teardown in the open document, then no script for the
// documents to come. ExecuteScript gets no completion handler, because this mod's code is unloaded
// before the page could answer. A registration still pending leaves the script and the record to
// OnScriptAdded, on this same thread.
struct ProjectWork;
static void DropPushWindow(Tracked* t);
static void TakeBack(void* arg) {
    using ExecuteScript_t = HRESULT(STDMETHODCALLTYPE*)(IUnknown*, LPCWSTR, IUnknown*);
    auto* t = static_cast<Tracked*>(arg);
    DropPushWindow(t);
    Slot<ExecuteScript_t>(t->webview, kWebViewExecuteScript)(t->webview, kTeardown, nullptr);
    {
        std::lock_guard<std::mutex> lock(g_lock);
        if (!t->settled) { t->handedOver = true; return; }
    }
    Retire(t);
}

static BOOL CALLBACK FirstWindow(HWND window, LPARAM out) {
    *reinterpret_cast<HWND*>(out) = window;
    return FALSE;
}

// Whether `window` still exists, belongs to `thread` and is one of this mod's hand-over windows
// (Windows may give a gone window's handle to a new one).
static bool IsPushWindowOf(HWND window, DWORD thread) {
    wchar_t cls[64];
    return window && IsWindow(window) && GetWindowThreadProcessId(window, nullptr) == thread &&
           GetClassNameW(window, cls, 64) && wcscmp(cls, kPushClass) == 0;
}

// The window to reach a tracked web view's thread through. First the web view's own hand-over
// window: it is this mod's window on exactly that thread, so it reaches the thread for as long as
// it exists, also when the thread has no top-level window left (EnumThreadWindows never lists a
// message-only window), and the work message never goes to a window of Power BI's. TakeBack then
// destroys that same window from inside the thread hook, before its procedure would see the work
// message; the work has run by then (RunOnWindowThread reports it), and the procedure is still
// loaded during Wh_ModBeforeUninit, so a message delivered to the window after that is harmless.
// Else the window its controller was made for, else any other window of the same thread (the web
// view may have moved to a new parent).
static HWND WindowOf(Tracked* t) {
    if (IsPushWindowOf(t->push, t->thread)) return t->push;
    HWND window = t->window;
    if (!IsWindow(window) || GetWindowThreadProcessId(window, nullptr) != t->thread) {
        window = nullptr;
        EnumThreadWindows(t->thread, FirstWindow, reinterpret_cast<LPARAM>(&window));
    }
    return window;
}

// ---- a report opened or saved while Power BI runs: its files reach the open pages ----
// What the open page runs: pbt.project.set (page/05-project.js) swaps the files and redraws the
// tools that read them. Only the top-level page of Power BI's own origin takes them.
// `seen` (the folder whose definition.pbir started this read) goes to the page's log only.
static std::wstring PushScript(const std::wstring& object, const std::wstring& seen) {
    std::wstring s = L"(function () { if (window.top !== window || location.origin !== '";
    s += kPowerBiOrigin;
    s += L"') return; var p = window.__pbt && window.__pbt.project; if (!p || typeof p.set !== 'function') return; try { p.set(JSON.parse(";
    AppendJsonString(s, object.empty() ? std::wstring(L"null") : object);
    s += L"), { seen: ";
    AppendJsonString(s, seen);
    s += L" }); } catch (e) {} })();";
    return s;
}

struct ProjectPush {
    Tracked* t;
    const std::wstring* script;  // the whole script for documents to come
    const std::wstring* push;    // the files for the document open now
};
// On the web view's own thread: the documents to come get the new files (the registration is
// replaced once the last one has settled), and the open document gets them at once.
static void PushOnThread(void* arg) {
    using AddScript_t = HRESULT(STDMETHODCALLTYPE*)(IUnknown*, LPCWSTR, IUnknown*);
    using ExecuteScript_t = HRESULT(STDMETHODCALLTYPE*)(IUnknown*, LPCWSTR, IUnknown*);
    auto* p = static_cast<ProjectPush*>(arg);
    Tracked* t = p->t;
    std::wstring old;
    bool again;
    {
        std::lock_guard<std::mutex> lock(g_lock);
        if (g_unloading) return;
        again = t->settled;
        if (again) { old.swap(t->scriptId); t->settled = false; }
    }
    if (again) {
        if (!old.empty()) Slot<RemoveScript_t>(t->webview, kWebViewRemoveScriptOnDocumentCreated)(t->webview, old.c_str());
        IUnknown* done = MakeHandler(IID_AddScriptCompleted, nullptr, OnScriptAdded, t);
        HRESULT hr = Slot<AddScript_t>(t->webview, kWebViewAddScriptOnDocumentCreated)(t->webview, p->script->c_str(), done);
        done->Release();
        if (FAILED(hr)) {
            // No callback is coming: the record is settled with no script to take back.
            std::lock_guard<std::mutex> lock(g_lock);
            t->settled = true;
        }
    }
    Slot<ExecuteScript_t>(t->webview, kWebViewExecuteScript)(t->webview, p->push->c_str(), nullptr);
}

// One web view's hand-over, posted to its window; the strings are shared by every web view.
struct ProjectWork {
    Tracked* t;
    std::shared_ptr<const std::wstring> script, push;
};
static LRESULT CALLBACK PushWndProc(HWND w, UINT msg, WPARAM wp, LPARAM lp) {
    if (msg == WM_NCDESTROY) {
        std::lock_guard<std::mutex> lock(g_pushLock);
        for (auto it = g_pushList.begin(); it != g_pushList.end(); ++it) if (*it == w) { g_pushList.erase(it); break; }
    }
    if (msg == kPushMsg) {
        HookCall in;  // Wh_ModUninit waits for a hand-over still running here
        auto* work = reinterpret_cast<ProjectWork*>(lp);
        ProjectPush p{work->t, work->script.get(), work->push.get()};
        PushOnThread(&p);
        delete work;
        return 0;
    }
    return DefWindowProcW(w, msg, wp, lp);
}
// On the web view's thread, at a disable: hand-overs still queued are dropped with the window that
// would have run them.
static void DropPushWindow(Tracked* t) {
    if (!t->push) return;
    MSG m;
    while (PeekMessageW(&m, t->push, kPushMsg, kPushMsg, PM_REMOVE)) delete reinterpret_cast<ProjectWork*>(m.lParam);
    DestroyWindow(t->push);  // its WM_NCDESTROY takes it off the list
    t->push = nullptr;
}

// On the reader's thread. The files are read here, never in the hook; each web view's work is
// posted to its own thread, which WebView2 requires. Nothing is posted when the files did not change.
static int g_emptyReads = 0;  // reads in a row that found no files while the page had some (reader thread only)
static void RefreshProject() {
    std::wstring seen;
    bool seenOk = false;
    std::wstring object = CurrentProjectObject(&seen, &seenOk);
    // The folder Power BI wrote to had no files to read (gone, or still being moved): what was read
    // instead may be from before the save, so the read is tried again shortly, up to five times.
    if (!seen.empty() && !seenOk) { if (++g_emptyReads <= 5) ScheduleProjectRead(2000); }
    else g_emptyReads = 0;
    std::vector<Tracked*> views;
    {
        std::lock_guard<std::mutex> lock(g_lock);
        if (g_unloading || object == g_projectObject) return;
        // No files where the page had some: most likely a save still moving its files into place.
        // The page keeps what it has, and the read is tried again, up to five times.
        if (object.empty() && !g_projectObject.empty()) {
            if (++g_emptyReads <= 5) ScheduleProjectRead(2000);
            return;
        }
        g_projectObject = object;
        g_projectJs = ProjectScriptFrom(object);
        views = g_tracked;
    }
    auto script = std::make_shared<const std::wstring>(ScriptForNewPage());
    auto push = std::make_shared<const std::wstring>(PushScript(object, seen));
    int done = 0;
    for (Tracked* t : views) {
        if (g_stopWorker) break;
        auto* work = new ProjectWork{t, script, push};
        if (t->push && PostMessageW(t->push, kPushMsg, 0, reinterpret_cast<LPARAM>(work))) done++;
        else delete work;
    }
    std::wstring root;
    {
        std::lock_guard<std::mutex> lock(g_reportLock);
        root = g_reportRoot;
    }
    Wh_Log(L"project: %s, given to %d of %d web views", root.empty() ? L"none" : root.c_str(), done, (int)views.size());
}

// Waits for a .pbip, a save or a settings change, then reads once things are quiet. Tracked
// records are only freed during a disable, and Wh_ModBeforeUninit stops this thread before it
// takes the list, so the records it works on stay valid.
static DWORD WINAPI ProjectWorker(void*) {
    for (;;) {
        ULONGLONG due = g_dueAt.load(), now = GetTickCount64();
        DWORD wait = !due ? INFINITE : due > now ? (DWORD)(due - now) : 0;
        WaitForSingleObject(g_wake, wait);
        if (g_stopWorker) return 0;
        due = g_dueAt.load();
        if (!due || GetTickCount64() < due) continue;
        if (!g_dueAt.compare_exchange_strong(due, 0)) continue;
        RefreshProject();
    }
}

void Wh_ModBeforeUninit() {
    // The reader first: it works on the records the list below is about to hand over. It only
    // posts to the web views' threads, so it stops at once; what it posted and no thread has run
    // yet is dropped by TakeBack.
    g_stopWorker = true;
    if (g_wake) SetEvent(g_wake);
    if (g_worker) {
        WaitForSingleObject(g_worker, INFINITE);
        CloseHandle(g_worker);
        g_worker = nullptr;
    }
    std::vector<Tracked*> tracked;
    {
        std::lock_guard<std::mutex> lock(g_lock);
        g_unloading = true;
        tracked.swap(g_tracked);
    }
    int done = 0;
    for (Tracked* t : tracked) {
        HWND window = WindowOf(t);
        if (window && RunOnWindowThread(window, TakeBack, t)) done++;
        // Else the record is left as it is, reference and all: releasing it from this thread is not
        // allowed, and freeing it could race a callback still to come on its own thread.
    }
    Wh_Log(L"before uninit: script taken back out of %d of %d web views", done, (int)tracked.size());
}

// A hand-over window Wh_ModBeforeUninit could not reach: its thread is still running, but the
// thread hook could not be set on it. Its procedure goes back to the system's DefWindowProcW
// (allowed from any thread of the same process), so no message can reach this module's code once
// the image is gone. Hand-overs still queued for it stay in the queue unread, a few bytes each.
static long RetargetPushWindowsLeft() {
    std::lock_guard<std::mutex> lock(g_pushLock);
    long n = 0;
    wchar_t cls[64];
    for (HWND h : g_pushList) {
        if (!IsWindow(h) || !GetClassNameW(h, cls, 64) || wcscmp(cls, kPushClass) != 0) continue;
        SetWindowLongPtrW(h, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(DefWindowProcW));
        n++;
    }
    g_pushList.clear();
    return n;
}

// By now Windhawk has taken the hooks off, so no new handler can appear. Completions still to come
// (a script registration or a controller in flight) arrive on Power BI's own threads and let go of
// their handlers, and hook calls that started before the hooks came off (a CreateFileW on a slow
// path can take seconds) finish. The code of both is in this module, so this waits until they are
// gone: the same kind of wait the plain SendMessage in RunOnWindowThread and the join of the reader
// thread already make. Like outlook-refined, the wait is bounded at two minutes, and when the bound
// runs out the mod logs what is still held and lets Windhawk unload it anyway. The other choice,
// keeping the image loaded for the rest of the session, leaves a reference Windhawk does not
// expect and makes an update load a second copy beside the first; a completion WebView2 has not
// delivered in two minutes, or a file open that has not returned in two minutes, is taken to be one
// that will not come back, and that is the trade-off accepted here.
void Wh_ModUninit() {
    long windows = RetargetPushWindowsLeft();
    for (int i = 0; i < 2400 && (g_liveHandlers > 0 || g_hookCalls > 0); i++) Sleep(50);
    long live = g_liveHandlers.load(), calls = g_hookCalls.load();
    if (live > 0 || calls > 0) Wh_Log(L"uninit: %ld handler(s) and %ld hook call(s) still held after two minutes; unloading anyway", live, calls);
    // The last H_Release (or hook) lowers its count a few instructions before it returns out of this module.
    Sleep(200);
    // Fails harmlessly while a window of the class is left; the class uses DefWindowProcW, never this module.
    HMODULE self = nullptr;
    GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, reinterpret_cast<LPCWSTR>(&PushWndProc), &self);
    UnregisterClassW(kPushClass, self);
    // Closed only once no hook call is left that could still set it; otherwise it is left open.
    if (g_wake && calls == 0) { CloseHandle(g_wake); g_wake = nullptr; }
    Wh_Log(L"uninit: %ld handler(s), %ld hook call(s) still held; %ld hand-over window(s) given back to the system", live, calls, windows);
}
