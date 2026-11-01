// ==WindhawkMod==
// @id              touch-keyboard-chinese-t9
// @name            系统触摸键盘中文九宫格
// @description     在 Windows 11 系统触摸键盘中完整接通微软拼音原生十二键/九宫格输入法。
// @version         28.0.8
// @author          NekoMirra
// @github          https://github.com/NekoMirra
// @include         TextInputHost.exe
// @include         ctfmon.exe
// @include         ChsIME.exe
// @architecture    arm64
// @architecture    x86-64
// @compilerOptions -lruntimeobject -lbcrypt
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# 系统触摸键盘中文九宫格 (Windows 11 Touch Keyboard Chinese T9 / 12-Key)

在 Windows 11 系统触摸键盘中重新启用并完整接通原生微软拼音的十二键/九宫格（narrow12key）输入法。

## 鍔熻兘鐗规€?
- **鍘熺敓涔濆鏍兼嫾闊宠緭鍏?*锛氱煭鎸夋寜閿甯镐骇鐢熶腑鏂囨眽瀛楀€欓€夎瘝銆?
- **闀挎寜绾暟瀛楀€欓€?*锛氶暱鎸変節閿寜閿椂浠呰緭鍑鸿鏁板瓧鍊欓€夛紝鏉炬墜涓嶄骇鐢熶簩娆¤緭鍏ャ€?
- **数字面板直接输入**：切换到数字键盘时，数字直接上屏，不再弹出多余的拼音候选词。
- **原生表情符号面板唤起**：点击表情按钮及更多符号直接呼出与输入面板上方候选栏同款的原生 Windows 11 表情符号面板（支持表情、颜文字与全 Unicode 符号），彻底避免跳转到空白占位界面。
- **稳定性与防闪退**：内置多层防御，自动拦截 Windows 11 在渲染狭窄九键及符号多页布局时的空指针与内部未适配断言，运行稳定流畅。

## 使用方法
1. 瀹夎骞跺惎鐢ㄦ湰妯＄粍锛?
2. 纭繚褰撳墠杈撳叆娉曚负 **涓枃锛堢畝浣擄級- 寰蒋鎷奸煶**锛?
3. 从任务栏唤起系统触摸键盘；
4. 鐐瑰嚮閿洏宸︿笂瑙掔殑榻胯疆璁剧疆鑿滃崟 -> 閿洏甯冨眬 -> 閫夋嫨 **銆屽崄浜岄敭銆?* 鍗冲彲銆?
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- enabled: true
  $name: 启用原生中文九宫格
*/
// ==/WindhawkModSettings==
#include <windhawk_utils.h>
#include <stdio.h>
#include <stdarg.h>
static void TraceLog(const wchar_t* fmt, ...) {
    wchar_t buffer[512]{};
    va_list ap; va_start(ap, fmt);
    _vsnwprintf_s(buffer, _TRUNCATE, fmt, ap);
    va_end(ap);
    Wh_Log(L"%s", buffer);
}

#include <windows.h>
#include <intrin.h>
#include <winstring.h>
#include <atomic>
#include <string>
#include <vector>

namespace {
std::atomic<bool> g_enabled{true};
HMODULE ownedDecoder;
}  // namespace
#include <bcrypt.h>
#include <cstddef>
#include <cstring>

namespace NativeT9 {
struct NativeString {
    union {
        const wchar_t* pointer;
        wchar_t local[8];
    } storage;
    size_t length;
    size_t capacity;
    const wchar_t* data() const {
        return capacity < 8 ? storage.local : storage.pointer;
    }
};
static_assert(sizeof(NativeString) == 32);
using Regularize = HRESULT (*)(void*, void*);
using DecideMode = HRESULT (*)(void*, WORD);
using Candidates = HRESULT (*)(void*, void*, void**);
using NativeInsert = HRESULT (*)(void*, const void*);
using ProcessString = HRESULT (*)(void*, const void*);
using ChannelHandle = HRESULT (*)(void*, const void*, bool*, bool*);
using VKeyHandle = HRESULT (*)(void*, const void*, bool*, bool*);
using ChcOpic = HRESULT (*)(void*, int);
using RequestSuggestion = HRESULT (*)(void*, const wchar_t*, unsigned int, void*, void*, bool);
using LayoutChange = HRESULT (*)(void*, const NativeString*);
using EnsureChannelFn = HRESULT (*)(void*, size_t);
using DefaultChannel = HRESULT (*)(void*, bool, size_t*);
using OnKeyDownFn = HRESULT (*)(void*, void*, void*);
Regularize originalRegularize;
DecideMode originalDecide;
Candidates originalCandidates;
NativeInsert originalInsert;
ProcessString originalProcess;
ChannelHandle originalChannelHandle;
VKeyHandle originalVKeyHandle;
ChcOpic originalChcOpic;
RequestSuggestion originalRequestSuggestion;
LayoutChange originalLayout;
EnsureChannelFn originalEnsureChannel;
DefaultChannel originalDefaultChannel;
OnKeyDownFn originalOnKeyDown;
std::atomic<unsigned> serviceKeyDownCount{0};
std::atomic<unsigned> regEntryCount{0};
BYTE* qwertyProfile;
BYTE* twelveProfile;
void* pinyinVtable;
std::atomic<unsigned> normalized{0};
std::atomic<unsigned> processCount{0};
std::atomic<unsigned> channelCount{0};
std::atomic<unsigned> vkeyCount{0};
std::atomic<unsigned> insertedCount{0};
std::atomic<unsigned> chcOpicCount{0};
std::atomic<unsigned> rsbsCount{0};
std::atomic<unsigned> candCallCount{0};
std::atomic<unsigned> decideCallCount{0};
std::atomic<unsigned> layoutCount{0};
std::atomic<unsigned> decoded{0};
thread_local bool twelveQuery = false;

// MTF 对单个解码对象串行查询；不同对象的并发查询用 TLS 隔离来源。
struct QueryScope {
    explicit QueryScope(BYTE* data, bool isTwelve)
        : previousQuery(twelveQuery), pData(data), previousMode(0), modified(false) {
        twelveQuery = isTwelve;
        if (isTwelve && pData) {
            auto* pMode = reinterpret_cast<DWORD*>(pData + 304712);
            previousMode = *pMode;
            if (previousMode == 0) {
                *pMode = 4;
                // removed corrupting pData+32 write
                modified = true;
            }
        }
    }
    ~QueryScope() {
        twelveQuery = previousQuery;
        if (modified && pData) {
            *reinterpret_cast<DWORD*>(pData + 304712) = previousMode;
                // removed corrupting pData+32 write
        }
    }
    bool previousQuery;
    BYTE* pData;
    DWORD previousMode;
    bool modified;
};
struct QueryProperties {
    DWORD type;
    DWORD flags;
    DWORD maximum;
    DWORD mode;
    DWORD reserved[20];
};

bool CodeMatches(const void* function, ULONG size, const char* expected) {
    BCRYPT_ALG_HANDLE algorithm = nullptr;
    if (BCryptOpenAlgorithmProvider(&algorithm, BCRYPT_SHA256_ALGORITHM,
                                    nullptr, 0) < 0)
        return false;
    BYTE digest[32];
    const NTSTATUS status =
        BCryptHash(algorithm, nullptr, 0,
                   const_cast<PUCHAR>(static_cast<const BYTE*>(function)), size,
                   digest, sizeof(digest));
    BCryptCloseAlgorithmProvider(algorithm, 0);
    if (status < 0)
        return false;
    constexpr char digits[] = "0123456789abcdef";
    char actual[65]{};
    for (size_t index = 0; index < sizeof(digest); ++index) {
        actual[index * 2] = digits[digest[index] >> 4];
        actual[index * 2 + 1] = digits[digest[index] & 15];
    }
    return strcmp(actual, expected) == 0;
}
// 触摸键盘 12 键的两个面板使用不同的键码体系，据此判别面板来源：
//   拼音面板(Alpha)  : 宫格键对应 VK_NUMPAD0~VK_NUMPAD9 (0x60..0x69)
//   数字面板(Symbols): 宫格键对应 ASCII '0'~'9' (0x30..0x39)
// 面板来源以 SIP 标志、当前输入通道与视图号共同判定。
bool IsNumpadKeyCode(WORD key) {
    return key >= VK_NUMPAD0 && key <= VK_NUMPAD9;
}
// 浠呯敤浜?CandidatesHook锛氳鍥惧彿鐢?SetViewInternal 鐪熷疄鍐欏叆锛?=鎷奸煶, 31=鏁板瓧锛?
bool IsCurrentViewTwelveAlpha() {
    WCHAR frontMode[64]{};
    const bool isTwelve = Wh_GetStringValue(L"native-front-mode", frontMode, ARRAYSIZE(frontMode)) &&
                          wcsstr(frontMode, L"12key") != nullptr;
    if (!isTwelve)
        return false;
    const int viewName = Wh_GetIntValue(L"native-front-view-name", -1);
    return viewName == 1;
}

// self 属于当前同步文档回调，不缓存跨文档指针。物理输入保持原生返回值。
HRESULT OnKeyDownHook(void* self, void* context, void* args) {
    const unsigned count = ++serviceKeyDownCount;
    return originalOnKeyDown(self, context, args);
}

HRESULT RegularizeHook(void* self, void* event) {
    const unsigned rCount = ++regEntryCount;
    if (!g_enabled.load() || !self || !event)
        return originalRegularize(self, event);

    auto* data = static_cast<BYTE*>(event);
    auto* text = reinterpret_cast<NativeString*>(event);
    const WORD flags = *reinterpret_cast<WORD*>(data + 34);
    size_t channel = *reinterpret_cast<size_t*>(static_cast<BYTE*>(self) + 112);
    auto* key = reinterpret_cast<WORD*>(data + 92);
    const WORD origKey = *key;


    const bool isHardware = (flags & 0x4010) == 0x10 && (flags & 5) != 0 && (flags & 5) != 5;
    if (isHardware) {
        Wh_SetIntValue(L"native-panel-mode", 2);
        return originalRegularize(self, event);
    }
    const HRESULT status = originalRegularize(self, event);
    if (FAILED(status))
        return status;
    const bool grid = (flags & 0x4000) != 0 && IsCurrentViewTwelveAlpha() && origKey >= L'2' && origKey <= L'9';
    Wh_SetIntValue(L"native-panel-mode", grid ? 1 : 0);
    if (grid) {
        // 褰撳墠 ARM64 RegularizeStringKey 涓?InsertVKey 鍧囩‘璁?+92 涓虹嫭绔?WORD 閿爜銆?
        *key = static_cast<WORD>(VK_NUMPAD0 + origKey - L'0');
    }
    TraceLog(L"REG sip view=%d key=0x%04X normalized=0x%04X flags=0x%04X ch=%llu",
             Wh_GetIntValue(L"native-front-view-name", -1), origKey, *key, flags, channel);

    return status;

}

HRESULT DefaultChannelHook(void* self, bool off, size_t* channel) {
    const HRESULT res = originalDefaultChannel(self, off, channel);
    if (!off && channel) {
        const bool isTwelveAlpha = IsCurrentViewTwelveAlpha();
        if (isTwelveAlpha) {
            *channel = 4;
            *reinterpret_cast<DWORD*>(static_cast<BYTE*>(self) + 164) = 4;
        }
    }
    return res;
}

HRESULT LayoutHook(void* self, const NativeString* layout) {
    // 布局名是后续通道判定的唯一来源，属于必要状态而非逐键遥测。
    if (layout && layout->length > 0 && layout->length < 100)
        Wh_SetStringValue(L"native-layout-name", layout->data());
    const HRESULT res = originalLayout(self, layout);
    if (FAILED(res))
        return res;
    size_t target = 4;
    if (!IsCurrentViewTwelveAlpha()) {
        const HRESULT status = originalDefaultChannel(self, false, &target);
        if (FAILED(status))
            return status;
    }
    const size_t current = *reinterpret_cast<size_t*>(static_cast<BYTE*>(self) + 112);
    if (current != target) {
        const HRESULT status = originalEnsureChannel(self, target);
        if (FAILED(status))
            return status;
    }
    const size_t channel = *reinterpret_cast<size_t*>(static_cast<BYTE*>(self) + 112);
    return res;
}

HRESULT ProcessStringHook(void* self, const void* event) {
    void* pinyinBase = static_cast<BYTE*>(self) - 136;
    size_t channel = *reinterpret_cast<size_t*>(static_cast<BYTE*>(pinyinBase) + 112);
    auto* data = static_cast<const BYTE*>(event);
    const WORD flags = *reinterpret_cast<const WORD*>(data + 34);
    const bool hardware = (flags & 0x4010) == 0x10 && (flags & 5) != 0 && (flags & 5) != 5;
    const bool isTwelveAlpha = IsCurrentViewTwelveAlpha();
    if (!hardware && isTwelveAlpha && channel != 4 && originalEnsureChannel) {
        originalEnsureChannel(pinyinBase, 4);
        *reinterpret_cast<DWORD*>(static_cast<BYTE*>(pinyinBase) + 164) = 4;
        channel = 4;
    } else if (!hardware && !isTwelveAlpha && channel == 4 && originalEnsureChannel) {
        size_t target = 0;
        const HRESULT status = originalDefaultChannel(pinyinBase, false, &target);
        if (FAILED(status))
            return status;
        const HRESULT ensured = originalEnsureChannel(pinyinBase, target);
        if (FAILED(ensured))
            return ensured;
        channel = target;
    }
    return originalProcess(self, event);
}

HRESULT ChannelHandleHook(void* self, const void* event, bool* a3, bool* a4) {
    return originalChannelHandle(self, event, a3, a4);
}

HRESULT VKeyHandleHook(void* self, const void* event, bool* a3, bool* a4) {
    return originalVKeyHandle(self, event, a3, a4);
}

HRESULT NativeInsertHook(void* self, const void* event) {
    return originalInsert(self, event);
}

HRESULT ChcOpicHook(void* self, int mask) {
    return originalChcOpic(self, mask);
}

HRESULT RequestSuggestionHook(void* self, const wchar_t* text, unsigned int len, void* propBag, void* handler, bool b) {
    return originalRequestSuggestion(self, text, len, propBag, handler, b);
}

HRESULT CandidatesHook(void* self, void* query, void** list) {
    if (!g_enabled.load() || !query || !list)
        return originalCandidates(self, query, list);
    using GetProperties = HRESULT(WINAPI*)(void*, QueryProperties*);
    QueryProperties properties{};
    auto methods = *static_cast<void***>(query);
    const HRESULT status =
        reinterpret_cast<GetProperties>(methods[3])(query, &properties);
    if (FAILED(status))
        return status;
    const bool isTwelveAlpha = IsCurrentViewTwelveAlpha();
    const bool isTwelve = isTwelveAlpha &&
                          ((properties.type & 0xFF) == 11 || properties.type == 11 || properties.type == 2 || properties.type == 0);
    QueryScope scope(static_cast<BYTE*>(self), isTwelve);
    if (isTwelve) {
    }
    const HRESULT candStatus = originalCandidates(self, query, list);
    if (SUCCEEDED(candStatus) && list && *list) {
        using GetCountFn = HRESULT (*)(void*, int*);
        int cCount = 0;
        void** vtbl = *reinterpret_cast<void***>(*list);
        if (vtbl) {
            GetCountFn getCount = reinterpret_cast<GetCountFn>(vtbl[3]);
            if (SUCCEEDED(getCount(*list, &cCount))) {
            }
        }
    }
    return candStatus;
}

HRESULT DecideHook(void* self, WORD first) {

    // 面板来源由 RegularizeHook 记录：1=拼音面板 0=数字面板 2=物理键盘
    const int panelMode = Wh_GetIntValue(L"native-panel-mode", 2);
    if (!g_enabled.load() || panelMode != 1)
        return originalDecide(self, first);

    // 拼音面板：把宫格数字统一归一为十二键键码
    WORD digit = 0;
    if (first >= VK_NUMPAD2 && first <= VK_NUMPAD9)
        digit = static_cast<WORD>(first - VK_NUMPAD0);
    else if (first >= '2' && first <= '9')
        digit = static_cast<WORD>(first - '0');

    if (digit < 2 || digit > 9)
        return originalDecide(self, first);

    auto* data = static_cast<BYTE*>(self);
    auto* manager = *reinterpret_cast<BYTE**>(data + 57616);
    auto* reading = *reinterpret_cast<BYTE**>(data + 22152);

    if (manager)
        *reinterpret_cast<BYTE**>(manager) = twelveProfile;
    if (reading) {
        *reinterpret_cast<BYTE**>(data + 57608) = twelveProfile;
        *reinterpret_cast<int*>(reading + 4396) = 2;
        *reinterpret_cast<BYTE**>(reading + 8064) = twelveProfile + 28;
        *reinterpret_cast<BYTE**>(reading + 8072) = twelveProfile + 540;
    }

    TraceLog(L"DEC twelveProfile LOADED for digit=%u", digit);
    // 不能调用原生：DecideInputMode 收到 '2'..'9' 时会把 profile 强制写回 Qwerty，
    // 浠庤€屾竻鎺夊垰瑁呬笂鐨?twelveProfile锛屽鑷村€欓€夋暟涓?0銆?
    return 0;
}

WH_HOOK_SYMBOLS_OPTIONS SymbolOptions() {
    WH_HOOK_SYMBOLS_OPTIONS options{};
    options.optionsSize = sizeof(options);
    options.noUndecoratedSymbols = TRUE;
    return options;
}

bool InstallService(HMODULE module) {
    WindhawkUtils::SYMBOL_HOOK inputserviceDllHooks[] = {
        {{L"?RegularizeStringKey@InputMethodBase@ChxIME@@IEBAJPEAUSTRING_KEY_"
          L"EVENT@2@@Z"},
         &originalRegularize},
        {{L"??_7InputMethodPinyin@ChxIME@@6BInputMethodBase@1@@"},
         &pinyinVtable},
        {{L"?GetDefaultInputChannelID@InputMethodPinyin@ChxIME@@MEBAJ_NPEA_K@Z"},
         &originalDefaultChannel},
        {{L"?EnsureInputChannel@InputMethodBase@ChxIME@@IEAAJ_K@Z"},
         &originalEnsureChannel},
        {{L"?OnSipLayoutChanged@InputMethodPinyin@ChxIME@@IEAAJAEBV?$basic_string@GU?$char_traits@G@std@@V?$allocator@G@2@@std@@@Z"},
         &originalLayout},
        {{L"?ProcessStringKey@InputMethodPinyin@ChxIME@@UEAAJAEBUSTRING_KEY_EVENT@2@@Z"},
         &originalProcess},
        {{L"?HandleStringKey@InputChannel12Key@ChxIME@@UEAAJPEBUSTRING_KEY_EVENT@2@PEA_N1@Z"},
         &originalChannelHandle},
        {{L"?HandleStringKey@VKeyHandler12Key@ChxIME@@UEAAJPEBUSTRING_KEY_EVENT@2@PEA_N1@Z"},
         &originalVKeyHandle},
        {{L"?InsertVKey@CompositionEditor12Key@ChxIME@@UEAAJPEBUSTRING_KEY_EVENT@2@@Z"},
         &originalInsert},
        {{L"?OnProductItemChanged@CandidateHandlerConversion12Key@ChxIME@@UEAAJK@Z"},
         &originalChcOpic},
        {{L"?RequestSuggestionByString@MtfQueryHandlerBase@ChxIME@@IEAAJPEBGIPEAUIMtfPropertyBag@@PEAUIMtfQueryHandler2@@_N@Z"},
         &originalRequestSuggestion},
        {{L"?OnKeyDown@InputMethodHostTSF3@ChxIME@@AEAAJPEAUICoreTextInputMethodContext@Core@Text@UI@Windows@@PEAUICoreTextKeyEventArgs@4567@@Z"},
         &originalOnKeyDown},
    };
    auto options = SymbolOptions();
    if (!WindhawkUtils::HookSymbols(module, inputserviceDllHooks,
                                    ARRAYSIZE(inputserviceDllHooks), &options))
        return false;
    // 缺少任一必需符号时不再安装依赖其对象布局的钩子。
    if (!originalRegularize || !originalProcess || !originalDefaultChannel ||
        !originalEnsureChannel || !originalLayout)
        return false;
#if defined(__x86_64__) || defined(_M_X64)
    if (!CodeMatches(
            reinterpret_cast<void*>(originalRegularize), 0x213,
            "a5eaa8b59796a8181280a5a79489a74a75994357a6d140908a5e0a4f23026278"))
        return false;
    if (!CodeMatches(
            reinterpret_cast<void*>(originalDefaultChannel), 0x19,
            "16d5d34487009bd4cdb1ba45b24e836fa16a2a1f13b0df0243a3dab3fc4ecd04"))
        return false;
    if (!CodeMatches(
            reinterpret_cast<void*>(originalEnsureChannel), 0xd6,
            "b9d0088110763a66322abf222345bfe69aa091c7f0528f28999c04b4b93bbef6"))
        return false;
    if (!CodeMatches(
            reinterpret_cast<void*>(originalLayout), 0x34c,
            "ec573c7d1a87d2db4ef6a2d221e192a70abcf2b48e6ef00264b48a4cf706c50a"))
        return false;
    if (!CodeMatches(
            reinterpret_cast<void*>(originalProcess), 0x53a,
            "dc0dbc04caa88cb2be63002c79031a848d421aff39f8007fb3db8b0ef0781d48"))
        return false;
    if (!CodeMatches(
            reinterpret_cast<void*>(originalChannelHandle), 0x48,
            "196fc03630c4fabddbf13ee6c2c9e43098101d6d3525da9e4c5bf2c7f21fa248"))
        return false;
    if (!CodeMatches(
            reinterpret_cast<void*>(originalVKeyHandle), 0x3a,
            "9417444f3f25bab271d6d782fcf16b9e2b7e72dc5b16ca85f0cb633194361b89"))
        return false;
    if (!CodeMatches(
            reinterpret_cast<void*>(originalInsert), 0xdf,
            "a06ee02a5f0c7ad06b08fa24f6c28b38e68b1d7be15ffa974404a73036c15106"))
        return false;
    if (!CodeMatches(
            reinterpret_cast<void*>(originalChcOpic), 0x368,
            "926095fefb60087d497fb2fa27fda49a11828713576c83e7872acee2d3e417f4"))
        return false;
    if (!CodeMatches(
            reinterpret_cast<void*>(originalRequestSuggestion), 0x3b7,
            "7d4d2a14deffbe6578d15ae5c069bcdd40905de8da11e938e9967956cd418ece"))
        return false;
    if (!CodeMatches(
            reinterpret_cast<void*>(originalOnKeyDown), 0x6b6,
            "91e1bf21e466cf3a9f53d578c490214f74abada736e90f0939ace40b9af7db7c"))
        return false;
#endif
    return WindhawkUtils::SetFunctionHook(originalRegularize, RegularizeHook,
                                          &originalRegularize) &&
           WindhawkUtils::SetFunctionHook(originalProcess, ProcessStringHook,
                                          &originalProcess) &&
           WindhawkUtils::SetFunctionHook(originalChannelHandle, ChannelHandleHook,
                                          &originalChannelHandle) &&
           WindhawkUtils::SetFunctionHook(originalVKeyHandle, VKeyHandleHook,
                                          &originalVKeyHandle) &&
           WindhawkUtils::SetFunctionHook(originalInsert, NativeInsertHook,
                                          &originalInsert) &&
           WindhawkUtils::SetFunctionHook(originalChcOpic, ChcOpicHook,
                                          &originalChcOpic) &&
           WindhawkUtils::SetFunctionHook(originalRequestSuggestion, RequestSuggestionHook,
                                          &originalRequestSuggestion) &&
           WindhawkUtils::SetFunctionHook(originalOnKeyDown, OnKeyDownHook,
                                          &originalOnKeyDown);
}


bool InstallDecoder(HMODULE module) {
    WindhawkUtils::SYMBOL_HOOK chspinyindsDllHooks[] = {
        {{L"?GetCandidates@ChsPinyinDS@@UEAAJPEAUIMtfSuggestionInputQuery@@"
          L"PEAPEAUIMtfSuggestionList@@@Z"},
         &originalCandidates},
        {{L"?DecideInputMode@ChsPinyinDS@@AEAAJG@Z"}, &originalDecide},
        {{L"?s_InputModeProfileQwerty@InputModeProfileManager@TaihangCore@@"
          L"0UInputModeProfile@2@A"},
         &qwertyProfile},
        {{L"?s_InputModeProfile12Key@InputModeProfileManager@TaihangCore@@"
          L"0UInputModeProfile@2@A"},
         &twelveProfile},
    };
    auto options = SymbolOptions();
    if (!WindhawkUtils::HookSymbols(module, chspinyindsDllHooks, ARRAYSIZE(chspinyindsDllHooks),
                                    &options))
        return false;
#if defined(__x86_64__) || defined(_M_X64)
    if (!CodeMatches(
            reinterpret_cast<void*>(originalDecide), 0x1f3,
            "d2d9038ce0d8be4c4019eeb741bda43aece66f8e98de847011f10a8f63adc46e"))
        return false;
    if (!CodeMatches(
            reinterpret_cast<void*>(originalCandidates), 0x2ed3,
            "7e0b8b1dc419ec319a1bb3275cbc2e2735e21332a238cef6b25959c7dfcc0677"))
        return false;
#endif
    return WindhawkUtils::SetFunctionHook(originalCandidates, CandidatesHook,
                                          &originalCandidates) &&
           WindhawkUtils::SetFunctionHook(originalDecide, DecideHook,
                                          &originalDecide);
}
}  // namespace NativeT9


namespace {
using Availability = bool (*)(void*, HSTRING);
using IsAvailableOnModeSwitcherFn = bool (*)(void*, HSTRING);
IsAvailableOnModeSwitcherFn originalIsAvailableOnModeSwitcher;
using ModeView = void (*)(void*, HSTRING, int);
using OnPressActionFn = void (*)(void*, void*);
using OnPressAndHoldActionFn = void (*)(void*, void*);
using OnReleaseActionFn = void (*)(void*, void*);
using HandleFlowerKeyReleaseFn = void (*)(void*, void*);
using InjectKeyWithoutKeyControlFn = void (*)(void*, void*, void*, unsigned int);
struct InputPoint { float x; float y; };
using InjectKeyFn = void (*)(void*, void*, HSTRING, InputPoint, unsigned __int64, int);
using GetDirectionFn = int (*)(void*);
using GetHeightFn = int (*)(void*);
using IsDictionaryReadyFn = bool (*)(void*, void*);
using NotifyUIMessageFn = void (*)(void*, int, int, int, int, void*);
using GetKeyDataFn = void* (*)(void*);
using GetVirtualKeyFn = unsigned char (*)(void*);
using GetStateFn = int (*)(void*, void*);
using AcquireFn = void* (*)(HSTRING, int, void*, int, int, unsigned __int64, void*);
using NotifyStringKeyFn = void (*)(void*, unsigned int, unsigned int, HSTRING, int, int, unsigned int);
using GetBlockPageCountFn = unsigned int (*)(void*, HSTRING);
using InitializeGroupModelFn = void (*)(void*, void*, void*, void*, void*, void*, void*, void*, void*, char, int, void*, void*, void*);
using IsSubTypeFn = bool (*)(void*, int);
using GetRecognizedTextFn = HSTRING (*)(void*);
using GetSecondaryLabelFontScalingFn = float (*)(void*);
using IsEmojiViewEnabledFn = bool (*)(void*);

Availability originalAvailability;
ModeView originalMode;
OnPressActionFn originalOnPressAction;
OnPressAndHoldActionFn originalOnPressAndHoldAction;
OnReleaseActionFn originalOnReleaseAction;
HandleFlowerKeyReleaseFn originalHandleFlowerKeyRelease;
InjectKeyWithoutKeyControlFn originalInjectKeyWithoutKeyControl;
InjectKeyFn originalInjectKey;
GetDirectionFn originalGetDirection;
GetHeightFn originalGetHeight;
IsDictionaryReadyFn originalIsDictionaryReady;
NotifyUIMessageFn originalNotifyUIMessage;
GetKeyDataFn originalGetKeyData;
GetVirtualKeyFn originalGetVirtualKey;
GetStateFn originalGetPressedKeyHandlingState;
using SetViewInternalFn = void (*)(void*, int, void*, bool);
SetViewInternalFn originalSetViewInternal;
void Hook_SetViewInternal(void* self, int viewName, void* layoutArgs, bool a4) {
    // 视图号用于区分拼音面板与数字/符号面板，是通道判定的必要状态。
    Wh_SetIntValue(L"native-front-view-name", viewName);
    TraceLog(L"VIEW switch %d self=%p", viewName, self);
    if (originalSetViewInternal)
        originalSetViewInternal(self, viewName, layoutArgs, a4);
}
AcquireFn originalAcquire;
NotifyStringKeyFn originalNotifyStringKey;
GetBlockPageCountFn originalGetBlockPageCount;
InitializeGroupModelFn originalInitializeGroupModel;
IsSubTypeFn originalIsSubType;
GetRecognizedTextFn originalGetRecognizedText;
GetSecondaryLabelFontScalingFn originalGetSecondaryLabelFontScaling;
IsEmojiViewEnabledFn originalIsEmojiViewEnabled;

bool frontProcess = false;
thread_local unsigned g_releaseDepth = 0;
struct ReleaseActionScope {
    ReleaseActionScope() { ++g_releaseDepth; }
    ~ReleaseActionScope() { if (g_releaseDepth > 0) --g_releaseDepth; }
};
std::atomic<unsigned> frontPressCount{0};
std::atomic<unsigned> frontReleaseCount{0};
std::atomic<unsigned> frontFlowerCount{0};
std::atomic<unsigned> frontInjectKcCount{0};
std::atomic<unsigned> frontInjectKeyCount{0};
std::atomic<unsigned> frontDirCount{0};
std::atomic<unsigned> frontHeightCount{0};
std::atomic<unsigned> frontVkGetCount{0};
std::atomic<unsigned> frontAcquireCount{0};
std::atomic<unsigned> frontDictReadyCount{0};
std::atomic<unsigned> frontNskCount{0};
std::atomic<unsigned> frontNotifyMsgCount{0};

bool IsTwelve(HSTRING mode) {
    if (!mode) return true;
    UINT32 length = 0;
    const wchar_t* text = WindowsGetStringRawBuffer(mode, &length);
    if (!text || length == 0) return true;
    return wcsstr(text, L"narrow") != nullptr || wcsstr(text, L"12key") != nullptr;
}

bool AvailabilityHook(void* self, HSTRING mode) {
    if (IsTwelve(mode)) {
        return true;
    }
    return originalAvailability(self, mode);
}

using IsAvailableStrategyFn = bool (*)(void*, HSTRING, void*, HSTRING, bool*);
using CanRenderModeFn = bool (*)(void*, HSTRING);
using IsModeAvailableInternalFn = bool (*)(void*, void*, HSTRING, bool*);
using SetUpMenuFn = void (*)(void* self);
using ShowAsDisabledFn = bool (*)(void* self);

IsAvailableStrategyFn originalIsAvailableStrategy = nullptr;
CanRenderModeFn originalCanRenderModeWide = nullptr;
CanRenderModeFn originalCanRenderModeCtrl = nullptr;
IsModeAvailableInternalFn originalIsModeAvailableInternal = nullptr;
SetUpMenuFn originalSetUpMenu = nullptr;
ShowAsDisabledFn originalShowAsDisabled = nullptr;

bool IsAvailableStrategyHook(void* self, HSTRING mode, void* sizeMgr, HSTRING a3, bool* outNarrow) {
    if (IsTwelve(mode)) {
        if (outNarrow) *outNarrow = true;
        return true;
    }
    return originalIsAvailableStrategy ? originalIsAvailableStrategy(self, mode, sizeMgr, a3, outNarrow) : true;
}

bool CanRenderModeWideHook(void* self, HSTRING mode) {
    if (IsTwelve(mode)) return true;
    return originalCanRenderModeWide ? originalCanRenderModeWide(self, mode) : true;
}

bool CanRenderModeCtrlHook(void* self, HSTRING mode) {
    if (IsTwelve(mode)) return true;
    return originalCanRenderModeCtrl ? originalCanRenderModeCtrl(self, mode) : true;
}

bool IsModeAvailableInternalHook(void* self, void* stackMode, HSTRING modeName, bool* isAvailable) {
    if (IsTwelve(modeName)) {
        if (isAvailable) *isAvailable = true;
        return true;
    }
    return originalIsModeAvailableInternal ? originalIsModeAvailableInternal(self, stackMode, modeName, isAvailable) : false;
}

bool ShowAsDisabledHook(void* self) {
    return false; // Force NOT disabled on mode switcher
}

void SetUpMenuHook(void* self) {
    if (self) {
        // Clear cached MenuFlyout pointer at self + 0x360 so it always regenerates fresh with 12-key!
        *reinterpret_cast<void**>(static_cast<BYTE*>(self) + 0x360) = nullptr;
    }
    if (originalSetUpMenu) originalSetUpMenu(self);
}

bool IsAvailableOnModeSwitcherHook(void* self, HSTRING mode) {
    if (IsTwelve(mode)) {
        return true;
    }
    return originalIsAvailableOnModeSwitcher(self, mode);
}


void ModeHook(void* self, HSTRING mode, int view) {
    Wh_SetStringValue(L"native-front-mode",
                      WindowsGetStringRawBuffer(mode, nullptr));
    originalMode(self, mode, view);
}

using GetItemTypeFn = int (*)(void*);
GetItemTypeFn originalGetItemType;
using GetKeyboardSetFn = void* (*)(void*);
GetKeyboardSetFn originalGetKeyboardSet;
int GetItemTypeHook(void* self) {
    const int v = originalGetItemType ? originalGetItemType(self) : -1;
    return v;
}

void OnReleaseActionHook(void* self, void* action) {
    const unsigned count = ++frontReleaseCount;
    int handledBefore = -1;
    if (action)
        handledBefore = static_cast<BYTE*>(action)[144];

    const int vn = Wh_GetIntValue(L"native-front-view-name", -1);
    // action 的对象引用由系统持有；观察路径不调用其他接口或修改引用计数。
    TraceLog(L"RELEASE action=%p handled=%d view=%d", action, handledBefore, vn);

    if (originalOnReleaseAction) {
        originalOnReleaseAction(self, action);
    }
    if (action) {
        const int handledAfter = static_cast<BYTE*>(action)[144];
        TraceLog(L"RELEASE after handled=%d", handledAfter);
    }
}

void OnPressActionHook(void* self, void* action) {
    TraceLog(L"PRESS action=%p", action);
    originalOnPressAction(self, action);
}
thread_local bool g_inPressAndHold = false;
void OnPressAndHoldActionHook(void* self, void* action) {
    g_inPressAndHold = true;
    originalOnPressAndHoldAction(self, action);
    g_inPressAndHold = false;
}


int DirectionHook(void* self) {
    if (!self)
        return 0;
    const int val = originalGetDirection(self);
    if (val >= 30) {
        return 0;
    }
    if (g_releaseDepth != 0) {
    }
    return val;
}

int HeightHook(void* self) {
    const int val = originalGetHeight(self);
    if (g_releaseDepth != 0) {
    }
    return val;
}
using LabelVisualFn = void (*)(void*, void*, HSTRING, HSTRING, double);
LabelVisualFn originalLabelVisual;
// 系统 UI 线程同步排版；只调整本次参数，不保存标签指针或修改对象引用。
void LabelVisualHook(void* self, void* label, HSTRING text, HSTRING font, double scale) {
    if (g_enabled.load() && self) {
        WCHAR layout[128]{};
        if (Wh_GetStringValue(L"native-layout-name", layout, ARRAYSIZE(layout)) &&
            wcsstr(layout, L"12key") != nullptr) {
            auto** labels = reinterpret_cast<void**>(static_cast<BYTE*>(self) + 696);
            // 仅缩短同时带副标签的宫格键主标签，避免影响数字与符号面板的单一标签。
            if (label == labels[0] && labels[1] != nullptr)
                scale *= 0.70;
            if (label == labels[1])
                scale *= 0.40;
        }
    }
    originalLabelVisual(self, label, text, font, scale);
}
bool IsEmojiViewEnabledHook(void* self) {
    const bool native = originalIsEmojiViewEnabled ? originalIsEmojiViewEnabled(self) : false;
    return native;
}
int GetPressedKeyHandlingStateHook(void* self, void* keyControl) {
    return originalGetPressedKeyHandlingState(self, keyControl);
}
HSTRING GetRecognizedTextHook(void* self) {
    return originalGetRecognizedText(self);
}

bool IsDictionaryReadyHook(void* self, void* str) {
    const bool res = originalIsDictionaryReady ? originalIsDictionaryReady(self, str) : false;
    TraceLog(L"DICTREADY res=%d", res ? 1 : 0);
    return res;
}

void NotifyUIMessageHook(void* self, int a2, int a3, int a4, int a5, void* a6) {
    if (g_releaseDepth != 0) {
    }
    originalNotifyUIMessage(self, a2, a3, a4, a5, a6);
}
void* KeyDataHook(void* self) {
    return originalGetKeyData(self);
}

unsigned char VirtualKeyHook(void* self) {
    const unsigned char val = originalGetVirtualKey(self);
    if (g_releaseDepth != 0) {
    }
    return val;
}
unsigned int GetBlockPageCountHook(void* self, HSTRING block) {
    if (!self) {
        return 0;
    }
    return originalGetBlockPageCount(self, block);
}
using AddKeySetBlockFn = void (*)(void*, void*, void*);
AddKeySetBlockFn originalAddKeySetBlock;
    // 符号与表情面板构建时 this+104 可能为空；原生入口对此直接 FailFast(0x8000FFFF)。
// 璇ョ粨鏋勪笌閿洏缃戞牸鍚岀嚎绋嬫瀯寤猴紝璋冪敤鏂规寔鏈?this 鐢熷懡鍛ㄦ湡銆?
void AddKeySetBlockHook(void* self, void* row, void* keyData) {
    if (self) {
        auto* data = static_cast<BYTE*>(self);
        void** keySet104 = reinterpret_cast<void**>(data + 104);
        void** keySet128 = reinterpret_cast<void**>(data + 128);
        TraceLog(L"KEYSET entry self=%p k104=%p k128=%p", self, *keySet104, *keySet128);
        // 鍘熺敓鍏ュ彛瀵?this+104 涓虹┖鐩存帴 FailFast(0x8000FFFF) 缁堟瀹夸富銆?
        // 扩展符号与表情面板构建时该键集未建立，跳过本次键块构建保持界面可用。
        if (*keySet104 == nullptr) {
            return;
        }
    }
    originalAddKeySetBlock(self, row, keyData);
}
void NotifyStringKeyHook(void* self, unsigned int qualifiers, unsigned int key, HSTRING text, int x, int y, unsigned int time) {
    const unsigned count = ++frontNskCount;
    WCHAR nbuf[8]{}; UINT32 nlen = 0;
    if (text) {
        const wchar_t* b = WindowsGetStringRawBuffer(text, &nlen);
        if (b && nlen > 0 && nlen < 8) wcsncpy(nbuf, b, nlen);
    }
    if (g_enabled.load() && NativeT9::IsCurrentViewTwelveAlpha() && nlen == 1 && nbuf[0] >= L'2' && nbuf[0] <= L'9' && key >= VK_NUMPAD2 && key <= VK_NUMPAD9) {
        key = static_cast<unsigned int>(nbuf[0]);
    }
    TraceLog(L"NOTIFYSTRKEY key=0x%04X qual=0x%X text='%ls' len=%u", key, qualifiers, nbuf, nlen);
    originalNotifyStringKey(self, qualifiers, key, text, x, y, time);
}



void HandleFlowerKeyReleaseHook(void* self, void* action) {
    const unsigned count = ++frontFlowerCount;
    if (action) {
        auto* data = static_cast<BYTE*>(action);
    }
    originalHandleFlowerKeyRelease(self, action);
}

void InjectKeyWithoutKeyControlHook(void* self, void* str, void* keyData, unsigned int a4) {
    const unsigned count = ++frontInjectKcCount;
    if (g_inPressAndHold && str) {
        auto hStr = reinterpret_cast<HSTRING>(str);
        UINT32 len = 0;
        const wchar_t* buf = WindowsGetStringRawBuffer(hStr, &len);
        if (buf && len == 1 && buf[0] >= L'2' && buf[0] <= L'9') {
            static std::atomic<unsigned int> s_longPressSeq{0};
        }
    }
    originalInjectKeyWithoutKeyControl(self, str, keyData, a4);
}

void InjectKeyHook(void* self, void* keyControl, HSTRING text, InputPoint point, unsigned __int64 a5, int flags) {
    const unsigned count = ++frontInjectKeyCount;
    WCHAR ibuf[8]{}; UINT32 ilen = 0;
    if (text) {
        const wchar_t* b = WindowsGetStringRawBuffer(text, &ilen);
        if (b && ilen > 0 && ilen < 8) wcsncpy(ibuf, b, ilen);
    }
    TraceLog(L"INJECTKEY text='%ls' len=%u flags=0x%X", ibuf, ilen, flags);

    originalInjectKey(self, keyControl, text, point, a5, flags);
}


using LoadLibraryExW_t = HMODULE (WINAPI *)(LPCWSTR, HANDLE, DWORD);
LoadLibraryExW_t original_LoadLibraryExW = nullptr;

bool InstallFrontend(HMODULE module);

HMODULE WINAPI Hook_LoadLibraryExW(LPCWSTR lpLibFileName, HANDLE hFile, DWORD dwFlags) {
    HMODULE hModule = original_LoadLibraryExW(lpLibFileName, hFile, dwFlags);
    if (hModule && lpLibFileName) {
        if (frontProcess) {
            if (wcsstr(lpLibFileName, L"TextInput.dll")) {
                InstallFrontend(hModule);
                Wh_ApplyHookOperations();
            }
        } else {
            if (wcsstr(lpLibFileName, L"InputService.dll")) {
                NativeT9::InstallService(hModule);
                Wh_ApplyHookOperations();
            } else if (wcsstr(lpLibFileName, L"ChsPinyinDS.dll")) {
                NativeT9::InstallDecoder(hModule);
                Wh_ApplyHookOperations();
            }
        }
    }
    return hModule;
}

bool InstallFrontend(HMODULE module) {
    auto options = NativeT9::SymbolOptions();
    // TextInput.dll
    WindhawkUtils::SYMBOL_HOOK textinputDllHooks[] = {
        {{L"?IsNarrowModeAvailableInternal@"
          L"ChineseSimplifiedModeAvailabilityStrategy@Controls@"
          L"TextInputUndocked@Experiences@ComposableShell@WindowsInternal@@"
          L"AEAA_NPE$AAVString@Platform@@@Z"},
         &originalAvailability,
         AvailabilityHook},
        {{L"?SetKeyboardModeView@?QITextFrameworkAdapter@Common@TextInput@"
          L"Experiences@ComposableShell@WindowsInternal@@"
          L"SystemInputProcessorTextFrameworkAdapter@TextFramework@"
          L"TextInputUndocked@456@UE$AAAXPE$AAVString@Platform@@W4ViewName@"
          L"LayoutData@23456@@Z"},
         &originalMode,
         ModeHook},
        {{L"?IsAvailableOnModeSwitcher@?QIInputPanelTemplateManager@TouchKeyboardUX@@"
          L"InputPanelTemplateManager@Controls@TextInputUndocked@Experiences@"
          L"ComposableShell@WindowsInternal@@UE$AAA_NPE$AAVString@Platform@@@Z"},
         &originalIsAvailableOnModeSwitcher,
         IsAvailableOnModeSwitcherHook},
        {{L"?IsAvailable@"
          L"ChineseSimplifiedModeAvailabilityStrategy@Controls@"
          L"TextInputUndocked@Experiences@ComposableShell@WindowsInternal@@"
          L"UEAA_NPE$AAVString@Platform@@PE$AAUIInputPanelSizePositionManager@TouchKeyboardUX@@0PEA_N@Z"},
         &originalIsAvailableStrategy,
         IsAvailableStrategyHook},
        {{L"?CanRenderMode@"
          L"WideNarrowSizePositionManager@Controls@TextInputUndocked@"
          L"Experiences@ComposableShell@WindowsInternal@@UEAA_NPE$AAVString@Platform@@@Z"},
         &originalCanRenderModeWide,
         CanRenderModeWideHook},
        {{L"?CanRenderMode@"
          L"ControllerModernUXSizePositionManager@Controls@TextInputUndocked@"
          L"Experiences@ComposableShell@WindowsInternal@@UEAA_NPE$AAVString@Platform@@@Z"},
         &originalCanRenderModeCtrl,
         CanRenderModeCtrlHook},
        {{L"?IsModeAvailableInternal@"
          L"ModeAvailabilityManager@Controls@TextInputUndocked@Experiences@ComposableShell@WindowsInternal@@"
          L"AE$AAA_NPE$AAUIKeyboardStackMode@LayoutData@Common@TextInput@456@PE$AAVString@Platform@@PEA_N@Z"},
         &originalIsModeAvailableInternal,
         IsModeAvailableInternalHook},
        {{L"?get@ShowAsDisabledOnModeSwitcher@IAvailableModeInfo@TouchKeyboardUX@@UE$AAA_NXZ"},
         &originalShowAsDisabled,
         ShowAsDisabledHook},
        {{L"?SetUpKeyboardSettingsMenu@InputPanelControl@Controls@TextInputUndocked@Experiences@ComposableShell@WindowsInternal@@AE$AAAXXZ"},
         &originalSetUpMenu,
         SetUpMenuHook},
        {{L"?get@?QIInputPanelModel@TouchKeyboardUX@@IsEmojiViewEnabled@InputPanelModel@"
          L"Controls@TextInputUndocked@Experiences@ComposableShell@WindowsInternal@@UE$AAA_NXZ"},
         &originalIsEmojiViewEnabled,
         IsEmojiViewEnabledHook},
        // Crash prevention hooks:
        {{L"?AddKeySetKeyBlock@KeyboardGridGroupModel@Controls@TextInputUndocked@Experiences@"
          L"ComposableShell@WindowsInternal@@AE$AAAXPE$AAVKeyboardGridRow@23456@"
          L"PE$AAUIKeyboardKeyData@LayoutData@Common@TextInput@456@@Z"},
         &originalAddKeySetBlock,
         AddKeySetBlockHook},
        {{L"?GetBlockPageCount@IKeyboardKeySet@LayoutData@Common@TextInput@"
          L"Experiences@ComposableShell@WindowsInternal@@UE$AAAIPE$AAVString@Platform@@@Z"},
         &originalGetBlockPageCount,
         GetBlockPageCountHook},
        {{L"?SetViewInternal@InputPanelModel@Controls@TextInputUndocked@Experiences@"
          L"ComposableShell@WindowsInternal@@AE$AAAXW4ViewName@LayoutData@Common@"
          L"TextInput@456@PE$AAUIInputPanelLayoutArgs@TouchKeyboardUX@@_N@Z"},
         &originalSetViewInternal,
         Hook_SetViewInternal},
        {{L"?CalculateLabelVisualUpdateInfo@KeyboardKey@Controls@TextInputUndocked@Experiences@"
          L"ComposableShell@WindowsInternal@@AE$AAAXPE$AAVKeyboardKeyLabel@23456@PE$AAVString@Platform@@1N@Z"},
         &originalLabelVisual,
         LabelVisualHook},
        // 追踪触摸键盘按键去向
        {{L"?OnPressAction@WideNarrowChineseSimplifiedKeyActionHandler@Controls@TextInputUndocked@"
          L"Experiences@ComposableShell@WindowsInternal@@MEAAXPE$AAVSingleKeyAction@23456@@Z"},
         &originalOnPressAction,
         OnPressActionHook},
        {{L"?OnReleaseAction@WideNarrowChineseSimplifiedKeyActionHandler@Controls@TextInputUndocked@"
          L"Experiences@ComposableShell@WindowsInternal@@MEAAXPE$AAVSingleKeyAction@23456@@Z"},
         &originalOnReleaseAction,
         OnReleaseActionHook},
        {{L"?InjectKey@DefaultKeyActionHandlerImpl@Controls@TextInputUndocked@"
          L"Experiences@ComposableShell@WindowsInternal@@MEAAXPE$AAUIKeyboardKeyControl@TouchKeyboardUX@@"
          L"PE$AAVString@Platform@@VPoint@Foundation@Windows@@_KW4TextInputKeyEventFlag@Common@TextInput@456@@Z"},
         &originalInjectKey,
         InjectKeyHook},
        {{L"?NotifyStringKey@?QISystemInputProcessor@TextFramework@TextInputUndocked@Experiences@"
          L"ComposableShell@WindowsInternal@@SystemInputProcessor@23456@UE$AAAXIIPE$AAVString@Platform@@HHI@Z"},
         &originalNotifyStringKey,
         NotifyStringKeyHook},
        {{L"?IsDictionaryReady@ITextFrameworkAdapter@Common@TextInput@Experiences@"
          L"ComposableShell@WindowsInternal@@UE$AAA_NPE$AAVString@Platform@@@Z"},
         &originalIsDictionaryReady,
         IsDictionaryReadyHook},
    };
    // 一次解析该模块的全部符号，避免反复失效符号缓存。
    return WindhawkUtils::HookSymbols(module, textinputDllHooks,
                                      ARRAYSIZE(textinputDllHooks), &options);
}
}  // namespace
BOOL Wh_ModInit() {
    g_enabled.store(Wh_GetIntSetting(L"enabled") != 0);
    wchar_t procPath[MAX_PATH]{};
    GetModuleFileNameW(nullptr, procPath, ARRAYSIZE(procPath));
    frontProcess = (wcsstr(procPath, L"TextInputHost") != nullptr);
    const wchar_t* ready =
        frontProcess ? L"native-front-ready" : L"native-service-ready";
    Wh_SetIntValue(
        frontProcess ? L"native-front-version" : L"native-service-version",
        2808);

    // Dynamic library loader hook catches modules loaded after process startup
    WindhawkUtils::SetFunctionHook(LoadLibraryExW, Hook_LoadLibraryExW, &original_LoadLibraryExW);

    if (frontProcess) {
        HMODULE front = GetModuleHandleW(L"TextInput.dll");
        if (front) {
            InstallFrontend(front);
        }
        return TRUE;
    }

    // Backend: ctfmon.exe
    HMODULE service = GetModuleHandleW(L"InputService.dll");
    if (service) {
        NativeT9::InstallService(service);
    }
    HMODULE decoder = GetModuleHandleW(L"ChsPinyinDS.dll");
    if (decoder) {
        NativeT9::InstallDecoder(decoder);
    }
    return TRUE;
}

void Wh_ModAfterInit() {
    Wh_SetIntValue(
        frontProcess ? L"native-front-ready" : L"native-service-ready",
        GetCurrentProcessId());
}
void Wh_ModSettingsChanged() {
    g_enabled.store(Wh_GetIntSetting(L"enabled") != 0);
}
void Wh_ModBeforeUninit() {
    g_enabled.store(false);
}
void Wh_ModUninit() {
    if (ownedDecoder)
        FreeLibrary(ownedDecoder);
    ownedDecoder = nullptr;
}
