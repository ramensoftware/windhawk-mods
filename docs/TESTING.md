# 测试与验收

## 重跑 Phase 0 音频探针

```powershell
./scripts/build-probe.ps1
./out/audio-probe.exe
./out/audio-probe.exe --policy-write
# 先用枚举输出确认编号。以下 1 和 2 是本机样本。
./out/audio-probe.exe --mirror 1 2
```

`--policy-write` 暂时修改探针自身的多媒体角色并在退出前恢复。镜像模式约 5 秒，播出低幅度测试音。镜像参数接受 1 或 2 个互不相同的非主端点。主端点为启动时取得的多媒体默认端点，由子进程按该 ID 打开。每次测试确认退出码为 0。

符号测试先从本机 DLL 的 RSDS 记录取得该版本 PDB，把 PDB 放在同一符号目录。将目录路径传给探针。

```powershell
./out/audio-probe.exe --symbols $symbolDir "$env:windir/System32/ControlCenter.dll"
./out/audio-probe.exe --symbols $symbolDir "$env:windir/System32/ControlCenter.dll" '?OnGotFocus'
./out/audio-probe.exe --symbols $symbolDir "$env:windir/System32/AudioHandlers.dll" 'SessionInfoItem::'
```

## Windhawk 诊断

编译检查使用 Windhawk 源码签出目录以及从 SDK 生成的 C++/WinRT 头文件。

```powershell
./scripts/build-probe.ps1 -WindhawkSource $windhawkSource
```

在具备 Windhawk 的测试桌面中，先打开快捷设置以加载 `ControlCenter.dll`，再在 Windhawk 编辑器创建本地模块并编译 `research/native-audio-router-inspect.wh.cpp`。开启该模块的调试日志。播放测试音，打开快捷设置的声音输出页，再用 Tab 将键盘焦点移入应用滑块。检查是否出现 `VolumeMixerList focus`，至少一行 `AppVolumeLevel` 及对应的模型类型。诊断只报告字段是否出现，映射仍需继续核对字段值和真实会话。测试后禁用本地诊断模块。

## 产品阶段验收

- Phase 1 检查每个已实现音量行只增加一个按钮，原生音量和静音操作正常。覆盖英文、中文、浅色、深色、高 DPI、键盘和 Narrator，以及面板重建和卸载。
- Phase 2 检查两应用分别绑定、应用重启、默认设备切换、蓝牙断开和重连、角色策略读回、用户外部修改和卸载恢复。
- Phase 3 检查真实双端点与三端点播放、目标进程与另一应用的音频隔离、主设备没有镜像副本、设备动态移除以及音量和静音行为。
- Phase 4 检查不同采样率和蓝牙时延、8 小时播放、睡眠唤醒、锁屏解锁、频繁热插拔、ShellHost 和 Explorer 重启、重复启停以及常见模块共存。记录缓冲水位、欠载、设备延迟、CPU 和内存，并对照空闲 CPU 接近零及额外内存低于 20 MB 的目标。

## 任务栏预览复测

先让测试应用产生音频会话。完全关闭快捷设置，再从任务栏进入声音输出页。应用行末尾应直接显示小号下拉箭头。首次查询和设备变更后的查询在后台等待原生列表，等待期间不展开菜单。对同一个原生应用行反复开关菜单，确认马上显示缓存的列表，日志应出现 `Reused cached output list`。连接或断开输出设备后重新打开，确认旧缓存失效。切走焦点或关闭面板时，待打开的菜单应取消。

本机日志观察到普通应用最多获得 6 个原生选项，另有行仅返回默认项。用户在本机确认可打开设备菜单。缓存版的首次展开时延、设备热插拔和实际路由切换待复测。

确认只提供「默认」的主音频行没有箭头，也不占额外布局空间。在同一应用上快速切换几个输出，其中一次切回最初的输出，验证下一次展开时勾选最近提交的选项。等 Windows 更新后，再核对系统设置或实际播放设备。连接或断开设备时检查箭头也会随可选设备变化。

记录测试应用的原输出，选择不同的端点，再次打开菜单验证勾选状态。用实际播放或系统设置确认原生会话的输出，然后恢复原输出。区分“策略已读回”和“当前音频流已迁移”。若两个端点的原生标签相同，预览会停用该同名选项，避免把列表索引误用于另一个设备。
