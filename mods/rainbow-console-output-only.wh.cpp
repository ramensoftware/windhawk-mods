// ==WindhawkMod==
// @id              rainbow-console-output-only
// @name            Rainbow Console
// @description     Continuous true-color rainbow output for terminals
// @version         1.0
// @author          nopp
// @github          https://github.com/848488489-ff
// @include         *
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Rainbow Console
This mod injects true-color (24-bit) rainbow colors into your terminal output.

### Features:
- **Continuous Rainbow**: Text is colored character by character using HSV color
space.
- **Safe Override**: Replaces original ANSI colors with the rainbow spectrum.

### ⚠️ IMPORTANT WARNING
**DO NOT use this mod with `Always On` enabled when running Terminal AI Agents
(e.g., Claude Code, Aider, Cursor CLI, Gemini CLI).** The rainbow injection adds
massive amounts of invisible ANSI escape sequences to every single character.
This will severely pollute the AI's context window (wasting tokens) and may
break terminal buffer calculations, leading to hallucinations or logic errors.
*/
// ==/WindhawkModReadme==

// clang-format off
// ==WindhawkModSettings==
/*
- injectAll: false
  $name: Inject into All Processes
  $description: "[NOT RECOMMENDED] Injects into every process, which may cause legacy or GUI apps to crash. Default is false."
- targetRegex: "cmd|powershell|pwsh|WindowsTerminal|OpenConsole|conhost|git|bash|mintty|py|node"
  $name: Target Processes Regex
  $description: "Simple wildcards only (e.g., 'cmd|git*|python'). No complex regex like [] or +. Do NOT include .exe."
- alwaysOn: false
  $name: Always On
  $description: "Enable rainbow immediately. [WARNING: DO NOT use with AI Agents.]"
*/
// ==/WindhawkModSettings==
// clang-format on

#include <windows.h>

#define MAX_RC_BUF 65536

static BOOL(WINAPI* pOrigWriteConsoleW)(HANDLE,
                                        const VOID*,
                                        DWORD,
                                        LPDWORD,
                                        LPVOID) = NULL;
static BOOL(WINAPI* pOrigWriteFile)(HANDLE, LPCVOID, DWORD, LPDWORD, void*) =
    NULL;

static CRITICAL_SECTION rc_cs;
static int rc_colorIndex = 0;
static BOOL rc_active = FALSE;

static BOOL rc_WildcardMatch(const WCHAR* str, const WCHAR* pattern) {
    if (*pattern == L'\0')
        return *str == L'\0';
    if (*pattern == L'*') {
        while (*(pattern + 1) == L'*')
            pattern++;
        if (rc_WildcardMatch(str, pattern + 1))
            return TRUE;
        if (*str == L'\0')
            return FALSE;
        return rc_WildcardMatch(str + 1, pattern);
    }
    if (*pattern == L'?') {
        if (*str == L'\0')
            return FALSE;
        return rc_WildcardMatch(str + 1, pattern + 1);
    }
    if (*str != L'\0' && towlower(*str) == towlower(*pattern)) {
        return rc_WildcardMatch(str + 1, pattern + 1);
    }
    return FALSE;
}

static BOOL rc_IsTargetProcess() {
    BOOL injectAll = Wh_GetIntSetting(L"injectAll") != 0;
    if (injectAll) {
        Wh_Log(
            L"[RainbowConsole] Global Injection enabled. Targeting ALL "
            L"processes.");
        return TRUE;
    }

    PCWSTR regexStr = Wh_GetStringSetting(L"targetRegex");
    if (!regexStr || wcslen(regexStr) == 0) {
        Wh_Log(
            L"[RainbowConsole] Error: Target Regex is empty and Global "
            L"Injection is false. Mod will not load.");
        if (regexStr)
            Wh_FreeStringSetting(regexStr);
        return FALSE;
    }

    BOOL hasInvalidFormat = FALSE;
    for (int i = 0; regexStr[i] != L'\0'; i++) {
        WCHAR c = regexStr[i];
        if (c == L'[' || c == L']' || c == L'+' || c == L'{' || c == L'}' ||
            c == L'^' || c == L'$' || c == L'\\') {
            Wh_Log(
                L"[RainbowConsole] Warning: Unsupported regex character '%c' "
                L"detected. Only '*' and '?' wildcards are supported.",
                c);
            hasInvalidFormat = TRUE;
        }
    }
    if (hasInvalidFormat) {
        Wh_Log(
            L"[RainbowConsole] Format error! Please simplify your regex. "
            L"Example: 'cmd|git*|python'");
    }

    WCHAR procPath[MAX_PATH];
    GetModuleFileNameW(NULL, procPath, MAX_PATH);
    WCHAR* procName = wcsrchr(procPath, L'\\');
    if (procName)
        procName++;
    else
        procName = procPath;

    WCHAR* dot = wcsrchr(procName, L'.');
    if (dot && _wcsicmp(dot, L".exe") == 0)
        *dot = L'\0';

    WCHAR targets[MAX_PATH * 4];
    wcsncpy_s(targets, MAX_PATH * 4, regexStr, MAX_PATH * 4 - 1);
    Wh_FreeStringSetting(regexStr);

    BOOL found = FALSE;
    WCHAR* nextToken = NULL;
    WCHAR* token = wcstok_s(targets, L"|", &nextToken);
    while (token) {
        WCHAR* tDot = wcsrchr(token, L'.');
        if (tDot && _wcsicmp(tDot, L".exe") == 0)
            *tDot = L'\0';

        if (rc_WildcardMatch(procName, token)) {
            found = TRUE;
            Wh_Log(
                L"[RainbowConsole] Success: Process '%s' matched pattern '%s'. "
                L"Mod loaded.",
                procName, token);
            break;
        }
        token = wcstok_s(NULL, L"|", &nextToken);
    }

    if (!found) {
        Wh_Log(L"[RainbowConsole] Process '%s' did not match any Target Regex.",
               procName);
    }
    return found;
}

static int rc_UIntToWStr(unsigned int val, WCHAR* buf) {
    if (val == 0) {
        buf[0] = L'0';
        return 1;
    }
    WCHAR temp[10];
    int i = 0;
    while (val > 0) {
        temp[i++] = (WCHAR)(L'0' + (val % 10));
        val /= 10;
    }
    int len = i;
    for (int j = 0; j < len; j++)
        buf[j] = temp[len - 1 - j];
    return len;
}

static float rc_fmod(float a, float b) {
    return a - (int)(a / b) * b;
}
static float rc_fabs(float x) {
    return x < 0 ? -x : x;
}

static void rc_HSVtoRGB(float H, float S, float V, int* r, int* g, int* b) {
    float s = S / 100.0f;
    float v = V / 100.0f;
    float C = s * v;
    float X = C * (1 - rc_fabs(rc_fmod(H / 60.0f, 2.0f) - 1));
    float m = v - C;
    float r1 = 0, g1 = 0, b1 = 0;
    if (H >= 0 && H < 60) {
        r1 = C;
        g1 = X;
        b1 = 0;
    } else if (H >= 60 && H < 120) {
        r1 = X;
        g1 = C;
        b1 = 0;
    } else if (H >= 120 && H < 180) {
        r1 = 0;
        g1 = C;
        b1 = X;
    } else if (H >= 180 && H < 240) {
        r1 = 0;
        g1 = X;
        b1 = C;
    } else if (H >= 240 && H < 300) {
        r1 = X;
        g1 = 0;
        b1 = C;
    } else if (H >= 300 && H < 360) {
        r1 = C;
        g1 = 0;
        b1 = X;
    }
    *r = (int)((r1 + m) * 255);
    *g = (int)((g1 + m) * 255);
    *b = (int)((b1 + m) * 255);
}

static void rc_MakeRainbow(const WCHAR* src, WCHAR* dst, int dstMax) {
    int d = 0;
    for (int i = 0; src[i] != L'\0' && d < dstMax - 32; ++i) {
        WCHAR ch = src[i];
        if (ch == L'\x1b') {
            if (src[i + 1] == L'\0') {
                dst[d++] = ch;
                continue;
            }
            if (src[i + 1] == L'[') {
                int j = i + 2;
                while (src[j] >= L'0' && src[j] <= L'?')
                    j++;
                if (src[j] == L'm') {
                    i = j;
                    continue;
                } else {
                    dst[d++] = src[i];
                    dst[d++] = src[i + 1];
                    for (int k = i + 2; k <= j; k++)
                        dst[d++] = src[k];
                    i = j;
                    continue;
                }
            } else if (src[i + 1] == L']' || src[i + 1] == L'P' ||
                       src[i + 1] == L'X' || src[i + 1] == L'^' ||
                       src[i + 1] == L'_') {
                dst[d++] = src[i];
                dst[d++] = src[i + 1];
                int j = i + 2;
                while (src[j] != L'\0' && d < dstMax - 32) {
                    if (src[j] == L'\x07') {
                        dst[d++] = src[j];
                        break;
                    }
                    if (src[j] == L'\x1b' && src[j + 1] == L'\\') {
                        dst[d++] = src[j];
                        dst[d++] = src[j + 1];
                        j++;
                        break;
                    }
                    dst[d++] = src[j];
                    j++;
                }
                i = j;
                if (src[j] == L'\0')
                    break;
                continue;
            }
        }
        if (ch == L'\r') {
            dst[d++] = ch;
            rc_colorIndex = 0;
            continue;
        }
        if (ch < L' ' || ch == 0x7f) {
            dst[d++] = ch;
            continue;
        }

        int r, g, b;
        float hue = (float)((rc_colorIndex * 15) % 360);
        rc_HSVtoRGB(hue, 100.0f, 100.0f, &r, &g, &b);
        dst[d++] = L'\x1b';
        dst[d++] = L'[';
        dst[d++] = L'3';
        dst[d++] = L'8';
        dst[d++] = L';';
        dst[d++] = L'2';
        dst[d++] = L';';
        d += rc_UIntToWStr((unsigned int)r, &dst[d]);
        dst[d++] = L';';
        d += rc_UIntToWStr((unsigned int)g, &dst[d]);
        dst[d++] = L';';
        d += rc_UIntToWStr((unsigned int)b, &dst[d]);
        dst[d++] = L'm';
        dst[d++] = ch;
        rc_colorIndex++;
    }
    if (d < dstMax - 5)
        wcscpy_s(&dst[d], dstMax - d, L"\x1b[0m");
}

static BOOL WINAPI
rc_HookWriteConsoleW(HANDLE h, const VOID* b, DWORD n, LPDWORD w, LPVOID r) {
    if (!rc_active || n == 0 || n > (DWORD)INT_MAX)
        return pOrigWriteConsoleW(h, b, n, w, r);

    EnterCriticalSection(&rc_cs);

    // 堆分配，避免 64K 栈数组溢出；每个 WCHAR 最多膨胀约 24 个 WCHAR
    size_t need = (size_t)n * 24 + 64;
    if (need > (size_t)INT_MAX)
        need = (size_t)INT_MAX;
    WCHAR* src = (WCHAR*)malloc(need * sizeof(WCHAR));
    WCHAR* out = (WCHAR*)malloc(need * sizeof(WCHAR));
    if (!src || !out) {
        free(src);
        free(out);
        LeaveCriticalSection(&rc_cs);
        return pOrigWriteConsoleW(h, b, n, w, r);
    }

    // 复制并始终保留终止符（修复 8192 字符把结束符挤掉的问题）
    wcsncpy_s(src, need, (const WCHAR*)b, n);

    rc_MakeRainbow(src, out, (int)need);
    DWORD outLen = (DWORD)wcslen(out);
    BOOL ret = pOrigWriteConsoleW(h, out, outLen, w, r);
    if (w && ret)
        *w = n;

    free(src);
    free(out);
    LeaveCriticalSection(&rc_cs);
    return ret;
}

static BOOL WINAPI
rc_HookWriteFile(HANDLE h, LPCVOID b, DWORD n, LPDWORD w, void* o) {
    // 重叠 I/O：WriteFile 异步返回后系统仍会读取缓冲区，提前释放会导致驱动
    // 读到脏数据（蓝屏/崩溃）。这类调用不 Hook，交给原函数处理。
    if (o != NULL)
        return pOrigWriteFile(h, b, n, w, o);
    if (!rc_active || n == 0 || n > (DWORD)INT_MAX)
        return pOrigWriteFile(h, b, n, w, o);
    if (GetFileType(h) != FILE_TYPE_CHAR)
        return pOrigWriteFile(h, b, n, w, o);

    EnterCriticalSection(&rc_cs);

    size_t need = (size_t)n * 24 + 64;
    if (need > (size_t)INT_MAX)
        need = (size_t)INT_MAX;
    WCHAR* src = (WCHAR*)malloc(((size_t)n + 1) * sizeof(WCHAR));
    WCHAR* out = (WCHAR*)malloc(need * sizeof(WCHAR));
    if (!src || !out) {
        free(src);
        free(out);
        LeaveCriticalSection(&rc_cs);
        return pOrigWriteFile(h, b, n, w, o);
    }

    int wlen = MultiByteToWideChar(CP_UTF8, 0, (LPCCH)b, n, src, (int)n);
    if (wlen <= 0)
        wlen = MultiByteToWideChar(CP_ACP, 0, (LPCCH)b, n, src, (int)n);
    if (wlen > 0) {
        src[wlen] = L'\0';
        rc_MakeRainbow(src, out, (int)need);

        int clen =
            WideCharToMultiByte(CP_UTF8, 0, out, -1, NULL, 0, NULL, NULL);
        if (clen > 0) {
            CHAR* ansi = (CHAR*)malloc(clen);
            if (ansi) {
                WideCharToMultiByte(CP_UTF8, 0, out, -1, ansi, clen, NULL,
                                    NULL);
                BOOL ret = pOrigWriteFile(h, ansi, clen - 1, w, o);
                free(ansi);
                free(src);
                free(out);
                LeaveCriticalSection(&rc_cs);
                return ret;
            }
        }
    }

    free(src);
    free(out);
    LeaveCriticalSection(&rc_cs);
    return pOrigWriteFile(h, b, n, w, o);
}

static void LoadSettings() {
    rc_active = Wh_GetIntSetting(L"alwaysOn") != 0;
}

BOOL Wh_ModInit() {
    if (!rc_IsTargetProcess())
        return FALSE;

    InitializeCriticalSection(&rc_cs);
    LoadSettings();

    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD mode = 0;
    if (hOut && hOut != INVALID_HANDLE_VALUE && GetConsoleMode(hOut, &mode)) {
        mode |= 0x0004;
        SetConsoleMode(hOut, mode);
    }

    // 关键：kernel32.dll 的 WriteConsoleW/WriteFile 只是转发 stub，真正的实现在
    // kernelbase.dll。现代程序（cmd.exe、C 运行时等）直接调用 kernelbase，所以
    // 必须 Hook kernelbase.dll，否则在别人机器上不会生效。
    HMODULE hKernelBase = GetModuleHandleW(L"kernelbase.dll");
    if (hKernelBase) {
        void* pWCW = (void*)GetProcAddress(hKernelBase, "WriteConsoleW");
        void* pWF = (void*)GetProcAddress(hKernelBase, "WriteFile");
        if (pWCW)
            Wh_SetFunctionHook(pWCW, (void*)rc_HookWriteConsoleW,
                               (void**)&pOrigWriteConsoleW);
        if (pWF)
            Wh_SetFunctionHook(pWF, (void*)rc_HookWriteFile,
                               (void**)&pOrigWriteFile);
    }

    return TRUE;
}

void Wh_ModSettingsChanged() {
    LoadSettings();
}
void Wh_ModUninit() {
    DeleteCriticalSection(&rc_cs);
}