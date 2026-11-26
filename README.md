# Native Audio Router for Windows 11

目标是在 Windows 11 快捷设置的原生应用音量混合器中，为每行增加一个输出设备选择入口。应用直接使用 Windows 逐应用路由播放主输出，其他选中端点接收该应用的 WASAPI 镜像。

当前交付处于 Phase 0。仓库提供可运行的音频探针，以及用于检查原生混合器行模型的 Windhawk 诊断源码。产品模块从 Phase 1 开始，首个具备单设备路由能力的版本计划为 v0.1.0。

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

Native Audio Router is a planned Windhawk extension for the Windows 11 Quick Settings volume mixer. The current repository contains a standalone Core Audio feasibility probe and a Windhawk mixer-model diagnostic. The probe exercises per-app policy readback and process-loopback fan-out to explicitly selected output endpoints. See [test procedures](docs/TESTING.md) for build and run commands.
