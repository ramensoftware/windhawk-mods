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
// @compilerOptions -lole32 -lshell32 -luuid -ladvapi32
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
  mounted ISO images), floppy disks and memory cards.
- Only volumes are shown, matching macOS: phones (MTP/PTP, iPhone) are not mounted as volumes
  and therefore never appear on the desktop.
- Icons are taken from the volume itself. The shell derives them from the volume label,
  `autorun.inf` or the device description; the mod never substitutes its own icons.
- Icon names combine the volume label with the drive letter. When the label is empty, the
  device type name reported by the shell is used, for example `U 盘 (F)` or `CD-ROM (H)`.
  
![Desktop icons](https://i.imgur.com/oTo4VIr.png)

## How it works

By default the mod registers a shell namespace item for each drive. Such an item belongs to the
same category of desktop objects as "This PC" and "Recycle Bin": no `.lnk` file is created on
the desktop and no shortcut arrow is drawn. The item is registered through the shell's built-in
delegated folder (`InProcServer32` is `shell32.dll`, `Instance\InitPropertyBag\TargetFolderPath`
is the volume), so the shell uses its own folder object: no mod code is loaded by the shell or by
any other process, and the item behaves exactly like a built-in folder.

The mod runs inside `explorer.exe`, listens for the shell's drive and media change
notifications, and additionally polls at a configurable interval. Every synchronization pass
brings the desktop icons in line with the devices that are currently attached.

## Known limitations and cleanup

- External hard disks are recognised by bus type (USB, IEEE 1394, SD/MMC, virtual). Disks
  attached through Thunderbolt or eSATA that report an internal bus may not be recognised;
  internal disks are never shown.
- Portable devices (MTP/PTP, and phones in general) are not shown, because they are not volumes.
- Removing a device deletes its icon immediately; there is no eject command. Use the system
  tray safe-removal function before unplugging.
- Unplugging a device deletes the corresponding item; disabling or uninstalling the mod deletes
  every item created by it.
- The registry keys are **volatile** (`REG_OPTION_VOLATILE`): they only exist in memory, are
  never written to disk, and disappear on restart even after a crash. If the mod is terminated
  forcefully, the leftover in-memory keys are removed on the next start.
- Updating the mod takes effect immediately; no Explorer restart is required, because the item is
  served by the shell itself.
- If plugging or unplugging a disk does not update the desktop, set the polling interval to
  about 10 seconds in the settings.

## Links

- GitHub: <https://github.com/AomiRaku/desktop-disk-icon>

---

# 中文部分

## 功能

- 覆盖 USB 闪存盘、外接硬盘与固态硬盘、光盘（CD/DVD/蓝光及已挂载的 ISO 映像）、软盘、
  存储卡。
- 只显示「卷」，与 macOS 一致：手机（MTP/PTP、iPhone）不会被挂载成卷，因此不会出现在桌面。
- 图标取自磁盘本身，由系统依据卷标、`autorun.inf` 或设备描述提供，模组不做替换。
- 图标名称由卷标与盘符组合而成；卷标为空时采用系统提供的设备类型名称，
  例如 `U 盘 (F)`、`CD-ROM (H)`。

## 实现方式

默认情况下，模组为每个驱动器注册一个 shell 命名空间项，与“此电脑”“回收站”属于同一类桌面
对象：桌面不产生 `.lnk` 文件，也不显示快捷方式箭头。该项通过 shell 自带的委托文件夹注册
（`InProcServer32` 为 `shell32.dll`，`Instance\InitPropertyBag\TargetFolderPath` 为该卷根
目录），因此使用的是系统自己的文件夹对象：shell 与其他进程都不会加载模组代码，行为与系统
内置文件夹完全一致。

模组运行于 `explorer.exe`，监听 shell 的驱动器与介质变更通知，并以定时轮询作为补充；
每次同步均使桌面图标与当前设备状态保持一致。

## 已知限制与清理行为

- 外接硬盘依据设备总线类型识别（USB、IEEE 1394、SD/MMC、虚拟）；雷雳或 eSATA 接口且被
  报告为内置总线的硬盘可能无法识别，内置磁盘始终不会显示。
- 便携设备（MTP/PTP 及手机）不显示，因为它们不是卷。
- 拔出设备时直接移除图标，不提供弹出操作；请沿用系统托盘中的安全删除功能。
- 拔出设备时删除对应项；禁用或卸载模组时删除全部由本模组创建的项目。
- 注册表项是**易失性键**（`REG_OPTION_VOLATILE`）：只存在于内存中，永不写入磁盘；
  即使异常终止，重启后也会自然消失。被强制终止时残留的内存键会在下次启动时清除。
- 更新模组即时生效，无需重启资源管理器（项本身由系统实现）。
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
- showEmptyDrives: false
  $name: Show drives without media
  $name:zh-CN: 显示无介质驱动器
  $description: Also shows empty card reader slots and optical drives without a disc.
  $description:zh-CN: 同时显示未插入存储卡的读卡器插槽，以及未放入光盘的光盘驱动器。

- pollSeconds: 0
  $name: Polling interval (seconds)
  $name:zh-CN: 轮询间隔（秒）
  $description: Interval of the periodic synchronization, which covers device changes that raise no shell notification. 0 relies on shell events only; set it to around 10 if plugging or unplugging a disk does not update the desktop.
  $description:zh-CN: 周期同步间隔，用于捕获未触发 shell 通知的设备变更。设为 0 时仅依赖系统事件；若插拔磁盘后桌面没有反应，可将其设为 10 左右再试。
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <dbt.h>
#include <shlguid.h>
#include <shlobj.h>
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
    bool showEmptyDrives = false;
    int pollSeconds = 0;
};

Settings g_settings;
std::mutex g_settingsMutex;

std::atomic<bool> g_unloading{false};
HANDLE g_thread = nullptr;
HANDLE g_stopEvent = nullptr;       // 请求工作线程退出（手动重置）
HANDLE g_reconcileEvent = nullptr;  // 请求立刻同步一次（自动重置）
DWORD g_threadId = 0;
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
    s.showEmptyDrives = Wh_GetIntSetting(L"showEmptyDrives") != 0;


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


// ---------------------------------------------------------------------------
// 设备枚举
// ---------------------------------------------------------------------------

struct Device {
    std::wstring name;        // 桌面显示名
    std::wstring targetPath;  // 盘符路径，例如 "F:\"
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
        device.name = base + L" (" + root[0] + L")";
        device.targetPath = root;
        out.push_back(std::move(device));
    }
}


// 前置声明：定义在下方（见「shell 通知：一律异步发送」）。
void NotifyShell(LONG event, PCWSTR path = nullptr);

// 扫描桌面目录，找出本模组创建的快捷方式：标记 -> 路径。
// 选一个不会和用户自己的文件冲突的名字。
// ---------------------------------------------------------------------------
// shell 通知：直接在调用线程上发出
//
// SHCNF_FLUSHNOWAIT 表示不等待监听者处理完毕，因此不会阻塞调用线程，也就不需要
// 额外的工作线程。之前的实现为每条通知开一个线程且不等它结束，那些线程会活过
// Wh_ModUninit，Windhawk 卸载 DLL 后仍在执行 → 崩溃。
// ---------------------------------------------------------------------------
void NotifyShell(LONG event, PCWSTR path) {
    if (path) {
        SHChangeNotify(event, SHCNF_PATHW | SHCNF_FLUSHNOWAIT, path, nullptr);
    } else {
        SHChangeNotify(event, SHCNF_IDLIST | SHCNF_FLUSHNOWAIT, nullptr, nullptr);
    }
}


// ---------------------------------------------------------------------------
// 命名空间项：像“此电脑 / 回收站”那样的桌面图标（没有快捷方式箭头）
//
// 每个盘符一个 CLSID，注册到 HKCU 的 Classes\CLSID 与 Desktop\NameSpace。
// 项本身由 shell 自带的委托文件夹实现：InProcServer32 指向 shell32.dll，
// Instance\InitPropertyBag\TargetFolderPath 指向该卷根目录。
// 因此枚举、打开、右键菜单、图标都与真实目录一致，桌面上没有 .lnk 文件。
//
// 实测要点（Windows 11 26H1）：
//   * Instance\InitPropertyBag\Attributes 必须为 0x11，否则项不显示。
//   * ShellFolder\Attributes 用 0xF080004D + FolderValueFlags 0x28。
// ---------------------------------------------------------------------------

// CLSID 模板：最后一段前 10 位固定，后 2 位编码盘符序号，便于扫描清理自己的残留。
constexpr PCWSTR kNamespaceClsidPrefix = L"{D3F0A1B2-C3D4-4E5F-8A9B-0C1D2E3F4A";
constexpr PCWSTR kValueTargetPath = L"TargetFolderPath";

// shell 自带的委托文件夹（Delegated Folder）：把命名空间项交给系统实现，
// 指向一个真实的文件系统目录，无需模组提供 COM 服务器。
constexpr PCWSTR kDelegateFolderClsid =
    L"{0E5AAE11-A475-4c5b-AB00-C66DE400274E}";
constexpr PCWSTR kValueDisplayName = L"DisplayName";
// shell 会把“某个 CLSID 实现了 IShellFolder({000214E6-…})”记在 Shell Extensions\Cached
// 下。这是我们的项附带的副作用键（数值 0xFFFF 表示已失效），必须一并清掉，
// 否则它会一直留在系统里、并可能在复用时被当成“已知无效”。
constexpr PCWSTR kShellExtCachedKey =
    L"Software\\Microsoft\\Windows\\CurrentVersion\\Shell Extensions\\Cached\\";

constexpr PCWSTR kNamespaceNameSpaceKey =
    L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Desktop\\NameSpace\\";


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

// 全部写入都用 REG_OPTION_VOLATILE（易失性键）：这些键只存在于内存中，
// 永不写入磁盘。模组卸载时主动删除，异常终止也会在重启后自然消失，
// 因此不会在系统注册表里留下任何持久残留。
constexpr DWORD kVolatileOptions = REG_OPTION_VOLATILE;

bool SetRegistryString(HKEY root,
                       const std::wstring& subKey,
                       PCWSTR valueName,
                       const std::wstring& value) {
    HKEY key = nullptr;
    if (RegCreateKeyExW(root, subKey.c_str(), 0, nullptr, kVolatileOptions,
                        KEY_WRITE, nullptr, &key, nullptr) != ERROR_SUCCESS) {
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
    if (RegCreateKeyExW(root, subKey.c_str(), 0, nullptr, kVolatileOptions,
                        KEY_WRITE, nullptr, &key, nullptr) != ERROR_SUCCESS) {
        return false;
    }
    LSTATUS status =
        RegSetValueExW(key, valueName, 0, REG_DWORD, (const BYTE*)&value,
                       sizeof(value));
    RegCloseKey(key);
    return status == ERROR_SUCCESS;
}

void DeleteRegistryTree(HKEY root, const std::wstring& subKey) {
    // RegDeleteTreeW 会连同所有子键一起删除。
    // 之前手写循环：子键下还有子键时 RegDeleteKeyW 会失败，而枚举下标 0 仍返回
    // 同一个名字，导致无限循环（挂死在 Wh_ModUninit 里）。
    RegDeleteTreeW(root, subKey.c_str());
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

// ---- 注册表同步 -----------------------------------------------------------

// 返回 true 表示注册表有变化（新建或取值不同）——这种变化必须配一次
// SHChangeNotify(SHCNE_ASSOCCHANGED)，否则桌面不会重新枚举、插拔不生效。
bool SyncNamespaceItem(const Device& device) {
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
    ReadRegistryString(HKEY_CURRENT_USER,
                       clsidKey + L"\\Instance\\InitPropertyBag",
                       L"TargetFolderPath", &existingTarget);
    bool changed = !existed || existingName != device.name ||
                   existingTarget != device.targetPath;

    SetRegistryString(HKEY_CURRENT_USER, clsidKey, nullptr, device.name);
    SetRegistryString(HKEY_CURRENT_USER, clsidKey, kValueDisplayName,
                      device.name);

    // 不实现文件夹对象本身，而是让 shell 自带的委托文件夹指向该卷：
    //   InProcServer32 = shell32.dll
    //   Instance\CLSID = {0E5AAE11-A475-4c5b-AB00-C66DE400274E}
    //   Instance\InitPropertyBag\{Attributes=0x11, TargetFolderPath=<卷根>}
    // 这样 shell 用的是它自己的文件夹对象，任何进程都不会加载模组代码。
    SetRegistryString(HKEY_CURRENT_USER, clsidKey + L"\\InProcServer32", nullptr,
                      L"%SystemRoot%\\system32\\shell32.dll");
    SetRegistryString(HKEY_CURRENT_USER, clsidKey + L"\\InProcServer32",
                      L"ThreadingModel", L"Apartment");
    SetRegistryString(HKEY_CURRENT_USER, clsidKey + L"\\Instance", L"CLSID",
                      kDelegateFolderClsid);
    SetRegistryDword(HKEY_CURRENT_USER,
                     clsidKey + L"\\Instance\\InitPropertyBag", L"Attributes",
                     0x11);
    SetRegistryString(HKEY_CURRENT_USER,
                      clsidKey + L"\\Instance\\InitPropertyBag",
                      L"TargetFolderPath", device.targetPath);
    std::wstring iconLocation = GetDriveIconLocation(device.targetPath.c_str());
    if (!iconLocation.empty()) {
        SetRegistryString(HKEY_CURRENT_USER, clsidKey + L"\\DefaultIcon", nullptr,
                          iconLocation);
    }
    SetRegistryDword(HKEY_CURRENT_USER, clsidKey + L"\\ShellFolder",
                     L"FolderValueFlags", 0x28);
    SetRegistryDword(HKEY_CURRENT_USER, clsidKey + L"\\ShellFolder",
                     L"Attributes", 0xF080004D);
    SetRegistryString(HKEY_CURRENT_USER, namespaceKey, nullptr, device.name);

    if (changed) {
        Wh_Log(L"Namespace item %s: %s (%s)",
               existed ? L"updated" : L"created", device.name.c_str(),
               clsid.c_str());

    }
    return changed;
}

void RemoveNamespaceItemByIndex(int index) {
    std::wstring clsid = NamespaceClsidForDriveIndex(index);
    DeleteRegistryTree(HKEY_CURRENT_USER, kNamespaceNameSpaceKey + clsid);
    DeleteRegistryTree(HKEY_CURRENT_USER, L"Software\\Classes\\CLSID\\" + clsid);
    DeleteRegistryTree(HKEY_CURRENT_USER, kShellExtCachedKey + clsid);
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
                           DeleteRegistryTree(HKEY_CURRENT_USER, kShellExtCachedKey + clsid);
        removed = true;
    }
    return removed;
}

bool RemoveAllNamespaceItems() {
    bool removed = false;
    for (int index = 0; index < 26; index++) {
        std::wstring clsid = NamespaceClsidForDriveIndex(index);
        // 即使条目本身已经不在了，也要清掉 shell 记录在该 CLSID 下的缓存书签。
        DeleteRegistryTree(HKEY_CURRENT_USER, kShellExtCachedKey + clsid);
        std::wstring name;
        if (ReadRegistryString(HKEY_CURRENT_USER,
                               kNamespaceNameSpaceKey + clsid, nullptr, &name)) {
            RemoveNamespaceItemByIndex(index);
            removed = true;
        } else {
            // 可能只剩 CLSID 半截，一并清掉
            std::wstring clsidKey = L"Software\\Classes\\CLSID\\" + clsid;
            std::wstring target;
            if (ReadRegistryString(
                  HKEY_CURRENT_USER, clsidKey + L"\\Instance\\InitPropertyBag",
                  kValueTargetPath, &target)) {
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
constexpr DWORD kNamespaceNotifyIntervalMs = 1000;

DWORD g_lastNamespaceNotifyTick = 0;
bool g_namespaceNotifyPending = false;  // 有变更待通知（节流窗口结束后补发）
// ---------------------------------------------------------------------------
// 主同步逻辑：让桌面图标和设备现状一致
// ---------------------------------------------------------------------------

void Reconcile() {
    if (g_unloading) {
        return;
    }

    DWORD reconcileStart = GetTickCount();

    Settings settings = GetSettings();

    std::vector<Device> devices;
    AddDriveDevices(settings, devices);

    // 诊断用：开启 Windhawk 的调试日志后，可据此确认轮询/事件是否在工作。
    Wh_Log(L"Reconcile: %zu drive(s)", devices.size());

    for (auto& device : devices) {
        SanitizeFileName(device.name);
        if (device.name.empty()) {
            device.name = L"Disk";
        }
    }

    // 每个盘符一个命名空间项；已拔出设备的项由 sweep 清掉。
    std::set<int> namespaceIndexes;
    bool namespaceChanged = false;

    for (const auto& device : devices) {
        if (device.targetPath.empty()) {
            continue;
        }
        int index = towupper(device.targetPath[0]) - L'A';
        if (index < 0 || index > 25) {
            continue;
        }
        namespaceIndexes.insert(index);
        namespaceChanged |= SyncNamespaceItem(device);
    }
    namespaceChanged |= SweepNamespaceItems(namespaceIndexes);

    // 关键：命名空间项是注册表驱动的，增删必须通知 shell 重新枚举，
    // 否则插拔 U 盘后桌面不会新增/移除图标（只会在资源管理器启动时读一次）。
    //
    // 通知要节流（ASSOCCHANGED 会让整个 shell 刷新），但被节流掉的那次不能丢失：
    // 记一个待发标记，由主循环在节流窗口结束时补发。否则连续插入两个磁盘时，
    // 第二次变更的通知被丢掉，而后续轮次发现注册表已是最新（changed == false），
    // 桌面就永远不会刷新。
    DWORD nowTick = GetTickCount();
    if (namespaceChanged || !g_namespaceRefreshDone) {
        g_namespaceNotifyPending = true;
    }
    if (g_namespaceNotifyPending &&
        nowTick - g_lastNamespaceNotifyTick >= kNamespaceNotifyIntervalMs) {
        g_lastNamespaceNotifyTick = nowTick;
        g_namespaceNotifyPending = false;
        g_namespaceRefreshDone = true;
        NotifyShell(SHCNE_ASSOCCHANGED);
    }

    Wh_Log(L"Reconcile took %lu ms", GetTickCount() - reconcileStart);
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

    ULONG shellRegistration = RegisterShellChangeNotifications(hwnd);

    Reconcile();

    // 主循环：用事件等待代替 WM_TIMER，轮询不依赖窗口消息；
    // 设备变化 / shell 通知 / 设置变更都只是置位 g_reconcileEvent。
    HANDLE waitHandles[2] = {g_stopEvent, g_reconcileEvent};
    for (;;) {
        int pollSeconds = GetSettings().pollSeconds;
        DWORD pollMs = pollSeconds > 0 ? (DWORD)pollSeconds * 1000 : INFINITE;

        // 有通知待发时把等待时间限制在节流窗口的剩余时间内，保证补发一定发生
        // （pollSeconds 为 0 时本来会无限等待）。
        if (g_namespaceNotifyPending) {
            DWORD elapsed = GetTickCount() - g_lastNamespaceNotifyTick;
            DWORD remaining = elapsed >= kNamespaceNotifyIntervalMs
                                  ? 50
                                  : kNamespaceNotifyIntervalMs - elapsed;
            if (remaining < pollMs) {
                pollMs = remaining;
            }
        }

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
    UnregisterClassW(className, windowClass.hInstance);

    if (g_unloading) {
        // 卸载路径：所有清理与最后一条通知都在本线程内完成。Wh_ModUninit 只负责
        // 等本线程结束——它返回后 Windhawk 会立即 FreeLibrary，之后不能再有模组
        // 代码运行。
        if (RemoveAllNamespaceItems()) {
            NotifyShell(SHCNE_ASSOCCHANGED);
        }
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

    // @include explorer.exe 也会匹配承载文件夹窗口的独立进程
    // （explorer.exe /factory,… -Embedding）。它们不需要也不应该跑同步：
    // 多个进程会重复创建图标，并互相把对方的删除误判成“用户删除了图标”。
    const wchar_t* cmdLine = GetCommandLineW();
    if (wcsstr(cmdLine, L"/factory") || wcsstr(cmdLine, L"-Embedding")) {
        Wh_Log(L"Folder window process, skipping");
        return FALSE;
    }

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

    // 旧版本把键写在真实注册表里（持久）。这里先清一遍，随后同步会以易失性键
    // 重新创建，保证不会同时存在持久键与易失键。
    RemoveAllNamespaceItems();

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
    if (g_thread) {
        // 不限时等待。Wh_ModUninit 运行在 Windhawk 引擎线程上，不是桌面线程，
        // 因此这里等待不会死锁；而返回后 DLL 会立即被卸载，不能留下仍在执行的
        // 模组代码（清理与最后一条通知都由工作线程自己完成）。
        WaitForSingleObject(g_thread, INFINITE);
        CloseHandle(g_thread);
        g_thread = nullptr;
    } else {
        // 工作线程从未启动，没有并发，直接在这里清理。
        if (RemoveAllNamespaceItems()) {
            NotifyShell(SHCNE_ASSOCCHANGED);
        }
    }

    if (g_stopEvent) {
        CloseHandle(g_stopEvent);
        g_stopEvent = nullptr;
    }
    if (g_reconcileEvent) {
        CloseHandle(g_reconcileEvent);
        g_reconcileEvent = nullptr;
    }


}
