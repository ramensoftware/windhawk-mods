// ==WindhawkMod==
// @id              outlook-refined
// @name            Outlook Refined
// @description     Restyles new Outlook: favorites groups, per-account compose buttons, a resizable folder pane, an agenda with a month grid, the Other inbox under Focused, a tidier calendar and reading pane, and a print layout
// @version         0.4
// @author          Analytic Endeavors
// @github          https://github.com/ae-reid-havens
// @include         olk.exe
// @architecture    x86-64
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Outlook Refined

Restyles the new Outlook for Windows (olk.exe) from inside its own page. The mod
adds one script to each Outlook window when the window is created, and the
script stops at once unless the page is one of Outlook's own. It reads nothing
from disk, opens no ports and fetches nothing from the network.

![Folder pane](https://raw.githubusercontent.com/ae-reid-havens/outlook-refined/main/docs/folder-pane.png)

Restart Outlook after enabling the mod or changing a setting.

**What it asks Outlook for.** The agenda, month grid and Other inbox card read
your calendar and inbox through Outlook's own mail service, from inside its
page, signed with the session Outlook already holds: the mod keeps the request
headers Outlook sets, in memory, and reuses them. Those features refresh every
few minutes while they are on, and each is off until you turn it on. Nothing is
stored, logged or sent anywhere else, and no credential is written down.

**Where your edits are kept.** Groups, renames, folds, account order and the
pane and card sizes are saved in Windhawk's own storage for this mod, so they
leave when the mod does. State an earlier version wrote into Outlook's own local
storage is moved across the first time this one runs. Other Outlook windows
that are already open pick up a change when they are reopened.

**Folder pane:** Favorites in groups that fold (right-click a group header for
its menu, Shift+right-click a favorite to rename or move it), per-account
compose buttons, a resizable pane, account trees under one foldable heading,
accounts reordered by dragging.

**Agenda:** today and the next days from every account's calendar at the bottom
of the folder pane, with a join button for online meetings, under a month grid
shaded by booked hours. **Other inbox:** the newest Other conversations below
the Focused list.

![Agenda and the Other inbox card](https://raw.githubusercontent.com/ae-reid-havens/outlook-refined/main/docs/other-inbox.png)

**Calendar:** one row per account, compact time zones, start and end times on
event tiles, overlapping events cascaded or merged.

**Reading pane:** To and Cc on one line, a compact blocked-content bar, a
readable body width. **Ribbon:** the group-name row under the buttons goes.

**Print:** printing a message prints the message, not the whole window: the
panes, ribbon and buttons go, panes that scroll are released so nothing is cut
off, quoted replies can be lightened or dropped, pictures and tables are kept
whole, and anything too wide for the page is scaled down to fit.

**Known limits.** Most features find Outlook's own elements by English labels
("Focused", "Other", "Navigation pane", "Reading Pane") and read times written
as 1:30 PM, so in another display language, or with a 24-hour clock, those
parts quietly do nothing. The folder pane, the agenda and the print layout are
the parts that depend on this most.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- favoritesGroups: true
  $name: Favorites groups
  $description: Show favorites in groups that fold. Starts with an Outgoing group (Drafts, Sent, Outbox) and a Junk group
- hiddenFolders: [""]
  $name: Hidden favorites
  $description: Favorite folder names to hide, as Outlook shows them (for example "Archive")
- composeButtons:
  - - account: ""
      $name: Account address
    - label: ""
      $name: Button label
    - color: "#0f6cbd"
      $name: Button color
  $name: Compose buttons
  $description: One button per account at the top of Favorites; a new email starts from that account
- accountLabels:
  - - account: ""
      $name: Account address
    - label: ""
      $name: Short label
  $name: Account labels
  $description: Short names for accounts in the folder pane, pattern groups and the calendar list
- primaryAccount: ""
  $name: Primary account address
  $description: Leave empty to detect it. Outlook names the primary account's folders without an address suffix
- accountsHeading: Accounts
  $name: Accounts heading
  $description: Heading that folds every account tree. Empty for none
- hideAccountTrees: false
  $name: Hide account trees
- hideAddAccount: false
  $name: Hide "Add account"
- resizablePane: true
  $name: Resizable folder pane
  $description: Drag the pane's right edge; double-click it to reset
- paneWidth: 0
  $name: Folder pane width (px)
  $description: 0 keeps Outlook's own width. A width set by dragging the pane's edge wins; double-click the edge to return to this one
- agenda: false
  $name: Agenda under the folder pane
  $description: Today and the next days from every account's calendar, with a join button for online meetings. Reads your calendar through Outlook's own mail service, signed with the session Outlook already holds, and refreshes every five minutes while it is on. Nothing is stored or sent anywhere else
- agendaDays: 3
  $name: Agenda days
  $description: Days the agenda lists, today included (1 to 14)
- monthHeatMap: false
  $name: Month grid above the agenda
  $description: Each day shaded by its booked hours; the arrows browse other months. Reads the same calendar data as the agenda, through Outlook's session
- otherCard: false
  $name: Other inbox under Focused
  $description: While an inbox shows its Focused tab, its newest Other conversations sit below the list. A button by the tabs switches back to Outlook's own tabs. Reads the Other list through Outlook's session and refreshes every two minutes while it is on
- calendarRowPerAccount: true
  $name: Calendar, one row per account
- calendarPaneWidth: 180
  $name: Calendar pane width (px)
  $description: 0 keeps Outlook's own width
- compactTimeZones: true
  $name: Calendar, compact time zones
- tileTimes: true
  $name: Calendar, times on event tiles
- overlapLayout: merge
  $name: Calendar, overlapping events
  $options:
  - none: Outlook's layout
  - cascade: Cascade
  - merge: Cascade, and merge larger overlaps into one tile
- mergeAt: 3
  $name: Merge at this many overlapping events
- oneLineRecipients: true
  $name: Reading pane, To and Cc on one line
- blockedContentChip: true
  $name: Reading pane, compact blocked-content bar
- readingWidth: 90ch
  $name: Reading pane, body width
  $description: A CSS width such as 90ch or 800px. Empty for Outlook's own
- hideRibbonGroupLabels: true
  $name: Ribbon, hide group names
- printCompactHeader: true
  $name: Print, compact header
  $description: Drops the avatar and action buttons and tightens the sender block, so more of the message fits the first page
- printFit: overflow
  $name: Print, fit to the page width
  $options:
  - none: Never
  - overflow: Only when the message is too wide
  - always: Always
- printKeepChartsWhole: true
  $name: Print, keep pictures and tables whole
  $description: Never split a picture, table or code block across two pages
- printQuotedReplies: lighten
  $name: Print, quoted replies
  $options:
  - show: Print them as they are
  - lighten: Print them smaller and grey
  - hide: Leave them out
*/
// ==/WindhawkModSettings==

#include <unknwn.h>
#include <atomic>
#include <map>
#include <string>
#include <string_view>
#include <vector>

// Interface ids and vtable slots from WebView2.h (Microsoft.Web.WebView2 SDK
// 1.0.3179.45, BSD licence). COM vtables are a stable ABI, so slots never move.
constexpr GUID kIID_IUnknown = {0x00000000, 0x0000, 0x0000, {0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46}};
constexpr GUID kIID_EnvCompleted = {0x4e8a3389, 0xc9d8, 0x4bd2, {0xb6, 0xb5, 0x12, 0x4f, 0xee, 0x6c, 0xc1, 0x4d}};
constexpr GUID kIID_ControllerCompleted = {0x6c4819f3, 0xc9b7, 0x4260, {0x81, 0x27, 0xc9, 0xf5, 0xbd, 0xe7, 0xf6, 0x8c}};
constexpr GUID kIID_CompositionCompleted = {0x02fab84b, 0x1428, 0x4fb7, {0xad, 0x45, 0x1b, 0x2e, 0x64, 0x73, 0x61, 0x84}};
constexpr GUID kIID_ScriptAdded = {0xb99369f3, 0x9b11, 0x47b5, {0xbc, 0x6f, 0x8e, 0x78, 0x95, 0xfc, 0xea, 0x17}};
constexpr GUID kIID_Env3 = {0x80a22ae3, 0xbe7c, 0x4ce2, {0xaf, 0xe1, 0x5a, 0x50, 0x05, 0x6c, 0xde, 0xeb}};
constexpr GUID kIID_Env10 = {0xee0eb9df, 0x6f12, 0x46ce, {0xb5, 0x3f, 0x3f, 0x47, 0xb9, 0xc9, 0x28, 0xe0}};
constexpr GUID kIID_Controller = {0x4d00c0d1, 0x9434, 0x4eb6, {0x80, 0x78, 0x86, 0x97, 0xa5, 0x60, 0x33, 0x4f}};
constexpr GUID kIID_WebMessageReceived = {0x57213f19, 0x00e6, 0x49fa, {0x8e, 0x07, 0x89, 0x8e, 0xa0, 0x1e, 0xcb, 0xd2}};

enum {
    kEnv_CreateController = 3,
    kEnv3_CreateCompositionController = 9,
    kEnv10_CreateControllerWithOptions = 21,
    kEnv10_CreateCompositionControllerWithOptions = 22,
    kController_GetParentWindow = 21,
    kController_Close = 24,
    kController_GetCoreWebView2 = 25,
    kWebView_AddScriptOnDocumentCreated = 27,
    kWebView_RemoveScriptOnDocumentCreated = 28,
    kWebView_ExecuteScript = 29,
    kWebView_AddWebMessageReceived = 34,
    kWebView_RemoveWebMessageReceived = 35,
    kArgs_TryGetWebMessageAsString = 5,
    kHandler_Invoke = 3,
};

// The page script (src/page.js), stitched in by build.mjs.
const wchar_t kPageScript[] = LR"olkt(// Runs inside Outlook's page. The Windhawk mod wraps it as `{ const OLKT_CONFIG = {...}; <this file> }`,
// where OLKT_CONFIG holds the mod's settings (see the settings block in src/mod.cpp).
// Everything is CSS on Outlook's own elements (found by data-folder-name and aria
// attributes, never by generated class names) plus elements of our own (group
// headers, compose buttons, menus, the resize handle). Outlook's own nodes are
// never moved, so its React tree stays intact.
(() => {
  // Outlook has hidden subframes with folder trees of their own; only the main window is ours.
  if (window.top !== window) return 'skipped subframe';
  // Outlook opens WebViews for pages of its own and for add-in dialogs, which load
  // third-party sites. Only Outlook's own hosts are ours: anywhere else the script
  // returns before it wraps anything or adds a stylesheet.
  const OUTLOOK_HOSTS = /(^|\.)outlook(-sdf)?\.(office|office365|live)\.com$|(^|\.)outlook(-sdf)?\.cloud\.microsoft$/i;
  if (!OUTLOOK_HOSTS.test(location.hostname)) return 'not an Outlook page';
  // ==== Outlook's own session ====
  // Outlook signs each call to its mail service with a per-account Authorization header.
  // fetch and XMLHttpRequest are wrapped once per page to keep the newest such headers
  // per mailbox, so the agenda, month grid and Other card can ask that same service with
  // the session Outlook is already signed in to. The headers stay inside this closure: the
  // store only says which mailboxes it holds and sends requests on their behalf. Nothing
  // is stored or logged, and removing the mod leaves the wrappers idle.
  const SESSION_V = 5;
  const session = (window.__olktSession && window.__olktSession.v === SESSION_V) ? window.__olktSession : (window.__olktSession = (() => {
    const byAnchor = new Map(); // x-anchormailbox -> { base, headers, at }
    const byCategory = new Map(); // x-anchormailbox (or '*') -> Map(category name -> colour index)
    const asked = new Set(); // mailboxes whose palette has been asked for, so it is asked once
    const KEEP = ['authorization', 'x-owa-sessionid', 'x-client-version', 'x-tenantid', 'prefer', 'x-owa-hosted-ux', 'x-req-source'];
    const listeners = new Set();
    const entries = src => {
      try {
        if (!src) return [];
        if (typeof src.entries === 'function' && !Array.isArray(src)) return [...src.entries()];
        return Array.isArray(src) ? src : Object.entries(src);
      } catch { return []; }
    };
    // Outlook addresses its own service with a relative URL as often as an absolute one, so
    // the address is resolved against the page before it is matched. The service's own base
    // is kept per mailbox, so each one is asked on the host Outlook itself used.
    const note = (url, headers) => {
      let u;
      try { u = new URL(url, location.href); } catch { return; }
      if (u.protocol !== 'https:' || !u.pathname.includes('/owa/') || !/\/service\.svc$/.test(u.pathname) || !u.searchParams.get('action')) return;
      const h = {};
      for (const [k, v] of headers) h[String(k).toLowerCase()] = String(v);
      const anchor = h['x-anchormailbox'];
      if (!anchor || !/^Bearer /.test(h.authorization || '')) return;
      const kept = {};
      for (const k of KEEP) if (h[k]) kept[k] = h[k];
      const fresh = !byAnchor.has(anchor);
      byAnchor.set(anchor, { base: u.origin + u.pathname, headers: kept, at: Date.now() });
      if (fresh) for (const fn of listeners) { try { fn(anchor); } catch {} }
    };
    // Each category in the startup payload is written as {"Name":"...","Color":n,"Id":"..."},
    // where the colour is an index into Outlook's fixed palette. Pulled out with a search
    // rather than by parsing the whole payload, which is large and none of our business.
    const CATEGORY = /\{"Name":"((?:[^"\\]|\\.)*)","Color":(-?\d+),"Id":"/g;
    const readPalette = (anchor, text) => {
      const map = new Map();
      for (const m of text.matchAll(CATEGORY)) {
        let name = m[1];
        try { name = JSON.parse('"' + m[1] + '"'); } catch {}
        const color = Number(m[2]);
        if (name && color >= 0) map.set(name, color);
      }
      // Only for the mailbox it came from: two accounts can give the same category name
      // different colours, and a palette with no mailbox on it belongs to none of them.
      if (map.size && anchor) byCategory.set(anchor, map);
    };
    const isStartup = url => { try { return /\/owa\/startupdata\.ashx/i.test(new URL(url, location.href).pathname); } catch { return false; } };
    const nativeFetch = window.fetch;
    const wrappedFetch = function (input, init) {
      let url, anchor;
      try {
        url = typeof input === 'string' ? input : input instanceof URL ? input.href : input && input.url;
        const headers = entries((init && init.headers) || (input && input.headers));
        note(url, headers);
        for (const [k, v] of headers) if (String(k).toLowerCase() === 'x-anchormailbox') anchor = String(v);
      } catch {}
      const out = nativeFetch.apply(this, arguments);
      if (url && isStartup(url)) {
        // A copy is read, so Outlook still gets its own body untouched.
        try { out.then(res => res.clone().text().then(t => readPalette(anchor, t), () => {}), () => {}); } catch {}
      }
      return out;
    };
    window.fetch = wrappedFetch;
    const X = XMLHttpRequest.prototype, open = X.open, setHeader = X.setRequestHeader, send = X.send;
    const wrappedOpen = function (method, url) { this.__olktUrl = String(url); this.__olktHeaders = []; return open.apply(this, arguments); };
    const wrappedSetHeader = function (k, v) { if (this.__olktHeaders) this.__olktHeaders.push([k, v]); return setHeader.apply(this, arguments); };
    const wrappedSend = function () { try { note(this.__olktUrl, this.__olktHeaders || []); } catch {} return send.apply(this, arguments); };
    X.open = wrappedOpen;
    X.setRequestHeader = wrappedSetHeader;
    X.send = wrappedSend;
    return {
      v: SESSION_V,
      has: anchor => byAnchor.has(anchor),
      // Called when the mod is disabled: puts fetch and XHR back the way they were and
      // drops every header the store holds. Only restores what is still ours, so a
      // wrapper another extension added after ours stays in place.
      stop() {
        if (window.fetch === wrappedFetch) window.fetch = nativeFetch;
        if (X.open === wrappedOpen) X.open = open;
        if (X.setRequestHeader === wrappedSetHeader) X.setRequestHeader = setHeader;
        if (X.send === wrappedSend) X.send = send;
        byAnchor.clear();
        byCategory.clear();
        asked.clear();
        listeners.clear();
        if (window.__olktSession === this) delete window.__olktSession;
        return 'session released';
      },
      // The category palette for a mailbox, if Outlook has loaded one this session.
      palette: anchor => byCategory.get(anchor) || null,
      anchors: () => [...byAnchor.keys()],
      on: fn => listeners.add(fn),
      off: fn => listeners.delete(fn),
      // Ask for the startup payload once, for its category palette, when Outlook loaded it
      // somewhere this page could not watch. Resolves to how many categories it found.
      async palettes(anchor) {
        const e = byAnchor.get(anchor);
        if (!e || byCategory.has(anchor) || asked.has(anchor)) return 0;
        asked.add(anchor);
        // A request that fails is not an answer: let a later refresh try again, while the
        // flag still stops two running at once.
        const release = () => setTimeout(() => asked.delete(anchor), 60000);
        const headers = { ...e.headers, 'x-anchormailbox': anchor, 'content-type': 'application/json; charset=utf-8', 'x-owa-actionsource': 'OutlookRefined' };
        const url = new URL(e.base).origin + '/owa/startupdata.ashx?app=Mail&n=1';
        let res;
        try { res = await nativeFetch.call(window, url, { method: 'POST', headers, body: '{}' }); }
        catch (err) { release(); throw err; }
        if (!res.ok) { release(); throw new Error('startup payload: ' + res.status); }
        readPalette(anchor, await res.text());
        const got = byCategory.get(anchor);
        return got ? got.size : 0;
      },
      // One service call on a mailbox's behalf, with the headers kept for it, to the same
      // service host Outlook used. body is the JSON text to post; resolves to the Response.
      call(anchor, action, body, n) {
        const e = byAnchor.get(anchor);
        if (!e) return Promise.reject(new Error('waiting for Outlook'));
        const headers = { ...e.headers, action, 'x-anchormailbox': anchor, 'content-type': 'application/json; charset=utf-8', 'x-owa-actionsource': 'OutlookRefined' };
        return nativeFetch.call(window, e.base + '?action=' + action + '&app=Mail&n=' + n, { method: 'POST', headers, body });
      },
    };
  })());
  // Each install replaces the previous one. Older copies (for example from an
  // injector that was restarted) see a newer generation and shut themselves down.
  if (window.__olkt) window.__olkt.remove();
  const gen = window.__olktGen = (window.__olktGen || 0) + 1;
  const current = () => window.__olktGen === gen;
  const OURS = '.olkt-head, .olkt-accounts, .olkt-compose, .olkt-menu, .olkt-rename, .olkt-resize, .olkt-agenda, .olkt-other, .olkt-other-toggle, .olkt-merge, #olkt-style';
  document.querySelectorAll(OURS).forEach(n => n.remove());

  const FOLD_KEY = 'olkt-folds';
  const WIDTH_KEY = 'olkt-pane-width';
  const FAVORITES_KEY = 'olkt-favorites';
  const ACCOUNT_ORDER_KEY = 'olkt-account-order';
  // Static choices come from the mod's settings. Edits made inside Outlook (groups,
  // renames, folds, account order, pane and card sizes) belong to the mod rather than to
  // Outlook, so they live in Windhawk's own storage: the mod hands them in through
  // OLKT_CONFIG.store and takes each write back over the WebView's message channel.
  // Nothing of ours is left in Outlook's profile when the mod goes.
  const S = OLKT_CONFIG;
  const STORE_KEYS = [FOLD_KEY, WIDTH_KEY, FAVORITES_KEY, ACCOUNT_ORDER_KEY, 'olkt-agenda-height', 'olkt-other-height', 'olkt-other-split', 'olkt-other-folds'];
  const store = (() => {
    const mem = new Map(Object.entries(S.store || {}));
    // One line each: the marker, the key, then the value, with an empty value meaning
    // delete. Outlook's own handler reads its messages as JSON and ignores this shape.
    const post = (key, value) => {
      try { window.chrome.webview.postMessage('olkt-store\n' + key + '\n' + (value == null ? '' : value)); } catch {}
    };
    // State an earlier version wrote into Outlook's localStorage moves across once, so
    // favorites and sizes survive the change and Outlook's profile is left clean.
    for (const key of STORE_KEYS) {
      try {
        const old = localStorage.getItem(key);
        if (old == null) continue;
        if (!mem.has(key)) { mem.set(key, old); post(key, old); }
        localStorage.removeItem(key);
      } catch {}
    }
    return {
      getItem: key => (mem.has(key) ? mem.get(key) : null),
      setItem: (key, value) => { mem.set(key, String(value)); post(key, String(value)); },
      removeItem: key => { mem.delete(key); post(key, null); },
    };
  })();
  const stored = (key, fallback) => { try { const v = JSON.parse(store.getItem(key)); return Array.isArray(v) ? v : fallback; } catch { return fallback; } };
  // Pattern groups fill themselves with each account's folder of that name.
  const DEFAULT_FAVORITES = [
    { id: 'outgoing', label: 'Outgoing', items: [
      { id: 'out-drafts', label: 'Drafts', match: '^drafts( \\(.*\\))?$', autoLabel: true, items: [] },
      { id: 'out-sent', label: 'Sent', match: '^sent items( \\(.*\\))?$', autoLabel: true, items: [] },
      { id: 'out-outbox', label: 'Outbox', match: '^outbox( \\(.*\\))?$', autoLabel: true, folded: true, items: [] },
    ] },
    { id: 'junk', label: 'Junk', match: '^(junk email|spam)( \\(.*\\))?$', autoLabel: true, items: [] },
  ];
  const lower = s => String(s || '').trim().toLowerCase();
  // Accounts in the saved drag order first, then the ones that only have a label.
  // A value is the account's short label, or '' to keep Outlook's own name.
  const labels = Object.fromEntries((S.accountLabels || []).filter(a => a.account).map(a => [lower(a.account), a.label || '']));
  function orderedAccounts(order) {
    const accounts = {};
    for (const a of order) accounts[lower(a)] = labels[lower(a)] || '';
    for (const [a, label] of Object.entries(labels)) if (!(a in accounts)) accounts[a] = label;
    return accounts;
  }
  const cfg = {
    favorites: S.favoritesGroups ? stored(FAVORITES_KEY, DEFAULT_FAVORITES) : [],
    hide: (S.hiddenFolders || []).map(lower).filter(Boolean),
    accounts: orderedAccounts(stored(ACCOUNT_ORDER_KEY, [])),
    accountsHeading: S.accountsHeading || '',
    hideAccountTrees: !!S.hideAccountTrees,
    hideAddAccount: !!S.hideAddAccount,
    compose: (S.composeButtons || []).filter(c => c.account && c.label).map(c => ({ account: lower(c.account), label: c.label, color: c.color || '#0f6cbd' })),
    paneWidth: Number(S.paneWidth) || 0, // 0 = Outlook's own width
    calendar: { paneWidth: Number(S.calendarPaneWidth) || 0, labels, mergeAt: S.overlapLayout === 'cascade' ? Infinity : Number(S.mergeAt) || 3 },
    calendarOverlap: S.overlapLayout !== 'none',
    readingWidth: S.readingWidth || '',
    hideRibbonGroupLabels: !!S.hideRibbonGroupLabels,
    agenda: S.agenda !== false,
    agendaDays: Math.max(1, Math.min(14, Number(S.agendaDays) || 3)),
    monthHeatMap: S.monthHeatMap !== false,
    otherCard: !!S.otherCard,
    print: {
      compactHeader: S.printCompactHeader !== false,
      fit: ['none', 'overflow', 'always'].includes(S.printFit) ? S.printFit : 'overflow',
      keepWhole: S.printKeepChartsWhole !== false,
      quoted: ['show', 'lighten', 'hide'].includes(S.printQuotedReplies) ? S.printQuotedReplies : 'lighten',
    },
  };
  // Folders with no "(address)" suffix belong to the primary account. Outlook lists
  // the primary account's tree first, so its root names it; the setting overrides.
  // Read each time, never cached: the tree can show placeholders while Outlook starts.
  function primary() {
    if (S.primaryAccount) return lower(S.primaryAccount);
    const root = document.querySelector(ACCT_ROOT);
    return lower(root && root.getAttribute('data-folder-name'));
  }
  // With several accounts Outlook usually suffixes every account's folders, the
  // primary's too; the plain name is the fallback for the primary only.
  const inboxOf = account => {
    const suffixed = `inbox (${account})`;
    return !document.querySelector(row(suffixed)) && account === primary() ? 'inbox' : suffixed;
  };
  let folds = {};
  try { folds = JSON.parse(store.getItem(FOLD_KEY) || '{}'); } catch {}
  let paneWidth = 0;
  try { paneWidth = Number(store.getItem(WIDTH_KEY)) || 0; } catch {}
  if (!S.resizablePane) paneWidth = 0;
  paneWidth = paneWidth || cfg.paneWidth;

  const q = s => '"' + CSS.escape(s) + '"';
  const PANE = '[role="region"][aria-label="Navigation pane"]';
  const ACCT_ROOT = '[role="treeitem"][aria-level="1"][id^="primaryMailboxRoot"]';
  const ACCT_LIST = `div:has(> div > ${ACCT_ROOT})`;
  const FAVS = '[data-olkt-favs]';
  const row = f => `[role="treeitem"][data-folder-name=${q(f)}]`;
  // Favorites rules are scoped to the Favorites list: the same folder also
  // appears in its account's tree, which must keep its real name.
  const favRow = f => `${FAVS} > div > ${row(f)}`;
  const favWrap = f => `${FAVS} > div:has(> ${row(f)})`;
  const sleep = ms => new Promise(r => setTimeout(r, ms));

  // ==== favorites tree: groups ({id, label, items}) holding folders ({folder, label}) ====
  const isGroup = n => Array.isArray(n.items);
  function walk(items, fn, depth = 0, parent = null, hidden = false) {
    items.forEach((n, index) => {
      fn(n, { depth, parent, index, hidden });
      if (isGroup(n)) walk(n.items, fn, depth + 1, n, hidden || !!folds[n.id]);
    });
  }
  function locate(target) {
    let found = null;
    walk(cfg.favorites, (n, ctx) => { if (n === target) found = ctx; });
    return found && { ...found, siblings: found.parent ? found.parent.items : cfg.favorites };
  }
  function findFolder(folder) {
    let found = null;
    walk(cfg.favorites, n => { if (!isGroup(n) && n.folder === folder) found = n; });
    return found;
  }
  function findGroup(id) {
    let found = null;
    walk(cfg.favorites, n => { if (isGroup(n) && n.id === id) found = n; });
    return found;
  }
  const groups = () => { const out = []; walk(cfg.favorites, (n, ctx) => { if (isGroup(n)) out.push({ n, depth: ctx.depth }); }); return out; };
  for (const { n } of groups()) if (!(n.id in folds)) folds[n.id] = !!n.folded;
  // The account trees fold under one "All folders" header, collapsed until opened.
  const ACCOUNTS = '__accounts';
  if (!(ACCOUNTS in folds)) folds[ACCOUNTS] = true;

  // Groups with a "match" pattern fill themselves with every favorite whose name
  // matches (for example each account's Drafts), labeled by account. Computed on
  // every render, never saved, so Outlook renaming "Sent Items" to
  // "Sent Items (account)" when a second one is added needs no config change.
  function accountOf(name) {
    const m = /\(([^()]+@[^()]+)\)$/.exec(name);
    return (m ? m[1] : primary()).toLowerCase();
  }
  function favoriteNames() {
    const list = favoritesList();
    if (!list) return [];
    return [...list.querySelectorAll(':scope > div > [role="treeitem"]')].map(t => t.getAttribute('data-folder-name')).filter(Boolean);
  }
  // Group patterns, compiled once per pattern string. A pattern that does not compile
  // matches nothing (one console warning) instead of stopping every tweak.
  const NEVER = /(?!)/;
  const patterns = new Map();
  function pattern(src) {
    let re = patterns.get(src);
    if (!re) {
      try { re = new RegExp(src, 'i'); } catch (err) {
        console.warn(`outlook-refined: group match ${JSON.stringify(src)} is not a valid pattern (${err.message}); it matches nothing`);
        re = NEVER;
      }
      patterns.set(src, re);
    }
    return re;
  }
  function matchedIn(group) {
    if (!group.match) return [];
    const re = pattern(group.match);
    const hidden = new Set(cfg.hide);
    const accountKeys = Object.keys(cfg.accounts || {}); // current, so a drag reorders these too
    return favoriteNames()
      .filter(name => re.test(name) && !hidden.has(name) && !findFolder(name))
      .map(name => {
        const acct = accountOf(name);
        return { name, rank: accountKeys.indexOf(acct), label: group.autoLabel ? (cfg.accounts[acct] || acct || name) : undefined };
      })
      .sort((a, b) => (a.rank < 0 ? 99 : a.rank) - (b.rank < 0 ? 99 : b.rank));
  }
  const matchesAnyGroup = name => groups().some(({ n: g }) => g.match && pattern(g.match).test(name));

  function save() {
    // Per PC, on Outlook's origin; other open windows pick it up when they next start.
    try {
      if (S.favoritesGroups) store.setItem(FAVORITES_KEY, JSON.stringify(cfg.favorites));
      store.setItem(ACCOUNT_ORDER_KEY, JSON.stringify(Object.keys(cfg.accounts)));
    } catch {}
    render();
  }

  // ==== CSS ====
  let fs;
  function buildCss() {
    // Read Outlook's label size once the tree exists, before rename rules zero it.
    const label = document.querySelector('[role="treeitem"] > span');
    if (!fs && label) fs = getComputedStyle(label).fontSize;
    const size = fs || '14px';
    const out = [];
    out.push(`${PANE}{position:relative}`);
    // No width set and never resized: the pane keeps Outlook's own width.
    if (paneWidth) out.push(`${PANE}{width:var(--olkt-pane-w)!important;min-width:var(--olkt-pane-w)!important;max-width:var(--olkt-pane-w)!important}`);
    out.push(`${PANE} [role="treeitem"]{max-width:100%;box-sizing:border-box}`);
    out.push(`.olkt-resize{position:absolute;top:0;right:-3px;width:6px;height:100%;cursor:col-resize;z-index:20}`);
    out.push(`.olkt-resize:hover,.olkt-resize.dragging{background:var(--colorBrandStroke1,#4aa3f0);opacity:.6}`);

    if (cfg.hideAccountTrees) {
      out.push(`${PANE} div:has(> [role="treeitem"][aria-level="1"]:not([data-folder-name="favorites"])){display:none!important}`);
      out.push(`${PANE} div:has(> ${row('add account')}){display:none!important}`);
    }
    for (const [acct, name] of Object.entries(cfg.accounts || {})) {
      if (!name) continue;
      const r = `${PANE} [role="treeitem"][aria-level="1"][data-folder-name=${q(acct)}] > span:first-of-type`;
      out.push(`${r}{font-size:0!important}`, `${r}::after{content:${JSON.stringify(name)};font-size:${size}}`);
    }
    if (cfg.accountsHeading && folds[ACCOUNTS]) {
      out.push(`${PANE} div:has(> [role="treeitem"][aria-level="1"]:not([data-folder-name="favorites"])){display:none!important}`);
      out.push(`${PANE} div:has(> ${row('add account')}){display:none!important}`);
    }
    if (cfg.hideAddAccount) out.push(`${PANE} div:has(> ${row('add account')}){display:none!important}`);
    // Account order follows the "accounts" list; dragging an account rewrites it.
    out.push(`${ACCT_LIST}{display:flex!important;flex-direction:column}`);
    out.push(`${ACCT_LIST} > div{order:50}`);
    out.push(`${ACCT_LIST} > .olkt-accounts{order:0}`);
    Object.keys(cfg.accounts || {}).forEach((acct, i) => {
      out.push(`${ACCT_LIST} > div:has(> [role="treeitem"][aria-level="1"][data-folder-name=${q(acct)}]){order:${i + 1}}`);
    });
    out.push(`.olkt-acct-drop{position:fixed;z-index:100000;height:2px;background:var(--colorBrandStroke1,#4aa3f0);pointer-events:none}`);
    out.push(`${ACCT_LIST}.olkt-dragging [role="treeitem"][aria-level="1"]{cursor:grabbing}`);
    out.push(`.olkt-head.olkt-accounts{margin-top:10px;padding-left:12px;border-top:1px solid var(--colorNeutralStroke2,#3a3a3a);border-radius:0;height:32px}`);

    // Favorites: our compose bar first, then anything not yet placed (in Outlook's
    // order, at top level, so a new install keeps every existing favorite in view),
    // then the tree in order.
    out.push(`${FAVS} > div{order:0}`);
    out.push(`${FAVS} > .olkt-compose{order:-1}`);
    for (const f of cfg.hide) out.push(`${favWrap(f)}{display:none!important}`);
    let n = 0;
    const folder = (name, label, depth, hidden) => {
      out.push(`${favWrap(name)}{order:${++n}}`);
      out.push(`${favRow(name)}{padding-left:${12 + depth * 12}px!important}`);
      if (label) {
        out.push(`${favRow(name)} > span:first-of-type{font-size:0!important}`);
        out.push(`${favRow(name)} > span:first-of-type::after{content:${JSON.stringify(label)};font-size:${size}}`);
      }
      if (hidden) out.push(`${favWrap(name)}{display:none!important}`);
    };
    const emit = (items, depth, hidden) => {
      for (const node of items) {
        if (!isGroup(node)) { folder(node.folder, node.label, depth, hidden); continue; }
        const h = `${FAVS} > .olkt-head[data-group=${q(node.id)}]`;
        out.push(`${h}{order:${++n};padding-left:${12 + depth * 12}px}`);
        if (hidden) out.push(`${h}{display:none!important}`);
        const inner = hidden || !!folds[node.id];
        emit(node.items, depth + 1, inner);
        for (const m of matchedIn(node)) folder(m.name, m.label, depth + 1, inner);
      }
    };
    emit(cfg.favorites, 0, false);

    out.push(`.olkt-head{all:unset;box-sizing:border-box;display:flex;align-items:center;gap:8px;height:26px;width:100%;cursor:pointer;font-size:12px;font-weight:600;letter-spacing:.04em;color:var(--colorNeutralForeground3,#a3a3a3);border-radius:4px}`);
    out.push(`.olkt-head:hover{background:var(--colorNeutralBackground1Hover,#333);color:var(--colorNeutralForeground1,#e6e6e6)}`);
    out.push(`.olkt-head:focus-visible{outline:2px solid var(--colorBrandStroke1,#4aa3f0)}`);
    out.push(`.olkt-head .olkt-chev{flex:none;width:6px;height:6px;border-right:1.5px solid currentColor;border-bottom:1.5px solid currentColor;transform:rotate(45deg) translate(-2px,-1px)}`);
    out.push(`.olkt-head[aria-expanded="false"] .olkt-chev{transform:rotate(-45deg)}`);
    out.push(`.olkt-head .olkt-lbl{overflow:hidden;text-overflow:ellipsis;white-space:nowrap}`);
    out.push(`.olkt-head .olkt-sum{margin-left:auto;margin-right:8px}`);
    out.push(`.olkt-head .olkt-sum:not(:empty){min-width:18px;height:18px;padding:0 6px;box-sizing:border-box;border-radius:9px;display:inline-flex;align-items:center;justify-content:center;font-size:11px;font-weight:700;letter-spacing:0;color:var(--colorNeutralForegroundOnBrand,#fff);background:var(--colorBrandBackground,#0f6cbd)}`);
    out.push(`.olkt-head .olkt-stale{flex:none;margin-left:6px;padding:0 4px;border:1px solid currentColor;border-radius:4px;font-size:9.5px;line-height:13px;font-weight:600;letter-spacing:0;text-transform:none;color:var(--colorStatusWarningForeground1,#f7a35c)}`);
    out.push(`.olkt-compose{display:flex;gap:6px;padding:2px 8px 8px 4px}`);
    out.push(`.olkt-compose button{all:unset;cursor:pointer;flex:1;display:flex;align-items:center;justify-content:center;gap:5px;font-size:12px;font-weight:600;padding:5px 0;border-radius:4px;color:#fff}`);
    out.push(`.olkt-compose button i{font-size:16px;line-height:1;color:#fff;display:inline-flex}`);
    out.push(`.olkt-compose button:hover{filter:brightness(1.15)}`);
    out.push(`.olkt-compose button:focus-visible{outline:2px solid var(--colorBrandStroke1,#4aa3f0);outline-offset:1px}`);
    out.push(`.olkt-menu{position:fixed;z-index:100000;min-width:190px;padding:4px;background:var(--colorNeutralBackground1,#292929);border:1px solid var(--colorNeutralStroke1,#474747);border-radius:6px;box-shadow:0 8px 24px rgba(0,0,0,.45);font-size:13px;color:var(--colorNeutralForeground1,#e6e6e6)}`);
    out.push(`.olkt-menu button{all:unset;box-sizing:border-box;display:block;width:100%;padding:6px 12px;border-radius:4px;cursor:pointer;white-space:nowrap}`);
    out.push(`.olkt-menu button:hover,.olkt-menu button:focus-visible{background:var(--colorNeutralBackground1Hover,#383838)}`);
    out.push(`.olkt-menu button:disabled{opacity:.4;cursor:default}`);
    out.push(`.olkt-menu .olkt-sep{height:1px;margin:4px 6px;background:var(--colorNeutralStroke2,#3a3a3a)}`);
    out.push(`.olkt-menu .olkt-sub{padding:6px 12px 2px;font-size:11px;letter-spacing:.06em;color:var(--colorNeutralForeground3,#a3a3a3)}`);
    out.push(`.olkt-agenda{position:absolute;left:0;right:2px;bottom:0;z-index:15;box-sizing:border-box;display:flex;flex-direction:column;padding:0 8px 6px 6px;font-size:12px;color:var(--colorNeutralForeground1,#e6e6e6)}`);
    // Outlook hides the pane by sliding its own tree out; ours would stay behind.
    out.push(`body:has(button[aria-label="Show navigation pane"]) ${PANE} :is(.olkt-agenda,.olkt-resize){display:none}`);
    out.push(`.olkt-agenda::before{content:"";flex:none;height:1px;margin:0 2px 0 6px;background:var(--colorNeutralStroke2,#3a3a3a)}`);
    out.push(`.olkt-agenda-grip{position:absolute;left:0;right:0;top:-3px;height:7px;cursor:row-resize;z-index:2}`);
    out.push(`.olkt-agenda-grip:hover{background:var(--colorBrandStroke1,#4aa3f0);opacity:.5}`);
    out.push(`.olkt-feed{flex:1;min-height:0;overflow-y:auto;scrollbar-width:thin;scrollbar-color:transparent transparent}`);
    out.push(`.olkt-feed:hover{scrollbar-color:var(--colorNeutralStroke1,#555) transparent}`);
    out.push(`.olkt-agenda .olkt-head.olkt-allday{height:22px;margin-left:12px;width:calc(100% - 12px);font-size:11px;letter-spacing:0;font-weight:400}`);
    out.push(`.olkt-agenda .olkt-head{margin-top:2px}`);
    out.push(`.olkt-month{flex:none;padding:4px 0 6px 12px}`);
    out.push(`.olkt-month-bar{display:flex;align-items:center;justify-content:space-between;padding:0 0 4px}`);
    out.push(`.olkt-month-title{all:unset;cursor:pointer;flex:1;text-align:center;font-size:12px;font-weight:700;color:var(--colorNeutralForeground1,#e6e6e6)}`);
    out.push(`.olkt-month-nav{all:unset;cursor:pointer;width:22px;height:20px;border-radius:4px;text-align:center;font-size:16px;line-height:18px;color:var(--colorNeutralForeground3,#a3a3a3)}`);
    out.push(`.olkt-month-nav:hover,.olkt-month-title:hover{color:var(--colorNeutralForeground1,#fff);background:var(--colorNeutralBackground1Hover,#333)}`);
    out.push(`.olkt-cell{all:unset;cursor:pointer;text-align:center}`);
    out.push(`.olkt-cell:hover{outline:1px solid var(--colorNeutralStroke1,#666)}`);
    out.push(`.olkt-cell.picked{outline:1.5px solid var(--colorBrandStroke1,#4aa3f0)}`);
    out.push(`.olkt-back{all:unset;cursor:pointer;margin:6px 0 0 12px;font-size:11px;color:var(--colorBrandForeground1,#4aa3f0)}`);
    out.push(`.olkt-month-nav:focus-visible,.olkt-month-title:focus-visible,.olkt-cell:focus-visible,.olkt-back:focus-visible{outline:2px solid var(--colorBrandStroke1,#4aa3f0)}`);
    out.push(`.olkt-month-grid{display:grid;grid-template-columns:repeat(7,1fr);gap:2px;text-align:center;font-size:11px;font-variant-numeric:tabular-nums}`);
    out.push(`.olkt-dow{color:var(--colorNeutralForeground3,#a3a3a3);font-size:10px}`);
    out.push(`.olkt-cell{border-radius:4px;line-height:20px;color:var(--colorNeutralForeground1,#e6e6e6)}`);
    out.push(`.olkt-cell.h1{background:rgba(51,173,173,.18)}`);
    out.push(`.olkt-cell.h2{background:rgba(51,173,173,.34)}`);
    out.push(`.olkt-cell.h3{background:rgba(51,173,173,.52)}`);
    out.push(`.olkt-cell.h4{background:rgba(51,173,173,.72)}`);
    out.push(`.olkt-cell.out{color:var(--colorNeutralForeground4,#6b6b6b)}`);
    out.push(`.olkt-cell.today{font-weight:800;color:var(--colorNeutralForeground1,#fff);box-shadow:inset 0 0 0 1.5px var(--colorNeutralForeground1,#fff)}`);
    out.push(`.olkt-agenda .olkt-day{position:sticky;top:0;z-index:1;display:flex;align-items:center;gap:8px;margin:10px 0 2px 12px;padding:3px 0;background:var(--olkt-pane-bg,#1f1f1f);font-size:10.5px;font-weight:700;letter-spacing:.08em;text-transform:uppercase;color:var(--colorBrandForeground1,#4aa3f0)}`);
    out.push(`.olkt-agenda .olkt-day::after{content:"";flex:1;height:1px;background:var(--colorNeutralStroke1,#474747)}`);
    out.push(`.olkt-ev{display:grid;grid-template-columns:3px 1fr auto;gap:1px 8px;align-items:center;margin:4px 0 0 12px;line-height:1.3}`);
    out.push(`.olkt-ev::before{content:"";grid-row:1 / span 2;align-self:stretch;border-radius:2px;background:var(--olkt-ev-color,#4aa3f0)}`);
    out.push(`.olkt-ev b{font-weight:600;overflow:hidden;text-overflow:ellipsis;white-space:nowrap;grid-column:2}`);
    out.push(`.olkt-repwrap{display:inline-flex;vertical-align:-2px;margin-right:4px;color:var(--colorNeutralForeground3,#a3a3a3)}`);
    out.push(`.olkt-rep{width:12px;height:12px;fill:none;stroke:currentColor;stroke-width:1.4;stroke-linecap:round;stroke-linejoin:round}`);
    out.push(`.olkt-ev small{grid-column:2;color:var(--colorNeutralForeground3,#a3a3a3);font-size:11px;font-variant-numeric:tabular-nums;overflow:hidden;text-overflow:ellipsis;white-space:nowrap}`);
    out.push(`.olkt-ev.free b{font-weight:400;color:var(--colorNeutralForeground2,#c8c8c8)}`);
    out.push(`.olkt-join{all:unset;cursor:pointer;grid-column:3;grid-row:1 / span 2;width:22px;height:22px;margin-right:6px;border-radius:50%;display:inline-flex;align-items:center;justify-content:center;background:#5b5fc7}`);
    out.push(`.olkt-join svg{width:13px;height:13px;fill:#fff}`);
    out.push(`.olkt-join[data-kind="Zoom"]{background:#0b5cff}`);
    out.push(`.olkt-join[data-kind="Meet"]{background:#1e8e3e}`);
    out.push(`.olkt-join[data-kind="Webex"]{background:#07a0a8}`);
    out.push(`.olkt-join:hover{filter:brightness(1.15)}`);
    out.push(`.olkt-join:focus-visible{outline:2px solid var(--colorBrandStroke1,#4aa3f0)}`);
    out.push(`.olkt-join.soon{box-shadow:0 0 0 2px var(--olkt-pane-bg,#1f1f1f),0 0 0 4px #6bd18a}`);
    out.push(`.olkt-agenda .olkt-none{margin:4px 0 0 12px;color:var(--colorNeutralForeground3,#a3a3a3)}`);
    out.push(`.olkt-rename{position:fixed;z-index:100001;box-sizing:border-box;font:inherit;font-size:13px;padding:2px 6px;color:var(--colorNeutralForeground1,#e6e6e6);background:var(--colorNeutralBackground3,#1f1f1f);border:1px solid var(--colorBrandStroke1,#4aa3f0);border-radius:4px;outline:none}`);
    calendarCss(out);
    readingCss(out);
    printCss(out);
    otherCss(out);
    // Ribbon: drop the group-name row (New, Delete, Respond...) under the buttons.
    // Every ribbon container has a fixed height (92px inside, 100px outside); take the
    // label row's 16px off each so the space is really freed.
    if (cfg.hideRibbonGroupLabels !== false) {
      // Scoped to #RibbonRoot: the class prefixes are generic enough to exist elsewhere.
      out.push(`#RibbonRoot [role="group"] [class*="groupLabelContainer-"]{display:none!important}`);
      out.push(`#RibbonRoot :is(#BottomBar,#BottomBar > div,[role="toolbar"][class*="outerContainer-"],[role="toolbar"][class*="outerContainer-"] [role="tabpanel"],[class*="tabContentContainer-"],[class*="tabContentContainer-"] > div,[class*="resizeGroup-"],[class*="resizeGroup-"] > div){height:84px!important;min-height:0!important}`);
      out.push(`#RibbonRoot :is([class*="resizeGroup-"] > div > div,[class*="bottomBarContainer-"],[class*="groupContainer-"],[class*="groupContainer-"] > div,[role="group"]:has([class*="groupLabelContainer-"]),[role="group"] > [class*="container-"]){height:76px!important;min-height:0!important}`);
    }
    return out.join('\n');
  }

  // ==== favorites list, compose bar, headers ====
  // Only through Outlook's own Favorites item: guessing from a folder name can
  // land in an account tree, which has folders with the same names.
  function favoritesList() {
    const fav = document.querySelector('[role="treeitem"][aria-level="1"][data-folder-name="favorites"]');
    const list = fav && fav.nextElementSibling && fav.nextElementSibling.firstElementChild;
    return list && list.querySelector(':scope > div > [role="treeitem"]') ? list : null;
  }

  function onPress(el, fn) {
    // Act on press, not click: Outlook's focus manager re-renders the tree
    // between press and release, so a click event often never arrives.
    el.addEventListener('pointerdown', ev => {
      if (ev.button !== 0) return;
      ev.preventDefault();
      ev.stopPropagation();
      fn();
    });
    el.addEventListener('click', ev => ev.stopPropagation());
    el.addEventListener('keydown', ev => {
      if (ev.key === 'Enter' || ev.key === ' ') { ev.preventDefault(); ev.stopPropagation(); fn(); }
    });
  }

  // Select the account's inbox, wait for Outlook to switch, then press New:
  // new mail uses the account of the folder being viewed, so From starts right.
  async function composeFrom(folder) {
    const target = document.querySelector(row(folder));
    if (!target) return;
    if (target.getAttribute('aria-selected') !== 'true') {
      target.click();
      for (let i = 0; i < 40 && target.getAttribute('aria-selected') !== 'true'; i++) await sleep(50);
      await sleep(250);
    }
    const newBtn = document.querySelector('[data-app-section="Ribbon"] button[aria-label="New"]');
    if (newBtn) newBtn.click();
  }

  // Writes that skip unchanged values: each real write wakes the MutationObserver again.
  function setText(n, text) { if (n.textContent !== text) n.textContent = text; }
  function setAttr(n, name, value) { if (n.getAttribute(name) !== value) n.setAttribute(name, value); }

  function ensureCompose(list) {
    if (!cfg.compose || !cfg.compose.length) return;
    const existing = list.querySelector(':scope > .olkt-compose');
    if (existing) {
      if (!existing.querySelector('button:not(:has(> i))')) return; // every button has its icon
      // At startup the ribbon may not have its icon yet; add it once it exists.
      const late = document.querySelector('[data-app-section="Ribbon"] button[aria-label="New"] i[data-icon-name]');
      if (late) existing.querySelectorAll('button').forEach(b => { if (!b.querySelector('i')) b.prepend(late.cloneNode(true)); });
      return;
    }
    const bar = document.createElement('div');
    bar.className = 'olkt-compose';
    // Same compose icon as Outlook's own New button.
    const icon = document.querySelector('[data-app-section="Ribbon"] button[aria-label="New"] i[data-icon-name]');
    for (const c of cfg.compose) {
      const b = document.createElement('button');
      b.type = 'button';
      if (icon) b.appendChild(icon.cloneNode(true));
      const t = document.createElement('span');
      t.textContent = c.label;
      b.appendChild(t);
      b.title = `New email from ${c.account}`;
      b.style.background = c.color;
      // Resolved on press: the primary account is read from the folder tree.
      onPress(b, () => composeFrom(inboxOf(c.account)));
      bar.appendChild(b);
    }
    list.appendChild(bar);
  }

  // The count Outlook shows on each row, summed across a group, its subgroups and its
  // pattern-matched folders, from Outlook's own row titles ("... - 42 items (3 unread)").
  // Drafts and Outbox show their item count (Outlook's "[2]") rather than unread.
  const ITEM_COUNT_FOLDER = /^(drafts|outbox)\b/i;
  function unreadIn(group) {
    const names = [];
    const collect = g => {
      for (const n of g.items) { if (isGroup(n)) collect(n); else names.push(n.folder); }
      for (const m of matchedIn(g)) names.push(m.name);
    };
    collect(group);
    let total = 0;
    for (const name of names) {
      const el = document.querySelector(favRow(name));
      const title = (el && el.getAttribute('title')) || '';
      let m = ITEM_COUNT_FOLDER.test(name) ? /([\d,]+) items?\b/.exec(title) : /\(([\d,]+) unread\)/.exec(title);
      // Fallback for item counts: the "[2]" Outlook draws on the row itself.
      if (!m && el && ITEM_COUNT_FOLDER.test(name)) {
        const badge = [...el.querySelectorAll('span, div')].find(x => x.children.length === 0 && /^\[[\d,]+\]$/.test(x.textContent.trim()));
        m = badge && /\[([\d,]+)\]/.exec(badge.textContent);
      }
      if (m) total += Number(m[1].replace(/,/g, ''));
    }
    return total;
  }

  function toggleFold(id) {
    folds[id] = !folds[id];
    try { store.setItem(FOLD_KEY, JSON.stringify(folds)); } catch {}
    render();
  }

  function ensureHeaders(list) {
    const live = new Set();
    for (const { n: g } of groups()) {
      live.add(g.id);
      let h = list.querySelector(`:scope > .olkt-head[data-group=${q(g.id)}]`);
      if (!h) {
        h = document.createElement('button');
        h.type = 'button';
        h.className = 'olkt-head';
        h.dataset.group = g.id;
        // Outlook enforces Trusted Types, so no innerHTML.
        for (const cls of ['olkt-chev', 'olkt-lbl', 'olkt-sum']) {
          const s = document.createElement('span');
          s.className = cls;
          h.appendChild(s);
        }
        const id = g.id;
        onPress(h, () => toggleFold(id));
        h.addEventListener('contextmenu', ev => {
          ev.preventDefault();
          ev.stopPropagation();
          const grp = findGroup(id);
          if (grp) groupMenu(grp, h, ev.clientX, ev.clientY);
        });
        list.appendChild(h);
      }
      setText(h.querySelector('.olkt-lbl'), g.label);
      setAttr(h, 'title', 'Right-click for group options');
      setAttr(h, 'aria-expanded', String(!folds[g.id]));
      const sum = folds[g.id] ? unreadIn(g) : 0;
      setText(h.querySelector('.olkt-sum'), sum ? String(sum) : '');
    }
    list.querySelectorAll(':scope > .olkt-head').forEach(h => { if (!live.has(h.dataset.group)) h.remove(); });
  }

  // Where the last drag was released over Favorites. Outlook appends a newly
  // dropped favorite at the end of its own list, so we remember the spot instead.
  let lastDrop = null;
  function onListRelease(ev) {
    const el = document.elementFromPoint(ev.clientX, ev.clientY);
    const head = el && el.closest('.olkt-head');
    const item = el && el.closest('[role="treeitem"]');
    const target = head ? findGroup(head.dataset.group) : item ? findFolder(item.getAttribute('data-folder-name')) : null;
    // The names before the release: only a favorite that appears after it was dropped.
    lastDrop = target ? { target, at: Date.now(), before: new Set(favoriteNames()) } : null;
  }

  // Header above the first account tree. Inserted next to Outlook's nodes, never moving them.
  function ensureAccountsHeader() {
    if (!cfg.accountsHeading) return;
    const root = document.querySelector(ACCT_ROOT);
    const wrapEl = root && root.parentElement;
    if (!wrapEl || !wrapEl.parentElement) return;
    let h = wrapEl.parentElement.querySelector(':scope > .olkt-accounts');
    if (!h) {
      h = document.createElement('button');
      h.type = 'button';
      h.className = 'olkt-head olkt-accounts';
      for (const cls of ['olkt-chev', 'olkt-lbl']) {
        const s = document.createElement('span');
        s.className = cls;
        h.appendChild(s);
      }
      onPress(h, () => toggleFold(ACCOUNTS));
      wrapEl.parentElement.insertBefore(h, wrapEl);
    }
    setText(h.querySelector('.olkt-lbl'), cfg.accountsHeading.toUpperCase());
    setAttr(h, 'aria-expanded', String(!folds[ACCOUNTS]));
  }

  // Drag an account root up or down under "All folders" to reorder. A press
  // without movement stays a normal click (expand or collapse).
  let acctListBound = null;
  function ensureAccountDrag() {
    const root = document.querySelector(ACCT_ROOT);
    const listEl = root && root.parentElement && root.parentElement.parentElement;
    if (!listEl || acctListBound === listEl) return;
    if (acctListBound) acctListBound.removeEventListener('pointerdown', onAcctPress, true);
    acctListBound = listEl;
    listEl.addEventListener('pointerdown', onAcctPress, true);
  }

  function onAcctPress(ev) {
    if (ev.button !== 0) return;
    const item = ev.target.closest(ACCT_ROOT);
    if (!item) return;
    const listEl = acctListBound;
    const startY = ev.clientY;
    let line = null;
    let dropIndex = -1;
    const roots = () => [...listEl.querySelectorAll(`:scope > div > ${ACCT_ROOT}`)]
      .filter(r => r.getBoundingClientRect().height > 0)
      .sort((a, b) => a.getBoundingClientRect().top - b.getBoundingClientRect().top);
    const move = e => {
      if (!line && Math.abs(e.clientY - startY) < 6) return;
      if (!line) {
        line = document.createElement('div');
        line.className = 'olkt-acct-drop';
        document.body.appendChild(line);
        listEl.classList.add('olkt-dragging');
      }
      const rs = roots();
      dropIndex = rs.findIndex(r => { const b = r.parentElement.getBoundingClientRect(); return e.clientY < b.top + b.height / 2; });
      if (dropIndex < 0) dropIndex = rs.length;
      const ref = rs[Math.min(dropIndex, rs.length - 1)].parentElement.getBoundingClientRect();
      const y = dropIndex < rs.length ? ref.top : ref.bottom;
      const lb = listEl.getBoundingClientRect();
      Object.assign(line.style, { left: lb.left + 8 + 'px', width: lb.width - 16 + 'px', top: y - 1 + 'px' });
    };
    const up = () => {
      document.removeEventListener('pointermove', move, true);
      document.removeEventListener('pointerup', up, true);
      if (!line) return;
      line.remove();
      listEl.classList.remove('olkt-dragging');
      // Swallow the click the release would otherwise turn into.
      document.addEventListener('click', e => { e.stopPropagation(); e.preventDefault(); }, { capture: true, once: true });
      const order = roots().map(r => r.getAttribute('data-folder-name'));
      const dragged = item.getAttribute('data-folder-name');
      const from = order.indexOf(dragged);
      if (from < 0) return;
      order.splice(from, 1);
      order.splice(dropIndex > from ? dropIndex - 1 : dropIndex, 0, dragged);
      const next = {};
      for (const a of order) next[a] = (cfg.accounts || {})[a] || '';
      for (const [a, label] of Object.entries(cfg.accounts || {})) if (!(a in next)) next[a] = label;
      cfg.accounts = next;
      save();
    };
    document.addEventListener('pointermove', move, true);
    document.addEventListener('pointerup', up, true);
  }

  // A favorite dropped onto Favorites joins the spot where it was dropped: after
  // the folder it was released on, or first inside the group header it was
  // released on. Only right after a real drop: while Outlook boots, its list
  // briefly shows placeholder folder names that must not be adopted. A favorite
  // added another way, or there before the drop, stays at the top until moved
  // (Shift+right-click).
  function adoptNewFavorites(list) {
    const drop = lastDrop && Date.now() - lastDrop.at < 15000 ? lastDrop.target : null;
    if (!drop || !cfg.favorites.length) return false;
    const before = lastDrop.before;
    const hidden = new Set(cfg.hide);
    let prev = null;
    let changed = false;
    for (const wrapEl of list.children) {
      const item = wrapEl.firstElementChild;
      if (!item || item.getAttribute('role') !== 'treeitem') continue;
      const folder = item.getAttribute('data-folder-name');
      if (!folder || hidden.has(folder) || matchesAnyGroup(folder)) continue;
      let node = findFolder(folder);
      if (!node && !before.has(folder)) {
        node = { folder };
        if (isGroup(drop)) {
          drop.items.unshift(node);
          if (folds[drop.id]) folds[drop.id] = false;
        } else {
          const at = locate(drop || prev);
          if (at) at.siblings.splice(at.index + 1, 0, node);
          else cfg.favorites.push(node);
        }
        lastDrop = null;
        changed = true;
      }
      prev = node;
    }
    return changed;
  }

  // ==== pane resize ====
  function ensureResize() {
    if (!S.resizablePane) return;
    const pane = document.querySelector(PANE);
    if (!pane || pane.querySelector(':scope > .olkt-resize')) return;
    const handle = document.createElement('div');
    handle.className = 'olkt-resize';
    handle.title = 'Drag to resize; double-click to reset';
    handle.addEventListener('pointerdown', ev => {
      if (ev.button !== 0) return;
      ev.preventDefault();
      ev.stopPropagation();
      handle.setPointerCapture(ev.pointerId);
      handle.classList.add('dragging');
      const startX = ev.clientX;
      // From Outlook's own width the first time; the width rule starts with the drag.
      if (!paneWidth) { paneWidth = Math.round(pane.getBoundingClientRect().width); render(); }
      const startW = paneWidth;
      const move = e => {
        paneWidth = Math.max(140, Math.min(420, Math.round(startW + e.clientX - startX)));
        document.documentElement.style.setProperty('--olkt-pane-w', paneWidth + 'px');
      };
      const up = () => {
        handle.classList.remove('dragging');
        handle.removeEventListener('pointermove', move);
        try { store.setItem(WIDTH_KEY, String(paneWidth)); } catch {}
      };
      handle.addEventListener('pointermove', move);
      handle.addEventListener('pointerup', up, { once: true });
      handle.addEventListener('pointercancel', up, { once: true });
    });
    handle.addEventListener('dblclick', () => {
      paneWidth = cfg.paneWidth;
      try { store.removeItem(WIDTH_KEY); } catch {}
      render();
    });
    pane.appendChild(handle);
  }

  // ==== calendar and Other inbox through Outlook's own service ====
  // The same JSON calls Outlook's page makes to its mail service, sent with the headers
  // kept above. Property names are EWS field URIs; one unknown name fails a whole call.
  const OWA_HEADER = { __type: 'JsonRequestHeaders:#Exchange', RequestServerVersion: 'V2018_01_08', TimeZoneContext: { __type: 'TimeZoneContext:#Exchange', TimeZoneDefinition: { __type: 'TimeZoneDefinitionType:#Exchange', Id: 'UTC' } } };
  const PROP = f => ({ __type: 'PropertyUri:#Exchange', FieldURI: f });
  let owaCount = 0;
  // multi: a batch whose per-item results the caller checks itself.
  // The handful of actions that answer with a bare object rather than the usual envelope.
  const BARE_REPLY = new Set(['GetCalendarFolders']);
  async function owa(acct, action, body, multi) {
    const res = await session.call(acct.anchor, action, JSON.stringify({ __type: action + 'JsonRequest:#Exchange', Header: OWA_HEADER, Body: body }), ++owaCount);
    if (!res.ok) throw new Error(action + ' failed (' + res.status + ')');
    const json = await res.json();
    const out = (json && json.Body) || (BARE_REPLY.has(action) ? json : null);
    if (!out) throw new Error(action + ': empty reply');
    if (out.ErrorCode || out.ExceptionName) throw new Error(action + ': ' + (out.ExceptionName || out.ErrorCode));
    const msg = out.ResponseMessages && out.ResponseMessages.Items ? out.ResponseMessages.Items[0] : out;
    if (!multi && msg && msg.ResponseClass && msg.ResponseClass !== 'Success') throw new Error(action + ': ' + (msg.MessageText || msg.ResponseCode || msg.ResponseClass));
    return out;
  }

  // Outlook's accounts: address, the mailbox id its requests are routed by, our label and
  // the compose button's color. Primary first, then the saved account order.
  function accountMap() {
    const seen = new Set(), out = [];
    for (let i = 0; i < localStorage.length; i++) {
      const k = localStorage.key(i);
      if (!k || !k.startsWith('olk-Microsoft.AccountStorage.Coprincipal.Item.')) continue;
      try {
        const a = JSON.parse(localStorage.getItem(k));
        const address = a && a.emailAddress && a.routingHint ? lower(a.emailAddress) : '';
        if (address && !seen.has(address)) { seen.add(address); out.push({ address, anchor: String(a.routingHint) }); }
      } catch {}
    }
    const order = Object.keys(cfg.accounts), p = primary();
    const rank = a => a.address === p ? -1 : order.indexOf(a.address) < 0 ? order.length : order.indexOf(a.address);
    out.sort((a, b) => rank(a) - rank(b) || a.address.localeCompare(b.address));
    for (const a of out) {
      a.label = labels[a.address] || a.address.split('@')[0];
      a.color = (cfg.compose.find(c => c.account === a.address) || {}).color;
    }
    return out;
  }

  // Join links: a meeting link is trusted by its host (and, for Teams and Zoom, its path),
  // never by a provider's name elsewhere in the URL; punctuation that plain text leaves on
  // the end is dropped. Sources: the server's own field, the location or preview, else
  // (agenda days only) the invite's text, read in one batch.
  const MEETING_HOSTS = [['teams.microsoft.com', 'Teams'], ['teams.live.com', 'Teams'], ['zoom.us', 'Zoom'], ['meet.google.com', 'Meet'], ['webex.com', 'Webex']];
  const URLS_IN_TEXT = /https:\/\/[^\s"'<>]+/g;
  function meetingLink(text) {
    for (const raw of String(text || '').match(URLS_IN_TEXT) || []) {
      let u;
      try { u = new URL(raw.replace(/[.,;:)\]]+$/, '')); } catch { continue; }
      const host = u.hostname.toLowerCase();
      const hit = MEETING_HOSTS.find(([h]) => host === h || host.endsWith('.' + h));
      if (!hit) continue;
      if (hit[1] === 'Teams' && !/^\/(l\/meetup-join|meet)\//.test(u.pathname)) continue;
      if (hit[1] === 'Zoom' && !/^\/[jw]\//.test(u.pathname)) continue;
      return { url: u.href, kind: hit[1] };
    }
    return undefined;
  }
  const joinIn = (...texts) => { for (const t of texts) { const j = meetingLink(t); if (j) return j; } return undefined; };
  // Outlook's 25 category colours, in the order its colour index uses.
  const CATEGORY_COLORS = ('#d13438 #ca5010 #8e562e #c19c00 #498205 #038387 #5c7e10 #0078d4 #8764b8 #c30052 ' +
    '#69797e #394146 #7a7574 #525252 #000000 #750b1c #8e3b05 #4d291c #817400 #0b6a0b ' +
    '#005b70 #3a4d00 #004e8c #5c2e91 #77004d').split(' ');
  const categoryColor = (acct, categories) => {
    const palette = session.palette(acct.anchor);
    if (!palette || !categories) return '';
    for (const name of categories) {
      const i = palette.get(name);
      if (i >= 0 && i < CATEGORY_COLORS.length) return CATEGORY_COLORS[i];
    }
    return '';
  };

  const calendarCache = new Map(); // address -> { byFolder, fallback, at }
  const CAL_COLOR_TTL = 1800000;
  async function calendarColors(acct) {
    const cached = calendarCache.get(acct.address);
    if (cached && Date.now() - cached.at < (cached.failed ? 300000 : CAL_COLOR_TTL)) return cached.failed ? null : cached;
    let j;
    try { j = await owa(acct, 'GetCalendarFolders', {}); }
    catch (err) {
      // Remembered for five minutes so every refresh does not retry a call that is failing.
      calendarCache.set(acct.address, { failed: true, at: Date.now(), byFolder: new Map(), fallback: '' });
      throw err;
    }
    const byFolder = new Map();
    let fallback = '';
    for (const g of j.CalendarGroups || []) {
      for (const c of g.Calendars || []) {
        const id = c.CalendarFolderId && c.CalendarFolderId.Id;
        const hex = String(c.HexColorString || '');
        if (!/^#[0-9a-f]{6}$/i.test(hex) || hex === '#000000') continue;
        if (id) byFolder.set(id, hex);
        if (c.IsDefaultCalendar) fallback = hex;
      }
    }
    const rec = { byFolder, fallback, at: Date.now() };
    calendarCache.set(acct.address, rec);
    return rec;
  }

  const CAL_PROPS = ['calendar:IsAllDayEvent', 'calendar:IsCancelled', 'calendar:IsSeriesCancelled', 'calendar:Location', 'calendar:IsOnlineMeeting', 'calendar:JoinOnlineMeetingUrl', 'calendar:StartWallClock', 'calendar:EndWallClock', 'item:Categories', 'item:Preview', 'calendar:UID', 'item:ParentFolderId'].map(PROP);
  const dateOnly = s => String(s).slice(0, 10) + 'T00:00:00Z'; // all-day events: the item's own date
  const instant = s => new Date(s).toISOString();
  // One account's events between from and to (occurrences expanded), as the agenda draws them.
  async function readAccount(acct, from, to, joinsUntil) {
    // Outlook colours an event by its calendar; a failure here only costs the colours.
    const colors = await calendarColors(acct).catch(() => null);
    const found = await owa(acct, 'FindItem', { ItemShape: { __type: 'ItemResponseShape:#Exchange', BaseShape: 'Default', AdditionalProperties: CAL_PROPS }, ParentFolderIds: [{ __type: 'DistinguishedFolderId:#Exchange', Id: 'calendar' }], Traversal: 'Shallow', Paging: { __type: 'CalendarPageView:#Exchange', StartDate: from.toISOString(), EndDate: to.toISOString() } });
    const msg = found.ResponseMessages && found.ResponseMessages.Items && found.ResponseMessages.Items[0];
    const root = msg && msg.RootFolder;
    const events = [], bodies = [];
    const now = new Date();
    for (const i of (root && root.Items) || []) {
      if (i.IsCancelled || i.IsSeriesCancelled || i.ResponseType === 'Decline') continue;
      const allDay = !!i.IsAllDayEvent;
      const place = (i.Location && i.Location.DisplayName) || '';
      const e = {
        id: i.ItemId && i.ItemId.Id,
        uid: i.UID,
        subject: i.Subject || '(no title)',
        start: allDay ? dateOnly(i.StartWallClock || i.Start) : instant(i.Start),
        end: allDay ? dateOnly(i.EndWallClock || i.End) : instant(i.End),
        allDay, free: i.FreeBusyType === 'Free', account: acct.address, label: acct.label,
        color: categoryColor(acct, i.Categories)
          || (colors && (colors.byFolder.get(i.ParentFolderId && i.ParentFolderId.Id) || colors.fallback))
          || acct.color,
        repeating: i.CalendarItemType === 'Occurrence' || i.CalendarItemType === 'Exception',
        categories: i.Categories && i.Categories.length ? i.Categories : undefined,
        where: meetingLink(place) || /^microsoft teams meeting$/i.test(place) ? '' : place,
        join: joinIn(i.JoinOnlineMeetingUrl, place, i.Preview),
      };
      if (!e.join && joinsUntil && e.id && !allDay && (i.IsOnlineMeeting || /teams|zoom|webex|meet\b/i.test(place))
        && new Date(e.start) < joinsUntil && new Date(e.end) > now) bodies.push(e);
      events.push(e);
    }
    if (!session.palette(acct.anchor) && events.some(e => e.categories && e.categories.length)) {
      // The next read paints them; this one keeps the calendar colours it already has.
      session.palettes(acct.anchor).then(n => { if (n && current()) ensureAgendaData(true); }, () => {});
    }
    if (bodies.length) {
      // The invite text holds the link when the field above is empty (most Teams invites).
      // A failure here only loses join buttons, never the events.
      try {
        const got = await owa(acct, 'GetItem', { ItemIds: bodies.slice(0, 20).map(e => ({ __type: 'ItemId:#Exchange', Id: e.id })), ItemShape: { __type: 'ItemResponseShape:#Exchange', BaseShape: 'IdOnly', BodyType: 'Text', AdditionalProperties: [PROP('item:Body')] } }, true);
        ((got.ResponseMessages && got.ResponseMessages.Items) || []).forEach((m, k) => {
          const item = m.ResponseClass === 'Success' && m.Items && m.Items[0];
          if (item && bodies[k]) bodies[k].join = joinIn(item.Body && item.Body.Value);
        });
      } catch {}
    }
    return events;
  }

  // Every account's events between from and to: declined and cancelled ones dropped, the
  // same meeting kept on two calendars shown once. An account Outlook has not signed a call
  // for yet is reported as waiting; a failed read is reported too.
  // Copies of one meeting on two mailboxes carry different UIDs, so the match is subject,
  // start and end together; two different items on ONE calendar are both kept, even when
  // they share a subject and start.
  const eventKey = e => e.subject + '|' + e.start + '|' + e.end;
  async function readEvents(from, to, joinsUntil) {
    const accounts = accountMap();
    const events = [], errors = [], failed = [];
    let asked = 0, succeeded = 0;
    await Promise.all(accounts.map(async acct => {
      if (!session.has(acct.anchor)) { failed.push(acct.address); errors.push(acct.label + ': waiting for Outlook'); return; }
      asked++;
      try { events.push(...await readAccount(acct, from, to, joinsUntil)); succeeded++; }
      catch (err) { failed.push(acct.address); errors.push(acct.label + ': ' + err.message); }
    }));
    const order = accounts.map(a => a.address);
    events.sort((a, b) => a.start.localeCompare(b.start) || order.indexOf(a.account) - order.indexOf(b.account));
    const first = new Map(); // subject|start|end -> the copy being shown
    const seen = new Set();   // account + occurrence, so one occurrence never lands twice
    const out = [];
    for (const e of events) {
      // A recurring series shares its UID, so the occurrence is the item plus its start.
      const occurrence = e.account + '|' + (e.uid || e.id || '') + '|' + e.start;
      if (e.uid || e.id) { if (seen.has(occurrence)) continue; seen.add(occurrence); }
      const kept = first.get(eventKey(e));
      if (kept && kept.account !== e.account) {
        // The same meeting on another calendar: the first copy stays, and takes what it lacks.
        for (const f of ['join', 'where', 'categories']) if (!kept[f] && e[f]) kept[f] = e[f];
        continue;
      }
      if (!kept) first.set(eventKey(e), e);
      out.push(e);
    }
    return { events: out, errors, failed, asked, succeeded, broke: asked > succeeded };
  }

  // Busy minutes per local day (timed events not marked free), clipped to each day.
  function heatFor(events, from, to) {
    const heat = {};
    for (const e of events) {
      if (e.allDay || e.free) continue;
      let s = new Date(e.start);
      const end = new Date(e.end);
      while (s < end) {
        const dayEnd = new Date(s.getFullYear(), s.getMonth(), s.getDate() + 1);
        const stop = end < dayEnd ? end : dayEnd;
        if (s >= from && s < to) heat[ymd(s)] = (heat[ymd(s)] || 0) + Math.round((stop - s) / 60000);
        s = stop;
      }
    }
    return heat;
  }

  const AGENDA_REFRESH = 300000;
  const addHeat = (into, heat) => { for (const [k, v] of Object.entries(heat || {})) into[k] = (into[k] || 0) + v; return into; };
  // The upcoming feed (today plus the next days) and, with the month grid on, this month's
  // events and heat, in one read. An account that fails keeps its events and heat from the
  // previous agenda; the result is marked stale and read again a minute later.
  async function fetchAgenda(prev) {
    const now = new Date();
    const days = cfg.agendaDays;
    const end = new Date(now); end.setDate(end.getDate() + days); end.setHours(0, 0, 0, 0);
    const monthStart = new Date(now.getFullYear(), now.getMonth(), 1);
    const monthEnd = new Date(now.getFullYear(), now.getMonth() + 1, 1);
    const month = ym(monthStart);
    const wholeMonth = !!cfg.monthHeatMap;
    const from = wholeMonth ? monthStart : dayStart(now);
    const to = wholeMonth && monthEnd > end ? monthEnd : end;
    const { events, errors, failed, succeeded, broke } = await readEvents(from, to, end);
    if (!succeeded) throw new Error(errors.join('; ') || 'waiting for Outlook');
    const today = ymd(now);
    const live = e => new Date(e.start) < end && (e.allDay ? e.end.slice(0, 10) > today : new Date(e.end) >= now);
    // Heat per account, so a failed account's last known heat can stand in for it.
    const heatBy = {};
    if (wholeMonth) for (const acct of accountMap()) heatBy[acct.address] = heatFor(events.filter(e => e.account === acct.address), monthStart, monthEnd);
    const monthEvents = wholeMonth ? events : undefined;
    const fetched = now.toISOString();
    if (!broke) return { fetched, days, month, events: events.filter(live), monthEvents, heat: Object.values(heatBy).reduce(addHeat, {}), heatBy, errors, stale: false, lastGood: fetched };
    const carry = prev && prev.month === month ? prev : null;
    const have = new Set(events.map(eventKey));
    const kept = carry ? carry.events.filter(e => failed.includes(e.account) && live(e) && !have.has(eventKey(e))) : [];
    const order = accountMap().map(a => a.address);
    const merged = events.filter(live).concat(kept).sort((a, b) => a.start.localeCompare(b.start) || order.indexOf(a.account) - order.indexOf(b.account));
    if (carry && carry.heatBy) for (const a of failed) if (carry.heatBy[a]) heatBy[a] = carry.heatBy[a];
    // The read time stays the last good one, so the agenda counts as due and is retried.
    return { fetched: prev ? prev.fetched : new Date(0).toISOString(), days, month, events: merged, monthEvents, heat: Object.values(heatBy).reduce(addHeat, {}), heatBy, errors, stale: true, lastGood: prev ? prev.lastGood : null };
  }

  // A whole month, for the grid's arrows and for picking a day outside the feed.
  async function fetchMonth(key) {
    const [y, m] = key.split('-').map(Number);
    const from = new Date(y, m - 1, 1), to = new Date(y, m, 1);
    const { events, errors, succeeded } = await readEvents(from, to, null);
    if (!succeeded) throw new Error(errors.join('; ') || 'waiting for Outlook');
    return { month: key, heat: heatFor(events, from, to), events, errors };
  }

  // The newest Other conversations of one inbox, newest first, as the card draws them.
  async function fetchOther(account, limit) {
    const acct = accountMap().find(a => a.address === account);
    if (!acct) throw new Error('unknown account');
    const max = Math.max(1, Math.min(1000, Math.floor(limit) || 40));
    const by = f => ({ __type: 'SortResults:#Exchange', Order: 'Descending', Path: PROP(f) });
    const out = await owa(acct, 'FindConversation', {
      ParentFolderId: { __type: 'TargetFolderId:#Exchange', BaseFolderId: { __type: 'DistinguishedFolderId:#Exchange', Id: 'inbox' } },
      ConversationShape: { __type: 'ConversationResponseShape:#Exchange', BaseShape: 'IdOnly', AdditionalProperties: [PROP('ConversationInferenceClassification')] },
      ShapeName: 'ReactConversationListView',
      Paging: { __type: 'IndexedPageView:#Exchange', BasePoint: 'Beginning', Offset: 0, MaxEntriesReturned: max },
      ViewFilter: 'All', FocusedViewFilter: 1,
      SortOrder: [by('ConversationLastDeliveryOrRenewTime'), by('ConversationLastDeliveryTime')],
    });
    const convs = out.Conversations || [];
    return {
      account, fetched: new Date().toISOString(), more: convs.length >= max,
      messages: convs.map(c => ({
        id: (c.ItemIds && c.ItemIds[0] && c.ItemIds[0].Id) || (c.ConversationId && c.ConversationId.Id) || '',
        subject: c.ConversationTopic || '',
        from: (c.LastSender && c.LastSender.Mailbox && c.LastSender.Mailbox.Name) || (c.UniqueSenders && c.UniqueSenders[0]) || '',
        received: c.LastDeliveryOrRenewTime || c.LastDeliveryTime,
        read: !(c.UnreadCount > 0),
        preview: (c.Preview || '').replace(/\s+/g, ' ').trim().slice(0, 160),
      })),
    };
  }

  // The agenda is read when due (every AGENDA_REFRESH, a minute after a failure or a stale
  // result, and soon after Outlook signs a call for a new account), only in a window with a
  // folder pane. Only the newest request may answer: a new account's headers can arrive
  // mid-read and start another.
  let agendaAsked = 0, agendaRun = 0;
  function ensureAgendaData(force) {
    if (!cfg.agenda || !current() || !document.querySelector(PANE)) return;
    const due = force || !agenda || Date.now() - Date.parse(agenda.fetched) > AGENDA_REFRESH;
    if (!due || (!force && Date.now() - agendaAsked < 60000)) return;
    agendaAsked = Date.now();
    const run = ++agendaRun;
    fetchAgenda(agenda).then(data => {
      if (run !== agendaRun || !current()) return;
      if (!data.stale) agendaAsked = 0;
      setAgenda(data);
    }, () => {});
  }
  let sessionTimer = 0;
  const onSession = () => {
    if (!current()) return;
    clearTimeout(sessionTimer);
    sessionTimer = setTimeout(() => { if (!current()) return; ensureAgendaData(true); if (other && other.error) askOther(); }, 1500);
  };
  session.on(onSession);

  // ==== agenda: a scrolling feed of the next few days, pinned to the bottom of the folder pane ====
  // fetchAgenda() reads it through Outlook's session and hands it to setAgenda(data).
  // Height: fills the free space below the folder tree until dragged; drag the top
  // edge to fix a height, double-click it to go back to filling.
  const AGENDA = '__agenda';
  const ALLDAY = '__allday';
  const HEIGHT_KEY = 'olkt-agenda-height';
  if (!(ALLDAY in folds)) folds[ALLDAY] = true;
  let agenda = null;
  let agendaEl = null;
  let agendaHeight = Number(store.getItem(HEIGHT_KEY)) || 0; // 0 = fill free space
  const fmtTime = d => d.toLocaleTimeString([], { hour: 'numeric', minute: '2-digit' });
  const dayKey = d => d.toDateString();
  // Local midnight that starts d's day, and whether a timed event overlaps that day
  // (a multi-day event shows on every day it touches, not just its first).
  const dayStart = d => new Date(d.getFullYear(), d.getMonth(), d.getDate());
  const nextDay = d => new Date(d.getFullYear(), d.getMonth(), d.getDate() + 1);
  const onDay = (e, d) => new Date(e.start) < nextDay(d) && new Date(e.end) > dayStart(d);
  const duration = ms => {
    const m = Math.round(ms / 60000);
    return m < 60 ? `${m}m` : m % 60 ? `${Math.floor(m / 60)}h ${m % 60}m` : `${m / 60}h`;
  };

  function setAgenda(data) { agenda = data; renderAgenda(); return 'agenda ' + data.events.length; }

  // Outlook's own event icons ("charms") are not drawn here: their art lives on a
  // Microsoft CDN, and a mod fetches nothing from the network.

  function el(tag, cls, text) {
    const e = document.createElement(tag);
    if (cls) e.className = cls;
    if (text != null) e.textContent = text;
    return e;
  }

  function saveFolds() { try { store.setItem(FOLD_KEY, JSON.stringify(folds)); } catch {} }

  function foldButton(label, key, count) {
    const b = el('button', 'olkt-head');
    b.type = 'button';
    b.append(el('span', 'olkt-chev'), el('span', 'olkt-lbl', label), el('span', 'olkt-sum', count && folds[key] ? String(count) : ''));
    b.setAttribute('aria-expanded', String(!folds[key]));
    onPress(b, () => { folds[key] = !folds[key]; saveFolds(); renderAgenda(); });
    return b;
  }

  function eventRow(e, now, d) {
    const row = el('div', 'olkt-ev' + (e.free ? ' free' : ''));
    row.style.setProperty('--olkt-ev-color', e.color || '#4aa3f0');
    row.title = [e.subject, e.label, e.categories && e.categories.join(', '), e.where].filter(Boolean).join(' · ');
    const start = new Date(e.start), end = new Date(e.end);
    // Carried over from an earlier day: say when it ends here, or that it runs on.
    const carried = !e.allDay && d && start < dayStart(d);
    const when = e.allDay ? 'All day'
      : carried ? (end > nextDay(d) ? 'Continues' : `Until ${fmtTime(end)}`)
      : `${fmtTime(start)} · ${duration(end - start)}`;
    const title = el('b', null, e.subject);
    if (e.repeating) {
      const svg = document.createElementNS('http://www.w3.org/2000/svg', 'svg');
      svg.setAttribute('viewBox', '0 0 16 16');
      svg.setAttribute('class', 'olkt-rep');
      const p = document.createElementNS('http://www.w3.org/2000/svg', 'path');
      // A loop with an arrow head, the way Outlook draws a repeating event.
      p.setAttribute('d', 'M4 6.5a4 4 0 0 1 6.9-2.2M12 9.5a4 4 0 0 1-6.9 2.2M10.4 4.3h2.2V2.1M5.6 11.7H3.4v2.2');
      svg.appendChild(p);
      const wrap = el('span', 'olkt-repwrap');
      wrap.title = 'Repeating event';
      wrap.appendChild(svg);
      title.prepend(wrap);
    }
    row.append(title, el('small', null, e.where ? `${when} · ${e.where}` : when));
    if (e.join) {
      // A camera in the meeting app's color: Teams purple, Zoom blue, Meet green, Webex teal.
      const join = el('button', 'olkt-join');
      join.type = 'button';
      join.dataset.kind = e.join.kind;
      join.title = `Join in ${e.join.kind}`;
      join.setAttribute('aria-label', `Join ${e.subject} in ${e.join.kind}`);
      const svg = document.createElementNS('http://www.w3.org/2000/svg', 'svg');
      svg.setAttribute('viewBox', '0 0 16 16');
      const cam = document.createElementNS('http://www.w3.org/2000/svg', 'path');
      cam.setAttribute('d', 'M2 5.5A1.5 1.5 0 0 1 3.5 4h6A1.5 1.5 0 0 1 11 5.5v5A1.5 1.5 0 0 1 9.5 12h-6A1.5 1.5 0 0 1 2 10.5zM12 6.6l2.2-1.4a.5.5 0 0 1 .8.4v4.8a.5.5 0 0 1-.8.4L12 9.4z');
      svg.appendChild(cam);
      join.appendChild(svg);
      if (now >= new Date(start - 10 * 60000) && now < end) join.classList.add('soon');
      onPress(join, () => { window.open(e.join.url); });
      row.appendChild(join);
    }
    return row;
  }

  // Month grid, Monday first: each day shaded by booked hours, today in bold. The
  // arrows change month (other months are read on demand);
  // clicking a day jumps the feed to it, or shows that day's events if it is
  // outside the upcoming feed.
  let viewMonth = null;      // first day of the month shown; null = the current month
  let selectedDay = null;    // Date picked in the grid, or null for the upcoming feed
  const monthCache = {};     // 'YYYY-MM' -> { heat, events, at } (at: when it arrived)
  const ymd = d => `${d.getFullYear()}-${String(d.getMonth() + 1).padStart(2, '0')}-${String(d.getDate()).padStart(2, '0')}`;
  const ym = d => ymd(d).slice(0, 7);

  // One request in flight per month (a failed one may retry after a minute), and a
  // cached month is fetched again once it is older than agenda.refreshMinutes; the
  // cached copy stays on screen meanwhile.
  const monthPending = new Map(); // 'YYYY-MM' -> when it was asked for
  const refreshMs = () => AGENDA_REFRESH;
  function askMonth(key) {
    const cached = monthCache[key];
    if (cached && Date.now() - cached.at < refreshMs()) return;
    if (Date.now() - (monthPending.get(key) || 0) < 60000) return;
    monthPending.set(key, Date.now());
    fetchMonth(key).then(data => { if (current()) setMonth(data); }, () => monthPending.delete(key));
  }

  function setMonth(data) {
    monthPending.delete(data.month);
    monthCache[data.month] = { ...data, at: Date.now() };
    renderAgenda();
    return 'month ' + data.month;
  }

  function monthData(d) {
    const key = ym(d);
    if (key === ym(new Date())) return { heat: agenda.heat || {}, events: monthCache[key] ? monthCache[key].events : (agenda.monthEvents || null) };
    return monthCache[key] || null;
  }

  function monthGrid(now) {
    const shown = viewMonth || new Date(now.getFullYear(), now.getMonth(), 1);
    const data = monthData(shown);
    // The current month's heat comes with the agenda; any other month (or the current
    // one once its events were asked for) is fetched, or refreshed when old.
    if (!data || monthCache[ym(shown)]) askMonth(ym(shown));
    const wrap = el('div', 'olkt-month');
    const bar = el('div', 'olkt-month-bar');
    const nav = (label, delta) => {
      const b = el('button', 'olkt-month-nav', label);
      b.type = 'button';
      b.setAttribute('aria-label', delta < 0 ? 'Previous month' : 'Next month');
      onPress(b, () => { viewMonth = new Date(shown.getFullYear(), shown.getMonth() + delta, 1); renderAgenda(); });
      return b;
    };
    const title = el('button', 'olkt-month-title', shown.toLocaleDateString([], { month: 'long', year: 'numeric' }));
    title.type = 'button';
    title.title = 'Back to this month and the upcoming feed';
    onPress(title, () => { viewMonth = null; selectedDay = null; renderAgenda(); });
    bar.append(nav('‹', -1), title, nav('›', 1));
    wrap.appendChild(bar);

    const grid = el('div', 'olkt-month-grid');
    for (const d of ['M', 'T', 'W', 'T', 'F', 'S', 'S']) grid.appendChild(el('span', 'olkt-dow', d));
    const first = new Date(shown);
    const start = new Date(first); start.setDate(1 - ((first.getDay() + 6) % 7));
    const last = new Date(shown.getFullYear(), shown.getMonth() + 1, 0);
    const cells = Math.ceil(((first.getDay() + 6) % 7 + last.getDate()) / 7) * 7;
    for (let i = 0; i < cells; i++) {
      const d = new Date(start); d.setDate(start.getDate() + i);
      const inMonth = d.getMonth() === shown.getMonth();
      const mins = inMonth && data ? data.heat[ymd(d)] || 0 : 0;
      const level = !mins ? 0 : mins < 120 ? 1 : mins < 240 ? 2 : mins < 420 ? 3 : 4;
      const cls = `olkt-cell h${level}` + (inMonth ? '' : ' out') + (dayKey(d) === dayKey(now) ? ' today' : '')
        + (selectedDay && dayKey(d) === dayKey(selectedDay) ? ' picked' : '');
      const c = el('button', cls, String(d.getDate()));
      c.type = 'button';
      if (mins) c.title = `${Math.round(mins / 6) / 10} h booked`;
      onPress(c, () => pickDay(d));
      grid.appendChild(c);
    }
    wrap.appendChild(grid);
    return wrap;
  }

  function pickDay(d) {
    const now = new Date();
    const startOfToday = new Date(now); startOfToday.setHours(0, 0, 0, 0);
    const offset = Math.round((new Date(d.getFullYear(), d.getMonth(), d.getDate()) - startOfToday) / 86400000);
    if (offset >= 0 && offset < (agenda.days || 2)) {
      // Inside the upcoming feed: snap to that day's divider.
      selectedDay = null;
      renderAgenda();
      const feed = agendaEl.querySelector('.olkt-feed');
      const mark = feed && feed.querySelector(`[data-day="${ymd(d)}"]`);
      if (feed) feed.scrollTop = mark ? mark.offsetTop - feed.firstElementChild.offsetTop : 0;
      return;
    }
    selectedDay = d;
    if (d.getMonth() !== (viewMonth || now).getMonth() || d.getFullYear() !== (viewMonth || now).getFullYear()) viewMonth = new Date(d.getFullYear(), d.getMonth(), 1);
    renderAgenda();
  }

  // One day's rows: an "All day (n)" fold, then timed events in order.
  function dayRows(feed, events, d, now, emptyText) {
    const noon = iso => new Date(iso.slice(0, 10) + 'T12:00:00');
    const mid = new Date(d); mid.setHours(12, 0, 0, 0);
    // All-day events carry UTC-midnight dates and can span days: show them on
    // every calendar day from start up to (not including) end.
    const allDay = events.filter(e => e.allDay && mid >= noon(e.start) && mid < noon(e.end));
    const timed = events.filter(e => !e.allDay && onDay(e, d));
    if (allDay.length) {
      const fold = foldButton(`All day (${allDay.length})`, ALLDAY, 0);
      fold.classList.add('olkt-allday');
      feed.appendChild(fold);
      if (!folds[ALLDAY]) for (const e of allDay) feed.appendChild(eventRow(e, now));
    }
    if (!timed.length) feed.appendChild(el('div', 'olkt-none', emptyText));
    for (const e of timed) feed.appendChild(eventRow(e, now, d));
  }

  function renderAgenda() {
    const pane = document.querySelector(PANE);
    if (!pane || !agenda) return;
    if (!agendaEl || !agendaEl.isConnected) {
      agendaEl = el('div', 'olkt-agenda');
      pane.appendChild(agendaEl);
    }
    // Match the pane's own background so the tree does not show through.
    let bgEl = pane, bg = 'rgba(0, 0, 0, 0)';
    while (bgEl && (bg === 'rgba(0, 0, 0, 0)' || bg === 'transparent')) { bg = getComputedStyle(bgEl).backgroundColor; bgEl = bgEl.parentElement; }
    agendaEl.style.background = bg;
    agendaEl.style.setProperty('--olkt-pane-bg', bg);

    const now = new Date();
    const events = agenda.events.filter(e => e.allDay || new Date(e.end) > now);
    const oldFeed = agendaEl.querySelector('.olkt-feed');
    const keepScroll = oldFeed ? oldFeed.scrollTop : 0;
    agendaEl.replaceChildren();

    const grip = el('div', 'olkt-agenda-grip');
    grip.title = 'Drag to resize; double-click to fill the free space';
    bindGrip(grip);
    agendaEl.appendChild(grip);
    const left = events.filter(e => !e.allDay && onDay(e, now)).length;
    const head = foldButton('TODAY  ' + now.toLocaleDateString([], { weekday: 'short', month: 'numeric', day: 'numeric' }), AGENDA, left);
    head.title = agenda.errors.length ? 'Some calendars failed: ' + agenda.errors.join('; ') : 'Updated ' + fmtTime(new Date(agenda.fetched));
    // Stale: a calendar read failed (older events kept), or no update for two refresh periods.
    const old = Date.now() - new Date(agenda.fetched) > 2 * refreshMs();
    if (agenda.stale || old) {
      const badge = el('span', 'olkt-stale', 'stale');
      const good = agenda.lastGood ? fmtTime(new Date(agenda.lastGood)) : 'never';
      badge.title = [agenda.errors.length ? 'Calendar read failed: ' + agenda.errors.join('; ') : 'No update since ' + fmtTime(new Date(agenda.fetched)),
        'Last good read: ' + good].join('\n');
      head.title = badge.title;
      head.querySelector('.olkt-lbl').after(badge);
    }
    agendaEl.appendChild(head);

    if (!folds[AGENDA]) {
      if (cfg.monthHeatMap) agendaEl.appendChild(monthGrid(now));
      const feed = el('div', 'olkt-feed');
      if (selectedDay) {
        // A day outside the upcoming feed: its events, and a way back.
        const back = el('button', 'olkt-back', 'Back to upcoming');
        back.type = 'button';
        onPress(back, () => { selectedDay = null; viewMonth = null; renderAgenda(); });
        feed.appendChild(back);
        const label = el('div', 'olkt-day', selectedDay.toLocaleDateString([], { weekday: 'long', month: 'numeric', day: 'numeric' }));
        feed.appendChild(label);
        const md = monthData(selectedDay);
        if (md && md.events) dayRows(feed, md.events, selectedDay, now, 'Nothing scheduled');
        else feed.appendChild(el('div', 'olkt-none', 'Loading...'));
        if (!md || !md.events) askMonth(ym(selectedDay));
      } else {
        for (let i = 0; i < (agenda.days || 2); i++) {
          const d = new Date(now); d.setDate(d.getDate() + i);
          if (i > 0) {
            const label = el('div', 'olkt-day', i === 1 ? 'Tomorrow' : d.toLocaleDateString([], { weekday: 'long', month: 'numeric', day: 'numeric' }));
            label.dataset.day = ymd(d);
            feed.appendChild(label);
          }
          dayRows(feed, events, d, now, i === 0 ? 'Nothing else today' : 'Nothing scheduled');
        }
      }
      agendaEl.appendChild(feed);
      feed.scrollTop = keepScroll;
    }
    layoutAgenda();
  }

  // The folder tree's scrolling element. Two elements carry Outlook's customScrollBar
  // class; only the one whose overflow scrolls is the right one.
  // Outlook only turns its overflow on while hovered, so pick it by class: the
  // innermost customScrollBar (the outer one is a focus wrapper).
  function treeScroller(pane) {
    const all = [...pane.querySelectorAll('.customScrollBar')].filter(d => !d.closest('.olkt-agenda'));
    return all[all.length - 1] || null;
  }

  // Size the agenda (fixed height, or the space the folder tree leaves free) and keep
  // the tree scrollable above it.
  function layoutAgenda() {
    const pane = document.querySelector(PANE);
    if (!pane || !agendaEl || !agendaEl.isConnected) return;
    const scroller = treeScroller(pane);
    const max = Math.round(pane.clientHeight * 0.85);
    let h;
    if (folds[AGENDA]) h = 0;
    else if (agendaHeight) h = Math.min(agendaHeight, max);
    else {
      // Free space = pane bottom minus where the folder tree's content actually ends
      // (scrollHeight never drops below the visible height, so it cannot be used).
      let treeEnd = pane.getBoundingClientRect().top;
      for (const n of pane.querySelectorAll('[role="treeitem"], .olkt-head, .olkt-compose')) {
        if (agendaEl.contains(n)) continue;
        const r = n.getBoundingClientRect();
        if (r.height && r.bottom > treeEnd) treeEnd = r.bottom;
      }
      h = Math.max(150, Math.min(max, Math.round(pane.getBoundingClientRect().bottom - treeEnd - 12)));
    }
    agendaEl.style.height = h ? h + 'px' : '';
    // Cap the folder tree at the agenda's top edge so it scrolls in the space left.
    // (Padding cannot work: this element has no fixed height and just grows.)
    if (scroller) {
      scroller.style.paddingBottom = '';
      const avail = agendaEl.getBoundingClientRect().top - scroller.getBoundingClientRect().top;
      scroller.style.maxHeight = Math.max(60, Math.round(avail)) + 'px';
    }
  }

  function bindGrip(grip) {
    grip.addEventListener('pointerdown', ev => {
      if (ev.button !== 0) return;
      ev.preventDefault();
      ev.stopPropagation();
      grip.setPointerCapture(ev.pointerId);
      const startY = ev.clientY;
      const startH = agendaEl.offsetHeight;
      const move = e => { agendaHeight = Math.max(80, Math.round(startH - (e.clientY - startY))); layoutAgenda(); };
      const up = () => {
        grip.removeEventListener('pointermove', move);
        try { store.setItem(HEIGHT_KEY, String(agendaHeight)); } catch {}
      };
      grip.addEventListener('pointermove', move);
      grip.addEventListener('pointerup', up, { once: true });
      grip.addEventListener('pointercancel', up, { once: true });
    });
    grip.addEventListener('dblclick', () => {
      agendaHeight = 0;
      try { store.removeItem(HEIGHT_KEY); } catch {}
      layoutAgenda();
    });
  }
  const agendaTimer = setInterval(() => { if (!current()) { clearInterval(agendaTimer); return; } ensureAgendaData(); renderAgenda(); }, 60000);

  // ==== menus and rename ====
  function closeMenus() {
    document.querySelectorAll('.olkt-menu, .olkt-rename').forEach(n => n.remove());
  }

  function showMenu(x, y, entries) {
    closeMenus();
    const menu = document.createElement('div');
    menu.className = 'olkt-menu';
    menu.setAttribute('role', 'menu');
    for (const e of entries) {
      if (e === '-') { const s = document.createElement('div'); s.className = 'olkt-sep'; menu.appendChild(s); continue; }
      if (e.heading) { const s = document.createElement('div'); s.className = 'olkt-sub'; s.textContent = e.heading; menu.appendChild(s); continue; }
      const b = document.createElement('button');
      b.type = 'button';
      b.setAttribute('role', 'menuitem');
      b.textContent = e.label;
      if (e.indent) b.style.paddingLeft = (12 + e.indent * 12) + 'px';
      if (e.disabled) b.disabled = true;
      b.addEventListener('click', ev => { ev.stopPropagation(); closeMenus(); e.run(); });
      menu.appendChild(b);
    }
    document.body.appendChild(menu);
    const r = menu.getBoundingClientRect();
    menu.style.left = Math.min(x, innerWidth - r.width - 8) + 'px';
    menu.style.top = Math.min(y, innerHeight - r.height - 8) + 'px';
    const first = menu.querySelector('button:not(:disabled)');
    if (first) first.focus();
  }

  function rename(anchor, value, commit) {
    closeMenus();
    const r = anchor.getBoundingClientRect();
    const input = document.createElement('input');
    input.className = 'olkt-rename';
    input.value = value;
    input.style.left = r.left + 'px';
    input.style.top = r.top + 'px';
    input.style.width = Math.max(120, r.width) + 'px';
    input.style.height = r.height + 'px';
    let done = false;
    const finish = ok => {
      if (done) return;
      done = true;
      input.remove();
      if (ok) commit(input.value.trim());
    };
    input.addEventListener('keydown', ev => {
      ev.stopPropagation();
      if (ev.key === 'Enter') finish(true);
      if (ev.key === 'Escape') finish(false);
    });
    input.addEventListener('blur', () => finish(true));
    document.body.appendChild(input);
    input.focus();
    input.select();
  }

  const newId = () => 'g' + Date.now().toString(36) + Math.random().toString(36).slice(2, 5);

  function addGroupAfter(node) {
    const at = locate(node);
    const g = { id: newId(), label: 'New group', items: [] };
    if (at) at.siblings.splice(at.index + 1, 0, g); else cfg.favorites.push(g);
    save();
    renameGroupSoon(g);
  }

  function renameGroupSoon(g) {
    requestAnimationFrame(() => {
      const h = document.querySelector(`.olkt-head[data-group=${q(g.id)}]`);
      if (h) rename(h, g.label, v => { if (v) { g.label = v; save(); } });
    });
  }

  function move(node, delta) {
    const at = locate(node);
    if (!at) return;
    const to = at.index + delta;
    if (to < 0 || to >= at.siblings.length) return;
    at.siblings.splice(at.index, 1);
    at.siblings.splice(to, 0, node);
    save();
  }

  function moveTo(node, group) {
    const at = locate(node);
    if (!at) return;
    at.siblings.splice(at.index, 1);
    (group ? group.items : cfg.favorites).push(node);
    if (group && folds[group.id]) toggleFold(group.id);
    save();
  }

  function groupMenu(g, header, x, y) {
    const at = locate(g);
    showMenu(x, y, [
      { label: 'Rename group', run: () => rename(header, g.label, v => { if (v) { g.label = v; save(); } }) },
      { label: 'New group below', run: () => addGroupAfter(g) },
      { label: 'New subgroup', run: () => {
        const sub = { id: newId(), label: 'New group', items: [] };
        g.items.push(sub);
        if (folds[g.id]) folds[g.id] = false;
        save();
        renameGroupSoon(sub);
      } },
      '-',
      { label: 'Move up', disabled: !at || at.index === 0, run: () => move(g, -1) },
      { label: 'Move down', disabled: !at || at.index === at.siblings.length - 1, run: () => move(g, 1) },
      '-',
      { label: 'Delete group (keeps its folders)', run: () => {
        const pos = locate(g);
        if (!pos) return;
        pos.siblings.splice(pos.index, 1, ...g.items);
        delete folds[g.id];
        save();
      } },
    ]);
  }

  function folderMenu(node, item, x, y) {
    const at = locate(node);
    const original = (item.getAttribute('title') || node.folder).replace(/ - [\d,]+ items?( \([\d,]+ unread\))?$/, '');
    const targets = [{ heading: 'MOVE TO' }, { label: 'Top level', indent: 0, disabled: !at || !at.parent, run: () => moveTo(node, null) }];
    for (const { n: g, depth } of groups()) {
      targets.push({ label: g.label, indent: depth + 1, disabled: at && at.parent === g, run: () => moveTo(node, g) });
    }
    showMenu(x, y, [
      { label: 'Rename (short label)', run: () => rename(item, node.label || original, v => { node.label = v && v !== original ? v : undefined; save(); }) },
      { label: 'Use original name', disabled: !node.label, run: () => { delete node.label; save(); } },
      { label: 'New group below', run: () => addGroupAfter(node) },
      '-',
      { label: 'Move up', disabled: !at || at.index === 0, run: () => move(node, -1) },
      { label: 'Move down', disabled: !at || at.index === at.siblings.length - 1, run: () => move(node, 1) },
      '-',
      ...targets,
    ]);
  }

  function onListContextMenu(ev) {
    // Plain right-click keeps Outlook's own menu; Shift+right-click opens ours.
    if (!ev.shiftKey) return;
    const item = ev.target.closest('[role="treeitem"]');
    const folder = item && item.getAttribute('data-folder-name');
    let node = folder && findFolder(folder);
    if (!node && folder && cfg.favorites.length && !matchesAnyGroup(folder) && placeUnplaced()) node = findFolder(folder);
    if (!node) return;
    ev.preventDefault();
    ev.stopPropagation();
    folderMenu(node, item, ev.clientX, ev.clientY);
  }

  // Favorites that are in no group yet (all of them, on a new install) show at the
  // top in Outlook's order. The first time one is edited, all of them join the tree
  // there, in that order, so the list looks the same and each can then be moved.
  function placeUnplaced() {
    const hidden = new Set(cfg.hide);
    const names = favoriteNames().filter(n => !hidden.has(n) && !findFolder(n) && !matchesAnyGroup(n));
    if (!names.length) return false;
    cfg.favorites.unshift(...names.map(folder => ({ folder })));
    save();
    return true;
  }

  // Another Outlook window saved its edits: take them, so this window's next save
  // cannot write back an older tree.
  // ==== calendar module ====
  // Left list: one row per account (the expanded account heading and "Show all" go, and
  // each account's "Calendar" row shows the account's short label); a narrower pane.
  // Time zones: compact columns with labels like "10p", the primary zone (the column
  // next to the grid) bright, the others dim and darker outside 9 to 5 in their zone.
  // Tiles: title and time; the location and organizer lines go.
  const CAL = cfg.calendar || {};
  const CALNAV = '[role="navigation"][aria-label^="date selection"]';
  const CALTILE = '[data-calitemid] [role="button"]';
  function calendarCss(out) {
    const w = CAL.paneWidth;
    if (w) {
      out.push(`body:not(:has(button[aria-label="Show navigation pane"])) div:has(> ${CALNAV}){width:${w}px!important;min-width:${w}px!important;max-width:${w}px!important}`);
      // The month picker is a fixed 7 x 28px table in 12px padding; tighten both to fit.
      out.push(`${CALNAV} div:has(> [role="grid"]){padding-left:4px!important;padding-right:4px!important}`);
      out.push(`${CALNAV} div:has([role="grid"]){width:auto!important;max-width:100%!important;box-sizing:border-box}`);
      out.push(`${CALNAV} [role="grid"]{margin-left:0!important;width:auto!important}`);
      out.push(`${CALNAV} div:has(> button[aria-label*="select to change the month"]){width:auto!important}`);
      out.push(`${CALNAV} button[aria-label*="select to change the month"]{flex:1 1 auto!important;min-width:0!important;width:auto!important;padding-left:3px!important}`);
      out.push(`${CALNAV} [role="grid"] :is(td,th){padding-left:0!important;padding-right:0!important}`);
    }
    if (S.calendarRowPerAccount) {
      out.push(`${CALNAV} div:has(> [id^="calendarAccountGroupHeader"][aria-expanded="true"]):has(~ div [data-olkt-label]){display:none}`);
      out.push(`${CALNAV} button[title="Show all calendars"]{display:none}`);
      out.push(`[data-olkt-label]{visibility:hidden;position:relative}`);
      out.push(`[data-olkt-label]::after{content:attr(data-olkt-label);visibility:visible;position:absolute;left:0;top:0;white-space:nowrap}`);
    }
    if (S.compactTimeZones) {
      const zw = CAL.zoneWidth || 32;
      out.push(`[data-olkt-tz]{width:${zw}px!important;min-width:${zw}px!important;max-width:${zw}px!important}`);
      out.push(`time[data-olkt-short]{font-size:0!important}`);
      out.push(`time[data-olkt-short]::after{content:attr(data-olkt-short);font-size:10.5px;font-variant-numeric:tabular-nums}`);
      out.push(`[data-olkt-tz="pri"] time{color:var(--colorNeutralForeground1,#e6e6e6)!important;font-weight:600}`);
      out.push(`[data-olkt-tz="sec"] time{color:var(--colorNeutralForeground3,#9a9a9a)!important}`);
      out.push(`[data-olkt-tz="sec"] time[data-olkt-off]{color:var(--colorNeutralForeground4,#555)!important}`);
      out.push(`[data-olkt-tz] p{font-size:10.5px!important}`);
    }
    // Tiles only in the hourly grid (Day, Work week, Week); Month view tiles are one line.
    const GRID = 'body:has([data-tabid="surfaceHeader_timeStripHeader"])';
    if (S.tileTimes) {
      // Only tiles whose time was read (data-olkt-time): all-day and unusual tiles keep every line.
      out.push(`${GRID} ${CALTILE}:has(> div > [data-olkt-time]) > div > div:nth-child(n+2):not(:has(i)){display:none!important}`);
      out.push(`${GRID} [data-olkt-time]::after{content:attr(data-olkt-time);display:block;font-weight:400;font-size:11.5px;opacity:.8}`);
    }
    // Overlaps (ensureOverlap): two events cascade, the later one 18px in and on top;
    // three or more become one merged tile listing them, over the hidden originals.
    out.push(`[data-olkt-ov="c0"]{left:0!important;right:0!important}`);
    for (let i = 1; i < 4; i++) out.push(`[data-olkt-ov="c${i}"]{left:${i * 18}px!important;right:0!important;z-index:${i + 1};box-shadow:-2px 0 0 var(--olkt-cal-bg,#1f1f1f)}`);
    out.push(`[data-olkt-ov^="c"]:hover{z-index:4!important}`);
    out.push(`[data-olkt-ov="m"]{opacity:0!important;pointer-events:none!important}`);
    out.push(`.olkt-merge{position:absolute;left:0;right:0;margin:1px 2px;z-index:3;box-sizing:border-box;display:flex;flex-direction:column;gap:3px;padding:2px 4px;overflow-y:auto;scrollbar-width:none;border-radius:4px;border:1px solid var(--colorNeutralStroke1,#474747);background:var(--colorNeutralBackground3,#2b2b2b);color:var(--colorNeutralForeground1,#fff);font-size:11.5px;line-height:15px}`);
    out.push(`.olkt-merge:is(:hover,:focus-within){height:auto!important;min-height:var(--h);z-index:6;box-shadow:0 4px 12px rgba(0,0,0,.5)}`);
    out.push(`.olkt-merge .ct{flex:none;font-size:10.5px;font-weight:600;color:var(--colorNeutralForeground3,#adadad);white-space:nowrap;overflow:hidden;text-overflow:ellipsis}`);
    out.push(`.olkt-merge .crow{flex:none;border-left:3px solid var(--edge,#479ef5);padding-left:4px;border-radius:2px;cursor:pointer}`);
    out.push(`.olkt-merge .crow:hover{background:var(--colorNeutralBackground1Hover,#333)}`);
    out.push(`.olkt-merge .crow b{display:block;font-weight:600;white-space:nowrap;overflow:hidden;text-overflow:ellipsis}`);
    out.push(`.olkt-merge .crow small{display:block;opacity:.75;font-variant-numeric:tabular-nums}`);
    out.push(`.olkt-merge .bar{position:relative;height:3px;margin-top:1px;border-radius:2px;background:var(--colorNeutralStroke2,#3d3d3d)}`);
    out.push(`.olkt-merge .bar i{position:absolute;top:0;bottom:0;border-radius:2px;background:var(--edge,#479ef5)}`);
  }

  // ==== Other inbox under Focused ====
  // While an inbox shows its Focused tab, the bottom of the message list holds the Other
  // tab's newest conversations (read through Outlook's session), with a drag grip on its top
  // edge. A row opens that message in its own Outlook window.
  const OTHER_H_KEY = 'olkt-other-height';
  const OTHER_REFRESH = 120000;
  let otherEl = null;
  let other = null;          // { account, messages } or { account, error }
  let otherAccount = null;
  let otherAsked = 0;
  const OTHER_PAGE = 40;
  const OTHER_WAIT = 20000; // no reply by then: show Outlook's own Other tab again
  let otherLimit = OTHER_PAGE;
  let otherHeight = Number(store.getItem(OTHER_H_KEY)) || 0; // 0 = 40% of the column

  function setOther(data) {
    if (data && data.account === otherAccount) {
      // A failed refresh or "Show more" keeps the messages already shown.
      const keep = data.error && other && other.messages && other.messages.length;
      if (!keep) other = data;
      else if (otherLimit > other.messages.length) otherLimit = Math.max(OTHER_PAGE, otherLimit - OTHER_PAGE); // that page failed
      renderOther();
    }
    return 'other ' + ((data && data.messages) || []).length; }
  let otherRun = 0;
  function askOther() {
    otherAsked = Date.now();
    // Only the newest request may answer: a refresh and a "Show more" can overlap.
    const account = otherAccount, run = ++otherRun;
    fetchOther(account, otherLimit).then(data => { if (run === otherRun && current()) setOther(data); }, err => { if (run === otherRun && current()) setOther({ account, error: err.message }); });
  }

  function otherCss(out) {
    // Sizes come from a live Outlook header and row (measureList), so density changes carry over.
    // A section that failed (data-err) steps aside and Outlook's Other tab comes back.
    out.push(`body:has(.olkt-other:not([data-err])) [data-olkt-othertab]{display:none!important}`);
    out.push(`.olkt-other[data-err]{display:none!important}`);
    out.push(`.olkt-other{flex:none;position:relative;display:flex;flex-direction:column;box-sizing:border-box;min-height:80px;margin-top:8px;border-top:1px solid var(--olkt-stroke,#525252);font-size:var(--o-fs,12px);line-height:var(--o-lh,16px);color:var(--colorNeutralForeground1,#d6d6d6)}`);
    // The gap paints over the column's side borders, so Focused and Other read as two cards.
    out.push(`.olkt-other::before{content:"";position:absolute;left:-1px;right:-1px;top:-9px;height:8px;background:var(--olkt-ground,#141414);border-top:1px solid var(--olkt-stroke,#525252)}`);
    out.push(`:has(> [role="tablist"] + .olkt-other-toggle){display:flex!important;flex-direction:row!important;align-items:center}`);
    out.push(`.olkt-other-toggle[aria-pressed="true"]{color:var(--colorBrandForeground1,#479ef5)}`);
    out.push(`.olkt-other-toggle svg{width:16px;height:16px;fill:none;stroke:currentColor;stroke-width:1.2}`);
    out.push(`.olkt-other-grip{position:absolute;left:0;right:0;top:-9px;height:8px;cursor:row-resize;z-index:2;display:flex;align-items:center;justify-content:center}`);
    out.push(`.olkt-other-grip::after{content:"";width:40px;height:3px;border-radius:2px;background:var(--colorNeutralStroke1,#474747)}`);
    out.push(`.olkt-other-grip:hover::after{background:var(--colorBrandStroke1,#4aa3f0)}`);
    out.push(`.olkt-other-head{flex:none;display:flex;align-items:center;height:38px;padding-left:var(--o-x,67px);background:var(--o-rowbg,#292929);border-bottom:1px solid var(--olkt-ground,#141414)}`);
    out.push(`.olkt-other-head span{position:relative;font-size:14px;line-height:20px;font-weight:600;color:var(--colorNeutralForeground1,#fff)}`);
    out.push(`.olkt-other-head span::after{content:"";position:absolute;left:0;right:0;bottom:-5px;height:3px;border-radius:2px;background:var(--colorBrandStroke1,#479ef5)}`);
    out.push(`.olkt-other-list{flex:1;min-height:0;overflow-y:auto;background:var(--olkt-ground,#141414)}`);
    out.push(`.olkt-ogrp{display:flex;align-items:center;height:var(--o-gh,25px);padding-left:var(--o-gx,67px);position:relative;background:var(--o-gbg,#141414);font-size:var(--o-gfs,12px);font-weight:600;color:var(--colorNeutralForeground1,#fff);cursor:pointer;user-select:none}`);
    out.push(`.olkt-ogrp svg{position:absolute;left:calc(var(--o-gx,67px) - 28px);width:12px;height:12px;fill:none;stroke:currentColor;stroke-width:1.4;transition:transform .12s}`);
    out.push(`.olkt-ogrp[aria-expanded="false"] svg{transform:rotate(-90deg)}`);
    out.push(`.olkt-orow{position:relative;display:grid;grid-template-columns:1fr auto;column-gap:8px;padding:var(--o-pad,6px) 24px var(--o-pad,6px) var(--o-x,67px);background:var(--o-rowbg,#292929);border-bottom:1px solid var(--olkt-ground,#141414);cursor:pointer}`);
    out.push(`.olkt-orow:hover{background:var(--colorNeutralBackground1Hover,#333)}`);
    out.push(`.olkt-orow > *{overflow:hidden;text-overflow:ellipsis;white-space:nowrap}`);
    out.push(`.olkt-orow .olkt-ofrom{grid-column:1 / span 2;color:var(--colorNeutralForeground1,#fff)}`);
    out.push(`.olkt-orow .olkt-osubj{color:var(--colorNeutralForeground1,#fff)}`);
    out.push(`.olkt-orow .olkt-otime{text-align:right;color:var(--colorNeutralForeground1,#fff);font-variant-numeric:tabular-nums}`);
    out.push(`.olkt-orow .olkt-oprev{grid-column:1 / span 2;color:var(--colorNeutralForeground3,#adadad)}`);
    out.push(`.olkt-coin{position:absolute;left:var(--o-avx,calc(var(--o-x,67px) - var(--o-av,28px) - 9px));top:var(--o-avy,var(--o-pad,6px));width:var(--o-av,28px);height:var(--o-av,28px);border-radius:50%;display:flex;align-items:center;justify-content:center;font-size:calc(var(--o-av,28px) * .4);font-weight:600;overflow:visible}`);
    out.push(`.olkt-orow.unread::before{content:"";position:absolute;left:0;top:0;bottom:0;width:3px;background:var(--colorBrandForeground1,#479ef5)}`);
    out.push(`.olkt-orow.unread .olkt-ofrom{font-weight:600}`);
    out.push(`.olkt-orow.unread .olkt-osubj,.olkt-orow.unread .olkt-otime{font-weight:600;color:var(--colorBrandForeground1,#479ef5)}`);
    out.push(`.olkt-other .olkt-none{padding:10px var(--o-x,67px);color:var(--colorNeutralForeground3,#a3a3a3)}`);
    out.push(`.olkt-other .olkt-omore{display:block;padding:10px var(--o-x,67px);color:var(--colorBrandForeground1,#479ef5);cursor:pointer}`);
    out.push(`.olkt-other .olkt-omore:hover{text-decoration:underline}`);
    out.push(`.olkt-orow:focus-visible,.olkt-ogrp:focus-visible,.olkt-omore:focus-visible,.olkt-nrow:focus-visible,.olkt-ngrp:focus-visible{outline:2px solid var(--colorStrokeFocus2,#fff);outline-offset:-2px}`);
    // Cloned (native) parts: Outlook's classes draw them; only the header, fold and scrollbar are ours.
    out.push(`.olkt-other-head.native{padding-left:0;border-bottom:0;align-items:center}`);
    out.push(`.olkt-other-head.native > *{flex:none}`);
    out.push(`.olkt-nrow{cursor:pointer}`);
    out.push(`.olkt-ngrp[aria-expanded="false"] > i{transform:rotate(-90deg)}`);
    out.push(`.olkt-other-list{scrollbar-width:thin;scrollbar-color:var(--colorNeutralStroke1,#5c5c5c) transparent}`);
  }

  // The selected folder's account when it is an inbox ("inbox (name@domain)").
  function inboxAccount() {
    const sel = document.querySelector('[role="treeitem"][aria-selected="true"]');
    if (!sel) {
      // Right after startup no folder is selected yet; the URL still says which inbox shows.
      // A cold start sits at /mail/ (or /mail/<address>/) with no folder named at all, and
      // that is the account's inbox, so the folder part is optional.
      let path = location.pathname;
      try { path = decodeURIComponent(path); } catch {}
      const u = /^\/mail\/(?:([^/]+@[^/]+)\/)?(?:inbox)?(?:\/|$)/i.exec(path);
      return u ? ((u[1] || primary()).toLowerCase() || null) : null;
    }
    const name = sel.getAttribute('data-folder-name') || '';
    if (!/^inbox\b/i.test(name)) return null;
    const m = /\(([^)\s]+@[^)\s]+)\)/.exec(name);
    return ((m ? m[1] : primary()) || '').toLowerCase() || null;
  }
  function focusedTabActive() {
    const tab = [...document.querySelectorAll('[role="tab"]')].find(t => t.textContent.startsWith('Focused'));
    return !!tab && tab.getAttribute('aria-selected') === 'true';
  }

  function ensureOther() {
    const col = cfg.otherCard && document.querySelector(MSGLIST);
    if (col) ensureOtherToggle();
    const account = col && otherSplit && focusedTabActive() ? inboxAccount() : null;
    if (!account) { if (otherEl) { otherEl.remove(); otherEl = null; } return; }
    // Other shows below Focused, so its tab goes (CSS hides it while the section works).
    const otherTab = [...document.querySelectorAll('[role="tab"]')].find(t => t.textContent.startsWith('Other'));
    if (otherTab && !otherTab.hasAttribute('data-olkt-othertab')) otherTab.setAttribute('data-olkt-othertab', '');
    if (account !== otherAccount) { other = null; otherAccount = account; otherLimit = OTHER_PAGE; askOther(); }
    else if (Date.now() - otherAsked > OTHER_REFRESH) askOther();
    else if (!other && Date.now() - otherAsked > OTHER_WAIT) { other = { account, error: 'no reply' }; renderOther(); }
    if (!otherEl || !otherEl.isConnected || otherEl.parentElement !== col) {
      if (otherEl) otherEl.remove();
      otherEl = el('div', 'olkt-other');
      col.appendChild(otherEl);
      renderOther();
    } else if (otherEl.dataset.account !== account) renderOther();
  }

  function renderOther() {
    if (!otherEl || !otherEl.isConnected) return;
    const col = otherEl.parentElement;
    let bgEl = col, bg = 'rgba(0, 0, 0, 0)';
    while (bgEl && (bg === 'rgba(0, 0, 0, 0)' || bg === 'transparent')) { bgEl = bgEl.parentElement; bg = bgEl ? getComputedStyle(bgEl).backgroundColor : bg; }
    otherEl.style.setProperty('--olkt-ground', bg);
    otherEl.style.setProperty('--olkt-stroke', getComputedStyle(col).borderTopColor);
    otherEl.style.height = Math.min(otherHeight || Math.round(col.clientHeight * 0.4), Math.round(col.clientHeight * 0.8)) + 'px';
    otherEl.dataset.account = otherAccount;
    otherEl.toggleAttribute('data-err', !!(other && other.error));
    Object.assign(listMetrics, measureList(col));
    for (const [k, v] of Object.entries(listMetrics)) otherEl.style.setProperty('--o-' + k, v);
    captureTemplates(col);
    const keepScroll = otherEl.querySelector('.olkt-other-list')?.scrollTop || 0;
    otherEl.replaceChildren();
    const grip = el('div', 'olkt-other-grip');
    grip.title = 'Drag to resize; double-click to reset';
    bindOtherGrip(grip);
    const msgs = other && other.messages;
    const head = el('div', 'olkt-other-head');
    let ourLabel = null, liveLabel = null, tabEl = null;
    if (tpl.tab) {
      // Outlook's own Focused tab, relabeled, at the same spot in a header of the same size.
      const tab = tpl.tab.node.cloneNode(true);
      let label = null;
      for (const s of tab.querySelectorAll('span')) if (s.children.length === 0 && s.textContent.trim() === 'Focused') { s.textContent = 'Other'; label = s; }
      for (const b of tab.querySelectorAll('[role="tab"]')) { b.setAttribute('role', 'presentation'); b.tabIndex = -1; }
      // Lined up with the live Focused tab rather than the offset measured when the template
      // was taken: the card sits inside the message column, so that offset counted the
      // column's own padding a second time.
      const live = [...document.querySelectorAll('[role="tab"]')].find(t => (t.textContent || '').trim().startsWith('Focused') && t.getBoundingClientRect().height > 0);
      const from = otherEl.getBoundingClientRect().x;
      const offset = live && live.parentElement ? Math.round(live.parentElement.getBoundingClientRect().x - from) : tpl.tab.x;
      tab.style.marginLeft = Math.max(0, offset) + 'px';
      tabEl = tab;
      ourLabel = label;
      liveLabel = live ? [...live.querySelectorAll('span')].find(x => !x.children.length && x.textContent.trim() === 'Focused') || live : null;
      head.classList.add('native');
      head.style.height = tpl.tab.h + 'px';
      if (tpl.tab.bg) head.style.background = tpl.tab.bg;
      head.append(tab);
    } else head.append(el('span', null, 'Other'));
    const list = el('div', 'olkt-other-list');
    if (!other) list.appendChild(el('div', 'olkt-none', 'Loading...'));
    else if (other.error) list.appendChild(el('div', 'olkt-none', 'Other inbox not available for this account'));
    else if (!msgs.length) list.appendChild(el('div', 'olkt-none', 'Nothing in Other'));
    else {
      // Date groups as Outlook draws them, each folding like Outlook's own.
      const now = new Date();
      let group = null;
      for (const m of msgs) {
        const g = dateGroup(new Date(m.received), now);
        if (g !== group) {
          group = g;
          const native = cloneGroup(g, !otherFolds[g]);
          if (native) {
            const key = g;
            onPress(native, () => { otherFolds[key] = !otherFolds[key]; try { store.setItem(OTHER_FOLD_KEY, JSON.stringify(otherFolds)); } catch {} renderOther(); });
            list.appendChild(native);
            if (!otherFolds[g]) list.appendChild(otherRow(m, now));
            continue;
          }
          const hdr = el('div', 'olkt-ogrp');
          hdr.setAttribute('role', 'button');
          hdr.tabIndex = 0;
          hdr.setAttribute('aria-expanded', String(!otherFolds[g]));
          const svg = document.createElementNS('http://www.w3.org/2000/svg', 'svg');
          svg.setAttribute('viewBox', '0 0 12 12');
          const p = document.createElementNS('http://www.w3.org/2000/svg', 'path');
          p.setAttribute('d', 'M2.5 4.5 6 8l3.5-3.5');
          svg.appendChild(p);
          hdr.append(svg, el('span', null, g));
          const key = g;
          onPress(hdr, () => { otherFolds[key] = !otherFolds[key]; try { store.setItem(OTHER_FOLD_KEY, JSON.stringify(otherFolds)); } catch {} renderOther(); });
          list.appendChild(hdr);
        }
        if (!otherFolds[g]) list.appendChild(otherRow(m, now));
      }
      // One FindConversation page holds at most 1000, so paging stops there.
      if (other.more && otherLimit < 1000) {
        const more = el('div', 'olkt-omore', 'Show more');
        more.setAttribute('role', 'button');
        more.tabIndex = 0;
        onPress(more, () => {
          more.textContent = 'Loading...';
          otherLimit += OTHER_PAGE;
          askOther();
          setTimeout(() => { if (more.isConnected) more.textContent = 'Show more'; }, OTHER_WAIT);
        });
        list.appendChild(more);
      }
    }
    otherEl.append(grip, head, list);
    // Line the word up with the word above it, on the next frame: measured in this one the
    // card has not been laid out yet and every rectangle reads as zero.
    if (tabEl && ourLabel && liveLabel) requestAnimationFrame(() => {
      if (!current() || !ourLabel.isConnected || !liveLabel.isConnected) return;
      const drift = Math.round(liveLabel.getBoundingClientRect().x - ourLabel.getBoundingClientRect().x);
      if (!drift) return;
      const margin = parseFloat(tabEl.style.marginLeft) || 0;
      tabEl.style.marginLeft = Math.max(0, Math.round(margin + drift)) + 'px';
    });
    list.scrollTop = keepScroll;
    // Unless dragged, the section is as tall as its messages (the rows' own heights, since the
    // list stretches to fill whatever height it has), up to 45% of the column.
    if (!otherHeight) {
      const content = [...list.children].reduce((h, c) => h + c.offsetHeight, 0);
      otherEl.style.height = Math.max(80, Math.min(head.offsetHeight + content + 2, Math.round(col.clientHeight * 0.45))) + 'px';
    }
  }

  // The on/off button by the tabs: a copy of Outlook's own header button (Filter), so it
  // looks and hovers like its neighbors. Off: no section, and Outlook's Other tab is back.
  const OTHER_SPLIT_KEY = 'olkt-other-split';
  let otherSplit = true;
  try { otherSplit = store.getItem(OTHER_SPLIT_KEY) !== '0'; } catch {}
  let otherToggle = null;
  function ensureOtherToggle() {
    const tab = [...document.querySelectorAll('[role="tab"]')].find(t => t.textContent.startsWith('Focused'));
    const tablist = tab && tab.closest('[role="tablist"]');
    if (!tablist || !inboxAccount()) { if (otherToggle) { otherToggle.remove(); otherToggle = null; } return; }
    if (!otherToggle) {
      const model = [...document.querySelectorAll(`${MSGLIST} button[aria-label]`)].find(b => b.getAttribute('aria-label') === 'Filter');
      otherToggle = model ? model.cloneNode(false) : el('button');
      for (const a of ['id', 'aria-haspopup', 'aria-expanded', 'data-tabid']) otherToggle.removeAttribute(a);
      otherToggle.classList.add('olkt-other-toggle');
      otherToggle.style.marginLeft = '6px';
      otherToggle.style.alignSelf = 'center';
      const svg = document.createElementNS('http://www.w3.org/2000/svg', 'svg');
      svg.setAttribute('viewBox', '0 0 16 16');
      for (const d of ['M2.5 2.5h11v11h-11z', 'M2.5 8.5h11']) {
        const p = document.createElementNS('http://www.w3.org/2000/svg', 'path');
        p.setAttribute('d', d);
        svg.appendChild(p);
      }
      otherToggle.appendChild(svg);
      onPress(otherToggle, () => {
        otherSplit = !otherSplit;
        try { store.setItem(OTHER_SPLIT_KEY, otherSplit ? '1' : '0'); } catch {}
        ensureOther();
      });
    }
    const label = otherSplit ? 'Show Other as a tab' : 'Show Other below Focused';
    if (otherToggle.title !== label) { otherToggle.title = label; otherToggle.setAttribute('aria-label', label); }
    setAttr(otherToggle, 'aria-pressed', String(otherSplit));
    if (otherToggle.previousElementSibling !== tablist) tablist.after(otherToggle);
  }

  const OTHER_FOLD_KEY = 'olkt-other-folds';
  let otherFolds = {};
  try { otherFolds = JSON.parse(store.getItem(OTHER_FOLD_KEY) || '{}'); } catch {}
  const listMetrics = {};

  // Sizes from Outlook's own list: a date header and the shortest unselected row.
  function measureList(col) {
    const v = {};
    const lb = col.querySelector('[role="listbox"]');
    if (!lb) return v;
    const cx = col.getBoundingClientRect().x;
    const px = n => Math.round(n) + 'px';
    // Real words only: icon fonts draw their glyphs as text too.
    const leafText = (root, minX) => [...root.querySelectorAll('*')].find(e => e.children.length === 0 && /[A-Za-z0-9]{2}/.test(e.textContent) && e.getBoundingClientRect().height > 0 && e.getBoundingClientRect().x >= minX);
    const g = lb.querySelector('[role="button"][aria-expanded]');
    if (g) {
      const s = getComputedStyle(g), gt = leafText(g, 0);
      v.gh = px(g.getBoundingClientRect().height); v.gbg = s.backgroundColor;
      if (gt) { v.gfs = getComputedStyle(gt).fontSize; v.gx = px(gt.getBoundingClientRect().x - cx); }
    }
    // The least indented unselected row (children of an expanded conversation sit further in).
    let best = null;
    const shown = [...lb.querySelectorAll('[role="option"]')].filter(o => o.getBoundingClientRect().height > 0);
    const unselected = shown.filter(o => o.getAttribute('aria-selected') !== 'true');
    for (const row of (unselected.length ? unselected : shown).slice(0, 12)) {
      const coin = [...row.querySelectorAll('*')].find(e => {
        const r = e.getBoundingClientRect();
        return r.width >= 20 && r.width <= 48 && Math.abs(r.width - r.height) < 1.5 && getComputedStyle(e).borderRadius.includes('50%');
      });
      const cr = coin && coin.getBoundingClientRect();
      const text = leafText(row, cr ? cr.right : 0);
      if (!text) continue;
      const tx = text.getBoundingClientRect().x;
      if (!best || tx < best.tx) best = { row, coin: cr, text, tx };
    }
    if (!best) return v;
    const rr = best.row.getBoundingClientRect(), tr = best.text.getBoundingClientRect(), ts = getComputedStyle(best.text);
    const bg = getComputedStyle(best.row).backgroundColor;
    if (bg !== 'rgba(0, 0, 0, 0)' && best.row.getAttribute('aria-selected') !== 'true') v.rowbg = bg;
    v.x = px(tr.x - cx); v.pad = px(tr.y - rr.y); v.fs = ts.fontSize; v.lh = ts.lineHeight;
    if (best.coin) { v.av = px(best.coin.width); v.avx = px(best.coin.x - cx); v.avy = px(best.coin.y - rr.y); }
    return v;
  }

  // Outlook's list groups: Today, Yesterday, This Week, Last Week, This Month, Last Month, Older.
  function dateGroup(d, now) {
    const day = x => new Date(x.getFullYear(), x.getMonth(), x.getDate());
    const t = day(now), dd = day(d);
    const diff = Math.round((t - dd) / 864e5);
    if (diff <= 0) return 'Today';
    if (diff === 1) return 'Yesterday';
    const week = new Date(t); week.setDate(t.getDate() - t.getDay());
    if (dd >= week) return 'This Week';
    const last = new Date(week); last.setDate(week.getDate() - 7);
    if (dd >= last) return 'Last Week';
    if (dd.getFullYear() === t.getFullYear() && dd.getMonth() === t.getMonth()) return 'This Month';
    if (dd >= new Date(t.getFullYear(), t.getMonth() - 1, 1)) return 'Last Month';
    return 'Older';
  }
  // Outlook's time column: 1:46 PM today, Tue 11:09 AM this week, Mon 10/19 before that.
  function listTime(d, now) {
    const days = (now - d) / 864e5;
    const wd = d.toLocaleDateString([], { weekday: 'short' });
    if (dayKey(d) === dayKey(now)) return fmtTime(d);
    if (days < 6) return `${wd} ${fmtTime(d)}`;
    if (d.getFullYear() === now.getFullYear()) return `${wd} ${d.getMonth() + 1}/${d.getDate()}`;
    return `${d.getMonth() + 1}/${d.getDate()}/${d.getFullYear()}`;
  }
  // Avatar coins in the dark persona colors, picked from the sender's name.
  const COINS = [['#750b1c', '#f1bbbc'], ['#8e562e', '#f6d1b8'], ['#5c2e91', '#d8c7ed'], ['#0b556a', '#a6e9ed'], ['#2c3c85', '#c5cbfa'], ['#3b6d2c', '#c9e7b9'],
    ['#6b2a5a', '#ecc0e0'], ['#804d00', '#fbe2a6'], ['#004e8c', '#b3d6f2'], ['#005e50', '#a6e5dc'], ['#4b1d5c', '#e0c6ea'], ['#3d3d3d', '#d6d6d6']];
  function coin(name) {
    const words = name.replace(/[^\p{L}\p{N} ]/gu, ' ').trim().split(/\s+/).filter(Boolean);
    const initials = ((words[0] || '?')[0] + (words.length > 1 ? words[words.length - 1][0] : '')).toUpperCase();
    let h = 0; for (const ch of name) h = (h * 31 + ch.codePointAt(0)) >>> 0;
    const [bg, fg] = COINS[h % COINS.length];
    const c = el('span', 'olkt-coin', initials);
    c.style.background = bg;
    c.style.color = fg;
    return c;
  }

  // Templates cloned from Outlook's own list (a read row, an unread row, a date header, the
  // Focused tab), so the Other section is drawn by Outlook's CSS. Cached, because the list
  // only renders what is on screen; the hand-built rows below are the fallback.
  const tpl = { read: null, unread: null, group: null, tab: null };
  const pathOf = (node, root) => { const p = []; for (let n = node; n !== root; n = n.parentElement) p.unshift([...n.parentElement.children].indexOf(n)); return p; };
  const nodeAt = (root, path) => path.reduce((n, i) => n && n.children[i], root);
  function captureTemplates(col) {
    const lb = col.querySelector('[role="listbox"]');
    if (!lb) return;
    const seen = {};
    for (const row of lb.querySelectorAll('[role="option"]')) {
      if (row.getAttribute('aria-selected') === 'true' || row.querySelector('[aria-expanded]') || row.getBoundingClientRect().height === 0) continue;
      const kind = row.querySelector('button[title="Mark as read"]') ? 'unread' : 'read';
      if (seen[kind]) continue;
      const img = row.querySelector('[role="img"]');
      const initials = img && [...img.querySelectorAll('span')].find(s => s.children.length === 0 && s.textContent.trim());
      // Text lines by position: sender; subject and time; preview. Rows with more (attachments) are skipped.
      const leaves = [...row.querySelectorAll('span, div')].filter(e => e.children.length === 0 && e.textContent.trim() && !e.closest('[role="img"]') && e.getBoundingClientRect().height > 0);
      const lines = [];
      for (const e of leaves) {
        const r = e.getBoundingClientRect();
        const line = lines.find(l => Math.abs(l.y - r.y) < 5);
        if (line) line.items.push({ e, x: r.x }); else lines.push({ y: r.y, items: [{ e, x: r.x }] });
      }
      lines.sort((a, b) => a.y - b.y).forEach(l => l.items.sort((a, b) => a.x - b.x));
      if (!initials || lines.length !== 3 || lines[0].items.length !== 1 || lines[1].items.length !== 2 || lines[2].items.length !== 1) continue;
      seen[kind] = true;
      tpl[kind] = {
        node: row.cloneNode(true),
        paths: { img: pathOf(img, row), initials: pathOf(initials, row), from: pathOf(lines[0].items[0].e, row), subj: pathOf(lines[1].items[0].e, row), time: pathOf(lines[1].items[1].e, row), prev: pathOf(lines[2].items[0].e, row) },
      };
    }
    const g = lb.querySelector('[role="button"][aria-expanded="true"]');
    const gt = g && g.querySelector('[title]');
    if (gt) tpl.group = { node: g.cloneNode(true), title: pathOf(gt, g) };
    const tab = [...document.querySelectorAll('[role="tab"]')].find(t => t.textContent.startsWith('Focused') && t.getAttribute('aria-selected') === 'true');
    if (tab) {
      const wrap = tab.parentElement;
      let bar = wrap;
      while (bar.parentElement && bar.parentElement !== col && bar.getBoundingClientRect().width < col.clientWidth - 4) bar = bar.parentElement;
      let bg = 'rgba(0, 0, 0, 0)';
      for (let b = bar; b && b !== col.parentElement && (bg === 'rgba(0, 0, 0, 0)' || bg === 'transparent'); b = b.parentElement) bg = getComputedStyle(b).backgroundColor;
      const br = bar.getBoundingClientRect();
      tpl.tab = { node: wrap.cloneNode(true), x: Math.round(wrap.getBoundingClientRect().x - col.getBoundingClientRect().x), h: Math.round(br.height), bg: bg === 'rgba(0, 0, 0, 0)' ? '' : bg };
    }
  }
  // Clones keep Outlook's classes but none of its handlers; strip what would mislead
  // (action buttons, ids, focus stops) and keep the read-state bar as decoration.
  function stripClone(n) {
    n.removeAttribute('id');
    for (const e of n.querySelectorAll('[id]')) e.removeAttribute('id');
    for (const e of n.querySelectorAll('[tabindex], input, button')) e.tabIndex = -1;
    for (const b of n.querySelectorAll('button[aria-label]')) b.remove();
    for (const d of n.querySelectorAll('[title="Delete"]')) d.replaceChildren();
    for (const e of n.querySelectorAll('[title="Delete"], button[title^="Mark as"]')) e.removeAttribute('title');
  }
  function cloneGroup(name, open) {
    if (!tpl.group) return null;
    const g = tpl.group.node.cloneNode(true);
    stripClone(g);
    const t = nodeAt(g, tpl.group.title);
    if (t) { t.textContent = name; t.title = name; }
    g.setAttribute('aria-label', name);
    g.setAttribute('aria-expanded', String(open));
    g.classList.add('olkt-ngrp');
    g.tabIndex = 0;
    return g;
  }
  function cloneRow(m, now) {
    // Read and unread look different; until Outlook has shown that kind, the drawn row stands in.
    const t = m.read ? tpl.read : tpl.unread;
    if (!t) return null;
    const row = t.node.cloneNode(true);
    stripClone(row);
    // Status icons (replied, attachment, flag) belong to the template's message, not this one.
    for (const e of row.querySelectorAll('svg, i')) if (!e.closest('[role="img"]')) e.remove();
    const set = (k, text, title) => { const n = nodeAt(row, t.paths[k]); if (n) { n.textContent = text; if (title) n.title = title; else n.removeAttribute('title'); } };
    const when = new Date(m.received);
    set('from', m.from || '?', m.from);
    set('subj', m.subject || '(no subject)');
    set('time', listTime(when, now), when.toLocaleString());
    set('prev', m.preview);
    const c = coin(m.from || '?');
    const img = nodeAt(row, t.paths.img);
    if (img) { img.style.background = c.style.background; img.style.color = c.style.color; img.removeAttribute('aria-label'); }
    set('initials', c.textContent);
    row.setAttribute('role', 'button');
    row.removeAttribute('aria-selected');
    row.setAttribute('aria-label', `${m.from}: ${m.subject}`);
    row.classList.add('olkt-nrow');
    row.tabIndex = 0;
    return row;
  }

  function otherRow(m, now) {
    const native = cloneRow(m, now);
    const row = native || el('div', 'olkt-orow' + (m.read ? '' : ' unread'));
    row.title = `${m.from}: ${m.subject}`;
    row.setAttribute('role', 'button');
    row.tabIndex = 0;
    if (!native) row.append(coin(m.from || '?'), el('span', 'olkt-ofrom', m.from), el('span', 'olkt-osubj', m.subject || '(no subject)'),
      el('span', 'olkt-otime', listTime(new Date(m.received), now)), el('span', 'olkt-oprev', m.preview));
    onPress(row, () => {
      m.read = true;
      otherAsked = Date.now() - OTHER_REFRESH + 8000; // refetch shortly, once Outlook has marked it read
      openInOther(m);
    });
    return row;
  }

  // The reading pane only opens messages from the list Outlook has loaded, so a row switches
  // the inbox to its Other tab and selects the message there (Focused comes back with its tab).
  // Plain clicks work on the tab and on list rows. Not found after scrolling: its own window.
  // Rows carry no id, so a message is found by subject and sender; among look-alikes
  // (a weekly newsletter) its rank in our newest-first list picks the same rank in Outlook's.
  let openRun = 0;
  async function openInOther(m) {
    const run = ++openRun, account = otherAccount;
    const otherTab = () => [...document.querySelectorAll('[role="tab"]')].find(t => t.textContent.startsWith('Other'));
    const tab = otherTab();
    if (!tab) return;
    tab.click();
    const same = x => x.subject === m.subject && x.from === m.from;
    const rank = ((other && other.messages) || []).filter(same).findIndex(x => x.id === m.id);
    const needle = (m.subject || m.from || '').trim();
    const time = listTime(new Date(m.received), new Date());
    let shown = false;
    for (let i = 0; i < 40; i++) {
      await sleep(150);
      // Given up when another row was clicked, the tweaks were replaced, or the user left the Other tab.
      if (run !== openRun || !current() || inboxAccount() !== account) return;
      const t = otherTab();
      const selected = !!t && t.getAttribute('aria-selected') === 'true';
      if (shown && !selected) return;
      shown = shown || selected;
      const lb = document.querySelector(`${MSGLIST} [role="listbox"]`);
      if (!lb || !selected || !needle) continue;
      const found = [...lb.querySelectorAll('[role="option"]')].filter(o => {
        const label = o.getAttribute('aria-label') || '';
        return label.includes(needle) && (!m.from || label.includes(m.from));
      });
      // The list time tells look-alikes apart when it is unique; otherwise rank decides.
      const byTime = found.filter(o => (o.getAttribute('aria-label') || '').includes(time));
      const opt = byTime.length === 1 ? byTime[0] : rank >= 0 ? found[rank] : found[0];
      if (opt) { opt.scrollIntoView({ block: 'nearest' }); opt.click(); return; }
      // Older messages are not rendered yet: page the list down (every few tries, once it has loaded).
      if (i % 4 === 3) {
        let sc = lb;
        while (sc && sc !== document.body && !/(auto|scroll)/.test(getComputedStyle(sc).overflowY)) sc = sc.parentElement;
        if (sc && sc !== document.body) sc.scrollTop += sc.clientHeight;
      }
    }
    // Not found while still on Other: its own window (deeplinks only reach the primary mailbox).
    if (run === openRun && current() && account === primary())
      window.open('https://outlook.office.com/mail/deeplink/read/' + encodeURIComponent(m.id));
  }

  function bindOtherGrip(grip) {
    grip.addEventListener('pointerdown', ev => {
      ev.preventDefault();
      ev.stopPropagation();
      const col = otherEl.parentElement, startY = ev.clientY, startH = otherEl.getBoundingClientRect().height;
      grip.setPointerCapture(ev.pointerId);
      const move = e => {
        otherHeight = Math.max(80, Math.min(col.clientHeight - 120, Math.round(startH + startY - e.clientY)));
        otherEl.style.height = otherHeight + 'px';
      };
      const up = () => {
        grip.removeEventListener('pointermove', move);
        grip.removeEventListener('pointerup', up);
        grip.removeEventListener('pointercancel', up);
        try { store.setItem(OTHER_H_KEY, String(otherHeight)); } catch {}
      };
      grip.addEventListener('pointermove', move);
      grip.addEventListener('pointerup', up);
      grip.addEventListener('pointercancel', up, { once: true });
    });
    grip.addEventListener('dblclick', () => { otherHeight = 0; try { store.removeItem(OTHER_H_KEY); } catch {} renderOther(); });
  }

  // ==== reading pane ====
  // To and Cc share one line (click it for the full list); the blocked-content bar shrinks
  // to a chip with its two buttons; the body stops at about 90 characters a line.
  const WELL = '[data-testid="RecipientWell"]';
  // ==== print ====
  // Outlook prints the page itself (Ctrl+P goes straight to the browser's print view), so
  // print is a stylesheet: everything but the open message is hidden, the panes that scroll
  // are released so the whole message prints, and the message is laid out for paper.
  const READING = '[role="main"][aria-label="Reading Pane"]';
  const BODY = '[role="document"][aria-label="Message body"]';
  // A4 and Letter are both at least 8.27in wide; 7.3in of that is printable at the usual
  // half-inch margins, which is 700px at the 96dpi CSS inch.
  const PRINT_WIDTH = 700;
  function printCss(out) {
    const p = cfg.print;
    out.push('@media print{');
    // Only when a message is open: printing the calendar or any other view must stay
    // Outlook's business, and hiding everything there would print a blank page.
    const HAS = `body:has(${READING})`;
    // Hidden, not removed: Outlook's layout assumes its panes exist.
    out.push(`${HAS} *{visibility:hidden!important}`);
    // Scoped the same way, or the rule above (more specific now) would hide the message too.
    out.push(`${HAS} ${READING},${HAS} ${READING} *{visibility:visible!important}`);
    out.push(`${READING}{position:absolute!important;left:0!important;top:0!important;width:100%!important;max-width:none!important;height:auto!important;overflow:visible!important}`);
    // Every scroller and fixed height between the pane and the message body, or only the
    // part that happens to be on screen reaches the paper.
    out.push(`${READING} *{height:auto!important;max-height:none!important;overflow:visible!important}`);
    out.push(`${BODY}{max-width:none!important;width:auto!important}`);
    // Nothing interactive belongs on paper, except the recipients: Outlook draws the whole
    // To and Cc line as one button, so the well is exempt or the names leave with it.
    out.push(`${READING} [role="toolbar"],${READING} [role="menubar"],${READING} input,${READING} textarea{display:none!important}`);
    out.push(`${READING} button:not(${WELL}):not(${WELL} button),${READING} [role="button"]:not(${WELL}):not(${WELL} [role="button"]){display:none!important}`);
    out.push(`.olkt-agenda,.olkt-other,.olkt-other-toggle,.olkt-compose,.olkt-head,.olkt-resize{display:none!important}`);
    // Paper is white. Under a dark theme Outlook rewrites the message with inline
    // `color: ... !important`, which no stylesheet can outrank, so the text is painted black
    // through its fill colour, which those inline rules do not set.
    out.push(`${READING}{background:#fff!important}`);
    out.push(`${READING} *{background-color:transparent!important;box-shadow:none!important}`);
    out.push(`${READING},${READING} *{color:#000!important;-webkit-text-fill-color:#000!important}`);
    out.push(`${BODY} a{text-decoration:underline}`);
    // The one-line recipients rule relies on clipping, which print undoes, so the names would
    // otherwise spill across the date. On paper they wrap instead.
    // Written through the pane so these outrank the screen rule, which is more specific than a
    // bare `WELL` selector and would otherwise keep the line in its one-line form.
    const printWell = `${READING} ${WELL}:not([data-olkt-open])`;
    out.push(`${printWell}{display:block!important}`);
    out.push(`${printWell} > div{flex:none!important;display:block!important;max-width:100%!important;overflow:visible!important}`);
    // Outlook measures each line and pins its width inline, so the names must be let out of it.
    out.push(`${printWell} .EditorClass{display:block!important;width:auto!important;max-width:100%!important;white-space:normal!important;overflow:visible!important;text-overflow:clip!important}`);
    out.push(`${READING} [data-olkt-collapsed]{display:none!important}`);
    if (p.compactHeader) {
      // The sender block, its avatar and the chips above the message take a third of page one.
      // Scoped out of the message body: an email's own picture can carry the same markup as
      // a sender avatar, and a hidden one cannot be brought back by a later rule.
      out.push(`${READING} img[alt][role="img"]:not(${BODY} *),${READING} [data-testid="persona-coin"]:not(${BODY} *),${READING} .fui-Avatar:not(${BODY} *){display:none!important}`);
      out.push(`${WELL}{flex-direction:row!important;flex-wrap:wrap!important;gap:6px!important;font-size:11px!important}`);
      out.push(`${READING} [role="heading"]{font-size:15px!important;line-height:1.25!important;margin:0 0 2px!important}`);
      out.push(`${READING} .fui-MessageBar,${READING} [data-olkt-chip]{display:none!important}`);
    }
    if (p.keepWhole) out.push(`${BODY} img,${BODY} table,${BODY} figure,${BODY} svg,${BODY} pre{break-inside:avoid;page-break-inside:avoid;max-width:100%!important}`);
    if (p.quoted !== 'show') {
      // Each form repeats the pane, because the black fill above outranks a bare attribute.
      const parts = [`${READING} [data-olkt-quoted]`, `${BODY} blockquote`, `${BODY} #divRplyFwdMsg`, `${BODY} #divRplyFwdMsg ~ *`, `${BODY} [id^="appendonsend"]`, `${BODY} .OutlookMessageHeader`, `${BODY} .gmail_quote`];
      const quoted = parts.join(',');
      if (p.quoted === 'hide') out.push(`${quoted}{display:none!important}`);
      else {
        // The text itself sits in descendants, and each one needs its own selector: a list
        // followed by " *" would only reach the children of its last entry.
        out.push(`${parts.concat(parts.map(x => x + ' *')).join(',')}{color:#666!important;-webkit-text-fill-color:#666!important}`);
        out.push(`${quoted}{font-size:97%!important}`);
      }
    }
    if (p.fit !== 'none') {
      // Only when the pass below found something too wide: a transformed box is a single unit
      // in paged layout, so scaling a message that fits would cost every page after the first.
      out.push(`[data-olkt-fit] ${BODY}{transform:scale(var(--olkt-print-scale,1));transform-origin:top left;width:calc(100% / var(--olkt-print-scale,1))!important}`);
    }
    out.push('}');
  }

  // Just before the print view renders, measure the widest thing in the message and, when it
  // cannot fit the page, scale the body down so a receipt or a fixed-width table arrives whole
  // instead of cropped. 'always' fits every message to the page width.
  function printScale() {
    const p = cfg.print;
    if (p.fit === 'none') return 1;
    const body = document.querySelector(BODY);
    if (!body) return 1;
    // The body itself is held to the reading width, so measuring it would scale down a
    // message that fits. What matters is the widest thing inside it.
    let widest = 0;
    for (const e of body.querySelectorAll('table,img,pre,figure,div,p')) widest = Math.max(widest, e.scrollWidth || 0);
    if (!widest) return 1;
    if (p.fit === 'overflow' && widest <= PRINT_WIDTH) return 1;
    return Math.max(0.4, Math.min(1, PRINT_WIDTH / widest));
  }
  // A thread's older messages carry no class or id of their own: the history starts at a rule
  // whose next elements are the previous message's "From:" block. Marked here rather than in
  // the stylesheet, so a rule used inside a real message is left alone.
  const QUOTED_HEAD = /^\s*from\s*:/i;
  // Two fields or more: one "Date:" line in a receipt is not a quoted message.
  const QUOTED_FIELDS = /[\n\r]\s*(sent|date|to|cc|subject)\s*:/gi;
  const looksQuotedHead = text => {
    const t = (text || '').trim();
    return QUOTED_HEAD.test(t) && (t.match(QUOTED_FIELDS) || []).length >= 1;
  };
  // "+2 others" and the like: the short form of a recipient line, which print would otherwise
  // draw over the full list it stands in for.
  const COLLAPSED = /^\s*\+\s*\d+\s+(others?|more)\b/i;
  function markCollapsed() {
    const pane = document.querySelector(READING);
    const well = pane && pane.querySelector(WELL);
    if (!well) return;
    for (const e of well.querySelectorAll('*')) {
      if (e.children.length) continue;
      if (COLLAPSED.test(e.textContent || '')) {
        let n = e;
        // The chip is usually a span inside a small wrapper; hide the wrapper when it holds
        // nothing else, so its spacing goes too.
        while (n.parentElement && n.parentElement !== well && n.parentElement.children.length === 1) n = n.parentElement;
        n.setAttribute('data-olkt-collapsed', '');
      }
    }
  }
  const clearCollapsed = () => document.querySelectorAll('[data-olkt-collapsed]').forEach(n => n.removeAttribute('data-olkt-collapsed'));
  function markQuoted() {
    const body = document.querySelector(BODY);
    if (!body) return;
    const mark = from => { for (let n = from; n; n = n.nextElementSibling) n.setAttribute('data-olkt-quoted', ''); };
    // A rule followed by the previous message's header. Spacer nodes carry no text and sit
    // between the two in most threads, so they are stepped over rather than counted.
    for (const hr of body.querySelectorAll('hr')) {
      let looks = false, checked = 0;
      for (let n = hr.nextElementSibling; n && checked < 4 && !looks; n = n.nextElementSibling) {
        const t = (n.innerText || '').trim();
        if (!t) continue;
        checked++;
        looks = looksQuotedHead(t);
      }
      if (looks) mark(hr);
    }
    // Or the header block itself, for a thread that separates with spacing instead of a rule.
    for (const e of body.querySelectorAll('div,p,blockquote,table')) {
      if (e.hasAttribute('data-olkt-quoted') || e === body.firstElementChild) continue;
      const t = (e.innerText || '').trim();
      // Only the header block itself, never a wrapper that happens to begin with one: marking
      // an ancestor would take the whole message with it.
      if (!looksQuotedHead(t)) continue;
      if (e.querySelector('div,p,blockquote,table') && t.length > 600) continue;
      mark(e);
    }
  }
  const clearQuoted = () => document.querySelectorAll('[data-olkt-quoted]').forEach(n => n.removeAttribute('data-olkt-quoted'));
  const onBeforePrint = () => {
    try {
      // A print that never reported finishing leaves its marks behind.
      clearQuoted();
      clearCollapsed();
      if (cfg.print.quoted !== 'show') markQuoted();
      markCollapsed();
      const scale = printScale();
      document.documentElement.toggleAttribute('data-olkt-fit', scale < 1);
      document.documentElement.style.setProperty('--olkt-print-scale', String(Math.round(scale * 1000) / 1000));
    } catch {}
  };
  const onAfterPrint = () => {
    try {
      clearQuoted();
      clearCollapsed();
      document.documentElement.removeAttribute('data-olkt-fit');
      document.documentElement.style.removeProperty('--olkt-print-scale');
    } catch {}
  };

  function readingCss(out) {
    if (S.oneLineRecipients) {
      out.push(`${WELL}:not([data-olkt-open]){flex-direction:row!important;flex-wrap:nowrap!important;gap:14px;overflow:hidden;cursor:pointer}`);
      // Outlook fits names to each line's measured width, so give the lines fixed shares.
      out.push(`${WELL}:not([data-olkt-open]) > div{flex:1 1 0;min-width:0;overflow:hidden}`);
      out.push(`${WELL}:not([data-olkt-open]) > div:first-child{flex:0 0 52%}`);
      out.push(`${WELL}:not([data-olkt-open]) .EditorClass{white-space:nowrap!important;overflow:hidden;text-overflow:ellipsis}`);
    }
    if (S.blockedContentChip) {
      out.push(`[data-olkt-chip]{min-height:0!important;padding-top:2px!important;padding-bottom:2px!important}`);
      out.push(`[data-olkt-chip] .fui-MessageBarBody > *{display:none!important}`);
      out.push(`[data-olkt-chip] .fui-MessageBarBody::before{content:"Blocked content";font-size:12px;line-height:20px}`);
      out.push(`[data-olkt-chip] .fui-MessageBarActions button{min-height:22px!important;height:22px!important;padding:0 8px!important;font-size:12px!important}`);
    }
    // A CSS length only (letters, digits, dot, percent), so a typo cannot break out of the rule.
    if (/^[\w.%]+$/.test(cfg.readingWidth)) out.push(`[role="document"][aria-label="Message body"]{max-width:${cfg.readingWidth}}`);
  }
  function ensureReading() {
    if (!S.blockedContentChip) return;
    // Every bar, every pass: Outlook reuses the bar node for a different message.
    for (const bar of document.querySelectorAll('.fui-MessageBar')) {
      const blocked = /content in this message has been blocked/i.test(bar.textContent);
      if (blocked !== bar.hasAttribute('data-olkt-chip')) bar.toggleAttribute('data-olkt-chip', blocked);
    }
  }
  // An opened recipient line closes again when another message is picked (the well node is reused).
  const MSGLIST = '[role="complementary"][aria-label="Message list"]';
  const closeWells = () => document.querySelectorAll(`${WELL}[data-olkt-open]`).forEach(w => w.removeAttribute('data-olkt-open'));
  const onWellPress = ev => {
    if (!S.oneLineRecipients || !ev.target.closest) return;
    if (ev.target.closest(`${MSGLIST} [role="option"]`)) return closeWells();
    const well = ev.target.closest(WELL);
    if (well && !well.hasAttribute('data-olkt-open')) well.setAttribute('data-olkt-open', '');
  };
  const onWellKey = ev => {
    if (/^(ArrowUp|ArrowDown|Home|End|PageUp|PageDown)$/.test(ev.key) && ev.target.closest && ev.target.closest(MSGLIST)) closeWells();
  };

  const shortHour = t => { const m = /^(\d{1,2})(?::\d{2})?\s*([AP])M$/i.exec(t.trim()); return m && { label: m[1] + m[2].toLowerCase(), h24: (Number(m[1]) % 12) + (/p/i.test(m[2]) ? 12 : 0) }; };
  function ensureCalendar() {
    const nav = document.querySelector(CALNAV);
    if (!nav) return;
    // Account rows: label the primary "Calendar" row of each account with its short name.
    for (const head of S.calendarRowPerAccount ? nav.querySelectorAll('[id^="calendarAccountGroupHeader"]') : []) {
      const email = (/'([^']+)'/.exec(head.title) || [])[1];
      if (!email) continue;
      const key = email.toLowerCase();
      const label = (CAL.labels && CAL.labels[key]) || (cfg.accounts || {})[key] || key.split('@')[0];
      const group = head.parentElement.parentElement;
      for (const opt of group.querySelectorAll(`[role="option"][title$=" calendar 'Calendar'"]`)) {
        const lbl = [...opt.querySelectorAll('div')].find(d => d.textContent.trim() === 'Calendar');
        if (lbl && lbl.getAttribute('data-olkt-label') !== label) lbl.setAttribute('data-olkt-label', label);
      }
    }
    // Time-zone gutters (Outlook can draw more than one strip): in each strip the columns
    // run left to right and the last one, next to the grid, is the primary zone.
    const mark = (list, strip) => {
      const groups = new Map();
      for (const c of list) { const k = strip(c); if (!groups.has(k)) groups.set(k, []); groups.get(k).push(c); }
      for (const g of groups.values()) {
        g.sort((a, b) => a.getBoundingClientRect().x - b.getBoundingClientRect().x);
        g.forEach((c, i) => { const v = i === g.length - 1 ? 'pri' : 'sec'; if (c.getAttribute('data-olkt-tz') !== v) c.setAttribute('data-olkt-tz', v); });
      }
    };
    // Gutter labels are whole hours stacked in one column; event tiles use <time> too.
    const cols = !S.compactTimeZones ? [] : [...new Set([...document.querySelectorAll('time')].map(t => t.parentElement))]
      .filter(c => [...c.children].filter(t => t.tagName === 'TIME' && /^\d{1,2} [AP]M$/i.test(t.textContent.trim())).length >= 12);
    const times = cols.flatMap(c => [...c.children].filter(t => t.tagName === 'TIME' && shortHour(t.textContent)));
    // Ordering reads layout, so it runs only when a column is new or no column is primary
    // (zones removed or swapped); this loop runs every frame.
    const stale = list => list.length && (list.some(c => !c.hasAttribute('data-olkt-tz')) || !list.some(c => c.getAttribute('data-olkt-tz') === 'pri'));
    if (stale(cols)) mark(cols, c => c.parentElement);
    for (const header of S.compactTimeZones ? document.querySelectorAll('[data-tabid="surfaceHeader_timeStripHeader"]') : []) {
      const heads = [...header.querySelectorAll('p')].map(p => p.parentElement);
      if (stale(heads)) mark(heads, () => header);
    }
    for (const t of times) {
      const s = shortHour(t.textContent);
      if (t.getAttribute('data-olkt-short') !== s.label) t.setAttribute('data-olkt-short', s.label);
      t.toggleAttribute('data-olkt-off', s.h24 < 9 || s.h24 >= 17);
    }
    // Tiles: the time from the tile's own label, under the title.
    for (const tile of S.tileTimes ? document.querySelectorAll(CALTILE) : []) {
      const m = /(\d{1,2}:\d{2}) ([AP]M) to (\d{1,2}:\d{2}) ([AP]M)/.exec(tile.getAttribute('aria-label') || '');
      const title = tile.firstElementChild && tile.firstElementChild.firstElementChild;
      if (!title) continue;
      if (!m) { title.removeAttribute('data-olkt-time'); continue; } // Outlook reuses tile nodes
      const when = m[2] === m[4] ? `${m[1]} to ${m[3]} ${m[4]}` : `${m[1]} ${m[2]} to ${m[3]} ${m[4]}`;
      if (title.getAttribute('data-olkt-time') !== when) title.setAttribute('data-olkt-time', when);
    }
    if (cfg.calendarOverlap !== false && document.querySelector('[data-tabid="surfaceHeader_timeStripHeader"]')) ensureOverlap();
  }

  // Timed tiles sit in one container per day with a percent inset (top and bottom are the
  // times), so overlaps come from the styles alone, with no layout reads. A container is
  // redone only when its tiles change (its signature), which keeps this loop from feeding itself.
  const overlapDays = new Set(); // day boxes holding marks or merged tiles, to clean when they empty
  function ensureOverlap() {
    const days = new Map();
    for (const it of document.querySelectorAll('[data-calitemid]')) {
      const top = parseFloat(it.style.top), bottom = parseFloat(it.style.bottom);
      if (!it.style.top.endsWith('%') || !it.style.bottom.endsWith('%') || !it.parentElement) continue;
      // Each tile has a wrapper of its own; the day is the positioned box holding the wrappers.
      const day = it.parentElement.children.length === 1 && it.parentElement.parentElement ? it.parentElement.parentElement : it.parentElement;
      if (!days.has(day)) days.set(day, []);
      days.get(day).push({ it, s: top, e: 100 - bottom });
    }
    for (const day of overlapDays) {
      if (days.has(day)) continue;
      overlapDays.delete(day);
      day.querySelectorAll(':scope > .olkt-merge').forEach(n => n.remove());
      delete day.dataset.olktSig;
    }
    for (const [day, evs] of days) {
      // Redone when tiles move, are renamed, or are re-created as new nodes (which lose their marks).
      const label = it => (it.querySelector('[role="button"]') || it).getAttribute('aria-label') || '';
      const sig = evs.map(v => `${v.it.getAttribute('data-calitemid')}@${v.s},${v.e}:${label(v.it)}`).sort().join('|');
      const marked = evs.filter(v => v.it.hasAttribute('data-olkt-ov')).length;
      if (day.dataset.olktSig === sig && marked === Number(day.dataset.olktMarked || 0) &&
        day.querySelectorAll(':scope > .olkt-merge').length === Number(day.dataset.olktMerges || 0)) continue;
      day.dataset.olktSig = sig;
      overlapDays.add(day);
      day.querySelectorAll(':scope > .olkt-merge').forEach(n => n.remove());
      evs.sort((a, b) => a.s - b.s || (b.e - b.s) - (a.e - a.s));
      const clusters = [];
      for (const v of evs) {
        const c = clusters[clusters.length - 1];
        if (c && v.s < c.end - 0.01) { c.list.push(v); c.end = Math.max(c.end, v.e); } else clusters.push({ list: [v], end: v.e });
      }
      for (const { list } of clusters) {
        const mode = list.length === 1 ? null : list.length < (CAL.mergeAt || 3) ? 'c' : 'm';
        list.forEach((v, i) => {
          const val = mode === 'c' ? 'c' + Math.min(i, 3) : mode;
          if (val) v.it.setAttribute('data-olkt-ov', val); else v.it.removeAttribute('data-olkt-ov');
        });
        if (mode === 'm') day.appendChild(mergedTile(list));
      }
      day.dataset.olktMerges = clusters.filter(c => c.list.length > 1 && c.list.length >= (CAL.mergeAt || 3)).length;
      day.dataset.olktMarked = clusters.reduce((n, c) => n + (c.list.length > 1 ? c.list.length : 0), 0);
    }
  }
  function mergedTile(list) {
    const parse = v => {
      const label = (v.it.querySelector('[role="button"]') || v.it).getAttribute('aria-label') || '';
      const m = /^(.*?), (\d{1,2}:\d{2} [AP]M) to (\d{1,2}:\d{2} [AP]M), /.exec(label);
      // Events with an icon start their label with it ("Gym icon, Gym Session, ...").
      const title = s => s.replace(/^[A-Za-z ]+ icon, /, '');
      return m ? { title: title(m[1]), from: m[2], to: m[3] } : { title: title(label).split(',')[0], from: '', to: '' };
    };
    const s = Math.min(...list.map(v => v.s)), e = Math.max(...list.map(v => v.e));
    const first = parse(list.find(v => v.s === s)), last = parse(list.find(v => v.e === e));
    const t = el('div', 'olkt-merge');
    t.style.top = s + '%';
    t.style.height = (e - s) + '%';
    t.style.setProperty('--h', (e - s) + '%');
    t.append(el('div', 'ct', `${list.length} events, ${first.from} to ${last.to}`));
    for (const v of list) {
      const info = parse(v);
      const btn = v.it.querySelector('[role="button"]');
      const r = el('div', 'crow');
      r.setAttribute('role', 'button');
      r.tabIndex = 0;
      r.title = `${info.title}, ${info.from} to ${info.to}`;
      // The tile's own color strip (a narrow colored box at its left) marks the row and bar.
      const strip = [...v.it.querySelectorAll('div')].find(d => d.getBoundingClientRect().width <= 8 && getComputedStyle(d).backgroundColor !== 'rgba(0, 0, 0, 0)');
      if (strip) r.style.setProperty('--edge', getComputedStyle(strip).backgroundColor);
      const bar = el('div', 'bar'), i = el('i');
      i.style.left = ((v.s - s) / (e - s)) * 100 + '%';
      i.style.width = ((v.e - v.s) / (e - s)) * 100 + '%';
      bar.append(i);
      r.append(el('b', null, info.title), el('small', null, `${info.from} to ${info.to}`), bar);
      // Looked up at click time: Outlook may have re-created the tile since this was built.
      const id = v.it.getAttribute('data-calitemid');
      onPress(r, () => { const b = document.querySelector(`[data-calitemid="${CSS.escape(id)}"] [role="button"]`) || btn; if (b) b.click(); }); // Outlook opens its own event card
      t.append(r);
    }
    return t;
  }

  // ==== render loop ====
  let style;
  let boundList = null;
  let lastNames = '';
  function unbindList(list) {
    list.removeEventListener('contextmenu', onListContextMenu, true);
    list.removeEventListener('drop', onListRelease, true);
    list.removeEventListener('pointerup', onListRelease, true);
  }
  function ensure() {
    ensureCalendar();
    ensureReading();
    ensureAgendaData();
    ensureOther();
    const list = favoritesList();
    if (!list) return;
    if (!list.hasAttribute('data-olkt-favs')) list.setAttribute('data-olkt-favs', '');
    if (boundList !== list) {
      if (boundList) unbindList(boundList);
      boundList = list;
      list.addEventListener('contextmenu', onListContextMenu, true);
      list.addEventListener('drop', onListRelease, true);
      list.addEventListener('pointerup', onListRelease, true);
    }
    ensureCompose(list);
    if (adoptNewFavorites(list)) { save(); return; }
    // Favorites added or renamed by Outlook change what the pattern groups hold.
    const names = favoriteNames().join('|');
    if (names !== lastNames) { lastNames = names; render(); return; }
    ensureHeaders(list);
    ensureAccountsHeader();
    ensureAccountDrag();
    ensureResize();
    if (agenda && (!agendaEl || !agendaEl.isConnected)) renderAgenda();
  }

  function render() {
    if (!current()) { observer.disconnect(); return; }
    document.documentElement.style.setProperty('--olkt-pane-w', paneWidth + 'px');
    if (!style || !style.isConnected) {
      style = document.createElement('style');
      style.id = 'olkt-style';
      document.head.appendChild(style);
    }
    style.textContent = buildCss();
    ensure();
  }

  // Typing in a message body and our own redraws (agenda, Other card, menus) cannot
  // change anything ensure() looks at, so those batches are skipped.
  const EDITING = '[contenteditable]:not([contenteditable="false"])';
  const ignorable = r => {
    const t = r.target.nodeType === 1 ? r.target : r.target.parentElement;
    return !!t && !!t.closest(`${EDITING}, ${OURS}`);
  };

  let pending = false;
  const observer = new MutationObserver(records => {
    if (!current()) { observer.disconnect(); return; }
    if (pending || records.every(ignorable)) return;
    pending = true;
    requestAnimationFrame(() => {
      pending = false;
      // First time the tree appears: rebuild the CSS with Outlook's real label size.
      if (!fs && document.querySelector('[role="treeitem"] > span')) render(); else ensure();
    });
  });

  // A resize can leave Outlook's own list rebuilt with nothing of ours in it, and the
  // mutations that did it are the kind we skip, so the pane is rebuilt here instead.
  let resizeTimer = 0;
  const onResize = () => {
    clearTimeout(resizeTimer);
    resizeTimer = setTimeout(() => { if (current()) render(); }, 250);
  };
  const onDocPointer = ev => { if (!ev.target.closest('.olkt-menu, .olkt-rename')) closeMenus(); };
  const onDocKey = ev => { if (ev.key === 'Escape') closeMenus(); };

  function start() {
    render();
    observer.observe(document.body, { childList: true, subtree: true, attributes: true, attributeFilter: ['title'] });
    document.addEventListener('pointerdown', onDocPointer, true);
    document.addEventListener('pointerdown', onWellPress, true);
    document.addEventListener('keydown', onDocKey, true);
    document.addEventListener('keydown', onWellKey, true);
    window.addEventListener('resize', onResize);
    window.addEventListener('beforeprint', onBeforePrint);
    window.addEventListener('afterprint', onAfterPrint);
  }

  window.__olkt = {
    // `full` is passed by the mod when it is being disabled or unloaded: on top of the
    // handover cleanup it puts fetch and XHR back, so nothing of ours is left running in
    // a window that stays open. A newer install calls it without `full` and re-wraps.
    remove(full) {
      observer.disconnect();
      window.__olktGen = gen + 1;
      document.removeEventListener('pointerdown', onDocPointer, true);
      document.removeEventListener('pointerdown', onWellPress, true);
      document.removeEventListener('keydown', onDocKey, true);
      document.removeEventListener('keydown', onWellKey, true);
      window.removeEventListener('resize', onResize);
      clearTimeout(resizeTimer);
      window.removeEventListener('beforeprint', onBeforePrint);
      window.removeEventListener('afterprint', onAfterPrint);
      onAfterPrint();
      session.off(onSession);
      clearTimeout(sessionTimer);
      if (boundList) unbindList(boundList);
      if (acctListBound) acctListBound.removeEventListener('pointerdown', onAcctPress, true);
      clearInterval(agendaTimer);
      document.querySelectorAll(`${PANE} .customScrollBar`).forEach(n => { n.style.paddingBottom = ''; n.style.maxHeight = ''; });
      document.querySelectorAll(OURS).forEach(n => n.remove());
      document.querySelectorAll('[data-olkt-favs]').forEach(n => n.removeAttribute('data-olkt-favs'));
      for (const a of ['data-olkt-label', 'data-olkt-tz', 'data-olkt-short', 'data-olkt-off', 'data-olkt-time', 'data-olkt-chip', 'data-olkt-open', 'data-olkt-othertab', 'data-olkt-ov', 'data-olkt-sig', 'data-olkt-merges', 'data-olkt-marked', 'data-olkt-quoted', 'data-olkt-collapsed'])document.querySelectorAll(`[${a}]`).forEach(n => n.removeAttribute(a));
      delete window.__olkt;
      if (full) session.stop();
      return 'removed';
    },
    config: () => cfg,
    agenda: setAgenda,
    other: setOther,
    month: setMonth,
    refresh: () => { agendaAsked = 0; ensureAgendaData(true); if (otherAccount) askOther(); return 'refreshing'; },
  };
  if (document.body) start(); else addEventListener('DOMContentLoaded', start, { once: true });
  return 'installed';
})())olkt";

// ==== settings to OLKT_CONFIG ====

// A JSON string literal. U+2028 and U+2029 are escaped too: JSON allows them raw,
// older JavaScript string literals do not.
std::wstring JsonString(PCWSTR text) {
    std::wstring out = L"\"";
    for (const wchar_t* p = text; *p; ++p) {
        wchar_t c = *p;
        if (c == L'"' || c == L'\\') {
            out += L'\\';
            out += c;
        } else if (c < 0x20 || c == 0x2028 || c == 0x2029) {
            wchar_t buf[8];
            swprintf(buf, 8, L"\\u%04x", (unsigned)c);
            out += buf;
        } else {
            out += c;
        }
    }
    return out + L"\"";
}

std::wstring StringSetting(PCWSTR name, ...) {
    va_list args;
    va_start(args, name);
    wchar_t key[128];
    vswprintf(key, 128, name, args);
    va_end(args);
    PCWSTR value = Wh_GetStringSetting(key);
    std::wstring out = value;
    Wh_FreeStringSetting(value);
    return out;
}

void AddField(std::wstring& json, PCWSTR name, const std::wstring& value) {
    if (json.size() > 1) json += L",";
    json += JsonString(name) + L":" + value;
}

void AddBool(std::wstring& json, PCWSTR name) { AddField(json, name, Wh_GetIntSetting(name) ? L"true" : L"false"); }
void AddInt(std::wstring& json, PCWSTR name) { AddField(json, name, std::to_wstring(Wh_GetIntSetting(name))); }
void AddString(std::wstring& json, PCWSTR name) { AddField(json, name, JsonString(StringSetting(name).c_str())); }

// A list setting as a JSON array. `fields` empty: a list of strings; otherwise a
// list of objects with those fields. Entries whose first field is empty are skipped.
void AddList(std::wstring& json, PCWSTR name, std::initializer_list<PCWSTR> fields) {
    std::wstring arr = L"[";
    for (int i = 0; i < 100; ++i) {
        std::wstring item;
        if (fields.size() == 0) {
            std::wstring value = StringSetting(L"%s[%d]", name, i);
            if (value.empty()) continue;
            item = JsonString(value.c_str());
        } else {
            item = L"{";
            bool first = true;
            for (PCWSTR field : fields) {
                std::wstring value = StringSetting(L"%s[%d].%s", name, i, field);
                if (first && value.empty()) break;
                if (!first) item += L",";
                item += JsonString(field) + L":" + JsonString(value.c_str());
                first = false;
            }
            if (first) continue;
            item += L"}";
        }
        if (arr.size() > 1) arr += L",";
        arr += item;
    }
    AddField(json, name, arr + L"]");
}

// ==== the mod's own saved state ====

// Edits made inside Outlook (groups, renames, folds, account order, pane and card
// sizes) are the mod's state, not Outlook's, so they live in Windhawk's storage and
// leave with the mod. They are handed to the page in OLKT_CONFIG.store, and the page
// sends each change back over the WebView's message channel (see MessageReceived).
constexpr PCWSTR kStoreKeys[] = {
    L"olkt-favorites", L"olkt-account-order", L"olkt-folds",        L"olkt-pane-width",
    L"olkt-agenda-height", L"olkt-other-height", L"olkt-other-split", L"olkt-other-folds",
};
constexpr size_t kStoreMaxChars = 65536;

SRWLOCK g_storeLock = SRWLOCK_INIT;
std::map<std::wstring, std::wstring> g_store;

void LoadStore() {
    std::vector<wchar_t> buffer(kStoreMaxChars);
    AcquireSRWLockExclusive(&g_storeLock);
    g_store.clear();
    for (PCWSTR key : kStoreKeys) {
        buffer[0] = L'\0';
        if (Wh_GetStringValue(key, buffer.data(), buffer.size()) && buffer[0]) g_store[key] = buffer.data();
    }
    ReleaseSRWLockExclusive(&g_storeLock);
}

void AddStore(std::wstring& json) {
    std::wstring object = L"{";
    AcquireSRWLockShared(&g_storeLock);
    for (const auto& [key, value] : g_store) {
        if (object.size() > 1) object += L",";
        object += JsonString(key.c_str()) + L":" + JsonString(value.c_str());
    }
    ReleaseSRWLockShared(&g_storeLock);
    AddField(json, L"store", object + L"}");
}

SRWLOCK g_scriptLock = SRWLOCK_INIT;
std::wstring g_script;

// Wraps the page as `{ const OLKT_CONFIG = {...}; <page> }`. Windows created after a
// settings change get the new settings; open windows keep theirs until a restart.
void BuildScript() {
    std::wstring json = L"{";
    AddBool(json, L"favoritesGroups");
    AddList(json, L"hiddenFolders", {});
    AddList(json, L"composeButtons", {L"account", L"label", L"color"});
    AddList(json, L"accountLabels", {L"account", L"label"});
    AddString(json, L"primaryAccount");
    AddString(json, L"accountsHeading");
    AddBool(json, L"hideAccountTrees");
    AddBool(json, L"hideAddAccount");
    AddBool(json, L"resizablePane");
    AddInt(json, L"paneWidth");
    AddBool(json, L"agenda");
    AddInt(json, L"agendaDays");
    AddBool(json, L"monthHeatMap");
    AddBool(json, L"otherCard");
    AddBool(json, L"calendarRowPerAccount");
    AddInt(json, L"calendarPaneWidth");
    AddBool(json, L"compactTimeZones");
    AddBool(json, L"tileTimes");
    AddString(json, L"overlapLayout");
    AddInt(json, L"mergeAt");
    AddBool(json, L"oneLineRecipients");
    AddBool(json, L"blockedContentChip");
    AddString(json, L"readingWidth");
    AddBool(json, L"hideRibbonGroupLabels");
    AddBool(json, L"printCompactHeader");
    AddString(json, L"printFit");
    AddBool(json, L"printKeepChartsWhole");
    AddString(json, L"printQuotedReplies");
    AddStore(json);
    json += L"}";
    std::wstring script = L"{\nconst OLKT_CONFIG = " + json + L";\n" + kPageScript + L"\n}";
    AcquireSRWLockExclusive(&g_scriptLock);
    g_script.swap(script);
    ReleaseSRWLockExclusive(&g_scriptLock);
}

// ==== WebView2 hooks ====

void* Slot(IUnknown* obj, int index) {
    return (*reinterpret_cast<void***>(obj))[index];
}

// COM objects of ours that WebView2 still holds. Their code lives in this DLL, so
// Wh_ModUninit waits for them: a call into an unloaded mod would crash Outlook.
std::atomic<int> g_liveObjects{0};

using Invoke_t = HRESULT(STDMETHODCALLTYPE*)(IUnknown*, HRESULT, IUnknown*);
HRESULT Forward(IUnknown* handler, HRESULT hr, IUnknown* result) {
    return reinterpret_cast<Invoke_t>(Slot(handler, kHandler_Invoke))(handler, hr, result);
}

// EventRegistrationToken from eventtoken.h: one INT64, so an add/remove pair can be
// called through the vtable without pulling in the WinRT headers.
struct EventToken {
    INT64 value;
};

// ==== state the page sends back ====

// "olkt-store", the key, then the value, one per line; an empty value deletes the key.
// Outlook's own messages are JSON and never match, so they pass straight through.
void StoreMessage(PCWSTR text) {
    constexpr std::wstring_view kPrefix = L"olkt-store\n";
    std::wstring_view message(text);
    if (!message.starts_with(kPrefix)) return;
    message.remove_prefix(kPrefix.size());
    size_t line = message.find(L'\n');
    if (line == std::wstring_view::npos) return;
    std::wstring key(message.substr(0, line));
    std::wstring value(message.substr(line + 1));
    // Only the mod's own keys, and nothing big enough to bloat the registry: the page
    // is trusted no further than that.
    bool known = false;
    for (PCWSTR k : kStoreKeys) known = known || key == k;
    if (!known || value.size() >= kStoreMaxChars) return;
    AcquireSRWLockExclusive(&g_storeLock);
    if (value.empty()) {
        g_store.erase(key);
        Wh_DeleteValue(key.c_str());
    } else {
        g_store[key] = value;
        Wh_SetStringValue(key.c_str(), value.c_str());
    }
    ReleaseSRWLockExclusive(&g_storeLock);
    BuildScript();  // windows opened from here on start with the new state
}

class MessageReceived final : public IUnknown {
    std::atomic<ULONG> refs_{1};

   public:
    MessageReceived() { ++g_liveObjects; }
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid, void** out) override {
        if (IsEqualGUID(iid, kIID_IUnknown) || IsEqualGUID(iid, kIID_WebMessageReceived)) {
            *out = this;
            AddRef();
            return S_OK;
        }
        *out = nullptr;
        return E_NOINTERFACE;
    }
    ULONG STDMETHODCALLTYPE AddRef() override { return ++refs_; }
    ULONG STDMETHODCALLTYPE Release() override {
        ULONG n = --refs_;
        if (!n) {
            delete this;
            --g_liveObjects;
        }
        return n;
    }
    virtual HRESULT STDMETHODCALLTYPE Invoke(IUnknown* sender, IUnknown* args) {
        LPWSTR text = nullptr;
        using TryGet_t = HRESULT(STDMETHODCALLTYPE*)(IUnknown*, LPWSTR*);
        // Not a string message (Outlook posts objects): nothing of ours.
        if (FAILED(reinterpret_cast<TryGet_t>(Slot(args, kArgs_TryGetWebMessageAsString))(args, &text)) || !text) {
            return S_OK;
        }
        StoreMessage(text);
        CoTaskMemFree(text);
        return S_OK;
    }
};

// ==== the WebViews our script is in ====

// One per live Outlook window, so disabling the mod can take the script out again
// instead of leaving it running until Outlook restarts. `controller` is kept as an
// identity only, never called, so it holds no reference and no window alive.
struct PageView {
    IUnknown* webview;
    void* controller;
    HWND window;
    std::wstring scriptId;
    EventToken token;
};

SRWLOCK g_viewsLock = SRWLOCK_INIT;
std::vector<PageView> g_views;

void AddView(IUnknown* webview, void* controller, HWND window, PCWSTR scriptId) {
    MessageReceived* handler = new MessageReceived();
    EventToken token{};
    using Add_t = HRESULT(STDMETHODCALLTYPE*)(IUnknown*, IUnknown*, EventToken*);
    HRESULT hr = reinterpret_cast<Add_t>(Slot(webview, kWebView_AddWebMessageReceived))(webview, handler, &token);
    handler->Release();
    if (FAILED(hr)) Wh_Log(L"add_WebMessageReceived failed: 0x%08X", hr);
    webview->AddRef();
    AcquireSRWLockExclusive(&g_viewsLock);
    g_views.push_back({webview, controller, window, scriptId ? scriptId : L"", token});
    ReleaseSRWLockExclusive(&g_viewsLock);
}

// Lets go of one WebView on its own thread: the script comes off, what is already on
// the page is undone, and the message handler goes with it.
void ReleaseView(const PageView& view, bool removeScript) {
    using RemoveScript_t = HRESULT(STDMETHODCALLTYPE*)(IUnknown*, LPCWSTR);
    using Execute_t = HRESULT(STDMETHODCALLTYPE*)(IUnknown*, LPCWSTR, IUnknown*);
    using RemoveMessage_t = HRESULT(STDMETHODCALLTYPE*)(IUnknown*, EventToken);
    if (removeScript) {
        if (!view.scriptId.empty()) {
            reinterpret_cast<RemoveScript_t>(Slot(view.webview, kWebView_RemoveScriptOnDocumentCreated))(
                view.webview, view.scriptId.c_str());
        }
        reinterpret_cast<Execute_t>(Slot(view.webview, kWebView_ExecuteScript))(
            view.webview, L"window.__olkt && window.__olkt.remove(true)", nullptr);
    }
    if (view.token.value) {
        reinterpret_cast<RemoveMessage_t>(Slot(view.webview, kWebView_RemoveWebMessageReceived))(view.webview,
                                                                                                 view.token);
    }
    view.webview->Release();
}

// Completion handler for AddScriptToExecuteOnDocumentCreated. Holds back Outlook's
// controller handler until the script is registered, because Outlook navigates from
// that handler and the window's first document must already get the script.
class ScriptAdded final : public IUnknown {
    std::atomic<ULONG> refs_{1};
    std::atomic<bool> forwarded_{false};
    IUnknown* outlook_;
    IUnknown* controller_;
    IUnknown* webview_;
    void* controllerKey_;
    HWND window_;

   public:
    ScriptAdded(IUnknown* outlook, IUnknown* controller, IUnknown* webview, void* controllerKey, HWND window)
        : outlook_(outlook), controller_(controller), webview_(webview), controllerKey_(controllerKey), window_(window) {
        outlook_->AddRef();
        controller_->AddRef();
        webview_->AddRef();
        ++g_liveObjects;
    }
    // Hands Outlook its controller, once only.
    void ForwardOnce() {
        if (!forwarded_.exchange(true)) Forward(outlook_, S_OK, controller_);
    }
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid, void** out) override {
        if (IsEqualGUID(iid, kIID_IUnknown) || IsEqualGUID(iid, kIID_ScriptAdded)) {
            *out = this;
            AddRef();
            return S_OK;
        }
        *out = nullptr;
        return E_NOINTERFACE;
    }
    ULONG STDMETHODCALLTYPE AddRef() override { return ++refs_; }
    ULONG STDMETHODCALLTYPE Release() override {
        ULONG n = --refs_;
        if (!n) {
            ForwardOnce();  // dropped without a completion: Outlook still gets its window
            outlook_->Release();
            controller_->Release();
            webview_->Release();
            delete this;
            --g_liveObjects;
        }
        return n;
    }
    virtual HRESULT STDMETHODCALLTYPE Invoke(HRESULT hr, LPCWSTR id) {
        if (FAILED(hr)) {
            Wh_Log(L"Page script not added: hr=0x%08X", hr);
        } else {
            AddView(webview_, controllerKey_, window_, id);
        }
        ForwardOnce();
        return S_OK;
    }
};

// Stands in for Outlook's completion handler. `handle` gets the result and Outlook's
// handler, and is responsible for passing the result on to it.
using Handle_t = HRESULT (*)(HRESULT hr, IUnknown* result, IUnknown* outlook);

class WrappedHandler final : public IUnknown {
    std::atomic<ULONG> refs_{1};
    GUID iid_;
    IUnknown* inner_;
    Handle_t handle_;

   public:
    WrappedHandler(REFIID iid, IUnknown* inner, Handle_t handle) : iid_(iid), inner_(inner), handle_(handle) {
        inner_->AddRef();
        ++g_liveObjects;
    }
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid, void** out) override {
        if (IsEqualGUID(iid, kIID_IUnknown) || IsEqualGUID(iid, iid_)) {
            *out = this;
            AddRef();
            return S_OK;
        }
        *out = nullptr;
        return E_NOINTERFACE;
    }
    ULONG STDMETHODCALLTYPE AddRef() override { return ++refs_; }
    ULONG STDMETHODCALLTYPE Release() override {
        ULONG n = --refs_;
        if (!n) {
            inner_->Release();
            delete this;
            --g_liveObjects;
        }
        return n;
    }
    virtual HRESULT STDMETHODCALLTYPE Invoke(HRESULT hr, IUnknown* result) { return handle_(hr, result, inner_); }
};

// Set while one of our controller hooks runs, so an implementation that calls
// another hooked method internally is wrapped only once.
thread_local int g_inCreate = 0;

using ControllerClose_t = HRESULT(STDMETHODCALLTYPE*)(IUnknown*);
ControllerClose_t ControllerClose_Original;

// Outlook closes a window's controller when the window goes. Dropping our reference
// here keeps the mod from holding dead WebViews alive for the rest of the session.
HRESULT STDMETHODCALLTYPE ControllerClose_Hook(IUnknown* controller) {
    PageView view{};
    bool found = false;
    AcquireSRWLockExclusive(&g_viewsLock);
    for (auto it = g_views.begin(); it != g_views.end(); ++it) {
        if (it->controller == controller) {
            view = *it;
            found = true;
            g_views.erase(it);
            break;
        }
    }
    ReleaseSRWLockExclusive(&g_viewsLock);
    // Close is called on the WebView's own thread, so this is the right place for it.
    if (found) ReleaseView(view, false);
    return ControllerClose_Original(controller);
}

std::atomic<bool> g_controllerHooked{false};

// Controllers share one vtable, so this hook covers every window Outlook opens.
void HookControllerClose(IUnknown* controller) {
    if (g_controllerHooked.exchange(true)) return;
    Wh_SetFunctionHook(Slot(controller, kController_Close), (void*)ControllerClose_Hook,
                       (void**)&ControllerClose_Original);
    Wh_ApplyHookOperations();
    Wh_Log(L"Controller Close hooked");
}

// Adds the page script to each new WebView, then hands Outlook its controller once
// the script is registered (ScriptAdded). Any failure hands it over at once.
HRESULT OnController(HRESULT result_hr, IUnknown* result, IUnknown* outlook) {
    if (FAILED(result_hr) || !result) return Forward(outlook, result_hr, result);
    IUnknown* controller = nullptr;
    if (FAILED(result->QueryInterface(kIID_Controller, (void**)&controller))) {
        Wh_Log(L"Controller interface missing");
        return Forward(outlook, result_hr, result);
    }
    IUnknown* webview = nullptr;
    using Get_t = HRESULT(STDMETHODCALLTYPE*)(IUnknown*, IUnknown**);
    HRESULT hr = reinterpret_cast<Get_t>(Slot(controller, kController_GetCoreWebView2))(controller, &webview);
    // The window the WebView lives in: taking the script out again later has to happen
    // on that window's own thread.
    HWND window = nullptr;
    using GetWindow_t = HRESULT(STDMETHODCALLTYPE*)(IUnknown*, HWND*);
    reinterpret_cast<GetWindow_t>(Slot(controller, kController_GetParentWindow))(controller, &window);
    HookControllerClose(controller);
    void* controllerKey = controller;
    controller->Release();
    if (FAILED(hr) || !webview) {
        Wh_Log(L"get_CoreWebView2 failed: 0x%08X", hr);
        return Forward(outlook, result_hr, result);
    }
    AcquireSRWLockShared(&g_scriptLock);
    std::wstring script = g_script;
    ReleaseSRWLockShared(&g_scriptLock);
    auto* done = new ScriptAdded(outlook, result, webview, controllerKey, window);
    using Add_t = HRESULT(STDMETHODCALLTYPE*)(IUnknown*, LPCWSTR, IUnknown*);
    hr = reinterpret_cast<Add_t>(Slot(webview, kWebView_AddScriptOnDocumentCreated))(webview, script.c_str(), done);
    webview->Release();
    Wh_Log(L"New WebView: AddScriptToExecuteOnDocumentCreated hr=0x%08X", hr);
    if (FAILED(hr)) done->ForwardOnce();  // no completion will come
    done->Release();
    return S_OK;
}

using CreateController_t = HRESULT(STDMETHODCALLTYPE*)(IUnknown*, HWND, IUnknown*);
using CreateControllerOpt_t = HRESULT(STDMETHODCALLTYPE*)(IUnknown*, HWND, IUnknown*, IUnknown*);
CreateController_t CreateController_Original;
CreateController_t CreateComposition_Original;
CreateControllerOpt_t CreateControllerOpt_Original;
CreateControllerOpt_t CreateCompositionOpt_Original;

HRESULT CallWrapped(REFIID iid, IUnknown* handler, auto call) {
    ++g_inCreate;
    IUnknown* h = handler;
    if (h && g_inCreate == 1) {
        h = new WrappedHandler(iid, handler, OnController);
    } else if (h) {
        h->AddRef();
    }
    HRESULT hr = call(h);
    if (h) h->Release();
    --g_inCreate;
    return hr;
}

HRESULT STDMETHODCALLTYPE CreateController_Hook(IUnknown* env, HWND parent, IUnknown* handler) {
    return CallWrapped(kIID_ControllerCompleted, handler,
                       [&](IUnknown* h) { return CreateController_Original(env, parent, h); });
}

HRESULT STDMETHODCALLTYPE CreateComposition_Hook(IUnknown* env, HWND parent, IUnknown* handler) {
    return CallWrapped(kIID_CompositionCompleted, handler,
                       [&](IUnknown* h) { return CreateComposition_Original(env, parent, h); });
}

HRESULT STDMETHODCALLTYPE CreateControllerOpt_Hook(IUnknown* env, HWND parent, IUnknown* opts, IUnknown* handler) {
    return CallWrapped(kIID_ControllerCompleted, handler,
                       [&](IUnknown* h) { return CreateControllerOpt_Original(env, parent, opts, h); });
}

HRESULT STDMETHODCALLTYPE CreateCompositionOpt_Hook(IUnknown* env, HWND parent, IUnknown* opts, IUnknown* handler) {
    return CallWrapped(kIID_CompositionCompleted, handler,
                       [&](IUnknown* h) { return CreateCompositionOpt_Original(env, parent, opts, h); });
}

std::atomic<bool> g_envMethodsHooked{false};

// The environment's methods live in the WebView2 runtime and are shared by every
// environment, so hooking them once covers every window Outlook opens.
void HookEnvironment(IUnknown* env) {
    if (g_envMethodsHooked.exchange(true)) return;
    Wh_SetFunctionHook(Slot(env, kEnv_CreateController), (void*)CreateController_Hook, (void**)&CreateController_Original);
    IUnknown* env3 = nullptr;
    if (SUCCEEDED(env->QueryInterface(kIID_Env3, (void**)&env3))) {
        Wh_SetFunctionHook(Slot(env3, kEnv3_CreateCompositionController), (void*)CreateComposition_Hook,
                           (void**)&CreateComposition_Original);
        env3->Release();
    }
    IUnknown* env10 = nullptr;
    if (SUCCEEDED(env->QueryInterface(kIID_Env10, (void**)&env10))) {
        Wh_SetFunctionHook(Slot(env10, kEnv10_CreateControllerWithOptions), (void*)CreateControllerOpt_Hook,
                           (void**)&CreateControllerOpt_Original);
        Wh_SetFunctionHook(Slot(env10, kEnv10_CreateCompositionControllerWithOptions), (void*)CreateCompositionOpt_Hook,
                           (void**)&CreateCompositionOpt_Original);
        env10->Release();
    }
    Wh_ApplyHookOperations();
    Wh_Log(L"Environment methods hooked");
}

HRESULT OnEnvironment(HRESULT hr, IUnknown* env, IUnknown* outlook) {
    if (SUCCEEDED(hr) && env) HookEnvironment(env);
    return Forward(outlook, hr, env);
}

using CreateEnv_t = HRESULT(WINAPI*)(PCWSTR, PCWSTR, IUnknown*, IUnknown*);
CreateEnv_t CreateEnv_Original;

HRESULT WINAPI CreateEnv_Hook(PCWSTR browserFolder, PCWSTR userDataFolder, IUnknown* options, IUnknown* handler) {
    if (!handler) return CreateEnv_Original(browserFolder, userDataFolder, options, handler);
    auto* wrapped = new WrappedHandler(kIID_EnvCompleted, handler, OnEnvironment);
    HRESULT hr = CreateEnv_Original(browserFolder, userDataFolder, options, wrapped);
    wrapped->Release();
    return hr;
}

std::atomic<bool> g_loaderHooked{false};

bool HookLoader(bool apply) {
    HMODULE loader = GetModuleHandleW(L"WebView2Loader.dll");
    if (!loader || g_loaderHooked.exchange(true)) return false;
    void* target = (void*)GetProcAddress(loader, "CreateCoreWebView2EnvironmentWithOptions");
    if (!target) {
        Wh_Log(L"CreateCoreWebView2EnvironmentWithOptions not exported");
        return false;
    }
    Wh_SetFunctionHook(target, (void*)CreateEnv_Hook, (void**)&CreateEnv_Original);
    if (apply) Wh_ApplyHookOperations();
    Wh_Log(L"WebView2Loader hooked");
    return true;
}

using LoadLibraryExW_t = HMODULE(WINAPI*)(LPCWSTR, HANDLE, DWORD);
LoadLibraryExW_t LoadLibraryExW_Original;

// WebView2Loader.dll can arrive as a dependency of another DLL, so check after every load.
HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR name, HANDLE file, DWORD flags) {
    HMODULE module = LoadLibraryExW_Original(name, file, flags);
    if (module && !g_loaderHooked) HookLoader(true);
    return module;
}

// ==== unload ====

// WebView2 objects answer only on the thread that made them, so the teardown is sent
// to each window's own thread, the way Windhawk's taskbar mods do it.
using RunFromWindowThreadProc_t = void(WINAPI*)(PVOID parameter);

struct RunFromWindowThreadParam {
    RunFromWindowThreadProc_t proc;
    PVOID param;
};

UINT g_runFromWindowThreadMessage;

LRESULT CALLBACK CallWndProcForRunFromWindowThread(int code, WPARAM wParam, LPARAM lParam) {
    if (code == HC_ACTION) {
        const CWPSTRUCT* cwp = (const CWPSTRUCT*)lParam;
        if (cwp->message == g_runFromWindowThreadMessage) {
            auto* param = (RunFromWindowThreadParam*)cwp->lParam;
            param->proc(param->param);
        }
    }
    return CallNextHookEx(nullptr, code, wParam, lParam);
}

bool RunFromWindowThread(HWND window, RunFromWindowThreadProc_t proc, PVOID procParam) {
    DWORD thread = GetWindowThreadProcessId(window, nullptr);
    if (!thread) return false;
    if (thread == GetCurrentThreadId()) {
        proc(procParam);
        return true;
    }
    HHOOK hook = SetWindowsHookEx(WH_CALLWNDPROC, CallWndProcForRunFromWindowThread, nullptr, thread);
    if (!hook) return false;
    RunFromWindowThreadParam param{proc, procParam};
    DWORD_PTR unused = 0;
    // A hung Outlook window must not hold the unload up for ever.
    LRESULT sent = SendMessageTimeout(window, g_runFromWindowThreadMessage, 0, (LPARAM)&param, SMTO_ABORTIFHUNG, 5000,
                                      &unused);
    UnhookWindowsHookEx(hook);
    return sent != 0;
}

void WINAPI ReleaseViewOnThread(PVOID param) {
    ReleaseView(*(const PageView*)param, true);
}

// Runs while the hooks are still in place, so every open Outlook window gives the
// script back before the DLL goes: the styling, the timers and the wrapped fetch all
// stop at once, with no restart needed.
void Wh_ModBeforeUninit() {
    AcquireSRWLockExclusive(&g_viewsLock);
    std::vector<PageView> views;
    views.swap(g_views);
    ReleaseSRWLockExclusive(&g_viewsLock);
    for (const PageView& view : views) {
        if (!RunFromWindowThread(view.window, ReleaseViewOnThread, (PVOID)&view)) {
            // Its window is gone or not answering: leave the object alone rather than
            // call into WebView2 from the wrong thread.
            Wh_Log(L"Could not reach the WebView's thread; its script stays until Outlook restarts");
        }
    }
}

BOOL Wh_ModInit() {
    g_runFromWindowThreadMessage = RegisterWindowMessage(L"Windhawk_RunFromWindowThread_outlook-refined");
    LoadStore();
    BuildScript();
    if (HookLoader(false)) return TRUE;
    HMODULE kernelBase = GetModuleHandleW(L"kernelbase.dll");
    void* target = kernelBase ? (void*)GetProcAddress(kernelBase, "LoadLibraryExW") : nullptr;
    if (!target) return FALSE;
    Wh_SetFunctionHook(target, (void*)LoadLibraryExW_Hook, (void**)&LoadLibraryExW_Original);
    return TRUE;
}

void Wh_ModSettingsChanged() {
    BuildScript();
}

// Runs after Windhawk has removed the hooks, so no new handlers appear. Waits up to
// 5 seconds for WebView2 to release the ones it still holds before the DLL unloads.
void Wh_ModUninit() {
    for (int i = 0; i < 100 && g_liveObjects > 0; ++i) Sleep(50);
    if (g_liveObjects > 0) Wh_Log(L"Unloading while WebView2 still holds %d handlers", g_liveObjects.load());
}
