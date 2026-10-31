// ==WindhawkMod==
// @id              windhawk-ui-translator
// @name            Windhawk UI Translator
// @name:zh-CN      Windhawk 界面翻译
// @description     Translates Windhawk mod content — names, summaries, READMEs, setting labels and changelogs — in place, sending page text to Microsoft's translation service by default
// @description:zh-CN 就地翻译 Windhawk 的模组内容（名称、简介、说明文档、设置项与更新日志），默认会把页面文本发送到微软翻译服务
// @version         0.1
// @author          Zhanyuanium
// @github          https://github.com/Zhanyuanium
// @include         windhawk-ui.exe
// @architecture    x86-64
// @compilerOptions -lole32 -luuid -ladvapi32 -lwinhttp -fms-extensions
// @license         GPL-3.0
// ==/WindhawkMod==

// ==WindhawkModSettings==
/*
- targetLang: auto
  $name: Target language
  $name:zh-CN: 目标语言
  $description: Language the mod content is translated into. "auto" follows the Windhawk UI language.
  $description:zh-CN: 把模组内容翻译成哪种语言。选“自动”时跟随 Windhawk 界面语言。
  $options:
  - auto: Automatic
  - zh-CN: Chinese (Simplified)
  - zh-TW: Chinese (Traditional)
  - ja: Japanese
  - ko: Korean
  - en: English
  - de: German
  - fr: French
  - ru: Russian
  - es: Spanish
  - pt-BR: Portuguese (Brazil)
  $options:zh-CN:
  - auto: 自动
  - zh-CN: 简体中文
  - zh-TW: 繁体中文
  - ja: 日语
  - ko: 韩语
  - en: 英语
  - de: 德语
  - fr: 法语
  - ru: 俄语
  - es: 西班牙语
  - pt-BR: 葡萄牙语（巴西）
- provider: auto
  $name: Translation provider
  $name:zh-CN: 翻译服务
  $description: Which translation backend the page uses. "auto" lets the page decide between the LLM and the free endpoints.
  $description:zh-CN: 页面使用哪种翻译后端。选“自动”时由页面在大模型与免费接口之间选择。
  $options:
  - auto: Automatic
  - edge: Microsoft Edge (free, no key)
  - llm: OpenAI-compatible LLM
  $options:zh-CN:
  - auto: 自动
  - edge: 微软 Edge（免费，无需密钥）
  - llm: 兼容 OpenAI 的大模型
- llmBaseUrl: https://api.openai.com/v1
  $name: LLM base URL
  $name:zh-CN: 大模型 API 地址
  $description: OpenAI-compatible API base URL, for example https://api.openai.com/v1. It must use https://; plain http:// endpoints, localhost included, are rejected.
  $description:zh-CN: 兼容 OpenAI 的接口根地址，例如 https://api.openai.com/v1。必须使用 https://，纯 http:// 地址（包括 localhost）会被拒绝。
- llmModel: ""
  $name: LLM model
  $name:zh-CN: 模型名称
  $description: Model name sent to the LLM endpoint. Leave empty to leave the field out of the request, so the server uses its default model.
  $description:zh-CN: 发送给大模型接口的模型名称。留空则请求中不包含该字段，由服务端使用默认模型。
- credentialTarget: windhawk-ui-translator
  $name: Credential target
  $name:zh-CN: 凭据名称
  $description: Windows Credential Manager target that holds the API key as a generic credential.
  $description:zh-CN: 在 Windows 凭据管理器中保存 API 密钥的凭据名称（普通凭据）。
- llmApiKey: ""
  $name: API key (plaintext fallback)
  $name:zh-CN: API 密钥（明文备用）
  $description: Used when the key source is "setting", and as the fallback when the credential is missing. It is stored as plain text in the registry, so prefer the credential source.
  $description:zh-CN: 在密钥来源为“设置项”时使用，凭据缺失时也会作为备用。密钥会以明文写入注册表，建议优先使用凭据管理器。
- llmKeySource: credential
  $name: API key source
  $name:zh-CN: 密钥来源
  $description: Where the LLM API key comes from. "credential" reads Windows Credential Manager, "setting" uses the field above, "none" sends requests without a key.
  $description:zh-CN: 大模型 API 密钥的来源。“凭据”从 Windows 凭据管理器读取，“设置项”使用上面的字段，“无”表示请求不带密钥。
  $options:
  - credential: Windows Credential Manager
  - setting: The API key setting above
  - none: No key
  $options:zh-CN:
  - credential: Windows 凭据管理器
  - setting: 上面的 API 密钥设置项
  - none: 不使用密钥
- glossary:
    - ""
  $name: Glossary
  $name:zh-CN: 术语表
  $description: "One entry per line, written as source=target. Sent with every LLM request and takes priority over the model's own wording. Ignored by the free providers."
  $description:zh-CN: 每行一条，写成“原文=译文”。随每个大模型请求一起发送，优先级高于模型自己的措辞。免费接口不使用本项。
- scopeReadme: true
  $name: Translate mod descriptions
  $name:zh-CN: 翻译模组说明
  $description: Translate README content on the mod details page.
  $description:zh-CN: 翻译模组详情页中的说明文档。
- scopeSettings: true
  $name: Translate mod settings
  $name:zh-CN: 翻译设置项
  $description: Translate setting names, descriptions and option labels.
  $description:zh-CN: 翻译设置项名称、说明与选项文字。
- scopeModName: true
  $name: Translate mod names
  $name:zh-CN: 翻译模组名称
  $description: "Translate mod names everywhere they appear: the tile grid, the table view and the details header."
  $description:zh-CN: 翻译所有出现模组名称的位置：磁贴视图、列表视图和详情页标题。
- scopeModSummary: true
  $name: Translate mod summaries
  $name:zh-CN: 翻译模组简介
  $description: "Translate the one-line mod summary everywhere it appears: the tile grid, the table view and the details header."
  $description:zh-CN: 翻译所有出现模组简介的位置：磁贴视图、列表视图和详情页标题。
- scopeChangelog: true
  $name: Translate changelogs
  $name:zh-CN: 翻译更新日志
  $description: Translate changelog sections on the mod details page.
  $description:zh-CN: 翻译模组详情页中的更新日志。
- hoverDelayMs: 120
  $name: Hover delay (milliseconds)
  $name:zh-CN: 悬停延迟（毫秒）
  $description: How long the pointer must rest on a translated block before the original text appears. 0 to 2000.
  $description:zh-CN: 鼠标在已翻译的文本块上停留多久后显示原文。可填 0 到 2000。
- shortcutToggle: "Alt+Q"
  $name: Toggle translation shortcut
  $name:zh-CN: 翻译开关快捷键
  $description: Keyboard shortcut that turns translation on or off. Leave empty to disable it.
  $description:zh-CN: 开、关翻译的快捷键。留空表示不启用。
- gestureToggle: true
  $name: Two-finger double-tap toggles translation
  $name:zh-CN: 双指双击开关翻译
  $description: On a touch screen, tapping twice with two fingers turns translation off or on, like the toggle shortcut.
  $description:zh-CN: 在触屏上双指连点两次即可关闭或重新开启翻译，效果与翻译开关快捷键相同。
- batchSize: 20
  $name: Segments per batch
  $name:zh-CN: 每批文本段数
  $description: Maximum number of text segments sent in one translation request.
  $description:zh-CN: 单次翻译请求最多包含的文本段数。
- batchChars: 10000
  $name: Characters per batch
  $name:zh-CN: 每批字符数
  $description: Maximum number of characters sent in one translation request.
  $description:zh-CN: 单次翻译请求最多包含的字符数。
- batchIntervalMs: 400
  $name: Batch window (milliseconds)
  $name:zh-CN: 批处理间隔（毫秒）
  $description: How long newly seen text is collected before a request is sent.
  $description:zh-CN: 新出现的文本先收集多久再发送请求。
- concurrency: 4
  $name: Parallel requests
  $name:zh-CN: 并发请求数
  $description: Maximum number of translation requests in flight at the same time.
  $description:zh-CN: 同时进行的翻译请求数量上限。
- timeoutMs: 20000
  $name: Request timeout (milliseconds)
  $name:zh-CN: 请求超时（毫秒）
  $description: Timeout for each HTTP request made through the mod.
  $description:zh-CN: 每个经模组发出的 HTTP 请求的超时时间。
- cacheEnabled: true
  $name: Cache translations
  $name:zh-CN: 缓存翻译结果
  $description: Reuse translations that were already requested.
  $description:zh-CN: 复用已经请求过的翻译结果。
- cacheTtlDays: 30
  $name: Cache lifetime (days)
  $name:zh-CN: 缓存有效天数
  $description: How long a cached translation stays valid.
  $description:zh-CN: 缓存翻译结果的有效天数。
- clearCache: false
  $name: Clear the cache when switched on
  $name:zh-CN: 开启本项时清除缓存
  $description: Clears the translation cache on every off-to-on change of this setting, including while the UI is running. Switch it off again to arm the next clear.
  $description:zh-CN: 本项每次从关闭变为开启时都会清空翻译缓存，界面运行期间同样生效。清空后请再关闭本项，以便下次重新触发。
*/
// ==/WindhawkModSettings==

// ==WindhawkModReadme==
/*
# Windhawk UI Translator

Windhawk localizes its own interface, but not what mod authors write: names and
summaries only have the narrow `@name:zh-CN` / `$description:zh-CN` /
`$options:zh-CN` metadata channel, and READMEs and changelogs have no
localization at all.

This mod translates that content in place, inside the Windhawk 2.0 window.

![](https://raw.githubusercontent.com/Zhanyuanium/windhawk-ui-translator/main/docs/images/details.png)

## What it translates

| Where | What |
| --- | --- |
| Details page | mod name, summary, README, changelog |
| Settings page | titles, descriptions, tooltips, dropdown values and options, reset labels |
| Explore and installed lists | mod names and one-line summaries |

Each group has its own switch in the mod's settings. The Source tab and
Windhawk's own interface strings are left alone.

## How it reads

The translation replaces the original text in place. Rest the pointer on a
translated block to see the original; on a touch screen, tap once for the
original and again for the translation. `Alt+Q` (configurable) or a two-finger
double-tap toggles translation in both directions.

## Providers

Any OpenAI-compatible `/chat/completions` endpoint is recommended; a glossary
and batching are built in. The API key is read from Windows Credential Manager
(a plaintext setting is the fallback) and never reaches the page:

    cmdkey /generic:windhawk-ui-translator /user:translator /pass:<your key>

The free Microsoft Edge endpoint needs no key and is the fallback; `auto` uses
the LLM as soon as a base URL and a key are configured, and needs no model
name. Google Translate is not offered.

## Notes

* If the mod is enabled while the UI is already running, it does nothing until
  the UI process is restarted. Close and reopen Windhawk (`windhawk-ui.exe`);
  restarting only the tray icon is not enough.
* If the UI fails to start: `windhawk.exe -safe-mode`, or
  `windhawk-cli.exe mod disable windhawk-ui-translator`.

## Privacy

Page text is sent to Microsoft's translation service by default; with an LLM
configured it goes to that endpoint instead. No telemetry, no other network
access. The key is stored by Windows, not by this mod.
*/
// ==/WindhawkModReadme==

// Injects the page-side translator into Windhawk's WebView2 UI.
//
// windhawk-ui.exe is a Tauri app which hosts WebView2. Tauri/wry links the
// static WebView2 loader, so CreateCoreWebView2EnvironmentWithOptions is not an
// import of windhawk-ui.exe: the loader resolves
// CreateWebViewEnvironmentWithOptionsInternal from EmbeddedBrowserWebView.dll
// via GetProcAddress, which is what this mod intercepts.
//
// Everything here is best-effort and fail-soft: every unexpected condition is
// logged and then ignored. The mod must never take down Windhawk's own UI.
//
// Logging goes to Wh_Log only (Windhawk's log pane). To watch it outside the
// app, run scripts/dbwin-log.ps1, which captures OutputDebugString from the
// windhawk-ui.exe process.
//
// Vtable slots are 0-based COM indices taken from the official WebView2.h
// (1.0.3719.77 and 1.0.992.28 declare the interfaces in the same order, so the
// navigation slots 7/8/15/16 line up with AddScript = 27,
// RemoveScriptToExecuteOnDocumentCreated = 28 and ExecuteScript = 29). The
// named constants below are the reference; every slot is still range-validated
// against EmbeddedBrowserWebView.dll before it is called.

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <objbase.h>
#include <wincred.h>
#include <winhttp.h>

#include <algorithm>
#include <atomic>
#include <cstdlib>
#include <cstring>
#include <cwchar>
#include <deque>
#include <mutex>
#include <new>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include <windhawk_api.h>

// ---------------------------------------------------------------------------
// Logging helpers
//
// Every line goes to Wh_Log (Windhawk's own log pane). A string that comes
// from the app or from the page is bounded in the format string itself
// (L"%.200s"): the MSVC/llvm-mingw CRT applies the precision of a wide %s as a
// character count, and a bounded conversion needs no intermediate storage.
//
// There is deliberately no shared scratch buffer. A static ring would hand out
// a pointer that another thread can overwrite while the caller is still
// formatting or logging from it, and module thread_local storage is unusable:
// Windhawk's loader does not register a mod's TLS directory, so the first
// module TLS access reads TEB->ThreadLocalStoragePointer (gs:0x58) before it is
// populated and faults.
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Slot numbers
// ---------------------------------------------------------------------------

static constexpr size_t kEnvSlotCreateController = 3;
static constexpr size_t kEnvSlotCreateControllerWithOptions = 21;
static constexpr size_t kSlotAddRef = 1;
static constexpr size_t kSlotRelease = 2;

static constexpr size_t kControllerSlotGetCoreWebView2 = 25;

static constexpr size_t kWebViewSlotGetSettings = 3;
static constexpr size_t kWebViewSlotAddNavigationStarting = 7;
static constexpr size_t kWebViewSlotRemoveNavigationStarting = 8;
static constexpr size_t kWebViewSlotAddNavigationCompleted = 15;
static constexpr size_t kWebViewSlotRemoveNavigationCompleted = 16;
static constexpr size_t kWebViewSlotAddScript = 27;
static constexpr size_t kWebViewSlotRemoveScript = 28;
static constexpr size_t kWebViewSlotExecuteScript = 29;
static constexpr size_t kWebViewSlotPostWebMessageAsString = 33;
static constexpr size_t kWebViewSlotAddWebMessageReceived = 34;
static constexpr size_t kWebViewSlotRemoveWebMessageReceived = 35;

static constexpr size_t kSettingsSlotGetIsWebMessageEnabled = 5;
static constexpr size_t kSettingsSlotPutIsWebMessageEnabled = 6;

static constexpr size_t kArgsSlotTryGetWebMessageAsString = 5;
static constexpr size_t kNavigationStartingArgsSlotGetUri = 3;
static constexpr size_t kNavigationCompletedArgsSlotGetIsSuccess = 3;
static constexpr size_t kNavigationCompletedArgsSlotGetWebErrorStatus = 4;
static constexpr size_t kNavigationCompletedArgsSlotGetNavigationId = 5;

// The callback object always has Invoke at vtable index 3.
static constexpr size_t kCallbackSlotInvoke = 3;

// ICoreWebView2Environment12+ has well under 64 methods. Copying a few extra
// entries is harmless (the same words would be read from the original vtable
// area anyway) and keeps future callers from running off the copy.
static constexpr size_t kEnvVtblSlots = 64;

// ---------------------------------------------------------------------------
// P0 JavaScript
//
// The same text runs twice: at document-created time (AddScriptToExecute...) and
// again on the current document from ExecuteScript. At document-created time
// window.chrome.webview does not exist yet; WebView2 installs it later, after
// that page script has run. Creating the "already initialised" marker at that
// point would therefore disable every later injection without ever having sent
// anything. So the marker is only created once the channel exists; until then
// a bounded setTimeout retry waits for it. `installed` (message listener
// registered) and `sent` (hello posted) are independent flags, and the page
// stays untouched when the channel never appears.
//
// The IIFE's return value is a JSON probe. ExecuteScript returns the value of
// the script's last expression, so the mod's "... result=%s" log line shows
// the state of the document the script actually reached.
// ---------------------------------------------------------------------------

static constexpr wchar_t kP0Js[] = LR"WHTR_JS(
/*!
 * windhawk-ui-translator - page-side engine (P2)
 *
 * Runs inside the Windhawk 2.0 UI, injected at document-created time and again
 * on navigation completion. Self-contained ES2020 IIFE: no imports, no eval,
 * no network access; every byte leaves the page through the mod's `whtr1`
 * channel (see docs/PROTOCOL.md). Translation renders in place; pointer over a
 * block shows the original, and on touch a tap toggles that block.
 */
(() => {
  'use strict';

  const VERSION = '1.0.0-p2';
  const TAG = 'whtr1';
  const SEP = '\x1f';

  // Attributes we own: the hidden React node, our clone and a text-node swap.
  const ATTR_ORIG = 'data-wh-orig';
  const ATTR_CLONE = 'data-wh-clone';
  const ATTR_SWAPPED = 'data-wh-swapped';
  const ATTR_UNIT = 'data-wh-unit';

  // ---------------------------------------------------------------------------
  // Tiny helpers
  // ---------------------------------------------------------------------------

  const safe = (fn, where) => {
    try {
      return fn();
    } catch (e) {
      try {
        problem(where + ': ' + (e && e.message ? e.message : e));
      } catch (_) {
        /* nothing else we can do */
      }
      return undefined;
    }
  };

  const clamp = (n, lo, hi) => {
    n = Number(n);
    if (!isFinite(n)) return lo;
    return Math.min(hi, Math.max(lo, n));
  };

  const sleep = (ms) => new Promise((r) => setTimeout(r, ms));

  const clean = (s) => String(s == null ? '' : s).replace(/\x1f/g, ' ');

  // Cache keys, provider requests and in-flight keys all use this normalised
  // form; the DOM keeps its own whitespace for restoring the original.
  const normalizeSource = (s) => String(s == null ? '' : s).replace(/\s+/g, ' ').trim();

  // Protocol fields: every field but the last is separator-free, so split the
  // leading fields with indexOf and leave the remainder (which may contain
  // separators) as the last field.
  const splitFields = (s, count) => {
    const out = [];
    let at = 0;
    for (let i = 0; i < count - 1; i++) {
      const next = s.indexOf(SEP, at);
      if (next === -1) {
        if (at < s.length || out.length) out.push(s.slice(at));
        return out;
      }
      out.push(s.slice(at, next));
      at = next + 1;
    }
    out.push(s.slice(at));
    return out;
  };

  const hash32 = (s) => {
    // FNV-1a, 32 bit; used for in-session source identity.
    let h = 0x811c9dc5;
    for (let i = 0; i < s.length; i++) {
      h ^= s.charCodeAt(i);
      h = Math.imul(h, 0x01000193);
    }
    h ^= s.length;
    return (h >>> 0).toString(16).padStart(8, '0');
  };

  const hash32b = (s) => {
    // djb2-xor, 32 bit; independent of hash32, so the pair is 64 bits.
    let h = 5381;
    for (let i = 0; i < s.length; i++) {
      h = Math.imul(h, 33) ^ s.charCodeAt(i);
    }
    h ^= s.length;
    return (h >>> 0).toString(16).padStart(8, '0');
  };

  const hash64 = (s) => hash32(s) + hash32b(s);

  const idle = (fn) => {
    if (typeof window.requestIdleCallback === 'function') {
      return window.requestIdleCallback(fn, { timeout: 500 });
    }
    return setTimeout(() => fn({ timeRemaining: () => 8, didTimeout: true }), 16);
  };

  const CHANNEL = () => {
    try {
      return (window.chrome && window.chrome.webview) || null;
    } catch (_) {
      return null;
    }
  };

  // ---------------------------------------------------------------------------
  // Settings, state, diagnostics
  // ---------------------------------------------------------------------------

  function defaultSettings() {
    return {
      targetLang: 'auto',
      provider: 'auto',
      llmBaseUrl: '',
      llmModel: '',
      llmKeySource: 'none',
      glossary: [],
      scopeReadme: true,
      scopeSettings: true,
      scopeModName: true,
      scopeModSummary: true,
      scopeChangelog: true,
      hoverDelayMs: 120,
      shortcutToggle: 'Alt+Q',
      gestureToggle: true,
      batchSize: 20,
      batchChars: 10000,
      batchIntervalMs: 400,
      concurrency: 4,
      timeoutMs: 20000,
      cacheEnabled: true,
      cacheTtlDays: 30,
      clearCache: false
    };
  }

  const state = {
    settings: defaultSettings(),
    settingsSeen: false,
    settingsCount: 0,
    connected: false,
    enabled: true,
    // Set by `teardown`: the mod is unloading, so nothing may be sent to it.
    dead: false,
    targetLang: '',
    provider: 'edge',
    // Set once an LLM batch failed: the rest of the session stays on Edge.
    llmFallback: false,
    llmWarned: false,
    googleWarned: false,
    epoch: 0,
    units: new Set(),
    unitByHost: new WeakMap(),
    unitByDisplay: new WeakMap(),
    // Host -> (slot + text -> Text). A protected changelog token is rendered
    // through the ordinary placeholder refs; keeping the text nodes stable
    // means a rescan does not look like a changed unit.
    literalRefs: new WeakMap(),
    written: new WeakMap(), // Text node -> value we wrote last
    origText: new WeakMap(), // Text node -> last known React-authored value
    nextUnitId: 1,
    nextRequestId: 1,
    pending: new Map(),
    // Request id -> { id, key, entry, timer } whose `cache-get` reply is still
    // outstanding. Matching on the id as well as the key keeps a late reply
    // from resolving a newer lookup for the same string. The deadline turns a
    // silent mod into a normal miss.
    cacheLookups: new Map(),
    // Keys with an outstanding lookup; a second string with the same key is
    // translated normally instead of sharing an ambiguous request.
    cacheLookupKeys: new Set(),
    // One warning per session when the cache stops answering.
    cacheWarned: false,
    // Normalised source -> shared translation entry, so a distinct string is
    // requested once per epoch.
    textEntries: new Map(),
    queue: [],
    inflight: 0,
    flushTimer: 0,
    scanTimer: 0,
    settingsTimer: 0,
    observer: null,
    messageListener: null,
    renderQueue: new Set(),
    renderScheduled: false,
    scanQueue: null,
    scanScheduled: false,
    shortcuts: { toggle: null },
    settingsAttempts: 0,
    stats: {
      scans: 0,
      scannedHosts: 0,
      requests: 0,
      errors: 0,
      cacheHits: 0,
      cacheMisses: 0,
      batches: 0,
      batchesFailed: 0,
      translations: 0,
      startedAt: Date.now()
    }
  };

  // The object exposed as window.__WHTR__; teardown deletes it so a later
  // injection in the same document can build a fresh engine.
  let publicApi = null;

  function log(msg) {
    send(TAG + SEP + 'log' + SEP + clean(msg).slice(0, 500));
  }

  function problem(msg) {
    log('whtr: ' + clean(msg));
  }

  function send(text) {
    if (state.dead) return false;
    const wv = CHANNEL();
    if (!wv) return false;
    try {
      wv.postMessage(text);
      return true;
    } catch (e) {
      return false;
    }
  }

  function snapshot() {
    let translated = 0;
    let rendered = 0;
    for (const u of state.units) {
      if (u.translatedOk) translated++;
      if (u.rendered) rendered++;
    }
    return {
      version: VERSION,
      enabled: state.enabled,
      settingsSeen: state.settingsSeen,
      connected: state.connected,
      targetLang: state.targetLang,
      provider: state.provider,
      activeProvider: activeProvider(),
      llmFallback: state.llmFallback,
      units: state.units.size,
      translated: translated,
      rendered: rendered,
      queued: state.queue.length,
      inflight: state.inflight,
      requests: state.stats.requests,
      errors: state.stats.errors,
      scans: state.stats.scans,
      scannedHosts: state.stats.scannedHosts,
      cacheHits: state.stats.cacheHits,
      cacheMisses: state.stats.cacheMisses,
      uptimeMs: Date.now() - state.stats.startedAt
    };
  }

  // ---------------------------------------------------------------------------
  // HTTP through the mod
  // ---------------------------------------------------------------------------

  function httpRequest(opts) {
    return new Promise((resolve) => {
      let settled = false;
      const done = (v) => {
        if (settled) return;
        settled = true;
        resolve(v);
      };
      try {
        if (state.dead) {
          done({ status: 0, body: '', error: -1 });
          return;
        }
        const wv = CHANNEL();
        if (!wv) {
          done({ status: 0, body: '', error: -1 });
          return;
        }
        const method = String((opts && opts.method) || 'GET').toUpperCase();
        const url = String((opts && opts.url) || '');
        if (!url) {
          done({ status: 0, body: '', error: -1 });
          return;
        }
        const body = String(opts && opts.body != null ? opts.body : '');
        const id = state.nextRequestId++;

        const ms = clamp(state.settings.timeoutMs, 1000, 120000) || 20000;
        const timer = setTimeout(() => {
          if (!state.pending.has(id)) return;
          state.pending.delete(id);
          state.stats.errors++;
          done({ status: 0, body: '', error: -2 });
        }, ms);

        state.pending.set(id, { resolve: done, timer: timer });
        state.stats.requests++;
        // Protocol v2: no headers. The mod sets Content-Type and, for the LLM
        // host only, Authorization (docs/PROTOCOL.md).
        wv.postMessage([TAG, 'http', String(id), method, url, body].join(SEP));
      } catch (e) {
        state.stats.errors++;
        done({ status: 0, body: '', error: -3 });
      }
    });
  }

  function onHttpReply(parts) {
    // parts: [tag, 'http', id, status, error, body]
    const id = Number(parts[2]);
    const entry = state.pending.get(id);
    if (!entry) return;
    state.pending.delete(id);
    try {
      clearTimeout(entry.timer);
    } catch (_) {
      /* ignore */
    }
    const status = Number(parts[3]);
    const error = Number(parts[4]);
    const body = parts[5] == null ? '' : parts[5];
    entry.resolve({
      status: isFinite(status) ? status : 0,
      error: isFinite(error) ? error : -1,
      body: body
    });
  }

  // ---------------------------------------------------------------------------
  // Shortcuts
  // ---------------------------------------------------------------------------

  const NAMED_CODES = {
    space: 'Space',
    escape: 'Escape',
    esc: 'Escape',
    enter: 'Enter',
    tab: 'Tab',
    backspace: 'Backspace',
    delete: 'Delete',
    insert: 'Insert',
    home: 'Home',
    end: 'End',
    pageup: 'PageUp',
    pagedown: 'PageDown',
    arrowup: 'ArrowUp',
    arrowdown: 'ArrowDown',
    arrowleft: 'ArrowLeft',
    arrowright: 'ArrowRight',
    '`': 'Backquote',
    '-': 'Minus',
    '=': 'Equal',
    '[': 'BracketLeft',
    ']': 'BracketRight',
    '\\': 'Backslash',
    ';': 'Semicolon',
    "'": 'Quote',
    ',': 'Comma',
    '.': 'Period',
    '/': 'Slash'
  };

  function parseShortcut(text) {
    const s = String(text == null ? '' : text).trim();
    if (!s) return null;
    const want = { ctrl: false, alt: false, shift: false, meta: false };
    const codes = new Set();
    for (const raw of s.split('+')) {
      const token = raw.trim();
      if (!token) continue;
      const low = token.toLowerCase();
      if (low === 'ctrl' || low === 'control') {
        want.ctrl = true;
        continue;
      }
      if (low === 'alt' || low === 'option') {
        want.alt = true;
        continue;
      }
      if (low === 'shift') {
        want.shift = true;
        continue;
      }
      if (low === 'meta' || low === 'win' || low === 'cmd' || low === 'command') {
        want.meta = true;
        continue;
      }
      if (/^f([1-9]|1[0-9]|2[0-4])$/.test(low)) {
        codes.add('F' + low.slice(1));
      } else if (/^[a-z]$/.test(low)) {
        codes.add('Key' + low.toUpperCase());
      } else if (/^[0-9]$/.test(low)) {
        codes.add('Digit' + low);
      } else if (NAMED_CODES[low]) {
        codes.add(NAMED_CODES[low]);
      } else if (low.length > 1) {
        codes.add(token);
      }
    }
    if (!codes.size) return null;
    return { want: want, codes: codes };
  }

  function shortcutMatches(combo, e) {
    if (!combo) return false;
    if (!!e.ctrlKey !== combo.want.ctrl) return false;
    if (!!e.altKey !== combo.want.alt) return false;
    if (!!e.shiftKey !== combo.want.shift) return false;
    if (!!e.metaKey !== combo.want.meta) return false;
    return combo.codes.has(e.code);
  }

  function isTypingTarget(target) {
    if (!target || target.nodeType !== 1) return false;
    try {
      const tag = target.tagName;
      if (tag === 'INPUT' || tag === 'TEXTAREA' || tag === 'SELECT' || tag === 'OPTION') return true;
      if (target.isContentEditable) return true;
      if (target.closest('.cm-editor, .monaco-editor, [contenteditable], [role="textbox"]')) return true;
    } catch (_) {
      /* ignore */
    }
    return false;
  }

  function onKeyDown(e) {
    if (!e || e.repeat) return;
    if (isTypingTarget(e.target)) return;
    try {
      if (isTypingTarget(document.activeElement)) return;
    } catch (_) {
      /* ignore */
    }
    try {
      if (shortcutMatches(state.shortcuts.toggle, e)) {
        e.preventDefault();
        setEnabled(!state.enabled);
      }
    } catch (err) {
      problem('shortcut: ' + (err && err.message ? err.message : err));
    }
  }

  // ---------------------------------------------------------------------------
  // Scanning
  // ---------------------------------------------------------------------------

  const PROSE_SEL = 'h1,h2,h3,h4,h5,h6,p,li,blockquote,td,th';
  // Block-level elements that make a prose host "mixed": it owns text and also
  // wraps other blocks, which are translated as their own units.
  const BLOCK_TAGS = new Set([
    'ADDRESS', 'ARTICLE', 'ASIDE', 'BLOCKQUOTE', 'DD', 'DETAILS', 'DIV', 'DL',
    'DT', 'FIELDSET', 'FIGCAPTION', 'FIGURE', 'FOOTER', 'FORM', 'H1', 'H2',
    'H3', 'H4', 'H5', 'H6', 'HEADER', 'HR', 'LI', 'MAIN', 'NAV', 'OL', 'P',
    'PRE', 'SECTION', 'SUMMARY', 'TABLE', 'TBODY', 'TD', 'TFOOT', 'TH',
    'THEAD', 'TR', 'UL'
  ]);
  // Closed select hosts. antd 6 renders a single select's value as a text node
  // in `.ant-select-content` (which also carries `title` and the search input);
  // `.ant-select-selection-item` remains for antd 5 and multiple/tags mode.
  const SELECT_HOST_SELECTORS = [
    '.ant-select-single .ant-select-content',
    '.ant-select-content-value',
    '.ant-select-placeholder',
    '.ant-select-selection-item',
    '.ant-select-selection-placeholder'
  ];
  // A setting description. Inside the compact title row it belongs to the
  // tooltip, not the title unit; the visible portal copy translates normally.
  const DESC_TEXT_SEL = '[data-testid="mod-setting-description-text"]';
  // The collapsed current-value summary ("Non-default"): a string of its own.
  const SUMMARY_TEXT_SEL = '[data-testid="mod-setting-summary"]';
  const TITLE_SEL = '.windhawk-row-meta-title';
  // styled-components keeps the component name as the class prefix, so a
  // substring match survives the generated suffix.
  const TITLE_TEXT_SEL = '[class*="SettingTitleText"]';
  const SETTINGS_SELECTORS = [
    DESC_TEXT_SEL,
    // Claimed before the title host so a summary nested in the title row stays
    // its own unit.
    SUMMARY_TEXT_SEL,
    '.ant-select-item-option-content',
    // Only the text of a closed control's value/placeholder is swapped; the
    // search input stays atomic.
    ...SELECT_HOST_SELECTORS,
    // Claimed before the title host so a copy nested in the title row is its
    // own unit; the title reads only the title text.
    '.windhawk-row-meta-description',
    '.windhawk-row-meta-title',
    '[data-testid="mod-setting-reset"]'
  ];
  // Mod names: installed-list row title, tile Card.Meta title and the details
  // header's name div. On a tile only the Typography inside the mod-card-title
  // box is the name; the ribbon, local-mod icon and metadata line are siblings.
  const NAME_LIST_SELECTORS = ['.windhawk-row-meta-title', '.ant-card-meta-title'];
  const MOD_CARD_TITLE_SEL = '[data-testid="mod-card-title"]';
  // Never part of a name unit: an icon, the status ribbon, a metadata line.
  const NAME_IMPURE_SEL =
    'svg,[class*="ant-ribbon"],[class*="MetadataLine"],[data-testid*="metadata"]';
  // Mod summaries: tile/table descriptions and the details header Typography.
  const SUMMARY_LIST_SELECTORS = [
    '.windhawk-row-meta-description',
    '.ant-card-meta-description',
    '.ant-typography'
  ];
  const MODS_TABLE_SEL = '[data-testid="installed-mods-table"]';
  const ROW_SELECT_SEL = '[data-testid="mod-row-select"]';
  // The aggregated update wizard and the changelog dialog it opens. Both are
  // antd modals portaled to the document root, so they are scanned as their own
  // surfaces rather than through the details page. The wizard table is a CSS
  // grid with ARIA roles, not an antd table: the mod name is the first div in
  // the second cell of a `mod-update-row`; the last cell holds the button. The
  // dialog title repeats the mod name; the changelog body is the only prose
  // container (the sibling "changes" tab is a diff view, not markdown).
  const UPDATE_WIZARD_SEL = '[data-testid="mod-updates-modal"]';
  const UPDATE_DETAILS_MODAL_SEL = '[data-testid="mod-update-details-modal"]';
  // The single-mod run: its antd title is the mod name while the install runs
  // and after it ends, so it is a name host like the dialog's title.
  const UPDATE_RUN_MODAL_SEL = '[data-testid="mod-update-run-modal"]';
  const UPDATE_ROW_SEL = '[data-testid="mod-update-row"]';
  const UPDATE_CHANGELOG_SEL = '[data-testid="mod-update-changelog-content"]';

  // Subtrees left alone; everything else is an inline wrapper or an atomic
  // placeholder (see serializeHost).
  const SKIP_TAGS = new Set(['SCRIPT', 'STYLE', 'NOSCRIPT', 'TEMPLATE', 'LINK', 'META', 'BASE', 'TITLE']);
  const ATOMIC_TAGS = new Set([
    'CODE', 'PRE', 'KBD', 'SAMP', 'VAR', 'IMG', 'BR', 'HR', 'SVG', 'MATH',
    'VIDEO', 'AUDIO', 'CANVAS', 'IFRAME', 'OBJECT', 'EMBED', 'BUTTON',
    'WBR', 'INPUT', 'TEXTAREA', 'SELECT', 'OPTION', 'PROGRESS', 'METER',
    'COLGROUP', 'COL'
  ]);
  const STRUCT_TAGS = new Set([
    'A', 'SPAN', 'EM', 'STRONG', 'B', 'I', 'U', 'MARK', 'SMALL', 'SUP',
    'SUB', 'ABBR', 'DEL', 'INS', 'Q', 'CITE', 'TIME', 'BDI', 'BDO', 'FONT',
    'LABEL', 'RUBY', 'RT', 'RP', 'S'
  ]);
  const DENY_ANCESTOR_SEL =
    'script,style,noscript,template,textarea,input,select,option,code,pre,kbd,' +
    '.cm-editor,.monaco-editor,[contenteditable],[translate="no"]';

  const CJK_RE = /[\u2e80-\u2eff\u3040-\u30ff\u31f0-\u31ff\u3400-\u4dbf\u4e00-\u9fff\uf900-\ufaff\uac00-\ud7af]/;
  const URL_RE = /^https?:\/\/\S+$/i;
  const EMAIL_RE = /^[\w.+-]+@[\w-]+(?:\.[\w-]+)+$/;
  const WINPATH_RE = /^[a-z]:\\\S*$/i;
  const UNIXPATH_RE = /^(?:\.{0,2}\/|\/)[\w./\\-]*$/;
  const VERSION_RE = /^v?\d+(?:\.\d+){1,3}(?:[-+][\w.]+)?$/i;
  const NUMERIC_RE = /^[\d\s.,:;%+\-()[\]{}]+$/;
  const LETTER_RE = /\p{L}/u;
  // Dates written the way changelog headings write them: "Oct 30, 2026",
  // "30 October 2026", "2026-10-30". A standalone date is not prose.
  const MONTH_SRC =
    '(?:Jan(?:uary)?|Feb(?:ruary)?|Mar(?:ch)?|Apr(?:il)?|May|Jun(?:e)?|' +
    'Jul(?:y)?|Aug(?:ust)?|Sep(?:t(?:ember)?)?|Oct(?:ober)?|Nov(?:ember)?|Dec(?:ember)?)';
  const DATE_SRC =
    '(?:' + MONTH_SRC + '\\s+\\d{1,2},?\\s+\\d{4}|' +
    '\\d{1,2}\\s+' + MONTH_SRC + '\\s+\\d{4}|' +
    '\\d{4}-\\d{2}-\\d{2})';
  const DATE_RE = new RegExp('^' + DATE_SRC + '$', 'i');
  // A changelog heading carries a version and a date beside its prose. These
  // tokens are lifted out of the source as placeholders so a provider cannot
  // reword or reorder them (see protectPieces).
  const PROTECT_RE = new RegExp(
    '\\b(?:' + DATE_SRC + ')\\b|\\bv?\\d+(?:\\.\\d+){1,3}(?:[-+][\\w.]+)?\\b',
    'gi'
  );

  // A CJK source is already in the target script only when the target itself is
  // CJK; for en/de/fr/... a Chinese README or changelog must still be translated.
  function cjkTarget() {
    return /^(zh|ja|ko)\b/i.test(state.targetLang || '');
  }

  function isTranslatable(src) {
    if (typeof src !== 'string') return false;
    const t = src.replace(/\s+/g, ' ').trim();
    if (t.length < 2) return false;
    if (cjkTarget() && CJK_RE.test(t)) return false;
    if (!LETTER_RE.test(t)) return false;
    if (URL_RE.test(t)) return false;
    if (EMAIL_RE.test(t)) return false;
    if (WINPATH_RE.test(t)) return false;
    if (UNIXPATH_RE.test(t)) return false;
    if (VERSION_RE.test(t)) return false;
    if (DATE_RE.test(t)) return false;
    if (NUMERIC_RE.test(t)) return false;
    return true;
  }

  function scopeEnabled(surface) {
    const s = state.settings;
    if (surface === 'readme') return !!s.scopeReadme;
    if (surface === 'changelog') return !!s.scopeChangelog;
    if (surface === 'settings') return !!s.scopeSettings;
    if (surface === 'name') return !!s.scopeModName;
    if (surface === 'summary') return !!s.scopeModSummary;
    return !!s.scopeModSummary;
  }

  function classifyProse(body) {
    let node = body;
    for (let i = 0; node && i < 14; i++, node = node.parentElement) {
      let testid = '';
      try {
        testid = node.getAttribute('data-testid') || '';
      } catch (_) {
        testid = '';
      }
      if (/changelog|changes|version-history|release-notes/i.test(testid)) return 'changelog';
    }
    const card = body.closest ? body.closest('.ant-card') : null;
    const head = card ? card.querySelector('.ant-card-head-title, .ant-card-head') : null;
    if (head && /changelog|what'?s new|change ?log|history|\u66f4\u65b0|\u53d8\u66f4|\u5386\u53f2/i.test(head.textContent || '')) {
      return 'changelog';
    }
    const pane = body.closest ? body.closest('[role="tabpanel"]') : null;
    if (pane) {
      const labelled = pane.getAttribute('aria-labelledby');
      const tab = labelled ? document.getElementById(labelled) : null;
      if (tab && /changelog|changes|history|\u66f4\u65b0|\u53d8\u66f4|\u5386\u53f2/i.test(tab.textContent || '')) return 'changelog';
    }
    return 'readme';
  }

  function isUpdateModalHost(el) {
    try {
      return !!el.closest(
        UPDATE_WIZARD_SEL + ',' + UPDATE_DETAILS_MODAL_SEL + ',' + UPDATE_RUN_MODAL_SEL
      );
    } catch (_) {
      return false;
    }
  }

  function compactSurface(el) {
    try {
      // The update wizard is a list surface even though it is an antd modal:
      // its table cells hold mod names and the dialog header repeats the name.
      if (isUpdateModalHost(el)) return 'list';
      if (el.closest('[data-testid*="setting"], .ant-select-dropdown, .ant-modal, .ant-drawer, .ant-form')) {
        return 'settings';
      }
      const row = el.closest('.windhawk-row');
      if (
        row &&
        row.querySelector(
          '.ant-select,.ant-switch,.ant-input,.ant-btn,[data-testid="mod-setting-reset"],' +
          '[data-testid="mod-setting-description"],[data-testid="mod-setting-description-text"]'
        )
      ) {
        return 'settings';
      }
    } catch (_) {
      /* ignore */
    }
    return 'list';
  }

  function hasText(el) {
    try {
      return (el.textContent || '').trim().length >= 2;
    } catch (_) {
      return false;
    }
  }

  function settingsRejectReason(el) {
    try {
      if (el.closest('[' + ATTR_CLONE + '="1"]')) return 'inside our clone';
      if (el.closest(DENY_ANCESTOR_SEL)) return 'denied ancestor';
    } catch (_) {
      return 'unreadable';
    }
    if (!eligibleHost(el)) return 'card head';
    if (compactSurface(el) !== 'settings') return 'not a settings surface';
    // Inline wrappers are flattened by the serializer; only a host with no text
    // at all is skipped.
    if (!hasText(el)) return hasDirectText(el) ? 'no text' : 'no direct text';
    return '';
  }

  // The details header packs the mod name, id chip, metadata line and buttons
  // into one subtree. Only its summary Typography is prose, so the card-head
  // rule wins over every other selector.
  function isCardHeadHost(el) {
    try {
      return !!el.closest('.ant-card-head');
    } catch (_) {
      return false;
    }
  }

  function isTypography(el) {
    try {
      return !!el.matches('.ant-typography');
    } catch (_) {
      return false;
    }
  }

  // Metadata rows are Typography too but carry an icon and are proper nouns.
  // Only inside the card head does that disqualify a host.
  function hasSvgIcon(el) {
    try {
      return !!el.querySelector('svg');
    } catch (_) {
      return false;
    }
  }

  // A name unit is name text only: no icon, no status ribbon, no metadata line
  // text. Checked when a host is resolved and again before a unit is made.
  function nameHostClean(el) {
    if (!el || el.nodeType !== 1) return false;
    try {
      if (el.matches(NAME_IMPURE_SEL)) return false;
      if (el.closest('[class*="MetadataLine"],[data-testid*="metadata"]')) return false;
      if (el.querySelector(NAME_IMPURE_SEL)) return false;
    } catch (_) {
      return false;
    }
    return true;
  }

  // The details header's name is the first bare div in the meta title that owns
  // text directly; the metadata items hold theirs in icon-bearing spans.
  function headerNameHost(meta) {
    let divs = [];
    try {
      divs = Array.from(meta.querySelectorAll('div'));
    } catch (_) {
      divs = [];
    }
    for (const el of divs) {
      if (el.closest('.ant-typography')) continue;
      if (!hasDirectText(el)) continue;
      if (!hasText(el)) continue;
      if (!nameHostClean(el)) continue;
      return el;
    }
    return null;
  }

  // A tile's Card.Meta title: the name is the EllipsisText Typography inside
  // the mod-card-title box. The ribbon, local-mod icon and metadata line sit
  // beside it and must never join the unit.
  function tileNameHost(meta) {
    let box = null;
    try {
      box = meta.querySelector(MOD_CARD_TITLE_SEL);
    } catch (_) {
      box = null;
    }
    let typo = null;
    try {
      typo = (box || meta).querySelector('.ant-typography');
    } catch (_) {
      typo = null;
    }
    if (typo && typo !== meta && nameHostClean(typo)) return typo;
    // The older shape puts the name straight in the meta title; use it only
    // when it carries nothing but the name.
    if (!box && nameHostClean(meta)) return meta;
    return null;
  }

  // One resolver for every name surface: installed-list row title, tile name,
  // details header name.
  function resolveNameHost(el) {
    if (!el || el.nodeType !== 1) return null;
    try {
      if (el.matches('.windhawk-row-meta-title')) return nameHostClean(el) ? el : null;
      if (el.matches('.ant-card-meta-title')) {
        if (el.closest('.ant-card-head') || el.closest('[data-testid="mod-details"]')) {
          return headerNameHost(el);
        }
        return tileNameHost(el);
      }
    } catch (_) {
      return null;
    }
    return nameHostClean(el) ? el : null;
  }

  function detailsNameHosts() {
    const out = [];
    let metas = [];
    try {
      metas = Array.from(
        document.querySelectorAll(
          '.ant-card-head .ant-card-meta-title, [data-testid="mod-details"] .ant-card-meta-title'
        )
      );
    } catch (_) {
      return out;
    }
    for (const meta of metas) {
      const el = resolveNameHost(meta);
      if (el) out.push(el);
    }
    return out;
  }

  // The update wizard's table names and the changelog dialog title. The name is
  // the first div in the row's second cell (a failed source adds a note after
  // it); the dialog title is antd's `.ant-modal-title`. Both are swap hosts.
  function updateNameHosts() {
    const out = [];
    let rows = [];
    try {
      rows = Array.from(document.querySelectorAll(UPDATE_ROW_SEL));
    } catch (_) {
      rows = [];
    }
    for (const row of rows) {
      const cell = row.children && row.children[1];
      const name = cell && cell.firstElementChild;
      if (!name || name.tagName !== 'DIV') continue;
      if (!nameHostClean(name) || !hasText(name)) continue;
      out.push(name);
    }
    try {
      for (const modal of document.querySelectorAll(
        UPDATE_DETAILS_MODAL_SEL + ',' + UPDATE_RUN_MODAL_SEL
      )) {
        const title = modal.querySelector('.ant-modal-title');
        if (title && nameHostClean(title) && hasText(title)) out.push(title);
      }
    } catch (_) {
      /* ignore */
    }
    return out;
  }

  // Every name surface: tile/explore titles, table name link, details name div.
  // Settings titles share the row-title class and are kept out by compactSurface.
  function nameHosts() {
    const out = [];
    const add = (el) => {
      if (el && out.indexOf(el) === -1) out.push(el);
    };
    try {
      for (const a of document.querySelectorAll(MODS_TABLE_SEL + ' tbody a')) add(resolveNameHost(a));
    } catch (_) {
      /* ignore */
    }
    try {
      for (const box of document.querySelectorAll(ROW_SELECT_SEL)) {
        const cell = box.closest('td');
        if (cell) add(resolveNameHost(cell.querySelector('a')));
      }
    } catch (_) {
      /* ignore */
    }
    for (const sel of NAME_LIST_SELECTORS) {
      try {
        for (const el of document.querySelectorAll(sel)) {
          // A title inside a card head is header metadata, not a mod name.
          if (el.closest('.ant-card-head')) continue;
          add(resolveNameHost(el));
        }
      } catch (_) {
        /* ignore */
      }
    }
    for (const el of detailsNameHosts()) add(el);
    for (const el of updateNameHosts()) add(el);
    return out;
  }

  function summaryHosts() {
    const out = [];
    for (const sel of SUMMARY_LIST_SELECTORS) {
      try {
        for (const el of document.querySelectorAll(sel)) {
          // A tile's name Typography is never a summary, even with the name
          // scope off.
          if (sel === '.ant-typography' && el.closest(MOD_CARD_TITLE_SEL)) continue;
          if (out.indexOf(el) === -1) out.push(el);
        }
      } catch (_) {
        /* ignore */
      }
    }
    return out;
  }

  function eligibleHost(el, kind) {
    if (!el || el.nodeType !== 1) return false;
    if (!isCardHeadHost(el)) return true;
    if (kind === 'name') return !isTypography(el) && !hasSvgIcon(el);
    return isTypography(el) && !hasSvgIcon(el);
  }

  function hasDirectText(el) {
    try {
      for (let n = el.firstChild; n; n = n.nextSibling) {
        if (n.nodeType === 3 && /\S/.test(n.nodeValue || '')) return true;
      }
    } catch (_) {
      /* ignore */
    }
    return false;
  }

  function hasBlockChild(el, atomic, skipAria) {
    try {
      for (let n = el.firstElementChild; n; n = n.nextElementSibling) {
        // A claimed descendant or the excluded screen-reader copy is a
        // placeholder, not a block that makes this wrapper a container.
        if (atomic && atomic.has(n)) continue;
        if (isAriaCopy(n, skipAria)) continue;
        if (BLOCK_TAGS.has(n.tagName)) return true;
      }
    } catch (_) {
      /* ignore */
    }
    return false;
  }

  // Own sentence plus nested blocks: cloning would duplicate the nested items.
  function isMixedProseHost(el) {
    return hasDirectText(el) && hasBlockChild(el);
  }

  // Collect the prose blocks of one container. The details page uses it for
  // README/changelog card bodies; the update wizard's changelog dialog uses it
  // for `mod-update-changelog-content`, which is already known to be changelog
  // prose (the sibling "changes" tab is a diff view and is never scanned).
  function collectProseCandidates(body, surface, claim, claimSwap, candidates) {
    if (!scopeEnabled(surface)) return;
    let blocks = [];
    try {
      blocks = Array.from(body.querySelectorAll(PROSE_SEL));
    } catch (_) {
      blocks = [];
    }
    const mixed = [];
    for (const block of blocks) {
      if (block.closest('[' + ATTR_CLONE + '="1"]')) continue;
      if (block.closest(DENY_ANCESTOR_SEL)) continue;
      // A settings row inside the card body belongs to the swap pass, never a clone.
      if (
        block.closest('.windhawk-row') ||
        block.matches('.windhawk-row-meta-title, .windhawk-row-meta-description')
      ) {
        continue;
      }
      if (!eligibleHost(block)) continue;
      // Own sentence plus nested blocks: swap only the host's own text.
      if (isMixedProseHost(block)) {
        mixed.push(block);
        continue;
      }
      // Keep only the innermost block so a <li><p> pair is not rendered twice.
      try {
        if (block.querySelector(PROSE_SEL)) continue;
      } catch (_) {
        /* ignore */
      }
      if (!hasText(block)) continue;
      if (!claim(block)) continue;
      candidates.push({ el: block, mode: 'clone', surface: surface });
    }
    // Deepest first: each keeps the nested blocks as atomic placeholders.
    for (let i = mixed.length - 1; i >= 0; i--) {
      const block = mixed[i];
      if (!hasText(block)) continue;
      const atomic = claimSwap(block);
      if (!atomic) continue;
      candidates.push({ el: block, mode: 'swap', surface: surface, atomic: atomic });
    }
  }

  function scanAll() {
    if (state.dead) return;
    // The document-created injection may run before <head> exists.
    installCss();
    if (!state.enabled || !state.settingsSeen) return;
    state.stats.scans++;
    pruneUnits();

    const claimed = [];
    const candidates = [];
    const claim = (el) => {
      for (const host of claimed) {
        if (host === el) return false;
        if (host.contains(el) || el.contains(host)) return false;
      }
      claimed.push(el);
      return true;
    };
    // A swap host may contain units claimed before it (deepest first); those
    // stay atomic placeholders and are never translated twice.
    const claimSwap = (el) => {
      for (const host of claimed) {
        if (host === el || host.contains(el)) return null;
      }
      const atomic = [];
      for (const host of claimed) {
        if (el.contains(host)) atomic.push(host);
      }
      claimed.push(el);
      return atomic;
    };
    // A control nested in a claimed host (the reset button) gets its own unit.
    const claimInner = (el) => {
      for (const host of claimed) {
        if (host === el) return false;
        if (el.contains(host)) return false;
      }
      claimed.push(el);
      return true;
    };

    // README / changelog prose: React owns the nodes, so render a translated clone.
    if (state.settings.scopeReadme || state.settings.scopeChangelog) {
      let bodies = [];
      try {
        bodies = Array.from(document.querySelectorAll('[data-testid="mod-details"] .ant-card-body'));
      } catch (_) {
        bodies = [];
      }
      for (const body of bodies) {
        collectProseCandidates(body, classifyProse(body), claim, claimSwap, candidates);
      }
    }

    // The update wizard's changelog dialog lives in a portal outside the details
    // page. Its container is explicit, so the surface is forced to changelog and
    // the sibling "changes" diff tab is left alone.
    if (scopeEnabled('changelog')) {
      let contents = [];
      try {
        contents = Array.from(document.querySelectorAll(UPDATE_CHANGELOG_SEL));
      } catch (_) {
        contents = [];
      }
      for (const content of contents) {
        collectProseCandidates(content, 'changelog', claim, claimSwap, candidates);
      }
    }

    // Settings surfaces: text-node swap, never a clone.
    if (state.settings.scopeSettings) {
      for (const sel of SETTINGS_SELECTORS) {
        let list = [];
        try {
          list = Array.from(document.querySelectorAll(sel));
        } catch (_) {
          list = [];
        }
        for (const el of list) {
          // A hidden copy nested in a title is never its own unit; the visible
          // tooltip portal outside the title is claimed like any description.
          if (sel === DESC_TEXT_SEL && el.closest(TITLE_SEL)) continue;
          const reason = settingsRejectReason(el);
          if (reason) continue;
          // A title with no text but the excluded copy gets no unit.
          if (isTitleHost(el) && !isTranslatable(normalizeSource(titleHostText(el, claimed)))) continue;
          const atomic = claimSwap(el);
          if (!atomic) continue;
          candidates.push({ el: el, mode: 'swap', surface: 'settings', atomic: atomic });
        }
      }
      // The reset button is an atomic token in the title's source; give its
      // label a nested unit so it translates too.
      let resets = [];
      try {
        resets = Array.from(document.querySelectorAll('[data-testid="mod-setting-reset"]'));
      } catch (_) {
        resets = [];
      }
      for (const el of resets) {
        if (el.closest('[' + ATTR_CLONE + '="1"]')) continue;
        if (el.closest(DENY_ANCESTOR_SEL)) continue;
        if (!eligibleHost(el)) continue;
        if (compactSurface(el) !== 'settings') continue;
        if (!hasText(el)) continue;
        if (!claimInner(el)) continue;
        candidates.push({ el: el, mode: 'swap', surface: 'settings' });
      }
    }

    // Mod names: swap mode keeps the link/div and replaces only its text node.
    if (scopeEnabled('name')) {
      for (const el of nameHosts()) {
        if (el.closest('[' + ATTR_CLONE + '="1"]')) continue;
        if (el.closest(DENY_ANCESTOR_SEL)) continue;
        if (!eligibleHost(el, 'name')) continue;
        if (compactSurface(el) !== 'list') continue;
        if (!hasText(el)) continue;
        const atomic = claimSwap(el);
        if (!atomic) continue;
        candidates.push({ el: el, mode: 'swap', surface: 'name', atomic: atomic });
      }
    }

    // Mod summaries: same swap rule.
    if (scopeEnabled('summary')) {
      for (const el of summaryHosts()) {
        if (el.closest('[' + ATTR_CLONE + '="1"]')) continue;
        if (el.closest(DENY_ANCESTOR_SEL)) continue;
        if (!eligibleHost(el, 'summary')) continue;
        if (compactSurface(el) !== 'list') continue;
        if (!hasText(el)) continue;
        const atomic = claimSwap(el);
        if (!atomic) continue;
        candidates.push({ el: el, mode: 'swap', surface: 'summary', atomic: atomic });
      }
    }

    state.stats.scannedHosts += candidates.length;
    state.scanQueue = { items: candidates, at: 0, epoch: state.epoch };
    scheduleScanChunk();
  }

  function scheduleScanChunk() {
    if (state.scanScheduled || state.dead) return;
    state.scanScheduled = true;
    idle((deadline) => {
      state.scanScheduled = false;
      safe(() => runScanChunk(deadline), 'scan-chunk');
    });
  }

  function runScanChunk(deadline) {
    if (!state.enabled) {
      state.scanQueue = null;
      return;
    }
    const queue = state.scanQueue;
    if (!queue) return;
    if (queue.epoch !== state.epoch) {
      state.scanQueue = null;
      return;
    }
    let n = 0;
    while (queue.at < queue.items.length) {
      const item = queue.items[queue.at++];
      if (item && item.el && item.el.isConnected) {
        safe(() => processHost(item.el, item.mode, item.surface, item.atomic), 'scan-host');
      }
      n++;
      const left = deadline && typeof deadline.timeRemaining === 'function' ? deadline.timeRemaining() : 8;
      if (n >= 25 || left < 2) break;
    }
    if (queue.at < queue.items.length) scheduleScanChunk();
    else if (state.scanQueue === queue) state.scanQueue = null;
  }

  function scheduleScan(delay) {
    if (state.dead) return;
    if (state.scanTimer) clearTimeout(state.scanTimer);
    state.scanTimer = setTimeout(() => {
      state.scanTimer = 0;
      safe(scanAll, 'scan');
    }, Math.max(0, delay || 0));
  }

  function pruneUnits() {
    for (const u of Array.from(state.units)) {
      if (!u.el || !u.el.isConnected) dropUnit(u);
    }
  }

  // ---------------------------------------------------------------------------
  // Serialization: text with placeholders
  //
  // clone mode: <n>inner</n> structural wrapper, {{n}} atomic node
  // swap mode:  {{n}} only; inline wrappers are flattened into text
  // ---------------------------------------------------------------------------

  function isSkippedElement(el) {
    if (!el || el.nodeType !== 1) return true;
    if (SKIP_TAGS.has(el.tagName)) return true;
    if (el.matches('.cm-editor, .monaco-editor, [contenteditable], [translate="no"]')) return true;
    return false;
  }

  function isAtomicElement(el) {
    if (ATOMIC_TAGS.has(el.tagName)) return true;
    if (el.matches('.cm-editor, .monaco-editor, [contenteditable], [translate="no"]')) return true;
    return false;
  }

  // Swap mode treats inline wrappers as text flow (a title span, a select's
  // value span); clone mode keeps structural markup.
  function isInlineWrapper(el, mode, atomic, skipAria) {
    if (STRUCT_TAGS.has(el.tagName)) return true;
    if (mode !== 'swap') return false;
    if (!BLOCK_TAGS.has(el.tagName)) return true;
    // A bare div used as a label line is still part of the text flow.
    return el.tagName === 'DIV' && !hasBlockChild(el, atomic, skipAria);
  }

  // A setting title host: only its title text is the unit.
  function isTitleHost(el) {
    try {
      return !!el.matches(TITLE_SEL);
    } catch (_) {
      return false;
    }
  }

  // The hidden description copy duplicates the tooltip; inside a title it is
  // dropped from the source entirely, not even emitted as a placeholder.
  function isAriaCopy(el, hostIsTitle) {
    try {
      if (!el.matches(DESC_TEXT_SEL)) return false;
      return hostIsTitle || !!el.closest(TITLE_SEL);
    } catch (_) {
      return false;
    }
  }

  // The title text span inside the row; the closest-title check keeps a nested
  // row's title from being taken for this row's.
  function titleTextHost(host) {
    try {
      const el = host.querySelector(TITLE_TEXT_SEL);
      if (el && el !== host && el.closest(TITLE_SEL) === host && !isTitleExtra(el)) return el;
    } catch (_) {
      /* ignore */
    }
    return null;
  }

  // Row content that shares the title row but never joins the title text.
  function isTitleExtra(el) {
    try {
      return !!el.closest(SUMMARY_TEXT_SEL) || !!el.closest('[data-testid="mod-setting-reset"]');
    } catch (_) {
      return false;
    }
  }

  // Walk the title text span, or the host's direct text when there is none.
  // Claimed descendants are skipped: they never become a placeholder here.
  function walkTitleText(root, directOnly, onText, atomic) {
    const atomics = atomic && atomic.length ? new Set(atomic) : null;
    const visit = (node) => {
      for (let child = node.firstChild; child; child = child.nextSibling) {
        if (child.nodeType === 3) {
          onText(child, child.nodeValue || '');
          continue;
        }
        if (child.nodeType !== 1) continue;
        if (directOnly) continue;
        if (atomics && atomics.has(child)) continue;
        if (isSkippedElement(child)) continue;
        if (isAtomicElement(child)) continue;
        if (isAriaCopy(child, true)) continue;
        if (isTitleExtra(child)) continue;
        visit(child);
      }
    };
    visit(root);
  }

  function titleHostText(host, atomic) {
    const target = titleTextHost(host);
    let text = '';
    walkTitleText(target || host, !target, (node, value) => {
      text += value;
    }, atomic);
    return text;
  }

  // Title units are text only: no refs, no placeholders.
  function serializeTitleHost(host, atomic) {
    const target = titleTextHost(host);
    const segs = [];
    let text = '';
    walkTitleText(target || host, !target, (node, value) => {
      segs.push({ kind: 'text', node: node, orig: value, out: value });
      state.origText.set(node, value);
      text += value;
    }, atomic);
    return { text: text, refs: [], segs: segs };
  }

  // Split a text run around version/date tokens. Only used for changelog
  // headings: the tokens become ordinary `{{n}}` placeholders whose ref is a
  // text node, so a provider never sees or rewrites them.
  function protectPieces(text) {
    const pieces = [];
    let last = 0;
    let m;
    PROTECT_RE.lastIndex = 0;
    while ((m = PROTECT_RE.exec(text))) {
      if (m.index > last) pieces.push({ text: text.slice(last, m.index), token: false });
      pieces.push({ text: m[0], token: true });
      last = PROTECT_RE.lastIndex;
    }
    if (last < text.length) pieces.push({ text: text.slice(last), token: false });
    return pieces;
  }

  // Stable text-node refs for protected tokens: the same host, slot and text
  // reuse the same node, so refsEqual() does not force a rebuild on every scan.
  function literalRef(host, text, slot) {
    let map = state.literalRefs.get(host);
    if (!map) {
      map = new Map();
      state.literalRefs.set(host, map);
    }
    const key = slot + '\u0000' + text;
    let node = map.get(key);
    if (!node) {
      node = document.createTextNode(text);
      map.set(key, node);
    }
    return node;
  }

  // `atomic` descendants are emitted as placeholders and never walked;
  // `skipAria` drops the hidden description copy without a placeholder.
  // `protect` lifts version/date tokens out of a changelog heading's text.
  function serializeHost(host, mode, atomic, skipAria, protect) {
    const atomics = atomic && atomic.length ? new Set(atomic) : null;
    const refs = [];
    const segs = [];
    let out = '';
    let protectSlot = 0;
    const walk = (node) => {
      let child = node.firstChild;
      while (child) {
        if (child.nodeType === 3) {
          const value = child.nodeValue || '';
          if (mode === 'swap') {
            segs.push({ kind: 'text', node: child, orig: value, out: value });
            state.origText.set(child, value);
          }
          if (protect && mode === 'clone') {
            for (const piece of protectPieces(value)) {
              if (!piece.token) {
                out += piece.text;
                continue;
              }
              const index = refs.length;
              refs.push(literalRef(host, piece.text, protectSlot++));
              out += '{{' + index + '}}';
            }
          } else {
            out += value;
          }
        } else if (child.nodeType === 1) {
          const el = child;
          if (!isSkippedElement(el) && !isAriaCopy(el, skipAria)) {
            if ((atomics && atomics.has(el)) || isAtomicElement(el)) {
              const index = refs.length;
              refs.push(el);
              out += '{{' + index + '}}';
              if (mode === 'swap') segs.push({ kind: 'token', index: index, el: el });
            } else if (isInlineWrapper(el, mode, atomics, skipAria)) {
              if (mode === 'clone') {
                const index = refs.length;
                refs.push(el);
                out += '<' + index + '>';
                walk(el);
                out += '</' + index + '>';
              } else {
                walk(el);
              }
            } else {
              const index = refs.length;
              refs.push(el);
              out += '{{' + index + '}}';
              if (mode === 'swap') segs.push({ kind: 'token', index: index, el: el });
            }
          }
        }
        child = child.nextSibling;
      }
    };
    walk(host);
    return { text: out, refs: refs, segs: segs };
  }

  function splitSerialized(src, limit) {
    if (src.length <= limit) return [src];
    const cuts = [];
    let depth = 0;
    for (let i = 0; i < src.length; ) {
      const c = src[i];
      if (c === '{' && src[i + 1] === '{') {
        const end = src.indexOf('}}', i + 2);
        if (end !== -1) {
          i = end + 2;
          continue;
        }
      }
      if (c === '<') {
        const m = /^<(\/?)(\d+)>/.exec(src.slice(i, i + 12));
        if (m) {
          depth += m[1] ? -1 : 1;
          i += m[0].length;
          continue;
        }
      }
      if (depth <= 0) {
        const boundary =
          c === '.' || c === '!' || c === '?' || c === ';' ||
          c === '\u3002' || c === '\uff01' || c === '\uff1f' || c === '\uff1b' ||
          c === '\n';
        if (boundary && /[\s"'\u201d\u2019)\]]/.test(src[i + 1] || ' ')) cuts.push(i + 1);
      }
      i++;
    }
    if (!cuts.length) return [src];

    const parts = [];
    let start = 0;
    let k = 0;
    while (src.length - start > limit) {
      let chosen = -1;
      while (k < cuts.length && cuts[k] <= start) k++;
      for (let j = k; j < cuts.length; j++) {
        if (cuts[j] - start > limit) break;
        chosen = cuts[j];
      }
      if (chosen <= start) {
        if (k < cuts.length) chosen = cuts[k];
        else break;
      }
      if (chosen <= start) break;
      parts.push(src.slice(start, chosen));
      start = chosen;
    }
    if (start < src.length) parts.push(src.slice(start));
    return parts.filter((p) => p.length > 0);
  }

  // ---------------------------------------------------------------------------
  // Units
  // ---------------------------------------------------------------------------

  function makeUnit(el, mode, surface) {
    const u = {
      id: state.nextUnitId++,
      el: el,
      mode: mode,
      surface: surface,
      source: '',
      sourceHash: '',
      leadTokens: '',
      trailTokens: '',
      refs: [],
      segs: [],
      parts: [],
      translatedOk: false,
      noop: false,
      rendered: false,
      skip: false,
      clone: null,
      hoverOrig: false,
      tapOrig: false,
      hoverTimer: 0,
      leaveTimer: 0,
      appliedShowOrig: null
    };
    state.unitByHost.set(el, u);
    state.units.add(u);
    return u;
  }

  function restoreSwapText(u) {
    if (!u || !u.segs) return;
    for (const seg of u.segs) {
      if (seg.kind !== 'text') continue;
      const node = seg.node;
      if (!node || !node.isConnected) continue;
      const orig = state.origText.has(node) ? state.origText.get(node) : seg.orig;
      // Only undo our own translation; never overwrite text React just wrote.
      if (seg.out !== orig && node.nodeValue === seg.out) {
        node.nodeValue = orig;
        state.written.set(node, orig);
      }
    }
  }

  function swapChanged(u) {
    if (!u.segs || !u.segs.length) return true;
    for (const seg of u.segs) {
      if (seg.kind !== 'text') continue;
      if (!seg.node || !seg.node.isConnected) return true;
      const value = seg.node.nodeValue;
      if (value !== seg.orig && value !== seg.out) return true;
    }
    return false;
  }

  // A closed select renders its value with the search input as an edge token
  // ("Where the mouse cursor is{{0}}"), the dropdown option without it. Both
  // must share one source and translation, so edge tokens are split off and
  // re-attached around the output; their DOM nodes never move.
  function splitEdgeTokens(text) {
    let lead = '';
    let trail = '';
    let rest = text;
    const head = /^(?:\{\{\d+\}\})+/.exec(rest);
    if (head) {
      lead = head[0];
      rest = rest.slice(lead.length);
    }
    const tail = /(?:\{\{\d+\}\})+$/.exec(rest);
    if (tail) {
      trail = tail[0];
      rest = rest.slice(0, rest.length - trail.length);
    }
    return { text: normalizeSource(rest), lead: lead, trail: trail };
  }

  // "<prefix>: <tail>" whose tail is an option label with a known translation:
  // reuse that translation instead of wording the sentence differently.
  const RESET_LABEL_RE = /^([^:：]{1,80}?)([:：][ ]?)(\S(?:[\s\S]*\S)?)$/;

  function resetLabelParts(u, source) {
    if (u.surface !== 'settings') return null;
    const m = RESET_LABEL_RE.exec(source);
    if (!m) return null;
    const prefix = m[1].trim();
    const sep = m[2];
    const tail = m[3].trim();
    if (!prefix || !tail) return null;
    if (!isTranslatable(prefix)) return null;
    // Only compose when the tail is already translated; otherwise translate the
    // whole sentence as one piece.
    const tailTranslated = lookupTranslation(tail);
    if (tailTranslated === undefined) return null;
    return [
      {
        text: prefix,
        translated: null,
        state: 0,
        unit: u,
        sep: sep,
        tail: tail,
        tailTranslated: tailTranslated
      }
    ];
  }

  function buildParts(u, source) {
    const reset = resetLabelParts(u, source);
    if (reset) return reset;
    return splitSerialized(source, 1000).map((text) => ({
      text: text,
      translated: null,
      state: 0,
      unit: u
    }));
  }

  function resetForSource(u, serialized) {
    // Parts built for the previous source are dead; release them before the
    // unit replaces its list, or they would keep the unit in entry.parts.
    detachUnitParts(u);
    // Downstream keys use the normalised source; the DOM keeps its whitespace.
    const source = normalizeSource(serialized.text);
    const edge = u.mode === 'swap' ? splitEdgeTokens(source) : { text: source, lead: '', trail: '' };
    u.refs = serialized.refs;
    u.segs = serialized.segs;
    u.source = source;
    u.sourceHash = hash32(source);
    u.leadTokens = edge.lead;
    u.trailTokens = edge.trail;
    u.translatedOk = false;
    u.noop = false;
    u.rendered = false;
    u.appliedShowOrig = null;
    u.hoverOrig = false;
    u.tapOrig = false;
    if (u.hoverTimer) {
      clearTimeout(u.hoverTimer);
      u.hoverTimer = 0;
    }
    if (u.leaveTimer) {
      clearTimeout(u.leaveTimer);
      u.leaveTimer = 0;
    }
    if (u.clone) {
      try {
        u.clone.remove();
      } catch (_) {
        /* ignore */
      }
      u.clone = null;
    }
    try {
      u.el.removeAttribute(ATTR_ORIG);
      u.el.removeAttribute(ATTR_SWAPPED);
      u.el.setAttribute(ATTR_UNIT, String(u.id));
    } catch (_) {
      /* ignore */
    }
    // u.source keeps the edge tokens for the no-op comparison; requests do not.
    if (!isTranslatable(edge.text)) {
      u.skip = true;
      u.parts = [];
      return;
    }
    u.skip = false;
    u.parts = buildParts(u, edge.text);
    for (const part of u.parts) enqueuePart(u, part);
  }

  function processHost(el, mode, surface, atomic) {
    const atomics = atomic || [];
    // A settings title is a unit of its title text only; the rest of the row is
    // other units or excluded.
    const titleUnit = surface === 'settings' && isTitleHost(el);
    let u = state.unitByHost.get(el);
    if (!el.isConnected) {
      if (u) dropUnit(u);
      return;
    }
    // A name unit is name text only; refuse an icon, ribbon or metadata host
    // even if a caller ever hands one over.
    if (surface === 'name' && !nameHostClean(el)) {
      if (u) dropUnit(u);
      return;
    }
    // A title stripped down to the hidden copy keeps no unit.
    if (titleUnit && !isTranslatable(normalizeSource(titleHostText(el, atomics)))) {
      if (u) dropUnit(u);
      return;
    }
    // A hidden copy moved inside a title is no longer a unit.
    if (isAriaCopy(el, false)) {
      if (u) dropUnit(u);
      return;
    }
    if (!u) {
      u = makeUnit(el, mode, surface);
    } else {
      u.surface = surface;
    }

    if (u.mode !== mode) {
      if (u.mode === 'swap') restoreSwapText(u);
      if (u.clone) {
        try {
          u.clone.remove();
        } catch (_) {
          /* ignore */
        }
        u.clone = null;
      }
      u.rendered = false;
      try {
        u.el.removeAttribute(ATTR_ORIG);
      } catch (_) {
        /* ignore */
      }
    }
    u.mode = mode;

    let serialized = null;
    if (mode === 'swap' && u.segs && u.segs.length) {
      if (!swapChanged(u)) {
        ensureUnitRendered(u, true);
        return;
      }
      restoreSwapText(u);
      serialized = titleUnit ? serializeTitleHost(el, atomics) : serializeHost(el, 'swap', atomics, false, false);
    } else {
      // Version/date tokens in a changelog heading stay verbatim even when the
      // heading also carries prose that must be translated.
      const protect = mode === 'clone' && surface === 'changelog' && /^H[1-6]$/.test(el.tagName);
      serialized = titleUnit ? serializeTitleHost(el, atomics) : serializeHost(el, mode, atomics, false, protect);
    }

    const hash = hash32(normalizeSource(serialized.text));
    if (u.sourceHash === hash && (u.translatedOk || u.parts.length)) {
      const refsSame = refsEqual(u.refs, serialized.refs);
      u.refs = serialized.refs;
      u.segs = serialized.segs;
      if (mode === 'swap') {
        // React replaced the text nodes with identical text: remap the outputs.
        if (u.translatedOk && !u.noop && u.parts.length) {
          computeSwapOutputs(u);
          u.rendered = true;
          state.unitByDisplay.set(el, u);
        }
        ensureUnitRendered(u, true);
        return;
      }
      if (!refsSame && u.rendered) {
        if (u.clone) {
          try {
            u.clone.remove();
          } catch (_) {
            /* ignore */
          }
          u.clone = null;
        }
        u.rendered = false;
        try {
          u.el.removeAttribute(ATTR_ORIG);
        } catch (_) {
          /* ignore */
        }
        scheduleRender(u);
      } else {
        ensureUnitRendered(u, true);
      }
      return;
    }

    resetForSource(u, serialized);
  }

  function refsEqual(a, b) {
    if (!a || !b || a.length !== b.length) return false;
    for (let i = 0; i < a.length; i++) if (a[i] !== b[i]) return false;
    return true;
  }

  // Remove a unit's parts from the shared registry entries that still list
  // them. A completed entry with no parts is the session's translation
  // registry, and a pending one still completes into it, so the entry itself
  // stays; only the references to this unit go.
  function detachUnitParts(u) {
    if (!u || !u.parts) return;
    for (const part of u.parts) {
      const entry = part.entry;
      if (!entry || !entry.parts) continue;
      const at = entry.parts.indexOf(part);
      if (at !== -1) entry.parts.splice(at, 1);
    }
  }

  // The host left the document while its reply was in flight: the translation
  // now lives in the session registry, but this unit's parts and serialized
  // source no longer describe anything it can render. Clear them so the next
  // scan re-serializes the DOM (instead of concluding the text is unchanged)
  // and consumes the completed entry without another request. A unit with any
  // dead part is invalidated as a whole: keeping the remaining parts would
  // leave it with a partial source.
  function invalidateUnit(u) {
    if (!u || !state.units.has(u)) return;
    // A unit that already rendered goes back to its original text first, so
    // the rescan serializes the DOM as React authored it.
    if (u.mode === 'swap' && u.rendered) restoreSwapText(u);
    if (u.clone) {
      try {
        u.clone.remove();
      } catch (_) {
        /* ignore */
      }
      u.clone = null;
    }
    detachUnitParts(u);
    u.parts = [];
    u.source = '';
    u.sourceHash = '';
    u.refs = [];
    u.segs = [];
    u.leadTokens = '';
    u.trailTokens = '';
    u.translatedOk = false;
    u.noop = false;
    u.rendered = false;
    u.skip = false;
    u.appliedShowOrig = null;
    u.hoverOrig = false;
    u.tapOrig = false;
    if (u.hoverTimer) {
      clearTimeout(u.hoverTimer);
      u.hoverTimer = 0;
    }
    if (u.leaveTimer) {
      clearTimeout(u.leaveTimer);
      u.leaveTimer = 0;
    }
    state.renderQueue.delete(u);
  }

  function dropUnit(u) {
    if (!u) return;
    if (u.hoverTimer) clearTimeout(u.hoverTimer);
    if (u.leaveTimer) clearTimeout(u.leaveTimer);
    u.hoverTimer = 0;
    u.leaveTimer = 0;
    // Release the unit from every registry entry that still lists its parts.
    // Otherwise a closed modal's unit and DOM stay reachable through
    // state.textEntries after a request that was not served from the cache.
    detachUnitParts(u);
    u.parts = [];
    try {
      if (u.mode === 'swap') restoreSwapText(u);
      if (u.clone) u.clone.remove();
      if (u.el) {
        u.el.removeAttribute(ATTR_ORIG);
        u.el.removeAttribute(ATTR_SWAPPED);
        u.el.removeAttribute(ATTR_UNIT);
      }
    } catch (_) {
      /* ignore */
    }
    if (u.clone) state.unitByDisplay.delete(u.clone);
    u.clone = null;
    if (u.el) {
      state.unitByHost.delete(u.el);
      state.unitByDisplay.delete(u.el);
    }
    state.units.delete(u);
    state.renderQueue.delete(u);
  }

  function teardownAll() {
    for (const u of Array.from(state.units)) dropUnit(u);
    state.queue.length = 0;
    // The dedupe map is keyed by language/provider, so a rebuild must not reuse it.
    state.textEntries.clear();
    clearCacheLookups();
    state.renderQueue.clear();
    if (state.flushTimer) {
      clearTimeout(state.flushTimer);
      state.flushTimer = 0;
    }
  }

  // ---------------------------------------------------------------------------
  // Translation queue
  // ---------------------------------------------------------------------------

  // The provider actually serving requests right now. A failed LLM session
  // falls back to Edge and never retries the LLM until the page reloads.
  function activeProvider() {
    return state.provider === 'llm' && !state.llmFallback ? 'llm' : 'edge';
  }

  function llmConfigured() {
    const s = state.settings;
    // The model is optional: the endpoint's default applies when it is empty.
    return !!String(s.llmBaseUrl || '').trim();
  }

  // Settings JSON has no glossary key yet; read it when a build adds one.
  // Entries are `source=target` strings (also accept an object map).
  function glossaryEntries() {
    const raw = state.settings.glossary;
    const out = [];
    const seen = new Set();
    const add = (src, dst) => {
      const a = String(src == null ? '' : src).trim();
      const b = String(dst == null ? '' : dst).trim();
      if (!a || !b) return;
      const key = a.toLowerCase();
      if (seen.has(key)) return;
      seen.add(key);
      out.push([a, b]);
    };
    if (Array.isArray(raw)) {
      for (const item of raw) {
        const line = String(item == null ? '' : item).trim();
        if (!line) continue;
        const at = line.indexOf('=');
        if (at <= 0) continue;
        add(line.slice(0, at), line.slice(at + 1));
      }
    } else if (raw && typeof raw === 'object') {
      for (const key of Object.keys(raw)) add(key, raw[key]);
    }
    return out;
  }

  function glossarySignature(settings) {
    const raw = settings && settings.glossary;
    if (!Array.isArray(raw)) return raw && typeof raw === 'object' ? JSON.stringify(raw) : '';
    return raw.map((item) => String(item == null ? '' : item).trim()).filter(Boolean).join('\u0001');
  }

  function glossaryObject(entries) {
    const out = {};
    for (const [src, dst] of entries) out[src] = dst;
    return out;
  }

  function buildLlmSystemPrompt(entries) {
    const lines = [
      'You are a translation API. Translate every input segment into ' +
        (state.targetLang || 'the target language') + '.',
      'Return one raw JSON object and nothing else: {"translations":[{"id":<n>,"text":"<translation>"}]}',
      'No prose, no explanations, no markdown and no code fences.',
      'Keep every placeholder exactly as given: {{n}} atomic tokens and <n>...</n> structural tags ' +
        '(also written <n>…</n>). Never translate, rename, drop, duplicate or reorder them.',
      'Translate only human-readable text. Never translate code, identifiers, URLs, version numbers, ' +
        'or text that is already in the target language.',
      'Keep the id, count and order of the input segments.'
    ];
    if (entries.length) {
      lines.push('Glossary (mandatory, takes priority over any other wording; apply every glossary entry):');
      for (const [src, dst] of entries) lines.push('- ' + src + '=' + dst);
    }
    return lines.join('\n');
  }

  function llmUrl() {
    const base = String(state.settings.llmBaseUrl || '').trim().replace(/\/+$/, '');
    if (!base) return '';
    return base + '/chat/completions';
  }

  function buildLlmBody(entries) {
    const glossary = glossaryEntries();
    const user = {
      targetLanguage: state.targetLang,
      segments: entries.map((entry, index) => ({ id: index + 1, text: entry.text }))
    };
    if (glossary.length) user.glossary = glossaryObject(glossary);
    const body = {
      temperature: 0,
      messages: [
        { role: 'system', content: buildLlmSystemPrompt(glossary) },
        { role: 'user', content: JSON.stringify(user) }
      ]
    };
    // An empty model omits the key entirely so the endpoint's default applies.
    const model = String(state.settings.llmModel || '').trim();
    if (model) body.model = model;
    return JSON.stringify(body);
  }

  function warnLlmOnce(why) {
    if (state.llmWarned) return;
    state.llmWarned = true;
    log(
      'llm: ' + why + '; falling back to Edge (Edge fallback)' +
      (state.llmFallback ? ' for this session' : ' for these segments')
    );
  }

  function disableLlm(why) {
    state.llmFallback = true;
    warnLlmOnce(why);
  }

  // A part's output, composing the cached tail for a reset label.
  function partOutput(p) {
    if (!p || p.translated == null) return null;
    if (p.tailTranslated != null) return p.translated + p.sep + p.tailTranslated;
    return p.translated;
  }

  // The unit's rendered text including edge tokens, for the no-op comparison.
  function unitOutput(u) {
    if (!u || !u.parts) return '';
    return (u.leadTokens || '') + u.parts.map((p) => partOutput(p) || '').join('') + (u.trailTokens || '');
  }

  // A string already known this session; used so a reset label never disagrees
  // with the option label. The mod's store is only reachable asynchronously, so
  // a reset label composes from this session's completed entries.
  function lookupTranslation(text) {
    const entry = state.textEntries.get(text);
    if (entry && entry.state === 3 && typeof entry.translated === 'string') return entry.translated;
    return undefined;
  }

  function attachPart(entry, u, part) {
    part.entry = entry;
    if (entry.state === 3) {
      // Already translated: fan the result out, no request and no bookkeeping.
      part.translated = entry.translated;
      part.state = 3;
      maybeFinishUnit(u);
      return;
    }
    if (entry.state === 4) {
      part.state = 4;
      return;
    }
    entry.parts.push(part);
    part.state = entry.state === 0 ? 0 : 1;
  }

  function enqueuePart(u, part) {
    if (!state.enabled || state.dead) return;
    if (!state.targetLang) {
      part.state = 4;
      return;
    }
    const provider = activeProvider();
    const text = part.text;
    const existing = state.textEntries.get(text);
    if (existing) {
      // The same normalised string as another unit: share its request and result.
      attachPart(existing, u, part);
      return;
    }
    // state 0: waiting for the mod's cache reply; 1: queued/in flight.
    const entry = { text: text, state: 0, translated: null, parts: [], provider: provider };
    state.textEntries.set(text, entry);
    attachPart(entry, u, part);
    if (state.settings.cacheEnabled) requestCached(entry);
    else queueEntry(entry);
  }

  function queueEntry(entry) {
    if (state.dead || entry.state !== 0) return;
    entry.state = 1;
    for (const p of entry.parts) p.state = 1;
    state.queue.push(entry);
    scheduleFlush();
  }

  function scheduleFlush() {
    if (state.flushTimer || state.dead) return;
    const ms = clamp(state.settings.batchIntervalMs, 0, 10000);
    state.flushTimer = setTimeout(() => {
      state.flushTimer = 0;
      safe(flushQueue, 'flush');
    }, ms);
  }

  function flushQueue() {
    if (state.dead || !state.enabled || !state.targetLang) return;
    const s = state.settings;
    // The LLM protocol is capped harder than Edge: 12 segments / 6000 chars.
    const llm = activeProvider() === 'llm';
    const maxSegments = llm ? Math.min(s.batchSize, LLM_MAX_SEGMENTS) : s.batchSize;
    const maxChars = llm ? Math.min(s.batchChars, LLM_MAX_CHARS) : s.batchChars;
    while (state.queue.length && state.inflight < s.concurrency) {
      const batch = [];
      let chars = 0;
      while (state.queue.length && batch.length < maxSegments) {
        // The queue holds distinct normalised strings only.
        const entry = state.queue[0];
        if (batch.length && chars + entry.text.length > maxChars) break;
        state.queue.shift();
        batch.push(entry);
        chars += entry.text.length;
      }
      if (!batch.length) break;
      state.inflight++;
      state.stats.batches++;
      const epoch = state.epoch;
      runBatch(batch, epoch)
        .catch((e) => problem('batch: ' + (e && e.message ? e.message : e)))
        .finally(() => {
          state.inflight--;
          safe(flushQueue, 'flush-next');
        });
    }
    if (state.queue.length) scheduleFlush();
  }

  // Fan a finished translation out to every part that still belongs to a live
  // unit. Used for both a cache hit and a provider reply.
  function completeEntry(entry, translated, provider) {
    entry.translated = translated;
    entry.state = 3;
    entry.provider = provider || activeProvider();
    const units = new Set();
    const stale = new Set();
    const live = [];
    for (const p of Array.from(entry.parts)) {
      const u = p.unit;
      // A part whose unit was dropped, is no longer registered, or whose host
      // has left the document is dead: release it instead of reviving it. A
      // host that comes back (a modal unmounted while its reply was in flight)
      // is re-sourced by the next scan from this entry, so the whole unit is
      // invalidated rather than left with a partial list of parts.
      const dead =
        !u ||
        !u.el ||
        !u.el.isConnected ||
        !state.units.has(u) ||
        !u.parts ||
        u.parts.indexOf(p) === -1;
      if (dead) {
        if (u) stale.add(u);
        continue;
      }
      p.translated = translated;
      p.state = 3;
      live.push(p);
      units.add(u);
    }
    // Invalidate after the loop: releasing a stale unit's parts mutates the
    // entry lists, and every part of that unit must go, not just the first.
    for (const u of stale) units.delete(u);
    entry.parts = live.filter((p) => !stale.has(p.unit));
    for (const u of stale) invalidateUnit(u);
    for (const u of units) maybeFinishUnit(u);
  }

  function finishEntry(entry, translated, provider) {
    const produced = provider || activeProvider();
    completeEntry(entry, translated, produced);
    // Store under the provider that actually produced the text: an Edge result
    // from an LLM fallback is an Edge cache entry, never an LLM one.
    cacheSet(entry.text, translated, produced);
    state.stats.translations++;
  }

  function failEntry(entry) {
    entry.state = 4;
    for (const p of entry.parts) p.state = 4;
    entry.parts = [];
    // Forget the failed string so a later unit may retry it.
    if (state.textEntries.get(entry.text) === entry) state.textEntries.delete(entry.text);
  }

  async function runBatch(entries, epoch) {
    const provider = activeProvider();
    const chunkSize = provider === 'llm' ? LLM_MAX_SEGMENTS : 10;
    for (let i = 0; i < entries.length; i += chunkSize) {
      if (!state.enabled || state.epoch !== epoch) return;
      const chunk = entries.slice(i, i + chunkSize);
      if (provider === 'llm' && !state.llmFallback) {
        const result = await translateLlmBatch(chunk, epoch);
        if (!result) return; // disabled or epoch changed
        if (result.texts && result.texts.length === chunk.length) {
          const missing = [];
          for (let j = 0; j < chunk.length; j++) {
            if (result.texts[j] != null) finishEntry(chunk[j], result.texts[j], 'llm');
            else missing.push(chunk[j]);
          }
          if (missing.length) {
            // Valid ids keep their LLM text; the omitted ones go to Edge.
            warnLlmOnce('reply omitted ' + missing.length + ' segment(s)');
            await runEdgeEntries(missing, epoch);
          }
          continue;
        }
        // Network error, non-2xx, unparseable reply or no id matched: this
        // batch goes to Edge and the rest of the session stays there.
        disableLlm(result.why || 'request failed');
        state.stats.batchesFailed++;
        state.stats.errors++;
        await runEdgeEntries(chunk, epoch);
        continue;
      }
      await runEdgeEntries(chunk, epoch);
    }
  }

  async function runEdgeEntries(entries, epoch) {
    if (!entries.length) return;
    const texts = entries.map((e) => e.text);
    let translated = null;
    try {
      translated = await translateEdgeChunk(texts, epoch);
    } catch (_) {
      translated = null;
    }
    if (state.epoch !== epoch) return;
    if (!translated || translated.length !== entries.length) {
      for (const e of entries) failEntry(e);
      state.stats.batchesFailed++;
      state.stats.errors++;
      return;
    }
    for (let j = 0; j < entries.length; j++) finishEntry(entries[j], translated[j], 'edge');
  }

  const EDGE_URL =
    'https://edge.microsoft.com/translate/translatetext?from=&to=';
  const LLM_MAX_SEGMENTS = 12;
  const LLM_MAX_CHARS = 6000;

  async function translateEdgeChunk(texts, epoch) {
    const url = EDGE_URL + encodeURIComponent(state.targetLang) + '&isEnterpriseClient=false';
    let why = '';
    for (let attempt = 0; attempt <= 2; attempt++) {
      if (!state.enabled || state.epoch !== epoch) return null;
      const res = await httpRequest({
        method: 'POST',
        url: url,
        body: JSON.stringify(texts)
      });
      if (!state.enabled || state.epoch !== epoch) return null;
      if (res.error === 0 && res.status === 200) {
        const parsed = parseEdgeResponse(res.body, texts.length);
        if (parsed) return parsed;
        why = 'malformed response';
        problem('edge: malformed response');
        break;
      }
      why = 'status=' + res.status + ' error=' + res.error;
      const retryable =
        res.error !== 0 || res.status === 429 || (res.status >= 500 && res.status <= 599);
      if (!retryable) break;
      if (attempt < 2) await sleep(400 * Math.pow(2, attempt) + Math.random() * 200);
    }
    problem('edge failed: ' + why);
    return null;
  }

  async function translateLlmBatch(entries, epoch) {
    const url = llmUrl();
    if (!url) return { why: 'no LLM endpoint configured', texts: null };
    const body = buildLlmBody(entries);
    let why = '';
    for (let attempt = 0; attempt <= 1; attempt++) {
      if (!state.enabled || state.epoch !== epoch) return null;
      if (state.llmFallback) return { why: 'fallback active', texts: null };
      // No headers here: the page never sees the key; the mod sets
      // Content-Type and Authorization for the llmBaseUrl host
      // (docs/PROTOCOL.md).
      const res = await httpRequest({
        method: 'POST',
        url: url,
        body: body
      });
      if (!state.enabled || state.epoch !== epoch) return null;
      if (res.error === 0 && res.status >= 200 && res.status < 300) {
        const map = parseLlmReply(res.body, entries.length);
        if (map && map.size) {
          const texts = entries.map((entry, index) => (map.has(index + 1) ? map.get(index + 1) : null));
          return { why: '', texts: texts };
        }
        why = map ? 'reply had no usable translations' : 'unparseable reply';
        break;
      }
      why = 'status=' + res.status + ' error=' + res.error;
      // -2 is the page-side timeout; 12002 is ERROR_WINHTTP_TIMEOUT from the
      // mod. Other transport errors fall back without a retry.
      const retryable =
        res.error === -2 || res.error === 12002 ||
        res.status === 429 || (res.status >= 500 && res.status <= 599);
      if (!retryable || attempt >= 1) break;
      await sleep(400 + Math.random() * 200);
    }
    return { why: why, texts: null };
  }

  // The chat reply's message content may be fenced or surrounded by prose;
  // only the first `{` through the last `}` is the JSON object.
  function llmContent(outer) {
    if (!outer || typeof outer !== 'object') return null;
    // Some OpenAI-compatible proxies answer with the object itself.
    if (Array.isArray(outer.translations)) return JSON.stringify(outer);
    const choice = Array.isArray(outer.choices) ? outer.choices[0] : null;
    const message = choice && choice.message ? choice.message : null;
    if (!message) return null;
    if (typeof message.content === 'string') return message.content;
    if (Array.isArray(message.content)) {
      let out = '';
      for (const part of message.content) if (part && typeof part.text === 'string') out += part.text;
      return out;
    }
    return null;
  }

  function parseLlmReply(body, count) {
    let outer = null;
    try {
      outer = JSON.parse(body);
    } catch (_) {
      outer = null;
    }
    // Prefer the chat envelope's message content; a proxy may also answer with
    // the JSON object or a fenced block as the raw body.
    let content = outer ? llmContent(outer) : null;
    if (content == null) content = String(body == null ? '' : body);
    const first = content.indexOf('{');
    const last = content.lastIndexOf('}');
    if (first === -1 || last <= first) return null;
    let data = null;
    try {
      data = JSON.parse(content.slice(first, last + 1));
    } catch (_) {
      return null;
    }
    if (!data || !Array.isArray(data.translations)) return null;
    const map = new Map();
    for (const item of data.translations) {
      if (!item) continue;
      const id = Number(item.id);
      if (!isFinite(id) || id < 1 || id > count) continue;
      if (typeof item.text !== 'string' || !item.text.trim()) continue;
      if (!map.has(id)) map.set(id, item.text);
    }
    return map;
  }

  function parseEdgeResponse(body, count) {
    let data;
    try {
      data = JSON.parse(body);
    } catch (_) {
      return null;
    }
    if (!Array.isArray(data) || data.length < count) return null;
    const out = [];
    for (let i = 0; i < count; i++) {
      const item = data[i];
      const first = item && Array.isArray(item.translations) ? item.translations[0] : null;
      if (!first || typeof first.text !== 'string') return null;
      out.push(first.text);
    }
    return out;
  }

  function maybeFinishUnit(u) {
    if (!u || !u.parts || !u.parts.length) return;
    for (const p of u.parts) if (p.state !== 3) return;
    u.translatedOk = true;
    u.noop = unitOutput(u) === u.source;
    if (u.noop) {
      u.rendered = false;
      return;
    }
    scheduleRender(u);
  }

  // ---------------------------------------------------------------------------
  // Cache (the mod owns the store; the page only looks keys up and stores)
  //
  // Key format:
  //   whtr1|v4|<provider>|<model>|<lang>|<baseUrl>|<glossaryHash>|<hash64>|<length>
  // The endpoint and the effective glossary are part of the identity, so
  // changing either cannot serve a translation produced under the old config.
  // A lookup is one `cache-get` carrying a per-page request id and resolves on
  // a `cache-hit` / `cache-miss` that echoes both that id and the key; a store
  // is one `cache-put` that is never awaited.
  // ---------------------------------------------------------------------------

  const CACHE_VERSION = 'v4';
  const CACHE_PREFIX = 'whtr1|' + CACHE_VERSION + '|';
  const CACHE_LOOKUP_TIMEOUT_MS = 5000;

  // The endpoint is part of the key: two LLM hosts may answer differently.
  function cacheBaseUrl() {
    return String(state.settings.llmBaseUrl || '').trim().replace(/\/+$/, '');
  }

  // A stable fingerprint of the effective glossary, normalised and deduped the
  // same way the prompt builds it.
  function cacheGlossaryHash() {
    const entries = glossaryEntries();
    if (!entries.length) return '';
    return hash32(entries.map(([src, dst]) => src + '=' + dst).join('\n'));
  }

  function cacheKey(text, provider) {
    const p = provider || activeProvider();
    return (
      CACHE_PREFIX + p + '|' + String(state.settings.llmModel || '').trim() + '|' + state.targetLang + '|' +
      cacheBaseUrl() + '|' + cacheGlossaryHash() + '|' + hash64(text) + '|' + text.length
    );
  }

  // Fire-and-forget: the mod owns the store, the page keeps only this session's
  // registry. No reply is expected.
  function cacheSet(text, translated, provider) {
    if (!state.settings.cacheEnabled || state.dead) return;
    if (typeof translated !== 'string') return;
    send(TAG + SEP + 'cache-put' + SEP + cacheKey(text, provider) + SEP + translated);
  }

  function warnCacheOnce() {
    if (state.cacheWarned) return;
    state.cacheWarned = true;
    log(
      'cache: no reply within ' + CACHE_LOOKUP_TIMEOUT_MS / 1000 +
      ' s; the cache is not answering, translating without it'
    );
  }

  // Remove one pending lookup and stop its deadline. Both the id and the key
  // must match: a reply for an id nobody is waiting for, or one whose key
  // disagrees with the record, returns null and is ignored.
  function forgetLookup(id, key) {
    const pending = state.cacheLookups.get(id);
    if (!pending || pending.key !== key) return null;
    state.cacheLookups.delete(id);
    state.cacheLookupKeys.delete(pending.key);
    try {
      clearTimeout(pending.timer);
    } catch (_) {
      /* ignore */
    }
    return pending.entry;
  }

  // Drop every outstanding lookup. With `retry`, entries still waiting are
  // counted as misses and translated normally (a cache clear invalidated the
  // store); otherwise their units are being torn down and nothing is queued.
  function clearCacheLookups(retry) {
    const pendingList = Array.from(state.cacheLookups.values());
    state.cacheLookups.clear();
    state.cacheLookupKeys.clear();
    for (const pending of pendingList) {
      try {
        clearTimeout(pending.timer);
      } catch (_) {
        /* ignore */
      }
      if (retry && !state.dead && pending.entry.state === 0) {
        state.stats.cacheMisses++;
        queueEntry(pending.entry);
      }
    }
  }

  function requestCached(entry) {
    // No channel (or a dead engine): skip the cache and translate normally.
    if (state.dead || !CHANNEL()) {
      queueEntry(entry);
      return;
    }
    const key = cacheKey(entry.text, entry.provider);
    if (state.cacheLookupKeys.has(key)) {
      // A hash collision on a key that is already being looked up: do not
      // overwrite the pending record, just translate this string normally.
      queueEntry(entry);
      return;
    }
    // The id is monotonic for the page and shared with http requests, so no
    // reply can ever be mistaken for a later lookup.
    const id = state.nextRequestId++;
    const timer = setTimeout(() => {
      const pending = state.cacheLookups.get(id);
      if (!pending || pending.entry !== entry) return;
      forgetLookup(id, key);
      if (state.dead || entry.state !== 0) return;
      warnCacheOnce();
      state.stats.cacheMisses++;
      queueEntry(entry);
    }, CACHE_LOOKUP_TIMEOUT_MS);
    state.cacheLookups.set(id, { id: id, key: key, entry: entry, timer: timer });
    state.cacheLookupKeys.add(key);
    if (!send(TAG + SEP + 'cache-get' + SEP + String(id) + SEP + key)) {
      // The channel vanished between the check and the send: no reply will
      // ever come, so translate now.
      forgetLookup(id, key);
      queueEntry(entry);
    }
  }

  function onCacheHit(parts) {
    // parts: [tag, 'cache-hit', requestId, key, text]
    const entry = forgetLookup(Number(parts[2]), parts[3]);
    if (!entry || entry.state !== 0) return;
    state.stats.cacheHits++;
    completeEntry(entry, parts[4] == null ? '' : parts[4], entry.provider);
  }

  function onCacheMiss(parts) {
    // parts: [tag, 'cache-miss', requestId, key]
    const entry = forgetLookup(Number(parts[2]), parts[3]);
    if (!entry || entry.state !== 0) return;
    state.stats.cacheMisses++;
    queueEntry(entry);
  }

  // The engine forgets its in-session map; the mod clears its own store when it
  // applies the clearCache setting. Outstanding lookups were answered from the
  // old store, so they are dropped and their strings translated normally.
  function cacheClear() {
    clearCacheLookups(true);
    for (const [text, entry] of Array.from(state.textEntries)) {
      if (entry.state === 3) state.textEntries.delete(text);
    }
  }

  // ---------------------------------------------------------------------------
  // Rendering: clone mode
  // ---------------------------------------------------------------------------

  function sanitizeClone(el) {
    try {
      if (el.removeAttribute) {
        el.removeAttribute('id');
        el.removeAttribute('data-testid');
        el.removeAttribute(ATTR_UNIT);
        el.removeAttribute(ATTR_CLONE);
        el.removeAttribute(ATTR_ORIG);
        el.removeAttribute(ATTR_SWAPPED);
      }
      let all = [];
      try {
        all = el.querySelectorAll('[id], [data-testid]');
      } catch (_) {
        all = [];
      }
      for (const node of all) {
        node.removeAttribute('id');
        node.removeAttribute('data-testid');
      }
    } catch (_) {
      /* ignore */
    }
  }

  function cloneForDisplay(el) {
    const copy = el.cloneNode(true);
    sanitizeClone(copy);
    return copy;
  }

  function cloneShell(el) {
    const copy = el.cloneNode(false);
    sanitizeClone(copy);
    return copy;
  }

  function buildFragment(src, refs) {
    const frag = document.createDocumentFragment();
    const stack = [{ node: frag, index: -1 }];
    const used = new Set();
    let buf = '';
    let last = 0;
    const flush = () => {
      if (!buf) return;
      const top = stack[stack.length - 1];
      try {
        top.node.appendChild(document.createTextNode(buf));
      } catch (_) {
        /* ignore */
      }
      buf = '';
    };
    const re = /\{\{(\d+)\}\}|<(\/?)(\d+)>/g;
    let m;
    while ((m = re.exec(src))) {
      buf += src.slice(last, m.index);
      last = re.lastIndex;
      if (m[1] !== undefined) {
        const index = Number(m[1]);
        const el = refs[index];
        if (el && !used.has(index)) {
          used.add(index);
          flush();
          try {
            stack[stack.length - 1].node.appendChild(cloneForDisplay(el));
          } catch (_) {
            /* ignore */
          }
        }
        // Unknown / repeated placeholders are dropped, as agreed.
      } else if (m[2] === '/') {
        const index = Number(m[3]);
        let k = stack.length - 1;
        while (k > 0 && stack[k].index !== index) k--;
        if (k > 0) {
          flush();
          stack.length = k;
        }
      } else {
        const index = Number(m[3]);
        const el = refs[index];
        flush();
        if (el && !used.has(index) && stack.length < 200) {
          used.add(index);
          const shell = cloneShell(el);
          try {
            stack[stack.length - 1].node.appendChild(shell);
          } catch (_) {
            /* ignore */
          }
          stack.push({ node: shell, index: index });
        } else {
          // Transparent frame: interior text still lands in the current parent.
          stack.push({ node: stack[stack.length - 1].node, index: -2 });
        }
      }
    }
    buf += src.slice(last);
    flush();
    return frag;
  }

  function buildClone(u) {
    const clone = cloneShell(u.el);
    clone.setAttribute(ATTR_CLONE, '1');
    clone.setAttribute(ATTR_UNIT, String(u.id));
    for (const part of u.parts) {
      const text = partOutput(part);
      if (text == null) continue;
      try {
        clone.appendChild(buildFragment(text, u.refs));
      } catch (e) {
        problem('build clone: ' + (e && e.message ? e.message : e));
      }
    }
    return clone;
  }

  function ensureCloneAttached(u) {
    if (!u.clone) return false;
    const parent = u.el.parentNode;
    if (!parent) return false;
    if (u.clone.parentNode !== parent || u.clone.nextSibling !== u.el) {
      try {
        parent.insertBefore(u.clone, u.el);
      } catch (_) {
        return false;
      }
    }
    state.unitByDisplay.set(u.clone, u);
    return true;
  }

  // ---------------------------------------------------------------------------
  // Rendering: swap mode
  // ---------------------------------------------------------------------------

  function edgeSpaces(original, text) {
    if (!text) return '';
    const lead = /^\s/.test(original);
    const trail = /\s$/.test(original);
    let out = text.trim();
    if (!out) return '';
    if (lead && !/^\s/.test(out)) out = ' ' + out;
    if (trail && !/\s$/.test(out)) out = out + ' ';
    return out;
  }

  function distribute(text, segs) {
    if (!segs || !segs.length) return;
    if (!text) {
      for (const seg of segs) seg.out = '';
      return;
    }
    // Whitespace-only nodes must not receive part of the translation.
    let targets = segs.filter((seg) => /[^\s]/.test(seg.orig || ''));
    if (!targets.length) targets = segs;
    for (const seg of segs) if (targets.indexOf(seg) === -1) seg.out = seg.orig;
    if (targets.length === 1) {
      targets[0].out = edgeSpaces(targets[0].orig, text);
      return;
    }
    let total = 0;
    for (const seg of targets) total += Math.max(1, (seg.orig || '').length);
    const chars = Array.from(text);
    let index = 0;
    let acc = 0;
    for (let i = 0; i < targets.length; i++) {
      const seg = targets[i];
      acc += Math.max(1, (seg.orig || '').length);
      let end = i === targets.length - 1 ? chars.length : Math.round((chars.length * acc) / total);
      if (end < index) end = index;
      const piece = chars.slice(index, end).join('');
      index = end;
      seg.out = edgeSpaces(seg.orig, piece);
    }
  }

  function computeSwapOutputs(u) {
    const source =
      (u.leadTokens || '') + u.parts.map((p) => partOutput(p) || '').join('') + (u.trailTokens || '');
    const groups = [[]];
    for (const seg of u.segs) {
      if (seg.kind === 'token') groups.push([]);
      else groups[groups.length - 1].push(seg);
    }

    const chunks = [];
    let current = '';
    let tokenCount = 0;
    const used = new Set();
    const re = /\{\{(\d+)\}\}/g;
    let last = 0;
    let m;
    while ((m = re.exec(source))) {
      current += source.slice(last, m.index);
      last = re.lastIndex;
      const index = Number(m[1]);
      if (u.refs[index] && !used.has(index)) {
        used.add(index);
        chunks.push(current);
        current = '';
        tokenCount++;
      }
      // A token we cannot restore is dropped from the text stream.
    }
    current += source.slice(last);
    chunks.push(current);

    if (tokenCount === 0) {
      // The translation lost every placeholder: spread it over the whole host.
      const all = [];
      for (const group of groups) for (const seg of group) all.push(seg);
      distribute(chunks[0] || '', all);
      return;
    }

    for (let g = 0; g < groups.length; g++) {
      let text = g < chunks.length ? chunks[g] : '';
      if (g === groups.length - 1 && chunks.length > groups.length) {
        text += chunks.slice(groups.length).join('');
      }
      distribute(text, groups[g]);
    }
  }

  function applySwapText(u) {
    for (const seg of u.segs) {
      if (seg.kind !== 'text') continue;
      const node = seg.node;
      if (!node || !node.isConnected) continue;
      const want = seg.out == null ? seg.orig : seg.out;
      if (node.nodeValue === want) continue;
      // A third value means React re-rendered the node; let the observer rescan.
      if (node.nodeValue !== seg.orig && node.nodeValue !== seg.out) continue;
      node.nodeValue = want;
      state.written.set(node, want);
    }
  }

  function swapOutputsReady(u) {
    return u.parts.length > 0 && u.parts.every((p) => partOutput(p) != null);
  }

  // ---------------------------------------------------------------------------
  // Rendering: shared
  // ---------------------------------------------------------------------------

  function scheduleRender(u) {
    if (!u || state.dead) return;
    state.renderQueue.add(u);
    if (state.renderScheduled) return;
    state.renderScheduled = true;
    idle(() => {
      state.renderScheduled = false;
      safe(runRenderChunk, 'render-chunk');
    });
  }

  function runRenderChunk(deadline) {
    let n = 0;
    for (const u of Array.from(state.renderQueue)) {
      state.renderQueue.delete(u);
      if (u.el && u.el.isConnected) safe(() => renderUnit(u), 'render-unit');
      n++;
      const left = deadline && typeof deadline.timeRemaining === 'function' ? deadline.timeRemaining() : 8;
      if (n >= 30 || left < 2) break;
    }
    if (state.renderQueue.size) scheduleRenderIdle();
  }

  function scheduleRenderIdle() {
    if (state.renderScheduled || state.dead) return;
    state.renderScheduled = true;
    idle(() => {
      state.renderScheduled = false;
      safe(runRenderChunk, 'render-chunk2');
    });
  }

  function renderUnit(u) {
    if (!state.enabled || !u || !u.el || !u.el.isConnected) return;
    if (!u.translatedOk || u.noop || u.skip) return;
    if (u.mode === 'clone') {
      if (!u.clone) {
        u.clone = buildClone(u);
        state.unitByDisplay.set(u.clone, u);
        state.unitByDisplay.set(u.el, u);
      }
      if (!ensureCloneAttached(u)) return;
      u.rendered = true;
      applyUnit(u, true);
      return;
    }
    if (!swapOutputsReady(u)) return;
    if (swapChanged(u)) {
      // React rewrote the host while the request was in flight; rescan instead.
      scheduleScan(120);
      return;
    }
    computeSwapOutputs(u);
    u.rendered = true;
    state.unitByDisplay.set(u.el, u);
    applyUnit(u, true);
  }

  function ensureUnitRendered(u, force) {
    if (!u || !u.el || !u.el.isConnected) return;
    if (u.mode === 'clone') {
      if (u.rendered && u.clone && u.clone.isConnected) {
        ensureCloneAttached(u);
        applyUnit(u, !!force);
      } else if (u.translatedOk && !u.noop) {
        scheduleRender(u);
      }
      return;
    }
    if (u.rendered) applyUnit(u, !!force);
  }

  function effectiveShowOrig(u) {
    return !state.enabled || u.hoverOrig || u.tapOrig;
  }

  function applyUnit(u, force) {
    if (!u || !u.el || !u.el.isConnected) return;
    const showOrig = effectiveShowOrig(u);
    if (!force && u.appliedShowOrig === showOrig) return;
    u.appliedShowOrig = showOrig;

    if (u.mode === 'clone') {
      if (!u.clone) return;
      try {
        u.clone.style.display = showOrig ? 'none' : '';
        if (showOrig) u.el.removeAttribute(ATTR_ORIG);
        else u.el.setAttribute(ATTR_ORIG, '1');
      } catch (_) {
        /* ignore */
      }
      return;
    }

    if (!u.rendered) return;
    if (showOrig) {
      for (const seg of u.segs) {
        if (seg.kind !== 'text') continue;
        const node = seg.node;
        if (!node || !node.isConnected) continue;
        const orig = state.origText.has(node) ? state.origText.get(node) : seg.orig;
        if (node.nodeValue === seg.out && node.nodeValue !== orig) {
          node.nodeValue = orig;
          state.written.set(node, orig);
        }
      }
      try {
        u.el.removeAttribute(ATTR_SWAPPED);
      } catch (_) {
        /* ignore */
      }
    } else {
      if (swapChanged(u)) {
        scheduleScan(120);
        return;
      }
      applySwapText(u);
      try {
        u.el.setAttribute(ATTR_SWAPPED, '1');
      } catch (_) {
        /* ignore */
      }
    }
  }

  // ---------------------------------------------------------------------------
  // Pointer / touch interaction (delegated, one listener each)
  // ---------------------------------------------------------------------------

  function unitFromEvent(e) {
    let node = e && e.target;
    if (!node || node.nodeType !== 1) {
      node = node && node.parentElement ? node.parentElement : null;
    }
    while (node) {
      const u = state.unitByDisplay.get(node);
      if (u) return u;
      node = node.parentElement;
    }
    return null;
  }

  function withinUnit(u, node) {
    if (!u || !node || node.nodeType !== 1) return false;
    try {
      if (u.el && u.el.contains(node)) return true;
      if (u.clone && u.clone.contains(node)) return true;
    } catch (_) {
      /* ignore */
    }
    return false;
  }

  function onPointerOver(e) {
    if (!state.enabled) return;
    if (e.pointerType === 'touch') return;
    const u = unitFromEvent(e);
    if (!u) return;
    if (withinUnit(u, e.relatedTarget)) return;
    if (u.leaveTimer) {
      clearTimeout(u.leaveTimer);
      u.leaveTimer = 0;
    }
    if (u.hoverTimer) clearTimeout(u.hoverTimer);
    const delay = clamp(state.settings.hoverDelayMs, 0, 2000);
    if (delay <= 0) {
      u.hoverOrig = true;
      applyUnit(u);
      return;
    }
    u.hoverTimer = setTimeout(() => {
      u.hoverTimer = 0;
      safe(() => {
        u.hoverOrig = true;
        applyUnit(u);
      }, 'hover');
    }, delay);
  }

  function onPointerOut(e) {
    if (!state.enabled) return;
    if (e.pointerType === 'touch') return;
    const u = unitFromEvent(e);
    if (!u) return;
    if (withinUnit(u, e.relatedTarget)) return;
    if (u.hoverTimer) {
      clearTimeout(u.hoverTimer);
      u.hoverTimer = 0;
    }
    if (!u.hoverOrig) return;
    if (u.leaveTimer) clearTimeout(u.leaveTimer);
    // Short grace: hiding the clone must not immediately restart the hover loop.
    const grace = Math.max(120, clamp(state.settings.hoverDelayMs, 0, 2000));
    u.leaveTimer = setTimeout(() => {
      u.leaveTimer = 0;
      safe(() => {
        u.hoverOrig = false;
        applyUnit(u);
      }, 'hover-out');
    }, grace);
  }

  // ---------------------------------------------------------------------------
  // Touch: per-block single tap and the two-finger double tap.
  //
  // The single tap is deferred briefly so a second finger can cancel it. Two
  // pointers down and both lifted, twice within 400 ms, toggles translation
  // globally like the keyboard shortcut. Each pointer may wander at most 30 px.
  // ---------------------------------------------------------------------------

  const GESTURE_WINDOW_MS = 400;
  const GESTURE_SLOP_PX = 30;
  const TAP_SLOP_PX = 10;
  const SINGLE_TAP_DELAY_MS = 160;

  let touchStart = null;
  let pendingTapTimer = 0;
  const touchPointers = new Map(); // pointerId -> { x, y, at, moved, tapMoved }
  let twoFingerTap = null; // { ids: [id, id], at, ok }
  let lastTwoFingerTapAt = 0;

  function cancelPendingTap() {
    if (!pendingTapTimer) return;
    clearTimeout(pendingTapTimer);
    pendingTapTimer = 0;
  }

  function toggleTapOrig(u) {
    if (!state.enabled || !u || !u.el || !u.el.isConnected) return;
    u.tapOrig = !u.tapOrig;
    if (u.hoverTimer) {
      clearTimeout(u.hoverTimer);
      u.hoverTimer = 0;
    }
    u.hoverOrig = false;
    if (u.leaveTimer) {
      clearTimeout(u.leaveTimer);
      u.leaveTimer = 0;
    }
    applyUnit(u, true);
  }

  function queueSingleTap(u) {
    cancelPendingTap();
    pendingTapTimer = setTimeout(() => {
      pendingTapTimer = 0;
      safe(() => toggleTapOrig(u), 'tap');
    }, SINGLE_TAP_DELAY_MS);
  }

  function pointerDistance(start, e) {
    const dx = Number(e.clientX) - start.x;
    const dy = Number(e.clientY) - start.y;
    return Math.sqrt(dx * dx + dy * dy);
  }

  function onPointerDown(e) {
    if (e.pointerType !== 'touch') return;
    // Gesture tracking runs whether or not translation is enabled; only the
    // per-block tap is gated on it. Any new touch cancels a queued tap.
    cancelPendingTap();
    if (isTypingTarget(e.target)) return;
    const now = Date.now();
    touchPointers.set(e.pointerId, {
      x: Number(e.clientX),
      y: Number(e.clientY),
      at: now,
      moved: false,
      tapMoved: false
    });
    if (touchPointers.size === 1) {
      touchStart = { x: Number(e.clientX), y: Number(e.clientY), t: now, id: e.pointerId };
      twoFingerTap = null;
      return;
    }
    // A second finger cancels the block tap the first one may have queued.
    touchStart = null;
    if (touchPointers.size === 2) {
      let at = now;
      for (const p of touchPointers.values()) at = Math.min(at, p.at);
      twoFingerTap = { ids: Array.from(touchPointers.keys()), at: at, ok: true };
    } else if (twoFingerTap) {
      twoFingerTap.ok = false;
    }
  }

  function onPointerMove(e) {
    if (e.pointerType !== 'touch') return;
    const start = touchPointers.get(e.pointerId);
    if (!start) return;
    const dist = pointerDistance(start, e);
    if (dist > TAP_SLOP_PX) start.tapMoved = true;
    if (start.moved || dist <= GESTURE_SLOP_PX) return;
    start.moved = true;
    if (twoFingerTap && twoFingerTap.ids.indexOf(e.pointerId) !== -1) twoFingerTap.ok = false;
  }

  function onPointerUp(e) {
    if (e.pointerType !== 'touch') return;
    const start = touchPointers.get(e.pointerId);

    if (!start) {
      // Synthetic pointerup without a matching pointerdown: still a tap.
      if (isTypingTarget(e.target)) return;
      if (!state.enabled) return;
      const u = unitFromEvent(e);
      if (u) queueSingleTap(u);
      return;
    }

    touchPointers.delete(e.pointerId);
    const now = Date.now();
    const dist = pointerDistance(start, e);
    const moved = start.moved || dist > GESTURE_SLOP_PX;
    const tapMoved = start.tapMoved || dist > TAP_SLOP_PX;
    if (twoFingerTap && twoFingerTap.ids.indexOf(e.pointerId) !== -1 && moved) twoFingerTap.ok = false;

    if (twoFingerTap) {
      // A two-finger attempt never taps a single block.
      if (touchPointers.size === 0) {
        const tap = twoFingerTap;
        twoFingerTap = null;
        const ok = tap.ok && now - tap.at <= GESTURE_WINDOW_MS;
        if (!ok) return;
        const on = !!state.settings.gestureToggle;
        if (on && lastTwoFingerTapAt && now - lastTwoFingerTapAt <= GESTURE_WINDOW_MS) {
          lastTwoFingerTapAt = 0;
          setEnabled(!state.enabled);
        } else if (on) {
          lastTwoFingerTapAt = now;
        }
      }
      return;
    }

    if (touchPointers.size > 0) return; // another finger is still down
    if (touchStart && touchStart.id === e.pointerId) {
      const elapsed = now - touchStart.t;
      touchStart = null;
      if (tapMoved || elapsed > 900) return;
    } else {
      touchStart = null;
    }
    if (!state.enabled) return;
    const u = unitFromEvent(e);
    if (u) queueSingleTap(u);
  }

  function onPointerCancel(e) {
    const id = e && e.pointerId;
    if (id == null) touchPointers.clear();
    else touchPointers.delete(id);
    touchStart = null;
    if (touchPointers.size === 0) twoFingerTap = null;
    else if (twoFingerTap) twoFingerTap.ok = false;
    cancelPendingTap();
  }

  // Called on every enable/disable: a pointer or half-finished gesture from
  // before the switch must not be paired with the next one.
  function resetPointerState() {
    touchPointers.clear();
    touchStart = null;
    twoFingerTap = null;
    lastTwoFingerTapAt = 0;
    cancelPendingTap();
  }

  // ---------------------------------------------------------------------------
  // Mutation observer
  // ---------------------------------------------------------------------------

  function isOurNode(node) {
    if (!node) return false;
    if (node.nodeType === 1) {
      try {
        return !!node.closest('[' + ATTR_CLONE + '="1"]');
      } catch (_) {
        return false;
      }
    }
    if (node.nodeType === 3) {
      const parent = node.parentElement;
      if (!parent) return false;
      try {
        return !!parent.closest('[' + ATTR_CLONE + '="1"]');
      } catch (_) {
        return false;
      }
    }
    return false;
  }

  function isOurMutation(record) {
    if (record.type === 'characterData') {
      const target = record.target;
      return state.written.get(target) === target.nodeValue;
    }
    const nodes = [];
    try {
      for (const n of record.addedNodes) nodes.push(n);
      for (const n of record.removedNodes) nodes.push(n);
    } catch (_) {
      return false;
    }
    if (!nodes.length) return true;
    for (const node of nodes) if (!isOurNode(node)) return false;
    return true;
  }

  function onMutations(records) {
    if (!state.enabled || !state.settingsSeen) return;
    for (const record of records) {
      if (!isOurMutation(record)) {
        // A settings tab can mount in one React commit; scan on the next task.
        scheduleScan(0);
        return;
      }
    }
  }

  function startObserver() {
    if (state.observer || typeof MutationObserver !== 'function') return;
    try {
      const target = document.documentElement || document;
      const observer = new MutationObserver((records) => {
        safe(() => onMutations(records), 'observer');
      });
      observer.observe(target, { childList: true, subtree: true, characterData: true });
      state.observer = observer;
    } catch (e) {
      problem('observer unavailable: ' + (e && e.message ? e.message : e));
    }
  }

  // ---------------------------------------------------------------------------
  // Settings and lifecycle
  // ---------------------------------------------------------------------------

  function normalizeLang(raw) {
    let lang = String(raw == null ? '' : raw).trim().replace(/_/g, '-');
    if (!lang) return '';
    const low = lang.toLowerCase();
    if (low === 'zh' || low.indexOf('zh-') === 0) {
      return /hant|tw|hk|mo/.test(low) ? 'zh-Hant' : 'zh-Hans';
    }
    return low.split('-')[0];
  }

  function resolveTarget(raw) {
    if (raw && String(raw).toLowerCase() !== 'auto') return normalizeLang(raw);
    let lang = '';
    try {
      lang = document.documentElement ? document.documentElement.lang : '';
    } catch (_) {
      lang = '';
    }
    if (!lang) {
      try {
        lang = navigator.language || '';
      } catch (_) {
        lang = '';
      }
    }
    return normalizeLang(lang);
  }

  function resolveProvider() {
    const p = String(state.settings.provider || 'auto').toLowerCase();
    if (p === 'google') {
      if (!state.googleWarned) {
        state.googleWarned = true;
        log('provider "google" is not implemented; using Edge');
      }
      return 'edge';
    }
    if (p === 'llm') return 'llm';
    if (p === 'auto') {
      const keySource = String(state.settings.llmKeySource || 'none').toLowerCase();
      return keySource !== 'none' && llmConfigured() ? 'llm' : 'edge';
    }
    return 'edge';
  }

  function rebuildKeysChanged(prev, next) {
    return (
      prev.targetLang !== next.targetLang ||
      prev.provider !== next.provider ||
      prev.llmBaseUrl !== next.llmBaseUrl ||
      prev.llmModel !== next.llmModel ||
      prev.llmKeySource !== next.llmKeySource ||
      glossarySignature(prev) !== glossarySignature(next) ||
      prev.scopeReadme !== next.scopeReadme ||
      prev.scopeSettings !== next.scopeSettings ||
      prev.scopeModName !== next.scopeModName ||
      prev.scopeModSummary !== next.scopeModSummary ||
      prev.scopeChangelog !== next.scopeChangelog
    );
  }

  // The first protocol revision had a single `scopeList`; a payload without the
  // split keys keeps its choice, and no key at all means both are on.
  function applyScopeSettings(raw, next) {
    const has = (key) => Object.prototype.hasOwnProperty.call(raw, key);
    const legacy = has('scopeList') ? !!raw.scopeList : undefined;
    const fallback = legacy === undefined ? true : legacy;
    if (!has('scopeModName')) next.scopeModName = fallback;
    if (!has('scopeModSummary')) next.scopeModSummary = fallback;
    next.scopeModName = next.scopeModName !== false;
    next.scopeModSummary = next.scopeModSummary !== false;
    delete next.scopeList;
  }

  function applySettings(raw) {
    if (!raw || typeof raw !== 'object') return;
    const prev = state.settings;
    const next = Object.assign(defaultSettings(), prev, raw);
    applyScopeSettings(raw, next);
    next.hoverDelayMs = clamp(next.hoverDelayMs, 0, 2000);
    next.batchSize = clamp(next.batchSize, 1, 100);
    next.batchChars = clamp(next.batchChars, 200, 200000);
    next.batchIntervalMs = clamp(next.batchIntervalMs, 0, 10000);
    next.concurrency = clamp(next.concurrency, 1, 8);
    next.timeoutMs = clamp(next.timeoutMs, 1000, 120000);
    next.cacheTtlDays = clamp(next.cacheTtlDays, 0, 3650);
    next.gestureToggle = next.gestureToggle !== false;
    if (!next.gestureToggle) lastTwoFingerTapAt = 0;

    state.settings = next;
    state.settingsSeen = true;
    state.settingsAttempts = 0;

    state.targetLang = resolveTarget(next.targetLang);
    const prevProvider = state.provider;
    const provider = resolveProvider();
    if (prevProvider !== provider) {
      // A new provider choice is a fresh start; a previous LLM failure must not
      // pin this configuration to Edge.
      state.llmFallback = false;
      state.llmWarned = false;
    }
    state.provider = provider;
    state.shortcuts.toggle = parseShortcut(next.shortcutToggle);

    if (next.clearCache) {
      // The mod clears its own store; the page forgets this session's map.
      cacheClear();
      next.clearCache = false;
    }

    const isFirst = state.settingsCount === 0;
    state.settingsCount++;
    const changed = isFirst || prevProvider !== provider || rebuildKeysChanged(prev, next);

    if (!state.targetLang) problem('no target language resolved; idle');

    if (changed) {
      state.epoch++;
      teardownAll();
      scheduleScan(0);
    } else {
      // Re-apply hover state (delay may have changed); rendered units stay as they are.
      for (const u of state.units) applyUnit(u, true);
    }
  }

  function setEnabled(on) {
    if (state.dead) return;
    on = !!on;
    resetPointerState();
    if (state.enabled === on) {
      if (on) {
        if (!state.settingsSeen) requestSettings();
        else scheduleScan(0);
      }
      return;
    }
    state.enabled = on;
    state.epoch++;
    teardownAll();
    if (on) {
      // Recovery from any state: without settings there is nothing to scan.
      if (!state.settingsSeen) {
        if (!state.connected) connect();
        requestSettings();
      } else {
        scheduleScan(0);
      }
    }
  }

  function onMessage(e) {
    if (state.dead) return;
    try {
      const data = e && e.data;
      if (typeof data !== 'string' || data.indexOf(TAG + SEP) !== 0) return;
      if (data === TAG + SEP + 'teardown') {
        teardown();
        return;
      }
      if (data.indexOf(TAG + SEP + 'http' + SEP) === 0) {
        onHttpReply(splitFields(data, 6));
        return;
      }
      if (data.indexOf(TAG + SEP + 'settings' + SEP) === 0) {
        const parts = splitFields(data, 3);
        let json = null;
        try {
          json = JSON.parse(parts[2]);
        } catch (_) {
          json = null;
        }
        if (json && typeof json === 'object') applySettings(json);
        return;
      }
      if (data.indexOf(TAG + SEP + 'cache-hit' + SEP) === 0) {
        onCacheHit(splitFields(data, 5));
        return;
      }
      if (data.indexOf(TAG + SEP + 'cache-miss' + SEP) === 0) {
        onCacheMiss(splitFields(data, 4));
      }
    } catch (err) {
      problem('message: ' + (err && err.message ? err.message : err));
    }
  }

  function requestSettings() {
    if (state.dead) return;
    state.settingsAttempts++;
    send(TAG + SEP + 'settings?');
    if (!state.settingsSeen && state.settingsAttempts < 3) {
      state.settingsTimer = setTimeout(() => {
        state.settingsTimer = 0;
        if (!state.settingsSeen) safe(requestSettings, 'settings-retry');
      }, 1500 * state.settingsAttempts);
    }
  }

  function connect() {
    if (state.dead) return false;
    const wv = CHANNEL();
    if (!wv) return false;
    if (!state.connected) {
      try {
        const listener = (e) => safe(() => onMessage(e), 'message');
        wv.addEventListener('message', listener);
        state.messageListener = listener;
        state.connected = true;
      } catch (e) {
        problem('message listener: ' + (e && e.message ? e.message : e));
        return false;
      }
      requestSettings();
      scheduleScan(250);
    }
    return true;
  }

  function installCss() {
    try {
      if (document.getElementById('whtr-style')) return;
      const style = document.createElement('style');
      style.id = 'whtr-style';
      style.textContent =
        '[' + ATTR_ORIG + '="1"]{display:none !important}' +
        '[' + ATTR_CLONE + '="1"]{box-shadow:inset 2px 0 0 rgba(99,150,255,.35)}';
      const parent = document.head || document.documentElement;
      if (parent) parent.appendChild(style);
    } catch (_) {
      /* CSP should allow inline styles, but stay quiet if not */
    }
  }

  const inputHandlers = [];

  function listen(target, type, fn, options) {
    const wrapped = (e) => safe(() => fn(e), type);
    try {
      target.addEventListener(type, wrapped, options);
      inputHandlers.push([target, type, wrapped, options]);
    } catch (e) {
      problem('listeners: ' + (e && e.message ? e.message : e));
    }
  }

  function removeInputHandlers() {
    for (const [target, type, fn, options] of inputHandlers) {
      try {
        target.removeEventListener(type, fn, options);
      } catch (_) {
        /* ignore */
      }
    }
    inputHandlers.length = 0;
  }

  function installInputHandlers() {
    listen(document, 'pointerover', onPointerOver, true);
    listen(document, 'pointerout', onPointerOut, true);
    listen(document, 'pointerdown', onPointerDown, true);
    listen(document, 'pointermove', onPointerMove, true);
    listen(document, 'pointerup', onPointerUp, true);
    listen(document, 'pointercancel', onPointerCancel, true);
    listen(window, 'keydown', onKeyDown, false);
    listen(window, 'load', () => {
      if (state.settingsSeen) scheduleScan(200);
    }, false);
  }

  function detachChannel() {
    const wv = CHANNEL();
    if (!wv || !state.messageListener) return;
    try {
      wv.removeEventListener('message', state.messageListener);
    } catch (_) {
      /* ignore */
    }
    state.messageListener = null;
  }

  // `teardown`: the mod is unloading. Run the disable path synchronously and
  // never talk to the mod again. No reply.
  function teardown() {
    if (state.dead) return;
    state.dead = true;
    state.enabled = false;
    state.epoch++;
    try {
      if (state.observer) state.observer.disconnect();
    } catch (_) {
      /* ignore */
    }
    state.observer = null;
    detachChannel();
    removeInputHandlers();
    resetPointerState();
    state.scanQueue = null;
    state.scanScheduled = false;
    state.renderScheduled = false;
    if (state.scanTimer) {
      clearTimeout(state.scanTimer);
      state.scanTimer = 0;
    }
    if (state.settingsTimer) {
      clearTimeout(state.settingsTimer);
      state.settingsTimer = 0;
    }
    for (const entry of Array.from(state.pending.values())) {
      try {
        clearTimeout(entry.timer);
      } catch (_) {
        /* ignore */
      }
      try {
        entry.resolve({ status: 0, body: '', error: -1 });
      } catch (_) {
        /* ignore */
      }
    }
    state.pending.clear();
    clearCacheLookups();
    teardownAll();
    try {
      const style = document.getElementById('whtr-style');
      if (style && style.parentNode) style.parentNode.removeChild(style);
    } catch (_) {
      /* ignore */
    }
    // A later injection in the same document must build a fresh engine, not
    // revive this dead one.
    try {
      if (window.__WHTR__ === publicApi) delete window.__WHTR__;
    } catch (_) {
      try {
        window.__WHTR__ = undefined;
      } catch (_) {
        /* ignore */
      }
    }
  }

  function revivify() {
    if (state.dead) return;
    if (!state.connected) connect();
    if (!state.settingsSeen) requestSettings();
    if (state.enabled && state.settingsSeen) scheduleScan(0);
  }

  // ---------------------------------------------------------------------------
  // Boot
  // ---------------------------------------------------------------------------

  try {
    const prev = window.__WHTR__;
    const engine = prev && prev.__engine;
    if (engine && typeof engine.revive === 'function') {
      // A leftover handle from a dead engine is replaced; only a live engine
      // makes this the navigation-completed re-injection.
      let alive = true;
      if (typeof engine.alive === 'function') {
        try {
          alive = !!engine.alive();
        } catch (_) {
          alive = false;
        }
      }
      if (alive) {
        try {
          engine.revive();
        } catch (_) {
          /* ignore */
        }
        return;
      }
    }
  } catch (_) {
    /* ignore */
  }

  try {
    installCss();
    startObserver();
    installInputHandlers();

    const deadline = Date.now() + 5000;
    const boot = () => {
      if (state.dead) return;
      if (connect()) return;
      if (Date.now() < deadline) setTimeout(() => safe(boot, 'boot'), 50);
    };
    boot();

    publicApi = {
      __engine: { revive: revivify, alive: () => !state.dead },
      version: VERSION,
      enable: () => safe(() => setEnabled(true), 'api.enable'),
      disable: () => safe(() => setEnabled(false), 'api.disable'),
      stats: () => safe(snapshot, 'api.stats') || {},
      rescan: () => safe(() => scheduleScan(0), 'api.rescan')
    };
    window.__WHTR__ = publicApi;
  } catch (e) {
    try {
      problem('boot: ' + (e && e.message ? e.message : e));
    } catch (_) {
      /* ignore */
    }
  }
})();
)WHTR_JS";

// ---------------------------------------------------------------------------
// Types
// ---------------------------------------------------------------------------

// ABI-compatible with WebView2's EventRegistrationToken (a single 64-bit
// value passed by value on x86-64).
struct EventRegistrationToken {
    LONGLONG value;
};
static_assert(sizeof(EventRegistrationToken) == sizeof(LONGLONG));

using FnQueryInterface = HRESULT(STDMETHODCALLTYPE*)(void*, const IID&, void**);
using FnAddRef = ULONG(STDMETHODCALLTYPE*)(void*);
using FnRelease = ULONG(STDMETHODCALLTYPE*)(void*);

using FnEnvCreateController =
    HRESULT(STDMETHODCALLTYPE*)(void*, HWND, void*);
using FnEnvCreateControllerWithOptions =
    HRESULT(STDMETHODCALLTYPE*)(void*, HWND, void*, void*);
using FnEnvHandlerInvoke = HRESULT(STDMETHODCALLTYPE*)(void*, HRESULT, void*);

using FnControllerGetCoreWebView2 =
    HRESULT(STDMETHODCALLTYPE*)(void*, void**);
using FnControllerHandlerInvoke =
    HRESULT(STDMETHODCALLTYPE*)(void*, HRESULT, void*);

using FnGetSettings = HRESULT(STDMETHODCALLTYPE*)(void*, void**);
using FnGetBool = HRESULT(STDMETHODCALLTYPE*)(void*, BOOL*);
using FnPutBool = HRESULT(STDMETHODCALLTYPE*)(void*, BOOL);
using FnAddScript =
    HRESULT(STDMETHODCALLTYPE*)(void*, LPCWSTR, void*);
using FnRemoveScript =
    HRESULT(STDMETHODCALLTYPE*)(void*, LPCWSTR);
using FnExecuteScript =
    HRESULT(STDMETHODCALLTYPE*)(void*, LPCWSTR, void*);
using FnPostWebMessageAsString =
    HRESULT(STDMETHODCALLTYPE*)(void*, LPCWSTR);
using FnAddWebMessageReceived =
    HRESULT(STDMETHODCALLTYPE*)(void*, void*, EventRegistrationToken*);
using FnRemoveWebMessageReceived =
    HRESULT(STDMETHODCALLTYPE*)(void*, EventRegistrationToken);
using FnTryGetWebMessageAsString =
    HRESULT(STDMETHODCALLTYPE*)(void*, LPWSTR*);
using FnScriptHandlerInvoke =
    HRESULT(STDMETHODCALLTYPE*)(void*, HRESULT, LPCWSTR);

// Navigation starting/completed events and their args. The event handler
// signature is Invoke(ICoreWebView2* sender, <args>* args).
using FnGetString = HRESULT(STDMETHODCALLTYPE*)(void*, LPWSTR*);
using FnGetInt32 = HRESULT(STDMETHODCALLTYPE*)(void*, int*);
using FnGetUInt64 = HRESULT(STDMETHODCALLTYPE*)(void*, UINT64*);
using FnAddNavigationEventHandler =
    HRESULT(STDMETHODCALLTYPE*)(void*, void*, EventRegistrationToken*);
using FnRemoveNavigationEventHandler =
    HRESULT(STDMETHODCALLTYPE*)(void*, EventRegistrationToken);
using FnNavigationEventHandlerInvoke =
    HRESULT(STDMETHODCALLTYPE*)(void*, void*, void*);

using FnCreateEnv = HRESULT(STDMETHODCALLTYPE*)(
    BOOL fromClientDll,
    int runtimeType,
    PCWSTR userDataFolder,
    IUnknown* environmentOptions,
    void* handler);

using FnGetProcAddress = FARPROC(WINAPI*)(HMODULE, LPCSTR);

// ---------------------------------------------------------------------------
// Module / vtable helpers
// ---------------------------------------------------------------------------

static std::atomic<HMODULE> g_embeddedBrowserModule{nullptr};

static HMODULE P0GetEmbeddedBrowserModule() {
    HMODULE module = g_embeddedBrowserModule.load(std::memory_order_acquire);
    if (module) {
        return module;
    }

    // Cheap atomic load on the hot path; the API call only happens until the
    // DLL is loaded for the first time.
    module = GetModuleHandleW(L"EmbeddedBrowserWebView.dll");
    if (module) {
        g_embeddedBrowserModule.store(module, std::memory_order_release);
    }
    return module;
}

// Checks that a function pointer belongs to EmbeddedBrowserWebView.dll. This
// is deliberately stricter than "is executable": the WebView2 COM objects are
// implemented by that module, so every slot we call must live there.
static bool P0IsModuleCode(const void* code) {
    if (!code) {
        return false;
    }

    HMODULE module = P0GetEmbeddedBrowserModule();
    if (!module) {
        return false;
    }

    MEMORY_BASIC_INFORMATION mbi{};
    if (!VirtualQuery(code, &mbi, sizeof(mbi))) {
        return false;
    }

    return mbi.AllocationBase == module;
}

static void* P0GetRawSlot(void* object, size_t index) {
    if (!object) {
        return nullptr;
    }
    void** vtbl = *reinterpret_cast<void***>(object);
    return vtbl ? vtbl[index] : nullptr;
}

static bool P0GetValidatedSlot(void* object, size_t index, void** out) {
    if (!object || !out) {
        return false;
    }
    void* fn = P0GetRawSlot(object, index);
    if (!P0IsModuleCode(fn)) {
        return false;
    }
    *out = fn;
    return true;
}

// Copying from an untrusted-looking vtable pointer must never fault. This
// helper intentionally contains no C++ objects so SEH can be used.
static bool P0CopyVtable(void** dst, void** src, size_t count) {
    __try {
        for (size_t i = 0; i < count; ++i) {
            dst[i] = src[i];
        }
        return true;
    } __except (EXCEPTION_EXECUTE_HANDLER) {
        return false;
    }
}

static HRESULT P0SafeForwardEnvHandler(void* app, HRESULT hr, void* env) {
    if (!app) {
        return S_OK;
    }
    __try {
        auto fn =
            reinterpret_cast<FnEnvHandlerInvoke>(P0GetRawSlot(app, kCallbackSlotInvoke));
        if (!fn) {
            return E_FAIL;
        }
        return fn(app, hr, env);
    } __except (EXCEPTION_EXECUTE_HANDLER) {
        return E_FAIL;
    }
}

static HRESULT P0SafeForwardControllerHandler(void* app,
                                              HRESULT hr,
                                              void* controller) {
    if (!app) {
        return S_OK;
    }
    __try {
        auto fn = reinterpret_cast<FnControllerHandlerInvoke>(
            P0GetRawSlot(app, kCallbackSlotInvoke));
        if (!fn) {
            return E_FAIL;
        }
        return fn(app, hr, controller);
    } __except (EXCEPTION_EXECUTE_HANDLER) {
        return E_FAIL;
    }
}

// ---------------------------------------------------------------------------
// Mod lifecycle
//
// One atomic state drives every entry point: Init -> Running -> Stopping ->
// Stopped. No entry point does any work unless the state is Running (the
// teardown path itself is the only exception, and it is driven by the UI
// thread). The state is set to Stopping as the very first thing
// Wh_ModBeforeUninit does, so no new work can be admitted while the pool, the
// cache saver and the WebView2 objects are being torn down.
// ---------------------------------------------------------------------------

enum class P1Lifecycle : int {
    Init = 0,
    Running,
    Stopping,
    Stopped,
};

static std::atomic<P1Lifecycle> g_p1Lifecycle{P1Lifecycle::Init};

// Thread id of the WebView2 UI thread that created the dispatch window and
// owns every wrapper handed to WebView2. Set by P1NoteUiThread before the first
// wrapper is handed over and never cleared; zero until then.
static std::atomic<DWORD> g_p1UiThreadId{0};

static bool P1IsRunning() {
    return g_p1Lifecycle.load(std::memory_order_acquire) ==
           P1Lifecycle::Running;
}

// Number of COM callback objects handed to WebView2 that have not reached
// their final Release yet. Every wrapper passed to a WebView2 method is
// counted before the call; P0WrapperRelease decrements it after the finalizer
// ran.
//
// Number of webview references held by AddScript completion handlers. The
// reference keeps the webview alive until the completion arrives, so a late
// completion can still remove the document-created script. P0ReleaseScriptWebView
// transfers the reference to the UI-thread release queue and decrements it.
//
// Both counters share one lock. The engine thread waits for them to reach zero
// after it asked the UI thread for the teardown: that is the point where no
// wrapper and no held AddScript reference can call into this image any more.
// WebView2 hands these objects back, and runs the completions that decrement
// them, on the UI thread that created them; every decrement latches a release
// seen on any other thread, and P1MaybeSignalTeardownLocked refuses to treat a
// zero as the end of the teardown once that latch is set.
static SRWLOCK g_p0OutstandingLock = SRWLOCK_INIT;
static long g_p0OutstandingCallbacks = 0;      // g_p0OutstandingLock
static long g_p0OutstandingAddScriptRefs = 0;  // g_p0OutstandingLock

// Manual-reset event, created in Wh_ModInit. Signalled once the teardown was
// requested, the UI-thread handler completed, and both counters reached zero
// on the UI thread; Wh_ModBeforeUninit waits on it before it asks the UI thread
// to destroy the dispatch window.
static HANDLE g_p1TeardownEvent = nullptr;
static std::atomic<bool> g_p1TeardownRequested{false};

// Re-entry guard for the UI-thread teardown handler. It is never cleared, so a
// duplicate posted request cannot start a second teardown. It does not gate the
// event; g_p1TeardownCompleted does.
static std::atomic<bool> g_p1TeardownInProgress{false};

// Set by P1RunTeardownOnUiThread only after the whole cleanup, including
// P0RestoreEnvironment(true), returned inside its try block. A release that
// empties the counters before that point cannot signal the event early, and an
// exception leaves this false so the engine thread keeps waiting.
static std::atomic<bool> g_p1TeardownCompleted{false};

// Sticky: a wrapper was released on a thread that is not the WebView2 UI
// thread. That frame cannot be proven to have returned, so the unload never
// proceeds; the flag also makes the DEFECT log once. The engine thread then
// waits, which is the accepted failure mode.
static std::atomic<bool> g_p1ReleaseOffThread{false};

// Must be called with g_p0OutstandingLock held.
static void P1MaybeSignalTeardownLocked() {
    // Only a zero reached after the unload asked for the teardown says
    // anything about the teardown.
    if (!g_p1TeardownRequested.load(std::memory_order_acquire)) {
        return;
    }
    if (g_p0OutstandingCallbacks != 0 || g_p0OutstandingAddScriptRefs != 0) {
        return;
    }

    // The mod's contract is that WebView2 releases these objects, and runs the
    // completions that decrement them, on the UI thread that created them. A
    // zero reached anywhere else does not prove that the releasing frame has
    // returned, so it must not signal.
    DWORD uiThread = g_p1UiThreadId.load(std::memory_order_acquire);
    DWORD current = GetCurrentThreadId();
    if (uiThread == 0 || current != uiThread) {
        return;
    }
    if (g_p1ReleaseOffThread.load(std::memory_order_acquire)) {
        return;
    }

    if (!g_p1TeardownCompleted.load(std::memory_order_acquire)) {
        return;
    }
    SetEvent(g_p1TeardownEvent);
}

// Must be called with g_p0OutstandingLock held. Every decrement runs this: a
// wrapper released on a thread that is not the WebView2 UI thread means that
// frame cannot be proven to have returned, so the unload never proceeds.
static void P1LatchReleaseOffThreadLocked() {
    DWORD uiThread = g_p1UiThreadId.load(std::memory_order_acquire);
    DWORD current = GetCurrentThreadId();
    if (uiThread != 0 && current == uiThread) {
        return;
    }
    if (!g_p1ReleaseOffThread.exchange(true, std::memory_order_acq_rel)) {
        Wh_Log(L"[p1] DEFECT: a wrapper was released on thread %lu, not the "
               L"known UI thread %lu; that frame cannot be proven to have "
               L"returned, so the unload never proceeds",
               static_cast<unsigned long>(current),
               static_cast<unsigned long>(uiThread));
    }
}

static void P0CountHandedOver() {
    AcquireSRWLockExclusive(&g_p0OutstandingLock);
    ++g_p0OutstandingCallbacks;
    ReleaseSRWLockExclusive(&g_p0OutstandingLock);
}

static void P0CountCallbackReleased() {
    AcquireSRWLockExclusive(&g_p0OutstandingLock);
    --g_p0OutstandingCallbacks;
    P1LatchReleaseOffThreadLocked();
    P1MaybeSignalTeardownLocked();
    ReleaseSRWLockExclusive(&g_p0OutstandingLock);
}

static void P0CountAddScriptRefHeld() {
    AcquireSRWLockExclusive(&g_p0OutstandingLock);
    ++g_p0OutstandingAddScriptRefs;
    ReleaseSRWLockExclusive(&g_p0OutstandingLock);
}

static void P0CountAddScriptRefReleased() {
    AcquireSRWLockExclusive(&g_p0OutstandingLock);
    --g_p0OutstandingAddScriptRefs;
    P1LatchReleaseOffThreadLocked();
    P1MaybeSignalTeardownLocked();
    ReleaseSRWLockExclusive(&g_p0OutstandingLock);
}

// ---------------------------------------------------------------------------
// Ref-counted callback wrappers
//
// The wrappers are plain structs whose first bytes are the COM vptr. The
// vtable is built by hand: QueryInterface, AddRef, Release, Invoke.
//
// Each wrapper answers QueryInterface for IID_IUnknown and for exactly the
// handler interface it implements; every other IID gets E_NOINTERFACE. The
// IIDs are copied from the local WebView2.h
// (microsoft.web.webview2\1.0.3719.77\build\native\include\WebView2.h).
// ---------------------------------------------------------------------------

// ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler
// {4E8A3389-C9D8-4BD2-B6B5-124FEE6CC14D}
static const IID kIidEnvironmentCompleted = {
    0x4e8a3389, 0xc9d8, 0x4bd2, {0xb6, 0xb5, 0x12, 0x4f, 0xee, 0x6c, 0xc1, 0x4d}};
// ICoreWebView2CreateCoreWebView2ControllerCompletedHandler
// {6C4819F3-C9B7-4260-8127-C9F5BDE7F68C}
static const IID kIidControllerCompleted = {
    0x6c4819f3, 0xc9b7, 0x4260, {0x81, 0x27, 0xc9, 0xf5, 0xbd, 0xe7, 0xf6, 0x8c}};
// ICoreWebView2WebMessageReceivedEventHandler
// {57213F19-00E6-49FA-8E07-898EA01ECBD2}
static const IID kIidWebMessageReceived = {
    0x57213f19, 0x00e6, 0x49fa, {0x8e, 0x07, 0x89, 0x8e, 0xa0, 0x1e, 0xcb, 0xd2}};
// ICoreWebView2NavigationStartingEventHandler
// {9ADBE429-F36D-432B-9DDC-F8881FBD76E3}
static const IID kIidNavigationStarting = {
    0x9adbe429, 0xf36d, 0x432b, {0x9d, 0xdc, 0xf8, 0x88, 0x1f, 0xbd, 0x76, 0xe3}};
// ICoreWebView2NavigationCompletedEventHandler
// {D33A35BF-1C49-4F98-93AB-006E0533FE1C}
static const IID kIidNavigationCompleted = {
    0xd33a35bf, 0x1c49, 0x4f98, {0x93, 0xab, 0x00, 0x6e, 0x05, 0x33, 0xfe, 0x1c}};
// ICoreWebView2AddScriptToExecuteOnDocumentCreatedCompletedHandler
// {B99369F3-9B11-47B5-BC6F-8E7895FCEA17}
static const IID kIidAddScriptCompleted = {
    0xb99369f3, 0x9b11, 0x47b5, {0xbc, 0x6f, 0x8e, 0x78, 0x95, 0xfc, 0xea, 0x17}};
// ICoreWebView2ExecuteScriptCompletedHandler
// {49511172-CC67-4BCA-9923-137112F4C4CC}
static const IID kIidExecuteScriptCompleted = {
    0x49511172, 0xcc67, 0x4bca, {0x99, 0x23, 0x13, 0x71, 0x12, 0xf4, 0xc4, 0xcc}};

struct P0WrapperBase {
    const void* const* vtbl;
    volatile LONG ref;
    void (*finalize)(P0WrapperBase*);
    const IID* iid;  // the one handler interface this wrapper implements
    // True once the wrapper was passed to a WebView2 method and counted in
    // g_p0OutstandingCallbacks. The final Release decrements the counter.
    bool counted;
};

struct P0EnvHandler : P0WrapperBase {
    void* app;
};

struct P0ControllerHandler : P0WrapperBase {
    void* app;
};

struct P0MessageHandler : P0WrapperBase {};

struct P0ScriptHandler : P0WrapperBase {
    const wchar_t* label;
    // HRESULT of the call that created this handler, filled in right after the
    // call returns so the (asynchronous) completion log can include it.
    HRESULT immediateHr;
    // AddScript only: an owned webview reference kept until the completion
    // arrives. It must outlive the teardown's P0CleanupWebView, otherwise a
    // completion that arrives after Stopping cannot remove the script.
    void* webView;
};

struct P0NavigationStartingHandler : P0WrapperBase {};

struct P0NavigationCompletedHandler : P0WrapperBase {};

static HRESULT STDMETHODCALLTYPE P0WrapperQueryInterface(P0WrapperBase* self,
                                                         const IID& riid,
                                                         void** ppv);
static ULONG STDMETHODCALLTYPE P0WrapperAddRef(P0WrapperBase* self);
static ULONG STDMETHODCALLTYPE P0WrapperRelease(P0WrapperBase* self);

static void P0EnvHandlerFinalize(P0WrapperBase* self);
static void P0ControllerHandlerFinalize(P0WrapperBase* self);
static void P0MessageHandlerFinalize(P0WrapperBase* self);
static void P0ScriptHandlerFinalize(P0WrapperBase* self);
static void P0NavigationStartingHandlerFinalize(P0WrapperBase* self);
static void P0NavigationCompletedHandlerFinalize(P0WrapperBase* self);

// Defined next to the release queue; the finalizer hands the held webview
// reference to the UI thread instead of calling Release itself.
static void P0ReleaseScriptWebView(P0ScriptHandler* handler);

static HRESULT STDMETHODCALLTYPE P0EnvHandlerInvoke(P0EnvHandler* self,
                                                    HRESULT hr,
                                                    void* env);
static HRESULT STDMETHODCALLTYPE P0ControllerHandlerInvoke(
    P0ControllerHandler* self,
    HRESULT hr,
    void* controller);
static HRESULT STDMETHODCALLTYPE P0MessageHandlerInvoke(
    P0MessageHandler* self,
    void* sender,
    void* args);
static HRESULT STDMETHODCALLTYPE P0ScriptHandlerInvoke(
    P0ScriptHandler* self,
    HRESULT hr,
    LPCWSTR text);
static HRESULT STDMETHODCALLTYPE P0NavigationStartingInvoke(
    P0NavigationStartingHandler* self,
    void* sender,
    void* args);
static HRESULT STDMETHODCALLTYPE P0NavigationCompletedInvoke(
    P0NavigationCompletedHandler* self,
    void* sender,
    void* args);

static HRESULT STDMETHODCALLTYPE P0EnvCreateControllerThunk(void* self,
                                                            HWND parent,
                                                            void* handler);
static HRESULT STDMETHODCALLTYPE P0EnvCreateControllerWithOptionsThunk(
    void* self,
    HWND parent,
    void* options,
    void* handler);

static const void* const kEnvHandlerVtbl[4] = {
    reinterpret_cast<const void*>(&P0WrapperQueryInterface),
    reinterpret_cast<const void*>(&P0WrapperAddRef),
    reinterpret_cast<const void*>(&P0WrapperRelease),
    reinterpret_cast<const void*>(&P0EnvHandlerInvoke),
};

static const void* const kControllerHandlerVtbl[4] = {
    reinterpret_cast<const void*>(&P0WrapperQueryInterface),
    reinterpret_cast<const void*>(&P0WrapperAddRef),
    reinterpret_cast<const void*>(&P0WrapperRelease),
    reinterpret_cast<const void*>(&P0ControllerHandlerInvoke),
};

static const void* const kMessageHandlerVtbl[4] = {
    reinterpret_cast<const void*>(&P0WrapperQueryInterface),
    reinterpret_cast<const void*>(&P0WrapperAddRef),
    reinterpret_cast<const void*>(&P0WrapperRelease),
    reinterpret_cast<const void*>(&P0MessageHandlerInvoke),
};

static const void* const kScriptHandlerVtbl[4] = {
    reinterpret_cast<const void*>(&P0WrapperQueryInterface),
    reinterpret_cast<const void*>(&P0WrapperAddRef),
    reinterpret_cast<const void*>(&P0WrapperRelease),
    reinterpret_cast<const void*>(&P0ScriptHandlerInvoke),
};

static const void* const kNavigationStartingHandlerVtbl[4] = {
    reinterpret_cast<const void*>(&P0WrapperQueryInterface),
    reinterpret_cast<const void*>(&P0WrapperAddRef),
    reinterpret_cast<const void*>(&P0WrapperRelease),
    reinterpret_cast<const void*>(&P0NavigationStartingInvoke),
};

static const void* const kNavigationCompletedHandlerVtbl[4] = {
    reinterpret_cast<const void*>(&P0WrapperQueryInterface),
    reinterpret_cast<const void*>(&P0WrapperAddRef),
    reinterpret_cast<const void*>(&P0WrapperRelease),
    reinterpret_cast<const void*>(&P0NavigationCompletedInvoke),
};

// Answer IID_IUnknown and the one handler interface the wrapper implements.
// Anything else is E_NOINTERFACE with a null out pointer, as COM requires.
static HRESULT STDMETHODCALLTYPE P0WrapperQueryInterface(P0WrapperBase* self,
                                                         const IID& riid,
                                                         void** ppv) {
    if (!ppv) {
        return E_POINTER;
    }

    if (!self || !(IsEqualGUID(riid, IID_IUnknown) ||
                   (self->iid && IsEqualGUID(riid, *self->iid)))) {
        *ppv = nullptr;
        return E_NOINTERFACE;
    }

    *ppv = self;
    InterlockedIncrement(&self->ref);
    return S_OK;
}

static ULONG STDMETHODCALLTYPE P0WrapperAddRef(P0WrapperBase* self) {
    if (!self) {
        return 0;
    }
    return static_cast<ULONG>(InterlockedIncrement(&self->ref));
}

static ULONG STDMETHODCALLTYPE P0WrapperRelease(P0WrapperBase* self) {
    if (!self) {
        return 0;
    }

    LONG count = InterlockedDecrement(&self->ref);
    if (count == 0) {
        bool counted = self->counted;
        void (*finalize)(P0WrapperBase*) = self->finalize;
        if (finalize) {
            finalize(self);
        }
        // Decrement only after the finalizer ran: the teardown wait must not
        // observe zero while the object is still being destroyed. The
        // decrement latches an off-UI-thread release, and when it empties the
        // counters P1MaybeSignalTeardownLocked signals the event if the
        // teardown completed on the UI thread.
        if (counted) {
            P0CountCallbackReleased();
        }
    }
    return static_cast<ULONG>(count);
}

// Counts a wrapper that is about to be passed to WebView2. Called on the UI
// thread. Under the WebView2 contract the final Release also runs on that
// thread; every decrement latches a release seen anywhere else.
static void P0MarkHandedOver(P0WrapperBase* handler) {
    if (!handler) {
        return;
    }
    handler->counted = true;
    P0CountHandedOver();
}

static void P0EnvHandlerFinalize(P0WrapperBase* self) {
    auto* handler = static_cast<P0EnvHandler*>(self);
    if (handler->app) {
        auto release =
            reinterpret_cast<FnRelease>(P0GetRawSlot(handler->app, kSlotRelease));
        if (release) {
            release(handler->app);
        }
    }
    delete handler;
}

static void P0ControllerHandlerFinalize(P0WrapperBase* self) {
    auto* handler = static_cast<P0ControllerHandler*>(self);
    if (handler->app) {
        auto release =
            reinterpret_cast<FnRelease>(P0GetRawSlot(handler->app, kSlotRelease));
        if (release) {
            release(handler->app);
        }
    }
    delete handler;
}

static void P0MessageHandlerFinalize(P0WrapperBase* self) {
    delete static_cast<P0MessageHandler*>(self);
}

static void P0ScriptHandlerFinalize(P0WrapperBase* self) {
    auto* handler = static_cast<P0ScriptHandler*>(self);
    // No completion arrived (or it never took the reference): hand the held
    // webview reference to the UI thread instead of releasing it here.
    P0ReleaseScriptWebView(handler);
    delete handler;
}

static void P0NavigationStartingHandlerFinalize(P0WrapperBase* self) {
    delete static_cast<P0NavigationStartingHandler*>(self);
}

static void P0NavigationCompletedHandlerFinalize(P0WrapperBase* self) {
    delete static_cast<P0NavigationCompletedHandler*>(self);
}

static P0EnvHandler* P0CreateEnvHandler(void* app) {
    auto* handler = new (std::nothrow) P0EnvHandler();
    if (!handler) {
        return nullptr;
    }

    handler->vtbl = kEnvHandlerVtbl;
    handler->ref = 1;
    handler->finalize = &P0EnvHandlerFinalize;
    handler->iid = &kIidEnvironmentCompleted;
    handler->counted = false;
    handler->app = app;

    if (app) {
        auto addRef = reinterpret_cast<FnAddRef>(P0GetRawSlot(app, kSlotAddRef));
        if (!addRef) {
            delete handler;
            return nullptr;
        }
        addRef(app);
    }

    return handler;
}

static P0ControllerHandler* P0CreateControllerHandler(void* app) {
    auto* handler = new (std::nothrow) P0ControllerHandler();
    if (!handler) {
        return nullptr;
    }

    handler->vtbl = kControllerHandlerVtbl;
    handler->ref = 1;
    handler->finalize = &P0ControllerHandlerFinalize;
    handler->iid = &kIidControllerCompleted;
    handler->counted = false;
    handler->app = app;

    if (app) {
        auto addRef = reinterpret_cast<FnAddRef>(P0GetRawSlot(app, kSlotAddRef));
        if (!addRef) {
            delete handler;
            return nullptr;
        }
        addRef(app);
    }

    return handler;
}

static P0MessageHandler* P0CreateMessageHandler() {
    auto* handler = new (std::nothrow) P0MessageHandler();
    if (!handler) {
        return nullptr;
    }

    handler->vtbl = kMessageHandlerVtbl;
    handler->ref = 1;
    handler->finalize = &P0MessageHandlerFinalize;
    handler->iid = &kIidWebMessageReceived;
    handler->counted = false;
    return handler;
}

static P0ScriptHandler* P0CreateScriptHandler(const wchar_t* label) {
    auto* handler = new (std::nothrow) P0ScriptHandler();
    if (!handler) {
        return nullptr;
    }

    handler->vtbl = kScriptHandlerVtbl;
    handler->ref = 1;
    handler->finalize = &P0ScriptHandlerFinalize;
    // The two completion handlers differ only by IID; the label says which call
    // created this one.
    handler->iid = (label && wcscmp(label, L"AddScript") == 0)
                       ? &kIidAddScriptCompleted
                       : &kIidExecuteScriptCompleted;
    handler->label = label;
    handler->immediateHr = E_FAIL;
    handler->webView = nullptr;
    handler->counted = false;
    return handler;
}

static P0NavigationStartingHandler* P0CreateNavigationStartingHandler() {
    auto* handler = new (std::nothrow) P0NavigationStartingHandler();
    if (!handler) {
        return nullptr;
    }

    handler->vtbl = kNavigationStartingHandlerVtbl;
    handler->ref = 1;
    handler->finalize = &P0NavigationStartingHandlerFinalize;
    handler->iid = &kIidNavigationStarting;
    handler->counted = false;
    return handler;
}

static P0NavigationCompletedHandler* P0CreateNavigationCompletedHandler() {
    auto* handler = new (std::nothrow) P0NavigationCompletedHandler();
    if (!handler) {
        return nullptr;
    }

    handler->vtbl = kNavigationCompletedHandlerVtbl;
    handler->ref = 1;
    handler->finalize = &P0NavigationCompletedHandlerFinalize;
    handler->iid = &kIidNavigationCompleted;
    handler->counted = false;
    return handler;
}

// ---------------------------------------------------------------------------
// Environment vtable patching
// ---------------------------------------------------------------------------

struct P0EnvPatchState {
    std::mutex mtx;
    void* env = nullptr;  // owns a reference while patched
    void** originalVtbl = nullptr;
    void** patchedVtbl = nullptr;
    bool installed = false;
};

static P0EnvPatchState g_envPatch;
static std::atomic<void*> g_origEnvCreateController{nullptr};
static std::atomic<void*> g_origEnvCreateControllerWithOptions{nullptr};

static void P0PatchEnvironment(void* env) {
    if (!env) {
        Wh_Log(L"[p0] env not patched: null environment");
        return;
    }

    void** originalVtbl = *reinterpret_cast<void***>(env);
    if (!originalVtbl || !P0IsModuleCode(originalVtbl)) {
        Wh_Log(L"[p0] env=%p: vtable %p is not inside "
               L"EmbeddedBrowserWebView.dll; not patching",
               env, static_cast<void*>(originalVtbl));
        return;
    }

    void* slot3 = P0GetRawSlot(env, kEnvSlotCreateController);
    void* slot21 = P0GetRawSlot(env, kEnvSlotCreateControllerWithOptions);
    bool slot3InModule = P0IsModuleCode(slot3);
    bool slot21InModule = P0IsModuleCode(slot21);
    bool has3 = slot3InModule;
    bool has21 = slot21InModule;

    Wh_Log(L"[p0] env vtable: base=%p slot3=%p slot21=%p slot3InModule=%d "
           L"slot21InModule=%d",
           static_cast<void*>(originalVtbl), slot3, slot21,
           slot3InModule ? 1 : 0, slot21InModule ? 1 : 0);

    if (!has3 && !has21) {
        Wh_Log(L"[p0] env=%p: neither slot 3 nor slot 21 is module code; not "
               L"patching",
               env);
        return;
    }

    std::lock_guard<std::mutex> lock(g_envPatch.mtx);
    if (g_envPatch.installed) {
        Wh_Log(L"[p0] env=%p: another environment is already patched; skipping",
               env);
        return;
    }

    auto* copy = new (std::nothrow) void*[kEnvVtblSlots];
    if (!copy) {
        Wh_Log(L"[p0] env=%p: out of memory for the vtable copy", env);
        return;
    }

    if (!P0CopyVtable(copy, originalVtbl, kEnvVtblSlots)) {
        Wh_Log(L"[p0] env=%p: vtable copy faulted; not patching", env);
        delete[] copy;
        return;
    }

    if (has3) {
        copy[kEnvSlotCreateController] =
            reinterpret_cast<void*>(&P0EnvCreateControllerThunk);
    }
    if (has21) {
        copy[kEnvSlotCreateControllerWithOptions] =
            reinterpret_cast<void*>(&P0EnvCreateControllerWithOptionsThunk);
    }

    auto addRef =
        reinterpret_cast<FnAddRef>(P0GetRawSlot(env, kSlotAddRef));
    if (!addRef) {
        Wh_Log(L"[p0] env=%p: AddRef slot missing; not patching", env);
        delete[] copy;
        return;
    }
    addRef(env);

    g_envPatch.env = env;
    g_envPatch.originalVtbl = originalVtbl;
    g_envPatch.patchedVtbl = copy;
    g_envPatch.installed = true;

    // Publish the originals before switching the vptr: a thunk may fire on
    // another thread as soon as the swap is visible.
    if (has3) {
        g_origEnvCreateController.store(slot3, std::memory_order_release);
    }
    if (has21) {
        g_origEnvCreateControllerWithOptions.store(slot21,
                                                   std::memory_order_release);
    }

    *reinterpret_cast<void***>(env) = copy;

    Wh_Log(L"[p0] env=%p patched (slot3=%p valid=%d, slot21=%p valid=%d)", env,
           slot3, has3 ? 1 : 0, slot21, has21 ? 1 : 0);
}

// `full` is false only when the UI thread was never reachable: the vptr is put
// back so the app cannot jump into this image, but the copy and the reference
// are deliberately kept (leaked) because freeing them could race the UI thread.
static void P0RestoreEnvironment(bool full) {
    std::lock_guard<std::mutex> lock(g_envPatch.mtx);
    if (!g_envPatch.installed) {
        Wh_Log(L"cleanup: environment not patched, nothing to restore");
        return;
    }

    if (g_envPatch.env) {
        void** current = *reinterpret_cast<void***>(g_envPatch.env);
        if (current == g_envPatch.patchedVtbl) {
            *reinterpret_cast<void***>(g_envPatch.env) =
                g_envPatch.originalVtbl;
            Wh_Log(L"[p0] env=%p vtable restored", g_envPatch.env);
        } else {
            Wh_Log(L"[p0] env=%p vptr changed externally (%p); not restoring",
                   g_envPatch.env, current);
        }

        if (!full) {
            Wh_Log(L"[p0] environment reference and vtable copy kept: teardown "
                   L"did not run on the UI thread");
            g_origEnvCreateController.store(nullptr);
            g_origEnvCreateControllerWithOptions.store(nullptr);
            return;
        }

        auto release = reinterpret_cast<FnRelease>(
            P0GetRawSlot(g_envPatch.env, kSlotRelease));
        if (release) {
            release(g_envPatch.env);
            Wh_Log(L"cleanup: env reference released");
        }
    }

    delete[] g_envPatch.patchedVtbl;
    Wh_Log(L"cleanup: env vtable copy freed");

    g_envPatch.installed = false;
    g_envPatch.env = nullptr;
    g_envPatch.originalVtbl = nullptr;
    g_envPatch.patchedVtbl = nullptr;
    g_origEnvCreateController.store(nullptr);
    g_origEnvCreateControllerWithOptions.store(nullptr);
}

// ---------------------------------------------------------------------------
// WebView state
// ---------------------------------------------------------------------------

static std::mutex g_webViewMutex;
static void* g_webView = nullptr;
static EventRegistrationToken g_webMessageToken{};
static P0MessageHandler* g_webMessageHandler = nullptr;
static EventRegistrationToken g_navigationStartingToken{};
static EventRegistrationToken g_navigationCompletedToken{};
static P0NavigationStartingHandler* g_navigationStartingHandler = nullptr;
static P0NavigationCompletedHandler* g_navigationCompletedHandler = nullptr;

// Last URI seen by NavigationStarting. NavigationCompleted's args expose only
// IsSuccess/WebErrorStatus/NavigationId, and get_Source on the webview is
// deliberately not used, so the URI is carried over from the previous event.
static std::mutex g_navigationMutex;
static std::wstring g_lastNavigationUri;

// One-shot: inject once more when AddScriptToExecuteOnDocumentCreated finishes
// asynchronously after the controller path already ran its fallback.
static std::atomic<bool> g_postAddScriptInjectionDone{false};

static void P0SetLastNavigationUri(const std::wstring& uri) {
    try {
        std::lock_guard<std::mutex> lock(g_navigationMutex);
        g_lastNavigationUri = uri;
    } catch (...) {
    }
}

static std::wstring P0GetLastNavigationUri() {
    try {
        std::lock_guard<std::mutex> lock(g_navigationMutex);
        return g_lastNavigationUri;
    } catch (...) {
        return std::wstring();
    }
}

// Immediate HRESULT of the AddScript call. The completion handler reports the
// asynchronous result (and the script id), which is logged together with it.
// ExecuteScript's immediate HRESULT travels in the handler itself
// (P0ScriptHandler::immediateHr), so repeated injections cannot overwrite it.
static std::atomic<unsigned long> g_addScriptHr{0xFFFFFFFFul};

// Script id returned by the AddScriptToExecuteOnDocumentCreated completion. The
// unload sequence passes it to RemoveScriptToExecuteOnDocumentCreated (slot 28)
// so the next document cannot start the engine after the mod is gone.
static std::mutex g_scriptIdMutex;
static std::wstring g_scriptId;
static std::atomic<bool> g_scriptRemoved{false};

static void P0SetScriptId(const std::wstring& id) {
    try {
        std::lock_guard<std::mutex> lock(g_scriptIdMutex);
        g_scriptId = id;
    } catch (...) {
    }
}

static std::wstring P0GetScriptId() {
    try {
        std::lock_guard<std::mutex> lock(g_scriptIdMutex);
        return g_scriptId;
    } catch (...) {
        return std::wstring();
    }
}

// RAII holder for a COM reference that is released through slot 2.
struct P0ComRef {
    void* object = nullptr;

    ~P0ComRef() {
        if (object) {
            auto release =
                reinterpret_cast<FnRelease>(P0GetRawSlot(object, kSlotRelease));
            if (release) {
                release(object);
            }
        }
    }

    P0ComRef() = default;
    P0ComRef(const P0ComRef&) = delete;
    P0ComRef& operator=(const P0ComRef&) = delete;
};

struct P0HandlerRef {
    P0WrapperBase* handler = nullptr;

    ~P0HandlerRef() {
        if (handler) {
            P0WrapperRelease(handler);
        }
    }

    P0HandlerRef() = default;
    P0HandlerRef(const P0HandlerRef&) = delete;
    P0HandlerRef& operator=(const P0HandlerRef&) = delete;
};

static bool P0StoreWebView(void* webView,
                           EventRegistrationToken token,
                           P0MessageHandler* handler) {
    std::lock_guard<std::mutex> lock(g_webViewMutex);
    if (g_webView) {
        return false;
    }

    auto addRef = reinterpret_cast<FnAddRef>(P0GetRawSlot(webView, kSlotAddRef));
    if (!addRef) {
        return false;
    }
    addRef(webView);

    g_webView = webView;
    g_webMessageToken = token;
    g_webMessageHandler = handler;
    return true;
}

// Removes the document-created script registered by AddScript. Must run on the
// UI thread. The teardown calls it before the handlers are removed; a late
// AddScript completion calls it afterwards through the webview reference the
// completion handler kept alive.
static void P0RemoveRegisteredScript(void* webView) {
    if (!webView) {
        return;
    }
    if (g_scriptRemoved.load(std::memory_order_acquire)) {
        Wh_Log(L"[p0] AddScript script already removed");
        return;
    }
    std::wstring id = P0GetScriptId();
    if (id.empty()) {
        Wh_Log(L"[p0] no AddScript id was captured yet; the document-created "
               L"script installs observers and retries, so a late completion "
               L"must still remove it");
        return;
    }
    void* removeFn = nullptr;
    if (!P0GetValidatedSlot(webView, kWebViewSlotRemoveScript, &removeFn)) {
        Wh_Log(L"[p0] RemoveScriptToExecuteOnDocumentCreated slot invalid");
        return;
    }
    HRESULT hr = reinterpret_cast<FnRemoveScript>(removeFn)(webView, id.c_str());
    if (SUCCEEDED(hr)) {
        g_scriptRemoved.store(true, std::memory_order_release);
    }
    Wh_Log(L"[p0] RemoveScriptToExecuteOnDocumentCreated id=%.200s -> %08X",
           id.c_str(), static_cast<unsigned>(hr));
}

static void P0CleanupWebView() {
    void* webView = nullptr;
    EventRegistrationToken token{};
    P0MessageHandler* handler = nullptr;
    EventRegistrationToken navStartingToken{};
    EventRegistrationToken navCompletedToken{};
    P0NavigationStartingHandler* navStartingHandler = nullptr;
    P0NavigationCompletedHandler* navCompletedHandler = nullptr;

    {
        std::lock_guard<std::mutex> lock(g_webViewMutex);
        webView = g_webView;
        g_webView = nullptr;
        token = g_webMessageToken;
        g_webMessageToken = {};
        handler = g_webMessageHandler;
        g_webMessageHandler = nullptr;
        navStartingToken = g_navigationStartingToken;
        g_navigationStartingToken = {};
        navCompletedToken = g_navigationCompletedToken;
        g_navigationCompletedToken = {};
        navStartingHandler = g_navigationStartingHandler;
        g_navigationStartingHandler = nullptr;
        navCompletedHandler = g_navigationCompletedHandler;
        g_navigationCompletedHandler = nullptr;
    }

    if (webView) {
        if (navStartingToken.value != 0) {
            void* removeFn = nullptr;
            if (P0GetValidatedSlot(webView, kWebViewSlotRemoveNavigationStarting,
                                   &removeFn)) {
                HRESULT hr = reinterpret_cast<FnRemoveNavigationEventHandler>(
                    removeFn)(webView, navStartingToken);
                Wh_Log(L"[p0] remove_NavigationStarting token=%lld -> %08X",
                       static_cast<long long>(navStartingToken.value),
                       static_cast<unsigned>(hr));
            } else {
                Wh_Log(L"[p0] remove_NavigationStarting slot invalid");
            }
        }

        if (navCompletedToken.value != 0) {
            void* removeFn = nullptr;
            if (P0GetValidatedSlot(webView,
                                   kWebViewSlotRemoveNavigationCompleted,
                                   &removeFn)) {
                HRESULT hr = reinterpret_cast<FnRemoveNavigationEventHandler>(
                    removeFn)(webView, navCompletedToken);
                Wh_Log(L"[p0] remove_NavigationCompleted token=%lld -> %08X",
                       static_cast<long long>(navCompletedToken.value),
                       static_cast<unsigned>(hr));
            } else {
                Wh_Log(L"[p0] remove_NavigationCompleted slot invalid");
            }
        }

        if (token.value != 0) {
            void* removeFn = nullptr;
            if (P0GetValidatedSlot(webView, kWebViewSlotRemoveWebMessageReceived,
                                   &removeFn)) {
                HRESULT hr = reinterpret_cast<FnRemoveWebMessageReceived>(
                    removeFn)(webView, token);
                Wh_Log(L"[p0] remove_WebMessageReceived token=%lld -> %08X",
                       static_cast<long long>(token.value),
                       static_cast<unsigned>(hr));
            } else {
                Wh_Log(L"[p0] remove_WebMessageReceived slot invalid");
            }
        }

        auto release =
            reinterpret_cast<FnRelease>(P0GetRawSlot(webView, kSlotRelease));
        if (release) {
            release(webView);
            Wh_Log(L"[p0] WebView2 reference released");
        }
    }

    if (handler) {
        P0WrapperRelease(handler);
        Wh_Log(L"[p0] message handler released");
    }

    if (navStartingHandler) {
        P0WrapperRelease(navStartingHandler);
        Wh_Log(L"[p0] NavigationStarting handler released");
    }

    if (navCompletedHandler) {
        P0WrapperRelease(navCompletedHandler);
        Wh_Log(L"[p0] NavigationCompleted handler released");
    }
}

// Returns a referenced WebView2 pointer, or nullptr when none is stored yet.
// The caller releases it (P0ComRef is the usual holder).
static void* P0AcquireWebView() {
    std::lock_guard<std::mutex> lock(g_webViewMutex);
    void* webView = g_webView;
    if (!webView) {
        return nullptr;
    }
    auto addRef = reinterpret_cast<FnAddRef>(P0GetRawSlot(webView, kSlotAddRef));
    if (!addRef) {
        return nullptr;
    }
    addRef(webView);
    return webView;
}

// ---------------------------------------------------------------------------
// P1: settings, HTTP relay and the page bridge
//
// The page owns request building and response parsing; the mod only moves
// bytes. Every whtr1 message is handled on the WebView2 UI thread. The HTTP
// work runs on a small worker pool, and results travel back through a hidden
// message-only window owned by the UI thread, so PostWebMessageAsString is
// always called on the thread that owns the webview.
//
// The protocol is documented in docs/PROTOCOL.md. Messages are separated by
// U+001F and the last field may contain it, so every split uses a field limit.
// ---------------------------------------------------------------------------

static constexpr wchar_t kP1Tag[] = L"whtr1";
static constexpr wchar_t kP1DispatchClass[] = L"WindhawkUITranslator.P1Dispatch";
static constexpr UINT kP1MsgDrainReplies = WM_APP + 0x51;
static constexpr UINT kP1MsgSettingsChanged = WM_APP + 0x52;
// Posted teardown request. Its handler runs on the UI thread, releases every
// object this mod owns there and then signals g_p1TeardownEvent once the
// cleanup completed and both outstanding counters are zero on the UI thread. It
// is posted, not sent: the UI thread's own message loop keeps dispatching
// WebView2 completions after the handler returns, and those completions are
// what release the last wrappers.
static constexpr UINT kP1MsgTeardown = WM_APP + 0x53;
// Destroys the dispatch window. Wh_ModBeforeUninit sends it with SendMessageW
// as the last step. The handler sets g_p1DestroyCompleted only when the window
// and its class are gone; the sender checks that flag before it treats the
// teardown as finished.
static constexpr UINT kP1MsgDestroyWindow = WM_APP + 0x54;

static constexpr size_t kP1MaxMessageChars = 4u * 1024u * 1024u;
static constexpr size_t kP1MaxUrlChars = 4096;
static constexpr size_t kP1MaxBodyBytes = 1024u * 1024u;
static constexpr size_t kP1MaxReplyBytes = 1024u * 1024u;
static constexpr size_t kP1MaxJsLogChars = 500;
static constexpr size_t kP1HttpWorkers = 4;
static constexpr size_t kP1HttpQueueLimit = 32;

// ---------------------------------------------------------------------------
// Small text helpers
// ---------------------------------------------------------------------------

static std::string P1WideToUtf8(const std::wstring& text) {
    std::string result;
    try {
        if (text.empty()) {
            return result;
        }
        int length = WideCharToMultiByte(CP_UTF8, 0, text.c_str(),
                                         static_cast<int>(text.size()), nullptr,
                                         0, nullptr, nullptr);
        if (length <= 0) {
            return result;
        }
        result.resize(static_cast<size_t>(length));
        WideCharToMultiByte(CP_UTF8, 0, text.c_str(),
                            static_cast<int>(text.size()), &result[0], length,
                            nullptr, nullptr);
    } catch (...) {
        result.clear();
    }
    return result;
}

static std::wstring P1Utf8ToWide(const std::string& text) {
    std::wstring result;
    try {
        if (text.empty()) {
            return result;
        }
        // Flags 0: invalid sequences become U+FFFD instead of failing.
        int length = MultiByteToWideChar(CP_UTF8, 0, text.data(),
                                         static_cast<int>(text.size()), nullptr,
                                         0);
        if (length <= 0) {
            return result;
        }
        result.resize(static_cast<size_t>(length));
        MultiByteToWideChar(CP_UTF8, 0, text.data(),
                            static_cast<int>(text.size()), &result[0], length);
    } catch (...) {
        result.clear();
    }
    return result;
}

static wchar_t P1AsciiLower(wchar_t ch) {
    if (ch >= L'A' && ch <= L'Z') {
        return static_cast<wchar_t>(ch - L'A' + L'a');
    }
    return ch;
}

static wchar_t P1AsciiUpper(wchar_t ch) {
    if (ch >= L'a' && ch <= L'z') {
        return static_cast<wchar_t>(ch - L'a' + L'A');
    }
    return ch;
}

static std::wstring P1Lower(const std::wstring& text) {
    std::wstring result = text;
    for (wchar_t& ch : result) {
        ch = P1AsciiLower(ch);
    }
    return result;
}

static bool P1EqualsNoCase(const std::wstring& a, const std::wstring& b) {
    if (a.size() != b.size()) {
        return false;
    }
    for (size_t i = 0; i < a.size(); ++i) {
        if (P1AsciiLower(a[i]) != P1AsciiLower(b[i])) {
            return false;
        }
    }
    return true;
}

// Splits on `sep` into at most maxFields entries. The last entry receives the
// rest of the text unchanged, which is what the protocol needs for bodies and
// for the settings JSON.
static void P1Split(const std::wstring& text,
                    wchar_t sep,
                    size_t maxFields,
                    std::vector<std::wstring>& out) {
    out.clear();
    if (maxFields == 0) {
        return;
    }
    size_t start = 0;
    while (out.size() + 1 < maxFields) {
        size_t pos = text.find(sep, start);
        if (pos == std::wstring::npos) {
            break;
        }
        out.push_back(text.substr(start, pos - start));
        start = pos + 1;
    }
    out.push_back(text.substr(start));
}

static bool P1ParseUInt64(const std::wstring& text, unsigned long long& value) {
    // 20 digits is the longest possible uint64; the overflow check below
    // rejects anything above 18446744073709551615.
    if (text.empty() || text.size() > 20) {
        return false;
    }
    unsigned long long result = 0;
    for (wchar_t ch : text) {
        if (ch < L'0' || ch > L'9') {
            return false;
        }
        unsigned digit = static_cast<unsigned>(ch - L'0');
        if (result > (~0ull - digit) / 10ull) {
            return false;
        }
        result = result * 10ull + digit;
    }
    if (result == 0) {
        return false;
    }
    value = result;
    return true;
}

static int P1ClampInt(int value, int low, int high) {
    if (value < low) {
        return low;
    }
    if (value > high) {
        return high;
    }
    return value;
}

static std::wstring P1ClipForLog(const std::wstring& text, size_t maxChars) {
    if (text.size() <= maxChars) {
        return text;
    }
    std::wstring result = text.substr(0, maxChars);
    result += L"...";
    return result;
}

// ---------------------------------------------------------------------------
// Settings
// ---------------------------------------------------------------------------

struct P1SettingsSnapshot {
    std::wstring targetLang;
    std::wstring provider;
    std::wstring llmBaseUrl;
    std::wstring llmModel;
    std::wstring credentialTarget;
    std::wstring llmApiKey;
    std::wstring keySourceSetting;
    std::vector<std::wstring> glossary;
    std::wstring shortcutToggle;
    bool gestureToggle = true;
    bool scopeReadme = true;
    bool scopeSettings = true;
    bool scopeModName = true;
    bool scopeModSummary = true;
    bool scopeChangelog = true;
    int hoverDelayMs = 120;
    int batchSize = 20;
    int batchChars = 10000;
    int batchIntervalMs = 400;
    int concurrency = 4;
    int timeoutMs = 20000;
    bool cacheEnabled = true;
    int cacheTtlDays = 30;
    bool clearCacheRequested = false;
};

static std::wstring P1ReadStringSetting(PCWSTR name,
                                        PCWSTR fallback,
                                        bool allowEmpty) {
    PCWSTR value = Wh_GetStringSetting(name);
    std::wstring result;
    try {
        if (value) {
            result.assign(value);
        }
    } catch (...) {
    }
    if (value) {
        Wh_FreeStringSetting(value);
    }
    if (result.empty() && !allowEmpty && fallback) {
        try {
            result.assign(fallback);
        } catch (...) {
        }
    }
    return result;
}

static P1SettingsSnapshot P1ReadSettings() {
    P1SettingsSnapshot s;
    s.targetLang = P1ReadStringSetting(L"targetLang", L"auto", false);
    s.provider = P1ReadStringSetting(L"provider", L"auto", false);
    s.llmBaseUrl =
        P1ReadStringSetting(L"llmBaseUrl", L"https://api.openai.com/v1", false);
    s.llmModel = P1ReadStringSetting(L"llmModel", L"", true);
    s.credentialTarget = P1ReadStringSetting(L"credentialTarget",
                                             L"windhawk-ui-translator", false);
    s.llmApiKey = P1ReadStringSetting(L"llmApiKey", L"", true);
    s.keySourceSetting =
        P1ReadStringSetting(L"llmKeySource", L"credential", false);
    // A list setting: elements are read as `glossary[0]`, `glossary[1]`, ... and
    // the first missing (or empty) entry ends the list. The settings block needs
    // one default row because Windhawk rejects an empty array; that row is empty
    // and therefore reads back as an empty list.
    s.glossary.clear();
    for (int index = 0;; ++index) {
        PCWSTR value = Wh_GetStringSetting(L"glossary[%d]", index);
        std::wstring entry;
        if (value) {
            try {
                entry.assign(value);
            } catch (...) {
            }
            Wh_FreeStringSetting(value);
        }
        if (entry.empty()) {
            break;
        }
        try {
            s.glossary.push_back(entry);
        } catch (...) {
            break;
        }
    }
    // Empty is a valid value for the shortcut: it disables it.
    s.shortcutToggle = P1ReadStringSetting(L"shortcutToggle", L"Alt+Q", true);
    s.gestureToggle = Wh_GetIntSetting(L"gestureToggle") != 0;

    s.scopeReadme = Wh_GetIntSetting(L"scopeReadme") != 0;
    s.scopeSettings = Wh_GetIntSetting(L"scopeSettings") != 0;
    s.scopeModName = Wh_GetIntSetting(L"scopeModName") != 0;
    s.scopeModSummary = Wh_GetIntSetting(L"scopeModSummary") != 0;
    s.scopeChangelog = Wh_GetIntSetting(L"scopeChangelog") != 0;
    s.cacheEnabled = Wh_GetIntSetting(L"cacheEnabled") != 0;
    s.clearCacheRequested = Wh_GetIntSetting(L"clearCache") != 0;

    s.hoverDelayMs = P1ClampInt(Wh_GetIntSetting(L"hoverDelayMs"), 0, 2000);
    s.batchSize = P1ClampInt(Wh_GetIntSetting(L"batchSize"), 1, 100);
    s.batchChars = P1ClampInt(Wh_GetIntSetting(L"batchChars"), 256, 200000);
    s.batchIntervalMs =
        P1ClampInt(Wh_GetIntSetting(L"batchIntervalMs"), 0, 5000);
    s.concurrency = P1ClampInt(Wh_GetIntSetting(L"concurrency"), 1, 8);
    s.timeoutMs = P1ClampInt(Wh_GetIntSetting(L"timeoutMs"), 1000, 120000);
    s.cacheTtlDays = P1ClampInt(Wh_GetIntSetting(L"cacheTtlDays"), 0, 3650);
    return s;
}

static void P1JsonEscapeAppend(std::wstring& out, const std::wstring& value) {
    out.push_back(L'"');
    for (wchar_t ch : value) {
        switch (ch) {
            case L'"':
                out += L"\\\"";
                break;
            case L'\\':
                out += L"\\\\";
                break;
            case L'\b':
                out += L"\\b";
                break;
            case L'\f':
                out += L"\\f";
                break;
            case L'\n':
                out += L"\\n";
                break;
            case L'\r':
                out += L"\\r";
                break;
            case L'\t':
                out += L"\\t";
                break;
            default:
                if (ch < 0x20) {
                    wchar_t buffer[8] = {};
                    swprintf_s(buffer, ARRAYSIZE(buffer), L"\\u%04X",
                               static_cast<unsigned>(ch));
                    out += buffer;
                } else {
                    out.push_back(ch);
                }
                break;
        }
    }
    out.push_back(L'"');
}

static std::wstring P1BuildSettingsJson(const P1SettingsSnapshot& s,
                                        const std::wstring& keySource,
                                        bool clearCache) {
    std::wstring json;
    json.reserve(1024);
    json += L"{";
    bool first = true;
    auto sepName = [&](const wchar_t* name) {
        if (!first) {
            json += L",";
        }
        first = false;
        json += L'"';
        json += name;
        json += L"\":";
    };
    auto appendString = [&](const wchar_t* name, const std::wstring& value) {
        sepName(name);
        P1JsonEscapeAppend(json, value);
    };
    auto appendNumber = [&](const wchar_t* name, int value) {
        sepName(name);
        json += std::to_wstring(value);
    };
    auto appendBool = [&](const wchar_t* name, bool value) {
        sepName(name);
        json += value ? L"true" : L"false";
    };
    auto appendStringArray = [&](const wchar_t* name,
                                 const std::vector<std::wstring>& values) {
        sepName(name);
        json += L"[";
        bool firstItem = true;
        for (const std::wstring& value : values) {
            if (!firstItem) {
                json += L",";
            }
            firstItem = false;
            P1JsonEscapeAppend(json, value);
        }
        json += L"]";
    };

    appendString(L"targetLang", s.targetLang);
    appendString(L"provider", s.provider);
    appendString(L"llmBaseUrl", s.llmBaseUrl);
    appendString(L"llmModel", s.llmModel);
    appendString(L"llmKeySource", keySource);
    appendStringArray(L"glossary", s.glossary);
    appendBool(L"scopeReadme", s.scopeReadme);
    appendBool(L"scopeSettings", s.scopeSettings);
    appendBool(L"scopeModName", s.scopeModName);
    appendBool(L"scopeModSummary", s.scopeModSummary);
    appendBool(L"scopeChangelog", s.scopeChangelog);
    appendNumber(L"hoverDelayMs", s.hoverDelayMs);
    appendString(L"shortcutToggle", s.shortcutToggle);
    appendBool(L"gestureToggle", s.gestureToggle);
    appendNumber(L"batchSize", s.batchSize);
    appendNumber(L"batchChars", s.batchChars);
    appendNumber(L"batchIntervalMs", s.batchIntervalMs);
    appendNumber(L"concurrency", s.concurrency);
    appendNumber(L"timeoutMs", s.timeoutMs);
    appendBool(L"cacheEnabled", s.cacheEnabled);
    appendNumber(L"cacheTtlDays", s.cacheTtlDays);
    appendBool(L"clearCache", clearCache);
    json += L"}";
    return json;
}

// ---------------------------------------------------------------------------
// API key
// ---------------------------------------------------------------------------

struct P1KeyInfo {
    std::wstring source;  // none | credential | setting
    std::string key;      // UTF-8, empty when there is no key
    std::wstring host;    // lower-case host of llmBaseUrl, may be empty
};

// The free Edge endpoint never receives the LLM key, whatever llmBaseUrl says.
// Compared case-insensitively.
static bool P1HostExcludedFromAuth(const std::wstring& host) {
    return P1EqualsNoCase(host, L"edge.microsoft.com");
}

// Only the host decides whether a request gets the key: ports and paths are
// ignored. `llmBaseUrl` of `https://api.example.com/v1` therefore covers
// `https://api.example.com/v1/chat/completions`, any port, and any letter case
// in the host.
static bool P1HostMatchesKey(const std::wstring& requestHost,
                             const std::wstring& keyHost) {
    if (requestHost.empty() || keyHost.empty()) {
        return false;
    }
    return P1EqualsNoCase(requestHost, keyHost);
}

static std::mutex g_p1KeyMutex;
static bool g_p1KeyCacheValid = false;
static long g_p1KeyCacheGeneration = -1;
static P1KeyInfo g_p1KeyCache;
static std::atomic<long> g_p1SettingsGeneration{0};

static bool P1DecodeCredentialBlob(const CREDENTIALW* credential,
                                   std::string& out) {
    out.clear();
    if (!credential || !credential->CredentialBlob ||
        credential->CredentialBlobSize < 2) {
        return false;
    }

    const BYTE* bytes = credential->CredentialBlob;
    DWORD size = credential->CredentialBlobSize;

    if (size >= 2 && bytes[0] == 0xFF && bytes[1] == 0xFE) {
        size_t chars = (size - 2) / sizeof(wchar_t);
        std::wstring text(reinterpret_cast<const wchar_t*>(bytes + 2), chars);
        while (!text.empty() && text.back() == L'\0') {
            text.pop_back();
        }
        out = P1WideToUtf8(text);
        return !out.empty();
    }

    const BYTE* data = bytes;
    DWORD dataSize = size;
    bool utf16Likely = false;
    if (size >= 3 && bytes[0] == 0xEF && bytes[1] == 0xBB &&
        bytes[2] == 0xBF) {
        data = bytes + 3;
        dataSize = size - 3;
    } else if ((dataSize % 2) == 0) {
        // BOM-less UTF-16LE text has NUL bytes at the odd offsets. It is valid
        // UTF-8 as well (all bytes below 0x80), so the NUL pattern has to win
        // before the "try UTF-8" rule. The last UTF-16 code unit (usually the
        // NUL terminator) is excluded from the pattern check.
        size_t scanSize = dataSize >= 2 ? dataSize - 2 : dataSize;
        size_t oddNuls = 0;
        size_t evenNuls = 0;
        for (size_t i = 0; i < scanSize; ++i) {
            if (data[i] == 0) {
                if (i % 2) {
                    ++oddNuls;
                } else {
                    ++evenNuls;
                }
            }
        }
        utf16Likely = oddNuls > 0 && evenNuls == 0;
    }

    if (utf16Likely) {
        std::wstring text(reinterpret_cast<const wchar_t*>(data),
                          dataSize / sizeof(wchar_t));
        while (!text.empty() && text.back() == L'\0') {
            text.pop_back();
        }
        out = P1WideToUtf8(text);
        return !out.empty();
    }

    // No BOM and no UTF-16 pattern: UTF-8 if it decodes cleanly, otherwise
    // UTF-16 LE as a last attempt (the form cmdkey writes on some systems).
    int wideLength =
        MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS,
                            reinterpret_cast<const char*>(data),
                            static_cast<int>(dataSize), nullptr, 0);
    if (wideLength <= 0 && (dataSize % 2) == 0) {
        std::wstring text(reinterpret_cast<const wchar_t*>(data),
                          dataSize / sizeof(wchar_t));
        while (!text.empty() && text.back() == L'\0') {
            text.pop_back();
        }
        out = P1WideToUtf8(text);
        return !out.empty();
    }

    out.assign(reinterpret_cast<const char*>(data), dataSize);
    while (!out.empty() && out.back() == '\0') {
        out.pop_back();
    }
    return !out.empty();
}

static bool P1ReadCredentialKey(const std::wstring& target,
                                std::string& key,
                                DWORD& error) {
    key.clear();
    error = 0;
    PCREDENTIALW credential = nullptr;
    if (!CredReadW(target.c_str(), CRED_TYPE_GENERIC, 0, &credential) ||
        !credential) {
        error = GetLastError();
        return false;
    }

    bool ok = P1DecodeCredentialBlob(credential, key);
    if (credential->CredentialBlob && credential->CredentialBlobSize) {
        SecureZeroMemory(credential->CredentialBlob,
                         credential->CredentialBlobSize);
    }
    CredFree(credential);
    if (!ok) {
        error = ERROR_INVALID_DATA;
    }
    return ok;
}

struct P1UrlParts {
    std::wstring host;   // as written; compare case-insensitively
    std::wstring path;   // path plus query, never a fragment
    INTERNET_PORT port = INTERNET_DEFAULT_HTTPS_PORT;
    bool secure = true;
    bool explicitPort = false;  // the URL spelled the port out
};

// True when the authority part of `url` contains a colon outside IPv6
// brackets. WinHttpCrackUrl reports the effective port either way, so the
// literal form has to be inspected to distinguish `host` from `host:443`.
static bool P1UrlHasExplicitPort(const std::wstring& url) {
    size_t scheme = url.find(L"://");
    if (scheme == std::wstring::npos) {
        return false;
    }
    size_t start = scheme + 3;
    size_t end = url.find_first_of(L"/?#", start);
    if (end == std::wstring::npos) {
        end = url.size();
    }
    size_t hostStart = start;
    if (hostStart < end && url[hostStart] == L'[') {
        size_t close = url.find(L']', hostStart);
        if (close == std::wstring::npos || close >= end) {
            return false;
        }
        hostStart = close + 1;
    }
    size_t colon = url.find(L':', hostStart);
    return colon != std::wstring::npos && colon < end;
}

// Strict parser for the two relay endpoints: https only, no userinfo, no
// fragment. A fragment would silently change the request target and is not
// part of the protocol.
static bool P1ParseUrl(const std::wstring& url, P1UrlParts& parts) {
    if (url.empty() || url.size() > kP1MaxUrlChars) {
        return false;
    }
    if (url.find(L'#') != std::wstring::npos) {
        return false;
    }

    URL_COMPONENTS components{};
    components.dwStructSize = sizeof(components);
    components.dwSchemeLength = static_cast<DWORD>(-1);
    components.dwHostNameLength = static_cast<DWORD>(-1);
    components.dwUserNameLength = static_cast<DWORD>(-1);
    components.dwPasswordLength = static_cast<DWORD>(-1);
    components.dwUrlPathLength = static_cast<DWORD>(-1);
    components.dwExtraInfoLength = static_cast<DWORD>(-1);

    if (!WinHttpCrackUrl(url.c_str(), 0, 0, &components)) {
        return false;
    }
    if (components.nScheme != INTERNET_SCHEME_HTTPS) {
        return false;
    }
    if (!components.lpszHostName || components.dwHostNameLength == 0) {
        return false;
    }
    // Credentials in the URL are not part of the protocol.
    if (components.dwUserNameLength != 0 || components.dwPasswordLength != 0) {
        return false;
    }

    parts.host.assign(components.lpszHostName, components.dwHostNameLength);
    parts.secure = true;
    parts.explicitPort = P1UrlHasExplicitPort(url);
    parts.port = components.nPort;
    if (parts.port == 0) {
        parts.port = INTERNET_DEFAULT_HTTPS_PORT;
    }

    std::wstring path;
    if (components.lpszUrlPath && components.dwUrlPathLength) {
        path.assign(components.lpszUrlPath, components.dwUrlPathLength);
    }
    if (components.lpszExtraInfo && components.dwExtraInfoLength) {
        path.append(components.lpszExtraInfo, components.dwExtraInfoLength);
    }
    if (path.empty()) {
        path = L"/";
    } else if (path[0] != L'/') {
        path.insert(path.begin(), L'/');
    }
    parts.path = path;
    return true;
}

// A key with CR/LF/NUL in it must never reach the header block: it would
// either break the request or inject extra headers.
static void P1SanitizeKey(std::string& key) {
    std::string cleaned;
    cleaned.reserve(key.size());
    for (char ch : key) {
        if (ch != '\r' && ch != '\n' && ch != '\0') {
            cleaned.push_back(ch);
        }
    }
    key.swap(cleaned);
}

static P1KeyInfo P1BuildKeyInfo(const P1SettingsSnapshot& s) {
    P1KeyInfo info;
    P1UrlParts base;
    if (P1ParseUrl(s.llmBaseUrl, base)) {
        info.host = P1Lower(base.host);
    } else {
        Wh_Log(L"[p1] llmBaseUrl has no usable https host; auth injection "
               L"disabled");
    }

    // Anything but the three documented values is treated as none, with one
    // line, so a typo can never silently send a key.
    std::wstring source = P1Lower(s.keySourceSetting);
    if (source != L"credential" && source != L"setting" && source != L"none") {
        Wh_Log(L"[p1] unknown llmKeySource '%s'; treating it as none",
               s.keySourceSetting.c_str());
        source = L"none";
    }

    if (source == L"none") {
        // The user asked for no key: neither the credential nor the plaintext
        // field is read.
        info.source = L"none";
        return info;
    }

    if (source == L"credential") {
        std::string key;
        DWORD error = 0;
        if (P1ReadCredentialKey(s.credentialTarget, key, error)) {
            P1SanitizeKey(key);
            if (!key.empty()) {
                Wh_Log(L"[p1] credential target=%s found=1 bytes=%llu",
                       s.credentialTarget.c_str(),
                       static_cast<unsigned long long>(key.size()));
                info.source = L"credential";
                info.key = std::move(key);
                return info;
            }
            // A blob that decodes to nothing is a missing key as well.
            error = ERROR_INVALID_DATA;
        }
        // Exactly one line, with the target name and the Win32 error, so a
        // missing credential is diagnosable from the mod log. The key value
        // itself is never part of any log call. Then the plaintext setting is
        // used if it has anything in it.
        Wh_Log(L"[p1] credential target=%s found=0 error=%lu; plaintext "
               L"fallback=%s",
               s.credentialTarget.c_str(), error,
               s.llmApiKey.empty() ? L"absent" : L"used");
        source = L"setting";
    }

    std::string fallback = P1WideToUtf8(s.llmApiKey);
    P1SanitizeKey(fallback);
    if (fallback.empty()) {
        // No key anywhere: report none, so the page knows the key is missing.
        info.source = L"none";
        info.key.clear();
        return info;
    }
    info.source = L"setting";
    info.key = std::move(fallback);
    return info;
}

static P1KeyInfo P1GetKeyInfo() {
    long generation = g_p1SettingsGeneration.load(std::memory_order_acquire);
    std::lock_guard<std::mutex> lock(g_p1KeyMutex);
    if (g_p1KeyCacheValid && g_p1KeyCacheGeneration == generation) {
        return g_p1KeyCache;
    }
    P1SettingsSnapshot s = P1ReadSettings();
    P1KeyInfo info = P1BuildKeyInfo(s);
    g_p1KeyCache = info;
    g_p1KeyCacheGeneration = generation;
    g_p1KeyCacheValid = true;
    return info;
}

static void P1ClearKeyCache() {
    std::lock_guard<std::mutex> lock(g_p1KeyMutex);
    if (!g_p1KeyCache.key.empty()) {
        SecureZeroMemory(&g_p1KeyCache.key[0], g_p1KeyCache.key.size());
    }
    g_p1KeyCache = P1KeyInfo();
    g_p1KeyCacheValid = false;
}

// ---------------------------------------------------------------------------
// UI-thread dispatcher
//
// Win32 marshalling: the window is created on the WebView2 UI thread, workers
// only PostMessage to it, and the window procedure runs on the UI thread. A
// message-only window is never visible and is not enumerated. The class is
// registered against the mod's own module, so the window procedure cannot be
// resolved from another image.
// ---------------------------------------------------------------------------

static std::atomic<HWND> g_p1DispatchWindow{nullptr};
static std::atomic<bool> g_p1DispatchClassRegistered{false};
// Set only when kP1MsgDestroyWindow destroyed the window and unregistered its
// class (or no class was registered). The engine thread checks it after the
// SendMessageW returns; false means the teardown did not finish and the unload
// must block.
static std::atomic<bool> g_p1DestroyCompleted{false};
static std::atomic<bool> g_p1PageReady{false};
static std::atomic<bool> g_p1SettingsDirty{false};
static std::atomic<bool> g_p1DispatchFailureLogged{false};

// Dispatch-window protocol. The lock guards the state and the creator thread
// id; the HWND stays atomic because workers only post to it and never need the
// lock. The condition variable is signalled whenever a creation settles
// (published or abandoned), so the unloader waits for an in-flight creation
// instead of racing it.
//
//   None     - no window; a creation may be admitted while Running.
//   Creating - one thread owns creation and is outside the lock; every other
//              caller waits, the creator itself must not.
//   Ready    - the HWND is published and stays valid until the UI-thread
//              teardown destroys it and resets the state to None.
//
// Only the creating thread cleans up an unpublished window. The unloader never
// touches a window it did not see published.
enum class P1DispatchState : int {
    None = 0,
    Creating,
    Ready,
};

static SRWLOCK g_p1DispatchLock = SRWLOCK_INIT;
static CONDITION_VARIABLE g_p1DispatchCv = CONDITION_VARIABLE_INIT;
static P1DispatchState g_p1DispatchState = P1DispatchState::None;  // lock
static DWORD g_p1DispatchCreatorThreadId = 0;                      // lock

// Handle of the mod DLL itself, obtained from a function address so it is the
// module that owns P1DispatchWndProc even when the host image differs.
static HMODULE g_modModule = nullptr;

static void P1NoteUiThread() {
    DWORD current = GetCurrentThreadId();
    DWORD expected = 0;
    g_p1UiThreadId.compare_exchange_strong(expected, current);
}

static void P1DrainRepliesOnUiThread(bool postReplies);
static void P1PushSettingsToAll(const wchar_t* reason);
static void P1RunTeardownOnUiThread();

static LRESULT CALLBACK P1DispatchWndProc(HWND hwnd,
                                          UINT message,
                                          WPARAM wParam,
                                          LPARAM lParam) {
    (void)wParam;
    (void)lParam;
    try {
        switch (message) {
            case kP1MsgDrainReplies: {
                // While Running a drained reply is posted to the page; while
                // Stopping it is only released on this thread.
                P1Lifecycle state =
                    g_p1Lifecycle.load(std::memory_order_acquire);
                if (state == P1Lifecycle::Running) {
                    P1DrainRepliesOnUiThread(true);
                } else if (state == P1Lifecycle::Stopping) {
                    P1DrainRepliesOnUiThread(false);
                }
                return 0;
            }
            case kP1MsgSettingsChanged:
                if (!P1IsRunning()) {
                    return 0;
                }
                P1DrainRepliesOnUiThread(true);
                if (g_p1SettingsDirty.exchange(false) &&
                    g_p1PageReady.load()) {
                    P1PushSettingsToAll(L"settings-changed");
                }
                return 0;
            case kP1MsgTeardown:
                if (g_p1Lifecycle.load(std::memory_order_acquire) !=
                    P1Lifecycle::Stopping) {
                    return 0;
                }
                P1RunTeardownOnUiThread();
                return 0;
            case kP1MsgDestroyWindow: {
                if (g_p1Lifecycle.load(std::memory_order_acquire) !=
                    P1Lifecycle::Stopping) {
                    return 0;
                }
                // A late AddScript completion or finalizer can queue a release
                // and post kP1MsgDrainReplies; a sent message can overtake that
                // posted one, so drain once before the window goes away.
                P1DrainRepliesOnUiThread(false);

                bool destroyed = DestroyWindow(hwnd) != FALSE;
                if (!destroyed) {
                    Wh_Log(L"[p1] DestroyWindow(dispatch window) failed (%lu)",
                           GetLastError());
                }

                bool classGone = true;
                if (g_p1DispatchClassRegistered.load(
                        std::memory_order_acquire)) {
                    if (!g_modModule) {
                        Wh_Log(L"[p1] dispatch window class is registered but "
                               L"the module handle is missing");
                        classGone = false;
                    } else if (!UnregisterClassW(kP1DispatchClass,
                                                 g_modModule)) {
                        Wh_Log(L"[p1] UnregisterClassW(dispatch window class) "
                               L"failed (%lu)",
                               GetLastError());
                        classGone = false;
                    } else {
                        g_p1DispatchClassRegistered.store(
                            false, std::memory_order_release);
                    }
                }

                if (!destroyed || !classGone) {
                    return 0;
                }

                g_p1DispatchWindow.store(nullptr, std::memory_order_release);
                AcquireSRWLockExclusive(&g_p1DispatchLock);
                g_p1DispatchState = P1DispatchState::None;
                g_p1DispatchCreatorThreadId = 0;
                WakeAllConditionVariable(&g_p1DispatchCv);
                ReleaseSRWLockExclusive(&g_p1DispatchLock);
                g_p1DestroyCompleted.store(true, std::memory_order_release);
                g_p1Lifecycle.store(P1Lifecycle::Stopped,
                                    std::memory_order_release);
                Wh_Log(L"[p1] dispatch window destroyed");
                return 0;
            }
            default:
                break;
        }
    } catch (...) {
    }
    return DefWindowProcW(hwnd, message, wParam, lParam);
}

// Called by the creating thread on every path that does not publish a window.
// Must not be called with g_p1DispatchLock held.
static void P1AbortDispatchCreation() {
    AcquireSRWLockExclusive(&g_p1DispatchLock);
    g_p1DispatchState = P1DispatchState::None;
    g_p1DispatchCreatorThreadId = 0;
    WakeAllConditionVariable(&g_p1DispatchCv);
    ReleaseSRWLockExclusive(&g_p1DispatchLock);
}

static bool P1EnsureDispatchWindow() {
    DWORD currentThread = GetCurrentThreadId();
    DWORD uiThread = g_p1UiThreadId.load(std::memory_order_acquire);
    if (uiThread == 0 || currentThread != uiThread) {
        return false;
    }
    if (!g_modModule) {
        Wh_Log(L"[p1] mod module handle is missing; the dispatcher window "
               L"cannot be created");
        return false;
    }

    AcquireSRWLockExclusive(&g_p1DispatchLock);
    for (;;) {
        if (g_p1DispatchState == P1DispatchState::Ready) {
            HWND window = g_p1DispatchWindow.load(std::memory_order_acquire);
            ReleaseSRWLockExclusive(&g_p1DispatchLock);
            return window != nullptr;
        }
        if (g_p1DispatchState == P1DispatchState::Creating) {
            if (g_p1DispatchCreatorThreadId == currentThread) {
                // Re-entrant call from the creating thread. Waiting would
                // deadlock: this thread is the one that settles the state.
                ReleaseSRWLockExclusive(&g_p1DispatchLock);
                return false;
            }
            // Another thread owns the creation. Wait until it publishes or
            // abandons the window, then re-evaluate.
            SleepConditionVariableSRW(&g_p1DispatchCv, &g_p1DispatchLock,
                                      INFINITE, 0);
            continue;
        }
        // None: admit a creation only while the mod is running.
        if (g_p1Lifecycle.load(std::memory_order_acquire) !=
            P1Lifecycle::Running) {
            ReleaseSRWLockExclusive(&g_p1DispatchLock);
            return false;
        }
        g_p1DispatchState = P1DispatchState::Creating;
        g_p1DispatchCreatorThreadId = currentThread;
        break;
    }
    ReleaseSRWLockExclusive(&g_p1DispatchLock);

    // RegisterClassExW and CreateWindowExW run outside the lock: either can
    // dispatch messages, and a re-entrant P1EnsureDispatchWindow must observe
    // Creating instead of blocking on a lock this thread holds.
    if (!g_p1DispatchClassRegistered.load(std::memory_order_acquire)) {
        WNDCLASSEXW windowClass{};
        windowClass.cbSize = sizeof(windowClass);
        windowClass.lpfnWndProc = &P1DispatchWndProc;
        windowClass.hInstance = g_modModule;
        windowClass.lpszClassName = kP1DispatchClass;
        if (!RegisterClassExW(&windowClass)) {
            // Any failure, ERROR_CLASS_ALREADY_EXISTS included, means this
            // class is not ours: without the window the mod cannot be torn
            // down safely, so injection is skipped by the caller.
            Wh_Log(L"[p1] RegisterClassExW failed (%lu)", GetLastError());
            P1AbortDispatchCreation();
            return false;
        }
        g_p1DispatchClassRegistered.store(true, std::memory_order_release);
    }

    HWND window = CreateWindowExW(0, kP1DispatchClass, L"", 0, 0, 0, 0, 0,
                                  HWND_MESSAGE, nullptr, g_modModule, nullptr);
    if (!window) {
        DWORD error = GetLastError();
        Wh_Log(L"[p1] CreateWindowExW failed (%lu)", error);
        // Do not leave a registered class pointing into this image when the
        // window could not be created: the caller must be able to fall back to
        // the real export with nothing owned by the mod. The class stays
        // registered if it cannot actually be unregistered; the no-window
        // unload path then retries and blocks on failure.
        if (g_p1DispatchClassRegistered.load(std::memory_order_acquire)) {
            if (g_modModule &&
                UnregisterClassW(kP1DispatchClass, g_modModule)) {
                g_p1DispatchClassRegistered.store(false,
                                                 std::memory_order_release);
            } else {
                Wh_Log(L"[p1] UnregisterClassW after a failed window creation "
                       L"failed (%lu); keeping the class registered",
                       GetLastError());
            }
        }
        P1AbortDispatchCreation();
        return false;
    }

    AcquireSRWLockExclusive(&g_p1DispatchLock);
    if (g_p1Lifecycle.load(std::memory_order_acquire) ==
        P1Lifecycle::Running) {
        g_p1DispatchWindow.store(window, std::memory_order_release);
        g_p1DispatchState = P1DispatchState::Ready;
        g_p1DispatchCreatorThreadId = 0;
        WakeAllConditionVariable(&g_p1DispatchCv);
        ReleaseSRWLockExclusive(&g_p1DispatchLock);
        return true;
    }
    ReleaseSRWLockExclusive(&g_p1DispatchLock);

    // Shutdown started while the window was being created. The creator owns
    // this cleanup unless it cannot destroy the window: then the window is
    // published instead, so the unloader can reach it and retry the destroy.
    Wh_Log(L"[p1] dispatch window creation cancelled: shutdown started");
    if (!DestroyWindow(window)) {
        Wh_Log(L"[p1] DestroyWindow(cancelled window) failed (%lu); publishing "
               L"it so the unload can retry the destroy",
               GetLastError());
        // Publishing keeps the window reachable: the unload posts the teardown
        // to it and its destroy handler gets a second chance. If that destroy
        // also fails, the engine blocks on g_p1DestroyCompleted.
        AcquireSRWLockExclusive(&g_p1DispatchLock);
        g_p1DispatchWindow.store(window, std::memory_order_release);
        g_p1DispatchState = P1DispatchState::Ready;
        g_p1DispatchCreatorThreadId = 0;
        WakeAllConditionVariable(&g_p1DispatchCv);
        ReleaseSRWLockExclusive(&g_p1DispatchLock);
        return false;
    }
    if (g_p1DispatchClassRegistered.load(std::memory_order_acquire)) {
        if (g_modModule && UnregisterClassW(kP1DispatchClass, g_modModule)) {
            g_p1DispatchClassRegistered.store(false,
                                             std::memory_order_release);
        } else {
            Wh_Log(L"[p1] UnregisterClassW(cancelled window class) failed "
                   L"(%lu); keeping the class registered",
                   GetLastError());
        }
    }
    P1AbortDispatchCreation();
    return false;
}

static void P1WakeUi() {
    HWND window = g_p1DispatchWindow.load(std::memory_order_acquire);
    if (window) {
        PostMessageW(window, kP1MsgDrainReplies, 0, 0);
    }
}

static void P1RequestSettingsPush() {
    if (!P1IsRunning()) {
        return;
    }
    g_p1SettingsDirty.store(true);

    DWORD uiThread = g_p1UiThreadId.load(std::memory_order_acquire);
    if (uiThread != 0 && GetCurrentThreadId() == uiThread) {
        if (g_p1PageReady.load()) {
            g_p1SettingsDirty.store(false);
            P1PushSettingsToAll(L"settings-changed");
        }
        return;
    }

    HWND window = g_p1DispatchWindow.load(std::memory_order_acquire);
    if (window) {
        PostMessageW(window, kP1MsgSettingsChanged, 0, 0);
    }
}

// Runs steps 1-4 of the teardown on the WebView2 UI thread, inside the
// dispatch window's WndProc. It sets g_p1TeardownCompleted only after
// P0RestoreEnvironment(true) returned, then asks P1MaybeSignalTeardownLocked to
// signal g_p1TeardownEvent if both counters are already zero. There is no
// message pump and no window destroy here: the UI thread's own message loop
// keeps dispatching WebView2 completions after this function returns, and those
// completions release the last wrappers and held AddScript references.
static void P1RunTeardownOnUiThread() {
    if (g_p1TeardownInProgress.load(std::memory_order_acquire)) {
        // A second teardown request is a no-op; the first one owns the work.
        Wh_Log(L"[p1] duplicate teardown request ignored: teardown already in "
               L"progress");
        return;
    }
    g_p1TeardownInProgress.store(true, std::memory_order_release);
    Wh_Log(L"[p1] UI-thread teardown started");

    try {
        // Queued worker references are released here, on the UI thread, before
        // the webview itself goes away.
        P1DrainRepliesOnUiThread(false);

        void* webView = P0AcquireWebView();
        P0ComRef webViewRef;
        webViewRef.object = webView;

        // 1) The page restores every translated text and stops; it must not
        //    call into the mod again. No completion handler is needed.
        if (webView) {
            void* postFn = nullptr;
            if (P0GetValidatedSlot(webView, kWebViewSlotPostWebMessageAsString,
                                   &postFn)) {
                std::wstring message = kP1Tag;
                message.push_back(0x1f);
                message += L"teardown";
                HRESULT hr = reinterpret_cast<FnPostWebMessageAsString>(postFn)(
                    webView, message.c_str());
                if (FAILED(hr)) {
                    Wh_Log(L"[p1] teardown message could not be posted (%08X)",
                           static_cast<unsigned>(hr));
                }
            } else {
                Wh_Log(L"[p1] teardown message skipped: post slot invalid");
            }

            // 2) The document-created script is removed by the id its AddScript
            //    completion returned. If the completion has not arrived yet,
            //    the completion itself removes the script when it runs; the
            //    AddScript reference counter keeps the engine thread waiting
            //    until that happened.
            P0RemoveRegisteredScript(webView);
        }

        // 3) Navigation and web-message handlers are removed and every stored
        //    reference is released.
        P0CleanupWebView();

        // 4) The environment vtable is restored before anything else can call
        //    into this image.
        P0RestoreEnvironment(true);
    } catch (...) {
        // The cleanup is incomplete. No completion flag is set, so the engine
        // thread keeps waiting; nothing here can pretend the callbacks are
        // gone.
        Wh_Log(L"[p1] exception during the UI-thread teardown");
        return;
    }

    g_p1TeardownCompleted.store(true, std::memory_order_release);

    AcquireSRWLockExclusive(&g_p0OutstandingLock);
    P1MaybeSignalTeardownLocked();
    ReleaseSRWLockExclusive(&g_p0OutstandingLock);

    Wh_Log(L"[p1] UI-thread teardown complete");
}

// Blocks until no dispatch-window creation is in flight. Wh_ModBeforeUninit
// calls this after the lifecycle became Stopping, so a creator that is still
// running will observe Stopping in its publish branch, destroy its own window
// and report None. The unloader must never set Stopped while a creation is
// still in flight.
static void P1WaitForDispatchCreation() {
    DWORD currentThread = GetCurrentThreadId();
    AcquireSRWLockExclusive(&g_p1DispatchLock);
    while (g_p1DispatchState == P1DispatchState::Creating) {
        if (g_p1DispatchCreatorThreadId == currentThread) {
            // The unload re-entered the creator on its own thread. Waiting
            // would deadlock; the creator's publish branch will see Stopping
            // and clean up as soon as this call returns to it.
            Wh_Log(L"[p1] DEFECT: unload re-entered dispatch-window creation "
                   L"on the creating thread");
            break;
        }
        SleepConditionVariableSRW(&g_p1DispatchCv, &g_p1DispatchLock,
                                  INFINITE, 0);
    }
    ReleaseSRWLockExclusive(&g_p1DispatchLock);
}

// A cleanup step that must succeed before the image may unload failed. The
// mod takes no loader reference and Windhawk frees the DLL as soon as uninit
// returns, so the only outcome that cannot run code from an unmapped image is
// to not return.
static void P1BlockUnload(const wchar_t* reason) {
    Wh_Log(L"[p1] DEFECT: %s; the unload cannot proceed, blocking", reason);
    Sleep(INFINITE);
}

// Called from Wh_ModBeforeUninit (engine thread) after the state became
// Stopping and P1WaitForDispatchCreation returned. It asks the UI thread for
// the teardown, waits until no wrapper and no held AddScript reference is left,
// and only then destroys the dispatch window with a blocking SendMessageW.
//
// The send proves that the UI thread's own message loop dispatched the destroy
// handler and came back to the sender after the teardown handler completed.
// Because this module's entry points do not pump messages, the UI thread cannot
// be inside one when it returns here. It does not prove that every outer stack
// in this image has exited; the callback counters and the UI-thread check are
// what cover the callback frames.
static void P1TeardownUiThread() {
    if (g_p1Lifecycle.load(std::memory_order_acquire) == P1Lifecycle::Stopped) {
        return;
    }

    HWND window = nullptr;
    AcquireSRWLockExclusive(&g_p1DispatchLock);
    window = g_p1DispatchWindow.load(std::memory_order_acquire);
    ReleaseSRWLockExclusive(&g_p1DispatchLock);

    if (!window) {
        // No creation is in flight (Wh_ModBeforeUninit waited for it), so
        // either none was ever admitted or the creator cancelled its own
        // window when it saw Stopping. A wrapper is handed to WebView2 only
        // after the window is Ready, so there is nothing to tear down.
        // P0RestoreEnvironment is defensive: it is a no-op when no environment
        // was patched.
        Wh_Log(L"[p1] no dispatch window is published at unload (no creation "
               L"in flight); nothing to tear down");
        try {
            P0RestoreEnvironment(false);
        } catch (...) {
            Wh_Log(L"[p1] exception while restoring the environment without a "
                   L"dispatch window");
            P1BlockUnload(
                L"restoring the environment without a dispatch window failed");
        }
        // The creator may have destroyed its window but failed to unregister
        // the class. Any thread may unregister a class, so retry here; a class
        // still pointing into this image blocks the unload.
        if (g_p1DispatchClassRegistered.load(std::memory_order_acquire)) {
            bool unregistered = false;
            if (g_modModule) {
                unregistered =
                    UnregisterClassW(kP1DispatchClass, g_modModule) != FALSE;
            }
            if (!unregistered) {
                Wh_Log(L"[p1] UnregisterClassW(dispatch window class) without "
                       L"a window failed (%lu)",
                       GetLastError());
                P1BlockUnload(
                    L"the dispatch window class could not be unregistered");
            }
            g_p1DispatchClassRegistered.store(false,
                                             std::memory_order_release);
        }
        g_p1Lifecycle.store(P1Lifecycle::Stopped, std::memory_order_release);
        return;
    }

    if (!g_p1TeardownEvent) {
        P1BlockUnload(L"the teardown event is missing");
    }

    long callbacks = 0;
    long addScriptRefs = 0;
    AcquireSRWLockExclusive(&g_p0OutstandingLock);
    g_p1TeardownRequested.store(true, std::memory_order_release);
    callbacks = g_p0OutstandingCallbacks;
    addScriptRefs = g_p0OutstandingAddScriptRefs;
    ReleaseSRWLockExclusive(&g_p0OutstandingLock);

    Wh_Log(L"[p1] UI-thread teardown requested (window=%p outstanding=%ld "
           L"addScriptRefs=%ld)",
           window, callbacks, addScriptRefs);

    if (!PostMessageW(window, kP1MsgTeardown, 0, 0)) {
        // A posted message that never arrives would leave the wait below
        // forever, so fall back to a blocking send. The handler runs on the UI
        // thread either way.
        Wh_Log(L"[p1] teardown message could not be posted (%lu); sending it "
               L"instead",
               GetLastError());
        SendMessageW(window, kP1MsgTeardown, 0, 0);
        if (!IsWindow(window)) {
            P1BlockUnload(
                L"the dispatch window is gone before the teardown ran");
        }
    }
    DWORD wait = WaitForSingleObject(g_p1TeardownEvent, INFINITE);
    if (wait != WAIT_OBJECT_0) {
        Wh_Log(L"[p1] WaitForSingleObject(teardown event) returned %lu", wait);
        P1BlockUnload(L"waiting for the teardown event failed");
    }

    AcquireSRWLockExclusive(&g_p0OutstandingLock);
    callbacks = g_p0OutstandingCallbacks;
    addScriptRefs = g_p0OutstandingAddScriptRefs;
    ReleaseSRWLockExclusive(&g_p0OutstandingLock);
    Wh_Log(L"[p1] teardown event signalled (outstanding=%ld addScriptRefs=%ld); "
           L"destroying the dispatch window",
           callbacks, addScriptRefs);

    SendMessageW(window, kP1MsgDestroyWindow, 0, 0);
    if (!g_p1DestroyCompleted.load(std::memory_order_acquire)) {
        P1BlockUnload(L"the dispatch window could not be destroyed");
    }
    Wh_Log(L"[p1] UI-thread teardown returned");
}

// ---------------------------------------------------------------------------
// HTTP request/response structures
// ---------------------------------------------------------------------------

struct P1HttpRequest {
    unsigned long long id = 0;
    std::wstring method;
    std::wstring url;
    std::string body;
    unsigned long long timeoutMs = 20000;
    std::string apiKey;       // never passed to a log call; header only
    std::wstring apiKeyHost;  // lower-case host the key is allowed for
    void* webView = nullptr;  // owned reference, released on the UI thread
};

struct P1HttpReply {
    unsigned long long id = 0;
    int status = 0;
    unsigned long error = 0;
    std::string body;
    void* webView = nullptr;  // owned reference, released on the UI thread
    // True for a queue item that only carries a webview reference to release
    // on the UI thread (a worker or the engine thread must never release one).
    bool releaseOnly = false;
};

// Pool lifecycle. The state is changed under g_p1HttpLock; the join itself is
// done outside it because the workers need that lock to observe Stopping.
enum class P1PoolState : int {
    NotStarted = 0,
    Starting,
    Running,
    Stopping,
    Stopped,
};

static SRWLOCK g_p1HttpLock = SRWLOCK_INIT;
static CONDITION_VARIABLE g_p1HttpCv = CONDITION_VARIABLE_INIT;
static std::deque<P1HttpRequest*> g_p1HttpQueue;
// Fixed-size storage reserved before any thread is created: no allocation can
// fail between CreateThread and storing the handle, and no handle can be lost.
// Guarded by g_p1HttpLock.
static HANDLE g_p1Workers[kP1HttpWorkers] = {};
static size_t g_p1WorkerCount = 0;
static P1PoolState g_p1PoolState = P1PoolState::NotStarted;
// Sticky: a join failed, so a worker may still be running. Checked before the
// pool state, because a later stop must not read "Stopped" as proof.
static std::atomic<bool> g_p1WorkerStopFailed{false};

static SRWLOCK g_p1ReplyLock = SRWLOCK_INIT;
static std::deque<P1HttpReply*> g_p1ReplyQueue;

static void P1ReleaseWebViewRef(void* webView) {
    if (!webView) {
        return;
    }
    auto release =
        reinterpret_cast<FnRelease>(P0GetRawSlot(webView, kSlotRelease));
    if (release) {
        release(webView);
    }
}

// Queues a webview reference for release on the UI thread. Used by workers and
// by the engine thread; the reply drain releases it. If the queue cannot be
// reached the reference is deliberately leaked rather than released on the
// wrong thread.
static void P1QueueWebViewRelease(void* webView) {
    if (!webView) {
        return;
    }
    auto* item = new (std::nothrow) P1HttpReply();
    if (!item) {
        Wh_Log(L"[p1] out of memory while queueing a webview release; the "
               L"reference is leaked");
        return;
    }
    item->releaseOnly = true;
    item->webView = webView;

    bool stored = false;
    AcquireSRWLockExclusive(&g_p1ReplyLock);
    try {
        g_p1ReplyQueue.push_back(item);
        stored = true;
    } catch (...) {
    }
    ReleaseSRWLockExclusive(&g_p1ReplyLock);

    if (!stored) {
        Wh_Log(L"[p1] reply queue rejected a webview release; the reference is "
               L"leaked");
        delete item;
        return;
    }
    P1WakeUi();
}

// Transfers the webview reference held by an AddScript completion handler to
// the UI thread's release queue. Called from the completion and from the
// handler finalizer, both on the UI thread under the WebView2 contract; it
// never calls Release directly.
static void P0ReleaseScriptWebView(P0ScriptHandler* handler) {
    if (!handler || !handler->webView) {
        return;
    }
    void* webView = handler->webView;
    handler->webView = nullptr;
    P1QueueWebViewRelease(webView);
    // The decrement is the teardown notification: it latches an off-UI-thread
    // release, and when it empties the counters P1MaybeSignalTeardownLocked
    // signals the event if the teardown completed on the UI thread.
    P0CountAddScriptRefReleased();
}

static std::wstring P1BuildHttpReplyMessage(unsigned long long id,
                                           int status,
                                           unsigned long error,
                                           const std::string& bodyUtf8) {
    std::wstring text;
    text.reserve(bodyUtf8.size() + 96);
    text += kP1Tag;
    text.push_back(0x1f);
    text += L"http";
    text.push_back(0x1f);
    text += std::to_wstring(id);
    text.push_back(0x1f);
    text += std::to_wstring(status);
    text.push_back(0x1f);
    text += std::to_wstring(error);
    text.push_back(0x1f);
    text += P1Utf8ToWide(bodyUtf8);
    return text;
}

static void P1PostHttpReply(void* webView,
                            unsigned long long id,
                            int status,
                            unsigned long error,
                            const std::string& bodyUtf8) {
    if (!webView) {
        return;
    }
    void* postFn = nullptr;
    if (!P0GetValidatedSlot(webView, kWebViewSlotPostWebMessageAsString,
                            &postFn)) {
        return;
    }
    std::wstring text = P1BuildHttpReplyMessage(id, status, error, bodyUtf8);
    HRESULT hr = reinterpret_cast<FnPostWebMessageAsString>(postFn)(
        webView, text.c_str());
    if (FAILED(hr)) {
        Wh_Log(L"[p1] http reply id=%llu could not be posted (hr=%08X)", id,
               static_cast<unsigned>(hr));
    }
}

// Runs on the UI thread. `postReplies` is false while Stopping: the page is
// going away, only the webview references still have to be released here.
static void P1DrainRepliesOnUiThread(bool postReplies) {
    std::deque<P1HttpReply*> replies;
    AcquireSRWLockExclusive(&g_p1ReplyLock);
    replies.swap(g_p1ReplyQueue);
    ReleaseSRWLockExclusive(&g_p1ReplyLock);

    for (P1HttpReply* reply : replies) {
        if (!reply) {
            continue;
        }
        if (postReplies && !reply->releaseOnly) {
            try {
                P1PostHttpReply(reply->webView, reply->id, reply->status,
                                reply->error, reply->body);
            } catch (...) {
                Wh_Log(L"[p1] exception while posting http reply id=%llu",
                       reply->id);
            }
        }
        P1ReleaseWebViewRef(reply->webView);
        delete reply;
    }
}

// ---------------------------------------------------------------------------
// HTTP worker pool
// ---------------------------------------------------------------------------

// The redirect-policy failure is logged once per process: it is a configuration
// or WinHTTP failure, not a per-request event.
static std::atomic<bool> g_p1RedirectPolicyLogged{false};

static P1HttpReply* P1PerformHttp(const P1HttpRequest* request) {
    auto* reply = new (std::nothrow) P1HttpReply();
    if (!reply) {
        return nullptr;
    }
    reply->id = request->id;
    reply->webView = request->webView;

    unsigned long long started = GetTickCount64();
    std::wstring hostForLog = L"(invalid)";
    std::wstring methodForLog = request->method;
    const wchar_t* stage = L"parse-url";
    bool truncated = false;

    try {
        P1UrlParts parts;
        if (!P1ParseUrl(request->url, parts)) {
            reply->error = ERROR_INVALID_PARAMETER;
            Wh_Log(L"[p1] http id=%llu method=%s host=%s stage=%s error=%lu "
                   L"ms=%llu",
                   request->id, methodForLog.c_str(), hostForLog.c_str(), stage,
                   reply->error, GetTickCount64() - started);
            return reply;
        }
        hostForLog = P1ClipForLog(parts.host, 80);

        struct Handles {
            HINTERNET session = nullptr;
            HINTERNET connect = nullptr;
            HINTERNET request = nullptr;
            ~Handles() {
                if (request) {
                    WinHttpCloseHandle(request);
                }
                if (connect) {
                    WinHttpCloseHandle(connect);
                }
                if (session) {
                    WinHttpCloseHandle(session);
                }
            }
        } handles;

        stage = L"open";
        handles.session = WinHttpOpen(
            L"WindhawkUITranslator/1.0", WINHTTP_ACCESS_TYPE_AUTOMATIC_PROXY,
            WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
        if (!handles.session) {
            reply->error = GetLastError();
            throw reply->error;
        }

        int timeout = static_cast<int>(request->timeoutMs);
        WinHttpSetTimeouts(handles.session, timeout, timeout, timeout, timeout);

        stage = L"connect";
        handles.connect = WinHttpConnect(handles.session, parts.host.c_str(),
                                         parts.port, 0);
        if (!handles.connect) {
            reply->error = GetLastError();
            throw reply->error;
        }

        stage = L"open-request";
        handles.request = WinHttpOpenRequest(
            handles.connect, request->method.c_str(), parts.path.c_str(),
            nullptr, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES,
            parts.secure ? WINHTTP_FLAG_SECURE : 0);
        if (!handles.request) {
            reply->error = GetLastError();
            throw reply->error;
        }

        // Redirects are never followed: the relay accepts exactly one URL, so
        // a 3xx is an error and must not turn into a request to another host.
        // The policy is part of the allowlist, so failing to set it aborts the
        // request instead of falling back to WinHTTP's default policy.
        DWORD redirectPolicy = WINHTTP_OPTION_REDIRECT_POLICY_NEVER;
        if (!WinHttpSetOption(handles.request, WINHTTP_OPTION_REDIRECT_POLICY,
                              &redirectPolicy, sizeof(redirectPolicy))) {
            reply->error = GetLastError();
            if (!g_p1RedirectPolicyLogged.exchange(true,
                                                   std::memory_order_relaxed)) {
                Wh_Log(L"[p1] http id=%llu redirect policy option failed (%lu); "
                       L"request aborted",
                       request->id, reply->error);
            }
            return reply;
        }

        DWORD decompression = WINHTTP_DECOMPRESSION_FLAG_ALL;
        if (!WinHttpSetOption(handles.request, WINHTTP_OPTION_DECOMPRESSION,
                              &decompression, sizeof(decompression))) {
            Wh_Log(L"[p1] http id=%llu decompression option failed (%lu); "
                   L"continuing",
                   request->id, GetLastError());
        }

        stage = L"headers";
        // The page sends no headers at all; the protocol fixes the content
        // type. The key lives in P1HttpRequest::apiKey and in this header block,
        // and nowhere else: no log line below includes it.
        std::wstring headerBlock = L"Content-Type: application/json\r\n";

        bool hostMatchesKey =
            !request->apiKey.empty() &&
            P1HostMatchesKey(parts.host, request->apiKeyHost);
        // The free endpoint never takes the key, even when llmBaseUrl is
        // misconfigured to point at it.
        if (hostMatchesKey && P1HostExcludedFromAuth(parts.host)) {
            hostMatchesKey = false;
        }
        if (hostMatchesKey && parts.secure) {
            headerBlock += L"Authorization: Bearer ";
            headerBlock += P1Utf8ToWide(request->apiKey);
            headerBlock += L"\r\n";
            Wh_Log(L"[p1] injected auth for host=%s", hostForLog.c_str());
        } else if (hostMatchesKey) {
            Wh_Log(L"[p1] not injecting auth for non-https host=%s",
                   hostForLog.c_str());
        }

        const wchar_t* headers = headerBlock.c_str();
        DWORD headersLength = static_cast<DWORD>(-1);

        LPVOID body = nullptr;
        DWORD bodyLength = 0;
        if (request->method != L"GET" && !request->body.empty()) {
            body = const_cast<char*>(request->body.data());
            bodyLength = static_cast<DWORD>(request->body.size());
        }

        stage = L"send";
        if (!WinHttpSendRequest(handles.request, headers, headersLength, body,
                                bodyLength, bodyLength, 0)) {
            reply->error = GetLastError();
            throw reply->error;
        }

        stage = L"receive";
        if (!WinHttpReceiveResponse(handles.request, nullptr)) {
            reply->error = GetLastError();
            throw reply->error;
        }

        stage = L"status";
        DWORD statusCode = 0;
        DWORD statusSize = sizeof(statusCode);
        if (!WinHttpQueryHeaders(handles.request,
                                 WINHTTP_QUERY_STATUS_CODE |
                                     WINHTTP_QUERY_FLAG_NUMBER,
                                 WINHTTP_HEADER_NAME_BY_INDEX, &statusCode,
                                 &statusSize, WINHTTP_NO_HEADER_INDEX)) {
            reply->error = GetLastError();
            throw reply->error;
        }
        reply->status = static_cast<int>(statusCode);
        if (statusCode >= 300 && statusCode <= 399) {
            reply->error = ERROR_WINHTTP_REDIRECT_FAILED;
            Wh_Log(L"[p1] http id=%llu method=%s host=%s status=%d error=%lu "
                   L"redirect=rejected ms=%llu",
                   request->id, methodForLog.c_str(), hostForLog.c_str(),
                   reply->status, reply->error,
                   GetTickCount64() - started);
            return reply;
        }

        stage = L"read";
        std::string bodyText;
        bodyText.reserve(64 * 1024);
        for (;;) {
            DWORD available = 0;
            if (!WinHttpQueryDataAvailable(handles.request, &available)) {
                reply->error = GetLastError();
                break;
            }
            if (available == 0) {
                break;
            }

            size_t room = kP1MaxReplyBytes - bodyText.size();
            if (room == 0) {
                DWORD more = 0;
                if (WinHttpQueryDataAvailable(handles.request, &more) &&
                    more > 0) {
                    truncated = true;
                }
                break;
            }

            char buffer[16384];
            DWORD toRead = static_cast<DWORD>(
                std::min<size_t>(room, sizeof(buffer)));
            DWORD read = 0;
            if (!WinHttpReadData(handles.request, buffer, toRead, &read)) {
                reply->error = GetLastError();
                break;
            }
            if (read == 0) {
                break;
            }
            bodyText.append(buffer, read);
            if (bodyText.size() >= kP1MaxReplyBytes) {
                DWORD more = 0;
                if (WinHttpQueryDataAvailable(handles.request, &more) &&
                    more > 0) {
                    truncated = true;
                }
                break;
            }
        }
        reply->body = std::move(bodyText);

        if (reply->error != 0) {
            // Protocol: the body is empty when the request did not complete
            // cleanly, even if a partial response had been read.
            reply->body.clear();
            stage = L"read";
        } else {
            stage = L"done";
        }

        unsigned long long elapsed = GetTickCount64() - started;
        Wh_Log(L"[p1] http id=%llu method=%s host=%s status=%d error=%lu "
               L"bytes=%llu ms=%llu%s",
               request->id, methodForLog.c_str(), hostForLog.c_str(),
               reply->status, reply->error,
               static_cast<unsigned long long>(reply->body.size()), elapsed,
               truncated ? L" truncated=1" : L"");
        if (truncated) {
            Wh_Log(L"[p1] http id=%llu response truncated at %llu bytes",
                   request->id,
                   static_cast<unsigned long long>(kP1MaxReplyBytes));
        }
        return reply;
    } catch (...) {
        if (reply->error == 0) {
            reply->error = ERROR_NOT_ENOUGH_MEMORY;
        }
        unsigned long long elapsed = GetTickCount64() - started;
        Wh_Log(L"[p1] http id=%llu method=%s host=%s status=%d stage=%s "
               L"error=%lu bytes=%llu ms=%llu",
               request->id, methodForLog.c_str(), hostForLog.c_str(),
               reply->status, stage, reply->error,
               static_cast<unsigned long long>(reply->body.size()), elapsed);
        return reply;
    }
}

static DWORD WINAPI P1HttpWorkerProc(void*) {
    for (;;) {
        P1HttpRequest* request = nullptr;

        AcquireSRWLockExclusive(&g_p1HttpLock);
        while (g_p1PoolState == P1PoolState::Running &&
               g_p1HttpQueue.empty()) {
            SleepConditionVariableSRW(&g_p1HttpCv, &g_p1HttpLock, INFINITE, 0);
        }
        if (g_p1PoolState != P1PoolState::Running) {
            ReleaseSRWLockExclusive(&g_p1HttpLock);
            break;
        }
        request = g_p1HttpQueue.front();
        g_p1HttpQueue.pop_front();
        ReleaseSRWLockExclusive(&g_p1HttpLock);

        // Once Stopping was set no new request may be performed; a request
        // that was already in flight runs to completion. Its WinHTTP timeouts
        // are per phase, not a total deadline.
        if (!request || !P1IsRunning()) {
            if (request) {
                P1QueueWebViewRelease(request->webView);
                delete request;
            }
            continue;
        }

        P1HttpReply* reply = nullptr;
        try {
            reply = P1PerformHttp(request);
        } catch (...) {
            reply = nullptr;
        }
        if (!reply) {
            // P1PerformHttp failed before taking over the webview reference.
            P1QueueWebViewRelease(request->webView);
            delete request;
            continue;
        }
        delete request;

        // A request that finished after Stopping is never answered; only its
        // webview reference is handed to the UI thread for release.
        if (!P1IsRunning()) {
            P1QueueWebViewRelease(reply->webView);
            delete reply;
            continue;
        }

        bool stored = false;
        AcquireSRWLockExclusive(&g_p1ReplyLock);
        try {
            g_p1ReplyQueue.push_back(reply);
            stored = true;
        } catch (...) {
        }
        ReleaseSRWLockExclusive(&g_p1ReplyLock);

        if (stored) {
            P1WakeUi();
        } else {
            // Cannot queue: leak the reference instead of releasing it here.
            Wh_Log(L"[p1] reply queue rejected an http reply; its webview "
                   L"reference is leaked");
            delete reply;
        }
    }
    return 0;
}

static bool P1EnsureCacheSaver();
static bool P1StopHttpWorkers();

// Starts the HTTP pool. The state transition, the handle storage and the
// thread creation happen under g_p1HttpLock; a partial start is torn down by
// P1StopHttpWorkers, which is the only place that waits on and closes worker
// handles. Never starts once the mod is not Running, and never moves a
// Stopping/Stopped pool back to Running. The cache saver starts with the pool,
// as the two are joined together at teardown.
static bool P1StartHttpWorkers() {
    AcquireSRWLockExclusive(&g_p1HttpLock);
    if (g_p1PoolState == P1PoolState::Running) {
        ReleaseSRWLockExclusive(&g_p1HttpLock);
        return true;
    }
    if (g_p1PoolState != P1PoolState::NotStarted || !P1IsRunning()) {
        ReleaseSRWLockExclusive(&g_p1HttpLock);
        return false;
    }
    g_p1PoolState = P1PoolState::Starting;
    g_p1WorkerCount = 0;
    for (HANDLE& handle : g_p1Workers) {
        handle = nullptr;
    }

    for (size_t i = 0; i < kP1HttpWorkers; ++i) {
        HANDLE thread =
            CreateThread(nullptr, 0, &P1HttpWorkerProc, nullptr, 0, nullptr);
        if (!thread) {
            Wh_Log(L"[p1] CreateThread for an http worker failed (%lu)",
                   GetLastError());
            break;
        }
        g_p1Workers[g_p1WorkerCount++] = thread;
    }

    bool complete = g_p1WorkerCount == kP1HttpWorkers;
    if (!complete || !P1IsRunning()) {
        size_t started = g_p1WorkerCount;
        ReleaseSRWLockExclusive(&g_p1HttpLock);
        Wh_Log(L"[p1] http worker start failed (%llu of %llu); stopping the "
               L"partial pool",
               static_cast<unsigned long long>(started),
               static_cast<unsigned long long>(kP1HttpWorkers));
        if (!P1StopHttpWorkers()) {
            Wh_Log(L"[p1] the partial http pool could not be joined; the "
                   L"unload will refuse to proceed");
        }
        return false;
    }

    g_p1PoolState = P1PoolState::Running;
    size_t started = g_p1WorkerCount;
    ReleaseSRWLockExclusive(&g_p1HttpLock);
    Wh_Log(L"[p1] %llu http workers started",
           static_cast<unsigned long long>(started));
    P1EnsureCacheSaver();
    return true;
}

static bool P1EnqueueHttpRequest(P1HttpRequest* request) {
    if (!P1IsRunning() || !P1StartHttpWorkers()) {
        return false;
    }

    AcquireSRWLockExclusive(&g_p1HttpLock);
    if (g_p1PoolState != P1PoolState::Running ||
        g_p1HttpQueue.size() >= kP1HttpQueueLimit) {
        ReleaseSRWLockExclusive(&g_p1HttpLock);
        return false;
    }
    bool queued = false;
    try {
        g_p1HttpQueue.push_back(request);
        queued = true;
    } catch (...) {
    }
    ReleaseSRWLockExclusive(&g_p1HttpLock);

    if (queued) {
        WakeAllConditionVariable(&g_p1HttpCv);
    }
    return queued;
}

static bool P1StopHttpWorkers() {
    // Sticky: a previous join failed, so a worker may still be running. The
    // pool state alone cannot carry that (the partial-start path stops the
    // pool and may never retry the join), and the unload must not treat the
    // pool as stopped.
    if (g_p1WorkerStopFailed.load(std::memory_order_acquire)) {
        Wh_Log(L"[p1] DEFECT: a previous http worker join failed; the pool is "
               L"not proven stopped");
        return false;
    }

    std::deque<P1HttpRequest*> pending;
    HANDLE workers[kP1HttpWorkers] = {};
    size_t count = 0;

    AcquireSRWLockExclusive(&g_p1HttpLock);
    if (g_p1PoolState == P1PoolState::NotStarted ||
        g_p1PoolState == P1PoolState::Stopped) {
        g_p1PoolState = P1PoolState::Stopped;
        // The check before the lock is only a fast path: a join that failed
        // while this caller was between that check and this lock must still be
        // reported.
        bool stopped = !g_p1WorkerStopFailed.load(std::memory_order_acquire);
        ReleaseSRWLockExclusive(&g_p1HttpLock);
        return stopped;
    }
    if (g_p1PoolState == P1PoolState::Stopping) {
        // Another caller already owns the join. Wait for its result: returning
        // success here would let an unload proceed before the join proved
        // anything. The owner publishes Stopped and wakes this condition
        // variable on every path.
        while (g_p1PoolState == P1PoolState::Stopping) {
            SleepConditionVariableSRW(&g_p1HttpCv, &g_p1HttpLock, INFINITE, 0);
        }
        bool stopped = !g_p1WorkerStopFailed.load(std::memory_order_acquire);
        ReleaseSRWLockExclusive(&g_p1HttpLock);
        return stopped;
    }
    g_p1PoolState = P1PoolState::Stopping;
    count = g_p1WorkerCount;
    for (size_t i = 0; i < count; ++i) {
        workers[i] = g_p1Workers[i];
        g_p1Workers[i] = nullptr;
    }
    g_p1WorkerCount = 0;
    pending.swap(g_p1HttpQueue);
    ReleaseSRWLockExclusive(&g_p1HttpLock);
    WakeAllConditionVariable(&g_p1HttpCv);

    // Pending requests never ran: their webview references go to the UI
    // thread's release queue, never to Release on this thread.
    for (P1HttpRequest* request : pending) {
        if (request) {
            P1QueueWebViewRelease(request->webView);
            delete request;
        }
    }

    bool stopped = true;
    if (count) {
        // WinHttpSetTimeouts bounds each phase, not the whole request, so a
        // slow server can extend this wait. The join has no deadline on
        // purpose: Wh_ModBeforeUninit must not return while a worker could
        // still run code from this image.
        DWORD wait = WaitForMultipleObjects(static_cast<DWORD>(count), workers,
                                            TRUE, INFINITE);
        if (wait == WAIT_FAILED || wait == WAIT_TIMEOUT) {
            Wh_Log(L"[p1] WaitForMultipleObjects(http workers) returned %lu "
                   L"(error %lu)",
                   static_cast<unsigned long>(wait),
                   static_cast<unsigned long>(GetLastError()));
            g_p1WorkerStopFailed.store(true, std::memory_order_release);
            stopped = false;
        }
        for (size_t i = 0; i < count; ++i) {
            CloseHandle(workers[i]);
        }
    }

    AcquireSRWLockExclusive(&g_p1HttpLock);
    // Publish the outcome and wake a waiter in the Stopping branch. A failed
    // join also sets the sticky flag above, so every later caller returns
    // false.
    g_p1PoolState = P1PoolState::Stopped;
    WakeAllConditionVariable(&g_p1HttpCv);
    ReleaseSRWLockExclusive(&g_p1HttpLock);

    // The reply queue is drained by the UI thread, which releases the webview
    // references there. If it is never drained those references leak, which is
    // still safer than releasing them here.
    P1WakeUi();
    return stopped;
}

// ---------------------------------------------------------------------------
// Settings push
// ---------------------------------------------------------------------------

static void P1CacheClear();

// clearCache is an edge trigger: remember the value carried by the previous
// push and clear only on a false -> true transition. Wh_SetIntSetting is not
// available for mod settings, so the user switches the checkbox off by hand
// before it can fire again.
static std::atomic<bool> g_p1LastClearCachePushed{false};

static bool P1ConsumeClearCache(bool requested) {
    bool previous =
        g_p1LastClearCachePushed.exchange(requested, std::memory_order_acq_rel);
    if (!requested || previous) {
        return false;
    }
    Wh_Log(L"[p1] clearCache false->true; clearing the translation cache");
    return true;
}

static void P1PushSettingsToWebView(void* webView, const wchar_t* reason) {
    if (!webView || !P1IsRunning()) {
        return;
    }

    P1SettingsSnapshot snapshot = P1ReadSettings();
    P1KeyInfo key = P1GetKeyInfo();
    bool clearCache = P1ConsumeClearCache(snapshot.clearCacheRequested);
    if (clearCache) {
        P1CacheClear();
    }
    std::wstring json =
        P1BuildSettingsJson(snapshot, key.source, clearCache);

    void* postFn = nullptr;
    if (!P0GetValidatedSlot(webView, kWebViewSlotPostWebMessageAsString,
                            &postFn)) {
        Wh_Log(L"[p1] settings not pushed: PostWebMessageAsString slot invalid");
        return;
    }

    std::wstring message = kP1Tag;
    message.push_back(0x1f);
    message += L"settings";
    message.push_back(0x1f);
    message += json;

    HRESULT hr = reinterpret_cast<FnPostWebMessageAsString>(postFn)(
        webView, message.c_str());
    Wh_Log(L"[p1] settings pushed (%s): targetLang=%s provider=%s key=%s "
           L"clearCache=%d hr=%08X",
           reason ? reason : L"?", snapshot.targetLang.c_str(),
           snapshot.provider.c_str(), key.source.c_str(),
           clearCache ? 1 : 0, static_cast<unsigned>(hr));
}

static void P1PushSettingsToAll(const wchar_t* reason) {
    if (!P1IsRunning()) {
        return;
    }
    void* webView = P0AcquireWebView();
    if (!webView) {
        return;
    }
    P0ComRef webViewRef;
    webViewRef.object = webView;
    P1PushSettingsToWebView(webView, reason);
}

// ---------------------------------------------------------------------------
// Translation cache
//
// The page supplies the key; the mod owns the store. An in-memory map is
// persisted as a small UTF-8 text file under Wh_GetModStoragePath(), entries
// expire after cacheTtlDays and the store is capped at 8 MB with the oldest
// entries evicted first. The UI thread only mutates the map and marks it
// dirty; one saver thread writes the file after a short debounce. The file is
// loaded at init and flushed by the saver on teardown.
//
// A put is accepted for any non-empty key and value. That check is a guard
// against empty writes, not a security boundary: the page may store a result
// it obtained from any provider, including an LLM -> Edge fallback.
// ---------------------------------------------------------------------------

static constexpr size_t kP1CacheMaxBytes = 8u * 1024u * 1024u;
static constexpr size_t kP1CachePruneEveryPuts = 32;
static constexpr DWORD kP1CacheSaveDebounceMs = 2000;
static constexpr wchar_t kP1CacheFileName[] = L"translation-cache.tsv";

struct P1CacheEntry {
    std::wstring text;
    unsigned long long at = 0;  // stored, FILETIME milliseconds
};

static std::mutex g_p1CacheMutex;
static std::unordered_map<std::wstring, P1CacheEntry> g_p1Cache;
static std::wstring g_p1CachePath;
static size_t g_p1CacheBytes = 0;  // running total, never rescanned per put
static unsigned g_p1CachePutsSincePrune = 0;  // guarded by g_p1CacheMutex
static bool g_p1CacheDirty = false;
static bool g_p1CacheClearPending = false;

// Saver thread. g_p1CacheEvents[0] is the auto-reset save signal, [1] the
// manual stop event. The mutex guards only the thread/event handles.
static std::mutex g_p1CacheSaverMutex;
static HANDLE g_p1CacheEvents[2] = {nullptr, nullptr};
static HANDLE g_p1CacheSaverThread = nullptr;
static bool g_p1CacheSaverStarted = false;

// cacheTtlDays is read once per settings generation: a cache-get must not hit
// the registry for every translated string.
static std::mutex g_p1CacheTtlMutex;
static int g_p1CacheTtlDays = 30;
static long g_p1CacheTtlGeneration = -1;

static int P1CacheTtlDays() {
    long generation = g_p1SettingsGeneration.load(std::memory_order_acquire);
    std::lock_guard<std::mutex> lock(g_p1CacheTtlMutex);
    if (g_p1CacheTtlGeneration != generation) {
        g_p1CacheTtlDays = P1ReadSettings().cacheTtlDays;
        g_p1CacheTtlGeneration = generation;
    }
    return g_p1CacheTtlDays;
}

// Wall-clock milliseconds since 1601, so the lifetime survives a restart
// (GetTickCount64 resets with the machine).
static unsigned long long P1CacheNowMs() {
    FILETIME fileTime{};
    GetSystemTimeAsFileTime(&fileTime);
    ULARGE_INTEGER value{};
    value.LowPart = fileTime.dwLowDateTime;
    value.HighPart = fileTime.dwHighDateTime;
    return value.QuadPart / 10000ull;
}

static size_t P1CacheEntryBytes(const std::wstring& key,
                                const P1CacheEntry& entry) {
    return (key.size() + entry.text.size()) * sizeof(wchar_t) + 32;
}

// Evicts oldest-first only while the running total is over the cap.
static void P1CacheEvictLocked() {
    if (g_p1CacheBytes <= kP1CacheMaxBytes) {
        return;
    }
    std::vector<std::pair<unsigned long long, std::wstring>> order;
    order.reserve(g_p1Cache.size());
    for (const auto& item : g_p1Cache) {
        order.emplace_back(item.second.at, item.first);
    }
    std::sort(order.begin(), order.end(),
              [](const auto& a, const auto& b) { return a.first < b.first; });
    for (const auto& item : order) {
        if (g_p1CacheBytes <= kP1CacheMaxBytes) {
            break;
        }
        auto found = g_p1Cache.find(item.second);
        if (found == g_p1Cache.end()) {
            continue;
        }
        size_t bytes = P1CacheEntryBytes(found->first, found->second);
        g_p1CacheBytes =
            g_p1CacheBytes > bytes ? g_p1CacheBytes - bytes : 0;
        g_p1Cache.erase(found);
    }
}

static void P1CachePruneExpiredLocked(int ttlDays, unsigned long long now) {
    if (ttlDays <= 0) {
        return;
    }
    const unsigned long long ttl =
        static_cast<unsigned long long>(ttlDays) * 86400000ull;
    for (auto it = g_p1Cache.begin(); it != g_p1Cache.end();) {
        if (it->second.at != 0 && now > it->second.at &&
            now - it->second.at > ttl) {
            size_t bytes = P1CacheEntryBytes(it->first, it->second);
            g_p1CacheBytes =
                g_p1CacheBytes > bytes ? g_p1CacheBytes - bytes : 0;
            it = g_p1Cache.erase(it);
        } else {
            ++it;
        }
    }
}

static void P1CacheEscapeAppend(std::wstring& out, const std::wstring& value) {
    for (wchar_t ch : value) {
        switch (ch) {
            case L'\\':
                out += L"\\\\";
                break;
            case L'\t':
                out += L"\\t";
                break;
            case L'\n':
                out += L"\\n";
                break;
            case L'\r':
                out += L"\\r";
                break;
            default:
                out.push_back(ch);
                break;
        }
    }
}

static bool P1CacheUnescape(const std::wstring& in, std::wstring& out) {
    out.clear();
    out.reserve(in.size());
    for (size_t i = 0; i < in.size(); ++i) {
        wchar_t ch = in[i];
        if (ch != L'\\') {
            out.push_back(ch);
            continue;
        }
        if (++i >= in.size()) {
            return false;
        }
        switch (in[i]) {
            case L'\\':
                out.push_back(L'\\');
                break;
            case L't':
                out.push_back(L'\t');
                break;
            case L'n':
                out.push_back(L'\n');
                break;
            case L'r':
                out.push_back(L'\r');
                break;
            default:
                return false;
        }
    }
    return true;
}

static bool P1ResolveStorageFile(const wchar_t* fileName, std::wstring& path) {
    wchar_t storage[MAX_PATH]{};
    size_t length = Wh_GetModStoragePath(storage, ARRAYSIZE(storage));
    if (length == 0 || length >= ARRAYSIZE(storage)) {
        return false;
    }
    std::wstring resolved;
    try {
        resolved.assign(storage, length);
        if (resolved.empty()) {
            return false;
        }
        if (resolved.back() != L'\\') {
            resolved.push_back(L'\\');
        }
        resolved += fileName;
    } catch (...) {
        return false;
    }
    path = std::move(resolved);
    return true;
}

static bool P1ResolveCachePath(std::wstring& path) {
    return P1ResolveStorageFile(kP1CacheFileName, path);
}

static void P1CacheLoad() {
    try {
        std::wstring path;
        if (!P1ResolveCachePath(path)) {
            Wh_Log(L"[p1] cache: no storage path; memory only");
            return;
        }
        {
            std::lock_guard<std::mutex> lock(g_p1CacheMutex);
            g_p1CachePath = path;
        }

        HANDLE file = CreateFileW(path.c_str(), GENERIC_READ,
                                  FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr,
                                  OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
        if (file == INVALID_HANDLE_VALUE) {
            Wh_Log(L"[p1] cache: no file yet");
            return;
        }
        LARGE_INTEGER size{};
        if (!GetFileSizeEx(file, &size) || size.QuadPart <= 0 ||
            size.QuadPart > 32 * 1024 * 1024) {
            CloseHandle(file);
            Wh_Log(L"[p1] cache: file has an unusable size; ignored");
            return;
        }
        std::string utf8(static_cast<size_t>(size.QuadPart), '\0');
        DWORD read = 0;
        BOOL ok = ReadFile(file, &utf8[0], static_cast<DWORD>(utf8.size()),
                           &read, nullptr);
        CloseHandle(file);
        if (!ok) {
            Wh_Log(L"[p1] cache: read failed (%lu)", GetLastError());
            return;
        }
        utf8.resize(read);

        std::wstring text = P1Utf8ToWide(utf8);
        int ttlDays = P1CacheTtlDays();
        unsigned long long now = P1CacheNowMs();
        size_t loaded = 0;
        size_t start = 0;
        while (start < text.size()) {
            size_t end = text.find(L'\n', start);
            if (end == std::wstring::npos) {
                end = text.size();
            }
            std::wstring line = text.substr(start, end - start);
            start = end + 1;
            if (!line.empty() && line.back() == L'\r') {
                line.pop_back();
            }
            if (line.empty()) {
                continue;
            }
            std::vector<std::wstring> fields;
            P1Split(line, L'\t', 3, fields);
            if (fields.size() != 3) {
                continue;
            }
            unsigned long long at = 0;
            if (!P1ParseUInt64(fields[0], at)) {
                continue;
            }
            std::wstring key;
            std::wstring value;
            if (!P1CacheUnescape(fields[1], key) ||
                !P1CacheUnescape(fields[2], value) || key.empty()) {
                continue;
            }
            P1CacheEntry entry;
            entry.text = std::move(value);
            entry.at = at;
            std::lock_guard<std::mutex> lock(g_p1CacheMutex);
            auto inserted = g_p1Cache.emplace(key, std::move(entry));
            if (inserted.second) {
                g_p1CacheBytes +=
                    P1CacheEntryBytes(inserted.first->first,
                                      inserted.first->second);
            }
            ++loaded;
        }

        std::lock_guard<std::mutex> lock(g_p1CacheMutex);
        P1CachePruneExpiredLocked(ttlDays, now);
        P1CacheEvictLocked();
        Wh_Log(L"[p1] cache loaded: %llu entries, %llu bytes",
               static_cast<unsigned long long>(loaded),
               static_cast<unsigned long long>(g_p1CacheBytes));
    } catch (...) {
        Wh_Log(L"[p1] cache load failed");
    }
}

static void P1CacheSubtractBytes(size_t bytes) {
    g_p1CacheBytes = g_p1CacheBytes > bytes ? g_p1CacheBytes - bytes : 0;
}

// Serializes the map and writes the file. Called only from the saver thread,
// or from the engine thread when the saver was never started; never from the
// UI thread. The map is only copied under g_p1CacheMutex; formatting and the
// file I/O happen outside it, so the UI thread never waits for a full
// serialization.
static void P1CacheSaveIfNeeded() {
    try {
        std::wstring path;
        std::vector<std::pair<std::wstring, P1CacheEntry>> entries;
        bool clear = false;
        bool dirty = false;
        {
            std::lock_guard<std::mutex> lock(g_p1CacheMutex);
            if (g_p1CachePath.empty()) {
                return;
            }
            clear = g_p1CacheClearPending;
            dirty = g_p1CacheDirty;
            if (!clear && !dirty) {
                return;
            }
            g_p1CacheClearPending = false;
            g_p1CacheDirty = false;
            path = g_p1CachePath;
            if (dirty) {
                entries.reserve(g_p1Cache.size());
                for (const auto& item : g_p1Cache) {
                    entries.emplace_back(item.first, item.second);
                }
            }
        }

        if (clear) {
            DeleteFileW(path.c_str());
            Wh_Log(L"[p1] cache file removed");
        }
        if (!dirty) {
            return;
        }

        std::wstring out;
        out.reserve(entries.size() * 64);
        for (const auto& item : entries) {
            out += std::to_wstring(item.second.at);
            out.push_back(L'\t');
            P1CacheEscapeAppend(out, item.first);
            out.push_back(L'\t');
            P1CacheEscapeAppend(out, item.second.text);
            out.push_back(L'\n');
        }

        std::string utf8 = P1WideToUtf8(out);
        std::wstring temp = path + L".tmp";
        HANDLE file = CreateFileW(temp.c_str(), GENERIC_WRITE, 0, nullptr,
                                  CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL,
                                  nullptr);
        if (file == INVALID_HANDLE_VALUE) {
            Wh_Log(L"[p1] cache save: cannot open %s (%lu)", temp.c_str(),
                   GetLastError());
            std::lock_guard<std::mutex> lock(g_p1CacheMutex);
            g_p1CacheDirty = true;
            return;
        }
        DWORD written = 0;
        BOOL ok = WriteFile(file, utf8.data(),
                            static_cast<DWORD>(utf8.size()), &written, nullptr);
        CloseHandle(file);
        if (!ok || written != utf8.size()) {
            DeleteFileW(temp.c_str());
            Wh_Log(L"[p1] cache save: write failed (%lu)", GetLastError());
            std::lock_guard<std::mutex> lock(g_p1CacheMutex);
            g_p1CacheDirty = true;
            return;
        }
        if (!MoveFileExW(temp.c_str(), path.c_str(), MOVEFILE_REPLACE_EXISTING)) {
            DeleteFileW(temp.c_str());
            Wh_Log(L"[p1] cache save: replace failed (%lu)", GetLastError());
            std::lock_guard<std::mutex> lock(g_p1CacheMutex);
            g_p1CacheDirty = true;
        }
    } catch (...) {
        Wh_Log(L"[p1] cache save failed");
        std::lock_guard<std::mutex> lock(g_p1CacheMutex);
        g_p1CacheDirty = true;
    }
}

static DWORD WINAPI P1CacheSaverProc(void*) {
    for (;;) {
        DWORD wait =
            WaitForMultipleObjects(2, g_p1CacheEvents, FALSE, INFINITE);
        if (wait == WAIT_OBJECT_0 + 1) {
            P1CacheSaveIfNeeded();
            return 0;
        }
        if (wait != WAIT_OBJECT_0) {
            return 0;
        }
        // Debounce: another signal restarts the wait, a timeout writes.
        for (;;) {
            DWORD again = WaitForMultipleObjects(2, g_p1CacheEvents, FALSE,
                                                 kP1CacheSaveDebounceMs);
            if (again == WAIT_OBJECT_0 + 1) {
                P1CacheSaveIfNeeded();
                return 0;
            }
            if (again == WAIT_OBJECT_0) {
                continue;
            }
            break;
        }
        P1CacheSaveIfNeeded();
    }
}

static bool P1EnsureCacheSaver() {
    std::lock_guard<std::mutex> lock(g_p1CacheSaverMutex);
    if (g_p1CacheSaverStarted) {
        return true;
    }
    if (!P1IsRunning()) {
        return false;
    }
    if (!g_p1CacheEvents[0]) {
        g_p1CacheEvents[0] = CreateEventW(nullptr, FALSE, FALSE, nullptr);
        g_p1CacheEvents[1] = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    }
    if (!g_p1CacheEvents[0] || !g_p1CacheEvents[1]) {
        Wh_Log(L"[p1] cache saver events could not be created (%lu)",
               GetLastError());
        for (HANDLE& event : g_p1CacheEvents) {
            if (event) {
                CloseHandle(event);
                event = nullptr;
            }
        }
        return false;
    }
    HANDLE thread =
        CreateThread(nullptr, 0, &P1CacheSaverProc, nullptr, 0, nullptr);
    if (!thread) {
        Wh_Log(L"[p1] cache saver thread could not be started (%lu)",
               GetLastError());
        for (HANDLE& event : g_p1CacheEvents) {
            if (event) {
                CloseHandle(event);
                event = nullptr;
            }
        }
        return false;
    }
    g_p1CacheSaverThread = thread;
    g_p1CacheSaverStarted = true;
    return true;
}

static void P1SignalCacheSave() {
    std::lock_guard<std::mutex> lock(g_p1CacheSaverMutex);
    if (g_p1CacheEvents[0]) {
        SetEvent(g_p1CacheEvents[0]);
    }
}

// Stops the saver and flushes any last dirty state on this thread. The UI
// thread never serializes the map.
static bool P1StopCacheSaver() {
    HANDLE thread = nullptr;
    {
        std::lock_guard<std::mutex> lock(g_p1CacheSaverMutex);
        if (!g_p1CacheSaverStarted) {
            P1CacheSaveIfNeeded();
            return true;
        }
        g_p1CacheSaverStarted = false;
        thread = g_p1CacheSaverThread;
        g_p1CacheSaverThread = nullptr;
        if (g_p1CacheEvents[1]) {
            SetEvent(g_p1CacheEvents[1]);
        }
    }

    bool stopped = true;
    if (thread) {
        DWORD wait = WaitForSingleObject(thread, INFINITE);
        if (wait != WAIT_OBJECT_0) {
            Wh_Log(L"[p1] WaitForSingleObject(cache saver) returned %lu "
                   L"(error %lu)",
                   static_cast<unsigned long>(wait),
                   static_cast<unsigned long>(GetLastError()));
            stopped = false;
        }
        CloseHandle(thread);
    }

    std::lock_guard<std::mutex> lock(g_p1CacheSaverMutex);
    for (HANDLE& event : g_p1CacheEvents) {
        if (event) {
            CloseHandle(event);
            event = nullptr;
        }
    }
    return stopped;
}

static void P1CacheClear() {
    try {
        P1EnsureCacheSaver();
        {
            std::lock_guard<std::mutex> lock(g_p1CacheMutex);
            g_p1Cache.clear();
            g_p1CacheBytes = 0;
            g_p1CachePutsSincePrune = 0;
            g_p1CacheDirty = false;
            g_p1CacheClearPending = true;
        }
        P1SignalCacheSave();
        Wh_Log(L"[p1] cache cleared");
    } catch (...) {
        Wh_Log(L"[p1] cache clear failed");
    }
}

static bool P1CacheGet(const std::wstring& key,
                       std::wstring& text,
                       int ttlDays) {
    if (key.empty() || !P1IsRunning()) {
        return false;
    }
    std::lock_guard<std::mutex> lock(g_p1CacheMutex);
    auto found = g_p1Cache.find(key);
    if (found == g_p1Cache.end()) {
        return false;
    }
    unsigned long long now = P1CacheNowMs();
    if (ttlDays > 0 && found->second.at != 0 && now > found->second.at &&
        now - found->second.at >
            static_cast<unsigned long long>(ttlDays) * 86400000ull) {
        P1CacheSubtractBytes(P1CacheEntryBytes(found->first, found->second));
        g_p1Cache.erase(found);
        return false;
    }
    text = found->second.text;
    return true;
}

// Returns false when the put could not be stored: empty key or value, or an
// allocation failure. The non-empty check is a guard against empty writes, not
// a security boundary; any non-empty key and value is accepted.
static bool P1CachePut(const std::wstring& key,
                       const std::wstring& text,
                       int ttlDays) {
    if (key.empty() || text.empty() || !P1IsRunning()) {
        return false;
    }
    P1EnsureCacheSaver();
    try {
        std::lock_guard<std::mutex> lock(g_p1CacheMutex);
        unsigned long long now = P1CacheNowMs();
        auto found = g_p1Cache.find(key);
        if (found != g_p1Cache.end()) {
            P1CacheSubtractBytes(
                P1CacheEntryBytes(found->first, found->second));
            found->second.text = text;
            found->second.at = now;
            g_p1CacheBytes +=
                P1CacheEntryBytes(found->first, found->second);
        } else {
            P1CacheEntry entry;
            entry.text = text;
            entry.at = now;
            auto inserted = g_p1Cache.emplace(key, std::move(entry));
            if (!inserted.second) {
                return false;
            }
            g_p1CacheBytes +=
                P1CacheEntryBytes(inserted.first->first,
                                  inserted.first->second);
        }
        g_p1CacheDirty = true;
        // Do not walk the whole map on every put: prune at most once every
        // kP1CachePruneEveryPuts puts, and always when the byte total is over
        // the cap.
        ++g_p1CachePutsSincePrune;
        if (g_p1CacheBytes > kP1CacheMaxBytes ||
            g_p1CachePutsSincePrune >= kP1CachePruneEveryPuts) {
            P1CachePruneExpiredLocked(ttlDays, now);
            P1CacheEvictLocked();
            g_p1CachePutsSincePrune = 0;
        }
    } catch (...) {
        return false;
    }
    P1SignalCacheSave();
    return true;
}

// ---------------------------------------------------------------------------
// Message handling
// ---------------------------------------------------------------------------

static void P1HandleJsLog(const std::wstring& text) {
    std::wstring line;
    try {
        line.assign(text, 0, std::min<size_t>(text.size(), kP1MaxJsLogChars));
    } catch (...) {
        return;
    }
    for (wchar_t& ch : line) {
        if (ch == L'\r' || ch == L'\n') {
            ch = L' ';
        }
    }
    Wh_Log(L"[js] %s", line.c_str());
}

// Protocol rule: unknown kinds, malformed messages and oversized payloads are
// logged once per session, not once per message.
static std::atomic<bool> g_p1LogOversized{false};
static std::atomic<bool> g_p1LogMalformed{false};
static std::atomic<bool> g_p1LogUnknownKind{false};
static std::atomic<bool> g_p1LogCachePutRejected{false};

static void P1RejectHttp(void* webView,
                         unsigned long long id,
                         const wchar_t* reason,
                         unsigned long error) {
    Wh_Log(L"[p1] http rejected id=%llu: %s", id, reason);
    P1PostHttpReply(webView, id, 0, error, std::string());
}

// The relay accepts exactly two endpoints: Microsoft's Edge translation
// service and the configured LLM's /chat/completions. Anything else is refused
// with status 0 / error 87, so the mod cannot be used as an open proxy.
static bool P1IsAllowedHttpUrl(const std::wstring& url,
                               const P1SettingsSnapshot& settings) {
    P1UrlParts parts;
    if (!P1ParseUrl(url, parts)) {
        return false;
    }

    // https://edge.microsoft.com/translate/translatetext?...
    // The host is case-insensitive; no explicit port; the path is exact and a
    // query must follow the path. P1ParseUrl already rejected fragments.
    if (P1EqualsNoCase(parts.host, L"edge.microsoft.com")) {
        if (parts.explicitPort) {
            return false;
        }
        static constexpr wchar_t kPath[] = L"/translate/translatetext";
        constexpr size_t kPathChars = ARRAYSIZE(kPath) - 1;
        if (parts.path.size() <= kPathChars ||
            parts.path.compare(0, kPathChars, kPath) != 0) {
            return false;
        }
        return parts.path[kPathChars] == L'?';
    }

    // Exactly <llmBaseUrl>/chat/completions. Scheme and host are compared
    // case-insensitively; the path (and any port) must match exactly.
    P1UrlParts base;
    std::wstring trimmed = settings.llmBaseUrl;
    while (!trimmed.empty() && trimmed.back() == L'/') {
        trimmed.pop_back();
    }
    if (trimmed.empty() || !P1ParseUrl(trimmed, base)) {
        return false;
    }
    // A query or fragment in the base would make the expected path ambiguous.
    if (base.path.find(L'?') != std::wstring::npos) {
        return false;
    }
    if (base.explicitPort != parts.explicitPort || base.port != parts.port) {
        return false;
    }
    if (!P1EqualsNoCase(base.host, parts.host)) {
        return false;
    }

    std::wstring expectedPath = base.path;
    while (!expectedPath.empty() && expectedPath.back() == L'/') {
        expectedPath.pop_back();
    }
    expectedPath += L"/chat/completions";
    return parts.path == expectedPath;
}

static void P1HandleHttp(void* sender, const std::wstring& rest) {
    // Fields after the kind: requestId, method, url, body. The body is last and
    // may itself contain the separator, hence the field limit.
    std::vector<std::wstring> head;
    P1Split(rest, 0x1f, 4, head);
    if (head.size() < 4) {
        if (!g_p1LogMalformed.exchange(true, std::memory_order_relaxed)) {
            Wh_Log(L"[p1] http message malformed (fields=%llu); ignored",
                   static_cast<unsigned long long>(head.size()));
        }
        return;
    }

    unsigned long long id = 0;
    if (!P1ParseUInt64(head[0], id)) {
        if (!g_p1LogMalformed.exchange(true, std::memory_order_relaxed)) {
            Wh_Log(L"[p1] http message has no usable request id; ignored");
        }
        return;
    }

    std::wstring method = head[1];
    if (method.empty() || method.size() > 16) {
        P1RejectHttp(sender, id, L"bad method", ERROR_INVALID_PARAMETER);
        return;
    }
    for (wchar_t& ch : method) {
        ch = P1AsciiUpper(ch);
    }
    if (method != L"GET" && method != L"POST") {
        P1RejectHttp(sender, id, L"unsupported method", ERROR_INVALID_PARAMETER);
        return;
    }

    const std::wstring& url = head[2];
    if (url.size() > kP1MaxUrlChars) {
        P1RejectHttp(sender, id, L"url too long", ERROR_INVALID_PARAMETER);
        return;
    }

    const std::wstring& bodyWide = head[3];
    if (bodyWide.size() > kP1MaxBodyBytes) {
        P1RejectHttp(sender, id, L"body too large", ERROR_INVALID_PARAMETER);
        return;
    }
    if (method == L"GET" && !bodyWide.empty()) {
        Wh_Log(L"[p1] http id=%llu: GET with a body; the body is ignored", id);
    }

    P1SettingsSnapshot snapshot = P1ReadSettings();
    if (!P1IsAllowedHttpUrl(url, snapshot)) {
        P1RejectHttp(sender, id, L"url is not an allowed endpoint",
                     ERROR_INVALID_PARAMETER);
        return;
    }

    std::string body;
    if (method != L"GET") {
        body = P1WideToUtf8(bodyWide);
        if (body.size() > kP1MaxBodyBytes) {
            P1RejectHttp(sender, id, L"body too large after utf-8 encoding",
                         ERROR_INVALID_PARAMETER);
            return;
        }
    }

    P1KeyInfo key = P1GetKeyInfo();

    auto* request = new (std::nothrow) P1HttpRequest();
    if (!request) {
        P1RejectHttp(sender, id, L"out of memory for the request",
                     ERROR_NOT_ENOUGH_MEMORY);
        return;
    }
    request->id = id;
    request->method = method;
    request->url = url;
    request->body = std::move(body);
    request->timeoutMs =
        static_cast<unsigned long long>(snapshot.timeoutMs);
    request->apiKey = key.key;
    request->apiKeyHost = key.host;

    auto addRef =
        reinterpret_cast<FnAddRef>(P0GetRawSlot(sender, kSlotAddRef));
    if (!addRef) {
        delete request;
        P1RejectHttp(sender, id, L"no AddRef slot on the webview",
                     ERROR_INVALID_HANDLE);
        return;
    }
    addRef(sender);
    request->webView = sender;

    if (!P1EnqueueHttpRequest(request)) {
        P1ReleaseWebViewRef(request->webView);
        delete request;
        P1RejectHttp(sender, id, L"http queue is busy", ERROR_BUSY);
    }
}

static void P1HandleMessage(void* sender, const std::wstring& text) {
    P1NoteUiThread();
    if (!P1IsRunning()) {
        return;
    }
    // The dispatch window is a prerequisite for injection, so it must exist
    // here; without it replies and teardown could not reach the UI thread.
    if (!P1EnsureDispatchWindow()) {
        if (!g_p1DispatchFailureLogged.exchange(true)) {
            Wh_Log(L"[p1] dispatch window unavailable; page messages are "
                   L"ignored");
        }
        return;
    }
    P1DrainRepliesOnUiThread(true);

    if (g_p1SettingsDirty.exchange(false) && g_p1PageReady.load()) {
        P1PushSettingsToAll(L"deferred");
    }

    if (text.size() > kP1MaxMessageChars) {
        if (!g_p1LogOversized.exchange(true, std::memory_order_relaxed)) {
            Wh_Log(L"[p1] message too large (%llu chars); ignored",
                   static_cast<unsigned long long>(text.size()));
        }
        return;
    }

    std::vector<std::wstring> parts;
    P1Split(text, 0x1f, 3, parts);
    if (parts.size() < 2 || parts[0] != kP1Tag) {
        if (!g_p1LogMalformed.exchange(true, std::memory_order_relaxed)) {
            Wh_Log(L"[p1] malformed message ignored (len=%llu)",
                   static_cast<unsigned long long>(text.size()));
        }
        return;
    }

    const std::wstring& kind = parts[1];
    const std::wstring& rest = parts.size() >= 3 ? parts[2] : std::wstring();

    if (kind == L"settings?") {
        g_p1PageReady.store(true);
        Wh_Log(L"[p1] settings requested (page ready)");
        P1PushSettingsToWebView(sender, L"settings?");
        return;
    }

    if (kind == L"log") {
        g_p1PageReady.store(true);
        P1HandleJsLog(rest);
        return;
    }

    if (kind == L"http") {
        g_p1PageReady.store(true);
        P1HandleHttp(sender, rest);
        return;
    }

    if (kind == L"cache-get") {
        g_p1PageReady.store(true);
        // Fields after the kind: requestId, key. The reply echoes requestId so
        // a late answer cannot resolve a newer lookup for the same key.
        std::vector<std::wstring> fields;
        P1Split(rest, 0x1f, 2, fields);
        unsigned long long requestId = 0;
        if (fields.size() != 2 || fields[1].empty() ||
            !P1ParseUInt64(fields[0], requestId)) {
            if (!g_p1LogMalformed.exchange(true, std::memory_order_relaxed)) {
                Wh_Log(L"[p1] cache-get malformed (requestId and key are "
                       L"required); ignored");
            }
            return;
        }
        std::wstring value;
        bool hit = false;
        try {
            hit = P1CacheGet(fields[1], value, P1CacheTtlDays());
        } catch (...) {
            hit = false;
        }
        std::wstring reply = kP1Tag;
        reply.push_back(0x1f);
        reply += hit ? L"cache-hit" : L"cache-miss";
        reply.push_back(0x1f);
        reply += fields[0];  // echo the requestId token exactly
        reply.push_back(0x1f);
        reply += fields[1];
        if (hit) {
            reply.push_back(0x1f);
            reply += value;
        }
        void* postFn = nullptr;
        if (P0GetValidatedSlot(sender, kWebViewSlotPostWebMessageAsString,
                               &postFn)) {
            HRESULT hr = reinterpret_cast<FnPostWebMessageAsString>(postFn)(
                sender, reply.c_str());
            if (FAILED(hr)) {
                Wh_Log(L"[p1] cache reply could not be posted (%08X)",
                       static_cast<unsigned>(hr));
            }
        }
        return;
    }

    if (kind == L"cache-put") {
        g_p1PageReady.store(true);
        std::vector<std::wstring> fields;
        P1Split(rest, 0x1f, 2, fields);
        if (fields.size() != 2 || fields[0].empty() || fields[1].empty()) {
            if (!g_p1LogMalformed.exchange(true, std::memory_order_relaxed)) {
                Wh_Log(L"[p1] cache-put with an empty key or value; ignored "
                       L"(guard against empty writes, not a security boundary)");
            }
            return;
        }
        bool accepted = false;
        try {
            accepted = P1CachePut(fields[0], fields[1], P1CacheTtlDays());
        } catch (...) {
            accepted = false;
        }
        if (!accepted &&
            !g_p1LogCachePutRejected.exchange(true,
                                              std::memory_order_relaxed)) {
            Wh_Log(L"[p1] cache-put could not be stored (out of memory or the "
                   L"mod is shutting down)");
        }
        return;
    }

    if (kind == L"teardown") {
        // The page never sends this; the mod sends it. Ignored on purpose.
        if (!g_p1LogMalformed.exchange(true, std::memory_order_relaxed)) {
            Wh_Log(L"[p1] unexpected teardown message from the page; ignored");
        }
        return;
    }

    if (!g_p1LogUnknownKind.exchange(true, std::memory_order_relaxed)) {
        std::wstring kindForLog = P1ClipForLog(kind, 32);
        Wh_Log(L"[p1] unknown message kind '%s' ignored", kindForLog.c_str());
    }
}

// Runs the P0 script in the webview's current document. `label` must be a
// string literal: the completion handler stores the pointer and runs later.
// `when`, when not null, labels the log line with the injection path.
static HRESULT P0ExecuteScript(void* webView,
                               const wchar_t* label,
                               const wchar_t* when) {
    if (!webView) {
        Wh_Log(L"[p0] ExecuteScript(%s) skipped: no webview", label);
        return E_POINTER;
    }

    void* executeScriptFn = nullptr;
    if (!P0GetValidatedSlot(webView, kWebViewSlotExecuteScript,
                            &executeScriptFn)) {
        Wh_Log(L"[p0] ExecuteScript(%s) skipped: slot invalid", label);
        return E_FAIL;
    }

    P0ScriptHandler* handler = P0CreateScriptHandler(label);
    if (!handler) {
        Wh_Log(L"[p0] ExecuteScript(%s) skipped: no completion handler", label);
        return E_OUTOFMEMORY;
    }
    P0HandlerRef handlerRef;
    handlerRef.handler = handler;

    P0MarkHandedOver(handler);
    HRESULT hr = reinterpret_cast<FnExecuteScript>(executeScriptFn)(
        webView, kP0Js, handler);
    handler->immediateHr = hr;

    Wh_Log(L"[p0] %s (when=%s) -> %08X", label, when ? when : L"immediate",
           static_cast<unsigned>(hr));
    return hr;
}

// ---------------------------------------------------------------------------
// Controller -> WebView2 setup
// ---------------------------------------------------------------------------

static void P0OnControllerReady(void* controller) {
    if (!controller) {
        return;
    }

    // This callback runs on the WebView2 UI thread. The dispatcher window is a
    // prerequisite for injection: without it replies could not reach this
    // thread and the unloader could not request the teardown, so nothing is
    // injected at all.
    P1NoteUiThread();
    if (!P1IsRunning()) {
        Wh_Log(L"[p0] controller completed after shutdown; not injecting");
        return;
    }
    if (!P1EnsureDispatchWindow()) {
        if (!g_p1DispatchFailureLogged.exchange(true,
                                               std::memory_order_relaxed)) {
            Wh_Log(L"[p0] dispatch window unavailable; injection skipped");
        }
        return;
    }

    {
        std::lock_guard<std::mutex> lock(g_webViewMutex);
        if (g_webView) {
            Wh_Log(L"[p0] controller=%p: a webview is already set up; ignoring",
                   controller);
            return;
        }
    }

    void* getWebViewFn = nullptr;
    if (!P0GetValidatedSlot(controller, kControllerSlotGetCoreWebView2,
                            &getWebViewFn)) {
        Wh_Log(L"[p0] controller=%p: get_CoreWebView2 slot invalid", controller);
        return;
    }

    void* webView = nullptr;
    HRESULT hr =
        reinterpret_cast<FnControllerGetCoreWebView2>(getWebViewFn)(
            controller, &webView);
    if (FAILED(hr) || !webView) {
        Wh_Log(L"[p0] controller=%p: get_CoreWebView2 -> %08X, webview=%p",
               controller, static_cast<unsigned>(hr), webView);
        return;
    }

    Wh_Log(L"[p0] controller=%p CoreWebView2=%p", controller, webView);

    P0ComRef webViewRef;
    webViewRef.object = webView;

    // 1) Make sure the page may post messages.
    void* getSettingsFn = nullptr;
    if (P0GetValidatedSlot(webView, kWebViewSlotGetSettings, &getSettingsFn)) {
        void* settings = nullptr;
        hr = reinterpret_cast<FnGetSettings>(getSettingsFn)(webView, &settings);
        if (SUCCEEDED(hr) && settings) {
            P0ComRef settingsRef;
            settingsRef.object = settings;

            BOOL enabled = TRUE;
            void* getEnabledFn = nullptr;
            if (P0GetValidatedSlot(settings, kSettingsSlotGetIsWebMessageEnabled,
                                   &getEnabledFn)) {
                HRESULT getHr =
                    reinterpret_cast<FnGetBool>(getEnabledFn)(settings, &enabled);
                if (FAILED(getHr)) {
                    enabled = TRUE;
                    Wh_Log(L"[p0] get_IsWebMessageEnabled -> %08X",
                           static_cast<unsigned>(getHr));
                }
            } else {
                Wh_Log(L"[p0] get_IsWebMessageEnabled slot invalid");
            }

            Wh_Log(L"[p0] IsWebMessageEnabled=%d", enabled ? 1 : 0);
            if (!enabled) {
                void* putEnabledFn = nullptr;
                if (P0GetValidatedSlot(settings,
                                       kSettingsSlotPutIsWebMessageEnabled,
                                       &putEnabledFn)) {
                    HRESULT putHr = reinterpret_cast<FnPutBool>(putEnabledFn)(
                        settings, TRUE);
                    Wh_Log(L"[p0] put_IsWebMessageEnabled(TRUE) -> %08X",
                           static_cast<unsigned>(putHr));
                } else {
                    Wh_Log(L"[p0] put_IsWebMessageEnabled slot invalid");
                }
            }
        } else {
            Wh_Log(L"[p0] get_Settings -> %08X, settings=%p",
                   static_cast<unsigned>(hr), settings);
        }
    } else {
        Wh_Log(L"[p0] get_Settings slot invalid");
    }

    // 2) Register the script for future documents. The handler keeps its own
    //    webview reference until the asynchronous completion arrives: the
    //    completion may run after teardown and must still be able to remove
    //    the script.
    void* addScriptFn = nullptr;
    if (P0GetValidatedSlot(webView, kWebViewSlotAddScript, &addScriptFn)) {
        P0HandlerRef scriptHandler;
        scriptHandler.handler = P0CreateScriptHandler(L"AddScript");
        if (scriptHandler.handler) {
            auto* script = static_cast<P0ScriptHandler*>(scriptHandler.handler);
            auto addRef =
                reinterpret_cast<FnAddRef>(P0GetRawSlot(webView, kSlotAddRef));
            if (!addRef) {
                Wh_Log(L"[p0] AddScript skipped: no AddRef slot, so the script "
                       L"could not be removed at unload");
            } else {
                addRef(webView);
                script->webView = webView;
                P0CountAddScriptRefHeld();
                P0MarkHandedOver(scriptHandler.handler);
                HRESULT addHr = reinterpret_cast<FnAddScript>(addScriptFn)(
                    webView, kP0Js, scriptHandler.handler);
                g_addScriptHr.store(static_cast<unsigned long>(addHr));
                Wh_Log(L"[p0] AddScriptToExecuteOnDocumentCreated -> %08X",
                       static_cast<unsigned>(addHr));
            }
        } else {
            Wh_Log(L"[p0] out of memory for the AddScript completion handler");
        }
    } else {
        Wh_Log(L"[p0] AddScriptToExecuteOnDocumentCreated slot invalid");
    }

    // 3) Register the message handler before running any script, so that the
    //    hello message can be answered.
    void* addMessageFn = nullptr;
    if (P0GetValidatedSlot(webView, kWebViewSlotAddWebMessageReceived,
                           &addMessageFn)) {
        P0HandlerRef messageHandler;
        messageHandler.handler = P0CreateMessageHandler();
        if (messageHandler.handler) {
            EventRegistrationToken token{};
            P0MarkHandedOver(messageHandler.handler);
            HRESULT addHr = reinterpret_cast<FnAddWebMessageReceived>(
                addMessageFn)(webView, messageHandler.handler, &token);
            if (SUCCEEDED(addHr) && token.value != 0) {
                if (P0StoreWebView(webView, token, static_cast<P0MessageHandler*>(
                                                      messageHandler.handler))) {
                    // The global state now owns the handler reference.
                    messageHandler.handler = nullptr;
                    Wh_Log(L"[p0] add_WebMessageReceived -> %08X token=%lld",
                           static_cast<unsigned>(addHr),
                           static_cast<long long>(token.value));
                } else {
                    void* removeFn = nullptr;
                    if (P0GetValidatedSlot(webView,
                                           kWebViewSlotRemoveWebMessageReceived,
                                           &removeFn)) {
                        reinterpret_cast<FnRemoveWebMessageReceived>(removeFn)(
                            webView, token);
                    }
                    Wh_Log(L"[p0] add_WebMessageReceived raced; removed token");
                }
            } else {
                Wh_Log(L"[p0] add_WebMessageReceived -> %08X token=%lld",
                       static_cast<unsigned>(addHr),
                       static_cast<long long>(token.value));
            }
        } else {
            Wh_Log(L"[p0] out of memory for message handler");
        }
    } else {
        Wh_Log(L"[p0] add_WebMessageReceived slot invalid");
    }

    // 4) Navigation events. AddScriptToExecuteOnDocumentCreated completes
    //    asynchronously, after wry has already started navigating, so the first
    //    real document can miss the registered script. Logging both events and
    //    injecting again on NavigationCompleted makes that visible and fixed.
    bool webViewStored = false;
    {
        std::lock_guard<std::mutex> lock(g_webViewMutex);
        webViewStored = (g_webView == webView);
    }

    if (!webViewStored) {
        Wh_Log(L"[p0] navigation events not registered: webview was not stored");
    } else {
        void* addNavStartingFn = nullptr;
        if (P0GetValidatedSlot(webView, kWebViewSlotAddNavigationStarting,
                               &addNavStartingFn)) {
            P0HandlerRef navHandler;
            navHandler.handler = P0CreateNavigationStartingHandler();
            if (navHandler.handler) {
                EventRegistrationToken navToken{};
                P0MarkHandedOver(navHandler.handler);
                HRESULT addHr = reinterpret_cast<FnAddNavigationEventHandler>(
                    addNavStartingFn)(webView, navHandler.handler, &navToken);
                if (SUCCEEDED(addHr) && navToken.value != 0) {
                    {
                        std::lock_guard<std::mutex> lock(g_webViewMutex);
                        g_navigationStartingToken = navToken;
                        g_navigationStartingHandler =
                            static_cast<P0NavigationStartingHandler*>(
                                navHandler.handler);
                        // The global state now owns the handler reference.
                        navHandler.handler = nullptr;
                    }
                    Wh_Log(L"[p0] add_NavigationStarting -> %08X token=%lld",
                           static_cast<unsigned>(addHr),
                           static_cast<long long>(navToken.value));
                } else {
                    Wh_Log(L"[p0] add_NavigationStarting -> %08X token=%lld "
                           L"(not registered)",
                           static_cast<unsigned>(addHr),
                           static_cast<long long>(navToken.value));
                }
            } else {
                Wh_Log(L"[p0] out of memory for NavigationStarting handler");
            }
        } else {
            Wh_Log(L"[p0] add_NavigationStarting slot invalid");
        }

        void* addNavCompletedFn = nullptr;
        if (P0GetValidatedSlot(webView, kWebViewSlotAddNavigationCompleted,
                               &addNavCompletedFn)) {
            P0HandlerRef navHandler;
            navHandler.handler = P0CreateNavigationCompletedHandler();
            if (navHandler.handler) {
                EventRegistrationToken navToken{};
                P0MarkHandedOver(navHandler.handler);
                HRESULT addHr = reinterpret_cast<FnAddNavigationEventHandler>(
                    addNavCompletedFn)(webView, navHandler.handler, &navToken);
                if (SUCCEEDED(addHr) && navToken.value != 0) {
                    {
                        std::lock_guard<std::mutex> lock(g_webViewMutex);
                        g_navigationCompletedToken = navToken;
                        g_navigationCompletedHandler =
                            static_cast<P0NavigationCompletedHandler*>(
                                navHandler.handler);
                        navHandler.handler = nullptr;
                    }
                    Wh_Log(L"[p0] add_NavigationCompleted -> %08X token=%lld",
                           static_cast<unsigned>(addHr),
                           static_cast<long long>(navToken.value));
                } else {
                    Wh_Log(L"[p0] add_NavigationCompleted -> %08X token=%lld "
                           L"(not registered)",
                           static_cast<unsigned>(addHr),
                           static_cast<long long>(navToken.value));
                }
            } else {
                Wh_Log(L"[p0] out of memory for NavigationCompleted handler");
            }
        } else {
            Wh_Log(L"[p0] add_NavigationCompleted slot invalid");
        }
    }

    // 5) Fallback for the current document, often still about:blank; that is why
    //    NavigationCompleted injects again. The script is idempotent. Without a
    //    stored webview there is no message handler, so injecting would only
    //    create a page that can never talk to the mod.
    if (!webViewStored) {
        Wh_Log(L"[p0] ExecuteScript fallback skipped: no message handler");
        return;
    }
    HRESULT execHr = P0ExecuteScript(webView, L"ExecuteScript-fallback", nullptr);
    Wh_Log(L"ExecuteScript fallback hr=0x%08X", static_cast<unsigned>(execHr));
}

// ---------------------------------------------------------------------------
// Callback implementations
// ---------------------------------------------------------------------------

static HRESULT STDMETHODCALLTYPE P0EnvHandlerInvoke(P0EnvHandler* self,
                                                    HRESULT hr,
                                                    void* env) {
    try {
        Wh_Log(L"[p0] environment completed: hr=%08X env=%p",
               static_cast<unsigned>(hr), env);

        // The app's continuation is forwarded either way; only our own work
        // is gated by the lifecycle state.
        if (P1IsRunning() && SUCCEEDED(hr) && env) {
            P0PatchEnvironment(env);
        } else if (SUCCEEDED(hr) && env) {
            Wh_Log(L"[p0] environment completed after shutdown; not patching");
        } else {
            Wh_Log(L"[p0] no environment to patch; forwarding as-is");
        }

        return P0SafeForwardEnvHandler(self ? self->app : nullptr, hr, env);
    } catch (...) {
        Wh_Log(L"[p0] exception in environment callback");
        return S_OK;
    }
}

static HRESULT STDMETHODCALLTYPE P0ControllerHandlerInvoke(
    P0ControllerHandler* self,
    HRESULT hr,
    void* controller) {
    try {
        Wh_Log(L"[p0] controller completed: hr=%08X controller=%p",
               static_cast<unsigned>(hr), controller);

        if (P1IsRunning() && SUCCEEDED(hr) && controller) {
            P0OnControllerReady(controller);
        }

        return P0SafeForwardControllerHandler(self ? self->app : nullptr, hr,
                                              controller);
    } catch (...) {
        Wh_Log(L"[p0] exception in controller callback");
        return S_OK;
    }
}

static HRESULT STDMETHODCALLTYPE P0MessageHandlerInvoke(
    P0MessageHandler* self,
    void* sender,
    void* args) {
    (void)self;

    try {
        if (!sender || !args) {
            return S_OK;
        }

        // WebView2 raises this event on the UI thread. P1HandleMessage owns
        // the dispatcher/drain/settings work; this callback only decodes the
        // transport and enforces the lifecycle gate.
        P1NoteUiThread();
        if (!P1IsRunning()) {
            return S_OK;
        }

        void* tryGetFn = nullptr;
        if (!P0GetValidatedSlot(args, kArgsSlotTryGetWebMessageAsString,
                                &tryGetFn)) {
            Wh_Log(L"[p1] TryGetWebMessageAsString slot invalid");
            return S_OK;
        }

        LPWSTR text = nullptr;
        HRESULT hr =
            reinterpret_cast<FnTryGetWebMessageAsString>(tryGetFn)(args, &text);
        if (FAILED(hr) || !text) {
            if (text) {
                CoTaskMemFree(text);
            }
            // Non-string messages are not part of the protocol; log once.
            static std::atomic<bool> nonStringLogged{false};
            if (!nonStringLogged.exchange(true, std::memory_order_relaxed)) {
                Wh_Log(L"[p1] non-string web message ignored (%08X)",
                       static_cast<unsigned>(hr));
            }
            return S_OK;
        }

        if (wcsncmp(text, kP1Tag, 5) == 0) {
            try {
                std::wstring message(text);
                P1HandleMessage(sender, message);
            } catch (...) {
                Wh_Log(L"[p1] exception while handling a page message");
            }
        } else {
            static std::atomic<bool> unknownPrefixLogged{false};
            if (!unknownPrefixLogged.exchange(true, std::memory_order_relaxed)) {
                Wh_Log(L"[p1] message with an unknown prefix ignored: %.200s",
                       text ? text : L"(null)");
            }
        }

        CoTaskMemFree(text);
    } catch (...) {
        Wh_Log(L"[p1] exception in message handler");
    }

    return S_OK;
}

static HRESULT STDMETHODCALLTYPE P0ScriptHandlerInvoke(
    P0ScriptHandler* self,
    HRESULT hr,
    LPCWSTR text) {
    const wchar_t* label = self && self->label ? self->label : L"?";
    try {
        if (self && wcscmp(label, L"AddScript") == 0) {
            // Handle the result before any lifecycle check. A completion that
            // arrives after Stopping was set still carries the script id and
            // must remove the document-created script; the teardown's own
            // removal already ran (or will run without an id).
            if (text && *text) {
                P0SetScriptId(text);
                if (!P1IsRunning()) {
                    // We are on the WebView2 UI thread, the thread the removal
                    // requires. The reference kept in self->webView is still
                    // valid here even though P0CleanupWebView has run.
                    try {
                        P0RemoveRegisteredScript(self->webView);
                    } catch (...) {
                        Wh_Log(L"[p0] exception while removing the AddScript "
                               L"script");
                    }
                }
            }
            P0ReleaseScriptWebView(self);

            Wh_Log(L"[p0] AddScript completed: hr=%08X id=%.200s running=%d",
                   static_cast<unsigned>(g_addScriptHr.load()),
                   text ? text : L"(null)", P1IsRunning() ? 1 : 0);
            if (!P1IsRunning()) {
                return S_OK;
            }

            // AddScriptToExecuteOnDocumentCreated finishes asynchronously. By
            // the time this fires, wry has usually started the first real
            // navigation, so injecting here has a much better chance of hitting
            // the app document than the controller-path fallback that ran while
            // it was still about:blank. Done at most once.
            if (!g_postAddScriptInjectionDone.exchange(
                    true, std::memory_order_acq_rel)) {
                void* webView = P0AcquireWebView();
                if (webView) {
                    P0ComRef webViewRef;
                    webViewRef.object = webView;
                    Wh_Log(L"[p0] AddScript completed late; injecting into the "
                           L"current document");
                    P0ExecuteScript(webView, L"ExecuteScript-afterAddScript",
                                    L"afteraddscript");
                } else {
                    Wh_Log(L"[p0] AddScript completed but no webview is stored; "
                           L"relying on NavigationCompleted");
                }
            }
            return S_OK;
        }

        if (!P1IsRunning()) {
            return S_OK;
        }

        HRESULT immediateHr = self ? self->immediateHr : E_FAIL;
        if (wcscmp(label, L"ExecuteScript-fallback") == 0) {
            Wh_Log(L"[p0] ExecuteScript fallback: hr=%08X result=%.200s",
                   static_cast<unsigned>(immediateHr),
                   text ? text : L"(null)");
        } else if (wcscmp(label, L"ExecuteScript-afterAddScript") == 0) {
            Wh_Log(L"[p0] ExecuteScript afterAddScript: hr=%08X result=%.200s",
                   static_cast<unsigned>(immediateHr),
                   text ? text : L"(null)");
        } else if (wcscmp(label, L"ExecuteScript-navcompleted") == 0) {
            Wh_Log(L"[p0] ExecuteScript navcompleted: hr=%08X result=%.200s",
                   static_cast<unsigned>(immediateHr),
                   text ? text : L"(null)");
        }
    } catch (...) {
    }
    return S_OK;
}

// NavigationStarting records the URI for the matching NavigationCompleted log.
// Fail-soft: a missing args/slot is logged and S_OK is still returned.
static HRESULT STDMETHODCALLTYPE P0NavigationStartingInvoke(
    P0NavigationStartingHandler* self,
    void* sender,
    void* args) {
    (void)self;
    (void)sender;

    if (!P1IsRunning()) {
        return S_OK;
    }

    try {
        if (!args) {
            Wh_Log(L"[p0] NavigationStarting: no event args");
            return S_OK;
        }

        void* getUriFn = nullptr;
        if (!P0GetValidatedSlot(args, kNavigationStartingArgsSlotGetUri,
                                &getUriFn)) {
            Wh_Log(L"[p0] NavigationStarting: get_Uri slot invalid");
            return S_OK;
        }

        LPWSTR rawUri = nullptr;
        HRESULT getHr = reinterpret_cast<FnGetString>(getUriFn)(args, &rawUri);
        std::wstring uri;
        if (SUCCEEDED(getHr) && rawUri) {
            try {
                uri.assign(rawUri);
            } catch (...) {
            }
        }
        if (rawUri) {
            CoTaskMemFree(rawUri);
        }

        if (FAILED(getHr)) {
            Wh_Log(L"[p0] NavigationStarting: get_Uri -> %08X",
                   static_cast<unsigned>(getHr));
            return S_OK;
        }

        P0SetLastNavigationUri(uri);
        Wh_Log(L"[p0] NavigationStarting: uri=%.200s get_Uri=0x%08X",
               uri.c_str(), static_cast<unsigned>(getHr));
    } catch (...) {
        Wh_Log(L"[p0] exception in NavigationStarting handler");
    }

    return S_OK;
}

// NavigationCompleted logs IsSuccess/WebErrorStatus/NavigationId and injects the
// P0 script into the document that now exists: the fix for the first real
// document missing AddScriptToExecuteOnDocumentCreated.
static HRESULT STDMETHODCALLTYPE P0NavigationCompletedInvoke(
    P0NavigationCompletedHandler* self,
    void* sender,
    void* args) {
    (void)self;

    if (!P1IsRunning()) {
        return S_OK;
    }

    try {
        BOOL isSuccess = FALSE;
        int webErrorStatus = -1;
        UINT64 navigationId = 0;
        HRESULT successHr = E_POINTER;
        HRESULT errorStatusHr = E_POINTER;
        HRESULT navigationIdHr = E_POINTER;

        if (!args) {
            Wh_Log(L"[p0] NavigationCompleted: no event args");
        } else {
            void* fn = nullptr;
            if (P0GetValidatedSlot(
                    args, kNavigationCompletedArgsSlotGetIsSuccess, &fn)) {
                successHr = reinterpret_cast<FnGetBool>(fn)(args, &isSuccess);
            } else {
                Wh_Log(L"[p0] NavigationCompleted: get_IsSuccess slot invalid");
            }

            fn = nullptr;
            if (P0GetValidatedSlot(
                    args, kNavigationCompletedArgsSlotGetWebErrorStatus, &fn)) {
                errorStatusHr = reinterpret_cast<FnGetInt32>(fn)(
                    args, &webErrorStatus);
            } else {
                Wh_Log(
                    L"[p0] NavigationCompleted: get_WebErrorStatus slot invalid");
            }

            fn = nullptr;
            if (P0GetValidatedSlot(
                    args, kNavigationCompletedArgsSlotGetNavigationId, &fn)) {
                navigationIdHr =
                    reinterpret_cast<FnGetUInt64>(fn)(args, &navigationId);
            }

            if (FAILED(successHr)) {
                Wh_Log(L"[p0] NavigationCompleted: get_IsSuccess -> %08X",
                       static_cast<unsigned>(successHr));
            }
            if (FAILED(errorStatusHr)) {
                Wh_Log(L"[p0] NavigationCompleted: get_WebErrorStatus -> %08X",
                       static_cast<unsigned>(errorStatusHr));
            }
            if (FAILED(navigationIdHr)) {
                Wh_Log(L"[p0] NavigationCompleted: get_NavigationId -> %08X",
                       static_cast<unsigned>(navigationIdHr));
            }
        }

        std::wstring uri = P0GetLastNavigationUri();
        Wh_Log(L"[p0] NavigationCompleted: uri=%.200s success=%d "
               L"webErrorStatus=%d navigationId=%llu",
               uri.c_str(), isSuccess ? 1 : 0, webErrorStatus,
               static_cast<unsigned long long>(navigationId));

        // sender is the ICoreWebView2 that raised the event; use it directly.
        // Only fall back to the stored webview (with its own reference) when it
        // is missing.
        void* target = sender;
        P0ComRef acquired;
        if (!target) {
            target = P0AcquireWebView();
            acquired.object = target;
        }
        if (!target) {
            Wh_Log(L"[p0] NavigationCompleted: sender is null and no webview is "
                   L"stored");
            return S_OK;
        }

        Wh_Log(L"[p0] NavigationCompleted: late injection into the completed "
               L"document");
        P0ExecuteScript(target, L"ExecuteScript-navcompleted", L"navcompleted");
    } catch (...) {
        Wh_Log(L"[p0] exception in NavigationCompleted handler");
    }

    return S_OK;
}

// ---------------------------------------------------------------------------
// Environment method thunks
// ---------------------------------------------------------------------------

static HRESULT STDMETHODCALLTYPE P0EnvCreateControllerThunk(void* self,
                                                            HWND parent,
                                                            void* handler) {
    try {
        Wh_Log(L"[p0] env->CreateCoreWebView2Controller(parent=%p handler=%p)",
               reinterpret_cast<void*>(parent), handler);

        auto original = reinterpret_cast<FnEnvCreateController>(
            g_origEnvCreateController.load(std::memory_order_acquire));
        if (!original) {
            Wh_Log(L"[p0] original CreateCoreWebView2Controller missing");
            return E_FAIL;
        }

        if (!handler || !P1IsRunning()) {
            return original(self, parent, handler);
        }

        P0HandlerRef wrapped;
        wrapped.handler = P0CreateControllerHandler(handler);
        if (!wrapped.handler) {
            Wh_Log(L"[p0] out of memory for controller handler; passing through");
            return original(self, parent, handler);
        }

        P0MarkHandedOver(wrapped.handler);
        HRESULT hr = original(self, parent, wrapped.handler);
        Wh_Log(L"[p0] CreateCoreWebView2Controller -> %08X",
               static_cast<unsigned>(hr));
        return hr;
    } catch (...) {
        Wh_Log(L"[p0] exception in CreateCoreWebView2Controller thunk");
        return E_FAIL;
    }
}

static HRESULT STDMETHODCALLTYPE P0EnvCreateControllerWithOptionsThunk(
    void* self,
    HWND parent,
    void* options,
    void* handler) {
    try {
        Wh_Log(L"[p0] env->CreateCoreWebView2ControllerWithOptions(parent=%p "
               L"options=%p handler=%p)",
               reinterpret_cast<void*>(parent), options, handler);

        auto original = reinterpret_cast<FnEnvCreateControllerWithOptions>(
            g_origEnvCreateControllerWithOptions.load(std::memory_order_acquire));
        if (!original) {
            Wh_Log(L"[p0] original CreateCoreWebView2ControllerWithOptions "
                   L"missing");
            return E_FAIL;
        }

        if (!handler || !P1IsRunning()) {
            return original(self, parent, options, handler);
        }

        P0HandlerRef wrapped;
        wrapped.handler = P0CreateControllerHandler(handler);
        if (!wrapped.handler) {
            Wh_Log(L"[p0] out of memory for controller handler; passing through");
            return original(self, parent, options, handler);
        }

        P0MarkHandedOver(wrapped.handler);
        HRESULT hr = original(self, parent, options, wrapped.handler);
        Wh_Log(L"[p0] CreateCoreWebView2ControllerWithOptions -> %08X",
               static_cast<unsigned>(hr));
        return hr;
    } catch (...) {
        Wh_Log(L"[p0] exception in CreateCoreWebView2ControllerWithOptions "
               L"thunk");
        return E_FAIL;
    }
}

// ---------------------------------------------------------------------------
// GetProcAddress hook + environment creation shim
// ---------------------------------------------------------------------------

static FnGetProcAddress g_originalGetProcAddress = nullptr;
static std::atomic<FnCreateEnv> g_realCreateEnv{nullptr};
static std::atomic<bool> g_createExportLogged{false};
static std::atomic<bool> g_sharedExportLogged{false};

static HRESULT STDMETHODCALLTYPE P0CreateEnvShim(BOOL fromClientDll,
                                                 int runtimeType,
                                                 PCWSTR userDataFolder,
                                                 IUnknown* environmentOptions,
                                                 void* handler);

static FARPROC WINAPI P0GetProcAddressHook(HMODULE module, LPCSTR procName) {
    // Once Stopping was set there is no wrapper work left to do, so the
    // original export is returned untouched.
    if (!g_originalGetProcAddress) {
        return nullptr;
    }
    if (!P1IsRunning()) {
        return g_originalGetProcAddress(module, procName);
    }

    __try {
        if (module && procName &&
            HIWORD(reinterpret_cast<ULONG_PTR>(procName))) {
            // Check the two export names before touching the module list:
            // GetModuleHandleW takes the loader lock, and this hook runs for
            // every GetProcAddress call in the process.
            bool isCreate =
                strcmp(procName,
                       "CreateWebViewEnvironmentWithOptionsInternal") == 0;
            bool isCreateShared =
                strcmp(procName,
                       "CreateSharedWebViewEnvironmentInternal") == 0;

            if ((isCreate || isCreateShared) &&
                module == P0GetEmbeddedBrowserModule()) {
                if (isCreate) {
                    // Resolve the real export once, through the unhooked
                    // trampoline. Returning the shim is cheap and does not
                    // recurse into this hook.
                    auto real = reinterpret_cast<FnCreateEnv>(
                        g_originalGetProcAddress(module, procName));
                    if (real) {
                        g_realCreateEnv.store(real, std::memory_order_release);

                        // The dispatch window is a prerequisite for every
                        // wrapper this mod hands to WebView2. Create it here,
                        // on the environment-creation (UI) thread, before the
                        // shim can wrap anything. If it cannot be created,
                        // return the real export unchanged: no wrapper, no
                        // script, no message handler, nothing owned by the mod.
                        P1NoteUiThread();
                        if (!P1EnsureDispatchWindow()) {
                            if (!g_p1DispatchFailureLogged.exchange(
                                    true, std::memory_order_relaxed)) {
                                Wh_Log(L"[p0] GetProcAddress("
                                       L"EmbeddedBrowserWebView.dll, "
                                       L"CreateWebViewEnvironmentWithOptions"
                                       L"Internal) -> real export (dispatch "
                                       L"window unavailable)");
                            }
                            return reinterpret_cast<FARPROC>(real);
                        }

                        if (!g_createExportLogged.exchange(
                                true, std::memory_order_relaxed)) {
                            Wh_Log(L"GetProcAddress(EmbeddedBrowserWebView.dll, "
                                   L"CreateWebViewEnvironmentWithOptionsInternal) "
                                   L"-> shim original=%p",
                                   reinterpret_cast<void*>(real));
                        }
                        return reinterpret_cast<FARPROC>(&P0CreateEnvShim);
                    }
                    if (!g_createExportLogged.exchange(
                            true, std::memory_order_relaxed)) {
                        Wh_Log(L"GetProcAddress(EmbeddedBrowserWebView.dll, "
                               L"CreateWebViewEnvironmentWithOptionsInternal) "
                               L"-> unresolved");
                    }
                } else {
                    // This export's exact signature could not be verified
                    // locally. It is not used by wry's normal environment path,
                    // so pass it through untouched instead of risking a
                    // mismatched call. Logged once for diagnostics.
                    FARPROC original =
                        g_originalGetProcAddress(module, procName);
                    if (!g_sharedExportLogged.exchange(
                            true, std::memory_order_relaxed)) {
                        Wh_Log(L"[p0] GetProcAddress(EmbeddedBrowserWebView.dll, "
                               L"CreateSharedWebViewEnvironmentInternal) -> "
                               L"passthrough original=%p",
                               reinterpret_cast<void*>(original));
                    }
                }
            }
        }
    } __except (EXCEPTION_EXECUTE_HANDLER) {
    }

    return g_originalGetProcAddress(module, procName);
}

static HRESULT STDMETHODCALLTYPE P0CreateEnvShim(
    BOOL fromClientDll,
    int runtimeType,
    PCWSTR userDataFolder,
    IUnknown* environmentOptions,
    void* handler) {
    try {
        Wh_Log(L"[p0] CreateWebViewEnvironmentWithOptionsInternal: "
               L"fromClientDll=%d runtimeType=%d userDataFolder=%.200s handler=%p",
               static_cast<int>(fromClientDll), runtimeType,
               userDataFolder ? userDataFolder : L"(null)", handler);

        FnCreateEnv real = g_realCreateEnv.load(std::memory_order_acquire);
        if (!real) {
            Wh_Log(L"[p0] real CreateWebViewEnvironmentWithOptionsInternal "
                   L"not resolved");
            return E_FAIL;
        }

        if (!handler || !P1IsRunning()) {
            HRESULT passthroughHr = real(fromClientDll, runtimeType,
                                         userDataFolder, environmentOptions,
                                         handler);
            Wh_Log(L"original createEnv returned hr=0x%08X",
                   static_cast<unsigned>(passthroughHr));
            return passthroughHr;
        }

        // The hook created the dispatch window before returning this shim; if
        // it is gone now, pass the original handler through unwrapped rather
        // than handing WebView2 something the mod could not tear down.
        P1NoteUiThread();
        if (!P1EnsureDispatchWindow()) {
            if (!g_p1DispatchFailureLogged.exchange(true,
                                                    std::memory_order_relaxed)) {
                Wh_Log(L"[p0] dispatch window unavailable; environment handler "
                       L"passed through unwrapped");
            }
            return real(fromClientDll, runtimeType, userDataFolder,
                        environmentOptions, handler);
        }

        P0HandlerRef wrapped;
        wrapped.handler = P0CreateEnvHandler(handler);
        if (!wrapped.handler) {
            Wh_Log(L"[p0] out of memory for environment handler; passing through");
            return real(fromClientDll, runtimeType, userDataFolder,
                        environmentOptions, handler);
        }

        P0MarkHandedOver(wrapped.handler);
        HRESULT hr = real(fromClientDll, runtimeType, userDataFolder,
                          environmentOptions, wrapped.handler);
        Wh_Log(L"[p0] environment creation call -> %08X",
               static_cast<unsigned>(hr));
        return hr;
    } catch (...) {
        Wh_Log(L"[p0] exception in environment creation shim");
        return E_FAIL;
    }
}

// ---------------------------------------------------------------------------
// Mod entry points
// ---------------------------------------------------------------------------

BOOL Wh_ModInit() {
    // Created before the GetProcAddress hook can admit a wrapper: the engine
    // thread uses it to learn when the UI thread released every wrapper. If it
    // cannot be created, the mod does not load.
    g_p1TeardownEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_p1TeardownEvent) {
        Wh_Log(L"[p1] CreateEventW failed (%lu); not loading", GetLastError());
        g_p1Lifecycle.store(P1Lifecycle::Stopped, std::memory_order_release);
        return FALSE;
    }

    wchar_t exePath[MAX_PATH]{};
    GetModuleFileNameW(nullptr, exePath, ARRAYSIZE(exePath));

    // The dispatch window class belongs to this module, not to the host
    // image. The same handle unregisters it at unload.
    if (!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                                GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                            reinterpret_cast<LPCWSTR>(&P1DispatchWndProc),
                            &g_modModule)) {
        g_modModule = nullptr;
        Wh_Log(L"[p1] GetModuleHandleExW failed (%lu); the dispatcher window is "
               L"unavailable",
               GetLastError());
    }

    Wh_Log(L"mod init: pid=%lu exe=%.200s",
           static_cast<unsigned long>(GetCurrentProcessId()), exePath);
    Wh_Log(L"[p0] Wh_ModInit (P0 injection proof + P1 bridge)");

    P1CacheLoad();

    // Read the settings once at init: this also resolves the API key source so
    // the log shows whether a credential was found. The key itself is never
    // logged.
    try {
        P1SettingsSnapshot snapshot = P1ReadSettings();
        P1KeyInfo key = P1GetKeyInfo();
        Wh_Log(L"[p1] settings: targetLang=%s provider=%s keySource=%s "
               L"timeoutMs=%d concurrency=%d batchSize=%d batchChars=%d "
               L"clearCache=%d",
               snapshot.targetLang.c_str(), snapshot.provider.c_str(),
               key.source.c_str(), snapshot.timeoutMs, snapshot.concurrency,
               snapshot.batchSize, snapshot.batchChars,
               snapshot.clearCacheRequested ? 1 : 0);
        Wh_Log(L"[p1] llm base host=%s keyBytes=%llu",
               key.host.empty() ? L"(none)" : key.host.c_str(),
               static_cast<unsigned long long>(key.key.size()));
    } catch (...) {
        Wh_Log(L"[p1] exception while reading settings at init");
    }

    HMODULE kernel32 = GetModuleHandleW(L"kernel32.dll");
    if (!kernel32) {
        Wh_Log(L"[p0] kernel32.dll not found; aborting");
        g_p1Lifecycle.store(P1Lifecycle::Stopped, std::memory_order_release);
        return FALSE;
    }

    FARPROC getProcAddress = GetProcAddress(kernel32, "GetProcAddress");
    if (!getProcAddress) {
        Wh_Log(L"[p0] GetProcAddress not found; aborting");
        g_p1Lifecycle.store(P1Lifecycle::Stopped, std::memory_order_release);
        return FALSE;
    }

    // Running is set before the hook is installed: an environment creation
    // that races Wh_ModInit must see the mod as live.
    g_p1Lifecycle.store(P1Lifecycle::Running, std::memory_order_release);
    if (!Wh_SetFunctionHook(reinterpret_cast<void*>(getProcAddress),
                            reinterpret_cast<void*>(&P0GetProcAddressHook),
                            reinterpret_cast<void**>(
                                &g_originalGetProcAddress))) {
        Wh_Log(L"[p0] failed to hook GetProcAddress; aborting");
        g_p1Lifecycle.store(P1Lifecycle::Stopped, std::memory_order_release);
        return FALSE;
    }

    Wh_Log(L"[p0] GetProcAddress hooked; waiting for WebView2 environment "
           L"creation");

    // The hook only sees environments created from here on. When the UI
    // already had one, this instance does nothing; the next UI start loads the
    // mod into a fresh process and catches its environment.
    return TRUE;
}

void Wh_ModSettingsChanged() {
    if (!P1IsRunning()) {
        return;
    }
    try {
        Wh_Log(L"[p1] mod settings changed");
        // Settings are read fresh for every push and request; the generation
        // only invalidates the cached credential lookup.
        g_p1SettingsGeneration.fetch_add(1, std::memory_order_acq_rel);
        P1ClearKeyCache();
        P1RequestSettingsPush();
    } catch (...) {
        Wh_Log(L"[p1] exception in Wh_ModSettingsChanged");
    }
}

void Wh_ModBeforeUninit() {
    Wh_Log(L"[p0] Wh_ModBeforeUninit");

    // First thing: no new work may be admitted from here on. Every entry point
    // checks this state and returns unless it is Running.
    P1Lifecycle previous =
        g_p1Lifecycle.exchange(P1Lifecycle::Stopping,
                               std::memory_order_acq_rel);
    if (previous != P1Lifecycle::Running) {
        if (previous == P1Lifecycle::Stopped) {
            Wh_Log(L"[p0] already stopped; nothing to do");
            // Init may have failed after the event was created, in which case
            // this is the only place that closes it.
            HANDLE teardownEvent = g_p1TeardownEvent;
            g_p1TeardownEvent = nullptr;
            if (teardownEvent) {
                CloseHandle(teardownEvent);
            }
            g_p1Lifecycle.store(P1Lifecycle::Stopped,
                                std::memory_order_release);
            return;
        }
        // previous == Stopping: another call owns the teardown.
        Wh_Log(L"[p0] concurrent uninit ignored; the first call owns the "
               L"teardown");
        return;
    }

    // Wait for an in-flight dispatch-window creation to settle: the creator
    // either publishes the window or sees Stopping and destroys its own.
    P1WaitForDispatchCreation();

    // Stop the worker pool and the cache saver, both joined with INFINITE.
    // Each is attempted even if the other throws: after the joins no worker can
    // return into this image, and a failure means the unload must not proceed.
    bool threadsStopped = true;
    try {
        if (!P1StopHttpWorkers()) {
            threadsStopped = false;
        }
    } catch (...) {
        Wh_Log(L"[p0] exception while stopping the http workers");
        threadsStopped = false;
    }
    try {
        if (!P1StopCacheSaver()) {
            threadsStopped = false;
        }
    } catch (...) {
        Wh_Log(L"[p0] exception while stopping the cache saver");
        threadsStopped = false;
    }
    if (!threadsStopped) {
        P1BlockUnload(L"the mod's threads could not be stopped");
    }

    // Ask the UI thread for the teardown, wait for every wrapper and held
    // AddScript reference to be released, then destroy the dispatch window.
    try {
        P1TeardownUiThread();
    } catch (...) {
        Wh_Log(L"[p0] exception in the UI-thread teardown");
        P1BlockUnload(L"the UI-thread teardown threw");
    }

    try {
        P1ClearKeyCache();
    } catch (...) {
        Wh_Log(L"[p0] exception while clearing the key cache");
    }

    g_p1Lifecycle.store(P1Lifecycle::Stopped, std::memory_order_release);
    HANDLE teardownEvent = g_p1TeardownEvent;
    g_p1TeardownEvent = nullptr;
    if (teardownEvent) {
        CloseHandle(teardownEvent);
    }
    Wh_Log(L"[p0] cleanup done");
}
