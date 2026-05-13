// ==WindhawkMod==
// @id              clipboard-forever
// @name            Clipboard Forever
// @description     Persist the latest text/image clipboard item and restore it after clipboard clears or Windows restarts
// @version         1.0.0
// @author          Guy
// @github          https://github.com/fggedr
// @license         MIT
// @include         %SystemRoot%\explorer.exe
// @architecture    x86-64
// @compilerOptions -luser32 -lgdi32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Persistent Clipboard Forever

Keeps the latest supported clipboard item in Windhawk's mod storage and restores
it when the clipboard becomes empty, Explorer restarts, or Windows restarts.

Supported data:

- Plain text: Unicode, ANSI, OEM text, and locale metadata.
- Rich text helpers: HTML Format and Rich Text Format.
- Images: DIB, DIBV5, PNG clipboard format, and a fallback conversion from
  CF_BITMAP to DIB.

Notes:

- This keeps the latest supported clipboard item, not a full clipboard history.
- File-copy clipboard data and private app-only clipboard formats are left alone
  so normal Explorer copy/paste workflows keep working.
- "Forever" means as long as Windhawk remains installed and the mod storage file
  is not deleted. The saved clipboard data is not encrypted, so don't use this
  with passwords or secrets unless you accept that they can remain on disk.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- RestoreOnStartup: true
  $name: Restore after Windows restart
  $description: Restore the saved clipboard item if Explorer starts and the clipboard is empty.
- RestoreWhenCleared: true
  $name: Restore when clipboard is cleared
  $description: Put the saved item back when the clipboard becomes empty.
- PersistImages: true
  $name: Persist images
  $description: Save image clipboard formats in addition to text.
- PersistRichText: true
  $name: Persist rich text formats
  $description: Save HTML Format and Rich Text Format when apps provide them.
- PersistPng: true
  $name: Persist PNG image format
  $description: Save the registered PNG clipboard format when apps provide it.
- CaptureDelayMs: 150
  $name: Capture delay in milliseconds
  $description: Small debounce after a clipboard update before saving it.
- MaxFormatMegabytes: 64
  $name: Maximum megabytes per format
  $description: Safety limit for each saved clipboard format.
*/
// ==/WindhawkModSettings==

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <algorithm>
#include <limits>
#include <string>
#include <vector>

namespace {

constexpr wchar_t kWindowClassName[] =
    L"WindhawkPersistentClipboardForeverWindow";
constexpr wchar_t kClipboardFileName[] = L"clipboard.bin";
constexpr wchar_t kClipboardTempFileName[] = L"clipboard.bin.tmp";

constexpr DWORD kFileMagic = 0x31435057;  // WPC1
constexpr DWORD kFileVersion = 1;
constexpr DWORD kMaxStoredFormats = 32;
constexpr DWORD kMaxFormatNameChars = 256;
constexpr ULONGLONG kMaxFileBytes = 1024ull * 1024ull * 1024ull;

constexpr UINT kRegisteredFormatFirst = 0xC000;
constexpr UINT kMsgRestoreStartup = WM_APP + 1;
constexpr UINT kMsgShutdown = WM_APP + 2;
constexpr UINT_PTR kTimerCapture = 1;
constexpr UINT_PTR kTimerRestore = 2;

struct Settings {
    bool restoreOnStartup = true;
    bool restoreWhenCleared = true;
    bool persistImages = true;
    bool persistRichText = true;
    bool persistPng = true;
    DWORD captureDelayMs = 150;
    SIZE_T maxFormatBytes = 64ull * 1024ull * 1024ull;
};

struct ClipboardEntry {
    UINT format = 0;
    std::wstring registeredName;
    std::vector<BYTE> data;
};

enum class CaptureResult {
    Saved,
    Empty,
    Unsupported,
    Failed,
};

Settings g_settings;
HWND g_window = nullptr;
HANDLE g_thread = nullptr;
DWORD g_threadId = 0;
HANDLE g_readyEvent = nullptr;
volatile LONG g_restoring = 0;

int ClampInt(int value, int minValue, int maxValue) {
    return std::max(minValue, std::min(value, maxValue));
}

HMODULE GetCurrentModuleHandle() {
    MEMORY_BASIC_INFORMATION info = {};
    if (!VirtualQuery(reinterpret_cast<void*>(&GetCurrentModuleHandle), &info,
                      sizeof(info))) {
        return nullptr;
    }

    return reinterpret_cast<HMODULE>(info.AllocationBase);
}

void LoadSettings() {
    g_settings.restoreOnStartup = Wh_GetIntSetting(L"RestoreOnStartup") != 0;
    g_settings.restoreWhenCleared =
        Wh_GetIntSetting(L"RestoreWhenCleared") != 0;
    g_settings.persistImages = Wh_GetIntSetting(L"PersistImages") != 0;
    g_settings.persistRichText = Wh_GetIntSetting(L"PersistRichText") != 0;
    g_settings.persistPng = Wh_GetIntSetting(L"PersistPng") != 0;

    int captureDelayMs = Wh_GetIntSetting(L"CaptureDelayMs");
    if (captureDelayMs <= 0) {
        captureDelayMs = 150;
    }
    g_settings.captureDelayMs =
        static_cast<DWORD>(ClampInt(captureDelayMs, 10, 5000));

    int maxFormatMegabytes = Wh_GetIntSetting(L"MaxFormatMegabytes");
    if (maxFormatMegabytes <= 0) {
        maxFormatMegabytes = 64;
    }
    maxFormatMegabytes = ClampInt(maxFormatMegabytes, 1, 512);
    g_settings.maxFormatBytes =
        static_cast<SIZE_T>(maxFormatMegabytes) * 1024ull * 1024ull;
}

std::wstring GetStorageFilePath(PCWSTR fileName) {
    std::vector<wchar_t> storagePath(32768);
    size_t chars =
        Wh_GetModStoragePath(storagePath.data(), storagePath.size());
    if (chars == 0 || chars >= storagePath.size()) {
        return {};
    }

    CreateDirectoryW(storagePath.data(), nullptr);

    std::wstring path(storagePath.data(), chars);
    if (!path.empty() && path.back() != L'\\' && path.back() != L'/') {
        path += L'\\';
    }

    path += fileName;
    return path;
}

template <typename T>
void AppendValue(std::vector<BYTE>& buffer, const T& value) {
    size_t oldSize = buffer.size();
    buffer.resize(oldSize + sizeof(T));
    CopyMemory(buffer.data() + oldSize, &value, sizeof(T));
}

void AppendBytes(std::vector<BYTE>& buffer, const void* data, size_t size) {
    size_t oldSize = buffer.size();
    buffer.resize(oldSize + size);
    CopyMemory(buffer.data() + oldSize, data, size);
}

template <typename T>
bool ReadValue(const std::vector<BYTE>& buffer, size_t& offset, T* value) {
    if (offset > buffer.size() || buffer.size() - offset < sizeof(T)) {
        return false;
    }

    CopyMemory(value, buffer.data() + offset, sizeof(T));
    offset += sizeof(T);
    return true;
}

bool ReadBytes(const std::vector<BYTE>& buffer,
               size_t& offset,
               void* data,
               size_t size) {
    if (offset > buffer.size() || buffer.size() - offset < size) {
        return false;
    }

    CopyMemory(data, buffer.data() + offset, size);
    offset += size;
    return true;
}

bool WriteWholeFile(const std::wstring& path, const std::vector<BYTE>& data) {
    std::wstring tempPath = GetStorageFilePath(kClipboardTempFileName);
    if (path.empty() || tempPath.empty()) {
        return false;
    }

    HANDLE file = CreateFileW(tempPath.c_str(), GENERIC_WRITE, 0, nullptr,
                              CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (file == INVALID_HANDLE_VALUE) {
        Wh_Log(L"CreateFileW for write failed: 0x%x", GetLastError());
        return false;
    }

    bool ok = true;
    size_t offset = 0;
    while (offset < data.size()) {
        DWORD chunk =
            static_cast<DWORD>(std::min<size_t>(data.size() - offset, 1 << 20));
        DWORD written = 0;
        if (!WriteFile(file, data.data() + offset, chunk, &written, nullptr) ||
            written != chunk) {
            Wh_Log(L"WriteFile failed: 0x%x", GetLastError());
            ok = false;
            break;
        }

        offset += written;
    }

    if (ok) {
        FlushFileBuffers(file);
    }

    CloseHandle(file);

    if (!ok) {
        DeleteFileW(tempPath.c_str());
        return false;
    }

    if (!MoveFileExW(tempPath.c_str(), path.c_str(),
                     MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH)) {
        Wh_Log(L"MoveFileExW failed: 0x%x", GetLastError());
        DeleteFileW(tempPath.c_str());
        return false;
    }

    return true;
}

bool ReadWholeFile(const std::wstring& path, std::vector<BYTE>* data) {
    data->clear();
    if (path.empty()) {
        return false;
    }

    HANDLE file =
        CreateFileW(path.c_str(), GENERIC_READ,
                    FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                    nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (file == INVALID_HANDLE_VALUE) {
        return false;
    }

    LARGE_INTEGER fileSize = {};
    if (!GetFileSizeEx(file, &fileSize) || fileSize.QuadPart <= 0 ||
        static_cast<ULONGLONG>(fileSize.QuadPart) > kMaxFileBytes) {
        CloseHandle(file);
        return false;
    }

    data->resize(static_cast<size_t>(fileSize.QuadPart));

    bool ok = true;
    size_t offset = 0;
    while (offset < data->size()) {
        DWORD chunk = static_cast<DWORD>(
            std::min<size_t>(data->size() - offset, 1 << 20));
        DWORD read = 0;
        if (!ReadFile(file, data->data() + offset, chunk, &read, nullptr) ||
            read == 0) {
            ok = false;
            break;
        }

        offset += read;
    }

    CloseHandle(file);

    if (!ok || offset != data->size()) {
        data->clear();
        return false;
    }

    return true;
}

bool GetRegisteredFormatName(UINT format, std::wstring* name) {
    wchar_t buffer[kMaxFormatNameChars] = {};
    int chars = GetClipboardFormatNameW(format, buffer, ARRAYSIZE(buffer));
    if (chars <= 0) {
        return false;
    }

    name->assign(buffer, chars);
    return true;
}

bool IsSupportedHGlobalFormat(UINT format, std::wstring* registeredName) {
    registeredName->clear();

    switch (format) {
        case CF_UNICODETEXT:
        case CF_TEXT:
        case CF_OEMTEXT:
        case CF_LOCALE:
            return true;

        case CF_DIB:
        case CF_DIBV5:
            return g_settings.persistImages;
    }

    if (format < kRegisteredFormatFirst) {
        return false;
    }

    std::wstring name;
    if (!GetRegisteredFormatName(format, &name)) {
        return false;
    }

    if (g_settings.persistRichText &&
        (lstrcmpiW(name.c_str(), L"HTML Format") == 0 ||
         lstrcmpiW(name.c_str(), L"Rich Text Format") == 0)) {
        *registeredName = name;
        return true;
    }

    if (g_settings.persistImages && g_settings.persistPng &&
        lstrcmpiW(name.c_str(), L"PNG") == 0) {
        *registeredName = name;
        return true;
    }

    return false;
}

bool CopyHGlobalClipboardFormat(UINT format,
                                const std::wstring& registeredName,
                                ClipboardEntry* entry) {
    HANDLE dataHandle = GetClipboardData(format);
    if (!dataHandle) {
        return false;
    }

    SIZE_T dataSize = GlobalSize(dataHandle);
    if (dataSize == 0 || dataSize > g_settings.maxFormatBytes) {
        return false;
    }

    void* lockedData = GlobalLock(dataHandle);
    if (!lockedData) {
        return false;
    }

    entry->format = format;
    entry->registeredName = registeredName;
    entry->data.resize(dataSize);
    CopyMemory(entry->data.data(), lockedData, dataSize);

    GlobalUnlock(dataHandle);
    return true;
}

bool CopyBitmapClipboardFallback(ClipboardEntry* entry) {
    HBITMAP bitmap = reinterpret_cast<HBITMAP>(GetClipboardData(CF_BITMAP));
    if (!bitmap) {
        return false;
    }

    BITMAP bitmapInfo = {};
    if (!GetObjectW(bitmap, sizeof(bitmapInfo), &bitmapInfo)) {
        return false;
    }

    LONG width = bitmapInfo.bmWidth;
    LONG height = bitmapInfo.bmHeight >= 0 ? bitmapInfo.bmHeight
                                           : -bitmapInfo.bmHeight;
    if (width <= 0 || height <= 0) {
        return false;
    }

    ULONGLONG stride = static_cast<ULONGLONG>(width) * 4ull;
    ULONGLONG pixelBytes = stride * static_cast<ULONGLONG>(height);
    ULONGLONG totalBytes = sizeof(BITMAPINFOHEADER) + pixelBytes;
    if (totalBytes > g_settings.maxFormatBytes ||
        totalBytes >
            static_cast<ULONGLONG>((std::numeric_limits<size_t>::max)())) {
        return false;
    }

    entry->format = CF_DIB;
    entry->registeredName.clear();
    entry->data.resize(static_cast<size_t>(totalBytes));

    auto* dib = reinterpret_cast<BITMAPINFO*>(entry->data.data());
    dib->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    dib->bmiHeader.biWidth = width;
    dib->bmiHeader.biHeight = height;
    dib->bmiHeader.biPlanes = 1;
    dib->bmiHeader.biBitCount = 32;
    dib->bmiHeader.biCompression = BI_RGB;
    dib->bmiHeader.biSizeImage = static_cast<DWORD>(pixelBytes);

    HDC screenDc = GetDC(nullptr);
    if (!screenDc) {
        entry->data.clear();
        return false;
    }

    int scanLines = GetDIBits(
        screenDc, bitmap, 0, static_cast<UINT>(height),
        entry->data.data() + sizeof(BITMAPINFOHEADER), dib, DIB_RGB_COLORS);

    ReleaseDC(nullptr, screenDc);

    if (scanLines != height) {
        entry->data.clear();
        return false;
    }

    return true;
}

bool IsImageEntry(const ClipboardEntry& entry) {
    if (entry.format == CF_DIB || entry.format == CF_DIBV5) {
        return true;
    }

    return !entry.registeredName.empty() &&
           lstrcmpiW(entry.registeredName.c_str(), L"PNG") == 0;
}

bool SaveEntries(const std::vector<ClipboardEntry>& entries) {
    if (entries.empty() || entries.size() > kMaxStoredFormats) {
        return false;
    }

    std::vector<BYTE> serialized;
    AppendValue(serialized, kFileMagic);
    AppendValue(serialized, kFileVersion);
    AppendValue(serialized, static_cast<DWORD>(entries.size()));

    for (const ClipboardEntry& entry : entries) {
        DWORD nameChars = static_cast<DWORD>(entry.registeredName.size());
        ULONGLONG dataSize = static_cast<ULONGLONG>(entry.data.size());

        AppendValue(serialized, static_cast<DWORD>(entry.format));
        AppendValue(serialized, nameChars);
        AppendValue(serialized, dataSize);

        if (nameChars > 0) {
            AppendBytes(serialized, entry.registeredName.data(),
                        entry.registeredName.size() * sizeof(wchar_t));
        }

        AppendBytes(serialized, entry.data.data(), entry.data.size());
    }

    std::wstring path = GetStorageFilePath(kClipboardFileName);
    if (!WriteWholeFile(path, serialized)) {
        return false;
    }

    Wh_Log(L"Saved %u clipboard format(s)",
           static_cast<unsigned int>(entries.size()));
    return true;
}

bool LoadEntries(std::vector<ClipboardEntry>* entries) {
    entries->clear();

    std::vector<BYTE> serialized;
    if (!ReadWholeFile(GetStorageFilePath(kClipboardFileName), &serialized)) {
        return false;
    }

    size_t offset = 0;
    DWORD magic = 0;
    DWORD version = 0;
    DWORD count = 0;
    if (!ReadValue(serialized, offset, &magic) ||
        !ReadValue(serialized, offset, &version) ||
        !ReadValue(serialized, offset, &count) || magic != kFileMagic ||
        version != kFileVersion || count == 0 || count > kMaxStoredFormats) {
        return false;
    }

    for (DWORD i = 0; i < count; i++) {
        DWORD format = 0;
        DWORD nameChars = 0;
        ULONGLONG dataSize = 0;
        if (!ReadValue(serialized, offset, &format) ||
            !ReadValue(serialized, offset, &nameChars) ||
            !ReadValue(serialized, offset, &dataSize)) {
            return false;
        }

        if (nameChars > kMaxFormatNameChars || dataSize == 0 ||
            dataSize > g_settings.maxFormatBytes ||
            dataSize >
                static_cast<ULONGLONG>(
                    (std::numeric_limits<size_t>::max)())) {
            return false;
        }

        ClipboardEntry entry;
        entry.format = static_cast<UINT>(format);

        if (nameChars > 0) {
            entry.registeredName.resize(nameChars);
            if (!ReadBytes(serialized, offset, entry.registeredName.data(),
                           nameChars * sizeof(wchar_t))) {
                return false;
            }
        }

        entry.data.resize(static_cast<size_t>(dataSize));
        if (!ReadBytes(serialized, offset, entry.data.data(),
                       entry.data.size())) {
            return false;
        }

        entries->push_back(std::move(entry));
    }

    return !entries->empty();
}

CaptureResult CaptureCurrentClipboard(HWND ownerWindow) {
    if (!OpenClipboard(ownerWindow)) {
        return CaptureResult::Failed;
    }

    std::vector<ClipboardEntry> entries;
    UINT format = 0;
    DWORD formatCount = 0;
    bool hasImageEntry = false;

    while ((format = EnumClipboardFormats(format)) != 0) {
        formatCount++;

        std::wstring registeredName;
        if (!IsSupportedHGlobalFormat(format, &registeredName)) {
            continue;
        }

        ClipboardEntry entry;
        if (!CopyHGlobalClipboardFormat(format, registeredName, &entry)) {
            continue;
        }

        hasImageEntry = hasImageEntry || IsImageEntry(entry);
        entries.push_back(std::move(entry));

        if (entries.size() >= kMaxStoredFormats) {
            break;
        }
    }

    if (g_settings.persistImages && !hasImageEntry &&
        entries.size() < kMaxStoredFormats &&
        IsClipboardFormatAvailable(CF_BITMAP)) {
        ClipboardEntry bitmapEntry;
        if (CopyBitmapClipboardFallback(&bitmapEntry)) {
            entries.push_back(std::move(bitmapEntry));
        }
    }

    CloseClipboard();

    if (!entries.empty()) {
        return SaveEntries(entries) ? CaptureResult::Saved
                                    : CaptureResult::Failed;
    }

    return formatCount == 0 ? CaptureResult::Empty : CaptureResult::Unsupported;
}

bool ClipboardHasAnyFormat(HWND ownerWindow) {
    if (!OpenClipboard(ownerWindow)) {
        return true;
    }

    bool hasAnyFormat = EnumClipboardFormats(0) != 0;
    CloseClipboard();
    return hasAnyFormat;
}

bool RestoreClipboard(HWND ownerWindow) {
    std::vector<ClipboardEntry> entries;
    if (!LoadEntries(&entries)) {
        return false;
    }

    InterlockedIncrement(&g_restoring);

    bool restoredAny = false;
    if (OpenClipboard(ownerWindow)) {
        if (EmptyClipboard()) {
            for (const ClipboardEntry& entry : entries) {
                UINT format = entry.format;
                if (!entry.registeredName.empty()) {
                    format = RegisterClipboardFormatW(
                        entry.registeredName.c_str());
                }

                if (format == 0 || entry.data.empty()) {
                    continue;
                }

                HGLOBAL dataHandle =
                    GlobalAlloc(GMEM_MOVEABLE, entry.data.size());
                if (!dataHandle) {
                    continue;
                }

                void* lockedData = GlobalLock(dataHandle);
                if (!lockedData) {
                    GlobalFree(dataHandle);
                    continue;
                }

                CopyMemory(lockedData, entry.data.data(), entry.data.size());
                GlobalUnlock(dataHandle);

                if (SetClipboardData(format, dataHandle)) {
                    dataHandle = nullptr;
                    restoredAny = true;
                } else {
                    GlobalFree(dataHandle);
                }
            }
        }

        CloseClipboard();
    }

    InterlockedDecrement(&g_restoring);

    if (restoredAny) {
        Wh_Log(L"Restored clipboard");
    }

    return restoredAny;
}

LRESULT CALLBACK ClipboardWindowProc(HWND hwnd,
                                     UINT msg,
                                     WPARAM wParam,
                                     LPARAM lParam) {
    switch (msg) {
        case WM_CLIPBOARDUPDATE:
            if (InterlockedCompareExchange(&g_restoring, 0, 0) == 0) {
                SetTimer(hwnd, kTimerCapture, g_settings.captureDelayMs,
                         nullptr);
            }
            return 0;

        case WM_TIMER:
            if (wParam == kTimerCapture) {
                KillTimer(hwnd, kTimerCapture);
                CaptureResult result = CaptureCurrentClipboard(hwnd);
                if (result == CaptureResult::Empty &&
                    g_settings.restoreWhenCleared) {
                    SetTimer(hwnd, kTimerRestore, 250, nullptr);
                }

                return 0;
            }

            if (wParam == kTimerRestore) {
                KillTimer(hwnd, kTimerRestore);
                if (!ClipboardHasAnyFormat(hwnd)) {
                    RestoreClipboard(hwnd);
                }
                return 0;
            }

            break;

        case kMsgRestoreStartup:
            if (g_settings.restoreOnStartup && !ClipboardHasAnyFormat(hwnd)) {
                RestoreClipboard(hwnd);
            }

            return 0;

        case kMsgShutdown:
            KillTimer(hwnd, kTimerCapture);
            KillTimer(hwnd, kTimerRestore);
            RemoveClipboardFormatListener(hwnd);
            DestroyWindow(hwnd);
            return 0;

        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }

    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

DWORD WINAPI ClipboardThreadProc(LPVOID) {
    g_threadId = GetCurrentThreadId();

    HINSTANCE instance = GetCurrentModuleHandle();
    if (!instance) {
        Wh_Log(L"Failed to get current module handle");
        if (g_readyEvent) {
            SetEvent(g_readyEvent);
        }
        return 0;
    }

    WNDCLASSEXW windowClass = {};
    windowClass.cbSize = sizeof(windowClass);
    windowClass.lpfnWndProc = ClipboardWindowProc;
    windowClass.hInstance = instance;
    windowClass.lpszClassName = kWindowClassName;

    if (!RegisterClassExW(&windowClass) &&
        GetLastError() != ERROR_CLASS_ALREADY_EXISTS) {
        Wh_Log(L"RegisterClassExW failed: 0x%x", GetLastError());
        if (g_readyEvent) {
            SetEvent(g_readyEvent);
        }
        return 0;
    }

    HWND hwnd =
        CreateWindowExW(0, kWindowClassName, L"", 0, 0, 0, 0, 0,
                        HWND_MESSAGE, nullptr, instance, nullptr);
    if (!hwnd) {
        Wh_Log(L"CreateWindowExW failed: 0x%x", GetLastError());
        if (g_readyEvent) {
            SetEvent(g_readyEvent);
        }
        return 0;
    }

    g_window = hwnd;

    if (g_readyEvent) {
        SetEvent(g_readyEvent);
    }

    if (!AddClipboardFormatListener(hwnd)) {
        Wh_Log(L"AddClipboardFormatListener failed: 0x%x", GetLastError());
    }

    if (g_settings.restoreOnStartup) {
        PostMessageW(hwnd, kMsgRestoreStartup, 0, 0);
    }

    MSG message;
    while (GetMessageW(&message, nullptr, 0, 0) > 0) {
        TranslateMessage(&message);
        DispatchMessageW(&message);
    }

    g_window = nullptr;
    UnregisterClassW(kWindowClassName, instance);
    return 0;
}

}  // namespace

BOOL Wh_ModInit() {
    LoadSettings();

    g_readyEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_readyEvent) {
        Wh_Log(L"CreateEventW failed: 0x%x", GetLastError());
        return FALSE;
    }

    g_thread =
        CreateThread(nullptr, 0, ClipboardThreadProc, nullptr, 0, nullptr);
    if (!g_thread) {
        Wh_Log(L"CreateThread failed: 0x%x", GetLastError());
        CloseHandle(g_readyEvent);
        g_readyEvent = nullptr;
        return FALSE;
    }

    DWORD waitResult = WaitForSingleObject(g_readyEvent, 3000);
    CloseHandle(g_readyEvent);
    g_readyEvent = nullptr;

    if (waitResult == WAIT_OBJECT_0 && !g_window) {
        CloseHandle(g_thread);
        g_thread = nullptr;
        return FALSE;
    }

    return TRUE;
}

void Wh_ModUninit() {
    if (g_window) {
        PostMessageW(g_window, kMsgShutdown, 0, 0);
    } else if (g_threadId) {
        PostThreadMessageW(g_threadId, WM_QUIT, 0, 0);
    }

    if (g_thread) {
        WaitForSingleObject(g_thread, 5000);
        CloseHandle(g_thread);
        g_thread = nullptr;
    }
}

BOOL Wh_ModSettingsChanged(BOOL* reload) {
    *reload = TRUE;
    return TRUE;
}
