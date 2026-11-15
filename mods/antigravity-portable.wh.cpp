// ==WindhawkMod==
// @id              antigravity-portable
// @name            Antigravity IDE Portable
// @description     Make Antigravity IDE portable by redirecting user data and extensions directories.
// @description:zh-CN 使 Antigravity IDE 成为便携版，重定向数据与扩展目录。
// @version         1.0.260614
// @author          easyatm
// @github          https://github.com/easyatm
// @include         Antigravity IDE.exe
// @compilerOptions -lshell32
// ==/WindhawkMod==

// ==WindhawkModSettings==
/*
- data_dir: "../data/"
  $name: Data directory
  $name:zh-CN: 数据目录
  $description: The directory where user data and extensions will be stored, which can be an absolute path or a relative path from the exe directory.
  $description:zh-CN: 存储用户 data 和 extensions 的目录，可以是绝对路径，或者是相对于 exe 所在目录的相对路径。
*/
// ==/WindhawkModSettings==

// ==WindhawkModReadme==
/*
# Antigravity IDE Portable

Make Antigravity IDE portable by redirecting user data and extensions directories via hooking `GetCommandLineW`.

## Features
- Uses `../data/` relative to the exe directory as the default data directory.
- Automatically converts relative paths to absolute paths to ensure the program recognizes them correctly.
- Keeps original parameters unchanged if `--user-data-dir` or `--extensions-dir` is already present.
- Detects and skips child processes (those with `--type=` or `--node-ipc` in their command line).
- Supports configuring the data directory in Windhawk settings.

---

# Antigravity IDE 便携版

这个 Mod 为 Antigravity IDE 制作便携版。通过 Hook `GetCommandLineW` 在启动参数中注入 `--user-data-dir` 和 `--extensions-dir`。

## 功能特性：
- 默认使用相对于 exe 所在目录的 `../data/` 作为数据目录。
- 自动将相对路径转换为绝对路径，确保程序能够正确识别。
- 如果原启动参数中已经包含了 `--user-data-dir` 或 `--extensions-dir`，则不会修改对应的参数。
- 自动检测并跳过子进程（即启动参数中含有 `--type=` 或 `--node-ipc` 的进程）。
- 支持在 Windhawk 设置中自定义数据目录。
*/
// ==/WindhawkModReadme==

#include <windows.h>
#include <shellapi.h>
#include <string>
#include <vector>
#include <windhawk_api.h>

// 全局变量保存修改后的命令行
std::vector<wchar_t> g_newCommandLine;

// 缓存配置的数据目录
std::wstring g_settingsDataDir;

void LoadSettings() {
    PCWSTR dataDirSetting = Wh_GetStringSetting(L"data_dir");
    if (dataDirSetting) {
        g_settingsDataDir = dataDirSetting;
        Wh_FreeStringSetting(dataDirSetting);
    } else {
        g_settingsDataDir = L"../data/";
    }
    Wh_Log(L"Antigravity 便携版 - 加载设置: 数据目录 = %s", g_settingsDataDir.c_str());
}

// 辅助函数：将相对路径或绝对路径解析为规范的绝对路径
std::wstring GetAbsoluteFullPath(const std::wstring& relativeOrAbsolutePath) {
    // 判断是否已经是绝对路径（例如以 C:\ 或 \\ 开头）
    bool isAbsolute = false;
    if (relativeOrAbsolutePath.size() >= 2 && iswalpha(relativeOrAbsolutePath[0]) && relativeOrAbsolutePath[1] == L':') {
        isAbsolute = true;
    } else if (relativeOrAbsolutePath.size() >= 2 && relativeOrAbsolutePath[0] == L'\\' && relativeOrAbsolutePath[1] == L'\\') {
        isAbsolute = true;
    }

    std::wstring combined;
    if (isAbsolute) {
        combined = relativeOrAbsolutePath;
    } else {
        wchar_t exePath[MAX_PATH];
        DWORD len = GetModuleFileNameW(NULL, exePath, MAX_PATH);
        if (len == 0 || len >= MAX_PATH) {
            Wh_Log(L"Antigravity 便携版 - 转换绝对路径失败: 无法获取模块文件名或路径过长");
            return L"";
        }

        // 获取 exe 所在的目录
        std::wstring pathStr(exePath);
        size_t pos = pathStr.find_last_of(L"\\/");
        if (pos == std::wstring::npos) {
            Wh_Log(L"Antigravity 便携版 - 转换绝对路径失败: exe 路径无效 %s", exePath);
            return L"";
        }
        std::wstring dir = pathStr.substr(0, pos);

        // 拼接相对路径
        combined = dir + L"\\" + relativeOrAbsolutePath;
    }

    // 规范化为绝对路径
    wchar_t fullPath[MAX_PATH * 2];
    DWORD fullLen = GetFullPathNameW(combined.c_str(), ARRAYSIZE(fullPath), fullPath, NULL);
    if (fullLen == 0 || fullLen >= ARRAYSIZE(fullPath)) {
        Wh_Log(L"Antigravity 便携版 - 转换绝对路径失败: 规范化 %s 失败", combined.c_str());
        return L"";
    }

    Wh_Log(L"Antigravity 便携版 - 路径转换成功: 将 %s 解析为绝对路径 %s", relativeOrAbsolutePath.c_str(), fullPath);
    return std::wstring(fullPath);
}

using GetCommandLineW_t = LPWSTR (WINAPI *)(VOID);
GetCommandLineW_t GetCommandLineW_Original;

LPWSTR WINAPI GetCommandLineW_Hook(VOID) {
    Wh_Log(L"Antigravity 便携版 - 触发命令行 Hook, 返回修改后的命令行: %s", g_newCommandLine.data());
    return g_newCommandLine.data();
}

BOOL Wh_ModInit() {
    Wh_Log(L"Antigravity 便携版 - 模块初始化中");

    LoadSettings();

    // 1. 获取系统原始命令行
    LPWSTR originalCmd = GetCommandLineW();
    if (!originalCmd) {
        Wh_Log(L"Antigravity 便携版 - 获取原始命令行失败");
        return TRUE;
    }
    Wh_Log(L"Antigravity 便携版 - 进程原始命令行: %s", originalCmd);

    // 2. 解析参数
    int argc = 0;
    LPWSTR* argv = CommandLineToArgvW(originalCmd, &argc);
    if (!argv) {
        Wh_Log(L"Antigravity 便携版 - 解析命令行参数失败");
        return TRUE;
    }

    bool isSubProcess = false;
    std::wstring subProcessReason;
    bool hasUserDataDir = false;
    bool hasExtensionsDir = false;

    for (int i = 0; i < argc; ++i) {
        std::wstring arg(argv[i]);
        
        // 如果是子进程则不修改
        if (arg.find(L"--type=") != std::wstring::npos) {
            isSubProcess = true;
            subProcessReason = L"包含 --type=";
        } else if (arg.find(L"--node-ipc") != std::wstring::npos) {
            isSubProcess = true;
            subProcessReason = L"包含 --node-ipc";
        }
        if (arg.find(L"--user-data-dir") != std::wstring::npos) {
            hasUserDataDir = true;
        }
        if (arg.find(L"--extensions-dir") != std::wstring::npos) {
            hasExtensionsDir = true;
        }
    }
    LocalFree(argv);

    // 3. 如果是子进程，或者已经包含了全部两个目标参数，就不需要挂载 Hook
    if (isSubProcess) {
        Wh_Log(L"Antigravity 便携版 - 无需修改命令行: 检测到子进程 (%s)", subProcessReason.c_str());
        return TRUE;
    }
    if (hasUserDataDir || hasExtensionsDir) {
        Wh_Log(L"Antigravity 便携版 - 无需修改命令行: --user-data-dir 和 --extensions-dir 已同时存在");
        return TRUE;
    }

    // 4. 构造新的命令行参数
    std::wstring newCmd(originalCmd);
    std::wstring baseDataDir = GetAbsoluteFullPath(g_settingsDataDir);
    if (!baseDataDir.empty()) {
        // 确保尾部没有反斜杠
        if (baseDataDir.back() == L'\\' || baseDataDir.back() == L'/') {
            baseDataDir.pop_back();
        }

        std::wstring userDataPath = baseDataDir + L"\\user-data";
        std::wstring extensionsPath = baseDataDir + L"\\extensions";

        if (!hasUserDataDir) {
            newCmd += L" --user-data-dir=\"" + userDataPath + L"\"";
            Wh_Log(L"Antigravity 便携版 - 正在注入参数: --user-data-dir=\"%s\"", userDataPath.c_str());
        } else {
            Wh_Log(L"Antigravity 便携版 - 跳过注入 --user-data-dir (原始参数已包含)");
        }
        if (!hasExtensionsDir) {
            newCmd += L" --extensions-dir=\"" + extensionsPath + L"\"";
            Wh_Log(L"Antigravity 便携版 - 正在注入参数: --extensions-dir=\"%s\"", extensionsPath.c_str());
        } else {
            Wh_Log(L"Antigravity 便携版 - 跳过注入 --extensions-dir (原始参数已包含)");
        }
    } else {
        Wh_Log(L"Antigravity 便携版 - 无法解析出绝对数据目录");
        return TRUE;
    }

    // 5. 保存到全局 buffer 中并加上空字符
    g_newCommandLine.assign(newCmd.begin(), newCmd.end());
    g_newCommandLine.push_back(L'\0');

    Wh_Log(L"Antigravity 便携版 - 最终生成的新命令行: %s", g_newCommandLine.data());

    // 6. 挂载 Hook
    Wh_SetFunctionHook(
        (void*)GetCommandLineW,
        (void*)GetCommandLineW_Hook,
        (void**)&GetCommandLineW_Original
    );
    Wh_Log(L"Antigravity 便携版 - 成功挂钩 GetCommandLineW");

    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L"Antigravity 便携版 - 模块卸载");
}

void Wh_ModSettingsChanged() {
    Wh_Log(L"Antigravity 便携版 - 设置已更改，重新加载");
    LoadSettings();
}
