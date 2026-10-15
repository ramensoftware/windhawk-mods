// ==WindhawkMod==
// @id              touch-keyboard-chinese-t9
// @name            系统触摸键盘中文九宫格
// @description     在 Windows 11 系统触摸键盘中完整接通微软拼音原生十二键/九宫格输入法。
// @version         3.6.0
// @author          NekoMirra
// @github          https://github.com/NekoMirra
// @include         TextInputHost.exe
// @include         ctfmon.exe
// @architecture    x86-64
// @compilerOptions -lruntimeobject -lbcrypt
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# 系统触摸键盘中文九宫格
在 Windows 11 系统触摸键盘中完整接通微软拼音原生十二键/九宫格输入法。
- 采用原生 TSF3 通道与微软拼音原生十二键 Profile，无需第三方输入法；
- 短按产生中文拼音候选词，长按产生数字候选；
- 采用纯原生软件注入，不注入任何模拟硬件键盘事件，输入法进程稳定长驻。
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
#include <winstring.h>
#include <atomic>
#include <string>

namespace {
std::atomic<bool> g_enabled{true};
HMODULE ownedDecoder;
}  // namespace

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
using EnsureChannel = HRESULT (*)(void*, size_t);
using DefaultChannel = HRESULT (*)(void*, bool, size_t*);
using Layout = HRESULT (*)(void*, const NativeString*);
using ProcessStringKey = HRESULT (*)(void*, const void*);
using ChannelHandleStringKey = HRESULT (*)(void*, const void*, bool*, bool*);
using VKeyHandleStringKey = HRESULT (*)(void*, const void*, bool*, bool*);
using InsertVKey = HRESULT (*)(void*, const void*);
using OnChcOpic = HRESULT (*)(void*, int);
using RequestSuggestion = HRESULT (*)(void*, const wchar_t*, unsigned int, void*, void*, bool);
using OnKeyDown = HRESULT (*)(void*, void*, void*);

Regularize originalRegularize;
DecideMode originalDecide;
Candidates originalCandidates;
EnsureChannel originalEnsureChannel;
DefaultChannel originalDefaultChannel;
Layout originalLayout;
ProcessStringKey originalProcess;
ChannelHandleStringKey originalChannelHandle;
VKeyHandleStringKey originalVKeyHandle;
InsertVKey originalInsert;
OnChcOpic originalChcOpic;
RequestSuggestion originalRequestSuggestion;
OnKeyDown originalOnKeyDown;

BYTE* qwertyProfile;
BYTE* twelveProfile;
void* pinyinVtable;

std::atomic<unsigned> normalized{0};
std::atomic<unsigned> decoded{0};
std::atomic<unsigned> serviceKeyDownCount{0};
std::atomic<unsigned> regEntryCount{0};
std::atomic<unsigned> candCallCount{0};
std::atomic<unsigned> decideCallCount{0};
std::atomic<unsigned> layoutCount{0};
std::atomic<unsigned> processCount{0};
std::atomic<unsigned> channelCount{0};
std::atomic<unsigned> vkeyCount{0};
std::atomic<unsigned> insertedCount{0};
std::atomic<unsigned> chcOpicCount{0};
std::atomic<unsigned> rsbsCount{0};

thread_local bool twelveQuery = false;

struct QueryScope {
    QueryScope(BYTE* decoder, bool value)
        : previous(twelveQuery), decoderPtr(decoder), previousProfile(nullptr) {
        twelveQuery = value;
        if (value && decoderPtr && twelveProfile) {
            BYTE** current = reinterpret_cast<BYTE**>(decoderPtr + 57640);
            previousProfile = *current;
            *current = twelveProfile;
            Wh_SetIntValue(L"native-profile-switched", 1);
        }
    }
    ~QueryScope() {
        twelveQuery = previous;
        if (previousProfile && decoderPtr) {
            *reinterpret_cast<BYTE**>(decoderPtr + 57640) = previousProfile;
        }
    }
    bool previous;
    BYTE* decoderPtr;
    BYTE* previousProfile;
};

struct QueryProperties {
    DWORD type;
    DWORD flags;
    DWORD maximum;
    DWORD mode;
    DWORD reserved[20];
};

bool CodeMatches(const void* function, ULONG size, const char* expected) {
    if (!function || size == 0)
        return false;
    BCRYPT_ALG_HANDLE algorithm{};
    NTSTATUS status = BCryptOpenAlgorithmProvider(
        &algorithm, BCRYPT_SHA256_ALGORITHM, nullptr, 0);
    if (status < 0)
        return false;
    BCRYPT_HASH_HANDLE hash{};
    status = BCryptCreateHash(algorithm, &hash, nullptr, 0, nullptr, 0, 0);
    if (status < 0) {
        BCryptCloseAlgorithmProvider(algorithm, 0);
        return false;
    }
    status = BCryptHashData(
        hash, static_cast<UCHAR*>(const_cast<void*>(function)), size, 0);
    UCHAR digest[32]{};
    if (status >= 0)
        status = BCryptFinishHash(hash, digest, sizeof(digest), 0);
    BCryptDestroyHash(hash);
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
    WCHAR layoutName[128]{};
    const bool isTwelveLayout = Wh_GetStringValue(L"native-layout-name", layoutName, ARRAYSIZE(layoutName)) &&
                                wcsstr(layoutName, L"12key") != nullptr;
    if (isTwelveLayout) {
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
    }
    return result;
}

HRESULT DefaultChannelHook(void* self, bool off, size_t* channel) {
    const HRESULT res = originalDefaultChannel(self, off, channel);
    if (!off && channel) {
        WCHAR layoutName[128]{};
        if (Wh_GetStringValue(L"native-layout-name", layoutName, ARRAYSIZE(layoutName)) &&
            wcsstr(layoutName, L"12key") != nullptr) {
            *channel = 4;
            *reinterpret_cast<DWORD*>(static_cast<BYTE*>(self) + 164) = 4;
            Wh_SetIntValue(L"native-default-channel", 4);
        }
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
    WCHAR layoutName[128]{};
    const bool isNarrow12 = Wh_GetStringValue(L"native-layout-name", layoutName, ARRAYSIZE(layoutName)) &&
                            wcsstr(layoutName, L"12key") != nullptr;
    if (!hardware && isNarrow12 && channel != 4 && originalEnsureChannel) {
        originalEnsureChannel(pinyinBase, 4);
        *reinterpret_cast<DWORD*>(static_cast<BYTE*>(pinyinBase) + 164) = 4;
        channel = 4;
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
    WCHAR layoutName[128]{};
    const bool isTwelveLayout = Wh_GetStringValue(L"native-layout-name", layoutName, ARRAYSIZE(layoutName)) &&
                                wcsstr(layoutName, L"12key") != nullptr;
    const bool isTwelve = isTwelveLayout ||
                          (Wh_GetIntValue(L"native-layout-channel", 0) == 4) ||
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
    WCHAR layoutName[128]{};
    const bool isTwelveLayout = Wh_GetStringValue(L"native-layout-name", layoutName, ARRAYSIZE(layoutName)) &&
                                wcsstr(layoutName, L"12key") != nullptr;
    const bool isTwelve = twelveQuery || isTwelveLayout ||
                          (Wh_GetIntValue(L"native-layout-channel", 0) == 4);
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
        void** readingVtbl = *reinterpret_cast<void***>(reading);
        void** managerVtbl = *reinterpret_cast<void***>(manager);
        using ResetFn = HRESULT (*)(void*);
        using AddFn = HRESULT (*)(void*, WORD);
        reinterpret_cast<ResetFn>(readingVtbl[2])(reading);
        reinterpret_cast<AddFn>(readingVtbl[4])(reading, first);
        return reinterpret_cast<AddFn>(managerVtbl[4])(manager, first);
    }
    return originalDecide(self, first);
}

WH_HOOK_SYMBOLS_OPTIONS SymbolOptions() {
    WH_HOOK_SYMBOLS_OPTIONS options{};
    options.optionsSize = sizeof(options);
    options.noUndecoratedSymbols = TRUE;
    options.onlineCacheUrl = L"";
    return options;
}

bool InstallService(HMODULE module) {
    WindhawkUtils::SYMBOL_HOOK symbols[] = {
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
    if (!WindhawkUtils::HookSymbols(module, symbols, ARRAYSIZE(symbols),
                                    &options))
        return false;
    if (!CodeMatches(
            reinterpret_cast<void*>(originalRegularize), 0x915,
            "1c261eef81180b031b2c45155f8be009fa5695015b67e20b33b00e31846175e1"))
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
            reinterpret_cast<void*>(originalOnKeyDown), 0x3cb,
            "bfa7075c2763264c8d50b4ea547537b830d1d784a929ba26a472c67623910385"))
        return false;
    return WindhawkUtils::SetFunctionHook(originalRegularize, RegularizeHook,
                                          &originalRegularize) &&
           WindhawkUtils::SetFunctionHook(originalOnKeyDown, OnKeyDownHook,
                                          &originalOnKeyDown) &&
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
                                          &originalRequestSuggestion);
}

bool InstallDecoder(HMODULE module) {
    WindhawkUtils::SYMBOL_HOOK symbols[] = {
        {{L"?DecideInputMode@ProfileManager@ChsPinyinDS@@IEAAJW4InputType@Mtf@"
          L"2@G@Z"},
         &originalDecide},
        {{L"?CreateCandidates@CandidateManager@ChsPinyinDS@@UEAAJPEAUICandidatesQuery@Mtf@2@PEAPEAUICandidatesList@52@@Z"},
         &originalCandidates},
        {{L"?Qwerty@KeyboardProfile@ChsPinyinDS@@2V12@A"},
         &qwertyProfile},
        {{L"?TwelveKey@KeyboardProfile@ChsPinyinDS@@2V12@A"},
         &twelveProfile},
    };
    auto options = SymbolOptions();
    if (!WindhawkUtils::HookSymbols(module, symbols, ARRAYSIZE(symbols),
                                    &options))
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
using IsAvailableFn = bool (*)(void*, HSTRING, void*, HSTRING, bool*);
IsAvailableFn originalIsAvailable;
using ModeView = void (*)(void*, HSTRING, int);
using ActionFn = void (*)(void*, void*);
using GetPressedKeyHandlingStateFn = int (*)(void*, void*);
using GetKeyDataFn = void* (*)(void*);
using GetVirtualKeyFn = unsigned char (*)(void*);
using InjectKeyWithoutKeyControlFn = void (*)(void*, HSTRING, void*, bool);
using GetKeyTemplateNameFn = HSTRING (*)(void*, void*);
using SetKeyTemplateNameFn = void (*)(void*, void*, HSTRING);
using GetKeyWidthFactorFn = float (*)(void*, void*);
using GetKeyHeightFactorFn = float (*)(void*, void*);
using NotifyStringKeyFn = void (*)(void*, void*, int);
using GetBlockPageCountFn = int (*)(void*);
using InitializeGroupModelFn = void (*)(void*, void*);
using IsSubTypeFn = bool (*)(void*, int);
using GetRecognizedTextFn = void* (*)(void*);
using GetSecondaryLabelFontScalingFn = float (*)(void*);
using IsEmojiViewEnabledFn = bool (*)(void*);

Availability originalAvailability;
ModeView originalMode;
ActionFn originalOnPressAction;
ActionFn originalOnPressAndHoldAction;
ActionFn originalOnReleaseAction;
GetPressedKeyHandlingStateFn originalGetPressedKeyHandlingState;
GetKeyDataFn originalGetKeyData;
GetVirtualKeyFn originalGetVirtualKey;
InjectKeyWithoutKeyControlFn originalInjectKeyWithoutKeyControl;
GetKeyTemplateNameFn originalGetKeyTemplateName;
SetKeyTemplateNameFn originalSetKeyTemplateName;
GetKeyWidthFactorFn originalGetKeyWidthFactor;
GetKeyHeightFactorFn originalGetKeyHeightFactor;
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

bool IsAvailableHook(void* self, HSTRING mode, void* sizeMgr, HSTRING param3, bool* outBool) {
    const bool result = originalIsAvailable(self, mode, sizeMgr, param3, outBool);
    if (g_enabled.load() && IsTwelve(mode)) {
        if (outBool) *outBool = true;
        Wh_SetStringValue(L"native-front-mode", L"narrow12key");
        Wh_SetIntValue(L"native-front-is-available-hooked", 1);
        return true;
    }
    return result;
}

void ModeHook(void* self, HSTRING mode, int view) {
    Wh_SetStringValue(L"native-front-mode",
                      WindowsGetStringRawBuffer(mode, nullptr));
    Wh_SetIntValue(L"native-front-view", view);
    if (view == 1 || view == 4 || view == 0) {
        Wh_SetStringValue(L"native-layout-name", L"zh-cn.narrow12key.alpha.0");
    }
    originalMode(self, mode, view);
}

void OnReleaseActionHook(void* self, void* action) {
    const unsigned count = ++frontReleaseCount;
    Wh_SetIntValue(L"native-front-release-count", count);
    int priorHandlingState = 5;
    if (action) {
        auto* data = static_cast<BYTE*>(action);
        Wh_SetIntValue(L"native-front-release-handled-before", data[144]);
        void* keyCtrl = *reinterpret_cast<void**>(data + 56);
        Wh_SetIntValue(L"native-front-release-key-ctrl", keyCtrl != nullptr ? 1 : 0);
        if (self && keyCtrl && originalGetPressedKeyHandlingState) {
            priorHandlingState = originalGetPressedKeyHandlingState(self, keyCtrl);
        }
        Wh_SetIntValue(L"native-front-release-state", priorHandlingState);
    }

    {
        ReleaseActionScope scope;
        originalOnReleaseAction(self, action);
    }

    if (action) {
        auto* data = static_cast<BYTE*>(action);
        WCHAR layoutName[128]{};
        const bool isTwelve = Wh_GetStringValue(L"native-layout-name", layoutName, ARRAYSIZE(layoutName)) &&
                              wcsstr(layoutName, L"12key") != nullptr;
        void* keyCtrl = *reinterpret_cast<void**>(data + 56);
        if (isTwelve && priorHandlingState == 1) {
            data[144] = 1;
            Wh_SetIntValue(L"native-front-release-suppressed-long", 1);
        } else if (isTwelve && priorHandlingState == 0 && data[144] == 0 && keyCtrl && originalGetKeyData) {
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

HSTRING GetKeyTemplateNameHook(void* self, void* keyData) {
    HSTRING result = originalGetKeyTemplateName(self, keyData);
    if (!keyData || !originalGetVirtualKey)
        return result;
    const unsigned char vk = originalGetVirtualKey(keyData);
    if (vk >= 0x62 && vk <= 0x69) {
        WCHAR layoutName[128]{};
        if (Wh_GetStringValue(L"native-layout-name", layoutName, ARRAYSIZE(layoutName)) &&
            wcsstr(layoutName, L"12key") != nullptr) {
            HSTRING fixedTemplate = nullptr;
            if (SUCCEEDED(WindowsCreateString(
                    L"ModernUX2TwelveKeySecondaryLabelKeyStyle", 40,
                    &fixedTemplate))) {
                if (originalSetKeyTemplateName)
                    originalSetKeyTemplateName(self, keyData, fixedTemplate);
                Wh_SetIntValue(L"native-front-template-fixed", 1);
                return fixedTemplate;
            }
        }
    }
    return result;
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

void InitializeGroupModelHook(void* self, void* param) {
    originalInitializeGroupModel(self, param);
}

bool IsSubTypeHook(void* self, int subType) {
    if (subType == 2) {
        Wh_SetIntValue(L"native-front-subtype2-intercepted", 1);
        return false;
    }
    return originalIsSubType(self, subType);
}

void* GetRecognizedTextHook(void* self) {
    return originalGetRecognizedText(self);
}

bool InstallFrontend(HMODULE module) {
    WindhawkUtils::SYMBOL_HOOK symbols[] = {
        {{L"?IsNarrowModeAvailableInternal@"
          L"ChineseSimplifiedModeAvailabilityStrategy@Controls@"
          L"TextInputUndocked@Experiences@ComposableShell@WindowsInternal@@"
          L"AEAA_NPE$AAVString@Platform@@@Z"},
         &originalAvailability,
         AvailabilityHook},
        {{L"?IsAvailable@ChineseSimplifiedModeAvailabilityStrategy@Controls@"
          L"TextInputUndocked@Experiences@ComposableShell@WindowsInternal@@"
          L"UEAA_NPE$AAVString@Platform@@PE$AAUIInputPanelSizePositionManager@"
          L"TouchKeyboardUX@@0PEA_N@Z"},
         &originalIsAvailable,
         IsAvailableHook},
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
        {{L"?GetPressedKeyHandlingState@WideNarrowChineseSimplifiedKeyActionHandler@"
          L"Controls@TextInputUndocked@Experiences@ComposableShell@WindowsInternal@@"
          L"AEAA?AW4PressedKeyHandlingState@12345@PE$AAVIKeyboardKeyControl@LayoutData@"
          L"Common@TextInput@45@@Z"},
         &originalGetPressedKeyHandlingState},
        {{L"?GetKeyData@IKeyboardKeyControl@LayoutData@Common@TextInput@"
          L"Experiences@ComposableShell@WindowsInternal@@UE$AAAPE$AAVIKeyboardKeyData@"
          L"23456@XZ"},
         &originalGetKeyData},
        {{L"?get_VirtualKey@IKeyboardKeyData@LayoutData@Common@TextInput@"
          L"Experiences@ComposableShell@WindowsInternal@@UE$AAAEXZ"},
         &originalGetVirtualKey},
        {{L"?InjectKeyWithoutKeyControl@WideNarrowChineseSimplifiedKeyActionHandler@"
          L"Controls@TextInputUndocked@Experiences@ComposableShell@WindowsInternal@@"
          L"IEAAXPE$AAVString@Platform@@PE$AAVIKeyboardKeyData@LayoutData@Common@"
          L"TextInput@345@_N@Z"},
         &originalInjectKeyWithoutKeyControl},
        {{L"?get_KeyTemplateName@IKeyboardKeyControl@LayoutData@Common@TextInput@"
          L"Experiences@ComposableShell@WindowsInternal@@UE$AAAPE$AAVString@Platform@@XZ"},
         &originalGetKeyTemplateName,
         GetKeyTemplateNameHook},
        {{L"?put_KeyTemplateName@IKeyboardKeyControl@LayoutData@Common@TextInput@"
          L"Experiences@ComposableShell@WindowsInternal@@UE$AAAXPE$AAVString@Platform@@@Z"},
         &originalSetKeyTemplateName},
        {{L"?Initialize@KeyboardGridGroupModel@Controls@TextInputUndocked@"
          L"Experiences@ComposableShell@WindowsInternal@@QEAAXPE$AAVKeyboardGrid@"
          L"LayoutData@Common@TextInput@345@@Z"},
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
    auto options = NativeT9::SymbolOptions();
    return WindhawkUtils::HookSymbols(module, symbols, ARRAYSIZE(symbols),
                                      &options);
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
        360);
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
        Wh_SetIntValue(L"native-front-inj-key-vk", -1);
        Wh_SetIntValue(L"native-front-inj-key-char", 0);
        Wh_SetIntValue(L"native-front-inj-key-flags", 0);
        Wh_SetIntValue(L"native-front-inj-key-dir", -1);
        Wh_SetIntValue(L"native-front-inj-key-height", -1);
        Wh_SetIntValue(L"native-front-inj-key-count", 0);
        Wh_SetIntValue(L"native-front-inject-kc-a4", 0);
        Wh_SetIntValue(L"native-front-inject-kc-has-data", 0);
        Wh_SetIntValue(L"native-front-inject-kc-has-str", 0);
        Wh_SetIntValue(L"native-front-inject-kc-count", 0);
        Wh_SetIntValue(L"native-front-nsk-key-in", -1);
        Wh_SetIntValue(L"native-front-nsk-char", 0);
        Wh_SetIntValue(L"native-front-nsk-qual", 0);
        Wh_SetIntValue(L"native-front-nsk-key-fixed", 0);
        Wh_SetIntValue(L"native-front-nsk-count", 0);
        Wh_SetIntValue(L"native-front-numeric-direct-injected", 0);
        Wh_SetIntValue(L"native-front-release-suppressed-raw", 0);
        Wh_SetIntValue(L"native-front-release-suppressed-long", 0);
        const bool readyFront = InstallFrontend(front);
        if (!readyFront) {
            Wh_Log(L"[中文九键] 前端接口挂钩失败");
        }
        return readyFront;
    }

    HMODULE service = GetModuleHandleW(L"InputService.dll");
    if (!service) {
        service = LoadLibraryW(L"InputService.dll");
    }
    if (!service) {
        return FALSE;
    }
    ownedDecoder = LoadLibraryW(L"ChsPinyinDS.dll");
    if (!ownedDecoder) {
        return FALSE;
    }
    Wh_SetIntValue(L"native-normalized-count", 0);
    Wh_SetIntValue(L"native-decoded-count", 0);
    const bool readyService = NativeT9::InstallService(service) &&
                              NativeT9::InstallDecoder(ownedDecoder);
    if (!readyService) {
        FreeLibrary(ownedDecoder);
        ownedDecoder = nullptr;
        Wh_Log(L"[中文九键] 原生接口或函数版本校验失败");
    }
    return readyService;
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
