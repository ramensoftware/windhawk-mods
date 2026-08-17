// ==WindhawkMod==
// @id              explorer-home-devices
// @name            Devices and drives in Explorer Home
// @name:pt         Dispositivos e unidades na Página Inicial do Explorador
// @name:es         Dispositivos y unidades en Inicio del Explorador
// @description     Adds a native WinUI section for devices and drives after Quick access on the Windows 11 File Explorer Home page.
// @description:pt  Adiciona uma seção WinUI nativa de dispositivos e unidades depois de Acesso rápido na Página Inicial do Explorador do Windows 11.
// @description:es  Agrega una sección WinUI nativa de dispositivos y unidades después de Acceso rápido en la página Inicio del Explorador de Windows 11.
// @version         1.0
// @author          crazyboyybs
// @github          https://github.com/crazyboyybs
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lruntimeobject -lshell32 -lshlwapi -lcomctl32 -lwindowscodecs -luuid -lgdi32
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Devices and drives in the File Explorer Home folder

![Devices and drives in Explorer Home](https://raw.githubusercontent.com/crazyboyybs/assets/refs/heads/main/Dispositivos%20e%20unidades.png)

The This PC folder in Windows File Explorer shows your drives, this mod
brings that to the Home folder. It adds a native **Devices and drives**
section right after **Quick access**, built with the same WinUI the rest of
the Home page already uses.

**What you get:**
- Drive cards with the real name, icon, type and free-space bar, taken
  straight from the Shell, the same info you'd see in "This PC".
- Double-click a card to open the drive in the current tab; drag one out to
  create a shortcut, or drag files onto it to copy or move them there.
- Right-click a card (or use your keyboard's context-menu shortcut) for the
  real Windows context menu, Eject, Properties, Rename and everything else
  you're used to, including with multiple drives selected.
- Rename works inline, just like a normal folder.
- The section header can collapse the whole grid if you want it out of the
  way.
- The grid keeps itself up to date automatically whenever a drive is plugged
  in, removed, or its free space changes.

**A quick technical note:** the mod reads Explorer's own WinUI page and
inserts the section directly into it, without relying on XAML Diagnostics,
so it plays nicely alongside other Explorer styling mods that need that
facility for themselves.

---

# Dispositivos e unidades na pasta Início do Explorador de Arquivos

A pasta Este Computador do Explorador de Arquivos do Windows mostra suas
unidades, esse mod traz isso para a pasta Início. Ele adiciona uma seção
nativa de **Dispositivos e unidades** logo depois do **Acesso rápido**,
construída com o mesmo WinUI que o resto da Página Inicial já usa.

**O que você ganha:**
- Cards de unidade com nome, ícone, tipo e barra de espaço livre reais,
  vindos direto do Shell, a mesma informação que você veria em "Este
  Computador".
- Dê duplo clique num card pra abrir a unidade na aba atual; arraste um card
  pra fora pra criar um atalho, ou arraste arquivos pra dentro dele pra
  copiar ou mover.
- Clique com o botão direito (ou use o atalho de menu de contexto do seu
  teclado) pra ver o menu de contexto de verdade do Windows, Ejetar,
  Propriedades, Renomear e tudo mais que você já conhece, inclusive com
  várias unidades selecionadas.
- Renomear funciona direto ali, igual numa pasta comum.
- O cabeçalho da seção pode recolher a grade inteira se você quiser tirá-la
  do caminho.
- A grade se atualiza sozinha sempre que uma unidade é conectada, removida,
  ou o espaço livre muda.

**Um detalhe técnico rápido:** o mod lê a própria página WinUI do Explorador
e insere a seção diretamente nela, sem depender do XAML Diagnostics, então
ele convive bem com outros mods de estilo do Explorador que precisam desse
recurso pra si.

---

# Dispositivos y unidades en la carpeta Inicio del Explorador de archivos

La carpeta Este equipo del Explorador de archivos de Windows muestra tus
unidades, este mod trae eso a la carpeta Inicio. Agrega una sección nativa
de **Dispositivos y unidades** justo después de **Acceso rápido**,
construida con el mismo WinUI que ya usa el resto de la página de Inicio.

**Qué obtienes:**
- Tarjetas de unidad con el nombre, ícono, tipo y barra de espacio libre
  reales, tomados directamente del Shell, la misma información que verías
  en "Equipo".
- Haz doble clic en una tarjeta para abrir la unidad en la pestaña actual;
  arrastra una hacia afuera para crear un acceso directo, o arrastra
  archivos sobre ella para copiarlos o moverlos.
- Haz clic derecho (o usa el atajo de menú contextual de tu teclado) para
  ver el menú contextual real de Windows, Expulsar, Propiedades, Cambiar
  nombre y todo lo demás de siempre, incluso con varias unidades
  seleccionadas.
- Cambiar el nombre funciona en línea, igual que en una carpeta normal.
- El encabezado de la sección puede contraer toda la cuadrícula si quieres
  quitarla de en medio.
- La cuadrícula se actualiza sola cada vez que se conecta o quita una
  unidad, o cambia el espacio libre.

**Un detalle técnico rápido:** el mod lee la propia página WinUI del
Explorador e inserta la sección directamente en ella, sin depender de XAML
Diagnostics, así que convive bien con otros mods de estilo del Explorador
que necesitan ese recurso para sí mismos.
*/
// ==/WindhawkModReadme==

#include <windows.h>

#include <commctrl.h>
#include <dbt.h>
#include <inspectable.h>
#include <robuffer.h>
#include <shellapi.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <propkey.h>
#include <wincodec.h>

#include <windhawk_utils.h>

#include <atomic>
#include <algorithm>
#include <chrono>
#include <cmath>
#include <condition_variable>
#include <cstdint>
#include <cstring>
#include <list>
#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <string_view>
#include <thread>
#include <unordered_map>
#include <vector>

// Conflicts with a WinRT method of the same name.
#undef GetCurrentTime

#include <winrt/base.h>
#include <winrt/Windows.ApplicationModel.DataTransfer.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Storage.h>
#include <winrt/Windows.Storage.Streams.h>
#include <winrt/Windows.System.h>
#include <winrt/Windows.UI.Text.h>
// Not spelled out directly in this file, but required transitively: the
// pointer-tracking code (UpdateDriveMarqueeSelection and friends) calls
// Properties()/PointerId()/Position() on a Microsoft::UI::Input::PointerPoint
// obtained from PointerRoutedEventArgs::GetCurrentPoint(), and those methods'
// deduced return types don't resolve without this header's definitions
// visible, even though the type itself is never named explicitly.
#include <winrt/Microsoft.UI.Input.h>
#include <winrt/Microsoft.UI.Xaml.h>
#include <winrt/Microsoft.UI.Xaml.Controls.h>
#include <winrt/Microsoft.UI.Xaml.Controls.Primitives.h>
#include <winrt/Microsoft.UI.Xaml.Input.h>
#include <winrt/Microsoft.UI.Xaml.Media.h>
#include <winrt/Microsoft.UI.Xaml.Media.Imaging.h>

namespace mux = winrt::Microsoft::UI::Xaml;
namespace muxc = winrt::Microsoft::UI::Xaml::Controls;
namespace muxcp = winrt::Microsoft::UI::Xaml::Controls::Primitives;
namespace muxi = winrt::Microsoft::UI::Xaml::Input;
namespace muxm = winrt::Microsoft::UI::Xaml::Media;
namespace muxmi = winrt::Microsoft::UI::Xaml::Media::Imaging;
namespace wut = winrt::Windows::UI::Text;

namespace {

constexpr wchar_t kPrimaryContentPresenterName[] =
    L"PrimaryGroupContentPresenter";
constexpr wchar_t kDevicesSectionName[] = L"WindhawkDevicesSection";
constexpr wchar_t kDevicesGridName[] = L"WindhawkDevicesGrid";
constexpr UINT_PTR kExplorerWindowSubclassId = 0x48444D45;

// CREATE/DELETE/MKDIR/RMDIR cover drives and devices appearing/disappearing
// under This PC; the direct drive events cover capacity/media changes;
// RENAMEITEM/RENAMEFOLDER catch a drive renamed from outside this mod (e.g.
// Explorer's own Properties dialog). UPDATEITEM/UPDATEDIR are deliberately
// left out: per the SHChangeNotify docs they only ever fire for content
// changes that are *not* a create/delete/rename (those instead use
// CREATE/DELETE/RENAMEITEM/RENAMEFOLDER, all already covered above), so
// they add delivery volume without covering anything this mod needs.
// Portable/MTP device arrival is caught separately via WM_DEVICECHANGE (see
// ExplorerWindowSubclassProc).
constexpr LONG kShellDriveEvents = SHCNE_DRIVEADD | SHCNE_DRIVEREMOVED |
                                    SHCNE_MEDIAINSERTED |
                                    SHCNE_MEDIAREMOVED | SHCNE_FREESPACE |
                                    SHCNE_CREATE | SHCNE_DELETE |
                                    SHCNE_MKDIR | SHCNE_RMDIR |
                                    SHCNE_RENAMEITEM | SHCNE_RENAMEFOLDER;

std::atomic<bool> g_unloading{false};
std::atomic<bool> g_fileExplorerExtensionsSymbolsHooked{false};
std::atomic<bool> g_navigationSymbolsHooked{false};
std::atomic<int> g_openContextMenuCount{0};
std::atomic<int> g_pendingCommandInvocations{0};
std::atomic<int> g_pendingShellUiCalls{0};
std::atomic<int> g_pendingDragPreparations{0};
std::mutex g_dragPreparationMutex;
std::condition_variable g_dragPreparationCondition;
std::atomic<int> g_pendingDropOperations{0};
std::mutex g_dropOperationMutex;
std::condition_variable g_dropOperationCondition;

struct IFileExplorerNavigationControllerAbi : IInspectable {
    virtual HRESULT STDMETHODCALLTYPE StartNavigation(IInspectable* target) = 0;
    virtual HRESULT STDMETHODCALLTYPE SubmitAddressBarText(HSTRING target) = 0;
};

constexpr IID kIidFileExplorerNavigationController = {
    0x47319F7F,
    0x354A,
    0x5591,
    {0x9E, 0x6C, 0x43, 0xCF, 0x87, 0x97, 0x37, 0xD6}};

struct DriveInfo {
    // This is an absolute Shell parsing name. For file-system drives it is
    // normally C:\; namespace devices can use a ::{CLSID}\... form instead.
    std::wstring rootPath;
    std::wstring fileSystemPath;
    std::wstring displayName;
    std::wstring typeName;
    std::wstring spaceDescription;
    UINT driveType = DRIVE_UNKNOWN;
    // Owned BGRA pixels let the worker transfer Shell icons without sharing
    // thread-affine WinUI or WIC objects with the Explorer UI thread.
    UINT iconWidth = 0;
    UINT iconHeight = 0;
    std::vector<uint8_t> iconPixels;
    double percentUsed = 0;
    bool hasSpaceInformation = false;
    bool canStartWinUiDrag = false;
    bool canAcceptDrop = true;
};

using DriveSnapshot = std::vector<DriveInfo>;

constexpr uint32_t kDriveRefreshTopology = 1 << 0;
constexpr uint32_t kDriveRefreshCapacity = 1 << 1;

std::mutex g_refreshMutex;
std::condition_variable g_refreshCondition;
uint64_t g_requestedRefreshGeneration = 0;
uint32_t g_requestedRefreshKinds = 0;
bool g_refreshWorkerStopping = false;
// Windhawk doesn't call Wh_ModUninit during process shutdown, so a plain
// std::thread would still be joinable when its destructor runs there,
// calling std::terminate() and aborting Explorer. [[clang::no_destroy]]
// suppresses that destructor on every path; StopDriveRefreshWorker() does
// the real join()+reset() explicitly instead.
[[clang::no_destroy]] std::optional<std::thread> g_refreshWorker;
std::shared_ptr<const DriveSnapshot> g_latestDriveSnapshot;

std::mutex g_registeredWindowsMutex;
std::vector<HWND> g_registeredWindows;

std::mutex g_navigationControllersMutex;
std::unordered_map<DWORD, IFileExplorerNavigationControllerAbi*>
    g_navigationControllersByThread;

// XAML objects are only accessed by the UI thread which owns them.
struct HomePanelState {
    winrt::weak_ref<muxc::StackPanel> panel;
    IFileExplorerNavigationControllerAbi* navigationController = nullptr;
};

thread_local std::vector<HomePanelState> g_homePanels;
// A zero registration ID means that the window subclass is active and
// WM_DEVICECHANGE remains available as a fallback.
thread_local std::unordered_map<HWND, ULONG>
    g_windowNotificationRegistrations;
thread_local HHOOK g_driveKeyboardMessageHook = nullptr;
thread_local IContextMenu2* g_trackedContextMenu2 = nullptr;
thread_local IContextMenu3* g_trackedContextMenu3 = nullptr;
// Set for the duration of a drag that started on one of the mod's own drive
// cards (DragStarting to DropCompleted), so the cards decline to be drop
// targets for their own kind of drag -- dropping one drive card on another
// would otherwise queue a whole-volume copy/move.
thread_local bool g_driveCardDragInProgress = false;

struct DriveCardEventState {
    winrt::weak_ref<muxc::GridViewItem> item;
    muxc::TextBlock title{nullptr};
    muxc::TextBox renameBox{nullptr};
    mux::UIElement driveIcon{nullptr};
    muxc::CheckBox selectionCheckBox{nullptr};
    winrt::event_token doubleTappedToken;
    winrt::Windows::Foundation::IInspectable keyDownHandler{nullptr};
    winrt::event_token rightTappedToken;
    winrt::event_token dragStartingToken;
    winrt::event_token dragEnterToken;
    winrt::event_token dragOverToken;
    winrt::event_token dropToken;
    winrt::event_token dropCompletedToken;
    winrt::event_token pointerEnteredToken;
    winrt::event_token pointerExitedToken;
    winrt::event_token selectionCheckBoxClickToken;
    int64_t selectedChangedToken = 0;
    winrt::event_token renameKeyDownToken;
    winrt::event_token renameLostFocusToken;
    IFileExplorerNavigationControllerAbi* navigationController = nullptr;
    bool renaming = false;
    // Separate from renaming: renaming stays true for the whole edit
    // (including while RenameDriveWithShell's Shell call is in flight, so
    // IsDriveCardRenamingInGrid keeps reporting the card busy), while this
    // one guards CompleteDriveRename itself against being re-entered while
    // still on the stack -- e.g. a queued LostFocus dispatched by the
    // pumping SetNameOf call.
    bool renameCompleting = false;
    bool renameFocusPending = false;
    bool selectionCheckBoxesEnabled = false;
    bool pointerOver = false;
};

// A std::list, not a std::vector: FindDriveCardState()/FindDriveRenameState()
// return pointers into this container that callers keep across Shell calls
// (RenameDriveWithShell, context-menu invocation) which can pump messages
// and reentrantly trigger a refresh that erases unrelated entries. A vector
// would shift every element after an erased one, invalidating those
// pointers; a list only invalidates the iterator/pointer of the erased
// element itself.
//
// Wrapped in [[clang::no_destroy]] std::optional for the same reason as
// g_refreshWorker: Windhawk doesn't call Wh_ModUninit during process
// shutdown, but the CRT still runs thread_local destructors on
// DLL_PROCESS_DETACH. DriveCardEventState holds strong XAML references
// (TextBlock/TextBox/UIElement/CheckBox), and releasing those after the
// XAML core has already torn down is a use-after-free, not a leak.
// ClearDriveCardEventHandlersForCurrentThread() empties the list explicitly
// on its owning thread during normal unload; an empty list keeps no heap
// buffer, so suppressing the destructor elsewhere leaks nothing per thread.
[[clang::no_destroy]] thread_local std::optional<std::list<DriveCardEventState>>
    g_driveCardEventStates{std::in_place};

struct DevicesHeaderEventState {
    winrt::weak_ref<muxc::Button> button;
    winrt::weak_ref<muxc::GridView> grid;
    winrt::event_token clickToken;
    winrt::event_token themeChangedToken;
    bool expanded = true;
};

thread_local std::vector<DevicesHeaderEventState>
    g_devicesHeaderEventStates;

struct HomeSelectionEventState {
    winrt::weak_ref<muxc::Grid> surface;
    winrt::weak_ref<muxc::StackPanel> panel;
    winrt::Windows::Foundation::IInspectable pressedHandler{nullptr};
    winrt::Windows::Foundation::IInspectable movedHandler{nullptr};
    winrt::Windows::Foundation::IInspectable releasedHandler{nullptr};
    winrt::Windows::Foundation::Point start{};
    uint32_t pointerId = 0;
    bool tracking = false;
    bool moved = false;
    bool additive = false;
    std::vector<winrt::weak_ref<muxc::GridViewItem>> initialSelection;
};

thread_local std::vector<HomeSelectionEventState>
    g_homeSelectionEventStates;

void RequestDriveRefresh(uint32_t refreshKinds = kDriveRefreshTopology);
void RequestInitialDriveSnapshot();
void RefreshDevicesSectionsForCurrentThread();
void RefreshThemedVisualsForCurrentThread();
void EnsureShellNotificationsForCurrentThread();
std::vector<HWND> GetFileExplorerWindows();
void ClearDriveCardEventHandlersForCurrentThread();
void ClearDevicesHeaderEventHandlersForCurrentThread();
void ClearHomeSelectionEventHandlersForCurrentThread();

muxm::Brush TryGetBrush(std::wstring_view key) {
    try {
        auto application = mux::Application::Current();
        if (!application) {
            return nullptr;
        }

        auto resources = application.Resources();
        auto boxedKey = winrt::box_value(winrt::hstring{key});
        if (!resources.HasKey(boxedKey)) {
            return nullptr;
        }

        return resources.Lookup(boxedKey).try_as<muxm::Brush>();
    } catch (...) {
        return nullptr;
    }
}

template <typename T>
void ApplyBrushIfAvailable(T const& element, std::wstring_view resourceKey) {
    if (auto brush = TryGetBrush(resourceKey)) {
        element.Foreground(brush);
    }
}

mux::Style TryGetStyle(std::wstring_view key) {
    try {
        auto application = mux::Application::Current();
        if (!application) {
            return nullptr;
        }

        auto resources = application.Resources();
        auto boxedKey = winrt::box_value(winrt::hstring{key});
        if (!resources.HasKey(boxedKey)) {
            return nullptr;
        }

        return resources.Lookup(boxedKey).try_as<mux::Style>();
    } catch (...) {
        return nullptr;
    }
}

// A local property value (a direct FontSize()/FontFamily() call) always
// outranks a Style setter for the same property, so this only sets the
// hard-coded literals as a fallback when the named style isn't found —
// applying both would make the style's setters dead weight and keep text
// from following the system text-size accessibility setting.
template <typename T>
void ApplyBodyStrongTextStyle(T const& element) {
    if (auto style = TryGetStyle(L"BodyStrongTextBlockStyle")) {
        element.Style(style);
    } else {
        element.FontFamily(muxm::FontFamily{L"Segoe UI Variable"});
        element.FontSize(14);
        element.FontWeight(wut::FontWeights::SemiBold());
    }
}

template <typename T>
void ApplyCaptionTextStyle(T const& element) {
    if (auto style = TryGetStyle(L"CaptionTextBlockStyle")) {
        element.Style(style);
    } else {
        element.FontFamily(muxm::FontFamily{L"Segoe UI Variable"});
        element.FontSize(12);
    }
}

std::wstring LoadShellString(PCWSTR reference, std::wstring_view fallback) {
    wchar_t buffer[256]{};
    if (SUCCEEDED(SHLoadIndirectString(reference, buffer, ARRAYSIZE(buffer),
                                       nullptr)) &&
        buffer[0]) {
        return buffer;
    }

    return std::wstring{fallback};
}

struct ShellStrings {
    std::wstring devicesAndDrives = LoadShellString(
        L"@shell32.dll,-9339", L"Devices and drives");
    std::wstring freeSpace =
        LoadShellString(L"@shell32.dll,-9307", L"Free space");
    std::wstring removableDisk =
        LoadShellString(L"@shell32.dll,-9309", L"Removable Disk");
    std::wstring dvdDrive =
        LoadShellString(L"@shell32.dll,-9316", L"DVD Drive");
    std::wstring networkDrive =
        LoadShellString(L"@shell32.dll,-9319", L"Network Drive");
};

const ShellStrings& GetShellStrings() {
    static const ShellStrings strings;
    return strings;
}

bool IsAutoCheckSelectEnabled() {
    SHELLSTATE shellState{};
    SHGetSetSettings(&shellState, SSF_AUTOCHECKSELECT, FALSE);
    return shellState.fAutoCheckSelect;
}

std::wstring FormatByteSize(uint64_t byteCount) {
    wchar_t buffer[64]{};
    if (StrFormatByteSizeW(static_cast<LONGLONG>(byteCount), buffer,
                           ARRAYSIZE(buffer))) {
        return buffer;
    }

    return std::to_wstring(byteCount);
}

std::wstring GetDriveDisplayName(IShellItem* shellItem,
                                 std::wstring const& rootPath) {
    if (shellItem) {
        PWSTR displayName = nullptr;
        if (SUCCEEDED(shellItem->GetDisplayName(SIGDN_NORMALDISPLAY,
                                                &displayName)) &&
            displayName) {
            std::wstring result{displayName};
            CoTaskMemFree(displayName);
            if (!result.empty()) {
                return result;
            }
        }
    }

    return rootPath;
}

std::wstring GetShellItemTypeName(IShellItem2* shellItem) {
    if (!shellItem) {
        return {};
    }

    PWSTR typeName = nullptr;
    HRESULT result = shellItem->GetString(PKEY_ItemTypeText, &typeName);
    if (FAILED(result) || !typeName) {
        return {};
    }

    std::wstring value{typeName};
    CoTaskMemFree(typeName);
    return value;
}

std::wstring GetDriveTypeName(UINT driveType) {
    auto const& strings = GetShellStrings();
    switch (driveType) {
        case DRIVE_REMOVABLE:
            return strings.removableDisk;
        case DRIVE_CDROM:
            return strings.dvdDrive;
        case DRIVE_REMOTE:
            return strings.networkDrive;
        default:
            return {};
    }
}

struct ShellDriveIdentity {
    std::wstring parsingName;
    std::wstring fileSystemPath;
    DWORD categoryId = static_cast<DWORD>(-1);
};

std::vector<ShellDriveIdentity> EnumerateThisPcDevices() {
    std::vector<ShellDriveIdentity> candidates;

    PIDLIST_ABSOLUTE computerPidl = nullptr;
    HRESULT result = SHGetKnownFolderIDList(
        FOLDERID_ComputerFolder, KF_FLAG_DEFAULT, nullptr, &computerPidl);
    if (FAILED(result) || !computerPidl) {
        Wh_Log(L"Couldn't resolve the This PC PIDL: %08X", result);
        return candidates;
    }

    winrt::com_ptr<IShellFolder> desktopFolder;
    result = SHGetDesktopFolder(desktopFolder.put());
    if (FAILED(result)) {
        CoTaskMemFree(computerPidl);
        Wh_Log(L"Couldn't get the Desktop Shell folder: %08X", result);
        return candidates;
    }

    winrt::com_ptr<IShellFolder> computerFolder;
    result = desktopFolder->BindToObject(
        computerPidl, nullptr, IID_PPV_ARGS(computerFolder.put()));
    if (FAILED(result)) {
        CoTaskMemFree(computerPidl);
        Wh_Log(L"Couldn't bind to the This PC Shell folder: %08X", result);
        return candidates;
    }

    winrt::com_ptr<ICategoryProvider> categoryProvider;
    result = computerFolder->CreateViewObject(
        nullptr, IID_ICategoryProvider,
        reinterpret_cast<void**>(categoryProvider.put()));

    winrt::com_ptr<ICategorizer> categorizer;
    if (SUCCEEDED(result) && categoryProvider) {
        GUID unusedGuid{};
        SHCOLUMNID defaultColumn{};
        result = categoryProvider->GetDefaultCategory(&unusedGuid,
                                                      &defaultColumn);
        GUID categorizerGuid{};
        if (SUCCEEDED(result)) {
            result = categoryProvider->GetCategoryForSCID(&defaultColumn,
                                                          &categorizerGuid);
        }
        if (SUCCEEDED(result)) {
            result = categoryProvider->CreateCategory(
                &categorizerGuid, IID_ICategorizer,
                reinterpret_cast<void**>(categorizer.put()));
        }
    }
    if (!categorizer) {
        Wh_Log(L"Couldn't create the native This PC categorizer: %08X; "
               L"using file-system roots only",
               result);
    }

    winrt::com_ptr<IEnumIDList> items;
    result = computerFolder->EnumObjects(
        nullptr, SHCONTF_FOLDERS | SHCONTF_NONFOLDERS, items.put());
    if (FAILED(result)) {
        CoTaskMemFree(computerPidl);
        Wh_Log(L"Couldn't enumerate the This PC Shell folder: %08X", result);
        return candidates;
    }

    wchar_t systemRoot[MAX_PATH]{};
    if (GetWindowsDirectoryW(systemRoot, ARRAYSIZE(systemRoot))) {
        PathStripToRootW(systemRoot);
    }
    DWORD driveCategoryId = static_cast<DWORD>(-1);
    DWORD fallbackDriveCategoryId = static_cast<DWORD>(-1);

    while (true) {
        PITEMID_CHILD childPidl = nullptr;
        ULONG fetched = 0;
        result = items->Next(1, &childPidl, &fetched);
        if (result != S_OK || fetched != 1 || !childPidl) {
            break;
        }

        DWORD categoryId = static_cast<DWORD>(-1);
        if (categorizer) {
            PCUITEMID_CHILD childPidls[] = {childPidl};
            if (FAILED(categorizer->GetCategory(1, childPidls,
                                                &categoryId))) {
                categoryId = static_cast<DWORD>(-1);
            }
        }

        PIDLIST_ABSOLUTE absolutePidl = ILCombine(computerPidl, childPidl);
        CoTaskMemFree(childPidl);
        if (!absolutePidl) {
            continue;
        }

        ShellDriveIdentity candidate;
        candidate.categoryId = categoryId;
        wchar_t path[MAX_PATH]{};
        if (SHGetPathFromIDListW(absolutePidl, path) && PathIsRootW(path)) {
            candidate.fileSystemPath = path;
            if (categoryId != static_cast<DWORD>(-1)) {
                if (fallbackDriveCategoryId == static_cast<DWORD>(-1)) {
                    fallbackDriveCategoryId = categoryId;
                }
                if (systemRoot[0] && _wcsicmp(path, systemRoot) == 0) {
                    driveCategoryId = categoryId;
                }
            }
        }

        PWSTR parsingName = nullptr;
        if (SUCCEEDED(SHGetNameFromIDList(
                absolutePidl, SIGDN_DESKTOPABSOLUTEPARSING, &parsingName)) &&
            parsingName) {
            candidate.parsingName = parsingName;
            CoTaskMemFree(parsingName);
        } else {
            candidate.parsingName = candidate.fileSystemPath;
        }

        if (!candidate.parsingName.empty()) {
            candidates.push_back(std::move(candidate));
        }
        CoTaskMemFree(absolutePidl);
    }

    CoTaskMemFree(computerPidl);

    if (driveCategoryId == static_cast<DWORD>(-1)) {
        driveCategoryId = fallbackDriveCategoryId;
    }

    if (categorizer && driveCategoryId != static_cast<DWORD>(-1)) {
        std::erase_if(candidates, [&](ShellDriveIdentity const& candidate) {
            return candidate.categoryId != driveCategoryId;
        });
    } else {
        std::erase_if(candidates, [](ShellDriveIdentity const& candidate) {
            return candidate.fileSystemPath.empty();
        });
    }

    return candidates;
}

class UniqueBitmap {
  public:
    explicit UniqueBitmap(HBITMAP bitmap = nullptr) : bitmap_(bitmap) {}
    ~UniqueBitmap() {
        if (bitmap_) {
            DeleteObject(bitmap_);
        }
    }

    UniqueBitmap(UniqueBitmap const&) = delete;
    UniqueBitmap& operator=(UniqueBitmap const&) = delete;

    HBITMAP get() const {
        return bitmap_;
    }

  private:
    HBITMAP bitmap_;
};

// The card renders the icon at 48 effective (DIP) pixels; 96 covers up to
// 200% display scaling crisply without the memory cost of a full 256x256
// jumbo bitmap for every drive. IShellItemImageFactory::GetImage scales
// proportionally to whatever is actually available, so unlike the system
// image list's fixed-size slots (SHIL_JUMBO included), a drive whose icon
// has no 256px variant is never padded into an empty canvas at this size.
constexpr int kDriveIconTargetSize = 96;

bool PopulateDriveIcon(DriveInfo& drive, IShellItem* shellItem,
                       IWICImagingFactory* imagingFactory) {
    if (!shellItem || !imagingFactory) {
        return false;
    }

    winrt::com_ptr<IShellItemImageFactory> imageFactory;
    HRESULT result =
        shellItem->QueryInterface(IID_PPV_ARGS(imageFactory.put()));
    if (FAILED(result) || !imageFactory) {
        return false;
    }

    // SIIGBF_ICONONLY: an icon, never a thumbnail (matches what This PC
    // shows for drives). SIIGBF_BIGGERSIZEOK: don't force the Shell's own
    // GDI stretch blit when only a bigger cached image is on hand; the
    // pixels are read back at their actual returned size below, and WinUI
    // scales that down to the card's 48x48 slot itself.
    SIZE requestedSize{kDriveIconTargetSize, kDriveIconTargetSize};
    HBITMAP rawBitmap = nullptr;
    result = imageFactory->GetImage(
        requestedSize,
        static_cast<SIIGBF>(SIIGBF_ICONONLY | SIIGBF_BIGGERSIZEOK),
        &rawBitmap);
    if (FAILED(result) || !rawBitmap) {
        return false;
    }
    UniqueBitmap bitmap{rawBitmap};

    // GetImage's returned HBITMAP carries straight (non-premultiplied)
    // alpha, not premultiplied: telling WIC it was already premultiplied
    // skipped the multiply step the format converter below would otherwise
    // do, leaving edge pixels at full color intensity instead of fading
    // toward transparent, which showed up as a dark fringe/halo around
    // icons.
    winrt::com_ptr<IWICBitmap> source;
    result = imagingFactory->CreateBitmapFromHBITMAP(
        bitmap.get(), nullptr, WICBitmapUseAlpha, source.put());
    if (FAILED(result)) {
        return false;
    }

    winrt::com_ptr<IWICFormatConverter> converter;
    result = imagingFactory->CreateFormatConverter(converter.put());
    if (FAILED(result)) {
        return false;
    }
    result = converter->Initialize(
        source.get(), GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone,
        nullptr, 0, WICBitmapPaletteTypeCustom);
    if (FAILED(result)) {
        return false;
    }

    UINT width = 0;
    UINT height = 0;
    result = converter->GetSize(&width, &height);
    if (FAILED(result) || !width || !height || width > 256 || height > 256) {
        return false;
    }

    UINT stride = width * 4;
    UINT bufferSize = stride * height;
    std::vector<uint8_t> pixels(bufferSize);
    result = converter->CopyPixels(nullptr, stride, bufferSize, pixels.data());
    if (FAILED(result)) {
        return false;
    }

    drive.iconWidth = width;
    drive.iconHeight = height;
    drive.iconPixels = std::move(pixels);
    return true;
}

void UpdateDriveSpaceInformation(DriveInfo& drive) {
    drive.hasSpaceInformation = false;
    drive.percentUsed = 0;
    drive.spaceDescription.clear();
    if (drive.fileSystemPath.empty()) {
        return;
    }

    ULARGE_INTEGER availableToCaller{};
    ULARGE_INTEGER totalBytes{};
    ULARGE_INTEGER totalFreeBytes{};
    if (!GetDiskFreeSpaceExW(drive.fileSystemPath.c_str(),
                             &availableToCaller, &totalBytes,
                             &totalFreeBytes) ||
        !totalBytes.QuadPart) {
        return;
    }

    // availableToCaller, not totalFreeBytes: matches what This PC's own
    // space bar reflects, and differs from the whole-volume free space when
    // a disk quota applies to the calling user.
    drive.hasSpaceInformation = true;
    drive.percentUsed =
        100.0 - static_cast<double>(availableToCaller.QuadPart) * 100.0 /
                    static_cast<double>(totalBytes.QuadPart);
    drive.percentUsed = std::clamp(drive.percentUsed, 0.0, 100.0);
    drive.spaceDescription =
        GetShellStrings().freeSpace + L": " +
        FormatByteSize(availableToCaller.QuadPart) + L" / " +
        FormatByteSize(totalBytes.QuadPart);
}

DriveSnapshot EnumerateDrives(IWICImagingFactory* imagingFactory) {
    DriveSnapshot drives;
    auto shellDrives = EnumerateThisPcDevices();
    for (auto const& shellDrive : shellDrives) {
        DriveInfo drive;
        drive.rootPath = shellDrive.parsingName;
        drive.fileSystemPath = shellDrive.fileSystemPath;
        drive.driveType = drive.fileSystemPath.empty()
                              ? DRIVE_UNKNOWN
                              : GetDriveTypeW(drive.fileSystemPath.c_str());

        winrt::com_ptr<IShellItem> shellItem;
        SHCreateItemFromParsingName(drive.rootPath.c_str(), nullptr,
                                    IID_PPV_ARGS(shellItem.put()));
        winrt::com_ptr<IShellItem2> shellItem2;
        if (shellItem) {
            shellItem->QueryInterface(IID_PPV_ARGS(shellItem2.put()));
        }
        drive.displayName = GetDriveDisplayName(shellItem.get(),
                                                drive.rootPath);
        drive.typeName = drive.fileSystemPath.empty()
                             ? GetShellItemTypeName(shellItem2.get())
                             : GetDriveTypeName(drive.driveType);
        drive.canStartWinUiDrag = !drive.fileSystemPath.empty();

        if (shellItem) {
            SFGAOF attributes = 0;
            if (SUCCEEDED(shellItem->GetAttributes(SFGAO_DROPTARGET,
                                                   &attributes))) {
                drive.canAcceptDrop = (attributes & SFGAO_DROPTARGET) != 0;
            }
        }
        PopulateDriveIcon(drive, shellItem.get(), imagingFactory);
        UpdateDriveSpaceInformation(drive);

        drives.push_back(std::move(drive));
    }

    std::sort(drives.begin(), drives.end(),
              [](DriveInfo const& left, DriveInfo const& right) {
                  if (left.fileSystemPath.empty() !=
                      right.fileSystemPath.empty()) {
                      return !left.fileSystemPath.empty();
                  }

                  // Lettered volumes follow their Shell roots so labels don't
                  // move a newly connected E:\ drive ahead of C:\ and D:\.
                  if (!left.fileSystemPath.empty()) {
                      return StrCmpLogicalW(left.fileSystemPath.c_str(),
                                            right.fileSystemPath.c_str()) < 0;
                  }

                  return StrCmpLogicalW(left.displayName.c_str(),
                                        right.displayName.c_str()) < 0;
              });
    return drives;
}

bool DriveSnapshotsEqual(DriveSnapshot const& left,
                         DriveSnapshot const& right) {
    if (left.size() != right.size()) {
        return false;
    }

    for (size_t i = 0; i < left.size(); ++i) {
        auto const& a = left[i];
        auto const& b = right[i];
        if (a.rootPath != b.rootPath ||
            a.fileSystemPath != b.fileSystemPath ||
            a.displayName != b.displayName ||
            a.typeName != b.typeName ||
            a.spaceDescription != b.spaceDescription ||
            a.driveType != b.driveType ||
            a.iconWidth != b.iconWidth || a.iconHeight != b.iconHeight ||
            a.iconPixels != b.iconPixels ||
            a.hasSpaceInformation != b.hasSpaceInformation ||
            a.canStartWinUiDrag != b.canStartWinUiDrag ||
            a.canAcceptDrop != b.canAcceptDrop ||
            // Rounded to the nearest percentage point rather than compared
            // exactly: percentUsed is a continuous value derived from free
            // space in bytes, so during any real disk activity it's almost
            // never bit-identical between snapshots, forcing a grid rebuild
            // on essentially every debounce tick for a change too small to
            // see on the progress bar. spaceDescription (the visible text)
            // is still compared exactly above.
            std::llround(a.percentUsed) != std::llround(b.percentUsed)) {
            return false;
        }
    }

    return true;
}

std::shared_ptr<const DriveSnapshot> GetLatestDriveSnapshot() {
    std::lock_guard lock(g_refreshMutex);
    return g_latestDriveSnapshot;
}

void AddRegisteredWindow(HWND window) {
    std::lock_guard lock(g_registeredWindowsMutex);
    if (std::find(g_registeredWindows.begin(), g_registeredWindows.end(),
                  window) == g_registeredWindows.end()) {
        g_registeredWindows.push_back(window);
    }
}

void RemoveRegisteredWindow(HWND window) {
    std::lock_guard lock(g_registeredWindowsMutex);
    std::erase(g_registeredWindows, window);
}

UINT GetApplyDriveSnapshotMessage() {
    static const UINT message = RegisterWindowMessageW(
        L"Windhawk_ApplyDriveSnapshot_" WH_MOD_ID);
    return message;
}

UINT GetFocusDriveRenameMessage() {
    static const UINT message = RegisterWindowMessageW(
        L"Windhawk_FocusDriveRename_" WH_MOD_ID);
    return message;
}

UINT GetInvokeDrivePropertiesMessage() {
    static const UINT message = RegisterWindowMessageW(
        L"Windhawk_InvokeDriveProperties_" WH_MOD_ID);
    return message;
}

UINT GetToggleDevicesExpandedMessage() {
    static const UINT message = RegisterWindowMessageW(
        L"Windhawk_ToggleDevicesExpanded_" WH_MOD_ID);
    return message;
}

void DriveRefreshWorkerProc() {
    HRESULT comResult = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    bool uninitializeCom = SUCCEEDED(comResult);
    uint64_t completedGeneration = 0;

    {
        winrt::com_ptr<IWICImagingFactory> imagingFactory;
        bool imagingFailureLogged = false;

        auto ensureImageResources = [&] {
            if (!imagingFactory) {
                HRESULT result = CoCreateInstance(
                    CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER,
                    IID_PPV_ARGS(imagingFactory.put()));
                if (FAILED(result) && !imagingFailureLogged) {
                    imagingFailureLogged = true;
                    Wh_Log(L"Couldn't create the WIC imaging factory: %08X",
                           result);
                }
            }
        };

        auto createSnapshot = [&](uint32_t refreshKinds, bool forceFull) {
            if (!forceFull &&
                !(refreshKinds & kDriveRefreshTopology)) {
                if (auto previous = GetLatestDriveSnapshot()) {
                    DriveSnapshot drives = *previous;
                    for (auto& drive : drives) {
                        UpdateDriveSpaceInformation(drive);
                    }
                    return std::make_shared<const DriveSnapshot>(
                        std::move(drives));
                }
            }

            ensureImageResources();
            return std::make_shared<const DriveSnapshot>(
                EnumerateDrives(imagingFactory.get()));
        };

        auto applySnapshot = [&](std::shared_ptr<const DriveSnapshot> snapshot,
                                 uint64_t targetGeneration,
                                 bool completeRequest) {
            bool snapshotIsCurrent = false;
            bool snapshotChanged = false;
            {
                std::lock_guard lock(g_refreshMutex);
                if (completeRequest) {
                    completedGeneration = targetGeneration;
                }
                if (!g_refreshWorkerStopping &&
                    targetGeneration == g_requestedRefreshGeneration) {
                    snapshotIsCurrent = true;
                    if (completeRequest) {
                        g_requestedRefreshKinds = 0;
                    }
                    snapshotChanged =
                        !g_latestDriveSnapshot ||
                        !DriveSnapshotsEqual(*g_latestDriveSnapshot,
                                             *snapshot);
                    if (snapshotChanged) {
                        g_latestDriveSnapshot = std::move(snapshot);
                    }
                }
            }

            if (!snapshotChanged || g_unloading.load()) {
                return snapshotIsCurrent;
            }

            std::vector<HWND> windows;
            {
                std::lock_guard lock(g_registeredWindowsMutex);
                windows = g_registeredWindows;
            }
            for (HWND window : windows) {
                if (IsWindow(window)) {
                    PostMessageW(window, GetApplyDriveSnapshotMessage(), 0,
                                 0);
                }
            }
            return snapshotIsCurrent;
        };

        while (true) {
            uint64_t targetGeneration = 0;
            uint32_t refreshKinds = 0;
            {
                std::unique_lock lock(g_refreshMutex);
                g_refreshCondition.wait(lock, [&] {
                    return g_refreshWorkerStopping ||
                           g_requestedRefreshGeneration >
                               completedGeneration;
                });
                if (g_refreshWorkerStopping) {
                    break;
                }

                targetGeneration = g_requestedRefreshGeneration;
                while (g_refreshCondition.wait_for(
                    lock, std::chrono::milliseconds(200), [&] {
                        return g_refreshWorkerStopping ||
                               g_requestedRefreshGeneration !=
                                   targetGeneration;
                    })) {
                    if (g_refreshWorkerStopping) {
                        break;
                    }
                    targetGeneration = g_requestedRefreshGeneration;
                }
                if (g_refreshWorkerStopping) {
                    break;
                }
                refreshKinds = g_requestedRefreshKinds;
            }

            if (!applySnapshot(createSnapshot(refreshKinds, false),
                               targetGeneration, true)) {
                continue;
            }

            if (!(refreshKinds & kDriveRefreshTopology)) {
                continue;
            }

            // The Shell namespace can lag behind a device removal event. A
            // single quiet-period confirmation prevents stale device cards.
            {
                std::unique_lock lock(g_refreshMutex);
                if (g_refreshCondition.wait_for(
                        lock, std::chrono::milliseconds(800), [&] {
                            return g_refreshWorkerStopping ||
                                   g_requestedRefreshGeneration !=
                                       targetGeneration;
                        })) {
                    if (g_refreshWorkerStopping) {
                        break;
                    }
                    continue;
                }
            }

            applySnapshot(createSnapshot(kDriveRefreshTopology, true),
                          targetGeneration, false);
        }
    }

    if (uninitializeCom) {
        CoUninitialize();
    }
}

bool StartDriveRefreshWorker() {
    try {
        std::lock_guard lock(g_refreshMutex);
        if (g_refreshWorker) {
            return true;
        }
        g_refreshWorkerStopping = false;
        g_requestedRefreshGeneration = 0;
        g_requestedRefreshKinds = 0;
        g_latestDriveSnapshot.reset();
        g_refreshWorker.emplace(DriveRefreshWorkerProc);
        return true;
    } catch (...) {
        Wh_Log(L"Failed to start the drive refresh worker");
        return false;
    }
}

void StopDriveRefreshWorker() {
    {
        std::lock_guard lock(g_refreshMutex);
        if (!g_refreshWorker) {
            return;
        }
        g_refreshWorkerStopping = true;
    }
    g_refreshCondition.notify_all();

    if (g_refreshWorker->joinable()) {
        g_refreshWorker->join();
    }

    std::lock_guard lock(g_refreshMutex);
    g_refreshWorker.reset();
}

void RequestDriveRefresh(uint32_t refreshKinds) {
    if (!refreshKinds) {
        return;
    }

    if (g_unloading.load()) {
        return;
    }

    {
        std::lock_guard lock(g_refreshMutex);
        if (g_refreshWorkerStopping) {
            return;
        }
        g_requestedRefreshKinds |= refreshKinds;
        ++g_requestedRefreshGeneration;
    }
    g_refreshCondition.notify_one();
}

void RequestInitialDriveSnapshot() {
    if (g_unloading.load()) {
        return;
    }

    bool notify = false;
    {
        std::lock_guard lock(g_refreshMutex);
        if (!g_refreshWorkerStopping && !g_latestDriveSnapshot &&
            g_requestedRefreshGeneration == 0) {
            g_requestedRefreshKinds |= kDriveRefreshTopology;
            ++g_requestedRefreshGeneration;
            notify = true;
        }
    }

    if (notify) {
        g_refreshCondition.notify_one();
    }
}

void RememberNavigationControllerForCurrentThread(void* implementation) {
    if (!implementation || g_unloading.load()) {
        return;
    }

    IFileExplorerNavigationControllerAbi* controller = nullptr;
    HRESULT result = reinterpret_cast<IUnknown*>(implementation)->QueryInterface(
        kIidFileExplorerNavigationController,
        reinterpret_cast<void**>(&controller));
    if (FAILED(result) || !controller) {
        Wh_Log(L"Couldn't capture the tab navigation controller: %08X",
               result);
        return;
    }

    IFileExplorerNavigationControllerAbi* previous = nullptr;
    {
        std::lock_guard lock(g_navigationControllersMutex);
        auto& entry = g_navigationControllersByThread[GetCurrentThreadId()];
        previous = entry;
        if (previous == controller) {
            controller->Release();
            return;
        }
        entry = controller;
    }

    if (previous) {
        previous->Release();
    }

}

IFileExplorerNavigationControllerAbi*
GetNavigationControllerForCurrentThread() {
    std::lock_guard lock(g_navigationControllersMutex);
    auto it = g_navigationControllersByThread.find(GetCurrentThreadId());
    if (it == g_navigationControllersByThread.end() || !it->second) {
        return nullptr;
    }

    it->second->AddRef();
    return it->second;
}

void ReleaseNavigationControllerForCurrentThread() {
    IFileExplorerNavigationControllerAbi* controller = nullptr;
    {
        std::lock_guard lock(g_navigationControllersMutex);
        auto it = g_navigationControllersByThread.find(GetCurrentThreadId());
        if (it == g_navigationControllersByThread.end()) {
            return;
        }
        controller = it->second;
        g_navigationControllersByThread.erase(it);
    }

    if (controller) {
        controller->Release();
    }
}

HWND GetExplorerWindowForCurrentThread() {
    for (auto const& [window, registrationId] :
         g_windowNotificationRegistrations) {
        (void)registrationId;
        if (IsWindow(window)) {
            return window;
        }
    }

    DWORD currentThreadId = GetCurrentThreadId();
    for (HWND window : GetFileExplorerWindows()) {
        if (GetWindowThreadProcessId(window, nullptr) == currentThreadId) {
            return window;
        }
    }

    return nullptr;
}

class OpenContextMenuScope {
   public:
    OpenContextMenuScope() {
        g_openContextMenuCount.fetch_add(1);
    }

    ~OpenContextMenuScope() {
        g_openContextMenuCount.fetch_sub(1);
    }
};

// Separate from OpenContextMenuScope: WM_CANCELMODE can dismiss a tracked
// popup menu, but InvokeCommand can run modal Shell UI (Format, Properties,
// Eject confirmations) that it cannot touch. Unload waits this phase out
// instead of trying to cancel it.
class CommandInvocationScope {
   public:
    CommandInvocationScope() {
        g_pendingCommandInvocations.fetch_add(1);
    }

    ~CommandInvocationScope() {
        g_pendingCommandInvocations.fetch_sub(1);
    }
};

// Same shape as CommandInvocationScope: IShellFolder::SetNameOf on a volume
// needs elevation, so it can show a modal UAC/error dialog, and
// ShellExecuteExW's fallback path can too. Both pump messages, so unload
// waits this out instead of letting the mod be unmapped while the call is
// still on the UI thread's stack.
class ShellUiCallScope {
   public:
    ShellUiCallScope() {
        g_pendingShellUiCalls.fetch_add(1);
    }

    ~ShellUiCallScope() {
        g_pendingShellUiCalls.fetch_sub(1);
    }
};

std::wstring GetFileSystemRootPath(std::wstring const& parsingName) {
    PIDLIST_ABSOLUTE absolutePidl = nullptr;
    HRESULT result = SHParseDisplayName(parsingName.c_str(), nullptr,
                                        &absolutePidl, 0, nullptr);
    if (FAILED(result) || !absolutePidl) {
        return {};
    }

    wchar_t path[MAX_PATH]{};
    bool isRoot = SHGetPathFromIDListW(absolutePidl, path) &&
                  PathIsRootW(path);
    CoTaskMemFree(absolutePidl);
    return isRoot ? std::wstring{path} : std::wstring{};
}

bool BindDriveToParent(std::wstring const& rootPath,
                       PIDLIST_ABSOLUTE* absolutePidl,
                       winrt::com_ptr<IShellFolder>& parentFolder,
                       PCUITEMID_CHILD* childPidl) {
    *absolutePidl = nullptr;
    *childPidl = nullptr;

    HRESULT result = SHParseDisplayName(rootPath.c_str(), nullptr,
                                        absolutePidl, 0, nullptr);
    if (FAILED(result) || !*absolutePidl) {
        Wh_Log(L"Couldn't resolve the drive PIDL: %08X", result);
        return false;
    }

    result = SHBindToParent(*absolutePidl, IID_PPV_ARGS(parentFolder.put()),
                            childPidl);
    if (FAILED(result) || !parentFolder || !*childPidl) {
        CoTaskMemFree(*absolutePidl);
        *absolutePidl = nullptr;
        Wh_Log(L"Couldn't bind the drive parent: %08X", result);
        return false;
    }

    return true;
}

std::wstring GetDriveEditingName(std::wstring const& rootPath) {
    PIDLIST_ABSOLUTE absolutePidl = nullptr;
    winrt::com_ptr<IShellFolder> parentFolder;
    PCUITEMID_CHILD childPidl = nullptr;
    if (!BindDriveToParent(rootPath, &absolutePidl, parentFolder,
                           &childPidl)) {
        return {};
    }

    STRRET name{};
    auto flags = static_cast<SHGDNF>(SHGDN_INFOLDER | SHGDN_FOREDITING);
    HRESULT result = parentFolder->GetDisplayNameOf(childPidl, flags, &name);
    wchar_t buffer[256]{};
    if (SUCCEEDED(result)) {
        result = StrRetToBufW(&name, childPidl, buffer, ARRAYSIZE(buffer));
    }
    CoTaskMemFree(absolutePidl);

    if (FAILED(result)) {
        Wh_Log(L"Couldn't get the drive editing name: %08X", result);
        return {};
    }
    return buffer;
}

HRESULT RenameDriveWithShell(HWND owner, std::wstring const& rootPath,
                             std::wstring const& newName) {
    ShellUiCallScope shellUiScope;
    PIDLIST_ABSOLUTE absolutePidl = nullptr;
    winrt::com_ptr<IShellFolder> parentFolder;
    PCUITEMID_CHILD childPidl = nullptr;
    if (!BindDriveToParent(rootPath, &absolutePidl, parentFolder,
                           &childPidl)) {
        return E_FAIL;
    }

    SFGAOF attributes = SFGAO_CANRENAME;
    HRESULT result = parentFolder->GetAttributesOf(1, &childPidl, &attributes);
    if (SUCCEEDED(result) && !(attributes & SFGAO_CANRENAME)) {
        result = HRESULT_FROM_WIN32(ERROR_NOT_SUPPORTED);
    }

    PITEMID_CHILD renamedPidl = nullptr;
    if (SUCCEEDED(result)) {
        auto flags =
            static_cast<SHGDNF>(SHGDN_INFOLDER | SHGDN_FOREDITING);
        result = parentFolder->SetNameOf(owner, childPidl, newName.c_str(),
                                         flags, &renamedPidl);
    }

    if (renamedPidl) {
        CoTaskMemFree(renamedPidl);
    }
    CoTaskMemFree(absolutePidl);
    return result;
}

struct CoTaskMemPidlDeleter {
    void operator()(ITEMIDLIST* pidl) const {
        CoTaskMemFree(pidl);
    }
};

using UniqueAbsolutePidl =
    std::unique_ptr<ITEMIDLIST, CoTaskMemPidlDeleter>;

struct MenuDeleter {
    void operator()(HMENU__* menu) const {
        DestroyMenu(reinterpret_cast<HMENU>(menu));
    }
};

using UniqueMenu = std::unique_ptr<HMENU__, MenuDeleter>;

bool ShowDriveContextMenu(HWND owner,
                          std::vector<std::wstring> const& rootPaths,
                          POINT screenPoint,
                          bool* renameRequested,
                          std::wstring_view commandToInvoke = {}) {
    *renameRequested = false;
    if (!owner || rootPaths.empty() || g_unloading.load()) {
        return false;
    }

    // Covers the whole function, not just TrackPopupMenuEx/InvokeCommand
    // (which additionally get their own narrower scopes below):
    // GetUIObjectOf/QueryContextMenu run third-party shell extensions that
    // can also pump before either of those points is reached.
    ShellUiCallScope shellUiScope;

    std::vector<UniqueAbsolutePidl> absolutePidls;
    std::vector<PCUITEMID_CHILD> childPidls;
    winrt::com_ptr<IShellFolder> parentFolder;
    for (auto const& rootPath : rootPaths) {
        PIDLIST_ABSOLUTE rawPidl = nullptr;
        HRESULT result = SHParseDisplayName(rootPath.c_str(), nullptr,
                                            &rawPidl, 0, nullptr);
        if (FAILED(result) || !rawPidl) {
            Wh_Log(L"Couldn't resolve the drive context-menu PIDL: %08X",
                   result);
            return false;
        }

        UniqueAbsolutePidl absolutePidl{rawPidl};
        PCUITEMID_CHILD childPidl = nullptr;
        if (!parentFolder) {
            result = SHBindToParent(
                absolutePidl.get(), IID_PPV_ARGS(parentFolder.put()),
                &childPidl);
        } else {
            childPidl = ILFindLastID(absolutePidl.get());
        }
        if (FAILED(result) || !parentFolder || !childPidl) {
            Wh_Log(L"Couldn't bind the drive context-menu parent: %08X",
                   result);
            return false;
        }

        childPidls.push_back(childPidl);
        absolutePidls.push_back(std::move(absolutePidl));
    }

    winrt::com_ptr<IContextMenu> contextMenu;
    HRESULT result = parentFolder->GetUIObjectOf(
        owner, static_cast<UINT>(childPidls.size()), childPidls.data(),
        IID_IContextMenu, nullptr,
        reinterpret_cast<void**>(contextMenu.put()));
    if (FAILED(result) || !contextMenu) {
        Wh_Log(L"Couldn't create the drive context menu: %08X", result);
        return false;
    }

    UniqueMenu menu{CreatePopupMenu()};
    if (!menu) {
        return false;
    }

    constexpr UINT kFirstCommandId = 1;
    constexpr UINT kLastCommandId = 0x7FFF;
    bool shiftDown = (GetKeyState(VK_SHIFT) & 0x8000) != 0;
    UINT queryFlags = CMF_NORMAL | CMF_EXPLORE;
    if (rootPaths.size() == 1) {
        queryFlags |= CMF_CANRENAME;
    }
    if (shiftDown) {
        queryFlags |= CMF_EXTENDEDVERBS;
    }

    result = contextMenu->QueryContextMenu(menu.get(), 0, kFirstCommandId,
                                            kLastCommandId, queryFlags);
    if (FAILED(result)) {
        Wh_Log(L"Couldn't populate the drive context menu: %08X", result);
        return false;
    }

    UINT command = 0;
    if (commandToInvoke.empty()) {
        winrt::com_ptr<IContextMenu2> contextMenu2;
        winrt::com_ptr<IContextMenu3> contextMenu3;
        contextMenu->QueryInterface(IID_PPV_ARGS(contextMenu2.put()));
        contextMenu->QueryInterface(IID_PPV_ARGS(contextMenu3.put()));
        g_trackedContextMenu2 = contextMenu2.get();
        g_trackedContextMenu3 = contextMenu3.get();

        {
            // Scoped tightly around the tracked popup: this is the phase
            // DismissOpenContextMenus's WM_CANCELMODE can actually dismiss.
            OpenContextMenuScope openMenuScope;
            command = TrackPopupMenuEx(
                menu.get(),
                TPM_RETURNCMD | TPM_LEFTALIGN | TPM_LEFTBUTTON |
                    TPM_RIGHTBUTTON,
                screenPoint.x, screenPoint.y, owner, nullptr);
        }

        g_trackedContextMenu3 = nullptr;
        g_trackedContextMenu2 = nullptr;
    } else {
        std::wstring commandVerb{commandToInvoke};
        UINT commandCount = HRESULT_CODE(result);
        for (UINT offset = 0; offset < commandCount; ++offset) {
            wchar_t canonicalVerb[128]{};
            HRESULT verbResult = contextMenu->GetCommandString(
                offset, GCS_VERBW, nullptr,
                reinterpret_cast<LPSTR>(canonicalVerb),
                ARRAYSIZE(canonicalVerb));
            if (SUCCEEDED(verbResult) &&
                _wcsicmp(canonicalVerb, commandVerb.c_str()) == 0) {
                command = kFirstCommandId + offset;
                break;
            }
        }
        if (!command) {
            Wh_Log(L"Drive context-menu verb wasn't found: %.*s",
                   static_cast<int>(commandToInvoke.size()),
                   commandToInvoke.data());
        }
    }

    // menu (a UniqueMenu) stays alive past this point, through
    // InvokeCommand below: some shell extensions expect the HMENU to still
    // exist while their verb runs.
    if (commandToInvoke.empty()) {
        PostMessageW(owner, WM_NULL, 0, 0);
    } else if (!command) {
        // An explicitly requested verb that isn't in the menu was never
        // invoked, so this isn't a success the caller should act on (e.g.
        // InvokeFocusedDriveProperties falling through to normal Alt+Enter
        // handling instead of reporting that Properties was shown).
        return false;
    }

    if (command && !g_unloading.load()) {
        wchar_t canonicalVerb[128]{};
        HRESULT verbResult = contextMenu->GetCommandString(
            command - kFirstCommandId, GCS_VERBW, nullptr,
            reinterpret_cast<LPSTR>(canonicalVerb),
            ARRAYSIZE(canonicalVerb));
        if (SUCCEEDED(verbResult) &&
            _wcsicmp(canonicalVerb, L"rename") == 0 &&
            rootPaths.size() == 1) {
            *renameRequested = true;
            // Catches up a refresh that RefreshDevicesGridPreservingState
            // deferred while this menu was open (see
            // g_openContextMenuCount there) -- otherwise it stays lost,
            // since the window message that would have applied it was
            // already dispatched and handled while deferring.
            if (owner) {
                PostMessageW(owner, GetApplyDriveSnapshotMessage(), 0, 0);
            }
            return true;
        }

        CMINVOKECOMMANDINFOEX invokeInfo{};
        invokeInfo.cbSize = sizeof(invokeInfo);
        invokeInfo.fMask = CMIC_MASK_UNICODE | CMIC_MASK_PTINVOKE;
        if (shiftDown) {
            invokeInfo.fMask |= CMIC_MASK_SHIFT_DOWN;
        }
        if ((GetKeyState(VK_CONTROL) & 0x8000) != 0) {
            invokeInfo.fMask |= CMIC_MASK_CONTROL_DOWN;
        }
        invokeInfo.hwnd = owner;
        invokeInfo.lpVerb = MAKEINTRESOURCEA(command - kFirstCommandId);
        invokeInfo.lpVerbW = MAKEINTRESOURCEW(command - kFirstCommandId);
        invokeInfo.nShow = SW_SHOWNORMAL;
        invokeInfo.ptInvoke = screenPoint;
        {
            CommandInvocationScope commandScope;
            result = contextMenu->InvokeCommand(
                reinterpret_cast<CMINVOKECOMMANDINFO*>(&invokeInfo));
        }
        if (FAILED(result)) {
            Wh_Log(L"Drive context-menu command failed: %08X (verb=%s, "
                   L"verbResult=%08X)",
                   result, canonicalVerb[0] ? canonicalVerb : L"<unknown>",
                   verbResult);
        }
    }

    // Same catch-up as the rename-requested return above, for the
    // interactive-popup and other-command-invoked paths.
    if (owner) {
        PostMessageW(owner, GetApplyDriveSnapshotMessage(), 0, 0);
    }
    return true;
}

void SelectDriveCard(muxc::GridViewItem const& item) {
    if (item.IsSelected()) {
        return;
    }

    if (auto grid = muxc::ItemsControl::ItemsControlFromItemContainer(item)
                        .try_as<muxc::GridView>()) {
        grid.SelectedItems().Clear();
    }
    item.IsSelected(true);
}

std::vector<std::wstring> GetSelectedDriveRootPaths(
    muxc::GridViewItem const& item) {
    std::vector<std::wstring> rootPaths;
    if (auto grid = muxc::ItemsControl::ItemsControlFromItemContainer(item)
                        .try_as<muxc::GridView>()) {
        for (auto const& selected : grid.SelectedItems()) {
            if (auto selectedItem = selected.try_as<muxc::GridViewItem>()) {
                rootPaths.emplace_back(winrt::unbox_value<winrt::hstring>(
                    selectedItem.Tag()));
            }
        }
    }

    if (rootPaths.empty()) {
        rootPaths.emplace_back(
            winrt::unbox_value<winrt::hstring>(item.Tag()));
    }
    return rootPaths;
}

POINT GetDriveKeyboardMenuPoint(muxc::GridViewItem const& item,
                                HWND owner) {
    POINT screenPoint{};
    try {
        auto xamlRoot = item.XamlRoot();
        auto rootContent = xamlRoot ? xamlRoot.Content() : nullptr;
        if (rootContent) {
            auto transform = item.TransformToVisual(rootContent);
            auto center = transform.TransformPoint(
                {static_cast<float>(item.ActualWidth() / 2),
                 static_cast<float>(item.ActualHeight() / 2)});
            double scale = xamlRoot.RasterizationScale();
            screenPoint.x = static_cast<LONG>(center.X * scale);
            screenPoint.y = static_cast<LONG>(center.Y * scale);
            if (ClientToScreen(owner, &screenPoint)) {
                return screenPoint;
            }
        }
    } catch (...) {
    }

    GetCursorPos(&screenPoint);
    return screenPoint;
}

bool ExecuteShellParsingName(std::wstring const& parsingName) {
    ShellUiCallScope shellUiScope;
    PIDLIST_ABSOLUTE absolutePidl = nullptr;
    HRESULT result = SHParseDisplayName(parsingName.c_str(), nullptr,
                                        &absolutePidl, 0, nullptr);
    if (FAILED(result) || !absolutePidl) {
        Wh_Log(L"Couldn't resolve the Shell item for fallback navigation: "
               L"%08X",
               result);
        return false;
    }

    SHELLEXECUTEINFOW executeInfo{};
    executeInfo.cbSize = sizeof(executeInfo);
    executeInfo.fMask = SEE_MASK_IDLIST;
    executeInfo.lpIDList = absolutePidl;
    executeInfo.nShow = SW_SHOWNORMAL;
    bool succeeded = ShellExecuteExW(&executeInfo);
    if (!succeeded) {
        Wh_Log(L"Shell item fallback navigation failed: %08X",
               GetLastError());
    }
    CoTaskMemFree(absolutePidl);
    return succeeded;
}

void OpenDriveCard(muxc::GridViewItem const& item) {
    if (g_unloading.load()) {
        return;
    }

    try {
        auto rootPath = winrt::unbox_value<winrt::hstring>(item.Tag());
        winrt::com_ptr<IFileExplorerNavigationControllerAbi> controller;
        for (auto const& state : *g_driveCardEventStates) {
            if (state.item.get() == item && state.navigationController) {
                controller.copy_from(state.navigationController);
                break;
            }
        }

        // The section can be created before the Explorer finishes binding its
        // navigation controller. Resolve it again at invocation time so cards
        // created during that interval still navigate in their owning tab.
        if (!controller) {
            controller.attach(GetNavigationControllerForCurrentThread());
        }

        if (controller) {
            HRESULT result = controller->SubmitAddressBarText(
                reinterpret_cast<HSTRING>(winrt::get_abi(rootPath)));
            if (SUCCEEDED(result)) {
                return;
            }

            Wh_Log(L"Current-tab drive navigation failed: %08X", result);
        } else {
            Wh_Log(L"No current-tab controller was available for %s",
                   rootPath.c_str());
        }

        ExecuteShellParsingName(rootPath.c_str());
    } catch (...) {
        Wh_Log(L"Couldn't open the selected drive: %08X",
               winrt::to_hresult().value);
    }
}

DriveCardEventState* FindDriveCardState(
    muxc::GridViewItem const& item) {
    for (auto& state : *g_driveCardEventStates) {
        if (state.item.get() == item) {
            return &state;
        }
    }
    return nullptr;
}

void UpdateDriveSelectionCheckBox(DriveCardEventState& state) {
    auto item = state.item.get();
    auto driveIcon = state.driveIcon;
    auto checkBox = state.selectionCheckBox;
    if (!item || !driveIcon || !checkBox) {
        return;
    }

    bool selected = item.IsSelected();
    // Sharing the 48-pixel icon slot preserves text width while matching the
    // Explorer convention of revealing selection controls on interaction.
    bool showCheckBox = state.selectionCheckBoxesEnabled &&
                        (state.pointerOver || selected);
    checkBox.IsChecked(selected);
    checkBox.Visibility(showCheckBox ? mux::Visibility::Visible
                                     : mux::Visibility::Collapsed);
    driveIcon.Visibility(showCheckBox ? mux::Visibility::Collapsed
                                      : mux::Visibility::Visible);
}

void DriveCard_PointerEntered(
    winrt::Windows::Foundation::IInspectable const& sender,
    muxi::PointerRoutedEventArgs const&) {
    if (auto state =
            FindDriveCardState(sender.as<muxc::GridViewItem>())) {
        state->pointerOver = true;
        UpdateDriveSelectionCheckBox(*state);
    }
}

void DriveCard_PointerExited(
    winrt::Windows::Foundation::IInspectable const& sender,
    muxi::PointerRoutedEventArgs const&) {
    if (auto state =
            FindDriveCardState(sender.as<muxc::GridViewItem>())) {
        state->pointerOver = false;
        UpdateDriveSelectionCheckBox(*state);
    }
}

void DriveCard_IsSelectedChanged(mux::DependencyObject const& sender,
                                 mux::DependencyProperty const&) {
    if (auto state =
            FindDriveCardState(sender.as<muxc::GridViewItem>())) {
        UpdateDriveSelectionCheckBox(*state);
    }
}

void DriveSelectionCheckBox_Click(
    winrt::Windows::Foundation::IInspectable const& sender,
    mux::RoutedEventArgs const&) {
    auto checkBox = sender.as<muxc::CheckBox>();
    for (auto& state : *g_driveCardEventStates) {
        if (state.selectionCheckBox != checkBox) {
            continue;
        }

        if (auto item = state.item.get()) {
            auto isChecked = checkBox.IsChecked();
            item.IsSelected(isChecked && isChecked.Value());
        }
        return;
    }
}

void UpdateDriveSelectionCheckBoxesForCurrentThread() {
    bool enabled = IsAutoCheckSelectEnabled();
    for (auto& state : *g_driveCardEventStates) {
        state.selectionCheckBoxesEnabled = enabled;
        UpdateDriveSelectionCheckBox(state);
    }
}

DriveCardEventState* FindDriveRenameState(muxc::TextBox const& renameBox) {
    for (auto& state : *g_driveCardEventStates) {
        if (state.renameBox == renameBox) {
            return &state;
        }
    }
    return nullptr;
}

muxc::GridViewItem FindFocusedDriveCard() {
    for (auto const& state : *g_driveCardEventStates) {
        auto item = state.item.get();
        if (!item) {
            continue;
        }
        // FocusState() reflects the item's own last-known focus state, which
        // can go stale once focus moves outside this XamlRoot entirely (a
        // different island, or a Win32 control in the frame). Checking the
        // FocusManager ties this to what's actually focused right now.
        if (auto xamlRoot = item.XamlRoot()) {
            if (muxi::FocusManager::GetFocusedElement(xamlRoot) == item) {
                return item;
            }
        }
    }
    return nullptr;
}

void CompleteDriveRename(muxc::TextBox const& renameBox, bool commit,
                         bool restoreCardFocus = true) {
    auto state = FindDriveRenameState(renameBox);
    if (!state || !state->renaming || state->renameCompleting) {
        return;
    }
    // Collapsing renameBox/moving focus below queues an asynchronous
    // LostFocus that can be dispatched while RenameDriveWithShell's
    // SetNameOf is still pumping (e.g. an elevation dialog) -- renaming
    // alone can't guard against that reentrant call, since it deliberately
    // stays true for that whole window. renameCompleting is cleared
    // alongside it further down.
    state->renameCompleting = true;

    // state->renaming is cleared further down, only after the Shell call
    // below returns. RenameDriveWithShell can pump messages, and
    // IsDriveCardRenamingInGrid() must keep reporting this card busy for
    // that whole window so a reentrant refresh doesn't rebuild the grid
    // while the call is in flight.
    state->renameFocusPending = false;
    auto item = state->item.get();
    auto title = state->title;
    bool editorHadFocus =
        renameBox.FocusState() != mux::FocusState::Unfocused;
    std::wstring newName{renameBox.Text()};
    std::wstring originalName;
    if (auto tag = renameBox.Tag()) {
        originalName = winrt::unbox_value<winrt::hstring>(tag);
    }

    renameBox.Visibility(mux::Visibility::Collapsed);
    if (title) {
        title.Visibility(mux::Visibility::Visible);
    }
    if (item) {
        auto parsingName = winrt::unbox_value<winrt::hstring>(item.Tag());
        item.CanDrag(!GetFileSystemRootPath(parsingName.c_str()).empty());
        if (restoreCardFocus && editorHadFocus) {
            item.Focus(mux::FocusState::Programmatic);
        }
    }

    HWND owner = GetExplorerWindowForCurrentThread();
    bool renameSubmitted =
        commit && item && newName != originalName && !g_unloading.load();
    if (renameSubmitted) {
        auto rootPath = winrt::unbox_value<winrt::hstring>(item.Tag());
        HRESULT result =
            RenameDriveWithShell(owner, rootPath.c_str(), newName);
        if (SUCCEEDED(result)) {
            RequestDriveRefresh();
        } else {
            Wh_Log(L"Drive rename failed for %s: %08X", rootPath.c_str(),
                   result);
        }
    }

    // Re-resolve rather than reuse the pointer from before the Shell call:
    // RenameDriveWithShell can pump messages, and a reentrant path unrelated
    // to the renaming-guarded grid refresh (e.g. unload tearing the list
    // down) could have erased this node while it was in flight.
    state = FindDriveRenameState(renameBox);
    if (state) {
        state->renaming = false;
        state->renameCompleting = false;
    }

    // A grid refresh arriving while this card was renaming was deferred (see
    // RefreshDevicesGridPreservingState) so it wouldn't destroy the editor.
    // Catch it up now that renaming is over, whether committed or cancelled.
    if (owner) {
        PostMessageW(owner, GetApplyDriveSnapshotMessage(), 0, 0);
    }
}

void CompleteOtherDriveRenames(muxc::GridViewItem const& activeItem) {
    std::vector<muxc::TextBox> renameBoxes;
    for (auto const& state : *g_driveCardEventStates) {
        auto item = state.item.get();
        if (state.renaming && item && item != activeItem && state.renameBox) {
            renameBoxes.push_back(state.renameBox);
        }
    }

    for (auto const& renameBox : renameBoxes) {
        CompleteDriveRename(renameBox, true, false);
    }
}

void FocusPendingDriveRenameForCurrentThread() {
    for (auto& state : *g_driveCardEventStates) {
        if (!state.renameFocusPending) {
            continue;
        }

        state.renameFocusPending = false;
        auto renameBox = state.renameBox;
        if (!state.renaming || !renameBox ||
            renameBox.Visibility() != mux::Visibility::Visible) {
            continue;
        }

        if (renameBox.Focus(mux::FocusState::Programmatic)) {
            renameBox.SelectAll();
        } else {
            Wh_Log(L"Couldn't focus the inline drive rename editor");
        }
    }
}

void QueueDriveRenameFocus(DriveCardEventState& state) {
    state.renameFocusPending = true;
    HWND owner = GetExplorerWindowForCurrentThread();
    if (owner && PostMessageW(owner, GetFocusDriveRenameMessage(), 0, 0)) {
        return;
    }

    state.renameFocusPending = false;
    if (state.renameBox.Focus(mux::FocusState::Programmatic)) {
        state.renameBox.SelectAll();
    } else {
        Wh_Log(L"Couldn't queue focus for the drive rename editor");
    }
}

void BeginDriveRename(muxc::GridViewItem const& item) {
    auto state = FindDriveCardState(item);
    if (!state) {
        Wh_Log(L"Couldn't start drive rename: card state wasn't found");
        return;
    }
    if (!state->renameBox || !state->title || g_unloading.load()) {
        Wh_Log(L"Couldn't start drive rename: editor elements weren't "
               L"available");
        return;
    }
    if (state->renaming) {
        QueueDriveRenameFocus(*state);
        return;
    }

    CompleteOtherDriveRenames(item);

    auto rootPath = winrt::unbox_value<winrt::hstring>(item.Tag());
    std::wstring editingName;
    {
        ShellUiCallScope shellUiScope;
        editingName = GetDriveEditingName(rootPath.c_str());
    }

    // Re-resolve: CompleteOtherDriveRenames (RenameDriveWithShell can pump
    // via a modal UAC/error dialog) and GetDriveEditingName (Shell/COM calls
    // that dispatch sent messages on this STA) can both let something else
    // run on this thread, including a reentrant teardown that erases this
    // node.
    state = FindDriveCardState(item);
    if (!state || g_unloading.load()) {
        return;
    }
    auto renameBox = state->renameBox;
    auto title = state->title;
    if (!renameBox || !title) {
        return;
    }

    renameBox.Text(editingName);
    renameBox.Tag(winrt::box_value(winrt::hstring{editingName}));
    state->renaming = true;
    item.CanDrag(false);
    title.Visibility(mux::Visibility::Collapsed);
    renameBox.Visibility(mux::Visibility::Visible);
    QueueDriveRenameFocus(*state);
}

void DriveRenameBox_KeyDown(
    winrt::Windows::Foundation::IInspectable const& sender,
    muxi::KeyRoutedEventArgs const& args) {
    if (args.Key() != winrt::Windows::System::VirtualKey::Enter &&
        args.Key() != winrt::Windows::System::VirtualKey::Escape) {
        return;
    }

    args.Handled(true);
    try {
        CompleteDriveRename(sender.as<muxc::TextBox>(),
                            args.Key() ==
                                winrt::Windows::System::VirtualKey::Enter,
                            true);
    } catch (...) {
        Wh_Log(L"Couldn't complete the drive rename: %08X",
               winrt::to_hresult().value);
    }
}

void DriveRenameBox_LostFocus(
    winrt::Windows::Foundation::IInspectable const& sender,
    mux::RoutedEventArgs const&) {
    try {
        CompleteDriveRename(sender.as<muxc::TextBox>(), true, false);
    } catch (...) {
        Wh_Log(L"Couldn't complete the drive rename: %08X",
               winrt::to_hresult().value);
    }
}

void DriveCard_DoubleTapped(
    winrt::Windows::Foundation::IInspectable const& sender,
    muxi::DoubleTappedRoutedEventArgs const& args) {
    args.Handled(true);
    OpenDriveCard(sender.as<muxc::GridViewItem>());
}

void DriveCard_KeyDown(
    winrt::Windows::Foundation::IInspectable const& sender,
    muxi::KeyRoutedEventArgs const& args) {
    try {
        auto item = sender.as<muxc::GridViewItem>();
        if (args.Key() == winrt::Windows::System::VirtualKey::F2) {
            args.Handled(true);
            BeginDriveRename(item);
            return;
        }

        bool shiftDown = (GetKeyState(VK_SHIFT) & 0x8000) != 0;
        bool altDown = args.KeyStatus().IsMenuKeyDown ||
                       (GetKeyState(VK_MENU) & 0x8000) != 0;
        bool contextMenuKey =
            args.Key() == winrt::Windows::System::VirtualKey::Application ||
            (args.Key() == winrt::Windows::System::VirtualKey::F10 &&
             shiftDown);
        // AltGr registers as right-Alt plus left-Ctrl, so
        // DriveKeyboardGetMessageHookProc's Ctrl-excluding Alt+Enter check
        // never matches it; this routed-input path is what catches that
        // case.
        bool propertiesKey = args.Key() ==
                                  winrt::Windows::System::VirtualKey::Enter &&
                              altDown;
        if (contextMenuKey || propertiesKey) {
            args.Handled(true);
            SelectDriveCard(item);
            HWND owner = GetExplorerWindowForCurrentThread();
            if (!owner) {
                Wh_Log(
                    L"Couldn't locate the Explorer window for the drive "
                    L"menu");
                return;
            }

            auto rootPaths = GetSelectedDriveRootPaths(item);
            POINT screenPoint = GetDriveKeyboardMenuPoint(item, owner);
            bool renameRequested = false;
            ShowDriveContextMenu(owner, rootPaths, screenPoint,
                                 &renameRequested,
                                 propertiesKey ? L"properties" : L"");
            if (renameRequested && !g_unloading.load()) {
                BeginDriveRename(item);
            }
            return;
        }

        if (args.Key() != winrt::Windows::System::VirtualKey::Enter) {
            return;
        }

        args.Handled(true);
        OpenDriveCard(item);
    } catch (...) {
        Wh_Log(L"Couldn't handle the drive card key press: %08X",
               winrt::to_hresult().value);
    }
}

void DriveCard_RightTapped(
    winrt::Windows::Foundation::IInspectable const& sender,
    muxi::RightTappedRoutedEventArgs const& args) {
    if (g_unloading.load()) {
        return;
    }

    try {
        auto item = sender.as<muxc::GridViewItem>();
        SelectDriveCard(item);
        HWND owner = GetExplorerWindowForCurrentThread();
        POINT screenPoint{};
        if (!owner || !GetCursorPos(&screenPoint)) {
            Wh_Log(L"Couldn't locate the Explorer window for the drive menu");
            return;
        }

        args.Handled(true);
        auto rootPaths = GetSelectedDriveRootPaths(item);
        bool renameRequested = false;
        ShowDriveContextMenu(owner, rootPaths, screenPoint,
                             &renameRequested);
        if (renameRequested && !g_unloading.load()) {
            BeginDriveRename(item);
        }
    } catch (...) {
        Wh_Log(L"Couldn't show the drive context menu: %08X",
               winrt::to_hresult().value);
    }
}

bool InvokeFocusedDriveProperties(HWND owner) {
    if (!owner || g_unloading.load()) {
        return false;
    }

    try {
        if (auto item = FindFocusedDriveCard()) {
            SelectDriveCard(item);
            auto rootPaths = GetSelectedDriveRootPaths(item);
            POINT screenPoint = GetDriveKeyboardMenuPoint(item, owner);
            bool renameRequested = false;
            return ShowDriveContextMenu(owner, rootPaths, screenPoint,
                                        &renameRequested, L"properties");
        }
    } catch (...) {
        Wh_Log(L"Couldn't invoke drive properties from Alt+Enter: %08X",
               winrt::to_hresult().value);
    }

    return false;
}

// The decrement must happen under g_dragPreparationMutex, not just the
// notify: WaitForDriveDragPreparations() checks the predicate and starts
// waiting on the condition variable while holding that same mutex, so a
// decrement that races the lock can drop the wakeup and leave it waiting
// forever.
void FinishDriveDragPreparation() {
    bool finished;
    {
        std::lock_guard lock(g_dragPreparationMutex);
        finished = g_pendingDragPreparations.fetch_sub(1) == 1;
    }
    if (finished) {
        g_dragPreparationCondition.notify_all();
    }
}

winrt::fire_and_forget PopulateDriveDragData(
    mux::DragStartingEventArgs args,
    std::vector<std::wstring> rootPaths) {
    struct CompletionScope {
        ~CompletionScope() {
            FinishDriveDragPreparation();
        }
    } completionScope;

    mux::DragOperationDeferral deferral{nullptr};
    try {
        deferral = args.GetDeferral();
        auto storageItems = winrt::single_threaded_vector<
            winrt::Windows::Storage::IStorageItem>();
        for (auto const& parsingName : rootPaths) {
            try {
                auto rootPath = GetFileSystemRootPath(parsingName);
                if (rootPath.empty()) {
                    continue;
                }
                auto folder = co_await
                    winrt::Windows::Storage::StorageFolder::
                        GetFolderFromPathAsync(rootPath);
                if (folder) {
                    storageItems.Append(folder);
                }
            } catch (...) {
                Wh_Log(L"Couldn't resolve %s for WinUI drag: %08X",
                       parsingName.c_str(), winrt::to_hresult().value);
            }
        }

        if (g_unloading.load() || storageItems.Size() == 0) {
            // DropCompleted, the only other place that clears
            // g_driveCardDragInProgress, is documented to fire when a
            // drag-and-drop operation *with this element as the source
            // ends* -- a drag cancelled here never becomes one, so it's
            // never raised and the flag would otherwise stay stuck true.
            g_driveCardDragInProgress = false;
            args.Cancel(true);
        } else {
            args.Data().SetStorageItems(storageItems);
            args.AllowedOperations(
                winrt::Windows::ApplicationModel::DataTransfer::
                    DataPackageOperation::Link);
        }
    } catch (...) {
        auto error = winrt::to_hresult().value;
        g_driveCardDragInProgress = false;
        try {
            args.Cancel(true);
        } catch (...) {
        }
        Wh_Log(L"Couldn't populate the WinUI drive drag data: %08X",
               error);
    }

    // A fire_and_forget coroutine that lets an exception escape terminates
    // Explorer (unhandled_exception() calls winrt::terminate()). Complete()
    // can throw on a stale deferral, so it needs its own guard too.
    try {
        if (deferral) {
            deferral.Complete();
        }
    } catch (...) {
        Wh_Log(L"Couldn't complete the WinUI drive drag deferral: %08X",
               winrt::to_hresult().value);
    }
}

void DriveCard_DragStarting(
    winrt::Windows::Foundation::IInspectable const& sender,
    mux::DragStartingEventArgs const& args) {
    if (g_unloading.load()) {
        args.Cancel(true);
        return;
    }

    try {
        auto item = sender.as<muxc::GridViewItem>();
        SelectDriveCard(item);

        std::vector<std::wstring> rootPaths;
        if (auto grid = muxc::ItemsControl::ItemsControlFromItemContainer(item)
                            .try_as<muxc::GridView>()) {
            for (auto const& selected : grid.SelectedItems()) {
                if (auto selectedItem = selected.try_as<muxc::GridViewItem>()) {
                    rootPaths.emplace_back(
                        winrt::unbox_value<winrt::hstring>(selectedItem.Tag()));
                }
            }
        }
        if (rootPaths.empty()) {
            rootPaths.emplace_back(
                winrt::unbox_value<winrt::hstring>(item.Tag()));
        }
        g_pendingDragPreparations.fetch_add(1);
        g_driveCardDragInProgress = true;
        try {
            PopulateDriveDragData(args, std::move(rootPaths));
        } catch (...) {
            FinishDriveDragPreparation();
            g_driveCardDragInProgress = false;
            throw;
        }
    } catch (...) {
        args.Cancel(true);
        Wh_Log(L"Couldn't start the WinUI drive drag: %08X",
               winrt::to_hresult().value);
    }
}

void DriveCard_DropCompleted(
    winrt::Windows::Foundation::IInspectable const&,
    mux::DropCompletedEventArgs const&) {
    g_driveCardDragInProgress = false;
}

winrt::Windows::ApplicationModel::DataTransfer::DataPackageOperation
GetDriveDropOperation(
    winrt::Windows::ApplicationModel::DataTransfer::DataPackageView const&
        dataView) {
    using winrt::Windows::ApplicationModel::DataTransfer::DataPackageOperation;

    bool controlDown = (GetKeyState(VK_CONTROL) & 0x8000) != 0;
    bool shiftDown = (GetKeyState(VK_SHIFT) & 0x8000) != 0;
    if (shiftDown && !controlDown) {
        return DataPackageOperation::Move;
    }
    if (controlDown) {
        return DataPackageOperation::Copy;
    }

    auto requested = dataView.RequestedOperation();
    return requested == DataPackageOperation::Move
               ? DataPackageOperation::Move
               : DataPackageOperation::Copy;
}

HRESULT TransferStorageItemsToDrive(
    HWND owner,
    winrt::Windows::Foundation::Collections::IVectorView<
        winrt::Windows::Storage::IStorageItem> const& storageItems,
    std::wstring const& targetRoot,
    winrt::Windows::ApplicationModel::DataTransfer::DataPackageOperation
        operation) {
    winrt::com_ptr<IShellItem> targetFolder;
    HRESULT result = SHCreateItemFromParsingName(
        targetRoot.c_str(), nullptr, IID_PPV_ARGS(targetFolder.put()));
    if (FAILED(result)) {
        return result;
    }

    winrt::com_ptr<IFileOperation> fileOperation;
    result = CoCreateInstance(CLSID_FileOperation, nullptr,
                              CLSCTX_INPROC_SERVER,
                              IID_PPV_ARGS(fileOperation.put()));
    if (FAILED(result)) {
        return result;
    }

    result = fileOperation->SetOwnerWindow(owner);
    if (FAILED(result)) {
        return result;
    }
    result = fileOperation->SetOperationFlags(
        FOF_ALLOWUNDO | FOF_NOCONFIRMMKDIR | FOFX_SHOWELEVATIONPROMPT |
        FOFX_ADDUNDORECORD);
    if (FAILED(result)) {
        return result;
    }

    UINT queuedItems = 0;
    for (auto const& storageItem : storageItems) {
        auto path = storageItem.Path();
        // A volume root is never a valid copy/move source here -- every
        // drive card is also a drop target, so without this a card dragged
        // onto another card would queue the whole volume for
        // IFileOperation::CopyItem/MoveItem.
        if (path.empty() || PathIsRootW(path.c_str())) {
            continue;
        }

        winrt::com_ptr<IShellItem> sourceItem;
        result = SHCreateItemFromParsingName(
            path.c_str(), nullptr, IID_PPV_ARGS(sourceItem.put()));
        if (FAILED(result)) {
            Wh_Log(L"Couldn't resolve dropped item %s: %08X", path.c_str(),
                   result);
            continue;
        }

        if (operation == winrt::Windows::ApplicationModel::DataTransfer::
                             DataPackageOperation::Move) {
            result = fileOperation->MoveItem(sourceItem.get(),
                                             targetFolder.get(), nullptr,
                                             nullptr);
        } else {
            result = fileOperation->CopyItem(sourceItem.get(),
                                             targetFolder.get(), nullptr,
                                             nullptr);
        }
        if (FAILED(result)) {
            return result;
        }
        ++queuedItems;
    }

    if (!queuedItems) {
        return HRESULT_FROM_WIN32(ERROR_NO_MORE_ITEMS);
    }

    result = fileOperation->PerformOperations();
    if (FAILED(result)) {
        return result;
    }

    BOOL aborted = FALSE;
    result = fileOperation->GetAnyOperationsAborted(&aborted);
    if (FAILED(result)) {
        return result;
    }
    return aborted ? HRESULT_FROM_WIN32(ERROR_CANCELLED) : S_OK;
}

void DriveCard_DragOver(
    winrt::Windows::Foundation::IInspectable const&,
    mux::DragEventArgs const& args) {
    if (g_unloading.load() || g_driveCardDragInProgress) {
        return;
    }

    try {
        auto dataView = args.DataView();
        if (!dataView.Contains(
                winrt::Windows::ApplicationModel::DataTransfer::
                    StandardDataFormats::StorageItems())) {
            return;
        }

        args.AcceptedOperation(GetDriveDropOperation(dataView));
        args.Handled(true);
    } catch (...) {
        Wh_Log(L"Couldn't accept the drive drop: %08X",
               winrt::to_hresult().value);
    }
}

// Same reasoning as FinishDriveDragPreparation: decrement under
// g_dropOperationMutex so the wakeup can't race
// WaitForDriveDropOperations()'s wait registration and get lost.
void FinishDriveDropOperation() {
    bool finished;
    {
        std::lock_guard lock(g_dropOperationMutex);
        finished = g_pendingDropOperations.fetch_sub(1) == 1;
    }
    if (finished) {
        g_dropOperationCondition.notify_all();
    }
}

winrt::fire_and_forget PerformDriveDrop(
    mux::DragEventArgs args,
    std::wstring targetRoot,
    winrt::Windows::ApplicationModel::DataTransfer::DataPackageOperation
        operation) {
    struct CompletionScope {
        ~CompletionScope() {
            FinishDriveDropOperation();
        }
    } completionScope;

    mux::DragOperationDeferral deferral{nullptr};
    try {
        deferral = args.GetDeferral();
        auto storageItems = co_await args.DataView().GetStorageItemsAsync();
        if (g_unloading.load()) {
            args.AcceptedOperation(
                winrt::Windows::ApplicationModel::DataTransfer::
                    DataPackageOperation::None);
        } else {
            HRESULT result = TransferStorageItemsToDrive(
                GetExplorerWindowForCurrentThread(), storageItems,
                targetRoot, operation);
            if (SUCCEEDED(result)) {
                RequestDriveRefresh(kDriveRefreshCapacity);
            } else if (result != HRESULT_FROM_WIN32(ERROR_CANCELLED)) {
                args.AcceptedOperation(
                    winrt::Windows::ApplicationModel::DataTransfer::
                        DataPackageOperation::None);
                Wh_Log(L"Drive drop failed for %s: %08X",
                       targetRoot.c_str(), result);
            }
        }
    } catch (...) {
        auto error = winrt::to_hresult().value;
        try {
            args.AcceptedOperation(
                winrt::Windows::ApplicationModel::DataTransfer::
                    DataPackageOperation::None);
        } catch (...) {
        }
        Wh_Log(L"Couldn't complete the drive drop: %08X", error);
    }

    // Same reasoning as PopulateDriveDragData: an exception escaping this
    // fire_and_forget coroutine terminates Explorer, and Complete() itself
    // can throw on a stale deferral.
    try {
        if (deferral) {
            deferral.Complete();
        }
    } catch (...) {
        Wh_Log(L"Couldn't complete the drive drop deferral: %08X",
               winrt::to_hresult().value);
    }
}

void DriveCard_Drop(
    winrt::Windows::Foundation::IInspectable const& sender,
    mux::DragEventArgs const& args) {
    if (g_unloading.load() || g_driveCardDragInProgress) {
        return;
    }

    try {
        auto dataView = args.DataView();
        if (!dataView.Contains(
                winrt::Windows::ApplicationModel::DataTransfer::
                    StandardDataFormats::StorageItems())) {
            return;
        }

        auto item = sender.as<muxc::GridViewItem>();
        auto targetRoot = winrt::unbox_value<winrt::hstring>(item.Tag());
        auto operation = GetDriveDropOperation(dataView);
        args.AcceptedOperation(operation);
        args.Handled(true);

        g_pendingDropOperations.fetch_add(1);
        try {
            PerformDriveDrop(args, targetRoot.c_str(), operation);
        } catch (...) {
            FinishDriveDropOperation();
            throw;
        }
    } catch (...) {
        Wh_Log(L"Couldn't start the drive drop: %08X",
               winrt::to_hresult().value);
    }
}

void WaitForDriveDragPreparations() {
    std::unique_lock lock(g_dragPreparationMutex);
    unsigned logIterations = 0;
    while (!g_dragPreparationCondition.wait_for(
        lock, std::chrono::milliseconds(200), [] {
            return g_pendingDragPreparations.load() == 0;
        })) {
        if (++logIterations % 25 == 0) {
            Wh_Log(L"Waiting for %d drive drag preparation(s) to finish",
                   g_pendingDragPreparations.load());
        }
    }
}

void WaitForDriveDropOperations() {
    std::unique_lock lock(g_dropOperationMutex);
    unsigned logIterations = 0;
    while (!g_dropOperationCondition.wait_for(
        lock, std::chrono::milliseconds(200), [] {
            return g_pendingDropOperations.load() == 0;
        })) {
        if (++logIterations % 25 == 0) {
            Wh_Log(L"Waiting for %d drive drop operation(s) to finish",
                   g_pendingDropOperations.load());
        }
    }
}

void ClearDriveCardEventHandlers(DriveCardEventState const& state) {
    if (auto item = state.item.get()) {
        try {
            item.DoubleTapped(state.doubleTappedToken);
            item.RemoveHandler(mux::UIElement::KeyDownEvent(),
                               state.keyDownHandler);
            item.RightTapped(state.rightTappedToken);
            item.DragStarting(state.dragStartingToken);
            item.DragEnter(state.dragEnterToken);
            item.DragOver(state.dragOverToken);
            item.Drop(state.dropToken);
            item.DropCompleted(state.dropCompletedToken);
            item.PointerEntered(state.pointerEnteredToken);
            item.PointerExited(state.pointerExitedToken);
            item.UnregisterPropertyChangedCallback(
                muxcp::SelectorItem::IsSelectedProperty(),
                state.selectedChangedToken);
        } catch (...) {
            Wh_Log(L"Drive card event cleanup failed: %08X",
                   winrt::to_hresult().value);
        }
    }

    if (auto checkBox = state.selectionCheckBox) {
        try {
            checkBox.Click(state.selectionCheckBoxClickToken);
        } catch (...) {
            Wh_Log(L"Drive selection check box cleanup failed: %08X",
                   winrt::to_hresult().value);
        }
    }

    if (auto renameBox = state.renameBox) {
        try {
            renameBox.KeyDown(state.renameKeyDownToken);
            renameBox.LostFocus(state.renameLostFocusToken);
        } catch (...) {
            Wh_Log(L"Drive rename event cleanup failed: %08X",
                   winrt::to_hresult().value);
        }
    }

    if (state.navigationController) {
        state.navigationController->Release();
    }
}

void ClearDriveCardEventHandlersForCurrentThread() {
    for (auto const& state : *g_driveCardEventStates) {
        ClearDriveCardEventHandlers(state);
    }

    g_driveCardEventStates->clear();
}

void PruneExpiredDriveCardEventHandlersForCurrentThread() {
    for (auto it = g_driveCardEventStates->begin();
         it != g_driveCardEventStates->end();) {
        if (it->item.get()) {
            ++it;
            continue;
        }

        ClearDriveCardEventHandlers(*it);
        it = g_driveCardEventStates->erase(it);
    }
}

muxm::ImageSource CreateDriveImageSource(DriveInfo const& drive) {
    if (!drive.iconWidth || !drive.iconHeight || drive.iconPixels.empty()) {
        return nullptr;
    }

    try {
        muxmi::WriteableBitmap bitmap(
            static_cast<int32_t>(drive.iconWidth),
            static_cast<int32_t>(drive.iconHeight));
        auto pixelBuffer = bitmap.PixelBuffer();
        if (pixelBuffer.Capacity() < drive.iconPixels.size()) {
            return nullptr;
        }

        BYTE* destination = nullptr;
        winrt::check_hresult(
            pixelBuffer
                .as<::Windows::Storage::Streams::IBufferByteAccess>()
                ->Buffer(&destination));
        if (!destination) {
            return nullptr;
        }

        std::memcpy(destination, drive.iconPixels.data(),
                    drive.iconPixels.size());
        bitmap.Invalidate();
        return bitmap.as<muxm::ImageSource>();
    } catch (...) {
        Wh_Log(L"Couldn't create the WinUI drive image: %08X",
               winrt::to_hresult().value);
        return nullptr;
    }
}

// AddHandler projects routed-event delegates as Object. Keep the boxed object
// so RemoveHandler receives the exact same instance during unload.
winrt::Windows::Foundation::IInspectable RetainRoutedEventHandler(
    muxi::PointerEventHandler const& handler) {
    return winrt::box_value(handler);
}

winrt::Windows::Foundation::IInspectable RetainRoutedEventHandler(
    muxi::KeyEventHandler const& handler) {
    return winrt::box_value(handler);
}

muxc::GridViewItem CreateDriveCard(
    DriveInfo const& drive,
    IFileExplorerNavigationControllerAbi* navigationController,
    bool selectionCheckBoxesEnabled) {
    muxc::GridViewItem card;
    card.Width(264);
    // MinHeight, not a fixed Height: matches the native Quick access tile's
    // own ContentGrid (fixed Width, MinHeight only, Height left at its
    // default Auto), confirmed against a live UWPSpy tree dump of that
    // control. Letting height grow from content is what keeps a taller
    // BodyStrongTextBlockStyle/CaptionTextBlockStyle (system text-size
    // accessibility setting turned up) from clipping instead of just
    // overflowing the fixed box.
    card.MinHeight(82);
    card.Margin(mux::Thickness{0, 0, 20, 3});
    card.Padding(mux::Thickness{10, 8, 12, 8});
    card.CornerRadius(mux::CornerRadius{8, 8, 8, 8});
    card.HorizontalContentAlignment(mux::HorizontalAlignment::Stretch);
    card.VerticalContentAlignment(mux::VerticalAlignment::Stretch);
    card.Tag(winrt::box_value(winrt::hstring{drive.rootPath}));
    card.CanDrag(drive.canStartWinUiDrag);
    card.AllowDrop(drive.canAcceptDrop);

    muxc::Grid layout;

    muxc::ColumnDefinition iconColumn;
    iconColumn.Width(mux::GridLength{48, mux::GridUnitType::Pixel});
    muxc::ColumnDefinition textColumn;
    textColumn.Width(mux::GridLength{1, mux::GridUnitType::Star});
    layout.ColumnDefinitions().Append(iconColumn);
    layout.ColumnDefinitions().Append(textColumn);

    muxc::Grid iconHost;
    iconHost.Width(48);
    iconHost.Height(48);
    iconHost.HorizontalAlignment(mux::HorizontalAlignment::Left);
    iconHost.VerticalAlignment(mux::VerticalAlignment::Center);
    muxc::Grid::SetColumn(iconHost, 0);

    mux::UIElement driveIcon{nullptr};

    if (auto imageSource = CreateDriveImageSource(drive)) {
        muxc::Image image;
        image.Width(48);
        image.Height(48);
        image.Stretch(muxm::Stretch::Uniform);
        image.Source(imageSource);
        driveIcon = image;
        iconHost.Children().Append(image);
    } else {
        muxc::FontIcon fallbackIcon;
        fallbackIcon.Glyph(L"\xEDA2");
        fallbackIcon.FontFamily(muxm::FontFamily{L"Segoe Fluent Icons"});
        fallbackIcon.FontSize(32);
        fallbackIcon.HorizontalAlignment(mux::HorizontalAlignment::Center);
        fallbackIcon.VerticalAlignment(mux::VerticalAlignment::Center);
        ApplyBrushIfAvailable(fallbackIcon, L"TextFillColorPrimaryBrush");
        driveIcon = fallbackIcon;
        iconHost.Children().Append(fallbackIcon);
    }

    muxc::CheckBox selectionCheckBox;
    selectionCheckBox.Width(20);
    selectionCheckBox.Height(20);
    selectionCheckBox.MinWidth(0);
    selectionCheckBox.MinHeight(0);
    selectionCheckBox.Padding({0, 0, 0, 0});
    selectionCheckBox.HorizontalAlignment(mux::HorizontalAlignment::Center);
    selectionCheckBox.VerticalAlignment(mux::VerticalAlignment::Center);
    selectionCheckBox.IsTabStop(false);
    selectionCheckBox.Visibility(mux::Visibility::Collapsed);
    muxc::Grid::SetColumn(selectionCheckBox, 0);
    muxc::Canvas::SetZIndex(selectionCheckBox, 1);

    muxc::StackPanel details;
    details.Margin(mux::Thickness{12, 0, 0, 0});
    details.VerticalAlignment(mux::VerticalAlignment::Center);
    muxc::Grid::SetColumn(details, 1);

    muxc::Grid titleHost;

    muxc::TextBlock title;
    title.Text(drive.displayName);
    ApplyBodyStrongTextStyle(title);
    title.TextTrimming(mux::TextTrimming::CharacterEllipsis);
    ApplyBrushIfAvailable(title, L"TextFillColorPrimaryBrush");

    muxc::TextBox renameBox;
    renameBox.Height(30);
    renameBox.MaxLength(32);
    renameBox.Padding(mux::Thickness{4, 0, 4, 0});
    // Matches title (the label it replaces while editing): otherwise, once
    // BodyStrongTextBlockStyle resolves, the editor stops following the
    // system text-size accessibility setting the label does.
    ApplyBodyStrongTextStyle(renameBox);
    renameBox.HorizontalAlignment(mux::HorizontalAlignment::Stretch);
    renameBox.VerticalAlignment(mux::VerticalAlignment::Center);
    renameBox.Visibility(mux::Visibility::Collapsed);

    titleHost.Children().Append(title);
    titleHost.Children().Append(renameBox);
    details.Children().Append(titleHost);

    if (!drive.typeName.empty()) {
        muxc::TextBlock type;
        type.Text(drive.typeName);
        ApplyCaptionTextStyle(type);
        type.Margin(mux::Thickness{0, 2, 0, 0});
        type.TextTrimming(mux::TextTrimming::CharacterEllipsis);
        ApplyBrushIfAvailable(type, L"TextFillColorSecondaryBrush");
        details.Children().Append(type);
    }

    if (drive.hasSpaceInformation) {
        muxc::ProgressBar progress;
        progress.Minimum(0);
        progress.Maximum(100);
        progress.Value(drive.percentUsed);
        progress.Height(4);
        progress.Margin(mux::Thickness{0, 7, 0, 3});
        if (drive.percentUsed >= 90) {
            ApplyBrushIfAvailable(progress, L"SystemFillColorCriticalBrush");
        }
        details.Children().Append(progress);

        muxc::TextBlock space;
        space.Text(drive.spaceDescription);
        ApplyCaptionTextStyle(space);
        space.TextTrimming(mux::TextTrimming::CharacterEllipsis);
        ApplyBrushIfAvailable(space, L"TextFillColorSecondaryBrush");
        details.Children().Append(space);
    }

    layout.Children().Append(iconHost);
    layout.Children().Append(selectionCheckBox);
    layout.Children().Append(details);
    card.Content(layout);
    muxc::ToolTipService::SetToolTip(
        card, winrt::box_value(winrt::hstring{
                  drive.fileSystemPath.empty() ? drive.displayName
                                               : drive.fileSystemPath}));

    auto doubleTappedToken = card.DoubleTapped(DriveCard_DoubleTapped);
    auto keyDownHandler =
        RetainRoutedEventHandler(muxi::KeyEventHandler{DriveCard_KeyDown});
    card.AddHandler(mux::UIElement::KeyDownEvent(), keyDownHandler, true);
    auto rightTappedToken = card.RightTapped(DriveCard_RightTapped);
    auto dragStartingToken = card.DragStarting(DriveCard_DragStarting);
    auto dragEnterToken = card.DragEnter(DriveCard_DragOver);
    auto dragOverToken = card.DragOver(DriveCard_DragOver);
    auto dropToken = card.Drop(DriveCard_Drop);
    auto dropCompletedToken = card.DropCompleted(DriveCard_DropCompleted);
    auto pointerEnteredToken = card.PointerEntered(DriveCard_PointerEntered);
    auto pointerExitedToken = card.PointerExited(DriveCard_PointerExited);
    auto selectionCheckBoxClickToken =
        selectionCheckBox.Click(DriveSelectionCheckBox_Click);
    auto selectedChangedToken = card.RegisterPropertyChangedCallback(
        muxcp::SelectorItem::IsSelectedProperty(),
        mux::DependencyPropertyChangedCallback{DriveCard_IsSelectedChanged});
    auto renameKeyDownToken = renameBox.KeyDown(DriveRenameBox_KeyDown);
    auto renameLostFocusToken = renameBox.LostFocus(DriveRenameBox_LostFocus);
    if (navigationController) {
        navigationController->AddRef();
    }
    // Designated initializers: with 20+ positional fields, adding or
    // reordering one would otherwise silently shift every field after it.
    g_driveCardEventStates->push_back({
        .item = winrt::make_weak(card),
        .title = title,
        .renameBox = renameBox,
        .driveIcon = driveIcon,
        .selectionCheckBox = selectionCheckBox,
        .doubleTappedToken = doubleTappedToken,
        .keyDownHandler = keyDownHandler,
        .rightTappedToken = rightTappedToken,
        .dragStartingToken = dragStartingToken,
        .dragEnterToken = dragEnterToken,
        .dragOverToken = dragOverToken,
        .dropToken = dropToken,
        .dropCompletedToken = dropCompletedToken,
        .pointerEnteredToken = pointerEnteredToken,
        .pointerExitedToken = pointerExitedToken,
        .selectionCheckBoxClickToken = selectionCheckBoxClickToken,
        .selectedChangedToken = selectedChangedToken,
        .renameKeyDownToken = renameKeyDownToken,
        .renameLostFocusToken = renameLostFocusToken,
        .navigationController = navigationController,
        .selectionCheckBoxesEnabled = selectionCheckBoxesEnabled,
    });
    UpdateDriveSelectionCheckBox(g_driveCardEventStates->back());

    return card;
}

muxc::GridView FindDevicesGrid(muxc::StackPanel const& panel) {
    for (auto const& child : panel.Children()) {
        auto section = child.try_as<muxc::StackPanel>();
        if (!section || section.Name() != kDevicesSectionName) {
            continue;
        }

        for (auto const& sectionChild : section.Children()) {
            auto element =
                sectionChild.try_as<mux::FrameworkElement>();
            if (element && element.Name() == kDevicesGridName) {
                return element.try_as<muxc::GridView>();
            }
        }
    }

    return nullptr;
}

void PopulateDevicesGrid(muxc::GridView const& grid,
                          DriveSnapshot const& drives,
                          IFileExplorerNavigationControllerAbi*
                              navigationController) {
    auto items = grid.Items();
    items.Clear();
    bool selectionCheckBoxesEnabled = IsAutoCheckSelectEnabled();
    for (auto const& drive : drives) {
        items.Append(CreateDriveCard(drive, navigationController,
                                     selectionCheckBoxesEnabled));
    }
}

bool IsDriveCardRenamingInGrid(muxc::GridView const& grid) {
    for (auto const& state : *g_driveCardEventStates) {
        if (!state.renaming) {
            continue;
        }

        auto item = state.item.get();
        if (item &&
            muxc::ItemsControl::ItemsControlFromItemContainer(item) == grid) {
            return true;
        }
    }
    return false;
}

void ClearDriveCardEventHandlersForGrid(muxc::GridView const& grid) {
    for (auto it = g_driveCardEventStates->begin();
         it != g_driveCardEventStates->end();) {
        auto item = it->item.get();
        if (item &&
            muxc::ItemsControl::ItemsControlFromItemContainer(item) == grid) {
            ClearDriveCardEventHandlers(*it);
            it = g_driveCardEventStates->erase(it);
        } else {
            ++it;
        }
    }
}

// Rebuilding the grid drops and recreates every GridViewItem, which would
// silently discard an in-progress inline rename and the current selection.
// Defer entirely while a card is being renamed (CompleteDriveRename() wakes
// up a fresh refresh once it ends, committed or not), and restore selection
// by root path across the rebuild otherwise.
void RefreshDevicesGridPreservingState(
    muxc::GridView const& grid,
    DriveSnapshot const& drives,
    IFileExplorerNavigationControllerAbi* navigationController) {
    // Also defers while a drive context menu is open or a Shell UI call
    // (rename, InvokeCommand) is in flight: TrackPopupMenuEx and those
    // calls all pump, so a refresh landing mid-interaction would otherwise
    // tear down and recreate every GridViewItem underneath the open menu --
    // ShowDriveContextMenu posts GetApplyDriveSnapshotMessage() on its way
    // out, so the deferred refresh isn't lost, just delayed.
    if (IsDriveCardRenamingInGrid(grid) || g_openContextMenuCount.load() > 0 ||
        g_pendingShellUiCalls.load() > 0) {
        return;
    }

    // Rebuilding replaces every GridViewItem, so selection and keyboard
    // focus (unlike hover and any open tooltip) are captured here and
    // reapplied by matching root path afterward, rather than being lost.
    std::vector<std::wstring> selectedRootPaths;
    std::wstring focusedRootPath;
    for (auto const& child : grid.Items()) {
        auto gridViewItem = child.try_as<muxc::GridViewItem>();
        if (!gridViewItem) {
            continue;
        }

        if (gridViewItem.IsSelected()) {
            selectedRootPaths.emplace_back(
                winrt::unbox_value<winrt::hstring>(gridViewItem.Tag())
                    .c_str());
        }
        // Same FocusManager check as FindFocusedDriveCard, not
        // FocusState(): that reflects the item's own last-known state,
        // which can go stale once focus has moved outside this XamlRoot
        // entirely, and would otherwise make an automatic refresh call
        // Focus(Programmatic) on a card the user isn't actually on.
        if (focusedRootPath.empty()) {
            if (auto xamlRoot = gridViewItem.XamlRoot()) {
                if (muxi::FocusManager::GetFocusedElement(xamlRoot) ==
                    gridViewItem) {
                    focusedRootPath = winrt::unbox_value<winrt::hstring>(
                                           gridViewItem.Tag())
                                           .c_str();
                }
            }
        }
    }

    ClearDriveCardEventHandlersForGrid(grid);
    PopulateDevicesGrid(grid, drives, navigationController);

    for (auto const& child : grid.Items()) {
        auto gridViewItem = child.try_as<muxc::GridViewItem>();
        if (!gridViewItem) {
            continue;
        }

        auto rootPath = winrt::unbox_value<winrt::hstring>(gridViewItem.Tag());
        if (std::find(selectedRootPaths.begin(), selectedRootPaths.end(),
                      rootPath.c_str()) != selectedRootPaths.end()) {
            gridViewItem.IsSelected(true);
        }
        if (!focusedRootPath.empty() && rootPath == focusedRootPath.c_str()) {
            gridViewItem.Focus(mux::FocusState::Programmatic);
        }
    }
}

constexpr wchar_t kDevicesExpandedValueName[] = L"DevicesExpanded";

// Applies the collapsed/expanded state to every devices header tracked on
// the current thread -- used both for the thread that owns the clicked
// button and for every other registered window catching up to it, so a
// change in one Explorer window doesn't leave the rest stuck showing the
// state from whenever their own section was last built.
void ApplyDevicesExpandedState(bool expanded) {
    for (auto& state : g_devicesHeaderEventStates) {
        if (state.expanded == expanded) {
            continue;
        }
        state.expanded = expanded;
        if (auto grid = state.grid.get()) {
            grid.Visibility(expanded ? mux::Visibility::Visible
                                     : mux::Visibility::Collapsed);
        }
        if (auto button = state.button.get()) {
            if (auto content = button.Content().try_as<muxc::Grid>()) {
                for (auto const& child : content.Children()) {
                    if (auto icon = child.try_as<muxc::FontIcon>()) {
                        icon.Glyph(expanded ? L"\xE70D" : L"\xE76C");
                        break;
                    }
                }
            }
        }
    }
}

void DevicesHeader_Click(
    winrt::Windows::Foundation::IInspectable const& sender,
    mux::RoutedEventArgs const&) {
    auto button = sender.as<muxc::Button>();

    bool newExpanded = true;
    bool found = false;
    for (auto const& state : g_devicesHeaderEventStates) {
        if (state.button.get() == button) {
            newExpanded = !state.expanded;
            found = true;
            break;
        }
    }
    if (!found) {
        return;
    }

    Wh_SetIntValue(kDevicesExpandedValueName, newExpanded ? 1 : 0);
    ApplyDevicesExpandedState(newExpanded);

    std::vector<HWND> windows;
    {
        std::lock_guard lock(g_registeredWindowsMutex);
        windows = g_registeredWindows;
    }
    for (HWND window : windows) {
        if (IsWindow(window)) {
            PostMessageW(window, GetToggleDevicesExpandedMessage(),
                        newExpanded ? 1 : 0, 0);
        }
    }
}

void ClearDevicesHeaderEventHandlersForCurrentThread() {
    for (auto const& state : g_devicesHeaderEventStates) {
        if (auto button = state.button.get()) {
            try {
                button.Click(state.clickToken);
            } catch (...) {
                Wh_Log(L"Devices header event cleanup failed: %08X",
                       winrt::to_hresult().value);
            }
        }
        if (auto grid = state.grid.get()) {
            try {
                grid.ActualThemeChanged(state.themeChangedToken);
            } catch (...) {
                Wh_Log(L"Devices header event cleanup failed: %08X",
                       winrt::to_hresult().value);
            }
        }
    }
    g_devicesHeaderEventStates.clear();
}

bool IsTrackedDriveSource(mux::DependencyObject source,
                          muxc::Grid const& surface) {
    while (source && source != surface) {
        if (auto item = source.try_as<muxc::GridViewItem>()) {
            for (auto const& state : *g_driveCardEventStates) {
                if (state.item.get() == item) {
                    return true;
                }
            }
        }
        source = muxm::VisualTreeHelper::GetParent(source);
    }
    return false;
}

bool IsSourceWithinElement(mux::DependencyObject source,
                           mux::DependencyObject const& element,
                           muxc::Grid const& surface) {
    while (source && source != surface) {
        if (source == element) {
            return true;
        }
        source = muxm::VisualTreeHelper::GetParent(source);
    }
    return source == element;
}

void CompleteDriveRenamesOutsideSource(mux::DependencyObject const& source,
                                       muxc::Grid const& surface) {
    std::vector<muxc::TextBox> renameBoxes;
    for (auto const& state : *g_driveCardEventStates) {
        if (state.renaming && state.renameBox &&
            !IsSourceWithinElement(source, state.renameBox, surface)) {
            renameBoxes.push_back(state.renameBox);
        }
    }

    for (auto const& renameBox : renameBoxes) {
        CompleteDriveRename(renameBox, true, false);
    }
}

bool IsInteractiveHomeSource(mux::DependencyObject source,
                             muxc::Grid const& surface) {
    while (source && source != surface) {
        if (auto control = source.try_as<muxc::Control>()) {
            if (control.IsTabStop()) {
                return true;
            }
        }
        source = muxm::VisualTreeHelper::GetParent(source);
    }
    return false;
}

void ClearDriveSelection(muxc::StackPanel const& panel) {
    if (auto grid = FindDevicesGrid(panel)) {
        grid.SelectedItems().Clear();
    }
}

HomeSelectionEventState* FindHomeSelectionState(muxc::Grid const& surface) {
    for (auto& state : g_homeSelectionEventStates) {
        if (state.surface.get() == surface) {
            return &state;
        }
    }
    return nullptr;
}

void UpdateDriveMarqueeSelection(
    HomeSelectionEventState& state,
    winrt::Windows::Foundation::Point const& current) {
    auto surface = state.surface.get();
    auto panel = state.panel.get();
    auto grid = panel ? FindDevicesGrid(panel) : nullptr;
    // Collapsing the section collapses the GridView, not the individual
    // cards, so with the section collapsed the cards are still Visible but
    // with stale/zero layout bounds -- without this, a drag near the
    // Home surface's top-left could flip their IsSelected.
    if (!surface || !grid || grid.Visibility() != mux::Visibility::Visible) {
        return;
    }

    float left = std::min(state.start.X, current.X);
    float top = std::min(state.start.Y, current.Y);
    float right = std::max(state.start.X, current.X);
    float bottom = std::max(state.start.Y, current.Y);

    for (auto const& cardState : *g_driveCardEventStates) {
        auto item = cardState.item.get();
        if (!item || item.Visibility() != mux::Visibility::Visible ||
            muxc::ItemsControl::ItemsControlFromItemContainer(item) != grid) {
            continue;
        }

        auto transform = item.TransformToVisual(surface);
        auto itemTopLeft = transform.TransformPoint({0, 0});
        float itemRight = itemTopLeft.X +
                          static_cast<float>(item.ActualWidth());
        float itemBottom = itemTopLeft.Y +
                           static_cast<float>(item.ActualHeight());
        bool intersects = right >= itemTopLeft.X && left <= itemRight &&
                          bottom >= itemTopLeft.Y && top <= itemBottom;

        bool initiallySelected = false;
        if (state.additive) {
            for (auto const& initial : state.initialSelection) {
                if (initial.get() == item) {
                    initiallySelected = true;
                    break;
                }
            }
        }
        item.IsSelected(intersects || initiallySelected);
    }
}

void HomeSelection_PointerPressed(
    winrt::Windows::Foundation::IInspectable const& sender,
    muxi::PointerRoutedEventArgs const& args) {
    if (g_unloading.load()) {
        return;
    }

    try {
        auto surface = sender.as<muxc::Grid>();
        auto source = args.OriginalSource().try_as<mux::DependencyObject>();
        CompleteDriveRenamesOutsideSource(source, surface);

        auto state = FindHomeSelectionState(surface);
        auto point = args.GetCurrentPoint(surface);
        if (!state || !point.Properties().IsLeftButtonPressed()) {
            return;
        }
        if (IsTrackedDriveSource(source, surface)) {
            return;
        }

        auto panel = state->panel.get();
        if (!panel) {
            return;
        }
        if (IsInteractiveHomeSource(source, surface)) {
            ClearDriveSelection(panel);
            return;
        }

        state->tracking = true;
        state->moved = false;
        state->pointerId = point.PointerId();
        state->start = point.Position();
        state->additive = (GetKeyState(VK_CONTROL) & 0x8000) != 0;
        state->initialSelection.clear();
        if (state->additive) {
            if (auto grid = FindDevicesGrid(panel)) {
                for (auto const& selected : grid.SelectedItems()) {
                    if (auto item = selected.try_as<muxc::GridViewItem>()) {
                        state->initialSelection.push_back(
                            winrt::make_weak(item));
                    }
                }
            }
        }
    } catch (...) {
        Wh_Log(L"Home selection press handling failed: %08X",
               winrt::to_hresult().value);
    }
}

void HomeSelection_PointerMoved(
    winrt::Windows::Foundation::IInspectable const& sender,
    muxi::PointerRoutedEventArgs const& args) {
    try {
        auto surface = sender.as<muxc::Grid>();
        auto state = FindHomeSelectionState(surface);
        if (!state || !state->tracking) {
            return;
        }

        // GetCurrentPoint has a real cost, so it's worth skipping on the
        // common path above (most pointer moves over the Home page aren't
        // an active marquee drag).
        auto point = args.GetCurrentPoint(surface);
        if (state->pointerId != point.PointerId()) {
            return;
        }
        if (!point.Properties().IsLeftButtonPressed()) {
            state->tracking = false;
            state->initialSelection.clear();
            return;
        }

        auto current = point.Position();
        float deltaX = current.X - state->start.X;
        float deltaY = current.Y - state->start.Y;
        if (!state->moved && deltaX * deltaX + deltaY * deltaY < 16) {
            return;
        }

        if (!state->moved) {
            state->moved = true;
            if (!state->additive) {
                if (auto panel = state->panel.get()) {
                    ClearDriveSelection(panel);
                }
            }
        }
        UpdateDriveMarqueeSelection(*state, current);
    } catch (...) {
        Wh_Log(L"Home marquee selection failed: %08X",
               winrt::to_hresult().value);
    }
}

void HomeSelection_PointerReleased(
    winrt::Windows::Foundation::IInspectable const& sender,
    muxi::PointerRoutedEventArgs const& args) {
    try {
        auto surface = sender.as<muxc::Grid>();
        auto state = FindHomeSelectionState(surface);
        auto point = args.GetCurrentPoint(surface);
        if (!state || !state->tracking ||
            state->pointerId != point.PointerId()) {
            return;
        }

        if (!state->moved) {
            if (auto panel = state->panel.get()) {
                ClearDriveSelection(panel);
            }
        }
        state->tracking = false;
        state->initialSelection.clear();
    } catch (...) {
        Wh_Log(L"Home selection release handling failed: %08X",
               winrt::to_hresult().value);
    }
}

void EnsureHomeSelectionHandlers(muxc::Grid const& surface,
                                 muxc::StackPanel const& panel) {
    for (auto it = g_homeSelectionEventStates.begin();
         it != g_homeSelectionEventStates.end();) {
        auto trackedSurface = it->surface.get();
        if (!trackedSurface) {
            it = g_homeSelectionEventStates.erase(it);
            continue;
        }
        if (trackedSurface == surface) {
            it->panel = winrt::make_weak(panel);
            return;
        }
        ++it;
    }

    HomeSelectionEventState state;
    state.surface = winrt::make_weak(surface);
    state.panel = winrt::make_weak(panel);
    state.pressedHandler = RetainRoutedEventHandler(
        muxi::PointerEventHandler{HomeSelection_PointerPressed});
    state.movedHandler = RetainRoutedEventHandler(
        muxi::PointerEventHandler{HomeSelection_PointerMoved});
    state.releasedHandler = RetainRoutedEventHandler(
        muxi::PointerEventHandler{HomeSelection_PointerReleased});

    bool pressedAdded = false;
    bool movedAdded = false;
    try {
        surface.AddHandler(mux::UIElement::PointerPressedEvent(),
                           state.pressedHandler, true);
        pressedAdded = true;
        surface.AddHandler(mux::UIElement::PointerMovedEvent(),
                           state.movedHandler, true);
        movedAdded = true;
        surface.AddHandler(mux::UIElement::PointerReleasedEvent(),
                           state.releasedHandler, true);
    } catch (...) {
        if (movedAdded) {
            surface.RemoveHandler(mux::UIElement::PointerMovedEvent(),
                                  state.movedHandler);
        }
        if (pressedAdded) {
            surface.RemoveHandler(mux::UIElement::PointerPressedEvent(),
                                  state.pressedHandler);
        }
        throw;
    }
    g_homeSelectionEventStates.push_back(std::move(state));
}

void ClearHomeSelectionEventHandlersForCurrentThread() {
    for (auto const& state : g_homeSelectionEventStates) {
        if (auto surface = state.surface.get()) {
            try {
                surface.RemoveHandler(mux::UIElement::PointerPressedEvent(),
                                      state.pressedHandler);
                surface.RemoveHandler(mux::UIElement::PointerMovedEvent(),
                                      state.movedHandler);
                surface.RemoveHandler(mux::UIElement::PointerReleasedEvent(),
                                      state.releasedHandler);
            } catch (...) {
                Wh_Log(L"Home selection event cleanup failed: %08X",
                       winrt::to_hresult().value);
            }
        }
    }
    g_homeSelectionEventStates.clear();
}

void RefreshDevicesSectionsForCurrentThread() {
    auto snapshot = GetLatestDriveSnapshot();
    if (!snapshot || g_unloading.load()) {
        return;
    }

    PruneExpiredDriveCardEventHandlersForCurrentThread();
    for (auto it = g_homePanels.begin(); it != g_homePanels.end();) {
        auto panel = it->panel.get();
        if (!panel) {
            if (it->navigationController) {
                it->navigationController->Release();
            }
            it = g_homePanels.erase(it);
            continue;
        }

        try {
            if (auto grid = FindDevicesGrid(panel)) {
                RefreshDevicesGridPreservingState(grid, *snapshot,
                                                  it->navigationController);
            }
        } catch (...) {
            Wh_Log(L"Drive grid refresh failed: %08X",
                   winrt::to_hresult().value);
        }
        ++it;
    }
}

// TryGetBrush()/ApplyBrushIfAvailable() are one-time resource lookups
// assigned as local values -- a frozen snapshot, not a live {ThemeResource}
// binding, which only compiled XAML gets. The native tiles don't set
// Foreground locally at all (confirmed against a live tree dump); their
// color comes from a Style's ThemeResource setter instead. Re-resolving on
// ActualThemeChanged is the practical equivalent for code-built UI.
void RefreshThemedVisualsForCurrentThread() {
    for (auto& state : g_devicesHeaderEventStates) {
        if (auto button = state.button.get()) {
            if (auto brush = TryGetBrush(L"SubtleFillColorTransparentBrush")) {
                button.Background(brush);
            }
            if (auto content = button.Content().try_as<muxc::Grid>()) {
                for (auto const& child : content.Children()) {
                    if (auto icon = child.try_as<muxc::FontIcon>()) {
                        ApplyBrushIfAvailable(icon,
                                              L"TextFillColorPrimaryBrush");
                    } else if (auto text = child.try_as<muxc::TextBlock>()) {
                        ApplyBrushIfAvailable(text,
                                              L"TextFillColorPrimaryBrush");
                    }
                }
            }
        }
    }

    // Rebuilds from the already-cached snapshot (icon pixels included), no
    // worker round-trip or disk/Shell I/O, so this is as fast as the native
    // tiles updating.
    RefreshDevicesSectionsForCurrentThread();
}

void DevicesGrid_ActualThemeChanged(
    mux::FrameworkElement const&,
    winrt::Windows::Foundation::IInspectable const&) {
    if (!g_unloading.load()) {
        RefreshThemedVisualsForCurrentThread();
    }
}

muxc::StackPanel CreateDevicesSection() {
    for (auto it = g_devicesHeaderEventStates.begin();
         it != g_devicesHeaderEventStates.end();) {
        if (!it->button.get()) {
            it = g_devicesHeaderEventStates.erase(it);
        } else {
            ++it;
        }
    }

    bool expanded = Wh_GetIntValue(kDevicesExpandedValueName, 1) != 0;

    muxc::StackPanel section;
    section.Name(kDevicesSectionName);
    section.HorizontalAlignment(mux::HorizontalAlignment::Stretch);

    muxc::Button headerButton;
    headerButton.Height(36);
    headerButton.MinWidth(0);
    headerButton.MinHeight(0);
    headerButton.Margin(mux::Thickness{14, 8, 14, 0});
    headerButton.Padding(mux::Thickness{0, 0, 0, 0});
    headerButton.BorderThickness(mux::Thickness{0, 0, 0, 0});
    headerButton.CornerRadius(mux::CornerRadius{4, 4, 4, 4});
    headerButton.HorizontalAlignment(mux::HorizontalAlignment::Left);
    headerButton.HorizontalContentAlignment(
        mux::HorizontalAlignment::Left);
    headerButton.VerticalContentAlignment(mux::VerticalAlignment::Center);
    if (auto brush = TryGetBrush(L"SubtleFillColorTransparentBrush")) {
        headerButton.Background(brush);
    }

    muxc::Grid headerContent;
    muxc::ColumnDefinition toggleColumn;
    toggleColumn.Width(mux::GridLength{36, mux::GridUnitType::Pixel});
    muxc::ColumnDefinition textColumn;
    textColumn.Width(mux::GridLength{1, mux::GridUnitType::Auto});
    headerContent.ColumnDefinitions().Append(toggleColumn);
    headerContent.ColumnDefinitions().Append(textColumn);

    muxc::FontIcon headerIcon;
    headerIcon.FontFamily(muxm::FontFamily{L"Segoe Fluent Icons"});
    headerIcon.FontSize(16);
    headerIcon.Glyph(expanded ? L"\xE70D" : L"\xE76C");
    ApplyBrushIfAvailable(headerIcon, L"TextFillColorPrimaryBrush");
    muxc::Grid::SetColumn(headerIcon, 0);

    muxc::TextBlock headerText;
    headerText.Text(GetShellStrings().devicesAndDrives);
    ApplyBodyStrongTextStyle(headerText);
    headerText.VerticalAlignment(mux::VerticalAlignment::Center);
    headerText.Margin(mux::Thickness{8, 0, 12, 0});
    ApplyBrushIfAvailable(headerText, L"TextFillColorPrimaryBrush");
    muxc::Grid::SetColumn(headerText, 1);

    headerContent.Children().Append(headerIcon);
    headerContent.Children().Append(headerText);
    headerButton.Content(headerContent);

    muxc::GridView driveGrid;
    driveGrid.Name(kDevicesGridName);
    driveGrid.Margin(mux::Thickness{14, 4, 0, 0});
    driveGrid.Padding(mux::Thickness{0, 0, 0, 0});
    driveGrid.HorizontalAlignment(mux::HorizontalAlignment::Stretch);
    driveGrid.SelectionMode(muxc::ListViewSelectionMode::Extended);
    driveGrid.IsMultiSelectCheckBoxEnabled(false);
    driveGrid.IsItemClickEnabled(false);
    driveGrid.IsTabStop(false);
    driveGrid.Visibility(expanded ? mux::Visibility::Visible
                                  : mux::Visibility::Collapsed);
    muxc::ScrollViewer::SetVerticalScrollMode(
        driveGrid, muxc::ScrollMode::Disabled);
    muxc::ScrollViewer::SetVerticalScrollBarVisibility(
        driveGrid, muxc::ScrollBarVisibility::Disabled);

    section.Children().Append(headerButton);
    section.Children().Append(driveGrid);

    auto clickToken = headerButton.Click(DevicesHeader_Click);
    auto themeChangedToken =
        driveGrid.ActualThemeChanged(DevicesGrid_ActualThemeChanged);
    g_devicesHeaderEventStates.push_back({
        .button = winrt::make_weak(headerButton),
        .grid = winrt::make_weak(driveGrid),
        .clickToken = clickToken,
        .themeChangedToken = themeChangedToken,
        .expanded = expanded,
    });
    return section;
}

void TrackHomePanel(muxc::StackPanel const& panel) {
    auto navigationController = GetNavigationControllerForCurrentThread();

    for (auto it = g_homePanels.begin(); it != g_homePanels.end();) {
        auto tracked = it->panel.get();
        if (!tracked) {
            if (it->navigationController) {
                it->navigationController->Release();
            }
            it = g_homePanels.erase(it);
            continue;
        }

        if (tracked == panel) {
            if (navigationController) {
                if (navigationController == it->navigationController) {
                    navigationController->Release();
                } else {
                    if (it->navigationController) {
                        it->navigationController->Release();
                    }
                    it->navigationController = navigationController;
                }
            }
            return;
        }

        ++it;
    }

    try {
        g_homePanels.push_back(
            {winrt::make_weak(panel), navigationController});
    } catch (...) {
        if (navigationController) {
            navigationController->Release();
        }
        throw;
    }
}

bool PopulateDevicesSectionIfEmpty(muxc::StackPanel const& panel) {
    auto grid = FindDevicesGrid(panel);
    if (!grid) {
        return false;
    }

    if (grid.Items().Size() != 0) {
        return true;
    }

    auto snapshot = GetLatestDriveSnapshot();
    if (!snapshot || g_unloading.load()) {
        return false;
    }

    IFileExplorerNavigationControllerAbi* navigationController = nullptr;
    for (auto const& state : g_homePanels) {
        if (state.panel.get() == panel) {
            navigationController = state.navigationController;
            break;
        }
    }

    PruneExpiredDriveCardEventHandlersForCurrentThread();
    PopulateDevicesGrid(grid, *snapshot, navigationController);
    return true;
}

bool IsHomePanelTracked(muxc::StackPanel const& panel) {
    for (auto const& state : g_homePanels) {
        if (state.panel.get() == panel) {
            return true;
        }
    }
    return false;
}

bool InsertDevicesSection(muxc::StackPanel const& panel) {
    auto children = panel.Children();
    uint32_t primaryContentIndex = UINT32_MAX;
    uint32_t existingSectionIndex = UINT32_MAX;

    for (uint32_t i = 0; i < children.Size(); ++i) {
        auto element = children.GetAt(i).try_as<mux::FrameworkElement>();
        if (!element) {
            continue;
        }

        auto name = element.Name();
        if (name == kPrimaryContentPresenterName) {
            primaryContentIndex = i;
        } else if (name == kDevicesSectionName) {
            existingSectionIndex = i;
        }
    }

    if (primaryContentIndex == UINT32_MAX) {
        return false;
    }

    // An existing WindhawkDevicesSection is only trusted if this mod
    // instance already tracks the panel it's in. An untracked one may be a
    // leftover from a previous, already-unloaded instance, whose handlers
    // point into a freed image -- rebuilding it is cheap insurance.
    bool trustExisting =
        existingSectionIndex != UINT32_MAX && IsHomePanelTracked(panel);

    uint32_t desiredIndex = primaryContentIndex + 1;
    if (existingSectionIndex == desiredIndex && trustExisting) {
        TrackHomePanel(panel);
        return true;
    }

    mux::UIElement section = nullptr;
    if (existingSectionIndex != UINT32_MAX) {
        if (trustExisting) {
            section = children.GetAt(existingSectionIndex);
        }
        children.RemoveAt(existingSectionIndex);
        if (existingSectionIndex < desiredIndex) {
            --desiredIndex;
        }
    }
    if (!section) {
        section = CreateDevicesSection();
    }

    children.InsertAt(desiredIndex, section);
    TrackHomePanel(panel);
    return true;
}

void TryInjectFromHomeScrollViewer(mux::FrameworkElement const& element) {
    auto scrollViewer = element.try_as<muxc::ScrollViewer>();
    if (!scrollViewer) {
        return;
    }

    auto contentGrid = scrollViewer.Content().try_as<muxc::Grid>();
    if (!contentGrid) {
        Wh_Log(L"HomeScrollViewer content isn't a Grid");
        return;
    }

    for (auto const& child : contentGrid.Children()) {
        if (auto panel = child.try_as<muxc::StackPanel>()) {
            if (!InsertDevicesSection(panel)) {
                Wh_Log(L"The Home StackPanel has no PrimaryGroupContentPresenter");
            } else {
                EnsureHomeSelectionHandlers(contentGrid, panel);
                EnsureShellNotificationsForCurrentThread();
                if (!PopulateDevicesSectionIfEmpty(panel)) {
                    RequestInitialDriveSnapshot();
                }
            }
            return;
        }
    }

    Wh_Log(L"No direct StackPanel child was found in HomeScrollViewer.Content");
}

muxc::ScrollViewer FindHomeScrollViewer(
    mux::DependencyObject const& root) {
    constexpr size_t kMaxVisitedElements = 4096;

    std::vector<mux::DependencyObject> pending{root};
    for (size_t index = 0;
         index < pending.size() && index < kMaxVisitedElements; ++index) {
        auto const& current = pending[index];
        if (auto element = current.try_as<mux::FrameworkElement>()) {
            if (element.Name() == L"HomeScrollViewer") {
                return element.try_as<muxc::ScrollViewer>();
            }
        }

        int childCount = muxm::VisualTreeHelper::GetChildrenCount(current);
        for (int childIndex = 0;
             childIndex < childCount && pending.size() < kMaxVisitedElements;
             ++childIndex) {
            if (auto child =
                    muxm::VisualTreeHelper::GetChild(current, childIndex)) {
                pending.push_back(child);
            }
        }
    }

    return nullptr;
}

// OnXamlRootChanged also fires for resize and DPI/visibility changes, not
// just real navigation. If this XamlRoot's panel already carries a live
// devices section, skip the visual-tree BFS and the EnumWindows pass inside
// TryInjectFromHomeScrollViewer entirely instead of redoing them every time.
// Confirms a tracked panel is still reachable by walking up from it, rather
// than trusting its XamlRoot property alone: if Explorer replaced the Home
// panel with a fresh one (e.g. on a later navigation) while the old one's
// weak_ref is still resolvable, both panels report the same XamlRoot even
// though only the new one is actually in the tree.
bool IsElementInXamlTree(mux::UIElement const& element,
                         mux::UIElement const& root) {
    mux::DependencyObject current = element;
    while (current) {
        if (current == root) {
            return true;
        }
        current = muxm::VisualTreeHelper::GetParent(current);
    }
    return false;
}

bool XamlRootAlreadyHasDevicesSection(mux::XamlRoot const& xamlRoot) {
    auto content = xamlRoot.Content();
    if (!content) {
        return false;
    }

    for (auto const& state : g_homePanels) {
        auto panel = state.panel.get();
        if (panel && panel.XamlRoot() == xamlRoot && FindDevicesGrid(panel) &&
            IsElementInXamlTree(panel, content)) {
            return true;
        }
    }
    return false;
}

void TryInjectFromXamlRoot(mux::XamlRoot const& xamlRoot) {
    if (XamlRootAlreadyHasDevicesSection(xamlRoot)) {
        return;
    }

    auto content = xamlRoot.Content();
    if (!content) {
        return;
    }

    auto root = content.try_as<mux::DependencyObject>();
    auto scrollViewer = root ? FindHomeScrollViewer(root) : nullptr;
    if (!scrollViewer) {
        return;
    }

    TryInjectFromHomeScrollViewer(scrollViewer);
}

void ClearTrackedHomePanelsForCurrentThread() {
    for (auto const& state : g_homePanels) {
        if (state.navigationController) {
            state.navigationController->Release();
        }
    }
    g_homePanels.clear();
}

void RemoveDevicesSectionsForCurrentThread() {
    ClearHomeSelectionEventHandlersForCurrentThread();
    ClearDriveCardEventHandlersForCurrentThread();
    ClearDevicesHeaderEventHandlersForCurrentThread();

    for (auto const& state : g_homePanels) {
        auto panel = state.panel.get();
        if (!panel) {
            continue;
        }

        try {
            auto children = panel.Children();
            for (uint32_t i = children.Size(); i > 0; --i) {
                auto element =
                    children.GetAt(i - 1).try_as<mux::FrameworkElement>();
                if (element && element.Name() == kDevicesSectionName) {
                    children.RemoveAt(i - 1);
                }
            }
        } catch (...) {
            Wh_Log(L"Section removal failed: %08X",
                   winrt::to_hresult().value);
        }
    }

    ClearTrackedHomePanelsForCurrentThread();
}

// OnXamlRootChanged returns fire_and_forget, so its hidden return buffer is the
// second ABI parameter. The XamlRoot and event args are passed indirectly.
using HomeViewControl_Wave2_OnXamlRootChanged_t = void*(WINAPI*)(
    void* pThis,
    void* returnValue,
    void* xamlRoot,
    void* args);
HomeViewControl_Wave2_OnXamlRootChanged_t
    HomeViewControl_Wave2_OnXamlRootChanged_Original;

void* WINAPI HomeViewControl_Wave2_OnXamlRootChanged_Hook(
    void* pThis,
    void* returnValue,
    void* xamlRoot,
    void* args) {
    mux::XamlRoot root{nullptr};

    // The original consumes the by-value parameter. Keep our own reference
    // before calling it so the projected object remains valid afterwards.
    if (!g_unloading.load() && xamlRoot) {
        try {
            auto const& incomingRoot =
                *reinterpret_cast<mux::XamlRoot const*>(xamlRoot);
            if (incomingRoot) {
                root = incomingRoot;
            }
        } catch (...) {
            Wh_Log(L"Couldn't retain the Home XamlRoot: %08X",
                   winrt::to_hresult().value);
        }
    }

    void* result = HomeViewControl_Wave2_OnXamlRootChanged_Original(
        pThis, returnValue, xamlRoot, args);

    if (g_unloading.load() || !root) {
        return result;
    }

    try {
        TryInjectFromXamlRoot(root);
    } catch (...) {
        auto error = winrt::to_hresult();
        if (error.value != E_INVALIDARG) {
            Wh_Log(L"Home XamlRoot scan failed: %08X", error.value);
        }
    }

    return result;
}

using FileExplorerNavigationController_SetNavigationState_t = HRESULT(WINAPI*)(
    void* pThis,
    unsigned long state);
FileExplorerNavigationController_SetNavigationState_t
    FileExplorerNavigationController_SetNavigationState_Original;

HRESULT WINAPI FileExplorerNavigationController_SetNavigationState_Hook(
    void* pThis,
    unsigned long state) {
    RememberNavigationControllerForCurrentThread(pThis);
    return FileExplorerNavigationController_SetNavigationState_Original(pThis,
                                                                         state);
}

enum class SymbolHookResult {
    Success,
    ResolutionFailed,
    NoSymbolFound,
};

SymbolHookResult HookFileExplorerExtensionsSymbols(HMODULE module) {
    // FileExplorerExtensions.dll.
    WindhawkUtils::SYMBOL_HOOK fileExplorerExtensionsDllHooks[] = {
        {
            {
                LR"(private: struct winrt::fire_and_forget __cdecl winrt::FileExplorerExtensions::implementation::HomeViewControl_Wave2::OnXamlRootChanged(struct winrt::Microsoft::UI::Xaml::XamlRoot,struct winrt::Microsoft::UI::Xaml::XamlRootChangedEventArgs))",
                LR"(private: struct winrt::fire_and_forget __cdecl winrt::FileExplorerExtensions::implementation::HomeViewControl_Wave2::OnXamlRootChanged(struct winrt::Microsoft::UI::Xaml::XamlRoot,struct winrt::Microsoft::UI::Xaml::XamlRootChangedEventArgs) __ptr64)",
            },
            &HomeViewControl_Wave2_OnXamlRootChanged_Original,
            HomeViewControl_Wave2_OnXamlRootChanged_Hook,
        },
    };

    if (!WindhawkUtils::HookSymbols(module, fileExplorerExtensionsDllHooks,
                                    ARRAYSIZE(fileExplorerExtensionsDllHooks))) {
        Wh_Log(L"Failed to resolve FileExplorerExtensions.dll symbols");
        return SymbolHookResult::ResolutionFailed;
    }

    if (!HomeViewControl_Wave2_OnXamlRootChanged_Original) {
        Wh_Log(L"OnXamlRootChanged symbol wasn't found");
        return SymbolHookResult::NoSymbolFound;
    }

    return SymbolHookResult::Success;
}

bool HookFileExplorerExtensionsIfLoaded(bool applyHooks) {
    if (g_fileExplorerExtensionsSymbolsHooked.load()) {
        return true;
    }

    HMODULE module = GetModuleHandleW(L"FileExplorerExtensions.dll");
    if (!module) {
        return true;
    }

    if (g_fileExplorerExtensionsSymbolsHooked.exchange(true)) {
        return true;
    }

    switch (HookFileExplorerExtensionsSymbols(module)) {
        case SymbolHookResult::Success:
            break;
        case SymbolHookResult::ResolutionFailed:
        case SymbolHookResult::NoSymbolFound:
            // Left true (not reset): this is a definitive resolution
            // failure against the loaded module, not a "not loaded yet"
            // case, so a later LoadLibraryExW of the same DLL would only
            // repeat the same failure.
            return false;
    }

    if (applyHooks && !g_unloading.load()) {
        if (!Wh_ApplyHookOperations()) {
            Wh_Log(L"Failed to apply deferred File Explorer hooks");
            return false;
        }
    }

    return true;
}

bool HookWindowsUiFileExplorerIfLoaded(bool applyHooks) {
    if (g_navigationSymbolsHooked.load()) {
        return true;
    }

    HMODULE module = GetModuleHandleW(L"Windows.UI.FileExplorer.dll");
    if (!module) {
        return true;
    }

    if (g_navigationSymbolsHooked.exchange(true)) {
        return true;
    }

    // Navigation is optional; the cards retain their ShellExecute fallback
    // if this symbol changes on another build.
    // Windows.UI.FileExplorer.dll
    WindhawkUtils::SYMBOL_HOOK windowsUiFileExplorerHooks[] = {
        {
            {
                LR"(public: virtual long __cdecl winrt::WindowsUdk::UI::Shell::implementation::FileExplorerNavigationController::SetNavigationState(unsigned long))",
            },
            &FileExplorerNavigationController_SetNavigationState_Original,
            FileExplorerNavigationController_SetNavigationState_Hook,
            true,
        },
    };

    if (!WindhawkUtils::HookSymbols(
            module, windowsUiFileExplorerHooks,
            ARRAYSIZE(windowsUiFileExplorerHooks))) {
        // Left true (not reset): this is a definitive resolution failure
        // against the loaded module, not a "not loaded yet" case, so a later
        // LoadLibraryExW of the same DLL would only repeat the same failure.
        Wh_Log(L"Failed to resolve Windows.UI.FileExplorer.dll symbols; "
               L"current-tab navigation is unavailable");
        return true;
    }

    if (!FileExplorerNavigationController_SetNavigationState_Original) {
        Wh_Log(L"SetNavigationState wasn't found; current-tab navigation is "
               L"unavailable");
        return true;
    }

    if (applyHooks && !g_unloading.load() && !Wh_ApplyHookOperations()) {
        Wh_Log(L"Failed to apply the deferred navigation hook");
        return false;
    }

    return true;
}

using LoadLibraryExW_t = decltype(&LoadLibraryExW);
LoadLibraryExW_t LoadLibraryExW_Original;

HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR fileName,
                                   HANDLE file,
                                   DWORD flags) {
    HMODULE module = LoadLibraryExW_Original(fileName, file, flags);
    if (!module || !fileName || g_unloading.load()) {
        return module;
    }

    PCWSTR baseName = fileName;
    for (PCWSTR p = fileName; *p; ++p) {
        if (*p == L'\\' || *p == L'/') {
            baseName = p + 1;
        }
    }

    if (_wcsicmp(baseName, L"FileExplorerExtensions.dll") == 0 ||
        _wcsicmp(baseName, L"FileExplorerExtensions") == 0) {
        HookFileExplorerExtensionsIfLoaded(true);
    } else if (_wcsicmp(baseName, L"Windows.UI.FileExplorer.dll") == 0 ||
               _wcsicmp(baseName, L"Windows.UI.FileExplorer") == 0) {
        HookWindowsUiFileExplorerIfLoaded(true);
    }

    return module;
}

using RunFromWindowThreadProc = void(WINAPI*)(void* parameter);

struct RunFromWindowThreadParameters {
    RunFromWindowThreadProc procedure;
    void* parameter;
};

UINT GetRunFromWindowThreadMessage() {
    static const UINT message =
        RegisterWindowMessageW(L"Windhawk_RunFromWindowThread_" WH_MOD_ID);
    return message;
}

LRESULT CALLBACK RunFromWindowThreadHookProc(int code,
                                             WPARAM wParam,
                                             LPARAM lParam) {
    if (code == HC_ACTION) {
        auto call = reinterpret_cast<const CWPSTRUCT*>(lParam);
        if (call->message == GetRunFromWindowThreadMessage()) {
            auto parameters = reinterpret_cast<RunFromWindowThreadParameters*>(
                call->lParam);
            parameters->procedure(parameters->parameter);
        }
    }

    return CallNextHookEx(nullptr, code, wParam, lParam);
}

bool RunFromWindowThread(HWND window,
                         RunFromWindowThreadProc procedure,
                         void* parameter) {
    DWORD threadId = GetWindowThreadProcessId(window, nullptr);
    if (!threadId) {
        return false;
    }

    if (threadId == GetCurrentThreadId()) {
        procedure(parameter);
        return true;
    }

    HHOOK hook = SetWindowsHookExW(WH_CALLWNDPROC, RunFromWindowThreadHookProc,
                                   nullptr, threadId);
    if (!hook) {
        return false;
    }

    RunFromWindowThreadParameters parameters{procedure, parameter};
    SendMessageW(window, GetRunFromWindowThreadMessage(), 0,
                 reinterpret_cast<LPARAM>(&parameters));
    UnhookWindowsHookEx(hook);
    return true;
}

BOOL CALLBACK EnumFileExplorerWindowsProc(HWND window, LPARAM parameter) {
    auto& windows = *reinterpret_cast<std::vector<HWND>*>(parameter);

    DWORD processId = 0;
    if (!GetWindowThreadProcessId(window, &processId) ||
        processId != GetCurrentProcessId()) {
        return TRUE;
    }

    wchar_t className[64];
    if (GetClassNameW(window, className, ARRAYSIZE(className)) &&
        _wcsicmp(className, L"CabinetWClass") == 0) {
        windows.push_back(window);
    }

    return TRUE;
}

std::vector<HWND> GetFileExplorerWindows() {
    std::vector<HWND> windows;
    EnumWindows(EnumFileExplorerWindowsProc,
                reinterpret_cast<LPARAM>(&windows));
    return windows;
}

UINT GetShellChangeMessage() {
    static const UINT message =
        RegisterWindowMessageW(L"Windhawk_ShellDriveChange_" WH_MOD_ID);
    return message;
}

bool IsDriveRootPidl(PCIDLIST_ABSOLUTE pidl) {
    if (!pidl) {
        return false;
    }

    wchar_t path[MAX_PATH]{};
    return SHGetPathFromIDListW(pidl, path) && PathIsRootW(path);
}

bool IsDirectChildOfComputerFolder(PCIDLIST_ABSOLUTE pidl,
                                   PCIDLIST_ABSOLUTE computerPidl) {
    if (!pidl || !computerPidl) {
        return false;
    }

    // ILIsParent is reflexive, so equality must be rejected explicitly when
    // testing for an immediate child of This PC.
    return !ILIsEqual(computerPidl, pidl) &&
           ILIsParent(computerPidl, pidl, TRUE);
}

// FOLDERID_ComputerFolder's PIDL doesn't change for the life of the
// process, so resolving it once here avoids a SHGetKnownFolderIDList
// allocation on every delivered Shell notification.
PCIDLIST_ABSOLUTE GetCachedComputerPidl() {
    static std::once_flag once;
    static UniqueAbsolutePidl cachedPidl;
    std::call_once(once, [] {
        PIDLIST_ABSOLUTE rawPidl = nullptr;
        if (SUCCEEDED(SHGetKnownFolderIDList(FOLDERID_ComputerFolder,
                                             KF_FLAG_DEFAULT, nullptr,
                                             &rawPidl)) &&
            rawPidl) {
            cachedPidl.reset(rawPidl);
        }
    });
    return cachedPidl.get();
}

bool IsRelevantShellDriveEvent(WPARAM wParam, LPARAM lParam,
                               LONG* eventIdResult) {
    PIDLIST_ABSOLUTE* pidls = nullptr;
    LONG eventId = 0;
    HANDLE notificationLock = SHChangeNotification_Lock(
        reinterpret_cast<HANDLE>(wParam), static_cast<DWORD>(lParam), &pidls,
        &eventId);
    if (!notificationLock) {
        return false;
    }

    constexpr LONG directDriveEvents =
        SHCNE_DRIVEADD | SHCNE_DRIVEREMOVED | SHCNE_MEDIAINSERTED |
        SHCNE_MEDIAREMOVED | SHCNE_FREESPACE;
    bool relevant = (eventId & directDriveEvents) != 0;
    // Matches kShellDriveEvents: only the events the mod actually registers
    // for can ever arrive here.
    constexpr LONG computerChildEvents = SHCNE_CREATE | SHCNE_DELETE |
                                         SHCNE_MKDIR | SHCNE_RMDIR |
                                         SHCNE_RENAMEITEM |
                                         SHCNE_RENAMEFOLDER;
    if (!relevant && (eventId & computerChildEvents) && pidls) {
        bool isDriveRoot =
            IsDriveRootPidl(pidls[0]) || IsDriveRootPidl(pidls[1]);
        relevant = isDriveRoot;
        if (!relevant) {
            if (PCIDLIST_ABSOLUTE computerPidl = GetCachedComputerPidl()) {
                relevant =
                    IsDirectChildOfComputerFolder(pidls[0], computerPidl) ||
                    IsDirectChildOfComputerFolder(pidls[1], computerPidl);
            }
        }
    }

    SHChangeNotification_Unlock(notificationLock);
    if (eventIdResult) {
        *eventIdResult = eventId;
    }
    return relevant;
}

LRESULT CALLBACK DriveKeyboardGetMessageHookProc(int code,
                                                  WPARAM wParam,
                                                  LPARAM lParam) {
    if (code == HC_ACTION && wParam == PM_REMOVE && !g_unloading.load()) {
        auto message = reinterpret_cast<MSG*>(lParam);
        constexpr LPARAM kAltContextBit = static_cast<LPARAM>(1) << 29;
        constexpr LPARAM kPreviousStateBit = static_cast<LPARAM>(1) << 30;
        if (message && message->message == WM_SYSKEYDOWN &&
            message->wParam == VK_RETURN &&
            (message->lParam & kAltContextBit) &&
            !(message->lParam & kPreviousStateBit) &&
            !(GetKeyState(VK_CONTROL) & 0x8000) && FindFocusedDriveCard()) {
            HWND owner = GetExplorerWindowForCurrentThread();
            if (owner &&
                PostMessageW(owner, GetInvokeDrivePropertiesMessage(), 0, 0)) {
                // Alt+Enter is a system-key message and doesn't enter WinUI's
                // routed KeyDown path. Replace it before chaining so
                // downstream hooks observe the same replacement.
                message->message = WM_NULL;
                message->wParam = 0;
                message->lParam = 0;
            }
        }
    }

    return CallNextHookEx(nullptr, code, wParam, lParam);
}

bool EnsureDriveKeyboardMessageHookForCurrentThread() {
    if (g_driveKeyboardMessageHook) {
        return true;
    }

    g_driveKeyboardMessageHook = SetWindowsHookExW(
        WH_GETMESSAGE, DriveKeyboardGetMessageHookProc, nullptr,
        GetCurrentThreadId());
    if (!g_driveKeyboardMessageHook) {
        Wh_Log(L"Couldn't install the drive keyboard message hook: %08X",
               GetLastError());
        return false;
    }
    return true;
}

void RemoveDriveKeyboardMessageHookForCurrentThread() {
    if (!g_driveKeyboardMessageHook) {
        return;
    }

    UnhookWindowsHookEx(g_driveKeyboardMessageHook);
    g_driveKeyboardMessageHook = nullptr;
}

LRESULT CALLBACK ExplorerWindowSubclassProc(HWND window, UINT message,
                                             WPARAM wParam, LPARAM lParam,
                                             UINT_PTR subclassId,
                                             DWORD_PTR) {
    if (g_trackedContextMenu3 &&
        (message == WM_INITMENUPOPUP || message == WM_DRAWITEM ||
         message == WM_MEASUREITEM || message == WM_MENUCHAR)) {
        LRESULT menuResult = 0;
        if (SUCCEEDED(g_trackedContextMenu3->HandleMenuMsg2(
                message, wParam, lParam, &menuResult))) {
            return menuResult;
        }
    } else if (g_trackedContextMenu2 &&
               (message == WM_INITMENUPOPUP || message == WM_DRAWITEM ||
                message == WM_MEASUREITEM)) {
        if (SUCCEEDED(g_trackedContextMenu2->HandleMenuMsg(message, wParam,
                                                           lParam))) {
            return 0;
        }
    }

    if (message == GetApplyDriveSnapshotMessage()) {
        if (!g_unloading.load()) {
            RefreshDevicesSectionsForCurrentThread();
        }
        return 0;
    }

    if (message == GetFocusDriveRenameMessage()) {
        if (!g_unloading.load()) {
            FocusPendingDriveRenameForCurrentThread();
        }
        return 0;
    }

    if (message == GetInvokeDrivePropertiesMessage()) {
        if (!g_unloading.load()) {
            InvokeFocusedDriveProperties(window);
        }
        return 0;
    }

    if (message == GetToggleDevicesExpandedMessage()) {
        if (!g_unloading.load()) {
            ApplyDevicesExpandedState(wParam != 0);
        }
        return 0;
    }

    if (message == WM_SETTINGCHANGE && !g_unloading.load()) {
        UpdateDriveSelectionCheckBoxesForCurrentThread();
    }

    if (message == GetShellChangeMessage()) {
        // Called unconditionally, even while unloading: it's what runs
        // SHChangeNotification_Lock/Unlock, which releases this delivery's
        // SHCNRF_NewDelivery shared-memory entry regardless of whether the
        // event ends up mattering. Only the resulting refresh is skipped.
        LONG eventId = 0;
        bool relevant = IsRelevantShellDriveEvent(wParam, lParam, &eventId);
        if (relevant && !g_unloading.load()) {
            RequestDriveRefresh(
                (eventId & ~SHCNE_FREESPACE) ? kDriveRefreshTopology
                                             : kDriveRefreshCapacity);
        }
        return 0;
    }

    // Unconditional, not gated on whether Shell notifications are also
    // registered: a portable/MTP device's arrival can surface from the
    // Shell only as a generic SHCNE_UPDATEDIR anchored at the Desktop root,
    // which IsRelevantShellDriveEvent has no PIDL-based way to recognize as
    // relevant. DBT_DEVICEARRIVAL is what actually catches that case, so it
    // has to keep firing even when Shell registration also succeeded.
    if (message == WM_DEVICECHANGE && !g_unloading.load()) {
        switch (wParam) {
            case DBT_DEVICEARRIVAL:
            case DBT_DEVICEREMOVECOMPLETE:
            case DBT_DEVNODES_CHANGED:
                RequestDriveRefresh();
                break;
        }
    }

    if (message == WM_NCDESTROY) {
        auto registration = g_windowNotificationRegistrations.find(window);
        if (registration != g_windowNotificationRegistrations.end()) {
            if (registration->second) {
                SHChangeNotifyDeregister(registration->second);
            }
            g_windowNotificationRegistrations.erase(registration);
        }
        RemoveRegisteredWindow(window);
        if (g_windowNotificationRegistrations.empty()) {
            RemoveDriveKeyboardMessageHookForCurrentThread();
            ClearHomeSelectionEventHandlersForCurrentThread();
            ClearDriveCardEventHandlersForCurrentThread();
            ClearDevicesHeaderEventHandlersForCurrentThread();
            ClearTrackedHomePanelsForCurrentThread();
            ReleaseNavigationControllerForCurrentThread();
        }
        RemoveWindowSubclass(window, ExplorerWindowSubclassProc, subclassId);
    }

    return DefSubclassProc(window, message, wParam, lParam);
}

bool RegisterShellNotificationsForWindow(HWND window) {
    if (g_windowNotificationRegistrations.contains(window)) {
        return true;
    }

    if (!SetWindowSubclass(window, ExplorerWindowSubclassProc,
                           kExplorerWindowSubclassId, 0)) {
        Wh_Log(L"Couldn't subclass Explorer window %p", window);
        return false;
    }
    EnsureDriveKeyboardMessageHookForCurrentThread();

    // Registered on FOLDERID_ComputerFolder (not Desktop), non-recursive:
    // a non-recursive registration still delivers events for the folder's
    // direct children, which is exactly what IsRelevantShellDriveEvent
    // accepts (a drive root, or a direct child of Computer) -- drive
    // add/remove/media/free-space/rename are all delivered this way.
    // fRecursive=TRUE would put every file on every drive back in scope.
    // SHCNRF_InterruptLevel is dropped too: Computer is a virtual folder,
    // not a real file-system directory, so there's no raw file-system
    // driver source of notifications for it to receive. Portable/MTP
    // device arrival is handled separately via WM_DEVICECHANGE since a
    // connected device can surface here only as a generic SHCNE_UPDATEDIR
    // anchored at the Desktop root, unreachable from a Computer-scoped
    // registration either way.
    PIDLIST_ABSOLUTE computerPidl = nullptr;
    if (FAILED(SHGetKnownFolderIDList(FOLDERID_ComputerFolder,
                                      KF_FLAG_DEFAULT, nullptr,
                                      &computerPidl)) ||
        !computerPidl) {
        g_windowNotificationRegistrations.emplace(window, 0);
        AddRegisteredWindow(window);
        Wh_Log(L"Couldn't resolve the Computer PIDL; using WM_DEVICECHANGE");
        return true;
    }

    SHChangeNotifyEntry entry{computerPidl, FALSE};
    ULONG registrationId = SHChangeNotifyRegister(
        window, SHCNRF_ShellLevel | SHCNRF_NewDelivery, kShellDriveEvents,
        GetShellChangeMessage(), 1, &entry);
    CoTaskMemFree(computerPidl);

    if (!registrationId) {
        g_windowNotificationRegistrations.emplace(window, 0);
        AddRegisteredWindow(window);
        Wh_Log(L"Couldn't register Shell notifications; using WM_DEVICECHANGE");
        return true;
    }

    g_windowNotificationRegistrations.emplace(window, registrationId);
    AddRegisteredWindow(window);
    return true;
}

void EnsureShellNotificationsForCurrentThread() {
    DWORD currentThreadId = GetCurrentThreadId();
    for (HWND window : GetFileExplorerWindows()) {
        if (GetWindowThreadProcessId(window, nullptr) == currentThreadId) {
            RegisterShellNotificationsForWindow(window);
        }
    }
}

void RemoveShellNotificationsForCurrentThread() {
    auto registrations = std::move(g_windowNotificationRegistrations);
    g_windowNotificationRegistrations.clear();

    for (auto const& [window, registrationId] : registrations) {
        if (registrationId) {
            SHChangeNotifyDeregister(registrationId);
        }
        RemoveRegisteredWindow(window);
        if (IsWindow(window)) {
            RemoveWindowSubclass(window, ExplorerWindowSubclassProc,
                                 kExplorerWindowSubclassId);
        }
    }
    RemoveDriveKeyboardMessageHookForCurrentThread();
}

void DismissOpenContextMenus() {
    unsigned waitIterations = 0;
    while (g_openContextMenuCount.load() > 0) {
        std::vector<HWND> windows;
        {
            std::lock_guard lock(g_registeredWindowsMutex);
            windows = g_registeredWindows;
        }

        for (HWND window : windows) {
            if (IsWindow(window)) {
                PostMessageW(window, WM_CANCELMODE, 0, 0);
            }
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        if (++waitIterations % 100 == 0) {
            Wh_Log(L"Waiting for %d drive context menu(s) to close",
                   g_openContextMenuCount.load());
        }
    }

    // InvokeCommand can be running modal Shell UI (Format, Properties, an
    // Eject confirmation) that WM_CANCELMODE cannot dismiss. Just wait for
    // it, without spinning a cancel attempt that can't succeed.
    unsigned logIterations = 0;
    while (g_pendingCommandInvocations.load() > 0) {
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        if (++logIterations % 25 == 0) {
            Wh_Log(L"Waiting for %d drive command dialog(s) to close",
                   g_pendingCommandInvocations.load());
        }
    }

    // Same reasoning for RenameDriveWithShell's SetNameOf (elevation can put
    // up a UAC/error dialog) and the ExecuteShellParsingName fallback.
    // CompleteDriveRename already checks !g_unloading before starting a
    // rename, so no new one can start after this point; the wait is bounded
    // by whichever single call was already in flight.
    unsigned shellUiLogIterations = 0;
    while (g_pendingShellUiCalls.load() > 0) {
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        if (++shellUiLogIterations % 25 == 0) {
            Wh_Log(L"Waiting for %d drive Shell UI call(s) to finish",
                   g_pendingShellUiCalls.load());
        }
    }
}

void WINAPI RemoveDevicesSectionsForCurrentThreadProc(void*) {
    RemoveShellNotificationsForCurrentThread();
    RemoveDevicesSectionsForCurrentThread();
    // Runs here (via RunFromWindowThread) so the release happens on the
    // thread that created the controller, not on whichever Windhawk thread
    // is running Wh_ModUninit.
    ReleaseNavigationControllerForCurrentThread();
}

// Shared by Wh_ModBeforeUninit and Wh_ModUninit: Wh_ModBeforeUninit always
// runs first in a normal unload, but every step here is idempotent, so
// calling it from both is a harmless safety net rather than a real repeat.
void DrainModStateForUnload() {
    g_unloading.store(true);
    DismissOpenContextMenus();
    WaitForDriveDragPreparations();
    WaitForDriveDropOperations();
    StopDriveRefreshWorker();
}

}  // namespace

BOOL Wh_ModInit() {
    g_unloading.store(false);

    bool needsLoadLibraryHook = false;
    if (GetModuleHandleW(L"FileExplorerExtensions.dll")) {
        if (!HookFileExplorerExtensionsIfLoaded(false)) {
            return FALSE;
        }
    } else {
        needsLoadLibraryHook = true;
    }

    if (GetModuleHandleW(L"Windows.UI.FileExplorer.dll")) {
        if (!HookWindowsUiFileExplorerIfLoaded(false)) {
            return FALSE;
        }
    } else {
        needsLoadLibraryHook = true;
    }

    if (needsLoadLibraryHook) {
        HMODULE kernelBase = GetModuleHandleW(L"kernelbase.dll");
        if (!kernelBase) {
            Wh_Log(L"Failed to get kernelbase.dll");
            return FALSE;
        }

        auto loadLibraryExW = reinterpret_cast<LoadLibraryExW_t>(
            GetProcAddress(kernelBase, "LoadLibraryExW"));
        if (!loadLibraryExW ||
            !WindhawkUtils::SetFunctionHook(loadLibraryExW, LoadLibraryExW_Hook,
                                            &LoadLibraryExW_Original)) {
            Wh_Log(L"Failed to hook LoadLibraryExW");
            return FALSE;
        }
    }

    if (!StartDriveRefreshWorker()) {
        return FALSE;
    }

    return TRUE;
}

void Wh_ModAfterInit() {
    HookFileExplorerExtensionsIfLoaded(true);
    HookWindowsUiFileExplorerIfLoaded(true);
    RequestInitialDriveSnapshot();
}

void Wh_ModBeforeUninit() {
    DrainModStateForUnload();
}

void Wh_ModUninit() {
    // Not DrainModStateForUnload() again, and not another g_unloading.store:
    // Wh_ModBeforeUninit always runs first, and DrainModStateForUnload
    // already did both while hooks were still active.
    for (HWND window : GetFileExplorerWindows()) {
        if (!RunFromWindowThread(
                window, RemoveDevicesSectionsForCurrentThreadProc, nullptr)) {
            Wh_Log(L"Couldn't clean Explorer window %p", window);
        }
    }

    // A navigation controller whose owning thread couldn't be reached above
    // (e.g. its window is already gone) is deliberately left alive rather
    // than released here: per the thread-affine ownership wiki, if no UI
    // thread can be found to release from, it's better to leak a COM ref
    // than to release one from the wrong thread.
}
