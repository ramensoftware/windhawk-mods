// ==WindhawkMod==
// @id              touch-keyboard-chinese-t9
// @name            系统触摸键盘中文九宫格
// @description     在 Windows 11 系统触摸键盘中完整接通微软拼音原生十二键/九宫格输入法。
// @version         2.8.7
// @author          NekoMirra
// @github          https://github.com/NekoMirra
// @twitter         NekoMirra
// @include         TextInputHost.exe
// @include         ctfmon.exe
// @architecture    x86-64
// @compilerOptions -lruntimeobject -lbcrypt
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# 系统触摸键盘中文九宫格 (Windows 11 Touch Keyboard Chinese T9 / 12-Key)

在 Windows 11 系统触摸键盘中重新启用并完整接通原生微软拼音的十二键/九宫格（narrow12key）输入法。

## 功能特性
- **原生九宫格拼音输入**：短按按键正常产生中文汉字候选词。
- **长按纯数字候选**：长按九键按键时仅输出该数字候选，松手不产生二次输入。
- **数字面板直接输入**：切换到数字键盘时，数字直接上屏，不再弹出多余的拼音候选词。
- **原生表情符号面板唤起**：点击表情按钮及更多符号直接呼出与输入面板上方候选栏同款的原生 Windows 11 表情符号面板（支持表情、颜文字与全套 Unicode 符号），彻底避免跳转到空白占位界面。
- **稳定性与防闪退**：内置多层防御，自动拦截 Windows 11 在渲染狭窄九键及符号多页布局时的空指针与内部未适配断言，运行稳定流畅。

## 使用方法
1. 安装并启用本模组；
2. 确保当前输入法为 **中文（简体）- 微软拼音**；
3. 从任务栏唤起系统触摸键盘；
4. 点击键盘左上角的齿轮设置菜单 -> 键盘布局 -> 选择 **「十二键」** 即可。
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- enabled: true
  $name: 启用原生中文九宫格
*/
// ==/WindhawkModSettings==
#include <windhawk_utils.h>
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
                *reinterpret_cast<BYTE*>(pData + 32) = 1;
                modified = true;
            }
        }
    }
    ~QueryScope() {
        twelveQuery = previousQuery;
        if (modified && pData) {
            *reinterpret_cast<DWORD*>(pData + 304712) = previousMode;
            *reinterpret_cast<BYTE*>(pData + 32) = (previousMode == 4 || previousMode == 5) ? 1 : 0;
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
bool IsCurrentViewTwelveAlpha() {
    WCHAR frontMode[64]{};
    const bool isTwelve = Wh_GetStringValue(L"native-front-mode", frontMode, ARRAYSIZE(frontMode)) &&
                          wcsstr(frontMode, L"12key") != nullptr;
    const int frontView = Wh_GetIntValue(L"native-front-view", 1);
    return isTwelve && (frontView == 1 || frontView == 4 || frontView == 0);
}

// self 属于当前同步文档回调，不缓存跨文档指针。物理输入保持原生返回值。
HRESULT OnKeyDownHook(void* self, void* context, void* args) {
    const unsigned count = ++serviceKeyDownCount;
    Wh_SetIntValue(L"native-service-key-down-count", count);
    return originalOnKeyDown(self, context, args);
}

HRESULT RegularizeHook(void* self, void* event) {
    const unsigned rCount = ++regEntryCount;
    Wh_SetIntValue(L"native-reg-entry-count", rCount);
    if (event) {
        auto* data = static_cast<BYTE*>(event);
        Wh_SetIntValue(L"native-reg-entry-key", *reinterpret_cast<WORD*>(data + 92));
    }
    const HRESULT result = originalRegularize(self, event);
    if (FAILED(result) || !g_enabled.load())
        return result;
    if (*static_cast<void**>(self) != pinyinVtable)
        return result;
    auto* data = static_cast<BYTE*>(event);
    auto* text = reinterpret_cast<NativeString*>(event);
    const WORD flags = *reinterpret_cast<WORD*>(data + 34);
    const bool hardware =
        (flags & 0x4010) == 0x10 && (flags & 5) != 0 && (flags & 5) != 5;
    size_t channel =
        *reinterpret_cast<size_t*>(static_cast<BYTE*>(self) + 112);
    Wh_SetIntValue(L"native-reg-channel", static_cast<int>(channel));
    Wh_SetIntValue(L"native-reg-flags", flags);
    Wh_SetIntValue(L"native-reg-len", static_cast<int>(text->length));
    if (text->length > 0)
        Wh_SetIntValue(L"native-reg-char", text->data()[0]);
    Wh_SetIntValue(L"native-reg-key", *reinterpret_cast<WORD*>(data + 92));
    const bool isTwelveAlpha = IsCurrentViewTwelveAlpha();
    if (isTwelveAlpha) {
        if (channel != 4 && originalEnsureChannel) {
            originalEnsureChannel(self, 4);
            *reinterpret_cast<DWORD*>(static_cast<BYTE*>(self) + 164) = 4;
            channel = 4;
            Wh_SetIntValue(L"native-reg-channel-switched", 4);
        }
        auto* key = reinterpret_cast<WORD*>(data + 92);
        WORD normVk = 0;
        if (*key >= '1' && *key <= '9') {
            normVk = static_cast<WORD>(VK_NUMPAD0 + *key - '0');
        } else if (text->length == 1 && text->data()[0] >= L'1' && text->data()[0] <= L'9') {
            normVk = static_cast<WORD>(VK_NUMPAD0 + text->data()[0] - L'0');
        } else if (*key >= 0x61 && *key <= 0x69) {
            normVk = *key;
        }
        if (normVk != 0) {
            Wh_SetIntValue(L"native-reg-key-before", *key);
            *key = normVk;
            *reinterpret_cast<WORD*>(data + 40) = normVk;
            Wh_SetIntValue(L"native-normalized-count", ++normalized);
            Wh_SetIntValue(L"native-normalized-key", normVk);
            Wh_SetIntValue(L"native-reg-key-after", normVk);
        }
    } else if (!isTwelveAlpha && channel == 4 && originalEnsureChannel) {
        originalEnsureChannel(self, 1);
        *reinterpret_cast<DWORD*>(static_cast<BYTE*>(self) + 164) = 1;
        channel = 1;
        Wh_SetIntValue(L"native-reg-channel-switched", 1);
    }
    return result;
}

HRESULT DefaultChannelHook(void* self, bool off, size_t* channel) {
    const HRESULT res = originalDefaultChannel(self, off, channel);
    if (!off && channel) {
        const bool isTwelveAlpha = IsCurrentViewTwelveAlpha();
        const size_t targetCh = isTwelveAlpha ? 4 : 1;
        *channel = targetCh;
        *reinterpret_cast<DWORD*>(static_cast<BYTE*>(self) + 164) = static_cast<DWORD>(targetCh);
        Wh_SetIntValue(L"native-default-channel", static_cast<int>(targetCh));
    }
    return res;
}

HRESULT LayoutHook(void* self, const NativeString* layout) {
    Wh_SetIntValue(L"native-layout-count", ++layoutCount);
    if (layout && layout->length > 0 && layout->length < 100) {
        Wh_SetStringValue(L"native-layout-name", layout->data());
    }
    const HRESULT res = originalLayout(self, layout);
    WCHAR layoutName[128]{};
    if (Wh_GetStringValue(L"native-layout-name", layoutName, ARRAYSIZE(layoutName)) &&
        wcsstr(layoutName, L"12key") != nullptr) {
        const size_t curChan = *reinterpret_cast<size_t*>(static_cast<BYTE*>(self) + 112);
        if (curChan != 4 && originalEnsureChannel) {
            originalEnsureChannel(self, 4);
            *reinterpret_cast<DWORD*>(static_cast<BYTE*>(self) + 164) = 4;
        }
    }
    const size_t channel = *reinterpret_cast<size_t*>(static_cast<BYTE*>(self) + 112);
    Wh_SetIntValue(L"native-layout-channel", static_cast<int>(channel));
    return res;
}

HRESULT ProcessStringHook(void* self, const void* event) {
    Wh_SetIntValue(L"native-process-count", ++processCount);
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
        originalEnsureChannel(pinyinBase, 1);
        *reinterpret_cast<DWORD*>(static_cast<BYTE*>(pinyinBase) + 164) = 1;
        channel = 1;
    }
    Wh_SetIntValue(L"native-process-channel", static_cast<int>(channel));
    auto* text = reinterpret_cast<const NativeString*>(event);
    Wh_SetIntValue(L"native-process-key", *reinterpret_cast<const WORD*>(data + 92));
    Wh_SetIntValue(L"native-process-flags", flags);
    if (text->length > 0)
        Wh_SetIntValue(L"native-process-char", text->data()[0]);
    return originalProcess(self, event);
}

HRESULT ChannelHandleHook(void* self, const void* event, bool* a3, bool* a4) {
    Wh_SetIntValue(L"native-channel-count", ++channelCount);
    return originalChannelHandle(self, event, a3, a4);
}

HRESULT VKeyHandleHook(void* self, const void* event, bool* a3, bool* a4) {
    Wh_SetIntValue(L"native-vkey-count", ++vkeyCount);
    return originalVKeyHandle(self, event, a3, a4);
}

HRESULT NativeInsertHook(void* self, const void* event) {
    Wh_SetIntValue(L"native-insert-count", ++insertedCount);
    return originalInsert(self, event);
}

HRESULT ChcOpicHook(void* self, int mask) {
    Wh_SetIntValue(L"native-chc-opic-count", ++chcOpicCount);
    Wh_SetIntValue(L"native-chc-opic-mask", mask);
    return originalChcOpic(self, mask);
}

HRESULT RequestSuggestionHook(void* self, const wchar_t* text, unsigned int len, void* propBag, void* handler, bool b) {
    Wh_SetIntValue(L"native-rsbs-count", ++rsbsCount);
    if (text && len > 0)
        Wh_SetIntValue(L"native-rsbs-char", text[0]);
    return originalRequestSuggestion(self, text, len, propBag, handler, b);
}

HRESULT CandidatesHook(void* self, void* query, void** list) {
    Wh_SetIntValue(L"native-cand-call-count", ++candCallCount);
    if (!g_enabled.load() || !query || !list)
        return originalCandidates(self, query, list);
    using GetProperties = HRESULT(WINAPI*)(void*, QueryProperties*);
    QueryProperties properties{};
    auto methods = *static_cast<void***>(query);
    const HRESULT status =
        reinterpret_cast<GetProperties>(methods[3])(query, &properties);
    if (FAILED(status))
        return status;
    Wh_SetIntValue(L"native-cand-raw-type", properties.type);
    Wh_SetIntValue(L"native-cand-raw-flags", properties.flags);
    Wh_SetIntValue(L"native-cand-raw-mode", properties.mode);
    const bool isTwelveAlpha = IsCurrentViewTwelveAlpha();
    const bool isTwelve = isTwelveAlpha &&
                          ((properties.type & 0xFF) == 11 || properties.type == 11 || properties.type == 2 || properties.type == 0);
    QueryScope scope(static_cast<BYTE*>(self), isTwelve);
    if (isTwelve) {
        Wh_SetIntValue(L"native-twelve-query-flags", properties.flags);
        Wh_SetIntValue(L"native-cand-type", properties.type);
    }
    const HRESULT candStatus = originalCandidates(self, query, list);
    Wh_SetIntValue(L"native-cand-res", candStatus);
    if (SUCCEEDED(candStatus) && list && *list) {
        using GetCountFn = HRESULT (*)(void*, int*);
        int cCount = 0;
        void** vtbl = *reinterpret_cast<void***>(*list);
        if (vtbl) {
            GetCountFn getCount = reinterpret_cast<GetCountFn>(vtbl[3]);
            if (SUCCEEDED(getCount(*list, &cCount))) {
                Wh_SetIntValue(L"native-cand-result-count", cCount);
            }
        }
    }
    return candStatus;
}

HRESULT DecideHook(void* self, WORD first) {
    Wh_SetIntValue(L"native-decide-call-count", ++decideCallCount);
    Wh_SetIntValue(L"native-decide-char", first);
    const bool isTwelveAlpha = IsCurrentViewTwelveAlpha();
    const bool isTwelve = isTwelveAlpha && twelveQuery;
    static unsigned int s_lastConsumedLongSeq = 0;
    const unsigned int currentLongSeq = static_cast<unsigned int>(Wh_GetIntValue(L"native-t9-long-seq", 0));
    const unsigned int longDigit = static_cast<unsigned int>(Wh_GetIntValue(L"native-t9-long-digit", 0));
    const bool isLongPress = (currentLongSeq != 0 && currentLongSeq != s_lastConsumedLongSeq && first == longDigit);
    if (isLongPress) {
        s_lastConsumedLongSeq = currentLongSeq;
        Wh_SetIntValue(L"native-t9-long-consumed", currentLongSeq);
        return originalDecide(self, first);
    }
    if (g_enabled.load() && isTwelve && first >= L'2' && first <= L'9') {
        auto* data = static_cast<BYTE*>(self);
        auto* manager = *reinterpret_cast<BYTE**>(data + 57616);
        auto* reading = *reinterpret_cast<BYTE**>(data + 22152);
        if (!manager || !reading)
            return E_UNEXPECTED;
        *reinterpret_cast<BYTE**>(manager) = twelveProfile;
        *reinterpret_cast<BYTE**>(data + 57608) = twelveProfile;
        *reinterpret_cast<int*>(reading + 4396) = 2;
        *reinterpret_cast<BYTE**>(reading + 8064) = twelveProfile + 28;
        *reinterpret_cast<BYTE**>(reading + 8072) = twelveProfile + 540;
        Wh_SetIntValue(L"native-decoded-count", ++decoded);
        return 0;
    }
    return originalDecide(self, first);
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
    if (!WindhawkUtils::HookSymbols(module, inputserviceDllHooks,
                                    ARRAYSIZE(inputserviceDllHooks), nullptr))
        return false;
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
    return WindhawkUtils::SetFunctionHook(originalRegularize, RegularizeHook,
                                          &originalRegularize) &&
           WindhawkUtils::SetFunctionHook(originalDefaultChannel, DefaultChannelHook,
                                          &originalDefaultChannel) &&
           WindhawkUtils::SetFunctionHook(originalLayout, LayoutHook,
                                          &originalLayout) &&
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
    if (!WindhawkUtils::HookSymbols(module, chspinyindsDllHooks,
                                    ARRAYSIZE(chspinyindsDllHooks), nullptr))
        return false;
    if (!CodeMatches(
            reinterpret_cast<void*>(originalDecide), 0x1f3,
            "d2d9038ce0d8be4c4019eeb741bda43aece66f8e98de847011f10a8f63adc46e"))
        return false;
    if (!CodeMatches(
            reinterpret_cast<void*>(originalCandidates), 0x2ed3,
            "7e0b8b1dc419ec319a1bb3275cbc2e2735e21332a238cef6b25959c7dfcc0677"))
        return false;
    return WindhawkUtils::SetFunctionHook(originalCandidates, CandidatesHook,
                                          &originalCandidates) &&
           WindhawkUtils::SetFunctionHook(originalDecide, DecideHook,
                                          &originalDecide);
}
}  // namespace NativeT9


namespace {
using Availability = bool (*)(void*, HSTRING);
using ModeView = void (*)(void*, HSTRING, int);
using OnPressActionFn = void (*)(void*, void*);
using OnPressAndHoldActionFn = void (*)(void*, void*);
using OnReleaseActionFn = void (*)(void*, void*);
using HandleFlowerKeyReleaseFn = void (*)(void*, void*);
using InjectKeyWithoutKeyControlFn = void (*)(void*, void*, void*, unsigned int);
using InjectKeyFn = void (*)(void*, void*, HSTRING, void*, unsigned __int64, int);
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
    UINT32 length = 0;
    const wchar_t* text = WindowsGetStringRawBuffer(mode, &length);
    return length == 11 && CompareStringOrdinal(text, length, L"narrow12key",
                                                11, TRUE) == CSTR_EQUAL;
}

bool AvailabilityHook(void* self, HSTRING mode) {
    const bool result = originalAvailability(self, mode);
    if (g_enabled.load() && IsTwelve(mode)) {
        Wh_SetStringValue(L"native-front-mode", L"narrow12key");
        return true;
    }
    return result;
}

void ModeHook(void* self, HSTRING mode, int view) {
    Wh_SetStringValue(L"native-front-mode",
                      WindowsGetStringRawBuffer(mode, nullptr));
    Wh_SetIntValue(L"native-front-view", view);
    originalMode(self, mode, view);
}

void OnReleaseActionHook(void* self, void* action) {
    const unsigned count = ++frontReleaseCount;
    Wh_SetIntValue(L"native-front-release-count", count);
    int priorHandlingState = 5;
    bool isEmojiOrCategory = false;
    if (action) {
        auto* data = static_cast<BYTE*>(action);
        Wh_SetIntValue(L"native-front-release-handled-before", data[144]);
        void* keyCtrl = *reinterpret_cast<void**>(data + 56);
        Wh_SetIntValue(L"native-front-release-key-ctrl", keyCtrl != nullptr ? 1 : 0);
        if (self && keyCtrl && originalGetPressedKeyHandlingState) {
            priorHandlingState = originalGetPressedKeyHandlingState(self, keyCtrl);
        }
        Wh_SetIntValue(L"native-front-release-state", priorHandlingState);

        if (keyCtrl && originalGetKeyData && originalGetRecognizedText) {
            void* keyData = originalGetKeyData(keyCtrl);
            if (keyData) {
                HSTRING hName = originalGetRecognizedText(keyData);
                if (hName) {
                    UINT32 len = 0;
                    const wchar_t* buf = WindowsGetStringRawBuffer(hName, &len);
                    if (buf && len > 0) {
                        Wh_SetStringValue(L"native-front-last-key-name", buf);
                        if (wcsstr(buf, L"Emoji") || wcsstr(buf, L"EMOJI") || wcsstr(buf, L"emoji") ||
                            wcsstr(buf, L"Category") || wcsstr(buf, L"CATEGORY") ||
                            wcsstr(buf, L"category") || wcsstr(buf, L"More") ||
                            wcsstr(buf, L"MORE")) {
                            isEmojiOrCategory = true;
                        }
                    }
                    WindowsDeleteString(hName);
                }
                using ReleaseFn = ULONG (*)(void*);
                reinterpret_cast<ReleaseFn>((*reinterpret_cast<void***>(keyData))[2])(keyData);
            }
        }
    }

    if (isEmojiOrCategory) {
        if (action) {
            static_cast<BYTE*>(action)[144] = 1;
        }
        keybd_event(VK_LWIN, 0, 0, 0);
        keybd_event(VK_OEM_PERIOD, 0, 0, 0);
        keybd_event(VK_OEM_PERIOD, 0, KEYEVENTF_KEYUP, 0);
        keybd_event(VK_LWIN, 0, KEYEVENTF_KEYUP, 0);
        Wh_SetIntValue(L"native-front-emoji-triggered", 1);
        return;
    }

    {
        ReleaseActionScope scope;
        originalOnReleaseAction(self, action);
    }
    if (action) {
        auto* data = static_cast<BYTE*>(action);
        const bool isTwelveAlpha = NativeT9::IsCurrentViewTwelveAlpha();
        void* keyCtrl = *reinterpret_cast<void**>(data + 56);
        if (isTwelveAlpha && priorHandlingState == 1) {
            data[144] = 1;
            Wh_SetIntValue(L"native-front-release-suppressed-long", 1);
        } else if (isTwelveAlpha && priorHandlingState == 0 && data[144] == 0 && keyCtrl && originalGetKeyData) {
            void* keyData = originalGetKeyData(keyCtrl);
            if (keyData && originalGetVirtualKey) {
                const unsigned char vk = originalGetVirtualKey(keyData);
                if (vk >= 0x62 && vk <= 0x69) {
                    wchar_t digitStr[2] = { static_cast<wchar_t>(L'0' + vk - 0x60), 0 };
                    HSTRING hStr = nullptr;
                    if (SUCCEEDED(WindowsCreateString(digitStr, 1, &hStr))) {
                        originalInjectKeyWithoutKeyControl(self, hStr, keyData, 255);
                        WindowsDeleteString(hStr);
                        data[144] = 1;
                        Wh_SetIntValue(L"native-front-release-suppressed-raw", vk);
                    }
                }
                using ReleaseFn = ULONG (*)(void*);
                reinterpret_cast<ReleaseFn>((*reinterpret_cast<void***>(keyData))[2])(keyData);
            }
        } else if (!isTwelveAlpha && data[144] == 0 && keyCtrl && originalGetKeyData) {
            void* keyData = originalGetKeyData(keyCtrl);
            if (keyData && originalGetVirtualKey) {
                const unsigned char vk = originalGetVirtualKey(keyData);
                BYTE digit = 0;
                if (vk >= '0' && vk <= '9') {
                    digit = vk;
                } else if (vk >= 0x60 && vk <= 0x69) {
                    digit = static_cast<BYTE>('0' + vk - 0x60);
                }
                if (digit != 0) {
                    keybd_event(digit, 0, 0, 0);
                    keybd_event(digit, 0, KEYEVENTF_KEYUP, 0);
                    data[144] = 1;
                    Wh_SetIntValue(L"native-front-numeric-direct-injected", digit);
                } else if (originalGetRecognizedText) {
                    HSTRING hName = originalGetRecognizedText(keyData);
                    if (hName) {
                        UINT32 len = 0;
                        const wchar_t* buf = WindowsGetStringRawBuffer(hName, &len);
                        if (buf && len > 0) {
                            std::vector<INPUT> inputs(len * 2);
                            for (UINT32 i = 0; i < len; ++i) {
                                inputs[i * 2].type = INPUT_KEYBOARD;
                                inputs[i * 2].ki.wScan = buf[i];
                                inputs[i * 2].ki.dwFlags = KEYEVENTF_UNICODE;
                                inputs[i * 2 + 1].type = INPUT_KEYBOARD;
                                inputs[i * 2 + 1].ki.wScan = buf[i];
                                inputs[i * 2 + 1].ki.dwFlags = KEYEVENTF_UNICODE | KEYEVENTF_KEYUP;
                            }
                            SendInput(static_cast<UINT>(inputs.size()), inputs.data(), sizeof(INPUT));
                            data[144] = 1;
                            Wh_SetIntValue(L"native-front-symbol-injected", buf[0]);
                        }
                        WindowsDeleteString(hName);
                    }
                }
                using ReleaseFn = ULONG (*)(void*);
                reinterpret_cast<ReleaseFn>((*reinterpret_cast<void***>(keyData))[2])(keyData);
            }
        }
        Wh_SetIntValue(L"native-front-release-handled-after", data[144]);
    }
}

void OnPressActionHook(void* self, void* action) {
    Wh_SetIntValue(L"native-front-press-count", ++frontPressCount);
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
        Wh_SetIntValue(L"native-front-dir-30-intercepted", val);
        return 0;
    }
    if (g_releaseDepth != 0) {
        Wh_SetIntValue(L"native-front-dir-val", val);
        Wh_SetIntValue(L"native-front-dir-count", ++frontDirCount);
    }
    return val;
}

int HeightHook(void* self) {
    const int val = originalGetHeight(self);
    if (g_releaseDepth != 0) {
        Wh_SetIntValue(L"native-front-height-val", val);
        Wh_SetIntValue(L"native-front-height-count", ++frontHeightCount);
    }
    return val;
}
float GetSecondaryLabelFontScalingHook(void* self) {
    WCHAR layoutName[128]{};
    if (Wh_GetStringValue(L"native-layout-name", layoutName, ARRAYSIZE(layoutName)) &&
        wcsstr(layoutName, L"12key") != nullptr) {
        Wh_SetIntValue(L"native-front-scaling-fixed", 1);
        return 0.50f;
    }
    return originalGetSecondaryLabelFontScaling(self);
}
bool IsEmojiViewEnabledHook(void* self) {
    Wh_SetIntValue(L"native-front-emoji-view-enabled", 1);
    return true;
}
int GetPressedKeyHandlingStateHook(void* self, void* keyControl) {
    return originalGetPressedKeyHandlingState(self, keyControl);
}
HSTRING GetRecognizedTextHook(void* self) {
    return originalGetRecognizedText(self);
}

bool IsDictionaryReadyHook(void* self, void* str) {
    const bool res = originalIsDictionaryReady(self, str);
    if (g_releaseDepth != 0) {
        Wh_SetIntValue(L"native-front-dict-ready-res", res ? 1 : 0);
        Wh_SetIntValue(L"native-front-dict-ready-count", ++frontDictReadyCount);
    }
    return res;
}

void NotifyUIMessageHook(void* self, int a2, int a3, int a4, int a5, void* a6) {
    if (g_releaseDepth != 0) {
        Wh_SetIntValue(L"native-front-notify-a2", a2);
        Wh_SetIntValue(L"native-front-notify-a3", a3);
        Wh_SetIntValue(L"native-front-notify-a4", a4);
        Wh_SetIntValue(L"native-front-notify-count", ++frontNotifyMsgCount);
    }
    originalNotifyUIMessage(self, a2, a3, a4, a5, a6);
}
void* KeyDataHook(void* self) {
    return originalGetKeyData(self);
}

unsigned char VirtualKeyHook(void* self) {
    const unsigned char val = originalGetVirtualKey(self);
    if (g_releaseDepth != 0) {
        Wh_SetIntValue(L"native-front-vk-val", val);
        Wh_SetIntValue(L"native-front-vk-count", ++frontVkGetCount);
    }
    return val;
}
unsigned int GetBlockPageCountHook(void* self, HSTRING block) {
    if (!self) {
        Wh_SetIntValue(L"native-front-block-page-null-intercepted", 1);
        return 0;
    }
    return originalGetBlockPageCount(self, block);
}
void InitializeGroupModelHook(void* self, void* a2, void* a3, void* a4, void* a5, void* a6, void* a7, void* a8, void* a9, char a10, int a11, void* a12, void* a13, void* a14) {
    originalInitializeGroupModel(self, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14);
    if (self) {
        auto* data = static_cast<BYTE*>(self);
        void** keySet104 = reinterpret_cast<void**>(data + 104);
        void** keySet128 = reinterpret_cast<void**>(data + 128);
        if (*keySet104 == nullptr && *keySet128 != nullptr) {
            *keySet104 = *keySet128;
            using AddRefFn = ULONG (*)(void*);
            reinterpret_cast<AddRefFn>((*reinterpret_cast<void***>(*keySet104))[1])(*keySet104);
            Wh_SetIntValue(L"native-front-keyset-fixed", 1);
        }
    }
}
bool IsSubTypeHook(void* self, int subType) {
    if (subType == 19) {
        const uintptr_t ret = reinterpret_cast<uintptr_t>(__builtin_return_address(0));
        const uintptr_t base = reinterpret_cast<uintptr_t>(GetModuleHandleW(L"TextInput.dll"));
        if (base != 0 && (ret - base) == 0x000a8115) {
            Wh_SetIntValue(L"native-front-subtype-intercepted", 19);
            return false;
        }
    }
    return originalIsSubType(self, subType);
}
void* AcquireHook(HSTRING text, int viewState, void* keyData, int virtualKey, int flags, unsigned __int64 a6, void* a7) {
    const unsigned count = ++frontAcquireCount;
    Wh_SetIntValue(L"native-front-acq-count", count);
    Wh_SetIntValue(L"native-front-acq-vk-in", virtualKey);
    if (virtualKey == 0 && keyData && originalGetVirtualKey) {
        const unsigned char vk = originalGetVirtualKey(keyData);
        if (vk != 0) {
            virtualKey = vk;
            Wh_SetIntValue(L"native-front-acq-vk-fixed", virtualKey);
        }
    }
    return originalAcquire(text, viewState, keyData, virtualKey, flags, a6, a7);
}
void NotifyStringKeyHook(void* self, unsigned int qualifiers, unsigned int key, HSTRING text, int x, int y, unsigned int time) {
    const unsigned count = ++frontNskCount;
    Wh_SetIntValue(L"native-front-nsk-count", count);
    Wh_SetIntValue(L"native-front-nsk-key-in", key);
    Wh_SetIntValue(L"native-front-nsk-qual", qualifiers);
    if (text) {
        UINT32 len = 0;
        const wchar_t* buf = WindowsGetStringRawBuffer(text, &len);
        if (buf && len > 0) {
            Wh_SetIntValue(L"native-front-nsk-char", buf[0]);
        }
    }
    originalNotifyStringKey(self, qualifiers, key, text, x, y, time);
}



void HandleFlowerKeyReleaseHook(void* self, void* action) {
    const unsigned count = ++frontFlowerCount;
    Wh_SetIntValue(L"native-front-flower-count", count);
    if (action) {
        auto* data = static_cast<BYTE*>(action);
        Wh_SetIntValue(L"native-front-flower-f80", *reinterpret_cast<DWORD*>(data + 80));
        Wh_SetIntValue(L"native-front-flower-f84", *reinterpret_cast<DWORD*>(data + 84));
        Wh_SetIntValue(L"native-front-flower-has-str", *reinterpret_cast<void**>(data + 64) != nullptr ? 1 : 0);
    }
    originalHandleFlowerKeyRelease(self, action);
}

void InjectKeyWithoutKeyControlHook(void* self, void* str, void* keyData, unsigned int a4) {
    const unsigned count = ++frontInjectKcCount;
    Wh_SetIntValue(L"native-front-inject-kc-count", count);
    Wh_SetIntValue(L"native-front-inject-kc-a4", a4);
    Wh_SetIntValue(L"native-front-inject-kc-has-data", keyData != nullptr ? 1 : 0);
    Wh_SetIntValue(L"native-front-inject-kc-has-str", str != nullptr ? 1 : 0);
    if (g_inPressAndHold && str) {
        auto hStr = reinterpret_cast<HSTRING>(str);
        UINT32 len = 0;
        const wchar_t* buf = WindowsGetStringRawBuffer(hStr, &len);
        if (buf && len == 1 && buf[0] >= L'2' && buf[0] <= L'9') {
            static std::atomic<unsigned int> s_longPressSeq{0};
            Wh_SetIntValue(L"native-t9-long-digit", buf[0]);
            Wh_SetIntValue(L"native-t9-long-seq", ++s_longPressSeq);
        }
    }
    originalInjectKeyWithoutKeyControl(self, str, keyData, a4);
}

void InjectKeyHook(void* self, void* keyControl, HSTRING text, void* point, unsigned __int64 a5, int flags) {
    const unsigned count = ++frontInjectKeyCount;
    Wh_SetIntValue(L"native-front-inj-key-count", count);
    Wh_SetIntValue(L"native-front-inj-key-flags", flags);
    if (text) {
        UINT32 len = 0;
        const wchar_t* buf = WindowsGetStringRawBuffer(text, &len);
        if (buf && len > 0) {
            Wh_SetIntValue(L"native-front-inj-key-char", buf[0]);
        }
    }
    if (keyControl && originalGetKeyData) {
        void* keyData = originalGetKeyData(keyControl);
        if (keyData) {
            if (originalGetDirection)
                Wh_SetIntValue(L"native-front-inj-key-dir", originalGetDirection(keyData));
            if (originalGetHeight)
                Wh_SetIntValue(L"native-front-inj-key-height", originalGetHeight(keyData));
            if (originalGetVirtualKey)
                Wh_SetIntValue(L"native-front-inj-key-vk", originalGetVirtualKey(keyData));
            using ReleaseFn = ULONG (*)(void*);
            reinterpret_cast<ReleaseFn>((*reinterpret_cast<void***>(keyData))[2])(keyData);
        }
    }
    originalInjectKey(self, keyControl, text, point, a5, flags);
}

bool InstallFrontend(HMODULE module) {
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
        {{L"?OnPressAction@WideNarrowChineseSimplifiedKeyActionHandler@"
          L"Controls@TextInputUndocked@Experiences@ComposableShell@"
          L"WindowsInternal@@MEAAXPE$AAVSingleKeyAction@23456@@Z"},
         &originalOnPressAction,
         OnPressActionHook},
        {{L"?OnPressAndHoldAction@WideNarrowChineseSimplifiedKeyActionHandler@"
          L"Controls@TextInputUndocked@Experiences@ComposableShell@"
          L"WindowsInternal@@MEAAXPE$AAVSingleKeyAction@23456@@Z"},
         &originalOnPressAndHoldAction,
         OnPressAndHoldActionHook},
        {{L"?OnReleaseAction@WideNarrowChineseSimplifiedKeyActionHandler@"
          L"Controls@TextInputUndocked@Experiences@ComposableShell@"
          L"WindowsInternal@@MEAAXPE$AAVSingleKeyAction@23456@@Z"},
         &originalOnReleaseAction,
         OnReleaseActionHook},
        {{L"?HandleFlowerKeyRelease@WideNarrowChineseSimplifiedKeyActionHandler@"
          L"Controls@TextInputUndocked@Experiences@ComposableShell@"
          L"WindowsInternal@@MEAAXPE$AAVFlowerKeyAction@23456@@Z"},
         &originalHandleFlowerKeyRelease,
         HandleFlowerKeyReleaseHook},
        {{L"?InjectKeyWithoutKeyControl@DefaultKeyActionHandlerImpl@Controls@"
          L"TextInputUndocked@Experiences@ComposableShell@WindowsInternal@@"
          L"IEAAXPE$AAVString@Platform@@PE$AAUIKeyboardKeyData@LayoutData@"
          L"Common@TextInput@456@I@Z"},
         &originalInjectKeyWithoutKeyControl,
         InjectKeyWithoutKeyControlHook},
        {{L"?InjectKey@DefaultKeyActionHandlerImpl@Controls@TextInputUndocked@"
          L"Experiences@ComposableShell@WindowsInternal@@MEAAXPE$AAUIKeyboardKeyControl@"
          L"TouchKeyboardUX@@PE$AAVString@Platform@@VPoint@Foundation@Windows@@_K"
          L"W4TextInputKeyEventFlag@Common@TextInput@456@@Z"},
         &originalInjectKey,
         InjectKeyHook},
        {{L"?get@Direction@IViewPositionAndSizeManagerTransferringXYFocusEventArgs@"
          L"TouchKeyboardUX@@UE$AAA?AW4DirectionalNavigationDirection@Common@"
          L"TextInput@Experiences@ComposableShell@WindowsInternal@@XZ"},
         &originalGetDirection,
         DirectionHook},
        {{L"?get@HandwritingLineHeight@IPenAndInkSettings@Inking@Input@UI@"
          L"Windows@@UE$AAA?AW413456@XZ"},
         &originalGetHeight,
         HeightHook},
        {{L"?IsDictionaryReady@ITextFrameworkAdapter@Common@TextInput@"
          L"Experiences@ComposableShell@WindowsInternal@@UE$AAA_NPE$AAVString@"
          L"Platform@@@Z"},
         &originalIsDictionaryReady,
         IsDictionaryReadyHook},
        {{L"?NotifyInputUIMessage@ITextFrameworkAdapter@Common@TextInput@"
          L"Experiences@ComposableShell@WindowsInternal@@UE$AAAXW4"
          L"TextInputCandidateControlId@23456@IW4TextInputKeyboardMessageKind@"
          L"23456@IP$01E$ABV?$Array@E$00@Platform@@@Z"},
         &originalNotifyUIMessage,
         NotifyUIMessageHook},
        {{L"?get@KeyData@IKeyboardKeyControl@TouchKeyboardUX@@UE$AAAPE$AAUIKeyboardKeyData@LayoutData@Common@TextInput@Experiences@ComposableShell@WindowsInternal@@XZ"},
         &originalGetKeyData,
         KeyDataHook},
        {{L"?get@VirtualKey@IKeyboardKeyData@LayoutData@Common@TextInput@Experiences@ComposableShell@WindowsInternal@@UE$AAAEXZ"},
         &originalGetVirtualKey,
         VirtualKeyHook},
        {{L"?Acquire@KeyboardKeyEventArgs@Controls@TextInputUndocked@Experiences@ComposableShell@WindowsInternal@@SAPE$AAV123456@PE$AAVString@Platform@@W4KeyViewState@TouchKeyboardUX@@PE$AAUIKeyboardKeyData@LayoutData@Common@TextInput@456@W4VirtualKey@System@Windows@@W4TextInputKeyEventFlag@Common@TextInput@456@_KAEBVPoint@Foundation@Windows@@@Z"},
         &originalAcquire,
         AcquireHook},
        {{L"?NotifyStringKey@ISystemInputProcessor@TextFramework@TextInputUndocked@Experiences@ComposableShell@WindowsInternal@@UE$AAAXIIPE$AAVString@Platform@@HHI@Z"},
         &originalNotifyStringKey,
         NotifyStringKeyHook},
        {{L"?GetPressedKeyHandlingState@DefaultKeyActionHandlerImpl@Controls@"
          L"TextInputUndocked@Experiences@ComposableShell@WindowsInternal@@"
          L"IEAA?AW4HandlingState@123456@PE$AAUIKeyboardKeyControl@TouchKeyboardUX@@@Z"},
         &originalGetPressedKeyHandlingState,
         GetPressedKeyHandlingStateHook},
        {{L"?GetBlockPageCount@IKeyboardKeySet@LayoutData@Common@TextInput@"
          L"Experiences@ComposableShell@WindowsInternal@@UE$AAAIPE$AAVString@Platform@@@Z"},
         &originalGetBlockPageCount,
         GetBlockPageCountHook},
        {{L"?Initialize@KeyboardGridGroupModel@Controls@TextInputUndocked@"
          L"Experiences@ComposableShell@WindowsInternal@@QE$AAAXPE$AAUIKeyboardFrameGroup@"
          L"LayoutData@Common@TextInput@456@PE$AAUIKeyboardFrameRow@89TextInput@456@11"
          L"PE$AAUIKeyboardKeySet@89TextInput@456@22PE$AAUIKeyboardCategorySet@89"
          L"TextInput@456@_NW4CoreTextInputScope@Core@Text@UI@Windows@@PE$AAUIInputPanelTemplateManager@"
          L"TouchKeyboardUX@@PE$AAUITextInputSettingsManager@9TextInput@456@PE$AAUIInputPanelModel@"
          L"TouchKeyboardUX@@@Z"},
         &originalInitializeGroupModel,
         InitializeGroupModelHook},
        {{L"?IsSubType@IKeyboardKeyData@LayoutData@Common@TextInput@"
          L"Experiences@ComposableShell@WindowsInternal@@UE$AAA_NW4KeySubType@234567@@Z"},
         &originalIsSubType,
         IsSubTypeHook},
        {{L"?get@RecognizedText@IInkAnalysisLine@Analysis@Inking@Input@UI@"
          L"Windows@@UE$AAAPE$AAVString@Platform@@XZ"},
         &originalGetRecognizedText,
         GetRecognizedTextHook},
        {{L"?get@SecondaryLabelFontScaling@IKeyboardKeyLabelResources@LayoutData@Common@"
          L"TextInput@Experiences@ComposableShell@WindowsInternal@@UE$AAAMXZ"},
         &originalGetSecondaryLabelFontScaling,
         GetSecondaryLabelFontScalingHook},
        {{L"?get@?QIInputPanelModel@TouchKeyboardUX@@IsEmojiViewEnabled@InputPanelModel@"
          L"Controls@TextInputUndocked@Experiences@ComposableShell@WindowsInternal@@UE$AAA_NXZ"},
         &originalIsEmojiViewEnabled,
         IsEmojiViewEnabledHook},
    };
    return WindhawkUtils::HookSymbols(module, textinputDllHooks,
                                      ARRAYSIZE(textinputDllHooks), nullptr);
}
}  // namespace
BOOL Wh_ModInit() {
    g_enabled.store(Wh_GetIntSetting(L"enabled") != 0);
    wchar_t procPath[MAX_PATH]{};
    GetModuleFileNameW(nullptr, procPath, ARRAYSIZE(procPath));
    frontProcess = (wcsstr(procPath, L"TextInputHost") != nullptr);
    const wchar_t* ready =
        frontProcess ? L"native-front-ready" : L"native-service-ready";
    Wh_SetIntValue(ready, 0);
    Wh_SetIntValue(
        frontProcess ? L"native-front-version" : L"native-service-version",
        287);
    if (frontProcess) {
        HMODULE front = GetModuleHandleW(L"TextInput.dll");
        if (!front) {
            front = LoadLibraryW(L"TextInput.dll");
        }
        if (!front) {
            return FALSE;
        }
        Wh_SetIntValue(L"native-front-release-count", 0);
        Wh_SetIntValue(L"native-front-release-handled-before", 0);
        Wh_SetIntValue(L"native-front-release-handled-after", 0);
        Wh_SetIntValue(L"native-front-release-key-ctrl", 0);
        Wh_SetIntValue(L"native-front-dir-val", -1);
        Wh_SetIntValue(L"native-front-dir-count", 0);
        Wh_SetIntValue(L"native-front-height-val", -1);
        Wh_SetIntValue(L"native-front-height-count", 0);
        Wh_SetIntValue(L"native-front-dict-ready-res", -1);
        Wh_SetIntValue(L"native-front-dict-ready-count", 0);
        Wh_SetIntValue(L"native-front-notify-a2", 0);
        Wh_SetIntValue(L"native-front-notify-a3", 0);
        Wh_SetIntValue(L"native-front-notify-a4", 0);
        Wh_SetIntValue(L"native-front-notify-count", 0);
        Wh_SetIntValue(L"native-front-flower-count", 0);
        Wh_SetIntValue(L"native-front-flower-f80", 0);
        Wh_SetIntValue(L"native-front-flower-f84", 0);
        Wh_SetIntValue(L"native-front-flower-has-str", 0);
        Wh_SetIntValue(L"native-front-inject-kc-count", 0);
        Wh_SetIntValue(L"native-front-inject-kc-a4", 0);
        Wh_SetIntValue(L"native-front-inject-kc-has-data", 0);
        Wh_SetIntValue(L"native-front-inject-kc-has-str", 0);
        Wh_SetIntValue(L"native-front-inj-key-count", 0);
        Wh_SetIntValue(L"native-front-inj-key-flags", 0);
        Wh_SetIntValue(L"native-front-inj-key-char", 0);
        Wh_SetIntValue(L"native-front-inj-key-dir", -1);
        Wh_SetIntValue(L"native-front-inj-key-height", -1);
        Wh_SetIntValue(L"native-front-inj-key-vk", -1);
        Wh_SetIntValue(L"native-front-vk-val", -1);
        Wh_SetIntValue(L"native-front-vk-count", 0);
        Wh_SetIntValue(L"native-front-acq-count", 0);
        Wh_SetIntValue(L"native-front-acq-vk-in", 0);
        Wh_SetIntValue(L"native-front-acq-vk-fixed", 0);
        Wh_SetIntValue(L"native-front-nsk-count", 0);
        Wh_SetIntValue(L"native-front-nsk-key-in", 0);
        Wh_SetIntValue(L"native-front-nsk-key-fixed", 0);
        Wh_SetIntValue(L"native-front-nsk-char", 0);
        Wh_SetIntValue(L"native-front-nsk-qual", 0);
        Wh_SetIntValue(L"native-front-release-suppressed-raw", 0);
        Wh_SetIntValue(L"native-cand-result-count", 0);
        Wh_SetIntValue(L"native-cand-res", -1);
        Wh_SetIntValue(L"native-front-press-count", 0);
        Wh_SetIntValue(L"native-front-release-state", 0);
        Wh_SetIntValue(L"native-front-release-suppressed-long", 0);
        Wh_SetIntValue(L"native-t9-long-seq", 0);
        Wh_SetIntValue(L"native-t9-long-consumed", 0);
        Wh_SetIntValue(L"native-t9-long-digit", 0);
        Wh_SetIntValue(L"native-front-block-page-null-intercepted", 0);
        Wh_SetIntValue(L"native-front-keyset-fixed", 0);
        Wh_SetIntValue(L"native-front-subtype-intercepted", 0);
        Wh_SetIntValue(L"native-front-dir-30-intercepted", 0);
        Wh_SetIntValue(L"native-front-emoji-triggered", 0);
        Wh_SetStringValue(L"native-front-last-key-name", L"");
        Wh_SetIntValue(L"native-front-scaling-fixed", 0);
        Wh_SetIntValue(L"native-front-emoji-view-enabled", 0);
        Wh_SetIntValue(L"native-front-numeric-direct-injected", 0);
        return InstallFrontend(front);
    }
#if !defined(_M_X64) && !defined(__x86_64__)
    Wh_Log(L"[中文九键] 当前私有接口仅核验 x64，拒绝加载其他架构");
    return FALSE;
#else
    HMODULE service = GetModuleHandleW(L"InputService.dll");
    if (!service) {
        wchar_t sysDir[MAX_PATH];
        if (GetSystemDirectoryW(sysDir, ARRAYSIZE(sysDir))) {
            const std::wstring svcPath = std::wstring(sysDir) + L"\\InputService.dll";
            service = LoadLibraryExW(svcPath.c_str(), nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
        }
    }
    if (!service)
        return FALSE;
    wchar_t directory[MAX_PATH];
    const UINT length = GetSystemDirectoryW(directory, ARRAYSIZE(directory));
    if (!length || length >= ARRAYSIZE(directory))
        return FALSE;
    const std::wstring path =
        std::wstring(directory) + L"\\InputMethod\\CHS\\ChsPinyinDS.dll";
    ownedDecoder = LoadLibraryExW(
        path.c_str(), nullptr,
        LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR | LOAD_LIBRARY_SEARCH_DEFAULT_DIRS);
    if (!ownedDecoder)
        return FALSE;
    Wh_SetIntValue(L"native-process-count", 0);
    Wh_SetIntValue(L"native-channel-count", 0);
    Wh_SetIntValue(L"native-vkey-count", 0);
    Wh_SetIntValue(L"native-insert-count", 0);
    Wh_SetIntValue(L"native-chc-opic-count", 0);
    Wh_SetIntValue(L"native-rsbs-count", 0);
    Wh_SetIntValue(L"native-cand-call-count", 0);
    Wh_SetIntValue(L"native-decide-call-count", 0);
    Wh_SetIntValue(L"native-layout-count", 0);
    Wh_SetIntValue(L"native-reg-channel-switched", 0);
    Wh_SetIntValue(L"native-default-channel", 0);
    Wh_SetIntValue(L"native-normalized-count", 0);
    Wh_SetIntValue(L"native-decoded-count", 0);
    Wh_SetIntValue(L"native-service-key-down-count", 0);
    Wh_SetIntValue(L"native-reg-entry-count", 0);
    Wh_SetIntValue(L"native-reg-entry-key", 0);
    const bool readyService = NativeT9::InstallService(service) &&
                              NativeT9::InstallDecoder(ownedDecoder);
    if (!readyService) {
        FreeLibrary(ownedDecoder);
        ownedDecoder = nullptr;
        Wh_Log(L"[中文九键] 原生接口或函数版本校验失败");
    }
    return readyService;
#endif
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
