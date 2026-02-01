// ==WindhawkMod==
// @id              icon-resource-redirect
// @name            Resource Redirect
// @description     Define alternative files for loading various resources (e.g. icons in imageres.dll) for simple theming without having to modify system files
// @version         1.2.4
// @author          m417z
// @github          https://github.com/m417z
// @twitter         https://twitter.com/m417z
// @homepage        https://m417z.com/
// @include         *
// @compilerOptions -lcomctl32 -lole32 -loleaut32
// ==/WindhawkMod==

// Source code is published under The GNU General Public License v3.0.
//
// For bug reports and feature requests, please open an issue here:
// https://github.com/ramensoftware/windhawk-mods/issues
//
// For pull requests, development takes place here:
// https://github.com/m417z/my-windhawk-mods

// ==WindhawkModReadme==
/*
# Resource Redirect

Define alternative files for loading various resources (e.g. icons in
imageres.dll) for simple theming without having to modify system files.

**Note**: This mod requires Windhawk v1.6 or newer.

## Icon themes

A collection of community contributed icon theme packs can be found in the
[Resource Redirect icon
themes](https://github.com/ramensoftware/resource-redirect-icon-themes)
repository. An icon theme can be selected in the mod's settings.

An icon theme can also be installed manually by downloading it and specifying
its path in the mod's settings. For details, refer to the guide in the
repository.

A short demonstration can be found [here on
YouTube](https://youtu.be/irzVmKHB83E).

## Theme paths

Theme paths can be set in the settings. A theme path is a folder with
alternative resource files, and the `theme.ini` file that contains redirection
rules. For example, the `theme.ini` file may contain the following:

```
[redirections]
%SystemRoot%\explorer.exe=explorer.exe
%SystemRoot%\System32\imageres.dll=imageres.dll
```

In this case, the folder must also contain the `explorer.exe`, `imageres.dll`
files which will be used as the redirection resource files.

Alternatively, the theme path can be the `.ini` file itself.

## Supported resource types and loading methods

The mod supports the following resource types and loading methods:

* Icons extracted with the `PrivateExtractIconsW` function.
* Icons, cursors and bitmaps loaded with the `LoadImageW` function.
* Icons loaded with the `LoadIconW` function.
* Cursors loaded with the `LoadCursorW` function.
* Bitmaps loaded with the `LoadBitmapW` function.
* Menus loaded with the `LoadMenuW` function.
* Dialogs loaded with the `DialogBoxParamW`, `CreateDialogParamW` functions.
* Strings loaded with the `LoadStringW` function.
* GDI+ images (e.g. PNGs) loaded with the `SHCreateStreamOnModuleResourceW`
  function.
* DirectUI resources (usually `UIFILE` and `XML`) loaded with the
  `SetXMLFromResource` function.

## Redirect all loaded resources (experimental)

The option to redirect all resources can be enabled in the settings. In this
case, redirection won't be limited to the resource types and loading methods
listed above. This option might become the default in the future.

## Choosing the redirected resource file

For some files, Windows has additional .mui and/or .mun resource files. For
example, when loading a resource from `%SystemRoot%\System32\imageres.dll`,
Windows looks for the resource in these files, in order:

* `%SystemRoot%\System32\en-US\imageres.dll.mui` - The language-specific file.
  `en-US` may be different depending on the Windows language.
* `%SystemRoot%\System32\imageres.dll` - The target file itself.
* `%SystemRoot%\SystemResources\imageres.dll.mun` - The language-neutral file.

For overriding resources, `imageres.dll` must be specified as the redirected
resource file, not `imageres.dll.mui` or `imageres.dll.mun`, regardless of where
the resources to be redirected are actually located.

The resource lookup order then becomes:

* `imageres_redirection.dll` (the redirection resource file specified in this
  mod)
* `imageres.dll.mui`
* `imageres.dll`
* `imageres.dll.mun`
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- iconTheme: ""
  $name: Icon theme
  $description: >-
    The icon theme to use. For details, refer to the mod description.
  $options:
  - "": None
  - All White Icons|themes/icons/niivu/All%20White%20Icons.zip: All White Icons (by niivu)
  - Antu alt|themes/icons/niivu/antu%20alt.zip: Antu alt (by niivu)
  - Antu|themes/icons/niivu/antu.zip: Antu (by niivu)
  - Kuyen Alt|themes/icons/niivu/kuyen%20alt.zip: Kuyen Alt (by niivu)
  - Kuyen|themes/icons/niivu/kuyen.zip: Kuyen (by niivu)
  - ARC Symbolic|themes/icons/niivu/ARC%20Symbolic.zip: ARC Symbolic (by niivu)
  - ARC|themes/icons/niivu/ARC.zip: ARC (by niivu)
  - Arc Neutral Brown|themes/icons/niivu/arc-neutral%20brown.zip: Arc Neutral Brown (by niivu)
  - Arc Neutral Grey|themes/icons/niivu/arc-neutral%20grey.zip: Arc Neutral Grey (by niivu)
  - BananaOneUI|themes/icons/niivu/BANAANA%20OneUI.zip: BananaOneUI (by niivu)
  - Big Sur DarkMode|themes/icons/niivu/Big%20Sur%20DarkMode.zip: Big Sur DarkMode (by niivu)
  - Big Sur LightMode|themes/icons/niivu/Big%20Sur%20LightMode.zip: Big Sur LightMode (by niivu)
  - Blanked DarkMode|themes/icons/niivu/blanked%20dark%20mode.zip: Blanked DarkMode (by niivu)
  - Blanked LightMode|themes/icons/niivu/blanked%20light%20mode.zip: Blanked LightMode (by niivu)
  - Bonny|themes/icons/niivu/bonny%20by%20niivu.zip: Bonny (by niivu)
  - Bouquet|themes/icons/niivu/bouquet.zip: Bouquet (by niivu)
  - Buuf|themes/icons/niivu/buuf.zip: Buuf (by niivu)
  - CakeOS 2.0|themes/icons/niivu/cakeOS%202.0.zip: CakeOS 2.0 (by niivu)
  - CakeOS Blue|themes/icons/niivu/Cake%20OS%20Blue.zip: CakeOS Blue (by niivu)
  - CakeOS Green|themes/icons/niivu/Cake%20OS%20Green.zip: CakeOS Green (by niivu)
  - CakeOS Orange|themes/icons/niivu/Cake%20OS%20Orange.zip: CakeOS Orange (by niivu)
  - CakeOS Purple|themes/icons/niivu/Cake%20OS%20Purple.zip: CakeOS Purple (by niivu)
  - CakeOS Red|themes/icons/niivu/Cake%20OS%20Red.zip: CakeOS Red (by niivu)
  - Candy Original|themes/icons/niivu/candy%20original%20folders.zip: Candy Original (by niivu)
  - Candy Outlined|themes/icons/niivu/candy%20outlined%20folders.zip: Candy Outlined (by niivu)
  - Catppuccin|themes/icons/niivu/Catppuccin.zip: Catppuccin (by niivu)
  - Catppuccin Blue|themes/icons/niivu/Catppuccin%20blue.zip: Catppuccin Blue (by niivu)
  - Catppuccin Flamingo|themes/icons/niivu/Catppuccin%20flamingo.zip: Catppuccin Flamingo (by niivu)
  - Catppuccin Green|themes/icons/niivu/Catppuccin%20green.zip: Catppuccin Green (by niivu)
  - Catppuccin Latte|themes/icons/niivu/Catppuccin%20Latte.zip: Catppuccin Latte (by niivu)
  - Catppuccin Lavender|themes/icons/niivu/Catppuccin%20lavender.zip: Catppuccin Lavender (by niivu)
  - Catppuccin Maroon|themes/icons/niivu/Catppuccin%20maroon.zip: Catppuccin Maroon (by niivu)
  - Catppuccin Mauve|themes/icons/niivu/Catppuccin%20mauve.zip: Catppuccin Mauve (by niivu)
  - Catppuccin Mocha|themes/icons/niivu/Catppuccin%20Mocha.zip: Catppuccin Mocha (by niivu)
  - Catppuccin Peach|themes/icons/niivu/Catppuccin%20peach.zip: Catppuccin Peach (by niivu)
  - Catppuccin Pink|themes/icons/niivu/Catppuccin%20pink.zip: Catppuccin Pink (by niivu)
  - Catppuccin Red|themes/icons/niivu/Catppuccin%20red.zip: Catppuccin Red (by niivu)
  - Catppuccin Sky|themes/icons/niivu/Catppuccin%20sky.zip: Catppuccin Sky (by niivu)
  - Catppuccin Teal|themes/icons/niivu/Catppuccin%20teal.zip: Catppuccin Teal (by niivu)
  - Catppuccin Yellow|themes/icons/niivu/Catppuccin%20yellow.zip: Catppuccin Yellow (by niivu)
  - Deepin Blue DarkMode|themes/icons/niivu/Deepin%20Blue%20-%20for%20dark%20themes.zip: Deepin Blue DarkMode (by niivu)
  - Deepin Blue LightMode|themes/icons/niivu/Deepin%20Blue%20-%20for%20light%20themes.zip: Deepin Blue LightMode (by niivu)
  - Deepin Brown DarkMode|themes/icons/niivu/Deepin%20Brown%20-%20for%20dark%20themes.zip: Deepin Brown DarkMode (by niivu)
  - Deepin Brown LightMode|themes/icons/niivu/Deepin%20Brown%20-%20for%20light%20themes.zip: Deepin Brown LightMode (by niivu)
  - Deepin Green DarkMode|themes/icons/niivu/Deepin%20Green%20-%20for%20dark%20themes.zip: Deepin Green DarkMode (by niivu)
  - Deepin Green LightMode|themes/icons/niivu/Deepin%20Green%20-%20for%20light%20themes.zip: Deepin Green LightMode (by niivu)
  - Deepin Slate DarkMode|themes/icons/niivu/Deepin%20Slate%20-%20for%20dark%20themes.zip: Deepin Slate DarkMode (by niivu)
  - Deepin Slate LightMode|themes/icons/niivu/Deepin%20Slate%20-%20for%20light%20themes.zip: Deepin Slate LightMode (by niivu)
  - Deepo|themes/icons/niivu/Deepo%20Icon%20pack.zip: Deepo (by niivu)
  - Tango|themes/icons/niivu/Tango.zip: Tango (by niivu)
  - Tangerine|themes/icons/niivu/Tangerine.zip: Tangerine (by niivu)
  - Gnome|themes/icons/niivu/Gnome.zip: Gnome (by niivu)
  - Cheser|themes/icons/niivu/Cheser.zip: Cheser (by niivu)
  - Gnome Brave|themes/icons/niivu/Gnome%20Brave.zip: Gnome Brave (by niivu)
  - Gnome Human|themes/icons/niivu/Gnome%20Human.zip: Gnome Human (by niivu)
  - Gnome Noble|themes/icons/niivu/Gnome%20Noble.zip: Gnome Noble (by niivu)
  - Gnome Wine|themes/icons/niivu/Gnome%20Wine.zip: Gnome Wine (by niivu)
  - Gnome Wise|themes/icons/niivu/Gnome%20Wise.zip: Gnome Wise (by niivu)
  - Elementary|themes/icons/niivu/Elementary.zip: Elementary (by niivu)
  - Elementary New|themes/icons/niivu/Elementary%20NEW.zip: Elementary New (by niivu)
  - Humanity|themes/icons/niivu/Humanity.zip: Humanity (by niivu)
  - Everblush|themes/icons/niivu/Everblush.zip: Everblush (by niivu)
  - Everforest|themes/icons/niivu/everforest.zip: Everforest (by niivu)
  - Everforest Blank|themes/icons/niivu/everforest%20blank.zip: Everforest Blank (by niivu)
  - Eyecandy|themes/icons/niivu/Eyecandy.zip: Eyecandy (by niivu)
  - Faba|themes/icons/niivu/FABA.zip: Faba (by niivu)
  - Faba Symbolic|themes/icons/niivu/FABA%20Symbolic.zip: Faba Symbolic (by niivu)
  - Slate|themes/icons/niivu/SLATE.zip: Slate (by niivu)
  - Slate Symbolic|themes/icons/niivu/SLATE%20Symbolic.zip: Slate Symbolic (by niivu)
  - Fetch|themes/icons/niivu/Fetch.zip: Fetch (by niivu)
  - Fluent|themes/icons/niivu/Fluent.zip: Fluent (by niivu)
  - Fluent Keys Night|themes/icons/niivu/Fluent%20Keys%20Night.zip: Fluent Keys Night (by niivu)
  - Fluent Keys Day|themes/icons/niivu/Fluent%20Keys%20Day.zip: Fluent Keys Day (by niivu)
  - Flurry|themes/icons/niivu/FLURRY.zip: Flurry (by niivu)
  - Gruvbox|themes/icons/niivu/Gruvbox.zip: Gruvbox (by niivu)
  - Gruvbox Plus Olive|themes/icons/niivu/gruvbox%20plus%20-%20Olive.zip: Gruvbox Plus Olive (by niivu)
  - Gruvbox Numix|themes/icons/niivu/Gruvbox%20numix.zip: Gruvbox Numix (by niivu)
  - Haiku BeOS|themes/icons/niivu/Haiku%20BeOS.zip: Haiku BeOS (by niivu)
  - Janguru Blue|themes/icons/niivu/janguru%20blue.zip: Janguru Blue (by niivu)
  - Janguru BlueGrey|themes/icons/niivu/janguru%20bluegrey.zip: Janguru BlueGrey (by niivu)
  - Janguru Brown|themes/icons/niivu/janguru%20brown.zip: Janguru Brown (by niivu)
  - Janguru Green|themes/icons/niivu/janguru%20green.zip: Janguru Green (by niivu)
  - Janguru Grey|themes/icons/niivu/janguru%20grey.zip: Janguru Grey (by niivu)
  - Janguru Orange|themes/icons/niivu/janguru%20orange.zip: Janguru Orange (by niivu)
  - koZ|themes/icons/niivu/koZ.zip: koZ (by niivu)
  - Kripton Flatery|themes/icons/niivu/Kripton%20Flatery.zip: Kripton Flatery (by niivu)
  - Linuxfx 11 AIO|themes/icons/niivu/Linuxfx-11-AIO.zip: Linuxfx 11 AIO (by niivu)
  - Linuxfx 11 Lite|themes/icons/niivu/Linuxfx-11-lite.zip: Linuxfx 11 Lite (by niivu)
  - Lol|themes/icons/niivu/lol.zip: Lol (by niivu)
  - Lumicons Folders|themes/icons/niivu/Lumicons%20Folders.zip: Lumicons Folders (by niivu)
  - Lumicons Symbols|themes/icons/niivu/Lumicons%20Symbols.zip: Lumicons Symbols (by niivu)
  - macOSx|themes/icons/niivu/mac%20osx.zip: macOSx (by niivu)
  - macOS Regular|themes/icons/niivu/macOS%20Regular.zip: macOS Regular (by niivu)
  - macOS Blue|themes/icons/niivu/macOS%20blue.zip: macOS Blue (by niivu)
  - macOS Yellow|themes/icons/niivu/macOS%20Yellow%20Folders.zip: macOS Yellow (by niivu)
  - macOS DarkMode|themes/icons/niivu/macOS%20Dark%20Mode.zip: macOS DarkMode (by niivu)
  - macOS LightMode|themes/icons/niivu/macOS%20Light%20Mode.zip: macOS LightMode (by niivu)
  - macPac DarkMode|themes/icons/niivu/macpac%20darkmode.zip: macPac DarkMode (by niivu)
  - macPac LightMode|themes/icons/niivu/macpac%20lightmode.zip: macPac LightMode (by niivu)
  - Mechanical|themes/icons/niivu/mechanical.zip: Mechanical (by niivu)
  - Minium2|themes/icons/niivu/MINIUM2.zip: Minium2 (by niivu)
  - Nord Papirus|themes/icons/niivu/Nord%20Papirus.zip: Nord Papirus (by niivu)
  - Nord Papirus NovaGalactic|themes/icons/niivu/Nord-Papirus-Nova-galactic.zip: Nord Papirus NovaGalactic (by niivu)
  - Numix|themes/icons/niivu/numix.zip: Numix (by niivu)
  - Numix Blue|themes/icons/niivu/numix-remix-blue.zip: Numix Blue (by niivu)
  - Numix Green|themes/icons/niivu/numix-remix-green.zip: Numix Green (by niivu)
  - Numix macOS|themes/icons/niivu/numix-remix-macos.zip: Numix macOS (by niivu)
  - Numix Slate|themes/icons/niivu/numix-remix-slate.zip: Numix Slate (by niivu)
  - Numix Windows|themes/icons/niivu/numix-remix-windows.zip: Numix Windows (by niivu)
  - NUX|themes/icons/niivu/NUX.zip: NUX (by niivu)
  - One Dark Pro|themes/icons/niivu/One%20Dark%20Pro.zip: One Dark Pro (by niivu)
  - One Dark Pro Alt|themes/icons/niivu/One%20Dark%20Pro%20alt.zip: One Dark Pro Alt (by niivu)
  - One UI4|themes/icons/niivu/OneUI4.zip: One UI4 (by niivu)
  - OS X Minimalism|themes/icons/niivu/OS%20X%20Minimalism.zip: OS X Minimalism (by niivu)
  - OS X Minimalism Symbolic|themes/icons/niivu/OS%20X%20Minimalism%20Symbolic.zip: OS X Minimalism Symbolic (by niivu)
  - Paper|themes/icons/niivu/Paper.zip: Paper (by niivu)
  - Papirus Black|themes/icons/niivu/Papirus%20Black.zip: Papirus Black (by niivu)
  - Papirus BlueGrey|themes/icons/niivu/Papirus%20Blue%20Grey.zip: Papirus BlueGrey (by niivu)
  - Papirus Blue|themes/icons/niivu/Papirus%20Blue.zip: Papirus Blue (by niivu)
  - Papirus Brown|themes/icons/niivu/Papirus%20Brown.zip: Papirus Brown (by niivu)
  - Papirus Deep Orange|themes/icons/niivu/Papirus%20Deep%20Orange.zip: Papirus Deep Orange (by niivu)
  - Papirus Dracula|themes/icons/niivu/Papirus%20Dracula.zip: Papirus Dracula (by niivu)
  - Papirus Grey|themes/icons/niivu/Papirus%20Grey.zip: Papirus Grey (by niivu)
  - Papirus Magenta|themes/icons/niivu/Papirus%20Magenta.zip: Papirus Magenta (by niivu)
  - Papirus Pink|themes/icons/niivu/Papirus%20Pink.zip: Papirus Pink (by niivu)
  - Papirus Red|themes/icons/niivu/Papirus%20Red.zip: Papirus Red (by niivu)
  - Papirus Solarized|themes/icons/niivu/Papirus%20Solarized.zip: Papirus Solarized (by niivu)
  - Papirus Teal|themes/icons/niivu/Papirus%20Teal.zip: Papirus Teal (by niivu)
  - Papirus Violet|themes/icons/niivu/Papirus%20Violet.zip: Papirus Violet (by niivu)
  - Tokyo Night|themes/icons/niivu/Tokyo%20Night%20blank.zip: Tokyo Night (by niivu)
  - Tokyo Night Papirus|themes/icons/niivu/Tokyo%20Night%20Papirus.zip: Tokyo Night Papirus (by niivu)
  - Tokyo Night SE Papirus|themes/icons/niivu/Tokyo%20Night%20SE%20Papirus.zip: Tokyo Night SE Papirus (by niivu)
  - Pink Folders|themes/icons/niivu/pink%20folders.zip: Pink Folders (by niivu)
  - Post|themes/icons/niivu/post.zip: Post (by niivu)
  - Pure Dark|themes/icons/niivu/Pure%20for%20dark%20or%20dark%20side%20panel%20themes.zip: Pure Dark (by niivu)
  - Pure Light|themes/icons/niivu/Pure%20for%20light%20themes.zip: Pure Light (by niivu)
  - Quixotic Day|themes/icons/niivu/Quixotic-SE%20Day%20AIO.zip: Quixotic Day (by niivu)
  - Quixotic Dark|themes/icons/niivu/Quixotic-SE%20Dark%20AIO.zip: Quixotic Dark (by niivu)
  - Quixotic Night|themes/icons/niivu/Quixotic-SE%20Night%20AIO.zip: Quixotic Night (by niivu)
  - Rose Pine|themes/icons/niivu/Rose%20Pine.zip: Rose Pine (by niivu)
  - Solarized Day AIO|themes/icons/niivu/solarized%20Day%20AIO.zip: Solarized Day AIO (by niivu)
  - Solarized Night AIO|themes/icons/niivu/solarized%20Night%20AIO.zip: Solarized Night AIO (by niivu)
  - Solus|themes/icons/niivu/Solus.zip: Solus (by niivu)
  - Somatic Rebirth|themes/icons/niivu/Somatic%20Rebirth.zip: Somatic Rebirth (by niivu)
  - Spaceshrooms Blue|themes/icons/niivu/space-shrooms-blue.zip: Spaceshrooms Blue (by niivu)
  - Spaceshrooms Green|themes/icons/niivu/space-shrooms-green.zip: Spaceshrooms Green (by niivu)
  - Spaceshrooms Yellow|themes/icons/niivu/space-shrooms-yellow.zip: Spaceshrooms Yellow (by niivu)
  - Super Remix Blue|themes/icons/niivu/Super%20Remix%20Blue.zip: Super Remix Blue (by niivu)
  - Super Remix Green|themes/icons/niivu/Super%20Remix%20Green.zip: Super Remix Green (by niivu)
  - Super Remix Slate|themes/icons/niivu/Super%20Remix%20Slate.zip: Super Remix Slate (by niivu)
  - Sweet Awesomeness|themes/icons/niivu/Sweet%20Awesomeness.zip: Sweet Awesomeness (by niivu)
  - Sweetness Blue|themes/icons/niivu/Sweetness%20Blue%20folders.zip: Sweetness Blue (by niivu)
  - Sweetness Neutral|themes/icons/niivu/Sweetness%20Neutral.zip: Sweetness Neutral (by niivu)
  - Sweetness Original|themes/icons/niivu/Sweetness%20Original.zip: Sweetness Original (by niivu)
  - Sweetness Pink|themes/icons/niivu/Sweetness%20Pink%20folders.zip: Sweetness Pink (by niivu)
  - Sweetness Purple|themes/icons/niivu/Sweetness%20Purple%20folders.zip: Sweetness Purple (by niivu)
  - Sweet Rainbow|themes/icons/niivu/Sweet-Rainbow.zip: Sweet Rainbow (by niivu)
  - UOS|themes/icons/niivu/Uos%20Icon%20pack.zip: UOS (by niivu)
  - Windows 11 New (default)|themes/icons/niivu/Windows%2011%20New%20%28default%29.zip: Windows 11 New (default) (by niivu)
  - Windows 11 New Folders Blue|themes/icons/niivu/Windows%2011%20New%20Folders%20Blue.zip: Windows 11 New Folders Blue (by niivu)
  - Windows 11 New Folders Green|themes/icons/niivu/Windows%2011%20New%20Folders%20Green.zip: Windows 11 New Folders Green (by niivu)
  - Windows 11 New Folders Purple|themes/icons/niivu/Windows%2011%20New%20Folders%20Purple.zip: Windows 11 New Folders Purple (by niivu)
  - Windows 11 New Folders Slate|themes/icons/niivu/Windows%2011%20New%20Folders%20Slate.zip: Windows 11 New Folders Slate (by niivu)
  - Windows 11 New Folders Yellow|themes/icons/niivu/Windows%2011%20New%20Folders%20Yellow.zip: Windows 11 New Folders Yellow (by niivu)
  - Pane7|themes/icons/ImSwordQueen/Pane7.zip: Pane7 (by ImSwordQueen)
- themePaths: [""]
  $name: Theme paths
  $description: >-
    Each path can be a folder with alternative resource files and the theme.ini
    file, or the .ini theme file itself.
- redirectionResourcePaths:
  - - original: ""
      $name: The redirected resource file
      $description: >-
        The original file from which resources are loaded. Can be a pattern
        where '*' matches any number of characters and '?' matches any single
        character.
    - redirect: ""
      $name: The redirection resource file
      $description: The custom resource file that will be used instead.
  $name: Redirection resource paths
- allResourceRedirect: false
  $name: Redirect all loaded resources (experimental)
  $description: >-
    Try to redirect all loaded resources, not only the supported resources
    that are listed in the description.
- themeFolder: ""
  $name: Theme folder (deprecated)
  $description: >-
    A folder with alternative resource files and theme.ini.

    This option will be removed in the future, please use the new "Theme paths"
    option above.
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <initguid.h>

#include <comutil.h>
#include <psapi.h>
#include <shldisp.h>
#include <shlobj.h>
#include <winrt/base.h>

#include <atomic>
#include <filesystem>
#include <functional>
#include <mutex>
#include <optional>
#include <shared_mutex>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

#ifndef LR_EXACTSIZEONLY
#define LR_EXACTSIZEONLY 0x10000
#endif

struct {
    WindhawkUtils::StringSetting iconTheme;
    bool allResourceRedirect;
} g_settings;

std::shared_mutex g_redirectionResourcePathsMutex;
thread_local bool g_redirectionResourcePathsMutexLocked;
std::unordered_map<std::wstring, std::vector<std::wstring>>
    g_redirectionResourcePaths;
std::unordered_map<std::string, std::vector<std::string>>
    g_redirectionResourcePathsA;
std::vector<std::pair<std::wstring, std::wstring>>
    g_redirectionResourcePathPatterns;
std::vector<std::pair<std::string, std::string>>
    g_redirectionResourcePathPatternsA;

std::shared_mutex g_redirectionResourceModulesMutex;
std::unordered_map<std::wstring, HMODULE> g_redirectionResourceModules;

std::atomic<DWORD> g_operationCounter;

HANDLE g_clearCachePromptThread;
std::atomic<HWND> g_clearCachePromptWindow;

// The resource operation count is used to mark recognized high level resource
// operations, which will then be checked in the lower level hooks:
// FindResourceExA, FindResourceExW, LoadResource, SizeofResource,
// RtlLoadString. This allows handling recognized high level calls in a more
// reliable way.
//
// Example: Consider LoadImageW for loading an icon. Internally, it reads two
// resources: the icon group resource and the actual icon resource. If the
// FindResourceExW hook runs for both resources, it might fall back for the icon
// group, but redirect the actual icon resource, causing a mismatch. By handling
// the redirection in LoadImageW, both operations are either redirected or not.
thread_local int g_resourceOperationCount;

auto resourceOperationCountScope() {
    g_resourceOperationCount++;
    return std::unique_ptr<decltype(g_resourceOperationCount),
                           void (*)(decltype(g_resourceOperationCount)*)>{
        &g_resourceOperationCount,
        [](auto resourceOperationCount) { (*resourceOperationCount)--; }};
}

constexpr WCHAR kClearCachePromptTitle[] = L"Resource Redirect - Windhawk";
constexpr WCHAR kClearCachePromptText[] =
    L"For some icons to be updated, the icon cache must be cleared. Do you "
    L"want to clear the icon cache now?\n\nIcon cache files will be deleted, "
    L"and Explorer will be restarted.";
constexpr WCHAR kClearCacheCommand[] =
    LR"(cmd /c "echo Terminating Explorer...)"
    LR"( & taskkill /f /im explorer.exe)"
    LR"( & timeout /t 1 /nobreak >nul)"
    LR"( & del /f /q /a "%LocalAppData%\IconCache.db")"
    LR"( & del /f /s /q /a "%LocalAppData%\Microsoft\Windows\Explorer\iconcache_*.db")"
    LR"( & del /f /s /q /a "%LocalAppData%\Microsoft\Windows\Explorer\thumbcache_*.db")"
    LR"( & timeout /t 1 /nobreak >nul)"
    LR"( & start explorer.exe)"
    LR"( & echo Starting Explorer...)"
    LR"( & timeout /t 3 /nobreak >nul")";

// https://github.com/tidwall/match.c
//
// match returns true if str matches pattern. This is a very
// simple wildcard match where '*' matches on any number characters
// and '?' matches on any one character.
//
// pattern:
//   { term }
// term:
// 	 '*'         matches any sequence of non-Separator characters
// 	 '?'         matches any single non-Separator character
// 	 c           matches character c (c != '*', '?')
template <typename T>
bool strmatch(const T* pat, size_t plen, const T* str, size_t slen) {
    while (plen > 0) {
        if (pat[0] == '*') {
            if (plen == 1)
                return true;
            if (pat[1] == '*') {
                pat++;
                plen--;
                continue;
            }
            if (strmatch(pat + 1, plen - 1, str, slen))
                return true;
            if (slen == 0)
                return false;
            str++;
            slen--;
            continue;
        }
        if (slen == 0)
            return false;
        if (pat[0] != '?' && str[0] != pat[0])
            return false;
        pat++;
        plen--;
        str++;
        slen--;
    }
    return slen == 0 && plen == 0;
}

// chooseAW<char> returns OptionA.
// chooseAW<WCHAR> returns OptionW.
template <typename T, auto OptionA, auto OptionW>
auto chooseAW() {
    if constexpr (std::is_same_v<T, char>) {
        return OptionA;
    } else {
        static_assert(std::is_same_v<T, WCHAR>);
        return OptionW;
    }
}

auto StrToW(PCWSTR str) {
    struct {
        PCWSTR p;
    } result;
    result.p = str;
    return result;
}

// https://stackoverflow.com/a/69410299
auto StrToW(PCSTR str) {
    struct {
        std::wstring wstr;
        PCWSTR p;
    } result;

    if (*str) {
        int strLen = static_cast<int>(strlen(str));
        int sizeNeeded =
            MultiByteToWideChar(CP_ACP, 0, str, strLen, nullptr, 0);
        if (sizeNeeded) {
            result.wstr.resize(sizeNeeded);
            MultiByteToWideChar(CP_ACP, 0, str, strLen, result.wstr.data(),
                                sizeNeeded);
        }
    }

    result.p = result.wstr.c_str();
    return result;
}

// A helper function to skip locking if the thread already holds the lock, since
// it's UB. Nested locks may happen if one hooked function is implemented with
// the help of another hooked function. We assume here that the locks are freed
// in reverse order.
auto RedirectionResourcePathsMutexSharedLock() {
    class Result {
       public:
        Result() = default;

        Result(std::shared_mutex& mutex) : lock(std::shared_lock{mutex}) {
            g_redirectionResourcePathsMutexLocked = true;
        }

        ~Result() {
            if (lock) {
                g_redirectionResourcePathsMutexLocked = false;
            }
        }

       private:
        std::optional<std::shared_lock<std::shared_mutex>> lock;
    };

    if (g_redirectionResourcePathsMutexLocked) {
        return Result{};
    }

    return Result{g_redirectionResourcePathsMutex};
}

bool DevicePathToDosPath(const WCHAR* device_path,
                         WCHAR* dos_path,
                         size_t dos_path_size) {
    WCHAR drive_strings[MAX_PATH];
    if (!GetLogicalDriveStrings(ARRAYSIZE(drive_strings), drive_strings)) {
        return false;
    }

    // Drive strings are stored as a set of null terminated strings, with an
    // extra null after the last string. Each drive string is of the form "C:\".
    // We convert it to the form "C:", which is the format expected by
    // QueryDosDevice().
    WCHAR drive_colon[3] = L" :";
    for (const WCHAR* next_drive_letter = drive_strings; *next_drive_letter;
         next_drive_letter += wcslen(next_drive_letter) + 1) {
        // Dos device of the form "C:".
        *drive_colon = *next_drive_letter;
        WCHAR device_name[MAX_PATH];
        if (!QueryDosDevice(drive_colon, device_name, ARRAYSIZE(device_name))) {
            continue;
        }

        size_t name_length = wcslen(device_name);
        if (_wcsnicmp(device_path, device_name, name_length) == 0) {
            size_t dos_path_size_required =
                2 + wcslen(device_path + name_length) + 1;
            if (dos_path_size < dos_path_size_required) {
                return false;
            }

            // Construct DOS path.
            wcscpy(dos_path, drive_colon);
            wcscat(dos_path, device_path + name_length);
            return true;
        }
    }

    return false;
}

HMODULE GetRedirectedModule(std::wstring_view fileName) {
    std::wstring fileNameUpper{fileName};
    LCMapStringEx(LOCALE_NAME_USER_DEFAULT, LCMAP_UPPERCASE, &fileNameUpper[0],
                  static_cast<int>(fileNameUpper.length()), &fileNameUpper[0],
                  static_cast<int>(fileNameUpper.length()), nullptr, nullptr,
                  0);

    {
        std::shared_lock lock{g_redirectionResourceModulesMutex};
        const auto it = g_redirectionResourceModules.find(fileNameUpper);
        if (it != g_redirectionResourceModules.end()) {
            return it->second;
        }
    }

    HINSTANCE module = LoadLibraryEx(
        fileNameUpper.c_str(), nullptr,
        LOAD_LIBRARY_AS_DATAFILE | LOAD_LIBRARY_AS_IMAGE_RESOURCE);
    if (!module) {
        DWORD dwError = GetLastError();
        Wh_Log(L"LoadLibraryEx failed with error %u", dwError);
        return nullptr;
    }

    {
        std::unique_lock lock{g_redirectionResourceModulesMutex};
        g_redirectionResourceModules.try_emplace(fileNameUpper, module);
    }

    return module;
}

void FreeAndClearRedirectedModules() {
    std::unordered_map<std::wstring, HMODULE> modules;

    {
        std::unique_lock lock{g_redirectionResourceModulesMutex};
        modules.swap(g_redirectionResourceModules);
    }

    for (const auto& it : modules) {
        FreeLibrary(it.second);
    }
}

template <typename T>
bool RedirectFileName(DWORD c,
                      const T* fileName,
                      std::function<void()> beforeFirstRedirectionFunction,
                      std::function<bool(const T*)> redirectFunction) {
    if (!fileName) {
        Wh_Log(L"[%u] Error, nullptr file name, falling back to original", c);
        return false;
    }

    std::basic_string<T> fileNameUpper{fileName};
    (chooseAW<T, LCMapStringA, LCMapStringW>())(
        LOCALE_USER_DEFAULT, LCMAP_UPPERCASE, &fileNameUpper[0],
        static_cast<int>(fileNameUpper.length()), &fileNameUpper[0],
        static_cast<int>(fileNameUpper.length()));

    bool triedRedirection = false;

    {
        auto lock{RedirectionResourcePathsMutexSharedLock()};

        const auto& redirectionResourcePaths =
            *(chooseAW<T, &g_redirectionResourcePathsA,
                       &g_redirectionResourcePaths>());
        if (const auto it = redirectionResourcePaths.find(fileNameUpper);
            it != redirectionResourcePaths.end()) {
            const auto& redirects = it->second;
            for (const auto& redirect : redirects) {
                if (!triedRedirection) {
                    beforeFirstRedirectionFunction();
                    triedRedirection = true;
                }

                Wh_Log(L"[%u] Trying %s", c, StrToW(redirect.c_str()).p);

                if (redirectFunction(redirect.c_str())) {
                    return true;
                }
            }
        }

        const auto& redirectionResourcePathPatterns =
            *(chooseAW<T, &g_redirectionResourcePathPatternsA,
                       &g_redirectionResourcePathPatterns>());
        for (const auto& [pattern, redirect] :
             redirectionResourcePathPatterns) {
            if (!strmatch(pattern.data(), pattern.size(), fileNameUpper.data(),
                          fileNameUpper.size())) {
                continue;
            }

            if (!triedRedirection) {
                beforeFirstRedirectionFunction();
                triedRedirection = true;
            }

            Wh_Log(L"[%u] Trying %s", c, StrToW(redirect.c_str()).p);

            if (redirectFunction(redirect.c_str())) {
                return true;
            }
        }
    }

    if (triedRedirection) {
        Wh_Log(L"[%u] No redirection succeeded, falling back to original", c);
    }

    return false;
}

bool RedirectModule(DWORD c,
                    HINSTANCE hInstance,
                    std::function<void()> beforeFirstRedirectionFunction,
                    std::function<bool(HINSTANCE)> redirectFunction) {
    WCHAR szFileName[MAX_PATH];
    DWORD fileNameLen;
    if ((ULONG_PTR)hInstance & 3) {
        WCHAR szNtFileName[MAX_PATH * 2];

        if (!GetMappedFileName(GetCurrentProcess(), (void*)hInstance,
                               szNtFileName, ARRAYSIZE(szNtFileName))) {
            DWORD dwError = GetLastError();
            Wh_Log(L"[%u] GetMappedFileName(%p) failed with error %u", c,
                   hInstance, dwError);
            return false;
        }

        if (!DevicePathToDosPath(szNtFileName, szFileName,
                                 ARRAYSIZE(szFileName))) {
            Wh_Log(L"[%u] DevicePathToDosPath failed", c);
            return false;
        }

        fileNameLen = wcslen(szFileName);
    } else {
        fileNameLen =
            GetModuleFileName(hInstance, szFileName, ARRAYSIZE(szFileName));
        switch (fileNameLen) {
            case 0: {
                DWORD dwError = GetLastError();
                Wh_Log(L"[%u] GetModuleFileName(%p) failed with error %u", c,
                       hInstance, dwError);
                return false;
            }

            case ARRAYSIZE(szFileName):
                Wh_Log(L"[%u] GetModuleFileName(%p) failed, name too long", c,
                       hInstance);
                return false;
        }
    }

    Wh_Log(L"[%u] Module: %s", c, szFileName);

    LCMapStringEx(LOCALE_NAME_USER_DEFAULT, LCMAP_UPPERCASE, &szFileName[0],
                  fileNameLen, &szFileName[0], fileNameLen, nullptr, nullptr,
                  0);

    bool triedRedirection = false;

    {
        auto lock{RedirectionResourcePathsMutexSharedLock()};

        if (const auto it = g_redirectionResourcePaths.find(szFileName);
            it != g_redirectionResourcePaths.end()) {
            const auto& redirects = it->second;
            for (const auto& redirect : redirects) {
                if (!triedRedirection) {
                    beforeFirstRedirectionFunction();
                    triedRedirection = true;
                }

                Wh_Log(L"[%u] Trying %s", c, redirect.c_str());

                HINSTANCE hInstanceRedirect = GetRedirectedModule(redirect);
                if (!hInstanceRedirect) {
                    Wh_Log(L"[%u] GetRedirectedModule failed", c);
                    continue;
                }

                if (redirectFunction(hInstanceRedirect)) {
                    return true;
                }
            }
        }

        for (const auto& [pattern, redirect] :
             g_redirectionResourcePathPatterns) {
            if (!strmatch(pattern.data(), pattern.size(), szFileName,
                          fileNameLen)) {
                continue;
            }

            if (!triedRedirection) {
                beforeFirstRedirectionFunction();
                triedRedirection = true;
            }

            Wh_Log(L"[%u] Trying %s", c, redirect.c_str());

            HINSTANCE hInstanceRedirect = GetRedirectedModule(redirect);
            if (!hInstanceRedirect) {
                Wh_Log(L"[%u] GetRedirectedModule failed", c);
                continue;
            }

            if (redirectFunction(hInstanceRedirect)) {
                return true;
            }
        }
    }

    if (triedRedirection) {
        Wh_Log(L"[%u] No redirection succeeded, falling back to original", c);
    }

    return false;
}

typedef struct {
    int targetIndex;
    int currentIndex;
    LPWSTR foundName;
} ENUMICONCTX;

BOOL CALLBACK EnumIconsProc(HMODULE hModule,
                            LPCWSTR lpszType,
                            LPWSTR lpszName,
                            LONG_PTR lParam) {
    ENUMICONCTX* ctx = (ENUMICONCTX*)lParam;

    if (ctx->currentIndex == ctx->targetIndex) {
        ctx->foundName = lpszName;
        return FALSE;  // Stop enumeration.
    }

    ctx->currentIndex++;
    return TRUE;  // Continue.
}

LPWSTR GetIconGroupNameByIndex(HMODULE hModule, int index) {
    ENUMICONCTX ctx = {
        .targetIndex = index,
        .currentIndex = 0,
        .foundName = nullptr,
    };
    EnumResourceNames(hModule, RT_GROUP_ICON, EnumIconsProc, (LONG_PTR)&ctx);
    return ctx.foundName;
}

typedef struct {
    LPCWSTR targetName;
    int currentIndex;
    int foundIndex;
} ENUMICONBYNAMECTX;

BOOL CALLBACK EnumIconsByNameProc(HMODULE hModule,
                                  LPCWSTR lpszType,
                                  LPWSTR lpszName,
                                  LONG_PTR lParam) {
    ENUMICONBYNAMECTX* ctx = (ENUMICONBYNAMECTX*)lParam;

    // Only compare string names, skip integer resource IDs.
    if (!IS_INTRESOURCE(lpszName)) {
        if (wcscmp(lpszName, ctx->targetName) == 0) {
            ctx->foundIndex = ctx->currentIndex;
            return FALSE;  // Stop enumeration.
        }
    }

    ctx->currentIndex++;
    return TRUE;  // Continue.
}

int GetIconIndexByGroupName(HMODULE hModule, LPCWSTR name) {
    ENUMICONBYNAMECTX ctx = {
        .targetName = name,
        .currentIndex = 0,
        .foundIndex = -1,
    };
    EnumResourceNames(hModule, RT_GROUP_ICON, EnumIconsByNameProc,
                      (LONG_PTR)&ctx);
    return ctx.foundIndex;
}

using PrivateExtractIconsW_t = decltype(&PrivateExtractIconsW);
PrivateExtractIconsW_t PrivateExtractIconsW_Original;
UINT WINAPI PrivateExtractIconsW_Hook(LPCWSTR szFileName,
                                      int nIconIndex,
                                      int cxIcon,
                                      int cyIcon,
                                      HICON* phicon,
                                      UINT* piconid,
                                      UINT nIcons,
                                      UINT flags) {
    auto resOp = resourceOperationCountScope();
    DWORD c = ++g_operationCounter;

    Wh_Log(L"[%u] > Icon index: %d, file name: %s", c, nIconIndex, szFileName);

    UINT result;

    bool redirected = RedirectFileName<WCHAR>(
        c, szFileName,
        [&]() {
            Wh_Log(L"[%u] cxIcon: %d, %d", c, LOWORD(cxIcon), HIWORD(cxIcon));
            Wh_Log(L"[%u] cyIcon: %d, %d", c, LOWORD(cyIcon), HIWORD(cyIcon));
            Wh_Log(L"[%u] phicon: %s", c, phicon ? L"out_ptr" : L"nullptr");
            Wh_Log(L"[%u] piconid: %s", c, piconid ? L"out_ptr" : L"nullptr");
            Wh_Log(L"[%u] nIcons: %u", c, nIcons);
            Wh_Log(L"[%u] flags: 0x%08X", c, flags);
        },
        [&](PCWSTR fileNameRedirect) {
            // nIconIndex can be either:
            // * Negative: the icon id
            // * Non-negative: The icon index (0 for first icon, etc.)
            //
            // Using the non-negative value for the redirected file is
            // problematic as it might contain only part of the icons.
            // Therefore, convert it to an id before proceeding.
            int iconId = nIconIndex;
            if (iconId >= 0) {
                HMODULE module = LoadLibraryEx(szFileName, nullptr,
                                               LOAD_LIBRARY_AS_DATAFILE);
                if (module) {
                    LPWSTR iconGroupName =
                        GetIconGroupNameByIndex(module, iconId);

                    // Best effort: use the icon id if available, otherwise
                    // continue using the index.
                    if (!iconGroupName) {
                        Wh_Log(L"[%u] Failed to get icon group name", c);
                    } else if (!IS_INTRESOURCE(iconGroupName)) {
                        Wh_Log(L"[%u] Icon group name is not an id: %s", c,
                               iconGroupName);

                        // Load redirect file and find the string name's index.
                        HMODULE redirectModule =
                            LoadLibraryEx(fileNameRedirect, nullptr,
                                          LOAD_LIBRARY_AS_DATAFILE);
                        if (redirectModule) {
                            int redirectIndex = GetIconIndexByGroupName(
                                redirectModule, iconGroupName);
                            FreeLibrary(redirectModule);

                            if (redirectIndex >= 0) {
                                iconId = redirectIndex;
                                Wh_Log(
                                    L"[%u] Found icon group name in redirect "
                                    L"file at index %d",
                                    c, redirectIndex);
                            } else {
                                Wh_Log(
                                    L"[%u] Icon group name not found in "
                                    L"redirect file, using original index",
                                    c);
                            }
                        } else {
                            Wh_Log(
                                L"[%u] Failed to load redirect file for icon "
                                L"name lookup",
                                c);
                        }
                    } else {
                        iconId = -(WORD)(ULONG_PTR)iconGroupName;
                        Wh_Log(L"[%u] Using icon group id %d", c, -iconId);
                    }

                    FreeLibrary(module);
                } else {
                    // May happen e.g. for .ico files.
                    Wh_Log(L"[%u] Failed to get module handle", c);
                }
            }

            if (phicon) {
                std::fill_n(phicon, nIcons, nullptr);
            }

            result = PrivateExtractIconsW_Original(fileNameRedirect, iconId,
                                                   cxIcon, cyIcon, phicon,
                                                   piconid, nIcons, flags);
            if (result != 0xFFFFFFFF && result != 0) {
                // In case multiple icons are requested and the custom resource
                // only overrides some of them, we'd ideally like to return a
                // combined result. Unfortunately, that's not trivial to
                // implement, so return the original icons in this case.
                //
                // An example where multiple icons are requested is the Change
                // Icon dialog in shortcut file properties. If a partial result
                // is returned, only the returned icons are displayed.
                bool multipleIcons = nIcons > (HIWORD(cxIcon) ? 2 : 1);
                bool partialResult = result < nIcons;
                UINT multipleIconsOriginalCount = 0;
                if (multipleIcons && partialResult) {
                    multipleIconsOriginalCount = PrivateExtractIconsW_Original(
                        szFileName, nIconIndex, cxIcon, cyIcon, nullptr,
                        nullptr, nIcons, flags);
                }

                if (result < multipleIconsOriginalCount) {
                    Wh_Log(
                        L"[%u] Got less icons than the original file has: %u "
                        L"vs. %u, replacing redirection with the original "
                        L"icons",
                        c, result, multipleIconsOriginalCount);

                    if (phicon) {
                        for (UINT i = 0; i < nIcons; i++) {
                            if (phicon[i]) {
                                DestroyIcon(phicon[i]);
                                phicon[i] = nullptr;
                            }
                        }
                    }

                    result = PrivateExtractIconsW_Original(
                        szFileName, nIconIndex, cxIcon, cyIcon, phicon, piconid,
                        nIcons, flags);
                }

                Wh_Log(L"[%u] Redirected successfully, result: %u", c, result);
                return true;
            }

            // If `LR_EXACTSIZEONLY` is used and the exact size is missing, the
            // function will return no icons. If the replacement file doesn't
            // have this icon, we want to fall back to the original file, but if
            // it has other sizes, we prefer to return an empty result,
            // hopefully the target app will try other sizes in this case.
            if (result == 0 && phicon && (flags & LR_EXACTSIZEONLY)) {
                HICON testIcon = nullptr;
                UINT testIconId;
                UINT testResult = PrivateExtractIconsW_Original(
                    fileNameRedirect, iconId, LOWORD(cxIcon), LOWORD(cyIcon),
                    &testIcon, &testIconId, 1, flags & ~LR_EXACTSIZEONLY);

                if (testIcon) {
                    DestroyIcon(testIcon);
                    testIcon = nullptr;
                }

                if (testResult > 0) {
                    Wh_Log(
                        L"[%u] Redirected successfully with an empty result: "
                        L"%u",
                        c, result);
                    return true;
                }
            }

            // If there is no exact match the API can return 0 but phicon[0] set
            // to a valid hicon. In that case destroy the icon and reset the
            // entry.
            // https://github.com/microsoft/terminal/blob/6d0342f0bb31bf245843411c6781d6d5399ff651/src/interactivity/win32/icon.cpp#L178
            if (phicon) {
                for (UINT i = 0; i < nIcons; i++) {
                    if (phicon[i]) {
                        DestroyIcon(phicon[i]);
                        phicon[i] = nullptr;
                    }
                }
            }

            Wh_Log(L"[%u] Redirection failed, result: %u", c, result);
            return false;
        });
    if (redirected) {
        return result;
    }

    return PrivateExtractIconsW_Original(szFileName, nIconIndex, cxIcon, cyIcon,
                                         phicon, piconid, nIcons, flags);
}

template <auto* Original, typename T>
HANDLE LoadImageAW_Hook(HINSTANCE hInst,
                        const T* name,
                        UINT type,
                        int cx,
                        int cy,
                        UINT fuLoad) {
    auto resOp = resourceOperationCountScope();
    DWORD c = ++g_operationCounter;

    PCWSTR typeClarification = L"";
    switch (type) {
        case IMAGE_BITMAP:
            typeClarification = L" (bitmap)";
            break;
        case IMAGE_ICON:
            typeClarification = L" (icon)";
            break;
        case IMAGE_CURSOR:
            typeClarification = L" (cursor)";
            break;
    }

    WCHAR prefix[64];
    swprintf_s(prefix, L"[%u] > %c, type: %u%s", c, chooseAW<T, L'A', L'W'>(),
               type, typeClarification);

    if (!hInst) {
        if (fuLoad & LR_LOADFROMFILE) {
            Wh_Log(L"%s, file name: %s", prefix, StrToW(name).p);
        } else {
            Wh_Log(L"%s, resource identifier: %zu", prefix, (ULONG_PTR)name);
        }
    } else if (IS_INTRESOURCE(name)) {
        Wh_Log(L"%s, resource number: %u", prefix, (DWORD)(ULONG_PTR)name);
    } else {
        Wh_Log(L"%s, resource name: %s", prefix, StrToW(name).p);
    }

    HANDLE result;
    bool redirected;

    auto beforeFirstRedirectionFunction = [&]() {
        Wh_Log(L"[%u] Width: %d", c, cx);
        Wh_Log(L"[%u] Height: %d", c, cy);
        Wh_Log(L"[%u] Flags: 0x%08X", c, fuLoad);
    };

    if (!hInst && (fuLoad & LR_LOADFROMFILE)) {
        redirected = RedirectFileName<T>(
            c, name, std::move(beforeFirstRedirectionFunction),
            [&](const T* fileNameRedirect) {
                result =
                    (*Original)(hInst, fileNameRedirect, type, cx, cy, fuLoad);
                if (result) {
                    Wh_Log(L"[%u] Redirected successfully", c);
                    return true;
                }

                DWORD dwError = GetLastError();
                Wh_Log(L"[%u] LoadImage failed with error %u", c, dwError);
                return false;
            });
    } else {
        redirected = RedirectModule(
            c, hInst, std::move(beforeFirstRedirectionFunction),
            [&](HINSTANCE hInstanceRedirect) {
                result =
                    (*Original)(hInstanceRedirect, name, type, cx, cy, fuLoad);
                if (result) {
                    Wh_Log(L"[%u] Redirected successfully", c);
                    return true;
                }

                DWORD dwError = GetLastError();
                Wh_Log(L"[%u] LoadImage failed with error %u", c, dwError);
                return false;
            });
    }

    if (redirected) {
        return result;
    }

    return (*Original)(hInst, name, type, cx, cy, fuLoad);
}

using LoadImageA_t = decltype(&LoadImageA);
LoadImageA_t LoadImageA_Original;
HANDLE WINAPI LoadImageA_Hook(HINSTANCE hInst,
                              LPCSTR name,
                              UINT type,
                              int cx,
                              int cy,
                              UINT fuLoad) {
    return LoadImageAW_Hook<&LoadImageA_Original>(hInst, name, type, cx, cy,
                                                  fuLoad);
}

using LoadImageW_t = decltype(&LoadImageW);
LoadImageW_t LoadImageW_Original;
HANDLE WINAPI LoadImageW_Hook(HINSTANCE hInst,
                              LPCWSTR name,
                              UINT type,
                              int cx,
                              int cy,
                              UINT fuLoad) {
    return LoadImageAW_Hook<&LoadImageW_Original>(hInst, name, type, cx, cy,
                                                  fuLoad);
}

template <auto* Original, typename T>
HICON LoadIconAW_Hook(HINSTANCE hInstance, const T* lpIconName) {
    auto resOp = resourceOperationCountScope();
    DWORD c = ++g_operationCounter;

    WCHAR prefix[64];
    swprintf_s(prefix, L"[%u] > %c", c, chooseAW<T, L'A', L'W'>());

    if (!hInstance) {
        Wh_Log(L"%s, resource identifier: %zu", prefix, (ULONG_PTR)lpIconName);
    } else if (IS_INTRESOURCE(lpIconName)) {
        Wh_Log(L"%s, resource number: %u", prefix,
               (DWORD)(ULONG_PTR)lpIconName);
    } else {
        Wh_Log(L"%s, resource name: %s", prefix, StrToW(lpIconName).p);
    }

    HICON result;

    bool redirected = RedirectModule(
        c, hInstance, []() {},
        [&](HINSTANCE hInstanceRedirect) {
            result = (*Original)(hInstanceRedirect, lpIconName);
            if (result) {
                Wh_Log(L"[%u] Redirected successfully", c);
                return true;
            }

            DWORD dwError = GetLastError();
            Wh_Log(L"[%u] LoadIcon failed with error %u", c, dwError);
            return false;
        });
    if (redirected) {
        return result;
    }

    return (*Original)(hInstance, lpIconName);
}

using LoadIconA_t = decltype(&LoadIconA);
LoadIconA_t LoadIconA_Original;
HICON WINAPI LoadIconA_Hook(HINSTANCE hInstance, LPCSTR lpIconName) {
    return LoadIconAW_Hook<&LoadIconA_Original>(hInstance, lpIconName);
}

using LoadIconW_t = decltype(&LoadIconW);
LoadIconW_t LoadIconW_Original;
HICON WINAPI LoadIconW_Hook(HINSTANCE hInstance, LPCWSTR lpIconName) {
    return LoadIconAW_Hook<&LoadIconW_Original>(hInstance, lpIconName);
}

template <auto* Original, typename T>
HCURSOR LoadCursorAW_Hook(HINSTANCE hInstance, const T* lpCursorName) {
    auto resOp = resourceOperationCountScope();
    DWORD c = ++g_operationCounter;

    WCHAR prefix[64];
    swprintf_s(prefix, L"[%u] > %c", c, chooseAW<T, L'A', L'W'>());

    if (!hInstance) {
        Wh_Log(L"%s, resource identifier: %zu", prefix,
               (ULONG_PTR)lpCursorName);
    } else if (IS_INTRESOURCE(lpCursorName)) {
        Wh_Log(L"%s, resource number: %u", prefix,
               (DWORD)(ULONG_PTR)lpCursorName);
    } else {
        Wh_Log(L"%s, resource name: %s", prefix, StrToW(lpCursorName).p);
    }

    HCURSOR result;

    bool redirected = RedirectModule(
        c, hInstance, []() {},
        [&](HINSTANCE hInstanceRedirect) {
            result = (*Original)(hInstanceRedirect, lpCursorName);
            if (result) {
                Wh_Log(L"[%u] Redirected successfully", c);
                return true;
            }

            DWORD dwError = GetLastError();
            Wh_Log(L"[%u] LoadCursor failed with error %u", c, dwError);
            return false;
        });
    if (redirected) {
        return result;
    }

    return (*Original)(hInstance, lpCursorName);
}

using LoadCursorA_t = decltype(&LoadCursorA);
LoadCursorA_t LoadCursorA_Original;
HCURSOR WINAPI LoadCursorA_Hook(HINSTANCE hInstance, LPCSTR lpCursorName) {
    return LoadCursorAW_Hook<&LoadCursorA_Original>(hInstance, lpCursorName);
}

using LoadCursorW_t = decltype(&LoadCursorW);
LoadCursorW_t LoadCursorW_Original;
HCURSOR WINAPI LoadCursorW_Hook(HINSTANCE hInstance, LPCWSTR lpCursorName) {
    return LoadCursorAW_Hook<&LoadCursorW_Original>(hInstance, lpCursorName);
}

template <auto* Original, typename T>
HBITMAP LoadBitmapAW_Hook(HINSTANCE hInstance, const T* lpBitmapName) {
    auto resOp = resourceOperationCountScope();
    DWORD c = ++g_operationCounter;

    WCHAR prefix[64];
    swprintf_s(prefix, L"[%u] > %c", c, chooseAW<T, L'A', L'W'>());

    if (!hInstance) {
        Wh_Log(L"%s, resource identifier: %zu", prefix,
               (ULONG_PTR)lpBitmapName);
    } else if (IS_INTRESOURCE(lpBitmapName)) {
        Wh_Log(L"%s, resource number: %u", prefix,
               (DWORD)(ULONG_PTR)lpBitmapName);
    } else {
        Wh_Log(L"%s, resource name: %s", prefix, StrToW(lpBitmapName).p);
    }

    HBITMAP result;

    bool redirected = RedirectModule(
        c, hInstance, []() {},
        [&](HINSTANCE hInstanceRedirect) {
            result = (*Original)(hInstanceRedirect, lpBitmapName);
            if (result) {
                Wh_Log(L"[%u] Redirected successfully", c);
                return true;
            }

            DWORD dwError = GetLastError();
            Wh_Log(L"[%u] LoadBitmap failed with error %u", c, dwError);
            return false;
        });
    if (redirected) {
        return result;
    }

    return (*Original)(hInstance, lpBitmapName);
}

using LoadBitmapA_t = decltype(&LoadBitmapA);
LoadBitmapA_t LoadBitmapA_Original;
HBITMAP WINAPI LoadBitmapA_Hook(HINSTANCE hInstance, LPCSTR lpBitmapName) {
    return LoadBitmapAW_Hook<&LoadBitmapA_Original>(hInstance, lpBitmapName);
}

using LoadBitmapW_t = decltype(&LoadBitmapW);
LoadBitmapW_t LoadBitmapW_Original;
HBITMAP WINAPI LoadBitmapW_Hook(HINSTANCE hInstance, LPCWSTR lpBitmapName) {
    return LoadBitmapAW_Hook<&LoadBitmapW_Original>(hInstance, lpBitmapName);
}

template <auto* Original, typename T>
HMENU LoadMenuAW_Hook(HINSTANCE hInstance, const T* lpMenuName) {
    auto resOp = resourceOperationCountScope();
    DWORD c = ++g_operationCounter;

    WCHAR prefix[64];
    swprintf_s(prefix, L"[%u] > %c", c, chooseAW<T, L'A', L'W'>());

    if (IS_INTRESOURCE(lpMenuName)) {
        Wh_Log(L"%s, resource number: %u", prefix,
               (DWORD)(ULONG_PTR)lpMenuName);
    } else {
        Wh_Log(L"%s, resource name: %s", prefix, StrToW(lpMenuName).p);
    }

    HMENU result;

    bool redirected = RedirectModule(
        c, hInstance, []() {},
        [&](HINSTANCE hInstanceRedirect) {
            result = (*Original)(hInstanceRedirect, lpMenuName);
            if (result) {
                Wh_Log(L"[%u] Redirected successfully", c);
                return true;
            }

            DWORD dwError = GetLastError();
            Wh_Log(L"[%u] LoadMenu failed with error %u", c, dwError);
            return false;
        });
    if (redirected) {
        return result;
    }

    return (*Original)(hInstance, lpMenuName);
}

using LoadMenuA_t = decltype(&LoadMenuA);
LoadMenuA_t LoadMenuA_Original;
HMENU WINAPI LoadMenuA_Hook(HINSTANCE hInstance, LPCSTR lpMenuName) {
    return LoadMenuAW_Hook<&LoadMenuA_Original>(hInstance, lpMenuName);
}

using LoadMenuW_t = decltype(&LoadMenuW);
LoadMenuW_t LoadMenuW_Original;
HMENU WINAPI LoadMenuW_Hook(HINSTANCE hInstance, LPCWSTR lpMenuName) {
    return LoadMenuAW_Hook<&LoadMenuW_Original>(hInstance, lpMenuName);
}

template <auto* Original, typename T>
INT_PTR DialogBoxParamAW_Hook(HINSTANCE hInstance,
                              const T* lpTemplateName,
                              HWND hWndParent,
                              DLGPROC lpDialogFunc,
                              LPARAM dwInitParam) {
    auto resOp = resourceOperationCountScope();
    DWORD c = ++g_operationCounter;

    WCHAR prefix[64];
    swprintf_s(prefix, L"[%u] > %c", c, chooseAW<T, L'A', L'W'>());

    if (IS_INTRESOURCE(lpTemplateName)) {
        Wh_Log(L"%s, resource number: %u", prefix,
               (DWORD)(ULONG_PTR)lpTemplateName);
    } else {
        Wh_Log(L"%s, resource name: %s", prefix, StrToW(lpTemplateName).p);
    }

    INT_PTR result;

    bool redirected = RedirectModule(
        c, hInstance, []() {},
        [&](HINSTANCE hInstanceRedirect) {
            // For other redirected functions, we check whether the function
            // succeeded. If it didn't, we try another redirection or fall back
            // to the original file.
            //
            // In this case, there's no reliable way to find out whether
            // DialogBoxParamW failed, since any value can be returned.
            // Therefore, only make sure that the dialog resource exists.
            if (!(chooseAW<T, FindResourceExA, FindResourceExW>())(
                    hInstanceRedirect, (T*)RT_DIALOG, lpTemplateName, 0)) {
                Wh_Log(L"[%u] Resource not found", c);
                return false;
            }

            Wh_Log(L"[%u] Redirected successfully", c);
            result = (*Original)(hInstanceRedirect, lpTemplateName, hWndParent,
                                 lpDialogFunc, dwInitParam);
            return true;
        });
    if (redirected) {
        return result;
    }

    return (*Original)(hInstance, lpTemplateName, hWndParent, lpDialogFunc,
                       dwInitParam);
}

using DialogBoxParamA_t = decltype(&DialogBoxParamA);
DialogBoxParamA_t DialogBoxParamA_Original;
INT_PTR WINAPI DialogBoxParamA_Hook(HINSTANCE hInstance,
                                    LPCSTR lpTemplateName,
                                    HWND hWndParent,
                                    DLGPROC lpDialogFunc,
                                    LPARAM dwInitParam) {
    return DialogBoxParamAW_Hook<&DialogBoxParamA_Original>(
        hInstance, lpTemplateName, hWndParent, lpDialogFunc, dwInitParam);
}

using DialogBoxParamW_t = decltype(&DialogBoxParamW);
DialogBoxParamW_t DialogBoxParamW_Original;
INT_PTR WINAPI DialogBoxParamW_Hook(HINSTANCE hInstance,
                                    LPCWSTR lpTemplateName,
                                    HWND hWndParent,
                                    DLGPROC lpDialogFunc,
                                    LPARAM dwInitParam) {
    return DialogBoxParamAW_Hook<&DialogBoxParamW_Original>(
        hInstance, lpTemplateName, hWndParent, lpDialogFunc, dwInitParam);
}

template <auto* Original, typename T>
HWND CreateDialogParamAW_Hook(HINSTANCE hInstance,
                              const T* lpTemplateName,
                              HWND hWndParent,
                              DLGPROC lpDialogFunc,
                              LPARAM dwInitParam) {
    auto resOp = resourceOperationCountScope();
    DWORD c = ++g_operationCounter;

    WCHAR prefix[64];
    swprintf_s(prefix, L"[%u] > %c", c, chooseAW<T, L'A', L'W'>());

    if (IS_INTRESOURCE(lpTemplateName)) {
        Wh_Log(L"%s, resource number: %u", prefix,
               (DWORD)(ULONG_PTR)lpTemplateName);
    } else {
        Wh_Log(L"%s, resource name: %s", prefix, StrToW(lpTemplateName).p);
    }

    HWND result;

    bool redirected = RedirectModule(
        c, hInstance, []() {},
        [&](HINSTANCE hInstanceRedirect) {
            result = (*Original)(hInstanceRedirect, lpTemplateName, hWndParent,
                                 lpDialogFunc, dwInitParam);
            if (result) {
                Wh_Log(L"[%u] Redirected successfully", c);
                return true;
            }

            DWORD dwError = GetLastError();
            Wh_Log(L"[%u] CreateDialogParam failed with error %u", c, dwError);
            return false;
        });
    if (redirected) {
        return result;
    }

    return (*Original)(hInstance, lpTemplateName, hWndParent, lpDialogFunc,
                       dwInitParam);
}

using CreateDialogParamA_t = decltype(&CreateDialogParamA);
CreateDialogParamA_t CreateDialogParamA_Original;
HWND WINAPI CreateDialogParamA_Hook(HINSTANCE hInstance,
                                    LPCSTR lpTemplateName,
                                    HWND hWndParent,
                                    DLGPROC lpDialogFunc,
                                    LPARAM dwInitParam) {
    return CreateDialogParamAW_Hook<&CreateDialogParamA_Original>(
        hInstance, lpTemplateName, hWndParent, lpDialogFunc, dwInitParam);
}

using CreateDialogParamW_t = decltype(&CreateDialogParamW);
CreateDialogParamW_t CreateDialogParamW_Original;
HWND WINAPI CreateDialogParamW_Hook(HINSTANCE hInstance,
                                    LPCWSTR lpTemplateName,
                                    HWND hWndParent,
                                    DLGPROC lpDialogFunc,
                                    LPARAM dwInitParam) {
    return CreateDialogParamAW_Hook<&CreateDialogParamW_Original>(
        hInstance, lpTemplateName, hWndParent, lpDialogFunc, dwInitParam);
}

template <auto* Original, typename T>
int LoadStringAW_Hook(HINSTANCE hInstance,
                      UINT uID,
                      T* lpBuffer,
                      int cchBufferMax) {
    auto resOp = resourceOperationCountScope();
    DWORD c = ++g_operationCounter;

    WCHAR prefix[64];
    swprintf_s(prefix, L"[%u] > %c", c, chooseAW<T, L'A', L'W'>());

    Wh_Log(L"%s, string number: %u", prefix, uID);

    int result;

    bool redirected = RedirectModule(
        c, hInstance, []() {},
        [&](HINSTANCE hInstanceRedirect) {
            result =
                (*Original)(hInstanceRedirect, uID, lpBuffer, cchBufferMax);
            if (result) {
                Wh_Log(L"[%u] Redirected successfully", c);
                return true;
            }

            DWORD dwError = GetLastError();
            Wh_Log(L"[%u] LoadString failed with error %u", c, dwError);
            return false;
        });
    if (redirected) {
        return result;
    }

    return (*Original)(hInstance, uID, lpBuffer, cchBufferMax);
}

using LoadStringA_t = decltype(&LoadStringA);
LoadStringA_t LoadStringA_u_Original;
int WINAPI LoadStringA_u_Hook(HINSTANCE hInstance,
                              UINT uID,
                              LPSTR lpBuffer,
                              int cchBufferMax) {
    return LoadStringAW_Hook<&LoadStringA_u_Original>(hInstance, uID, lpBuffer,
                                                      cchBufferMax);
}

using LoadStringW_t = decltype(&LoadStringW);
LoadStringW_t LoadStringW_u_Original;
int WINAPI LoadStringW_u_Hook(HINSTANCE hInstance,
                              UINT uID,
                              LPWSTR lpBuffer,
                              int cchBufferMax) {
    return LoadStringAW_Hook<&LoadStringW_u_Original>(hInstance, uID, lpBuffer,
                                                      cchBufferMax);
}

LoadStringA_t LoadStringA_k_Original;
int WINAPI LoadStringA_k_Hook(HINSTANCE hInstance,
                              UINT uID,
                              LPSTR lpBuffer,
                              int cchBufferMax) {
    return LoadStringAW_Hook<&LoadStringA_k_Original>(hInstance, uID, lpBuffer,
                                                      cchBufferMax);
}

LoadStringW_t LoadStringW_k_Original;
int WINAPI LoadStringW_k_Hook(HINSTANCE hInstance,
                              UINT uID,
                              LPWSTR lpBuffer,
                              int cchBufferMax) {
    return LoadStringAW_Hook<&LoadStringW_k_Original>(hInstance, uID, lpBuffer,
                                                      cchBufferMax);
}

template <auto* Original, typename T>
HRSRC FindResourceExAW_Hook(HMODULE hModule,
                            const T* lpType,
                            const T* lpName,
                            WORD wLanguage) {
    DWORD c = ++g_operationCounter;

    WCHAR prefix[64];
    swprintf_s(prefix, L"[%u] > %c", c, chooseAW<T, L'A', L'W'>());

    auto logType = [lpType]() -> std::wstring {
        if (IS_INTRESOURCE(lpType)) {
            return std::to_wstring((DWORD)(ULONG_PTR)lpType);
        } else {
            return StrToW(lpType).p;
        }
    };

    if (IS_INTRESOURCE(lpName)) {
        Wh_Log(L"%s, resource type: %s, number: %u, language: 0x%04X", prefix,
               logType().c_str(), (DWORD)(ULONG_PTR)lpName, wLanguage);
    } else {
        Wh_Log(L"%s, resource type: %s, name: %s, language: 0x%04X", prefix,
               logType().c_str(), StrToW(lpName).p, wLanguage);
    }

    HRSRC result;

    bool redirected = RedirectModule(
        c, hModule, []() {},
        [&](HINSTANCE hInstanceRedirect) {
            result = (*Original)(hInstanceRedirect, lpType, lpName, wLanguage);
            if (result) {
                Wh_Log(L"[%u] Redirected successfully, result=%p", c, result);
                return true;
            }

            DWORD dwError = GetLastError();
            Wh_Log(L"[%u] FindResourceEx failed with error %u", c, dwError);
            return false;
        });
    if (redirected) {
        return result;
    }

    return (*Original)(hModule, lpType, lpName, wLanguage);
}

using FindResourceExA_t = decltype(&FindResourceExA);
FindResourceExA_t FindResourceExA_Original;
HRSRC WINAPI FindResourceExA_Hook(HMODULE hModule,
                                  LPCSTR lpType,
                                  LPCSTR lpName,
                                  WORD wLanguage) {
    if (g_resourceOperationCount > 0) {
        return FindResourceExA_Original(hModule, lpType, lpName, wLanguage);
    }

    return FindResourceExAW_Hook<&FindResourceExA_Original>(hModule, lpType,
                                                            lpName, wLanguage);
}

using FindResourceExW_t = decltype(&FindResourceExW);
FindResourceExW_t FindResourceExW_Original;
HRSRC WINAPI FindResourceExW_Hook(HMODULE hModule,
                                  LPCWSTR lpType,
                                  LPCWSTR lpName,
                                  WORD wLanguage) {
    if (g_resourceOperationCount > 0) {
        return FindResourceExW_Original(hModule, lpType, lpName, wLanguage);
    }

    return FindResourceExAW_Hook<&FindResourceExW_Original>(hModule, lpType,
                                                            lpName, wLanguage);
}

bool IsResourceHandlePartOfModule(HMODULE hModule, HRSRC hResInfo) {
    if ((ULONG_PTR)hModule & 3) {
        MEMORY_BASIC_INFORMATION mbi;
        if (!VirtualQuery((void*)hModule, &mbi, sizeof(mbi))) {
            DWORD dwError = GetLastError();
            Wh_Log(L"VirtualQuery failed with error %u", dwError);
            return false;
        }

        return (void*)hResInfo >= mbi.BaseAddress &&
               (void*)hResInfo <
                   (void*)((BYTE*)mbi.BaseAddress + mbi.RegionSize);
    } else {
        HMODULE module;
        return GetModuleHandleEx(
                   GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                       GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                   (PCWSTR)hResInfo, &module) &&
               module == hModule;
    }
}

using LoadResource_t = decltype(&LoadResource);
LoadResource_t LoadResource_Original;
HGLOBAL WINAPI LoadResource_Hook(HMODULE hModule, HRSRC hResInfo) {
    if (g_resourceOperationCount > 0) {
        return LoadResource_Original(hModule, hResInfo);
    }

    DWORD c = ++g_operationCounter;

    Wh_Log(L"[%u] > hModule=%p, hResInfo=%p", c, hModule, hResInfo);

    HGLOBAL result;

    bool redirected = RedirectModule(
        c, hModule, []() {},
        [&](HINSTANCE hInstanceRedirect) {
            if (!IsResourceHandlePartOfModule(hInstanceRedirect, hResInfo)) {
                Wh_Log(
                    L"[%u] Resource handle is not part of the module, skipping",
                    c);
                return false;
            }

            result = LoadResource_Original(hInstanceRedirect, hResInfo);
            if (result) {
                Wh_Log(L"[%u] Redirected successfully", c);
                return true;
            }

            DWORD dwError = GetLastError();
            Wh_Log(L"[%u] LoadResource failed with error %u", c, dwError);
            return false;
        });
    if (redirected) {
        return result;
    }

    return LoadResource_Original(hModule, hResInfo);
}

using SizeofResource_t = decltype(&SizeofResource);
SizeofResource_t SizeofResource_Original;
DWORD WINAPI SizeofResource_Hook(HMODULE hModule, HRSRC hResInfo) {
    if (g_resourceOperationCount > 0) {
        return SizeofResource_Original(hModule, hResInfo);
    }

    DWORD c = ++g_operationCounter;

    Wh_Log(L"[%u] > hModule=%p, hResInfo=%p", c, hModule, hResInfo);

    DWORD result;

    bool redirected = RedirectModule(
        c, hModule, []() {},
        [&](HINSTANCE hInstanceRedirect) {
            if (!IsResourceHandlePartOfModule(hInstanceRedirect, hResInfo)) {
                Wh_Log(
                    L"[%u] Resource handle is not part of the module, skipping",
                    c);
                return false;
            }

            // Zero can be an error or the actual resource size. Check last
            // error to be sure.
            SetLastError(0);
            result = SizeofResource_Original(hInstanceRedirect, hResInfo);
            DWORD dwError = GetLastError();
            if (result || dwError == 0) {
                Wh_Log(L"[%u] Redirected successfully", c);
                return true;
            }

            Wh_Log(L"[%u] SizeofResource failed with error %u", c, dwError);
            return false;
        });
    if (redirected) {
        return result;
    }

    return SizeofResource_Original(hModule, hResInfo);
}

// https://ntdoc.m417z.com/rtlloadstring
using RtlLoadString_t = NTSTATUS(NTAPI*)(_In_ PVOID DllHandle,
                                         _In_ ULONG StringId,
                                         _In_opt_ PCWSTR StringLanguage,
                                         _In_ ULONG Flags,
                                         _Out_ PCWSTR* ReturnString,
                                         _Out_opt_ PUSHORT ReturnStringLen,
                                         _Out_writes_(ReturnLanguageLen)
                                             PWSTR ReturnLanguageName,
                                         _Inout_opt_ PULONG ReturnLanguageLen);
RtlLoadString_t RtlLoadString_Original;
HRESULT NTAPI RtlLoadString_Hook(_In_ PVOID DllHandle,
                                 _In_ ULONG StringId,
                                 _In_opt_ PCWSTR StringLanguage,
                                 _In_ ULONG Flags,
                                 _Out_ PCWSTR* ReturnString,
                                 _Out_opt_ PUSHORT ReturnStringLen,
                                 _Out_writes_(ReturnLanguageLen)
                                     PWSTR ReturnLanguageName,
                                 _Inout_opt_ PULONG ReturnLanguageLen) {
    if (g_resourceOperationCount > 0) {
        return RtlLoadString_Original(DllHandle, StringId, StringLanguage,
                                      Flags, ReturnString, ReturnStringLen,
                                      ReturnLanguageName, ReturnLanguageLen);
    }

    DWORD c = ++g_operationCounter;

    Wh_Log(L"[%u] > string number: %u", c, StringId);

    NTSTATUS result;

    bool redirected = RedirectModule(
        c, (HINSTANCE)DllHandle, []() {},
        [&](HINSTANCE hInstanceRedirect) {
            result = RtlLoadString_Original(hInstanceRedirect, StringId,
                                            StringLanguage, Flags, ReturnString,
                                            ReturnStringLen, ReturnLanguageName,
                                            ReturnLanguageLen);
            if (result != 0) {
                Wh_Log(L"[%u] RtlLoadString failed with error %08X", c, result);
                return false;
            }

            if (!*ReturnString) {
                Wh_Log(L"[%u] RtlLoadString returned an empty string", c);
                return false;
            }

            Wh_Log(L"[%u] Redirected successfully", c);
            return true;
        });
    if (redirected) {
        return result;
    }

    return RtlLoadString_Original(DllHandle, StringId, StringLanguage, Flags,
                                  ReturnString, ReturnStringLen,
                                  ReturnLanguageName, ReturnLanguageLen);
}

using SHCreateStreamOnModuleResourceW_t = HRESULT(WINAPI*)(HMODULE hModule,
                                                           LPCWSTR pwszName,
                                                           LPCWSTR pwszType,
                                                           IStream** ppStream);
SHCreateStreamOnModuleResourceW_t SHCreateStreamOnModuleResourceW_Original;
HRESULT WINAPI SHCreateStreamOnModuleResourceW_Hook(HMODULE hModule,
                                                    LPCWSTR pwszName,
                                                    LPCWSTR pwszType,
                                                    IStream** ppStream) {
    auto resOp = resourceOperationCountScope();
    DWORD c = ++g_operationCounter;

    PCWSTR logTypeStr;
    WCHAR logTypeStrBuffer[16];
    if (IS_INTRESOURCE(pwszType)) {
        swprintf_s(logTypeStrBuffer, L"%u", (DWORD)(ULONG_PTR)pwszType);
        logTypeStr = logTypeStrBuffer;
    } else {
        logTypeStr = pwszType;
    }

    if (IS_INTRESOURCE(pwszName)) {
        Wh_Log(L"[%u] > Type: %s, resource number: %u", c, logTypeStr,
               (DWORD)(ULONG_PTR)pwszName);
    } else {
        Wh_Log(L"[%u] > Type: %s, resource name: %s", c, logTypeStr, pwszName);
    }

    HRESULT result;

    bool redirected = RedirectModule(
        c, hModule, []() {},
        [&](HINSTANCE hInstanceRedirect) {
            result = SHCreateStreamOnModuleResourceW_Original(
                hInstanceRedirect, pwszName, pwszType, ppStream);
            if (SUCCEEDED(result)) {
                Wh_Log(L"[%u] Redirected successfully", c);
                return true;
            }

            Wh_Log(
                L"[%u] SHCreateStreamOnModuleResourceW failed with error %08X",
                c, result);
            return false;
        });
    if (redirected) {
        return result;
    }

    return SHCreateStreamOnModuleResourceW_Original(hModule, pwszName, pwszType,
                                                    ppStream);
}

void DirectUI_DUIXmlParser_SetDefaultHInstance(void* pThis, HMODULE hModule) {
    using DirectUI_DUIXmlParser_SetDefaultHInstance_t =
        void(__thiscall*)(void* pThis, HMODULE hModule);
    static DirectUI_DUIXmlParser_SetDefaultHInstance_t pSetDefaultHInstance = []() {
        HMODULE duiModule =
            LoadLibraryEx(L"dui70.dll", NULL, LOAD_LIBRARY_SEARCH_SYSTEM32);
        if (duiModule) {
            PCSTR procName =
#ifdef _WIN64
                R"(?SetDefaultHInstance@DUIXmlParser@DirectUI@@QEAAXPEAUHINSTANCE__@@@Z)";
#else
                R"(?SetDefaultHInstance@DUIXmlParser@DirectUI@@QAEXPAUHINSTANCE__@@@Z)";
#endif
            FARPROC pSetXMLFromResource = GetProcAddress(duiModule, procName);
            if (pSetXMLFromResource) {
                return (DirectUI_DUIXmlParser_SetDefaultHInstance_t)
                    pSetXMLFromResource;
            } else {
                Wh_Log(L"Couldn't find SetDefaultHInstance");
            }
        } else {
            Wh_Log(L"Couldn't load dui70.dll");
        }

        return (DirectUI_DUIXmlParser_SetDefaultHInstance_t) nullptr;
    }();

    if (pSetDefaultHInstance) {
        pSetDefaultHInstance(pThis, hModule);
    }
}

using SetXMLFromResource_t = HRESULT(__thiscall*)(void* pThis,
                                                  PCWSTR lpName,
                                                  PCWSTR lpType,
                                                  HMODULE hModule,
                                                  HINSTANCE param4,
                                                  HINSTANCE param5);
SetXMLFromResource_t SetXMLFromResource_Original;
HRESULT __thiscall SetXMLFromResource_Hook(void* pThis,
                                           PCWSTR lpName,
                                           PCWSTR lpType,
                                           HMODULE hModule,
                                           HINSTANCE param4,
                                           HINSTANCE param5) {
    auto resOp = resourceOperationCountScope();
    DWORD c = ++g_operationCounter;

    PCWSTR logTypeStr;
    WCHAR logTypeStrBuffer[16];
    if (IS_INTRESOURCE(lpType)) {
        swprintf_s(logTypeStrBuffer, L"%u", (DWORD)(ULONG_PTR)lpType);
        logTypeStr = logTypeStrBuffer;
    } else {
        logTypeStr = lpType;
    }

    if (IS_INTRESOURCE(lpName)) {
        Wh_Log(L"[%u] > Type: %s, resource number: %u", c, logTypeStr,
               (DWORD)(ULONG_PTR)lpName);
    } else {
        Wh_Log(L"[%u] > Type: %s, resource name: %s", c, logTypeStr, lpName);
    }

    HRESULT result;

    bool redirected = RedirectModule(
        c, hModule, []() {},
        [&](HINSTANCE hInstanceRedirect) {
            result = SetXMLFromResource_Original(
                pThis, lpName, lpType, hInstanceRedirect, param4, param5);
            if (SUCCEEDED(result)) {
                Wh_Log(L"[%u] Redirected successfully", c);
                return true;
            }

            Wh_Log(L"[%u] SetXMLFromResource failed with error %08X", c,
                   result);
            return false;
        });
    if (redirected) {
        // By using a redirected module, its handle will be saved by
        // DUIXmlParser and will be used for loading additional resources, such
        // as strings. This might be undesirable, so set the original module. An
        // example for why setting the original module is sometimes preferable:
        // https://github.com/ramensoftware/windhawk-mods/issues/639
        DirectUI_DUIXmlParser_SetDefaultHInstance(pThis, hModule);
        return result;
    }

    return SetXMLFromResource_Original(pThis, lpName, lpType, hModule, param4,
                                       param5);
}

// https://devblogs.microsoft.com/oldnewthing/20040130-00/?p=40813
LPCWSTR FindStringResourceEx(HINSTANCE hinst, UINT uId, UINT langId) {
    // Convert the string ID into a bundle number
    LPCWSTR pwsz = NULL;
    HRSRC hrsrc =
        FindResourceEx(hinst, RT_STRING, MAKEINTRESOURCE(uId / 16 + 1), langId);
    if (hrsrc) {
        HGLOBAL hglob = LoadResource(hinst, hrsrc);
        if (hglob) {
            pwsz = reinterpret_cast<LPCWSTR>(LockResource(hglob));
            if (pwsz) {
                // okay now walk the string table
                for (UINT i = 0; i < (uId & 15); i++) {
                    pwsz += 1 + (UINT)*pwsz;
                }
            }
        }
    }
    return pwsz;
}

using DirectUI_CreateString_t = void*(WINAPI*)(PCWSTR name,
                                               HINSTANCE hInstance);
DirectUI_CreateString_t DirectUI_CreateString_Original;
void* WINAPI DirectUI_CreateString_Hook(PCWSTR name, HINSTANCE hInstance) {
    if (!hInstance) {
        return DirectUI_CreateString_Original(name, hInstance);
    }

    auto resOp = resourceOperationCountScope();
    DWORD c = ++g_operationCounter;

    Wh_Log(L"[%u] > DUI string number: %u", c, (DWORD)(ULONG_PTR)name);

    void* result;

    bool redirected = RedirectModule(
        c, hInstance, []() {},
        [&](HINSTANCE hInstanceRedirect) {
            // For other redirected functions, we check whether the function
            // succeeded. If it didn't, we try another redirection or fall back
            // to the original file.
            //
            // In this case, there's no reliable way to find out whether
            // the function failed, since it just uses an empty string if it's
            // missing. Therefore, only make sure that the string resource
            // exists.
            UINT uId = (DWORD)(ULONG_PTR)name;
            PCWSTR string = FindStringResourceEx(hInstanceRedirect, uId, 0);
            if (!string || !*string) {
                Wh_Log(L"[%u] Resource not found", c);
                return false;
            }

            result = DirectUI_CreateString_Original(name, hInstanceRedirect);
            Wh_Log(L"[%u] Redirected successfully", c);
            return true;
        });
    if (redirected) {
        return result;
    }

    return DirectUI_CreateString_Original(name, hInstance);
}

bool IsExplorerProcess() {
    WCHAR path[MAX_PATH];
    if (!GetWindowsDirectory(path, ARRAYSIZE(path))) {
        Wh_Log(L"GetWindowsDirectory failed");
        return false;
    }

    wcscat_s(path, MAX_PATH, L"\\explorer.exe");

    return GetModuleHandle(path) == GetModuleHandle(nullptr);
}

HWND FindCurrentProcessTaskbarWnd() {
    HWND hTaskbarWnd = nullptr;

    EnumWindows(
        [](HWND hWnd, LPARAM lParam) WINAPI -> BOOL {
            DWORD dwProcessId;
            WCHAR className[32];
            if (GetWindowThreadProcessId(hWnd, &dwProcessId) &&
                dwProcessId == GetCurrentProcessId() &&
                GetClassName(hWnd, className, ARRAYSIZE(className)) &&
                _wcsicmp(className, L"Shell_TrayWnd") == 0) {
                *reinterpret_cast<HWND*>(lParam) = hWnd;
                return FALSE;
            }
            return TRUE;
        },
        reinterpret_cast<LPARAM>(&hTaskbarWnd));

    return hTaskbarWnd;
}

bool DoesCurrentProcessOwnTaskbar() {
    return IsExplorerProcess() && FindCurrentProcessTaskbarWnd();
}

void PromptToClearCache() {
    if (g_clearCachePromptThread) {
        if (WaitForSingleObject(g_clearCachePromptThread, 0) != WAIT_OBJECT_0) {
            return;
        }

        CloseHandle(g_clearCachePromptThread);
    }

    g_clearCachePromptThread = CreateThread(
        nullptr, 0,
        [](LPVOID lpParameter) WINAPI -> DWORD {
            TASKDIALOGCONFIG taskDialogConfig{
                .cbSize = sizeof(taskDialogConfig),
                .dwFlags = TDF_ALLOW_DIALOG_CANCELLATION,
                .dwCommonButtons = TDCBF_YES_BUTTON | TDCBF_NO_BUTTON,
                .pszWindowTitle = kClearCachePromptTitle,
                .pszMainIcon = TD_INFORMATION_ICON,
                .pszContent = kClearCachePromptText,
                .pfCallback = [](HWND hwnd, UINT msg, WPARAM wParam,
                                 LPARAM lParam, LONG_PTR lpRefData)
                                  WINAPI -> HRESULT {
                    switch (msg) {
                        case TDN_CREATED:
                            g_clearCachePromptWindow = hwnd;
                            SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0,
                                         SWP_NOMOVE | SWP_NOSIZE);
                            break;

                        case TDN_DESTROYED:
                            g_clearCachePromptWindow = nullptr;
                            break;
                    }

                    return S_OK;
                },
            };

            static decltype(&TaskDialogIndirect) pTaskDialogIndirect = []() {
                HMODULE hComctl32 = LoadLibraryEx(L"comctl32.dll", nullptr,
                                                  LOAD_LIBRARY_SEARCH_SYSTEM32);
                if (!hComctl32) {
                    Wh_Log(L"Failed to load comctl32.dll");
                    return (decltype(&TaskDialogIndirect))nullptr;
                }

                return (decltype(&TaskDialogIndirect))GetProcAddress(
                    hComctl32, "TaskDialogIndirect");
            }();

            int button;
            if (pTaskDialogIndirect &&
                SUCCEEDED(pTaskDialogIndirect(&taskDialogConfig, &button,
                                              nullptr, nullptr)) &&
                button == IDYES) {
                WCHAR commandLine[ARRAYSIZE(kClearCacheCommand)];
                memcpy(commandLine, kClearCacheCommand,
                       sizeof(kClearCacheCommand));
                STARTUPINFO si = {
                    .cb = sizeof(si),
                };
                PROCESS_INFORMATION pi{};
                if (CreateProcess(nullptr, commandLine, nullptr, nullptr, FALSE,
                                  0, nullptr, nullptr, &si, &pi)) {
                    CloseHandle(pi.hThread);
                    CloseHandle(pi.hProcess);
                }
            }

            return 0;
        },
        nullptr, 0, nullptr);
}

HANDLE LockTempFileExclusive(PCWSTR filePath, DWORD timeoutMs) {
    HANDLE hFile =
        CreateFile(filePath, GENERIC_READ | GENERIC_WRITE,
                   FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_ALWAYS,
                   FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, nullptr);
    if (hFile == INVALID_HANDLE_VALUE) {
        return INVALID_HANDLE_VALUE;
    }

    HANDLE hEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);
    if (!hEvent) {
        CloseHandle(hFile);
        return INVALID_HANDLE_VALUE;
    }

    OVERLAPPED ov = {
        .hEvent = hEvent,
    };

    // Lock first byte only.
    BOOL locked = LockFileEx(hFile, LOCKFILE_EXCLUSIVE_LOCK, 0, 1, 0, &ov);
    if (!locked) {
        DWORD err = GetLastError();
        if (err != ERROR_IO_PENDING) {
            CloseHandle(hEvent);
            CloseHandle(hFile);
            return INVALID_HANDLE_VALUE;
        }

        DWORD waitResult = WaitForSingleObject(hEvent, timeoutMs);
        if (waitResult != WAIT_OBJECT_0) {
            CancelIo(hFile);
            CloseHandle(hEvent);
            CloseHandle(hFile);
            return INVALID_HANDLE_VALUE;
        }

        DWORD bytesTransferred;
        if (!GetOverlappedResult(hFile, &ov, &bytesTransferred, FALSE)) {
            CloseHandle(hEvent);
            CloseHandle(hFile);
            return INVALID_HANDLE_VALUE;
        }
    }

    CloseHandle(hEvent);
    return hFile;
}

BOOL UnlockTempFileExclusive(HANDLE hFile) {
    OVERLAPPED ov = {};
    BOOL unlocked = UnlockFileEx(hFile, 0, 1, 0, &ov);
    CloseHandle(hFile);
    return unlocked;
}

HRESULT UnzipToFolder(BSTR zipFilePath, BSTR destinationPath) {
    winrt::com_ptr<IShellDispatch> shellDispatch;
    HRESULT hr = CoCreateInstance(CLSID_Shell, nullptr, CLSCTX_INPROC_SERVER,
                                  IID_PPV_ARGS(shellDispatch.put()));
    if (FAILED(hr))
        return hr;

    VARIANT zipFileVariant;
    zipFileVariant.vt = VT_BSTR;
    zipFileVariant.bstrVal = zipFilePath;

    winrt::com_ptr<Folder> zipFile;
    hr = shellDispatch->NameSpace(zipFileVariant, zipFile.put());
    if (FAILED(hr))
        return hr;
    if (!zipFile)
        return E_FAIL;

    VARIANT destinationVariant;
    destinationVariant.vt = VT_BSTR;
    destinationVariant.bstrVal = destinationPath;

    winrt::com_ptr<Folder> destination;
    hr = shellDispatch->NameSpace(destinationVariant, destination.put());
    if (FAILED(hr))
        return hr;
    if (!destination)
        return E_FAIL;

    winrt::com_ptr<FolderItems> zipFiles;
    hr = zipFile->Items(zipFiles.put());
    if (FAILED(hr))
        return hr;
    if (!zipFiles)
        return E_FAIL;

    LONG zipFilesCount;
    hr = zipFiles->get_Count(&zipFilesCount);
    if (FAILED(hr))
        return hr;

    // If the zip contains a single folder, select it to avoid an extra nesting.
    if (zipFilesCount == 1) {
        VARIANT index;
        index.vt = VT_I4;
        index.lVal = 0;

        winrt::com_ptr<FolderItem> zipSubFolderItem;
        hr = zipFiles->Item(index, zipSubFolderItem.put());
        if (FAILED(hr))
            return hr;
        if (!zipSubFolderItem)
            return E_FAIL;

        VARIANT_BOOL isFolder;
        hr = zipSubFolderItem->get_IsFolder(&isFolder);
        if (FAILED(hr))
            return hr;

        if (isFolder) {
            winrt::com_ptr<IDispatch> zipSubFolderDispatch;
            hr = zipSubFolderItem->get_GetFolder(zipSubFolderDispatch.put());
            if (FAILED(hr))
                return hr;
            if (!zipSubFolderDispatch)
                return E_FAIL;

            winrt::com_ptr<Folder> zipSubFolder;
            hr = zipSubFolderDispatch->QueryInterface(
                IID_PPV_ARGS(zipSubFolder.put()));
            if (FAILED(hr))
                return hr;
            if (!zipSubFolder)
                return E_FAIL;

            hr = zipSubFolder->Items(zipFiles.put());
            if (FAILED(hr))
                return hr;
            if (!zipFiles)
                return E_FAIL;
        }
    }

    winrt::com_ptr<IDispatch> zipFilesDispatch;
    hr = zipFiles->QueryInterface(IID_PPV_ARGS(zipFilesDispatch.put()));
    if (FAILED(hr))
        return hr;
    if (!zipFilesDispatch)
        return E_FAIL;

    VARIANT itemVariant;
    itemVariant.vt = VT_DISPATCH;
    itemVariant.pdispVal = zipFilesDispatch.get();

    VARIANT options;
    options.vt = VT_I4;
    options.lVal = FOF_NO_UI;

    return destination->CopyHere(itemVariant, options);
}

bool DownloadAndExtractIconTheme(std::wstring_view relativeUrl,
                                 const std::filesystem::path& tempFilePath,
                                 const std::filesystem::path& tempFolderPath,
                                 const std::filesystem::path& targetPath) {
    std::wstring url =
        L"https://ramensoftware.github.io/resource-redirect-themes/";
    url += relativeUrl;

    WH_GET_URL_CONTENT_OPTIONS options{
        .optionsSize = sizeof(options),
        .targetFilePath = tempFilePath.c_str(),
    };
    const WH_URL_CONTENT* urlContent = Wh_GetUrlContent(url.c_str(), &options);
    if (!urlContent) {
        Wh_Log(L"Wh_GetUrlContent failed");
        return false;
    }

    if (urlContent->statusCode != 200) {
        Wh_Log(L"Wh_GetUrlContent returned %d", urlContent->statusCode);
        return false;
    }

    Wh_FreeUrlContent(urlContent);

    HRESULT hr = CoInitialize(nullptr);
    if (SUCCEEDED(hr)) {
        std::error_code ec;
        std::filesystem::remove_all(tempFolderPath, ec);
        std::filesystem::create_directories(tempFolderPath, ec);

        hr = UnzipToFolder(_bstr_t(tempFilePath.c_str()),
                           _bstr_t(tempFolderPath.c_str()));
        if (FAILED(hr)) {
            Wh_Log(L"UnzipToFolder returned 0x%08X", hr);
        }

        std::filesystem::rename(tempFolderPath, targetPath, ec);

        CoUninitialize();
    } else {
        Wh_Log(L"CoInitialize returned 0x%08X", hr);
    }

    return SUCCEEDED(hr);
}

std::filesystem::path EnsureIconThemeAvailable(
    const std::filesystem::path& storagePath,
    std::wstring_view themeName,
    std::wstring_view relativeUrl,
    PCWSTR themeFolderKey) {
    std::error_code ec;

    // Check if we have a stored folder name for this theme.
    WCHAR storedFolderName[MAX_PATH];
    Wh_GetStringValue(themeFolderKey, storedFolderName,
                      ARRAYSIZE(storedFolderName));
    if (*storedFolderName) {
        auto storedPath = storagePath / storedFolderName;
        if (std::filesystem::is_directory(storedPath, ec)) {
            return storedPath;
        }
    }

    // Theme not available, need to find a usable folder and download.
    WCHAR lastErrorThemeName[256];
    Wh_GetStringValue(L"lastErrorThemeName", lastErrorThemeName,
                      ARRAYSIZE(lastErrorThemeName));
    if (lastErrorThemeName == themeName) {
        FILETIME filetimeNow;
        GetSystemTimeAsFileTime(&filetimeNow);
        ULARGE_INTEGER timeNow{
            .HighPart = filetimeNow.dwHighDateTime,
            .LowPart = filetimeNow.dwLowDateTime,
        };

        ULARGE_INTEGER timeLastError{
            .HighPart = (DWORD)Wh_GetIntValue(L"lastErrorTimeHigh", 0),
            .LowPart = (DWORD)Wh_GetIntValue(L"lastErrorTimeLow", 0),
        };

        ULONGLONG elapsedSec =
            (timeNow.QuadPart - timeLastError.QuadPart) / 10000000;
        if (elapsedSec < 60 * 60 * 4) {
            Wh_Log(L"Aborting due to error %u seconds ago", elapsedSec);
            return std::filesystem::path();
        }
    }

    // Find a usable folder name.
    std::filesystem::path targetPath;
    std::wstring folderName;
    for (int suffix = 0; suffix < 100; suffix++) {
        if (suffix == 0) {
            folderName = themeName;
        } else {
            folderName =
                std::wstring(themeName) + L"_" + std::to_wstring(suffix + 1);
        }

        targetPath = storagePath / folderName;

        if (!std::filesystem::is_directory(targetPath, ec)) {
            // Folder doesn't exist, we can use it.
            break;
        }

        // Folder exists, try to remove it.
        Wh_Log(L"Folder exists, trying to remove: %s", targetPath.c_str());
        std::filesystem::remove_all(targetPath, ec);

        if (!std::filesystem::is_directory(targetPath, ec)) {
            // Successfully removed.
            break;
        }

        Wh_Log(L"Failed to remove folder, trying next name");
    }

    if (std::filesystem::is_directory(targetPath, ec)) {
        Wh_Log(L"Failed to find a usable folder name");
        return std::filesystem::path();
    }

    Wh_Log(L"Downloading from %.*s", static_cast<int>(relativeUrl.length()),
           relativeUrl.data());

    bool downloaded = false;
    auto tempZip = storagePath / L"_temp.zip";
    auto tempFolder = storagePath / L"_temp_extracted";
    if (DownloadAndExtractIconTheme(relativeUrl, tempZip, tempFolder,
                                    targetPath)) {
        DeleteFile(tempZip.c_str());
        downloaded = std::filesystem::is_directory(targetPath, ec);
    }

    if (!downloaded) {
        FILETIME filetimeNow;
        GetSystemTimeAsFileTime(&filetimeNow);
        Wh_SetStringValue(L"lastErrorThemeName",
                          std::wstring(themeName).c_str());
        Wh_SetIntValue(L"lastErrorTimeHigh", (int)filetimeNow.dwHighDateTime);
        Wh_SetIntValue(L"lastErrorTimeLow", (int)filetimeNow.dwLowDateTime);
        return std::filesystem::path();
    }

    // Store the folder name on successful download and extract.
    Wh_SetStringValue(themeFolderKey, folderName.c_str());

    return targetPath;
}

std::wstring GetIconThemePath(std::wstring_view iconTheme) {
    auto iconThemeSepIt = iconTheme.find(L"|");
    if (iconThemeSepIt == iconTheme.npos) {
        return std::wstring();
    }

    auto themeName = iconTheme.substr(0, iconThemeSepIt);
    auto relativeUrl = iconTheme.substr(iconThemeSepIt + 1);

    WCHAR storagePathBuffer[MAX_PATH];
    if (!Wh_GetModStoragePath(storagePathBuffer,
                              ARRAYSIZE(storagePathBuffer))) {
        Wh_Log(L"Wh_GetModStoragePath failed");
        return std::wstring();
    }

    const auto storagePath = std::filesystem::path{storagePathBuffer};

    auto themeFolderKey = L"themeFolder_" + std::wstring(themeName);

    WCHAR storedFolderName[MAX_PATH];
    Wh_GetStringValue(themeFolderKey.c_str(), storedFolderName,
                      ARRAYSIZE(storedFolderName));
    if (*storedFolderName) {
        auto storedPath = storagePath / storedFolderName;
        std::error_code ec;
        if (std::filesystem::is_directory(storedPath, ec)) {
            return storedPath;
        }
    }

    auto lockFilePath = storagePath / L"_lock";

    HANDLE lockFile = LockTempFileExclusive(lockFilePath.c_str(), 30000);
    if (!lockFile) {
        Wh_Log(L"LockTempFileExclusive failed");
        return std::wstring();
    }

    auto targetPath = EnsureIconThemeAvailable(
        storagePath, themeName, relativeUrl, themeFolderKey.c_str());

    UnlockTempFileExclusive(lockFile);
    DeleteFile(lockFilePath.c_str());

    return targetPath;
}

void LoadSettings() {
    g_settings.iconTheme = WindhawkUtils::StringSetting::make(L"iconTheme");
    g_settings.allResourceRedirect = Wh_GetIntSetting(L"allResourceRedirect");

    std::unordered_map<std::wstring, std::vector<std::wstring>> paths;
    std::unordered_map<std::string, std::vector<std::string>> pathsA;
    std::vector<std::pair<std::wstring, std::wstring>> pathPatterns;
    std::vector<std::pair<std::string, std::string>> pathPatternsA;

    auto addRedirectionPath = [&paths, &pathsA, &pathPatterns, &pathPatternsA](
                                  PCWSTR original, PCWSTR redirect) {
        WCHAR originalExpanded[MAX_PATH];
        DWORD originalExpandedLen = ExpandEnvironmentStrings(
            original, originalExpanded, ARRAYSIZE(originalExpanded));
        if (!originalExpandedLen ||
            originalExpandedLen > ARRAYSIZE(originalExpanded)) {
            Wh_Log(L"Failed to expand path: %s", original);
            return;
        }

        // Remove null terminator from len.
        originalExpandedLen--;

        bool isPattern = wcscspn(originalExpanded, L"*?") < originalExpandedLen;

        Wh_Log(L"Configuring%s %s->%s", isPattern ? L" pattern" : L"",
               originalExpanded, redirect);

        LCMapStringEx(LOCALE_NAME_USER_DEFAULT, LCMAP_UPPERCASE,
                      originalExpanded, originalExpandedLen, originalExpanded,
                      originalExpandedLen, nullptr, nullptr, 0);

        if (isPattern) {
            pathPatterns.push_back({originalExpanded, redirect});
        } else {
            paths[originalExpanded].push_back(redirect);
        }

        char originalExpandedA[MAX_PATH];
        char redirectA[MAX_PATH];
        size_t charsConverted = 0;
        if (wcstombs_s(&charsConverted, originalExpandedA,
                       ARRAYSIZE(originalExpandedA), originalExpanded,
                       _TRUNCATE) == 0 &&
            wcstombs_s(&charsConverted, redirectA, ARRAYSIZE(redirectA),
                       redirect, _TRUNCATE) == 0) {
            if (isPattern) {
                pathPatternsA.push_back({originalExpandedA, redirectA});
            } else {
                pathsA[originalExpandedA].push_back(redirectA);
            }
        } else {
            Wh_Log(L"Error configuring ANSI redirection");
        }
    };

    auto addRedirectionThemePath = [&addRedirectionPath](PCWSTR themePath) {
        auto initialPath = std::filesystem::path{themePath};

        std::filesystem::path themeFolder;
        std::filesystem::path themeIniFile;
        if (std::filesystem::is_directory(initialPath)) {
            themeFolder = initialPath;
            themeIniFile = themeFolder / L"theme.ini";
        } else {
            themeIniFile = initialPath;
            themeFolder = themeIniFile.parent_path();
        }

        auto fileSize = std::filesystem::file_size(themeIniFile);

        std::wstring data(fileSize + 2, L'\0');
        DWORD result = GetPrivateProfileSection(
            L"redirections", data.data(), data.size(), themeIniFile.c_str());
        if (!result || result == data.size() - 2) {
            DWORD dwError = GetLastError();
            Wh_Log(L"Error reading data from %s: %u", themeIniFile.c_str(),
                   dwError);
            return false;
        }

        for (auto* p = data.data(); *p;) {
            auto* pNext = p + wcslen(p) + 1;
            auto* pEq = wcschr(p, L'=');
            if (pEq) {
                *pEq = L'\0';

                auto redirectFile = themeFolder / (pEq + 1);
                addRedirectionPath(p, redirectFile.c_str());
            } else {
                Wh_Log(L"Skipping %s", p);
            }

            p = pNext;
        }

        return true;
    };

    if (*g_settings.iconTheme) {
        std::wstring iconThemePath =
            GetIconThemePath(g_settings.iconTheme.get());
        if (!iconThemePath.empty()) {
            try {
                addRedirectionThemePath(iconThemePath.c_str());
            } catch (const std::exception& ex) {
                Wh_Log(L"Error: %S", ex.what());
            }
        }
    }

    for (int i = 0;; i++) {
        PCWSTR themePath = Wh_GetStringSetting(L"themePaths[%d]", i);
        bool hasThemePath = *themePath;
        if (hasThemePath) {
            try {
                addRedirectionThemePath(themePath);
            } catch (const std::exception& ex) {
                Wh_Log(L"Error: %S", ex.what());
            }
        }
        Wh_FreeStringSetting(themePath);
        if (!hasThemePath) {
            break;
        }
    }

    PCWSTR themeFolder = Wh_GetStringSetting(L"themeFolder");
    if (*themeFolder) {
        try {
            addRedirectionThemePath(themeFolder);
        } catch (const std::exception& ex) {
            Wh_Log(L"Error: %S", ex.what());
        }
    }
    Wh_FreeStringSetting(themeFolder);

    for (int i = 0;; i++) {
        PCWSTR original =
            Wh_GetStringSetting(L"redirectionResourcePaths[%d].original", i);
        PCWSTR redirect =
            Wh_GetStringSetting(L"redirectionResourcePaths[%d].redirect", i);

        bool hasRedirection = *original || *redirect;

        if (hasRedirection) {
            addRedirectionPath(original, redirect);
        }

        Wh_FreeStringSetting(original);
        Wh_FreeStringSetting(redirect);

        if (!hasRedirection) {
            break;
        }
    }

    // Reverse the order to allow later entries override earlier ones.
    std::reverse(pathPatterns.begin(), pathPatterns.end());
    std::reverse(pathPatternsA.begin(), pathPatternsA.end());

    std::unique_lock lock{g_redirectionResourcePathsMutex};
    g_redirectionResourcePaths = std::move(paths);
    g_redirectionResourcePathsA = std::move(pathsA);
    g_redirectionResourcePathPatterns = std::move(pathPatterns);
    g_redirectionResourcePathPatternsA = std::move(pathPatternsA);
}

BOOL Wh_ModInit() {
    Wh_Log(L">");

    LoadSettings();

    HMODULE kernelBaseModule = GetModuleHandle(L"kernelbase.dll");
    HMODULE kernel32Module = GetModuleHandle(L"kernel32.dll");

    auto setKernelFunctionHook = [kernelBaseModule, kernel32Module](
                                     PCSTR targetName, void* hookFunction,
                                     void** originalFunction) {
        void* targetFunction =
            (void*)GetProcAddress(kernelBaseModule, targetName);
        if (!targetFunction) {
            targetFunction = (void*)GetProcAddress(kernel32Module, targetName);
            if (!targetFunction) {
                return FALSE;
            }
        }

        return Wh_SetFunctionHook(targetFunction, hookFunction,
                                  originalFunction);
    };

    Wh_SetFunctionHook((void*)PrivateExtractIconsW,
                       (void*)PrivateExtractIconsW_Hook,
                       (void**)&PrivateExtractIconsW_Original);

    // The functions below use FindResourceEx, LoadResource, SizeofResource.
    Wh_SetFunctionHook((void*)LoadImageA, (void*)LoadImageA_Hook,
                       (void**)&LoadImageA_Original);

    Wh_SetFunctionHook((void*)LoadImageW, (void*)LoadImageW_Hook,
                       (void**)&LoadImageW_Original);

    Wh_SetFunctionHook((void*)LoadIconA, (void*)LoadIconA_Hook,
                       (void**)&LoadIconA_Original);

    Wh_SetFunctionHook((void*)LoadIconW, (void*)LoadIconW_Hook,
                       (void**)&LoadIconW_Original);

    Wh_SetFunctionHook((void*)LoadCursorA, (void*)LoadCursorA_Hook,
                       (void**)&LoadCursorA_Original);

    Wh_SetFunctionHook((void*)LoadCursorW, (void*)LoadCursorW_Hook,
                       (void**)&LoadCursorW_Original);

    Wh_SetFunctionHook((void*)LoadBitmapA, (void*)LoadBitmapA_Hook,
                       (void**)&LoadBitmapA_Original);

    Wh_SetFunctionHook((void*)LoadBitmapW, (void*)LoadBitmapW_Hook,
                       (void**)&LoadBitmapW_Original);

    Wh_SetFunctionHook((void*)LoadMenuA, (void*)LoadMenuA_Hook,
                       (void**)&LoadMenuA_Original);

    Wh_SetFunctionHook((void*)LoadMenuW, (void*)LoadMenuW_Hook,
                       (void**)&LoadMenuW_Original);

    Wh_SetFunctionHook((void*)DialogBoxParamA, (void*)DialogBoxParamA_Hook,
                       (void**)&DialogBoxParamA_Original);

    Wh_SetFunctionHook((void*)DialogBoxParamW, (void*)DialogBoxParamW_Hook,
                       (void**)&DialogBoxParamW_Original);

    Wh_SetFunctionHook((void*)CreateDialogParamA,
                       (void*)CreateDialogParamA_Hook,
                       (void**)&CreateDialogParamA_Original);

    Wh_SetFunctionHook((void*)CreateDialogParamW,
                       (void*)CreateDialogParamW_Hook,
                       (void**)&CreateDialogParamW_Original);

    // The functions below use RtlLoadString.
    Wh_SetFunctionHook((void*)LoadStringA, (void*)LoadStringA_u_Hook,
                       (void**)&LoadStringA_u_Original);

    Wh_SetFunctionHook((void*)LoadStringW, (void*)LoadStringW_u_Hook,
                       (void**)&LoadStringW_u_Original);

    setKernelFunctionHook("LoadStringA", (void*)LoadStringA_k_Hook,
                          (void**)&LoadStringA_k_Original);

    setKernelFunctionHook("LoadStringW", (void*)LoadStringW_k_Hook,
                          (void**)&LoadStringW_k_Original);

    if (g_settings.allResourceRedirect) {
        setKernelFunctionHook("FindResourceExA", (void*)FindResourceExA_Hook,
                              (void**)&FindResourceExA_Original);
        setKernelFunctionHook("FindResourceExW", (void*)FindResourceExW_Hook,
                              (void**)&FindResourceExW_Original);
        setKernelFunctionHook("LoadResource", (void*)LoadResource_Hook,
                              (void**)&LoadResource_Original);
        setKernelFunctionHook("SizeofResource", (void*)SizeofResource_Hook,
                              (void**)&SizeofResource_Original);

        void* pRtlLoadString = (void*)GetProcAddress(
            GetModuleHandle(L"ntdll.dll"), "RtlLoadString");
        if (pRtlLoadString) {
            Wh_SetFunctionHook(pRtlLoadString, (void*)RtlLoadString_Hook,
                               (void**)&RtlLoadString_Original);
        }
    }

    // The functions below use FindResourceEx, LoadResource, SizeofResource.
    HMODULE shcoreModule =
        LoadLibraryEx(L"shcore.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (shcoreModule) {
        FARPROC pSHCreateStreamOnModuleResourceW =
            GetProcAddress(shcoreModule, (PCSTR)109);
        if (pSHCreateStreamOnModuleResourceW) {
            Wh_SetFunctionHook(
                (void*)pSHCreateStreamOnModuleResourceW,
                (void*)SHCreateStreamOnModuleResourceW_Hook,
                (void**)&SHCreateStreamOnModuleResourceW_Original);
        } else {
            Wh_Log(L"Couldn't find SHCreateStreamOnModuleResourceW (#109)");
        }
    } else {
        Wh_Log(L"Couldn't load shcore.dll");
    }

    HMODULE duiModule =
        LoadLibraryEx(L"dui70.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (duiModule) {
        PCSTR SetXMLFromResource_Name =
            R"(?_SetXMLFromResource@DUIXmlParser@DirectUI@@IAEJPBG0PAUHINSTANCE__@@11@Z)";
        FARPROC pSetXMLFromResource =
            GetProcAddress(duiModule, SetXMLFromResource_Name);
        if (!pSetXMLFromResource) {
#ifdef _WIN64
            PCSTR SetXMLFromResource_Name_Win10_x64 =
                R"(?_SetXMLFromResource@DUIXmlParser@DirectUI@@IEAAJPEBG0PEAUHINSTANCE__@@11@Z)";
            pSetXMLFromResource =
                GetProcAddress(duiModule, SetXMLFromResource_Name_Win10_x64);
#endif
        }

        if (pSetXMLFromResource) {
            Wh_SetFunctionHook((void*)pSetXMLFromResource,
                               (void*)SetXMLFromResource_Hook,
                               (void**)&SetXMLFromResource_Original);
        } else {
            Wh_Log(L"Couldn't find SetXMLFromResource");
        }

        PCSTR DirectUI_CreateString_Name =
#ifdef _WIN64
            R"(?CreateString@Value@DirectUI@@SAPEAV12@PEBGPEAUHINSTANCE__@@@Z)";
#else
            R"(?CreateString@Value@DirectUI@@SGPAV12@PBGPAUHINSTANCE__@@@Z)";
#endif
        FARPROC pDirectUI_CreateString =
            GetProcAddress(duiModule, DirectUI_CreateString_Name);

        if (pDirectUI_CreateString) {
            Wh_SetFunctionHook((void*)pDirectUI_CreateString,
                               (void*)DirectUI_CreateString_Hook,
                               (void**)&DirectUI_CreateString_Original);
        } else {
            Wh_Log(L"Couldn't find DirectUI::Value::CreateString");
        }
    } else {
        Wh_Log(L"Couldn't load dui70.dll");
    }

    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L">");

    FreeAndClearRedirectedModules();

    HWND clearCachePromptWindow = g_clearCachePromptWindow;
    if (clearCachePromptWindow) {
        PostMessage(clearCachePromptWindow, WM_CLOSE, 0, 0);
    }

    if (g_clearCachePromptThread) {
        WaitForSingleObject(g_clearCachePromptThread, INFINITE);
        CloseHandle(g_clearCachePromptThread);
        g_clearCachePromptThread = nullptr;
    }

    if (DoesCurrentProcessOwnTaskbar()) {
        // Let other processes some time to unload the mod.
        Sleep(400);

        // Invalidate icon cache.
        SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, nullptr, nullptr);
    }
}

BOOL Wh_ModSettingsChanged(BOOL* bReload) {
    Wh_Log(L">");

    auto prevIconTheme = std::move(g_settings.iconTheme);
    int prevAllResourceRedirect = g_settings.allResourceRedirect;

    LoadSettings();

    if (g_settings.allResourceRedirect != prevAllResourceRedirect) {
        *bReload = TRUE;
        return TRUE;
    }

    FreeAndClearRedirectedModules();

    if (DoesCurrentProcessOwnTaskbar()) {
        if (wcscmp(g_settings.iconTheme, prevIconTheme) != 0) {
            PromptToClearCache();
        }

        // Let other processes some time to load the new config.
        Sleep(400);

        // Invalidate icon cache.
        SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, nullptr, nullptr);
    }

    return TRUE;
}
