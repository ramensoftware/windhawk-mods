# Phase 0 技术调研

## 参考源码和许可证

| 项目 | 已读代码及用途 | 许可证 |
| --- | --- | --- |
| Refined Quick Settings and Sound | 上游提交 `aa310d7d48d76c515042706a79ee0d6aece199df`。`HookControlCenter` 从焦点和 XAML 连接入口发现面板。`PrepareMixerRows` 识别 `AppVolumeLevel`，`RowModel` 用 `ItemFromContainer` 查找行模型，源码实际读取的是 `AppName` 等展示或音量相关字段。官方收录版也标注 GPL-3.0-only。 [上游源码与许可证](https://github.com/HeyOkay/Refined-Quick-Settings-and-Sound/tree/aa310d7d48d76c515042706a79ee0d6aece199df) | GPL-3.0-only。仅用作研究依据 |
| AppAudioRouter | 提交 `4ed32a9fda7a576b53b97922cf9d665236599d63`。检查 Rust 后端的 `routing/policy.rs`、`duplication/activation.rs`、`capture.rs`、`pump.rs` 和会话通知模块。`policy.rs` 给出 Win11 IID 和方法顺序。镜像代码按端点处理缓冲水位和写入，包含保留有限排队时延的补偿策略。 [Rust 后端](https://github.com/Eververdants/AppAudioRouter/tree/4ed32a9fda7a576b53b97922cf9d665236599d63/src-tauri/src/audio) | MIT |
| Nimbus Menu | 官方仓库快照 `6d372f22942b13c7bf1f93ef2b2ac3fa06859d58`。检查 `SndPolicy`、`SndRoute` 和 `SndCopyThread`，确认私有接口签名、端点路径和定向捕获到渲染的实现。作者仓库用于反馈，模块实现位于 Windhawk 官方仓库。 [模块源码](https://github.com/ramensoftware/windhawk-mods/blob/6d372f22942b13c7bf1f93ef2b2ac3fa06859d58/mods/nimbus-menu.wh.cpp) | 模块源码未显式声明许可证，官方仓库 README 规定这类提交默认 MIT |
| Microsoft Application Loopback | 提交 `434f6002bdf9cf9829406c3ff2b33387982d6168`。检查 `LoopbackCapture.cpp`、共享模式 PCM、完成回调和 capture packet 循环。 [示例源码](https://github.com/microsoft/Windows-classic-samples/tree/434f6002bdf9cf9829406c3ff2b33387982d6168/Samples/ApplicationLoopback) | MIT |
| Windhawk 官方模块仓库 | 验证模块元数据、`.wh.cpp` 单文件提交、作者 GitHub 元数据，以及模块默认许可证。诊断源码用 Windhawk 主仓库提交 `7413a57a3f964a6e0e11f491f388da103d947a5d` 的 API 声明编译检查。 [模块仓库规则](https://github.com/ramensoftware/windhawk-mods/blob/6d372f22942b13c7bf1f93ef2b2ac3fa06859d58/README.md) · [Windhawk 开发文档](https://github.com/ramensoftware/windhawk/wiki/creating-a-new-mod) | 模块按各自声明或默认规则确定 |

Refined 和 Nimbus 的目录站链接均用对应 GitHub 源码交叉验证。项目采用 MIT。当前提交独立实现探针和诊断，记录参考项目和接口证据。后续如直接引入 MIT 代码，随相关源码保留原始版权与许可声明。

## Shell 宿主与 XAML 入口

- 在 x64 Windows 11 26H2 `26300.9974`，查询运行中的 `ShellHost.exe` 模块表，确认加载 `ControlCenter.dll`、`Windows.UI.Xaml.dll`、`QuickActionsDataModel.dll` 和 `AudioHandlers.dll`。
- `dumpbin /headers` 读取本机 `ControlCenter.dll` 的 RSDS。使用 Microsoft 符号服务器 PDB `7938913B0445A1F8F50C355CC97647421`，DbgHelp 以 exact symbols 模式匹配成功。文件 SHA-256 为 `61fb606b734724b6172fc00e97caf19735a3b1b93a9648e9c8b7c57336c8d4cb`。
- 已匹配 `ControlCenterView` 和 `FullScreenPage` 的 `OnGotFocus` / `Connect`。更窄的 `VolumeMixerList` 同时有 `IControlOverrides::OnGotFocus`、`IFrameworkElementOverrides::OnApplyTemplate` 和 `IComponentConnector::Connect` 的实现符号。诊断使用前者的完整修饰名，并依靠 Windhawk 符号解析。RVA 仅用于核对证据。
- 在 Windhawk 1.7.3 便携版中实际加载只读诊断。任务栏声音输出页的 `VolumeMixerList::OnGotFocus` 命中，扫描到 4 个 `AppVolumeLevel` 滑块，`ItemFromContainer` 返回 `ControlCenter.QuickActionViewModel`。第一轮 CustomProperty 仅确认字段可查找，不能凭此判断 getter 返回真实值。
- 第二轮实际读取 5 个任务栏行。`AppName` 是 WinRT String，`SelectedOutput` 返回 UInt32，`PossibleOutputs` 是 `Windows.UI.Xaml.Interop.IBindableObservableVector`。`ProcessId`、`SessionIdentifier`、`SessionInstanceIdentifier` 和 `AppUserModelId` 未返回值。现阶段单设备预览直接使用已观察到的 Windows 模型，逐进程镜像仍需要另行证明身份映射。
- `AudioHandlers.dll` 的匹配 PDB 包含 `SessionInfoItem::get_Id`、`GetAppName`、`GetProperty` 和 `SetPersistedDefaultAudioEndpoint`。按 PDB 地址核对反编译结果，`get_Id` 生成 `SystemSettings_Audio_Mixer_App%d` 形式的设置项标识。`GetProperty` 处理 `SelectedOutput`、`PossibleOutputs`，`SetProperty` 的 `SelectedOutput` 分支进入原生逐应用策略。索引零清除该项的显式输出。正索引映射原生设备集合。需要在任务栏的模型包装层验证两项属性是否实际可用，以及设备热插拔时索引的有效期。静态证据 `ev_05c084b1c2d9e03843303b081a07ebae954887ab379d28c23e4e0764e9bb815a`。

位置候选为 `VolumeMixerList` 下名称为 `AppVolumeLevel` 的 Slider 所属 `ListViewItem`。Phase 1 首先验证原生行尾是否有足够空间容纳可键盘聚焦的原生按钮，覆盖回收、主题和高 DPI。行能否关联可路由身份取决于实际 XAML 模型提供的属性。仅凭显示名的同名和本地化情况无法安全决定目标 PID。

## 原生逐应用路由

- `RoGetActivationFactory` 对 `Windows.Media.Internal.AudioPolicyConfig` 的确切 Win11 IID `ab3d4648-e242-459f-b02f-541c70306324` 成功。用两份参考实现一致的 COM 方法顺序声明类型，并验证该进程的 `GetPersistedDefaultAudioEndpoint`。
- 对探针自身的 `eRender/eMultimedia` 写入另一个活动端点，读回相同路径，然后恢复调用前的值并再次读回。此测试证明本机构建的接口及端点路径可用。应用重建音频流后是否切换、不同角色、蓝牙变体以及现有会话迁移仍待测。
- 探针仅请求一个已知 IID。产品适配层应同时检查 Windows 版本、精确 IID、非修改性读取和端点状态。未知 ABI 应停用对应能力。不要调用全局清除接口。对于每个应用及角色，恢复时只回写模块仍拥有的值。

## Process Loopback 与多端点

- 在独立子进程向 Halo PixelBar 提交低幅度测试音。父进程对该子进程树激活 `VIRTUAL_AUDIO_DEVICE_PROCESS_LOOPBACK`，用 48 kHz 双声道 PCM 获取事件驱动采集。父进程向显式选择的 X7 HDMI 与 Realtek 数字输出开启两个独立的共享模式渲染流。源端点在参数检查中排除。
- 一次三秒窗口中采集 144,000 帧，左声道非零 140,640 帧，两个镜像端点各接受 144,000 帧。计数来自 WASAPI 写入成功。硬件可听性、物理同步、回放延迟和长时漂移需要独立测量。
- 采集使用 `PROCESS_LOOPBACK_MODE_INCLUDE_TARGET_PROCESS_TREE`。仍需专门的双源对照确认目标与其他应用信号的隔离，并验证 Windows 原生静音、应用音量和可保护音频的行为。 [Microsoft 激活接口](https://learn.microsoft.com/en-us/windows/win32/api/mmdeviceapi/nf-mmdeviceapi-activateaudiointerfaceasync)

## 未决问题

1. 复核任务栏行模型的 `SelectedOutput` 和 `PossibleOutputs` 能否读出实际值。若能，应优先探索通过原生行模型提供的设置动作实现单设备路由。若焦点触发不稳定，继续验证 `OnApplyTemplate` 与行实例化事件。
2. 将行模型 ID 对照 `GetSessionIdentifier`、`GetSessionInstanceIdentifier` 或 PID 与进程创建时间。若公开成员无法映射，调查同一原生模型提供的会话对象或动作接口。遇到歧义时应停用该行的路由操作。
3. 在本机及 24H2 上核对真实面板的重建、行回收、热插拔和卸载。随后验证 Windows 设置外部改动与模块持有策略之间的所有权冲突。
4. 实测主设备原生流与镜像的音量、静音、时延和时钟差。根据数据决定是否需要进程隔离及 IPC 分发方式。
