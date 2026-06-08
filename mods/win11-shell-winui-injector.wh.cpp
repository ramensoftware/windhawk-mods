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
# Windows 11 Shell WinUI Injector

This framework offers fully isolated configuration environments to target live XAML/WinUI nodes across the principal layout wrappers of the Windows 11 shell interface.

## Supported Shell Processes
- **StartMenuExperienceHost.exe:** Native Start Menu layout engine.
- **SearchHost.exe:** Modern taskbar Search layout canvas.
- **SearchApp.exe:** Legacy or enterprise localized Search index panel.
- **LockApp.exe:** Interactive Windows Lock Screen panel.
- **ShellExperienceHost.exe:** Universal host process governing the **Notification Center** and **Action Center** (Quick Settings) layout flyouts.
- **explorer.exe:** System core workspace governing the Windows **Taskbar** and **File Explorer**.

## UI Usage Guide
Each shell process has its own dedicated settings category block to prevent configuration cross-contamination.
- **Add Target Item:** Instantiates a target monitoring frame bound to a specific runtime XAML class name (e.g., `ScrollViewer`).
- **Add Option Submenu:** Appends a layout property definition (e.g., `Margin`) chained exclusively to that element.
- **Add Value Box:** Appends an additional variable box inside a specific property submenu, providing full compatibility with multi-argument parameters.
- All dashboard inputs default to an empty slate, leaving your shell processes unmanipulated until target constraints are declared.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- StartMenuTargets:
    - - TargetElement: ""
        $name: Target
        $description: "XAML element name to intercept inside the Start Menu process."
      - Options:
          - - PropertyName: ""
              $name: Property Name
              $description: "The specific layout property handle to modify."
            - PropertyValues:
                - - ValueString: ""
                    $name: Property Value
                    $description: "An individual value parameter bound to this property."
              $name: Property Values Array
        $name: Property Options Submenu
  $name: Start Menu (StartMenuExperienceHost.exe)

- SearchHostTargets:
    - - TargetElement: ""
        $name: Target
        $description: "XAML element name to intercept inside the modern Search panel process."
      - Options:
          - - PropertyName: ""
              $name: Property Name
              $description: "The specific layout property handle to modify."
            - PropertyValues:
                - - ValueString: ""
                    $name: Property Value
                    $description: "An individual value parameter bound to this property."
              $name: Property Values Array
        $name: Property Options Submenu
  $name: Modern Taskbar Search (SearchHost.exe)

- SearchAppTargets:
    - - TargetElement: ""
        $name: Target
        $description: "XAML element name to intercept inside the SearchApp process."
      - Options:
          - - PropertyName: ""
              $name: Property Name
              $description: "The specific layout property handle to modify."
            - PropertyValues:
                - - ValueString: ""
                    $name: Property Value
                    $description: "An individual value parameter bound to this property."
              $name: Property Values Array
        $name: Property Options Submenu
  $name: Core Search Index (SearchApp.exe)

- LockAppTargets:
    - - TargetElement: ""
        $name: Target
        $description: "XAML element name to intercept inside the Lock Screen interface process."
      - Options:
          - - PropertyName: ""
              $name: Property Name
              $description: "The specific layout property handle to modify."
            - PropertyValues:
                - - ValueString: ""
                    $name: Property Value
                    $description: "An individual value parameter bound to this property."
              $name: Property Values Array
        $name: Property Options Submenu
  $name: Lock Screen Interface (LockApp.exe)

- ShellExperienceTargets:
    - - TargetElement: ""
        $name: Target
        $description: "XAML element name to intercept inside the Notification Center and Action Center process."
      - Options:
          - - PropertyName: ""
              $name: Property Name
              $description: "The specific layout property handle to modify."
            - PropertyValues:
                - - ValueString: ""
                    $name: Property Value
                    $description: "An individual value parameter bound to this property."
              $name: Property Values Array
        $name: Property Options Submenu
  $name: Notification Center & Action Center (ShellExperienceHost.exe)

- ExplorerTargets:
    - - TargetElement: ""
        $name: Target
        $description: "XAML element name to intercept inside the Taskbar and File Explorer workspace."
      - Options:
          - - PropertyName: ""
              $name: Property Name
              $description: "The specific layout property handle to modify."
            - PropertyValues:
                - - ValueString: ""
                    $name: Property Value
                    $description: "An individual value parameter bound to this property."
              $name: Property Values Array
        $name: Property Options Submenu
  $name: Taskbar & File Explorer (explorer.exe)
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <inspectable.h>
#include <roapi.h>
#include <winstring.h>
#include <vector>
#include <string>
#include <cstdint>

// Internal storage mapping the three-dimensional nested Windhawk YAML layout hierarchy
struct SubmenuOption {
    std::wstring propName;
    std::vector<std::wstring> propValues;
};

struct ElementTarget {
    std::wstring targetClass;
    std::vector<SubmenuOption> elementOptions;
};

std::vector<ElementTarget> g_ActiveProcessTargets;

// Helper function to extract and verify the current host process identity
std::wstring GetCurrentProcessImageName() {
    wchar_t buffer[MAX_PATH] = {0};
    GetModuleFileNameW(NULL, buffer, MAX_PATH);
    std::wstring fullPath(buffer);
    size_t lastSlash = fullPath.find_last_of(L"\\");
    if (lastSlash != std::wstring::npos) {
        return fullPath.substr(lastSlash + 1);
    }
    return L"";
}

// Parses only the specific configuration block matching the current injected process workspace
void LoadProcessSpecificConfiguration() {
    g_ActiveProcessTargets.clear();
    
    std::wstring processName = GetCurrentProcessImageName();
    std::wstring settingsPrefix = L"";

    // Determine settings root based strictly on process context execution boundaries
    if (wcsicmp(processName.c_str(), L"StartMenuExperienceHost.exe") == 0) {
        settingsPrefix = L"StartMenuTargets";
    } else if (wcsicmp(processName.c_str(), L"SearchHost.exe") == 0) {
        settingsPrefix = L"SearchHostTargets";
    } else if (wcsicmp(processName.c_str(), L"SearchApp.exe") == 0) {
        settingsPrefix = L"SearchAppTargets";
    } else if (wcsicmp(processName.c_str(), L"LockApp.exe") == 0) {
        settingsPrefix = L"LockAppTargets";
    } else if (wcsicmp(processName.c_str(), L"ShellExperienceHost.exe") == 0) {
        settingsPrefix = L"ShellExperienceTargets";
    } else if (wcsicmp(processName.c_str(), L"explorer.exe") == 0) {
        settingsPrefix = L"ExplorerTargets";
    } else {
        return; // Current target context unmapped
    }

    int targetIdx = 0;
    while (true) {
        // Construct dynamic setting query strings using Windhawk format markers
        std::wstring targetQuery = settingsPrefix + L"[%d].TargetElement";
        const wchar_t* rawTarget = Wh_GetStringSetting(targetQuery.c_str(), targetIdx);
        if (!rawTarget) break;

        ElementTarget element;
        element.targetClass = rawTarget;

        int optionIdx = 0;
        while (true) {
            std::wstring propQuery = settingsPrefix + L"[%d].Options[%d].PropertyName";
            const wchar_t* rawProp = Wh_GetStringSetting(propQuery.c_str(), targetIdx, optionIdx);
            if (!rawProp) break;

            SubmenuOption opt;
            opt.propName = rawProp;

            int valueIdx = 0;
            while (true) {
                std::wstring valQuery = settingsPrefix + L"[%d].Options[%d].PropertyValues[%d].ValueString";
                const wchar_t* rawVal = Wh_GetStringSetting(valQuery.c_str(), targetIdx, optionIdx, valueIdx);
                if (!rawVal) break;

                opt.propValues.push_back(rawVal);
                valueIdx++;
            }

            element.elementOptions.push_back(opt);
            optionIdx++;
        }

    }
}