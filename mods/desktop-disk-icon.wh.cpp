// ==WindhawkMod==
// @id              desktop-disk-icon
// @name            Desktop Disk Icon
// @name:zh-CN      桌面显示U盘图标
// @description     Displays external storage devices as native desktop icons
// @description:zh-CN 将外接存储设备以系统桌面图标的形式呈现
// @version         1.0.0
// @author          Raku Inkyetta
// @github          https://github.com/AomiRaku
// @homepage        https://raku404.com/
// @license         GPL-3.0
// @include         explorer.exe
// @compilerOptions -lole32 -loleaut32 -lshell32 -luuid
// ==/WindhawkMod==

// Source code is published under The GNU General Public License v3.0.
//
// For bug reports and feature requests, please open an issue here:
// https://github.com/AomiRaku/desktop-disk-icon/issues
//
// For pull requests, development takes place here:
// https://github.com/AomiRaku/desktop-disk-icon

// ==WindhawkModReadme==
/*
# Desktop Disk Icon / 桌面显示U盘图标

> 中文描述在下方，请继续向下滚动查看。
> English first; the Chinese section is at the bottom.

Renders external storage devices as native Windows desktop icons, in a manner comparable to how
mounted volumes appear on the macOS desktop.

## Features

- Covers USB flash drives, external hard disks and SSDs, optical media (CD/DVD/Blu-ray and
  mounted ISO images), floppy disks, memory cards, and MTP/PTP devices such as mobile phones.
- Icons are taken from the volume itself. The shell derives them from the volume label,
  `autorun.inf` or the device description; the mod never substitutes its own icons.
- Icon names combine the volume label with the drive letter. When the label is empty, the
  device type name reported by the shell is used, for example `U 盘 (F)` or `CD-ROM (H)`.
  
![Desktop icons](https://i.imgur.com/oTo4VIr.png)

## How it works

By default the mod registers a shell namespace item for each drive. Such an item belongs to the
same category of desktop objects as "This PC" and "Recycle Bin": no `.lnk` file is created on
the desktop and no shortcut arrow is drawn. The COM server of that item is the mod itself
(`InProcServer32` points to the mod DLL), which implements a forwarding `IShellFolder` that
passes enumeration, binding, view creation and context menu calls to the real folder object of
the volume, so the behaviour matches a built-in shell folder.

The mod runs inside `explorer.exe`, listens for the shell's drive and media change
notifications, and additionally polls at a configurable interval. Every synchronization pass
brings the desktop icons in line with the devices that are currently attached.

## Known limitations and cleanup

- External hard disks are recognised by bus type (USB, IEEE 1394, SD/MMC, virtual). Disks
  attached through Thunderbolt or eSATA that report an internal bus may not be recognised;
  internal disks are never shown.
- Portable devices (MTP/PTP) are presented as desktop shortcuts rather than namespace items.
- Removing a device deletes its icon immediately; there is no eject command. Use the system
  tray safe-removal function before unplugging.
- Unplugging a device deletes the corresponding item and its registry keys; disabling or
  uninstalling the mod deletes every item created by it.
- If the mod is terminated forcefully, registry keys may remain; they are cleaned up on the
  next start.
- The namespace extension DLL remains loaded once the shell has loaded it, so Explorer must be
  restarted after updating the mod.
- If plugging or unplugging a disk does not update the desktop, set the polling interval to
  about 10 seconds in the settings.

## Links

- GitHub: <https://github.com/AomiRaku/desktop-disk-icon>

---

# 中文部分

## 功能

- 覆盖 USB 闪存盘、外接硬盘与固态硬盘、光盘（CD/DVD/蓝光及已挂载的 ISO 映像）、软盘、
  存储卡，以及以 MTP/PTP 方式连接的手机。
- 图标取自磁盘本身，由系统依据卷标、`autorun.inf` 或设备描述提供，模组不做替换。
- 图标名称由卷标与盘符组合而成；卷标为空时采用系统提供的设备类型名称，
  例如 `U 盘 (F)`、`CD-ROM (H)`。

## 实现方式

默认情况下，模组为每个驱动器注册一个 shell 命名空间项，与“此电脑”“回收站”属于同一类桌面
对象：桌面不产生 `.lnk` 文件，也不显示快捷方式箭头。该对象的 COM 服务器即为模组自身
（`InProcServer32` 指向模组 DLL），其中实现了一个转发型 `IShellFolder`，将枚举、绑定、
视图创建与上下文菜单等调用转发至该卷真实的文件夹对象，因此行为与系统内置文件夹一致。

模组运行于 `explorer.exe`，监听 shell 的驱动器与介质变更通知，并以定时轮询作为补充；
每次同步均使桌面图标与当前设备状态保持一致。

## 已知限制与清理行为

- 外接硬盘依据设备总线类型识别（USB、IEEE 1394、SD/MMC、虚拟）；雷雳或 eSATA 接口且被
  报告为内置总线的硬盘可能无法识别，内置磁盘始终不会显示。
- 便携设备（MTP/PTP）以桌面快捷方式呈现，不采用命名空间项。
- 拔出设备时直接移除图标，不提供弹出操作；请沿用系统托盘中的安全删除功能。
- 拔出设备时删除对应项与注册表项；禁用或卸载模组时删除全部由本模组创建的项目。
- 模组被强制终止时可能残留注册表项，下次启动时自动清理。
- 命名空间扩展 DLL 被加载后不会卸载，更新模组后需重启资源管理器方可生效。
- 若插拔磁盘后桌面未更新，可在设置中将轮询间隔设为 10 秒左右。

## 相关链接

- GitHub：<https://github.com/AomiRaku/desktop-disk-icon>

*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- includeRemovable: true
  $name: Removable drives
  $name:zh-CN: 可移动存储设备
  $description: Shows drives that Windows reports as removable, including USB flash drives, floppy disks and card readers. Slots without media are shown only if the option below is enabled.
  $description:zh-CN: 显示由系统识别为可移动存储的驱动器，包括 U 盘、软盘与读卡器。未插入介质的插槽需启用下方“显示无介质驱动器”后才会显示。
- includeExternalFixed: true
  $name: External fixed disks
  $name:zh-CN: 外接固定磁盘
  $description: Shows fixed-type disks attached through an external bus (USB, IEEE 1394, SD/MMC, virtual). Disks attached through the internal bus are never shown.
  $description:zh-CN: 显示通过外部总线（USB、IEEE 1394、SD/MMC、虚拟）连接的固定类型磁盘。通过内部总线连接的磁盘始终不显示。
- includeOptical: true
  $name: Optical drives
  $name:zh-CN: 光盘驱动器
  $description: Shows optical drives and mounted ISO images.
  $description:zh-CN: 显示光盘驱动器及已挂载的 ISO 映像。
- includePortable: true
  $name: Portable devices
  $name:zh-CN: 便携设备
  $description: Shows MTP/PTP devices such as mobile phones and cameras. These devices are presented as desktop shortcuts, because their shell items cannot be represented as a file system folder.
  $description:zh-CN: 显示以 MTP/PTP 方式连接的手机、相机等设备。此类设备以桌面快捷方式呈现，原因是其 shell 项无法表示为文件系统文件夹。
- showEmptyDrives: false
  $name: Show drives without media
  $name:zh-CN: 显示无介质驱动器
  $description: Also shows empty card reader slots and optical drives without a disc.
  $description:zh-CN: 同时显示未插入存储卡的读卡器插槽，以及未放入光盘的光盘驱动器。
- publicDesktop: false
  $name: Place icons on the public desktop
  $name:zh-CN: 图标写入公用桌面
  $description: Writes the icons to the public desktop folder so that all users of this computer see them. When disabled, only the current user's desktop is used.
  $description:zh-CN: 将图标写入公用桌面，使本机所有用户均可看到。关闭时仅使用当前用户的桌面。
- iconPlacement: default
  $name: New icon placement
  $name:zh-CN: 新图标位置
  $options:
  - default: Default position
  - rightmost: First free slot of the rightmost column
  $options:zh-CN:
  - default: 默认位置
  - rightmost: 最右一列的首个空位
  $description: With the rightmost option, each newly created icon is moved to the right edge of the desktop and that column is filled from the top. The setting has no effect while "Auto arrange icons" is enabled.
  $description:zh-CN: 选择“最右一列的首个空位”时，新建图标将被移动至桌面右侧，并按从上至下的顺序填充该列。桌面启用“自动排列图标”时该设置不生效。
- iconStyle: namespace
  $name: Icon type
  $name:zh-CN: 图标类型
  $options:
  - namespace: Native shell items
  - shortcut: Shortcut files
  $options:zh-CN:
  - namespace: 系统图标
  - shortcut: 快捷方式文件
  $description: Native shell items implement an IShellFolder for each drive, so the item behaves like a built-in shell folder and has no shortcut arrow. Switch to shortcut files if problems occur.
  $description:zh-CN: “系统命名空间项”为每个驱动器实现独立的 IShellFolder，其行为与系统内置文件夹一致且不带快捷方式箭头。如遇异常可切换为“快捷方式文件”。
- pollSeconds: 0
  $name: Polling interval (seconds)
  $name:zh-CN: 轮询间隔（秒）
  $description: Interval of the periodic synchronization, which covers device changes that raise no shell notification. 0 relies on shell events only; set it to around 10 if plugging or unplugging a disk does not update the desktop.
  $description:zh-CN: 周期同步间隔，用于捕获未触发 shell 通知的设备变更。设为 0 时仅依赖系统事件；若插拔磁盘后桌面没有反应，可将其设为 10 左右再试。
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <dbt.h>
#include <exdisp.h>
#include <shlguid.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <shobjidl.h>
#include <winioctl.h>

#include <atomic>
#include <iterator>
#include <map>
#include <mutex>
#include <new>
#include <set>
#include <string>
#include <utility>
#include <vector>

// ---------------------------------------------------------------------------
// 常量
// ---------------------------------------------------------------------------

// 快捷方式参数里写入的隐藏标记，用来区分“本模组创建的图标”和用户自己的图标。
// 形如 "wh-dex|d|F"（盘符）或 "wh-dex|p|<哈希>"（便携设备）。
constexpr PCWSTR kMarkerPrefix = L"wh-dex|";
constexpr size_t kMarkerPrefixLen = 7;


constexpr UINT kMsgShellChange = WM_APP + 1;      // 收到 shell 变更通知
constexpr UINT kMsgSettingsChanged = WM_APP + 2;  // 设置已更改

// 只在驱动器和介质变化时同步；其余事件交给轮询，避免拖动文件也触发重建。
constexpr LONG kShellEvents =
    SHCNE_DRIVEADD | SHCNE_DRIVEREMOVED | SHCNE_MEDIAINSERTED |
    SHCNE_MEDIAREMOVED | SHCNE_NETSHARE | SHCNE_NETUNSHARE;

// ---------------------------------------------------------------------------
// 设置
// ---------------------------------------------------------------------------

struct Settings {
    bool includeRemovable = true;
    bool includeExternalFixed = true;
    bool includeOptical = true;
    bool includePortable = true;
    bool showEmptyDrives = false;
    bool publicDesktop = false;
    bool placeRightmost = false;
    bool useNamespace = true;
    int pollSeconds = 0;
};

Settings g_settings;
std::mutex g_settingsMutex;

std::atomic<bool> g_unloading{false};
HANDLE g_thread = nullptr;
HANDLE g_stopEvent = nullptr;       // 请求工作线程退出（手动重置）
HANDLE g_reconcileEvent = nullptr;  // 请求立刻同步一次（自动重置）
DWORD g_threadId = 0;
HWND g_hwnd = nullptr;
PIDLIST_ABSOLUTE g_shellNotifyPidl = nullptr;

// 请求工作线程立刻同步一次。这里只用事件，不依赖窗口消息是否正常。
void RequestReconcile() {
    if (g_reconcileEvent) {
        SetEvent(g_reconcileEvent);
    }
}

Settings GetSettings() {
    std::lock_guard<std::mutex> lock(g_settingsMutex);
    return g_settings;
}

void LoadSettings() {
    Settings s;

    s.includeRemovable = Wh_GetIntSetting(L"includeRemovable") != 0;
    s.includeExternalFixed = Wh_GetIntSetting(L"includeExternalFixed") != 0;
    s.includeOptical = Wh_GetIntSetting(L"includeOptical") != 0;
    s.includePortable = Wh_GetIntSetting(L"includePortable") != 0;
    s.showEmptyDrives = Wh_GetIntSetting(L"showEmptyDrives") != 0;
    s.publicDesktop = Wh_GetIntSetting(L"publicDesktop") != 0;

    PCWSTR iconPlacement = Wh_GetStringSetting(L"iconPlacement");
    s.placeRightmost = wcscmp(iconPlacement, L"rightmost") == 0;
    Wh_FreeStringSetting(iconPlacement);

    PCWSTR iconStyle = Wh_GetStringSetting(L"iconStyle");
    s.useNamespace = _wcsicmp(iconStyle, L"shortcut") != 0;
    Wh_FreeStringSetting(iconStyle);

    int pollSeconds = Wh_GetIntSetting(L"pollSeconds");
    if (pollSeconds < 0) {
        pollSeconds = 0;
    } else if (pollSeconds > 3600) {
        pollSeconds = 3600;
    }
    s.pollSeconds = pollSeconds;

    std::lock_guard<std::mutex> lock(g_settingsMutex);
    g_settings = s;
}

// ---------------------------------------------------------------------------
// 小工具
// ---------------------------------------------------------------------------

// PIDL 的 RAII 包装，避免各种提前 return 造成泄漏。
class Pidl {
  public:
    Pidl() = default;
    explicit Pidl(PIDLIST_ABSOLUTE pidl) : pidl_(pidl) {}
    Pidl(const Pidl&) = delete;
    Pidl& operator=(const Pidl&) = delete;
    Pidl(Pidl&& other) noexcept : pidl_(other.pidl_) { other.pidl_ = nullptr; }
    Pidl& operator=(Pidl&& other) noexcept {
        if (this != &other) {
            Reset(other.pidl_);
            other.pidl_ = nullptr;
        }
        return *this;
    }
    ~Pidl() { Reset(nullptr); }

    void Reset(PIDLIST_ABSOLUTE pidl) {
        if (pidl_) {
            ILFree(pidl_);
        }
        pidl_ = pidl;
    }

    PIDLIST_ABSOLUTE get() const { return pidl_; }

  private:
    PIDLIST_ABSOLUTE pidl_ = nullptr;
};

std::wstring GetKnownFolderPath(REFKNOWNFOLDERID folderId) {
    PWSTR path = nullptr;
    std::wstring result;
    if (SUCCEEDED(SHGetKnownFolderPath(folderId, KF_FLAG_DEFAULT, nullptr,
                                       &path))) {
        result = path;
        CoTaskMemFree(path);
    }
    return result;
}

std::wstring ToLower(std::wstring s) {
    if (!s.empty()) {
        CharLowerBuffW(s.data(), (DWORD)s.size());
    }
    return s;
}

// 去掉文件名里不允许的字符，并限制长度。
void SanitizeFileName(std::wstring& name) {
    constexpr PCWSTR kInvalid = L"<>:\"/\\|?*";
    for (auto& c : name) {
        if (c < 32 || wcschr(kInvalid, c)) {
            c = L'_';
        }
    }
    while (!name.empty() && (name.back() == L' ' || name.back() == L'.')) {
        name.pop_back();
    }
    if (name.size() > 96) {
        name.resize(96);
    }
}

// FNV-1a 64 位哈希，用于给便携设备生成短而稳定的标记。
std::wstring HashToHex(const std::wstring& text) {
    uint64_t hash = 1469598103934665603ULL;
    for (WCHAR c : text) {
        hash ^= (uint16_t)c;
        hash *= 1099511628211ULL;
    }
    WCHAR buffer[17];
    swprintf(buffer, ARRAYSIZE(buffer), L"%016llX", (unsigned long long)hash);
    return buffer;
}

// 卷是否已插入介质（空的读卡器插槽、没放盘的光驱都返回 false）。
bool HasMedia(PCWSTR root) {
    WCHAR label[MAX_PATH + 1];
    return GetVolumeInformationW(root, label, ARRAYSIZE(label), nullptr, nullptr,
                                 nullptr, nullptr, 0) != FALSE;
}

// 磁盘是否接在外部总线上。内置 SATA/NVMe 磁盘返回 false。
bool IsExternalBus(WCHAR driveLetter) {
    WCHAR devicePath[8] = {L'\\', L'\\', L'.', L'\\', driveLetter, L':', 0};
    HANDLE handle =
        CreateFileW(devicePath, 0, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr,
                    OPEN_EXISTING, 0, nullptr);
    if (handle == INVALID_HANDLE_VALUE) {
        return false;
    }

    STORAGE_PROPERTY_QUERY query = {};
    query.PropertyId = StorageDeviceProperty;
    query.QueryType = PropertyStandardQuery;

    BYTE buffer[1024] = {};
    DWORD bytesReturned = 0;
    BOOL ok = DeviceIoControl(handle, IOCTL_STORAGE_QUERY_PROPERTY, &query,
                              sizeof(query), buffer, sizeof(buffer),
                              &bytesReturned, nullptr);
    CloseHandle(handle);
    if (!ok || bytesReturned < sizeof(STORAGE_DEVICE_DESCRIPTOR)) {
        return false;
    }

    auto* descriptor = (const STORAGE_DEVICE_DESCRIPTOR*)buffer;
    switch (descriptor->BusType) {
        case BusType1394:
        case BusTypeUsb:
        case BusTypeSd:
        case BusTypeMmc:
        case BusTypeVirtual:
        case BusTypeFileBackedVirtual:
            return true;
        default:
            return false;
    }
}

// 是否像便携设备（手机、相机、MTP 播放器）的 shell 路径。
bool LooksLikePortableDevice(const std::wstring& parsingNameLower) {
    static PCWSTR kHints[] = {L"usb#", L"swd#", L"#vid_", L"wpdbusenum"};
    for (PCWSTR hint : kHints) {
        if (parsingNameLower.find(hint) != std::wstring::npos) {
            return true;
        }
    }
    return false;
}

// ---------------------------------------------------------------------------
// 设备枚举
// ---------------------------------------------------------------------------

struct Device {
    std::wstring marker;       // 唯一标记，写进快捷方式参数
    std::wstring name;         // 桌面显示名
    std::wstring targetPath;   // 盘符路径，例如 "F:\"（便携设备为空）
    std::wstring description;  // 快捷方式说明（工具提示）
    Pidl pidl;                 // 便携设备的 shell 项，可使用 SetIDList
};

void AddDriveDevices(const Settings& settings, std::vector<Device>& out) {
    DWORD mask = GetLogicalDrives();

    for (int i = 0; i < 26; i++) {
        if (!(mask & (1u << i))) {
            continue;
        }

        WCHAR root[4] = {(WCHAR)(L'A' + i), L':', L'\\', 0};
        UINT driveType = GetDriveTypeW(root);

        bool include = false;
        switch (driveType) {
            case DRIVE_REMOVABLE:
                include = settings.includeRemovable;
                break;
            case DRIVE_FIXED:
                include = settings.includeExternalFixed &&
                          IsExternalBus(root[0]);
                break;
            case DRIVE_CDROM:
                include = settings.includeOptical;
                break;
            default:
                break;  // 网络驱动器、内存盘等一律忽略
        }
        if (!include) {
            continue;
        }

        if (!settings.showEmptyDrives && !HasMedia(root)) {
            continue;
        }

        // 名称：卷标优先，没有卷标就用系统给的类型名（“U 盘”、“CD 驱动器”…）
        WCHAR label[MAX_PATH + 1] = {};
        GetVolumeInformationW(root, label, ARRAYSIZE(label), nullptr, nullptr,
                              nullptr, nullptr, 0);

        std::wstring base = label;
        SHFILEINFOW fileInfo = {};
        if (base.empty() &&
            SHGetFileInfoW(root, 0, &fileInfo, sizeof(fileInfo),
                           SHGFI_TYPENAME)) {
            base = fileInfo.szTypeName;
        }
        if (base.empty()) {
            base = L"Disk";
        }

        Device device;
        device.marker = kMarkerPrefix;
        device.marker += L"d|";
        device.marker += root[0];
        device.name = base + L" (" + root[0] + L")";
        device.targetPath = root;

        if (SHGetFileInfoW(root, 0, &fileInfo, sizeof(fileInfo),
                           SHGFI_DISPLAYNAME)) {
            device.description = fileInfo.szDisplayName;
        } else {
            device.description = device.name;
        }

        out.push_back(std::move(device));
    }
}

void AddPortableDevices(const Settings& settings, std::vector<Device>& out) {
    if (!settings.includePortable) {
        return;
    }

    Pidl thisPcPidl;
    {
        PIDLIST_ABSOLUTE pidl = nullptr;
        if (FAILED(SHGetKnownFolderIDList(FOLDERID_ComputerFolder, 0, nullptr,
                                          &pidl))) {
            return;
        }
        thisPcPidl.Reset(pidl);
    }

    IShellFolder* desktopFolder = nullptr;
    if (FAILED(SHGetDesktopFolder(&desktopFolder))) {
        return;
    }

    IShellFolder* thisPcFolder = nullptr;
    HRESULT hr = desktopFolder->BindToObject(
        thisPcPidl.get(), nullptr, IID_PPV_ARGS(&thisPcFolder));
    desktopFolder->Release();
    if (FAILED(hr)) {
        return;
    }

    IEnumIDList* enumIds = nullptr;
    hr = thisPcFolder->EnumObjects(nullptr, SHCONTF_FOLDERS, &enumIds);
    if (FAILED(hr)) {
        thisPcFolder->Release();
        return;
    }

    LPITEMIDLIST relativePidl = nullptr;
    while (enumIds->Next(1, &relativePidl, nullptr) == S_OK) {
        Pidl absolutePidl(ILCombine(thisPcPidl.get(), relativePidl));

        // 文件系统项（盘符、用户文件夹）已经由 AddDriveDevices 处理。
        // 注意必须用绝对 PIDL 判断，相对 PIDL 无法转换出路径。
        WCHAR fileSystemPath[MAX_PATH];
        bool isFileSystem =
            absolutePidl.get() &&
            SHGetPathFromIDListW(absolutePidl.get(), fileSystemPath);

        SFGAOF attributes = SFGAO_HIDDEN;
        if (!isFileSystem) {
            thisPcFolder->GetAttributesOf(1,
                                          (PCUITEMID_CHILD_ARRAY)&relativePidl,
                                          &attributes);
        }

        CoTaskMemFree(relativePidl);
        relativePidl = nullptr;

        if (!absolutePidl.get() || isFileSystem ||
            (attributes & SFGAO_HIDDEN)) {
            continue;
        }

        PWSTR parsingName = nullptr;
        if (FAILED(SHGetNameFromIDList(absolutePidl.get(),
                                       SIGDN_DESKTOPABSOLUTEPARSING,
                                       &parsingName))) {
            continue;
        }

        std::wstring parsingNameLower = ToLower(parsingName);
        CoTaskMemFree(parsingName);

        if (!LooksLikePortableDevice(parsingNameLower)) {
            // 此电脑里偶尔会出现第三方的命名空间项（云盘等），不当作便携设备。
            continue;
        }

        PWSTR displayName = nullptr;
        if (FAILED(SHGetNameFromIDList(absolutePidl.get(), SIGDN_NORMALDISPLAY,
                                       &displayName))) {
            continue;
        }

        Device device;
        device.marker = kMarkerPrefix;
        device.marker += L"p|";
        device.marker += HashToHex(parsingNameLower);
        device.name = displayName;
        device.description = displayName;
        device.pidl = Pidl(ILClone(absolutePidl.get()));
        CoTaskMemFree(displayName);

        if (device.pidl.get()) {
            out.push_back(std::move(device));
        }
    }

    if (relativePidl) {
        CoTaskMemFree(relativePidl);
    }

    enumIds->Release();
    thisPcFolder->Release();
}

// ---------------------------------------------------------------------------
// 快捷方式的读写
// ---------------------------------------------------------------------------

bool ReadShortcutMarker(const std::wstring& lnkPath, std::wstring& marker) {
    IShellLinkW* link = nullptr;
    if (FAILED(CoCreateInstance(CLSID_ShellLink, nullptr, CLSCTX_INPROC_SERVER,
                                IID_PPV_ARGS(&link)))) {
        return false;
    }

    bool found = false;
    IPersistFile* persistFile = nullptr;
    if (SUCCEEDED(link->QueryInterface(IID_PPV_ARGS(&persistFile)))) {
        if (SUCCEEDED(persistFile->Load(lnkPath.c_str(), STGM_READ))) {
            WCHAR arguments[512] = {};
            if (SUCCEEDED(
                    link->GetArguments(arguments, ARRAYSIZE(arguments))) &&
                wcsncmp(arguments, kMarkerPrefix, kMarkerPrefixLen) == 0) {
                marker = arguments;
                found = true;
            }
        }
        persistFile->Release();
    }

    link->Release();
    return found;
}

// 前置声明：定义在下方（见「shell 通知：一律异步发送」）。
void NotifyShellAsync(LONG event, const std::wstring& path = std::wstring());

bool CreateShortcut(const Device& device, const std::wstring& lnkPath) {
    IShellLinkW* link = nullptr;
    if (FAILED(CoCreateInstance(CLSID_ShellLink, nullptr, CLSCTX_INPROC_SERVER,
                                IID_PPV_ARGS(&link)))) {
        return false;
    }

    HRESULT hr;
    if (device.pidl.get()) {
        // 便携设备：链接到 shell 项本身，图标和行为都与“此电脑”里一致。
        hr = link->SetIDList(device.pidl.get());
    } else {
        hr = link->SetPath(device.targetPath.c_str());
        if (SUCCEEDED(hr)) {
            link->SetWorkingDirectory(device.targetPath.c_str());
        }
    }

    bool created = false;
    if (SUCCEEDED(hr)) {
        link->SetDescription(device.description.c_str());
        link->SetArguments(device.marker.c_str());

        IPersistFile* persistFile = nullptr;
        if (SUCCEEDED(link->QueryInterface(IID_PPV_ARGS(&persistFile)))) {
            created = SUCCEEDED(persistFile->Save(lnkPath.c_str(), FALSE));
            persistFile->Release();
        }
    }

    link->Release();

    if (created) {
        NotifyShellAsync(SHCNE_CREATE, lnkPath);
    } else {
        Wh_Log(L"Failed to create shortcut %s", lnkPath.c_str());
    }
    return created;
}

// 扫描桌面目录，找出本模组创建的快捷方式：标记 -> 路径。
void ScanOwnShortcuts(const std::wstring& directory,
                      std::map<std::wstring, std::wstring>& own) {
    WIN32_FIND_DATAW findData;
    HANDLE find = FindFirstFileW((directory + L"*.lnk").c_str(), &findData);
    if (find == INVALID_HANDLE_VALUE) {
        return;
    }

    do {
        if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            continue;
        }

        std::wstring path = directory + findData.cFileName;
        std::wstring marker;
        if (ReadShortcutMarker(path, marker)) {
            own[marker] = path;
        }
    } while (FindNextFileW(find, &findData));

    FindClose(find);
}

bool PathBelongsToMarker(const std::map<std::wstring, std::wstring>& own,
                         const std::wstring& path,
                         const std::wstring& marker) {
    for (const auto& [ownMarker, ownPath] : own) {
        if (_wcsicmp(ownPath.c_str(), path.c_str()) == 0) {
            return _wcsicmp(ownMarker.c_str(), marker.c_str()) == 0;
        }
    }
    return false;
}

// 选一个不会和用户自己的文件冲突的名字。
std::wstring ChooseShortcutPath(const std::wstring& directory,
                                const std::wstring& baseName,
                                const std::map<std::wstring, std::wstring>& own,
                                const std::wstring& marker) {
    std::wstring path = directory + baseName + L".lnk";

    for (int i = 2; i <= 99; i++) {
        if (GetFileAttributesW(path.c_str()) == INVALID_FILE_ATTRIBUTES) {
            return path;
        }
        if (PathBelongsToMarker(own, path, marker)) {
            return path;
        }
        path = directory + baseName + L" (" + std::to_wstring(i) + L").lnk";
    }

    return path;
}

// ---------------------------------------------------------------------------
// shell 通知：一律异步发送
//
// SHChangeNotify 是同步广播，如果在自己的工作线程（STA）里直接调用，shell 在处理
// 广播期间可能反过来向本线程发起 COM 调用，而本线程正阻塞在广播里 → 互相等待。
// 一旦卡住，后续所有插拔同步都会停摆。因此改成由一个独立的 MTA 短线程发送。
// ---------------------------------------------------------------------------
struct ShellNotifyRequest {
    LONG event;
    std::wstring path;
};

DWORD WINAPI ShellNotifyThreadProc(LPVOID param) {
    auto* request = static_cast<ShellNotifyRequest*>(param);
    HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    if (request->path.empty()) {
        SHChangeNotify(request->event, SHCNF_IDLIST, nullptr, nullptr);
    } else {
        SHChangeNotify(request->event, SHCNF_PATHW, request->path.c_str(),
                       nullptr);
    }
    if (SUCCEEDED(hr)) {
        CoUninitialize();
    }
    delete request;
    return 0;
}

void NotifyShellAsync(LONG event, const std::wstring& path) {
    auto* request = new (std::nothrow) ShellNotifyRequest{event, path};
    if (!request) {
        return;
    }
    HANDLE thread =
        CreateThread(nullptr, 0, ShellNotifyThreadProc, request, 0, nullptr);
    if (!thread) {
        delete request;
        return;
    }
    CloseHandle(thread);
}

void DeleteShortcut(const std::wstring& path) {
    if (DeleteFileW(path.c_str())) {
        Wh_Log(L"Deleted %s", path.c_str());
        NotifyShellAsync(SHCNE_DELETE, path);
    }
}

// ---------------------------------------------------------------------------
// 图标位置：把新建的图标挪到桌面最右一列的第一个空位
// ---------------------------------------------------------------------------

// 在给定占用位置里，找“从最右一列开始、从上往下的第一个空位”。
bool ComputeRightmostSlot(const std::vector<POINT>& used,
                          int clientWidth,
                          int clientHeight,
                          POINT* out) {
    int cellWidth = GetSystemMetrics(SM_CXICONSPACING);
    int cellHeight = GetSystemMetrics(SM_CYICONSPACING);
    if (cellWidth <= 0 || cellHeight <= 0 || clientWidth <= cellWidth ||
        clientHeight <= cellHeight) {
        return false;
    }

    auto isOccupied = [&used, cellWidth, cellHeight](POINT candidate) {
        for (const POINT& point : used) {
            if (abs(point.x - candidate.x) < cellWidth / 2 &&
                abs(point.y - candidate.y) < cellHeight / 2) {
                return true;
            }
        }
        return false;
    };

    int maxColumn = (clientWidth - 1) / cellWidth;
    int maxRow = (clientHeight - 1) / cellHeight;
    for (int column = maxColumn; column >= 0; column--) {
        for (int row = 0; row <= maxRow; row++) {
            POINT candidate = {column * cellWidth, row * cellHeight};
            if (!isOccupied(candidate)) {
                *out = candidate;
                return true;
            }
        }
    }
    return false;
}

// 兜底方案：直接操作桌面列表控件。模组就在 explorer 进程里，所以这些消息是本进程内的，
// 不涉及跨进程指针。
// 注意：ENUMWINDOWSPROC 是 __stdcall，32 位下不能用 lambda 隐式转换，所以用普通回调函数。
BOOL CALLBACK FindDefViewProc(HWND hWnd, LPARAM lParam) {
    HWND child = FindWindowExW(hWnd, nullptr, L"SHELLDLL_DefView", nullptr);
    if (child) {
        *reinterpret_cast<HWND*>(lParam) = child;
        return FALSE;
    }
    return TRUE;
}

HWND FindDesktopListView() {
    HWND progman = FindWindowW(L"Progman", nullptr);
    HWND defView =
        progman ? FindWindowExW(progman, nullptr, L"SHELLDLL_DefView", nullptr)
                : nullptr;
    if (!defView) {
        HWND found = nullptr;
        EnumWindows(FindDefViewProc, reinterpret_cast<LPARAM>(&found));
        defView = found;
    }
    if (!defView) {
        return nullptr;
    }
    return FindWindowExW(defView, nullptr, L"SysListView32", nullptr);
}

bool PlaceViaListView(const std::wstring& lnkPath) {
    HWND listView = FindDesktopListView();
    if (!listView) {
        return false;
    }

    // 列表控件里的文字是去掉 ".lnk" 的文件名
    size_t slash = lnkPath.find_last_of(L'\\');
    std::wstring name =
        slash == std::wstring::npos ? lnkPath : lnkPath.substr(slash + 1);
    if (name.size() > 4 && _wcsicmp(name.c_str() + name.size() - 4, L".lnk") == 0) {
        name.resize(name.size() - 4);
    }

    WCHAR itemText[MAX_PATH] = {};
    wcsncpy(itemText, name.c_str(), ARRAYSIZE(itemText) - 1);

    LVFINDINFOW findInfo = {};
    findInfo.flags = LVFI_STRING;
    findInfo.psz = itemText;
    int index = (int)SendMessageW(listView, LVM_FINDITEMW, (WPARAM)-1,
                                  (LPARAM)&findInfo);
    if (index < 0) {
        return false;
    }

    std::vector<POINT> used;
    int count = (int)SendMessageW(listView, LVM_GETITEMCOUNT, 0, 0);
    for (int i = 0; i < count; i++) {
        POINT point = {};
        if (SendMessageW(listView, LVM_GETITEMPOSITION, i, (LPARAM)&point)) {
            used.push_back(point);
        }
    }

    RECT client = {};
    GetClientRect(listView, &client);
    POINT target = {-1, -1};
    if (!ComputeRightmostSlot(used, client.right, client.bottom, &target)) {
        return false;
    }

    SendMessageW(listView, LVM_SETITEMPOSITION32, index, (LPARAM)&target);
    return true;
}

bool PositionItemRightmost(IFolderView* folderView,
                           IShellView* view,
                           const std::wstring& lnkPath) {
    HWND viewWindow = nullptr;
    if (FAILED(view->GetWindow(&viewWindow)) || !viewWindow) {
        return false;
    }

    RECT client = {};
    GetClientRect(viewWindow, &client);

    // 收集所有图标当前占用的网格位置
    std::vector<POINT> used;
    int itemCount = 0;
    if (FAILED(folderView->ItemCount(SVGIO_ALLVIEW, &itemCount))) {
        return false;
    }
    for (int i = 0; i < itemCount; i++) {
        PITEMID_CHILD child = nullptr;
        if (FAILED(folderView->Item(i, &child))) {
            continue;
        }
        POINT point = {};
        if (SUCCEEDED(folderView->GetItemPosition(child, &point))) {
            used.push_back(point);
        }
        CoTaskMemFree(child);
    }

    POINT target = {-1, -1};
    if (!ComputeRightmostSlot(used, client.right, client.bottom, &target)) {
        return false;
    }

    IShellFolder* folder = nullptr;
    if (FAILED(folderView->GetFolder(IID_PPV_ARGS(&folder)))) {
        return false;
    }

    PIDLIST_RELATIVE child = nullptr;
    ULONG eaten = 0;
    HRESULT hr = folder->ParseDisplayName(nullptr, nullptr,
                                          (PWSTR)lnkPath.c_str(), &eaten,
                                          &child, nullptr);
    folder->Release();
    if (FAILED(hr) || !child) {
        return false;
    }

    PCUITEMID_CHILD pidlArray[1] = {child};
    hr = folderView->SelectAndPositionItems(1, pidlArray, &target,
                                            SVSI_POSITIONITEM);
    CoTaskMemFree(child);
    return SUCCEEDED(hr);
}

// 新建图标后调用。桌面开启“自动排列图标”时，资源管理器会自己重排，这里设置的位置会被忽略。
void PlaceShortcutRightmost(const std::wstring& lnkPath) {
    IShellWindows* shellWindows = nullptr;
    if (FAILED(CoCreateInstance(CLSID_ShellWindows, nullptr, CLSCTX_ALL,
                                IID_PPV_ARGS(&shellWindows)))) {
        return;
    }

    bool placed = false;

    VARIANT location = {};
    location.vt = VT_I4;
    location.lVal = CSIDL_DESKTOP;
    VARIANT empty = {};
    long hwnd = 0;
    IDispatch* dispatch = nullptr;
    if (SUCCEEDED(shellWindows->FindWindowSW(&location, &empty, SWC_DESKTOP,
                                             &hwnd, SWFO_NEEDDISPATCH,
                                             &dispatch))) {
        IServiceProvider* serviceProvider = nullptr;
        IShellBrowser* browser = nullptr;
        IShellView* view = nullptr;
        IFolderView* folderView = nullptr;

        if (SUCCEEDED(
                dispatch->QueryInterface(IID_PPV_ARGS(&serviceProvider))) &&
            SUCCEEDED(serviceProvider->QueryService(
                SID_STopLevelBrowser, IID_PPV_ARGS(&browser))) &&
            SUCCEEDED(browser->QueryActiveShellView(&view)) &&
            SUCCEEDED(view->QueryInterface(IID_PPV_ARGS(&folderView)))) {
            placed = PositionItemRightmost(folderView, view, lnkPath);
        }

        if (folderView) {
            folderView->Release();
        }
        if (view) {
            view->Release();
        }
        if (browser) {
            browser->Release();
        }
        if (serviceProvider) {
            serviceProvider->Release();
        }
        dispatch->Release();
    }

    shellWindows->Release();

    if (!placed) {
        // COM 路径失败（例如视图还没准备好）时，直接操作桌面列表控件再试一次。
        placed = PlaceViaListView(lnkPath);
    }

    if (placed) {
        Wh_Log(L"Placed %s at the rightmost column", lnkPath.c_str());
    } else {
        Wh_Log(L"Could not position %s (auto arrange icons enabled?)",
               lnkPath.c_str());
    }
}

// ---------------------------------------------------------------------------
// 命名空间项：像“此电脑 / 回收站”那样的桌面图标（没有快捷方式箭头）
//
// 做法：每个盘符一个 CLSID，InProcServer32 指向本模组自己的 DLL；模组导出
// DllGetClassObject，返回一个“转发型 IShellFolder”——把调用原样转给该磁盘真实的
// 文件夹对象，因此枚举、打开、右键菜单、图标都与真实目录一致，而桌面上是一个
// 纯命名空间项，不是 .lnk。
//
// 实测要点（Windows 11 26H1）：
//   * ShellFolder\Attributes 必须含 SFGAO_HASSUBFOLDER，否则资源管理器不认、
//     点了也打不开；这里用 0x28000000（FOLDER | HASSUBFOLDER）。
//   * 目标磁盘路径通过 CLSID 下的 TargetFolderPath 传给扩展（DLL 自己读 HKCU）。
// ---------------------------------------------------------------------------

// CLSID 模板：最后一段前 10 位固定，后 2 位编码盘符序号，便于扫描清理自己的残留。
constexpr PCWSTR kNamespaceClsidPrefix = L"{D3F0A1B2-C3D4-4E5F-8A9B-0C1D2E3F4A";
constexpr PCWSTR kValueTargetPath = L"TargetFolderPath";
constexpr PCWSTR kValueDisplayName = L"DisplayName";
constexpr PCWSTR kNamespaceNameSpaceKey =
    L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Desktop\\NameSpace\\";

std::wstring GetOwnModulePath() {
    HMODULE module = nullptr;
    if (!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                                GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                            (LPCWSTR)&GetOwnModulePath, &module)) {
        return std::wstring();
    }
    WCHAR path[MAX_PATH * 2] = {};
    DWORD length = GetModuleFileNameW(module, path, ARRAYSIZE(path));
    if (length == 0 || length >= ARRAYSIZE(path)) {
        return std::wstring();
    }
    return path;
}

std::wstring NamespaceClsidForDriveIndex(int index) {
    WCHAR buffer[64];
    swprintf(buffer, ARRAYSIZE(buffer), L"%s%02X}", kNamespaceClsidPrefix, index);
    return buffer;
}

// 从 CLSID 里解出盘符序号，不是我们的 CLSID 就返回 -1。
int DriveIndexFromNamespaceClsid(const std::wstring& clsid) {
    size_t prefixLength = wcslen(kNamespaceClsidPrefix);
    if (clsid.size() != prefixLength + 3 ||
        _wcsnicmp(clsid.c_str(), kNamespaceClsidPrefix, prefixLength) != 0) {
        return -1;
    }
    WCHAR hex[3] = {clsid[prefixLength], clsid[prefixLength + 1], 0};
    WCHAR* end = nullptr;
    long value = wcstol(hex, &end, 16);
    if (!end || *end || value < 0 || value > 25) {
        return -1;
    }
    return (int)value;
}

bool ReadRegistryString(HKEY root,
                        const std::wstring& subKey,
                        PCWSTR valueName,
                        std::wstring* out) {
    HKEY key = nullptr;
    if (RegOpenKeyExW(root, subKey.c_str(), 0, KEY_READ, &key) !=
        ERROR_SUCCESS) {
        return false;
    }
    WCHAR buffer[512] = {};
    DWORD size = sizeof(buffer);
    DWORD type = 0;
    LSTATUS status = RegQueryValueExW(key, valueName, nullptr, &type,
                                      (LPBYTE)buffer, &size);
    RegCloseKey(key);
    if (status != ERROR_SUCCESS) {
        return false;
    }
    *out = buffer;
    return true;
}

bool SetRegistryString(HKEY root,
                       const std::wstring& subKey,
                       PCWSTR valueName,
                       const std::wstring& value) {
    HKEY key = nullptr;
    if (RegCreateKeyExW(root, subKey.c_str(), 0, nullptr, 0, KEY_WRITE, nullptr,
                        &key, nullptr) != ERROR_SUCCESS) {
        return false;
    }
    LSTATUS status = RegSetValueExW(
        key, valueName, 0, REG_SZ, (const BYTE*)value.c_str(),
        (DWORD)((value.size() + 1) * sizeof(WCHAR)));
    RegCloseKey(key);
    return status == ERROR_SUCCESS;
}

bool SetRegistryDword(HKEY root,
                      const std::wstring& subKey,
                      PCWSTR valueName,
                      DWORD value) {
    HKEY key = nullptr;
    if (RegCreateKeyExW(root, subKey.c_str(), 0, nullptr, 0, KEY_WRITE, nullptr,
                        &key, nullptr) != ERROR_SUCCESS) {
        return false;
    }
    LSTATUS status =
        RegSetValueExW(key, valueName, 0, REG_DWORD, (const BYTE*)&value,
                       sizeof(value));
    RegCloseKey(key);
    return status == ERROR_SUCCESS;
}

void DeleteRegistryTree(HKEY root, const std::wstring& subKey) {
    // 两层的键足够（CLSID\{guid}\Instance 等），用 shell 的删除更省事：
    // 这里手工递归，避免依赖 SHDeleteKey 之外的组件。
    HKEY key = nullptr;
    if (RegOpenKeyExW(root, subKey.c_str(), 0, KEY_READ | KEY_WRITE, &key) !=
        ERROR_SUCCESS) {
        return;
    }
    for (;;) {
        WCHAR name[256] = {};
        DWORD nameLength = ARRAYSIZE(name);
        if (RegEnumKeyExW(key, 0, name, &nameLength, nullptr, nullptr, nullptr,
                          nullptr) != ERROR_SUCCESS) {
            break;
        }
        HKEY child = nullptr;
        if (RegOpenKeyExW(key, name, 0, KEY_READ | KEY_WRITE, &child) ==
            ERROR_SUCCESS) {
            RegDeleteKeyW(key, name);
            RegCloseKey(child);
        }
    }
    RegCloseKey(key);
    RegDeleteKeyW(root, subKey.c_str());
}

// 取磁盘自己的图标位置（形如 "C:\Windows\system32\imageres.dll,-35"），
// 光盘会拿到 autorun.inf 里指定的图标。
std::wstring GetDriveIconLocation(PCWSTR root) {
    SHFILEINFOW fileInfo = {};
    if (!SHGetFileInfoW(root, 0, &fileInfo, sizeof(fileInfo),
                        SHGFI_ICONLOCATION) ||
        !fileInfo.szDisplayName[0]) {
        return std::wstring();
    }
    WCHAR buffer[MAX_PATH + 16];
    swprintf(buffer, ARRAYSIZE(buffer), L"%s,%d", fileInfo.szDisplayName,
             fileInfo.iIcon);
    return buffer;
}

// ---- 转发型 IShellFolder（在 explorer 进程内被 shell 创建）------------------

#pragma pack(push, 1)
struct NamespaceItemId {
    USHORT cb;
    GUID guid;
};
#pragma pack(pop)

class NamespaceFolder : public IShellFolder, public IPersistFolder2 {
  public:
    explicit NamespaceFolder(const GUID& guid) : m_guid(guid) { m_ref = 1; }
    virtual ~NamespaceFolder() {
        if (m_target) {
            m_target->Release();
        }
        if (m_pidl) {
            CoTaskMemFree(m_pidl);
        }
    }

    STDMETHODIMP QueryInterface(REFIID riid, void** ppv) override {
        if (!ppv) {
            return E_POINTER;
        }
        if (riid == IID_IUnknown || riid == IID_IShellFolder) {
            *ppv = static_cast<IShellFolder*>(this);
        } else if (riid == IID_IPersistFolder || riid == IID_IPersistFolder2 ||
                   riid == IID_IPersist) {
            *ppv = static_cast<IPersistFolder2*>(this);
        } else {
            *ppv = nullptr;
            return E_NOINTERFACE;
        }
        AddRef();
        return S_OK;
    }
    STDMETHODIMP_(ULONG) AddRef() override {
        return (ULONG)InterlockedIncrement(&m_ref);
    }
    STDMETHODIMP_(ULONG) Release() override {
        LONG ref = InterlockedDecrement(&m_ref);
        if (ref == 0) {
            delete this;
        }
        return (ULONG)ref;
    }

    STDMETHODIMP ParseDisplayName(HWND hwnd,
                                  IBindCtx* pbc,
                                  LPWSTR name,
                                  ULONG* eaten,
                                  PIDLIST_RELATIVE* pidlOut,
                                  ULONG* attributes) override {
        if (!pidlOut) {
            return E_POINTER;
        }
        *pidlOut = nullptr;
        if (eaten) {
            *eaten = 0;
        }
        std::wstring parsing = L"::" + ClsidToString();
        if (!name || !*name || _wcsicmp(name, parsing.c_str()) == 0) {
            *pidlOut = EmptyPidl();
            return *pidlOut ? S_OK : E_OUTOFMEMORY;
        }
        HRESULT hr = EnsureTarget();
        if (FAILED(hr)) {
            return hr;
        }
        return m_target->ParseDisplayName(hwnd, pbc, name, eaten, pidlOut,
                                          attributes);
    }

    STDMETHODIMP EnumObjects(HWND hwnd,
                             SHCONTF flags,
                             IEnumIDList** enumOut) override {
        HRESULT hr = EnsureTarget();
        if (FAILED(hr)) {
            return hr;
        }
        return m_target->EnumObjects(hwnd, flags, enumOut);
    }

    STDMETHODIMP BindToObject(PCUIDLIST_RELATIVE pidl,
                              IBindCtx* pbc,
                              REFIID riid,
                              void** ppv) override {
        if (IsRootPidl(pidl)) {
            return QueryInterface(riid, ppv);
        }
        HRESULT hr = EnsureTarget();
        if (FAILED(hr)) {
            return hr;
        }
        return m_target->BindToObject(pidl, pbc, riid, ppv);
    }

    STDMETHODIMP BindToStorage(PCUIDLIST_RELATIVE pidl,
                               IBindCtx* pbc,
                               REFIID riid,
                               void** ppv) override {
        HRESULT hr = EnsureTarget();
        if (FAILED(hr)) {
            return hr;
        }
        return m_target->BindToStorage(pidl, pbc, riid, ppv);
    }

    STDMETHODIMP CompareIDs(LPARAM param,
                            PCUIDLIST_RELATIVE pidl1,
                            PCUIDLIST_RELATIVE pidl2) override {
        HRESULT hr = EnsureTarget();
        if (FAILED(hr)) {
            return hr;
        }
        return m_target->CompareIDs(param, pidl1, pidl2);
    }

    STDMETHODIMP CreateViewObject(HWND hwnd, REFIID riid, void** ppv) override {
        HRESULT hr = EnsureTarget();
        if (FAILED(hr)) {
            return hr;
        }
        // 目标就是磁盘根目录，直接把视图交给它，内容/工具栏都正确。
        return m_target->CreateViewObject(hwnd, riid, ppv);
    }

    STDMETHODIMP GetAttributesOf(UINT cidl,
                                 PCUITEMID_CHILD_ARRAY apidl,
                                 SFGAOF* attributes) override {
        if (IsRootRequest(cidl, apidl)) {
            SFGAOF supported = SFGAO_FOLDER | SFGAO_BROWSABLE |
                               SFGAO_HASSUBFOLDER | SFGAO_STORAGEANCESTOR |
                               SFGAO_CANCOPY | SFGAO_CANLINK | SFGAO_CANDELETE;
            *attributes = supported & *attributes;
            return S_OK;
        }
        HRESULT hr = EnsureTarget();
        if (FAILED(hr)) {
            return hr;
        }
        return m_target->GetAttributesOf(cidl, apidl, attributes);
    }

    STDMETHODIMP GetUIObjectOf(HWND hwnd,
                               UINT cidl,
                               PCUITEMID_CHILD_ARRAY apidl,
                               REFIID riid,
                               UINT* reserved,
                               void** ppv) override {
        HRESULT hr = EnsureTarget();
        if (FAILED(hr)) {
            return hr;
        }
        if (cidl == 1 && IsRootPidl(apidl ? apidl[0] : nullptr)) {
            // 根节点（磁盘本身）的图标、右键菜单都从目标目录取。
            PCUITEMID_CHILD empty = nullptr;
            return m_target->GetUIObjectOf(hwnd, 1, &empty, riid, reserved, ppv);
        }
        return m_target->GetUIObjectOf(hwnd, cidl, apidl, riid, reserved, ppv);
    }

    STDMETHODIMP GetDisplayNameOf(PCUITEMID_CHILD pidl,
                                  SHGDNF flags,
                                  STRRET* name) override {
        if (!name) {
            return E_POINTER;
        }
        if (IsRootPidl(pidl)) {
            if (flags & SHGDN_FORPARSING) {
                return StringToStrRet(L"::" + ClsidToString(), name);
            }
            return StringToStrRet(DisplayName(), name);
        }
        HRESULT hr = EnsureTarget();
        if (FAILED(hr)) {
            return hr;
        }
        return m_target->GetDisplayNameOf(pidl, flags, name);
    }

    STDMETHODIMP SetNameOf(HWND hwnd,
                           PCUITEMID_CHILD pidl,
                           LPCOLESTR newName,
                           SHGDNF flags,
                           PITEMID_CHILD* pidlOut) override {
        HRESULT hr = EnsureTarget();
        if (FAILED(hr)) {
            return hr;
        }
        return m_target->SetNameOf(hwnd, pidl, newName, flags, pidlOut);
    }

    STDMETHODIMP GetClassID(CLSID* clsid) override {
        if (!clsid) {
            return E_POINTER;
        }
        *clsid = m_guid;
        return S_OK;
    }

    STDMETHODIMP Initialize(PCIDLIST_ABSOLUTE pidl) override {
        if (!pidl) {
            return E_INVALIDARG;
        }
        if (m_pidl) {
            CoTaskMemFree(m_pidl);
            m_pidl = nullptr;
        }
        m_pidl = ILClone(pidl);
        return m_pidl ? S_OK : E_OUTOFMEMORY;
    }

    STDMETHODIMP GetCurFolder(PIDLIST_ABSOLUTE* pidl) override {
        if (!pidl) {
            return E_POINTER;
        }
        *pidl = m_pidl ? ILClone(m_pidl) : nullptr;
        return *pidl ? S_OK : E_FAIL;
    }

  private:
    static PIDLIST_RELATIVE EmptyPidl() {
        auto* pidl = (PIDLIST_RELATIVE)CoTaskMemAlloc(2);
        if (pidl) {
            pidl->mkid.cb = 0;
        }
        return pidl;
    }

    static bool IsRootPidl(PCUIDLIST_RELATIVE pidl) {
        return !pidl || ILGetSize(pidl) == 0;
    }

    static bool IsRootRequest(UINT cidl, PCUITEMID_CHILD_ARRAY apidl) {
        if (cidl == 0) {
            return true;
        }
        if (cidl == 1) {
            return IsRootPidl(apidl ? apidl[0] : nullptr);
        }
        return false;
    }

    static HRESULT StringToStrRet(const std::wstring& text, STRRET* out) {
        size_t bytes = (text.size() + 1) * sizeof(WCHAR);
        LPWSTR buffer = (LPWSTR)CoTaskMemAlloc(bytes);
        if (!buffer) {
            return E_OUTOFMEMORY;
        }
        memcpy(buffer, text.c_str(), bytes);
        out->uType = STRRET_WSTR;
        out->pOleStr = buffer;
        return S_OK;
    }

    std::wstring ClsidToString() const {
        WCHAR buffer[64] = {};
        StringFromGUID2(m_guid, buffer, ARRAYSIZE(buffer));
        return buffer;
    }

    std::wstring RegistrySubKey() const {
        return L"Software\\Classes\\CLSID\\" + ClsidToString();
    }

    std::wstring DisplayName() {
        if (!m_displayName.empty()) {
            return m_displayName;
        }
        ReadRegistryString(HKEY_CURRENT_USER, RegistrySubKey(),
                           kValueDisplayName, &m_displayName);
        if (m_displayName.empty()) {
            m_displayName = L"磁盘";
        }
        return m_displayName;
    }

    HRESULT EnsureTarget() {
        if (m_target) {
            return S_OK;
        }
        std::wstring path;
        if (!ReadRegistryString(HKEY_CURRENT_USER, RegistrySubKey(),
                                kValueTargetPath, &path) ||
            path.empty()) {
            return E_FAIL;
        }
        PIDLIST_ABSOLUTE pidl = nullptr;
        if (FAILED(SHParseDisplayName(path.c_str(), nullptr, &pidl, 0,
                                      nullptr))) {
            return E_FAIL;
        }
        HRESULT hr =
            SHBindToObject(nullptr, pidl, nullptr, IID_PPV_ARGS(&m_target));
        CoTaskMemFree(pidl);
        return hr;
    }

    LONG m_ref;
    GUID m_guid;
    PIDLIST_ABSOLUTE m_pidl = nullptr;
    IShellFolder* m_target = nullptr;
    std::wstring m_displayName;
};

class NamespaceFactory : public IClassFactory {
  public:
    virtual ~NamespaceFactory() = default;
    explicit NamespaceFactory(const GUID& guid) : m_guid(guid) { m_ref = 1; }
    STDMETHODIMP QueryInterface(REFIID riid, void** ppv) override {
        if (!ppv) {
            return E_POINTER;
        }
        if (riid == IID_IUnknown || riid == IID_IClassFactory) {
            *ppv = static_cast<IClassFactory*>(this);
            AddRef();
            return S_OK;
        }
        *ppv = nullptr;
        return E_NOINTERFACE;
    }
    STDMETHODIMP_(ULONG) AddRef() override {
        return (ULONG)InterlockedIncrement(&m_ref);
    }
    STDMETHODIMP_(ULONG) Release() override {
        LONG ref = InterlockedDecrement(&m_ref);
        if (ref == 0) {
            delete this;
        }
        return (ULONG)ref;
    }
    STDMETHODIMP CreateInstance(IUnknown* outer, REFIID riid, void** ppv) override {
        if (outer) {
            return CLASS_E_NOAGGREGATION;
        }
        auto* folder = new (std::nothrow) NamespaceFolder(m_guid);
        if (!folder) {
            return E_OUTOFMEMORY;
        }
        HRESULT hr = folder->QueryInterface(riid, ppv);
        folder->Release();
        return hr;
    }
    STDMETHODIMP LockServer(BOOL) override { return S_OK; }

  private:
    GUID m_guid;
    LONG m_ref;
};

// 供 shell 加载本模组 DLL 时调用（Windhawk 已把它加载进 explorer，这里只是加导出）。
//
// 注意：combaseapi.h 里已经声明过 DllGetClassObject/DllCanUnloadNow，这里再带
// __declspec(dllexport) 定义会触发 clang 的 “should not add 'dllexport'” 警告。
// 该警告本身无害（导出确实生效），这里显式关掉，保持编辑器面板干净。
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdll-attribute-on-redeclaration"

extern "C" __declspec(dllexport) HRESULT __stdcall DllGetClassObject(
    REFCLSID rclsid,
    REFIID riid,
    void** ppv) {
    auto* factory = new (std::nothrow) NamespaceFactory(rclsid);
    if (!factory) {
        return E_OUTOFMEMORY;
    }
    HRESULT hr = factory->QueryInterface(riid, ppv);
    factory->Release();
    return hr;
}

extern "C" __declspec(dllexport) HRESULT __stdcall DllCanUnloadNow() {
    return S_FALSE;
}

#pragma clang diagnostic pop

// ---- 注册表同步 -----------------------------------------------------------

// 返回 true 表示注册表有变化（新建或取值不同）——这种变化必须配一次
// SHChangeNotify(SHCNE_ASSOCCHANGED)，否则桌面不会重新枚举、插拔不生效。
// 新建的项把 "::{CLSID}" 放进 createdParsingNames，供之后定位。
bool SyncNamespaceItem(const Device& device,
                       std::vector<std::wstring>* createdParsingNames) {
    int index = towupper(device.targetPath[0]) - L'A';
    if (index < 0 || index > 25) {
        return false;
    }

    std::wstring clsid = NamespaceClsidForDriveIndex(index);
    std::wstring clsidKey = L"Software\\Classes\\CLSID\\" + clsid;
    std::wstring namespaceKey = kNamespaceNameSpaceKey + clsid;

    std::wstring existingName;
    std::wstring existingTarget;
    bool existed = ReadRegistryString(HKEY_CURRENT_USER, clsidKey,
                                      kValueDisplayName, &existingName) &&
                   !existingName.empty();
    ReadRegistryString(HKEY_CURRENT_USER, clsidKey, kValueTargetPath,
                       &existingTarget);
    bool changed = !existed || existingName != device.name ||
                   existingTarget != device.targetPath;

    std::wstring modulePath = GetOwnModulePath();
    if (modulePath.empty()) {
        return false;
    }

    SetRegistryString(HKEY_CURRENT_USER, clsidKey, nullptr, device.name);
    SetRegistryString(HKEY_CURRENT_USER, clsidKey, kValueDisplayName,
                      device.name);
    SetRegistryString(HKEY_CURRENT_USER, clsidKey, kValueTargetPath,
                      device.targetPath);
    SetRegistryString(HKEY_CURRENT_USER, clsidKey + L"\\InProcServer32", nullptr,
                      modulePath);
    SetRegistryString(HKEY_CURRENT_USER, clsidKey + L"\\InProcServer32",
                      L"ThreadingModel", L"Apartment");
    std::wstring iconLocation = GetDriveIconLocation(device.targetPath.c_str());
    if (!iconLocation.empty()) {
        SetRegistryString(HKEY_CURRENT_USER, clsidKey + L"\\DefaultIcon", nullptr,
                          iconLocation);
    }
    // 必须含 SFGAO_HASSUBFOLDER，否则资源管理器点了打不开。
    SetRegistryDword(HKEY_CURRENT_USER, clsidKey + L"\\ShellFolder",
                     L"Attributes", 0x28000000);
    SetRegistryString(HKEY_CURRENT_USER, namespaceKey, nullptr, device.name);

    if (changed) {
        Wh_Log(L"Namespace item %s: %s (%s)",
               existed ? L"updated" : L"created", device.name.c_str(),
               clsid.c_str());
        if (!existed && createdParsingNames) {
            createdParsingNames->push_back(L"::" + clsid);
        }
    }
    return changed;
}

void RemoveNamespaceItemByIndex(int index) {
    std::wstring clsid = NamespaceClsidForDriveIndex(index);
    DeleteRegistryTree(HKEY_CURRENT_USER, kNamespaceNameSpaceKey + clsid);
    DeleteRegistryTree(HKEY_CURRENT_USER, L"Software\\Classes\\CLSID\\" + clsid);
}

// 清掉所有不再需要的命名空间项（拔掉的盘、切换样式后的残留、被强杀后的孤儿）。
bool SweepNamespaceItems(const std::set<int>& presentIndexes) {
    bool removed = false;
    HKEY key = nullptr;
    std::vector<std::wstring> stale;
    if (RegOpenKeyExW(HKEY_CURRENT_USER,
                      L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Desktop\\NameSpace",
                      0, KEY_READ, &key) == ERROR_SUCCESS) {
        for (DWORD i = 0;; i++) {
            WCHAR name[128] = {};
            DWORD nameLength = ARRAYSIZE(name);
            if (RegEnumKeyExW(key, i, name, &nameLength, nullptr, nullptr,
                              nullptr, nullptr) != ERROR_SUCCESS) {
                break;
            }
            int index = DriveIndexFromNamespaceClsid(name);
            if (index >= 0 && !presentIndexes.contains(index)) {
                stale.push_back(name);
            }
        }
        RegCloseKey(key);
    }
    for (const auto& clsid : stale) {
        Wh_Log(L"Namespace item removed: %s", clsid.c_str());
        DeleteRegistryTree(HKEY_CURRENT_USER, kNamespaceNameSpaceKey + clsid);
        DeleteRegistryTree(HKEY_CURRENT_USER,
                           L"Software\\Classes\\CLSID\\" + clsid);
        removed = true;
    }
    return removed;
}

bool RemoveAllNamespaceItems() {
    bool removed = false;
    for (int index = 0; index < 26; index++) {
        std::wstring clsid = NamespaceClsidForDriveIndex(index);
        std::wstring name;
        if (ReadRegistryString(HKEY_CURRENT_USER,
                               kNamespaceNameSpaceKey + clsid, nullptr, &name)) {
            RemoveNamespaceItemByIndex(index);
            removed = true;
        } else {
            // 可能只剩 CLSID 半截，一并清掉
            std::wstring clsidKey = L"Software\\Classes\\CLSID\\" + clsid;
            std::wstring target;
            if (ReadRegistryString(HKEY_CURRENT_USER, clsidKey, kValueTargetPath,
                                   &target)) {
                RemoveNamespaceItemByIndex(index);
                removed = true;
            }
        }
    }
    return removed;
}

// 进程内第一次同步时强制刷新一次桌面命名空间（接管已有项、清掉遗留）。
bool g_namespaceRefreshDone = false;
// 通知限流：桌面刷新较重，最多每 2 秒一次（正常情况下只在增删时触发）。
DWORD g_lastNamespaceNotifyTick = 0;

// ---------------------------------------------------------------------------
// 主同步逻辑：让桌面图标和设备现状一致
// ---------------------------------------------------------------------------

// 只在工作线程访问。用来区分“用户自己动了图标”和“设备状态变化”，
// 避免把用户手动删除/改名的图标又抢回来。
std::set<std::wstring> g_dismissedMarkers;  // 用户删掉的图标，设备重新插入前不再重建
std::set<std::wstring> g_presentMarkers;    // 上一轮结束时桌面上确实有图标的设备
std::map<std::wstring, std::wstring> g_lastNames;  // 设备标记 -> 上一轮的显示名
std::wstring g_lastActiveDirectory;  // 上一次同步用的桌面目录

void Reconcile() {
    if (g_unloading) {
        return;
    }

    DWORD reconcileStart = GetTickCount();

    Settings settings = GetSettings();

    std::wstring directory = GetKnownFolderPath(
        settings.publicDesktop ? FOLDERID_PublicDesktop : FOLDERID_Desktop);
    if (directory.empty()) {
        Wh_Log(L"Failed to get the desktop folder");
        return;
    }
    if (directory.back() != L'\\') {
        directory += L'\\';
    }

    // 桌面位置变了（例如把图标从个人桌面改到公用桌面）：把旧位置里自己创建的图标清掉，
    // 否则会在另一处留下没人管理的残留图标。
    if (_wcsicmp(directory.c_str(), g_lastActiveDirectory.c_str()) != 0) {
        std::wstring inactiveDirectory = GetKnownFolderPath(
            settings.publicDesktop ? FOLDERID_Desktop : FOLDERID_PublicDesktop);
        if (!inactiveDirectory.empty()) {
            if (inactiveDirectory.back() != L'\\') {
                inactiveDirectory += L'\\';
            }
            if (_wcsicmp(inactiveDirectory.c_str(), directory.c_str()) != 0) {
                std::map<std::wstring, std::wstring> stale;
                ScanOwnShortcuts(inactiveDirectory, stale);
                for (const auto& [marker, path] : stale) {
                    DeleteShortcut(path);
                }
            }
        }
        // 上一轮的“图标还在/被用户删掉”记录是针对旧目录的，换目录后必须作废，
        // 否则上面的清理会被误判成用户手动删除，新位置就一个图标也建不出来。
        g_presentMarkers.clear();
        g_dismissedMarkers.clear();
        g_lastActiveDirectory = directory;
    }

    std::vector<Device> devices;
    AddDriveDevices(settings, devices);
    AddPortableDevices(settings, devices);

    // 诊断用：开启 Windhawk 的调试日志后，可据此确认轮询/事件是否在工作。
    Wh_Log(L"Reconcile: %zu device(s), style=%s", devices.size(),
           settings.useNamespace ? L"namespace" : L"shortcut");

    for (auto& device : devices) {
        SanitizeFileName(device.name);
        if (device.name.empty()) {
            device.name = L"Disk";
        }
    }

    // 命名空间项（无箭头）只处理盘符；便携设备仍走快捷方式。
    std::set<int> namespaceIndexes;
    bool namespaceChanged = false;
    std::vector<std::wstring> newNamespaceItems;
    if (settings.useNamespace) {
        for (const auto& device : devices) {
            if (device.pidl.get() || device.targetPath.empty()) {
                continue;
            }
            int index = towupper(device.targetPath[0]) - L'A';
            if (index < 0 || index > 25) {
                continue;
            }
            namespaceIndexes.insert(index);
            namespaceChanged |=
                SyncNamespaceItem(device, &newNamespaceItems);
        }
        namespaceChanged |= SweepNamespaceItems(namespaceIndexes);
    } else {
        namespaceChanged = RemoveAllNamespaceItems();
    }

    // 关键：命名空间项是注册表驱动的，增删必须通知 shell 重新枚举，
    // 否则插拔 U 盘后桌面不会新增/移除图标（只会在资源管理器启动时读一次）。
    DWORD nowTick = GetTickCount();
    bool notifyWanted =
        namespaceChanged || (settings.useNamespace && !g_namespaceRefreshDone);
    if (notifyWanted && nowTick - g_lastNamespaceNotifyTick >= 2000) {
        g_lastNamespaceNotifyTick = nowTick;
        if (settings.useNamespace) {
            g_namespaceRefreshDone = true;
        }
        NotifyShellAsync(SHCNE_ASSOCCHANGED);
        if (!newNamespaceItems.empty() && settings.placeRightmost) {
            // 图标要先由 shell 建出来才能定位，稍等再试。
            Sleep(400);
            for (const auto& parsingName : newNamespaceItems) {
                PlaceShortcutRightmost(parsingName);
            }
        }
    }

    std::set<std::wstring> wanted;
    for (const auto& device : devices) {
        if (settings.useNamespace && !device.pidl.get()) {
            continue;  // 盘符由命名空间项负责，旧的 .lnk 会被当成残留清掉
        }
        wanted.insert(device.marker);
    }

    std::map<std::wstring, std::wstring> own;
    ScanOwnShortcuts(directory, own);

    // 设备已经不在了：忘掉它的“已删除”记录和上次的名字，下次插入重新显示。
    for (auto it = g_dismissedMarkers.begin();
         it != g_dismissedMarkers.end();) {
        it = wanted.contains(*it) ? std::next(it) : g_dismissedMarkers.erase(it);
    }
    for (auto it = g_lastNames.begin(); it != g_lastNames.end();) {
        it = wanted.contains(it->first) ? std::next(it) : g_lastNames.erase(it);
    }

    // 上一轮桌面上还有、这一轮不见了：是用户自己删的，尊重用户，不要在下一轮又建回来。
    for (const auto& marker : g_presentMarkers) {
        if (wanted.contains(marker) && !own.contains(marker)) {
            g_dismissedMarkers.insert(marker);
            Wh_Log(L"Removed by the user, leaving it alone: %s", marker.c_str());
        }
    }

    g_presentMarkers.clear();

    for (auto& device : devices) {
        if (g_dismissedMarkers.contains(device.marker)) {
            continue;
        }

        if (settings.useNamespace && !device.pidl.get()) {
            continue;  // 盘符由命名空间项负责
        }

        auto lastNameIt = g_lastNames.find(device.marker);
        bool identityChanged =
            lastNameIt != g_lastNames.end() && lastNameIt->second != device.name;
        if (identityChanged) {
            // 换了介质或卷标变了（不是用户改名），按新名字重建。
            g_dismissedMarkers.erase(device.marker);
        }

        auto ownIt = own.find(device.marker);
        if (ownIt != own.end()) {
            if (!identityChanged) {
                // 已经有这个设备的图标了。哪怕用户改过名字也保持原样。
                g_lastNames[device.marker] = device.name;
                g_presentMarkers.insert(device.marker);
                continue;
            }
            DeleteShortcut(ownIt->second);
            own.erase(ownIt);
        }

        std::wstring path =
            ChooseShortcutPath(directory, device.name, own, device.marker);
        if (CreateShortcut(device, path)) {
            Wh_Log(L"Created %s (%s)", path.c_str(), device.marker.c_str());
            if (settings.placeRightmost) {
                PlaceShortcutRightmost(path);
            }
            own[device.marker] = path;
            g_lastNames[device.marker] = device.name;
            g_presentMarkers.insert(device.marker);
        }
    }

    // 清掉已经拔掉的设备留下的图标
    for (const auto& [marker, path] : own) {
        if (!wanted.contains(marker)) {
            DeleteShortcut(path);
        }
    }

    Wh_Log(L"Reconcile took %lu ms", GetTickCount() - reconcileStart);
}

void DeleteAllOwnShortcuts() {
    for (REFKNOWNFOLDERID folderId :
         {FOLDERID_Desktop, FOLDERID_PublicDesktop}) {
        std::wstring directory = GetKnownFolderPath(folderId);
        if (directory.empty()) {
            continue;
        }
        if (directory.back() != L'\\') {
            directory += L'\\';
        }

        std::map<std::wstring, std::wstring> own;
        ScanOwnShortcuts(directory, own);
        for (const auto& [marker, path] : own) {
            DeleteShortcut(path);
        }
    }

    // 图标是我们自己删的，不能把这份记录留给下一次启用（否则会被误判成用户手动删除，
    // 重新启用后一个图标都不建）。
    g_presentMarkers.clear();
    g_dismissedMarkers.clear();
    g_lastNames.clear();
    g_lastActiveDirectory.clear();
}

// ---------------------------------------------------------------------------
// 消息循环
// ---------------------------------------------------------------------------


LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam,
                         LPARAM lParam) {
    switch (message) {
        case kMsgShellChange:
            Wh_Log(L"Shell change notification received (wParam=%zu lParam=%zu)",
                   wParam, lParam);
            RequestReconcile();
            return 0;

        case WM_DEVICECHANGE:
            // 卷插入/拔出会以广播形式送达顶层窗口，是最直接的设备信号。
            if (wParam == DBT_DEVICEARRIVAL ||
                wParam == DBT_DEVICEREMOVECOMPLETE) {
                Wh_Log(L"WM_DEVICECHANGE: wParam=0x%zX", wParam);
                RequestReconcile();
            }
            return TRUE;

        case WM_CLOSE:
            PostQuitMessage(0);
            return 0;
    }

    return DefWindowProcW(hwnd, message, wParam, lParam);
}

ULONG RegisterShellChangeNotifications(HWND hwnd) {
    PIDLIST_ABSOLUTE desktopPidl = nullptr;
    if (FAILED(SHGetKnownFolderIDList(FOLDERID_Desktop, 0, nullptr,
                                      &desktopPidl))) {
        return 0;
    }

    SHChangeNotifyEntry entry = {};
    entry.pidl = desktopPidl;
    entry.fRecursive = TRUE;

    // 这个 pidl 由我们保存，注销时再释放。
    ULONG registration = SHChangeNotifyRegister(
        hwnd, SHCNRF_ShellLevel, kShellEvents, kMsgShellChange, 1, &entry);
    if (!registration) {
        Wh_Log(L"SHChangeNotifyRegister failed: %u", GetLastError());
        CoTaskMemFree(desktopPidl);
        return 0;
    }

    g_shellNotifyPidl = desktopPidl;
    return registration;
}

DWORD WINAPI WorkerThread(LPVOID) {
    HRESULT hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);

    // 注意：窗口类名必须每次加载都不一样。
    // 若沿用固定类名，模组被 Windhawk 重载时 RegisterClassExW 会因“类已存在”失败，
    // 窗口就会挂到上一个 DLL 实例的 WndProc 上——那个实例已被卸载（g_unloading=true），
    // 它的 Reconcile() 直接返回，于是定时器/设备广播/shell 通知全部被吞掉。
    WCHAR className[64];
    swprintf(className, ARRAYSIZE(className),
             L"WindhawkDesktopExternalDrives_%llu", GetTickCount64());
    WNDCLASSEXW windowClass = {};
    windowClass.cbSize = sizeof(windowClass);
    windowClass.lpfnWndProc = WndProc;
    windowClass.hInstance = GetModuleHandleW(nullptr);
    windowClass.lpszClassName = className;
    if (!RegisterClassExW(&windowClass)) {
        Wh_Log(L"RegisterClassExW failed: %u", GetLastError());
    }

    // 注意：必须是顶层窗口才能收到卷插拔的 WM_DEVICECHANGE 广播
    // （消息专用窗口收不到广播）。这里创建为隐藏、无激活的弹出窗口，不显示任何内容。
    HWND hwnd = CreateWindowExW(
        WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE, className, L"", WS_POPUP, 0, 0,
        0, 0, nullptr, nullptr, windowClass.hInstance, nullptr);
    if (hwnd) {
        ShowWindow(hwnd, SW_HIDE);
        Wh_Log(L"Worker window created (class=%s, hwnd=%p)", className,
               (void*)hwnd);
    }
    if (!hwnd) {
        Wh_Log(L"CreateWindowExW failed: %u", GetLastError());
        if (SUCCEEDED(hr)) {
            CoUninitialize();
        }
        return 0;
    }
    g_hwnd = hwnd;

    ULONG shellRegistration = RegisterShellChangeNotifications(hwnd);

    Reconcile();

    // 主循环：用事件等待代替 WM_TIMER，轮询不依赖窗口消息；
    // 设备变化 / shell 通知 / 设置变更都只是置位 g_reconcileEvent。
    HANDLE waitHandles[2] = {g_stopEvent, g_reconcileEvent};
    for (;;) {
        int pollSeconds = GetSettings().pollSeconds;
        DWORD pollMs = pollSeconds > 0 ? (DWORD)pollSeconds * 1000 : INFINITE;

        DWORD waitResult = MsgWaitForMultipleObjects(2, waitHandles, FALSE,
                                                    pollMs, QS_ALLINPUT);
        if (waitResult == WAIT_OBJECT_0) {
            Wh_Log(L"Stop event signalled");
            break;
        }

        bool settingsChanged = false;
        bool quitRequested = false;
        MSG msg;
        while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) {
                Wh_Log(L"WM_QUIT received");
                quitRequested = true;
                break;
            }
            if (!msg.hwnd && msg.message == kMsgSettingsChanged) {
                settingsChanged = true;
                continue;
            }
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
        if (quitRequested) {
            break;
        }

        bool isPoll = waitResult == WAIT_TIMEOUT;
        bool isRequested = waitResult == WAIT_OBJECT_0 + 1;
        if (isPoll || isRequested || settingsChanged) {
            if (isPoll) {
                Wh_Log(L"Poll tick (%d s)", pollSeconds);
            }
            Reconcile();
        }
    }

    if (shellRegistration) {
        SHChangeNotifyDeregister(shellRegistration);
    }
    if (g_shellNotifyPidl) {
        CoTaskMemFree(g_shellNotifyPidl);
        g_shellNotifyPidl = nullptr;
    }

    DestroyWindow(hwnd);
    g_hwnd = nullptr;
    UnregisterClassW(className, windowClass.hInstance);

    if (g_unloading) {
        DeleteAllOwnShortcuts();
    }

    Wh_Log(L"WorkerThread exiting (unloading=%d)", g_unloading ? 1 : 0);

    if (SUCCEEDED(hr)) {
        CoUninitialize();
    }
    return 0;
}

// ---------------------------------------------------------------------------
// Windhawk 入口
// ---------------------------------------------------------------------------

BOOL Wh_ModInit() {
    Wh_Log(L">");

    // 模块可能在同一个进程里被重新启用，这里必须复位。
    g_unloading = false;
    g_namespaceRefreshDone = false;

    if (g_stopEvent) {
        CloseHandle(g_stopEvent);
    }
    if (g_reconcileEvent) {
        CloseHandle(g_reconcileEvent);
    }
    g_stopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_reconcileEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    if (!g_stopEvent || !g_reconcileEvent) {
        Wh_Log(L"CreateEvent failed: %u", GetLastError());
        return FALSE;
    }

    LoadSettings();

    g_thread = CreateThread(nullptr, 0, WorkerThread, nullptr, 0, &g_threadId);
    if (!g_thread) {
        Wh_Log(L"CreateThread failed: %u", GetLastError());
        return FALSE;
    }

    return TRUE;
}

void Wh_ModSettingsChanged() {
    Wh_Log(L">");

    LoadSettings();

    // 同时用线程消息和事件通知，任一通道可用即可。
    if (g_threadId) {
        PostThreadMessageW(g_threadId, kMsgSettingsChanged, 0, 0);
    }
    RequestReconcile();
}

void Wh_ModUninit() {
    Wh_Log(L">");

    g_unloading = true;

    if (g_stopEvent) {
        SetEvent(g_stopEvent);
    }
    if (g_threadId) {
        // 兼容旧路径：万一工作线程没在看事件，也让它从消息循环里退出。
        PostThreadMessageW(g_threadId, WM_QUIT, 0, 0);
    }

    if (g_thread) {
        if (WaitForSingleObject(g_thread, 2000) != WAIT_OBJECT_0) {
            Wh_Log(L"Worker thread did not stop in time");
        }
        CloseHandle(g_thread);
        g_thread = nullptr;
    }

    if (g_stopEvent) {
        CloseHandle(g_stopEvent);
        g_stopEvent = nullptr;
    }
    if (g_reconcileEvent) {
        CloseHandle(g_reconcileEvent);
        g_reconcileEvent = nullptr;
    }

    // 命名空间项写在 HKCU，卸载时必须清掉，避免桌面上留下打不开的死图标。
    if (RemoveAllNamespaceItems()) {
        NotifyShellAsync(SHCNE_ASSOCCHANGED);
    }
}
