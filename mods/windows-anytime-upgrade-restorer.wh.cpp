// ==WindhawkMod==
// @id              windows-anytime-upgrade-restorer
// @name            Windows Anytime Upgrade Restorer
// @description     Restores the decorational Windows Anytime Upgrade Application. You can launch it from the Control Panel
// @version         1.0
// @author          RandomGuy5747
// @github          https://github.com/RandomGuy5747
// @include         windhawk.exe
// @include         WindowsAnytimeUpgradeui.exe
// @compilerOptions -lwinhttp -lbcrypt -ladvapi32 -lshell32 -luser32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Windows Anytime Upgrade Control Panel Restorer

Restores the decorational Windows Anytime Upgrade Application. You can launch it from "Control Panel\System and Security".
This mod downloads WindowsAnytimeUpgradeui.exe and dui70.dll from the Microsoft Symbol Server (msdl.microsoft.com) and uses these files for the application. Downloads happen on the first tool-process start after enabling the mod, and again whenever a cached file is missing or fails validation. The files are stored in Windhawk mod storage and are checked against pinned SHA-256 hashes before use.

Supported languages are English, Italian, Spanish, French, Portuguese, and German. The language is selected automatically based on the current Windows display language.

Not-supported languages will use English.

## WARNING

This application should only show "Windows Anytime Upgrade is not in this edition of Windows". But if it doesn't, and it shows upgrade options, then keep in mind that this application CANNOT upgrade Windows editions. It is only for decoration.

![preview1](https://raw.githubusercontent.com/RandomGuy5747/windhawk-preview/refs/heads/main/preview1.PNG)
*/
// ==/WindhawkModReadme==

#include <windows.h>
#include <winhttp.h>
#include <bcrypt.h>
#include <shellapi.h>
#include <shlobj.h>
#include <windhawk_utils.h>
#include <atomic>
#include <algorithm>
#include <array>
#include <map>
#include <optional>
#include <cstring>
#include <cwchar>
#include <string>
#include <thread>
#include <vector>

namespace {

constexpr wchar_t kExeName[] = L"WindowsAnytimeUpgradeui.exe";
constexpr wchar_t kDllName[] = L"dui71.dll";
// Official Microsoft Symbol Server (index = TimeDateStamp + SizeOfImage).
// dui71.dll is the Windows 7 RTM DUI70.dll (6.1.7600.16385) under another name.
constexpr const wchar_t* kExeUrls[] = {
    L"https://msdl.microsoft.com/download/symbols/WindowsAnytimeUpgradeui.exe/4A5BC99662000/WindowsAnytimeUpgradeui.exe",
};
constexpr const wchar_t* kDllUrls[] = {
    L"https://msdl.microsoft.com/download/symbols/dui70.dll/4A5BDF25f2000/dui70.dll",
};
constexpr BYTE kExeSha256[32] = {0x5f, 0x7b, 0x02, 0x2f, 0x81, 0x99, 0x9f, 0x80, 0x0e, 0x1f, 0xbc, 0xc6, 0x35, 0x5d, 0x47, 0xe2, 0xd3, 0xe8, 0xa9, 0xa5, 0x96, 0x1f, 0x99, 0x6f, 0x9a, 0xc5, 0xb8, 0x62, 0xa5, 0x73, 0x35, 0x18};
constexpr BYTE kDllSha256[32] = {0x98, 0xd2, 0x1e, 0xff, 0xf5, 0x11, 0xe4, 0x07, 0x33, 0x6a, 0x22, 0x64, 0x20, 0x70, 0x1e, 0x82, 0x55, 0x4d, 0xa0, 0x1f, 0xa0, 0x56, 0x61, 0x30, 0x38, 0x36, 0xb6, 0x86, 0x0d, 0x63, 0x74, 0x9d};

// Original Windows Anytime Upgrade Control Panel identity.
constexpr wchar_t kClsid[] = L"{BE122A0E-4503-11DA-8BDE-F66BAD1E3F3A}";
constexpr wchar_t kCanonicalName[] = L"Microsoft.WindowsAnytimeUpgrade";
constexpr wchar_t kDisplayName[] = L"Windows Anytime Upgrade";
constexpr wchar_t kMarkerName[] = L"Windhawk.WindowsAnytimeUpgradeRestorer";
constexpr wchar_t kMarkerValue[] = L"windows-anytime-upgrade-control-panel/1";

constexpr ULONGLONG kMaxDownloadBytes = 64ull * 1024ull * 1024ull;
constexpr DWORD kHttpTimeoutMs = 20000;
constexpr wchar_t kLocalizedDirectoryName[] = L"localized";
constexpr LANGID kResourceLanguage = MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US);

using Bytes = std::vector<BYTE>;

std::atomic<bool> g_stopping{false};
std::atomic<HINTERNET> g_activeRequest{nullptr};
[[clang::no_destroy]] std::optional<std::thread> g_worker;
std::wstring g_storageDir;
std::wstring g_localizedDir;
std::vector<std::wstring> g_createdRegistryKeys;
bool g_registrationInstalled = false;

struct InternetHandle {
    HINTERNET value = nullptr;
    InternetHandle() = default;
    explicit InternetHandle(HINTERNET handle) : value(handle) {}
    ~InternetHandle() {
        if (value) WinHttpCloseHandle(value);
    }
    InternetHandle(const InternetHandle&) = delete;
    InternetHandle& operator=(const InternetHandle&) = delete;
    InternetHandle(InternetHandle&& other) noexcept : value(other.value) {
        other.value = nullptr;
    }
    InternetHandle& operator=(InternetHandle&& other) noexcept {
        if (this != &other) {
            if (value) WinHttpCloseHandle(value);
            value = other.value;
            other.value = nullptr;
        }
        return *this;
    }
};

static bool IsAnytimeUpgradeUiProcess() {
    wchar_t path[MAX_PATH] = {};
    const DWORD length = GetModuleFileNameW(nullptr, path, ARRAYSIZE(path));
    if (!length || length >= ARRAYSIZE(path)) return false;
    const wchar_t* fileName = wcsrchr(path, L'\\');
    fileName = fileName ? fileName + 1 : path;
    return _wcsicmp(fileName, kExeName) == 0;
}

static std::wstring GetStorageDirectory() {
    wchar_t path[MAX_PATH] = {};
    const size_t length = Wh_GetModStoragePath(path, ARRAYSIZE(path));
    if (!length || length >= ARRAYSIZE(path)) return {};
    std::wstring result(path, length);
    while (!result.empty() && result.back() == L'\\') result.pop_back();
    return result;
}

static bool EnsureDirectory(const std::wstring& path) {
    if (CreateDirectoryW(path.c_str(), nullptr)) return true;
    return GetLastError() == ERROR_ALREADY_EXISTS;
}

static bool ReadWholeFile(const std::wstring& path, Bytes& data) {
    data.clear();
    HANDLE file = CreateFileW(path.c_str(), GENERIC_READ,
                              FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                              nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (file == INVALID_HANDLE_VALUE) return false;

    LARGE_INTEGER size{};
    bool ok = GetFileSizeEx(file, &size) && size.QuadPart >= 0 &&
              static_cast<ULONGLONG>(size.QuadPart) <= kMaxDownloadBytes;
    if (ok) {
        try {
            data.resize(static_cast<size_t>(size.QuadPart));
        } catch (...) {
            ok = false;
        }
    }

    size_t offset = 0;
    while (ok && offset < data.size()) {
        const DWORD amount = static_cast<DWORD>(
            (std::min)(data.size() - offset, static_cast<size_t>(1u << 20)));
        DWORD read = 0;
        if (!ReadFile(file, data.data() + offset, amount, &read, nullptr) || !read) {
            ok = false;
            break;
        }
        offset += read;
    }

    CloseHandle(file);
    if (!ok) data.clear();
    return ok;
}

struct PeInfo {
    WORD machine = 0;
    WORD characteristics = 0;
};

static bool ReadPeInfo(const Bytes& data, PeInfo& info) {
    info = {};
    if (data.size() < sizeof(IMAGE_DOS_HEADER)) return false;

    IMAGE_DOS_HEADER dos{};
    memcpy(&dos, data.data(), sizeof(dos));
    if (dos.e_magic != IMAGE_DOS_SIGNATURE || dos.e_lfanew < 0) return false;

    const size_t peOffset = static_cast<size_t>(dos.e_lfanew);
    if (peOffset > data.size() ||
        data.size() - peOffset < sizeof(DWORD) + sizeof(IMAGE_FILE_HEADER)) {
        return false;
    }

    DWORD signature = 0;
    IMAGE_FILE_HEADER fileHeader{};
    memcpy(&signature, data.data() + peOffset, sizeof(signature));
    memcpy(&fileHeader, data.data() + peOffset + sizeof(signature), sizeof(fileHeader));
    if (signature != IMAGE_NT_SIGNATURE) return false;

    info.machine = fileHeader.Machine;
    info.characteristics = fileHeader.Characteristics;
    return info.machine != IMAGE_FILE_MACHINE_UNKNOWN &&
           (info.characteristics & IMAGE_FILE_EXECUTABLE_IMAGE) != 0;
}

static bool IsExpectedPe(const Bytes& data, bool expectDll, WORD* machineOut = nullptr) {
    PeInfo info{};
    if (!ReadPeInfo(data, info)) return false;

    const bool isDll = (info.characteristics & IMAGE_FILE_DLL) != 0;
    if (isDll != expectDll) return false;
    if (machineOut) *machineOut = info.machine;
    return true;
}

static bool WriteFileAtomically(const std::wstring& path, const Bytes& data) {
    const std::wstring temporary = path + L"." + std::to_wstring(GetCurrentProcessId()) + L".tmp";
    HANDLE file = CreateFileW(temporary.c_str(), GENERIC_WRITE, 0, nullptr,
                              CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (file == INVALID_HANDLE_VALUE) return false;

    bool ok = true;
    size_t offset = 0;
    while (ok && offset < data.size()) {
        const DWORD amount = static_cast<DWORD>(
            (std::min)(data.size() - offset, static_cast<size_t>(1u << 20)));
        DWORD written = 0;
        ok = WriteFile(file, data.data() + offset, amount, &written, nullptr) && written == amount;
        offset += written;
    }
    if (ok) ok = FlushFileBuffers(file) != FALSE;
    CloseHandle(file);

    if (ok) {
        ok = MoveFileExW(temporary.c_str(), path.c_str(),
                         MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH) != FALSE;
    }
    if (!ok) DeleteFileW(temporary.c_str());
    return ok;
}

static void CancelActiveRequest() {
    if (HINTERNET request = g_activeRequest.exchange(nullptr)) {
        WinHttpCloseHandle(request);
    }
}

static bool DownloadUrl(const wchar_t* url, Bytes& output) {
    output.clear();
    if (!url || g_stopping.load()) return false;

    URL_COMPONENTS parts{};
    parts.dwStructSize = sizeof(parts);
    parts.dwSchemeLength = static_cast<DWORD>(-1);
    parts.dwHostNameLength = static_cast<DWORD>(-1);
    parts.dwUrlPathLength = static_cast<DWORD>(-1);
    parts.dwExtraInfoLength = static_cast<DWORD>(-1);
    if (!WinHttpCrackUrl(url, 0, 0, &parts)) {
        Wh_Log(L"Anytime Upgrade: URL parsing failed (error=%lu)", GetLastError());
        return false;
    }

    const std::wstring host(parts.lpszHostName, parts.dwHostNameLength);
    std::wstring requestPath(parts.lpszUrlPath, parts.dwUrlPathLength);
    if (parts.dwExtraInfoLength) {
        requestPath.append(parts.lpszExtraInfo, parts.dwExtraInfoLength);
    }

    InternetHandle session(WinHttpOpen(L"Windhawk Windows Anytime Upgrade Restorer/1.0",
                                       WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
                                       WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0));
    if (!session.value) {
        Wh_Log(L"Anytime Upgrade: WinHttpOpen failed (error=%lu)", GetLastError());
        return false;
    }
    WinHttpSetTimeouts(session.value, kHttpTimeoutMs, kHttpTimeoutMs,
                       kHttpTimeoutMs, kHttpTimeoutMs);

    InternetHandle connection(WinHttpConnect(session.value, host.c_str(), parts.nPort, 0));
    if (!connection.value) {
        Wh_Log(L"Anytime Upgrade: WinHttpConnect failed (error=%lu)", GetLastError());
        return false;
    }

    const DWORD requestFlags = parts.nScheme == INTERNET_SCHEME_HTTPS ? WINHTTP_FLAG_SECURE : 0;
    HINTERNET rawRequest = WinHttpOpenRequest(connection.value, L"GET", requestPath.c_str(),
                                              nullptr, WINHTTP_NO_REFERER,
                                              WINHTTP_DEFAULT_ACCEPT_TYPES, requestFlags);
    if (!rawRequest) {
        Wh_Log(L"Anytime Upgrade: WinHttpOpenRequest failed (error=%lu)", GetLastError());
        return false;
    }

    g_activeRequest.store(rawRequest);
    bool requestStillOwned = true;
    auto closeRequest = [&]() {
        if (!requestStillOwned) return;
        HINTERNET expected = rawRequest;
        if (g_activeRequest.compare_exchange_strong(expected, nullptr)) {
            WinHttpCloseHandle(rawRequest);
        }
        requestStillOwned = false;
    };

    bool ok = false;
    DWORD statusCode = 0;
    DWORD statusSize = sizeof(statusCode);
    if (!g_stopping.load() &&
        WinHttpSendRequest(rawRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0,
                           WINHTTP_NO_REQUEST_DATA, 0, 0, 0) &&
        WinHttpReceiveResponse(rawRequest, nullptr) &&
        WinHttpQueryHeaders(rawRequest,
                            WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER,
                            WINHTTP_HEADER_NAME_BY_INDEX, &statusCode,
                            &statusSize, WINHTTP_NO_HEADER_INDEX) &&
        statusCode == 200) {
        try {
            ok = true;
            for (;;) {
                if (g_stopping.load()) {
                    ok = false;
                    break;
                }
                BYTE buffer[16384];
                DWORD received = 0;
                if (!WinHttpReadData(rawRequest, buffer, sizeof(buffer), &received)) {
                    ok = false;
                    break;
                }
                if (!received) break;
                if (output.size() + received > kMaxDownloadBytes) {
                    ok = false;
                    break;
                }
                output.insert(output.end(), buffer, buffer + received);
            }
        } catch (...) {
            ok = false;
        }
    } else if (statusCode) {
        Wh_Log(L"Anytime Upgrade: download returned HTTP status %lu", statusCode);
    } else if (!g_stopping.load()) {
        Wh_Log(L"Anytime Upgrade: HTTP request failed (error=%lu)", GetLastError());
    }

    closeRequest();
    if (!ok) output.clear();
    return ok;
}

static bool Sha256Matches(const Bytes& data, const BYTE* expected) {
    BCRYPT_ALG_HANDLE alg = nullptr;
    if (BCryptOpenAlgorithmProvider(&alg, BCRYPT_SHA256_ALGORITHM, nullptr, 0) < 0) return false;
    BYTE hash[32] = {};
    const NTSTATUS status = BCryptHash(alg, nullptr, 0,
        const_cast<PUCHAR>(data.data()), static_cast<ULONG>(data.size()), hash, sizeof(hash));
    BCryptCloseAlgorithmProvider(alg, 0);
    return status >= 0 && memcmp(hash, expected, sizeof(hash)) == 0;
}

// Microsoft's original WindowsAnytimeUpgradeui.exe imports "DUI70.dll". The
// pinned hash is for the variant that imports "DUI71.dll" (the private name
// this mod gives to the DirectUI copy). Patch only the import-table name, then
// the SHA-256 check decides whether the result is exactly the expected file.
static bool RvaToOffset(const BYTE* base, size_t size, const IMAGE_NT_HEADERS64* nt,
                        DWORD rva, size_t& offset) {
    const IMAGE_SECTION_HEADER* section = IMAGE_FIRST_SECTION(nt);
    for (WORD i = 0; i < nt->FileHeader.NumberOfSections; ++i, ++section) {
        const DWORD span = (std::max)(section->Misc.VirtualSize, section->SizeOfRawData);
        if (rva >= section->VirtualAddress && rva - section->VirtualAddress < span) {
            const size_t candidate = static_cast<size_t>(section->PointerToRawData) +
                                     (rva - section->VirtualAddress);
            if (candidate >= size) return false;
            offset = candidate;
            return true;
        }
    }
    return false;
}

static bool PatchDirectUiImport(Bytes& data) {
    if (data.size() < sizeof(IMAGE_DOS_HEADER)) return false;
    IMAGE_DOS_HEADER dos{};
    memcpy(&dos, data.data(), sizeof(dos));
    if (dos.e_magic != IMAGE_DOS_SIGNATURE || dos.e_lfanew <= 0) return false;
    const size_t ntOffset = static_cast<size_t>(dos.e_lfanew);
    if (ntOffset > data.size() || data.size() - ntOffset < sizeof(IMAGE_NT_HEADERS64)) return false;
    IMAGE_NT_HEADERS64 nt{};
    memcpy(&nt, data.data() + ntOffset, sizeof(nt));
    if (nt.Signature != IMAGE_NT_SIGNATURE ||
        nt.OptionalHeader.Magic != IMAGE_NT_OPTIONAL_HDR64_MAGIC) return false;

    // Section headers follow the optional header; re-read them from the buffer.
    const size_t sectionsOffset = ntOffset + offsetof(IMAGE_NT_HEADERS64, OptionalHeader) +
                                  nt.FileHeader.SizeOfOptionalHeader;
    const size_t sectionsBytes = sizeof(IMAGE_SECTION_HEADER) * nt.FileHeader.NumberOfSections;
    if (sectionsOffset > data.size() || data.size() - sectionsOffset < sectionsBytes) return false;
    Bytes headers(data.begin() + ntOffset, data.begin() + sectionsOffset + sectionsBytes);
    const IMAGE_NT_HEADERS64* ntView = reinterpret_cast<const IMAGE_NT_HEADERS64*>(headers.data());

    const IMAGE_DATA_DIRECTORY dir = nt.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT];
    size_t descriptorOffset = 0;
    if (!dir.VirtualAddress ||
        !RvaToOffset(data.data(), data.size(), ntView, dir.VirtualAddress, descriptorOffset)) return false;

    bool patched = false;
    for (; descriptorOffset + sizeof(IMAGE_IMPORT_DESCRIPTOR) <= data.size();
         descriptorOffset += sizeof(IMAGE_IMPORT_DESCRIPTOR)) {
        IMAGE_IMPORT_DESCRIPTOR descriptor{};
        memcpy(&descriptor, data.data() + descriptorOffset, sizeof(descriptor));
        if (!descriptor.Name) break;
        size_t nameOffset = 0;
        if (!RvaToOffset(data.data(), data.size(), ntView, descriptor.Name, nameOffset) ||
            data.size() - nameOffset < 10) continue;
        char* name = reinterpret_cast<char*>(data.data() + nameOffset);
        if (_strnicmp(name, "dui70.dll", 9) == 0 && name[9] == '\0') {
            name[4] = '1';
            patched = true;
        }
    }
    return patched;
}

static bool LoadOrDownload(const wchar_t* fileName, const wchar_t* const* urls, size_t urlCount,
                           const BYTE* expectedHash, bool expectDll, Bytes& output, WORD& machine,
                           bool patchDuiImport = false) {
    const std::wstring path = g_storageDir + L"\\" + fileName;
    if (ReadWholeFile(path, output) && IsExpectedPe(output, expectDll, &machine) &&
        Sha256Matches(output, expectedHash)) {
        Wh_Log(L"Anytime Upgrade: reusing cached %s", fileName);
        return true;
    }
    output.clear();

    for (size_t u = 0; u < urlCount && !g_stopping.load(); ++u) {
        for (int attempt = 1; attempt <= 2 && !g_stopping.load(); ++attempt) {
            Wh_Log(L"Anytime Upgrade: downloading %s from %s (attempt %d/2)", fileName, urls[u], attempt);
            Bytes downloaded;
            const bool valid = DownloadUrl(urls[u], downloaded) &&
                               IsExpectedPe(downloaded, expectDll, &machine);
            if (valid && patchDuiImport && PatchDirectUiImport(downloaded)) {
                Wh_Log(L"Anytime Upgrade: %s import name DUI70 -> DUI71", fileName);
            }
            if (valid && Sha256Matches(downloaded, expectedHash)) {
                if (g_stopping.load()) return false;
                if (WriteFileAtomically(path, downloaded)) {
                    output.swap(downloaded);
                    Wh_Log(L"Anytime Upgrade: %s downloaded and verified", fileName);
                    return true;
                }
                Wh_Log(L"Anytime Upgrade: could not write %s (error=%lu)", fileName, GetLastError());
            } else if (!g_stopping.load()) {
                Wh_Log(L"Anytime Upgrade: %s failed download, PE or SHA-256 validation", fileName);
            }
            for (int tick = 0; tick < 10 && !g_stopping.load(); ++tick) Sleep(100);
        }
    }
    return false;
}

// The original Anytime Upgrade UI expects its language-neutral/English MUI
// string resources to be present. On newer Windows installations the matching
// satellite resources are not necessarily available, which can leave the page
// as a blank white surface with an error icon. These are the English strings
// supplied with this mod, written as genuine RT_STRING resources into private
// copies of BOTH modules. The downloaded originals are never modified.
struct AnytimeUpgradeString {
    WORD id;
    const wchar_t* text;
};

static constexpr AnytimeUpgradeString kAnytimeUpgradeStrings[] = {
    {1, L"Windows Anytime Upgrade"},
    {2, L"A convenient and affordable way to upgrade Windows"},
    {3, L"Get more features with a new edition of Windows 7"},
    {4, L"Unknown"},
    {50, L"Enter Windows Anytime Upgrade key"},
    {51, L"Back"},
    {52, L"Security"},
    {53, L"Download progress"},
    {54, L"Upgrade progress"},
    {55, L"Upgrade Key verification progress"},
    {56, L"Internet"},
    {57, L"Error"},
    {58, L"Warning"},
    {59, L"Information"},
    {101, L"Clicking \"I accept\" means that you agree to the <a>Microsoft Software License Terms.</a>"},
    {102, L"Connect to the Internet and open Windows Anytime Upgrade to compare editions of Windows 7.  You can then buy the upgrade online and install it on your computer.  For more information, see:  <a>What do I need to connect to the Internet?</a>"},
    {103, L"Go online to see if your computer is ready to upgrade to another edition of Windows 7"},
    {104, L"The upgrade has been canceled\n\nTo continue the upgrade later, open Windows Anytime Upgrade and resume the upgrade process."},
    {105, L"To upgrade, you need to accept the license terms\n\nThe upgrade has been canceled. To continue the upgrade, open Windows Anytime Upgrade again and accept the license terms."},
    {106, L"There is security encryption for this web page. Click here to view the digital certificate information."},
    {107, L"After your purchase, Windows will upgrade automatically."},
    {108, L"If you already have a Windows Anytime Upgrade key, begin the process here."},
    {109, L"Check your Windows Anytime Upgrade key and make sure to type all 25 characters correctly."},
    {151, L"The upgrade key is not valid. Please check the key and try again."},
    {152, L"The upgrade key you entered is for an edition of Windows that does not work with Windows Anytime Upgrade.  Enter a different upgrade key."},
    {153, L"The upgrade was not successful. Please try the upgrade again."},
    {154, L"To complete the upgrade, allow Windows Anytime Upgrade to run.  Try again and click \"Yes\" when prompted by User Account Control."},
    {155, L"Before you can upgrade, you need to activate the edition of Windows your computer is currently running."},
    {156, L"Windows Anytime Upgrade is not in this edition of Windows"},
    {157, L"Contact your system administrator for more information."},
    {158, L"Windows Anytime Upgrade is already running.  Close Windows Anytime Upgrade on one of the user accounts."},
    {159, L"Turn on System Restore for the disk that contains your Windows installation and try again."},
    {160, L"Windows Anytime Upgrade can't open the webpage"},
    {161, L"Close Windows Anytime Upgrade, and open it again."},
    {162, L"Windows Anytime Upgrade can't download your receipt"},
    {163, L"Check your e-mail for your receipt and confirmation."},
    {164, L"Your upgrade key was downloaded successfully."},
    {165, L"Windows Anytime Upgrade was not successful"},
    {166, L"Close Windows Anytime Upgrade and try to upgrade again."},
    {167, L"Close Windows Anytime Upgrade, and open it again to retry validation."},
    {168, L"Activate Windows"},
    {169, L"Activate Windows now"},
    {170, L"Windows Anytime Upgrade couldn't create a restore point"},
    {171, L"How do I turn on System Restore?"},
    {174, L"Checking for updates"},
    {175, L"Downloading 1 update"},
    {176, L"Downloading %d updates"},
    {177, L"Installing 1 update"},
    {178, L"Installing %d updates"},
    {179, L"Enter a different upgrade key"},
    {180, L"Allow Windows Anytime Upgrade to run"},
    {181, L"Windows Anytime Upgrade has been disabled"},
    {182, L"Windows Anytime Upgrade is already running"},
    {183, L"The upgrade key couldn't be validated"},
    {184, L"Go online to try and resolve this issue"},
    {1100, L"Your computer is currently running"},
    {1101, L"Windows Anytime Upgrade"},
    {1102, L"Do even more with your PC"},
    {1103, L"How do you want to begin?"},
    {1104, L"Go online to choose the edition of Windows 7 that's best for you"},
    {1106, L"Enter an upgrade key"},
    {1108, L"How can I upgrade when I am not connected to the Internet?"},
    {1109, L"Cancel"},
    {1110, L"Enter your upgrade key"},
    {1111, L"To begin the upgrade process, enter the 25-character Windows Anytime Upgrade key for your new edition of Windows 7."},
    {1112, L"Make sure to enter your Windows Anytime Upgrade key and not the product key that came with your computer or copy of Windows."},
    {1113, L"Windows Anytime Upgrade key:"},
    {1114, L"(dashes will be added automatically)"},
    {1115, L"Next"},
    {1116, L"You are not connected to the Internet"},
    {1117, L"You can begin Windows Anytime Upgrade in one of two ways:"},
    {1118, L"Go to a retail store and buy a Windows Anytime Upgrade key.  When you have it, open Windows Anytime Upgrade on your computer and upgrade to a new edition of Windows 7."},
    {1119, L"Verifying key"},
    {1120, L"Key verification will take less than one minute."},
    {1121, L"Please accept the license terms"},
    {1122, L"I accept"},
    {1123, L"Upgrade Windows"},
    {1124, L"Before upgrading, save your work and close all open programs."},
    {1125, L"The upgrade will take as few as 10 minutes.  It might take longer depending on your particular PC and whether online updates are needed.  You cannot use your computer during the upgrade."},
    {1126, L"Your computer will restart automatically.  For some upgrades, your computer might restart several times."},
    {1127, L"Upgrade"},
    {1128, L"Upgrading Windows"},
    {1129, L"The upgrade will take as few as 10 minutes.  It might take longer depending on your particular PC and whether online updates are needed."},
    {1130, L"Your computer will restart automatically."},
    {1131, L"Starting upgrade"},
    {1132, L"Close"},
    {1133, L"The upgrade key looks similar to this:"},
    {1134, L"UPGRADE KEY: XXXXX-XXXXX-XXXXX-XXXXX-XXXXX"},
    {1135, L"In as few as 10 minutes, you can add new features to Windows 7 and do more with your PC.  It's easy, quick, and you'll keep your programs, files, and settings.  The upgrade might take longer depending on your particular PC and whether online updates are needed."},
    {1138, L"OK"},
};

static constexpr AnytimeUpgradeString kAnytimeUpgradeStringsIt[] = {
    {1, L"Windows Anytime Upgrade"},
    {2, L"Un modo comodo e conveniente per aggiornare Windows"},
    {3, L"Ottenere pi\u00f9 funzionalit\u00e0 con una nuova edizione di Windows 7"},
    {4, L"Sconosciuto"},
    {50, L"Immettere il codice di Windows Anytime Upgrade"},
    {51, L"Indietro"},
    {52, L"Sicurezza"},
    {53, L"Stato del download"},
    {54, L"Stato dell'aggiornamento"},
    {55, L"Stato della verifica del codice di aggiornamento"},
    {56, L"Internet"},
    {57, L"Errore"},
    {58, L"Avviso"},
    {59, L"Informazioni"},
    {101, L"Facendo clic su \"Accetto\" si accettano i <a>Termini di licenza del software Microsoft.</a>"},
    {102, L"Connettersi a Internet e aprire Windows Anytime Upgrade per confrontare le edizioni di Windows 7.  \u00c8 quindi possibile acquistare l'aggiornamento online e installarlo nel computer.  Per ulteriori informazioni, vedere:  <a>Cosa \u00e8 necessario per connettersi a Internet?</a>"},
    {103, L"Connettersi online per verificare se il computer \u00e8 pronto per l'aggiornamento a un'altra edizione di Windows 7"},
    {104, L"L'aggiornamento \u00e8 stato annullato\n\nPer continuare l'aggiornamento in un secondo momento, aprire Windows Anytime Upgrade e riprendere la procedura di aggiornamento."},
    {105, L"Per eseguire l'aggiornamento \u00e8 necessario accettare i termini di licenza\n\nL'aggiornamento \u00e8 stato annullato. Per continuare, aprire di nuovo Windows Anytime Upgrade e accettare i termini di licenza."},
    {106, L"Questa pagina Web \u00e8 protetta da crittografia. Fare clic qui per visualizzare le informazioni sul certificato digitale."},
    {107, L"Dopo l'acquisto, Windows verr\u00e0 aggiornato automaticamente."},
    {108, L"Se si dispone gi\u00e0 di un codice di Windows Anytime Upgrade, iniziare la procedura da qui."},
    {109, L"Verificare il codice di Windows Anytime Upgrade e assicurarsi di digitare correttamente tutti i 25 caratteri."},
    {151, L"Il codice di aggiornamento non \u00e8 valido. Controllare il codice e riprovare."},
    {152, L"Il codice di aggiornamento immesso \u00e8 relativo a un'edizione di Windows non compatibile con Windows Anytime Upgrade.  Immettere un codice di aggiornamento diverso."},
    {153, L"Aggiornamento non riuscito. Provare di nuovo a eseguire l'aggiornamento."},
    {154, L"Per completare l'aggiornamento, consentire l'esecuzione di Windows Anytime Upgrade.  Riprovare e fare clic su \"S\u00ec\" quando richiesto dal Controllo account utente."},
    {155, L"Prima di eseguire l'aggiornamento, \u00e8 necessario attivare l'edizione di Windows attualmente in esecuzione nel computer."},
    {156, L"Windows Anytime Upgrade non \u00e8 incluso in questa edizione di Windows"},
    {157, L"Per ulteriori informazioni, contattare l'amministratore di sistema."},
    {158, L"Windows Anytime Upgrade \u00e8 gi\u00e0 in esecuzione.  Chiudere Windows Anytime Upgrade in uno degli account utente."},
    {159, L"Attivare Ripristino configurazione di sistema per il disco che contiene l'installazione di Windows e riprovare."},
    {160, L"Impossibile aprire la pagina Web da Windows Anytime Upgrade"},
    {161, L"Chiudere Windows Anytime Upgrade e aprirlo di nuovo."},
    {162, L"Impossibile scaricare la ricevuta da Windows Anytime Upgrade"},
    {163, L"Controllare la posta elettronica per la ricevuta e la conferma."},
    {164, L"Il codice di aggiornamento \u00e8 stato scaricato correttamente."},
    {165, L"Windows Anytime Upgrade non \u00e8 riuscito"},
    {166, L"Chiudere Windows Anytime Upgrade e provare di nuovo a eseguire l'aggiornamento."},
    {167, L"Chiudere Windows Anytime Upgrade e aprirlo di nuovo per riprovare la convalida."},
    {168, L"Attiva Windows"},
    {169, L"Attiva Windows ora"},
    {170, L"Impossibile creare un punto di ripristino da Windows Anytime Upgrade"},
    {171, L"Come si attiva Ripristino configurazione di sistema?"},
    {174, L"Ricerca degli aggiornamenti in corso"},
    {175, L"Download di 1 aggiornamento in corso"},
    {176, L"Download di %d aggiornamenti in corso"},
    {177, L"Installazione di 1 aggiornamento in corso"},
    {178, L"Installazione di %d aggiornamenti in corso"},
    {179, L"Immettere un codice di aggiornamento diverso"},
    {180, L"Consenti l'esecuzione di Windows Anytime Upgrade"},
    {181, L"Windows Anytime Upgrade \u00e8 stato disattivato"},
    {182, L"Windows Anytime Upgrade \u00e8 gi\u00e0 in esecuzione"},
    {183, L"Impossibile convalidare il codice di aggiornamento"},
    {184, L"Connettersi online per tentare di risolvere il problema"},
    {1100, L"Il computer sta attualmente eseguendo"},
    {1101, L"Windows Anytime Upgrade"},
    {1102, L"Ottenere di pi\u00f9 dal PC"},
    {1103, L"Come si desidera iniziare?"},
    {1104, L"Connettersi online per scegliere l'edizione di Windows 7 pi\u00f9 adatta alle proprie esigenze"},
    {1106, L"Immettere un codice di aggiornamento"},
    {1108, L"Come \u00e8 possibile eseguire l'aggiornamento senza una connessione a Internet?"},
    {1109, L"Annulla"},
    {1110, L"Immettere il codice di aggiornamento"},
    {1111, L"Per iniziare la procedura di aggiornamento, immettere il codice di Windows Anytime Upgrade di 25 caratteri per la nuova edizione di Windows 7."},
    {1112, L"Assicurarsi di immettere il codice di Windows Anytime Upgrade e non il product key fornito con il computer o con la copia di Windows."},
    {1113, L"Codice di Windows Anytime Upgrade:"},
    {1114, L"(i trattini verranno aggiunti automaticamente)"},
    {1115, L"Avanti"},
    {1116, L"Non si \u00e8 connessi a Internet"},
    {1117, L"\u00c8 possibile iniziare Windows Anytime Upgrade in due modi:"},
    {1118, L"Recarsi in un negozio e acquistare un codice di Windows Anytime Upgrade.  Una volta ottenuto, aprire Windows Anytime Upgrade nel computer ed eseguire l'aggiornamento a una nuova edizione di Windows 7."},
    {1119, L"Verifica del codice in corso"},
    {1120, L"La verifica del codice richieder\u00e0 meno di un minuto."},
    {1121, L"Accettare i termini di licenza"},
    {1122, L"Accetto"},
    {1123, L"Aggiorna Windows"},
    {1124, L"Prima di eseguire l'aggiornamento, salvare il lavoro e chiudere tutti i programmi aperti."},
    {1125, L"L'aggiornamento richieder\u00e0 anche solo 10 minuti.  Potrebbe richiedere pi\u00f9 tempo a seconda del PC e della necessit\u00e0 di aggiornamenti online.  Non \u00e8 possibile usare il computer durante l'aggiornamento."},
    {1126, L"Il computer verr\u00e0 riavviato automaticamente.  Per alcuni aggiornamenti, potrebbe essere riavviato pi\u00f9 volte."},
    {1127, L"Aggiorna"},
    {1128, L"Aggiornamento di Windows in corso"},
    {1129, L"L'aggiornamento richieder\u00e0 anche solo 10 minuti.  Potrebbe richiedere pi\u00f9 tempo a seconda del PC e della necessit\u00e0 di aggiornamenti online."},
    {1130, L"Il computer verr\u00e0 riavviato automaticamente."},
    {1131, L"Avvio dell'aggiornamento in corso"},
    {1132, L"Chiudi"},
    {1133, L"Il codice di aggiornamento ha un aspetto simile al seguente:"},
    {1134, L"CODICE DI AGGIORNAMENTO: XXXXX-XXXXX-XXXXX-XXXXX-XXXXX"},
    {1135, L"In soli 10 minuti \u00e8 possibile aggiungere nuove funzionalit\u00e0 a Windows 7 e ottenere di pi\u00f9 dal PC.  \u00c8 semplice e rapido e verranno mantenuti programmi, file e impostazioni.  L'aggiornamento potrebbe richiedere pi\u00f9 tempo a seconda del PC e della necessit\u00e0 di aggiornamenti online."},
    {1138, L"OK"},
};

static constexpr AnytimeUpgradeString kAnytimeUpgradeStringsEs[] = {
    {1, L"Windows Anytime Upgrade"},
    {2, L"Una forma c\u00f3moda y asequible de actualizar Windows"},
    {3, L"Obtenga m\u00e1s funciones con una nueva edici\u00f3n de Windows 7"},
    {4, L"Desconocido"},
    {50, L"Introduzca la clave de Windows Anytime Upgrade"},
    {51, L"Atr\u00e1s"},
    {52, L"Seguridad"},
    {53, L"Progreso de la descarga"},
    {54, L"Progreso de la actualizaci\u00f3n"},
    {55, L"Progreso de la comprobaci\u00f3n de la clave de actualizaci\u00f3n"},
    {56, L"Internet"},
    {57, L"Error"},
    {58, L"Advertencia"},
    {59, L"Informaci\u00f3n"},
    {101, L"Al hacer clic en \u00abAcepto\u00bb, acepta los <a>T\u00e9rminos de licencia del software de Microsoft.</a>"},
    {102, L"Con\u00e9ctese a Internet y abra Windows Anytime Upgrade para comparar las ediciones de Windows 7. Despu\u00e9s podr\u00e1 comprar la actualizaci\u00f3n en l\u00ednea e instalarla en el equipo. Para obtener m\u00e1s informaci\u00f3n, consulte: <a>\u00bfQu\u00e9 necesito para conectarme a Internet?</a>"},
    {103, L"Con\u00e9ctese para comprobar si su equipo est\u00e1 preparado para actualizarse a otra edici\u00f3n de Windows 7"},
    {104, L"La actualizaci\u00f3n se ha cancelado\n\nPara continuar con la actualizaci\u00f3n m\u00e1s adelante, abra Windows Anytime Upgrade y reanude el proceso."},
    {105, L"Para actualizar, debe aceptar los t\u00e9rminos de licencia\n\nLa actualizaci\u00f3n se ha cancelado. Para continuar, vuelva a abrir Windows Anytime Upgrade y acepte los t\u00e9rminos de licencia."},
    {106, L"Esta p\u00e1gina web est\u00e1 protegida mediante cifrado. Haga clic aqu\u00ed para ver la informaci\u00f3n del certificado digital."},
    {107, L"Despu\u00e9s de la compra, Windows se actualizar\u00e1 autom\u00e1ticamente."},
    {108, L"Si ya tiene una clave de Windows Anytime Upgrade, comience el proceso aqu\u00ed."},
    {109, L"Compruebe su clave de Windows Anytime Upgrade y aseg\u00farese de escribir correctamente los 25 caracteres."},
    {151, L"La clave de actualizaci\u00f3n no es v\u00e1lida. Compruebe la clave e int\u00e9ntelo de nuevo."},
    {152, L"La clave de actualizaci\u00f3n introducida corresponde a una edici\u00f3n de Windows que no es compatible con Windows Anytime Upgrade. Introduzca otra clave de actualizaci\u00f3n."},
    {153, L"No se ha podido realizar la actualizaci\u00f3n. Vuelva a intentarlo."},
    {154, L"Para completar la actualizaci\u00f3n, permita que se ejecute Windows Anytime Upgrade. Vuelva a intentarlo y haga clic en \u00abS\u00ed\u00bb cuando se lo solicite el Control de cuentas de usuario."},
    {155, L"Antes de actualizar, debe activar la edici\u00f3n de Windows que est\u00e1 ejecutando actualmente el equipo."},
    {156, L"Windows Anytime Upgrade no est\u00e1 disponible en esta edici\u00f3n de Windows"},
    {157, L"P\u00f3ngase en contacto con el administrador del sistema para obtener m\u00e1s informaci\u00f3n."},
    {158, L"Windows Anytime Upgrade ya se est\u00e1 ejecutando. Cierre Windows Anytime Upgrade en una de las cuentas de usuario."},
    {159, L"Active Restaurar sistema para el disco que contiene la instalaci\u00f3n de Windows e int\u00e9ntelo de nuevo."},
    {160, L"Windows Anytime Upgrade no puede abrir la p\u00e1gina web"},
    {161, L"Cierre Windows Anytime Upgrade y vuelva a abrirlo."},
    {162, L"Windows Anytime Upgrade no puede descargar el recibo"},
    {163, L"Consulte su correo electr\u00f3nico para encontrar el recibo y la confirmaci\u00f3n."},
    {164, L"La clave de actualizaci\u00f3n se ha descargado correctamente."},
    {165, L"Windows Anytime Upgrade no se ha completado correctamente"},
    {166, L"Cierre Windows Anytime Upgrade e intente realizar la actualizaci\u00f3n de nuevo."},
    {167, L"Cierre Windows Anytime Upgrade y vuelva a abrirlo para intentar validar la clave otra vez."},
    {168, L"Activar Windows"},
    {169, L"Activar Windows ahora"},
    {170, L"Windows Anytime Upgrade no pudo crear un punto de restauraci\u00f3n"},
    {171, L"\u00bfC\u00f3mo se activa Restaurar sistema?"},
    {174, L"Buscando actualizaciones"},
    {175, L"Descargando 1 actualizaci\u00f3n"},
    {176, L"Descargando %d actualizaciones"},
    {177, L"Instalando 1 actualizaci\u00f3n"},
    {178, L"Instalando %d actualizaciones"},
    {179, L"Introduzca otra clave de actualizaci\u00f3n"},
    {180, L"Permitir que se ejecute Windows Anytime Upgrade"},
    {181, L"Windows Anytime Upgrade se ha deshabilitado"},
    {182, L"Windows Anytime Upgrade ya se est\u00e1 ejecutando"},
    {183, L"No se ha podido validar la clave de actualizaci\u00f3n"},
    {184, L"Con\u00e9ctese para intentar resolver este problema"},
    {1100, L"El equipo est\u00e1 ejecutando actualmente"},
    {1101, L"Windows Anytime Upgrade"},
    {1102, L"Haga mucho m\u00e1s con su PC"},
    {1103, L"\u00bfC\u00f3mo desea comenzar?"},
    {1104, L"Con\u00e9ctese para elegir la edici\u00f3n de Windows 7 que mejor se adapte a sus necesidades"},
    {1106, L"Introducir una clave de actualizaci\u00f3n"},
    {1108, L"\u00bfC\u00f3mo puedo actualizar si no estoy conectado a Internet?"},
    {1109, L"Cancelar"},
    {1110, L"Introduzca su clave de actualizaci\u00f3n"},
    {1111, L"Para comenzar el proceso de actualizaci\u00f3n, introduzca la clave de Windows Anytime Upgrade de 25 caracteres correspondiente a la nueva edici\u00f3n de Windows 7."},
    {1112, L"Aseg\u00farese de introducir la clave de Windows Anytime Upgrade y no la clave de producto que ven\u00eda con el equipo o con su copia de Windows."},
    {1113, L"Clave de Windows Anytime Upgrade:"},
    {1114, L"(los guiones se a\u00f1adir\u00e1n autom\u00e1ticamente)"},
    {1115, L"Siguiente"},
    {1116, L"No est\u00e1 conectado a Internet"},
    {1117, L"Puede comenzar Windows Anytime Upgrade de una de estas dos maneras:"},
    {1118, L"Vaya a una tienda y compre una clave de Windows Anytime Upgrade. Cuando la tenga, abra Windows Anytime Upgrade en el equipo y actualice a una nueva edici\u00f3n de Windows 7."},
    {1119, L"Comprobando la clave"},
    {1120, L"La comprobaci\u00f3n de la clave tardar\u00e1 menos de un minuto."},
    {1121, L"Acepte los t\u00e9rminos de licencia"},
    {1122, L"Acepto"},
    {1123, L"Actualizar Windows"},
    {1124, L"Antes de actualizar, guarde su trabajo y cierre todos los programas abiertos."},
    {1125, L"La actualizaci\u00f3n puede tardar tan solo 10 minutos. Puede tardar m\u00e1s dependiendo de su PC y de si se necesitan actualizaciones en l\u00ednea. No podr\u00e1 utilizar el equipo durante la actualizaci\u00f3n."},
    {1126, L"El equipo se reiniciar\u00e1 autom\u00e1ticamente. En algunas actualizaciones, es posible que el equipo se reinicie varias veces."},
    {1127, L"Actualizar"},
    {1128, L"Actualizando Windows"},
    {1129, L"La actualizaci\u00f3n puede tardar tan solo 10 minutos. Puede tardar m\u00e1s dependiendo de su PC y de si se necesitan actualizaciones en l\u00ednea."},
    {1130, L"El equipo se reiniciar\u00e1 autom\u00e1ticamente."},
    {1131, L"Iniciando la actualizaci\u00f3n"},
    {1132, L"Cerrar"},
    {1133, L"La clave de actualizaci\u00f3n tiene un aspecto similar al siguiente:"},
    {1134, L"CLAVE DE ACTUALIZACI\u00d3N: XXXXX-XXXXX-XXXXX-XXXXX-XXXXX"},
    {1135, L"En tan solo 10 minutos, puede a\u00f1adir nuevas funciones a Windows 7 y hacer mucho m\u00e1s con su PC. Es f\u00e1cil y r\u00e1pido, y conservar\u00e1 sus programas, archivos y configuraci\u00f3n. La actualizaci\u00f3n puede tardar m\u00e1s dependiendo de su PC y de si se necesitan actualizaciones en l\u00ednea."},
    {1138, L"Aceptar"},
};

static constexpr AnytimeUpgradeString kAnytimeUpgradeStringsFr[] = {
    {1, L"Windows Anytime Upgrade"},
    {2, L"Un moyen pratique et \u00e9conomique de mettre \u00e0 niveau Windows"},
    {3, L"Profitez de plus de fonctionnalit\u00e9s avec une nouvelle \u00e9dition de Windows 7"},
    {4, L"Inconnu"},
    {50, L"Entrer la cl\u00e9 Windows Anytime Upgrade"},
    {51, L"Pr\u00e9c\u00e9dent"},
    {52, L"S\u00e9curit\u00e9"},
    {53, L"Progression du t\u00e9l\u00e9chargement"},
    {54, L"Progression de la mise \u00e0 niveau"},
    {55, L"Progression de la v\u00e9rification de la cl\u00e9 de mise \u00e0 niveau"},
    {56, L"Internet"},
    {57, L"Erreur"},
    {58, L"Avertissement"},
    {59, L"Information"},
    {101, L"Si vous cliquez sur \u00ab J'accepte \u00bb, vous acceptez les <a>termes du contrat de licence logiciel Microsoft.</a>"},
    {102, L"Connectez-vous \u00e0 Internet et ouvrez Windows Anytime Upgrade pour comparer les \u00e9ditions de Windows 7.  Vous pouvez ensuite acheter la mise \u00e0 niveau en ligne et l'installer sur votre ordinateur.  Pour plus d'informations, consultez :  <a>De quoi ai-je besoin pour me connecter \u00e0 Internet ?</a>"},
    {103, L"Connectez-vous en ligne pour v\u00e9rifier si votre ordinateur peut \u00eatre mis \u00e0 niveau vers une autre \u00e9dition de Windows 7"},
    {104, L"La mise \u00e0 niveau a \u00e9t\u00e9 annul\u00e9e\n\nPour poursuivre la mise \u00e0 niveau ult\u00e9rieurement, ouvrez Windows Anytime Upgrade et reprenez la proc\u00e9dure de mise \u00e0 niveau."},
    {105, L"Pour effectuer la mise \u00e0 niveau, vous devez accepter les termes du contrat de licence\n\nLa mise \u00e0 niveau a \u00e9t\u00e9 annul\u00e9e. Pour la poursuivre, ouvrez de nouveau Windows Anytime Upgrade et acceptez les termes du contrat de licence."},
    {106, L"Cette page Web est prot\u00e9g\u00e9e par un chiffrement de s\u00e9curit\u00e9. Cliquez ici pour afficher les informations relatives au certificat num\u00e9rique."},
    {107, L"Apr\u00e8s votre achat, Windows sera mis \u00e0 niveau automatiquement."},
    {108, L"Si vous poss\u00e9dez d\u00e9j\u00e0 une cl\u00e9 Windows Anytime Upgrade, commencez la proc\u00e9dure ici."},
    {109, L"V\u00e9rifiez votre cl\u00e9 Windows Anytime Upgrade et assurez-vous de saisir correctement les 25 caract\u00e8res."},
    {151, L"La cl\u00e9 de mise \u00e0 niveau n'est pas valide. V\u00e9rifiez la cl\u00e9 et r\u00e9essayez."},
    {152, L"La cl\u00e9 de mise \u00e0 niveau que vous avez entr\u00e9e correspond \u00e0 une \u00e9dition de Windows qui n'est pas compatible avec Windows Anytime Upgrade.  Entrez une autre cl\u00e9 de mise \u00e0 niveau."},
    {153, L"La mise \u00e0 niveau a \u00e9chou\u00e9. Veuillez r\u00e9essayer."},
    {154, L"Pour terminer la mise \u00e0 niveau, autorisez l'ex\u00e9cution de Windows Anytime Upgrade.  R\u00e9essayez et cliquez sur \u00ab Oui \u00bb lorsque le Contr\u00f4le de compte d'utilisateur vous le demande."},
    {155, L"Avant de pouvoir effectuer la mise \u00e0 niveau, vous devez activer l'\u00e9dition de Windows actuellement ex\u00e9cut\u00e9e sur votre ordinateur."},
    {156, L"Windows Anytime Upgrade n'est pas disponible dans cette \u00e9dition de Windows"},
    {157, L"Contactez votre administrateur syst\u00e8me pour plus d'informations."},
    {158, L"Windows Anytime Upgrade est d\u00e9j\u00e0 en cours d'ex\u00e9cution.  Fermez Windows Anytime Upgrade sur l'un des comptes d'utilisateur."},
    {159, L"Activez la Restauration du syst\u00e8me pour le disque contenant votre installation de Windows, puis r\u00e9essayez."},
    {160, L"Windows Anytime Upgrade ne peut pas ouvrir la page Web"},
    {161, L"Fermez Windows Anytime Upgrade, puis rouvrez-le."},
    {162, L"Windows Anytime Upgrade ne peut pas t\u00e9l\u00e9charger votre re\u00e7u"},
    {163, L"Consultez votre messagerie pour obtenir votre re\u00e7u et votre confirmation."},
    {164, L"Votre cl\u00e9 de mise \u00e0 niveau a \u00e9t\u00e9 t\u00e9l\u00e9charg\u00e9e avec succ\u00e8s."},
    {165, L"Windows Anytime Upgrade a \u00e9chou\u00e9"},
    {166, L"Fermez Windows Anytime Upgrade et essayez de nouveau d'effectuer la mise \u00e0 niveau."},
    {167, L"Fermez Windows Anytime Upgrade, puis rouvrez-le pour relancer la validation."},
    {168, L"Activer Windows"},
    {169, L"Activer Windows maintenant"},
    {170, L"Windows Anytime Upgrade n'a pas pu cr\u00e9er de point de restauration"},
    {171, L"Comment activer la Restauration du syst\u00e8me ?"},
    {174, L"Recherche des mises \u00e0 jour"},
    {175, L"T\u00e9l\u00e9chargement de 1 mise \u00e0 jour"},
    {176, L"T\u00e9l\u00e9chargement de %d mises \u00e0 jour"},
    {177, L"Installation de 1 mise \u00e0 jour"},
    {178, L"Installation de %d mises \u00e0 jour"},
    {179, L"Entrer une autre cl\u00e9 de mise \u00e0 niveau"},
    {180, L"Autoriser l'ex\u00e9cution de Windows Anytime Upgrade"},
    {181, L"Windows Anytime Upgrade a \u00e9t\u00e9 d\u00e9sactiv\u00e9"},
    {182, L"Windows Anytime Upgrade est d\u00e9j\u00e0 en cours d'ex\u00e9cution"},
    {183, L"La cl\u00e9 de mise \u00e0 niveau n'a pas pu \u00eatre valid\u00e9e"},
    {184, L"Connectez-vous en ligne pour essayer de r\u00e9soudre ce probl\u00e8me"},
    {1100, L"Votre ordinateur ex\u00e9cute actuellement"},
    {1101, L"Windows Anytime Upgrade"},
    {1102, L"Tirez encore plus parti de votre PC"},
    {1103, L"Comment voulez-vous commencer ?"},
    {1104, L"Connectez-vous en ligne pour choisir l'\u00e9dition de Windows 7 qui vous convient le mieux"},
    {1106, L"Entrer une cl\u00e9 de mise \u00e0 niveau"},
    {1108, L"Comment effectuer la mise \u00e0 niveau si je ne suis pas connect\u00e9 \u00e0 Internet ?"},
    {1109, L"Annuler"},
    {1110, L"Entrez votre cl\u00e9 de mise \u00e0 niveau"},
    {1111, L"Pour commencer la mise \u00e0 niveau, entrez la cl\u00e9 Windows Anytime Upgrade de 25 caract\u00e8res correspondant \u00e0 votre nouvelle \u00e9dition de Windows 7."},
    {1112, L"Veillez \u00e0 entrer votre cl\u00e9 Windows Anytime Upgrade et non la cl\u00e9 de produit fournie avec votre ordinateur ou votre copie de Windows."},
    {1113, L"Cl\u00e9 Windows Anytime Upgrade :"},
    {1114, L"(les tirets seront ajout\u00e9s automatiquement)"},
    {1115, L"Suivant"},
    {1116, L"Vous n'\u00eates pas connect\u00e9 \u00e0 Internet"},
    {1117, L"Vous pouvez commencer Windows Anytime Upgrade de deux fa\u00e7ons :"},
    {1118, L"Rendez-vous dans un magasin et achetez une cl\u00e9 Windows Anytime Upgrade.  Une fois en sa possession, ouvrez Windows Anytime Upgrade sur votre ordinateur et passez \u00e0 une nouvelle \u00e9dition de Windows 7."},
    {1119, L"V\u00e9rification de la cl\u00e9"},
    {1120, L"La v\u00e9rification de la cl\u00e9 prendra moins d'une minute."},
    {1121, L"Veuillez accepter les termes du contrat de licence"},
    {1122, L"J'accepte"},
    {1123, L"Mettre \u00e0 niveau Windows"},
    {1124, L"Avant de proc\u00e9der \u00e0 la mise \u00e0 niveau, enregistrez votre travail et fermez tous les programmes ouverts."},
    {1125, L"La mise \u00e0 niveau peut prendre \u00e0 peine 10 minutes.  Elle peut durer plus longtemps selon votre PC et la n\u00e9cessit\u00e9 ou non de mises \u00e0 jour en ligne.  Vous ne pouvez pas utiliser votre ordinateur pendant la mise \u00e0 niveau."},
    {1126, L"Votre ordinateur red\u00e9marrera automatiquement.  Pour certaines mises \u00e0 niveau, il peut red\u00e9marrer plusieurs fois."},
    {1127, L"Mettre \u00e0 niveau"},
    {1128, L"Mise \u00e0 niveau de Windows"},
    {1129, L"La mise \u00e0 niveau peut prendre \u00e0 peine 10 minutes.  Elle peut durer plus longtemps selon votre PC et la n\u00e9cessit\u00e9 ou non de mises \u00e0 jour en ligne."},
    {1130, L"Votre ordinateur red\u00e9marrera automatiquement."},
    {1131, L"D\u00e9marrage de la mise \u00e0 niveau"},
    {1132, L"Fermer"},
    {1133, L"La cl\u00e9 de mise \u00e0 niveau ressemble \u00e0 ceci :"},
    {1134, L"CL\u00c9 DE MISE \u00c0 NIVEAU : XXXXX-XXXXX-XXXXX-XXXXX-XXXXX"},
    {1135, L"En \u00e0 peine 10 minutes, vous pouvez ajouter de nouvelles fonctionnalit\u00e9s \u00e0 Windows 7 et en faire plus avec votre PC.  C'est simple et rapide, et vous conservez vos programmes, fichiers et param\u00e8tres.  La mise \u00e0 niveau peut durer plus longtemps selon votre PC et la n\u00e9cessit\u00e9 ou non de mises \u00e0 jour en ligne."},
    {1138, L"OK"},
};

static constexpr AnytimeUpgradeString kAnytimeUpgradeStringsPt[] = {
    {1, L"Windows Anytime Upgrade"},
    {2, L"Uma maneira conveniente e econ\u00f4mica de atualizar o Windows"},
    {3, L"Obtenha mais recursos com uma nova edi\u00e7\u00e3o do Windows 7"},
    {4, L"Desconhecido"},
    {50, L"Inserir a chave do Windows Anytime Upgrade"},
    {51, L"Voltar"},
    {52, L"Seguran\u00e7a"},
    {53, L"Progresso do download"},
    {54, L"Progresso da atualiza\u00e7\u00e3o"},
    {55, L"Progresso da verifica\u00e7\u00e3o da chave de atualiza\u00e7\u00e3o"},
    {56, L"Internet"},
    {57, L"Erro"},
    {58, L"Aviso"},
    {59, L"Informa\u00e7\u00f5es"},
    {101, L"Ao clicar em \"Aceito\", voc\u00ea concorda com os <a>Termos de Licen\u00e7a para Software Microsoft.</a>"},
    {102, L"Conecte-se \u00e0 Internet e abra o Windows Anytime Upgrade para comparar as edi\u00e7\u00f5es do Windows 7.  Em seguida, voc\u00ea poder\u00e1 comprar a atualiza\u00e7\u00e3o online e instal\u00e1-la no computador.  Para obter mais informa\u00e7\u00f5es, consulte:  <a>O que preciso para me conectar \u00e0 Internet?</a>"},
    {103, L"Conecte-se online para ver se o computador est\u00e1 pronto para ser atualizado para outra edi\u00e7\u00e3o do Windows 7"},
    {104, L"A atualiza\u00e7\u00e3o foi cancelada\n\nPara continuar a atualiza\u00e7\u00e3o mais tarde, abra o Windows Anytime Upgrade e retome o processo de atualiza\u00e7\u00e3o."},
    {105, L"Para atualizar, \u00e9 necess\u00e1rio aceitar os termos de licen\u00e7a\n\nA atualiza\u00e7\u00e3o foi cancelada. Para continuar, abra o Windows Anytime Upgrade novamente e aceite os termos de licen\u00e7a."},
    {106, L"Esta p\u00e1gina da Web possui criptografia de seguran\u00e7a. Clique aqui para exibir as informa\u00e7\u00f5es do certificado digital."},
    {107, L"Ap\u00f3s a compra, o Windows ser\u00e1 atualizado automaticamente."},
    {108, L"Se voc\u00ea j\u00e1 tem uma chave do Windows Anytime Upgrade, inicie o processo aqui."},
    {109, L"Verifique a chave do Windows Anytime Upgrade e certifique-se de digitar corretamente todos os 25 caracteres."},
    {151, L"A chave de atualiza\u00e7\u00e3o n\u00e3o \u00e9 v\u00e1lida. Verifique a chave e tente novamente."},
    {152, L"A chave de atualiza\u00e7\u00e3o inserida \u00e9 de uma edi\u00e7\u00e3o do Windows que n\u00e3o funciona com o Windows Anytime Upgrade.  Insira uma chave de atualiza\u00e7\u00e3o diferente."},
    {153, L"A atualiza\u00e7\u00e3o n\u00e3o foi bem-sucedida. Tente atualizar novamente."},
    {154, L"Para concluir a atualiza\u00e7\u00e3o, permita a execu\u00e7\u00e3o do Windows Anytime Upgrade.  Tente novamente e clique em \"Sim\" quando o Controle de Conta de Usu\u00e1rio solicitar."},
    {155, L"Antes de atualizar, \u00e9 necess\u00e1rio ativar a edi\u00e7\u00e3o do Windows que o computador est\u00e1 executando no momento."},
    {156, L"O Windows Anytime Upgrade n\u00e3o est\u00e1 dispon\u00edvel nesta edi\u00e7\u00e3o do Windows"},
    {157, L"Entre em contato com o administrador do sistema para obter mais informa\u00e7\u00f5es."},
    {158, L"O Windows Anytime Upgrade j\u00e1 est\u00e1 em execu\u00e7\u00e3o.  Feche o Windows Anytime Upgrade em uma das contas de usu\u00e1rio."},
    {159, L"Ative a Restaura\u00e7\u00e3o do Sistema para o disco que cont\u00e9m a instala\u00e7\u00e3o do Windows e tente novamente."},
    {160, L"O Windows Anytime Upgrade n\u00e3o pode abrir a p\u00e1gina da Web"},
    {161, L"Feche o Windows Anytime Upgrade e abra-o novamente."},
    {162, L"O Windows Anytime Upgrade n\u00e3o pode baixar o seu recibo"},
    {163, L"Verifique seu e-mail para obter o recibo e a confirma\u00e7\u00e3o."},
    {164, L"A chave de atualiza\u00e7\u00e3o foi baixada com \u00eaxito."},
    {165, L"O Windows Anytime Upgrade n\u00e3o foi bem-sucedido"},
    {166, L"Feche o Windows Anytime Upgrade e tente atualizar novamente."},
    {167, L"Feche o Windows Anytime Upgrade e abra-o novamente para repetir a valida\u00e7\u00e3o."},
    {168, L"Ativar o Windows"},
    {169, L"Ativar o Windows agora"},
    {170, L"O Windows Anytime Upgrade n\u00e3o conseguiu criar um ponto de restaura\u00e7\u00e3o"},
    {171, L"Como ativar a Restaura\u00e7\u00e3o do Sistema?"},
    {174, L"Verificando atualiza\u00e7\u00f5es"},
    {175, L"Baixando 1 atualiza\u00e7\u00e3o"},
    {176, L"Baixando %d atualiza\u00e7\u00f5es"},
    {177, L"Instalando 1 atualiza\u00e7\u00e3o"},
    {178, L"Instalando %d atualiza\u00e7\u00f5es"},
    {179, L"Inserir uma chave de atualiza\u00e7\u00e3o diferente"},
    {180, L"Permitir a execu\u00e7\u00e3o do Windows Anytime Upgrade"},
    {181, L"O Windows Anytime Upgrade foi desabilitado"},
    {182, L"O Windows Anytime Upgrade j\u00e1 est\u00e1 em execu\u00e7\u00e3o"},
    {183, L"N\u00e3o foi poss\u00edvel validar a chave de atualiza\u00e7\u00e3o"},
    {184, L"Conecte-se online para tentar resolver este problema"},
    {1100, L"O computador est\u00e1 executando atualmente"},
    {1101, L"Windows Anytime Upgrade"},
    {1102, L"Fa\u00e7a ainda mais com o seu PC"},
    {1103, L"Como voc\u00ea deseja come\u00e7ar?"},
    {1104, L"Conecte-se online para escolher a edi\u00e7\u00e3o do Windows 7 mais adequada para voc\u00ea"},
    {1106, L"Inserir uma chave de atualiza\u00e7\u00e3o"},
    {1108, L"Como posso atualizar quando n\u00e3o estou conectado \u00e0 Internet?"},
    {1109, L"Cancelar"},
    {1110, L"Insira a chave de atualiza\u00e7\u00e3o"},
    {1111, L"Para iniciar o processo de atualiza\u00e7\u00e3o, insira a chave de 25 caracteres do Windows Anytime Upgrade para a nova edi\u00e7\u00e3o do Windows 7."},
    {1112, L"Insira a chave do Windows Anytime Upgrade e n\u00e3o a chave de produto que acompanha o computador ou a c\u00f3pia do Windows."},
    {1113, L"Chave do Windows Anytime Upgrade:"},
    {1114, L"(os hifens ser\u00e3o adicionados automaticamente)"},
    {1115, L"Avan\u00e7ar"},
    {1116, L"Voc\u00ea n\u00e3o est\u00e1 conectado \u00e0 Internet"},
    {1117, L"Voc\u00ea pode iniciar o Windows Anytime Upgrade de duas maneiras:"},
    {1118, L"V\u00e1 a uma loja e compre uma chave do Windows Anytime Upgrade.  Quando estiver com ela, abra o Windows Anytime Upgrade no computador e atualize para uma nova edi\u00e7\u00e3o do Windows 7."},
    {1119, L"Verificando a chave"},
    {1120, L"A verifica\u00e7\u00e3o da chave levar\u00e1 menos de um minuto."},
    {1121, L"Aceite os termos de licen\u00e7a"},
    {1122, L"Aceito"},
    {1123, L"Atualizar o Windows"},
    {1124, L"Antes de atualizar, salve seu trabalho e feche todos os programas abertos."},
    {1125, L"A atualiza\u00e7\u00e3o levar\u00e1 apenas 10 minutos.  Pode demorar mais, dependendo do seu PC e da necessidade de atualiza\u00e7\u00f5es online.  N\u00e3o \u00e9 poss\u00edvel usar o computador durante a atualiza\u00e7\u00e3o."},
    {1126, L"O computador ser\u00e1 reiniciado automaticamente.  Em algumas atualiza\u00e7\u00f5es, o computador pode ser reiniciado v\u00e1rias vezes."},
    {1127, L"Atualizar"},
    {1128, L"Atualizando o Windows"},
    {1129, L"A atualiza\u00e7\u00e3o levar\u00e1 apenas 10 minutos.  Pode demorar mais, dependendo do seu PC e da necessidade de atualiza\u00e7\u00f5es online."},
    {1130, L"O computador ser\u00e1 reiniciado automaticamente."},
    {1131, L"Iniciando a atualiza\u00e7\u00e3o"},
    {1132, L"Fechar"},
    {1133, L"A chave de atualiza\u00e7\u00e3o \u00e9 semelhante a esta:"},
    {1134, L"CHAVE DE ATUALIZA\u00c7\u00c3O: XXXXX-XXXXX-XXXXX-XXXXX-XXXXX"},
    {1135, L"Em apenas 10 minutos, voc\u00ea pode adicionar novos recursos ao Windows 7 e fazer mais com o seu PC.  \u00c9 f\u00e1cil e r\u00e1pido, e voc\u00ea mant\u00e9m seus programas, arquivos e configura\u00e7\u00f5es.  A atualiza\u00e7\u00e3o pode demorar mais, dependendo do seu PC e da necessidade de atualiza\u00e7\u00f5es online."},
    {1138, L"OK"},
};

static constexpr AnytimeUpgradeString kAnytimeUpgradeStringsDe[] = {
    {1, L"Windows Anytime Upgrade"},
    {2, L"Eine bequeme und g\u00fcnstige M\u00f6glichkeit, Windows zu aktualisieren"},
    {3, L"Mehr Funktionen mit einer neuen Edition von Windows 7"},
    {4, L"Unbekannt"},
    {50, L"Windows Anytime Upgrade-Schl\u00fcssel eingeben"},
    {51, L"Zur\u00fcck"},
    {52, L"Sicherheit"},
    {53, L"Downloadstatus"},
    {54, L"Upgradestatus"},
    {55, L"Status der Upgradeschl\u00fcssel\u00fcberpr\u00fcfung"},
    {56, L"Internet"},
    {57, L"Fehler"},
    {58, L"Warnung"},
    {59, L"Information"},
    {101, L"Wenn Sie auf \"Ich akzeptiere\" klicken, stimmen Sie den <a>Microsoft-Software-Lizenzbedingungen</a> zu."},
    {102, L"Stellen Sie eine Verbindung mit dem Internet her, und \u00f6ffnen Sie Windows Anytime Upgrade, um die Editionen von Windows 7 zu vergleichen.  Anschlie\u00dfend k\u00f6nnen Sie das Upgrade online kaufen und auf Ihrem Computer installieren.  Weitere Informationen finden Sie unter:  <a>Was wird f\u00fcr die Verbindung mit dem Internet ben\u00f6tigt?</a>"},
    {103, L"Stellen Sie eine Onlineverbindung her, um zu pr\u00fcfen, ob Ihr Computer f\u00fcr ein Upgrade auf eine andere Edition von Windows 7 bereit ist"},
    {104, L"Das Upgrade wurde abgebrochen\n\nUm das Upgrade sp\u00e4ter fortzusetzen, \u00f6ffnen Sie Windows Anytime Upgrade, und setzen Sie den Upgradevorgang fort."},
    {105, L"F\u00fcr das Upgrade m\u00fcssen Sie die Lizenzbedingungen akzeptieren\n\nDas Upgrade wurde abgebrochen. \u00d6ffnen Sie zum Fortsetzen Windows Anytime Upgrade erneut, und akzeptieren Sie die Lizenzbedingungen."},
    {106, L"Diese Webseite ist durch Sicherheitsverschl\u00fcsselung gesch\u00fctzt. Klicken Sie hier, um die Informationen zum digitalen Zertifikat anzuzeigen."},
    {107, L"Nach dem Kauf wird Windows automatisch aktualisiert."},
    {108, L"Wenn Sie bereits \u00fcber einen Windows Anytime Upgrade-Schl\u00fcssel verf\u00fcgen, beginnen Sie den Vorgang hier."},
    {109, L"\u00dcberpr\u00fcfen Sie Ihren Windows Anytime Upgrade-Schl\u00fcssel, und stellen Sie sicher, dass alle 25 Zeichen korrekt eingegeben wurden."},
    {151, L"Der Upgradeschl\u00fcssel ist ung\u00fcltig. \u00dcberpr\u00fcfen Sie den Schl\u00fcssel, und versuchen Sie es erneut."},
    {152, L"Der eingegebene Upgradeschl\u00fcssel geh\u00f6rt zu einer Windows-Edition, die nicht mit Windows Anytime Upgrade kompatibel ist.  Geben Sie einen anderen Upgradeschl\u00fcssel ein."},
    {153, L"Das Upgrade war nicht erfolgreich. Versuchen Sie das Upgrade erneut."},
    {154, L"Um das Upgrade abzuschlie\u00dfen, lassen Sie die Ausf\u00fchrung von Windows Anytime Upgrade zu.  Versuchen Sie es erneut, und klicken Sie auf \"Ja\", wenn die Benutzerkontensteuerung dazu auffordert."},
    {155, L"Vor dem Upgrade m\u00fcssen Sie die Windows-Edition aktivieren, die derzeit auf Ihrem Computer ausgef\u00fchrt wird."},
    {156, L"Windows Anytime Upgrade ist in dieser Windows-Edition nicht enthalten"},
    {157, L"Wenden Sie sich f\u00fcr weitere Informationen an Ihren Systemadministrator."},
    {158, L"Windows Anytime Upgrade wird bereits ausgef\u00fchrt.  Schlie\u00dfen Sie Windows Anytime Upgrade in einem der Benutzerkonten."},
    {159, L"Aktivieren Sie die Systemwiederherstellung f\u00fcr den Datentr\u00e4ger, auf dem Windows installiert ist, und versuchen Sie es erneut."},
    {160, L"Windows Anytime Upgrade kann die Webseite nicht \u00f6ffnen"},
    {161, L"Schlie\u00dfen Sie Windows Anytime Upgrade, und \u00f6ffnen Sie es erneut."},
    {162, L"Windows Anytime Upgrade kann Ihre Quittung nicht herunterladen"},
    {163, L"\u00dcberpr\u00fcfen Sie Ihr E-Mail-Postfach auf Quittung und Best\u00e4tigung."},
    {164, L"Ihr Upgradeschl\u00fcssel wurde erfolgreich heruntergeladen."},
    {165, L"Windows Anytime Upgrade war nicht erfolgreich"},
    {166, L"Schlie\u00dfen Sie Windows Anytime Upgrade, und versuchen Sie das Upgrade erneut."},
    {167, L"Schlie\u00dfen Sie Windows Anytime Upgrade, und \u00f6ffnen Sie es erneut, um die \u00dcberpr\u00fcfung zu wiederholen."},
    {168, L"Windows aktivieren"},
    {169, L"Windows jetzt aktivieren"},
    {170, L"Windows Anytime Upgrade konnte keinen Wiederherstellungspunkt erstellen"},
    {171, L"Wie aktiviere ich die Systemwiederherstellung?"},
    {174, L"Es wird nach Updates gesucht"},
    {175, L"1 Update wird heruntergeladen"},
    {176, L"%d Updates werden heruntergeladen"},
    {177, L"1 Update wird installiert"},
    {178, L"%d Updates werden installiert"},
    {179, L"Anderen Upgradeschl\u00fcssel eingeben"},
    {180, L"Ausf\u00fchrung von Windows Anytime Upgrade zulassen"},
    {181, L"Windows Anytime Upgrade wurde deaktiviert"},
    {182, L"Windows Anytime Upgrade wird bereits ausgef\u00fchrt"},
    {183, L"Der Upgradeschl\u00fcssel konnte nicht \u00fcberpr\u00fcft werden"},
    {184, L"Stellen Sie eine Onlineverbindung her, um das Problem zu beheben"},
    {1100, L"Auf Ihrem Computer wird derzeit ausgef\u00fchrt"},
    {1101, L"Windows Anytime Upgrade"},
    {1102, L"Holen Sie noch mehr aus Ihrem PC heraus"},
    {1103, L"Wie m\u00f6chten Sie beginnen?"},
    {1104, L"Stellen Sie eine Onlineverbindung her, um die f\u00fcr Sie am besten geeignete Edition von Windows 7 auszuw\u00e4hlen"},
    {1106, L"Upgradeschl\u00fcssel eingeben"},
    {1108, L"Wie kann ich ein Upgrade durchf\u00fchren, wenn keine Internetverbindung besteht?"},
    {1109, L"Abbrechen"},
    {1110, L"Geben Sie Ihren Upgradeschl\u00fcssel ein"},
    {1111, L"Geben Sie zum Starten des Upgradevorgangs den aus 25 Zeichen bestehenden Windows Anytime Upgrade-Schl\u00fcssel f\u00fcr Ihre neue Edition von Windows 7 ein."},
    {1112, L"Geben Sie unbedingt Ihren Windows Anytime Upgrade-Schl\u00fcssel ein und nicht den Product Key, der mit Ihrem Computer oder Ihrer Windows-Kopie geliefert wurde."},
    {1113, L"Windows Anytime Upgrade-Schl\u00fcssel:"},
    {1114, L"(Bindestriche werden automatisch hinzugef\u00fcgt)"},
    {1115, L"Weiter"},
    {1116, L"Es besteht keine Verbindung mit dem Internet"},
    {1117, L"Sie k\u00f6nnen Windows Anytime Upgrade auf zwei Arten starten:"},
    {1118, L"Kaufen Sie in einem Gesch\u00e4ft einen Windows Anytime Upgrade-Schl\u00fcssel.  \u00d6ffnen Sie anschlie\u00dfend Windows Anytime Upgrade auf Ihrem Computer, und f\u00fchren Sie ein Upgrade auf eine neue Edition von Windows 7 durch."},
    {1119, L"Schl\u00fcssel wird \u00fcberpr\u00fcft"},
    {1120, L"Die \u00dcberpr\u00fcfung des Schl\u00fcssels dauert weniger als eine Minute."},
    {1121, L"Bitte akzeptieren Sie die Lizenzbedingungen"},
    {1122, L"Ich akzeptiere"},
    {1123, L"Windows aktualisieren"},
    {1124, L"Speichern Sie vor dem Upgrade Ihre Arbeit, und schlie\u00dfen Sie alle ge\u00f6ffneten Programme."},
    {1125, L"Das Upgrade dauert nur etwa 10 Minuten.  Je nach PC und Bedarf an Onlineupdates kann es l\u00e4nger dauern.  W\u00e4hrend des Upgrades k\u00f6nnen Sie Ihren Computer nicht verwenden."},
    {1126, L"Ihr Computer wird automatisch neu gestartet.  Bei einigen Upgrades wird der Computer m\u00f6glicherweise mehrmals neu gestartet."},
    {1127, L"Upgrade"},
    {1128, L"Windows wird aktualisiert"},
    {1129, L"Das Upgrade dauert nur etwa 10 Minuten.  Je nach PC und Bedarf an Onlineupdates kann es l\u00e4nger dauern."},
    {1130, L"Ihr Computer wird automatisch neu gestartet."},
    {1131, L"Upgrade wird gestartet"},
    {1132, L"Schlie\u00dfen"},
    {1133, L"Der Upgradeschl\u00fcssel sieht ungef\u00e4hr so aus:"},
    {1134, L"UPGRADESCHL\u00dcSSEL: XXXXX-XXXXX-XXXXX-XXXXX-XXXXX"},
    {1135, L"In nur etwa 10 Minuten k\u00f6nnen Sie Windows 7 um neue Funktionen erweitern und mehr mit Ihrem PC machen.  Es ist einfach und schnell, und Ihre Programme, Dateien und Einstellungen bleiben erhalten.  Je nach PC und Bedarf an Onlineupdates kann das Upgrade l\u00e4nger dauern."},
    {1138, L"OK"},
};

static const AnytimeUpgradeString* g_strings = kAnytimeUpgradeStrings;
static size_t g_stringCount = ARRAYSIZE(kAnytimeUpgradeStrings);

// Select the embedded strings according to the user's Windows UI language.
// Italian, Spanish, French, Portuguese and German are localized; all other languages fall back to English.
static void SelectStringTable() {
    const LANGID language = PRIMARYLANGID(GetUserDefaultUILanguage());
    if (language == LANG_ITALIAN) {
        g_strings = kAnytimeUpgradeStringsIt;
        g_stringCount = ARRAYSIZE(kAnytimeUpgradeStringsIt);
    } else if (language == LANG_SPANISH) {
        g_strings = kAnytimeUpgradeStringsEs;
        g_stringCount = ARRAYSIZE(kAnytimeUpgradeStringsEs);
    } else if (language == LANG_FRENCH) {
        g_strings = kAnytimeUpgradeStringsFr;
        g_stringCount = ARRAYSIZE(kAnytimeUpgradeStringsFr);
    } else if (language == LANG_PORTUGUESE) {
        g_strings = kAnytimeUpgradeStringsPt;
        g_stringCount = ARRAYSIZE(kAnytimeUpgradeStringsPt);
    } else if (language == LANG_GERMAN) {
        g_strings = kAnytimeUpgradeStringsDe;
        g_stringCount = ARRAYSIZE(kAnytimeUpgradeStringsDe);
    } else {
        g_strings = kAnytimeUpgradeStrings;
        g_stringCount = ARRAYSIZE(kAnytimeUpgradeStrings);
    }
}

using AnytimeStringBlock = std::array<std::wstring, 16>;
using AnytimeStringBlocks = std::map<WORD, AnytimeStringBlock>;

static bool ReadExistingStringBlock(HMODULE module, WORD blockId,
                                    AnytimeStringBlock& strings) {
    if (!module) return false;
    HRSRC resource = FindResourceExW(module, RT_STRING,
                                     MAKEINTRESOURCEW(blockId), kResourceLanguage);
    if (!resource) {
        resource = FindResourceW(module, MAKEINTRESOURCEW(blockId), RT_STRING);
    }
    if (!resource) return false;

    HGLOBAL loaded = LoadResource(module, resource);
    const BYTE* cursor = loaded ? static_cast<const BYTE*>(LockResource(loaded)) : nullptr;
    DWORD remaining = SizeofResource(module, resource);
    if (!cursor) return false;

    for (size_t i = 0; i < strings.size(); ++i) {
        if (remaining < sizeof(WORD)) return false;
        WORD length = 0;
        memcpy(&length, cursor, sizeof(length));
        cursor += sizeof(length);
        remaining -= sizeof(length);
        const size_t bytes = static_cast<size_t>(length) * sizeof(wchar_t);
        if (bytes > remaining) return false;
        strings[i].assign(reinterpret_cast<const wchar_t*>(cursor), length);
        cursor += bytes;
        remaining -= static_cast<DWORD>(bytes);
    }
    return true;
}

static void AppendWord(Bytes& data, WORD value) {
    const BYTE* bytes = reinterpret_cast<const BYTE*>(&value);
    data.insert(data.end(), bytes, bytes + sizeof(value));
}

static bool BuildLocalizedModule(const std::wstring& sourcePath,
                                 const std::wstring& destinationPath) {
    if (DeleteFileW(destinationPath.c_str()) == FALSE &&
        GetLastError() != ERROR_FILE_NOT_FOUND && GetLastError() != ERROR_PATH_NOT_FOUND) {
        Wh_Log(L"Anytime Upgrade: could not remove old localized file %s (error=%lu)",
               destinationPath.c_str(), GetLastError());
        return false;
    }
    if (!CopyFileW(sourcePath.c_str(), destinationPath.c_str(), FALSE)) {
        Wh_Log(L"Anytime Upgrade: could not copy %s to the localized folder (error=%lu)",
               sourcePath.c_str(), GetLastError());
        return false;
    }

    HMODULE sourceModule = LoadLibraryExW(sourcePath.c_str(), nullptr,
        LOAD_LIBRARY_AS_DATAFILE | LOAD_LIBRARY_AS_IMAGE_RESOURCE);
    if (!sourceModule) {
        Wh_Log(L"Anytime Upgrade: could not inspect original resources in %s (error=%lu)",
               sourcePath.c_str(), GetLastError());
        DeleteFileW(destinationPath.c_str());
        return false;
    }

    AnytimeStringBlocks blocks;
    for (size_t entryIndex = 0; entryIndex < g_stringCount; ++entryIndex) {
        const auto& entry = g_strings[entryIndex];
        const WORD blockId = static_cast<WORD>((entry.id >> 4) + 1);
        blocks.try_emplace(blockId);
    }
    for (auto& pair : blocks) {
        // Preserve any strings already embedded in the source file before
        // replacing the supplied IDs in this resource block.
        ReadExistingStringBlock(sourceModule, pair.first, pair.second);
    }
    for (size_t entryIndex = 0; entryIndex < g_stringCount; ++entryIndex) {
        const auto& entry = g_strings[entryIndex];
        const WORD blockId = static_cast<WORD>((entry.id >> 4) + 1);
        const size_t slot = entry.id & 0x0F;
        blocks[blockId][slot] = entry.text;
    }

    HANDLE update = BeginUpdateResourceW(destinationPath.c_str(), FALSE);
    if (!update) {
        const DWORD error = GetLastError();
        FreeLibrary(sourceModule);
        DeleteFileW(destinationPath.c_str());
        Wh_Log(L"Anytime Upgrade: BeginUpdateResource failed for %s (error=%lu)",
               destinationPath.c_str(), error);
        return false;
    }

    bool ok = true;
    // The original modules carry a MUI configuration that redirects resources
    // (including RT_STRING) to an external .mui file that does not exist here.
    // Remove it from the private copies so the embedded strings are used.
    UpdateResourceW(update, L"MUI", MAKEINTRESOURCEW(1), kResourceLanguage, nullptr, 0);
    for (const auto& pair : blocks) {
        Bytes blob;
        try {
            for (const auto& value : pair.second) {
                if (value.size() > 0xFFFF) { ok = false; break; }
                AppendWord(blob, static_cast<WORD>(value.size()));
                const BYTE* chars = reinterpret_cast<const BYTE*>(value.data());
                blob.insert(blob.end(), chars, chars + value.size() * sizeof(wchar_t));
            }
        } catch (...) {
            ok = false;
        }
        if (!ok || blob.size() > MAXDWORD) {
            ok = false;
            break;
        }

        // DirectUI may request neutral resources even when the process UI
        // language is not English. Publish both neutral and en-US variants so
        // strings such as the page banner title (1101) and OK button (1138)
        // are found by the resource provider on German Windows as well.
        if (!UpdateResourceW(update, RT_STRING, MAKEINTRESOURCEW(pair.first),
                             MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL),
                             blob.data(), static_cast<DWORD>(blob.size())) ||
            !UpdateResourceW(update, RT_STRING, MAKEINTRESOURCEW(pair.first),
                             kResourceLanguage, blob.data(),
                             static_cast<DWORD>(blob.size()))) {
            ok = false;
            break;
        }
    }

    const DWORD updateError = ok ? ERROR_SUCCESS : GetLastError();
    const BOOL endResult = EndUpdateResourceW(update, ok ? FALSE : TRUE);
    FreeLibrary(sourceModule);
    if (!ok || !endResult) {
        const DWORD error = ok ? GetLastError() : updateError;
        DeleteFileW(destinationPath.c_str());
        Wh_Log(L"Anytime Upgrade: could not embed string resources in %s (error=%lu)",
               destinationPath.c_str(), error);
        return false;
    }
    return true;
}

static bool BuildLocalizedPayload() {
    SelectStringTable();
    g_localizedDir = g_storageDir + L"\\" + kLocalizedDirectoryName;
    if (!EnsureDirectory(g_localizedDir)) {
        Wh_Log(L"Anytime Upgrade: could not create localized payload directory");
        return false;
    }

    const std::wstring originalExe = g_storageDir + L"\\" + kExeName;
    const std::wstring originalDll = g_storageDir + L"\\" + kDllName;
    const std::wstring localizedExe = g_localizedDir + L"\\" + kExeName;
    const std::wstring localizedDll = g_localizedDir + L"\\" + kDllName;

    if (!BuildLocalizedModule(originalExe, localizedExe)) return false;
    if (!BuildLocalizedModule(originalDll, localizedDll)) {
        DeleteFileW(localizedExe.c_str());
        return false;
    }
    Wh_Log(L"Anytime Upgrade: embedded %u localized strings into the private EXE/DLL copies",
           static_cast<unsigned>(g_stringCount));
    return true;
}

static bool EnsurePayload() {
    if (g_storageDir.empty() || !EnsureDirectory(g_storageDir)) {
        Wh_Log(L"Anytime Upgrade: could not create the mod storage directory");
        return false;
    }

    Bytes exeBytes;
    Bytes dllBytes;
    WORD exeMachine = 0;
    WORD dllMachine = 0;

    if (!LoadOrDownload(kExeName, kExeUrls, ARRAYSIZE(kExeUrls), kExeSha256, false, exeBytes, exeMachine, true)) return false;
    if (!LoadOrDownload(kDllName, kDllUrls, ARRAYSIZE(kDllUrls), kDllSha256, true, dllBytes, dllMachine)) return false;

    if (exeMachine != dllMachine) {
        Wh_Log(L"Anytime Upgrade: EXE/DLL architecture mismatch (EXE=0x%04X, DLL=0x%04X)",
               exeMachine, dllMachine);
        // The pair must match. Remove both cached files so the next enable fetches a fresh pair.
        DeleteFileW((g_storageDir + L"\\" + kExeName).c_str());
        DeleteFileW((g_storageDir + L"\\" + kDllName).c_str());
        Wh_Log(L"Anytime Upgrade: cached pair removed; toggle the mod to retry the download");
        return false;
    }

    if (g_stopping.load()) return false;

    // Resource localization is optional. It must never prevent the Control
    // Panel item from being registered: the original EXE was already known to
    // launch correctly in version 1.0. If resource editing fails for either
    // module, fall back to the untouched downloaded pair in the storage root.
    if (!BuildLocalizedPayload()) {
        g_localizedDir = g_storageDir;
        Wh_Log(L"Anytime Upgrade: resource localization failed; using the original downloaded EXE/DLL pair");
    }
    return !g_stopping.load();
}

static bool KeyExists(HKEY root, const std::wstring& path) {
    HKEY key = nullptr;
    const LSTATUS status = RegOpenKeyExW(root, path.c_str(), 0,
                                         KEY_QUERY_VALUE | KEY_WOW64_64KEY, &key);
    if (status == ERROR_SUCCESS) RegCloseKey(key);
    return status == ERROR_SUCCESS;
}

static bool IsOurRegistrationKey(const std::wstring& path) {
    HKEY key = nullptr;
    if (RegOpenKeyExW(HKEY_CURRENT_USER, path.c_str(), 0,
                      KEY_QUERY_VALUE | KEY_WOW64_64KEY, &key) != ERROR_SUCCESS) {
        return false;
    }

    wchar_t marker[128] = {};
    DWORD type = 0;
    DWORD size = sizeof(marker);
    const LSTATUS status = RegQueryValueExW(key, kMarkerName, nullptr, &type,
                                            reinterpret_cast<BYTE*>(marker), &size);
    RegCloseKey(key);
    return status == ERROR_SUCCESS && type == REG_SZ &&
           _wcsicmp(marker, kMarkerValue) == 0;
}

static void DeleteOwnedTree(const std::wstring& path) {
    if (!IsOurRegistrationKey(path)) return;

    HKEY key = nullptr;
    const LSTATUS openStatus = RegOpenKeyExW(
        HKEY_CURRENT_USER, path.c_str(), 0,
        DELETE | KEY_ENUMERATE_SUB_KEYS | KEY_QUERY_VALUE | KEY_WOW64_64KEY,
        &key);
    if (openStatus == ERROR_SUCCESS) {
        const LSTATUS treeStatus = RegDeleteTreeW(key, nullptr);
        if (treeStatus != ERROR_SUCCESS) {
            Wh_Log(L"Anytime Upgrade: RegDeleteTree failed for %s (error=%ld)",
                   path.c_str(), treeStatus);
        }
        RegCloseKey(key);
    }

    const LSTATUS deleteStatus = RegDeleteKeyExW(
        HKEY_CURRENT_USER, path.c_str(), KEY_WOW64_64KEY, 0);
    if (deleteStatus != ERROR_SUCCESS && deleteStatus != ERROR_FILE_NOT_FOUND &&
        deleteStatus != ERROR_PATH_NOT_FOUND) {
        Wh_Log(L"Anytime Upgrade: RegDeleteKeyEx failed for %s (error=%ld)",
               path.c_str(), deleteStatus);
    }
}

static bool EnsureVolatilePath(const std::wstring& relativePath) {
    size_t position = 0;
    std::wstring current;
    while (position < relativePath.size()) {
        const size_t next = relativePath.find(L'\\', position);
        const size_t end = next == std::wstring::npos ? relativePath.size() : next;
        if (end == position) return false;
        if (!current.empty()) current += L"\\";
        current.append(relativePath, position, end - position);

        HKEY key = nullptr;
        DWORD disposition = 0;
        const LSTATUS status = RegCreateKeyExW(
            HKEY_CURRENT_USER, current.c_str(), 0, nullptr, REG_OPTION_VOLATILE,
            KEY_QUERY_VALUE | KEY_SET_VALUE | KEY_CREATE_SUB_KEY | KEY_WOW64_64KEY,
            nullptr, &key, &disposition);
        if (status != ERROR_SUCCESS) {
            Wh_Log(L"Anytime Upgrade: RegCreateKeyEx failed for %s (error=%ld)",
                   current.c_str(), status);
            return false;
        }
        RegCloseKey(key);
        if (disposition == REG_CREATED_NEW_KEY) {
            g_createdRegistryKeys.push_back(current);
        }
        if (next == std::wstring::npos) break;
        position = next + 1;
    }
    return true;
}

static bool SetRegistryString(const std::wstring& path, const wchar_t* valueName,
                              const std::wstring& value) {
    HKEY key = nullptr;
    const LSTATUS openStatus = RegOpenKeyExW(HKEY_CURRENT_USER, path.c_str(), 0,
                                             KEY_SET_VALUE | KEY_WOW64_64KEY, &key);
    if (openStatus != ERROR_SUCCESS) return false;
    const DWORD bytes = static_cast<DWORD>((value.size() + 1) * sizeof(wchar_t));
    const LSTATUS status = RegSetValueExW(key, valueName, 0, REG_SZ,
                                          reinterpret_cast<const BYTE*>(value.c_str()), bytes);
    RegCloseKey(key);
    if (status != ERROR_SUCCESS) {
        Wh_Log(L"Anytime Upgrade: RegSetValueEx failed for %s (error=%ld)",
               path.c_str(), status);
    }
    return status == ERROR_SUCCESS;
}

static bool SetRegistryDword(const std::wstring& path, const wchar_t* valueName,
                             DWORD value) {
    HKEY key = nullptr;
    const LSTATUS openStatus = RegOpenKeyExW(HKEY_CURRENT_USER, path.c_str(), 0,
                                             KEY_SET_VALUE | KEY_WOW64_64KEY, &key);
    if (openStatus != ERROR_SUCCESS) return false;
    const LSTATUS status = RegSetValueExW(key, valueName, 0, REG_DWORD,
                                          reinterpret_cast<const BYTE*>(&value),
                                          sizeof(value));
    RegCloseKey(key);
    if (status != ERROR_SUCCESS) {
        Wh_Log(L"Anytime Upgrade: RegSetValueEx (DWORD) failed for %s (error=%ld)",
               path.c_str(), status);
    }
    return status == ERROR_SUCCESS;
}

static void RemoveCreatedRegistryKeys() {
    // Child keys are longer than their parents, so deleting in descending path
    // length removes the tree safely while leaving pre-existing parent keys alone.
    std::sort(g_createdRegistryKeys.begin(), g_createdRegistryKeys.end(),
              [](const std::wstring& left, const std::wstring& right) {
                  return left.size() > right.size();
              });
    for (const auto& path : g_createdRegistryKeys) {
        const LSTATUS status = RegDeleteKeyExW(
            HKEY_CURRENT_USER, path.c_str(), KEY_WOW64_64KEY, 0);
        if (status != ERROR_SUCCESS && status != ERROR_FILE_NOT_FOUND &&
            status != ERROR_PATH_NOT_FOUND) {
            Wh_Log(L"Anytime Upgrade: could not remove temporary registry key %s (error=%ld)",
                   path.c_str(), status);
        }
    }
    g_createdRegistryKeys.clear();
}

static void NotifyControlPanel() {
    // Refresh only an already-open Control Panel folder. Do not broadcast
    // WM_SETTINGCHANGE or SHCNE_ASSOCCHANGED to every application in the session.
    PIDLIST_ABSOLUTE controlPanelPidl = nullptr;
    const HRESULT hr = SHGetSpecialFolderLocation(
        nullptr, CSIDL_CONTROLS, &controlPanelPidl);
    if (SUCCEEDED(hr) && controlPanelPidl) {
        SHChangeNotify(SHCNE_UPDATEDIR, SHCNF_IDLIST, controlPanelPidl, nullptr);
        ILFree(controlPanelPidl);
    } else {
        Wh_Log(L"Anytime Upgrade: could not resolve the Control Panel folder for refresh (error=0x%08X)",
               static_cast<unsigned>(hr));
    }
}

static bool RegisterControlPanelItem() {
    const std::wstring exePath = g_localizedDir + L"\\" + kExeName;
    const std::wstring namespaceKey =
        L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\ControlPanel\\NameSpace\\" +
        std::wstring(kClsid);
    const std::wstring clsidKey = L"Software\\Classes\\CLSID\\" + std::wstring(kClsid);
    const std::wstring iconKey = clsidKey + L"\\DefaultIcon";
    const std::wstring shellKey = clsidKey + L"\\Shell";
    const std::wstring openKey = shellKey + L"\\Open";
    const std::wstring commandKey = openKey + L"\\Command";

    // Never overwrite a per-user registration that does not belong to this mod.
    // A marker lets a later Explorer process recover cleanly after an interrupted unload.
    const std::wstring protectedKeys[] = {namespaceKey, clsidKey};
    for (const auto& path : protectedKeys) {
        if (KeyExists(HKEY_CURRENT_USER, path) && !IsOurRegistrationKey(path)) {
            Wh_Log(L"Anytime Upgrade: an existing per-user Control Panel registration was found; refusing to overwrite it");
            return false;
        }
    }

    for (const auto& path : protectedKeys) DeleteOwnedTree(path);
    g_createdRegistryKeys.clear();

    const std::wstring paths[] = {
        namespaceKey,
        clsidKey,
        iconKey,
        shellKey,
        openKey,
        commandKey,
    };
    for (const auto& path : paths) {
        if (!EnsureVolatilePath(path)) {
            RemoveCreatedRegistryKeys();
            return false;
        }
    }

    const std::wstring command = L"\"" + exePath + L"\"";
    const std::wstring icon = exePath + L",0";
    bool ok = true;
    ok = ok && SetRegistryString(namespaceKey, nullptr, kDisplayName);
    // The ControlPanel\NameSpace registration also carries its own numeric
    // category. Without it, Windows 7 can enumerate the namespace item before
    // the ordinary category entries, even though the CLSID says category 5.
    ok = ok && SetRegistryDword(namespaceKey, L"Category", 5);
    ok = ok && SetRegistryString(namespaceKey, kMarkerName, kMarkerValue);
    ok = ok && SetRegistryString(clsidKey, nullptr, kDisplayName);
    ok = ok && SetRegistryString(clsidKey, L"LocalizedString", kDisplayName);
    ok = ok && SetRegistryString(clsidKey, L"InfoTip", L"Open Windows Anytime Upgrade");
    ok = ok && SetRegistryString(clsidKey, L"System.ApplicationName", kCanonicalName);
    ok = ok && SetRegistryString(clsidKey, L"System.ControlPanel.Category", L"5");
    ok = ok && SetRegistryString(clsidKey, kMarkerName, kMarkerValue);
    ok = ok && SetRegistryString(iconKey, nullptr, icon);
    ok = ok && SetRegistryString(commandKey, nullptr, command);

    if (!ok) {
        RemoveCreatedRegistryKeys();
        return false;
    }

    g_registrationInstalled = true;
    Wh_Log(L"Anytime Upgrade: Control Panel registration installed at %s", exePath.c_str());
    NotifyControlPanel();
    return true;
}

static void SetupWorker() {
    try {
        if (!EnsurePayload() || g_stopping.load()) {
            if (!g_stopping.load()) {
                Wh_Log(L"Anytime Upgrade: setup failed; the Control Panel entry was not registered");
            }
            return;
        }
        if (g_stopping.load()) return;
        if (!RegisterControlPanelItem() && !g_stopping.load()) {
            Wh_Log(L"Anytime Upgrade: could not register the Control Panel item");
        }
    } catch (...) {
        Wh_Log(L"Anytime Upgrade: unexpected setup error");
        RemoveCreatedRegistryKeys();
    }
}

// The EXE and DirectUI DLL can also request strings through User32's
// LoadStringW API. The resource blocks above are the primary source; this hook
// is a fallback for code paths that still ask the original module for its MUI.
using LoadStringW_t = int(WINAPI*)(HINSTANCE, UINT, LPWSTR, int);
static LoadStringW_t g_loadStringWOriginal = nullptr;
static HMODULE g_exeResourceModule = nullptr;
static HMODULE g_duiResourceModule = nullptr;
static HMODULE g_duiRuntimeModule = nullptr;

static const wchar_t* FindAnytimeUpgradeString(UINT id) {
    for (size_t i = 0; i < g_stringCount; ++i) {
        if (g_strings[i].id == id) return g_strings[i].text;
    }
    return nullptr;
}

static bool IsAnytimeUpgradeResourceModule(HINSTANCE instance) {
    if (!instance) return false;
    const ULONG_PTR raw = reinterpret_cast<ULONG_PTR>(instance);
    HMODULE module = reinterpret_cast<HMODULE>(raw & ~static_cast<ULONG_PTR>(3));
    if (module == GetModuleHandleW(nullptr) ||
        module == g_exeResourceModule || module == g_duiResourceModule) return true;

    wchar_t path[MAX_PATH] = {};
    const DWORD length = GetModuleFileNameW(module, path, ARRAYSIZE(path));
    if (!length || length >= ARRAYSIZE(path)) return false;
    const wchar_t* fileName = wcsrchr(path, L'\\');
    fileName = fileName ? fileName + 1 : path;
    return _wcsicmp(fileName, kExeName) == 0 ||
           _wcsicmp(fileName, kDllName) == 0;
}

static int CopyAnytimeUpgradeString(const wchar_t* text, LPWSTR buffer,
                                    int bufferChars) {
    if (!text) return 0;
    const int length = static_cast<int>(wcslen(text));
    if (bufferChars == 0) {
        if (!buffer) return 0;
        *reinterpret_cast<LPCWSTR*>(buffer) = text;
        return length;
    }
    if (!buffer || bufferChars < 1) return 0;
    const int copied = (std::min)(length, bufferChars - 1);
    if (copied) memcpy(buffer, text, static_cast<size_t>(copied) * sizeof(wchar_t));
    buffer[copied] = L'\0';
    return copied;
}

static int WINAPI LoadStringWHook(HINSTANCE instance, UINT id,
                                  LPWSTR buffer, int bufferChars) {
    if (IsAnytimeUpgradeResourceModule(instance)) {
        if (const wchar_t* text = FindAnytimeUpgradeString(id)) {
            return CopyAnytimeUpgradeString(text, buffer, bufferChars);
        }
    }
    return g_loadStringWOriginal(instance, id, buffer, bufferChars);
}

// DirectUI's resstr(...) path reads string resources through
// XResourceProvider::Create instead of calling LoadStringW. Give that provider
// a data-file mapping of the patched EXE/DLL, so it sees our embedded RT_STRING
// blocks even on systems where the normal MUI lookup would otherwise win.
using XResourceProviderCreate_t = HRESULT(*)(HINSTANCE, LPCWSTR, LPCWSTR,
                                             LPCWSTR, void**);
static XResourceProviderCreate_t g_xResourceProviderCreateOriginal = nullptr;

static bool IsModuleNamed(HINSTANCE instance, const wchar_t* expectedName) {
    if (!instance || !expectedName) return false;
    const ULONG_PTR raw = reinterpret_cast<ULONG_PTR>(instance);
    HMODULE module = reinterpret_cast<HMODULE>(raw & ~static_cast<ULONG_PTR>(3));
    wchar_t path[MAX_PATH] = {};
    const DWORD length = GetModuleFileNameW(module, path, ARRAYSIZE(path));
    if (!length || length >= ARRAYSIZE(path)) return false;
    const wchar_t* fileName = wcsrchr(path, L'\\');
    fileName = fileName ? fileName + 1 : path;
    return _wcsicmp(fileName, expectedName) == 0;
}

static HRESULT XResourceProviderCreateHook(HINSTANCE instance,
                                           LPCWSTR resourceName,
                                           LPCWSTR resourceType,
                                           LPCWSTR stylesheetName,
                                           void** provider) {
    HINSTANCE resourceInstance = instance;
    if (g_exeResourceModule &&
        (instance == GetModuleHandleW(nullptr) ||
         IsModuleNamed(instance, kExeName))) {
        resourceInstance = reinterpret_cast<HINSTANCE>(g_exeResourceModule);
    } else if (g_duiResourceModule && IsModuleNamed(instance, kDllName)) {
        resourceInstance = reinterpret_cast<HINSTANCE>(g_duiResourceModule);
    }
    return g_xResourceProviderCreateOriginal(resourceInstance, resourceName,
                                              resourceType, stylesheetName,
                                              provider);
}

static void InstallDirectUIResourceHook() {
    wchar_t exePath[MAX_PATH] = {};
    const DWORD exeLength = GetModuleFileNameW(nullptr, exePath, ARRAYSIZE(exePath));
    if (!exeLength || exeLength >= ARRAYSIZE(exePath)) {
        Wh_Log(L"Anytime Upgrade: could not resolve the localized EXE path for DirectUI resources");
        return;
    }

    g_exeResourceModule = LoadLibraryExW(
        exePath, nullptr, LOAD_LIBRARY_AS_DATAFILE | LOAD_LIBRARY_AS_IMAGE_RESOURCE);

    wchar_t* slash = wcsrchr(exePath, L'\\');
    if (!slash) return;
    *(slash + 1) = L'\0';
    std::wstring dllPath = std::wstring(exePath) + kDllName;
    g_duiResourceModule = LoadLibraryExW(
        dllPath.c_str(), nullptr, LOAD_LIBRARY_AS_DATAFILE | LOAD_LIBRARY_AS_IMAGE_RESOURCE);

    // The UI normally imports dui71.dll, so it should already be loaded by the
    // time Windhawk initializes this process. If not, load the adjacent copy so
    // the DirectUI provider entry point can still be hooked before the UI opens.
    g_duiRuntimeModule = GetModuleHandleW(kDllName);
    if (!g_duiRuntimeModule) g_duiRuntimeModule = LoadLibraryW(dllPath.c_str());
    if (!g_duiRuntimeModule) {
        Wh_Log(L"Anytime Upgrade: dui71.dll runtime module was not available for DirectUI hook");
        return;
    }

    FARPROC create = GetProcAddress(g_duiRuntimeModule,
        "?Create@XResourceProvider@DirectUI@@SAJPEAUHINSTANCE__@@PEBG11PEAPEAV12@@Z");
    if (!create) {
        Wh_Log(L"Anytime Upgrade: DirectUI XResourceProvider::Create export was not found in dui71.dll");
        return;
    }
    if (!WindhawkUtils::SetFunctionHook(
            reinterpret_cast<XResourceProviderCreate_t>(create),
            XResourceProviderCreateHook, &g_xResourceProviderCreateOriginal)) {
        Wh_Log(L"Anytime Upgrade: could not install DirectUI resource hook");
        return;
    }
    Wh_Log(L"Anytime Upgrade: DirectUI provider now uses the embedded English string resources");
}

static void InstallAnytimeUpgradeStringHook() {
    HMODULE user32 = GetModuleHandleW(L"user32.dll");
    if (!user32) {
        user32 = LoadLibraryExW(L"user32.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    }
    if (!user32) {
        Wh_Log(L"Anytime Upgrade: could not load User32 for the string fallback hook");
        return;
    }
    FARPROC address = GetProcAddress(user32, "LoadStringW");
    if (!address) {
        Wh_Log(L"Anytime Upgrade: User32!LoadStringW was not found");
        return;
    }
    if (WindhawkUtils::SetFunctionHook(reinterpret_cast<LoadStringW_t>(address),
                                       LoadStringWHook, &g_loadStringWOriginal)) {
        Wh_Log(L"Anytime Upgrade: embedded string fallback hook installed");
    } else {
        Wh_Log(L"Anytime Upgrade: could not install the LoadStringW fallback hook");
    }
}

// DUI70 does not call LoadStringW: it reads RT_STRING blocks itself through
// FindResourceExW/FindResourceW + LoadResource, using the thread UI language.
// If that exact language is absent from the module, retry with neutral and
// en-US so the embedded English strings are always found.
using FindResourceExW_t = HRSRC(WINAPI*)(HMODULE, LPCWSTR, LPCWSTR, WORD);
using FindResourceW_t = HRSRC(WINAPI*)(HMODULE, LPCWSTR, LPCWSTR);
static FindResourceExW_t g_findResourceExWOriginal = nullptr;
static FindResourceW_t g_findResourceWOriginal = nullptr;

static HRSRC WINAPI FindResourceExWHook(HMODULE module, LPCWSTR type, LPCWSTR name, WORD language) {
    HRSRC result = g_findResourceExWOriginal(module, type, name, language);
    if (!result && type == RT_STRING && module &&
        IsAnytimeUpgradeResourceModule(reinterpret_cast<HINSTANCE>(module))) {
        result = g_findResourceExWOriginal(module, type, name,
                                           MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL));
        if (!result) result = g_findResourceExWOriginal(module, type, name, kResourceLanguage);
    }
    return result;
}

static HRSRC WINAPI FindResourceWHook(HMODULE module, LPCWSTR name, LPCWSTR type) {
    HRSRC result = g_findResourceWOriginal(module, name, type);
    if (!result && type == RT_STRING && module &&
        IsAnytimeUpgradeResourceModule(reinterpret_cast<HINSTANCE>(module))) {
        result = g_findResourceExWOriginal
            ? g_findResourceExWOriginal(module, type, name, kResourceLanguage)
            : nullptr;
    }
    return result;
}

static void InstallResourceFallbackHooks() {
    HMODULE kernel32 = GetModuleHandleW(L"kernel32.dll");
    if (!kernel32) return;
    FARPROC exAddress = GetProcAddress(kernel32, "FindResourceExW");
    FARPROC plainAddress = GetProcAddress(kernel32, "FindResourceW");
    if (exAddress && WindhawkUtils::SetFunctionHook(
            reinterpret_cast<FindResourceExW_t>(exAddress), FindResourceExWHook,
            &g_findResourceExWOriginal)) {
        Wh_Log(L"Anytime Upgrade: FindResourceExW fallback hook installed");
    } else {
        Wh_Log(L"Anytime Upgrade: could not hook FindResourceExW");
    }
    if (plainAddress && WindhawkUtils::SetFunctionHook(
            reinterpret_cast<FindResourceW_t>(plainAddress), FindResourceWHook,
            &g_findResourceWOriginal)) {
        Wh_Log(L"Anytime Upgrade: FindResourceW fallback hook installed");
    }
}

} // namespace

// The UI executable is still injected separately so its resource hooks remain
// local to WindowsAnytimeUpgradeui.exe. All downloading and registry work runs
// in the dedicated Windhawk tool process instead of Explorer.
bool g_isAnytimeUpgradeUiProcess = false;
bool g_isToolModProcessLauncher = false;
HANDLE g_toolModProcessMutex = nullptr;

void WINAPI EntryPoint_Hook() {
    Wh_Log(L">");
    ExitThread(0);
}

BOOL WhTool_ModInit() {
    g_stopping.store(false);
    g_storageDir = GetStorageDirectory();
    if (g_storageDir.empty()) {
        Wh_Log(L"Anytime Upgrade: Windhawk storage path is unavailable");
        return FALSE;
    }

    try {
        g_worker.emplace(SetupWorker);
    } catch (...) {
        Wh_Log(L"Anytime Upgrade: could not start the setup worker");
        return FALSE;
    }
    return TRUE;
}

void WhTool_ModSettingsChanged() {
}

void WhTool_ModUninit() {
    g_stopping.store(true);
    CancelActiveRequest();
    if (g_worker && g_worker->joinable()) {
        g_worker->join();
    }
    g_worker.reset();

    if (g_registrationInstalled) {
        RemoveCreatedRegistryKeys();
        g_registrationInstalled = false;
        NotifyControlPanel();
        Wh_Log(L"Anytime Upgrade: Control Panel registration removed");
    } else if (!g_createdRegistryKeys.empty()) {
        RemoveCreatedRegistryKeys();
    }
}

BOOL Wh_ModInit() {
    // The application process only installs its resource hooks.
    if (IsAnytimeUpgradeUiProcess()) {
        g_isAnytimeUpgradeUiProcess = true;
        SelectStringTable();
        InstallDirectUIResourceHook();
        InstallAnytimeUpgradeStringHook();
        InstallResourceFallbackHooks();
        return TRUE;
    }

    // Windhawk tool-mod launcher implementation (Windhawk 1.7.3 compatible).
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

    WCHAR commandLine[MAX_PATH + 2 +
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
    if (g_isToolModProcessLauncher || g_isAnytimeUpgradeUiProcess) {
        return;
    }

    WhTool_ModSettingsChanged();
}

void Wh_ModUninit() {
    if (g_isToolModProcessLauncher || g_isAnytimeUpgradeUiProcess) {
        return;
    }

    WhTool_ModUninit();
    ExitProcess(0);
}
