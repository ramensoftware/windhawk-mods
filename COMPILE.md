# 本地编译 Windhawk mod 说明

Windhawk 自带完整的编译器工具链，**不需要额外安装任何东西**。只要本机装了
Windhawk，就能在命令行直接编译 `.wh.cpp`，不必每次都复制进 Windhawk 编辑器点
"Compile"。

## 环境要求

- 已安装 Windhawk（任意 1.5+ 版本；本机为 1.7.3）。
- 安装目录（示例）：`E:\Program Files\Windhawk`
- 该目录下自带：
  - `Compiler\bin\clang++.exe` —— Clang 20（mingw-w64），以 **C++23** 编译
  - `Compiler\include\windhawk_api.h` —— mod API 头文件
  - `Engine\<版本>\<32|64|arm64>\windhawk.lib` —— 链接用的引擎库
  - `windhawk.ini` —— 内含 `EnginePath`，编译脚本据此定位 `windhawk.lib`

## 编译命令

仓库里的 `scripts/compile_mod.py` 会读取 `.wh.cpp` 头部的元数据（`@id`、`@version`、
`@compilerOptions`、`@architecture`），自动拼出正确的编译参数。

在 `Pull/` 目录下执行（路径按实际 Windhawk 安装位置修改）：

```bash
cd Pull
python scripts/compile_mod.py \
  -w "E:/Program Files/Windhawk" \
  -f mods/click-on-empty-explorer.wh.cpp \
  -o32 out/32/click-on-empty-explorer.dll \
  -o64 out/64/click-on-empty-explorer.dll \
  -oarm64 out/arm64/click-on-empty-explorer.dll
```

### 参数说明

| 参数 | 含义 |
|------|------|
| `-w, --windhawk-dir` | Windhawk 安装目录（必填），脚本从中找编译器/头文件/库 |
| `-f, --mod-files` | 要编译的 `.wh.cpp` 文件（可多个）；与 `-d` 互斥 |
| `-d, --mods-dir` | 或直接指定一个目录，递归编译目录下所有 `.wh.cpp` |
| `-o32 / -o64 / -oarm64` | 各架构输出 DLL 路径（必填） |

> 注意：实际编译哪些架构由 mod 头部的 `@architecture` 决定。本 mod 是 `x86-64`，
> 因此只编 **64 位 + arm64**（不会编 32 位），但三个 `-o*` 参数仍须都提供。

## 编译产物与验证

- 成功时终端输出 `Running compiler, target: ...` 且退出码为 `0`，并在对应 `-o*`
  路径生成 `.dll`。
- 编译出的 DLL 可直接在 Windhawk 中加载测试（编辑器里"Load from file"或丢进
  Windhawk 的 mods 目录）。
- 编译失败会打印 clang 报错并退出码非 0，不会生成 DLL。

## 备选：用 Windhawk 编辑器编译

打开 Windhawk 编辑器 → "Edit existing mod" / "Create new mod"，把 `.wh.cpp` 内容
粘贴进去 → 点 "Compile"。本质和上面命令行用的是同一套编译器，只是多了 GUI。

## 约定

- `@compilerOptions` 里的链接库（如 `-lcomctl32 -lole32`）会被脚本自动读取，
  无需手写。
- **改动代码后，先本地编译通过再提交**，避免把编不过的版本推上去。
