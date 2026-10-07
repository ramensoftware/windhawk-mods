// ==WindhawkMod==
// @id              precision-touchpad-elevated-actions
// @name            Fix Precision Touchpad Gestures for Elevated Windows
// @description     Makes precision-touchpad keyboard and mouse gesture actions work on elevated windows
// @version         0.6
// @author          meteoni
// @include         explorer.exe
// @include         windhawk-mod-uiaccess.exe
// @architecture    x86-64
// @compilerOptions -ladvapi32 -luser32 -lshell32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Precision Touchpad Elevated Window Actions

Windows precision-touchpad advanced gestures are synthesized by the shell as
keyboard or mouse input. Medium-integrity Explorer can synthesize the input,
but UIPI prevents it from reaching a higher-integrity target window.

This mod preserves Windows' native gesture semantics and only changes which
process performs the final synthetic input:

The privileged side is a native Windhawk 2.0 `windhawk-mod-uiaccess.exe` tool
host. It authenticates the shell Explorer client, revalidates the target just
before injection, and accepts only self-contained balanced keyboard actions or
middle/X-button mouse clicks. It cannot inject movement, wheel input, left/right
clicks, or unbalanced key/button state.

Requires Windhawk 2.0 alpha 4 or later.

Only the Explorer process that owns Windows' shell desktop window can relay
gestures. Other Explorer processes pass input through before stack inspection,
token queries, buffering or IPC. Shell identity is checked dynamically so the
hooks can load before the desktop exists and follow later shell registration.
There is no polling thread or process enumeration in Explorer.

The worker also checks the current shell immediately before injection. Its
pipe is scoped to this mod and Windows session. Windhawk owns host startup,
single-instance protection, hook removal and process teardown.

*/
// ==/WindhawkModReadme==

#include <windows.h>
#include <windhawk_tool_mod.h>
#include <sddl.h>
#include <atomic>
#include <cstdint>
#include <cstring>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

namespace {

constexpr wchar_t kUiAccessHostName[] = L"windhawk-mod-uiaccess.exe";
constexpr wchar_t kExplorerName[] = L"explorer.exe";

constexpr uint32_t kProtocolMagic = 0x54504857;  // "WHPT"
constexpr uint32_t kProtocolVersion = 2;
constexpr uint32_t kMaxKeyboardInputs = 32;
constexpr uint32_t kMaxMouseInputs = 4;
constexpr uint32_t kMaxPendingKeyboardDowns = kMaxKeyboardInputs / 2;
constexpr DWORD kPendingGestureTimeoutMs = 1000;
constexpr DWORD kPipeConnectTimeoutMs = 100;
constexpr DWORD kPipeTransactionTimeoutMs = 100;
constexpr DWORD kWorkerPipeRetryMs = 250;


enum class InputKind : uint32_t {
    Keyboard = 1,
    Mouse = 2,
};

struct InjectRequest {
    uint32_t magic;
    uint32_t version;
    InputKind kind;
    uint32_t count;
    uint32_t targetPid;
    uint64_t targetHwnd;
    union {
        KEYBDINPUT keyboard[kMaxKeyboardInputs];
        MOUSEINPUT mouse[kMaxMouseInputs];
    } payload;
};

static_assert(std::is_trivially_copyable_v<InjectRequest>);

enum class WorkerStatus : uint32_t {
    Injected = 1,
    StaleTarget = 2,
    Rejected = 3,
    InjectionFailed = 4,
    InternalError = 5,
};

enum class TransactionResult {
    NotSubmitted,
    Completed,
    SubmittedOutcomeUnknown,
};

struct InjectResponse {
    uint32_t magic;
    uint32_t version;
    WorkerStatus status;
    uint32_t injectResult;
    uint32_t lastError;
};

static_assert(std::is_trivially_copyable_v<InjectResponse>);

enum class ProcessMode {
    Explorer,
    UiAccessWorker,
    Other,
};

DWORD g_sessionId = 0;
DWORD g_explorerIntegrityRid = 0;
DWORD g_explorerPid = 0;
std::atomic<bool> g_explorerStopping{false};

HANDLE g_workerStopEvent = nullptr;
HANDLE g_workerThread = nullptr;

using InjectKeyboardInput_t = BOOL(WINAPI*)(const KEYBDINPUT*, UINT32);
InjectKeyboardInput_t g_workerInjectKeyboardInput = nullptr;

using InjectMouseInput_t = BOOL(WINAPI*)(const MOUSEINPUT*, UINT32);
InjectMouseInput_t g_workerInjectMouseInput = nullptr;

using NtUserInjectKeyboardInput_t = UINT(WINAPI*)(const KEYBDINPUT*, UINT32);
NtUserInjectKeyboardInput_t g_ntUserInjectKeyboardInputOriginal = nullptr;

using NtUserInjectMouseInput_t = UINT(WINAPI*)(const MOUSEINPUT*, UINT32);
NtUserInjectMouseInput_t g_ntUserInjectMouseInputOriginal = nullptr;

thread_local bool g_insideInjectionHook = false;

struct PendingKeyboardGesture {
    bool active = false;
    HWND target = nullptr;
    DWORD targetPid = 0;
    ULONGLONG startedAt = 0;
    uint32_t downCount = 0;
    KEYBDINPUT downs[kMaxPendingKeyboardDowns]{};
    uint32_t upCount = 0;
    KEYBDINPUT ups[kMaxPendingKeyboardDowns]{};
};

struct PendingMouseGesture {
    bool active = false;
    HWND target = nullptr;
    DWORD targetPid = 0;
    ULONGLONG startedAt = 0;
    MOUSEINPUT down{};
};

SRWLOCK g_pendingLock = SRWLOCK_INIT;
PendingKeyboardGesture g_pendingKeyboard;
PendingMouseGesture g_pendingMouse;

std::wstring GetCurrentProcessBaseName() {
    wchar_t path[1024] = {};
    DWORD length = GetModuleFileNameW(nullptr, path, ARRAYSIZE(path));
    if (!length || length >= ARRAYSIZE(path)) {
        return {};
    }

    const wchar_t* slash = wcsrchr(path, L'\\');
    return slash ? slash + 1 : path;
}

ProcessMode DetectProcessMode() {
    std::wstring name = GetCurrentProcessBaseName();
    if (_wcsicmp(name.c_str(), kExplorerName) == 0) {
        return ProcessMode::Explorer;
    }
    if (_wcsicmp(name.c_str(), kUiAccessHostName) == 0) {
        return ProcessMode::UiAccessWorker;
    }
    return ProcessMode::Other;
}

std::wstring BuildPipeName() {
    return std::wstring(L"\\\\.\\pipe\\Windhawk.PrecisionTouchpadElevatedActions.") +
           WH_MOD_ID + L"." + std::to_wstring(g_sessionId);
}

DWORD GetShellExplorerProcessId() {
    HWND shellWindow = GetShellWindow();
    DWORD shellPid = 0;
    if (shellWindow) {
        GetWindowThreadProcessId(shellWindow, &shellPid);
    }
    // Zero means the shell isn't registered yet or its window disappeared.
    // Never guess based on process age, a folder window, or a cached PID.
    return shellPid;
}

bool QueryTokenUser(HANDLE token, std::vector<BYTE>* bufferOut) {
    DWORD bytes = 0;
    GetTokenInformation(token, TokenUser, nullptr, 0, &bytes);
    if (!bytes || GetLastError() != ERROR_INSUFFICIENT_BUFFER) {
        return false;
    }

    std::vector<BYTE> buffer(bytes);
    if (!GetTokenInformation(token, TokenUser, buffer.data(), bytes, &bytes)) {
        return false;
    }

    *bufferOut = std::move(buffer);
    return true;
}

bool QueryProcessUser(DWORD pid, std::vector<BYTE>* bufferOut) {
    HANDLE process = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (!process) {
        return false;
    }

    HANDLE token = nullptr;
    bool ok = false;
    if (OpenProcessToken(process, TOKEN_QUERY, &token)) {
        ok = QueryTokenUser(token, bufferOut);
        CloseHandle(token);
    }

    CloseHandle(process);
    return ok;
}

bool QueryCurrentUser(std::vector<BYTE>* bufferOut) {
    HANDLE token = nullptr;
    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &token)) {
        return false;
    }

    bool ok = QueryTokenUser(token, bufferOut);
    CloseHandle(token);
    return ok;
}

bool SameUserBuffers(const std::vector<BYTE>& a,
                     const std::vector<BYTE>& b) {
    if (a.size() < sizeof(TOKEN_USER) || b.size() < sizeof(TOKEN_USER)) {
        return false;
    }

    auto aUser = reinterpret_cast<const TOKEN_USER*>(a.data());
    auto bUser = reinterpret_cast<const TOKEN_USER*>(b.data());
    return IsValidSid(aUser->User.Sid) && IsValidSid(bUser->User.Sid) &&
           EqualSid(aUser->User.Sid, bUser->User.Sid);
}

DWORD QueryTokenIntegrityRid(HANDLE token) {
    DWORD bytes = 0;
    GetTokenInformation(token, TokenIntegrityLevel, nullptr, 0, &bytes);
    if (!bytes || GetLastError() != ERROR_INSUFFICIENT_BUFFER) {
        return 0;
    }

    std::vector<BYTE> buffer(bytes);
    if (!GetTokenInformation(token, TokenIntegrityLevel, buffer.data(), bytes,
                             &bytes)) {
        return 0;
    }

    auto label = reinterpret_cast<const TOKEN_MANDATORY_LABEL*>(buffer.data());
    if (!IsValidSid(label->Label.Sid)) {
        return 0;
    }

    UCHAR count = *GetSidSubAuthorityCount(label->Label.Sid);
    if (!count) {
        return 0;
    }

    return *GetSidSubAuthority(label->Label.Sid, count - 1);
}

DWORD QueryProcessIntegrityRid(DWORD pid) {
    HANDLE process = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (!process) {
        return 0;
    }

    HANDLE token = nullptr;
    DWORD rid = 0;
    if (OpenProcessToken(process, TOKEN_QUERY, &token)) {
        rid = QueryTokenIntegrityRid(token);
        CloseHandle(token);
    }

    CloseHandle(process);
    return rid;
}

DWORD QueryCurrentIntegrityRid() {
    HANDLE token = nullptr;
    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &token)) {
        return 0;
    }

    DWORD rid = QueryTokenIntegrityRid(token);
    CloseHandle(token);
    return rid;
}

bool QueryCurrentUiAccess() {
    HANDLE token = nullptr;
    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &token)) {
        return false;
    }

    DWORD uiAccess = 0;
    DWORD bytes = 0;
    bool ok = GetTokenInformation(token, TokenUIAccess, &uiAccess,
                                  sizeof(uiAccess), &bytes) &&
              uiAccess != 0;
    CloseHandle(token);
    return ok;
}

bool QueryProcessImageBaseName(DWORD pid, std::wstring* baseNameOut) {
    HANDLE process = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (!process) {
        return false;
    }

    wchar_t path[1024] = {};
    DWORD chars = ARRAYSIZE(path);
    bool ok = QueryFullProcessImageNameW(process, 0, path, &chars) != FALSE;
    CloseHandle(process);
    if (!ok) {
        return false;
    }

    const wchar_t* slash = wcsrchr(path, L'\\');
    *baseNameOut = slash ? slash + 1 : path;
    return true;
}

bool ValidateSameSession(DWORD pid) {
    DWORD session = 0;
    return ProcessIdToSessionId(pid, &session) && session == g_sessionId;
}

bool ValidateWorkerClient(HANDLE pipe,
                          DWORD* clientPidOut,
                          DWORD* clientIntegrityOut) {
    ULONG clientPidRaw = 0;
    if (!GetNamedPipeClientProcessId(pipe, &clientPidRaw) || !clientPidRaw) {
        Wh_Log(L"Worker: GetNamedPipeClientProcessId failed: %lu",
               GetLastError());
        return false;
    }

    DWORD clientPid = static_cast<DWORD>(clientPidRaw);

    DWORD shellPid = GetShellExplorerProcessId();
    if (!shellPid || clientPid != shellPid) {
        Wh_Log(L"Worker: rejected client pid=%lu; shell explorer pid=%lu",
               clientPid, shellPid);
        return false;
    }

    if (!ValidateSameSession(clientPid)) {
        Wh_Log(L"Worker: rejected client %lu from another session", clientPid);
        return false;
    }

    std::wstring imageBaseName;
    if (!QueryProcessImageBaseName(clientPid, &imageBaseName) ||
        _wcsicmp(imageBaseName.c_str(), kExplorerName) != 0) {
        Wh_Log(L"Worker: rejected non-Explorer client pid=%lu image='%s'",
               clientPid, imageBaseName.c_str());
        return false;
    }

    std::vector<BYTE> currentUser;
    std::vector<BYTE> clientUser;
    if (!QueryCurrentUser(&currentUser) ||
        !QueryProcessUser(clientPid, &clientUser) ||
        !SameUserBuffers(currentUser, clientUser)) {
        Wh_Log(L"Worker: rejected client pid=%lu with a different user",
               clientPid);
        return false;
    }

    DWORD clientIntegrity = QueryProcessIntegrityRid(clientPid);
    if (!clientIntegrity) {
        Wh_Log(L"Worker: couldn't read client integrity pid=%lu", clientPid);
        return false;
    }

    *clientPidOut = clientPid;
    *clientIntegrityOut = clientIntegrity;
    return true;
}


bool ValidateWorkerServer(HANDLE pipe) {
    ULONG serverPidRaw = 0;
    if (!GetNamedPipeServerProcessId(pipe, &serverPidRaw) || !serverPidRaw) {
        Wh_Log(L"Explorer: GetNamedPipeServerProcessId failed: %lu",
               GetLastError());
        return false;
    }

    DWORD serverPid = static_cast<DWORD>(serverPidRaw);
    if (!ValidateSameSession(serverPid)) {
        Wh_Log(L"Explorer: rejected worker pid=%lu from another session",
               serverPid);
        return false;
    }

    HANDLE process = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE,
                                 serverPid);
    if (!process) {
        Wh_Log(L"Explorer: couldn't open worker pid=%lu: %lu", serverPid,
               GetLastError());
        return false;
    }

    wchar_t path[1024] = {};
    DWORD chars = ARRAYSIZE(path);
    if (!QueryFullProcessImageNameW(process, 0, path, &chars)) {
        Wh_Log(L"Explorer: couldn't query worker image pid=%lu: %lu",
               serverPid, GetLastError());
        CloseHandle(process);
        return false;
    }

    const wchar_t* slash = wcsrchr(path, L'\\');
    const wchar_t* baseName = slash ? slash + 1 : path;
    if (_wcsicmp(baseName, kUiAccessHostName) != 0) {
        Wh_Log(L"Explorer: rejected pipe server pid=%lu image='%s'",
               serverPid, baseName);
        CloseHandle(process);
        return false;
    }

    HANDLE token = nullptr;
    if (!OpenProcessToken(process, TOKEN_QUERY, &token)) {
        Wh_Log(L"Explorer: couldn't query worker token pid=%lu: %lu",
               serverPid, GetLastError());
        CloseHandle(process);
        return false;
    }

    std::vector<BYTE> serverUser;
    std::vector<BYTE> currentUser;
    DWORD uiAccess = 0;
    DWORD bytes = 0;
    bool tokenOk = QueryTokenUser(token, &serverUser) &&
                   QueryCurrentUser(&currentUser) &&
                   SameUserBuffers(serverUser, currentUser) &&
                   GetTokenInformation(token, TokenUIAccess, &uiAccess,
                                       sizeof(uiAccess), &bytes) &&
                   uiAccess != 0;

    CloseHandle(token);
    CloseHandle(process);

    if (!tokenOk) {
        Wh_Log(L"Explorer: rejected unauthenticated/non-UIAccess worker pid=%lu",
               serverPid);
        return false;
    }

    return true;
}

HMODULE ModuleFromAddress(const void* address) {
    if (!address) {
        return nullptr;
    }

    HMODULE module = nullptr;
    if (!GetModuleHandleExW(
            GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
            reinterpret_cast<LPCWSTR>(address), &module)) {
        return nullptr;
    }
    return module;
}

bool ModuleBaseNameEquals(HMODULE module, const wchar_t* expected) {
    if (!module || !expected) {
        return false;
    }

    wchar_t path[1024] = {};
    DWORD chars = GetModuleFileNameW(module, path, ARRAYSIZE(path));
    if (!chars || chars >= ARRAYSIZE(path)) {
        return false;
    }

    const wchar_t* slash = wcsrchr(path, L'\\');
    const wchar_t* baseName = slash ? slash + 1 : path;
    return _wcsicmp(baseName, expected) == 0;
}

bool AddressIsFromModule(const void* address, const wchar_t* moduleName) {
    return ModuleBaseNameEquals(ModuleFromAddress(address), moduleName);
}

bool StackContainsModule(const wchar_t* moduleName) {
    void* frames[24] = {};
    USHORT captured = CaptureStackBackTrace(1, ARRAYSIZE(frames), frames,
                                            nullptr);
    for (USHORT i = 0; i < captured; ++i) {
        if (AddressIsFromModule(frames[i], moduleName)) {
            return true;
        }
    }
    return false;
}

bool IsSupportedKeyboardRecord(const KEYBDINPUT& input) {
    constexpr DWORD kAllowedFlags = KEYEVENTF_EXTENDEDKEY |
                                    KEYEVENTF_KEYUP |
                                    KEYEVENTF_SCANCODE;
    return (input.dwFlags & ~kAllowedFlags) == 0;
}

bool SameKeyboardIdentity(const KEYBDINPUT& a, const KEYBDINPUT& b) {
    constexpr DWORD kIdentityFlags = KEYEVENTF_EXTENDEDKEY | KEYEVENTF_SCANCODE;
    return a.wVk == b.wVk && a.wScan == b.wScan &&
           (a.dwFlags & kIdentityFlags) ==
               (b.dwFlags & kIdentityFlags);
}

bool IsAllKeyboardDown(const KEYBDINPUT* inputs, uint32_t count) {
    if (!inputs || count == 0 || count > kMaxPendingKeyboardDowns) {
        return false;
    }

    for (uint32_t i = 0; i < count; ++i) {
        if (!IsSupportedKeyboardRecord(inputs[i]) ||
            (inputs[i].dwFlags & KEYEVENTF_KEYUP)) {
            return false;
        }
    }
    return true;
}

bool IsAllKeyboardUp(const KEYBDINPUT* inputs, uint32_t count) {
    if (!inputs || count == 0 || count > kMaxPendingKeyboardDowns) {
        return false;
    }

    for (uint32_t i = 0; i < count; ++i) {
        if (!IsSupportedKeyboardRecord(inputs[i]) ||
            !(inputs[i].dwFlags & KEYEVENTF_KEYUP)) {
            return false;
        }
    }
    return true;
}

bool KeyboardReleaseSetMatches(const KEYBDINPUT* downs,
                               uint32_t downCount,
                               const KEYBDINPUT* ups,
                               uint32_t upCount,
                               bool requireComplete) {
    if (!downs || !ups || !downCount || upCount > downCount ||
        (requireComplete && upCount != downCount)) {
        return false;
    }

    bool matched[kMaxPendingKeyboardDowns] = {};
    for (uint32_t i = 0; i < upCount; ++i) {
        if (!(ups[i].dwFlags & KEYEVENTF_KEYUP) ||
            !IsSupportedKeyboardRecord(ups[i])) {
            return false;
        }

        bool found = false;
        for (uint32_t j = 0; j < downCount; ++j) {
            if (!matched[j] && SameKeyboardIdentity(downs[j], ups[i])) {
                matched[j] = true;
                found = true;
                break;
            }
        }
        if (!found) {
            return false;
        }
    }

    return true;
}

bool IsBalancedKeyboardSequence(const KEYBDINPUT* inputs, uint32_t count) {
    if (!inputs || count < 2 || count > kMaxKeyboardInputs) {
        return false;
    }

    KEYBDINPUT active[kMaxPendingKeyboardDowns]{};
    uint32_t activeCount = 0;
    bool sawDown = false;
    bool sawUp = false;

    for (uint32_t i = 0; i < count; ++i) {
        const KEYBDINPUT& current = inputs[i];
        if (!IsSupportedKeyboardRecord(current)) {
            return false;
        }

        if (!(current.dwFlags & KEYEVENTF_KEYUP)) {
            if (activeCount >= ARRAYSIZE(active)) {
                return false;
            }
            active[activeCount++] = current;
            sawDown = true;
            continue;
        }

        sawUp = true;
        bool found = false;
        for (uint32_t j = activeCount; j > 0; --j) {
            uint32_t index = j - 1;
            if (SameKeyboardIdentity(active[index], current)) {
                for (uint32_t k = index + 1; k < activeCount; ++k) {
                    active[k - 1] = active[k];
                }
                --activeCount;
                found = true;
                break;
            }
        }
        if (!found) {
            return false;
        }
    }

    return sawDown && sawUp && activeCount == 0;
}

enum class MouseButtonKind {
    None,
    Middle,
    X1,
    X2,
};

MouseButtonKind GetMouseDownKind(const MOUSEINPUT& input) {
    if (input.dx != 0 || input.dy != 0) {
        return MouseButtonKind::None;
    }

    if (input.dwFlags == MOUSEEVENTF_MIDDLEDOWN && input.mouseData == 0) {
        return MouseButtonKind::Middle;
    }
    if (input.dwFlags == MOUSEEVENTF_XDOWN) {
        if (input.mouseData == XBUTTON1) return MouseButtonKind::X1;
        if (input.mouseData == XBUTTON2) return MouseButtonKind::X2;
    }
    return MouseButtonKind::None;
}

MouseButtonKind GetMouseUpKind(const MOUSEINPUT& input) {
    if (input.dx != 0 || input.dy != 0) {
        return MouseButtonKind::None;
    }

    if (input.dwFlags == MOUSEEVENTF_MIDDLEUP && input.mouseData == 0) {
        return MouseButtonKind::Middle;
    }
    if (input.dwFlags == MOUSEEVENTF_XUP) {
        if (input.mouseData == XBUTTON1) return MouseButtonKind::X1;
        if (input.mouseData == XBUTTON2) return MouseButtonKind::X2;
    }
    return MouseButtonKind::None;
}

bool IsBalancedMouseSequence(const MOUSEINPUT* inputs, uint32_t count) {
    if (!inputs || count != 2) {
        return false;
    }

    MouseButtonKind down = GetMouseDownKind(inputs[0]);
    MouseButtonKind up = GetMouseUpKind(inputs[1]);
    return down != MouseButtonKind::None && down == up;
}

HWND GetMouseRootTarget() {
    POINT pt{};
    if (!GetCursorPos(&pt)) {
        return nullptr;
    }

    HWND hit = WindowFromPoint(pt);
    if (!hit) {
        return nullptr;
    }
    return GetAncestor(hit, GA_ROOT);
}

bool ValidateTargetForKind(InputKind kind, HWND target, DWORD targetPid) {
    if (!target || !IsWindow(target)) {
        return false;
    }

    DWORD currentPid = 0;
    GetWindowThreadProcessId(target, &currentPid);
    if (!currentPid || currentPid != targetPid) {
        return false;
    }

    if (kind == InputKind::Keyboard) {
        return GetForegroundWindow() == target;
    }

    return GetMouseRootTarget() == target;
}

InjectResponse ProcessWorkerRequest(DWORD clientPid,
                                    DWORD clientIntegrity,
                                    const InjectRequest& request) {
    InjectResponse response{
        .magic = kProtocolMagic,
        .version = kProtocolVersion,
        .status = WorkerStatus::Rejected,
        .injectResult = 0,
        .lastError = ERROR_ACCESS_DENIED,
    };

    if (request.magic != kProtocolMagic ||
        request.version != kProtocolVersion || request.count == 0) {
        Wh_Log(L"Worker: rejected malformed request from pid=%lu", clientPid);
        response.lastError = ERROR_INVALID_DATA;
        return response;
    }

    switch (request.kind) {
        case InputKind::Keyboard:
            if (request.count > kMaxKeyboardInputs ||
                !IsBalancedKeyboardSequence(request.payload.keyboard,
                                            request.count)) {
                Wh_Log(L"Worker: rejected unbalanced/unsupported keyboard request from pid=%lu",
                       clientPid);
                response.lastError = ERROR_ACCESS_DENIED;
                return response;
            }
            break;

        case InputKind::Mouse:
            if (request.count > kMaxMouseInputs ||
                !IsBalancedMouseSequence(request.payload.mouse,
                                         request.count)) {
                Wh_Log(L"Worker: rejected unsupported mouse request from pid=%lu",
                       clientPid);
                response.lastError = ERROR_ACCESS_DENIED;
                return response;
            }
            break;

        default:
            response.lastError = ERROR_INVALID_DATA;
            return response;
    }

    HWND target = reinterpret_cast<HWND>(
        static_cast<uintptr_t>(request.targetHwnd));
    if (!ValidateTargetForKind(request.kind, target, request.targetPid)) {
        Wh_Log(L"Worker: target changed before injection (kind=%u hwnd=%p pid=%lu foreground=%p mouseRoot=%p)",
               static_cast<unsigned>(request.kind), target, request.targetPid,
               GetForegroundWindow(), GetMouseRootTarget());
        response.status = WorkerStatus::StaleTarget;
        response.lastError = ERROR_CANCELLED;
        return response;
    }

    DWORD targetPid = request.targetPid;
    if (!ValidateSameSession(targetPid)) {
        response.status = WorkerStatus::StaleTarget;
        response.lastError = ERROR_CANCELLED;
        return response;
    }

    std::vector<BYTE> currentUser;
    std::vector<BYTE> targetUser;
    if (!QueryCurrentUser(&currentUser) ||
        !QueryProcessUser(targetPid, &targetUser) ||
        !SameUserBuffers(currentUser, targetUser)) {
        Wh_Log(L"Worker: rejected target pid=%lu owned by another user",
               targetPid);
        response.lastError = ERROR_ACCESS_DENIED;
        return response;
    }

    DWORD targetIntegrity = QueryProcessIntegrityRid(targetPid);
    if (!targetIntegrity || targetIntegrity <= clientIntegrity) {
        response.status = WorkerStatus::StaleTarget;
        response.lastError = ERROR_CANCELLED;
        return response;
    }

    if ((request.kind == InputKind::Keyboard &&
         !g_workerInjectKeyboardInput) ||
        (request.kind == InputKind::Mouse && !g_workerInjectMouseInput)) {
        response.status = WorkerStatus::InternalError;
        response.lastError = ERROR_PROC_NOT_FOUND;
        return response;
    }

    // Revalidate at the last possible moment. Never replay privileged input
    // into a window that became active/under-pointer during validation.
    if (!ValidateTargetForKind(request.kind, target, targetPid)) {
        response.status = WorkerStatus::StaleTarget;
        response.lastError = ERROR_CANCELLED;
        return response;
    }

    // Authentication precedes the pipe read, which can block. The client may
    // have stopped being the shell by the time its request arrives, or this
    // worker may now be stopping. Check both before issuing privileged input.
    if (GetShellExplorerProcessId() != clientPid ||
        WaitForSingleObject(g_workerStopEvent, 0) == WAIT_OBJECT_0) {
        Wh_Log(L"Worker: cancelled request from retired/stopping shell pid=%lu",
               clientPid);
        response.lastError = ERROR_CANCELLED;
        return response;
    }

    SetLastError(ERROR_SUCCESS);
    BOOL injected = FALSE;
    if (request.kind == InputKind::Keyboard) {
        injected = g_workerInjectKeyboardInput(request.payload.keyboard,
                                               request.count);
    } else {
        injected = g_workerInjectMouseInput(request.payload.mouse,
                                            request.count);
    }
    DWORD injectError = GetLastError();

    response.injectResult = injected ? 1u : 0u;
    response.lastError = injectError;
    response.status = injected ? WorkerStatus::Injected
                               : WorkerStatus::InjectionFailed;

    Wh_Log(L"Worker: forwarded %u touchpad %s records to hwnd=%p pid=%lu IL=0x%lX -> %d err=%lu",
           request.count,
           request.kind == InputKind::Keyboard ? L"keyboard" : L"mouse",
           target, targetPid, targetIntegrity, injected, injectError);
    return response;
}

bool CreateWorkerPipeSecurity(SECURITY_ATTRIBUTES* attributes,
                              PSECURITY_DESCRIPTOR* descriptorOut) {
    // The pipe grants this desktop user only generic read/write (and SYSTEM
    // full access). The medium mandatory label allows the medium-IL shell to
    // reach the high-IL UIAccess
    // host. The server additionally authenticates the exact shell Explorer PID.
    HANDLE token = nullptr;
    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &token)) {
        Wh_Log(L"Worker: OpenProcessToken failed: %lu", GetLastError());
        return false;
    }

    std::vector<BYTE> userBuffer;
    bool gotUser = QueryTokenUser(token, &userBuffer);
    CloseHandle(token);
    if (!gotUser || userBuffer.size() < sizeof(TOKEN_USER)) {
        Wh_Log(L"Worker: couldn't read current user SID");
        return false;
    }

    auto user = reinterpret_cast<const TOKEN_USER*>(userBuffer.data());
    LPWSTR sidString = nullptr;
    if (!ConvertSidToStringSidW(user->User.Sid, &sidString)) {
        Wh_Log(L"Worker: ConvertSidToStringSid failed: %lu", GetLastError());
        return false;
    }

    std::wstring sddl = L"D:P(A;;GRGW;;;";
    sddl += sidString;
    sddl += L")(A;;GA;;;SY)S:(ML;;NW;;;ME)";
    LocalFree(sidString);

    PSECURITY_DESCRIPTOR descriptor = nullptr;
    if (!ConvertStringSecurityDescriptorToSecurityDescriptorW(
            sddl.c_str(), SDDL_REVISION_1, &descriptor, nullptr)) {
        Wh_Log(L"Worker: security descriptor creation failed: %lu",
               GetLastError());
        return false;
    }

    attributes->nLength = sizeof(*attributes);
    attributes->lpSecurityDescriptor = descriptor;
    attributes->bInheritHandle = FALSE;
    *descriptorOut = descriptor;
    return true;
}

DWORD WINAPI WorkerThreadProc(void*) {
    struct SignalStopOnExit {
        HANDLE event;
        ~SignalStopOnExit() {
            if (event) {
                SetEvent(event);
            }
        }
    } signalStopOnExit{g_workerStopEvent};

    std::wstring pipeName = BuildPipeName();

    SECURITY_ATTRIBUTES securityAttributes{};
    PSECURITY_DESCRIPTOR securityDescriptor = nullptr;
    while (!CreateWorkerPipeSecurity(&securityAttributes, &securityDescriptor)) {
        if (WaitForSingleObject(g_workerStopEvent,
                                kWorkerPipeRetryMs) == WAIT_OBJECT_0) {
            return 0;
        }
    }

    HANDLE pipe = INVALID_HANDLE_VALUE;
    DWORD lastCreateError = ERROR_SUCCESS;
    while (WaitForSingleObject(g_workerStopEvent, 0) != WAIT_OBJECT_0) {
        // Keep the server handle between requests. Closing it after every
        // gesture leaves a missing-pipe gap and can race a still-open client.
        if (pipe == INVALID_HANDLE_VALUE) {
            pipe = CreateNamedPipeW(
                pipeName.c_str(),
                PIPE_ACCESS_DUPLEX | FILE_FLAG_FIRST_PIPE_INSTANCE,
                PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT |
                    PIPE_REJECT_REMOTE_CLIENTS,
                1,
                sizeof(InjectResponse),
                sizeof(InjectRequest),
                0,
                &securityAttributes);

            if (pipe == INVALID_HANDLE_VALUE) {
                DWORD error = GetLastError();
                if (error != lastCreateError) {
                    Wh_Log(L"Worker: CreateNamedPipe failed: %lu; retrying", error);
                    lastCreateError = error;
                }
                if (WaitForSingleObject(g_workerStopEvent,
                                        kWorkerPipeRetryMs) == WAIT_OBJECT_0) {
                    break;
                }
                continue;
            }
            lastCreateError = ERROR_SUCCESS;
            Wh_Log(L"Worker: listening on %s", pipeName.c_str());
        }

        BOOL connected = ConnectNamedPipe(pipe, nullptr);
        if (!connected) {
            DWORD error = GetLastError();
            if (error == ERROR_PIPE_CONNECTED) {
                connected = TRUE;
            } else if (error == ERROR_OPERATION_ABORTED &&
                       WaitForSingleObject(g_workerStopEvent, 0) ==
                           WAIT_OBJECT_0) {
                break;
            } else {
                Wh_Log(L"Worker: ConnectNamedPipe failed: %lu", error);
            }
        }

        if (connected) {
            // Authenticate immediately after connection, before any blocking
            // read. The pipe DACL intentionally permits this desktop user, so
            // without this check another same-user process could connect and
            // hold the worker's only pipe instance open indefinitely.
            DWORD clientPid = 0;
            DWORD clientIntegrity = 0;
            if (ValidateWorkerClient(pipe, &clientPid, &clientIntegrity)) {
                InjectRequest request{};
                DWORD bytesRead = 0;
                if (ReadFile(pipe, &request, sizeof(request), &bytesRead,
                             nullptr) &&
                    bytesRead == sizeof(request)) {
                    InjectResponse response = ProcessWorkerRequest(
                        clientPid, clientIntegrity, request);
                    DWORD bytesWritten = 0;
                    if (!WriteFile(pipe, &response, sizeof(response),
                                   &bytesWritten, nullptr) ||
                        bytesWritten != sizeof(response)) {
                        Wh_Log(L"Worker: response write failed: %lu",
                               GetLastError());
                    } else {
                        // A successful WriteFile can still leave the reply in
                        // the pipe buffer. DisconnectNamedPipe discards unread
                        // data, so wait for the client to read before reusing it.
                        FlushFileBuffers(pipe);
                    }
                } else if (WaitForSingleObject(g_workerStopEvent, 0) !=
                           WAIT_OBJECT_0) {
                    Wh_Log(L"Worker: request read failed: %lu",
                           GetLastError());
                }
            }
        }

        if (!DisconnectNamedPipe(pipe) &&
            GetLastError() != ERROR_PIPE_NOT_CONNECTED) {
            Wh_Log(L"Worker: DisconnectNamedPipe failed: %lu; recreating pipe",
                   GetLastError());
            CloseHandle(pipe);
            pipe = INVALID_HANDLE_VALUE;
        }
    }

    if (pipe != INVALID_HANDLE_VALUE) {
        CloseHandle(pipe);
    }
    LocalFree(securityDescriptor);
    Wh_Log(L"Worker: stopped");
    return 0;
}

bool InitializeUiAccessWorker() {
    if (!ProcessIdToSessionId(GetCurrentProcessId(), &g_sessionId)) {
        Wh_Log(L"Worker: ProcessIdToSessionId failed: %lu", GetLastError());
        return false;
    }

    DWORD integrity = QueryCurrentIntegrityRid();
    bool uiAccess = QueryCurrentUiAccess();
    Wh_Log(L"Worker: starting in %s, session=%lu IL=0x%lX UIAccess=%d",
           kUiAccessHostName, g_sessionId, integrity, uiAccess);

    if (!uiAccess) {
        Wh_Log(L"Worker: host doesn't have UIAccess; refusing to run");
        return false;
    }

    HMODULE user32 = GetModuleHandleW(L"user32.dll");
    if (!user32) {
        user32 = LoadLibraryW(L"user32.dll");
    }
    if (!user32) {
        Wh_Log(L"Worker: couldn't load user32.dll");
        return false;
    }

    g_workerInjectKeyboardInput = reinterpret_cast<InjectKeyboardInput_t>(
        GetProcAddress(user32, "InjectKeyboardInput"));
    g_workerInjectMouseInput = reinterpret_cast<InjectMouseInput_t>(
        GetProcAddress(user32, "InjectMouseInput"));
    if (!g_workerInjectKeyboardInput || !g_workerInjectMouseInput) {
        Wh_Log(L"Worker: required user32 injection exports aren't available");
        return false;
    }

    g_workerStopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_workerStopEvent) {
        Wh_Log(L"Worker: CreateEvent failed: %lu", GetLastError());
        return false;
    }

    g_workerThread =
        CreateThread(nullptr, 0, WorkerThreadProc, nullptr, 0, nullptr);
    if (!g_workerThread) {
        Wh_Log(L"Worker: CreateThread failed: %lu", GetLastError());
        CloseHandle(g_workerStopEvent);
        g_workerStopEvent = nullptr;
        return false;
    }

    return true;
}

void StopUiAccessWorker() {
    if (g_workerStopEvent) {
        SetEvent(g_workerStopEvent);
    }

    if (g_workerThread) {
        // Cancellation can race the worker entering its next blocking call.
        // Repeat it until the thread exits, rather than cancelling only once.
        ULONGLONG deadline = GetTickCount64() + 1500;
        DWORD wait;
        do {
            CancelSynchronousIo(g_workerThread);
            wait = WaitForSingleObject(g_workerThread, 50);
        } while (wait == WAIT_TIMEOUT && GetTickCount64() < deadline);
        if (wait == WAIT_OBJECT_0) {
            CloseHandle(g_workerThread);
            g_workerThread = nullptr;
        } else {
            // Windhawk's tool header exits the process after this callback.
            // Don't close a handle or synchronization object that a still-live
            // worker may be using; process teardown will reclaim both.
            Wh_Log(L"Worker: thread didn't stop before process exit (wait=%lu)",
                   wait);
        }
    }

    if (!g_workerThread && g_workerStopEvent) {
        CloseHandle(g_workerStopEvent);
        g_workerStopEvent = nullptr;
    }
}

TransactionResult TransactWithWorker(const InjectRequest& request,
                                     InjectResponse* responseOut) {
    std::wstring pipeName = BuildPipeName();

    HANDLE pipe = INVALID_HANDLE_VALUE;
    ULONGLONG deadline = GetTickCount64() + kPipeConnectTimeoutMs;
    for (;;) {
        pipe = CreateFileW(pipeName.c_str(), GENERIC_READ | GENERIC_WRITE,
                           0, nullptr, OPEN_EXISTING,
                           FILE_FLAG_OVERLAPPED, nullptr);
        if (pipe != INVALID_HANDLE_VALUE) {
            break;
        }
        DWORD error = GetLastError();
        ULONGLONG now = GetTickCount64();
        if ((error != ERROR_FILE_NOT_FOUND && error != ERROR_PIPE_BUSY) ||
            now >= deadline) {
            Wh_Log(L"Explorer: worker connection failed: %lu", error);
            return TransactionResult::NotSubmitted;
        }
        DWORD remaining = static_cast<DWORD>(deadline - now);
        if (error == ERROR_PIPE_BUSY) {
            WaitNamedPipeW(pipeName.c_str(), remaining);
        } else {
            // WaitNamedPipe returns immediately when no server exists, even
            // with a timeout. Allow a starting worker to create its pipe.
            Sleep(remaining < 10 ? remaining : 10);
        }
    }

    // The pipe name is predictable, so authenticate the server before sending
    // the privileged request. This prevents a same-user process from
    // pre-creating the name and spoofing a successful response.
    if (!ValidateWorkerServer(pipe)) {
        CloseHandle(pipe);
        return TransactionResult::NotSubmitted;
    }

    DWORD mode = PIPE_READMODE_MESSAGE;
    if (!SetNamedPipeHandleState(pipe, &mode, nullptr, nullptr)) {
        CloseHandle(pipe);
        return TransactionResult::NotSubmitted;
    }

    HANDLE event = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!event) {
        CloseHandle(pipe);
        return TransactionResult::NotSubmitted;
    }

    OVERLAPPED overlapped{};
    overlapped.hEvent = event;

    InjectResponse response{};
    DWORD bytesRead = 0;
    BOOL ok = TransactNamedPipe(
        pipe,
        const_cast<InjectRequest*>(&request),
        sizeof(request),
        &response,
        sizeof(response),
        &bytesRead,
        &overlapped);

    // From this point onward the request may have reached the worker. Any
    // failure therefore has an ambiguous side effect and must never fall back
    // to injecting the same gesture locally a second time.
    bool outcomeUnknown = false;

    if (!ok) {
        DWORD transactError = GetLastError();
        if (transactError == ERROR_IO_PENDING) {
            DWORD wait = WaitForSingleObject(event, kPipeTransactionTimeoutMs);
            if (wait == WAIT_OBJECT_0) {
                ok = GetOverlappedResult(pipe, &overlapped, &bytesRead, FALSE);
                if (!ok) {
                    outcomeUnknown = true;
                }
            } else {
                // CancelIoEx only requests cancellation. The OVERLAPPED
                // structure and its event must remain alive until this exact
                // operation has actually completed.
                if (!CancelIoEx(pipe, &overlapped) &&
                    GetLastError() != ERROR_NOT_FOUND) {
                    Wh_Log(L"Explorer: CancelIoEx failed: %lu",
                           GetLastError());
                }

                DWORD cancelledBytes = 0;
                GetOverlappedResult(pipe, &overlapped, &cancelledBytes, TRUE);
                ok = FALSE;
                outcomeUnknown = true;
            }
        } else {
            outcomeUnknown = true;
        }
    }

    CloseHandle(event);
    CloseHandle(pipe);

    if (outcomeUnknown || !ok || bytesRead != sizeof(response) ||
        response.magic != kProtocolMagic ||
        response.version != kProtocolVersion) {
        return TransactionResult::SubmittedOutcomeUnknown;
    }

    *responseOut = response;
    return TransactionResult::Completed;
}

bool GetHigherIntegrityKeyboardTarget(HWND* targetOut,
                                      DWORD* pidOut,
                                      DWORD* integrityOut) {
    HWND target = GetForegroundWindow();
    DWORD pid = 0;
    if (!target || !GetWindowThreadProcessId(target, &pid) || !pid) {
        return false;
    }

    DWORD integrity = QueryProcessIntegrityRid(pid);
    if (!integrity || !g_explorerIntegrityRid ||
        integrity <= g_explorerIntegrityRid) {
        return false;
    }

    *targetOut = target;
    *pidOut = pid;
    *integrityOut = integrity;
    return true;
}

bool GetHigherIntegrityMouseTarget(HWND* targetOut,
                                   DWORD* pidOut,
                                   DWORD* integrityOut) {
    HWND target = GetMouseRootTarget();
    DWORD pid = 0;
    if (!target || !GetWindowThreadProcessId(target, &pid) || !pid) {
        return false;
    }

    DWORD integrity = QueryProcessIntegrityRid(pid);
    if (!integrity || !g_explorerIntegrityRid ||
        integrity <= g_explorerIntegrityRid) {
        return false;
    }

    *targetOut = target;
    *pidOut = pid;
    *integrityOut = integrity;
    return true;
}

bool IsPendingExpired(ULONGLONG startedAt, ULONGLONG now) {
    return !startedAt || now - startedAt > kPendingGestureTimeoutMs;
}

void ClearPendingKeyboardLocked() {
    g_pendingKeyboard = {};
}

void ClearPendingMouseLocked() {
    g_pendingMouse = {};
}

bool BufferKeyboardDown(const KEYBDINPUT* inputs,
                        uint32_t count,
                        HWND target,
                        DWORD targetPid) {
    ULONGLONG now = GetTickCount64();
    AcquireSRWLockExclusive(&g_pendingLock);

    if (g_pendingKeyboard.active &&
        IsPendingExpired(g_pendingKeyboard.startedAt, now)) {
        ClearPendingKeyboardLocked();
    }

    // Each observed touchpad gesture start carries its complete set of key-down
    // records in one call. Replace any unfinished older gesture rather than
    // accidentally merging two separate shortcuts if a release was lost.
    ClearPendingKeyboardLocked();
    g_pendingKeyboard.active = true;
    g_pendingKeyboard.target = target;
    g_pendingKeyboard.targetPid = targetPid;
    g_pendingKeyboard.startedAt = now;
    memcpy(g_pendingKeyboard.downs, inputs, sizeof(KEYBDINPUT) * count);
    g_pendingKeyboard.downCount = count;

    ReleaseSRWLockExclusive(&g_pendingLock);
    return true;
}

enum class BufferedReleaseResult {
    NoMatch,
    Partial,
    Complete,
};

BufferedReleaseResult ConsumeKeyboardRelease(
    const KEYBDINPUT* inputs,
    uint32_t count,
    InjectRequest* requestOut) {
    ULONGLONG now = GetTickCount64();
    AcquireSRWLockExclusive(&g_pendingLock);

    if (!g_pendingKeyboard.active ||
        IsPendingExpired(g_pendingKeyboard.startedAt, now)) {
        if (g_pendingKeyboard.active) {
            ClearPendingKeyboardLocked();
        }
        ReleaseSRWLockExclusive(&g_pendingLock);
        return BufferedReleaseResult::NoMatch;
    }

    if (g_pendingKeyboard.upCount + count >
        g_pendingKeyboard.downCount) {
        ReleaseSRWLockExclusive(&g_pendingLock);
        return BufferedReleaseResult::NoMatch;
    }

    KEYBDINPUT combinedUps[kMaxPendingKeyboardDowns]{};
    memcpy(combinedUps, g_pendingKeyboard.ups,
           sizeof(KEYBDINPUT) * g_pendingKeyboard.upCount);
    memcpy(combinedUps + g_pendingKeyboard.upCount, inputs,
           sizeof(KEYBDINPUT) * count);
    uint32_t newUpCount = g_pendingKeyboard.upCount + count;

    if (!KeyboardReleaseSetMatches(g_pendingKeyboard.downs,
                                   g_pendingKeyboard.downCount,
                                   combinedUps, newUpCount, false)) {
        ReleaseSRWLockExclusive(&g_pendingLock);
        return BufferedReleaseResult::NoMatch;
    }

    memcpy(g_pendingKeyboard.ups + g_pendingKeyboard.upCount,
           inputs, sizeof(KEYBDINPUT) * count);
    g_pendingKeyboard.upCount = newUpCount;

    if (g_pendingKeyboard.upCount < g_pendingKeyboard.downCount) {
        ReleaseSRWLockExclusive(&g_pendingLock);
        return BufferedReleaseResult::Partial;
    }

    if (!KeyboardReleaseSetMatches(g_pendingKeyboard.downs,
                                   g_pendingKeyboard.downCount,
                                   g_pendingKeyboard.ups,
                                   g_pendingKeyboard.upCount, true)) {
        ClearPendingKeyboardLocked();
        ReleaseSRWLockExclusive(&g_pendingLock);
        return BufferedReleaseResult::NoMatch;
    }

    InjectRequest request{};
    request.magic = kProtocolMagic;
    request.version = kProtocolVersion;
    request.kind = InputKind::Keyboard;
    request.count = g_pendingKeyboard.downCount + g_pendingKeyboard.upCount;
    request.targetPid = g_pendingKeyboard.targetPid;
    request.targetHwnd = static_cast<uint64_t>(
        reinterpret_cast<uintptr_t>(g_pendingKeyboard.target));
    memcpy(request.payload.keyboard, g_pendingKeyboard.downs,
           sizeof(KEYBDINPUT) * g_pendingKeyboard.downCount);
    memcpy(request.payload.keyboard + g_pendingKeyboard.downCount,
           g_pendingKeyboard.ups,
           sizeof(KEYBDINPUT) * g_pendingKeyboard.upCount);

    ClearPendingKeyboardLocked();
    ReleaseSRWLockExclusive(&g_pendingLock);

    *requestOut = request;
    return BufferedReleaseResult::Complete;
}

bool BufferMouseDown(const MOUSEINPUT& input,
                     HWND target,
                     DWORD targetPid) {
    ULONGLONG now = GetTickCount64();
    AcquireSRWLockExclusive(&g_pendingLock);

    if (g_pendingMouse.active &&
        IsPendingExpired(g_pendingMouse.startedAt, now)) {
        ClearPendingMouseLocked();
    }

    // A new gesture start replaces an unfinished old one. Nothing privileged
    // has been injected yet, so discarding it cannot leave button state held.
    g_pendingMouse.active = true;
    g_pendingMouse.target = target;
    g_pendingMouse.targetPid = targetPid;
    g_pendingMouse.startedAt = now;
    g_pendingMouse.down = input;

    ReleaseSRWLockExclusive(&g_pendingLock);
    return true;
}

bool ConsumeMouseRelease(const MOUSEINPUT& input, InjectRequest* requestOut) {
    ULONGLONG now = GetTickCount64();
    AcquireSRWLockExclusive(&g_pendingLock);

    if (!g_pendingMouse.active ||
        IsPendingExpired(g_pendingMouse.startedAt, now)) {
        if (g_pendingMouse.active) {
            ClearPendingMouseLocked();
        }
        ReleaseSRWLockExclusive(&g_pendingLock);
        return false;
    }

    MouseButtonKind downKind = GetMouseDownKind(g_pendingMouse.down);
    MouseButtonKind upKind = GetMouseUpKind(input);
    if (downKind == MouseButtonKind::None || downKind != upKind) {
        ReleaseSRWLockExclusive(&g_pendingLock);
        return false;
    }

    InjectRequest request{};
    request.magic = kProtocolMagic;
    request.version = kProtocolVersion;
    request.kind = InputKind::Mouse;
    request.count = 2;
    request.targetPid = g_pendingMouse.targetPid;
    request.targetHwnd = static_cast<uint64_t>(
        reinterpret_cast<uintptr_t>(g_pendingMouse.target));
    request.payload.mouse[0] = g_pendingMouse.down;
    request.payload.mouse[1] = input;

    ClearPendingMouseLocked();
    ReleaseSRWLockExclusive(&g_pendingLock);

    *requestOut = request;
    return true;
}

UINT FinishForwardedKeyboardRequest(const InjectRequest& request,
                                    DWORD incomingLastError,
                                    bool allowOriginalFallback) {
    InjectResponse response{};
    TransactionResult transaction = TransactWithWorker(request, &response);

    if (transaction == TransactionResult::NotSubmitted) {
        if (allowOriginalFallback) {
            SetLastError(incomingLastError);
            return g_ntUserInjectKeyboardInputOriginal(
                request.payload.keyboard, request.count);
        }

        Wh_Log(L"Explorer: worker unavailable after buffering keyboard gesture; consuming it");
        SetLastError(incomingLastError);
        return 1;
    }

    if (transaction == TransactionResult::SubmittedOutcomeUnknown) {
        Wh_Log(L"Explorer: keyboard worker transaction outcome unknown; suppressing fallback replay");
        SetLastError(incomingLastError);
        return 1;
    }

    switch (response.status) {
        case WorkerStatus::Injected:
            SetLastError(incomingLastError);
            return response.injectResult ? 1u : 0u;

        case WorkerStatus::StaleTarget:
            SetLastError(incomingLastError);
            return 1;

        case WorkerStatus::InjectionFailed:
            Wh_Log(L"UIAccess worker keyboard injection failed: %lu",
                   response.lastError);
            SetLastError(response.lastError);
            return 0;

        case WorkerStatus::Rejected:
        case WorkerStatus::InternalError:
        default:
            Wh_Log(L"UIAccess worker rejected keyboard action: status=%u error=%lu",
                   static_cast<unsigned>(response.status), response.lastError);
            SetLastError(response.lastError);
            return 0;
    }
}

UINT FinishForwardedMouseRequest(const InjectRequest& request,
                                 DWORD incomingLastError,
                                 bool allowOriginalFallback) {
    InjectResponse response{};
    TransactionResult transaction = TransactWithWorker(request, &response);

    if (transaction == TransactionResult::NotSubmitted) {
        if (allowOriginalFallback) {
            SetLastError(incomingLastError);
            return g_ntUserInjectMouseInputOriginal(
                request.payload.mouse, request.count);
        }

        Wh_Log(L"Explorer: worker unavailable after buffering mouse gesture; consuming it");
        SetLastError(incomingLastError);
        return 1;
    }

    if (transaction == TransactionResult::SubmittedOutcomeUnknown) {
        Wh_Log(L"Explorer: mouse worker transaction outcome unknown; suppressing fallback replay");
        SetLastError(incomingLastError);
        return 1;
    }

    switch (response.status) {
        case WorkerStatus::Injected:
            SetLastError(incomingLastError);
            return response.injectResult ? 1u : 0u;

        case WorkerStatus::StaleTarget:
            SetLastError(incomingLastError);
            return 1;

        case WorkerStatus::InjectionFailed:
            Wh_Log(L"UIAccess worker mouse injection failed: %lu",
                   response.lastError);
            SetLastError(response.lastError);
            return 0;

        case WorkerStatus::Rejected:
        case WorkerStatus::InternalError:
        default:
            Wh_Log(L"UIAccess worker rejected mouse action: status=%u error=%lu",
                   static_cast<unsigned>(response.status), response.lastError);
            SetLastError(response.lastError);
            return 0;
    }
}

UINT WINAPI NtUserInjectKeyboardInput_Hook(const KEYBDINPUT* inputs,
                                           UINT32 count) {
    if (g_insideInjectionHook || !inputs || count == 0 ||
        count > kMaxKeyboardInputs) {
        return g_ntUserInjectKeyboardInputOriginal(inputs, count);
    }

    DWORD incomingLastError = GetLastError();
    if (g_explorerStopping.load(std::memory_order_relaxed) ||
        GetShellExplorerProcessId() != g_explorerPid) {
        // Secondary Explorer instances do no stack walking, token queries,
        // buffering or IPC. Restore last-error before the untouched native call.
        SetLastError(incomingLastError);
        return g_ntUserInjectKeyboardInputOriginal(inputs, count);
    }

    g_insideInjectionHook = true;
    struct ResetHookFlag {
        ~ResetHookFlag() { g_insideInjectionHook = false; }
    } resetHookFlag;

    void* caller = __builtin_return_address(0);
    bool callerIsExplorer =
        ModuleFromAddress(caller) == GetModuleHandleW(nullptr);

    // A deferred release is recognized from pending state rather than stack
    // provenance because the observed key-up path is dispatched later through
    // Explorer's message loop and no longer carries InputHost.dll on its stack.
    if (callerIsExplorer && IsAllKeyboardUp(inputs, count)) {
        InjectRequest bufferedRequest{};
        BufferedReleaseResult releaseResult =
            ConsumeKeyboardRelease(inputs, count, &bufferedRequest);
        if (releaseResult == BufferedReleaseResult::Partial) {
            SetLastError(incomingLastError);
            return 1;
        }
        if (releaseResult == BufferedReleaseResult::Complete) {
            HWND target = reinterpret_cast<HWND>(
                static_cast<uintptr_t>(bufferedRequest.targetHwnd));
            if (!ValidateTargetForKind(InputKind::Keyboard, target,
                                       bufferedRequest.targetPid)) {
                SetLastError(incomingLastError);
                return 1;
            }
            return FinishForwardedKeyboardRequest(bufferedRequest,
                                                  incomingLastError, false);
        }
    }

    bool balanced = IsBalancedKeyboardSequence(inputs, count);

    // Complete twinui/NInput actions (for example Win+arrow and media actions)
    // are already self-contained and can be relayed immediately.
    if (balanced && AddressIsFromModule(caller, L"twinui.dll") &&
        (StackContainsModule(L"NInput.dll") ||
         StackContainsModule(L"InputHost.dll"))) {
        HWND target = nullptr;
        DWORD targetPid = 0;
        DWORD targetIntegrity = 0;
        if (GetHigherIntegrityKeyboardTarget(&target, &targetPid,
                                             &targetIntegrity)) {
            InjectRequest request{};
            request.magic = kProtocolMagic;
            request.version = kProtocolVersion;
            request.kind = InputKind::Keyboard;
            request.count = count;
            request.targetPid = targetPid;
            request.targetHwnd = static_cast<uint64_t>(
                reinterpret_cast<uintptr_t>(target));
            memcpy(request.payload.keyboard, inputs,
                   sizeof(KEYBDINPUT) * count);

            Wh_Log(L"Explorer: forwarding complete twinui touchpad keyboard action (%u records) to elevated pid=%lu IL=0x%lX",
                   count, targetPid, targetIntegrity);
            return FinishForwardedKeyboardRequest(request,
                                                  incomingLastError, true);
        }
    }

    // A future shell build may emit a complete Explorer/InputHost keyboard
    // action in one call. Handle that without relying on the split-state path.
    if (callerIsExplorer && balanced &&
        StackContainsModule(L"InputHost.dll")) {
        HWND target = nullptr;
        DWORD targetPid = 0;
        DWORD targetIntegrity = 0;
        if (GetHigherIntegrityKeyboardTarget(&target, &targetPid,
                                             &targetIntegrity)) {
            InjectRequest request{};
            request.magic = kProtocolMagic;
            request.version = kProtocolVersion;
            request.kind = InputKind::Keyboard;
            request.count = count;
            request.targetPid = targetPid;
            request.targetHwnd = static_cast<uint64_t>(
                reinterpret_cast<uintptr_t>(target));
            memcpy(request.payload.keyboard, inputs,
                   sizeof(KEYBDINPUT) * count);
            return FinishForwardedKeyboardRequest(request,
                                                  incomingLastError, true);
        }
    }

    // Explorer/InputHost custom shortcuts and some built-in actions issue all
    // key-down records first, then dispatch matching key-up records later.
    if (callerIsExplorer && IsAllKeyboardDown(inputs, count) &&
        StackContainsModule(L"InputHost.dll")) {
        HWND target = nullptr;
        DWORD targetPid = 0;
        DWORD targetIntegrity = 0;
        if (GetHigherIntegrityKeyboardTarget(&target, &targetPid,
                                             &targetIntegrity)) {
            BufferKeyboardDown(inputs, count, target, targetPid);
            Wh_Log(L"Explorer: buffered %u touchpad keyboard-down records for elevated pid=%lu IL=0x%lX",
                   count, targetPid, targetIntegrity);
            SetLastError(incomingLastError);
            return 1;
        }
    }

    SetLastError(incomingLastError);
    return g_ntUserInjectKeyboardInputOriginal(inputs, count);
}

UINT WINAPI NtUserInjectMouseInput_Hook(const MOUSEINPUT* inputs,
                                        UINT32 count) {
    if (g_insideInjectionHook || !inputs || count == 0 ||
        count > kMaxMouseInputs) {
        return g_ntUserInjectMouseInputOriginal(inputs, count);
    }

    DWORD incomingLastError = GetLastError();
    if (g_explorerStopping.load(std::memory_order_relaxed) ||
        GetShellExplorerProcessId() != g_explorerPid) {
        SetLastError(incomingLastError);
        return g_ntUserInjectMouseInputOriginal(inputs, count);
    }

    g_insideInjectionHook = true;
    struct ResetHookFlag {
        ~ResetHookFlag() { g_insideInjectionHook = false; }
    } resetHookFlag;

    void* caller = __builtin_return_address(0);
    bool callerIsExplorer =
        ModuleFromAddress(caller) == GetModuleHandleW(nullptr);

    // The observed direct touchpad mouse path uses one record per down/up call.
    if (callerIsExplorer && count == 1) {
        MouseButtonKind upKind = GetMouseUpKind(inputs[0]);
        if (upKind != MouseButtonKind::None) {
            InjectRequest bufferedRequest{};
            if (ConsumeMouseRelease(inputs[0], &bufferedRequest)) {
                HWND target = reinterpret_cast<HWND>(
                    static_cast<uintptr_t>(bufferedRequest.targetHwnd));
                if (!ValidateTargetForKind(InputKind::Mouse, target,
                                           bufferedRequest.targetPid)) {
                    SetLastError(incomingLastError);
                    return 1;
                }
                return FinishForwardedMouseRequest(bufferedRequest,
                                                   incomingLastError, false);
            }
        }

        MouseButtonKind downKind = GetMouseDownKind(inputs[0]);
        if (downKind != MouseButtonKind::None &&
            StackContainsModule(L"InputHost.dll")) {
            HWND target = nullptr;
            DWORD targetPid = 0;
            DWORD targetIntegrity = 0;
            if (GetHigherIntegrityMouseTarget(&target, &targetPid,
                                              &targetIntegrity)) {
                BufferMouseDown(inputs[0], target, targetPid);
                Wh_Log(L"Explorer: buffered touchpad mouse-down for elevated pid=%lu IL=0x%lX",
                       targetPid, targetIntegrity);
                SetLastError(incomingLastError);
                return 1;
            }
        }
    }

    // Keep support for a future shell build that emits a complete middle/X
    // click in one call rather than splitting down/up.
    if (callerIsExplorer && IsBalancedMouseSequence(inputs, count) &&
        StackContainsModule(L"InputHost.dll")) {
        HWND target = nullptr;
        DWORD targetPid = 0;
        DWORD targetIntegrity = 0;
        if (GetHigherIntegrityMouseTarget(&target, &targetPid,
                                          &targetIntegrity)) {
            InjectRequest request{};
            request.magic = kProtocolMagic;
            request.version = kProtocolVersion;
            request.kind = InputKind::Mouse;
            request.count = count;
            request.targetPid = targetPid;
            request.targetHwnd = static_cast<uint64_t>(
                reinterpret_cast<uintptr_t>(target));
            memcpy(request.payload.mouse, inputs,
                   sizeof(MOUSEINPUT) * count);
            return FinishForwardedMouseRequest(request,
                                               incomingLastError, true);
        }
    }

    SetLastError(incomingLastError);
    return g_ntUserInjectMouseInputOriginal(inputs, count);
}

bool HookWin32uInjectionBoundaries() {
    HMODULE win32u = GetModuleHandleW(L"win32u.dll");
    if (!win32u) {
        win32u = LoadLibraryW(L"win32u.dll");
    }
    if (!win32u) {
        return false;
    }

    void* keyboard = reinterpret_cast<void*>(
        GetProcAddress(win32u, "NtUserInjectKeyboardInput"));
    void* mouse = reinterpret_cast<void*>(
        GetProcAddress(win32u, "NtUserInjectMouseInput"));
    if (!keyboard || !mouse) {
        Wh_Log(L"Explorer: required win32u injection exports aren't available");
        return false;
    }

    if (!Wh_SetFunctionHook(
            keyboard,
            reinterpret_cast<void*>(NtUserInjectKeyboardInput_Hook),
            reinterpret_cast<void**>(&g_ntUserInjectKeyboardInputOriginal)) ||
        !Wh_SetFunctionHook(
            mouse,
            reinterpret_cast<void*>(NtUserInjectMouseInput_Hook),
            reinterpret_cast<void**>(&g_ntUserInjectMouseInputOriginal))) {
        return false;
    }

    return true;
}

bool InitializeExplorerHook() {
    g_explorerPid = GetCurrentProcessId();
    g_explorerStopping.store(false, std::memory_order_relaxed);
    if (!ProcessIdToSessionId(g_explorerPid, &g_sessionId)) {
        Wh_Log(L"Explorer: ProcessIdToSessionId failed: %lu", GetLastError());
        return false;
    }

    g_explorerIntegrityRid = QueryCurrentIntegrityRid();
    if (!g_explorerIntegrityRid) {
        Wh_Log(L"Explorer: couldn't determine current integrity level");
        return false;
    }

    if (!HookWin32uInjectionBoundaries()) {
        return false;
    }

    // The shell window often doesn't exist when Windhawk loads a new Explorer.
    // Keep the two hooks installed: their early gate follows shell registration
    // without a polling thread or installing/removing hooks during runtime.
    Wh_Log(L"Explorer: touchpad relay hooks installed (pid=%lu shellPid=%lu session=%lu IL=0x%lX)",
           g_explorerPid, GetShellExplorerProcessId(), g_sessionId,
           g_explorerIntegrityRid);
    return true;
}

}  // namespace

BOOL WhToolLauncher_ModInit(BOOL* launch) {
    // Windhawk's session manager launches the UIAccess host. The header's
    // legacy launcher would otherwise spawn another copy of Explorer here.
    *launch = FALSE;
    return DetectProcessMode() == ProcessMode::Explorer &&
                   InitializeExplorerHook()
               ? TRUE
               : FALSE;
}

void WhToolLauncher_ModSettingsChanged(BOOL* reload) {
    *reload = FALSE;
}

void WhToolLauncher_ModBeforeUninit() {
    // Windhawk disables the hooks after this callback. New invocations during
    // that transition take the native path; calls already in progress may finish.
    g_explorerStopping.store(true, std::memory_order_relaxed);
}

void WhToolLauncher_ModUninit() {
    AcquireSRWLockExclusive(&g_pendingLock);
    ClearPendingKeyboardLocked();
    ClearPendingMouseLocked();
    ReleaseSRWLockExclusive(&g_pendingLock);
}

BOOL WhTool_ModInit() {
    return DetectProcessMode() == ProcessMode::UiAccessWorker &&
                   InitializeUiAccessWorker()
               ? TRUE
               : FALSE;
}

void WhTool_ModUninit() {
    StopUiAccessWorker();
}
