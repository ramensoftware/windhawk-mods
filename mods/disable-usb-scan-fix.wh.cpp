// ==WindhawkMod==
// @id              disable-usb-scan-fix
// @name            Disable USB Scan and Fix Dialog
// @name:ru-RU      Отключение предупреждений о проверке USB-накопителей
// @description     Blocks USB scan dialogs and disk repair notifications
// @description:ru-RU Блокирует окна и уведомления о проверке USB-накопителей
// @version         1.1
// @author          d-p4d-67
// @github          https://github.com/d-p4d-67
// @include         explorer.exe
// @include         rundll32.exe
// @compilerOptions -lcomctl32 -luser32 -lruntimeobject -lole32 -loleaut32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Disable USB Scan and Fix Dialog

Suppresses matching Windows USB drive scan and repair dialogs and
AutoPlay notifications without disabling the AutoPlay service.

- Recognizes English and Russian warning text.
- Automatically selects "Continue without scanning" for matching dialogs.
- Leaves non-matching notifications and dialogs unchanged.
- Does not modify the registry or repair the filesystem.

The notification hook and absence of the warning dialog were verified on
the author's Windows installation. Other Windows builds and languages
have not been tested.

**Warning:** This mod hides disk warnings. It does not fix filesystem
errors or protect against data loss. Back up important files and check
drives manually when necessary.

---

# Отключение предупреждений о проверке USB-накопителей

Блокирует соответствующие окна и уведомления автозапуска без отключения
службы автозапуска. Поддерживает русский и английский текст предупреждений.
Другие уведомления и окна, не соответствующие фильтру, не блокируются.

Проверено на компьютере автора. Другие сборки Windows не тестировались.

**Важно:** мод скрывает предупреждения, но не исправляет ошибки файловой
системы и не предотвращает потерю данных.
*/
// ==/WindhawkModReadme==

#include <windows.h>
#include <roapi.h>
#include <commctrl.h>
#include <string>
#include <cwctype>
#include <winrt/Windows.UI.Notifications.h>
#include <winrt/Windows.Data.Xml.Dom.h>

using namespace winrt;
using namespace winrt::Windows::UI::Notifications;

// ========================================
// USB SCAN AND FIX DIALOG
// ========================================

using TaskDialogIndirect_t = HRESULT(WINAPI*)(
    const TASKDIALOGCONFIG*,
    int*,
    int*,
    BOOL*
);

TaskDialogIndirect_t TaskDialogIndirect_Original = nullptr;

std::wstring GetDialogText(
    HINSTANCE instance,
    PCWSTR text
) {
    if (!text) {
        return L"";
    }

    std::wstring result;

    if (IS_INTRESOURCE(text)) {
        wchar_t buffer[4096] = {};

        int length = LoadStringW(
            instance,
            static_cast<UINT>(
                reinterpret_cast<ULONG_PTR>(text)
            ),
            buffer,
            ARRAYSIZE(buffer)
        );

        if (length > 0) {
            result.assign(buffer, length);
        }
    } else {
        result = text;
    }

    return result;
}

bool ContainsIgnoreCase(
    const std::wstring& text,
    const std::wstring& pattern
) {
    if (pattern.empty()) {
        return true;
    }

    if (text.size() < pattern.size()) {
        return false;
    }

    for (size_t i = 0;
         i <= text.size() - pattern.size();
         i++) {

        if (CompareStringOrdinal(
                text.c_str() + i,
                static_cast<int>(pattern.size()),
                pattern.c_str(),
                static_cast<int>(pattern.size()),
                TRUE
            ) == CSTR_EQUAL) {
            return true;
        }
    }

    return false;
}

bool IsSkipButton(std::wstring text) {
    size_t pos;

    while ((pos = text.find(L'&')) !=
           std::wstring::npos) {
        text.erase(pos, 1);
    }

    return
        ContainsIgnoreCase(
            text,
            L"continue without scanning"
        ) ||
        ContainsIgnoreCase(
            text,
            L"продолжить без проверки"
        );
}

bool IsRepairButton(std::wstring text) {
    size_t pos;

    while ((pos = text.find(L'&')) !=
           std::wstring::npos) {
        text.erase(pos, 1);
    }

    return
        ContainsIgnoreCase(
            text,
            L"scan and fix"
        ) ||
        ContainsIgnoreCase(
            text,
            L"проверить и исправить"
        );
}

HRESULT WINAPI TaskDialogIndirect_Hook(
    const TASKDIALOGCONFIG* config,
    int* button,
    int* radioButton,
    BOOL* verification
) {
    if (config &&
        config->cbSize == sizeof(TASKDIALOGCONFIG) &&
        config->pButtons &&
        config->cButtons == 2 &&
        config->cRadioButtons == 0 &&
        !config->pfCallback &&
        button) {

        bool foundSkip = false;
        bool foundRepair = false;
        int skipId = 0;

        for (UINT i = 0; i < config->cButtons; i++) {
            const auto& item = config->pButtons[i];

            std::wstring text = GetDialogText(
                config->hInstance,
                item.pszButtonText
            );

            if (IsSkipButton(text)) {
                foundSkip = true;
                skipId = item.nButtonID;
            }

            if (IsRepairButton(text)) {
                foundRepair = true;
            }
        }

        if (foundSkip && foundRepair) {
            *button = skipId;

            if (radioButton) {
                *radioButton = 0;
            }

            if (verification) {
                *verification = FALSE;
            }

            Wh_Log(
                L"USB scan and fix dialog skipped"
            );

            return S_OK;
        }
    }

    return TaskDialogIndirect_Original(
        config,
        button,
        radioButton,
        verification
    );
}

bool InitDialogHook() {
    HMODULE module = LoadLibraryW(L"comctl32.dll");

    if (!module) {
        Wh_Log(L"Failed to load comctl32.dll");
        return false;
    }

    auto target = GetProcAddress(
        module,
        "TaskDialogIndirect"
    );

    if (!target) {
        Wh_Log(L"TaskDialogIndirect not found");
        return false;
    }

    bool result = Wh_SetFunctionHook(
        reinterpret_cast<void*>(target),
        reinterpret_cast<void*>(
            TaskDialogIndirect_Hook
        ),
        reinterpret_cast<void**>(
            &TaskDialogIndirect_Original
        )
    );

    Wh_Log(
        L"Dialog hook: %s",
        result ? L"OK" : L"FAILED"
    );

    return result;
}

// ========================================
// USB DISK WARNING NOTIFICATIONS
// ========================================

using ToastShow_t = HRESULT(WINAPI*)(
    void*,
    void*
);

ToastShow_t ToastShow_Original = nullptr;

bool IsDiskWarning(const std::wstring& text) {
    bool russian =
        ContainsIgnoreCase(
            text,
            L"Возникла проблема с этим диском"
        ) &&
        ContainsIgnoreCase(
            text,
            L"Проверьте диск"
        );

    bool english =
        (
            ContainsIgnoreCase(
                text,
                L"There's a problem with this drive"
            ) ||
            ContainsIgnoreCase(
                text,
                L"There is a problem with this drive"
            ) ||
            ContainsIgnoreCase(
                text,
                L"There\u2019s a problem with this drive"
            )
        ) &&
        ContainsIgnoreCase(
            text,
            L"scan"
        );

    return russian || english;
}

HRESULT WINAPI ToastShow_Hook(
    void* self,
    void* notification
) {
    if (notification) {
        try {
            ToastNotification toast{nullptr};

            winrt::copy_from_abi(
                toast,
                notification
            );

            auto xml = toast.Content().GetXml();

            std::wstring text(
                xml.c_str(),
                xml.size()
            );

            if (IsDiskWarning(text)) {
                Wh_Log(
                    L"USB disk warning toast blocked"
                );

                return S_OK;
            }

        } catch (...) {
            // Do not block unknown notifications.
        }
    }

    return ToastShow_Original(
        self,
        notification
    );
}

bool InitToastHook() {
    HRESULT hr = RoInitialize(RO_INIT_MULTITHREADED);

    bool initialized = SUCCEEDED(hr);

    if (FAILED(hr) && hr != RPC_E_CHANGED_MODE) {
        Wh_Log(
            L"WinRT initialization failed: 0x%08X",
            static_cast<unsigned int>(hr)
        );

        return false;
    }

    bool result = false;

    try {
        auto notifier =
            ToastNotificationManager::
            CreateToastNotifier(
                L"Microsoft.Windows.AutoPlay"
            );

        auto toastInterface =
            notifier.as<IToastNotifier>();

        void* abi = winrt::get_abi(toastInterface);

        if (abi) {
            void** vtable =
                *reinterpret_cast<void***>(abi);

            result = Wh_SetFunctionHook(
                vtable[6],
                reinterpret_cast<void*>(
                    ToastShow_Hook
                ),
                reinterpret_cast<void**>(
                    &ToastShow_Original
                )
            );
        }

    } catch (const winrt::hresult_error& error) {
        Wh_Log(
            L"Toast initialization failed: %s",
            error.message().c_str()
        );
    } catch (...) {
        Wh_Log(L"Unknown toast initialization error");
    }

    if (initialized) {
        RoUninitialize();
    }

    Wh_Log(
        L"Toast hook: %s",
        result ? L"OK" : L"FAILED"
    );

    return result;
}

// ========================================
// WINDHAWK INITIALIZATION
// ========================================

BOOL Wh_ModInit() {
    Wh_Log(
        L"Disable USB Scan and Fix Dialog v1.1"
    );

    bool dialogOk = InitDialogHook();
    bool toastOk = InitToastHook();

    Wh_Log(
        L"Dialog blocker: %s",
        dialogOk ? L"ACTIVE" : L"INACTIVE"
    );

    Wh_Log(
        L"Notification blocker: %s",
        toastOk ? L"ACTIVE" : L"INACTIVE"
    );

    return dialogOk || toastOk;
}

void Wh_ModUninit() {
    Wh_Log(
        L"USB Scan and Fix blocker unloaded"
    );
}
