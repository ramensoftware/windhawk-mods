// ==WindhawkMod==
// @id              chinese-logical-sort
// @name       Chinese Character Natural Sort
// @name:zh-CN     中文序号自然排序
// @name:zh-TW     中文序號自然排序
// @description      Provides natural sorting for Chinese file names. If you do not work with Chinese characters, there is no need to download this mod.
// @description:zh-CN     让 Windows 资源管理器支持中文习惯的自然排序（包含中文大小写数字、天干地支、上中下等）。彻底解决中文序号文件乱序问题。
// @description:zh-TW     讓 Windows 資源管理器支援中文習慣的自然排序（包含中文大小寫數字、天干地支、上中下等）。徹底解決中文序號檔案亂序問題。
// @version    1.0
// @author     Syphlix Oauthes
// @github     https://github.com/llw1573
// @include    *
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Chinese Character Natural Sort (Windhawk Mod)

**Notice: This plugin is designed exclusively for users who frequently work with Chinese file names. If your daily workflow does not involve Chinese characters, you do not need to install this mod.**

## Introduction
Windows lacks native support for the logical sequential sorting of Chinese characters. By default, Windows sorts Chinese characters alphabetically by their Pinyin pronunciation (in Simplified Chinese locales) or by stroke count (in Traditional Chinese locales). 

While this is fine for regular text, it completely breaks sequential numbering. For example, under Pinyin sorting rules, `二` (Er, 2) comes before `三` (San, 3), which comes before `一` (Yi, 1). Similarly, `上` (Shang, Upper), `下` (Xia, Lower), and `中` (Zhong, Middle) are sorted alphabetically (S -> X -> Z) rather than logically. This results in chaotic and frustrating file orders.

This Windhawk mod intercepts the underlying Windows string comparison APIs (`CompareStringEx`, `CompareStringW`, `StrCmpLogicalW`) to introduce a custom, natural sorting algorithm specifically tailored for Chinese counting systems and traditional sequences.

## Features

This mod perfectly handles the natural sort order for the following Chinese sequential characters:

*   **Chinese Numerals (Both Simplified/Traditional & Uppercase):** 
    Supports standard numerals (`一二三...十百千万亿`) and formal uppercase numerals (`壹贰叁...拾佰仟萬億`). It uses intelligent parsing, meaning complex numbers like `第一百二十三` (123) and `第一千` (1000) are correctly calculated and sorted sequentially.
*   **Heavenly Stems (Tiangan):** 
    Correctly sorts `甲, 乙, 丙, 丁, 戊, 己, 庚, 辛, 壬, 癸`.
*   **Earthly Branches (Dizhi):** 
    Correctly sorts `子, 丑, 寅, 卯, 辰, 巳, 午, 未, 申, 酉, 戌, 亥`.
*   **Traditional Volume Indicators:** 
    Correctly sorts `上, 中, 下` (Upper/Middle/Lower parts).
*   **Fallback Mechanism:** 
    If characters belong to different systems (e.g., Arabic numerals vs. Chinese numerals), it safely falls back to the native Windows sorting behavior.

This mod applies **globally** across your entire Windows environment. 

# 中文序号自然排序

## 简介
Windows 原生不支持中文序号的“自然逻辑排序”。在简体中文环境下，Windows 默认按照**拼音字母顺序**对汉字进行排序（繁体环境下按笔画数），这对于普通文本没问题，但对于文件序号来说简直是一场灾难。

例如，按照拼音规则：
*   `二(Er)` < `三(San)` < `一(Yi)`，导致“第二章”排在“第一章”前面。
*   `上(Shang)` < `下(Xia)` < `中(Zhong)`，导致排成了“上、下、中”。

**本插件通过在系统底层拦截 Windows 字符串比较 API（`CompareStringEx` 等），专门针对中文体系编写了自定义的自然排序算法，彻底解决上述乱序问题。**

## 核心功能
本插件完美支持并修正了以下中文序列的排序逻辑：

*   **中文数字（含繁简与大写）：** 
    支持常规数字（`一二三...十百千万亿`）以及财务大写（`壹贰叁...拾佰仟萬億`）。插件内置了智能解析引擎，能够正确识别多位数（例如 `第一百二十三` 会正确排在 `第九十九` 之后），全面取代单字对比。
*   **天干序号：** 
    正确识别并排序 `甲, 乙, 丙, 丁, 戊, 己, 庚, 辛, 壬, 癸`。
*   **地支序号：** 
    正确识别并排序 `子, 丑, 寅, 卯, 辰, 巳, 午, 未, 申, 酉, 戌, 亥`。
*   **传统卷宗分册：** 
    正确识别并排序 `上, 中, 下`。
*   **安全降级机制：** 
    如果遇到不同体系的序号（例如阿拉伯数字 `1` 对比 中文数字 `一`），或者完全无法构成逻辑比较的常规文字，插件会瞬间将裁决权交还给 Windows 原生系统，确保不会误伤或打乱其他正常文件的排序。

本插件被配置为**全局生效**。
*/
// ==/WindhawkModReadme==

#include <windows.h>
#include <shlwapi.h>
#include <cwctype>

#define CSTR_LESS_THAN            1
#define CSTR_EQUAL                2
#define CSTR_GREATER_THAN         3

typedef int (WINAPI *CompareStringEx_t)(LPCWSTR, DWORD, LPCWCH, int, LPCWCH, int, LPNLSVERSIONINFO, LPVOID, LPARAM);
CompareStringEx_t CompareStringEx_Original;

typedef int (WINAPI *CompareStringW_t)(LCID, DWORD, PCNZWCH, int, PCNZWCH, int);
CompareStringW_t CompareStringW_Original;

typedef int (WINAPI *StrCmpLogicalW_t)(PCWSTR, PCWSTR);
StrCmpLogicalW_t StrCmpLogicalW_Original;

enum Category { CAT_NONE, CAT_ARABIC, CAT_NUMBER, CAT_TIANGAN, CAT_DIZHI, CAT_SHANGZHONGXIA };

Category GetCategory(wchar_t c) {
    if (c == 0) return CAT_NONE;
    if (c >= L'0' && c <= L'9') return CAT_ARABIC;
    if (c < 0x3000) return CAT_NONE; 
    if (wcschr(L"甲乙丙丁戊己庚辛壬癸", c)) return CAT_TIANGAN;
    if (wcschr(L"子丑寅卯辰巳午未申酉戌亥", c)) return CAT_DIZHI;
    if (wcschr(L"上中下", c)) return CAT_SHANGZHONGXIA;
    if (wcschr(L"〇零一二两三四五六七八九十百千万亿壹贰叁肆伍陆柒捌玖拾佰仟萬億", c)) return CAT_NUMBER;
    return CAT_NONE;
}

int ParseSequence(wchar_t c, PCWSTR seq) {
    if (c == 0) return 0;
    PCWSTR p = wcschr(seq, c);
    if (p) return (int)(p - seq) + 1;
    return 0;
}

long long ParseArabicNumber(PCWSTR str, int maxLength, int& parsedLen) {
    long long val = 0;
    int i = 0;
    while (i < maxLength && str[i] >= L'0' && str[i] <= L'9') {
        if (val < LLONG_MAX / 10) val = val * 10 + (str[i] - L'0');
        i++;
    }
    parsedLen = i;
    return val;
}

long long ParseChineseNumber(PCWSTR str, int maxLength, int& parsedLen) {
    long long total = 0, section = 0, temp = 0;
    int i = 0;
    bool found = false;

    while (i < maxLength && str[i]) {
        wchar_t c = str[i];
        long long digit = -1;
        if (c == L'〇' || c == L'零') digit = 0;
        else if (c == L'一' || c == L'壹') digit = 1;
        else if (c == L'二' || c == L'两') digit = 2;
        else if (c == L'三' || c == L'叁') digit = 3;
        else if (c == L'四' || c == L'肆') digit = 4;
        else if (c == L'五' || c == L'伍') digit = 5;
        else if (c == L'六' || c == L'陆') digit = 6;
        else if (c == L'七' || c == L'柒') digit = 7;
        else if (c == L'八' || c == L'捌') digit = 8;
        else if (c == L'九' || c == L'玖') digit = 9;

        if (digit != -1) {
            temp = temp * 10 + digit;
            found = true;
        } 
        // 匹配单位
        else if (c == L'十' || c == L'拾') {
            if (temp == 0) temp = 1;
            section += temp * 10;
            temp = 0;
            found = true;
        } else if (c == L'百' || c == L'佰') {
            if (temp == 0) temp = 1;
            section += temp * 100;
            temp = 0;
            found = true;
        } else if (c == L'千' || c == L'仟') {
            if (temp == 0) temp = 1;
            section += temp * 1000;
            temp = 0;
            found = true;
        } else if (c == L'万' || c == L'萬') {
            if (temp == 0) temp = 1;
            total += (section + temp) * 10000;
            section = 0, temp = 0;
            found = true;
        } else if (c == L'亿' || c == L'億') {
            if (temp == 0) temp = 1;
            total += (total + section + temp) * 100000000;
            section = 0, temp = 0;
            found = true;
        } else {
            break;
        }
        i++;
    }

    if (found) {
        total += section + temp;
        parsedLen = i;
        return total;
    }

    parsedLen = 0;
    return -1;
}

// 核心比对引擎
int CustomCompare(LPCWCH lpString1, int len1, LPCWCH lpString2, int len2) {
    if (len1 == 0 || len2 == 0) return 0;

    int i = 0, j = 0;
    while (i < len1 && j < len2) {
        Category cat1 = GetCategory(lpString1[i]);
        Category cat2 = GetCategory(lpString2[j]);

        if (cat1 == CAT_ARABIC && cat2 == CAT_ARABIC) {
            int l1 = 0, l2 = 0;
            long long val1 = ParseArabicNumber(lpString1 + i, len1 - i, l1);
            long long val2 = ParseArabicNumber(lpString2 + j, len2 - j, l2);
            if (val1 != val2) return val1 < val2 ? CSTR_LESS_THAN : CSTR_GREATER_THAN;
            if (l1 == l2 && wcsncmp(lpString1 + i, lpString2 + j, l1) == 0) {
                i += l1; j += l2; continue;
            }
            return 0;
        }

        if (cat1 != CAT_NONE && cat1 != CAT_ARABIC && cat1 == cat2) {
            int l1 = 0, l2 = 0;
            long long val1 = -1, val2 = -1;
            
            if (cat1 == CAT_NUMBER) {
                val1 = ParseChineseNumber(lpString1 + i, len1 - i, l1);
                val2 = ParseChineseNumber(lpString2 + j, len2 - j, l2);
            } else if (cat1 == CAT_TIANGAN) {
                val1 = ParseSequence(lpString1[i], L"甲乙丙丁戊己庚辛壬癸"); l1 = 1;
                val2 = ParseSequence(lpString2[j], L"甲乙丙丁戊己庚辛壬癸"); l2 = 1;
            } else if (cat1 == CAT_DIZHI) {
                val1 = ParseSequence(lpString1[i], L"子丑寅卯辰巳午未申酉戌亥"); l1 = 1;
                val2 = ParseSequence(lpString2[j], L"子丑寅卯辰巳午未申酉戌亥"); l2 = 1;
            } else if (cat1 == CAT_SHANGZHONGXIA) {
                val1 = ParseSequence(lpString1[i], L"上中下"); l1 = 1;
                val2 = ParseSequence(lpString2[j], L"上中下"); l2 = 1;
            }
            
            if (val1 != -1 && val2 != -1 && val1 != val2) {
                return val1 < val2 ? CSTR_LESS_THAN : CSTR_GREATER_THAN;
            } else {
                if (l1 == l2 && wcsncmp(lpString1 + i, lpString2 + j, l1) == 0) {
                    i += l1; j += l2; continue;
                }
                return 0;
            }
        }

        if (towlower(lpString1[i]) == towlower(lpString2[j])) {
            i++; j++; continue;
        }

        return 0; 
    }
    
    return 0;
}

int WINAPI CompareStringEx_Hook(
  LPCWSTR          lpLocaleName,
  DWORD            dwCmpFlags,
  LPCWCH           lpString1,
  int              cchCount1,
  LPCWCH           lpString2,
  int              cchCount2,
  LPNLSVERSIONINFO lpVersionInformation,
  LPVOID           lpReserved,
  LPARAM           lParam)
{
    if (lpString1 && lpString2) {
        int len1 = (cchCount1 < 0) ? lstrlenW(lpString1) : cchCount1;
        int len2 = (cchCount2 < 0) ? lstrlenW(lpString2) : cchCount2;
        int res = CustomCompare(lpString1, len1, lpString2, len2);
        if (res != 0) return res;
    }
    return CompareStringEx_Original(lpLocaleName, dwCmpFlags, lpString1, cchCount1, lpString2, cchCount2, lpVersionInformation, lpReserved, lParam);
}

int WINAPI CompareStringW_Hook(
  LCID     Locale,
  DWORD    dwCmpFlags,
  PCNZWCH  lpString1,
  int      cchCount1,
  PCNZWCH  lpString2,
  int      cchCount2)
{
    if (lpString1 && lpString2) {
        int len1 = (cchCount1 < 0) ? lstrlenW(lpString1) : cchCount1;
        int len2 = (cchCount2 < 0) ? lstrlenW(lpString2) : cchCount2;
        int res = CustomCompare(lpString1, len1, lpString2, len2);
        if (res != 0) return res;
    }
    return CompareStringW_Original(Locale, dwCmpFlags, lpString1, cchCount1, lpString2, cchCount2);
}

int WINAPI StrCmpLogicalW_Hook(PCWSTR psz1, PCWSTR psz2) {
    if (psz1 && psz2) {
        int res = CustomCompare(psz1, lstrlenW(psz1), psz2, lstrlenW(psz2));
        if (res == CSTR_LESS_THAN) return -1;
        if (res == CSTR_GREATER_THAN) return 1;
    }
    return StrCmpLogicalW_Original(psz1, psz2);
}

BOOL Wh_ModInit() {
    Wh_Log(L"Initializing Modern Chinese Logical Sort mod");
    
    HMODULE hKernelBase = GetModuleHandleW(L"kernelbase.dll");
    if (!hKernelBase) hKernelBase = GetModuleHandleW(L"kernel32.dll");
    
    if (hKernelBase) {
        FARPROC pCompareStringEx = GetProcAddress(hKernelBase, "CompareStringEx");
        if (pCompareStringEx) {
            Wh_SetFunctionHook((void*)pCompareStringEx, (void*)CompareStringEx_Hook, (void**)&CompareStringEx_Original);
        }

        FARPROC pCompareStringW = GetProcAddress(hKernelBase, "CompareStringW");
        if (pCompareStringW) {
            Wh_SetFunctionHook((void*)pCompareStringW, (void*)CompareStringW_Hook, (void**)&CompareStringW_Original);
        }
    }

    HMODULE hShlwapi = LoadLibraryW(L"shlwapi.dll");
    if (hShlwapi) {
        FARPROC pStrCmpLogicalW = GetProcAddress(hShlwapi, "StrCmpLogicalW");
        if (pStrCmpLogicalW) {
            Wh_SetFunctionHook((void*)pStrCmpLogicalW, (void*)StrCmpLogicalW_Hook, (void**)&StrCmpLogicalW_Original);
        }
    }

    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L"Unloading Chinese Logical Sort mod");
}