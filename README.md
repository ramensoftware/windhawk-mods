# Native Audio Router for Windows 11

目标是在 Windows 11 快捷设置的原生应用音量混合器中，为每行增加一个输出设备选择入口。应用直接使用 Windows 逐应用路由播放主输出，其他选中端点接收该应用的 WASAPI 镜像。

当前提供任务栏单设备路由预览 `0.0.5`，在原生应用音量行末尾增加输出设备菜单，使用 Windows 行模型中的 `PossibleOutputs` 和 `SelectedOutput`。多设备镜像仍在独立探针阶段。

## Windhawk 预览

在 Windhawk 本地模块编辑器中编译 [发布格式源码](dist/native-audio-router.wh.cpp)。先打开一次任务栏快捷设置以加载 `ControlCenter.dll`，启用后重新进入声音输出页。若该页在启用前已经打开，将焦点移到任意应用音量行。

```powershell
./scripts/build.ps1
./scripts/build.ps1 -WindhawkRoot $windhawkDirectory
```

第二条命令使用已有 Windhawk 安装的 Clang 另行生成本地 DLL。预览仅在布局结构与符号匹配时添加菜单。恢复目前依赖还可访问的行模型，应用退出或者 Shell 重启后的恢复仍待实现。

| 文档 | 内容 |
| --- | --- |
| [调研](docs/RESEARCH.md) | 参考源码、许可证、本机符号与实验结果 |
| [架构](docs/ARCHITECTURE.md) | UI、应用身份、策略恢复和镜像的边界 |
| [兼容性](docs/COMPATIBILITY.md) | 已检验的构建和实际验证范围 |
| [测试](docs/TESTING.md) | 编译、复现实验和各阶段验收 |

## 构建探针

在 PowerShell 中运行以下命令。需要 Visual Studio x64 C++ Build Tools 和包含 `audioclientactivationparams.h` 的 Windows SDK。

```powershell
./scripts/build-probe.ps1
./out/audio-probe.exe
```

默认运行枚举活动输出端点，并读取探针自身的逐应用策略。镜像实验和 XAML 诊断见测试文档。

## English

Native Audio Router is an experimental Windhawk extension for the Windows 11 Quick Settings volume mixer. The current preview adds a native per-app output menu. The separate Core Audio probe exercises policy readback and process-loopback fan-out to explicitly selected output endpoints. See [test procedures](docs/TESTING.md) for build and run commands.
