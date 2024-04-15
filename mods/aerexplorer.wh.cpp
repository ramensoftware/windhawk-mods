// ==WindhawkMod==
// @id              aerexplorer
// @name            Aerexplorer
// @description     Various tweaks for Windows Explorer to make it more like older versions.
// @version         1.5.9
// @author          aubymori
// @github          https://github.com/aubymori
// @include         *
// @compilerOptions -lgdi32 -lcomctl32 -lole32 -loleaut32 -luxtheme -ldwmapi
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
  $name: Force Control Panel mode
  $description: Make Explorer think it's always Control Panel. This will fix Control Panel reopening Explorer but it will also enable old search and may be buggy.
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
  $description: Always show the column headers that normally only show in Details view, like Windows Vista. Use native list view must be enabled for this to work.
- npst: seven
  $name: Navigation pane style
  $description: Determines the style of the tree view on the left of the window.
  $options:
  - default: Default
  - vista: Windows Vista
  - seven: Windows 7/8
- nopins: true
  $name: Remove pins from Quick access
  $description: Remove the pins that appear on the right of pinned items in Quick access on the navigation pane
- navbarglass: true
  $name: Glass on navigation bar
  $description: Show glass on the navigation bar, like Windows Vista and 7. Enabling this option will also fix the bug where NavBar classes in MSSTYLES are swapped.
- nocomposition: false
  $name: Disable composition
  $description: Do not extend frames with navbar glass.
- hidetitle: true
  $name: Hide window title and icon
  $description: Hide the window title and icon on Explorer windows like Windows 7 and before.
- noup: true
  $name: No up button
  $description: Remove the "Up" button like Windows Vista and 7.
- aerotravel: true
  $name: Aero travel buttons
  $description: Restores the Windows Vista/7 style for the back and forward buttons.
- detailspane: true
  $name: Details pane on bottom
  $description: Put the details pane on bottom, like Windows Vista and 7. You can hide the status bar natively through the View tab in the Folder Options dialog.
- classicgrouping: true
  $name: Classic drive grouping
  $description: Split drives and devices into separate groups (Hard Disk Drives, Devices with Removable Storage, etc.) like Windows 8.0 and before.
- nopcfolders: true
  $name: Remove This PC folders
  $description: Remove the folders from the This PC folder, like Windows 8.0 and before
- vistasearchplaceholder: false
  $name: Windows Vista search box placeholder
  $description: Makes the search box always say just "Search", like it did in Windows Vista.
*/
// ==/WindhawkModSettings==

// Defines data shared by all instances of the library, even across processes.
#define SHARED_SECTION __attribute__((section(".shared")))
asm(".section .shared,\"dws\"\n");

#include <windhawk_utils.h>
#include <initguid.h>
#include <propvarutil.h>
#include <shobjidl.h>
#include <shlguid.h>
#include <shlobj.h>
#include <dwmapi.h>
#include <vssym32.h>

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
} settings = { 0 };

#define UseNavbarGlass() (settings.navbarglass && settings.noribbon)

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

typedef struct tagDRIVEGROUPI18N
{
    LANGID lid;
    WCHAR szHardDisks[256];
    WCHAR szRemovable[256];
    WCHAR szOther[256];
    WCHAR szScanners[256];
    WCHAR szPortableMedia[256];
    WCHAR szPortable[256];
} DRIVEGROUPI18N;

typedef const DRIVEGROUPI18N *LPCDRIVEGROUPI18N;

typedef enum _DRIVEGROUP
{
    DG_HARDDISKS = 0,
    DG_REMOVABLE,
    DG_OTHER,
    DG_SCANNERS,
    DG_PORTABLEMEDIA,
    DG_PORTABLE
} DRIVEGROUP;

#pragma region "Drive grouping localization"
const DRIVEGROUPI18N g_driveGroupI18n[] = {
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

LPCDRIVEGROUPI18N GetCurrentDriveLocale(void)
{
    LANGID lid = GetUserDefaultLangID();

    /* So we can fallback to English without iterating again. */
    LPCDRIVEGROUPI18N en = NULL;

    for (int i = 0; i < ARRAYSIZE(g_driveGroupI18n); i++)
    {
        if (g_driveGroupI18n[i].lid == MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US))
        {
            en = g_driveGroupI18n + i;
        }

        if (g_driveGroupI18n[i].lid == lid)
        {
            return g_driveGroupI18n + i;
        }
    }

    return en;
}

/* Only available in Windows 10, version 1607 and later. */
typedef UINT (WINAPI *GetDpiForSystem_t)(void);
GetDpiForSystem_t GetDpiForSystem = nullptr;

#define ScaleForDPI(n) MulDiv(n, GetDpiForSystem(), 96)
#define IsHighDPI() (GetDpiForSystem() >= 120)

typedef BOOL (WINAPI *ShouldAppsUseDarkMode_t)(void);
ShouldAppsUseDarkMode_t ShouldAppsUseDarkMode = nullptr;

typedef BOOL (WINAPI *AllowDarkModeForWindow_t)(HWND, BOOL);
AllowDarkModeForWindow_t AllowDarkModeForWindow = nullptr;

#pragma region "Hooks"

#pragma region "ExplorerFrame.dll hooks"

#pragma region "Old search box"
/* Old search box */
void *CUniversalSearchBand_IsModernSearchBoxEnabled_addr SHARED_SECTION = nullptr;
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
void *CSearchEditBox_HideSuggestions_addr SHARED_SECTION = nullptr;
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
void *CSearchBox_SetCueAndTooltipText_addr SHARED_SECTION = nullptr;
HRESULT (STDCALL *CSearchBox_SetCueAndTooltipText_orig)(void *, LPCWSTR, LPCWSTR) = nullptr;
HRESULT STDCALL CSearchBox_SetCueAndTooltipText_hook(
    void    *pThis,
    LPCWSTR  lpszText,
    LPCWSTR  lpszUnused
)
{
    if (settings.vistasearchplaceholder)
    {
        lpszText = NULL;
    }
    return CSearchBox_SetCueAndTooltipText_orig(
        pThis, lpszText, lpszUnused
    );
}

#pragma region "Small address bar"

#define CAddressBand_Window(pThis) *((HWND *)pThis + 9)

/* Fix address bar position and toolbar size */
void *CAddressBand__PositionChildWindows_addr SHARED_SECTION = nullptr;
void (THISCALL *CAddressBand__PositionChildWindows_orig)(void *) = nullptr;
void THISCALL CAddressBand__PositionChildWindows_hook(
    void *pThis
)
{
    HWND hWnd = CAddressBand_Window(pThis);
    if (settings.tbst != TBST_DEFAULT && hWnd && IsWindow(hWnd))
    {
        HWND hProg = FindWindowExW(hWnd, NULL, L"msctls_progress32", NULL);
        if (hProg)
        {
            HWND hToolbar = FindWindowExW(hProg, NULL, L"ToolbarWindow32", NULL);
            if (hToolbar)
            {
                TBBUTTONINFOW tbi;
                tbi.cbSize = sizeof(TBBUTTONINFOW);
                tbi.dwMask = TBIF_SIZE;

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
                SendMessageW(hToolbar, TB_SETBUTTONINFOW, 202, (LPARAM)&tbi);

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
                /* Go button (same as refresh) */
                SendMessageW(hToolbar, TB_SETBUTTONINFOW, 100, (LPARAM)&tbi);

                /* Stop button (same as refresh) */
                SendMessageW(hToolbar, TB_SETBUTTONINFOW, 101, (LPARAM)&tbi);

                /* Actual refresh button */
                SendMessageW(hToolbar, TB_SETBUTTONINFOW, 102, (LPARAM)&tbi);                    
            }
        }
    }

    return CAddressBand__PositionChildWindows_orig(pThis);
}

DWORD g_dwSubBuild SHARED_SECTION = 0;

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

void *CAddressBand__AddressBandWndProc_addr SHARED_SECTION = nullptr;
LRESULT (THISCALL *CAddressBand__AddressBandWndProc_orig)(void *, HWND, UINT, WPARAM, LPARAM) = nullptr;
LRESULT THISCALL CAddressBand__AddressBandWndProc_hook(
    void   *pThis,
    HWND    hWnd,
    UINT    uMsg,
    WPARAM  wParam,
    LPARAM  lParam
)
{
    if (settings.smalladdress && uMsg == WM_WINDOWPOSCHANGING && ModernSearchFeatureEnabled())
    {
        LPWINDOWPOS lpwp = (LPWINDOWPOS)lParam;
        HWND hwSearch = FindWindowExW(GetParent(hWnd), NULL, L"UniversalSearchBand", NULL);
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
        pThis, hWnd, uMsg, wParam, lParam
    );
}

#ifdef _WIN64
#   define CAddressList_ComboBox(pThis) *((HWND *)pThis + 3)
#else
#   define CAddressList_ComboBox(pThis) *((HWND *)pThis + 4)
#endif

void *CAddressList__InitCombobox_addr SHARED_SECTION = nullptr;
void (THISCALL *CAddressList__InitCombobox_orig)(void *) = nullptr;
void THISCALL CAddressList__InitCombobox_hook(
    void *pThis
)
{
    CAddressList__InitCombobox_orig(pThis);
    if (settings.smalladdress)
    {
        HWND hWnd = CAddressList_ComboBox(pThis);
        if (hWnd)
        {
            SendMessageW(hWnd, 0x40E, 0, 8);
        }
    }
}
#pragma endregion // "Small address bar"

/* Disable ribbon */
void *CShellBrowser__ShouldShowRibbon_addr SHARED_SECTION = nullptr;
bool (THISCALL *CShellBrowser__ShouldShowRibbon_orig)(void *, void *) = nullptr;
bool THISCALL CShellBrowser__ShouldShowRibbon_hook(
    void *pThis,
    void *pShellItem
)
{
    return settings.noribbon
    ? false
    : CShellBrowser__ShouldShowRibbon_orig(
        pThis, pShellItem
    );
}

#pragma region "Old navigation pane sizing"

/* Allow non-even heights */
void *CNscTree__CreateTreeview_addr SHARED_SECTION = nullptr;
HWND (THISCALL *CNscTree__CreateTreeview_orig)(void *, HWND) = nullptr;
HWND THISCALL CNscTree__CreateTreeview_hook(
    void *pThis,
    HWND  hWnd
)
{
    HWND hRes = CNscTree__CreateTreeview_orig(
        pThis, hWnd
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

#ifdef _WIN64
#define CNscTree_Window(pThis) *((HWND *)pThis + 47)
#else
#define CNscTree_Window(pThis) *((HWND *)pThis + 58)
#endif

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
void *CNscTree_s_SubClassTreeWndProc_addr SHARED_SECTION = nullptr;
SUBCLASSPROC CNscTree_s_SubClassTreeWndProc_orig = nullptr;
LRESULT CALLBACK CNscTree_s_SubClassTreeWndProc_hook(
    HWND      hWnd, 
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
        HWND hPar = GetParent(GetParent(hWnd));
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
                            HTREEITEM hItem = (HTREEITEM)DefSubclassProc(hWnd, uMsg, wParam, lParam);
                            lptvi->hItem = hItem;
                            /* The TVM_SETITEMW handle here already calls HandleTvItem, no need to here */
                            TreeView_SetItem(hWnd, lptvi);
                            if (hItem)
                            {
                                return (LRESULT)hItem;
                            }
                            break;
                        }
                        case TVM_SETITEMW:
                        {
                            HandleTvItem(hWnd, (LPTVITEMEXW)lParam);
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
        hWnd, uMsg, wParam, lParam, uIdSubclass, dwRefData
    );
}

#pragma endregion

#pragma region "Navbar glass"

#define CNavBar_Window(pThis) *((HWND *)pThis + 6)
#define CNavBar_ThemeWindow(pThis) *((HWND *)pThis + 73)

/* Re-impl of function from Windows 7 */
void CNavBar__UpdateGlass(void *pThis)
{
    HWND hWnd = CNavBar_Window(pThis);
    if (hWnd && IsWindow(hWnd) && IsCompositionActive() && !settings.nocomposition)
    {
        RECT rc;
        GetClientRect(hWnd, &rc);
        MARGINS mrg;
        SecureZeroMemory(&mrg, sizeof(MARGINS));
        mrg.cyTopHeight = rc.bottom;
        HWND hExplorer = GetAncestor(hWnd, GA_ROOT);
        if (hExplorer)
        {
            DwmExtendFrameIntoClientArea(
                hExplorer, &mrg
            );
        }
    }
    else
    {
        HWND hExplorer = GetAncestor(hWnd, GA_ROOT);
        if (hExplorer)
        {
            MARGINS mrg;
            SecureZeroMemory(&mrg, sizeof(MARGINS));
            DwmExtendFrameIntoClientArea(
                hExplorer, &mrg
            );
        }
    }
}

void *CNavBar__SetTheme_addr SHARED_SECTION = nullptr;
void (THISCALL *CNavBar__SetTheme_orig)(void *) = nullptr;
void THISCALL CNavBar__SetTheme_hook(
    void *pThis
)
{
    CNavBar__SetTheme_orig(pThis);
    if (UseNavbarGlass())
    {
        CNavBar__UpdateGlass(pThis);
    }
}

/* Nuke separator bands */
void *CNavBar__AddSpacerBand_addr SHARED_SECTION = nullptr;
HRESULT (THISCALL *CNavBar__AddSpacerBand_orig)(void *, int, int) = nullptr;
HRESULT THISCALL CNavBar__AddSpacerBand_hook(
    void *pThis,
    int   nIndex,
    int   nWidth
)
{
    if (settings.navbarglass && nIndex == 5)
    {
        return S_OK;
    }

    return CNavBar__AddSpacerBand_orig(
        pThis,
        nIndex,
        settings.navbarglass ? 0 : nWidth
    );
}

void *CNavBar_s_SizableWndProc_addr SHARED_SECTION = nullptr;
WNDPROC CNavBar_s_SizableWndProc_orig = nullptr;
LRESULT CALLBACK CNavBar_s_SizableWndProc_hook(
    HWND   hWnd,
    UINT   uMsg,
    WPARAM wParam,
    LPARAM lParam
)
{
    LRESULT res = CNavBar_s_SizableWndProc_orig(
        hWnd, uMsg, wParam, lParam
    );

    if (UseNavbarGlass() && uMsg == WM_SIZE)
    {
        void *pThis = (void *)GetWindowLongPtrW(hWnd, 0);
        if (pThis)
        {
            CNavBar__UpdateGlass(pThis);
        }
    }

    return res;
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
    if (UseNavbarGlass() && IsCompositionActive() && !settings.nocomposition && (uMsg == WM_ERASEBKGND || uMsg == WM_PAINT))
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

#define CNavBar_CNavBandSite_Window(pThis) *((HWND *)pThis + 17)

void *CNavBar_CNavBandSite__Initialize_addr SHARED_SECTION = nullptr;
HRESULT (THISCALL *CNavBar_CNavBandSite__Initialize_orig)(void *, HWND) = nullptr;
HRESULT THISCALL CNavBar_CNavBandSite__Initialize_hook(
    void *pThis,
    HWND  hWndParent
)
{
    HRESULT hr = CNavBar_CNavBandSite__Initialize_orig(
        pThis, hWndParent
    );

    if (UseNavbarGlass() && SUCCEEDED(hr))
    {
        HWND hWnd = CNavBar_CNavBandSite_Window(pThis);
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

void *CNavBar__OnFrameStateChanged_addr SHARED_SECTION = nullptr;
void (THISCALL *CNavBar__OnFrameStateChanged_orig)(void *, DWORD) = nullptr;
void THISCALL CNavBar__OnFrameStateChanged_hook(
    void  *pThis,
    DWORD dwUnknown
)
{
    CNavBar__OnFrameStateChanged_orig(
        pThis, dwUnknown
    );
    if (UseNavbarGlass())
    {
        CNavBar__UpdateGlass(pThis);
    }
}

/* Hide window icon and caption */
#define CExplorerFrame_Window(pThis) *((HWND *)pThis + 1)

void *CExplorerFrame__UpdateFrameState_addr SHARED_SECTION = nullptr;
void (THISCALL *CExplorerFrame__UpdateFrameState_orig)(void *) = nullptr;
void THISCALL CExplorerFrame__UpdateFrameState_hook(
    void *pThis
)
{
    CExplorerFrame__UpdateFrameState_orig(pThis);
    if (settings.hidetitle)
    {
        HWND hWnd = CExplorerFrame_Window(pThis);
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

void *CNavBar_ConstructNavBarThemeClassName_addr SHARED_SECTION = nullptr;
HRESULT (THISCALL *CNavBar_ConstructNavBarThemeClassName_orig)(void *, WCHAR **) = nullptr;
HRESULT THISCALL CNavBar_ConstructNavBarThemeClassName_hook(
    void   *pThis,
    WCHAR **out
)
{
    HRESULT hr = CNavBar_ConstructNavBarThemeClassName_orig(
        pThis, out
    );

    if (settings.navbarglass && SUCCEEDED(hr))
    {
        BOOL bDarkMode = FALSE;
        if (wcsstr(*out, L"DarkMode"))
        {
            bDarkMode = TRUE;
            wcscpy(*out, *out + 8);
        }

        if (wcsstr(*out, L"MaxInactive"))
        {
            wcscpy(*out + 3, *out + 11);
        }
        else if (wcsstr(*out, L"Inactive"))
        {
            wcscpy(*out, *out + 8);
        }
        else if (wcsstr(*out, L"Max"))
        {
            WCHAR szClass[256] = L"MaxInactive";
            wcscat(szClass, *out + 3);
            
            CoTaskMemFree(*out);
            *out = (WCHAR *)CoTaskMemAlloc((wcslen(szClass) + 1) * sizeof(WCHAR));
            wcscpy(*out, szClass);
        }
        else
        {
            WCHAR szClass[256] = L"Inactive";
            wcscat(szClass, *out);
            
            CoTaskMemFree(*out);
            *out = (WCHAR *)CoTaskMemAlloc((wcslen(szClass) + 1) * sizeof(WCHAR));
            wcscpy(*out, szClass);
        }

        if (bDarkMode)
        {
            WCHAR szClass[256] = L"DarkMode";
            wcscat(szClass, *out);

            CoTaskMemFree(*out);
            *out = (WCHAR *)CoTaskMemAlloc((wcslen(szClass) + 1) * sizeof(WCHAR));
            wcscpy(*out, szClass);
        }

        LPWSTR pszComposited = wcsstr(*out, L"Composited");
        if (settings.nocomposition && pszComposited)
        {
            *pszComposited = L'\0';
        }
    }

    return hr;
}

#pragma endregion // "Navbar glass"

/* Disable Up button */
void *CUpBand_GetBandInfo_addr SHARED_SECTION = nullptr;
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
#define CTravelBand_Toolbar(pThis) *((HWND *)pThis + 16)
#else
#define CTravelBand_Toolbar(pThis) *((HWND *)pThis + 18)
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
#define CTravelBand_himl(pThis) *((HIMAGELIST *)pThis + 31)
#define CTravelBand_state(pThis) *((DWORD *)pThis + 48)
#else
#define CTravelBand_himl(pThis) *((HIMAGELIST *)pThis + 40)
#define CTravelBand_state(pThis) *((DWORD *)pThis + 26)
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

        HIMAGELIST himl = CTravelBand_himl(pThis);
        if (himl)
        {
            ImageList_Draw(
                himl, CTravelBand_state(pThis), ps.hdc, settings.noribbon ? 1 : 5, 2, ILD_TRANSPARENT
            );
        }

        if (ps.fRestore)
        {
            EndPaint(hWnd, &ps);
        }
    }
    return 0;
}

void *CTravelBand_s_TravelWndProc_addr SHARED_SECTION = nullptr;
WNDPROC CTravelBand_s_TravelWndProc_orig = nullptr;
LRESULT CALLBACK CTravelBand_s_TravelWndProc_hook(
    HWND   hWnd,
    UINT   uMsg,
    WPARAM wParam,
    LPARAM lParam
)
{
    if (settings.aerotravel && (uMsg == WM_PAINT || uMsg == WM_PRINTCLIENT))
    {
        return DrawTravelBackground(hWnd, uMsg, wParam, lParam);
    }

    return CTravelBand_s_TravelWndProc_orig(
        hWnd, uMsg, wParam, lParam
    );
}

std::vector<HWND> g_subclassedTravelbands;

/* Fix spacing shit */
LRESULT CALLBACK TravelBandToolbarSubclassProc(
    HWND      hWnd,
    UINT      uMsg,
    WPARAM    wParam,
    LPARAM    lParam,
    DWORD_PTR dwRefData
)
{
    if (settings.aerotravel)
    {
        switch (uMsg)
        {
            case TB_SETBUTTONSIZE:
                return DefSubclassProc(hWnd, TB_SETBUTTONSIZE, 0, 0);
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
void *CTravelBand__CreateTravelButtons_addr SHARED_SECTION = nullptr;
void (THISCALL *CTravelBand__CreateTravelButtons_orig)(void *) = nullptr;
void THISCALL CTravelBand__CreateTravelButtons_hook(
    void *pThis
)
{
    CTravelBand__CreateTravelButtons_orig(pThis);
    HWND hWnd = CTravelBand_Toolbar(pThis);
    if (hWnd && IsWindow(hWnd))
    {
        if (WindhawkUtils::SetWindowSubclassFromAnyThread(hWnd, TravelBandToolbarSubclassProc, NULL))
        {
            g_subclassedTravelbands.push_back(hWnd);
        }
    }
}

/* Fix position and add pressed state for aero travel buttons */
void *CTravelBand__SetButtonImagesClassicMode_addr SHARED_SECTION = nullptr;
void (THISCALL *CTravelBand__SetButtonImagesClassicMode_orig)(void *) = nullptr;
void THISCALL CTravelBand__SetButtonImagesClassicMode_hook(
    void *pThis
)
{
    CTravelBand__SetButtonImagesClassicMode_orig(pThis);

    HWND hWnd = CTravelBand_Toolbar(pThis);
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

void *CTravelBand_GetBandInfo_addr SHARED_SECTION = nullptr;
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
#   define CTravelBand_OpeningMenu(pThis) *((DWORD *)pThis + 61)
#else
#   define CTravelBand_OpeningMenu(pThis) *((DWORD *)pThis + 39)
#endif

void *CTravelBand__SetControlState_addr SHARED_SECTION = nullptr;
void (THISCALL *CTravelBand__SetControlState_orig)(void *, int) = nullptr;
void THISCALL CTravelBand__SetControlState_hook(
    void *pThis,
    int   iStateId
)
{
    if (CTravelBand_OpeningMenu(pThis))
    {
        iStateId = 3;
    }

    CTravelBand__SetControlState_orig(
        pThis, iStateId
    );
}

/* Remove pins from Quick access */
void *CNscTree_SetStateImageList_addr SHARED_SECTION = nullptr;
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
void *CShellBrowser_GetFolderFlags_addr SHARED_SECTION = nullptr;
HRESULT (STDCALL *CShellBrowser_GetFolderFlags_orig)(void *, FOLDERFLAGS *, FOLDERFLAGS *);
HRESULT STDCALL CShellBrowser_GetFolderFlags_hook(
    void        *pThis,
    FOLDERFLAGS *pfolderMask,
    FOLDERFLAGS *pfolderFlags
)
{
    Wh_Log(L"getfolderflags");
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

/* Force Control Panel mode */
void *CExplorerLauncher_GetHostFromTarget_addr SHARED_SECTION = nullptr;
GUID *(THISCALL *CExplorerLauncher_GetHostFromTarget_orig)(void *, GUID *, LPCITEMIDLIST) = nullptr;
GUID *THISCALL CExplorerLauncher_GetHostFromTarget_hook(
    void          *pThis,
    GUID          *out,
    LPCITEMIDLIST  pItemIds
)
{
    if (settings.alwayscpl)
    {
        *out = CLSID_ControlPanelProcessExplorerHost;
        return out;
    }

    return CExplorerLauncher_GetHostFromTarget_orig(
        pThis, out, pItemIds
    );
}

#pragma endregion // "Explorerframe.dll hooks"

#pragma region "shell32.dll hooks"

#ifdef _WIN64
#   define CDefView_Window(pThis) *((HWND *)pThis + 79)
#else
#   define CDefView_Window(pThis) *((HWND *)pThis + 89)
#endif

/* Enable native list view */
void *CDefView__UseItemsView_addr SHARED_SECTION = nullptr;
BOOL (THISCALL *CDefView__UseItemsView_orig)(void *) = nullptr;
BOOL THISCALL CDefView__UseItemsView_hook(
    void *pThis
)
{
    if (settings.listview)
    {
        /* Fix for StartIsBack because I fucking hate Tihiy */
        HWND hWnd = CDefView_Window(pThis);
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
void *DUI_LoadUIFileFromResources_addr SHARED_SECTION = nullptr;
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

void *CStorageSystemTypeCategorizer_GetCategory_addr SHARED_SECTION = nullptr;
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

void *CStorageSystemTypeCategorizer_CompareCategory_addr SHARED_SECTION = nullptr;
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

void *CStorageSystemTypeCategorizer_GetCategoryInfo_addr SHARED_SECTION = nullptr;
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
        LPCDRIVEGROUPI18N dgi = GetCurrentDriveLocale();
        LPCWSTR lpszOut = NULL;

        if (dgi)
        {
            switch ((DRIVEGROUP)dwCategoryId)
            {
                case DG_HARDDISKS:
                    lpszOut = dgi->szHardDisks;
                    break;
                case DG_REMOVABLE:
                    lpszOut = dgi->szRemovable;
                    break;
                case DG_OTHER:
                    lpszOut = dgi->szOther;
                    break;
                case DG_SCANNERS:
                    lpszOut = dgi->szScanners;
                    break;
                case DG_PORTABLEMEDIA:
                    lpszOut = dgi->szPortableMedia;
                    break;
                case DG_PORTABLE:
                    lpszOut = dgi->szPortable;
                    break;
            }

            if (lpszOut)
            {
                wcscpy(pci->wszName, lpszOut);
            }
        }
    }
    return hr;
}

/* Always show column headers (SysListView32) */
void *IViewSettings_GetFolderFlags_addr SHARED_SECTION = nullptr;
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

#pragma endregion // "shell32.dll hooks"

#pragma region "windows.storage.dll hooks"

/* Remove This PC folders */
void *CObjectArray_AddItemsFromKeySkip_addr SHARED_SECTION = nullptr;
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
    if (UseNavbarGlass() && IsCompositionActive() && !settings.nocomposition)
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

#pragma endregion // "Other hooks"

#pragma endregion // "Hooks"

#define LoadIntSetting(NAME)    settings.NAME = Wh_GetIntSetting(L ## #NAME)
#define LoadStringSetting(NAME) settings.NAME = WindhawkUtils::StringSetting::make(L ## #NAME)

/* == M O D == */
void LoadSettings(void)
{
    LoadIntSetting(alwayscpl);
    LoadIntSetting(smalladdress);
    LoadIntSetting(dropdownwidth);
    LoadIntSetting(refreshwidth);
    LoadIntSetting(oldsearch);
    LoadIntSetting(noribbon);
    LoadIntSetting(listview);
    LoadIntSetting(colheaders);
    LoadIntSetting(nopins);
    LoadIntSetting(navbarglass);
    LoadIntSetting(nocomposition);
    LoadIntSetting(hidetitle);
    LoadIntSetting(noup);
    LoadIntSetting(aerotravel);
#ifdef _WIN64
    LoadIntSetting(detailspane);
#endif
    LoadIntSetting(classicgrouping);
    LoadIntSetting(nopcfolders);
    LoadIntSetting(vistasearchplaceholder);

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

#ifdef _WIN64
#   define PATHCACHE_VALNAME L"last-comctl32-v6-path"
#else
#   define PATHCACHE_VALNAME L"last-comctl32-v6-path-wow64"
#endif

#define COMCTL_582_SEARCH    L"microsoft.windows.common-controls_6595b64144ccf1df_5.82"

/* Load the ComCtl32 module */
HMODULE LoadComCtlModule(void)
{
    HMODULE hComCtl = LoadLibraryW(L"comctl32.dll");
    if (!hComCtl)
    {
        return NULL;
    }

    WCHAR szPath[MAX_PATH];
    GetModuleFileNameW(hComCtl, szPath, MAX_PATH);

    WCHAR szv6Path[MAX_PATH];
    BOOL bNoCache = FALSE;
    if (!Wh_GetStringValue(PATHCACHE_VALNAME, szv6Path, MAX_PATH))
    {
        bNoCache = TRUE;
    }

    /**
      * the !bNoCache check here is nested because we only want to fall through
      * to the cacher if the current comctl32 path is NOT 5.82.
      */
    if (wcsstr(szPath, COMCTL_582_SEARCH)
    || wcsstr(szPath, L"\\Windows\\System32")
    || wcsstr(szPath, L"\\Windows\\SysWOW64"))
    {
        if (!bNoCache)
        {
            hComCtl = LoadLibraryW(szv6Path);
        }
    }
    else if (bNoCache || wcsicmp(szPath, szv6Path))
    {
        Wh_SetStringValue(PATHCACHE_VALNAME, szPath);
    }

    return hComCtl;
}

struct CMWF_SYMBOL_HOOK {
    std::vector<std::wstring> symbols;
    void** pOriginalFunction;
    void* hookFunction = nullptr;
    bool optional = false;
    void **pSharedMemoryCache = nullptr;

    template <typename Prototype>
    CMWF_SYMBOL_HOOK(
            std::vector<std::wstring> symbols,
            Prototype **originalFunction,
            std::type_identity_t<Prototype *> hookFunction = nullptr,
            bool optional = false,
            void *pSharedMemoryCache = nullptr
    ) : symbols(std::move(symbols)),
        pOriginalFunction((void **)originalFunction),
        hookFunction((void *)hookFunction),
        optional(optional),
        pSharedMemoryCache((void **)pSharedMemoryCache) {}

    CMWF_SYMBOL_HOOK() = default;
};

/*
 * CmwfHookSymbols: A custom hook wrapper which allows storing symbol hook results in the
 *                  module's shared memory for faster access.
 */
bool CmwfHookSymbols(
        HMODULE module,
        const CMWF_SYMBOL_HOOK *symbolHooks,
        size_t symbolHooksCount
)
{
    bool anyUncachedHooks = false;
    std::vector<void **> proxyAddresses(symbolHooksCount);
    std::vector<WindhawkUtils::SYMBOL_HOOK> proxyHooks;
    
    for (size_t i = 0; i < symbolHooksCount; i++)
    {
        // Just gotta ensure the memory is initialized as null :P
        proxyAddresses[i] = nullptr;

        void *address = nullptr;
        if (symbolHooks[i].pSharedMemoryCache && *(symbolHooks[i].pSharedMemoryCache) != NULL)
        {
            address = *(symbolHooks[i].pSharedMemoryCache);
            if (address == nullptr)
            {
                continue;
            }
            Wh_Log(
                L"CmwfHookSymbols: Hooking symbol %.*s from in-memory cache.",
                symbolHooks[i].symbols[0].length(),
                symbolHooks[i].symbols[0].data()
            );
            Wh_SetFunctionHook(
                address,
                symbolHooks[i].hookFunction,
                symbolHooks[i].pOriginalFunction
            );
        }
        else
        {
            address = nullptr;
            anyUncachedHooks = true;

            WindhawkUtils::SYMBOL_HOOK hook = {
                symbolHooks[i].symbols,
                &proxyAddresses[i],
                NULL,
                symbolHooks[i].optional
            };

            proxyHooks.push_back(hook);
        }
    }

    if (anyUncachedHooks)
    {
        if (!WindhawkUtils::HookSymbols(module, proxyHooks.data(), proxyHooks.size()))
        {
            return false;
        }

        int curProxyHook = 0;
        for (void *address : proxyAddresses)
        {
            if (address == NULL)
            {
                // WARNING: You must increment this value before every next
                // iteration, or else it will crash.
                curProxyHook++;
                continue;
            }

            if (
                symbolHooks[curProxyHook].pSharedMemoryCache && 
                *(symbolHooks[curProxyHook].pSharedMemoryCache) == NULL
            )
            {
                *(symbolHooks[curProxyHook].pSharedMemoryCache) = address;
            }

            if (symbolHooks[curProxyHook].hookFunction && symbolHooks[curProxyHook].pOriginalFunction)
            {
                Wh_SetFunctionHook(
                    address,
                    symbolHooks[curProxyHook].hookFunction,
                    symbolHooks[curProxyHook].pOriginalFunction
                );
            }

            // Make sure to increment before the next iteration.
            curProxyHook++;
        }
    }

    return true;
}

BOOL Wh_ModInit(void)
{
    LoadSettings();

    HMODULE hNtDll = LoadLibraryW(L"ntdll.dll");
    if (hNtDll)
    {
        RtlQueryFeatureConfiguration = (RtlQueryFeatureConfiguration_t)GetProcAddress(hNtDll, "RtlQueryFeatureConfiguration");
    }

    HMODULE hUxTheme = LoadLibraryW(L"uxtheme.dll");
    if (hUxTheme)
    {
        ShouldAppsUseDarkMode = (ShouldAppsUseDarkMode_t)GetProcAddress(hUxTheme, (LPCSTR)132);
        AllowDarkModeForWindow = (AllowDarkModeForWindow_t)GetProcAddress(hUxTheme, (LPCSTR)133);
    }

    HMODULE hPropsys = LoadLibraryW(L"propsys.dll");
    if (hPropsys)
    {
        VariantToBuffer = (VariantToBuffer_t)GetProcAddress(hPropsys, "VariantToBuffer");
    }

    HMODULE hExplorerFrame = LoadLibraryW(L"ExplorerFrame.dll");
    if (!hExplorerFrame)
    {
        Wh_Log(L"Failed to load ExplorerFrame.dll");
        return FALSE;
    }

    /* This needs to be hooked EARLY. */
    const CMWF_SYMBOL_HOOK isCplHooks[] = {
        {
            {
                L"private: struct _GUID "
                STHISCALL
                L" CExplorerLauncher::GetHostFromTarget(struct _ITEMIDLIST_ABSOLUTE const *)"
            },
            &CExplorerLauncher_GetHostFromTarget_orig,
            CExplorerLauncher_GetHostFromTarget_hook,
            false,
            &CExplorerLauncher_GetHostFromTarget_addr
        }
    };
        

    if (!CmwfHookSymbols(
        hExplorerFrame,
        isCplHooks,
        ARRAYSIZE(isCplHooks)
    ))
    {
        Wh_Log(L"Failed to hook either CExplorerLauncher::GetHostFromTarget");
        return FALSE;
    }

    const CMWF_SYMBOL_HOOK efHooks[] = {
        {
            {
                L"private: bool "
                STHISCALL
                L" CUniversalSearchBand::IsModernSearchBoxEnabled(void)"
            },
            &CUniversalSearchBand_IsModernSearchBoxEnabled_orig,
            CUniversalSearchBand_IsModernSearchBoxEnabled_hook,
            true,
            &CUniversalSearchBand_IsModernSearchBoxEnabled_addr
        },
        {
            {
                L"public: long "
                STHISCALL
                L" CSearchEditBox::HideSuggestions(void)"
            },
            &CSearchEditBox_HideSuggestions_orig,
            CSearchEditBox_HideSuggestions_hook,
            false,
            &CSearchEditBox_HideSuggestions_addr
        },
        {
            {
                L"public: virtual long "
                SSTDCALL
                L" CSearchBox::SetCueAndTooltipText(unsigned short const *,unsigned short const *)"
            },
            &CSearchBox_SetCueAndTooltipText_orig,
            CSearchBox_SetCueAndTooltipText_hook,
            false,
            &CSearchBox_SetCueAndTooltipText_addr
        },
        {
            {
                L"private: void "
                STHISCALL
                L" CAddressBand::_PositionChildWindows(void)"
            },
            &CAddressBand__PositionChildWindows_orig,
            CAddressBand__PositionChildWindows_hook,
            false,
            &CAddressBand__PositionChildWindows_addr
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
            false,
            &CAddressBand__AddressBandWndProc_addr
        },
        {
            {
                L"protected: virtual void "
                STHISCALL
                L" CAddressList::_InitCombobox(void)"
            },
            &CAddressList__InitCombobox_orig,
            CAddressList__InitCombobox_hook,
            false,
            &CAddressList__InitCombobox_addr
        },
        {
            {
                L"private: bool "
                STHISCALL
                L" CShellBrowser::_ShouldShowRibbon(struct IShellItem *)"
            },
            &CShellBrowser__ShouldShowRibbon_orig,
            CShellBrowser__ShouldShowRibbon_hook,
            false,
            &CShellBrowser__ShouldShowRibbon_addr
        },
        {
            {
                L"private: struct HWND__ * "
                STHISCALL
                L" CNscTree::_CreateTreeview(struct HWND__ *)"
            },
            &CNscTree__CreateTreeview_orig,
            CNscTree__CreateTreeview_hook,
            false,
            &CNscTree__CreateTreeview_addr
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
            false,
            &CNscTree_s_SubClassTreeWndProc_addr
        },
        {
            {
                L"protected: void "
                STHISCALL
                L" CNavBar::_SetTheme(void)"
            },
            &CNavBar__SetTheme_orig,
            CNavBar__SetTheme_hook,
            false,
            &CNavBar__SetTheme_addr
        },
        {
            {
                L"protected: long "
                STHISCALL
                L" CNavBar::_AddSpacerBand(enum CNavBar::NavigationBands,unsigned long)"
            },
            &CNavBar__AddSpacerBand_orig,
            CNavBar__AddSpacerBand_hook,
            false,
            &CNavBar__AddSpacerBand_addr
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
            false,
            &CNavBar_s_SizableWndProc_addr
        },
        {
            {
                L"protected: void "
                STHISCALL
                L" CNavBar::_OnFrameStateChanged(unsigned long)"
            },
            &CNavBar__OnFrameStateChanged_orig,
            CNavBar__OnFrameStateChanged_hook,
            false,
            &CNavBar__OnFrameStateChanged_addr
        },
        {
            {
                L"protected: virtual long "
                STHISCALL
                L" CNavBar::CNavBandSite::_Initialize(struct HWND__ *)"
            },
            &CNavBar_CNavBandSite__Initialize_orig,
            CNavBar_CNavBandSite__Initialize_hook,
            false,
            &CNavBar_CNavBandSite__Initialize_addr
        },
        {
            {
                L"public: virtual long "
                SSTDCALL
                L" CUpBand::GetBandInfo(unsigned long,unsigned long,struct DESKBANDINFO *)"
            },
            &CUpBand_GetBandInfo_orig,
            CUpBand_GetBandInfo_hook,
            false,
            &CUpBand_GetBandInfo_addr
        },
        {
            {
                L"private: void "
                STHISCALL
                L" CExplorerFrame::_UpdateFrameState(void)"
            },
            &CExplorerFrame__UpdateFrameState_orig,
            CExplorerFrame__UpdateFrameState_hook,
            false,
            &CExplorerFrame__UpdateFrameState_addr
        },
        {
            {
                L"protected: long "
                STHISCALL
                L" CNavBar::ConstructNavBarThemeClassName(unsigned short * *)"
            },
            &CNavBar_ConstructNavBarThemeClassName_orig,
            CNavBar_ConstructNavBarThemeClassName_hook,
            false,
            &CNavBar_ConstructNavBarThemeClassName_addr
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
            false,
            &CTravelBand_s_TravelWndProc_addr
        },
        {
            {
                L"private: void "
                STHISCALL
                L" CTravelBand::_CreateTravelButtons(void)"
            },
            &CTravelBand__CreateTravelButtons_orig,
            CTravelBand__CreateTravelButtons_hook,
            false,
            &CTravelBand__CreateTravelButtons_addr
        },
        {
            {
                L"private: void "
                STHISCALL
                L" CTravelBand::_SetButtonImagesClassicMode(void)"
            },
            &CTravelBand__SetButtonImagesClassicMode_orig,
            CTravelBand__SetButtonImagesClassicMode_hook,
            false,
            &CTravelBand__SetButtonImagesClassicMode_addr
        },
        {
            {
                L"public: virtual long "
                SSTDCALL
                L" CTravelBand::GetBandInfo(unsigned long,unsigned long,struct DESKBANDINFO *)"
            },
            &CTravelBand_GetBandInfo_orig,
            CTravelBand_GetBandInfo_hook,
            false,
            &CTravelBand_GetBandInfo_addr
        },
        {
            {
                L"private: void "
                STHISCALL
                L" CTravelBand::_SetControlState(int)"
            },
            &CTravelBand__SetControlState_orig,
            CTravelBand__SetControlState_hook,
            false,
            &CTravelBand__SetControlState_addr
        },
        {
            {
                L"public: virtual long "
                SSTDCALL
                L" CNscTree::SetStateImageList(struct _IMAGELIST *)"
            },
            &CNscTree_SetStateImageList_orig,
            CNscTree_SetStateImageList_hook,
            false,
            &CNscTree_SetStateImageList_addr
        },
        {
            {
                L"public: virtual long "
                SSTDCALL
                L" CShellBrowser::GetFolderFlags(enum FOLDERFLAGS *,enum FOLDERFLAGS *)"
            },
            &CShellBrowser_GetFolderFlags_orig,
            CShellBrowser_GetFolderFlags_hook,
            false,
            &CShellBrowser_GetFolderFlags_addr
        }
    };

    if (!CmwfHookSymbols(
        hExplorerFrame,
        efHooks,
        ARRAYSIZE(efHooks)
    ))
    {
        Wh_Log(L"Failed to hook one or more symbol functions in ExplorerFrame.dll");
        return FALSE;
    }

    HMODULE hShell32 = LoadLibraryW(L"shell32.dll");
    if (!hShell32)
    {
        Wh_Log(L"Failed to load shell32.dll");
        return FALSE;
    }

    const CMWF_SYMBOL_HOOK shHooks[] = {
        {
            {
                L"private: int "
                STHISCALL
                L" CDefView::_UseItemsView(void)"
            },
            &CDefView__UseItemsView_orig,
            CDefView__UseItemsView_hook,
            false,
            &CDefView__UseItemsView_addr
        },
#ifdef _WIN64
        {
            {
                L"long __cdecl DUI_LoadUIFileFromResources(struct HINSTANCE__ *,unsigned int,unsigned short * *)"
            },
            &DUI_LoadUIFileFromResources_orig,
            DUI_LoadUIFileFromResources_hook,
            false,
            &DUI_LoadUIFileFromResources_addr
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
            false,
            &CStorageSystemTypeCategorizer_GetCategory_addr
        },
        {
            {
                L"public: virtual long "
                SSTDCALL
                L" CStorageSystemTypeCategorizer::CompareCategory(enum CATSORT_FLAGS,unsigned long,unsigned long)"
            },
            &CStorageSystemTypeCategorizer_CompareCategory_orig,
            CStorageSystemTypeCategorizer_CompareCategory_hook,
            false,
            &CStorageSystemTypeCategorizer_CompareCategory_addr
        },
        {
            {
                L"public: virtual long "
                SSTDCALL
                L" CStorageSystemTypeCategorizer::GetCategoryInfo(unsigned long,struct CATEGORY_INFO *)"
            },
            &CStorageSystemTypeCategorizer_GetCategoryInfo_orig,
            CStorageSystemTypeCategorizer_GetCategoryInfo_hook,
            false,
            &CStorageSystemTypeCategorizer_GetCategoryInfo_addr
        },
        {
            {
                L"enum FOLDERFLAGS "
                SSTDCALL
                L" IViewSettings_GetFolderFlags(struct IViewSettings *)"
            },
            &IViewSettings_GetFolderFlags_orig,
            IViewSettings_GetFolderFlags_hook,
            false,
            &IViewSettings_GetFolderFlags_addr
        }
    };

    if (!CmwfHookSymbols(
        hShell32,
        shHooks,
        ARRAYSIZE(shHooks)
    ))
    {
        Wh_Log(L"Failed to hook one or more symbol functions in shell32.dll");
        return FALSE;
    }

    HMODULE hStorage = LoadLibraryW(L"windows.storage.dll");
    if (!hStorage)
    {
        Wh_Log(L"Failed to load windwos.storage.dll");
        return FALSE;
    }

    const CMWF_SYMBOL_HOOK storageHooks[] = {
        {
            {
                L"public: long "
                STHISCALL
                L" CObjectArray::AddItemsFromKeySkip(struct HKEY__ *,unsigned short const *,unsigned long,struct _GUID const &)"
            },
            &CObjectArray_AddItemsFromKeySkip_orig,
            CObjectArray_AddItemsFromKeySkip_hook,
            false,
            &CObjectArray_AddItemsFromKeySkip_addr
        }
    };

    if (!CmwfHookSymbols(
        hStorage,
        storageHooks,
        ARRAYSIZE(storageHooks)
    ))
    {
        Wh_Log(L"Failed to hook one or more symbol functions in windows.storage.dll");
        return FALSE;
    }

    HMODULE hUser32 = LoadLibraryW(L"user32.dll");
    if (!hUser32)
    {
        Wh_Log(L"Failed to load user32.dll");
        return FALSE;
    }

    GetDpiForSystem = (GetDpiForSystem_t)GetProcAddress(hUser32, "GetDpiForSystem");

    HMODULE hComCtl = LoadComCtlModule();
    if (!hComCtl)
    {
        Wh_Log(L"Failed to load comctl32.dll");
        return FALSE;
    }

    if (!WindhawkUtils::Wh_SetFunctionHookT(
        (ImageList_LoadImageW_t)GetProcAddress(hComCtl, "ImageList_LoadImageW"),
        ImageList_LoadImageW_hook,
        &ImageList_LoadImageW_orig
    ))
    {
        Wh_Log(L"Failed to hook ImageList_LoadImageW");
        return FALSE;
    }

    if (!WindhawkUtils::Wh_SetFunctionHookT(
        (GetSystemMetricsForDpi_t)GetProcAddress(hUser32, "GetSystemMetricsForDpi"),
        GetSystemMetricsForDpi_hook,
        &GetSystemMetricsForDpi_orig
    ))
    {
        Wh_Log(L"Failed to hook GetSystemMetricsForDpi");
        return FALSE;
    }

    if (!WindhawkUtils::Wh_SetFunctionHookT(
        OpenThemeData,
        OpenThemeData_hook,
        &OpenThemeData_orig
    ))
    {
        Wh_Log(L"Failed to hook OpenThemeData");
        return FALSE;
    }

    if (!WindhawkUtils::Wh_SetFunctionHookT(
        DrawThemeParentBackground,
        DrawThemeParentBackground_hook,
        &DrawThemeParentBackground_orig
    ))
    {
        Wh_Log(L"Failed to hook DrawThemeParentBackground");
        return FALSE;
    }

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
