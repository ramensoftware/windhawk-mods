// ==WindhawkMod==
// @id              vscode-tweaker
// @name            VSCode Tweaker
// @description     Tweak Microsoft Visual Studio Code by injecting custom JavaScript and CSS code
// @version         1.0.1
// @author          m417z
// @github          https://github.com/m417z
// @twitter         https://twitter.com/m417z
// @homepage        https://m417z.com/
// @include         code.exe
// @compilerOptions -lshlwapi
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# VSCode Tweaker

Tweak Microsoft Visual Studio Code by injecting custom JavaScript and CSS code.

VSCode has to be restarted for the changes to apply.

No files are modified on disk - to revert all changes, disable the mod and restart VSCode.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- CodeSnippets:
  - - Type: js
      $name: Snippet type
      $options:
      - js: JavaScript
      - css: CSS
      - electron_main_js: Electron main.js
      - electron_workbench_html: Electron workbench.html
      - electron_workbench_js: Electron workbench.js
    - Source: inline
      $name: Snippet source
      $options:
      - inline: Inline code
      - file: Code from a file
      - inline_replace: 'Inline code regex replace (syntax: search=>replace)'
    - Code: >-
        setInterval(()=>{
        var t='WINDHAWK!',
        x=document.querySelector('.window-title'),
        y='textContent';
        x[y]=x[y].replace(/( \| .)?$/,
        ' | '+t[Math.floor(Date.now()/1000)%t.length])},1000)
      $name: The snippet code
      $description: For code from a file, enter the source file path
  - - Type: css
    - Source: inline
    - Code: >-
        .windows {font-family: "Comic Sans MS" !important;}
  $name: Code snippets
*/
// ==/WindhawkModSettings==

#include <shlwapi.h>

#include <algorithm>
#include <fstream>
#include <regex>
#include <sstream>
#include <string>

enum VSCODE_FILE {
    // Configurable with the mod.
    VSCODE_FILE_JS,
    VSCODE_FILE_CSS,
    VSCODE_FILE_ELECTRON_MAIN_JS,
    VSCODE_FILE_ELECTRON_WORKBENCH_HTML,
    VSCODE_FILE_ELECTRON_WORKBENCH_JS,

    // Contains file hashes.
    VSCODE_FILE_PRODUCT_JSON,

    VSCODE_FILE_COUNT,
};

PCWSTR g_vscodeFileTypes[] = {
    L"js",
    L"css",
    L"electron_main_js",
    L"electron_workbench_html",
    L"electron_workbench_js",
};

static_assert(ARRAYSIZE(g_vscodeFileTypes) == VSCODE_FILE_COUNT - 1);

PCWSTR g_vscodeFilePaths[] = {
    L"resources\\app\\out\\vs\\workbench\\workbench.desktop.main.js",
    L"resources\\app\\out\\vs\\workbench\\workbench.desktop.main.css",
    L"resources\\app\\out\\main.js",
    L"resources\\app\\out\\vs\\code\\electron-browser\\workbench\\workbench.html",
    L"resources\\app\\out\\vs\\code\\electron-browser\\workbench\\workbench.js",
    L"resources\\app\\product.json",
};

static_assert(ARRAYSIZE(g_vscodeFilePaths) == VSCODE_FILE_COUNT);

struct {
    WCHAR filePath[MAX_PATH];
    WCHAR newFilePath[MAX_PATH];
    std::string newFileHash;
} g_vscodeFiles[VSCODE_FILE_COUNT];

// https://gist.github.com/tomykaira/f0fd86b6c73063283afe550bc5d77594
std::string Base64Encode(const BYTE* data, size_t in_len)
{
    static constexpr char sEncodingTable[] = {
        'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M',
        'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
        'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm',
        'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
        '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', '/'};

    size_t out_len = (4 * in_len + 2) / 3;
    std::string ret(out_len, '\0');
    size_t i;
    char *p = ret.data();

    for (i = 0; i < in_len - 2; i += 3) {
        *p++ = sEncodingTable[(data[i] >> 2) & 0x3F];
        *p++ = sEncodingTable[((data[i] & 0x3) << 4) |
                              ((int)(data[i + 1] & 0xF0) >> 4)];
        *p++ = sEncodingTable[((data[i + 1] & 0xF) << 2) |
                              ((int)(data[i + 2] & 0xC0) >> 6)];
        *p++ = sEncodingTable[data[i + 2] & 0x3F];
    }
    if (i < in_len) {
        *p++ = sEncodingTable[(data[i] >> 2) & 0x3F];
        if (i == (in_len - 1)) {
            *p++ = sEncodingTable[((data[i] & 0x3) << 4)];
            // *p++ = '=';
        }
        else {
            *p++ = sEncodingTable[((data[i] & 0x3) << 4) |
                                  ((int)(data[i + 1] & 0xF0) >> 4)];
            *p++ = sEncodingTable[((data[i + 1] & 0xF) << 2)];
        }
        // *p++ = '=';
    }

    return ret;
}

// Calculates base64(md5(contents))
// Based on: https://github.com/DownWithUp/SHA-ME
// VSCode reference:
// computeChecksum in build\gulpfile.vscode.js
std::string FileHash(PCWSTR lpszFile)
{
    HCRYPTPROV	hProv;
    HCRYPTHASH	hHash;
    HANDLE		hFile;
    DWORD		dwBytesRead;
    BYTE		bReadFile[0x512];
    BYTE		bHash[16];
    std::string ret;

    hFile = CreateFile(lpszFile, FILE_READ_ACCESS, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
        return ret;
    }
    if (!CryptAcquireContext(&hProv, NULL, NULL, PROV_RSA_AES, CRYPT_VERIFYCONTEXT)) {
        CloseHandle(hFile);
        return ret;
    }
    if (!CryptCreateHash(hProv, CALG_MD5, 0, 0, &hHash)) {
        CloseHandle(hFile);
        CryptReleaseContext(hProv, 0);
        return ret;
    }
    while (ReadFile(hFile, bReadFile, sizeof(bReadFile), &dwBytesRead, NULL)) {
        if (dwBytesRead == 0) {
            break; // End of file
        }
        CryptHashData(hHash, bReadFile, dwBytesRead, 0);
    }
    dwBytesRead = sizeof(bHash); // Repurpose variable
    if (CryptGetHashParam(hHash, HP_HASHVAL, bHash, &dwBytesRead, 0)) {
        ret = Base64Encode(bHash, sizeof(bHash));
    }
    CryptReleaseContext(hProv, 0);
    CryptDestroyHash(hHash);
    CloseHandle(hFile);
    return ret;
}

using CreateFileW_t = decltype(&CreateFileW);
CreateFileW_t pOriginalCreateFileW;
HANDLE WINAPI CreateFileWHook(
    LPCWSTR lpFileName,
    DWORD dwDesiredAccess,
    DWORD dwShareMode,
    LPSECURITY_ATTRIBUTES lpSecurityAttributes,
    DWORD dwCreationDisposition,
    DWORD dwFlagsAndAttributes,
    HANDLE hTemplateFile)
{
    PCWSTR compareFileName = lpFileName;
    if (compareFileName[0] == '\\' &&
        compareFileName[1] == '\\' &&
        compareFileName[2] == '?' &&
        compareFileName[3] == '\\') {
        compareFileName += 4;
    }

    for (size_t i = 0; i < VSCODE_FILE_COUNT; i++) {
        if (wcsicmp(compareFileName, g_vscodeFiles[i].filePath) == 0) {
            Wh_Log(L"lpFileName = %s", compareFileName);
            Wh_Log(L"=>");
            Wh_Log(L"lpFileName = %s", g_vscodeFiles[i].newFilePath);
            lpFileName = g_vscodeFiles[i].newFilePath;
            break;
        }
    }

    return pOriginalCreateFileW(
        lpFileName,
        dwDesiredAccess,
        dwShareMode,
        lpSecurityAttributes,
        dwCreationDisposition,
        dwFlagsAndAttributes,
        hTemplateFile
    );
}

using GetFileAttributesExW_t = decltype(&GetFileAttributesExW);
GetFileAttributesExW_t pOriginalGetFileAttributesExW;
BOOL WINAPI GetFileAttributesExWHook(LPCWSTR lpFileName, GET_FILEEX_INFO_LEVELS fInfoLevelId, LPVOID lpFileInformation)
{
    PCWSTR compareFileName = lpFileName;
    if (compareFileName[0] == '\\' &&
        compareFileName[1] == '\\' &&
        compareFileName[2] == '?' &&
        compareFileName[3] == '\\') {
        compareFileName += 4;
    }

    for (size_t i = 0; i < VSCODE_FILE_COUNT; i++) {
        if (wcsicmp(compareFileName, g_vscodeFiles[i].filePath) == 0) {
            Wh_Log(L"lpFileName = %s", compareFileName);
            Wh_Log(L"=>");
            Wh_Log(L"lpFileName = %s", g_vscodeFiles[i].newFilePath);
            lpFileName = g_vscodeFiles[i].newFilePath;
            break;
        }
    }

    return pOriginalGetFileAttributesExW(
        lpFileName, fInfoLevelId, lpFileInformation
    );
}

void GetModTempPath(WCHAR tempPath[MAX_PATH])
{
    GetTempPath(MAX_PATH, tempPath);
    PathAppend(tempPath, L"vscode-tweaker");
    CreateDirectory(tempPath, nullptr);
}

void GetInitialTempFileName(WCHAR tempFileName[MAX_PATH])
{
    WCHAR tempPath[MAX_PATH];
    GetModTempPath(tempPath);
    GetTempFileName(tempPath, L"vst", 0, tempFileName);
}

void RenameToFinalTempFileName(WCHAR tempFileName[MAX_PATH], const std::string& fileHash, WCHAR finalTempFileName[MAX_PATH])
{
    std::wstring fileName = std::wstring(fileHash.begin(), fileHash.end());
    std::replace(fileName.begin(), fileName.end(), L'+', L'-');
    std::replace(fileName.begin(), fileName.end(), L'/', L'_');

    GetModTempPath(finalTempFileName);
    PathAppend(finalTempFileName, fileName.c_str());

    if (GetFileAttributes(finalTempFileName) == INVALID_FILE_ATTRIBUTES) {
        MoveFile(tempFileName, finalTempFileName);
    }
    else {
        // Already exists, delete.
        DeleteFile(tempFileName);
    }
}

// https://stackoverflow.com/a/69410299
std::string wide_string_to_string(PCWSTR wide_string)
{
    if (!*wide_string) {
        return "";
    }

    int size_needed = WideCharToMultiByte(CP_UTF8, 0, wide_string, -1, nullptr, 0, nullptr, nullptr);
    if (size_needed <= 0) {
        throw std::runtime_error("WideCharToMultiByte() failed: " + std::to_string(size_needed));
    }

    size_needed--; // no need for the NULL terminator

    std::string result(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, wide_string, size_needed, result.data(), size_needed, nullptr, nullptr);
    return result;
}

std::string MakeReplacementsInCode(std::string code, PCWSTR fileType)
{
    for (int i = 0; ; i++) {
        PCWSTR type = Wh_GetStringSetting(L"CodeSnippets[%d].Type", i);
        bool done = !*type;
        bool typeMatch = !done && wcscmp(type, fileType) == 0;
        Wh_FreeStringSetting(type);

        if (done) {
            break;
        }

        if (!typeMatch) {
            continue;
        }

        PCWSTR source = Wh_GetStringSetting(L"CodeSnippets[%d].Source", i);
        bool isInlineReplace = wcscmp(source, L"inline_replace") == 0;
        Wh_FreeStringSetting(source);

        if (!isInlineReplace) {
            continue;
        }

        PCWSTR searchReplaceCode = Wh_GetStringSetting(L"CodeSnippets[%d].Code", i);
        std::string searchReplace = wide_string_to_string(searchReplaceCode);
        Wh_FreeStringSetting(searchReplaceCode);

        auto splitPos = searchReplace.find("=>");
        if (splitPos == std::string::npos) {
            continue;
        }

        std::string search = searchReplace.substr(0, splitPos);
        std::string replace = searchReplace.substr(splitPos + 2);

        std::regex regex(search);
        code = std::regex_replace(code, regex, replace);
    }

    return code;
}

void AppendModCodeToStream(std::ofstream& output, PCWSTR typeToAppend)
{
    for (int i = 0; ; i++) {
        PCWSTR type = Wh_GetStringSetting(L"CodeSnippets[%d].Type", i);
        bool done = !*type;
        bool typeMatch = !done && wcscmp(type, typeToAppend) == 0;
        Wh_FreeStringSetting(type);

        if (done) {
            break;
        }

        if (!typeMatch) {
            continue;
        }

        PCWSTR source = Wh_GetStringSetting(L"CodeSnippets[%d].Source", i);
        bool fromFile = wcscmp(source, L"file") == 0;
        bool fromInlineCode = wcscmp(source, L"inline") == 0;
        Wh_FreeStringSetting(source);

        PCWSTR code = Wh_GetStringSetting(L"CodeSnippets[%d].Code", i);

        output << '\n';

        if (fromFile) {
            std::ifstream input(code);
            output << input.rdbuf();
        }
        else if (fromInlineCode) {
            output << wide_string_to_string(code);
        }

        Wh_FreeStringSetting(code);
    }
}

std::string CreateNewVscodeFile(PCWSTR fileType, WCHAR sourceFilePath[MAX_PATH], WCHAR targetFilePath[MAX_PATH])
{
    std::stringstream buffer;
    {
        std::ifstream input(sourceFilePath);
        buffer << input.rdbuf();
    }

    WCHAR tempFilePath[MAX_PATH];
    GetInitialTempFileName(tempFilePath);

    {
        std::ofstream output(tempFilePath);
        output << MakeReplacementsInCode(buffer.str(), fileType);
        AppendModCodeToStream(output, fileType);
    }

    std::string fileHash = FileHash(tempFilePath);
    RenameToFinalTempFileName(tempFilePath, fileHash, targetFilePath);

    return fileHash;
}

std::string CreateNewProductFile(WCHAR sourceFilePath[MAX_PATH], WCHAR targetFilePath[MAX_PATH])
{
    std::stringstream buffer;
    {
        std::ifstream input(sourceFilePath);
        buffer << input.rdbuf();
    }

    struct {
        std::string regexPath;
        std::string& newHash;
    } hashItems[] = {
        {
            R"(vs/workbench/workbench\.desktop\.main\.js)",
            g_vscodeFiles[VSCODE_FILE_JS].newFileHash
        },
        {
            R"(vs/workbench/workbench\.desktop\.main\.css)",
            g_vscodeFiles[VSCODE_FILE_CSS].newFileHash
        },
        {
            R"(vs/code/electron-browser/workbench/workbench\.html)",
            g_vscodeFiles[VSCODE_FILE_ELECTRON_WORKBENCH_HTML].newFileHash
        },
        {
            R"(vs/code/electron-browser/workbench/workbench\.js)",
            g_vscodeFiles[VSCODE_FILE_ELECTRON_WORKBENCH_JS].newFileHash
        },
    };

    std::string newContent = buffer.str();
    for (const auto& item : hashItems) {
        std::regex regex(R"((")" + item.regexPath + R"("\s*:\s*")[A-Za-z0-9+/]+("))");
        newContent = std::regex_replace(newContent, regex, "$01" + item.newHash + "$02");
    }

    WCHAR tempFilePath[MAX_PATH];
    GetInitialTempFileName(tempFilePath);

    {
        std::ofstream output(tempFilePath);
        output << newContent;
    }

    std::string fileHash = FileHash(tempFilePath);
    RenameToFinalTempFileName(tempFilePath, fileHash, targetFilePath);

    return fileHash;
}

BOOL Wh_ModInit(void)
{
    Wh_Log(L"Init");

    WCHAR modulePath[MAX_PATH];
    GetModuleFileName(nullptr, modulePath, ARRAYSIZE(modulePath));
    PathRemoveFileSpec(modulePath);

    for (size_t i = 0; i < VSCODE_FILE_COUNT; i++) {
        PathCombine(g_vscodeFiles[i].filePath, modulePath, g_vscodeFilePaths[i]);

        if (i != VSCODE_FILE_PRODUCT_JSON) {
            g_vscodeFiles[i].newFileHash = CreateNewVscodeFile(
                g_vscodeFileTypes[i], g_vscodeFiles[i].filePath, g_vscodeFiles[i].newFilePath);
        }
    }

    CreateNewProductFile(
        g_vscodeFiles[VSCODE_FILE_PRODUCT_JSON].filePath,
        g_vscodeFiles[VSCODE_FILE_PRODUCT_JSON].newFilePath);

    Wh_SetFunctionHook((void*)CreateFileW, (void*)CreateFileWHook, (void**)&pOriginalCreateFileW);
    Wh_SetFunctionHook((void*)GetFileAttributesExW, (void*)GetFileAttributesExWHook, (void**)&pOriginalGetFileAttributesExW);

    return TRUE;
}
