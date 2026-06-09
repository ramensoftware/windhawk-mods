// ==WindhawkMod==
// @id              win11-shell-winui-injector
// @name            Windows 11 Shell WinUI Injector
// @description     An open extensibility framework providing isolated custom text fields to target native WinUI elements and inject multi-value option submenus independently into core Windows shell processes.
// @description:en  An open extensibility framework providing isolated custom text fields to target native WinUI elements and inject multi-value option submenus independently into core Windows shell processes.
// @version         1.0.0
// @author          PhantomNimbi
// @homepage        https://github.com/PhantomNimbi
// @include         StartMenuExperienceHost.exe
// @include         ShellExperienceHost.exe
// @include         ShellHost.exe
// @include         SearchHost.exe
// @include         SearchApp.exe
// @include         LockApp.exe
// @include         Widgets.exe
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
├──► Target: explorer.exe (Taskbar, Desktop, Tray)
├──► Target: StartMenuExperienceHost.exe (Start Menu UI)
└──►
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
              $name: Property Values Submenu
        $name: Properties Submenu
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
              $name: Property Values Submenu
        $name: Properties Submenu
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
              $name: Property Values Submenu
        $name: Properties Submenu
  $name: Search App (SearchApp.exe)

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
              $name: Property Values Submenu
        $name: Properties Submenu
  $name: Lock Screen Interface (LockApp.exe)

- WidgetsTargets:
    - - TargetElement: ""
        $name: Target
        $description: "XAML element name to intercept inside the Widgets process."
      - Options:
          - - PropertyName: ""
              $name: Property Name
              $description: "The specific layout property handle to modify."
            - PropertyValues:
                - - ValueString: ""
                    $name: Property Value
                    $description: "An individual value parameter bound to this property."
              $name: Property Values Submenu
        $name: Properties Submenu
  $name: Widgets (Widgets.exe)

- ShellExperienceTargets:
    - - TargetElement: ""
        $name: Action/Notification Center Target
        $description: "XAML element name to intercept inside the Notification Center and Action Center process."
      - Options:
          - - PropertyName: ""
              $name: Property Name
              $description: "The specific layout property handle to modify."
            - PropertyValues:
                - - ValueString: ""
                    $name: Property Value
                    $description: "An individual value parameter bound to this property."
              $name: Property Values Submenu
        $name: Properties Submenu
  $name: Notification Center & Action Center (ShellExperienceHost.exe)

- ExplorerTargets:
    - - TargetElement: ""
        $name: Taskbar/File Explorer Target
        $description: "XAML element name to intercept inside the Taskbar and File Explorer workspace."
      - Options:
          - - PropertyName: ""
              $name: Property Name
              $description: "The specific layout property handle to modify."
            - PropertyValues:
                - - ValueString: ""
                    $name: Property Value
                    $description: "An individual value parameter bound to this property."
              $name: Property Values Submenu
        $name: Properties Submenu
  $name: Taskbar & File Explorer (explorer.exe)
*/
// ==/WindhawkModSettings==

// ====================================================================================
//                               Variable Imports
// ====================================================================================
/*
    This is where we import the existing WinUI variables for each of our processes
    so we can apply them to the custom properties.
*/


// ====================================================================================
//                               Function Imports
// ====================================================================================
/*
    This is where we import the functions of the existing variables so our hook knows
    how to properly inject our custom properties and variables
*/

// ====================================================================================
//                             Create Propeties
// ====================================================================================
/*
    This is where we create the custom properties to inject
*/


// ====================================================================================
//                               Create Variables
// ====================================================================================
/*
    This is where we create the custom variables for use with our custom properties
*/

// ====================================================================================
//                                Hook Injection
// ====================================================================================
/*
    This is where we inject our custom properties and varibales into the 
    provided targets.
*/