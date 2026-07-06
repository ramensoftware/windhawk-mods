// ==WindhawkMod==
// @id              selection-text-ocr
// @name            Selection Text OCR
// @description     Global hotkey to capture a screen region and copy extracted text to the clipboard
// @version         1.0.4
// @author          adfastltda
// @github          https://github.com/adfastltda/selection-text-ocr
// @homepage        https://github.com/adfastltda/selection-text-ocr
// @include         windhawk.exe
// @compilerOptions -lgdi32 -lmsimg32 -lole32
// @license         MIT
// ==/WindhawkMod==

// For bug reports, support, and contributions:
// https://github.com/adfastltda/selection-text-ocr

// ==WindhawkModReadme==
/*
# Selection Text OCR

Capture a screen region with a global hotkey and copy the recognized text to the clipboard.

Inspired by PowerToys Text Extractor / Text Grab.

## Support and development

- **Repository:** https://github.com/adfastltda/selection-text-ocr
- **Issues:** report bugs or request features on GitHub Issues
- **Contributions:** pull requests are welcome on GitHub

## Usage

1. Enable the mod in Windhawk.
2. Press the configured hotkey (default: `Ctrl+Shift+O`).
3. Click and drag to select a region on the screen.
4. Release the mouse button to run OCR.
5. Press `Esc` to cancel.

The extracted text is copied to the clipboard automatically.

## Requirements

- Windows 10/11 with an OCR language pack installed.
- Install language packs via **Settings > Time & language > Language & region > Options > Optical character recognition**.

## Notes

- OCR runs through Windows PowerShell 5.1 using the built-in `Windows.Media.Ocr` API.
- Recognition quality depends on the installed OCR language pack and image quality.
- Very small selections are ignored.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- hotkeyModifier: ctrl+shift
  $name: Hotkey modifier
  $description: 'Modifier keys. Combine with +: alt, ctrl, shift, win. Examples: ctrl+shift, alt+shift, win+ctrl'

- hotkeyKey: O
  $name: Hotkey key
  $description: Single character key combined with the modifier (A-Z, 0-9, or common symbols)

- ocrLanguage: pt-BR
  $name: OCR language
  $description: BCP-47 language tag (for example pt-BR, en-US, es-ES)

- showResultDialog: false
  $name: Show result dialog
  $description: Show a dialog with the extracted text in addition to copying it to the clipboard

- minimumSelectionSize: 8
  $name: Minimum selection size
  $description: Minimum width and height in pixels for a valid selection
*/
// ==/WindhawkModSettings==

#include <windhawk_api.h>

#include <windows.h>
#include <windowsx.h>

#include <algorithm>
#include <atomic>
#include <string>
#include <vector>

namespace {

constexpr UINT kHotkeyId = 1;
constexpr wchar_t kOverlayClassName[] = L"WindhawkSelectionTextOcrOverlay";
constexpr wchar_t kHotkeyClassName[] = L"WindhawkSelectionTextOcrHotkey";

HWND g_hotkeyHwnd = nullptr;
HANDLE g_captureThread = nullptr;
std::atomic<bool> g_captureInProgress{false};
std::atomic<bool> g_stopMessageLoop{false};

UINT g_hotkeyModifiers = MOD_CONTROL | MOD_SHIFT;
UINT g_hotkeyKey = 'O';
wchar_t g_ocrLanguage[32] = L"pt-BR";
bool g_showResultDialog = false;
int g_minimumSelectionSize = 8;

void StartCaptureWorkflow();

//=============================================================================
// Hotkey window (global shortcut via main-thread message loop)
//=============================================================================

void UnregisterGlobalHotkey() {
    if (g_hotkeyHwnd) {
        UnregisterHotKey(g_hotkeyHwnd, kHotkeyId);
    }
}

bool RegisterGlobalHotkey() {
    if (!g_hotkeyHwnd) {
        return false;
    }

    UnregisterGlobalHotkey();

    const BOOL registered =
        RegisterHotKey(g_hotkeyHwnd, kHotkeyId, g_hotkeyModifiers | MOD_NOREPEAT, g_hotkeyKey);
    if (!registered) {
        Wh_Log(L"RegisterHotKey failed: %lu", GetLastError());
        return false;
    }

    Wh_Log(L"Global hotkey registered: modifiers=0x%X key=0x%X", g_hotkeyModifiers, g_hotkeyKey);
    return true;
}

DWORD WINAPI CaptureThreadProc(LPVOID) {
    StartCaptureWorkflow();
    g_captureInProgress = false;
    return 0;
}

void StartCaptureOnWorkerThread() {
    if (g_captureInProgress.exchange(true)) {
        Wh_Log(L"Capture already in progress, ignoring hotkey");
        return;
    }

    if (g_captureThread) {
        WaitForSingleObject(g_captureThread, INFINITE);
        CloseHandle(g_captureThread);
        g_captureThread = nullptr;
    }

    g_captureThread = CreateThread(nullptr, 0, CaptureThreadProc, nullptr, 0, nullptr);
    if (!g_captureThread) {
        g_captureInProgress = false;
        Wh_Log(L"Failed to start capture thread: %lu", GetLastError());
    }
}

LRESULT CALLBACK HotkeyWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
        case WM_HOTKEY:
            if (wParam == kHotkeyId) {
                Wh_Log(L"Global hotkey pressed");
                StartCaptureOnWorkerThread();
            }
            return 0;

        case WM_DESTROY:
            UnregisterGlobalHotkey();
            PostQuitMessage(0);
            return 0;

        default:
            return DefWindowProc(hwnd, message, wParam, lParam);
    }
}

bool CreateHotkeyWindow() {
    static bool classRegistered = false;
    if (!classRegistered) {
        WNDCLASSEXW windowClass{
            .cbSize = sizeof(WNDCLASSEXW),
            .lpfnWndProc = HotkeyWndProc,
            .hInstance = GetModuleHandle(nullptr),
            .lpszClassName = kHotkeyClassName,
        };
        if (!RegisterClassExW(&windowClass)) {
            Wh_Log(L"RegisterClassExW failed: %lu", GetLastError());
            return false;
        }
        classRegistered = true;
    }

    if (g_hotkeyHwnd) {
        return true;
    }

    g_hotkeyHwnd = CreateWindowExW(0, kHotkeyClassName, L"Selection Text OCR Hotkey", 0, 0, 0, 0, 0, HWND_MESSAGE,
                                   nullptr, GetModuleHandle(nullptr), nullptr);
    if (!g_hotkeyHwnd) {
        Wh_Log(L"CreateWindowExW failed: %lu", GetLastError());
        return false;
    }

    return RegisterGlobalHotkey();
}

void DestroyHotkeyWindow() {
    UnregisterGlobalHotkey();

    if (g_captureThread) {
        WaitForSingleObject(g_captureThread, INFINITE);
        CloseHandle(g_captureThread);
        g_captureThread = nullptr;
    }

    if (g_hotkeyHwnd) {
        DestroyWindow(g_hotkeyHwnd);
        g_hotkeyHwnd = nullptr;
    }
}

void RunMainMessageLoop() {
    Wh_Log(L"Main message loop started");

    MSG message{};
    while (!g_stopMessageLoop) {
        const BOOL result = GetMessage(&message, nullptr, 0, 0);
        if (result == 0 || result == -1) {
            break;
        }

        TranslateMessage(&message);
        DispatchMessage(&message);
    }

    Wh_Log(L"Main message loop exiting");
}

//=============================================================================
// Settings helpers
//=============================================================================

UINT ParseModifiers(PCWSTR str) {
    UINT modifiers = 0;
    if (!str) {
        return modifiers;
    }
    if (wcsstr(str, L"alt")) {
        modifiers |= MOD_ALT;
    }
    if (wcsstr(str, L"ctrl")) {
        modifiers |= MOD_CONTROL;
    }
    if (wcsstr(str, L"shift")) {
        modifiers |= MOD_SHIFT;
    }
    if (wcsstr(str, L"win")) {
        modifiers |= MOD_WIN;
    }
    return modifiers;
}

UINT ParseSingleCharKey(PCWSTR str) {
    if (!str || !str[0]) {
        return 0;
    }

    const wchar_t c = str[0];
    if (c >= L'A' && c <= L'Z') {
        return c;
    }
    if (c >= L'a' && c <= L'z') {
        return c - L'a' + L'A';
    }
    if (c >= L'0' && c <= L'9') {
        return c;
    }

    switch (c) {
        case L'!':
            return '1';
        case L'@':
            return '2';
        case L'#':
            return '3';
        case L'$':
            return '4';
        case L'%':
            return '5';
        case L'^':
            return '6';
        case L'&':
            return '7';
        case L'*':
            return '8';
        case L'(':
            return '9';
        case L')':
            return '0';
        case L'`':
        case L'~':
            return VK_OEM_3;
        case L'-':
        case L'_':
            return VK_OEM_MINUS;
        case L'=':
        case L'+':
            return VK_OEM_PLUS;
        case L'[':
        case L'{':
            return VK_OEM_4;
        case L']':
        case L'}':
            return VK_OEM_6;
        case L'\\':
        case L'|':
            return VK_OEM_5;
        case L';':
        case L':':
            return VK_OEM_1;
        case L'\'':
        case L'"':
            return VK_OEM_7;
        case L',':
        case L'<':
            return VK_OEM_COMMA;
        case L'.':
        case L'>':
            return VK_OEM_PERIOD;
        case L'/':
        case L'?':
            return VK_OEM_2;
        case L' ':
            return VK_SPACE;
        default:
            return 0;
    }
}

void LoadSettings() {
    PCWSTR modifier = Wh_GetStringSetting(L"hotkeyModifier");
    const UINT parsedModifier = ParseModifiers(modifier);
    g_hotkeyModifiers = parsedModifier ? parsedModifier : (MOD_CONTROL | MOD_SHIFT);
    Wh_FreeStringSetting(modifier);

    PCWSTR key = Wh_GetStringSetting(L"hotkeyKey");
    const UINT parsedKey = ParseSingleCharKey(key);
    g_hotkeyKey = parsedKey ? parsedKey : 'O';
    Wh_FreeStringSetting(key);

    PCWSTR language = Wh_GetStringSetting(L"ocrLanguage");
    if (language && language[0]) {
        wcsncpy_s(g_ocrLanguage, language, _TRUNCATE);
    } else {
        wcscpy_s(g_ocrLanguage, L"pt-BR");
    }
    Wh_FreeStringSetting(language);

    g_showResultDialog = Wh_GetIntSetting(L"showResultDialog") != 0;

    g_minimumSelectionSize = Wh_GetIntSetting(L"minimumSelectionSize");
    if (g_minimumSelectionSize < 1) {
        g_minimumSelectionSize = 8;
    }

    Wh_Log(L"Settings: modifiers=0x%X key=0x%X language=%s", g_hotkeyModifiers, g_hotkeyKey,
           g_ocrLanguage);
}

//=============================================================================
// Bitmap helpers
//=============================================================================

struct VirtualScreenInfo {
    int x = 0;
    int y = 0;
    int width = 0;
    int height = 0;
};

VirtualScreenInfo GetVirtualScreenInfo() {
    VirtualScreenInfo info;
    info.x = GetSystemMetrics(SM_XVIRTUALSCREEN);
    info.y = GetSystemMetrics(SM_YVIRTUALSCREEN);
    info.width = GetSystemMetrics(SM_CXVIRTUALSCREEN);
    info.height = GetSystemMetrics(SM_CYVIRTUALSCREEN);
    return info;
}

HBITMAP CaptureScreenBitmap(const VirtualScreenInfo& screen) {
    HDC screenDc = GetDC(nullptr);
    if (!screenDc) {
        return nullptr;
    }

    HDC memoryDc = CreateCompatibleDC(screenDc);
    if (!memoryDc) {
        ReleaseDC(nullptr, screenDc);
        return nullptr;
    }

    HBITMAP bitmap =
        CreateCompatibleBitmap(screenDc, screen.width, screen.height);
    if (!bitmap) {
        DeleteDC(memoryDc);
        ReleaseDC(nullptr, screenDc);
        return nullptr;
    }

    HGDIOBJ oldBitmap = SelectObject(memoryDc, bitmap);
    BitBlt(memoryDc, 0, 0, screen.width, screen.height, screenDc, screen.x, screen.y, SRCCOPY);
    SelectObject(memoryDc, oldBitmap);

    DeleteDC(memoryDc);
    ReleaseDC(nullptr, screenDc);
    return bitmap;
}

HBITMAP CropBitmap(HBITMAP source, int x, int y, int width, int height) {
    HDC screenDc = GetDC(nullptr);
    if (!screenDc) {
        return nullptr;
    }

    HDC sourceDc = CreateCompatibleDC(screenDc);
    HDC destDc = CreateCompatibleDC(screenDc);
    if (!sourceDc || !destDc) {
        if (sourceDc) {
            DeleteDC(sourceDc);
        }
        if (destDc) {
            DeleteDC(destDc);
        }
        ReleaseDC(nullptr, screenDc);
        return nullptr;
    }

    HBITMAP cropped = CreateCompatibleBitmap(screenDc, width, height);
    if (!cropped) {
        DeleteDC(sourceDc);
        DeleteDC(destDc);
        ReleaseDC(nullptr, screenDc);
        return nullptr;
    }

    HGDIOBJ oldSource = SelectObject(sourceDc, source);
    HGDIOBJ oldDest = SelectObject(destDc, cropped);
    BitBlt(destDc, 0, 0, width, height, sourceDc, x, y, SRCCOPY);
    SelectObject(sourceDc, oldSource);
    SelectObject(destDc, oldDest);

    DeleteDC(sourceDc);
    DeleteDC(destDc);
    ReleaseDC(nullptr, screenDc);
    return cropped;
}

bool SaveBitmapAsBmp(HBITMAP bitmap, const std::wstring& path) {
    BITMAP bmpInfo{};
    if (!GetObject(bitmap, sizeof(bmpInfo), &bmpInfo)) {
        return false;
    }

    BITMAPINFOHEADER bi{};
    bi.biSize = sizeof(BITMAPINFOHEADER);
    bi.biWidth = bmpInfo.bmWidth;
    bi.biHeight = bmpInfo.bmHeight;
    bi.biPlanes = 1;
    bi.biBitCount = 32;
    bi.biCompression = BI_RGB;

    const DWORD imageSize = bmpInfo.bmWidth * bmpInfo.bmHeight * 4;
    std::vector<BYTE> pixels(imageSize);

    HDC dc = GetDC(nullptr);
    if (!dc) {
        return false;
    }

    if (!GetDIBits(dc, bitmap, 0, bmpInfo.bmHeight, pixels.data(),
                   reinterpret_cast<BITMAPINFO*>(&bi), DIB_RGB_COLORS)) {
        ReleaseDC(nullptr, dc);
        return false;
    }
    ReleaseDC(nullptr, dc);

    BITMAPFILEHEADER bfh{};
    bfh.bfType = 0x4D42;
    bfh.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
    bfh.bfSize = bfh.bfOffBits + imageSize;

    HANDLE file =
        CreateFileW(path.c_str(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (file == INVALID_HANDLE_VALUE) {
        return false;
    }

    DWORD written = 0;
    const bool ok = WriteFile(file, &bfh, sizeof(bfh), &written, nullptr) && written == sizeof(bfh) &&
                    WriteFile(file, &bi, sizeof(bi), &written, nullptr) && written == sizeof(bi) &&
                    WriteFile(file, pixels.data(), imageSize, &written, nullptr) && written == imageSize;
    CloseHandle(file);
    return ok;
}

bool SetClipboardText(const std::wstring& text) {
    if (!OpenClipboard(nullptr)) {
        return false;
    }

    EmptyClipboard();

    const size_t bytes = (text.size() + 1) * sizeof(wchar_t);
    HGLOBAL memory = GlobalAlloc(GMEM_MOVEABLE, bytes);
    if (!memory) {
        CloseClipboard();
        return false;
    }

    void* locked = GlobalLock(memory);
    if (!locked) {
        GlobalFree(memory);
        CloseClipboard();
        return false;
    }

    memcpy(locked, text.c_str(), bytes);
    GlobalUnlock(memory);

    if (!SetClipboardData(CF_UNICODETEXT, memory)) {
        GlobalFree(memory);
        CloseClipboard();
        return false;
    }

    CloseClipboard();
    return true;
}

std::wstring GetTempImagePath() {
    wchar_t tempPath[MAX_PATH]{};
    if (!GetTempPathW(ARRAYSIZE(tempPath), tempPath)) {
        return L"";
    }

    wchar_t filePath[MAX_PATH]{};
    if (!GetTempFileNameW(tempPath, L"whocr", 0, filePath)) {
        return L"";
    }

    DeleteFileW(filePath);
    std::wstring path = filePath;
    path += L".bmp";
    return path;
}

std::wstring ReadProcessOutput(HANDLE readHandle) {
    std::wstring output;
    char buffer[4096]{};
    DWORD bytesRead = 0;

    while (ReadFile(readHandle, buffer, sizeof(buffer) - 1, &bytesRead, nullptr) && bytesRead > 0) {
        buffer[bytesRead] = '\0';
        const int wideLength = MultiByteToWideChar(CP_UTF8, 0, buffer, -1, nullptr, 0);
        if (wideLength <= 1) {
            continue;
        }

        std::wstring chunk(wideLength - 1, L'\0');
        MultiByteToWideChar(CP_UTF8, 0, buffer, -1, chunk.data(), wideLength);
        output += chunk;
    }

    while (!output.empty() && (output.back() == L'\r' || output.back() == L'\n')) {
        output.pop_back();
    }

    return output;
}

std::wstring GetTempScriptPath() {
    wchar_t tempPath[MAX_PATH]{};
    if (!GetTempPathW(ARRAYSIZE(tempPath), tempPath)) {
        return L"";
    }

    wchar_t filePath[MAX_PATH]{};
    if (!GetTempFileNameW(tempPath, L"whocr", 0, filePath)) {
        return L"";
    }

    DeleteFileW(filePath);
    std::wstring path = filePath;
    path += L".ps1";
    return path;
}

std::string WideToUtf8(const std::wstring& value) {
    if (value.empty()) {
        return {};
    }

    const int size =
        WideCharToMultiByte(CP_UTF8, 0, value.c_str(), static_cast<int>(value.size()), nullptr, 0, nullptr, nullptr);
    if (size <= 0) {
        return {};
    }

    std::string result(size, '\0');
    WideCharToMultiByte(CP_UTF8, 0, value.c_str(), static_cast<int>(value.size()), result.data(), size, nullptr,
                        nullptr);
    return result;
}

bool WriteOcrScript(const std::wstring& scriptPath, const std::wstring& imagePath,
                    const std::wstring& language) {
    const std::string imagePathUtf8 = WideToUtf8(imagePath);
    const std::string languageUtf8 = WideToUtf8(language);

    std::string script;
    script.reserve(2048);
    script += "$ErrorActionPreference = 'Stop'\n";
    script += "Add-Type -AssemblyName System.Drawing\n";
    script += "Add-Type -AssemblyName System.Runtime.WindowsRuntime\n";
    script += "[void][Windows.Graphics.Imaging.BitmapDecoder, Windows.Graphics, ContentType = WindowsRuntime]\n";
    script += "[void][Windows.Graphics.Imaging.SoftwareBitmap, Windows.Graphics, ContentType = WindowsRuntime]\n";
    script += "[void][Windows.Media.Ocr.OcrEngine, Windows.Media.Ocr, ContentType = WindowsRuntime]\n";
    script += "[void][Windows.Media.Ocr.OcrResult, Windows.Media.Ocr, ContentType = WindowsRuntime]\n";
    script += "[void][Windows.Globalization.Language, Windows.Globalization, ContentType = WindowsRuntime]\n";
    script += "$runtimeMethods = [System.WindowsRuntimeSystemExtensions].GetMethods()\n";
    script += "$asTaskGeneric = ($runtimeMethods | Where-Object { $_.Name -eq 'AsTask' -and $_.GetParameters().Count -eq 1 -and $_.GetParameters()[0].ParameterType.Name -eq 'IAsyncOperation`1' })[0]\n";
    script += "function Await($WinRtTask, [Type]$ResultType) {\n";
    script += "    $asTask = $asTaskGeneric.MakeGenericMethod($ResultType)\n";
    script += "    $netTask = $asTask.Invoke($null, @($WinRtTask))\n";
    script += "    $netTask.Wait(-1) | Out-Null\n";
    script += "    $netTask.Result\n";
    script += "}\n";
    script += "$imagePath = '";
    script += imagePathUtf8;
    script += "'\n$langTag = '";
    script += languageUtf8;
    script += "'\n$stream = [System.IO.File]::OpenRead($imagePath)\n";
    script += "$randomAccess = [System.IO.WindowsRuntimeStreamExtensions]::AsRandomAccessStream($stream)\n";
    script += "$decoder = Await ([Windows.Graphics.Imaging.BitmapDecoder]::CreateAsync($randomAccess)) ([Windows.Graphics.Imaging.BitmapDecoder])\n";
    script += "$softwareBitmap = Await ($decoder.GetSoftwareBitmapAsync()) ([Windows.Graphics.Imaging.SoftwareBitmap])\n";
    script += "$stream.Dispose()\n";
    script += "$engine = [Windows.Media.Ocr.OcrEngine]::TryCreateFromLanguage((New-Object Windows.Globalization.Language($langTag)))\n";
    script += "if ($null -eq $engine) {\n";
    script += "    $engine = [Windows.Media.Ocr.OcrEngine]::TryCreateFromUserProfileLanguages()\n";
    script += "}\n";
    script += "if ($null -eq $engine) {\n";
    script += "    Write-Error 'No OCR engine available. Install an OCR language pack in Windows Settings.'\n";
    script += "    exit 1\n";
    script += "}\n";
    script += "$result = Await ($engine.RecognizeAsync($softwareBitmap)) ([Windows.Media.Ocr.OcrResult])\n";
    script += "[Console]::OutputEncoding = [System.Text.UTF8Encoding]::new($false)\n";
    script += "Write-Output $result.Text\n";

    HANDLE file = CreateFileW(scriptPath.c_str(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL,
                              nullptr);
    if (file == INVALID_HANDLE_VALUE) {
        return false;
    }

    DWORD written = 0;
    const BOOL ok = WriteFile(file, script.data(), static_cast<DWORD>(script.size()), &written, nullptr);
    CloseHandle(file);
    return ok && written == script.size();
}

std::wstring RunPowerShellOcr(const std::wstring& imagePath, const std::wstring& language) {
    const std::wstring scriptPath = GetTempScriptPath();
    if (scriptPath.empty() || !WriteOcrScript(scriptPath, imagePath, language)) {
        return L"";
    }

    SECURITY_ATTRIBUTES securityAttributes{
        .nLength = sizeof(SECURITY_ATTRIBUTES),
        .bInheritHandle = TRUE,
    };

    HANDLE stdoutRead = nullptr;
    HANDLE stdoutWrite = nullptr;
    if (!CreatePipe(&stdoutRead, &stdoutWrite, &securityAttributes, 0)) {
        DeleteFileW(scriptPath.c_str());
        return L"";
    }

    SetHandleInformation(stdoutRead, HANDLE_FLAG_INHERIT, 0);

    wchar_t commandLine[MAX_PATH * 2]{};
    swprintf_s(commandLine,
               L"powershell.exe -NoProfile -NonInteractive -STA -ExecutionPolicy Bypass -File \"%s\"",
               scriptPath.c_str());

    STARTUPINFOW startupInfo{
        .cb = sizeof(STARTUPINFOW),
        .dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW,
        .wShowWindow = SW_HIDE,
        .hStdOutput = stdoutWrite,
        .hStdError = stdoutWrite,
    };
    PROCESS_INFORMATION processInfo{};

    const BOOL created =
        CreateProcessW(nullptr, commandLine, nullptr, nullptr, TRUE, CREATE_NO_WINDOW, nullptr, nullptr,
                     &startupInfo, &processInfo);
    CloseHandle(stdoutWrite);

    if (!created) {
        CloseHandle(stdoutRead);
        DeleteFileW(scriptPath.c_str());
        Wh_Log(L"CreateProcess failed: %lu", GetLastError());
        return L"";
    }

    const std::wstring output = ReadProcessOutput(stdoutRead);
    CloseHandle(stdoutRead);

    WaitForSingleObject(processInfo.hProcess, INFINITE);
    CloseHandle(processInfo.hProcess);
    CloseHandle(processInfo.hThread);
    DeleteFileW(scriptPath.c_str());
    return output;
}

//=============================================================================
// Region selection overlay
//=============================================================================

struct CaptureState {
    HWND hwnd = nullptr;
    HBITMAP screenshot = nullptr;
    VirtualScreenInfo screen{};
    bool dragging = false;
    int startX = 0;
    int startY = 0;
    int currentX = 0;
    int currentY = 0;
    bool cancelled = false;
    bool completed = false;
    RECT selection{};
};

CaptureState g_captureState;
HHOOK g_keyboardHook = nullptr;

void CancelCaptureOverlay() {
    g_captureState.cancelled = true;
    if (g_captureState.dragging) {
        ReleaseCapture();
        g_captureState.dragging = false;
    }
    if (g_captureState.hwnd) {
        PostMessage(g_captureState.hwnd, WM_CLOSE, 0, 0);
    }
}

LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION && (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN)) {
        const auto* keyboard = reinterpret_cast<KBDLLHOOKSTRUCT*>(lParam);
        if (keyboard->vkCode == VK_ESCAPE && g_captureState.hwnd) {
            CancelCaptureOverlay();
            return 1;
        }
    }
    return CallNextHookEx(g_keyboardHook, nCode, wParam, lParam);
}

bool InstallCaptureKeyboardHook() {
    if (g_keyboardHook) {
        return true;
    }

    g_keyboardHook =
        SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, GetModuleHandle(nullptr), 0);
    if (!g_keyboardHook) {
        Wh_Log(L"SetWindowsHookEx failed: %lu", GetLastError());
        return false;
    }
    return true;
}

void UninstallCaptureKeyboardHook() {
    if (g_keyboardHook) {
        UnhookWindowsHookEx(g_keyboardHook);
        g_keyboardHook = nullptr;
    }
}

RECT NormalizeRect(int x1, int y1, int x2, int y2) {
    RECT rect{};
    rect.left = std::min(x1, x2);
    rect.top = std::min(y1, y2);
    rect.right = std::max(x1, x2);
    rect.bottom = std::max(y1, y2);
    return rect;
}

void PaintOverlay(HDC targetDc, const RECT& clientRect) {
    if (!g_captureState.screenshot) {
        return;
    }

    const int width = clientRect.right - clientRect.left;
    const int height = clientRect.bottom - clientRect.top;

    HDC frameDc = CreateCompatibleDC(targetDc);
    HDC shotDc = CreateCompatibleDC(targetDc);
    if (!frameDc || !shotDc) {
        if (frameDc) {
            DeleteDC(frameDc);
        }
        if (shotDc) {
            DeleteDC(shotDc);
        }
        return;
    }

    HBITMAP frameBitmap = CreateCompatibleBitmap(targetDc, width, height);
    HGDIOBJ oldFrame = SelectObject(frameDc, frameBitmap);
    HGDIOBJ oldShot = SelectObject(shotDc, g_captureState.screenshot);

    BitBlt(frameDc, 0, 0, width, height, shotDc, 0, 0, SRCCOPY);

    HBITMAP dimBitmap = CreateCompatibleBitmap(targetDc, width, height);
    HDC dimDc = CreateCompatibleDC(targetDc);
    if (dimBitmap && dimDc) {
        HGDIOBJ oldDim = SelectObject(dimDc, dimBitmap);
        RECT dimRect{0, 0, width, height};
        HBRUSH dimBrush = CreateSolidBrush(RGB(0, 0, 0));
        FillRect(dimDc, &dimRect, dimBrush);
        DeleteObject(dimBrush);

        BLENDFUNCTION blend{
            .BlendOp = AC_SRC_OVER,
            .SourceConstantAlpha = 128,
            .AlphaFormat = 0,
        };
        AlphaBlend(frameDc, 0, 0, width, height, dimDc, 0, 0, width, height, blend);
        SelectObject(dimDc, oldDim);
        DeleteDC(dimDc);
        DeleteObject(dimBitmap);
    }

    if (g_captureState.dragging || g_captureState.completed) {
        const RECT selection = g_captureState.dragging
                                   ? NormalizeRect(g_captureState.startX, g_captureState.startY,
                                                   g_captureState.currentX, g_captureState.currentY)
                                   : g_captureState.selection;

        if (selection.right > selection.left && selection.bottom > selection.top) {
            const int selWidth = selection.right - selection.left;
            const int selHeight = selection.bottom - selection.top;
            BitBlt(frameDc, selection.left, selection.top, selWidth, selHeight, shotDc, selection.left,
                   selection.top, SRCCOPY);

            HPEN borderPen = CreatePen(PS_SOLID, 2, RGB(0, 120, 215));
            HGDIOBJ oldPen = SelectObject(frameDc, borderPen);
            HGDIOBJ oldBrush = SelectObject(frameDc, GetStockObject(HOLLOW_BRUSH));
            Rectangle(frameDc, selection.left, selection.top, selection.right, selection.bottom);
            SelectObject(frameDc, oldBrush);
            SelectObject(frameDc, oldPen);
            DeleteObject(borderPen);
        }
    }

    BitBlt(targetDc, 0, 0, width, height, frameDc, 0, 0, SRCCOPY);

    SelectObject(frameDc, oldFrame);
    SelectObject(shotDc, oldShot);
    DeleteObject(frameBitmap);
    DeleteDC(frameDc);
    DeleteDC(shotDc);
}

LRESULT CALLBACK OverlayWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
        case WM_ERASEBKGND:
            return 1;

        case WM_PAINT: {
            PAINTSTRUCT paint{};
            HDC hdc = BeginPaint(hwnd, &paint);
            RECT clientRect{};
            GetClientRect(hwnd, &clientRect);
            PaintOverlay(hdc, clientRect);
            EndPaint(hwnd, &paint);
            return 0;
        }

        case WM_LBUTTONDOWN:
            g_captureState.dragging = true;
            g_captureState.startX = GET_X_LPARAM(lParam);
            g_captureState.startY = GET_Y_LPARAM(lParam);
            g_captureState.currentX = g_captureState.startX;
            g_captureState.currentY = g_captureState.startY;
            SetCapture(hwnd);
            InvalidateRect(hwnd, nullptr, FALSE);
            return 0;

        case WM_MOUSEMOVE:
            if (g_captureState.dragging) {
                g_captureState.currentX = GET_X_LPARAM(lParam);
                g_captureState.currentY = GET_Y_LPARAM(lParam);
                InvalidateRect(hwnd, nullptr, FALSE);
            }
            return 0;

        case WM_LBUTTONUP:
            if (g_captureState.dragging) {
                ReleaseCapture();
                g_captureState.dragging = false;
                g_captureState.currentX = GET_X_LPARAM(lParam);
                g_captureState.currentY = GET_Y_LPARAM(lParam);
                g_captureState.selection =
                    NormalizeRect(g_captureState.startX, g_captureState.startY, g_captureState.currentX,
                                  g_captureState.currentY);

                const int selWidth = g_captureState.selection.right - g_captureState.selection.left;
                const int selHeight = g_captureState.selection.bottom - g_captureState.selection.top;
                if (selWidth >= g_minimumSelectionSize && selHeight >= g_minimumSelectionSize) {
                    g_captureState.completed = true;
                    DestroyWindow(hwnd);
                } else {
                    g_captureState.selection = {};
                    InvalidateRect(hwnd, nullptr, FALSE);
                }
            }
            return 0;

        case WM_KEYDOWN:
        case WM_SYSKEYDOWN:
            if (wParam == VK_ESCAPE) {
                CancelCaptureOverlay();
            }
            return 0;

        case WM_CLOSE:
            g_captureState.cancelled = true;
            if (g_captureState.dragging) {
                ReleaseCapture();
                g_captureState.dragging = false;
            }
            DestroyWindow(hwnd);
            return 0;

        case WM_DESTROY:
            if (g_captureState.screenshot) {
                DeleteObject(g_captureState.screenshot);
                g_captureState.screenshot = nullptr;
            }
            PostQuitMessage(0);
            return 0;

        default:
            return DefWindowProc(hwnd, message, wParam, lParam);
    }
}

bool RegisterOverlayWindowClass() {
    static bool registered = false;
    if (registered) {
        return true;
    }

    WNDCLASSEXW windowClass{
        .cbSize = sizeof(WNDCLASSEXW),
        .lpfnWndProc = OverlayWndProc,
        .hInstance = GetModuleHandle(nullptr),
        .hCursor = LoadCursor(nullptr, IDC_CROSS),
        .lpszClassName = kOverlayClassName,
    };

    registered = RegisterClassExW(&windowClass) != 0;
    return registered;
}

bool CaptureRegion(RECT* outSelection) {
    if (!RegisterOverlayWindowClass()) {
        return false;
    }

    const VirtualScreenInfo screen = GetVirtualScreenInfo();
    g_captureState = {};
    g_captureState.screen = screen;
    g_captureState.screenshot = CaptureScreenBitmap(screen);
    if (!g_captureState.screenshot) {
        return false;
    }

    g_captureState.hwnd = CreateWindowExW(
        WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE, kOverlayClassName, L"Selection Text OCR",
        WS_POPUP, screen.x, screen.y, screen.width, screen.height, nullptr, nullptr, GetModuleHandle(nullptr),
        nullptr);
    if (!g_captureState.hwnd) {
        DeleteObject(g_captureState.screenshot);
        g_captureState.screenshot = nullptr;
        return false;
    }

    SetWindowPos(g_captureState.hwnd, HWND_TOPMOST, screen.x, screen.y, screen.width, screen.height,
                 SWP_SHOWWINDOW | SWP_NOACTIVATE);
    UpdateWindow(g_captureState.hwnd);

    InstallCaptureKeyboardHook();

    MSG message{};
    while (!g_captureState.completed && !g_captureState.cancelled) {
        if (!GetMessage(&message, nullptr, 0, 0)) {
            break;
        }
        TranslateMessage(&message);
        DispatchMessage(&message);
    }

    UninstallCaptureKeyboardHook();

    if (g_captureState.cancelled || !g_captureState.completed) {
        if (IsWindow(g_captureState.hwnd)) {
            DestroyWindow(g_captureState.hwnd);
        }
        return false;
    }

    outSelection->left = g_captureState.selection.left + screen.x;
    outSelection->top = g_captureState.selection.top + screen.y;
    outSelection->right = g_captureState.selection.right + screen.x;
    outSelection->bottom = g_captureState.selection.bottom + screen.y;
    return true;
}

void ProcessCapturedRegion(const RECT& screenSelection) {
    const VirtualScreenInfo screen = GetVirtualScreenInfo();
    HBITMAP fullBitmap = CaptureScreenBitmap(screen);
    if (!fullBitmap) {
        Wh_Log(L"Failed to capture screen bitmap");
        return;
    }

    const int localLeft = screenSelection.left - screen.x;
    const int localTop = screenSelection.top - screen.y;
    const int width = screenSelection.right - screenSelection.left;
    const int height = screenSelection.bottom - screenSelection.top;

    HBITMAP cropped = CropBitmap(fullBitmap, localLeft, localTop, width, height);
    DeleteObject(fullBitmap);
    if (!cropped) {
        Wh_Log(L"Failed to crop bitmap");
        return;
    }

    const std::wstring imagePath = GetTempImagePath();
    if (imagePath.empty()) {
        DeleteObject(cropped);
        Wh_Log(L"Failed to create temp image path");
        return;
    }

    if (!SaveBitmapAsBmp(cropped, imagePath)) {
        DeleteObject(cropped);
        Wh_Log(L"Failed to save bitmap");
        return;
    }
    DeleteObject(cropped);

    Wh_Log(L"Running OCR on %s", imagePath.c_str());
    const std::wstring text = RunPowerShellOcr(imagePath, g_ocrLanguage);
    DeleteFileW(imagePath.c_str());

    if (text.empty()) {
        Wh_Log(L"OCR returned no text");
        MessageBoxW(nullptr,
                    L"Nenhum texto foi reconhecido. Verifique se o pacote de idioma OCR esta instalado em "
                    L"Configuracoes > Hora e idioma > Idioma e regiao.",
                    L"Selection Text OCR", MB_OK | MB_ICONINFORMATION | MB_TOPMOST);
        return;
    }

    if (!SetClipboardText(text)) {
        Wh_Log(L"Failed to copy text to clipboard");
        return;
    }

    Wh_Log(L"Copied %zu characters to clipboard", text.size());

    if (g_showResultDialog) {
        const size_t maxPreview = 2000;
        std::wstring preview = text.substr(0, std::min(text.size(), maxPreview));
        if (text.size() > maxPreview) {
            preview += L"\n...";
        }
        MessageBoxW(nullptr, preview.c_str(), L"Selection Text OCR", MB_OK | MB_ICONINFORMATION | MB_TOPMOST);
    }
}

void StartCaptureWorkflow() {
    RECT selection{};
    if (!CaptureRegion(&selection)) {
        Wh_Log(L"Region capture cancelled");
        return;
    }
    ProcessCapturedRegion(selection);
}

//=============================================================================
// Windhawk tool mod entry points
//=============================================================================

BOOL WhTool_ModInit() {
    Wh_Log(L"Selection Text OCR initializing");
    LoadSettings();
    if (!CreateHotkeyWindow()) {
        Wh_Log(L"Failed to create hotkey window");
        return FALSE;
    }
    return TRUE;
}

void WhTool_ModUninit() {
    g_stopMessageLoop = true;
    if (g_hotkeyHwnd) {
        PostMessage(g_hotkeyHwnd, WM_CLOSE, 0, 0);
    }
    DestroyHotkeyWindow();
}

void WhTool_ModSettingsChanged() {
    LoadSettings();
    RegisterGlobalHotkey();
}

}  // namespace

//=============================================================================
// Windhawk tool mod launcher (runs in dedicated windhawk.exe process)
//=============================================================================

bool g_isToolModProcessLauncher;
HANDLE g_toolModProcessMutex;

void WINAPI EntryPoint_Hook() {
    Wh_Log(L"Entry point hook - starting global hotkey message loop");
    RunMainMessageLoop();
    ExitThread(0);
}

BOOL Wh_ModInit() {
    DWORD sessionId = 0;
    if (ProcessIdToSessionId(GetCurrentProcessId(), &sessionId) && sessionId == 0) {
        return FALSE;
    }

    bool isExcluded = false;
    bool isToolModProcess = false;
    bool isCurrentToolModProcess = false;

    int argc = 0;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLine(), &argc);
    if (!argv) {
        return FALSE;
    }

    for (int i = 1; i < argc; i++) {
        if (wcscmp(argv[i], L"-service") == 0 || wcscmp(argv[i], L"-service-start") == 0 ||
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
        g_toolModProcessMutex = CreateMutex(nullptr, TRUE, L"windhawk-tool-mod_" WH_MOD_ID);
        if (!g_toolModProcessMutex) {
            ExitProcess(1);
        }

        if (GetLastError() == ERROR_ALREADY_EXISTS) {
            Wh_Log(L"Tool mod already running (%s)", WH_MOD_ID);
            ExitProcess(1);
        }

        if (!WhTool_ModInit()) {
            ExitProcess(1);
        }

        IMAGE_DOS_HEADER* dosHeader = reinterpret_cast<IMAGE_DOS_HEADER*>(GetModuleHandle(nullptr));
        IMAGE_NT_HEADERS* ntHeaders =
            reinterpret_cast<IMAGE_NT_HEADERS*>(reinterpret_cast<BYTE*>(dosHeader) + dosHeader->e_lfanew);
        void* entryPoint =
            reinterpret_cast<BYTE*>(dosHeader) + ntHeaders->OptionalHeader.AddressOfEntryPoint;
        Wh_SetFunctionHook(entryPoint, reinterpret_cast<void*>(EntryPoint_Hook), nullptr);
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

    WCHAR currentProcessPath[MAX_PATH]{};
    if (!GetModuleFileName(nullptr, currentProcessPath, ARRAYSIZE(currentProcessPath))) {
        return;
    }

    WCHAR commandLine[MAX_PATH + 64]{};
    swprintf_s(commandLine, L"\"%s\" -tool-mod \"%s\"", currentProcessPath, WH_MOD_ID);

    HMODULE kernelModule = GetModuleHandle(L"kernelbase.dll");
    if (!kernelModule) {
        kernelModule = GetModuleHandle(L"kernel32.dll");
        if (!kernelModule) {
            return;
        }
    }

    using CreateProcessInternalW_t = BOOL(WINAPI*)(
        HANDLE, LPCWSTR, LPWSTR, LPSECURITY_ATTRIBUTES, LPSECURITY_ATTRIBUTES, BOOL, DWORD, LPVOID, LPCWSTR,
        LPSTARTUPINFOW, LPPROCESS_INFORMATION, PHANDLE);
    auto createProcessInternal =
        reinterpret_cast<CreateProcessInternalW_t>(GetProcAddress(kernelModule, "CreateProcessInternalW"));
    if (!createProcessInternal) {
        return;
    }

    STARTUPINFOW startupInfo{
        .cb = sizeof(STARTUPINFOW),
        .dwFlags = STARTF_FORCEOFFFEEDBACK,
    };
    PROCESS_INFORMATION processInfo{};
    if (!createProcessInternal(nullptr, currentProcessPath, commandLine, nullptr, nullptr, FALSE,
                               NORMAL_PRIORITY_CLASS, nullptr, nullptr, &startupInfo, &processInfo, nullptr)) {
        Wh_Log(L"CreateProcess failed");
        return;
    }

    CloseHandle(processInfo.hProcess);
    CloseHandle(processInfo.hThread);
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
