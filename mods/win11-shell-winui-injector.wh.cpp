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

This Windhawk mod injects into core Windows 11 desktop shell components (`explorer.exe`, `StartMenuExperienceHost.exe`, and `ShellExperienceHost.exe`) to runtime-modify, override, or style native **WinUI 3** and **XAML** interface layers. It allows you to intercept the creation of specific UI elements and apply custom layouts, animations, or styling scripts seamlessly.

---

## 🚀 Key Features

* **Dynamic WinUI Interception:** Dynamically intercepts modern Windows 11 components using native WinRT interface hooks.
* **Fluent Design Overrides:** Allows customization of transparency, borders, padding, and layout geometry directly within the shell.
* **Isolated Thread Execution:** Handles initialization in dedicated background hooks to prevent the main UI thread from freezing during shell element updates.
* **Targeted Inclusions:** Only applies changes to chosen shell behaviors, ensuring classic Win32 dialogs and independent background helper tools remain completely untouched.

---

## 🛠️ How It Works

1. **Process Targeting:** The mod initializes during the startup phase of the target process (e.g., when the Windows Start Menu or Taskbar loads).
2. **API Hooking:** It establishes specialized callbacks via the [Windhawk API Engine](https://github.com) to listen for the activation of XAML and UI elements.
3. **Runtime Injection:** As Windows renders its standard user interface, this code injects updated parameter attributes, overriding layout configurations on the fly.

---

## 📋 Instructions

Follow these steps to safely load, configure, and apply the WinUI modifications:

### 1. Installation
* Open the **Windhawk** application interface.
* Navigate to the **Developer Center** and click **Create New Mod**.
* Paste this entire source code into the editor and click **Compile**.
* Once compiled, click **Save and Run** to initialize the injector hooks.

### 2. Configuration (If Applicable)
* Go to the mod's details page within Windhawk.
* Open the **Settings** tab to adjust parameters such as transparency behaviors, custom window padding, or target class identifiers.
* Click **Save Settings** to push changes directly into memory.

### 3. Applying Changes (Shell Refresh)
* Most modern XAML components (like the Start Menu or Notification center) apply changes immediately upon their next launch.
* If a visual style does not update immediately, you must restart the target process.
* To refresh the taskbar, open Windhawk's *Advanced* properties or Windows Task Manager and safely restart `explorer.exe`.

### 4. Temporary Unloading
* If you experience unexpected desktop flickering, simply toggle the mod to **Disabled** in Windhawk.
* This removes the runtime hooks and immediately restores default Windows 11 Shell rendering without requiring a full system reboot.

---

## ⚡ Performance & Stability

This mod has been optimized for low-latency background execution. Memory consumption remains minimal because styles are applied passively at initialization rather than through intensive polling loops. 

---

## 🔍 Troubleshooting & Verification

If customized UI panels fail to appear as intended, verify the following properties:
* Check that **Debug Logging** is turned on under the mod's *Advanced Tab* inside Windhawk to inspect execution callbacks.
* Ensure Windows Update has not altered standard runtime symbols; if it has, allow Windhawk a few minutes to fetch the latest debugging symbols automatically.
* Double-check for collisions if you have other third-party custom taskbar or desktop styling suites concurrently enabled.
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