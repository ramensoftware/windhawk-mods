// ==WindhawkMod==
// @id              safediscshim
// @name            SafeDiscShim
// @description     Run SafeDisc protected games on Windows 10 and above.
// @version         1.0
// @author          Rib
// @github          https://github.com/RibShark
// @include         *
// @architecture    x86
// ==/WindhawkMod==

// Source code is published under The GNU General Public License v3.0.

// ==WindhawkModReadme==
/*
# IMPORTANT
Windhawk needs to hook into `svchost.exe` for SafeDiscShim to work on certain games.
Please navigate to Windhawk's Settings, Advanced settings, More advanced settings, and make sure that
`svchost.exe` is in the Process inclusion list.

# SafeDiscShim
SafeDiscShim is a compatibility tool that allows for SafeDisc protected games which utilize the insecure Macrovision
Security Driver ("secdrv.sys") to run on modern versions of Windows which have said driver blacklisted. Previous methods
to restore functionality to these games relied on forcefully installing the driver, potentially opening security risks.

In contrast, this tool does not rely on any drivers to function. Instead, it automatically loads alongside SafeDisc
protected games and intercepts any communication requests that would have been sent to the driver, instead sending the 
expected response itself and allowing the game to boot.

# Disclaimer
SafeDiscShim is purely designed as a compatibility tool: no security mechanisms are bypassed in the operation of this 
tool and SafeDisc protected games still require their original discs in order to function, even when using this tool.
Certain games may have additional compatibility issues outside of the SafeDisc protection; this tool makes no attempt to
fix such issues.
*/
// ==/WindhawkModReadme==

#include <windhawk_utils.h>
#include <string>

enum SafeDiscCommand : DWORD {
  GetDebugRegisterInfo = 0x3c,
  GetIdtInfo = 0x3d,
  SetupVerification = 0x3e,
  /* commands below this point are implemented in driver versions with function names stripped */
  Command3Fh = 0x3f,
  Command40h = 0x40,
  Command41h = 0x41,
  Command42h = 0x42,
  Command43h = 0x43
};

typedef struct SecDrvIoctlInBuffer {
  DWORD VersionMajor;
  DWORD VersionMinor;
  DWORD VersionPatch;

  SafeDiscCommand Command;
  DWORD VerificationData[0x100];

  DWORD ExtraDataSize;
  DWORD ExtraData[0x40];
} SecDrvIoctlInBuffer;

typedef struct SecDrvIoctlOutBuffer {
  DWORD VersionMajor;
  DWORD VersionMinor;
  DWORD VersionPatch;

  DWORD VerificationData[0x100];

  DWORD ExtraDataSize;
  DWORD ExtraData[0x80];
} SecDrvIoctlOutBuffer;

void BuildVerificationData(DWORD verificationData[0x100]) {
  DWORD curValue = 0xF367AC7F;

  /* TODO: this is hacky, see if there are any better ways to get the kernel
   * tick count */
  verificationData[0] = *reinterpret_cast<int*>(0x7FFE0320);

  for ( int i = 3; i > 0; --i ) {
    curValue = 0x361962E9 - 0xD5ACB1B * curValue;
    verificationData[i] = curValue;
    verificationData[0] ^= curValue;
  }
}

BOOL ProcessSecDrvIoctl(PVOID pInBuffer,
                        DWORD dwInBufferLength,
                        PVOID pOutBuffer,
                        DWORD dwOutBufferLength) {
    if (!pInBuffer || !pOutBuffer) {
        Wh_Log(L"SafeDiscShim error: invalid ioctl buffers - pInBuffer = 0x%X, pOutBuffer = 0x%X", pInBuffer, pOutBuffer);
        return FALSE;
    }

    if (dwInBufferLength != sizeof(SecDrvIoctlInBuffer)) {
        Wh_Log(L"SafeDiscShim error: invalid ioctl in-buffer length - 0x%X", dwInBufferLength);
        return FALSE;
    }

    /* later versions report a buffer size of 0xC18 for some reason, though it is never read from or written to outside of the normal size */
    if ( dwOutBufferLength != sizeof(SecDrvIoctlOutBuffer) && dwOutBufferLength != 0xC18 ) {
        Wh_Log(L"invalid ioctl out-buffer size: 0x%X}", dwOutBufferLength);
        return FALSE;
    }

    auto* pSecDrvInBuffer = static_cast<SecDrvIoctlInBuffer *>(pInBuffer);
    auto* pSecDrvOutBuffer = static_cast<SecDrvIoctlOutBuffer *>(pOutBuffer);

    // match latest secdrv version
    pSecDrvOutBuffer->VersionMajor = 4;
    pSecDrvOutBuffer->VersionMinor = 3;
    pSecDrvOutBuffer->VersionPatch = 86;

    /* return expected values for each command. note that the latest driver version is hardcoded to return these values; earlier driver versions would perform more checks */
    switch (pSecDrvInBuffer->Command) {
        case GetDebugRegisterInfo:
            pSecDrvOutBuffer->ExtraDataSize = 4;
            pSecDrvOutBuffer->ExtraData[0] = 0x400;
            break;
        case GetIdtInfo:
            pSecDrvOutBuffer->ExtraDataSize = 4;
            pSecDrvOutBuffer->ExtraData[0] = 0x2C8;
            break;
        case SetupVerification:
            pSecDrvOutBuffer->ExtraDataSize = 4;
            pSecDrvOutBuffer->ExtraData[0] = 0x5278D11B;
            break;
        case Command3Fh:
            if ( dwOutBufferLength != 0xC18 || pSecDrvInBuffer->ExtraData[0] > 0x60 ) return FALSE;
            pSecDrvOutBuffer->ExtraDataSize = 4;
            pSecDrvOutBuffer->ExtraData[0] = 0;
            break;
        case Command40h:
            if ( dwOutBufferLength != 0xC18 || !pSecDrvInBuffer->ExtraData[0] || !pSecDrvInBuffer->ExtraData[1] ) return FALSE;
            pSecDrvOutBuffer->ExtraDataSize = 4;
            if ( pSecDrvInBuffer->ExtraData[1] <= 0x80 ) {
                pSecDrvOutBuffer->ExtraData[0] = 0x56791283;
            }
            else {
                pSecDrvOutBuffer->ExtraData[0] = 0x587C1284;
            }
            break;
        case Command41h:
            if ( dwOutBufferLength != 0xC18 || !LOBYTE(pSecDrvInBuffer->ExtraData[0]) ) return FALSE;
            pSecDrvOutBuffer->ExtraDataSize = 4;
            break;
        case Command42h:
            return FALSE;
        case Command43h:
            if ( pSecDrvInBuffer->ExtraData[0] != 0x98A64100 || pSecDrvInBuffer->ExtraData[1] > 7 || pSecDrvInBuffer->ExtraData[1] == 4 ) return FALSE;
            pSecDrvOutBuffer->ExtraDataSize = 4;
            pSecDrvOutBuffer->ExtraData[0] = 0;
            break;
        default:
            Wh_Log(L"SafeDiscShim error: unhandled ioctl command: 0x%X}", static_cast<DWORD>(pSecDrvInBuffer->Command));
            return FALSE;
    }
    
    BuildVerificationData(pSecDrvOutBuffer->VerificationData);
    return TRUE;
}

using DeviceIoControl_t = decltype(&DeviceIoControl);
DeviceIoControl_t DeviceIoControl_Orig;
BOOL WINAPI DeviceIoControl_Hook(HANDLE hDevice,
                                    DWORD dwIoControlCode,
                                    LPVOID lpInBuffer,
                                    DWORD nInBufferSize,
                                    LPVOID lpOutBuffer,
                                    DWORD nOutBufferSize,
                                    LPDWORD lpBytesReturned,
                                    LPOVERLAPPED lpOverlapped) {
    // all IOCTLs will pass through this function, but it's probably fine since secdrv uses unique control codes
    if (dwIoControlCode == 0xEF002407) {
        if ( ProcessSecDrvIoctl(lpInBuffer, nInBufferSize, lpOutBuffer, nOutBufferSize) ) {
            *lpBytesReturned = nOutBufferSize;
            return TRUE;
        }
        else return FALSE;
    }
    else if (dwIoControlCode == 0xCA002813) {
        Wh_Log(L"SafeDiscShim error: unhandled IOCTL 0xCA002813 (please report!)");
        return FALSE;
    }
    else return DeviceIoControl_Orig(hDevice, dwIoControlCode, lpInBuffer, nInBufferSize, lpOutBuffer, nOutBufferSize, lpBytesReturned, lpOverlapped);
}

using CreateFileA_t = decltype(&CreateFileA);
CreateFileA_t CreateFileA_Orig;
HANDLE WINAPI CreateFileA_Hook(LPCSTR lpFileName,
                               DWORD dwDesiredAccess,
                               DWORD dwShareMode,
                               LPSECURITY_ATTRIBUTES lpSecurityAttributes,
                               DWORD dwCreationDisposition,
                               DWORD dwFlagsAndAttributes,
                               HANDLE hTemplateFile) {
    if ( !lstrcmpiA(lpFileName, R"(\\.\Secdrv)") || !lstrcmpiA(lpFileName, R"(\\.\Global\SecDrv)") ) {
        /* we need to return a handle when secdrv is opened, so we just open the null device to get an unused handle */
        auto hDummy = CreateFileA_Orig(
            "NUL",
            GENERIC_READ,
            FILE_SHARE_READ,
            nullptr,
            CREATE_ALWAYS,
            FILE_ATTRIBUTE_NORMAL,
            nullptr
        );
        if (hDummy == INVALID_HANDLE_VALUE) {
            Wh_Log(L"SafeDiscShim error: Unable to obtain a dummy handle for SecDrv.");
        }
        return hDummy;
    }
    
    return CreateFileA_Orig(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
}

BOOL IsSafeDiscEXE() {
    // SafeDisc 1.x ICD check
    std::wstring exePath;
    DWORD dwPathSize = 0;
    while (dwPathSize >= exePath.size()) {
        exePath.resize(exePath.size() + MAX_PATH);
        dwPathSize = GetModuleFileName(nullptr, exePath.data(), exePath.size());
    }
    exePath.resize(dwPathSize);
    LCMapStringEx(nullptr, LCMAP_LOWERCASE, exePath.data(), exePath.size(), exePath.data(), exePath.size(), nullptr, nullptr, 0); // Make EXE path lowercase

    if(exePath.ends_with(L".icd")) {
        return TRUE;
    }

    // SafeDisc EXE check
    char* pbExecutable = (char*)GetModuleHandle(nullptr);

    IMAGE_DOS_HEADER* pDosHeader = (IMAGE_DOS_HEADER*)pbExecutable;
    IMAGE_NT_HEADERS* pNtHeader = (IMAGE_NT_HEADERS*)((char*)pDosHeader + pDosHeader->e_lfanew);
    IMAGE_SECTION_HEADER* pSectionHeader = (IMAGE_SECTION_HEADER*)((char*)&pNtHeader->OptionalHeader + pNtHeader->FileHeader.SizeOfOptionalHeader);

    for (auto i = 0; i < pNtHeader->FileHeader.NumberOfSections; ++i) {
        if (!memcmp(pSectionHeader->Name, "stxt", 4) || !memcmp(pSectionHeader->Name, ".txt2", 4)) {
            return TRUE;
        }
        pSectionHeader++;
    }
    
    return FALSE;
}

BOOL Wh_ModInit() {
    if (!IsSafeDiscEXE()) {
        return FALSE;
    }

    WindhawkUtils::SetFunctionHook(DeviceIoControl, DeviceIoControl_Hook, &DeviceIoControl_Orig);
    WindhawkUtils::SetFunctionHook(CreateFileA, CreateFileA_Hook, &CreateFileA_Orig);
    
    return TRUE;
}
