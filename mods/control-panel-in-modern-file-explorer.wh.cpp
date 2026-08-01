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
// @architecture    x86-64
// @compilerOptions -lshell32
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Control Panel in Modern File Explorer

Windows 11 normally routes the Control Panel namespace to a dedicated legacy
Explorer window. This mod disables only that special host-selection rule, so
Control Panel opens in the regular modern File Explorer.

The Control Panel contents remain unchanged, but they are hosted by the modern
Explorer frame with its current address bar, command bar, and tab interface.
Its window also keeps File Explorer's taskbar identity instead of appearing as
a separately pinned application.

The mod allows normal File Explorer behavior and compatible tab-management mods
and programs to handle Control Panel like other File Explorer locations.
Separate-process behavior for regular folders is not changed, and there
are no settings or background tasks.

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
*/
// ==/WindhawkModReadme==

#include <windhawk_utils.h>
#include <propidl.h>
#include <shellapi.h>
#include <shobjidl.h>
#include <windows.h>
#include <wrl/client.h>

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

static IsControlPanel_t IsControlPanel_Original;
static UseSeparateProcess_t UseSeparateProcess_Original;
static CopyPropertyFromItemToPropStore_t
    CopyPropertyFromItemToPropStore_Original;
static WindowPropertyStore_SetValue_t WindowPropertyStore_SetValue_Original;

static HMODULE g_explorerFrameModule;
static bool g_ownsExplorerFrameModule;

static constexpr GUID kAppUserModelFormatId = {
    0x9f4c2855,
    0x9f79,
    0x4b39,
    {0xa8, 0xd0, 0xe1, 0xd4, 0x2d, 0xe1, 0xd5, 0xf3},
};

static constexpr DWORD kAppUserModelRelaunchCommandPid = 2;
static constexpr DWORD kAppUserModelRelaunchIconPid = 3;
static constexpr DWORD kAppUserModelRelaunchDisplayNamePid = 4;
static constexpr DWORD kAppUserModelIdPid = 5;
static constexpr DWORD kAppUserModelPreventPinningPid = 9;

static bool IsCopiedAppIdentityProperty(const PROPERTYKEY& key) {
    if (!IsEqualGUID(key.fmtid, kAppUserModelFormatId)) {
        return false;
    }

    return key.pid == kAppUserModelRelaunchCommandPid ||
           key.pid == kAppUserModelRelaunchIconPid ||
           key.pid == kAppUserModelRelaunchDisplayNamePid ||
           key.pid == kAppUserModelPreventPinningPid;
}

static bool IsControlPanelAppId(REFPROPVARIANT value) {
    const wchar_t* appId = nullptr;
    if (value.vt == VT_LPWSTR) {
        appId = value.pwszVal;
    } else if (value.vt == VT_BSTR) {
        appId = value.bstrVal;
    }

    return appId &&
           _wcsicmp(appId, L"Microsoft.Windows.ControlPanel") == 0;
}

static bool __cdecl UseSeparateProcess_Hook(IShellItem* item) {
    const bool useSeparateProcess = UseSeparateProcess_Original(item);
    if (!useSeparateProcess || !item) {
        return useSeparateProcess;
    }

    if (IsControlPanel_Original(item)) {
        return false;
    }

    return true;
}

static void __cdecl CopyPropertyFromItemToPropStore_Hook(
    const PROPERTYKEY& key,
    IShellItem2* item,
    IPropertyStore* propertyStore) {
    if (item && IsCopiedAppIdentityProperty(key) &&
        IsControlPanel_Original(item)) {
        return;
    }

    CopyPropertyFromItemToPropStore_Original(key, item, propertyStore);
}

static HRESULT STDMETHODCALLTYPE WindowPropertyStore_SetValue_Hook(
    IPropertyStore* propertyStore,
    REFPROPERTYKEY key,
    REFPROPVARIANT value) {
    if (key.pid == kAppUserModelIdPid &&
        IsEqualGUID(key.fmtid, kAppUserModelFormatId) &&
        IsControlPanelAppId(value)) {
        return S_OK;
    }

    return WindowPropertyStore_SetValue_Original(propertyStore, key, value);
}

static bool HookWindowPropertyStoreSetValue() {
    HWND probeWindow = CreateWindowExW(0, L"Static", nullptr, WS_POPUP, 0, 0,
                                       0, 0, nullptr, nullptr, nullptr, nullptr);
    if (!probeWindow) {
        return false;
    }

    Microsoft::WRL::ComPtr<IPropertyStore> propertyStore;
    const HRESULT hr = SHGetPropertyStoreForWindow(
        probeWindow, IID_PPV_ARGS(&propertyStore));
    const BOOL windowDestroyed = DestroyWindow(probeWindow);
    if (FAILED(hr) || !propertyStore || !windowDestroyed) {
        return false;
    }

    // IPropertyStore::SetValue is slot 6 after IUnknown and the read methods.
    void** vtable = *reinterpret_cast<void***>(propertyStore.Get());
    auto setValue =
        reinterpret_cast<WindowPropertyStore_SetValue_t>(vtable[6]);
    return WindhawkUtils::SetFunctionHook(
        setValue, WindowPropertyStore_SetValue_Hook,
        &WindowPropertyStore_SetValue_Original);
}

BOOL Wh_ModInit() {
    g_explorerFrameModule = GetModuleHandleW(L"ExplorerFrame.dll");
    if (!g_explorerFrameModule) {
        // These are free functions without a vtable fallback. Loading the DLL
        // here lets HookSymbols resolve the required functions during Wh_ModInit.
        g_explorerFrameModule = LoadLibraryExW(
            L"ExplorerFrame.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
        g_ownsExplorerFrameModule = g_explorerFrameModule != nullptr;
    }

    if (!g_explorerFrameModule) {
        Wh_Log(L"Failed to load ExplorerFrame.dll");
        return FALSE;
    }

    WindhawkUtils::SYMBOL_HOOK explorerFrameDllHooks[] = {
        {
            {
                LR"(bool __cdecl IsControlPanel(struct IShellItem *))",
                LR"(bool IsControlPanel(struct IShellItem *))",
                LR"(bool __cdecl IsControlPanel(struct IShellItem * __ptr64))",
            },
            &IsControlPanel_Original,
            nullptr,
            false,
        },
        {
            {
                LR"(bool __cdecl UseSeparateProcess(struct IShellItem *))",
                LR"(bool UseSeparateProcess(struct IShellItem *))",
                LR"(bool __cdecl UseSeparateProcess(struct IShellItem * __ptr64))",
            },
            &UseSeparateProcess_Original,
            UseSeparateProcess_Hook,
            false,
        },
        {
            {
                LR"(void __cdecl CopyPropertyFromItemToPropStore(struct _tagpropertykey const &,struct IShellItem2 *,struct IPropertyStore *))",
                LR"(void CopyPropertyFromItemToPropStore(struct _tagpropertykey const &,struct IShellItem2 *,struct IPropertyStore *))",
                LR"(void __cdecl CopyPropertyFromItemToPropStore(struct _tagpropertykey const & __ptr64,struct IShellItem2 * __ptr64,struct IPropertyStore * __ptr64))",
            },
            &CopyPropertyFromItemToPropStore_Original,
            CopyPropertyFromItemToPropStore_Hook,
            false,
        },
    };

    if (!WindhawkUtils::HookSymbols(g_explorerFrameModule,
                                    explorerFrameDllHooks,
                                    ARRAYSIZE(explorerFrameDllHooks)) ||
        !IsControlPanel_Original || !UseSeparateProcess_Original ||
        !CopyPropertyFromItemToPropStore_Original ||
        !HookWindowPropertyStoreSetValue() ||
        !WindowPropertyStore_SetValue_Original) {
        Wh_Log(L"Failed to install Control Panel hooks");
        if (g_ownsExplorerFrameModule) {
            FreeLibrary(g_explorerFrameModule);
            g_explorerFrameModule = nullptr;
            g_ownsExplorerFrameModule = false;
        }
        return FALSE;
    }

    return TRUE;
}

void Wh_ModUninit() {
    if (g_ownsExplorerFrameModule && g_explorerFrameModule) {
        FreeLibrary(g_explorerFrameModule);
    }

    g_explorerFrameModule = nullptr;
    g_ownsExplorerFrameModule = false;
    IsControlPanel_Original = nullptr;
    UseSeparateProcess_Original = nullptr;
    CopyPropertyFromItemToPropStore_Original = nullptr;
    WindowPropertyStore_SetValue_Original = nullptr;
}
