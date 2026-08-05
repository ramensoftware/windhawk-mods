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

# 中文序號自然排序 (繁体中文介绍)

## 簡介
Windows 原生不支援中文序號的「自然邏輯排序」。在簡體中文環境下，Windows 預設按照**拼音字母順序**對漢字進行排序（繁體環境下按筆畫數），這對於普通文本沒問題，但對於檔案序號來說簡直是一場災難。

例如，按照拼音規則：
*   `二(Er)` < `三(San)` < `一(Yi)`，導致「第二章」排在「第一章」前面。
*   `上(Shang)` < `下(Xia)` < `中(Zhong)`，導致排成了「上、下、中」。

**本模組透過在系統底層攔截 Windows 字串比較 API（`CompareStringEx` 等），專門針對中文體系編寫了自訂的自然排序演算法，徹底解決上述亂序問題。**

## 核心功能
本模組完美支援並修正了以下中文序列的排序邏輯：

*   **中文數字（含繁簡與大寫）：** 
支援常規數字（`一二三...十百千萬億`）以及財務大寫（`壹貳叁...拾佰仟萬億`）。模組內建了智慧解析引擎，能夠正確識別多位數（例如 `第一百二十三` 會正確排在 `第九十九` 之後），全面取代單字比對。
*   **天干序號：** 
正確識別並排序 `甲, 乙, 丙, 丁, 戊, 己, 庚, 辛, 壬, 癸`。
*   **地支序號：** 
正確識別並排序 `子, 丑, 寅, 卯, 辰, 巳, 午, 未, 申, 酉, 戌, 亥`。
*   **傳統卷宗分冊：** 
正確識別並排序 `上, 中, 下`。
*   **安全降級機制：** 
如果遇到不同體系的序號（例如阿拉伯數字 `1` 比對 中文數字 `一`），或者完全無法構成邏輯比較的常規文字，模組會瞬間將裁決權交還給 Windows 原生系統，確保不會誤傷或打亂其他正常檔案的排序。

本模組被配置為**全域生效**。
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- enable_number: true
  $name: Enable Chinese numerals (一二三 / 壹贰叁)
  $name:zh-CN: 启用中文数字 (一二三 / 壹贰叁)
  $name:zh-TW: 啟用中文數字 (一二三 / 壹貳叁)
- enable_tiangan: true
  $name: Enable Heavenly Stems (甲乙丙...)
  $name:zh-CN: 启用天干排序 (甲乙丙...)
  $name:zh-TW: 啟用天干排序 (甲乙丙...)
- enable_dizhi: true
  $name: Enable Earthly Branches (子丑寅...)
  $name:zh-CN: 启用地支排序 (子丑寅...)
  $name:zh-TW: 啟用地支排序 (子丑寅...)
- enable_ganzhi: true
  $name: Enable Ganzhi pairs (甲子, 乙丑...) using the 60-cycle order
  $name:zh-CN: 启用干支组合 (甲子, 乙丑...)，按六十甲子顺序
  $name:zh-TW: 啟用干支組合 (甲子, 乙丑...)，按六十甲子順序
- enable_szx: true
  $name: Enable Shang/Zhong/Xia (上中下)
  $name:zh-CN: 启用上中下排序 (上中下)
  $name:zh-TW: 啟用上中下排序 (上中下)
- hook_comparestring: true
  $name: Also hook CompareStringEx / CompareStringW
  $name:zh-CN: 同时 hook CompareStringEx / CompareStringW
  $name:zh-TW: 同時 hook CompareStringEx / CompareStringW
  $description: Off = only StrCmpLogicalW (what Explorer's file list uses). Smaller blast radius.
  $description:zh-CN: 关闭 = 只 hook StrCmpLogicalW（资源管理器文件列表用的就是它），影响面更小。
  $description:zh-TW: 關閉 = 只 hook StrCmpLogicalW（檔案總管檔案列表用的就是它），影響面更小。
- only_chinese_locale: true
  $name: Only take over for zh-* locales
  $name:zh-CN: 仅对 zh-* locale 生效
  $name:zh-TW: 僅對 zh-* locale 生效
  $description: Turn off if you keep Chinese file names on a non-Chinese Windows.
  $description:zh-CN: 如果你在非中文 Windows 上放中文文件名，关掉这项。
  $description:zh-TW: 如果你在非中文 Windows 上放中文檔名，關掉這項。
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <shlwapi.h>
#include <limits.h>
#include <windhawk_utils.h>

namespace cls {

typedef long long i64;

bool g_num = true;
bool g_tg = true;
bool g_dz = true;
bool g_gz = true;
bool g_szx = true;

typedef int (*NativeSegCmp)(void* ctx, const wchar_t* a, int alen,
                            const wchar_t* b, int blen);

enum Kind {
    K_NUM = 1,
    K_GANZHI = 2,
    K_TG = 3,
    K_DZ = 4,
    K_SZX = 5,
    K_TEXT = 6  
};

static bool IsWordChar(wchar_t c) {
    if (c >= L'0' && c <= L'9') return true;
    if (c >= L'A' && c <= L'Z') return true;
    if (c >= L'a' && c <= L'z') return true;
    if (c < 0x80) return false;
    if (c >= 0x00C0 && c <= 0x024F) return true;
    if (c >= 0x0370 && c <= 0x1FFF) return true;
    if (c >= 0x3040 && c <= 0x30FF) return true;
    if (c >= 0x3400 && c <= 0x4DBF) return true;
    if (c >= 0x4E00 && c <= 0x9FFF) return true;
    if (c >= 0xAC00 && c <= 0xD7A3) return true;
    if (c >= 0xF900 && c <= 0xFAFF) return true;
    if (c >= 0xFF21 && c <= 0xFF3A) return true;
    if (c >= 0xFF41 && c <= 0xFF5A) return true;
    if (c >= 0xFF10 && c <= 0xFF19) return true;
    return false;
}

static bool IsDelim(wchar_t c) { return !IsWordChar(c); }

static int CnDigit(wchar_t c) {
    switch (c) {
        case L'〇': case L'零': return 0;
        case L'一': case L'壹': return 1;
        case L'二': case L'贰': case L'貳': case L'貮': return 2;
        case L'三': case L'叁': return 3;
        case L'四': case L'肆': return 4;
        case L'五': case L'伍': return 5;
        case L'六': case L'陆': case L'陸': return 6;
        case L'七': case L'柒': return 7;
        case L'八': case L'捌': return 8;
        case L'九': case L'玖': return 9;
        default: return -1;
    }
}

static i64 CnUnit(wchar_t c) {
    switch (c) {
        case L'十': case L'拾': return 10;
        case L'百': case L'佰': return 100;
        case L'千': case L'仟': return 1000;
        case L'万': case L'萬': return 10000;
        case L'亿': case L'億': return 100000000;
        default: return 0;
    }
}

static bool IsCnNumChar(wchar_t c) { return CnDigit(c) >= 0 || CnUnit(c) > 0; }

static int TgIndex(wchar_t c) {
    switch (c) {
        case L'甲': return 1;
        case L'乙': return 2;
        case L'丙': return 3;
        case L'丁': return 4;
        case L'戊': return 5;
        case L'己': return 6;
        case L'庚': return 7;
        case L'辛': return 8;
        case L'壬': return 9;
        case L'癸': return 10;
        default: return 0;
    }
}

static int DzIndex(wchar_t c) {
    switch (c) {
        case L'子': return 1;
        case L'丑': return 2;
        case L'寅': return 3;
        case L'卯': return 4;
        case L'辰': return 5;
        case L'巳': return 6;
        case L'午': return 7;
        case L'未': return 8;
        case L'申': return 9;
        case L'酉': return 10;
        case L'戌': return 11;
        case L'亥': return 12;
        default: return 0;
    }
}

static int SzxIndex(wchar_t c) {
    switch (c) {
        case L'上': return 1;
        case L'中': return 2;
        case L'下': return 3;
        default: return 0;
    }
}

static bool IsStrongUnit(wchar_t c) {
    switch (c) {
        case L'章': case L'节': case L'節':
        case L'回': case L'册': case L'冊':
        case L'卷': case L'部': case L'篇':
        case L'课': case L'課': case L'集':
        case L'期': case L'号': case L'號':
        case L'版': case L'讲': case L'講':
        case L'话': case L'話': case L'季':
        case L'幕': case L'页': case L'頁':
        case L'条': case L'條': case L'款':
        case L'项': case L'項': case L'步':
        case L'层': case L'層': case L'届':
        case L'屆': case L'次': case L'段':
        case L'组': case L'組': case L'类':
        case L'類': case L'级': case L'級':
        case L'辑': case L'輯': case L'张':
        case L'張': case L'班': case L'队':
        case L'隊': case L'种': case L'種':
        case L'型': case L'表': case L'图':
        case L'圖': case L'楼': case L'室':
            return true;
        default: return false;
    }
}

static bool IsWeakUnit(wchar_t c) {
    switch (c) {
        case L'年': case L'月': case L'日':
        case L'天': case L'周': case L'份':
            return true;
        default: return false;
    }
}

static bool IsUnit(wchar_t c) { return IsStrongUnit(c) || IsWeakUnit(c); }

static bool IsMarker(wchar_t c) {
    switch (c) {
        case L'第': case L'其': case L'卷':
        case L'册': case L'冊': case L'篇':
        case L'章': case L'回': case L'节':
        case L'節': case L'课': case L'課':
        case L'部': case L'集': case L'期':
        case L'号': case L'號': case L'版':
        case L'讲': case L'講': case L'表':
        case L'图': case L'圖': case L'附':
        case L'组': case L'組': case L'类':
        case L'類': case L'级': case L'級':
        case L'编': case L'編':
            return true;
        default: return false;
    }
}

static bool IsTgUnit(wchar_t c) {
    switch (c) {
        case L'方': case L'组': case L'組':
        case L'类': case L'類': case L'级':
        case L'級': case L'号': case L'號':
        case L'部': case L'册': case L'冊':
        case L'卷': case L'篇': case L'章':
        case L'节': case L'節': case L'表':
        case L'图': case L'圖': case L'队':
        case L'隊': case L'班': case L'种':
        case L'種': case L'型':
            return true;
        default: return false;
    }
}

static bool IsDzUnit(wchar_t c) {
    switch (c) {
        case L'时': case L'時': case L'刻':
        case L'位': case L'宫': case L'宮':
            return true;
        default: return false;
    }
}

static bool IsGzUnit(wchar_t c) {
    switch (c) {
        case L'年': case L'月': case L'日':
        case L'时': case L'時': case L'岁':
        case L'歲':
            return true;
        default: return false;
    }
}

static bool IsSzxUnit(wchar_t c) {
    switch (c) {
        case L'册': case L'冊': case L'部':
        case L'卷': case L'集': case L'篇':
        case L'半': case L'旬':
            return true;
        default: return false;
    }
}

static bool MulChk(i64 a, i64 b, i64* out) {
    if (a == 0 || b == 0) { *out = 0; return true; }
    if (a > LLONG_MAX / b) return false;
    *out = a * b;
    return true;
}

static bool AddChk(i64 a, i64 b, i64* out) {
    if (a > LLONG_MAX - b) return false;
    *out = a + b;
    return true;
}

static bool ParseCnNumber(const wchar_t* str, int maxLen, i64* outVal, int* outLen) {
    i64 total = 0, section = 0, temp = 0;
    int i = 0;
    bool any = false;

    while (i < maxLen) {
        wchar_t c = str[i];
        int d = CnDigit(c);
        if (d >= 0) {
            i64 t;
            if (!MulChk(temp, 10, &t)) return false;
            if (!AddChk(t, d, &temp)) return false;
            any = true;
            i++;
            continue;
        }
        i64 u = CnUnit(c);
        if (u == 0) break;

        if (u == 10000 || u == 100000000) {
            i64 head;
            if (!AddChk(section, temp, &head)) return false;
            if (u == 100000000) {
                if (!AddChk(total, head, &head)) return false;
                if (head == 0) head = 1;
                if (!MulChk(head, u, &total)) return false;
            } else {
                if (head == 0) head = 1;
                i64 scaled;
                if (!MulChk(head, u, &scaled)) return false;
                if (!AddChk(total, scaled, &total)) return false;
            }
            section = 0;
            temp = 0;
        } else {
            if (temp == 0) temp = 1;
            i64 scaled;
            if (!MulChk(temp, u, &scaled)) return false;
            if (!AddChk(section, scaled, &section)) return false;
            temp = 0;
        }
        any = true;
        i++;
    }

    if (!any) return false;
    i64 v;
    if (!AddChk(section, temp, &v)) return false;
    if (!AddChk(total, v, &v)) return false;
    *outVal = v;
    *outLen = i;
    return true;
}

static wchar_t Prev(const wchar_t* s, int i) { return i > 0 ? s[i - 1] : 0; }

static bool NumGate(const wchar_t* s, int len, int i, int runLen) {
    wchar_t before = Prev(s, i);
    if (before != 0 && IsMarker(before)) return true;

    int after = i + runLen;
    if (after >= len) return true;
    wchar_t a = s[after];
    if (IsDelim(a)) return true;
    if (IsStrongUnit(a)) return true;
    if (IsWeakUnit(a)) {
        if (runLen >= 2) return true;
        int k = after;
        int steps = 0;
        while (k < len && steps < 2 && IsUnit(s[k])) { k++; steps++; }
        if (k >= len || IsDelim(s[k])) return true;
    }
    return false;
}

static bool SingleGate(const wchar_t* s, int len, int i,
                       bool (*isOwnUnit)(wchar_t)) {
    wchar_t before = Prev(s, i);
    if (before != 0 && IsMarker(before)) return true;
    wchar_t after = (i + 1 < len) ? s[i + 1] : 0;
    if (after != 0 && isOwnUnit(after)) return true;
    bool leftOk = (before == 0) || IsDelim(before);
    bool rightOk = (after == 0) || IsDelim(after);
    return leftOk && rightOk;
}

static int GanzhiIndex(int tg, int dz) {
    if (tg < 1 || tg > 10 || dz < 1 || dz > 12) return 0;
    for (int n = 0; n < 60; n++) {
        if (n % 10 == (tg - 1) && n % 12 == (dz - 1)) return n + 1;
    }
    return 0;
}

static bool GanzhiGate(const wchar_t* s, int len, int i) {
    wchar_t before = Prev(s, i);
    if (before != 0 && IsMarker(before)) return true;
    wchar_t after = (i + 2 < len) ? s[i + 2] : 0;
    if (after != 0 && IsGzUnit(after)) return true;
    bool leftOk = (before == 0) || IsDelim(before);
    bool rightOk = (after == 0) || IsDelim(after);
    return leftOk && rightOk;
}

struct Token {
    int kind;
    i64 val;
    int off;
    int len;
};

const int MAX_TOKENS = 64;

static bool IsCandidate(wchar_t c) {
    if (c != 0x3007 && (c < 0x4E00 || c > 0x9FFF)) return false;
    if (g_num && IsCnNumChar(c)) return true;
    if ((g_tg || g_gz) && TgIndex(c)) return true;
    if (g_dz && DzIndex(c)) return true;
    if (g_szx && SzxIndex(c)) return true;
    return false;
}

static bool HasCandidate(const wchar_t* s, int len) {
    for (int i = 0; i < len; i++) {
        if (IsCandidate(s[i])) return true;
    }
    return false;
}

static bool ReadOrdinal(const wchar_t* s, int len, int i, Token* out) {
    if (g_num && IsCnNumChar(s[i])) {
        int runLen = 0;
        while (i + runLen < len && IsCnNumChar(s[i + runLen])) runLen++;
        i64 val;
        int used;
        if (ParseCnNumber(s + i, runLen, &val, &used) && used == runLen &&
            NumGate(s, len, i, runLen)) {
            out->kind = K_NUM;
            out->val = val;
            out->off = i;
            out->len = runLen;
            return true;
        }
        return false;
    }

    int tg = TgIndex(s[i]);
    if (tg) {
        if (g_gz && i + 1 < len) {
            int dz = DzIndex(s[i + 1]);
            int gz = dz ? GanzhiIndex(tg, dz) : 0;
            if (gz && GanzhiGate(s, len, i)) {
                out->kind = K_GANZHI;
                out->val = gz;
                out->off = i;
                out->len = 2;
                return true;
            }
        }
        if (g_tg && SingleGate(s, len, i, IsTgUnit)) {
            out->kind = K_TG;
            out->val = tg;
            out->off = i;
            out->len = 1;
            return true;
        }
        return false;
    }

    int dz = DzIndex(s[i]);
    if (dz && g_dz && SingleGate(s, len, i, IsDzUnit)) {
        out->kind = K_DZ;
        out->val = dz;
        out->off = i;
        out->len = 1;
        return true;
    }

    int sx = SzxIndex(s[i]);
    if (sx && g_szx && SingleGate(s, len, i, IsSzxUnit)) {
        out->kind = K_SZX;
        out->val = sx;
        out->off = i;
        out->len = 1;
        return true;
    }
    return false;
}

static int Tokenize(const wchar_t* s, int len, Token* out, int maxTokens) {
    int n = 0;
    int textStart = 0;
    int i = 0;

    while (i < len) {
        Token t;
        if (n + 2 <= maxTokens && ReadOrdinal(s, len, i, &t)) {
            if (i > textStart) {
                out[n].kind = K_TEXT;
                out[n].val = 0;
                out[n].off = textStart;
                out[n].len = i - textStart;
                n++;
            }
            out[n++] = t;
            i += t.len;
            textStart = i;
            continue;
        }
        i++;
    }

    if (textStart < len && n < maxTokens) {
        out[n].kind = K_TEXT;
        out[n].val = 0;
        out[n].off = textStart;
        out[n].len = len - textStart;
        n++;
    }
    return n;
}

static int Compare(const wchar_t* a, int alen, const wchar_t* b, int blen,
                   NativeSegCmp segCmp, void* ctx, bool* handled) {
    *handled = false;
    if (alen <= 0 || blen <= 0) return 0;

    if (!HasCandidate(a, alen) && !HasCandidate(b, blen)) return 0;

    Token ta[MAX_TOKENS], tb[MAX_TOKENS];
    int na = Tokenize(a, alen, ta, MAX_TOKENS);
    int nb = Tokenize(b, blen, tb, MAX_TOKENS);

    int n = na < nb ? na : nb;
    for (int k = 0; k < n; k++) {
        if (ta[k].kind != tb[k].kind) {
            *handled = true;
            return ta[k].kind < tb[k].kind ? -1 : 1;
        }
        if (ta[k].kind != K_TEXT) {
            if (ta[k].val != tb[k].val) {
                *handled = true;
                return ta[k].val < tb[k].val ? -1 : 1;
            }
            continue;
        }
        int r = segCmp(ctx, a + ta[k].off, ta[k].len, b + tb[k].off, tb[k].len);
        if (r == -2) return 0;
        if (r != 0) {
            *handled = true;
            return r < 0 ? -1 : 1;
        }
    }

    if (na != nb) {
        *handled = true;
        return na < nb ? -1 : 1;
    }

    return 0;
}

}

enum ApiKind { API_EX, API_W, API_LOGICAL };

struct NativeCtx {
    ApiKind api;
    LPCWSTR localeName;
    LCID lcid;
    DWORD flags;
    LPNLSVERSIONINFO verInfo;
    LPVOID reserved;
    LPARAM lParam;
};

typedef int(WINAPI* CompareStringEx_t)(LPCWSTR, DWORD, LPCWCH, int, LPCWCH, int,
                                       LPNLSVERSIONINFO, LPVOID, LPARAM);
typedef int(WINAPI* CompareStringW_t)(LCID, DWORD, PCNZWCH, int, PCNZWCH, int);
typedef int(WINAPI* StrCmpLogicalW_t)(PCWSTR, PCWSTR);

CompareStringEx_t CompareStringEx_Original;
CompareStringW_t CompareStringW_Original;
StrCmpLogicalW_t StrCmpLogicalW_Original;

template <typename T>
static BOOL SetHookT(T targetFunction, T hookFunction, T* originalFunction) {
    return Wh_SetFunctionHook((void*)targetFunction, (void*)hookFunction,
                              (void**)originalFunction);
}

bool g_hookCompareString = true;
bool g_onlyChineseLocale = true;
bool g_userLocaleIsChinese = false;

DWORD g_tlsDepth = TLS_OUT_OF_INDEXES;

static bool GuardEnter() {
    if (g_tlsDepth == TLS_OUT_OF_INDEXES) return true;
    DWORD err = GetLastError();
    INT_PTR d = (INT_PTR)TlsGetValue(g_tlsDepth);
    SetLastError(err);
    if (d != 0) return false;
    TlsSetValue(g_tlsDepth, (LPVOID)(INT_PTR)1);
    SetLastError(err);
    return true;
}

static void GuardLeave() {
    if (g_tlsDepth == TLS_OUT_OF_INDEXES) return;
    DWORD err = GetLastError();
    TlsSetValue(g_tlsDepth, (LPVOID)(INT_PTR)0);
    SetLastError(err);
}

static bool IsChineseLocaleName(LPCWSTR name) {
    if (!name) return false;
    if (name[0] == 0) return false;
    if (CompareStringOrdinal(name, -1, LOCALE_NAME_SYSTEM_DEFAULT, -1, TRUE) ==
        CSTR_EQUAL) {
        return g_userLocaleIsChinese;
    }
    if (CompareStringOrdinal(name, -1, LOCALE_NAME_USER_DEFAULT, -1, TRUE) ==
        CSTR_EQUAL) {
        return g_userLocaleIsChinese;
    }
    return (name[0] == L'z' || name[0] == L'Z') &&
           (name[1] == L'h' || name[1] == L'H') &&
           (name[2] == 0 || name[2] == L'-');
}

static bool TakeoverAllowedEx(LPCWSTR localeName) {
    if (localeName && localeName[0] == 0) return false;
    if (!g_onlyChineseLocale) return true;
    if (!localeName) return g_userLocaleIsChinese;
    return IsChineseLocaleName(localeName);
}

static bool TakeoverAllowedW(LCID lcid) {
    if (lcid == LOCALE_INVARIANT) return false;
    if (!g_onlyChineseLocale) return true;
    if (lcid == LOCALE_USER_DEFAULT || lcid == LOCALE_SYSTEM_DEFAULT) {
        return g_userLocaleIsChinese;
    }
    return PRIMARYLANGID(LANGIDFROMLCID(lcid)) == LANG_CHINESE;
}

static int NativeSeg(void* ctxRaw, const wchar_t* a, int alen, const wchar_t* b,
                     int blen) {
    NativeCtx* c = (NativeCtx*)ctxRaw;
    int r = 0;

    if (c->api == API_EX) {
        r = CompareStringEx_Original(c->localeName, c->flags, a, alen, b, blen,
                                     c->verInfo, c->reserved, c->lParam);
    } else if (c->api == API_W) {
        r = CompareStringW_Original(c->lcid, c->flags, a, alen, b, blen);
    } else {
        wchar_t sa[MAX_PATH], sb[MAX_PATH];
        wchar_t* pa = sa;
        wchar_t* pb = sb;
        bool heapA = false, heapB = false;

        if (alen >= MAX_PATH) {
            pa = (wchar_t*)HeapAlloc(GetProcessHeap(), 0,
                                     (SIZE_T)(alen + 1) * sizeof(wchar_t));
            if (!pa) return -2;
            heapA = true;
        }
        if (blen >= MAX_PATH) {
            pb = (wchar_t*)HeapAlloc(GetProcessHeap(), 0,
                                     (SIZE_T)(blen + 1) * sizeof(wchar_t));
            if (!pb) {
                if (heapA) HeapFree(GetProcessHeap(), 0, pa);
                return -2;
            }
            heapB = true;
        }

        memcpy(pa, a, (SIZE_T)alen * sizeof(wchar_t));
        pa[alen] = 0;
        memcpy(pb, b, (SIZE_T)blen * sizeof(wchar_t));
        pb[blen] = 0;

        int lr = StrCmpLogicalW_Original(pa, pb);

        if (heapA) HeapFree(GetProcessHeap(), 0, pa);
        if (heapB) HeapFree(GetProcessHeap(), 0, pb);
        return lr < 0 ? -1 : (lr > 0 ? 1 : 0);
    }

    if (r == 0) return -2;
    return r == CSTR_LESS_THAN ? -1 : (r == CSTR_GREATER_THAN ? 1 : 0);
}

int WINAPI CompareStringEx_Hook(LPCWSTR lpLocaleName, DWORD dwCmpFlags,
                                LPCWCH lpString1, int cchCount1, LPCWCH lpString2,
                                int cchCount2, LPNLSVERSIONINFO lpVersionInformation,
                                LPVOID lpReserved, LPARAM lParam) {
    DWORD err = GetLastError();

    if (lpString1 && lpString2 && TakeoverAllowedEx(lpLocaleName) && GuardEnter()) {
        int len1 = (cchCount1 < 0) ? lstrlenW((LPCWSTR)lpString1) : cchCount1;
        int len2 = (cchCount2 < 0) ? lstrlenW((LPCWSTR)lpString2) : cchCount2;

        NativeCtx ctx = {API_EX,   lpLocaleName, 0,          dwCmpFlags,
                         lpVersionInformation, lpReserved,   lParam};
        bool handled = false;
        SetLastError(err);
        int r = cls::Compare(lpString1, len1, lpString2, len2, NativeSeg, &ctx,
                             &handled);
        if (handled) {
            GuardLeave();
            SetLastError(err);
            return r < 0 ? CSTR_LESS_THAN : CSTR_GREATER_THAN;
        }
        SetLastError(err);
        int nr = CompareStringEx_Original(lpLocaleName, dwCmpFlags, lpString1,
                                          cchCount1, lpString2, cchCount2,
                                          lpVersionInformation, lpReserved, lParam);
        GuardLeave();
        return nr;
    }

    SetLastError(err);
    return CompareStringEx_Original(lpLocaleName, dwCmpFlags, lpString1, cchCount1,
                                    lpString2, cchCount2, lpVersionInformation,
                                    lpReserved, lParam);
}

int WINAPI CompareStringW_Hook(LCID Locale, DWORD dwCmpFlags, PCNZWCH lpString1,
                               int cchCount1, PCNZWCH lpString2, int cchCount2) {
    DWORD err = GetLastError();

    if (lpString1 && lpString2 && TakeoverAllowedW(Locale) && GuardEnter()) {
        int len1 = (cchCount1 < 0) ? lstrlenW((LPCWSTR)lpString1) : cchCount1;
        int len2 = (cchCount2 < 0) ? lstrlenW((LPCWSTR)lpString2) : cchCount2;

        NativeCtx ctx = {API_W, nullptr, Locale, dwCmpFlags, nullptr, nullptr, 0};
        bool handled = false;
        SetLastError(err);
        int r = cls::Compare(lpString1, len1, lpString2, len2, NativeSeg, &ctx,
                             &handled);
        if (handled) {
            GuardLeave();
            SetLastError(err);
            return r < 0 ? CSTR_LESS_THAN : CSTR_GREATER_THAN;
        }
        SetLastError(err);
        int nr = CompareStringW_Original(Locale, dwCmpFlags, lpString1, cchCount1,
                                         lpString2, cchCount2);
        GuardLeave();
        return nr;
    }

    SetLastError(err);
    return CompareStringW_Original(Locale, dwCmpFlags, lpString1, cchCount1,
                                   lpString2, cchCount2);
}

int WINAPI StrCmpLogicalW_Hook(PCWSTR psz1, PCWSTR psz2) {
    DWORD err = GetLastError();

    if (psz1 && psz2 && (!g_onlyChineseLocale || g_userLocaleIsChinese) &&
        GuardEnter()) {
        int len1 = lstrlenW(psz1);
        int len2 = lstrlenW(psz2);

        NativeCtx ctx = {API_LOGICAL, nullptr, 0, 0, nullptr, nullptr, 0};
        bool handled = false;
        SetLastError(err);
        int r = cls::Compare(psz1, len1, psz2, len2, NativeSeg, &ctx, &handled);
        if (handled) {
            GuardLeave();
            SetLastError(err);
            return r;
        }
        SetLastError(err);
        int nr = StrCmpLogicalW_Original(psz1, psz2);
        GuardLeave();
        return nr;
    }

    SetLastError(err);
    return StrCmpLogicalW_Original(psz1, psz2);
}

BOOL Wh_ModInit() {
    cls::g_num = Wh_GetIntSetting(L"enable_number") != 0;
    cls::g_tg = Wh_GetIntSetting(L"enable_tiangan") != 0;
    cls::g_dz = Wh_GetIntSetting(L"enable_dizhi") != 0;
    cls::g_gz = Wh_GetIntSetting(L"enable_ganzhi") != 0;
    cls::g_szx = Wh_GetIntSetting(L"enable_szx") != 0;
    g_hookCompareString = Wh_GetIntSetting(L"hook_comparestring") != 0;
    g_onlyChineseLocale = Wh_GetIntSetting(L"only_chinese_locale") != 0;

    wchar_t userLocale[LOCALE_NAME_MAX_LENGTH] = {0};
    if (GetUserDefaultLocaleName(userLocale, LOCALE_NAME_MAX_LENGTH) > 0) {
        g_userLocaleIsChinese = (userLocale[0] == L'z' || userLocale[0] == L'Z') &&
                                (userLocale[1] == L'h' || userLocale[1] == L'H') &&
                                (userLocale[2] == 0 || userLocale[2] == L'-');
    }
    Wh_Log(L"user locale=%s chinese=%d", userLocale, (int)g_userLocaleIsChinese);

    g_tlsDepth = TlsAlloc();
    if (g_tlsDepth == TLS_OUT_OF_INDEXES) {
        Wh_Log(L"TlsAlloc failed, running without reentrancy guard");
    }

    int hooked = 0;

    if (g_hookCompareString) {
        HMODULE hBase = GetModuleHandleW(L"kernelbase.dll");
        if (!hBase) hBase = GetModuleHandleW(L"kernel32.dll");
        if (!hBase) {
            Wh_Log(L"kernelbase.dll/kernel32.dll not found");
        } else {
            auto pEx = (CompareStringEx_t)GetProcAddress(hBase, "CompareStringEx");
            if (!pEx) {
                Wh_Log(L"CompareStringEx export not found");
            } else if (!SetHookT(pEx, CompareStringEx_Hook,
                                 &CompareStringEx_Original)) {
                Wh_Log(L"hook CompareStringEx failed");
            } else {
                hooked++;
            }

            auto pW = (CompareStringW_t)GetProcAddress(hBase, "CompareStringW");
            if (!pW) {
                Wh_Log(L"CompareStringW export not found");
            } else if (!SetHookT(pW, CompareStringW_Hook,
                                 &CompareStringW_Original)) {
                Wh_Log(L"hook CompareStringW failed");
            } else {
                hooked++;
            }
        }
    }

    HMODULE hShlwapi = GetModuleHandleW(L"shlwapi.dll");
    if (!hShlwapi) {
        hShlwapi = LoadLibraryExW(L"shlwapi.dll", nullptr,
                                  LOAD_LIBRARY_SEARCH_SYSTEM32);
    }
    if (!hShlwapi) {
        Wh_Log(L"shlwapi.dll not available");
    } else {
        auto pLog = (StrCmpLogicalW_t)GetProcAddress(hShlwapi, "StrCmpLogicalW");
        if (!pLog) {
            Wh_Log(L"StrCmpLogicalW export not found");
        } else if (!SetHookT(pLog, StrCmpLogicalW_Hook,
                             &StrCmpLogicalW_Original)) {
            Wh_Log(L"hook StrCmpLogicalW failed");
        } else {
            hooked++;
        }
    }

    Wh_Log(L"installed %d hook(s)", hooked);
    return hooked > 0;
}

void Wh_ModUninit() {
    Wh_Log(L"Unloading Chinese Logical Sort mod");
}