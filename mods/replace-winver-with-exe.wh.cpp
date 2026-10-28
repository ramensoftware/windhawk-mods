// ==WindhawkMod==
// @id              replace-winver-with-exe
// @name            Winver Redirector
// @description     Replaces Winver with an executable, command, or Shell app
// @version         1.1
// @author          winui64
// @github          https://github.com/winui64
// @include         winver.exe
// @compilerOptions -lshell32 -lole32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Redirect Winver to an executable, command, or packaged app

Set **Replacement command or target** to one of:

- `C:\Tools\ModernWinver.exe`
- `"C:\Program Files\Modern Winver\ModernWinver.exe" --about`
- `explorer.exe shell:AppsFolder\PackageFamilyName!AppId`
- `shell:AppsFolder\PackageFamilyName!AppId`
- `ms-settings:about`
- `%LOCALAPPDATA%\Microsoft\WindowsApps\YourApp.exe`
- `cmd.exe /d /c start "" "C:\Tools\ModernWinver.exe"`

Replace `PackageFamilyName!AppId` with the installed app's actual AppUserModelID.
You can find it using PowerShell's `Get-StartApps` command.

Quote a target containing spaces when supplying arguments. Existing unquoted
file paths without arguments are still accepted. Argument quoting is preserved.
For command-shell built-ins, pipes, or redirection, explicitly use `cmd.exe /d /c`.
Shortcuts and registered URI protocols are also supported by Windows Shell.

An empty setting or a launch error falls back to the original Winver dialog.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- replacement_exe: ""
  $name: Replacement command or target
  $description: Executable path with optional arguments, shell:AppsFolder target, shortcut, or URI. Environment variables are supported.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <shellapi.h>
#include <objbase.h>
#include <windhawk_utils.h>
#include <cwchar>
#include <string>
#include <utility>

using ShellAboutW_t = decltype(&ShellAboutW);
ShellAboutW_t ShellAboutW_orig = nullptr;

INT WINAPI ShellAboutW_hook(HWND hWnd, LPCWSTR szApp,
                           LPCWSTR szOtherStuff, HICON hIcon) {
    // Read settings for this launch, so changes take effect without shared state.
    auto replacementSetting =
        WindhawkUtils::StringSetting::make(L"replacement_exe");
    std::wstring command = replacementSetting.get() ? replacementSetting.get() : L"";

    DWORD launchError = ERROR_SUCCESS;
    const DWORD expandedLength =
        ExpandEnvironmentStringsW(command.c_str(), nullptr, 0);
    if (expandedLength) {
        std::wstring expandedCommand(expandedLength, L'\0');
        const DWORD writtenLength = ExpandEnvironmentStringsW(
            command.c_str(), expandedCommand.data(), expandedLength);
        if (!writtenLength) {
            launchError = GetLastError();
        } else if (writtenLength > expandedLength) {
            launchError = ERROR_INSUFFICIENT_BUFFER;
        } else {
            expandedCommand.resize(writtenLength - 1);
            command = std::move(expandedCommand);
        }
    } else {
        launchError = GetLastError();
    }

    const auto firstCharacter = command.find_first_not_of(L" \t\r\n");
    if (firstCharacter == std::wstring::npos) {
        return ShellAboutW_orig(hWnd, szApp, szOtherStuff, hIcon);
    }
    command = command.substr(firstCharacter,
        command.find_last_not_of(L" \t\r\n") - firstCharacter + 1);

    std::wstring target;
    std::wstring arguments;
    if (command.front() == L'"') {
        const auto closingQuote = command.find(L'"', 1);
        if (closingQuote == std::wstring::npos || closingQuote == 1 ||
            (closingQuote + 1 < command.size() &&
             command.find_first_of(L" \t\r\n", closingQuote + 1) != closingQuote + 1)) {
            launchError = ERROR_INVALID_PARAMETER;
        } else {
            target = command.substr(1, closingQuote - 1);
            arguments = command.substr(closingQuote + 1);
        }
    } else if (_wcsnicmp(command.c_str(), L"shell:", 6) == 0 ||
               GetFileAttributesW(command.c_str()) != INVALID_FILE_ATTRIBUTES) {
        // Keep bare Shell targets and existing unquoted paths with spaces intact.
        target = command;
    } else {
        const auto separator = command.find_first_of(L" \t\r\n");
        target = command.substr(0, separator);
        if (separator != std::wstring::npos) {
            arguments = command.substr(separator);
        }
    }

    const auto firstArgument = arguments.find_first_not_of(L" \t\r\n");
    arguments = firstArgument == std::wstring::npos
        ? L"" : arguments.substr(firstArgument);

    if (launchError == ERROR_SUCCESS) {
        const HRESULT comResult = CoInitializeEx(
            nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
        if (FAILED(comResult) && comResult != RPC_E_CHANGED_MODE) {
            launchError = static_cast<DWORD>(comResult);
        } else {
            SHELLEXECUTEINFOW executeInfo = {};
            executeInfo.cbSize = sizeof(executeInfo);
            executeInfo.fMask = SEE_MASK_NOASYNC | SEE_MASK_FLAG_NO_UI;
            if (_wcsnicmp(target.c_str(), L"shell:", 6) == 0) {
                // Shell namespace apps need their Shell verb handler.
                executeInfo.fMask |= SEE_MASK_INVOKEIDLIST;
            }
            executeInfo.hwnd = hWnd;
            executeInfo.lpFile = target.c_str();
            executeInfo.lpParameters = arguments.empty() ? nullptr : arguments.c_str();
            executeInfo.nShow = SW_SHOWNORMAL;

            if (!ShellExecuteExW(&executeInfo)) {
                launchError = GetLastError();
                if (launchError == ERROR_SUCCESS) {
                    launchError = ERROR_GEN_FAILURE;
                }
            }
        }
        if (SUCCEEDED(comResult)) {
            CoUninitialize();
        }
    }

    if (launchError != ERROR_SUCCESS) {
        WCHAR errorMessage[512] = {};
        if (!FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                            nullptr, launchError, 0, errorMessage,
                            ARRAYSIZE(errorMessage), nullptr)) {
            swprintf_s(errorMessage, ARRAYSIZE(errorMessage),
                       L"Unable to launch the replacement (error 0x%08lX).", launchError);
        }
        MessageBoxW(hWnd, errorMessage, L"Failed to launch Winver replacement",
                    MB_OK | MB_ICONERROR);
        return ShellAboutW_orig(hWnd, szApp, szOtherStuff, hIcon);
    }

    return TRUE;
}

BOOL Wh_ModInit(void) {
    if (!Wh_SetFunctionHook(reinterpret_cast<void*>(ShellAboutW),
                            reinterpret_cast<void*>(ShellAboutW_hook),
                            reinterpret_cast<void**>(&ShellAboutW_orig))) {
        Wh_Log(L"Failed to hook ShellAboutW");
        return FALSE;
    }
    return TRUE;
}
