// ==WindhawkMod==
// @id              disable-usb-scan-fix
// @name            Disable USB Scan and Fix Dialog Windows 11
// @description     Blocks matching USB drive scan and repair notifications
// @description:ru-RU Блокирует соответствующие уведомления о проверке USB-накопителей
// @version         1.4
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
- Prevents scan dialogs from being opened through suppressed notifications.
- Non-matching notifications are passed through.
- Suppressing the warning may also skip the AutoPlay action for that insertion.

## Testing

Version 1.4 was tested on Windows 11 with Russian, German and French (Luxembourg) language
settings, including after a Windows restart. On the tested USB drive, the
warning notification, its associated dialog and automatic opening of File
Explorer were suppressed. Other notifications continued to work.
Version 1.4 uses notification filtering only.

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
- Предотвращает открытие окон проверки через заблокированные уведомления.
- Уведомления, не соответствующие фильтру, пропускаются.
- Блокировка предупреждения может также отменить действие автозапуска при данном подключении.

## Тестирование

Версия 1.4 проверена на Windows 11 с русскими, немецкими и французскими (Люксембург)
языковыми настройками, в том числе после перезагрузки Windows. На проверенном
USB-накопителе не появлялись предупреждение, связанное диалоговое окно и
автоматическое открытие Проводника. Остальные уведомления продолжали работать.
Версия 1.4 использует только фильтрацию уведомлений.

**Важно:** мод скрывает предупреждения, но не исправляет ошибки файловой
системы и не предотвращает потерю данных. Сохраняйте резервные копии и при
необходимости проверяйте накопители вручную.
*/
// ==/WindhawkModReadme==

#include <windows.h>
#include <roapi.h>
#include <string>
#include <vector>
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

std::wstring g_warningText;

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
                    g_warningText = warning;
            } else {
                Wh_Log(L"Windows loader resource 9995 unavailable: %lu", GetLastError());
            }
        } else {
            Wh_Log(L"Base twinui.dll unavailable: %lu", GetLastError());
        }
    }
    Wh_Log(L"Automatic notification resources: %u",
           g_warningText.empty() ? 0u : 1u);
}

using ToastShow_t = HRESULT(WINAPI*)(void*, void*);
ToastShow_t ToastShow_Original = nullptr;
HMODULE g_toastModule = nullptr;

bool IsDiskWarning(const std::vector<std::wstring>& texts) {
    // The observed AutoPlay warning has two text nodes: drive name and body.
    // This narrows matching, but is not proof of the sender's identity.
    if (texts.size() != 2 || Trim(texts[0]).empty()) return false;
    const auto body = Trim(texts[1]);
    return Equal(body, g_warningText) || KnownWarning(body);
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
    try { DiscoverLocalizedStrings(); }
    catch (...) { Wh_Log(L"Automatic resource initialization incomplete"); }
    Wh_Log(L"Disable USB Scan and Fix Dialog v1.4");
    return InitToastHook();
}

void Wh_ModUninit() {
    if (g_toastModule) { FreeLibrary(g_toastModule); g_toastModule = nullptr; }
    Wh_Log(L"USB Scan and Fix blocker unloaded");
}
