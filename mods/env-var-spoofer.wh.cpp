// ==WindhawkMod==
// @id              env-var-spoofer
// @name            Environment Variable Spoofer
// @name:zh-CN      环境变量伪造
// @name:ja-JP      環境変数の偽装
// @description     Spoofs environment variables such as TEMP and TMP for selected processes
// @description:zh-CN 为指定进程伪造 TEMP、TMP 等环境变量
// @description:ja-JP 指定したプロセスに対して、TEMP や TMP などの環境変数を偽装します
// @version         1.0
// @author          loliri
// @github          https://github.com/loliri
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Environment Variable Spoofer

Makes selected processes see fake values for environment variables such as `TEMP` and `TMP`. The system and other processes are not affected.

By default, the mod only intercepts the Win32 APIs that read environment variables, and the real environment block of the process is left untouched.

## Choosing the target program

The mod targets nothing by default, so it does nothing until you tell it which process to apply to. Open the mod in Windhawk, go to the **Advanced** tab, and put the executable name of your target in the **Custom process inclusion list**. The mod takes effect as soon as you save.

## Settings

| Setting                                       | Default                                  | Description                                                                                                                                                                                         |
| --------------------------------------------- | ---------------------------------------- | --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| Spoofed variables                             | `TEMP=C:\FakeTemp`, `TMP=C:\FakeTemp`    | The environment variables to spoof, one `NAME=VALUE` entry per item. Names are case-insensitive.                                                                                                    |
| Also write into the process environment block | off                                      | Also write the values into the process environment block, so that CRT `getenv`, `GetEnvironmentStrings` and child processes see them too. The original values are restored when the mod is unloaded. |

## Intercepted APIs

- `GetEnvironmentVariableW/A`
- `ExpandEnvironmentStringsW/A`, i.e. the expansion of `%TEMP%` and similar
- `GetTempPathW/A` and `GetTempPath2W/A`, which look at `TMP`, `TEMP` and `USERPROFILE` in that order

`GetTempPath` keeps the lookup order of the system: if the process has a real `TMP` and only `TEMP` is spoofed, the real `TMP` still wins. Spoof both.

## Notes

Changing the settings reloads the mod automatically.

With the environment block option turned off, programs that cache a copy of the environment at startup (CRT `getenv` / `_wgetenv`, Python `os.environ`) and child processes created by the target process still see the real values. Turn the option on to cover these cases.

With the option turned on, the mod must be injected before the target program caches its environment. A cache made earlier cannot be changed.

Programs that parse the environment block in the PEB directly, or call `ntdll!RtlQueryEnvironmentVariable*` themselves, are not affected by the API hooks. The environment block option covers them.

## Implementation notes

The mod hooks the exports in `kernelbase.dll`, falling back to `kernel32.dll`. `GetTempPath2W/A` do not exist on older systems and are skipped there.

`ExpandEnvironmentStrings` is only taken over when the source string contains at least one spoofed variable. Everything else goes to the original function, so without a spoofed variable in the string the behavior is unchanged.

The environment block option writes the values with `SetEnvironmentVariableW` before the hooks are installed, so the original values can be read back and restored when the mod is unloaded.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- variables: ['TEMP=C:\FakeTemp', 'TMP=C:\FakeTemp']
  $name: Spoofed variables
  $name:zh-CN: 伪造的环境变量
  $name:ja-JP: 偽装する環境変数
  $description: >-
    The environment variables to spoof, one NAME=VALUE entry per item, e.g.
    TEMP=C:\FakeTemp. Names are case-insensitive.
  $description:zh-CN: >-
    要伪造的环境变量，每项一条 NAME=VALUE，例如 TEMP=C:\FakeTemp。名称不区分大小写。
  $description:ja-JP: >-
    偽装する環境変数。1 項目につき NAME=VALUE を 1 つ指定します（例: TEMP=C:\FakeTemp）。名前は大文字と小文字を区別しません。
- writeToEnvBlock: false
  $name: Also write into the process environment block
  $name:zh-CN: 同时写入进程环境块
  $name:ja-JP: プロセスの環境ブロックにも書き込む
  $description: >-
    Also write the values into the process environment block, so that CRT
    getenv, GetEnvironmentStrings and child processes see them too. The
    original values are restored when the mod is unloaded.
  $description:zh-CN: >-
    同时把这些值写入进程环境块，使 CRT 的 getenv、GetEnvironmentStrings 以及子进程也能看到伪造值。卸载 Mod 时会还原原值。
  $description:ja-JP: >-
    値をプロセスの環境ブロックにも書き込み、CRT の getenv、GetEnvironmentStrings、子プロセスにも見えるようにします。Mod のアンロード時に元の値へ戻します。
*/
// ==/WindhawkModSettings==

#include <windows.h>

#include <string>
#include <vector>

// ---------------------------------------------------------------------------
// 配置
// ---------------------------------------------------------------------------

struct SpoofVar {
    std::wstring name;
    std::wstring value;
};

struct SavedVar {
    std::wstring name;
    bool existed;
    std::wstring value;
};

// 这些数据只在 Wh_ModInit 里写入，Hook 运行期间只读（设置变更时 mod 会整体重载），所以不需要加锁。
static std::vector<SpoofVar> g_vars;
static std::vector<SavedVar> g_saved;
static bool g_writeEnvBlock = false;

static void LoadSettings() {
    g_vars.clear();

    for (int i = 0;; i++) {
        PCWSTR s = Wh_GetStringSetting(L"variables[%d]", i);
        std::wstring item = s ? s : L"";
        Wh_FreeStringSetting(s);

        if (item.empty()) {
            break;
        }

        // 从下标 1 开始找 '='，名字至少要有一个字符
        size_t eq = item.find(L'=', 1);
        if (eq == std::wstring::npos) {
            Wh_Log(L"Ignoring invalid entry (expected NAME=VALUE): %s", item.c_str());
            continue;
        }

        SpoofVar v;
        v.name = item.substr(0, eq);
        v.value = item.substr(eq + 1);
        Wh_Log(L"Spoofing %s=%s", v.name.c_str(), v.value.c_str());
        g_vars.push_back(std::move(v));
    }

    g_writeEnvBlock = Wh_GetIntSetting(L"writeToEnvBlock") != 0;
}

// ---------------------------------------------------------------------------
// 工具函数
// ---------------------------------------------------------------------------

// 名称不区分大小写，命中返回对应条目
static const SpoofVar* FindVar(PCWSTR name) {
    if (!name || !*name) {
        return nullptr;
    }
    for (const auto& v : g_vars) {
        if (CompareStringOrdinal(name, -1, v.name.c_str(), (int)v.name.size(), TRUE) == CSTR_EQUAL) {
            return &v;
        }
    }
    return nullptr;
}

static std::wstring AnsiToWide(PCSTR s) {
    if (!s) {
        return L"";
    }
    int n = MultiByteToWideChar(CP_ACP, 0, s, -1, nullptr, 0);
    if (n <= 1) {
        return L"";
    }
    std::wstring r(n, L'\0');
    MultiByteToWideChar(CP_ACP, 0, s, -1, &r[0], n);
    r.resize(n - 1);  // 去掉结尾的 NUL
    return r;
}

static std::string WideToAnsi(const std::wstring& w) {
    if (w.empty()) {
        return "";
    }
    int n = WideCharToMultiByte(CP_ACP, 0, w.c_str(), (int)w.size(), nullptr, 0, nullptr, nullptr);
    if (n <= 0) {
        return "";
    }
    std::string r(n, '\0');
    WideCharToMultiByte(CP_ACP, 0, w.c_str(), (int)w.size(), &r[0], n, nullptr, nullptr);
    return r;
}

// GetEnvironmentVariable / GetTempPath 的返回约定：
//   缓冲区够大 -> 返回字符数（不含结尾 NUL）
//   缓冲区不够 -> 返回所需大小（含结尾 NUL），缓冲区内容不定
static DWORD CopyOutW(const std::wstring& s, LPWSTR buf, DWORD size) {
    DWORD need = (DWORD)s.size() + 1;
    if (!buf || size < need) {
        return need;
    }
    memcpy(buf, s.c_str(), need * sizeof(WCHAR));
    return need - 1;
}

static DWORD CopyOutA(const std::string& s, LPSTR buf, DWORD size) {
    DWORD need = (DWORD)s.size() + 1;
    if (!buf || size < need) {
        return need;
    }
    memcpy(buf, s.c_str(), need);
    return need - 1;
}

// ExpandEnvironmentStrings 的返回约定：始终返回所需大小（含结尾 NUL）
static DWORD ExpandCopyW(const std::wstring& s, LPWSTR dst, DWORD size) {
    DWORD need = (DWORD)s.size() + 1;
    if (dst && size >= need) {
        memcpy(dst, s.c_str(), need * sizeof(WCHAR));
    }
    return need;
}

static DWORD ExpandCopyA(const std::string& s, LPSTR dst, DWORD size) {
    DWORD need = (DWORD)s.size() + 1;
    if (dst && size >= need) {
        memcpy(dst, s.c_str(), need);
    }
    return need;
}

// 展开 %NAME%。只有当源字符串里至少有一个被伪造的变量时才返回 true，
// 否则调用方应该直接走原函数，保证没有伪造变量时行为完全不变。
static bool ExpandWithSpoof(PCWSTR src, std::wstring& out) {
    out.clear();
    bool hit = false;

    for (const WCHAR* p = src; *p;) {
        if (*p == L'%') {
            const WCHAR* end = wcschr(p + 1, L'%');
            if (end && end > p + 1) {
                std::wstring name(p + 1, end);

                if (const SpoofVar* v = FindVar(name.c_str())) {
                    out += v->value;
                    hit = true;
                    p = end + 1;
                    continue;
                }

                // 非伪造变量：查真实值（会经过我们的 Hook，但不会命中，最终走原函数）
                DWORD n = GetEnvironmentVariableW(name.c_str(), nullptr, 0);
                if (n > 0) {
                    std::wstring val(n, L'\0');
                    DWORD got = GetEnvironmentVariableW(name.c_str(), &val[0], n);
                    val.resize(got < n ? got : n - 1);
                    out += val;
                } else {
                    // 变量不存在：与系统行为一致，原样保留 %name%
                    out.append(p, end + 1);
                }
                p = end + 1;
                continue;
            }
        }
        out.push_back(*p++);
    }
    return hit;
}

// 复刻 GetTempPath 的查找顺序 TMP -> TEMP -> USERPROFILE。
// 只有"第一个生效的变量恰好是被伪造的"才接管，否则交回原函数。
static bool GetSpoofedTempPath(std::wstring& out) {
    static const PCWSTR order[] = {L"TMP", L"TEMP", L"USERPROFILE"};
    for (PCWSTR name : order) {
        if (const SpoofVar* v = FindVar(name)) {
            if (v->value.empty()) {
                continue;
            }
            out = v->value;
            if (out.back() != L'\\') {
                out += L'\\';  // GetTempPath 的结果总是以反斜杠结尾
            }
            return true;
        }
        if (GetEnvironmentVariableW(name, nullptr, 0) > 0) {
            return false;  // 真实变量优先级更高
        }
    }
    return false;
}

// ---------------------------------------------------------------------------
// Hook：GetEnvironmentVariable
// ---------------------------------------------------------------------------

using GetEnvironmentVariableW_t = DWORD(WINAPI*)(LPCWSTR, LPWSTR, DWORD);
static GetEnvironmentVariableW_t GetEnvironmentVariableW_Orig;

static DWORD WINAPI GetEnvironmentVariableW_Hook(LPCWSTR name, LPWSTR buf, DWORD size) {
    if (const SpoofVar* v = FindVar(name)) {
        SetLastError(ERROR_SUCCESS);
        return CopyOutW(v->value, buf, size);
    }
    return GetEnvironmentVariableW_Orig(name, buf, size);
}

using GetEnvironmentVariableA_t = DWORD(WINAPI*)(LPCSTR, LPSTR, DWORD);
static GetEnvironmentVariableA_t GetEnvironmentVariableA_Orig;

static DWORD WINAPI GetEnvironmentVariableA_Hook(LPCSTR name, LPSTR buf, DWORD size) {
    if (name) {
        std::wstring wname = AnsiToWide(name);
        if (const SpoofVar* v = FindVar(wname.c_str())) {
            SetLastError(ERROR_SUCCESS);
            return CopyOutA(WideToAnsi(v->value), buf, size);
        }
    }
    return GetEnvironmentVariableA_Orig(name, buf, size);
}

// ---------------------------------------------------------------------------
// Hook：ExpandEnvironmentStrings
// ---------------------------------------------------------------------------

using ExpandEnvironmentStringsW_t = DWORD(WINAPI*)(LPCWSTR, LPWSTR, DWORD);
static ExpandEnvironmentStringsW_t ExpandEnvironmentStringsW_Orig;

static DWORD WINAPI ExpandEnvironmentStringsW_Hook(LPCWSTR src, LPWSTR dst, DWORD size) {
    std::wstring out;
    if (src && ExpandWithSpoof(src, out)) {
        return ExpandCopyW(out, dst, size);
    }
    return ExpandEnvironmentStringsW_Orig(src, dst, size);
}

using ExpandEnvironmentStringsA_t = DWORD(WINAPI*)(LPCSTR, LPSTR, DWORD);
static ExpandEnvironmentStringsA_t ExpandEnvironmentStringsA_Orig;

static DWORD WINAPI ExpandEnvironmentStringsA_Hook(LPCSTR src, LPSTR dst, DWORD size) {
    if (src) {
        std::wstring out;
        if (ExpandWithSpoof(AnsiToWide(src).c_str(), out)) {
            return ExpandCopyA(WideToAnsi(out), dst, size);
        }
    }
    return ExpandEnvironmentStringsA_Orig(src, dst, size);
}

// ---------------------------------------------------------------------------
// Hook：GetTempPath / GetTempPath2
// ---------------------------------------------------------------------------

using GetTempPathW_t = DWORD(WINAPI*)(DWORD, LPWSTR);
using GetTempPathA_t = DWORD(WINAPI*)(DWORD, LPSTR);

static GetTempPathW_t GetTempPathW_Orig;
static GetTempPathA_t GetTempPathA_Orig;
static GetTempPathW_t GetTempPath2W_Orig;  // Win10 21H2+ / Win11
static GetTempPathA_t GetTempPath2A_Orig;

static DWORD TempPathW_Common(DWORD size, LPWSTR buf, GetTempPathW_t orig) {
    std::wstring path;
    if (GetSpoofedTempPath(path)) {
        return CopyOutW(path, buf, size);
    }
    return orig(size, buf);
}

static DWORD TempPathA_Common(DWORD size, LPSTR buf, GetTempPathA_t orig) {
    std::wstring path;
    if (GetSpoofedTempPath(path)) {
        return CopyOutA(WideToAnsi(path), buf, size);
    }
    return orig(size, buf);
}

static DWORD WINAPI GetTempPathW_Hook(DWORD size, LPWSTR buf) {
    return TempPathW_Common(size, buf, GetTempPathW_Orig);
}
static DWORD WINAPI GetTempPathA_Hook(DWORD size, LPSTR buf) {
    return TempPathA_Common(size, buf, GetTempPathA_Orig);
}
static DWORD WINAPI GetTempPath2W_Hook(DWORD size, LPWSTR buf) {
    return TempPathW_Common(size, buf, GetTempPath2W_Orig);
}
static DWORD WINAPI GetTempPath2A_Hook(DWORD size, LPSTR buf) {
    return TempPathA_Common(size, buf, GetTempPath2A_Orig);
}

// ---------------------------------------------------------------------------
// 可选：直接改写进程环境块
// ---------------------------------------------------------------------------

static void ApplyEnvBlock() {
    for (const auto& v : g_vars) {
        SavedVar sv;
        sv.name = v.name;

        DWORD n = GetEnvironmentVariableW(v.name.c_str(), nullptr, 0);
        sv.existed = n > 0;
        if (sv.existed) {
            sv.value.resize(n);
            DWORD got = GetEnvironmentVariableW(v.name.c_str(), &sv.value[0], n);
            sv.value.resize(got < n ? got : n - 1);
        }
        g_saved.push_back(std::move(sv));

        SetEnvironmentVariableW(v.name.c_str(), v.value.c_str());
    }
}

static void RestoreEnvBlock() {
    // 倒序还原，这样同名变量配置了多次时最终回到最初的值
    for (auto it = g_saved.rbegin(); it != g_saved.rend(); ++it) {
        SetEnvironmentVariableW(it->name.c_str(), it->existed ? it->value.c_str() : nullptr);
    }
    g_saved.clear();
}

// ---------------------------------------------------------------------------
// 安装 Hook
// ---------------------------------------------------------------------------

// 优先在 kernelbase.dll 里找（Win7+ 实际实现所在），找不到再退回 kernel32.dll
static void* FindKernelExport(const char* name) {
    void* p = nullptr;
    if (HMODULE m = GetModuleHandleW(L"kernelbase.dll")) {
        p = (void*)GetProcAddress(m, name);
    }
    if (!p) {
        if (HMODULE m = GetModuleHandleW(L"kernel32.dll")) {
            p = (void*)GetProcAddress(m, name);
        }
    }
    return p;
}

template <typename F>
static bool HookKernel(const char* name, F hook, F* orig) {
    void* target = FindKernelExport(name);
    if (!target) {
        Wh_Log(L"Export not found (skipped): %s", AnsiToWide(name).c_str());
        return false;
    }
    if (!Wh_SetFunctionHook(target, (void*)hook, (void**)orig)) {
        Wh_Log(L"Failed to hook: %s", AnsiToWide(name).c_str());
        return false;
    }
    return true;
}

BOOL Wh_ModInit() {
    Wh_Log(L"Init");

    LoadSettings();

    // 必须在安装 Hook 之前做，这样保存下来的是真实的原始值
    if (g_writeEnvBlock) {
        ApplyEnvBlock();
    }

    HookKernel("GetEnvironmentVariableW", GetEnvironmentVariableW_Hook, &GetEnvironmentVariableW_Orig);
    HookKernel("GetEnvironmentVariableA", GetEnvironmentVariableA_Hook, &GetEnvironmentVariableA_Orig);
    HookKernel("ExpandEnvironmentStringsW", ExpandEnvironmentStringsW_Hook, &ExpandEnvironmentStringsW_Orig);
    HookKernel("ExpandEnvironmentStringsA", ExpandEnvironmentStringsA_Hook, &ExpandEnvironmentStringsA_Orig);
    HookKernel("GetTempPathW", GetTempPathW_Hook, &GetTempPathW_Orig);
    HookKernel("GetTempPathA", GetTempPathA_Hook, &GetTempPathA_Orig);
    HookKernel("GetTempPath2W", GetTempPath2W_Hook, &GetTempPath2W_Orig);  // 旧系统上不存在，会被跳过
    HookKernel("GetTempPath2A", GetTempPath2A_Hook, &GetTempPath2A_Orig);

    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L"Uninit");
    RestoreEnvBlock();
}

// 设置变更时让 Windhawk 直接重载 mod，省去运行时同步配置的麻烦
BOOL Wh_ModSettingsChanged(BOOL* bReload) {
    *bReload = TRUE;
    return TRUE;
}
