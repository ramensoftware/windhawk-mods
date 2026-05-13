// ==WindhawkMod==
// @id              clipboard-forever
// @name            Clipboard Forever
// @description     Persist clipboard text/image history and restore it after clipboard clears or Windows restarts
// @version         1.2.0
// @author          Guy
// @github          https://github.com/fggedr
// @license         MIT
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -luser32 -lgdi32 -lole32 -lwindowsapp -lwindowscodecs
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Persistent Clipboard Forever

Keeps supported clipboard items in Windhawk's mod storage and restores them
when the clipboard becomes empty, Explorer restarts, or Windows restarts.

Supported data:

- Plain text: Unicode, ANSI, OEM text, and locale metadata.
- Rich text helpers: HTML Format and Rich Text Format.
- Images: DIB, DIBV5, PNG clipboard format, and a fallback conversion from
  CF_BITMAP to DIB.

Notes:

- Windows clears its own unpinned clipboard history on restart. This mod keeps
  its own saved history and replays it into Windows after Explorer starts so
  `Win+V` can be rebuilt.
- When Windows reports that the clipboard became empty, the mod deletes its
  saved history by default so manually cleared/deleted items do not come back.
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
- BringBackClearedClipboard: false
  $name: Bring back cleared clipboard
  $description: Restore saved items when the clipboard becomes empty. Leave this off if you want deleted items to stay deleted.
- DeleteSavedHistoryWhenClipboardClears: true
  $name: Forget saved history when clipboard clears
  $description: Delete the mod's saved history when Windows reports an empty clipboard.
- RepopulateClipboardHistory: true
  $name: Rebuild Win+V clipboard history
  $description: Replay saved items after startup so Windows clipboard history can show them again.
- MaxHistoryItems: 25
  $name: Maximum saved history items
  $description: Windows clipboard history normally shows up to 25 items.
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
- ReplayDelayMs: 1500
  $name: History replay delay in milliseconds
  $description: Delay between replayed items while rebuilding Win+V history.
- MaxFormatMegabytes: 64
  $name: Maximum megabytes per format
  $description: Safety limit for each saved clipboard format.
*/
// ==/WindhawkModSettings==

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <wincodec.h>

#include <algorithm>
#include <limits>
#include <string>
#include <vector>
#include <winrt/Windows.ApplicationModel.DataTransfer.h>
#include <winrt/Windows.Foundation.h>

namespace {

constexpr wchar_t kWindowClassName[] =
    L"WindhawkPersistentClipboardForeverWindow";
constexpr wchar_t kClipboardFileName[] = L"clipboard.bin";
constexpr wchar_t kClipboardTempFileName[] = L"clipboard.bin.tmp";
constexpr wchar_t kCanIncludeInClipboardHistoryFormat[] =
    L"CanIncludeInClipboardHistory";

constexpr DWORD kFileMagicV1 = 0x31435057;  // WPC1
constexpr DWORD kFileMagicV2 = 0x32435057;  // WPC2
constexpr DWORD kFileVersionV1 = 1;
constexpr DWORD kFileVersionV2 = 2;
constexpr DWORD kMaxStoredFormats = 32;
constexpr DWORD kMaxFormatNameChars = 256;
constexpr DWORD kMaxHistoryItemsHardLimit = 100;
constexpr ULONGLONG kMaxFileBytes = 1024ull * 1024ull * 1024ull;

constexpr UINT kRegisteredFormatFirst = 0xC000;
constexpr UINT kMsgRestoreStartup = WM_APP + 1;
constexpr UINT kMsgShutdown = WM_APP + 2;
constexpr UINT kMsgClipboardHistoryChanged = WM_APP + 3;
constexpr UINT_PTR kTimerCapture = 1;
constexpr UINT_PTR kTimerRestore = 2;
constexpr UINT_PTR kTimerStartupRestore = 3;
constexpr UINT_PTR kTimerReplayHistory = 4;
constexpr UINT kStartupRestoreMaxAttempts = 60;
constexpr DWORD kStartupReplayDelayMs = 10000;

namespace ClipboardRt = winrt::Windows::ApplicationModel::DataTransfer;

struct Settings {
    bool restoreOnStartup = true;
    bool restoreWhenCleared = false;
    bool deleteSavedHistoryWhenClipboardClears = true;
    bool repopulateClipboardHistory = true;
    bool persistImages = true;
    bool persistRichText = true;
    bool persistPng = true;
    DWORD captureDelayMs = 150;
    DWORD replayDelayMs = 1500;
    DWORD maxHistoryItems = 25;
    SIZE_T maxFormatBytes = 64ull * 1024ull * 1024ull;
};

struct ClipboardEntry {
    UINT format = 0;
    std::wstring registeredName;
    std::vector<BYTE> data;
};

struct ClipboardItem {
    std::vector<ClipboardEntry> entries;
};

enum class CaptureResult {
    Saved,
    Empty,
    Unsupported,
    Failed,
};

enum class ClipboardState {
    Unavailable,
    Empty,
    HasData,
};

Settings g_settings;
HWND g_window = nullptr;
HANDLE g_thread = nullptr;
DWORD g_threadId = 0;
HANDLE g_readyEvent = nullptr;
UINT g_startupRestoreAttemptsRemaining = 0;
volatile LONG g_restoring = 0;
std::vector<ClipboardItem> g_replayItems;
size_t g_replayIndex = 0;
winrt::event_token g_clipboardHistoryChangedToken{};
bool g_winrtInitialized = false;
bool g_clipboardHistoryChangedRegistered = false;

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
        Wh_GetIntSetting(L"BringBackClearedClipboard") != 0;
    g_settings.deleteSavedHistoryWhenClipboardClears =
        Wh_GetIntSetting(L"DeleteSavedHistoryWhenClipboardClears") != 0;
    g_settings.repopulateClipboardHistory =
        Wh_GetIntSetting(L"RepopulateClipboardHistory") != 0;
    g_settings.persistImages = Wh_GetIntSetting(L"PersistImages") != 0;
    g_settings.persistRichText = Wh_GetIntSetting(L"PersistRichText") != 0;
    g_settings.persistPng = Wh_GetIntSetting(L"PersistPng") != 0;

    int captureDelayMs = Wh_GetIntSetting(L"CaptureDelayMs");
    if (captureDelayMs <= 0) {
        captureDelayMs = 150;
    }
    g_settings.captureDelayMs =
        static_cast<DWORD>(ClampInt(captureDelayMs, 10, 5000));

    int replayDelayMs = Wh_GetIntSetting(L"ReplayDelayMs");
    if (replayDelayMs <= 0) {
        replayDelayMs = 1500;
    }
    g_settings.replayDelayMs =
        static_cast<DWORD>(ClampInt(replayDelayMs, 1000, 10000));

    int maxHistoryItems = Wh_GetIntSetting(L"MaxHistoryItems");
    if (maxHistoryItems <= 0) {
        maxHistoryItems = 25;
    }
    g_settings.maxHistoryItems = static_cast<DWORD>(
        ClampInt(maxHistoryItems, 1, kMaxHistoryItemsHardLimit));

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

bool DeleteSavedHistory() {
    bool deletedAny = false;

    std::wstring path = GetStorageFilePath(kClipboardFileName);
    if (!path.empty() && DeleteFileW(path.c_str())) {
        deletedAny = true;
    }

    std::wstring tempPath = GetStorageFilePath(kClipboardTempFileName);
    if (!tempPath.empty() && DeleteFileW(tempPath.c_str())) {
        deletedAny = true;
    }

    DWORD error = GetLastError();
    if (deletedAny || error == ERROR_FILE_NOT_FOUND ||
        error == ERROR_PATH_NOT_FOUND) {
        return true;
    }

    Wh_Log(L"Failed to delete saved clipboard history: 0x%x", error);
    return false;
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

bool IsDibFormat(UINT format) {
    return format == CF_DIB || format == CF_DIBV5;
}

bool IsPngEntry(const ClipboardEntry& entry) {
    return !entry.registeredName.empty() &&
           lstrcmpiW(entry.registeredName.c_str(), L"PNG") == 0;
}

const ClipboardEntry* FindDibEntry(
    const std::vector<ClipboardEntry>& entries) {
    for (const ClipboardEntry& entry : entries) {
        if (IsDibFormat(entry.format)) {
            return &entry;
        }
    }

    return nullptr;
}

const ClipboardEntry* FindPngEntry(
    const std::vector<ClipboardEntry>& entries) {
    for (const ClipboardEntry& entry : entries) {
        if (IsPngEntry(entry)) {
            return &entry;
        }
    }

    return nullptr;
}

template <typename T>
void SafeRelease(T** object) {
    if (*object) {
        (*object)->Release();
        *object = nullptr;
    }
}

HBITMAP CreateBitmapFromDibEntry(const ClipboardEntry& entry) {
    if (!IsDibFormat(entry.format) ||
        entry.data.size() < sizeof(BITMAPINFOHEADER)) {
        return nullptr;
    }

    const auto* header =
        reinterpret_cast<const BITMAPINFOHEADER*>(entry.data.data());
    if (header->biSize < sizeof(BITMAPINFOHEADER) ||
        header->biSize > entry.data.size() || header->biPlanes != 1) {
        return nullptr;
    }

    size_t offset = header->biSize;

    if (header->biSize == sizeof(BITMAPINFOHEADER)) {
        if (header->biCompression == BI_BITFIELDS) {
            offset += 3 * sizeof(DWORD);
        } else if (header->biCompression == 6 /* BI_ALPHABITFIELDS */) {
            offset += 4 * sizeof(DWORD);
        }
    }

    if (header->biClrUsed > 0) {
        offset += static_cast<size_t>(header->biClrUsed) * sizeof(RGBQUAD);
    } else if (header->biBitCount <= 8) {
        offset += (static_cast<size_t>(1) << header->biBitCount) *
                  sizeof(RGBQUAD);
    }

    if (offset >= entry.data.size()) {
        return nullptr;
    }

    HDC screenDc = GetDC(nullptr);
    if (!screenDc) {
        return nullptr;
    }

    HBITMAP bitmap = CreateDIBitmap(
        screenDc, header, CBM_INIT, entry.data.data() + offset,
        reinterpret_cast<const BITMAPINFO*>(entry.data.data()), DIB_RGB_COLORS);
    ReleaseDC(nullptr, screenDc);

    return bitmap;
}

bool ConvertPngEntryToDibEntry(const ClipboardEntry& pngEntry,
                               ClipboardEntry* dibEntry) {
    if (!IsPngEntry(pngEntry) || pngEntry.data.empty() ||
        pngEntry.data.size() > static_cast<size_t>(MAXDWORD)) {
        return false;
    }

    IWICImagingFactory* factory = nullptr;
    IWICStream* stream = nullptr;
    IWICBitmapDecoder* decoder = nullptr;
    IWICBitmapFrameDecode* frame = nullptr;
    IWICFormatConverter* converter = nullptr;

    HRESULT hr = CoCreateInstance(CLSID_WICImagingFactory, nullptr,
                                  CLSCTX_INPROC_SERVER,
                                  IID_PPV_ARGS(&factory));
    if (FAILED(hr)) {
        return false;
    }

    hr = factory->CreateStream(&stream);
    if (SUCCEEDED(hr)) {
        hr = stream->InitializeFromMemory(
            const_cast<BYTE*>(pngEntry.data.data()),
            static_cast<DWORD>(pngEntry.data.size()));
    }

    if (SUCCEEDED(hr)) {
        hr = factory->CreateDecoderFromStream(
            stream, nullptr, WICDecodeMetadataCacheOnLoad, &decoder);
    }

    if (SUCCEEDED(hr)) {
        hr = decoder->GetFrame(0, &frame);
    }

    if (SUCCEEDED(hr)) {
        hr = factory->CreateFormatConverter(&converter);
    }

    if (SUCCEEDED(hr)) {
        hr = converter->Initialize(
            frame, GUID_WICPixelFormat32bppBGRA, WICBitmapDitherTypeNone,
            nullptr, 0.0, WICBitmapPaletteTypeCustom);
    }

    UINT width = 0;
    UINT height = 0;
    if (SUCCEEDED(hr)) {
        hr = converter->GetSize(&width, &height);
    }

    if (SUCCEEDED(hr) && width > 0 && height > 0 &&
        width <= static_cast<UINT>((std::numeric_limits<LONG>::max)()) &&
        height <= static_cast<UINT>((std::numeric_limits<LONG>::max)())) {
        ULONGLONG stride = static_cast<ULONGLONG>(width) * 4ull;
        ULONGLONG pixelBytes = stride * static_cast<ULONGLONG>(height);
        ULONGLONG totalBytes = sizeof(BITMAPINFOHEADER) + pixelBytes;

        if (totalBytes <= g_settings.maxFormatBytes &&
            totalBytes <= static_cast<ULONGLONG>(
                              (std::numeric_limits<size_t>::max)())) {
            ClipboardEntry converted;
            converted.format = CF_DIB;
            converted.data.resize(static_cast<size_t>(totalBytes));

            auto* header =
                reinterpret_cast<BITMAPINFOHEADER*>(converted.data.data());
            header->biSize = sizeof(BITMAPINFOHEADER);
            header->biWidth = static_cast<LONG>(width);
            header->biHeight = -static_cast<LONG>(height);
            header->biPlanes = 1;
            header->biBitCount = 32;
            header->biCompression = BI_RGB;
            header->biSizeImage = static_cast<DWORD>(pixelBytes);

            hr = converter->CopyPixels(
                nullptr, static_cast<UINT>(stride),
                static_cast<UINT>(pixelBytes),
                converted.data.data() + sizeof(BITMAPINFOHEADER));

            if (SUCCEEDED(hr)) {
                *dibEntry = std::move(converted);
            }
        } else {
            hr = HRESULT_FROM_WIN32(ERROR_FILE_TOO_LARGE);
        }
    }

    SafeRelease(&converter);
    SafeRelease(&frame);
    SafeRelease(&decoder);
    SafeRelease(&stream);
    SafeRelease(&factory);

    return SUCCEEDED(hr) && !dibEntry->data.empty();
}

bool ReadSerializedEntry(const std::vector<BYTE>& serialized,
                         size_t& offset,
                         ClipboardEntry* entry) {
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
            static_cast<ULONGLONG>((std::numeric_limits<size_t>::max)())) {
        return false;
    }

    entry->format = static_cast<UINT>(format);
    entry->registeredName.clear();
    entry->data.clear();

    if (nameChars > 0) {
        entry->registeredName.resize(nameChars);
        if (!ReadBytes(serialized, offset, entry->registeredName.data(),
                       nameChars * sizeof(wchar_t))) {
            return false;
        }
    }

    entry->data.resize(static_cast<size_t>(dataSize));
    return ReadBytes(serialized, offset, entry->data.data(),
                     entry->data.size());
}

void AppendSerializedEntry(std::vector<BYTE>& serialized,
                           const ClipboardEntry& entry) {
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

bool EntriesEqual(const std::vector<ClipboardEntry>& left,
                  const std::vector<ClipboardEntry>& right) {
    if (left.size() != right.size()) {
        return false;
    }

    for (size_t i = 0; i < left.size(); i++) {
        if (left[i].format != right[i].format ||
            left[i].registeredName != right[i].registeredName ||
            left[i].data != right[i].data) {
            return false;
        }
    }

    return true;
}

bool IsValidClipboardItem(const ClipboardItem& item) {
    return !item.entries.empty() && item.entries.size() <= kMaxStoredFormats;
}

bool SaveHistory(const std::vector<ClipboardItem>& history) {
    if (history.empty() ||
        history.size() > static_cast<size_t>(kMaxHistoryItemsHardLimit)) {
        return false;
    }

    std::vector<BYTE> serialized;
    AppendValue(serialized, kFileMagicV2);
    AppendValue(serialized, kFileVersionV2);
    AppendValue(serialized, static_cast<DWORD>(history.size()));

    for (const ClipboardItem& item : history) {
        if (!IsValidClipboardItem(item)) {
            return false;
        }

        AppendValue(serialized, static_cast<DWORD>(item.entries.size()));
        for (const ClipboardEntry& entry : item.entries) {
            AppendSerializedEntry(serialized, entry);
        }
    }

    return WriteWholeFile(GetStorageFilePath(kClipboardFileName), serialized);
}

bool LoadHistory(std::vector<ClipboardItem>* history) {
    history->clear();

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
        !ReadValue(serialized, offset, &count) || count == 0) {
        return false;
    }

    if (magic == kFileMagicV1 && version == kFileVersionV1) {
        if (count > kMaxStoredFormats) {
            return false;
        }

        ClipboardItem item;
        for (DWORD i = 0; i < count; i++) {
            ClipboardEntry entry;
            if (!ReadSerializedEntry(serialized, offset, &entry)) {
                return false;
            }
            item.entries.push_back(std::move(entry));
        }

        history->push_back(std::move(item));
        return true;
    }

    if (magic != kFileMagicV2 || version != kFileVersionV2 ||
        count > kMaxHistoryItemsHardLimit) {
        return false;
    }

    for (DWORD i = 0; i < count; i++) {
        DWORD entryCount = 0;
        if (!ReadValue(serialized, offset, &entryCount) || entryCount == 0 ||
            entryCount > kMaxStoredFormats) {
            return false;
        }

        ClipboardItem item;
        for (DWORD j = 0; j < entryCount; j++) {
            ClipboardEntry entry;
            if (!ReadSerializedEntry(serialized, offset, &entry)) {
                return false;
            }
            item.entries.push_back(std::move(entry));
        }

        history->push_back(std::move(item));
    }

    return !history->empty();
}

bool SaveEntries(const std::vector<ClipboardEntry>& entries) {
    if (entries.empty() || entries.size() > kMaxStoredFormats) {
        return false;
    }

    std::vector<ClipboardItem> history;
    LoadHistory(&history);

    history.erase(std::remove_if(history.begin(), history.end(),
                                 [&](const ClipboardItem& item) {
                                     return EntriesEqual(item.entries, entries);
                                 }),
                  history.end());

    ClipboardItem newItem;
    newItem.entries = entries;
    history.push_back(std::move(newItem));

    while (history.size() > g_settings.maxHistoryItems) {
        history.erase(history.begin());
    }

    if (!SaveHistory(history)) {
        return false;
    }

    Wh_Log(L"Saved clipboard item (%u/%u history item(s), %u format(s))",
           static_cast<unsigned int>(history.size()),
           static_cast<unsigned int>(g_settings.maxHistoryItems),
           static_cast<unsigned int>(entries.size()));
    return true;
}

bool LoadEntries(std::vector<ClipboardEntry>* entries) {
    entries->clear();

    std::vector<ClipboardItem> history;
    if (!LoadHistory(&history)) {
        return false;
    }

    for (auto it = history.rbegin(); it != history.rend(); ++it) {
        if (IsValidClipboardItem(*it)) {
            *entries = it->entries;
            return true;
        }
    }

    return false;
}

CaptureResult CaptureCurrentClipboard(HWND ownerWindow) {
    if (!OpenClipboard(ownerWindow)) {
        return CaptureResult::Failed;
    }

    std::vector<ClipboardEntry> entries;
    UINT format = 0;
    DWORD formatCount = 0;
    bool hasDibEntry = false;

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

        hasDibEntry = hasDibEntry || IsDibFormat(entry.format);
        entries.push_back(std::move(entry));

        if (entries.size() >= kMaxStoredFormats) {
            break;
        }
    }

    if (g_settings.persistImages && !hasDibEntry &&
        entries.size() < kMaxStoredFormats &&
        IsClipboardFormatAvailable(CF_BITMAP)) {
        ClipboardEntry bitmapEntry;
        if (CopyBitmapClipboardFallback(&bitmapEntry)) {
            entries.push_back(std::move(bitmapEntry));
            hasDibEntry = true;
        }
    }

    if (g_settings.persistImages && !hasDibEntry &&
        entries.size() < kMaxStoredFormats) {
        const ClipboardEntry* pngEntry = FindPngEntry(entries);
        ClipboardEntry convertedEntry;
        if (pngEntry && ConvertPngEntryToDibEntry(*pngEntry, &convertedEntry)) {
            entries.push_back(std::move(convertedEntry));
        }
    }

    CloseClipboard();

    if (!entries.empty()) {
        return SaveEntries(entries) ? CaptureResult::Saved
                                    : CaptureResult::Failed;
    }

    return formatCount == 0 ? CaptureResult::Empty : CaptureResult::Unsupported;
}

ClipboardState GetClipboardState(HWND ownerWindow) {
    if (!OpenClipboard(ownerWindow)) {
        return ClipboardState::Unavailable;
    }

    bool hasAnyFormat = EnumClipboardFormats(0) != 0;
    CloseClipboard();
    return hasAnyFormat ? ClipboardState::HasData : ClipboardState::Empty;
}

bool SetClipboardDwordFormat(PCWSTR formatName, DWORD value) {
    UINT format = RegisterClipboardFormatW(formatName);
    if (format == 0) {
        return false;
    }

    HGLOBAL dataHandle = GlobalAlloc(GMEM_MOVEABLE, sizeof(DWORD));
    if (!dataHandle) {
        return false;
    }

    void* lockedData = GlobalLock(dataHandle);
    if (!lockedData) {
        GlobalFree(dataHandle);
        return false;
    }

    CopyMemory(lockedData, &value, sizeof(value));
    GlobalUnlock(dataHandle);

    if (SetClipboardData(format, dataHandle)) {
        return true;
    }

    GlobalFree(dataHandle);
    return false;
}

bool PutEntriesOnClipboard(HWND ownerWindow,
                           const std::vector<ClipboardEntry>& entries) {
    bool restoredAny = false;
    std::vector<ClipboardEntry> entriesToSet = entries;

    if (g_settings.persistImages && !FindDibEntry(entriesToSet)) {
        const ClipboardEntry* pngEntry = FindPngEntry(entriesToSet);
        ClipboardEntry convertedEntry;
        if (pngEntry && ConvertPngEntryToDibEntry(*pngEntry, &convertedEntry)) {
            entriesToSet.push_back(std::move(convertedEntry));
        }
    }

    if (OpenClipboard(ownerWindow)) {
        if (EmptyClipboard()) {
            const ClipboardEntry* dibEntryForBitmap = FindDibEntry(entriesToSet);

            for (const ClipboardEntry& entry : entriesToSet) {
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

            if (dibEntryForBitmap) {
                HBITMAP bitmap = CreateBitmapFromDibEntry(*dibEntryForBitmap);
                if (bitmap) {
                    if (SetClipboardData(CF_BITMAP, bitmap)) {
                        bitmap = nullptr;
                        restoredAny = true;
                    } else {
                        DeleteObject(bitmap);
                    }
                }
            }

            if (restoredAny) {
                SetClipboardDwordFormat(kCanIncludeInClipboardHistoryFormat, 1);
            }
        }

        CloseClipboard();
    }

    return restoredAny;
}

bool RestoreClipboard(HWND ownerWindow) {
    std::vector<ClipboardEntry> entries;
    if (!LoadEntries(&entries)) {
        return false;
    }

    InterlockedIncrement(&g_restoring);
    bool restoredAny = PutEntriesOnClipboard(ownerWindow, entries);
    InterlockedDecrement(&g_restoring);

    if (restoredAny) {
        Wh_Log(L"Restored clipboard");
    }

    return restoredAny;
}

bool RegisterClipboardHistoryChanged(HWND hwnd) {
    if (!g_winrtInitialized || g_clipboardHistoryChangedRegistered) {
        return g_clipboardHistoryChangedRegistered;
    }

    try {
        if (!ClipboardRt::Clipboard::IsHistoryEnabled()) {
            Wh_Log(L"Windows clipboard history is disabled");
            return false;
        }

        g_clipboardHistoryChangedToken =
            ClipboardRt::Clipboard::HistoryChanged(
                [hwnd](auto&&, auto&&) {
                    PostMessageW(hwnd, kMsgClipboardHistoryChanged, 0, 0);
                });
        g_clipboardHistoryChangedRegistered = true;
        Wh_Log(L"Registered Clipboard.HistoryChanged listener");
        return true;
    } catch (...) {
        Wh_Log(L"Failed to register Clipboard.HistoryChanged: 0x%x",
               static_cast<unsigned int>(winrt::to_hresult()));
        return false;
    }
}

void UnregisterClipboardHistoryChanged() {
    if (!g_winrtInitialized || !g_clipboardHistoryChangedRegistered) {
        return;
    }

    try {
        ClipboardRt::Clipboard::HistoryChanged(
            g_clipboardHistoryChangedToken);
    } catch (...) {
        Wh_Log(L"Failed to unregister Clipboard.HistoryChanged: 0x%x",
               static_cast<unsigned int>(winrt::to_hresult()));
    }

    g_clipboardHistoryChangedRegistered = false;
}

void StopHistoryReplay(HWND hwnd) {
    KillTimer(hwnd, kTimerReplayHistory);

    if (!g_replayItems.empty()) {
        InterlockedDecrement(&g_restoring);
    }

    g_replayItems.clear();
    g_replayIndex = 0;
}

void ReplayNextHistoryItem(HWND hwnd) {
    if (g_replayIndex >= g_replayItems.size()) {
        Wh_Log(L"Rebuilt Win+V clipboard history with %u saved item(s)",
               static_cast<unsigned int>(g_replayItems.size()));
        StopHistoryReplay(hwnd);
        return;
    }

    size_t itemIndex = g_replayIndex + 1;
    bool restored =
        PutEntriesOnClipboard(hwnd, g_replayItems[g_replayIndex].entries);
    g_replayIndex++;

    Wh_Log(L"Replayed clipboard history item %u/%u%s",
           static_cast<unsigned int>(itemIndex),
           static_cast<unsigned int>(g_replayItems.size()),
           restored ? L"" : L" (clipboard set failed)");

    SetTimer(hwnd, kTimerReplayHistory, g_settings.replayDelayMs, nullptr);
}

bool StartHistoryReplay(HWND hwnd) {
    if (!g_settings.repopulateClipboardHistory) {
        return false;
    }

    StopHistoryReplay(hwnd);

    std::vector<ClipboardItem> history;
    if (!LoadHistory(&history)) {
        return false;
    }

    history.erase(std::remove_if(history.begin(), history.end(),
                                 [](const ClipboardItem& item) {
                                     return !IsValidClipboardItem(item);
                                 }),
                  history.end());

    if (history.empty()) {
        return false;
    }

    g_replayItems = std::move(history);
    g_replayIndex = 0;
    InterlockedIncrement(&g_restoring);
    RegisterClipboardHistoryChanged(hwnd);

    Wh_Log(L"Rebuilding Win+V clipboard history with %u saved item(s)",
           static_cast<unsigned int>(g_replayItems.size()));

    ReplayNextHistoryItem(hwnd);
    return true;
}

void TryStartupRestore(HWND hwnd, UINT attemptsRemaining) {
    if (!g_settings.restoreOnStartup) {
        g_startupRestoreAttemptsRemaining = 0;
        return;
    }

    if (StartHistoryReplay(hwnd)) {
        g_startupRestoreAttemptsRemaining = 0;
        return;
    }

    ClipboardState state = GetClipboardState(hwnd);
    if (state == ClipboardState::Empty) {
        g_startupRestoreAttemptsRemaining = 0;
        if (!RestoreClipboard(hwnd)) {
            Wh_Log(L"Clipboard is empty, but no saved clipboard item could be "
                   L"restored");
        }
        return;
    }

    if (state == ClipboardState::HasData) {
        g_startupRestoreAttemptsRemaining = 0;
        Wh_Log(L"Clipboard already has data; startup restore skipped");
        SetTimer(hwnd, kTimerCapture, g_settings.captureDelayMs, nullptr);
        return;
    }

    if (attemptsRemaining == 0) {
        g_startupRestoreAttemptsRemaining = 0;
        Wh_Log(L"Clipboard stayed unavailable during startup; restore gave up");
        return;
    }

    if (attemptsRemaining == kStartupRestoreMaxAttempts) {
        Wh_Log(L"Clipboard unavailable during startup; retrying restore");
    }

    g_startupRestoreAttemptsRemaining = attemptsRemaining - 1;
    SetTimer(hwnd, kTimerStartupRestore, 1000, nullptr);
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
                if (result == CaptureResult::Empty) {
                    StopHistoryReplay(hwnd);

                    if (g_startupRestoreAttemptsRemaining > 0) {
                        Wh_Log(L"Clipboard became empty during startup; saved "
                               L"history kept for replay");
                    } else if (g_settings.deleteSavedHistoryWhenClipboardClears) {
                        if (DeleteSavedHistory()) {
                            Wh_Log(L"Clipboard became empty; saved history "
                                   L"deleted");
                        }
                    } else if (g_settings.restoreWhenCleared) {
                        SetTimer(hwnd, kTimerRestore, 250, nullptr);
                    }
                }

                return 0;
            }

            if (wParam == kTimerRestore) {
                KillTimer(hwnd, kTimerRestore);
                if (GetClipboardState(hwnd) == ClipboardState::Empty) {
                    if (!StartHistoryReplay(hwnd)) {
                        RestoreClipboard(hwnd);
                    }
                }
                return 0;
            }

            if (wParam == kTimerStartupRestore) {
                KillTimer(hwnd, kTimerStartupRestore);
                TryStartupRestore(hwnd, g_startupRestoreAttemptsRemaining);
                return 0;
            }

            if (wParam == kTimerReplayHistory) {
                KillTimer(hwnd, kTimerReplayHistory);
                ReplayNextHistoryItem(hwnd);
                return 0;
            }

            break;

        case kMsgClipboardHistoryChanged:
            if (!g_replayItems.empty()) {
                KillTimer(hwnd, kTimerReplayHistory);
                ReplayNextHistoryItem(hwnd);
            }
            return 0;

        case kMsgRestoreStartup:
            TryStartupRestore(
                hwnd, wParam ? static_cast<UINT>(wParam)
                              : kStartupRestoreMaxAttempts);
            return 0;

        case kMsgShutdown:
            KillTimer(hwnd, kTimerCapture);
            KillTimer(hwnd, kTimerRestore);
            KillTimer(hwnd, kTimerStartupRestore);
            StopHistoryReplay(hwnd);
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

    try {
        winrt::init_apartment(winrt::apartment_type::single_threaded);
        g_winrtInitialized = true;
    } catch (...) {
        Wh_Log(L"Failed to initialize WinRT: 0x%x",
               static_cast<unsigned int>(winrt::to_hresult()));
    }

    HINSTANCE instance = GetCurrentModuleHandle();
    if (!instance) {
        Wh_Log(L"Failed to get current module handle");
        if (g_readyEvent) {
            SetEvent(g_readyEvent);
        }
        if (g_winrtInitialized) {
            winrt::uninit_apartment();
            g_winrtInitialized = false;
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
        if (g_winrtInitialized) {
            winrt::uninit_apartment();
            g_winrtInitialized = false;
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
        if (g_winrtInitialized) {
            winrt::uninit_apartment();
            g_winrtInitialized = false;
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
        g_startupRestoreAttemptsRemaining = kStartupRestoreMaxAttempts;
        SetTimer(hwnd, kTimerStartupRestore, kStartupReplayDelayMs, nullptr);
    }

    MSG message;
    while (GetMessageW(&message, nullptr, 0, 0) > 0) {
        TranslateMessage(&message);
        DispatchMessageW(&message);
    }

    UnregisterClipboardHistoryChanged();
    if (g_winrtInitialized) {
        winrt::uninit_apartment();
        g_winrtInitialized = false;
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

    Wh_Log(L"Clipboard Forever initialized");
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
