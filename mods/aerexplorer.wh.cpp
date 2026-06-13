// ==WindhawkMod==
// @id              aerexplorer
// @name            Aerexplorer
// @description     Various tweaks for Windows Explorer to make it more like older versions.
// @version         1.8.2
// @author          aubymori
// @github          https://github.com/aubymori
// @include         *
// @compilerOptions -lgdi32 -lcomctl32 -lole32 -loleaut32 -luxtheme -ldwmapi -lversion -DWINVER=0x0A00
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Aerexplorer
This mod includes various tweaks for the Windows Explorer to look and behave
more like its Windows Vista, 7, 8, or older Windows 10 counterpart.

# IMPORTANT: READ!
Windhawk needs to hook into `svchost.exe` to successfully capture the creation of Explorer windows.
Please navigate to Windhawk's Settings, Advanced settings, More advanced settings, and make sure that
`svchost.exe` is in the Process inclusion list.

# Showcase

**Configured like Windows Vista**

![Windows Vista](https://raw.githubusercontent.com/aubymori/images/main/aerexplorer-vista.png)

**Configured like Windows 7**

![Windows 7](https://raw.githubusercontent.com/aubymori/images/main/aerexplorer-7.png)

**Configured like Windows 8.0**

![Windows 8.0](https://raw.githubusercontent.com/aubymori/images/main/aerexplorer-8.png)

**Configured like Windows 10 RTM**

![Windows 10 RTM](https://raw.githubusercontent.com/aubymori/images/main/aerexplorer-10.png)

# Common issues

**Control Panel lost its styling.**

OldNewExplorer allows for modification of UIFILE resources located in shell32.dll through the theme's
shellstyle.dll. You can leave OldNewExplorer installed with all of its options disabled, or, if you're
on 1903 or greater version, look into porting the modified UIFILEs from your shellstyle.dll to shell32.dll.mun.
The former is generally easier, and works just fine, though.

**The user folders keep reappearing under This PC on the navigation pane.**

This is an issue with the way the navigation pane works. Since the user folders are under This PC in the path,
it will try to correct itself by adding them under This PC on the navigation pane. If you use
[this registry hack](https://winclassic.net/thread/2013/moving-user-folders-back-pc), they will be moved back
under your user folder, and stop reappearing under This PC on the navigation pane.

# Support
This mod is primarily tested on Windows 10 Vibranium (which includes 2004, 20H2,
21H1, 21H2, and 22H2), but it has been confirmed to work to a varying degree on:

- Redstone 4 (1803)
- Redstone 5 (1809)
- Vanadium (1909)
- Iron (Server 2022)

Additionally, you can get it working on newer versions such as Windows 11 by replacing
ExplorerFrame.dll with that from build 21332, but it is not supported and not recommended.

Feel free to try it on other versions, but it may not work.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- alwayscpl: true
  $name: Seamlessly switch to Control Panel
  $description: Allow Explorer to switch to Control Panel without closing and reopening. Will break certain pages with dark mode.
- smalladdress: true
  $name: Small address bar
  $description: Reverts the address bar to the size it was before Windows 10, version 1909.
- tbst: seven
  $name: Address toolbar sizing
  $description: Determines the sizing of the buttons to the right of the address bar.
  $options:
  - default: Default
  - seven: Windows Vista/7/8
  - ten: Windows 10 (1507-1709)
  - tennew: Windows 10 (1803-1903)
  - custom: Custom
- dropdownwidth: 19
  $name: Dropdown icon width
  $description: Dropdown icon width in address bar. Address toolbar sizing must be set to "Custom".
- refreshwidth: 25
  $name: Refresh icon width
  $description: Refresh icon width in address bar. Address toolbar sizing must be set to "Custom".
- oldsearch: true
  $name: Old search box
  $description: Disable the EdgeHTML-based search box and use the old one instead. You must enable this option to fix the bug where the placeholder disappears.
- noribbon: true
  $name: Disable ribbon
  $description: Disable the ribbon and use the command bar from Windows 7 instead.
- listview: false
  $name: Use native list view
  $description: Use the native list view control instead of the DirectUI ItemsView, like Windows Vista and before. This will break with dark mode enabled.
- colheaders: false
  $name: Always show column headers
  $description: Always show the column headers that normally only show in the Details view, like Windows Vista.
- npst: seven
  $name: Navigation pane style
  $description: Determines the style of the tree view on the left of the window.
  $options:
  - default: Default
  - vista: Windows Vista
  - seven: Windows 7/8
- nopins: true
  $name: Remove pins from Quick access
  $description: Remove the pin icon that appears on the right of pinned items in Quick access on the navigation pane
- navbarglass: true
  $name: Glass on navigation bar
  $description: Show a glass effect under the navigation bar, like Windows Vista and 7. Enabling this option will also fix the bug where Navbar classes in themes are swapped.
- nocomposition: false
  $name: Disable composition
  $description: Use the basic theme "glass" effect when "Glass on navigation bar" is enabled.
- hidetitle: true
  $name: Hide window title and icon
  $description: Hide the window title and icon on Explorer windows like Windows 7 and before. This will not apply to open/save dialogs, and you must re-open any Explorer windows for it to take effect.
- noup: true
  $name: No up button
  $description: Remove the "Up" button like Windows Vista and 7.
- aerotravel: true
  $name: Aero travel buttons
  $description: Restores the Windows Vista/7 Aero-styled back and forward buttons.
- detailspane: true
  $name: Details pane on bottom
  $description: Put the details pane on bottom, like Windows Vista and 7. You can hide the status bar natively through the View tab in the Folder Options dialog.
- classicgrouping: true
  $name: Classic drive grouping
  $description: Split drives and devices into separate groups (Hard Disk Drives, Devices with Removable Storage, etc.) like Windows 8.0 and before.
- nopcfolders: true
  $name: Remove This PC folders
  $description: Remove the folders from the This PC folder, like Windows 8.0 and before.
- vistasearchplaceholder: false
  $name: Windows Vista search box placeholder
  $description: Makes the search box always say just "Search", like it did in Windows Vista.
- beta8navbarbg: false
  $name: Windows 8 beta navigation bar background
  $description: Changes the background of the navigation bar to match Windows 8 betas.
- leftviews: false
  $name: Views dropdown on left
  $description: Moves the views dropdown to the left after the "Organize" button, like Windows Vista.
- cmdbaricons: false
  $name: Command bar icons
  $description: Shows icons on all command bar items, like Windows Vista.
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>
#include <initguid.h>
#include <propvarutil.h>
#include <shobjidl.h>
#include <shlguid.h>
#include <shlobj.h>
#include <dwmapi.h>
#include <vssym32.h>
#include <vector>

/* Defines */
#ifdef _WIN64
#   define THISCALL  __cdecl
#   define STHISCALL L"__cdecl"

#   define STDCALL  __cdecl
#   define SSTDCALL L"__cdecl"
#else
#   define THISCALL  __thiscall
#   define STHISCALL L"__thiscall"

#   define STDCALL  __stdcall
#   define SSTDCALL L"__stdcall"
#endif

typedef enum _TOOLBARSIZETYPE
{
    TBST_DEFAULT = 0,
    TBST_SEVEN,
    TBST_TEN,
    TBST_TENNEW,
    TBST_CUSTOM
} TOOLBARSIZETYPE;

typedef enum _NAVPANESTYLETYPE
{
    NPST_DEFAULT = 0,
    NPST_VISTA,
    NPST_SEVEN
} NAVPANESTYLETYPE;

/* Variables */
struct
{
    bool             alwayscpl;
    bool             smalladdress;
    TOOLBARSIZETYPE  tbst;
    int              dropdownwidth;
    int              refreshwidth;
    bool             oldsearch;
    bool             noribbon;
    bool             listview;
    bool             colheaders;
    NAVPANESTYLETYPE npst;
    bool             nopins;
    bool             navbarglass;
    bool             hidetitle;
    bool             nocomposition;
    bool             noup;
    bool             aerotravel;
#ifdef _WIN64
    bool             detailspane;
#endif
    bool             classicgrouping;
    bool             nopcfolders;
    bool             vistasearchplaceholder;
    bool             beta8navbarbg;
    bool             leftviews;
    bool             cmdbaricons;
} settings = { 0 };

typedef HRESULT (WINAPI *VariantToBuffer_t)(LPVARIANT, void *, UINT);
VariantToBuffer_t VariantToBuffer = nullptr;

typedef enum FEATURE_ENABLED_STATE
{
    FEATURE_ENABLED_STATE_DEFAULT = 0,
    FEATURE_ENABLED_STATE_DISABLED = 1,
    FEATURE_ENABLED_STATE_ENABLED = 2
} FEATURE_ENABLED_STATE;

#pragma pack(push, 1)
struct RTL_FEATURE_CONFIGURATION {
    unsigned int featureId;
    unsigned __int32 group : 4;
    FEATURE_ENABLED_STATE enabledState : 2;
    unsigned __int32 enabledStateOptions : 1;
    unsigned __int32 unused1 : 1;
    unsigned __int32 variant : 6;
    unsigned __int32 variantPayloadKind : 2;
    unsigned __int32 unused2 : 16;
    unsigned int payload;
};
#pragma pack(pop)

typedef int (NTAPI *RtlQueryFeatureConfiguration_t)(UINT32, int, INT64 *, RTL_FEATURE_CONFIGURATION *);
RtlQueryFeatureConfiguration_t RtlQueryFeatureConfiguration = nullptr;

#ifdef _WIN64
#pragma region "Details pane UIFILE replacements"
LPCWSTR UIFILE_3 =
L"<duixml>\n"
L"<Element resid=\"FolderLayout\" layout=\"shellborderlayout()\" sheet=\"genericlayoutstyle\">\n"
L"<StatusBarModule ModuleID=\"StatusBarModule\" layoutpos=\"bottom\" layout=\"filllayout()\"/>\n"
L"<TemplateBackground id=\"atom(PreviewContainer)\" height=\"53rp\" layoutpos=\"bottom\" layout=\"filllayout()\">\n"
L"<TemplateBackground id=\"atom(PreviewShineLayer)\"/>\n"
L"<PreviewBackground id=\"atom(BackgroundClear)\" background=\"ARGB(0, 0, 0, 0)\" layout=\"borderlayout()\">\n"
L"<PreviewThumbnail ModuleID=\"PreviewThumbnailModule\" MaxThumbSize=\"256\" layoutpos=\"Left\"/>\n"
L"<Element layoutpos=\"Client\" layout=\"filllayout()\">\n"
L"<PreviewMetadata ModuleID=\"PreviewMetadataModule\" NullSelectPropertyString=\"prop:*System.OfflineStatus;*System.OfflineAvailability\" layoutpos=\"Client\"/>\n"
L"</Element>\n"
L"</PreviewBackground>\n"
L"<Sizer id=\"atom(PreviewPaneSizer)\" sizingtarget=\"atom(PreviewContainer)\" FramePersistType=\"2\" SlidesUpAndDown=\"true\" layoutpos=\"top\" DownOrRightGrowsTarget=\"false\"/>\n"
L"</TemplateBackground>\n"
L"<TemplateBackground id=\"atom(ReadingPane)\" layout=\"filllayout()\" layoutpos=\"right\">\n"
L"<PreviewThumbnail ModuleID=\"ReadingPaneThumbnailModule\" HideProperty=\"PreviewHandlerRunning\" Vertical=\"true\" layoutpos=\"client\" background=\"ARGB(0, 0, 0, 0)\"/>\n"
L"<ReadingPaneModule ModuleID=\"ReadingPaneModule\" layoutpos=\"none\"/>\n"
L"</TemplateBackground>\n"
L"<Sizer id=\"atom(ReadingPaneSizer)\" sizingtarget=\"atom(ReadingPane)\" GrowTargetFirst=\"true\" SlidesUpAndDown=\"false\" PrioritySizer=\"atom(PageSpaceControlSizer)\" DownOrRightGrowsTarget=\"false\" FramePersistType=\"2\" layoutpos=\"Right\"/>\n"
L"<ProperTreeModule id=\"atom(ProperTree)\" ModuleID=\"ProperTreeModule\" sheet=\"documentslayoutstyle\" layoutpos=\"Left\"/>\n"
L"<Element id=\"atom(ViewHostContainer)\" layoutpos=\"Client\" sheet=\"documentslayoutstyle\" layout=\"borderlayout()\">\n"
L"<ViewHost id=\"atom(clientviewhost)\" layout=\"borderlayout()\" layoutpos=\"client\">\n"
L"</ViewHost>\n"
L"</Element>\n"
L"</Element>\n"
L"</duixml>\n"
L"\n";

LPCWSTR UIFILE_4 =
L"<duixml>\n"
L"<Element resid=\"FolderLayout\" layout=\"shellborderlayout()\" sheet=\"musiclayoutstyle\">\n"
L"<StatusBarModule ModuleID=\"StatusBarModule\" layoutpos=\"bottom\" layout=\"filllayout()\"/>\n"
L"<TemplateBackground id=\"atom(PreviewContainer)\" height=\"53rp\" layoutpos=\"bottom\" layout=\"filllayout()\">\n"
L"<TemplateBackground id=\"atom(PreviewShineLayer)\"/>\n"
L"<PreviewBackground id=\"atom(BackgroundClear)\" background=\"ARGB(0, 0, 0, 0)\" layout=\"borderlayout()\">\n"
L"<PreviewThumbnail ModuleID=\"PreviewThumbnailModule\" MaxThumbSize=\"256\" layoutpos=\"Left\"/>\n"
L"<Element layoutpos=\"Client\" layout=\"filllayout()\">\n"
L"<PreviewMetadata ModuleID=\"PreviewMetadataModule\" NullSelectPropertyString=\"prop:*System.OfflineStatus;*System.OfflineAvailability\" layoutpos=\"Client\"/>\n"
L"</Element>\n"
L"</PreviewBackground>\n"
L"<Sizer id=\"atom(PreviewPaneSizer)\" sizingtarget=\"atom(PreviewContainer)\" FramePersistType=\"2\" SlidesUpAndDown=\"true\" layoutpos=\"top\" DownOrRightGrowsTarget=\"false\"/>\n"
L"</TemplateBackground>\n"
L"<TemplateBackground id=\"atom(ReadingPane)\" layout=\"filllayout()\" layoutpos=\"right\">\n"
L"<PreviewThumbnail ModuleID=\"ReadingPaneThumbnailModule\" HideProperty=\"PreviewHandlerRunning\" Vertical=\"true\" layoutpos=\"client\" background=\"ARGB(0, 0, 0, 0)\"/>\n"
L"<ReadingPaneModule ModuleID=\"ReadingPaneModule\" layoutpos=\"none\"/>\n"
L"</TemplateBackground>\n"
L"<Sizer id=\"atom(ReadingPaneSizer)\" sizingtarget=\"atom(ReadingPane)\" GrowTargetFirst=\"true\" SlidesUpAndDown=\"false\" PrioritySizer=\"atom(PageSpaceControlSizer)\" DownOrRightGrowsTarget=\"false\" FramePersistType=\"2\" layoutpos=\"Right\"/>\n"
L"<ProperTreeModule id=\"atom(ProperTree)\" ModuleID=\"ProperTreeModule\" sheet=\"musiclayoutstyle\" layoutpos=\"Left\"/>\n"
L"<Element id=\"atom(ViewHostContainer)\" layoutpos=\"Client\" sheet=\"documentslayoutstyle\" layout=\"borderlayout()\">\n"
L"<ViewHost id=\"atom(clientviewhost)\" layout=\"borderlayout()\" layoutpos=\"client\">\n"
L"</ViewHost>\n"
L"</Element>\n"
L"</Element>\n"
L"</duixml>\n"
L"\n";

LPCWSTR UIFILE_5 =
L"<duixml>\n"
L"<Element resid=\"FolderLayout\" layout=\"shellborderlayout()\" sheet=\"photolayoutstyle\">\n"
L"<StatusBarModule ModuleID=\"StatusBarModule\" layoutpos=\"bottom\" layout=\"filllayout()\"/>\n"
L"<TemplateBackground id=\"atom(PreviewContainer)\" height=\"53rp\" layoutpos=\"bottom\" layout=\"filllayout()\">\n"
L"<TemplateBackground id=\"atom(PreviewShineLayer)\"/>\n"
L"<PreviewBackground id=\"atom(BackgroundClear)\" background=\"ARGB(0, 0, 0, 0)\" layout=\"borderlayout()\">\n"
L"<PreviewThumbnail ModuleID=\"PreviewThumbnailModule\" MaxThumbSize=\"256\" layoutpos=\"Left\"/>\n"
L"<Element layoutpos=\"Client\" layout=\"filllayout()\">\n"
L"<PreviewMetadata ModuleID=\"PreviewMetadataModule\" NullSelectPropertyString=\"prop:*System.OfflineStatus;*System.OfflineAvailability\" layoutpos=\"Client\"/>\n"
L"</Element>\n"
L"</PreviewBackground>\n"
L"<Sizer id=\"atom(PreviewPaneSizer)\" sizingtarget=\"atom(PreviewContainer)\" FramePersistType=\"2\" SlidesUpAndDown=\"true\" layoutpos=\"top\" DownOrRightGrowsTarget=\"false\"/>\n"
L"</TemplateBackground>\n"
L"<TemplateBackground id=\"atom(ReadingPane)\" layout=\"filllayout()\" layoutpos=\"right\">\n"
L"<PreviewThumbnail ModuleID=\"ReadingPaneThumbnailModule\" HideProperty=\"PreviewHandlerRunning\" Vertical=\"true\" layoutpos=\"client\" background=\"ARGB(0, 0, 0, 0)\"/>\n"
L"<ReadingPaneModule ModuleID=\"ReadingPaneModule\" layoutpos=\"none\"/>\n"
L"</TemplateBackground>\n"
L"<Sizer id=\"atom(ReadingPaneSizer)\" sizingtarget=\"atom(ReadingPane)\" GrowTargetFirst=\"true\" SlidesUpAndDown=\"false\" PrioritySizer=\"atom(PageSpaceControlSizer)\" DownOrRightGrowsTarget=\"false\" FramePersistType=\"2\" layoutpos=\"Right\"/>\n"
L"<ProperTreeModule id=\"atom(ProperTree)\" ModuleID=\"ProperTreeModule\" sheet=\"photolayoutstyle\" layoutpos=\"Left\"/>\n"
L"<Element id=\"atom(ViewHostContainer)\" layoutpos=\"Client\" sheet=\"documentslayoutstyle\" layout=\"borderlayout()\">\n"
L"<ViewHost id=\"atom(clientviewhost)\" layout=\"borderlayout()\" layoutpos=\"client\">\n"
L"</ViewHost>\n"
L"</Element>\n"
L"</Element>\n"
L"</duixml>\n"
L"\n";

LPCWSTR UIFILE_6 =
L"<duixml>\n"
L"<Element resid=\"FolderLayout\" layout=\"shellborderlayout()\" sheet=\"documentslayoutstyle\">\n"
L"<StatusBarModule ModuleID=\"StatusBarModule\" layoutpos=\"bottom\" layout=\"filllayout()\"/>\n"
L"<TemplateBackground id=\"atom(PreviewContainer)\" height=\"90rp\" layoutpos=\"bottom\" layout=\"filllayout()\">\n"
L"<TemplateBackground id=\"atom(PreviewShineLayer)\"/>\n"
L"<PreviewBackground id=\"atom(BackgroundClear)\" background=\"ARGB(0, 0, 0, 0)\" layout=\"borderlayout()\">\n"
L"<PreviewThumbnail ModuleID=\"PreviewThumbnailModule\" MaxThumbSize=\"256\" layoutpos=\"Left\"/>\n"
L"<Element layoutpos=\"Client\" layout=\"filllayout()\">\n"
L"<PreviewMetadata ModuleID=\"PreviewMetadataModule\" NullSelectPropertyString=\"prop:*System.OfflineStatus;*System.OfflineAvailability\" layoutpos=\"Client\"/>\n"
L"</Element>\n"
L"</PreviewBackground>\n"
L"<Sizer id=\"atom(PreviewPaneSizer)\" sizingtarget=\"atom(PreviewContainer)\" FramePersistType=\"2\" SlidesUpAndDown=\"true\" layoutpos=\"top\" DownOrRightGrowsTarget=\"false\"/>\n"
L"</TemplateBackground>\n"
L"<TemplateBackground id=\"atom(ReadingPane)\" layout=\"filllayout()\" layoutpos=\"right\">\n"
L"<PreviewThumbnail ModuleID=\"ReadingPaneThumbnailModule\" HideProperty=\"PreviewHandlerRunning\" Vertical=\"true\" layoutpos=\"client\" background=\"ARGB(0, 0, 0, 0)\"/>\n"
L"<ReadingPaneModule ModuleID=\"ReadingPaneModule\" layoutpos=\"none\"/>\n"
L"</TemplateBackground>\n"
L"<Sizer id=\"atom(ReadingPaneSizer)\" sizingtarget=\"atom(ReadingPane)\" GrowTargetFirst=\"true\" SlidesUpAndDown=\"false\" PrioritySizer=\"atom(PageSpaceControlSizer)\" DownOrRightGrowsTarget=\"false\" FramePersistType=\"2\" layoutpos=\"Right\"/>\n"
L"<ProperTreeModule id=\"atom(ProperTree)\" ModuleID=\"ProperTreeModule\" sheet=\"documentslayoutstyle\" layoutpos=\"Left\"/>\n"
L"<Element id=\"atom(ViewHostContainer)\" layoutpos=\"Client\" sheet=\"documentslayoutstyle\" layout=\"borderlayout()\">\n"
L"<ViewHost id=\"atom(clientviewhost)\" layout=\"borderlayout()\" layoutpos=\"client\">\n"
L"</ViewHost>\n"
L"</Element>\n"
L"</Element>\n"
L"</duixml>\n"
L"\n";

LPCWSTR UIFILE_19 =
L"<duixml>\n"
L"<Element resid=\"FolderLayout\" layout=\"shellborderlayout()\" sheet=\"documentslayoutstyle\">\n"
L"<StatusBarModule ModuleID=\"StatusBarModule\" layoutpos=\"bottom\" layout=\"filllayout()\"/>\n"
L"<TemplateBackground id=\"atom(PreviewContainer)\" height=\"53rp\" layoutpos=\"bottom\" layout=\"filllayout()\">\n"
L"<TemplateBackground id=\"atom(PreviewShineLayer)\"/>\n"
L"<PreviewBackground id=\"atom(BackgroundClear)\" background=\"ARGB(0, 0, 0, 0)\" layout=\"borderlayout()\">\n"
L"<PreviewThumbnail ModuleID=\"PreviewThumbnailModule\" MaxThumbSize=\"256\" layoutpos=\"Left\"/>\n"
L"<Element layoutpos=\"Client\" layout=\"filllayout()\">\n"
L"<PreviewMetadata ModuleID=\"PreviewMetadataModule\" NullSelectPropertyString=\"prop:*System.OfflineStatus;*System.OfflineAvailability\" layoutpos=\"Client\"/>\n"
L"</Element>\n"
L"</PreviewBackground>\n"
L"<Sizer id=\"atom(PreviewPaneSizer)\" sizingtarget=\"atom(PreviewContainer)\" FramePersistType=\"2\" SlidesUpAndDown=\"true\" layoutpos=\"top\" DownOrRightGrowsTarget=\"false\"/>\n"
L"</TemplateBackground>\n"
L"<TemplateBackground id=\"atom(ReadingPane)\" layout=\"filllayout()\" layoutpos=\"right\">\n"
L"<PreviewThumbnail ModuleID=\"ReadingPaneThumbnailModule\" HideProperty=\"PreviewHandlerRunning\" Vertical=\"true\" layoutpos=\"client\"/>\n"
L"<ReadingPaneModule ModuleID=\"ReadingPaneModule\" layoutpos=\"none\"/>\n"
L"</TemplateBackground>\n"
L"<Sizer id=\"atom(ReadingPaneSizer)\" sizingtarget=\"atom(ReadingPane)\" GrowTargetFirst=\"true\" SlidesUpAndDown=\"false\" PrioritySizer=\"atom(PageSpaceControlSizer)\" DownOrRightGrowsTarget=\"false\" FramePersistType=\"2\" layoutpos=\"Right\"/>\n"
L"<ProperTreeModule id=\"atom(ProperTree)\" ModuleID=\"ProperTreeModule\" sheet=\"documentslayoutstyle\" layoutpos=\"Left\"/>\n"
L"<Element id=\"atom(ViewHostContainer)\" layoutpos=\"Client\" sheet=\"documentslayoutstyle\" layout=\"borderlayout()\">\n"
L"<ViewHost id=\"atom(clientviewhost)\" layout=\"borderlayout()\" layoutpos=\"client\">\n"
L"</ViewHost>\n"
L"</Element>\n"
L"</Element>\n"
L"</duixml>\n"
L"\n";

LPCWSTR UIFILE_20 =
L"<duixml>\n"
L"<Element resid=\"FolderLayout\" layout=\"shellborderlayout()\" sheet=\"documentslayoutstyle\">\n"
L"<StatusBarModule ModuleID=\"StatusBarModule\" layoutpos=\"bottom\" layout=\"filllayout()\"/>\n"
L"<TemplateBackground id=\"atom(PreviewContainer)\" height=\"53rp\" layoutpos=\"bottom\" layout=\"filllayout()\">\n"
L"<TemplateBackground id=\"atom(PreviewShineLayer)\"/>\n"
L"<PreviewBackground id=\"atom(BackgroundClear)\" background=\"ARGB(0, 0, 0, 0)\" layout=\"borderlayout()\">\n"
L"<PreviewThumbnail ModuleID=\"PreviewThumbnailModule\" MaxThumbSize=\"256\" layoutpos=\"Left\"/>\n"
L"<Element layoutpos=\"Client\" layout=\"filllayout()\">\n"
L"<PreviewMetadata ModuleID=\"PreviewMetadataModule\" NullSelectPropertyString=\"prop:*System.OfflineStatus;*System.OfflineAvailability\" layoutpos=\"Client\"/>\n"
L"</Element>\n"
L"</PreviewBackground>\n"
L"<Sizer id=\"atom(PreviewPaneSizer)\" sizingtarget=\"atom(PreviewContainer)\" FramePersistType=\"2\" SlidesUpAndDown=\"true\" layoutpos=\"top\" DownOrRightGrowsTarget=\"false\"/>\n"
L"</TemplateBackground>\n"
L"<TemplateBackground id=\"atom(ReadingPane)\" layout=\"filllayout()\" layoutpos=\"right\">\n"
L"<PreviewThumbnail ModuleID=\"ReadingPaneThumbnailModule\" HideProperty=\"PreviewHandlerRunning\" Vertical=\"true\" layoutpos=\"client\" background=\"ARGB(0, 0, 0, 0)\"/>\n"
L"<ReadingPaneModule ModuleID=\"ReadingPaneModule\" layoutpos=\"none\"/>\n"
L"</TemplateBackground>\n"
L"<Sizer id=\"atom(ReadingPaneSizer)\" sizingtarget=\"atom(ReadingPane)\" GrowTargetFirst=\"true\" SlidesUpAndDown=\"false\" PrioritySizer=\"atom(PageSpaceControlSizer)\" DownOrRightGrowsTarget=\"false\" FramePersistType=\"2\" layoutpos=\"Right\"/>\n"
L"<ProperTreeModule id=\"atom(ProperTree)\" ModuleID=\"ProperTreeModule\" sheet=\"documentslayoutstyle\" layoutpos=\"Left\"/>\n"
L"<Element id=\"atom(ViewHostContainer)\" layoutpos=\"Client\" sheet=\"documentslayoutstyle\" layout=\"borderlayout()\">\n"
L"<ViewHost id=\"atom(clientviewhost)\" layout=\"borderlayout()\" layoutpos=\"client\">\n"
L"</ViewHost>\n"
L"</Element>\n"
L"</Element>\n"
L"</duixml>\n"
L"\n";

LPCWSTR UIFILE_21 =
L"<duixml>\n"
L"<Element resid=\"FolderLayout\" layout=\"shellborderlayout()\" sheet=\"genericlayoutstyle\">\n"
L"<StatusBarModule ModuleID=\"StatusBarModule\" layoutpos=\"bottom\" layout=\"filllayout()\"/>\n"
L"<TemplateBackground id=\"atom(PreviewContainer)\" height=\"53rp\" layoutpos=\"bottom\" layout=\"filllayout()\">\n"
L"<TemplateBackground id=\"atom(PreviewShineLayer)\"/>\n"
L"<PreviewBackground id=\"atom(BackgroundClear)\" background=\"ARGB(0, 0, 0, 0)\" layout=\"borderlayout()\">\n"
L"<PreviewThumbnail ModuleID=\"PreviewThumbnailModule\" MaxThumbSize=\"256\" layoutpos=\"Left\"/>\n"
L"<Element layoutpos=\"Client\" layout=\"filllayout()\">\n"
L"<PreviewMetadata ModuleID=\"PreviewMetadataModule\" NullSelectTitlePropertyString=\"prop:System.Computer.SimpleName;*System.Computer.Description\" NullSelectPropertyString=\"prop:*System.Computer.DomainName;*System.Computer.Workgroup;*System.Computer.Processor;System.Computer.Memory\" layoutpos=\"Client\"/>\n"
L"</Element>\n"
L"</PreviewBackground>\n"
L"<Sizer id=\"atom(PreviewPaneSizer)\" sizingtarget=\"atom(PreviewContainer)\" SlidesUpAndDown=\"true\" layoutpos=\"top\" DownOrRightGrowsTarget=\"false\"/>\n"
L"</TemplateBackground>\n"
L"<TemplateBackground id=\"atom(ReadingPane)\" layout=\"filllayout()\" layoutpos=\"right\">\n"
L"<PreviewThumbnail ModuleID=\"ReadingPaneThumbnailModule\" HideProperty=\"PreviewHandlerRunning\" Vertical=\"true\" layoutpos=\"client\" background=\"ARGB(0, 0, 0, 0)\"/>\n"
L"<ReadingPaneModule ModuleID=\"ReadingPaneModule\" layoutpos=\"none\"/>\n"
L"</TemplateBackground>\n"
L"<Sizer id=\"atom(ReadingPaneSizer)\" sizingtarget=\"atom(ReadingPane)\" GrowTargetFirst=\"true\" SlidesUpAndDown=\"false\" PrioritySizer=\"atom(PageSpaceControlSizer)\" DownOrRightGrowsTarget=\"false\" FramePersistType=\"2\" layoutpos=\"Right\"/>\n"
L"<ProperTreeModule id=\"atom(ProperTree)\" ModuleID=\"ProperTreeModule\" sheet=\"documentslayoutstyle\" layoutpos=\"Left\" RootMgrClsid=\"{93319CCC-B277-48FF-95BD-6CDCCAFCBD31}\"/>\n"
L"<Element id=\"atom(ViewHostContainer)\" layoutpos=\"Client\" sheet=\"genericlayoutstyle\" layout=\"filllayout()\">\n"
L"<ViewHost id=\"atom(clientviewhost)\" layout=\"borderlayout()\">\n"
L"</ViewHost>\n"
L"</Element>\n"
L"</Element>\n"
L"</duixml>\n"
L"\n";
#pragma endregion // "Details pane UIFILE replacements"
#endif

typedef struct _DRVGRPSTRINGS
{
    LANGID  langid;
    LPCWSTR pszHardDisks;
    LPCWSTR pszRemovable;
    LPCWSTR pszOther;
    LPCWSTR pszScanners;
    LPCWSTR pszPortableMedia;
    LPCWSTR pszPortable;
} DRVGRPSTRINGS;

typedef const DRVGRPSTRINGS *LPCDRVGRPSTRINGS;

typedef enum _DRVGRP
{
    DG_HARDDISKS = 0,
    DG_REMOVABLE,
    DG_OTHER,
    DG_SCANNERS,
    DG_PORTABLEMEDIA,
    DG_PORTABLE
} DRVGRP;

#pragma region "Drive grouping localization"
constexpr DRVGRPSTRINGS c_rgDriveGroupStrings[] = {
    {
        MAKELANGID(LANG_ARABIC, SUBLANG_ARABIC_SAUDI_ARABIA),
        L"محركات الأقراص الثابتة",
        L"أجهزة ذات وحدة تخزين قابلة للنقل",
        L"أخرى",
        L"الماسحات الضوئية والكاميرات",
        L"أجهزة الوسائط المحمولة",
        L"الأجهزة المحمولة"
    },
    {
        MAKELANGID(LANG_BULGARIAN, SUBLANG_DEFAULT),
        L"Твърди дискови устройства",
        L"Устройства със сменяеми носители",
        L"Друго",
        L"Скенери и фотоапарати",
        L"Преносими мултимедийни устройства",
        L"Преносими устройства"
    },
    {
        MAKELANGID(LANG_CHINESE, SUBLANG_CHINESE_TRADITIONAL),
        L"硬碟",
        L"裝置中含有卸除式存放裝置",
        L"其他",
        L"掃描器與數位相機",
        L"可攜式媒體裝置",
        L"可攜式裝置"
    },
    {
        MAKELANGID(LANG_CZECH, SUBLANG_DEFAULT),
        L"Jednotky pevných disků",
        L"Zařízení s vyměnitelným úložištěm",
        L"Jiná",
        L"Skenery a fotoaparáty",
        L"Portable Media Devices",
        L"Přenosná zařízení"
    },
    {
        MAKELANGID(LANG_DANISH, SUBLANG_DEFAULT),
        L"Harddiskdrev",
        L"Enheder med flytbare medier",
        L"Andet",
        L"Scannere og kameraer",
        L"Bærbare medieenheder",
        L"Bærbare enheder"
    },
    {
        MAKELANGID(LANG_GERMAN, SUBLANG_GERMAN),
        L"Festplatten",
        L"Geräte mit Wechselmedien",
        L"Weitere",
        L"Scanner und Kameras",
        L"Tragbare Mediengeräte",
        L"Tragbare Geräte"
    },
    {
        MAKELANGID(LANG_GREEK, SUBLANG_DEFAULT),
        L"Μονάδες σκληρών δίσκων",
        L"Συσκευές με αφαιρούμενα μέσα αποθήκευσης",
        L"Άλλα",
        L"Σαρωτές και κάμερες",
        L"Φορητές συσκευές πολυμέσων",
        L"Φορητές συσκευές"
    },
    {
        MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US),
        L"Hard Disk Drives",
        L"Devices with Removable Storage",
        L"Other",
        L"Scanners and Cameras",
        L"Portable Media Devices",
        L"Portable Devices"
    },
    {
        MAKELANGID(LANG_FINNISH, SUBLANG_DEFAULT),
        L"Kiintolevyasemat",
        L"Laitteet, joissa on siirrettävä tallennusväline",
        L"Muu",
        L"Skannerit ja kamerat",
        L"Kannettavat medialaitteet",
        L"Kannettavat laitteet"
    },
    {
        MAKELANGID(LANG_FRENCH, SUBLANG_FRENCH),
        L"Disques durs",
        L"Périphériques utilisant des supports de stockage amovibles",
        L"Autre",
        L"Scanneurs et appareils photo",
        L"Appareils mobiles multimédias",
        L"Périphériques amovibles"
    },
    {
        MAKELANGID(LANG_HEBREW, SUBLANG_DEFAULT),
        L"כוננים קשיחים",
        L"התקנים עם אחסון נשלף",
        L"אחר",
        L"סורקים ומצלמות",
        L"מכשירי מדיה ניידים",
        L"מכשירים ניידים"
    },
    {
        MAKELANGID(LANG_HUNGARIAN, SUBLANG_DEFAULT),
        L"Merevlemez-meghajtók",
        L"Cserélhető adathordozós eszközök",
        L"Egyéb",
        L"Képolvasók és fényképezőgépek",
        L"Hordozható lejátszóeszközök",
        L"Hordozható eszközök"
    },
    {
        MAKELANGID(LANG_ITALIAN, SUBLANG_ITALIAN),
        L"Unità disco rigido",
        L"Dispositivi con archivi rimovibili",
        L"Altro",
        L"Scanner e fotocamere digitali",
        L"Dispositivi audio/video mobili",
        L"Dispositivi portatili"
    },
    {
        MAKELANGID(LANG_JAPANESE, SUBLANG_DEFAULT),
        L"ハード ディスク ドライブ",
        L"リムーバブル記憶域があるデバイス",
        L"その他",
        L"スキャナーとカメラ",
        L"ポータブル メディア デバイス",
        L"ポータブル デバイス"
    },
    {
        MAKELANGID(LANG_KOREAN, SUBLANG_KOREAN),
        L"하드 디스크 드라이브",
        L"이동식 미디어 장치",
        L"기타",
        L"스캐너 및 카메라",
        L"휴대용 미디어 장치",
        L"휴대용 장치"
    },
    {
        MAKELANGID(LANG_DUTCH, SUBLANG_DUTCH),
        L"Hardeschijfstations",
        L"Apparaten met verwisselbare opslagmedia",
        L"Overige",
        L"Scanners en camera's",
        L"Draagbare media-apparaten",
        L"Draagbare apparaten"
    },
    {
        MAKELANGID(LANG_NORWEGIAN, SUBLANG_NORWEGIAN_BOKMAL),
        L"Harddiskstasjoner",
        L"Flyttbare lagringsmedier",
        L"Annet",
        L"Skannere og kameraer",
        L"Bærbare medieenheter",
        L"Bærbare enheter"
    },
    {
        MAKELANGID(LANG_POLISH, SUBLANG_DEFAULT),
        L"Dyski twarde",
        L"Urządzenia z wymiennymi nośnikami pamięci",
        L"Inne",
        L"Skanery i aparaty fotograficzne",
        L"Przenośne urządzenia multimedialne",
        L"Urządzenia przenośne"
    },
    {
        MAKELANGID(LANG_PORTUGUESE, SUBLANG_PORTUGUESE_BRAZILIAN),
        L"Unidades de Disco Rígido",
        L"Dispositivos com Armazenamento Removível",
        L"Outros",
        L"Scanners e Câmeras",
        L"Dispositivos de Mídia Portáteis",
        L"Dispositivos Portáteis"
    },
    {
        MAKELANGID(LANG_ROMANIAN, SUBLANG_DEFAULT),
        L"Unități de hard disk",
        L"Unități cu stocare detașabilă",
        L"Altul",
        L"Scanere și aparate foto",
        L"Dispozitive media portabile",
        L"Dispozitive portabile"
    },
    {
        MAKELANGID(LANG_RUSSIAN, SUBLANG_DEFAULT),
        L"Жесткие диски",
        L"Устройства со съемными носителями",
        L"Другие",
        L"Сканеры и камеры",
        L"Переносные устройства мультимедиа",
        L"Портативные устройства"
    },
    {
        MAKELANGID(LANG_CROATIAN, SUBLANG_DEFAULT),
        L"Pogoni tvrdih diskova",
        L"Uređaji s prijenosnom pohranom",
        L"Ostalo",
        L"Skeneri i kamere",
        L"Prijenosni medijski uređaji",
        L"Prijenosni uređaji"
    },
    {
        MAKELANGID(LANG_SLOVAK, SUBLANG_DEFAULT),
        L"Jednotky pevného disku",
        L"Zariadenia s vymeniteľným ukladacím priestorom",
        L"Iné",
        L"Skenery a fotoaparáty",
        L"Prenosné mediálne zariadenia",
        L"Prenosné zariadenia"
    },
    {
        MAKELANGID(LANG_SWEDISH, SUBLANG_SWEDISH),
        L"Hårddiskar",
        L"Enheter med flyttbara lagringsmedia",
        L"Annan",
        L"Skannrar och kameror",
        L"Bärbara medieenheter",
        L"Bärbara enheter"
    },
    {
        MAKELANGID(LANG_THAI, SUBLANG_DEFAULT),
        L"ฮาร์ดดิสก์ไดรฟ์",
        L"อุปกรณ์ที่มีที่เก็บข้อมูลแบบถอดได้",
        L"อื่นๆ",
        L"สแกนเนอร์และกล้อง",
        L"อุปกรณ์สื่อแบบพกพา",
        L"อุปกรณ์แบบพกพา"
    },
    {
        MAKELANGID(LANG_TURKISH, SUBLANG_DEFAULT),
        L"Sabit Disk Sürücüleri",
        L"Çıkarılabilir Depolama Birimi Olan Aygıtlar",
        L"Diğer",
        L"Tarayıcılar ve Kameralar",
        L"Taşınabilir Medya Aygıtları",
        L"Taşınabilir Aygıtlar"
    },
    {
        MAKELANGID(LANG_UKRAINIAN, SUBLANG_DEFAULT),
        L"Жорсткі диски",
        L"Пристрої зі знімними носіями",
        L"Інше",
        L"Сканери та камери",
        L"Портативні носії",
        L"Портативні пристрої"
    },
    {
        MAKELANGID(LANG_SLOVENIAN, SUBLANG_DEFAULT),
        L"Trdi diski",
        L"Naprave z izmenljivimi mediji",
        L"Drugo",
        L"Optični bralniki in fotoaparati",
        L"Prenosne predstavnostne naprave",
        L"Prenosne naprave"
    },
    {
        MAKELANGID(LANG_ESTONIAN, SUBLANG_DEFAULT),
        L"Kõvakettad",
        L"Irdsalvestiga seadmed",
        L"Muu",
        L"Skannerid ja kaamerad",
        L"Kantavad meediumiseadmed",
        L"Kandeseadmed"
    },
    {
        MAKELANGID(LANG_LATVIAN, SUBLANG_DEFAULT),
        L"Cietie diski",
        L"Ierīces ar noņemamu krātuvi",
        L"Citi",
        L"Skeneri un kameras",
        L"Portatīvās datu nesēju ierīces",
        L"Portatīvās ierīces"
    },
    {
        MAKELANGID(LANG_LITHUANIAN, SUBLANG_LITHUANIAN),
        L"Standieji diskai",
        L"Įrenginiai su keičiamąja laikmena",
        L"Kita",
        L"Skaitytuvai ir fotoaparatai",
        L"Nešiojamieji medijos įrenginiai",
        L"Nešiojamieji įrenginiai"
    },
    {
        MAKELANGID(LANG_CHINESE, SUBLANG_CHINESE_SIMPLIFIED),
        L"硬盘",
        L"有可移动存储的设备",
        L"其他",
        L"扫描仪和照相机",
        L"便携媒体设备",
        L"便携设备"
    },
    {
        MAKELANGID(LANG_PORTUGUESE, SUBLANG_PORTUGUESE),
        L"Unidades de Disco Rígido",
        L"Dispositivos com Armazenamento Amovível",
        L"Outro",
        L"Scanners e Câmaras",
        L"Dispositivos de Multimédia Portáteis",
        L"Dispositivos Portáteis"
    },
    {
        MAKELANGID(LANG_CROATIAN, 0x2),
        L"Jedinice čvrstog diska",
        L"Uređaji sa prenosivim skladištenjem",
        L"Ostalo",
        L"Skeneri i fotoaparati",
        L"Prenosni medijski uređaji",
        L"Prenosni uređaji"
    },
    {
        MAKELANGID(LANG_SPANISH, SUBLANG_SPANISH_MODERN),
        L"Unidades de disco duro",
        L"Dispositivos con almacenamiento extraíble",
        L"Otros",
        L"Escáneres y cámaras",
        L"Dispositivos multimedia portátiles",
        L"Dispositivos portátiles"
    },
};
#pragma endregion

BYTE ParseHexChar(WCHAR c)
{
    if (c >= L'0' && c <= L'9')
        return c - L'0';

    if (c >= 'A' && c <= 'F')
        return c - ('A' - 0xA);

    if (c >= 'a' && c <= 'f')
        return c - ('a' - 0xA);

    return 0;
}

LPCDRVGRPSTRINGS GetCurrentDriveLocale(void)
{
    ULONG ulNumLanguages;
    WCHAR szLanguages[MAX_PATH];
    DWORD cchLanguages = ARRAYSIZE(szLanguages);
    szLanguages[0] = L'\0';
    GetThreadPreferredUILanguages(MUI_LANGUAGE_ID, &ulNumLanguages, szLanguages, &cchLanguages);

    // Primary lang ID match
    LPCDRVGRPSTRINGS pdgsPrimaryMatch = nullptr;
    // Full lang ID match
    LPCDRVGRPSTRINGS pdgsFullMatch    = nullptr;

    constexpr UINT EN_US_INDEX = 7;
    static_assert(
        c_rgDriveGroupStrings[EN_US_INDEX].langid == MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US),
        "Incorrect index for en-US drive group strings"
    );

    LANGID langid = 0;
    for (ULONG i = 0; i < ulNumLanguages; i++)
    {
        LPWSTR pszLang = szLanguages + (i * 5);
        langid = ParseHexChar(pszLang[0]) << 12
            | ParseHexChar(pszLang[1]) << 8
            | ParseHexChar(pszLang[2]) << 4
            | ParseHexChar(pszLang[3]);

        if (langid == MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US))
            return &c_rgDriveGroupStrings[EN_US_INDEX];
        
        for (UINT i = 0; i < ARRAYSIZE(c_rgDriveGroupStrings); i++)
        {
            LPCDRVGRPSTRINGS pdgsCurrent = &c_rgDriveGroupStrings[i];

            if (PRIMARYLANGID(pdgsCurrent->langid) == PRIMARYLANGID(langid))
            {
                pdgsPrimaryMatch = pdgsCurrent;
            }

            if (pdgsCurrent->langid == langid)
            {
                pdgsFullMatch = pdgsCurrent;
                break;
            }
        }

        if (pdgsPrimaryMatch)
            break;
    }

    if (pdgsFullMatch)
        return pdgsFullMatch;
    if (pdgsPrimaryMatch)
        return pdgsPrimaryMatch;

    return &c_rgDriveGroupStrings[EN_US_INDEX];
}

/* Only available in Windows 10, version 1607 and later. */
WINUSERAPI UINT WINAPI GetDpiForSystem(void);
WINUSERAPI UINT WINAPI GetDpiForWindow(HWND);
WINUSERAPI int WINAPI GetSystemMetricsForDpi(int, UINT);

#define ScaleForDPI(n) MulDiv(n, GetDpiForSystem(), 96)
#define IsHighDPI() (GetDpiForSystem() >= 120)

#pragma region "Hooks"

#pragma region "ExplorerFrame.dll hooks"

#pragma region "Old search box"
/* Old search box */
bool (__fastcall *CUniversalSearchBand_IsModernSearchBoxEnabled_orig)(void *) = nullptr;
bool __fastcall CUniversalSearchBand_IsModernSearchBoxEnabled_hook(
    void *pThis
)
{
    return settings.oldsearch
    ? false
    : CUniversalSearchBand_IsModernSearchBoxEnabled_orig(
        pThis
    );
}

/* Fix placeholder blanking */
HRESULT (THISCALL *CSearchEditBox_HideSuggestions_orig)(void *) = nullptr;
HRESULT THISCALL CSearchEditBox_HideSuggestions_hook(
    void *pThis
)
{
    return settings.oldsearch
    ? S_OK
    : CSearchEditBox_HideSuggestions_orig(pThis);
}
#pragma endregion // "Old search box"

/* Windows Vista search box placeholder */
HRESULT (STDCALL *CSearchBox_SetCueAndTooltipText_orig)(void *, LPCWSTR, LPCWSTR) = nullptr;
HRESULT STDCALL CSearchBox_SetCueAndTooltipText_hook(
    void    *pThis,
    LPCWSTR  pszCueText,
    LPCWSTR  pszTooltipText
)
{
    if (settings.vistasearchplaceholder)
    {
        pszCueText = NULL;
    }
    return CSearchBox_SetCueAndTooltipText_orig(
        pThis, pszCueText, pszTooltipText
    );
}

#pragma region "Small address bar"

#define CAddressBand__hwnd(pThis) *((HWND *)pThis + 9)

#ifdef _WIN64
#   define CAddressBand__hwndTools(pThis) *((HWND *)pThis + 29)
#else
#   define CAddressBand__hwndTools(pThis) *((HWND *)pThis + 31)
#endif

/* Fix address bar position and toolbar size */
void (THISCALL *CAddressBand__PositionChildWindows_orig)(void *) = nullptr;
void THISCALL CAddressBand__PositionChildWindows_hook(
    void *pThis
)
{
    if (settings.tbst != TBST_DEFAULT)
    {
        HWND hwndTools = CAddressBand__hwndTools(pThis);
        if (hwndTools)
        {
            TBBUTTONINFOW tbi;
            tbi.cbSize = sizeof(TBBUTTONINFOW);
            tbi.dwMask = TBIF_SIZE;

            /* Spacer band that only gets added on classic. For some reason,
               it's huge by default. */
            tbi.cx = 2;
            SendMessageW(hwndTools, TB_SETBUTTONINFOW, 203, (LPARAM)&tbi);

            /* Dropdown width */
            switch (settings.tbst)
            {
                case TBST_SEVEN:
                    tbi.cx = ScaleForDPI(19);
                    break;
                case TBST_TEN:
                    tbi.cx = ScaleForDPI(20);
                    break;
                case TBST_TENNEW:
                    tbi.cx = ScaleForDPI(17);
                    break;
                case TBST_CUSTOM:
                    tbi.cx = ScaleForDPI(settings.dropdownwidth);
                    break;
            }
            SendMessageW(hwndTools, TB_SETBUTTONINFOW, 202, (LPARAM)&tbi);

            /* Refresh width */
            switch (settings.tbst)
            {
                case TBST_SEVEN:
                    tbi.cx = ScaleForDPI(25);
                    break;
                case TBST_TEN:
                    tbi.cx = ScaleForDPI(20);
                    break;
                case TBST_TENNEW:
                    tbi.cx = ScaleForDPI(17);
                    break;
                case TBST_CUSTOM:
                    tbi.cx = ScaleForDPI(settings.refreshwidth);
                    break;
            }

            /* Toolbar buttons 100-102 are go, stop, and refresh respectively */
            for (int i = 100; i <= 102; i++)
            {
                SendMessageW(hwndTools, TB_SETBUTTONINFOW, i, (LPARAM)&tbi);
            }
        }
    }

    return CAddressBand__PositionChildWindows_orig(pThis);
}

DWORD g_dwSubBuild = 0;

bool ModernSearchFeatureEnabled(void)
{
    /* Collect sub-build if it hasn't been selected */
    if (g_dwSubBuild == 0)
    {
        DWORD dwSize = sizeof(DWORD);
        RegGetValueW(
            HKEY_LOCAL_MACHINE,
            L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion",
            L"UBR",
            RRF_RT_DWORD,
            NULL,
            &g_dwSubBuild,
            &dwSize
        );
    }

    RTL_FEATURE_CONFIGURATION feature = { 0 };
    INT64 changeStamp = 0;
    if (SUCCEEDED(RtlQueryFeatureConfiguration(18755234, 1, &changeStamp, &feature)))
    {
        switch (feature.enabledState)
        {
            case FEATURE_ENABLED_STATE_DISABLED:
                return false;
            case FEATURE_ENABLED_STATE_ENABLED:
                return true;
            case FEATURE_ENABLED_STATE_DEFAULT:
                return (g_dwSubBuild < 3754);
        }
    }
    
    return (g_dwSubBuild < 3754);
}

LRESULT (THISCALL *CAddressBand__AddressBandWndProc_orig)(void *, HWND, UINT, WPARAM, LPARAM) = nullptr;
LRESULT THISCALL CAddressBand__AddressBandWndProc_hook(
    void   *pThis,
    HWND    hwnd,
    UINT    uMsg,
    WPARAM  wParam,
    LPARAM  lParam
)
{
    if (settings.smalladdress && uMsg == WM_WINDOWPOSCHANGING && ModernSearchFeatureEnabled())
    {
        LPWINDOWPOS lpwp = (LPWINDOWPOS)lParam;
        HWND hwSearch = FindWindowExW(GetParent(hwnd), NULL, L"UniversalSearchBand", NULL);
        if (hwSearch)
        {
            RECT rc;
            GetWindowRect(hwSearch, &rc);
            MapWindowPoints(HWND_DESKTOP, GetParent(hwSearch), (LPPOINT)&rc, 2);
            lpwp->y = rc.top - 1;
        }
        return 0;
    }

    return CAddressBand__AddressBandWndProc_orig(
        pThis, hwnd, uMsg, wParam, lParam
    );
}

#ifdef _WIN64
#   define CAddressList__hwnd(pThis) *((HWND *)pThis + 3)
#else
#   define CAddressList__hwnd(pThis) *((HWND *)pThis + 4)
#endif

void (THISCALL *CAddressList__InitCombobox_orig)(void *) = nullptr;
void THISCALL CAddressList__InitCombobox_hook(
    void *pThis
)
{
    CAddressList__InitCombobox_orig(pThis);
    if (settings.smalladdress)
    {
        HWND hWnd = CAddressList__hwnd(pThis);
        if (hWnd)
        {
            SendMessageW(hWnd, 0x40E, 0, 8);
        }
    }
}
#pragma endregion // "Small address bar"

/* Disable ribbon */
bool (THISCALL *CShellBrowser__ShouldShowRibbon_orig)(void *, void *) = nullptr;
bool THISCALL CShellBrowser__ShouldShowRibbon_hook(
    void *pThis,
    void *psiLocation
)
{
    return settings.noribbon
    ? false
    : CShellBrowser__ShouldShowRibbon_orig(
        pThis, psiLocation
    );
}

#define CWM_GETISHELLBROWSER WM_USER + 7

#ifdef _WIN64
#   define IShellBrowser_IShellItem(pThis) *((IShellItem **)pThis + 46)
#else
#   define IShellBrowser_IShellItem(pThis) *((IShellItem **)pThis + 47)
#endif

/* Is the Ribbon visible? */
bool ExplorerHasRibbon(HWND hWnd)
{
    if (GetParent(hWnd))
    {
        hWnd = GetAncestor(hWnd, GA_ROOT);
    }

    HWND hRibbonDock = FindWindowExW(hWnd, NULL, L"UIRibbonCommandBarDock", L"UIRibbonDockTop");
    if (hRibbonDock)
    {
        RECT rc = { 0 };
        if (GetWindowRect(hRibbonDock, &rc))
        {
            return (rc.bottom - rc.top) > 0;
        }
    }

    return false;
}

#pragma region "Old navigation pane sizing"

/* Allow non-even heights */
HWND (THISCALL *CNscTree__CreateTreeview_orig)(void *, HWND) = nullptr;
HWND THISCALL CNscTree__CreateTreeview_hook(
    void *pThis,
    HWND  hwndParent
)
{
    HWND hRes = CNscTree__CreateTreeview_orig(
        pThis, hwndParent
    );

    if (settings.npst != NPST_DEFAULT && hRes)
    {
        DWORD dwStyle = GetWindowLongPtrW(hRes, GWL_STYLE);
        dwStyle |= TVS_NONEVENHEIGHT;

        if (settings.npst == NPST_VISTA)
        {
            dwStyle &= ~TVS_FULLROWSELECT;
        }

        SetWindowLongPtrW(
            hRes,
            GWL_STYLE,
            dwStyle
        );
    }

    return hRes;
}

/* Undocumented */
#define TVM_SETTOPMARGIN (TV_FIRST+74)

void HandleTvItem(HWND hTreeView, LPTVITEMEXW lptvi)
{
    if (!(lptvi->mask & TVIF_INTEGRAL))
    {
        return;
    }

    if (settings.npst == NPST_VISTA)
    {
        lptvi->iIntegral = 1;
    }
    else
    {
        if (lptvi->iReserved == 1)
        {
            HTREEITEM hFirst = TreeView_GetFirstVisible(hTreeView);
            BOOL bIsFirst = (hFirst == lptvi->hItem) | !hFirst;
            
            lptvi->iIntegral = bIsFirst ? 1 : 2;
        }
        else
        {
            lptvi->iIntegral = 1;
        }
    }
}

/* Set old height */
SUBCLASSPROC CNscTree_s_SubClassTreeWndProc_orig = nullptr;
LRESULT CALLBACK CNscTree_s_SubClassTreeWndProc_hook(
    HWND      hwnd, 
    UINT      uMsg,
    WPARAM    wParam,
    LPARAM    lParam,
    UINT_PTR  uIdSubclass,
    DWORD_PTR dwRefData
)
{
    if (settings.npst != NPST_DEFAULT)
    {
        /* For some UNGODLY reason, other shit (StartIsBack++, folder dialog)
           use this same class so we have to get creative */
        HWND hPar = GetParent(GetParent(hwnd));
        if (hPar && IsWindow(hPar))
        {
            WCHAR szClass[256];
            if (GetClassNameW(hPar, szClass, 256))
            {
                if (0 == wcscmp(szClass, L"CtrlNotifySink"))
                {
                    switch (uMsg)
                    {
                        case TVM_INSERTITEMW:
                        {
                            LPTVINSERTSTRUCTW lpis = (LPTVINSERTSTRUCTW)lParam;
                            LPTVITEMEXW lptvi = &lpis->itemex;
                            HTREEITEM hItem = (HTREEITEM)DefSubclassProc(hwnd, uMsg, wParam, lParam);
                            lptvi->hItem = hItem;
                            /* The TVM_SETITEMW handle here already calls HandleTvItem, no need to here */
                            TreeView_SetItem(hwnd, lptvi);
                            if (hItem)
                            {
                                return (LRESULT)hItem;
                            }
                            break;
                        }
                        case TVM_SETITEMW:
                        {
                            HandleTvItem(hwnd, (LPTVITEMEXW)lParam);
                            break;
                        }
                        case TVM_SETITEMHEIGHT:
                            if (settings.npst == NPST_VISTA)
                            {
                                wParam = ScaleForDPI(19);
                            }
                            else
                            {
                                wParam = ScaleForDPI(21);
                            }
                            break;
                        case TVM_SETTOPMARGIN:
                            wParam = (settings.npst == NPST_VISTA)
                            ? 0
                            : ScaleForDPI(9);
                            break;
                        case TVM_SETINDENT:
                            if (settings.npst == NPST_VISTA)
                            {
                                wParam = ScaleForDPI(1);
                            }
                            break;
                    }
                }
            }
        }
    }

    return CNscTree_s_SubClassTreeWndProc_orig(
        hwnd, uMsg, wParam, lParam, uIdSubclass, dwRefData
    );
}

#ifdef _WIN64
#   define CNscTree_m_logicalIndent(pThis)  *((DWORD *)pThis + 116)
#else
#   define CNscTree_m_logicalIndent(pThis)  *((DWORD *)pThis + 75)
#endif

void (THISCALL *CNscTree_ScaleAndSetIndent_orig)(void *);
void THISCALL CNscTree_ScaleAndSetIndent_hook(
    void *pThis
)
{
    /* It's inaccurate in Vista but changing this value doesn't
       seem to do anything there */
    if (settings.npst == NPST_SEVEN)
    {
        CNscTree_m_logicalIndent(pThis) = 10;
    }
    CNscTree_ScaleAndSetIndent_orig(pThis);
}

#pragma endregion // ""Old navigation pane sizing""

#pragma region "Navbar glass"

#define CNavBar__hwnd(pThis) *((HWND *)pThis + 6)

using DwmExtendFrameIntoClientArea_t = decltype(&DwmExtendFrameIntoClientArea);
DwmExtendFrameIntoClientArea_t DwmExtendFrameIntoClientArea_orig = nullptr;

/* Re-impl of function from Windows 7 */
void CNavBar__UpdateGlass(void *pThis)
{
    HWND hWnd = CNavBar__hwnd(pThis);
    if (hWnd && IsWindow(hWnd))
    {
        UINT dpi = GetDpiForWindow(hWnd);
        const int captionHeight =
        GetSystemMetricsForDpi(SM_CYFRAME, dpi)
        + GetSystemMetricsForDpi(SM_CYCAPTION, dpi)
        + GetSystemMetricsForDpi(SM_CXPADDEDBORDER, dpi);

        if (IsCompositionActive()
        && !settings.nocomposition)
        {
            HWND hExplorer = GetAncestor(hWnd, GA_ROOT);
            if (hExplorer && !ExplorerHasRibbon(hWnd))
            {
                RECT rc = { 0 };
                GetWindowRect(hWnd, &rc);
                MapWindowPoints(HWND_DESKTOP, GetParent(hWnd), (LPPOINT)&rc, 2);

                if (rc.top == 0 || rc.top == captionHeight)
                {
                    MARGINS mrg = { 0 };
                    mrg.cyTopHeight = rc.bottom;
                    DwmExtendFrameIntoClientArea_orig(
                        hExplorer, &mrg
                    );
                }
            }
            else
            {
                MARGINS mrg = { 0 };
                mrg.cyTopHeight = captionHeight;
                DwmExtendFrameIntoClientArea_orig(
                    hExplorer, &mrg
                );
            }
        }
    }
}

void (THISCALL *CNavBar__SetTheme_orig)(void *) = nullptr;
void THISCALL CNavBar__SetTheme_hook(
    void *pThis
)
{
    CNavBar__SetTheme_orig(pThis);
    if (settings.navbarglass)
    {
        CNavBar__UpdateGlass(pThis);
    }
}

HRESULT (STDCALL *CSeparatorBand_GetBandInfo_orig)(void *, DWORD, DWORD, DESKBANDINFO *) = nullptr;
HRESULT STDCALL CSeparatorBand_GetBandInfo_hook(
    void         *pThis,
    DWORD         dwBandID,
    DWORD         dwViewMode,
    DESKBANDINFO *pdbi
)
{
    HRESULT hr = CSeparatorBand_GetBandInfo_orig(pThis, dwBandID, dwViewMode, pdbi);
    if (settings.navbarglass && SUCCEEDED(hr))
    {
        if (settings.noribbon)
        {
            int cx = (dwBandID == 5) ? -4 : 0;
            pdbi->ptMinSize.x = cx;
            pdbi->ptMaxSize.x = cx;
            pdbi->ptActual.x = cx;
        }
        else if (dwBandID == 5)
        {
            IDeskBand *pdb = (IDeskBand *)pThis;
            HWND hWnd = NULL;
            if (SUCCEEDED(pdb->GetWindow(&hWnd)) && !ExplorerHasRibbon(hWnd))
            {
                pdbi->ptMinSize.x = -4;
                pdbi->ptMaxSize.x = -4;
                pdbi->ptActual.x = -4;
            }
        }
    }
    return hr;
}

WNDPROC CNavBar_s_SizableWndProc_orig = nullptr;
LRESULT CALLBACK CNavBar_s_SizableWndProc_hook(
    HWND   hwnd,
    UINT   uMsg,
    WPARAM wParam,
    LPARAM lParam
)
{
    LRESULT lRet = CNavBar_s_SizableWndProc_orig(
        hwnd, uMsg, wParam, lParam
    );

    if (settings.navbarglass)
    {
        void *pThis = (void *)GetWindowLongPtrW(hwnd, 0);
        switch (uMsg)
        {
            case WM_SIZE:
            case WM_NCPAINT:
                if (pThis)
                {
                    CNavBar__UpdateGlass(pThis);
                }
                break;
            case WM_PAINT:
            case WM_NOTIFY:
                if (pThis && lParam && ((LPNMHDR)lParam)->code == 0xFFFFFFF4)
                {
                    CNavBar__UpdateGlass(pThis);
                }
                break;
            case WM_ERASEBKGND:
                if (!ExplorerHasRibbon(hwnd))
                    return lRet;
        }
    }

    if (settings.beta8navbarbg && uMsg == WM_ERASEBKGND)
    {
        HDC hdc = (HDC)wParam;
        RECT rc;
        GetClientRect(hwnd, &rc);

        // Windows 8 betas used some Ribbon API to get the background color,
        // but it shouldn't ever change.
        HBRUSH hbrBackground = CreateSolidBrush(RGB(223, 233, 244));
        HBRUSH hbrBorder = CreateSolidBrush(RGB(186, 201, 219));

        FillRect(hdc, &rc, hbrBackground);
        rc.top = rc.bottom - 1;
        FillRect(hdc, &rc, hbrBorder);
        
        DeleteObject(hbrBackground);
        DeleteObject(hbrBorder);
    }

    return lRet;
}

/* Subclassed rebars */
std::vector<HWND> g_subclassedRebars;

/* The x86 version of the actual subclass proc got fucking mangled,
   so just subclass it ourself. */
LRESULT CALLBACK RebarSubclassWndProc(
    HWND       hWnd,
    UINT       uMsg,
    WPARAM     wParam,
    LPARAM     lParam,
    DWORD_PTR  dwRefData
)
{
    if (settings.navbarglass
    && IsCompositionActive()
    && !settings.nocomposition
    && !ExplorerHasRibbon(hWnd)
    && (uMsg == WM_ERASEBKGND || uMsg == WM_PAINT))
    {
        PAINTSTRUCT ps;
        HDC hDC = (uMsg == WM_ERASEBKGND) ? (HDC)wParam : BeginPaint(hWnd, &ps);

        RECT rc;
        GetClientRect(hWnd, &rc);
        FillRect(hDC, &rc, (HBRUSH)GetStockObject(BLACK_BRUSH));

        if (uMsg == WM_PAINT)
        {
            EndPaint(hWnd, &ps);
        }

        return 0;
    }
    else if (uMsg == WM_NCDESTROY)
    {
        g_subclassedRebars.erase(std::remove_if(
            g_subclassedRebars.begin(),
            g_subclassedRebars.end(),
            [hWnd](HWND hw)
            {
                return hw == hWnd;
            }
        ));
    }

    return DefSubclassProc(
        hWnd, uMsg, wParam, lParam
    );
}

#define CNavBar_CNavBandSite__hwnd(pThis) *((HWND *)pThis + 17)

HRESULT (THISCALL *CNavBar_CNavBandSite__Initialize_orig)(void *, HWND) = nullptr;
HRESULT THISCALL CNavBar_CNavBandSite__Initialize_hook(
    void *pThis,
    HWND  hwndParent
)
{
    HRESULT hr = CNavBar_CNavBandSite__Initialize_orig(
        pThis, hwndParent
    );

    if (settings.navbarglass && SUCCEEDED(hr))
    {
        HWND hWnd = CNavBar_CNavBandSite__hwnd(pThis);
        if (hWnd && IsWindow(hWnd))
        {
            if (WindhawkUtils::SetWindowSubclassFromAnyThread(hWnd, RebarSubclassWndProc, NULL))
            {
                g_subclassedRebars.push_back(hWnd);
            }
        }
    }

    return hr;
}

void (THISCALL *CNavBar__OnFrameStateChanged_orig)(void *, DWORD) = nullptr;
void THISCALL CNavBar__OnFrameStateChanged_hook(
    void  *pThis,
    DWORD dwUnknown
)
{
    CNavBar__OnFrameStateChanged_orig(
        pThis, dwUnknown
    );
    if (settings.navbarglass)
    {
        CNavBar__UpdateGlass(pThis);
    }
}

/* Hide window icon and caption */
#define CExplorerFrame__hwnd(pThis) *((HWND *)pThis + 1)

void (THISCALL *CExplorerFrame__UpdateFrameState_orig)(void *) = nullptr;
void THISCALL CExplorerFrame__UpdateFrameState_hook(
    void *pThis
)
{
    CExplorerFrame__UpdateFrameState_orig(pThis);
    if (settings.hidetitle && settings.noribbon)
    {
        HWND hWnd = CExplorerFrame__hwnd(pThis);
        if (hWnd && IsWindow(hWnd))
        {
            int attr = WTNCA_NODRAWCAPTION | WTNCA_NODRAWICON;
            SetWindowThemeAttribute(
                hWnd,
                WTA_NONCLIENT,
                &attr,
                sizeof(int) * 2
            );
        }
    }
}

HRESULT (THISCALL *CNavBar_ConstructNavBarThemeClassName_orig)(void *, WCHAR **) = nullptr;
HRESULT THISCALL CNavBar_ConstructNavBarThemeClassName_hook(
    void   *pThis,
    LPWSTR *ppszOut
)
{
    HRESULT hr = CNavBar_ConstructNavBarThemeClassName_orig(
        pThis, ppszOut
    );

    if (settings.navbarglass && SUCCEEDED(hr))
    {
        const WCHAR c_szDarkMode[]       = L"DarkMode";
        const WCHAR c_szMax[]            = L"Max";
        const WCHAR c_szNavbar[]         = L"Navbar";
        const WCHAR c_szInactive[]       = L"Inactive";
        const WCHAR c_szComposited[]     = L"Composited";
        constexpr size_t c_cchDarkMode   = sizeof("DarkMode") - 1;
        constexpr size_t c_cchMax        = sizeof("Max") - 1;
        constexpr size_t c_cchNavbar     = sizeof("Navbar") - 1;
        constexpr size_t c_cchInactive   = sizeof("Inactive") - 1;
        constexpr size_t c_cchComposited = sizeof("Composited") - 1;

        bool fDarkMode = nullptr != wcsstr(*ppszOut, c_szDarkMode);
        bool fMax = nullptr != wcsstr(*ppszOut, c_szMax);
        // Inverted: This is because Windows inverts inactive class name for some reason.
        bool fInactive = nullptr == wcsstr(*ppszOut, c_szInactive);
        bool fComposited = !settings.nocomposition && nullptr != wcsstr(*ppszOut, c_szComposited);

        size_t cchAlloc = c_cchNavbar + 1;
        if (fDarkMode)
            cchAlloc += c_cchDarkMode;
        if (fMax)
            cchAlloc += c_cchMax;
        if (fInactive)
            cchAlloc += c_cchInactive;
        if (fComposited)
            cchAlloc += c_cchComposited;

        LPWSTR pszClassName = (LPWSTR)CoTaskMemAlloc(cchAlloc * sizeof(WCHAR));
        if (!pszClassName)
            return E_OUTOFMEMORY;
        CoTaskMemFree(*ppszOut);
        *ppszOut = pszClassName;

        ZeroMemory(pszClassName, cchAlloc * sizeof(WCHAR));
        if (fDarkMode)
            wcscat(pszClassName, c_szDarkMode);
        if (fMax)
            wcscat(pszClassName, c_szMax);
        if (fInactive)
            wcscat(pszClassName, c_szInactive);
        wcscat(pszClassName, c_szNavbar);
        if (fComposited)
            wcscat(pszClassName, c_szComposited);
    }

    return hr;
}

#pragma endregion // "Navbar glass"

/* Disable Up button */
HRESULT (STDCALL *CUpBand_GetBandInfo_orig)(void *, DWORD, DWORD, DESKBANDINFO *) = nullptr;
HRESULT STDCALL CUpBand_GetBandInfo_hook(
    void         *pThis,
    DWORD         dwBandID,
    DWORD         dwViewMode,
    DESKBANDINFO *pdbi
)
{
    HRESULT hr = CUpBand_GetBandInfo_orig(
        pThis, dwBandID, dwViewMode, pdbi
    );

    if (settings.noup && SUCCEEDED(hr))
    {
        pdbi->ptIntegral.x = 0;
        pdbi->ptMaxSize.x = 0;
        pdbi->ptMinSize.x = 0;
    }

    return hr;
}

#pragma region "Aero travel buttons"
#ifdef _WIN64
#define CTravelBand__hwndTools(pThis) *((HWND *)pThis + 16)
#else
#define CTravelBand__hwndTools(pThis) *((HWND *)pThis + 18)
#endif

#define MAP_RESOURCE(from, to)       \
case from:                           \
    lpnewbmp = MAKEINTRESOURCEW(to); \
    break

/* Redirect classic travel images and sizes */
using ImageList_LoadImageW_t = decltype(&ImageList_LoadImageW);
ImageList_LoadImageW_t ImageList_LoadImageW_orig = nullptr;
HIMAGELIST WINAPI ImageList_LoadImageW_hook(
    HINSTANCE hi,
    LPCWSTR   lpbmp,
    int       cx,
    int       cGrow,
    COLORREF  crMask,
    UINT      uType,
    UINT      uFlags
)
{
    if (settings.aerotravel)
    {
        WCHAR szPath[MAX_PATH];
        GetModuleFileNameW(hi, szPath, MAX_PATH);
        WCHAR *slash = wcsrchr(szPath, L'\\');
        LPCWSTR lpnewbmp = NULL;
        if (slash && 0 == wcsicmp(slash, L"\\explorerframe.dll"))
        {
            if (IsHighDPI())
            {               
                switch ((long long)lpbmp)
                {
                    MAP_RESOURCE(281, 589);
                    MAP_RESOURCE(582, 585);
                    MAP_RESOURCE(583, 586);
                    MAP_RESOURCE(584, 587);
                }
            }
            else 
            {
                switch ((long long)lpbmp)
                {
                    MAP_RESOURCE(281, 280);
                    MAP_RESOURCE(582, 577);
                    MAP_RESOURCE(583, 578);
                    MAP_RESOURCE(584, 579);
                }
            }

            if (lpnewbmp)
            {
                lpbmp = lpnewbmp;
                switch (cx)
                {
                    case 25:
                        cx = IsHighDPI() ? 34 : 27;
                        break;
                    case 72:
                        cx = IsHighDPI() ? 87 : 70;
                        break;
                }
            }
        }
    }
    return ImageList_LoadImageW_orig(
        hi, lpbmp, cx, cGrow, crMask, uType, uFlags
    );
}

#ifdef _WIN64
#define CTravelBand__himlTravelBackground(pThis) *((HIMAGELIST *)pThis + 31)
#define CTravelBand__iControlState(pThis) *((DWORD *)pThis + 48)
#else
#define CTravelBand__himlTravelBackground(pThis) *((HIMAGELIST *)pThis + 40)
#define CTravelBand__iControlState(pThis) *((DWORD *)pThis + 26)
#endif

LRESULT CALLBACK DrawTravelBackground(
    HWND   hWnd,
    UINT   uMsg,
    WPARAM wParam,
    LPARAM lParam
)
{
    void *pThis = (void *)GetWindowLongPtrW(hWnd, GWLP_USERDATA);
    if (pThis)
    {
        PAINTSTRUCT ps;
        if (wParam)
        {
            ps.hdc = (HDC)wParam;
            if (!GetClipBox(ps.hdc, &ps.rcPaint))
            {
                SetRectEmpty(&ps.rcPaint);
            }
            ps.fRestore = FALSE;
        }
        else
        {
            BeginPaint(hWnd, &ps);
            ps.fRestore = TRUE;
        }

        DrawThemeParentBackground(hWnd, ps.hdc, &ps.rcPaint);

        HIMAGELIST himl = CTravelBand__himlTravelBackground(pThis);
        if (himl)
        {
            ImageList_Draw(
                himl, CTravelBand__iControlState(pThis), ps.hdc, settings.noribbon ? 1 : 5, 2, ILD_TRANSPARENT
            );
        }

        if (ps.fRestore)
        {
            EndPaint(hWnd, &ps);
        }
    }
    return 0;
}

WNDPROC CTravelBand_s_TravelWndProc_orig = nullptr;
LRESULT CALLBACK CTravelBand_s_TravelWndProc_hook(
    HWND   hwnd,
    UINT   uMsg,
    WPARAM wParam,
    LPARAM lParam
)
{
    if (settings.aerotravel && (uMsg == WM_PAINT || uMsg == WM_PRINTCLIENT))
    {
        return DrawTravelBackground(hwnd, uMsg, wParam, lParam);
    }

    return CTravelBand_s_TravelWndProc_orig(
        hwnd, uMsg, wParam, lParam
    );
}

std::vector<HWND> g_subclassedTravelbands;

/* Fix spacing */
LRESULT CALLBACK TravelBandToolbarSubclassProc(
    HWND      hWnd,
    UINT      uMsg,
    WPARAM    wParam,
    LPARAM    lParam,
    DWORD_PTR dwRefData
)
{
    if (uMsg == TB_SETBUTTONSIZE)
    {
        return DefSubclassProc(hWnd, TB_SETBUTTONSIZE, 0, 0);
    }

    if (settings.aerotravel)
    {
        switch (uMsg)
        {
            case TB_SETPADDING:
                return DefSubclassProc(hWnd, TB_SETPADDING, 0, MAKELPARAM(1, 0));
        }
    }

    if (uMsg == WM_NCDESTROY)
    {
        g_subclassedTravelbands.erase(std::remove_if(
            g_subclassedTravelbands.begin(),
            g_subclassedTravelbands.end(),
            [hWnd](HWND hw)
            {
                return hw == hWnd;
            }
        ));
    }

    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

/* Subclass toolbar */
void (THISCALL *CTravelBand__CreateTravelButtons_orig)(void *) = nullptr;
void THISCALL CTravelBand__CreateTravelButtons_hook(
    void *pThis
)
{
    CTravelBand__CreateTravelButtons_orig(pThis);
    HWND hWnd = CTravelBand__hwndTools(pThis);
    if (hWnd && IsWindow(hWnd))
    {
        if (WindhawkUtils::SetWindowSubclassFromAnyThread(hWnd, TravelBandToolbarSubclassProc, NULL))
        {
            g_subclassedTravelbands.push_back(hWnd);
        }
    }
}

/* Fix position and add pressed state for aero travel buttons */
void (THISCALL *CTravelBand__SetButtonImagesClassicMode_orig)(void *) = nullptr;
void THISCALL CTravelBand__SetButtonImagesClassicMode_hook(
    void *pThis
)
{
    CTravelBand__SetButtonImagesClassicMode_orig(pThis);

    HWND hWnd = CTravelBand__hwndTools(pThis);
    if (settings.aerotravel && hWnd && IsWindow(hWnd))
    {
        int x = settings.noribbon ? 2 : 6;
        int y = 2;

        SetWindowPos(
            hWnd,
            NULL,
            x, y,
            IsHighDPI() ? 85 : 68,
            IsHighDPI() ? 36 : 30,
            SWP_NOZORDER
        );

        HINSTANCE hInstance = GetModuleHandleW(L"ExplorerFrame.dll");

        HIMAGELIST hilPressed = ImageList_LoadImageW_orig(
            hInstance,
            IsHighDPI() ? MAKEINTRESOURCEW(588) : MAKEINTRESOURCEW(581),
            IsHighDPI() ? 34 : 27,
            0,
            0xFF000000,
            IMAGE_BITMAP,
            LR_CREATEDIBSECTION
        );

        if (hilPressed)
        {
            HIMAGELIST hilOld = (HIMAGELIST)SendMessageW(
                hWnd, TB_SETPRESSEDIMAGELIST, 0, (LPARAM)hilPressed
            );

            if (hilOld)
            {
                ImageList_Destroy(hilOld);
            }
        }

        /* Microsoft intentionally fucks up the sizing
           in this function, so we need to set it ourselves */
        SendMessageW(hWnd, TB_SETBUTTONSIZE, 0, 0);
        SendMessageW(hWnd, TB_SETPADDING, 0, MAKELPARAM(1, 0));
    }
}

HRESULT (STDCALL *CTravelBand_GetBandInfo_orig)(void *, DWORD, DWORD, DESKBANDINFO *) = nullptr;
HRESULT STDCALL CTravelBand_GetBandInfo_hook(
    void *pThis,
    DWORD dwBandID,
    DWORD dwViewMode,
    DESKBANDINFO *pdbi
)
{
    HRESULT hr = CTravelBand_GetBandInfo_orig(pThis, dwBandID, dwViewMode, pdbi);
    if (settings.aerotravel && SUCCEEDED(hr))
    {

        int w, h;
        if (settings.noribbon)
        {
            w = IsHighDPI() ? 87 : 70;
        }
        else
        {
            w = IsHighDPI() ? 101 : 84;
        }
        h = IsHighDPI() ? 38 : 30;

        pdbi->ptMinSize.x = w;
        pdbi->ptActual.x = w;
        pdbi->ptMaxSize.x = w;

        pdbi->ptMinSize.y = h;
        pdbi->ptActual.y = h;
        pdbi->ptMaxSize.y = h;
    }
    return hr;
}
#pragma endregion // "Aero travel buttons"

/* Fix state for travel band; not for Aero specifically, just in general */
#ifdef _WIN64
#   define CTravelBand__fInDropDown(pThis) *((DWORD *)pThis + 61)
#else
#   define CTravelBand__fInDropDown(pThis) *((DWORD *)pThis + 39)
#endif

void (THISCALL *CTravelBand__SetControlState_orig)(void *, int) = nullptr;
void THISCALL CTravelBand__SetControlState_hook(
    void *pThis,
    int   iControlState
)
{
    if (CTravelBand__fInDropDown(pThis))
    {
        iControlState = 3;
    }

    CTravelBand__SetControlState_orig(
        pThis, iControlState
    );
}

/* Remove pins from Quick access */
HRESULT (STDCALL *CNscTree_SetStateImageList_orig)(void *, HIMAGELIST) = nullptr;
HRESULT STDCALL CNscTree_SetStateImageList_hook(
    void       *pThis,
    HIMAGELIST  himl
)
{
    if (settings.nopins)
    {
        return S_OK;
    }
    return CNscTree_SetStateImageList_orig(pThis, himl);
}

/* Always show column headers (ItemsView) */
HRESULT (STDCALL *CShellBrowser_GetFolderFlags_orig)(void *, FOLDERFLAGS *, FOLDERFLAGS *);
HRESULT STDCALL CShellBrowser_GetFolderFlags_hook(
    void        *pThis,
    FOLDERFLAGS *pfolderMask,
    FOLDERFLAGS *pfolderFlags
)
{
    HRESULT hr = CShellBrowser_GetFolderFlags_orig(pThis, pfolderMask, pfolderFlags);
    if (SUCCEEDED(hr) && settings.colheaders)
    {
        if ((*pfolderMask & FWF_ALIGNLEFT) ? !(*pfolderFlags & FWF_ALIGNLEFT) : true)
        {
            *pfolderMask |= FWF_NOHEADERINALLVIEWS;
            *pfolderFlags &= ~FWF_NOHEADERINALLVIEWS;
        }
    }
    return hr;
}

DEFINE_GUID(CLSID_ControlPanelProcessExplorerHost, 0x5BD95610, 0x9434, 0x43C2, 0x88,0x6C, 0x57,0x85,0x2C,0xC8,0xA1,0x20);

/* Stop Explorer from relaunching on CPL enter */
bool (STDCALL *UseSeparateProcess_orig)(IShellItem *);
bool STDCALL UseSeparateProcess_hook(IShellItem *shellItem)
{
    SHELLSTATEW ss = { 0 };
    SHGetSetSettings(&ss, SSF_SEPPROCESS, FALSE);
    // For whatever reason, it gets set to -1 when this option is enabled
    if (!settings.alwayscpl || ss.fSepProcess != 0)
        return UseSeparateProcess_orig(shellItem);
    return false;
}

/* Ditto, but for separate process mode. */
GUID *(THISCALL *CExplorerLauncher_GetHostFromTarget_orig)(void *, GUID *, LPCITEMIDLIST) = nullptr;
GUID *THISCALL CExplorerLauncher_GetHostFromTarget_hook(
    void          *pThis,
    GUID          *pclsid,
    LPCITEMIDLIST  pidl
)
{
    if (settings.alwayscpl)
    {
        IShellItem *psi = nullptr;
        SHCreateItemFromIDList(pidl, IID_PPV_ARGS(&psi));
        if (psi)
        {
            if (UseSeparateProcess_hook(psi))
            {
                *pclsid = CLSID_ControlPanelProcessExplorerHost;
                psi->Release();
                return pclsid;
            }
            psi->Release();
        }
    }

    return CExplorerLauncher_GetHostFromTarget_orig(
        pThis, pclsid, pidl
    );
}

#ifdef _WIN64
#   define CShellBrowser__bbd_ns_hwndFrame(pThis) *((HWND *)pThis + 55)

static BOOL CALLBACK Aerexplorer_InvalidateAllChildren(HWND hWnd, LPARAM lParam)
{
    if (!hWnd)
        return FALSE;

    InvalidateRect(hWnd, NULL, TRUE);
    return TRUE;
}
/* Hook Ribbon visibility change to fix backgrounds. */
void (STDCALL *CShellBrowser___OnRibbonVisibilityChange_orig)(void *pThis, int newState) = nullptr;
void STDCALL CShellBrowser___OnRibbonVisibilityChange_hook(void *pThis, int newState)
{
    if (!settings.navbarglass)
        return CShellBrowser___OnRibbonVisibilityChange_orig(pThis, newState);

    // We want to get the window of the shell browser, which is the DUIViewWndClassName
    // inside of the explorer window.
    HWND hWnd = CShellBrowser__bbd_ns_hwndFrame(pThis);

    HWND hWndExplorerRoot = GetAncestor(hWnd, GA_ROOTOWNER);
    HWND hWndWorkerW = FindWindowExW(hWndExplorerRoot, NULL, L"WorkerW", NULL);

    if (hWndWorkerW)
    {
        HWND hWndRebar = FindWindowExW(hWndWorkerW, NULL, L"RebarWindow32", NULL);

        if (hWndRebar)
        {
            // Invalidate the window to update the background. This is required whenever
            // the state changes, because otherwise graphical artifacts are always visible.
            InvalidateRect(hWndRebar, NULL, TRUE);
            EnumChildWindows(hWndRebar, Aerexplorer_InvalidateAllChildren, NULL);

            // A pointer to the CNavBar is stored in the data for this window.
            void *pNavBar = (void *)GetWindowLongPtrW(hWndWorkerW, 0);

            // Refresh the glass state since it might changed after this event.
            CNavBar__UpdateGlass(pNavBar);

            // Flicker the nonclient activation state on the navbar window to force it
            // to reflow:
            SendMessageW(hWndWorkerW, WM_NCACTIVATE, 0, NULL);
            SendMessageW(hWndWorkerW, WM_NCACTIVATE, 1, NULL);
        }
    }

    return CShellBrowser___OnRibbonVisibilityChange_orig(pThis, newState);
}
#endif

#pragma endregion // "ExplorerFrame.dll hooks"

#pragma region "shell32.dll hooks"

#ifdef _WIN64
#   define CDefView__hwndView(pThis) *((HWND *)pThis + 79)
#else
#   define CDefView__hwndView(pThis) *((HWND *)pThis + 89)
#endif

/* Enable native list view */
BOOL (THISCALL *CDefView__UseItemsView_orig)(void *) = nullptr;
BOOL THISCALL CDefView__UseItemsView_hook(
    void *pThis
)
{
    if (settings.listview)
    {
        /* Fix for StartIsBack because I fucking hate Tihiy */
        HWND hWnd = CDefView__hwndView(pThis);
        if (hWnd)
        {
            WCHAR szClass[256];
            if (GetClassNameW(GetParent(hWnd), szClass, 256)
            && 0 == wcscmp(szClass, L"SearchResults"))
            {
                return CDefView__UseItemsView_orig(pThis);
            }
        }
        return FALSE;
    }

    return CDefView__UseItemsView_orig(pThis);
}

#define REPLACE_UIFILE(n)         \
case n:                           \
    szReplacement = UIFILE_ ## n; \
    break;

/* Redirect UIFILEs for details pane alignment */
#ifdef _WIN64
HRESULT (STDCALL *DUI_LoadUIFileFromResources_orig)(HINSTANCE, UINT, LPWSTR *) = nullptr;
HRESULT STDCALL DUI_LoadUIFileFromResources_hook(
    HINSTANCE  hInstance,
    UINT       uResId,
    LPWSTR    *pszOut
)
{
    LPCWSTR szReplacement = NULL;
    if (settings.detailspane)
    {
        WCHAR szPath[MAX_PATH];
        if (GetModuleFileNameW(hInstance, szPath, MAX_PATH))
        {
            WCHAR *slash = wcsrchr(szPath, L'\\');
            if (slash && 0 == wcsicmp(slash, L"\\shell32.dll"))
            {
                switch (uResId)
                {
                    REPLACE_UIFILE(3)
                    REPLACE_UIFILE(4)
                    REPLACE_UIFILE(5)
                    REPLACE_UIFILE(6)
                    REPLACE_UIFILE(19)
                    REPLACE_UIFILE(20)
                    REPLACE_UIFILE(21)
                }
            }
        }
    }

    if (szReplacement)
    {
        *pszOut = (LPWSTR)LocalAlloc(LPTR, (wcslen(szReplacement) + 1) * sizeof(WCHAR));
        if (*pszOut)
        {
            wcscpy(*pszOut, szReplacement);
        }
        return S_OK;
    }

    return DUI_LoadUIFileFromResources_orig(
        hInstance, uResId, pszOut
    );
}
#endif

const struct { DWORD dwDescId; UINT uResId; } g_categoryMap[] = {
    { SHDID_FS_DIRECTORY,        9338 },
    { SHDID_COMPUTER_SHAREDDOCS, 9338 },
    { SHDID_COMPUTER_FIXED,      DG_HARDDISKS },
    { SHDID_COMPUTER_DRIVE35,    DG_REMOVABLE },
    { SHDID_COMPUTER_REMOVABLE,  DG_REMOVABLE },
    { SHDID_COMPUTER_CDROM,      DG_REMOVABLE },
    { SHDID_COMPUTER_DRIVE525,   DG_REMOVABLE },
    { SHDID_COMPUTER_NETDRIVE,   9340 },
    { SHDID_COMPUTER_OTHER,      DG_OTHER },
    { SHDID_COMPUTER_RAMDISK,    DG_OTHER },
    { SHDID_COMPUTER_IMAGING,    DG_SCANNERS },
    { SHDID_COMPUTER_AUDIO,      DG_PORTABLEMEDIA },
    { SHDID_MOBILE_DEVICE,       DG_PORTABLE }
};

HRESULT (STDCALL *CStorageSystemTypeCategorizer_GetCategory_orig)(ICategorizer *, UINT, PCUITEMID_CHILD_ARRAY, DWORD *) = nullptr;
HRESULT STDCALL CStorageSystemTypeCategorizer_GetCategory_hook(
    ICategorizer          *pThis,
    UINT                   cidl,
    PCUITEMID_CHILD_ARRAY  apidl,
    DWORD                 *rgCategoryIds
)
{
    if (!settings.classicgrouping)
    {
        return CStorageSystemTypeCategorizer_GetCategory_orig(
            pThis, cidl, apidl, rgCategoryIds
        );
    }

    HRESULT hr = S_OK;
    IShellFolder2 *pShellFolder = (IShellFolder2 *)*((INT_PTR *)pThis + 3);
    if (pShellFolder)
    {
        for (UINT i = 0; i < cidl; i++)
        {
            rgCategoryIds[i] = DG_OTHER;

            VARIANT v;
            VariantInit(&v);

            SHCOLUMNID scid;
            scid.fmtid = FMTID_ShellDetails;
            scid.pid = PID_DESCRIPTIONID;

            hr = pShellFolder->GetDetailsEx(apidl[i], &scid, &v);
            if (SUCCEEDED(hr))
            {
                SHDESCRIPTIONID shdid;
                if (VariantToBuffer && SUCCEEDED(VariantToBuffer(&v, &shdid, sizeof(SHDESCRIPTIONID))))
                {
                    for (UINT j = 0; j < ARRAYSIZE(g_categoryMap); j++)
                    {
                        if (shdid.dwDescriptionId == g_categoryMap[j].dwDescId)
                        {
                            rgCategoryIds[i] = g_categoryMap[j].uResId;
                            break;
                        }
                    }
                }
            }
        }
    }
    
    return hr;
}

HRESULT (STDCALL *CStorageSystemTypeCategorizer_CompareCategory_orig)(ICategorizer *, CATSORT_FLAGS, DWORD, DWORD) = nullptr;
HRESULT STDCALL CStorageSystemTypeCategorizer_CompareCategory_hook(
    ICategorizer *pThis,
    CATSORT_FLAGS csfFlags,
    DWORD         dwCategoryId1,
    DWORD         dwCategoryId2
)
{
    if (!settings.classicgrouping)
    {
        return CStorageSystemTypeCategorizer_CompareCategory_orig(
            pThis, csfFlags, dwCategoryId1, dwCategoryId2
        );
    }

    int categoryArraySize = ARRAYSIZE(g_categoryMap);

    int firstPos = -1;
    int secondPos = -1;

    for (int i = 0; i < categoryArraySize; i++)
    {
        if (g_categoryMap[i].uResId == dwCategoryId1)
        {
            firstPos = i;
            break;
        }
    }

    for (int i = 0; i < categoryArraySize; i++)
    {
        if (g_categoryMap[i].uResId == dwCategoryId2)
        {
            secondPos = i;
            break;
        }
    }

    int diff = firstPos - secondPos;

    if (diff < 0)
        return 0xFFFF;

    return diff > 0;
}

HRESULT (STDCALL *CStorageSystemTypeCategorizer_GetCategoryInfo_orig)(ICategorizer *, DWORD, CATEGORY_INFO *) = nullptr;
HRESULT STDCALL CStorageSystemTypeCategorizer_GetCategoryInfo_hook(
    ICategorizer  *pThis,
    DWORD          dwCategoryId,
    CATEGORY_INFO *pci
)
{
    HRESULT hr = CStorageSystemTypeCategorizer_GetCategoryInfo_orig(
        pThis, dwCategoryId, pci
    );
    if (SUCCEEDED(hr) && settings.classicgrouping)
    {
        LPCDRVGRPSTRINGS pdgs = GetCurrentDriveLocale();
        LPCWSTR pszString = NULL;

        if (pdgs)
        {
            switch ((DRVGRP)dwCategoryId)
            {
                case DG_HARDDISKS:
                    pszString = pdgs->pszHardDisks;
                    break;
                case DG_REMOVABLE:
                    pszString = pdgs->pszRemovable;
                    break;
                case DG_OTHER:
                    pszString = pdgs->pszOther;
                    break;
                case DG_SCANNERS:
                    pszString = pdgs->pszScanners;
                    break;
                case DG_PORTABLEMEDIA:
                    pszString = pdgs->pszPortableMedia;
                    break;
                case DG_PORTABLE:
                    pszString = pdgs->pszPortable;
                    break;
            }

            if (pszString)
            {
                wcscpy(pci->wszName, pszString);
            }
        }
    }
    return hr;
}

/* Always show column headers (SysListView32) */
FOLDERFLAGS (THISCALL *IViewSettings_GetFolderFlags_orig)(void *) = nullptr;
FOLDERFLAGS THISCALL IViewSettings_GetFolderFlags_hook(
    void *pThis
)
{
    FOLDERFLAGS ff = IViewSettings_GetFolderFlags_orig(pThis);

    /* Testing for NOT FWF_ALIGNLEFT makes sure print dialogs are unaffected */
    if (settings.colheaders && !(ff & FWF_ALIGNLEFT))
    {
        ff &= ~FWF_NOHEADERINALLVIEWS;
    }
    return ff;
}

/* Views dropdown on left */

BOOL (__fastcall *IsRightSideCommand_orig)(REFGUID);
BOOL __fastcall IsRightSideCommand_hook(REFGUID rguidCmd)
{
    return settings.leftviews ? FALSE : IsRightSideCommand_orig(rguidCmd);
}

HRESULT (THISCALL *CCommandFolder__AppendRightSideCommands_orig)(void *, IUnknown *);
HRESULT THISCALL CCommandFolder__AppendRightSideCommands_hook(void *pThis, IUnknown *punk)
{
    return settings.leftviews ? S_OK : CCommandFolder__AppendRightSideCommands_orig(pThis, punk);
}

HRESULT (THISCALL *CCommandFolder__AppendOrganizeCommand_orig)(void *, IUnknown *);
HRESULT THISCALL CCommandFolder__AppendOrganizeCommand_hook(void *pThis, IUnknown *punk)
{
    HRESULT hr = CCommandFolder__AppendOrganizeCommand_orig(pThis, punk);
    if (settings.leftviews && SUCCEEDED(hr))
    {
        hr = CCommandFolder__AppendRightSideCommands_orig(pThis, punk);
    }
    return hr;
}

/* Command bar icons */

#ifdef _WIN64
#   define ExplorerCommandItem_iconIndex(pThis)  *((DWORD *)pThis + 10)
#else
#   define ExplorerCommandItem_iconIndex(pThis)  *((DWORD *)pThis + 8)
#endif

void (THISCALL *CSplitButton_UpdateIcon_orig)(void *, int);
void THISCALL CSplitButton_UpdateIcon_hook(
    void *pThis,
    int nIndex
)
{
    if (!settings.cmdbaricons)
        CSplitButton_UpdateIcon_orig(pThis, nIndex);
}

void (THISCALL *CSplitButton__UpdateDisplay_orig)(void *pThis, const struct ExplorerCommandItem *);
void THISCALL CSplitButton__UpdateDisplay_hook(
    void *pThis,
    const struct ExplorerCommandItem *pItem
)
{
    if (settings.cmdbaricons)
    {
        int nIndex = ExplorerCommandItem_iconIndex(pItem);
        // The "Open" command on EXE files has no icon, unlike Vista.
        // If that's the case, just grab the default document icon (the
        // blank page)
        if (nIndex == -1)
            nIndex = Shell_GetCachedImageIndexW(L"shell32.dll", -1, 0);
        CSplitButton_UpdateIcon_orig(pThis, nIndex);
    }
    CSplitButton__UpdateDisplay_orig(pThis, pItem);
}

#pragma endregion // "shell32.dll hooks"

#pragma region "windows.storage.dll hooks"

/* Remove This PC folders */
HRESULT (THISCALL *CObjectArray_AddItemsFromKeySkip_orig)(void *, HKEY, LPCWSTR, UINT, GUID *) = nullptr;
HRESULT THISCALL CObjectArray_AddItemsFromKeySkip_hook(
    void    *pThis,
    HKEY     hKey,
    LPCWSTR  lpSubKey,
    UINT     uUnused,
    GUID    *pGuid
)
{ 
    if (settings.nopcfolders && 0 == wcscmp(lpSubKey, L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\MyComputer\\NameSpace"))
    {
        return S_OK;
    }

    return CObjectArray_AddItemsFromKeySkip_orig(
        pThis, hKey, lpSubKey, uUnused, pGuid
    );
}

#pragma endregion // "windows.storage.dll hooks"

#pragma region "Other hooks"

/* Old address band sizing */
typedef int (WINAPI *GetSystemMetricsForDpi_t)(int, UINT);
GetSystemMetricsForDpi_t GetSystemMetricsForDpi_orig = nullptr;
int WINAPI GetSystemMetricsForDpi_hook(
    int  nIndex,
    UINT nDpi
)
{
    if (settings.smalladdress && nIndex == SM_CYFIXEDFRAME)
    {
        return MulDiv(-1, nDpi, 96);
    }

    return GetSystemMetricsForDpi_orig(
        nIndex, nDpi
    );
}

/* Enable classic travel band */
using OpenThemeData_t = decltype(&OpenThemeData);
OpenThemeData_t OpenThemeData_orig = nullptr;
HTHEME WINAPI OpenThemeData_hook(
    HWND    hWnd,
    LPCWSTR lpszClassList
)
{
    if (settings.aerotravel && 0 == wcscmp(lpszClassList, L"Navigation"))
    {
        WCHAR szClass[256];
        GetClassNameW(hWnd, szClass, 256);
        if (0 == wcscmp(szClass, L"TravelBand"))
        {
            return NULL;
        }
    }
    
    return OpenThemeData_orig(
        hWnd, lpszClassList
    );
}


/* Fix artifacting with navbar glass */
using DrawThemeParentBackground_t = decltype(&DrawThemeParentBackground);
DrawThemeParentBackground_t DrawThemeParentBackground_orig = nullptr;
HRESULT WINAPI DrawThemeParentBackground_hook(
    HWND    hwnd,
    HDC     hdc,
    LPRECT  prc
)
{
    if (settings.navbarglass
    && IsCompositionActive()
    && !ExplorerHasRibbon(hwnd)
    && !settings.nocomposition)
    {
        WCHAR szCls[256];
        GetClassNameW(hwnd, szCls, 256);
        HWND hPar = GetParent(hwnd);
        if (hPar && wcscmp(szCls, L"ToolbarWindow32"))
        {
            WCHAR szClass[256];
            GetClassNameW(hPar, szClass, 256);
            if (0 == wcscmp(szClass, L"ReBarWindow32"))
            {
                HWND hParPar = GetParent(hPar);
                if (hParPar)
                {
                    GetClassNameW(hParPar, szClass, 256);
                    if (0 == wcscmp(szClass, L"WorkerW"))
                    {
                        FillRect(
                            hdc,
                            prc,
                            (HBRUSH)GetStockObject(BLACK_BRUSH)
                        );
                        return S_OK;
                    }
                }
            }
        }
    }

    if (settings.aerotravel)
    {
        WCHAR szCls[256];
        GetClassNameW(hwnd, szCls, 256);
        HWND hPar = GetParent(hwnd);
        if (hPar && wcscmp(szCls, L"ToolbarWindow32"))
        {
            WCHAR szClass[256];
            GetClassNameW(hPar, szClass, 256);
            if (0 == wcscmp(szClass, L"TravelBand"))
            {
                DrawTravelBackground(hPar, WM_PAINT, (WPARAM)hdc, NULL);
            }
        }
    }

    return DrawThemeParentBackground_orig(
        hwnd, hdc, prc
    );
}

HRESULT WINAPI DwmExtendFrameIntoClientArea_hook(
    HWND           hWnd,
    const MARGINS *pMarInset
)
{
    if (settings.navbarglass)
    {
        WCHAR szClass[256];
        if (GetClassNameW(hWnd, szClass, 256)
        && 0 == wcscmp(szClass, L"CabinetWClass")
        && !ExplorerHasRibbon(hWnd))
        {
            HWND hNavBar = FindWindowExW(hWnd, NULL, L"WorkerW", NULL);
            if (hNavBar)
            {
                RECT rc = { 0 };
                GetWindowRect(hNavBar, &rc);
                MapWindowPoints(HWND_DESKTOP, hWnd, (LPPOINT)&rc, 2);

                if (pMarInset->cyTopHeight < rc.bottom)
                {
                    return S_OK;
                }
            }
        }
    }

    return DwmExtendFrameIntoClientArea_orig(
        hWnd, pMarInset
    );
}

#pragma endregion // "Other hooks"

#pragma endregion // "Hooks"

#define LOAD_INT_SETTING(NAME)    settings.NAME = Wh_GetIntSetting(L ## #NAME)
#define LOAD_STRING_SETTING(NAME) settings.NAME = WindhawkUtils::StringSetting::make(L ## #NAME)

/* == M O D == */
void LoadSettings(void)
{
    LOAD_INT_SETTING(alwayscpl);
    LOAD_INT_SETTING(smalladdress);
    LOAD_INT_SETTING(dropdownwidth);
    LOAD_INT_SETTING(refreshwidth);
    LOAD_INT_SETTING(oldsearch);
    LOAD_INT_SETTING(noribbon);
    LOAD_INT_SETTING(listview);
    LOAD_INT_SETTING(colheaders);
    LOAD_INT_SETTING(nopins);
    LOAD_INT_SETTING(navbarglass);
    LOAD_INT_SETTING(nocomposition);
    LOAD_INT_SETTING(hidetitle);
    LOAD_INT_SETTING(noup);
    LOAD_INT_SETTING(aerotravel);
#ifdef _WIN64
    LOAD_INT_SETTING(detailspane);
#endif
    LOAD_INT_SETTING(classicgrouping);
    LOAD_INT_SETTING(nopcfolders);
    LOAD_INT_SETTING(vistasearchplaceholder);
    LOAD_INT_SETTING(beta8navbarbg);
    LOAD_INT_SETTING(leftviews);
    LOAD_INT_SETTING(cmdbaricons);

    LPCWSTR szNpst = Wh_GetStringSetting(L"npst");
    if (0 == wcscmp(szNpst, L"vista"))
    {
        settings.npst = NPST_VISTA;
    }
    else if (0 == wcscmp(szNpst, L"seven"))
    {
        settings.npst = NPST_SEVEN;
    }
    else
    {
        settings.npst = NPST_DEFAULT;
    }
    Wh_FreeStringSetting(szNpst);

    LPCWSTR szTbst = Wh_GetStringSetting(L"tbst");
    if (0 == wcscmp(szTbst, L"seven"))
    {
        settings.tbst = TBST_SEVEN;
    }
    else if (0 == wcscmp(szTbst, L"ten"))
    {
        settings.tbst = TBST_TEN;
    }
    else if (0 == wcscmp(szTbst, L"tennew"))
    {
        settings.tbst = TBST_TENNEW;
    }
    else if (0 == wcscmp(szTbst, L"custom"))
    {
        settings.tbst = TBST_CUSTOM;
    }
    else
    {
        settings.tbst = TBST_DEFAULT;
    }
    Wh_FreeStringSetting(szTbst);
}

VS_FIXEDFILEINFO* GetModuleVersionInfo(HMODULE hModule, UINT *puPtrLen) 
{ 
    void *pFixedFileInfo = nullptr; 
    UINT uPtrLen = 0; 

    HRSRC hResource = 
        FindResourceW(hModule, MAKEINTRESOURCEW(VS_VERSION_INFO), RT_VERSION); 
    if (hResource)
    { 
        HGLOBAL hGlobal = LoadResource(hModule, hResource); 
        if (hGlobal)
        { 
            void *pData = LockResource(hGlobal); 
            if (pData)
            { 
                if (!VerQueryValueW(pData, L"\\", &pFixedFileInfo, &uPtrLen)
                || uPtrLen == 0)
                { 
                    pFixedFileInfo = nullptr; 
                    uPtrLen = 0; 
                } 
            } 
        } 
    } 

    if (puPtrLen)
    { 
        *puPtrLen = uPtrLen; 
    } 
  
     return (VS_FIXEDFILEINFO *)pFixedFileInfo; 
 } 

/**
  * Loads comctl32.dll, version 6.0.
  * This uses an activation context that uses shell32.dll's manifest
  * to load 6.0, even in apps which don't have the proper manifest for
  * it.
  */
HMODULE LoadComCtlModule(void)
{
    HMODULE hShell32 = LoadLibraryExW(L"shell32.dll", NULL, LOAD_LIBRARY_SEARCH_SYSTEM32);
    ACTCTXW actCtx = { sizeof(actCtx) };
    actCtx.dwFlags = ACTCTX_FLAG_RESOURCE_NAME_VALID | ACTCTX_FLAG_HMODULE_VALID;
    actCtx.lpResourceName = MAKEINTRESOURCEW(124);
    actCtx.hModule = hShell32;
    HANDLE hActCtx = CreateActCtxW(&actCtx);
    ULONG_PTR ulCookie;
    ActivateActCtx(hActCtx, &ulCookie);
    HMODULE hComCtl = LoadLibraryExW(L"comctl32.dll", NULL, LOAD_LIBRARY_SEARCH_SYSTEM32);
    /**
      * Certain processes will ignore the activation context and load
      * comctl32.dll 5.82 anyway. If that occurs, just reject it.
      */
    VS_FIXEDFILEINFO *pVerInfo = GetModuleVersionInfo(hComCtl, nullptr);
    if (!pVerInfo || HIWORD(pVerInfo->dwFileVersionMS) < 6)
    {
        FreeLibrary(hComCtl);
        hComCtl = NULL;
    }
    DeactivateActCtx(0, ulCookie);
    ReleaseActCtx(hActCtx);
    FreeLibrary(hShell32);
    return hComCtl;
}

const WindhawkUtils::SYMBOL_HOOK explorerframeDllHooks[] = {
    {
        {
            L"bool "
            SSTDCALL
            L" UseSeparateProcess(struct IShellItem *)"
        },
        &UseSeparateProcess_orig,
        UseSeparateProcess_hook,
        false
    },
    {
        {
            L"private: struct _GUID "
            STHISCALL
            L" CExplorerLauncher::GetHostFromTarget(struct _ITEMIDLIST_ABSOLUTE const *)"
        },
        &CExplorerLauncher_GetHostFromTarget_orig,
        CExplorerLauncher_GetHostFromTarget_hook,
        false
    },
    {
        {
            L"private: bool "
            STHISCALL
            L" CUniversalSearchBand::IsModernSearchBoxEnabled(void)"
        },
        &CUniversalSearchBand_IsModernSearchBoxEnabled_orig,
        CUniversalSearchBand_IsModernSearchBoxEnabled_hook,
        true
    },
    {
        {
            L"public: long "
            STHISCALL
            L" CSearchEditBox::HideSuggestions(void)"
        },
        &CSearchEditBox_HideSuggestions_orig,
        CSearchEditBox_HideSuggestions_hook,
        false
    },
    {
        {
            L"public: virtual long "
            SSTDCALL
            L" CSearchBox::SetCueAndTooltipText(unsigned short const *,unsigned short const *)"
        },
        &CSearchBox_SetCueAndTooltipText_orig,
        CSearchBox_SetCueAndTooltipText_hook,
        false
    },
    {
        {
            L"private: void "
            STHISCALL
            L" CAddressBand::_PositionChildWindows(void)"
        },
        &CAddressBand__PositionChildWindows_orig,
        CAddressBand__PositionChildWindows_hook,
        false
    },
    {
        {
#ifdef _WIN64
            L"private: __int64 __cdecl CAddressBand::_AddressBandWndProc(struct HWND__ *,unsigned int,unsigned __int64,__int64)"
#else
            L"private: long __thiscall CAddressBand::_AddressBandWndProc(struct HWND__ *,unsigned int,unsigned int,long)"
#endif
        },
        &CAddressBand__AddressBandWndProc_orig,
        CAddressBand__AddressBandWndProc_hook,
        false
    },
    {
        {
            L"protected: virtual void "
            STHISCALL
            L" CAddressList::_InitCombobox(void)"
        },
        &CAddressList__InitCombobox_orig,
        CAddressList__InitCombobox_hook,
        false
    },
    {
        {
            L"private: bool "
            STHISCALL
            L" CShellBrowser::_ShouldShowRibbon(struct IShellItem *)"
        },
        &CShellBrowser__ShouldShowRibbon_orig,
        CShellBrowser__ShouldShowRibbon_hook,
        false
    },
    {
        {
            L"private: struct HWND__ * "
            STHISCALL
            L" CNscTree::_CreateTreeview(struct HWND__ *)"
        },
        &CNscTree__CreateTreeview_orig,
        CNscTree__CreateTreeview_hook,
        false
    },
    {
        {
#ifdef _WIN64
            L"private: static __int64 __cdecl CNscTree::s_SubClassTreeWndProc(struct HWND__ *,unsigned int,unsigned __int64,__int64,unsigned __int64,unsigned __int64)"
#else
            L"private: static long __stdcall CNscTree::s_SubClassTreeWndProc(struct HWND__ *,unsigned int,unsigned int,long,unsigned int,unsigned long)"
#endif
        },
        &CNscTree_s_SubClassTreeWndProc_orig,
        CNscTree_s_SubClassTreeWndProc_hook,
        false
    },
    {
        {
            L"private: void "
            STHISCALL
            L" CNscTree::ScaleAndSetIndent(void)"
        },
        &CNscTree_ScaleAndSetIndent_orig,
        CNscTree_ScaleAndSetIndent_hook,
        false
    },
    {
        {
            L"protected: void "
            STHISCALL
            L" CNavBar::_SetTheme(void)"
        },
        &CNavBar__SetTheme_orig,
        CNavBar__SetTheme_hook,
        false
    },
    {
        {
            L"public: virtual long "
            SSTDCALL
            L" CSeparatorBand::GetBandInfo(unsigned long,unsigned long,struct DESKBANDINFO *)"
        },
        &CSeparatorBand_GetBandInfo_orig,
        CSeparatorBand_GetBandInfo_hook,
        false
    },
    {
        {
#ifdef _WIN64
            L"protected: static __int64 __cdecl CNavBar::s_SizableWndProc(struct HWND__ *,unsigned int,unsigned __int64,__int64)"
#else
            L"protected: static long __stdcall CNavBar::s_SizableWndProc(struct HWND__ *,unsigned int,unsigned int,long)"
#endif
        },
        &CNavBar_s_SizableWndProc_orig,
        CNavBar_s_SizableWndProc_hook,
        false
    },
    {
        {
            L"protected: void "
            STHISCALL
            L" CNavBar::_OnFrameStateChanged(unsigned long)"
        },
        &CNavBar__OnFrameStateChanged_orig,
        CNavBar__OnFrameStateChanged_hook,
        false
    },
    {
        {
            L"protected: virtual long "
            STHISCALL
            L" CNavBar::CNavBandSite::_Initialize(struct HWND__ *)"
        },
        &CNavBar_CNavBandSite__Initialize_orig,
        CNavBar_CNavBandSite__Initialize_hook,
        false
    },
    {
        {
            L"public: virtual long "
            SSTDCALL
            L" CUpBand::GetBandInfo(unsigned long,unsigned long,struct DESKBANDINFO *)"
        },
        &CUpBand_GetBandInfo_orig,
        CUpBand_GetBandInfo_hook,
        false
    },
    {
        {
            L"private: void "
            STHISCALL
            L" CExplorerFrame::_UpdateFrameState(void)"
        },
        &CExplorerFrame__UpdateFrameState_orig,
        CExplorerFrame__UpdateFrameState_hook,
        false
    },
    {
        {
            L"protected: long "
            STHISCALL
            L" CNavBar::ConstructNavBarThemeClassName(unsigned short * *)"
        },
        &CNavBar_ConstructNavBarThemeClassName_orig,
        CNavBar_ConstructNavBarThemeClassName_hook,
        false
    },
    {
        {
#ifdef _WIN64
            L"protected: static __int64 __cdecl CTravelBand::s_TravelWndProc(struct HWND__ *,unsigned int,unsigned __int64,__int64)"
#else
            L"protected: static long __stdcall CTravelBand::s_TravelWndProc(struct HWND__ *,unsigned int,unsigned int,long)"
#endif
        },
        &CTravelBand_s_TravelWndProc_orig,
        CTravelBand_s_TravelWndProc_hook,
        false
    },
    {
        {
            L"private: void "
            STHISCALL
            L" CTravelBand::_CreateTravelButtons(void)"
        },
        &CTravelBand__CreateTravelButtons_orig,
        CTravelBand__CreateTravelButtons_hook,
        false
    },
    {
        {
            L"private: void "
            STHISCALL
            L" CTravelBand::_SetButtonImagesClassicMode(void)"
        },
        &CTravelBand__SetButtonImagesClassicMode_orig,
        CTravelBand__SetButtonImagesClassicMode_hook,
        false
    },
    {
        {
            L"public: virtual long "
            SSTDCALL
            L" CTravelBand::GetBandInfo(unsigned long,unsigned long,struct DESKBANDINFO *)"
        },
        &CTravelBand_GetBandInfo_orig,
        CTravelBand_GetBandInfo_hook,
        false
    },
    {
        {
            L"private: void "
            STHISCALL
            L" CTravelBand::_SetControlState(int)"
        },
        &CTravelBand__SetControlState_orig,
        CTravelBand__SetControlState_hook,
        false
    },
    {
        {
            L"public: virtual long "
            SSTDCALL
            L" CNscTree::SetStateImageList(struct _IMAGELIST *)"
        },
        &CNscTree_SetStateImageList_orig,
        CNscTree_SetStateImageList_hook,
        false
    },
    {
        {
            L"public: virtual long "
            SSTDCALL
            L" CShellBrowser::GetFolderFlags(enum FOLDERFLAGS *,enum FOLDERFLAGS *)"
        },
        &CShellBrowser_GetFolderFlags_orig,
        CShellBrowser_GetFolderFlags_hook,
        false
    },
#ifdef _WIN64
    {
        {
            L"private: void "
            SSTDCALL
            L" CShellBrowser::_OnRibbonVisibilityChange(enum CShellBrowser::ONRIBBONVISIBILITY_CHANGE)"
        },
        &CShellBrowser___OnRibbonVisibilityChange_orig,
        CShellBrowser___OnRibbonVisibilityChange_hook,
        false
    }
#endif
};

const WindhawkUtils::SYMBOL_HOOK shell32DllHooks[] = {
    {
        {
            L"private: int "
            STHISCALL
            L" CDefView::_UseItemsView(void)"
        },
        &CDefView__UseItemsView_orig,
        CDefView__UseItemsView_hook,
        false
    },
#ifdef _WIN64
    {
        {
            L"long __cdecl DUI_LoadUIFileFromResources(struct HINSTANCE__ *,unsigned int,unsigned short * *)"
        },
        &DUI_LoadUIFileFromResources_orig,
        DUI_LoadUIFileFromResources_hook,
        false
    },
#endif
    {
        {
            L"public: virtual long "
            SSTDCALL
            L" CStorageSystemTypeCategorizer::GetCategory(unsigned int,struct _ITEMID_CHILD const "
#ifdef _WIN64
            L"__unaligned "
#endif
            L"* const *,unsigned long *)"
        },
        &CStorageSystemTypeCategorizer_GetCategory_orig,
        CStorageSystemTypeCategorizer_GetCategory_hook,
        false
    },
    {
        {
            L"public: virtual long "
            SSTDCALL
            L" CStorageSystemTypeCategorizer::CompareCategory(enum CATSORT_FLAGS,unsigned long,unsigned long)"
        },
        &CStorageSystemTypeCategorizer_CompareCategory_orig,
        CStorageSystemTypeCategorizer_CompareCategory_hook,
        false
    },
    {
        {
            L"public: virtual long "
            SSTDCALL
            L" CStorageSystemTypeCategorizer::GetCategoryInfo(unsigned long,struct CATEGORY_INFO *)"
        },
        &CStorageSystemTypeCategorizer_GetCategoryInfo_orig,
        CStorageSystemTypeCategorizer_GetCategoryInfo_hook,
        false
    },
    {
        {
            L"enum FOLDERFLAGS "
            SSTDCALL
            L" IViewSettings_GetFolderFlags(struct IViewSettings *)"
        },
        &IViewSettings_GetFolderFlags_orig,
        IViewSettings_GetFolderFlags_hook,
        false
    },
    {
        {
            L"int "
            SSTDCALL
            " IsRightSideCommand(struct _GUID const &)"
        },
        &IsRightSideCommand_orig,
        IsRightSideCommand_hook,
        false
    },
    {
        {
            L"private: long "
            STHISCALL
            " CCommandFolder::_AppendRightSideCommands(struct IUnknown *)"
        },
        &CCommandFolder__AppendRightSideCommands_orig,
        CCommandFolder__AppendRightSideCommands_hook,
        false
    },
    {
        {
            L"private: long "
            STHISCALL
            " CCommandFolder::_AppendOrganizeCommand(struct IUnknown *)"
        },
        &CCommandFolder__AppendOrganizeCommand_orig,
        CCommandFolder__AppendOrganizeCommand_hook,
        false
    },
    {
        {
            L"private: void "
            STHISCALL
            L" CSplitButton::UpdateIcon(int)"
        },
        &CSplitButton_UpdateIcon_orig,
        CSplitButton_UpdateIcon_hook,
        false
    },
    {
        {
            L"private: void "
            STHISCALL
            L" CSplitButton::_UpdateDisplay(struct ExplorerCommandItem const *)"
        },
        &CSplitButton__UpdateDisplay_orig,
        CSplitButton__UpdateDisplay_hook,
        false
    }
};

// windows.storage.dll
const WindhawkUtils::SYMBOL_HOOK windowsStorageHooks[] = {
    {
        {
            L"public: long "
            STHISCALL
            L" CObjectArray::AddItemsFromKeySkip(struct HKEY__ *,unsigned short const *,unsigned long,struct _GUID const &)"
        },
        &CObjectArray_AddItemsFromKeySkip_orig,
        CObjectArray_AddItemsFromKeySkip_hook,
        false
    }
};

#define LOAD_MODULE_(module, varName)                                                          \
    HMODULE varName = LoadLibraryExW(L ## #module ".dll", NULL, LOAD_LIBRARY_SEARCH_SYSTEM32); \
    if (!varName)                                                                              \
    {                                                                                          \
        Wh_Log(L"Failed to load " #module ".dll");                                             \
        return FALSE;                                                                          \
    }

#define LOAD_MODULE(name) LOAD_MODULE_(name, name)

#define LOAD_FUNCTION_(moduleVar, module, name)                           \
    name = (name ## _t)GetProcAddress(moduleVar, #name);                  \
    if (!name)                                                            \
    {                                                                     \
        Wh_Log(L"Failed to get address of " #name " in " #module ".dll"); \
        return false;                                                     \
    }

#define LOAD_FUNCTION(module, name) LOAD_FUNCTION_(module, module, name)

#define HOOK_SYMBOLS_(module, moduleVar, hooks)                                    \
    if (!WindhawkUtils::HookSymbols(                                               \
        moduleVar,                                                                 \
        hooks,                                                                     \
        ARRAYSIZE(hooks)                                                           \
    ))                                                                             \
    {                                                                              \
        Wh_Log(L"Failed to hook one or more symbol functions in " #module ".dll"); \
        return FALSE;                                                              \
    }

#define HOOK_SYMBOLS(module, hooks) HOOK_SYMBOLS_(module, module, hooks)

#define HOOK_FUNCTION(function)               \
    if (!Wh_SetFunctionHook(                  \
        (void *)function,                     \
        (void *)function ## _hook,            \
        (void **)&function ## _orig           \
    ))                                        \
    {                                         \
        Wh_Log(L"Failed to hook " #function); \
        return FALSE;                         \
    }

BOOL Wh_ModInit(void)
{
    LoadSettings();

    LOAD_MODULE(ntdll)
    LOAD_FUNCTION(ntdll, RtlQueryFeatureConfiguration)

    LOAD_MODULE(propsys)
    LOAD_FUNCTION(propsys, VariantToBuffer)

    LOAD_MODULE(ExplorerFrame)
    VS_FIXEDFILEINFO *pVerInfo = GetModuleVersionInfo(ExplorerFrame, nullptr);
    if (!pVerInfo || HIWORD(pVerInfo->dwFileVersionLS) > 21332)
    {
        Wh_Log(L"Rejecting invalid or Windows 11 ExplorerFrame.dll");
        return FALSE;
    }
    HOOK_SYMBOLS(ExplorerFrame, explorerframeDllHooks)

    LOAD_MODULE(shell32)
    HOOK_SYMBOLS(shell32, shell32DllHooks)

    LOAD_MODULE_(windows.storage, windows_storage)
    HOOK_SYMBOLS_(windows.storage, windows_storage, windowsStorageHooks)

    HMODULE comctl32 = LoadComCtlModule();
    if (!comctl32)
    {
        Wh_Log(L"Failed to load comctl32.dll");
        return FALSE;
    }

    ImageList_LoadImageW_t ImageList_LoadImageW;
    LOAD_FUNCTION(comctl32, ImageList_LoadImageW)
    HOOK_FUNCTION(ImageList_LoadImageW)

    HOOK_FUNCTION(GetSystemMetricsForDpi)
    HOOK_FUNCTION(OpenThemeData)
    HOOK_FUNCTION(DrawThemeParentBackground)

    LOAD_MODULE(dwmapi)
    DwmExtendFrameIntoClientArea_t DwmExtendFrameIntoClientArea;
    LOAD_FUNCTION(dwmapi, DwmExtendFrameIntoClientArea)
    HOOK_FUNCTION(DwmExtendFrameIntoClientArea)

    return TRUE;
}

void Wh_ModUninit(void)
{
    size_t len = g_subclassedRebars.size();
    for (size_t i = 0; i < len; i++)
    {
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(
            g_subclassedRebars[i],
            RebarSubclassWndProc
        );
    }

    len = g_subclassedTravelbands.size();
    for (size_t i = 0; i < len; i++)
    {
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(
            g_subclassedTravelbands[i],
            TravelBandToolbarSubclassProc
        );
    }
}

void Wh_ModSettingsChanged(void)
{
    LoadSettings();
}
