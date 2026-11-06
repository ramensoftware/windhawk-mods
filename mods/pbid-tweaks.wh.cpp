// ==WindhawkMod==
// @id              pbid-tweaks
// @name            Skins & Tools for Power BI
// @description     Skins for the Power BI Desktop window, plus layout, filter, theme, interaction, color and mock-up tools, injected from inside Power BI with no extra programs
// @version         1.0.2
// @author          Reid Havens
// @github          https://github.com/ae-reid-havens
// @include         PBIDesktop.exe
// @license         CC-BY-ND-4.0
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Skins & Tools for Power BI

Restyles the Power BI Desktop window (title bar, ribbon, panes, page tabs, status bar, every
view) with one of several skins. Your reports are never changed: the report page and its
visuals look exactly as your report theme says.

Power BI is a trademark of Microsoft. This mod is not affiliated with or endorsed by Microsoft.
Licensed CC BY-ND 4.0: share unchanged copies with credit; modified versions may not be shared.
By Reid Havens, Analytic Endeavors (fingerprint AE-PBT-29A1ED34-2026).

![The 13 skins, one after another](https://raw.githubusercontent.com/ae-reid-havens/skins-tools-for-power-bi/main/media/skins-v1.webp)

![Layout QA and Colors staying on while the skin changes](https://raw.githubusercontent.com/ae-reid-havens/skins-tools-for-power-bi/main/media/skin-tools-v1.webp)

More demos of each tool, and every release: https://github.com/ae-reid-havens/skins-tools-for-power-bi

**Skins:** Blue Hills, Classic Grey, Stronghold, Gunmetal Console, Hazard Suit, Analytic Endeavors, Starship Panel, Phosphor Terminal, Neon Grid, High Contrast Ink, Graph Paper, Midnight Orchid, Arctic Frost.

**How to pick one:** open the **View** tab; a **Skins** group sits at the end of the ribbon.
The status bar also gets a small chip (the skin name and the page's visual count) that opens
the menu, or press **Ctrl+Alt+K**. **Ctrl+Alt+]** and **Ctrl+Alt+[** step through the skins.
"None" puts Power BI back exactly as it was. The choice is remembered; changing this mod's
settings resets it to the settings.

**Tools for report authors** (switches at the top of the menu, grouped by job: Plan, Layout,
Check, While editing):
- **Stencil (Ctrl+Alt+S):** a see-through mock-up layer on the page. Draw cards, charts, tables,
  slicers, titles and text on the grid, in report pixels; blocks are saved per report and page.
  The Build menu copies a PowerShell script that writes them into the report project folder as
  real visuals with no fields (new files only); save the report, paste the script, then click Apply external changes in the bar Power BI shows.
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
  page. It updates by itself when a filter changes, and never selects or clicks anything you see.
- **Theme check (Ctrl+Alt+T):** a badge on every visual that sets formatting differently from
  the report theme ("3 differ from theme"), colors outside the theme included; settings that
  differ from most of the page are grey information, off by default. The card gives the value
  each should have. Uses the report project folder setting; without
  it, compares what each visual draws with most of the page.
- **Colors (Ctrl+Alt+L, or Ctrl+Alt+A):** one badge on every visual whose text, bars, lines or
  points are hard to read against what is behind them (WCAG AA or AAA, picked in its strip), or
  that draws colors off the palette (the report theme's colors when the report project folder is
  set, else Power BI's default theme colors, unless you set your own in this mod's settings).
  Hover it for each finding, numbered and outlined on the page; click a swatch to copy its hex.
- **Interactions (Ctrl+Alt+I, on by default; shows in Format > Edit interactions):** the active icon on
  every visual is colored (None red, Filter blue, Highlight green), and a bar above the selected
  visual sets every other visual to None, Filter or Highlight in one click.

Badge colors mean severity only (red to fix, amber to look at, grey for information), and each
badge leads with its tool's letter (F, T, C).

**Help (Ctrl+Alt+H, or "Help and keys" in the menu):** what every mark the tools draw means,
the extras, every key, and where choices are kept.

**Extras (menu):** a splash when a skin is picked, short notes on visual errors and finished
refreshes, confetti when a refresh finishes, optional sounds; each has a Try button.

**What it reads and keeps:**
- **Files:** the mod reads files for two settings only. "Report project folder" reads that
  report's own JSON files (every .json under its definition and StaticResources folders) when
  Power BI starts and when the settings change; "Sound file for the refresh confetti" reads that
  one audio file. The mod itself writes no files.
- **Stencil's Build** copies a PowerShell script to the clipboard and changes nothing by itself.
  Only when you paste and run it yourself does it write into the report project folder: new
  visual files (and for a new page its page.json, with pages.json first backed up to a
  .stencil-backup folder beside the .Report folder, then updated to list the page). It never
  overwrites a file that exists.
- **Browser storage:** choices made inside Power BI (the skin, the switches, each tool's settings,
  Stencil blocks) are kept in the Power BI page's own localStorage under the key `pbid-skins`;
  conveniences for the viewer (folded menu sections, the last help tab, the dock tab, the
  one-time menu hint) under `pbid-skins-ui`. Disabling the mod takes the skin and tools out of
  the open window and leaves both keys for next time.

Works with Power BI's light and dark modes. Power BI changes its interface often; if an update
leaves something looking wrong, pick "None", which puts Power BI back exactly as it was.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- skin: off
  $name: Skin
  $description: The skin to use until you pick another inside Power BI
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
- cheer: ""
  $name: Sound file for the refresh confetti
  $description: Full path to an .mp3, .wav, .ogg or .m4a under 2 MB, played with the confetti when a refresh finishes; empty for none. A "with the cheer sound" switch appears under Confetti in the menu
- project: ""
  $name: Report project folder
  $description: The .Report folder of a report saved as a Power BI project (or its .pbip file), for the theme check and the palette from the report theme. Read when Power BI starts and when these settings change; empty for none
- visualCount: true
  $name: Visual count in the status bar
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
  $description: "Hex colors separated by commas or spaces, for example #118DFF, #12239E, #E66C37. Empty means the report theme's colors when the project folder is set, else Power BI's default theme colors"
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
#include <atomic>
#include <mutex>
#include <string>
#include <vector>
#include <cwctype>

// ---- the skins and tools: generated by scripts/build-mod.mjs from the pbid-tweaks repo ----
static const wchar_t kPayload[] =
LR"PBT((() => {
const PBT_HOST = 'mod';
const __defaults = {"skin":"off","features":{"statusChip":true,"visualCount":true,"notes":true,"confetti":true,"sounds":false,"splash":true,"reportFilterPane":false,"refreshCheer":true,"interactions":true},"qa":{"grid":8,"nearMiss":8,"snap":true},"lint":{"palette":["#118DFF","#12239E","#E66C37","#6B007B","#E044A7","#744EC2","#D9B300","#D64550","#197278","#1AAB40","#15C6F4","#4092FF","#FFA058","#BE5DC9","#F472D0","#B5A1FF","#C4A200","#FF8080","#00DBBC","#5BD667"],"tolerance":12},"pageSize":null};
const __settings = (typeof __PBT_SETTINGS__ !== 'undefined' && __PBT_SETTINGS__) || {};
const PBT_SETTINGS_KEY = JSON.stringify(__settings);
// The sound file the mod's "cheer" setting names, embedded by the mod as a data URL before this
// runs; else none. The build never puts a sound of its own here.
const PBT_CHEER = (typeof __PBT_CHEER__ !== 'undefined' && __PBT_CHEER__) || null;
// The report project files the mod's "project" setting names, read by the mod when Power BI
// starts or the settings change: { root, files: [{ path, text }] }, or none.
const PBT_PROJECT = (typeof __PBT_PROJECT__ !== 'undefined' && __PBT_PROJECT__) || (typeof __PBT_PROJECT_JSON__ === 'string' && window === window.top ? (() => { try { return JSON.parse(__PBT_PROJECT_JSON__); } catch { return null; } })() : null);
let __stored = null;
try { const s = JSON.parse(localStorage.getItem('pbid-skins') || 'null'); if (s && s.settings === PBT_SETTINGS_KEY) __stored = s.cfg; } catch {}
const PBT_CONFIG = __stored || { ...__defaults, ...__settings, features: { ...__defaults.features, ...(__settings.features || {}) }, qa: { ...__defaults.qa, ...(__settings.qa || {}) }, lint: { ...__defaults.lint, ...(__settings.lint || {}) } };
const PBT_SKINS = {"luna":{"short":"Blue","name":"Blue Hills","order":1,"splash":["Welcome","Blue sky, green hills."],"confetti":{"shapes":["square"],"colors":["#f25022","#7fba00","#00a4ef","#ffb900"]},"sound":{"wave":"sine","notes":[659,784,988,1319],"step":0.12,"len":0.5},"palette":{"ramp":[["#000000","#ece9d8"],["#141414","#f1efe2"],["#292929","#ffffff"],["#3d3d3d","#d6d2c2"],["#5c5c5c","#aca899"],["#8f8f8f","#716f64"],["#d6d6d6","#1f1f1f"],["#ffffff","#000000"]],"accent":"#316ac5","vars":{"font":"Tahoma, \"Segoe UI\", sans-serif","title-font":"\"Trebuchet MS\", Tahoma, sans-serif","titlebar-bg":"linear-gradient(180deg, #0997ff 0%, #0053ee 9%, #0050ee 40%, #0066ff 86%, #005bff 94%, #003dd7 100%)","titlebar-fg":"#ffffff","titlebar-hover":"rgba(255, 255, 255, .25)","icon":"#ffffff","icon-alt":"#cfe0ff","search-bg":"rgba(255, 255, 255, .92)","icon-fill":"#ffffff","tabs-bg":"#ece9d8","tab-fg":"#000000","tab-active-bg":"#fcfcfe","tab-active-fg":"#000000","tab-hover-bg":"#fff4d1","tab-radius":"3px","file-bg":"linear-gradient(180deg, #3c9a3c, #2d7d2d)","file-fg":"#ffffff","ribbon-bg":"linear-gradient(180deg, #fcfcfe 0%, #f4f3ee 100%)","ribbon-border":"1px solid #919b9c","group-fg":"#215dc6","nav-bg":"linear-gradient(180deg, #5f8ee0, #4a5fc8)","icon-nav":"#ffffff","icon-active":"#ffffff","nav-active-bg":"rgba(0, 20, 90, .35)","pagetabs-bg":"#ece9d8","pagetab-bg":"linear-gradient(180deg, #ffffff, #ecebe5)","pagetab-active-bg":"#fcfcfe","panehead-bg":"linear-gradient(90deg, #ffffff 0%, #c6d3f7 100%)","panehead-fg":"#215dc6","status-bg":"linear-gradient(180deg, #f0eee3, #e3dfcd)","status-fg":"#000000","gutter":"radial-gradient(120% 60% at 30% 108%, #5fa83a 0 40%, transparent 41%), radial-gradient(90% 50% at 85% 112%, #4c9a2a 0 45%, transparent 46%), linear-gradient(180deg, #2f6fd8 0%, #6aa5ee 55%, #bcd9f7 80%, #5fa83a 81%)","ui-bg":"#ffffff","ui-fg":"#000000","ui-border":"#7f9db9","ui-font":"Tahoma, sans-serif","ui-radius":"4px","hover-bg":"#fff4d1","hover-ring":"#ffc83c","hover-radius":"3px","avatar-bg":"rgba(255, 255, 255, .2)","nav-hover-bg":"rgba(255, 255, 255, .2)","wc-glyph":"14px","search-fg":"#3a4a6b"}},"css":"/* Luna: glossy blue title bar, beige chrome, white tabs with the orange hover line. */\nhtml[data-pbt-skin=\"luna\"] div.titlebar .title { font: bold 13px \"Trebuchet MS\", Tahoma, sans-serif !important; text-shadow: 1px 1px #0a1f8a; }\nhtml[data-pbt-skin=\"luna\"] div.titlebar .timestamp { text-shadow: 1px 1px #0a1f8a; }\n\n\n/* XP tabs: raised white tab, orange line along the top when hovered or selected. */\nhtml[data-pbt-skin=\"luna\"] #tablist button[role=\"tab\"] { border: 1px solid transparent !important; }\nhtml[data-pbt-skin=\"luna\"] #tablist button[role=\"tab\"]:hover { box-shadow: inset 0 3px 0 #ffc83c; border-color: #919b9c !important; }\nhtml[data-pbt-skin=\"luna\"] #tablist button[role=\"tab\"][aria-selected=\"true\"] {\n  border-color: #919b9c !important; border-bottom-color: #fcfcfe !important; box-shadow: inset 0 3px 0 #e68b2c;\n}\nhtml[data-pbt-skin=\"luna\"] #tablist button[role=\"tab\"]::after { display: none !important; }\nhtml[data-pbt-skin=\"luna\"] [id=\"Ribbon-file\"] { border-radius: 0 8px 8px 0 !important; font-style: italic; font-weight: bold; }\n\n/* Ribbon: rounded panel; buttons get the XP hover. */\nhtml[data-pbt-skin=\"luna\"] .ribbonContainer .bottomBarContainer { border-radius: 0 0 4px 4px !important; }\n\n/* Explorer style task panes. */\nhtml[data-pbt-skin=\"luna\"] .sidePane, html[data-pbt-skin=\"luna\"] .outspacePane { border-left: 1px solid #7f9db9 !important; }\nhtml[data-pbt-skin=\"luna\"] :is(.pane-header, .outspacePane .paneHeader) { border-radius: 4px 4px 0 0; font-weight: bold; }\n\n/* Page tabs */\nhtml[data-pbt-skin=\"luna\"] .carouselScrollPane li.section, html[data-pbt-skin=\"luna\"] button.section.static.create .thumbnail-container { border: 1px solid #919b9c !important; border-bottom: 0 !important; }\nhtml[data-pbt-skin=\"luna\"] .carouselScrollPane li.section.se)PBT"
LR"PBT(lected { box-shadow: inset 0 3px 0 #e68b2c; }\n\n/* Scrollbars */\nhtml[data-pbt-skin=\"luna\"] ::-webkit-scrollbar { width: 16px; height: 16px; background: #f4f3ee; }\nhtml[data-pbt-skin=\"luna\"] ::-webkit-scrollbar-thumb { background: linear-gradient(90deg, #c8d6fb, #b2c8f8); border: 1px solid #fff; border-radius: 3px; box-shadow: inset 0 0 0 1px #7f9db9; }\n\n/* Office icons draw their outlines light grey for dark ribbons; darken them on this light one. */\nhtml[data-pbt-skin=\"luna\"] .ribbonContainer svg .OfficeIconColors_m22 { fill: #5f6b7a !important; }\n\n/* The search magnifier on the pale search field. */\nhtml[data-pbt-skin=\"luna\"] div.ribbonSearchOuter svg, html[data-pbt-skin=\"luna\"] div.ribbonSearchOuter svg path { fill: #3a4a6b !important; color: #3a4a6b !important; }\n\n/* Window controls: rounded glossy squares inside Power BI's own button boxes; Close is orange-red. */\nhtml[data-pbt-skin=\"luna\"] {\n  --pbt-wc-inset: 5px 9px; --pbt-wc-radius: 3px; --pbt-wc-fg: #fff;\n  --pbt-wc-face: radial-gradient(circle at 30% 25%, #7fb0ff, #2463e0 60%, #1546b8);\n  --pbt-wc-close-face: radial-gradient(circle at 30% 25%, #f6a28a, #e0491f 60%, #b5320f);\n  --pbt-wc-ring: inset 0 0 0 1px #fff, inset 0 0 3px rgba(0, 0, 0, .45);\n  --pbt-wc-hover-bg: transparent; --pbt-wc-close-hover-bg: transparent;\n}\n"},"classic":{"short":"Classic","name":"Classic Grey","order":2,"splash":["Classic","Please wait while your settings are applied..."],"confetti":{"shapes":["square"],"colors":["#0a246a","#a6caf0","#d4d0c8","#808080","#3a6ea5"]},"sound":{"wave":"square","notes":[880,660],"step":0.09,"len":0.15},"palette":{"ramp":[["#000000","#d4d0c8"],["#141414","#d4d0c8"],["#292929","#ffffff"],["#3d3d3d","#c0c0c0"],["#5c5c5c","#808080"],["#8f8f8f","#404040"],["#d6d6d6","#000000"],["#ffffff","#000000"]],"accent":"#0a246a","vars":{"font":"Tahoma, \"Microsoft Sans Serif\", sans-serif","title-font":"Tahoma, sans-serif","titlebar-bg":"linear-gradient(90deg, #0a246a 0%, #a6caf0 100%)","titlebar-fg":"#ffffff","titlebar-hover":"rgba(255, 255, 255, .2)","icon":"#ffffff","icon-alt":"#d4d0c8","search-bg":"#ffffff","icon-fill":"#ffffff","tabs-bg":"#d4d0c8","tab-fg":"#000000","tab-active-bg":"#d4d0c8","tab-active-fg":"#000000","tab-hover-bg":"#d4d0c8","file-bg":"#d4d0c8","file-fg":"#000000","ribbon-bg":"#d4d0c8","group-fg":"#000000","nav-bg":"#d4d0c8","icon-nav":"#000000","icon-active":"#000000","nav-active-bg":"#ffffff","pagetabs-bg":"#d4d0c8","pagetab-bg":"#d4d0c8","pagetab-active-bg":"#d4d0c8","panehead-bg":"linear-gradient(90deg, #0a246a, #4a7cc7)","panehead-fg":"#ffffff","status-bg":"#d4d0c8","status-fg":"#000000","gutter":"#3a6ea5","ui-bg":"#d4d0c8","ui-fg":"#000000","ui-border":"#808080","ui-font":"Tahoma, sans-serif","ui-radius":"0","ui-shadow":"inset -1px -1px #404040, inset 1px 1px #ffffff, inset -2px -2px #808080, inset 2px 2px #dfdfdf, 2px 2px 0 rgba(0,0,0,.35)","avatar-bg":"transparent","wc-glyph":"12px","hover-radius":"0"}},"css":"/* Classic: grey 3D bevels everywhere, navy gradient title bar, square corners. */\nhtml[data-pbt-skin=\"classic\"] {\n  --pbt-outset: inset -1px -1px #404040, inset 1px 1px #ffffff, inset -2px -2px #808080, inset 2px 2px #dfdfdf;\n  --pbt-inset: inset -1px -1px #ffffff, inset 1px 1px #808080, inset -2px -2px #dfdfdf, inset 2px 2px #404040;\n}\nhtml[data-pbt-skin=\"classic\"] div.titlebar .title { font-weight: bold !important; }\nhtml[data-pbt-skin=\"classic\"] * { border-radius: 0 !important; }\nhtml[data-pbt-skin=\"classic\"] .displayArea * { border-radius: revert !important; }\n\n\n/* Tabs as property-sheet tabs on one baseline, File included. The open tab is bold, a shade\n   lighter, and open at the bottom into the panel instead of jumping up. */\nhtml[data-pbt-skin=\"classic\"] :is(#tablist button[role=\"tab\"], [id=\"Ribbon-file\"]) { box-shadow: inset 1px 1px #fff, inset -1px 0 #404040, inset -2px 0 #808080 !important; margin-top: 3px !important; height: 28px !important; }\nhtml[data-pbt-sk)PBT"
LR"PBT(in=\"classic\"] #tablist button[role=\"tab\"][aria-selected=\"true\"] { font-weight: bold; background: #e0ddd6 !important; box-shadow: inset 1px 1px #fff, inset -1px 0 #404040, inset -2px 0 #808080, 0 2px 0 #e0ddd6 !important; }\nhtml[data-pbt-skin=\"classic\"] #tablist button[role=\"tab\"]::after { display: none !important; }\n\n/* Ribbon panel and buttons bevel. */\nhtml[data-pbt-skin=\"classic\"] .ribbonContainer .bottomBarContainer { box-shadow: var(--pbt-outset); }\nhtml[data-pbt-skin=\"classic\"][data-pbt-skin] .ribbonContainer .bottomBarContainer button:not(:disabled):not(.is-disabled):not(.pbt-rbtn):hover { box-shadow: var(--pbt-outset) !important; background: #d4d0c8 !important; }\nhtml[data-pbt-skin=\"classic\"] .ribbonContainer .bottomBarContainer button:not(:disabled):active { box-shadow: var(--pbt-inset); }\n\n/* Panes are sunken wells; inputs are white wells. */\nhtml[data-pbt-skin=\"classic\"] :is(.sidePane, .outspacePane) { box-shadow: var(--pbt-outset); }\nhtml[data-pbt-skin=\"classic\"] :is(.sidePane, .outspacePane, .ribbonSearchOuter) input { box-shadow: var(--pbt-inset); background: #fff !important; }\nhtml[data-pbt-skin=\"classic\"] :is(.pane-header, .outspacePane .paneHeader) { font-weight: bold; }\nhtml[data-pbt-skin=\"classic\"] view-navigator button[aria-selected=\"true\"] { box-shadow: var(--pbt-inset); }\n\n/* Page tabs */\nhtml[data-pbt-skin=\"classic\"] .carouselScrollPane li.section, html[data-pbt-skin=\"classic\"] button.section.static.create .thumbnail-container { box-shadow: inset 1px 0 #fff, inset -1px 0 #404040, inset 0 -1px #fff; }\nhtml[data-pbt-skin=\"classic\"] .carouselScrollPane li.section.selected { box-shadow: inset 1px 0 #fff, inset -1px 0 #404040, inset 0 -2px #d4d0c8; }\n\n/* Status bar: sunken panels. */\nhtml[data-pbt-skin=\"classic\"] section.statusBar > div { box-shadow: inset -1px -1px #fff, inset 1px 1px #808080; margin: 1px; }\n\n/* Our menu and toasts get the bevel too. */\nhtml[data-pbt-skin=\"classic\"] .pbt-menu button:hover { background: #0a246a !important; color: #fff; --pbt-ui-fg: #fff; --pbt-ui-bg: #0a246a; --pbt-accent: #fff; }\nhtml[data-pbt-skin=\"classic\"] .pbt-toast { border: 0; border-left: 0; }\n\nhtml[data-pbt-skin=\"classic\"] ::-webkit-scrollbar { width: 16px; height: 16px; background: repeating-conic-gradient(#d4d0c8 0 25%, #fff 0 50%) 0 0 / 2px 2px; }\nhtml[data-pbt-skin=\"classic\"] ::-webkit-scrollbar-thumb { background: #d4d0c8; box-shadow: var(--pbt-outset); }\n\n/* The title gradient ends light blue; the account sits on that end, so it takes dark ink. */\nhtml[data-pbt-skin=\"classic\"] div.titlebar .accountNameLabel { color: #0a246a !important; }\nhtml[data-pbt-skin=\"classic\"] titlebar account .imageAvatarIcon { color: #0a246a !important; }\n\n/* Office icons draw their outlines light grey for dark ribbons; darken them on this light one. */\nhtml[data-pbt-skin=\"classic\"] .ribbonContainer svg .OfficeIconColors_m22 { fill: #404040 !important; }\n\n/* Share is a square raised button like the window controls. */\nhtml[data-pbt-skin=\"classic\"] .farPeripheralControls button { box-shadow: var(--pbt-outset) !important; border: 0 !important; background: #d4d0c8 !important; }\n/* Search fields: one sunken well around the whole field, not a second one around the text. */\nhtml[data-pbt-skin=\"classic\"] :is(.outspacePane, .ribbonSearchOuter) input { box-shadow: none !important; }\nhtml[data-pbt-skin=\"classic\"]:not(.pbt-report-filters) .outspacePane search-box-modern { box-shadow: var(--pbt-inset) !important; background-color: #fff !important; }\n\nhtml[data-pbt-skin=\"classic\"] .sidePane input { box-shadow: none !important; }\n\n/* Window controls: small bevelled grey buttons inside Power BI's own button boxes. */\nhtml[data-pbt-skin=\"classic\"] {\n  --pbt-wc-inset: 6px 5px; --pbt-wc-radius: 0; --pbt-wc-fg: #000; --pbt-wc-close-hover-fg: #000;\n  --pbt-wc-face: #d4d0c8; --pbt-wc-ring: inset -1px -1px #404040, inset 1px 1px #ffffff, inset)PBT"
LR"PBT( -2px -2px #808080, inset 2px 2px #dfdfdf;\n  --pbt-wc-hover-bg: transparent; --pbt-wc-close-hover-bg: transparent;\n}\nhtml[data-pbt-skin=\"classic\"] .windowControls button:hover::before { filter: none; }\nhtml[data-pbt-skin=\"classic\"] .windowControls button:active::before { box-shadow: inset -1px -1px #ffffff, inset 1px 1px #808080, inset -2px -2px #dfdfdf, inset 2px 2px #404040; }\n"},"stronghold":{"short":"Stronghold","name":"Stronghold","order":3,"splash":["Stronghold Ready","The war room is open. Bring data, chieftain."],"confetti":{"shapes":["coin"],"colors":["#f2c94c","#d4a531","#b8860b","#ffe08a"]},"sound":{"wave":"triangle","notes":[110,110,147,98],"step":0.16,"len":0.25},"palette":{"ramp":[["#000000","#0e0b09"],["#141414","#1b1612"],["#292929","#2a221b"],["#3d3d3d","#40342a"],["#5c5c5c","#5e4c3b"],["#8f8f8f","#a08a6a"],["#d6d6d6","#efdcae"],["#ffffff","#fff1cc"]],"accent":"#b3121b","vars":{"font":"\"Segoe UI\", sans-serif","title-font":"\"Book Antiqua\", \"Palatino Linotype\", Georgia, serif","titlebar-bg":"radial-gradient(circle at 50% 50%, #f2d27a 0 1.4px, #6b4a14 2px, transparent 2.6px) 12px calc(100% - 1px) / 48px 7px repeat-x, radial-gradient(circle at 50% 50%, #f2d27a 0 1.4px, #6b4a14 2px, transparent 2.6px) 12px 1px / 48px 7px repeat-x, linear-gradient(180deg, #9a6a1c 0 1px, #3a332d 1px, #24201c 50%, #171411 calc(100% - 2px), #9a6a1c calc(100% - 2px))","titlebar-fg":"#f2d27a","titlebar-hover":"rgba(242, 210, 122, .2)","icon":"#f2d27a","icon-alt":"#a08a6a","avatar-bg":"rgba(0, 0, 0, .35)","search-bg":"rgba(14, 11, 9, .75)","tabs-bg":"linear-gradient(180deg, rgba(0,0,0,.15), rgba(0,0,0,.45)), repeating-linear-gradient(0deg, #15110e 0 2px, transparent 2px 18px), repeating-linear-gradient(90deg, #15110e 0 2px, transparent 2px 46px) 0 0 / 46px 36px, linear-gradient(135deg, #4a423a, #332d28)","tab-fg":"#e6cf96","tab-active-bg":"linear-gradient(180deg, #c4161f 0%, #8e0d14 70%, #5e070c 100%)","tab-active-fg":"#ffe9a8","tab-hover-bg":"rgba(242, 210, 122, .16)","file-bg":"linear-gradient(180deg, #f2d27a, #b98a2e 50%, #7a5414)","file-fg":"#2a1606","ribbon-bg":"radial-gradient(circle at 25% 20%, rgba(255, 220, 150, .06), transparent 45%), repeating-linear-gradient(45deg, rgba(0, 0, 0, .12) 0 2px, transparent 2px 6px), linear-gradient(180deg, #33281f, #221a14)","group-fg":"#e0b44a","hover-bg":"rgba(179, 18, 27, .28)","hover-ring":"#e0b44a","hover-radius":"2px","nav-bg":"linear-gradient(90deg, #171411, #24201c 60%, #171411), linear-gradient(#9a6a1c, #9a6a1c)","icon-nav":"#cdb47a","icon-active":"#ffe9a8","nav-active-bg":"linear-gradient(90deg, rgba(179, 18, 27, .7), rgba(179, 18, 27, .2))","nav-hover-bg":"rgba(242, 210, 122, .14)","pagetabs-bg":"linear-gradient(180deg, #24201c, #171411)","pagetab-bg":"linear-gradient(180deg, #40342a, #2a221b)","pagetab-fg":"#e6cf96","pagetab-active-bg":"linear-gradient(180deg, #c4161f, #6e0a10)","pagetab-active-fg":"#ffe9a8","panehead-bg":"linear-gradient(180deg, #8e0d14, #4a060a)","panehead-fg":"#f2d27a","status-bg":"radial-gradient(circle at 50% 50%, #e0b44a 0 1.2px, #5a3a0c 1.8px, transparent 2.4px) 8px 0 / 96px 6px repeat-x, linear-gradient(180deg, #3a332d, #171411)","status-fg":"#cdb47a","ui-bg":"radial-gradient(120% 120% at 50% 40%, #f6e8c3 0%, #ead5a2 60%, #cfae72 100%)","ui-fg":"#3a2410","ui-border":"#8a5a14","ui-font":"\"Segoe UI\", sans-serif","ui-radius":"3px","ui-shadow":"0 0 0 1px #3a2410, 0 10px 28px rgba(0, 0, 0, .6)","wc-glyph":"12px","share-bg":"linear-gradient(180deg, #3a332d, #24201c)"}},"css":"/* Stronghold: an iron band with bronze rivets across the top, a stone wall behind red banner\n   tabs, the ribbon in a bronze frame with spiked corner bosses, stitched leather panes and\n   parchment for every message. All drawn with gradients; no images. */\nhtml[data-pbt-skin=\"stronghold\"] {\n  --pbt-bronze: linear-gradient(180deg, #f7dd8c 0%, #c1913a 30%, #6b4a14 52%, #a8782a 75%, #e8c36a 100%);\n  /* A boss: bronze stud with a dark ring, and two tusks (curved )PBT"
LR"PBT(spikes) pointing out of the corner. */\n  --pbt-boss: radial-gradient(circle, #fff0b8 0 2px, #d9a640 3px 5px, #5a3a0c 6px 7px, transparent 7.5px);\n}\n\n/* Type: the chrome reads in Segoe UI (palette.vars font), so ribbon captions, field names and\n   filter labels stay easy to scan; the display face is kept for headings only: the title, the\n   ribbon tabs and File, the pane headers and the menu's section names. The base font rule reads\n   --pbt-font on each element, so setting it on a heading reaches every span inside. */\nhtml[data-pbt-skin=\"stronghold\"] :is(div.titlebar .title, #tablist, [id=\"Ribbon-file\"], .pane-header, .outspacePane .paneHeader, .pbt-menu h4) { --pbt-font: var(--pbt-title-font); font-family: var(--pbt-title-font); }\n\n/* Title: engraved gold, crossed blades before the file name. */\nhtml[data-pbt-skin=\"stronghold\"] div.titlebar .title { font: bold 15px \"Book Antiqua\", \"Palatino Linotype\", serif !important; letter-spacing: .05em; text-shadow: 0 1px 0 #000, 0 -1px 0 rgba(255, 240, 184, .25), 0 0 8px rgba(242, 201, 76, .35); }\nhtml[data-pbt-skin=\"stronghold\"] div.titlebar .title::before { content: \"\\2694\"; margin-right: 7px; color: #e0b44a; text-shadow: 0 1px 0 #000; font-family: \"Segoe UI Symbol\", serif; }\n\n\n/* Banner tabs: the chosen tab hangs as a red pennant with a gold edge and a cut point. */\nhtml[data-pbt-skin=\"stronghold\"] #tablist button[role=\"tab\"] { text-shadow: 0 1px 0 #000; letter-spacing: .04em; font-variant: small-caps; font-size: 15px !important; }\nhtml[data-pbt-skin=\"stronghold\"] #tablist button[role=\"tab\"]::after { display: none !important; }\nhtml[data-pbt-skin=\"stronghold\"] #tablist button[role=\"tab\"][aria-selected=\"true\"] {\n  clip-path: polygon(0 0, 100% 0, 100% 78%, 50% 100%, 0 78%); padding-bottom: 4px;\n  box-shadow: inset 2px 0 0 #e0b44a, inset -2px 0 0 #e0b44a, inset 0 2px 0 #f7dd8c;\n}\nhtml[data-pbt-skin=\"stronghold\"] [id=\"Ribbon-file\"] { font-weight: bold; text-shadow: 0 1px 0 rgba(255, 240, 184, .6); box-shadow: inset 0 0 0 1px #3a2408; clip-path: polygon(0 0, calc(100% - 10px) 0, 100% 50%, calc(100% - 10px) 100%, 0 100%); padding-right: 10px; }\n\n/* Ribbon: bronze frame, dark inner edge, a boss at each corner. */\nhtml[data-pbt-skin=\"stronghold\"] .ribbonContainer .bottomBarContainer {\n  border: 3px solid transparent !important; border-image: var(--pbt-bronze) 1 !important; border-radius: 0 !important;\n  box-shadow: inset 0 0 0 1px #000, inset 0 0 18px rgba(0, 0, 0, .75), 0 3px 8px rgba(0, 0, 0, .7);\n  background:\n    var(--pbt-boss) 0 0 / 16px 16px no-repeat,\n    var(--pbt-boss) 100% 0 / 16px 16px no-repeat,\n    var(--pbt-boss) 0 100% / 16px 16px no-repeat,\n    var(--pbt-boss) 100% 100% / 16px 16px no-repeat,\n    var(--pbt-ribbon-bg) !important;\n}\nhtml[data-pbt-skin=\"stronghold\"] #RibbonModeToggle:hover { box-shadow: inset 0 0 0 1px #e0b44a; }\nhtml[data-pbt-skin=\"stronghold\"] .ribbonContainer [role=\"group\"] label[class*=\"groupLabel\"] { letter-spacing: .03em; text-shadow: 0 1px 0 #000; }\n/* Group dividers become thin bronze posts. */\nhtml[data-pbt-skin=\"stronghold\"] .ribbonContainer [class*=\"groupContainer\"] + [class*=\"groupContainer\"] { border-left: 1px solid #6b4a14 !important; box-shadow: inset 1px 0 0 rgba(255, 240, 184, .12); }\n\n/* Panes: leather with a stitched edge behind a bronze rail. */\nhtml[data-pbt-skin=\"stronghold\"] .sidePane { background: repeating-linear-gradient(45deg, rgba(0, 0, 0, .1) 0 2px, transparent 2px 6px), linear-gradient(180deg, #2a221b, #1b1612) !important; }\nhtml[data-pbt-skin=\"stronghold\"] :is(.sidePane, .outspacePane) { border-left: 3px solid transparent !important; border-image: var(--pbt-bronze) 1 !important; }\nhtml[data-pbt-skin=\"stronghold\"] .sidePane .pane-container { outline: 1px dashed rgba(224, 180, 74, .35); outline-offset: -5px; }\nhtml[data-pbt-skin=\"stronghold\"] :is(.pane-header, .outspacePane .paneHeader) { border-bottom: 2px solid #e0b44)PBT"
LR"PBT(a; font-variant: small-caps; letter-spacing: .06em; text-shadow: 0 1px 0 #000; }\nhtml[data-pbt-skin=\"stronghold\"] view-navigator { border-right: 3px solid transparent; border-image: var(--pbt-bronze) 1; }\n\n/* Page tabs: small red shields with a gold rim. */\nhtml[data-pbt-skin=\"stronghold\"] .carouselScrollPane li.section, html[data-pbt-skin=\"stronghold\"] button.section.static.create .thumbnail-container { border: 1px solid #6b4a14 !important; border-top: 0 !important; border-radius: 0 0 10px 10px !important; }\nhtml[data-pbt-skin=\"stronghold\"] .carouselScrollPane li.section.selected { border-color: #e0b44a !important; box-shadow: inset 0 0 0 1px #3a2408; }\nhtml[data-pbt-skin=\"stronghold\"] button.section.static.create { color: #f2d27a !important; }\nhtml[data-pbt-skin=\"stronghold\"] button.section.static.create .thumbnail-container { box-shadow: none; }\n\n/* Parchment for menus and toasts, with a wax seal on toasts; the splash is a scroll. */\nhtml[data-pbt-skin=\"stronghold\"] .pbt-toast { border: 1px solid #8a5a14; border-left: 1px solid #8a5a14; padding-left: 40px; position: relative; }\nhtml[data-pbt-skin=\"stronghold\"] .pbt-toast::before { content: \"\"; position: absolute; left: 10px; top: 50%; width: 20px; height: 20px; margin-top: -10px; border-radius: 50%; background: radial-gradient(circle at 40% 35%, #e04a4a, #9e1015 60%, #5e070c); box-shadow: 0 1px 2px rgba(0, 0, 0, .5); }\nhtml[data-pbt-skin=\"stronghold\"] .pbt-menu button:hover { background: rgba(179, 18, 27, .18) !important; }\nhtml[data-pbt-skin=\"stronghold\"] .pbt-menu h4 { color: #8e0d14; opacity: 1; font-variant: small-caps; }\nhtml[data-pbt-skin=\"stronghold\"] .pbt-splash > div { border: 0; color: #3a2410; padding: 34px 64px; box-shadow: 0 0 0 1px #3a2410, 0 -10px 0 -4px #8a5a14, 0 10px 0 -4px #8a5a14, 0 18px 40px rgba(0, 0, 0, .7); }\n\nhtml[data-pbt-skin=\"stronghold\"] ::-webkit-scrollbar { width: 14px; height: 14px; background: #1b1612; }\nhtml[data-pbt-skin=\"stronghold\"] ::-webkit-scrollbar-thumb { background: var(--pbt-bronze); border-radius: 7px; border: 3px solid #1b1612; }\n\n/* The Filters pane matches the side panes: leather behind a stitched inner frame. */\nhtml[data-pbt-skin=\"stronghold\"]:not(.pbt-report-filters) .outspacePane {\n  background: repeating-linear-gradient(45deg, rgba(0, 0, 0, .1) 0 2px, transparent 2px 6px), linear-gradient(180deg, #2a221b, #1b1612) !important;\n  outline: 1px dashed rgba(224, 180, 74, .35); outline-offset: -5px;\n}\nhtml[data-pbt-skin=\"stronghold\"] .farPeripheralControls button { border-color: #6b4a14 !important; }\n\n/* Window controls: round bronze rivets inside Power BI's own button boxes; Close glows red. */\nhtml[data-pbt-skin=\"stronghold\"] {\n  --pbt-wc-inset: 5px 10px; --pbt-wc-radius: 50%; --pbt-wc-fg: #2a1606; --pbt-wc-close-fg: #ffe9a8; --pbt-wc-close-hover-fg: #ffe9a8;\n  --pbt-wc-face: radial-gradient(circle at 38% 30%, #fff0b8, #d9a640 45%, #7a5414 80%, #3a2408);\n  --pbt-wc-close-face: radial-gradient(circle at 38% 30%, #ff9a8a, #c4161f 45%, #6e0a10 85%);\n  --pbt-wc-ring: 0 0 0 1px #000, 0 1px 2px rgba(0, 0, 0, .8);\n  --pbt-wc-hover-bg: transparent; --pbt-wc-close-hover-bg: transparent;\n}\nhtml[data-pbt-skin=\"stronghold\"] .windowControls button:hover::before { box-shadow: 0 0 0 1px #000, 0 0 8px rgba(255, 220, 120, .8); }\nhtml[data-pbt-skin=\"stronghold\"] .windowControls button.closeButton:hover::before { box-shadow: 0 0 0 1px #000, 0 0 10px #ff3b30; }\n"},"gunmetal-console":{"short":"Gunmetal","name":"Gunmetal Console","order":4,"splash":["Console Online","Systems nominal. Awaiting orders, commander."],"confetti":{"shapes":["crystal"],"colors":["#ffb020","#8fb2cc","#d4dee5","#ffd27a"]},"sound":{"wave":"sawtooth","notes":[1200,1600,1200,2000],"step":0.06,"len":0.07},"palette":{"ramp":[["#000000","#05090b"],["#141414","#0b1419"],["#292929","#13212a"],["#3d3d3d","#1d3240"],["#5c5c5c","#2e4d5f"],["#8f8f8f","#7f97a6"],["#d6d6d6","#d4dee5"],["#ffffff","#f0f5)PBT"
LR"PBT(f8"]],"accent":"#ffb020","vars":{"font":"Bahnschrift, \"Agency FB\", \"Segoe UI\", sans-serif","title-font":"\"Agency FB\", Bahnschrift, sans-serif","titlebar-bg":"linear-gradient(180deg, #2a3740 0%, #141c21 55%, #0b1014 100%)","titlebar-fg":"#d4dee5","titlebar-hover":"rgba(143, 178, 204, .2)","icon":"#c4d3dc","icon-alt":"#7f97a6","search-bg":"rgba(5, 9, 11, .8)","tabs-bg":"linear-gradient(180deg, #1a252c, #0f171c)","tab-fg":"#9fb4c2","tab-active-bg":"linear-gradient(180deg, rgba(255, 176, 32, .22), rgba(255, 176, 32, .04))","tab-active-fg":"#ffc04d","tab-hover-bg":"rgba(143, 178, 204, .16)","file-bg":"linear-gradient(180deg, #3b5568 0%, #253747 55%, #182531 100%)","file-fg":"#ffd27a","ribbon-bg":"linear-gradient(180deg, #24323b 0%, #18232a 50%, #121a1f 100%)","ribbon-border":"1px solid #3d5666","group-fg":"#9ec3db","nav-bg":"linear-gradient(90deg, #121a1f, #1d2a32)","icon-nav":"#9fb4c2","icon-active":"#ffc04d","nav-active-bg":"rgba(255, 176, 32, .14)","pagetabs-bg":"#0b1419","pagetab-bg":"linear-gradient(180deg, #1d3240, #13212a)","pagetab-fg":"#9fb4c2","pagetab-active-bg":"linear-gradient(180deg, rgba(255, 176, 32, .28), #13212a)","pagetab-active-fg":"#ffc04d","panehead-bg":"linear-gradient(90deg, #1d3240, transparent)","panehead-fg":"#ffc04d","status-bg":"linear-gradient(180deg, #1a252c, #0b1014)","status-fg":"#ffc04d","gutter":"radial-gradient(1px 1px at 20px 30px, #fff, transparent) 0 0 / 220px 200px, radial-gradient(1px 1px at 110px 80px, #9ff7ff, transparent) 0 0 / 220px 200px, radial-gradient(1.5px 1.5px at 160px 20px, #fff, transparent) 0 0 / 220px 200px, radial-gradient(1px 1px at 60px 140px, #cfe, transparent) 0 0 / 220px 200px, radial-gradient(1px 1px at 190px 170px, #fff, transparent) 0 0 / 220px 200px, radial-gradient(1px 1px at 37px 95px, #fff, transparent) 0 0 / 310px 260px, radial-gradient(120% 100% at 80% 10%, #10243a, #03060a 70%)","ui-bg":"#0f171c","ui-fg":"#d4dee5","ui-border":"#3d5666","ui-font":"Bahnschrift, \"Segoe UI\", sans-serif","ui-radius":"0","hover-bg":"rgba(143, 178, 204, .14)","hover-ring":"rgba(255, 176, 32, .7)","hover-radius":"0","avatar-bg":"rgba(255, 176, 32, .1)","panehead-case":"uppercase","panehead-tracking":".1em"}},"css":"/* Gunmetal Console: blue-grey steel plates with chamfered corners and rivets, pale steel text,\n   amber readouts for whatever is lit or chosen, scanlines on the panes, a starfield behind the page. */\nhtml[data-pbt-skin=\"gunmetal-console\"] { --pbt-chamfer: polygon(10px 0, calc(100% - 10px) 0, 100% 10px, 100% calc(100% - 10px), calc(100% - 10px) 100%, 10px 100%, 0 calc(100% - 10px), 0 10px); }\nhtml[data-pbt-skin=\"gunmetal-console\"] div.titlebar { border-bottom: 1px solid #5f7f94; box-shadow: 0 1px 8px rgba(143, 178, 204, .3); }\nhtml[data-pbt-skin=\"gunmetal-console\"] div.titlebar .title { font: 600 16px \"Agency FB\", Bahnschrift, sans-serif !important; letter-spacing: .12em; text-transform: uppercase; text-shadow: 0 0 6px rgba(143, 178, 204, .6); }\n\n\nhtml[data-pbt-skin=\"gunmetal-console\"] #tablist button[role=\"tab\"] { text-transform: uppercase; letter-spacing: .08em; font-size: 12px !important; }\nhtml[data-pbt-skin=\"gunmetal-console\"] #tablist button[role=\"tab\"][aria-selected=\"true\"] { text-shadow: 0 0 6px rgba(255, 176, 32, .6); box-shadow: inset 0 -2px 0 #ffb020; }\nhtml[data-pbt-skin=\"gunmetal-console\"] #tablist button[role=\"tab\"]::after { display: none !important; }\n/* File: a lit console key, chamfered, with an amber edge and a sweep of light on hover. */\nhtml[data-pbt-skin=\"gunmetal-console\"] [id=\"Ribbon-file\"] {\n  clip-path: polygon(8px 0, 100% 0, 100% calc(100% - 8px), calc(100% - 8px) 100%, 0 100%, 0 8px);\n  box-shadow: inset 0 0 0 1px #ffb020, inset 0 0 12px rgba(255, 176, 32, .35) !important;\n  text-transform: uppercase; letter-spacing: .14em; font-weight: 600;\n  text-shadow: 0 0 6px rgba(255, 176, 32, .7);\n}\nhtml[data-pbt-skin=\"gunmetal-console\"] [id=\"Ribbon-file\"]:hover {\n  background: linear-gradie)PBT"
LR"PBT(nt(100deg, transparent 30%, rgba(255, 228, 176, .3) 50%, transparent 70%) -100% 0 / 200% 100% no-repeat, var(--pbt-file-bg) !important;\n  animation: pbt-gunmetal-console-sweep .6s ease-out;\n}\n@keyframes pbt-gunmetal-console-sweep { from { background-position: -100% 0, 0 0; } to { background-position: 100% 0, 0 0; } }\n\n/* Ribbon plate: chamfered, riveted, with a thin cyan edge light. */\nhtml[data-pbt-skin=\"gunmetal-console\"] .ribbonContainer .bottomBarContainer {\n  clip-path: var(--pbt-chamfer);\n  background-image:\n    radial-gradient(circle at 16px 7px, #8fa7b3 0 1.5px, #2a3740 2.5px, transparent 3px),\n    radial-gradient(circle at calc(100% - 16px) 7px, #8fa7b3 0 1.5px, #2a3740 2.5px, transparent 3px),\n    radial-gradient(circle at 16px calc(100% - 7px), #8fa7b3 0 1.5px, #2a3740 2.5px, transparent 3px),\n    radial-gradient(circle at calc(100% - 16px) calc(100% - 7px), #8fa7b3 0 1.5px, #2a3740 2.5px, transparent 3px),\n    linear-gradient(90deg, rgba(90, 209, 255, .35), transparent 3px, transparent calc(100% - 3px), rgba(90, 209, 255, .35)),\n    var(--pbt-ribbon-bg) !important;\n}\nhtml[data-pbt-skin=\"gunmetal-console\"] .ribbonContainer [role=\"group\"] label[class*=\"groupLabel\"] { text-transform: uppercase; letter-spacing: .1em; font-size: 10px !important; }\n\n/* Panes: scanlines and a steel edge. The Filters pane keeps its colors: the report sets them. */\nhtml[data-pbt-skin=\"gunmetal-console\"] .sidePane { background-image: repeating-linear-gradient(180deg, rgba(0, 0, 0, .22) 0 1px, transparent 1px 3px) !important; }\nhtml[data-pbt-skin=\"gunmetal-console\"] :is(.sidePane, .outspacePane) { border-left: 1px solid #2e4d5f !important; box-shadow: inset 1px 0 0 rgba(90, 209, 255, .25); }\nhtml[data-pbt-skin=\"gunmetal-console\"] :is(.pane-header, .outspacePane .paneHeader) { text-transform: uppercase; letter-spacing: .1em; border-bottom: 1px solid #2e4d5f; }\nhtml[data-pbt-skin=\"gunmetal-console\"] .carouselScrollPane li.section, html[data-pbt-skin=\"gunmetal-console\"] button.section.static.create .thumbnail-container { clip-path: polygon(8px 0, calc(100% - 8px) 0, 100% 100%, 0 100%); padding: 0 6px; }\nhtml[data-pbt-skin=\"gunmetal-console\"] section.statusBar { border-top: 1px solid #2e4d5f; text-shadow: 0 0 4px rgba(255, 176, 32, .5); }\n\nhtml[data-pbt-skin=\"gunmetal-console\"] ::-webkit-scrollbar { width: 12px; height: 12px; background: #0b1419; }\nhtml[data-pbt-skin=\"gunmetal-console\"] ::-webkit-scrollbar-thumb { background: linear-gradient(90deg, #2e4d5f, #3d6a80); border: 2px solid #0b1419; }\nhtml[data-pbt-skin=\"gunmetal-console\"] .pbt-toast, html[data-pbt-skin=\"gunmetal-console\"] .pbt-menu { clip-path: var(--pbt-chamfer); }\n\n/* The Filters pane matches the side panes. */\nhtml[data-pbt-skin=\"gunmetal-console\"]:not(.pbt-report-filters) .outspacePane { background-image: repeating-linear-gradient(180deg, rgba(0, 0, 0, .22) 0 1px, transparent 1px 3px) !important; }\n/* New page: as tall as the page tabs, with the + centered. */\nhtml[data-pbt-skin=\"gunmetal-console\"] button.section.static.create .thumbnail-container { height: 35px !important; display: flex !important; align-items: center; justify-content: center; }\n\n/* The + centered in the New page button. */\nhtml[data-pbt-skin=\"gunmetal-console\"] button.section.static.create .thumbnail { display: flex !important; align-items: center; justify-content: center; height: 100% !important; }\nhtml[data-pbt-skin=\"gunmetal-console\"] button.section.static.create svg-use-symbol { display: flex !important; align-items: center; justify-content: center; height: 100% !important; }\n\n/* Window controls: chamfered gunmetal plates inside Power BI's own button boxes. */\nhtml[data-pbt-skin=\"gunmetal-console\"] {\n  --pbt-wc-inset: 5px 4px; --pbt-wc-radius: 0; --pbt-wc-fg: #d4dee5; --pbt-wc-close-fg: #ffb3b3; --pbt-wc-close-hover-fg: #ffd6d6;\n  --pbt-wc-face: linear-gradient(180deg, #2e3d47, #141c21); --pbt-wc-close-face: linear-gra)PBT"
LR"PBT(dient(180deg, #7a1f1f, #3d0d0d);\n}\nhtml[data-pbt-skin=\"gunmetal-console\"] .windowControls button::before { clip-path: polygon(6px 0, 100% 0, 100% calc(100% - 6px), calc(100% - 6px) 100%, 0 100%, 0 6px); }\nhtml[data-pbt-skin=\"gunmetal-console\"] .windowControls button:hover::before { filter: brightness(1.45); }\n"},"hazard-suit":{"short":"Hazard","name":"Hazard Suit","order":5,"splash":["Hazard Suit Online","Biomonitor active. Data integrity nominal."],"confetti":{"shapes":["lambda"],"colors":["#ff9d00","#ffb43a","#ffd27a","#ff6a00"]},"sound":{"wave":"square","notes":[523,784],"step":0.14,"len":0.12},"palette":{"ramp":[["#000000","#070604"],["#141414","#0f0c07"],["#292929","#19140b"],["#3d3d3d","#2a2111"],["#5c5c5c","#4a3a1c"],["#8f8f8f","#c8963c"],["#d6d6d6","#ffb43a"],["#ffffff","#ffd27a"]],"accent":"#ff9d00","vars":{"font":"Bahnschrift, \"Trebuchet MS\", sans-serif","title-font":"Bahnschrift, \"Trebuchet MS\", sans-serif","titlebar-bg":"linear-gradient(180deg, #14110c, #050403)","titlebar-fg":"#ffb43a","titlebar-hover":"rgba(255, 157, 0, .2)","icon":"#ffb43a","icon-alt":"#c8963c","search-bg":"rgba(0, 0, 0, .6)","tabs-bg":"#0a0805","tab-fg":"#e6a43c","tab-active-bg":"rgba(255, 157, 0, .16)","tab-active-fg":"#ffd27a","tab-hover-bg":"rgba(255, 157, 0, .1)","file-bg":"#120e08","file-fg":"#ffb43a","ribbon-bg":"linear-gradient(180deg, #17130c, #0d0b07)","ribbon-border":"1px solid #6b4b12","group-fg":"#ff9d00","nav-bg":"#0a0805","icon-nav":"#e6a43c","icon-active":"#ffd27a","nav-active-bg":"rgba(255, 157, 0, .14)","pagetabs-bg":"#070604","pagetab-bg":"transparent","pagetab-fg":"#e6a43c","pagetab-active-bg":"rgba(255, 157, 0, .18)","pagetab-active-fg":"#ffd27a","panehead-bg":"transparent","panehead-fg":"#ffb43a","status-bg":"#070604","status-fg":"#ffb43a","gutter":"linear-gradient(rgba(255, 157, 0, .07) 1px, transparent 1px) 0 0 / 40px 40px, linear-gradient(90deg, rgba(255, 157, 0, .07) 1px, transparent 1px) 0 0 / 40px 40px, radial-gradient(120% 90% at 50% 40%, #1d1810, #050403 75%)","ui-bg":"#0c0905","ui-fg":"#ffb43a","ui-border":"#6b4b12","ui-font":"Bahnschrift, sans-serif","ui-radius":"2px","hover-bg":"rgba(255, 157, 0, .12)","hover-ring":"rgba(255, 157, 0, .6)","hover-radius":"0","avatar-bg":"rgba(255, 157, 0, .1)","panehead-case":"uppercase","panehead-tracking":".1em"}},"css":"/* Hazard Suit: amber HUD on black, glowing text, HUD corner brackets on the panes,\n   a hazard-stripe rail along the status bar, a lambda before the file name. */\nhtml[data-pbt-skin=\"hazard-suit\"] { --pbt-glow: 0 0 6px rgba(255, 157, 0, .65); }\nhtml[data-pbt-skin=\"hazard-suit\"] div.titlebar .title { font: 600 14px Bahnschrift, sans-serif !important; letter-spacing: .08em; text-shadow: var(--pbt-glow); }\nhtml[data-pbt-skin=\"hazard-suit\"] div.titlebar .title::before { content: \"\u03bb\"; margin-right: 8px; font-weight: 700; color: #ff9d00; }\nhtml[data-pbt-skin=\"hazard-suit\"] div.titlebar { border-bottom: 1px solid #6b4b12; }\n\nhtml[data-pbt-skin=\"hazard-suit\"] #tablist button[role=\"tab\"] { text-transform: uppercase; letter-spacing: .07em; font-size: 12px !important; }\nhtml[data-pbt-skin=\"hazard-suit\"] #tablist button[role=\"tab\"][aria-selected=\"true\"] { text-shadow: var(--pbt-glow); box-shadow: inset 0 -2px 0 #ff9d00; }\nhtml[data-pbt-skin=\"hazard-suit\"] #tablist button[role=\"tab\"]::after { display: none !important; }\nhtml[data-pbt-skin=\"hazard-suit\"] [id=\"Ribbon-file\"] { text-shadow: 0 0 3px #000, 0 0 3px #000; font-weight: bold; }\n\n/* HUD brackets: four corner marks drawn with gradients. */\nhtml[data-pbt-skin=\"hazard-suit\"] {\n  --pbt-brackets:\n    linear-gradient(#ff9d00, #ff9d00) top left / 14px 2px, linear-gradient(#ff9d00, #ff9d00) top left / 2px 14px,\n    linear-gradient(#ff9d00, #ff9d00) top right / 14px 2px, linear-gradient(#ff9d00, #ff9d00) top right / 2px 14px,\n    linear-gradient(#ff9d00, #ff9d00) bottom left / 14px 2px, linear-gradient(#ff9d00, #ff9d00) bottom left / 2px 14px,\n    lin)PBT"
LR"PBT(ear-gradient(#ff9d00, #ff9d00) bottom right / 14px 2px, linear-gradient(#ff9d00, #ff9d00) bottom right / 2px 14px;\n}\nhtml[data-pbt-skin=\"hazard-suit\"] .ribbonContainer .bottomBarContainer { background: var(--pbt-brackets), var(--pbt-ribbon-bg) !important; background-repeat: no-repeat !important; }\n/* The Filters pane keeps its colors (the report sets them), so only the side panes get brackets. */\nhtml[data-pbt-skin=\"hazard-suit\"] .sidePane { background: var(--pbt-brackets), #0f0c07 !important; background-repeat: no-repeat !important; }\nhtml[data-pbt-skin=\"hazard-suit\"] :is(.sidePane, .outspacePane) { border-left: 1px solid #2a2111 !important; }\nhtml[data-pbt-skin=\"hazard-suit\"] .ribbonContainer [role=\"group\"] label[class*=\"groupLabel\"] { text-transform: uppercase; letter-spacing: .1em; font-size: 10px !important; text-shadow: var(--pbt-glow); }\nhtml[data-pbt-skin=\"hazard-suit\"] :is(.pane-header, .outspacePane .paneHeader) { text-transform: uppercase; letter-spacing: .1em; text-shadow: var(--pbt-glow); }\nhtml[data-pbt-skin=\"hazard-suit\"] .carouselScrollPane li.section.selected { box-shadow: inset 0 2px 0 #ff9d00; }\n\n/* Hazard rail above the status bar. */\nhtml[data-pbt-skin=\"hazard-suit\"] section.statusBar { border-top: 4px solid transparent; border-image: repeating-linear-gradient(-45deg, #ff9d00 0 8px, #111 8px 16px) 4; text-shadow: var(--pbt-glow); }\n\nhtml[data-pbt-skin=\"hazard-suit\"] ::-webkit-scrollbar { width: 10px; height: 10px; background: #070604; }\nhtml[data-pbt-skin=\"hazard-suit\"] ::-webkit-scrollbar-thumb { background: #6b4b12; border: 1px solid #ff9d00; }\nhtml[data-pbt-skin=\"hazard-suit\"] .pbt-toast { text-shadow: var(--pbt-glow); }\n\n/* File: an upright amber label on a dark plate inside a solid amber outline (striped, it broke\n   up into dashes at this size and read as disabled). */\nhtml[data-pbt-skin=\"hazard-suit\"] [id=\"Ribbon-file\"] { border: 2px solid #ff9d00 !important; border-image: none !important; }\nhtml[data-pbt-skin=\"hazard-suit\"] [id=\"Ribbon-file\"], html[data-pbt-skin=\"hazard-suit\"] [id=\"Ribbon-file\"] * { font-style: normal !important; text-shadow: var(--pbt-glow) !important; }\n/* The Filters pane gets the same HUD brackets as the side panes. */\nhtml[data-pbt-skin=\"hazard-suit\"]:not(.pbt-report-filters) .outspacePane { background: var(--pbt-brackets), #0f0c07 !important; background-repeat: no-repeat !important; }\n\n/* File matches its neighbours: capitals at the tab size. */\nhtml[data-pbt-skin=\"hazard-suit\"] [id=\"Ribbon-file\"] { text-transform: uppercase; letter-spacing: .07em; font-size: 12px !important; }\nhtml[data-pbt-skin=\"hazard-suit\"] [id=\"Ribbon-file\"] * { font-size: 12px !important; }\n/* Painted Filters pane: let the brackets show through its header, as on the side panes. */\nhtml[data-pbt-skin=\"hazard-suit\"]:not(.pbt-report-filters) .outspacePane .paneHeader { background-color: transparent !important; }\n\n/* Window controls: no face, an amber hover inside the box; Close turns red. */\nhtml[data-pbt-skin=\"hazard-suit\"] { --pbt-wc-radius: 0; --pbt-wc-hover-face: rgba(255, 157, 0, .2); --pbt-wc-close-hover-face: #b3261e; }\n"},"ae":{"short":"AE","name":"Analytic Endeavors","order":6,"splash":["Analytic Endeavors","On brand. Navy ground, teal action, orange accent."],"confetti":{"shapes":["square","ring"],"colors":["#009999","#00B8B8","#FF8C42","#00587C","#8474B2"]},"sound":{"wave":"sine","notes":[784,1047,1319],"step":0.1,"len":0.35},"palette":{"ramp":[["#000000","#0a1628"],["#141414","#0d0d1a"],["#292929","#1a1a2e"],["#3d3d3d","#2a3a5a"],["#5c5c5c","#3d4f73"],["#8f8f8f","#8d8d9e"],["#d6d6d6","#d6dbe6"],["#ffffff","#ffffff"]],"accent":"#009999","vars":{"font":"Inter, \"Segoe UI\", sans-serif","title-font":"Inter, \"Segoe UI\", sans-serif","titlebar-bg":"linear-gradient(90deg, #0a1628 0%, #003d56 100%)","titlebar-fg":"#ffffff","icon":"#00b8b8","icon-alt":"#b0b0b0","tabs-bg":"#0a1628","tab-fg":"#b0b0b0","tab-acti)PBT"
LR"PBT(ve-fg":"#ffffff","file-bg":"#009999","file-fg":"#ffffff","ribbon-bg":"#1a1a2e","ribbon-border":"1px solid #2a3a5a","group-fg":"#00b8b8","nav-bg":"#0d0d1a","icon-nav":"#b0b0b0","icon-active":"#00b8b8","pagetabs-bg":"#0d0d1a","pagetab-active-bg":"#1a1a2e","panehead-fg":"#ffffff","status-bg":"#0a1628","status-fg":"#b0b0b0","gutter":"linear-gradient(160deg, #0a1628 0%, #0d0d1a 55%, #0a1628 100%)","ui-bg":"#1a1a2e","ui-fg":"#ffffff","ui-border":"#2a3a5a","ui-radius":"8px","hover-bg":"rgba(0, 153, 153, .18)","hover-ring":"#009999","hover-radius":"6px"}},"css":"/* Analytic Endeavors: navy ground, teal action color, orange accent line under the chosen tab. */\nhtml[data-pbt-skin=\"ae\"] #tablist button[role=\"tab\"][aria-selected=\"true\"] { box-shadow: inset 0 -3px 0 #ff8c42; }\nhtml[data-pbt-skin=\"ae\"] #tablist button[role=\"tab\"]::after { display: none !important; }\nhtml[data-pbt-skin=\"ae\"] .ribbonContainer .bottomBarContainer { border-radius: 8px !important; }\nhtml[data-pbt-skin=\"ae\"] .carouselScrollPane li.section.selected { box-shadow: inset 0 -3px 0 #009999; }\nhtml[data-pbt-skin=\"ae\"] ::-webkit-scrollbar { width: 10px; height: 10px; background: #0d0d1a; }\nhtml[data-pbt-skin=\"ae\"] ::-webkit-scrollbar-thumb { background: #2a3a5a; border-radius: 5px; }\nhtml[data-pbt-skin=\"ae\"] ::-webkit-scrollbar-thumb:hover { background: #009999; }\n"},"starship-panel":{"short":"Starship","name":"Starship Panel","order":7,"splash":["Computer Ready","All decks report nominal. Awaiting your report."],"confetti":{"shapes":["pill"],"colors":["#ff9966","#cc99cc","#9999ff","#ffcc99","#cc6666","#ffcc66"]},"sound":{"wave":"sine","notes":[1046,1397],"step":0.07,"len":0.09},"palette":{"ramp":[["#000000","#000000"],["#141414","#05050a"],["#292929","#101018"],["#3d3d3d","#22222e"],["#5c5c5c","#3a3a4a"],["#8f8f8f","#9a8fb0"],["#d6d6d6","#f0d8b0"],["#ffffff","#ffe6c8"]],"accent":"#ff9966","vars":{"font":"Bahnschrift, \"Agency FB\", \"Segoe UI\", sans-serif","title-font":"Bahnschrift, \"Agency FB\", sans-serif","titlebar-bg":"#000000","titlebar-fg":"#ff9966","titlebar-hover":"rgba(255, 153, 102, .22)","icon":"#ff9966","icon-alt":"#cc99cc","search-bg":"#101018","tabs-bg":"#000000","tab-fg":"#000000","tab-active-bg":"#000000","tab-active-fg":"#ffcc66","tab-hover-bg":"#ffcc99","file-bg":"#cc6666","file-fg":"#000000","ribbon-bg":"#000000","group-fg":"#ffcc99","hover-bg":"rgba(255, 153, 102, .22)","hover-ring":"#ff9966","hover-radius":"14px","nav-bg":"#cc99cc","icon-nav":"#000000","icon-active":"#000000","nav-active-bg":"#ff9966","nav-hover-bg":"rgba(0, 0, 0, .15)","pagetabs-bg":"#000000","pagetab-bg":"#9999ff","pagetab-fg":"#000000","pagetab-active-bg":"#ffcc66","pagetab-active-fg":"#000000","panehead-bg":"linear-gradient(90deg, #ff9966, #ff9966)","panehead-fg":"#000000","status-bg":"#000000","status-fg":"#ffcc99","wc-glyph":"12px","ui-bg":"#000000","ui-fg":"#ffcc99","ui-border":"#cc99cc","ui-font":"Bahnschrift, sans-serif","ui-radius":"18px","share-fg":"#ff9966","panehead-case":"uppercase","panehead-tracking":".06em"}},"css":"/* Starship Panel: black ground, rounded pill buttons in orange, lilac, periwinkle and peach,\n   an elbow bar sweeping around the ribbon, condensed capitals everywhere. */\nhtml[data-pbt-skin=\"starship-panel\"] {\n  --pbt-lc-orange: #ff9966; --pbt-lc-lilac: #cc99cc; --pbt-lc-blue: #9999ff; --pbt-lc-peach: #ffcc99; --pbt-lc-red: #cc6666; --pbt-lc-gold: #ffcc66;\n}\nhtml[data-pbt-skin=\"starship-panel\"] :is(#tablist button[role=\"tab\"], [id=\"Ribbon-file\"], .ribbonContainer label[class*=\"groupLabel\"], .pane-header h2, .outspacePane .paneHeader, section.statusBar, .carouselScrollPane li.section, div.titlebar .title, .pbt-rgroup-label) {\n  text-transform: uppercase; letter-spacing: .06em; font-stretch: condensed;\n}\n\n/* Title: an orange cap before the name. */\nhtml[data-pbt-skin=\"starship-panel\"] div.titlebar .title { font-weight: 600 !important; }\nhtml[data-pbt-skin=\"starship-panel\"] div.titlebar .ti)PBT"
LR"PBT(tle::before {\n  content: \"\"; display: inline-block; width: 44px; height: 14px; margin-right: 10px; vertical-align: -2px;\n  border-radius: 999px 0 0 999px; background: var(--pbt-lc-orange);\n}\n\n\n/* Tabs: pills, each in the next console color. The open one is the only hollow pill: black with\n   a gold rim and gold letters, so it never reads as one more color in the row. */\nhtml[data-pbt-skin=\"starship-panel\"] #tablist button[role=\"tab\"] { border-radius: 999px !important; margin: 3px 2px !important; height: 25px !important; padding: 0 14px !important; font-weight: 600; }\nhtml[data-pbt-skin=\"starship-panel\"] #tablist button[role=\"tab\"]::after { display: none !important; }\n/* Any tab outside the usual tab list (contextual tabs such as Format) still gets a pill to sit on. */\nhtml[data-pbt-skin=\"starship-panel\"] .ribbonContainer button[role=\"tab\"]:not([aria-selected=\"true\"]) { background: var(--pbt-lc-peach) !important; }\nhtml[data-pbt-skin=\"starship-panel\"] #tablist .ms-OverflowSet-item:nth-child(5n+1) button[role=\"tab\"]:not([aria-selected=\"true\"]) { background: var(--pbt-lc-orange) !important; }\nhtml[data-pbt-skin=\"starship-panel\"] #tablist .ms-OverflowSet-item:nth-child(5n+2) button[role=\"tab\"]:not([aria-selected=\"true\"]) { background: var(--pbt-lc-lilac) !important; }\nhtml[data-pbt-skin=\"starship-panel\"] #tablist .ms-OverflowSet-item:nth-child(5n+3) button[role=\"tab\"]:not([aria-selected=\"true\"]) { background: var(--pbt-lc-blue) !important; }\nhtml[data-pbt-skin=\"starship-panel\"] #tablist .ms-OverflowSet-item:nth-child(5n+4) button[role=\"tab\"]:not([aria-selected=\"true\"]) { background: var(--pbt-lc-peach) !important; }\nhtml[data-pbt-skin=\"starship-panel\"] #tablist .ms-OverflowSet-item:nth-child(5n+5) button[role=\"tab\"]:not([aria-selected=\"true\"]) { background: var(--pbt-lc-red) !important; }\nhtml[data-pbt-skin=\"starship-panel\"] #tablist button[role=\"tab\"]:hover { filter: brightness(1.15); }\nhtml[data-pbt-skin=\"starship-panel\"] #tablist button[role=\"tab\"][aria-selected=\"true\"] { box-shadow: inset 0 0 0 2px var(--pbt-lc-gold); }\nhtml[data-pbt-skin=\"starship-panel\"] [id=\"Ribbon-file\"] { border-radius: 999px !important; margin: 3px 4px 3px 6px !important; height: 25px !important; padding: 0 16px !important; font-weight: 700; }\n\n/* Ribbon: the elbow. A thick lilac bar runs along the top and down the left with a rounded corner. */\nhtml[data-pbt-skin=\"starship-panel\"] .ribbonContainer .bottomBarContainer {\n  border-style: solid !important; border-color: var(--pbt-lc-lilac) !important;\n  border-width: 7px 0 0 41px !important; border-top-left-radius: 48px 30px !important; border-radius: 48px 0 0 0 / 30px 0 0 0 !important;\n}\nhtml[data-pbt-skin=\"starship-panel\"] .ribbonContainer .bottomBarContainer button:not(:disabled):hover { filter: brightness(1.15); }\nhtml[data-pbt-skin=\"starship-panel\"] .ribbonContainer [role=\"group\"] label[class*=\"groupLabel\"] { font-weight: 600; }\nhtml[data-pbt-skin=\"starship-panel\"] .ribbonContainer [class*=\"groupContainer\"] + [class*=\"groupContainer\"] { border-left: 2px solid var(--pbt-lc-blue) !important; }\n\n/* View switcher: a lilac column ending in a rounded foot. */\nhtml[data-pbt-skin=\"starship-panel\"] .viewNavigatorContainer { border-radius: 0 0 0 26px; }\nhtml[data-pbt-skin=\"starship-panel\"] view-navigator button { margin: 2px 0; border-radius: 0 14px 14px 0; }\nhtml[data-pbt-skin=\"starship-panel\"] view-navigator button[aria-selected=\"true\"] { box-shadow: none; }\n\n/* Panes: an orange header pill, black body. */\nhtml[data-pbt-skin=\"starship-panel\"] :is(.pane-header, .outspacePane .paneHeader) { border-radius: 999px 0 0 999px; margin: 4px 0 4px 4px; font-weight: 600; }\nhtml[data-pbt-skin=\"starship-panel\"] .outspacePane.outspacePaneCollapsed { border-radius: 0 0 0 22px; }\nhtml[data-pbt-skin=\"starship-panel\"] .sidePane { border-left: 4px solid #000 !important; }\n\n/* Page tabs: pi)PBT"
LR"PBT(lls; the New page button is one too. */\nhtml[data-pbt-skin=\"starship-panel\"] .carouselScrollPane li.section, html[data-pbt-skin=\"starship-panel\"] button.section.static.create .thumbnail-container { border-radius: 999px !important; margin: 3px 2px; }\nhtml[data-pbt-skin=\"starship-panel\"] button.section.static.create .thumbnail-container { background: var(--pbt-pagetab-bg) !important; box-shadow: none; }\nhtml[data-pbt-skin=\"starship-panel\"] button.section.static.create { color: #000 !important; }\n\n/* Status bar: a lilac segment leading the readouts. */\nhtml[data-pbt-skin=\"starship-panel\"] section.statusBar .leftPane::before {\n  content: \"\"; display: inline-block; width: 60px; height: 12px; margin-right: 8px; vertical-align: middle; border-radius: 999px; background: var(--pbt-lc-lilac);\n}\n\n/* Share, menu and toasts as console panels. */\nhtml[data-pbt-skin=\"starship-panel\"] .farPeripheralControls button { border-radius: 999px !important; }\nhtml[data-pbt-skin=\"starship-panel\"] .pbt-toast { border: 0; border-left: 22px solid var(--pbt-lc-orange); border-radius: 22px 6px 6px 22px; }\nhtml[data-pbt-skin=\"starship-panel\"] .pbt-menu h4 { color: var(--pbt-lc-orange); opacity: 1; text-transform: uppercase; }\nhtml[data-pbt-skin=\"starship-panel\"] .pbt-menu button:hover { background: var(--pbt-lc-orange) !important; color: #000; border-radius: 999px; --pbt-ui-fg: #000; --pbt-ui-bg: var(--pbt-lc-orange); --pbt-accent: #000; }\nhtml[data-pbt-skin=\"starship-panel\"] .pbt-menu .pbt-skins button:hover { border-radius: 6px; }\nhtml[data-pbt-skin=\"starship-panel\"] ::-webkit-scrollbar { width: 12px; height: 12px; background: #000; }\nhtml[data-pbt-skin=\"starship-panel\"] ::-webkit-scrollbar-thumb { background: var(--pbt-lc-lilac); border-radius: 999px; border: 2px solid #000; }\n\n/* The elbow bar takes 7px off the panel; lift the Skins group label clear of the bottom edge. */\nhtml[data-pbt-skin=\"starship-panel\"] .pbt-rgroup { padding-bottom: 8px; }\n\n/* A page tab's close x sits inside the pill, clear of its curved end. */\n/* The tab's close \"x\" (shown on hover and focus) gets its own room at the pill's right end, so it\n   never runs into the label (\"SALES ANALYSISX\"); lowercase and dimmed, like Power BI's own. */\nhtml[data-pbt-skin=\"starship-panel\"] .carouselScrollPane li.section .sectionDeleteIcon { position: absolute !important; top: 50% !important; right: -18px !important; transform: translateY(-50%); text-transform: none !important; opacity: .7; }\nhtml[data-pbt-skin=\"starship-panel\"] .carouselScrollPane li.section { padding: 0 26px 0 10px !important; }\n\n/* Window controls: colored pills inside Power BI's own button boxes. */\nhtml[data-pbt-skin=\"starship-panel\"] {\n  --pbt-wc-inset: 7px 5px; --pbt-wc-radius: 999px; --pbt-wc-fg: #000; --pbt-wc-close-hover-fg: #000;\n  --pbt-wc-face: var(--pbt-lc-blue); --pbt-wc-close-face: var(--pbt-lc-red);\n  --pbt-wc-hover-bg: transparent; --pbt-wc-close-hover-bg: transparent;\n}\nhtml[data-pbt-skin=\"starship-panel\"] .windowControls button:nth-child(2)::before { background: var(--pbt-lc-lilac); }\n\n/* The 8 px gutter the ribbon row keeps at its left showed black beside the lilac elbow and ended where the wider lilac rail begins; the elbow runs to the window edge instead. */\nhtml[data-pbt-skin=\"starship-panel\"] .ribbonContainer div:has(> .bottomBarContainer) { padding-left: 0 !important; }\n"},"phosphor-terminal":{"short":"Phosphor","name":"Phosphor Terminal","order":8,"splash":["TERMINAL READY","> LOAD REPORT ... OK"],"confetti":{"shapes":["bits"],"colors":["#33ff77","#66ff99","#1fbf55","#b8ffcc"]},"sound":{"wave":"square","notes":[440,880,440],"step":0.05,"len":0.05},"palette":{"mono":true,"ramp":[["#000000","#020a04"],["#141414","#041208"],["#292929","#072010"],["#3d3d3d","#0c3318"],["#5c5c5c","#145226"],["#8f8f8f","#2f9a50"],["#d6d6d6","#6dff9a"],["#ffffff","#b8ffcc"]],"accent":"#33ff77","vars":{"font":"Consolas, \"Lucida Console\",)PBT"
LR"PBT( monospace","title-font":"Consolas, \"Lucida Console\", monospace","titlebar-bg":"repeating-linear-gradient(180deg, rgba(0, 0, 0, .3) 0 1px, transparent 1px 3px), linear-gradient(180deg, #06200f, #031208)","titlebar-fg":"#66ff99","titlebar-hover":"rgba(51, 255, 119, .18)","icon":"#66ff99","icon-alt":"#2f9a50","avatar-bg":"transparent","search-bg":"#020a04","tabs-bg":"repeating-linear-gradient(180deg, rgba(0, 0, 0, .3) 0 1px, transparent 1px 3px), #031208","tab-fg":"#3ee673","tab-active-bg":"#33ff77","tab-active-fg":"#021006","tab-hover-bg":"rgba(51, 255, 119, .15)","file-bg":"transparent","file-fg":"#66ff99","ribbon-bg":"repeating-linear-gradient(180deg, rgba(0, 0, 0, .28) 0 1px, transparent 1px 3px), radial-gradient(120% 140% at 50% 40%, #0a2a14, #020a04 80%)","ribbon-border":"1px solid #1d6b36","group-fg":"#3ee673","hover-bg":"rgba(51, 255, 119, .14)","hover-ring":"#33ff77","hover-radius":"0","nav-bg":"repeating-linear-gradient(180deg, rgba(0, 0, 0, .3) 0 1px, transparent 1px 3px), #031208","icon-nav":"#3ee673","icon-active":"#021006","nav-active-bg":"#33ff77","nav-hover-bg":"rgba(51, 255, 119, .15)","pagetabs-bg":"#020a04","pagetab-bg":"transparent","pagetab-fg":"#3ee673","pagetab-active-bg":"#33ff77","pagetab-active-fg":"#021006","panehead-bg":"linear-gradient(90deg, #0c3318, #072010)","panehead-fg":"#66ff99","status-bg":"repeating-linear-gradient(180deg, rgba(0, 0, 0, .3) 0 1px, transparent 1px 3px), #031208","status-fg":"#3ee673","ui-bg":"#020a04","ui-fg":"#66ff99","ui-border":"#1d6b36","ui-font":"Consolas, monospace","ui-radius":"0","panehead-case":"uppercase","panehead-tracking":"0"}},"css":"/* Phosphor Terminal: one-color green phosphor screen. Scanlines, a soft glow on every letter, a dark\n   vignette at the edges, Office icons tinted green, inverted highlight for whatever is selected,\n   and a blinking cursor after the file name. */\nhtml[data-pbt-skin=\"phosphor-terminal\"] :is(titlebar, ribbon-report-view, view-navigator, .sidePane, .outspacePane .paneHeader, status-bar, exploration-footer-modern) {\n  text-shadow: 0 0 4px rgba(51, 255, 119, .55);\n}\n/* Selected items are inverted (dark text on green); no glow there. */\nhtml[data-pbt-skin=\"phosphor-terminal\"] :is(#tablist button[role=\"tab\"][aria-selected=\"true\"], .carouselScrollPane li.section.selected) * { text-shadow: none !important; }\n\n/* Title: a prompt and a blinking cursor. */\nhtml[data-pbt-skin=\"phosphor-terminal\"] div.titlebar .title::before { content: \"> \"; }\nhtml[data-pbt-skin=\"phosphor-terminal\"] div.titlebar .title::after { content: \"_\"; margin-left: 2px; animation: pbt-phosphor-terminal-blink 1.1s steps(1) infinite; }\n@keyframes pbt-phosphor-terminal-blink { 50% { opacity: 0; } }\n@media (prefers-reduced-motion: reduce) { html[data-pbt-skin=\"phosphor-terminal\"] div.titlebar .title::after { animation: none; } }\n\n\n/* Tabs and File in capitals; File in brackets. */\nhtml[data-pbt-skin=\"phosphor-terminal\"] #tablist button[role=\"tab\"] { text-transform: uppercase; letter-spacing: .04em; font-size: 12px !important; }\nhtml[data-pbt-skin=\"phosphor-terminal\"] #tablist button[role=\"tab\"]::after { display: none !important; }\nhtml[data-pbt-skin=\"phosphor-terminal\"] [id=\"Ribbon-file\"] { text-transform: uppercase; box-shadow: inset 0 0 0 1px #1d6b36; }\nhtml[data-pbt-skin=\"phosphor-terminal\"] [id=\"Ribbon-file\"] .ms-Button-label::before { content: \"[ \"; }\nhtml[data-pbt-skin=\"phosphor-terminal\"] [id=\"Ribbon-file\"] .ms-Button-label::after { content: \" ]\"; }\n\n/* The screen: rounded glass with a vignette. Office icons go green, except the report theme\n   previews in the Themes gallery, which must keep their real colors to be chosen. */\nhtml[data-pbt-skin=\"phosphor-terminal\"] .ribbonContainer .bottomBarContainer { border-radius: 10px !important; box-shadow: inset 0 0 40px rgba(0, 0, 0, .85), 0 0 10px rgba(51, 255, 119, .15); }\nhtml[data-pbt-skin=\"phosphor-terminal\"] .ribbonContainer .bottomBarCon)PBT"
LR"PBT(tainer :is(svg, img):not(.pbt-sw):not(#themesGroup-group *) { filter: grayscale(1) sepia(1) hue-rotate(75deg) saturate(4) brightness(1.15); }\nhtml[data-pbt-skin=\"phosphor-terminal\"] .ribbonContainer [role=\"group\"] label[class*=\"groupLabel\"] { text-transform: uppercase; letter-spacing: .06em; }\nhtml[data-pbt-skin=\"phosphor-terminal\"] .sidePane { box-shadow: inset 0 0 30px rgba(0, 0, 0, .8); background-image: repeating-linear-gradient(180deg, rgba(0, 0, 0, .22) 0 1px, transparent 1px 3px) !important; }\nhtml[data-pbt-skin=\"phosphor-terminal\"] :is(.pane-header, .outspacePane .paneHeader) { border-bottom: 1px solid #1d6b36; }\nhtml[data-pbt-skin=\"phosphor-terminal\"] :is(.pane-header, .outspacePane .paneHeader) :is(h2, .pane-title) { letter-spacing: 0; font-size: 13px !important; }\nhtml[data-pbt-skin=\"phosphor-terminal\"] :is(.sidePane, .outspacePane) { border-left: 1px solid #1d6b36 !important; }\n\n/* Page tabs: plain text until chosen, then inverted. */\nhtml[data-pbt-skin=\"phosphor-terminal\"] .carouselScrollPane li.section, html[data-pbt-skin=\"phosphor-terminal\"] button.section.static.create .thumbnail-container { box-shadow: inset 0 0 0 1px #145226 !important; }\nhtml[data-pbt-skin=\"phosphor-terminal\"] section.statusBar { border-top: 1px solid #1d6b36; }\n\nhtml[data-pbt-skin=\"phosphor-terminal\"] .pbt-menu button:hover { background: #33ff77 !important; color: #021006; text-shadow: none; --pbt-ui-fg: #021006; --pbt-ui-bg: #33ff77; --pbt-accent: #021006; }\nhtml[data-pbt-skin=\"phosphor-terminal\"] .pbt-toast { text-shadow: 0 0 4px rgba(51, 255, 119, .55); border-left-color: #33ff77; }\nhtml[data-pbt-skin=\"phosphor-terminal\"] ::-webkit-scrollbar { width: 12px; height: 12px; background: #020a04; }\nhtml[data-pbt-skin=\"phosphor-terminal\"] ::-webkit-scrollbar-thumb { background: #145226; border: 1px solid #33ff77; }\n\n/* Visual type icons in the Visualizations pane carry fixed colors (dark lines, white and blue\n   fills, the same in both modes). Inverted first, so lines turn bright and white fills go dark,\n   then tinted green by brightness: bright lines over dim fills, two tones like the ribbon. */\nhtml[data-pbt-skin=\"phosphor-terminal\"] .sidePane .visual-types-container :is(svg, img) { filter: invert(1) grayscale(1) sepia(1) hue-rotate(75deg) saturate(4) brightness(1.05); }\n/* The Filters pane matches the side panes. */\nhtml[data-pbt-skin=\"phosphor-terminal\"]:not(.pbt-report-filters) .outspacePane { box-shadow: inset 0 0 30px rgba(0, 0, 0, .8); background-image: repeating-linear-gradient(180deg, rgba(0, 0, 0, .22) 0 1px, transparent 1px 3px) !important; }\n\n/* Window controls: bracketed terminal keys inside Power BI's own button boxes; hover inverts. */\nhtml[data-pbt-skin=\"phosphor-terminal\"] {\n  --pbt-wc-inset: 5px 4px; --pbt-wc-radius: 0; --pbt-wc-ring: inset 0 0 0 1px #1d6b36;\n  --pbt-wc-hover-face: #33ff77; --pbt-wc-close-hover-face: #33ff77; --pbt-wc-close-hover-fg: #021006;\n}\nhtml[data-pbt-skin=\"phosphor-terminal\"] .windowControls button:hover { color: #021006 !important; }\n"},"grid":{"short":"Neon","name":"Neon Grid","order":9,"splash":["PROGRAM LOADED","The grid is yours. Bring your data."],"confetti":{"shapes":["streak"],"colors":["#00e5ff","#7ff6ff","#ff7a00","#ffffff"]},"sound":{"wave":"sawtooth","notes":[220,440,880],"step":0.05,"len":0.12},"palette":{"ramp":[["#000000","#000308"],["#141414","#02070f"],["#292929","#061224"],["#3d3d3d","#0b1f3a"],["#5c5c5c","#123158"],["#8f8f8f","#4a7fb0"],["#d6d6d6","#bff4ff"],["#ffffff","#e8fdff"]],"accent":"#00e5ff","vars":{"font":"Bahnschrift, \"Segoe UI\", sans-serif","title-font":"Bahnschrift, sans-serif","titlebar-bg":"linear-gradient(180deg, #02070f, #000308)","titlebar-fg":"#9ff4ff","titlebar-hover":"rgba(0, 229, 255, .16)","icon":"#00e5ff","icon-alt":"#4a7fb0","search-bg":"#02070f","tabs-bg":"#000308","tab-fg":"#7fdfff","tab-active-bg":"transparent","tab-active-fg":"#ffb066","tab-hover-bg":"rgba(0, 229, 255, .1)","f)PBT"
LR"PBT(ile-bg":"transparent","file-fg":"#ff9a3c","ribbon-bg":"linear-gradient(180deg, #030b18, #01040a)","ribbon-border":"1px solid #00e5ff","group-fg":"#00e5ff","hover-bg":"rgba(0, 229, 255, .12)","hover-ring":"#00e5ff","hover-radius":"2px","nav-bg":"#000308","icon-nav":"#4fd8ff","icon-active":"#ff9a3c","nav-active-bg":"rgba(255, 122, 0, .12)","pagetabs-bg":"#000308","pagetab-bg":"transparent","pagetab-fg":"#7fdfff","pagetab-active-bg":"rgba(255, 122, 0, .12)","pagetab-active-fg":"#ffb066","panehead-bg":"linear-gradient(180deg, #061224, #02070f)","panehead-fg":"#9ff4ff","status-bg":"#000308","status-fg":"#7fdfff","ui-bg":"#02070f","ui-fg":"#bff4ff","ui-border":"#00e5ff","ui-font":"Bahnschrift, sans-serif","ui-radius":"2px","panehead-case":"uppercase","panehead-tracking":".1em","gutter":"linear-gradient(rgba(0, 229, 255, .07) 1px, transparent 1px) 0 0 / 24px 24px, linear-gradient(90deg, rgba(0, 229, 255, .07) 1px, transparent 1px) 0 0 / 24px 24px, radial-gradient(120% 100% at 50% 0%, #04142a, #000308 70%)"}},"css":"/* Grid Runner: black space, thin neon cyan edges that glow, orange for whatever is chosen, a faint\n   grid on the canvas around the page (palette.vars gutter; never behind controls, where its lines\n   read as separators or input edges), and a light trail running along the bottom of the title bar. */\nhtml[data-pbt-skin=\"grid\"] { --pbt-glow-c: 0 0 6px rgba(0, 229, 255, .8), 0 0 14px rgba(0, 229, 255, .35); --pbt-glow-o: 0 0 6px rgba(255, 122, 0, .85), 0 0 14px rgba(255, 122, 0, .35); }\nhtml[data-pbt-skin=\"grid\"] :is(#tablist button[role=\"tab\"], .ribbonContainer label[class*=\"groupLabel\"], .pane-header h2, .outspacePane .paneHeader, div.titlebar .title) { text-transform: uppercase; letter-spacing: .1em; }\n\n/* Title bar: neon base line with a light trail running along it. The trail is a full-width strip\n   whose background moves, so nothing ever extends past the window edge. */\nhtml[data-pbt-skin=\"grid\"] div.titlebar { position: relative; box-shadow: inset 0 -1px 0 rgba(0, 229, 255, .7); }\nhtml[data-pbt-skin=\"grid\"] div.titlebar::after {\n  content: \"\"; position: absolute; left: 0; right: 0; bottom: 0; height: 2px; pointer-events: none;\n  background: linear-gradient(90deg, transparent, #00e5ff 70%, #fff) no-repeat -160px 0 / 160px 2px;\n  filter: drop-shadow(0 0 3px #00e5ff);\n  animation: pbt-grid-trail 7s linear infinite;\n}\n@keyframes pbt-grid-trail { from { background-position: -160px 0; } to { background-position: calc(100% + 160px) 0; } }\n@media (prefers-reduced-motion: reduce) { html[data-pbt-skin=\"grid\"] div.titlebar::after { animation: none; display: none; } }\nhtml[data-pbt-skin=\"grid\"] div.titlebar .title { text-shadow: 0 0 6px rgba(0, 229, 255, .8); font-weight: 600 !important; }\n\n\n/* Tabs: cyan text, the open tab orange with a glowing underline. */\nhtml[data-pbt-skin=\"grid\"] #tablist button[role=\"tab\"] { text-shadow: 0 0 4px rgba(0, 229, 255, .5); font-size: 12px !important; }\nhtml[data-pbt-skin=\"grid\"] #tablist button[role=\"tab\"]::after { display: none !important; }\nhtml[data-pbt-skin=\"grid\"] #tablist button[role=\"tab\"][aria-selected=\"true\"] { box-shadow: inset 0 -2px 0 #ff7a00, 0 8px 10px -8px #ff7a00; text-shadow: 0 0 6px rgba(255, 122, 0, .8); }\nhtml[data-pbt-skin=\"grid\"] [id=\"Ribbon-file\"] { box-shadow: inset 0 0 0 1px #ff7a00; text-shadow: 0 0 6px rgba(255, 122, 0, .8); text-transform: uppercase; letter-spacing: .1em; }\nhtml[data-pbt-skin=\"grid\"] .ribbonTopBarContainer { box-shadow: inset 0 -1px 0 rgba(0, 229, 255, .25); }\n\n/* Ribbon: a glowing frame. */\nhtml[data-pbt-skin=\"grid\"] .ribbonContainer .bottomBarContainer { border-radius: 4px !important; box-shadow: var(--pbt-glow-c), inset 0 0 14px rgba(0, 229, 255, .12); }\nhtml[data-pbt-skin=\"grid\"] .ribbonContainer .bottomBarContainer button:not(:disabled):hover { box-shadow: inset 0 0 0 1px #00e5ff, var(--pbt-glow-c) !important; }\nhtml[data-pbt-skin=\"grid\"] .ribbonContainer [rol)PBT"
LR"PBT(e=\"group\"] label[class*=\"groupLabel\"] { font-size: 10px !important; text-shadow: 0 0 4px rgba(0, 229, 255, .6); }\nhtml[data-pbt-skin=\"grid\"] .ribbonContainer [class*=\"groupContainer\"] + [class*=\"groupContainer\"] { border-left: 1px solid rgba(0, 229, 255, .35) !important; }\n\n/* Panes and the view switcher: neon edges. */\nhtml[data-pbt-skin=\"grid\"] :is(.sidePane, .outspacePane) { border-left: 1px solid #00e5ff !important; box-shadow: -2px 0 8px -2px rgba(0, 229, 255, .6); }\nhtml[data-pbt-skin=\"grid\"] :is(.pane-header, .outspacePane .paneHeader) { box-shadow: inset 0 -1px 0 rgba(0, 229, 255, .6); text-shadow: 0 0 4px rgba(0, 229, 255, .6); }\nhtml[data-pbt-skin=\"grid\"] .viewNavigatorContainer { box-shadow: inset -1px 0 0 rgba(0, 229, 255, .6); }\nhtml[data-pbt-skin=\"grid\"] view-navigator button[aria-selected=\"true\"] { box-shadow: inset 3px 0 0 #ff7a00; }\n\n/* Page tabs: outlined; the open one orange. */\nhtml[data-pbt-skin=\"grid\"] .carouselScrollPane li.section, html[data-pbt-skin=\"grid\"] button.section.static.create .thumbnail-container { box-shadow: inset 0 0 0 1px rgba(0, 229, 255, .5) !important; }\nhtml[data-pbt-skin=\"grid\"] .carouselScrollPane li.section.selected { box-shadow: inset 0 0 0 1px #ff7a00, var(--pbt-glow-o) !important; }\nhtml[data-pbt-skin=\"grid\"] section.statusBar { box-shadow: inset 0 1px 0 rgba(0, 229, 255, .7), 0 -2px 8px -2px rgba(0, 229, 255, .5); }\n\nhtml[data-pbt-skin=\"grid\"] .pbt-toast, html[data-pbt-skin=\"grid\"] .pbt-menu { box-shadow: var(--pbt-glow-c); }\nhtml[data-pbt-skin=\"grid\"] .pbt-toast { border-left-color: #ff7a00; }\nhtml[data-pbt-skin=\"grid\"] ::-webkit-scrollbar { width: 10px; height: 10px; background: #000308; }\nhtml[data-pbt-skin=\"grid\"] ::-webkit-scrollbar-thumb { background: #0b1f3a; border: 1px solid #00e5ff; border-radius: 5px; }\n\n/* The open page tab is marked in orange, like the open ribbon tab. */\nhtml[data-pbt-skin=\"grid\"] .carouselScrollPane li.section.selected { border-bottom-color: #ff7a00 !important; }\n\n/* Window controls: neon outlines inside Power BI's own button boxes; Close burns orange. */\nhtml[data-pbt-skin=\"grid\"] {\n  --pbt-wc-inset: 5px 4px; --pbt-wc-radius: 2px; --pbt-wc-ring: inset 0 0 0 1px rgba(0, 229, 255, .7);\n  --pbt-wc-hover-face: rgba(0, 229, 255, .12); --pbt-wc-close-hover-face: rgba(255, 122, 0, .14); --pbt-wc-close-hover-fg: #ffb066;\n}\nhtml[data-pbt-skin=\"grid\"] .windowControls button:hover::before { box-shadow: inset 0 0 0 1px #00e5ff, var(--pbt-glow-c); }\nhtml[data-pbt-skin=\"grid\"] .windowControls button.closeButton:hover::before { box-shadow: inset 0 0 0 1px #ff7a00, var(--pbt-glow-o); }\n\n/* Disabled Undo and Redo: the cyan icons sit at the edge of visibility at the base opacity. */\nhtml[data-pbt-skin=\"grid\"] titlebar-toolbar button.is-disabled { opacity: .82; }\n"},"ink":{"short":"Ink","name":"High Contrast Ink","order":10,"splash":["High Contrast Ink","Black on white. Every word at 7:1 or better."],"confetti":{"shapes":["square"],"colors":["#000000","#ffd400","#0033a0","#595959"]},"sound":{"wave":"triangle","notes":[880,660,880],"step":0.08,"len":0.12},"palette":{"ramp":[["#000000","#ffffff"],["#141414","#ffffff"],["#292929","#f5f5f5"],["#3d3d3d","#e0e0e0"],["#5c5c5c","#6e6e6e"],["#8f8f8f","#262626"],["#d6d6d6","#000000"],["#ffffff","#000000"]],"accent":"#0033a0","vars":{"font":"\"Segoe UI\", sans-serif","title-font":"\"Segoe UI\", sans-serif","titlebar-bg":"#ffffff","titlebar-fg":"#000000","titlebar-hover":"#e0e0e0","icon":"#000000","icon-alt":"#000000","icon-fill":"#ffffff","icon-line":"#000000","search-bg":"#ffffff","search-fg":"#000000","tabs-bg":"#ffffff","tab-fg":"#000000","tab-active-bg":"#000000","tab-active-fg":"#ffffff","tab-hover-bg":"#e0e0e0","file-bg":"#ffffff","file-fg":"#000000","ribbon-bg":"#ffffff","ribbon-border":"2px solid #000000","ribbon-fg":"#000000","ribbon-fg-disabled":"#525252","group-fg":"#000000","nav-bg":"#ffffff","icon-nav":"#000000","icon-act)PBT"
LR"PBT(ive":"#ffffff","nav-active-bg":"#000000","nav-hover-bg":"#e0e0e0","pagetabs-bg":"#ffffff","pagetab-bg":"#ffffff","pagetab-fg":"#000000","pagetab-active-bg":"#000000","pagetab-active-fg":"#ffffff","panehead-bg":"#ffffff","panehead-solid":"#ffffff","panehead-fg":"#000000","status-bg":"#ffffff","status-fg":"#000000","share-fg":"#000000","gutter":"#ececec","ui-bg":"#ffffff","ui-fg":"#000000","ui-border":"#000000","ui-font":"\"Segoe UI\", sans-serif","ui-radius":"0","ui-shadow":"0 0 0 1px #000000, 0 6px 18px rgba(0, 0, 0, .35)","hover-bg":"#e0e0e0","hover-ring":"#000000","hover-radius":"0","avatar-bg":"transparent"}},"css":"/* High Contrast Ink: black on white, every word at 7:1 or better, 2 px black rules, a yellow focus\n   ring, no gradients, glow or shadows on the chrome, and UI text a size up. The open tab, view and\n   page are inverted (white on black), one marker each. */\n\n/* Focus: a yellow ring with a black edge outside it, so it shows on white and on black alike.\n   Fluent draws its own focus outline from these tokens. */\nhtml[data-pbt-skin=\"ink\"][data-theme], html[data-pbt-skin=\"ink\"] .fui-FluentProvider { --colorStrokeFocus1: #000000 !important; --colorStrokeFocus2: #ffd400 !important; }\nhtml[data-pbt-skin=\"ink\"] :focus-visible { outline: 3px solid #ffd400 !important; outline-offset: 0 !important; box-shadow: 0 0 0 5px #000 !important; }\n\n/* Disabled text stays at 7:1 too, on white and on the grey panels: a dark grey in place of the\n   half-strength mix the base pins and of the half opacity the Filters pane's Apply button takes. */\nhtml[data-pbt-skin=\"ink\"][data-theme] { --colorNeutralForegroundDisabled: #525252 !important; --colorNeutralForegroundDisabled2: #525252 !important; --colorNeutralForeground4: #525252 !important; }\nhtml[data-pbt-skin=\"ink\"] .outspacePane button.applyFilter:disabled { opacity: 1 !important; color: #525252 !important; }\n\n/* Title bar and tabs: a black rule under each row; text a size up. */\nhtml[data-pbt-skin=\"ink\"] div.titlebar { border-bottom: 2px solid #000; }\nhtml[data-pbt-skin=\"ink\"] div.titlebar .title { font-weight: 700 !important; }\nhtml[data-pbt-skin=\"ink\"] #tablist button[role=\"tab\"] { font-size: 14px !important; font-weight: 600; }\nhtml[data-pbt-skin=\"ink\"] #tablist button[role=\"tab\"]::after { display: none !important; }\nhtml[data-pbt-skin=\"ink\"] #tablist button[role=\"tab\"]:hover:not([aria-selected=\"true\"]) { box-shadow: inset 0 0 0 2px #000; }\nhtml[data-pbt-skin=\"ink\"] [id=\"Ribbon-file\"] { box-shadow: inset 0 0 0 2px #000; font-size: 14px !important; font-weight: 700; }\nhtml[data-pbt-skin=\"ink\"] [id=\"Ribbon-file\"] * { font-size: 14px !important; }\n\n/* Ribbon: a plain white panel inside a 2 px black frame; group labels a size up, divided by rules. */\nhtml[data-pbt-skin=\"ink\"] .ribbonContainer .bottomBarContainer { border-radius: 0 !important; box-shadow: none !important; }\nhtml[data-pbt-skin=\"ink\"] .ribbonContainer [role=\"group\"] label[class*=\"groupLabel\"] { font-size: 12px !important; font-weight: 600; }\nhtml[data-pbt-skin=\"ink\"] .ribbonContainer [class*=\"groupContainer\"] + [class*=\"groupContainer\"] { border-left: 2px solid #000 !important; }\nhtml[data-pbt-skin=\"ink\"] .ribbonContainer .bottomBarContainer button:not(:disabled):not(.is-disabled):not(.pbt-rbtn):hover { box-shadow: inset 0 0 0 2px #000 !important; }\n\n/* Panes: 2 px rules; titles a size up. */\nhtml[data-pbt-skin=\"ink\"] :is(.sidePane, .outspacePane) { border-left: 2px solid #000 !important; }\nhtml[data-pbt-skin=\"ink\"] :is(.pane-header, .outspacePane .paneHeader) { border-bottom: 2px solid #000; }\nhtml[data-pbt-skin=\"ink\"] :is(.pane-header, .outspacePane .paneHeader) :is(h2, .pane-title) { font-size: 16px !important; font-weight: 700 !important; }\nhtml[data-pbt-skin=\"ink\"] .viewNavigatorContainer { box-shadow: inset -2px 0 0 #000; }\nhtml[data-pbt-skin=\"ink\"] view-navigator button[aria-selected=\"true\"] { box-shadow: )PBT"
LR"PBT(none; }\n\n/* Page tabs: 2 px outlined boxes; the open one inverted. Status bar under a rule. */\nhtml[data-pbt-skin=\"ink\"] .carouselScrollPane li.section, html[data-pbt-skin=\"ink\"] button.section.static.create .thumbnail-container { box-shadow: inset 0 0 0 2px #000 !important; }\nhtml[data-pbt-skin=\"ink\"] .carouselScrollPane li.section * { font-size: 13px !important; }\nhtml[data-pbt-skin=\"ink\"] section.statusBar { border-top: 2px solid #000; font-size: 13px; }\n\n/* Our own UI a size up, square, with a 2 px black edge. */\nhtml[data-pbt-skin=\"ink\"] :is(.pbt-menu, .pbt-help) { font-size: 14px; border-width: 2px !important; }\nhtml[data-pbt-skin=\"ink\"] .pbt-tip { font-size: 13px; border-width: 2px !important; }\nhtml[data-pbt-skin=\"ink\"] .pbt-toast { border: 2px solid #000; border-left-width: 6px; box-shadow: none; }\nhtml[data-pbt-skin=\"ink\"] .pbt-menu button:hover { background: #000 !important; color: #fff; --pbt-ui-fg: #fff; --pbt-accent: #ffd400; }\nhtml[data-pbt-skin=\"ink\"] .pbt-chip:is(:hover, :focus-visible) { background: #000 !important; color: #fff !important; }\n\nhtml[data-pbt-skin=\"ink\"] ::-webkit-scrollbar { width: 14px; height: 14px; background: #fff; }\nhtml[data-pbt-skin=\"ink\"] ::-webkit-scrollbar-thumb { background: #000; border: 3px solid #fff; }\n\n/* Window controls: black outlined squares inside Power BI's own button boxes; hover inverts. */\nhtml[data-pbt-skin=\"ink\"] {\n  --pbt-wc-inset: 5px 6px; --pbt-wc-radius: 0; --pbt-wc-fg: #000; --pbt-wc-close-fg: #000;\n  --pbt-wc-ring: inset 0 0 0 2px #000; --pbt-wc-hover-face: #000; --pbt-wc-close-hover-face: #b00020; --pbt-wc-close-hover-fg: #fff;\n  --pbt-wc-hover-bg: transparent; --pbt-wc-close-hover-bg: transparent;\n}\nhtml[data-pbt-skin=\"ink\"] .windowControls button:hover { color: #fff !important; }\n"},"graphpaper":{"short":"Paper","name":"Graph Paper","order":11,"splash":["Graph Paper","Sharp pencil, red pen, a fresh pad."],"confetti":{"shapes":["square","streak"],"colors":["#c8102e","#3a6aa8","#45484d","#7fa7d9"]},"sound":{"wave":"triangle","notes":[523,659,784],"step":0.09,"len":0.18},"palette":{"ramp":[["#000000","#f3eee1"],["#141414","#f7f3e8"],["#292929","#fffdf7"],["#3d3d3d","#e4dccb"],["#5c5c5c","#b5ab98"],["#8f8f8f","#66625a"],["#d6d6d6","#45484d"],["#ffffff","#33363a"]],"accent":"#c8102e","vars":{"font":"\"Segoe UI\", sans-serif","title-font":"\"Segoe Print\", \"Segoe UI\", sans-serif","titlebar-bg":"#efe9da","titlebar-fg":"#33363a","titlebar-hover":"rgba(58, 106, 168, .14)","icon":"#33363a","icon-alt":"#66625a","icon-fill":"#fffdf7","search-bg":"#fffdf7","search-fg":"#33363a","tabs-bg":"#efe9da","tab-fg":"#45484d","tab-active-bg":"#fffdf7","tab-active-fg":"#b00d26","tab-hover-bg":"rgba(58, 106, 168, .12)","file-bg":"#fffdf7","file-fg":"#b00d26","ribbon-bg":"#fffdf7","ribbon-border":"1px solid #c9d8ea","group-fg":"#3a6aa8","nav-bg":"#efe9da","icon-nav":"#45484d","icon-active":"#b00d26","nav-active-bg":"rgba(200, 16, 46, .08)","nav-hover-bg":"rgba(58, 106, 168, .12)","pagetabs-bg":"#efe9da","pagetab-bg":"#f7f3e8","pagetab-fg":"#45484d","pagetab-active-bg":"#fffdf7","pagetab-active-fg":"#b00d26","panehead-bg":"#efe9da","panehead-solid":"#efe9da","panehead-fg":"#33363a","status-bg":"#efe9da","status-fg":"#45484d","gutter":"#e6dfcd","ui-bg":"#fffdf7","ui-fg":"#33363a","ui-border":"#b5ab98","ui-font":"\"Segoe UI\", sans-serif","ui-radius":"2px","hover-bg":"rgba(58, 106, 168, .12)","hover-ring":"#3a6aa8","hover-radius":"2px","avatar-bg":"transparent"}},"css":"/* Graph Paper: warm off-white paper, pencil-grey text, a red pen for whatever is chosen, and a\n   faint blue engineering grid in the panes. The grid is paper, never a control: inputs, lists,\n   field wells, filter cards and the visual icons sit on plain sheets above it, so its lines never\n   read as separators or input edges. Flat; no gradients on the chrome. */\nhtml[data-pbt-skin=\"graphpaper\"] {\n  --pbt-gp-sheet: #fffdf7;\n  --pbt-gp-grid:\n    linear)PBT"
LR"PBT(-gradient(rgba(58, 106, 168, .2) 1px, transparent 1px) 0 0 / 80px 80px,\n    linear-gradient(90deg, rgba(58, 106, 168, .2) 1px, transparent 1px) 0 0 / 80px 80px,\n    linear-gradient(rgba(58, 106, 168, .09) 1px, transparent 1px) 0 0 / 16px 16px,\n    linear-gradient(90deg, rgba(58, 106, 168, .09) 1px, transparent 1px) 0 0 / 16px 16px;\n}\n\n/* Title: handwritten. */\nhtml[data-pbt-skin=\"graphpaper\"] div.titlebar { border-bottom: 1px solid #c9d8ea; }\nhtml[data-pbt-skin=\"graphpaper\"] div.titlebar .title { font: 15px \"Segoe Print\", \"Segoe UI\", sans-serif !important; }\n\n/* Tabs: the open one underlined in red pen. File is written in red inside a pencil box. */\nhtml[data-pbt-skin=\"graphpaper\"] #tablist button[role=\"tab\"]::after { display: none !important; }\nhtml[data-pbt-skin=\"graphpaper\"] #tablist button[role=\"tab\"][aria-selected=\"true\"] { box-shadow: inset 0 -2px 0 #c8102e, inset 1px 0 0 #c9d8ea, inset -1px 0 0 #c9d8ea; font-weight: 600; }\nhtml[data-pbt-skin=\"graphpaper\"] [id=\"Ribbon-file\"] { box-shadow: inset 0 0 0 1px #b00d26; font-weight: 600; }\n\n/* Ribbon: a clean sheet with blue rules between groups. */\nhtml[data-pbt-skin=\"graphpaper\"] .ribbonContainer .bottomBarContainer { border-radius: 2px !important; }\nhtml[data-pbt-skin=\"graphpaper\"] .ribbonContainer [class*=\"groupContainer\"] + [class*=\"groupContainer\"] { border-left: 1px solid #c9d8ea !important; }\nhtml[data-pbt-skin=\"graphpaper\"] .ribbonContainer svg .OfficeIconColors_m22 { fill: #45484d !important; }\n\n/* Panes: the grid on the pane itself, a red margin rule under each title. */\nhtml[data-pbt-skin=\"graphpaper\"] .sidePane,\nhtml[data-pbt-skin=\"graphpaper\"]:not(.pbt-report-filters) .outspacePane { background: var(--pbt-gp-grid), #f5f1e5 !important; }\nhtml[data-pbt-skin=\"graphpaper\"] :is(.sidePane, .outspacePane) { border-left: 1px solid #c9d8ea !important; }\nhtml[data-pbt-skin=\"graphpaper\"] :is(.pane-header, .outspacePane .paneHeader) { border-bottom: 1px solid rgba(200, 16, 46, .55); }\nhtml[data-pbt-skin=\"graphpaper\"] :is(.pane-header, .outspacePane .paneHeader) :is(h2, .pane-title) { font-weight: 600 !important; }\n/* Pane headers and the Filters pane's label rows (\"Filters on this visual\") on plain paper, so no\n   grid line runs through their words. */\nhtml[data-pbt-skin=\"graphpaper\"] :is(.pane-header, .outspacePane .paneHeader) { background-image: none !important; }\nhtml[data-pbt-skin=\"graphpaper\"]:not(.pbt-report-filters) .outspacePane section.bucket > .header { background-color: #f5f1e5 !important; box-shadow: 0 0 0 4px #f5f1e5; }\n/* Plain sheets for everything a person clicks or types in, so the grid only shows on empty paper.\n   Hovered and chosen items keep their own highlight. */\nhtml[data-pbt-skin=\"graphpaper\"] .sidePane :is(input, .field-list-search-container, .visual-types-container, field-well-bucket-modern, li.fieldWellProperty, cdk-tree-node, [role=\"tablist\"], [role=\"textbox\"], [role=\"combobox\"], [role=\"switch\"]):not(:hover):not([aria-selected=\"true\"]):not([aria-checked=\"true\"]),\nhtml[data-pbt-skin=\"graphpaper\"]:not(.pbt-report-filters) .outspacePane :is(.card, mat-form-field.searchBox, search-box-modern, input, mat-select):not(:hover) {\n  background-color: var(--pbt-gp-sheet) !important;\n}\nhtml[data-pbt-skin=\"graphpaper\"] .sidePane .visual-types-container { box-shadow: 0 0 0 4px var(--pbt-gp-sheet); }\n\n/* Page tabs: index cards; the open one underlined in red. */\nhtml[data-pbt-skin=\"graphpaper\"] .carouselScrollPane li.section, html[data-pbt-skin=\"graphpaper\"] button.section.static.create .thumbnail-container { box-shadow: inset 0 0 0 1px #c9d8ea !important; }\nhtml[data-pbt-skin=\"graphpaper\"] .carouselScrollPane li.section.selected { box-shadow: inset 0 0 0 1px #c9d8ea, inset 0 -2px 0 #c8102e !important; }\nhtml[data-pbt-skin=\"graphpaper\"] section.statusBar { border-top: 1px solid #c9d8ea; }\n\n/* Our own UI: a sheet of the same paper, red)PBT"
LR"PBT( pen headings. */\nhtml[data-pbt-skin=\"graphpaper\"] .pbt-menu h4 { color: #b00d26; opacity: 1; }\nhtml[data-pbt-skin=\"graphpaper\"] .pbt-toast { border-left-color: #c8102e; }\nhtml[data-pbt-skin=\"graphpaper\"] .pbt-splash > div { background: var(--pbt-gp-grid), #fffdf7; color: #33363a; }\n\nhtml[data-pbt-skin=\"graphpaper\"] ::-webkit-scrollbar { width: 12px; height: 12px; background: #f3eee1; }\nhtml[data-pbt-skin=\"graphpaper\"] ::-webkit-scrollbar-thumb { background: #c4bba8; border: 3px solid #f3eee1; border-radius: 6px; }\n\n/* Window controls: pencil outlines inside Power BI's own button boxes; Close turns red pen. */\nhtml[data-pbt-skin=\"graphpaper\"] {\n  --pbt-wc-inset: 5px 6px; --pbt-wc-radius: 2px; --pbt-wc-fg: #33363a; --pbt-wc-close-fg: #33363a;\n  --pbt-wc-ring: inset 0 0 0 1px #b5ab98; --pbt-wc-hover-face: rgba(58, 106, 168, .14); --pbt-wc-close-hover-face: #c8102e; --pbt-wc-close-hover-fg: #fff;\n  --pbt-wc-hover-bg: transparent; --pbt-wc-close-hover-bg: transparent;\n}\n\n/* Disabled Undo and Redo: pencil grey at the base opacity sits at the edge of visibility. */\nhtml[data-pbt-skin=\"graphpaper\"] titlebar-toolbar button.is-disabled { opacity: .86; }\n"},"orchid":{"short":"Midnight","name":"Midnight Orchid","order":12,"splash":["Midnight Orchid","Night shift. Pink and purple on deep blue-grey."],"confetti":{"shapes":["ring","square"],"colors":["#ff79c6","#bd93f9","#8be9fd","#50fa7b","#f1fa8c"]},"sound":{"wave":"sine","notes":[587,740,880,1175],"step":0.1,"len":0.3},"palette":{"ramp":[["#000000","#21222c"],["#141414","#282a36"],["#292929","#343746"],["#3d3d3d","#44475a"],["#5c5c5c","#565a72"],["#8f8f8f","#a4abcf"],["#d6d6d6","#e6e6e0"],["#ffffff","#f8f8f2"]],"accent":"#bd93f9","vars":{"font":"\"Segoe UI\", sans-serif","title-font":"\"Segoe UI\", sans-serif","titlebar-bg":"#191a21","titlebar-fg":"#f8f8f2","titlebar-hover":"rgba(189, 147, 249, .18)","icon":"#f8f8f2","icon-alt":"#bd93f9","search-bg":"#282a36","search-fg":"#f8f8f2","tabs-bg":"#191a21","tab-fg":"#d0d2e4","tab-active-bg":"#282a36","tab-active-fg":"#ff79c6","tab-hover-bg":"rgba(98, 114, 164, .3)","file-bg":"#bd93f9","file-fg":"#191a21","ribbon-bg":"#282a36","ribbon-border":"1px solid #44475a","group-fg":"#bd93f9","nav-bg":"#21222c","icon-nav":"#a4abcf","icon-active":"#ff79c6","nav-active-bg":"rgba(255, 121, 198, .12)","nav-hover-bg":"rgba(98, 114, 164, .3)","pagetabs-bg":"#21222c","pagetab-bg":"#282a36","pagetab-fg":"#d0d2e4","pagetab-active-bg":"#44475a","pagetab-active-fg":"#f8f8f2","panehead-bg":"#21222c","panehead-solid":"#21222c","panehead-fg":"#bd93f9","status-bg":"#191a21","status-fg":"#d0d2e4","gutter":"radial-gradient(120% 90% at 50% 0%, #2e3040, #191a21 75%)","ui-bg":"#282a36","ui-fg":"#f8f8f2","ui-border":"#6272a4","ui-font":"\"Segoe UI\", sans-serif","ui-radius":"6px","hover-bg":"rgba(189, 147, 249, .16)","hover-ring":"#bd93f9","hover-radius":"4px","avatar-bg":"rgba(189, 147, 249, .15)"}},"css":"/* Midnight Orchid: a deep blue-grey night (#282A36) with soft white text, purple for structure\n   (group labels, pane titles, File) and pink for whatever is open or chosen. Flat panels, rounded\n   corners, no texture. */\n\n/* Title: a pink dot before the file name. */\nhtml[data-pbt-skin=\"orchid\"] div.titlebar { border-bottom: 1px solid #44475a; }\nhtml[data-pbt-skin=\"orchid\"] div.titlebar .title { font-weight: 600 !important; }\nhtml[data-pbt-skin=\"orchid\"] div.titlebar .title::before { content: \"\"; display: inline-block; width: 8px; height: 8px; margin-right: 8px; border-radius: 50%; background: #ff79c6; vertical-align: 1px; }\n\n/* Tabs: the open one with a pink underline. File is a purple pill. */\nhtml[data-pbt-skin=\"orchid\"] #tablist button[role=\"tab\"]::after { display: none !important; }\nhtml[data-pbt-skin=\"orchid\"] #tablist button[role=\"tab\"] { border-radius: 6px 6px 0 0; }\nhtml[data-pbt-skin=\"orchid\"] #tablist button[role=\"tab\"][aria-selected=\"true\"] { box-shadow: inset 0 -2px 0 #ff79c6; font-)PBT"
LR"PBT(weight: 600; }\nhtml[data-pbt-skin=\"orchid\"] [id=\"Ribbon-file\"] { border-radius: 6px !important; margin-right: 4px !important; font-weight: 600; }\n\n/* Ribbon: a rounded panel; group rules in the comment blue-grey. */\nhtml[data-pbt-skin=\"orchid\"] .ribbonContainer .bottomBarContainer { border-radius: 8px !important; }\nhtml[data-pbt-skin=\"orchid\"] .ribbonContainer [class*=\"groupContainer\"] + [class*=\"groupContainer\"] { border-left: 1px solid #44475a !important; }\n\n/* Panes: a quiet edge; titles purple with a thin pink rule. */\nhtml[data-pbt-skin=\"orchid\"] :is(.sidePane, .outspacePane) { border-left: 1px solid #44475a !important; }\nhtml[data-pbt-skin=\"orchid\"] :is(.pane-header, .outspacePane .paneHeader) { border-bottom: 1px solid rgba(255, 121, 198, .45); }\nhtml[data-pbt-skin=\"orchid\"] :is(.pane-header, .outspacePane .paneHeader) :is(h2, .pane-title) { font-weight: 600 !important; }\nhtml[data-pbt-skin=\"orchid\"] view-navigator button[aria-selected=\"true\"] { box-shadow: inset 3px 0 0 #ff79c6; }\n\n/* Page tabs: rounded tops; the open one marked in pink. */\nhtml[data-pbt-skin=\"orchid\"] .carouselScrollPane li.section, html[data-pbt-skin=\"orchid\"] button.section.static.create .thumbnail-container { border-radius: 6px 6px 0 0 !important; }\nhtml[data-pbt-skin=\"orchid\"] .carouselScrollPane li.section.selected { box-shadow: inset 0 -2px 0 #ff79c6 !important; }\nhtml[data-pbt-skin=\"orchid\"] section.statusBar { border-top: 1px solid #44475a; }\n\nhtml[data-pbt-skin=\"orchid\"] .pbt-menu h4 { color: #bd93f9; opacity: 1; }\nhtml[data-pbt-skin=\"orchid\"] .pbt-toast { border-left-color: #ff79c6; }\nhtml[data-pbt-skin=\"orchid\"] ::-webkit-scrollbar { width: 10px; height: 10px; background: #21222c; }\nhtml[data-pbt-skin=\"orchid\"] ::-webkit-scrollbar-thumb { background: #44475a; border-radius: 5px; }\nhtml[data-pbt-skin=\"orchid\"] ::-webkit-scrollbar-thumb:hover { background: #6272a4; }\n\n/* Window controls: soft rounded faces inside Power BI's own button boxes; Close turns red. */\nhtml[data-pbt-skin=\"orchid\"] {\n  --pbt-wc-inset: 6px 8px; --pbt-wc-radius: 6px; --pbt-wc-fg: #f8f8f2; --pbt-wc-close-fg: #f8f8f2;\n  --pbt-wc-hover-face: #44475a; --pbt-wc-close-hover-face: #ff5555; --pbt-wc-close-hover-fg: #191a21;\n  --pbt-wc-hover-bg: transparent; --pbt-wc-close-hover-bg: transparent;\n}\n"},"frost":{"short":"Frost","name":"Arctic Frost","order":13,"splash":["Arctic Frost","Polar night, snow-white text, frost-blue edges."],"confetti":{"shapes":["crystal","square"],"colors":["#88c0d0","#81a1c1","#eceff4","#8fbcbb","#b48ead"]},"sound":{"wave":"sine","notes":[988,1319,1568],"step":0.11,"len":0.4},"palette":{"ramp":[["#000000","#272c36"],["#141414","#2e3440"],["#292929","#3b4252"],["#3d3d3d","#434c5e"],["#5c5c5c","#4c566a"],["#8f8f8f","#aab4c8"],["#d6d6d6","#d8dee9"],["#ffffff","#eceff4"]],"accent":"#88c0d0","vars":{"font":"\"Segoe UI\", sans-serif","title-font":"\"Segoe UI\", sans-serif","titlebar-bg":"#242933","titlebar-fg":"#eceff4","titlebar-hover":"rgba(136, 192, 208, .16)","icon":"#d8dee9","icon-alt":"#88c0d0","search-bg":"#2e3440","search-fg":"#eceff4","tabs-bg":"#242933","tab-fg":"#d8dee9","tab-active-bg":"#2e3440","tab-active-fg":"#88c0d0","tab-hover-bg":"rgba(136, 192, 208, .12)","file-bg":"#88c0d0","file-fg":"#242933","ribbon-bg":"#2e3440","ribbon-border":"1px solid #434c5e","group-fg":"#88c0d0","nav-bg":"#242933","icon-nav":"#aab4c8","icon-active":"#88c0d0","nav-active-bg":"rgba(136, 192, 208, .14)","nav-hover-bg":"rgba(136, 192, 208, .12)","pagetabs-bg":"#242933","pagetab-bg":"#2e3440","pagetab-fg":"#d8dee9","pagetab-active-bg":"#3b4252","pagetab-active-fg":"#eceff4","panehead-bg":"#2e3440","panehead-solid":"#2e3440","panehead-fg":"#eceff4","status-bg":"#242933","status-fg":"#d8dee9","gutter":"radial-gradient(120% 80% at 80% 0%, #354152, #242933 70%)","ui-bg":"#2e3440","ui-fg":"#eceff4","ui-border":"#4c566a","ui-font":"\"Segoe UI\", sans-serif","ui-radius":"4px","hover-bg":"rgba)PBT"
LR"PBT((136, 192, 208, .14)","hover-ring":"#88c0d0","hover-radius":"4px","avatar-bg":"rgba(136, 192, 208, .12)"}},"css":"/* Arctic Frost: polar-night panes (#2E3440), snow-white text, frost blue (#88C0D0) for whatever is\n   open or chosen and for the structure, a thin frost line along the top of each panel. Flat and\n   quiet; no texture. */\n\n/* Title bar: a frost line along the bottom. */\nhtml[data-pbt-skin=\"frost\"] div.titlebar { border-bottom: 1px solid #3b4252; box-shadow: inset 0 -1px 0 rgba(136, 192, 208, .25); }\nhtml[data-pbt-skin=\"frost\"] div.titlebar .title { font-weight: 600 !important; letter-spacing: .02em; }\n\n/* Tabs: the open one with a frost-blue underline. File is a frost-blue key. */\nhtml[data-pbt-skin=\"frost\"] #tablist button[role=\"tab\"]::after { display: none !important; }\nhtml[data-pbt-skin=\"frost\"] #tablist button[role=\"tab\"][aria-selected=\"true\"] { box-shadow: inset 0 -2px 0 #88c0d0; font-weight: 600; }\nhtml[data-pbt-skin=\"frost\"] [id=\"Ribbon-file\"] { border-radius: 4px !important; margin-right: 4px !important; font-weight: 600; }\n\n/* Ribbon: a panel with a frost line along its top edge. */\nhtml[data-pbt-skin=\"frost\"] .ribbonContainer .bottomBarContainer { border-radius: 6px !important; box-shadow: inset 0 1px 0 rgba(136, 192, 208, .35); }\nhtml[data-pbt-skin=\"frost\"] .ribbonContainer [class*=\"groupContainer\"] + [class*=\"groupContainer\"] { border-left: 1px solid #434c5e !important; }\n\n/* Panes: a frost line at the top, a quiet edge. */\nhtml[data-pbt-skin=\"frost\"] :is(.sidePane, .outspacePane) { border-left: 1px solid #3b4252 !important; }\nhtml[data-pbt-skin=\"frost\"] :is(.pane-header, .outspacePane .paneHeader) { border-top: 2px solid #88c0d0; border-bottom: 1px solid #3b4252; }\nhtml[data-pbt-skin=\"frost\"] :is(.pane-header, .outspacePane .paneHeader) :is(h2, .pane-title) { font-weight: 600 !important; }\n\n/* Page tabs: the open one marked in frost blue. */\nhtml[data-pbt-skin=\"frost\"] .carouselScrollPane li.section, html[data-pbt-skin=\"frost\"] button.section.static.create .thumbnail-container { border-radius: 4px 4px 0 0 !important; }\nhtml[data-pbt-skin=\"frost\"] .carouselScrollPane li.section.selected { box-shadow: inset 0 -2px 0 #88c0d0 !important; }\nhtml[data-pbt-skin=\"frost\"] section.statusBar { border-top: 1px solid #3b4252; }\n\nhtml[data-pbt-skin=\"frost\"] .pbt-menu h4 { color: #88c0d0; opacity: 1; }\nhtml[data-pbt-skin=\"frost\"] .pbt-toast { border-left-color: #88c0d0; }\nhtml[data-pbt-skin=\"frost\"] ::-webkit-scrollbar { width: 10px; height: 10px; background: #272c36; }\nhtml[data-pbt-skin=\"frost\"] ::-webkit-scrollbar-thumb { background: #434c5e; border-radius: 5px; }\nhtml[data-pbt-skin=\"frost\"] ::-webkit-scrollbar-thumb:hover { background: #5e81ac; }\n\n/* Window controls: frosted faces inside Power BI's own button boxes; Close turns aurora red. */\nhtml[data-pbt-skin=\"frost\"] {\n  --pbt-wc-inset: 6px 8px; --pbt-wc-radius: 4px; --pbt-wc-fg: #eceff4; --pbt-wc-close-fg: #eceff4;\n  --pbt-wc-hover-face: #3b4252; --pbt-wc-close-hover-face: #bf616a; --pbt-wc-close-hover-fg: #eceff4;\n  --pbt-wc-hover-bg: transparent; --pbt-wc-close-hover-bg: transparent;\n}\n"}};
const PBT_BASE_CSS = "/* Skins & Tools for Power BI, by Reid Havens, Analytic Endeavors. Licensed CC BY-ND 4.0.\n   Fingerprint: AE-PBT-29A1ED34-2026 (also --ae-fp below; nothing reads it).\n   Shared by every skin, and by our own UI when no skin is on.\n   Our UI reads --pbt-* variables; each skin's palette.vars sets its own. */\n:root {\n  --ae-fp: \"AE-PBT-29A1ED34-2026\";\n  --pbt-ui-bg: #1f1f1f;\n  --pbt-ui-fg: #f0f0f0;\n  --pbt-ui-border: #444;\n  --pbt-ui-font: \"Segoe UI\", sans-serif;\n  --pbt-ui-radius: 6px;\n  --pbt-accent: #2aac94;\n  --pbt-ui-shadow: 0 8px 28px rgba(0, 0, 0, .45);\n}\n\n/* Chrome font: everything outside the report page, except icon fonts. Elements that draw an icon\n   glyph through a pseudo-element are named by what they are (arrow, chevron, caret) rather than\n   \"icon\" or \"glyph\"; the dropdown arrow in the Filters pane became a missing-glyph box when the\n   font rule reached it. */\nhtml[data-pbt-skin] :is(titlebar, ribbon-report-view, view-navigator, .sidePane, .outspacePane, status-bar, exploration-footer-modern, .cdk-overlay-container, .ms-Layer)\n  :is(span, div, button, label, h1, h2, h3, h4, input, li, a, p):not([class*=\"glyph\" i]):not([class*=\"icon\" i]):not([class*=\"arrow\" i]):not([class*=\"chevron\" i]):not([class*=\"caret\" i]):not(i):not(.zoomButton):not(.displayArea *) {\n  font-family: var(--pbt-font, inherit) !important;\n}\n\n/* ---- shared skin hooks ----\n   Each reads a --pbt-* variable a skin may set in palette.vars; unset ones fall back to\n   whatever the color remap produced, so a skin only names what it wants to change. */\nhtml[data-pbt-skin] div.titlebar {\n  background: var(--pbt-titlebar-bg, var(--titlebar-background)) !important;\n  color: var(--pbt-titlebar-fg, var(--titlebar-foreground)) !important;\n}\nhtml[data-pbt-skin] div.titlebar :is(.title, .timestamp, .dotSeparator, .accountNameLabel, .caretdownIcon, .titlebarControl) {\n  color: var(--pbt-titlebar-fg, inherit) !important;\n}\n/* Power BI's own tooltips (Fluent v9) carry a fixed charcoal or white face per mode. */\nhtml[data-pbt-skin] .fui-Tooltip__content {\n  background: var(--pbt-ui-bg) !important; color: var(--pbt-ui-fg) !important; border: 1px solid var(--pbt-ui-border) !important;\n  border-radius: var(--pbt-ui-radius) !important; box-shadow: var(--pbt-ui-shadow) !important; font-family: var(--pbt-ui-font) !important;\n}\nhtml[data-pbt-skin] .fui-Tooltip__content * { color: inherit !important; }\n/* Quick access icons (Save, Undo, Redo) are Office SVGs with fills fixed for a dark title bar.\n   Repaint them from the skin so they read on any title bar. */\nhtml[data-pbt-skin] titlebar-toolbar button { color: var(--pbt-icon, var(--pbt-titlebar-fg)) !important; }\nhtml[data-pbt-skin] titlebar-toolbar svg .Of)PBT"
LR"PBT(ficeIconColors_HighContrast,\nhtml[data-pbt-skin] titlebar-toolbar svg .OfficeIconColors_DynamicColor { fill: var(--pbt-icon, var(--pbt-titlebar-fg, currentColor)) !important; }\nhtml[data-pbt-skin] titlebar-toolbar svg .OfficeIconColors_m22 { fill: var(--pbt-icon-alt, var(--pbt-icon, currentColor)) !important; }\nhtml[data-pbt-skin] titlebar-toolbar svg .OfficeIconColors_m20 { fill: var(--pbt-icon-fill, transparent) !important; }\nhtml[data-pbt-skin] titlebar-toolbar button.is-disabled { opacity: .72; }\n/* Office icons everywhere else in the chrome: their fill class (m20) is transparent in Power BI's\n   dark mode and white in its light mode, so under a skin the same icon drew hollow in one mode\n   and filled in the other. Pinned here; light skins set --pbt-icon-fill to white. */\nhtml[data-pbt-skin] :is(.ribbonContainer, div.ribbonSearchOuter, .sidePane, .outspacePane, section.statusBar, exploration-footer-modern) svg .OfficeIconColors_m20 { fill: var(--pbt-icon-fill, transparent) !important; }\n/* Their line greys (m22 the main stroke, m23 the secondary, m21 the light one, m252 black) are\n   fixed per mode as well and sat too close to a dark skin's ground. They become shades of the\n   skin's text color (--pbt-icon-line for the main stroke), lighter on a dark skin, darker on a\n   light one; the \"d\" classes are the same strokes on disabled buttons. */\nhtml[data-pbt-skin] { --pbt-icon-line-default: color-mix(in srgb, var(--pbt-ribbon-fg, var(--colorNeutralForeground1)) 82%, transparent); }\nhtml[data-pbt-skin] :is(.ribbonContainer, div.ribbonSearchOuter, .sidePane, .outspacePane, section.statusBar, exploration-footer-modern) svg :is(.OfficeIconColors_m22, .OfficeIconColors_m252) { fill: var(--pbt-icon-line, var(--pbt-icon-line-default)) !important; }\nhtml[data-pbt-skin] :is(.ribbonContainer, div.ribbonSearchOuter, .sidePane, .outspacePane, section.statusBar, exploration-footer-modern) svg .OfficeIconColors_m23 { fill: color-mix(in srgb, var(--pbt-icon-line, var(--pbt-icon-line-default)) 72%, transparent) !important; }\nhtml[data-pbt-skin] :is(.ribbonContainer, div.ribbonSearchOuter, .sidePane, .outspacePane, section.statusBar, exploration-footer-modern) svg .OfficeIconColors_m21 { fill: color-mix(in srgb, var(--pbt-icon-line, var(--pbt-icon-line-default)) 40%, transparent) !important; }\nhtml[data-pbt-skin] :is(.ribbonContainer, .sidePane, .outspacePane) svg :is(.OfficeIconColors_d22, .OfficeIconColors_d252, .OfficeIconColors_d30, .OfficeIconColors_d31, .OfficeIconColors_d32) { fill: color-mix(in srgb, var(--pbt-icon-line, var(--pbt-icon-line-default)) 55%, transparent) !important; }\nhtml[data-pbt-skin] :is(.ribbonContainer, .sidePane, .outspacePane) svg :is(.OfficeIconColors_d23, .OfficeIconColors_d24, .OfficeIconColors_d210, .OfficeIconColors_d211) { fill: color-mix(in srgb, var(--pbt-icon-line, var(--pbt-icon-line-default)) 40%, transparent) !important; }\nhtml[data-pbt-skin] :is(.ribbonContainer, .sidePane, .outspacePane) svg :is(.OfficeIconColors_d20, .OfficeIconColors_d21, .OfficeIconColors_d212) { fill: var(--pbt-icon-fill, transparent) !important; }\nhtml[data-pbt-skin] titlebar-toolbar button:not(.is-disabled):hover { background: var(--pbt-titlebar-hover, rgba(255, 255, 255, .18)) !important; }\n/* Window controls keep Power BI's exact size and position. Windows decides what a click there\n   means from fixed button rectangles (Snap layouts depend on them), so a resized or shifted\n   button would maximize when you meant to minimize. A skin draws its own look as a face inside\n   the box instead, through these variables:\n     --pbt-wc-inset      where the face sits inside the 44 x 34 box (default 4px 4px, so no face\n                         or hover ever touches the window frame; keep at least 4px each way)\n     --pbt-wc-radius     its corners\n     --pbt-wc-face       its paint; --pbt-wc-close-face for Close\n     --pbt-wc-ring       its box-shadow (bevels, outlines, glow)\n     --pbt-wc-fg  )PBT"
LR"PBT(       glyph color; --pbt-wc-close-fg for Close\n     --pbt-wc-hover-face the face's paint under the pointer (default: the face, brightened; a\n                         grey tint when there is no face); --pbt-wc-close-hover-face for Close\n                         (default: the close face, else red)\n     --pbt-wc-hover-bg   whole-box hover tint, transparent by default\n     --pbt-wc-glyph      glyph size */\nhtml[data-pbt-skin] .windowControls button {\n  color: var(--pbt-wc-fg, var(--pbt-titlebar-fg, var(--titlebar-foreground))) !important;\n  position: relative; display: inline-grid !important; place-items: center; transition: background-color .12s;\n}\nhtml[data-pbt-skin] .windowControls button.closeButton { color: var(--pbt-wc-close-fg, var(--pbt-wc-fg, var(--pbt-titlebar-fg, var(--titlebar-foreground)))) !important; }\nhtml[data-pbt-skin] .windowControls button::before {\n  content: \"\"; position: absolute; inset: var(--pbt-wc-inset, 4px 4px); pointer-events: none;\n  border-radius: var(--pbt-wc-radius, 3px); background: var(--pbt-wc-face, transparent); box-shadow: var(--pbt-wc-ring, none);\n  transition: filter .12s, box-shadow .12s, background-color .12s;\n}\nhtml[data-pbt-skin] .windowControls button.closeButton::before { background: var(--pbt-wc-close-face, var(--pbt-wc-face, transparent)); }\nhtml[data-pbt-skin] .windowControls button:hover::before { background: var(--pbt-wc-hover-face, var(--pbt-wc-face, var(--pbt-titlebar-hover, rgba(127, 127, 127, .3)))); filter: brightness(1.18); }\nhtml[data-pbt-skin] .windowControls button.closeButton:hover::before { background: var(--pbt-wc-close-hover-face, var(--pbt-wc-close-face, var(--pbt-wc-face, #c42b1c))); }\n/* The button sizes itself around this 16 px box, so the box stays 16 px and only the drawing\n   inside it scales to --pbt-wc-glyph. */\nhtml[data-pbt-skin] .windowControls button pbi-office-icon { position: relative; z-index: 1; width: 16px !important; height: 16px !important; display: grid !important; place-items: center; margin: 0 !important; }\nhtml[data-pbt-skin] .windowControls button pbi-office-icon svg { display: block; width: var(--pbt-wc-glyph, 16px) !important; height: var(--pbt-wc-glyph, 16px) !important; }\nhtml[data-pbt-skin] .windowControls svg [class^=\"OfficeIconColors\"] { fill: currentColor !important; }\nhtml[data-pbt-skin] .windowControls button:hover { background-color: var(--pbt-wc-hover-bg, transparent) !important; }\nhtml[data-pbt-skin] .windowControls button.closeButton:hover { background-color: var(--pbt-wc-close-hover-bg, var(--pbt-wc-hover-bg, transparent)) !important; color: var(--pbt-wc-close-hover-fg, #fff) !important; }\n\n/* Account: avatar and hover follow the title bar instead of Fluent's white. */\nhtml[data-pbt-skin] titlebar account .header { border-radius: 4px; transition: background-color .12s; }\nhtml[data-pbt-skin] titlebar account .header:hover { background: var(--pbt-titlebar-hover, rgba(127, 127, 127, .2)) !important; }\nhtml[data-pbt-skin] titlebar account .imageAvatarIcon { background: var(--pbt-avatar-bg, transparent) !important; color: var(--pbt-icon, var(--pbt-titlebar-fg, currentColor)) !important; box-shadow: 0 0 0 1px currentColor; border-radius: 50%; }\nhtml[data-pbt-skin] titlebar account .imageAvatarIcon svg { fill: currentColor; color: inherit; }\n\n/* One hover language for the chrome; skins tune it with --pbt-hover-bg and --pbt-hover-ring. */\nhtml[data-pbt-skin] {\n  --pbt-hover-bg-default: color-mix(in srgb, var(--pbt-accent) 16%, transparent);\n  --pbt-hover-ring-default: color-mix(in srgb, var(--pbt-accent) 70%, transparent);\n}\nhtml[data-pbt-skin] .ribbonContainer .bottomBarContainer button:not(:disabled):not(.is-disabled):not(.pbt-rbtn):hover {\n  background-color: var(--pbt-hover-bg, var(--pbt-hover-bg-default)) !important;\n  box-shadow: inset 0 0 0 1px var(--pbt-hover-ring, var(--pbt-hover-ring-default));\n  border-radius: var(--pbt-hover-radius, 4px) !important;\n}\nhtml[data-pbt-)PBT"
LR"PBT(skin] .ribbonContainer .bottomBarContainer button { transition: background-color .12s, box-shadow .12s; }\n/* Skin fonts run wider than Segoe UI; a little side room keeps neighbouring button labels apart. */\nhtml[data-pbt-skin] .ribbonContainer .bottomBarContainer [role=\"group\"] button.ms-ButtonNext { padding-inline: 4px !important; margin-inline: 1px !important; }\nhtml[data-pbt-skin] :is(.carouselScrollPane li.section, .pane-header button, .outspacePane .paneHeader button):hover { filter: brightness(1.08); }\nhtml[data-pbt-skin] .carouselScrollPane li.section:not(.selected):hover { box-shadow: inset 0 -2px 0 var(--pbt-hover-ring, var(--pbt-hover-ring-default)) !important; }\n\n/* ribbon tab row and panel */\nhtml[data-pbt-skin] .ribbonTopBarContainer { background: var(--pbt-tabs-bg, var(--pbt-titlebar-bg, transparent)) !important; }\nhtml[data-pbt-skin] .ribbonTopBarContainer > div,\nhtml[data-pbt-skin] .ribbonTopBarContainer > div > div { background: transparent !important; }\nhtml[data-pbt-skin] #tablist button[role=\"tab\"] {\n  color: var(--pbt-tab-fg, var(--colorNeutralForeground1)) !important; border-radius: var(--pbt-tab-radius, 0) var(--pbt-tab-radius, 0) 0 0;\n}\nhtml[data-pbt-skin] #tablist button[role=\"tab\"] * { color: inherit !important; }\nhtml[data-pbt-skin] #tablist button[role=\"tab\"]:hover { background: var(--pbt-tab-hover-bg, rgba(127, 127, 127, .18)) !important; }\nhtml[data-pbt-skin] #tablist button[role=\"tab\"][aria-selected=\"true\"] {\n  background: var(--pbt-tab-active-bg, transparent) !important; color: var(--pbt-tab-active-fg, var(--colorNeutralForeground1)) !important;\n}\n/* Power BI's sliding underline is placed from the tabs' measured widths, so a skin's font leaves\n   it under the wrong tab (and on top of the skin's own marker). Every skin marks the open tab on\n   the tab itself, so the underline goes. */\nhtml[data-pbt-skin] #selectedPivotIndicator { display: none !important; }\nhtml[data-pbt-skin] [id=\"Ribbon-file\"] {\n  background: var(--pbt-file-bg, transparent) !important; color: var(--pbt-file-fg, var(--pbt-tab-fg, inherit)) !important;\n}\nhtml[data-pbt-skin] [id=\"Ribbon-file\"] * { color: inherit !important; }\nhtml[data-pbt-skin] .ribbonContainer div:has(> .bottomBarContainer) { background: var(--pbt-tabs-bg, var(--pbt-titlebar-bg, transparent)) !important; }\nhtml[data-pbt-skin] .ribbonContainer .bottomBarContainer {\n  background: var(--pbt-ribbon-bg, var(--colorNeutralBackground1)) !important;\n  border: var(--pbt-ribbon-border, 0) !important;\n}\nhtml[data-pbt-skin] .ribbonContainer [role=\"group\"] label[class*=\"groupLabel\"] { color: var(--pbt-group-fg, inherit) !important; }\n/* The title bar search box text and magnifier; skins with a light search field set --pbt-search-fg. */\nhtml[data-pbt-skin] div.ribbonSearchOuter :is(input, svg, span, i) { color: var(--pbt-search-fg, inherit) !important; }\nhtml[data-pbt-skin] div.ribbonSearchOuter input::placeholder { color: var(--pbt-search-fg, inherit) !important; opacity: .8; }\n/* Ribbon buttons sit flat on the panel (textured panels would otherwise show white boxes);\n   pressed toggles such as Filters keep their state color. */\nhtml[data-pbt-skin] .ribbonContainer .bottomBarContainer button:not([aria-pressed=\"true\"]):not([aria-checked=\"true\"]):not(.is-checked):not(:hover):not(.pbt-rbtn) { background-color: transparent !important; background-image: none !important; }\n/* Fluent v8 parts of the ribbon (the External tools buttons, split-button chevrons) bake in text\n   colors for Power BI's own light or dark mode instead of reading the theme variables, so they\n   flipped with the mode underneath a skin. Skins set them directly, so a skin looks the same in\n   either mode. Disabled ones are the skin text at half strength: Power BI's own disabled grey\n   differs by mode and lands at different points on a skin's palette. */\nhtml[data-pbt-skin] .ribbonContainer .bottomBarContainer :is(.ms-Button-label, .m)PBT"
LR"PBT(s-Button-textContainer, .ms-Button-description, .ms-Button-menuIcon, .ms-Button-icon) {\n  color: var(--pbt-ribbon-fg, var(--colorNeutralForeground1)) !important;\n}\n/* Fluent v9 buttons (Paste, Cut, New visual calculation, ...) color their text on the button\n   itself, so the disabled color goes on the button and its text inherits it. */\nhtml[data-pbt-skin] .ribbonContainer .bottomBarContainer :is(button:disabled, button[aria-disabled=\"true\"], .is-disabled),\nhtml[data-pbt-skin] .ribbonContainer .bottomBarContainer :is(button:disabled, button[aria-disabled=\"true\"], .is-disabled) :is(.ms-Button-label, .ms-Button-textContainer, .ms-Button-description, .ms-Button-menuIcon, span:not(svg *)) {\n  color: var(--pbt-ribbon-fg-disabled, color-mix(in srgb, var(--pbt-ribbon-fg, var(--colorNeutralForeground1)) 55%, transparent)) !important;\n}\n/* Fluent v8 dropdown menus (ribbon menus, context menus) have the same baked-in colors. */\nhtml[data-pbt-skin] :is(.ms-ContextualMenu, .ms-Callout-main) { background-color: var(--colorNeutralBackground1) !important; color: var(--colorNeutralForeground1) !important; }\nhtml[data-pbt-skin] .ms-ContextualMenu-link { background-color: transparent !important; color: var(--colorNeutralForeground1) !important; }\nhtml[data-pbt-skin] .ms-ContextualMenu-link:hover:not(.is-disabled) { background-color: var(--colorNeutralBackground1Hover, var(--pbt-hover-bg-default)) !important; }\nhtml[data-pbt-skin] .ms-ContextualMenu-link :is(.ms-ContextualMenu-itemText, .ms-ContextualMenu-icon, .ms-ContextualMenu-submenuIcon, .ms-ContextualMenu-secondaryText) { color: inherit !important; }\nhtml[data-pbt-skin] .ms-ContextualMenu-link.is-disabled { color: color-mix(in srgb, var(--colorNeutralForeground1) 55%, transparent) !important; }\nhtml[data-pbt-skin] .ms-ContextualMenu-divider { background-color: var(--colorNeutralStroke2) !important; }\n\n/* The collapse chevron at the panel's bottom right sits in a container with a fixed dark fill. */\nhtml[data-pbt-skin] [class*=\"chevronContainer\"] { background: transparent !important; }\nhtml[data-pbt-skin] #RibbonModeToggle { background: transparent !important; color: var(--pbt-group-fg, var(--colorNeutralForeground1)) !important; border-radius: var(--pbt-hover-radius, 4px) !important; }\nhtml[data-pbt-skin] #RibbonModeToggle svg [class^=\"OfficeIconColors\"] { fill: currentColor !important; }\n\n/* view switcher (Report, Table, Model, DAX, TMDL) */\nhtml[data-pbt-skin] .viewNavigatorContainer { background: var(--pbt-nav-bg, var(--colorPaneBackground)) !important; }\n/* The icons are strokes in currentColor; the buttons' own (remapped) fill would hide the rail behind them. */\nhtml[data-pbt-skin] view-navigator button { color: var(--pbt-icon-nav, var(--pbt-icon, inherit)) !important; background: transparent !important; }\nhtml[data-pbt-skin] view-navigator button:hover { background: var(--pbt-nav-hover-bg, rgba(127, 127, 127, .2)) !important; }\nhtml[data-pbt-skin] view-navigator button[aria-selected=\"true\"] {\n  color: var(--pbt-icon-active, var(--pbt-accent)) !important; background: var(--pbt-nav-active-bg, transparent) !important;\n  box-shadow: inset 3px 0 0 var(--pbt-accent);\n}\n\n/* page tabs */\nhtml[data-pbt-skin] ul.pane.sections, html[data-pbt-skin] .explorationNavigationContent { background: var(--pbt-pagetabs-bg, var(--pane-background-color)) !important; }\nhtml[data-pbt-skin] .carouselScrollPane li.section { background: var(--pbt-pagetab-bg, transparent) !important; border-radius: var(--pbt-tab-radius, 0) var(--pbt-tab-radius, 0) 0 0; }\nhtml[data-pbt-skin] .carouselScrollPane li.section * { color: var(--pbt-pagetab-fg, inherit) !important; }\nhtml[data-pbt-skin] .carouselScrollPane li.section.selected { background: var(--pbt-pagetab-active-bg, var(--section-background-selected-color)) !important; }\nhtml[data-pbt-skin] .carouselScrollPane li.section.selected * { color: var(--pbt-pagetab-active-fg, inherit) !important; }\n/* New page (+) f)PBT"
LR"PBT(ollows the page tabs instead of keeping its accent-filled square. */\nhtml[data-pbt-skin] button.section.static.create { background: transparent !important; color: var(--pbt-pagetab-fg, var(--pbt-tab-fg, var(--colorNeutralForeground1))) !important; }\nhtml[data-pbt-skin] button.section.static.create .thumbnail-container {\n  background: var(--pbt-pagetab-bg, transparent) !important; border-radius: var(--pbt-tab-radius, 0) var(--pbt-tab-radius, 0) 0 0;\n  box-shadow: inset 0 0 0 1px color-mix(in srgb, currentColor 30%, transparent);\n}\nhtml[data-pbt-skin] button.section.static.create:hover .thumbnail-container { background: var(--pbt-hover-bg, var(--pbt-hover-bg-default)) !important; }\n/* The + symbol paints with these variables, not currentColor. */\nhtml[data-pbt-skin] button.section.static.create {\n  --add-new-page-icon-color: currentColor; --add-new-page-icon-color-hover: currentColor; --add-new-page-icon-color-pressed: currentColor;\n}\n\n/* side panes and status bar */\nhtml[data-pbt-skin] .pane-header, html[data-pbt-skin] .outspacePane .paneHeader {\n  background: var(--pbt-panehead-bg, transparent) !important; color: var(--pbt-panehead-fg, inherit) !important;\n}\n/* The Filters pane body keeps the report's colors (often white), so its header gets a solid chrome\n   color under the skin's header paint: the same color the side panes show behind theirs. */\nhtml[data-pbt-skin] .outspacePane .paneHeader { background-color: var(--pbt-panehead-solid, var(--exploreUIPane-colorBackground)) !important; }\n/* Collapsed, the pane is a thin strip with the title turned on its side and running below the\n   header block; the strip shows no report content, so it takes the header paint end to end. */\nhtml[data-pbt-skin] .outspacePane.outspacePaneCollapsed {\n  background: var(--pbt-panehead-bg, transparent) !important;\n  background-color: var(--pbt-panehead-solid, var(--exploreUIPane-colorBackground)) !important;\n}\nhtml[data-pbt-skin] .outspacePane.outspacePaneCollapsed .paneHeader { background: transparent !important; }\n/* Every part of a pane header takes the header color, including the Filters title and glyph,\n   which otherwise keep the report's inline filter-pane text color (dark on a skinned header). */\nhtml[data-pbt-skin] :is(.pane-header, .outspacePane .paneHeader) :is(h2, button, span, div, .glyphicon) { color: var(--pbt-panehead-fg, inherit) !important; }\n/* Pane titles set their own letter case; a skin sets --pbt-panehead-case once and every title follows. */\nhtml[data-pbt-skin] :is(.pane-header, .outspacePane .paneHeader) :is(h2, .pane-title, .sidePaneTitle) { text-transform: var(--pbt-panehead-case, none) !important; letter-spacing: var(--pbt-panehead-tracking, normal) !important; }\n\n/* The Filters pane body: the report sets its colors (white by default), which leaves a white\n   block beside dark skinned panes. Skins paint it like the other panes; the menu's \"Keep the\n   report's Filters pane colors\" (html.pbt-report-filters) shows the report's own formatting. */\nhtml[data-pbt-skin]:not(.pbt-report-filters) .outspacePane {\n  background-color: var(--exploreUIPane-colorBackground) !important; color: var(--colorNeutralForeground1) !important;\n  border-color: var(--pbt-line-soft) !important;\n}\nhtml[data-pbt-skin]:not(.pbt-report-filters) .outspacePane .paneContents :is(div, span, section, label, p, h3, h4, button, a):not([role=\"checkbox\"]):not([class*=\"checkbox\" i]):not([class*=\"slider\" i]):not([class*=\"toggle\" i]) {\n  background-color: transparent !important; color: inherit !important;\n}\n/* Borders come from the text color, not a mid-grey token: a mid-grey lands on a different spot of\n   the skin's ramp under Power BI's light and dark modes, while the text end is the same in both. */\nhtml[data-pbt-skin] { --pbt-line: color-mix(in srgb, var(--colorNeutralForeground1) 32%, transparent); --pbt-line-soft: color-mix(in srgb, var(--colorNeutralForeground1) 18%, transparent); }\nhtml[)PBT"
LR"PBT(data-pbt-skin]:not(.pbt-report-filters) .outspacePane .paneContents :is([class*=\"card\" i], [class*=\"drop\" i], [class*=\"well\" i]) { border-color: var(--pbt-line) !important; }\nhtml[data-pbt-skin]:not(.pbt-report-filters) .outspacePane input { background: var(--colorNeutralBackground1) !important; color: var(--colorNeutralForeground1) !important; border-color: var(--pbt-line) !important; }\n/* The filter card's controls are Angular Material parts, not plain elements, so they are named:\n   the Filter type and condition dropdowns, their open list (in the overlay layer), and the\n   \"Require single selection\" check box. */\nhtml[data-pbt-skin]:not(.pbt-report-filters) .outspacePane mat-select {\n  background-color: var(--colorNeutralBackground1) !important; color: var(--colorNeutralForeground1) !important; border-color: var(--pbt-line) !important;\n}\nhtml[data-pbt-skin]:not(.pbt-report-filters) .outspacePane mat-select :is(.mat-mdc-select-value, .mat-mdc-select-value-text, .mat-mdc-select-placeholder, .mat-mdc-select-arrow) { color: inherit !important; }\nhtml[data-pbt-skin]:not(.pbt-report-filters) .outspacePane mat-select .mat-mdc-select-placeholder { opacity: .7; }\nhtml[data-pbt-skin]:not(.pbt-report-filters) .cdk-overlay-container .mat-mdc-select-panel {\n  background-color: var(--colorNeutralBackground1) !important; color: var(--colorNeutralForeground1) !important; border: 1px solid var(--pbt-line); box-shadow: var(--pbt-ui-shadow) !important;\n}\nhtml[data-pbt-skin]:not(.pbt-report-filters) .cdk-overlay-container .mat-mdc-option { background-color: transparent !important; color: var(--colorNeutralForeground1) !important; font-family: var(--pbt-font, inherit) !important; }\nhtml[data-pbt-skin]:not(.pbt-report-filters) .cdk-overlay-container .mat-mdc-option :is(.mdc-list-item__primary-text, span) { color: inherit !important; }\nhtml[data-pbt-skin]:not(.pbt-report-filters) .cdk-overlay-container .mat-mdc-option:is(:hover, .mat-mdc-option-active, .mdc-list-item--selected) { background-color: color-mix(in srgb, var(--pbt-accent) 22%, transparent) !important; }\nhtml[data-pbt-skin]:not(.pbt-report-filters) .outspacePane .mdc-checkbox__background { border-color: var(--colorNeutralForeground1) !important; background-color: transparent !important; }\nhtml[data-pbt-skin]:not(.pbt-report-filters) .outspacePane .mdc-checkbox__native-control:checked ~ .mdc-checkbox__background { background-color: var(--pbt-accent) !important; border-color: var(--pbt-accent) !important; }\nhtml[data-pbt-skin]:not(.pbt-report-filters) .outspacePane .mdc-checkbox__checkmark-path { stroke: var(--colorNeutralBackground1) !important; }\nhtml[data-pbt-skin]:not(.pbt-report-filters) .outspacePane .mdc-checkbox__ripple { display: none; }\n/* The search field inside a filter card sits on its own row with the magnifier; a filled input\n   there showed as a lighter block behind the words. */\nhtml[data-pbt-skin]:not(.pbt-report-filters) .outspacePane .filterContent input.searchInput { background: transparent !important; }\n/* The search box sits beside the contents with its own inline report colors. */\nhtml[data-pbt-skin]:not(.pbt-report-filters) .outspacePane search-box-modern {\n  background-color: var(--colorNeutralBackground1) !important; color: var(--colorNeutralForeground1) !important;\n}\nhtml[data-pbt-skin]:not(.pbt-report-filters) .outspacePane search-box-modern * { color: inherit !important; background-color: transparent !important; }\nhtml[data-pbt-skin]:not(.pbt-report-filters) .outspacePane .searchFiltersLineBreak { border-color: var(--colorNeutralStroke2) !important; background: transparent !important; }\nhtml[data-pbt-skin] section.statusBar { background: var(--pbt-status-bg, var(--colorBackground)) !important; color: var(--pbt-status-fg, var(--colorNeutralForeground2)) !important; }\nhtml[data-pbt-skin] section.statusBar :is(div, span, button):not(.pbt-chip, .pbt-chip *) { color: inherit !important; }\n/* Share (top right) is )PBT"
LR"PBT(a Fluent v8 button with fixed dark colors. */\nhtml[data-pbt-skin] .farPeripheralControls button {\n  background: var(--pbt-share-bg, transparent) !important; color: var(--pbt-share-fg, var(--pbt-tab-fg, var(--colorNeutralForeground1))) !important;\n  border: 1px solid color-mix(in srgb, currentColor 45%, transparent) !important; border-radius: var(--pbt-hover-radius, 4px) !important;\n  width: auto !important; padding: 0 10px !important;\n}\nhtml[data-pbt-skin] .farPeripheralControls button * { color: inherit !important; }\nhtml[data-pbt-skin] .farPeripheralControls button .ms-Button-flexContainer { gap: 6px; }\nhtml[data-pbt-skin] .farPeripheralControls svg :is(path, [class^=\"OfficeIconColors\"]) { fill: currentColor !important; }\n/* Disabled until signed in: dim the whole button evenly instead of fading only the icon. */\nhtml[data-pbt-skin] .farPeripheralControls button.is-disabled { opacity: .85; }\nhtml[data-pbt-skin] .farPeripheralControls button.is-disabled * { opacity: 1 !important; }\nhtml[data-pbt-skin] .farPeripheralControls button:not(.is-disabled):hover {\n  background: var(--pbt-hover-bg, color-mix(in srgb, var(--pbt-accent) 28%, transparent)) !important;\n  box-shadow: inset 0 0 0 1px var(--pbt-hover-ring, var(--pbt-hover-ring-default));\n}\n\n/* canvas gutter around the report page */\nhtml[data-pbt-skin] .explorationContainer .exploration { background: var(--pbt-gutter, var(--white-color)) !important; }\n\n/* ---- our UI ---- */\n.pbt-toasts {\n  position: fixed; right: 24px; bottom: 44px; z-index: 2147483000;\n  display: flex; flex-direction: column; gap: 8px; align-items: flex-end; pointer-events: none;\n}\n.pbt-toast {\n  max-width: 380px; padding: 10px 14px; border-radius: var(--pbt-ui-radius);\n  background: var(--pbt-ui-bg); color: var(--pbt-ui-fg); border: 1px solid var(--pbt-ui-border);\n  border-left: 4px solid var(--pbt-accent);\n  font: 13px/1.4 var(--pbt-ui-font); box-shadow: var(--pbt-ui-shadow);\n  opacity: 0; transform: translateY(8px); transition: opacity .25s, transform .25s;\n  display: flex; flex-direction: column; gap: 2px;\n}\n.pbt-toast.pbt-in { opacity: 1; transform: none; }\n.pbt-toast b { font-weight: 700; }\n/* Tone wins over a skin's own toast edge color, so an error never looks like good news. */\n.pbt-toast.pbt-bad, html[data-pbt-skin] .pbt-toast.pbt-bad { border-left-color: #e74c3c !important; }\n.pbt-toast.pbt-good, html[data-pbt-skin] .pbt-toast.pbt-good { border-left-color: #2ecc71 !important; }\n.pbt-toast.pbt-warn, html[data-pbt-skin] .pbt-toast.pbt-warn { border-left-color: #f39c12 !important; }\n\n/* status bar chip and menu */\n.pbt-chip {\n  display: inline-flex; align-items: center; gap: 6px; height: 18px; margin: 0 6px; padding: 0 8px;\n  border: 1px solid transparent; border-radius: 3px; background: transparent; color: inherit;\n  font: inherit; font-size: 12px; cursor: pointer; white-space: nowrap;\n}\n/* Hover and keyboard focus: a tint plus the outline, at a selector strong enough that no skin's\n   status bar rules flatten it. */\n.pbt-chip:is(:hover, :focus-visible), html[data-pbt-skin] section.statusBar .pbt-chip:is(:hover, :focus-visible) {\n  border-color: currentColor !important; background: color-mix(in srgb, currentColor 16%, transparent) !important; outline: none;\n}\n/* The separator belongs to the count, so a hidden or empty count leaves no stray dot. */\n.pbt-chip .pbt-count:not(:empty)::before { content: \"\\00b7\"; margin-right: 6px; }\n.pbt-chip .pbt-dot { width: 8px; height: 8px; border-radius: 50%; background: var(--pbt-accent); }\n/* The caret: an up chevron drawn from two borders, as tall as the chip's text (the menu opens upward). */\n.pbt-chip .pbt-caret { flex: none; width: 7px; height: 7px; margin: 4px 2px 0; box-sizing: border-box; border: solid currentColor; border-width: 1.6px 1.6px 0 0; transform: rotate(-45deg); opacity: .85; }\n.pbt-count.pbt-warn { color: #f39c12; font-weight: 600; }\n/* The one-time hint beside the chip )PBT"
LR"PBT((page/20-menu.js, hint). */\n.pbt-hint {\n  display: inline-flex; align-items: center; gap: 4px; height: 18px; margin: 0 4px; padding: 0 2px 0 8px; box-sizing: border-box;\n  border: 1px dashed currentColor; border-radius: 9px; font-size: 11px; white-space: nowrap; opacity: .9;\n}\n.pbt-hint .pbt-hint-x { border: 0; background: transparent; color: inherit; font: inherit; font-size: 13px; line-height: 1; padding: 0 5px; cursor: pointer; border-radius: 7px; }\n.pbt-hint .pbt-hint-x:hover, .pbt-hint .pbt-hint-x:focus-visible { background: color-mix(in srgb, currentColor 18%, transparent); outline: none; }\n\n/* The menu: skins as tiles, then tools and extras as switches with a one-line description. */\n.pbt-menu {\n  position: fixed; z-index: 2147483001; width: 310px; max-height: 88vh; overflow: auto; padding: 6px; box-sizing: border-box;\n  background: var(--pbt-ui-bg); color: var(--pbt-ui-fg); border: 1px solid var(--pbt-ui-border);\n  border-radius: var(--pbt-ui-radius); box-shadow: var(--pbt-ui-shadow); font: 13px/1.3 var(--pbt-ui-font);\n}\n.pbt-menu h4 { display: flex; align-items: baseline; gap: 8px; margin: 6px 8px 2px; font-size: 11px; text-transform: uppercase; letter-spacing: .06em; font-weight: 600; }\n.pbt-menu h4 .pbt-key { text-transform: none; letter-spacing: 0; }\n.pbt-menu h4 button.pbt-sect { display: inline-flex; width: auto; align-items: center; gap: 6px; padding: 2px 4px; margin: 0 0 0 -4px; opacity: .75; font: inherit; text-transform: inherit; letter-spacing: inherit; }\n.pbt-menu h4 button.pbt-sect:hover, .pbt-menu h4 button.pbt-sect:focus-visible { opacity: 1; }\n/* A long skin name in the Skins heading ends in an ellipsis (the full name is in its tooltip); the key hint keeps its words. */\n.pbt-menu h4 button.pbt-sect { min-width: 0; }\n.pbt-menu h4 button.pbt-sect > span:last-child { min-width: 0; overflow: hidden; text-overflow: ellipsis; white-space: nowrap; }\n.pbt-menu h4 .pbt-key { flex: none; white-space: nowrap; }\n.pbt-menu .pbt-chev { display: inline-block; width: 10px; font-size: 10px; transition: transform .12s; }\n.pbt-menu .pbt-section.pbt-folded .pbt-chev { transform: rotate(-90deg); }\n.pbt-menu .pbt-section.pbt-folded .pbt-body { display: none; }\n.pbt-menu .pbt-blurb { margin: 0 8px 6px; font-size: 11px; line-height: 1.35; opacity: .8; }\n/* Compact (short windows): descriptions live in the tooltips only. */\n/* A drag handle at the bottom right; the menu is anchored by its bottom edge, so it grows upward. */\n.pbt-menu { resize: vertical; min-height: 140px; }\n.pbt-menu hr { border: 0; border-top: 1px solid var(--pbt-ui-border); margin: 6px 4px; }\n.pbt-menu button {\n  display: flex; width: 100%; align-items: center; gap: 8px; padding: 5px 8px; border: 0; border-radius: 4px; box-sizing: border-box;\n  background: transparent; color: inherit; font: inherit; text-align: left; cursor: pointer;\n}\n.pbt-menu button:hover, .pbt-menu button:focus-visible { background: color-mix(in srgb, var(--pbt-accent) 25%, transparent); outline: none; }\n.pbt-menu .pbt-key { margin-left: auto; opacity: .75; font-size: 11px; white-space: nowrap; }\n.pbt-skins { display: grid; grid-template-columns: repeat(3, 1fr); gap: 3px; padding: 0 4px; }\n.pbt-skins button { flex-direction: column; align-items: flex-start; gap: 3px; padding: 5px 7px; border: 1px solid transparent; font-size: 12px; }\n.pbt-skins button[aria-checked=\"true\"] { border-color: var(--pbt-accent); background: color-mix(in srgb, var(--pbt-accent) 16%, transparent); }\n.pbt-skins .pbt-sw { width: 100%; height: 10px; border-radius: 3px; border: 1px solid rgba(128, 128, 128, .5); box-sizing: border-box; }\n.pbt-skins .pbt-name { display: flex; align-items: center; gap: 4px; width: 100%; }\n.pbt-skins .pbt-name::after { content: \"\"; margin-left: auto; }\n.pbt-skins button[aria-checked=\"true\"] .pbt-name::after { content: \"\\2713\"; color: var(--pbt-accent); font-weight: 700; }\n.pbt-menu .pbt-item { display: grid; grid-template-c)PBT"
LR"PBT(olumns: 30px 1fr auto; grid-template-areas: \"sw name key\" \". desc desc\"; column-gap: 8px; row-gap: 1px; align-items: center; }\n.pbt-menu .pbt-item .pbt-switch { grid-area: sw; }\n.pbt-menu .pbt-item .pbt-name { grid-area: name; }\n.pbt-menu .pbt-item .pbt-key, .pbt-menu .pbt-item .pbt-pills { grid-area: key; margin-left: 0; }\n.pbt-menu .pbt-pills { display: inline-flex; gap: 4px; }\n.pbt-menu .pbt-item .pbt-desc { grid-area: desc; font-size: 11px; line-height: 1.3; opacity: .72; }\n.pbt-switch { position: relative; width: 28px; height: 15px; border-radius: 8px; background: color-mix(in srgb, var(--pbt-ui-fg) 25%, transparent); box-shadow: inset 0 0 0 1px color-mix(in srgb, var(--pbt-ui-fg) 40%, transparent); transition: background .12s; }\n.pbt-switch::after { content: \"\"; position: absolute; top: 2px; left: 2px; width: 11px; height: 11px; border-radius: 50%; background: var(--pbt-ui-fg); transition: left .12s; }\n[aria-checked=\"true\"] > .pbt-switch { background: var(--pbt-accent); box-shadow: none; }\n[aria-checked=\"true\"] > .pbt-switch::after { left: 15px; background: var(--pbt-ui-bg); }\n.pbt-menu .pbt-try { padding: 1px 9px; border: 1px solid color-mix(in srgb, var(--pbt-ui-fg) 45%, transparent); border-radius: 10px; font-size: 11px; cursor: pointer; }\n.pbt-menu .pbt-try:hover, .pbt-menu .pbt-try:focus-visible { border-color: var(--pbt-accent); background: color-mix(in srgb, var(--pbt-accent) 25%, transparent); outline: none; }\n.pbt-menu .pbt-try .pbt-pill-icon { width: 11px; height: 11px; margin-right: 4px; vertical-align: -1px; fill: none; stroke: currentColor; stroke-width: 1.5; stroke-linejoin: round; stroke-linecap: round; }\n.pbt-menu .pbt-try[aria-pressed=\"true\"] { border-color: var(--pbt-accent); background: var(--pbt-accent); color: var(--pbt-ui-bg); font-weight: 600; }\n.pbt-menu .pbt-help-item .pbt-q { width: 30px; text-align: center; font-weight: 700; }\n\n/* tooltips: the shared bubble pbt.tip draws, title line then explanation lines */\n.pbt-tip {\n  position: fixed; z-index: 2147483004; max-width: 300px; padding: 6px 9px; pointer-events: none; box-sizing: border-box;\n  background: var(--pbt-ui-bg); color: var(--pbt-ui-fg); border: 1px solid var(--pbt-ui-border);\n  border-radius: var(--pbt-ui-radius); box-shadow: var(--pbt-ui-shadow); font: 12px/1.35 var(--pbt-ui-font);\n}\n.pbt-tip b { display: block; font-weight: 700; }\n.pbt-tip span { display: block; }\n.pbt-tip::after {\n  content: \"\"; position: absolute; left: var(--pbt-tip-arrow, 18px); bottom: -6px; width: 10px; height: 10px; transform: translateX(-50%) rotate(45deg);\n  background: var(--pbt-ui-bg); border-right: 1px solid var(--pbt-ui-border); border-bottom: 1px solid var(--pbt-ui-border);\n}\n.pbt-tip.pbt-tip-below::after { bottom: auto; top: -6px; border: 0; border-left: 1px solid var(--pbt-ui-border); border-top: 1px solid var(--pbt-ui-border); }\n\n/* help page */\n.pbt-help-backdrop { position: fixed; inset: 0; z-index: 2147483001; display: grid; place-items: center; background: rgba(0, 0, 0, .35); }\n.pbt-help {\n  width: min(680px, 92vw); max-height: 88vh; overflow: auto; outline: none; box-sizing: border-box;\n  background: var(--pbt-ui-bg); color: var(--pbt-ui-fg); border: 1px solid var(--pbt-ui-border); border-radius: var(--pbt-ui-radius); box-shadow: var(--pbt-ui-shadow);\n  font: 13px/1.45 var(--pbt-ui-font); text-align: left;\n}\n.pbt-help header { position: sticky; top: 0; z-index: 1; display: flex; align-items: center; gap: 10px; padding: 10px 14px; background: var(--pbt-ui-bg); border-bottom: 1px solid var(--pbt-ui-border); }\n.pbt-help header h2 { margin: 0; font-size: 15px; font-weight: 700; }\n.pbt-help header nav { display: flex; flex-wrap: wrap; gap: 2px; margin-left: 8px; font-size: 12px; }\n.pbt-help header .pbt-tab { border: 0; border-bottom: 2px solid transparent; background: transparent; color: inherit; font: inherit; font-size: 12px; padding: 3px 7px; opacity: .75; cursor: pointer; }\n.pbt-h)PBT"
LR"PBT(elp header .pbt-tab:hover, .pbt-help header .pbt-tab:focus-visible { opacity: 1; outline: none; background: color-mix(in srgb, var(--pbt-accent) 18%, transparent); }\n.pbt-help header .pbt-tab[aria-selected=\"true\"] { opacity: 1; border-bottom-color: var(--pbt-accent); font-weight: 700; }\n.pbt-help .pbt-tabpanel[hidden] { display: none; }\n/* A drag handle at the bottom right; the page is centered, so it grows from the middle. */\n.pbt-help { resize: both; min-width: 360px; min-height: 160px; }\n.pbt-help header .pbt-x { margin-left: auto; border: 0; background: transparent; color: inherit; font: 18px/1 var(--pbt-ui-font); cursor: pointer; padding: 0 4px; }\n.pbt-help section { padding: 10px 14px; border-bottom: 1px solid color-mix(in srgb, var(--pbt-ui-border) 50%, transparent); }\n.pbt-help section:last-child { border-bottom: 0; }\n.pbt-help h3 { display: flex; align-items: baseline; gap: 8px; margin: 0 0 4px; font-size: 13px; font-weight: 700; }\n.pbt-help h3 .pbt-key { margin-left: auto; font-size: 11px; opacity: .75; font-weight: 400; white-space: nowrap; }\n.pbt-help p { margin: 0 0 6px; opacity: .92; }\n.pbt-help ul { margin: 0 0 6px; padding-left: 18px; }\n.pbt-help li { margin: 2px 0; }\n.pbt-help li .pbt-state { margin-left: 6px; font-size: 11px; opacity: .6; }\n.pbt-help table { border-collapse: collapse; font-size: 12px; }\n.pbt-help td { padding: 2px 10px 2px 0; vertical-align: top; }\n.pbt-help td:first-child { white-space: nowrap; opacity: .8; }\n.pbt-help .pbt-legend { display: grid; grid-template-columns: 24px 1fr; gap: 4px 10px; align-items: center; margin: 4px 0 6px; }\n.pbt-help .pbt-lg-box { width: 22px; height: 14px; outline: 1px solid rgba(0, 153, 255, .9); background: rgba(0, 153, 255, .08); }\n.pbt-help .pbt-lg-bad { width: 22px; height: 14px; outline: 2px solid #e5383b; background: rgba(229, 56, 59, .1); }\n.pbt-help .pbt-lg-guide { width: 22px; height: 0; border-top: 1px dashed #e5383b; }\n.pbt-help .pbt-lg-spill { width: 22px; height: 14px; outline: 2px solid #e5383b; background: rgba(229, 56, 59, .1); clip-path: inset(0 0 0 40%); margin-left: -8px; }\n.pbt-help .pbt-help-skins { display: grid; grid-template-columns: repeat(2, 1fr); gap: 3px 14px; margin: 2px 0 6px; font-size: 12px; }\n.pbt-help .pbt-help-skin { display: flex; align-items: center; gap: 6px; }\n.pbt-help .pbt-help-skin span:not(.pbt-sw) { opacity: .7; overflow: hidden; text-overflow: ellipsis; }\n.pbt-help .pbt-sw { flex: none; width: 24px; height: 11px; border-radius: 3px; border: 1px solid rgba(128, 128, 128, .5); display: inline-block; vertical-align: middle; }\n.pbt-help .pbt-help-palette { display: flex; flex-wrap: wrap; align-items: center; gap: 4px; margin: 0 0 6px; }\n.pbt-help .pbt-help-palette .pbt-sw { width: 16px; height: 16px; }\n.pbt-help .pbt-btn { padding: 1px 10px; border: 1px solid color-mix(in srgb, var(--pbt-ui-fg) 45%, transparent); border-radius: 10px; background: transparent; color: inherit; font: 11px var(--pbt-ui-font); cursor: pointer; }\n.pbt-help .pbt-btn:hover { border-color: var(--pbt-accent); background: color-mix(in srgb, var(--pbt-accent) 25%, transparent); }\n/* The footer under every tab, like a card's footer: the credit line, then the fingerprint, more muted. */\n.pbt-help .pbt-help-foot { display: flex; flex-wrap: wrap; gap: 2px 12px; padding: 8px 14px; border-top: 1px solid var(--pbt-ui-border); font-size: 11px; opacity: .85; }\n.pbt-help .pbt-help-foot .pbt-help-fp { margin-left: auto; opacity: .7; }\n\n/* splash shown when a skin is chosen */\n.pbt-splash {\n  position: fixed; inset: 0; z-index: 2147483002; display: grid; place-items: center; pointer-events: none;\n  background: radial-gradient(circle at 50% 45%, rgba(0, 0, 0, .35), rgba(0, 0, 0, .75));\n  animation: pbt-splash 1.9s ease forwards;\n}\n.pbt-splash > div {\n  padding: 28px 48px; text-align: center; color: var(--pbt-ui-fg); font: 600 34px/1.2 var(--pbt-title-font, var(--pbt-ui-font));\n  background: var(--pbt-ui-bg);)PBT"
LR"PBT( border: 2px solid var(--pbt-accent); border-radius: var(--pbt-ui-radius); box-shadow: var(--pbt-ui-shadow);\n}\n.pbt-splash small { display: block; margin-top: 8px; font: 14px/1.3 var(--pbt-ui-font); opacity: .8; }\n@keyframes pbt-splash { 0% { opacity: 0; } 12% { opacity: 1; } 75% { opacity: 1; } 100% { opacity: 0; } }\n\n/* confetti canvas */\n.pbt-confetti { position: fixed; inset: 0; z-index: 2147483003; pointer-events: none; }\n\n/* Konami code: the report page does a little dance */\n@keyframes pbt-wobble { 0%, 100% { transform: none; } 25% { transform: rotate(-1.2deg); } 75% { transform: rotate(1.2deg); } }\n.pbt-dance .displayArea .visualContainer { animation: pbt-wobble .35s ease-in-out 6; }\n\n/* ---- layout QA overlay ---- */\n.pbt-qa { position: fixed; z-index: 2147482000; pointer-events: none; }\n.pbt-qa {\n  background-image:\n    linear-gradient(to right, rgba(0, 153, 255, .22) 1px, transparent 1px),\n    linear-gradient(to bottom, rgba(0, 153, 255, .22) 1px, transparent 1px),\n    linear-gradient(to right, rgba(0, 153, 255, .10) 1px, transparent 1px),\n    linear-gradient(to bottom, rgba(0, 153, 255, .10) 1px, transparent 1px);\n  background-size: var(--pbt-major) var(--pbt-major), var(--pbt-major) var(--pbt-major), var(--pbt-grid) var(--pbt-grid), var(--pbt-grid) var(--pbt-grid);\n  outline: 1px dashed rgba(0, 153, 255, .8);\n}\n.pbt-ruler { position: absolute; font: 10px/1 Consolas, monospace; color: #0a84ff; }\n.pbt-ruler-x { left: 0; right: 0; top: -14px; height: 14px; }\n.pbt-ruler-y { top: 0; bottom: 0; left: -30px; width: 30px; }\n.pbt-ruler span { position: absolute; background: rgba(255, 255, 255, .85); padding: 1px 2px; border-radius: 2px; }\n.pbt-ruler-x span { transform: translateX(-50%); }\n.pbt-ruler-y span { right: 2px; transform: translateY(-50%); }\n.pbt-box { position: absolute; outline: 1px solid rgba(0, 153, 255, .9); background: rgba(0, 153, 255, .05); }\n.pbt-box.pbt-bad { outline: 2px solid #e5383b; background: rgba(229, 56, 59, .07); }\n.pbt-label {\n  position: absolute; left: -1px; bottom: 100%; pointer-events: auto; cursor: copy;\n  display: flex; gap: 6px; align-items: center; max-width: max(100%, 160px); overflow: hidden; white-space: nowrap;\n  padding: 1px 5px; border: 0; border-radius: 3px 3px 0 0; background: #0a84ff; color: #fff; font: 10px/1.4 Consolas, monospace;\n}\n.pbt-box.pbt-bad .pbt-label { background: #e5383b; }\n.pbt-label em { font-style: normal; font-weight: 700; }\n.pbt-guide { position: absolute; pointer-events: none; }\n.pbt-guide-x { top: 0; bottom: 0; border-left: 1px dashed #e5383b; }\n.pbt-guide-y { left: 0; right: 0; border-top: 1px dashed #e5383b; }\n\n/* ---- Skins group added to the View ribbon ---- */\n/* Sized to the row it sits in (skins with thick frames make that row shorter), and kept clear\n   of the ribbon's collapse chevron at the far right. */\n.pbt-rgroup {\n  display: flex; flex-direction: column; align-self: stretch; box-sizing: border-box; max-height: 100%; overflow: hidden;\n  margin-right: 36px; padding: 0 8px; border-left: 1px solid var(--colorNeutralStroke2, rgba(127, 127, 127, .4)); color: var(--colorNeutralForeground1, inherit);\n}\n.pbt-rgroup-body { flex: 1 1 auto; min-height: 0; display: flex; align-items: center; }\n.pbt-rgroup-label { display: block; flex: none; text-align: center; font-size: 11px; line-height: 14px; margin-bottom: 2px; color: var(--pbt-group-fg, inherit); }\n/* Two rows that grow sideways, however many skins there are. */\n.pbt-rgallery { display: grid; grid-template-rows: repeat(2, auto); grid-auto-flow: column; gap: 2px 3px; }\n.pbt-rbtn {\n  display: flex; flex-direction: column; align-items: center; gap: 0; min-width: 50px; padding: 1px 4px; line-height: 1.15;\n  border: 1px solid transparent; border-radius: 4px; background: transparent; color: inherit; font: inherit; font-size: 11px; cursor: pointer;\n}\n.pbt-rbtn:hover { border-color: var(--pbt-hover-ring, var(--colorNeutralStroke1, #888)); bac)PBT"
LR"PBT(kground: var(--pbt-hover-bg, transparent); }\n.pbt-rbtn[aria-pressed=\"true\"] { border-color: var(--pbt-accent); background: color-mix(in srgb, var(--pbt-accent) 18%, transparent); }\n.pbt-rbtn .pbt-sw { width: 34px; height: 10px; border-radius: 3px; border: 1px solid rgba(127, 127, 127, .6); }\n\n/* An edge off the grid: amber, apart from the red marks, because leaving the grid can be a choice. */\n.pbt-box.pbt-off { outline: 2px dashed #d98e04; background: rgba(217, 142, 4, .06); }\n.pbt-box.pbt-off .pbt-label { background: #b8780a; }\n.pbt-help .pbt-lg-off { width: 22px; height: 14px; outline: 2px dashed #d98e04; background: rgba(217, 142, 4, .1); }\n\n/* A sub-switch under its parent (the cheer under Confetti). */\n.pbt-menu .pbt-item.pbt-sub { padding-left: 34px; }\n/* The tools' jobs (Plan, Layout, Check, While editing) as small labels inside the Tools section. */\n.pbt-menu .pbt-group { margin: 6px 8px 1px 46px; font-size: 10px; letter-spacing: .04em; opacity: .7; }\n/* A skin whose hover paint is its accent would hide the \"on\" track: a ring in the row's own text color keeps it. */\n.pbt-menu button:hover > .pbt-switch, .pbt-menu button:focus-visible > .pbt-switch { box-shadow: inset 0 0 0 1px currentColor; }\n\n/* ---- Layout QA: card, guides with tags, hover distances, shared edges, status strip ---- */\n.pbt-tip.pbt-tip-rich { padding: 0; max-width: 360px; }\n.pbt-card { width: 340px; font: 12px/1.35 var(--pbt-ui-font); }\n.pbt-card header { display: flex; align-items: center; gap: 8px; padding: 8px 10px; border-bottom: 1px solid var(--pbt-ui-border); }\n.pbt-card header b { font-size: 13px; }\n.pbt-card .pbt-card-type { opacity: .7; }\n.pbt-card .pbt-card-badge { margin-left: auto; padding: 1px 8px; border-radius: 10px; font-weight: 700; font-size: 11px; color: #fff; white-space: nowrap; }\n/* Severity, the only thing a badge's color means, for every check tool's badges and cards alike:\n   red to fix, amber to look at, grey for information, green for a pass. */\n.pbt-sev-bad, .pbt-badge-bad { background: #c4314b; }\n.pbt-sev-warn, .pbt-badge-off { background: #9c6609; }\n.pbt-sev-info { background: #5f6b78; }\n.pbt-sev-ok, .pbt-badge-ok { background: #1c8353; }\n.pbt-card-kpi { display: grid; grid-template-columns: repeat(4, 1fr); gap: 1px; background: var(--pbt-ui-border); border-bottom: 1px solid var(--pbt-ui-border); }\n.pbt-card-kpi div { background: var(--pbt-ui-bg); padding: 5px 10px; }\n.pbt-card-kpi small { display: block; opacity: .6; font-size: 10px; text-transform: uppercase; letter-spacing: .06em; }\n.pbt-card-kpi strong { font: 700 15px Consolas, monospace; }\n.pbt-card-kpi strong i { font: 10px var(--pbt-ui-font); opacity: .75; margin-left: 3px; font-style: normal; color: #f39c12; }\n.pbt-card-rows { padding: 6px 10px; display: grid; gap: 5px; }\n.pbt-card-row { display: grid; grid-template-columns: 14px 1fr auto; gap: 8px; align-items: start; }\n.pbt-card-dot { display: inline-block; width: 10px; height: 10px; border-radius: 2px; margin-top: 3px; flex: none; }\n.pbt-dot-edge { background: #e5383b; } .pbt-dot-gap { background: repeating-linear-gradient(45deg, #e5383b 0 2px, #f5a4a0 2px 4px); }\n.pbt-dot-grid { background: #d98e04; } .pbt-dot-ok { background: #1e8e5a; } .pbt-dot-dist { background: #0a84ff; } .pbt-dot-vis { background: #0a84ff; }\n.pbt-card-fix { color: var(--pbt-accent); font: 11px/1.4 Consolas, monospace; white-space: pre-line; text-align: right; }\n.pbt-card footer { padding: 6px 10px; border-top: 1px solid var(--pbt-ui-border); display: flex; gap: 12px; font-size: 11px; opacity: .85; }\n.pbt-card footer b { color: var(--pbt-accent); font-weight: 600; }\n.pbt-label .pbt-pill { padding: 0 5px; border-radius: 8px; background: rgba(255, 255, 255, .25); font-weight: 700; }\n.pbt-box.pbt-hovered { outline-width: 2px; background: rgba(0, 153, 255, .12); }\n.pbt-box.pbt-bad.pbt-hovered { background: rgba(229, 56, 59, .14); }\n.pbt-guide-tag { position: absolute; padding: 1px )PBT"
LR"PBT(5px; background: #e5383b; color: #fff; font: 10px/1.4 Consolas, monospace; border-radius: 3px; white-space: nowrap; }\n.pbt-hover { position: absolute; inset: 0; pointer-events: none; }\n.pbt-dist-line { position: absolute; background: #0a84ff; }\n.pbt-dist-line.pbt-h { height: 1px; } .pbt-dist-line.pbt-v { width: 1px; }\n.pbt-dist-line::before, .pbt-dist-line::after { content: \"\"; position: absolute; background: #0a84ff; }\n.pbt-dist-line.pbt-h::before { left: 0; top: -3px; width: 1px; height: 7px; } .pbt-dist-line.pbt-h::after { right: 0; top: -3px; width: 1px; height: 7px; }\n.pbt-dist-line.pbt-v::before { top: 0; left: -3px; height: 1px; width: 7px; } .pbt-dist-line.pbt-v::after { bottom: 0; left: -3px; height: 1px; width: 7px; }\n.pbt-dist { position: absolute; transform: translate(-50%, -50%); padding: 2px 5px; background: #0a84ff; color: #fff; font: 10px/1 Consolas, monospace; border-radius: 3px; white-space: nowrap; }\n.pbt-dist.pbt-dist-page { background: #5a6b7a; }\n.pbt-shared { position: absolute; pointer-events: none; }\n.pbt-shared-x { top: 0; bottom: 0; border-left: 1px solid rgba(30, 142, 90, .85); }\n.pbt-shared-y { left: 0; right: 0; border-top: 1px solid rgba(30, 142, 90, .85); }\n.pbt-shared-tag { position: absolute; padding: 1px 5px; background: #1e8e5a; color: #fff; font: 10px/1.4 Consolas, monospace; border-radius: 3px; white-space: nowrap; }\n.pbt-shared-x .pbt-shared-tag { left: 3px; bottom: 4px; }\n.pbt-shared-y .pbt-shared-tag { left: 6px; top: -9px; }\n.pbt-qa-strip {\n  position: absolute; left: 0; right: 0; display: flex; flex-wrap: wrap; align-items: center; gap: 4px 12px; padding: 4px 10px; pointer-events: auto; box-sizing: border-box;\n  background: var(--pbt-ui-bg); color: var(--pbt-ui-fg); border: 1px solid var(--pbt-ui-border); border-radius: var(--pbt-ui-radius); font: 12px/1.3 var(--pbt-ui-font); box-shadow: var(--pbt-ui-shadow);\n}\n.pbt-qa-strip .pbt-strip-item { display: inline-flex; align-items: center; gap: 5px; white-space: nowrap; }\n.pbt-qa-strip .pbt-strip-item b { font: 700 13px Consolas, monospace; }\n.pbt-qa-strip .pbt-strip-seg { margin-left: auto; display: inline-flex; gap: 2px; align-items: center; }\n.pbt-qa-strip .pbt-strip-label { opacity: .7; margin-right: 4px; }\n.pbt-qa-strip button { border: 1px solid color-mix(in srgb, var(--pbt-ui-fg) 35%, transparent); border-radius: 3px; background: transparent; color: inherit; font: 11px var(--pbt-ui-font); padding: 1px 7px; cursor: pointer; }\n.pbt-qa-strip button.pbt-on { background: var(--pbt-accent); border-color: var(--pbt-accent); color: var(--pbt-ui-bg); font-weight: 700; }\n.pbt-qa-strip button:hover { border-color: var(--pbt-accent); }\n.pbt-qa-strip .pbt-strip-help { width: 20px; height: 20px; padding: 0; border-radius: 50%; font-weight: 700; }\n.pbt-help .pbt-lg-shared { width: 22px; height: 0; border-top: 1px solid rgba(30, 142, 90, .9); }\n.pbt-help .pbt-lg-dist { width: 22px; height: 12px; background: #0a84ff; border-radius: 2px; }\n\n/* ---- Filter map (page/35-filters.js) ---- */\n.pbt-fm { position: fixed; z-index: 2147482990; pointer-events: none; }\n.pbt-fm-badge {\n  position: absolute; display: flex; gap: 3px; align-items: center; pointer-events: auto; cursor: default;\n  padding: 2px 3px; border-radius: 3px; box-shadow: 0 1px 3px rgba(0, 0, 0, .3);\n  font: 600 10px/1.3 Consolas, monospace; color: #fff; white-space: nowrap;\n}\n.pbt-fm-badge.pbt-fm-has { outline: 1px solid rgba(255, 255, 255, .75); outline-offset: -2px; }\n/* Chips are a letter and a count, all one neutral tint (the badge's color is its severity); the\n   words take their place while the pointer is on the badge. */\n.pbt-fm-chip { padding: 0 4px; border-radius: 2px; background: rgba(255, 255, 255, .16); }\n.pbt-fm-badge .pbt-fm-long, .pbt-fm-badge:hover .pbt-fm-short { display: none; }\n.pbt-fm-badge:hover .pbt-fm-long { display: inline; }\n.pbt-dot-fvisual { background: #8a4dff; }\n.pbt-dot-fpage { background: #0a84ff; }\n.pbt)PBT"
LR"PBT(-dot-freport { background: #12957f; }\n.pbt-dot-fother { background: #d98e04; }\n.pbt-fm-hidden { background: transparent; outline: 1px dashed #c9b3ff; color: #e3d6ff; }\n.pbt-fm-none { background: transparent; color: #c8ced6; font-weight: 400; }\n.pbt-fm .pbt-fm-strip { position: absolute; left: 0; right: 0; pointer-events: auto; }\n.pbt-fm-pane { margin-right: 10px; }\n.pbt-fm-menu {\n  position: fixed; z-index: 2147483000; min-width: 230px; padding: 4px 0; display: grid;\n  background: var(--pbt-fm-bg, var(--pbt-ui-bg, #fff)); color: var(--pbt-fm-fg, var(--pbt-ui-fg, #252423)); border: 1px solid color-mix(in srgb, currentColor 22%, transparent);\n  border-radius: 4px; box-shadow: 0 0 2px rgba(0, 0, 0, .24), 0 4px 8px rgba(0, 0, 0, .28); font: 12px/1.35 var(--pbt-ui-font, \"Segoe UI\", sans-serif);\n}\n.pbt-fm-menu:focus { outline: none; }\n.pbt-fm-menu-head { padding: 6px 12px 4px; font-size: 11px; font-weight: 600; opacity: .7; }\n.pbt-fm-item {\n  display: flex; align-items: center; gap: 10px; width: 100%; min-height: 26px; padding: 4px 12px; border: 0; border-radius: 0;\n  background: transparent; color: inherit; font: inherit; text-align: left; cursor: pointer;\n}\n.pbt-fm-item:hover:not(:disabled), .pbt-fm-item:focus-visible { background: color-mix(in srgb, currentColor 12%, transparent); outline: none; }\n.pbt-fm-item:disabled { opacity: .45; cursor: default; }\n.pbt-fm-icon { flex: none; width: 16px; text-align: center; font-size: 13px; }\n.pbt-fm-sep { height: 1px; margin: 4px 0; background: color-mix(in srgb, currentColor 16%, transparent); }\n.pbt-help .pbt-lg-fvisual, .pbt-help .pbt-lg-fpage, .pbt-help .pbt-lg-freport, .pbt-help .pbt-lg-fother { width: 22px; height: 14px; border-radius: 3px; }\n.pbt-help .pbt-lg-fvisual { background: #8a4dff; } .pbt-help .pbt-lg-fpage { background: #0a84ff; } .pbt-help .pbt-lg-freport { background: #12957f; } .pbt-help .pbt-lg-fother { background: #d98e04; }\n\n/* ---- Theme check (page/45-theme.js) ---- */\n.pbt-th { position: fixed; z-index: 2147482980; pointer-events: none; }\n.pbt-th-badge {\n  position: absolute; pointer-events: auto; cursor: default; padding: 1px 6px; border-radius: 3px;\n  font: 600 10px/1.4 Consolas, monospace; color: #fff; white-space: nowrap; box-shadow: 0 1px 3px rgba(0, 0, 0, .3);\n}\n.pbt-th .pbt-th-strip { position: absolute; left: 0; right: 0; pointer-events: auto; }\n/* One line in the dock, however many tabs lead it: the counts and buttons keep their size and the\n   words beside the counts give way (cut with an ellipsis; each item's tooltip has them in full),\n   the theme check's \"most often\" list first. */\n.pbt-dock .pbt-qa-strip { flex-wrap: nowrap; }\n.pbt-dock .pbt-qa-strip > * { flex-shrink: 0; white-space: nowrap; }\n.pbt-dock .pbt-qa-strip > .pbt-strip-item { flex-shrink: 1; min-width: 0; overflow: hidden; }\n.pbt-dock .pbt-qa-strip > .pbt-th-top { flex-shrink: 1000; }\n.pbt-dock .pbt-qa-strip > .pbt-strip-item > span:last-child { min-width: 0; overflow: hidden; text-overflow: ellipsis; }\n.pbt-dock .pbt-qa-strip > .pbt-strip-item > :is(b, .pbt-card-dot) { flex-shrink: 0; }\n/* Too wide for the dock (page/00-core.js, pbt.dock.place): the extras go, each also in a tooltip. */\n.pbt-dock .pbt-qa-strip.pbt-strip-tight [data-opt] { display: none; }\n.pbt-card-sub { padding: 6px 0 0; font-size: 10px; text-transform: uppercase; letter-spacing: .06em; opacity: .6; }\n/* A heading's note on the right names what the blue values are (the theme check's \"theme value\"). */\n.pbt-card-sub:has(> .pbt-card-fix) { display: flex; justify-content: space-between; gap: 8px; }\n.pbt-card-sub > .pbt-card-fix { text-transform: none; letter-spacing: 0; font-size: 10px; }\n\n/* While the filter map reads Power BI's own filter lists, the popup and the visual header icons\n   it opens are kept out of sight (page/35-filters.js, quiet). */\n/* Only the overlay box that holds the filter list, never another overlay of yours. */\nhtml.pbt-fm-quiet .cdk-)PBT"
LR"PBT(overlay-container > :has(filter-restatement-overlay),\nhtml.pbt-fm-quiet .cdk-overlay-container > :has(filter-restatement-overlay) *,\nhtml.pbt-fm-quiet .displayArea .visualHeaderItemContainer { opacity: 0 !important; }\n/* ...and lets clicks through, so a click of yours during a pass reaches the report. */\nhtml.pbt-fm-quiet .cdk-overlay-container > :has(filter-restatement-overlay),\nhtml.pbt-fm-quiet .cdk-overlay-container > :has(filter-restatement-overlay) * { pointer-events: none !important; }\n\n/* The visual a hovered badge belongs to (pbt.ownBadge). */\n.pbt-owner { position: fixed; z-index: 2147482970; pointer-events: none; outline: 2px dashed #0a84ff; outline-offset: 1px; background: rgba(10, 132, 255, .06); border-radius: 2px; }\n\n/* ---- Interactions helper (page/36-interactions.js), only while Edit interactions is on ---- */\n/* The active interaction icon on each visual: None red, Filter blue, Highlight green. */\nhtml.pbt-ia .displayArea .visualHeaderItemContainer:has(> [aria-label=\"None\"][class*=\"filled\"]) { background: #d13438 !important; border-color: #d13438 !important; border-radius: 3px; }\nhtml.pbt-ia .displayArea .visualHeaderItemContainer:has(> [aria-label=\"Filter\"][class*=\"filled\"]) { background: #0f6cbd !important; border-color: #0f6cbd !important; border-radius: 3px; }\nhtml.pbt-ia .displayArea .visualHeaderItemContainer:has(> [aria-label=\"Highlight\"][class*=\"filled\"]) { background: #107c41 !important; border-color: #107c41 !important; border-radius: 3px; }\nhtml.pbt-ia .displayArea .visualHeaderItemContainer:has(> [class*=\"filled\"][aria-label]) > * { color: #fff !important; opacity: 1 !important; }\n.pbt-ia-host { position: fixed; z-index: 2147482995; pointer-events: none; }\n.pbt-ia-bar {\n  position: absolute; display: flex; align-items: center; gap: 4px; padding: 3px; pointer-events: auto; white-space: nowrap;\n  background: var(--pbt-ui-bg, #fff); color: var(--pbt-ui-fg, #252423); border: 1px solid color-mix(in srgb, currentColor 22%, transparent);\n  border-radius: 4px; box-shadow: 0 0 2px rgba(0, 0, 0, .24), 0 4px 8px rgba(0, 0, 0, .28); font: 12px/1.3 var(--pbt-ui-font, \"Segoe UI\", sans-serif);\n}\n.pbt-ia-btn {\n  display: inline-flex; align-items: center; gap: 6px; padding: 3px 9px; border: 1px solid color-mix(in srgb, currentColor 35%, transparent); border-radius: 3px;\n  background: color-mix(in srgb, currentColor 6%, transparent); color: inherit; font: inherit; cursor: pointer; box-shadow: 0 1px 0 color-mix(in srgb, currentColor 14%, transparent);\n}\n.pbt-ia-btn:hover, .pbt-ia-btn:focus-visible { border-color: var(--pbt-accent, currentColor); background: color-mix(in srgb, currentColor 14%, transparent); outline: none; }\n.pbt-ia-btn:active { box-shadow: none; transform: translateY(1px); }\n.pbt-ia-btn[aria-disabled=\"true\"] { opacity: .45; cursor: default; box-shadow: none; }\n.pbt-ia-btn[aria-disabled=\"true\"]:hover { border-color: color-mix(in srgb, currentColor 35%, transparent); background: color-mix(in srgb, currentColor 6%, transparent); }\n.pbt-ia-btn[aria-disabled=\"true\"]:active { transform: none; }\n.pbt-ia-dot { width: 10px; height: 10px; border-radius: 2px; }\n.pbt-ia-none .pbt-ia-dot { background: #d13438; } .pbt-ia-filter .pbt-ia-dot { background: #0f6cbd; } .pbt-ia-highlight .pbt-ia-dot { background: #107c41; }\n.pbt-ia-count { padding: 0 8px 0 6px; opacity: .75; border-left: 1px solid color-mix(in srgb, currentColor 22%, transparent); margin-left: 2px; }\n\n/* ---- Stencil (page/38-stencil.js) ---- */\n/* Under the other tools' layers, so their badges and strips stay reachable over it. */\n.pbt-st { position: fixed; z-index: 2147481900; pointer-events: none; }\n.pbt-st-edit {\n  pointer-events: auto; cursor: crosshair; outline: 1px dashed color-mix(in srgb, var(--pbt-accent) 80%, transparent);\n  background-image: linear-gradient(to right, color-mix(in srgb, var(--pbt-accent) 16%, transparent) 1px, transparent 1px), linear-gradient(to bott)PBT"
LR"PBT(om, color-mix(in srgb, var(--pbt-accent) 16%, transparent) 1px, transparent 1px);\n  background-size: var(--pbt-grid) var(--pbt-grid);\n}\n.pbt-st .pbt-st-strip { position: absolute; left: 0; right: 0; pointer-events: auto; cursor: default; }\n.pbt-st-block {\n  position: absolute; box-sizing: border-box; border: 1.5px dashed var(--pbt-accent); border-radius: 4px;\n  background: color-mix(in srgb, var(--pbt-accent) 9%, transparent); color: var(--pbt-ui-fg, #252423);\n}\n.pbt-st-edit .pbt-st-block { cursor: move; background: color-mix(in srgb, var(--pbt-accent) 14%, transparent); }\n.pbt-st-block.pbt-st-sel { border-style: solid; box-shadow: 0 0 0 1px var(--pbt-accent); }\n.pbt-st-block.pbt-st-built { border-style: solid; border-color: rgba(30, 142, 90, .9); background: transparent; }\n.pbt-st-block.pbt-st-new { border-style: solid; }\n/* Over an empty page, opaque: Power BI's empty-canvas art does not show through the blocks. */\n.pbt-st-empty .pbt-st-block:not(.pbt-st-built) { background: color-mix(in srgb, var(--pbt-accent) 12%, var(--pbt-ui-bg, #fff)); }\n.pbt-st-empty.pbt-st-edit .pbt-st-block:not(.pbt-st-built) { background: color-mix(in srgb, var(--pbt-accent) 18%, var(--pbt-ui-bg, #fff)); }\n/* No blocks yet: one line centred on the page just above the dock, as wide as its text. */\n.pbt-st-hint {\n  position: absolute; left: 50%; bottom: 8px; transform: translateX(-50%); width: max-content; max-width: 80%; box-sizing: border-box; padding: 6px 12px; pointer-events: none;\n  background: var(--pbt-ui-bg, #fff); color: var(--pbt-ui-fg, #252423); border: 1px dashed var(--pbt-accent); border-radius: 4px;\n  box-shadow: 0 1px 3px rgba(0, 0, 0, .2); font: 12px/1.4 var(--pbt-ui-font, \"Segoe UI\", sans-serif); text-align: center;\n}\n.pbt-st-tag {\n  position: absolute; left: 4px; top: 4px; max-width: calc(100% - 8px); display: inline-flex; align-items: center; gap: 5px; overflow: hidden;\n  padding: 1px 6px; border-radius: 3px; background: var(--pbt-ui-bg, #fff); color: var(--pbt-ui-fg, #252423); font: 11px/1.4 var(--pbt-ui-font, \"Segoe UI\", sans-serif);\n  white-space: nowrap; text-overflow: ellipsis; box-shadow: 0 1px 2px rgba(0, 0, 0, .2);\n}\n.pbt-st-tag .glyphicon { font-size: 12px; color: var(--pbt-accent); }\n.pbt-st-check { color: rgb(30, 142, 90); font-weight: 700; }\n.pbt-st-size {\n  position: absolute; right: 4px; bottom: 4px; padding: 1px 5px; border-radius: 3px; background: var(--pbt-accent); color: var(--pbt-ui-bg, #fff);\n  font: 600 10px/1.4 Consolas, monospace; white-space: nowrap; pointer-events: none;\n}\n.pbt-st-h { position: absolute; width: 8px; height: 8px; box-sizing: border-box; background: var(--pbt-ui-bg, #fff); border: 1.5px solid var(--pbt-accent); border-radius: 1px; }\n.pbt-st-h-n, .pbt-st-h-s { left: calc(50% - 4px); cursor: ns-resize; } .pbt-st-h-e, .pbt-st-h-w { top: calc(50% - 4px); cursor: ew-resize; }\n.pbt-st-h-n, .pbt-st-h-ne, .pbt-st-h-nw { top: -5px; } .pbt-st-h-s, .pbt-st-h-se, .pbt-st-h-sw { bottom: -5px; }\n.pbt-st-h-e, .pbt-st-h-ne, .pbt-st-h-se { right: -5px; } .pbt-st-h-w, .pbt-st-h-nw, .pbt-st-h-sw { left: -5px; }\n.pbt-st-h-ne, .pbt-st-h-sw { cursor: nesw-resize; } .pbt-st-h-nw, .pbt-st-h-se { cursor: nwse-resize; }\n.pbt-st-strip .pbt-st-name { font-weight: 600; }\n.pbt-st-strip .pbt-st-palette { margin-left: 0; }\n.pbt-st-strip .pbt-st-type { width: 26px; height: 22px; padding: 0; display: inline-flex; align-items: center; justify-content: center; }\n.pbt-st-strip .pbt-st-type .glyphicon { font-size: 13px; }\n.pbt-st-strip .pbt-st-typename { min-width: 76px; font-size: 11px; font-weight: 600; white-space: nowrap; }\n.pbt-st-strip .pbt-st-typename.pbt-st-dim { font-weight: 400; opacity: .7; }\n.pbt-st-strip .pbt-st-fields { margin-left: 0; gap: 6px; }\n.pbt-st-field { display: inline-flex; align-items: center; gap: 3px; }\n.pbt-st-field span { opacity: .7; }\n.pbt-st-strip input {\n  box-sizing: border-box; height: 22px; border: 1px solid color-mix(in srgb, var(-)PBT"
LR"PBT(-pbt-ui-fg) 35%, transparent); border-radius: 3px;\n  background: transparent; color: inherit; font: 11px var(--pbt-ui-font); padding: 0 5px;\n}\n.pbt-st-strip input:focus { outline: none; border-color: var(--pbt-accent); }\n.pbt-st-strip .pbt-st-num { width: 56px; font-family: Consolas, monospace; }\n.pbt-st-strip .pbt-st-label-input { width: 150px; }\n.pbt-st-strip .pbt-st-mode { margin-left: auto; }\n.pbt-st-confirm { max-width: 260px; padding: 2px 12px 6px; font-size: 11px; opacity: .85; }\n\n/* Visualizations pane, drill-through well: Power BI fills it with its own light drop-target color and\n   a dark dashed border, unreadable under any dark skin. Same look as the \"Add data fields here\"\n   wells above it: no fill, a border in the pane's text color at 32%. */\nhtml[data-pbt-skin] .drillthroughPane .cards.droppableElement {\n  background: transparent !important; border: 1px solid color-mix(in srgb, currentColor 32%, transparent) !important;\n}\n/* Bottom right and click-through: the visual's own header icons sit top right. */\n.pbt-st-tag.pbt-st-tag-built { left: auto; top: auto; right: 4px; bottom: 4px; padding: 0 5px; pointer-events: none; }\n/* Stencil layouts in the menu: tiles with a small picture of each layout. */\n.pbt-st-tiles { display: grid; grid-template-columns: repeat(3, 132px); gap: 4px; padding: 2px 8px 6px; }\n.pbt-st-tile {\n  display: grid; gap: 4px; padding: 6px; border: 1px solid transparent; border-radius: 4px; background: transparent; color: inherit;\n  font: 11px/1.3 inherit; text-align: left; cursor: pointer;\n}\n.pbt-st-tile:hover, .pbt-st-tile:focus-visible { background: color-mix(in srgb, currentColor 10%, transparent); border-color: color-mix(in srgb, currentColor 22%, transparent); outline: none; }\n.pbt-st-thumb { width: 118px; height: auto; aspect-ratio: 16 / 9; display: block; }\n.pbt-st-thumb-page { fill: color-mix(in srgb, currentColor 6%, transparent); stroke: color-mix(in srgb, currentColor 25%, transparent); stroke-width: 8; }\n.pbt-st-thumb rect:not(.pbt-st-thumb-page) { fill: color-mix(in srgb, var(--pbt-accent) 28%, transparent); stroke: var(--pbt-accent); stroke-width: 6; }\n.pbt-st-thumb rect.pbt-st-thumb-title, .pbt-st-thumb rect.pbt-st-thumb-text { fill: color-mix(in srgb, currentColor 30%, transparent); stroke: none; }\n.pbt-st-thumb rect.pbt-st-thumb-card { fill: color-mix(in srgb, var(--pbt-accent) 50%, transparent); }\n\n/* ---- Colors (page/46-colors.js): hard to read and off palette in one badge per visual ---- */\n.pbt-co { position: fixed; z-index: 2147482970; pointer-events: none; }\n.pbt-co-badge {\n  position: absolute; display: flex; gap: 3px; align-items: center; pointer-events: auto; cursor: default; padding: 1px 6px; border-radius: 3px;\n  font: 600 10px/1.4 Consolas, monospace; color: #fff; white-space: nowrap; box-shadow: 0 1px 3px rgba(0, 0, 0, .3);\n}\n.pbt-co-badge b { font-weight: 600; }\n.pbt-co-badge .pbt-sw { width: 11px; height: 11px; padding: 0; border: 1px solid #fff; border-radius: 2px; cursor: copy; }\n.pbt-co .pbt-co-strip { position: absolute; left: 0; right: 0; pointer-events: auto; }\n/* \"bars, lines and points\": a switch with a track, apart from the AA and AAA buttons. */\n.pbt-qa-strip button.pbt-strip-switch { display: inline-flex; align-items: center; gap: 6px; border: 0; padding: 1px 2px; background: transparent; color: inherit; font-weight: 400; }\n.pbt-qa-strip button.pbt-strip-switch.pbt-on { background: transparent; color: inherit; font-weight: 400; }\n.pbt-switch-track { position: relative; flex: none; width: 24px; height: 12px; border-radius: 6px; box-sizing: border-box; border: 1px solid color-mix(in srgb, var(--pbt-ui-fg) 45%, transparent); }\n.pbt-switch-track::after { content: \"\"; position: absolute; left: 2px; top: 2px; width: 6px; height: 6px; border-radius: 50%; background: color-mix(in srgb, var(--pbt-ui-fg) 60%, transparent); transition: left .12s; }\n.pbt-strip-switch.pbt-on .pbt-switch-track { background:)PBT"
LR"PBT( var(--pbt-accent); border-color: var(--pbt-accent); }\n.pbt-strip-switch.pbt-on .pbt-switch-track::after { left: 14px; background: var(--pbt-ui-bg); }\n.pbt-qa-strip button.pbt-strip-switch:hover .pbt-switch-track, .pbt-qa-strip button.pbt-strip-switch:focus-visible .pbt-switch-track { border-color: var(--pbt-accent); }\n.pbt-ct-pair { display: inline-flex; align-items: center; gap: 4px; min-width: 0; }\n.pbt-ct-pair > span:last-child { overflow: hidden; text-overflow: ellipsis; white-space: nowrap; }\n.pbt-ct-sw { flex: none; width: 12px; height: 12px; border-radius: 2px; box-shadow: inset 0 0 0 1px rgba(127, 127, 127, .6); }\n/* While a contrast scan hit-tests the canvas, every part of it answers, so a data label's bar is\n   found under it (Power BI turns pointer events off on many SVG parts). */\nhtml.pbt-ct-probe .displayArea * { pointer-events: auto !important; }\n.pbt-help .pbt-lg-ctbad, .pbt-help .pbt-lg-ctwarn, .pbt-help .pbt-lg-ctok { width: 30px; height: 14px; border-radius: 3px; }\n.pbt-help .pbt-lg-ctbad { background: #c4314b; } .pbt-help .pbt-lg-ctwarn { background: #9c6609; } .pbt-help .pbt-lg-ctok { background: rgba(28, 131, 83, .9); }\n/* The tooltip turns spans into lines; a contrast pair stays on one: swatches, then the text. */\n.pbt-tip .pbt-ct-pair { display: flex; }\n.pbt-tip .pbt-ct-pair > span { display: inline-block; }\n\n/* ---- The tools' dock (page/00-core.js, pbt.dock): one bar under the page for every tool's\n   strip, a tab row first when two or more tools are on. Never below the visible canvas. ---- */\n.pbt-dock {\n  position: fixed; z-index: 2147482995; display: flex; align-items: stretch; pointer-events: auto; box-sizing: border-box;\n  background: var(--pbt-ui-bg); color: var(--pbt-ui-fg); border: 1px solid var(--pbt-ui-border); border-radius: var(--pbt-ui-radius);\n  box-shadow: var(--pbt-ui-shadow); font: 12px/1.3 var(--pbt-ui-font);\n}\n.pbt-dock-tabs { flex: none; display: flex; align-items: center; gap: 2px; padding: 4px 8px; border-right: 1px solid color-mix(in srgb, var(--pbt-ui-fg) 18%, transparent); }\n.pbt-dock-tab {\n  display: inline-flex; align-items: center; gap: 5px; border: 1px solid transparent; border-bottom-width: 2px; border-radius: 3px 3px 0 0;\n  background: transparent; color: inherit; font: 11px var(--pbt-ui-font); padding: 1px 7px; cursor: pointer; white-space: nowrap;\n}\n.pbt-dock-tab:hover, .pbt-dock-tab:focus-visible { border-color: color-mix(in srgb, var(--pbt-accent) 60%, transparent); outline: none; }\n/* The dot: this tool is on (drawing on the page). The underline: its strip is the one shown. */\n.pbt-dock-dot { flex: none; width: 6px; height: 6px; border-radius: 50%; background: #1e8e5a; box-shadow: 0 0 0 1px color-mix(in srgb, var(--pbt-ui-bg) 70%, transparent); }\n.pbt-dock-tab.pbt-on { border-bottom-color: var(--pbt-accent); font-weight: 700; background: color-mix(in srgb, var(--pbt-accent) 14%, transparent); }\n.pbt-dock-body { flex: 1; min-width: 0; display: flex; }\n/* A strip in the dock is the dock's content: no frame or position of its own. */\n.pbt-dock .pbt-qa-strip { position: static; flex: 1; min-width: 0; border: 0; box-shadow: none; border-radius: 0; background: transparent; }\n/* Power BI's page has no [hidden] rule of its own, and the strips are display: flex. */\n.pbt-dock [hidden] { display: none !important; }\n\n/* ---- Badge rails (page/00-core.js, pbt.rail): one row of check badges per visual, just outside its\n   bottom-right corner, tethered to its bottom edge in the worst badge's severity color (inside the\n   corner when the gap below is too short); Filter map, Theme, then Colors. ---- */\n.pbt-rails { position: fixed; left: 0; top: 0; width: 0; height: 0; z-index: 2147482990; pointer-events: none; }\n.pbt-rail { position: absolute; transform: translateX(-100%); display: flex; align-items: center; gap: 3px; white-space: nowrap; pointer-events: none; }\n.pbt-rail[hidden] { display: none; }\n.pbt-rail-tether { position:)PBT"
LR"PBT( absolute; left: 3px; right: 3px; bottom: 100%; height: 4px; box-sizing: border-box; border: 2px solid rgba(90, 107, 122, .9); border-bottom: 0; pointer-events: none; }\n.pbt-rail[data-sev=\"bad\"] .pbt-rail-tether { border-color: #c4314b; }\n.pbt-rail[data-sev=\"warn\"] .pbt-rail-tether { border-color: #9c6609; }\n.pbt-rail[data-sev=\"info\"] .pbt-rail-tether { border-color: #5f6b78; }\n.pbt-rail[data-sev=\"ok\"] .pbt-rail-tether { border-color: #1c8353; }\n.pbt-rail.pbt-rail-in .pbt-rail-tether { display: none; }\n.pbt-rail > [data-rail-tool] { position: static; transform: none; margin: 0; }\n/* Every badge leads with its tool's letter (F, T, C), so its color can mean severity alone. */\n.pbt-rail > [data-rail-tool]::before {\n  content: attr(data-glyph); display: inline-block; margin-right: 4px; padding-right: 4px; border-right: 1px solid rgba(255, 255, 255, .45);\n  font: 700 9px/1 Consolas, monospace; color: #fff; opacity: .85;\n}\n.pbt-rail > .pbt-fm-badge::before { margin-right: 1px; }\n.pbt-rail > [data-rail-tool=\"filters\"] { order: 1; }\n.pbt-rail > [data-rail-tool=\"theme\"] { order: 2; }\n.pbt-rail > [data-rail-tool=\"colors\"] { order: 3; }\n/* Focus mode: a tool whose strip the dock does not show draws a dot with its letter; full on hover. */\n.pbt-rail > .pbt-dot:not(:hover) {\n  width: 15px; height: 15px; padding: 0; gap: 0; box-sizing: border-box; border-radius: 50%; outline: 0;\n  display: inline-flex; align-items: center; justify-content: center; font-size: 0;\n}\n.pbt-rail > .pbt-dot:not(:hover) > * { display: none; }\n.pbt-rail > .pbt-dot:not(:hover)::before { content: attr(data-glyph); margin: 0; padding: 0; border: 0; opacity: 1; font: 700 9px/1 Consolas, monospace; color: #fff; }\n/* Layout QA's guide tags while a check tool is on: kept in place (the rails avoid them) but shown\n   only for the hovered visual. */\n.pbt-qa.pbt-qa-quiet .pbt-guide-tag:not(.pbt-hov) { visibility: hidden; }\n\n/* Pinpointing (pbt.pinpoint): the parts a finding is about, outlined and numbered while its badge\n   (or swatch) is hovered; the numbers match the card's rows. Under the card, over the page. */\n.pbt-hit { position: fixed; z-index: 2147482996; box-sizing: border-box; border: 2px solid #c4314b; border-radius: 3px; box-shadow: 0 0 0 1px rgba(255, 255, 255, .85); pointer-events: none; }\n.pbt-hit-pin, .pbt-hit-num {\n  display: inline-flex; align-items: center; justify-content: center; min-width: 16px; height: 16px; padding: 0 3px; box-sizing: border-box;\n  border-radius: 8px; background: #c4314b; color: #fff; font: 700 10px/1 Consolas, monospace;\n}\n.pbt-hit-pin { position: absolute; top: -10px; left: -10px; box-shadow: 0 1px 3px rgba(0, 0, 0, .35); }\n.pbt-tip .pbt-hit-num { display: inline-flex; }\n";
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
  pbt.cleanups.push(() => target.removeEventListener(type, fn, opts));
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
c)PBT"
LR"PBT(onst HOST = typeof PBT_HOST !== 'undefined' ? PBT_HOST : 'devtools';
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
  } catch {}
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
pbt.hotkey = (key, label, fn, extra = {}) => pbt.hotkeys.push({ key: key.toLowerCase(), label, fn, ...extra });
pbt.on(window, 'keydown', e => {
  if (!e.ctrlKey || !e.altKey || e.metaKey) return;
  const hk = pbt.hotkeys.find(h => h.key === e.key.toLowerCase());
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
    pbt.hideOwner();
    const r = v.el.isConnected ? v.el.getBoundingClientRect() : null;
    if (!r || !r.width) return;
    document.body.append(pbt.el('div', { class: 'pbt-owner', 'data-pbt': '', style: `left:${r.left}px;top:${r.top}px;width:${r.width}px;height:${r.height}px` }));
  };
  badge.addEventListener('pointerenter', show);
  badge.addEventListener('pointerleave', pbt.hideOwner);
  return badge;
};
pbt.hideOwner = () => document.querySelectorAll('.pbt-owner').forEach(e => e.remove());

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
  c)PBT"
LR"PBT(onst fine = pbt.tip(el('span', { class: 'pbt-strip-item', dataset: { count: 'ok' } }, el('span', { class: 'pbt-card-dot pbt-dot-ok' }), el('b', { text: `${pass} of ${total}` }), el('span', { text: 'fine' })),
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
  badge.dataset.railTool = tool;
  badge.dataset.glyph = RAIL_GLYPH[tool] || '?';
  badge.classList.toggle('pbt-dot', !pbt.focusFull(tool));
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
  if (pbt.qa && pbt.qa.host) pbt.qa.host.classList.toggle('pbt-qa-quiet', Object.values(t).some(x => x && x.on));
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
    out.push({ left: r.left, right: r)PBT"
LR"PBT(.right, top: r.top, bottom });
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
    // A rail under the pointer stays put: its badge may have grown from a dot.
    if (rail.matches(':hover')) { placed.push(at(wasX, wasY)); continue; }
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
      const lo = dy < 0 ? (slide < 2 * RAIL_INSET ? Math.round(slide / 2) : RAIL_INSET) : 0, hi = dy < 0 ? Math.max(lo, slide - RAIL_INSET) : slide;
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
pbt.pinpoint = groups => {
  pbt.unpinpoint();
  // Pins on the same part line up side by side instead of covering each other.
  const pinsAt = new Map();
  for (const grp of groups) {
    let first = true;
    for (const e of (grp.els || []).slice(0, 40)) {
      if (!e || !e.isConnected) continue;
      const r = e.getBoundingClientRect();
      if (r.width < 1 || r.height < 1) continue;
      const box = pbt.el('div', { class: 'pbt-hit', 'data-pbt': '', style: `left:${r.left - 2}px;top:${r.top - 2}px;width:${r.width + 4}px;height:${r.height + 4}px` });
      if (first && grp.n !== '' && grp.n != null) {
        const at = `${Math.round(r.left)},${Math.round(r.top)}`, k = pinsAt.get(at) || 0;
        pinsAt.set(at, k + 1);
        box.append(pbt.el('span', { class: 'pbt-hit-pin', style: `left:${-10 + k * 20}px`, text: String(grp.n) }));
      }
      firs)PBT"
LR"PBT(t = false;
      document.body.append(box);
    }
  }
};
pbt.unpinpoint = () => document.querySelectorAll('.pbt-hit').forEach(e => e.remove());
// A number for a card row, matching the pin on the page.
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
pbt.placeStr)PBT"
LR"PBT(ip = (bar, name) => pbt.dock.adopt(name, bar);
// A tool turned on or off: every other tool redraws on its next tick and the dock lays out again.
// Only the redraw keys are reset (sig, drawKey); never a name a tool uses for a method.
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
{
  let box = null, timer = 0, target = null;
  const hide = () => { clearTimeout(timer); timer = 0; target = null; if (box) box.hidden = true; };
  const show = el => {
    const text = el.dataset.pbtTip;
    if (!text || !el.isConnected) return;
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
  pbt.on(document, 'pointerover', e => {
    const el = e.target.closest && e.target.closest('[data-pbt-tip]');
    if (el === target) return;
    hide();
    if (!el) return;
    target = el;
    timer = setTimeout(() => show(el), 350);
  }, true);
  pbt.on(document, 'pointerout', e => { if (target && !(e.relatedTarget && target.contains(e.relatedTarget))) hide(); }, true);
  pbt.on(document, 'focusin', e => { const el = e.target.closest && e.target.closest('[data-pbt-tip]'); if (el) { hide(); target = el; show(el); } }, true);
  pbt.on(document, 'focusout', () => hide(), true);
  pbt.on(document, 'pointerdown', () => hide(), true);
  pbt.on(document, 'keydown', e => { if (e.key === 'Escape') hide(); }, true);
  pbt.on(window, 'blur', () => hide());
  pbt.hideTip = hide;
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
    for (const m of pbt.modules) { try { m.start && m.start(); } catch (e) { pbt.log(`${m.name} start: ${e.stack}`); } }
    if (isReport) pbt.every(400, () => { if (document.querySelector('.pbt-nudge, .pbt-rail')) pbt.nudgeBadges(); });
    observer.observe(document.body, { childList: true, subtree: true });
  };
  boot();
  return `pbid-tweaks gen ${gen} (${pbt.modules.map(m => m.name).join(', ')})`;
};

// ---- 05-project.js ----
// The report's saved project files (PBIR), for the tools that compare the page with what the
// report defines: the theme, each page and each visual's own formatting and filters. Both hosts
// hand over the same shape, PBT_PROJECT = { root, files: [{ path, text }] } with paths relative
// to the .Report folder: inject.mjs reads tweaks.json "project" on every reload, the mod reads
// its "project" setting when Power BI starts or the settings change. Nothing here touches the
// page; everything is pure so scripts/tool-check.mjs can run it on fixtures.

const project = {
  raw: typeof PBT_PROJECT )PBT"
LR"PBT(!== 'undefined' ? PBT_PROJECT : null,
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
  // filters, visuals: [{ name, type, x, y, w, h, z, title, objects, containerObjects, filters }] }] }.
  parse(raw) {
    if (!raw || !Array.isArray(raw.files)) return null;
    const json = new Map();
    for (const f of raw.files) { try { json.set(f.path, JSON.parse(f.text)); } catch {} }
    const report = json.get('definition/report.json') || {};
    const tc = report.themeCollection || {};
    // The custom theme over the base theme, when the project carries the base theme's file
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
  )PBT"
LR"PBT(      const vis = v.visual || {};
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
      pages.push({ name: page.name || m[1], displayName: page.displayName || m[1], width: page.width, height: page.height, filters: this.filters(page.filterConfig), visuals });
    }
    pages.sort((a, b) => (order.indexOf(a.name) + 1 || 1e6) - (order.indexOf(b.name) + 1 || 1e6));
    return { root: raw.root || '', report, themeName: themeName || '', theme, filters: this.filters(report.filterConfig), pages, activePage: pagesMeta.activePageName || '' };
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
  match(v, page = this.page(), tol = 0.5) {
    if (!page) return { file: null, reason: 'no page of that name in the project files' };
    // "Clustered bar chart" on the page is "barChart" in the file, "Table visual" is "tableEx".
    // The page's name for a type ("Clustered bar chart") against the file's visualType
    // ("clusteredBarChart", or the older "barChart"): exact after letters only, or a known alias.
    // A prefix match is used only when exactly one saved visual on the page fits it, so a line
    // chart is never paired with a line and column combo chart.
    const ALIAS = { tableex: 'table', pivottable: 'matrix', barchart: 'stackedbarchart', columnchart: 'stackedcolumnchart', cardvisual: 'card', multirowcard: 'multirowcard', basicshape: 'shape', actionbutton: 'button', textbox: 'textbox' };
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
//  )PBT"
LR"PBT( vars    extra --pbt-* values the skin CSS and our own UI read (font, title bar, etc.).

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
  const lum = c => { const f = v => { v /= 255; return v <= 0.03928 ? v / 12.92 : ((v + 0.055) / 1.055) ** 2.4; }; return 0.2126 * f(c.r) + 0.7152 * f(c.g) + 0.0722 * f(c.b); };
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
// source's polarity when )PBT"
LR"PBT(it is not the theme's (the Monaco editor follows the mode, not the theme).
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
    if (srcDark !== skinDark) return color.hex(color.fromHsl({ ...h, l: Math.max(0.2, Math.min(0.8, 1 - h.l)) }, c.a));
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
    if (sheet.ownerNode && sheet.ownerN)PBT"
LR"PBT(ode.dataset && 'pbt' in sheet.ownerNode.dataset) continue;
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
    `${sel} .monaco-editor .cursor { background-color: var(--colorNeutralForeground1) !important; }`,
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
    // Timers rather than animation frames, which a covered window does not get.
    setTimeout(() => window.dispatchEvent(new Event('resize')), 30);
    for (const ms of [60, 400]) setTimeout(() => this.alignUnderline(), ms);
  },
  alignUnderline() {
    const bar = document.querySelector('#selectedPivotIndicator');
    const open = document.querySelector('#tablist button[role="tab"][aria-selected="true"]');
    if (!bar || !open) return;
    const b = bar.getBoundingClientRect(), o = open.getBoundingClientRect();
    if (!b.width) return;
    const shift = (o.left + (o.width - b.width) / 2) - b.left;
    if (Math.abs(shift) > 0.5) bar.style.left = `${(parseFloat(getComputedStyle(bar).left) || 0) + shift}px`;
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
    // color tokens only when )PBT"
LR"PBT(its controls first render, through insertRule, which no DOM
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
  // style sheet and data-pbt-skin are taken away by pbt.remove itself.
  stop() { document.documentElement.classList.remove('pbt-report-filters'); },
};
pbt.skin = skins;
pbt.module(skins);

// ---- 20-menu.js ----
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
  sounds: { label: 'Skin sounds', desc: 'A few synthesized notes when a skin is picked or a refresh finishes. No audio files.', tryLabel: 'Try', helpTry: 'Play this skin\'s notes', tryTip: 'Play this skin\'s notes now', run: () => pbt.fun && pbt.fun.play(pbt.skins[pbt.skin.current] || pbt.skins[pbt.cfg.skin], true) },
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
  shownSkin() { return pbt.skin && pbt.skin.current !== undefined ? pbt.skin.current : pbt.cfg.skin; },
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
    document.querySelectorAll('.pbt-hint').forEach(n => n.remove());
    if (!this.chip || !this.chip.isConnected || pbt.ui.get('hint.menu')) return null;
    const done = () => { clear)PBT"
LR"PBT(Timeout(timer); pbt.ui.set('hint.menu', true); h.remove(); };
    const h = pbt.el('span', { class: 'pbt-hint', 'data-pbt': '', role: 'note' },
      pbt.el('span', { text: 'Ctrl+Alt+K: tools and skins' }),
      pbt.tip(pbt.el('button', { class: 'pbt-hint-x', 'aria-label': 'Dismiss', text: '\u00d7', onclick: e => { e.stopPropagation(); done(); } }), 'Dismiss\nThis hint does not come back.'));
    const timer = setTimeout(done, 20000);
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
    count.textContent = `${n} visual${n === 1 ? '' : 's'}`;
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

    /)PBT"
LR"PBT(/ Tools first, grouped by job (pbt.JOBS: the order the dock, the help and the docs use too).
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
    const tile = (id, label, sw) => pbt.tip(pbt.el('button', { role: 'menuitemradio', 'aria-checked': String(shown === id), onclick: e => { e.stopPropagation(); this.close(); if (id !== 'off') pbt.ui.set('menu.skins', true); pbt.skin.set(id); } },
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
    pbt.on(document, 'pointerdown', e => { if (this.open && !this.open.contains(e.target) && e.target !== this.chip && !this.chip.contains(e.target)) this.close(); }, true);
    pbt.on(document, 'keydown', e => { if (e.key === 'Escape' && this.open) { e.stopPropagation(); this.close(); } }, true);
    pbt.hotkey('k', 'Open the menu', () => this.toggle(this.chip), { desc: 'Skins, tools, extras and help, from anywhere in the window.' });
  },
  onConfig() { this.refresh(); },
  onApplied() { this.refresh(); if (this.open) this.open.querySelectorAll('.pbt-skins button').forEach((b, i) => b.setAttribute('aria-checked', String((i ? pbt.skin.ids()[i - 1] : 'off') === this.shownSkin()))); },
  stop() { this.close(); document.querySelectorAll('.pbt-hint').forEach(n => n.remove()); },
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
      [w, h] = best ? [be)PBT"
LR"PBT(st.cand, best.h] : [Math.round(dw / zoom), Math.round(dh / zoom)];
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
      const typeName = (String(type && type.className).match(/\bvisual-(\w+)/) || [])[1] || 'visual';
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
          else if (Math.abs(d) >= 1 && Math.abs(d) <= nearMiss) { const k = String(edgeValue(b.v, e)); if (!near.has(k)) near.set(k, []); near.get(k).push(b.v); )PBT"
LR"PBT(}
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
        if (exact.length >= refs.length) continue; // it sits with as many as it misses: aligned
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
  )PBT"
LR"PBT(    // finding moved (that move is a translation, so the far edge goes with it); the finding lists
      // only the snaps applied, so the card and the figures agree.
      const moved = new Set(a.findings.filter(f => f.kind === 'edge').map(f => f.edge));
      for (const f of a.findings) if (f.kind === 'gap') (f.axis === 'x' ? ['left', 'right'] : ['top', 'bottom']).forEach(e => moved.add(e));
      const gridFinding = a.findings[a.findings.length - 1];
      gridFinding.suggestEdges = gridFinding.suggestEdges.filter(([e]) => !moved.has(e));
      for (const [e, val] of gridFinding.suggestEdges) {
        if (e === 'left') { a.suggest.w = a.suggest.x + a.suggest.w - val; a.suggest.x = val; }
        else if (e === 'right') a.suggest.w = val - a.suggest.x;
        else if (e === 'top') { a.suggest.h = a.suggest.y + a.suggest.h - val; a.suggest.y = val; }
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
    const gridBtn = n => button({ class: `pbt-strip-grid${cfg.grid === n ? ')PBT"
LR"PBT( pbt-on' : ''}`, text: String(n) }, () => this.setQa('grid', n), `${n} px grid\nDraws the grid every ${n} px and checks edges against it (qa.grid).`);
    const snapBtn = button({ class: `pbt-strip-grid${cfg.snap ? ' pbt-on' : ''}`, text: 'grid check' }, () => this.setQa('snap', !cfg.snap), 'Grid check\nAmber marks on visuals with an edge off the grid (qa.snap). Snap to grid in the View ribbon places edges on it while you drag.');
    const helpBtn = button({ class: 'pbt-strip-help', text: '?' }, () => { if (pbt.help) { pbt.help.close(); pbt.help.open(); pbt.help.show('qa'); } }, 'What the marks mean\nOpens the help page at Layout QA.');
    const bar = el('div', { class: 'pbt-qa-strip' },
      item('', null, `${g.w} \u00d7 ${g.h} page${g.scale < 0.999 ? ` \u00b7 \u00b11 px at ${Math.round(g.scale * 100)}%` : ''}`, `Page size in report pixels\nRead from the zoom level and the page's shape; pageSize in the settings pins it.${g.scale < 0.999 ? `\nBelow 100% zoom one screen pixel covers ${(1 / g.scale).toFixed(1)} report pixels, so an edge can read 1 px off; where two values draw the same, the one on the grid is shown. Zoom to 100% (or wider) for exact values.` : ''}`),
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

  draw(announce) {
    const g = pbt.page.geometry();
    if (!g) { this.clear(); return; }
    const vs = pbt.page.visuals(g);
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
      const box = pbt.el('div', { class: `pbt-box${bad ? ' pbt-bad' : offGrid ? ' pbt-off' : ''}`, dataset: { index: String(a.v.i) }, style: `left:${px(v.x)};top:${px(v.y)};width:${px(v.w)};height:${px(v.h)}` });
      const dims = `${v.x}, ${v.y} \u00b7 ${v.w} \u00d7 )PBT"
LR"PBT(${v.h}`;
      const n = a.findings.filter(f => f.kind !== 'grid').length;
      const label = pbt.tip(pbt.el('button', {
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
    if (idx < 0) return;
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
    const t = pbt.el('textarea', { 'data-pbt': )PBT"
LR"PBT('', style: 'position:fixed;left:-9999px' });
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
  report: { label: 'Report', short: 'R', words: 'report', tip: 'A report-level filter: every visual on every page.' },
  other: { label: 'Slicers & cross-filters', short: 'S', words: 'from slicers & cross-filters', tip: 'From slicers & cross-filters: a slicer, a cross-filter or a selection in another visual, a drillthrough or a tooltip page.' },
};

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
    if (!g) return f)PBT"
LR"PBT(alse;
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
  // Closes the filter list this tool opened, and nothing else: the Escape goes to that popup and
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
        if (level === 'visual' && !c.hidden && !listed.has()PBT"
LR"PBT(c.title.toLowerCase())) visualFields.set(c.field.toLowerCase(), (visualFields.get(c.field.toLowerCase()) || 0) + 1);
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
  // Keyed by page, type and position, not by element: Power BI builds new elements when you
  // leave a page and come back.
  live: new Map(),
  popups: new Map(),
  key(v) { return `${(document.querySelector('.carouselScrollPane li.section.selected') || {}).textContent || ''}|${v.kind}|${v.x},${v.y},${v.w},${v.h}`; },
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
  refreshPane() {
    this.capturePane();
    if (this.on) { this.compose(); this.sig = this.layoutSig(); this.draw(); }
  },
  quiet(on) { document.documentElement.classList.toggle('pbt-fm-quiet', on); },
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
    const run = this.chain.then(async () => {
      if (gen !== this.gen) return undefined;
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
  refreshPopups() {
    // A minimized or hidden window throttles timers (a 100 ms wait takes most of a second), so
    // the lists are read when it is shown again.
    if (document.hidden) { this.stale = true; return Promise.resolve(); }
    if (this.passQueued) return this.passPromise;
    this.passQueued = true;
    this.passPromise = this.exclusive(async gen => {
      this.passQueued = false;
      if (this.userBusy()) { this.retryPass(1500); return; }
      this.quiet(true);
      const until = Date.now() + 15000;
      let resumed = false;
      try {
        const vs = pbt.page.visuals();
        const start = this.resumeAt && this.resumeAt < vs.length ? this.resumeAt : 0;
        this.resumeAt = 0;
        for (let i = start; i < vs.length; i++) {
          if (gen !== this.gen) return;
          if (Date.now() > until || document.hidden || this.userBusy()) { this.resumeAt = i; resumed = true; break; }
          const v = vs[i];
          const prevEl = document.querySelector('.displayArea .visualContainer.selected');
          this.popups.set(this.key(v), )PBT"
LR"PBT(await this.popup(v));
          // Safety net, narrow on purpose: if reading this visual selected it, undo that one
          // selection; a selection you make meanwhile is left alone.
          if (v.el.classList.contains('selected') && prevEl !== v.el) {
            const prev = prevEl && prevEl.isConnected && vs.find(x => x.el === prevEl);
            if (prev) this.select(prev); else this.deselect();
          }
        }
      } finally { this.quiet(false); }
      if (gen !== this.gen) return;
      if (this.on) { this.compose(); this.sig = this.layoutSig(); this.draw(); }
      if (resumed) this.retryPass(document.hidden ? 0 : 400);
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
  async scan(announce) {
    this.pageName = (document.querySelector('.carouselScrollPane li.section.selected') || {}).textContent;
    await this.idle();
    this.refreshPane();
    await this.refreshPopups();
    await this.idle();
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
  // Where a filter from another element comes from: the slicers on the page for that field (by
  // the slicer's own header or title). A cross-filter's source is not named by Power BI, so none.
  sourcesFor(item, live) {
    const f = String(item.field || '').replace(/^(included|excluded)\b.*$/i, '').trim().toLowerCase() || String(item.text || '').split(/\s+is\s+/i)[0].trim().toLowerCase();
    if (!f) return [];
    return live.filter(v => /slicer/i.test(v.kind) && ((v.title || '').toLowerCase().includes(f) || [...v.el.querySelectorAll('.slicer-header-text, [class*="headerText" i]')].some(h => h.textContent.trim().toLowerCase() === f))).map(v => v.el);
  },
  card(row, sources = new Map()) {
    const el = pbt.el, v = row.v, n = this.counts(row.items);
    const total = row.items.length;
    let pin = 0;
    const rows = row.items.map(i => el('div', { class: 'pbt-card-row' },
      i.level === 'other' && (sources.get(i) || []).length ? pbt.pinNum(++pin) : el('span', { class: `pbt-card-dot pbt-dot-f${i.level}` }),
      el('span', { text: `${LEVELS[i.level].label}: ${i.text}` }),
      el('span', { class: 'pbt-card-fix', text: [i.hidden ? 'hidden' : '', i.locked ? 'locked' : ''].filter(Boolean).join(' \u00b7 ') })));
    if (!rows.length && !row.unknown) rows.push(el('div', { class: 'pbt-card-row' }, el('span', { class: 'pbt-card-dot pbt-dot-ok' }), el('span', { text: 'No filters affect this visual.' }), el('span')));
    if (row.unknown) rows.push(el('div', { class: 'pbt-card-row' }, el('span', { class: 'pbt-card-dot pbt-dot-grid' }), el('span', { text: 'Not in the project files, so its own filters (hidden ones included) are unknown: select it, or use "Read each visual" on the strip.' }), el('span')));
    if (row.noButton) rows.push(el('div', { class: 'pbt-card-row' }, el('span', { class: 'pbt-card-dot pbt-dot-grid' }), el('span', { text: 'This visual has no filter list of its own, so slicers and cross-filters are not shown for it.' }), el('span')));
    return el('div', { class: 'pbt-card' },
      el('header', {}, el('b', { text: v.title || v.kind }), v.title ? el('span', { class: 'pbt-card-type', text: v.kind }) : null,
        el('span', { class: 'pbt-card-badge pbt-sev-info', text: total ? `${total} filter${total === 1 ? '' : 's'}` : 'none' })),
      el('div', { class: 'pbt-card-kpi' }, ...['visual', 'page', 'report', 'other'].map(k => e)PBT"
LR"PBT(l('div', {}, el('small', { text: LEVELS[k].label }), el('strong', {}, String(n[k]), k === 'visual' && (n.hidden || n.locked) ? el('i', { text: [n.hidden ? `${n.hidden} hidden` : '', n.locked ? `${n.locked} locked` : ''].filter(Boolean).join(', ') }) : null)))),
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
      const sources = new Map();
      const n = this.counts(row.items);
      // A chip is a letter and a count ("V1"), spelled out while the pointer is on the badge
      // ("1 visual"); the badge is grey information like every finding that is not a fault.
      const chips = ['visual', 'page', 'report', 'other'].filter(k => n[k]).map(k => el('span', { class: `pbt-fm-chip pbt-fm-${k}` },
        el('span', { class: 'pbt-fm-short', text: `${LEVELS[k].short}${n[k]}` }), el('span', { class: 'pbt-fm-long', text: `${n[k]} ${LEVELS[k].words}` })));
      if (n.hidden) chips.push(el('span', { class: 'pbt-fm-chip pbt-fm-hidden', text: `${n.hidden} hidden` }));
      if (row.unknown) chips.unshift(el('span', { class: 'pbt-fm-chip pbt-fm-unknown' }, el('span', { class: 'pbt-fm-short', text: 'V?' }), el('span', { class: 'pbt-fm-long', text: 'own filters unknown' })));
      const badge = pbt.tip(el('div', { class: `pbt-fm-badge pbt-sev-info${n.visual ? ' pbt-fm-has' : ''}` },
        ...(chips.length ? chips : [el('span', { class: 'pbt-fm-chip pbt-fm-none', text: 'no filters' })])), () => this.card(row, sources));
      badge.dataset.fmVisual = row.v.title || row.v.kind;
      pbt.ownBadge(badge, v);
      // Kept beside the items, never on them: the items are plain data (tests serialize them).
      for (const i of row.items) if (i.level === 'other') sources.set(i, this.sourcesFor(i, live).filter(e => e !== v.el));
      badge.addEventListener('pointerenter', () => { let n = 0; pbt.pinpoint(row.items.filter(i => (sources.get(i) || []).length).map(i => ({ els: sources.get(i), n: ++n }))); });
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
    const item = (cls, n, text, tip) => pbt.tip(el('span', { class: 'pbt-strip-item', dataset: { count: cls } }, el('span', { class: `pbt-card-dot pbt-dot-f${cls}` }), el('b', { text: String(n) }), el('span', { text })), tip);
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
        rows.some(r => r.unknown) ? button({ class: 'pbt-strip-grid', )PBT"
LR"PBT(text: 'Read each visual' }, () => this.readEachVisual(), 'Read each visual\nSome visuals are not in the project files, so their hidden filters are unknown. This selects each visual in turn to read its cards, then puts the selection back. Setting the report project folder avoids it.') : null,
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
    if (pane) { m.style.setProperty('--pbt-fm-bg', pane.backgroundColor); m.style.setProperty('--pbt-fm-fg', getComputedStyle(document.querySelector('.outspacePane')).color); }
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
  tabs() { return [...document.querySelectorAll('.carouselScrollPane li.section')].filter(li => li.o)PBT"
LR"PBT(ffsetParent !== null); },
  bulk(action, scope) { return this.exclusive(gen => this.bulkRun(action, scope, gen)); },
  async bulkRun(action, scope, gen) {
    this.cancel = false;
    const verb = { hide: 'Hid', show: 'Showed', lock: 'Locked', unlock: 'Unlocked' }[action];
    const ing = { hide: 'Hiding', show: 'Showing', lock: 'Locking', unlock: 'Unlocking' }[action];
    const stopKey = e => { if (e.key === 'Escape') this.cancel = true; };
    const live = () => !this.cancel && gen === this.gen;
    document.addEventListener('keydown', stopKey, true);
    let n = 0, pages = 0;
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
            await fwait(() => li.classList.contains('selected'), 4000);
            // Wait for the page's visuals to settle: the same count twice, 400 ms apart.
            let last = -1;
            for (let k = 0; k < 15; k++) { await fsleep(400); const c = pbt.page.visuals().length; if (c && c === last) break; last = c; }
          }
          const name = (li.querySelector('.textLabel') || li).textContent.trim();
          n += await this.onPage(action, (i, of, k) => pbt.toast(`${ing} visual filters: page ${p + 1} of ${tabs.length} (${name}), visual ${i} of ${of}, ${n + k} so far. Esc stops.`, { title: 'Filter map', ms: 1500 }), live);
          this.deselect();
          pages++;
        }
        const back = this.tabs()[start];
        if (back && !back.classList.contains('selected')) { (back.querySelector('[role="tab"]') || back).click(); await fwait(() => back.classList.contains('selected'), 4000); await fsleep(800); }
      }
      pbt.toast(`${verb} ${n} visual-level filter${n === 1 ? '' : 's'}${scope === 'report' ? ` on ${pages} page${pages === 1 ? '' : 's'}` : ''}${this.cancel ? ' (stopped)' : ''}. Save to keep it.`, { title: 'Filter map', ms: 6000, tone: this.cancel ? 'warn' : 'info' });
      return n;
    } finally {
      document.removeEventListener('keydown', stopKey, true);
      if (this.on) setTimeout(() => this.refreshPane(), 300);
    }
  },

  // The same menu in the Filters pane, beside the "Filters on this visual" caption (or "Filters
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
    if (!this.on) { this.clear(); this.result = null; pbt.toast('Filter map off'); return; }
    this.scan(true);
  },
  start() {
    pbt.hotkey('f', 'Filter map', () => this.toggle(), {
      id: 'filters',
      tool: true, active: () => this.on,
      desc: 'A badge on every visual: its visual, page and report filters and what else filters it (slicers, cross-filters). Hover a badge for the list; hide or lock visual filters on a visual, a page or every page.',
      tip: 'Filter map\nReads visual filters from the report project folder and the Filters pane, and slicers and cross-filters from Power BI\'s own list for each visual, out of sight; it updates itself when a filter changes.',
    });
    // Updates by themselves: a change in the Filters pane (a filter added, edited, hidden, a visual
    // selected) refreshes from the pane at once; a real click or key on the canvas (a slicer pick,
    // a cross-filter) refreshes Power BI's lists a moment later. Synthetic events (this tool's own)
 )PBT"
LR"PBT(   // are ignored.
    let paneTimer = 0, canvasTimer = 0;
    // The pane is always read (the selected visual's cards are kept even during a pass); the
    // redraw waits for a running pass, which redraws when it ends.
    const paneObserver = new MutationObserver(() => { clearTimeout(paneTimer); paneTimer = setTimeout(() => this.refreshPane(), 250); });
    pbt.cleanups.push(() => paneObserver.disconnect());
    const canvasChange = e => {
      // Canvas only: a change in the Filters pane is read by the pane watcher, and a pass started
      // from a pane click would land while its dropdowns are in use.
      if (!this.on || !e.isTrusted || !e.target || !e.target.closest || e.target.closest('[data-pbt]') || !e.target.closest('.displayArea')) return;
      clearTimeout(canvasTimer);
      canvasTimer = setTimeout(() => this.refreshPopups(), 900);
    };
    pbt.on(document, 'visibilitychange', () => { if (!document.hidden && this.stale && this.on) { this.stale = false; this.refreshPopups(); } });
    pbt.on(document, 'pointerup', canvasChange, true);
    pbt.on(document, 'keyup', canvasChange, true);
    // Keep the overlay on its visuals while the page moves, rescan after a page change, and keep
    // the pane button in place (the pane re-renders its sections often).
    pbt.every(500, () => {
      this.inject();
      const pane = document.querySelector('.outspacePane');
      if (pane && pane !== this.observed) { paneObserver.disconnect(); paneObserver.observe(pane, { subtree: true, childList: true, characterData: true, attributes: true, attributeFilter: ['aria-label', 'class'] }); this.observed = pane; }
      if (!this.on || this.busy) return;
      const sig = this.layoutSig();
      if (!this.result || !sig) return;
      const pageNow = (document.querySelector('.carouselScrollPane li.section.selected') || {}).textContent;
      if (pageNow !== this.pageName) { this.pageName = pageNow; if (this.result.rows.length) { this.scan(false); return; } }
      if (sig !== this.sig) { this.sig = sig; this.compose(); this.draw(); }
    });
  },
  // Stopping (the injector removed or reloaded) ends running work: the generation changes, queued
  // operations skip, loops return at their next check, and bulk runs see cancel.
  stop() {
    this.gen++;
    this.cancel = true;
    this.on = false;
    clearTimeout(this.retryTimer);
    this.quiet(false);
    this.clear();
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

const CHOICES = [
  { id: 'none', aria: /^none$/i, label: 'None', verb: 'No interaction', tip: 'The selected visual does not filter or highlight it.' },
  { id: 'filter', aria: /^filter$/i, label: 'Filter', verb: 'Filter', tip: 'The selected visual filters it.' },
  { id: 'highlight', aria: /^highlight$/i, label: 'Highlight', verb: 'Highlight', tip: 'The selected visual highlights its matching part (charts only).' },
];

const interactions = {
  name: 'interactions',
  host: null,
  sig: '',
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
        choices.push({ id: choice.id, glyph: g, box: c, active: /filled\b/.test(String(g.className)) });
      }
      if (choices.length) targets.push({ v, choices, active: (choices.find(c => c.active) || {}).id || '' });
    }
    if (!targets.length) return null;
    const source = vs.find(v => v.el.classList.contains('selected')) || null;
    return { source, targets };
  },

  // Sets every target to one choice by clicking its icon; returns how many changed.
  async setAll(id) {
    const st = this.state();
    if (!st) return 0;
    let n = 0;
    for (const t of st.targets) {
      const c = t.choices.find(x => x.id === id);
      if (!c || c.active) continue;
      c.glyph.click();
      n++;
      await new Promise(r => setTimeout(r, 60));
    }
    const choice = CHOICES.find(c => c.id === id);
    pbt.toast(n ? `${choice.verb}: ${n} visual${n === 1 ? '' : 's'} changed` : `Every visual already set to ${choice.label}`, { title: 'Interactions' });
    this.sig = '';
    setTimeout(() => this.draw(), 250);
    return n;
  },

  clear() { if (this.host) { this.host.remove(); this.host = null; } this.sig = ''; },
  draw() {
    const st = this.on ? this.state() : null;
    document.documentElement.classList.toggle('pbt-ia', !!st);
    if (!st || !st.source) { this.clear(); return; }
    const g = pbt.page.geometry();
    if (!g) { this.clear(); return; })PBT"
LR"PBT(
    const counts = { none: 0, filter: 0, highlight: 0 };
    for (const t of st.targets) if (counts[t.active] !== undefined) counts[t.active]++;
    const hasHighlight = st.targets.some(t => t.choices.some(c => c.id === 'highlight'));
    const s = st.source;
    const sig = JSON.stringify([g.rect.left, g.rect.top, g.scale, s.x, s.y, s.w, s.h, counts, hasHighlight, st.targets.length]);
    if (sig === this.sig && this.host && this.host.isConnected) return;
    this.sig = sig;
    this.clear();
    const el = pbt.el;
    const host = this.host = el('div', { class: 'pbt-ia-host', 'data-pbt': '', style: `left:${g.rect.left}px;top:${g.rect.top}px;width:${g.rect.width}px;height:${g.rect.height}px` });
    const swallow = e => { e.stopPropagation(); e.preventDefault(); };
    // Highlight always shows; with no target that offers it (cards, slicers, tables) it is
    // greyed (aria-disabled, so its tooltip still answers the pointer) and says why.
    const button = (c, off) => pbt.tip(el('button', { class: `pbt-ia-btn pbt-ia-${c.id}`, dataset: { set: c.id }, 'aria-disabled': off ? 'true' : null, onpointerdown: swallow, onclick: e => { e.stopPropagation(); if (!off) this.setAll(c.id); } },
      el('span', { class: 'pbt-ia-dot' }), el('span', { text: `${c.label} for all` })),
      off ? `${c.verb} for every other visual\nNo visual on this page offers Highlight (only charts do); its targets can be set to None or Filter.`
        : `${c.verb} for every other visual\n${c.tip} Clicks each visual's ${c.label} icon, the same as doing it by hand; Ctrl+Z undoes them one at a time.`);
    const total = st.targets.length;
    const summary = pbt.tip(el('span', { class: 'pbt-ia-count', text: `${counts.none} of ${total} set to None` }),
      `Interactions from ${s.title || s.kind}\nNone ${counts.none} \u00b7 Filter ${counts.filter}${hasHighlight ? ` \u00b7 Highlight ${counts.highlight}` : ''} of ${total} visuals. The active icon on each visual is colored: None red, Filter blue, Highlight green.`);
    const bar = el('div', { class: 'pbt-ia-bar' }, button(CHOICES[0]), button(CHOICES[1]), button(CHOICES[2], !hasHighlight), summary);
    // Above the selected visual, inside the page; below its top edge when there is no room above.
    const top = s.y * g.scale - 34;
    bar.style.left = `${Math.max(0, s.x * g.scale)}px`;
    bar.style.top = `${top >= 0 ? top : s.y * g.scale + 4}px`;
    host.append(bar);
    document.body.append(host);
  },

  toggle() {
    pbt.setFeature('interactions', !this.on);
    this.sig = '';
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
  stop() { this.clear(); document.documentElement.classList.remove('pbt-ia'); },
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
  { id: 'bar', label: 'Bar chart', visual: 'clusteredBarChart', glyph: 'pbi-glyph-barchart', w: 400, h: 280 },
  { id: 'line', label: 'Line chart', visual: 'lineChart', glyph: 'pbi-glyph-chart', w: 400, h: 280 },
  { id: 'table', label: 'Table', visual: 'tableEx', glyph: 'pbi-glyph-table', w: 400, h: 280 },
  { id: 'slicer', label: 'Slicer', visual: 'slicer', glyph: 'pbi-glyph-checkbox', w: 200, h: 100 },
  { id: 'title', label: 'Title', visual: 'textbox', glyph: 'pbi-glyph-title', w: 600, h: 48, text: { fontSize: '20pt', fontWeight: 'bold' } },
  { id: 'text', label: 'Text', visual: 'textbox', glyph: 'pbi-glyph-inserttextbox', w: 300, h: 80, text: { fontSize: '11pt' } },
];
const ST_TYPE = Object.fromEntries(ST_TYPES.map(t => [t.id, t]));
const ST_SCHEMA = 'https://developer.microsoft.com/json-schemas/fabric/item/report/definition/visualContainer/2.5.0/schema.json';
const ST_PAGE_SCHEMA = 'https://developer.microsoft.com/json-schemas/fabric/item/report/definition/page/2.0.0/schema.json';
// The only files a build may write, relative to the .Report folder (the injector and the build
// script check the same rule).
const ST_PATH = /^definition\/pages\/[A-Za-z0-9_-]{1,64}\/(visuals\/[A-Za-z0-9_-]{1,64}\/visual\.json|page\.json)$/;
const ST_MAX = 200;
// Starter layouts, drawn for a 1280 x 720 page on an 8 px grid )PBT"
LR"PBT((32 px margins, 16 px gaps, a title
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
  types: ST_TYPES,

  // ---- pure core (fixtures in scripts/tool-check.mjs) ----
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
    if (/card|kpi|multi.?row/.test(k)) return )PBT"
LR"PBT('card';
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
  // A paste-ready PowerShell script that writes a build (Windows PowerShell 5.1 or 7).
  script(root, build) {
    // The project folder travels inside the base64 data, never as PowerShell source: a folder name
    // can hold a typographic quote (U+2018 to U+201B), which PowerShell also reads as a quote.
    const data = btoa(unescape(encodeURIComponent(JSON.stringify({ files: build.files, appendPage: build.appendPage, root }))));
    const visuals = build.files.filter(f => f.path.endsWith('/visual.json')).length;
    return [
      `# Stencil build: ${visuals} placeholder visual${visuals === 1 ? '' : 's'}${build.appendPage ? ' on a new page' : ''} for ${root}`,
      '# Writes only new files under definition\\pages (never one that exists); pages.json is backed up before a new page joins it.',
      '# Everything runs as one block, so a refusal stops it all even when pasted line by line into a console.',
      '& {',
      "$ErrorActionPreference = 'Stop'",
      `$b = ConvertFrom-Json ([Text.Encoding]::UTF8.GetString([Convert]::FromBase64String('${data}')))`,
      '$root = $b.root',
      '$enc = New-Object Text.UTF8Encoding $false',
      `$ok = '${ST_PATH.source}'`,
      "$pj = Join-Path $root 'definition/pages/pages.json'",
      'if (-not (Test-Path -LiteralPath $pj)) { throw "Not a report folder (no definition/pages/pages.json): $root" }',
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
      'foreach ($f in $b.files) {',
      '  $p = Join-Path $root $f.path',
      '  [void](New-Item -ItemType Directory -Force -Path (Split-Path $p))',
      '  [IO.File]::WriteAllText($p, $f.text, $enc)',
      '}',
      'if ($b.appendPage) {',
      "  $bk = Join-Path (Split-Path $root) '.stencil-backup'",
      '  [void](New-Item -ItemType Directory -Force -Path $bk)',
      "  Copy-Item -LiteralPath $pj (Join-Path $bk ('pages-' + (Get-Date -Format 'yyyyMMdd-HHmmss') + '.json'))",
      '  $m = [IO.File]::ReadAllText($pj) | ConvertFrom-Json',
      '  $m.pageOrder = @($m.pageOrder) + $b.appendPage',
      '  [IO.File]::WriteAllText($pj, ($m | ConvertTo-Json -Depth 10), $enc)',
      '}',
      '"Built $($b.files.Count) file(s) under $root. In Power BI, click Apply extern)PBT"
LR"PBT(al changes in the bar at the top (or reopen the report) to see them."',
      '}',
      '',
    ].join('\r\n');
  },
  // The stencil as an SVG at report size (blocks as labelled boxes), for Figma and the like.
  svg(blocks, w, h) {
    const esc = s => String(s).replace(/[&<>"]/g, c => ({ '&': '&amp;', '<': '&lt;', '>': '&gt;', '"': '&quot;' }[c]));
    const rects = blocks.map(b => `  <g><rect x="${b.x}" y="${b.y}" width="${b.w}" height="${b.h}" rx="4" fill="#e8f1fb" stroke="#4a7fc1" stroke-dasharray="6 4"/><text x="${b.x + 8}" y="${b.y + 18}" font-family="Segoe UI, sans-serif" font-size="12" fill="#1f3b5c">${esc(b.label || ST_TYPE[b.type].label)}</text><text x="${b.x + 8}" y="${b.y + 34}" font-family="Segoe UI, sans-serif" font-size="10" fill="#5b7390">${esc(ST_TYPE[b.type].label)} \u00b7 ${b.w} \u00d7 ${b.h}</text></g>`);
    return `<svg xmlns="http://www.w3.org/2000/svg" width="${w}" height="${h}" viewBox="0 0 ${w} ${h}">\n  <rect width="${w}" height="${h}" fill="#ffffff"/>\n${rects.join('\n')}\n</svg>\n`;
  },

  presets: ST_PRESETS,
  // A layout's blocks on a page of w x h: scaled from 1280 x 720, every edge on the grid, inside the
  // page (pure, for fixtures).
  presetBlocks(preset, w, h, grid, newId = (i => `p${i}`)) {
    const sx = w / 1280, sy = h / 720, g = Math.max(1, grid || 1);
    const snap = v => Math.round(v / g) * g;
    return preset.blocks.map(([type, label, x, y, bw, bh], i) => {
      const x0 = Math.max(0, snap(x * sx)), y0 = Math.max(0, snap(y * sy));
      const x1 = Math.min(w, Math.max(x0 + g, snap((x + bw) * sx))), y1 = Math.min(h, Math.max(y0 + g, snap((y + bh) * sy)));
      return { id: newId(i), type, label, x: x0, y: y0, w: x1 - x0, h: y1 - y0 };
    });
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
  applyPreset(id) {
    const preset = ST_PRESETS.find(p => p.id === id), g = this.g;
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
    if (p) return p.root;
    return this.reportName() || 'report';
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
    if (list.length) page[pk] = list.map(b => ({ id: b.id, type: b.type, label: b.label, x: b.x, y: b.y, w: b.w, h: b.h }));
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
  ne)PBT"
LR"PBT(wId() { return `b${Date.now().toString(36)}${Math.floor(Math.random() * 1e6).toString(36)}`; },
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
  lines(g, skipId) {
    const xs = [0, g.w], ys = [0, g.h];
    for (const v of pbt.page.visuals(g)) { xs.push(v.x, v.x + v.w); ys.push(v.y, v.y + v.h); }
    for (const b of this.blocks()) if (b.id !== skipId) { xs.push(b.x, b.x + b.w); ys.push(b.y, b.y + b.h); }
    return { xs, ys };
  },
  // Blocks with a real visual of the same type at the same place: built. The type reads the same
  // way "Copy page into stencil" does, so a copied donut is built by the donut under it.
  sameType(a, b) { return a === b || (a === 'title' && b === 'text') || (a === 'text' && b === 'title'); },
  builtIds(g, list) {
    const vs = pbt.page.visuals(g);
    return new Set(list.filter(b => vs.some(v => Math.abs(v.x - b.x) <= 1 && Math.abs(v.y - b.y) <= 1 && Math.abs(v.w - b.w) <= 1 && Math.abs(v.h - b.h) <= 1 && this.sameType(this.kindToType(v.kind), b.type))).map(b => b.id));
  },
  // The same against the saved files: visuals a build wrote that the page does not show until the
  // report reopens.
  savedIds(page, list) {
    const vs = (page && page.visuals) || [];
    return new Set(list.filter(b => vs.some(v => Math.abs(v.x - b.x) <= 1 && Math.abs(v.y - b.y) <= 1 && Math.abs(v.w - b.w) <= 1 && Math.abs(v.h - b.h) <= 1 && this.sameType(this.kindToType(v.type), b.type))).map(b => b.id));
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
      if (b) this.drag = { kind: 'move', p0: p, r0: { ...b }, before, lines: this.lines(g, b.id) };
    } else {
      this.sel = null;
      const grid = free ? 1 : this.grid();
      const lines = this.lines(g);
      const tol = free ? 0 : grid / 2;
      this.drag = { kind: 'draw', p0: { x: this.snapTo(p.x, lines.xs, grid, tol), y: this.snapTo(p.y, lines.ys, grid, tol) }, before, lines };
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
    if (d.kind === 'move') {
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
    if (d.kind === 'draw' )PBT"
LR"PBT(&& g) {
      const t = ST_TYPE[this.type];
      const min = Math.max(this.grid(), 8);
      // A click places the type's usual size; a drag draws the box.
      const r = d.rect && d.rect.w >= min && d.rect.h >= min ? d.rect : { x: Math.min(d.p0.x, Math.max(0, g.w - t.w)), y: Math.min(d.p0.y, Math.max(0, g.h - t.h)), w: Math.min(t.w, g.w), h: Math.min(t.h, g.h) };
      const b = { id: this.newId(), type: this.type, label: this.defaultLabel(this.type, list), ...r };
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
    else if (b && (k === 'Delete' || k === 'Backspace')) { this.sel = null; this.commit(list.filter(x => x !== b)); }
    else if (b && ctrl && (k === 'd' || k === 'D')) {
      const g = this.g, step = this.grid();
      const c = { ...b, id: this.newId(), label: this.defaultLabel(b.type, list), x: Math.min(b.x + step, g ? g.w - b.w : b.x), y: Math.min(b.y + step, g ? g.h - b.h : b.y) };
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
  pick(type) { this.type = type; if (this.mode !== 'edit') this.setMode('edit'); else { this.sig = ''; this.draw(); } },
  copyPage() {
    const g = this.g;
    if (!g) return;
    const list = this.blocks();
    const add = pbt.page.visuals(g).filter(v => !list.some(b => b.x === v.x && b.y === v.y && b.w === v.w && b.h === v.h)).map(v => {
      let type = this.kindToType(v.kind);
      if (type === 'text' && v.h <= 80 && v.y < 120) type = 'title';
      return { id: this.newId(), type, label: v.title || v.kind || ST_TYPE[type].label, x: v.x, y: v.y, w: v.w, h: v.h };
    });
    if (!add.length) { pbt.toast('Every visual on this page is already a block', { title: 'Stencil' }); return; }
    this.commit([...list, ...add]);
    pbt.toast(`${add.length} visual${add.length === 1 ? '' : 's'} copied into the stencil`, { title: 'Stencil' });
  },
  clear(builtOnly) {
    const g = this.g, list = this.blocks();
    const built = g ? this.builtIds(g, list) : new Set();
    const keep = builtOnly ? list.filter(b => !built.has(b.id)) : [];
    this.sel = null;
    this.commit(keep);
    pbt.toast(builtOnly ? `${list.length - keep.length} built block${list.length - keep.length === 1 ? '' : 's'} cleared` : 'Stencil cleared for this page; Ctrl+Z in edit mode brings it back', { title: 'Stencil' });
  },
  // The build for this page or a new one, or { error } when it cannot be built.
  plan(newPage) {
    const project = this.project();
    const list = this.blocks();
    const g = this.g;
    if (!project) {
      const p = pbt.project && pbt.project.get();
      if (p) return { error: `This window shows ${this.reportName()}, but the report project folder is ${p.root.split(/[\\/]/).pop()}, so building here would write into another report.` };
      return { error: 'No report project folder is set, so there is nowhere to write. Set "project" (development) or the mod\'s Report project folder setting.' };
    }
    if (!list.length) return { error: 'Nothing to build: draw some blocks first.' };
    const page = this.projectPage();
    if (!newPage && !page) return { error: 'This page is not in the saved project files yet. Save the report in Power BI (and restart it with the mod), or build as a new page.' };
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
      const f = pbt.project.raw.)PBT"
LR"PBT(files.find(x => x.path.startsWith(`definition/pages/${page.name}/visuals/`) && x.path.endsWith('/visual.json'));
      const m = f && /"\$schema"\s*:\s*"([^"]*visualContainer[^"]*)"/.exec(f.text);
      if (m) schema = m[1];
    }
    // "Sales (stencil)" built again is "Sales (stencil) 2", not "Sales (stencil) (stencil)".
    const display = ((page && page.displayName) || this.tabName() || 'Page').replace(/ \(stencil\)( \d+)?$/, '');
    try {
      const build = this.files({ project, page: page && page.name, newPage: newPage ? { displayName: `${display} (stencil)`, width: g ? g.w : 1280, height: g ? g.h : 720 } : null, blocks: todo, schema });
      return { ...build, root: project.root, count: todo.length, ids: todo.map(b => b.id) };
    } catch (err) { return { error: err.message }; }
  },
  build(newPage, given) {
    const b = given || this.plan(newPage);
    if (b.error) { pbt.toast(b.error, { title: 'Stencil', tone: 'bad', ms: 7000 }); return false; }
    if (typeof window.pbtBuild !== 'function') return this.copyScript(newPage);
    if (this.building) { pbt.toast('A build is still being written; wait for its answer', { title: 'Stencil' }); return false; }
    this.building = true;
    clearTimeout(this.buildTimer);
    this.buildTimer = setTimeout(() => { this.building = false; }, 60000);
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
  copyScript(newPage) {
    const b = this.plan(newPage);
    if (b.error) { pbt.toast(b.error, { title: 'Stencil', tone: 'bad', ms: 7000 }); return false; }
    pbt.copy(this.script(b.root, b), 'Build script copied. Paste it into PowerShell; then, in Power BI, click Apply external changes in the bar at the top.');
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
    const close = () => { m.remove(); document.removeEventListener('pointerdown', outside, true); };
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
    if (confirm === 'presets') {
      const tiles = ST_PRESETS.map(p => {
        const b = el('button', { class: 'pbt-st-tile', role: 'menuitem', dataset: { preset: p.id }, onpointerdown: e => { e.stopPropagation(); e.preventDefault(); }, onclick: e => { e.stopPropagation(); close(); this.applyPreset(p.id); } },
          this.thumb(p), el('span', { text: p.name }));
        items.push(b);
        return pbt.tip(b, `${p.name}\n${p.desc} ${p.blocks.length} blocks, scaled to this page and placed on the grid.${list.length ? `\nReplaces the ${list.length} block${list.length === 1 ? '' : 's'} on this page; Ctrl+Z in edit mode brings them back.` : ''}`);
      });
      body = [
        el('div', { class: 'pbt-fm-menu-head', text: 'Start from a layout' }),
        el('div', { class: 'pbt-st-tiles' }, ...tiles),
        sep(),
        item('pbi-glyph-pageleft', 'Back', () => this.menu(anchor), 'Back\nThe Build menu.'),
      ];
    } else if (confirm) {
      const newPage = confirm === 'new';
      body = [
        el('div)PBT"
LR"PBT(', { class: 'pbt-fm-menu-head', text: 'Is the report saved?' }),
        el('div', { class: 'pbt-st-confirm', text: 'Building writes new files into the project and reloads the report from them. Unsaved changes in Power BI would be lost, so save first (Ctrl+S).' }),
        item('pbi-glyph-checkmark', newPage ? 'Saved: build as a new page' : 'Saved: build on this page', () => { close(); this.build(newPage); }, 'Build now\nWrites one new visual.json per block; existing files are never changed.'),
        item('pbi-glyph-cancel', 'Cancel', () => close(), 'Cancel\nNothing is written.'),
      ];
    } else {
      const noProject = !project ? '\nNeeds the report project folder (development: "project" in tweaks.json; the mod: the Report project folder setting).' : '';
      body = [
        el('div', { class: 'pbt-fm-menu-head', text: 'Stencil' }),
        item('pbi-glyph-fittopage', 'Start from a layout', () => this.menu(anchor, 'presets'), `Start from a layout\n${ST_PRESETS.length} starter layouts (KPI rows, slicer sidebars, chart grids, detail tables and more), scaled to this page; adjust them, then build.`),
        sep(),
        dev ? item('pbi-glyph-add', 'Build on this page', () => this.menu(anchor, 'page'), `Build on this page\nEach block not built yet becomes a real visual with no fields, titled with its label, at its exact position.${noProject}`, !project || !list.length) : null,
        dev ? item('pbi-glyph-page', 'Build as a new page', () => this.menu(anchor, 'new'), `Build as a new page\nThe same visuals on a new page named after this one; pages.json is backed up first.${noProject}`, !project || !list.length) : null,
        item('pbi-glyph-code', 'Copy build script: this page', () => { close(); this.copyScript(false); }, `Copy build script for this page\nA PowerShell script that writes the visuals; paste it into PowerShell with the report saved, then click Apply external changes in the bar Power BI shows.${noProject}`, !project || !list.length),
        item('pbi-glyph-code', 'Copy build script: new page', () => { close(); this.copyScript(true); }, `Copy build script for a new page\nThe same, onto a new page named after this one.${noProject}`, !project || !list.length),
        sep(),
        item('pbi-glyph-download', 'Copy page into stencil', () => { close(); this.copyPage(); }, 'Copy page into stencil\nEvery visual on this page becomes a block at its position, to redesign from.'),
        item('pbi-glyph-copy', 'Copy as SVG', () => { close(); this.copySvg(); }, 'Copy as SVG\nThe blocks as labelled boxes at report size, to paste into Figma or another design tool.', !list.length),
        sep(),
        item('pbi-glyph-checkmark', 'Clear built blocks', () => { close(); this.clear(true); }, 'Clear built blocks\nRemoves the blocks whose visual now sits on the page.', !list.length),
        item('pbi-glyph-eraser', 'Clear this page\'s stencil', () => { close(); this.clear(false); }, 'Clear this page\'s stencil\nRemoves every block on this page; Ctrl+Z in edit mode brings them back.', !list.length),
      ];
    }
    const m = el('div', { class: 'pbt-fm-menu pbt-st-menu', 'data-pbt': '', role: 'menu', 'aria-label': 'Stencil',
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
    const palette = ST_TYPES.map(t => button({ class: `pbt-strip-grid pbt-st-type${this.type === t.id && edit ? ' pbt-on' : ''}`, dataset: { type: t.id }, 'aria-label': t.label }, () => this.pick(t.id),
      `${t.label}\nDraw a ${t.label.toLowerCase()} block: drag on the page, or click for the usual ${t.w} \u00d7 ${t.h}. Builds as ${t.visual === 'textbox' ? 'a text box' : `a ${t.visual} visual`} with no fields.`,
      el('span', { class: `glyphicon glyph-small ${t.glyph}`, 'aria-hidden': 'true' })));
    const b = list.find(x => x.id === this.sel);
    let fields = null;
    if (b && edit) {
      const num = (key, label) => {
        const i = el('input', { class: 'pbt-st-num', type: 'number', step: '1', 'aria-label': label, value: String(b[key]) });
        i.addEventListener('keydown', e => { e.stopPropagation(); if (e.key === 'Enter') i.blur(); if (e.key === 'Escape') { i.value = String(b[key]); i.blur(); } });
        i.addEventListener('change', () => { const v = Math.round(Number(i.value)); if (!Number.isFinite(v)) return; const l = this.blocks(), x = l.find(y => y.id === b.id); if (!x) return; x[key] = Math.max(key === 'w' || key === 'h' ? 8 : 0, v); this.commit(l); });
        return pbt.tip(el('label', { class: 'pbt-st-field' }, el('span', { text: label }), i), `${label === 'w' ? 'Width' : label === 'h' ? 'Height' : label} in report pixels\nType a value and press Enter.`);
      };
      const name = el('input', { class: 'pbt-st-label-inp)PBT"
LR"PBT(ut', type: 'text', 'aria-label': 'Label', value: b.label || '' });
      name.addEventListener('keydown', e => { e.stopPropagation(); if (e.key === 'Enter') name.blur(); if (e.key === 'Escape') { name.value = b.label || ''; name.blur(); } });
      name.addEventListener('change', () => { const l = this.blocks(), x = l.find(y => y.id === b.id); if (!x) return; x.label = name.value.trim() || this.defaultLabel(x.type, l); this.commit(l); });
      fields = el('span', { class: 'pbt-strip-seg pbt-st-fields' }, pbt.tip(name, 'Label\nThe visual\'s title when built (a text box\'s text). Double-click a block to edit it.'), num('x', 'x'), num('y', 'y'), num('w', 'w'), num('h', 'h'));
    }
    const count = pbt.tip(el('span', { class: 'pbt-strip-item' }, el('b', { text: String(list.length) }), el('span', { text: `block${list.length === 1 ? '' : 's'}${built.size ? ` \u00b7 ${built.size} built` : ''}` })),
      `Stencil blocks on this page\nSaved with your choices, per report and page.${built.size ? ` ${built.size} already have their visual on the page (a check mark).` : ''}`);
    const modeBtn = button({ class: `pbt-strip-grid pbt-st-mode${edit ? ' pbt-on' : ''}`, text: edit ? 'Done' : 'Edit' }, () => this.setMode(edit ? 'view' : 'edit'),
      edit ? 'Done editing (Esc)\nThe stencil stays visible and every click goes to Power BI again.' : 'Edit the stencil\nWhile editing, the page takes no clicks: drag to draw, drag a block to move it, its handles to resize. Alt snaps to 1 px; Esc ends.');
    const menuBtn = button({ class: 'pbt-strip-grid pbt-st-menu-btn', text: 'Build \u25be' }, e => this.menu(e.currentTarget), 'Build, copy and clear\nBuild the blocks as real visuals, copy a build script or an SVG, copy the page into the stencil, clear blocks.');
    const helpBtn = button({ class: 'pbt-strip-help', text: '?' }, () => { if (pbt.help) { pbt.help.close(); pbt.help.open(); pbt.help.show('stencil'); } }, 'How the stencil works\nOpens the help page at Stencil.');
    const bar = el('div', { class: 'pbt-qa-strip pbt-st-strip' },
      pbt.tip(el('span', { class: 'pbt-strip-item pbt-st-name', text: 'Stencil' }), 'Stencil (Ctrl+Alt+S)\nA mock-up layer on this page: draw blocks in Edit, build them as real visuals from the Build menu.'), count,
      el('span', { class: 'pbt-strip-seg pbt-st-palette' }, ...palette),
      // The picked type in words beside its icon (the type the next block drawn takes).
      el('span', { class: `pbt-st-typename${edit ? '' : ' pbt-st-dim'}`, text: (ST_TYPE[this.type] || ST_TYPE.card).label }),
      fields, modeBtn, menuBtn, helpBtn);
    pbt.placeStrip(bar, 'stencil', g);
    return bar;
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
    const sig = JSON.stringify([g.rect.left, g.rect.top, g.rect.width, g.rect.height, g.w, g.h, empty, this.mode, this.type, this.sel, list, [...built], d && d.rect, grid, pbt.stripOrder.filter(n => (pbt.stripTools()[n] || {}).on)]);
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
    for (const b of list) {
      const t = ST_TYPE[b.type] || ST_TYPE.card;
      const sel = this.mode === 'edit' && b.id === this.sel;
      const node = el('div', { class: `pbt-st-block${sel ? ' pbt-st-sel' : ''}${built.has(b.id) ? ' pbt-st-built' : ''}`, dataset: { id: b.id, type: b.type }, style: `left:${px(b.x)};top:${px(b.y)};width:${px(b.w)};height:${px(b.h)}` },
        // A built block's visual says what it is; the block keeps only a check mark in its corner.
        built.has(b.id) ? el('span', { class: 'pbt-st-tag pbt-st-tag-built' }, el('span', { class: 'pbt-st-check', text: '\u2713' }))
          : el('span', { class: 'pbt-st-tag' }, el('span', { class: `glyphicon glyph-small ${t.glyph}`, 'aria-hidden': 'true' }), el('span', { text: b.label || t.label })),
        sel || (d && d.r0 && d.r0.id === b.id) ? el('span', { class: 'pbt-st-size', text: `${Math.round(b.x)}, ${Math.round(b.y)} \u00b7 ${Math.round(b.w)} \u00d7 ${Math.round(b.h)}` }) : null,
        ...(sel ? ['n', 'ne', 'e', 'se', 's', 'sw', 'w', 'nw'].map(h => el('span', { class: `pbt-st-h pbt-st-h-${h}`, dataset: { h } })) : []));
      kids.push(node);
    }
    if (d && d.kind === 'draw' && d.rect) kids.push(el('div', { class: 'pbt-st-block pbt-st-new', style: `left:${px(d.rect.x)};top:${px(d.rect.y)};width:${px(d.rect.w)};height:${px(d.rect.h)}` },
      el('span', { class: 'pbt-st-size', text: `${Math.round(d.rect.w)} \u00d7 ${Math.round(d.rect.h))PBT"
LR"PBT(}` })));
    // No blocks yet: one line on the page, just above the dock, says how to start. The dock sits
    // below the page when there is room, else inside its bottom edge; the hint stays above it.
    const dock = document.querySelector('.pbt-dock'), dockTop = dock ? dock.getBoundingClientRect().top : Infinity;
    const lift = dockTop < g.rect.top + g.rect.height ? g.rect.top + g.rect.height - dockTop + 8 : 8;
    if (!list.length && !(d && d.kind === 'draw')) kids.push(el('div', { class: 'pbt-st-hint', style: `bottom:${lift}px` }, this.mode === 'edit'
      ? `Drag on the page to draw a ${(ST_TYPE[this.type] || ST_TYPE.card).label.toLowerCase()} block, or click for its usual size. Pick another type in the strip below.`
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
    pbt.every(300, () => { if (this.on && !this.drag && !(document.activeElement && document.activeElement.tagName === 'INPUT' && document.activeElement.closest('.pbt-st-strip'))) this.draw(); });
  },
  stop() { this.clearHost(); document.querySelectorAll('.pbt-st-menu').forEach(m => m.remove()); },
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
    const tc = t)PBT"
LR"PBT(.textClasses || {};
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
          if (!this.same(v, want)) rows.push({ kind: 'theme', key: k, label: label(k), value: v, want });
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
    const)PBT"
LR"PBT( found = this.majority(list);
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
    return { sev: 'ok', text: res.hasTheme ? '\u2713 theme' : '\u2713 page' };
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
    const line = (i, n) => el('div', { class: 'pbt-card-row' },
      n ? pbt.pinNum(n) : el('span', { class: `pbt-card-dot pbt-dot-${dot[i.kind]}` }),
      el('span', { text: i.kind === 'palette' ? `${i.label} ${this.fmt(i.value)}: ${i.note}.` : i.kind === 'info' ? `${i.label}: ${this.fmt(i.value)} (${i.note})` : `${i.label}: ${this.fmt(i.value)} here, ${i.kind === 'theme' ? 'theme' : 'most'} ${this.fmt(i.want)}${i.kind === 'page' ? ` (${i.note})` : ''}` }),
      i.kind === 'theme' || i.kind === 'page' ? el('span', { class: 'pbt-card-fix', text: `\u2192 ${this.fmt(i.want)}`, 'aria-label': `${i.kind === 'theme' ? 'Theme value' : 'Most of the page'}: ${this.fmt(i.want)}` }) : el('span', { class: 'pbt-card-fix' }));
    const rows = off.map((i, n) => line(i, n + 1));
    // The card shows only while its badge is hovered, so the blue values are named in the heading
    // (a tooltip on them could never be reached).
    const valueIs = vals => el('span', { class: 'pbt-card-fix', text: vals.some(i => i.kind === 'theme') ? '\u2192 theme value' : vals.some(i => i.kind === 'page') ? '\u2192 most of the page' : '' });
    if (off.length) rows.unshift(el('div', { class: 'pbt-card-sub' }, el('span', { text: 'Differences (numbered on the page)' }), valueIs(off)));
    if (!off.length) rows.push(el('div', { class: 'pbt-card-row' }, el('span', { class: 'pbt-card-dot pbt-dot-ok' }), el('span', { text: res.hasTheme ? 'Takes the theme for everything the theme sets.' : 'Matches most visuals on the page.' }), el('span')));
    if (row.missing) rows.push(el('div', { class: 'pbt-card-row' }, el('span', { class: 'pbt-card-dot pbt-dot-grid' }), el('span', { text: 'Not in the saved project files (added since the last save?); save to check it.' }), el('span')));
    if (row.stale) rows.push(el('div', { class: 'pbt-card-row' }, el('span')PBT"
LR"PBT(, { class: 'pbt-card-dot pbt-dot-grid' }), el('span', { text: 'Moved since the last save; the formatting read is the saved one.' }), el('span')));
    const infoShown = info.slice(0, 4).map(i => line(i, 0));
    if (info.length > 4) infoShown.push(el('div', { class: 'pbt-card-row' }, el('span'), el('span', { text: `and ${info.length - 4} more set on the visual without a finding` }), el('span')));
    const n = { theme: 0, palette: 0, page: quiet.length, info: info.length };
    for (const i of off) n[i.kind]++;
    const vd = this.verdict(row, res);
    return el('div', { class: 'pbt-card' },
      el('header', {}, el('b', { text: v.title || v.kind }), v.title ? el('span', { class: 'pbt-card-type', text: v.kind }) : null,
        el('span', { class: `pbt-card-badge pbt-sev-${vd.sev}`, text: vd.text })),
      el('div', { class: 'pbt-card-kpi' }, ...[['Differ from theme', n.theme], ['Off palette', n.palette], ['Differ from page', n.page], ['Set on this visual',n.info + off.length + quiet.length]].map(([k, x]) => el('div', {}, el('small', { text: k }), el('strong', { text: String(x) })))),
      el('div', { class: 'pbt-card-rows' }, ...rows,
        ...(quiet.length ? [el('div', { class: 'pbt-card-sub', text: 'Differs from most of the page (information)' }), ...quiet.map(i => line(i, 0))] : []),
        ...(infoShown.length ? [el('div', { class: 'pbt-card-sub', text: 'Also set on this visual' }), ...infoShown] : [])),
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
    const passes = pbt.passItems(res.rows.filter(r => !this.r)PBT"
LR"PBT(eportable(r, res)).length, res.rows.length, `Nothing set differently from ${this.against(res)}.`);
    const bar = el('div', { class: 'pbt-qa-strip pbt-th-strip' },
      item('grid', `${offRows.length} of ${res.rows.length}`, pageOnly ? 'differ from the page' : 'differ from theme', `Visuals that set something differently from ${pageOnly ? 'most of the page' : 'the theme file'}\nHover a visual's badge for the list.`),
      item('dist', null, res.mode === 'file' && res.hasTheme ? `theme: ${res.source}` : 'page majority', res.mode === 'file' && res.hasTheme ? 'Compared with the report theme, from the saved project files\nSave the report to check changes made since.' : res.mode === 'file' ? 'The project files have no theme file (the report uses Power BI\'s built-in theme), so each visual\'s saved formatting is compared with the rest of the page.' : 'No project folder set, so each visual is compared with what most visuals on the page draw\nSet the report project folder for a check against the theme itself.'),
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
// interactivity line is a 10 px stroke at opacity 0 in the app's white, which a skin re)PBT"
LR"PBT(colours).
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
    if (!layers.length) return { r: 255, g: 255, b: 255, a: 1 };
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
  // T)PBT"
LR"PBT(he palette color closest to a stray one (largest channel difference), for the card. A stray
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
    return { r: ch('r'), g: ch('g'), b: ch('b'), a };
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
    if (has(/legend/)) return)PBT"
LR"PBT( 'Legend';
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
    return `${count} ${name}${many ? 's' : ''}`;
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
      // The largest shape of this color that )PBT"
LR"PBT(is actually visible (an opacity of 0 anywhere up its
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
    const g = pbt.page.geometry();
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
    const note = (dot, text) => el('div', { class: 'pbt-card-row' }, el('span', { class: `pbt-card-dot pbt-dot-${dot}` }), el('span', { text }), el('span'));
    const tone = this.tone(row, cfg);
    const rows = [el('div', { class: 'pbt-card-sub', text: 'Hard to read' })];
    for (const { j, n } of hard) rows.push(el('div', { class: 'pbt-card-row' }, pbt.pinNum(n),
      el('span', { class: 'pbt-ct-pair' }, this.swatch(j.fg), this.swatch(j.bg),
        el('span', { text: j.kind === 'mark' ? `${j.role[0].toUpperCase()}${j.role.slice(1)} in ${j.text} on ${hex(j.bg)}` : `${j.role} "${j.text}"${j.large ? ' (large)' : ''} on ${hex(j.bg)}` })),
      el('span', { class: 'pbt-card-fix', text: `${j.ratio.toFixed(2)}:1 \u2192 ${j.need}:1` })));
    if (fails.length > CO_ROWS) rows.push(note('grid', `and ${fails.length - CO_ROWS} more`));
    if (!fails.length) rows.push(note(row.judged.length ? 'ok' : 'grid', row.judged.length ? `Every text${cfg.marks ? ' and bar, line and point' : ''} measured meets ${cfg.level}.` : 'Nothing measurable here (text drawn as an image or in a canvas cannot be read).'));
    if (row.skipped) rows.pus)PBT"
LR"PBT(h(note('grid', `${row.skipped} over an image, a canvas or a gradient: not checked.`));
    if (row.unseen) rows.push(note('grid', `${row.unseen} outside the window: scroll them into view to check.`));
    if (row.truncated) rows.push(note('grid', `A large visual: its first ${CT_NODES.toLocaleString()} elements were checked.`));
    rows.push(el('div', { class: 'pbt-card-sub', text: 'Off palette' }));
    for (const { hex: h, n } of off) {
      const near = this.nearest(h);
      rows.push(el('div', { class: 'pbt-card-row' }, pbt.pinNum(n),
        el('span', { class: 'pbt-ct-pair' }, this.swatch(pbt.color.parse(h)), el('span', { text: `${h} as ${[...row.colors.get(h)].join(' and ')}` })),
        el('span', { class: 'pbt-card-fix', text: near ? `nearest ${near}` : '' })));
    }
    if (row.off.length > CO_ROWS) rows.push(note('grid', `and ${row.off.length - CO_ROWS} more`));
    if (!row.off.length) rows.push(note('ok', `Every color is within ${this.tolerance()} of a palette color.`));
    if (row.fontsOff.length) rows.push(note('edge', `Fonts outside ${(pbt.cfg.lint.fonts || []).join(', ')}: ${row.fontsOff.join(', ')}`));
    const drawn = [...row.colors.keys()];
    rows.push(note('grid', `Drawn: ${drawn.length ? drawn.slice(0, 10).join(', ') + (drawn.length > 10 ? ` and ${drawn.length - 10} more` : '') : 'neutrals only'}; fonts ${[...row.fonts].join(', ') || 'none'}`));
    return el('div', { class: 'pbt-card' },
      el('header', {}, el('b', { text: v.title || v.kind }), v.title ? el('span', { class: 'pbt-card-type', text: v.kind }) : null,
        el('span', { class: `pbt-card-badge pbt-sev-${CO_SEV[tone]}`, text: this.label(row, cfg) })),
      el('div', { class: 'pbt-card-kpi' }, ...[['Hard to read', fails.length], ['Off palette', row.off.length], ['Lowest', lowest == null ? '-' : `${lowest.toFixed(1)}:1`], ['Texts', row.texts]].map(([k, x]) => el('div', {}, el('small', { text: k }), el('strong', { text: String(x) })))),
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
    // The switch's track, then its words (built h)PBT"
LR"PBT(ere so the button keeps one tooltip).
    bar.querySelector('.pbt-strip-switch').append(el('span', { class: 'pbt-switch-track' }), el('span', { text: 'bars, lines and points' }));
    // On a narrow dock the palette's name and the "level" label give way first (pbt.dock.place).
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
    pbt.hotkey('l', 'Colors', () => this.)PBT"
LR"PBT(toggle(), { ...extra, tool: true, id: 'colors' });
    // The contrast check's old key opens the same tool; only one entry in the Tools menu.
    pbt.hotkey('a', 'Colors (as Ctrl+Alt+L)', () => this.toggle(), extra);
    // Any change on the canvas marks the scan stale; the 2-second tick rescans only then. The shared
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
          ctx.fillStyle = 'rgba(255, 255, 255, .)PBT"
LR"PBT(5)'; ctx.beginPath(); ctx.moveTo(0, -p.s); ctx.lineTo(p.s / 2.4, 0); ctx.lineTo(0, 0); ctx.fill();
          break;
        case 'lambda':
          ctx.font = `bold ${p.s * 2}px Bahnschrift, sans-serif`; ctx.textAlign = 'center'; ctx.textBaseline = 'middle'; ctx.fillText('\u03bb', 0, 0);
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
    pbt.on(window, 'keydown', e => {
      at = e.key === code[at] || e.key.toLowerCase() === code[at] ? at + 1 : (e.key === code[0] ? 1 : 0);
      if (at === code.length) {
        at = 0;
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
  shownSkin() { return pbt.skin && pbt.skin.current !== undefined ? pbt.skin.current : pbt.cfg.skin; },
  build() {
    const viewOpen = document.querySelector('#tablist button#view[role="tab"][aria-selected="true"]');
    const groups = viewOpen && [...document.querySelectorAll('.ribbonContainer .bottomBarContainer [role="group"][id$="-group"]')];
    if (!groups || !groups.length) {
      // Another tab is open. The ribbon re-renders its row in place and would keep our group in
      // it (it showed up at the start of Home), so take it out.
      document.querySelectorAll('.pbt-rgroup').forEach(g => g.remove());
      this.group = null;
      return;
    }
    // Each group sits in a "groupContainer" inside one flex row; ours goes at the end)PBT"
LR"PBT( of that row.
    const lastWrap = groups[groups.length - 1].closest('[class*="groupContainer"]');
    const row = lastWrap && lastWrap.parentElement;
    if (!row) return;
    // A group left behind by an earlier copy of the script never stays beside ours.
    document.querySelectorAll('.pbt-rgroup').forEach(g => { if (g !== this.group) g.remove(); });
    const shown = this.shownSkin();
    const sig = `${shown}`;
    if (this.group && this.group.isConnected && this.group.parentElement === row && row.lastElementChild === this.group && sig === this.sig) return;
    if (this.group) this.group.remove();
    this.sig = sig;

    const btn = (id, label, sw) => pbt.tip(pbt.el('button', {
      class: 'pbt-rbtn', 'aria-pressed': String(shown === id || (id === 'off' && !pbt.skins[shown])),
      onpointerdown: e => e.stopPropagation(),
      onclick: e => { e.stopPropagation(); pbt.skin.set(id); },
    }, pbt.el('span', { class: 'pbt-sw', style: `background:${sw}` }), pbt.el('span', { text: label })), pbt.skinTip(id));

    this.group = pbt.el('div', { class: 'pbt-rgroup', 'data-pbt': '', role: 'group', 'aria-label': 'Skins' },
      pbt.el('div', { class: 'pbt-rgroup-body' },
        pbt.el('div', { class: 'pbt-rgallery' },
          btn('off', 'None', 'repeating-linear-gradient(45deg, #888 0 3px, transparent 3px 6px)'),
          pbt.skin.ids().map(id => btn(id, pbt.skins[id].short || pbt.skins[id].name, pbt.swatch(pbt.skins[id]))))),
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

    const intro = section('intro',
      p('Restyles the window around your report: title bar, ribbon, panes, page tabs, status bar and every view. Your report page and its visuals are never changed. Tools help with report layout, filters, theme and colors, and a few extras are along for fun.'),
      p('The tools, by job: Plan (Stencil), Layout (Layout QA), Check (Filter map, Theme check, Colors) and While editing (Interactions). The check tools put their badges in one row just under each visual\'s bottom right corner, each led by the tool\'s letter (F, T, C). A badge\'s color is its severity only: red to fix, amber to look at, grey for information, and green for a pass while "show passes" is on. With two or more tools on, the dock under the page has a tab per tool; a dot marks each tool that is on, and the underlined tab is the one whose strip shows.'));

    const skins = section('skins', h3('Skins', `${hk(']')} next \u00b7 ${hk('[')} previous`),
      p('Pick one from the Skins group at the end of the View ribbon, from the chip in the status bar, or with the keys. None puts Power BI back exactly as it was set. The choice is remembered and follows every open view and dialog.'),
      p('Skins look the same over Power BI\'s light and dark modes; the theme button in the title bar still switches the mode underneath.'),
      el('div', { class: 'pbt-help-skins' }, ...pbt.skin.ids().map(id => {
        const s = pbt.skins[id];
        return el('div', { class: 'pbt-help-skin' }, el('span', { class: 'pbt-sw', style: `background:${pbt.swatch(s)}` }), el('b', { text: s.name }), s.inspiredBy ? el('span', { text: ` ${s.inspiredBy}` }) : null);
      })));

    const legend = rows => el('div', { class: 'pbt-legend' }, ...rows.flatMap(([cls, text]) => [el('span', { class: cls }), el('span', { text })]));
    const qaCfg = pbt.cfg.qa || {}, gridNow = qaCfg.grid || 8, nearNow = qaCfg.nearMiss || 8;
    const qa = section('qa', h3('Layout QA', hk('g')),
      p(`A grid (${gridNow} report px, every fifth line stronger) and rulers over the page, a label on every visual with its type and x, y \u00b7 w \u00d7 h in report pixels, and a strip under the page with the counts. Positions are exact report pixels at any zoom (they match the Format pane). The overlay never takes clicks meant for the canvas and redraws as visuals move.`),
      legend([
        ['pbt-lg-box', 'A visual in order: its edges line up with its neighbours and its gaps match the page.'],
        ['pbt-lg-bad', `A visual to look at: an edge 1 to ${nearNow} px off a neighbour's matching edge, a gap that far off the page's usual gap, or it runs past the page edge.`],
        ['pbt-lg-guide', 'Where a near-missed edge belongs; the tag says which edge, how far off and where the line is ("Right edge 4 px off the line at 1248").'],
        )PBT"
LR"PBT(['pbt-lg-off', `A visual with an edge off the ${gridNow} px grid (amber, dashed), when the grid check is on.`],
        ['pbt-lg-shared', 'Shown while you hover a visual: an edge it shares with others, tagged with how many.'],
        ['pbt-lg-dist', 'Shown while you hover a visual: its gap to the nearest neighbour, or to the page edge, in each direction.'],
      ]),
      p('Hover a label for the visual\'s card: its position and size, each finding in words with the neighbours it refers to, and the value that fixes it (an arrow on x, y, width or height shows the change). Click the label to copy x, y, w, h; Shift+click copies the suggested values, ready for the Format pane\'s Properties.'),
      p('An edge counts as aligned when at least as many neighbours share it as nearly miss it, so one visual off a row is reported, not the row. Centers are compared only when neither edge on that axis lines up.'),
      p(`The strip: page size, visual count, edges off, odd gaps with the page's usual gap, how many visuals sit on the grid, a grid picker (4 to 20 px, saved as qa.grid), the grid check switch (qa.snap) and this help. qa.nearMiss (${nearNow}) sets how far off counts as a miss; pageSize pins the page size.`),
      h3('When to snap'),
      p('Snap to grid (View ribbon) keeps edges on Power BI\'s grid while you drag, and the amber marks read that back. Aim for the grid on the skeleton: cards, charts, the rows and columns they form, and the page gutter, so gaps repeat and columns line up without measuring.'),
      p('Let content-sized things sit off the grid when a pixel or two of optical alignment matters more: slicers and buttons sized to their text, text boxes, legends, a title that should line up with a chart\'s plot area rather than its frame. If a page is built on a gap that is not a grid multiple, set the grid to that gap (qa.grid) instead of fighting it; qa.snap false turns the amber marks off.'));

    const fm = section('filters', h3('Filter map', hk('f')),
      p('A grey badge on every visual with something worth knowing says what filters it, by level: a letter and a count, spelled out while the pointer is on the badge ("V1 S2" reads "1 visual \u00b7 2 from slicers & cross-filters"). Hover a badge for the list: each filter in words, and for visual-level filters whether they are hidden or locked in view mode. The strip under the page has the counts, Rescan, and the visual-filters menu.'),
      legend([
        ['pbt-lg-fvisual', 'V: visual-level filters, from the Filters pane (hidden ones included, which Power BI\'s own list for the visual leaves out). An outline on the badge marks a visual that has any.'],
        ['pbt-lg-fpage', 'P: filters on this page.'],
        ['pbt-lg-freport', 'R: filters on all pages.'],
        ['pbt-lg-fother', 'S: slicers & cross-filters: a slicer, a cross-filter or a selection in another visual, read from Power BI\'s "Filters and slicers affecting this visual".'],
      ]),
      p('How it reads them, without clicking anything you can see: page and report filters from the Filters pane; each visual\'s own filters from the report project folder (hidden and locked included), or live from the pane once you select the visual; slicers and cross-filters from Power BI\'s list for each visual, opened out of sight. It updates by itself when a filter changes in the pane or you click a slicer or a visual. Without the project folder, "Read each visual" in the strip reads the cards it cannot see otherwise by selecting each visual in turn.'),
      h3('Hide, show, lock or unlock visual filters at once'),
      p('The "Visual filters" button in the strip, or the small button beside "Filters on this visual" in the Filters pane (beside "Filters on this page" when nothing is selected), opens a menu: Hide, Show, Lock or Unlock, for the selected visual, this page or every page. It presses the same eye and lock buttons you would, one card at a time, including field-well cards; page and report filters are left alone. Esc stops a run; save the report to keep the result.'));

    const th = section('theme', h3('Theme check', hk('t')),
      p('An amber badge on every visual that sets something differently from the theme file: "3 differ from theme". Settings that differ from most of the page are grey information ("1 differs from the page"), off by default; the strip\'s "differ from the page" switch shows them on the badges (themeCheck.page). With "show passes" on, a visual that takes the theme gets "\u2713 theme". Hover a badge for the card: each setting, this visual\'s value and the one it should have, and what the visual sets on itself without a finding. While you hover, the part each finding is about is outlined and numbered on the page (the title, the axes, the columns in a color, the visual\'s box for its background or border).'),
      legend([
        ['pbt-lg-off', 'Theme: the visual sets something the theme sets, to another value (a title size of 16 where the theme says 14).'],
        ['pbt-lg-bad', 'Palette: the visual sets a color that is not one of the theme\'s colors.'],
        ['pbt-lg-guide', 'Page (information): the theme leaves it open, and most visuals on the page that set it use another value (one hidden title among shown ones).'],
      ]),
      p('With the report project folder set (Settings), each visual\'s saved formatting is compared with the theme file: the style for its visual type, then for every visual, then the title text class. Save the report to check changes made since. Without the folder, what each visual draws (title, color, weight, background, border) is compared with most of the page, and those differences always show. The strip names the setting most often off ("Title font size"); its tooltip lists the top six with their counts.'));

    const ia = section('interactions', h3('Interactions', hk('i')),
      p('Shows while Edit interactions is on (select a visual, then Format > Edit interactions). The active interaction icon on every )PBT"
LR"PBT(other visual gets a color: None red, Filter blue, Highlight green, so you can read the page at a glance.'),
      p('A bar above the selected visual sets every other visual at once: "None for all", "Filter for all" and "Highlight for all" (greyed when no visual on the page offers Highlight; only charts do), with a count of how many are set to None. It clicks each visual\'s own icon, so Ctrl+Z undoes the changes one at a time. On by default; Ctrl+Alt+I switches it off and on.'));

    const st = section('stencil', h3('Stencil', hk('s')),
      p('A see-through mock-up layer on the report page. Draw where the visuals will go, in report pixels on the Layout QA grid, then build the blocks as real visuals with no fields and drag the fields in. Blocks are saved per report and page.'),
      p('Edit (in the strip) turns the page into a drawing surface: pick a type in the strip (card, column, bar or line chart, table, slicer, title, text), drag on the page to draw a block or click for its usual size, drag a block to move it and its handles to resize it. Edges snap to the grid and to the edges of visuals and other blocks; hold Alt for 1 px steps. While editing, the page takes no clicks; Done or Esc gives them back. The x, y, width, height and label of the selected block can be typed in the strip.'),
      el('table', {}, ...[
        ['Arrows', 'Move the selected block 1 px (Shift: one grid step)'],
        ['Delete', 'Remove the selected block'],
        ['Ctrl+D', 'Duplicate it'],
        ['Ctrl+Z, Ctrl+Y', 'Undo and redo stencil changes (Power BI\'s own undo is not touched)'],
        ['Double-click', 'Rename a block (its title when built; a text box\'s text)'],
        ['Esc', 'Deselect, then leave edit mode'],
      ].map(([k, what]) => el('tr', {}, el('td', { text: k }), el('td', { text: what })))),
      p(mod
        ? 'Build: the Build menu copies a PowerShell script that writes one new visual.json per block into the report project folder (Settings); save the report first, paste the script into PowerShell, then click Apply external changes in the bar Power BI shows at the top (or reopen the report). It writes only new files under definition\\pages and backs up pages.json before adding a new page.'
        : 'Build: the Build menu writes one new visual.json per block into the report project folder ("project"), on this page or a new one, after asking whether the report is saved. Power BI then shows a bar at the top: click Apply external changes to load the new visuals (with the Power BI Desktop bridge library installed the report reloads by itself). Only new files under definition/pages are written; pages.json is backed up before a new page is added. "Copy build script" gives the same build as a PowerShell script.'),
      p('Built blocks get a check mark once their visual sits at the same place; "Clear built blocks" removes them. "Copy page into stencil" turns the page\'s visuals into blocks for a redesign, and "Copy as SVG" puts the blocks on the clipboard at report size, for Figma or another design tool.'));

    const co = section('colors', h3('Colors', `${hk('l')} or ${hk('a')}`),
      p('One badge per visual with something to fix, in plain words: "1 hard to read", "2 off palette", or both ("1 hard to read \u00b7 2 off palette"). Red when anything is hard to read, amber when the only finding is a color off the palette. Hover a badge for the card: one section per question, every finding numbered, and while you hover each numbered part is outlined on the page with the same number. Each stray color is also a swatch on the badge: point at it to outline every part drawn in it, click it to copy its hex.'),
      legend([
        ['pbt-lg-ctbad', 'Hard to read: text, or a bar, line or point, that does not stand out enough from what is behind it.'],
        ['pbt-lg-ctwarn', 'Off palette: the visual draws a color outside the palette (or a font outside the allowed families), and nothing is hard to read.'],
        ['pbt-lg-ctok', 'With "show passes" on: every text is measured and meets the level, and every color is on the palette.'],
      ]),
      h3('Hard to read'),
      p('Text is measured against the color actually behind it: the bar under a data label, the visual\'s own background, the page. AA asks for 4.5:1, or 3:1 for large text (24 px or more, or 18.66 px and bold); AAA for 7:1 and 4.5:1 (WCAG 2.1, 1.4.3). Bars, lines and points need 3:1 against the visual\'s background (1.4.11); the "bars, lines and points" switch in the strip turns that part off. AA or AAA is picked in the strip.'),
      p('Colors are read as Power BI draws them, so conditional formatting and the theme both count. Text over an image or a gradient is listed as not checked, and text a custom visual draws into a canvas cannot be read. Contrast is one part of accessibility: alt text, tab order and meaning carried by color alone are not checked here.'),
      h3('Off palette'),
      p(`Reads the colors each visual actually drew (fills, strokes, text, backgrounds) and lists those outside the palette, each with where it is used and the nearest palette color. Greys, white and black always pass. The visual header, Power BI's invisible hover targets and anything fully see-through are not read, so the verdict is the same in every skin. The palette is the report theme's colors when the report's project folder is set (its data colors plus good, neutral, bad, foreground, background and accents), else the palette in the settings; the strip names which. Now: ${pbt.colors.source()}.`),
      el('div', { class: 'pbt-help-palette' },
        el('span', { text: `The palette in the settings: ${palette.length} color${palette.length === 1 ? '' : 's'}, each channel within ${lintCfg.tolerance || 12}. ` }),
        ...palette.slice(0, 24).map(hex => pbt.tip(el('span', { class: 'pbt-sw', style: `background:${hex}` }), String(hex)))),
      p(lintCfg.fonts && lintCfg.fonts.length ? `Fonts outside ${lintCfg.fonts.join(', ')} are flagged as)PBT"
LR"PBT( well.` : 'Fonts are listed on each card and checked when the settings name the allowed families.'));

    const extras = section('extras', h3('Extras'),
      el('ul', {}, ...(pbt.featureList ? pbt.featureList() : Object.entries(pbt.FEATURES || {})).map(([f, def]) => el('li', {},
        el('b', { text: def.label }), el('span', { text: `: ${def.desc} ` }),
        def.run ? button(def.helpTry || def.tryLabel || 'Try', def.tryTip, def.run) : null,
        el('span', { class: 'pbt-state', text: pbt.feature(f) ? 'on' : 'off' }))),
        el('li', {}, el('b', { text: 'The Konami code' }), el('span', { text: ' does something to the page for two seconds.' }))),
      p('Switch each one in the menu (status bar chip or Ctrl+Alt+K).'));

    const keys = section('keys', h3('Keys'),
      el('table', {}, ...[
        ...pbt.hotkeys.map(h => [`Ctrl+Alt+${h.key.toUpperCase()}`, h.label]),
        ['Esc', 'Close the menu, this help, or a tooltip'],
      ].map(([k, what]) => el('tr', {}, el('td', { text: k }), el('td', { text: what })))));

    const settings = section('settings', h3('Settings'),
      p(mod
        ? 'Windhawk > Skins & Tools for Power BI > Settings holds the starting skin, the extras, the palette and tolerance for Colors and the report project folder (the .Report folder of a report saved as a Power BI project; read when Power BI starts and when the settings change, so save and restart Power BI to refresh it). Choices made inside Power BI are remembered until the mod\'s settings change, then the settings apply again.'
        : 'tweaks.json beside inject.mjs holds the skin, the extras (features), the Layout QA grid and threshold (qa.grid, qa.nearMiss), the palette, tolerance and fonts for Colors (lint.palette, lint.tolerance, lint.fonts, lint.fromTheme) and its contrast level and switch (contrast.level, contrast.marks), the report project folder (project, read on every reload) and pageSize. Choices made in Power BI are written back to it.'),
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
    this.panel.focus();
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
  },
  stop() { this.close(); },
};
pbt.help = help;
pbt.module(help);

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
static std::wstring g_projectJs;     // "var __PBT_PROJECT__ = {...};" when a project folder is set

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
    PCWSTR setting = Wh_GetStringSetting(L"cheer");
    std::wstring path = setting ? setting : L"";
    Wh_FreeStringSetting(setting);
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

// The report's project files (the "project" setting: a .Report folder or a .pbip file), every
// .json under definition and StaticResources, as { root, files: [{ path, text }] } for the page.
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
            if (name != L".pbi" && name != L"CustomVisuals") ProjectWalk(root, sub, files, total, count);
            continue;
        }
        if (name.size() < 5 || _wcsicmp(name.c_str() + name.size() - 5, L".json") != 0) continue;
        if (fd.nFileSizeHigh || fd.nFileSizeLow > 1024 * 1024 || total + fd.nFileSizeLow > 8 * 1024 * 1024) continue;
        std::wstring full = root + L"\\" + sub;
        for (auto& c : full) if (c == L'/') c = L'\\';
        HANDLE h = CreateFileW(full.c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
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
    HANDLE h = CreateFileW(pbip.c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
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
    std::wstring wrel(rel.begin(), rel.end());
    for (auto& c : wrel) if (c == L'/') c = L'\\';
    return wrel.empty() ? fallback : dir + wrel;
}

static std::wstring ProjectScript() {
    PCWSTR setting = Wh_GetStringSetting(L"project");
    std::wstring path = setting ? setting : L"";
    Wh_FreeStringSetting(setting);
    while (!path.empty() && (path.back() == L' ' || path.back() == L'"' || path.back() == L'\\' || path.back() == L'/')) path.pop_back();
    while (!path.empty() && (path.front() == L' ' || path.front() == L'"')) path.erase(path.begin());
    if (path.empty()) return L"";
    if (path.size() > 5 && _wcsicmp(path.c_str() + path.size() - 5, L".pbip") == 0) path = PbipReportFolder(path);
    std::wstring files;
    size_t total = 0;
    int count = 0;
    // Only what the tools read, as the development injector does.
    ProjectWalk(path, L"/definition", files, total, count);
    ProjectWalk(path, L"/StaticResources", files, total, count);
    if (!count) { Wh_Log(L"project: no report files under %s", path.c_str()); return L""; }
    std::wstring rootJson;
    AppendJsonString(rootJson, path);
    // Handed over as one string that only the top-level page parses: every other frame (custom
    // visual sandboxes too) leaves the script's function before this line, and a string literal
    // costs those frames far less to compile than a multi-megabyte object literal would.
    std::wstring object = L"{\"root\":" + rootJson + L",\"files\":[" + files + L"]}";
    std::wstring s = L"var __PBT_PROJECT_JSON__ = ";
    AppendJsonString(s, object);
    s += L";\n";
    return s;
}

static void LoadSettings() {
    std::wstring cheer = CheerScript();
    std::wstring project = ProjectScript();
    PCWSTR skin = Wh_GetStringSetting(L"skin");
    std::wstring s = L"{\"skin\":\"";
    for (const wchar_t* c = skin; *c; c++) if (iswalnum(*c) || *c == L'-' || *c == L'_') s += *c;
    Wh_FreeStringSetting(skin);
    auto flag = [](PCWSTR name) { return Wh_GetIntSetting(name) ? L"true" : L"false"; };
    s += L"\",\"features\":{";
    s += L"\"splash\":";           s += flag(L"splash");
    s += L",\"notes\":";           s += flag(L"notes");
    s += L",\"confetti\":";        s += flag(L"confetti");
    s += L",\"sounds\":";          s += flag(L"sounds");
    s += L",\"visualCount\":";     s += flag(L"visualCount");
    s += L",\"reportFilterPane\":"; s += flag(L"keepReportFilterColors");
    // The palette for Colors: hex codes in any separators, with or without '#'. Anything
    // else is dropped; an empty list leaves the page's default palette in place.
    PCWSTR pal = Wh_GetStringSetting(L"palette");
    std::wstring palJson, tok;
    auto flush = [&]() {
        if (tok.size() == 4 || tok.size() == 7) { if (!palJson.empty()) palJson += L","; palJson += L"\"" + tok + L"\""; }
        tok.clear();
    };
    for (const wchar_t* c = pal; *c; c++) {
        if (*c == L'#') { flush(); tok = L"#"; }
        else if (iswxdigit(*c)) { if (tok.empty()) tok = L"#"; tok += *c; }
        else flush();
    }
    flush();
    Wh_FreeStringSetting(pal);
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
    g_projectJs = project;
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
// Handlers WebView2 still holds; any left at Wh_ModUninit keep this module loaded (their code is here).
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
// A record is freed only on its web view's thread, by TakeBack or by OnScriptAdded, never by
// Wh_ModUninit: a registration still pending when the mod is disabled has its callback to come.
struct Tracked {
    IUnknown* webview;
    HWND window;
    DWORD thread;
    std::wstring scriptId;
    bool settled = false;     // OnScriptAdded has run (the id, or a failure, is known)
    bool handedOver = false;  // TakeBack ran first and left the rest to OnScriptAdded
};
static std::vector<Tracked*> g_tracked;  // under g_lock

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
    // The record keeps the reference GetCoreWebView2 returned; Wh_ModUninit releases it.
    auto* t = new Tracked{webview, window, GetCurrentThreadId(), L""};
    {
        std::lock_guard<std::mutex> lock(g_lock);
        g_tracked.push_back(t);
    }
    IUnknown* done = MakeHandler(IID_AddScriptCompleted, nullptr, OnScriptAdded, t);
    Slot<AddScript_t>(webview, kWebViewAddScriptOnDocumentCreated)(webview, script.c_str(), done);
    done->Release();
}

static HRESULT OnControllerCreated(Handler* self, void* hr, void* controller) {
    if (SUCCEEDED((HRESULT)(INT_PTR)hr) && controller) Attach(static_cast<IUnknown*>(controller), static_cast<HWND>(self->ctx));
    return Forward(self, hr, controller);
}

// ---- the environment's controller factories ----
using CreateController_t = HRESULT(STDMETHODCALLTYPE*)(IUnknown*, HWND, IUnknown*);
using CreateControllerWithOptions_t = HRESULT(STDMETHODCALLTYPE*)(IUnknown*, HWND, IUnknown*, IUnknown*);
CreateController_t CreateController_Original;
CreateControllerWithOptions_t CreateControllerWithOptions_Original;

HRESULT STDMETHODCALLTYPE CreateController_Hook(IUnknown* env, HWND parent, IUnknown* handler) {
    IUnknown* wrapped = MakeHandler(IID_CtrlCompleted, handler, OnControllerCreated, parent);
    HRESULT hr = CreateController_Original(env, parent, wrapped);
    wrapped->Release();
    return hr;
}
HRESULT STDMETHODCALLTYPE CreateControllerWithOptions_Hook(IUnknown* env, HWND parent, IUnknown* options, IUnknown* handler) {
    IUnknown* wrapped = MakeHandler(IID_CtrlCompleted, handler, OnControllerCreated, parent);
    HRESULT hr = CreateControllerWithOptions_Original(env, parent, options, wrapped);
    wrapped->Release();
    return hr;
}

static std::atomic<bool> g_envHooked{false};
static HRESULT OnEnvironmentCreated(Handler* self, void* hr, void* envPtr) {
    IUnknown* env = static_cast<IUnknown*>(envPtr);
    if (SUCCEEDED((HRESULT)(INT_PTR)hr) && env && !g_envHooked.exchange(true)) {
        // Every environment shares one implementation, so hooking its methods once covers all.
        Wh_SetFunctionHook(Slot<void*>(env, kEnvCreateController), (void*)CreateController_Hook, (void**)&CreateController_Original);
        IUnknown* env10 = nullptr;
        if (SUCCEEDED(env->QueryInterface(IID_Environment10, (void**)&env10)) && env10) {
            Wh_SetFunctionHook(Slot<void*>(env10, kEnv10CreateControllerWithOptions), (void*)CreateControllerWithOptions_Hook, (void**)&CreateControllerWithOptions_Original);
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
    IUnknown* wrapped = MakeHandler(IID_EnvCompleted, handler, OnEnvironmentCreated);
    HRESULT hr = CreateEnv_Original(browserFolder, userDataFolder, options, wrapped);
    wrapped->Release();
    return hr;
}

static std::atomic<bool> g_loaderHooked{false};
static void HookLoader(HMODULE loader, bool apply) {
    if (!loader || g_loaderHooked.exchange(true)) return;
    void* target = (void*)GetProcAddress(loader, "CreateCoreWebView2EnvironmentWithOptions");
    if (!target) {
        Wh_Log(L"CreateCoreWebView2EnvironmentWithOptions not found in WebView2Loader.dll");
        return;
    }
    Wh_SetFunctionHook(target, (void*)CreateEnv_Hook, (void**)&CreateEnv_Original);
    if (apply) Wh_ApplyHookOperations();
}

using LoadLibraryExW_t = HMODULE(WINAPI*)(LPCWSTR, HANDLE, DWORD);
LoadLibraryExW_t LoadLibraryExW_Original;
HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR name, HANDLE file, DWORD flags) {
    HMODULE m = LoadLibraryExW_Original(name, file, flags);
    if (m && name && !g_loaderHooked) {
        const wchar_t* base = wcsrchr(name, L'\\');
        base = base ? base + 1 : name;
        if (_wcsicmp(base, L"WebView2Loader.dll") == 0) HookLoader(m, true);
    }
    return m;
}

BOOL Wh_ModInit() {
    LoadSettings();
    HMODULE already = GetModuleHandleW(L"WebView2Loader.dll");
    if (already) HookLoader(already, false);
    HMODULE kernelBase = GetModuleHandleW(L"kernelbase.dll");
    Wh_SetFunctionHook((void*)GetProcAddress(kernelBase, "LoadLibraryExW"), (void*)LoadLibraryExW_Hook, (void**)&LoadLibraryExW_Original);
    return TRUE;
}

// New settings win over the choice remembered inside Power BI (it is tagged with the settings it
// was made under) and apply to pages opened from now on.
void Wh_ModSettingsChanged() {
    LoadSettings();
}

// ---- disabling the mod: undo the page and take the script back out ----
// Windhawk calls Wh_ModUninit on a thread of its own, and WebView2 only answers the thread that
// made each web view. A thread-local WH_CALLWNDPROC hook sees a message sent to one of that
// thread's windows and runs the work there before the window gets it; SendMessage returns once
// the work is done, and the hook goes again.
struct ThreadWork {
    void (*fn)(void*);
    void* arg;
};
static UINT g_workMsg;
static LRESULT CALLBACK WorkHook(int code, WPARAM wParam, LPARAM lParam) {
    if (code == HC_ACTION) {
        const CWPSTRUCT* m = reinterpret_cast<const CWPSTRUCT*>(lParam);
        if (g_workMsg && m->message == g_workMsg) {
            const ThreadWork* w = reinterpret_cast<const ThreadWork*>(m->lParam);
            w->fn(w->arg);
        }
    }
    return CallNextHookEx(nullptr, code, wParam, lParam);
}
// A busy or hung Power BI thread must not hold the unload up: each web view gets this long. A
// message that timed out may still be handled later, so its work item is left allocated then.
constexpr UINT kTakeBackWaitMs = 3000;
static bool RunOnWindowThread(HWND window, void (*fn)(void*), void* arg, bool* timedOut) {
    DWORD thread = GetWindowThreadProcessId(window, nullptr);
    if (!thread) return false;
    if (thread == GetCurrentThreadId()) { fn(arg); return true; }
    if (!g_workMsg) g_workMsg = RegisterWindowMessageW(L"pbid-tweaks: run on the web view thread");
    if (!g_workMsg) return false;
    HHOOK hook = SetWindowsHookExW(WH_CALLWNDPROC, WorkHook, nullptr, thread);
    if (!hook) return false;
    auto* work = new ThreadWork{fn, arg};
    DWORD_PTR result = 0;
    LRESULT sent = SendMessageTimeoutW(window, g_workMsg, 0, reinterpret_cast<LPARAM>(work), SMTO_NORMAL | SMTO_ABORTIFHUNG, kTakeBackWaitMs, &result);
    UnhookWindowsHookEx(hook);
    if (!sent) { *timedOut = true; return false; }
    delete work;
    return true;
}

// On the web view's own thread: the page's teardown in the open document, then no script for the
// documents to come. ExecuteScript gets no completion handler, because this mod's code is unloaded
// before the page could answer. A registration still pending leaves the script and the record to
// OnScriptAdded, on this same thread.
static void TakeBack(void* arg) {
    using ExecuteScript_t = HRESULT(STDMETHODCALLTYPE*)(IUnknown*, LPCWSTR, IUnknown*);
    auto* t = static_cast<Tracked*>(arg);
    Slot<ExecuteScript_t>(t->webview, kWebViewExecuteScript)(t->webview, kTeardown, nullptr);
    {
        std::lock_guard<std::mutex> lock(g_lock);
        if (!t->settled) { t->handedOver = true; return; }
    }
    Retire(t);
}

// Code WebView2 may still call (a handler it holds, a hook mid-run after a timeout) stays loaded.
static void KeepModuleLoaded() {
    HMODULE module = nullptr;
    GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_PIN, reinterpret_cast<LPCWSTR>(&KeepModuleLoaded), &module);
}

static BOOL CALLBACK FirstWindow(HWND window, LPARAM out) {
    *reinterpret_cast<HWND*>(out) = window;
    return FALSE;
}

void Wh_ModUninit() {
    std::vector<Tracked*> tracked;
    {
        std::lock_guard<std::mutex> lock(g_lock);
        tracked.swap(g_tracked);
    }
    int done = 0;
    bool timedOut = false;
    for (Tracked* t : tracked) {
        // The window the controller was made for, else any other window of the same thread (the
        // web view may have moved to a new parent).
        HWND window = t->window;
        if (!IsWindow(window) || GetWindowThreadProcessId(window, nullptr) != t->thread) {
            window = nullptr;
            EnumThreadWindows(t->thread, FirstWindow, reinterpret_cast<LPARAM>(&window));
        }
        if (window && RunOnWindowThread(window, TakeBack, t, &timedOut)) done++;
        // Else the record is left as it is, reference and all: releasing it from this thread is not
        // allowed, and freeing it could race a callback still to come on its own thread.
    }
    long live = g_liveHandlers.load();
    if (timedOut || live > 0) KeepModuleLoaded();
    Wh_Log(L"uninit: script taken back out of %d of %d web views%s; %ld handler(s) still held%s", done, (int)tracked.size(),
           timedOut ? L" (one or more timed out)" : L"", live, timedOut || live > 0 ? L", module kept loaded" : L"");
}
