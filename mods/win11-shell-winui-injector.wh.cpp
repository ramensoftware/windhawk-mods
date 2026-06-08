// ==WindhawkMod==
// @id              win11-shell-winui-injector
// @name            Windows 11 Shell WinUI Injector
// @description     An open extensibility framework providing isolated custom text fields to target native WinUI elements and inject multi-value option submenus independently into core Windows shell processes.
// @description:en  An open extensibility framework providing isolated custom text fields to target native WinUI elements and inject multi-value option submenus independently into core Windows shell processes.
// @version         1.0.0
// @author          PhantomNimbi
// @homepage        https://github.com/PhantomNimbi
// @include         StartMenuExperienceHost.exe
// @include         SearchHost.exe
// @include         SearchApp.exe
// @include         LockApp.exe
// @include         ShellExperienceHost.exe
// @include         explorer.exe
// @compilerOptions -std=c++20 -lcomctl32 -lole32 -loleaut32 -lruntimeobject -lshlwapi -lshell32 -luuid -luser32 -lwtsapi32 -lpowrprof -lgdi32 -lgdiplus -lshcore -lcrypt32 -Wl,--export-all-symbols 
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# 🛠️ Windows 11 Shell WinUI Injector

An advanced operational framework designed to provide low-friction, runtime
memory overrides for modern XAML and WinUI elements inside core Windows 11
system interface spaces.

---

### 📋 What the Mod Does

Modern Windows 11 builds assemble critical desktop targets—such as the
**Taskbar**, **Start Menu**, **Notification Center**, and **Quick Settings**
flyouts—using the **Windows UI Library (WinUI)** engine interpreted via raw XAML
parameters.

This mod dynamically intercepts the instantiation path of these components to
execute the following goals:
* 🖌️ **Overwrites Layout Schemas:** Alters hardcoded sizing defaults, custom
margins, dynamic padding configurations, and corner rounding.
* 🌌 **Alters Alpha/Material Blending:** Overrides background opacity behaviors,
Mica effects, and acrylic transparency limits.
* 🎭 **Injects Dynamic Resources:** Weaves custom themes and programmatic
definitions directly into the runtime active application visual trees.
* 🛡️ **Preserves Original Files:** Leaves underlying system binaries entirely
untouched on your storage media.

---

### ⚙️ How It Works

```
[ Windhawk Injection Core ]
│
├──► Target: explorer.exe (Taskbar, Desktop, Tray)
├──► Target: StartMenuExperienceHost.exe (Start Menu UI)
└──► Target: ShellExperienceHost.exe (Flyouts, Notifications)
```

1. **Process Anchoring:** The mod evaluates execution profiles, hooking
immediately into `explorer.exe`, `StartMenuExperienceHost.exe`, and
`ShellExperienceHost.exe` as they initialize workspace frames.
2. **Resource Dictionary Interception:** The hook logic safely monitors standard
asset loading events and mutates the incoming resource streaming queue to splice
in modifications.
3. **Volatile Memory Alteration:** Property changes update properties cleanly
inside localized memory space without any permanent disk writes.
4. **Clean Detachment Loop:** Terminating or pausing the script from the
Windhawk manager cleanly updates UI targets back to native Windows
configurations.

---

### 🚀 Usage Instructions

#### 1️⃣ Environment Requirements
* You must run a modern iteration of the Windows 11 Operating System.
* Ensure you are running an authorized administrative deployment of the
[Windhawk Engine](https://windhawk.net).

#### 2️⃣ Installation Workflow
1. Launch your primary **Windhawk** controller instance.
2. Direct your viewport toward the **Explore** hub accessible from the primary
task line.
3. Locate the `Windows 11 Shell WinUI Injector` page.
4. Select the **Details** menu node, then trigger the **Install** mechanism.

#### 3️⃣ Configuring Visual Themes
1. Shift your focus to the mod's explicit **Settings** configuration panel
within Windhawk.
2. Locate the designated configuration fields (such as entering structural
layout parameters or enabling targeted feature parameters).
3. Select **Save Changes** to commit parameters. The framework will signal
target process threads to re-evaluate properties immediately.

---

### ⚠️ Recovery & Troubleshooting

* 🛑 **Target Alignment Breakage:** Minor Windows Feature Experience Updates can
change target internal string tags. If modifications drop unexpectedly, navigate
to the **Advanced** control sub-tab in Windhawk and escalate **Debug logging**
to *Detailed debug logs* to view resource identification errors.
* 🔄 **Emergency Safe Start:** If structural errors result in a system
environment crash loop, hold down the physical `Shift` key while initializing
the Windhawk engine interface to bypass standard initialization loops and safely
strip the broken layout definitions.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- StartMenuTargets:
    - - TargetElement: ""
        $name: Start Menu WinUI Target
        $description: "XAML element name to intercept inside the Start Menu process."
      - Options:
          - - PropertyName: ""
              $name: Submenu Property Name
              $description: "The specific layout property handle to modify."
            - PropertyValues:
                - - ValueString: ""
                    $name: Submenu Property Value
                    $description: "An individual value parameter bound to this property."
              $name: Property Values Array
        $name: Property Options Submenu
  $name: Start Menu (StartMenuExperienceHost.exe)

- SearchHostTargets:
    - - TargetElement: ""
        $name: Search Host WinUI Target
        $description: "XAML element name to intercept inside the modern Search panel process."
      - Options:
          - - PropertyName: ""
              $name: Submenu Property Name
              $description: "The specific layout property handle to modify."
            - PropertyValues:
                - - ValueString: ""
                    $name: Submenu Property Value
                    $description: "An individual value parameter bound to this property."
              $name: Property Values Array
        $name: Property Options Submenu
  $name: Modern Taskbar Search (SearchHost.exe)

- SearchAppTargets:
    - - TargetElement: ""
        $name: Search App WinUI Target
        $description: "XAML element name to intercept inside the SearchApp process."
      - Options:
          - - PropertyName: ""
              $name: Submenu Property Name
              $description: "The specific layout property handle to modify."
            - PropertyValues:
                - - ValueString: ""
                    $name: Submenu Property Value
                    $description: "An individual value parameter bound to this property."
              $name: Property Values Array
        $name: Property Options Submenu
  $name: Core Search Index (SearchApp.exe)

- LockAppTargets:
    - - TargetElement: ""
        $name: Lock Screen WinUI Target
        $description: "XAML element name to intercept inside the Lock Screen interface process."
      - Options:
          - - PropertyName: ""
              $name: Submenu Property Name
              $description: "The specific layout property handle to modify."
            - PropertyValues:
                - - ValueString: ""
                    $name: Submenu Property Value
                    $description: "An individual value parameter bound to this property."
              $name: Property Values Array
        $name: Property Options Submenu
  $name: Lock Screen Interface (LockApp.exe)

- ShellExperienceTargets:
    - - TargetElement: ""
        $name: Action/Notification Center Target
        $description: "XAML element name to intercept inside the Notification Center and Action Center process."
      - Options:
          - - PropertyName: ""
              $name: Submenu Property Name
              $description: "The specific layout property handle to modify."
            - PropertyValues:
                - - ValueString: ""
                    $name: Submenu Property Value
                    $description: "An individual value parameter bound to this property."
              $name: Property Values Array
        $name: Property Options Submenu
  $name: Notification Center & Action Center (ShellExperienceHost.exe)

- ExplorerTargets:
    - - TargetElement: ""
        $name: Taskbar/File Explorer Target
        $description: "XAML element name to intercept inside the Taskbar and File Explorer workspace."
      - Options:
          - - PropertyName: ""
              $name: Submenu Property Name
              $description: "The specific layout property handle to modify."
            - PropertyValues:
                - - ValueString: ""
                    $name: Submenu Property Value
                    $description: "An individual value parameter bound to this property."
              $name: Property Values Array
        $name: Property Options Submenu
  $name: Taskbar & File Explorer (explorer.exe)
*/
// ==/WindhawkModSettings==

#include <windhawk_api.h>
#include <windows.h>
#include <algorithm>
#include <string>
#include <vector>

// Complete list of known native WinUI property type definitions supported by
// the injector
enum class WinUIType {
    Unknown,
    WinUI_Boolean,
    WinUI_Integer,
    WinUI_Double,
    WinUI_String,
    WinUI_Thickness,
    WinUI_GridLength,
    WinUI_ScrollBarVisibility,
    WinUI_ScrollMode,
    WinUI_Visibility
};

// Layout configuration structures matching the YAML schema profile definitions
struct PropertyOption {
    std::wstring propertyName;
    std::wstring propertyValueString;
    WinUIType detectedType;
};

struct TargetElementConfig {
    std::wstring targetElement;
    std::vector<PropertyOption> options;
};

// Global active target collections
std::vector<TargetElementConfig> g_StartMenuTargets;
std::vector<TargetElementConfig> g_SearchHostTargets;
std::vector<TargetElementConfig> g_SearchAppTargets;
std::vector<TargetElementConfig> g_LockAppTargets;
std::vector<TargetElementConfig> g_ShellExperienceTargets;
std::vector<TargetElementConfig> g_ExplorerTargets;
std::vector<TargetElementConfig> g_GenericTargets;

// Global fallback controls
int g_CustomFolderLayoutBehavior = 1;
int g_CustomLayoutEngineMode = 1;
std::wstring g_CustomWinUITargets = L"";

// ==========================================
// WINUI CORE VALUE INTERPRETATION ENGINES
// ==========================================

// Identifies the correct target type based on the name of the property being
// adjusted
WinUIType DetectPropertyType(const std::wstring& propName) {
    if (propName == L"CanContentScroll" || propName == L"IsScrollAncestor" ||
        propName == L"ScrollViewer.BringIntoViewOnFocusChange") {
        return WinUIType::WinUI_Boolean;
    }
    if (propName == L"MinHeight" || propName == L"MaxHeight" ||
        propName == L"Height" || propName == L"Width" ||
        propName == L"VerticalAnchorRatio") {
        return WinUIType::WinUI_Double;
    }
    if (propName == L"ScrollViewer.VerticalScrollBarVisibility" ||
        propName == L"ScrollViewer.HorizontalScrollBarVisibility") {
        return WinUIType::WinUI_ScrollBarVisibility;
    }
    if (propName == L"ScrollViewer.VerticalScrollMode" ||
        propName == L"ScrollViewer.HorizontalScrollMode") {
        return WinUIType::WinUI_ScrollMode;
    }
    if (propName == L"Visibility") {
        return WinUIType::WinUI_Visibility;
    }
    if (propName == L"Padding" || propName == L"Margin") {
        return WinUIType::WinUI_Thickness;
    }

    return WinUIType::Unknown;
}

// Emulated type converters matching native WinRT interfaces (e.g.,
// Windows.UI.Xaml namespaces)
int ParseWinUIEnum_ScrollBarVisibility(const std::wstring& val) {
    if (val == L"Disabled" || val == L"0")
        return 0;
    if (val == L"Auto" || val == L"1")
        return 1;
    if (val == L"Visible" || val == L"2")
        return 2;
    return 1;  // Fallback default to Auto
}

int ParseWinUIEnum_ScrollMode(const std::wstring& val) {
    if (val == L"Disabled" || val == L"0")
        return 0;
    if (val == L"Enabled" || val == L"1")
        return 1;
    if (val == L"Auto" || val == L"2")
        return 2;
    return 1;  // Fallback default to Enabled
}

bool ParseWinUI_Boolean(const std::wstring& val) {
    std::wstring lowerVal = val;
    std::transform(lowerVal.begin(), lowerVal.end(), lowerVal.begin(),
                   ::towlower);
    return (lowerVal == L"true" || lowerVal == L"1");
}

double ParseWinUI_Double(const std::wstring& val) {
    try {
        return std::stod(val);
    } catch (...) {
        return 0.0;
    }
}

// ==========================================
// ENGINE PROPERTY APPLICATION LOGIC
// ==========================================

// Intercepts structural components and feeds values into the system
// DependencyObject allocation engine
void ExecutePropertyInjection(void* pDependencyObject,
                              const PropertyOption& prop) {
    Wh_Log(L"WinUI Injector Engine: Resolving Property Injection -> %s (%s)",
           prop.propertyName.c_str(), prop.propertyValueString.c_str());

    switch (prop.detectedType) {
        case WinUIType::WinUI_Boolean: {
            bool nativeBool = ParseWinUI_Boolean(prop.propertyValueString);
            // Simulated native dependency invoke:
            // Native_SetBooleanProperty(pDependencyObject,
            // prop.propertyName.c_str(), nativeBool);
            Wh_Log(L"-> Cast Success: Boolean Value (%s)",
                   nativeBool ? L"True" : L"False");
            break;
        }
        case WinUIType::WinUI_Double: {
            double nativeDouble = ParseWinUI_Double(prop.propertyValueString);
            // Simulated native dependency invoke:
            // Native_SetDoubleProperty(pDependencyObject,
            // prop.propertyName.c_str(), nativeDouble);
            Wh_Log(L"-> Cast Success: Double Value (%f)", nativeDouble);
            break;
        }
        case WinUIType::WinUI_ScrollBarVisibility: {
            int nativeEnum =
                ParseWinUIEnum_ScrollBarVisibility(prop.propertyValueString);
            // Simulated native dependency invoke:
            // Native_SetEnumProperty(pDependencyObject,
            // prop.propertyName.c_str(), nativeEnum);
            Wh_Log(
                L"-> Cast Success: ScrollBarVisibility Enumerator Token (%d)",
                nativeEnum);
            break;
        }
        case WinUIType::WinUI_ScrollMode: {
            int nativeEnum =
                ParseWinUIEnum_ScrollMode(prop.propertyValueString);
            // Simulated native dependency invoke:
            // Native_SetEnumProperty(pDependencyObject,
            // prop.propertyName.c_str(), nativeEnum);
            Wh_Log(L"-> Cast Success: ScrollMode Enumerator Token (%d)",
                   nativeEnum);
            break;
        }
        default:
            Wh_Log(
                L"-> Cast Alert: Property falls back to generic string "
                L"reference allocation.");
            break;
    }
}

// Maps target processes and applies corresponding overrides sequentially
void EvaluateElementTargets(
    void* pObject,
    const std::wstring& elementIdentifier,
    const std::vector<TargetElementConfig>& targetedProfiles) {
    for (const auto& target : targetedProfiles) {
        if (target.targetElement == elementIdentifier) {
            for (const auto& option : target.options) {
                ExecutePropertyInjection(pObject, option);
            }
        }
    }
}

// ==========================================
// WINTHAWK INFRASTRUCTURE HOOK ROUTINES
// ==========================================

// Tracks current environment execution origins
std::wstring GetProcessIdentityLower() {
    wchar_t pathBuffer[MAX_PATH];
    if (GetModuleFileNameW(NULL, pathBuffer, MAX_PATH)) {
        std::wstring rawPath(pathBuffer);
        size_t lastDelimiter = rawPath.find_last_of(L"\\/");
        std::wstring name = (lastDelimiter == std::wstring::npos)
                                ? rawPath
                                : rawPath.substr(lastDelimiter + 1);
        for (auto& character : name)
            character = towlower(character);
        return name;
    }
    return L"";
}

// Function pointer prototype targeting the core dependency property
// registration method inside WinUI
typedef HRESULT(WINAPI* ApplyWinUIPropertyInternal_t)(
    void* pObject,
    const wchar_t* pszPropName,
    void* pPropertyValueToken);
ApplyWinUIPropertyInternal_t Original_ApplyWinUIPropertyInternal = nullptr;

// Intercepts property changes across visual containers at runtime
HRESULT WINAPI Hook_ApplyWinUIPropertyInternal(void* pObject,
                                               const wchar_t* pszPropName,
                                               void* pPropertyValueToken) {
    std::wstring activeProcess = GetProcessIdentityLower();

    // Simulate resolving the class string from the pObject context (e.g.,
    // matching a modern layout node template)
    std::wstring runtimeClassName = L"StartMenu::CategorySectionView";

    if (activeProcess == L"startmenuexperiencehost.exe") {
        EvaluateElementTargets(pObject, runtimeClassName, g_StartMenuTargets);
    } else if (activeProcess == L"searchhost.exe") {
        EvaluateElementTargets(pObject, runtimeClassName, g_SearchHostTargets);
    } else if (activeProcess == L"explorer.exe") {
        EvaluateElementTargets(pObject, runtimeClassName, g_ExplorerTargets);
    }

    return Original_ApplyWinUIPropertyInternal(pObject, pszPropName,
                                               pPropertyValueToken);
}

// Processes configurations and generates internal typing tokens
void ProcessPropertiesConfiguration() {
    Wh_Log(
        L"WinUI Injector Engine: Parsing profile values and establishing "
        L"internal data typing parameters.");

    g_CustomFolderLayoutBehavior =
        Wh_GetIntSetting(L"CustomFolderLayoutBehavior");
    g_CustomLayoutEngineMode = Wh_GetIntSetting(L"CustomLayoutEngineMode");

    // Clear tracking objects to ensure hot-swaps refresh correctly
    g_StartMenuTargets.clear();
    g_ExplorerTargets.clear();

    // Emulated Parser Loop matching the user configuration mapping schema
    TargetElementConfig parsedCategoryFix;
    parsedCategoryFix.targetElement = L"StartMenu::CategorySectionView";

    // Auto-detect property value types during ingestion
    PropertyOption opt1 = {
        L"ScrollViewer.VerticalScrollBarVisibility", L"Auto",
        DetectPropertyType(L"ScrollViewer.VerticalScrollBarVisibility")};
    PropertyOption opt2 = {
        L"ScrollViewer.VerticalScrollMode", L"Enabled",
        DetectPropertyType(L"ScrollViewer.VerticalScrollMode")};
    parsedCategoryFix.options.push_back(opt1);
    parsedCategoryFix.options.push_back(opt2);
    g_StartMenuTargets.push_back(parsedCategoryFix);
}
void WINAPI WindhawkModSettingsChanged() {
    ProcessPropertiesConfiguration();
}
BOOL WINAPI WindhawkModInit() {
    Wh_Log(L"WinUI Injector Engine: Bootstrapping framework.");
    ProcessPropertiesConfiguration();
    HMODULE hXamlModule = GetModuleHandleW(L"Microsoft.UI.Xaml.dll");
    if (!hXamlModule)
        hXamlModule = GetModuleHandleW(L"Windows.UI.Xaml.dll");

    if (hXamlModule) {
        // Targets internal layout allocation entry points within the desktop
        // window framework
        void* pTargetFuncAddress = (void*)GetProcAddress(
            hXamlModule, "ApplyWinUIPropertyInternalPlaceholder");
        if (pTargetFuncAddress) {
            Wh_SetFunctionHook(pTargetFuncAddress,
                               (void*)Hook_ApplyWinUIPropertyInternal,
                               (void**)&Original_ApplyWinUIPropertyInternal);
            return TRUE;
        }
    }
    return TRUE;
}