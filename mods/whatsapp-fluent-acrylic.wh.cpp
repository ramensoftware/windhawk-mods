// ==WindhawkMod==
// @id              whatsapp-fluent-acrylic
// @name            WhatsApp Fluent Acrylic
// @description     Transparent Fluent acrylic/mica background for the WhatsApp desktop app
// @version         1.1.3
// @author          Matrakalero
// @github          https://github.com/Matrakalero
// @include         WhatsApp.Root.exe
// @architecture    x86-64
// @compilerOptions -ldwmapi -lole32 -loleaut32 -lruntimeobject
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# WhatsApp Fluent Acrylic

![WhatsApp Fluent Acrylic](https://i.imgur.com/5kx1mTA.png)

*Wallpaper in the screenshot:
[jellyfish wallpaper for Wallpaper Engine](https://steamcommunity.com/sharedfiles/filedetails/?id=3815670106).*

Gives the WhatsApp desktop app (Microsoft Store version) a translucent Windows 11
Fluent background: acrylic, mica or mica alt, with the chat panels see-through
and menus blurring the content behind them.

The current WhatsApp for Windows is WhatsApp Web inside a WebView2 hosted in a
WinUI 3 window, so generic transparency mods can't reach it: several layers paint
an opaque background on top of the window backdrop. This mod makes each of them
transparent and applies the backdrop to the WinUI window itself.

## Usage
1. Enable the mod.
2. Fully close WhatsApp (right-click its tray icon, **Exit**) and open it again.

Changing settings also requires restarting WhatsApp this way.

## Settings
- **Backdrop**: acrylic, mica or mica alt.
- **Panel tint / opacity**: color and strength of the single tint layer over
  the backdrop. Leave the color empty to follow WhatsApp's light/dark theme.
- **Menu opacity / blur**: fill and blur radius of menus, dialogs, tooltips and the
  emoji/sticker panel.
- **Image viewer opacity**: fill of the image and sticker viewer, which also
  blurs the chat behind it.
- **Hide chat wallpaper**: hides the doodle pattern behind conversations.
- **Extra CSS**: custom CSS injected into WhatsApp, for further tweaks.

## Requirements
- Windows 11 22H2 or newer.
- *Settings > Personalization > Colors > Transparency effects* turned on.
- If another mod applies a backdrop to every window (e.g. Translucent Windows),
  exclude `WhatsApp.Root.exe` there if you see artifacts.

## Credits
The XAML diagnostics part is adapted from m417z's Windows 11 styler mods.

Built for WhatsApp 2.26xx (WinAppSDK 2.x). A future WhatsApp update may change
its internals and require an update of this mod.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- backdrop: acrylic
  $name: Backdrop
  $options:
  - acrylic: Acrylic
  - mica: Mica
  - mica_tabbed: Mica Alt
- panelTint: ""
  $name: Panel tint color
  $description: >-
    Color laid over the backdrop, e.g. #101820. Empty uses WhatsApp's own
    background color.
- panelOpacity: 35
  $name: Panel tint opacity (0-100)
  $description: Lower is more transparent.
- menuOpacity: 60
  $name: Menu opacity (0-100)
  $description: Fill of menus and dialogs.
- menuBlur: 28
  $name: Menu blur radius (px)
  $description: Blur of the WhatsApp content behind menus, panels and viewers.
- viewerOpacity: 75
  $name: Image viewer opacity (0-100)
  $description: Fill of the image and sticker viewer.
- hideWallpaper: true
  $name: Hide chat wallpaper
  $description: Hides the doodle pattern behind conversations.
- extraCss: ""
  $name: Extra CSS
  $description: Additional CSS injected into WhatsApp.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <dwmapi.h>
#include <ocidl.h>
#include <unknwn.h>
#include <winstring.h>
#include <xamlom.h>

#undef GetCurrentTime

#include <winrt/base.h>
#include <winrt/Microsoft.UI.Composition.SystemBackdrops.h>
#include <winrt/Microsoft.UI.Xaml.Controls.h>
#include <winrt/Microsoft.UI.Xaml.Media.h>
#include <winrt/Microsoft.UI.Xaml.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.UI.h>

#include <atomic>
#include <map>
#include <mutex>
#include <string>
#include <vector>

namespace wf = winrt::Windows::Foundation;
namespace mux = winrt::Microsoft::UI::Xaml;

struct {
    int backdrop;
    std::wstring panelTint;
    int panelOpacity;
    int menuOpacity;
    int menuBlur;
    int viewerOpacity;
    bool hideWallpaper;
    std::wstring extraCss;
} g_settings;

std::wstring g_script;

struct ARGB {
    BYTE A, R, G, B;
};

constexpr ARGB kTransparent = {0, 0, 0, 0};

template <typename T>
T VtblFn(void* obj, int slot) {
    return reinterpret_cast<T>((*reinterpret_cast<void***>(obj))[slot]);
}

struct VtblHooks {
    std::mutex lock;
    std::map<void**, void*> originals;
    int patchedSlot = -1;

    void* Get(void* obj) {
        std::lock_guard<std::mutex> guard(lock);
        auto it = originals.find(*reinterpret_cast<void***>(obj));
        return it == originals.end() ? nullptr : it->second;
    }

    void Patch(void* obj, int slot, void* hook) {
        void** vtbl = *reinterpret_cast<void***>(obj);
        std::lock_guard<std::mutex> guard(lock);
        if (vtbl[slot] == hook || originals.count(vtbl)) {
            return;
        }
        DWORD oldProtect;
        if (!VirtualProtect(&vtbl[slot], sizeof(void*), PAGE_READWRITE,
                            &oldProtect)) {
            return;
        }
        originals[vtbl] = vtbl[slot];
        patchedSlot = slot;
        vtbl[slot] = hook;
        VirtualProtect(&vtbl[slot], sizeof(void*), oldProtect, &oldProtect);
    }

    void RestoreAll() {
        std::lock_guard<std::mutex> guard(lock);
        for (auto& [vtbl, original] : originals) {
            DWORD oldProtect;
            if (VirtualProtect(&vtbl[patchedSlot], sizeof(void*),
                               PAGE_READWRITE, &oldProtect)) {
                vtbl[patchedSlot] = original;
                VirtualProtect(&vtbl[patchedSlot], sizeof(void*), oldProtect,
                               &oldProtect);
            }
        }
        originals.clear();
    }
};

struct ICompletedHandler : IUnknown {
    virtual HRESULT STDMETHODCALLTYPE Invoke(HRESULT hr, void* result) = 0;
};

static const GUID IID_IMarshal_ = {
    0x00000003, 0x0000, 0x0000, {0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46}};

class HandlerWrapper final : public ICompletedHandler {
   public:
    using Callback = void (*)(HRESULT hr, void* result);

    HandlerWrapper(ICompletedHandler* inner, Callback cb)
        : m_inner(inner), m_cb(cb) {
        if (m_inner) {
            m_inner->AddRef();
        }
    }

    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid,
                                             void** ppv) override {
        if (!ppv) {
            return E_POINTER;
        }
        *ppv = nullptr;
        if (riid == __uuidof(IUnknown)) {
            *ppv = static_cast<IUnknown*>(this);
            AddRef();
            return S_OK;
        }
        if (riid == IID_IMarshal_ || !m_inner) {
            return E_NOINTERFACE;
        }
        void* tmp = nullptr;
        HRESULT hr = m_inner->QueryInterface(riid, &tmp);
        if (FAILED(hr)) {
            return hr;
        }
        static_cast<IUnknown*>(tmp)->Release();
        *ppv = static_cast<ICompletedHandler*>(this);
        AddRef();
        return S_OK;
    }

    ULONG STDMETHODCALLTYPE AddRef() override { return ++m_refs; }

    ULONG STDMETHODCALLTYPE Release() override {
        ULONG refs = --m_refs;
        if (refs == 0) {
            delete this;
        }
        return refs;
    }

    HRESULT STDMETHODCALLTYPE Invoke(HRESULT hr, void* result) override {
        if (m_cb) {
            m_cb(hr, result);
        }
        return m_inner ? m_inner->Invoke(hr, result) : S_OK;
    }

   private:
    ~HandlerWrapper() {
        if (m_inner) {
            m_inner->Release();
        }
    }

    std::atomic<ULONG> m_refs{1};
    ICompletedHandler* m_inner;
    Callback m_cb;
};

static const GUID IID_ICoreWebView2Environment3 = {
    0x80a22ae3, 0xbe7c, 0x4ce2, {0xaf, 0xe1, 0x5a, 0x50, 0x05, 0x6c, 0xde, 0xeb}};
static const GUID IID_ICoreWebView2Environment10 = {
    0xee0eb9df, 0x6f12, 0x46ce, {0xb5, 0x3f, 0x3f, 0x47, 0xb9, 0xc9, 0x28, 0xe0}};
static const GUID IID_ICoreWebView2Controller = {
    0x4d00c0d1, 0x9434, 0x4eb6, {0x80, 0x78, 0x86, 0x97, 0xa5, 0x60, 0x33, 0x4f}};
static const GUID IID_ICoreWebView2Controller2 = {
    0xc979903e, 0xd4ca, 0x4228, {0x92, 0xeb, 0x47, 0xee, 0x3f, 0xa9, 0x6e, 0xab}};
static const GUID IID_ICoreWebView2Controller3 = {
    0xf9614724, 0x5d2b, 0x41dc, {0xae, 0xf7, 0x73, 0xd6, 0x2b, 0x51, 0x54, 0x3b}};
static const GUID IID_ICoreWebView2Controller4 = {
    0x97d418d5, 0xa426, 0x4e49, {0xa1, 0x51, 0xe1, 0xa1, 0x0f, 0x32, 0x7d, 0x9e}};

enum {
    kEnv_CreateCoreWebView2Controller = 3,
    kEnv3_CreateCoreWebView2CompositionController = 9,
    kEnv10_CreateCoreWebView2ControllerWithOptions = 21,
    kEnv10_CreateCoreWebView2CompositionControllerWithOptions = 22,
    kController_get_CoreWebView2 = 25,
    kController2_get_DefaultBackgroundColor = 26,
    kController2_put_DefaultBackgroundColor = 27,
    kWebView_AddScriptToExecuteOnDocumentCreated = 27,
};

VtblHooks g_createController;
VtblHooks g_createCompositionController;
VtblHooks g_createControllerWithOptions;
VtblHooks g_createCompositionControllerWithOptions;
VtblHooks g_putDefaultBackgroundColor;

using PutColor_t = HRESULT(STDMETHODCALLTYPE*)(void* self, ARGB color);
using GetColor_t = HRESULT(STDMETHODCALLTYPE*)(void* self, ARGB* color);

HRESULT STDMETHODCALLTYPE PutDefaultBackgroundColor_Hook(void* self,
                                                         ARGB color) {
    auto original =
        reinterpret_cast<PutColor_t>(g_putDefaultBackgroundColor.Get(self));
    if (!original) {
        return E_FAIL;
    }
    color.A = 0;
    return original(self, color);
}

IUnknown* g_controller2;

void EnforceTransparentController() {
    if (!g_controller2) {
        return;
    }
    ARGB color{};
    VtblFn<GetColor_t>(g_controller2, kController2_get_DefaultBackgroundColor)(
        g_controller2, &color);
    if (color.A == 0) {
        return;
    }
    auto original = reinterpret_cast<PutColor_t>(
        g_putDefaultBackgroundColor.Get(g_controller2));
    if (!original) {
        original = VtblFn<PutColor_t>(g_controller2,
                                      kController2_put_DefaultBackgroundColor);
    }
    original(g_controller2, kTransparent);
}

void OnControllerCreated(HRESULT hr, void* result) {
    if (FAILED(hr) || !result) {
        return;
    }

    IUnknown* unk = static_cast<IUnknown*>(result);

    for (const GUID* iid :
         {&IID_ICoreWebView2Controller2, &IID_ICoreWebView2Controller3,
          &IID_ICoreWebView2Controller4}) {
        void* controller = nullptr;
        if (FAILED(unk->QueryInterface(*iid, &controller))) {
            continue;
        }
        g_putDefaultBackgroundColor.Patch(
            controller, kController2_put_DefaultBackgroundColor,
            reinterpret_cast<void*>(PutDefaultBackgroundColor_Hook));
        PutDefaultBackgroundColor_Hook(controller, kTransparent);
        if (!g_controller2) {
            g_controller2 = static_cast<IUnknown*>(controller);
        } else {
            static_cast<IUnknown*>(controller)->Release();
        }
    }

    void* controller = nullptr;
    if (FAILED(unk->QueryInterface(IID_ICoreWebView2Controller,
                                   &controller))) {
        return;
    }

    using GetCoreWebView2_t = HRESULT(STDMETHODCALLTYPE*)(void*, void**);
    void* webview = nullptr;
    VtblFn<GetCoreWebView2_t>(controller, kController_get_CoreWebView2)(
        controller, &webview);
    static_cast<IUnknown*>(controller)->Release();
    if (!webview) {
        return;
    }

    using AddScript_t =
        HRESULT(STDMETHODCALLTYPE*)(void*, LPCWSTR, ICompletedHandler*);
    auto* handler = new HandlerWrapper(nullptr, nullptr);
    hr = VtblFn<AddScript_t>(webview,
                             kWebView_AddScriptToExecuteOnDocumentCreated)(
        webview, g_script.c_str(), handler);
    handler->Release();
    static_cast<IUnknown*>(webview)->Release();
    if (FAILED(hr)) {
        Wh_Log(L"Script injection failed: %08X", hr);
    }
}

using CreateController_t = HRESULT(STDMETHODCALLTYPE*)(void* self,
                                                       HWND parent,
                                                       ICompletedHandler* h);
using CreateControllerWithOptions_t =
    HRESULT(STDMETHODCALLTYPE*)(void* self,
                                HWND parent,
                                IUnknown* options,
                                ICompletedHandler* h);

template <VtblHooks& hooks>
HRESULT STDMETHODCALLTYPE CreateController_Hook(void* self,
                                                HWND parent,
                                                ICompletedHandler* h) {
    auto original = reinterpret_cast<CreateController_t>(hooks.Get(self));
    if (!original) {
        return E_FAIL;
    }
    auto* wrapper = new HandlerWrapper(h, OnControllerCreated);
    HRESULT hr = original(self, parent, wrapper);
    wrapper->Release();
    return hr;
}

template <VtblHooks& hooks>
HRESULT STDMETHODCALLTYPE CreateControllerWithOptions_Hook(
    void* self,
    HWND parent,
    IUnknown* options,
    ICompletedHandler* h) {
    auto original =
        reinterpret_cast<CreateControllerWithOptions_t>(hooks.Get(self));
    if (!original) {
        return E_FAIL;
    }
    auto* wrapper = new HandlerWrapper(h, OnControllerCreated);
    HRESULT hr = original(self, parent, options, wrapper);
    wrapper->Release();
    return hr;
}

void OnEnvironmentCreated(HRESULT hr, void* result) {
    if (FAILED(hr) || !result) {
        return;
    }

    IUnknown* env = static_cast<IUnknown*>(result);

    g_createController.Patch(
        env, kEnv_CreateCoreWebView2Controller,
        reinterpret_cast<void*>(CreateController_Hook<g_createController>));

    void* env3 = nullptr;
    if (SUCCEEDED(env->QueryInterface(IID_ICoreWebView2Environment3, &env3))) {
        g_createController.Patch(
            env3, kEnv_CreateCoreWebView2Controller,
            reinterpret_cast<void*>(
                CreateController_Hook<g_createController>));
        g_createCompositionController.Patch(
            env3, kEnv3_CreateCoreWebView2CompositionController,
            reinterpret_cast<void*>(
                CreateController_Hook<g_createCompositionController>));
        static_cast<IUnknown*>(env3)->Release();
    }

    void* env10 = nullptr;
    if (SUCCEEDED(
            env->QueryInterface(IID_ICoreWebView2Environment10, &env10))) {
        g_createController.Patch(
            env10, kEnv_CreateCoreWebView2Controller,
            reinterpret_cast<void*>(
                CreateController_Hook<g_createController>));
        g_createCompositionController.Patch(
            env10, kEnv3_CreateCoreWebView2CompositionController,
            reinterpret_cast<void*>(
                CreateController_Hook<g_createCompositionController>));
        g_createControllerWithOptions.Patch(
            env10, kEnv10_CreateCoreWebView2ControllerWithOptions,
            reinterpret_cast<void*>(CreateControllerWithOptions_Hook<
                                    g_createControllerWithOptions>));
        g_createCompositionControllerWithOptions.Patch(
            env10, kEnv10_CreateCoreWebView2CompositionControllerWithOptions,
            reinterpret_cast<void*>(CreateControllerWithOptions_Hook<
                                    g_createCompositionControllerWithOptions>));
        static_cast<IUnknown*>(env10)->Release();
    }
}

using CreateWebViewEnvironment_t =
    HRESULT(STDAPICALLTYPE*)(bool checkRunningInstance,
                             int runtimeType,
                             PCWSTR userDataDir,
                             IUnknown* options,
                             ICompletedHandler* handler);
CreateWebViewEnvironment_t CreateWebViewEnvironment_Original;

HRESULT STDAPICALLTYPE CreateWebViewEnvironment_Hook(bool checkRunningInstance,
                                                     int runtimeType,
                                                     PCWSTR userDataDir,
                                                     IUnknown* options,
                                                     ICompletedHandler* handler) {
    auto* wrapper = new HandlerWrapper(handler, OnEnvironmentCreated);
    HRESULT hr = CreateWebViewEnvironment_Original(
        checkRunningInstance, runtimeType, userDataDir, options, wrapper);
    wrapper->Release();
    return hr;
}

using DllGetActivationFactory_t = HRESULT(WINAPI*)(HSTRING classId,
                                                   void** factory);

VtblHooks g_windowFactoryCreateInstance;
mux::Window g_window{nullptr};
mux::Media::SystemBackdrop g_backdrop{nullptr};
std::atomic<bool> g_backdropApplied;

enum { kWindowFactory_CreateInstance = 6 };

using WindowCreateInstance_t = HRESULT(STDMETHODCALLTYPE*)(void* self,
                                                           void* baseInterface,
                                                           void** inner,
                                                           void** instance);

HRESULT STDMETHODCALLTYPE WindowCreateInstance_Hook(void* self,
                                                    void* baseInterface,
                                                    void** inner,
                                                    void** instance) {
    auto original = reinterpret_cast<WindowCreateInstance_t>(
        g_windowFactoryCreateInstance.Get(self));
    if (!original) {
        return E_FAIL;
    }
    HRESULT hr = original(self, baseInterface, inner, instance);
    if (SUCCEEDED(hr) && instance && *instance && !g_window) {
        winrt::copy_from_abi(g_window, *instance);
    }
    return hr;
}

DllGetActivationFactory_t XamlDllGetActivationFactory_Original;

HRESULT WINAPI XamlDllGetActivationFactory_Hook(HSTRING classId,
                                                void** factory) {
    HRESULT hr = XamlDllGetActivationFactory_Original(classId, factory);
    if (FAILED(hr) || !factory || !*factory) {
        return hr;
    }
    PCWSTR name = WindowsGetStringRawBuffer(classId, nullptr);
    if (!name || wcscmp(name, L"Microsoft.UI.Xaml.Window") != 0) {
        return hr;
    }
    void* windowFactory = nullptr;
    if (SUCCEEDED(static_cast<IUnknown*>(*factory)->QueryInterface(
            winrt::guid_of<mux::IWindowFactory>(), &windowFactory))) {
        g_windowFactoryCreateInstance.Patch(
            windowFactory, kWindowFactory_CreateInstance,
            reinterpret_cast<void*>(WindowCreateInstance_Hook));
        static_cast<IUnknown*>(windowFactory)->Release();
    }
    return hr;
}

static const GUID IID_IContentExternalOutputLinkStatics = {
    0xb758f401, 0x833e, 0x587d, {0xb0, 0xcd, 0xa3, 0x93, 0x4e, 0xba, 0x37, 0x21}};
static const GUID IID_IContentExternalOutputLink = {
    0x3dac8ec8, 0x011f, 0x5ad2, {0x8d, 0xb7, 0xb7, 0x3c, 0x44, 0x52, 0xf7, 0x55}};

enum {
    kOutputLinkStatics_Create = 6,
    kOutputLink_put_BackgroundColor = 7,
};

VtblHooks g_outputLinkCreate;
VtblHooks g_outputLinkPutBackgroundColor;

HRESULT STDMETHODCALLTYPE OutputLinkPutBackgroundColor_Hook(void* self,
                                                            ARGB) {
    auto original =
        reinterpret_cast<PutColor_t>(g_outputLinkPutBackgroundColor.Get(self));
    return original ? original(self, kTransparent) : E_FAIL;
}

using OutputLinkCreate_t = HRESULT(STDMETHODCALLTYPE*)(void* self,
                                                       void* compositor,
                                                       void** result);

HRESULT STDMETHODCALLTYPE OutputLinkCreate_Hook(void* self,
                                                void* compositor,
                                                void** result) {
    auto original =
        reinterpret_cast<OutputLinkCreate_t>(g_outputLinkCreate.Get(self));
    if (!original) {
        return E_FAIL;
    }
    HRESULT hr = original(self, compositor, result);
    if (SUCCEEDED(hr) && result && *result) {
        void* link = nullptr;
        if (SUCCEEDED(static_cast<IUnknown*>(*result)->QueryInterface(
                IID_IContentExternalOutputLink, &link))) {
            g_outputLinkPutBackgroundColor.Patch(
                link, kOutputLink_put_BackgroundColor,
                reinterpret_cast<void*>(OutputLinkPutBackgroundColor_Hook));
            OutputLinkPutBackgroundColor_Hook(link, kTransparent);
            static_cast<IUnknown*>(link)->Release();
        }
    }
    return hr;
}

DllGetActivationFactory_t InputDllGetActivationFactory_Original;

HRESULT WINAPI InputDllGetActivationFactory_Hook(HSTRING classId,
                                                 void** factory) {
    HRESULT hr = InputDllGetActivationFactory_Original(classId, factory);
    if (FAILED(hr) || !factory || !*factory) {
        return hr;
    }
    PCWSTR name = WindowsGetStringRawBuffer(classId, nullptr);
    if (!name ||
        wcscmp(name, L"Microsoft.UI.Content.ContentExternalOutputLink") != 0) {
        return hr;
    }
    void* statics = nullptr;
    if (SUCCEEDED(static_cast<IUnknown*>(*factory)->QueryInterface(
            IID_IContentExternalOutputLinkStatics, &statics))) {
        g_outputLinkCreate.Patch(statics, kOutputLinkStatics_Create,
                                 reinterpret_cast<void*>(OutputLinkCreate_Hook));
        static_cast<IUnknown*>(statics)->Release();
    }
    return hr;
}

std::atomic<bool> g_webViewDllHooked;
std::atomic<bool> g_xamlDllHooked;
std::atomic<bool> g_inputDllHooked;

void HookExport(std::atomic<bool>& done,
                HMODULE module,
                PCSTR exportName,
                void* hook,
                void** original,
                bool applyNow) {
    if (done.exchange(true)) {
        return;
    }
    void* target = (void*)GetProcAddress(module, exportName);
    if (!target) {
        Wh_Log(L"Export not found: %S", exportName);
        return;
    }
    Wh_SetFunctionHook(target, hook, original);
    if (applyNow) {
        Wh_ApplyHookOperations();
    }
}

void HookModules(bool applyNow) {
    if (HMODULE module = GetModuleHandleW(L"EmbeddedBrowserWebView.dll")) {
        HookExport(g_webViewDllHooked, module,
                   "CreateWebViewEnvironmentWithOptionsInternal",
                   (void*)CreateWebViewEnvironment_Hook,
                   (void**)&CreateWebViewEnvironment_Original, applyNow);
    }
    if (HMODULE module = GetModuleHandleW(L"Microsoft.UI.Xaml.dll")) {
        HookExport(g_xamlDllHooked, module, "DllGetActivationFactory",
                   (void*)XamlDllGetActivationFactory_Hook,
                   (void**)&XamlDllGetActivationFactory_Original, applyNow);
    }
    if (HMODULE module = GetModuleHandleW(L"Microsoft.UI.Input.dll")) {
        HookExport(g_inputDllHooked, module, "DllGetActivationFactory",
                   (void*)InputDllGetActivationFactory_Hook,
                   (void**)&InputDllGetActivationFactory_Original, applyNow);
    }
}

using LoadLibraryExW_t = decltype(&LoadLibraryExW);
LoadLibraryExW_t LoadLibraryExW_Original;

HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR name, HANDLE file, DWORD flags) {
    HMODULE module = LoadLibraryExW_Original(name, file, flags);
    if (module &&
        !(g_webViewDllHooked && g_xamlDllHooked && g_inputDllHooked)) {
        HookModules(true);
    }
    return module;
}

struct ClearedBackground {
    winrt::weak_ref<mux::DependencyObject> element;
    mux::DependencyProperty property{nullptr};
    wf::IInspectable original;
};
std::vector<ClearedBackground> g_clearedBackgrounds;
winrt::weak_ref<mux::Controls::WebView2> g_webView;
winrt::Windows::UI::Color g_webViewOriginalColor;
int64_t g_webViewColorToken;

mux::Media::SystemBackdrop CreateBackdrop() {
    if (g_settings.backdrop == DWMSBT_MAINWINDOW) {
        return mux::Media::MicaBackdrop();
    }
    if (g_settings.backdrop == DWMSBT_TABBEDWINDOW) {
        mux::Media::MicaBackdrop mica;
        mica.Kind(
            winrt::Microsoft::UI::Composition::SystemBackdrops::MicaKind::BaseAlt);
        return mica;
    }
    return mux::Media::DesktopAcrylicBackdrop();
}

void ApplyWindowBackdrop() {
    if (!g_window) {
        return;
    }
    try {
        if (!g_backdrop) {
            g_backdrop = CreateBackdrop();
        }
        if (g_window.SystemBackdrop() != g_backdrop) {
            g_window.SystemBackdrop(g_backdrop);
            g_backdropApplied = true;
        }
    } catch (...) {
        Wh_Log(L"SystemBackdrop error %08X", winrt::to_hresult());
    }
}

void ClearBackground(mux::DependencyObject const& obj,
                     mux::DependencyProperty const& property) {
    auto value = obj.GetValue(property);
    auto brush = value.try_as<mux::Media::SolidColorBrush>();
    if (!value || (brush && brush.Color().A == 0)) {
        return;
    }
    g_clearedBackgrounds.push_back({winrt::make_weak(obj), property, value});
    obj.SetValue(property, mux::Media::SolidColorBrush(
                               winrt::Windows::UI::Color{0, 0, 0, 0}));
}

void ClearWebViewAncestors(mux::DependencyObject const& element) {
    for (auto obj = element; obj;
         obj = mux::Media::VisualTreeHelper::GetParent(obj)) {
        if (obj.try_as<mux::Controls::Panel>()) {
            ClearBackground(obj, mux::Controls::Panel::BackgroundProperty());
        } else if (obj.try_as<mux::Controls::Border>()) {
            ClearBackground(obj, mux::Controls::Border::BackgroundProperty());
        } else if (obj.try_as<mux::Controls::ContentPresenter>()) {
            ClearBackground(
                obj, mux::Controls::ContentPresenter::BackgroundProperty());
        } else if (obj.try_as<mux::Controls::Control>()) {
            ClearBackground(obj, mux::Controls::Control::BackgroundProperty());
        }
    }
}

void RefreshWebView() {
    if (auto webView = g_webView.get()) {
        try {
            ClearWebViewAncestors(webView);
        } catch (...) {
        }
    }
}

void OnXamlElementAdded(wf::IInspectable const& inspectable, PCWSTR type) {
    if (wcscmp(type, L"Microsoft.UI.Xaml.Controls.WebView2") != 0) {
        return;
    }
    auto webView = inspectable.as<mux::Controls::WebView2>();
    ClearWebViewAncestors(webView);

    g_webViewOriginalColor = webView.DefaultBackgroundColor();
    g_webView = winrt::make_weak(webView);
    webView.DefaultBackgroundColor(winrt::Windows::UI::Color{0, 0, 0, 0});
    g_webViewColorToken = webView.RegisterPropertyChangedCallback(
        mux::Controls::WebView2::DefaultBackgroundColorProperty(),
        [](mux::DependencyObject const& sender, auto&&) {
            auto wv = sender.as<mux::Controls::WebView2>();
            if (wv.DefaultBackgroundColor().A != 0) {
                wv.DefaultBackgroundColor(
                    winrt::Windows::UI::Color{0, 0, 0, 0});
            }
        });
}

void RestoreXaml() {
    for (auto& cleared : g_clearedBackgrounds) {
        if (auto obj = cleared.element.get()) {
            obj.SetValue(cleared.property, cleared.original);
        }
    }
    g_clearedBackgrounds.clear();
    if (auto webView = g_webView.get()) {
        webView.UnregisterPropertyChangedCallback(
            mux::Controls::WebView2::DefaultBackgroundColorProperty(),
            g_webViewColorToken);
        webView.DefaultBackgroundColor(g_webViewOriginalColor);
    }
    g_webView = nullptr;
    if (g_window && g_backdropApplied) {
        g_window.SystemBackdrop(nullptr);
    }
    g_backdropApplied = false;
    g_backdrop = nullptr;
    g_window = nullptr;
}

HMODULE GetCurrentModuleHandle() {
    HMODULE module;
    if (!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                                GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                            reinterpret_cast<LPCWSTR>(&GetCurrentModuleHandle),
                            &module)) {
        return nullptr;
    }
    return module;
}

class VisualTreeWatcher
    : public winrt::implements<VisualTreeWatcher,
                               IVisualTreeServiceCallback2,
                               winrt::non_agile> {
   public:
    explicit VisualTreeWatcher(winrt::com_ptr<IUnknown> site)
        : m_diagnostics(site.as<IXamlDiagnostics>()) {
        AddRef();
        HANDLE thread =
            CreateThread(nullptr, 0, AdviseThread, this, 0, nullptr);
        if (thread) {
            CloseHandle(thread);
        } else {
            Release();
        }
    }

    void Unadvise() {
        m_diagnostics.as<IVisualTreeService3>()->UnadviseVisualTreeChange(
            this);
    }

   private:
    static DWORD WINAPI AdviseThread(LPVOID param) {
        auto self = static_cast<VisualTreeWatcher*>(param);
        self->m_diagnostics.as<IVisualTreeService3>()->AdviseVisualTreeChange(
            self);
        self->Release();
        return 0;
    }

    HRESULT STDMETHODCALLTYPE
    OnVisualTreeChange(ParentChildRelation,
                       VisualElement element,
                       VisualMutationType mutationType) override try {
        if (mutationType != Add) {
            return S_OK;
        }
        wf::IInspectable inspectable;
        winrt::check_hresult(m_diagnostics->GetIInspectableFromHandle(
            element.Handle,
            reinterpret_cast<::IInspectable**>(winrt::put_abi(inspectable))));
        OnXamlElementAdded(inspectable, element.Type);
        return S_OK;
    } catch (...) {
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE OnElementStateChanged(InstanceHandle,
                                                    VisualElementState,
                                                    LPCWSTR) noexcept override {
        return S_OK;
    }

    winrt::com_ptr<IXamlDiagnostics> m_diagnostics;
};

winrt::com_ptr<VisualTreeWatcher> g_visualTreeWatcher;

constexpr CLSID CLSID_WhatsAppAcrylicTAP = {
    0x6e1b5c9a, 0x3f2d, 0x4b7e, {0x9a, 0x41, 0x0c, 0x8d, 0x2e, 0x7f, 0x5b, 0x13}};

class WhatsAppAcrylicTAP
    : public winrt::implements<WhatsAppAcrylicTAP,
                               IObjectWithSite,
                               winrt::non_agile> {
   public:
    HRESULT STDMETHODCALLTYPE SetSite(IUnknown* site) override try {
        if (g_visualTreeWatcher) {
            g_visualTreeWatcher->Unadvise();
            g_visualTreeWatcher = nullptr;
        }
        m_site.copy_from(site);
        if (m_site) {
            FreeLibrary(GetCurrentModuleHandle());
            g_visualTreeWatcher = winrt::make_self<VisualTreeWatcher>(m_site);
        }
        return S_OK;
    } catch (...) {
        return winrt::to_hresult();
    }

    HRESULT STDMETHODCALLTYPE GetSite(REFIID riid,
                                      void** ppv) noexcept override {
        return m_site.as(riid, ppv);
    }

   private:
    winrt::com_ptr<IUnknown> m_site;
};

struct TapFactory
    : winrt::implements<TapFactory, IClassFactory, winrt::non_agile> {
    HRESULT STDMETHODCALLTYPE CreateInstance(IUnknown* outer,
                                             REFIID riid,
                                             void** ppv) override try {
        *ppv = nullptr;
        if (outer) {
            return CLASS_E_NOAGGREGATION;
        }
        return winrt::make<WhatsAppAcrylicTAP>().as(riid, ppv);
    } catch (...) {
        return winrt::to_hresult();
    }

    HRESULT STDMETHODCALLTYPE LockServer(BOOL) noexcept override {
        return S_OK;
    }
};

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdll-attribute-on-redeclaration"

__declspec(dllexport) STDAPI DllGetClassObject(REFCLSID rclsid,
                                               REFIID riid,
                                               LPVOID* ppv) try {
    if (rclsid != CLSID_WhatsAppAcrylicTAP) {
        return CLASS_E_CLASSNOTAVAILABLE;
    }
    *ppv = nullptr;
    return winrt::make<TapFactory>().as(riid, ppv);
} catch (...) {
    return winrt::to_hresult();
}

__declspec(dllexport) STDAPI DllCanUnloadNow() {
    return winrt::get_module_lock() ? S_FALSE : S_OK;
}

#pragma clang diagnostic pop

void InjectTap() {
    WCHAR location[MAX_PATH];
    DWORD len = GetModuleFileNameW(GetCurrentModuleHandle(), location,
                                   ARRAYSIZE(location));
    if (len == 0 || len == ARRAYSIZE(location)) {
        return;
    }

    HMODULE udk = GetModuleHandleW(L"Microsoft.Internal.FrameworkUdk.dll");
    auto ixde = udk ? reinterpret_cast<decltype(&InitializeXamlDiagnosticsEx)>(
                          GetProcAddress(udk, "InitializeXamlDiagnosticsEx"))
                    : nullptr;
    if (!ixde) {
        Wh_Log(L"InitializeXamlDiagnosticsEx not available");
        return;
    }

    HRESULT hr = E_FAIL;
    for (int i = 1; i <= 10000; i++) {
        WCHAR endpoint[64];
        swprintf_s(endpoint, L"WinUIVisualDiagConnection%d", i);
        hr = ixde(endpoint, GetCurrentProcessId(), L"", location,
                  CLSID_WhatsAppAcrylicTAP, nullptr);
        if (hr != HRESULT_FROM_WIN32(ERROR_NOT_FOUND)) {
            break;
        }
    }
    if (FAILED(hr)) {
        Wh_Log(L"InitializeXamlDiagnosticsEx: %08X", hr);
    }
}

using RunFromWindowThreadProc_t = void (*)(PVOID param);

const UINT g_runFromWindowThreadMsg = RegisterWindowMessageW(
    L"Windhawk_RunFromWindowThread_whatsapp-fluent-acrylic");

struct RunFromWindowThreadCall {
    RunFromWindowThreadProc_t proc;
    PVOID param;
};

LRESULT CALLBACK RunFromWindowThreadHookProc(int code,
                                             WPARAM wParam,
                                             LPARAM lParam) {
    if (code == HC_ACTION) {
        auto cwp = reinterpret_cast<const CWPSTRUCT*>(lParam);
        if (cwp->message == g_runFromWindowThreadMsg) {
            auto call = reinterpret_cast<RunFromWindowThreadCall*>(cwp->lParam);
            call->proc(call->param);
        }
    }
    return CallNextHookEx(nullptr, code, wParam, lParam);
}

bool RunFromWindowThread(HWND hwnd, RunFromWindowThreadProc_t proc,
                         PVOID param) {
    DWORD threadId = GetWindowThreadProcessId(hwnd, nullptr);
    if (!threadId) {
        return false;
    }
    if (threadId == GetCurrentThreadId()) {
        proc(param);
        return true;
    }

    HHOOK hook = SetWindowsHookExW(WH_CALLWNDPROC, RunFromWindowThreadHookProc,
                                   nullptr, threadId);
    if (!hook) {
        return false;
    }
    RunFromWindowThreadCall call{proc, param};
    SendMessageW(hwnd, g_runFromWindowThreadMsg, 0,
                 reinterpret_cast<LPARAM>(&call));
    UnhookWindowsHookEx(hook);
    return true;
}

constexpr wchar_t kAppliedProp[] = L"WhFluentAcrylicApplied";

std::atomic<bool> g_tapInjected;
HWND g_mainHwnd;

bool IsWhatsAppWindow(HWND hwnd) {
    wchar_t cls[64];
    return GetAncestor(hwnd, GA_ROOT) == hwnd &&
           GetClassNameW(hwnd, cls, ARRAYSIZE(cls)) &&
           wcscmp(cls, L"WinUIDesktopWin32WindowClass") == 0;
}

using DwmSetWindowAttribute_t = decltype(&DwmSetWindowAttribute);
DwmSetWindowAttribute_t DwmSetWindowAttribute_Original;
HRESULT WINAPI DwmSetWindowAttribute_Hook(HWND hwnd,
                                          DWORD attribute,
                                          LPCVOID value,
                                          DWORD size) {
    if (attribute == DWMWA_SYSTEMBACKDROP_TYPE && IsWhatsAppWindow(hwnd)) {
        int type = g_settings.backdrop;
        return DwmSetWindowAttribute_Original(hwnd, attribute, &type,
                                              sizeof(type));
    }
    return DwmSetWindowAttribute_Original(hwnd, attribute, value, size);
}

using DwmExtendFrameIntoClientArea_t = decltype(&DwmExtendFrameIntoClientArea);
DwmExtendFrameIntoClientArea_t DwmExtendFrameIntoClientArea_Original;
HRESULT WINAPI DwmExtendFrameIntoClientArea_Hook(HWND hwnd,
                                                 const MARGINS* margins) {
    if (IsWhatsAppWindow(hwnd)) {
        MARGINS full = {-1, -1, -1, -1};
        return DwmExtendFrameIntoClientArea_Original(hwnd, &full);
    }
    return DwmExtendFrameIntoClientArea_Original(hwnd, margins);
}

bool IsAppsDarkMode() {
    DWORD value = 1, size = sizeof(value);
    RegGetValueW(
        HKEY_CURRENT_USER,
        L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
        L"AppsUseLightTheme", RRF_RT_REG_DWORD, nullptr, &value, &size);
    return value == 0;
}

void ApplyDwmBackdrop(HWND hwnd) {
    BOOL dark = IsAppsDarkMode();
    DwmSetWindowAttribute_Original(hwnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &dark,
                                   sizeof(dark));
    MARGINS margins = {-1, -1, -1, -1};
    DwmExtendFrameIntoClientArea_Original(hwnd, &margins);
    int type = g_settings.backdrop;
    DwmSetWindowAttribute_Original(hwnd, DWMWA_SYSTEMBACKDROP_TYPE, &type,
                                   sizeof(type));
}

BOOL CALLBACK WindowEnumProc(HWND hwnd, LPARAM restore) {
    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    if (pid != GetCurrentProcessId() || !IsWhatsAppWindow(hwnd)) {
        return TRUE;
    }
    if (restore) {
        if (RemovePropW(hwnd, kAppliedProp)) {
            int type = DWMSBT_AUTO;
            DwmSetWindowAttribute_Original(hwnd, DWMWA_SYSTEMBACKDROP_TYPE,
                                           &type, sizeof(type));
            MARGINS margins = {};
            DwmExtendFrameIntoClientArea_Original(hwnd, &margins);
        }
    } else if (IsWindowVisible(hwnd)) {
        if (!GetPropW(hwnd, kAppliedProp)) {
            ApplyDwmBackdrop(hwnd);
            SetPropW(hwnd, kAppliedProp, (HANDLE)1);
        }
        if (!g_tapInjected.exchange(true)) {
            g_mainHwnd = hwnd;
            RunFromWindowThread(hwnd, [](PVOID) { InjectTap(); }, nullptr);
        }
    }
    return TRUE;
}

HANDLE g_stopEvent;
HANDLE g_windowThread;

DWORD WINAPI WindowThread(LPVOID) {
    do {
        EnumWindows(WindowEnumProc, FALSE);
        if (g_mainHwnd && IsWindow(g_mainHwnd)) {
            RunFromWindowThread(
                g_mainHwnd,
                [](PVOID) {
                    EnforceTransparentController();
                    ApplyWindowBackdrop();
                    RefreshWebView();
                },
                nullptr);
        }
    } while (WaitForSingleObject(g_stopEvent, 1000) == WAIT_TIMEOUT);
    return 0;
}

const wchar_t kScriptTemplate[] = LR"JS(
(() => {
  if (window.top !== window || window.__whFluentAcrylic) return;
  window.__whFluentAcrylic = true;

  const TINT = __TINT__;
  const ALPHA = __ALPHA__;
  const MENU_ALPHA = __MENU_ALPHA__;
  const MENU_BLUR = __MENU_BLUR__;
  const VIEWER_ALPHA = __VIEWER_ALPHA__;
  const HIDE_WALLPAPER = __HIDE_WALLPAPER__;
  const EXTRA_CSS = __EXTRA_CSS__;

  const NAME_RE = /(background|panel|surface|wallpaper|chat-?list|sidebar|drawer|app-?bg|-bg\b|bg-)/i;
  const SKIP_RE = /(bubble|outgoing|incoming|button|badge|unread|icon|text|border|shadow|accent|hover|active|pressed|focus|selected|highlight|overlay|modal|popup|popover|menu|dropdown|tooltip|toast|danger|warning|success|teal|green|avatar|profile-photo|input|search)/i;

  const BASE_CSS = `
    html, body, #app, #app > div, #app > div > div {
      background: transparent !important;
      background-color: transparent !important;
    }
    html body #main#main,
    html body [data-testid="conversation-panel-wrapper"] {
      background-color: transparent !important;
    }
    [role="menu"], [role="listbox"], [role="dialog"], [role="application"],
    [role="tooltip"] {
      background-color: rgba(var(--WDS-surface-elevated-default-RGB, 29, 31, 31), ${MENU_ALPHA}) !important;
      backdrop-filter: blur(${MENU_BLUR}px) saturate(1.4) !important;
    }
    [role="menu"], [role="application"] { border-radius: 16px !important; }
    [role="tooltip"] * { color: var(--WDS-content-default, #fafafa) !important; }
    #app .two:has(> * > [data-testid="drawer-left"] [data-testid$="drawer"]) > * > div:has(> #side),
    #app .two:has(> * > [data-testid="drawer-middle"] [data-testid$="drawer"]) > * > div:has(> #main) {
      opacity: 0 !important;
      pointer-events: none !important;
    }
    [data-testid="media-viewer-modal"],
    div:has(> * > * > * > [data-testid="status-player-uie"]) {
      background-color: rgba(var(--WDS-surface-default-RGB, 22, 23, 23), ${VIEWER_ALPHA}) !important;
      backdrop-filter: blur(${MENU_BLUR}px) saturate(1.4) !important;
    }
    ${HIDE_WALLPAPER ? `
    [data-asset-chat-background-dark], [data-asset-chat-background-light],
    [data-asset-chat-background], [data-asset-intro-image-dark],
    [data-asset-intro-image-light],
    [data-testid^="conversation-background"] { opacity: 0 !important; }` : ''}
  `;

  let baseStyle, varStyle, probe, timer, updating = false;

  const ensureNodes = () => {
    const parent = document.head || document.documentElement;
    if (!parent) return false;
    if (!baseStyle) {
      baseStyle = document.createElement('style');
      baseStyle.textContent = BASE_CSS + '\n' + EXTRA_CSS;
      varStyle = document.createElement('style');
    }
    if (parent.lastElementChild !== varStyle) {
      parent.appendChild(baseStyle);
      parent.appendChild(varStyle);
    }
    return true;
  };

  const parseColor = (value) => {
    if (!document.body) return null;
    if (!probe) {
      probe = document.createElement('div');
      probe.style.display = 'none';
    }
    if (!probe.isConnected) document.body.appendChild(probe);
    probe.style.color = '';
    probe.style.color = value;
    if (!probe.style.color) return null;
    const m = getComputedStyle(probe).color.match(/rgba?\(([^)]+)\)/);
    if (!m) return null;
    const p = m[1].split(/[ ,/]+/).filter(Boolean).map(Number);
    return { r: p[0], g: p[1], b: p[2], a: p.length > 3 ? p[3] : 1 };
  };

  const update = () => {
    if (!document.body || !ensureNodes()) return;
    updating = true;
    try {
      varStyle.textContent = '';

      const root = document.querySelector('#app .two') ||
                   document.querySelector('#app > div > div');
      const base = root && parseColor(getComputedStyle(root).backgroundColor);
      const dark = document.body.classList.contains('dark');
      const tint = (TINT && parseColor(TINT)) ||
                   (base && base.a > 0 ? base
                    : dark ? { r: 22, g: 23, b: 23 } : { r: 255, g: 255, b: 255 });

      const cs = getComputedStyle(document.body);
      const rules = [];
      for (let i = 0; i < cs.length; i++) {
        const name = cs[i];
        if (!name.startsWith('--') || !NAME_RE.test(name) || SKIP_RE.test(name)) continue;
        const raw = cs.getPropertyValue(name).trim();
        if (!raw || raw.length > 64 || /url\(|gradient/i.test(raw)) continue;
        const c = parseColor(raw);
        if (!c || c.a < 0.6) continue;
        rules.push(`${name}: rgba(${c.r}, ${c.g}, ${c.b}, 0) !important;`);
      }
      varStyle.textContent =
        (rules.length ? `:root, body, body .dark, body .light { ${rules.join(' ')} }\n` : '') +
        `html body { background-color: rgba(${tint.r}, ${tint.g}, ${tint.b}, ${ALPHA}) !important; }`;
    } finally {
      updating = false;
    }
  };

  const schedule = () => {
    if (updating) return;
    clearTimeout(timer);
    timer = setTimeout(update, 250);
  };

  const observer = new MutationObserver((mutations) => {
    for (const m of mutations) {
      if (m.target === varStyle || m.target === baseStyle) continue;
      if (m.type === 'attributes' ||
          [...m.addedNodes].some(n => n.nodeName === 'STYLE' || n.nodeName === 'LINK')) {
        schedule();
        return;
      }
    }
  });

  const start = () => {
    ensureNodes();
    observer.observe(document.documentElement, {
      attributes: true, attributeFilter: ['class', 'data-theme', 'style'],
    });
    if (document.head) observer.observe(document.head, { childList: true });
    if (document.body) {
      observer.observe(document.body, {
        attributes: true, attributeFilter: ['class', 'data-theme'],
      });
    }
    update();
    [500, 1500, 4000, 10000].forEach(t => setTimeout(update, t));
  };

  if (document.readyState === 'loading') {
    document.addEventListener('DOMContentLoaded', start, { once: true });
  } else {
    start();
  }
  window.addEventListener('load', schedule);
  window.matchMedia('(prefers-color-scheme: dark)').addEventListener('change', schedule);
})();
)JS";

std::wstring JsStringLiteral(const std::wstring& s) {
    std::wstring out = L"\"";
    for (wchar_t c : s) {
        switch (c) {
            case L'\\': out += L"\\\\"; break;
            case L'"': out += L"\\\""; break;
            case L'\n': out += L"\\n"; break;
            case L'\r': break;
            case L'<': out += L"\\u003c"; break;
            default: out += c;
        }
    }
    return out + L"\"";
}

void ReplaceAll(std::wstring& s, const std::wstring& from,
                const std::wstring& to) {
    for (size_t pos = 0; (pos = s.find(from, pos)) != std::wstring::npos;
         pos += to.size()) {
        s.replace(pos, from.size(), to);
    }
}

void BuildScript() {
    wchar_t buffer[32];
    g_script = kScriptTemplate;
    ReplaceAll(g_script, L"__TINT__", JsStringLiteral(g_settings.panelTint));
    swprintf_s(buffer, L"%.2f", g_settings.panelOpacity / 100.0);
    ReplaceAll(g_script, L"__ALPHA__", buffer);
    swprintf_s(buffer, L"%.2f", g_settings.menuOpacity / 100.0);
    ReplaceAll(g_script, L"__MENU_ALPHA__", buffer);
    ReplaceAll(g_script, L"__MENU_BLUR__",
               std::to_wstring(g_settings.menuBlur));
    swprintf_s(buffer, L"%.2f", g_settings.viewerOpacity / 100.0);
    ReplaceAll(g_script, L"__VIEWER_ALPHA__", buffer);
    ReplaceAll(g_script, L"__HIDE_WALLPAPER__",
               g_settings.hideWallpaper ? L"true" : L"false");
    ReplaceAll(g_script, L"__EXTRA_CSS__",
               JsStringLiteral(g_settings.extraCss));
}

int Clamp(int value, int min, int max) {
    return value < min ? min : value > max ? max : value;
}

std::wstring GetString(PCWSTR name) {
    PCWSTR value = Wh_GetStringSetting(name);
    std::wstring result = value;
    Wh_FreeStringSetting(value);
    return result;
}

void LoadSettings() {
    std::wstring backdrop = GetString(L"backdrop");
    if (backdrop == L"mica") {
        g_settings.backdrop = DWMSBT_MAINWINDOW;
    } else if (backdrop == L"mica_tabbed") {
        g_settings.backdrop = DWMSBT_TABBEDWINDOW;
    } else {
        g_settings.backdrop = DWMSBT_TRANSIENTWINDOW;
    }

    g_settings.panelTint = GetString(L"panelTint");
    g_settings.panelOpacity = Clamp(Wh_GetIntSetting(L"panelOpacity"), 0, 100);
    g_settings.menuOpacity = Clamp(Wh_GetIntSetting(L"menuOpacity"), 0, 100);
    g_settings.menuBlur = Clamp(Wh_GetIntSetting(L"menuBlur"), 0, 200);
    g_settings.viewerOpacity = Clamp(Wh_GetIntSetting(L"viewerOpacity"), 0, 100);
    g_settings.hideWallpaper = Wh_GetIntSetting(L"hideWallpaper");
    g_settings.extraCss = GetString(L"extraCss");
}

BOOL Wh_ModInit() {
    LoadSettings();
    BuildScript();

    SetEnvironmentVariableW(L"WEBVIEW2_DEFAULT_BACKGROUND_COLOR", L"00000000");

    Wh_SetFunctionHook((void*)DwmSetWindowAttribute,
                       (void*)DwmSetWindowAttribute_Hook,
                       (void**)&DwmSetWindowAttribute_Original);
    Wh_SetFunctionHook((void*)DwmExtendFrameIntoClientArea,
                       (void*)DwmExtendFrameIntoClientArea_Hook,
                       (void**)&DwmExtendFrameIntoClientArea_Original);

    HookModules(false);

    auto pLoadLibraryExW = (LoadLibraryExW_t)GetProcAddress(
        GetModuleHandleW(L"kernelbase.dll"), "LoadLibraryExW");
    Wh_SetFunctionHook((void*)pLoadLibraryExW, (void*)LoadLibraryExW_Hook,
                       (void**)&LoadLibraryExW_Original);

    return TRUE;
}

void Wh_ModAfterInit() {
    g_stopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_windowThread = CreateThread(nullptr, 0, WindowThread, nullptr, 0, nullptr);
}

void Wh_ModUninit() {
    g_createController.RestoreAll();
    g_createCompositionController.RestoreAll();
    g_createControllerWithOptions.RestoreAll();
    g_createCompositionControllerWithOptions.RestoreAll();
    g_putDefaultBackgroundColor.RestoreAll();
    g_windowFactoryCreateInstance.RestoreAll();
    g_outputLinkCreate.RestoreAll();
    g_outputLinkPutBackgroundColor.RestoreAll();

    if (g_windowThread) {
        SetEvent(g_stopEvent);
        WaitForSingleObject(g_windowThread, INFINITE);
        CloseHandle(g_windowThread);
        CloseHandle(g_stopEvent);
        g_windowThread = nullptr;
        EnumWindows(WindowEnumProc, TRUE);
    }

    if (g_mainHwnd && IsWindow(g_mainHwnd)) {
        RunFromWindowThread(
            g_mainHwnd,
            [](PVOID) {
                if (g_visualTreeWatcher) {
                    g_visualTreeWatcher->Unadvise();
                    g_visualTreeWatcher = nullptr;
                }
                if (g_controller2) {
                    g_controller2->Release();
                    g_controller2 = nullptr;
                }
                try {
                    RestoreXaml();
                } catch (...) {
                }
            },
            nullptr);
    }
}

BOOL Wh_ModSettingsChanged(BOOL* bReload) {
    *bReload = TRUE;
    return TRUE;
}
