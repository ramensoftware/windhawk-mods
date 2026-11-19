// ==WindhawkMod==
// @id              text-capitalization
// @name            Text Capitalization
// @description     Change selected text with five case modes and customizable keyboard shortcuts.
// @version         0.3.1
// @author          rockinggreninja-bot
// @github          https://github.com/rockinggreninja-bot
// @include         windhawk.exe
// @compilerOptions -lole32 -loleaut32 -lgdi32 -luser32 -luiautomationcore -lshcore
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Text Capitalization

Select text and press Ctrl+Alt+P to capture the selection and open a compact picker. The selected text is shown in a read-only preview; Up/Down changes the highlighted case and updates the preview. Enter or clicking a case replaces the original selection with the transformed text. Escape cancels.

Default shortcuts are Ctrl+Alt+P (open picker), Ctrl+Alt+C (apply saved case immediately), Up/Down (navigate), Enter (apply highlighted case), and Escape (cancel). Shortcuts are configurable in Windhawk settings. On older Windhawk versions enter numeric VK values such as ctrl+alt+80 for Ctrl+Alt+P.

The picker never takes keyboard focus from the original editor. Selection capture and replacement use a guarded clipboard copy/paste workflow instead of depending on UI Automation selection detection. The clipboard is snapshotted and restoration is best-effort. The mode is remembered in Windhawk local storage.

Compatibility depends on the target app supporting copy and paste and leaving the original selection intact while the non-activating picker is shown. Elevated apps may reject simulated input if Windhawk/Explorer runs at a lower integrity level.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- OpenPickerHotkey: "ctrl+alt+80"
  $name: Open picker from selected text
  $description: Copy the current selection into the read-only preview and open the case picker. On older Windhawk versions, enter ctrl+alt+80 (Ctrl+Alt+P).
  #! $format: hotkey
- ApplyHotkey: "ctrl+alt+67"
  $name: Apply saved case immediately
  $description: Apply the saved case to the current selection without opening the picker. On older Windhawk versions, enter ctrl+alt+67 (Ctrl+Alt+C).
  #! $format: hotkey
- PreviousModeHotkey: "38"
  $name: Previous mode
  $description: Move the highlight up while the picker is open.
  #! $format: hotkey
- NextModeHotkey: "40"
  $name: Next mode
  $description: Move the highlight down while the picker is open.
  #! $format: hotkey
- ConfirmModeHotkey: "13"
  $name: Apply highlighted case
  $description: Replace the original selection with the highlighted case and close the picker.
  #! $format: hotkey
- CancelPickerHotkey: "27"
  $name: Cancel picker
  $description: Close the picker without saving the highlighted mode.
  #! $format: hotkey
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <windowsx.h>
#include <ole2.h>
#include <oleauto.h>
#include <UIAutomation.h>
#include <shellscalingapi.h>
#include <string>
#include <vector>
#include <algorithm>
#include <cwctype>
#include <cwchar>
#include <cstring>
#include <new>

namespace {

enum class CaseMode : int {
    Title = 0,
    Sentence = 1,
    Upper = 2,
    Lower = 3,
    Toggle = 4,
};

struct HotkeyBinding {
    UINT vk = 0;
    UINT modifiers = 0;
};

constexpr int kModeCount = 5;
// Define the UI Automation coclass CLSID locally. Some Windhawk linker
// configurations do not resolve the external CLSID symbol from the SDK.
const CLSID kCuiAutomationClsid = {
    0xFF48DBA4, 0x60EF, 0x4201,
    {0xAA, 0x87, 0x54, 0x10, 0x3E, 0xEF, 0x59, 0x4E}
};
constexpr UINT WM_APP_SHOW_PICKER = WM_APP + 41;
constexpr UINT WM_APP_CLOSE_PICKER = WM_APP + 43;
constexpr UINT WM_APP_MOVE_HIGHLIGHT = WM_APP + 44;
constexpr UINT WM_APP_COMMIT_MODE = WM_APP + 45;
constexpr UINT WM_APP_APPLY = WM_APP + 46;
constexpr UINT WM_APP_SHUTDOWN = WM_APP + 47;
constexpr UINT WM_APP_RELOAD_SHORTCUTS = WM_APP + 48;
constexpr UINT WM_APP_CAPTURE_READY = WM_APP + 49;
constexpr UINT_PTR kPickerTimer = 77;
constexpr int kPickerWidth = 300;
constexpr int kPickerHeight = 338;
constexpr int kPreviewTop = 10;
constexpr int kPreviewHeight = 78;
constexpr int kRowTop = 98;
constexpr int kRowHeight = 46;
constexpr wchar_t kMessageClass[] = L"TextCapitalizationMessageWindow_8F31A9";
constexpr wchar_t kPickerClass[] = L"TextCapitalizationPickerWindow_8F31A9";

const wchar_t* const kModeNames[kModeCount] = {
    L"Title Case",
    L"Sentence case",
    L"UPPERCASE",
    L"lowercase",
    L"Toggle Case",
};



HINSTANCE g_modInstance = nullptr;
int g_dpi = 96;
HMODULE g_modModule = nullptr;
HANDLE g_threadHandle = nullptr;
HANDLE g_readyEvent = nullptr;
HANDLE g_applyThreadHandle = nullptr;
HANDLE g_captureThreadHandle = nullptr;
SRWLOCK g_lifecycleLock = SRWLOCK_INIT;
DWORD g_threadId = 0;
HWND g_messageWindow = nullptr;
HWND g_pickerWindow = nullptr;
HHOOK g_keyboardHook = nullptr;
HHOOK g_mouseHook = nullptr;
IUIAutomation2* g_uiAutomation = nullptr;

HotkeyBinding g_openPickerHotkey;
HotkeyBinding g_applyHotkey;
HotkeyBinding g_previousModeHotkey;
HotkeyBinding g_nextModeHotkey;
HotkeyBinding g_confirmModeHotkey;
HotkeyBinding g_cancelPickerHotkey;

volatile LONG g_threadReadyState = 0;
volatile LONG g_shuttingDown = 0;
volatile LONG g_popupOpen = 0;
volatile LONG g_applyInProgress = 0;
volatile LONG g_captureInProgress = 0;
volatile LONG g_consumedVk = 0;
// Updated by the low-level hook from modifier key down/up events.
LONG g_heldModifierFlags = 0;
volatile LONG g_activeMode = static_cast<LONG>(CaseMode::Title);
PVOID volatile g_originForeground = nullptr;
PVOID volatile g_originFocus = nullptr;

int g_highlightedMode = static_cast<int>(CaseMode::Title);
std::wstring g_selectedText;
HFONT g_bodyFont = nullptr;

struct ApplyRequest {
    HWND foreground;
    HWND focus;
    CaseMode mode;
    bool fromPicker;
    std::wstring selectedText;
};

struct UiActionRequest {
    HWND foreground;
    HWND focus;
    CaseMode mode;
    bool fromPicker;
};

struct SelectionCaptureRequest {
    HWND foreground;
    HWND focus;
};

struct SelectionCaptureResult {
    HWND foreground;
    HWND focus;
    bool success;
    std::wstring selectedText;
};

struct ClipboardSnapshot {
    bool wasEmpty = false;
    IDataObject* dataObject = nullptr;
};

int DpiScale(int value) {
    return MulDiv(value, g_dpi, 96);
}

bool IsValidMode(int value) {
    return value >= 0 && value < kModeCount;
}

CaseMode GetActiveMode() {
    LONG value = InterlockedCompareExchange(&g_activeMode, 0, 0);
    return static_cast<CaseMode>(IsValidMode(value) ? value : 0);
}

void PersistMode(CaseMode mode) {
    int value = static_cast<int>(mode);
    if (!IsValidMode(value)) {
        return;
    }
    InterlockedExchange(&g_activeMode, value);
    Wh_SetIntValue(L"ActiveMode", value);
}

bool CaptureFocusWindow(HWND foreground, HWND* focusOut) {
    if (!foreground || !IsWindow(foreground) || !focusOut) {
        return false;
    }

    DWORD threadId = GetWindowThreadProcessId(foreground, nullptr);
    if (!threadId) {
        return false;
    }

    GUITHREADINFO info = {};
    info.cbSize = sizeof(info);
    if (!GetGUIThreadInfo(threadId, &info) || !info.hwndFocus ||
        !IsWindow(info.hwndFocus)) {
        return false;
    }

    *focusOut = info.hwndFocus;
    return true;
}

bool IsCurrentFocus(HWND expectedForeground, HWND expectedFocus) {
    if (!expectedForeground || !expectedFocus ||
        !IsWindow(expectedForeground) || !IsWindow(expectedFocus) ||
        GetForegroundWindow() != expectedForeground) {
        return false;
    }

    HWND currentFocus = nullptr;
    if (!CaptureFocusWindow(expectedForeground, &currentFocus)) {
        return false;
    }

    return currentFocus == expectedFocus;
}

std::wstring MapWholeString(const std::wstring& input, DWORD mapFlags) {
    if (input.empty()) {
        return input;
    }

    int required = LCMapStringEx(
        LOCALE_NAME_USER_DEFAULT, mapFlags, input.c_str(),
        static_cast<int>(input.size()), nullptr, 0, nullptr, nullptr, 0);
    if (required <= 0) {
        return input;
    }

    std::wstring output(static_cast<size_t>(required), L'\0');
    int written = LCMapStringEx(
        LOCALE_NAME_USER_DEFAULT, mapFlags, input.c_str(),
        static_cast<int>(input.size()), output.data(), required,
        nullptr, nullptr, 0);
    if (written <= 0) {
        return input;
    }

    output.resize(static_cast<size_t>(written));
    return output;
}

bool IsAlphaOrDigit(wchar_t ch) {
    WORD type = 0;
    if (GetStringTypeW(CT_CTYPE1, &ch, 1, &type)) {
        return (type & (C1_ALPHA | C1_DIGIT)) != 0;
    }
    return iswalnum(ch) != 0;
}

bool IsLetter(wchar_t ch) {
    WORD type = 0;
    if (GetStringTypeW(CT_CTYPE1, &ch, 1, &type)) {
        return (type & C1_ALPHA) != 0;
    }
    return iswalpha(ch) != 0;
}

std::wstring MapOneCharacter(const std::wstring& input, DWORD mapFlags) {
    if (input.empty()) {
        return input;
    }
    return MapWholeString(input, mapFlags);
}

std::wstring TransformText(const std::wstring& input, CaseMode mode) {
    switch (mode) {
        case CaseMode::Upper:
            return MapWholeString(input, LCMAP_UPPERCASE);
        case CaseMode::Lower:
            return MapWholeString(input, LCMAP_LOWERCASE);
        case CaseMode::Title: {
            std::wstring output = MapWholeString(input, LCMAP_LOWERCASE);
            bool wordStart = true;
            for (size_t i = 0; i < output.size(); ++i) {
                wchar_t ch = output[i];
                bool apostropheInsideWord =
                    (ch == L'\'' || ch == L'\u2019') && i > 0 &&
                    i + 1 < output.size() && IsAlphaOrDigit(output[i - 1]) &&
                    IsAlphaOrDigit(output[i + 1]);

                if (IsAlphaOrDigit(ch)) {
                    if (wordStart) {
                        std::wstring one(1, ch);
                        output.replace(i, 1, MapOneCharacter(one, LCMAP_UPPERCASE));
                    }
                    wordStart = false;
                } else if (apostropheInsideWord) {
                    wordStart = false;
                } else {
                    wordStart = true;
                }
            }
            return output;
        }
        case CaseMode::Sentence: {
            std::wstring output = MapWholeString(input, LCMAP_LOWERCASE);
            bool sentenceStart = true;
            for (size_t i = 0; i < output.size(); ++i) {
                wchar_t ch = output[i];
                if (sentenceStart && IsLetter(ch)) {
                    std::wstring one(1, ch);
                    output.replace(i, 1, MapOneCharacter(one, LCMAP_UPPERCASE));
                    sentenceStart = false;
                }
                if (ch == L'.' || ch == L'!' || ch == L'?' || ch == L'\r' || ch == L'\n') {
                    sentenceStart = true;
                }
            }
            return output;
        }
        case CaseMode::Toggle: {
            std::wstring output;
            output.reserve(input.size());
            for (size_t i = 0; i < input.size();) {
                size_t charLength = 1;
                wchar_t ch = input[i];
                if (ch >= 0xD800 && ch <= 0xDBFF && i + 1 < input.size() &&
                    input[i + 1] >= 0xDC00 && input[i + 1] <= 0xDFFF) {
                    charLength = 2;
                }
                std::wstring one = input.substr(i, charLength);
                std::wstring upper = MapOneCharacter(one, LCMAP_UPPERCASE);
                std::wstring lower = MapOneCharacter(one, LCMAP_LOWERCASE);
                if (upper == lower) {
                    output += one;
                } else if (one == lower) {
                    output += upper;
                } else {
                    output += lower;
                }
                i += charLength;
            }
            return output;
        }
        default:
            return input;
    }
}

bool GetWindowClassName(HWND hwnd, wchar_t* className, int charCount) {
    return hwnd && className && charCount > 0 &&
           GetClassNameW(hwnd, className, charCount) > 0;
}

bool IsStandardEditControl(HWND hwnd) {
    wchar_t className[64] = {};
    return GetWindowClassName(hwnd, className, ARRAYSIZE(className)) &&
           lstrcmpiW(className, L"Edit") == 0;
}

bool HasNativeEditSelection(HWND focus, bool* recognized) {
    if (recognized) {
        *recognized = false;
    }
    if (!focus || !IsWindow(focus)) {
        return false;
    }

    if (IsStandardEditControl(focus)) {
        if (recognized) {
            *recognized = true;
        }
        LONG_PTR style = GetWindowLongPtrW(focus, GWL_STYLE);
        if ((style & ES_READONLY) || (style & ES_PASSWORD)) {
            return false;
        }
        DWORD start = 0;
        DWORD end = 0;
        SendMessageW(focus, EM_GETSEL, reinterpret_cast<WPARAM>(&start),
                     reinterpret_cast<LPARAM>(&end));
        return end > start;
    }

    return false;
}

bool CreateAutomationClient(IUIAutomation2** automationOut) {
    if (!automationOut) {
        return false;
    }
    *automationOut = nullptr;
    IUIAutomation* base = nullptr;
    HRESULT hr = CoCreateInstance(kCuiAutomationClsid, nullptr,
                                  CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&base));
    if (FAILED(hr) || !base) {
        return false;
    }
    hr = base->QueryInterface(IID_PPV_ARGS(automationOut));
    base->Release();
    if (FAILED(hr) || !*automationOut) {
        *automationOut = nullptr;
        return false;
    }
    // Keep this check on a low-level keyboard hook responsive if a UIA provider
    // is hung or unusually slow. Unsupported controls safely return false.
    (*automationOut)->put_ConnectionTimeout(100);
    (*automationOut)->put_TransactionTimeout(100);
    return true;
}

bool HasEditableSelectionOnElement(IUIAutomationElement* element,
                                   bool* recognizedTextControl) {
    if (recognizedTextControl) {
        *recognizedTextControl = false;
    }
    if (!element) {
        return false;
    }

    VARIANT controlType;
    VariantInit(&controlType);
    HRESULT typeHr = element->GetCurrentPropertyValue(UIA_ControlTypePropertyId,
                                                      &controlType);
    if (FAILED(typeHr) || controlType.vt != VT_I4) {
        VariantClear(&controlType);
        return false;
    }

    const bool isEdit = controlType.lVal == UIA_EditControlTypeId;
    const bool isDocument = controlType.lVal == UIA_DocumentControlTypeId;
    VariantClear(&controlType);
    if (!isEdit && !isDocument) {
        return false;
    }
    // Do not stop the ancestor walk merely because this node is classified as
    // Edit/Document. Some apps expose a focused Edit node without TextPattern
    // and put the actual selection provider on a parent Document node.
    // Mark a node as the text provider only when we have evidence it can expose
    // selection data (or it explicitly reports that it is read-only).

    VARIANT valueReadOnly;
    VariantInit(&valueReadOnly);
    HRESULT valueReadOnlyHr = element->GetCurrentPropertyValue(
        UIA_ValueIsReadOnlyPropertyId, &valueReadOnly);
    const bool valueReadOnlyKnown = SUCCEEDED(valueReadOnlyHr) &&
                                    valueReadOnly.vt == VT_BOOL;
    const bool valueIsReadOnly = valueReadOnlyKnown &&
                                 valueReadOnly.boolVal == VARIANT_TRUE;
    VariantClear(&valueReadOnly);
    if (valueIsReadOnly) {
        if (recognizedTextControl) {
            *recognizedTextControl = true;
        }
        return false;
    }

    IUIAutomationTextPattern* textPattern = nullptr;
    HRESULT hr = element->GetCurrentPatternAs(UIA_TextPatternId,
                                               IID_PPV_ARGS(&textPattern));
    if (FAILED(hr) || !textPattern) {
        // Let HasUiaEditableTextSelection continue to a parent provider.
        return false;
    }
    if (recognizedTextControl) {
        *recognizedTextControl = true;
    }

    IUIAutomationTextRangeArray* ranges = nullptr;
    hr = textPattern->GetSelection(&ranges);
    textPattern->Release();
    if (FAILED(hr) || !ranges) {
        return false;
    }

    int rangeCount = 0;
    bool hasEditableSelection = false;
    if (SUCCEEDED(ranges->get_Length(&rangeCount))) {
        for (int i = 0; i < rangeCount && !hasEditableSelection; ++i) {
            IUIAutomationTextRange* range = nullptr;
            if (FAILED(ranges->GetElement(i, &range)) || !range) {
                continue;
            }

            int compare = 0;
            const bool nonEmpty = SUCCEEDED(range->CompareEndpoints(
                TextPatternRangeEndpoint_Start, range,
                TextPatternRangeEndpoint_End, &compare)) && compare != 0;
            if (nonEmpty) {
                VARIANT textReadOnly;
                VariantInit(&textReadOnly);
                HRESULT attrHr = range->GetAttributeValue(
                    UIA_IsReadOnlyAttributeId, &textReadOnly);
                if (SUCCEEDED(attrHr) && textReadOnly.vt == VT_BOOL) {
                    // This attribute describes the selected text range itself.
                    hasEditableSelection = textReadOnly.boolVal != VARIANT_TRUE;
                } else if (isEdit && valueReadOnlyKnown) {
                    // Some Edit providers do not implement this text attribute.
                    // An explicit non-read-only Value property is an acceptable
                    // fallback for Edit controls, but not for whole Documents.
                    hasEditableSelection = !valueIsReadOnly;
                }
                VariantClear(&textReadOnly);
            }
            range->Release();
        }
    }

    ranges->Release();
    return hasEditableSelection;
}

bool HasUiaEditableTextSelection(IUIAutomation2* automation) {
    if (!automation) {
        return false;
    }

    IUIAutomationElement* current = nullptr;
    HRESULT hr = automation->GetFocusedElement(&current);
    if (FAILED(hr) || !current) {
        return false;
    }

    IUIAutomationTreeWalker* walker = nullptr;
    automation->get_ControlViewWalker(&walker);

    bool selected = false;
    // The focused UIA element can be a hosting/bridge node. Walk up a few
    // Control View ancestors to find its actual Edit or Document text provider.
    for (int depth = 0; current && depth < 6; ++depth) {
        bool recognizedTextControl = false;
        selected = HasEditableSelectionOnElement(current, &recognizedTextControl);
        if (recognizedTextControl) {
            // If the nearest editable text provider has no selection, do not
            // fall through to a parent Document that might represent page text.
            break;
        }

        if (!walker) {
            break;
        }
        IUIAutomationElement* parent = nullptr;
        if (FAILED(walker->GetParentElement(current, &parent)) || !parent) {
            break;
        }
        current->Release();
        current = parent;
    }

    if (current) {
        current->Release();
    }
    if (walker) {
        walker->Release();
    }
    return selected;
}

bool HasEditableTextSelection(HWND foreground, HWND focus,
                              IUIAutomation2* automation) {
    if (!IsCurrentFocus(foreground, focus)) {
        return false;
    }

    bool recognizedNativeControl = false;
    bool nativeSelection = HasNativeEditSelection(focus, &recognizedNativeControl);
    if (recognizedNativeControl) {
        return nativeSelection;
    }

    bool selected = HasUiaEditableTextSelection(automation);
    return selected && IsCurrentFocus(foreground, focus);
}

HotkeyBinding ParseHotkeySetting(PCWSTR settingName) {
    HotkeyBinding result = {};
    PCWSTR value = Wh_GetStringSetting(settingName);
    if (!value || !*value) {
        if (value) {
            Wh_FreeStringSetting(value);
        }
        return result;
    }

    if (wcsstr(value, L"ctrl+")) result.modifiers |= MOD_CONTROL;
    if (wcsstr(value, L"alt+")) result.modifiers |= MOD_ALT;
    if (wcsstr(value, L"shift+")) result.modifiers |= MOD_SHIFT;
    if (wcsstr(value, L"win+")) result.modifiers |= MOD_WIN;

    const wchar_t* keyText = wcsrchr(value, L'+');
    keyText = keyText ? keyText + 1 : value;
    wchar_t* end = nullptr;
    unsigned long vk = wcstoul(keyText, &end, 10);
    if (end != keyText && *end == L'\0' && vk > 0 && vk <= 255) {
        result.vk = static_cast<UINT>(vk);
    }
    if (value) {
        Wh_FreeStringSetting(value);
    }
    if (result.vk == VK_SHIFT || result.vk == VK_CONTROL || result.vk == VK_MENU ||
        result.vk == VK_LWIN || result.vk == VK_RWIN) {
        return {};
    }
    return result;
}

void LoadShortcutSettings() {
    g_openPickerHotkey = ParseHotkeySetting(L"OpenPickerHotkey");
    g_applyHotkey = ParseHotkeySetting(L"ApplyHotkey");
    g_previousModeHotkey = ParseHotkeySetting(L"PreviousModeHotkey");
    g_nextModeHotkey = ParseHotkeySetting(L"NextModeHotkey");
    g_confirmModeHotkey = ParseHotkeySetting(L"ConfirmModeHotkey");
    g_cancelPickerHotkey = ParseHotkeySetting(L"CancelPickerHotkey");
}

LONG ModifierFlagForVk(UINT vk) {
    switch (vk) {
        case VK_CONTROL:
        case VK_LCONTROL:
        case VK_RCONTROL:
            return MOD_CONTROL;
        case VK_MENU:
        case VK_LMENU:
        case VK_RMENU:
            return MOD_ALT;
        case VK_SHIFT:
        case VK_LSHIFT:
        case VK_RSHIFT:
            return MOD_SHIFT;
        case VK_LWIN:
        case VK_RWIN:
            return MOD_WIN;
        default:
            return 0;
    }
}

void UpdateHeldModifierState(UINT vk, bool down, bool up,
                             const KBDLLHOOKSTRUCT* key) {
    // AltGr arrives as a fake Left Ctrl (scan code 0x21D) followed by Right
    // Alt. On layouts with AltGr it types characters, so it must never count
    // as Ctrl+Alt (otherwise the default shortcuts swallow those characters).
    static DWORD s_lctrlDownTime = 0;
    static bool s_altGr = false;

    if (down) {
        if (vk == VK_LCONTROL) {
            s_lctrlDownTime = key ? key->time : 0;
            if (key && key->scanCode == 0x21D) {
                s_altGr = true;
            }
        } else if (vk == VK_RMENU && key && key->time == s_lctrlDownTime) {
            s_altGr = true;
        }
    } else if (up) {
        if (vk == VK_LCONTROL || vk == VK_RMENU) {
            s_altGr = false;
        }
    }

    const bool altGr = s_altGr && (GetAsyncKeyState(VK_RMENU) & 0x8000) != 0;
    if (altGr && (vk == VK_LCONTROL || vk == VK_RMENU)) {
        // Don't let the AltGr components register as Ctrl/Alt modifiers.
        return;
    }

    LONG flag = ModifierFlagForVk(vk);
    if (!flag) {
        return;
    }
    if (down) {
        g_heldModifierFlags |= flag;
    } else if (up) {
        // Preserve the modifier if the other side of the same modifier is held.
        bool otherSideDown = false;
        if (flag == MOD_CONTROL) {
            otherSideDown = (vk == VK_LCONTROL || vk == VK_CONTROL)
                ? (GetAsyncKeyState(VK_RCONTROL) & 0x8000) != 0
                : (GetAsyncKeyState(VK_LCONTROL) & 0x8000) != 0;
        } else if (flag == MOD_ALT) {
            otherSideDown = (vk == VK_LMENU || vk == VK_MENU)
                ? (GetAsyncKeyState(VK_RMENU) & 0x8000) != 0
                : (GetAsyncKeyState(VK_LMENU) & 0x8000) != 0;
        } else if (flag == MOD_SHIFT) {
            otherSideDown = (vk == VK_LSHIFT || vk == VK_SHIFT)
                ? (GetAsyncKeyState(VK_RSHIFT) & 0x8000) != 0
                : (GetAsyncKeyState(VK_LSHIFT) & 0x8000) != 0;
        }
        if (!otherSideDown) {
            g_heldModifierFlags &= ~flag;
        }
    }
}

bool HotkeyMatches(const HotkeyBinding& hotkey, UINT vk) {
    if (!hotkey.vk || hotkey.vk != vk) {
        return false;
    }
    return (g_heldModifierFlags & MOD_CONTROL) == (hotkey.modifiers & MOD_CONTROL) &&
           (g_heldModifierFlags & MOD_ALT) == (hotkey.modifiers & MOD_ALT) &&
           (g_heldModifierFlags & MOD_SHIFT) == (hotkey.modifiers & MOD_SHIFT) &&
           (g_heldModifierFlags & MOD_WIN) == (hotkey.modifiers & MOD_WIN);
}

bool TryDirectEditReplacement(const ApplyRequest& request, bool* handled) {
    *handled = false;
    if (!IsStandardEditControl(request.focus)) {
        return false;
    }

    *handled = true;
    if (!IsCurrentFocus(request.foreground, request.focus) ||
        !IsWindowEnabled(request.focus) || !IsWindowVisible(request.focus)) {
        return false;
    }

    LONG_PTR style = GetWindowLongPtrW(request.focus, GWL_STYLE);
    if ((style & ES_READONLY) || (style & ES_PASSWORD)) {
        return false;
    }

    DWORD start = 0;
    DWORD end = 0;
    SendMessageW(request.focus, EM_GETSEL,
                 reinterpret_cast<WPARAM>(&start),
                 reinterpret_cast<LPARAM>(&end));
    if (end <= start) {
        return false;
    }

    int textLength = GetWindowTextLengthW(request.focus);
    if (textLength <= 0 || textLength > 4 * 1024 * 1024 || end > static_cast<DWORD>(textLength)) {
        return false;
    }

    std::vector<wchar_t> text(static_cast<size_t>(textLength) + 1, L'\0');
    int copied = GetWindowTextW(request.focus, text.data(), static_cast<int>(text.size()));
    if (copied <= 0 || end > static_cast<DWORD>(copied)) {
        return false;
    }

    std::wstring selected(text.data() + start, text.data() + end);
    if (selected.empty()) {
        return false;
    }

    std::wstring transformed = TransformText(selected, request.mode);
    if (transformed == selected || !IsCurrentFocus(request.foreground, request.focus)) {
        return false;
    }

    SendMessageW(request.focus, EM_REPLACESEL, TRUE,
                 reinterpret_cast<LPARAM>(transformed.c_str()));
    Wh_Log(L"Text Capitalization: replaced a selection in a standard Edit control.");
    return true;
}

bool OpenClipboardWithRetry() {
    for (int i = 0; i < 25; ++i) {
        if (g_messageWindow && OpenClipboard(g_messageWindow)) {
            return true;
        }
        Sleep(8);
    }
    return false;
}

bool SnapshotClipboard(ClipboardSnapshot* snapshot) {
    if (!snapshot) {
        return false;
    }

    if (!OpenClipboardWithRetry()) {
        return false;
    }
    snapshot->wasEmpty = (EnumClipboardFormats(0) == 0);
    CloseClipboard();

    if (snapshot->wasEmpty) {
        return true;
    }

    HRESULT hr = OleGetClipboard(&snapshot->dataObject);
    if (FAILED(hr) || !snapshot->dataObject) {
        Wh_Log(L"Text Capitalization: couldn't snapshot the existing clipboard; cancelling.");
        if (snapshot->dataObject) {
            snapshot->dataObject->Release();
            snapshot->dataObject = nullptr;
        }
        return false;
    }
    return true;
}

bool RestoreClipboard(ClipboardSnapshot* snapshot) {
    if (!snapshot) {
        return false;
    }

    bool restored = false;
    if (snapshot->wasEmpty) {
        if (OpenClipboardWithRetry()) {
            restored = EmptyClipboard() != FALSE;
            CloseClipboard();
        }
    } else if (snapshot->dataObject) {
        HRESULT hr = OleSetClipboard(snapshot->dataObject);
        if (SUCCEEDED(hr)) {
            // Flush the saved IDataObject back into native clipboard storage so
            // it remains available after this worker releases its reference.
            HRESULT flushHr = OleFlushClipboard();
            restored = SUCCEEDED(flushHr);
        }
    }

    if (snapshot->dataObject) {
        snapshot->dataObject->Release();
        snapshot->dataObject = nullptr;
    }
    if (!restored) {
        Wh_Log(L"Text Capitalization: warning: clipboard restoration failed.");
    }
    return restored;
}

bool SendCtrlKey(WORD key) {
    INPUT events[4] = {};
    events[0].type = INPUT_KEYBOARD;
    events[0].ki.wVk = VK_CONTROL;
    events[1].type = INPUT_KEYBOARD;
    events[1].ki.wVk = key;
    events[2].type = INPUT_KEYBOARD;
    events[2].ki.wVk = key;
    events[2].ki.dwFlags = KEYEVENTF_KEYUP;
    events[3].type = INPUT_KEYBOARD;
    events[3].ki.wVk = VK_CONTROL;
    events[3].ki.dwFlags = KEYEVENTF_KEYUP;
    return SendInput(ARRAYSIZE(events), events, sizeof(INPUT)) == ARRAYSIZE(events);
}

bool ReadClipboardText(std::wstring* text) {
    if (!text || !OpenClipboardWithRetry()) {
        return false;
    }

    bool success = false;
    HANDLE handle = GetClipboardData(CF_UNICODETEXT);
    if (handle) {
        const wchar_t* data = static_cast<const wchar_t*>(GlobalLock(handle));
        if (data) {
            size_t length = wcsnlen(data, 4 * 1024 * 1024 + 1);
            if (length > 0 && length <= 4 * 1024 * 1024) {
                text->assign(data, length);
                success = true;
            }
            GlobalUnlock(handle);
        }
    } else {
        HANDLE ansiHandle = GetClipboardData(CF_TEXT);
        if (ansiHandle) {
            const char* data = static_cast<const char*>(GlobalLock(ansiHandle));
            if (data) {
                size_t length = strnlen(data, 8 * 1024 * 1024 + 1);
                if (length > 0 && length <= 8 * 1024 * 1024) {
                    int required = MultiByteToWideChar(CP_ACP, 0, data,
                                                       static_cast<int>(length), nullptr, 0);
                    if (required > 0 && required <= 4 * 1024 * 1024) {
                        text->resize(static_cast<size_t>(required));
                        success = MultiByteToWideChar(CP_ACP, 0, data,
                            static_cast<int>(length), text->data(), required) == required;
                    }
                }
                GlobalUnlock(ansiHandle);
            }
        }
    }

    CloseClipboard();
    return success;
}

bool SetClipboardText(const std::wstring& text) {
    if (!OpenClipboardWithRetry()) {
        return false;
    }

    bool success = false;
    if (EmptyClipboard()) {
        const size_t bytes = (text.size() + 1) * sizeof(wchar_t);
        HGLOBAL memory = GlobalAlloc(GMEM_MOVEABLE, bytes);
        if (memory) {
            void* data = GlobalLock(memory);
            if (data) {
                memcpy(data, text.c_str(), bytes);
                GlobalUnlock(memory);
                if (SetClipboardData(CF_UNICODETEXT, memory)) {
                    success = true;
                    memory = nullptr; // clipboard owns the handle now
                }
            }
            if (memory) {
                GlobalFree(memory);
            }
        }
    }

    CloseClipboard();
    return success;
}

bool WaitForClipboardSequenceChange(DWORD before) {
    const DWORD started = GetTickCount();
    while (GetTickCount() - started < 700) {
        if (InterlockedCompareExchange(&g_shuttingDown, 0, 0)) {
            return false;
        }
        if (GetClipboardSequenceNumber() != before) {
            return true;
        }
        Sleep(8);
    }
    return false;
}

bool WaitForHotkeyModifiersToRelease() {
    const DWORD started = GetTickCount();
    while ((GetAsyncKeyState(VK_CONTROL) & 0x8000) ||
           (GetAsyncKeyState(VK_LCONTROL) & 0x8000) ||
           (GetAsyncKeyState(VK_RCONTROL) & 0x8000) ||
           (GetAsyncKeyState(VK_MENU) & 0x8000) ||
           (GetAsyncKeyState(VK_LMENU) & 0x8000) ||
           (GetAsyncKeyState(VK_RMENU) & 0x8000) ||
           (GetAsyncKeyState(VK_SHIFT) & 0x8000) ||
           (GetAsyncKeyState(VK_LSHIFT) & 0x8000) ||
           (GetAsyncKeyState(VK_RSHIFT) & 0x8000) ||
           (GetAsyncKeyState(VK_LWIN) & 0x8000) ||
           (GetAsyncKeyState(VK_RWIN) & 0x8000)) {
        if (InterlockedCompareExchange(&g_shuttingDown, 0, 0) ||
            GetTickCount() - started > 1500) {
            return false;
        }
        Sleep(8);
    }
    return true;
}

bool CaptureSelectedText(HWND foreground, HWND focus, std::wstring* selectedText) {
    if (!selectedText || !IsCurrentFocus(foreground, focus)) {
        return false;
    }

    HRESULT oleHr = OleInitialize(nullptr);
    if (FAILED(oleHr) && oleHr != RPC_E_CHANGED_MODE) {
        Wh_Log(L"Text Capitalization: OLE initialization failed during selection capture.");
        return false;
    }
    const bool initializedOle = SUCCEEDED(oleHr);

    ClipboardSnapshot snapshot;
    if (!SnapshotClipboard(&snapshot)) {
        if (initializedOle) OleUninitialize();
        Wh_Log(L"Text Capitalization: couldn't preserve the clipboard before selection capture.");
        return false;
    }

    bool copied = false;
    if (IsCurrentFocus(foreground, focus)) {
        DWORD sequenceBefore = GetClipboardSequenceNumber();
        copied = SendCtrlKey('C') && WaitForClipboardSequenceChange(sequenceBefore);
    }
    if (copied && IsCurrentFocus(foreground, focus)) {
        copied = ReadClipboardText(selectedText) && !selectedText->empty();
    } else {
        copied = false;
    }

    RestoreClipboard(&snapshot);
    if (initializedOle) OleUninitialize();

    if (!copied || !IsCurrentFocus(foreground, focus)) {
        selectedText->clear();
        return false;
    }
    return true;
}

DWORD WINAPI SelectionCaptureWorkerProc(void* parameter) {
    SelectionCaptureRequest* request = static_cast<SelectionCaptureRequest*>(parameter);
    if (!request) {
        InterlockedExchange(&g_captureInProgress, 0);
        return 0;
    }

    SelectionCaptureResult* result = new (std::nothrow) SelectionCaptureResult{
        request->foreground, request->focus, false, std::wstring()};
    if (result && WaitForHotkeyModifiersToRelease() &&
        IsCurrentFocus(request->foreground, request->focus)) {
        result->success = CaptureSelectedText(request->foreground, request->focus,
                                              &result->selectedText);
    }

    if (result && g_messageWindow &&
        PostMessageW(g_messageWindow, WM_APP_CAPTURE_READY, 0,
                     reinterpret_cast<LPARAM>(result))) {
        // Result is owned by the message thread.
    } else {
        delete result;
        InterlockedExchange(&g_captureInProgress, 0);
    }
    delete request;
    return 0;
}

void StartSelectionCapture(HWND foreground, HWND focus) {
    if (!foreground || !focus || !IsWindow(foreground) || !IsWindow(focus)) {
        Wh_Log(L"Text Capitalization: picker not opened; no focused target window was found.");
        return;
    }
    if (InterlockedCompareExchange(&g_captureInProgress, 1, 0) != 0) {
        Wh_Log(L"Text Capitalization: selection capture is already running.");
        return;
    }

    SelectionCaptureRequest* request = new (std::nothrow) SelectionCaptureRequest{
        foreground, focus};
    if (!request) {
        InterlockedExchange(&g_captureInProgress, 0);
        return;
    }

    AcquireSRWLockExclusive(&g_lifecycleLock);
    if (InterlockedCompareExchange(&g_shuttingDown, 0, 0)) {
        ReleaseSRWLockExclusive(&g_lifecycleLock);
        delete request;
        InterlockedExchange(&g_captureInProgress, 0);
        return;
    }

    if (g_captureThreadHandle) {
        // The previous capture worker has already handed off its result.
        WaitForSingleObject(g_captureThreadHandle, INFINITE);
        CloseHandle(g_captureThreadHandle);
        g_captureThreadHandle = nullptr;
    }

    g_captureThreadHandle = CreateThread(nullptr, 0, SelectionCaptureWorkerProc,
                                         request, 0, nullptr);
    if (!g_captureThreadHandle) {
        delete request;
        InterlockedExchange(&g_captureInProgress, 0);
        Wh_Log(L"Text Capitalization: couldn't start selection capture worker.");
    }
    ReleaseSRWLockExclusive(&g_lifecycleLock);
}

bool ApplyCapturedSelectionViaClipboard(const ApplyRequest& request) {
    if (request.selectedText.empty() ||
        !IsCurrentFocus(request.foreground, request.focus)) {
        Wh_Log(L"Text Capitalization: captured selection apply cancelled; original focus is no longer active.");
        return false;
    }

    HRESULT oleHr = OleInitialize(nullptr);
    if (FAILED(oleHr) && oleHr != RPC_E_CHANGED_MODE) {
        Wh_Log(L"Text Capitalization: OLE initialization failed during replacement.");
        return false;
    }
    const bool initializedOle = SUCCEEDED(oleHr);

    ClipboardSnapshot snapshot;
    if (!SnapshotClipboard(&snapshot)) {
        if (initializedOle) OleUninitialize();
        return false;
    }

    const std::wstring transformed = TransformText(request.selectedText, request.mode);
    bool changed = false;
    if (!transformed.empty() && transformed != request.selectedText &&
        IsCurrentFocus(request.foreground, request.focus) &&
        SetClipboardText(transformed)) {
        Sleep(45);
        if (IsCurrentFocus(request.foreground, request.focus) && SendCtrlKey('V')) {
            // Give the target editor time to consume the clipboard data before restoring it.
            Sleep(220);
            changed = true;
        }
    } else if (transformed == request.selectedText) {
        Wh_Log(L"Text Capitalization: selected text already matches the chosen case; no paste needed.");
    }

    RestoreClipboard(&snapshot);
    if (initializedOle) OleUninitialize();
    if (changed) {
        Wh_Log(L"Text Capitalization: replaced the captured selection via paste.");
    } else {
        Wh_Log(L"Text Capitalization: captured selection wasn't replaced; focus, clipboard, or paste failed.");
    }
    return changed;
}

bool ApplyViaClipboard(const ApplyRequest& request) {
    if (!IsCurrentFocus(request.foreground, request.focus)) {
        Wh_Log(L"Text Capitalization: cancelled; the original window/focus changed.");
        return false;
    }

    HRESULT oleHr = OleInitialize(nullptr);
    if (FAILED(oleHr) && oleHr != RPC_E_CHANGED_MODE) {
        Wh_Log(L"Text Capitalization: COM/OLE initialization failed.");
        return false;
    }
    const bool initializedOle = SUCCEEDED(oleHr);

    ClipboardSnapshot snapshot;
    if (!SnapshotClipboard(&snapshot)) {
        if (initializedOle) {
            OleUninitialize();
        }
        return false;
    }

    DWORD sequenceBefore = GetClipboardSequenceNumber();
    bool copied = SendCtrlKey('C');
    if (copied) {
        copied = WaitForClipboardSequenceChange(sequenceBefore);
    }

    std::wstring selected;
    if (copied && IsCurrentFocus(request.foreground, request.focus)) {
        copied = ReadClipboardText(&selected) && !selected.empty();
    } else {
        copied = false;
    }

    bool changed = false;
    if (copied && IsCurrentFocus(request.foreground, request.focus)) {
        std::wstring transformed = TransformText(selected, request.mode);
        if (!transformed.empty() && transformed != selected &&
            SetClipboardText(transformed) &&
            IsCurrentFocus(request.foreground, request.focus)) {
            Sleep(40);
            if (SendCtrlKey('V')) {
                // Give standard and web-based editors a short interval to read
                // the paste data before restoring the user's previous clipboard.
                Sleep(180);
                changed = true;
            }
        }
    }

    RestoreClipboard(&snapshot);
    if (initializedOle) {
        OleUninitialize();
    }

    if (changed) {
        Wh_Log(L"Text Capitalization: applied the active mode through copy/paste.");
    } else {
        Wh_Log(L"Text Capitalization: no safe text replacement was completed.");
    }
    return changed;
}

bool ConfirmEditableSelectionOnWorkerThread(HWND foreground, HWND focus) {
    bool recognizedNativeControl = false;
    bool nativeSelected = HasNativeEditSelection(focus, &recognizedNativeControl);
    if (recognizedNativeControl) {
        return nativeSelected && IsCurrentFocus(foreground, focus);
    }

    // This worker uses its own COM apartment/UIA client instead of sharing the
    // message thread's apartment-bound interface across threads.
    HRESULT oleResult = OleInitialize(nullptr);
    if (FAILED(oleResult)) {
        return false;
    }

    IUIAutomation2* automation = nullptr;
    bool selected = CreateAutomationClient(&automation) &&
                    HasEditableTextSelection(foreground, focus, automation);
    if (automation) {
        automation->Release();
    }
    OleUninitialize();
    return selected;
}

DWORD WINAPI ApplyWorkerProc(void* parameter) {
    ApplyRequest* request = static_cast<ApplyRequest*>(parameter);
    if (!request) {
        InterlockedExchange(&g_applyInProgress, 0);
        return 0;
    }

    if (!WaitForHotkeyModifiersToRelease()) {
        delete request;
        InterlockedExchange(&g_applyInProgress, 0);
        return 0;
    }

    if (!IsCurrentFocus(request->foreground, request->focus)) {
        Wh_Log(L"Text Capitalization: cancelled; focus changed before apply.");
        delete request;
        InterlockedExchange(&g_applyInProgress, 0);
        return 0;
    }

    if (request->fromPicker) {
        if (request->selectedText.empty()) {
            Wh_Log(L"Text Capitalization: picker apply cancelled because no captured text was available.");
        } else {
            ApplyCapturedSelectionViaClipboard(*request);
        }
    } else {
        // This optional shortcut uses the same clipboard-based replacement path.
        // It avoids requiring a UI Automation provider to expose TextPattern.
        ApplyViaClipboard(*request);
    }

    delete request;
    InterlockedExchange(&g_applyInProgress, 0);
    return 0;
}

void StartApply(HWND foreground, HWND focus, CaseMode mode, bool fromPicker,
                const std::wstring* capturedText = nullptr) {
    if (!foreground || !focus || !IsWindow(foreground) || !IsWindow(focus)) {
        Wh_Log(L"Text Capitalization: apply cancelled because a foreground text control wasn't available.");
        return;
    }

    AcquireSRWLockExclusive(&g_lifecycleLock);
    if (InterlockedCompareExchange(&g_shuttingDown, 0, 0)) {
        ReleaseSRWLockExclusive(&g_lifecycleLock);
        return;
    }

    if (InterlockedCompareExchange(&g_applyInProgress, 1, 0) != 0) {
        ReleaseSRWLockExclusive(&g_lifecycleLock);
        Wh_Log(L"Text Capitalization: an apply operation is already in progress.");
        return;
    }

    if (g_applyThreadHandle) {
        // The previous worker has already cleared g_applyInProgress. Wait for
        // its final return before closing the handle or starting a new worker.
        WaitForSingleObject(g_applyThreadHandle, INFINITE);
        CloseHandle(g_applyThreadHandle);
        g_applyThreadHandle = nullptr;
    }

    ApplyRequest* request = new (std::nothrow) ApplyRequest{
        foreground, focus, mode, fromPicker, std::wstring()};
    if (!request) {
        InterlockedExchange(&g_applyInProgress, 0);
        ReleaseSRWLockExclusive(&g_lifecycleLock);
        return;
    }
    if (capturedText) {
        request->selectedText = *capturedText;
    }

    g_applyThreadHandle = CreateThread(nullptr, 0, ApplyWorkerProc, request, 0, nullptr);
    if (!g_applyThreadHandle) {
        delete request;
        InterlockedExchange(&g_applyInProgress, 0);
        Wh_Log(L"Text Capitalization: couldn't start the apply worker thread.");
    }
    ReleaseSRWLockExclusive(&g_lifecycleLock);
}

void ClosePicker(bool commitHighlightedMode) {
    if (!InterlockedCompareExchange(&g_popupOpen, 0, 0)) {
        return;
    }

    if (commitHighlightedMode && IsValidMode(g_highlightedMode)) {
        PersistMode(static_cast<CaseMode>(g_highlightedMode));
    } else {
        g_highlightedMode = static_cast<int>(GetActiveMode());
    }

    InterlockedExchange(&g_popupOpen, 0);
    HWND window = g_pickerWindow;
    g_pickerWindow = nullptr;
    if (window && IsWindow(window)) {
        DestroyWindow(window);
    }
    g_selectedText.clear();
}

void CommitPickerMode() {
    if (!InterlockedCompareExchange(&g_popupOpen, 0, 0) ||
        !IsValidMode(g_highlightedMode) || g_selectedText.empty()) {
        ClosePicker(false);
        return;
    }

    const int modeValue = g_highlightedMode;
    const CaseMode mode = static_cast<CaseMode>(modeValue);
    const HWND foreground = reinterpret_cast<HWND>(InterlockedCompareExchangePointer(
        &g_originForeground, nullptr, nullptr));
    const HWND focus = reinterpret_cast<HWND>(InterlockedCompareExchangePointer(
        &g_originFocus, nullptr, nullptr));
    const std::wstring captured = g_selectedText;

    PersistMode(mode);
    ClosePicker(false);
    StartApply(foreground, focus, mode, true, &captured);
}

void OpenPicker() {
    if (InterlockedCompareExchange(&g_popupOpen, 0, 0)) {
        return;
    }

    g_highlightedMode = static_cast<int>(GetActiveMode());
    InterlockedExchange(&g_popupOpen, 1);

    POINT cursor = {};
    GetCursorPos(&cursor);
    HMONITOR monitor = MonitorFromPoint(cursor, MONITOR_DEFAULTTONEAREST);
    MONITORINFO monitorInfo = {};
    monitorInfo.cbSize = sizeof(monitorInfo);
    GetMonitorInfoW(monitor, &monitorInfo);
    RECT work = monitorInfo.rcWork;

    UINT dpiX = 96;
    UINT dpiY = 96;
    if (FAILED(GetDpiForMonitor(monitor, MDT_EFFECTIVE_DPI, &dpiX, &dpiY)) || dpiX == 0) {
        dpiX = 96;
    }
    g_dpi = static_cast<int>(dpiX);
    const int pickerWidth = DpiScale(kPickerWidth);
    const int pickerHeight = DpiScale(kPickerHeight);

    int x = cursor.x + 16;
    int y = cursor.y + 16;
    if (x + pickerWidth > work.right) {
        x = cursor.x - pickerWidth - 12;
    }
    if (y + pickerHeight > work.bottom) {
        y = cursor.y - pickerHeight - 12;
    }
    x = std::max(static_cast<int>(work.left), std::min(x, static_cast<int>(work.right) - pickerWidth));
    y = std::max(static_cast<int>(work.top), std::min(y, static_cast<int>(work.bottom) - pickerHeight));

    g_pickerWindow = CreateWindowExW(
        WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE,
        kPickerClass, L"Text Capitalization", WS_POPUP,
        x, y, pickerWidth, pickerHeight,
        nullptr, nullptr, g_modModule, nullptr);

    if (!g_pickerWindow) {
        InterlockedExchange(&g_popupOpen, 0);
        Wh_Log(L"Text Capitalization: couldn't create the mode picker window.");
        return;
    }

    HRGN region = CreateRoundRectRgn(0, 0, pickerWidth + 1,
                                    pickerHeight + 1, DpiScale(18), DpiScale(18));
    if (region) {
        SetWindowRgn(g_pickerWindow, region, TRUE);
    }
    SetWindowPos(g_pickerWindow, HWND_TOPMOST, x, y, pickerWidth,
                 pickerHeight, SWP_NOACTIVATE | SWP_SHOWWINDOW);
    InvalidateRect(g_pickerWindow, nullptr, TRUE);
}

void MoveHighlight(int delta) {
    if (!InterlockedCompareExchange(&g_popupOpen, 0, 0)) {
        return;
    }
    g_highlightedMode = (g_highlightedMode + delta + kModeCount) % kModeCount;
    if (g_pickerWindow) {
        InvalidateRect(g_pickerWindow, nullptr, TRUE);
    }
}

int GetModeAtPoint(int y) {
    const int rowTop = DpiScale(kRowTop);
    const int rowHeight = DpiScale(kRowHeight);
    if (y < rowTop || y >= rowTop + kModeCount * rowHeight) {
        return -1;
    }
    int mode = (y - rowTop) / rowHeight;
    return IsValidMode(mode) ? mode : -1;
}

void DrawPicker(HWND hwnd) {
    PAINTSTRUCT ps = {};
    HDC target = BeginPaint(hwnd, &ps);
    RECT client = {};
    GetClientRect(hwnd, &client);
    int width = client.right - client.left;
    int height = client.bottom - client.top;

    HDC memory = CreateCompatibleDC(target);
    HBITMAP bitmap = CreateCompatibleBitmap(target, width, height);
    HGDIOBJ oldBitmap = SelectObject(memory, bitmap);

    HBRUSH background = CreateSolidBrush(RGB(25, 28, 35));
    RECT all = {0, 0, width, height};
    FillRect(memory, &all, background);

    HPEN borderPen = CreatePen(PS_SOLID, 1, RGB(55, 61, 73));
    HGDIOBJ oldPen = SelectObject(memory, borderPen);
    HGDIOBJ oldBrush = SelectObject(memory, GetStockObject(NULL_BRUSH));
    const int cornerRadius = DpiScale(18);
    RoundRect(memory, 0, 0, width - 1, height - 1, cornerRadius, cornerRadius);
    SelectObject(memory, oldBrush);
    SelectObject(memory, oldPen);
    DeleteObject(borderPen);

    SetBkMode(memory, TRANSPARENT);
    HGDIOBJ oldFont = SelectObject(memory, g_bodyFont);

    // Read-only, edit-like text preview. It shows the transformation for the
    // currently highlighted mode; it never accepts focus or keyboard input.
    RECT previewRect = {DpiScale(10), DpiScale(kPreviewTop), width - DpiScale(10), DpiScale(kPreviewTop) + DpiScale(kPreviewHeight)};
    HBRUSH previewBrush = CreateSolidBrush(RGB(17, 19, 24));
    HPEN previewPen = CreatePen(PS_SOLID, 1, RGB(63, 69, 82));
    HGDIOBJ previousPreviewBrush = SelectObject(memory, previewBrush);
    HGDIOBJ previousPreviewPen = SelectObject(memory, previewPen);
    const int smallCorner = DpiScale(8);
    RoundRect(memory, previewRect.left, previewRect.top, previewRect.right,
              previewRect.bottom, smallCorner, smallCorner);
    SelectObject(memory, previousPreviewBrush);
    SelectObject(memory, previousPreviewPen);
    DeleteObject(previewBrush);
    DeleteObject(previewPen);

    const size_t previewLimit = 220;
    std::wstring preview = TransformText(
        g_selectedText.substr(0, previewLimit + 1),
        static_cast<CaseMode>(IsValidMode(g_highlightedMode) ? g_highlightedMode : 0));
    if (preview.size() > previewLimit) {
        preview.resize(previewLimit);
        preview += L"\u2026";
    }
    RECT previewTextRect = {previewRect.left + DpiScale(9), previewRect.top + DpiScale(6),
                            previewRect.right - DpiScale(9), previewRect.bottom - DpiScale(6)};
    SetTextColor(memory, RGB(232, 235, 242));
    DrawTextW(memory, preview.c_str(), -1, &previewTextRect,
              DT_LEFT | DT_TOP | DT_WORDBREAK | DT_NOPREFIX | DT_END_ELLIPSIS);

    for (int i = 0; i < kModeCount; ++i) {
        const int rowTop = DpiScale(kRowTop);
        const int rowHeight = DpiScale(kRowHeight);
        RECT row = {DpiScale(7), rowTop + i * rowHeight,
                    width - DpiScale(7), rowTop + (i + 1) * rowHeight - DpiScale(3)};
        const bool selected = (i == g_highlightedMode);
        const bool active = (i == static_cast<int>(GetActiveMode()));
        if (selected) {
            HBRUSH selectedBrush = CreateSolidBrush(RGB(47, 99, 190));
            HPEN selectedPen = CreatePen(PS_SOLID, 1, RGB(47, 99, 190));
            HGDIOBJ previousBrush = SelectObject(memory, selectedBrush);
            HGDIOBJ previousPen = SelectObject(memory, selectedPen);
            const int rowCorner = DpiScale(9);
            RoundRect(memory, row.left, row.top, row.right, row.bottom, rowCorner, rowCorner);
            SelectObject(memory, previousBrush);
            SelectObject(memory, previousPen);
            DeleteObject(selectedBrush);
            DeleteObject(selectedPen);
        } else {
            HBRUSH rowBrush = CreateSolidBrush(RGB(32, 36, 44));
            HPEN rowPen = CreatePen(PS_SOLID, 1, RGB(43, 48, 58));
            HGDIOBJ previousBrush = SelectObject(memory, rowBrush);
            HGDIOBJ previousPen = SelectObject(memory, rowPen);
            const int rowCorner = DpiScale(9);
            RoundRect(memory, row.left, row.top, row.right, row.bottom, rowCorner, rowCorner);
            SelectObject(memory, previousBrush);
            SelectObject(memory, previousPen);
            DeleteObject(rowBrush);
            DeleteObject(rowPen);
        }

        RECT nameRect = {row.left + DpiScale(12), row.top + DpiScale(2), row.right - DpiScale(42), row.bottom - DpiScale(2)};
        SelectObject(memory, g_bodyFont);
        SetTextColor(memory, selected ? RGB(255, 255, 255) : RGB(231, 234, 240));
        DrawTextW(memory, kModeNames[i], -1, &nameRect,
                  DT_LEFT | DT_VCENTER | DT_SINGLELINE);

        if (active) {
            SelectObject(memory, g_bodyFont);
            SetTextColor(memory, selected ? RGB(255, 255, 255) : RGB(116, 177, 255));
            RECT checkRect = {row.right - DpiScale(37), row.top + DpiScale(3), row.right - DpiScale(10), row.bottom - DpiScale(3)};
            DrawTextW(memory, L"\u2713", -1, &checkRect,
                      DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    }

    BitBlt(target, 0, 0, width, height, memory, 0, 0, SRCCOPY);
    SelectObject(memory, oldFont);
    SelectObject(memory, oldBitmap);
    DeleteObject(bitmap);
    DeleteObject(background);
    DeleteDC(memory);
    EndPaint(hwnd, &ps);
}

LRESULT CALLBACK PickerWindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
        case WM_CREATE:
            g_bodyFont = CreateFontW(-DpiScale(14), 0, 0, 0, FW_MEDIUM, FALSE, FALSE,
                FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");
            SetTimer(hwnd, kPickerTimer, 120, nullptr);
            return 0;

        case WM_MOUSEACTIVATE:
            return MA_NOACTIVATE;

        case WM_TIMER:
            if (wParam == kPickerTimer) {
                HWND origin = reinterpret_cast<HWND>(InterlockedCompareExchangePointer(
                    &g_originForeground, nullptr, nullptr));
                if (origin && GetForegroundWindow() != origin) {
                    PostMessageW(g_messageWindow, WM_APP_CLOSE_PICKER, 0, 0);
                }
            }
            return 0;

        case WM_MOUSEMOVE: {
            int y = GET_Y_LPARAM(lParam);
            int mode = GetModeAtPoint(y);
            if (mode >= 0 && mode != g_highlightedMode) {
                g_highlightedMode = mode;
                InvalidateRect(hwnd, nullptr, FALSE);
            }
            return 0;
        }

        case WM_LBUTTONDOWN: {
            int mode = GetModeAtPoint(GET_Y_LPARAM(lParam));
            if (IsValidMode(mode)) {
                g_highlightedMode = mode;
                CommitPickerMode();
            }
            // Clicking the read-only preview area does not edit or cancel it.
            return 0;
        }

        case WM_PAINT:
            DrawPicker(hwnd);
            return 0;

        case WM_ERASEBKGND:
            return 1;

        case WM_NCDESTROY:
            KillTimer(hwnd, kPickerTimer);
            if (g_bodyFont) { DeleteObject(g_bodyFont); g_bodyFont = nullptr; }
            return DefWindowProcW(hwnd, message, wParam, lParam);
    }
    return DefWindowProcW(hwnd, message, wParam, lParam);
}

void HandleShowPicker(UiActionRequest* request) {
    if (!request) {
        return;
    }
    if (InterlockedCompareExchange(&g_popupOpen, 0, 0) ||
        InterlockedCompareExchange(&g_captureInProgress, 0, 0)) {
        delete request;
        return;
    }
    StartSelectionCapture(request->foreground, request->focus);
    delete request;
}

void HandleCaptureReady(SelectionCaptureResult* result) {
    InterlockedExchange(&g_captureInProgress, 0);
    if (!result) {
        return;
    }

    if (result->success && !result->selectedText.empty() &&
        IsCurrentFocus(result->foreground, result->focus)) {
        InterlockedExchangePointer(&g_originForeground, result->foreground);
        InterlockedExchangePointer(&g_originFocus, result->focus);
        g_selectedText = result->selectedText;
        g_highlightedMode = static_cast<int>(GetActiveMode());
        OpenPicker();
        Wh_Log(L"Text Capitalization: captured %zu selected characters and opened picker.",
               g_selectedText.size());
    } else {
        g_selectedText.clear();
        Wh_Log(L"Text Capitalization: picker not opened; no text selection could be copied, or the target focus changed.");
    }
    delete result;
}

LRESULT CALLBACK MessageWindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
        case WM_APP_SHOW_PICKER:
            HandleShowPicker(reinterpret_cast<UiActionRequest*>(lParam));
            return 0;
        case WM_APP_CAPTURE_READY:
            HandleCaptureReady(reinterpret_cast<SelectionCaptureResult*>(lParam));
            return 0;
        case WM_APP_CLOSE_PICKER:
            ClosePicker(false);
            return 0;
        case WM_APP_RELOAD_SHORTCUTS:
            LoadShortcutSettings();
            Wh_Log(L"Text Capitalization: keyboard shortcuts reloaded from settings.");
            return 0;
        case WM_APP_MOVE_HIGHLIGHT:
            MoveHighlight(wParam == 0 ? -1 : 1);
            return 0;
        case WM_APP_COMMIT_MODE:
            CommitPickerMode();
            return 0;
        case WM_APP_APPLY: {
            UiActionRequest* request = reinterpret_cast<UiActionRequest*>(lParam);
            if (request) {
                if (request->fromPicker) {
                    ClosePicker(false);
                }
                StartApply(request->foreground, request->focus,
                           request->mode, request->fromPicker);
                delete request;
            }
            return 0;
        }
        case WM_APP_SHUTDOWN:
            ClosePicker(false);
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProcW(hwnd, message, wParam, lParam);
}

LRESULT CALLBACK LowLevelKeyboardProc(int code, WPARAM wParam, LPARAM lParam) {
    if (code != HC_ACTION || !lParam) {
        return CallNextHookEx(g_keyboardHook, code, wParam, lParam);
    }

    if (InterlockedCompareExchange(&g_shuttingDown, 0, 0)) {
        return CallNextHookEx(g_keyboardHook, code, wParam, lParam);
    }

    const KBDLLHOOKSTRUCT* key = reinterpret_cast<const KBDLLHOOKSTRUCT*>(lParam);
    if (key->flags & LLKHF_INJECTED) {
        return CallNextHookEx(g_keyboardHook, code, wParam, lParam);
    }

    const bool down = (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN);
    const bool up = (wParam == WM_KEYUP || wParam == WM_SYSKEYUP);
    const UINT vk = key->vkCode;
    UpdateHeldModifierState(vk, down, up, key);

    LONG consumedVk = InterlockedCompareExchange(&g_consumedVk, 0, 0);
    if (consumedVk == static_cast<LONG>(vk)) {
        if (up) {
            InterlockedExchange(&g_consumedVk, 0);
        }
        return 1;
    }

    const bool popupOpen = InterlockedCompareExchange(&g_popupOpen, 0, 0) != 0;

    // While the picker is open, navigation and confirmation affect its highlighted mode.
    // Confirmation now transforms and pastes the captured selection.
    if (down && popupOpen && HotkeyMatches(g_previousModeHotkey, vk)) {
        InterlockedExchange(&g_consumedVk, static_cast<LONG>(vk));
        PostMessageW(g_messageWindow, WM_APP_MOVE_HIGHLIGHT, 0, 0);
        return 1;
    }
    if (down && popupOpen && HotkeyMatches(g_nextModeHotkey, vk)) {
        InterlockedExchange(&g_consumedVk, static_cast<LONG>(vk));
        PostMessageW(g_messageWindow, WM_APP_MOVE_HIGHLIGHT, 1, 0);
        return 1;
    }
    if (down && popupOpen && HotkeyMatches(g_confirmModeHotkey, vk)) {
        InterlockedExchange(&g_consumedVk, static_cast<LONG>(vk));
        PostMessageW(g_messageWindow, WM_APP_COMMIT_MODE, 0, 0);
        return 1;
    }
    if (down && popupOpen && HotkeyMatches(g_cancelPickerHotkey, vk)) {
        InterlockedExchange(&g_consumedVk, static_cast<LONG>(vk));
        PostMessageW(g_messageWindow, WM_APP_CLOSE_PICKER, 0, 0);
        return 1;
    }

    if (down && HotkeyMatches(g_openPickerHotkey, vk)) {
        InterlockedExchange(&g_consumedVk, static_cast<LONG>(vk));
        if (popupOpen) {
            PostMessageW(g_messageWindow, WM_APP_CLOSE_PICKER, 0, 0);
        } else {
            HWND foreground = GetForegroundWindow();
            HWND focus = nullptr;
            CaptureFocusWindow(foreground, &focus);
            UiActionRequest* request = new (std::nothrow) UiActionRequest{
                foreground, focus, GetActiveMode(), false};
            if (request && !PostMessageW(g_messageWindow, WM_APP_SHOW_PICKER,
                                         0, reinterpret_cast<LPARAM>(request))) {
                delete request;
            }
        }
        return 1;
    }

    if (down && !popupOpen && HotkeyMatches(g_applyHotkey, vk)) {
        HWND foreground = GetForegroundWindow();
        HWND focus = nullptr;
        CaptureFocusWindow(foreground, &focus);
        if (!focus || !foreground || !IsWindow(focus) || !IsWindow(foreground)) {
            return CallNextHookEx(g_keyboardHook, code, wParam, lParam);
        }
        InterlockedExchange(&g_consumedVk, static_cast<LONG>(vk));
        UiActionRequest* request = new (std::nothrow) UiActionRequest{
            foreground, focus, GetActiveMode(), false};
        if (request && !PostMessageW(g_messageWindow, WM_APP_APPLY,
                                     0, reinterpret_cast<LPARAM>(request))) {
            delete request;
        }
        return 1;
    }

    if (popupOpen && down) {
        // The picker is non-activating, so unhandled keys would otherwise reach
        // the editor and could replace the still-selected text. Close and swallow
        // ordinary keys instead of allowing them to modify the selection.
        const bool modifierKey = vk == VK_SHIFT || vk == VK_LSHIFT || vk == VK_RSHIFT ||
            vk == VK_CONTROL || vk == VK_LCONTROL || vk == VK_RCONTROL ||
            vk == VK_MENU || vk == VK_LMENU || vk == VK_RMENU ||
            vk == VK_LWIN || vk == VK_RWIN;
        if (!modifierKey) {
            InterlockedExchange(&g_consumedVk, static_cast<LONG>(vk));
            PostMessageW(g_messageWindow, WM_APP_CLOSE_PICKER, 0, 0);
            return 1;
        }
    }

    return CallNextHookEx(g_keyboardHook, code, wParam, lParam);
}

LRESULT CALLBACK LowLevelMouseProc(int code, WPARAM wParam, LPARAM lParam) {
    if (code == HC_ACTION && lParam &&
        !InterlockedCompareExchange(&g_shuttingDown, 0, 0) &&
        InterlockedCompareExchange(&g_popupOpen, 0, 0)) {
        bool buttonDown = wParam == WM_LBUTTONDOWN || wParam == WM_RBUTTONDOWN ||
                          wParam == WM_MBUTTONDOWN || wParam == WM_XBUTTONDOWN;
        if (buttonDown && g_pickerWindow && IsWindow(g_pickerWindow)) {
            const MSLLHOOKSTRUCT* mouse = reinterpret_cast<const MSLLHOOKSTRUCT*>(lParam);
            RECT rect = {};
            if (GetWindowRect(g_pickerWindow, &rect) &&
                (mouse->pt.x < rect.left || mouse->pt.x >= rect.right ||
                 mouse->pt.y < rect.top || mouse->pt.y >= rect.bottom)) {
                PostMessageW(g_messageWindow, WM_APP_CLOSE_PICKER, 0, 0);
            }
        }
    }
    return CallNextHookEx(g_mouseHook, code, wParam, lParam);
}

DWORD WINAPI MessageThreadProc(void*) {
    HRESULT comResult = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    bool comInitialized = SUCCEEDED(comResult);
    if (FAILED(comResult) && comResult != RPC_E_CHANGED_MODE) {
        Wh_Log(L"Text Capitalization: COM initialization failed; UI Automation selection checks will be unavailable.");
    } else if (CreateAutomationClient(&g_uiAutomation)) {
        Wh_Log(L"Text Capitalization: UI Automation selection checks are ready.");
    } else {
        Wh_Log(L"Text Capitalization: UI Automation unavailable; only supported native edit controls can pass the selection guard.");
    }
    LoadShortcutSettings();
    Wh_Log(L"Text Capitalization: shortcuts loaded (Apply VK=%u, modifiers=0x%X).",
           g_applyHotkey.vk, g_applyHotkey.modifiers);

    WNDCLASSEXW messageClass = {};
    messageClass.cbSize = sizeof(messageClass);
    messageClass.lpfnWndProc = MessageWindowProc;
    messageClass.hInstance = g_modModule;
    messageClass.lpszClassName = kMessageClass;
    if (!RegisterClassExW(&messageClass)) {
        if (g_uiAutomation) { g_uiAutomation->Release(); g_uiAutomation = nullptr; }
        if (comInitialized) CoUninitialize();
        InterlockedExchange(&g_threadReadyState, -1);
        SetEvent(g_readyEvent);
        return 1;
    }

    WNDCLASSEXW pickerClass = {};
    pickerClass.cbSize = sizeof(pickerClass);
    pickerClass.lpfnWndProc = PickerWindowProc;
    pickerClass.hInstance = g_modModule;
    pickerClass.lpszClassName = kPickerClass;
    pickerClass.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    if (!RegisterClassExW(&pickerClass)) {
        UnregisterClassW(kMessageClass, g_modModule);
        if (g_uiAutomation) { g_uiAutomation->Release(); g_uiAutomation = nullptr; }
        if (comInitialized) CoUninitialize();
        InterlockedExchange(&g_threadReadyState, -1);
        SetEvent(g_readyEvent);
        return 1;
    }

    // Use a hidden top-level window rather than HWND_MESSAGE: clipboard
    // ownership requires a real, non-NULL owner HWND when EmptyClipboard runs.
    g_messageWindow = CreateWindowExW(0, kMessageClass, L"", WS_POPUP,
        0, 0, 0, 0, nullptr, nullptr, g_modModule, nullptr);
    if (!g_messageWindow) {
        UnregisterClassW(kPickerClass, g_modModule);
        UnregisterClassW(kMessageClass, g_modModule);
        if (g_uiAutomation) { g_uiAutomation->Release(); g_uiAutomation = nullptr; }
        if (comInitialized) CoUninitialize();
        InterlockedExchange(&g_threadReadyState, -1);
        SetEvent(g_readyEvent);
        return 1;
    }

    g_keyboardHook = SetWindowsHookExW(WH_KEYBOARD_LL, LowLevelKeyboardProc,
                                       g_modModule, 0);
    g_mouseHook = SetWindowsHookExW(WH_MOUSE_LL, LowLevelMouseProc,
                                    g_modModule, 0);
    if (!g_keyboardHook || !g_mouseHook) {
        Wh_Log(L"Text Capitalization: failed to install global keyboard/mouse hooks (error %lu).",
               GetLastError());
        if (g_keyboardHook) { UnhookWindowsHookEx(g_keyboardHook); g_keyboardHook = nullptr; }
        if (g_mouseHook) { UnhookWindowsHookEx(g_mouseHook); g_mouseHook = nullptr; }
        DestroyWindow(g_messageWindow);
        g_messageWindow = nullptr;
        UnregisterClassW(kPickerClass, g_modModule);
        UnregisterClassW(kMessageClass, g_modModule);
        if (g_uiAutomation) { g_uiAutomation->Release(); g_uiAutomation = nullptr; }
        if (comInitialized) CoUninitialize();
        InterlockedExchange(&g_threadReadyState, -1);
        SetEvent(g_readyEvent);
        return 1;
    }

    InterlockedExchange(&g_threadReadyState, 1);
    SetEvent(g_readyEvent);
    Wh_Log(L"Text Capitalization: global shortcuts installed.");

    MSG message = {};
    while (GetMessageW(&message, nullptr, 0, 0) > 0) {
        TranslateMessage(&message);
        DispatchMessageW(&message);
    }

    ClosePicker(false);
    if (g_keyboardHook) { UnhookWindowsHookEx(g_keyboardHook); g_keyboardHook = nullptr; }
    if (g_mouseHook) { UnhookWindowsHookEx(g_mouseHook); g_mouseHook = nullptr; }
    if (g_messageWindow) { DestroyWindow(g_messageWindow); g_messageWindow = nullptr; }
    UnregisterClassW(kPickerClass, g_modModule);
    UnregisterClassW(kMessageClass, g_modModule);
    if (g_uiAutomation) { g_uiAutomation->Release(); g_uiAutomation = nullptr; }
    if (comInitialized) CoUninitialize();
    return 0;
}

} // namespace

BOOL WhTool_ModInit() {
    InterlockedExchange(&g_shuttingDown, 0);
    g_modInstance = GetModuleHandleW(nullptr);
    GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                           GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                       reinterpret_cast<LPCWSTR>(&Wh_ModInit), &g_modModule);

    int storedMode = Wh_GetIntValue(L"ActiveMode", static_cast<int>(CaseMode::Title));
    if (!IsValidMode(storedMode)) {
        storedMode = static_cast<int>(CaseMode::Title);
    }
    InterlockedExchange(&g_activeMode, storedMode);

    g_readyEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_readyEvent) {
        Wh_Log(L"Text Capitalization: couldn't create the startup event.");
        return FALSE;
    }

    g_threadHandle = CreateThread(nullptr, 0, MessageThreadProc, nullptr, 0, &g_threadId);
    if (!g_threadHandle) {
        CloseHandle(g_readyEvent);
        g_readyEvent = nullptr;
        Wh_Log(L"Text Capitalization: couldn't create the message thread.");
        return FALSE;
    }

    if (WaitForSingleObject(g_readyEvent, 4000) != WAIT_OBJECT_0 ||
        InterlockedCompareExchange(&g_threadReadyState, 0, 0) != 1) {
        InterlockedExchange(&g_shuttingDown, 1);
        if (g_messageWindow) {
            PostMessageW(g_messageWindow, WM_APP_SHUTDOWN, 0, 0);
        } else if (g_threadId) {
            PostThreadMessageW(g_threadId, WM_QUIT, 0, 0);
        }
        WaitForSingleObject(g_threadHandle, INFINITE);
        CloseHandle(g_threadHandle);
        g_threadHandle = nullptr;
        CloseHandle(g_readyEvent);
        g_readyEvent = nullptr;
        Wh_Log(L"Text Capitalization: startup failed. Check the mod log for hook errors.");
        return FALSE;
    }

    return TRUE;
}

void WhTool_ModSettingsChanged() {
    if (g_messageWindow && IsWindow(g_messageWindow)) {
        PostMessageW(g_messageWindow, WM_APP_RELOAD_SHORTCUTS, 0, 0);
    }
}

void WhTool_ModUninit() {
    InterlockedExchange(&g_shuttingDown, 1);

    HANDLE applyThread = nullptr;
    HANDLE captureThread = nullptr;
    AcquireSRWLockExclusive(&g_lifecycleLock);
    applyThread = g_applyThreadHandle;
    g_applyThreadHandle = nullptr;
    captureThread = g_captureThreadHandle;
    g_captureThreadHandle = nullptr;
    ReleaseSRWLockExclusive(&g_lifecycleLock);

    // Keep the hidden owner window's message loop alive until the worker
    // threads finish; OLE clipboard restoration can need that loop to service
    // messages.
    if (captureThread) {
        WaitForSingleObject(captureThread, INFINITE);
        CloseHandle(captureThread);
    }
    if (applyThread) {
        WaitForSingleObject(applyThread, INFINITE);
        CloseHandle(applyThread);
    }

    if (g_messageWindow && IsWindow(g_messageWindow)) {
        PostMessageW(g_messageWindow, WM_APP_SHUTDOWN, 0, 0);
    } else if (g_threadId) {
        PostThreadMessageW(g_threadId, WM_QUIT, 0, 0);
    }

    if (g_threadHandle) {
        WaitForSingleObject(g_threadHandle, INFINITE);
        CloseHandle(g_threadHandle);
        g_threadHandle = nullptr;
    }
    if (g_readyEvent) {
        CloseHandle(g_readyEvent);
        g_readyEvent = nullptr;
    }
}

// Paste the code below as part of the mod code, and use these callbacks:
// * WhTool_ModInit
// * WhTool_ModSettingsChanged
// * WhTool_ModUninit
//
// Currently, other callbacks are not supported.

bool g_isToolModProcessLauncher;
HANDLE g_toolModProcessMutex;

void WINAPI EntryPoint_Hook() {
    Wh_Log(L">");
    ExitThread(0);
}

BOOL Wh_ModInit() {
    DWORD sessionId;
    if (ProcessIdToSessionId(GetCurrentProcessId(), &sessionId) &&
        sessionId == 0) {
        return FALSE;
    }

    bool isExcluded = false;
    bool isToolModProcess = false;
    bool isCurrentToolModProcess = false;
    int argc;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLine(), &argc);
    if (!argv) {
        Wh_Log(L"CommandLineToArgvW failed");
        return FALSE;
    }

    for (int i = 1; i < argc; i++) {
        if (wcscmp(argv[i], L"-service") == 0 ||
            wcscmp(argv[i], L"-service-start") == 0 ||
            wcscmp(argv[i], L"-service-stop") == 0) {
            isExcluded = true;
            break;
        }
    }

    for (int i = 1; i < argc - 1; i++) {
        if (wcscmp(argv[i], L"-tool-mod") == 0) {
            isToolModProcess = true;
            if (wcscmp(argv[i + 1], WH_MOD_ID) == 0) {
                isCurrentToolModProcess = true;
            }
            break;
        }
    }

    LocalFree(argv);

    if (isExcluded) {
        return FALSE;
    }

    if (isCurrentToolModProcess) {
        g_toolModProcessMutex =
            CreateMutex(nullptr, TRUE, L"windhawk-tool-mod_" WH_MOD_ID);
        if (!g_toolModProcessMutex) {
            Wh_Log(L"CreateMutex failed");
            ExitProcess(1);
        }

        if (GetLastError() == ERROR_ALREADY_EXISTS) {
            Wh_Log(L"Tool mod already running (%s)", WH_MOD_ID);
            ExitProcess(1);
        }

        if (!WhTool_ModInit()) {
            ExitProcess(1);
        }

        IMAGE_DOS_HEADER* dosHeader =
            (IMAGE_DOS_HEADER*)GetModuleHandle(nullptr);
        IMAGE_NT_HEADERS* ntHeaders =
            (IMAGE_NT_HEADERS*)((BYTE*)dosHeader + dosHeader->e_lfanew);

        DWORD entryPointRVA = ntHeaders->OptionalHeader.AddressOfEntryPoint;
        void* entryPoint = (BYTE*)dosHeader + entryPointRVA;

        Wh_SetFunctionHook(entryPoint, (void*)EntryPoint_Hook, nullptr);
        return TRUE;
    }

    if (isToolModProcess) {
        return FALSE;
    }

    g_isToolModProcessLauncher = true;
    return TRUE;
}

void Wh_ModAfterInit() {
    if (!g_isToolModProcessLauncher) {
        return;
    }

    WCHAR currentProcessPath[MAX_PATH];
    switch (GetModuleFileName(nullptr, currentProcessPath,
                              ARRAYSIZE(currentProcessPath))) {
        case 0:
        case ARRAYSIZE(currentProcessPath):
            Wh_Log(L"GetModuleFileName failed");
            return;
    }

    WCHAR
    commandLine[MAX_PATH + 2 +
                (sizeof(L" -tool-mod \"" WH_MOD_ID "\"") / sizeof(WCHAR)) - 1];
    swprintf_s(commandLine, L"\"%s\" -tool-mod \"%s\"", currentProcessPath,
               WH_MOD_ID);

    HMODULE kernelModule = GetModuleHandle(L"kernelbase.dll");
    if (!kernelModule) {
        kernelModule = GetModuleHandle(L"kernel32.dll");
        if (!kernelModule) {
            Wh_Log(L"No kernelbase.dll/kernel32.dll");
            return;
        }
    }

    using CreateProcessInternalW_t = BOOL(WINAPI*)(
        HANDLE hUserToken, LPCWSTR lpApplicationName, LPWSTR lpCommandLine,
        LPSECURITY_ATTRIBUTES lpProcessAttributes,
        LPSECURITY_ATTRIBUTES lpThreadAttributes, WINBOOL bInheritHandles,
        DWORD dwCreationFlags, LPVOID lpEnvironment, LPCWSTR lpCurrentDirectory,
        LPSTARTUPINFOW lpStartupInfo,
        LPPROCESS_INFORMATION lpProcessInformation,
        PHANDLE hRestrictedUserToken);
    CreateProcessInternalW_t pCreateProcessInternalW =
        (CreateProcessInternalW_t)GetProcAddress(kernelModule,
                                                 "CreateProcessInternalW");
    if (!pCreateProcessInternalW) {
        Wh_Log(L"No CreateProcessInternalW");
        return;
    }

    STARTUPINFO si{
        .cb = sizeof(STARTUPINFO),
        .dwFlags = STARTF_FORCEOFFFEEDBACK,
    };
    PROCESS_INFORMATION pi;
    if (!pCreateProcessInternalW(nullptr, currentProcessPath, commandLine,
                                 nullptr, nullptr, FALSE, NORMAL_PRIORITY_CLASS,
                                 nullptr, nullptr, &si, &pi, nullptr)) {
        Wh_Log(L"CreateProcess failed");
        return;
    }

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}

void Wh_ModSettingsChanged() {
    if (g_isToolModProcessLauncher) {
        return;
    }

    WhTool_ModSettingsChanged();
}

void Wh_ModUninit() {
    if (g_isToolModProcessLauncher) {
        return;
    }

    WhTool_ModUninit();
    ExitProcess(0);
}