// ==WindhawkMod==
// @id              icon-customizer
// @name            Icon Customizer
// @description     Allows you to override almost 100 built-in Windows icons.
// @version         0.1
// @author          FireBlade
// @github          https://github.com/FireBlade211
// @include         *
// @compilerOptions -lshell32 -lshlwapi
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Icon Customizer
Allows you to override almost 100 Windows icons to anything you like.

## Override an icon
To change an icon, open the mod settings and change the icons you like by entering the path to an ICO, EXE, or DLL file.
Optionally you can also specify an index, for example:

```
C:\Windows\System32\shell32.dll,32
```

## How it works
This mod adds a hook for `SHGetStockIconInfo`, which Windows apps call whenever they need to retrieve a stock icon. It then
 checks if the icon for the specified ID is overriden and gives Windows the new icon.

## Note
This mod will only work on apps that load icons themselves, and will not work on built-in Windows dialogs such as message boxes.
This is because those dialogs do not utilize the `SHGetStockIconInfo` function.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
# Here you can define settings, in YAML format, that the mod users will be able
# to configure. Metadata values such as $name and $description are optional.
# Check out the documentation for more information:
# https://github.com/ramensoftware/windhawk/wiki/Creating-a-new-mod#settings
- DOCNOASSOC: ""
  $name: Document (No Association)
  $description: "Icon: Document of a type with no associated application."
- DOCASSOC: ""
  $name: Document (With Association)
  $description: "Icon: Document of a type with an associated application."
- APPLICATION: ""
  $name: Application
  $description: "Icon: Generic application with no custom icon."
- FOLDER: ""
  $name: Folder
  $description: "Icon: Folder (generic, unspecified state)."
- FOLDEROPEN: ""
  $name: Folder (Open)
  $description: "Icon: Folder (open)."
- DRIVE525: ""
  $name: 5.25 drive
  $description: "Icon: 5.25-inch disk drive."
- DRIVE35: ""
  $name: 3.5 drive
  $description: "Icon: 3.5-inch disk drive."
- DRIVEREMOVE: ""
  $name: Removable Drive
  $description: "Icon: Removable drive."
- DRIVEFIXED: ""
  $name: Fixed Drive
  $description: "Icon: Fixed drive (hard disk)."
- DRIVENET: ""
  $name: Network Drive
  $description: "Icon: Network drive (connected)."
- DRIVENETDISABLED: ""
  $name: Disconnected Network Drive
  $description: "Icon: Network drive (disconnected)."
- DRIVECD: ""
  $name: CD Drive
  $description: "Icon: CD drive."
- DRIVERAM: ""
  $name: RAM Disk
  $description: "Icon: RAM disk drive."
- WORLD: ""
  $name: World
  $description: "Icon: The entire network."
- SERVER: ""
  $name: Server
  $description: "Icon: A computer on the network."
- PRINTER: ""
  $name: Printer
  $description: "Icon: A local printer or print destination."
- MYNETWORK: ""
  $name: Network
  $description: "Icon: The Network virtual folder."
- FIND: ""
  $name: Find
  $description: "Icon: The Find feature."
- HELP: ""
  $name: Help
  $description: "Icon: The Help feature."
- SHARE: ""
  $name: Share
  $description: "Icon: Overlay for a shared item."
- LINK: ""
  $name: Link
  $description: "Icon: Overlay for a shortcut."
- SLOWFILE: ""
  $name: Slow File
  $description: "Icon: Overlay for items that are expected to be slow to access."
- RECYCLER: ""
  $name: Recycler
  $description: "Icon: The Recycle Bin (empty)."
- RECYCLERFULL: ""
  $name: Recycler (Full)
  $description: "Icon: The Recycle Bin (not empty)."
- MEDIACDAUDIO: ""
  $name: Audio CD
  $description: "Icon: Audio CD media."
- LOCK: ""
  $name: Lock
  $description: "Icon: Security lock."
- AUTOLIST: ""
  $name: Auto-List
  $description: "Icon: A virtual folder that contains the results of a search."
- PRINTERNET: ""
  $name: Network Printer
  $description: "Icon: A network printer."
- SERVERSHARE: ""
  $name: Server Share
  $description: "Icon: A server shared on a network."
- PRINTERFAX: ""
  $name: Fax Printer (Local)
  $description: "Icon: A local fax printer."
- PRINTERFAXNET: ""
  $name: Fax Printer (Network)
  $description: "Icon: A network fax printer."
- PRINTERFILE: ""
  $name: Printer File
  $description: "Icon: A file that receives the output of a operation."
- STACK: ""
  $name: Stack
  $description: "Icon: A category that results from a command to organize the contents of a folder."
- MEDIASVCD: ""
  $name: SVCD Media
  $description: "Icon: Super Video CD (SVCD) media."
- STUFFEDFOLDER: ""
  $name: Stuffed Folder
  $description: "Icon: A folder that contains only subfolders as child items."
- DRIVEUNKNOWN: ""
  $name: Unknown Drive
  $description: "Icon: Unknown drive type."
- DRIVEDVD: ""
  $name: DVD Drive
  $description: "Icon: DVD drive."
- MEDIADVD: ""
  $name: DVD Media
  $description: "Icon: DVD media."
- MEDIADVDRAM: ""
  $name: DVD-RAM Media
  $description: "Icon: DVD-RAM media."
- MEDIADVDRW: ""
  $name: DVD-RW Media
  $description: "Icon: DVD-RW media."
- MEDIADVDR: ""
  $name: DVD-R Media
  $description: "Icon: DVD-R media."
- MEDIADVDROM: ""
  $name: DVD-ROM Media
  $description: "Icon: DVD-ROM media."
- MEDIACDAUDIOPLUS: ""
  $name: CD+ Media
  $description: "Icon: CD+ (enhanced audio CD) media."
- MEDIACDRW: ""
  $name: CD-RW Media
  $description: "Icon: CD-RW media."
- MEDIACDR: ""
  $name: CD-R Media
  $description: "Icon: CD-R media."
- MEDIACDBURN: ""
  $name: CD (Burning)
  $description: "Icon: A writable CD in the process of being burned."
- MEDIABLANKCD: ""
  $name: CD (Blank)
  $description: "Icon: Blank writable CD media."
- MEDIACDROM: ""
  $name: CD-ROM Media
  $description: "Icon: CD-ROM media."
- AUDIOFILES: ""
  $name: Audio File
  $description: "Icon: An audio file."
- IMAGEFILES: ""
  $name: Image File
  $description: "Icon: An image file."
- VIDEOFILES: ""
  $name: Video File
  $description: "Icon: A video file."
- MIXEDFILES: ""
  $name: Mixed File
  $description: "Icon: A mixed file."
- FOLDERBACK: ""
  $name: Folder Back
  $description: "Icon: Folder back."
- FOLDERFRONT: ""
  $name: Folder Front
  $description: "Icon: Folder front."
- SHIELD: ""
  $name: Shield
  $description: "Icon: Security shield."
- WARNING: ""
  $name: Warning
  $description: "Icon: Warning."
- INFO: ""
  $name: Info
  $description: "Icon: Informational."
- iERROR: ""
  $name: Error
  $description: "Icon: Error."
- KEY: ""
  $name: Key
  $description: "Icon: Key."
- SOFTWARE: ""
  $name: Software
  $description: "Icon: Software."
- RENAME: ""
  $name: Rename
  $description: "Icon: A UI item, such as a button, that issues a rename command."
- iDELETE: ""
  $name: Delete
  $description: "Icon: A UI item, such as a button, that issues a delete command."
- MEDIAAUDIODVD: ""
  $name: Audio DVD Media
  $description: "Icon: Audio DVD media."
- MEDIAMOVIEDVD: ""
  $name: Movie DVD Media
  $description: "Icon: Movie DVD media."
- MEDIAENHANCEDCD: ""
  $name: Enhanced CD Media
  $description: "Icon: Enhanced CD media."
- MEDIAENHANCEDDVD: ""
  $name: Enhanced DVD Media
  $description: "Icon: Enhanced DVD media."
- MEDIAHDDVD: ""
  $name: HD-DVD Media
  $description: "Icon: High definition DVD media in the HD DVD format."
- MEDIABLURAY: ""
  $name: Blu-Ray Media
  $description: "Icon: High definition DVD media in the Blu-ray Discâ„¢ format."
- MEDIAVCD: ""
  $name: VCD Media
  $description: "Icon: Video CD (VCD) media."
- MEDIADVDPLUSR: ""
  $name: DVD+R Media
  $description: "Icon: DVD+R media."
- MEDIADVDPLUSRW: ""
  $name: DVD+RW Media
  $description: "Icon: DVD+RW media."
- DESKTOPPC: ""
  $name: Desktop PC
  $description: "Icon: A desktop computer."
- MOBILEPC: ""
  $name: Mobile PC
  $description: "Icon: A mobile computer (laptop)."
- USERS: ""
  $name: Users
  $description: "Icon: The Users Control Panel item."
- MEDIASMARTMEDIA: ""
  $name: Smart Media
  $description: "Icon: Smart media."
- MEDIACOMPACTFLASH: ""
  $name: Compact Flash
  $description: "Icon: CompactFlash media."
- DEVICECELLPHONE: ""
  $name: Cell Phone
  $description: "Icon: A cell phone."
- DEVICECAMERA: ""
  $name: Device Camera
  $description: "Icon: A digital camera."
- DEVICEVIDEOCAMERA: ""
  $name: Device Video Camera
  $description: "Icon: A digital video camera."
- DEVICEAUDIOPLAYER: ""
  $name: Device Audio Player
  $description: "Icon: An audio player."
- NETWORKCONNECT: ""
  $name: Network Connect
  $description: "Icon: Connect to network."
- INTERNET: ""
  $name: Internet
  $description: "Icon: The Internet Control Panel item."
- ZIPFILE: ""
  $name: ZIP File
  $description: "Icon: A compressed file with a .zip file name extension."
- SETTINGS: ""
  $name: Settings
  $description: "Icon: The Control Panel item."
- DRIVEHDDVD: ""
  $name: HD-DVD Drive
  $description: "Icon: High definition DVD drive (any type - HD DVD-ROM, HD DVD-R, HD-DVD-RAM) that uses the HD DVD format."
- DRIVEBD: ""
  $name: Blu-ray Disc Drive
  $description: "Icon: High definition DVD drive (any type - BD-ROM, BD-R, BD-RE) that uses the Blu-ray Disc format."
- MEDIAHDDVDROM: ""
  $name: HD DVD-ROM Media
  $description: "Icon: High definition DVD-ROM media in the HD DVD-ROM format."
- MEDIAHDDVDR: ""
  $name: HD DVD-R Media
  $description: "Icon: High definition DVD-R media in the HD DVD-R format."
- MEDIAHDDVDRAM: ""
  $name: HD DVD-RAM Media
  $description: "Icon: High definition DVD-RAM media in the HD DVD-RAM format."
- MEDIABDROM: ""
  $name: Blu-ray Disc BD-ROM Media
  $description: "Icon: High definition DVD-ROM media in the Blu-ray Disc BD-ROM format."
- MEDIABDR: ""
  $name: Blu-ray Disc BD-R Media
  $description: "Icon: High definition write-once media in the Blu-ray Disc BD-R format."
- MEDIABDRE: ""
  $name: Blu-ray Disc BD-RE Media
  $description: "Icon: High definition read/write media in the Blu-ray Disc BD-RE format."
- CLUSTEREDDRIVE: ""
  $name: Clustered Drive
  $description: "Icon: A cluster disk array."
*/
// ==/WindhawkModSettings==

// The source code of the mod starts here. This sample was inspired by the great
// article of Kyle Halladay, X64 Function Hooking by Example:
// http://kylehalladay.com/blog/2020/11/13/Hooking-By-Example.html
// If you're new to terms such as code injection and function hooking, the
// article is great to get started.

#include <shellapi.h>
#include <shlwapi.h>

struct {
    LPCWSTR DOCNOASSOC;
    LPCWSTR DOCASSOC;
    LPCWSTR APPLICATION;
    LPCWSTR FOLDER;
    LPCWSTR FOLDEROPEN;
    LPCWSTR DRIVE525;
    LPCWSTR DRIVE35;
    LPCWSTR DRIVEREMOVE;
    LPCWSTR DRIVEFIXED;
    LPCWSTR DRIVENET;
    LPCWSTR DRIVENETDISABLED;
    LPCWSTR DRIVECD;
    LPCWSTR DRIVERAM;
    LPCWSTR WORLD;
    LPCWSTR SERVER;
    LPCWSTR PRINTER;
    LPCWSTR MYNETWORK;
    LPCWSTR FIND;
    LPCWSTR HELP;
    LPCWSTR SHARE;
    LPCWSTR LINK;
    LPCWSTR SLOWFILE;
    LPCWSTR RECYCLER;
    LPCWSTR RECYCLERFULL;
    LPCWSTR MEDIACDAUDIO;
    LPCWSTR LOCK;
    LPCWSTR AUTOLIST;
    LPCWSTR PRINTERNET;
    LPCWSTR SERVERSHARE;
    LPCWSTR PRINTERFAX;
    LPCWSTR PRINTERFAXNET;
    LPCWSTR PRINTERFILE;
    LPCWSTR STACK;
    LPCWSTR MEDIASVCD;
    LPCWSTR STUFFEDFOLDER;
    LPCWSTR DRIVEUNKNOWN;
    LPCWSTR DRIVEDVD;
    LPCWSTR MEDIADVD;
    LPCWSTR MEDIADVDRAM;
    LPCWSTR MEDIADVDRW;
    LPCWSTR MEDIADVDR;
    LPCWSTR MEDIADVDROM;
    LPCWSTR MEDIACDAUDIOPLUS;
    LPCWSTR MEDIACDRW;
    LPCWSTR MEDIACDR;
    LPCWSTR MEDIACDBURN;
    LPCWSTR MEDIABLANKCD;
    LPCWSTR MEDIACDROM;
    LPCWSTR AUDIOFILES;
    LPCWSTR IMAGEFILES;
    LPCWSTR VIDEOFILES;
    LPCWSTR MIXEDFILES;
    LPCWSTR FOLDERBACK;
    LPCWSTR FOLDERFRONT;
    LPCWSTR SHIELD;
    LPCWSTR WARNING;
    LPCWSTR INFO;
    LPCWSTR iERROR;
    LPCWSTR KEY;
    LPCWSTR SOFTWARE;
    LPCWSTR RENAME;
    LPCWSTR iDELETE;
    LPCWSTR MEDIAAUDIODVD;
    LPCWSTR MEDIAMOVIEDVD;
    LPCWSTR MEDIAENHANCEDCD;
    LPCWSTR MEDIAENHANCEDDVD;
    LPCWSTR MEDIAHDDVD;
    LPCWSTR MEDIABLURAY;
    LPCWSTR MEDIAVCD;
    LPCWSTR MEDIADVDPLUSR;
    LPCWSTR MEDIADVDPLUSRW;
    LPCWSTR DESKTOPPC;
    LPCWSTR MOBILEPC;
    LPCWSTR USERS;
    LPCWSTR MEDIASMARTMEDIA;
    LPCWSTR MEDIACOMPACTFLASH;
    LPCWSTR DEVICECELLPHONE;
    LPCWSTR DEVICECAMERA;
    LPCWSTR DEVICEVIDEOCAMERA;
    LPCWSTR DEVICEAUDIOPLAYER;
    LPCWSTR NETWORKCONNECT;
    LPCWSTR INTERNET;
    LPCWSTR ZIPFILE;
    LPCWSTR SETTINGS;
    LPCWSTR DRIVEHDDVD;
    LPCWSTR DRIVEBD;
    LPCWSTR MEDIAHDDVDROM;
    LPCWSTR MEDIAHDDVDR;
    LPCWSTR MEDIAHDDVDRAM;
    LPCWSTR MEDIABDROM;
    LPCWSTR MEDIABDR;
    LPCWSTR MEDIABDRE;
    LPCWSTR CLUSTEREDDRIVE;
} settings;

using SHGetStockIconInfo_t = decltype(&SHGetStockIconInfo);
SHGetStockIconInfo_t SHGetStockIconInfo_Original;

WINAPI HRESULT SHGetStockIconInfo_Hook(
            SHSTOCKICONID   siid,
            UINT            uFlags,
            SHSTOCKICONINFO *psii
)
{
    LPCWSTR iconString = NULL;

    switch (siid)
    {
        case SIID_DOCNOASSOC:
            iconString = settings.DOCNOASSOC;
            break;
        case SIID_DOCASSOC:
            iconString = settings.DOCASSOC;
            break;
        case SIID_APPLICATION:
            iconString = settings.APPLICATION;
            break;
        case SIID_FOLDER:
            iconString = settings.FOLDER;
            break;
        case SIID_FOLDEROPEN:
            iconString = settings.FOLDEROPEN;
            break;
        case SIID_DRIVE525:
            iconString = settings.DRIVE525;
            break;
        case SIID_DRIVE35:
            iconString = settings.DRIVE35;
            break;
        case SIID_DRIVEREMOVE:
            iconString = settings.DRIVEREMOVE;
            break;
        case SIID_DRIVEFIXED:
            iconString = settings.DRIVEFIXED;
            break;
        case SIID_DRIVENET:
            iconString = settings.DRIVENET;
            break;
        case SIID_DRIVENETDISABLED:
            iconString = settings.DRIVENETDISABLED;
            break;
        case SIID_DRIVECD:
            iconString = settings.DRIVECD;
            break;
        case SIID_DRIVERAM:
            iconString = settings.DRIVERAM;
            break;
        case SIID_WORLD:
            iconString = settings.WORLD;
            break;
        case SIID_SERVER:
            iconString = settings.SERVER;
            break;
        case SIID_PRINTER:
            iconString = settings.PRINTER;
            break;
        case SIID_MYNETWORK:
            iconString = settings.MYNETWORK;
            break;
        case SIID_FIND:
            iconString = settings.FIND;
            break;
        case SIID_HELP:
            iconString = settings.HELP;
            break;
        case SIID_SHARE:
            iconString = settings.SHARE;
            break;
        case SIID_LINK:
            iconString = settings.LINK;
            break;
        case SIID_SLOWFILE:
            iconString = settings.SLOWFILE;
            break;
        case SIID_RECYCLER:
            iconString = settings.RECYCLER;
            break;
        case SIID_RECYCLERFULL:
            iconString = settings.RECYCLERFULL;
            break;
        case SIID_MEDIACDAUDIO:
            iconString = settings.MEDIACDAUDIO;
            break;
        case SIID_LOCK:
            iconString = settings.LOCK;
            break;
        case SIID_AUTOLIST:
            iconString = settings.AUTOLIST;
            break;
        case SIID_PRINTERNET:
            iconString = settings.PRINTERNET;
            break;
        case SIID_SERVERSHARE:
            iconString = settings.SERVERSHARE;
            break;
        case SIID_PRINTERFAX:
            iconString = settings.PRINTERFAX;
            break;
        case SIID_PRINTERFAXNET:
            iconString = settings.PRINTERFAXNET;
            break;
        case SIID_PRINTERFILE:
            iconString = settings.PRINTERFILE;
            break;
        case SIID_STACK:
            iconString = settings.STACK;
            break;
        case SIID_MEDIASVCD:
            iconString = settings.MEDIASVCD;
            break;
        case SIID_STUFFEDFOLDER:
            iconString = settings.STUFFEDFOLDER;
            break;
        case SIID_DRIVEUNKNOWN:
            iconString = settings.DRIVEUNKNOWN;
            break;
        case SIID_DRIVEDVD:
            iconString = settings.DRIVEDVD;
            break;
        case SIID_MEDIADVD:
            iconString = settings.MEDIADVD;
            break;
        case SIID_MEDIADVDRAM:
            iconString = settings.MEDIADVDRAM;
            break;
        case SIID_MEDIADVDRW:
            iconString = settings.MEDIADVDRW;
            break;
        case SIID_MEDIADVDR:
            iconString = settings.MEDIADVDR;
            break;
        case SIID_MEDIADVDROM:
            iconString = settings.MEDIADVDROM;
            break;
        case SIID_MEDIACDAUDIOPLUS:
            iconString = settings.MEDIACDAUDIOPLUS;
            break;
        case SIID_MEDIACDRW:
            iconString = settings.MEDIACDRW;
            break;
        case SIID_MEDIACDR:
            iconString = settings.MEDIACDR;
            break;
        case SIID_MEDIACDBURN:
            iconString = settings.MEDIACDBURN;
            break;
        case SIID_MEDIABLANKCD:
            iconString = settings.MEDIABLANKCD;
            break;
        case SIID_MEDIACDROM:
            iconString = settings.MEDIACDROM;
            break;
        case SIID_AUDIOFILES:
            iconString = settings.AUDIOFILES;
            break;
        case SIID_IMAGEFILES:
            iconString = settings.IMAGEFILES;
            break;
        case SIID_VIDEOFILES:
            iconString = settings.VIDEOFILES;
            break;
        case SIID_MIXEDFILES:
            iconString = settings.MIXEDFILES;
            break;
        case SIID_FOLDERBACK:
            iconString = settings.FOLDERBACK;
            break;
        case SIID_FOLDERFRONT:
            iconString = settings.FOLDERFRONT;
            break;
        case SIID_SHIELD:
            iconString = settings.SHIELD;
            break;
        case SIID_WARNING:
            iconString = settings.WARNING;
            break;
        case SIID_INFO:
            iconString = settings.INFO;
            break;
        case SIID_ERROR:
            iconString = settings.iERROR;
            break;
        case SIID_KEY:
            iconString = settings.KEY;
            break;
        case SIID_SOFTWARE:
            iconString = settings.SOFTWARE;
            break;
        case SIID_RENAME:
            iconString = settings.RENAME;
            break;
        case SIID_DELETE:
            iconString = settings.iDELETE;
            break;
        case SIID_MEDIAAUDIODVD:
            iconString = settings.MEDIAAUDIODVD;
            break;
        case SIID_MEDIAMOVIEDVD:
            iconString = settings.MEDIAMOVIEDVD;
            break;
        case SIID_MEDIAENHANCEDCD:
            iconString = settings.MEDIAENHANCEDCD;
            break;
        case SIID_MEDIAENHANCEDDVD:
            iconString = settings.MEDIAENHANCEDDVD;
            break;
        case SIID_MEDIAHDDVD:
            iconString = settings.MEDIAHDDVD;
            break;
        case SIID_MEDIABLURAY:
            iconString = settings.MEDIABLURAY;
            break;
        case SIID_MEDIAVCD:
            iconString = settings.MEDIAVCD;
            break;
        case SIID_MEDIADVDPLUSR:
            iconString = settings.MEDIADVDPLUSR;
            break;
        case SIID_MEDIADVDPLUSRW:
            iconString = settings.MEDIADVDPLUSRW;
            break;
        case SIID_DESKTOPPC:
            iconString = settings.DESKTOPPC;
            break;
        case SIID_MOBILEPC:
            iconString = settings.MOBILEPC;
            break;
        case SIID_USERS:
            iconString = settings.USERS;
            break;
        case SIID_MEDIASMARTMEDIA:
            iconString = settings.MEDIASMARTMEDIA;
            break;
        case SIID_MEDIACOMPACTFLASH:
            iconString = settings.MEDIACOMPACTFLASH;
            break;
        case SIID_DEVICECELLPHONE:
            iconString = settings.DEVICECELLPHONE;
            break;
        case SIID_DEVICECAMERA:
            iconString = settings.DEVICECAMERA;
            break;
        case SIID_DEVICEVIDEOCAMERA:
            iconString = settings.DEVICEVIDEOCAMERA;
            break;
        case SIID_DEVICEAUDIOPLAYER:
            iconString = settings.DEVICEAUDIOPLAYER;
            break;
        case SIID_NETWORKCONNECT:
            iconString = settings.NETWORKCONNECT;
            break;
        case SIID_INTERNET:
            iconString = settings.INTERNET;
            break;
        case SIID_ZIPFILE:
            iconString = settings.ZIPFILE;
            break;
        case SIID_SETTINGS:
            iconString = settings.SETTINGS;
            break;
        case SIID_DRIVEHDDVD:
            iconString = settings.DRIVEHDDVD;
            break;
        case SIID_DRIVEBD:
            iconString = settings.DRIVEBD;
            break;
        case SIID_MEDIAHDDVDROM:
            iconString = settings.MEDIAHDDVDROM;
            break;
        case SIID_MEDIAHDDVDR:
            iconString = settings.MEDIAHDDVDR;
            break;
        case SIID_MEDIAHDDVDRAM:
            iconString = settings.MEDIAHDDVDRAM;
            break;
        case SIID_MEDIABDROM:
            iconString = settings.MEDIABDROM;
            break;
        case SIID_MEDIABDR:
            iconString = settings.MEDIABDR;
            break;
        case SIID_MEDIABDRE:
            iconString = settings.MEDIABDRE;
            break;
        case SIID_CLUSTEREDDRIVE:
            iconString = settings.CLUSTEREDDRIVE;
            break;
        case SIID_MAX_ICONS:
            break;
    }

    if (iconString && wcscmp(iconString, L"") != 0)
    {
        WCHAR buffer[1024] = L""; // max_path is 260 but alloc space for the index
        wcscpy_s(buffer, iconString);

        int i = PathParseIconLocationW(buffer);

        wcscpy_s(psii->szPath, MAX_PATH, buffer);

        psii->iIcon = i;

        UINT flags = 0;
        if ((uFlags & SHGSI_ICON) != 0)
        {
            flags |= SHGFI_ICON;

            if ((uFlags & SHGSI_LARGEICON) != 0)
                flags |= SHGFI_LARGEICON;
            else if ((uFlags & SHGSI_SMALLICON) != 0)
                flags |= SHGFI_SMALLICON;
            else if ((uFlags & SHGSI_SHELLICONSIZE) != 0)
                flags |= SHGFI_SHELLICONSIZE;
        }

        if ((uFlags & SHGSI_SYSICONINDEX) != 0)
            flags |= SHGFI_SYSICONINDEX;
        
        SHFILEINFOW shfi = {};
        if (SHGetFileInfoW(buffer, NULL, &shfi, sizeof(shfi), flags))
        {
            psii->hIcon = shfi.hIcon;
            psii->iSysImageIndex = shfi.iIcon;

            return S_OK;
        }
    }

    return SHGetStockIconInfo_Original(siid, uFlags, psii);
}

void LoadSettings() {
    settings.DOCNOASSOC = Wh_GetStringSetting(L"DOCNOASSOC");
    settings.DOCASSOC = Wh_GetStringSetting(L"DOCASSOC");
    settings.APPLICATION = Wh_GetStringSetting(L"APPLICATION");
    settings.FOLDER = Wh_GetStringSetting(L"FOLDER");
    settings.FOLDEROPEN = Wh_GetStringSetting(L"FOLDEROPEN");
    settings.DRIVE525 = Wh_GetStringSetting(L"DRIVE525");
    settings.DRIVE35 = Wh_GetStringSetting(L"DRIVE35");
    settings.DRIVEREMOVE = Wh_GetStringSetting(L"DRIVEREMOVE");
    settings.DRIVEFIXED = Wh_GetStringSetting(L"DRIVEFIXED");
    settings.DRIVENET = Wh_GetStringSetting(L"DRIVENET");
    settings.DRIVENETDISABLED = Wh_GetStringSetting(L"DRIVENETDISABLED");
    settings.DRIVECD = Wh_GetStringSetting(L"DRIVECD");
    settings.DRIVERAM = Wh_GetStringSetting(L"DRIVERAM");
    settings.WORLD = Wh_GetStringSetting(L"WORLD");
    settings.SERVER = Wh_GetStringSetting(L"SERVER");
    settings.PRINTER = Wh_GetStringSetting(L"PRINTER");
    settings.MYNETWORK = Wh_GetStringSetting(L"MYNETWORK");
    settings.FIND = Wh_GetStringSetting(L"FIND");
    settings.HELP = Wh_GetStringSetting(L"HELP");
    settings.SHARE = Wh_GetStringSetting(L"SHARE");
    settings.LINK = Wh_GetStringSetting(L"LINK");
    settings.SLOWFILE = Wh_GetStringSetting(L"SLOWFILE");
    settings.RECYCLER = Wh_GetStringSetting(L"RECYCLER");
    settings.RECYCLERFULL = Wh_GetStringSetting(L"RECYCLERFULL");
    settings.MEDIACDAUDIO = Wh_GetStringSetting(L"MEDIACDAUDIO");
    settings.LOCK = Wh_GetStringSetting(L"LOCK");
    settings.AUTOLIST = Wh_GetStringSetting(L"AUTOLIST");
    settings.PRINTERNET = Wh_GetStringSetting(L"PRINTERNET");
    settings.SERVERSHARE = Wh_GetStringSetting(L"SERVERSHARE");
    settings.PRINTERFAX = Wh_GetStringSetting(L"PRINTERFAX");
    settings.PRINTERFAXNET = Wh_GetStringSetting(L"PRINTERFAXNET");
    settings.PRINTERFILE = Wh_GetStringSetting(L"PRINTERFILE");
    settings.STACK = Wh_GetStringSetting(L"STACK");
    settings.MEDIASVCD = Wh_GetStringSetting(L"MEDIASVCD");
    settings.STUFFEDFOLDER = Wh_GetStringSetting(L"STUFFEDFOLDER");
    settings.DRIVEUNKNOWN = Wh_GetStringSetting(L"DRIVEUNKNOWN");
    settings.DRIVEDVD = Wh_GetStringSetting(L"DRIVEDVD");
    settings.MEDIADVD = Wh_GetStringSetting(L"MEDIADVD");
    settings.MEDIADVDRAM = Wh_GetStringSetting(L"MEDIADVDRAM");
    settings.MEDIADVDRW = Wh_GetStringSetting(L"MEDIADVDRW");
    settings.MEDIADVDR = Wh_GetStringSetting(L"MEDIADVDR");
    settings.MEDIADVDROM = Wh_GetStringSetting(L"MEDIADVDROM");
    settings.MEDIACDAUDIOPLUS = Wh_GetStringSetting(L"MEDIACDAUDIOPLUS");
    settings.MEDIACDRW = Wh_GetStringSetting(L"MEDIACDRW");
    settings.MEDIACDR = Wh_GetStringSetting(L"MEDIACDR");
    settings.MEDIACDBURN = Wh_GetStringSetting(L"MEDIACDBURN");
    settings.MEDIABLANKCD = Wh_GetStringSetting(L"MEDIABLANKCD");
    settings.MEDIACDROM = Wh_GetStringSetting(L"MEDIACDROM");
    settings.AUDIOFILES = Wh_GetStringSetting(L"AUDIOFILES");
    settings.IMAGEFILES = Wh_GetStringSetting(L"IMAGEFILES");
    settings.VIDEOFILES = Wh_GetStringSetting(L"VIDEOFILES");
    settings.MIXEDFILES = Wh_GetStringSetting(L"MIXEDFILES");
    settings.FOLDERBACK = Wh_GetStringSetting(L"FOLDERBACK");
    settings.FOLDERFRONT = Wh_GetStringSetting(L"FOLDERFRONT");
    settings.SHIELD = Wh_GetStringSetting(L"SHIELD");
    settings.WARNING = Wh_GetStringSetting(L"WARNING");
    settings.INFO = Wh_GetStringSetting(L"INFO");
    settings.iERROR = Wh_GetStringSetting(L"iERROR");
    settings.KEY = Wh_GetStringSetting(L"KEY");
    settings.SOFTWARE = Wh_GetStringSetting(L"SOFTWARE");
    settings.RENAME = Wh_GetStringSetting(L"RENAME");
    settings.iDELETE = Wh_GetStringSetting(L"iDELETE");
    settings.MEDIAAUDIODVD = Wh_GetStringSetting(L"MEDIAAUDIODVD");
    settings.MEDIAMOVIEDVD = Wh_GetStringSetting(L"MEDIAMOVIEDVD");
    settings.MEDIAENHANCEDCD = Wh_GetStringSetting(L"MEDIAENHANCEDCD");
    settings.MEDIAENHANCEDDVD = Wh_GetStringSetting(L"MEDIAENHANCEDDVD");
    settings.MEDIAHDDVD = Wh_GetStringSetting(L"MEDIAHDDVD");
    settings.MEDIABLURAY = Wh_GetStringSetting(L"MEDIABLURAY");
    settings.MEDIAVCD = Wh_GetStringSetting(L"MEDIAVCD");
    settings.MEDIADVDPLUSR = Wh_GetStringSetting(L"MEDIADVDPLUSR");
    settings.MEDIADVDPLUSRW = Wh_GetStringSetting(L"MEDIADVDPLUSRW");
    settings.DESKTOPPC = Wh_GetStringSetting(L"DESKTOPPC");
    settings.MOBILEPC = Wh_GetStringSetting(L"MOBILEPC");
    settings.USERS = Wh_GetStringSetting(L"USERS");
    settings.MEDIASMARTMEDIA = Wh_GetStringSetting(L"MEDIASMARTMEDIA");
    settings.MEDIACOMPACTFLASH = Wh_GetStringSetting(L"MEDIACOMPACTFLASH");
    settings.DEVICECELLPHONE = Wh_GetStringSetting(L"DEVICECELLPHONE");
    settings.DEVICECAMERA = Wh_GetStringSetting(L"DEVICECAMERA");
    settings.DEVICEVIDEOCAMERA = Wh_GetStringSetting(L"DEVICEVIDEOCAMERA");
    settings.DEVICEAUDIOPLAYER = Wh_GetStringSetting(L"DEVICEAUDIOPLAYER");
    settings.NETWORKCONNECT = Wh_GetStringSetting(L"NETWORKCONNECT");
    settings.INTERNET = Wh_GetStringSetting(L"INTERNET");
    settings.ZIPFILE = Wh_GetStringSetting(L"ZIPFILE");
    settings.SETTINGS = Wh_GetStringSetting(L"SETTINGS");
    settings.DRIVEHDDVD = Wh_GetStringSetting(L"DRIVEHDDVD");
    settings.DRIVEBD = Wh_GetStringSetting(L"DRIVEBD");
    settings.MEDIAHDDVDROM = Wh_GetStringSetting(L"MEDIAHDDVDROM");
    settings.MEDIAHDDVDR = Wh_GetStringSetting(L"MEDIAHDDVDR");
    settings.MEDIAHDDVDRAM = Wh_GetStringSetting(L"MEDIAHDDVDRAM");
    settings.MEDIABDROM = Wh_GetStringSetting(L"MEDIABDROM");
    settings.MEDIABDR = Wh_GetStringSetting(L"MEDIABDR");
    settings.MEDIABDRE = Wh_GetStringSetting(L"MEDIABDRE");
    settings.CLUSTEREDDRIVE = Wh_GetStringSetting(L"CLUSTEREDDRIVE");
}

// The mod is being initialized, load settings, hook functions, and do other
// initialization stuff if required.
BOOL Wh_ModInit() {
    Wh_Log(L"Init");

    LoadSettings();

    HMODULE hShell32Module = LoadLibraryW(L"shell32.dll");
    SHGetStockIconInfo_t SHGetStockIconInfo =
        (SHGetStockIconInfo_t)GetProcAddress(hShell32Module,
                                                "SHGetStockIconInfo");

    Wh_SetFunctionHook((void*)SHGetStockIconInfo,
                       (void*)SHGetStockIconInfo_Hook,
                       (void**)&SHGetStockIconInfo_Original);

    return TRUE;
}

// The mod is being unloaded, free all allocated resources.
void Wh_ModUninit() {
    Wh_Log(L"Uninit");
    Wh_FreeStringSetting(settings.DOCNOASSOC);
    Wh_FreeStringSetting(settings.DOCASSOC);
    Wh_FreeStringSetting(settings.APPLICATION);
    Wh_FreeStringSetting(settings.FOLDER);
    Wh_FreeStringSetting(settings.FOLDEROPEN);
    Wh_FreeStringSetting(settings.DRIVE525);
    Wh_FreeStringSetting(settings.DRIVE35);
    Wh_FreeStringSetting(settings.DRIVEREMOVE);
    Wh_FreeStringSetting(settings.DRIVEFIXED);
    Wh_FreeStringSetting(settings.DRIVENET);
    Wh_FreeStringSetting(settings.DRIVENETDISABLED);
    Wh_FreeStringSetting(settings.DRIVECD);
    Wh_FreeStringSetting(settings.DRIVERAM);
    Wh_FreeStringSetting(settings.WORLD);
    Wh_FreeStringSetting(settings.SERVER);
    Wh_FreeStringSetting(settings.PRINTER);
    Wh_FreeStringSetting(settings.MYNETWORK);
    Wh_FreeStringSetting(settings.FIND);
    Wh_FreeStringSetting(settings.HELP);
    Wh_FreeStringSetting(settings.SHARE);
    Wh_FreeStringSetting(settings.LINK);
    Wh_FreeStringSetting(settings.SLOWFILE);
    Wh_FreeStringSetting(settings.RECYCLER);
    Wh_FreeStringSetting(settings.RECYCLERFULL);
    Wh_FreeStringSetting(settings.MEDIACDAUDIO);
    Wh_FreeStringSetting(settings.LOCK);
    Wh_FreeStringSetting(settings.AUTOLIST);
    Wh_FreeStringSetting(settings.PRINTERNET);
    Wh_FreeStringSetting(settings.SERVERSHARE);
    Wh_FreeStringSetting(settings.PRINTERFAX);
    Wh_FreeStringSetting(settings.PRINTERFAXNET);
    Wh_FreeStringSetting(settings.PRINTERFILE);
    Wh_FreeStringSetting(settings.STACK);
    Wh_FreeStringSetting(settings.MEDIASVCD);
    Wh_FreeStringSetting(settings.STUFFEDFOLDER);
    Wh_FreeStringSetting(settings.DRIVEUNKNOWN);
    Wh_FreeStringSetting(settings.DRIVEDVD);
    Wh_FreeStringSetting(settings.MEDIADVD);
    Wh_FreeStringSetting(settings.MEDIADVDRAM);
    Wh_FreeStringSetting(settings.MEDIADVDRW);
    Wh_FreeStringSetting(settings.MEDIADVDR);
    Wh_FreeStringSetting(settings.MEDIADVDROM);
    Wh_FreeStringSetting(settings.MEDIACDAUDIOPLUS);
    Wh_FreeStringSetting(settings.MEDIACDRW);
    Wh_FreeStringSetting(settings.MEDIACDR);
    Wh_FreeStringSetting(settings.MEDIACDBURN);
    Wh_FreeStringSetting(settings.MEDIABLANKCD);
    Wh_FreeStringSetting(settings.MEDIACDROM);
    Wh_FreeStringSetting(settings.AUDIOFILES);
    Wh_FreeStringSetting(settings.IMAGEFILES);
    Wh_FreeStringSetting(settings.VIDEOFILES);
    Wh_FreeStringSetting(settings.MIXEDFILES);
    Wh_FreeStringSetting(settings.FOLDERBACK);
    Wh_FreeStringSetting(settings.FOLDERFRONT);
    Wh_FreeStringSetting(settings.SHIELD);
    Wh_FreeStringSetting(settings.WARNING);
    Wh_FreeStringSetting(settings.INFO);
    Wh_FreeStringSetting(settings.iERROR);
    Wh_FreeStringSetting(settings.KEY);
    Wh_FreeStringSetting(settings.SOFTWARE);
    Wh_FreeStringSetting(settings.RENAME);
    Wh_FreeStringSetting(settings.iDELETE);
    Wh_FreeStringSetting(settings.MEDIAAUDIODVD);
    Wh_FreeStringSetting(settings.MEDIAMOVIEDVD);
    Wh_FreeStringSetting(settings.MEDIAENHANCEDCD);
    Wh_FreeStringSetting(settings.MEDIAENHANCEDDVD);
    Wh_FreeStringSetting(settings.MEDIAHDDVD);
    Wh_FreeStringSetting(settings.MEDIABLURAY);
    Wh_FreeStringSetting(settings.MEDIAVCD);
    Wh_FreeStringSetting(settings.MEDIADVDPLUSR);
    Wh_FreeStringSetting(settings.MEDIADVDPLUSRW);
    Wh_FreeStringSetting(settings.DESKTOPPC);
    Wh_FreeStringSetting(settings.MOBILEPC);
    Wh_FreeStringSetting(settings.USERS);
    Wh_FreeStringSetting(settings.MEDIASMARTMEDIA);
    Wh_FreeStringSetting(settings.MEDIACOMPACTFLASH);
    Wh_FreeStringSetting(settings.DEVICECELLPHONE);
    Wh_FreeStringSetting(settings.DEVICECAMERA);
    Wh_FreeStringSetting(settings.DEVICEVIDEOCAMERA);
    Wh_FreeStringSetting(settings.DEVICEAUDIOPLAYER);
    Wh_FreeStringSetting(settings.NETWORKCONNECT);
    Wh_FreeStringSetting(settings.INTERNET);
    Wh_FreeStringSetting(settings.ZIPFILE);
    Wh_FreeStringSetting(settings.SETTINGS);
    Wh_FreeStringSetting(settings.DRIVEHDDVD);
    Wh_FreeStringSetting(settings.DRIVEBD);
    Wh_FreeStringSetting(settings.MEDIAHDDVDROM);
    Wh_FreeStringSetting(settings.MEDIAHDDVDR);
    Wh_FreeStringSetting(settings.MEDIAHDDVDRAM);
    Wh_FreeStringSetting(settings.MEDIABDROM);
    Wh_FreeStringSetting(settings.MEDIABDR);
    Wh_FreeStringSetting(settings.MEDIABDRE);
    Wh_FreeStringSetting(settings.CLUSTEREDDRIVE);
}

// The mod settings were changed, reload them.
void Wh_ModSettingsChanged() {
    Wh_Log(L"SettingsChanged");

    LoadSettings();
}
