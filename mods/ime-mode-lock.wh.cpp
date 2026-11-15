// ==WindhawkMod==
// @id              ime-mode-lock
// @name            IME Native Mode Lock
// @description     Forces supported IMEs to stay in their native input mode, preventing accidental mode switches.
// @version         0.1
// @author          ZeonXr
// @github          https://github.com/ZeonXr
// @include         *
// @compilerOptions -limm32
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# IME Native Mode Lock

Forces supported IMEs to stay in their native input mode, blocking
both system-initiated and user-initiated mode switches.

By default, Windows automatically switches input modes when changing
window focus or switching between applications. This mod blocks that
behavior, keeping your IME in its native mode at all times.

## Supported IMEs
- **Simplified Chinese** (Microsoft Pinyin, etc.) — always stays in Chinese mode
- More languages can be added in future updates

**Pair with** Caps IME Switcher (`caps-ime-switcher`) — short-press
Caps Lock to switch input languages, long-press for Caps Lock.

---

强制支持的输入法保持在原生输入模式，同时阻止系统自动切换和用户主动切换。

默认情况下，Windows 在切换窗口焦点或应用程序时会自动切换输入模式。
此 mod 阻止该行为，始终保持输入法在原生输入模式。

## 支持的输入法
- **简体中文**（微软拼音等）— 始终保持中文模式
- 未来可添加更多语言支持

**推荐搭配** Caps IME Switcher (`caps-ime-switcher`) — 可使用 Caps Lock
短按切换输入语言，长按触发大小写锁定，配合本 mod 体验更佳。
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- ForceChineseNativeMode: false
  $name: Keep Simplified Chinese IMEs in Chinese mode
  $description: Automatically switch Simplified Chinese IMEs (e.g. Microsoft Pinyin) to Chinese input mode when activated or when they spontaneously switch to English mode
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <imm.h>

#include <windhawk_api.h>
#include <windhawk_utils.h>

#include <atomic>

#ifndef IMC_SETCONVERSIONMODE
#define IMC_SETCONVERSIONMODE 0x0002
#endif

#ifndef IMN_SETCONVERSIONMODE
#define IMN_SETCONVERSIONMODE 0x0006
#endif

struct ImeModeProfile {
    PCWSTR settingsKey;
    LANGID langId;
    DWORD  conversionMode;
};

constexpr ImeModeProfile kImeModeProfiles[] = {
    {L"ForceChineseNativeMode", 0x0804, 0x0001},   // IME_CMODE_CHINESE
    // Future: {L"ForceJapaneseNativeMode", 0x0411, 0x0001},   // IME_CMODE_HIRAGANA
    // Future: {L"ForceKoreanNativeMode",   0x0412, 0x0001},   // IME_CMODE_HANGUL
};
constexpr size_t kImeModeProfileCount = ARRAYSIZE(kImeModeProfiles);

std::atomic<bool> g_profileEnabled[kImeModeProfileCount];
std::atomic<UINT> g_msuimPrivateMessage{0};

using DispatchMessageA_t = decltype(&DispatchMessageA);
using DispatchMessageW_t = decltype(&DispatchMessageW);
DispatchMessageA_t DispatchMessageA_Original = nullptr;
DispatchMessageW_t DispatchMessageW_Original = nullptr;

LRESULT WINAPI DispatchMessageA_Hook(const MSG* msg);
LRESULT WINAPI DispatchMessageW_Hook(const MSG* msg);
bool    AnyProfileEnabled();

void InitializeMsuimMessage() {
    if (g_msuimPrivateMessage.load(std::memory_order_acquire)) {
        return;
    }
    g_msuimPrivateMessage.store(
        RegisterWindowMessageW(L"MSUIM.Msg.Private"),
        std::memory_order_release);
}

void LoadSettings() {
    InitializeMsuimMessage();

    for (size_t i = 0; i < kImeModeProfileCount; ++i) {
        bool enabled =
            Wh_GetIntSetting(kImeModeProfiles[i].settingsKey) != 0;
        g_profileEnabled[i].store(enabled, std::memory_order_relaxed);
    }
}

bool AnyProfileEnabled() {
    for (size_t i = 0; i < kImeModeProfileCount; ++i) {
        if (g_profileEnabled[i].load(std::memory_order_relaxed)) {
            return true;
        }
    }
    return false;
}

bool IsProfileEnabled(size_t profileIndex) {
    if (profileIndex >= kImeModeProfileCount) {
        return false;
    }
    return g_profileEnabled[profileIndex].load(std::memory_order_relaxed);
}

LANGID GetKeyboardLayoutLangId(HKL keyboardLayout) {
    return static_cast<LANGID>(reinterpret_cast<UINT_PTR>(keyboardLayout) &
                               0xFFFF);
}

const ImeModeProfile* FindMatchingProfile(HKL keyboardLayout) {
    LANGID currentLangId = GetKeyboardLayoutLangId(keyboardLayout);

    for (size_t i = 0; i < kImeModeProfileCount; ++i) {
        if (IsProfileEnabled(i) &&
            currentLangId == kImeModeProfiles[i].langId) {
            return &kImeModeProfiles[i];
        }
    }
    return nullptr;
}

HWND GetInputTargetWindow() {
    HWND foregroundWindow = GetForegroundWindow();
    if (!foregroundWindow) {
        return nullptr;
    }

    DWORD foregroundThreadId =
        GetWindowThreadProcessId(foregroundWindow, nullptr);
    if (!foregroundThreadId) {
        return foregroundWindow;
    }

    GUITHREADINFO guiThreadInfo = {sizeof(guiThreadInfo)};
    if (GetGUIThreadInfo(foregroundThreadId, &guiThreadInfo) &&
        guiThreadInfo.hwndFocus) {
        return guiThreadInfo.hwndFocus;
    }

    return foregroundWindow;
}

void ApplyNativeImeMode(HWND preferredTargetWindow = nullptr) {
    HWND targetWindow = preferredTargetWindow ? preferredTargetWindow
                                              : GetInputTargetWindow();
    if (!targetWindow) {
        return;
    }

    DWORD targetThreadId =
        GetWindowThreadProcessId(targetWindow, nullptr);
    HKL keyboardLayout =
        GetKeyboardLayout(targetThreadId ? targetThreadId : 0);

    const ImeModeProfile* profile = FindMatchingProfile(keyboardLayout);
    if (!profile) {
        return;
    }

    DWORD desiredConversionMode = profile->conversionMode;
    if (!desiredConversionMode) {
        return;
    }

    HWND imeWindow = ImmGetDefaultIMEWnd(targetWindow);
    if (!imeWindow) {
        return;
    }

    DWORD_PTR ignoredResult = 0;
    SendMessageTimeoutW(imeWindow, WM_IME_CONTROL, IMC_SETCONVERSIONMODE,
                        desiredConversionMode, SMTO_ABORTIFHUNG, 100,
                        &ignoredResult);
    SendMessageTimeoutW(imeWindow, WM_IME_NOTIFY, IMN_SETCONVERSIONMODE, 0,
                        SMTO_ABORTIFHUNG, 100, &ignoredResult);
}

bool ShouldApplyNativeMode(const MSG* msg) {
    if (!msg) {
        return false;
    }

    UINT msuimMsg = g_msuimPrivateMessage.load(std::memory_order_acquire);
    if (msuimMsg && msg->message == msuimMsg) {
        return true;
    }

    if (msg->message == WM_IME_NOTIFY &&
        msg->wParam == IMN_SETCONVERSIONMODE) {
        return true;
    }

    if (msg->message == WM_INPUTLANGCHANGE) {
        return true;
    }

    return false;
}

LRESULT WINAPI DispatchMessageA_Hook(const MSG* msg) {
    LRESULT result = DispatchMessageA_Original(msg);
    if (ShouldApplyNativeMode(msg)) {
        ApplyNativeImeMode(msg->hwnd);
    }
    return result;
}

LRESULT WINAPI DispatchMessageW_Hook(const MSG* msg) {
    LRESULT result = DispatchMessageW_Original(msg);
    if (ShouldApplyNativeMode(msg)) {
        ApplyNativeImeMode(msg->hwnd);
    }
    return result;
}

BOOL InstallHooks() {
    if (!Wh_SetFunctionHook(
            reinterpret_cast<void*>(DispatchMessageA),
            reinterpret_cast<void*>(DispatchMessageA_Hook),
            reinterpret_cast<void**>(&DispatchMessageA_Original))) {
        Wh_Log(L"Failed to hook DispatchMessageA");
        return FALSE;
    }

    if (!Wh_SetFunctionHook(
            reinterpret_cast<void*>(DispatchMessageW),
            reinterpret_cast<void*>(DispatchMessageW_Hook),
            reinterpret_cast<void**>(&DispatchMessageW_Original))) {
        Wh_Log(L"Failed to hook DispatchMessageW");
        return FALSE;
    }

    return TRUE;
}

BOOL Wh_ModInit() {
    DWORD sessionId;
    if (ProcessIdToSessionId(GetCurrentProcessId(), &sessionId) &&
        sessionId == 0) {
        return FALSE;
    }

    LoadSettings();

    if (!AnyProfileEnabled()) {
        return FALSE;
    }

    return InstallHooks();
}

void Wh_ModSettingsChanged() {
    LoadSettings();
}
