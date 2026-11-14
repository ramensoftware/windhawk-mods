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
- alwaysOn: false
  $name: Always On
  $description: "Enable rainbow immediately. [WARNING: DO NOT use with AI Agents.]"
*/
// ==/WindhawkModSettings==
// clang-format on

#include <windows.h>
#include <windhawk_utils.h>

typedef BOOL(
    WINAPI* WriteConsoleW_t)(HANDLE, const VOID*, DWORD, LPDWORD, LPVOID);
WriteConsoleW_t pOrigWriteConsoleW = nullptr;

typedef BOOL(
    WINAPI* WriteFile_t)(HANDLE, LPCVOID, DWORD, LPDWORD, LPOVERLAPPED);
WriteFile_t pOrigWriteFile = nullptr;

static CRITICAL_SECTION rc_cs;
static int rc_colorIndex = 0;
static BOOL rc_active = FALSE;

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
    if (!rc_active || n == 0 || n > (DWORD)INT_MAX) {
        return pOrigWriteConsoleW(h, b, n, w, r);
    }

    EnterCriticalSection(&rc_cs);

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
rc_HookWriteFile(HANDLE h, LPCVOID b, DWORD n, LPDWORD w, LPOVERLAPPED o) {
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
    if (wlen <= 0) {
        wlen = MultiByteToWideChar(CP_ACP, 0, (LPCCH)b, n, src, (int)n);
    }
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
    InitializeCriticalSection(&rc_cs);
    LoadSettings();

    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD mode = 0;
    if (hOut && hOut != INVALID_HANDLE_VALUE && GetConsoleMode(hOut, &mode)) {
        mode |= 0x0004;
        SetConsoleMode(hOut, mode);
    }

    HMODULE hKernelBase = GetModuleHandleW(L"kernelbase.dll");
    if (hKernelBase) {
        void* pWCW = (void*)GetProcAddress(hKernelBase, "WriteConsoleW");
        void* pWF = (void*)GetProcAddress(hKernelBase, "WriteFile");
        if (pWCW) {
            Wh_SetFunctionHook(pWCW, (void*)rc_HookWriteConsoleW,
                               (void**)&pOrigWriteConsoleW);
        }
        if (pWF) {
            Wh_SetFunctionHook(pWF, (void*)rc_HookWriteFile,
                               (void**)&pOrigWriteFile);
        }
    }

    return TRUE;
}

void Wh_ModSettingsChanged() {
    LoadSettings();
}

void Wh_ModUninit() {
    DeleteCriticalSection(&rc_cs);
}