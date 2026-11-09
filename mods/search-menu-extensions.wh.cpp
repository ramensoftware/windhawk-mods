// ==WindhawkMod==
// @id              search-menu-extensions
// @name            New Windows Search Extensions (library)
// @description     For the NEW Windows 11 Insider search box (composer search, build 26320+), not classic Start/taskbar search. Lets other mods, scripts and programs add results, actions, toggles and cards, mixed in with the native results
// @version         0.1.0
// @author          TurtlesMaster1
// @github          https://github.com/TurtlesMaster1
// @include         shellhost.exe
// @compilerOptions -lole32 -loleaut32 -lruntimeobject -lshlwapi -lshell32 -luuid
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
> **Updated for Build 26320.9876 (Windows 11 Insider Beta).** Insider builds
> change quickly, so newer builds may break or move this.

# New Windows Search Extensions (library)

**This is only for the NEW Windows search**: the compact "composer" search box
on recent Windows 11 Insider builds (hosted by
`shellhost.exe loadComponent SystemComposerControl`, the one with inline
actions like "Turn on dark mode" and the volume slider). It does **nothing** for
the classic Start menu search, the old taskbar search flyout (`SearchHost.exe`)
or Windows 10.

![Demo provider items mixed into the new Windows search](https://raw.githubusercontent.com/TurtlesMaster1/windhawk-search-menu-extensions/main/images/demo.png)

A **dependency mod**: on its own it only adds an optional demo provider (type
`wh hello`). Other mods (or any program/script) use it to put items into the new
search box. Providers see every query and decide for themselves when to answer,
so no prefix is needed.

![A pipe client showing live weather with an Adaptive Card](https://raw.githubusercontent.com/TurtlesMaster1/windhawk-search-menu-extensions/main/images/weather.png)

*Above: `examples/weather_client.py`, an out-of-process provider talking over
the named pipe.*

Items are injected as genuine native search providers. Windows' own search is
built from *Palette extensions* (`Windows.UI.Palettes`): Apps, Files, Settings,
Windows Actions, Command Line and so on are each one extension. This mod adds
one more extension per registered provider, so your items are real
`PaletteItem`s. They mix into the native list with native ranking, keyboard
navigation, icons, toggles and Adaptive Card content.

## Two ways to provide items

### 1. From another Windhawk mod (in-process)

Your mod must also `@include shellhost.exe`. Find this library's DLL and call its
exported C API (Windhawk exports every symbol of a mod DLL):

```cpp
typedef struct WSE_ProviderCallbacks {
    UINT32 cbSize;
    void* ctx;
    // Called for every query (text may be empty = zero state). Answer with
    // WSE_SubmitResults(queryHandle, itemsJson, done) from any thread, now or later.
    void (WINAPI* onQuery)(void* ctx, UINT64 queryHandle, PCWSTR text, PCWSTR queryJson);
    // An item was invoked: actionId is "invoke", "toggle" or a card action id.
    void (WINAPI* onInvoke)(void* ctx, PCWSTR itemId, PCWSTR actionId, PCWSTR dataJson, PCWSTR inputsJson);
    // Optional. Query abandoned (user typed more / closed search).
    void (WINAPI* onCancel)(void* ctx, UINT64 queryHandle);
    // Optional. The library is unloading: drop the provider handle, don't call the API again.
    void (WINAPI* onDetach)(void* ctx);
} WSE_ProviderCallbacks;

int    WINAPI WSE_GetApiVersion(void);                                   // 1
UINT64 WINAPI WSE_RegisterProvider(PCWSTR providerJson, const WSE_ProviderCallbacks* cb);
BOOL   WINAPI WSE_SubmitResults(UINT64 queryHandle, PCWSTR itemsJson, BOOL done);
BOOL   WINAPI WSE_SetStaticItems(UINT64 provider, PCWSTR itemsJson);     // items always offered (GetItems)
BOOL   WINAPI WSE_UpdateCard(UINT64 provider, PCWSTR itemId, PCWSTR dataJson); // live-update a card's data
BOOL   WINAPI WSE_NotifyChanged(UINT64 provider);                        // ask search to re-query
BOOL   WINAPI WSE_UnregisterProvider(UINT64 provider);
```

Finding the library: enumerate the modules of the process and pick the one whose
file name starts with `search-menu-extensions_`, then `GetProcAddress`. Mods load
in any order, so retry for a few seconds if it isn't there yet.

### 2. From any process (named pipe)

Connect to `\\.\pipe\WindhawkSearchExtensions` and exchange **newline-delimited
UTF-8 JSON** messages. Each connection can register any number of providers.
They're removed automatically when the connection closes.

Client to library:

```json
{"op":"register","provider":{"id":"weather","name":"Weather","glyph":"\uE706"}}
{"op":"results","queryId":"17","items":[ ... ],"done":true}
{"op":"setItems","provider":"weather","items":[ ... ]}
{"op":"updateCard","provider":"weather","itemId":"now","data":{ ... }}
{"op":"refresh","provider":"weather"}
{"op":"unregister","provider":"weather"}
```

Library to client:

```json
{"op":"registered","provider":"weather","extensionId":"Windhawk.weather"}
{"op":"query","provider":"weather","queryId":"17","text":"weather lond","query":{...}}
{"op":"cancel","provider":"weather","queryId":"17"}
{"op":"invoke","provider":"weather","itemId":"now","actionId":"invoke","data":"","inputs":""}
{"op":"error","message":"..."}
```

## Provider JSON

```json
{
  "id": "weather",                       // unique, [A-Za-z0-9._-]
  "name": "Weather",                     // shown in search settings/diagnostics
  "description": "Current conditions",
  "glyph": "\uE706", "font": "Segoe Fluent Icons",  // or "icon": "C:\\path\\icon.png"
  "zeroState": false,                    // also asked when the box is empty
  "kinds": ["QueryResult.SystemComposer"],          // advanced: palette kinds this source serves
  "itemKinds": ["Command"]               // default kinds for items
}
```

## Item JSON

```json
{
  "id": "now",                           // stable id within the provider
  "title": "London: 14°C, light rain",
  "subtitle": "Feels like 12°C",         // string or array of strings
  "icon": {"glyph": "\uE9E2", "font": "Segoe Fluent Icons"},  // or {"uri": "https://..."} / {"path": "C:\\x.png"} / "C:\\x.png"
  "tags": [{"text": "Live", "tooltip": "Updated just now"}],
  "kinds": ["Command"],
  "score": 0.9,                          // goes to the native ranking (KnownPaletteProperties.Score)
  "properties": {"Path": "C:\\Users"},   // any extra KnownPaletteProperties
  "action": {"type": "callback"},        // default: onInvoke(id, "invoke")
  //  {"type":"uri","uri":"https://bbc.co.uk/weather"}
  //  {"type":"run","command":"notepad.exe","args":"x.txt"}
  //  {"type":"copy","text":"14°C"}
  //  {"type":"toggle","isToggled":true}   -> onInvoke(id, "toggle", "true"|"false")
  "card": {                              // optional Adaptive Card content (template + data)
    "template": { "type": "AdaptiveCard", "version": "1.5", "body": [ ... ], "actions": [ ... ] },
    "data": { ... }                      // Action.Submit/Execute -> onInvoke(id, actionId, data, inputs)
  }
}
```

## Notes

* This mod only works on builds that have the new composer search
  (`Windows.UI.Palettes`). Elsewhere it does nothing.
* Windows reads its provider list once, when the composer starts. If the mod
  is enabled while the composer is already running, it restarts the composer
  (setting **Restart search when enabled late**). The composer comes back the
  next time you open search. Providers registered later (other mods, pipe
  clients) are picked up live.
* **Card buttons:** cards render under the selected row and you can update them
  live (`WSE_UpdateCard` / `updateCard`). But Windows ignores Action.Submit
  buttons in that inline preview ("slot previews do not support card actions").
  Put the action on the item itself.
* **Pipe clients:** if you open the pipe as an ordinary synchronous handle
  (Python `open()`, .NET `FileStream` without async), don't write from one
  thread while another is blocked reading. Windows serializes I/O on
  synchronous handles and the write waits forever. Either answer from the
  reading thread or open the pipe overlapped.
* Turn on **Debug logging** and open the Windhawk log to see the queries and
  what Windows asks of the providers.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- demoProvider: true
  $name: Demo provider
  $description: Adds sample items when you type "wh" (echo, toggle, card, launcher). Handy to check the library works.
- pipeServer: true
  $name: Named pipe API
  $description: Let other processes and scripts add items through \\.\pipe\WindhawkSearchExtensions
- queryTimeoutMs: 2500
  $name: Provider timeout (ms)
  $description: How long to wait for a provider's results before showing what it has so far
- restartIfLate: true
  $name: Restart search when enabled late
  $description: If the mod is enabled while the search composer is already running, restart it (it reopens on next use) so Windows picks up the providers
- debugLog: false
  $name: Debug logging
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <inspectable.h>
#include <roapi.h>
#include <winstring.h>
#include <shellapi.h>
#include <shlwapi.h>

#include <atomic>
#include <functional>
#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Data.Json.h>


#define LOG(fmt, ...) Wh_Log(fmt, ##__VA_ARGS__)
#define DLOG(fmt, ...) do { if (g_settings.debugLog) Wh_Log(fmt, ##__VA_ARGS__); } while (0)

namespace wf = winrt::Windows::Foundation;
namespace wfc = winrt::Windows::Foundation::Collections;
namespace wdj = winrt::Windows::Data::Json;

struct Settings {
    bool demoProvider = true;
    bool pipeServer = true;
    int queryTimeoutMs = 2500;
    bool debugLog = false;
    bool restartIfLate = true;
} g_settings;

// ---- generated from Windows.UI.winmd (Windows.UI.Palettes) ----
namespace Pal {
struct IKnownPaletteKindsStatics;
struct IKnownPalettePropertiesStatics;
struct IPaletteCatalog;
struct IPaletteCatalogStatics;
struct IPaletteContent;
struct IPaletteContentFactory;
struct IPaletteContentView;
struct IPaletteContentViewFactory;
struct IPaletteDelegateInvoker;
struct IPaletteDelegateInvokerFactory;
struct IPaletteExtensionDefinition;
struct IPaletteExtensionHost;
struct IPaletteIconData;
struct IPaletteIconDataStatics;
struct IPaletteInvoker;
struct IPaletteInvokerFactory;
struct IPaletteItem;
struct IPaletteItemAnalytics;
struct IPaletteItemAnalyticsFactory;
struct IPaletteItemFactory;
struct IPaletteItemSourceExtension;
struct IPaletteItemView;
struct IPaletteKinds;
struct IPaletteKindsFactory;
struct IPaletteProgressState;
struct IPaletteProgressStateFactory;
struct IPaletteProgressStateView;
struct IPalettePropertyChangedEventArgs;
struct IPaletteQuery;
struct IPaletteQueryProgress;
struct IPaletteQueryProgressFactory;
struct IPaletteQueryResults;
struct IPaletteQueryResultsFactory;
struct IPaletteQueryResultsView;
struct IPaletteQueryStatics;
struct IPaletteStatus;
struct IPaletteStatusFactory;
struct IPaletteSystemInvoker;
struct IPaletteSystemInvokerFactory;
struct IPaletteTag;
struct IPaletteTagFactory;
struct IPaletteTemplateContent;
struct IPaletteTemplateContentFactory;
struct IPaletteTemplateContentView;
struct IPaletteToggleInvoker;
struct IPaletteToggleInvokerFactory;
struct IPaletteUriInvoker;
struct IPaletteUriInvokerFactory;
struct IVersionInfoStatics;
struct ExecutePaletteQueryDelegate;
struct IconResourceDelegate;
struct PaletteContentDelegate;
struct PaletteItemInvokeDelegate;
struct PaletteTemplateActionInvokeDelegate;
constexpr GUID IID_ExecutePaletteQueryDelegate = {0xD28C2DE0, 0x91DB, 0x55D2, {0xB3, 0x00, 0x27, 0x60, 0x4B, 0xAA, 0xC8, 0x19}};
struct ExecutePaletteQueryDelegate : IUnknown { virtual HRESULT STDMETHODCALLTYPE Invoke(Pal::IPaletteItemFactory* factory, Pal::IPaletteQuery* query, IInspectable** operation) = 0; };
constexpr GUID IID_IconResourceDelegate = {0x7BE9EF24, 0x7260, 0x5189, {0x83, 0x14, 0x1D, 0x9F, 0x3E, 0xF9, 0xC1, 0xE5}};
struct IconResourceDelegate : IUnknown { virtual HRESULT STDMETHODCALLTYPE Invoke(IInspectable* qualifierValues, IInspectable** result) = 0; };
constexpr GUID IID_PaletteContentDelegate = {0xAA523F7D, 0xE64D, 0x514F, {0x8F, 0xD7, 0x35, 0xD6, 0x5E, 0x86, 0x6E, 0xE7}};
struct PaletteContentDelegate : IUnknown { virtual HRESULT STDMETHODCALLTYPE Invoke(HSTRING itemId, IInspectable** operation) = 0; };
constexpr GUID IID_PaletteItemInvokeDelegate = {0xA2335A2E, 0xABC8, 0x5C1D, {0x8B, 0x62, 0x89, 0xC7, 0xF3, 0x9B, 0x0E, 0x0A}};
struct PaletteItemInvokeDelegate : IUnknown { virtual HRESULT STDMETHODCALLTYPE Invoke(Pal::IPaletteQuery* query) = 0; };
constexpr GUID IID_PaletteTemplateActionInvokeDelegate = {0xD3A63B88, 0xA52B, 0x59A7, {0x9E, 0x44, 0x50, 0xA5, 0x7A, 0x7F, 0x6E, 0xBF}};
struct PaletteTemplateActionInvokeDelegate : IUnknown { virtual HRESULT STDMETHODCALLTYPE Invoke(Pal::IPaletteTemplateContent* content, HSTRING actionId, HSTRING data, HSTRING inputs) = 0; };
constexpr GUID IID_IPaletteCatalog = {0x75D4E7E7, 0x9B3E, 0x577D, {0x9C, 0x34, 0x86, 0x76, 0x43, 0xFD, 0x71, 0x1A}};
struct IPaletteCatalog : IInspectable {
  virtual HRESULT STDMETHODCALLTYPE GetPalette(HSTRING extensionId, Pal::IPaletteExtensionDefinition** result) = 0;
  virtual HRESULT STDMETHODCALLTYPE GetPalettes(UINT32* __resultSize, Pal::IPaletteExtensionDefinition*** result) = 0;
  virtual HRESULT STDMETHODCALLTYPE add_Changed(IInspectable* handler, INT64* token) = 0;
  virtual HRESULT STDMETHODCALLTYPE remove_Changed(INT64 token) = 0;
};
constexpr GUID IID_IPaletteCatalogStatics = {0x084D2066, 0x3184, 0x50F3, {0xA0, 0xFF, 0x98, 0x04, 0xFF, 0x4C, 0x00, 0x02}};
struct IPaletteCatalogStatics : IInspectable {
  virtual HRESULT STDMETHODCALLTYPE Parse(HSTRING input, UINT32* __resultSize, Pal::IPaletteItemView*** result) = 0;
  virtual HRESULT STDMETHODCALLTYPE Parse2(HSTRING input, Pal::IPaletteExtensionHost* host, UINT32* __resultSize, Pal::IPaletteItemView*** result) = 0;
  virtual HRESULT STDMETHODCALLTYPE TryParse(HSTRING input, UINT32* __itemsSize, Pal::IPaletteItemView*** items, boolean* succeeded) = 0;
  virtual HRESULT STDMETHODCALLTYPE TryParse2(HSTRING input, Pal::IPaletteExtensionHost* host, UINT32* __itemsSize, Pal::IPaletteItemView*** items, boolean* succeeded) = 0;
  virtual HRESULT STDMETHODCALLTYPE OpenDefault(Pal::IPaletteCatalog** result) = 0;
  virtual HRESULT STDMETHODCALLTYPE OpenWithKinds(Pal::IPaletteKinds* kindSpecs, Pal::IPaletteCatalog** result) = 0;
  virtual HRESULT STDMETHODCALLTYPE OpenWithKindsAndConfig(Pal::IPaletteKinds* kindSpecs, HSTRING configRegistryKeyPath, Pal::IPaletteCatalog** result) = 0;
  virtual HRESULT STDMETHODCALLTYPE IsCallerIdentityAccessSystemFor(IInspectable* calledObject, boolean* result) = 0;
  virtual HRESULT STDMETHODCALLTYPE TryGetItem(HSTRING extensionId, HSTRING itemId, Pal::IPaletteItemView** result) = 0;
};
constexpr GUID IID_IPaletteContent = {0x42CD7936, 0x3FDD, 0x59CF, {0xAF, 0x35, 0x55, 0x84, 0x42, 0x89, 0x12, 0x5F}};
struct IPaletteContent : IInspectable {
  virtual HRESULT STDMETHODCALLTYPE get_Kind(INT32* value) = 0;
  virtual HRESULT STDMETHODCALLTYPE GetView(Pal::IPaletteContentView** result) = 0;
};
constexpr GUID IID_IPaletteDelegateInvokerFactory = {0xE7898783, 0xCE65, 0x5218, {0xB5, 0x94, 0x9A, 0x6E, 0x9D, 0xBC, 0x5E, 0x6F}};
struct IPaletteDelegateInvokerFactory : IInspectable {
  virtual HRESULT STDMETHODCALLTYPE CreateInstance(Pal::PaletteItemInvokeDelegate* handler, Pal::IPaletteDelegateInvoker** value) = 0;
};
constexpr GUID IID_IPaletteExtensionDefinition = {0xCA5CC86A, 0x8C7E, 0x5DB6, {0xB4, 0x6E, 0xD5, 0x88, 0xDE, 0xEE, 0x0A, 0xE7}};
struct IPaletteExtensionDefinition : IInspectable {
  virtual HRESULT STDMETHODCALLTYPE CreateExtension(Pal::IPaletteExtensionHost* host, Pal::IPaletteItemSourceExtension** result) = 0;
  virtual HRESULT STDMETHODCALLTYPE get_ItemFactory(Pal::IPaletteItemFactory** value) = 0;
  virtual HRESULT STDMETHODCALLTYPE get_ExtensionId(HSTRING* value) = 0;
  virtual HRESULT STDMETHODCALLTYPE get_PackageFamilyName(HSTRING* value) = 0;
  virtual HRESULT STDMETHODCALLTYPE get_ApplicationId(HSTRING* value) = 0;
  virtual HRESULT STDMETHODCALLTYPE get_Icon(Pal::IPaletteIconData** value) = 0;
  virtual HRESULT STDMETHODCALLTYPE get_DisplayName(HSTRING* value) = 0;
  virtual HRESULT STDMETHODCALLTYPE get_Description(HSTRING* value) = 0;
  virtual HRESULT STDMETHODCALLTYPE get_AcceptedKindSpecs(UINT32* __valueSize, HSTRING** value) = 0;
  virtual HRESULT STDMETHODCALLTYPE get_Publisher(HSTRING* value) = 0;
  virtual HRESULT STDMETHODCALLTYPE get_Properties(IInspectable** value) = 0;
  virtual HRESULT STDMETHODCALLTYPE get_Kinds(Pal::IPaletteKinds** value) = 0;
  virtual HRESULT STDMETHODCALLTYPE get_ContainedKinds(Pal::IPaletteKinds** value) = 0;
};
constexpr GUID IID_IPaletteExtensionHost = {0x3E789EFC, 0x49DD, 0x5670, {0x84, 0x58, 0x7F, 0xB5, 0x8C, 0x45, 0x72, 0x65}};
struct IPaletteExtensionHost : IInspectable {
  virtual HRESULT STDMETHODCALLTYPE ReportStatusAsync(Pal::IPaletteStatus* status, IInspectable** operation) = 0;
  virtual HRESULT STDMETHODCALLTYPE ReportStatusAsyncWithMessage(HSTRING message, INT32 severity, IInspectable** operation) = 0;
  virtual HRESULT STDMETHODCALLTYPE ReportStatusAsyncWithProgress(HSTRING message, INT32 severity, Pal::IPaletteProgressState* progress, IInspectable** operation) = 0;
  virtual HRESULT STDMETHODCALLTYPE ReportStatusAsyncWithItem(HSTRING message, INT32 severity, Pal::IPaletteProgressState* progress, Pal::IPaletteItemView* item, IInspectable** operation) = 0;
  virtual HRESULT STDMETHODCALLTYPE GetSystemInvoker(INT32 kind, Pal::IPaletteSystemInvoker** result) = 0;
  virtual HRESULT STDMETHODCALLTYPE GetSystemInvokerWithContext(INT32 kind, IInspectable* context, Pal::IPaletteSystemInvoker** result) = 0;
  virtual HRESULT STDMETHODCALLTYPE add_Dismissed(IInspectable* handler, INT64* token) = 0;
  virtual HRESULT STDMETHODCALLTYPE remove_Dismissed(INT64 token) = 0;
  virtual HRESULT STDMETHODCALLTYPE RequestUnload() = 0;
};
constexpr GUID IID_IPaletteIconData = {0xA2C917A5, 0x6B93, 0x5D6D, {0xA3, 0xBB, 0x44, 0x3B, 0x82, 0xBE, 0x5B, 0x5C}};
struct IPaletteIconData : IInspectable {
  virtual HRESULT STDMETHODCALLTYPE get_IconUri(IInspectable** value) = 0;
  virtual HRESULT STDMETHODCALLTYPE get_IconGlyph(HSTRING* value) = 0;
  virtual HRESULT STDMETHODCALLTYPE get_FontFamily(HSTRING* value) = 0;
  virtual HRESULT STDMETHODCALLTYPE get_ImageStreamReference(IInspectable** value) = 0;
  virtual HRESULT STDMETHODCALLTYPE get_Bitmap(IInspectable** value) = 0;
  virtual HRESULT STDMETHODCALLTYPE GetBitmapAsync(IInspectable* qualifierValues, IInspectable** operation) = 0;
};
constexpr GUID IID_IPaletteIconDataStatics = {0xBDE0245B, 0x9389, 0x565C, {0x8B, 0xF2, 0x09, 0x49, 0xA7, 0xBE, 0x7C, 0xD4}};
struct IPaletteIconDataStatics : IInspectable {
  virtual HRESULT STDMETHODCALLTYPE FromResourceQualifier(Pal::IconResourceDelegate* handler, Pal::IPaletteIconData** result) = 0;
  virtual HRESULT STDMETHODCALLTYPE FromBitmap(IInspectable* bitmap, Pal::IPaletteIconData** result) = 0;
  virtual HRESULT STDMETHODCALLTYPE FromUri(IInspectable* iconUri, Pal::IPaletteIconData** result) = 0;
  virtual HRESULT STDMETHODCALLTYPE FromGlyph(HSTRING iconGlyph, Pal::IPaletteIconData** result) = 0;
  virtual HRESULT STDMETHODCALLTYPE FromGlyph2(HSTRING iconGlyph, HSTRING fontFamily, Pal::IPaletteIconData** result) = 0;
  virtual HRESULT STDMETHODCALLTYPE FromStreamReference(IInspectable* streamReference, Pal::IPaletteIconData** result) = 0;
};
constexpr GUID IID_IPaletteInvoker = {0x4DCE3641, 0xD67A, 0x5402, {0x82, 0xE6, 0xF7, 0x7B, 0xE4, 0x62, 0xC9, 0x42}};
struct IPaletteInvoker : IInspectable {
  virtual HRESULT STDMETHODCALLTYPE Invoke(Pal::IPaletteQuery* query) = 0;
};
constexpr GUID IID_IPaletteItem = {0x095B1F54, 0x5206, 0x5F2B, {0xAC, 0xC5, 0xAE, 0x72, 0xBE, 0x80, 0x1E, 0x11}};
struct IPaletteItem : IInspectable {
  virtual HRESULT STDMETHODCALLTYPE get_Id(HSTRING* value) = 0;
  virtual HRESULT STDMETHODCALLTYPE get_ExtensionId(HSTRING* value) = 0;
  virtual HRESULT STDMETHODCALLTYPE get_Icon(Pal::IPaletteIconData** value) = 0;
  virtual HRESULT STDMETHODCALLTYPE put_Icon(Pal::IPaletteIconData* value) = 0;
  virtual HRESULT STDMETHODCALLTYPE get_IconOverlay(Pal::IPaletteIconData** value) = 0;
  virtual HRESULT STDMETHODCALLTYPE put_IconOverlay(Pal::IPaletteIconData* value) = 0;
  virtual HRESULT STDMETHODCALLTYPE get_Title(HSTRING* value) = 0;
  virtual HRESULT STDMETHODCALLTYPE put_Title(HSTRING value) = 0;
  virtual HRESULT STDMETHODCALLTYPE get_Subtitle(UINT32* __valueSize, IInspectable*** value) = 0;
  virtual HRESULT STDMETHODCALLTYPE put_Subtitle(UINT32 __valueSize, IInspectable** value) = 0;
  virtual HRESULT STDMETHODCALLTYPE get_Tags(UINT32* __valueSize, Pal::IPaletteTag*** value) = 0;
  virtual HRESULT STDMETHODCALLTYPE put_Tags(UINT32 __valueSize, Pal::IPaletteTag** value) = 0;
  virtual HRESULT STDMETHODCALLTYPE get_Properties(IInspectable** value) = 0;
  virtual HRESULT STDMETHODCALLTYPE GetView(Pal::IPaletteItemView** result) = 0;
  virtual HRESULT STDMETHODCALLTYPE get_Status(Pal::IPaletteStatus** value) = 0;
  virtual HRESULT STDMETHODCALLTYPE put_Status(Pal::IPaletteStatus* value) = 0;
  virtual HRESULT STDMETHODCALLTYPE get_Kinds(Pal::IPaletteKinds** value) = 0;
  virtual HRESULT STDMETHODCALLTYPE get_Invoker(Pal::IPaletteInvoker** value) = 0;
  virtual HRESULT STDMETHODCALLTYPE put_Invoker(Pal::IPaletteInvoker* value) = 0;
  virtual HRESULT STDMETHODCALLTYPE get_ExecuteQueryDelegate(Pal::ExecutePaletteQueryDelegate** value) = 0;
  virtual HRESULT STDMETHODCALLTYPE put_ExecuteQueryDelegate(Pal::ExecutePaletteQueryDelegate* value) = 0;
  virtual HRESULT STDMETHODCALLTYPE get_ContentDelegate(Pal::PaletteContentDelegate** value) = 0;
  virtual HRESULT STDMETHODCALLTYPE put_ContentDelegate(Pal::PaletteContentDelegate* value) = 0;
  virtual HRESULT STDMETHODCALLTYPE add_PropertyChanged(IInspectable* handler, INT64* token) = 0;
  virtual HRESULT STDMETHODCALLTYPE remove_PropertyChanged(INT64 token) = 0;
};
constexpr GUID IID_IPaletteItemFactory = {0x72784ABE, 0xDA8A, 0x5F48, {0x89, 0x5C, 0xD8, 0xE3, 0x1D, 0x03, 0xAE, 0x40}};
struct IPaletteItemFactory : IInspectable {
  virtual HRESULT STDMETHODCALLTYPE get_ExtensionId(HSTRING* value) = 0;
  virtual HRESULT STDMETHODCALLTYPE Create(HSTRING id, Pal::IPaletteItem** result) = 0;
  virtual HRESULT STDMETHODCALLTYPE Create2(HSTRING id, Pal::IPaletteKinds* kinds, Pal::IPaletteItem** result) = 0;
};
constexpr GUID IID_IPaletteItemSourceExtension = {0x2F89BC97, 0x3238, 0x5E81, {0x8E, 0x02, 0x5D, 0xAA, 0x9C, 0x56, 0x3F, 0x81}};
struct IPaletteItemSourceExtension : IInspectable {
  virtual HRESULT STDMETHODCALLTYPE Initialize(Pal::IPaletteItemFactory* factory, Pal::IPaletteExtensionHost* host) = 0;
  virtual HRESULT STDMETHODCALLTYPE GetItem(HSTRING id, Pal::IPaletteItemView** result) = 0;
  virtual HRESULT STDMETHODCALLTYPE GetItems(UINT32* __resultSize, Pal::IPaletteItemView*** result) = 0;
  virtual HRESULT STDMETHODCALLTYPE ExecuteQueryAsync(Pal::IPaletteQuery* query, IInspectable** operation) = 0;
  virtual HRESULT STDMETHODCALLTYPE ReportItemAnalyticsAsync(UINT32 __analyticsSize, Pal::IPaletteItemAnalytics** analytics, IInspectable** operation) = 0;
  virtual HRESULT STDMETHODCALLTYPE ReportConversationAnalyticsAsync(UINT32 __itemAnalyticsSize, Pal::IPaletteItemAnalytics** itemAnalytics, IInspectable** operation) = 0;
  virtual HRESULT STDMETHODCALLTYPE add_ItemsChanged(IInspectable* handler, INT64* token) = 0;
  virtual HRESULT STDMETHODCALLTYPE remove_ItemsChanged(INT64 token) = 0;
};
constexpr GUID IID_IPaletteItemView = {0x49D58C99, 0x5BBF, 0x5745, {0xA6, 0x93, 0x67, 0xC1, 0x67, 0x15, 0xF0, 0xC0}};
struct IPaletteItemView : IInspectable {
  virtual HRESULT STDMETHODCALLTYPE get_Id(HSTRING* value) = 0;
  virtual HRESULT STDMETHODCALLTYPE get_ExtensionId(HSTRING* value) = 0;
  virtual HRESULT STDMETHODCALLTYPE get_Icon(Pal::IPaletteIconData** value) = 0;
  virtual HRESULT STDMETHODCALLTYPE get_IconOverlay(Pal::IPaletteIconData** value) = 0;
  virtual HRESULT STDMETHODCALLTYPE get_Title(HSTRING* value) = 0;
  virtual HRESULT STDMETHODCALLTYPE get_Subtitle(UINT32* __valueSize, IInspectable*** value) = 0;
  virtual HRESULT STDMETHODCALLTYPE get_Tags(UINT32* __valueSize, Pal::IPaletteTag*** value) = 0;
  virtual HRESULT STDMETHODCALLTYPE get_Properties(IInspectable** value) = 0;
  virtual HRESULT STDMETHODCALLTYPE ExecuteQueryAsync(Pal::IPaletteQuery* query, IInspectable** operation) = 0;
  virtual HRESULT STDMETHODCALLTYPE get_Status(Pal::IPaletteStatus** value) = 0;
  virtual HRESULT STDMETHODCALLTYPE get_IsContentAvailable(boolean* value) = 0;
  virtual HRESULT STDMETHODCALLTYPE GetContentAsync(IInspectable** operation) = 0;
  virtual HRESULT STDMETHODCALLTYPE get_Kinds(Pal::IPaletteKinds** value) = 0;
  virtual HRESULT STDMETHODCALLTYPE add_ContentChanged(IInspectable* handler, INT64* token) = 0;
  virtual HRESULT STDMETHODCALLTYPE remove_ContentChanged(INT64 token) = 0;
  virtual HRESULT STDMETHODCALLTYPE TryGetInvoker(Pal::IPaletteInvoker** result) = 0;
  virtual HRESULT STDMETHODCALLTYPE add_PropertyChanged(IInspectable* handler, INT64* token) = 0;
  virtual HRESULT STDMETHODCALLTYPE remove_PropertyChanged(INT64 token) = 0;
};
constexpr GUID IID_IPaletteKinds = {0x666CB275, 0x1764, 0x5016, {0xA7, 0x30, 0x1B, 0xF7, 0xE9, 0xE1, 0xD1, 0xBC}};
struct IPaletteKinds : IInspectable {
  virtual HRESULT STDMETHODCALLTYPE get_Values(UINT32* __valueSize, HSTRING** value) = 0;
  virtual HRESULT STDMETHODCALLTYPE GetPrimaryKind(HSTRING* result) = 0;
  virtual HRESULT STDMETHODCALLTYPE IsOfKind(HSTRING kindSpec, boolean* result) = 0;
  virtual HRESULT STDMETHODCALLTYPE IsOfKinds(UINT32 __kindSpecsSize, HSTRING* kindSpecs, INT32 selector, boolean* result) = 0;
};
constexpr GUID IID_IPaletteKindsFactory = {0xBE554067, 0x0C61, 0x5273, {0xB6, 0x6E, 0xDC, 0x14, 0x2F, 0x6E, 0x01, 0xA9}};
struct IPaletteKindsFactory : IInspectable {
  virtual HRESULT STDMETHODCALLTYPE CreateInstance(UINT32 __valuesSize, HSTRING* values, Pal::IPaletteKinds** value) = 0;
};
constexpr GUID IID_IPaletteProgressStateFactory = {0xDBC1B51F, 0x0FD4, 0x5C73, {0x9F, 0x3E, 0xD1, 0xDE, 0x7E, 0x18, 0x5B, 0x72}};
struct IPaletteProgressStateFactory : IInspectable {
  virtual HRESULT STDMETHODCALLTYPE CreateInstance(boolean isIndeterminate, FLOAT progress, Pal::IPaletteProgressState** value) = 0;
};
constexpr GUID IID_IPaletteQuery = {0xA3D9EE47, 0x0151, 0x5A33, {0x96, 0x4A, 0xD2, 0xD4, 0xC9, 0x1E, 0x97, 0xEA}};
struct IPaletteQuery : IInspectable {
  virtual HRESULT STDMETHODCALLTYPE ToJson(HSTRING* result) = 0;
  virtual HRESULT STDMETHODCALLTYPE get_QueryId(GUID* value) = 0;
  virtual HRESULT STDMETHODCALLTYPE get_Arguments(UINT32* __valueSize, IInspectable*** value) = 0;
  virtual HRESULT STDMETHODCALLTYPE put_Arguments(UINT32 __valueSize, IInspectable** value) = 0;
  virtual HRESULT STDMETHODCALLTYPE get_DisplayLanguageTag(HSTRING* value) = 0;
  virtual HRESULT STDMETHODCALLTYPE put_DisplayLanguageTag(HSTRING value) = 0;
  virtual HRESULT STDMETHODCALLTYPE get_SystemLanguageTag(HSTRING* value) = 0;
  virtual HRESULT STDMETHODCALLTYPE put_SystemLanguageTag(HSTRING value) = 0;
  virtual HRESULT STDMETHODCALLTYPE get_ItemCount(INT32* value) = 0;
  virtual HRESULT STDMETHODCALLTYPE put_ItemCount(INT32 value) = 0;
  virtual HRESULT STDMETHODCALLTYPE get_Scopes(UINT32* __valueSize, Pal::IPaletteItemView*** value) = 0;
  virtual HRESULT STDMETHODCALLTYPE put_Scopes(UINT32 __valueSize, Pal::IPaletteItemView** value) = 0;
  virtual HRESULT STDMETHODCALLTYPE get_RequestedProperties(UINT32* __valueSize, HSTRING** value) = 0;
  virtual HRESULT STDMETHODCALLTYPE put_RequestedProperties(UINT32 __valueSize, HSTRING* value) = 0;
  virtual HRESULT STDMETHODCALLTYPE get_Context(IInspectable** value) = 0;
  virtual HRESULT STDMETHODCALLTYPE get_Mode(INT32* value) = 0;
  virtual HRESULT STDMETHODCALLTYPE put_Mode(INT32 value) = 0;
};
constexpr GUID IID_IPaletteQueryProgressFactory = {0x16412CF9, 0x3F5E, 0x5E6C, {0x85, 0xCD, 0x8F, 0xE5, 0x7F, 0x1B, 0xC5, 0x38}};
struct IPaletteQueryProgressFactory : IInspectable {
  virtual HRESULT STDMETHODCALLTYPE CreateInstance(UINT32 __itemsSize, Pal::IPaletteItemView** items, Pal::IPaletteProgressState* progressState, Pal::IPaletteQueryProgress** value) = 0;
};
constexpr GUID IID_IPaletteQueryResults = {0x587B3BED, 0x2C37, 0x53AF, {0xB4, 0x79, 0xC2, 0x98, 0x6B, 0xCA, 0x59, 0xDC}};
struct IPaletteQueryResults : IInspectable {
  virtual HRESULT STDMETHODCALLTYPE get_QueryId(GUID* value) = 0;
  virtual HRESULT STDMETHODCALLTYPE get_Items(UINT32* __valueSize, Pal::IPaletteItemView*** value) = 0;
  virtual HRESULT STDMETHODCALLTYPE put_Items(UINT32 __valueSize, Pal::IPaletteItemView** value) = 0;
  virtual HRESULT STDMETHODCALLTYPE get_HasMoreItems(boolean* value) = 0;
  virtual HRESULT STDMETHODCALLTYPE put_HasMoreItems(boolean value) = 0;
  virtual HRESULT STDMETHODCALLTYPE GetView(Pal::IPaletteQueryResultsView** result) = 0;
};
constexpr GUID IID_IPaletteQueryResultsFactory = {0x3C262865, 0xA9E4, 0x5C17, {0xB3, 0x03, 0xEC, 0xC3, 0x10, 0xF5, 0xEB, 0x71}};
struct IPaletteQueryResultsFactory : IInspectable {
  virtual HRESULT STDMETHODCALLTYPE CreateInstance(Pal::IPaletteQuery* query, Pal::IPaletteQueryResults** value) = 0;
  virtual HRESULT STDMETHODCALLTYPE CreateInstance2(Pal::IPaletteQuery* query, Pal::ExecutePaletteQueryDelegate* getNextDelegate, Pal::IPaletteQueryResults** value) = 0;
};
constexpr GUID IID_IPaletteQueryResultsView = {0xE11F2FF3, 0x5F5B, 0x503F, {0x84, 0x4F, 0xFD, 0x2B, 0xB9, 0xF0, 0x63, 0x3D}};
struct IPaletteQueryResultsView : IInspectable {
  virtual HRESULT STDMETHODCALLTYPE get_QueryId(GUID* value) = 0;
  virtual HRESULT STDMETHODCALLTYPE get_Items(UINT32* __valueSize, Pal::IPaletteItemView*** value) = 0;
  virtual HRESULT STDMETHODCALLTYPE get_HasMoreItems(boolean* value) = 0;
  virtual HRESULT STDMETHODCALLTYPE GetMoreItemsAsync(IInspectable** operation) = 0;
};
constexpr GUID IID_IPaletteTag = {0xEF42C037, 0xBC6B, 0x5802, {0xA6, 0xAE, 0xD4, 0xB4, 0x33, 0x2A, 0xBD, 0x32}};
struct IPaletteTag : IInspectable {
  virtual HRESULT STDMETHODCALLTYPE get_Icon(Pal::IPaletteIconData** value) = 0;
  virtual HRESULT STDMETHODCALLTYPE get_Text(HSTRING* value) = 0;
  virtual HRESULT STDMETHODCALLTYPE get_ToolTip(HSTRING* value) = 0;
};
constexpr GUID IID_IPaletteTagFactory = {0x64702731, 0x160E, 0x580F, {0xAD, 0xD9, 0xC6, 0xA8, 0xB7, 0x78, 0x86, 0xC3}};
struct IPaletteTagFactory : IInspectable {
  virtual HRESULT STDMETHODCALLTYPE CreateInstance(Pal::IPaletteIconData* icon, HSTRING text, HSTRING tooltip, Pal::IPaletteTag** value) = 0;
};
constexpr GUID IID_IPaletteTemplateContent = {0xA2561B14, 0x488B, 0x5B78, {0xAC, 0x4B, 0x00, 0x13, 0x9C, 0xCE, 0x94, 0x99}};
struct IPaletteTemplateContent : IInspectable {
  virtual HRESULT STDMETHODCALLTYPE get_TemplateJson(HSTRING* value) = 0;
  virtual HRESULT STDMETHODCALLTYPE put_TemplateJson(HSTRING value) = 0;
  virtual HRESULT STDMETHODCALLTYPE get_DataJson(HSTRING* value) = 0;
  virtual HRESULT STDMETHODCALLTYPE put_DataJson(HSTRING value) = 0;
  virtual HRESULT STDMETHODCALLTYPE add_PropertyChanged(IInspectable* handler, INT64* token) = 0;
  virtual HRESULT STDMETHODCALLTYPE remove_PropertyChanged(INT64 token) = 0;
};
constexpr GUID IID_IPaletteTemplateContentFactory = {0x1AFD3A97, 0x0151, 0x5E70, {0xA9, 0x6A, 0xFF, 0xD7, 0x14, 0xE9, 0xD8, 0x2D}};
struct IPaletteTemplateContentFactory : IInspectable {
  virtual HRESULT STDMETHODCALLTYPE CreateInstance(Pal::PaletteTemplateActionInvokeDelegate* invokeAction, Pal::IPaletteTemplateContent** value) = 0;
};
constexpr GUID IID_IPaletteToggleInvokerFactory = {0x48CFA05A, 0x6C07, 0x5DEE, {0xA3, 0x63, 0x1F, 0x5A, 0x48, 0xBB, 0x5A, 0x8B}};
struct IPaletteToggleInvokerFactory : IInspectable {
  virtual HRESULT STDMETHODCALLTYPE CreateInstance(Pal::IPaletteInvoker* invoker, boolean isToggled, Pal::IPaletteToggleInvoker** value) = 0;
};
constexpr GUID IID_IPaletteUriInvokerFactory = {0x8F2D6F1B, 0x4B0D, 0x5D77, {0xA4, 0x65, 0x6F, 0xD9, 0x67, 0x31, 0x70, 0x9D}};
struct IPaletteUriInvokerFactory : IInspectable {
  virtual HRESULT STDMETHODCALLTYPE CreateInstance(IInspectable* uri, Pal::IPaletteUriInvoker** value) = 0;
  virtual HRESULT STDMETHODCALLTYPE CreateInstance2(IInspectable* uri, IInspectable* launcherOptions, Pal::IPaletteUriInvoker** value) = 0;
};
} // namespace Pal


// ---------------------------------------------------------------------------
// Generic WinRT ABI pieces we implement by hand.
// ---------------------------------------------------------------------------
constexpr GUID kIID_IInspectable = {0xAF86E2E0, 0xB12D, 0x4c6a, {0x9C, 0x5A, 0xD7, 0xAA, 0x65, 0x10, 0x1E, 0x90}};
constexpr GUID IID_IAgileObject_ = {0x94ea2b94, 0xe9cc, 0x49e0, {0xc0, 0xff, 0xee, 0x64, 0xca, 0x8f, 0x5b, 0x90}};
constexpr GUID IID_IAsyncInfo_ = {0x00000036, 0x0000, 0x0000, {0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46}};
constexpr GUID IID_IAsyncAction_ = {0x5a648006, 0x843a, 0x4da9, {0x86, 0x5b, 0x9d, 0x26, 0xe5, 0xdf, 0xad, 0x7b}};
// IAsyncOperationWithProgress<PaletteQueryResultsView, PaletteQueryProgress>
constexpr GUID IID_AsyncOpResults = {0xcb62b7ba, 0x8e45, 0x55c1, {0xbd, 0x2e, 0x81, 0xb7, 0xf5, 0x13, 0xd1, 0x35}};
// IAsyncOperation<PaletteContent>
constexpr GUID IID_AsyncOpContent = {0xcb73ad0d, 0x69fd, 0x5ae3, {0x98, 0x8c, 0xad, 0x2b, 0xf6, 0x8a, 0x16, 0x60}};
constexpr GUID IID_IMapStringObject = {0x1b0d3570, 0x0877, 0x5ec2, {0x8a, 0x2c, 0x3b, 0x95, 0x39, 0x50, 0x6a, 0xca}};
constexpr GUID IID_IPaletteCatalogPrivate = {0xdcaa0cae, 0x03e9, 0x40b2, {0x94, 0x96, 0x67, 0x6c, 0xd0, 0x56, 0x82, 0x78}};
constexpr GUID IID_IValueUnmarshalByPropertySet = {0xecabe149, 0x4833, 0x452b, {0x81, 0x12, 0x3e, 0xee, 0xc6, 0xd2, 0x0c, 0xb0}};

struct IValueUnmarshalByPropertySet : ::IInspectable {
    virtual HRESULT STDMETHODCALLTYPE UnmarshalObjectFromPropertySet(void* propertySet, void** result) = 0;
};

#ifndef E_ILLEGAL_DELEGATE_ASSIGNMENT
#define E_ILLEGAL_DELEGATE_ASSIGNMENT ((HRESULT)0x80000018L)
#endif
#ifndef E_ILLEGAL_METHOD_CALL
#define E_ILLEGAL_METHOD_CALL ((HRESULT)0x8000000EL)
#endif
enum AsyncStatus_ : INT32 { AS_Started = 0, AS_Completed = 1, AS_Canceled = 2, AS_Error = 3 };

struct IAsyncInfoRaw : ::IInspectable {
    virtual HRESULT STDMETHODCALLTYPE get_Id(UINT32* id) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Status(INT32* status) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_ErrorCode(HRESULT* code) = 0;
    virtual HRESULT STDMETHODCALLTYPE Cancel() = 0;
    virtual HRESULT STDMETHODCALLTYPE Close() = 0;
};
struct IAsyncOpWithProgressRaw : ::IInspectable {
    virtual HRESULT STDMETHODCALLTYPE put_Progress(::IUnknown* handler) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Progress(::IUnknown** handler) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_Completed(::IUnknown* handler) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Completed(::IUnknown** handler) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetResults(::IUnknown** results) = 0;
};
struct IAsyncOpRaw : ::IInspectable {
    virtual HRESULT STDMETHODCALLTYPE put_Completed(::IUnknown* handler) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Completed(::IUnknown** handler) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetResults(::IUnknown** results) = 0;
};
struct IAsyncActionRaw : ::IInspectable {
    virtual HRESULT STDMETHODCALLTYPE put_Completed(::IUnknown* handler) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Completed(::IUnknown** handler) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetResults() = 0;
};
// Every completed/progress handler and TypedEventHandler has Invoke(a, b) at slot 3.
struct IHandler2Ptr : ::IUnknown {
    virtual HRESULT STDMETHODCALLTYPE Invoke(void* a, void* b) = 0;
};
struct IHandler2Int : ::IUnknown {
    virtual HRESULT STDMETHODCALLTYPE Invoke(void* a, INT32 b) = 0;
};

struct IPaletteCatalogPrivate : ::IInspectable {
    virtual void STDMETHODCALLTYPE SetEnumerateDisabledSources(BOOL v) = 0;
    virtual void STDMETHODCALLTYPE GetOrderedSourceDefinitions(HSTRING kind, UINT32* n, void*** defs) = 0;
    virtual BOOL STDMETHODCALLTYPE IsEnabled(HSTRING id) = 0;
    virtual BOOL STDMETHODCALLTYPE IsUserDisabled(HSTRING id) = 0;
    virtual void STDMETHODCALLTYPE SetItemSourceEnabled(HSTRING id, BOOL enabled) = 0;
};

#define IMPL_REFCOUNT                                                         \
    std::atomic<ULONG> m_ref{1};                                              \
    ULONG STDMETHODCALLTYPE AddRef() override { return ++m_ref; }             \
    ULONG STDMETHODCALLTYPE Release() override {                              \
        ULONG r = --m_ref;                                                    \
        if (!r) delete this;                                                  \
        return r;                                                             \
    }
#define IMPL_INSPECTABLE(cls)                                                 \
    HRESULT STDMETHODCALLTYPE GetIids(ULONG* c, IID** i) override {           \
        *c = 0;                                                               \
        *i = nullptr;                                                         \
        return S_OK;                                                          \
    }                                                                         \
    HRESULT STDMETHODCALLTYPE GetRuntimeClassName(HSTRING* n) override {      \
        return WindowsCreateString(cls, (UINT32)wcslen(cls), n);              \
    }                                                                         \
    HRESULT STDMETHODCALLTYPE GetTrustLevel(TrustLevel* t) override {         \
        *t = BaseTrust;                                                       \
        return S_OK;                                                          \
    }

static inline HSTRING H(winrt::hstring const& s) { return (HSTRING)winrt::get_abi(s); }
static inline winrt::hstring FromH(HSTRING h) {
    UINT32 n;
    const wchar_t* p = WindowsGetStringRawBuffer(h, &n);
    return winrt::hstring(p, n);
}
static HRESULT DupH(winrt::hstring const& s, HSTRING* out) {
    return WindowsDuplicateString(H(s), out);
}

static std::atomic<bool> g_alive{false};

static void PinModule() {
    static std::once_flag once;
    std::call_once(once, [] {
        HMODULE m;
        GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_PIN,
                           (LPCWSTR)&PinModule, &m);
    });
}

// Activation factories are agile; cache them.
template <class T>
static winrt::com_ptr<T> Factory(const wchar_t* cls, const GUID& iid) {
    winrt::com_ptr<T> out;
    try {
        auto f = winrt::get_activation_factory<wf::IInspectable>(cls);
        ((::IUnknown*)winrt::get_abi(f))->QueryInterface(iid, out.put_void());
    } catch (...) {
    }
    if (!out) LOG(L"activation factory %s unavailable", cls);
    return out;
}
#define CACHED_FACTORY(T, cls)                                                \
    ([]() -> T* {                                                             \
        static winrt::com_ptr<T> f = Factory<T>(cls, IID_##T);                \
        return f.get();                                                       \
    }())
#define IID_IPaletteKindsFactory Pal::IID_IPaletteKindsFactory
#define IID_IPaletteIconDataStatics Pal::IID_IPaletteIconDataStatics
#define IID_IPaletteTagFactory Pal::IID_IPaletteTagFactory
#define IID_IPaletteQueryResultsFactory Pal::IID_IPaletteQueryResultsFactory
#define IID_IPaletteDelegateInvokerFactory Pal::IID_IPaletteDelegateInvokerFactory
#define IID_IPaletteToggleInvokerFactory Pal::IID_IPaletteToggleInvokerFactory
#define IID_IPaletteUriInvokerFactory Pal::IID_IPaletteUriInvokerFactory
#define IID_IPaletteTemplateContentFactory Pal::IID_IPaletteTemplateContentFactory
#define IID_IPaletteQueryProgressFactory Pal::IID_IPaletteQueryProgressFactory
using Pal::IPaletteKindsFactory;
using Pal::IPaletteIconDataStatics;
using Pal::IPaletteTagFactory;
using Pal::IPaletteQueryResultsFactory;
using Pal::IPaletteDelegateInvokerFactory;
using Pal::IPaletteToggleInvokerFactory;
using Pal::IPaletteUriInvokerFactory;
using Pal::IPaletteTemplateContentFactory;
using Pal::IPaletteQueryProgressFactory;

static winrt::com_ptr<Pal::IPaletteKinds> MakeKinds(std::vector<std::wstring> const& v) {
    winrt::com_ptr<Pal::IPaletteKinds> k;
    auto* f = CACHED_FACTORY(IPaletteKindsFactory, L"Windows.UI.Palettes.PaletteKinds");
    if (!f) return k;
    std::vector<winrt::hstring> hs(v.begin(), v.end());
    std::vector<HSTRING> raw;
    for (auto& h : hs) raw.push_back(H(h));
    f->CreateInstance((UINT32)raw.size(), raw.data(), k.put());
    return k;
}

static bool KindMatches(std::wstring const& a, std::wstring const& b) {
    if (!_wcsicmp(a.c_str(), b.c_str())) return true;
    auto pref = [](std::wstring const& w, std::wstring const& x) {
        if (w.size() < 2 || w.compare(w.size() - 2, 2, L".*")) return false;
        std::wstring p = w.substr(0, w.size() - 1);  // keep the dot
        return x.size() >= p.size() && !_wcsnicmp(x.c_str(), p.c_str(), p.size());
    };
    return pref(a, b) || pref(b, a) || a == L"*" || b == L"*";
}

// ---------------------------------------------------------------------------
// Async operations
// ---------------------------------------------------------------------------
struct AsyncBase : IAsyncInfoRaw {
    std::mutex m;
    INT32 status = AS_Started;
    HRESULT error = S_OK;
    winrt::com_ptr<::IUnknown> completed;
    winrt::com_ptr<::IUnknown> result;
    UINT32 id;
    AsyncBase() {
        static std::atomic<UINT32> next{1};
        id = next++;
    }
    virtual ~AsyncBase() = default;
    virtual void* OpPtr() = 0;  // interface pointer passed to handlers

    HRESULT STDMETHODCALLTYPE get_Id(UINT32* v) override { *v = id; return S_OK; }
    HRESULT STDMETHODCALLTYPE get_Status(INT32* v) override { std::lock_guard l(m); *v = status; return S_OK; }
    HRESULT STDMETHODCALLTYPE get_ErrorCode(HRESULT* v) override { std::lock_guard l(m); *v = error; return S_OK; }
    HRESULT STDMETHODCALLTYPE Cancel() override {
        OnCancel();
        Finish(AS_Canceled, nullptr, HRESULT_FROM_WIN32(ERROR_CANCELLED));
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE Close() override { return S_OK; }
    virtual void OnCancel() {}

    HRESULT SetCompleted(::IUnknown* h) {
        INT32 st;
        {
            std::lock_guard l(m);
            if (completed) return E_ILLEGAL_DELEGATE_ASSIGNMENT;
            if (!h) return S_OK;
            st = status;
            if (st == AS_Started) {
                completed.copy_from(h);
                return S_OK;
            }
        }
        ((IHandler2Int*)h)->Invoke(OpPtr(), st);
        return S_OK;
    }
    HRESULT GetCompleted(::IUnknown** h) {
        std::lock_guard l(m);
        completed.copy_to(h);
        return S_OK;
    }
    HRESULT Results(::IUnknown** r) {
        std::lock_guard l(m);
        if (status == AS_Completed) {
            result.copy_to(r);
            return S_OK;
        }
        if (status == AS_Error) return error;
        if (status == AS_Canceled) return HRESULT_FROM_WIN32(ERROR_CANCELLED);
        return E_ILLEGAL_METHOD_CALL;
    }
    // Returns false if already finished.
    bool Finish(INT32 st, ::IUnknown* res, HRESULT err = S_OK) {
        winrt::com_ptr<::IUnknown> h;
        {
            std::lock_guard l(m);
            if (status != AS_Started) return false;
            status = st;
            error = err;
            result.copy_from(res);
            h = std::move(completed);
        }
        if (h) ((IHandler2Int*)h.get())->Invoke(OpPtr(), st);
        return true;
    }
};

struct ResultsOp final : IAsyncOpWithProgressRaw, AsyncBase {
    IMPL_REFCOUNT
    IMPL_INSPECTABLE(L"Windows.Foundation.IAsyncOperationWithProgress`2<Windows.UI.Palettes.PaletteQueryResultsView, Windows.UI.Palettes.PaletteQueryProgress>")
    std::function<void()> onCancel;
    winrt::com_ptr<::IUnknown> progress;

    void* OpPtr() override { return static_cast<IAsyncOpWithProgressRaw*>(this); }
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppv) override {
        if (riid == IID_IUnknown || riid == kIID_IInspectable || riid == IID_AsyncOpResults || riid == IID_IAgileObject_)
            *ppv = static_cast<IAsyncOpWithProgressRaw*>(this);
        else if (riid == IID_IAsyncInfo_)
            *ppv = static_cast<IAsyncInfoRaw*>(this);
        else {
            *ppv = nullptr;
            return E_NOINTERFACE;
        }
        AddRef();
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE put_Progress(::IUnknown* h) override { std::lock_guard l(m); progress.copy_from(h); return S_OK; }
    HRESULT STDMETHODCALLTYPE get_Progress(::IUnknown** h) override { std::lock_guard l(m); progress.copy_to(h); return S_OK; }
    HRESULT STDMETHODCALLTYPE put_Completed(::IUnknown* h) override { return SetCompleted(h); }
    HRESULT STDMETHODCALLTYPE get_Completed(::IUnknown** h) override { return GetCompleted(h); }
    HRESULT STDMETHODCALLTYPE GetResults(::IUnknown** r) override { return Results(r); }
    void OnCancel() override { if (onCancel) onCancel(); }
    void ReportProgress(::IUnknown* p) {
        winrt::com_ptr<::IUnknown> h;
        {
            std::lock_guard l(m);
            if (status != AS_Started) return;
            h = progress;
        }
        if (h) ((IHandler2Ptr*)h.get())->Invoke(OpPtr(), p);
    }
};

struct ContentOp final : IAsyncOpRaw, AsyncBase {
    IMPL_REFCOUNT
    IMPL_INSPECTABLE(L"Windows.Foundation.IAsyncOperation`1<Windows.UI.Palettes.PaletteContent>")
    void* OpPtr() override { return static_cast<IAsyncOpRaw*>(this); }
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppv) override {
        if (riid == IID_IUnknown || riid == kIID_IInspectable || riid == IID_AsyncOpContent || riid == IID_IAgileObject_)
            *ppv = static_cast<IAsyncOpRaw*>(this);
        else if (riid == IID_IAsyncInfo_)
            *ppv = static_cast<IAsyncInfoRaw*>(this);
        else {
            *ppv = nullptr;
            return E_NOINTERFACE;
        }
        AddRef();
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE put_Completed(::IUnknown* h) override { return SetCompleted(h); }
    HRESULT STDMETHODCALLTYPE get_Completed(::IUnknown** h) override { return GetCompleted(h); }
    HRESULT STDMETHODCALLTYPE GetResults(::IUnknown** r) override { return Results(r); }
};

struct DoneAction final : IAsyncActionRaw, AsyncBase {
    IMPL_REFCOUNT
    IMPL_INSPECTABLE(L"Windows.Foundation.IAsyncAction")
    DoneAction() { status = AS_Completed; }
    void* OpPtr() override { return static_cast<IAsyncActionRaw*>(this); }
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppv) override {
        if (riid == IID_IUnknown || riid == kIID_IInspectable || riid == IID_IAsyncAction_ || riid == IID_IAgileObject_)
            *ppv = static_cast<IAsyncActionRaw*>(this);
        else if (riid == IID_IAsyncInfo_)
            *ppv = static_cast<IAsyncInfoRaw*>(this);
        else {
            *ppv = nullptr;
            return E_NOINTERFACE;
        }
        AddRef();
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE put_Completed(::IUnknown* h) override { return SetCompleted(h); }
    HRESULT STDMETHODCALLTYPE get_Completed(::IUnknown** h) override { return GetCompleted(h); }
    HRESULT STDMETHODCALLTYPE GetResults() override { return S_OK; }
};

// Generic delegate implementation: IUnknown + Invoke with a fixed IID.
template <class Iface>
struct Delegate final : Iface {
    IMPL_REFCOUNT
    GUID iid;
    std::function<HRESULT(Iface*)> dummy;
    Delegate(GUID const& i) : iid(i) {}
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppv) override {
        if (riid == IID_IUnknown || riid == iid || riid == IID_IAgileObject_) {
            *ppv = static_cast<Iface*>(this);
            AddRef();
            return S_OK;
        }
        *ppv = nullptr;
        return E_NOINTERFACE;
    }
};

struct InvokeDelegate final : Pal::PaletteItemInvokeDelegate {
    IMPL_REFCOUNT
    std::function<void(Pal::IPaletteQuery*)> fn;
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppv) override {
        if (riid == IID_IUnknown || riid == Pal::IID_PaletteItemInvokeDelegate || riid == IID_IAgileObject_) {
            *ppv = static_cast<Pal::PaletteItemInvokeDelegate*>(this);
            AddRef();
            return S_OK;
        }
        *ppv = nullptr;
        return E_NOINTERFACE;
    }
    HRESULT STDMETHODCALLTYPE Invoke(Pal::IPaletteQuery* q) override {
        try {
            if (fn && g_alive) fn(q);
        } catch (...) {
        }
        return S_OK;
    }
};

struct TemplateActionDelegate final : Pal::PaletteTemplateActionInvokeDelegate {
    IMPL_REFCOUNT
    std::function<void(HSTRING, HSTRING, HSTRING)> fn;
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppv) override {
        if (riid == IID_IUnknown || riid == Pal::IID_PaletteTemplateActionInvokeDelegate || riid == IID_IAgileObject_) {
            *ppv = static_cast<Pal::PaletteTemplateActionInvokeDelegate*>(this);
            AddRef();
            return S_OK;
        }
        *ppv = nullptr;
        return E_NOINTERFACE;
    }
    HRESULT STDMETHODCALLTYPE Invoke(Pal::IPaletteTemplateContent*, HSTRING actionId, HSTRING data, HSTRING inputs) override {
        try {
            if (fn && g_alive) fn(actionId, data, inputs);
        } catch (...) {
        }
        return S_OK;
    }
};

struct ContentDelegate final : Pal::PaletteContentDelegate {
    IMPL_REFCOUNT
    std::function<winrt::com_ptr<::IUnknown>(winrt::hstring const&)> fn;
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppv) override {
        if (riid == IID_IUnknown || riid == Pal::IID_PaletteContentDelegate || riid == IID_IAgileObject_) {
            *ppv = static_cast<Pal::PaletteContentDelegate*>(this);
            AddRef();
            return S_OK;
        }
        *ppv = nullptr;
        return E_NOINTERFACE;
    }
    HRESULT STDMETHODCALLTYPE Invoke(HSTRING itemId, IInspectable** operation) override {
        auto* op = new ContentOp();
        winrt::com_ptr<::IUnknown> content;
        try {
            if (fn && g_alive) content = fn(FromH(itemId));
        } catch (...) {
        }
        if (content)
            op->Finish(AS_Completed, content.get());
        else
            op->Finish(AS_Error, nullptr, E_FAIL);
        *operation = static_cast<IAsyncOpRaw*>(op);
        return S_OK;
    }
};

// ---------------------------------------------------------------------------
// Providers
// ---------------------------------------------------------------------------
struct ProviderCallbacks {
    std::function<void(UINT64 q, std::wstring const& text, std::wstring const& json)> onQuery;
    std::function<void(std::wstring const& item, std::wstring const& action, std::wstring const& data,
                       std::wstring const& inputs)>
        onInvoke;
    std::function<void(UINT64 q)> onCancel;
    std::function<void()> onDetach;
};

struct Extension;
struct Definition;

struct CardState {
    winrt::com_ptr<Pal::IPaletteTemplateContent> content;
    std::wstring templateJson, dataJson;
};

struct Provider {
    UINT64 handle = 0;
    std::wstring id, name, description, extId;
    std::wstring glyph, font, iconPath;
    bool zeroState = false;
    std::vector<std::wstring> kinds, containedKinds, itemKinds;
    ProviderCallbacks cb;
    void* owner = nullptr;  // pipe client, for cleanup

    winrt::com_ptr<Pal::IPaletteItemFactory> factory;
    winrt::com_ptr<Definition> def;

    std::mutex m;
    std::wstring staticItemsJson = L"[]";
    std::vector<winrt::com_ptr<Pal::IPaletteItemView>> staticItems;
    std::map<std::wstring, winrt::com_ptr<Pal::IPaletteItemView>> itemCache;
    std::map<std::wstring, CardState> cards;
    std::vector<Extension*> extensions;  // weak, removed in ~Extension
};

static std::mutex g_providersLock;
static std::vector<std::shared_ptr<Provider>> g_providers;
static std::atomic<UINT64> g_nextHandle{0x5E00};

static std::shared_ptr<Provider> FindProvider(UINT64 h) {
    std::lock_guard l(g_providersLock);
    for (auto& p : g_providers)
        if (p->handle == h) return p;
    return nullptr;
}
static std::shared_ptr<Provider> FindProviderByExtId(winrt::hstring const& ext) {
    std::lock_guard l(g_providersLock);
    for (auto& p : g_providers)
        if (ext == p->extId) return p;
    return nullptr;
}
static std::vector<std::shared_ptr<Provider>> ProvidersSnapshot() {
    std::lock_guard l(g_providersLock);
    return g_providers;
}

// ---------------------------------------------------------------------------
// JSON -> native PaletteItem
// ---------------------------------------------------------------------------
static std::wstring JStr(wdj::JsonObject const& o, const wchar_t* k, std::wstring def = L"") {
    if (!o.HasKey(k)) return def;
    auto v = o.GetNamedValue(k);
    switch (v.ValueType()) {
    case wdj::JsonValueType::String: return std::wstring(v.GetString());
    case wdj::JsonValueType::Null: return def;
    default: return std::wstring(v.Stringify());
    }
}
static std::vector<std::wstring> JStrArr(wdj::JsonObject const& o, const wchar_t* k) {
    std::vector<std::wstring> r;
    if (!o.HasKey(k)) return r;
    auto v = o.GetNamedValue(k);
    if (v.ValueType() == wdj::JsonValueType::String) {
        r.emplace_back(v.GetString());
    } else if (v.ValueType() == wdj::JsonValueType::Array) {
        for (auto e : v.GetArray())
            if (e.ValueType() == wdj::JsonValueType::String) r.emplace_back(e.GetString());
    }
    return r;
}
static wf::IInspectable JsonToBoxed(wdj::IJsonValue const& v) {
    switch (v.ValueType()) {
    case wdj::JsonValueType::String: return winrt::box_value(v.GetString());
    case wdj::JsonValueType::Number: return winrt::box_value(v.GetNumber());
    case wdj::JsonValueType::Boolean: return winrt::box_value(v.GetBoolean());
    case wdj::JsonValueType::Null: return nullptr;
    default: return winrt::box_value(v.Stringify());
    }
}

static winrt::com_ptr<Pal::IPaletteIconData> MakeIcon(std::wstring const& glyph, std::wstring const& font,
                                                      std::wstring const& uriOrPath) {
    winrt::com_ptr<Pal::IPaletteIconData> icon;
    auto* st = CACHED_FACTORY(IPaletteIconDataStatics, L"Windows.UI.Palettes.PaletteIconData");
    if (!st) return icon;
    try {
        if (!glyph.empty()) {
            winrt::hstring g(glyph), f(font.empty() ? L"Segoe Fluent Icons" : font);
            st->FromGlyph2(H(g), H(f), icon.put());
        } else if (!uriOrPath.empty()) {
            std::wstring u = uriOrPath;
            if (u.find(L"://") == std::wstring::npos && u.find(L":") == 1) {  // C:\path
                wchar_t buf[2048];
                DWORD n = ARRAYSIZE(buf);
                if (SUCCEEDED(UrlCreateFromPathW(u.c_str(), buf, &n, 0))) u = buf;
            }
            wf::Uri uri(u);
            st->FromUri((IInspectable*)winrt::get_abi(uri), icon.put());
        }
    } catch (...) {
        LOG(L"bad icon '%s'", uriOrPath.c_str());
    }
    return icon;
}
static winrt::com_ptr<Pal::IPaletteIconData> MakeIconJson(wdj::IJsonValue const& v) {
    if (v.ValueType() == wdj::JsonValueType::String) {
        std::wstring s(v.GetString());
        if (s.size() <= 2) return MakeIcon(s, L"", L"");
        return MakeIcon(L"", L"", s);
    }
    if (v.ValueType() == wdj::JsonValueType::Object) {
        auto o = v.GetObject();
        std::wstring uri = JStr(o, L"uri");
        if (uri.empty()) uri = JStr(o, L"path");
        return MakeIcon(JStr(o, L"glyph"), JStr(o, L"font"), uri);
    }
    return nullptr;
}

static void CopyToClipboard(std::wstring const& text) {
    if (!OpenClipboard(nullptr)) return;
    EmptyClipboard();
    size_t bytes = (text.size() + 1) * sizeof(wchar_t);
    HGLOBAL g = GlobalAlloc(GMEM_MOVEABLE, bytes);
    if (g) {
        memcpy(GlobalLock(g), text.c_str(), bytes);
        GlobalUnlock(g);
        if (!SetClipboardData(CF_UNICODETEXT, g)) GlobalFree(g);
    }
    CloseClipboard();
}

static void DispatchInvoke(std::shared_ptr<Provider> const& p, std::wstring const& item, std::wstring const& action,
                           std::wstring const& data, std::wstring const& inputs) {
    DLOG(L"[%s] invoke item=%s action=%s data=%s inputs=%s", p->id.c_str(), item.c_str(), action.c_str(),
         data.c_str(), inputs.c_str());
    if (!p->cb.onInvoke) return;
    auto cb = p->cb.onInvoke;
    std::thread([cb, item, action, data, inputs] {
        try {
            cb(item, action, data, inputs);
        } catch (...) {
        }
    }).detach();
}

static winrt::com_ptr<::IUnknown> MakeCardContent(std::shared_ptr<Provider> const& p, std::wstring const& itemId) {
    std::wstring tmpl, data;
    {
        std::lock_guard l(p->m);
        auto it = p->cards.find(itemId);
        if (it == p->cards.end()) return nullptr;
        tmpl = it->second.templateJson;
        data = it->second.dataJson;
    }
    auto* f = CACHED_FACTORY(IPaletteTemplateContentFactory, L"Windows.UI.Palettes.PaletteTemplateContent");
    if (!f) return nullptr;
    winrt::com_ptr<TemplateActionDelegate> act;
    act.attach(new TemplateActionDelegate());
    std::weak_ptr<Provider> wp = p;
    act->fn = [wp, itemId](HSTRING a, HSTRING d, HSTRING in) {
        if (auto p = wp.lock())
            DispatchInvoke(p, itemId, std::wstring(FromH(a)), std::wstring(FromH(d)), std::wstring(FromH(in)));
    };
    winrt::com_ptr<Pal::IPaletteTemplateContent> content;
    if (FAILED(f->CreateInstance(act.get(), content.put())) || !content) return nullptr;
    winrt::hstring ht(tmpl), hd(data);
    content->put_TemplateJson(H(ht));
    content->put_DataJson(H(hd));
    {
        std::lock_guard l(p->m);
        p->cards[itemId].content = content;
    }
    winrt::com_ptr<::IUnknown> asContent;
    content->QueryInterface(Pal::IID_IPaletteContent, asContent.put_void());
    DLOG(L"[%s] card content created for %s", p->id.c_str(), itemId.c_str());
    return asContent;
}

static winrt::com_ptr<Pal::IPaletteItemView> BuildItem(std::shared_ptr<Provider> const& p, wdj::JsonObject const& o,
                                                       int index) {
    winrt::com_ptr<Pal::IPaletteItemView> view;
    if (!p->factory) return view;
    std::wstring title = JStr(o, L"title");
    std::wstring id = JStr(o, L"id");
    if (id.empty()) id = L"item" + std::to_wstring(index) + L"-" + title;

    auto kindsV = JStrArr(o, L"kinds");
    if (kindsV.empty()) kindsV = p->itemKinds;
    auto kinds = MakeKinds(kindsV);

    winrt::com_ptr<Pal::IPaletteItem> item;
    winrt::hstring hid(id);
    HRESULT hr = kinds ? p->factory->Create2(H(hid), kinds.get(), item.put()) : p->factory->Create(H(hid), item.put());
    if (FAILED(hr) || !item) {
        LOG(L"[%s] PaletteItemFactory.Create failed %08lx", p->id.c_str(), hr);
        return view;
    }

    winrt::hstring ht(title);
    item->put_Title(H(ht));

    auto subs = JStrArr(o, L"subtitle");
    if (!subs.empty()) {
        std::vector<wf::IInspectable> boxes;
        std::vector<IInspectable*> raw;
        for (auto& s : subs) boxes.push_back(winrt::box_value(winrt::hstring(s)));
        for (auto& b : boxes) raw.push_back((IInspectable*)winrt::get_abi(b));
        item->put_Subtitle((UINT32)raw.size(), raw.data());
    }

    if (o.HasKey(L"icon")) {
        if (auto icon = MakeIconJson(o.GetNamedValue(L"icon"))) item->put_Icon(icon.get());
    } else if (auto icon = MakeIcon(p->glyph, p->font, p->iconPath)) {
        item->put_Icon(icon.get());
    }
    if (o.HasKey(L"iconOverlay"))
        if (auto icon = MakeIconJson(o.GetNamedValue(L"iconOverlay"))) item->put_IconOverlay(icon.get());

    if (o.HasKey(L"tags") && o.GetNamedValue(L"tags").ValueType() == wdj::JsonValueType::Array) {
        auto* tf = CACHED_FACTORY(IPaletteTagFactory, L"Windows.UI.Palettes.PaletteTag");
        std::vector<winrt::com_ptr<Pal::IPaletteTag>> tags;
        for (auto tv : o.GetNamedArray(L"tags")) {
            if (!tf || tv.ValueType() != wdj::JsonValueType::Object) continue;
            auto to = tv.GetObject();
            winrt::com_ptr<Pal::IPaletteIconData> ti;
            if (to.HasKey(L"icon")) ti = MakeIconJson(to.GetNamedValue(L"icon"));
            winrt::hstring tt(JStr(to, L"text")), tip(JStr(to, L"tooltip"));
            winrt::com_ptr<Pal::IPaletteTag> tag;
            if (SUCCEEDED(tf->CreateInstance(ti.get(), H(tt), H(tip), tag.put()))) tags.push_back(tag);
        }
        std::vector<Pal::IPaletteTag*> raw;
        for (auto& t : tags) raw.push_back(t.get());
        if (!raw.empty()) item->put_Tags((UINT32)raw.size(), raw.data());
    }

    // Properties (score + arbitrary).
    winrt::com_ptr<IInspectable> propsRaw;
    if (SUCCEEDED(item->get_Properties(propsRaw.put())) && propsRaw) {
        wfc::IMap<winrt::hstring, wf::IInspectable> props{nullptr};
        winrt::copy_from_abi(props, propsRaw.get());
        try {
            if (o.HasKey(L"score")) props.Insert(L"Score", winrt::box_value(o.GetNamedNumber(L"score")));
            if (o.HasKey(L"properties") && o.GetNamedValue(L"properties").ValueType() == wdj::JsonValueType::Object)
                for (auto kv : o.GetNamedObject(L"properties")) props.Insert(kv.Key(), JsonToBoxed(kv.Value()));
        } catch (...) {
            LOG(L"[%s] failed to set properties on %s", p->id.c_str(), id.c_str());
        }
    }

    // Action.
    std::wstring type = L"callback";
    wdj::JsonObject action{nullptr};
    if (o.HasKey(L"action") && o.GetNamedValue(L"action").ValueType() == wdj::JsonValueType::Object) {
        action = o.GetNamedObject(L"action");
        type = JStr(action, L"type", L"callback");
    }
    std::weak_ptr<Provider> wp = p;
    auto makeDelegateInvoker = [&](std::function<void(Pal::IPaletteQuery*)> fn) {
        winrt::com_ptr<Pal::IPaletteInvoker> inv;
        auto* df = CACHED_FACTORY(IPaletteDelegateInvokerFactory, L"Windows.UI.Palettes.PaletteDelegateInvoker");
        if (!df) return inv;
        winrt::com_ptr<InvokeDelegate> d;
        d.attach(new InvokeDelegate());
        d->fn = std::move(fn);
        winrt::com_ptr<IInspectable> di;
        if (SUCCEEDED(df->CreateInstance(d.get(), (Pal::IPaletteDelegateInvoker**)di.put())) && di)
            di->QueryInterface(Pal::IID_IPaletteInvoker, inv.put_void());
        return inv;
    };
    winrt::com_ptr<Pal::IPaletteInvoker> invoker;
    if (type == L"uri") {
        auto* uf = CACHED_FACTORY(IPaletteUriInvokerFactory, L"Windows.UI.Palettes.PaletteUriInvoker");
        try {
            wf::Uri uri(JStr(action, L"uri"));
            winrt::com_ptr<IInspectable> ui;
            if (uf && SUCCEEDED(uf->CreateInstance((IInspectable*)winrt::get_abi(uri), (Pal::IPaletteUriInvoker**)ui.put())) && ui)
                ui->QueryInterface(Pal::IID_IPaletteInvoker, invoker.put_void());
        } catch (...) {
            LOG(L"[%s] bad uri on %s", p->id.c_str(), id.c_str());
        }
    } else if (type == L"run") {
        std::wstring cmd = JStr(action, L"command"), args = JStr(action, L"args"), dir = JStr(action, L"cwd");
        invoker = makeDelegateInvoker([cmd, args, dir](Pal::IPaletteQuery*) {
            ShellExecuteW(nullptr, L"open", cmd.c_str(), args.empty() ? nullptr : args.c_str(),
                          dir.empty() ? nullptr : dir.c_str(), SW_SHOWNORMAL);
        });
    } else if (type == L"copy") {
        std::wstring text = JStr(action, L"text", title);
        invoker = makeDelegateInvoker([text](Pal::IPaletteQuery*) { CopyToClipboard(text); });
    } else if (type == L"toggle") {
        bool on = action.HasKey(L"isToggled") && action.GetNamedBoolean(L"isToggled");
        auto inner = makeDelegateInvoker([wp, id, on](Pal::IPaletteQuery*) {
            if (auto p = wp.lock()) DispatchInvoke(p, id, L"toggle", on ? L"false" : L"true", L"");
        });
        auto* tf = CACHED_FACTORY(IPaletteToggleInvokerFactory, L"Windows.UI.Palettes.PaletteToggleInvoker");
        winrt::com_ptr<IInspectable> ti;
        if (tf && inner && SUCCEEDED(tf->CreateInstance(inner.get(), on, (Pal::IPaletteToggleInvoker**)ti.put())) && ti)
            ti->QueryInterface(Pal::IID_IPaletteInvoker, invoker.put_void());
    } else if (type != L"none") {
        invoker = makeDelegateInvoker([wp, id](Pal::IPaletteQuery*) {
            if (auto p = wp.lock()) DispatchInvoke(p, id, L"invoke", L"", L"");
        });
    }
    if (invoker) item->put_Invoker(invoker.get());

    // Card.
    if (o.HasKey(L"card") && o.GetNamedValue(L"card").ValueType() == wdj::JsonValueType::Object) {
        auto card = o.GetNamedObject(L"card");
        std::wstring tmpl = JStr(card, L"template"), data = JStr(card, L"data", L"{}");
        {
            std::lock_guard l(p->m);
            auto& cs = p->cards[id];
            cs.templateJson = tmpl;
            cs.dataJson = data;
            cs.content = nullptr;
        }
        winrt::com_ptr<ContentDelegate> cd;
        cd.attach(new ContentDelegate());
        cd->fn = [wp](winrt::hstring const& itemId) -> winrt::com_ptr<::IUnknown> {
            if (auto p = wp.lock()) return MakeCardContent(p, std::wstring(itemId));
            return nullptr;
        };
        item->put_ContentDelegate(cd.get());
    }

    if (FAILED(item->GetView(view.put()))) view = nullptr;
    if (view) {
        std::lock_guard l(p->m);
        p->itemCache[id] = view;
    }
    return view;
}

static std::vector<winrt::com_ptr<Pal::IPaletteItemView>> BuildItems(std::shared_ptr<Provider> const& p,
                                                                     std::wstring const& json) {
    std::vector<winrt::com_ptr<Pal::IPaletteItemView>> out;
    try {
        auto v = wdj::JsonValue::Parse(json);
        wdj::JsonArray arr{nullptr};
        if (v.ValueType() == wdj::JsonValueType::Array)
            arr = v.GetArray();
        else if (v.ValueType() == wdj::JsonValueType::Object && v.GetObject().HasKey(L"items"))
            arr = v.GetObject().GetNamedArray(L"items");
        else if (v.ValueType() == wdj::JsonValueType::Object) {
            arr = wdj::JsonArray();
            arr.Append(v);
        }
        int i = 0;
        if (arr)
            for (auto e : arr) {
                if (e.ValueType() == wdj::JsonValueType::Object)
                    if (auto view = BuildItem(p, e.GetObject(), i)) out.push_back(view);
                i++;
            }
    } catch (winrt::hresult_error const& e) {
        LOG(L"[%s] bad items JSON: %s", p->id.c_str(), e.message().c_str());
    } catch (...) {
        LOG(L"[%s] bad items JSON", p->id.c_str());
    }
    return out;
}

// ---------------------------------------------------------------------------
// Queries
// ---------------------------------------------------------------------------
struct Query {
    UINT64 handle;
    std::weak_ptr<Provider> provider;
    winrt::com_ptr<Pal::IPaletteQuery> query;
    winrt::com_ptr<ResultsOp> op;
    std::mutex m;
    std::vector<winrt::com_ptr<Pal::IPaletteItemView>> items;
    PTP_TIMER timer = nullptr;
    std::atomic<bool> finished{false};
};
static std::mutex g_queriesLock;
static std::map<UINT64, std::shared_ptr<Query>> g_queries;
static std::atomic<UINT64> g_nextQuery{1};

static void FinishQuery(std::shared_ptr<Query> const& q, const wchar_t* why) {
    if (q->finished.exchange(true)) return;
    {
        std::lock_guard l(g_queriesLock);
        g_queries.erase(q->handle);
    }
    if (q->timer) {
        SetThreadpoolTimer(q->timer, nullptr, 0, 0);
        // Can't wait from inside the callback; just close (callback holds a ref).
        CloseThreadpoolTimer(q->timer);
        q->timer = nullptr;
    }
    std::vector<winrt::com_ptr<Pal::IPaletteItemView>> items;
    {
        std::lock_guard l(q->m);
        items = q->items;
    }
    winrt::com_ptr<Pal::IPaletteQueryResults> results;
    auto* rf = CACHED_FACTORY(IPaletteQueryResultsFactory, L"Windows.UI.Palettes.PaletteQueryResults");
    HRESULT hr = rf ? rf->CreateInstance(q->query.get(), results.put()) : E_FAIL;
    winrt::com_ptr<Pal::IPaletteQueryResultsView> view;
    if (SUCCEEDED(hr) && results) {
        std::vector<Pal::IPaletteItemView*> raw;
        for (auto& i : items) raw.push_back(i.get());
        results->put_Items((UINT32)raw.size(), raw.data());
        results->put_HasMoreItems(false);
        hr = results->GetView(view.put());
    }
    auto p = q->provider.lock();
    DLOG(L"[%s] query %llu finished (%s): %zu items, hr=%08lx", p ? p->id.c_str() : L"?", q->handle, why,
         items.size(), hr);
    if (view)
        q->op->Finish(AS_Completed, view.get());
    else
        q->op->Finish(AS_Error, nullptr, FAILED(hr) ? hr : E_FAIL);
}

static std::shared_ptr<Query> FindQuery(UINT64 h) {
    std::lock_guard l(g_queriesLock);
    auto it = g_queries.find(h);
    return it == g_queries.end() ? nullptr : it->second;
}

static bool SubmitResults(UINT64 qh, std::wstring const& json, bool done) {
    auto q = FindQuery(qh);
    if (!q) return false;
    auto p = q->provider.lock();
    if (!p) return false;
    auto items = BuildItems(p, json);
    {
        std::lock_guard l(q->m);
        for (auto& i : items) q->items.push_back(i);
    }
    if (done) {
        FinishQuery(q, L"done");
    } else if (!items.empty()) {
        // Stream partial results as progress.
        auto* pf = CACHED_FACTORY(IPaletteQueryProgressFactory, L"Windows.UI.Palettes.PaletteQueryProgress");
        if (pf) {
            std::vector<Pal::IPaletteItemView*> raw;
            for (auto& i : items) raw.push_back(i.get());
            winrt::com_ptr<IInspectable> prog;
            if (SUCCEEDED(pf->CreateInstance((UINT32)raw.size(), raw.data(), nullptr, (Pal::IPaletteQueryProgress**)prog.put())) && prog)
                q->op->ReportProgress(prog.get());
        }
    }
    return true;
}

static std::wstring QueryText(Pal::IPaletteQuery* q, std::wstring& jsonOut) {
    std::wstring text;
    winrt::hstring json;
    if (SUCCEEDED(q->ToJson((HSTRING*)winrt::put_abi(json)))) jsonOut = std::wstring(json);
    UINT32 n = 0;
    IInspectable** args = nullptr;
    if (SUCCEEDED(q->get_Arguments(&n, &args)) && args) {
        for (UINT32 i = 0; i < n; i++) {
            if (!args[i]) continue;
            if (text.empty()) {
                wf::IInspectable a{nullptr};
                winrt::copy_from_abi(a, args[i]);
                if (auto pv = a.try_as<wf::IPropertyValue>(); pv && pv.Type() == wf::PropertyType::String)
                    text = std::wstring(pv.GetString());
            }
            args[i]->Release();
        }
        CoTaskMemFree(args);
    }
    if (text.empty() && !jsonOut.empty()) {
        // Fall back to the JSON form: first string inside "Arguments"/"arguments", else "Query"/"text".
        try {
            auto o = wdj::JsonObject::Parse(jsonOut);
            for (auto k : {L"Arguments", L"arguments"}) {
                if (o.HasKey(k) && o.GetNamedValue(k).ValueType() == wdj::JsonValueType::Array)
                    for (auto e : o.GetNamedArray(k)) {
                        if (e.ValueType() == wdj::JsonValueType::String) return std::wstring(e.GetString());
                        if (e.ValueType() == wdj::JsonValueType::Object) {
                            auto eo = e.GetObject();
                            for (auto kk : {L"Value", L"value", L"Text", L"text"})
                                if (eo.HasKey(kk) && eo.GetNamedValue(kk).ValueType() == wdj::JsonValueType::String)
                                    return std::wstring(eo.GetNamedString(kk));
                        }
                    }
            }
            for (auto k : {L"Query", L"query", L"Text", L"text"})
                if (o.HasKey(k) && o.GetNamedValue(k).ValueType() == wdj::JsonValueType::String)
                    return std::wstring(o.GetNamedString(k));
        } catch (...) {
        }
    }
    return text;
}

static VOID CALLBACK QueryTimeout(PTP_CALLBACK_INSTANCE, PVOID ctx, PTP_TIMER) {
    auto* sp = (std::shared_ptr<Query>*)ctx;
    std::shared_ptr<Query> q = *sp;
    delete sp;
    FinishQuery(q, L"timeout");
}

static IAsyncOpWithProgressRaw* StartQuery(std::shared_ptr<Provider> const& p, Pal::IPaletteQuery* pq) {
    auto q = std::make_shared<Query>();
    q->handle = g_nextQuery++;
    q->provider = p;
    q->query.copy_from(pq);
    q->op.attach(new ResultsOp());
    std::weak_ptr<Query> wq = q;
    q->op->onCancel = [wq] {
        if (auto q = wq.lock()) {
            if (auto p = q->provider.lock())
                if (p->cb.onCancel) p->cb.onCancel(q->handle);
            q->finished = true;
            std::lock_guard l(g_queriesLock);
            g_queries.erase(q->handle);
        }
    };
    std::wstring json;
    std::wstring text = QueryText(pq, json);
    INT32 mode = 0;
    pq->get_Mode(&mode);
    DLOG(L"[%s] query %llu mode=%d text='%s' json=%s", p->id.c_str(), q->handle, mode, text.c_str(), json.c_str());

    IAsyncOpWithProgressRaw* ret = static_cast<IAsyncOpWithProgressRaw*>(q->op.get());
    ret->AddRef();

    if (!g_alive || !p->cb.onQuery || (text.empty() && !p->zeroState)) {
        FinishQuery(q, L"skipped");
        return ret;
    }
    {
        std::lock_guard l(g_queriesLock);
        g_queries[q->handle] = q;
    }
    q->timer = CreateThreadpoolTimer(QueryTimeout, new std::shared_ptr<Query>(q), nullptr);
    if (q->timer) {
        LARGE_INTEGER due;
        due.QuadPart = -10000LL * g_settings.queryTimeoutMs;
        FILETIME ft{due.LowPart, (DWORD)due.HighPart};
        SetThreadpoolTimer(q->timer, &ft, 0, 0);
    }
    auto cb = p->cb.onQuery;
    UINT64 h = q->handle;
    // Never run provider code on Windows' thread.
    std::thread([cb, h, text, json] {
        try {
            cb(h, text, json);
        } catch (...) {
        }
    }).detach();
    return ret;
}

// ---------------------------------------------------------------------------
// IPaletteItemSourceExtension
// ---------------------------------------------------------------------------
struct Extension final : Pal::IPaletteItemSourceExtension {
    IMPL_REFCOUNT
    IMPL_INSPECTABLE(L"Windhawk.SearchMenuExtensions.ItemSource")
    std::weak_ptr<Provider> provider;
    winrt::com_ptr<Pal::IPaletteExtensionHost> host;
    std::mutex m;
    std::map<INT64, winrt::com_ptr<::IUnknown>> handlers;
    INT64 nextToken = 1;

    explicit Extension(std::shared_ptr<Provider> const& p) : provider(p) {
        std::lock_guard l(p->m);
        p->extensions.push_back(this);
    }
    ~Extension() {
        if (auto p = provider.lock()) {
            std::lock_guard l(p->m);
            std::erase(p->extensions, this);
        }
    }
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppv) override {
        if (riid == IID_IUnknown || riid == kIID_IInspectable || riid == Pal::IID_IPaletteItemSourceExtension ||
            riid == IID_IAgileObject_) {
            *ppv = static_cast<Pal::IPaletteItemSourceExtension*>(this);
            AddRef();
            return S_OK;
        }
        *ppv = nullptr;
        return E_NOINTERFACE;
    }
    HRESULT STDMETHODCALLTYPE Initialize(Pal::IPaletteItemFactory*, Pal::IPaletteExtensionHost* h) override {
        host.copy_from(h);
        auto p = provider.lock();
        DLOG(L"[%s] Initialize", p ? p->id.c_str() : L"?");
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE GetItem(HSTRING id, Pal::IPaletteItemView** result) override {
        *result = nullptr;
        auto p = provider.lock();
        if (!p) return S_OK;
        std::wstring k(FromH(id));
        DLOG(L"[%s] GetItem %s", p->id.c_str(), k.c_str());
        std::lock_guard l(p->m);
        auto it = p->itemCache.find(k);
        if (it != p->itemCache.end()) it->second.copy_to(result);
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE GetItems(UINT32* n, Pal::IPaletteItemView*** result) override {
        *n = 0;
        *result = nullptr;
        auto p = provider.lock();
        if (!p || !g_alive) return S_OK;
        std::lock_guard l(p->m);
        DLOG(L"[%s] GetItems -> %zu", p->id.c_str(), p->staticItems.size());
        if (p->staticItems.empty()) return S_OK;
        auto** arr = (Pal::IPaletteItemView**)CoTaskMemAlloc(sizeof(void*) * p->staticItems.size());
        if (!arr) return E_OUTOFMEMORY;
        for (size_t i = 0; i < p->staticItems.size(); i++) p->staticItems[i].copy_to(&arr[i]);
        *n = (UINT32)p->staticItems.size();
        *result = arr;
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE ExecuteQueryAsync(Pal::IPaletteQuery* query, IInspectable** operation) override {
        auto p = provider.lock();
        if (!p) {
            auto* op = new ResultsOp();
            op->Finish(AS_Error, nullptr, E_ABORT);
            *operation = static_cast<IAsyncOpWithProgressRaw*>(op);
            return S_OK;
        }
        *operation = StartQuery(p, query);
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE ReportItemAnalyticsAsync(UINT32, Pal::IPaletteItemAnalytics**, IInspectable** op) override {
        *op = static_cast<IAsyncActionRaw*>(new DoneAction());
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE ReportConversationAnalyticsAsync(UINT32, Pal::IPaletteItemAnalytics**, IInspectable** op) override {
        *op = static_cast<IAsyncActionRaw*>(new DoneAction());
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE add_ItemsChanged(IInspectable* handler, INT64* token) override {
        std::lock_guard l(m);
        *token = nextToken++;
        handlers[*token].copy_from((::IUnknown*)handler);
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE remove_ItemsChanged(INT64 token) override {
        std::lock_guard l(m);
        handlers.erase(token);
        return S_OK;
    }
    void RaiseItemsChanged() {
        std::vector<winrt::com_ptr<::IUnknown>> hs;
        {
            std::lock_guard l(m);
            for (auto& [k, h] : handlers) hs.push_back(h);
        }
        for (auto& h : hs) ((IHandler2Ptr*)h.get())->Invoke(static_cast<Pal::IPaletteItemSourceExtension*>(this), nullptr);
    }
};

// ---------------------------------------------------------------------------
// IPaletteExtensionDefinition
// ---------------------------------------------------------------------------
struct Definition final : Pal::IPaletteExtensionDefinition {
    IMPL_REFCOUNT
    IMPL_INSPECTABLE(L"Windows.UI.Palettes.PaletteExtensionDefinition")
    std::weak_ptr<Provider> provider;
    winrt::hstring extId, name, description;
    winrt::com_ptr<Pal::IPaletteItemFactory> factory;
    winrt::com_ptr<Pal::IPaletteKinds> kinds, contained;
    winrt::com_ptr<Pal::IPaletteIconData> icon;
    wfc::IMap<winrt::hstring, wf::IInspectable> props = winrt::single_threaded_map<winrt::hstring, wf::IInspectable>();

    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppv) override {
        if (riid == IID_IUnknown || riid == kIID_IInspectable || riid == Pal::IID_IPaletteExtensionDefinition ||
            riid == IID_IAgileObject_) {
            *ppv = static_cast<Pal::IPaletteExtensionDefinition*>(this);
            AddRef();
            return S_OK;
        }
        *ppv = nullptr;
        return E_NOINTERFACE;
    }
    HRESULT STDMETHODCALLTYPE CreateExtension(Pal::IPaletteExtensionHost* host, Pal::IPaletteItemSourceExtension** result) override {
        *result = nullptr;
        auto p = provider.lock();
        LOG(L"CreateExtension for %s (provider %s)", extId.c_str(), p ? L"alive" : L"gone");
        if (!p || !g_alive) return E_ABORT;
        PinModule();
        auto* e = new Extension(p);
        e->host.copy_from(host);
        *result = e;
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE get_ItemFactory(Pal::IPaletteItemFactory** v) override { factory.copy_to(v); return S_OK; }
    HRESULT STDMETHODCALLTYPE get_ExtensionId(HSTRING* v) override { return DupH(extId, v); }
    HRESULT STDMETHODCALLTYPE get_PackageFamilyName(HSTRING* v) override { return DupH(L"Windhawk.SearchMenuExtensions_windhawk", v); }
    HRESULT STDMETHODCALLTYPE get_ApplicationId(HSTRING* v) override { return DupH(L"Windhawk.SearchMenuExtensions_windhawk!App", v); }
    HRESULT STDMETHODCALLTYPE get_Icon(Pal::IPaletteIconData** v) override { icon.copy_to(v); return S_OK; }
    HRESULT STDMETHODCALLTYPE get_DisplayName(HSTRING* v) override { return DupH(name, v); }
    HRESULT STDMETHODCALLTYPE get_Description(HSTRING* v) override { return DupH(description, v); }
    HRESULT STDMETHODCALLTYPE get_AcceptedKindSpecs(UINT32* n, HSTRING** v) override { *n = 0; *v = nullptr; return S_OK; }
    HRESULT STDMETHODCALLTYPE get_Publisher(HSTRING* v) override { return DupH(L"windhawk", v); }
    HRESULT STDMETHODCALLTYPE get_Properties(IInspectable** v) override {
        *v = (IInspectable*)winrt::detach_abi(wfc::IMap<winrt::hstring, wf::IInspectable>(props));
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE get_Kinds(Pal::IPaletteKinds** v) override { kinds.copy_to(v); return S_OK; }
    HRESULT STDMETHODCALLTYPE get_ContainedKinds(Pal::IPaletteKinds** v) override { contained.copy_to(v); return S_OK; }
};

// ---------------------------------------------------------------------------
// Catalog hooks (vtable patches inside OSClient.API.dll)
// ---------------------------------------------------------------------------
struct VtPatch {
    void** slot = nullptr;
    void* orig = nullptr;
    void* hook = nullptr;
};
static std::mutex g_patchLock;
static std::vector<VtPatch> g_patches;

static void* PatchSlot(void** vtbl, int idx, void* hook) {
    std::lock_guard l(g_patchLock);
    void** slot = &vtbl[idx];
    for (auto& p : g_patches)
        if (p.slot == slot) return p.orig;  // already patched
    DWORD old;
    if (!VirtualProtect(slot, sizeof(void*), PAGE_READWRITE, &old)) return nullptr;
    void* orig = *slot;
    *slot = hook;
    VirtualProtect(slot, sizeof(void*), old, &old);
    g_patches.push_back({slot, orig, hook});
    return orig;
}
static void UnpatchAll() {
    std::lock_guard l(g_patchLock);
    for (auto& p : g_patches) {
        DWORD old;
        if (VirtualProtect(p.slot, sizeof(void*), PAGE_READWRITE, &old)) {
            *p.slot = p.orig;
            VirtualProtect(p.slot, sizeof(void*), old, &old);
        }
    }
    g_patches.clear();
}

using GetPalette_t = HRESULT(STDMETHODCALLTYPE*)(void*, HSTRING, void**);
using GetPalettes_t = HRESULT(STDMETHODCALLTYPE*)(void*, UINT32*, void***);
using AddChanged_t = HRESULT(STDMETHODCALLTYPE*)(void*, ::IUnknown*, INT64*);
using RemoveChanged_t = HRESULT(STDMETHODCALLTYPE*)(void*, INT64);
using OpenDefault_t = HRESULT(STDMETHODCALLTYPE*)(void*, void**);
using OpenWithKinds_t = HRESULT(STDMETHODCALLTYPE*)(void*, void*, void**);
using OpenWithKindsAndConfig_t = HRESULT(STDMETHODCALLTYPE*)(void*, void*, HSTRING, void**);
using TryGetItem_t = HRESULT(STDMETHODCALLTYPE*)(void*, HSTRING, HSTRING, void**);
using GetOrdered_t = void(STDMETHODCALLTYPE*)(void*, HSTRING, UINT32*, void***);
using IsEnabled_t = BOOL(STDMETHODCALLTYPE*)(void*, HSTRING);

static GetPalette_t o_GetPalette;
static GetPalettes_t o_GetPalettes;
static AddChanged_t o_AddChanged;
static RemoveChanged_t o_RemoveChanged;
static OpenDefault_t o_OpenDefault;
static OpenWithKinds_t o_OpenWithKinds;
static OpenWithKindsAndConfig_t o_OpenWithKindsAndConfig;
static TryGetItem_t o_TryGetItem;
static GetOrdered_t o_GetOrdered;
static IsEnabled_t o_IsEnabled;
static IsEnabled_t o_IsUserDisabled;

struct ChangedSub {
    winrt::com_ptr<::IUnknown> catalog;  // IPaletteCatalog*
    winrt::com_ptr<::IUnknown> handler;
    INT64 token;
};
static std::mutex g_subsLock;
static std::vector<ChangedSub> g_subs;

static std::vector<winrt::com_ptr<Definition>> DefinitionsFor(const std::wstring* kind) {
    std::vector<winrt::com_ptr<Definition>> r;
    if (!g_alive) return r;
    for (auto& p : ProvidersSnapshot()) {
        if (!p->def) continue;
        if (kind) {
            bool ok = false;
            for (auto& k : p->kinds)
                if (KindMatches(k, *kind)) ok = true;
            if (!ok) continue;
        }
        r.push_back(p->def);
    }
    return r;
}

static void AppendDefs(UINT32* n, void*** arr, std::vector<winrt::com_ptr<Definition>> const& defs) {
    if (defs.empty()) return;
    UINT32 old = *n;
    void** na = (void**)CoTaskMemAlloc(sizeof(void*) * (old + defs.size()));
    if (!na) return;
    if (old && *arr) memcpy(na, *arr, sizeof(void*) * old);
    for (size_t i = 0; i < defs.size(); i++) {
        defs[i]->AddRef();
        na[old + i] = static_cast<Pal::IPaletteExtensionDefinition*>(defs[i].get());
    }
    if (*arr) CoTaskMemFree(*arr);
    *arr = na;
    *n = old + (UINT32)defs.size();
    PinModule();
}

static HRESULT STDMETHODCALLTYPE h_GetPalettes(void* self, UINT32* n, void*** arr) {
    HRESULT hr = o_GetPalettes(self, n, arr);
    if (SUCCEEDED(hr)) {
        UINT32 before = *n;
        AppendDefs(n, arr, DefinitionsFor(nullptr));
        DLOG(L"GetPalettes: %u native + %u ours", before, *n - before);
    }
    return hr;
}
static HRESULT STDMETHODCALLTYPE h_GetPalette(void* self, HSTRING id, void** result) {
    if (auto p = FindProviderByExtId(FromH(id)); p && p->def && g_alive) {
        DLOG(L"GetPalette(%s) -> ours", p->extId.c_str());
        p->def->AddRef();
        *result = static_cast<Pal::IPaletteExtensionDefinition*>(p->def.get());
        return S_OK;
    }
    return o_GetPalette(self, id, result);
}
static HRESULT STDMETHODCALLTYPE h_AddChanged(void* self, ::IUnknown* handler, INT64* token) {
    HRESULT hr = o_AddChanged(self, handler, token);
    if (SUCCEEDED(hr) && handler) {
        ChangedSub s;
        s.catalog.copy_from((::IUnknown*)self);
        s.handler.copy_from(handler);
        s.token = *token;
        std::lock_guard l(g_subsLock);
        g_subs.push_back(std::move(s));
    }
    return hr;
}
static HRESULT STDMETHODCALLTYPE h_RemoveChanged(void* self, INT64 token) {
    {
        std::lock_guard l(g_subsLock);
        std::erase_if(g_subs, [&](ChangedSub const& s) { return s.catalog.get() == self && s.token == token; });
    }
    return o_RemoveChanged(self, token);
}
static void STDMETHODCALLTYPE h_GetOrdered(void* self, HSTRING kind, UINT32* n, void*** arr) {
    o_GetOrdered(self, kind, n, arr);
    std::wstring k(FromH(kind));
    UINT32 before = *n;
    AppendDefs(n, arr, DefinitionsFor(&k));
    DLOG(L"GetOrderedSourceDefinitions(%s): %u native + %u ours", k.c_str(), before, *n - before);
}
static BOOL STDMETHODCALLTYPE h_IsEnabled(void* self, HSTRING id) {
    if (g_alive && FindProviderByExtId(FromH(id))) return TRUE;
    return o_IsEnabled(self, id);
}
static BOOL STDMETHODCALLTYPE h_IsUserDisabled(void* self, HSTRING id) {
    if (g_alive && FindProviderByExtId(FromH(id))) return FALSE;
    return o_IsUserDisabled(self, id);
}

static void PatchCatalogInstance(void* catalog) {
    if (!catalog) return;
    void** vt = *(void***)catalog;  // IPaletteCatalog vtable
    o_GetPalette = (GetPalette_t)PatchSlot(vt, 6, (void*)h_GetPalette);
    o_GetPalettes = (GetPalettes_t)PatchSlot(vt, 7, (void*)h_GetPalettes);
    o_AddChanged = (AddChanged_t)PatchSlot(vt, 8, (void*)h_AddChanged);
    o_RemoveChanged = (RemoveChanged_t)PatchSlot(vt, 9, (void*)h_RemoveChanged);
    winrt::com_ptr<::IUnknown> priv;
    if (SUCCEEDED(((::IUnknown*)catalog)->QueryInterface(IID_IPaletteCatalogPrivate, priv.put_void())) && priv) {
        void** pvt = *(void***)priv.get();
        o_GetOrdered = (GetOrdered_t)PatchSlot(pvt, 7, (void*)h_GetOrdered);
        o_IsEnabled = (IsEnabled_t)PatchSlot(pvt, 8, (void*)h_IsEnabled);
        o_IsUserDisabled = (IsEnabled_t)PatchSlot(pvt, 9, (void*)h_IsUserDisabled);
    }
}

static HRESULT STDMETHODCALLTYPE h_OpenDefault(void* self, void** result) {
    HRESULT hr = o_OpenDefault(self, result);
    if (SUCCEEDED(hr) && *result) PatchCatalogInstance(*result);
    DLOG(L"PaletteCatalog.OpenDefault hr=%08lx", hr);
    return hr;
}
static HRESULT STDMETHODCALLTYPE h_OpenWithKinds(void* self, void* kinds, void** result) {
    HRESULT hr = o_OpenWithKinds(self, kinds, result);
    if (SUCCEEDED(hr) && *result) PatchCatalogInstance(*result);
    DLOG(L"PaletteCatalog.OpenWithKinds hr=%08lx", hr);
    return hr;
}
static HRESULT STDMETHODCALLTYPE h_OpenWithKindsAndConfig(void* self, void* kinds, HSTRING cfg, void** result) {
    HRESULT hr = o_OpenWithKindsAndConfig(self, kinds, cfg, result);
    if (SUCCEEDED(hr) && *result) PatchCatalogInstance(*result);
    DLOG(L"PaletteCatalog.OpenWithKindsAndConfig(%s) hr=%08lx", std::wstring(FromH(cfg)).c_str(), hr);
    return hr;
}
static HRESULT STDMETHODCALLTYPE h_TryGetItem(void* self, HSTRING ext, HSTRING item, void** result) {
    if (auto p = FindProviderByExtId(FromH(ext)); p && g_alive) {
        *result = nullptr;
        std::lock_guard l(p->m);
        auto it = p->itemCache.find(std::wstring(FromH(item)));
        if (it != p->itemCache.end()) it->second.copy_to((Pal::IPaletteItemView**)result);
        return S_OK;
    }
    return o_TryGetItem(self, ext, item, result);
}

static winrt::com_ptr<Pal::IPaletteCatalogStatics> g_statics;

static bool InstallCatalogHooks() {
    HMODULE mod = LoadLibraryExW(L"OSClient.API.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!mod) {
        LOG(L"OSClient.API.dll not found - this build has no Windows.UI.Palettes");
        return false;
    }
    using DGAF = HRESULT(WINAPI*)(HSTRING, ::IUnknown**);
    auto dgaf = (DGAF)GetProcAddress(mod, "DllGetActivationFactory");
    if (!dgaf) return false;
    winrt::hstring cls = L"Windows.UI.Palettes.PaletteCatalog";
    winrt::com_ptr<::IUnknown> f;
    HRESULT hr = dgaf(H(cls), f.put());
    if (FAILED(hr) || !f) {
        LOG(L"DllGetActivationFactory(PaletteCatalog) failed %08lx", hr);
        return false;
    }
    if (FAILED(f->QueryInterface(Pal::IID_IPaletteCatalogStatics, g_statics.put_void())) || !g_statics) return false;
    void** vt = *(void***)g_statics.get();
    o_OpenDefault = (OpenDefault_t)PatchSlot(vt, 10, (void*)h_OpenDefault);
    o_OpenWithKinds = (OpenWithKinds_t)PatchSlot(vt, 11, (void*)h_OpenWithKinds);
    o_OpenWithKindsAndConfig = (OpenWithKindsAndConfig_t)PatchSlot(vt, 12, (void*)h_OpenWithKindsAndConfig);
    o_TryGetItem = (TryGetItem_t)PatchSlot(vt, 14, (void*)h_TryGetItem);
    LOG(L"catalog statics hooked");
    return true;
}

// Ask Windows to re-read the catalog (raises PaletteCatalog.Changed on every subscriber we saw).
static void RaiseCatalogChanged() {
    std::vector<ChangedSub> subs;
    {
        std::lock_guard l(g_subsLock);
        subs = g_subs;
    }
    DLOG(L"raising PaletteCatalog.Changed to %zu subscribers", subs.size());
    for (auto& s : subs) {
        try {
            ((IHandler2Ptr*)s.handler.get())->Invoke(s.catalog.get(), nullptr);
        } catch (...) {
        }
    }
}
static std::atomic<bool> g_changePending{false};
static void ScheduleCatalogChanged() {
    if (g_changePending.exchange(true)) return;
    std::thread([] {
        Sleep(300);
        g_changePending = false;
        RaiseCatalogChanged();
    }).detach();
}

// ---------------------------------------------------------------------------
// Provider registration
// ---------------------------------------------------------------------------
static winrt::com_ptr<Pal::IPaletteItemFactory> MakeFactory(winrt::hstring const& extId) {
    winrt::com_ptr<Pal::IPaletteItemFactory> out;
    auto* um = CACHED_FACTORY(IValueUnmarshalByPropertySet, L"Windows.UI.Palettes.PaletteItemFactory");
    if (!um) return out;
    wfc::PropertySet ps;
    ps.Insert(L"ExtensionId", winrt::box_value(extId));
    HRESULT hr = um->UnmarshalObjectFromPropertySet(winrt::get_abi(ps), out.put_void());
    if (FAILED(hr)) LOG(L"PaletteItemFactory unmarshal failed %08lx", hr);
    return out;
}

static bool ValidId(std::wstring const& id) {
    if (id.empty() || id.size() > 64) return false;
    for (wchar_t c : id)
        if (!(iswalnum(c) || c == L'.' || c == L'_' || c == L'-')) return false;
    return true;
}

static UINT64 RegisterProvider(std::wstring const& json, ProviderCallbacks cb, void* owner, std::wstring* err) {
    if (!g_alive) {
        if (err) *err = L"library not running";
        return 0;
    }
    auto p = std::make_shared<Provider>();
    try {
        auto o = wdj::JsonObject::Parse(json);
        p->id = JStr(o, L"id");
        p->name = JStr(o, L"name", p->id);
        p->description = JStr(o, L"description");
        p->glyph = JStr(o, L"glyph");
        p->font = JStr(o, L"font");
        p->iconPath = JStr(o, L"icon");
        p->zeroState = o.HasKey(L"zeroState") && o.GetNamedValue(L"zeroState").ValueType() == wdj::JsonValueType::Boolean &&
                       o.GetNamedBoolean(L"zeroState");
        p->kinds = JStrArr(o, L"kinds");
        p->containedKinds = JStrArr(o, L"containedKinds");
        p->itemKinds = JStrArr(o, L"itemKinds");
    } catch (...) {
        if (err) *err = L"provider JSON is invalid";
        return 0;
    }
    if (!ValidId(p->id)) {
        if (err) *err = L"provider id must be 1-64 chars of [A-Za-z0-9._-]";
        return 0;
    }
    if (p->kinds.empty()) p->kinds = {L"QueryResult.SystemComposer"};
    if (p->zeroState) p->kinds.push_back(L"ZeroState.SystemComposer");
    if (p->itemKinds.empty()) p->itemKinds = {L"Command"};
    if (p->containedKinds.empty()) p->containedKinds = p->itemKinds;
    p->extId = L"Windhawk." + p->id;
    p->cb = std::move(cb);
    p->owner = owner;

    {
        std::lock_guard l(g_providersLock);
        for (auto& q : g_providers)
            if (q->id == p->id) {
                if (err) *err = L"provider id already registered";
                return 0;
            }
    }

    winrt::hstring ext(p->extId);
    p->factory = MakeFactory(ext);
    if (!p->factory) {
        if (err) *err = L"could not create PaletteItemFactory";
        return 0;
    }
    winrt::com_ptr<Definition> def;
    def.attach(new Definition());
    def->provider = p;
    def->extId = ext;
    def->name = winrt::hstring(p->name);
    def->description = winrt::hstring(p->description);
    def->factory = p->factory;
    std::vector<std::wstring> kinds = p->kinds;
    kinds.push_back(L"BuiltIn");  // skips the "new provider" delayed-enablement gate
    def->kinds = MakeKinds(kinds);
    def->contained = MakeKinds(p->containedKinds);
    def->icon = MakeIcon(p->glyph.empty() && p->iconPath.empty() ? std::wstring(L"") : p->glyph, p->font,
                         p->iconPath);
    p->def = def;
    p->handle = g_nextHandle++;
    {
        std::lock_guard l(g_providersLock);
        g_providers.push_back(p);
    }
    LOG(L"provider registered: %s (%s) handle=%llu", p->id.c_str(), p->extId.c_str(), p->handle);
    ScheduleCatalogChanged();
    return p->handle;
}

static bool UnregisterProvider(UINT64 h, bool notifyDetach = false) {
    std::shared_ptr<Provider> p;
    {
        std::lock_guard l(g_providersLock);
        for (auto it = g_providers.begin(); it != g_providers.end(); ++it)
            if ((*it)->handle == h) {
                p = *it;
                g_providers.erase(it);
                break;
            }
    }
    if (!p) return false;
    // Fail any of its pending queries.
    std::vector<std::shared_ptr<Query>> qs;
    {
        std::lock_guard l(g_queriesLock);
        for (auto& [k, q] : g_queries)
            if (q->provider.lock() == p) qs.push_back(q);
    }
    for (auto& q : qs) FinishQuery(q, L"unregistered");
    LOG(L"provider unregistered: %s", p->id.c_str());
    if (notifyDetach && p->cb.onDetach) p->cb.onDetach();
    ScheduleCatalogChanged();
    return true;
}

static bool SetStaticItems(UINT64 h, std::wstring const& json) {
    auto p = FindProvider(h);
    if (!p) return false;
    auto items = BuildItems(p, json);
    {
        std::lock_guard l(p->m);
        p->staticItemsJson = json;
        p->staticItems = std::move(items);
    }
    bool NotifyChanged(UINT64 h);
    return NotifyChanged(h);
}

bool NotifyChanged(UINT64 h) {
    auto p = FindProvider(h);
    if (!p) return false;
    std::vector<winrt::com_ptr<Extension>> exts;
    {
        std::lock_guard l(p->m);
        for (auto* e : p->extensions) {
            winrt::com_ptr<Extension> c;
            c.copy_from(e);
            exts.push_back(c);
        }
    }
    for (auto& e : exts) e->RaiseItemsChanged();
    return true;
}

static bool UpdateCard(UINT64 h, std::wstring const& itemId, std::wstring const& dataJson) {
    auto p = FindProvider(h);
    if (!p) return false;
    winrt::com_ptr<Pal::IPaletteTemplateContent> content;
    {
        std::lock_guard l(p->m);
        auto it = p->cards.find(itemId);
        if (it == p->cards.end()) return false;
        it->second.dataJson = dataJson;
        content = it->second.content;
    }
    if (content) {
        winrt::hstring d(dataJson);
        content->put_DataJson(H(d));
    }
    return true;
}

// ---------------------------------------------------------------------------
// Exported C API (in-process clients)
// ---------------------------------------------------------------------------
extern "C" {
typedef struct WSE_ProviderCallbacks {
    UINT32 cbSize;
    void* ctx;
    void(WINAPI* onQuery)(void* ctx, UINT64 queryHandle, PCWSTR text, PCWSTR queryJson);
    void(WINAPI* onInvoke)(void* ctx, PCWSTR itemId, PCWSTR actionId, PCWSTR dataJson, PCWSTR inputsJson);
    void(WINAPI* onCancel)(void* ctx, UINT64 queryHandle);
    void(WINAPI* onDetach)(void* ctx);
} WSE_ProviderCallbacks;

__declspec(dllexport) int WINAPI WSE_GetApiVersion(void) { return 1; }

__declspec(dllexport) UINT64 WINAPI WSE_RegisterProvider(PCWSTR providerJson, const WSE_ProviderCallbacks* c) {
    if (!providerJson || !c || c->cbSize < offsetof(WSE_ProviderCallbacks, onCancel)) return 0;
    WSE_ProviderCallbacks cc{};
    memcpy(&cc, c, std::min((size_t)c->cbSize, sizeof(cc)));
    ProviderCallbacks cb;
    if (cc.onQuery)
        cb.onQuery = [cc](UINT64 q, std::wstring const& t, std::wstring const& j) { cc.onQuery(cc.ctx, q, t.c_str(), j.c_str()); };
    if (cc.onInvoke)
        cb.onInvoke = [cc](std::wstring const& i, std::wstring const& a, std::wstring const& d, std::wstring const& in) {
            cc.onInvoke(cc.ctx, i.c_str(), a.c_str(), d.c_str(), in.c_str());
        };
    if (cc.onCancel) cb.onCancel = [cc](UINT64 q) { cc.onCancel(cc.ctx, q); };
    if (cc.onDetach) cb.onDetach = [cc] { cc.onDetach(cc.ctx); };
    std::wstring err;
    UINT64 h = RegisterProvider(providerJson, std::move(cb), nullptr, &err);
    if (!h) LOG(L"WSE_RegisterProvider failed: %s", err.c_str());
    return h;
}
__declspec(dllexport) BOOL WINAPI WSE_SubmitResults(UINT64 q, PCWSTR itemsJson, BOOL done) {
    return SubmitResults(q, itemsJson ? itemsJson : L"[]", done);
}
__declspec(dllexport) BOOL WINAPI WSE_SetStaticItems(UINT64 p, PCWSTR itemsJson) {
    return SetStaticItems(p, itemsJson ? itemsJson : L"[]");
}
__declspec(dllexport) BOOL WINAPI WSE_UpdateCard(UINT64 p, PCWSTR itemId, PCWSTR dataJson) {
    return itemId && dataJson && UpdateCard(p, itemId, dataJson);
}
__declspec(dllexport) BOOL WINAPI WSE_NotifyChanged(UINT64 p) { return NotifyChanged(p); }
__declspec(dllexport) BOOL WINAPI WSE_UnregisterProvider(UINT64 p) { return UnregisterProvider(p); }
}

// ---------------------------------------------------------------------------
// Named pipe server (out-of-process clients)
// ---------------------------------------------------------------------------
static const wchar_t kPipeName[] = L"\\\\.\\pipe\\WindhawkSearchExtensions";

static std::string ToUtf8(std::wstring const& w) {
    if (w.empty()) return {};
    int n = WideCharToMultiByte(CP_UTF8, 0, w.data(), (int)w.size(), nullptr, 0, nullptr, nullptr);
    std::string s(n, 0);
    WideCharToMultiByte(CP_UTF8, 0, w.data(), (int)w.size(), s.data(), n, nullptr, nullptr);
    return s;
}
static std::wstring FromUtf8(std::string const& s) {
    if (s.empty()) return {};
    int n = MultiByteToWideChar(CP_UTF8, 0, s.data(), (int)s.size(), nullptr, 0);
    std::wstring w(n, 0);
    MultiByteToWideChar(CP_UTF8, 0, s.data(), (int)s.size(), w.data(), n);
    return w;
}

struct PipeClient : std::enable_shared_from_this<PipeClient> {
    HANDLE pipe = INVALID_HANDLE_VALUE;
    std::mutex writeLock;
    std::mutex provLock;
    std::map<std::wstring, UINT64> providers;  // id -> handle
    std::map<UINT64, std::wstring> queryProvider;
    std::atomic<bool> closed{false};

    void Send(wdj::JsonObject const& o) {
        if (closed) return;
        std::string line = ToUtf8(std::wstring(o.Stringify())) + "\n";
        std::lock_guard l(writeLock);
        OVERLAPPED ov{};
        ov.hEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
        DWORD w = 0;
        if (!WriteFile(pipe, line.data(), (DWORD)line.size(), &w, &ov) && GetLastError() == ERROR_IO_PENDING) {
            if (WaitForSingleObject(ov.hEvent, 5000) != WAIT_OBJECT_0) CancelIoEx(pipe, &ov);
            GetOverlappedResult(pipe, &ov, &w, TRUE);
        }
        CloseHandle(ov.hEvent);
    }
    void Error(std::wstring const& msg) {
        wdj::JsonObject o;
        o.SetNamedValue(L"op", wdj::JsonValue::CreateStringValue(L"error"));
        o.SetNamedValue(L"message", wdj::JsonValue::CreateStringValue(msg));
        Send(o);
    }
    UINT64 Lookup(wdj::JsonObject const& o) {
        std::lock_guard l(provLock);
        auto it = providers.find(JStr(o, L"provider"));
        return it == providers.end() ? 0 : it->second;
    }
    static std::wstring ItemsOf(wdj::JsonObject const& o) {
        if (!o.HasKey(L"items")) return L"[]";
        return std::wstring(o.GetNamedValue(L"items").Stringify());
    }

    void Handle(std::wstring const& line) {
        wdj::JsonObject o{nullptr};
        if (!wdj::JsonObject::TryParse(line, o)) {
            Error(L"invalid JSON");
            return;
        }
        std::wstring op = JStr(o, L"op");
        if (op == L"register") {
            if (!o.HasKey(L"provider") || o.GetNamedValue(L"provider").ValueType() != wdj::JsonValueType::Object) {
                Error(L"register needs a provider object");
                return;
            }
            auto po = o.GetNamedObject(L"provider");
            std::wstring id = JStr(po, L"id");
            std::weak_ptr<PipeClient> self = shared_from_this();
            ProviderCallbacks cb;
            cb.onQuery = [self, id](UINT64 q, std::wstring const& text, std::wstring const& json) {
                auto c = self.lock();
                if (!c) return;
                {
                    std::lock_guard l(c->provLock);
                    c->queryProvider[q] = id;
                }
                wdj::JsonObject m;
                m.SetNamedValue(L"op", wdj::JsonValue::CreateStringValue(L"query"));
                m.SetNamedValue(L"provider", wdj::JsonValue::CreateStringValue(id));
                m.SetNamedValue(L"queryId", wdj::JsonValue::CreateStringValue(std::to_wstring(q)));
                m.SetNamedValue(L"text", wdj::JsonValue::CreateStringValue(text));
                wdj::JsonObject qj{nullptr};
                if (!json.empty() && wdj::JsonObject::TryParse(json, qj)) m.SetNamedValue(L"query", qj);
                c->Send(m);
            };
            cb.onInvoke = [self, id](std::wstring const& item, std::wstring const& action, std::wstring const& data,
                                     std::wstring const& inputs) {
                auto c = self.lock();
                if (!c) return;
                wdj::JsonObject m;
                m.SetNamedValue(L"op", wdj::JsonValue::CreateStringValue(L"invoke"));
                m.SetNamedValue(L"provider", wdj::JsonValue::CreateStringValue(id));
                m.SetNamedValue(L"itemId", wdj::JsonValue::CreateStringValue(item));
                m.SetNamedValue(L"actionId", wdj::JsonValue::CreateStringValue(action));
                m.SetNamedValue(L"data", wdj::JsonValue::CreateStringValue(data));
                m.SetNamedValue(L"inputs", wdj::JsonValue::CreateStringValue(inputs));
                c->Send(m);
            };
            cb.onCancel = [self, id](UINT64 q) {
                auto c = self.lock();
                if (!c) return;
                wdj::JsonObject m;
                m.SetNamedValue(L"op", wdj::JsonValue::CreateStringValue(L"cancel"));
                m.SetNamedValue(L"provider", wdj::JsonValue::CreateStringValue(id));
                m.SetNamedValue(L"queryId", wdj::JsonValue::CreateStringValue(std::to_wstring(q)));
                c->Send(m);
            };
            std::wstring err;
            UINT64 h = RegisterProvider(std::wstring(po.Stringify()), std::move(cb), this, &err);
            if (!h) {
                Error(L"register failed: " + err);
                return;
            }
            {
                std::lock_guard l(provLock);
                providers[id] = h;
            }
            wdj::JsonObject m;
            m.SetNamedValue(L"op", wdj::JsonValue::CreateStringValue(L"registered"));
            m.SetNamedValue(L"provider", wdj::JsonValue::CreateStringValue(id));
            m.SetNamedValue(L"extensionId", wdj::JsonValue::CreateStringValue(L"Windhawk." + id));
            Send(m);
        } else if (op == L"results") {
            UINT64 q = _wcstoui64(JStr(o, L"queryId").c_str(), nullptr, 10);
            bool done = !o.HasKey(L"done") || o.GetNamedValue(L"done").ValueType() != wdj::JsonValueType::Boolean ||
                        o.GetNamedBoolean(L"done");
            SubmitResults(q, ItemsOf(o), done);
        } else if (op == L"setItems") {
            if (UINT64 h = Lookup(o)) SetStaticItems(h, ItemsOf(o));
            else Error(L"unknown provider");
        } else if (op == L"updateCard") {
            UINT64 h = Lookup(o);
            std::wstring data = o.HasKey(L"data") ? std::wstring(o.GetNamedValue(L"data").Stringify()) : L"{}";
            if (o.HasKey(L"data") && o.GetNamedValue(L"data").ValueType() == wdj::JsonValueType::String)
                data = o.GetNamedString(L"data");
            if (!h || !UpdateCard(h, JStr(o, L"itemId"), data)) Error(L"updateCard: unknown provider or item");
        } else if (op == L"refresh") {
            if (UINT64 h = Lookup(o)) NotifyChanged(h);
        } else if (op == L"unregister") {
            UINT64 h = Lookup(o);
            if (h) {
                UnregisterProvider(h);
                std::lock_guard l(provLock);
                providers.erase(JStr(o, L"provider"));
            }
        } else if (op == L"ping") {
            wdj::JsonObject m;
            m.SetNamedValue(L"op", wdj::JsonValue::CreateStringValue(L"pong"));
            Send(m);
        } else {
            Error(L"unknown op '" + op + L"'");
        }
    }

    void Run(HANDLE stopEvent) {
        winrt::init_apartment(winrt::apartment_type::multi_threaded);
        std::string buf;
        char chunk[8192];
        HANDLE ev = CreateEventW(nullptr, TRUE, FALSE, nullptr);
        while (!closed) {
            OVERLAPPED ov{};
            ov.hEvent = ev;
            ResetEvent(ev);
            DWORD n = 0;
            BOOL ok = ReadFile(pipe, chunk, sizeof(chunk), &n, &ov);
            if (!ok && GetLastError() == ERROR_IO_PENDING) {
                HANDLE hs[2] = {ev, stopEvent};
                DWORD w = WaitForMultipleObjects(2, hs, FALSE, INFINITE);
                if (w != WAIT_OBJECT_0) {
                    CancelIoEx(pipe, &ov);
                    GetOverlappedResult(pipe, &ov, &n, TRUE);
                    break;
                }
                ok = GetOverlappedResult(pipe, &ov, &n, FALSE);
            }
            if (!ok && GetLastError() != ERROR_MORE_DATA) break;
            if (n == 0) continue;
            buf.append(chunk, n);
            if (buf.size() > 16 * 1024 * 1024) break;  // runaway client
            size_t pos;
            while ((pos = buf.find('\n')) != std::string::npos) {
                std::string line = buf.substr(0, pos);
                buf.erase(0, pos + 1);
                if (!line.empty() && line.back() == '\r') line.pop_back();
                if (line.empty()) continue;
                try {
                    Handle(FromUtf8(line));
                } catch (winrt::hresult_error const& e) {
                    Error(std::wstring(L"error: ") + e.message().c_str());
                } catch (...) {
                    Error(L"internal error");
                }
            }
        }
        CloseHandle(ev);
        closed = true;
        std::map<std::wstring, UINT64> ps;
        {
            std::lock_guard l(provLock);
            ps.swap(providers);
        }
        for (auto& [id, h] : ps) UnregisterProvider(h);
        std::lock_guard l(writeLock);
        DisconnectNamedPipe(pipe);
        CloseHandle(pipe);
        pipe = INVALID_HANDLE_VALUE;
        winrt::uninit_apartment();
    }
};

static HANDLE g_pipeStop;
static std::thread g_pipeThread;
static std::mutex g_clientThreadsLock;
static std::vector<std::thread> g_clientThreads;

static void PipeListener() {
    HANDLE ev = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    while (WaitForSingleObject(g_pipeStop, 0) != WAIT_OBJECT_0) {
        HANDLE pipe = CreateNamedPipeW(kPipeName, PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED,
                                       PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT | PIPE_REJECT_REMOTE_CLIENTS,
                                       PIPE_UNLIMITED_INSTANCES, 64 * 1024, 64 * 1024, 0, nullptr);
        if (pipe == INVALID_HANDLE_VALUE) {
            LOG(L"CreateNamedPipe failed: %lu", GetLastError());
            if (WaitForSingleObject(g_pipeStop, 5000) == WAIT_OBJECT_0) break;
            continue;
        }
        OVERLAPPED ov{};
        ov.hEvent = ev;
        ResetEvent(ev);
        BOOL connected = ConnectNamedPipe(pipe, &ov);
        DWORD err = GetLastError();
        if (!connected && err == ERROR_IO_PENDING) {
            HANDLE hs[2] = {ev, g_pipeStop};
            if (WaitForMultipleObjects(2, hs, FALSE, INFINITE) != WAIT_OBJECT_0) {
                CancelIoEx(pipe, &ov);
                CloseHandle(pipe);
                break;
            }
            DWORD dummy;
            connected = GetOverlappedResult(pipe, &ov, &dummy, FALSE);
        } else if (!connected && err == ERROR_PIPE_CONNECTED) {
            connected = TRUE;
        }
        if (!connected) {
            CloseHandle(pipe);
            continue;
        }
        auto c = std::make_shared<PipeClient>();
        c->pipe = pipe;
        DLOG(L"pipe client connected");
        std::lock_guard l(g_clientThreadsLock);
        g_clientThreads.emplace_back([c] { c->Run(g_pipeStop); });
    }
    CloseHandle(ev);
}

static void StartPipe() {
    g_pipeStop = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_pipeThread = std::thread(PipeListener);
}
static void StopPipe() {
    if (!g_pipeStop) return;
    SetEvent(g_pipeStop);
    if (g_pipeThread.joinable()) g_pipeThread.join();
    std::vector<std::thread> ts;
    {
        std::lock_guard l(g_clientThreadsLock);
        ts.swap(g_clientThreads);
    }
    for (auto& t : ts)
        if (t.joinable()) t.join();
    CloseHandle(g_pipeStop);
    g_pipeStop = nullptr;
}

// ---------------------------------------------------------------------------
// Demo provider
// ---------------------------------------------------------------------------
static UINT64 g_demo;
static std::atomic<bool> g_demoToggle{false};
static std::atomic<int> g_demoClicks{0};

static std::wstring JsonEscape(std::wstring const& s) {
    return std::wstring(wdj::JsonValue::CreateStringValue(s).Stringify());
}

static void DemoQuery(UINT64 q, std::wstring const& text) {
    std::wstring t = text;
    if (_wcsnicmp(t.c_str(), L"wh", 2) != 0) {
        SubmitResults(q, L"[]", TRUE);
        return;
    }
    std::wstring rest = t.size() > 2 ? t.substr(2) : L"";
    while (!rest.empty() && rest[0] == L' ') rest.erase(0, 1);
    std::wstring json = L"[";
    json += L"{\"id\":\"echo\",\"title\":" + JsonEscape(L"Windhawk heard: " + (rest.empty() ? std::wstring(L"(nothing yet)") : rest)) +
            L",\"subtitle\":\"Search Menu Extensions demo. Press Enter to copy\",\"score\":1.0,"
            L"\"icon\":{\"glyph\":\"\\uE8D4\"},\"tags\":[{\"text\":\"Windhawk\"}],"
            L"\"action\":{\"type\":\"copy\",\"text\":" + JsonEscape(rest) + L"}},";
    json += L"{\"id\":\"toggle\",\"title\":\"Demo toggle\",\"subtitle\":\"Flips a value inside the library\","
            L"\"icon\":{\"glyph\":\"\\uE7E8\"},\"kinds\":[\"Toggle\",\"Command\"],"
            L"\"action\":{\"type\":\"toggle\",\"isToggled\":" + std::wstring(g_demoToggle ? L"true" : L"false") + L"}},";
    json += L"{\"id\":\"open\",\"title\":\"Open Windhawk\",\"subtitle\":\"Runs windhawk.exe\","
            L"\"icon\":{\"glyph\":\"\\uE8A7\"},"
            L"\"action\":{\"type\":\"run\",\"command\":\"C:\\\\Program Files\\\\Windhawk\\\\windhawk.exe\"}},";
    json += L"{\"id\":\"card\",\"title\":\"Demo card\",\"subtitle\":\"Adaptive Card content from a mod\","
            L"\"icon\":{\"glyph\":\"\\uE9E9\"},"
            L"\"card\":{\"template\":{\"type\":\"AdaptiveCard\",\"version\":\"1.5\",\"body\":["
            L"{\"type\":\"TextBlock\",\"text\":\"${title}\",\"weight\":\"Bolder\",\"size\":\"Large\"},"
            L"{\"type\":\"TextBlock\",\"text\":\"Clicked ${clicks} times\",\"wrap\":true}],"
            L"\"actions\":[{\"type\":\"Action.Submit\",\"id\":\"click\",\"title\":\"Click me\",\"data\":{\"x\":1}}]},"
            L"\"data\":{\"title\":\"Hello from Windhawk\",\"clicks\":" + std::to_wstring(g_demoClicks.load()) + L"}}}";
    json += L"]";
    SubmitResults(q, json, TRUE);
}

static void RegisterDemo() {
    ProviderCallbacks cb;
    cb.onQuery = [](UINT64 q, std::wstring const& text, std::wstring const&) { DemoQuery(q, text); };
    cb.onInvoke = [](std::wstring const& item, std::wstring const& action, std::wstring const& data, std::wstring const&) {
        if (item == L"toggle") g_demoToggle = (data == L"true");
        if (item == L"card") {
            int n = ++g_demoClicks;
            UpdateCard(g_demo, L"card", L"{\"title\":\"Hello from Windhawk\",\"clicks\":" + std::to_wstring(n) + L"}");
        }
        LOG(L"demo invoke %s/%s", item.c_str(), action.c_str());
    };
    std::wstring err;
    g_demo = RegisterProvider(
        L"{\"id\":\"demo\",\"name\":\"Windhawk demo\",\"description\":\"Search Menu Extensions sample items\",\"glyph\":\"\\uE8D4\"}",
        std::move(cb), nullptr, &err);
    if (!g_demo) LOG(L"demo registration failed: %s", err.c_str());
}

// ---------------------------------------------------------------------------
// Lifetime
// ---------------------------------------------------------------------------
static void LoadSettings() {
    g_settings.demoProvider = Wh_GetIntSetting(L"demoProvider") != 0;
    g_settings.pipeServer = Wh_GetIntSetting(L"pipeServer") != 0;
    g_settings.queryTimeoutMs = Wh_GetIntSetting(L"queryTimeoutMs");
    if (g_settings.queryTimeoutMs < 100 || g_settings.queryTimeoutMs > 60000) g_settings.queryTimeoutMs = 2500;
    g_settings.debugLog = Wh_GetIntSetting(L"debugLog") != 0;
    g_settings.restartIfLate = Wh_GetIntSetting(L"restartIfLate") != 0;
}

static bool IsComposerProcess() {
    const wchar_t* cmd = GetCommandLineW();
    return cmd && StrStrIW(cmd, L"SystemComposerControl");
}

static bool g_started;

static bool LibInit() {
    LoadSettings();
    if (!IsComposerProcess()) {
        DLOG(L"not the composer shellhost, idle");
        return true;
    }
    {
        using IncMta_t = HRESULT(WINAPI*)(void**);
        static void* cookie;
        if (auto f = (IncMta_t)GetProcAddress(GetModuleHandleW(L"combase.dll"), "CoIncrementMTAUsage")) f(&cookie);
    }  // so any thread can use WinRT objects
    bool lateLoad = GetModuleHandleW(L"RunUX.AOT.dll") != nullptr;
    (void)lateLoad;
    g_alive = true;
    if (!InstallCatalogHooks()) {
        g_alive = false;
        return true;  // stay loaded but inactive
    }
    g_started = true;
    // Patch the catalog instance vtables now in case Windows already opened its catalogs.
    std::thread([] {
        winrt::init_apartment(winrt::apartment_type::multi_threaded);
        winrt::com_ptr<Pal::IPaletteCatalog> cat;
        HRESULT hr = g_statics->OpenDefault(cat.put());
        LOG(L"background OpenDefault hr=%08lx", hr);
        if (g_settings.demoProvider) RegisterDemo();
        winrt::uninit_apartment();
    }).detach();
    if (g_settings.pipeServer) StartPipe();
    LOG(L"Search Menu Extensions running (pid %lu)", GetCurrentProcessId());
    // Enabled while search was already running: Windows has read its provider list and won't
    // re-read it, so restart the composer (it relaunches the next time search opens, with us in it).
    if (lateLoad && g_settings.restartIfLate) {
        std::thread([] {
            Sleep(1500);
            bool haveSubs;
            {
                std::lock_guard l(g_subsLock);
                haveSubs = !g_subs.empty();
            }
            if (!haveSubs && g_alive) {
                LOG(L"loaded after search started; restarting the composer so it picks up our providers");
                TerminateProcess(GetCurrentProcess(), 0);
            }
        }).detach();
    }
    return true;
}

static void LibUninit() {
    if (!g_started) return;
    StopPipe();
    std::vector<UINT64> hs;
    for (auto& p : ProvidersSnapshot()) hs.push_back(p->handle);
    for (auto h : hs) UnregisterProvider(h, true);
    g_alive = false;
    UnpatchAll();
    RaiseCatalogChanged();
    // Keep g_subs: if we're re-initialised in this process we can refresh search without a restart.
    g_started = false;
    LOG(L"Search Menu Extensions stopped");
}

BOOL Wh_ModInit() {
    LOG(L"Init");
    return LibInit();
}
void Wh_ModUninit() { LibUninit(); }
void Wh_ModSettingsChanged() {
    bool oldDemo = g_settings.demoProvider;
    LoadSettings();
    if (!g_started) return;
    if (g_settings.demoProvider && !g_demo) RegisterDemo();
    if (!g_settings.demoProvider && oldDemo && g_demo) {
        UnregisterProvider(g_demo);
        g_demo = 0;
    }
}
