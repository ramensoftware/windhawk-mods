// ==WindhawkMod==
// @id              disable-usb-scan-fix
// @name            Disable USB Scan and Fix Dialog Windows 11
// @description     Blocks matching USB scan dialogs and disk repair notifications
// @description:ru-RU Блокирует соответствующие окна и уведомления о проверке USB-накопителей
// @version         1.3
// @author          d-p4d-67
// @github          https://github.com/d-p4d-67
// @include         explorer.exe
// @homepage        https://github.com/d-p4d-67/Disable-USB-Scan-and-Fix-Dialog-Windows-11
// @architecture    x86-64
// @compilerOptions -lruntimeobject -lole32 -loleaut32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Disable USB Scan and Fix Dialog Windows 11

Suppresses matching USB drive scan and repair notifications without globally
disabling AutoPlay.

![USB Scan and Fix dialog](https://raw.githubusercontent.com/d-p4d-67/Disable-USB-Scan-and-Fix-Dialog-Windows-11/main/Disk_Check_dialog.png)

## Features

- Automatically loads localized warning text from Windows system resources
  (`twinui.dll`, string ID 9995).
- No manual translation is needed when the corresponding resource is available.
- English and Russian text patterns provide fallback matching.
- Prevents associated scan dialogs from opening through suppressed notifications.
- Non-matching notifications and dialogs are passed through.

## Testing

Tested on Windows 11 with Russian, German and French (Luxembourg) language
settings, including after a Windows restart. On the tested USB drive, the
warning notification, its associated dialog and automatic opening of File
Explorer were suppressed. Other notifications continued to work.

**Warning:** This mod hides warnings. It does not repair filesystem errors
or prevent data loss. Back up important files and check drives manually when needed.

---

# Отключение предупреждений о проверке USB-накопителей в Windows 11

Мод блокирует соответствующие предупреждения о проверке и исправлении
USB-накопителей без глобального отключения автозапуска.

## Возможности

- Автоматическая загрузка локализованного текста из системных ресурсов Windows
  (`twinui.dll`, идентификатор строки 9995).
- Ручной перевод не требуется, если соответствующий ресурс доступен.
- Русские и английские шаблоны служат резервом.
- Предотвращает открытие связанных диалоговых окон через заблокированные уведомления.
- Уведомления и диалоги, не соответствующие фильтру, пропускаются.

## Тестирование

Проверено на Windows 11 с русскими, немецкими и французскими (Люксембург)
языковыми настройками, в том числе после перезагрузки Windows. На проверенном
USB-накопителе не появлялись предупреждение, связанное диалоговое окно и
автоматическое открытие Проводника. Остальные уведомления продолжали работать.

**Важно:** мод скрывает предупреждения, но не исправляет ошибки файловой
системы и не предотвращает потерю данных. Сохраняйте резервные копии и при
необходимости проверяйте накопители вручную.
*/
// ==/WindhawkModReadme==

#include <windows.h>
#include <roapi.h>
#include <commctrl.h>
#include <string>
#include <vector>
#include <algorithm>
#include <utility>
#include <windhawk_utils.h>
#include <winrt/Windows.UI.Notifications.h>
#include <winrt/Windows.Data.Xml.Dom.h>

using namespace winrt;
using namespace winrt::Windows::UI::Notifications;

std::wstring Trim(std::wstring s) {
    const auto first = s.find_first_not_of(L" \t\r\n");
    if (first == std::wstring::npos) return {};
    return s.substr(first, s.find_last_not_of(L" \t\r\n") - first + 1);
}

std::wstring NormalizeLabel(std::wstring s) {
    s.erase(std::remove(s.begin(), s.end(), L'&'), s.end());
    return Trim(std::move(s));
}

bool Equal(const std::wstring& a, const std::wstring& b) {
    return !a.empty() && !b.empty() && a.size() == b.size() &&
        CompareStringOrdinal(a.data(), static_cast<int>(a.size()),
                             b.data(), static_cast<int>(b.size()), TRUE) == CSTR_EQUAL;
}

bool Contains(const std::wstring& text, const std::wstring& pattern) {
    if (pattern.empty() || text.size() < pattern.size()) return false;
    for (size_t i = 0; i <= text.size() - pattern.size(); ++i) {
        if (CompareStringOrdinal(text.data() + i, static_cast<int>(pattern.size()),
                                 pattern.data(), static_cast<int>(pattern.size()), TRUE) == CSTR_EQUAL)
            return true;
    }
    return false;
}

bool KnownWarning(const std::wstring& text) {
    return (Contains(text, L"Возникла проблема с этим диском") &&
            Contains(text, L"Проверьте диск")) ||
           ((Contains(text, L"There's a problem with this drive") ||
             Contains(text, L"There is a problem with this drive") ||
             Contains(text, L"There\u2019s a problem with this drive")) &&
            Contains(text, L"scan"));
}

struct ResourceModule {
    HMODULE handle;
    ~ResourceModule() { if (handle) FreeLibrary(handle); }
};

std::vector<std::wstring> g_warningTexts;

void DiscoverLocalizedStrings() {
    wchar_t directory[MAX_PATH] = {};
    UINT n = GetSystemDirectoryW(directory, ARRAYSIZE(directory));
    if (!n || n >= ARRAYSIZE(directory)) return;
    const std::wstring root(directory, n);
    constexpr DWORD flags = LOAD_LIBRARY_AS_DATAFILE | LOAD_LIBRARY_AS_IMAGE_RESOURCE;
    // Let Windows resolve the language resources of the base PE module.
    const auto warningPath = root + L"\\twinui.dll";
    {
        ResourceModule module{LoadLibraryExW(warningPath.c_str(), nullptr, flags)};
        if (module.handle) {
            wchar_t buffer[4096] = {};
            int length = LoadStringW(module.handle, 9995, buffer, ARRAYSIZE(buffer));
            if (length > 0) {
                auto warning = Trim(std::wstring(buffer, length));
                if (!warning.empty() && warning.find(L'%') == std::wstring::npos)
                    g_warningTexts.push_back(warning);
            } else {
                Wh_Log(L"Windows loader resource 9995 unavailable: %lu", GetLastError());
            }
        } else {
            Wh_Log(L"Base twinui.dll unavailable: %lu", GetLastError());
        }
    }
    Wh_Log(L"Automatic notification resources: %u",
           static_cast<unsigned>(g_warningTexts.size()));
}

std::wstring GetDialogText(HINSTANCE instance, PCWSTR text) {
    if (!text) return {};
    if (!IS_INTRESOURCE(text)) return text;
    wchar_t buffer[4096] = {};
    int length = LoadStringW(instance, static_cast<UINT>(reinterpret_cast<ULONG_PTR>(text)),
                             buffer, ARRAYSIZE(buffer));
    return length > 0 ? std::wstring(buffer, length) : std::wstring();
}

using TaskDialogIndirect_t = HRESULT(WINAPI*)(const TASKDIALOGCONFIG*, int*, int*, BOOL*);
TaskDialogIndirect_t TaskDialogIndirect_Original = nullptr;
HMODULE g_dialogModule = nullptr;

// Keep both labels from the same language pair; never infer from button order.
bool IsButtonPair(const std::wstring& a, const std::wstring& b) {
    return (Contains(a, L"Continue without scanning") && Contains(b, L"Scan and fix")) ||
           (Contains(a, L"Продолжить без проверки") && Contains(b, L"Проверить и исправить"));
}

HRESULT WINAPI TaskDialogIndirect_Hook(const TASKDIALOGCONFIG* config,
                                       int* button, int* radioButton, BOOL* verification) {
    try {

        if (config && config->cbSize == sizeof(TASKDIALOGCONFIG) && config->pButtons &&
            config->cButtons == 2) {
            const auto& a = config->pButtons[0];
            const auto& b = config->pButtons[1];
            auto ta = NormalizeLabel(GetDialogText(config->hInstance, a.pszButtonText));
            auto tb = NormalizeLabel(GetDialogText(config->hInstance, b.pszButtonText));
            if (button && config->cRadioButtons == 0 && !config->pfCallback &&
                !config->dwCommonButtons && a.nButtonID != b.nButtonID) {
                bool ab = IsButtonPair(ta, tb);
                bool ba = IsButtonPair(tb, ta);
                if (ab != ba) {
                    *button = ab ? a.nButtonID : b.nButtonID;
                    if (radioButton) *radioButton = 0;
                    if (verification) *verification = FALSE;
                    Wh_Log(L"USB scan and fix dialog skipped");
                    return S_OK;
                }
            }
        }
    } catch (...) {
        // Fail open.
    }
    return TaskDialogIndirect_Original(config, button, radioButton, verification);
}

bool InitDialogHook() {
    HMODULE module = LoadLibraryExW(L"comctl32.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!module) return false;
    auto target = GetProcAddress(module, "TaskDialogIndirect");
    bool ok = target && WindhawkUtils::SetFunctionHook(
        reinterpret_cast<TaskDialogIndirect_t>(target), TaskDialogIndirect_Hook,
        &TaskDialogIndirect_Original);
    if (ok) g_dialogModule = module;
    else FreeLibrary(module);
    Wh_Log(L"Dialog hook: %s", ok ? L"OK" : L"FAILED");
    return ok;
}

using ToastShow_t = HRESULT(WINAPI*)(void*, void*);
ToastShow_t ToastShow_Original = nullptr;
HMODULE g_toastModule = nullptr;

bool IsDiskWarning(const std::vector<std::wstring>& texts) {
    // The observed AutoPlay warning has two text nodes: drive name and body.
    // This narrows matching, but is not proof of the sender's identity.
    if (texts.size() != 2 || Trim(texts[0]).empty()) return false;
    const auto body = Trim(texts[1]);
    for (const auto& warning : g_warningTexts) {
        if (Equal(body, warning)) {
            return true;
        }
    }
    return KnownWarning(body);
}

HRESULT WINAPI ToastShow_Hook(void* self, void* notification) {
    if (notification) {
        try {

            ToastNotification toast{nullptr};
            winrt::copy_from_abi(toast, notification);
            auto content = toast.Content();
            auto nodes = content.GetElementsByTagName(L"text");
            std::vector<std::wstring> texts;
            for (uint32_t i = 0; i < nodes.Length(); ++i) {
                auto value = nodes.Item(i).InnerText();
                texts.emplace_back(value.c_str(), value.size());
            }
            if (IsDiskWarning(texts)) {
                Wh_Log(L"USB disk warning toast blocked");
                return S_OK;
            }
        } catch (const winrt::hresult_error& e) {
            Wh_Log(L"Toast hook error: 0x%08X", static_cast<unsigned>(e.code().value));
        } catch (...) {
            Wh_Log(L"Toast hook: unexpected error");
        }
    }
    return ToastShow_Original(self, notification);
}

bool InitToastHook() {
    HRESULT hr = RoInitialize(RO_INIT_MULTITHREADED);
    bool initialized = SUCCEEDED(hr);
    if (FAILED(hr) && hr != RPC_E_CHANGED_MODE) return false;
    bool ok = false;
    try {
        auto notifier = ToastNotificationManager::CreateToastNotifier(L"Microsoft.Windows.AutoPlay");
        auto iface = notifier.as<IToastNotifier>();
        void* abi = winrt::get_abi(iface);
        if (abi) {
            void** vtable = *reinterpret_cast<void***>(abi);
            if (GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS,
                                  reinterpret_cast<LPCWSTR>(vtable[6]), &g_toastModule)) {
                ok = WindhawkUtils::SetFunctionHook(reinterpret_cast<ToastShow_t>(vtable[6]),
                                                    ToastShow_Hook, &ToastShow_Original);
                if (!ok) {
                    FreeLibrary(g_toastModule);
                    g_toastModule = nullptr;
                }
            }
        }
    } catch (const winrt::hresult_error& e) {
        Wh_Log(L"Toast initialization failed: 0x%08X", static_cast<unsigned>(e.code().value));
    } catch (...) {
        Wh_Log(L"Unknown toast initialization error");
    }
    if (initialized) RoUninitialize();
    Wh_Log(L"Toast hook: %s", ok ? L"OK" : L"FAILED");
    return ok;
}

BOOL Wh_ModInit() {
    g_warningTexts.clear();
    try { DiscoverLocalizedStrings(); }
    catch (...) { Wh_Log(L"Automatic resource initialization incomplete"); }
    Wh_Log(L"Disable USB Scan and Fix Dialog v1.3");
    bool dialogOk = InitDialogHook();
    bool toastOk = InitToastHook();
    return dialogOk || toastOk;
}

void Wh_ModUninit() {
    if (g_toastModule) { FreeLibrary(g_toastModule); g_toastModule = nullptr; }
    if (g_dialogModule) { FreeLibrary(g_dialogModule); g_dialogModule = nullptr; }
    Wh_Log(L"USB Scan and Fix blocker unloaded");
}
