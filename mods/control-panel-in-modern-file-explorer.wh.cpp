// ==WindhawkMod==
// @id              control-panel-in-modern-file-explorer
// @name            Control Panel in Modern File Explorer
// @name:pt         Painel de Controle no Explorador moderno
// @name:es         Panel de control en el Explorador moderno
// @description     Opens Control Panel in the modern Windows 11 File Explorer and groups it with Explorer on the taskbar
// @description:pt  Abre o Painel de Controle no Explorador de Arquivos moderno do Windows 11 e o agrupa com o Explorer na barra de tarefas
// @description:es  Abre el Panel de control en el Explorador de archivos moderno de Windows 11 y lo agrupa con Explorer en la barra de tareas
// @version         1.0
// @author          crazyboyybs
// @github          https://github.com/crazyboyybs
// @include         explorer.exe
// @include         control.exe
// @architecture    x86-64
// @compilerOptions -lshell32 -luuid
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Control Panel in Modern File Explorer

Windows 11 normally routes the Control Panel namespace to a dedicated legacy
Explorer window. This mod disables only that special host-selection rule, so
Control Panel opens in the regular modern File Explorer.

![Control Panel in modern File Explorer](https://raw.githubusercontent.com/crazyboyybs/assets/main/Painel%20de%20Controle.jpg)

The Control Panel contents remain unchanged, but they are hosted by the modern
Explorer frame with its current address bar, command bar, and tab interface.
Its window also keeps File Explorer's taskbar identity instead of appearing as
a separately pinned application.

The mod allows normal File Explorer behavior and compatible tab-management mods
and programs to handle Control Panel like other File Explorer locations.
Separate-process behavior for regular folders is not changed, and there
are no settings or background tasks.

Unlike `aerexplorer`, this mod changes host selection only for Control Panel
items and also removes their separate taskbar identity. It doesn't modify the
host selection of other File Explorer locations or include legacy Explorer
customizations.

## Known behavior

* Control Panel is intentionally kept in the modern File Explorer even when
  "Launch folder windows in a separate process" is enabled. Regular folders
  continue to follow that setting.
* Pinning a Control Panel window pins File Explorer because the window shares
  File Explorer's taskbar identity.
* Control Panel launched via Run (`Win+R` → `control`) or `control.exe`
  directly is also covered — the mod hooks `control.exe` too, since it goes
  through the same host-selection functions before launching its own
  Explorer window.

---

# Painel de Controle no Explorador moderno

Normalmente, o Windows 11 encaminha o namespace do Painel de Controle para uma
janela legada dedicada do Explorer. Este mod desativa somente essa regra especial
de seleção de host, fazendo o Painel de Controle abrir no Explorador de Arquivos
moderno normal.

O conteúdo do Painel de Controle não é alterado, mas passa a ser hospedado pelo
frame moderno do Explorer, com a barra de endereços, a barra de comandos e a
interface de abas atuais.
A janela também mantém a identidade do Explorador de Arquivos na barra de
tarefas, em vez de aparecer como um aplicativo fixado separado.

O mod permite que o comportamento normal do Explorador de Arquivos e mods e
programas compatíveis de gerenciamento de abas tratem o Painel de Controle como
outras localizações do Explorador de Arquivos. O comportamento de processo separado
das pastas comuns não é alterado, e não há configurações nem tarefas em segundo plano.

Ao contrário do `aerexplorer`, este mod altera a seleção de host somente para
itens do Painel de Controle e também remove sua identidade separada na barra de
tarefas. Ele não altera o host de outras localizações nem inclui personalizações
do Explorador legado.

## Comportamento conhecido

* O Painel de Controle é mantido intencionalmente no Explorador moderno mesmo
  quando "Iniciar as janelas de pastas em um processo separado" está ativado.
  As pastas comuns continuam seguindo essa configuração.
* Fixar uma janela do Painel de Controle fixa o Explorador de Arquivos, pois a
  janela compartilha a identidade do Explorador na barra de tarefas.
* O Painel de Controle aberto pelo Executar (`Win+R` → `control`) ou
  diretamente pelo `control.exe` também é coberto — o mod também intercepta
  o `control.exe`, pois ele passa pelas mesmas funções de seleção de host
  antes de abrir sua própria janela do Explorer.

---

# Panel de control en el Explorador moderno

Windows 11 normalmente dirige el espacio de nombres del Panel de control a una
ventana heredada dedicada de Explorer. Este mod desactiva solamente esa regla
especial de selección de host, haciendo que el Panel de control se abra en el
Explorador de archivos moderno normal.

El contenido del Panel de control no cambia, pero pasa a estar alojado en el
marco moderno de Explorer, con la barra de direcciones, la barra de comandos y
la interfaz de pestañas actuales.
La ventana también conserva la identidad del Explorador de archivos en la barra
de tareas, en vez de aparecer como una aplicación anclada independiente.

Este mod permite que el Explorador de archivos funcione con normalidad y que los
mods y programas compatibles de gestión de pestañas traten el Panel de control como
cualquier otra ubicación del Explorador de archivos. El comportamiento de los procesos
independientes de las carpetas comunes no se ve alterado, y no hay configuraciones ni
tareas en segundo plano.

A diferencia de `aerexplorer`, este mod cambia la selección del host solamente
para los elementos del Panel de control y también elimina su identidad separada
en la barra de tareas. No cambia el host de otras ubicaciones ni incluye las
personalizaciones heredadas de Explorer.

## Comportamiento conocido

* El Panel de control se mantiene intencionadamente en el Explorador moderno
  incluso cuando está activada la opción de iniciar ventanas de carpeta en un
  proceso independiente. Las carpetas normales siguen respetando esa opción.
* Anclar una ventana del Panel de control ancla el Explorador de archivos porque
  la ventana comparte su identidad en la barra de tareas.
* El Panel de control abierto mediante Ejecutar (`Win+R` → `control`) o
  directamente con `control.exe` también está cubierto — el mod también
  engancha `control.exe`, ya que pasa por las mismas funciones de selección
  de host antes de abrir su propia ventana de Explorer.
*/
// ==/WindhawkModReadme==

#include <windhawk_utils.h>

#include <initguid.h>  // Must appear before propkey.h.

#include <propkey.h>
#include <propidl.h>
#include <shobjidl.h>
#include <windows.h>
#include <wrl/client.h>

#include <atomic>

using IsControlPanel_t = bool(__cdecl*)(IShellItem* item);
using UseSeparateProcess_t = bool(__cdecl*)(IShellItem* item);
using CopyPropertyFromItemToPropStore_t = void(__cdecl*)(
    const PROPERTYKEY& key,
    IShellItem2* item,
    IPropertyStore* propertyStore);
using WindowPropertyStore_SetValue_t = HRESULT(STDMETHODCALLTYPE*)(
    IPropertyStore* propertyStore,
    REFPROPERTYKEY key,
    REFPROPVARIANT value);
using LoadLibraryExW_t = HMODULE(WINAPI*)(LPCWSTR fileName,
                                         HANDLE file,
                                         DWORD flags);

static IsControlPanel_t IsControlPanel_Original;
static UseSeparateProcess_t UseSeparateProcess_Original;
static CopyPropertyFromItemToPropStore_t
    CopyPropertyFromItemToPropStore_Original;
static WindowPropertyStore_SetValue_t WindowPropertyStore_SetValue_Original;
static LoadLibraryExW_t LoadLibraryExW_Original;

static thread_local IPropertyStore* g_controlPanelPropertyStore;
static std::atomic<bool> g_explorerFrameInitializationStarted;
static std::atomic<bool> g_setValueHookInstalled;
static std::atomic<WindowPropertyStore_SetValue_t> g_hookedSetValue;
static std::atomic<bool> g_propertyStoreMismatchLogged;
static std::atomic<bool> g_unloading;
static HMODULE g_explorerFrameRef;
static HMODULE g_setValueModuleRef;

static bool IsCopiedAppIdentityProperty(const PROPERTYKEY& key) {
    return IsEqualPropertyKey(key, PKEY_AppUserModel_RelaunchCommand) ||
           IsEqualPropertyKey(key, PKEY_AppUserModel_RelaunchIconResource) ||
           IsEqualPropertyKey(
               key, PKEY_AppUserModel_RelaunchDisplayNameResource) ||
           IsEqualPropertyKey(key, PKEY_AppUserModel_PreventPinning) ||
           IsEqualPropertyKey(key, PKEY_AppUserModel_ID);
}

static bool __cdecl UseSeparateProcess_Hook(IShellItem* item) {
    const bool useSeparateProcess = UseSeparateProcess_Original(item);
    if (!useSeparateProcess || !item) {
        return useSeparateProcess;
    }

    // IsControlPanel is a required symbol: this hook is only ever live once
    // InitializeExplorerFrameHooks fully resolved, so it's never null here.
    return !IsControlPanel_Original(item);
}

using GetHostFromTarget_t = GUID*(__cdecl*)(void* explorerLauncher,
                                            GUID* clsid,
                                            LPCITEMIDLIST pidl);
static GetHostFromTarget_t GetHostFromTarget_Original;

// The host CExplorerLauncher::GetHostFromTarget selects for items it wants
// to launch in the separate legacy Control Panel host process.
DEFINE_GUID(CLSID_ControlPanelProcessExplorerHost, 0x5BD95610, 0x9434,
           0x43C2, 0x88, 0x6C, 0x57, 0x85, 0x2C, 0xC8, 0xA1, 0x20);

static GUID* __cdecl GetHostFromTarget_Hook(void* explorerLauncher,
                                            GUID* clsid,
                                            LPCITEMIDLIST pidl) {
    GUID* result = GetHostFromTarget_Original(explorerLauncher, clsid, pidl);

    Microsoft::WRL::ComPtr<IShellItem> item;
    if (FAILED(SHCreateItemFromIDList(pidl, IID_PPV_ARGS(&item))) ||
        !IsControlPanel_Original(item.Get())) {
        return result;
    }

    // Diagnostic only, no override: UseSeparateProcess_Hook already steers
    // Control Panel away from CLSID_ControlPanelProcessExplorerHost before
    // this runs (this function's own host choice depends on it - confirmed
    // it returns CLSID_DesktopExplorerHost once UseSeparateProcess is
    // hooked), so no override has ever been observed as necessary here. Logs
    // in case some untested launch path (jump list, pinned shortcut,
    // right-click Open) ever disagrees.
    if (IsEqualGUID(*result, CLSID_ControlPanelProcessExplorerHost)) {
        Wh_Log(L"GetHostFromTarget returned the legacy Control Panel host "
               L"CLSID even though UseSeparateProcess should have prevented "
               L"it");
    }

    return result;
}

static HRESULT STDMETHODCALLTYPE WindowPropertyStore_SetValue_Hook(
    IPropertyStore* propertyStore,
    REFPROPERTYKEY key,
    REFPROPVARIANT value) {
    if (g_controlPanelPropertyStore &&
        propertyStore == g_controlPanelPropertyStore &&
        IsEqualPropertyKey(key, PKEY_AppUserModel_ID)) {
        g_controlPanelPropertyStore = nullptr;

        if (value.vt == VT_LPWSTR && value.pwszVal &&
            CompareStringOrdinal(value.pwszVal, -1,
                                 L"Microsoft.Windows.ControlPanel", -1,
                                 TRUE) == CSTR_EQUAL) {
            // Omitting the value makes the window inherit Explorer's taskbar
            // identity; a later read-back is intentionally empty.
            Wh_Log(L"Suppressed Control Panel AppUserModel.ID");
            return S_OK;
        }

        if (value.vt == VT_LPWSTR && value.pwszVal) {
            Wh_Log(L"Unexpected Control Panel AppUserModel.ID: %s",
                   value.pwszVal);
        } else {
            Wh_Log(L"Unexpected Control Panel AppUserModel.ID type: %u",
                   value.vt);
        }
    }

    return WindowPropertyStore_SetValue_Original(propertyStore, key, value);
}

static void RefSetValueModule(WindowPropertyStore_SetValue_t setValue) {
    // Keeps whatever module implements IPropertyStore::SetValue mapped for
    // as long as the hook is installed, symmetric with RefExplorerFrameModule
    // below - matters more in a short-lived host like control.exe.
    if (!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS,
                            reinterpret_cast<LPCWSTR>(setValue),
                            &g_setValueModuleRef)) {
        Wh_Log(L"Failed to reference the IPropertyStore::SetValue module");
    }
}

static void __cdecl CopyPropertyFromItemToPropStore_Hook(
    const PROPERTYKEY& key,
    IShellItem2* item,
    IPropertyStore* propertyStore) {
    const bool isControlPanel = item && IsControlPanel_Original(item);

    // The direct AppUserModel.ID write follows these copies synchronously on
    // the same thread and uses the same IPropertyStore interface pointer. This
    // raw pointer is only a comparison token and is replaced by the next copy.
    g_controlPanelPropertyStore = isControlPanel ? propertyStore : nullptr;

    if (isControlPanel && propertyStore &&
        !g_unloading.load(std::memory_order_acquire) &&
        !g_setValueHookInstalled.exchange(true, std::memory_order_acq_rel)) {
        // IPropertyStore::SetValue is slot 6 after IUnknown and the read
        // methods. This is the real window property store Explorer is about
        // to write AppUserModel.ID into, so the vtable slot is authoritative
        // for this process - no separate probe is needed.
        auto setValue = reinterpret_cast<WindowPropertyStore_SetValue_t>(
            (*reinterpret_cast<void***>(propertyStore))[6]);
        if (!WindhawkUtils::SetFunctionHook(
                setValue, WindowPropertyStore_SetValue_Hook,
                &WindowPropertyStore_SetValue_Original)) {
            Wh_Log(L"Failed to hook IPropertyStore::SetValue; Control Panel "
                   L"windows will keep their own taskbar identity");
        } else if (!Wh_ApplyHookOperations()) {
            Wh_Log(L"Failed to apply the IPropertyStore::SetValue hook; "
                   L"Control Panel windows will keep their own taskbar "
                   L"identity");
        } else {
            g_hookedSetValue.store(setValue, std::memory_order_relaxed);
            RefSetValueModule(setValue);
        }
    }

    if (isControlPanel && propertyStore) {
        // Detects a different SetValue implementation on a later Control
        // Panel window (e.g. wrapped by a shell extension) - a silent no-op
        // on the call that just installed the hook above, since it compares
        // the same store's slot against the value it was just read from.
        const WindowPropertyStore_SetValue_t hookedSetValue =
            g_hookedSetValue.load(std::memory_order_relaxed);
        if (hookedSetValue &&
            (*reinterpret_cast<void***>(propertyStore))[6] !=
                reinterpret_cast<void*>(hookedSetValue) &&
            !g_propertyStoreMismatchLogged.exchange(
                true, std::memory_order_relaxed)) {
            Wh_Log(L"Window property store uses a different SetValue "
                   L"implementation; the Control Panel taskbar identity "
                   L"won't be suppressed");
        }
    }

    if (isControlPanel && IsCopiedAppIdentityProperty(key)) {
        return;
    }

    CopyPropertyFromItemToPropStore_Original(key, item, propertyStore);
}

static bool InitializeExplorerFrameHooks(HMODULE explorerFrameModule) {
    // Symbol forms confirmed against this build's PDB dump; the plain
    // __cdecl spelling is the only one that resolves (no __ptr64 variant).
    WindhawkUtils::SYMBOL_HOOK explorerFrameDllHooks[] = {
        {
            {
                LR"(bool __cdecl IsControlPanel(struct IShellItem *))",
            },
            &IsControlPanel_Original,
            nullptr,
            false,
        },
        {
            {
                LR"(bool __cdecl UseSeparateProcess(struct IShellItem *))",
            },
            &UseSeparateProcess_Original,
            UseSeparateProcess_Hook,
            false,
        },
        {
            {
                LR"(void __cdecl CopyPropertyFromItemToPropStore(struct _tagpropertykey const &,struct IShellItem2 *,struct IPropertyStore *))",
            },
            &CopyPropertyFromItemToPropStore_Original,
            CopyPropertyFromItemToPropStore_Hook,
            true,
        },
        {
            {
                LR"(private: struct _GUID __cdecl CExplorerLauncher::GetHostFromTarget(struct _ITEMIDLIST_ABSOLUTE const *))",
            },
            &GetHostFromTarget_Original,
            GetHostFromTarget_Hook,
            true,
        },
    };

    if (!WindhawkUtils::HookSymbols(explorerFrameModule,
                                    explorerFrameDllHooks,
                                    ARRAYSIZE(explorerFrameDllHooks)) ||
        !IsControlPanel_Original || !UseSeparateProcess_Original) {
        return false;
    }

    // Both are optional: losing either only narrows Control Panel's modern-
    // frame coverage, it doesn't break the mod's main feature.
    if (!CopyPropertyFromItemToPropStore_Original) {
        Wh_Log(L"CopyPropertyFromItemToPropStore not found; Control Panel "
               L"windows will keep their own taskbar identity");
    }

    if (!GetHostFromTarget_Original) {
        Wh_Log(L"CExplorerLauncher::GetHostFromTarget not found; losing "
               L"only the legacy-host diagnostic, not functionality");
    }

    return true;
}

static void RefExplorerFrameModule(HMODULE explorerFrameModule) {
    // An ordinary reference, not a pin: released in Wh_ModUninit, once hooks
    // are already removed and the module is safe to let go.
    if (!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS,
                            reinterpret_cast<LPCWSTR>(explorerFrameModule),
                            &g_explorerFrameRef)) {
        Wh_Log(L"Failed to reference ExplorerFrame.dll");
    }
}

static void ReleaseExplorerFrameRef() {
    if (g_explorerFrameRef) {
        FreeLibrary(g_explorerFrameRef);
        g_explorerFrameRef = nullptr;
    }
}

static void ReleaseSetValueModuleRef() {
    if (g_setValueModuleRef) {
        FreeLibrary(g_setValueModuleRef);
        g_setValueModuleRef = nullptr;
    }
}

static bool InitializeExplorerFrameHooksAfterLoad(HMODULE explorerFrameModule,
                                                   bool applyHooks) {
    if (g_unloading.load(std::memory_order_acquire)) {
        return false;
    }

    if (g_explorerFrameInitializationStarted.exchange(
            true, std::memory_order_acq_rel)) {
        return false;
    }

    RefExplorerFrameModule(explorerFrameModule);
    if (!InitializeExplorerFrameHooks(explorerFrameModule)) {
        Wh_Log(L"Failed to install ExplorerFrame.dll hooks after late load");
        return false;
    }

    if (applyHooks && !Wh_ApplyHookOperations()) {
        Wh_Log(L"Failed to apply ExplorerFrame.dll hooks");
    }

    return true;
}

static HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR fileName,
                                          HANDLE file,
                                          DWORD flags) {
    HMODULE module = LoadLibraryExW_Original(fileName, file, flags);
    constexpr DWORD kDataOnlyFlags =
        LOAD_LIBRARY_AS_DATAFILE | LOAD_LIBRARY_AS_DATAFILE_EXCLUSIVE |
        LOAD_LIBRARY_AS_IMAGE_RESOURCE;
    if (module && !g_unloading.load(std::memory_order_acquire) &&
        !(flags & kDataOnlyFlags) &&
        !g_explorerFrameInitializationStarted.load(std::memory_order_acquire) &&
        module == GetModuleHandleW(L"ExplorerFrame.dll")) {
        InitializeExplorerFrameHooksAfterLoad(module, true);
    }

    return module;
}

BOOL Wh_ModInit() {
    Wh_Log(L">");

    HMODULE explorerFrameModule = GetModuleHandleW(L"ExplorerFrame.dll");
    if (explorerFrameModule) {
        g_explorerFrameInitializationStarted.store(true,
                                                   std::memory_order_release);
        RefExplorerFrameModule(explorerFrameModule);
        if (!InitializeExplorerFrameHooks(explorerFrameModule)) {
            Wh_Log(L"Failed to install ExplorerFrame.dll hooks");
            // Wh_ModUninit won't run since Wh_ModInit is about to return
            // FALSE - release the reference here instead.
            ReleaseExplorerFrameRef();
            return FALSE;
        }

        return TRUE;
    }

    HMODULE kernelBaseModule = GetModuleHandleW(L"kernelbase.dll");
    auto loadLibraryExW = kernelBaseModule
                              ? reinterpret_cast<LoadLibraryExW_t>(
                                    GetProcAddress(kernelBaseModule,
                                                   "LoadLibraryExW"))
                              : nullptr;
    if (!loadLibraryExW) {
        Wh_Log(L"Failed to resolve LoadLibraryExW");
        return FALSE;
    }

    if (!WindhawkUtils::SetFunctionHook(loadLibraryExW, LoadLibraryExW_Hook,
                                        &LoadLibraryExW_Original)) {
        Wh_Log(L"Failed to hook LoadLibraryExW");
        return FALSE;
    }

    return TRUE;
}

void Wh_ModAfterInit() {
    Wh_Log(L">");

    if (HMODULE explorerFrameModule =
            GetModuleHandleW(L"ExplorerFrame.dll")) {
        InitializeExplorerFrameHooksAfterLoad(explorerFrameModule, true);
    }
}

void Wh_ModBeforeUninit() {
    Wh_Log(L">");
    g_unloading.store(true, std::memory_order_release);
}

void Wh_ModUninit() {
    Wh_Log(L">");
    ReleaseExplorerFrameRef();
    ReleaseSetValueModuleRef();
}
