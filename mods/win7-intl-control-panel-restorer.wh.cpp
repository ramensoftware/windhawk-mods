// ==WindhawkMod==
// @id              win7-intl-control-panel-restorer
// @name            Windows 7 Region and Language Restorer
// @description     This mod restores the classic Windows 7 Region and Language Control Panel pages on Windows 10 and 11
// @version         1.0.0
// @author          babamohammed
// @github          https://github.com/babamohammed2022
// @include         explorer.exe
// @include         control.exe
// @include         rundll32.exe
// @architecture    amd64
// @compilerOptions -lbcrypt -lwinhttp -luser32 -lshell32 -ladvapi32 -lole32 -lpsapi -lversion
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Windows 7 Region and Language Restorer

This mod restores the classic Windows 7 "Region and Language" Control Panel page on Windows 10 and Windows 11.

---
## Screenshot 

![region.PNG](https://raw.githubusercontent.com/babamohammed2022/babamohammed2022/main/region.PNG)

## Functionality

- The mod restores the classic Region page with its four tabs: Formats, Location, Keyboards and Languages, and Administrative.
- All changes are applied through the standard Windows controls and nothing is simulated.
- The restored dialogs use per-monitor DPI awareness on their UI thread, so scaling follows the active monitor without changing Explorer system-wide.
- The interface is available in 20 languages and tries to follow the system language automatically. This covers the Region pages and the Text Services and Input Languages dialogs (input.dll): its pages and dialogs are rebuilt from verified embedded templates and its runtime strings are served through a LoadStringW hook, so the whole restored UI follows the selected language.

---

## Requirements

- 64-bit Windows 10 or Windows 11 (ARM64 and Windows Server are not supported).
- On first use, the original Windows 7 components (intl.cpl and input.dll, about 620 KB) are downloaded automatically from Microsoft's public symbol server and verified by size and SHA-256 before use. An Internet connection is needed only for this step; afterwards everything works offline from the mod's private cache.
- The download runs on a background thread, and opening the Control Panel folder (which merely enumerates the applet) never waits for it. Only actually opening Region waits, and at most a few seconds in total: if the download has not finished by then, that one opening shows the normal modern Region page and the classic page appears from the next one on. Once the payloads are in the private cache, nothing waits at all - including the very first enumeration after a restart. Nothing is executed before it has been verified.

---

## Settings

The mod includes a series of settings:
- **UI language** (default: Automatic): this setting controls the language of the restored page. Automatic follows the Windows display language. It takes effect the next time the page is opened.
- **Redirect Settings pages** (default: off): this setting opens the classic page instead of the modern Settings Region pages. The redirect follows the same defensive, pass-through approach as the reference Settings-to-Control-Panel mod and covers ShellExecuteExW, ShellExecuteW, and CreateProcessW launches from Explorer. Only Region/Language URIs are redirected; unrelated Settings pages pass through unchanged. It takes effect immediately.
- Neither setting reloads the mod, so changing them while a restored dialog is open does not tear anything down.

---

## Notes

- This modification has been tested on Windows 10 21H2 , Windows 11 24H2 and Windows 11 25H2.
- This modification is a best-effort reimplementation of the Windows 7 intl.cpl (Region and Language Control Panel) on Windows 10/11. While it aims to restore the classic experience, 100% feature parity with the original Windows 7 component is not guaranteed. Some Windows 7 features no longer exist on modern Windows and are redirected to their closest modern equivalents.
- Some Windows 7 features no longer exist on modern Windows. In those cases the closest modern equivalent is opened instead (for example, the "Default location" link opens the Location privacy page).
- Settings that were already applied are kept after the mod is disabled.
- Windows system files **are not modified** and the modern intl.cpl is used as a fallback.
- Changing a setting never reloads the mod: the redirect option is a runtime flag and a language change is applied the next time the page is opened.
- Disabling the mod asks any restored dialog to close and, after a few seconds, closes the ones it created itself (its property sheets and modal dialogs). It then waits for them to be gone before it unloads, because the restored component still points into the mod while a dialog is open. In practice that means: if you disable the mod while the Region page is open, the page closes; if the classic page is waiting on a UAC prompt, answering or dismissing that prompt is what finishes the unload.

---

## Known limitations

- This modification is a best-effort reimplementation of a NT 6.1 binary file. Some translations might not be completely accurate to the original files as the mod provides them by itself.
- Display-language installation is not available. It depends on components that exist only on Windows 7.
- Inside input.dll, the Chinese IME hotkey-action descriptions and the English fallback keyboard-layout names are not translated; they are rare and mostly language-neutral. Key-cap labels on the Keyboard Layout Preview (Tab, Caps, Shift, Enter, BackSp) intentionally stay English to fit the original geometry.
- **Stability risk of running a Windows 7 binary on Windows 10/11.** The original `intl.cpl` is executed in-process, inside `explorer.exe`/`control.exe`, through a compatibility layer (a private import table, a Windows 7 version answer, and a private copy of the CRT heap calls it needs). This mod deliberately has **no crash guard**: it does not install any exception handler and does not swallow faults. A genuine C++ exception from the legacy component is caught and answered by falling back to the modern page, but a hardware fault (an access violation, for example) inside that 2010-era binary is **not** contained and can take the host process down - in Explorer's case, restarting the shell. The component is the unmodified, hash-verified Microsoft binary and is exercised through the same code paths Windows 7 used, but it is being run on an OS it was never built for, and that risk cannot be engineered away from user mode. If you are not comfortable with it, do not use this mod.


---

## Opening the page

While the mod is active, the restored page can also be opened directly:

- `control.exe intl.cpl` - the classic Region page.
- `control.exe intl.cpl,,3` - the page opened on Administrative.

---

## Credits

- AdministratoX – Testing on Windows 11 25H2
- m417z - Code review

---
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- language: auto
  $name: UI language
  $description: This setting controls the language of the restored Region UI. Automatic follows your Windows display language. It applies from the next time the page is opened; the mod is not reloaded.
  $options:
  - auto: Automatic (follow Windows)
  - en-US: English (genuine Microsoft)
  - it-IT: Italiano
  - de-DE: Deutsch
  - fr-FR: Français
  - es-ES: Español
  - pt-BR: Português (Brasil)
  - nl-NL: Nederlands
  - pl-PL: Polski
  - ru-RU: Русский
  - zh-CN: 中文简体
  - ja-JP: 日本語
  - ko-KR: 한국어
  - tr-TR: Türkçe
  - cs-CZ: Čeština
  - hu-HU: Magyar
  - ro-RO: Română
  - sv-SE: Svenska
  - uk-UA: Українська
  - el-GR: Ελληνικά
  - ar-SA: العربية (RTL)
- redirectSettings: false
  $name: Redirect Settings pages
  $description: This setting redirects the modern Settings Region and Language pages (ms-settings:regionlanguage, ms-settings:regionformatting) to the restored classic Region dialog. It applies to links opened from Explorer; Date & time, Speech and Typing pages are never touched. It applies immediately; the mod is not reloaded.
*/
// ==/WindhawkModSettings==

// ============================================================================
// Provenance: the pinned Win7 binaries (intl.cpl, input.dll) and their
// SHA-256 hashes, download URLs, PE-contract values, and version numbers
// are verified by the mod implementation below. Call-site
// evidence for each override is cited at the Private*/Shim* function.
// ============================================================================

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0A00
#endif
#ifndef WINVER
#define WINVER 0x0A00
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <new>
#include <cpl.h>
#include <commctrl.h>
#include <shellapi.h>
#include <objbase.h>
#include <evntprov.h>
#include <bcrypt.h>
#include <winhttp.h>
#include <psapi.h>
#include <winver.h>
#include <algorithm>
#include <atomic>
#include <cstring>
#include <cwchar>
#include <cwctype>
#include <string>
#include <vector>
#include <utility>
#include <stdexcept>
#include <windhawk_utils.h>

#if !defined(__x86_64__) && !defined(_M_X64)
#error Only native AMD64 is supported.
#endif
namespace IntlRestore {
// DPI awareness is applied per UI thread, never process-wide. This avoids
// changing Explorer's global DPI policy while ensuring restored dialogs use the
// monitor's effective scale factor.
#ifndef DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2
#define DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2 ((HANDLE)-4)
#endif
void TranslateInputWindow(HWND window);

// SetThreadDpiAwarenessContext is a static user32 import only from build
// 14393 (1607) onward, but Environment() accepts builds from 10240, so a
// static reference would fail the whole mod DLL to load on 10240-14392.
// Resolve it dynamically instead; on builds where it's missing this is a
// silent no-op, same as when the API is present but PER_MONITOR_AWARE_V2
// isn't understood yet (finding 7).
using SetThreadDpiAwarenessContextProc = DPI_AWARENESS_CONTEXT(WINAPI*)(DPI_AWARENESS_CONTEXT);
SetThreadDpiAwarenessContextProc ResolveSetThreadDpiAwarenessContext() {
    static SetThreadDpiAwarenessContextProc proc = reinterpret_cast<SetThreadDpiAwarenessContextProc>(
        reinterpret_cast<void*>(GetProcAddress(GetModuleHandleW(L"user32.dll"), "SetThreadDpiAwarenessContext")));
    return proc;
}
struct DpiScope {
    DPI_AWARENESS_CONTEXT previous = nullptr;
    bool changed = false;
    DpiScope() {
        if (auto proc = ResolveSetThreadDpiAwarenessContext()) {
            previous = proc(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
            changed = previous != nullptr;
        }
    }
    ~DpiScope() {
        if (changed) {
            if (auto proc = ResolveSetThreadDpiAwarenessContext()) proc(previous);
        }
    }
    DpiScope(const DpiScope&) = delete;
    DpiScope& operator=(const DpiScope&) = delete;
};

enum class Host { Control, Rundll32, Explorer, Other };
wchar_t Fold(wchar_t c) { return c >= L'A' && c <= L'Z' ? c + (L'a' - L'A') : c; }
bool Equal(const std::wstring& a, const std::wstring& b) {
    if (a.size() != b.size()) return false;
    for (size_t i = 0; i < a.size(); ++i) if (Fold(a[i]) != Fold(b[i])) return false;
    return true;
}
std::wstring Slashes(std::wstring s) {
    for (auto& c : s) if (c == L'/') c = L'\\';
    if (s.rfind(L"\\\\?\\", 0) == 0) s.erase(0, 4);
    return s;
}
bool SystemFileToken(const std::wstring& token, const std::wstring& file,
                     const std::wstring& system) {
    const auto s = Slashes(token);
    return Equal(s, file) || Equal(s, Slashes(system) + L"\\" + file);
}
bool IntlToken(const std::wstring& token, const std::wstring& system) {
    // Commas belong to the CPL suffix, not to its file name. A path to an
    // unrelated/private third-party intl.cpl is not a request for the system CPL.
    return SystemFileToken(token.substr(0, token.find(L',')), L"intl.cpl", system);
}
bool SelectedLaunch(Host host, const std::vector<std::wstring>& argv,
                    const std::wstring& system) {
    // Explorer hosts the Control Panel namespace; a Region activation may run
    // in-process there. Its command line carries no CPL selection, so any
    // native explorer.exe session is accepted; the CPL hook only fires if
    // Region is actually invoked inside that process.
    if (host == Host::Explorer) return true;
    if (argv.size() < 2) return false;
    if (host == Host::Control) {
        if (IntlToken(argv[1], system)) return true;
        return argv.size() >= 3 && Equal(argv[1], L"/name") &&
            (Equal(argv[2], L"Microsoft.RegionAndLanguage") ||
             Equal(argv[2], L"Microsoft.RegionalAndLanguageOptions"));
    }
    if (host == Host::Rundll32 && argv.size() >= 3) {
        const auto comma = argv[1].find(L',');
        if (comma == std::wstring::npos ||
            !SystemFileToken(argv[1].substr(0, comma), L"shell32.dll", system)) return false;
        const auto entry = argv[1].substr(comma + 1);
        if (entry != L"Control_RunDLL" && entry != L"Control_RunDLLW" &&
            entry != L"Control_RunDLLA") return false;
        return IntlToken(argv[2], system);
    }
    return false;
}
using CplProc = LONG(CALLBACK*)(HWND, UINT, LPARAM, LPARAM);
using EntryProc = BOOL(WINAPI*)(HINSTANCE, DWORD, LPVOID);
using PSProc = INT_PTR(WINAPI*)(LPCPROPSHEETHEADERW);
using PageProc = HPROPSHEETPAGE(WINAPI*)(LPCPROPSHEETPAGEW);
using DialogProc = INT_PTR(WINAPI*)(HINSTANCE, LPCWSTR, HWND, DLGPROC, LPARAM);
using RtlVersionProc = LONG(WINAPI*)(OSVERSIONINFOEXW*);
using RtlAddTableProc = BOOLEAN(NTAPI*)(PRUNTIME_FUNCTION, DWORD, DWORD64);
using RtlDeleteTableProc = BOOLEAN(NTAPI*)(PRUNTIME_FUNCTION);
// Pinned payloads (proven: each URL downloads  content).
constexpr PCWSTR kIntlSha = L"fb7a1d08fe2ae16741ba6b0b7527528147b56c6a6307608076108ffcfef0dadd";
constexpr PCWSTR kInputSha = L"db91ab6cd37eb0131e2c9d4789833910cd3cabd5b00db3f96e95ab3fdaac9801";
constexpr PCWSTR kIntlUrl = L"https://msdl.microsoft.com/download/symbols/intl.cpl/4CE7C6D461000/intl.cpl";
constexpr PCWSTR kInputUrl = L"https://msdl.microsoft.com/download/symbols/input.dll/4A5BDF4F40000/input.dll";
constexpr DWORD kIntlSize = 373248;
constexpr DWORD kInputSize = 246784;
constexpr size_t kMaxFile = 4 * 1024 * 1024;
constexpr GUID kNlsProvider = {0x3aa52b8b, 0x6357, 0x4c18,
                              {0xa9, 0x2e, 0xb5, 0x3f, 0xb1, 0x77, 0x85, 0x3b}};

struct Handle {
    HANDLE value = INVALID_HANDLE_VALUE;
    Handle() = default;
    explicit Handle(HANDLE h) : value(h) {}
    ~Handle() { if (value && value != INVALID_HANDLE_VALUE) CloseHandle(value); }
    Handle(const Handle&) = delete;
    Handle& operator=(const Handle&) = delete;
    Handle(Handle&& other) noexcept : value(std::exchange(other.value, INVALID_HANDLE_VALUE)) {}
    Handle& operator=(Handle&& other) noexcept {
        if (this != &other) {
            if (value && value != INVALID_HANDLE_VALUE) CloseHandle(value);
            value = std::exchange(other.value, INVALID_HANDLE_VALUE);
        }
        return *this;
    }
    explicit operator bool() const { return value && value != INVALID_HANDLE_VALUE; }
};
// Tracks the WinHTTP session handle currently owned by an in-flight
// Download(), so an unload request can close it out from under a blocked call
// instead of waiting out its connect/send/receive timeouts (finding 3).
// g_cancelledSession remembers WHICH session the unload path closed, so the
// owner can tell "I still have to close this tree" from "this tree is already
// gone" (finding 4).
SRWLOCK g_downloadLock = SRWLOCK_INIT;
HINTERNET g_activeSession = nullptr;
HINTERNET g_cancelledSession = nullptr;
// One WinHTTP handle tree (session -> connect -> request) for a single
// Download() call. Ownership is settled under g_downloadLock in the
// destructor, because closing a session destroys its connect/request children
// with it: if CancelInFlightDownload() already closed the session, closing
// those two again would be a double-close. explorer.exe uses WinHTTP itself,
// so a recycled handle value would let this mod close an unrelated
// component's handle. Exactly one side ever closes each handle (finding 4).
struct HttpSession {
    HINTERNET session = nullptr;
    HINTERNET connect = nullptr;
    HINTERNET request = nullptr;
    HttpSession() = default;
    HttpSession(const HttpSession&) = delete;
    HttpSession& operator=(const HttpSession&) = delete;
    ~HttpSession() {
        bool closedByUnload = false;
        AcquireSRWLockExclusive(&g_downloadLock);
        if (session && session == g_cancelledSession) { closedByUnload = true; g_cancelledSession = nullptr; }
        if (session && session == g_activeSession) g_activeSession = nullptr;
        ReleaseSRWLockExclusive(&g_downloadLock);
        if (closedByUnload) return; // the whole tree died with the session
        if (request) WinHttpCloseHandle(request);
        if (connect) WinHttpCloseHandle(connect);
        if (session) WinHttpCloseHandle(session);
    }
};
void CancelInFlightDownload() {
    AcquireSRWLockExclusive(&g_downloadLock);
    if (g_activeSession) {
        const HINTERNET session = g_activeSession;
        g_activeSession = nullptr;
        // Publish the identity BEFORE closing: Download()'s destructor may run
        // the moment the blocked call fails, and it decides ownership by
        // matching this value.
        g_cancelledSession = session;
        WinHttpCloseHandle(session);
    }
    ReleaseSRWLockExclusive(&g_downloadLock);
}
struct ActScope {
    ULONG_PTR cookie = 0;
    bool active = false;
    explicit ActScope(HANDLE act) {
        if (act && act != INVALID_HANDLE_VALUE) active = ActivateActCtx(act, &cookie) != FALSE;
    }
    ~ActScope() { if (active) DeactivateActCtx(0, cookie); }
};
// Logging and probes must not clobber the caller's LastError: hold one of
// these across any Wh_Log/diagnostic block on a hook or teardown path.
struct LastErrorScope {
    DWORD saved;
    LastErrorScope() : saved(GetLastError()) {}
    ~LastErrorScope() { SetLastError(saved); }
    LastErrorScope(const LastErrorScope&) = delete;
    LastErrorScope& operator=(const LastErrorScope&) = delete;
};
// LocalAlloc-family RAII (CommandLineToArgvW and friends): an exception or
// early return between acquire and release must never leak.
struct LocalMem {
    HLOCAL value = nullptr;
    explicit LocalMem(HLOCAL h) : value(h) {}
    ~LocalMem() { if (value) LocalFree(value); }
    LocalMem(const LocalMem&) = delete;
    LocalMem& operator=(const LocalMem&) = delete;
    explicit operator bool() const { return value != nullptr; }
};
struct ReadFile {
    Handle pin;
    std::vector<BYTE> bytes;
};
struct Image {
    BYTE* base = nullptr;
    DWORD size = 0;
    EntryProc entry = nullptr;
    CplProc cpl = nullptr;
    PRUNTIME_FUNCTION functions = nullptr;
    bool functionTable = false;
    bool attached = false;
};
struct IatPatch { ULONG_PTR* slot; ULONG_PTR before; ULONG_PTR after; };
struct Dep { std::string name; HMODULE module; };
struct Blob { const BYTE* data = nullptr; DWORD size = 0; };
std::wstring g_system, g_windows, g_cache, g_intlPath, g_inputPath;
DWORD g_build = 0;
ReadFile g_intlFile, g_inputFile;
std::vector<Dep> g_deps;
std::vector<IatPatch> g_inputPatches;
Image g_image;
HMODULE g_nativeModule = nullptr, g_inputModule = nullptr;
// Balances the extra references PrivateLoadLibraryW hands to the legacy
// provider for "input.dll" (each call bumps input.dll's refcount via
// LoadLibraryExW). PrivateFreeLibrary decrements when the legacy code
// releases one; Cleanup() releases whatever the legacy code never gave back.
std::atomic<LONG> g_inputExtraRefs{0};
HANDLE g_act = INVALID_HANDLE_VALUE, g_idle = nullptr, g_jobsIdle = nullptr;
CplProc g_nativeCpl = nullptr;
PSProc g_mainPS = nullptr, g_inputPS = nullptr;
PageProc g_mainPage = nullptr;
PageProc g_inputPage = nullptr;
DialogProc g_inputDialog = nullptr;
RtlAddTableProc g_rtlAddTable = nullptr;
RtlDeleteTableProc g_rtlDeleteTable = nullptr;
std::atomic<bool> g_stopping{false};
// Thread ID that ran the legacy CPL_INIT (and therefore owns whatever
// thread-affine COM/TSF state the Win7 provider created there). Cleanup()
// marshals its CPL_EXIT/DLL_PROCESS_DETACH calls to this thread instead of
// running them on the arbitrary Windhawk unload thread (finding 5).
std::atomic<DWORD> g_cplInitThreadId{0};
std::atomic<bool> g_legacyInitialized{false};
std::atomic<bool> g_nativeInitialized{false};
std::atomic<bool> g_useLegacy{true};
std::atomic<LONG> g_active{0};
std::atomic<LONG> g_jobs{0};
SRWLOCK g_gate = SRWLOCK_INIT;
SRWLOCK g_windowsLock = SRWLOCK_INIT;
// Windows this mod created and serves: property sheets (SheetCallback) and
// modal dialogs (DialogCallback). The `sheet` flag is what lets the unload path
// escalate correctly - a sheet is closed by pressing its Cancel button, a modal
// dialog by EndDialog from its own thread (see CloseOwnedWindows).
struct OwnedWindow { HWND hwnd = nullptr; bool sheet = false; };
std::vector<OwnedWindow> g_owned;
constexpr size_t kMaxOwnedWindows = 64;
SRWLOCK g_threadsLock = SRWLOCK_INIT;
std::vector<HANDLE> g_threads;
constexpr size_t kMaxTrackedThreads = 256;
// ===== Unload rundown =====
// The waits on the unload path are NOT bounded, and that is deliberate.
// Windhawk unmaps this mod's image with a single FreeLibrary the moment
// Wh_ModUninit returns - unconditionally - and at that moment the mapped Win7
// provider still points into it from four directions:
//   * its compatibility IAT: BindImports(g_image, true) binds ~40 imports to
//     mod code (PrivateMalloc/PrivateFree/PrivateLoadStringW/MainPropertySheet/
//     MainDialogBox/PrivateShellExecuteExW/PrivateCreateThread/...);
//   * every dialog and sheet this mod serves: DialogCallback and SheetCallback
//     are installed as the real DLGPROC / PFNPROPSHEETCALLBACK, so the next
//     message a still-open Region sheet receives goes into unmapped memory;
//   * ThreadBridge under every private worker that has not returned;
//   * the return address of any thread still inside a legacy call.
// So "give up after N seconds and leave the provider mapped" is not a safe
// degradation: it converts a recoverable hang into a host crash. Instead the
// unload path removes the reasons the wait can be long - see the escalation in
// Wh_ModBeforeUninit/CloseOwnedWindows - and Wh_ModSettingsChanged no longer
// forces a reload, so editing a setting does not run this path at all.
//
// How long the UI is asked to take before it is force-closed.
constexpr DWORD kUnloadEscalateMs = 3000;
// Shorter budget for the job wait when it runs on a UI thread (CPL_EXIT). Safe
// to bound there: Cleanup() still joins everything unconditionally before the
// image can be unmapped, so timing out here only defers the wait.
constexpr DWORD kUiJobWaitMs = 5000;
// Set once the unload path starts force-closing the UI this mod created.
// DialogCallback honours it by calling EndDialog from the dialog's own thread.
std::atomic<bool> g_forceCloseUi{false};
void RegisterThread(HANDLE thread) {
    HANDLE dup = nullptr;
    if (!DuplicateHandle(GetCurrentProcess(), thread, GetCurrentProcess(), &dup, 0, FALSE,
                         DUPLICATE_SAME_ACCESS)) return;
    AcquireSRWLockExclusive(&g_threadsLock);
    if (g_threads.size() < kMaxTrackedThreads) g_threads.push_back(dup);
    else { ReleaseSRWLockExclusive(&g_threadsLock); CloseHandle(dup); return; }
    ReleaseSRWLockExclusive(&g_threadsLock);
}
// Waits for every tracked private worker thread to actually return (not just
// signal EndJob) before the caller unmaps/frees the image those threads'
// epilogues and thread-start thunks still live in (finding 4). The unload path
// passes INFINITE on purpose: ThreadBridge is mod code, so a worker that has
// not returned means a live frame in the image Windhawk is about to unmap.
// Returns false only on WAIT_FAILED, or on timeout for a caller that supplied
// one (the CPL_EXIT UI path, which Cleanup() still joins afterwards).
bool JoinTrackedThreads(DWORD timeoutMs) {
    std::vector<HANDLE> copy;
    AcquireSRWLockExclusive(&g_threadsLock);
    copy.swap(g_threads);
    ReleaseSRWLockExclusive(&g_threadsLock);
    const bool unbounded = timeoutMs == INFINITE;
    const ULONGLONG deadline = unbounded ? 0 : GetTickCount64() + timeoutMs;
    bool joined = true;
    for (size_t i = 0; i < copy.size(); i += MAXIMUM_WAIT_OBJECTS) {
        DWORD count = static_cast<DWORD>(std::min<size_t>(MAXIMUM_WAIT_OBJECTS, copy.size() - i));
        DWORD slice = INFINITE;
        if (!unbounded) {
            const ULONGLONG now = GetTickCount64();
            // A zero slice is deliberate: WaitForMultipleObjects then simply
            // reports the current state, so threads that already exited are not
            // mistaken for stuck ones when the budget has run out.
            slice = now >= deadline ? 0
                : static_cast<DWORD>(std::min<ULONGLONG>(deadline - now, 0xFFFFFFFEULL));
        }
        const DWORD wait = WaitForMultipleObjects(count, &copy[i], TRUE, slice);
        if (wait != WAIT_OBJECT_0) {
            Wh_Log(L"%lu private worker thread(s) not joined (wait=0x%lX, budget=%lu ms)",
                   static_cast<unsigned long>(count), static_cast<unsigned long>(wait),
                   static_cast<unsigned long>(timeoutMs));
            joined = false;
            break;
        }
    }
    for (HANDLE h : copy) CloseHandle(h);
    return joined;
}
std::vector<BYTE> g_blobStore[32];
Blob g_blobs[32];
DWORD g_blobCount = 0;
int g_blobForDialog[1024];
int g_blobForStrBlock[64];
std::vector<BYTE> g_inpBlobStore[12];   // rebuilt input.dll dialog templates
Blob g_inpBlobs[12];
DWORD g_inpBlobCount = 0;
int g_inpBlobForDialog[1024];
thread_local unsigned g_uiCreated = 0;
thread_local bool g_uiFailed = false;

void Cleanup();
FARPROC ResolvePrivate(HMODULE module, const char* dll, LPCSTR proc);
HMODULE WINAPI PrivateLoadLibraryW(LPCWSTR name);
FARPROC WINAPI PrivateGetProcAddress(HMODULE module, LPCSTR name);
INT_PTR WINAPI MainPropertySheet(LPCPROPSHEETHEADERW header);
INT_PTR WINAPI InputPropertySheet(LPCPROPSHEETHEADERW header);
HPROPSHEETPAGE WINAPI MainCreatePage(LPCPROPSHEETPAGEW page);
INT_PTR WINAPI MainDialogBox(HINSTANCE, LPCWSTR, HWND, DLGPROC, LPARAM);
INT_PTR WINAPI InputDialogBox(HINSTANCE, LPCWSTR, HWND, DLGPROC, LPARAM);
HANDLE WINAPI PrivateCreateThread(LPSECURITY_ATTRIBUTES, SIZE_T, LPTHREAD_START_ROUTINE, LPVOID, DWORD, LPDWORD);
BOOL WINAPI PrivateSHCreateThread(LPTHREAD_START_ROUTINE, void*, DWORD, LPTHREAD_START_ROUTINE);
BOOL WINAPI PrivateShellExecuteExW(SHELLEXECUTEINFOW*);
BOOL WINAPI PrivateDisableThreadLibraryCalls(HMODULE);
bool WaitForJobs(DWORD timeoutMs = INFINITE);

bool Fail(PCWSTR stage, DWORD error = GetLastError()) {
    Wh_Log(L"ERROR: %s; Win32=%lu (0x%08lX)", stage, error, error);
    SetLastError(error);
    return false;
}
bool Range(size_t offset, size_t length, size_t total) {
    return offset <= total && length <= total - offset;
}
std::wstring FullPath(const std::wstring& input) {
    DWORD n = ExpandEnvironmentStringsW(input.c_str(), nullptr, 0);
    if (!n) return {};
    std::wstring expanded(n, L'\0');
    n = ExpandEnvironmentStringsW(input.c_str(), &expanded[0], n);
    if (!n) return {};
    expanded.resize(n - 1);
    DWORD fullLen = GetFullPathNameW(expanded.c_str(), 0, nullptr, nullptr);
    if (!fullLen) return {};
    std::wstring full(fullLen, L'\0');
    fullLen = GetFullPathNameW(expanded.c_str(), fullLen, &full[0], nullptr);
    if (!fullLen) return {};
    full.resize(fullLen);
    return Slashes(full);
}
bool LocalAbsolute(const std::wstring& s) {
    return s.size() >= 3 && ((s[0] >= L'A' && s[0] <= L'Z') ||
           (s[0] >= L'a' && s[0] <= L'z')) && s[1] == L':' && s[2] == L'\\';
}
bool Directory(const std::wstring& path) {
    if (CreateDirectoryW(path.c_str(), nullptr)) return true;
    if (GetLastError() != ERROR_ALREADY_EXISTS) return Fail(L"CreateDirectory private cache");
    DWORD a = GetFileAttributesW(path.c_str());
    return a != INVALID_FILE_ATTRIBUTES && (a & FILE_ATTRIBUTE_DIRECTORY) &&
           !(a & FILE_ATTRIBUTE_REPARSE_POINT);
}
bool OpenRead(const std::wstring& path, ReadFile& file) {
    file.pin = Handle(CreateFileW(path.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr,
        OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OPEN_REPARSE_POINT, nullptr));
    if (!file.pin) return false;
    FILE_ATTRIBUTE_TAG_INFO info{};
    LARGE_INTEGER size{};
    if (!GetFileInformationByHandleEx(file.pin.value, FileAttributeTagInfo, &info, sizeof(info)) ||
        (info.FileAttributes & (FILE_ATTRIBUTE_DIRECTORY | FILE_ATTRIBUTE_REPARSE_POINT)) ||
        !GetFileSizeEx(file.pin.value, &size) || size.QuadPart <= 0 || static_cast<ULONGLONG>(size.QuadPart) > kMaxFile) {
        SetLastError(ERROR_BAD_FORMAT); return false;
    }
    file.bytes.resize(static_cast<size_t>(size.QuadPart));
    DWORD got = 0;
    return ::ReadFile(file.pin.value, file.bytes.data(), static_cast<DWORD>(file.bytes.size()), &got, nullptr) &&
           got == file.bytes.size();
}
std::wstring Hash(const std::vector<BYTE>& data) {
    BYTE digest[32]{};
    BCRYPT_ALG_HANDLE algorithm = nullptr;
    NTSTATUS status = BCryptOpenAlgorithmProvider(&algorithm, BCRYPT_SHA256_ALGORITHM, nullptr, 0);
    if (status >= 0) {
        status = BCryptHash(algorithm, nullptr, 0, const_cast<PUCHAR>(data.data()),
                            static_cast<ULONG>(data.size()), digest, sizeof(digest));
        BCryptCloseAlgorithmProvider(algorithm, 0);
    }
    if (status < 0) {
        Wh_Log(L"SHA256 NTSTATUS=0x%08lX", static_cast<ULONG>(status)); return {};
    }
    constexpr wchar_t hex[] = L"0123456789abcdef";
    std::wstring result(64, L'0');
    for (size_t i = 0; i < 32; ++i) { result[i * 2] = hex[digest[i] >> 4]; result[i * 2 + 1] = hex[digest[i] & 15]; }
    return result;
}
// WinHttpSetTimeouts is per-operation only, so it bounds a dead connection but
// not a server that trickles bytes: every receive restarts the clock. These
// two budgets bound the whole transfer and any no-progress stretch inside it,
// so the fetch can never outlive them (finding 5).
constexpr ULONGLONG kDownloadStallMs = 10000;   // no new byte for this long -> give up
constexpr ULONGLONG kDownloadTotalMs = 60000;   // whole request, all operations
bool Download(PCWSTR url, std::vector<BYTE>& bytes) {
    URL_COMPONENTS parts{}; parts.dwStructSize = sizeof(parts);
    wchar_t host[256]{}, object[2048]{};
    parts.lpszHostName = host; parts.dwHostNameLength = ARRAYSIZE(host);
    parts.lpszUrlPath = object; parts.dwUrlPathLength = ARRAYSIZE(object);
    if (!WinHttpCrackUrl(url, 0, 0, &parts) || parts.nScheme != INTERNET_SCHEME_HTTPS ||
        !Equal(host, L"msdl.microsoft.com")) return Fail(L"Unapproved payload URL", ERROR_INVALID_NAME);
    if (g_stopping.load(std::memory_order_acquire)) { SetLastError(ERROR_SHUTDOWN_IN_PROGRESS); return false; }
    HttpSession http;
    http.session = WinHttpOpen(L"Windhawk-IntlRestore/1.1.0", WINHTTP_ACCESS_TYPE_AUTOMATIC_PROXY,
                               WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
    if (!http.session) return false;
    // Published for the unload path (CancelInFlightDownload) so a blocked
    // WinHTTP call fails immediately instead of waiting out its timeout.
    AcquireSRWLockExclusive(&g_downloadLock);
    g_activeSession = http.session;
    ReleaseSRWLockExclusive(&g_downloadLock);
    WinHttpSetTimeouts(http.session, 10000, 10000, 15000, 15000);
    http.connect = WinHttpConnect(http.session, host, parts.nPort, 0);
    if (!http.connect) return false;
    http.request = WinHttpOpenRequest(http.connect, L"GET", object, nullptr,
        WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, WINHTTP_FLAG_SECURE);
    if (!http.request) return false;
    // HTTPS redirects only; a digest check is still mandatory before any use.
    DWORD policy = WINHTTP_OPTION_REDIRECT_POLICY_DISALLOW_HTTPS_TO_HTTP;
    WinHttpSetOption(http.request, WINHTTP_OPTION_REDIRECT_POLICY, &policy, sizeof(policy));
    const ULONGLONG started = GetTickCount64();
    if (!WinHttpSendRequest(http.request, WINHTTP_NO_ADDITIONAL_HEADERS, 0,
                           WINHTTP_NO_REQUEST_DATA, 0, 0, 0) || !WinHttpReceiveResponse(http.request, nullptr)) return false;
    DWORD status = 0, size = sizeof(status);
    if (!WinHttpQueryHeaders(http.request, WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER,
                            WINHTTP_HEADER_NAME_BY_INDEX, &status, &size, WINHTTP_NO_HEADER_INDEX) || status != 200) {
        Wh_Log(L"Download HTTP=%lu", status); SetLastError(ERROR_BAD_NET_RESP); return false;
    }
    bytes.clear();
    BYTE chunk[16384];
    for (;;) {
        if (g_stopping.load(std::memory_order_acquire)) { SetLastError(ERROR_SHUTDOWN_IN_PROGRESS); return false; }
        const ULONGLONG readStarted = GetTickCount64();
        DWORD n = 0;
        if (!WinHttpReadData(http.request, chunk, sizeof(chunk), &n)) return false;
        const ULONGLONG now = GetTickCount64();
        // No-progress detector: WinHTTP's receive timeout is per operation, so a
        // server that trickles a few bytes at a time keeps restarting it. A
        // single receive that blocks longer than this budget ends the download.
        if (now - readStarted > kDownloadStallMs)
            return Fail(L"Download stalled: a receive made no progress", ERROR_TIMEOUT);
        if (!n) break;
        if (bytes.size() + n > kMaxFile) return Fail(L"Download exceeds pinned payload limit", ERROR_FILE_TOO_LARGE);
        bytes.insert(bytes.end(), chunk, chunk + n);
        if (now - started > kDownloadTotalMs) return Fail(L"Download exceeded its overall deadline", ERROR_TIMEOUT);
    }
    if (GetTickCount64() - started > kDownloadTotalMs) return Fail(L"Download exceeded its overall deadline", ERROR_TIMEOUT);
    return !bytes.empty();
}
bool WriteAtomic(const std::wstring& path, const std::vector<BYTE>& bytes) {
    const std::wstring temp = path + L".tmp-" + std::to_wstring(GetCurrentProcessId()) +
                             L"-" + std::to_wstring(GetTickCount64());
    {
        Handle file(CreateFileW(temp.c_str(), GENERIC_WRITE, 0, nullptr, CREATE_NEW,
                                FILE_ATTRIBUTE_NORMAL, nullptr));
        if (!file) return false;
        DWORD done = 0;
        if (!::WriteFile(file.value, bytes.data(), static_cast<DWORD>(bytes.size()), &done, nullptr) ||
            done != bytes.size() || !FlushFileBuffers(file.value)) {
            const DWORD e = GetLastError(); file = Handle(); DeleteFileW(temp.c_str()); SetLastError(e); return false;
        }
    }
    if (!MoveFileExW(temp.c_str(), path.c_str(), MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH)) {
        const DWORD e = GetLastError(); DeleteFileW(temp.c_str()); SetLastError(e); return false;
    }
    return true;
}
bool VerifyVersion(const std::wstring& path, DWORD expectMS, DWORD expectLS, std::wstring& actual) {
    actual.clear();
    DWORD handle = 0;
    DWORD bytes = GetFileVersionInfoSizeW(path.c_str(), &handle);
    if (!bytes) return false;
    std::vector<BYTE> info(bytes);
    if (!GetFileVersionInfoW(path.c_str(), 0, bytes, info.data())) return false;
    VS_FIXEDFILEINFO* fixed = nullptr;
    UINT fixedBytes = 0;
    if (!VerQueryValueW(info.data(), L"\\", reinterpret_cast<void**>(&fixed), &fixedBytes) ||
        !fixed || fixedBytes < sizeof(*fixed) || fixed->dwSignature != 0xFEEF04BD)
        return false;
    actual = std::to_wstring(HIWORD(fixed->dwFileVersionMS)) + L"." + std::to_wstring(LOWORD(fixed->dwFileVersionMS)) +
        L"." + std::to_wstring(HIWORD(fixed->dwFileVersionLS)) + L"." + std::to_wstring(LOWORD(fixed->dwFileVersionLS));
    return fixed->dwFileVersionMS == expectMS && fixed->dwFileVersionLS == expectLS;
}
bool EnsurePinned(PCWSTR relative, PCWSTR expected, DWORD expectedSize, PCWSTR url) {
    const std::wstring dest = g_cache + L"\\" + relative;
    {
        ReadFile f;
        if (OpenRead(dest, f) && f.bytes.size() == expectedSize && Hash(f.bytes) == expected) return true;
        if (!f.bytes.empty()) DeleteFileW(dest.c_str()); // Corrupt cache entry: never execute.
    }
    Wh_Log(L"Fetching original Microsoft payload: %s", url);
    std::vector<BYTE> data;
    if (!Download(url, data)) return Fail(L"Download original payload");
    if (data.size() != expectedSize || Hash(data) != expected) {
        Wh_Log(L"Downloaded payload failed size/digest check; not executing it");
        return Fail(L"Downloaded payload verification failed", ERROR_CRC);
    }
    if (!WriteAtomic(dest, data)) {
        // Another selected host might have published the same validated file.
        ReadFile f;
        if (OpenRead(dest, f) && f.bytes.size() == expectedSize && Hash(f.bytes) == expected) return true;
        return Fail(L"Publish verified payload");
    }
    return true;
}

// BEGIN NLS_COMPAT
// Win7 kernel32 NLS implementations live in kernelbase on Win10 (Wine spec
// evidence: commented out in kernel32.spec, present in kernelbase.spec).
// Forwarding keeps the REAL implementation; only genuinely absent APIs get
// honest failure shims below. Setters/policy APIs fail closed (nullptr) when
// the forward target is missing, so locale state is never faked.
FARPROC ForwardKernelBase(LPCSTR name) {
    try {
        HMODULE kb = GetModuleHandleW(L"kernelbase.dll");
        FARPROC proc = kb ? GetProcAddress(kb, name) : nullptr;
        if (!proc) Wh_Log(L"kernelbase.dll!%S missing on this build", name);
        return proc;

    } catch (...) {
        return nullptr;
    }
}
// Fallback only: both call sites (0x9C53, 0xB998, kind=2) check the return and
// skip the ETW write when it is 0. Returning 0 takes that genuine skip path.
ULONG WINAPI FallbackNlsEventDataDescCreate(EVENT_DATA_DESCRIPTOR*, ULONG, void*, ULONG) {
    try {
        SetLastError(ERROR_PROC_NOT_FOUND); return 0;

    } catch (...) {
        return 0;
    }
}
// Fallback only: performs a REAL EventWrite through a temporary registration
// of the genuine NLS provider GUID (found in the binary at RVA 0x2E10) and
// returns the REAL status. No fake success.
ULONG WINAPI FallbackNlsWriteEtwEvent(REGHANDLE handle, const EVENT_DESCRIPTOR* descriptor,
                                     ULONG count, const EVENT_DATA_DESCRIPTOR* data) {
    try {
        if (!descriptor || (count && !data)) return ERROR_INVALID_PARAMETER;
        if (handle) return EventWrite(handle, descriptor, count, const_cast<PEVENT_DATA_DESCRIPTOR>(data));
        REGHANDLE temporary = 0;
        ULONG status = EventRegister(&kNlsProvider, nullptr, nullptr, &temporary);
        if (status != ERROR_SUCCESS) return status;
        EventWrite(temporary, descriptor, count, const_cast<PEVENT_DATA_DESCRIPTOR>(data));
        return EventUnregister(temporary);

    } catch (...) {
        return ERROR_NOT_ENOUGH_MEMORY;
    }
}
// WinSqmAddToStream: 6 args, ~15 sites, return value ignored everywhere.
ULONG WINAPI ShimWinSqmAddToStream(DWORD, DWORD, PVOID, DWORD, PVOID, DWORD) {
    try {
        return 0;

    } catch (...) {
        return 0;
    }
}
// WinSqmSetString: 3 args (0, 0xEB6, str), 1 site, return ignored.
ULONG WINAPI ShimWinSqmSetString(DWORD, DWORD, PCWSTR) {
    try {
        return 0;

    } catch (...) {
        return 0;
    }
}
// RtlGetUILanguageInfo: 5-6 args, NTSTATUS, callers check success.
// Absent on Win10 ntdll -> honest failure so genuine error paths run.
LONG WINAPI ShimRtlGetUILanguageInfo(DWORD, PVOID, PVOID, PVOID, PVOID, PVOID) {
    try {
        return static_cast<LONG>(0xC00000BB); // STATUS_NOT_SUPPORTED

    } catch (...) {
        return static_cast<LONG>(0xC00000BB);
    }
}
// RtlpSetPreferredUILanguages: 3 args (flags 0x408/0x2888/0x3088..., buf,
// status-out), NTSTATUS checked with JL. Honest failure, no state faked.
LONG WINAPI ShimRtlpSetPreferredUILanguages(DWORD, PVOID, PVOID) {
    try {
        return static_cast<LONG>(0xC00000BB); // STATUS_NOT_SUPPORTED

    } catch (...) {
        return static_cast<LONG>(0xC00000BB);
    }
}
// NotifyUILanguageChange: 5 args (1, langlist, prev, NULL, status-out),
// BOOL checked with JE. Honest FALSE: display-language install shows the
// genuine error instead of pretending success.
BOOL WINAPI ShimNotifyUILanguageChange(DWORD, PCWSTR, PCWSTR, DWORD, PVOID) {
    try {
        SetLastError(ERROR_NOT_SUPPORTED); return FALSE;

    } catch (...) {
        SetLastError(ERROR_NOT_SUPPORTED); return FALSE;
    }
}
// GetUILanguageInfo fallback: 5 args, BOOL. Honest FALSE (a query, safe).
BOOL WINAPI ShimGetUILanguageInfo(DWORD, PCWSTR, PVOID, PVOID, PVOID) {
    try {
        SetLastError(ERROR_NOT_SUPPORTED); return FALSE;

    } catch (...) {
        SetLastError(ERROR_NOT_SUPPORTED); return FALSE;
    }
}
// CheckElevationEnabled(PBOOL): 1 site (0xFEB9) uses both the return value
// and the out-param. Real semantics from the documented policy value.
BOOL WINAPI ShimCheckElevationEnabled(PBOOL enabled) {
    try {
        DWORD value = 1; // Win10 default when unreadable: assume enabled (safer:
                         // admin flows still engage rather than silently vanish).
        HKEY key = nullptr;
        if (RegOpenKeyExW(HKEY_LOCAL_MACHINE,
                L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Policies\\System",
                0, KEY_QUERY_VALUE, &key) == ERROR_SUCCESS) {
            DWORD bytes = sizeof(value), type = 0;
            if (RegQueryValueExW(key, L"EnableLUA", nullptr, &type,
                    reinterpret_cast<LPBYTE>(&value), &bytes) != ERROR_SUCCESS ||
                type != REG_DWORD) value = 1;
            RegCloseKey(key);
        }
        if (enabled) *enabled = value ? TRUE : FALSE;
        return TRUE;

    } catch (...) {
        if (enabled) *enabled = TRUE; return TRUE;
    }
}
// END NLS_COMPAT

struct PEView {
    const std::vector<BYTE>& raw;
    const IMAGE_NT_HEADERS64* nt = nullptr;
    const IMAGE_SECTION_HEADER* sections = nullptr;
    bool Validate(bool strictIntl) {
        if (raw.size() < sizeof(IMAGE_DOS_HEADER)) return false;
        auto* dos = reinterpret_cast<const IMAGE_DOS_HEADER*>(raw.data());
        if (dos->e_magic != IMAGE_DOS_SIGNATURE || dos->e_lfanew < 0 ||
            !Range(dos->e_lfanew, sizeof(IMAGE_NT_HEADERS64), raw.size())) return false;
        nt = reinterpret_cast<const IMAGE_NT_HEADERS64*>(raw.data() + dos->e_lfanew);
        if (nt->Signature != IMAGE_NT_SIGNATURE || nt->FileHeader.Machine != IMAGE_FILE_MACHINE_AMD64 ||
            !(nt->FileHeader.Characteristics & IMAGE_FILE_DLL) || nt->OptionalHeader.Magic != IMAGE_NT_OPTIONAL_HDR64_MAGIC ||
            nt->FileHeader.SizeOfOptionalHeader != sizeof(IMAGE_OPTIONAL_HEADER64) ||
            nt->FileHeader.NumberOfSections == 0 || nt->FileHeader.NumberOfSections > 96 ||
            nt->OptionalHeader.NumberOfRvaAndSizes < IMAGE_NUMBEROF_DIRECTORY_ENTRIES ||
            !nt->OptionalHeader.SizeOfImage || nt->OptionalHeader.SizeOfImage > 8 * 1024 * 1024 ||
            nt->OptionalHeader.SizeOfHeaders > raw.size() ||
            nt->OptionalHeader.SizeOfHeaders > nt->OptionalHeader.SizeOfImage) return false;
        size_t offset = static_cast<size_t>(dos->e_lfanew) + sizeof(IMAGE_NT_HEADERS64);
        if (!Range(offset, nt->FileHeader.NumberOfSections * sizeof(IMAGE_SECTION_HEADER), raw.size())) return false;
        sections = reinterpret_cast<const IMAGE_SECTION_HEADER*>(raw.data() + offset);
        for (unsigned i = 0; i < nt->FileHeader.NumberOfSections; ++i) {
            const auto& s = sections[i];
            if (!Range(s.PointerToRawData, s.SizeOfRawData, raw.size()) ||
                !Range(s.VirtualAddress, std::max(s.Misc.VirtualSize, s.SizeOfRawData), nt->OptionalHeader.SizeOfImage)) return false;
        }
        if (strictIntl) {
            // Pinned intl.cpl 6.1.7601.17514 contract (measured).
            if (nt->OptionalHeader.SizeOfImage != 0x61000 || nt->OptionalHeader.AddressOfEntryPoint != 0x20064) return false;
            for (unsigned index : {IMAGE_DIRECTORY_ENTRY_TLS, IMAGE_DIRECTORY_ENTRY_LOAD_CONFIG,
                                   IMAGE_DIRECTORY_ENTRY_DELAY_IMPORT, IMAGE_DIRECTORY_ENTRY_COM_DESCRIPTOR}) {
                if (nt->OptionalHeader.DataDirectory[index].VirtualAddress || nt->OptionalHeader.DataDirectory[index].Size) {
                    Wh_Log(L"Unsupported directory %u; no partial/guessed initialization", index); return false;
                }
            }
        }
        return true;
    }
};
IMAGE_NT_HEADERS64* ImageHeaders(BYTE* image) {
    return reinterpret_cast<IMAGE_NT_HEADERS64*>(image + reinterpret_cast<IMAGE_DOS_HEADER*>(image)->e_lfanew);
}
const char* ImageString(BYTE* image, DWORD size, DWORD rva) {
    if (rva >= size) return nullptr;
    const char* s = reinterpret_cast<const char*>(image + rva);
    return memchr(s, 0, size - rva) ? s : nullptr;
}
std::string LowerDll(const char* name) {
    std::string s(name);
    for (auto& c : s) if (c >= 'A' && c <= 'Z') c += 'a' - 'A';
    return s;
}
HMODULE SystemDependency(const char* name) {
    const auto key = LowerDll(name);
    // No absolute path or search path supplied by an import descriptor.
    if (key.find_first_of("\\/:") != std::string::npos) return nullptr;
    for (const auto& d : g_deps) if (d.name == key) return d.module;
    std::wstring wide(key.begin(), key.end());
    HMODULE m = LoadLibraryExW(wide.c_str(), nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!m) { Wh_Log(L"Dependency load FAILED: %S, Win32=%lu", name, GetLastError()); return nullptr; }
    g_deps.push_back({key, m});
    return m;
}
bool BindImports(Image& image, bool compatibility) {
    auto* nt = ImageHeaders(image.base);
    auto directory = nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT];
    if (!directory.Size) return true;
    if (!Range(directory.VirtualAddress, directory.Size, image.size)) return false;
    for (size_t off = 0; off + sizeof(IMAGE_IMPORT_DESCRIPTOR) <= directory.Size; off += sizeof(IMAGE_IMPORT_DESCRIPTOR)) {
        auto* desc = reinterpret_cast<IMAGE_IMPORT_DESCRIPTOR*>(image.base + directory.VirtualAddress + off);
        if (!desc->Name) return true;
        const char* dll = ImageString(image.base, image.size, desc->Name);
        if (!dll) return false;
        HMODULE module = SystemDependency(dll);
        if (!module) return false;
        DWORD readRva = desc->OriginalFirstThunk ? desc->OriginalFirstThunk : desc->FirstThunk;
        for (DWORD index = 0; ; ++index) {
            if (index > image.size / sizeof(ULONGLONG)) return false;
            size_t read = static_cast<size_t>(readRva) + index * sizeof(ULONGLONG);
            size_t write = static_cast<size_t>(desc->FirstThunk) + index * sizeof(ULONGLONG);
            if (!Range(read, 8, image.size) || !Range(write, 8, image.size)) return false;
            ULONGLONG value = *reinterpret_cast<ULONGLONG*>(image.base + read);
            if (!value) break;
            LPCSTR proc = nullptr;
            if (IMAGE_SNAP_BY_ORDINAL64(value)) proc = MAKEINTRESOURCEA(IMAGE_ORDINAL64(value));
            else {
                if (value > MAXDWORD - 2) return false;
                proc = ImageString(image.base, image.size, static_cast<DWORD>(value) + 2);
                if (!proc) return false;
            }
            FARPROC target = compatibility ? ResolvePrivate(module, dll, proc) : GetProcAddress(module, proc);
            if (!target) {
                if (reinterpret_cast<ULONG_PTR>(proc) <= 0xffff)
                    Wh_Log(L"Missing import: %S ordinal #%u", dll, LOWORD(proc));
                else Wh_Log(L"Missing import: %S!%S; no arbitrary stub is provided", dll, proc);
                SetLastError(ERROR_PROC_NOT_FOUND); return false;
            }
            *reinterpret_cast<ULONGLONG*>(image.base + write) = reinterpret_cast<ULONGLONG>(target);
        }
    }
    return false; // No terminator inside the import directory.
}
bool CopyImage(const PEView& pe, Image& image) {
    image.size = pe.nt->OptionalHeader.SizeOfImage;
    image.base = static_cast<BYTE*>(VirtualAlloc(nullptr, image.size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE));
    if (!image.base) return false;
    memcpy(image.base, pe.raw.data(), pe.nt->OptionalHeader.SizeOfHeaders);
    for (unsigned i = 0; i < pe.nt->FileHeader.NumberOfSections; ++i) {
        const auto& s = pe.sections[i];
        if (s.SizeOfRawData) memcpy(image.base + s.VirtualAddress, pe.raw.data() + s.PointerToRawData, s.SizeOfRawData);
    }
    return true;
}
bool Relocate(Image& image) {
    auto* nt = ImageHeaders(image.base);
    const ULONGLONG delta = reinterpret_cast<ULONGLONG>(image.base) - nt->OptionalHeader.ImageBase;
    if (!delta) return true;
    auto directory = nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC];
    if (!directory.Size || !Range(directory.VirtualAddress, directory.Size, image.size)) return false;
    size_t offset = 0;
    while (offset < directory.Size) {
        if (!Range(offset, sizeof(IMAGE_BASE_RELOCATION), directory.Size)) return false;
        auto* block = reinterpret_cast<IMAGE_BASE_RELOCATION*>(image.base + directory.VirtualAddress + offset);
        if (block->SizeOfBlock < sizeof(*block) || (block->SizeOfBlock & 1) ||
            !Range(offset, block->SizeOfBlock, directory.Size)) return false;
        const auto* entries = reinterpret_cast<const WORD*>(block + 1);
        size_t count = (block->SizeOfBlock - sizeof(*block)) / sizeof(WORD);
        for (size_t i = 0; i < count; ++i) {
            WORD type = entries[i] >> 12;
            size_t rva = static_cast<size_t>(block->VirtualAddress) + (entries[i] & 0xfff);
            if (type == IMAGE_REL_BASED_ABSOLUTE) continue;
            if (type != IMAGE_REL_BASED_DIR64 || !Range(rva, sizeof(ULONGLONG), image.size)) return false;
            *reinterpret_cast<ULONGLONG*>(image.base + rva) += delta;
        }
        offset += block->SizeOfBlock;
    }
    return true;
}
FARPROC PrivateExport(LPCSTR wanted) {
    try {
        if (!g_image.base) return nullptr;
        auto d = ImageHeaders(g_image.base)->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT];
        if (!Range(d.VirtualAddress, sizeof(IMAGE_EXPORT_DIRECTORY), g_image.size)) return nullptr;
        auto* e = reinterpret_cast<IMAGE_EXPORT_DIRECTORY*>(g_image.base + d.VirtualAddress);
        if (!Range(e->AddressOfFunctions, static_cast<size_t>(e->NumberOfFunctions) * 4, g_image.size) ||
            !Range(e->AddressOfNames, static_cast<size_t>(e->NumberOfNames) * 4, g_image.size) ||
            !Range(e->AddressOfNameOrdinals, static_cast<size_t>(e->NumberOfNames) * 2, g_image.size)) return nullptr;
        DWORD index = MAXDWORD;
        if (reinterpret_cast<ULONG_PTR>(wanted) <= 0xffff) {
            DWORD ordinal = LOWORD(wanted); if (ordinal < e->Base) return nullptr; index = ordinal - e->Base;
        } else {
            auto* names = reinterpret_cast<DWORD*>(g_image.base + e->AddressOfNames);
            auto* ordinals = reinterpret_cast<WORD*>(g_image.base + e->AddressOfNameOrdinals);
            for (DWORD i = 0; i < e->NumberOfNames; ++i) {
                const char* s = ImageString(g_image.base, g_image.size, names[i]);
                if (s && !strcmp(s, wanted)) { index = ordinals[i]; break; }
            }
        }
        if (index >= e->NumberOfFunctions) return nullptr;
        DWORD rva = reinterpret_cast<DWORD*>(g_image.base + e->AddressOfFunctions)[index];
        if (!rva || rva >= g_image.size || (rva >= d.VirtualAddress && rva - d.VirtualAddress < d.Size)) return nullptr;
        return reinterpret_cast<FARPROC>(g_image.base + rva);

    } catch (...) {
        return nullptr;
    }
}
bool ProtectImage(Image& image, const PEView& pe) {
    DWORD old = 0;
    if (!VirtualProtect(image.base, pe.nt->OptionalHeader.SizeOfHeaders, PAGE_READONLY, &old)) return false;
    for (unsigned i = 0; i < pe.nt->FileHeader.NumberOfSections; ++i) {
        const auto& s = pe.sections[i];
        DWORD count = std::max(s.Misc.VirtualSize, s.SizeOfRawData);
        if (!count) continue;
        bool execute = (s.Characteristics & IMAGE_SCN_MEM_EXECUTE) != 0;
        bool read = (s.Characteristics & IMAGE_SCN_MEM_READ) != 0;
        bool write = (s.Characteristics & IMAGE_SCN_MEM_WRITE) != 0;
        DWORD access = execute ? (write ? PAGE_EXECUTE_READWRITE : read ? PAGE_EXECUTE_READ : PAGE_EXECUTE)
                               : write ? PAGE_READWRITE : read ? PAGE_READONLY : PAGE_NOACCESS;
        if (execute && write) return Fail(L"Unexpected writable/executable payload section", ERROR_BAD_EXE_FORMAT);
        if (!VirtualProtect(image.base + s.VirtualAddress, count, access, &old)) return false;
    }
    return FlushInstructionCache(GetCurrentProcess(), image.base, image.size) != FALSE;
}
// ===== Legacy call boundary =====
// There is deliberately no crash guard here. The previous vectored-handler
// "guard" could not contain anything: a plain C++ try/catch does not catch an
// access violation, and a vectored CONTINUE handler only runs after every
// frame-based handler in the process has already declined the exception, i.e.
// once the unwind is decided. All it did was add two process-wide exception
// handlers to explorer.exe and log on the way down, so it is removed.
//
// What is left is a plain C++ try/catch around each call into the mapped Win7
// image. It contains genuine C++ exceptions (thrown by the legacy provider or
// by this mod) so they never cross back into an OS window-proc or DLL-entry
// dispatcher, and turns them into "legacy disabled, fall back to the native
// intl.cpl". A hardware fault inside the mapped image is NOT caught and NOT
// swallowed: it goes to the host's normal exception handling exactly as it
// would without this mod, which can take the host process down. That risk is
// documented in the README.
template <typename Fn>
bool LegacyInvoke(Fn&& fn, DWORD& exception) {
    exception = 0;
    try {
        fn();
    } catch (...) {
        exception = 0xE06D7363; // a real C++ exception, not a hardware fault code
        return false;
    }
    return true;
}
bool CallEntry(EntryProc entry, HINSTANCE image, DWORD reason, BOOL& result, DWORD& exception) {
    result = FALSE;
    return LegacyInvoke([&] { result = entry(image, reason, nullptr); }, exception);
}
bool CallCpl(CplProc proc, HWND hwnd, UINT message, LPARAM a, LPARAM b, LONG& result, DWORD& exception) {
    result = 0;
    return LegacyInvoke([&] { result = proc(hwnd, message, a, b); }, exception);
}

bool IsMain(HMODULE module) { return g_image.base && module == reinterpret_cast<HMODULE>(g_image.base); }

// ===== Private CRT heap overrides (msvcrt.dll, private IAT only) =====
// Evidence: the genuine attach chain fails CLOSED when msvcrt!malloc(0x100)
// returns NULL (C initializer at RVA 0x1FC5C, called from _CRT_INIT; import
// slot 0x16C0 verified as msvcrt!malloc from the descriptors). Observed on
// Win10 19044: DllMain result=0, exception=0. Each override calls the REAL
// msvcrt implementation first and only synthesizes a process-heap block when
// the real call fails; synthesized blocks are tracked so free/realloc/delete
// route them back to HeapFree instead of the CRT heap. Real memory, real
// semantics, no fake success; on a healthy system CRT these are pure
// pass-through. Plain signatures on purpose: x64 has a single calling
// convention, and MSVC keywords would trip the editor's clangd again.
using MallocFn = void* (*)(size_t);
using FreeFn = void (*)(void*);
using CallocFn = void* (*)(size_t, size_t);
using ReallocFn = void* (*)(void*, size_t);
MallocFn g_realMalloc = nullptr;
FreeFn g_realFree = nullptr;
CallocFn g_realCalloc = nullptr;
ReallocFn g_realRealloc = nullptr;
MallocFn g_realNew = nullptr;   // ??2@YAPEAX_K@Z (operator new, throws on failure)
FreeFn g_realDelete = nullptr;  // ??3@YAXPEAX@Z (operator delete)
struct FallbackAlloc { void* ptr = nullptr; };
FallbackAlloc g_fallback[32];
SRWLOCK g_fallbackLock = SRWLOCK_INIT;
LONG g_fallbackLogged = 0;
// RAII slim locks: an exception between acquire/release must never wedge these.
struct SrwExclusive {
    SRWLOCK* lock;
    explicit SrwExclusive(SRWLOCK& l) : lock(&l) { AcquireSRWLockExclusive(lock); }
    ~SrwExclusive() { ReleaseSRWLockExclusive(lock); }
    SrwExclusive(const SrwExclusive&) = delete;
    SrwExclusive& operator=(const SrwExclusive&) = delete;
};
struct SrwShared {
    SRWLOCK* lock;
    explicit SrwShared(SRWLOCK& l) : lock(&l) { AcquireSRWLockShared(lock); }
    ~SrwShared() { ReleaseSRWLockShared(lock); }
    SrwShared(const SrwShared&) = delete;
    SrwShared& operator=(const SrwShared&) = delete;
};
bool FallbackAdd(void* p) {
    if (!p) return false;
    SrwExclusive held(g_fallbackLock);
    for (auto& e : g_fallback) if (!e.ptr) { e.ptr = p; return true; }
    return false;
}
bool FallbackRemove(void* p) {
    if (!p) return false;
    SrwExclusive held(g_fallbackLock);
    for (auto& e : g_fallback) if (e.ptr == p) { e.ptr = nullptr; return true; }
    return false;
}
void FallbackLog(bool ok) {
    if (InterlockedIncrement(&g_fallbackLogged) <= 4)
        Wh_Log(L"System CRT allocation failed; process-heap fallback %s (real memory, tracked)",
               ok ? L"served" : L"FAILED");
}
void* PrivateMalloc(size_t size) {
    try {
        if (g_realMalloc) {
            if (void* p = g_realMalloc(size)) return p;
        }
        void* f = HeapAlloc(GetProcessHeap(), 0, size ? size : 1);
        bool ok = f && FallbackAdd(f);
        if (!ok && f) HeapFree(GetProcessHeap(), 0, f);
        FallbackLog(ok);
        return ok ? f : nullptr;

    } catch (...) {
        return nullptr;
    }
}
void PrivateFree(void* p) {
    try {
        if (!p) return;
        if (FallbackRemove(p)) { HeapFree(GetProcessHeap(), 0, p); return; }
        if (g_realFree) g_realFree(p);

    } catch (...) {
        return;
    }
}
void* PrivateCalloc(size_t num, size_t size) {
    try {
        if (g_realCalloc) {
            if (void* p = g_realCalloc(num, size)) return p;
        }
        constexpr size_t max = static_cast<size_t>(-1);
        if (num && size > max / num) return nullptr; // overflow: the real calloc fails too
        size_t total = num * size;
        void* f = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, total ? total : 1);
        bool ok = f && FallbackAdd(f);
        if (!ok && f) HeapFree(GetProcessHeap(), 0, f);
        if (!ok) FallbackLog(false);
        return ok ? f : nullptr;

    } catch (...) {
        return nullptr;
    }
}
void* PrivateRealloc(void* p, size_t size) {
    try {
        if (!p) return PrivateMalloc(size);
        if (!size) { PrivateFree(p); return nullptr; } // msvcrt: frees, returns NULL
        if (FallbackRemove(p)) {
            void* n = HeapReAlloc(GetProcessHeap(), 0, p, size);
            if (!n) { FallbackAdd(p); return nullptr; } // original block untouched
            FallbackAdd(n); return n;
        }
        // Real realloc of a real block: on failure the documented NULL (old block
        // intact) is honest; synthesizing a copy is impossible without the old size.
        return g_realRealloc ? g_realRealloc(p, size) : nullptr;

    } catch (...) {
        return nullptr;
    }
}
void* PrivateNew(size_t size) {
    if (g_realNew) {
        if (void* p = g_realNew(size)) return p;
    }
    void* f = HeapAlloc(GetProcessHeap(), 0, size ? size : 1);
    if (f && FallbackAdd(f)) return f;
    if (f) HeapFree(GetProcessHeap(), 0, f);
    throw std::bad_alloc(); // genuine operator-new failure semantics
}
void PrivateDelete(void* p) {
    try {     PrivateFree(p);
    } catch (...) {
        return;
    }
}
// VER_* constants (VerifyVersionInfo): declared here under guard because some
// Windhawk SDK setups do not provide them. Values are documented and stable.
#ifndef VER_MINOR_VERSION
#define VER_MINOR_VERSION 0x0000001
#define VER_MAJOR_VERSION 0x0000002
#define VER_BUILD_NUMBER 0x0000004
#define VER_PLATFORMID 0x0000008
#define VER_SERVICEPACKMINOR 0x0000010
#define VER_SERVICEPACKMAJOR 0x0000020
#define VER_SUITENAME 0x0000040
#define VER_PRODUCT_TYPE 0x0000080
#endif
#ifndef VER_EQUAL
#define VER_EQUAL 1
#define VER_GREATER 2
#define VER_GREATER_EQUAL 3
#define VER_LESS 4
#define VER_LESS_EQUAL 5
#endif
// ===== Windows 7 SP1 version spoof (private module only) =====
// intl.cpl imports no version API statically (verified), but may query
// dynamically; input.dll imports GetVersionExW (IAT-patched in AdaptInputIat).
// Whatever the private provider asks sees Windows 7 SP1 x64 (6.1.7601), the
// way ACT shims present a compatible OS to legacy code. System components
// keep calling the real APIs through their own IATs. IsOS needs no spoof:
// its single callsite passes OS_ANYSERVER and the real client answer is right.
constexpr WORD kSpoofMajor = 6, kSpoofMinor = 1;
constexpr DWORD kSpoofBuild = 7601;
constexpr WORD kSpoofSPMajor = 1, kSpoofSPMinor = 0;
constexpr WORD kSpoofSuite = 0x300; // SINGLEUSERTS|PERSONAL, like Win7 Pro
void FillSpoofVersion(OSVERSIONINFOEXW& v) {
    v.dwMajorVersion = kSpoofMajor; v.dwMinorVersion = kSpoofMinor;
    v.dwBuildNumber = kSpoofBuild; v.dwPlatformId = VER_PLATFORM_WIN32_NT;
    wcscpy_s(v.szCSDVersion, L"Service Pack 1");
    v.wServicePackMajor = kSpoofSPMajor; v.wServicePackMinor = kSpoofSPMinor;
    v.wSuiteMask = kSpoofSuite; v.wProductType = VER_NT_WORKSTATION;
}
LONG PrivateRtlGetVersion(OSVERSIONINFOEXW* v) {
    try {
        if (!v || v->dwOSVersionInfoSize < sizeof(OSVERSIONINFOW)) return 0xC000000D; // STATUS_INVALID_PARAMETER
        OSVERSIONINFOEXW full{}; full.dwOSVersionInfoSize = sizeof(full); FillSpoofVersion(full);
        DWORD size = v->dwOSVersionInfoSize;
        memcpy(v, &full, std::min<size_t>(size, sizeof(full)));
        v->dwOSVersionInfoSize = size;
        return 0; // STATUS_SUCCESS

    } catch (...) {
        return 0xC0000001;
    }
}
BOOL PrivateGetVersionExW(LPOSVERSIONINFOW v) {
    try {
        if (!v) { SetLastError(ERROR_INVALID_PARAMETER); return FALSE; }
        OSVERSIONINFOEXW full{}; full.dwOSVersionInfoSize = sizeof(full); FillSpoofVersion(full);
        if (v->dwOSVersionInfoSize == sizeof(OSVERSIONINFOEXW))
            *reinterpret_cast<OSVERSIONINFOEXW*>(v) = full;
        else if (v->dwOSVersionInfoSize == sizeof(OSVERSIONINFOW))
            memcpy(v, &full, sizeof(OSVERSIONINFOW));
        else { SetLastError(ERROR_INVALID_PARAMETER); return FALSE; }
        return TRUE;

    } catch (...) {
        SetLastError(ERROR_NOT_ENOUGH_MEMORY); return FALSE;
    }
}
BOOL PrivateGetVersionExA(LPOSVERSIONINFOA v) {
    try {
        if (!v) { SetLastError(ERROR_INVALID_PARAMETER); return FALSE; }
        OSVERSIONINFOEXW w{}; w.dwOSVersionInfoSize = sizeof(w); FillSpoofVersion(w);
        OSVERSIONINFOEXA a{}; a.dwOSVersionInfoSize = sizeof(a);
        a.dwMajorVersion = w.dwMajorVersion; a.dwMinorVersion = w.dwMinorVersion;
        a.dwBuildNumber = w.dwBuildNumber; a.dwPlatformId = w.dwPlatformId;
        WideCharToMultiByte(CP_ACP, 0, w.szCSDVersion, -1, a.szCSDVersion, ARRAYSIZE(a.szCSDVersion), nullptr, nullptr);
        a.wServicePackMajor = w.wServicePackMajor; a.wServicePackMinor = w.wServicePackMinor;
        a.wSuiteMask = w.wSuiteMask; a.wProductType = w.wProductType;
        if (v->dwOSVersionInfoSize == sizeof(OSVERSIONINFOEXA))
            *reinterpret_cast<OSVERSIONINFOEXA*>(v) = a;
        else if (v->dwOSVersionInfoSize == sizeof(OSVERSIONINFOA))
            memcpy(v, &a, sizeof(OSVERSIONINFOA));
        else { SetLastError(ERROR_INVALID_PARAMETER); return FALSE; }
        return TRUE;

    } catch (...) {
        SetLastError(ERROR_NOT_ENOUGH_MEMORY); return FALSE;
    }
}
DWORD PrivateGetVersion() {
    try {     return (kSpoofBuild << 16) | (kSpoofMinor << 8) | kSpoofMajor;
    } catch (...) {
        return 0;
    }
} // 6.1.7601
ULONGLONG PrivateVerSetConditionMask(ULONGLONG cond, DWORD type, BYTE op) {
    try {
        // Single-bit types only; operator in bits [3*i, 3*i+3) like the real API.
        if (!type || (type & (type - 1))) return cond;
        unsigned bit = 0;
        for (DWORD t = type; (t >>= 1) != 0; ++bit) {}
        if (bit > 7) return cond;
        cond |= static_cast<ULONGLONG>(op & 7) << (bit * 3);
        return cond;

    } catch (...) {
        return cond;
    }
}
bool CheckOp(LONGLONG actual, LONGLONG want, BYTE op) {
    switch (op) {
    case VER_EQUAL: return actual == want;
    case VER_GREATER: return actual > want;
    case VER_GREATER_EQUAL: return actual >= want;
    case VER_LESS: return actual < want;
    case VER_LESS_EQUAL: return actual <= want;
    default: return false;
    }
}
BOOL VerifySpoof(const OSVERSIONINFOEXW& want, DWORD mask, ULONGLONG cond) {
    OSVERSIONINFOEXW actual{}; actual.dwOSVersionInfoSize = sizeof(actual); FillSpoofVersion(actual);
    auto op = [&](DWORD type) -> BYTE {
        unsigned bit = 0;
        for (DWORD t = type; (t & 1) == 0; t >>= 1) ++bit;
        return static_cast<BYTE>((cond >> (bit * 3)) & 7);
    };
    if ((mask & VER_MINOR_VERSION) && !CheckOp(actual.dwMinorVersion, want.dwMinorVersion, op(VER_MINOR_VERSION))) return FALSE;
    if ((mask & VER_MAJOR_VERSION) && !CheckOp(actual.dwMajorVersion, want.dwMajorVersion, op(VER_MAJOR_VERSION))) return FALSE;
    if ((mask & VER_BUILD_NUMBER) && !CheckOp(actual.dwBuildNumber, want.dwBuildNumber, op(VER_BUILD_NUMBER))) return FALSE;
    if ((mask & VER_PLATFORMID) && !CheckOp(actual.dwPlatformId, want.dwPlatformId, op(VER_PLATFORMID))) return FALSE;
    if ((mask & VER_SERVICEPACKMINOR) && !CheckOp(actual.wServicePackMinor, want.wServicePackMinor, op(VER_SERVICEPACKMINOR))) return FALSE;
    if ((mask & VER_SERVICEPACKMAJOR) && !CheckOp(actual.wServicePackMajor, want.wServicePackMajor, op(VER_SERVICEPACKMAJOR))) return FALSE;
    if (mask & VER_SUITENAME) {
        if (op(VER_SUITENAME) != VER_EQUAL) return FALSE;
        if ((actual.wSuiteMask & want.wSuiteMask) != want.wSuiteMask) return FALSE;
    }
    if (mask & VER_PRODUCT_TYPE) {
        if (op(VER_PRODUCT_TYPE) != VER_EQUAL || actual.wProductType != want.wProductType) return FALSE;
    }
    return TRUE;
}
BOOL PrivateVerifyVersionInfoW(LPOSVERSIONINFOEXW info, DWORD mask, DWORDLONG cond) {
    try {
        if (!info || info->dwOSVersionInfoSize < sizeof(OSVERSIONINFOEXW)) { SetLastError(ERROR_INVALID_PARAMETER); return FALSE; }
        if (!VerifySpoof(*info, mask, cond)) { SetLastError(ERROR_OLD_WIN_VERSION); return FALSE; }
        return TRUE;

    } catch (...) {
        SetLastError(ERROR_OLD_WIN_VERSION); return FALSE;
    }
}
BOOL PrivateVerifyVersionInfoA(LPOSVERSIONINFOEXA info, DWORD mask, DWORDLONG cond) {
    try {
        if (!info || info->dwOSVersionInfoSize < sizeof(OSVERSIONINFOEXA)) { SetLastError(ERROR_INVALID_PARAMETER); return FALSE; }
        OSVERSIONINFOEXW w{}; w.dwOSVersionInfoSize = sizeof(w);
        w.dwMajorVersion = info->dwMajorVersion; w.dwMinorVersion = info->dwMinorVersion;
        w.dwBuildNumber = info->dwBuildNumber; w.dwPlatformId = info->dwPlatformId;
        w.wServicePackMajor = info->wServicePackMajor; w.wServicePackMinor = info->wServicePackMinor;
        w.wSuiteMask = info->wSuiteMask; w.wProductType = info->wProductType;
        if (!VerifySpoof(w, mask, cond)) { SetLastError(ERROR_OLD_WIN_VERSION); return FALSE; }
        return TRUE;

    } catch (...) {
        SetLastError(ERROR_OLD_WIN_VERSION); return FALSE;
    }
}
LONG g_versionLogged = 0;
bool IsVersionQuery(LPCSTR name) {
    try {
        return !strcmp(name, "RtlGetVersion") || !strcmp(name, "GetVersionExW") || !strcmp(name, "GetVersionExA") ||
               !strcmp(name, "GetVersion") || !strcmp(name, "VerifyVersionInfoW") ||
               !strcmp(name, "VerifyVersionInfoA") || !strcmp(name, "VerSetConditionMask");

    } catch (...) {
        return false;
    }
}
FARPROC VersionSpoof(LPCSTR name) {
    try {     // called only after IsVersionQuery(name)
        auto asProc = [](auto* p) { return reinterpret_cast<FARPROC>(reinterpret_cast<void*>(p)); };
        if (InterlockedIncrement(&g_versionLogged) <= 6)
            Wh_Log(L"Dynamic version query %S answered as Windows 7 SP1", name);
        if (!strcmp(name, "RtlGetVersion")) return asProc(PrivateRtlGetVersion);
        if (!strcmp(name, "GetVersionExW")) return asProc(PrivateGetVersionExW);
        if (!strcmp(name, "GetVersionExA")) return asProc(PrivateGetVersionExA);
        if (!strcmp(name, "GetVersion")) return asProc(PrivateGetVersion);
        if (!strcmp(name, "VerifyVersionInfoW")) return asProc(PrivateVerifyVersionInfoW);
        if (!strcmp(name, "VerifyVersionInfoA")) return asProc(PrivateVerifyVersionInfoA);
        return asProc(PrivateVerSetConditionMask);

    } catch (...) {
        return nullptr;
    }
}

struct EmbString { UINT id; const wchar_t* text; };
static const EmbString kEmbStrings[] = {
    {1, L"Region and Language"},
    {2, L"Customize settings for the display of languages, numbers, times, and dates."},
    {3, L"Customize Format"},
    {4, L"One or more of your regional settings are invalid. To fix this problem review and correct the customizable settings."},
    {8, L".7"},
    {9, L"0.7"},
    {10, L"Metric"},
    {11, L"U.S."},
    {12, L"One or more of the characters you entered in this field are invalid. Try using different characters."},
    {13, L"One or more of the characters you entered for %s are invalid. Try using a different character or enter a blank space."},
    {14, L"Decimal Symbol"},
    {15, L"Negative Sign"},
    {16, L"Grouping Symbol"},
    {18, L"AM Symbol"},
    {19, L"PM Symbol"},
    {21, L"Currency Symbol"},
    {22, L"Currency Decimal Symbol"},
    {23, L"Currency Grouping Symbol"},
    {24, L"One or more of the characters you entered for the %s format are invalid. Try using different characters."},
    {25, L"Long Time"},
    {26, L"Short Date"},
    {27, L"Long Date"},
    {28, L"The value in this field must be a number between 99 and 9999. Try using a different number."},
    {29, L"Short Time"},
    {30, L"&Format:"},
    {31, L"&Format: (* Custom Locale)"},
    {55, L"H"},
    {56, L"h"},
    {57, L"M"},
    {58, L"m"},
    {59, L"s"},
    {60, L"t"},
    {61, L"d"},
    {62, L"y"},
    {69, L"System locale has been changed. You must restart Windows for the changes to take effect."},
    {70, L"Change Regional Options"},
    {71, L"Setup was unable to install the chosen locale.  Please contact your system Administrator."},
    {72, L"System display language has been changed. You must restart Windows for the changes to take effect."},
    {77, L"Remove all customizations for the current format?"},
    {78, L"Would you like to apply your region and language changes?"},
    {79, L"Restart now"},
    {80, L"Cancel"},
    {81, L"Make sure you save your work and close all open programs before restarting."},
    {82, L"Change System Locale"},
    {83, L"Windows could not properly load the %s keyboard layout."},
    {96, L"Spanish (Spain)"},
    {100, L"You must log off for display language changes to take effect"},
    {201, L"Make sure you save your work and close all open programs before you log off."},
    {202, L"Log off now"},
    {203, L"Cancel"},
    {204, L"Change Display Language"},
    {205, L"To ensure that the computer reflects these changes we recommend that you apply them before making any further system changes."},
    {206, L"Apply"},
    {207, L"Cancel"},
    {208, L"The task cannot be completed"},
    {250, L"Current user"},
    {251, L"Welcome screen"},
    {252, L"New user accounts"},
    {253, L"Display language:"},
    {254, L"Input language:"},
    {255, L"Format:"},
    {256, L"Location:"},
    {257, L"Setting could not be read"},
    {900, L"Context"},
    {901, L"Never"},
    {902, L"National"},
    {0, nullptr},
};

struct EmbStrBlock { UINT block; DWORD size; };
static const EmbStrBlock kEmbStrBlocks[] = {
    {1, 1002},
    {2, 756},
    {4, 48},
    {5, 862},
    {6, 344},
    {7, 182},
    {13, 536},
    {14, 88},
    {16, 196},
    {17, 100},
    {57, 72},
    {0, 0},
};

struct EmbCtl {
    DWORD helpId, exStyle, style;
    short x, y, cx, cy;
    DWORD id;
    WORD clsOrd; const wchar_t* clsText;   // 0xFFFF => clsText
    WORD textOrd; const wchar_t* text;     // 0xFFFF => text, 0xFFFE => none
};
struct EmbDlg {
    WORD id; DWORD helpId, exStyle, style;
    short x, y, cx, cy;
    const wchar_t* title;
    WORD fontPt, fontWeight; BYTE fontItalic, fontCharset;
    const wchar_t* typeface;
    WORD count; const EmbCtl* ctls;
    DWORD expectSize; DWORD expectFnv;
};
static const EmbCtl kDlg101Ctl[] = {
    {0x00000000, 0x00000000, 0x50020000, 7, 7, 235, 8, 1039, 0x0082, nullptr, 0xfffe, nullptr},
    {0x00000000, 0x00000000, 0x50210103, 7, 17, 238, 60, 1021, 0x0085, nullptr, 0xfffe, nullptr},
    {0x00000000, 0x00000000, 0x50010000, 7, 30, 119, 8, 1033, 0xffff, L"SysLink", 0xffff, L"<A>Change sorting method</A>"},
    {0x00000000, 0x00000000, 0x50000007, 7, 42, 238, 99, 4294967295, 0x0080, nullptr, 0xffff, L"Date and time formats"},
    {0x00000000, 0x00000000, 0x50020000, 12, 56, 64, 8, 4294967295, 0x0082, nullptr, 0xffff, L"&Short date:"},
    {0x00000000, 0x00000000, 0x50210003, 77, 54, 163, 60, 1028, 0x0085, nullptr, 0xfffe, nullptr},
    {0x00000000, 0x00000000, 0x50020000, 12, 71, 64, 8, 4294967295, 0x0082, nullptr, 0xffff, L"&Long date:"},
    {0x00000000, 0x00000000, 0x50210003, 77, 69, 163, 60, 1029, 0x0085, nullptr, 0xfffe, nullptr},
    {0x00000000, 0x00000000, 0x50020000, 12, 86, 64, 8, 4294967295, 0x0082, nullptr, 0xffff, L"S&hort time:"},
    {0x00000000, 0x00000000, 0x50210003, 77, 84, 163, 60, 1030, 0x0085, nullptr, 0xfffe, nullptr},
    {0x00000000, 0x00000000, 0x50020000, 12, 101, 64, 8, 4294967295, 0x0082, nullptr, 0xffff, L"L&ong time:"},
    {0x00000000, 0x00000000, 0x50210003, 77, 99, 163, 60, 1031, 0x0085, nullptr, 0xfffe, nullptr},
    {0x00000000, 0x00000000, 0x50020000, 12, 116, 64, 8, 4294967295, 0x0082, nullptr, 0xffff, L"First day of &week:"},
    {0x00000000, 0x00000000, 0x50210003, 77, 114, 163, 60, 1032, 0x0085, nullptr, 0xfffe, nullptr},
    {0x00000000, 0x00000000, 0x50010000, 12, 129, 228, 8, 1034, 0xffff, L"SysLink", 0xffff, L"<A>What does the notation mean?</A>"},
    {0x00000000, 0x00000000, 0x50000007, 7, 144, 238, 60, 4294967295, 0x0080, nullptr, 0xffff, L"Examples"},
    {0x00000000, 0x00000000, 0x50020000, 12, 154, 64, 8, 4294967295, 0x0082, nullptr, 0xffff, L"Short date:"},
    {0x00000000, 0x00000000, 0x50020000, 77, 154, 163, 8, 1035, 0x0082, nullptr, 0xfffe, nullptr},
    {0x00000000, 0x00000000, 0x50020000, 12, 166, 64, 8, 4294967295, 0x0082, nullptr, 0xffff, L"Long date:"},
    {0x00000000, 0x00000000, 0x50020000, 77, 166, 163, 8, 1036, 0x0082, nullptr, 0xfffe, nullptr},
    {0x00000000, 0x00000000, 0x50020000, 12, 178, 64, 8, 4294967295, 0x0082, nullptr, 0xffff, L"Short time:"},
    {0x00000000, 0x00000000, 0x50020000, 77, 178, 163, 8, 1037, 0x0082, nullptr, 0xfffe, nullptr},
    {0x00000000, 0x00000000, 0x50020000, 12, 190, 64, 8, 4294967295, 0x0082, nullptr, 0xffff, L"Long time:"},
    {0x00000000, 0x00000000, 0x50020000, 77, 190, 163, 8, 1038, 0x0082, nullptr, 0xfffe, nullptr},
    {0x00000000, 0x00000000, 0x50010000, 165, 208, 80, 13, 1024, 0x0080, nullptr, 0xffff, L"A&dditional settings..."},
    {0x00000000, 0x00000000, 0x50010000, 7, 223, 238, 13, 1040, 0xffff, L"SysLink", 0xffff, L"<A>Go online to learn about changing languages and regional formats</A>"},
};
static const EmbCtl kDlg102Ctl[] = {
    {0x00000000, 0x00000000, 0x50000007, 5, 8, 242, 52, 1001, 0x0080, nullptr, 0xffff, L"Keyboards and other input languages"},
    {0x00000000, 0x00000000, 0x50020000, 12, 18, 230, 10, 1171, 0x0082, nullptr, 0xffff, L"To change your keyboard or input language click Change keyboards."},
    {0x00000000, 0x00000000, 0x50030000, 145, 32, 95, 14, 1172, 0x0080, nullptr, 0xffff, L"&Change keyboards..."},
    {0x00000000, 0x00000000, 0x50010000, 12, 48, 233, 10, 1042, 0xffff, L"SysLink", 0xffff, L"<A>How do I change the keyboard layout for the Welcome screen?</A>"},
    {0x00000000, 0x00000000, 0x50000007, 5, 62, 242, 156, 1180, 0x0080, nullptr, 0xffff, L"Display language"},
    {0x00000000, 0x00000000, 0x50020000, 14, 74, 225, 20, 1181, 0x0082, nullptr, 0xffff, L"Install or uninstall languages that Windows can use to display text and where supported recognize speech and handwriting."},
    {0x00000000, 0x00000000, 0x50030000, 125, 96, 115, 14, 1182, 0x0080, nullptr, 0xffff, L"&Install/uninstall languages..."},
    {0x00000000, 0x00000000, 0x50020000, 14, 113, 225, 10, 1150, 0x0082, nullptr, 0xffff, L"As a guest user you cannot change the display language:"},
    {0x00000000, 0x00000000, 0x50020000, 14, 113, 225, 10, 1179, 0x0082, nullptr, 0xffff, L"Display language selection is blocked by group policy."},
    {0x00000000, 0x00000000, 0x50020000, 14, 113, 225, 10, 1177, 0x0082, nullptr, 0xffff, L"C&hoose a display language:"},
    {0x00000000, 0x00000000, 0x50210043, 14, 125, 225, 40, 1178, 0x0085, nullptr, 0xfffe, nullptr},
    {0x00000000, 0x00000000, 0x50020000, 14, 143, 225, 17, 1173, 0x0082, nullptr, 0xffff, L"&Some text is not translated into the language you selected. Select another language for Windows to use to display this text:"},
    {0x00000000, 0x00000000, 0x50020000, 14, 143, 225, 20, 1151, 0x0082, nullptr, 0xffff, L"This language is only partially translated and you might see some text displayed in:"},
    {0x00000000, 0x00000000, 0x50210043, 14, 163, 225, 40, 1174, 0x0085, nullptr, 0xfffe, nullptr},
    {0x00000000, 0x00000000, 0x50020000, 14, 180, 225, 17, 1175, 0x0082, nullptr, 0xffff, L"&This language is also only partially translated. Select a third language for Windows to use when displaying the remaining text:"},
    {0x00000000, 0x00000000, 0x50020000, 14, 180, 225, 20, 1152, 0x0082, nullptr, 0xffff, L"This language is also only partially translated and you might see some text displayed in: "},
    {0x00000000, 0x00000000, 0x50210043, 14, 199, 225, 40, 1176, 0x0085, nullptr, 0xfffe, nullptr},
    {0x00000000, 0x00000000, 0x50010000, 7, 223, 238, 13, 1041, 0xffff, L"SysLink", 0xffff, L"<A>How can I install additional languages?</A>"},
};
static const EmbCtl kDlg104Ctl[] = {
    {0x00000000, 0x00000000, 0x50000007, 5, 8, 242, 73, 4294967295, 0x0080, nullptr, 0xffff, L"Welcome screen and new user accounts"},
    {0x00000000, 0x00000000, 0x50020000, 14, 20, 223, 23, 2107, 0x0082, nullptr, 0xffff, L"View and copy your international settings to the welcome screen, system accounts and new user accounts."},
    {0x00000000, 0x00000000, 0x50030000, 160, 51, 80, 14, 2104, 0x0080, nullptr, 0xffff, L"C&opy settings..."},
    {0x00000000, 0x00000000, 0x50010000, 14, 69, 223, 10, 1044, 0xffff, L"SysLink", 0xffff, L"<A>Tell me more about these accounts</A>"},
    {0x00000000, 0x00000000, 0x50000007, 5, 83, 242, 91, 4294967295, 0x0080, nullptr, 0xffff, L"Language for non-Unicode programs"},
    {0x00000000, 0x00000000, 0x50020000, 14, 95, 225, 26, 1050, 0x0082, nullptr, 0xffff, L"This setting (system locale) controls the language used when displaying text in programs that do not support Unicode."},
    {0x00000000, 0x00000000, 0x50020000, 14, 120, 223, 10, 1051, 0x0082, nullptr, 0xffff, L"Current language for non-Unicode programs:"},
    {0x00000000, 0x00000000, 0x50020000, 25, 132, 200, 10, 2100, 0x0082, nullptr, 0xffff, L"<systemLocale>"},
    {0x00000000, 0x00000000, 0x50030000, 140, 144, 100, 14, 2102, 0x0080, nullptr, 0xffff, L"&Change system locale..."},
    {0x00000000, 0x00000000, 0x50010000, 14, 162, 223, 10, 1043, 0xffff, L"SysLink", 0xffff, L"<A>What is system locale?</A>"},
};
static const EmbCtl kDlg105Ctl[] = {
    {0x00000000, 0x00000000, 0x50000007, 5, 7, 242, 48, 1009, 0x0080, nullptr, 0xffff, L"Example"},
    {0x00000000, 0x00000000, 0x50020000, 10, 20, 30, 10, 1007, 0x0082, nullptr, 0xffff, L"Positive:"},
    {0x00000000, 0x00000000, 0x50800880, 42, 18, 80, 14, 1005, 0x0081, nullptr, 0xfffe, nullptr},
    {0x00000000, 0x00000000, 0x50020000, 128, 20, 31, 10, 1008, 0x0082, nullptr, 0xffff, L"Negative:"},
    {0x00000000, 0x00000000, 0x50800880, 162, 18, 80, 14, 1006, 0x0081, nullptr, 0xfffe, nullptr},
    {0x00000000, 0x00000000, 0x50020000, 10, 38, 30, 10, 1012, 0x0082, nullptr, 0xffff, L"Positive:"},
    {0x00000000, 0x00003000, 0x50800882, 42, 36, 80, 14, 1010, 0x0081, nullptr, 0xfffe, nullptr},
    {0x00000000, 0x00000000, 0x50020000, 128, 38, 31, 10, 1013, 0x0082, nullptr, 0xffff, L"Negative:"},
    {0x00000000, 0x00003000, 0x50800882, 162, 36, 80, 14, 1011, 0x0081, nullptr, 0xfffe, nullptr},
    {0x00000000, 0x00000000, 0x50020000, 20, 61, 96, 9, 4294967295, 0x0082, nullptr, 0xffff, L"&Decimal symbol:"},
    {0x00000000, 0x00000000, 0x50210102, 128, 59, 100, 100, 1070, 0x0085, nullptr, 0xfffe, nullptr},
    {0x00000000, 0x00000000, 0x50020000, 20, 77, 96, 9, 4294967295, 0x0082, nullptr, 0xffff, L"&No. of digits after decimal:"},
    {0x00000000, 0x00000000, 0x50210103, 128, 75, 100, 100, 1072, 0x0085, nullptr, 0xfffe, nullptr},
    {0x00000000, 0x00000000, 0x50020000, 20, 93, 96, 9, 4294967295, 0x0082, nullptr, 0xffff, L"D&igit grouping symbol:"},
    {0x00000000, 0x00000000, 0x50210102, 128, 91, 100, 100, 1073, 0x0085, nullptr, 0xfffe, nullptr},
    {0x00000000, 0x00000000, 0x50020000, 20, 109, 96, 9, 4294967295, 0x0082, nullptr, 0xffff, L"Di&git grouping:"},
    {0x00000000, 0x00000000, 0x50210103, 128, 107, 100, 100, 1074, 0x0085, nullptr, 0xfffe, nullptr},
    {0x00000000, 0x00000000, 0x50020000, 20, 125, 96, 9, 4294967295, 0x0082, nullptr, 0xffff, L"N&egative sign symbol:"},
    {0x00000000, 0x00000000, 0x50210102, 128, 123, 100, 100, 1076, 0x0085, nullptr, 0xfffe, nullptr},
    {0x00000000, 0x00000000, 0x50020000, 20, 141, 96, 9, 4294967295, 0x0082, nullptr, 0xffff, L"Nega&tive number format:"},
    {0x00000000, 0x00000000, 0x50210103, 128, 139, 100, 100, 1078, 0x0085, nullptr, 0xfffe, nullptr},
    {0x00000000, 0x00000000, 0x50020000, 20, 157, 96, 9, 4294967295, 0x0082, nullptr, 0xffff, L"Dis&play leading zeros:"},
    {0x00000000, 0x00000000, 0x50210103, 128, 155, 100, 100, 1080, 0x0085, nullptr, 0xfffe, nullptr},
    {0x00000000, 0x00000000, 0x50020000, 20, 173, 96, 9, 4294967295, 0x0082, nullptr, 0xffff, L"&List separator:"},
    {0x00000000, 0x00000000, 0x50210102, 128, 171, 100, 100, 1079, 0x0085, nullptr, 0xfffe, nullptr},
    {0x00000000, 0x00000000, 0x50020000, 20, 189, 96, 9, 4294967295, 0x0082, nullptr, 0xffff, L"&Measurement system:"},
    {0x00000000, 0x00000000, 0x50210103, 128, 187, 100, 100, 1081, 0x0085, nullptr, 0xfffe, nullptr},
    {0x00000000, 0x00000000, 0x50020000, 20, 205, 96, 9, 1082, 0x0082, nullptr, 0xffff, L"&Standard digits:"},
    {0x00000000, 0x00000000, 0x50210103, 128, 203, 100, 100, 1083, 0x0085, nullptr, 0xfffe, nullptr},
    {0x00000000, 0x00000000, 0x50020000, 20, 221, 96, 9, 1084, 0x0082, nullptr, 0xffff, L"&Use native digits:"},
    {0x00000000, 0x00000000, 0x50210103, 128, 219, 100, 100, 1085, 0x0085, nullptr, 0xfffe, nullptr},
    {0x00000000, 0x00000000, 0x50020000, 7, 236, 185, 16, 1047, 0x0082, nullptr, 0xffff, L"Click Reset to restore the system default settings for numbers, currency, time, and date."},
    {0x00000000, 0x00000000, 0x50010000, 196, 237, 50, 13, 1046, 0x0080, nullptr, 0xffff, L"&Reset"},
};
static const EmbCtl kDlg106Ctl[] = {
    {0x00000000, 0x00000000, 0x50000007, 5, 7, 242, 40, 1009, 0x0080, nullptr, 0xffff, L"Example"},
    {0x00000000, 0x00000000, 0x50020000, 10, 25, 30, 10, 1007, 0x0082, nullptr, 0xffff, L"Positive:"},
    {0x00000000, 0x00000000, 0x50800880, 42, 23, 80, 14, 1005, 0x0081, nullptr, 0xfffe, nullptr},
    {0x00000000, 0x00000000, 0x50020000, 128, 25, 31, 10, 1008, 0x0082, nullptr, 0xffff, L"Negative:"},
    {0x00000000, 0x00000000, 0x50800880, 162, 23, 80, 14, 1006, 0x0081, nullptr, 0xfffe, nullptr},
    {0x00000000, 0x00000000, 0x50020000, 20, 61, 96, 9, 4294967295, 0x0082, nullptr, 0xffff, L"&Currency symbol:"},
    {0x00000000, 0x00000000, 0x50210102, 128, 59, 100, 100, 1071, 0x0085, nullptr, 0xfffe, nullptr},
    {0x00000000, 0x00000000, 0x50020000, 20, 78, 96, 9, 4294967295, 0x0082, nullptr, 0xffff, L"&Positive currency format:"},
    {0x00000000, 0x00000000, 0x50210103, 128, 76, 100, 100, 1077, 0x0085, nullptr, 0xfffe, nullptr},
    {0x00000000, 0x00000000, 0x50020000, 20, 95, 96, 9, 4294967295, 0x0082, nullptr, 0xffff, L"&Negative currency format:"},
    {0x00000000, 0x00000000, 0x50210103, 128, 93, 100, 100, 1078, 0x0085, nullptr, 0xfffe, nullptr},
    {0x00000000, 0x00000000, 0x50020000, 20, 121, 96, 9, 4294967295, 0x0082, nullptr, 0xffff, L"&Decimal symbol:"},
    {0x00000000, 0x00000000, 0x50210102, 128, 119, 100, 100, 1070, 0x0085, nullptr, 0xfffe, nullptr},
    {0x00000000, 0x00000000, 0x50020000, 20, 138, 96, 9, 4294967295, 0x0082, nullptr, 0xffff, L"N&o. of digits after decimal:"},
    {0x00000000, 0x00000000, 0x50210103, 128, 136, 100, 100, 1072, 0x0085, nullptr, 0xfffe, nullptr},
    {0x00000000, 0x00000000, 0x50020000, 20, 166, 96, 9, 4294967295, 0x0082, nullptr, 0xffff, L"Di&git grouping symbol:"},
    {0x00000000, 0x00000000, 0x50210102, 128, 164, 100, 100, 1073, 0x0085, nullptr, 0xfffe, nullptr},
    {0x00000000, 0x00000000, 0x50020000, 20, 183, 96, 9, 4294967295, 0x0082, nullptr, 0xffff, L"D&igit grouping:"},
    {0x00000000, 0x00000000, 0x50210103, 128, 181, 100, 100, 1074, 0x0085, nullptr, 0xfffe, nullptr},
    {0x00000000, 0x00000000, 0x50020000, 7, 236, 185, 16, 1047, 0x0082, nullptr, 0xffff, L"Click Reset to restore the system default settings for numbers, currency, time, and date."},
    {0x00000000, 0x00000000, 0x50010000, 196, 237, 50, 13, 1046, 0x0080, nullptr, 0xffff, L"&Reset"},
};
static const EmbCtl kDlg107Ctl[] = {
    {0x00000000, 0x00000000, 0x50000007, 5, 7, 242, 47, 4294967295, 0x0080, nullptr, 0xffff, L"Examples"},
    {0x00000000, 0x00000000, 0x50020000, 13, 20, 54, 10, 4294967295, 0x0082, nullptr, 0xffff, L"Short time:"},
    {0x00000000, 0x00000000, 0x50800880, 68, 18, 84, 14, 1006, 0x0081, nullptr, 0xfffe, nullptr},
    {0x00000000, 0x00002000, 0x50800882, 156, 18, 84, 14, 1011, 0x0081, nullptr, 0xfffe, nullptr},
    {0x00000000, 0x00000000, 0x50020000, 13, 36, 54, 10, 4294967295, 0x0082, nullptr, 0xffff, L"Long time:"},
    {0x00000000, 0x00000000, 0x50800880, 68, 34, 84, 14, 1005, 0x0081, nullptr, 0xfffe, nullptr},
    {0x00000000, 0x00002000, 0x50800882, 156, 34, 84, 14, 1010, 0x0081, nullptr, 0xfffe, nullptr},
    {0x00000000, 0x00000000, 0x50000007, 5, 57, 242, 82, 4294967295, 0x0080, nullptr, 0xffff, L"Time formats"},
    {0x00000000, 0x00000000, 0x50020000, 13, 70, 54, 10, 4294967295, 0x0082, nullptr, 0xffff, L"&Short time:"},
    {0x00000000, 0x00000000, 0x50210142, 68, 68, 84, 100, 1093, 0x0085, nullptr, 0xfffe, nullptr},
    {0x00000000, 0x00000000, 0x50020000, 13, 86, 54, 10, 4294967295, 0x0082, nullptr, 0xffff, L"&Long time:"},
    {0x00000000, 0x00000000, 0x50210142, 68, 84, 84, 100, 1090, 0x0085, nullptr, 0xfffe, nullptr},
    {0x00000000, 0x00000000, 0x50020000, 13, 106, 54, 10, 4294967295, 0x0082, nullptr, 0xffff, L"A&M symbol:"},
    {0x00000000, 0x00000000, 0x50210102, 68, 104, 84, 100, 1091, 0x0085, nullptr, 0xfffe, nullptr},
    {0x00000000, 0x00000000, 0x50020000, 13, 123, 54, 10, 4294967295, 0x0082, nullptr, 0xffff, L"&PM symbol:"},
    {0x00000000, 0x00000000, 0x50210102, 68, 121, 84, 100, 1092, 0x0085, nullptr, 0xfffe, nullptr},
    {0x00000000, 0x00000000, 0x50020000, 13, 144, 226, 89, 4294967295, 0x0082, nullptr, 0xffff, L"What the notations mean:\n\nh = hour   m = minute\ns = second (long time only)\ntt = A.M. or P.M.\n\nh/H = 12/24 hour\n\nhh, mm, ss = display leading zero\nh, m, s = do not display leading zero"},
    {0x00000000, 0x00000000, 0x50020000, 7, 236, 185, 16, 4294967295, 0x0082, nullptr, 0xffff, L"Click Reset to restore the system default settings for numbers, currency, time, and date."},
    {0x00000000, 0x00000000, 0x50010000, 196, 237, 50, 13, 1046, 0x0080, nullptr, 0xffff, L"&Reset"},
};
static const EmbCtl kDlg108Ctl[] = {
    {0x00000000, 0x00000000, 0x50000007, 5, 7, 242, 51, 1001, 0x0080, nullptr, 0xffff, L"Example"},
    {0x00000000, 0x00000000, 0x50020000, 13, 20, 63, 10, 1007, 0x0082, nullptr, 0xffff, L"Short date:"},
    {0x00000000, 0x00000000, 0x50800880, 77, 18, 110, 14, 1005, 0x0081, nullptr, 0xfffe, nullptr},
    {0x00000000, 0x00000000, 0x50020000, 13, 37, 61, 10, 1008, 0x0082, nullptr, 0xffff, L"Long date:"},
    {0x00000000, 0x00000000, 0x50800880, 77, 35, 110, 14, 1006, 0x0081, nullptr, 0xfffe, nullptr},
    {0x00000000, 0x00000000, 0x50000007, 5, 65, 242, 70, 1002, 0x0080, nullptr, 0xffff, L"Date formats"},
    {0x00000000, 0x00000000, 0x50020000, 13, 77, 60, 10, 4294967295, 0x0082, nullptr, 0xffff, L"&Short date:"},
    {0x00000000, 0x00000000, 0x50210142, 77, 75, 161, 100, 1106, 0x0085, nullptr, 0xfffe, nullptr},
    {0x00000000, 0x00000000, 0x50020000, 13, 95, 60, 10, 4294967295, 0x0082, nullptr, 0xffff, L"&Long date:"},
    {0x00000000, 0x00000000, 0x50210142, 77, 92, 161, 100, 1107, 0x0085, nullptr, 0xfffe, nullptr},
    {0x00000000, 0x00000000, 0x50020000, 13, 111, 226, 20, 1009, 0x0082, nullptr, 0xffff, L"What the notations mean:\nd, dd = day;  ddd, dddd = day of week;  M = month;  y = year"},
    {0x00000000, 0x00000000, 0x50000007, 5, 143, 242, 85, 1003, 0x0080, nullptr, 0xffff, L"Calendar"},
    {0x00000000, 0x00000000, 0x50020000, 13, 154, 225, 8, 4294967295, 0x0082, nullptr, 0xffff, L"W&hen a two-digit year is entered, interpret it as a year between:"},
    {0x00000000, 0x00000000, 0x58812080, 18, 165, 36, 12, 1102, 0x0081, nullptr, 0xfffe, nullptr},
    {0x00000000, 0x00000000, 0x50020000, 58, 167, 17, 8, 4294967295, 0x0082, nullptr, 0xffff, L"and"},
    {0x00000000, 0x00000000, 0x50812000, 77, 165, 37, 12, 1103, 0x0081, nullptr, 0xfffe, nullptr},
    {0x00000000, 0x00000000, 0x500000b7, 113, 165, 10, 12, 1104, 0xffff, L"msctls_updown32", 0xfffe, nullptr},
    {0x00000000, 0x00000000, 0x50020000, 13, 183, 60, 10, 4294967295, 0x0082, nullptr, 0xffff, L"&First day of week:"},
    {0x00000000, 0x00000000, 0x50210003, 77, 181, 161, 100, 1109, 0x0085, nullptr, 0xfffe, nullptr},
    {0x00000000, 0x00000000, 0x50020000, 13, 199, 56, 10, 1100, 0x0082, nullptr, 0xffff, L"&Calendar type:"},
    {0x00000000, 0x00000000, 0x50210103, 77, 197, 161, 100, 1101, 0x0085, nullptr, 0xfffe, nullptr},
    {0x00000000, 0x00000000, 0x50020000, 13, 215, 63, 10, 1108, 0x0082, nullptr, 0xffff, L"A&djust Hijri date to:"},
    {0x00000000, 0x00000000, 0x50210003, 77, 213, 161, 100, 1105, 0x0085, nullptr, 0xfffe, nullptr},
    {0x00000000, 0x00000000, 0x50020000, 7, 236, 185, 16, 1047, 0x0082, nullptr, 0xffff, L"Click Reset to restore the system default settings for numbers, currency, time, and date."},
    {0x00000000, 0x00000000, 0x50010000, 196, 237, 50, 13, 1046, 0x0080, nullptr, 0xffff, L"&Reset"},
};
static const EmbCtl kDlg109Ctl[] = {
    {0x00000000, 0x00000000, 0x50020000, 7, 15, 227, 20, 1121, 0x0082, nullptr, 0xffff, L"You can control the way some programs sort characters, words, files, and folders."},
    {0x00000000, 0x00000000, 0x50020000, 7, 38, 228, 8, 1122, 0x0082, nullptr, 0xffff, L"&Select the sorting method:"},
    {0x00000000, 0x00000000, 0x50210043, 7, 48, 230, 40, 1120, 0x0085, nullptr, 0xfffe, nullptr},
    {0x00000000, 0x00000000, 0x50020000, 7, 236, 185, 16, 1047, 0x0082, nullptr, 0xffff, L"Click Reset to restore the system default settings for numbers, currency, time, and date."},
    {0x00000000, 0x00000000, 0x50010000, 196, 237, 50, 13, 1046, 0x0080, nullptr, 0xffff, L"&Reset"},
};
static const EmbCtl kDlg600Ctl[] = {
    {0x00000000, 0x00000000, 0x50020000, 5, 8, 242, 24, 1022, 0x0082, nullptr, 0xffff, L"Some software, including Windows, may provide you with additional content for a particular location. Some services provide local information such as news and weather."},
    {0x00000000, 0x00000000, 0x50020000, 5, 38, 150, 10, 1027, 0x0082, nullptr, 0xffff, L"&Current location:"},
    {0x00000000, 0x00000000, 0x50210103, 5, 50, 237, 60, 1023, 0x0085, nullptr, 0xfffe, nullptr},
    {0x00000000, 0x00000000, 0x50020000, 5, 203, 150, 10, 1014, 0x0082, nullptr, 0xffff, L"See also"},
    {0x00000000, 0x00000000, 0x50010000, 5, 215, 238, 13, 1045, 0xffff, L"SysLink", 0xffff, L"<A>Default location</A>"},
};
static const EmbCtl kDlg700Ctl[] = {
    {0x00000000, 0x00000000, 0x50020000, 8, 7, 244, 16, 4294967295, 0x0082, nullptr, 0xffff, L"The &settings for the current user, welcome screen (system accounts) and new user accounts are displayed below."},
    {0x00000000, 0x00000000, 0x50814001, 8, 26, 244, 180, 2110, 0xffff, L"SysListView32", 0xfffe, nullptr},
    {0x00000000, 0x00000000, 0x50020000, 8, 206, 244, 8, 1026, 0x0082, nullptr, 0xffff, L"* Custom locale"},
    {0x00000000, 0x00000000, 0x50020000, 8, 220, 244, 8, 4294967295, 0x0082, nullptr, 0xffff, L"Copy your current settings to:"},
    {0x00000000, 0x00000000, 0x50012403, 18, 232, 232, 10, 1055, 0x0080, nullptr, 0xffff, L"&Welcome screen and system accounts"},
    {0x00000000, 0x00000000, 0x50012403, 18, 244, 232, 10, 1054, 0x0080, nullptr, 0xffff, L"&New user accounts"},
    {0x00000000, 0x00000000, 0x50020000, 18, 256, 234, 16, 3040, 0x0082, nullptr, 0xffff, L"The new user accounts display language is currently inherited from the welcome screen display language."},
    {0x00000000, 0x00000000, 0x50010000, 144, 276, 50, 14, 1, 0x0080, nullptr, 0xffff, L"OK"},
    {0x00000000, 0x00000000, 0x50010000, 202, 276, 50, 14, 2, 0x0080, nullptr, 0xffff, L"Cancel"},
};
static const EmbCtl kDlg800Ctl[] = {
    {0x00000000, 0x00000000, 0x50020000, 5, 7, 239, 26, 1050, 0x0082, nullptr, 0xffff, L"Select which language (system locale) to use when displaying text in programs that do not support Unicode. This setting affects all user accounts on the computer."},
    {0x00000000, 0x00000000, 0x50020000, 5, 37, 239, 10, 2101, 0x0082, nullptr, 0xffff, L"&Current system locale:"},
    {0x00000000, 0x00000000, 0x50210103, 5, 47, 239, 100, 1052, 0x0085, nullptr, 0xfffe, nullptr},
    {0x00000000, 0x00000000, 0x50020000, 5, 67, 225, 16, 1026, 0x0082, nullptr, 0xffff, L"* Custom locale"},
    {0x00000000, 0x00000000, 0x50010000, 140, 80, 50, 14, 1, 0x0080, nullptr, 0xffff, L"OK"},
    {0x00000000, 0x00000000, 0x50010000, 194, 80, 50, 14, 2, 0x0080, nullptr, 0xffff, L"Cancel"},
};
static const EmbDlg kEmbDialogs[] = {
    {101, 0x00000000, 0x00000000, 0x90c001c4, 0, 0, 252, 236, L"Formats", 9, 0, 0, 0, L"Segoe UI", 26, kDlg101Ctl, 1542, 0x9f4165c2},
    {102, 0x00000000, 0x00000000, 0x90c001cc, 0, 0, 252, 236, L"Keyboards and Languages", 9, 0, 0, 0, L"Segoe UI", 18, kDlg102Ctl, 2644, 0x31d774a1},
    {104, 0x00000000, 0x00000000, 0x90c001cc, 0, 0, 252, 236, L"Administrative", 9, 0, 0, 0, L"Segoe UI", 10, kDlg104Ctl, 1346, 0x1bb8362a},
    {105, 0x00000000, 0x00000000, 0x90c001cc, 0, 0, 252, 254, L"Numbers", 9, 0, 0, 0, L"Segoe UI", 33, kDlg105Ctl, 1876, 0x0268b747},
    {106, 0x00000000, 0x00000000, 0x90c001cc, 0, 0, 252, 254, L"Currency", 9, 0, 0, 0, L"Segoe UI", 21, kDlg106Ctl, 1304, 0x78b5b880},
    {107, 0x00000000, 0x00000000, 0x90c001cc, 0, 0, 252, 254, L"Time", 9, 0, 0, 0, L"Segoe UI", 19, kDlg107Ctl, 1412, 0x138a9afa},
    {108, 0x00000000, 0x00000000, 0x90c001cc, 0, 0, 252, 254, L"Date", 9, 0, 0, 0, L"Segoe UI", 25, kDlg108Ctl, 1660, 0xea017664},
    {109, 0x00000000, 0x00000000, 0x90c001cc, 0, 0, 252, 254, L"Sorting", 9, 0, 0, 0, L"Segoe UI", 5, kDlg109Ctl, 644, 0xbcc53f4b},
    {600, 0x00000000, 0x00000000, 0x90c001cc, 0, 0, 252, 236, L"Location", 9, 0, 0, 0, L"Segoe UI", 5, kDlg600Ctl, 674, 0x14a29c54},
    {700, 0x00000000, 0x00000000, 0x80c800c4, 0, 0, 260, 298, L"Welcome screen and new user accounts settings", 9, 0, 0, 0, L"Segoe UI", 9, kDlg700Ctl, 1108, 0xecfa427e},
    {800, 0x00000000, 0x00000000, 0x80c800cc, 0, 0, 252, 100, L"Region and Language Settings", 9, 0, 0, 0, L"Segoe UI", 6, kDlg800Ctl, 724, 0x65721653},
    {0},
};
static const short kDlg101CtlPhr[] = {-1,-1,84,0,1,-1,2,-1,3,-1,4,-1,5,-1,85,6,7,-1,8,-1,9,-1,10,-1,11,86};
static const short kDlg102CtlPhr[] = {12,13,14,87,15,16,17,18,19,20,-1,21,22,-1,23,24,-1,88};
static const short kDlg104CtlPhr[] = {25,26,27,89,28,29,30,31,32,90};
static const short kDlg105CtlPhr[] = {33,34,-1,35,-1,34,-1,35,-1,36,-1,37,-1,38,-1,39,-1,40,-1,41,-1,42,-1,43,-1,44,-1,45,-1,46,-1,47,48};
static const short kDlg106CtlPhr[] = {33,34,-1,35,-1,49,-1,50,-1,51,-1,36,-1,52,-1,53,-1,54,-1,47,48};
static const short kDlg107CtlPhr[] = {6,9,-1,-1,10,-1,-1,55,56,-1,57,-1,58,-1,59,-1,60,47,48};
static const short kDlg108CtlPhr[] = {33,7,-1,8,-1,61,1,-1,2,-1,62,63,64,-1,65,-1,-1,66,-1,67,-1,68,-1,47,48};
static const short kDlg109CtlPhr[] = {69,70,-1,47,48};
static const short kDlg600CtlPhr[] = {71,72,-1,73,91};
static const short kDlg700CtlPhr[] = {74,-1,75,76,77,78,79,80,81};
static const short kDlg800CtlPhr[] = {82,83,-1,75,80,81};
static const short* const kDlgPhrMaps[] = {kDlg101CtlPhr,kDlg102CtlPhr,kDlg104CtlPhr,kDlg105CtlPhr,kDlg106CtlPhr,kDlg107CtlPhr,kDlg108CtlPhr,kDlg109CtlPhr,kDlg600CtlPhr,kDlg700CtlPhr,kDlg800CtlPhr};
// ===== BEGIN LANGUAGE PACKS (v0.4.0) =====
// en-US (LangEN) is the genuine Microsoft text in restables.inc above and is
// the only pack verified  (size/FNV at build time). The 19
// packs below are mod-provided translations of the same items: they are NOT
// Microsoft text. Untranslatable items (format codes H h M m s t d y,
// examples .7 0.7, the "U.S." abbreviation, the "OK" button, the
// <systemLocale> placeholder) are nullptr = keep the English original.
// Dialog geometry stays en-US: translations are kept concise; rare clipping
// of the longest strings is a documented limitation (see readme).
enum LangIndex {
    LangEN = 0, LangIT, LangDE, LangFR, LangES, LangPT, LangNL, LangPL,
    LangRU, LangZH, LangJA, LangKO, LangTR, LangCS, LangHU, LangRO,
    LangSV, LangUK, LangEL, LangAR, LangCount
};

// ===== BEGIN GENERATED INPUT RESOURCE TABLES (input.dll) =====
// Parsed from the pinned Windows 7 input.dll. Dialogs 101, 106, 107, 108,
// 111, 500, 900 are DLGTEMPLATEEX resources that rebuild byte-identical;
// 112/113/114 are classic templates rebuilt canonically in EX form
// (their expectSize/expectFnv describe that canonical rebuild).
// Phrase maps index kInpDlgTr_*; titles use kInpTitlePhrase (112/113/114
// reuse the localized sheet title at build time).

static const EmbCtl kInpDlg101Ctl[] = {
    {0x00000000, 0x00000000, 0x50020000, 7, 7, 194, 10, 4294967295, 0x0082, nullptr, 0xffff, L"Select the language to add using the checkboxes below."},
    {0x00000000, 0x00000000, 0x50810137, 7, 20, 194, 190, 1001, 0xffff, L"SysTreeView32", 0xfffe, nullptr},
    {0x00000000, 0x00000000, 0x50010001, 210, 7, 50, 14, 1, 0x0080, nullptr, 0xffff, L"OK"},
    {0x00000000, 0x00000000, 0x50010000, 210, 24, 50, 14, 2, 0x0080, nullptr, 0xffff, L"Cancel"},
    {0x00000000, 0x00000000, 0x50010000, 210, 41, 50, 14, 1019, 0x0080, nullptr, 0xffff, L"&Preview..."},
};

static const EmbCtl kInpDlg106Ctl[] = {
    {0x00000000, 0x00000000, 0x50020007, 7, 6, 247, 61, 4294967295, 0x0080, nullptr, 0xffff, L"Language Bar "},
    {0x00000000, 0x00000000, 0x50030009, 17, 16, 142, 14, 1011, 0x0080, nullptr, 0xffff, L"&Floating On Desktop"},
    {0x00000000, 0x00000000, 0x50010009, 17, 32, 142, 14, 1012, 0x0080, nullptr, 0xffff, L"&Docked in the taskbar"},
    {0x00000000, 0x00000000, 0x50010009, 17, 50, 142, 14, 1013, 0x0080, nullptr, 0xffff, L"&Hidden"},
    {0x00000000, 0x00000000, 0x50010003, 7, 73, 246, 10, 1014, 0x0080, nullptr, 0xffff, L"Show the Language bar as transparent when i&nactive"},
    {0x00000000, 0x00000000, 0x50010003, 7, 90, 246, 10, 1015, 0x0080, nullptr, 0xffff, L"Show add&itional Language bar icons in the taskbar"},
    {0x00000000, 0x00000000, 0x50010003, 7, 107, 246, 10, 1016, 0x0080, nullptr, 0xffff, L"Show t&ext labels on the Language bar"},
};

static const EmbCtl kInpDlg107Ctl[] = {
    {0x00000000, 0x00000000, 0x50000007, 7, 7, 247, 26, 1030, 0x0080, nullptr, 0xffff, L"To turn off Caps Lock"},
    {0x00000000, 0x00000000, 0x50030009, 14, 17, 121, 11, 1031, 0x0080, nullptr, 0xffff, L"Press the CAPS &LOCK key"},
    {0x00000000, 0x00000000, 0x50000009, 141, 17, 110, 11, 1032, 0x0080, nullptr, 0xffff, L"Press the SHI&FT key"},
    {0x00000000, 0x00000000, 0x50000007, 7, 37, 247, 191, 1033, 0x0080, nullptr, 0xffff, L"Hot keys for input languages"},
    {0x00000000, 0x00000000, 0x50020000, 17, 47, 63, 9, 1034, 0x0082, nullptr, 0xffff, L"Action"},
    {0x00000000, 0x00000000, 0x50020002, 157, 47, 88, 9, 1035, 0x0082, nullptr, 0xffff, L"&Key sequence"},
    {0x00000000, 0x00000000, 0x50a10053, 15, 58, 232, 151, 1036, 0x0083, nullptr, 0xfffe, nullptr},
    {0x00000000, 0x00000000, 0x50030000, 141, 209, 106, 14, 1037, 0x0080, nullptr, 0xffff, L"&Change Key Sequence..."},
};

static const EmbCtl kInpDlg108Ctl[] = {
    {0x00000000, 0x00000000, 0x50010001, 214, 63, 50, 14, 1, 0x0080, nullptr, 0xffff, L"OK"},
    {0x00000000, 0x00000000, 0x50010000, 214, 80, 50, 14, 2, 0x0080, nullptr, 0xffff, L"Cancel"},
    {0x00000000, 0x00000000, 0x50000007, 7, 14, 94, 80, 4294967295, 0x0080, nullptr, 0xffff, L"Switch Input Language"},
    {0x00000000, 0x00000000, 0x50030009, 14, 28, 83, 10, 1090, 0x0080, nullptr, 0xffff, L"&Not Assigned"},
    {0x00000000, 0x00000000, 0x50010009, 14, 43, 83, 10, 1091, 0x0080, nullptr, 0xffff, L"&Ctrl + Shift"},
    {0x00000000, 0x00000000, 0x50010009, 14, 58, 83, 10, 1092, 0x0080, nullptr, 0xffff, L"&Left Alt + Shift"},
    {0x00000000, 0x00000000, 0x50010009, 14, 75, 83, 10, 1093, 0x0080, nullptr, 0xffff, L"&Grave Accent (`)"},
    {0x00000000, 0x00000000, 0x50000007, 112, 15, 94, 80, 4294967295, 0x0080, nullptr, 0xffff, L"Switch Keyboard Layout"},
    {0x00000000, 0x00000000, 0x50030009, 116, 29, 83, 10, 1094, 0x0080, nullptr, 0xffff, L"N&ot Assigned"},
    {0x00000000, 0x00000000, 0x50010009, 116, 44, 83, 10, 1095, 0x0080, nullptr, 0xffff, L"C&trl + Shift"},
    {0x00000000, 0x00000000, 0x50010009, 116, 58, 83, 10, 1096, 0x0080, nullptr, 0xffff, L"Le&ft Alt + Shift"},
    {0x00000000, 0x00000000, 0x50010009, 116, 76, 83, 10, 1097, 0x0080, nullptr, 0xffff, L"G&rave Accent (`)"},
};

static const EmbCtl kInpDlg111Ctl[] = {
    {0x00000000, 0x00000000, 0x50020000, 16, 5, 273, 10, 1080, 0x0082, nullptr, 0xfffe, nullptr},
    {0x00000000, 0x00000000, 0x50000007, 12, 17, 218, 48, 4294967295, 0x0080, nullptr, 0xfffe, nullptr},
    {0x00000000, 0x00000000, 0x50010003, 17, 17, 85, 8, 1050, 0x0080, nullptr, 0xffff, L"&Enable Key Sequence"},
    {0x00000000, 0x00000000, 0x50020001, 98, 37, 8, 10, 4294967295, 0x0082, nullptr, 0xffff, L"+"},
    {0x00000000, 0x00000000, 0x50210003, 24, 35, 80, 80, 1051, 0x0085, nullptr, 0xfffe, nullptr},
    {0x00000000, 0x00000000, 0x50020002, 110, 37, 23, 8, 4294967295, 0x0082, nullptr, 0xffff, L"&Key:"},
    {0x00000000, 0x00000000, 0x50210003, 137, 35, 81, 60, 1081, 0x0085, nullptr, 0xfffe, nullptr},
    {0x00000000, 0x00000000, 0x50010001, 239, 25, 50, 14, 1, 0x0080, nullptr, 0xffff, L"OK"},
    {0x00000000, 0x00000000, 0x50010000, 239, 47, 50, 14, 2, 0x0080, nullptr, 0xffff, L"Cancel"},
};

static const EmbCtl kInpDlg500Ctl[] = {
    {0x00000000, 0x00000000, 0x50000007, 7, 7, 248, 53, 4294967295, 0x0080, nullptr, 0xffff, L"Default input &language"},
    {0x00000000, 0x00000000, 0x50020000, 14, 17, 235, 18, 4294967295, 0x0082, nullptr, 0xffff, L"Select one of the installed input languages to use as the default for all input fields."},
    {0x00000000, 0x00000000, 0x50210103, 14, 40, 236, 60, 1002, 0x0085, nullptr, 0xfffe, nullptr},
    {0x00000000, 0x00000000, 0x50000007, 7, 65, 248, 159, 4294967295, 0x0080, nullptr, 0xffff, L"&Installed services"},
    {0x00000000, 0x00000000, 0x50020000, 14, 75, 235, 18, 4294967295, 0x0082, nullptr, 0xffff, L"Select the services that you want for each input language shown in the list. Use the Add and Remove buttons to modify this list."},
    {0x00000000, 0x00000000, 0x50a10032, 14, 98, 170, 120, 1001, 0xffff, L"SysTreeView32", 0xfffe, nullptr},
    {0x00000000, 0x00000000, 0x50030000, 187, 133, 64, 14, 1003, 0x0080, nullptr, 0xffff, L"A&dd..."},
    {0x00000000, 0x00000000, 0x50030000, 187, 151, 64, 14, 1004, 0x0080, nullptr, 0xffff, L"&Remove"},
    {0x00000000, 0x00000000, 0x50030000, 187, 169, 64, 14, 1019, 0x0080, nullptr, 0xffff, L"&Properties..."},
    {0x00000000, 0x00000000, 0x50030000, 187, 187, 64, 14, 1020, 0x0080, nullptr, 0xffff, L"Move &Up"},
    {0x00000000, 0x00000000, 0x50030000, 187, 205, 64, 14, 1021, 0x0080, nullptr, 0xffff, L"Move D&own"},
};

static const EmbCtl kInpDlg900Ctl[] = {
    {0x00000000, 0x00000000, 0x50010001, 220, 122, 50, 14, 1, 0x0080, nullptr, 0xffff, L"OK"},
    {0x00000000, 0x00000000, 0x50010000, 277, 122, 50, 14, 2, 0x0080, nullptr, 0xffff, L"Cancel"},
    {0x00000000, 0x00000000, 0x50000007, 5, 35, 20, 20, 2000, 0x0080, nullptr, 0xfffe, nullptr},
    {0x00000000, 0x00000000, 0x50020001, 7, 43, 16, 10, 2001, 0x0082, nullptr, 0xfffe, nullptr},
    {0x00000000, 0x00000000, 0x50000007, 27, 35, 20, 20, 2002, 0x0080, nullptr, 0xfffe, nullptr},
    {0x00000000, 0x00000000, 0x50020001, 29, 43, 16, 10, 2003, 0x0082, nullptr, 0xfffe, nullptr},
    {0x00000000, 0x00000000, 0x50000007, 49, 35, 20, 20, 2004, 0x0080, nullptr, 0xfffe, nullptr},
    {0x00000000, 0x00000000, 0x50020001, 51, 43, 16, 10, 2005, 0x0082, nullptr, 0xfffe, nullptr},
    {0x00000000, 0x00000000, 0x50000007, 71, 35, 20, 20, 2006, 0x0080, nullptr, 0xfffe, nullptr},
    {0x00000000, 0x00000000, 0x50020001, 73, 43, 16, 10, 2007, 0x0082, nullptr, 0xfffe, nullptr},
    {0x00000000, 0x00000000, 0x50000007, 93, 35, 20, 20, 2008, 0x0080, nullptr, 0xfffe, nullptr},
    {0x00000000, 0x00000000, 0x50020001, 95, 43, 16, 10, 2009, 0x0082, nullptr, 0xfffe, nullptr},
    {0x00000000, 0x00000000, 0x50000007, 117, 35, 20, 20, 2010, 0x0080, nullptr, 0xfffe, nullptr},
    {0x00000000, 0x00000000, 0x50020001, 119, 43, 16, 10, 2011, 0x0082, nullptr, 0xfffe, nullptr},
    {0x00000000, 0x00000000, 0x50000007, 139, 35, 20, 20, 2012, 0x0080, nullptr, 0xfffe, nullptr},
    {0x00000000, 0x00000000, 0x50020001, 141, 43, 16, 10, 2013, 0x0082, nullptr, 0xfffe, nullptr},
    {0x00000000, 0x00000000, 0x50000007, 161, 35, 20, 20, 2014, 0x0080, nullptr, 0xfffe, nullptr},
    {0x00000000, 0x00000000, 0x50020001, 163, 43, 16, 10, 2015, 0x0082, nullptr, 0xfffe, nullptr},
    {0x00000000, 0x00000000, 0x50000007, 183, 35, 20, 20, 2016, 0x0080, nullptr, 0xfffe, nullptr},
    {0x00000000, 0x00000000, 0x50020001, 185, 43, 16, 10, 2017, 0x0082, nullptr, 0xfffe, nullptr},
    {0x00000000, 0x00000000, 0x50000007, 205, 35, 20, 20, 2018, 0x0080, nullptr, 0xfffe, nullptr},
    {0x00000000, 0x00000000, 0x50020001, 207, 43, 16, 10, 2019, 0x0082, nullptr, 0xfffe, nullptr},
    {0x00000000, 0x00000000, 0x50000007, 227, 35, 20, 20, 2020, 0x0080, nullptr, 0xfffe, nullptr},
    {0x00000000, 0x00000000, 0x50020001, 229, 43, 16, 10, 2021, 0x0082, nullptr, 0xfffe, nullptr},
    {0x00000000, 0x00000000, 0x50000007, 249, 35, 20, 20, 2022, 0x0080, nullptr, 0xfffe, nullptr},
    {0x00000000, 0x00000000, 0x50020001, 251, 43, 16, 10, 2023, 0x0082, nullptr, 0xfffe, nullptr},
    {0x00000000, 0x00000000, 0x50000007, 271, 35, 20, 20, 2024, 0x0080, nullptr, 0xfffe, nullptr},
    {0x00000000, 0x00000000, 0x50020001, 273, 43, 16, 10, 2025, 0x0082, nullptr, 0xfffe, nullptr},
    {0x00000000, 0x00000000, 0x50000007, 37, 55, 20, 20, 2026, 0x0080, nullptr, 0xfffe, nullptr},
    {0x00000000, 0x00000000, 0x50020001, 39, 63, 16, 10, 2027, 0x0082, nullptr, 0xfffe, nullptr},
    {0x00000000, 0x00000000, 0x50000007, 59, 55, 20, 20, 2028, 0x0080, nullptr, 0xfffe, nullptr},
    {0x00000000, 0x00000000, 0x50020001, 61, 63, 16, 10, 2029, 0x0082, nullptr, 0xfffe, nullptr},
    {0x00000000, 0x00000000, 0x50000007, 81, 55, 20, 20, 2030, 0x0080, nullptr, 0xfffe, nullptr},
    {0x00000000, 0x00000000, 0x50020001, 83, 63, 16, 10, 2031, 0x0082, nullptr, 0xfffe, nullptr},
    {0x00000000, 0x00000000, 0x50000007, 103, 55, 20, 20, 2032, 0x0080, nullptr, 0xfffe, nullptr},
    {0x00000000, 0x00000000, 0x50020001, 105, 63, 16, 10, 2033, 0x0082, nullptr, 0xfffe, nullptr},
    {0x00000000, 0x00000000, 0x50000007, 125, 55, 20, 20, 2034, 0x0080, nullptr, 0xfffe, nullptr},
    {0x00000000, 0x00000000, 0x50020001, 127, 63, 16, 10, 2035, 0x0082, nullptr, 0xfffe, nullptr},
    {0x00000000, 0x00000000, 0x50000007, 147, 55, 20, 20, 2036, 0x0080, nullptr, 0xfffe, nullptr},
    {0x00000000, 0x00000000, 0x50020001, 149, 63, 16, 10, 2037, 0x0082, nullptr, 0xfffe, nullptr},
    {0x00000000, 0x00000000, 0x50000007, 169, 55, 20, 20, 2038, 0x0080, nullptr, 0xfffe, nullptr},
    {0x00000000, 0x00000000, 0x50020001, 171, 63, 16, 10, 2039, 0x0082, nullptr, 0xfffe, nullptr},
    {0x00000000, 0x00000000, 0x50000007, 191, 55, 20, 20, 2040, 0x0080, nullptr, 0xfffe, nullptr},
    {0x00000000, 0x00000000, 0x50020001, 193, 63, 16, 10, 2041, 0x0082, nullptr, 0xfffe, nullptr},
    {0x00000000, 0x00000000, 0x50000007, 213, 55, 20, 20, 2042, 0x0080, nullptr, 0xfffe, nullptr},
    {0x00000000, 0x00000000, 0x50020001, 215, 63, 16, 10, 2043, 0x0082, nullptr, 0xfffe, nullptr},
    {0x00000000, 0x00000000, 0x50000007, 235, 55, 20, 20, 2044, 0x0080, nullptr, 0xfffe, nullptr},
    {0x00000000, 0x00000000, 0x50020001, 237, 63, 16, 10, 2045, 0x0082, nullptr, 0xfffe, nullptr},
    {0x00000000, 0x00000000, 0x50000007, 257, 55, 20, 20, 2046, 0x0080, nullptr, 0xfffe, nullptr},
    {0x00000000, 0x00000000, 0x50020001, 259, 63, 16, 10, 2047, 0x0082, nullptr, 0xfffe, nullptr},
    {0x00000000, 0x00000000, 0x50000007, 279, 55, 20, 20, 2048, 0x0080, nullptr, 0xfffe, nullptr},
    {0x00000000, 0x00000000, 0x50020001, 281, 63, 16, 10, 2049, 0x0082, nullptr, 0xfffe, nullptr},
    {0x00000000, 0x00000000, 0x50000007, 301, 55, 26, 20, 2050, 0x0080, nullptr, 0xfffe, nullptr},
    {0x00000000, 0x00000000, 0x50020001, 303, 63, 22, 10, 2051, 0x0082, nullptr, 0xfffe, nullptr},
    {0x00000000, 0x00000000, 0x50000007, 45, 75, 20, 20, 2052, 0x0080, nullptr, 0xfffe, nullptr},
    {0x00000000, 0x00000000, 0x50020001, 47, 83, 16, 10, 2053, 0x0082, nullptr, 0xfffe, nullptr},
    {0x00000000, 0x00000000, 0x50000007, 67, 75, 20, 20, 2054, 0x0080, nullptr, 0xfffe, nullptr},
    {0x00000000, 0x00000000, 0x50020001, 69, 83, 16, 10, 2055, 0x0082, nullptr, 0xfffe, nullptr},
    {0x00000000, 0x00000000, 0x50000007, 89, 75, 20, 20, 2056, 0x0080, nullptr, 0xfffe, nullptr},
    {0x00000000, 0x00000000, 0x50020001, 91, 83, 16, 10, 2057, 0x0082, nullptr, 0xfffe, nullptr},
    {0x00000000, 0x00000000, 0x50000007, 111, 75, 20, 20, 2058, 0x0080, nullptr, 0xfffe, nullptr},
    {0x00000000, 0x00000000, 0x50020001, 113, 83, 16, 10, 2059, 0x0082, nullptr, 0xfffe, nullptr},
    {0x00000000, 0x00000000, 0x50000007, 133, 75, 20, 20, 2060, 0x0080, nullptr, 0xfffe, nullptr},
    {0x00000000, 0x00000000, 0x50020001, 135, 83, 16, 10, 2061, 0x0082, nullptr, 0xfffe, nullptr},
    {0x00000000, 0x00000000, 0x50000007, 155, 75, 20, 20, 2062, 0x0080, nullptr, 0xfffe, nullptr},
    {0x00000000, 0x00000000, 0x50020001, 157, 83, 16, 10, 2063, 0x0082, nullptr, 0xfffe, nullptr},
    {0x00000000, 0x00000000, 0x50000007, 177, 75, 20, 20, 2064, 0x0080, nullptr, 0xfffe, nullptr},
    {0x00000000, 0x00000000, 0x50020001, 179, 83, 16, 10, 2065, 0x0082, nullptr, 0xfffe, nullptr},
    {0x00000000, 0x00000000, 0x50000007, 199, 75, 20, 20, 2066, 0x0080, nullptr, 0xfffe, nullptr},
    {0x00000000, 0x00000000, 0x50020001, 201, 83, 16, 10, 2067, 0x0082, nullptr, 0xfffe, nullptr},
    {0x00000000, 0x00000000, 0x50000007, 221, 75, 20, 20, 2068, 0x0080, nullptr, 0xfffe, nullptr},
    {0x00000000, 0x00000000, 0x50020001, 223, 83, 16, 10, 2069, 0x0082, nullptr, 0xfffe, nullptr},
    {0x00000000, 0x00000000, 0x50000007, 243, 75, 20, 20, 2070, 0x0080, nullptr, 0xfffe, nullptr},
    {0x00000000, 0x00000000, 0x50020001, 245, 83, 16, 10, 2071, 0x0082, nullptr, 0xfffe, nullptr},
    {0x00000000, 0x00000000, 0x50000007, 265, 75, 20, 20, 2072, 0x0080, nullptr, 0xfffe, nullptr},
    {0x00000000, 0x00000000, 0x50020001, 267, 83, 16, 10, 2073, 0x0082, nullptr, 0xfffe, nullptr},
    {0x00000000, 0x00000000, 0x50000007, 56, 95, 20, 20, 2074, 0x0080, nullptr, 0xfffe, nullptr},
    {0x00000000, 0x00000000, 0x50020001, 59, 103, 16, 10, 2075, 0x0082, nullptr, 0xfffe, nullptr},
    {0x00000000, 0x00000000, 0x50000007, 79, 95, 20, 20, 2076, 0x0080, nullptr, 0xfffe, nullptr},
    {0x00000000, 0x00000000, 0x50020001, 81, 103, 16, 10, 2077, 0x0082, nullptr, 0xfffe, nullptr},
    {0x00000000, 0x00000000, 0x50000007, 101, 95, 20, 20, 2078, 0x0080, nullptr, 0xfffe, nullptr},
    {0x00000000, 0x00000000, 0x50020001, 103, 103, 16, 10, 2079, 0x0082, nullptr, 0xfffe, nullptr},
    {0x00000000, 0x00000000, 0x50000007, 123, 95, 20, 20, 2080, 0x0080, nullptr, 0xfffe, nullptr},
    {0x00000000, 0x00000000, 0x50020001, 125, 103, 16, 10, 2081, 0x0082, nullptr, 0xfffe, nullptr},
    {0x00000000, 0x00000000, 0x50000007, 145, 95, 20, 20, 2082, 0x0080, nullptr, 0xfffe, nullptr},
    {0x00000000, 0x00000000, 0x50020001, 147, 103, 16, 10, 2083, 0x0082, nullptr, 0xfffe, nullptr},
    {0x00000000, 0x00000000, 0x50000007, 167, 95, 20, 20, 2084, 0x0080, nullptr, 0xfffe, nullptr},
    {0x00000000, 0x00000000, 0x50020001, 169, 103, 16, 10, 2085, 0x0082, nullptr, 0xfffe, nullptr},
    {0x00000000, 0x00000000, 0x50000007, 189, 95, 20, 20, 2086, 0x0080, nullptr, 0xfffe, nullptr},
    {0x00000000, 0x00000000, 0x50020001, 191, 103, 16, 10, 2087, 0x0082, nullptr, 0xfffe, nullptr},
    {0x00000000, 0x00000000, 0x50000007, 211, 95, 20, 20, 2088, 0x0080, nullptr, 0xfffe, nullptr},
    {0x00000000, 0x00000000, 0x50020001, 213, 103, 16, 10, 2089, 0x0082, nullptr, 0xfffe, nullptr},
    {0x00000000, 0x00000000, 0x50000007, 233, 95, 20, 20, 2090, 0x0080, nullptr, 0xfffe, nullptr},
    {0x00000000, 0x00000000, 0x50020001, 235, 103, 16, 10, 2091, 0x0082, nullptr, 0xfffe, nullptr},
    {0x00000000, 0x00000000, 0x50000007, 255, 95, 20, 20, 2092, 0x0080, nullptr, 0xfffe, nullptr},
    {0x00000000, 0x00000000, 0x50020001, 257, 103, 16, 10, 2093, 0x0082, nullptr, 0xfffe, nullptr},
    {0x00000000, 0x00000000, 0x50000007, 5, 55, 29, 20, 2094, 0x0080, nullptr, 0xfffe, nullptr},
    {0x00000000, 0x00000000, 0x50020001, 7, 63, 25, 10, 2095, 0x0082, nullptr, 0xffff, L"Tab"},
    {0x00000000, 0x00000000, 0x50000007, 5, 75, 38, 20, 2096, 0x0080, nullptr, 0xfffe, nullptr},
    {0x00000000, 0x00000000, 0x50020001, 7, 83, 34, 10, 2097, 0x0082, nullptr, 0xffff, L"Caps"},
    {0x00000000, 0x00000000, 0x50000007, 5, 95, 49, 20, 2098, 0x0080, nullptr, 0xfffe, nullptr},
    {0x00000000, 0x00000000, 0x50020001, 7, 103, 45, 10, 2099, 0x0082, nullptr, 0xffff, L"Shift"},
    {0x00000000, 0x00000000, 0x50000007, 277, 95, 50, 20, 2100, 0x0080, nullptr, 0xfffe, nullptr},
    {0x00000000, 0x00000000, 0x50020001, 279, 103, 46, 10, 2101, 0x0082, nullptr, 0xffff, L"Shift"},
    {0x00000000, 0x00000000, 0x50000007, 287, 75, 40, 20, 2102, 0x0080, nullptr, 0xfffe, nullptr},
    {0x00000000, 0x00000000, 0x50020001, 289, 83, 36, 10, 2103, 0x0082, nullptr, 0xffff, L"Enter"},
    {0x00000000, 0x00000000, 0x50000007, 293, 35, 34, 20, 2104, 0x0080, nullptr, 0xfffe, nullptr},
    {0x00000000, 0x00000000, 0x50020001, 295, 43, 30, 10, 2105, 0x0082, nullptr, 0xffff, L"BackSp"},
    {0x00000000, 0x00000000, 0x50020000, 7, 16, 50, 10, 4294967295, 0x0082, nullptr, 0xffff, L"Layout Name:"},
    {0x00000000, 0x00000000, 0x50020000, 60, 16, 140, 10, 901, 0x0082, nullptr, 0xfffe, nullptr},
    {0x00000000, 0x00000000, 0x50000043, 222, 5, 32, 32, 902, 0x0082, nullptr, 0xfffe, nullptr},
    {0x00000000, 0x00000000, 0x50010000, 267, 15, 60, 15, 903, 0x0080, nullptr, 0xffff, L"&Change Icon..."},
};

static const EmbCtl kInpDlg112Ctl[] = {
    {0x00000000, 0x00000000, 0x50020001, 3, 7, 189, 20, 1100, 0x0082, nullptr, 0xfffe, nullptr},
    {0x00000000, 0x00000000, 0x50010003, 27, 33, 137, 10, 1101, 0x0080, nullptr, 0xffff, L"&Do not show me this message again."},
    {0x00000000, 0x00000000, 0x50010001, 30, 47, 50, 14, 1, 0x0080, nullptr, 0xffff, L"OK"},
    {0x00000000, 0x00000000, 0x50010000, 107, 47, 50, 14, 2, 0x0080, nullptr, 0xffff, L"Cancel"},
};

static const EmbCtl kInpDlg113Ctl[] = {
    {0x00000000, 0x00000000, 0x50020001, 3, 7, 250, 60, 1102, 0x0082, nullptr, 0xfffe, nullptr},
    {0x00000000, 0x00000000, 0x50010003, 60, 73, 130, 10, 1101, 0x0080, nullptr, 0xffff, L"&Do not show me this message again."},
    {0x00000000, 0x00000000, 0x50010001, 50, 87, 50, 14, 1, 0x0080, nullptr, 0xffff, L"OK"},
    {0x00000000, 0x00000000, 0x50010000, 150, 87, 50, 14, 2, 0x0080, nullptr, 0xffff, L"Cancel"},
};

static const EmbCtl kInpDlg114Ctl[] = {
    {0x00000000, 0x00000000, 0x50020001, 3, 7, 250, 60, 1103, 0x0082, nullptr, 0xfffe, nullptr},
    {0x00000000, 0x00000000, 0x50010001, 100, 77, 50, 14, 1, 0x0080, nullptr, 0xffff, L"OK"},
};

static const EmbDlg kInpDialogs[] = {
    {101, 0x00000000, 0x00000000, 0x80cc00c8, 0, 0, 267, 214, L"Add Input Language", 8, 400, 0, 1, L"MS Shell Dlg", 5, kInpDlg101Ctl, 430, 0xb742b491},
    {106, 0x00000000, 0x00000000, 0x90c001c4, 0, 0, 263, 236, L"Language Bar", 8, 0, 0, 0, L"MS Shell Dlg", 7, kInpDlg106Ctl, 718, 0x17ee5a32},
    {107, 0x00000000, 0x00000000, 0x90c001c4, 0, 0, 263, 236, L"Advanced Key Settings", 8, 0, 0, 1, L"MS Shell Dlg", 8, kInpDlg107Ctl, 638, 0x8001910c},
    {108, 0x00000000, 0x00000000, 0x80c800c8, 0, 0, 271, 103, L"Change Key Sequence", 8, 400, 0, 1, L"MS Shell Dlg", 12, kInpDlg108Ctl, 846, 0x912bfd8f},
    {111, 0x00000000, 0x00000000, 0x90c801c4, 5, 100, 298, 77, L"Change Key Sequence", 8, 0, 0, 1, L"MS Shell Dlg", 9, kInpDlg111Ctl, 464, 0x434b9344},
    {500, 0x00000000, 0x00000000, 0x90c001c4, 0, 0, 263, 236, L"General", 8, 400, 0, 1, L"MS Shell Dlg", 11, kInpDlg500Ctl, 1072, 0xeac3d3bf},
    {900, 0x00000000, 0x00000000, 0x80c800c8, 0, 0, 334, 140, L"Keyboard Layout Preview", 8, 400, 0, 1, L"MS Shell Dlg", 112, kInpDlg900Ctl, 3830, 0x0d1a6b95},
    {112, 0x00000000, 0x00000000, 0x80c800c0, 0, 0, 196, 63, L"Text Services and Input Languages", 8, 400, 0, 1, L"MS Shell Dlg", 4, kInpDlg112Ctl, 348, 0xc67b55f2},
    {113, 0x00000000, 0x00000000, 0x80c800c0, 0, 0, 253, 103, L"Text Services and Input Languages", 8, 400, 0, 1, L"MS Shell Dlg", 4, kInpDlg113Ctl, 348, 0x90159673},
    {114, 0x00000000, 0x00000000, 0x80c800c0, 0, 0, 253, 93, L"Text Services and Input Languages", 8, 400, 0, 1, L"MS Shell Dlg", 2, kInpDlg114Ctl, 200, 0xc04192f0},
    {0},
};

static const short kInpDlg101CtlPhr[] = {3,-1,0,1,4};

static const short kInpDlg106CtlPhr[] = {5,6,7,8,9,10,11};

static const short kInpDlg107CtlPhr[] = {13,14,15,16,17,18,-1,19};

static const short kInpDlg108CtlPhr[] = {0,1,21,22,23,24,25,26,27,28,29,30};

static const short kInpDlg111CtlPhr[] = {-1,-1,31,-1,-1,32,-1,0,1};

static const short kInpDlg500CtlPhr[] = {34,35,-1,36,37,-1,38,39,40,41,42};

static const short kInpDlg900CtlPhr[] = {0,1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,44,-1,-1,45};

static const short kInpDlg112CtlPhr[] = {-1,46,0,1};

static const short kInpDlg113CtlPhr[] = {-1,46,0,1};

static const short kInpDlg114CtlPhr[] = {-1,0};

static const short* const kInpPhrMaps[] = {kInpDlg101CtlPhr,kInpDlg106CtlPhr,kInpDlg107CtlPhr,kInpDlg108CtlPhr,kInpDlg111CtlPhr,kInpDlg500CtlPhr,kInpDlg900CtlPhr,kInpDlg112CtlPhr,kInpDlg113CtlPhr,kInpDlg114CtlPhr};

static const short kInpTitlePhrase[] = {2,5,12,20,20,33,43};

// ===== END GENERATED INPUT RESOURCE TABLES =====
// BCP-47 tags in LangIndex order (settings values + log output).
static const wchar_t* const kLangTags[LangCount] = {
    L"en-US", L"it-IT", L"de-DE", L"fr-FR", L"es-ES", L"pt-BR", L"nl-NL",
    L"pl-PL", L"ru-RU", L"zh-CN", L"ja-JP", L"ko-KR", L"tr-TR", L"cs-CZ",
    L"hu-HU", L"ro-RO", L"sv-SE", L"uk-UA", L"el-GR", L"ar-SA"
};
// Text Services and Input Languages title, localized consistently with the
// twenty UI packs. This title belongs to the input-language dialog (102), not
// to the surrounding Region tab caption.
static const wchar_t* const kInputLanguagesTitle[LangCount] = {
    L"Text Services and Input Languages",
    L"Tastiere e lingue",
    L"Textdienste und Eingabesprachen",
    L"Services de texte et langues d’entrée",
    L"Servicios de texto e idiomas de entrada",
    L"Serviços de texto e idiomas de entrada",
    L"Tekstservices en invoertalen",
    L"Usługi tekstowe i języki wprowadzania",
    L"Текстовые службы и языки ввода",
    L"文本服务和输入语言",
    L"テキスト サービスと入力言語",
    L"텍스트 서비스 및 입력 언어",
    L"Metin hizmetleri ve giriş dilleri",
    L"Textové služby a vstupní jazyky",
    L"Szöveges szolgáltatások és beviteli nyelvek",
    L"Servicii de text și limbi de intrare",
    L"Texttjänster och inmatningsspråk",
    L"Текстові служби та мови введення",
    L"Υπηρεσίες κειμένου και γλώσσες εισόδου",
    L"خدمات النصوص ولغات الإدخال"
};
static const wchar_t* const kLangNames[LangCount] = {
    L"English (genuine Microsoft)", L"Italiano", L"Deutsch", L"Fran\u00E7ais",
    L"Espa\u00F1ol", L"Portugu\u00EAs (Brasil)", L"Nederlands", L"Polski",
    L"\u0420\u0443\u0441\u0441\u043A\u0438\u0439", L"\u4E2D\u6587\u7B80\u4F53",
    L"\u65E5\u672C\u8A9E", L"\uD55C\uAD6D\uC5B4", L"T\u00FCrk\u00E7e",
    L"\u010Ce\u0161tina", L"Magyar", L"Rom\u00E2n\u0103", L"Svenska",
    L"\u0423\u043A\u0440\u0430\u0457\u043D\u0441\u044C\u043A\u0430",
    L"\u0395\u03BB\u03BB\u03B7\u03BD\u03B9\u03BA\u03AC",
    L"\u0627\u0644\u0639\u0631\u0628\u064A\u0629"
};
// Only Arabic needs mirrored dialog templates (WS_EX_LAYOUTRTL at build).
bool LangRtl(int lang) { return lang == LangAR; }
// Automatic matching: OS UI language -> pack. Traditional Chinese and every
// unlisted language fall back to genuine en-US.
int AutoLanguage() {
    const WORD ui = GetUserDefaultUILanguage();
    switch (PRIMARYLANGID(ui)) {
        case LANG_ITALIAN: return LangIT;
        case LANG_GERMAN: return LangDE;
        case LANG_FRENCH: return LangFR;
        case LANG_SPANISH: return LangES;
        case LANG_PORTUGUESE: return LangPT;
        case LANG_DUTCH: return LangNL;
        case LANG_POLISH: return LangPL;
        case LANG_RUSSIAN: return LangRU;
        case LANG_CHINESE:
            if (SUBLANGID(ui) == SUBLANG_CHINESE_SIMPLIFIED ||
                SUBLANGID(ui) == SUBLANG_CHINESE_SINGAPORE) return LangZH;
            return LangEN; // Traditional -> genuine English, not Simplified
        case LANG_JAPANESE: return LangJA;
        case LANG_KOREAN: return LangKO;
        case LANG_TURKISH: return LangTR;
        case LANG_CZECH: return LangCS;
        case LANG_HUNGARIAN: return LangHU;
        case LANG_ROMANIAN: return LangRO;
        case LANG_SWEDISH: return LangSV;
        case LANG_UKRAINIAN: return LangUK;
        case LANG_GREEK: return LangEL;
        case LANG_ARABIC: return LangAR;
        default: return LangEN;
    }
}
int TagLanguage(const std::wstring& value) {
    for (int i = 0; i < LangCount; ++i) {
        std::wstring tag = kLangTags[i];
        for (auto& c : tag) c = Fold(c);
        if (value == tag) return i;
        size_t dash = tag.find(L'-');
        if (dash != std::wstring::npos && value == tag.substr(0, dash)) return i;
    }
    return -1;
}
// Settings override ("auto" default) or explicit tag; unknown -> auto.
int ResolveSelectedLanguage() {
    auto raw = WindhawkUtils::StringSetting::make(L"language");
    PCWSTR r = raw.get();
    std::wstring value = (r && *r) ? r : L"auto";
    for (auto& c : value) c = Fold(c);
    if (value.empty() || value == L"auto" || value == L"system") {
        int autoLang = AutoLanguage();
        Wh_Log(L"UI language: auto -> %s (%s)", kLangTags[autoLang], kLangNames[autoLang]);
        return autoLang;
    }
    int forced = TagLanguage(value);
    if (forced < 0) {
        Wh_Log(L"Unknown language setting '%s'; using Automatic", value.c_str());
        forced = AutoLanguage();
    }
    Wh_Log(L"UI language: %s (%s)", kLangTags[forced], kLangNames[forced]);
    return forced;
}
// ================= ITALIANO (it-IT) =================
static const wchar_t* const kStrTr_IT[66] = {
    L"Paese e lingua",
    L"Personalizza le impostazioni per la visualizzazione di lingue, numeri, ore e date.",
    L"Personalizza formato",
    L"Una o pi\u00F9 impostazioni internazionali non sono valide. Per risolvere il problema, controlla e correggi le impostazioni personalizzabili.",
    nullptr, // .7 (example, keep)
    nullptr, // 0.7 (example, keep)
    L"Metrico",
    nullptr, // U.S. (abbreviation, keep)
    L"Uno o pi\u00F9 caratteri immessi in questo campo non sono validi. Prova a utilizzare caratteri diversi.",
    L"Uno o pi\u00F9 caratteri immessi per %s non sono validi. Prova a utilizzare un carattere diverso o immetti uno spazio.",
    L"Simbolo decimale",
    L"Segno negativo",
    L"Simbolo di raggruppamento",
    L"Simbolo AM",
    L"Simbolo PM",
    L"Simbolo di valuta",
    L"Simbolo decimale valuta",
    L"Simbolo raggruppamento valuta",
    L"Uno o pi\u00F9 caratteri immessi per il formato %s non sono validi. Prova a utilizzare caratteri diversi.",
    L"Ora estesa",
    L"Data breve",
    L"Data estesa",
    L"Il valore in questo campo deve essere un numero compreso tra 99 e 9999. Prova a utilizzare un numero diverso.",
    L"Ora breve",
    L"&Formato:",
    L"&Formato: (* impostazioni locali personalizzate)",
    nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, // H h M m s t d y (format codes, keep)
    L"Le impostazioni locali del sistema sono state modificate. \u00C8 necessario riavviare Windows per applicare le modifiche.",
    L"Cambia opzioni internazionali",
    L"Impossibile installare le impostazioni locali selezionate. Contattare l'amministratore di sistema.",
    L"La lingua di visualizzazione del sistema \u00E8 stata modificata. \u00C8 necessario riavviare Windows per applicare le modifiche.",
    L"Rimuovere tutte le personalizzazioni per il formato corrente?",
    L"Applicare le modifiche a paese e lingua?",
    L"Riavvia ora",
    L"Annulla",
    L"Salvare il lavoro e chiudere tutti i programmi aperti prima di riavviare.",
    L"Cambia locale di sistema",
    L"Impossibile caricare correttamente il layout di tastiera %s.",
    L"Spagnolo (Spagna)",
    L"\u00C8 necessario disconnettersi per applicare le modifiche alla lingua di visualizzazione",
    L"Salvare il lavoro e chiudere tutti i programmi aperti prima di disconnettersi.",
    L"Disconnetti ora",
    L"Annulla",
    L"Cambia lingua di visualizzazione",
    L"Per consentire al computer di applicare queste modifiche \u00E8 consigliabile applicarle prima di apportare altre modifiche al sistema.",
    L"Applica",
    L"Annulla",
    L"Impossibile completare l'operazione",
    L"Utente corrente",
    L"Schermata di benvenuto",
    L"Nuovi account utente",
    L"Lingua di visualizzazione:",
    L"Lingua di input:",
    L"Formato:",
    L"Località:",
    L"Impossibile leggere l'impostazione",
    L"Contesto",
    L"Mai",
    L"Nazionale",
};
static const wchar_t* const kDlgTr_IT[92] = {
    L"Formati di data e ora",
    L"&Data breve:",
    L"Data &estesa:",
    L"&Ora breve:",
    L"Or&a estesa:",
    L"Primo giorno della &settimana:",
    L"Esempi",
    L"Data breve:",
    L"Data estesa:",
    L"Ora breve:",
    L"Ora estesa:",
    L"&Altre impostazioni",
    L"Tastiere e altre lingue di input",
    L"Per cambiare la tastiera o la lingua di input, fai clic su Cambia tastiere.",
    L"&Cambia tastiere...",
    L"Lingua di visualizzazione",
    L"Installa o disinstalla le lingue che Windows pu\u00F2 utilizzare per visualizzare il testo e, dove supportato, riconoscere voce e grafia.",
    L"&Installa/disinstalla lingue...",
    L"Come utente ospite non \u00E8 possibile cambiare la lingua di visualizzazione:",
    L"La selezione della lingua di visualizzazione \u00E8 bloccata dai Criteri di gruppo.",
    L"Scegliere una lingua di &visualizzazione:",
    L"&Alcuni testi non sono tradotti nella lingua selezionata. Selezionare un'altra lingua che Windows utilizzer\u00E0 per visualizzare il testo:",
    L"Questa lingua \u00E8 solo parzialmente tradotta e alcuni testi potrebbero essere visualizzati in:",
    L"&Anche questa lingua \u00E8 solo parzialmente tradotta. Selezionare una terza lingua che Windows utilizzer\u00E0 per visualizzare il testo rimanente:",
    L"Anche questa lingua \u00E8 solo parzialmente tradotta e alcuni testi potrebbero essere visualizzati in: ",
    L"Schermata di benvenuto e nuovi account utente",
    L"Visualizza e copia le impostazioni internazionali nella schermata di benvenuto, negli account di sistema e nei nuovi account utente.",
    L"&Copia impostazioni...",
    L"Lingua per i programmi non Unicode",
    L"Questa impostazione (impostazioni locali di sistema) controlla la lingua utilizzata per visualizzare il testo nei programmi che non supportano Unicode.",
    L"Lingua corrente per i programmi non Unicode:",
    nullptr, // <systemLocale> (placeholder, keep)
    L"&Cambia locale di sistema...",
    L"Esempio",
    L"Positivo:",
    L"Negativo:",
    L"Simbolo &decimale:",
    L"Numero di cifre de&cimali:",
    L"Simbolo di rag&gruppamento cifre:",
    L"Raggrupp&amento cifre:",
    L"Simbolo segno &negativo:",
    L"Formato numeri negati&vi:",
    L"Visualizza &zeri iniziali:",
    L"Separatore di e&lenco:",
    L"&Sistema di misura:",
    L"Cifre s&tandard:",
    L"&Usa cifre native:",
    L"Fare clic su Reimposta per ripristinare le impostazioni predefinite di sistema per numeri, valuta, ora e data.",
    L"&Reimposta",
    L"&Simbolo di valuta:",
    L"Formato &valuta positivo:",
    L"Formato valuta ne&gativo:",
    L"N&umero di cifre decimali:",
    L"Simbolo raggruppamento &cifre:",
    L"Raggruppament&o cifre:",
    L"Formati ora",
    L"&Ora breve:",
    L"Ora &estesa:",
    L"Simbolo &AM:",
    L"Simbolo &PM:",
    L"Significato delle notazioni:\n\nh = ora   m = minuti\ns = secondi (solo ora estesa)\ntt = A.M. o P.M.\n\nh/H = 12/24 ore\n\nhh, mm, ss = mostra zero iniziale\nh, m, s = non mostra zero iniziale",
    L"Formati data",
    L"Significato delle notazioni:\nd, dd = giorno;  ddd, dddd = giorno della settimana;  M = mese;  y = anno",
    L"Calendario",
    L"Se viene immesso un anno a due cifre, &interpretalo come anno compreso tra:",
    L"e",
    L"Primo &giorno della settimana:",
    L"&Tipo di calendario:",
    L"Regola data &Hijri su:",
    L"\u00C8 possibile controllare il modo in cui alcuni programmi ordinano caratteri, parole, file e cartelle.",
    L"&Selezionare il metodo di ordinamento:",
    L"Alcuni software, incluso Windows, potrebbero offrire contenuti aggiuntivi per una posizione particolare. Alcuni servizi forniscono informazioni locali come notizie e meteo.",
    L"&Posizione corrente:",
    L"Vedere anche",
    L"Le &impostazioni per l'utente corrente, la schermata di benvenuto (account di sistema) e i nuovi account utente sono mostrate di seguito.",
    L"* Impostazioni locali personalizzate",
    L"Copia le impostazioni correnti in:",
    L"&Schermata di benvenuto e account di sistema",
    L"&Nuovi account utente",
    L"La lingua di visualizzazione dei nuovi account utente \u00E8 attualmente ereditata dalla lingua di visualizzazione della schermata di benvenuto.",
    nullptr, // OK (universal, keep)
    L"Annulla",
    L"Selezionare la lingua (impostazioni locali di sistema) da utilizzare per visualizzare il testo nei programmi che non supportano Unicode. Questa impostazione riguarda tutti gli account utente del computer.",
    L"&Impostazioni locali di sistema correnti:",
    L"<A>Cambia metodo di ordinamento</A>",
    L"<A>Che cosa significa la notazione?</A>",
    L"<A>Informazioni online sulla modifica di lingue e formati internazionali</A>",
    L"<A>Come modificare il layout di tastiera per la schermata di benvenuto?</A>",
    L"<A>Come installare altre lingue?</A>",
    L"<A>Ulteriori informazioni su questi account</A>",
    L"<A>Che cosa sono le impostazioni locali di sistema?</A>",
    L"<A>Posizione predefinita</A>",
};
static const wchar_t* const kTitleTr_IT[11] = {
    L"Formati",
    L"Tastiere e lingue",
    L"Opzioni di amministrazione",
    L"Numeri",
    L"Valuta",
    L"Ora",
    L"Data",
    L"Ordinamento",
    L"Località",
    L"Impostazioni schermata di benvenuto e nuovi account utente",
    L"Impostazioni paese e lingua",
};
// ================= DEUTSCH (de-DE) =================
static const wchar_t* const kStrTr_DE[66] = {
    L"Region und Sprache",
    L"Passen Sie die Einstellungen f\u00FCr die Anzeige von Sprachen, Zahlen, Uhrzeiten und Datum an.",
    L"Format anpassen",
    L"Mindestens eine Ihrer regionalen Einstellungen ist ung\u00FCltig. \u00DCberpr\u00FCfen und korrigieren Sie die anpassbaren Einstellungen, um das Problem zu beheben.",
    nullptr,
    nullptr,
    L"Metrisch",
    nullptr,
    L"Mindestens eines der in dieses Feld eingegebenen Zeichen ist ung\u00FCltig. Versuchen Sie es mit anderen Zeichen.",
    L"Mindestens eines der f\u00FCr %s eingegebenen Zeichen ist ung\u00FCltig. Verwenden Sie ein anderes Zeichen oder geben Sie ein Leerzeichen ein.",
    L"Dezimaltrennzeichen",
    L"Negatives Vorzeichen",
    L"Gruppentrennzeichen",
    L"AM-Symbol",
    L"PM-Symbol",
    L"W\u00E4hrungssymbol",
    L"Dezimaltrennzeichen (W\u00E4hrung)",
    L"Gruppentrennzeichen (W\u00E4hrung)",
    L"Mindestens eines der f\u00FCr das Format %s eingegebenen Zeichen ist ung\u00FCltig. Versuchen Sie es mit anderen Zeichen.",
    L"Lange Uhrzeit",
    L"Kurzes Datum",
    L"Langes Datum",
    L"Der Wert in diesem Feld muss eine Zahl zwischen 99 und 9999 sein. Versuchen Sie es mit einer anderen Zahl.",
    L"Kurze Uhrzeit",
    L"&Format:",
    L"&Format: (* benutzerdefiniertes Gebietsschema)",
    nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
    L"Das Systemgebietsschema wurde ge\u00E4ndert. Sie m\u00FCssen Windows neu starten, damit die \u00C4nderungen wirksam werden.",
    L"Regionale Optionen \u00E4ndern",
    L"Das ausgew\u00E4hlte Gebietsschema konnte nicht installiert werden. Wenden Sie sich an den Systemadministrator.",
    L"Die Anzeigesprache des Systems wurde ge\u00E4ndert. Sie m\u00FCssen Windows neu starten, damit die \u00C4nderungen wirksam werden.",
    L"Alle Anpassungen f\u00FCr das aktuelle Format entfernen?",
    L"M\u00F6chten Sie die \u00C4nderungen an Region und Sprache \u00FCbernehmen?",
    L"Jetzt neu starten",
    L"Abbrechen",
    L"Speichern Sie Ihre Arbeit, und schlie\u00DFen Sie alle ge\u00F6ffneten Programme, bevor Sie neu starten.",
    L"Systemgebietsschema",
    L"Das Tastaturlayout %s konnte von Windows nicht ordnungsgem\u00E4\u00DF geladen werden.",
    L"Spanisch (Spanien)",
    L"Sie m\u00FCssen sich abmelden, damit die \u00C4nderungen der Anzeigesprache wirksam werden",
    L"Speichern Sie Ihre Arbeit, und schlie\u00DFen Sie alle ge\u00F6ffneten Programme, bevor Sie sich abmelden.",
    L"Jetzt abmelden",
    L"Abbrechen",
    L"Anzeigesprache \u00E4ndern",
    L"Es wird empfohlen, diese \u00C4nderungen zu \u00FCbernehmen, bevor Sie weitere System\u00E4nderungen vornehmen, damit der Computer diese \u00C4nderungen widerspiegelt.",
    L"\u00DCbernehmen",
    L"Abbrechen",
    L"Der Vorgang kann nicht abgeschlossen werden",
    L"Aktueller Benutzer",
    L"Willkommensseite",
    L"Neue Benutzerkonten",
    L"Anzeigesprache:",
    L"Eingabesprache:",
    L"Format:",
    L"Standort:",
    L"Einstellung konnte nicht gelesen werden",
    L"Kontext",
    L"Nie",
    L"National",
};
static const wchar_t* const kDlgTr_DE[92] = {
    L"Datums- und Zeitformate",
    L"&Kurzes Datum:",
    L"&Langes Datum:",
    L"K&urze Uhrzeit:",
    L"Lan&ge Uhrzeit:",
    L"Erster Wochen&tag:",
    L"Beispiele",
    L"Kurzes Datum:",
    L"Langes Datum:",
    L"Kurze Uhrzeit:",
    L"Lange Uhrzeit:",
    L"&Weitere Einstellungen...",
    L"Tastaturen und andere Eingabesprachen",
    L"Klicken Sie auf \u201ETastaturen \u00E4ndern\u201C, um die Tastatur oder Eingabesprache zu \u00E4ndern.",
    L"Tastaturen &\u00E4ndern...",
    L"Anzeigesprache",
    L"Installieren oder deinstallieren Sie Sprachen, die Windows zum Anzeigen von Text und, sofern unterst\u00FCtzt, zum Erkennen von Sprache und Handschrift verwenden kann.",
    L"&Sprachen installieren/deinstallieren...",
    L"Als Gastbenutzer k\u00F6nnen Sie die Anzeigesprache nicht \u00E4ndern:",
    L"Die Auswahl der Anzeigesprache wird durch Gruppenrichtlinien blockiert.",
    L"Anzeigesprache &w\u00E4hlen:",
    L"&Mancher Text ist nicht in die ausgew\u00E4hlte Sprache \u00FCbersetzt. W\u00E4hlen Sie eine andere Sprache aus, die Windows zum Anzeigen dieses Texts verwenden soll:",
    L"Diese Sprache ist nur teilweise \u00FCbersetzt. Mancher Text wird m\u00F6glicherweise angezeigt in:",
    L"&Auch diese Sprache ist nur teilweise \u00FCbersetzt. W\u00E4hlen Sie eine dritte Sprache aus, die Windows zum Anzeigen des \u00FCbrigen Texts verwenden soll:",
    L"Auch diese Sprache ist nur teilweise \u00FCbersetzt. Mancher Text wird m\u00F6glicherweise angezeigt in: ",
    L"Willkommensseite und neue Benutzerkonten",
    L"Zeigen Sie Ihre internationalen Einstellungen an, und kopieren Sie sie auf die Willkommensseite, Systemkonten und neue Benutzerkonten.",
    L"Einstellungen &kopieren...",
    L"Sprache f\u00FCr Unicode-inkompatible Programme",
    L"Diese Einstellung (Systemgebietsschema) steuert die Sprache, die beim Anzeigen von Text in Programmen verwendet wird, die Unicode nicht unterst\u00FCtzen.",
    L"Aktuelle Sprache f\u00FCr Unicode-inkompatible Programme:",
    nullptr,
    L"&Systemgebietsschema...",
    L"Beispiel",
    L"Positiv:",
    L"Negativ:",
    L"&Dezimaltrennzeichen:",
    L"Anzahl Dezimalste&llen:",
    L"Gruppentrennzeichen f\u00FCr Zi&ffern:",
    L"&Zifferngruppierung:",
    L"Symbol f\u00FCr negati&ves Vorzeichen:",
    L"Format f\u00FCr ne&gative Zahlen:",
    L"F\u00FChrende &Nullen anzeigen:",
    L"&Listentrennzeichen:",
    L"&Ma\u00DFsystem:",
    L"&Standardziffern:",
    L"Nat&ive Ziffern verwenden:",
    L"Klicken Sie auf \u201EZur\u00FCcksetzen\u201C, um die Standardsystemeinstellungen f\u00FCr Zahlen, W\u00E4hrung, Uhrzeit und Datum wiederherzustellen.",
    L"&Zur\u00FCcksetzen",
    L"&W\u00E4hrungssymbol:",
    L"&Positives W\u00E4hrungsformat:",
    L"&Negatives W\u00E4hrungsformat:",
    L"Anzahl Dezimalstel&len:",
    L"Gruppentrennzeichen f\u00FCr Z&iffern:",
    L"Zifferngruppierun&g:",
    L"Uhrzeitformate",
    L"&Kurze Uhrzeit:",
    L"&Lange Uhrzeit:",
    L"&AM-Symbol:",
    L"&PM-Symbol:",
    L"Bedeutung der Notationen:\n\nh = Stunde   m = Minute\ns = Sekunde (nur lange Uhrzeit)\ntt = AM oder PM\n\nh/H = 12/24 Stunden\n\nhh, mm, ss = f\u00FChrende Null anzeigen\nh, m, s = keine f\u00FChrende Null anzeigen",
    L"Datumsformate",
    L"Bedeutung der Notationen:\nd, dd = Tag;  ddd, dddd = Wochentag;  M = Monat;  y = Jahr",
    L"Kalender",
    L"&Wenn eine zweistellige Jahreszahl eingegeben wird, als Jahr zwischen ... interpretieren:",
    L"und",
    L"Erster Wochenta&g:",
    L"K&alendertyp:",
    L"Hidschri-Datum an&passen auf:",
    L"Sie k\u00F6nnen steuern, wie einige Programme Zeichen, W\u00F6rter, Dateien und Ordner sortieren.",
    L"&Sortiermethode ausw\u00E4hlen:",
    L"Manche Software, einschlie\u00DFlich Windows, stellt Ihnen m\u00F6glicherweise zus\u00E4tzliche Inhalte f\u00FCr einen bestimmten Standort bereit. Manche Dienste stellen lokale Informationen wie Nachrichten und Wetter bereit.",
    L"&Aktueller Standort:",
    L"Siehe auch",
    L"Die &Einstellungen f\u00FCr den aktuellen Benutzer, die Willkommensseite (Systemkonten) und neue Benutzerkonten werden unten angezeigt.",
    L"* Benutzerdefiniertes Gebietsschema",
    L"Aktuelle Einstellungen kopieren nach:",
    L"&Willkommensseite und Systemkonten",
    L"&Neue Benutzerkonten",
    L"Die Anzeigesprache f\u00FCr neue Benutzerkonten wird derzeit von der Anzeigesprache der Willkommensseite \u00FCbernommen.",
    nullptr,
    L"Abbrechen",
    L"W\u00E4hlen Sie aus, welche Sprache (Systemgebietsschema) beim Anzeigen von Text in Programmen verwendet werden soll, die Unicode nicht unterst\u00FCtzen. Diese Einstellung betrifft alle Benutzerkonten auf dem Computer.",
    L"&Aktuelles Systemgebietsschema:",
    L"<A>Sortiermethode \u00E4ndern</A>",
    L"<A>Was bedeutet die Notation?</A>",
    L"<A>Onlineinformationen zum \u00C4ndern von Sprachen und regionalen Formaten</A>",
    L"<A>Wie \u00E4ndere ich das Tastaturlayout f\u00FCr die Willkommensseite?</A>",
    L"<A>Wie kann ich zus\u00E4tzliche Sprachen installieren?</A>",
    L"<A>Weitere Informationen zu diesen Konten</A>",
    L"<A>Was ist das Systemgebietsschema?</A>",
    L"<A>Standardstandort</A>",
};
static const wchar_t* const kTitleTr_DE[11] = {
    L"Formate",
    L"Tastaturen und Sprachen",
    L"Verwaltung",
    L"Zahlen",
    L"W\u00E4hrung",
    L"Uhrzeit",
    L"Datum",
    L"Sortierung",
    L"Standort",
    L"Einstellungen f\u00FCr Willkommensseite und neue Benutzerkonten",
    L"Regions- und Spracheinstellungen",
};
// ================= FRANCAIS (fr-FR) =================
static const wchar_t* const kStrTr_FR[66] = {
    L"R\u00E9gion et langue",
    L"Personnalisez les param\u00E8tres d'affichage des langues, des nombres, des heures et des dates.",
    L"Personnaliser le format",
    L"Un ou plusieurs de vos param\u00E8tres r\u00E9gionaux ne sont pas valides. Pour r\u00E9soudre ce probl\u00E8me, v\u00E9rifiez et corrigez les param\u00E8tres personnalisables.",
    nullptr,
    nullptr,
    L"M\u00E9trique",
    nullptr,
    L"Un ou plusieurs des caract\u00E8res entr\u00E9s dans ce champ ne sont pas valides. Essayez d'utiliser des caract\u00E8res diff\u00E9rents.",
    L"Un ou plusieurs des caract\u00E8res entr\u00E9s pour %s ne sont pas valides. Essayez d'utiliser un autre caract\u00E8re ou entrez un espace.",
    L"Symbole d\u00E9cimal",
    L"Signe n\u00E9gatif",
    L"Symbole de groupement",
    L"Symbole AM",
    L"Symbole PM",
    L"Symbole mon\u00E9taire",
    L"Symbole d\u00E9cimal mon\u00E9taire",
    L"Symbole de groupement mon\u00E9taire",
    L"Un ou plusieurs des caract\u00E8res entr\u00E9s pour le format %s ne sont pas valides. Essayez d'utiliser des caract\u00E8res diff\u00E9rents.",
    L"Heure longue",
    L"Date courte",
    L"Date longue",
    L"La valeur de ce champ doit \u00EAtre un nombre compris entre 99 et 9999. Essayez d'utiliser un autre nombre.",
    L"Heure courte",
    L"&Format :",
    L"&Format : (* Param\u00E8tres r\u00E9gionaux personnalis\u00E9s)",
    nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
    L"Les param\u00E8tres r\u00E9gionaux du syst\u00E8me ont \u00E9t\u00E9 modifi\u00E9s. Vous devez red\u00E9marrer Windows pour que les modifications prennent effet.",
    L"Modifier les options r\u00E9gionales",
    L"Le programme d'installation n'a pas pu installer les param\u00E8tres r\u00E9gionaux choisis. Contactez votre administrateur syst\u00E8me.",
    L"La langue d'affichage du syst\u00E8me a \u00E9t\u00E9 modifi\u00E9e. Vous devez red\u00E9marrer Windows pour que les modifications prennent effet.",
    L"Supprimer toutes les personnalisations du format actuel ?",
    L"Voulez-vous appliquer vos modifications de r\u00E9gion et de langue ?",
    L"Red\u00E9marrer maintenant",
    L"Annuler",
    L"Enregistrez votre travail et fermez tous les programmes ouverts avant de red\u00E9marrer.",
    L"Param\u00E8tres r\u00E9gionaux",
    L"Windows n'a pas pu charger correctement la disposition de clavier %s.",
    L"Espagnol (Espagne)",
    L"Vous devez fermer votre session pour que les modifications de langue d'affichage prennent effet",
    L"Enregistrez votre travail et fermez tous les programmes ouverts avant de fermer votre session.",
    L"Fermer la session maintenant",
    L"Annuler",
    L"Modifier la langue d'affichage",
    L"Pour que l'ordinateur refl\u00E8te ces modifications, il est recommand\u00E9 de les appliquer avant d'apporter d'autres modifications au syst\u00E8me.",
    L"Appliquer",
    L"Annuler",
    L"La t\u00E2che ne peut pas \u00EAtre achev\u00E9e",
    L"Utilisateur actuel",
    L"\u00C9cran d'accueil",
    L"Nouveaux comptes d'utilisateur",
    L"Langue d'affichage :",
    L"Langue d'entr\u00E9e :",
    L"Format :",
    L"Emplacement :",
    L"Impossible de lire le param\u00E8tre",
    L"Contexte",
    L"Jamais",
    L"National",
};
static const wchar_t* const kDlgTr_FR[92] = {
    L"Formats de date et d'heure",
    L"Date &courte :",
    L"Date &longue :",
    L"&Heure courte :",
    L"H&eure longue :",
    L"Premier &jour de la semaine :",
    L"Exemples",
    L"Date courte :",
    L"Date longue :",
    L"Heure courte :",
    L"Heure longue :",
    L"Param\u00E8tres &suppl\u00E9mentaires...",
    L"Claviers et autres langues d'entr\u00E9e",
    L"Pour changer votre clavier ou votre langue d'entr\u00E9e, cliquez sur Modifier les claviers.",
    L"&Modifier les claviers...",
    L"Langue d'affichage",
    L"Installez ou d\u00E9sinstallez les langues que Windows peut utiliser pour afficher du texte et, lorsque c'est pris en charge, pour reconna\u00EEtre la parole et l'\u00E9criture manuscrite.",
    L"&Installer/d\u00E9sinstaller des langues...",
    L"En tant qu'invit\u00E9, vous ne pouvez pas modifier la langue d'affichage :",
    L"La s\u00E9lection de la langue d'affichage est bloqu\u00E9e par la strat\u00E9gie de groupe.",
    L"&Choisir une langue d'affichage :",
    L"&Certains textes ne sont pas traduits dans la langue s\u00E9lectionn\u00E9e. S\u00E9lectionnez une autre langue que Windows utilisera pour afficher ce texte :",
    L"Cette langue n'est que partiellement traduite et certains textes peuvent s'afficher en :",
    L"&Cette langue n'est elle aussi que partiellement traduite. S\u00E9lectionnez une troisi\u00E8me langue que Windows utilisera pour afficher le texte restant :",
    L"Cette langue n'est elle aussi que partiellement traduite et certains textes peuvent s'afficher en : ",
    L"\u00C9cran d'accueil et nouveaux comptes d'utilisateur",
    L"Affichez et copiez vos param\u00E8tres internationaux vers l'\u00E9cran d'accueil, les comptes syst\u00E8me et les nouveaux comptes d'utilisateur.",
    L"&Copier les param\u00E8tres...",
    L"Langue pour les programmes non-Unicode",
    L"Ce param\u00E8tre (param\u00E8tres r\u00E9gionaux du syst\u00E8me) contr\u00F4le la langue utilis\u00E9e pour afficher du texte dans les programmes qui ne prennent pas en charge Unicode.",
    L"Langue actuelle pour les programmes non-Unicode :",
    nullptr,
    L"&Param\u00E8tres r\u00E9gionaux...",
    L"Exemple",
    L"Positif :",
    L"N\u00E9gatif :",
    L"Symbole &d\u00E9cimal :",
    L"No&mbre de d\u00E9cimales :",
    L"Symbole de &groupement des chiffres :",
    L"Groupement des &chiffres :",
    L"Symbole de signe &n\u00E9gatif :",
    L"Format des nombres n\u00E9ga&tifs :",
    L"Afficher les &z\u00E9ros de t\u00EAte :",
    L"S\u00E9parateur de &liste :",
    L"S&yst\u00E8me de mesure :",
    L"Chi&ffres standard :",
    L"Utiliser les chiffres &natifs :",
    L"Cliquez sur R\u00E9initialiser pour restaurer les param\u00E8tres syst\u00E8me par d\u00E9faut des nombres, de la monnaie, de l'heure et de la date.",
    L"&R\u00E9initialiser",
    L"Symbole &mon\u00E9taire :",
    L"Format mon\u00E9taire posi&tif :",
    L"Format mon\u00E9taire n\u00E9&gatif :",
    L"N&ombre de d\u00E9cimales :",
    L"Symbole de groupement des &chiffres :",
    L"Groupement des c&hiffres :",
    L"Formats d'heure",
    L"&Heure courte :",
    L"Heure &longue :",
    L"Symbole &AM :",
    L"Symbole &PM :",
    L"Signification des notations :\n\nh = heure   m = minute\ns = seconde (heure longue uniquement)\ntt = AM ou PM\n\nh/H = 12/24 heures\n\nhh, mm, ss = afficher le z\u00E9ro de t\u00EAte\nh, m, s = ne pas afficher le z\u00E9ro de t\u00EAte",
    L"Formats de date",
    L"Signification des notations :\nd, dd = jour ;  ddd, dddd = jour de la semaine ;  M = mois ;  y = ann\u00E9e",
    L"Calendrier",
    L"Lorsqu'une ann\u00E9e \u00E0 deux chiffres est entr\u00E9e, l'interpr\u00E9ter comme une ann\u00E9e comprise &entre :",
    L"et",
    L"Premier jo&ur de la semaine :",
    L"&Type de calendrier :",
    L"A&juster la date de l'H\u00E9gire sur :",
    L"Vous pouvez contr\u00F4ler la fa\u00E7on dont certains programmes trient les caract\u00E8res, les mots, les fichiers et les dossiers.",
    L"&S\u00E9lectionnez la m\u00E9thode de tri :",
    L"Certains logiciels, y compris Windows, peuvent vous fournir du contenu suppl\u00E9mentaire pour un emplacement particulier. Certains services fournissent des informations locales telles que les actualit\u00E9s et la m\u00E9t\u00E9o.",
    L"&Emplacement actuel :",
    L"Voir aussi",
    L"Les &param\u00E8tres de l'utilisateur actuel, de l'\u00E9cran d'accueil (comptes syst\u00E8me) et des nouveaux comptes d'utilisateur sont affich\u00E9s ci-dessous.",
    L"* Param\u00E8tres r\u00E9gionaux personnalis\u00E9s",
    L"Copier vos param\u00E8tres actuels vers :",
    L"\u00C9cran d'&accueil et comptes syst\u00E8me",
    L"&Nouveaux comptes d'utilisateur",
    L"La langue d'affichage des nouveaux comptes d'utilisateur est actuellement h\u00E9rit\u00E9e de la langue d'affichage de l'\u00E9cran d'accueil.",
    nullptr,
    L"Annuler",
    L"S\u00E9lectionnez la langue (param\u00E8tres r\u00E9gionaux du syst\u00E8me) \u00E0 utiliser pour afficher du texte dans les programmes qui ne prennent pas en charge Unicode. Ce param\u00E8tre affecte tous les comptes d'utilisateur de l'ordinateur.",
    L"Param\u00E8tres r\u00E9gionaux actuels du &syst\u00E8me :",
    L"<A>Modifier la m\u00E9thode de tri</A>",
    L"<A>Que signifie la notation ?</A>",
    L"<A>En savoir plus en ligne sur la modification des langues et des formats r\u00E9gionaux</A>",
    L"<A>Comment changer la disposition de clavier de l'\u00E9cran d'accueil ?</A>",
    L"<A>Comment installer des langues suppl\u00E9mentaires ?</A>",
    L"<A>En savoir plus sur ces comptes</A>",
    L"<A>Que sont les param\u00E8tres r\u00E9gionaux du syst\u00E8me ?</A>",
    L"<A>Emplacement par d\u00E9faut</A>",
};
static const wchar_t* const kTitleTr_FR[11] = {
    L"Formats",
    L"Claviers et langues",
    L"Administration",
    L"Nombres",
    L"Monnaie",
    L"Heure",
    L"Date",
    L"Tri",
    L"Emplacement",
    L"Param\u00E8tres de l'\u00E9cran d'accueil et des nouveaux comptes",
    L"Param\u00E8tres de r\u00E9gion et de langue",
};
// ================= ESPANOL (es-ES) =================
static const wchar_t* const kStrTr_ES[66] = {
    L"Regi\u00F3n e idioma",
    L"Personalice la configuraci\u00F3n de visualizaci\u00F3n de idiomas, n\u00FAmeros, horas y fechas.",
    L"Personalizar formato",
    L"Una o varias de sus configuraciones regionales no son v\u00E1lidas. Para solucionar este problema, revise y corrija la configuraci\u00F3n personalizable.",
    nullptr,
    nullptr,
    L"M\u00E9trico",
    nullptr,
    L"Uno o varios de los caracteres escritos en este campo no son v\u00E1lidos. Pruebe a usar otros caracteres.",
    L"Uno o varios de los caracteres escritos para %s no son v\u00E1lidos. Pruebe a usar otro car\u00E1cter o escriba un espacio en blanco.",
    L"S\u00EDmbolo decimal",
    L"Signo negativo",
    L"S\u00EDmbolo de agrupamiento",
    L"S\u00EDmbolo a. m.",
    L"S\u00EDmbolo p. m.",
    L"S\u00EDmbolo de moneda",
    L"S\u00EDmbolo decimal de moneda",
    L"S\u00EDmbolo de agrupamiento de moneda",
    L"Uno o varios de los caracteres escritos para el formato %s no son v\u00E1lidos. Pruebe a usar otros caracteres.",
    L"Hora larga",
    L"Fecha corta",
    L"Fecha larga",
    L"El valor de este campo debe ser un n\u00FAmero entre 99 y 9999. Pruebe a usar otro n\u00FAmero.",
    L"Hora corta",
    L"&Formato:",
    L"&Formato: (* configuraci\u00F3n regional personalizada)",
    nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
    L"La configuraci\u00F3n regional del sistema ha cambiado. Debe reiniciar Windows para que los cambios surtan efecto.",
    L"Cambiar opciones regionales",
    L"El programa de instalaci\u00F3n no pudo instalar la configuraci\u00F3n regional elegida. P\u00F3ngase en contacto con el administrador del sistema.",
    L"El idioma para mostrar del sistema ha cambiado. Debe reiniciar Windows para que los cambios surtan efecto.",
    L"\u00BFQuitar todas las personalizaciones del formato actual?",
    L"\u00BFDesea aplicar los cambios de regi\u00F3n e idioma?",
    L"Reiniciar ahora",
    L"Cancelar",
    L"Aseg\u00FArese de guardar su trabajo y cerrar todos los programas abiertos antes de reiniciar.",
    L"Configuraci\u00F3n regional",
    L"Windows no pudo cargar correctamente la distribuci\u00F3n del teclado %s.",
    L"Espa\u00F1ol (Espa\u00F1a)",
    L"Debe cerrar la sesi\u00F3n para que los cambios de idioma para mostrar surtan efecto",
    L"Aseg\u00FArese de guardar su trabajo y cerrar todos los programas abiertos antes de cerrar la sesi\u00F3n.",
    L"Cerrar sesi\u00F3n ahora",
    L"Cancelar",
    L"Cambiar idioma para mostrar",
    L"Para asegurarse de que el equipo refleje estos cambios, recomendamos aplicarlos antes de realizar m\u00E1s cambios en el sistema.",
    L"Aplicar",
    L"Cancelar",
    L"La tarea no se puede completar",
    L"Usuario actual",
    L"Pantalla de bienvenida",
    L"Cuentas de usuario nuevas",
    L"Idioma para mostrar:",
    L"Idioma de entrada:",
    L"Formato:",
    L"Ubicaci\u00F3n:",
    L"No se pudo leer la configuraci\u00F3n",
    L"Contexto",
    L"Nunca",
    L"Nacional",
};
static const wchar_t* const kDlgTr_ES[92] = {
    L"Formatos de fecha y hora",
    L"&Fecha corta:",
    L"Fecha &larga:",
    L"&Hora corta:",
    L"Ho&ra larga:",
    L"Primer &d\u00EDa de la semana:",
    L"Ejemplos",
    L"Fecha corta:",
    L"Fecha larga:",
    L"Hora corta:",
    L"Hora larga:",
    L"Configuraci\u00F3n &adicional...",
    L"Teclados y otros idiomas de entrada",
    L"Para cambiar el teclado o el idioma de entrada, haga clic en Cambiar teclados.",
    L"&Cambiar teclados...",
    L"Idioma para mostrar",
    L"Instale o desinstale los idiomas que Windows puede usar para mostrar texto y, donde se admita, reconocer voz y escritura manual.",
    L"&Instalar o desinstalar idiomas...",
    L"Como usuario invitado no puede cambiar el idioma para mostrar:",
    L"La selecci\u00F3n del idioma para mostrar est\u00E1 bloqueada por la directiva de grupo.",
    L"&Elegir un idioma para mostrar:",
    L"&Parte del texto no est\u00E1 traducido al idioma seleccionado. Seleccione otro idioma para que Windows lo use al mostrar este texto:",
    L"Este idioma solo est\u00E1 parcialmente traducido y es posible que vea parte del texto en:",
    L"&Este idioma tambi\u00E9n solo est\u00E1 parcialmente traducido. Seleccione un tercer idioma para que Windows lo use al mostrar el texto restante:",
    L"Este idioma tambi\u00E9n solo est\u00E1 parcialmente traducido y es posible que vea parte del texto en: ",
    L"Pantalla de bienvenida y cuentas de usuario nuevas",
    L"Vea y copie su configuraci\u00F3n internacional a la pantalla de bienvenida, las cuentas del sistema y las cuentas de usuario nuevas.",
    L"&Copiar configuraci\u00F3n...",
    L"Idioma para programas no Unicode",
    L"Esta configuraci\u00F3n (configuraci\u00F3n regional del sistema) controla el idioma usado al mostrar texto en programas que no admiten Unicode.",
    L"Idioma actual para programas no Unicode:",
    nullptr,
    L"&Configuraci\u00F3n regional...",
    L"Ejemplo",
    L"Positivo:",
    L"Negativo:",
    L"S\u00EDmbolo &decimal:",
    L"N\u00FAmero de de&cimales:",
    L"S\u00EDmbolo de agrupaci\u00F3n de d\u00ED&gitos:",
    L"&Agrupaci\u00F3n de d\u00EDgitos:",
    L"S\u00EDmbolo de signo &negativo:",
    L"Formato de n\u00FAmero negati&vo:",
    L"Mostrar ceros &iniciales:",
    L"Separador de &listas:",
    L"&Sistema de medida:",
    L"D\u00EDgitos est&\u00E1ndar:",
    L"&Usar d\u00EDgitos nativos:",
    L"Haga clic en Restablecer para restaurar la configuraci\u00F3n predeterminada del sistema para n\u00FAmeros, moneda, hora y fecha.",
    L"&Restablecer",
    L"S\u00EDmbolo de &moneda:",
    L"Formato de moneda &positivo:",
    L"Formato de moneda negati&vo:",
    L"N\u00FAmero de decimale&s:",
    L"S\u00EDmbolo de agrupaci\u00F3n de d\u00EDgit&os:",
    L"A&grupaci\u00F3n de d\u00EDgitos:",
    L"Formatos de hora",
    L"&Hora corta:",
    L"Hora lar&ga:",
    L"S\u00EDmbolo &a. m.:",
    L"S\u00EDmbolo &p. m.:",
    L"Significado de las notaciones:\n\nh = hora   m = minuto\ns = segundo (solo hora larga)\ntt = a. m. o p. m.\n\nh/H = 12/24 horas\n\nhh, mm, ss = mostrar cero inicial\nh, m, s = no mostrar cero inicial",
    L"Formatos de fecha",
    L"Significado de las notaciones:\nd, dd = d\u00EDa;  ddd, dddd = d\u00EDa de la semana;  M = mes;  y = a\u00F1o",
    L"Calendario",
    L"Cuando se escriba un a\u00F1o de dos d\u00EDgitos, interpretarlo como un a\u00F1o &entre:",
    L"y",
    L"Primer d\u00EDa de la &semana:",
    L"&Tipo de calendario:",
    L"Ajustar fecha de la H\u00E9&gira a:",
    L"Puede controlar el modo en que algunos programas ordenan caracteres, palabras, archivos y carpetas.",
    L"&Seleccione el m\u00E9todo de ordenaci\u00F3n:",
    L"Algunos programas, incluido Windows, pueden proporcionarle contenido adicional para una ubicaci\u00F3n concreta. Algunos servicios proporcionan informaci\u00F3n local, como noticias y el tiempo.",
    L"&Ubicaci\u00F3n actual:",
    L"Vea tambi\u00E9n",
    L"A continuaci\u00F3n se muestra la &configuraci\u00F3n del usuario actual, la pantalla de bienvenida (cuentas del sistema) y las cuentas de usuario nuevas.",
    L"* Configuraci\u00F3n regional personalizada",
    L"Copiar la configuraci\u00F3n actual a:",
    L"&Pantalla de bienvenida y cuentas del sistema",
    L"Cuentas de usuario &nuevas",
    L"El idioma para mostrar de las cuentas de usuario nuevas se hereda actualmente del idioma para mostrar de la pantalla de bienvenida.",
    nullptr,
    L"Cancelar",
    L"Seleccione el idioma (configuraci\u00F3n regional del sistema) que se usar\u00E1 al mostrar texto en programas que no admiten Unicode. Esta configuraci\u00F3n afecta a todas las cuentas de usuario del equipo.",
    L"Configuraci\u00F3n regional actual del &sistema:",
    L"<A>Cambiar m\u00E9todo de ordenaci\u00F3n</A>",
    L"<A>\u00BFQu\u00E9 significa la notaci\u00F3n?</A>",
    L"<A>M\u00E1s informaci\u00F3n en l\u00EDnea sobre c\u00F3mo cambiar idiomas y formatos regionales</A>",
    L"<A>\u00BFC\u00F3mo cambio la distribuci\u00F3n del teclado para la pantalla de bienvenida?</A>",
    L"<A>\u00BFC\u00F3mo puedo instalar otros idiomas?</A>",
    L"<A>M\u00E1s informaci\u00F3n sobre estas cuentas</A>",
    L"<A>\u00BFQu\u00E9 es la configuraci\u00F3n regional del sistema?</A>",
    L"<A>Ubicaci\u00F3n predeterminada</A>",
};
static const wchar_t* const kTitleTr_ES[11] = {
    L"Formatos",
    L"Teclados e idiomas",
    L"Administrativo",
    L"N\u00FAmeros",
    L"Moneda",
    L"Hora",
    L"Fecha",
    L"Ordenaci\u00F3n",
    L"Ubicaci\u00F3n",
    L"Configuraci\u00F3n de pantalla de bienvenida y cuentas nuevas",
    L"Configuraci\u00F3n regional y de idioma",
};
// ================= PORTUGUES-BR (pt-BR) =================
static const wchar_t* const kStrTr_PT[66] = {
    L"Regi\u00E3o e idioma",
    L"Personalizar configura\u00E7\u00F5es de exibi\u00E7\u00E3o de idiomas, n\u00FAmeros, horas e datas.",
    L"Personalizar formato",
    L"Uma ou mais de suas configura\u00E7\u00F5es regionais s\u00E3o inv\u00E1lidas. Para corrigir esse problema, revise e corrija as configura\u00E7\u00F5es personaliz\u00E1veis.",
    nullptr,
    nullptr,
    L"M\u00E9trico",
    nullptr,
    L"Um ou mais dos caracteres digitados neste campo s\u00E3o inv\u00E1lidos. Tente usar caracteres diferentes.",
    L"Um ou mais dos caracteres digitados para %s s\u00E3o inv\u00E1lidos. Tente usar um caractere diferente ou digite um espa\u00E7o em branco.",
    L"S\u00EDmbolo decimal",
    L"Sinal de negativo",
    L"S\u00EDmbolo de agrupamento",
    L"S\u00EDmbolo AM",
    L"S\u00EDmbolo PM",
    L"S\u00EDmbolo de moeda",
    L"S\u00EDmbolo decimal de moeda",
    L"S\u00EDmbolo de agrupamento de moeda",
    L"Um ou mais dos caracteres digitados para o formato %s s\u00E3o inv\u00E1lidos. Tente usar caracteres diferentes.",
    L"Hora longa",
    L"Data curta",
    L"Data longa",
    L"O valor deste campo deve ser um n\u00FAmero entre 99 e 9999. Tente usar um n\u00FAmero diferente.",
    L"Hora curta",
    L"&Formato:",
    L"&Formato: (* Localidade personalizada)",
    nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
    L"As configura\u00E7\u00F5es de localidade do sistema foram alteradas. \u00C9 necess\u00E1rio reiniciar o Windows para que as altera\u00E7\u00F5es entrem em vigor.",
    L"Alterar op\u00E7\u00F5es regionais",
    L"A instala\u00E7\u00E3o n\u00E3o p\u00F4de instalar a localidade escolhida. Entre em contato com o administrador do sistema.",
    L"O idioma de exibi\u00E7\u00E3o do sistema foi alterado. \u00C9 necess\u00E1rio reiniciar o Windows para que as altera\u00E7\u00F5es entrem em vigor.",
    L"Remover todas as personaliza\u00E7\u00F5es do formato atual?",
    L"Deseja aplicar as altera\u00E7\u00F5es de regi\u00E3o e idioma?",
    L"Reiniciar agora",
    L"Cancelar",
    L"Salve seu trabalho e feche todos os programas abertos antes de reiniciar.",
    L"Localidade do sistema",
    L"O Windows n\u00E3o p\u00F4de carregar corretamente o layout de teclado %s.",
    L"Espanhol (Espanha)",
    L"\u00C9 necess\u00E1rio fazer logoff para que as altera\u00E7\u00F5es de idioma de exibi\u00E7\u00E3o entrem em vigor",
    L"Salve seu trabalho e feche todos os programas abertos antes de fazer logoff.",
    L"Fazer logoff agora",
    L"Cancelar",
    L"Alterar idioma de exibi\u00E7\u00E3o",
    L"Para garantir que o computador reflita essas altera\u00E7\u00F5es, recomendamos aplic\u00E1-las antes de fazer outras altera\u00E7\u00F5es no sistema.",
    L"Aplicar",
    L"Cancelar",
    L"A tarefa n\u00E3o pode ser conclu\u00EDda",
    L"Usu\u00E1rio atual",
    L"Tela de boas-vindas",
    L"Novas contas de usu\u00E1rio",
    L"Idioma de exibi\u00E7\u00E3o:",
    L"Idioma de entrada:",
    L"Formato:",
    L"Local:",
    L"N\u00E3o foi poss\u00EDvel ler a configura\u00E7\u00E3o",
    L"Contexto",
    L"Nunca",
    L"Nacional",
};
static const wchar_t* const kDlgTr_PT[92] = {
    L"Formatos de data e hora",
    L"&Data curta:",
    L"Data &longa:",
    L"&Hora curta:",
    L"Ho&ra longa:",
    L"Primeiro dia da se&mana:",
    L"Exemplos",
    L"Data curta:",
    L"Data longa:",
    L"Hora curta:",
    L"Hora longa:",
    L"Configura\u00E7\u00F5es &adicionais...",
    L"Teclados e outros idiomas de entrada",
    L"Para alterar o teclado ou o idioma de entrada, clique em Alterar teclados.",
    L"&Alterar teclados...",
    L"Idioma de exibi\u00E7\u00E3o",
    L"Instale ou desinstale idiomas que o Windows pode usar para exibir texto e, onde houver suporte, reconhecer fala e manuscrito.",
    L"&Instalar/desinstalar idiomas...",
    L"Como usu\u00E1rio convidado, voc\u00EA n\u00E3o pode alterar o idioma de exibi\u00E7\u00E3o:",
    L"A sele\u00E7\u00E3o do idioma de exibi\u00E7\u00E3o est\u00E1 bloqueada pela pol\u00EDtica de grupo.",
    L"&Escolher um idioma de exibi\u00E7\u00E3o:",
    L"&Parte do texto n\u00E3o est\u00E1 traduzida para o idioma selecionado. Selecione outro idioma para o Windows usar ao exibir este texto:",
    L"Este idioma est\u00E1 traduzido apenas parcialmente e talvez voc\u00EA veja parte do texto exibida em:",
    L"&Este idioma tamb\u00E9m est\u00E1 traduzido apenas parcialmente. Selecione um terceiro idioma para o Windows usar ao exibir o texto restante:",
    L"Este idioma tamb\u00E9m est\u00E1 traduzido apenas parcialmente e talvez voc\u00EA veja parte do texto exibida em: ",
    L"Tela de boas-vindas e novas contas de usu\u00E1rio",
    L"Exiba e copie suas configura\u00E7\u00F5es internacionais para a tela de boas-vindas, as contas do sistema e as novas contas de usu\u00E1rio.",
    L"&Copiar configura\u00E7\u00F5es...",
    L"Idioma para programas n\u00E3o Unicode",
    L"Esta configura\u00E7\u00E3o (localidade do sistema) controla o idioma usado ao exibir texto em programas que n\u00E3o d\u00E3o suporte a Unicode.",
    L"Idioma atual para programas n\u00E3o Unicode:",
    nullptr,
    L"&Localidade do sistema...",
    L"Exemplo",
    L"Positivo:",
    L"Negativo:",
    L"S\u00EDmbolo &decimal:",
    L"N\u00FAmero de &casas decimais:",
    L"S\u00EDmbolo de agrupamento de d\u00ED&gitos:",
    L"&Agrupamento de d\u00EDgitos:",
    L"S\u00EDmbolo de sinal &negativo:",
    L"Formato de n\u00FAmero negati&vo:",
    L"Exibir zeros \u00E0 &esquerda:",
    L"Separador de &listas:",
    L"&Sistema de medida:",
    L"D\u00EDgitos &padr\u00E3o:",
    L"&Usar d\u00EDgitos nativos:",
    L"Clique em Redefinir para restaurar as configura\u00E7\u00F5es padr\u00E3o do sistema para n\u00FAmeros, moeda, hora e data.",
    L"&Redefinir",
    L"S\u00EDmbolo de &moeda:",
    L"Formato de moeda &positivo:",
    L"Formato de moeda negati&vo:",
    L"N\u00FAmero de casa&s decimais:",
    L"S\u00EDmbolo de agrupamento de d\u00EDgit&os:",
    L"A&grupamento de d\u00EDgitos:",
    L"Formatos de hora",
    L"&Hora curta:",
    L"Hora lo&nga:",
    L"S\u00EDmbolo &AM:",
    L"S\u00EDmbolo &PM:",
    L"Significado das nota\u00E7\u00F5es:\n\nh = hora   m = minuto\ns = segundo (somente hora longa)\ntt = AM ou PM\n\nh/H = 12/24 horas\n\nhh, mm, ss = exibir zero \u00E0 esquerda\nh, m, s = n\u00E3o exibir zero \u00E0 esquerda",
    L"Formatos de data",
    L"Significado das nota\u00E7\u00F5es:\nd, dd = dia;  ddd, dddd = dia da semana;  M = m\u00EAs;  y = ano",
    L"Calend\u00E1rio",
    L"Quando um ano de dois d\u00EDgitos for digitado, interpret\u00E1-lo como um ano &entre:",
    L"e",
    L"Primeiro dia da &semana:",
    L"&Tipo de calend\u00E1rio:",
    L"Ajustar data &Hijri para:",
    L"Voc\u00EA pode controlar o modo como alguns programas classificam caracteres, palavras, arquivos e pastas.",
    L"&Selecione o m\u00E9todo de classifica\u00E7\u00E3o:",
    L"Alguns softwares, incluindo o Windows, podem fornecer conte\u00FAdo adicional para um determinado local. Alguns servi\u00E7os fornecem informa\u00E7\u00F5es locais, como not\u00EDcias e previs\u00E3o do tempo.",
    L"&Local atual:",
    L"Consulte tamb\u00E9m",
    L"As &configura\u00E7\u00F5es do usu\u00E1rio atual, da tela de boas-vindas (contas do sistema) e das novas contas de usu\u00E1rio s\u00E3o mostradas abaixo.",
    L"* Localidade personalizada",
    L"Copiar as configura\u00E7\u00F5es atuais para:",
    L"&Tela de boas-vindas e contas do sistema",
    L"&Novas contas de usu\u00E1rio",
    L"O idioma de exibi\u00E7\u00E3o das novas contas de usu\u00E1rio no momento \u00E9 herdado do idioma de exibi\u00E7\u00E3o da tela de boas-vindas.",
    nullptr,
    L"Cancelar",
    L"Selecione o idioma (localidade do sistema) a ser usado ao exibir texto em programas que n\u00E3o d\u00E3o suporte a Unicode. Esta configura\u00E7\u00E3o afeta todas as contas de usu\u00E1rio do computador.",
    L"Localidade atual do &sistema:",
    L"<A>Alterar m\u00E9todo de classifica\u00E7\u00E3o</A>",
    L"<A>O que significa a nota\u00E7\u00E3o?</A>",
    L"<A>Saiba mais online sobre como alterar idiomas e formatos regionais</A>",
    L"<A>Como alterar o layout de teclado da tela de boas-vindas?</A>",
    L"<A>Como instalar outros idiomas?</A>",
    L"<A>Saiba mais sobre essas contas</A>",
    L"<A>O que \u00E9 a localidade do sistema?</A>",
    L"<A>Local padr\u00E3o</A>",
};
static const wchar_t* const kTitleTr_PT[11] = {
    L"Formatos",
    L"Teclados e idiomas",
    L"Administrativo",
    L"N\u00FAmeros",
    L"Moeda",
    L"Hora",
    L"Data",
    L"Classifica\u00E7\u00E3o",
    L"Local",
    L"Configura\u00E7\u00F5es de tela de boas-vindas e novas contas",
    L"Configura\u00E7\u00F5es de regi\u00E3o e idioma",
};
// ================= NEDERLANDS (nl-NL) =================
static const wchar_t* const kStrTr_NL[66] = {
    L"Land en taal",
    L"Pas de instellingen aan voor de weergave van talen, getallen, tijden en datums.",
    L"Notatie aanpassen",
    L"Een of meer van uw landinstellingen zijn ongeldig. Controleer en corrigeer de aanpasbare instellingen om dit probleem op te lossen.",
    nullptr,
    nullptr,
    L"Metrisch",
    nullptr,
    L"Een of meer tekens die u in dit veld hebt ingevoerd zijn ongeldig. Probeer andere tekens te gebruiken.",
    L"Een of meer tekens die u voor %s hebt ingevoerd zijn ongeldig. Gebruik een ander teken of voer een spatie in.",
    L"Decimaalteken",
    L"Minteken",
    L"Groeperingsteken",
    L"AM-symbool",
    L"PM-symbool",
    L"Valutasymbool",
    L"Decimaalteken voor valuta",
    L"Groeperingsteken voor valuta",
    L"Een of meer tekens die u voor de notatie %s hebt ingevoerd zijn ongeldig. Probeer andere tekens te gebruiken.",
    L"Lange tijd",
    L"Korte datum",
    L"Lange datum",
    L"De waarde in dit veld moet een getal tussen 99 en 9999 zijn. Probeer een ander getal te gebruiken.",
    L"Korte tijd",
    L"&Notatie:",
    L"&Notatie: (* aangepaste landinstelling)",
    nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
    L"De systeemlandinstelling is gewijzigd. U moet Windows opnieuw opstarten om de wijzigingen door te voeren.",
    L"Landopties wijzigen",
    L"De gekozen landinstelling kan niet worden ge\u00EFnstalleerd. Neem contact op met de systeembeheerder.",
    L"De weergavetaal van het systeem is gewijzigd. U moet Windows opnieuw opstarten om de wijzigingen door te voeren.",
    L"Alle aanpassingen voor de huidige notatie verwijderen?",
    L"Wilt u de wijzigingen voor land en taal toepassen?",
    L"Nu opnieuw opstarten",
    L"Annuleren",
    L"Zorg ervoor dat u uw werk opslaat en alle geopende programma's sluit voordat u opnieuw opstart.",
    L"Systeemlandinstelling",
    L"Windows kan de toetsenbordindeling %s niet correct laden.",
    L"Spaans (Spanje)",
    L"U moet zich afmelden om de wijzigingen van de weergavetaal door te voeren",
    L"Zorg ervoor dat u uw werk opslaat en alle geopende programma's sluit voordat u zich afmeldt.",
    L"Nu afmelden",
    L"Annuleren",
    L"Weergavetaal wijzigen",
    L"Wij raden u aan deze wijzigingen toe te passen voordat u verdere systeemwijzigingen aanbrengt, zodat de computer deze wijzigingen overneemt.",
    L"Toepassen",
    L"Annuleren",
    L"De taak kan niet worden voltooid",
    L"Huidige gebruiker",
    L"Welkomstscherm",
    L"Nieuwe gebruikersaccounts",
    L"Weergavetaal:",
    L"Invoertaal:",
    L"Notatie:",
    L"Locatie:",
    L"Instelling kan niet worden gelezen",
    L"Context",
    L"Nooit",
    L"Nationaal",
};
static const wchar_t* const kDlgTr_NL[92] = {
    L"Datum- en tijdnotaties",
    L"&Korte datum:",
    L"&Lange datum:",
    L"K&orte tijd:",
    L"Lan&ge tijd:",
    L"&Eerste dag van de week:",
    L"Voorbeelden",
    L"Korte datum:",
    L"Lange datum:",
    L"Korte tijd:",
    L"Lange tijd:",
    L"&Aanvullende instellingen...",
    L"Toetsenborden en andere invoertalen",
    L"Klik op Toetsenborden wijzigen om uw toetsenbord of invoertaal te wijzigen.",
    L"Toetsenborden &wijzigen...",
    L"Weergavetaal",
    L"Installeer talen die Windows kan gebruiken om tekst weer te geven en, waar ondersteund, spraak en handschrift te herkennen, of verwijder deze.",
    L"Talen &installeren/verwijderen...",
    L"Als gastgebruiker kunt u de weergavetaal niet wijzigen:",
    L"De selectie van de weergavetaal wordt geblokkeerd door groepsbeleid.",
    L"&Kies een weergavetaal:",
    L"&Sommige tekst is niet vertaald naar de geselecteerde taal. Selecteer een andere taal die Windows moet gebruiken om deze tekst weer te geven:",
    L"Deze taal is slechts gedeeltelijk vertaald en mogelijk wordt sommige tekst weergegeven in:",
    L"&Ook deze taal is slechts gedeeltelijk vertaald. Selecteer een derde taal die Windows moet gebruiken om de resterende tekst weer te geven:",
    L"Ook deze taal is slechts gedeeltelijk vertaald en mogelijk wordt sommige tekst weergegeven in: ",
    L"Welkomstscherm en nieuwe gebruikersaccounts",
    L"Bekijk en kopieer uw internationale instellingen naar het welkomstscherm, systeemaccounts en nieuwe gebruikersaccounts.",
    L"Instellingen &kopi\u00EBren...",
    L"Taal voor niet-Unicode-programma's",
    L"Deze instelling (systeemlandinstelling) bepaalt de taal die wordt gebruikt bij het weergeven van tekst in programma's die geen Unicode ondersteunen.",
    L"Huidige taal voor niet-Unicode-programma's:",
    nullptr,
    L"&Systeemlandinstelling...",
    L"Voorbeeld",
    L"Positief:",
    L"Negatief:",
    L"&Decimaalteken:",
    L"&Aantal decimalen:",
    L"Groeperingsteken voor &cijfers:",
    L"C&ijfergroepering:",
    L"Symbool voor &minteken:",
    L"Negatieve &getalnotatie:",
    L"Voorloopnullen &weergeven:",
    L"&Lijstscheidingsteken:",
    L"M&eetsysteem:",
    L"&Standaardcijfers:",
    L"Eigen cij&fers gebruiken:",
    L"Klik op Opnieuw instellen om de standaardsysteeminstellingen voor getallen, valuta, tijd en datum te herstellen.",
    L"&Opnieuw instellen",
    L"&Valutasymbool:",
    L"&Positieve valutanotatie:",
    L"&Negatieve valutanotatie:",
    L"Aant&al decimalen:",
    L"Groeperingsteken voor c&ijfers:",
    L"Cijfergroeperin&g:",
    L"Tijdnotaties",
    L"&Korte tijd:",
    L"Lan&ge tijd:",
    L"&AM-symbool:",
    L"&PM-symbool:",
    L"Betekenis van de notaties:\n\nh = uur   m = minuut\ns = seconde (alleen lange tijd)\ntt = AM of PM\n\nh/H = 12/24 uur\n\nhh, mm, ss = voorloopnul weergeven\nh, m, s = geen voorloopnul weergeven",
    L"Datumnotaties",
    L"Betekenis van de notaties:\nd, dd = dag;  ddd, dddd = dag van de week;  M = maand;  y = jaar",
    L"Agenda",
    L"Wanneer een jaartal van twee cijfers wordt ingevoerd, interpreteren als een jaar &tussen:",
    L"en",
    L"Eerste dag van de &week:",
    L"&Agendatype:",
    L"Hidjri-datum aa&npassen aan:",
    L"U kunt bepalen hoe sommige programma's tekens, woorden, bestanden en mappen sorteren.",
    L"&Selecteer de sorteermethode:",
    L"Sommige software, waaronder Windows, biedt u mogelijk extra inhoud voor een bepaalde locatie. Sommige services bieden lokale informatie, zoals nieuws en weer.",
    L"&Huidige locatie:",
    L"Zie ook",
    L"De &instellingen voor de huidige gebruiker, het welkomstscherm (systeemaccounts) en nieuwe gebruikersaccounts worden hieronder weergegeven.",
    L"* Aangepaste landinstelling",
    L"Uw huidige instellingen kopi\u00EBren naar:",
    L"&Welkomstscherm en systeemaccounts",
    L"&Nieuwe gebruikersaccounts",
    L"De weergavetaal voor nieuwe gebruikersaccounts wordt momenteel overgenomen van de weergavetaal van het welkomstscherm.",
    nullptr,
    L"Annuleren",
    L"Selecteer welke taal (systeemlandinstelling) moet worden gebruikt bij het weergeven van tekst in programma's die geen Unicode ondersteunen. Deze instelling is van invloed op alle gebruikersaccounts op de computer.",
    L"&Huidige systeemlandinstelling:",
    L"<A>Sorteermethode wijzigen</A>",
    L"<A>Wat betekent de notatie?</A>",
    L"<A>Online meer informatie over het wijzigen van talen en regionale notaties</A>",
    L"<A>Hoe wijzig ik de toetsenbordindeling voor het welkomstscherm?</A>",
    L"<A>Hoe kan ik extra talen installeren?</A>",
    L"<A>Meer informatie over deze accounts</A>",
    L"<A>Wat is de systeemlandinstelling?</A>",
    L"<A>Standaardlocatie</A>",
};
static const wchar_t* const kTitleTr_NL[11] = {
    L"Notaties",
    L"Toetsenborden en talen",
    L"Beheer",
    L"Getallen",
    L"Valuta",
    L"Tijd",
    L"Datum",
    L"Sorteren",
    L"Locatie",
    L"Instellingen voor welkomstscherm en nieuwe accounts",
    L"Land- en taalinstellingen",
};
// ================= POLSKI (pl-PL) =================
static const wchar_t* const kStrTr_PL[66] = {
    L"Region i j\u0119zyk",
    L"Dostosuj ustawienia wy\u015Bwietlania j\u0119zyk\u00F3w, liczb, godzin i dat.",
    L"Dostosuj format",
    L"Co najmniej jedno z ustawie\u0144 regionalnych jest nieprawid\u0142owe. Aby rozwi\u0105za\u0107 ten problem, przejrzyj i popraw ustawienia konfigurowalne.",
    nullptr,
    nullptr,
    L"Metryczny",
    nullptr,
    L"Co najmniej jeden ze znak\u00F3w wprowadzonych w tym polu jest nieprawid\u0142owy. Spr\u00F3buj u\u017Cy\u0107 innych znak\u00F3w.",
    L"Co najmniej jeden ze znak\u00F3w wprowadzonych dla %s jest nieprawid\u0142owy. Spr\u00F3buj u\u017Cy\u0107 innego znaku lub wprowad\u017A spacj\u0119.",
    L"Symbol dziesi\u0119tny",
    L"Znak ujemny",
    L"Symbol grupuj\u0105cy",
    L"Symbol AM",
    L"Symbol PM",
    L"Symbol waluty",
    L"Symbol dziesi\u0119tny waluty",
    L"Symbol grupuj\u0105cy waluty",
    L"Co najmniej jeden ze znak\u00F3w wprowadzonych dla formatu %s jest nieprawid\u0142owy. Spr\u00F3buj u\u017Cy\u0107 innych znak\u00F3w.",
    L"D\u0142uga godzina",
    L"Kr\u00F3tka data",
    L"D\u0142uga data",
    L"Warto\u015B\u0107 w tym polu musi by\u0107 liczb\u0105 z zakresu od 99 do 9999. Spr\u00F3buj u\u017Cy\u0107 innej liczby.",
    L"Kr\u00F3tka godzina",
    L"&Format:",
    L"&Format: (* ustawienia regionalne niestandardowe)",
    nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
    L"Ustawienia regionalne systemu zosta\u0142y zmienione. Aby zmiany odnios\u0142y skutek, nale\u017Cy ponownie uruchomi\u0107 system Windows.",
    L"Zmie\u0144 opcje regionalne",
    L"Nie mo\u017Cna zainstalowa\u0107 wybranych ustawie\u0144 regionalnych. Skontaktuj si\u0119 z administratorem systemu.",
    L"J\u0119zyk wy\u015Bwietlania systemu zosta\u0142 zmieniony. Aby zmiany odnios\u0142y skutek, nale\u017Cy ponownie uruchomi\u0107 system Windows.",
    L"Usun\u0105\u0107 wszystkie dostosowania bie\u017C\u0105cego formatu?",
    L"Czy zastosowa\u0107 zmiany regionu i j\u0119zyka?",
    L"Uruchom ponownie",
    L"Anuluj",
    L"Przed ponownym uruchomieniem zapisz swoj\u0105 prac\u0119 i zamknij wszystkie otwarte programy.",
    L"Ustawienia regionalne systemu",
    L"System Windows nie m\u00F3g\u0142 poprawnie za\u0142adowa\u0107 uk\u0142adu klawiatury %s.",
    L"Hiszpa\u0144ski (Hiszpania)",
    L"Aby zmiany j\u0119zyka wy\u015Bwietlania odnios\u0142y skutek, musisz si\u0119 wylogowa\u0107",
    L"Przed wylogowaniem zapisz swoj\u0105 prac\u0119 i zamknij wszystkie otwarte programy.",
    L"Wyloguj teraz",
    L"Anuluj",
    L"Zmie\u0144 j\u0119zyk wy\u015Bwietlania",
    L"Aby komputer uwzgl\u0119dni\u0142 te zmiany, zalecamy zastosowanie ich przed wprowadzeniem dalszych zmian systemowych.",
    L"Zastosuj",
    L"Anuluj",
    L"Nie mo\u017Cna uko\u0144czy\u0107 zadania",
    L"Bie\u017C\u0105cy u\u017Cytkownik",
    L"Ekran powitalny",
    L"Nowe konta u\u017Cytkownik\u00F3w",
    L"J\u0119zyk wy\u015Bwietlania:",
    L"J\u0119zyk wprowadzania:",
    L"Format:",
    L"Lokalizacja:",
    L"Nie mo\u017Cna odczyta\u0107 ustawienia",
    L"Kontekst",
    L"Nigdy",
    L"Narodowy",
};
static const wchar_t* const kDlgTr_PL[92] = {
    L"Formaty daty i godziny",
    L"&Kr\u00F3tka data:",
    L"D&\u0142uga data:",
    L"Kr&\u00F3tka godzina:",
    L"&D\u0142uga godzina:",
    L"Pierwszy dzie\u0144 t&ygodnia:",
    L"Przyk\u0142ady",
    L"Kr\u00F3tka data:",
    L"D\u0142uga data:",
    L"Kr\u00F3tka godzina:",
    L"D\u0142uga godzina:",
    L"Dodatkowe &ustawienia...",
    L"Klawiatury i inne j\u0119zyki wprowadzania",
    L"Aby zmieni\u0107 klawiatur\u0119 lub j\u0119zyk wprowadzania, kliknij Zmie\u0144 klawiatury.",
    L"&Zmie\u0144 klawiatury...",
    L"J\u0119zyk wy\u015Bwietlania",
    L"Zainstaluj lub odinstaluj j\u0119zyki, kt\u00F3rych system Windows mo\u017Ce u\u017Cywa\u0107 do wy\u015Bwietlania tekstu oraz, je\u015Bli jest obs\u0142ugiwane, rozpoznawania mowy i pisma odr\u0119cznego.",
    L"&Zainstaluj/odinstaluj j\u0119zyki...",
    L"Jako u\u017Cytkownik-go\u015B\u0107 nie mo\u017Cesz zmieni\u0107 j\u0119zyka wy\u015Bwietlania:",
    L"Wyb\u00F3r j\u0119zyka wy\u015Bwietlania jest zablokowany przez zasady grupy.",
    L"&Wybierz j\u0119zyk wy\u015Bwietlania:",
    L"&Niekt\u00F3re teksty nie zosta\u0142y przet\u0142umaczone na wybrany j\u0119zyk. Wybierz inny j\u0119zyk, kt\u00F3rego system Windows ma u\u017Cywa\u0107 do wy\u015Bwietlania tego tekstu:",
    L"Ten j\u0119zyk jest przet\u0142umaczony tylko cz\u0119\u015Bciowo i niekt\u00F3re teksty mog\u0105 by\u0107 wy\u015Bwietlane w:",
    L"&Ten j\u0119zyk r\u00F3wnie\u017C jest przet\u0142umaczony tylko cz\u0119\u015Bciowo. Wybierz trzeci j\u0119zyk, kt\u00F3rego system Windows ma u\u017Cywa\u0107 do wy\u015Bwietlania pozosta\u0142ego tekstu:",
    L"Ten j\u0119zyk r\u00F3wnie\u017C jest przet\u0142umaczony tylko cz\u0119\u015Bciowo i niekt\u00F3re teksty mog\u0105 by\u0107 wy\u015Bwietlane w: ",
    L"Ekran powitalny i nowe konta u\u017Cytkownik\u00F3w",
    L"Wy\u015Bwietl i skopiuj swoje ustawienia mi\u0119dzynarodowe na ekran powitalny, konta systemowe i nowe konta u\u017Cytkownik\u00F3w.",
    L"&Kopiuj ustawienia...",
    L"J\u0119zyk dla program\u00F3w nieobs\u0142uguj\u0105cych kodu Unicode",
    L"To ustawienie (ustawienia regionalne systemu) kontroluje j\u0119zyk u\u017Cywany podczas wy\u015Bwietlania tekstu w programach, kt\u00F3re nie obs\u0142uguj\u0105 kodu Unicode.",
    L"Bie\u017C\u0105cy j\u0119zyk dla program\u00F3w nieobs\u0142uguj\u0105cych kodu Unicode:",
    nullptr,
    L"Ustawienia regionalne &systemu...",
    L"Przyk\u0142ad",
    L"Dodatnie:",
    L"Ujemne:",
    L"Symbol d&ziesi\u0119tny:",
    L"&Liczba cyfr po przecinku:",
    L"Symbol grupuj\u0105cy c&yfry:",
    L"&Grupowanie cyfr:",
    L"Symbol z&naku ujemnego:",
    L"Format liczb u&jemnych:",
    L"Wy\u015Bwietl zera &wiod\u0105ce:",
    L"Separator li&sty:",
    L"Syste&m miar:",
    L"&Cyfry standardowe:",
    L"&U\u017Cyj cyfr rodzimych:",
    L"Kliknij Resetuj, aby przywr\u00F3ci\u0107 domy\u015Blne ustawienia systemowe liczb, waluty, godziny i daty.",
    L"&Resetuj",
    L"Symbol &waluty:",
    L"&Dodatni format waluty:",
    L"&Ujemny format waluty:",
    L"Liczba cyfr po przecin&ku:",
    L"Symbol grupuj\u0105cy cy&fry:",
    L"Grupowanie c&yfr:",
    L"Formaty godziny",
    L"&Kr\u00F3tka godzina:",
    L"D\u0142u&ga godzina:",
    L"Symbol &AM:",
    L"Symbol &PM:",
    L"Znaczenie notacji:\n\nh = godzina   m = minuta\ns = sekunda (tylko d\u0142uga godzina)\ntt = AM lub PM\n\nh/H = 12/24 godziny\n\nhh, mm, ss = wy\u015Bwietl zero wiod\u0105ce\nh, m, s = nie wy\u015Bwietlaj zera wiod\u0105cego",
    L"Formaty daty",
    L"Znaczenie notacji:\nd, dd = dzie\u0144;  ddd, dddd = dzie\u0144 tygodnia;  M = miesi\u0105c;  y = rok",
    L"Kalendarz",
    L"Gdy wprowadzono rok dwucyfrowy, interpretuj go jako rok &mi\u0119dzy:",
    L"a",
    L"Pierwszy dzie\u0144 t&ygodnia:",
    L"&Typ kalendarza:",
    L"Dostosuj dat\u0119 &hid\u017Cry do:",
    L"Mo\u017cesz kontrolowa\u0107 spos\u00F3b sortowania znak\u00F3w, wyraz\u00F3w, plik\u00F3w i folder\u00F3w przez niekt\u00F3re programy.",
    L"&Wybierz metod\u0119 sortowania:",
    L"Niekt\u00F3re oprogramowanie, w tym system Windows, mo\u017Ce udost\u0119pnia\u0107 dodatkow\u0105 zawarto\u015B\u0107 dla okre\u015Blonej lokalizacji. Niekt\u00F3re us\u0142ugi udost\u0119pniaj\u0105 informacje lokalne, takie jak wiadomo\u015Bci i pogoda.",
    L"&Bie\u017C\u0105ca lokalizacja:",
    L"Zobacz te\u017C",
    L"&Ustawienia bie\u017C\u0105cego u\u017Cytkownika, ekranu powitalnego (kont systemowych) i nowych kont u\u017Cytkownik\u00F3w s\u0105 wy\u015Bwietlone poni\u017Cej.",
    L"* Niestandardowe ustawienia regionalne",
    L"Kopiuj bie\u017C\u0105ce ustawienia do:",
    L"&Ekran powitalny i konta systemowe",
    L"N&owe konta u\u017Cytkownik\u00F3w",
    L"J\u0119zyk wy\u015Bwietlania nowych kont u\u017Cytkownik\u00F3w jest obecnie dziedziczony z j\u0119zyka wy\u015Bwietlania ekranu powitalnego.",
    nullptr,
    L"Anuluj",
    L"Wybierz j\u0119zyk (ustawienia regionalne systemu), kt\u00F3ry ma by\u0107 u\u017Cywany podczas wy\u015Bwietlania tekstu w programach, kt\u00F3re nie obs\u0142uguj\u0105 kodu Unicode. To ustawienie dotyczy wszystkich kont u\u017Cytkownik\u00F3w na komputerze.",
    L"&Bie\u017C\u0105ce ustawienia regionalne systemu:",
    L"<A>Zmie\u0144 metod\u0119 sortowania</A>",
    L"<A>Co oznacza ta notacja?</A>",
    L"<A>Dowiedz si\u0119 wi\u0119cej online o zmienianiu j\u0119zyk\u00F3w i format\u00F3w regionalnych</A>",
    L"<A>Jak zmieni\u0107 uk\u0142ad klawiatury dla ekranu powitalnego?</A>",
    L"<A>Jak zainstalowa\u0107 dodatkowe j\u0119zyki?</A>",
    L"<A>Powiedz mi wi\u0119cej o tych kontach</A>",
    L"<A>Co to s\u0105 ustawienia regionalne systemu?</A>",
    L"<A>Domy\u015Blna lokalizacja</A>",
};
static const wchar_t* const kTitleTr_PL[11] = {
    L"Formaty",
    L"Klawiatury i j\u0119zyki",
    L"Administracyjne",
    L"Liczby",
    L"Waluta",
    L"Godzina",
    L"Data",
    L"Sortowanie",
    L"Lokalizacja",
    L"Ustawienia ekranu powitalnego i nowych kont",
    L"Ustawienia regionu i j\u0119zyka",
};
// ================= RUSSKIJ (ru-RU) =================
static const wchar_t* const kStrTr_RU[66] = {
    L"\u042F\u0437\u044B\u043A \u0438 \u0440\u0435\u0433\u0438\u043E\u043D\u0430\u043B\u044C\u043D\u044B\u0435 \u0441\u0442\u0430\u043D\u0434\u0430\u0440\u0442\u044B",
    L"\u041D\u0430\u0441\u0442\u0440\u043E\u0439\u043A\u0430 \u043F\u0430\u0440\u0430\u043C\u0435\u0442\u0440\u043E\u0432 \u043E\u0442\u043E\u0431\u0440\u0430\u0436\u0435\u043D\u0438\u044F \u044F\u0437\u044B\u043A\u043E\u0432, \u0447\u0438\u0441\u0435\u043B, \u0432\u0440\u0435\u043C\u0435\u043D\u0438 \u0438 \u0434\u0430\u0442\u044B.",
    L"\u0418\u0437\u043C\u0435\u043D\u0438\u0442\u044C \u0444\u043E\u0440\u043C\u0430\u0442",
    L"\u041E\u0434\u0438\u043D \u0438\u043B\u0438 \u043D\u0435\u0441\u043A\u043E\u043B\u044C\u043A\u043E \u0440\u0435\u0433\u0438\u043E\u043D\u0430\u043B\u044C\u043D\u044B\u0445 \u043F\u0430\u0440\u0430\u043C\u0435\u0442\u0440\u043E\u0432 \u043D\u0435\u0434\u043E\u043F\u0443\u0441\u0442\u0438\u043C\u044B. \u0414\u043B\u044F \u0443\u0441\u0442\u0440\u0430\u043D\u0435\u043D\u0438\u044F \u043F\u0440\u043E\u0431\u043B\u0435\u043C\u044B \u043F\u0440\u043E\u0441\u043C\u043E\u0442\u0440\u0438\u0442\u0435 \u0438 \u0438\u0441\u043F\u0440\u0430\u0432\u044C\u0442\u0435 \u043D\u0430\u0441\u0442\u0440\u0430\u0438\u0432\u0430\u0435\u043C\u044B\u0435 \u043F\u0430\u0440\u0430\u043C\u0435\u0442\u0440\u044B.",
    nullptr,
    nullptr,
    L"\u041C\u0435\u0442\u0440\u0438\u0447\u0435\u0441\u043A\u0430\u044F",
    nullptr,
    L"\u041E\u0434\u0438\u043D \u0438\u043B\u0438 \u043D\u0435\u0441\u043A\u043E\u043B\u044C\u043A\u043E \u0441\u0438\u043C\u0432\u043E\u043B\u043E\u0432, \u0432\u0432\u0435\u0434\u0435\u043D\u043D\u044B\u0445 \u0432 \u044D\u0442\u043E \u043F\u043E\u043B\u0435, \u043D\u0435\u0434\u043E\u043F\u0443\u0441\u0442\u0438\u043C\u044B. \u041F\u043E\u043F\u0440\u043E\u0431\u0443\u0439\u0442\u0435 \u0438\u0441\u043F\u043E\u043B\u044C\u0437\u043E\u0432\u0430\u0442\u044C \u0434\u0440\u0443\u0433\u0438\u0435 \u0441\u0438\u043C\u0432\u043E\u043B\u044B.",
    L"\u041E\u0434\u0438\u043D \u0438\u043B\u0438 \u043D\u0435\u0441\u043A\u043E\u043B\u044C\u043A\u043E \u0441\u0438\u043C\u0432\u043E\u043B\u043E\u0432, \u0432\u0432\u0435\u0434\u0435\u043D\u043D\u044B\u0445 \u0434\u043B\u044F %s, \u043D\u0435\u0434\u043E\u043F\u0443\u0441\u0442\u0438\u043C\u044B. \u041F\u043E\u043F\u0440\u043E\u0431\u0443\u0439\u0442\u0435 \u0438\u0441\u043F\u043E\u043B\u044C\u0437\u043E\u0432\u0430\u0442\u044C \u0434\u0440\u0443\u0433\u043E\u0439 \u0441\u0438\u043C\u0432\u043E\u043B \u0438\u043B\u0438 \u0432\u0432\u0435\u0434\u0438\u0442\u0435 \u043F\u0440\u043E\u0431\u0435\u043B.",
    L"\u0414\u0435\u0441\u044F\u0442\u0438\u0447\u043D\u044B\u0439 \u0440\u0430\u0437\u0434\u0435\u043B\u0438\u0442\u0435\u043B\u044C",
    L"\u0417\u043D\u0430\u043A \u043C\u0438\u043D\u0443\u0441\u0430",
    L"\u0420\u0430\u0437\u0434\u0435\u043B\u0438\u0442\u0435\u043B\u044C \u0433\u0440\u0443\u043F\u043F",
    L"\u0421\u0438\u043C\u0432\u043E\u043B AM",
    L"\u0421\u0438\u043C\u0432\u043E\u043B PM",
    L"\u0421\u0438\u043C\u0432\u043E\u043B \u0432\u0430\u043B\u044E\u0442\u044B",
    L"\u0414\u0435\u0441\u044F\u0442\u0438\u0447\u043D\u044B\u0439 \u0440\u0430\u0437\u0434\u0435\u043B\u0438\u0442\u0435\u043B\u044C \u0432\u0430\u043B\u044E\u0442\u044B",
    L"\u0420\u0430\u0437\u0434\u0435\u043B\u0438\u0442\u0435\u043B\u044C \u0433\u0440\u0443\u043F\u043F \u0432\u0430\u043B\u044E\u0442\u044B",
    L"\u041E\u0434\u0438\u043D \u0438\u043B\u0438 \u043D\u0435\u0441\u043A\u043E\u043B\u044C\u043A\u043E \u0441\u0438\u043C\u0432\u043E\u043B\u043E\u0432, \u0432\u0432\u0435\u0434\u0435\u043D\u043D\u044B\u0445 \u0434\u043B\u044F \u0444\u043E\u0440\u043C\u0430\u0442\u0430 %s, \u043D\u0435\u0434\u043E\u043F\u0443\u0441\u0442\u0438\u043C\u044B. \u041F\u043E\u043F\u0440\u043E\u0431\u0443\u0439\u0442\u0435 \u0438\u0441\u043F\u043E\u043B\u044C\u0437\u043E\u0432\u0430\u0442\u044C \u0434\u0440\u0443\u0433\u0438\u0435 \u0441\u0438\u043C\u0432\u043E\u043B\u044B.",
    L"\u041F\u043E\u043B\u043D\u043E\u0435 \u0432\u0440\u0435\u043C\u044F",
    L"\u041A\u0440\u0430\u0442\u043A\u0430\u044F \u0434\u0430\u0442\u0430",
    L"\u041F\u043E\u043B\u043D\u0430\u044F \u0434\u0430\u0442\u0430",
    L"\u0417\u043D\u0430\u0447\u0435\u043D\u0438\u0435 \u0432 \u044D\u0442\u043E\u043C \u043F\u043E\u043B\u0435 \u0434\u043E\u043B\u0436\u043D\u043E \u0431\u044B\u0442\u044C \u0447\u0438\u0441\u043B\u043E\u043C \u043E\u0442 99 \u0434\u043E 9999. \u041F\u043E\u043F\u0440\u043E\u0431\u0443\u0439\u0442\u0435 \u0438\u0441\u043F\u043E\u043B\u044C\u0437\u043E\u0432\u0430\u0442\u044C \u0434\u0440\u0443\u0433\u043E\u0435 \u0447\u0438\u0441\u043B\u043E.",
    L"\u041A\u0440\u0430\u0442\u043A\u043E\u0435 \u0432\u0440\u0435\u043C\u044F",
    L"&\u0424\u043E\u0440\u043C\u0430\u0442:",
    L"&\u0424\u043E\u0440\u043C\u0430\u0442: (* \u043D\u0430\u0441\u0442\u0440\u0430\u0438\u0432\u0430\u0435\u043C\u044B\u0439 \u044F\u0437\u044B\u043A)",
    nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
    L"\u042F\u0437\u044B\u043A \u0441\u0438\u0441\u0442\u0435\u043C\u044B \u0438\u0437\u043C\u0435\u043D\u0435\u043D. \u0414\u043B\u044F \u0432\u0441\u0442\u0443\u043F\u043B\u0435\u043D\u0438\u044F \u0438\u0437\u043C\u0435\u043D\u0435\u043D\u0438\u0439 \u0432 \u0441\u0438\u043B\u0443 \u043D\u0435\u043E\u0431\u0445\u043E\u0434\u0438\u043C\u043E \u043F\u0435\u0440\u0435\u0437\u0430\u043F\u0443\u0441\u0442\u0438\u0442\u044C Windows.",
    L"\u0418\u0437\u043C\u0435\u043D\u0438\u0442\u044C \u0440\u0435\u0433\u0438\u043E\u043D\u0430\u043B\u044C\u043D\u044B\u0435 \u043F\u0430\u0440\u0430\u043C\u0435\u0442\u0440\u044B",
    L"\u041D\u0435 \u0443\u0434\u0430\u043B\u043E\u0441\u044C \u0443\u0441\u0442\u0430\u043D\u043E\u0432\u0438\u0442\u044C \u0432\u044B\u0431\u0440\u0430\u043D\u043D\u044B\u0439 \u044F\u0437\u044B\u043A. \u041E\u0431\u0440\u0430\u0442\u0438\u0442\u0435\u0441\u044C \u043A \u0441\u0438\u0441\u0442\u0435\u043C\u043D\u043E\u043C\u0443 \u0430\u0434\u043C\u0438\u043D\u0438\u0441\u0442\u0440\u0430\u0442\u043E\u0440\u0443.",
    L"\u042F\u0437\u044B\u043A \u0438\u043D\u0442\u0435\u0440\u0444\u0435\u0439\u0441\u0430 \u0441\u0438\u0441\u0442\u0435\u043C\u044B \u0438\u0437\u043C\u0435\u043D\u0435\u043D. \u0414\u043B\u044F \u0432\u0441\u0442\u0443\u043F\u043B\u0435\u043D\u0438\u044F \u0438\u0437\u043C\u0435\u043D\u0435\u043D\u0438\u0439 \u0432 \u0441\u0438\u043B\u0443 \u043D\u0435\u043E\u0431\u0445\u043E\u0434\u0438\u043C\u043E \u043F\u0435\u0440\u0435\u0437\u0430\u043F\u0443\u0441\u0442\u0438\u0442\u044C Windows.",
    L"\u0423\u0434\u0430\u043B\u0438\u0442\u044C \u0432\u0441\u0435 \u043D\u0430\u0441\u0442\u0440\u043E\u0439\u043A\u0438 \u0442\u0435\u043A\u0443\u0449\u0435\u0433\u043E \u0444\u043E\u0440\u043C\u0430\u0442\u0430?",
    L"\u041F\u0440\u0438\u043C\u0435\u043D\u0438\u0442\u044C \u0438\u0437\u043C\u0435\u043D\u0435\u043D\u0438\u044F \u044F\u0437\u044B\u043A\u0430 \u0438 \u0440\u0435\u0433\u0438\u043E\u043D\u0430\u043B\u044C\u043D\u044B\u0445 \u0441\u0442\u0430\u043D\u0434\u0430\u0440\u0442\u043E\u0432?",
    L"\u041F\u0435\u0440\u0435\u0437\u0430\u0433\u0440\u0443\u0437\u0438\u0442\u044C \u0441\u0435\u0439\u0447\u0430\u0441",
    L"\u041E\u0442\u043C\u0435\u043D\u0430",
    L"\u041F\u0435\u0440\u0435\u0434 \u043F\u0435\u0440\u0435\u0437\u0430\u0433\u0440\u0443\u0437\u043A\u043E\u0439 \u0441\u043E\u0445\u0440\u0430\u043D\u0438\u0442\u0435 \u0440\u0430\u0431\u043E\u0442\u0443 \u0438 \u0437\u0430\u043A\u0440\u043E\u0439\u0442\u0435 \u0432\u0441\u0435 \u043E\u0442\u043A\u0440\u044B\u0442\u044B\u0435 \u043F\u0440\u043E\u0433\u0440\u0430\u043C\u043C\u044B.",
    L"\u042F\u0437\u044B\u043A \u0441\u0438\u0441\u0442\u0435\u043C\u044B",
    L"\u041D\u0435 \u0443\u0434\u0430\u043B\u043E\u0441\u044C \u043F\u0440\u0430\u0432\u0438\u043B\u044C\u043D\u043E \u0437\u0430\u0433\u0440\u0443\u0437\u0438\u0442\u044C \u0440\u0430\u0441\u043A\u043B\u0430\u0434\u043A\u0443 \u043A\u043B\u0430\u0432\u0438\u0430\u0442\u0443\u0440\u044B %s.",
    L"\u0418\u0441\u043F\u0430\u043D\u0441\u043A\u0438\u0439 (\u0418\u0441\u043F\u0430\u043D\u0438\u044F)",
    L"\u0414\u043B\u044F \u0432\u0441\u0442\u0443\u043F\u043B\u0435\u043D\u0438\u044F \u0432 \u0441\u0438\u043B\u0443 \u0438\u0437\u043C\u0435\u043D\u0435\u043D\u0438\u0439 \u044F\u0437\u044B\u043A\u0430 \u0438\u043D\u0442\u0435\u0440\u0444\u0435\u0439\u0441\u0430 \u043D\u0435\u043E\u0431\u0445\u043E\u0434\u0438\u043C\u043E \u0432\u044B\u0439\u0442\u0438 \u0438\u0437 \u0441\u0438\u0441\u0442\u0435\u043C\u044B",
    L"\u041F\u0435\u0440\u0435\u0434 \u0432\u044B\u0445\u043E\u0434\u043E\u043C \u0438\u0437 \u0441\u0438\u0441\u0442\u0435\u043C\u044B \u0441\u043E\u0445\u0440\u0430\u043D\u0438\u0442\u0435 \u0440\u0430\u0431\u043E\u0442\u0443 \u0438 \u0437\u0430\u043A\u0440\u043E\u0439\u0442\u0435 \u0432\u0441\u0435 \u043E\u0442\u043A\u0440\u044B\u0442\u044B\u0435 \u043F\u0440\u043E\u0433\u0440\u0430\u043C\u043C\u044B.",
    L"\u0412\u044B\u0439\u0442\u0438 \u0441\u0435\u0439\u0447\u0430\u0441",
    L"\u041E\u0442\u043C\u0435\u043D\u0430",
    L"\u0418\u0437\u043C\u0435\u043D\u0438\u0442\u044C \u044F\u0437\u044B\u043A \u0438\u043D\u0442\u0435\u0440\u0444\u0435\u0439\u0441\u0430",
    L"\u0427\u0442\u043E\u0431\u044B \u0438\u0437\u043C\u0435\u043D\u0435\u043D\u0438\u044F \u043E\u0442\u0440\u0430\u0437\u0438\u043B\u0438\u0441\u044C \u043D\u0430 \u043A\u043E\u043C\u043F\u044C\u044E\u0442\u0435\u0440\u0435, \u0440\u0435\u043A\u043E\u043C\u0435\u043D\u0434\u0443\u0435\u0442\u0441\u044F \u043F\u0440\u0438\u043C\u0435\u043D\u0438\u0442\u044C \u0438\u0445 \u043F\u0435\u0440\u0435\u0434 \u0432\u043D\u0435\u0441\u0435\u043D\u0438\u0435\u043C \u0434\u0440\u0443\u0433\u0438\u0445 \u0441\u0438\u0441\u0442\u0435\u043C\u043D\u044B\u0445 \u0438\u0437\u043C\u0435\u043D\u0435\u043D\u0438\u0439.",
    L"\u041F\u0440\u0438\u043C\u0435\u043D\u0438\u0442\u044C",
    L"\u041E\u0442\u043C\u0435\u043D\u0430",
    L"\u041D\u0435 \u0443\u0434\u0430\u043B\u043E\u0441\u044C \u0432\u044B\u043F\u043E\u043B\u043D\u0438\u0442\u044C \u0437\u0430\u0434\u0430\u0447\u0443",
    L"\u0422\u0435\u043A\u0443\u0449\u0438\u0439 \u043F\u043E\u043B\u044C\u0437\u043E\u0432\u0430\u0442\u0435\u043B\u044C",
    L"\u042D\u043A\u0440\u0430\u043D \u043F\u0440\u0438\u0432\u0435\u0442\u0441\u0442\u0432\u0438\u044F",
    L"\u041D\u043E\u0432\u044B\u0435 \u0443\u0447\u0435\u0442\u043D\u044B\u0435 \u0437\u0430\u043F\u0438\u0441\u0438",
    L"\u042F\u0437\u044B\u043A \u0438\u043D\u0442\u0435\u0440\u0444\u0435\u0439\u0441\u0430:",
    L"\u042F\u0437\u044B\u043A \u0432\u0432\u043E\u0434\u0430:",
    L"\u0424\u043E\u0440\u043C\u0430\u0442:",
    L"\u0420\u0430\u0441\u043F\u043E\u043B\u043E\u0436\u0435\u043D\u0438\u0435:",
    L"\u041D\u0435 \u0443\u0434\u0430\u043B\u043E\u0441\u044C \u043F\u0440\u043E\u0447\u0438\u0442\u0430\u0442\u044C \u043F\u0430\u0440\u0430\u043C\u0435\u0442\u0440",
    L"\u041A\u043E\u043D\u0442\u0435\u043A\u0441\u0442",
    L"\u041D\u0438\u043A\u043E\u0433\u0434\u0430",
    L"\u041D\u0430\u0446\u0438\u043E\u043D\u0430\u043B\u044C\u043D\u044B\u0439",
};
static const wchar_t* const kDlgTr_RU[92] = {
    L"\u0424\u043E\u0440\u043C\u0430\u0442\u044B \u0434\u0430\u0442\u044B \u0438 \u0432\u0440\u0435\u043C\u0435\u043D\u0438",
    L"&\u041A\u0440\u0430\u0442\u043A\u0430\u044F \u0434\u0430\u0442\u0430:",
    L"&\u041F\u043E\u043B\u043D\u0430\u044F \u0434\u0430\u0442\u0430:",
    L"\u041A&\u0440\u0430\u0442\u043A\u043E\u0435 \u0432\u0440\u0435\u043C\u044F:",
    L"\u041F\u043E\u043B&\u043D\u043E\u0435 \u0432\u0440\u0435\u043C\u044F:",
    L"\u041F\u0435\u0440\u0432\u044B\u0439 &\u0434\u0435\u043D\u044C \u043D\u0435\u0434\u0435\u043B\u0438:",
    L"\u041F\u0440\u0438\u043C\u0435\u0440\u044B",
    L"\u041A\u0440\u0430\u0442\u043A\u0430\u044F \u0434\u0430\u0442\u0430:",
    L"\u041F\u043E\u043B\u043D\u0430\u044F \u0434\u0430\u0442\u0430:",
    L"\u041A\u0440\u0430\u0442\u043A\u043E\u0435 \u0432\u0440\u0435\u043C\u044F:",
    L"\u041F\u043E\u043B\u043D\u043E\u0435 \u0432\u0440\u0435\u043C\u044F:",
    L"\u0414\u043E\u043F\u043E\u043B\u043D\u0438&\u0442\u0435\u043B\u044C\u043D\u044B\u0435 \u043F\u0430\u0440\u0430\u043C\u0435\u0442\u0440\u044B...",
    L"\u041A\u043B\u0430\u0432\u0438\u0430\u0442\u0443\u0440\u044B \u0438 \u0434\u0440\u0443\u0433\u0438\u0435 \u044F\u0437\u044B\u043A\u0438 \u0432\u0432\u043E\u0434\u0430",
    L"\u0427\u0442\u043E\u0431\u044B \u0438\u0437\u043C\u0435\u043D\u0438\u0442\u044C \u043A\u043B\u0430\u0432\u0438\u0430\u0442\u0443\u0440\u0443 \u0438\u043B\u0438 \u044F\u0437\u044B\u043A \u0432\u0432\u043E\u0434\u0430, \u043D\u0430\u0436\u043C\u0438\u0442\u0435 \u043A\u043D\u043E\u043F\u043A\u0443 \u0418\u0437\u043C\u0435\u043D\u0438\u0442\u044C \u043A\u043B\u0430\u0432\u0438\u0430\u0442\u0443\u0440\u044B.",
    L"&\u0418\u0437\u043C\u0435\u043D\u0438\u0442\u044C \u043A\u043B\u0430\u0432\u0438\u0430\u0442\u0443\u0440\u044B...",
    L"\u042F\u0437\u044B\u043A \u0438\u043D\u0442\u0435\u0440\u0444\u0435\u0439\u0441\u0430",
    L"\u0423\u0441\u0442\u0430\u043D\u043E\u0432\u0438\u0442\u0435 \u0438\u043B\u0438 \u0443\u0434\u0430\u043B\u0438\u0442\u0435 \u044F\u0437\u044B\u043A\u0438, \u043A\u043E\u0442\u043E\u0440\u044B\u0435 Windows \u043C\u043E\u0436\u0435\u0442 \u0438\u0441\u043F\u043E\u043B\u044C\u0437\u043E\u0432\u0430\u0442\u044C \u0434\u043B\u044F \u043E\u0442\u043E\u0431\u0440\u0430\u0436\u0435\u043D\u0438\u044F \u0442\u0435\u043A\u0441\u0442\u0430 \u0438, \u0433\u0434\u0435 \u043F\u043E\u0434\u0434\u0435\u0440\u0436\u0438\u0432\u0430\u0435\u0442\u0441\u044F, \u0440\u0430\u0441\u043F\u043E\u0437\u043D\u0430\u0432\u0430\u043D\u0438\u044F \u0440\u0435\u0447\u0438 \u0438 \u0440\u0443\u043A\u043E\u043F\u0438\u0441\u043D\u043E\u0433\u043E \u0432\u0432\u043E\u0434\u0430.",
    L"&\u0423\u0441\u0442\u0430\u043D\u043E\u0432\u0438\u0442\u044C \u0438\u043B\u0438 \u0443\u0434\u0430\u043B\u0438\u0442\u044C \u044F\u0437\u044B\u043A\u0438...",
    L"\u0413\u043E\u0441\u0442\u044C \u043D\u0435 \u043C\u043E\u0436\u0435\u0442 \u0438\u0437\u043C\u0435\u043D\u0438\u0442\u044C \u044F\u0437\u044B\u043A \u0438\u043D\u0442\u0435\u0440\u0444\u0435\u0439\u0441\u0430:",
    L"\u0412\u044B\u0431\u043E\u0440 \u044F\u0437\u044B\u043A\u0430 \u0438\u043D\u0442\u0435\u0440\u0444\u0435\u0439\u0441\u0430 \u0437\u0430\u0431\u043B\u043E\u043A\u0438\u0440\u043E\u0432\u0430\u043D \u0433\u0440\u0443\u043F\u043F\u043E\u0432\u043E\u0439 \u043F\u043E\u043B\u0438\u0442\u0438\u043A\u043E\u0439.",
    L"&\u0412\u044B\u0431\u0435\u0440\u0438\u0442\u0435 \u044F\u0437\u044B\u043A \u0438\u043D\u0442\u0435\u0440\u0444\u0435\u0439\u0441\u0430:",
    L"&\u0427\u0430\u0441\u0442\u044C \u0442\u0435\u043A\u0441\u0442\u0430 \u043D\u0435 \u043F\u0435\u0440\u0435\u0432\u0435\u0434\u0435\u043D\u0430 \u043D\u0430 \u0432\u044B\u0431\u0440\u0430\u043D\u043D\u044B\u0439 \u044F\u0437\u044B\u043A. \u0412\u044B\u0431\u0435\u0440\u0438\u0442\u0435 \u0434\u0440\u0443\u0433\u043E\u0439 \u044F\u0437\u044B\u043A, \u043A\u043E\u0442\u043E\u0440\u044B\u0439 Windows \u0431\u0443\u0434\u0435\u0442 \u0438\u0441\u043F\u043E\u043B\u044C\u0437\u043E\u0432\u0430\u0442\u044C \u0434\u043B\u044F \u043E\u0442\u043E\u0431\u0440\u0430\u0436\u0435\u043D\u0438\u044F \u044D\u0442\u043E\u0433\u043E \u0442\u0435\u043A\u0441\u0442\u0430:",
    L"\u042D\u0442\u043E\u0442 \u044F\u0437\u044B\u043A \u043F\u0435\u0440\u0435\u0432\u0435\u0434\u0435\u043D \u043B\u0438\u0448\u044C \u0447\u0430\u0441\u0442\u0438\u0447\u043D\u043E, \u0438 \u0447\u0430\u0441\u0442\u044C \u0442\u0435\u043A\u0441\u0442\u0430 \u043C\u043E\u0436\u0435\u0442 \u043E\u0442\u043E\u0431\u0440\u0430\u0436\u0430\u0442\u044C\u0441\u044F \u043D\u0430:",
    L"&\u042D\u0442\u043E\u0442 \u044F\u0437\u044B\u043A \u0442\u0430\u043A\u0436\u0435 \u043F\u0435\u0440\u0435\u0432\u0435\u0434\u0435\u043D \u043B\u0438\u0448\u044C \u0447\u0430\u0441\u0442\u0438\u0447\u043D\u043E. \u0412\u044B\u0431\u0435\u0440\u0438\u0442\u0435 \u0442\u0440\u0435\u0442\u0438\u0439 \u044F\u0437\u044B\u043A, \u043A\u043E\u0442\u043E\u0440\u044B\u0439 Windows \u0431\u0443\u0434\u0435\u0442 \u0438\u0441\u043F\u043E\u043B\u044C\u0437\u043E\u0432\u0430\u0442\u044C \u0434\u043B\u044F \u043E\u0442\u043E\u0431\u0440\u0430\u0436\u0435\u043D\u0438\u044F \u043E\u0441\u0442\u0430\u0432\u0448\u0435\u0433\u043E\u0441\u044F \u0442\u0435\u043A\u0441\u0442\u0430:",
    L"\u042D\u0442\u043E\u0442 \u044F\u0437\u044B\u043A \u0442\u0430\u043A\u0436\u0435 \u043F\u0435\u0440\u0435\u0432\u0435\u0434\u0435\u043D \u043B\u0438\u0448\u044C \u0447\u0430\u0441\u0442\u0438\u0447\u043D\u043E, \u0438 \u0447\u0430\u0441\u0442\u044C \u0442\u0435\u043A\u0441\u0442\u0430 \u043C\u043E\u0436\u0435\u0442 \u043E\u0442\u043E\u0431\u0440\u0430\u0436\u0430\u0442\u044C\u0441\u044F \u043D\u0430: ",
    L"\u042D\u043A\u0440\u0430\u043D \u043F\u0440\u0438\u0432\u0435\u0442\u0441\u0442\u0432\u0438\u044F \u0438 \u043D\u043E\u0432\u044B\u0435 \u0443\u0447\u0435\u0442\u043D\u044B\u0435 \u0437\u0430\u043F\u0438\u0441\u0438",
    L"\u041F\u0440\u043E\u0441\u043C\u043E\u0442\u0440\u0438\u0442\u0435 \u0438 \u0441\u043A\u043E\u043F\u0438\u0440\u0443\u0439\u0442\u0435 \u0441\u0432\u043E\u0438 \u0440\u0435\u0433\u0438\u043E\u043D\u0430\u043B\u044C\u043D\u044B\u0435 \u043F\u0430\u0440\u0430\u043C\u0435\u0442\u0440\u044B \u043D\u0430 \u044D\u043A\u0440\u0430\u043D \u043F\u0440\u0438\u0432\u0435\u0442\u0441\u0442\u0432\u0438\u044F, \u0441\u0438\u0441\u0442\u0435\u043C\u043D\u044B\u0435 \u0443\u0447\u0435\u0442\u043D\u044B\u0435 \u0437\u0430\u043F\u0438\u0441\u0438 \u0438 \u043D\u043E\u0432\u044B\u0435 \u0443\u0447\u0435\u0442\u043D\u044B\u0435 \u0437\u0430\u043F\u0438\u0441\u0438.",
    L"&\u041A\u043E\u043F\u0438\u0440\u043E\u0432\u0430\u0442\u044C \u043F\u0430\u0440\u0430\u043C\u0435\u0442\u0440\u044B...",
    L"\u042F\u0437\u044B\u043A \u043F\u0440\u043E\u0433\u0440\u0430\u043C\u043C, \u043D\u0435 \u043F\u043E\u0434\u0434\u0435\u0440\u0436\u0438\u0432\u0430\u044E\u0449\u0438\u0445 \u042E\u043D\u0438\u043A\u043E\u0434",
    L"\u042D\u0442\u043E\u0442 \u043F\u0430\u0440\u0430\u043C\u0435\u0442\u0440 (\u044F\u0437\u044B\u043A \u0441\u0438\u0441\u0442\u0435\u043C\u044B) \u043E\u043F\u0440\u0435\u0434\u0435\u043B\u044F\u0435\u0442 \u044F\u0437\u044B\u043A, \u0438\u0441\u043F\u043E\u043B\u044C\u0437\u0443\u0435\u043C\u044B\u0439 \u043F\u0440\u0438 \u043E\u0442\u043E\u0431\u0440\u0430\u0436\u0435\u043D\u0438\u0438 \u0442\u0435\u043A\u0441\u0442\u0430 \u0432 \u043F\u0440\u043E\u0433\u0440\u0430\u043C\u043C\u0430\u0445, \u043D\u0435 \u043F\u043E\u0434\u0434\u0435\u0440\u0436\u0438\u0432\u0430\u044E\u0449\u0438\u0445 \u042E\u043D\u0438\u043A\u043E\u0434.",
    L"\u0422\u0435\u043A\u0443\u0449\u0438\u0439 \u044F\u0437\u044B\u043A \u043F\u0440\u043E\u0433\u0440\u0430\u043C\u043C, \u043D\u0435 \u043F\u043E\u0434\u0434\u0435\u0440\u0436\u0438\u0432\u0430\u044E\u0449\u0438\u0445 \u042E\u043D\u0438\u043A\u043E\u0434:",
    nullptr,
    L"&\u042F\u0437\u044B\u043A \u0441\u0438\u0441\u0442\u0435\u043C\u044B...",
    L"\u041F\u0440\u0438\u043C\u0435\u0440",
    L"\u041F\u043E\u043B\u043E\u0436\u0438\u0442\u0435\u043B\u044C\u043D\u044B\u0435:",
    L"\u041E\u0442\u0440\u0438\u0446\u0430\u0442\u0435\u043B\u044C\u043D\u044B\u0435:",
    L"&\u0414\u0435\u0441\u044F\u0442\u0438\u0447\u043D\u044B\u0439 \u0440\u0430\u0437\u0434\u0435\u043B\u0438\u0442\u0435\u043B\u044C:",
    L"&\u0427\u0438\u0441\u043B\u043E \u0434\u0435\u0441\u044F\u0442\u0438\u0447\u043D\u044B\u0445 \u0437\u043D\u0430\u043A\u043E\u0432:",
    L"\u0420\u0430\u0437\u0434\u0435\u043B\u0438\u0442\u0435\u043B\u044C &\u0433\u0440\u0443\u043F\u043F:",
    L"\u0413&\u0440\u0443\u043F\u043F\u0438\u0440\u043E\u0432\u043A\u0430:",
    L"\u0417\u043D\u0430\u043A &\u043C\u0438\u043D\u0443\u0441\u0430:",
    L"\u0424\u043E\u0440\u043C\u0430\u0442 &\u043E\u0442\u0440\u0438\u0446\u0430\u0442\u0435\u043B\u044C\u043D\u044B\u0445 \u0447\u0438\u0441\u0435\u043B:",
    L"\u041E\u0442\u043E\u0431\u0440\u0430\u0436\u0430\u0442\u044C &\u043D\u0443\u043B\u0438 \u0432 \u043D\u0430\u0447\u0430\u043B\u0435:",
    L"\u0420\u0430\u0437\u0434\u0435\u043B\u0438\u0442\u0435&\u043B\u044C \u0441\u043F\u0438\u0441\u043A\u0430:",
    L"&\u0421\u0438\u0441\u0442\u0435\u043C\u0430 \u043C\u0435\u0440:",
    L"\u0421\u0442\u0430\u043D\u0434\u0430\u0440\u0442\u043D&\u044B\u0435 \u0446\u0438\u0444\u0440\u044B:",
    L"&\u0418\u0441\u043F\u043E\u043B\u044C\u0437\u043E\u0432\u0430\u0442\u044C \u043D\u0430\u0446\u0438\u043E\u043D\u0430\u043B\u044C\u043D\u044B\u0435 \u0446\u0438\u0444\u0440\u044B:",
    L"\u041D\u0430\u0436\u043C\u0438\u0442\u0435 \u043A\u043D\u043E\u043F\u043A\u0443 \u0421\u0431\u0440\u043E\u0441, \u0447\u0442\u043E\u0431\u044B \u0432\u043E\u0441\u0441\u0442\u0430\u043D\u043E\u0432\u0438\u0442\u044C \u0441\u0438\u0441\u0442\u0435\u043C\u043D\u044B\u0435 \u043F\u0430\u0440\u0430\u043C\u0435\u0442\u0440\u044B \u043F\u043E \u0443\u043C\u043E\u043B\u0447\u0430\u043D\u0438\u044E \u0434\u043B\u044F \u0447\u0438\u0441\u0435\u043B, \u0432\u0430\u043B\u044E\u0442\u044B, \u0432\u0440\u0435\u043C\u0435\u043D\u0438 \u0438 \u0434\u0430\u0442\u044B.",
    L"\u0421&\u0431\u0440\u043E\u0441",
    L"&\u0421\u0438\u043C\u0432\u043E\u043B \u0432\u0430\u043B\u044E\u0442\u044B:",
    L"&\u041F\u043E\u043B\u043E\u0436\u0438\u0442\u0435\u043B\u044C\u043D\u044B\u0439 \u0444\u043E\u0440\u043C\u0430\u0442 \u0432\u0430\u043B\u044E\u0442\u044B:",
    L"\u041E\u0442\u0440\u0438\u0446\u0430\u0442\u0435\u043B\u044C\u043D\u044B\u0439 \u0444\u043E\u0440\u043C\u0430\u0442 \u0432\u0430\u043B\u044E\u0442&\u044B:",
    L"\u0427\u0438\u0441\u043B\u043E \u0434\u0435\u0441\u044F\u0442\u0438\u0447\u043D\u044B\u0445 \u0437&\u043D\u0430\u043A\u043E\u0432:",
    L"\u0420\u0430\u0437\u0434\u0435\u043B\u0438\u0442\u0435\u043B\u044C \u0433&\u0440\u0443\u043F\u043F:",
    L"\u0413\u0440\u0443\u043F\u043F\u0438\u0440\u043E\u0432&\u043A\u0430:",
    L"\u0424\u043E\u0440\u043C\u0430\u0442\u044B \u0432\u0440\u0435\u043C\u0435\u043D\u0438",
    L"&\u041A\u0440\u0430\u0442\u043A\u043E\u0435 \u0432\u0440\u0435\u043C\u044F:",
    L"&\u041F\u043E\u043B\u043D\u043E\u0435 \u0432\u0440\u0435\u043C\u044F:",
    L"&\u0421\u0438\u043C\u0432\u043E\u043B AM:",
    L"\u0421\u0438\u043C&\u0432\u043E\u043B PM:",
    L"\u0417\u043D\u0430\u0447\u0435\u043D\u0438\u0435 \u043E\u0431\u043E\u0437\u043D\u0430\u0447\u0435\u043D\u0438\u0439:\n\nh = \u0447\u0430\u0441   m = \u043C\u0438\u043D\u0443\u0442\u0430\ns = \u0441\u0435\u043A\u0443\u043D\u0434\u0430 (\u0442\u043E\u043B\u044C\u043A\u043E \u043F\u043E\u043B\u043D\u043E\u0435 \u0432\u0440\u0435\u043C\u044F)\ntt = AM \u0438\u043B\u0438 PM\n\nh/H = 12/24 \u0447\u0430\u0441\u0430\n\nhh, mm, ss = \u043E\u0442\u043E\u0431\u0440\u0430\u0436\u0430\u0442\u044C \u043D\u0443\u043B\u044C \u0432 \u043D\u0430\u0447\u0430\u043B\u0435\nh, m, s = \u043D\u0435 \u043E\u0442\u043E\u0431\u0440\u0430\u0436\u0430\u0442\u044C \u043D\u0443\u043B\u044C \u0432 \u043D\u0430\u0447\u0430\u043B\u0435",
    L"\u0424\u043E\u0440\u043C\u0430\u0442\u044B \u0434\u0430\u0442\u044B",
    L"\u0417\u043D\u0430\u0447\u0435\u043D\u0438\u0435 \u043E\u0431\u043E\u0437\u043D\u0430\u0447\u0435\u043D\u0438\u0439:\nd, dd = \u0434\u0435\u043D\u044C;  ddd, dddd = \u0434\u0435\u043D\u044C \u043D\u0435\u0434\u0435\u043B\u0438;  M = \u043C\u0435\u0441\u044F\u0446;  y = \u0433\u043E\u0434",
    L"\u041A\u0430\u043B\u0435\u043D\u0434\u0430\u0440\u044C",
    L"\u0415\u0441\u043B\u0438 \u0432\u0432\u0435\u0434\u0435\u043D \u0433\u043E\u0434 \u0438\u0437 \u0434\u0432\u0443\u0445 \u0446\u0438\u0444\u0440, \u0441\u0447\u0438\u0442\u0430\u0442\u044C \u0435\u0433\u043E \u0433\u043E\u0434\u043E\u043C &\u043C\u0435\u0436\u0434\u0443:",
    L"\u0438",
    L"\u041F\u0435\u0440\u0432\u044B\u0439 \u0434\u0435&\u043D\u044C \u043D\u0435\u0434\u0435\u043B\u0438:",
    L"&\u0422\u0438\u043F \u043A\u0430\u043B\u0435\u043D\u0434\u0430\u0440\u044F:",
    L"\u041F\u043E\u043F\u0440\u0430\u0432\u043A\u0430 \u0434\u0430\u0442\u044B &\u0445\u0438\u0434\u0436\u0440\u044B:",
    L"\u041C\u043E\u0436\u043D\u043E \u0443\u043F\u0440\u0430\u0432\u043B\u044F\u0442\u044C \u0441\u043F\u043E\u0441\u043E\u0431\u043E\u043C \u0441\u043E\u0440\u0442\u0438\u0440\u043E\u0432\u043A\u0438 \u0441\u0438\u043C\u0432\u043E\u043B\u043E\u0432, \u0441\u043B\u043E\u0432, \u0444\u0430\u0439\u043B\u043E\u0432 \u0438 \u043F\u0430\u043F\u043E\u043A \u043D\u0435\u043A\u043E\u0442\u043E\u0440\u044B\u043C\u0438 \u043F\u0440\u043E\u0433\u0440\u0430\u043C\u043C\u0430\u043C\u0438.",
    L"&\u0412\u044B\u0431\u0435\u0440\u0438\u0442\u0435 \u043C\u0435\u0442\u043E\u0434 \u0441\u043E\u0440\u0442\u0438\u0440\u043E\u0432\u043A\u0438:",
    L"\u041D\u0435\u043A\u043E\u0442\u043E\u0440\u044B\u0435 \u043F\u0440\u043E\u0433\u0440\u0430\u043C\u043C\u044B, \u0432\u043A\u043B\u044E\u0447\u0430\u044F Windows, \u043C\u043E\u0433\u0443\u0442 \u043F\u0440\u0435\u0434\u043E\u0441\u0442\u0430\u0432\u043B\u044F\u0442\u044C \u0434\u043E\u043F\u043E\u043B\u043D\u0438\u0442\u0435\u043B\u044C\u043D\u043E\u0435 \u0441\u043E\u0434\u0435\u0440\u0436\u0438\u043C\u043E\u0435 \u0434\u043B\u044F \u043E\u043F\u0440\u0435\u0434\u0435\u043B\u0435\u043D\u043D\u043E\u0433\u043E \u0440\u0430\u0441\u043F\u043E\u043B\u043E\u0436\u0435\u043D\u0438\u044F. \u041D\u0435\u043A\u043E\u0442\u043E\u0440\u044B\u0435 \u0441\u043B\u0443\u0436\u0431\u044B \u043F\u0440\u0435\u0434\u043E\u0441\u0442\u0430\u0432\u043B\u044F\u044E\u0442 \u043C\u0435\u0441\u0442\u043D\u044B\u0435 \u0441\u0432\u0435\u0434\u0435\u043D\u0438\u044F, \u043D\u0430\u043F\u0440\u0438\u043C\u0435\u0440 \u043D\u043E\u0432\u043E\u0441\u0442\u0438 \u0438 \u043F\u043E\u0433\u043E\u0434\u0443.",
    L"&\u0422\u0435\u043A\u0443\u0449\u0435\u0435 \u0440\u0430\u0441\u043F\u043E\u043B\u043E\u0436\u0435\u043D\u0438\u0435:",
    L"\u0421\u043C. \u0442\u0430\u043A\u0436\u0435",
    L"&\u041F\u0430\u0440\u0430\u043C\u0435\u0442\u0440\u044B \u0442\u0435\u043A\u0443\u0449\u0435\u0433\u043E \u043F\u043E\u043B\u044C\u0437\u043E\u0432\u0430\u0442\u0435\u043B\u044F, \u044D\u043A\u0440\u0430\u043D\u0430 \u043F\u0440\u0438\u0432\u0435\u0442\u0441\u0442\u0432\u0438\u044F (\u0441\u0438\u0441\u0442\u0435\u043C\u043D\u044B\u0445 \u0443\u0447\u0435\u0442\u043D\u044B\u0445 \u0437\u0430\u043F\u0438\u0441\u0435\u0439) \u0438 \u043D\u043E\u0432\u044B\u0445 \u0443\u0447\u0435\u0442\u043D\u044B\u0445 \u0437\u0430\u043F\u0438\u0441\u0435\u0439 \u043F\u043E\u043A\u0430\u0437\u0430\u043D\u044B \u043D\u0438\u0436\u0435.",
    L"* \u041D\u0430\u0441\u0442\u0440\u0430\u0438\u0432\u0430\u0435\u043C\u044B\u0439 \u044F\u0437\u044B\u043A",
    L"\u0421\u043A\u043E\u043F\u0438\u0440\u043E\u0432\u0430\u0442\u044C \u0442\u0435\u043A\u0443\u0449\u0438\u0435 \u043F\u0430\u0440\u0430\u043C\u0435\u0442\u0440\u044B \u0432:",
    L"&\u042D\u043A\u0440\u0430\u043D \u043F\u0440\u0438\u0432\u0435\u0442\u0441\u0442\u0432\u0438\u044F \u0438 \u0441\u0438\u0441\u0442\u0435\u043C\u043D\u044B\u0435 \u0443\u0447\u0435\u0442\u043D\u044B\u0435 \u0437\u0430\u043F\u0438\u0441\u0438",
    L"&\u041D\u043E\u0432\u044B\u0435 \u0443\u0447\u0435\u0442\u043D\u044B\u0435 \u0437\u0430\u043F\u0438\u0441\u0438",
    L"\u042F\u0437\u044B\u043A \u0438\u043D\u0442\u0435\u0440\u0444\u0435\u0439\u0441\u0430 \u043D\u043E\u0432\u044B\u0445 \u0443\u0447\u0435\u0442\u043D\u044B\u0445 \u0437\u0430\u043F\u0438\u0441\u0435\u0439 \u0432 \u043D\u0430\u0441\u0442\u043E\u044F\u0449\u0435\u0435 \u0432\u0440\u0435\u043C\u044F \u043D\u0430\u0441\u043B\u0435\u0434\u0443\u0435\u0442\u0441\u044F \u043E\u0442 \u044F\u0437\u044B\u043A\u0430 \u0438\u043D\u0442\u0435\u0440\u0444\u0435\u0439\u0441\u0430 \u044D\u043A\u0440\u0430\u043D\u0430 \u043F\u0440\u0438\u0432\u0435\u0442\u0441\u0442\u0432\u0438\u044F.",
    nullptr,
    L"\u041E\u0442\u043C\u0435\u043D\u0430",
    L"\u0412\u044B\u0431\u0435\u0440\u0438\u0442\u0435 \u044F\u0437\u044B\u043A (\u044F\u0437\u044B\u043A \u0441\u0438\u0441\u0442\u0435\u043C\u044B), \u0438\u0441\u043F\u043E\u043B\u044C\u0437\u0443\u0435\u043C\u044B\u0439 \u043F\u0440\u0438 \u043E\u0442\u043E\u0431\u0440\u0430\u0436\u0435\u043D\u0438\u0438 \u0442\u0435\u043A\u0441\u0442\u0430 \u0432 \u043F\u0440\u043E\u0433\u0440\u0430\u043C\u043C\u0430\u0445, \u043D\u0435 \u043F\u043E\u0434\u0434\u0435\u0440\u0436\u0438\u0432\u0430\u044E\u0449\u0438\u0445 \u042E\u043D\u0438\u043A\u043E\u0434. \u042D\u0442\u043E\u0442 \u043F\u0430\u0440\u0430\u043C\u0435\u0442\u0440 \u0432\u043B\u0438\u044F\u0435\u0442 \u043D\u0430 \u0432\u0441\u0435 \u0443\u0447\u0435\u0442\u043D\u044B\u0435 \u0437\u0430\u043F\u0438\u0441\u0438 \u043A\u043E\u043C\u043F\u044C\u044E\u0442\u0435\u0440\u0430.",
    L"&\u0422\u0435\u043A\u0443\u0449\u0438\u0439 \u044F\u0437\u044B\u043A \u0441\u0438\u0441\u0442\u0435\u043C\u044B:",
    L"<A>\u0418\u0437\u043C\u0435\u043D\u0438\u0442\u044C \u043C\u0435\u0442\u043E\u0434 \u0441\u043E\u0440\u0442\u0438\u0440\u043E\u0432\u043A\u0438</A>",
    L"<A>\u0427\u0442\u043E \u043E\u0437\u043D\u0430\u0447\u0430\u0435\u0442 \u043E\u0431\u043E\u0437\u043D\u0430\u0447\u0435\u043D\u0438\u0435?</A>",
    L"<A>\u041F\u043E\u0434\u0440\u043E\u0431\u043D\u0435\u0435 \u0432 \u0418\u043D\u0442\u0435\u0440\u043D\u0435\u0442\u0435 \u043E\u0431 \u0438\u0437\u043C\u0435\u043D\u0435\u043D\u0438\u0438 \u044F\u0437\u044B\u043A\u043E\u0432 \u0438 \u0440\u0435\u0433\u0438\u043E\u043D\u0430\u043B\u044C\u043D\u044B\u0445 \u0444\u043E\u0440\u043C\u0430\u0442\u043E\u0432</A>",
    L"<A>\u041A\u0430\u043A \u0438\u0437\u043C\u0435\u043D\u0438\u0442\u044C \u0440\u0430\u0441\u043A\u043B\u0430\u0434\u043A\u0443 \u043A\u043B\u0430\u0432\u0438\u0430\u0442\u0443\u0440\u044B \u0434\u043B\u044F \u044D\u043A\u0440\u0430\u043D\u0430 \u043F\u0440\u0438\u0432\u0435\u0442\u0441\u0442\u0432\u0438\u044F?</A>",
    L"<A>\u041A\u0430\u043A \u0443\u0441\u0442\u0430\u043D\u043E\u0432\u0438\u0442\u044C \u0434\u043E\u043F\u043E\u043B\u043D\u0438\u0442\u0435\u043B\u044C\u043D\u044B\u0435 \u044F\u0437\u044B\u043A\u0438?</A>",
    L"<A>\u041F\u043E\u0434\u0440\u043E\u0431\u043D\u0435\u0435 \u043E\u0431 \u044D\u0442\u0438\u0445 \u0443\u0447\u0435\u0442\u043D\u044B\u0445 \u0437\u0430\u043F\u0438\u0441\u044F\u0445</A>",
    L"<A>\u0427\u0442\u043E \u0442\u0430\u043A\u043E\u0435 \u044F\u0437\u044B\u043A \u0441\u0438\u0441\u0442\u0435\u043C\u044B?</A>",
    L"<A>\u0420\u0430\u0441\u043F\u043E\u043B\u043E\u0436\u0435\u043D\u0438\u0435 \u043F\u043E \u0443\u043C\u043E\u043B\u0447\u0430\u043D\u0438\u044E</A>",
};
static const wchar_t* const kTitleTr_RU[11] = {
    L"\u0424\u043E\u0440\u043C\u0430\u0442\u044B",
    L"\u041A\u043B\u0430\u0432\u0438\u0430\u0442\u0443\u0440\u044B \u0438 \u044F\u0437\u044B\u043A\u0438",
    L"\u0410\u0434\u043C\u0438\u043D\u0438\u0441\u0442\u0440\u0438\u0440\u043E\u0432\u0430\u043D\u0438\u0435",
    L"\u0427\u0438\u0441\u043B\u0430",
    L"\u0412\u0430\u043B\u044E\u0442\u0430",
    L"\u0412\u0440\u0435\u043C\u044F",
    L"\u0414\u0430\u0442\u0430",
    L"\u0421\u043E\u0440\u0442\u0438\u0440\u043E\u0432\u043A\u0430",
    L"\u0420\u0430\u0441\u043F\u043E\u043B\u043E\u0436\u0435\u043D\u0438\u0435",
    L"\u041F\u0430\u0440\u0430\u043C\u0435\u0442\u0440\u044B \u044D\u043A\u0440\u0430\u043D\u0430 \u043F\u0440\u0438\u0432\u0435\u0442\u0441\u0442\u0432\u0438\u044F \u0438 \u043D\u043E\u0432\u044B\u0445 \u0443\u0447\u0435\u0442\u043D\u044B\u0445 \u0437\u0430\u043F\u0438\u0441\u0435\u0439",
    L"\u042F\u0437\u044B\u043A \u0438 \u0440\u0435\u0433\u0438\u043E\u043D\u0430\u043B\u044C\u043D\u044B\u0435 \u0441\u0442\u0430\u043D\u0434\u0430\u0440\u0442\u044B",
};
// ================= T\u00DCRK\u00C7E (tr-TR) =================
static const wchar_t* const kStrTr_TR[66] = {
    L"B\u00F6lge ve Dil",
    L"Dillerin, say\u0131lar\u0131n, saatlerin ve tarihlerin g\u00F6r\u00FCnt\u00FCye ili\u015Fkin ayarlar\u0131n\u0131 yap\u0131land\u0131r\u0131n.",
    L"Bi\u00E7imi De\u011Fi\u015Ftir",
    L"Bir veya birden fazla b\u00F6lge ayar\u0131 ge\u00E7ersiz. L\u00FCtfen sorunun d\u00FCzeltilmesi i\u00E7in \u00F6zel ayarlar\u0131 g\u00F6zden ge\u00E7irip d\u00FCzeltin.",
    nullptr,
    nullptr,
    L"Metrik",
    nullptr,
    L"Bu alana girdi\u011Finiz karakterlerden bir veya birka\u00E7\u0131 ge\u00E7ersiz. L\u00FCtfen ba\u015Fka karakterler kullanmay\u0131 deneyin.",
    L"%s i\u00E7in girdi\u011Finiz karakterlerden bir veya birka\u00E7\u0131 ge\u00E7ersiz.  L\u00FCtfen farkl\u0131 bir karakter kullanmay\u0131 veya bo\u015Fluk girmeyi deneyin.",
    L"Ondal\u0131k simgesi",
    L"Eksi i\u015Fareti",
    L"Basamak grupland\u0131rma simgesi",
    L"\u00D6\u00D6 simgesi",
    L"\u00D6S simgesi",
    L"Para birimi simgesi",
    L"Para birimi ondal\u0131k simgesi",
    L"Para birimi basamak grupland\u0131rma simgesi",
    L"%s bi\u00E7imi i\u00E7in girdi\u011Finiz karakterlerden bir veya birka\u00E7\u0131 ge\u00E7ersiz. L\u00FCtfen ba\u015Fka karakterler kullanmay\u0131 deneyin.",
    L"Uzun saat",
    L"K\u0131sa tarih",
    L"Uzun tarih",
    L"Bu alandaki de\u011Fer 99 ile 9999 aras\u0131nda bir say\u0131 olmal\u0131d\u0131r. L\u00FCtfen ba\u015Fka bir say\u0131 kullanmay\u0131 deneyin.",
    L"K\u0131sa saat",
    L"&Bi\u00E7im:",
    L"&Bi\u00E7im: (* \u00F6zel b\u00F6lge ayar\u0131)",
    nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
    L"Sistem b\u00F6lge ayar\u0131 de\u011Fi\u015Ftirildi. De\u011Fi\u015Fikliklerin etkili olmas\u0131 i\u00E7in Windows'u yeniden ba\u015Flatmal\u0131s\u0131n\u0131z.",
    L"B\u00F6lge ayarlar\u0131n\u0131 de\u011Fi\u015Ftir",
    L"Se\u00E7ilen dil y\u00FCklenemedi. L\u00FCtfen sistem y\u00F6neticinize ba\u015Fvurun.",
    L"Sistem g\u00F6r\u00FCnt\u00FC dili de\u011Fi\u015Ftirildi. De\u011Fi\u015Fikliklerin etkili olmas\u0131 i\u00E7in Windows'u yeniden ba\u015Flatmal\u0131s\u0131n\u0131z.",
    L"Ge\u00E7erli bi\u00E7imin t\u00FCm \u00F6zelle\u015Ftirmelerini temizlemek istiyor musunuz?",
    L"Dil ve b\u00F6lge de\u011Fi\u015Fikliklerini uygulamak istiyor musunuz?",
    L"\u015Eimdi yeniden ba\u015Flat",
    L"\u0130ptal",
    L"Yeniden ba\u015Flatmadan \u00F6nce \u00E7al\u0131\u015Fman\u0131z\u0131 kaydedin ve t\u00FCm a\u00E7\u0131k programlar\u0131 kapat\u0131n.",
    L"Sistem b\u00F6lge ayar\u0131",
    L"%s klavye d\u00FCzeni d\u00FCzg\u00FCn \u015Fekilde y\u00FCklenemedi.",
    L"\u0130spanyolca (\u0130spanya)",
    L"G\u00F6r\u00FCnt\u00FC dili de\u011Fi\u015Fikli\u011Finin etkili olmas\u0131 i\u00E7in oturumunuzu kapat\u0131p yeniden a\u00E7mal\u0131s\u0131n\u0131z",
    L"Oturumu kapatmadan \u00F6nce \u00E7al\u0131\u015Fman\u0131z\u0131 kaydedin ve t\u00FCm a\u00E7\u0131k programlar\u0131 kapat\u0131n.",
    L"\u015Eimdi oturumu kapat",
    L"\u0130ptal",
    L"G\u00F6r\u00FCnt\u00FC dilini de\u011Fi\u015Ftir",
    L"Di\u011Fer sistem de\u011Fi\u015Fikliklerini yapmadan \u00F6nce de\u011Fi\u015Fiklikleri uygulaman\u0131z\u0131 \u00F6neririz, b\u00F6ylece bilgisayar\u0131n\u0131z bunlar\u0131 yans\u0131tabilir.",
    L"Uygula",
    L"\u0130ptal",
    L"\u0130\u015Flem tamamlanamad\u0131",
    L"Ge\u00E7erli kullan\u0131c\u0131",
    L"Kar\u015F\u0131lama ekran\u0131",
    L"Yeni hesaplar",
    L"G\u00F6r\u00FCnt\u00FC dili:",
    L"Giri\u015F dili:",
    L"Bi\u00E7im:",
    L"Konum:",
    L"Parametre okunamad\u0131",
    L"Ba\u011Flam",
    L"Asla",
    L"Yerel",
};
static const wchar_t* const kDlgTr_TR[92] = {
    L"Tarih ve saat bi\u00E7imleri",
    L"&K\u0131sa tarih:",
    L"&Uzun tarih:",
    L"K\u0131sa &saat:",
    L"U&zun saat:",
    L"Haftan\u0131n ilk g&\u00FCn\u00FC:",
    L"\u00D6rnekler",
    L"K\u0131sa tarih:",
    L"Uzun tarih:",
    L"K\u0131sa saat:",
    L"Uzun saat:",
    L"Ek &Ayarlar...",
    L"Klavyeler ve di\u011Fer giri\u015F dilleri",
    L"Klavyenizi veya giri\u015F dilinizi de\u011Fi\u015Ftirmek i\u00E7in Klavyeleri De\u011Fi\u015Ftir'e t\u0131klay\u0131n.",
    L"&Klavyeleri De\u011Fi\u015Ftir...",
    L"G\u00F6r\u00FCnt\u00FC dili",
    L"Windows'un metin g\u00F6r\u00FCnt\u00FClemek i\u00E7in ve desteklendi\u011Fi durumlarda konu\u015Fma ve el yaz\u0131s\u0131n\u0131 tan\u0131mak i\u00E7in kullanabilece\u011Fi dilleri y\u00FCkleyin veya kald\u0131r\u0131n.",
    L"Dilleri &y\u00FCkleyin veya kald\u0131r\u0131n...",
    L"Konuk g\u00F6r\u00FCnt\u00FC dilini de\u011Fi\u015Ftiremez:",
    L"G\u00F6r\u00FCnt\u00FC dili se\u00E7imi grup ilkesi taraf\u0131ndan kilitlendi.",
    L"G\u00F6r\u00FCnt\u00FC dili &se\u00E7in:",
    L"Metinlerin bir k\u0131sm\u0131 se\u00E7ili dile &yerelle\u015Ftirilmedi. Windows'un bu metni g\u00F6r\u00FCnt\u00FClemek i\u00E7in kullanaca\u011F\u0131 ba\u015Fka bir dil se\u00E7in:",
    L"Bu dil yaln\u0131zca k\u0131smen yerelle\u015Ftirildi ve metinlerin bir k\u0131sm\u0131 \u015Fu dilde g\u00F6r\u00FCnt\u00FClenebilir:",
    L"Bu dil de &yaln\u0131zca k\u0131smen yerelle\u015Ftirildi. Windows'un kalan metni g\u00F6r\u00FCnt\u00FClemek i\u00E7in kullanaca\u011F\u0131 \u00FC\u00E7\u00FCnc\u00FC bir dil se\u00E7in:",
    L"Bu dil de yaln\u0131zca k\u0131smen yerelle\u015Ftirildi ve metinlerin bir k\u0131sm\u0131 \u015Fu dilde g\u00F6r\u00FCnt\u00FClenebilir: ",
    L"Kar\u015F\u0131lama ekran\u0131 ve yeni kullan\u0131c\u0131 hesaplar\u0131",
    L"B\u00F6lge ayarlar\u0131n\u0131z\u0131 g\u00F6r\u00FCnt\u00FCleyin ve kar\u015F\u0131lama ekran\u0131na, sistem hesaplar\u0131na ve yeni kullan\u0131c\u0131 hesaplar\u0131na kopyalay\u0131n.",
    L"Ayarlar\u0131 &kopyala...",
    L"Unicode olmayan programlar\u0131n dili",
    L"Bu ayar (sistem b\u00F6lge ayar\u0131), Unicode'u desteklemeyen programlarda metin g\u00F6r\u00FCnt\u00FClenirken kullan\u0131lacak dili denetler.",
    L"Unicode olmayan programlar i\u00E7in ge\u00E7erli dil:",
    nullptr,
    L"Sistem &b\u00F6lge ayar\u0131...",
    L"\u00D6rnek",
    L"Olumlu:",
    L"Olumsuz:",
    L"&Ondal\u0131k simgesi:",
    L"&Ondal\u0131k basamak say\u0131s\u0131:",
    L"Basamak &grupland\u0131rma simgesi:",
    L"Basamak g&rupland\u0131rma:",
    L"&Eksi i\u015Fareti simgesi:",
    L"Negatif say\u0131 &bi\u00E7imi:",
    L"Ba\u015Fta &s\u0131f\u0131r g\u00F6r\u00FCnt\u00FCle:",
    L"Liste a&y\u0131rac\u0131:",
    L"\u00D6l\u00E7&\u00FCm sistemi:",
    L"Standart &rakamlar:",
    L"Ye&rel rakamlar\u0131 kullan:",
    L"Say\u0131lar, para birimi, saat ve tarih i\u00E7in sistem varsay\u0131lan ayarlar\u0131n\u0131 geri y\u00FCklemek \u00FCzere S\u0131f\u0131rla'ya t\u0131klay\u0131n.",
    L"S\u0131f\u0131&rla",
    L"Para birimi &simgesi:",
    L"&Olumlu para birimi bi\u00E7imi:",
    L"Olumsuz para birimi bi\u00E7im&i:",
    L"Ondal\u0131k basamak sa&y\u0131s\u0131:",
    L"Basamak grupland\u0131rma s&imgesi:",
    L"Basamak grupland\u0131r&ma:",
    L"Saat bi\u00E7imleri",
    L"&K\u0131sa saat:",
    L"&Uzun saat:",
    L"\u00D6\u00D6 &simgesi:",
    L"\u00D6&S simgesi:",
    L"G\u00F6sterim anlam\u0131:\n\nh = saat   m = dakika\ns = saniye (yaln\u0131zca uzun saat)\ntt = \u00D6\u00D6 veya \u00D6S\n\nh/H = 12/24 saat\n\nhh, mm, ss = ba\u015Fta s\u0131f\u0131r g\u00F6r\u00FCnt\u00FCle\nh, m, s = ba\u015Fta s\u0131f\u0131r g\u00F6r\u00FCnt\u00FCleme",
    L"Tarih bi\u00E7imleri",
    L"G\u00F6sterim anlam\u0131:\nd, dd = g\u00FCn;  ddd, dddd = haftan\u0131n g\u00FCn\u00FC;  M = ay;  y = y\u0131l",
    L"Takvim",
    L"\u0130ki basamakl\u0131 bir y\u0131l girildi\u011Finde, bunu &aras\u0131nda bir y\u0131l olarak yorumla:",
    L"ile",
    L"Haftan\u0131n ilk g&\u00FCn\u00FC:",
    L"Takvim &t\u00FCr\u00FC:",
    L"Hicri tarihi \u015Funa &ayarla:",
    L"Baz\u0131 programlar\u0131n karakterleri, s\u00F6zc\u00FCkleri, dosyalar\u0131 ve klas\u00F6rleri s\u0131ralama yolunu denetleyebilirsiniz.",
    L"S\u0131&ralama y\u00F6ntemi se\u00E7in:",
    L"Windows dahil baz\u0131 yaz\u0131l\u0131mlar belirli bir konum i\u00E7in ek i\u00E7erik sa\u011Flayabilir. Baz\u0131 hizmetler haberler ve hava durumu gibi yerel bilgiler sa\u011Flar.",
    L"Ge\u00E7erli &konum:",
    L"Di\u011Fer se\u00E7eneklere bak\u0131n",
    L"Ge\u00E7erli kullan\u0131c\u0131, kar\u015F\u0131lama ekran\u0131 (sistem hesaplar\u0131) ve yeni kullan\u0131c\u0131 hesaplar\u0131 ayarlar\u0131 a\u015Fa\u011F\u0131da g\u00F6sterilir.",
    L"* \u00D6zel b\u00F6lge ayar\u0131",
    L"Ge\u00E7erli ayarlar\u0131 \u015Furaya kopyala:",
    L"Kar\u015F\u0131lama ekran\u0131 ve sistem &hesaplar\u0131",
    L"&Yeni kullan\u0131c\u0131 hesaplar\u0131",
    L"Yeni kullan\u0131c\u0131 hesaplar\u0131n\u0131n g\u00F6r\u00FCnt\u00FC dili \u015Fu anda kar\u015F\u0131lama ekran\u0131n\u0131n g\u00F6r\u00FCnt\u00FC dilinden devral\u0131n\u0131yor.",
    nullptr,
    L"\u0130ptal",
    L"Unicode'u desteklemeyen programlarda metin g\u00F6r\u00FCnt\u00FClenirken kullan\u0131lacak dili (sistem b\u00F6lge ayar\u0131) se\u00E7in. Bu ayar bilgisayardaki t\u00FCm kullan\u0131c\u0131 hesaplar\u0131n\u0131 etkiler.",
    L"Ge\u00E7erli sistem b\u00F6lge ayar&\u0131:",
    L"<A>S\u0131ralama y\u00F6ntemini de\u011Fi\u015Ftir</A>",
    L"<A>Bu g\u00F6sterim ne anlama geliyor?</A>",
    L"<A>B\u00F6lge dillerini ve bi\u00E7imlerini de\u011Fi\u015Ftirme hakk\u0131nda \u00E7evrimi\u00E7i daha fazla bilgi edinin</A>",
    L"<A>Kar\u015F\u0131lama ekran\u0131 i\u00E7in klavye d\u00FCzeni nas\u0131l de\u011Fi\u015Ftirilir?</A>",
    L"<A>Ek diller nas\u0131l y\u00FCklenir?</A>",
    L"<A>Bu hesaplar hakk\u0131nda daha fazla bilgi ver</A>",
    L"<A>Sistem b\u00F6lge ayar\u0131 nedir?</A>",
    L"<A>Varsay\u0131lan konum</A>",
};
static const wchar_t* const kTitleTr_TR[11] = {
    L"Bi\u00E7imler",
    L"Klavyeler ve Diller",
    L"Y\u00F6netimsel",
    L"Say\u0131lar",
    L"Para Birimi",
    L"Saat",
    L"Tarih",
    L"S\u0131ralama",
    L"Konum",
    L"Kar\u015F\u0131lama ekran\u0131 ve yeni kullan\u0131c\u0131 hesab\u0131 ayarlar\u0131",
    L"B\u00F6lge ve Dil",
};
// ================= \u010CE\u0160TINA (cs-CZ) =================
static const wchar_t* const kStrTr_CS[66] = {
    L"Oblast a jazyk",
    L"Konfigurovat nastaven\u00ED pro zobrazen\u00ED jazyk\u016F, \u010D\u00EDsel a \u010Das\u016F a dat.",
    L"Zm\u011Bnit form\u00E1t",
    L"Jedno nebo v\u00EDce m\u00EDstn\u00EDch nastaven\u00ED je neplatn\u00FDch. Zkontrolujte a opravte pros\u00EDm vlastn\u00ED nastaven\u00ED, aby byl probl\u00E9m vy\u0159e\u0161en.",
    nullptr,
    nullptr,
    L"Metrick\u00FD",
    nullptr,
    L"Jeden nebo v\u00EDce znak\u016F, kter\u00E9 jste zadali do tohoto pole, je neplatn\u00FDch. Zkuste pros\u00EDm pou\u017E\u00EDt jin\u00E9 znaky.",
    L"Jeden nebo v\u00EDce znak\u016F, kter\u00E9 jste zadali pro %s, je neplatn\u00FDch.  Zkuste pros\u00EDm pou\u017E\u00EDt jin\u00FD znak nebo zadat mezeru.",
    L"Desetinn\u00E1 \u010D\u00E1rka",
    L"Znam\u00E9nko m\u00EDnus",
    L"Odd\u011Blova\u010D \u0159\u00E1d\u016F",
    L"Symbol dop.",
    L"Symbol odp.",
    L"Symbol m\u011Bny",
    L"Desetinn\u00E1 \u010D\u00E1rka m\u011Bny",
    L"Odd\u011Blova\u010D \u0159\u00E1d\u016F m\u011Bny",
    L"Jeden nebo v\u00EDce znak\u016F, kter\u00E9 jste zadali pro form\u00E1t %s, je neplatn\u00FDch. Zkuste pros\u00EDm pou\u017E\u00EDt jin\u00E9 znaky.",
    L"Dlouh\u00FD \u010Das",
    L"Kr\u00E1tk\u00E9 datum",
    L"Dlouh\u00E9 datum",
    L"Hodnota v tomto poli mus\u00ED b\u00FDt \u010D\u00EDslo od 99 do 9999. Zkuste pros\u00EDm pou\u017E\u00EDt jin\u00E9 \u010D\u00EDslo.",
    L"Kr\u00E1tk\u00FD \u010Das",
    L"&Form\u00E1t:",
    L"&Form\u00E1t: (* vlastn\u00ED m\u00EDstn\u00ED nastaven\u00ED)",
    nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
    L"Syst\u00E9mov\u00E9 m\u00EDstn\u00ED nastaven\u00ED bylo zm\u011Bn\u011Bno. Aby se zm\u011Bny projevily, je nutn\u00E9 restartovat syst\u00E9m Windows.",
    L"Zm\u011Bnit m\u00EDstn\u00ED nastaven\u00ED",
    L"Vybran\u00FD jazyk se nepoda\u0159ilo na\u010D\u00EDst. Obra\u0165te se pros\u00EDm na spr\u00E1vce syst\u00E9mu.",
    L"Jazyk zobrazen\u00ED syst\u00E9mu byl zm\u011Bn\u011Bn. Aby se zm\u011Bny projevily, je nutn\u00E9 restartovat syst\u00E9m Windows.",
    L"Opravdu chcete vymazat v\u0161echna p\u0159izp\u016Fsoben\u00ED aktu\u00E1ln\u00EDho form\u00E1tu?",
    L"Opravdu chcete pou\u017E\u00EDt zm\u011Bny jazyka a oblasti?",
    L"Restartovat nyn\u00ED",
    L"Storno",
    L"P\u0159ed restartov\u00E1n\u00EDm ulo\u017Ete svou pr\u00E1ci a zav\u0159ete v\u0161echny otev\u0159en\u00E9 programy.",
    L"Syst\u00E9mov\u00E9 m\u00EDstn\u00ED nastaven\u00ED",
    L"Rozlo\u017Een\u00ED kl\u00E1vesnice %s se nepoda\u0159ilo spr\u00E1vn\u011B na\u010D\u00EDst.",
    L"\u0160pan\u011Bl\u0161tina (\u0160pan\u011Blsko)",
    L"Aby se zm\u011Bna jazyka zobrazen\u00ED projevila, mus\u00EDte se odhl\u00E1sit a znovu p\u0159ihl\u00E1sit",
    L"P\u0159ed odhl\u00E1\u0161en\u00EDm ulo\u017Ete svou pr\u00E1ci a zav\u0159ete v\u0161echny otev\u0159en\u00E9 programy.",
    L"Odhl\u00E1sit nyn\u00ED",
    L"Storno",
    L"Zm\u011Bnit jazyk zobrazen\u00ED",
    L"P\u0159ed proveden\u00EDm dal\u0161\u00EDch zm\u011Bn syst\u00E9mu doporu\u010Dujeme pou\u017E\u00EDt zm\u011Bny, aby je po\u010D\u00EDta\u010D mohl zohlednit.",
    L"Pou\u017E\u00EDt",
    L"Storno",
    L"Operaci se nepoda\u0159ilo dokon\u010Dit",
    L"Aktu\u00E1ln\u00ED u\u017Eivatel",
    L"\u00DAvodn\u00ED obrazovka",
    L"Nov\u00E9 \u00FA\u010Dty",
    L"Jazyk zobrazen\u00ED:",
    L"Vstupn\u00ED jazyk:",
    L"Form\u00E1t:",
    L"Um\u00EDst\u011Bn\u00ED:",
    L"Nepoda\u0159ilo se p\u0159e\u010D\u00EDst parametr",
    L"Kontext",
    L"Nikdy",
    L"Nativn\u00ED",
};
static const wchar_t* const kDlgTr_CS[92] = {
    L"Form\u00E1ty data a \u010Dasu",
    L"&Kr\u00E1tk\u00E9 datum:",
    L"&Dlouh\u00E9 datum:",
    L"Kr\u00E1tk\u00FD \u010D&as:",
    L"Dlouh\u00FD \u010D&as:",
    L"Prvn\u00ED den v t&\u00FDdnu:",
    L"P\u0159\u00EDklady",
    L"Kr\u00E1tk\u00E9 datum:",
    L"Dlouh\u00E9 datum:",
    L"Kr\u00E1tk\u00FD \u010Das:",
    L"Dlouh\u00FD \u010Das:",
    L"Dal\u0161&iacute; nastaven\u00ED...",
    L"Kl\u00E1vesnice a dal\u0161\u00ED vstupn\u00ED jazyky",
    L"Chcete-li zm\u011Bnit kl\u00E1vesnici nebo vstupn\u00ED jazyk, klikn\u011Bte na Zm\u011Bnit kl\u00E1vesnice.",
    L"&Zm\u011Bnit kl\u00E1vesnice...",
    L"Jazyk zobrazen\u00ED",
    L"Nainstalujte nebo odinstalujte jazyky, kter\u00E9 m\u016F\u017Ee syst\u00E9m Windows pou\u017E\u00EDvat k zobrazen\u00ED textu a tam, kde je to podporov\u00E1no, k rozpozn\u00E1v\u00E1n\u00ED \u0159e\u010Di a rukopisu.",
    L"&Nainstalovat nebo odinstalovat jazyky...",
    L"Host nem\u016F\u017Ee zm\u011Bnit jazyk zobrazen\u00ED:",
    L"V\u00FDb\u011Br jazyka zobrazen\u00ED je uzam\u010Den z\u00E1sadami skupiny.",
    L"&Vyberte jazyk zobrazen\u00ED:",
    L"N\u011Bkter&yacute; text nebyl lokalizov\u00E1n do vybran\u00E9ho jazyka. Vyberte jin\u00FD jazyk, kter\u00FD m\u00E1 syst\u00E9m Windows pou\u017E\u00EDvat k zobrazen\u00ED tohoto textu:",
    L"Tento jazyk je lokalizov\u00E1n pouze \u010D\u00E1ste\u010Dn\u011B a n\u011Bkter\u00FD text se m\u016F\u017Ee zobrazit v:",
    L"Tento jazyk je tak&eacute; &lokalizov\u00E1n pouze \u010D\u00E1ste\u010Dn\u011B. Vyberte t\u0159et\u00ED jazyk, kter\u00FD m\u00E1 syst\u00E9m Windows pou\u017E\u00EDvat k zobrazen\u00ED zb\u00FDvaj\u00EDc\u00EDho textu:",
    L"Tento jazyk je tak\u00E9 lokalizov\u00E1n pouze \u010D\u00E1ste\u010Dn\u011B a n\u011Bkter\u00FD text se m\u016F\u017Ee zobrazit v: ",
    L"\u00DAvodn\u00ED obrazovka a nov\u00E9 u\u017Eivatelsk\u00E9 \u00FA\u010Dty",
    L"Zobrazte a zkop\u00EDrujte sv\u00E1 m\u00EDstn\u00ED nastaven\u00ED na \u00FAvodn\u00ED obrazovku, syst\u00E9mov\u00E9 \u00FA\u010Dty a nov\u00E9 u\u017Eivatelsk\u00E9 \u00FA\u010Dty.",
    L"&Kop\u00EDrovat nastaven\u00ED...",
    L"Jazyk program\u016F nepodporuj\u00EDc\u00EDch Unicode",
    L"Toto nastaven\u00ED (syst\u00E9mov\u00E9 m\u00EDstn\u00ED nastaven\u00ED) \u0159\u00EDd\u00ED jazyk pou\u017E\u00EDvan\u00FD p\u0159i zobrazen\u00ED textu v programech, kter\u00E9 nepodporuj\u00ED Unicode.",
    L"Aktu\u00E1ln\u00ED jazyk program\u016F nepodporuj\u00EDc\u00EDch Unicode:",
    nullptr,
    L"Syst\u00E9mov\u00E9 m\u00EDstn\u00ED &nastaven\u00ED...",
    L"P\u0159\u00EDklad",
    L"Kladn\u00E9:",
    L"Z\u00E1porn\u00E9:",
    L"&Desetinn\u00E1 \u010D\u00E1rka:",
    L"P&o\u010Det desetinn\u00FDch m\u00EDst:",
    L"Odd\u011Blova\u010D &\u0159\u00E1d\u016F:",
    L"Seskupov\u00E1n&iacute; \u010D\u00EDsel:",
    L"Symbol z&nam\u00E9nka m\u00EDnus:",
    L"Form\u00E1t z\u00E1porn&yacute;ch \u010D\u00EDsel:",
    L"Zobrazit &nuly na za\u010D\u00E1tku:",
    L"Odd\u011Blova\u010D po&lo\u017Eek seznamu:",
    L"Syst&eacute;&m m\u011Br:",
    L"Standardn&iacute; \u010D\u00EDslice:",
    L"Pou\u017E&iacute;t nativn\u00ED \u010D\u00EDslice:",
    L"Kliknut\u00EDm na Obnovit obnov\u00EDte syst\u00E9mov\u00E1 v\u00FDchoz\u00ED nastaven\u00ED \u010D\u00EDsel, m\u011Bny, \u010Dasu a data.",
    L"Obn&ovit",
    L"Symbol m\u011Bn&y:",
    L"&Kladn\u00FD form\u00E1t m\u011Bny:",
    L"Z\u00E1porn&yacute; form\u00E1t m\u011Bny:",
    L"Po\u010Det desetinn&yacute;ch m\u00EDst:",
    L"Odd\u011Blova\u010D \u0159&\u00E1d\u016F:",
    L"Seskupov\u00E1n\u00ED \u010D\u00ED&sel:",
    L"Form\u00E1ty \u010Dasu",
    L"&Kr\u00E1tk\u00FD \u010Das:",
    L"D&louh\u00FD \u010Das:",
    L"Symbol &dop.:",
    L"Symbol &odp.:",
    L"V\u00FDznam z\u00E1pisu:\n\nh = hodina   m = minuta\ns = sekunda (pouze dlouh\u00FD \u010Das)\ntt = dop. nebo odp.\n\nh/H = 12/24 hodin\n\nhh, mm, ss = zobrazit nulu na za\u010D\u00E1tku\nh, m, s = nezobrazovat nulu na za\u010D\u00E1tku",
    L"Form\u00E1ty data",
    L"V\u00FDznam z\u00E1pisu:\nd, dd = den;  ddd, dddd = den v t\u00FDdnu;  M = m\u011Bs\u00EDc;  y = rok",
    L"Kalend\u00E1\u0159",
    L"Pokud je zad\u00E1n rok ve tvaru dvou \u010D\u00EDslic, interpretovat jej jako rok &mezi:",
    L"a",
    L"Prvn&iacute; den v t\u00FDdnu:",
    L"&Typ kalend\u00E1\u0159e:",
    L"Upravit datum &hid\u017Ery na:",
    L"M\u016F\u017Eete \u0159\u00EDdit zp\u016Fsob, jak\u00FDm n\u011Bkter\u00E9 programy \u0159ad\u00ED znaky, slova, soubory a slo\u017Eky.",
    L"&Vyberte metodu \u0159azen\u00ED:",
    L"N\u011Bkter\u00FD software, v\u010Detn\u011B syst\u00E9mu Windows, m\u016F\u017Ee poskytovat dal\u0161\u00ED obsah pro ur\u010Den\u00E9 um\u00EDst\u011Bn\u00ED. N\u011Bkter\u00E9 slu\u017Eby poskytuj\u00ED m\u00EDstn\u00ED informace, nap\u0159\u00EDklad zpr\u00E1vy a po\u010Das\u00ED.",
    L"Aktu\u00E1ln&iacute; um\u00EDst\u011Bn\u00ED:",
    L"Viz tak\u00E9",
    L"Nastaven&iacute; aktu\u00E1ln\u00EDho u\u017Eivatele, \u00FAvodn\u00ED obrazovky (syst\u00E9mov\u00FDch \u00FA\u010Dt\u016F) a nov\u00FDch u\u017Eivatelsk\u00FDch \u00FA\u010Dt\u016F jsou zobrazena n\u00ED\u017Ee.",
    L"* Vlastn\u00ED m\u00EDstn\u00ED nastaven\u00ED",
    L"Kop\u00EDrovat aktu\u00E1ln\u00ED nastaven\u00ED do:",
    L"\u00DAvodn&iacute; obrazovka a syst\u00E9mov\u00E9 \u00FA\u010Dty",
    L"&Nov\u00E9 u\u017Eivatelsk\u00E9 \u00FA\u010Dty",
    L"Jazyk zobrazen\u00ED nov\u00FDch u\u017Eivatelsk\u00FDch \u00FA\u010Dt\u016F je v sou\u010Dasn\u00E9 dob\u011B zd\u011Bd\u011Bn z jazyka zobrazen\u00ED \u00FAvodn\u00ED obrazovky.",
    nullptr,
    L"Storno",
    L"Vyberte jazyk (syst\u00E9mov\u00E9 m\u00EDstn\u00ED nastaven\u00ED), kter\u00FD se m\u00E1 pou\u017E\u00EDvat p\u0159i zobrazen\u00ED textu v programech, kter\u00E9 nepodporuj\u00ED Unicode. Toto nastaven\u00ED ovliv\u0148uje v\u0161echny u\u017Eivatelsk\u00E9 \u00FA\u010Dty v po\u010D\u00EDta\u010Di.",
    L"Aktu\u00E1ln&iacute; syst\u00E9mov\u00E9 m\u00EDstn\u00ED nastaven\u00ED:",
    L"<A>Zm\u011Bnit metodu \u0159azen\u00ED</A>",
    L"<A>Co znamen\u00E1 tento z\u00E1pis?</A>",
    L"<A>Dal\u0161\u00ED informace online o zm\u011Bn\u011B jazyk\u016F a form\u00E1t\u016F oblast\u00ED</A>",
    L"<A>Jak zm\u011Bnit rozlo\u017Een\u00ED kl\u00E1vesnice pro \u00FAvodn\u00ED obrazovku?</A>",
    L"<A>Jak nainstalovat dal\u0161\u00ED jazyky?</A>",
    L"<A>\u0158ekn\u011Bte mi v\u00EDce o t\u011Bchto \u00FA\u010Dtech</A>",
    L"<A>Co je syst\u00E9mov\u00E9 m\u00EDstn\u00ED nastaven\u00ED?</A>",
    L"<A>V\u00FDchoz\u00ED um\u00EDst\u011Bn\u00ED</A>",
};
static const wchar_t* const kTitleTr_CS[11] = {
    L"Form\u00E1ty",
    L"Kl\u00E1vesnice a jazyky",
    L"Spr\u00E1va",
    L"\u010C\u00EDsla",
    L"M\u011Bna",
    L"\u010Cas",
    L"Datum",
    L"\u0158azen\u00ED",
    L"Um\u00EDst\u011Bn\u00ED",
    L"Nastaven\u00ED \u00FAvodn\u00ED obrazovky a nov\u00FDch u\u017Eivatelsk\u00FDch \u00FA\u010Dt\u016F",
    L"Oblast a jazyk",
};
// ================= MAGYAR (hu-HU) =================
static const wchar_t* const kStrTr_HU[66] = {
    L"Ter\u00FClet \u00E9s nyelv",
    L"A nyelvek, sz\u00E1mok, id\u0151pontok \u00E9s d\u00E1tumok megjelen\u00EDt\u00E9s\u00E9re vonatkoz\u00F3 be\u00E1ll\u00EDt\u00E1sok konfigur\u00E1l\u00E1sa.",
    L"Form\u00E1tum m\u00F3dos\u00EDt\u00E1sa",
    L"Egy vagy t\u00F6bb ter\u00FCleti be\u00E1ll\u00EDt\u00E1s \u00E9rv\u00E9nytelen. A probl\u00E9ma megold\u00E1s\u00E1hoz tekintse \u00E1t \u00E9s jav\u00EDtsa ki az egy\u00E9ni be\u00E1ll\u00EDt\u00E1sokat.",
    nullptr,
    nullptr,
    L"Metrikus",
    nullptr,
    L"Az ebbe a mez\u0151be be\u00EDrt karakterek egyike vagy t\u00F6bbje \u00E9rv\u00E9nytelen. Pr\u00F3b\u00E1ljon meg m\u00E1s karaktereket haszn\u00E1lni.",
    L"A %s mez\u0151be be\u00EDrt karakterek egyike vagy t\u00F6bbje \u00E9rv\u00E9nytelen.  Pr\u00F3b\u00E1ljon meg m\u00E1s karaktert haszn\u00E1lni vagy sz\u00F3k\u00F6zt be\u00EDrni.",
    L"Tizedesjel",
    L"M\u00EDnuszjel",
    L"Ezres tagol\u00F3jel",
    L"De. jel",
    L"Du. jel",
    L"P\u00E9nznemjel",
    L"P\u00E9nznem tizedesjele",
    L"P\u00E9nznem ezres tagol\u00F3jele",
    L"A %s form\u00E1tumba be\u00EDrt karakterek egyike vagy t\u00F6bbje \u00E9rv\u00E9nytelen. Pr\u00F3b\u00E1ljon meg m\u00E1s karaktereket haszn\u00E1lni.",
    L"Hossz\u00FA id\u0151",
    L"R\u00F6vid d\u00E1tum",
    L"Hossz\u00FA d\u00E1tum",
    L"Az ebben a mez\u0151ben l\u00E9v\u0151 \u00E9rt\u00E9knek 99 \u00E9s 9999 k\u00F6z\u00F6tti sz\u00E1mnak kell lennie. Pr\u00F3b\u00E1ljon meg m\u00E1s sz\u00E1mot haszn\u00E1lni.",
    L"R\u00F6vid id\u0151",
    L"&Form\u00E1tum:",
    L"&Form\u00E1tum: (* egy\u00E9ni ter\u00FCleti be\u00E1ll\u00EDt\u00E1s)",
    nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
    L"A rendszer ter\u00FCleti be\u00E1ll\u00EDt\u00E1sa megv\u00E1ltozott. A m\u00F3dos\u00EDt\u00E1sok \u00E9rv\u00E9nybe l\u00E9p\u00E9s\u00E9hez \u00FAjra kell ind\u00EDtania a Windowst.",
    L"Ter\u00FCleti be\u00E1ll\u00EDt\u00E1sok m\u00F3dos\u00EDt\u00E1sa",
    L"A kiv\u00E1lasztott nyelv bet\u00F6lt\u00E9se nem siker\u00FClt. Forduljon a rendszergazd\u00E1hoz.",
    L"A rendszer megjelen\u00EDt\u00E9si nyelve megv\u00E1ltozott. A m\u00F3dos\u00EDt\u00E1sok \u00E9rv\u00E9nybe l\u00E9p\u00E9s\u00E9hez \u00FAjra kell ind\u00EDtania a Windowst.",
    L"T\u00F6rli az aktu\u00E1lis form\u00E1tum \u00F6sszes testreszab\u00E1s\u00E1t?",
    L"Alkalmazza a nyelvi \u00E9s ter\u00FCleti m\u00F3dos\u00EDt\u00E1sokat?",
    L"\u00DAjraind\u00EDt\u00E1s most",
    L"M\u00E9gse",
    L"Az \u00FAjraind\u00EDt\u00E1s el\u0151tt mentse a munk\u00E1j\u00E1t, \u00E9s z\u00E1rja be az \u00F6sszes megnyitott programot.",
    L"Rendszer ter\u00FCleti be\u00E1ll\u00EDt\u00E1sa",
    L"A(z) %s billenty\u0171zetkioszt\u00E1s bet\u00F6lt\u00E9se nem siker\u00FClt megfelel\u0151en.",
    L"Spanyol (Spanyolorsz\u00E1g)",
    L"A megjelen\u00EDt\u00E9si nyelv m\u00F3dos\u00EDt\u00E1s\u00E1nak \u00E9rv\u00E9nybe l\u00E9p\u00E9s\u00E9hez ki kell jelentkeznie, majd \u00FAjra be kell jelentkeznie",
    L"A kijelentkez\u00E9s el\u0151tt mentse a munk\u00E1j\u00E1t, \u00E9s z\u00E1rja be az \u00F6sszes megnyitott programot.",
    L"Kijelentkez\u00E9s most",
    L"M\u00E9gse",
    L"Megjelen\u00EDt\u00E9si nyelv m\u00F3dos\u00EDt\u00E1sa",
    L"Javasoljuk, hogy a m\u00E1sik rendszerm\u00F3dos\u00EDt\u00E1sok elv\u00E9gz\u00E9se el\u0151tt alkalmazza a m\u00F3dos\u00EDt\u00E1sokat, hogy a sz\u00E1m\u00EDt\u00F3g\u00E9p t\u00FCkr\u00F6zhesse azokat.",
    L"Alkalmaz",
    L"M\u00E9gse",
    L"A m\u0171velet nem fejezhet\u0151 be",
    L"Aktu\u00E1lis felhaszn\u00E1l\u00F3",
    L"\u00DCdv\u00F6zl\u0151 k\u00E9perny\u0151",
    L"\u00DAj fi\u00F3kok",
    L"Megjelen\u00EDt\u00E9si nyelv:",
    L"Beviteli nyelv:",
    L"Form\u00E1tum:",
    L"Hely:",
    L"Nem siker\u00FClt beolvasni a param\u00E9tert",
    L"K\u00F6rnyezet",
    L"Soha",
    L"Nemzeti",
};
static const wchar_t* const kDlgTr_HU[92] = {
    L"D\u00E1tum- \u00E9s id\u0151form\u00E1tumok",
    L"&R\u00F6vid d\u00E1tum:",
    L"&Hossz\u00FA d\u00E1tum:",
    L"R\u00F6vid i&d\u0151:",
    L"Hossz\u00FA i&d\u0151:",
    L"A h\u00E9t els\u0151 &napja:",
    L"P\u00E9ld\u00E1k",
    L"R\u00F6vid d\u00E1tum:",
    L"Hossz\u00FA d\u00E1tum:",
    L"R\u00F6vid id\u0151:",
    L"Hossz\u00FA id\u0151:",
    L"Tov\u00E1bbi &be\u00E1ll\u00EDt\u00E1sok...",
    L"Billenty\u0171zetek \u00E9s egy\u00E9b beviteli nyelvek",
    L"A billenty\u0171zet vagy a beviteli nyelv m\u00F3dos\u00EDt\u00E1s\u00E1hoz kattintson a Billenty\u0171zetek m\u00F3dos\u00EDt\u00E1sa gombra.",
    L"&Billenty\u0171zetek m\u00F3dos\u00EDt\u00E1sa...",
    L"Megjelen\u00EDt\u00E9si nyelv",
    L"Telep\u00EDtse vagy t\u00E1vol\u00EDtsa el azokat a nyelveket, amelyeket a Windows sz\u00F6veg megjelen\u00EDt\u00E9s\u00E9re, valamint ahol t\u00E1mogatott, besz\u00E9d- \u00E9s k\u00E9z\u00EDr\u00E1s-felismer\u00E9sre haszn\u00E1lhat.",
    L"Nyelvek &telep\u00EDt\u00E9se vagy elt\u00E1vol\u00EDt\u00E1sa...",
    L"Vend\u00E9gk\u00E9nt nem m\u00F3dos\u00EDthatja a megjelen\u00EDt\u00E9si nyelvet:",
    L"A megjelen\u00EDt\u00E9si nyelv kiv\u00E1laszt\u00E1sa csoporth\u00E1zirend \u00E1ltal z\u00E1rolva van.",
    L"&V\u00E1lasszon megjelen\u00EDt\u00E9si nyelvet:",
    L"N\u00E9h\u00E1ny sz\u00F6veg nincs honos\u00EDtva a kiv\u00E1lasztott nyelvre. V\u00E1lasszon egy m&\u00E1sik nyelvet, amelyet a Windows haszn\u00E1ljon a sz\u00F6veg megjelen\u00EDt\u00E9s\u00E9hez:",
    L"Ez a nyelv csak r\u00E9szben van honos\u00EDtva, \u00E9s n\u00E9h\u00E1ny sz\u00F6veg megjelenhet a k\u00F6vetkez\u0151 nyelven:",
    L"Ez a nyelv &is csak r\u00E9szben van honos\u00EDtva. V\u00E1lasszon egy harmadik nyelvet, amelyet a Windows haszn\u00E1ljon a fennmarad\u00F3 sz\u00F6veg megjelen\u00EDt\u00E9s\u00E9hez:",
    L"Ez a nyelv is csak r\u00E9szben van honos\u00EDtva, \u00E9s n\u00E9h\u00E1ny sz\u00F6veg megjelenhet a k\u00F6vetkez\u0151 nyelven: ",
    L"\u00DCdv\u00F6zl\u0151 k\u00E9perny\u0151 \u00E9s \u00FAj felhaszn\u00E1l\u00F3i fi\u00F3kok",
    L"Tekintse meg \u00E9s m\u00E1solja ter\u00FCleti be\u00E1ll\u00EDt\u00E1sait az \u00FCdv\u00F6zl\u0151 k\u00E9perny\u0151re, a rendszerfi\u00F3kokra \u00E9s az \u00FAj felhaszn\u00E1l\u00F3i fi\u00F3kokra.",
    L"Be\u00E1ll\u00EDt\u00E1sok &m\u00E1sol\u00E1sa...",
    L"Nem Unicode-programok nyelve",
    L"Ez a be\u00E1ll\u00EDt\u00E1s (rendszer ter\u00FCleti be\u00E1ll\u00EDt\u00E1sa) szab\u00E1lyozza a sz\u00F6veg megjelen\u00EDt\u00E9sekor haszn\u00E1lt nyelvet az Unicode-ot nem t\u00E1mogat\u00F3 programokban.",
    L"Nem Unicode-programok aktu\u00E1lis nyelve:",
    nullptr,
    L"Rendszer &ter\u00FCleti be\u00E1ll\u00EDt\u00E1sa...",
    L"Minta",
    L"Pozit\u00EDv:",
    L"Negat\u00EDv:",
    L"&Tizedesjel:",
    L"Tizedes&jegyek sz\u00E1ma:",
    L"Ezres &tagol\u00F3jel:",
    L"Sz\u00E1mok &csoportos\u00EDt\u00E1sa:",
    L"M\u00EDnuszjel-&szimb\u00F3lum:",
    L"Negat\u00EDv sz\u00E1m&form\u00E1tum:",
    L"Bevezet\u0151 &null\u00E1k megjelen\u00EDt\u00E9se:",
    L"Lista&elv\u00E1laszt\u00F3:",
    L"M\u00E9rt\u00E9k&egys\u00E9grendszer:",
    L"Szabv\u00E1nyos sz\u00E1m&jegyek:",
    L"&Nemzeti sz\u00E1mjegyek haszn\u00E1lata:",
    L"Kattintson az Alaphelyzet gombra a sz\u00E1mok, p\u00E9nznem, id\u0151 \u00E9s d\u00E1tum rendszerbeli alap\u00E9rtelmezett be\u00E1ll\u00EDt\u00E1sainak vissza\u00E1ll\u00EDt\u00E1s\u00E1hoz.",
    L"&Alaphelyzet",
    L"P\u00E9nznem&jel:",
    L"&Pozit\u00EDv p\u00E9nznemform\u00E1tum:",
    L"Negat\u00EDv p\u00E9nznem&form\u00E1tum:",
    L"Tizedesjegyek sz&\u00E1ma:",
    L"Ezres tagol\u00F3&jel:",
    L"Sz\u00E1mok csopor&tos\u00EDt\u00E1sa:",
    L"Id\u0151form\u00E1tumok",
    L"&R\u00F6vid id\u0151:",
    L"H&ossz\u00FA id\u0151:",
    L"De. &jel:",
    L"Du. j&el:",
    L"A jel\u00F6l\u00E9s jelent\u00E9se:\n\nh = \u00F3ra   m = perc\ns = m\u00E1sodperc (csak hossz\u00FA id\u0151)\ntt = de. vagy du.\n\nh/H = 12/24 \u00F3ra\n\nhh, mm, ss = bevezet\u0151 nulla megjelen\u00EDt\u00E9se\nh, m, s = bevezet\u0151 nulla mell\u0151z\u00E9se",
    L"D\u00E1tumform\u00E1tumok",
    L"A jel\u00F6l\u00E9s jelent\u00E9se:\nd, dd = nap;  ddd, dddd = a h\u00E9t napja;  M = h\u00F3nap;  y = \u00E9v",
    L"Napt\u00E1r",
    L"Ha k\u00E9tjegy\u0171 \u00E9vet ad meg, \u00E9rtelmezze &a k\u00F6vetkez\u0151k k\u00F6z\u00F6tti \u00E9vk\u00E9nt:",
    L"\u00E9s",
    L"A h\u00E9t els\u0151 n&apja:",
    L"&Napt\u00E1rt\u00EDpus:",
    L"Hidzsri d\u00E1tum &korrig\u00E1l\u00E1sa:",
    L"Szab\u00E1lyozhatja, hogy egyes programok hogyan rendezz\u00E9k a karaktereket, szavakat, f\u00E1jlokat \u00E9s mapp\u00E1kat.",
    L"V\u00E1lasszon rendez\u00E9si &m\u00F3dszert:",
    L"Egyes szoftverek, k\u00F6zt\u00FCk a Windows, tov\u00E1bbi tartalmat biztos\u00EDthatnak egy adott tart\u00F3zkod\u00E1si helyhez. Egyes szolg\u00E1ltat\u00E1sok helyi inform\u00E1ci\u00F3kat ny\u00FAjtanak, p\u00E9ld\u00E1ul h\u00EDreket \u00E9s id\u0151j\u00E1r\u00E1st.",
    L"Aktu\u00E1lis tart\u00F3zkod\u00E1si &hely:",
    L"L\u00E1sd m\u00E9g",
    L"Az aktu\u00E1lis felhaszn\u00E1l\u00F3, az \u00FCdv\u00F6zl\u0151 k\u00E9perny\u0151 (rendszerfi\u00F3kok) \u00E9s az \u00FAj felhaszn\u00E1l\u00F3i fi\u00F3kok be\u00E1ll\u00EDt\u00E1sai al\u00E1bb l\u00E1that\u00F3k.",
    L"* Egy\u00E9ni ter\u00FCleti be\u00E1ll\u00EDt\u00E1s",
    L"Aktu\u00E1lis be\u00E1ll\u00EDt\u00E1sok m\u00E1sol\u00E1sa ide:",
    L"\u00DCdv\u00F6zl\u0151 k\u00E9perny\u0151 \u00E9s rendszer&fi\u00F3kok",
    L"\u00DAj felhaszn\u00E1l\u00F3i &fi\u00F3kok",
    L"Az \u00FAj felhaszn\u00E1l\u00F3i fi\u00F3kok megjelen\u00EDt\u00E9si nyelve jelenleg az \u00FCdv\u00F6zl\u0151 k\u00E9perny\u0151 megjelen\u00EDt\u00E9si nyelv\u00E9b\u0151l \u00F6r\u00F6kl\u0151dik.",
    nullptr,
    L"M\u00E9gse",
    L"V\u00E1lassza ki az Unicode-ot nem t\u00E1mogat\u00F3 programokban a sz\u00F6veg megjelen\u00EDt\u00E9sekor haszn\u00E1land\u00F3 nyelvet (rendszer ter\u00FCleti be\u00E1ll\u00EDt\u00E1sa). Ez a be\u00E1ll\u00EDt\u00E1s a sz\u00E1m\u00EDt\u00F3g\u00E9p \u00F6sszes felhaszn\u00E1l\u00F3i fi\u00F3kj\u00E1t \u00E9rinti.",
    L"Aktu\u00E1lis rendszer ter\u00FCleti be\u00E1ll\u00EDt\u00E1&sa:",
    L"<A>Rendez\u00E9si m\u00F3dszer m\u00F3dos\u00EDt\u00E1sa</A>",
    L"<A>Mit jelent ez a jel\u00F6l\u00E9s?</A>",
    L"<A>Tov\u00E1bbi inform\u00E1ci\u00F3 online a ter\u00FCleti nyelvek \u00E9s form\u00E1tumok m\u00F3dos\u00EDt\u00E1s\u00E1r\u00F3l</A>",
    L"<A>Hogyan m\u00F3dos\u00EDthat\u00F3 a billenty\u0171zetkioszt\u00E1s az \u00FCdv\u00F6zl\u0151 k\u00E9perny\u0151h\u00F6z?</A>",
    L"<A>Hogyan telep\u00EDthet\u0151k tov\u00E1bbi nyelvek?</A>",
    L"<A>Mondjon el t\u00F6bbet ezekr\u0151l a fi\u00F3kokr\u00F3l</A>",
    L"<A>Mi a rendszer ter\u00FCleti be\u00E1ll\u00EDt\u00E1sa?</A>",
    L"<A>Alap\u00E9rtelmezett hely</A>",
};
static const wchar_t* const kTitleTr_HU[11] = {
    L"Form\u00E1tumok",
    L"Billenty\u0171zetek \u00E9s nyelvek",
    L"Kezel\u00E9s",
    L"Sz\u00E1mok",
    L"P\u00E9nznem",
    L"Id\u0151",
    L"D\u00E1tum",
    L"Rendez\u00E9s",
    L"Hely",
    L"\u00DCdv\u00F6zl\u0151 k\u00E9perny\u0151 \u00E9s \u00FAj felhaszn\u00E1l\u00F3i fi\u00F3k be\u00E1ll\u00EDt\u00E1sai",
    L"Ter\u00FClet \u00E9s nyelv",
};
// ================= ROM\u00C2N\u0102 (ro-RO) =================
static const wchar_t* const kStrTr_RO[66] = {
    L"Regiune \u015Fi limb\u0103",
    L"Configura\u0163i set\u0103rile pentru afi\u015Farea limbilor, numerelor, orelor \u015Fi a datelor.",
    L"Modificare format",
    L"Una sau mai multe dintre set\u0103rile regionale nu sunt valide. Examina\u0163i \u015Fi corecta\u0163i set\u0103rile particularizate pentru a rezolva problema.",
    nullptr,
    nullptr,
    L"Metric",
    nullptr,
    L"Unul sau mai multe dintre caracterele pe care le-a\u0163i introdus \u00EEn acest c\u00E2mp nu sunt valide. \u00Cencerca\u0163i s\u0103 utiliza\u0163i alte caractere.",
    L"Unul sau mai multe dintre caracterele pe care le-a\u0163i introdus pentru %s nu sunt valide.  \u00Cencerca\u0163i s\u0103 utiliza\u0163i un alt caracter sau s\u0103 introduce\u0163i un spa\u0163iu.",
    L"Simbol zecimal",
    L"Semn minus",
    L"Simbol de grupare a cifrelor",
    L"Simbol AM",
    L"Simbol PM",
    L"Simbol monetar",
    L"Simbol zecimal monetar",
    L"Simbol de grupare a cifrelor monetare",
    L"Unul sau mai multe dintre caracterele pe care le-a\u0163i introdus pentru formatul %s nu sunt valide. \u00Cencerca\u0163i s\u0103 utiliza\u0163i alte caractere.",
    L"Or\u0103 lung\u0103",
    L"Dat\u0103 scurt\u0103",
    L"Dat\u0103 lung\u0103",
    L"Valoarea din acest c\u00E2mp trebuie s\u0103 fie un num\u0103r \u00EEntre 99 \u015Fi 9999. \u00Cencerca\u0163i s\u0103 utiliza\u0163i un alt num\u0103r.",
    L"Or\u0103 scurt\u0103",
    L"&Format:",
    L"&Format: (* setare regional\u0103 particularizat\u0103)",
    nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
    L"Setarea regional\u0103 de sistem s-a modificat. Trebuie s\u0103 reporni\u0163i Windows pentru ca modific\u0103rile s\u0103 aib\u0103 efect.",
    L"Modificare set\u0103ri regionale",
    L"Limba selectat\u0103 nu a putut fi \u00EEnc\u0103rcat\u0103. Lua\u0163i leg\u0103tura cu administratorul de sistem.",
    L"Limba de afi\u015Fare a sistemului s-a modificat. Trebuie s\u0103 reporni\u0163i Windows pentru ca modific\u0103rile s\u0103 aib\u0103 efect.",
    L"Dori\u0163i s\u0103 \u015Fterge\u0163i toate particulariz\u0103rile formatului curent?",
    L"Dori\u0163i s\u0103 aplica\u0163i modific\u0103rile de limb\u0103 \u015Fi de regiune?",
    L"Repornire acum",
    L"Anulare",
    L"\u00CEnainte de repornire salva\u0163i-v\u0103 lucrul \u015Fi \u00EEnchide\u0163i toate programele deschise.",
    L"Setare regional\u0103 de sistem",
    L"Aspectul de tastatur\u0103 %s nu a putut fi \u00EEnc\u0103rcat \u00EEn mod corespunz\u0103tor.",
    L"Spaniol\u0103 (Spania)",
    L"Pentru ca modificarea limbii de afi\u015Fare s\u0103 aib\u0103 efect, trebuie s\u0103 v\u0103 deconecta\u0163i \u015Fi s\u0103 v\u0103 reconecta\u0163i",
    L"\u00CEnainte de deconectare salva\u0163i-v\u0103 lucrul \u015Fi \u00EEnchide\u0163i toate programele deschise.",
    L"Deconectare acum",
    L"Anulare",
    L"Modificare limb\u0103 de afi\u015Fare",
    L"V\u0103 recomand\u0103m s\u0103 aplica\u0163i modific\u0103rile \u00EEnainte de a face alte modific\u0103ri de sistem, pentru ca acestea s\u0103 se reflecte pe computer.",
    L"Aplicare",
    L"Anulare",
    L"Opera\u0163ia nu s-a putut termina",
    L"Utilizator curent",
    L"Ecran de \u00EEnt\u00E2mpinare",
    L"Conturi noi",
    L"Limb\u0103 de afi\u015Fare:",
    L"Limb\u0103 de intrare:",
    L"Format:",
    L"Loca\u0163ie:",
    L"Parametrul nu s-a putut citi",
    L"Context",
    L"niciodat\u0103",
    L"Nativ",
};
static const wchar_t* const kDlgTr_RO[92] = {
    L"Formate de dat\u0103 \u015Fi or\u0103",
    L"&Dat\u0103 scurt\u0103:",
    L"Dat\u0103 &lung\u0103:",
    L"Or\u0103 s&curt\u0103:",
    L"Or\u0103 l&ung\u0103:",
    L"Prima zi a &s\u0103pt\u0103m\u00E2nii:",
    L"Exemple",
    L"Dat\u0103 scurt\u0103:",
    L"Dat\u0103 lung\u0103:",
    L"Or\u0103 scurt\u0103:",
    L"Or\u0103 lung\u0103:",
    L"Set\u0103ri supli&mentare...",
    L"Tastaturi \u015Fi alte limbi de intrare",
    L"Pentru a modifica tastatura sau limba de intrare, face\u0163i clic pe Modificare tastaturi.",
    L"&Modificare tastaturi...",
    L"Limb\u0103 de afi\u015Fare",
    L"Instala\u0163i sau dezinstala\u0163i limbile pe care Windows le poate utiliza pentru afi\u015Farea textului \u015Fi, acolo unde este acceptat, pentru recunoa\u015Fterea vorbirii \u015Fi a scrisului de m\u00E2n\u0103.",
    L"&Instalare sau dezinstalare limbi...",
    L"Ca invitat nu pute\u0163i modifica limba de afi\u015Fare:",
    L"Selec\u0163ia limbii de afi\u015Fare este blocat\u0103 de politica de grup.",
    L"&Selecta\u0163i o limb\u0103 de afi\u015Fare:",
    L"Anumite texte nu au fost locali&zate \u00EEn limba selectat\u0103. Selecta\u0163i o alt\u0103 limb\u0103 pe care s\u0103 o utilizeze Windows pentru afi\u015Farea acestui text:",
    L"Aceast\u0103 limb\u0103 este localizat\u0103 doar par\u0163ial, iar anumite texte pot fi afi\u015Fate \u00EEn:",
    L"Aceast\u0103 limb\u0103 este de asemenea localizat\u0103 doar par\u0163&ial. Selecta\u0163i o a treia limb\u0103 pe care s\u0103 o utilizeze Windows pentru afi\u015Farea textului r\u0103mas:",
    L"Aceast\u0103 limb\u0103 este de asemenea localizat\u0103 doar par\u0163ial, iar anumite texte pot fi afi\u015Fate \u00EEn: ",
    L"Ecran de \u00EEnt\u00E2mpinare \u015Fi conturi de utilizator noi",
    L"Vizualiza\u0163i \u015Fi copia\u0163i set\u0103rile regionale pe ecranul de \u00EEnt\u00E2mpinare, \u00EEn conturile de sistem \u015Fi \u00EEn conturile de utilizator noi.",
    L"&Copiere set\u0103ri...",
    L"Limb\u0103 pentru programe non-Unicode",
    L"Aceast\u0103 setare (setare regional\u0103 de sistem) controleaz\u0103 limba utilizat\u0103 la afi\u015Farea textului \u00EEn programe care nu accept\u0103 Unicode.",
    L"Limb\u0103 curent\u0103 pentru programe non-Unicode:",
    nullptr,
    L"Setare regional\u0103 de &sistem...",
    L"Exemplu",
    L"Pozitiv:",
    L"Negativ:",
    L"Simbol &zecimal:",
    L"Num\u0103r de &zecimale:",
    L"Simbol de grupare a &cifrelor:",
    L"&Grupare cifre:",
    L"Simbol semn &minus:",
    L"Format numere ne&gative:",
    L"Afi\u015Fare zerouri &ini\u0163iale:",
    L"Separator &list\u0103:",
    L"Sistem de m\u0103&sur\u0103:",
    L"Cifre stan&dard:",
    L"&Utilizare cifre native:",
    L"Face\u0163i clic pe Reini\u0163ializare pentru a restabili set\u0103rile implicite de sistem pentru numere, moned\u0103, or\u0103 \u015Fi dat\u0103.",
    L"&Reini\u0163ializare",
    L"Simbol &monetar:",
    L"Format monetar &pozitiv:",
    L"Format monetar ne&gativ:",
    L"Num\u0103r de &zecimale:",
    L"Simbol de &grupare a cifrelor:",
    L"&Grupare cifre:",
    L"Formate de or\u0103",
    L"&Or\u0103 scurt\u0103:",
    L"Or\u0103 &lung\u0103:",
    L"Simbol &AM:",
    L"Simbol &PM:",
    L"Semnifica\u0163ia nota\u0163iei:\n\nh = ora   m = minutul\ns = secunda (doar ora lung\u0103)\ntt = AM sau PM\n\nh/H = 12/24 ore\n\nhh, mm, ss = afi\u015Fare zero ini\u0163ial\nh, m, s = f\u0103r\u0103 afi\u015Fare zero ini\u0163ial",
    L"Formate de dat\u0103",
    L"Semnifica\u0163ia nota\u0163iei:\nd, dd = ziua;  ddd, dddd = ziua s\u0103pt\u0103m\u00E2nii;  M = luna;  y = anul",
    L"Calendar",
    L"C\u00E2nd se introduce un an din dou\u0103 cifre, se interpreteaz\u0103 ca an \u00EEn&tre:",
    L"\u015Fi",
    L"P&rima zi a s\u0103pt\u0103m\u00E2nii:",
    L"&Tip de calendar:",
    L"Ajustare dat\u0103 &Hijri la:",
    L"Pute\u0163i controla modul \u00EEn care unele programe sorteaz\u0103 caractere, cuvinte, fi\u015Fiere \u015Fi foldere.",
    L"Selecta\u0163i metoda de &sortare:",
    L"Unele programe software, inclusiv Windows, pot s\u0103 furnizeze con\u0163inut suplimentar pentru o anumit\u0103 loca\u0163ie. Unele servicii furnizeaz\u0103 informa\u0163ii locale, cum ar fi \u015Ftiri \u015Fi vreme.",
    L"Loca\u0163ie &curent\u0103:",
    L"Consulta\u0163i \u015Fi",
    L"Set\u0103rile utilizatorului curent, ecranului de \u00EEnt\u00E2mpinare (conturi de sistem) \u015Fi conturilor de utilizator noi sunt afi\u015Fate mai jos.",
    L"* Setare regional\u0103 particularizat\u0103",
    L"Copiere set\u0103ri curente la:",
    L"Ecran de \u00EEnt\u00E2mpinare \u015Fi conturi de &sistem",
    L"Conturi de utilizator &noi",
    L"Limba de afi\u015Fare a conturilor de utilizator noi este mo\u015Ftenit\u0103 \u00EEn prezent de la limba de afi\u015Fare a ecranului de \u00EEnt\u00E2mpinare.",
    nullptr,
    L"Anulare",
    L"Selecta\u0163i limba (setare regional\u0103 de sistem) care se utilizeaz\u0103 la afi\u015Farea textului \u00EEn programe care nu accept\u0103 Unicode. Aceast\u0103 setare afecteaz\u0103 toate conturile de utilizator de pe computer.",
    L"Setare regional\u0103 de sistem &curent\u0103:",
    L"<A>Modificare metod\u0103 de sortare</A>",
    L"<A>Ce \u00EEnseamn\u0103 aceast\u0103 nota\u0163ie?</A>",
    L"<A>Afla\u0163i mai multe online despre modificarea limbilor \u015Fi formatelor regionale</A>",
    L"<A>Cum se modific\u0103 aspectul de tastatur\u0103 pentru ecranul de \u00EEnt\u00E2mpinare?</A>",
    L"<A>Cum se instaleaz\u0103 limbi suplimentare?</A>",
    L"<A>Spune\u0163i-mi mai multe despre aceste conturi</A>",
    L"<A>Ce este setarea regional\u0103 de sistem?</A>",
    L"<A>Loca\u0163ie implicit\u0103</A>",
};
static const wchar_t* const kTitleTr_RO[11] = {
    L"Formate",
    L"Tastaturi \u015Fi limbi",
    L"Administrare",
    L"Numere",
    L"Moned\u0103",
    L"Or\u0103",
    L"Dat\u0103",
    L"Sortare",
    L"Loca\u0163ie",
    L"Set\u0103ri ecran de \u00EEnt\u00E2mpinare \u015Fi conturi de utilizator noi",
    L"Regiune \u015Fi limb\u0103",
};
// ================= SVENSKA (sv-SE) =================
static const wchar_t* const kStrTr_SV[66] = {
    L"Region och spr\u00E5k",
    L"Konfigurera inst\u00E4llningarna f\u00F6r hur spr\u00E5k, tal, tider och datum ska visas.",
    L"\u00C4ndra format",
    L"En eller flera nationella inst\u00E4llningar \u00E4r ogiltiga. Granska och korrigera de anpassade inst\u00E4llningarna f\u00F6r att l\u00F6sa problemet.",
    nullptr,
    nullptr,
    L"Metriskt",
    nullptr,
    L"Ett eller flera av de tecken som du angav i det h\u00E4r f\u00E4ltet \u00E4r ogiltiga. F\u00F6rs\u00F6k med andra tecken.",
    L"Ett eller flera av de tecken som du angav f\u00F6r %s \u00E4r ogiltiga.  F\u00F6rs\u00F6k med ett annat tecken eller ange ett blanksteg.",
    L"Decimalsymbol",
    L"Minustecken",
    L"Grupperingssymbol",
    L"AM-symbol",
    L"PM-symbol",
    L"Valutasymbol",
    L"Valutadecimalsymbol",
    L"Valutagrupperingssymbol",
    L"Ett eller flera av de tecken som du angav f\u00F6r formatet %s \u00E4r ogiltiga. F\u00F6rs\u00F6k med andra tecken.",
    L"L\u00E5ng tid",
    L"Kort datum",
    L"L\u00E5ngt datum",
    L"V\u00E4rdet i det h\u00E4r f\u00E4ltet m\u00E5ste vara ett tal mellan 99 och 9999. F\u00F6rs\u00F6k med ett annat tal.",
    L"Kort tid",
    L"&Format:",
    L"&Format: (* anpassad nationell inst\u00E4llning)",
    nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
    L"Systemets nationella inst\u00E4llning har \u00E4ndrats. Du m\u00E5ste starta om Windows f\u00F6r att \u00E4ndringarna ska b\u00F6rja g\u00E4lla.",
    L"\u00C4ndra nationella inst\u00E4llningar",
    L"Det gick inte att l\u00E4sa in valt spr\u00E5k. Kontakta systemadministrat\u00F6ren.",
    L"Systemets visningsspr\u00E5k har \u00E4ndrats. Du m\u00E5ste starta om Windows f\u00F6r att \u00E4ndringarna ska b\u00F6rja g\u00E4lla.",
    L"Vill du ta bort alla anpassningar av det aktuella formatet?",
    L"Vill du verkst\u00E4lla \u00E4ndringarna av spr\u00E5k och region?",
    L"Starta om nu",
    L"Avbryt",
    L"Spara ditt arbete och st\u00E4ng alla \u00F6ppna program innan du startar om.",
    L"Nationell inst\u00E4llning",
    L"Tangentbordslayouten %s kunde inte l\u00E4sas in ordentligt.",
    L"Spanska (Spanien)",
    L"F\u00F6r att \u00E4ndringen av visningsspr\u00E5k ska b\u00F6rja g\u00E4lla m\u00E5ste du logga ut och sedan logga in igen",
    L"Spara ditt arbete och st\u00E4ng alla \u00F6ppna program innan du loggar ut.",
    L"Logga ut nu",
    L"Avbryt",
    L"\u00C4ndra visningsspr\u00E5k",
    L"Vi rekommenderar att du verkst\u00E4ller \u00E4ndringarna innan du g\u00F6r andra system\u00E4ndringar, s\u00E5 att datorn kan \u00E5terspegla dem.",
    L"Verkst\u00E4ll",
    L"Avbryt",
    L"Det gick inte att slutf\u00F6ra \u00E5tg\u00E4rden",
    L"Aktuell anv\u00E4ndare",
    L"V\u00E4lkomstsk\u00E4rm",
    L"Nya konton",
    L"Visningsspr\u00E5k:",
    L"Inmatningsspr\u00E5k:",
    L"Format:",
    L"Plats:",
    L"Det gick inte att l\u00E4sa parametern",
    L"Kontext",
    L"Aldrig",
    L"Inhemsk",
};
static const wchar_t* const kDlgTr_SV[92] = {
    L"Datum- och tidsformat",
    L"&Kort datum:",
    L"&L\u00E5ngt datum:",
    L"Kort &tid:",
    L"L\u00E5ng &tid:",
    L"F\u00F6rsta dagen i &veckan:",
    L"Exempel",
    L"Kort datum:",
    L"L\u00E5ngt datum:",
    L"Kort tid:",
    L"L\u00E5ng tid:",
    L"&Fler inst\u00E4llningar...",
    L"Tangentbord och andra inmatningsspr\u00E5k",
    L"Om du vill \u00E4ndra tangentbord eller inmatningsspr\u00E5k klickar du p\u00E5 \u00C4ndra tangentbord.",
    L"\u00C4ndra &tangentbord...",
    L"Visningsspr\u00E5k",
    L"Installera eller avinstallera spr\u00E5k som Windows kan anv\u00E4nda f\u00F6r att visa text och, d\u00E4r det st\u00F6ds, f\u00F6r tal- och handskriftsigenk\u00E4nning.",
    L"&Installera eller avinstallera visningsspr\u00E5k...",
    L"Som g\u00E4st kan du inte \u00E4ndra visningsspr\u00E5ket:",
    L"Val av visningsspr\u00E5k \u00E4r l\u00E5st av grupprincip.",
    L"&V\u00E4lj ett visningsspr\u00E5k:",
    L"En del text har inte &lokaliserats till valt spr\u00E5k. V\u00E4lj ett annat spr\u00E5k som Windows ska anv\u00E4nda f\u00F6r att visa den h\u00E4r texten:",
    L"Det h\u00E4r spr\u00E5ket \u00E4r bara delvis lokaliserat och en del text kan visas p\u00E5:",
    L"Det h\u00E4r spr\u00E5ket \u00E4r ocks\u00E5 bara delvis &lokaliserat. V\u00E4lj ett tredje spr\u00E5k som Windows ska anv\u00E4nda f\u00F6r att visa \u00E5terst\u00E5ende text:",
    L"Det h\u00E4r spr\u00E5ket \u00E4r ocks\u00E5 bara delvis lokaliserat och en del text kan visas p\u00E5: ",
    L"V\u00E4lkomstsk\u00E4rm och nya anv\u00E4ndarkonton",
    L"Visa och kopiera dina nationella inst\u00E4llningar till v\u00E4lkomstsk\u00E4rmen, systemkonton och nya anv\u00E4ndarkonton.",
    L"&Kopiera inst\u00E4llningar...",
    L"Spr\u00E5k f\u00F6r program som inte anv\u00E4nder Unicode",
    L"Den h\u00E4r inst\u00E4llningen (systemets nationella inst\u00E4llning) styr vilket spr\u00E5k som anv\u00E4nds n\u00E4r text visas i program som inte st\u00F6der Unicode.",
    L"Aktuellt spr\u00E5k f\u00F6r program som inte anv\u00E4nder Unicode:",
    nullptr,
    L"&Nationell inst\u00E4llning...",
    L"Exempel",
    L"Positiva:",
    L"Negativa:",
    L"&Decimalsymbol:",
    L"Antal &decimaler:",
    L"&Grupperingssymbol:",
    L"Tal&gruppering:",
    L"Symbol f\u00F6r &minustecken:",
    L"Format f\u00F6r &negativa tal:",
    L"Visa inledande &nollor:",
    L"List&avgr\u00E4nsare:",
    L"M\u00E5tt&system:",
    L"Standard&siffror:",
    L"Anv\u00E4nd &nationella siffror:",
    L"Klicka p\u00E5 \u00C5terst\u00E4ll om du vill \u00E5terst\u00E4lla systemets standardinst\u00E4llningar f\u00F6r tal, valuta, tid och datum.",
    L"\u00C5ter&st\u00E4ll",
    L"&Valutasymbol:",
    L"Positivt &valutaformat:",
    L"Negativt valuta&format:",
    L"Antal decima&ler:",
    L"Grupperings&symbol:",
    L"Talgrupperin&g:",
    L"Tidsformat",
    L"&Kort tid:",
    L"L\u00E5n&g tid:",
    L"&AM-symbol:",
    L"&PM-symbol:",
    L"Betydelse av notation:\n\nh = timme   m = minut\ns = sekund (endast l\u00E5ng tid)\ntt = FM eller EM\n\nh/H = 12/24 timmar\n\nhh, mm, ss = visa inledande nolla\nh, m, s = visa inte inledande nolla",
    L"Datumformat",
    L"Betydelse av notation:\nd, dd = dag;  ddd, dddd = veckodag;  M = m\u00E5nad;  y = \u00E5r",
    L"Kalender",
    L"N\u00E4r ett tv\u00E5siffrigt \u00E5rtal anges, tolka det som ett \u00E5r &mellan:",
    L"och",
    L"F\u00F6rsta dagen i v&eckan:",
    L"&Kalendertyp:",
    L"Justera Hidjra-datum till &f\u00F6ljande:",
    L"Du kan styra hur vissa program sorterar tecken, ord, filer och mappar.",
    L"&V\u00E4lj sorteringsmetod:",
    L"Vissa program, inklusive Windows, kan tillhandah\u00E5lla extra inneh\u00E5ll f\u00F6r en viss plats. Vissa tj\u00E4nster tillhandah\u00E5ller lokal information, till exempel nyheter och v\u00E4der.",
    L"Aktuell &plats:",
    L"Se \u00E4ven",
    L"Inst\u00E4llningarna f\u00F6r aktuell anv\u00E4ndare, v\u00E4lkomstsk\u00E4rmen (systemkonton) och nya anv\u00E4ndarkonton visas nedan.",
    L"* Anpassad nationell inst\u00E4llning",
    L"Kopiera aktuella inst\u00E4llningar till:",
    L"V\u00E4lkomstsk\u00E4rm och system&konton",
    L"&Nya anv\u00E4ndarkonton",
    L"Visningsspr\u00E5ket f\u00F6r nya anv\u00E4ndarkonton \u00E4rvs f\u00F6r n\u00E4rvarande fr\u00E5n v\u00E4lkomstsk\u00E4rmens visningsspr\u00E5k.",
    nullptr,
    L"Avbryt",
    L"V\u00E4lj det spr\u00E5k (systemets nationella inst\u00E4llning) som ska anv\u00E4ndas n\u00E4r text visas i program som inte st\u00F6der Unicode. Den h\u00E4r inst\u00E4llningen p\u00E5verkar alla anv\u00E4ndarkonton p\u00E5 datorn.",
    L"Aktuell systemets nationella inst\u00E4ll&ning:",
    L"<A>\u00C4ndra sorteringsmetod</A>",
    L"<A>Vad betyder den h\u00E4r notationen?</A>",
    L"<A>L\u00E4s mer online om att \u00E4ndra regionala spr\u00E5k och format</A>",
    L"<A>Hur \u00E4ndrar jag tangentbordslayout f\u00F6r v\u00E4lkomstsk\u00E4rmen?</A>",
    L"<A>Hur installerar jag ytterligare spr\u00E5k?</A>",
    L"<A>Ber\u00E4tta mer om de h\u00E4r kontona</A>",
    L"<A>Vad \u00E4r systemets nationella inst\u00E4llning?</A>",
    L"<A>Standardplats</A>",
};
static const wchar_t* const kTitleTr_SV[11] = {
    L"Format",
    L"Tangentbord och spr\u00E5k",
    L"Administration",
    L"Tal",
    L"Valuta",
    L"Tid",
    L"Datum",
    L"Sortering",
    L"Plats",
    L"Inst\u00E4llningar f\u00F6r v\u00E4lkomstsk\u00E4rm och nya anv\u00E4ndarkonton",
    L"Region och spr\u00E5k",
};
// ================= \u0423\u041A\u0420\u0410\u0407\u041D\u0421\u042C\u041A\u0410 (uk-UA) \u2014 community translation =================
static const wchar_t* const kStrTr_UK[66] = {
    L"\u0420\u0435\u0433\u0456\u043E\u043D \u0456 \u043C\u043E\u0432\u0430",
    L"\u041D\u0430\u043B\u0430\u0448\u0442\u0443\u0432\u0430\u043D\u043D\u044F \u043F\u0430\u0440\u0430\u043C\u0435\u0442\u0440\u0456\u0432 \u0432\u0456\u0434\u043E\u0431\u0440\u0430\u0436\u0435\u043D\u043D\u044F \u043C\u043E\u0432, \u0447\u0438\u0441\u0435\u043B, \u0447\u0430\u0441\u0443 \u0442\u0430 \u0434\u0430\u0442\u0438.",
    L"\u0417\u043C\u0456\u043D\u0438\u0442\u0438 \u0444\u043E\u0440\u043C\u0430\u0442",
    L"\u041E\u0434\u0438\u043D \u0430\u0431\u043E \u043A\u0456\u043B\u044C\u043A\u0430 \u0440\u0435\u0433\u0456\u043E\u043D\u0430\u043B\u044C\u043D\u0438\u0445 \u043F\u0430\u0440\u0430\u043C\u0435\u0442\u0440\u0456\u0432 \u043D\u0435\u043F\u0440\u0438\u043F\u0443\u0441\u0442\u0438\u043C\u0456. \u041F\u0435\u0440\u0435\u0433\u043B\u044F\u043D\u044C\u0442\u0435 \u0442\u0430 \u0432\u0438\u043F\u0440\u0430\u0432\u0442\u0435 \u043D\u0430\u043B\u0430\u0448\u0442\u043E\u0432\u0443\u0432\u0430\u043D\u0456 \u043F\u0430\u0440\u0430\u043C\u0435\u0442\u0440\u0438, \u0449\u043E\u0431 \u0432\u0438\u0440\u0456\u0448\u0438\u0442\u0438 \u043F\u0440\u043E\u0431\u043B\u0435\u043C\u0443.",
    nullptr,
    nullptr,
    L"\u041C\u0435\u0442\u0440\u0438\u0447\u043D\u0430",
    nullptr,
    L"\u041E\u0434\u0438\u043D \u0430\u0431\u043E \u043A\u0456\u043B\u044C\u043A\u0430 \u0441\u0438\u043C\u0432\u043E\u043B\u0456\u0432, \u0443\u0432\u0435\u0434\u0435\u043D\u0438\u0445 \u0443 \u0446\u0435 \u043F\u043E\u043B\u0435, \u043D\u0435\u043F\u0440\u0438\u043F\u0443\u0441\u0442\u0438\u043C\u0456. \u0421\u043F\u0440\u043E\u0431\u0443\u0439\u0442\u0435 \u0432\u0438\u043A\u043E\u0440\u0438\u0441\u0442\u0430\u0442\u0438 \u0456\u043D\u0448\u0456 \u0441\u0438\u043C\u0432\u043E\u043B\u0438.",
    L"\u041E\u0434\u0438\u043D \u0430\u0431\u043E \u043A\u0456\u043B\u044C\u043A\u0430 \u0441\u0438\u043C\u0432\u043E\u043B\u0456\u0432, \u0443\u0432\u0435\u0434\u0435\u043D\u0438\u0445 \u0434\u043B\u044F %s, \u043D\u0435\u043F\u0440\u0438\u043F\u0443\u0441\u0442\u0438\u043C\u0456. \u0421\u043F\u0440\u043E\u0431\u0443\u0439\u0442\u0435 \u0432\u0438\u043A\u043E\u0440\u0438\u0441\u0442\u0430\u0442\u0438 \u0456\u043D\u0448\u0438\u0439 \u0441\u0438\u043C\u0432\u043E\u043B \u0430\u0431\u043E \u0443\u0432\u0435\u0434\u0456\u0442\u044C \u043F\u0440\u043E\u0431\u0456\u043B.",
    L"\u0414\u0435\u0441\u044F\u0442\u043A\u043E\u0432\u0438\u0439 \u0440\u043E\u0437\u0434\u0456\u043B\u044C\u043D\u0438\u043A",
    L"\u0417\u043D\u0430\u043A \u043C\u0456\u043D\u0443\u0441\u0430",
    L"\u0420\u043E\u0437\u0434\u0456\u043B\u044C\u043D\u0438\u043A \u0433\u0440\u0443\u043F",
    L"\u0421\u0438\u043C\u0432\u043E\u043B AM",
    L"\u0421\u0438\u043C\u0432\u043E\u043B PM",
    L"\u0421\u0438\u043C\u0432\u043E\u043B \u0432\u0430\u043B\u044E\u0442\u0438",
    L"\u0414\u0435\u0441\u044F\u0442\u043A\u043E\u0432\u0438\u0439 \u0440\u043E\u0437\u0434\u0456\u043B\u044C\u043D\u0438\u043A \u0432\u0430\u043B\u044E\u0442\u0438",
    L"\u0420\u043E\u0437\u0434\u0456\u043B\u044C\u043D\u0438\u043A \u0433\u0440\u0443\u043F \u0432\u0430\u043B\u044E\u0442\u0438",
    L"\u041E\u0434\u0438\u043D \u0430\u0431\u043E \u043A\u0456\u043B\u044C\u043A\u0430 \u0441\u0438\u043C\u0432\u043E\u043B\u0456\u0432, \u0443\u0432\u0435\u0434\u0435\u043D\u0438\u0445 \u0434\u043B\u044F \u0444\u043E\u0440\u043C\u0430\u0442\u0443 %s, \u043D\u0435\u043F\u0440\u0438\u043F\u0443\u0441\u0442\u0438\u043C\u0456. \u0421\u043F\u0440\u043E\u0431\u0443\u0439\u0442\u0435 \u0432\u0438\u043A\u043E\u0440\u0438\u0441\u0442\u0430\u0442\u0438 \u0456\u043D\u0448\u0456 \u0441\u0438\u043C\u0432\u043E\u043B\u0438.",
    L"\u041F\u043E\u0432\u043D\u0438\u0439 \u0447\u0430\u0441",
    L"\u041A\u043E\u0440\u043E\u0442\u043A\u0430 \u0434\u0430\u0442\u0430",
    L"\u041F\u043E\u0432\u043D\u0430 \u0434\u0430\u0442\u0430",
    L"\u0417\u043D\u0430\u0447\u0435\u043D\u043D\u044F \u0443 \u0446\u044C\u043E\u043C\u0443 \u043F\u043E\u043B\u0456 \u043C\u0430\u0454 \u0431\u0443\u0442\u0438 \u0447\u0438\u0441\u043B\u043E\u043C \u0432\u0456\u0434 99 \u0434\u043E 9999. \u0421\u043F\u0440\u043E\u0431\u0443\u0439\u0442\u0435 \u0432\u0438\u043A\u043E\u0440\u0438\u0441\u0442\u0430\u0442\u0438 \u0456\u043D\u0448\u0435 \u0447\u0438\u0441\u043B\u043E.",
    L"\u041A\u043E\u0440\u043E\u0442\u043A\u0438\u0439 \u0447\u0430\u0441",
    L"&\u0424\u043E\u0440\u043C\u0430\u0442:",
    L"&\u0424\u043E\u0440\u043C\u0430\u0442: (* \u043D\u0430\u043B\u0430\u0448\u0442\u043E\u0432\u0430\u043D\u0430 \u043C\u043E\u0432\u0430)",
    nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
    L"\u041C\u043E\u0432\u0443 \u0441\u0438\u0441\u0442\u0435\u043C\u0438 \u0437\u043C\u0456\u043D\u0435\u043D\u043E. \u0429\u043E\u0431 \u0437\u043C\u0456\u043D\u0438 \u043D\u0430\u0431\u0443\u043B\u0438 \u0447\u0438\u043D\u043D\u043E\u0441\u0442\u0456, \u043D\u0435\u043E\u0431\u0445\u0456\u0434\u043D\u043E \u043F\u0435\u0440\u0435\u0437\u0430\u043F\u0443\u0441\u0442\u0438\u0442\u0438 Windows.",
    L"\u0417\u043C\u0456\u043D\u0438\u0442\u0438 \u0440\u0435\u0433\u0456\u043E\u043D\u0430\u043B\u044C\u043D\u0456 \u043F\u0430\u0440\u0430\u043C\u0435\u0442\u0440\u0438",
    L"\u041D\u0435 \u0432\u0434\u0430\u043B\u043E\u0441\u044F \u0437\u0430\u0432\u0430\u043D\u0442\u0430\u0436\u0438\u0442\u0438 \u0432\u0438\u0431\u0440\u0430\u043D\u0443 \u043C\u043E\u0432\u0443. \u0417\u0432\u0435\u0440\u043D\u0456\u0442\u044C\u0441\u044F \u0434\u043E \u0441\u0438\u0441\u0442\u0435\u043C\u043D\u043E\u0433\u043E \u0430\u0434\u043C\u0456\u043D\u0456\u0441\u0442\u0440\u0430\u0442\u043E\u0440\u0430.",
    L"\u041C\u043E\u0432\u0443 \u0456\u043D\u0442\u0435\u0440\u0444\u0435\u0439\u0441\u0443 \u0441\u0438\u0441\u0442\u0435\u043C\u0438 \u0437\u043C\u0456\u043D\u0435\u043D\u043E. \u0429\u043E\u0431 \u0437\u043C\u0456\u043D\u0438 \u043D\u0430\u0431\u0443\u043B\u0438 \u0447\u0438\u043D\u043D\u043E\u0441\u0442\u0456, \u043D\u0435\u043E\u0431\u0445\u0456\u0434\u043D\u043E \u043F\u0435\u0440\u0435\u0437\u0430\u043F\u0443\u0441\u0442\u0438\u0442\u0438 Windows.",
    L"\u041E\u0447\u0438\u0441\u0442\u0438\u0442\u0438 \u0432\u0441\u0456 \u043D\u0430\u043B\u0430\u0448\u0442\u0443\u0432\u0430\u043D\u043D\u044F \u043F\u043E\u0442\u043E\u0447\u043D\u043E\u0433\u043E \u0444\u043E\u0440\u043C\u0430\u0442\u0443?",
    L"\u0417\u0430\u0441\u0442\u043E\u0441\u0443\u0432\u0430\u0442\u0438 \u0437\u043C\u0456\u043D\u0438 \u043C\u043E\u0432\u0438 \u0442\u0430 \u0440\u0435\u0433\u0456\u043E\u043D\u0430\u043B\u044C\u043D\u0438\u0445 \u0441\u0442\u0430\u043D\u0434\u0430\u0440\u0442\u0456\u0432?",
    L"\u041F\u0435\u0440\u0435\u0437\u0430\u043F\u0443\u0441\u0442\u0438\u0442\u0438 \u0437\u0430\u0440\u0430\u0437",
    L"\u0421\u043A\u0430\u0441\u0443\u0432\u0430\u0442\u0438",
    L"\u041F\u0435\u0440\u0435\u0434 \u043F\u0435\u0440\u0435\u0437\u0430\u0432\u0430\u043D\u0442\u0430\u0436\u0435\u043D\u043D\u044F\u043C \u0437\u0431\u0435\u0440\u0435\u0436\u0456\u0442\u044C \u0440\u043E\u0431\u043E\u0442\u0443 \u0442\u0430 \u0437\u0430\u043A\u0440\u0438\u0439\u0442\u0435 \u0432\u0441\u0456 \u0432\u0456\u0434\u043A\u0440\u0438\u0442\u0456 \u043F\u0440\u043E\u0433\u0440\u0430\u043C\u0438.",
    L"\u041C\u043E\u0432\u0430 \u0441\u0438\u0441\u0442\u0435\u043C\u0438",
    L"\u041D\u0435 \u0432\u0434\u0430\u043B\u043E\u0441\u044F \u043D\u0430\u043B\u0435\u0436\u043D\u0438\u043C \u0447\u0438\u043D\u043E\u043C \u0437\u0430\u0432\u0430\u043D\u0442\u0430\u0436\u0438\u0442\u0438 \u0440\u043E\u0437\u043A\u043B\u0430\u0434\u043A\u0443 \u043A\u043B\u0430\u0432\u0456\u0430\u0442\u0443\u0440\u0438 %s.",
    L"\u0406\u0441\u043F\u0430\u043D\u0441\u044C\u043A\u0430 (\u0406\u0441\u043F\u0430\u043D\u0456\u044F)",
    L"\u0429\u043E\u0431 \u0437\u043C\u0456\u043D\u0438 \u043C\u043E\u0432\u0438 \u0456\u043D\u0442\u0435\u0440\u0444\u0435\u0439\u0441\u0443 \u043D\u0430\u0431\u0443\u043B\u0438 \u0447\u0438\u043D\u043D\u043E\u0441\u0442\u0456, \u043D\u0435\u043E\u0431\u0445\u0456\u0434\u043D\u043E \u0432\u0438\u0439\u0442\u0438 \u0437 \u0441\u0438\u0441\u0442\u0435\u043C\u0438",
    L"\u041F\u0435\u0440\u0435\u0434 \u0432\u0438\u0445\u043E\u0434\u043E\u043C \u0437 \u0441\u0438\u0441\u0442\u0435\u043C\u0438 \u0437\u0431\u0435\u0440\u0435\u0436\u0456\u0442\u044C \u0440\u043E\u0431\u043E\u0442\u0443 \u0442\u0430 \u0437\u0430\u043A\u0440\u0438\u0439\u0442\u0435 \u0432\u0441\u0456 \u0432\u0456\u0434\u043A\u0440\u0438\u0442\u0456 \u043F\u0440\u043E\u0433\u0440\u0430\u043C\u0438.",
    L"\u0412\u0438\u0439\u0442\u0438 \u0437\u0430\u0440\u0430\u0437",
    L"\u0421\u043A\u0430\u0441\u0443\u0432\u0430\u0442\u0438",
    L"\u0417\u043C\u0456\u043D\u0438\u0442\u0438 \u043C\u043E\u0432\u0443 \u0456\u043D\u0442\u0435\u0440\u0444\u0435\u0439\u0441\u0443",
    L"\u0420\u0435\u043A\u043E\u043C\u0435\u043D\u0434\u0443\u0454\u0442\u044C\u0441\u044F \u0437\u0430\u0441\u0442\u043E\u0441\u0443\u0432\u0430\u0442\u0438 \u0437\u043C\u0456\u043D\u0438 \u043F\u0435\u0440\u0435\u0434 \u0432\u043D\u0435\u0441\u0435\u043D\u043D\u044F\u043C \u0456\u043D\u0448\u0438\u0445 \u0441\u0438\u0441\u0442\u0435\u043C\u043D\u0438\u0445 \u0437\u043C\u0456\u043D, \u0449\u043E\u0431 \u0432\u043E\u043D\u0438 \u0432\u0456\u0434\u043E\u0431\u0440\u0430\u0437\u0438\u043B\u0438\u0441\u044C \u043D\u0430 \u043A\u043E\u043C\u043F\u2019\u044E\u0442\u0435\u0440\u0456.",
    L"\u0417\u0430\u0441\u0442\u043E\u0441\u0443\u0432\u0430\u0442\u0438",
    L"\u0421\u043A\u0430\u0441\u0443\u0432\u0430\u0442\u0438",
    L"\u041D\u0435 \u0432\u0434\u0430\u043B\u043E\u0441\u044F \u0432\u0438\u043A\u043E\u043D\u0430\u0442\u0438 \u0437\u0430\u0432\u0434\u0430\u043D\u043D\u044F",
    L"\u041F\u043E\u0442\u043E\u0447\u043D\u0438\u0439 \u043A\u043E\u0440\u0438\u0441\u0442\u0443\u0432\u0430\u0447",
    L"\u0415\u043A\u0440\u0430\u043D \u043F\u0440\u0438\u0432\u0456\u0442\u0430\u043D\u043D\u044F",
    L"\u041D\u043E\u0432\u0456 \u043E\u0431\u043B\u0456\u043A\u043E\u0432\u0456 \u0437\u0430\u043F\u0438\u0441\u0438",
    L"\u041C\u043E\u0432\u0430 \u0456\u043D\u0442\u0435\u0440\u0444\u0435\u0439\u0441\u0443:",
    L"\u041C\u043E\u0432\u0430 \u0432\u0432\u0435\u0434\u0435\u043D\u043D\u044F:",
    L"\u0424\u043E\u0440\u043C\u0430\u0442:",
    L"\u0420\u043E\u0437\u0442\u0430\u0448\u0443\u0432\u0430\u043D\u043D\u044F:",
    L"\u041D\u0435 \u0432\u0434\u0430\u043B\u043E\u0441\u044F \u043F\u0440\u043E\u0447\u0438\u0442\u0430\u0442\u0438 \u043F\u0430\u0440\u0430\u043C\u0435\u0442\u0440",
    L"\u041A\u043E\u043D\u0442\u0435\u043A\u0441\u0442",
    L"\u041D\u0456\u043A\u043E\u043B\u0438",
    L"\u041D\u0430\u0446\u0456\u043E\u043D\u0430\u043B\u044C\u043D\u0438\u0439",
};
static const wchar_t* const kDlgTr_UK[92] = {
    L"\u0424\u043E\u0440\u043C\u0430\u0442\u0438 \u0434\u0430\u0442\u0438 \u0442\u0430 \u0447\u0430\u0441\u0443",
    L"&\u041A\u043E\u0440\u043E\u0442\u043A\u0430 \u0434\u0430\u0442\u0430:",
    L"&\u041F\u043E\u0432\u043D\u0430 \u0434\u0430\u0442\u0430:",
    L"\u041A&\u043E\u0440\u043E\u0442\u043A\u0438\u0439 \u0447\u0430\u0441:",
    L"\u041F\u043E\u0432&\u043D\u0438\u0439 \u0447\u0430\u0441:",
    L"\u041F\u0435\u0440\u0448\u0438\u0439 &\u0434\u0435\u043D\u044C \u0442\u0438\u0436\u043D\u044F:",
    L"\u041F\u0440\u0438\u043A\u043B\u0430\u0434\u0438",
    L"\u041A\u043E\u0440\u043E\u0442\u043A\u0430 \u0434\u0430\u0442\u0430:",
    L"\u041F\u043E\u0432\u043D\u0430 \u0434\u0430\u0442\u0430:",
    L"\u041A\u043E\u0440\u043E\u0442\u043A\u0438\u0439 \u0447\u0430\u0441:",
    L"\u041F\u043E\u0432\u043D\u0438\u0439 \u0447\u0430\u0441:",
    L"\u0414\u043E\u0434\u0430\u0442\u043A\u043E\u0432&\u0456 \u043F\u0430\u0440\u0430\u043C\u0435\u0442\u0440\u0438...",
    L"\u041A\u043B\u0430\u0432\u0456\u0430\u0442\u0443\u0440\u0438 \u0442\u0430 \u0456\u043D\u0448\u0456 \u043C\u043E\u0432\u0438 \u0432\u0432\u0435\u0434\u0435\u043D\u043D\u044F",
    L"\u0429\u043E\u0431 \u0437\u043C\u0456\u043D\u0438\u0442\u0438 \u043A\u043B\u0430\u0432\u0456\u0430\u0442\u0443\u0440\u0443 \u0430\u0431\u043E \u043C\u043E\u0432\u0443 \u0432\u0432\u0435\u0434\u0435\u043D\u043D\u044F, \u043D\u0430\u0442\u0438\u0441\u043D\u0456\u0442\u044C \u043A\u043D\u043E\u043F\u043A\u0443 \u0417\u043C\u0456\u043D\u0438\u0442\u0438 \u043A\u043B\u0430\u0432\u0456\u0430\u0442\u0443\u0440\u0438.",
    L"&\u0417\u043C\u0456\u043D\u0438\u0442\u0438 \u043A\u043B\u0430\u0432\u0456\u0430\u0442\u0443\u0440\u0438...",
    L"\u041C\u043E\u0432\u0430 \u0456\u043D\u0442\u0435\u0440\u0444\u0435\u0439\u0441\u0443",
    L"\u0423\u0441\u0442\u0430\u043D\u043E\u0432\u0456\u0442\u044C \u0430\u0431\u043E \u0432\u0438\u0434\u0430\u043B\u0456\u0442\u044C \u043C\u043E\u0432\u0438, \u044F\u043A\u0456 Windows \u043C\u043E\u0436\u0435 \u0432\u0438\u043A\u043E\u0440\u0438\u0441\u0442\u043E\u0432\u0443\u0432\u0430\u0442\u0438 \u0434\u043B\u044F \u0432\u0456\u0434\u043E\u0431\u0440\u0430\u0436\u0435\u043D\u043D\u044F \u0442\u0435\u043A\u0441\u0442\u0443 \u0442\u0430, \u0434\u0435 \u043F\u0456\u0434\u0442\u0440\u0438\u043C\u0443\u0454\u0442\u044C\u0441\u044F, \u0440\u043E\u0437\u043F\u0456\u0437\u043D\u0430\u0432\u0430\u043D\u043D\u044F \u043C\u043E\u0432\u043B\u0435\u043D\u043D\u044F \u0442\u0430 \u0440\u0443\u043A\u043E\u043F\u0438\u0441\u043D\u043E\u0433\u043E \u0432\u0432\u0435\u0434\u0435\u043D\u043D\u044F.",
    L"&\u0423\u0441\u0442\u0430\u043D\u043E\u0432\u0438\u0442\u0438 \u0430\u0431\u043E \u0432\u0438\u0434\u0430\u043B\u0438\u0442\u0438 \u043C\u043E\u0432\u0438...",
    L"\u0413\u0456\u0441\u0442\u044C \u043D\u0435 \u043C\u043E\u0436\u0435 \u0437\u043C\u0456\u043D\u0438\u0442\u0438 \u043C\u043E\u0432\u0443 \u0456\u043D\u0442\u0435\u0440\u0444\u0435\u0439\u0441\u0443:",
    L"\u0412\u0438\u0431\u0456\u0440 \u043C\u043E\u0432\u0438 \u0456\u043D\u0442\u0435\u0440\u0444\u0435\u0439\u0441\u0443 \u0437\u0430\u0431\u043B\u043E\u043A\u043E\u0432\u0430\u043D\u043E \u0433\u0440\u0443\u043F\u043E\u0432\u043E\u044E \u043F\u043E\u043B\u0456\u0442\u0438\u043A\u043E\u044E.",
    L"&\u0412\u0438\u0431\u0435\u0440\u0456\u0442\u044C \u043C\u043E\u0432\u0443 \u0456\u043D\u0442\u0435\u0440\u0444\u0435\u0439\u0441\u0443:",
    L"&\u0427\u0430\u0441\u0442\u0438\u043D\u0443 \u0442\u0435\u043A\u0441\u0442\u0443 \u043D\u0435 \u043B\u043E\u043A\u0430\u043B\u0456\u0437\u043E\u0432\u0430\u043D\u043E \u0432\u0438\u0431\u0440\u0430\u043D\u043E\u044E \u043C\u043E\u0432\u043E\u044E. \u0412\u0438\u0431\u0435\u0440\u0456\u0442\u044C \u0456\u043D\u0448\u0443 \u043C\u043E\u0432\u0443, \u044F\u043A\u0443 Windows \u0432\u0438\u043A\u043E\u0440\u0438\u0441\u0442\u043E\u0432\u0443\u0432\u0430\u0442\u0438\u043C\u0435 \u0434\u043B\u044F \u0432\u0456\u0434\u043E\u0431\u0440\u0430\u0436\u0435\u043D\u043D\u044F \u0446\u044C\u043E\u0433\u043E \u0442\u0435\u043A\u0441\u0442\u0443:",
    L"\u0426\u044F \u043C\u043E\u0432\u0430 \u043B\u043E\u043A\u0430\u043B\u0456\u0437\u043E\u0432\u0430\u043D\u0430 \u043B\u0438\u0448\u0435 \u0447\u0430\u0441\u0442\u043A\u043E\u0432\u043E, \u0442\u043E\u043C\u0443 \u0447\u0430\u0441\u0442\u0438\u043D\u0430 \u0442\u0435\u043A\u0441\u0442\u0443 \u043C\u043E\u0436\u0435 \u0432\u0456\u0434\u043E\u0431\u0440\u0430\u0436\u0430\u0442\u0438\u0441\u044F \u043C\u043E\u0432\u043E\u044E:",
    L"&\u0426\u044F \u043C\u043E\u0432\u0430 \u0442\u0430\u043A\u043E\u0436 \u043B\u043E\u043A\u0430\u043B\u0456\u0437\u043E\u0432\u0430\u043D\u0430 \u043B\u0438\u0448\u0435 \u0447\u0430\u0441\u0442\u043A\u043E\u0432\u043E. \u0412\u0438\u0431\u0435\u0440\u0456\u0442\u044C \u0442\u0440\u0435\u0442\u044E \u043C\u043E\u0432\u0443, \u044F\u043A\u0443 Windows \u0432\u0438\u043A\u043E\u0440\u0438\u0441\u0442\u043E\u0432\u0443\u0432\u0430\u0442\u0438\u043C\u0435 \u0434\u043B\u044F \u0432\u0456\u0434\u043E\u0431\u0440\u0430\u0436\u0435\u043D\u043D\u044F \u0440\u0435\u0448\u0442\u0438 \u0442\u0435\u043A\u0441\u0442\u0443:",
    L"\u0426\u044F \u043C\u043E\u0432\u0430 \u0442\u0430\u043A\u043E\u0436 \u043B\u043E\u043A\u0430\u043B\u0456\u0437\u043E\u0432\u0430\u043D\u0430 \u043B\u0438\u0448\u0435 \u0447\u0430\u0441\u0442\u043A\u043E\u0432\u043E, \u0442\u043E\u043C\u0443 \u0447\u0430\u0441\u0442\u0438\u043D\u0430 \u0442\u0435\u043A\u0441\u0442\u0443 \u043C\u043E\u0436\u0435 \u0432\u0456\u0434\u043E\u0431\u0440\u0430\u0436\u0430\u0442\u0438\u0441\u044F \u043C\u043E\u0432\u043E\u044E: ",
    L"\u0415\u043A\u0440\u0430\u043D \u043F\u0440\u0438\u0432\u0456\u0442\u0430\u043D\u043D\u044F \u0442\u0430 \u043D\u043E\u0432\u0456 \u043E\u0431\u043B\u0456\u043A\u043E\u0432\u0456 \u0437\u0430\u043F\u0438\u0441\u0438",
    L"\u041F\u0435\u0440\u0435\u0433\u043B\u044F\u043D\u044C\u0442\u0435 \u0442\u0430 \u0441\u043A\u043E\u043F\u0456\u044E\u0439\u0442\u0435 \u0441\u0432\u043E\u0457 \u0440\u0435\u0433\u0456\u043E\u043D\u0430\u043B\u044C\u043D\u0456 \u043F\u0430\u0440\u0430\u043C\u0435\u0442\u0440\u0438 \u043D\u0430 \u0435\u043A\u0440\u0430\u043D \u043F\u0440\u0438\u0432\u0456\u0442\u0430\u043D\u043D\u044F, \u0441\u0438\u0441\u0442\u0435\u043C\u043D\u0456 \u043E\u0431\u043B\u0456\u043A\u043E\u0432\u0456 \u0437\u0430\u043F\u0438\u0441\u0438 \u0442\u0430 \u043D\u043E\u0432\u0456 \u043E\u0431\u043B\u0456\u043A\u043E\u0432\u0456 \u0437\u0430\u043F\u0438\u0441\u0438.",
    L"&\u041A\u043E\u043F\u0456\u044E\u0432\u0430\u0442\u0438 \u043F\u0430\u0440\u0430\u043C\u0435\u0442\u0440\u0438...",
    L"\u041C\u043E\u0432\u0430 \u043F\u0440\u043E\u0433\u0440\u0430\u043C, \u0449\u043E \u043D\u0435 \u043F\u0456\u0434\u0442\u0440\u0438\u043C\u0443\u044E\u0442\u044C \u042E\u043D\u0456\u043A\u043E\u0434",
    L"\u0426\u0435\u0439 \u043F\u0430\u0440\u0430\u043C\u0435\u0442\u0440 (\u043C\u043E\u0432\u0430 \u0441\u0438\u0441\u0442\u0435\u043C\u0438) \u0432\u0438\u0437\u043D\u0430\u0447\u0430\u0454 \u043C\u043E\u0432\u0443, \u044F\u043A\u0430 \u0432\u0438\u043A\u043E\u0440\u0438\u0441\u0442\u043E\u0432\u0443\u0454\u0442\u044C\u0441\u044F \u043F\u0456\u0434 \u0447\u0430\u0441 \u0432\u0456\u0434\u043E\u0431\u0440\u0430\u0436\u0435\u043D\u043D\u044F \u0442\u0435\u043A\u0441\u0442\u0443 \u0432 \u043F\u0440\u043E\u0433\u0440\u0430\u043C\u0430\u0445, \u0449\u043E \u043D\u0435 \u043F\u0456\u0434\u0442\u0440\u0438\u043C\u0443\u044E\u0442\u044C \u042E\u043D\u0456\u043A\u043E\u0434.",
    L"\u041F\u043E\u0442\u043E\u0447\u043D\u0430 \u043C\u043E\u0432\u0430 \u043F\u0440\u043E\u0433\u0440\u0430\u043C, \u0449\u043E \u043D\u0435 \u043F\u0456\u0434\u0442\u0440\u0438\u043C\u0443\u044E\u0442\u044C \u042E\u043D\u0456\u043A\u043E\u0434:",
    nullptr,
    L"&\u041C\u043E\u0432\u0430 \u0441\u0438\u0441\u0442\u0435\u043C\u0438...",
    L"\u041F\u0440\u0438\u043A\u043B\u0430\u0434",
    L"\u0414\u043E\u0434\u0430\u0442\u043D\u0456:",
    L"\u0412\u0456\u0434\u2019\u0454\u043C\u043D\u0456:",
    L"&\u0414\u0435\u0441\u044F\u0442\u043A\u043E\u0432\u0438\u0439 \u0440\u043E\u0437\u0434\u0456\u043B\u044C\u043D\u0438\u043A:",
    L"&\u041A\u0456\u043B\u044C\u043A\u0456\u0441\u0442\u044C \u0434\u0435\u0441\u044F\u0442\u043A\u043E\u0432\u0438\u0445 \u0437\u043D\u0430\u043A\u0456\u0432:",
    L"\u0420\u043E\u0437\u0434\u0456\u043B\u044C\u043D\u0438\u043A &\u0433\u0440\u0443\u043F:",
    L"\u0413&\u0440\u0443\u043F\u0443\u0432\u0430\u043D\u043D\u044F:",
    L"\u0417\u043D\u0430\u043A &\u043C\u0456\u043D\u0443\u0441\u0430:",
    L"\u0424\u043E\u0440\u043C\u0430\u0442 &\u0432\u0456\u0434\u2019\u0454\u043C\u043D\u0438\u0445 \u0447\u0438\u0441\u0435\u043B:",
    L"\u0412\u0456\u0434\u043E\u0431\u0440\u0430\u0436\u0430\u0442\u0438 &\u043D\u0443\u043B\u0456 \u043D\u0430 \u043F\u043E\u0447\u0430\u0442\u043A\u0443:",
    L"\u0420\u043E\u0437\u0434\u0456\u043B\u044C\u043D\u0438&\u043A \u0441\u043F\u0438\u0441\u043A\u0443:",
    L"&\u0421\u0438\u0441\u0442\u0435\u043C\u0430 \u043C\u0456\u0440:",
    L"\u0421\u0442\u0430\u043D\u0434\u0430\u0440\u0442\u043D&\u0456 \u0446\u0438\u0444\u0440\u0438:",
    L"&\u0412\u0438\u043A\u043E\u0440\u0438\u0441\u0442\u043E\u0432\u0443\u0432\u0430\u0442\u0438 \u043D\u0430\u0446\u0456\u043E\u043D\u0430\u043B\u044C\u043D\u0456 \u0446\u0438\u0444\u0440\u0438:",
    L"\u041D\u0430\u0442\u0438\u0441\u043D\u0456\u0442\u044C \u043A\u043D\u043E\u043F\u043A\u0443 \u0421\u043A\u0438\u043D\u0443\u0442\u0438, \u0449\u043E\u0431 \u0432\u0456\u0434\u043D\u043E\u0432\u0438\u0442\u0438 \u0441\u0438\u0441\u0442\u0435\u043C\u043D\u0456 \u043F\u0430\u0440\u0430\u043C\u0435\u0442\u0440\u0438 \u0437\u0430 \u0437\u0430\u043C\u043E\u0432\u0447\u0443\u0432\u0430\u043D\u043D\u044F\u043C \u0434\u043B\u044F \u0447\u0438\u0441\u0435\u043B, \u0432\u0430\u043B\u044E\u0442\u0438, \u0447\u0430\u0441\u0443 \u0442\u0430 \u0434\u0430\u0442\u0438.",
    L"\u0421&\u043A\u0438\u043D\u0443\u0442\u0438",
    L"&\u0421\u0438\u043C\u0432\u043E\u043B \u0432\u0430\u043B\u044E\u0442\u0438:",
    L"&\u0414\u043E\u0434\u0430\u0442\u043D\u0438\u0439 \u0444\u043E\u0440\u043C\u0430\u0442 \u0432\u0430\u043B\u044E\u0442\u0438:",
    L"\u0412\u0456\u0434\u2019\u0454\u043C\u043D\u0438\u0439 \u0444\u043E\u0440\u043C\u0430\u0442 \u0432\u0430\u043B\u044E\u0442&\u0438:",
    L"\u041A\u0456\u043B\u044C\u043A\u0456\u0441\u0442\u044C \u0434\u0435\u0441\u044F\u0442\u043A\u043E\u0432\u0438\u0445 \u0437&\u043D\u0430\u043A\u0456\u0432:",
    L"\u0420\u043E\u0437\u0434\u0456\u043B\u044C\u043D\u0438\u043A \u0433&\u0440\u0443\u043F:",
    L"\u0413\u0440\u0443\u043F\u0443\u0432\u0430\u043D\u043D&\u044F:",
    L"\u0424\u043E\u0440\u043C\u0430\u0442\u0438 \u0447\u0430\u0441\u0443",
    L"&\u041A\u043E\u0440\u043E\u0442\u043A\u0438\u0439 \u0447\u0430\u0441:",
    L"&\u041F\u043E\u0432\u043D\u0438\u0439 \u0447\u0430\u0441:",
    L"&\u0421\u0438\u043C\u0432\u043E\u043B AM:",
    L"\u0421\u0438\u043C&\u0432\u043E\u043B PM:",
    L"\u0417\u043D\u0430\u0447\u0435\u043D\u043D\u044F \u043F\u043E\u0437\u043D\u0430\u0447\u0435\u043D\u044C:\n\nh = \u0433\u043E\u0434\u0438\u043D\u0430   m = \u0445\u0432\u0438\u043B\u0438\u043D\u0430\ns = \u0441\u0435\u043A\u0443\u043D\u0434\u0430 (\u043B\u0438\u0448\u0435 \u043F\u043E\u0432\u043D\u0438\u0439 \u0447\u0430\u0441)\ntt = AM \u0430\u0431\u043E PM\n\nh/H = 12/24 \u0433\u043E\u0434\u0438\u043D\u0438\n\nhh, mm, ss = \u0432\u0456\u0434\u043E\u0431\u0440\u0430\u0436\u0430\u0442\u0438 \u043D\u0443\u043B\u044C \u043D\u0430 \u043F\u043E\u0447\u0430\u0442\u043A\u0443\nh, m, s = \u043D\u0435 \u0432\u0456\u0434\u043E\u0431\u0440\u0430\u0436\u0430\u0442\u0438 \u043D\u0443\u043B\u044C \u043D\u0430 \u043F\u043E\u0447\u0430\u0442\u043A\u0443",
    L"\u0424\u043E\u0440\u043C\u0430\u0442\u0438 \u0434\u0430\u0442\u0438",
    L"\u0417\u043D\u0430\u0447\u0435\u043D\u043D\u044F \u043F\u043E\u0437\u043D\u0430\u0447\u0435\u043D\u044C:\nd, dd = \u0434\u0435\u043D\u044C;  ddd, dddd = \u0434\u0435\u043D\u044C \u0442\u0438\u0436\u043D\u044F;  M = \u043C\u0456\u0441\u044F\u0446\u044C;  y = \u0440\u0456\u043A",
    L"\u041A\u0430\u043B\u0435\u043D\u0434\u0430\u0440",
    L"\u042F\u043A\u0449\u043E \u0432\u0432\u0435\u0434\u0435\u043D\u043E \u0440\u0456\u043A \u0456\u0437 \u0434\u0432\u043E\u0445 \u0446\u0438\u0444\u0440, \u0432\u0432\u0430\u0436\u0430\u0442\u0438 \u0439\u043E\u0433\u043E \u0440\u043E\u043A\u043E\u043C &\u043C\u0456\u0436:",
    L"\u0442\u0430",
    L"\u041F\u0435\u0440\u0448\u0438\u0439 \u0434\u0435&\u043D\u044C \u0442\u0438\u0436\u043D\u044F:",
    L"&\u0422\u0438\u043F \u043A\u0430\u043B\u0435\u043D\u0434\u0430\u0440\u044F:",
    L"\u041F\u043E\u043F\u0440\u0430\u0432\u043A\u0430 \u0434\u0430\u0442\u0438 &\u0445\u0456\u0434\u0436\u0440\u0438:",
    L"\u041C\u043E\u0436\u043D\u0430 \u043A\u0435\u0440\u0443\u0432\u0430\u0442\u0438 \u0441\u043F\u043E\u0441\u043E\u0431\u043E\u043C \u0441\u043E\u0440\u0442\u0443\u0432\u0430\u043D\u043D\u044F \u0441\u0438\u043C\u0432\u043E\u043B\u0456\u0432, \u0441\u043B\u0456\u0432, \u0444\u0430\u0439\u043B\u0456\u0432 \u0456 \u043F\u0430\u043F\u043E\u043A \u0434\u0435\u044F\u043A\u0438\u043C\u0438 \u043F\u0440\u043E\u0433\u0440\u0430\u043C\u0430\u043C\u0438.",
    L"&\u0412\u0438\u0431\u0435\u0440\u0456\u0442\u044C \u043C\u0435\u0442\u043E\u0434 \u0441\u043E\u0440\u0442\u0443\u0432\u0430\u043D\u043D\u044F:",
    L"\u0414\u0435\u044F\u043A\u0456 \u043F\u0440\u043E\u0433\u0440\u0430\u043C\u0438, \u0437\u043E\u043A\u0440\u0435\u043C\u0430 Windows, \u043C\u043E\u0436\u0443\u0442\u044C \u043D\u0430\u0434\u0430\u0432\u0430\u0442\u0438 \u0434\u043E\u0434\u0430\u0442\u043A\u043E\u0432\u0438\u0439 \u0432\u043C\u0456\u0441\u0442 \u0434\u043B\u044F \u043F\u0435\u0432\u043D\u043E\u0433\u043E \u0440\u043E\u0437\u0442\u0430\u0448\u0443\u0432\u0430\u043D\u043D\u044F. \u0414\u0435\u044F\u043A\u0456 \u0441\u043B\u0443\u0436\u0431\u0438 \u043D\u0430\u0434\u0430\u044E\u0442\u044C \u043C\u0456\u0441\u0446\u0435\u0432\u0456 \u0432\u0456\u0434\u043E\u043C\u043E\u0441\u0442\u0456, \u043D\u0430\u043F\u0440\u0438\u043A\u043B\u0430\u0434 \u043D\u043E\u0432\u0438\u043D\u0438 \u0442\u0430 \u043F\u043E\u0433\u043E\u0434\u0443.",
    L"&\u041F\u043E\u0442\u043E\u0447\u043D\u0435 \u0440\u043E\u0437\u0442\u0430\u0448\u0443\u0432\u0430\u043D\u043D\u044F:",
    L"\u0414\u0438\u0432. \u0442\u0430\u043A\u043E\u0436",
    L"&\u041F\u0430\u0440\u0430\u043C\u0435\u0442\u0440\u0438 \u043F\u043E\u0442\u043E\u0447\u043D\u043E\u0433\u043E \u043A\u043E\u0440\u0438\u0441\u0442\u0443\u0432\u0430\u0447\u0430, \u0435\u043A\u0440\u0430\u043D\u0430 \u043F\u0440\u0438\u0432\u0456\u0442\u0430\u043D\u043D\u044F (\u0441\u0438\u0441\u0442\u0435\u043C\u043D\u0438\u0445 \u043E\u0431\u043B\u0456\u043A\u043E\u0432\u0438\u0445 \u0437\u0430\u043F\u0438\u0441\u0456\u0432) \u0442\u0430 \u043D\u043E\u0432\u0438\u0445 \u043E\u0431\u043B\u0456\u043A\u043E\u0432\u0438\u0445 \u0437\u0430\u043F\u0438\u0441\u0456\u0432 \u043F\u043E\u043A\u0430\u0437\u0430\u043D\u043E \u043D\u0438\u0436\u0447\u0435.",
    L"* \u041D\u0430\u043B\u0430\u0448\u0442\u043E\u0432\u0430\u043D\u0430 \u043C\u043E\u0432\u0430",
    L"\u0421\u043A\u043E\u043F\u0456\u044E\u0432\u0430\u0442\u0438 \u043F\u043E\u0442\u043E\u0447\u043D\u0456 \u043F\u0430\u0440\u0430\u043C\u0435\u0442\u0440\u0438 \u0432:",
    L"&\u0415\u043A\u0440\u0430\u043D \u043F\u0440\u0438\u0432\u0456\u0442\u0430\u043D\u043D\u044F \u0442\u0430 \u0441\u0438\u0441\u0442\u0435\u043C\u043D\u0456 \u043E\u0431\u043B\u0456\u043A\u043E\u0432\u0456 \u0437\u0430\u043F\u0438\u0441\u0438",
    L"&\u041D\u043E\u0432\u0456 \u043E\u0431\u043B\u0456\u043A\u043E\u0432\u0456 \u0437\u0430\u043F\u0438\u0441\u0438",
    L"\u041C\u043E\u0432\u0430 \u0456\u043D\u0442\u0435\u0440\u0444\u0435\u0439\u0441\u0443 \u043D\u043E\u0432\u0438\u0445 \u043E\u0431\u043B\u0456\u043A\u043E\u0432\u0438\u0445 \u0437\u0430\u043F\u0438\u0441\u0456\u0432 \u043D\u0430\u0440\u0430\u0437\u0456 \u0443\u0441\u043F\u0430\u0434\u043A\u043E\u0432\u0443\u0454\u0442\u044C\u0441\u044F \u0432\u0456\u0434 \u043C\u043E\u0432\u0438 \u0456\u043D\u0442\u0435\u0440\u0444\u0435\u0439\u0441\u0443 \u0435\u043A\u0440\u0430\u043D\u0430 \u043F\u0440\u0438\u0432\u0456\u0442\u0430\u043D\u043D\u044F.",
    nullptr,
    L"\u0421\u043A\u0430\u0441\u0443\u0432\u0430\u0442\u0438",
    L"\u0412\u0438\u0431\u0435\u0440\u0456\u0442\u044C \u043C\u043E\u0432\u0443 (\u043C\u043E\u0432\u0443 \u0441\u0438\u0441\u0442\u0435\u043C\u0438), \u044F\u043A\u0430 \u0432\u0438\u043A\u043E\u0440\u0438\u0441\u0442\u043E\u0432\u0443\u0454\u0442\u044C\u0441\u044F \u043F\u0456\u0434 \u0447\u0430\u0441 \u0432\u0456\u0434\u043E\u0431\u0440\u0430\u0436\u0435\u043D\u043D\u044F \u0442\u0435\u043A\u0441\u0442\u0443 \u0432 \u043F\u0440\u043E\u0433\u0440\u0430\u043C\u0430\u0445, \u0449\u043E \u043D\u0435 \u043F\u0456\u0434\u0442\u0440\u0438\u043C\u0443\u044E\u0442\u044C \u042E\u043D\u0456\u043A\u043E\u0434. \u0426\u0435\u0439 \u043F\u0430\u0440\u0430\u043C\u0435\u0442\u0440 \u0432\u043F\u043B\u0438\u0432\u0430\u0454 \u043D\u0430 \u0432\u0441\u0456 \u043E\u0431\u043B\u0456\u043A\u043E\u0432\u0456 \u0437\u0430\u043F\u0438\u0441\u0438 \u043A\u043E\u043C\u043F\u2019\u044E\u0442\u0435\u0440\u0430.",
    L"&\u041F\u043E\u0442\u043E\u0447\u043D\u0430 \u043C\u043E\u0432\u0430 \u0441\u0438\u0441\u0442\u0435\u043C\u0438:",
    L"<A>\u0417\u043C\u0456\u043D\u0438\u0442\u0438 \u043C\u0435\u0442\u043E\u0434 \u0441\u043E\u0440\u0442\u0443\u0432\u0430\u043D\u043D\u044F</A>",
    L"<A>\u0429\u043E \u043E\u0437\u043D\u0430\u0447\u0430\u0454 \u043F\u043E\u0437\u043D\u0430\u0447\u0435\u043D\u043D\u044F?</A>",
    L"<A>\u0414\u043E\u043A\u043B\u0430\u0434\u043D\u0456\u0448\u0435 \u0432 \u0406\u043D\u0442\u0435\u0440\u043D\u0435\u0442\u0456 \u043F\u0440\u043E \u0437\u043C\u0456\u043D\u0443 \u043C\u043E\u0432 \u0442\u0430 \u0440\u0435\u0433\u0456\u043E\u043D\u0430\u043B\u044C\u043D\u0438\u0445 \u0444\u043E\u0440\u043C\u0430\u0442\u0456\u0432</A>",
    L"<A>\u042F\u043A \u0437\u043C\u0456\u043D\u0438\u0442\u0438 \u0440\u043E\u0437\u043A\u043B\u0430\u0434\u043A\u0443 \u043A\u043B\u0430\u0432\u0456\u0430\u0442\u0443\u0440\u0438 \u0434\u043B\u044F \u0435\u043A\u0440\u0430\u043D\u0430 \u043F\u0440\u0438\u0432\u0456\u0442\u0430\u043D\u043D\u044F?</A>",
    L"<A>\u042F\u043A \u0443\u0441\u0442\u0430\u043D\u043E\u0432\u0438\u0442\u0438 \u0434\u043E\u0434\u0430\u0442\u043A\u043E\u0432\u0456 \u043C\u043E\u0432\u0438?</A>",
    L"<A>\u0414\u043E\u043A\u043B\u0430\u0434\u043D\u0456\u0448\u0435 \u043F\u0440\u043E \u0446\u0456 \u043E\u0431\u043B\u0456\u043A\u043E\u0432\u0456 \u0437\u0430\u043F\u0438\u0441\u0438</A>",
    L"<A>\u0429\u043E \u0442\u0430\u043A\u0435 \u043C\u043E\u0432\u0430 \u0441\u0438\u0441\u0442\u0435\u043C\u0438?</A>",
    L"<A>\u0420\u043E\u0437\u0442\u0430\u0448\u0443\u0432\u0430\u043D\u043D\u044F \u0437\u0430 \u0437\u0430\u043C\u043E\u0432\u0447\u0443\u0432\u0430\u043D\u043D\u044F\u043C</A>",
};
static const wchar_t* const kTitleTr_UK[11] = {
    L"\u0424\u043E\u0440\u043C\u0430\u0442\u0438",
    L"\u041A\u043B\u0430\u0432\u0456\u0430\u0442\u0443\u0440\u0438 \u0442\u0430 \u043C\u043E\u0432\u0438",
    L"\u0410\u0434\u043C\u0456\u043D\u0456\u0441\u0442\u0440\u0443\u0432\u0430\u043D\u043D\u044F",
    L"\u0427\u0438\u0441\u043B\u0430",
    L"\u0412\u0430\u043B\u044E\u0442\u0430",
    L"\u0427\u0430\u0441",
    L"\u0414\u0430\u0442\u0430",
    L"\u0421\u043E\u0440\u0442\u0443\u0432\u0430\u043D\u043D\u044F",
    L"\u0420\u043E\u0437\u0442\u0430\u0448\u0443\u0432\u0430\u043D\u043D\u044F",
    L"\u041F\u0430\u0440\u0430\u043C\u0435\u0442\u0440\u0438 \u0435\u043A\u0440\u0430\u043D\u0430 \u043F\u0440\u0438\u0432\u0456\u0442\u0430\u043D\u043D\u044F \u0442\u0430 \u043D\u043E\u0432\u0438\u0445 \u043E\u0431\u043B\u0456\u043A\u043E\u0432\u0438\u0445 \u0437\u0430\u043F\u0438\u0441\u0456\u0432",
    L"\u0420\u0435\u0433\u0456\u043E\u043D \u0456 \u043C\u043E\u0432\u0430",
};
// ================= \u0395\u039B\u039B\u0397\u039D\u0399\u039A\u0386 (el-GR) =================
static const wchar_t* const kStrTr_EL[66] = {
    L"\u03A0\u03B5\u03C1\u03B9\u03BF\u03C7\u03AE \u03BA\u03B1\u03B9 \u03B3\u03BB\u03CE\u03C3\u03C3\u03B1",
    L"\u0394\u03B9\u03B1\u03BC\u03CC\u03C1\u03C6\u03C9\u03C3\u03B7 \u03C4\u03C9\u03BD \u03C1\u03C5\u03B8\u03BC\u03AF\u03C3\u03B5\u03C9\u03BD \u03B3\u03B9\u03B1 \u03C4\u03BF\u03BD \u03C4\u03C1\u03CC\u03C0\u03BF \u03B5\u03BC\u03C6\u03AC\u03BD\u03B9\u03C3\u03B7\u03C2 \u03C4\u03C9\u03BD \u03B3\u03BB\u03C9\u03C3\u03C3\u03CE\u03BD, \u03C4\u03C9\u03BD \u03B1\u03C1\u03B9\u03B8\u03BC\u03CE\u03BD, \u03C4\u03C9\u03BD \u03C9\u03C1\u03CE\u03BD \u03BA\u03B1\u03B9 \u03C4\u03C9\u03BD \u03B7\u03BC\u03B5\u03C1\u03BF\u03BC\u03B7\u03BD\u03B9\u03CE\u03BD.",
    L"\u0391\u03BB\u03BB\u03B1\u03B3\u03AE \u03BC\u03BF\u03C1\u03C6\u03AE\u03C2",
    L"\u039C\u03AF\u03B1 \u03AE \u03C0\u03B5\u03C1\u03B9\u03C3\u03C3\u03CC\u03C4\u03B5\u03C1\u03B5\u03C2 \u03C4\u03BF\u03C0\u03B9\u03BA\u03AD\u03C2 \u03C1\u03C5\u03B8\u03BC\u03AF\u03C3\u03B5\u03B9\u03C2 \u03B4\u03B5\u03BD \u03B5\u03AF\u03BD\u03B1\u03B9 \u03AD\u03B3\u03BA\u03C5\u03C1\u03B5\u03C2. \u0395\u03BB\u03AD\u03B3\u03BE\u03C4\u03B5 \u03BA\u03B1\u03B9 \u03B4\u03B9\u03BF\u03C1\u03B8\u03CE\u03C3\u03C4\u03B5 \u03C4\u03B9\u03C2 \u03C0\u03C1\u03BF\u03C3\u03B1\u03C1\u03BC\u03BF\u03C3\u03BC\u03AD\u03BD\u03B5\u03C2 \u03C1\u03C5\u03B8\u03BC\u03AF\u03C3\u03B5\u03B9\u03C2 \u03B3\u03B9\u03B1 \u03BD\u03B1 \u03B5\u03C0\u03B9\u03BB\u03C5\u03B8\u03B5\u03AF \u03C4\u03BF \u03C0\u03C1\u03CC\u03B2\u03BB\u03B7\u03BC\u03B1.",
    nullptr,
    nullptr,
    L"\u039C\u03B5\u03C4\u03C1\u03B9\u03BA\u03CC",
    nullptr,
    L"\u0388\u03BD\u03B1\u03C2 \u03AE \u03C0\u03B5\u03C1\u03B9\u03C3\u03C3\u03CC\u03C4\u03B5\u03C1\u03BF\u03B9 \u03B1\u03C0\u03CC \u03C4\u03BF\u03C5\u03C2 \u03C7\u03B1\u03C1\u03B1\u03BA\u03C4\u03AE\u03C1\u03B5\u03C2 \u03C0\u03BF\u03C5 \u03B5\u03B9\u03C3\u03B1\u03B3\u03AC\u03B3\u03B1\u03C4\u03B5 \u03C3\u03B5 \u03B1\u03C5\u03C4\u03CC \u03C4\u03BF \u03C0\u03B5\u03B4\u03AF\u03BF \u03B4\u03B5\u03BD \u03B5\u03AF\u03BD\u03B1\u03B9 \u03AD\u03B3\u03BA\u03C5\u03C1\u03BF\u03B9. \u0394\u03BF\u03BA\u03B9\u03BC\u03AC\u03C3\u03C4\u03B5 \u03BD\u03B1 \u03C7\u03C1\u03B7\u03C3\u03B9\u03BC\u03BF\u03C0\u03BF\u03B9\u03AE\u03C3\u03B5\u03C4\u03B5 \u03AC\u03BB\u03BB\u03BF\u03C5\u03C2 \u03C7\u03B1\u03C1\u03B1\u03BA\u03C4\u03AE\u03C1\u03B5\u03C2.",
    L"\u0388\u03BD\u03B1\u03C2 \u03AE \u03C0\u03B5\u03C1\u03B9\u03C3\u03C3\u03CC\u03C4\u03B5\u03C1\u03BF\u03B9 \u03B1\u03C0\u03CC \u03C4\u03BF\u03C5\u03C2 \u03C7\u03B1\u03C1\u03B1\u03BA\u03C4\u03AE\u03C1\u03B5\u03C2 \u03C0\u03BF\u03C5 \u03B5\u03B9\u03C3\u03B1\u03B3\u03AC\u03B3\u03B1\u03C4\u03B5 \u03B3\u03B9\u03B1 \u03C4\u03BF %s \u03B4\u03B5\u03BD \u03B5\u03AF\u03BD\u03B1\u03B9 \u03AD\u03B3\u03BA\u03C5\u03C1\u03BF\u03B9.  \u0394\u03BF\u03BA\u03B9\u03BC\u03AC\u03C3\u03C4\u03B5 \u03BD\u03B1 \u03C7\u03C1\u03B7\u03C3\u03B9\u03BC\u03BF\u03C0\u03BF\u03B9\u03AE\u03C3\u03B5\u03C4\u03B5 \u03B4\u03B9\u03B1\u03C6\u03BF\u03C1\u03B5\u03C4\u03B9\u03BA\u03CC \u03C7\u03B1\u03C1\u03B1\u03BA\u03C4\u03AE\u03C1\u03B1 \u03AE \u03BD\u03B1 \u03B5\u03B9\u03C3\u03B1\u03B3\u03AC\u03B3\u03B5\u03C4\u03B5 \u03AD\u03BD\u03B1 \u03BA\u03B5\u03BD\u03CC \u03B4\u03B9\u03AC\u03C3\u03C4\u03B7\u03BC\u03B1.",
    L"\u03A5\u03C0\u03BF\u03B4\u03B9\u03B1\u03C3\u03C4\u03BF\u03BB\u03AE",
    L"\u03A3\u03CD\u03BC\u03B2\u03BF\u03BB\u03BF \u03BC\u03B5\u03AF\u03BF\u03BD",
    L"\u03A3\u03CD\u03BC\u03B2\u03BF\u03BB\u03BF \u03BF\u03BC\u03B1\u03B4\u03BF\u03C0\u03BF\u03AF\u03B7\u03C3\u03B7\u03C2 \u03C8\u03B7\u03C6\u03AF\u03C9\u03BD",
    L"\u03A3\u03CD\u03BC\u03B2\u03BF\u03BB\u03BF \u03C0.\u03BC.",
    L"\u03A3\u03CD\u03BC\u03B2\u03BF\u03BB\u03BF \u03BC.\u03BC.",
    L"\u03A3\u03CD\u03BC\u03B2\u03BF\u03BB\u03BF \u03BD\u03BF\u03BC\u03AF\u03C3\u03BC\u03B1\u03C4\u03BF\u03C2",
    L"\u03A5\u03C0\u03BF\u03B4\u03B9\u03B1\u03C3\u03C4\u03BF\u03BB\u03AE \u03BD\u03BF\u03BC\u03AF\u03C3\u03BC\u03B1\u03C4\u03BF\u03C2",
    L"\u03A3\u03CD\u03BC\u03B2\u03BF\u03BB\u03BF \u03BF\u03BC\u03B1\u03B4\u03BF\u03C0\u03BF\u03AF\u03B7\u03C3\u03B7\u03C2 \u03C8\u03B7\u03C6\u03AF\u03C9\u03BD \u03BD\u03BF\u03BC\u03AF\u03C3\u03BC\u03B1\u03C4\u03BF\u03C2",
    L"\u0388\u03BD\u03B1\u03C2 \u03AE \u03C0\u03B5\u03C1\u03B9\u03C3\u03C3\u03CC\u03C4\u03B5\u03C1\u03BF\u03B9 \u03B1\u03C0\u03CC \u03C4\u03BF\u03C5\u03C2 \u03C7\u03B1\u03C1\u03B1\u03BA\u03C4\u03AE\u03C1\u03B5\u03C2 \u03C0\u03BF\u03C5 \u03B5\u03B9\u03C3\u03B1\u03B3\u03AC\u03B3\u03B1\u03C4\u03B5 \u03B3\u03B9\u03B1 \u03C4\u03B7 \u03BC\u03BF\u03C1\u03C6\u03AE %s \u03B4\u03B5\u03BD \u03B5\u03AF\u03BD\u03B1\u03B9 \u03AD\u03B3\u03BA\u03C5\u03C1\u03BF\u03B9. \u0394\u03BF\u03BA\u03B9\u03BC\u03AC\u03C3\u03C4\u03B5 \u03BD\u03B1 \u03C7\u03C1\u03B7\u03C3\u03B9\u03BC\u03BF\u03C0\u03BF\u03B9\u03AE\u03C3\u03B5\u03C4\u03B5 \u03AC\u03BB\u03BB\u03BF\u03C5\u03C2 \u03C7\u03B1\u03C1\u03B1\u03BA\u03C4\u03AE\u03C1\u03B5\u03C2.",
    L"\u039C\u03B5\u03B3\u03AC\u03BB\u03B7 \u03CE\u03C1\u03B1",
    L"\u03A3\u03CD\u03BD\u03C4\u03BF\u03BC\u03B7 \u03B7\u03BC\u03B5\u03C1\u03BF\u03BC\u03B7\u03BD\u03AF\u03B1",
    L"\u039C\u03B5\u03B3\u03AC\u03BB\u03B7 \u03B7\u03BC\u03B5\u03C1\u03BF\u03BC\u03B7\u03BD\u03AF\u03B1",
    L"\u0397 \u03C4\u03B9\u03BC\u03AE \u03C3\u03B5 \u03B1\u03C5\u03C4\u03CC \u03C4\u03BF \u03C0\u03B5\u03B4\u03AF\u03BF \u03C0\u03C1\u03AD\u03C0\u03B5\u03B9 \u03BD\u03B1 \u03B5\u03AF\u03BD\u03B1\u03B9 \u03AD\u03BD\u03B1\u03C2 \u03B1\u03C1\u03B9\u03B8\u03BC\u03CC\u03C2 \u03B1\u03C0\u03CC \u03C4\u03BF 99 \u03AD\u03C9\u03C2 \u03C4\u03BF 9999. \u0394\u03BF\u03BA\u03B9\u03BC\u03AC\u03C3\u03C4\u03B5 \u03BD\u03B1 \u03C7\u03C1\u03B7\u03C3\u03B9\u03BC\u03BF\u03C0\u03BF\u03B9\u03AE\u03C3\u03B5\u03C4\u03B5 \u03B4\u03B9\u03B1\u03C6\u03BF\u03C1\u03B5\u03C4\u03B9\u03BA\u03CC \u03B1\u03C1\u03B9\u03B8\u03BC\u03CC.",
    L"\u03A3\u03CD\u03BD\u03C4\u03BF\u03BC\u03B7 \u03CE\u03C1\u03B1",
    L"&\u039C\u03BF\u03C1\u03C6\u03AE:",
    L"&\u039C\u03BF\u03C1\u03C6\u03AE: (* \u03C0\u03C1\u03BF\u03C3\u03B1\u03C1\u03BC\u03BF\u03C3\u03BC\u03AD\u03BD\u03B7 \u03C4\u03BF\u03C0\u03B9\u03BA\u03AE \u03C1\u03CD\u03B8\u03BC\u03B9\u03C3\u03B7)",
    nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
    L"\u0397 \u03C4\u03BF\u03C0\u03B9\u03BA\u03AE \u03C1\u03CD\u03B8\u03BC\u03B9\u03C3\u03B7 \u03C3\u03C5\u03C3\u03C4\u03AE\u03BC\u03B1\u03C4\u03BF\u03C2 \u03AC\u03BB\u03BB\u03B1\u03BE\u03B5. \u03A0\u03C1\u03AD\u03C0\u03B5\u03B9 \u03BD\u03B1 \u03B5\u03C0\u03B1\u03BD\u03B5\u03BA\u03BA\u03B9\u03BD\u03AE\u03C3\u03B5\u03C4\u03B5 \u03C4\u03B1 Windows \u03B3\u03B9\u03B1 \u03BD\u03B1 \u03B9\u03C3\u03C7\u03CD\u03C3\u03BF\u03C5\u03BD \u03BF\u03B9 \u03B1\u03BB\u03BB\u03B1\u03B3\u03AD\u03C2.",
    L"\u0391\u03BB\u03BB\u03B1\u03B3\u03AE \u03C4\u03BF\u03C0\u03B9\u03BA\u03CE\u03BD \u03C1\u03C5\u03B8\u03BC\u03AF\u03C3\u03B5\u03C9\u03BD",
    L"\u0394\u03B5\u03BD \u03AE\u03C4\u03B1\u03BD \u03B4\u03C5\u03BD\u03B1\u03C4\u03AE \u03B7 \u03C6\u03CC\u03C1\u03C4\u03C9\u03C3\u03B7 \u03C4\u03B7\u03C2 \u03B5\u03C0\u03B9\u03BB\u03B5\u03B3\u03BC\u03AD\u03BD\u03B7\u03C2 \u03B3\u03BB\u03CE\u03C3\u03C3\u03B1\u03C2. \u0395\u03C0\u03B9\u03BA\u03BF\u03B9\u03BD\u03C9\u03BD\u03AE\u03C3\u03C4\u03B5 \u03BC\u03B5 \u03C4\u03BF \u03B4\u03B9\u03B1\u03C7\u03B5\u03B9\u03C1\u03B9\u03C3\u03C4\u03AE \u03C4\u03BF\u03C5 \u03C3\u03C5\u03C3\u03C4\u03AE\u03BC\u03B1\u03C4\u03BF\u03C2.",
    L"\u0397 \u03B3\u03BB\u03CE\u03C3\u03C3\u03B1 \u03B5\u03BC\u03C6\u03AC\u03BD\u03B9\u03C3\u03B7\u03C2 \u03C4\u03BF\u03C5 \u03C3\u03C5\u03C3\u03C4\u03AE\u03BC\u03B1\u03C4\u03BF\u03C2 \u03AC\u03BB\u03BB\u03B1\u03BE\u03B5. \u03A0\u03C1\u03AD\u03C0\u03B5\u03B9 \u03BD\u03B1 \u03B5\u03C0\u03B1\u03BD\u03B5\u03BA\u03BA\u03B9\u03BD\u03AE\u03C3\u03B5\u03C4\u03B5 \u03C4\u03B1 Windows \u03B3\u03B9\u03B1 \u03BD\u03B1 \u03B9\u03C3\u03C7\u03CD\u03C3\u03BF\u03C5\u03BD \u03BF\u03B9 \u03B1\u03BB\u03BB\u03B1\u03B3\u03AD\u03C2.",
    L"\u0398\u03AD\u03BB\u03B5\u03C4\u03B5 \u03BD\u03B1 \u03B4\u03B9\u03B1\u03B3\u03C1\u03AC\u03C8\u03B5\u03C4\u03B5 \u03CC\u03BB\u03B5\u03C2 \u03C4\u03B9\u03C2 \u03C0\u03C1\u03BF\u03C3\u03B1\u03C1\u03BC\u03BF\u03B3\u03AD\u03C2 \u03C4\u03B7\u03C2 \u03C4\u03C1\u03AD\u03C7\u03BF\u03C5\u03C3\u03B1\u03C2 \u03BC\u03BF\u03C1\u03C6\u03AE\u03C2;",
    L"\u0398\u03AD\u03BB\u03B5\u03C4\u03B5 \u03BD\u03B1 \u03B5\u03C6\u03B1\u03C1\u03BC\u03CC\u03C3\u03B5\u03C4\u03B5 \u03C4\u03B9\u03C2 \u03B1\u03BB\u03BB\u03B1\u03B3\u03AD\u03C2 \u03B3\u03BB\u03CE\u03C3\u03C3\u03B1\u03C2 \u03BA\u03B1\u03B9 \u03C0\u03B5\u03C1\u03B9\u03BF\u03C7\u03AE\u03C2;",
    L"\u0395\u03C0\u03B1\u03BD\u03B5\u03BA\u03BA\u03AF\u03BD\u03B7\u03C3\u03B7 \u03C4\u03CE\u03C1\u03B1",
    L"\u0386\u03BA\u03C5\u03C1\u03BF",
    L"\u03A0\u03C1\u03B9\u03BD \u03B1\u03C0\u03CC \u03C4\u03B7\u03BD \u03B5\u03C0\u03B1\u03BD\u03B5\u03BA\u03BA\u03AF\u03BD\u03B7\u03C3\u03B7, \u03B1\u03C0\u03BF\u03B8\u03B7\u03BA\u03B5\u03CD\u03C3\u03C4\u03B5 \u03C4\u03B7\u03BD \u03B5\u03C1\u03B3\u03B1\u03C3\u03AF\u03B1 \u03C3\u03B1\u03C2 \u03BA\u03B1\u03B9 \u03BA\u03BB\u03B5\u03AF\u03C3\u03C4\u03B5 \u03CC\u03BB\u03B1 \u03C4\u03B1 \u03B1\u03BD\u03BF\u03B9\u03C7\u03C4\u03AC \u03C0\u03C1\u03BF\u03B3\u03C1\u03AC\u03BC\u03BC\u03B1\u03C4\u03B1.",
    L"\u03A4\u03BF\u03C0\u03B9\u03BA\u03AE \u03C1\u03CD\u03B8\u03BC\u03B9\u03C3\u03B7 \u03C3\u03C5\u03C3\u03C4\u03AE\u03BC\u03B1\u03C4\u03BF\u03C2",
    L"\u0394\u03B5\u03BD \u03AE\u03C4\u03B1\u03BD \u03B4\u03C5\u03BD\u03B1\u03C4\u03AE \u03B7 \u03C3\u03C9\u03C3\u03C4\u03AE \u03C6\u03CC\u03C1\u03C4\u03C9\u03C3\u03B7 \u03C4\u03B7\u03C2 \u03B4\u03B9\u03AC\u03C4\u03B1\u03BE\u03B7\u03C2 \u03C0\u03BB\u03B7\u03BA\u03C4\u03C1\u03BF\u03BB\u03BF\u03B3\u03AF\u03BF\u03C5 %s.",
    L"\u0399\u03C3\u03C0\u03B1\u03BD\u03B9\u03BA\u03AC (\u0399\u03C3\u03C0\u03B1\u03BD\u03AF\u03B1)",
    L"\u0393\u03B9\u03B1 \u03BD\u03B1 \u03B9\u03C3\u03C7\u03CD\u03C3\u03B5\u03B9 \u03B7 \u03B1\u03BB\u03BB\u03B1\u03B3\u03AE \u03C4\u03B7\u03C2 \u03B3\u03BB\u03CE\u03C3\u03C3\u03B1\u03C2 \u03B5\u03BC\u03C6\u03AC\u03BD\u03B9\u03C3\u03B7\u03C2, \u03C0\u03C1\u03AD\u03C0\u03B5\u03B9 \u03BD\u03B1 \u03B1\u03C0\u03BF\u03C3\u03C5\u03BD\u03B4\u03B5\u03B8\u03B5\u03AF\u03C4\u03B5 \u03BA\u03B1\u03B9 \u03BD\u03B1 \u03C3\u03C5\u03BD\u03B4\u03B5\u03B8\u03B5\u03AF\u03C4\u03B5 \u03BE\u03B1\u03BD\u03AC",
    L"\u03A0\u03C1\u03B9\u03BD \u03B1\u03C0\u03CC \u03C4\u03B7\u03BD \u03B1\u03C0\u03BF\u03C3\u03CD\u03BD\u03B4\u03B5\u03C3\u03B7, \u03B1\u03C0\u03BF\u03B8\u03B7\u03BA\u03B5\u03CD\u03C3\u03C4\u03B5 \u03C4\u03B7\u03BD \u03B5\u03C1\u03B3\u03B1\u03C3\u03AF\u03B1 \u03C3\u03B1\u03C2 \u03BA\u03B1\u03B9 \u03BA\u03BB\u03B5\u03AF\u03C3\u03C4\u03B5 \u03CC\u03BB\u03B1 \u03C4\u03B1 \u03B1\u03BD\u03BF\u03B9\u03C7\u03C4\u03AC \u03C0\u03C1\u03BF\u03B3\u03C1\u03AC\u03BC\u03BC\u03B1\u03C4\u03B1.",
    L"\u0391\u03C0\u03BF\u03C3\u03CD\u03BD\u03B4\u03B5\u03C3\u03B7 \u03C4\u03CE\u03C1\u03B1",
    L"\u0386\u03BA\u03C5\u03C1\u03BF",
    L"\u0391\u03BB\u03BB\u03B1\u03B3\u03AE \u03B3\u03BB\u03CE\u03C3\u03C3\u03B1\u03C2 \u03B5\u03BC\u03C6\u03AC\u03BD\u03B9\u03C3\u03B7\u03C2",
    L"\u03A3\u03C5\u03BD\u03B9\u03C3\u03C4\u03AC\u03C4\u03B1\u03B9 \u03BD\u03B1 \u03B5\u03C6\u03B1\u03C1\u03BC\u03CC\u03C3\u03B5\u03C4\u03B5 \u03C4\u03B9\u03C2 \u03B1\u03BB\u03BB\u03B1\u03B3\u03AD\u03C2 \u03C0\u03C1\u03B9\u03BD \u03BA\u03AC\u03BD\u03B5\u03C4\u03B5 \u03AC\u03BB\u03BB\u03B5\u03C2 \u03B1\u03BB\u03BB\u03B1\u03B3\u03AD\u03C2 \u03C3\u03C4\u03BF \u03C3\u03CD\u03C3\u03C4\u03B7\u03BC\u03B1, \u03CE\u03C3\u03C4\u03B5 \u03BD\u03B1 \u03C4\u03B9\u03C2 \u03B1\u03BD\u03C4\u03B9\u03BA\u03B1\u03C4\u03BF\u03C0\u03C4\u03C1\u03AF\u03B6\u03B5\u03B9 \u03BF \u03C5\u03C0\u03BF\u03BB\u03BF\u03B3\u03B9\u03C3\u03C4\u03AE\u03C2.",
    L"\u0395\u03C6\u03B1\u03C1\u03BC\u03BF\u03B3\u03AE",
    L"\u0386\u03BA\u03C5\u03C1\u03BF",
    L"\u0394\u03B5\u03BD \u03AE\u03C4\u03B1\u03BD \u03B4\u03C5\u03BD\u03B1\u03C4\u03AE \u03B7 \u03BF\u03BB\u03BF\u03BA\u03BB\u03AE\u03C1\u03C9\u03C3\u03B7 \u03C4\u03B7\u03C2 \u03BB\u03B5\u03B9\u03C4\u03BF\u03C5\u03C1\u03B3\u03AF\u03B1\u03C2",
    L"\u03A4\u03C1\u03AD\u03C7\u03C9\u03BD \u03C7\u03C1\u03AE\u03C3\u03C4\u03B7\u03C2",
    L"\u039F\u03B8\u03CC\u03BD\u03B7 \u03C5\u03C0\u03BF\u03B4\u03BF\u03C7\u03AE\u03C2",
    L"\u039D\u03AD\u03BF\u03B9 \u03BB\u03BF\u03B3\u03B1\u03C1\u03B9\u03B1\u03C3\u03BC\u03BF\u03AF",
    L"\u0393\u03BB\u03CE\u03C3\u03C3\u03B1 \u03B5\u03BC\u03C6\u03AC\u03BD\u03B9\u03C3\u03B7\u03C2:",
    L"\u0393\u03BB\u03CE\u03C3\u03C3\u03B1 \u03B5\u03B9\u03C3\u03CC\u03B4\u03BF\u03C5:",
    L"\u039C\u03BF\u03C1\u03C6\u03AE:",
    L"\u03A4\u03BF\u03C0\u03BF\u03B8\u03B5\u03C3\u03AF\u03B1:",
    L"\u0394\u03B5\u03BD \u03AE\u03C4\u03B1\u03BD \u03B4\u03C5\u03BD\u03B1\u03C4\u03AE \u03B7 \u03B1\u03BD\u03AC\u03B3\u03BD\u03C9\u03C3\u03B7 \u03C4\u03B7\u03C2 \u03C0\u03B1\u03C1\u03B1\u03BC\u03AD\u03C4\u03C1\u03BF\u03C5",
    L"\u03A0\u03B5\u03C1\u03B9\u03B2\u03AC\u03BB\u03BB\u03BF\u03BD",
    L"\u03A0\u03BF\u03C4\u03AD",
    L"\u0395\u03B3\u03C7\u03CE\u03C1\u03B9\u03BF",
};
static const wchar_t* const kDlgTr_EL[92] = {
    L"\u039C\u03BF\u03C1\u03C6\u03AD\u03C2 \u03B7\u03BC\u03B5\u03C1\u03BF\u03BC\u03B7\u03BD\u03AF\u03B1\u03C2 \u03BA\u03B1\u03B9 \u03CE\u03C1\u03B1\u03C2",
    L"&\u03A3\u03CD\u03BD\u03C4\u03BF\u03BC\u03B7 \u03B7\u03BC\u03B5\u03C1\u03BF\u03BC\u03B7\u03BD\u03AF\u03B1:",
    L"\u039C\u03B5&\u03B3\u03AC\u03BB\u03B7 \u03B7\u03BC\u03B5\u03C1\u03BF\u03BC\u03B7\u03BD\u03AF\u03B1:",
    L"\u03A3\u03CD\u03BD\u03C4\u03BF\u03BC\u03B7 &\u03CE\u03C1\u03B1:",
    L"\u039C\u03B5\u03B3\u03AC\u03BB\u03B7 \u03CE&\u03C1\u03B1:",
    L"\u03A0\u03C1\u03CE\u03C4\u03B7 \u03B7\u03BC\u03AD\u03C1\u03B1 \u03C4\u03B7\u03C2 \u03B5&\u03B2\u03B4\u03BF\u03BC\u03AC\u03B4\u03B1\u03C2:",
    L"\u03A0\u03B1\u03C1\u03B1\u03B4\u03B5\u03AF\u03B3\u03BC\u03B1\u03C4\u03B1",
    L"\u03A3\u03CD\u03BD\u03C4\u03BF\u03BC\u03B7 \u03B7\u03BC\u03B5\u03C1\u03BF\u03BC\u03B7\u03BD\u03AF\u03B1:",
    L"\u039C\u03B5\u03B3\u03AC\u03BB\u03B7 \u03B7\u03BC\u03B5\u03C1\u03BF\u03BC\u03B7\u03BD\u03AF\u03B1:",
    L"\u03A3\u03CD\u03BD\u03C4\u03BF\u03BC\u03B7 \u03CE\u03C1\u03B1:",
    L"\u039C\u03B5\u03B3\u03AC\u03BB\u03B7 \u03CE\u03C1\u03B1:",
    L"\u03A0\u03C1\u03CC\u03C3&\u03B8\u03B5\u03C4\u03B5\u03C2 \u03C1\u03C5\u03B8\u03BC\u03AF\u03C3\u03B5\u03B9\u03C2...",
    L"\u03A0\u03BB\u03B7\u03BA\u03C4\u03C1\u03BF\u03BB\u03CC\u03B3\u03B9\u03B1 \u03BA\u03B1\u03B9 \u03AC\u03BB\u03BB\u03B5\u03C2 \u03B3\u03BB\u03CE\u03C3\u03C3\u03B5\u03C2 \u03B5\u03B9\u03C3\u03CC\u03B4\u03BF\u03C5",
    L"\u0393\u03B9\u03B1 \u03BD\u03B1 \u03B1\u03BB\u03BB\u03AC\u03BE\u03B5\u03C4\u03B5 \u03C4\u03BF \u03C0\u03BB\u03B7\u03BA\u03C4\u03C1\u03BF\u03BB\u03CC\u03B3\u03B9\u03BF \u03AE \u03C4\u03B7 \u03B3\u03BB\u03CE\u03C3\u03C3\u03B1 \u03B5\u03B9\u03C3\u03CC\u03B4\u03BF\u03C5, \u03BA\u03AC\u03BD\u03C4\u03B5 \u03BA\u03BB\u03B9\u03BA \u03C3\u03C4\u03B7\u03BD \u0391\u03BB\u03BB\u03B1\u03B3\u03AE \u03C0\u03BB\u03B7\u03BA\u03C4\u03C1\u03BF\u03BB\u03BF\u03B3\u03AF\u03C9\u03BD.",
    L"&\u0391\u03BB\u03BB\u03B1\u03B3\u03AE \u03C0\u03BB\u03B7\u03BA\u03C4\u03C1\u03BF\u03BB\u03BF\u03B3\u03AF\u03C9\u03BD...",
    L"\u0393\u03BB\u03CE\u03C3\u03C3\u03B1 \u03B5\u03BC\u03C6\u03AC\u03BD\u03B9\u03C3\u03B7\u03C2",
    L"\u0395\u03B3\u03BA\u03B1\u03C4\u03B1\u03C3\u03C4\u03AE\u03C3\u03C4\u03B5 \u03AE \u03BA\u03B1\u03C4\u03B1\u03C1\u03B3\u03AE\u03C3\u03C4\u03B5 \u03B3\u03BB\u03CE\u03C3\u03C3\u03B5\u03C2 \u03C0\u03BF\u03C5 \u03BC\u03C0\u03BF\u03C1\u03BF\u03CD\u03BD \u03BD\u03B1 \u03C7\u03C1\u03B7\u03C3\u03B9\u03BC\u03BF\u03C0\u03BF\u03B9\u03AE\u03C3\u03BF\u03C5\u03BD \u03C4\u03B1 Windows \u03B3\u03B9\u03B1 \u03C4\u03B7\u03BD \u03B5\u03BC\u03C6\u03AC\u03BD\u03B9\u03C3\u03B7 \u03BA\u03B5\u03B9\u03BC\u03AD\u03BD\u03BF\u03C5 \u03BA\u03B1\u03B9, \u03CC\u03C0\u03BF\u03C5 \u03C5\u03C0\u03BF\u03C3\u03C4\u03B7\u03C1\u03AF\u03B6\u03B5\u03C4\u03B1\u03B9, \u03B3\u03B9\u03B1 \u03B1\u03BD\u03B1\u03B3\u03BD\u03CE\u03C1\u03B9\u03C3\u03B7 \u03BF\u03BC\u03B9\u03BB\u03AF\u03B1\u03C2 \u03BA\u03B1\u03B9 \u03C7\u03B5\u03B9\u03C1\u03BF\u03B3\u03C1\u03AC\u03C6\u03BF\u03C5.",
    L"&\u0395\u03B3\u03BA\u03B1\u03C4\u03AC\u03C3\u03C4\u03B1\u03C3\u03B7 \u03AE \u03BA\u03B1\u03C4\u03AC\u03C1\u03B3\u03B7\u03C3\u03B7 \u03B3\u03BB\u03C9\u03C3\u03C3\u03CE\u03BD...",
    L"\u03A9\u03C2 \u03B5\u03C0\u03B9\u03C3\u03BA\u03AD\u03C0\u03C4\u03B7\u03C2 \u03B4\u03B5\u03BD \u03BC\u03C0\u03BF\u03C1\u03B5\u03AF\u03C4\u03B5 \u03BD\u03B1 \u03B1\u03BB\u03BB\u03AC\u03BE\u03B5\u03C4\u03B5 \u03C4\u03B7 \u03B3\u03BB\u03CE\u03C3\u03C3\u03B1 \u03B5\u03BC\u03C6\u03AC\u03BD\u03B9\u03C3\u03B7\u03C2:",
    L"\u0397 \u03B5\u03C0\u03B9\u03BB\u03BF\u03B3\u03AE \u03B3\u03BB\u03CE\u03C3\u03C3\u03B1\u03C2 \u03B5\u03BC\u03C6\u03AC\u03BD\u03B9\u03C3\u03B7\u03C2 \u03B5\u03AF\u03BD\u03B1\u03B9 \u03BA\u03BB\u03B5\u03B9\u03B4\u03C9\u03BC\u03AD\u03BD\u03B7 \u03B1\u03C0\u03CC \u03C4\u03B7\u03BD \u03C0\u03BF\u03BB\u03B9\u03C4\u03B9\u03BA\u03AE \u03BF\u03BC\u03AC\u03B4\u03B1\u03C2.",
    L"&\u0395\u03C0\u03B9\u03BB\u03AD\u03BE\u03C4\u03B5 \u03BC\u03B9\u03B1 \u03B3\u03BB\u03CE\u03C3\u03C3\u03B1 \u03B5\u03BC\u03C6\u03AC\u03BD\u03B9\u03C3\u03B7\u03C2:",
    L"\u039A\u03AC\u03C0\u03BF\u03B9\u03BF \u03BA\u03B5\u03AF\u03BC\u03B5\u03BD\u03BF \u03B4\u03B5\u03BD \u03AD\u03C7\u03B5\u03B9 &\u03C4\u03BF\u03C0\u03B9\u03BA\u03BF\u03C0\u03BF\u03B9\u03B7\u03B8\u03B5\u03AF \u03C3\u03C4\u03B7\u03BD \u03B5\u03C0\u03B9\u03BB\u03B5\u03B3\u03BC\u03AD\u03BD\u03B7 \u03B3\u03BB\u03CE\u03C3\u03C3\u03B1. \u0395\u03C0\u03B9\u03BB\u03AD\u03BE\u03C4\u03B5 \u03BC\u03B9\u03B1 \u03AC\u03BB\u03BB\u03B7 \u03B3\u03BB\u03CE\u03C3\u03C3\u03B1 \u03C0\u03BF\u03C5 \u03B8\u03B1 \u03C7\u03C1\u03B7\u03C3\u03B9\u03BC\u03BF\u03C0\u03BF\u03B9\u03BF\u03CD\u03BD \u03C4\u03B1 Windows \u03B3\u03B9\u03B1 \u03C4\u03B7\u03BD \u03B5\u03BC\u03C6\u03AC\u03BD\u03B9\u03C3\u03B7 \u03B1\u03C5\u03C4\u03BF\u03CD \u03C4\u03BF\u03C5 \u03BA\u03B5\u03B9\u03BC\u03AD\u03BD\u03BF\u03C5:",
    L"\u0391\u03C5\u03C4\u03AE \u03B7 \u03B3\u03BB\u03CE\u03C3\u03C3\u03B1 \u03B5\u03AF\u03BD\u03B1\u03B9 \u03C4\u03BF\u03C0\u03B9\u03BA\u03BF\u03C0\u03BF\u03B9\u03B7\u03BC\u03AD\u03BD\u03B7 \u03BC\u03CC\u03BD\u03BF \u03B5\u03BD \u03BC\u03AD\u03C1\u03B5\u03B9 \u03BA\u03B1\u03B9 \u03BA\u03AC\u03C0\u03BF\u03B9\u03BF \u03BA\u03B5\u03AF\u03BC\u03B5\u03BD\u03BF \u03BC\u03C0\u03BF\u03C1\u03B5\u03AF \u03BD\u03B1 \u03B5\u03BC\u03C6\u03B1\u03BD\u03AF\u03B6\u03B5\u03C4\u03B1\u03B9 \u03C3\u03C4\u03B1:",
    L"\u0391\u03C5\u03C4\u03AE \u03B7 \u03B3\u03BB\u03CE\u03C3\u03C3\u03B1 \u03B5\u03AF\u03BD\u03B1\u03B9 \u03B5\u03C0\u03AF\u03C3\u03B7\u03C2 \u03C4\u03BF\u03C0\u03B9\u03BA\u03BF\u03C0\u03BF\u03B9\u03B7\u03BC\u03AD\u03BD\u03B7 \u03BC\u03CC\u03BD\u03BF \u03B5\u03BD \u03BC\u03AD&\u03C1\u03B5\u03B9. \u0395\u03C0\u03B9\u03BB\u03AD\u03BE\u03C4\u03B5 \u03BC\u03B9\u03B1 \u03C4\u03C1\u03AF\u03C4\u03B7 \u03B3\u03BB\u03CE\u03C3\u03C3\u03B1 \u03C0\u03BF\u03C5 \u03B8\u03B1 \u03C7\u03C1\u03B7\u03C3\u03B9\u03BC\u03BF\u03C0\u03BF\u03B9\u03BF\u03CD\u03BD \u03C4\u03B1 Windows \u03B3\u03B9\u03B1 \u03C4\u03B7\u03BD \u03B5\u03BC\u03C6\u03AC\u03BD\u03B9\u03C3\u03B7 \u03C4\u03BF\u03C5 \u03C5\u03C0\u03CC\u03BB\u03BF\u03B9\u03C0\u03BF\u03C5 \u03BA\u03B5\u03B9\u03BC\u03AD\u03BD\u03BF\u03C5:",
    L"\u0391\u03C5\u03C4\u03AE \u03B7 \u03B3\u03BB\u03CE\u03C3\u03C3\u03B1 \u03B5\u03AF\u03BD\u03B1\u03B9 \u03B5\u03C0\u03AF\u03C3\u03B7\u03C2 \u03C4\u03BF\u03C0\u03B9\u03BA\u03BF\u03C0\u03BF\u03B9\u03B7\u03BC\u03AD\u03BD\u03B7 \u03BC\u03CC\u03BD\u03BF \u03B5\u03BD \u03BC\u03AD\u03C1\u03B5\u03B9 \u03BA\u03B1\u03B9 \u03BA\u03AC\u03C0\u03BF\u03B9\u03BF \u03BA\u03B5\u03AF\u03BC\u03B5\u03BD\u03BF \u03BC\u03C0\u03BF\u03C1\u03B5\u03AF \u03BD\u03B1 \u03B5\u03BC\u03C6\u03B1\u03BD\u03AF\u03B6\u03B5\u03C4\u03B1\u03B9 \u03C3\u03C4\u03B1: ",
    L"\u039F\u03B8\u03CC\u03BD\u03B7 \u03C5\u03C0\u03BF\u03B4\u03BF\u03C7\u03AE\u03C2 \u03BA\u03B1\u03B9 \u03BD\u03AD\u03BF\u03B9 \u03BB\u03BF\u03B3\u03B1\u03C1\u03B9\u03B1\u03C3\u03BC\u03BF\u03AF \u03C7\u03C1\u03B7\u03C3\u03C4\u03CE\u03BD",
    L"\u03A0\u03C1\u03BF\u03B2\u03AC\u03BB\u03B5\u03C4\u03B5 \u03BA\u03B1\u03B9 \u03B1\u03BD\u03C4\u03B9\u03B3\u03C1\u03AC\u03C8\u03C4\u03B5 \u03C4\u03B9\u03C2 \u03C4\u03BF\u03C0\u03B9\u03BA\u03AD\u03C2 \u03C3\u03B1\u03C2 \u03C1\u03C5\u03B8\u03BC\u03AF\u03C3\u03B5\u03B9\u03C2 \u03C3\u03C4\u03B7\u03BD \u03BF\u03B8\u03CC\u03BD\u03B7 \u03C5\u03C0\u03BF\u03B4\u03BF\u03C7\u03AE\u03C2, \u03C3\u03C4\u03BF\u03C5\u03C2 \u03BB\u03BF\u03B3\u03B1\u03C1\u03B9\u03B1\u03C3\u03BC\u03BF\u03CD\u03C2 \u03C3\u03C5\u03C3\u03C4\u03AE\u03BC\u03B1\u03C4\u03BF\u03C2 \u03BA\u03B1\u03B9 \u03C3\u03C4\u03BF\u03C5\u03C2 \u03BD\u03AD\u03BF\u03C5\u03C2 \u03BB\u03BF\u03B3\u03B1\u03C1\u03B9\u03B1\u03C3\u03BC\u03BF\u03CD\u03C2 \u03C7\u03C1\u03B7\u03C3\u03C4\u03CE\u03BD.",
    L"&\u0391\u03BD\u03C4\u03B9\u03B3\u03C1\u03B1\u03C6\u03AE \u03C1\u03C5\u03B8\u03BC\u03AF\u03C3\u03B5\u03C9\u03BD...",
    L"\u0393\u03BB\u03CE\u03C3\u03C3\u03B1 \u03B3\u03B9\u03B1 \u03C0\u03C1\u03BF\u03B3\u03C1\u03AC\u03BC\u03BC\u03B1\u03C4\u03B1 \u03C0\u03BF\u03C5 \u03B4\u03B5\u03BD \u03C5\u03C0\u03BF\u03C3\u03C4\u03B7\u03C1\u03AF\u03B6\u03BF\u03C5\u03BD Unicode",
    L"\u0391\u03C5\u03C4\u03AE \u03B7 \u03C1\u03CD\u03B8\u03BC\u03B9\u03C3\u03B7 (\u03C4\u03BF\u03C0\u03B9\u03BA\u03AE \u03C1\u03CD\u03B8\u03BC\u03B9\u03C3\u03B7 \u03C3\u03C5\u03C3\u03C4\u03AE\u03BC\u03B1\u03C4\u03BF\u03C2) \u03B5\u03BB\u03AD\u03B3\u03C7\u03B5\u03B9 \u03C4\u03B7 \u03B3\u03BB\u03CE\u03C3\u03C3\u03B1 \u03C0\u03BF\u03C5 \u03C7\u03C1\u03B7\u03C3\u03B9\u03BC\u03BF\u03C0\u03BF\u03B9\u03B5\u03AF\u03C4\u03B1\u03B9 \u03BA\u03B1\u03C4\u03AC \u03C4\u03B7\u03BD \u03B5\u03BC\u03C6\u03AC\u03BD\u03B9\u03C3\u03B7 \u03BA\u03B5\u03B9\u03BC\u03AD\u03BD\u03BF\u03C5 \u03C3\u03B5 \u03C0\u03C1\u03BF\u03B3\u03C1\u03AC\u03BC\u03BC\u03B1\u03C4\u03B1 \u03C0\u03BF\u03C5 \u03B4\u03B5\u03BD \u03C5\u03C0\u03BF\u03C3\u03C4\u03B7\u03C1\u03AF\u03B6\u03BF\u03C5\u03BD Unicode.",
    L"\u03A4\u03C1\u03AD\u03C7\u03BF\u03C5\u03C3\u03B1 \u03B3\u03BB\u03CE\u03C3\u03C3\u03B1 \u03B3\u03B9\u03B1 \u03C0\u03C1\u03BF\u03B3\u03C1\u03AC\u03BC\u03BC\u03B1\u03C4\u03B1 \u03C0\u03BF\u03C5 \u03B4\u03B5\u03BD \u03C5\u03C0\u03BF\u03C3\u03C4\u03B7\u03C1\u03AF\u03B6\u03BF\u03C5\u03BD Unicode:",
    nullptr,
    L"\u03A4\u03BF\u03C0\u03B9\u03BA\u03AE \u03C1\u03CD\u03B8\u03BC\u03B9\u03C3\u03B7 &\u03C3\u03C5\u03C3\u03C4\u03AE\u03BC\u03B1\u03C4\u03BF\u03C2...",
    L"\u03A0\u03B1\u03C1\u03AC\u03B4\u03B5\u03B9\u03B3\u03BC\u03B1",
    L"\u0398\u03B5\u03C4\u03B9\u03BA\u03AC:",
    L"\u0391\u03C1\u03BD\u03B7\u03C4\u03B9\u03BA\u03AC:",
    L"&\u03A5\u03C0\u03BF\u03B4\u03B9\u03B1\u03C3\u03C4\u03BF\u03BB\u03AE:",
    L"\u0391\u03C1\u03B9\u03B8\u03BC\u03CC\u03C2 \u03B4\u03B5\u03BA\u03B1\u03B4\u03B9\u03BA\u03CE\u03BD \u03C8\u03B7&\u03C6\u03AF\u03C9\u03BD:",
    L"\u03A3\u03CD\u03BC\u03B2\u03BF\u03BB\u03BF &\u03BF\u03BC\u03B1\u03B4\u03BF\u03C0\u03BF\u03AF\u03B7\u03C3\u03B7\u03C2 \u03C8\u03B7\u03C6\u03AF\u03C9\u03BD:",
    L"\u039F\u03BC\u03B1\u03B4&\u03BF\u03C0\u03BF\u03AF\u03B7\u03C3\u03B7 \u03B1\u03C1\u03B9\u03B8\u03BC\u03CE\u03BD:",
    L"\u03A3\u03CD\u03BC\u03B2\u03BF\u03BB\u03BF &\u03BC\u03B5\u03AF\u03BF\u03BD:",
    L"\u039C\u03BF\u03C1\u03C6\u03AE \u03B1\u03C1&\u03BD\u03B7\u03C4\u03B9\u03BA\u03CE\u03BD \u03B1\u03C1\u03B9\u03B8\u03BC\u03CE\u03BD:",
    L"\u0395\u03BC\u03C6\u03AC\u03BD\u03B9\u03C3\u03B7 \u03B1\u03C1\u03C7\u03B9\u03BA\u03CE\u03BD \u03BC&\u03B7\u03B4\u03B5\u03BD\u03B9\u03BA\u03CE\u03BD:",
    L"\u0394\u03B9\u03B1\u03C7\u03C9\u03C1\u03B9\u03C3\u03C4\u03B9\u03BA\u03CC &\u03BB\u03AF\u03C3\u03C4\u03B1\u03C2:",
    L"\u03A3\u03CD\u03C3\u03C4\u03B7\u03BC\u03B1 \u03BC\u03AD&\u03C4\u03C1\u03B7\u03C3\u03B7\u03C2:",
    L"\u03A4\u03C5\u03C0\u03B9\u03BA\u03AC \u03C8\u03B7&\u03C6\u03AF\u03B1:",
    L"&\u03A7\u03C1\u03AE\u03C3\u03B7 \u03B5\u03B3\u03C7\u03CE\u03C1\u03B9\u03C9\u03BD \u03C8\u03B7\u03C6\u03AF\u03C9\u03BD:",
    L"\u039A\u03AC\u03BD\u03C4\u03B5 \u03BA\u03BB\u03B9\u03BA \u03C3\u03C4\u03B7\u03BD \u0395\u03C0\u03B1\u03BD\u03B1\u03C6\u03BF\u03C1\u03AC \u03B3\u03B9\u03B1 \u03BD\u03B1 \u03B5\u03C0\u03B1\u03BD\u03B1\u03C6\u03AD\u03C1\u03B5\u03C4\u03B5 \u03C4\u03B9\u03C2 \u03C0\u03C1\u03BF\u03B5\u03C0\u03B9\u03BB\u03B5\u03B3\u03BC\u03AD\u03BD\u03B5\u03C2 \u03C1\u03C5\u03B8\u03BC\u03AF\u03C3\u03B5\u03B9\u03C2 \u03C3\u03C5\u03C3\u03C4\u03AE\u03BC\u03B1\u03C4\u03BF\u03C2 \u03B3\u03B9\u03B1 \u03B1\u03C1\u03B9\u03B8\u03BC\u03BF\u03CD\u03C2, \u03BD\u03CC\u03BC\u03B9\u03C3\u03BC\u03B1, \u03CE\u03C1\u03B1 \u03BA\u03B1\u03B9 \u03B7\u03BC\u03B5\u03C1\u03BF\u03BC\u03B7\u03BD\u03AF\u03B1.",
    L"\u0395\u03C0\u03B1\u03BD\u03B1&\u03C6\u03BF\u03C1\u03AC",
    L"\u03A3\u03CD\u03BC\u03B2\u03BF\u03BB\u03BF &\u03BD\u03BF\u03BC\u03AF\u03C3\u03BC\u03B1\u03C4\u03BF\u03C2:",
    L"&\u0398\u03B5\u03C4\u03B9\u03BA\u03AE \u03BC\u03BF\u03C1\u03C6\u03AE \u03BD\u03BF\u03BC\u03AF\u03C3\u03BC\u03B1\u03C4\u03BF\u03C2:",
    L"\u0391\u03C1\u03BD\u03B7\u03C4\u03B9\u03BA\u03AE \u03BC\u03BF\u03C1\u03C6\u03AE \u03BD\u03BF\u03BC\u03AF\u03C3\u03BC\u03B1&\u03C4\u03BF\u03C2:",
    L"\u0391\u03C1\u03B9\u03B8\u03BC\u03CC\u03C2 \u03B4\u03B5\u03BA\u03B1\u03B4\u03B9\u03BA\u03CE\u03BD \u03C8&\u03B7\u03C6\u03AF\u03C9\u03BD:",
    L"\u03A3\u03CD\u03BC\u03B2\u03BF\u03BB\u03BF \u03BF\u03BC\u03B1\u03B4\u03BF\u03C0\u03BF\u03AF\u03B7\u03C3\u03B7\u03C2 \u03C8\u03B7&\u03C6\u03AF\u03C9\u03BD:",
    L"\u039F\u03BC\u03B1\u03B4\u03BF\u03C0\u03BF\u03AF\u03B7\u03C3\u03B7 \u03B1&\u03C1\u03B9\u03B8\u03BC\u03CE\u03BD:",
    L"\u039C\u03BF\u03C1\u03C6\u03AD\u03C2 \u03CE\u03C1\u03B1\u03C2",
    L"&\u03A3\u03CD\u03BD\u03C4\u03BF\u03BC\u03B7 \u03CE\u03C1\u03B1:",
    L"\u039C\u03B5&\u03B3\u03AC\u03BB\u03B7 \u03CE\u03C1\u03B1:",
    L"\u03A3\u03CD\u03BC\u03B2\u03BF\u03BB\u03BF &\u03C0.\u03BC.:",
    L"\u03A3\u03CD\u03BC\u03B2\u03BF\u03BB\u03BF &\u03BC.\u03BC.:",
    L"\u03A3\u03B7\u03BC\u03B1\u03C3\u03AF\u03B1 \u03C3\u03B7\u03BC\u03B5\u03B9\u03BF\u03B3\u03C1\u03B1\u03C6\u03AF\u03B1\u03C2:\n\nh = \u03CE\u03C1\u03B1   m = \u03BB\u03B5\u03C0\u03C4\u03CC\ns = \u03B4\u03B5\u03C5\u03C4\u03B5\u03C1\u03CC\u03BB\u03B5\u03C0\u03C4\u03BF (\u03BC\u03CC\u03BD\u03BF \u03BC\u03B5\u03B3\u03AC\u03BB\u03B7 \u03CE\u03C1\u03B1)\ntt = \u03C0.\u03BC. \u03AE \u03BC.\u03BC.\n\nh/H = 12/24 \u03CE\u03C1\u03B5\u03C2\n\nhh, mm, ss = \u03B5\u03BC\u03C6\u03AC\u03BD\u03B9\u03C3\u03B7 \u03B1\u03C1\u03C7\u03B9\u03BA\u03BF\u03CD \u03BC\u03B7\u03B4\u03B5\u03BD\u03B9\u03BA\u03BF\u03CD\nh, m, s = \u03C7\u03C9\u03C1\u03AF\u03C2 \u03B5\u03BC\u03C6\u03AC\u03BD\u03B9\u03C3\u03B7 \u03B1\u03C1\u03C7\u03B9\u03BA\u03BF\u03CD \u03BC\u03B7\u03B4\u03B5\u03BD\u03B9\u03BA\u03BF\u03CD",
    L"\u039C\u03BF\u03C1\u03C6\u03AD\u03C2 \u03B7\u03BC\u03B5\u03C1\u03BF\u03BC\u03B7\u03BD\u03AF\u03B1\u03C2",
    L"\u03A3\u03B7\u03BC\u03B1\u03C3\u03AF\u03B1 \u03C3\u03B7\u03BC\u03B5\u03B9\u03BF\u03B3\u03C1\u03B1\u03C6\u03AF\u03B1\u03C2:\nd, dd = \u03B7\u03BC\u03AD\u03C1\u03B1;  ddd, dddd = \u03B7\u03BC\u03AD\u03C1\u03B1 \u03B5\u03B2\u03B4\u03BF\u03BC\u03AC\u03B4\u03B1\u03C2;  M = \u03BC\u03AE\u03BD\u03B1\u03C2;  y = \u03AD\u03C4\u03BF\u03C2",
    L"\u0397\u03BC\u03B5\u03C1\u03BF\u03BB\u03CC\u03B3\u03B9\u03BF",
    L"\u038C\u03C4\u03B1\u03BD \u03B5\u03B9\u03C3\u03AC\u03B3\u03B5\u03C4\u03B1\u03B9 \u03AD\u03BD\u03B1 \u03AD\u03C4\u03BF\u03C2 \u03BC\u03B5 \u03B4\u03CD\u03BF \u03C8\u03B7\u03C6\u03AF\u03B1, \u03B5\u03C1\u03BC\u03B7\u03BD\u03B5\u03CD\u03C3\u03C4\u03B5 \u03C4\u03BF \u03C9\u03C2 \u03AD\u03C4\u03BF\u03C2 &\u03BC\u03B5\u03C4\u03B1\u03BE\u03CD:",
    L"\u03BA\u03B1\u03B9",
    L"\u03A0\u03C1\u03CE\u03C4\u03B7 \u03B7\u03BC\u03AD\u03C1\u03B1 \u03C4\u03B7\u03C2 \u03B5\u03B2&\u03B4\u03BF\u03BC\u03AC\u03B4\u03B1\u03C2:",
    L"&\u03A4\u03CD\u03C0\u03BF\u03C2 \u03B7\u03BC\u03B5\u03C1\u03BF\u03BB\u03BF\u03B3\u03AF\u03BF\u03C5:",
    L"\u03A0\u03C1\u03BF\u03C3\u03B1\u03C1\u03BC\u03BF\u03B3\u03AE \u03B7\u03BC\u03B5\u03C1\u03BF\u03BC\u03B7\u03BD\u03AF\u03B1\u03C2 &\u03A7\u03AF\u03C4\u03B6\u03C1\u03B9 \u03C3\u03B5:",
    L"\u039C\u03C0\u03BF\u03C1\u03B5\u03AF\u03C4\u03B5 \u03BD\u03B1 \u03B5\u03BB\u03AD\u03B3\u03C7\u03B5\u03C4\u03B5 \u03C4\u03BF\u03BD \u03C4\u03C1\u03CC\u03C0\u03BF \u03BC\u03B5 \u03C4\u03BF\u03BD \u03BF\u03C0\u03BF\u03AF\u03BF \u03BF\u03C1\u03B9\u03C3\u03BC\u03AD\u03BD\u03B1 \u03C0\u03C1\u03BF\u03B3\u03C1\u03AC\u03BC\u03BC\u03B1\u03C4\u03B1 \u03C4\u03B1\u03BE\u03B9\u03BD\u03BF\u03BC\u03BF\u03CD\u03BD \u03C7\u03B1\u03C1\u03B1\u03BA\u03C4\u03AE\u03C1\u03B5\u03C2, \u03BB\u03AD\u03BE\u03B5\u03B9\u03C2, \u03B1\u03C1\u03C7\u03B5\u03AF\u03B1 \u03BA\u03B1\u03B9 \u03C6\u03B1\u03BA\u03AD\u03BB\u03BF\u03C5\u03C2.",
    L"&\u0395\u03C0\u03B9\u03BB\u03AD\u03BE\u03C4\u03B5 \u03BC\u03AD\u03B8\u03BF\u03B4\u03BF \u03C4\u03B1\u03BE\u03B9\u03BD\u03CC\u03BC\u03B7\u03C3\u03B7\u03C2:",
    L"\u039F\u03C1\u03B9\u03C3\u03BC\u03AD\u03BD\u03B1 \u03C0\u03C1\u03BF\u03B3\u03C1\u03AC\u03BC\u03BC\u03B1\u03C4\u03B1, \u03C3\u03C5\u03BC\u03C0\u03B5\u03C1\u03B9\u03BB\u03B1\u03BC\u03B2\u03B1\u03BD\u03BF\u03BC\u03AD\u03BD\u03C9\u03BD \u03C4\u03C9\u03BD Windows, \u03BC\u03C0\u03BF\u03C1\u03B5\u03AF \u03BD\u03B1 \u03C0\u03B1\u03C1\u03AD\u03C7\u03BF\u03C5\u03BD \u03B5\u03C0\u03B9\u03C0\u03BB\u03AD\u03BF\u03BD \u03C0\u03B5\u03C1\u03B9\u03B5\u03C7\u03CC\u03BC\u03B5\u03BD\u03BF \u03B3\u03B9\u03B1 \u03BC\u03B9\u03B1 \u03C3\u03C5\u03B3\u03BA\u03B5\u03BA\u03C1\u03B9\u03BC\u03AD\u03BD\u03B7 \u03C4\u03BF\u03C0\u03BF\u03B8\u03B5\u03C3\u03AF\u03B1. \u039F\u03C1\u03B9\u03C3\u03BC\u03AD\u03BD\u03B5\u03C2 \u03C5\u03C0\u03B7\u03C1\u03B5\u03C3\u03AF\u03B5\u03C2 \u03C0\u03B1\u03C1\u03AD\u03C7\u03BF\u03C5\u03BD \u03C4\u03BF\u03C0\u03B9\u03BA\u03AD\u03C2 \u03C0\u03BB\u03B7\u03C1\u03BF\u03C6\u03BF\u03C1\u03AF\u03B5\u03C2, \u03CC\u03C0\u03C9\u03C2 \u03B5\u03B9\u03B4\u03AE\u03C3\u03B5\u03B9\u03C2 \u03BA\u03B1\u03B9 \u03BA\u03B1\u03B9\u03C1\u03CC.",
    L"\u03A4\u03C1\u03AD\u03C7\u03BF\u03C5\u03C3\u03B1 &\u03C4\u03BF\u03C0\u03BF\u03B8\u03B5\u03C3\u03AF\u03B1:",
    L"\u0394\u03B5\u03AF\u03C4\u03B5 \u03B5\u03C0\u03AF\u03C3\u03B7\u03C2",
    L"\u039F\u03B9 \u03C1\u03C5\u03B8\u03BC\u03AF\u03C3\u03B5\u03B9\u03C2 \u03C4\u03BF\u03C5 \u03C4\u03C1\u03AD\u03C7\u03BF\u03BD\u03C4\u03BF\u03C2 \u03C7\u03C1\u03AE\u03C3\u03C4\u03B7, \u03C4\u03B7\u03C2 \u03BF\u03B8\u03CC\u03BD\u03B7\u03C2 \u03C5\u03C0\u03BF\u03B4\u03BF\u03C7\u03AE\u03C2 (\u03BB\u03BF\u03B3\u03B1\u03C1\u03B9\u03B1\u03C3\u03BC\u03BF\u03AF \u03C3\u03C5\u03C3\u03C4\u03AE\u03BC\u03B1\u03C4\u03BF\u03C2) \u03BA\u03B1\u03B9 \u03C4\u03C9\u03BD \u03BD\u03AD\u03C9\u03BD \u03BB\u03BF\u03B3\u03B1\u03C1\u03B9\u03B1\u03C3\u03BC\u03CE\u03BD \u03C7\u03C1\u03B7\u03C3\u03C4\u03CE\u03BD \u03B5\u03BC\u03C6\u03B1\u03BD\u03AF\u03B6\u03BF\u03BD\u03C4\u03B1\u03B9 \u03C0\u03B1\u03C1\u03B1\u03BA\u03AC\u03C4\u03C9.",
    L"* \u03A0\u03C1\u03BF\u03C3\u03B1\u03C1\u03BC\u03BF\u03C3\u03BC\u03AD\u03BD\u03B7 \u03C4\u03BF\u03C0\u03B9\u03BA\u03AE \u03C1\u03CD\u03B8\u03BC\u03B9\u03C3\u03B7",
    L"\u0391\u03BD\u03C4\u03B9\u03B3\u03C1\u03B1\u03C6\u03AE \u03C4\u03C1\u03B5\u03C7\u03BF\u03C5\u03C3\u03CE\u03BD \u03C1\u03C5\u03B8\u03BC\u03AF\u03C3\u03B5\u03C9\u03BD \u03C3\u03B5:",
    L"\u039F\u03B8\u03CC\u03BD\u03B7 \u03C5\u03C0\u03BF\u03B4\u03BF\u03C7\u03AE\u03C2 \u03BA\u03B1\u03B9 \u03BB\u03BF\u03B3\u03B1\u03C1\u03B9\u03B1\u03C3\u03BC\u03BF\u03AF &\u03C3\u03C5\u03C3\u03C4\u03AE\u03BC\u03B1\u03C4\u03BF\u03C2",
    L"&\u039D\u03AD\u03BF\u03B9 \u03BB\u03BF\u03B3\u03B1\u03C1\u03B9\u03B1\u03C3\u03BC\u03BF\u03AF \u03C7\u03C1\u03B7\u03C3\u03C4\u03CE\u03BD",
    L"\u0397 \u03B3\u03BB\u03CE\u03C3\u03C3\u03B1 \u03B5\u03BC\u03C6\u03AC\u03BD\u03B9\u03C3\u03B7\u03C2 \u03C4\u03C9\u03BD \u03BD\u03AD\u03C9\u03BD \u03BB\u03BF\u03B3\u03B1\u03C1\u03B9\u03B1\u03C3\u03BC\u03CE\u03BD \u03C7\u03C1\u03B7\u03C3\u03C4\u03CE\u03BD \u03BA\u03BB\u03B7\u03C1\u03BF\u03BD\u03BF\u03BC\u03B5\u03AF\u03C4\u03B1\u03B9 \u03C0\u03C1\u03BF\u03C2 \u03C4\u03BF \u03C0\u03B1\u03C1\u03CC\u03BD \u03B1\u03C0\u03CC \u03C4\u03B7 \u03B3\u03BB\u03CE\u03C3\u03C3\u03B1 \u03B5\u03BC\u03C6\u03AC\u03BD\u03B9\u03C3\u03B7\u03C2 \u03C4\u03B7\u03C2 \u03BF\u03B8\u03CC\u03BD\u03B7\u03C2 \u03C5\u03C0\u03BF\u03B4\u03BF\u03C7\u03AE\u03C2.",
    nullptr,
    L"\u0386\u03BA\u03C5\u03C1\u03BF",
    L"\u0395\u03C0\u03B9\u03BB\u03AD\u03BE\u03C4\u03B5 \u03C4\u03B7 \u03B3\u03BB\u03CE\u03C3\u03C3\u03B1 (\u03C4\u03BF\u03C0\u03B9\u03BA\u03AE \u03C1\u03CD\u03B8\u03BC\u03B9\u03C3\u03B7 \u03C3\u03C5\u03C3\u03C4\u03AE\u03BC\u03B1\u03C4\u03BF\u03C2) \u03C0\u03BF\u03C5 \u03B8\u03B1 \u03C7\u03C1\u03B7\u03C3\u03B9\u03BC\u03BF\u03C0\u03BF\u03B9\u03B5\u03AF\u03C4\u03B1\u03B9 \u03BA\u03B1\u03C4\u03AC \u03C4\u03B7\u03BD \u03B5\u03BC\u03C6\u03AC\u03BD\u03B9\u03C3\u03B7 \u03BA\u03B5\u03B9\u03BC\u03AD\u03BD\u03BF\u03C5 \u03C3\u03B5 \u03C0\u03C1\u03BF\u03B3\u03C1\u03AC\u03BC\u03BC\u03B1\u03C4\u03B1 \u03C0\u03BF\u03C5 \u03B4\u03B5\u03BD \u03C5\u03C0\u03BF\u03C3\u03C4\u03B7\u03C1\u03AF\u03B6\u03BF\u03C5\u03BD Unicode. \u0391\u03C5\u03C4\u03AE \u03B7 \u03C1\u03CD\u03B8\u03BC\u03B9\u03C3\u03B7 \u03B5\u03C0\u03B7\u03C1\u03B5\u03AC\u03B6\u03B5\u03B9 \u03CC\u03BB\u03BF\u03C5\u03C2 \u03C4\u03BF\u03C5\u03C2 \u03BB\u03BF\u03B3\u03B1\u03C1\u03B9\u03B1\u03C3\u03BC\u03BF\u03CD\u03C2 \u03C7\u03C1\u03B7\u03C3\u03C4\u03CE\u03BD \u03C3\u03C4\u03BF\u03BD \u03C5\u03C0\u03BF\u03BB\u03BF\u03B3\u03B9\u03C3\u03C4\u03AE.",
    L"\u03A4\u03C1\u03AD\u03C7\u03BF\u03C5\u03C3\u03B1 \u03C4\u03BF\u03C0\u03B9\u03BA\u03AE \u03C1\u03CD\u03B8\u03BC\u03B9\u03C3\u03B7 \u03C3&\u03C5\u03C3\u03C4\u03AE\u03BC\u03B1\u03C4\u03BF\u03C2:",
    L"<A>\u0391\u03BB\u03BB\u03B1\u03B3\u03AE \u03BC\u03B5\u03B8\u03CC\u03B4\u03BF\u03C5 \u03C4\u03B1\u03BE\u03B9\u03BD\u03CC\u03BC\u03B7\u03C3\u03B7\u03C2</A>",
    L"<A>\u03A4\u03B9 \u03C3\u03B7\u03BC\u03B1\u03AF\u03BD\u03B5\u03B9 \u03B1\u03C5\u03C4\u03AE \u03B7 \u03C3\u03B7\u03BC\u03B5\u03B9\u03BF\u03B3\u03C1\u03B1\u03C6\u03AF\u03B1;</A>",
    L"<A>\u039C\u03AC\u03B8\u03B5\u03C4\u03B5 \u03C0\u03B5\u03C1\u03B9\u03C3\u03C3\u03CC\u03C4\u03B5\u03C1\u03B1 online \u03B3\u03B9\u03B1 \u03C4\u03B7\u03BD \u03B1\u03BB\u03BB\u03B1\u03B3\u03AE \u03B3\u03BB\u03C9\u03C3\u03C3\u03CE\u03BD \u03BA\u03B1\u03B9 \u03BC\u03BF\u03C1\u03C6\u03CE\u03BD \u03C0\u03B5\u03C1\u03B9\u03BF\u03C7\u03CE\u03BD</A>",
    L"<A>\u03A0\u03CE\u03C2 \u03BC\u03C0\u03BF\u03C1\u03CE \u03BD\u03B1 \u03B1\u03BB\u03BB\u03AC\u03BE\u03C9 \u03C4\u03B7 \u03B4\u03B9\u03AC\u03C4\u03B1\u03BE\u03B7 \u03C0\u03BB\u03B7\u03BA\u03C4\u03C1\u03BF\u03BB\u03BF\u03B3\u03AF\u03BF\u03C5 \u03B3\u03B9\u03B1 \u03C4\u03B7\u03BD \u03BF\u03B8\u03CC\u03BD\u03B7 \u03C5\u03C0\u03BF\u03B4\u03BF\u03C7\u03AE\u03C2;</A>",
    L"<A>\u03A0\u03CE\u03C2 \u03BC\u03C0\u03BF\u03C1\u03CE \u03BD\u03B1 \u03B5\u03B3\u03BA\u03B1\u03C4\u03B1\u03C3\u03C4\u03AE\u03C3\u03C9 \u03B5\u03C0\u03B9\u03C0\u03BB\u03AD\u03BF\u03BD \u03B3\u03BB\u03CE\u03C3\u03C3\u03B5\u03C2;</A>",
    L"<A>\u03A0\u03B5\u03AF\u03C4\u03B5 \u03BC\u03BF\u03C5 \u03C0\u03B5\u03C1\u03B9\u03C3\u03C3\u03CC\u03C4\u03B5\u03C1\u03B1 \u03B3\u03B9\u03B1 \u03B1\u03C5\u03C4\u03BF\u03CD\u03C2 \u03C4\u03BF\u03C5\u03C2 \u03BB\u03BF\u03B3\u03B1\u03C1\u03B9\u03B1\u03C3\u03BC\u03BF\u03CD\u03C2</A>",
    L"<A>\u03A4\u03B9 \u03B5\u03AF\u03BD\u03B1\u03B9 \u03B7 \u03C4\u03BF\u03C0\u03B9\u03BA\u03AE \u03C1\u03CD\u03B8\u03BC\u03B9\u03C3\u03B7 \u03C3\u03C5\u03C3\u03C4\u03AE\u03BC\u03B1\u03C4\u03BF\u03C2;</A>",
    L"<A>\u03A0\u03C1\u03BF\u03B5\u03C0\u03B9\u03BB\u03B5\u03B3\u03BC\u03AD\u03BD\u03B7 \u03C4\u03BF\u03C0\u03BF\u03B8\u03B5\u03C3\u03AF\u03B1</A>",
};
static const wchar_t* const kTitleTr_EL[11] = {
    L"\u039C\u03BF\u03C1\u03C6\u03AD\u03C2",
    L"\u03A0\u03BB\u03B7\u03BA\u03C4\u03C1\u03BF\u03BB\u03CC\u03B3\u03B9\u03B1 \u03BA\u03B1\u03B9 \u03B3\u03BB\u03CE\u03C3\u03C3\u03B5\u03C2",
    L"\u0394\u03B9\u03B1\u03C7\u03B5\u03AF\u03C1\u03B9\u03C3\u03B7",
    L"\u0391\u03C1\u03B9\u03B8\u03BC\u03BF\u03AF",
    L"\u039D\u03CC\u03BC\u03B9\u03C3\u03BC\u03B1",
    L"\u038F\u03C1\u03B1",
    L"\u0397\u03BC\u03B5\u03C1\u03BF\u03BC\u03B7\u03BD\u03AF\u03B1",
    L"\u03A4\u03B1\u03BE\u03B9\u03BD\u03CC\u03BC\u03B7\u03C3\u03B7",
    L"\u03A4\u03BF\u03C0\u03BF\u03B8\u03B5\u03C3\u03AF\u03B1",
    L"\u03A1\u03C5\u03B8\u03BC\u03AF\u03C3\u03B5\u03B9\u03C2 \u03BF\u03B8\u03CC\u03BD\u03B7\u03C2 \u03C5\u03C0\u03BF\u03B4\u03BF\u03C7\u03AE\u03C2 \u03BA\u03B1\u03B9 \u03BD\u03AD\u03C9\u03BD \u03BB\u03BF\u03B3\u03B1\u03C1\u03B9\u03B1\u03C3\u03BC\u03CE\u03BD \u03C7\u03C1\u03B7\u03C3\u03C4\u03CE\u03BD",
    L"\u03A0\u03B5\u03C1\u03B9\u03BF\u03C7\u03AE \u03BA\u03B1\u03B9 \u03B3\u03BB\u03CE\u03C3\u03C3\u03B1",
};
// ================= \u0627\u0644\u0639\u0631\u0628\u064A\u0629 (ar-SA, RTL) =================
static const wchar_t* const kStrTr_AR[66] = {
    L"\u0627\u0644\u0645\u0646\u0637\u0642\u0629 \u0648\u0627\u0644\u0644\u063A\u0629",
    L"\u062A\u0643\u0648\u064A\u0646 \u0627\u0644\u0625\u0639\u062F\u0627\u062F\u0627\u062A \u0627\u0644\u062E\u0627\u0635\u0629 \u0628\u0643\u064A\u0641\u064A\u0629 \u0639\u0631\u0636 \u0627\u0644\u0644\u063A\u0627\u062A \u0648\u0627\u0644\u0623\u0631\u0642\u0627\u0645 \u0648\u0627\u0644\u0623\u0648\u0642\u0627\u062A \u0648\u0627\u0644\u062A\u0648\u0627\u0631\u064A\u062E.",
    L"\u062A\u063A\u064A\u064A\u0631 \u0627\u0644\u062A\u0646\u0633\u064A\u0642",
    L"\u0625\u0639\u062F\u0627\u062F \u0648\u0627\u062D\u062F \u0623\u0648 \u0623\u0643\u062B\u0631 \u0645\u0646 \u0627\u0644\u0625\u0639\u062F\u0627\u062F\u0627\u062A \u0627\u0644\u0625\u0642\u0644\u064A\u0645\u064A\u0629 \u063A\u064A\u0631 \u0635\u0627\u0644\u062D. \u064A\u0631\u062C\u0649 \u0645\u0631\u0627\u062C\u0639\u0629 \u0627\u0644\u0625\u0639\u062F\u0627\u062F\u0627\u062A \u0627\u0644\u0645\u062E\u0635\u0635\u0629 \u0648\u062A\u0635\u062D\u064A\u062D\u0647\u0627 \u0644\u062D\u0644 \u0627\u0644\u0645\u0634\u0643\u0644\u0629.",
    nullptr,
    nullptr,
    L"\u0645\u062A\u0631\u064A",
    nullptr,
    L"\u062D\u0631\u0641 \u0648\u0627\u062D\u062F \u0623\u0648 \u0623\u0643\u062B\u0631 \u0645\u0646 \u0627\u0644\u0623\u062D\u0631\u0641 \u0627\u0644\u062A\u064A \u0623\u062F\u062E\u0644\u062A\u0647\u0627 \u0641\u064A \u0647\u0630\u0627 \u0627\u0644\u062D\u0642\u0644 \u063A\u064A\u0631 \u0635\u0627\u0644\u062D. \u064A\u0631\u062C\u0649 \u0645\u062D\u0627\u0648\u0644\u0629 \u0627\u0633\u062A\u062E\u062F\u0627\u0645 \u0623\u062D\u0631\u0641 \u0623\u062E\u0631\u0649.",
    L"\u062D\u0631\u0641 \u0648\u0627\u062D\u062F \u0623\u0648 \u0623\u0643\u062B\u0631 \u0645\u0646 \u0627\u0644\u0623\u062D\u0631\u0641 \u0627\u0644\u062A\u064A \u0623\u062F\u062E\u0644\u062A\u0647\u0627 \u0644\u0640 %s \u063A\u064A\u0631 \u0635\u0627\u0644\u062D.  \u064A\u0631\u062C\u0649 \u0645\u062D\u0627\u0648\u0644\u0629 \u0627\u0633\u062A\u062E\u062F\u0627\u0645 \u062D\u0631\u0641 \u0645\u062E\u062A\u0644\u0641 \u0623\u0648 \u0625\u062F\u062E\u0627\u0644 \u0645\u0633\u0627\u0641\u0629.",
    L"\u0627\u0644\u0631\u0645\u0632 \u0627\u0644\u0639\u0634\u0631\u064A",
    L"\u0639\u0644\u0627\u0645\u0629 \u0627\u0644\u0637\u0631\u062D",
    L"\u0631\u0645\u0632 \u062A\u062C\u0645\u064A\u0639 \u0627\u0644\u0623\u0631\u0642\u0627\u0645",
    L"\u0631\u0645\u0632 \u0635",
    L"\u0631\u0645\u0632 \u0645",
    L"\u0631\u0645\u0632 \u0627\u0644\u0639\u0645\u0644\u0629",
    L"\u0627\u0644\u0631\u0645\u0632 \u0627\u0644\u0639\u0634\u0631\u064A \u0644\u0644\u0639\u0645\u0644\u0629",
    L"\u0631\u0645\u0632 \u062A\u062C\u0645\u064A\u0639 \u0623\u0631\u0642\u0627\u0645 \u0627\u0644\u0639\u0645\u0644\u0629",
    L"\u062D\u0631\u0641 \u0648\u0627\u062D\u062F \u0623\u0648 \u0623\u0643\u062B\u0631 \u0645\u0646 \u0627\u0644\u0623\u062D\u0631\u0641 \u0627\u0644\u062A\u064A \u0623\u062F\u062E\u0644\u062A\u0647\u0627 \u0644\u0644\u062A\u0646\u0633\u064A\u0642 %s \u063A\u064A\u0631 \u0635\u0627\u0644\u062D. \u064A\u0631\u062C\u0649 \u0645\u062D\u0627\u0648\u0644\u0629 \u0627\u0633\u062A\u062E\u062F\u0627\u0645 \u0623\u062D\u0631\u0641 \u0623\u062E\u0631\u0649.",
    L"\u0627\u0644\u0648\u0642\u062A \u0627\u0644\u0637\u0648\u064A\u0644",
    L"\u0627\u0644\u062A\u0627\u0631\u064A\u062E \u0627\u0644\u0642\u0635\u064A\u0631",
    L"\u0627\u0644\u062A\u0627\u0631\u064A\u062E \u0627\u0644\u0637\u0648\u064A\u0644",
    L"\u064A\u062C\u0628 \u0623\u0646 \u062A\u0643\u0648\u0646 \u0627\u0644\u0642\u064A\u0645\u0629 \u0641\u064A \u0647\u0630\u0627 \u0627\u0644\u062D\u0642\u0644 \u0631\u0642\u0645\u064B\u0627 \u0645\u0646 99 \u0625\u0644\u0649 9999. \u064A\u0631\u062C\u0649 \u0645\u062D\u0627\u0648\u0644\u0629 \u0627\u0633\u062A\u062E\u062F\u0627\u0645 \u0631\u0642\u0645 \u0622\u062E\u0631.",
    L"\u0627\u0644\u0648\u0642\u062A \u0627\u0644\u0642\u0635\u064A\u0631",
    L"&\u0627\u0644\u062A\u0646\u0633\u064A\u0642:",
    L"&\u0627\u0644\u062A\u0646\u0633\u064A\u0642: (* \u0625\u0639\u062F\u0627\u062F \u0625\u0642\u0644\u064A\u0645\u064A \u0645\u062E\u0635\u0635)",
    nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
    L"\u062A\u0645 \u062A\u063A\u064A\u064A\u0631 \u0627\u0644\u0625\u0639\u062F\u0627\u062F \u0627\u0644\u0625\u0642\u0644\u064A\u0645\u064A \u0644\u0644\u0646\u0638\u0627\u0645. \u064A\u062C\u0628 \u0625\u0639\u0627\u062F\u0629 \u062A\u0634\u063A\u064A\u0644 Windows \u0644\u062A\u0635\u0628\u062D \u0627\u0644\u062A\u063A\u064A\u064A\u0631\u0627\u062A \u0633\u0627\u0631\u064A\u0629 \u0627\u0644\u0645\u0641\u0639\u0648\u0644.",
    L"\u062A\u063A\u064A\u064A\u0631 \u0627\u0644\u0625\u0639\u062F\u0627\u062F\u0627\u062A \u0627\u0644\u0625\u0642\u0644\u064A\u0645\u064A\u0629",
    L"\u062A\u0639\u0630\u0631 \u062A\u062D\u0645\u064A\u0644 \u0627\u0644\u0644\u063A\u0629 \u0627\u0644\u0645\u062D\u062F\u062F\u0629. \u064A\u0631\u062C\u0649 \u0627\u0644\u0627\u062A\u0635\u0627\u0644 \u0628\u0645\u0633\u0624\u0648\u0644 \u0627\u0644\u0646\u0638\u0627\u0645.",
    L"\u062A\u0645 \u062A\u063A\u064A\u064A\u0631 \u0644\u063A\u0629 \u0639\u0631\u0636 \u0627\u0644\u0646\u0638\u0627\u0645. \u064A\u062C\u0628 \u0625\u0639\u0627\u062F\u0629 \u062A\u0634\u063A\u064A\u0644 Windows \u0644\u062A\u0635\u0628\u062D \u0627\u0644\u062A\u063A\u064A\u064A\u0631\u0627\u062A \u0633\u0627\u0631\u064A\u0629 \u0627\u0644\u0645\u0641\u0639\u0648\u0644.",
    L"\u0647\u0644 \u062A\u0631\u064A\u062F \u0645\u0633\u062D \u0643\u0627\u0641\u0629 \u062A\u062E\u0635\u064A\u0635\u0627\u062A \u0627\u0644\u062A\u0646\u0633\u064A\u0642 \u0627\u0644\u062D\u0627\u0644\u064A\u061F",
    L"\u0647\u0644 \u062A\u0631\u064A\u062F \u062A\u0637\u0628\u064A\u0642 \u062A\u063A\u064A\u064A\u0631\u0627\u062A \u0627\u0644\u0644\u063A\u0629 \u0648\u0627\u0644\u0645\u0646\u0637\u0642\u0629\u061F",
    L"\u0625\u0639\u0627\u062F\u0629 \u0627\u0644\u062A\u0634\u063A\u064A\u0644 \u0627\u0644\u0622\u0646",
    L"\u0625\u0644\u063A\u0627\u0621 \u0627\u0644\u0623\u0645\u0631",
    L"\u0642\u0628\u0644 \u0625\u0639\u0627\u062F\u0629 \u0627\u0644\u062A\u0634\u063A\u064A\u0644\u060C \u0627\u062D\u0641\u0638 \u0639\u0645\u0644\u0643 \u0648\u0623\u063A\u0644\u0642 \u0643\u0627\u0641\u0629 \u0627\u0644\u0628\u0631\u0627\u0645\u062C \u0627\u0644\u0645\u0641\u062A\u0648\u062D\u0629.",
    L"\u0627\u0644\u0625\u0639\u062F\u0627\u062F \u0627\u0644\u0625\u0642\u0644\u064A\u0645\u064A \u0644\u0644\u0646\u0638\u0627\u0645",
    L"\u062A\u0639\u0630\u0631 \u062A\u062D\u0645\u064A\u0644 \u062A\u062E\u0637\u064A\u0637 \u0644\u0648\u062D\u0629 \u0627\u0644\u0645\u0641\u0627\u062A\u064A\u062D %s \u0628\u0634\u0643\u0644 \u0635\u062D\u064A\u062D.",
    L"\u0627\u0644\u0625\u0633\u0628\u0627\u0646\u064A\u0629 (\u0625\u0633\u0628\u0627\u0646\u064A\u0627)",
    L"\u0644\u0643\u064A \u064A\u0635\u0628\u062D \u062A\u063A\u064A\u064A\u0631 \u0644\u063A\u0629 \u0627\u0644\u0639\u0631\u0636 \u0633\u0627\u0631\u064A \u0627\u0644\u0645\u0641\u0639\u0648\u0644\u060C \u064A\u062C\u0628 \u062A\u0633\u062C\u064A\u0644 \u0627\u0644\u062E\u0631\u0648\u062C \u062B\u0645 \u062A\u0633\u062C\u064A\u0644 \u0627\u0644\u062F\u062E\u0648\u0644 \u0645\u0631\u0629 \u0623\u062E\u0631\u0649",
    L"\u0642\u0628\u0644 \u062A\u0633\u062C\u064A\u0644 \u0627\u0644\u062E\u0631\u0648\u062C\u060C \u0627\u062D\u0641\u0638 \u0639\u0645\u0644\u0643 \u0648\u0623\u063A\u0644\u0642 \u0643\u0627\u0641\u0629 \u0627\u0644\u0628\u0631\u0627\u0645\u062C \u0627\u0644\u0645\u0641\u062A\u0648\u062D\u0629.",
    L"\u062A\u0633\u062C\u064A\u0644 \u0627\u0644\u062E\u0631\u0648\u062C \u0627\u0644\u0622\u0646",
    L"\u0625\u0644\u063A\u0627\u0621 \u0627\u0644\u0623\u0645\u0631",
    L"\u062A\u063A\u064A\u064A\u0631 \u0644\u063A\u0629 \u0627\u0644\u0639\u0631\u0636",
    L"\u0646\u0648\u0635\u064A \u0628\u062A\u0637\u0628\u064A\u0642 \u0627\u0644\u062A\u063A\u064A\u064A\u0631\u0627\u062A \u0642\u0628\u0644 \u0625\u062C\u0631\u0627\u0621 \u062A\u063A\u064A\u064A\u0631\u0627\u062A \u0623\u062E\u0631\u0649 \u0639\u0644\u0649 \u0627\u0644\u0646\u0638\u0627\u0645\u060C \u062D\u062A\u0649 \u064A\u0639\u0643\u0633\u0647\u0627 \u0627\u0644\u0643\u0645\u0628\u064A\u0648\u062A\u0631.",
    L"\u062A\u0637\u0628\u064A\u0642",
    L"\u0625\u0644\u063A\u0627\u0621 \u0627\u0644\u0623\u0645\u0631",
    L"\u062A\u0639\u0630\u0631 \u0625\u0643\u0645\u0627\u0644 \u0627\u0644\u0639\u0645\u0644\u064A\u0629",
    L"\u0627\u0644\u0645\u0633\u062A\u062E\u062F\u0645 \u0627\u0644\u062D\u0627\u0644\u064A",
    L"\u0634\u0627\u0634\u0629 \u0627\u0644\u062A\u0631\u062D\u064A\u0628",
    L"\u062D\u0633\u0627\u0628\u0627\u062A \u062C\u062F\u064A\u062F\u0629",
    L"\u0644\u063A\u0629 \u0627\u0644\u0639\u0631\u0636:",
    L"\u0644\u063A\u0629 \u0627\u0644\u0625\u062F\u062E\u0627\u0644:",
    L"\u0627\u0644\u062A\u0646\u0633\u064A\u0642:",
    L"\u0627\u0644\u0645\u0648\u0642\u0639:",
    L"\u062A\u0639\u0630\u0631 \u0642\u0631\u0627\u0621\u0629 \u0627\u0644\u0645\u0639\u0644\u0645\u0629",
    L"\u0627\u0644\u0633\u064A\u0627\u0642",
    L"\u0623\u0628\u062F\u064B\u0627",
    L"\u0623\u0635\u0644\u064A",
};
static const wchar_t* const kDlgTr_AR[92] = {
    L"\u062A\u0646\u0633\u064A\u0642\u0627\u062A \u0627\u0644\u062A\u0627\u0631\u064A\u062E \u0648\u0627\u0644\u0648\u0642\u062A",
    L"\u0627\u0644\u062A\u0627\u0631\u064A\u062E &\u0627\u0644\u0642\u0635\u064A\u0631:",
    L"\u0627\u0644\u062A\u0627\u0631\u064A\u062E &\u0627\u0644\u0637\u0648\u064A\u0644:",
    L"\u0627\u0644\u0648\u0642\u062A \u0627\u0644\u0642&\u0635\u064A\u0631:",
    L"\u0627\u0644\u0648\u0642\u062A \u0627\u0644\u0637&\u0648\u064A\u0644:",
    L"\u0627\u0644\u064A\u0648\u0645 \u0627\u0644\u0623\u0648\u0644 \u0641\u064A \u0627\u0644\u0623\u0633&\u0628\u0648\u0639:",
    L"\u0623\u0645\u062B\u0644\u0629",
    L"\u0627\u0644\u062A\u0627\u0631\u064A\u062E \u0627\u0644\u0642\u0635\u064A\u0631:",
    L"\u0627\u0644\u062A\u0627\u0631\u064A\u062E \u0627\u0644\u0637\u0648\u064A\u0644:",
    L"\u0627\u0644\u0648\u0642\u062A \u0627\u0644\u0642\u0635\u064A\u0631:",
    L"\u0627\u0644\u0648\u0642\u062A \u0627\u0644\u0637\u0648\u064A\u0644:",
    L"\u0625\u0639\u062F\u0627\u062F\u0627\u062A &\u0625\u0636\u0627\u0641\u064A\u0629...",
    L"\u0644\u0648\u062D\u0627\u062A \u0627\u0644\u0645\u0641\u0627\u062A\u064A\u062D \u0648\u0644\u063A\u0627\u062A \u0627\u0644\u0625\u062F\u062E\u0627\u0644 \u0627\u0644\u0623\u062E\u0631\u0649",
    L"\u0644\u062A\u063A\u064A\u064A\u0631 \u0644\u0648\u062D\u0629 \u0627\u0644\u0645\u0641\u0627\u062A\u064A\u062D \u0623\u0648 \u0644\u063A\u0629 \u0627\u0644\u0625\u062F\u062E\u0627\u0644\u060C \u0627\u0646\u0642\u0631 \u0641\u0648\u0642 \u062A\u063A\u064A\u064A\u0631 \u0644\u0648\u062D\u0627\u062A \u0627\u0644\u0645\u0641\u0627\u062A\u064A\u062D.",
    L"&\u062A\u063A\u064A\u064A\u0631 \u0644\u0648\u062D\u0627\u062A \u0627\u0644\u0645\u0641\u0627\u062A\u064A\u062D...",
    L"\u0644\u063A\u0629 \u0627\u0644\u0639\u0631\u0636",
    L"\u062A\u062B\u0628\u064A\u062A \u0627\u0644\u0644\u063A\u0627\u062A \u0627\u0644\u062A\u064A \u064A\u0645\u0643\u0646 \u0623\u0646 \u064A\u0633\u062A\u062E\u062F\u0645\u0647\u0627 Windows \u0644\u0639\u0631\u0636 \u0627\u0644\u0646\u0635\u060C \u0648\u062D\u064A\u062B\u0645\u0627 \u064A\u0643\u0648\u0646 \u0645\u062F\u0639\u0648\u0645\u064B\u0627\u060C \u0644\u0644\u062A\u0639\u0631\u0641 \u0639\u0644\u0649 \u0627\u0644\u0643\u0644\u0627\u0645 \u0648\u0627\u0644\u0643\u062A\u0627\u0628\u0629 \u0627\u0644\u064A\u062F\u0648\u064A\u0629\u060C \u0623\u0648 \u0625\u0632\u0627\u0644\u062A\u0647\u0627.",
    L"\u062A&\u062B\u0628\u064A\u062A \u0627\u0644\u0644\u063A\u0627\u062A \u0623\u0648 \u0625\u0632\u0627\u0644\u062A\u0647\u0627...",
    L"\u0643\u0636\u064A\u0641 \u0644\u0627 \u064A\u0645\u0643\u0646\u0643 \u062A\u063A\u064A\u064A\u0631 \u0644\u063A\u0629 \u0627\u0644\u0639\u0631\u0636:",
    L"\u062A\u062D\u062F\u064A\u062F \u0644\u063A\u0629 \u0627\u0644\u0639\u0631\u0636 \u0645\u0642\u0641\u0644 \u0628\u0648\u0627\u0633\u0637\u0629 \u0646\u0647\u062C \u0627\u0644\u0645\u062C\u0645\u0648\u0639\u0629.",
    L"\u0627&\u062E\u062A\u0631 \u0644\u063A\u0629 \u0627\u0644\u0639\u0631\u0636:",
    L"\u0644\u0645 \u062A\u062A\u0645 \u062A\u0631\u062C\u0645\u0629 \u0628\u0639\u0636 \u0627\u0644\u0646\u0635 \u0625\u0644\u0649 \u0627\u0644\u0644\u063A\u0629 \u0627\u0644\u0645\u062D\u062F\u062F\u0629. \u0627&\u062E\u062A\u0631 \u0644\u063A\u0629 \u0623\u062E\u0631\u0649 \u0644\u064A\u0633\u062A\u062E\u062F\u0645\u0647\u0627 Windows \u0644\u0639\u0631\u0636 \u0647\u0630\u0627 \u0627\u0644\u0646\u0635:",
    L"\u0647\u0630\u0647 \u0627\u0644\u0644\u063A\u0629 \u0645\u062A\u0631\u062C\u0645\u0629 \u062C\u0632\u0626\u064A\u064B\u0627 \u0641\u0642\u0637 \u0648\u0642\u062F \u064A\u0638\u0647\u0631 \u0628\u0639\u0636 \u0627\u0644\u0646\u0635 \u0628\u0644\u063A\u0629:",
    L"\u0647\u0630\u0647 \u0627\u0644\u0644\u063A\u0629 \u0645\u062A\u0631\u062C\u0645\u0629 \u062C\u0632\u0626\u064A\u064B\u0627 \u0641\u0642\u0637 \u0623\u064A\u0636\u064B\u0627. \u0627&\u062E\u062A\u0631 \u0644\u063A\u0629 \u062B\u0627\u0644\u062B\u0629 \u0644\u064A\u0633\u062A\u062E\u062F\u0645\u0647\u0627 Windows \u0644\u0639\u0631\u0636 \u0627\u0644\u0646\u0635 \u0627\u0644\u0645\u062A\u0628\u0642\u064A:",
    L"\u0647\u0630\u0647 \u0627\u0644\u0644\u063A\u0629 \u0645\u062A\u0631\u062C\u0645\u0629 \u062C\u0632\u0626\u064A\u064B\u0627 \u0641\u0642\u0637 \u0623\u064A\u0636\u064B\u0627 \u0648\u0642\u062F \u064A\u0638\u0647\u0631 \u0628\u0639\u0636 \u0627\u0644\u0646\u0635 \u0628\u0644\u063A\u0629: ",
    L"\u0634\u0627\u0634\u0629 \u0627\u0644\u062A\u0631\u062D\u064A\u0628 \u0648\u062D\u0633\u0627\u0628\u0627\u062A \u0627\u0644\u0645\u0633\u062A\u062E\u062F\u0645\u064A\u0646 \u0627\u0644\u062C\u062F\u062F",
    L"\u0627\u0639\u0631\u0636 \u0625\u0639\u062F\u0627\u062F\u0627\u062A\u0643 \u0627\u0644\u0625\u0642\u0644\u064A\u0645\u064A\u0629 \u0648\u0627\u0646\u0633\u062E\u0647\u0627 \u0625\u0644\u0649 \u0634\u0627\u0634\u0629 \u0627\u0644\u062A\u0631\u062D\u064A\u0628 \u0648\u062D\u0633\u0627\u0628\u0627\u062A \u0627\u0644\u0646\u0638\u0627\u0645 \u0648\u062D\u0633\u0627\u0628\u0627\u062A \u0627\u0644\u0645\u0633\u062A\u062E\u062F\u0645\u064A\u0646 \u0627\u0644\u062C\u062F\u062F.",
    L"&\u0646\u0633\u062E \u0627\u0644\u0625\u0639\u062F\u0627\u062F\u0627\u062A...",
    L"\u0644\u063A\u0629 \u0627\u0644\u0628\u0631\u0627\u0645\u062C \u0627\u0644\u062A\u064A \u0644\u0627 \u062A\u062F\u0639\u0645 Unicode",
    L"\u064A\u062A\u062D\u0643\u0645 \u0647\u0630\u0627 \u0627\u0644\u0625\u0639\u062F\u0627\u062F (\u0627\u0644\u0625\u0639\u062F\u0627\u062F \u0627\u0644\u0625\u0642\u0644\u064A\u0645\u064A \u0644\u0644\u0646\u0638\u0627\u0645) \u0641\u064A \u0627\u0644\u0644\u063A\u0629 \u0627\u0644\u0645\u0633\u062A\u062E\u062F\u0645\u0629 \u0639\u0646\u062F \u0639\u0631\u0636 \u0627\u0644\u0646\u0635 \u0641\u064A \u0627\u0644\u0628\u0631\u0627\u0645\u062C \u0627\u0644\u062A\u064A \u0644\u0627 \u062A\u062F\u0639\u0645 Unicode.",
    L"\u0627\u0644\u0644\u063A\u0629 \u0627\u0644\u062D\u0627\u0644\u064A\u0629 \u0644\u0644\u0628\u0631\u0627\u0645\u062C \u0627\u0644\u062A\u064A \u0644\u0627 \u062A\u062F\u0639\u0645 Unicode:",
    nullptr,
    L"\u0627\u0644\u0625\u0639\u062F\u0627\u062F \u0627\u0644\u0625\u0642\u0644\u064A\u0645\u064A \u0644\u0644&\u0646\u0638\u0627\u0645...",
    L"\u0645\u062B\u0627\u0644",
    L"\u0645\u0648\u062C\u0628:",
    L"\u0633\u0627\u0644\u0628:",
    L"\u0627\u0644\u0631\u0645\u0632 &\u0627\u0644\u0639\u0634\u0631\u064A:",
    L"\u0639\u062F\u062F \u0627\u0644\u0623\u0631\u0642\u0627\u0645 &\u0627\u0644\u0639\u0634\u0631\u064A\u0629:",
    L"\u0631\u0645\u0632 \u062A\u062C\u0645\u064A\u0639 \u0627\u0644\u0623\u0631&\u0642\u0627\u0645:",
    L"\u062A\u062C\u0645\u064A\u0639 \u0627\u0644\u0623\u0631\u0642\u0627&\u0645:",
    L"\u0631\u0645\u0632 \u0639\u0644\u0627\u0645\u0629 \u0627\u0644\u0637&\u0631\u062D:",
    L"\u062A\u0646\u0633\u064A\u0642 \u0627\u0644\u0623\u0631\u0642\u0627\u0645 \u0627\u0644\u0633\u0627&\u0644\u0628\u0629:",
    L"\u0639\u0631\u0636 \u0627\u0644\u0623\u0635\u0641\u0627\u0631 &\u0627\u0644\u0628\u0627\u062F\u0626\u0629:",
    L"\u0641\u0627\u0635\u0644 \u0627\u0644\u0642\u0627\u0626&\u0645\u0629:",
    L"\u0646\u0638\u0627\u0645 \u0627\u0644\u0642&\u064A\u0627\u0633:",
    L"\u0627\u0644\u0623\u0631\u0642\u0627\u0645 \u0627\u0644\u0642\u064A\u0627&\u0633\u064A\u0629:",
    L"\u0627\u0633\u062A&\u062E\u062F\u0627\u0645 \u0627\u0644\u0623\u0631\u0642\u0627\u0645 \u0627\u0644\u0623\u0635\u0644\u064A\u0629:",
    L"\u0627\u0646\u0642\u0631 \u0641\u0648\u0642 \u0625\u0639\u0627\u062F\u0629 \u062A\u0639\u064A\u064A\u0646 \u0644\u0627\u0633\u062A\u0639\u0627\u062F\u0629 \u0625\u0639\u062F\u0627\u062F\u0627\u062A \u0627\u0644\u0646\u0638\u0627\u0645 \u0627\u0644\u0627\u0641\u062A\u0631\u0627\u0636\u064A\u0629 \u0644\u0644\u0623\u0631\u0642\u0627\u0645 \u0648\u0627\u0644\u0639\u0645\u0644\u0629 \u0648\u0627\u0644\u0648\u0642\u062A \u0648\u0627\u0644\u062A\u0627\u0631\u064A\u062E.",
    L"\u0625&\u0639\u0627\u062F\u0629 \u062A\u0639\u064A\u064A\u0646",
    L"\u0631\u0645\u0632 \u0627\u0644\u0639\u0645&\u0644\u0629:",
    L"\u062A\u0646\u0633\u064A\u0642 \u0627\u0644\u0639\u0645\u0644\u0629 \u0627\u0644\u0645\u0648&\u062C\u0628:",
    L"\u062A\u0646\u0633\u064A\u0642 \u0627\u0644\u0639\u0645\u0644\u0629 \u0627\u0644\u0633\u0627&\u0644\u0628:",
    L"\u0639\u062F\u062F \u0627\u0644\u0623\u0631\u0642\u0627\u0645 \u0627\u0644\u0639&\u0634\u0631\u064A\u0629:",
    L"\u0631\u0645\u0632 \u062A\u062C\u0645\u064A\u0639 \u0627\u0644\u0623\u0631\u0642\u0627&\u0645:",
    L"\u062A\u062C\u0645\u064A\u0639 \u0627\u0644\u0623\u0631&\u0642\u0627\u0645:",
    L"\u062A\u0646\u0633\u064A\u0642\u0627\u062A \u0627\u0644\u0648\u0642\u062A",
    L"\u0627\u0644\u0648\u0642\u062A \u0627\u0644\u0642&\u0635\u064A\u0631:",
    L"\u0627\u0644\u0648\u0642\u062A \u0627\u0644\u0637&\u0648\u064A\u0644:",
    L"\u0631\u0645\u0632 &\u0635:",
    L"\u0631\u0645\u0632 &\u0645:",
    L"\u0645\u0639\u0646\u0649 \u0627\u0644\u062A\u0631\u0645\u064A\u0632:\n\nh = \u0627\u0644\u0633\u0627\u0639\u0629   m = \u0627\u0644\u062F\u0642\u064A\u0642\u0629\ns = \u0627\u0644\u062B\u0627\u0646\u064A\u0629 (\u0627\u0644\u0648\u0642\u062A \u0627\u0644\u0637\u0648\u064A\u0644 \u0641\u0642\u0637)\ntt = \u0635 \u0623\u0648 \u0645\n\nh/H = 12/24 \u0633\u0627\u0639\u0629\n\nhh, mm, ss = \u0639\u0631\u0636 \u0627\u0644\u0635\u0641\u0631 \u0627\u0644\u0628\u0627\u062F\u0626\nh, m, s = \u0639\u062F\u0645 \u0639\u0631\u0636 \u0627\u0644\u0635\u0641\u0631 \u0627\u0644\u0628\u0627\u062F\u0626",
    L"\u062A\u0646\u0633\u064A\u0642\u0627\u062A \u0627\u0644\u062A\u0627\u0631\u064A\u062E",
    L"\u0645\u0639\u0646\u0649 \u0627\u0644\u062A\u0631\u0645\u064A\u0632:\nd, dd = \u0627\u0644\u064A\u0648\u0645\u061B  ddd, dddd = \u064A\u0648\u0645 \u0627\u0644\u0623\u0633\u0628\u0648\u0639\u061B  M = \u0627\u0644\u0634\u0647\u0631\u061B  y = \u0627\u0644\u0633\u0646\u0629",
    L"\u0627\u0644\u062A\u0642\u0648\u064A\u0645",
    L"\u0639\u0646\u062F \u0625\u062F\u062E\u0627\u0644 \u0633\u0646\u0629 \u0645\u0643\u0648\u0646\u0629 \u0645\u0646 \u0631\u0642\u0645\u064A\u0646\u060C \u0641\u0633\u0651\u0631\u0647\u0627 \u0639\u0644\u0649 \u0623\u0646\u0647\u0627 \u0633\u0646\u0629 &\u0628\u064A\u0646:",
    L"\u0648",
    L"\u0627\u0644\u064A\u0648\u0645 \u0627\u0644\u0623\u0648\u0644 \u0641\u064A \u0627\u0644\u0623\u0633\u0628&\u0648\u0639:",
    L"&\u0646\u0648\u0639 \u0627\u0644\u062A\u0642\u0648\u064A\u0645:",
    L"\u0636\u0628\u0637 \u0627\u0644\u062A\u0627\u0631\u064A\u062E \u0627\u0644\u0647&\u062C\u0631\u064A \u0625\u0644\u0649:",
    L"\u064A\u0645\u0643\u0646\u0643 \u0627\u0644\u062A\u062D\u0643\u0645 \u0641\u064A \u0627\u0644\u0637\u0631\u064A\u0642\u0629 \u0627\u0644\u062A\u064A \u062A\u0641\u0631\u0632 \u0628\u0647\u0627 \u0628\u0639\u0636 \u0627\u0644\u0628\u0631\u0627\u0645\u062C \u0627\u0644\u0623\u062D\u0631\u0641 \u0648\u0627\u0644\u0643\u0644\u0645\u0627\u062A \u0648\u0627\u0644\u0645\u0644\u0641\u0627\u062A \u0648\u0627\u0644\u0645\u062C\u0644\u062F\u0627\u062A.",
    L"\u0627&\u062E\u062A\u0631 \u0637\u0631\u064A\u0642\u0629 \u0627\u0644\u0641\u0631\u0632:",
    L"\u0642\u062F \u062A\u0648\u0641\u0631 \u0628\u0639\u0636 \u0627\u0644\u0628\u0631\u0627\u0645\u062C\u060C \u0628\u0645\u0627 \u0641\u064A\u0647\u0627 Windows\u060C \u0645\u062D\u062A\u0648\u0649 \u0625\u0636\u0627\u0641\u064A\u064B\u0627 \u0644\u0645\u0648\u0642\u0639 \u0645\u0639\u064A\u0646. \u062A\u0648\u0641\u0631 \u0628\u0639\u0636 \u0627\u0644\u062E\u062F\u0645\u0627\u062A \u0645\u0639\u0644\u0648\u0645\u0627\u062A \u0645\u062D\u0644\u064A\u0629\u060C \u0645\u062B\u0644 \u0627\u0644\u0623\u062E\u0628\u0627\u0631 \u0648\u0627\u0644\u0637\u0642\u0633.",
    L"\u0627\u0644\u0645\u0648\u0642\u0639 &\u0627\u0644\u062D\u0627\u0644\u064A:",
    L"\u0627\u0646\u0638\u0631 \u0623\u064A\u0636\u064B\u0627",
    L"\u062A\u0638\u0647\u0631 \u0623\u062F\u0646\u0627\u0647 \u0625\u0639\u062F\u0627\u062F\u0627\u062A \u0627\u0644\u0645\u0633\u062A\u062E\u062F\u0645 \u0627\u0644\u062D\u0627\u0644\u064A \u0648\u0634\u0627\u0634\u0629 \u0627\u0644\u062A\u0631\u062D\u064A\u0628 (\u062D\u0633\u0627\u0628\u0627\u062A \u0627\u0644\u0646\u0638\u0627\u0645) \u0648\u062D\u0633\u0627\u0628\u0627\u062A \u0627\u0644\u0645\u0633\u062A\u062E\u062F\u0645\u064A\u0646 \u0627\u0644\u062C\u062F\u062F.",
    L"* \u0625\u0639\u062F\u0627\u062F \u0625\u0642\u0644\u064A\u0645\u064A \u0645\u062E\u0635\u0635",
    L"\u0646\u0633\u062E \u0627\u0644\u0625\u0639\u062F\u0627\u062F\u0627\u062A \u0627\u0644\u062D\u0627\u0644\u064A\u0629 \u0625\u0644\u0649:",
    L"\u0634\u0627\u0634\u0629 \u0627\u0644\u062A\u0631\u062D\u064A\u0628 \u0648\u062D\u0633\u0627\u0628\u0627\u062A \u0627\u0644\u0646&\u0638\u0627\u0645",
    L"\u062D\u0633\u0627\u0628\u0627\u062A \u0627\u0644\u0645\u0633\u062A\u062E\u062F\u0645\u064A\u0646 \u0627\u0644\u062C&\u062F\u062F",
    L"\u0644\u063A\u0629 \u0639\u0631\u0636 \u062D\u0633\u0627\u0628\u0627\u062A \u0627\u0644\u0645\u0633\u062A\u062E\u062F\u0645\u064A\u0646 \u0627\u0644\u062C\u062F\u062F \u0645\u0648\u0631\u0648\u062B\u0629 \u062D\u0627\u0644\u064A\u064B\u0627 \u0645\u0646 \u0644\u063A\u0629 \u0639\u0631\u0636 \u0634\u0627\u0634\u0629 \u0627\u0644\u062A\u0631\u062D\u064A\u0628.",
    nullptr,
    L"\u0625\u0644\u063A\u0627\u0621 \u0627\u0644\u0623\u0645\u0631",
    L"\u062D\u062F\u062F \u0627\u0644\u0644\u063A\u0629 (\u0627\u0644\u0625\u0639\u062F\u0627\u062F \u0627\u0644\u0625\u0642\u0644\u064A\u0645\u064A \u0644\u0644\u0646\u0638\u0627\u0645) \u0627\u0644\u062A\u064A \u0633\u062A\u064F\u0633\u062A\u062E\u062F\u0645 \u0639\u0646\u062F \u0639\u0631\u0636 \u0627\u0644\u0646\u0635 \u0641\u064A \u0627\u0644\u0628\u0631\u0627\u0645\u062C \u0627\u0644\u062A\u064A \u0644\u0627 \u062A\u062F\u0639\u0645 Unicode. \u064A\u0624\u062B\u0631 \u0647\u0630\u0627 \u0627\u0644\u0625\u0639\u062F\u0627\u062F \u0639\u0644\u0649 \u0643\u0627\u0641\u0629 \u062D\u0633\u0627\u0628\u0627\u062A \u0627\u0644\u0645\u0633\u062A\u062E\u062F\u0645\u064A\u0646 \u0639\u0644\u0649 \u0627\u0644\u0643\u0645\u0628\u064A\u0648\u062A\u0631.",
    L"\u0627\u0644\u0625\u0639\u062F\u0627\u062F \u0627\u0644\u0625\u0642\u0644\u064A\u0645\u064A \u0627\u0644\u062D\u0627\u0644\u064A \u0644\u0644\u0646&\u0638\u0627\u0645:",
    L"<A>\u062A\u063A\u064A\u064A\u0631 \u0637\u0631\u064A\u0642\u0629 \u0627\u0644\u0641\u0631\u0632</A>",
    L"<A>\u0645\u0627\u0630\u0627 \u064A\u0639\u0646\u064A \u0647\u0630\u0627 \u0627\u0644\u062A\u0631\u0645\u064A\u0632\u061F</A>",
    L"<A>\u062A\u0639\u0631\u0641 \u0639\u0644\u0649 \u0627\u0644\u0645\u0632\u064A\u062F \u0639\u0628\u0631 \u0627\u0644\u0625\u0646\u062A\u0631\u0646\u062A \u062D\u0648\u0644 \u062A\u063A\u064A\u064A\u0631 \u0644\u063A\u0627\u062A \u0627\u0644\u0645\u0646\u0627\u0637\u0642 \u0648\u062A\u0646\u0633\u064A\u0642\u0627\u062A\u0647\u0627</A>",
    L"<A>\u0643\u064A\u0641 \u064A\u0645\u0643\u0646 \u062A\u063A\u064A\u064A\u0631 \u062A\u062E\u0637\u064A\u0637 \u0644\u0648\u062D\u0629 \u0627\u0644\u0645\u0641\u0627\u062A\u064A\u062D \u0644\u0634\u0627\u0634\u0629 \u0627\u0644\u062A\u0631\u062D\u064A\u0628\u061F</A>",
    L"<A>\u0643\u064A\u0641 \u064A\u0645\u0643\u0646 \u062A\u062B\u0628\u064A\u062A \u0644\u063A\u0627\u062A \u0625\u0636\u0627\u0641\u064A\u0629\u061F</A>",
    L"<A>\u0623\u062E\u0628\u0631\u0646\u064A \u0627\u0644\u0645\u0632\u064A\u062F \u0639\u0646 \u0647\u0630\u0647 \u0627\u0644\u062D\u0633\u0627\u0628\u0627\u062A</A>",
    L"<A>\u0645\u0627 \u0647\u0648 \u0627\u0644\u0625\u0639\u062F\u0627\u062F \u0627\u0644\u0625\u0642\u0644\u064A\u0645\u064A \u0644\u0644\u0646\u0638\u0627\u0645\u061F</A>",
    L"<A>\u0627\u0644\u0645\u0648\u0642\u0639 \u0627\u0644\u0627\u0641\u062A\u0631\u0627\u0636\u064A</A>",
};
static const wchar_t* const kTitleTr_AR[11] = {
    L"\u0627\u0644\u062A\u0646\u0633\u064A\u0642\u0627\u062A",
    L"\u0644\u0648\u062D\u0627\u062A \u0627\u0644\u0645\u0641\u0627\u062A\u064A\u062D \u0648\u0627\u0644\u0644\u063A\u0627\u062A",
    L"\u0625\u062F\u0627\u0631\u064A",
    L"\u0627\u0644\u0623\u0631\u0642\u0627\u0645",
    L"\u0627\u0644\u0639\u0645\u0644\u0629",
    L"\u0627\u0644\u0648\u0642\u062A",
    L"\u0627\u0644\u062A\u0627\u0631\u064A\u062E",
    L"\u0627\u0644\u0641\u0631\u0632",
    L"\u0627\u0644\u0645\u0648\u0642\u0639",
    L"\u0625\u0639\u062F\u0627\u062F\u0627\u062A \u0634\u0627\u0634\u0629 \u0627\u0644\u062A\u0631\u062D\u064A\u0628 \u0648\u062D\u0633\u0627\u0628\u0627\u062A \u0627\u0644\u0645\u0633\u062A\u062E\u062F\u0645\u064A\u0646 \u0627\u0644\u062C\u062F\u062F",
    L"\u0627\u0644\u0645\u0646\u0637\u0642\u0629 \u0648\u0627\u0644\u0644\u063A\u0629",
};
// ================= \u4E2D\u6587\u7B80\u4F53 (zh-CN) =================
static const wchar_t* const kStrTr_ZH[66] = {
    L"\u533A\u57DF\u548C\u8BED\u8A00",
    L"\u914D\u7F6E\u8BED\u8A00\u3001\u6570\u5B57\u3001\u65F6\u95F4\u548C\u65E5\u671F\u7684\u663E\u793A\u8BBE\u7F6E\u3002",
    L"\u66F4\u6539\u683C\u5F0F",
    L"\u4E00\u4E2A\u6216\u591A\u4E2A\u533A\u57DF\u8BBE\u7F6E\u65E0\u6548\u3002\u8BF7\u68C0\u67E5\u5E76\u66F4\u6B63\u81EA\u5B9A\u4E49\u8BBE\u7F6E\u4EE5\u89E3\u51B3\u6B64\u95EE\u9898\u3002",
    nullptr,
    nullptr,
    L"\u516C\u5236",
    nullptr,
    L"\u5728\u6B64\u5B57\u6BB5\u4E2D\u8F93\u5165\u7684\u4E00\u4E2A\u6216\u591A\u4E2A\u5B57\u7B26\u65E0\u6548\u3002\u8BF7\u5C1D\u8BD5\u4F7F\u7528\u5176\u4ED6\u5B57\u7B26\u3002",
    L"\u4E3A %s \u8F93\u5165\u7684\u4E00\u4E2A\u6216\u591A\u4E2A\u5B57\u7B26\u65E0\u6548\u3002  \u8BF7\u5C1D\u8BD5\u4F7F\u7528\u5176\u4ED6\u5B57\u7B26\u6216\u8F93\u5165\u7A7A\u683C\u3002",
    L"\u5C0F\u6570\u70B9\u7B26\u53F7",
    L"\u51CF\u53F7",
    L"\u6570\u5B57\u5206\u7EC4\u7B26\u53F7",
    L"\u4E0A\u5348\u7B26\u53F7",
    L"\u4E0B\u5348\u7B26\u53F7",
    L"\u8D27\u5E01\u7B26\u53F7",
    L"\u8D27\u5E01\u5C0F\u6570\u70B9\u7B26\u53F7",
    L"\u8D27\u5E01\u6570\u5B57\u5206\u7EC4\u7B26\u53F7",
    L"\u4E3A\u683C\u5F0F %s \u8F93\u5165\u7684\u4E00\u4E2A\u6216\u591A\u4E2A\u5B57\u7B26\u65E0\u6548\u3002\u8BF7\u5C1D\u8BD5\u4F7F\u7528\u5176\u4ED6\u5B57\u7B26\u3002",
    L"\u957F\u65F6\u95F4",
    L"\u77ED\u65E5\u671F",
    L"\u957F\u65E5\u671F",
    L"\u6B64\u5B57\u6BB5\u4E2D\u7684\u503C\u5FC5\u987B\u662F 99 \u5230 9999 \u4E4B\u95F4\u7684\u6570\u5B57\u3002\u8BF7\u5C1D\u8BD5\u4F7F\u7528\u5176\u4ED6\u6570\u5B57\u3002",
    L"\u77ED\u65F6\u95F4",
    L"\u683C\u5F0F(&F):",
    L"\u683C\u5F0F(&F): (* \u81EA\u5B9A\u4E49\u533A\u57DF\u8BBE\u7F6E)",
    nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
    L"\u7CFB\u7EDF\u533A\u57DF\u8BBE\u7F6E\u5DF2\u66F4\u6539\u3002\u5FC5\u987B\u91CD\u65B0\u542F\u52A8 Windows \u624D\u80FD\u4F7F\u66F4\u6539\u751F\u6548\u3002",
    L"\u66F4\u6539\u533A\u57DF\u8BBE\u7F6E",
    L"\u65E0\u6CD5\u52A0\u8F7D\u6240\u9009\u8BED\u8A00\u3002\u8BF7\u4E0E\u7CFB\u7EDF\u7BA1\u7406\u5458\u8054\u7CFB\u3002",
    L"\u7CFB\u7EDF\u663E\u793A\u8BED\u8A00\u5DF2\u66F4\u6539\u3002\u5FC5\u987B\u91CD\u65B0\u542F\u52A8 Windows \u624D\u80FD\u4F7F\u66F4\u6539\u751F\u6548\u3002",
    L"\u662F\u5426\u8981\u6E05\u9664\u5F53\u524D\u683C\u5F0F\u7684\u6240\u6709\u81EA\u5B9A\u4E49\u8BBE\u7F6E?",
    L"\u662F\u5426\u8981\u5E94\u7528\u8BED\u8A00\u548C\u533A\u57DF\u66F4\u6539?",
    L"\u7ACB\u5373\u91CD\u65B0\u542F\u52A8",
    L"\u53D6\u6D88",
    L"\u91CD\u65B0\u542F\u52A8\u4E4B\u524D\uFF0C\u8BF7\u4FDD\u5B58\u5DE5\u4F5C\u5E76\u5173\u95ED\u6240\u6709\u6253\u5F00\u7684\u7A0B\u5E8F\u3002",
    L"\u66F4\u6539\u7CFB\u7EDF\u533A\u57DF\u8BBE\u7F6E",
    L"\u65E0\u6CD5\u6B63\u786E\u52A0\u8F7D\u952E\u76D8\u5E03\u5C40 %s\u3002",
    L"\u897F\u73ED\u7259\u8BED(\u897F\u73ED\u7259)",
    L"\u82E5\u8981\u4F7F\u663E\u793A\u8BED\u8A00\u66F4\u6539\u751F\u6548\uFF0C\u5FC5\u987B\u6CE8\u9500\u7136\u540E\u91CD\u65B0\u767B\u5F55",
    L"\u6CE8\u9500\u4E4B\u524D\uFF0C\u8BF7\u4FDD\u5B58\u5DE5\u4F5C\u5E76\u5173\u95ED\u6240\u6709\u6253\u5F00\u7684\u7A0B\u5E8F\u3002",
    L"\u7ACB\u5373\u6CE8\u9500",
    L"\u53D6\u6D88",
    L"\u66F4\u6539\u663E\u793A\u8BED\u8A00",
    L"\u5EFA\u8BAE\u5728\u8FDB\u884C\u5176\u4ED6\u7CFB\u7EDF\u66F4\u6539\u4E4B\u524D\u5E94\u7528\u66F4\u6539\uFF0C\u4EE5\u4FBF\u8BA1\u7B97\u673A\u4F1A\u53CD\u6620\u8FD9\u4E9B\u66F4\u6539\u3002",
    L"\u5E94\u7528",
    L"\u53D6\u6D88",
    L"\u65E0\u6CD5\u5B8C\u6210\u64CD\u4F5C",
    L"\u5F53\u524D\u7528\u6237",
    L"\u6B22\u8FCE\u5C4F\u5E55",
    L"\u65B0\u5E10\u6237",
    L"\u663E\u793A\u8BED\u8A00:",
    L"\u8F93\u5165\u8BED\u8A00:",
    L"\u683C\u5F0F:",
    L"\u4F4D\u7F6E:",
    L"\u65E0\u6CD5\u8BFB\u53D6\u53C2\u6570",
    L"\u4E0A\u4E0B\u6587",
    L"\u4ECE\u4E0D",
    L"\u672C\u673A",
};
static const wchar_t* const kDlgTr_ZH[92] = {
    L"\u65E5\u671F\u548C\u65F6\u95F4\u683C\u5F0F",
    L"\u77ED\u65E5\u671F(&S):",
    L"\u957F\u65E5\u671F(&L):",
    L"\u77ED\u65F6\u95F4(&S):",
    L"\u957F\u65F6\u95F4(&L):",
    L"\u4E00\u5468\u7684\u7B2C\u4E00\u5929(&W):",
    L"\u793A\u4F8B",
    L"\u77ED\u65E5\u671F:",
    L"\u957F\u65E5\u671F:",
    L"\u77ED\u65F6\u95F4:",
    L"\u957F\u65F6\u95F4:",
    L"\u5176\u4ED6\u8BBE\u7F6E(&A)...",
    L"\u952E\u76D8\u548C\u5176\u4ED6\u8F93\u5165\u8BED\u8A00",
    L"\u82E5\u8981\u66F4\u6539\u952E\u76D8\u6216\u8F93\u5165\u8BED\u8A00\uFF0C\u8BF7\u5355\u51FB\u201C\u66F4\u6539\u952E\u76D8\u201D\u3002",
    L"\u66F4\u6539\u952E\u76D8(&C)...",
    L"\u663E\u793A\u8BED\u8A00",
    L"\u5B89\u88C5\u6216\u5378\u8F7D Windows \u53EF\u7528\u4E8E\u663E\u793A\u6587\u672C\u4EE5\u53CA(\u5728\u53D7\u652F\u6301\u7684\u60C5\u51B5\u4E0B)\u8BED\u97F3\u548C\u624B\u5199\u8BC6\u522B\u7684\u8BED\u8A00\u3002",
    L"\u5B89\u88C5/\u5378\u8F7D\u8BED\u8A00(&I)...",
    L"\u4F5C\u4E3A\u6765\u5BBE\uFF0C\u60A8\u65E0\u6CD5\u66F4\u6539\u663E\u793A\u8BED\u8A00:",
    L"\u663E\u793A\u8BED\u8A00\u9009\u62E9\u5DF2\u88AB\u7EC4\u7B56\u7565\u9501\u5B9A\u3002",
    L"\u9009\u62E9\u663E\u793A\u8BED\u8A00(&C):",
    L"\u67D0\u4E9B\u6587\u672C\u5C1A\u672A\u672C\u5730\u5316\u4E3A\u6240\u9009\u8BED\u8A00\u3002\u9009\u62E9 Windows \u7528\u4E8E\u663E\u793A\u6B64\u6587\u672C\u7684\u53E6\u4E00\u79CD\u8BED\u8A00(&L):",
    L"\u6B64\u8BED\u8A00\u4EC5\u7ECF\u8FC7\u90E8\u5206\u672C\u5730\u5316\uFF0C\u67D0\u4E9B\u6587\u672C\u53EF\u80FD\u663E\u793A\u4E3A:",
    L"\u6B64\u8BED\u8A00\u4E5F\u4EC5\u7ECF\u8FC7\u90E8\u5206\u672C\u5730\u5316\u3002\u9009\u62E9 Windows \u7528\u4E8E\u663E\u793A\u5269\u4F59\u6587\u672C\u7684\u7B2C\u4E09\u79CD\u8BED\u8A00(&T):",
    L"\u6B64\u8BED\u8A00\u4E5F\u4EC5\u7ECF\u8FC7\u90E8\u5206\u672C\u5730\u5316\uFF0C\u67D0\u4E9B\u6587\u672C\u53EF\u80FD\u663E\u793A\u4E3A: ",
    L"\u6B22\u8FCE\u5C4F\u5E55\u548C\u65B0\u7684\u7528\u6237\u5E10\u6237",
    L"\u67E5\u770B\u533A\u57DF\u8BBE\u7F6E\u5E76\u5C06\u5176\u590D\u5236\u5230\u6B22\u8FCE\u5C4F\u5E55\u3001\u7CFB\u7EDF\u5E10\u6237\u548C\u65B0\u7684\u7528\u6237\u5E10\u6237\u3002",
    L"\u590D\u5236\u8BBE\u7F6E(&C)...",
    L"\u975E Unicode \u7A0B\u5E8F\u7684\u8BED\u8A00",
    L"\u6B64\u8BBE\u7F6E(\u7CFB\u7EDF\u533A\u57DF\u8BBE\u7F6E)\u63A7\u5236\u5728\u4E0D\u652F\u6301 Unicode \u7684\u7A0B\u5E8F\u4E2D\u663E\u793A\u6587\u672C\u65F6\u6240\u7528\u7684\u8BED\u8A00\u3002",
    L"\u975E Unicode \u7A0B\u5E8F\u7684\u5F53\u524D\u8BED\u8A00:",
    nullptr,
    L"\u66F4\u6539\u7CFB\u7EDF\u533A\u57DF\u8BBE\u7F6E(&C)...",
    L"\u793A\u4F8B",
    L"\u6B63\u6570:",
    L"\u8D1F\u6570:",
    L"\u5C0F\u6570\u70B9\u7B26\u53F7(&D):",
    L"\u5C0F\u6570\u4F4D\u6570(&N):",
    L"\u6570\u5B57\u5206\u7EC4\u7B26\u53F7(&G):",
    L"\u6570\u5B57\u5206\u7EC4(&G):",
    L"\u51CF\u53F7(&M):",
    L"\u8D1F\u6570\u683C\u5F0F(&N):",
    L"\u663E\u793A\u524D\u5BFC\u96F6(&Z):",
    L"\u5217\u8868\u5206\u9694\u7B26(&L):",
    L"\u5EA6\u91CF\u7CFB\u7EDF(&M):",
    L"\u6807\u51C6\u6570\u5B57(&S):",
    L"\u4F7F\u7528\u672C\u673A\u6570\u5B57(&U):",
    L"\u5355\u51FB\u201C\u91CD\u7F6E\u201D\u53EF\u8FD8\u539F\u6570\u5B57\u3001\u8D27\u5E01\u3001\u65F6\u95F4\u548C\u65E5\u671F\u7684\u7CFB\u7EDF\u9ED8\u8BA4\u8BBE\u7F6E\u3002",
    L"\u91CD\u7F6E(&R)",
    L"\u8D27\u5E01\u7B26\u53F7(&C):",
    L"\u6B63\u8D27\u5E01\u683C\u5F0F(&P):",
    L"\u8D1F\u8D27\u5E01\u683C\u5F0F(&N):",
    L"\u5C0F\u6570\u4F4D\u6570(&N):",
    L"\u6570\u5B57\u5206\u7EC4\u7B26\u53F7(&G):",
    L"\u6570\u5B57\u5206\u7EC4(&G):",
    L"\u65F6\u95F4\u683C\u5F0F",
    L"\u77ED\u65F6\u95F4(&S):",
    L"\u957F\u65F6\u95F4(&L):",
    L"\u4E0A\u5348\u7B26\u53F7(&A):",
    L"\u4E0B\u5348\u7B26\u53F7(&P):",
    L"\u8868\u793A\u6CD5\u542B\u4E49:\n\nh = \u5C0F\u65F6   m = \u5206\u949F\ns = \u79D2(\u4EC5\u957F\u65F6\u95F4)\ntt = \u4E0A\u5348\u6216\u4E0B\u5348\n\nh/H = 12/24 \u5C0F\u65F6\n\nhh\u3001mm\u3001ss = \u663E\u793A\u524D\u5BFC\u96F6\nh\u3001m\u3001s = \u4E0D\u663E\u793A\u524D\u5BFC\u96F6",
    L"\u65E5\u671F\u683C\u5F0F",
    L"\u8868\u793A\u6CD5\u542B\u4E49:\nd\u3001dd = \u65E5\uFF1B  ddd\u3001dddd = \u661F\u671F\uFF1B  M = \u6708\uFF1B  y = \u5E74",
    L"\u65E5\u5386",
    L"\u5F53\u8F93\u5165\u4E24\u4F4D\u5E74\u4EFD\u65F6\uFF0C\u5C06\u5176\u89E3\u91CA\u4E3A\u4ECB\u4E8E(&B)\u4E4B\u95F4\u7684\u5E74\u4EFD:",
    L"\u548C",
    L"\u4E00\u5468\u7684\u7B2C\u4E00\u5929(&W):",
    L"\u65E5\u5386\u7C7B\u578B(&T):",
    L"\u5C06 Hijri \u65E5\u671F\u8C03\u6574\u4E3A(&H):",
    L"\u60A8\u53EF\u4EE5\u63A7\u5236\u67D0\u4E9B\u7A0B\u5E8F\u5BF9\u5B57\u7B26\u3001\u5355\u8BCD\u3001\u6587\u4EF6\u548C\u6587\u4EF6\u5939\u7684\u6392\u5E8F\u65B9\u5F0F\u3002",
    L"\u9009\u62E9\u6392\u5E8F\u65B9\u6CD5(&S):",
    L"\u67D0\u4E9B\u8F6F\u4EF6(\u5305\u62EC Windows)\u53EF\u80FD\u4F1A\u4E3A\u7279\u5B9A\u4F4D\u7F6E\u63D0\u4F9B\u989D\u5916\u5185\u5BB9\u3002\u67D0\u4E9B\u670D\u52A1\u63D0\u4F9B\u672C\u5730\u4FE1\u606F\uFF0C\u4F8B\u5982\u65B0\u95FB\u548C\u5929\u6C14\u3002",
    L"\u5F53\u524D\u4F4D\u7F6E(&L):",
    L"\u53E6\u8BF7\u53C2\u9605",
    L"\u5F53\u524D\u7528\u6237\u3001\u6B22\u8FCE\u5C4F\u5E55(\u7CFB\u7EDF\u5E10\u6237)\u548C\u65B0\u7684\u7528\u6237\u5E10\u6237\u7684\u8BBE\u7F6E\u663E\u793A\u5982\u4E0B\u3002",
    L"* \u81EA\u5B9A\u4E49\u533A\u57DF\u8BBE\u7F6E",
    L"\u5C06\u5F53\u524D\u8BBE\u7F6E\u590D\u5236\u5230:",
    L"\u6B22\u8FCE\u5C4F\u5E55\u548C\u7CFB\u7EDF\u5E10\u6237(&W)",
    L"\u65B0\u7684\u7528\u6237\u5E10\u6237(&N)",
    L"\u65B0\u7684\u7528\u6237\u5E10\u6237\u7684\u663E\u793A\u8BED\u8A00\u5F53\u524D\u7EE7\u627F\u81EA\u6B22\u8FCE\u5C4F\u5E55\u7684\u663E\u793A\u8BED\u8A00\u3002",
    nullptr,
    L"\u53D6\u6D88",
    L"\u9009\u62E9\u5728\u4E0D\u652F\u6301 Unicode \u7684\u7A0B\u5E8F\u4E2D\u663E\u793A\u6587\u672C\u65F6\u8981\u4F7F\u7528\u7684\u8BED\u8A00(\u7CFB\u7EDF\u533A\u57DF\u8BBE\u7F6E)\u3002\u6B64\u8BBE\u7F6E\u4F1A\u5F71\u54CD\u8BA1\u7B97\u673A\u4E0A\u7684\u6240\u6709\u7528\u6237\u5E10\u6237\u3002",
    L"\u5F53\u524D\u7CFB\u7EDF\u533A\u57DF\u8BBE\u7F6E(&C):",
    L"<A>\u66F4\u6539\u6392\u5E8F\u65B9\u6CD5</A>",
    L"<A>\u6B64\u8868\u793A\u6CD5\u662F\u4EC0\u4E48\u610F\u601D?</A>",
    L"<A>\u8054\u673A\u4E86\u89E3\u6709\u5173\u66F4\u6539\u533A\u57DF\u8BED\u8A00\u548C\u683C\u5F0F\u7684\u8BE6\u7EC6\u4FE1\u606F</A>",
    L"<A>\u5982\u4F55\u66F4\u6539\u6B22\u8FCE\u5C4F\u5E55\u7684\u952E\u76D8\u5E03\u5C40?</A>",
    L"<A>\u5982\u4F55\u5B89\u88C5\u5176\u4ED6\u8BED\u8A00?</A>",
    L"<A>\u544A\u8BC9\u6211\u6709\u5173\u8FD9\u4E9B\u5E10\u6237\u7684\u8BE6\u7EC6\u4FE1\u606F</A>",
    L"<A>\u4EC0\u4E48\u662F\u7CFB\u7EDF\u533A\u57DF\u8BBE\u7F6E?</A>",
    L"<A>\u9ED8\u8BA4\u4F4D\u7F6E</A>",
};
static const wchar_t* const kTitleTr_ZH[11] = {
    L"\u683C\u5F0F",
    L"\u952E\u76D8\u548C\u8BED\u8A00",
    L"\u7BA1\u7406",
    L"\u6570\u5B57",
    L"\u8D27\u5E01",
    L"\u65F6\u95F4",
    L"\u65E5\u671F",
    L"\u6392\u5E8F",
    L"\u4F4D\u7F6E",
    L"\u6B22\u8FCE\u5C4F\u5E55\u548C\u65B0\u7684\u7528\u6237\u5E10\u6237\u8BBE\u7F6E",
    L"\u533A\u57DF\u548C\u8BED\u8A00",
};
// ================= \u65E5\u672C\u8A9E (ja-JP) =================
static const wchar_t* const kStrTr_JA[66] = {
    L"\u5730\u57DF\u3068\u8A00\u8A9E",
    L"\u8A00\u8A9E\u3001\u6570\u5024\u3001\u6642\u523B\u3001\u65E5\u4ED8\u306E\u8868\u793A\u8A2D\u5B9A\u3092\u69CB\u6210\u3057\u307E\u3059\u3002",
    L"\u5F62\u5F0F\u306E\u5909\u66F4",
    L"1 \u3064\u4EE5\u4E0A\u306E\u5730\u57DF\u8A2D\u5B9A\u304C\u7121\u52B9\u3067\u3059\u3002\u554F\u984C\u3092\u89E3\u6C7A\u3059\u308B\u306B\u306F\u3001\u30AB\u30B9\u30BF\u30E0\u8A2D\u5B9A\u3092\u78BA\u8A8D\u3057\u3066\u4FEE\u6B63\u3057\u3066\u304F\u3060\u3055\u3044\u3002",
    nullptr,
    nullptr,
    L"\u30E1\u30FC\u30C8\u30EB\u6CD5",
    nullptr,
    L"\u3053\u306E\u30D5\u30A3\u30FC\u30EB\u30C9\u306B\u5165\u529B\u3055\u308C\u305F 1 \u3064\u4EE5\u4E0A\u306E\u6587\u5B57\u304C\u7121\u52B9\u3067\u3059\u3002\u4ED6\u306E\u6587\u5B57\u3092\u4F7F\u7528\u3057\u3066\u307F\u3066\u304F\u3060\u3055\u3044\u3002",
    L"%s \u306B\u5165\u529B\u3055\u308C\u305F 1 \u3064\u4EE5\u4E0A\u306E\u6587\u5B57\u304C\u7121\u52B9\u3067\u3059\u3002  \u4ED6\u306E\u6587\u5B57\u3092\u4F7F\u7528\u3059\u308B\u304B\u3001\u30B9\u30DA\u30FC\u30B9\u3092\u5165\u529B\u3057\u3066\u307F\u3066\u304F\u3060\u3055\u3044\u3002",
    L"\u5C0F\u6570\u70B9\u8A18\u53F7",
    L"\u30DE\u30A4\u30CA\u30B9\u8A18\u53F7",
    L"\u6841\u533A\u5207\u308A\u8A18\u53F7",
    L"\u5348\u524D\u8A18\u53F7",
    L"\u5348\u5F8C\u8A18\u53F7",
    L"\u901A\u8CA8\u8A18\u53F7",
    L"\u901A\u8CA8\u306E\u5C0F\u6570\u70B9\u8A18\u53F7",
    L"\u901A\u8CA8\u306E\u6841\u533A\u5207\u308A\u8A18\u53F7",
    L"\u5F62\u5F0F %s \u306B\u5165\u529B\u3055\u308C\u305F 1 \u3064\u4EE5\u4E0A\u306E\u6587\u5B57\u304C\u7121\u52B9\u3067\u3059\u3002\u4ED6\u306E\u6587\u5B57\u3092\u4F7F\u7528\u3057\u3066\u307F\u3066\u304F\u3060\u3055\u3044\u3002",
    L"\u6642\u523B (\u6B63\u5F0F)",
    L"\u65E5\u4ED8 (\u7565\u5F0F)",
    L"\u65E5\u4ED8 (\u6B63\u5F0F)",
    L"\u3053\u306E\u30D5\u30A3\u30FC\u30EB\u30C9\u306E\u5024\u306F 99 \u304B\u3089 9999 \u307E\u3067\u306E\u6570\u5024\u3067\u3042\u308B\u5FC5\u8981\u304C\u3042\u308A\u307E\u3059\u3002\u4ED6\u306E\u6570\u5024\u3092\u4F7F\u7528\u3057\u3066\u307F\u3066\u304F\u3060\u3055\u3044\u3002",
    L"\u6642\u523B (\u7565\u5F0F)",
    L"\u5F62\u5F0F(&F):",
    L"\u5F62\u5F0F(&F): (* \u30AB\u30B9\u30BF\u30E0\u5730\u57DF\u8A2D\u5B9A)",
    nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
    L"\u30B7\u30B9\u30C6\u30E0 \u30ED\u30B1\u30FC\u30EB\u304C\u5909\u66F4\u3055\u308C\u307E\u3057\u305F\u3002\u5909\u66F4\u3092\u6709\u52B9\u306B\u3059\u308B\u306B\u306F\u3001Windows \u3092\u518D\u8D77\u52D5\u3059\u308B\u5FC5\u8981\u304C\u3042\u308A\u307E\u3059\u3002",
    L"\u5730\u57DF\u8A2D\u5B9A\u306E\u5909\u66F4",
    L"\u9078\u629E\u3055\u308C\u305F\u8A00\u8A9E\u3092\u8AAD\u307F\u8FBC\u3081\u307E\u305B\u3093\u3067\u3057\u305F\u3002\u30B7\u30B9\u30C6\u30E0\u7BA1\u7406\u8005\u306B\u554F\u3044\u5408\u308F\u305B\u3066\u304F\u3060\u3055\u3044\u3002",
    L"\u30B7\u30B9\u30C6\u30E0\u306E\u8868\u793A\u8A00\u8A9E\u304C\u5909\u66F4\u3055\u308C\u307E\u3057\u305F\u3002\u5909\u66F4\u3092\u6709\u52B9\u306B\u3059\u308B\u306B\u306F\u3001Windows \u3092\u518D\u8D77\u52D5\u3059\u308B\u5FC5\u8981\u304C\u3042\u308A\u307E\u3059\u3002",
    L"\u73FE\u5728\u306E\u5F62\u5F0F\u306E\u3059\u3079\u3066\u306E\u30AB\u30B9\u30BF\u30DE\u30A4\u30BA\u3092\u30AF\u30EA\u30A2\u3057\u307E\u3059\u304B?",
    L"\u8A00\u8A9E\u3068\u5730\u57DF\u306E\u5909\u66F4\u3092\u9069\u7528\u3057\u307E\u3059\u304B?",
    L"\u4ECA\u3059\u3050\u518D\u8D77\u52D5\u3059\u308B",
    L"\u30AD\u30E3\u30F3\u30BB\u30EB",
    L"\u518D\u8D77\u52D5\u3059\u308B\u524D\u306B\u4F5C\u696D\u5185\u5BB9\u3092\u4FDD\u5B58\u3057\u3001\u958B\u3044\u3066\u3044\u308B\u3059\u3079\u3066\u306E\u30D7\u30ED\u30B0\u30E9\u30E0\u3092\u9589\u3058\u3066\u304F\u3060\u3055\u3044\u3002",
    L"\u30B7\u30B9\u30C6\u30E0 \u30ED\u30B1\u30FC\u30EB\u306E\u5909\u66F4",
    L"\u30AD\u30FC\u30DC\u30FC\u30C9 \u30EC\u30A4\u30A2\u30A6\u30C8 %s \u3092\u6B63\u3057\u304F\u8AAD\u307F\u8FBC\u3081\u307E\u305B\u3093\u3067\u3057\u305F\u3002",
    L"\u30B9\u30DA\u30A4\u30F3\u8A9E (\u30B9\u30DA\u30A4\u30F3)",
    L"\u8868\u793A\u8A00\u8A9E\u306E\u5909\u66F4\u3092\u6709\u52B9\u306B\u3059\u308B\u306B\u306F\u3001\u30ED\u30B0\u30AA\u30D5\u3057\u3066\u304B\u3089\u30ED\u30B0\u30AA\u30F3\u3057\u76F4\u3059\u5FC5\u8981\u304C\u3042\u308A\u307E\u3059",
    L"\u30ED\u30B0\u30AA\u30D5\u3059\u308B\u524D\u306B\u4F5C\u696D\u5185\u5BB9\u3092\u4FDD\u5B58\u3057\u3001\u958B\u3044\u3066\u3044\u308B\u3059\u3079\u3066\u306E\u30D7\u30ED\u30B0\u30E9\u30E0\u3092\u9589\u3058\u3066\u304F\u3060\u3055\u3044\u3002",
    L"\u4ECA\u3059\u3050\u30ED\u30B0\u30AA\u30D5\u3059\u308B",
    L"\u30AD\u30E3\u30F3\u30BB\u30EB",
    L"\u8868\u793A\u8A00\u8A9E\u306E\u5909\u66F4",
    L"\u30B3\u30F3\u30D4\u30E5\u30FC\u30BF\u30FC\u306B\u53CD\u6620\u3055\u305B\u308B\u305F\u3081\u3001\u4ED6\u306E\u30B7\u30B9\u30C6\u30E0\u5909\u66F4\u3092\u884C\u3046\u524D\u306B\u5909\u66F4\u3092\u9069\u7528\u3059\u308B\u3053\u3068\u3092\u304A\u52E7\u3081\u3057\u307E\u3059\u3002",
    L"\u9069\u7528",
    L"\u30AD\u30E3\u30F3\u30BB\u30EB",
    L"\u64CD\u4F5C\u3092\u5B8C\u4E86\u3067\u304D\u307E\u305B\u3093\u3067\u3057\u305F",
    L"\u73FE\u5728\u306E\u30E6\u30FC\u30B6\u30FC",
    L"\u3088\u3046\u3053\u305D\u753B\u9762",
    L"\u65B0\u3057\u3044\u30A2\u30AB\u30A6\u30F3\u30C8",
    L"\u8868\u793A\u8A00\u8A9E:",
    L"\u5165\u529B\u8A00\u8A9E:",
    L"\u5F62\u5F0F:",
    L"\u5834\u6240:",
    L"\u30D1\u30E9\u30E1\u30FC\u30BF\u30FC\u3092\u8AAD\u307F\u53D6\u308C\u307E\u305B\u3093\u3067\u3057\u305F",
    L"\u30B3\u30F3\u30C6\u30AD\u30B9\u30C8",
    L"\u306A\u3057",
    L"\u30CD\u30A4\u30C6\u30A3\u30D6",
};
static const wchar_t* const kDlgTr_JA[92] = {
    L"\u65E5\u4ED8\u3068\u6642\u523B\u306E\u5F62\u5F0F",
    L"\u65E5\u4ED8 (\u7565\u5F0F)(&S):",
    L"\u65E5\u4ED8 (\u6B63\u5F0F)(&L):",
    L"\u6642\u523B (\u7565\u5F0F)(&S):",
    L"\u6642\u523B (\u6B63\u5F0F)(&L):",
    L"\u9031\u306E\u6700\u521D\u306E\u66DC\u65E5(&W):",
    L"\u4F8B",
    L"\u65E5\u4ED8 (\u7565\u5F0F):",
    L"\u65E5\u4ED8 (\u6B63\u5F0F):",
    L"\u6642\u523B (\u7565\u5F0F):",
    L"\u6642\u523B (\u6B63\u5F0F):",
    L"\u8FFD\u52A0\u306E\u8A2D\u5B9A(&A)...",
    L"\u30AD\u30FC\u30DC\u30FC\u30C9\u3068\u8A00\u8A9E",
    L"\u30AD\u30FC\u30DC\u30FC\u30C9\u307E\u305F\u306F\u5165\u529B\u8A00\u8A9E\u3092\u5909\u66F4\u3059\u308B\u306B\u306F\u3001[\u30AD\u30FC\u30DC\u30FC\u30C9\u306E\u5909\u66F4] \u3092\u30AF\u30EA\u30C3\u30AF\u3057\u307E\u3059\u3002",
    L"\u30AD\u30FC\u30DC\u30FC\u30C9\u306E\u5909\u66F4(&C)...",
    L"\u8868\u793A\u8A00\u8A9E",
    L"Windows \u3067\u30C6\u30AD\u30B9\u30C8\u306E\u8868\u793A\u3084\u3001\u30B5\u30DD\u30FC\u30C8\u3055\u308C\u3066\u3044\u308B\u5834\u5408\u306F\u97F3\u58F0\u8A8D\u8B58\u3068\u624B\u66F8\u304D\u8A8D\u8B58\u306B\u4F7F\u7528\u3067\u304D\u308B\u8A00\u8A9E\u3092\u30A4\u30F3\u30B9\u30C8\u30FC\u30EB\u307E\u305F\u306F\u30A2\u30F3\u30A4\u30F3\u30B9\u30C8\u30FC\u30EB\u3057\u307E\u3059\u3002",
    L"\u8A00\u8A9E\u306E\u30A4\u30F3\u30B9\u30C8\u30FC\u30EB/\u30A2\u30F3\u30A4\u30F3\u30B9\u30C8\u30FC\u30EB(&I)...",
    L"\u30B2\u30B9\u30C8\u3068\u3057\u3066\u8868\u793A\u8A00\u8A9E\u3092\u5909\u66F4\u3059\u308B\u3053\u3068\u306F\u3067\u304D\u307E\u305B\u3093:",
    L"\u8868\u793A\u8A00\u8A9E\u306E\u9078\u629E\u306F\u30B0\u30EB\u30FC\u30D7 \u30DD\u30EA\u30B7\u30FC\u306B\u3088\u3063\u3066\u30ED\u30C3\u30AF\u3055\u308C\u3066\u3044\u307E\u3059\u3002",
    L"\u8868\u793A\u8A00\u8A9E\u306E\u9078\u629E(&C):",
    L"\u4E00\u90E8\u306E\u30C6\u30AD\u30B9\u30C8\u306F\u9078\u629E\u3055\u308C\u305F\u8A00\u8A9E\u306B\u30ED\u30FC\u30AB\u30E9\u30A4\u30BA\u3055\u308C\u3066\u3044\u307E\u305B\u3093\u3002\u3053\u306E\u30C6\u30AD\u30B9\u30C8\u306E\u8868\u793A\u306B Windows \u304C\u4F7F\u7528\u3059\u308B\u5225\u306E\u8A00\u8A9E\u3092\u9078\u629E\u3057\u3066\u304F\u3060\u3055\u3044(&L):",
    L"\u3053\u306E\u8A00\u8A9E\u306F\u90E8\u5206\u7684\u306B\u306E\u307F\u30ED\u30FC\u30AB\u30E9\u30A4\u30BA\u3055\u308C\u3066\u304A\u308A\u3001\u4E00\u90E8\u306E\u30C6\u30AD\u30B9\u30C8\u306F\u6B21\u306E\u8A00\u8A9E\u3067\u8868\u793A\u3055\u308C\u308B\u5834\u5408\u304C\u3042\u308A\u307E\u3059:",
    L"\u3053\u306E\u8A00\u8A9E\u3082\u90E8\u5206\u7684\u306B\u306E\u307F\u30ED\u30FC\u30AB\u30E9\u30A4\u30BA\u3055\u308C\u3066\u3044\u307E\u3059\u3002\u6B8B\u308A\u306E\u30C6\u30AD\u30B9\u30C8\u306E\u8868\u793A\u306B Windows \u304C\u4F7F\u7528\u3059\u308B 3 \u756A\u76EE\u306E\u8A00\u8A9E\u3092\u9078\u629E\u3057\u3066\u304F\u3060\u3055\u3044(&T):",
    L"\u3053\u306E\u8A00\u8A9E\u3082\u90E8\u5206\u7684\u306B\u306E\u307F\u30ED\u30FC\u30AB\u30E9\u30A4\u30BA\u3055\u308C\u3066\u304A\u308A\u3001\u4E00\u90E8\u306E\u30C6\u30AD\u30B9\u30C8\u306F\u6B21\u306E\u8A00\u8A9E\u3067\u8868\u793A\u3055\u308C\u308B\u5834\u5408\u304C\u3042\u308A\u307E\u3059: ",
    L"\u3088\u3046\u3053\u305D\u753B\u9762\u3068\u65B0\u3057\u3044\u30E6\u30FC\u30B6\u30FC \u30A2\u30AB\u30A6\u30F3\u30C8",
    L"\u5730\u57DF\u8A2D\u5B9A\u3092\u8868\u793A\u3057\u3001\u3088\u3046\u3053\u305D\u753B\u9762\u3001\u30B7\u30B9\u30C6\u30E0 \u30A2\u30AB\u30A6\u30F3\u30C8\u3001\u65B0\u3057\u3044\u30E6\u30FC\u30B6\u30FC \u30A2\u30AB\u30A6\u30F3\u30C8\u306B\u30B3\u30D4\u30FC\u3057\u307E\u3059\u3002",
    L"\u8A2D\u5B9A\u306E\u30B3\u30D4\u30FC(&C)...",
    L"Unicode \u5BFE\u5FDC\u3067\u306A\u3044\u30D7\u30ED\u30B0\u30E9\u30E0\u306E\u8A00\u8A9E",
    L"\u3053\u306E\u8A2D\u5B9A (\u30B7\u30B9\u30C6\u30E0 \u30ED\u30B1\u30FC\u30EB) \u306F\u3001Unicode \u5BFE\u5FDC\u3067\u306A\u3044\u30D7\u30ED\u30B0\u30E9\u30E0\u3067\u30C6\u30AD\u30B9\u30C8\u3092\u8868\u793A\u3059\u308B\u3068\u304D\u306B\u4F7F\u7528\u3055\u308C\u308B\u8A00\u8A9E\u3092\u5236\u5FA1\u3057\u307E\u3059\u3002",
    L"Unicode \u5BFE\u5FDC\u3067\u306A\u3044\u30D7\u30ED\u30B0\u30E9\u30E0\u306E\u73FE\u5728\u306E\u8A00\u8A9E:",
    nullptr,
    L"\u30B7\u30B9\u30C6\u30E0 \u30ED\u30B1\u30FC\u30EB\u306E\u5909\u66F4(&C)...",
    L"\u4F8B",
    L"\u6B63:",
    L"\u8CA0:",
    L"\u5C0F\u6570\u70B9\u8A18\u53F7(&D):",
    L"\u5C0F\u6570\u70B9\u4EE5\u4E0B\u306E\u6841\u6570(&N):",
    L"\u6841\u533A\u5207\u308A\u8A18\u53F7(&G):",
    L"\u6841\u533A\u5207\u308A(&G):",
    L"\u30DE\u30A4\u30CA\u30B9\u8A18\u53F7(&M):",
    L"\u8CA0\u306E\u6570\u306E\u5F62\u5F0F(&N):",
    L"\u5148\u982D\u306E 0 \u3092\u8868\u793A\u3059\u308B(&Z):",
    L"\u4E00\u89A7\u306E\u533A\u5207\u308A\u8A18\u53F7(&L):",
    L"\u6E2C\u5B9A\u5358\u4F4D(&M):",
    L"\u6A19\u6E96\u306E\u6570\u5B57(&S):",
    L"\u6BCD\u56FD\u8A9E\u306E\u6570\u5B57\u3092\u4F7F\u3046(&U):",
    L"\u6570\u5024\u3001\u901A\u8CA8\u3001\u6642\u523B\u3001\u65E5\u4ED8\u306E\u30B7\u30B9\u30C6\u30E0\u306E\u65E2\u5B9A\u306E\u8A2D\u5B9A\u306B\u623B\u3059\u306B\u306F\u3001[\u30EA\u30BB\u30C3\u30C8] \u3092\u30AF\u30EA\u30C3\u30AF\u3057\u307E\u3059\u3002",
    L"\u30EA\u30BB\u30C3\u30C8(&R)",
    L"\u901A\u8CA8\u8A18\u53F7(&C):",
    L"\u6B63\u306E\u901A\u8CA8\u306E\u5F62\u5F0F(&P):",
    L"\u8CA0\u306E\u901A\u8CA8\u306E\u5F62\u5F0F(&N):",
    L"\u5C0F\u6570\u70B9\u4EE5\u4E0B\u306E\u6841\u6570(&N):",
    L"\u6841\u533A\u5207\u308A\u8A18\u53F7(&G):",
    L"\u6841\u533A\u5207\u308A(&G):",
    L"\u6642\u523B\u306E\u5F62\u5F0F",
    L"\u6642\u523B (\u7565\u5F0F)(&S):",
    L"\u6642\u523B (\u6B63\u5F0F)(&L):",
    L"\u5348\u524D\u8A18\u53F7(&A):",
    L"\u5348\u5F8C\u8A18\u53F7(&P):",
    L"\u8868\u8A18\u306E\u610F\u5473:\n\nh = \u6642   m = \u5206\ns = \u79D2 (\u6642\u523B (\u6B63\u5F0F) \u306E\u307F)\ntt = \u5348\u524D\u307E\u305F\u306F\u5348\u5F8C\n\nh/H = 12/24 \u6642\u9593\n\nhh\u3001mm\u3001ss = \u5148\u982D\u306E 0 \u3092\u8868\u793A\u3059\u308B\nh\u3001m\u3001s = \u5148\u982D\u306E 0 \u3092\u8868\u793A\u3057\u306A\u3044",
    L"\u65E5\u4ED8\u306E\u5F62\u5F0F",
    L"\u8868\u8A18\u306E\u610F\u5473:\nd\u3001dd = \u65E5;  ddd\u3001dddd = \u66DC\u65E5;  M = \u6708;  y = \u5E74",
    L"\u30AB\u30EC\u30F3\u30C0\u30FC",
    L"2 \u6841\u306E\u5E74\u304C\u5165\u529B\u3055\u308C\u305F\u5834\u5408\u3001\u6B21\u306E\u7BC4\u56F2\u306E\u5E74\u3068\u3057\u3066\u89E3\u91C8\u3059\u308B(&B):",
    L"\u304B\u3089",
    L"\u9031\u306E\u6700\u521D\u306E\u66DC\u65E5(&W):",
    L"\u30AB\u30EC\u30F3\u30C0\u30FC\u306E\u7A2E\u985E(&T):",
    L"\u30D2\u30B8\u30E5\u30E9\u66A6\u306E\u65E5\u4ED8\u3092\u6B21\u306B\u5408\u308F\u305B\u308B(&H):",
    L"\u4E00\u90E8\u306E\u30D7\u30ED\u30B0\u30E9\u30E0\u304C\u6587\u5B57\u3001\u5358\u8A9E\u3001\u30D5\u30A1\u30A4\u30EB\u3001\u30D5\u30A9\u30EB\u30C0\u30FC\u3092\u4E26\u3079\u66FF\u3048\u308B\u65B9\u6CD5\u3092\u5236\u5FA1\u3067\u304D\u307E\u3059\u3002",
    L"\u4E26\u3079\u66FF\u3048\u65B9\u6CD5\u306E\u9078\u629E(&S):",
    L"Windows \u3092\u542B\u3080\u4E00\u90E8\u306E\u30BD\u30D5\u30C8\u30A6\u30A7\u30A2\u306F\u3001\u7279\u5B9A\u306E\u5834\u6240\u5411\u3051\u306E\u8FFD\u52A0\u30B3\u30F3\u30C6\u30F3\u30C4\u3092\u63D0\u4F9B\u3067\u304D\u307E\u3059\u3002\u4E00\u90E8\u306E\u30B5\u30FC\u30D3\u30B9\u306F\u3001\u30CB\u30E5\u30FC\u30B9\u3084\u5929\u6C17\u306A\u3069\u306E\u5730\u57DF\u60C5\u5831\u3092\u63D0\u4F9B\u3057\u307E\u3059\u3002",
    L"\u73FE\u5728\u306E\u5834\u6240(&L):",
    L"\u95A2\u9023\u9805\u76EE",
    L"\u73FE\u5728\u306E\u30E6\u30FC\u30B6\u30FC\u3001\u3088\u3046\u3053\u305D\u753B\u9762 (\u30B7\u30B9\u30C6\u30E0 \u30A2\u30AB\u30A6\u30F3\u30C8)\u3001\u65B0\u3057\u3044\u30E6\u30FC\u30B6\u30FC \u30A2\u30AB\u30A6\u30F3\u30C8\u306E\u8A2D\u5B9A\u306F\u6B21\u306E\u3068\u304A\u308A\u3067\u3059\u3002",
    L"* \u30AB\u30B9\u30BF\u30E0\u5730\u57DF\u8A2D\u5B9A",
    L"\u73FE\u5728\u306E\u8A2D\u5B9A\u306E\u30B3\u30D4\u30FC\u5148:",
    L"\u3088\u3046\u3053\u305D\u753B\u9762\u3068\u30B7\u30B9\u30C6\u30E0 \u30A2\u30AB\u30A6\u30F3\u30C8(&W)",
    L"\u65B0\u3057\u3044\u30E6\u30FC\u30B6\u30FC \u30A2\u30AB\u30A6\u30F3\u30C8(&N)",
    L"\u65B0\u3057\u3044\u30E6\u30FC\u30B6\u30FC \u30A2\u30AB\u30A6\u30F3\u30C8\u306E\u8868\u793A\u8A00\u8A9E\u306F\u3001\u73FE\u5728\u3088\u3046\u3053\u305D\u753B\u9762\u306E\u8868\u793A\u8A00\u8A9E\u304B\u3089\u7D99\u627F\u3055\u308C\u307E\u3059\u3002",
    nullptr,
    L"\u30AD\u30E3\u30F3\u30BB\u30EB",
    L"Unicode \u5BFE\u5FDC\u3067\u306A\u3044\u30D7\u30ED\u30B0\u30E9\u30E0\u3067\u30C6\u30AD\u30B9\u30C8\u3092\u8868\u793A\u3059\u308B\u3068\u304D\u306B\u4F7F\u7528\u3059\u308B\u8A00\u8A9E (\u30B7\u30B9\u30C6\u30E0 \u30ED\u30B1\u30FC\u30EB) \u3092\u9078\u629E\u3057\u307E\u3059\u3002\u3053\u306E\u8A2D\u5B9A\u306F\u3001\u30B3\u30F3\u30D4\u30E5\u30FC\u30BF\u30FC\u4E0A\u306E\u3059\u3079\u3066\u306E\u30E6\u30FC\u30B6\u30FC \u30A2\u30AB\u30A6\u30F3\u30C8\u306B\u5F71\u97FF\u3057\u307E\u3059\u3002",
    L"\u73FE\u5728\u306E\u30B7\u30B9\u30C6\u30E0 \u30ED\u30B1\u30FC\u30EB(&C):",
    L"<A>\u4E26\u3079\u66FF\u3048\u65B9\u6CD5\u306E\u5909\u66F4</A>",
    L"<A>\u3053\u306E\u8868\u8A18\u306E\u610F\u5473\u306F?</A>",
    L"<A>\u5730\u57DF\u306E\u8A00\u8A9E\u3068\u5F62\u5F0F\u306E\u5909\u66F4\u306E\u8A73\u7D30\u3092\u30AA\u30F3\u30E9\u30A4\u30F3\u3067\u53C2\u7167\u3059\u308B</A>",
    L"<A>\u3088\u3046\u3053\u305D\u753B\u9762\u306E\u30AD\u30FC\u30DC\u30FC\u30C9 \u30EC\u30A4\u30A2\u30A6\u30C8\u3092\u5909\u66F4\u3059\u308B\u65B9\u6CD5\u306F?</A>",
    L"<A>\u8FFD\u52A0\u306E\u8A00\u8A9E\u3092\u30A4\u30F3\u30B9\u30C8\u30FC\u30EB\u3059\u308B\u65B9\u6CD5\u306F?</A>",
    L"<A>\u3053\u308C\u3089\u306E\u30A2\u30AB\u30A6\u30F3\u30C8\u306E\u8A73\u7D30\u3092\u8868\u793A\u3059\u308B</A>",
    L"<A>\u30B7\u30B9\u30C6\u30E0 \u30ED\u30B1\u30FC\u30EB\u3068\u306F?</A>",
    L"<A>\u65E2\u5B9A\u306E\u5834\u6240</A>",
};
static const wchar_t* const kTitleTr_JA[11] = {
    L"\u5F62\u5F0F",
    L"\u30AD\u30FC\u30DC\u30FC\u30C9\u3068\u8A00\u8A9E",
    L"\u7BA1\u7406",
    L"\u6570\u5024",
    L"\u901A\u8CA8",
    L"\u6642\u523B",
    L"\u65E5\u4ED8",
    L"\u4E26\u3079\u66FF\u3048",
    L"\u5834\u6240",
    L"\u3088\u3046\u3053\u305D\u753B\u9762\u3068\u65B0\u3057\u3044\u30E6\u30FC\u30B6\u30FC \u30A2\u30AB\u30A6\u30F3\u30C8\u306E\u8A2D\u5B9A",
    L"\u5730\u57DF\u3068\u8A00\u8A9E",
};
// ================= \uD55C\uAD6D\uC5B4 (ko-KR) =================
static const wchar_t* const kStrTr_KO[66] = {
    L"\uAD6D\uAC00 \uBC0F \uC5B8\uC5B4",
    L"\uC5B8\uC5B4, \uC22B\uC790, \uC2DC\uAC04 \uBC0F \uB0A0\uC9DC \uD45C\uC2DC \uC124\uC815\uC744 \uAD6C\uC131\uD569\uB2C8\uB2E4.",
    L"\uD615\uC2DD \uBCC0\uACBD",
    L"\uD558\uB098 \uC774\uC0C1\uC758 \uAD6D\uAC00\uBCC4 \uC124\uC815\uC774 \uC720\uD6A8\uD558\uC9C0 \uC54A\uC2B5\uB2C8\uB2E4. \uBB38\uC81C\uB97C \uD574\uACB0\uD558\uB824\uBA74 \uC0AC\uC6A9\uC790 \uC9C0\uC815 \uC124\uC815\uC744 \uAC80\uD1A0\uD558\uACE0 \uC218\uC815\uD558\uC2ED\uC2DC\uC624.",
    nullptr,
    nullptr,
    L"\uBBF8\uD130\uBC95",
    nullptr,
    L"\uC774 \uD544\uB4DC\uC5D0 \uC785\uB825\uD55C \uBB38\uC790 \uC911 \uD558\uB098 \uC774\uC0C1\uC774 \uC720\uD6A8\uD558\uC9C0 \uC54A\uC2B5\uB2C8\uB2E4. \uB2E4\uB978 \uBB38\uC790\uB97C \uC0AC\uC6A9\uD574 \uBCF4\uC2ED\uC2DC\uC624.",
    L"%s\uC5D0 \uB300\uD574 \uC785\uB825\uD55C \uBB38\uC790 \uC911 \uD558\uB098 \uC774\uC0C1\uC774 \uC720\uD6A8\uD558\uC9C0 \uC54A\uC2B5\uB2C8\uB2E4.  \uB2E4\uB978 \uBB38\uC790\uB97C \uC0AC\uC6A9\uD558\uAC70\uB098 \uACF5\uBC31\uC744 \uC785\uB825\uD574 \uBCF4\uC2ED\uC2DC\uC624.",
    L"\uC18C\uC218\uC810 \uAE30\uD638",
    L"\uBE7C\uAE30 \uAE30\uD638",
    L"\uC790\uB9BF\uC218 \uADF8\uB8F9 \uAE30\uD638",
    L"\uC624\uC804 \uAE30\uD638",
    L"\uC624\uD6C4 \uAE30\uD638",
    L"\uD1B5\uD654 \uAE30\uD638",
    L"\uD1B5\uD654 \uC18C\uC218\uC810 \uAE30\uD638",
    L"\uD1B5\uD654 \uC790\uB9BF\uC218 \uADF8\uB8F9 \uAE30\uD638",
    L"%s \uD615\uC2DD\uC5D0 \uB300\uD574 \uC785\uB825\uD55C \uBB38\uC790 \uC911 \uD558\uB098 \uC774\uC0C1\uC774 \uC720\uD6A8\uD558\uC9C0 \uC54A\uC2B5\uB2C8\uB2E4. \uB2E4\uB978 \uBB38\uC790\uB97C \uC0AC\uC6A9\uD574 \uBCF4\uC2ED\uC2DC\uC624.",
    L"\uAE34 \uC2DC\uAC04",
    L"\uC9E7\uC740 \uB0A0\uC9DC",
    L"\uAE34 \uB0A0\uC9DC",
    L"\uC774 \uD544\uB4DC\uC758 \uAC12\uC740 99\uC5D0\uC11C 9999 \uC0AC\uC774\uC758 \uC22B\uC790\uC5EC\uC57C \uD569\uB2C8\uB2E4. \uB2E4\uB978 \uC22B\uC790\uB97C \uC0AC\uC6A9\uD574 \uBCF4\uC2ED\uC2DC\uC624.",
    L"\uC9E7\uC740 \uC2DC\uAC04",
    L"\uD615\uC2DD(&F):",
    L"\uD615\uC2DD(&F): (* \uC0AC\uC6A9\uC790 \uC9C0\uC815 \uAD6D\uAC00\uBCC4 \uC124\uC815)",
    nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
    L"\uC2DC\uC2A4\uD15C \uAD6D\uAC00\uBCC4 \uC124\uC815\uC774 \uBCC0\uACBD\uB418\uC5C8\uC2B5\uB2C8\uB2E4. \uBCC0\uACBD \uB0B4\uC6A9\uC744 \uC801\uC6A9\uD558\uB824\uBA74 Windows\uB97C \uB2E4\uC2DC \uC2DC\uC791\uD574\uC57C \uD569\uB2C8\uB2E4.",
    L"\uAD6D\uAC00\uBCC4 \uC124\uC815 \uBCC0\uACBD",
    L"\uC120\uD0DD\uD55C \uC5B8\uC5B4\uB97C \uB85C\uB4DC\uD560 \uC218 \uC5C6\uC2B5\uB2C8\uB2E4. \uC2DC\uC2A4\uD15C \uAD00\uB9AC\uC790\uC5D0\uAC8C \uBB38\uC758\uD558\uC2ED\uC2DC\uC624.",
    L"\uC2DC\uC2A4\uD15C \uD45C\uC2DC \uC5B8\uC5B4\uAC00 \uBCC0\uACBD\uB418\uC5C8\uC2B5\uB2C8\uB2E4. \uBCC0\uACBD \uB0B4\uC6A9\uC744 \uC801\uC6A9\uD558\uB824\uBA74 Windows\uB97C \uB2E4\uC2DC \uC2DC\uC791\uD574\uC57C \uD569\uB2C8\uB2E4.",
    L"\uD604\uC7AC \uD615\uC2DD\uC758 \uBAA8\uB4E0 \uC0AC\uC6A9\uC790 \uC9C0\uC815\uC744 \uC9C0\uC6B0\uC2DC\uACA0\uC2B5\uB2C8\uAE4C?",
    L"\uC5B8\uC5B4 \uBC0F \uAD6D\uAC00 \uBCC0\uACBD \uB0B4\uC6A9\uC744 \uC801\uC6A9\uD558\uC2DC\uACA0\uC2B5\uB2C8\uAE4C?",
    L"\uC9C0\uAE08 \uB2E4\uC2DC \uC2DC\uC791",
    L"\uCDE8\uC18C",
    L"\uB2E4\uC2DC \uC2DC\uC791\uD558\uAE30 \uC804\uC5D0 \uC791\uC5C5\uC744 \uC800\uC7A5\uD558\uACE0 \uC5F4\uB824 \uC788\uB294 \uBAA8\uB4E0 \uD504\uB85C\uADF8\uB7A8\uC744 \uB2EB\uC73C\uC2ED\uC2DC\uC624.",
    L"\uC2DC\uC2A4\uD15C \uAD6D\uAC00\uBCC4 \uC124\uC815 \uBCC0\uACBD",
    L"\uD0A4\uBCF4\uB4DC \uB808\uC774\uC544\uC6C3 %s\uC744(\uB97C) \uC81C\uB300\uB85C \uB85C\uB4DC\uD560 \uC218 \uC5C6\uC2B5\uB2C8\uB2E4.",
    L"\uC2A4\uD398\uC778\uC5B4(\uC2A4\uD398\uC778)",
    L"\uD45C\uC2DC \uC5B8\uC5B4 \uBCC0\uACBD \uB0B4\uC6A9\uC744 \uC801\uC6A9\uD558\uB824\uBA74 \uB85C\uADF8\uC624\uD504\uD55C \uD6C4 \uB2E4\uC2DC \uB85C\uADF8\uC628\uD574\uC57C \uD569\uB2C8\uB2E4",
    L"\uB85C\uADF8\uC624\uD504\uD558\uAE30 \uC804\uC5D0 \uC791\uC5C5\uC744 \uC800\uC7A5\uD558\uACE0 \uC5F4\uB824 \uC788\uB294 \uBAA8\uB4E0 \uD504\uB85C\uADF8\uB7A8\uC744 \uB2EB\uC73C\uC2ED\uC2DC\uC624.",
    L"\uC9C0\uAE08 \uB85C\uADF8\uC624\uD504",
    L"\uCDE8\uC18C",
    L"\uD45C\uC2DC \uC5B8\uC5B4 \uBCC0\uACBD",
    L"\uCEF4\uD4E8\uD130\uC5D0 \uBC18\uC601\uB418\uB3C4\uB85D \uB2E4\uB978 \uC2DC\uC2A4\uD15C\uC744 \uBCC0\uACBD\uD558\uAE30 \uC804\uC5D0 \uBCC0\uACBD \uB0B4\uC6A9\uC744 \uC801\uC6A9\uD558\uB294 \uAC83\uC774 \uC88B\uC2B5\uB2C8\uB2E4.",
    L"\uC801\uC6A9",
    L"\uCDE8\uC18C",
    L"\uC791\uC5C5\uC744 \uC644\uB8CC\uD560 \uC218 \uC5C6\uC2B5\uB2C8\uB2E4",
    L"\uD604\uC7AC \uC0AC\uC6A9\uC790",
    L"\uC2DC\uC791 \uD654\uBA74",
    L"\uC0C8 \uACC4\uC815",
    L"\uD45C\uC2DC \uC5B8\uC5B4:",
    L"\uC785\uB825 \uC5B8\uC5B4:",
    L"\uD615\uC2DD:",
    L"\uC704\uCE58:",
    L"\uB9E4\uAC1C \uBCC0\uC218\uB97C \uC77D\uC744 \uC218 \uC5C6\uC2B5\uB2C8\uB2E4",
    L"\uCEE8\uD14D\uC2A4\uD2B8",
    L"\uC548 \uD568",
    L"\uB124\uC774\uD2F0\uBE0C",
};
static const wchar_t* const kDlgTr_KO[92] = {
    L"\uB0A0\uC9DC \uBC0F \uC2DC\uAC04 \uD615\uC2DD",
    L"\uC9E7\uC740 \uB0A0\uC9DC(&S):",
    L"\uAE34 \uB0A0\uC9DC(&L):",
    L"\uC9E7\uC740 \uC2DC\uAC04(&S):",
    L"\uAE34 \uC2DC\uAC04(&L):",
    L"\uD55C \uC8FC\uC758 \uCCAB\uC9F8 \uB0A0(&W):",
    L"\uC608\uC81C",
    L"\uC9E7\uC740 \uB0A0\uC9DC:",
    L"\uAE34 \uB0A0\uC9DC:",
    L"\uC9E7\uC740 \uC2DC\uAC04:",
    L"\uAE34 \uC2DC\uAC04:",
    L"\uCD94\uAC00 \uC124\uC815(&A)...",
    L"\uD0A4\uBCF4\uB4DC \uBC0F \uAE30\uD0C0 \uC785\uB825 \uC5B8\uC5B4",
    L"\uD0A4\uBCF4\uB4DC\uB098 \uC785\uB825 \uC5B8\uC5B4\uB97C \uBCC0\uACBD\uD558\uB824\uBA74 \uD0A4\uBCF4\uB4DC \uBCC0\uACBD\uC744 \uD074\uB9AD\uD558\uC2ED\uC2DC\uC624.",
    L"\uD0A4\uBCF4\uB4DC \uBCC0\uACBD(&C)...",
    L"\uD45C\uC2DC \uC5B8\uC5B4",
    L"Windows\uC5D0\uC11C \uD14D\uC2A4\uD2B8 \uD45C\uC2DC\uC640 \uC9C0\uC6D0\uB418\uB294 \uACBD\uC6B0 \uC74C\uC131 \uBC0F \uD544\uAE30 \uC778\uC2DD\uC5D0 \uC0AC\uC6A9\uD560 \uC218 \uC788\uB294 \uC5B8\uC5B4\uB97C \uC124\uCE58\uD558\uAC70\uB098 \uC81C\uAC70\uD569\uB2C8\uB2E4.",
    L"\uC5B8\uC5B4 \uC124\uCE58/\uC81C\uAC70(&I)...",
    L"\uAC8C\uC2A4\uD2B8\uB294 \uD45C\uC2DC \uC5B8\uC5B4\uB97C \uBCC0\uACBD\uD560 \uC218 \uC5C6\uC2B5\uB2C8\uB2E4:",
    L"\uD45C\uC2DC \uC5B8\uC5B4 \uC120\uD0DD\uC740 \uADF8\uB8F9 \uC815\uCC45\uC5D0 \uC758\uD574 \uC7A0\uACA8 \uC788\uC2B5\uB2C8\uB2E4.",
    L"\uD45C\uC2DC \uC5B8\uC5B4 \uC120\uD0DD(&C):",
    L"\uC77C\uBD80 \uD14D\uC2A4\uD2B8\uAC00 \uC120\uD0DD\uD55C \uC5B8\uC5B4\uB85C \uC9C0\uC5ED\uD654\uB418\uC9C0 \uC54A\uC558\uC2B5\uB2C8\uB2E4. \uC774 \uD14D\uC2A4\uD2B8\uB97C \uD45C\uC2DC\uD558\uB294 \uB370 Windows\uC5D0\uC11C \uC0AC\uC6A9\uD560 \uB2E4\uB978 \uC5B8\uC5B4\uB97C \uC120\uD0DD\uD558\uC2ED\uC2DC\uC624(&L):",
    L"\uC774 \uC5B8\uC5B4\uB294 \uBD80\uBD84\uC801\uC73C\uB85C\uB9CC \uC9C0\uC5ED\uD654\uB418\uC5C8\uC73C\uBA70 \uC77C\uBD80 \uD14D\uC2A4\uD2B8\uB294 \uB2E4\uC74C \uC5B8\uC5B4\uB85C \uD45C\uC2DC\uB420 \uC218 \uC788\uC2B5\uB2C8\uB2E4:",
    L"\uC774 \uC5B8\uC5B4\uB3C4 \uBD80\uBD84\uC801\uC73C\uB85C\uB9CC \uC9C0\uC5ED\uD654\uB418\uC5C8\uC2B5\uB2C8\uB2E4. \uB098\uBA38\uC9C0 \uD14D\uC2A4\uD2B8\uB97C \uD45C\uC2DC\uD558\uB294 \uB370 Windows\uC5D0\uC11C \uC0AC\uC6A9\uD560 \uC138 \uBC88\uC9F8 \uC5B8\uC5B4\uB97C \uC120\uD0DD\uD558\uC2ED\uC2DC\uC624(&T):",
    L"\uC774 \uC5B8\uC5B4\uB3C4 \uBD80\uBD84\uC801\uC73C\uB85C\uB9CC \uC9C0\uC5ED\uD654\uB418\uC5C8\uC73C\uBA70 \uC77C\uBD80 \uD14D\uC2A4\uD2B8\uB294 \uB2E4\uC74C \uC5B8\uC5B4\uB85C \uD45C\uC2DC\uB420 \uC218 \uC788\uC2B5\uB2C8\uB2E4: ",
    L"\uC2DC\uC791 \uD654\uBA74 \uBC0F \uC0C8 \uC0AC\uC6A9\uC790 \uACC4\uC815",
    L"\uAD6D\uAC00\uBCC4 \uC124\uC815\uC744 \uBCF4\uACE0 \uC2DC\uC791 \uD654\uBA74, \uC2DC\uC2A4\uD15C \uACC4\uC815 \uBC0F \uC0C8 \uC0AC\uC6A9\uC790 \uACC4\uC815\uC5D0 \uBCF5\uC0AC\uD569\uB2C8\uB2E4.",
    L"\uC124\uC815 \uBCF5\uC0AC(&C)...",
    L"\uC720\uB2C8\uCF54\uB4DC\uB97C \uC9C0\uC6D0\uD558\uC9C0 \uC54A\uB294 \uD504\uB85C\uADF8\uB7A8\uC758 \uC5B8\uC5B4",
    L"\uC774 \uC124\uC815(\uC2DC\uC2A4\uD15C \uAD6D\uAC00\uBCC4 \uC124\uC815)\uC740 \uC720\uB2C8\uCF54\uB4DC\uB97C \uC9C0\uC6D0\uD558\uC9C0 \uC54A\uB294 \uD504\uB85C\uADF8\uB7A8\uC5D0\uC11C \uD14D\uC2A4\uD2B8\uB97C \uD45C\uC2DC\uD560 \uB54C \uC0AC\uC6A9\uB418\uB294 \uC5B8\uC5B4\uB97C \uC81C\uC5B4\uD569\uB2C8\uB2E4.",
    L"\uC720\uB2C8\uCF54\uB4DC\uB97C \uC9C0\uC6D0\uD558\uC9C0 \uC54A\uB294 \uD504\uB85C\uADF8\uB7A8\uC758 \uD604\uC7AC \uC5B8\uC5B4:",
    nullptr,
    L"\uC2DC\uC2A4\uD15C \uAD6D\uAC00\uBCC4 \uC124\uC815 \uBCC0\uACBD(&C)...",
    L"\uC608\uC81C",
    L"\uC591\uC218:",
    L"\uC74C\uC218:",
    L"\uC18C\uC218\uC810 \uAE30\uD638(&D):",
    L"\uC18C\uC218 \uC790\uB9BF\uC218(&N):",
    L"\uC790\uB9BF\uC218 \uADF8\uB8F9 \uAE30\uD638(&G):",
    L"\uC22B\uC790 \uADF8\uB8F9\uD654(&G):",
    L"\uBE7C\uAE30 \uAE30\uD638(&M):",
    L"\uC74C\uC218 \uD615\uC2DD(&N):",
    L"\uC55E\uC790\uB9AC 0 \uD45C\uC2DC(&Z):",
    L"\uBAA9\uB85D \uAD6C\uBD84 \uAE30\uD638(&L):",
    L"\uCE21\uC815 \uB2E8\uC704(&M):",
    L"\uD45C\uC900 \uC22B\uC790(&S):",
    L"\uB124\uC774\uD2F0\uBE0C \uC22B\uC790 \uC0AC\uC6A9(&U):",
    L"\uC22B\uC790, \uD1B5\uD654, \uC2DC\uAC04 \uBC0F \uB0A0\uC9DC\uC758 \uC2DC\uC2A4\uD15C \uAE30\uBCF8\uAC12\uC744 \uBCF5\uC6D0\uD558\uB824\uBA74 \uB2E4\uC2DC \uC124\uC815\uC744 \uD074\uB9AD\uD558\uC2ED\uC2DC\uC624.",
    L"\uB2E4\uC2DC \uC124\uC815(&R)",
    L"\uD1B5\uD654 \uAE30\uD638(&C):",
    L"\uC591\uC758 \uD1B5\uD654 \uD615\uC2DD(&P):",
    L"\uC74C\uC758 \uD1B5\uD654 \uD615\uC2DD(&N):",
    L"\uC18C\uC218 \uC790\uB9BF\uC218(&N):",
    L"\uC790\uB9BF\uC218 \uADF8\uB8F9 \uAE30\uD638(&G):",
    L"\uC22B\uC790 \uADF8\uB8F9\uD654(&G):",
    L"\uC2DC\uAC04 \uD615\uC2DD",
    L"\uC9E7\uC740 \uC2DC\uAC04(&S):",
    L"\uAE34 \uC2DC\uAC04(&L):",
    L"\uC624\uC804 \uAE30\uD638(&A):",
    L"\uC624\uD6C4 \uAE30\uD638(&P):",
    L"\uD45C\uAE30\uBC95 \uC758\uBBF8:\n\nh = \uC2DC\uAC04   m = \uBD84\ns = \uCD08(\uAE34 \uC2DC\uAC04\uB9CC)\ntt = \uC624\uC804 \uB610\uB294 \uC624\uD6C4\n\nh/H = 12/24\uC2DC\uAC04\n\nhh, mm, ss = \uC55E\uC790\uB9AC 0 \uD45C\uC2DC\nh, m, s = \uC55E\uC790\uB9AC 0 \uD45C\uC2DC \uC548 \uD568",
    L"\uB0A0\uC9DC \uD615\uC2DD",
    L"\uD45C\uAE30\uBC95 \uC758\uBBF8:\nd, dd = \uC77C;  ddd, dddd = \uC694\uC77C;  M = \uC6D4;  y = \uC5F0\uB3C4",
    L"\uB2EC\uB825",
    L"\uB450 \uC790\uB9AC \uC5F0\uB3C4\uB97C \uC785\uB825\uD558\uBA74 \uB2E4\uC74C \uC0AC\uC774\uC758 \uC5F0\uB3C4\uB85C \uD574\uC11D(&B):",
    L"\uBD80\uD130",
    L"\uD55C \uC8FC\uC758 \uCCAB\uC9F8 \uB0A0(&W):",
    L"\uB2EC\uB825 \uC885\uB958(&T):",
    L"Hijri \uB0A0\uC9DC \uC870\uC815(&H):",
    L"\uC77C\uBD80 \uD504\uB85C\uADF8\uB7A8\uC5D0\uC11C \uBB38\uC790, \uB2E8\uC5B4, \uD30C\uC77C \uBC0F \uD3F4\uB354\uB97C \uC815\uB82C\uD558\uB294 \uBC29\uBC95\uC744 \uC81C\uC5B4\uD560 \uC218 \uC788\uC2B5\uB2C8\uB2E4.",
    L"\uC815\uB82C \uBC29\uBC95 \uC120\uD0DD(&S):",
    L"Windows\uB97C \uD3EC\uD568\uD55C \uC77C\uBD80 \uC18C\uD504\uD2B8\uC6E8\uC5B4\uB294 \uD2B9\uC815 \uC704\uCE58\uC5D0 \uB300\uD55C \uCD94\uAC00 \uCF58\uD150\uCE20\uB97C \uC81C\uACF5\uD560 \uC218 \uC788\uC2B5\uB2C8\uB2E4. \uC77C\uBD80 \uC11C\uBE44\uC2A4\uB294 \uB274\uC2A4 \uBC0F \uB0A0\uC528\uC640 \uAC19\uC740 \uC9C0\uC5ED \uC815\uBCF4\uB97C \uC81C\uACF5\uD569\uB2C8\uB2E4.",
    L"\uD604\uC7AC \uC704\uCE58(&L):",
    L"\uCC38\uACE0 \uD56D\uBAA9",
    L"\uD604\uC7AC \uC0AC\uC6A9\uC790, \uC2DC\uC791 \uD654\uBA74(\uC2DC\uC2A4\uD15C \uACC4\uC815) \uBC0F \uC0C8 \uC0AC\uC6A9\uC790 \uACC4\uC815\uC758 \uC124\uC815\uC740 \uB2E4\uC74C\uACFC \uAC19\uC2B5\uB2C8\uB2E4.",
    L"* \uC0AC\uC6A9\uC790 \uC9C0\uC815 \uAD6D\uAC00\uBCC4 \uC124\uC815",
    L"\uD604\uC7AC \uC124\uC815 \uBCF5\uC0AC \uC704\uCE58:",
    L"\uC2DC\uC791 \uD654\uBA74 \uBC0F \uC2DC\uC2A4\uD15C \uACC4\uC815(&W)",
    L"\uC0C8 \uC0AC\uC6A9\uC790 \uACC4\uC815(&N)",
    L"\uC0C8 \uC0AC\uC6A9\uC790 \uACC4\uC815\uC758 \uD45C\uC2DC \uC5B8\uC5B4\uB294 \uD604\uC7AC \uC2DC\uC791 \uD654\uBA74\uC758 \uD45C\uC2DC \uC5B8\uC5B4\uC5D0\uC11C \uC0C1\uC18D\uB429\uB2C8\uB2E4.",
    nullptr,
    L"\uCDE8\uC18C",
    L"\uC720\uB2C8\uCF54\uB4DC\uB97C \uC9C0\uC6D0\uD558\uC9C0 \uC54A\uB294 \uD504\uB85C\uADF8\uB7A8\uC5D0\uC11C \uD14D\uC2A4\uD2B8\uB97C \uD45C\uC2DC\uD560 \uB54C \uC0AC\uC6A9\uD560 \uC5B8\uC5B4(\uC2DC\uC2A4\uD15C \uAD6D\uAC00\uBCC4 \uC124\uC815)\uB97C \uC120\uD0DD\uD569\uB2C8\uB2E4. \uC774 \uC124\uC815\uC740 \uCEF4\uD4E8\uD130\uC758 \uBAA8\uB4E0 \uC0AC\uC6A9\uC790 \uACC4\uC815\uC5D0 \uC601\uD5A5\uC744 \uC90D\uB2C8\uB2E4.",
    L"\uD604\uC7AC \uC2DC\uC2A4\uD15C \uAD6D\uAC00\uBCC4 \uC124\uC815(&C):",
    L"<A>\uC815\uB82C \uBC29\uBC95 \uBCC0\uACBD</A>",
    L"<A>\uC774 \uD45C\uAE30\uBC95\uC758 \uC758\uBBF8\uB294 \uBB34\uC5C7\uC785\uB2C8\uAE4C?</A>",
    L"<A>\uAD6D\uAC00 \uC5B8\uC5B4 \uBC0F \uD615\uC2DD \uBCC0\uACBD\uC5D0 \uB300\uD55C \uC790\uC138\uD55C \uB0B4\uC6A9\uC744 \uC628\uB77C\uC778\uC5D0\uC11C \uCC38\uC870</A>",
    L"<A>\uC2DC\uC791 \uD654\uBA74\uC758 \uD0A4\uBCF4\uB4DC \uB808\uC774\uC544\uC6C3\uC744 \uBCC0\uACBD\uD558\uB294 \uBC29\uBC95\uC740 \uBB34\uC5C7\uC785\uB2C8\uAE4C?</A>",
    L"<A>\uCD94\uAC00 \uC5B8\uC5B4\uB97C \uC124\uCE58\uD558\uB294 \uBC29\uBC95\uC740 \uBB34\uC5C7\uC785\uB2C8\uAE4C?</A>",
    L"<A>\uC774\uB7EC\uD55C \uACC4\uC815\uC5D0 \uB300\uD574 \uC790\uC138\uD788 \uC54C\uB824 \uC8FC\uC2ED\uC2DC\uC624</A>",
    L"<A>\uC2DC\uC2A4\uD15C \uAD6D\uAC00\uBCC4 \uC124\uC815\uC774\uB780 \uBB34\uC5C7\uC785\uB2C8\uAE4C?</A>",
    L"<A>\uAE30\uBCF8 \uC704\uCE58</A>",
};
static const wchar_t* const kTitleTr_KO[11] = {
    L"\uD615\uC2DD",
    L"\uD0A4\uBCF4\uB4DC \uBC0F \uC5B8\uC5B4",
    L"\uAD00\uB9AC",
    L"\uC22B\uC790",
    L"\uD1B5\uD654",
    L"\uC2DC\uAC04",
    L"\uB0A0\uC9DC",
    L"\uC815\uB82C",
    L"\uC704\uCE58",
    L"\uC2DC\uC791 \uD654\uBA74 \uBC0F \uC0C8 \uC0AC\uC6A9\uC790 \uACC4\uC815 \uC124\uC815",
    L"\uAD6D\uAC00 \uBC0F \uC5B8\uC5B4",
};
// ================= MASTER TABLES (index = LangIndex) =================
// EN (slot 0) intentionally nullptr: builders keep genuine tables when lang==English.
static const wchar_t* const* const kStrMasters[20] = {
    nullptr,
    kStrTr_IT, kStrTr_DE, kStrTr_FR, kStrTr_ES, kStrTr_PT,
    kStrTr_NL, kStrTr_PL, kStrTr_RU, kStrTr_ZH, kStrTr_JA,
    kStrTr_KO, kStrTr_TR, kStrTr_CS, kStrTr_HU, kStrTr_RO,
    kStrTr_SV, kStrTr_UK, kStrTr_EL, kStrTr_AR,
};
static const wchar_t* const* const kDlgMasters[20] = {
    nullptr,
    kDlgTr_IT, kDlgTr_DE, kDlgTr_FR, kDlgTr_ES, kDlgTr_PT,
    kDlgTr_NL, kDlgTr_PL, kDlgTr_RU, kDlgTr_ZH, kDlgTr_JA,
    kDlgTr_KO, kDlgTr_TR, kDlgTr_CS, kDlgTr_HU, kDlgTr_RO,
    kDlgTr_SV, kDlgTr_UK, kDlgTr_EL, kDlgTr_AR,
};
static const wchar_t* const* const kTitleMasters[20] = {
    nullptr,
    kTitleTr_IT, kTitleTr_DE, kTitleTr_FR, kTitleTr_ES, kTitleTr_PT,
    kTitleTr_NL, kTitleTr_PL, kTitleTr_RU, kTitleTr_ZH, kTitleTr_JA,
    kTitleTr_KO, kTitleTr_TR, kTitleTr_CS, kTitleTr_HU, kTitleTr_RO,
    kTitleTr_SV, kTitleTr_UK, kTitleTr_EL, kTitleTr_AR,
};
// Selected UI language (LangIndex). Resolved in Prepare() and switchable at
// runtime by Wh_ModSettingsChanged, which no longer reloads the mod - hence
// atomic: the dialog/template builders read it from the legacy call threads.
std::atomic<int> g_lang{LangEN};
// Language queued by Wh_ModSettingsChanged, or -1. Applied by
// ApplyPendingLanguageChange() (defined next to the resource builders) at the
// next quiet point; see the comment there for why it cannot be applied inline.
SRWLOCK g_langLock = SRWLOCK_INIT;
std::atomic<int> g_pendingLang{-1};

// ================= INPUT.DLL LANGUAGE PACKS (v1.1.0) =================
// en-US (LangEN) keeps the genuine Microsoft text of the pinned input.dll.
// The packs below are mod-provided translations of the Text Services and
// Input Languages UI. kInputText maps runtime LoadStringW ids (verified
// against the binary's string table and its disassembled call sites);
// kInpDlgTr_* holds the dialog-template phrases, indexed through
// kInpPhrMaps/kInpTitlePhrase in kInpDialogs order.

struct InputTextPack { UINT id; const wchar_t* text[LangCount]; };
// String ids verified against the pinned input.dll: LoadStringW call
// sites (static ids 2002-2043) plus the dynamically table-driven ids
// (categories, key names, defaults). Ids that are language-neutral
// (letters, digits, F-keys, key-cap pseudo-names) are not listed and
// keep the genuine English original.
static const InputTextPack kInputText[] = {
    {1,{L"Text Services and Input Languages",L"Tastiere e lingue",L"Textdienste und Eingabesprachen",L"Services de texte et langues d\u2019entr\u00E9e",L"Servicios de texto e idiomas de entrada",L"Servi\u00E7os de texto e idiomas de entrada",L"Tekstservices en invoertalen",L"Us\u0142ugi tekstowe i j\u0119zyki wprowadzania",L"\u0422\u0435\u043A\u0441\u0442\u043E\u0432\u044B\u0435 \u0441\u043B\u0443\u0436\u0431\u044B \u0438 \u044F\u0437\u044B\u043A\u0438 \u0432\u0432\u043E\u0434\u0430",L"\u6587\u672C\u670D\u52A1\u548C\u8F93\u5165\u8BED\u8A00",L"\u30C6\u30AD\u30B9\u30C8 \u30B5\u30FC\u30D3\u30B9\u3068\u5165\u529B\u8A00\u8A9E",L"\uD14D\uC2A4\uD2B8 \uC11C\uBE44\uC2A4 \uBC0F \uC785\uB825 \uC5B8\uC5B4",L"Metin hizmetleri ve giri\u015F dilleri",L"Textov\u00E9 slu\u017Eby a vstupn\u00ED jazyky",L"Sz\u00F6veges szolg\u00E1ltat\u00E1sok \u00E9s beviteli nyelvek",L"Servicii de text \u0219i limbi de intrare",L"Texttj\u00E4nster och inmatningsspr\u00E5k",L"\u0422\u0435\u043A\u0441\u0442\u043E\u0432\u0456 \u0441\u043B\u0443\u0436\u0431\u0438 \u0442\u0430 \u043C\u043E\u0432\u0438 \u0432\u0432\u0435\u0434\u0435\u043D\u043D\u044F",L"\u03A5\u03C0\u03B7\u03C1\u03B5\u03C3\u03AF\u03B5\u03C2 \u03BA\u03B5\u03B9\u03BC\u03AD\u03BD\u03BF\u03C5 \u03BA\u03B1\u03B9 \u03B3\u03BB\u03CE\u03C3\u03C3\u03B5\u03C2 \u03B5\u03B9\u03C3\u03CC\u03B4\u03BF\u03C5",L"\u062E\u062F\u0645\u0627\u062A \u0627\u0644\u0646\u0635\u0648\u0635 \u0648\u0644\u063A\u0627\u062A \u0627\u0644\u0625\u062F\u062E\u0627\u0644"}},
    {2,{L"Customizes settings for text input of languages",L"Consente di personalizzare le impostazioni per l\u2019input di testo delle lingue",L"Passt die Einstellungen f\u00FCr die Texteingabe von Sprachen an",L"Personnalise les param\u00E8tres de saisie de texte des langues",L"Personaliza la configuraci\u00F3n de entrada de texto de los idiomas",L"Personaliza as defini\u00E7\u00F5es de entrada de texto dos idiomas",L"Past de instellingen voor tekstinvoer van talen aan",L"Dostosowuje ustawienia wprowadzania tekstu dla j\u0119zyk\u00F3w",L"\u041D\u0430\u0441\u0442\u0440\u043E\u0439\u043A\u0430 \u043F\u0430\u0440\u0430\u043C\u0435\u0442\u0440\u043E\u0432 \u0432\u0432\u043E\u0434\u0430 \u0442\u0435\u043A\u0441\u0442\u0430 \u0434\u043B\u044F \u044F\u0437\u044B\u043A\u043E\u0432",L"\u81EA\u5B9A\u4E49\u8BED\u8A00\u6587\u672C\u8F93\u5165\u8BBE\u7F6E",L"\u8A00\u8A9E\u306E\u30C6\u30AD\u30B9\u30C8\u5165\u529B\u8A2D\u5B9A\u3092\u30AB\u30B9\u30BF\u30DE\u30A4\u30BA\u3057\u307E\u3059",L"\uC5B8\uC5B4\uC758 \uD14D\uC2A4\uD2B8 \uC785\uB825 \uC124\uC815\uC744 \uC0AC\uC6A9\uC790 \uC9C0\uC815\uD569\uB2C8\uB2E4",L"Dillerin metin giri\u015Fi ayarlar\u0131n\u0131 \u00F6zelle\u015Ftirir",L"P\u0159izp\u016Fsob\u00ED nastaven\u00ED zad\u00E1v\u00E1n\u00ED textu pro jazyky",L"A nyelvek sz\u00F6vegbeviteli be\u00E1ll\u00EDt\u00E1sainak testreszab\u00E1sa",L"Particularizeaz\u0103 set\u0103rile de introducere a textului pentru limbi",L"Anpassar inst\u00E4llningar f\u00F6r textinmatning f\u00F6r spr\u00E5k",L"\u041D\u0430\u043B\u0430\u0448\u0442\u043E\u0432\u0443\u0454 \u043F\u0430\u0440\u0430\u043C\u0435\u0442\u0440\u0438 \u0432\u0432\u0435\u0434\u0435\u043D\u043D\u044F \u0442\u0435\u043A\u0441\u0442\u0443 \u0434\u043B\u044F \u043C\u043E\u0432",L"\u03A0\u03C1\u03BF\u03C3\u03B1\u03C1\u03BC\u03CC\u03B6\u03B5\u03B9 \u03C4\u03B9\u03C2 \u03C1\u03C5\u03B8\u03BC\u03AF\u03C3\u03B5\u03B9\u03C2 \u03B5\u03B9\u03C3\u03B1\u03B3\u03C9\u03B3\u03AE\u03C2 \u03BA\u03B5\u03B9\u03BC\u03AD\u03BD\u03BF\u03C5 \u03B3\u03B9\u03B1 \u03B3\u03BB\u03CE\u03C3\u03C3\u03B5\u03C2",L"\u062A\u062E\u0635\u064A\u0635 \u0625\u0639\u062F\u0627\u062F\u0627\u062A \u0625\u062F\u062E\u0627\u0644 \u0627\u0644\u0646\u0635 \u0644\u0644\u063A\u0627\u062A"}},
    {2001,{L"Between input languages",L"Tra le lingue di input",L"Zwischen Eingabesprachen",L"Entre les langues d'entr\u00E9e",L"Entre idiomas de entrada",L"Entre idiomas de entrada",L"Tussen invoertalen",L"Mi\u0119dzy j\u0119zykami wprowadzania",L"\u041C\u0435\u0436\u0434\u0443 \u044F\u0437\u044B\u043A\u0430\u043C\u0438 \u0432\u0432\u043E\u0434\u0430",L"\u8F93\u5165\u8BED\u8A00\u4E4B\u95F4",L"\u5165\u529B\u8A00\u8A9E\u306E\u5207\u308A\u66FF\u3048",L"\uC785\uB825 \uC5B8\uC5B4 \uAC04 \uC804\uD658",L"Giri\u015F dilleri aras\u0131nda",L"Mezi vstupn\u00EDmi jazyky",L"Beviteli nyelvek k\u00F6z\u00F6tt",L"\u00CEntre limbile de intrare",L"Mellan inmatningsspr\u00E5k",L"\u041C\u0456\u0436 \u043C\u043E\u0432\u0430\u043C\u0438 \u0432\u0432\u0435\u0434\u0435\u043D\u043D\u044F",L"\u039C\u03B5\u03C4\u03B1\u03BE\u03CD \u03B3\u03BB\u03C9\u03C3\u03C3\u03CE\u03BD \u03B5\u03B9\u03C3\u03CC\u03B4\u03BF\u03C5",L"\u0628\u064A\u0646 \u0644\u063A\u0627\u062A \u0627\u0644\u0625\u062F\u062E\u0627\u0644"}},
    {2002,{L"To %s - %s",L"A %s - %s",L"Auf %s - %s",L"Vers %s - %s",L"A %s - %s",L"Para %s - %s",L"Naar %s - %s",L"Na %s - %s",L"\u041D\u0430 %s - %s",L"\u8F6C\u5230 %s - %s",L"%s - %s \u3078",L"%s - %s(\uC73C)\uB85C",L"%s - %s olarak",L"Na %s - %s",L"%s - %s nyelvre",L"C\u0103tre %s - %s",L"Till %s - %s",L"\u041D\u0430 %s - %s",L"\u03A3\u03B5 %s - %s",L"\u0625\u0644\u0649 %s - %s"}},
    {2003,{L"Ctrl+",L"Ctrl+",L"Strg+",L"Ctrl+",L"Ctrl+",L"Ctrl+",L"Ctrl+",L"Ctrl+",L"Ctrl+",L"Ctrl+",L"Ctrl+",L"Ctrl+",L"Ctrl+",L"Ctrl+",L"Ctrl+",L"Ctrl+",L"Ctrl+",L"Ctrl+",L"Ctrl+",L"Ctrl+"}},
    {2004,{L"Left Alt+",L"Alt sinistro+",L"Alt links+",L"Alt gauche+",L"Alt izquierdo+",L"Alt esquerdo+",L"Linker Alt+",L"Lewy Alt+",L"\u041B\u0435\u0432\u044B\u0439 Alt+",L"\u5DE6 Alt+",L"\u5DE6 Alt+",L"\uC67C\uCABD Alt+",L"Sol Alt+",L"Lev\u00FD Alt+",L"Bal Alt+",L"Alt st\u00E2nga+",L"V\u00E4nster Alt+",L"\u041B\u0456\u0432\u0438\u0439 Alt+",L"\u0391\u03C1\u03B9\u03C3\u03C4\u03B5\u03C1\u03CC Alt+",L"Alt \u064A\u0633\u0627\u0631+"}},
    {2005,{L"Shift+",L"Maiusc+",L"Umschalt+",L"Maj+",L"May\u00FAs+",L"Shift+",L"Shift+",L"Shift+",L"Shift+",L"Shift+",L"Shift+",L"Shift+",L"Shift+",L"Shift+",L"Shift+",L"Shift+",L"Skift+",L"Shift+",L"Shift+",L"Shift+"}},
    {2010,{L"Confirmation",L"Conferma",L"Best\u00E4tigung",L"Confirmation",L"Confirmaci\u00F3n",L"Confirma\u00E7\u00E3o",L"Bevestiging",L"Potwierdzenie",L"\u041F\u043E\u0434\u0442\u0432\u0435\u0440\u0436\u0434\u0435\u043D\u0438\u0435",L"\u786E\u8BA4",L"\u78BA\u8A8D",L"\uD655\uC778",L"Onay",L"Potvrzen\u00ED",L"Meger\u0151s\u00EDt\u00E9s",L"Confirmare",L"Bekr\u00E4ftelse",L"\u041F\u0456\u0434\u0442\u0432\u0435\u0440\u0434\u0436\u0435\u043D\u043D\u044F",L"\u0395\u03C0\u03B9\u03B2\u03B5\u03B2\u03B1\u03AF\u03C9\u03C3\u03B7",L"\u062A\u0623\u0643\u064A\u062F"}},
    {2011,{L"There are profiles that will be enabled with your change\r\n",L"Con la modifica apportata verranno abilitati alcuni profili\r\n",L"Es gibt Profile, die durch die \u00C4nderung aktiviert werden\r\n",L"Des profils seront activ\u00E9s par votre modification\r\n",L"Hay perfiles que se habilitar\u00E1n con el cambio\r\n",L"H\u00E1 perfis que ser\u00E3o habilitados com a sua altera\u00E7\u00E3o\r\n",L"Er zijn profielen die met uw wijziging worden ingeschakeld\r\n",L"Istniej\u0105 profile, kt\u00F3re zostan\u0105 w\u0142\u0105czone po wprowadzeniu zmian\r\n",L"\u0412 \u0440\u0435\u0437\u0443\u043B\u044C\u0442\u0430\u0442\u0435 \u0432\u043D\u0435\u0441\u0435\u043D\u043D\u044B\u0445 \u0438\u0437\u043C\u0435\u043D\u0435\u043D\u0438\u0439 \u0431\u0443\u0434\u0443\u0442 \u0432\u043A\u043B\u044E\u0447\u0435\u043D\u044B \u043D\u0435\u043A\u043E\u0442\u043E\u0440\u044B\u0435 \u043F\u0440\u043E\u0444\u0438\u043B\u0438\r\n",L"\u6709\u4E9B\u914D\u7F6E\u6587\u4EF6\u5C06\u968F\u60A8\u7684\u66F4\u6539\u800C\u542F\u7528\r\n",L"\u5909\u66F4\u306B\u3088\u3063\u3066\u6709\u52B9\u306B\u306A\u308B\u30D7\u30ED\u30D5\u30A1\u30A4\u30EB\u304C\u3042\u308A\u307E\u3059\r\n",L"\uBCC0\uACBD \uB0B4\uC6A9\uC73C\uB85C \uC0AC\uC6A9\uB418\uB3C4\uB85D \uC124\uC815\uB420 \uD504\uB85C\uD544\uC774 \uC788\uC2B5\uB2C8\uB2E4\r\n",L"De\u011Fi\u015Fikli\u011Finizle etkinle\u015Ftirilecek profiller var\r\n",L"N\u011Bkter\u00E9 profily budou zm\u011Bnou povoleny\r\n",L"Vannak profilok, amelyek a m\u00F3dos\u00EDt\u00E1ssal enged\u00E9lyezve lesznek\r\n",L"Exist\u0103 profiluri care vor fi activate cu modificarea dumneavoastr\u0103\r\n",L"Det finns profiler som aktiveras n\u00E4r du \u00E4ndrar\r\n",L"\u0404 \u043F\u0440\u043E\u0444\u0456\u043B\u0456, \u044F\u043A\u0456 \u0431\u0443\u0434\u0435 \u0443\u0432\u0456\u043C\u043A\u043D\u0435\u043D\u043E \u0432\u0430\u0448\u043E\u044E \u0437\u043C\u0456\u043D\u043E\u044E\r\n",L"\u03A5\u03C0\u03AC\u03C1\u03C7\u03BF\u03C5\u03BD \u03C0\u03C1\u03BF\u03C6\u03AF\u03BB \u03C0\u03BF\u03C5 \u03B8\u03B1 \u03B5\u03BD\u03B5\u03C1\u03B3\u03BF\u03C0\u03BF\u03B9\u03B7\u03B8\u03BF\u03CD\u03BD \u03BC\u03B5 \u03C4\u03B7\u03BD \u03B1\u03BB\u03BB\u03B1\u03B3\u03AE \u03C3\u03B1\u03C2\r\n",L"\u062A\u0648\u062C\u062F \u0645\u0644\u0641\u0627\u062A \u062A\u0639\u0631\u064A\u0641 \u0633\u064A\u062A\u0645 \u062A\u0645\u0643\u064A\u0646\u0647\u0627 \u0628\u0627\u0644\u062A\u063A\u064A\u064A\u0631 \u0627\u0644\u0630\u064A \u0623\u062C\u0631\u064A\u062A\u0647\r\n"}},
    {2012,{L"There are profiles that will be disabled with your change\r\n",L"Con la modifica apportata verranno disabilitati alcuni profili\r\n",L"Es gibt Profile, die durch die \u00C4nderung deaktiviert werden\r\n",L"Des profils seront d\u00E9sactiv\u00E9s par votre modification\r\n",L"Hay perfiles que se deshabilitar\u00E1n con el cambio\r\n",L"H\u00E1 perfis que ser\u00E3o desabilitados com a sua altera\u00E7\u00E3o\r\n",L"Er zijn profielen die met uw wijziging worden uitgeschakeld\r\n",L"Istniej\u0105 profile, kt\u00F3re zostan\u0105 wy\u0142\u0105czone po wprowadzeniu zmian\r\n",L"\u0412 \u0440\u0435\u0437\u0443\u043B\u044C\u0442\u0430\u0442\u0435 \u0432\u043D\u0435\u0441\u0435\u043D\u043D\u044B\u0445 \u0438\u0437\u043C\u0435\u043D\u0435\u043D\u0438\u0439 \u0431\u0443\u0434\u0443\u0442 \u043E\u0442\u043A\u043B\u044E\u0447\u0435\u043D\u044B \u043D\u0435\u043A\u043E\u0442\u043E\u0440\u044B\u0435 \u043F\u0440\u043E\u0444\u0438\u043B\u0438\r\n",L"\u6709\u4E9B\u914D\u7F6E\u6587\u4EF6\u5C06\u968F\u60A8\u7684\u66F4\u6539\u800C\u7981\u7528\r\n",L"\u5909\u66F4\u306B\u3088\u3063\u3066\u7121\u52B9\u306B\u306A\u308B\u30D7\u30ED\u30D5\u30A1\u30A4\u30EB\u304C\u3042\u308A\u307E\u3059\r\n",L"\uBCC0\uACBD \uB0B4\uC6A9\uC73C\uB85C \uC0AC\uC6A9\uD558\uC9C0 \uC54A\uB3C4\uB85D \uC124\uC815\uB420 \uD504\uB85C\uD544\uC774 \uC788\uC2B5\uB2C8\uB2E4\r\n",L"De\u011Fi\u015Fikli\u011Finizle devre d\u0131\u015F\u0131 b\u0131rak\u0131lacak profiller var\r\n",L"N\u011Bkter\u00E9 profily budou zm\u011Bnou zak\u00E1z\u00E1ny\r\n",L"Vannak profilok, amelyek a m\u00F3dos\u00EDt\u00E1ssal le lesznek tiltva\r\n",L"Exist\u0103 profiluri care vor fi dezactivate cu modificarea dumneavoastr\u0103\r\n",L"Det finns profiler som inaktiveras n\u00E4r du \u00E4ndrar\r\n",L"\u0404 \u043F\u0440\u043E\u0444\u0456\u043B\u0456, \u044F\u043A\u0456 \u0431\u0443\u0434\u0435 \u0432\u0438\u043C\u043A\u043D\u0435\u043D\u043E \u0432\u0430\u0448\u043E\u044E \u0437\u043C\u0456\u043D\u043E\u044E\r\n",L"\u03A5\u03C0\u03AC\u03C1\u03C7\u03BF\u03C5\u03BD \u03C0\u03C1\u03BF\u03C6\u03AF\u03BB \u03C0\u03BF\u03C5 \u03B8\u03B1 \u03B1\u03C0\u03B5\u03BD\u03B5\u03C1\u03B3\u03BF\u03C0\u03BF\u03B9\u03B7\u03B8\u03BF\u03CD\u03BD \u03BC\u03B5 \u03C4\u03B7\u03BD \u03B1\u03BB\u03BB\u03B1\u03B3\u03AE \u03C3\u03B1\u03C2\r\n",L"\u062A\u0648\u062C\u062F \u0645\u0644\u0641\u0627\u062A \u062A\u0639\u0631\u064A\u0641 \u0633\u064A\u062A\u0645 \u062A\u0639\u0637\u064A\u0644\u0647\u0627 \u0628\u0627\u0644\u062A\u063A\u064A\u064A\u0631 \u0627\u0644\u0630\u064A \u0623\u062C\u0631\u064A\u062A\u0647\r\n"}},
    {2020,{L"Keyboard",L"Tastiera",L"Tastatur",L"Clavier",L"Teclado",L"Teclado",L"Toetsenbord",L"Klawiatura",L"\u041A\u043B\u0430\u0432\u0438\u0430\u0442\u0443\u0440\u0430",L"\u952E\u76D8",L"\u30AD\u30FC\u30DC\u30FC\u30C9",L"\uD0A4\uBCF4\uB4DC",L"Klavye",L"Kl\u00E1vesnice",L"Billenty\u0171zet",L"Tastatur\u0103",L"Tangentbord",L"\u041A\u043B\u0430\u0432\u0456\u0430\u0442\u0443\u0440\u0430",L"\u03A0\u03BB\u03B7\u03BA\u03C4\u03C1\u03BF\u03BB\u03CC\u03B3\u03B9\u03BF",L"\u0644\u0648\u062D\u0629 \u0627\u0644\u0645\u0641\u0627\u062A\u064A\u062D"}},
    {2021,{L"Speech",L"Riconoscimento vocale",L"Spracherkennung",L"Reconnaissance vocale",L"Voz",L"Voz",L"Spraak",L"Mowa",L"\u0420\u0435\u0447\u044C",L"\u8BED\u97F3",L"\u97F3\u58F0",L"\uC74C\uC131",L"Konu\u015Fma",L"\u0158e\u010D",L"Besz\u00E9d",L"Vorbire",L"Tal",L"\u041C\u043E\u0432\u043B\u0435\u043D\u043D\u044F",L"\u039F\u03BC\u03B9\u03BB\u03AF\u03B1",L"\u0627\u0644\u0643\u0644\u0627\u0645"}},
    {2022,{L"Handwriting",L"Scrittura a mano",L"Handschrift",L"\u00C9criture manuscrite",L"Escritura a mano",L"Escrita manual",L"Handschrift",L"Pismo odr\u0119czne",L"\u0420\u0443\u043A\u043E\u043F\u0438\u0441\u043D\u044B\u0439 \u0432\u0432\u043E\u0434",L"\u624B\u5199",L"\u624B\u66F8\u304D",L"\uD544\uAE30",L"El yaz\u0131s\u0131",L"Psan\u00ED rukou",L"K\u00E9z\u00EDr\u00E1s",L"Scriere de m\u00E2n\u0103",L"Handskrift",L"\u0420\u0443\u043A\u043E\u043F\u0438\u0441\u043D\u0435 \u0432\u0432\u0435\u0434\u0435\u043D\u043D\u044F",L"\u03A7\u03B5\u03B9\u03C1\u03CC\u03B3\u03C1\u03B1\u03C6\u03BF",L"\u0627\u0644\u0643\u062A\u0627\u0628\u0629 \u0627\u0644\u064A\u062F\u0648\u064A\u0629"}},
    {2023,{L"Reference",L"Riferimento",L"Verweis",L"R\u00E9f\u00E9rence",L"Referencia",L"Refer\u00EAncia",L"Verwijzing",L"Odwo\u0142anie",L"\u0421\u043F\u0440\u0430\u0432\u043A\u0430",L"\u53C2\u8003",L"\u30EA\u30D5\u30A1\u30EC\u30F3\u30B9",L"\uCC38\uC870",L"Ba\u015Fvuru",L"Odkaz",L"Hivatkoz\u00E1s",L"Referin\u021B\u0103",L"Referens",L"\u0414\u043E\u0432\u0456\u0434\u043A\u0430",L"\u0391\u03BD\u03B1\u03C6\u03BF\u03C1\u03AC",L"\u0645\u0631\u062C\u0639"}},
    {2024,{L"Proofing",L"Strumenti di correzione",L"Korrekturhilfen",L"Correction",L"Correcci\u00F3n",L"Revis\u00E3o",L"Controle",L"Korekta",L"\u041F\u0440\u0430\u0432\u043E\u043F\u0438\u0441\u0430\u043D\u0438\u0435",L"\u6821\u5BF9",L"\u6587\u7AE0\u6821\u6B63",L"\uC5B8\uC5B4 \uAD50\uC815",L"Yaz\u0131m denetimi",L"Kontrola pravopisu",L"Nyelvhelyess\u00E9g-ellen\u0151rz\u00E9s",L"Corectare",L"Korrekturl\u00E4sning",L"\u041F\u0440\u0430\u0432\u043E\u043F\u0438\u0441",L"\u0394\u03B9\u03CC\u03C1\u03B8\u03C9\u03C3\u03B7",L"\u062A\u062F\u0642\u064A\u0642"}},
    {2025,{L"Smart Tag",L"Smart Tag",L"Smarttag",L"Balise active",L"Etiqueta inteligente",L"Marca Inteligente",L"Smarttag",L"Tag inteligentny",L"\u0421\u043C\u0430\u0440\u0442-\u0442\u0435\u0433",L"\u667A\u80FD\u6807\u8BB0",L"\u30B9\u30DE\u30FC\u30C8 \u30BF\u30B0",L"\uC2A4\uB9C8\uD2B8 \uD0DC\uADF8",L"Ak\u0131ll\u0131 Etiket",L"Inteligentn\u00ED zna\u010Dka",L"Intelligens c\u00EDmke",L"Marcaj inteligent",L"Smarttagg",L"\u0421\u043C\u0430\u0440\u0442-\u0442\u0435\u0433",L"\u0388\u03BE\u03C5\u03C0\u03BD\u03B7 \u03B5\u03C4\u03B9\u03BA\u03AD\u03C4\u03B1",L"\u0639\u0644\u0627\u0645\u0629 \u0630\u0643\u064A\u0629"}},
    {2026,{L"Other",L"Altro",L"Andere",L"Autre",L"Otro",L"Outro",L"Overig",L"Inne",L"\u0414\u0440\u0443\u0433\u043E\u0435",L"\u5176\u4ED6",L"\u305D\u306E\u4ED6",L"\uAE30\uD0C0",L"Di\u011Fer",L"Jin\u00E9",L"Egy\u00E9b",L"Altele",L"\u00D6vrigt",L"\u0406\u043D\u0448\u0435",L"\u0386\u03BB\u03BB\u03BF",L"\u0623\u062E\u0631\u0649"}},
    {2030,{L" (64Bit Only)",L" (solo a 64 bit)",L" (nur 64 Bit)",L" (64 bits uniquement)",L" (solo 64 bits)",L" (somente 64 bits)",L" (alleen 64-bits)",L" (tylko 64-bitowe)",L" (\u0442\u043E\u043B\u044C\u043A\u043E 64-\u0440\u0430\u0437\u0440\u044F\u0434\u043D\u044B\u0435)",L" (\u4EC5\u9650 64 \u4F4D)",L" (64 \u30D3\u30C3\u30C8\u306E\u307F)",L" (64\uBE44\uD2B8 \uC804\uC6A9)",L" (yaln\u0131zca 64 Bit)",L" (pouze 64bitov\u00E9)",L" (csak 64 bites)",L" (doar 64 de bi\u021Bi)",L" (endast 64 bitar)",L" (\u043B\u0438\u0448\u0435 64-\u0440\u043E\u0437\u0440\u044F\u0434\u043D\u0456)",L" (\u03BC\u03CC\u03BD\u03BF 64 bit)",L" (64 \u0628\u062A \u0641\u0642\u0637)"}},
    {2031,{L" (32Bit Only)",L" (solo a 32 bit)",L" (nur 32 Bit)",L" (32 bits uniquement)",L" (solo 32 bits)",L" (somente 32 bits)",L" (alleen 32-bits)",L" (tylko 32-bitowe)",L" (\u0442\u043E\u043B\u044C\u043A\u043E 32-\u0440\u0430\u0437\u0440\u044F\u0434\u043D\u044B\u0435)",L" (\u4EC5\u9650 32 \u4F4D)",L" (32 \u30D3\u30C3\u30C8\u306E\u307F)",L" (32\uBE44\uD2B8 \uC804\uC6A9)",L" (yaln\u0131zca 32 Bit)",L" (pouze 32bitov\u00E9)",L" (csak 32 bites)",L" (doar 32 de bi\u021Bi)",L" (endast 32 bitar)",L" (\u043B\u0438\u0448\u0435 32-\u0440\u043E\u0437\u0440\u044F\u0434\u043D\u0456)",L" (\u03BC\u03CC\u03BD\u03BF 32 bit)",L" (32 \u0628\u062A \u0641\u0642\u0637)"}},
    {2032,{L"%s is available only on 32 bit processes.\r\nDo you want to make this as a default input item?",L"%s \u00E8 disponibile solo in processi a 32 bit.\r\nImpostarlo come elemento di input predefinito?",L"%s ist nur in 32-Bit-Prozessen verf\u00FCgbar.\r\nSoll dies als Standardeingabeelement festgelegt werden?",L"%s est disponible uniquement dans les processus 32 bits.\r\nVoulez-vous en faire l'\u00E9l\u00E9ment d'entr\u00E9e par d\u00E9faut ?",L"%s est\u00E1 disponible solo en procesos de 32 bits.\r\n\u00BFDesea establecerlo como elemento de entrada predeterminado?",L"%s est\u00E1 dispon\u00EDvel somente em processos de 32 bits.\r\nDeseja torn\u00E1-lo o item de entrada padr\u00E3o?",L"%s is alleen beschikbaar in 32-bitsprocessen.\r\nWilt u dit als standaardinvoeritem instellen?",L"%s jest dost\u0119pny tylko w procesach 32-bitowych.\r\nCzy ustawi\u0107 go jako domy\u015Blny element wprowadzania?",L"%s \u0434\u043E\u0441\u0442\u0443\u043F\u0435\u043D \u0442\u043E\u043B\u044C\u043A\u043E \u0432 32-\u0440\u0430\u0437\u0440\u044F\u0434\u043D\u044B\u0445 \u043F\u0440\u043E\u0446\u0435\u0441\u0441\u0430\u0445.\r\n\u0421\u0434\u0435\u043B\u0430\u0442\u044C \u0435\u0433\u043E \u044D\u043B\u0435\u043C\u0435\u043D\u0442\u043E\u043C \u0432\u0432\u043E\u0434\u0430 \u043F\u043E \u0443\u043C\u043E\u043B\u0447\u0430\u043D\u0438\u044E?",L"%s \u4EC5\u5728 32 \u4F4D\u8FDB\u7A0B\u4E2D\u53EF\u7528\u3002\r\n\u662F\u5426\u5C06\u5176\u8BBE\u4E3A\u9ED8\u8BA4\u8F93\u5165\u9879?",L"%s \u306F 32 \u30D3\u30C3\u30C8 \u30D7\u30ED\u30BB\u30B9\u3067\u306E\u307F\u4F7F\u7528\u3067\u304D\u307E\u3059\u3002\r\n\u65E2\u5B9A\u306E\u5165\u529B\u9805\u76EE\u3068\u3057\u3066\u8A2D\u5B9A\u3057\u307E\u3059\u304B?",L"%s\uB294(\uC740) 32\uBE44\uD2B8 \uD504\uB85C\uC138\uC2A4\uC5D0\uC11C\uB9CC \uC0AC\uC6A9\uD560 \uC218 \uC788\uC2B5\uB2C8\uB2E4.\r\n\uAE30\uBCF8 \uC785\uB825 \uD56D\uBAA9\uC73C\uB85C \uC124\uC815\uD558\uC2DC\uACA0\uC2B5\uB2C8\uAE4C?",L"%s yaln\u0131zca 32 bit i\u015Flemlerde kullan\u0131labilir.\r\nVarsay\u0131lan giri\u015F \u00F6\u011Fesi olarak ayarlamak ister misiniz?",L"%s je dostupn\u00FD jen v 32bitov\u00FDch procesech.\r\nChcete ho nastavit jako v\u00FDchoz\u00ED vstupn\u00ED polo\u017Eku?",L"%s csak 32 bites folyamatokban \u00E9rhet\u0151 el.\r\nSzeretn\u00E9 alap\u00E9rtelmezett beviteli elemk\u00E9nt be\u00E1ll\u00EDtani?",L"%s este disponibil doar \u00EEn procese de 32 de bi\u021Bi.\r\nDori\u021Bi s\u0103 \u00EEl seta\u021Bi ca element de intrare implicit?",L"%s \u00E4r endast tillg\u00E4nglig i 32-bitarsprocesser.\r\nVill du g\u00F6ra den till standardinmatningsobjekt?",L"%s \u0434\u043E\u0441\u0442\u0443\u043F\u043D\u0438\u0439 \u043B\u0438\u0448\u0435 \u0432 32-\u0440\u043E\u0437\u0440\u044F\u0434\u043D\u0438\u0445 \u043F\u0440\u043E\u0446\u0435\u0441\u0430\u0445.\r\n\u0417\u0440\u043E\u0431\u0438\u0442\u0438 \u0439\u043E\u0433\u043E \u0435\u043B\u0435\u043C\u0435\u043D\u0442\u043E\u043C \u0432\u0432\u0435\u0434\u0435\u043D\u043D\u044F \u0437\u0430 \u0437\u0430\u043C\u043E\u0432\u0447\u0443\u0432\u0430\u043D\u043D\u044F\u043C?",L"\u03A4\u03BF %s \u03B5\u03AF\u03BD\u03B1\u03B9 \u03B4\u03B9\u03B1\u03B8\u03AD\u03C3\u03B9\u03BC\u03BF \u03BC\u03CC\u03BD\u03BF \u03C3\u03B5 \u03B4\u03B9\u03B1\u03B4\u03B9\u03BA\u03B1\u03C3\u03AF\u03B5\u03C2 32 bit.\r\n\u0398\u03AD\u03BB\u03B5\u03C4\u03B5 \u03BD\u03B1 \u03C4\u03BF \u03BF\u03C1\u03AF\u03C3\u03B5\u03C4\u03B5 \u03C9\u03C2 \u03C0\u03C1\u03BF\u03B5\u03C0\u03B9\u03BB\u03B5\u03B3\u03BC\u03AD\u03BD\u03BF \u03C3\u03C4\u03BF\u03B9\u03C7\u03B5\u03AF\u03BF \u03B5\u03B9\u03C3\u03CC\u03B4\u03BF\u03C5;",L"\u200E%s \u0645\u062A\u0648\u0641\u0631 \u0641\u064A \u0639\u0645\u0644\u064A\u0627\u062A 32 \u0628\u062A \u0641\u0642\u0637.\r\n\u0647\u0644 \u062A\u0631\u064A\u062F \u062A\u0639\u064A\u064A\u0646\u0647 \u0643\u0639\u0646\u0635\u0631 \u0625\u062F\u062E\u0627\u0644 \u0627\u0641\u062A\u0631\u0627\u0636\u064A\u061F"}},
    {2033,{L"The following input methods are available only on 32 bit processes.\r\n%sDo you want to include them in Default User Account?",L"I metodi di input seguenti sono disponibili solo in processi a 32 bit.\r\n%sIncluderli nell'account utente predefinito?",L"Die folgenden Eingabemethoden sind nur in 32-Bit-Prozessen verf\u00FCgbar.\r\n%sSollen sie in das Standardbenutzerkonto aufgenommen werden?",L"Les m\u00E9thodes d'entr\u00E9e suivantes sont disponibles uniquement dans les processus 32 bits.\r\n%sVoulez-vous les inclure dans le compte d'utilisateur par d\u00E9faut ?",L"Los m\u00E9todos de entrada siguientes solo est\u00E1n disponibles en procesos de 32 bits.\r\n%s\u00BFDesea incluirlos en la cuenta de usuario predeterminada?",L"Os m\u00E9todos de entrada a seguir est\u00E3o dispon\u00EDveis somente em processos de 32 bits.\r\n%sDeseja inclu\u00ED-los na Conta de Usu\u00E1rio Padr\u00E3o?",L"De volgende invoermethoden zijn alleen beschikbaar in 32-bitsprocessen.\r\n%sWilt u deze opnemen in het standaardgebruikersaccount?",L"Nast\u0119puj\u0105ce metody wprowadzania s\u0105 dost\u0119pne tylko w procesach 32-bitowych.\r\n%sCzy do\u0142\u0105czy\u0107 je do domy\u015Blnego konta u\u017Cytkownika?",L"\u0421\u043B\u0435\u0434\u0443\u044E\u0449\u0438\u0435 \u043C\u0435\u0442\u043E\u0434\u044B \u0432\u0432\u043E\u0434\u0430 \u0434\u043E\u0441\u0442\u0443\u043F\u043D\u044B \u0442\u043E\u043B\u044C\u043A\u043E \u0432 32-\u0440\u0430\u0437\u0440\u044F\u0434\u043D\u044B\u0445 \u043F\u0440\u043E\u0446\u0435\u0441\u0441\u0430\u0445.\r\n%s\u0412\u043A\u043B\u044E\u0447\u0438\u0442\u044C \u0438\u0445 \u0432 \u0443\u0447\u0435\u0442\u043D\u0443\u044E \u0437\u0430\u043F\u0438\u0441\u044C \u043F\u043E\u043B\u044C\u0437\u043E\u0432\u0430\u0442\u0435\u043B\u044F \u043F\u043E \u0443\u043C\u043E\u043B\u0447\u0430\u043D\u0438\u044E?",L"\u4EE5\u4E0B\u8F93\u5165\u6CD5\u4EC5\u5728 32 \u4F4D\u8FDB\u7A0B\u4E2D\u53EF\u7528\u3002\r\n%s\u662F\u5426\u5C06\u5B83\u4EEC\u5305\u542B\u5728\u9ED8\u8BA4\u7528\u6237\u5E10\u6237\u4E2D?",L"\u6B21\u306E\u5165\u529B\u30E1\u30BD\u30C3\u30C9\u306F 32 \u30D3\u30C3\u30C8 \u30D7\u30ED\u30BB\u30B9\u3067\u306E\u307F\u4F7F\u7528\u3067\u304D\u307E\u3059\u3002\r\n%s\u65E2\u5B9A\u306E\u30E6\u30FC\u30B6\u30FC \u30A2\u30AB\u30A6\u30F3\u30C8\u306B\u542B\u3081\u307E\u3059\u304B?",L"\uB2E4\uC74C \uC785\uB825 \uBC29\uBC95\uC740 32\uBE44\uD2B8 \uD504\uB85C\uC138\uC2A4\uC5D0\uC11C\uB9CC \uC0AC\uC6A9\uD560 \uC218 \uC788\uC2B5\uB2C8\uB2E4.\r\n%s\uAE30\uBCF8 \uC0AC\uC6A9\uC790 \uACC4\uC815\uC5D0 \uD3EC\uD568\uC2DC\uD0A4\uACA0\uC2B5\uB2C8\uAE4C?",L"A\u015Fa\u011F\u0131daki giri\u015F y\u00F6ntemleri yaln\u0131zca 32 bit i\u015Flemlerde kullan\u0131labilir.\r\n%sBunlar\u0131 Varsay\u0131lan Kullan\u0131c\u0131 Hesab\u0131'na eklemek ister misiniz?",L"N\u00E1sleduj\u00EDc\u00ED metody zad\u00E1v\u00E1n\u00ED jsou dostupn\u00E9 jen v 32bitov\u00FDch procesech.\r\n%sChcete je zahrnout do v\u00FDchoz\u00EDho u\u017Eivatelsk\u00E9ho \u00FA\u010Dtu?",L"A k\u00F6vetkez\u0151 beviteli m\u00F3dok csak 32 bites folyamatokban \u00E9rhet\u0151k el.\r\n%sSzeretn\u00E9 belefoglalni \u0151ket az alap\u00E9rtelmezett felhaszn\u00E1l\u00F3i fi\u00F3kba?",L"Urm\u0103toarele metode de introducere sunt disponibile doar \u00EEn procese de 32 de bi\u021Bi.\r\n%sDori\u021Bi s\u0103 le include\u021Bi \u00EEn Contul de utilizator implicit?",L"F\u00F6ljande inmatningsmetoder \u00E4r endast tillg\u00E4ngliga i 32-bitarsprocesser.\r\n%sVill du inkludera dem i standardanv\u00E4ndarkontot?",L"\u0422\u0430\u043A\u0456 \u043C\u0435\u0442\u043E\u0434\u0438 \u0432\u0432\u0435\u0434\u0435\u043D\u043D\u044F \u0434\u043E\u0441\u0442\u0443\u043F\u043D\u0456 \u043B\u0438\u0448\u0435 \u0432 32-\u0440\u043E\u0437\u0440\u044F\u0434\u043D\u0438\u0445 \u043F\u0440\u043E\u0446\u0435\u0441\u0430\u0445.\r\n%s\u0412\u043A\u043B\u044E\u0447\u0438\u0442\u0438 \u0457\u0445 \u0434\u043E \u043E\u0431\u043B\u0456\u043A\u043E\u0432\u043E\u0433\u043E \u0437\u0430\u043F\u0438\u0441\u0443 \u043A\u043E\u0440\u0438\u0441\u0442\u0443\u0432\u0430\u0447\u0430 \u0437\u0430 \u0437\u0430\u043C\u043E\u0432\u0447\u0443\u0432\u0430\u043D\u043D\u044F\u043C?",L"\u039F\u03B9 \u03B1\u03BA\u03CC\u03BB\u03BF\u03C5\u03B8\u03B5\u03C2 \u03BC\u03AD\u03B8\u03BF\u03B4\u03BF\u03B9 \u03B5\u03B9\u03C3\u03B1\u03B3\u03C9\u03B3\u03AE\u03C2 \u03B5\u03AF\u03BD\u03B1\u03B9 \u03B4\u03B9\u03B1\u03B8\u03AD\u03C3\u03B9\u03BC\u03B5\u03C2 \u03BC\u03CC\u03BD\u03BF \u03C3\u03B5 \u03B4\u03B9\u03B1\u03B4\u03B9\u03BA\u03B1\u03C3\u03AF\u03B5\u03C2 32 bit.\r\n%s\u0398\u03AD\u03BB\u03B5\u03C4\u03B5 \u03BD\u03B1 \u03C4\u03B9\u03C2 \u03C3\u03C5\u03BC\u03C0\u03B5\u03C1\u03B9\u03BB\u03AC\u03B2\u03B5\u03C4\u03B5 \u03C3\u03C4\u03BF\u03BD \u03C0\u03C1\u03BF\u03B5\u03C0\u03B9\u03BB\u03B5\u03B3\u03BC\u03AD\u03BD\u03BF \u03BB\u03BF\u03B3\u03B1\u03C1\u03B9\u03B1\u03C3\u03BC\u03CC \u03C7\u03C1\u03AE\u03C3\u03C4\u03B7;",L"\u0623\u0633\u0627\u0644\u064A\u0628 \u0627\u0644\u0625\u062F\u062E\u0627\u0644 \u0627\u0644\u062A\u0627\u0644\u064A\u0629 \u0645\u062A\u0648\u0641\u0631\u0629 \u0641\u064A \u0639\u0645\u0644\u064A\u0627\u062A 32 \u0628\u062A \u0641\u0642\u0637.\r\n%s\u0647\u0644 \u062A\u0631\u064A\u062F \u062A\u0636\u0645\u064A\u0646\u0647\u0627 \u0641\u064A \u062D\u0633\u0627\u0628 \u0627\u0644\u0645\u0633\u062A\u062E\u062F\u0645 \u0627\u0644\u0627\u0641\u062A\u0631\u0627\u0636\u064A\u061F"}},
    {2034,{L"The following input methods are available only on 32 bit processes.\r\n%sThese can not be applied to System Accounts.",L"I metodi di input seguenti sono disponibili solo in processi a 32 bit.\r\n%sNon \u00E8 possibile applicarli agli account di sistema.",L"Die folgenden Eingabemethoden sind nur in 32-Bit-Prozessen verf\u00FCgbar.\r\n%sSie k\u00F6nnen nicht auf Systemkonten angewendet werden.",L"Les m\u00E9thodes d'entr\u00E9e suivantes sont disponibles uniquement dans les processus 32 bits.\r\n%sElles ne peuvent pas \u00EAtre appliqu\u00E9es aux comptes syst\u00E8me.",L"Los m\u00E9todos de entrada siguientes solo est\u00E1n disponibles en procesos de 32 bits.\r\n%sNo se pueden aplicar a cuentas del sistema.",L"Os m\u00E9todos de entrada a seguir est\u00E3o dispon\u00EDveis somente em processos de 32 bits.\r\n%sEles n\u00E3o podem ser aplicados a Contas do Sistema.",L"De volgende invoermethoden zijn alleen beschikbaar in 32-bitsprocessen.\r\n%sDeze kunnen niet worden toegepast op systeemaccounts.",L"Nast\u0119puj\u0105ce metody wprowadzania s\u0105 dost\u0119pne tylko w procesach 32-bitowych.\r\n%sNie mo\u017Cna ich stosowa\u0107 do kont systemowych.",L"\u0421\u043B\u0435\u0434\u0443\u044E\u0449\u0438\u0435 \u043C\u0435\u0442\u043E\u0434\u044B \u0432\u0432\u043E\u0434\u0430 \u0434\u043E\u0441\u0442\u0443\u043F\u043D\u044B \u0442\u043E\u043B\u044C\u043A\u043E \u0432 32-\u0440\u0430\u0437\u0440\u044F\u0434\u043D\u044B\u0445 \u043F\u0440\u043E\u0446\u0435\u0441\u0441\u0430\u0445.\r\n%s\u0418\u0445 \u043D\u0435\u043B\u044C\u0437\u044F \u043F\u0440\u0438\u043C\u0435\u043D\u0438\u0442\u044C \u043A \u0441\u0438\u0441\u0442\u0435\u043C\u043D\u044B\u043C \u0443\u0447\u0435\u0442\u043D\u044B\u043C \u0437\u0430\u043F\u0438\u0441\u044F\u043C.",L"\u4EE5\u4E0B\u8F93\u5165\u6CD5\u4EC5\u5728 32 \u4F4D\u8FDB\u7A0B\u4E2D\u53EF\u7528\u3002\r\n%s\u65E0\u6CD5\u5C06\u5B83\u4EEC\u5E94\u7528\u4E8E\u7CFB\u7EDF\u5E10\u6237\u3002",L"\u6B21\u306E\u5165\u529B\u30E1\u30BD\u30C3\u30C9\u306F 32 \u30D3\u30C3\u30C8 \u30D7\u30ED\u30BB\u30B9\u3067\u306E\u307F\u4F7F\u7528\u3067\u304D\u307E\u3059\u3002\r\n%s\u3053\u308C\u3089\u306F\u30B7\u30B9\u30C6\u30E0 \u30A2\u30AB\u30A6\u30F3\u30C8\u306B\u306F\u9069\u7528\u3067\u304D\u307E\u305B\u3093\u3002",L"\uB2E4\uC74C \uC785\uB825 \uBC29\uBC95\uC740 32\uBE44\uD2B8 \uD504\uB85C\uC138\uC2A4\uC5D0\uC11C\uB9CC \uC0AC\uC6A9\uD560 \uC218 \uC788\uC2B5\uB2C8\uB2E4.\r\n%s\uC2DC\uC2A4\uD15C \uACC4\uC815\uC5D0\uB294 \uC801\uC6A9\uD560 \uC218 \uC5C6\uC2B5\uB2C8\uB2E4.",L"A\u015Fa\u011F\u0131daki giri\u015F y\u00F6ntemleri yaln\u0131zca 32 bit i\u015Flemlerde kullan\u0131labilir.\r\n%sBunlar Sistem Hesaplar\u0131na uygulanamaz.",L"N\u00E1sleduj\u00EDc\u00ED metody zad\u00E1v\u00E1n\u00ED jsou dostupn\u00E9 jen v 32bitov\u00FDch procesech.\r\n%sNejdou pou\u017E\u00EDt pro syst\u00E9mov\u00E9 \u00FA\u010Dty.",L"A k\u00F6vetkez\u0151 beviteli m\u00F3dok csak 32 bites folyamatokban \u00E9rhet\u0151k el.\r\n%sRendszerfi\u00F3kokra nem alkalmazhat\u00F3k.",L"Urm\u0103toarele metode de introducere sunt disponibile doar \u00EEn procese de 32 de bi\u021Bi.\r\n%sNu pot fi aplicate Conturilor de sistem.",L"F\u00F6ljande inmatningsmetoder \u00E4r endast tillg\u00E4ngliga i 32-bitarsprocesser.\r\n%sDe kan inte till\u00E4mpas p\u00E5 systemkonton.",L"\u0422\u0430\u043A\u0456 \u043C\u0435\u0442\u043E\u0434\u0438 \u0432\u0432\u0435\u0434\u0435\u043D\u043D\u044F \u0434\u043E\u0441\u0442\u0443\u043F\u043D\u0456 \u043B\u0438\u0448\u0435 \u0432 32-\u0440\u043E\u0437\u0440\u044F\u0434\u043D\u0438\u0445 \u043F\u0440\u043E\u0446\u0435\u0441\u0430\u0445.\r\n%s\u0407\u0445 \u043D\u0435 \u043C\u043E\u0436\u043D\u0430 \u0437\u0430\u0441\u0442\u043E\u0441\u0443\u0432\u0430\u0442\u0438 \u0434\u043E \u0441\u0438\u0441\u0442\u0435\u043C\u043D\u0438\u0445 \u043E\u0431\u043B\u0456\u043A\u043E\u0432\u0438\u0445 \u0437\u0430\u043F\u0438\u0441\u0456\u0432.",L"\u039F\u03B9 \u03B1\u03BA\u03CC\u03BB\u03BF\u03C5\u03B8\u03B5\u03C2 \u03BC\u03AD\u03B8\u03BF\u03B4\u03BF\u03B9 \u03B5\u03B9\u03C3\u03B1\u03B3\u03C9\u03B3\u03AE\u03C2 \u03B5\u03AF\u03BD\u03B1\u03B9 \u03B4\u03B9\u03B1\u03B8\u03AD\u03C3\u03B9\u03BC\u03B5\u03C2 \u03BC\u03CC\u03BD\u03BF \u03C3\u03B5 \u03B4\u03B9\u03B1\u03B4\u03B9\u03BA\u03B1\u03C3\u03AF\u03B5\u03C2 32 bit.\r\n%s\u0394\u03B5\u03BD \u03BC\u03C0\u03BF\u03C1\u03BF\u03CD\u03BD \u03BD\u03B1 \u03B5\u03C6\u03B1\u03C1\u03BC\u03BF\u03C3\u03C4\u03BF\u03CD\u03BD \u03C3\u03B5 \u03BB\u03BF\u03B3\u03B1\u03C1\u03B9\u03B1\u03C3\u03BC\u03BF\u03CD\u03C2 \u03C3\u03C5\u03C3\u03C4\u03AE\u03BC\u03B1\u03C4\u03BF\u03C2.",L"\u0623\u0633\u0627\u0644\u064A\u0628 \u0627\u0644\u0625\u062F\u062E\u0627\u0644 \u0627\u0644\u062A\u0627\u0644\u064A\u0629 \u0645\u062A\u0648\u0641\u0631\u0629 \u0641\u064A \u0639\u0645\u0644\u064A\u0627\u062A 32 \u0628\u062A \u0641\u0642\u0637.\r\n%s\u0644\u0627 \u064A\u0645\u0643\u0646 \u062A\u0637\u0628\u064A\u0642\u0647\u0627 \u0639\u0644\u0649 \u062D\u0633\u0627\u0628\u0627\u062A \u0627\u0644\u0646\u0638\u0627\u0645."}},
    {2036,{L"Show More...",L"Mostra altro...",L"Weitere anzeigen...",L"Afficher plus...",L"Mostrar m\u00E1s...",L"Mostrar mais...",L"Meer weergeven...",L"Poka\u017C wi\u0119cej...",L"\u041F\u043E\u043A\u0430\u0437\u0430\u0442\u044C \u0435\u0449\u0435...",L"\u663E\u793A\u66F4\u591A...",L"\u8A73\u7D30\u3092\u8868\u793A...",L"\uB354 \uBCF4\uAE30...",L"Daha fazla g\u00F6ster...",L"Zobrazit dal\u0161\u00ED...",L"Tov\u00E1bbiak megjelen\u00EDt\u00E9se...",L"Afi\u0219a\u021Bi mai multe...",L"Visa mer...",L"\u041F\u043E\u043A\u0430\u0437\u0430\u0442\u0438 \u0431\u0456\u043B\u044C\u0448\u0435...",L"\u0395\u03BC\u03C6\u03AC\u03BD\u03B9\u03C3\u03B7 \u03C0\u03B5\u03C1\u03B9\u03C3\u03C3\u03CC\u03C4\u03B5\u03C1\u03C9\u03BD...",L"\u0625\u0638\u0647\u0627\u0631 \u0627\u0644\u0645\u0632\u064A\u062F..."}},
    {2037,{L"Close",L"Chiudi",L"Schlie\u00DFen",L"Fermer",L"Cerrar",L"Fechar",L"Sluiten",L"Zamknij",L"\u0417\u0430\u043A\u0440\u044B\u0442\u044C",L"\u5173\u95ED",L"\u9589\u3058\u308B",L"\uB2EB\uAE30",L"Kapat",L"Zav\u0159\u00EDt",L"Bez\u00E1r\u00E1s",L"\u00CEnchidere",L"St\u00E4ng",L"\u0417\u0430\u043A\u0440\u0438\u0442\u0438",L"\u039A\u03BB\u03B5\u03AF\u03C3\u03B9\u03BC\u03BF",L"\u0625\u063A\u0644\u0627\u0642"}},
    {2038,{L"Text Services and Input Languages",L"Tastiere e lingue",L"Textdienste und Eingabesprachen",L"Services de texte et langues d\u2019entr\u00E9e",L"Servicios de texto e idiomas de entrada",L"Servi\u00E7os de texto e idiomas de entrada",L"Tekstservices en invoertalen",L"Us\u0142ugi tekstowe i j\u0119zyki wprowadzania",L"\u0422\u0435\u043A\u0441\u0442\u043E\u0432\u044B\u0435 \u0441\u043B\u0443\u0436\u0431\u044B \u0438 \u044F\u0437\u044B\u043A\u0438 \u0432\u0432\u043E\u0434\u0430",L"\u6587\u672C\u670D\u52A1\u548C\u8F93\u5165\u8BED\u8A00",L"\u30C6\u30AD\u30B9\u30C8 \u30B5\u30FC\u30D3\u30B9\u3068\u5165\u529B\u8A00\u8A9E",L"\uD14D\uC2A4\uD2B8 \uC11C\uBE44\uC2A4 \uBC0F \uC785\uB825 \uC5B8\uC5B4",L"Metin hizmetleri ve giri\u015F dilleri",L"Textov\u00E9 slu\u017Eby a vstupn\u00ED jazyky",L"Sz\u00F6veges szolg\u00E1ltat\u00E1sok \u00E9s beviteli nyelvek",L"Servicii de text \u0219i limbi de intrare",L"Texttj\u00E4nster och inmatningsspr\u00E5k",L"\u0422\u0435\u043A\u0441\u0442\u043E\u0432\u0456 \u0441\u043B\u0443\u0436\u0431\u0438 \u0442\u0430 \u043C\u043E\u0432\u0438 \u0432\u0432\u0435\u0434\u0435\u043D\u043D\u044F",L"\u03A5\u03C0\u03B7\u03C1\u03B5\u03C3\u03AF\u03B5\u03C2 \u03BA\u03B5\u03B9\u03BC\u03AD\u03BD\u03BF\u03C5 \u03BA\u03B1\u03B9 \u03B3\u03BB\u03CE\u03C3\u03C3\u03B5\u03C2 \u03B5\u03B9\u03C3\u03CC\u03B4\u03BF\u03C5",L"\u062E\u062F\u0645\u0627\u062A \u0627\u0644\u0646\u0635\u0648\u0635 \u0648\u0644\u063A\u0627\u062A \u0627\u0644\u0625\u062F\u062E\u0627\u0644"}},
    {2039,{L"The property setting for %s is not available.",L"L'impostazione della propriet\u00E0 per %s non \u00E8 disponibile.",L"Die Eigenschaftseinstellung f\u00FCr %s ist nicht verf\u00FCgbar.",L"Le param\u00E8tre de propri\u00E9t\u00E9 de %s n'est pas disponible.",L"La configuraci\u00F3n de la propiedad de %s no est\u00E1 disponible.",L"A configura\u00E7\u00E3o da propriedade para %s n\u00E3o est\u00E1 dispon\u00EDvel.",L"De eigenschapsinstelling voor %s is niet beschikbaar.",L"Ustawienie w\u0142a\u015Bciwo\u015Bci dla %s jest niedost\u0119pne.",L"\u041F\u0430\u0440\u0430\u043C\u0435\u0442\u0440 \u0441\u0432\u043E\u0439\u0441\u0442\u0432\u0430 \u0434\u043B\u044F %s \u043D\u0435\u0434\u043E\u0441\u0442\u0443\u043F\u0435\u043D.",L"%s \u7684\u5C5E\u6027\u8BBE\u7F6E\u4E0D\u53EF\u7528\u3002",L"%s \u306E\u30D7\u30ED\u30D1\u30C6\u30A3\u8A2D\u5B9A\u306F\u4F7F\u7528\u3067\u304D\u307E\u305B\u3093\u3002",L"%s\uC758 \uC18D\uC131 \uC124\uC815\uC744 \uC0AC\uC6A9\uD560 \uC218 \uC5C6\uC2B5\uB2C8\uB2E4.",L"%s i\u00E7in \u00F6zellik ayar\u0131 kullan\u0131lam\u0131yor.",L"Nastaven\u00ED vlastnosti pro %s nen\u00ED dostupn\u00E9.",L"A(z) %s tulajdons\u00E1gbe\u00E1ll\u00EDt\u00E1sa nem \u00E9rhet\u0151 el.",L"Setarea propriet\u0103\u021Bii pentru %s nu este disponibil\u0103.",L"Egenskapsinst\u00E4llningen f\u00F6r %s \u00E4r inte tillg\u00E4nglig.",L"\u041F\u0430\u0440\u0430\u043C\u0435\u0442\u0440 \u0432\u043B\u0430\u0441\u0442\u0438\u0432\u043E\u0441\u0442\u0456 \u0434\u043B\u044F %s \u043D\u0435\u0434\u043E\u0441\u0442\u0443\u043F\u043D\u0438\u0439.",L"\u0397 \u03C1\u03CD\u03B8\u03BC\u03B9\u03C3\u03B7 \u03B9\u03B4\u03B9\u03CC\u03C4\u03B7\u03C4\u03B1\u03C2 \u03B3\u03B9\u03B1 \u03C4\u03BF %s \u03B4\u03B5\u03BD \u03B5\u03AF\u03BD\u03B1\u03B9 \u03B4\u03B9\u03B1\u03B8\u03AD\u03C3\u03B9\u03BC\u03B7.",L"\u0625\u0639\u062F\u0627\u062F \u0627\u0644\u062E\u0627\u0635\u064A\u0629 \u0644\u0640 %s \u063A\u064A\u0631 \u0645\u062A\u0648\u0641\u0631."}},
    {2040,{L"Ctrl",L"Ctrl",L"Strg",L"Ctrl",L"Ctrl",L"Ctrl",L"Ctrl",L"Ctrl",L"Ctrl",L"Ctrl",L"Ctrl",L"Ctrl",L"Ctrl",L"Ctrl",L"Ctrl",L"Ctrl",L"Ctrl",L"Ctrl",L"Ctrl",L"Ctrl"}},
    {2041,{L"Ctrl + Shift",L"Ctrl + Maiusc",L"Strg + Umschalt",L"Ctrl + Maj",L"Ctrl + May\u00FAs",L"Ctrl + Shift",L"Ctrl + Shift",L"Ctrl + Shift",L"CTRL + SHIFT",L"Ctrl + Shift",L"Ctrl + Shift",L"Ctrl + Shift",L"Ctrl + Shift",L"Ctrl + Shift",L"Ctrl + Shift",L"Ctrl + Shift",L"Ctrl + Skift",L"CTRL + SHIFT",L"Ctrl + Shift",L"Ctrl + Shift"}},
    {2042,{L"Left Alt + Shift",L"Alt sinistro + Maiusc",L"Alt links + Umschalt",L"Alt gauche + Maj",L"Alt izquierdo + May\u00FAs",L"Alt esquerdo + Shift",L"Linker Alt + Shift",L"Lewy Alt + Shift",L"\u041B\u0435\u0432\u044B\u0439 ALT + SHIFT",L"\u5DE6 Alt + Shift",L"\u5DE6 Alt + Shift",L"\uC67C\uCABD Alt + Shift",L"Sol Alt + Shift",L"Lev\u00FD Alt + Shift",L"Bal Alt + Shift",L"Alt st\u00E2nga + Shift",L"V\u00E4nster Alt + Skift",L"\u041B\u0456\u0432\u0438\u0439 ALT + SHIFT",L"\u0391\u03C1\u03B9\u03C3\u03C4\u03B5\u03C1\u03CC Alt + Shift",L"Alt \u064A\u0633\u0627\u0631 + Shift"}},
    {2043,{L"Other Languages",L"Altre lingue",L"Andere Sprachen",L"Autres langues",L"Otros idiomas",L"Outros idiomas",L"Andere talen",L"Inne j\u0119zyki",L"\u0414\u0440\u0443\u0433\u0438\u0435 \u044F\u0437\u044B\u043A\u0438",L"\u5176\u4ED6\u8BED\u8A00",L"\u305D\u306E\u4ED6\u306E\u8A00\u8A9E",L"\uAE30\uD0C0 \uC5B8\uC5B4",L"Di\u011Fer diller",L"Dal\u0161\u00ED jazyky",L"Egy\u00E9b nyelvek",L"Alte limbi",L"Andra spr\u00E5k",L"\u0406\u043D\u0448\u0456 \u043C\u043E\u0432\u0438",L"\u0386\u03BB\u03BB\u03B5\u03C2 \u03B3\u03BB\u03CE\u03C3\u03C3\u03B5\u03C2",L"\u0644\u063A\u0627\u062A \u0623\u062E\u0631\u0649"}},
    {2200,{L"(None)",L"(Nessuno)",L"(Keine)",L"(Aucune)",L"(Ninguno)",L"(Nenhum)",L"(Geen)",L"(Brak)",L"(\u041D\u0435\u0442)",L"(\u65E0)",L"(\u306A\u3057)",L"(\uC5C6\uC74C)",L"(Yok)",L"(\u017D\u00E1dn\u00E9)",L"(Nincs)",L"(F\u0103r\u0103)",L"(Ingen)",L"(\u041D\u0435\u043C\u0430\u0454)",L"(\u039A\u03B1\u03BC\u03AF\u03B1)",L"(\u0628\u0644\u0627)"}},
    {2201,{L"Space",L"Spazio",L"Leertaste",L"Espace",L"Espacio",L"Espa\u00E7o",L"Spatie",L"Spacja",L"\u041F\u0440\u043E\u0431\u0435\u043B",L"\u7A7A\u683C\u952E",L"Space",L"Space",L"Bo\u015Fluk",L"Mezern\u00EDk",L"Sz\u00F3k\u00F6z",L"Spa\u021Biu",L"Blanksteg",L"\u041F\u0440\u043E\u0431\u0456\u043B",L"\u0394\u03B9\u03AC\u03C3\u03C4\u03B7\u03BC\u03B1",L"\u0645\u0633\u0627\u0641\u0629"}},
    {2202,{L"Page_Up",L"Pagina su",L"Bild auf",L"Page pr\u00E9c\u00E9dente",L"Re P\u00E1g",L"Page Up",L"Page Up",L"Page Up",L"Page Up",L"Page Up",L"Page Up",L"Page Up",L"Page Up",L"Page Up",L"Page Up",L"Page Up",L"Page Up",L"Page Up",L"Page Up",L"Page Up"}},
    {2203,{L"Page_Down",L"Pagina gi\u00F9",L"Bild ab",L"Page suivante",L"Av P\u00E1g",L"Page Down",L"Page Down",L"Page Down",L"Page Down",L"Page Down",L"Page Down",L"Page Down",L"Page Down",L"Page Down",L"Page Down",L"Page Down",L"Page Down",L"Page Down",L"Page Down",L"Page Down"}},
    {2204,{L"End",L"Fine",L"Ende",L"Fin",L"Fin",L"End",L"End",L"End",L"End",L"End",L"End",L"End",L"End",L"End",L"End",L"End",L"End",L"End",L"End",L"End"}},
    {2205,{L"Home",L"Home",L"Pos1",L"D\u00E9but",L"Inicio",L"Home",L"Home",L"Home",L"Home",L"Home",L"Home",L"Home",L"Home",L"Home",L"Home",L"Home",L"Home",L"Home",L"Home",L"Home"}},
    {2255,{L"(None)",L"(Nessuno)",L"(Keine)",L"(Aucune)",L"(Ninguno)",L"(Nenhum)",L"(Geen)",L"(Brak)",L"(\u041D\u0435\u0442)",L"(\u65E0)",L"(\u306A\u3057)",L"(\uC5C6\uC74C)",L"(Yok)",L"(\u017D\u00E1dn\u00E9)",L"(Nincs)",L"(F\u0103r\u0103)",L"(Ingen)",L"(\u041D\u0435\u043C\u0430\u0454)",L"(\u039A\u03B1\u03BC\u03AF\u03B1)",L"(\u0628\u0644\u0627)"}},
    {2267,{L"Grave Accent",L"Accento grave",L"Gravis",L"Accent grave",L"Acento grave",L"Acento grave",L"Accent grave",L"Akcent gravis",L"\u0421\u0438\u043C\u0432\u043E\u043B \u0443\u0434\u0430\u0440\u0435\u043D\u0438\u044F",L"\u91CD\u97F3\u7B26",L"\u30B0\u30EC\u30FC\u30D6 \u30A2\u30AF\u30BB\u30F3\u30C8",L"\uC5B5\uC74C \uC545\uC13C\uD2B8",L"Vurgu i\u015Fareti",L"P\u0159\u00EDzvuk",L"Visszav\u00E1g\u00F3jel",L"Accent grav",L"Grav accent",L"\u0417\u043D\u0430\u043A \u043D\u0430\u0433\u043E\u043B\u043E\u0441\u0443",L"\u0392\u03B1\u03C1\u03B5\u03AF\u03B1",L"\u0639\u0644\u0627\u0645\u0629 \u0627\u0644\u0646\u0628\u0631"}},
    {3000,{L"(Default)",L"(predefinito)",L"(Standard)",L"(Par d\u00E9faut)",L"(Predeterminado)",L"(Padr\u00E3o)",L"(Standaard)",L"(Domy\u015Blnie)",L"(\u041F\u043E \u0443\u043C\u043E\u043B\u0447\u0430\u043D\u0438\u044E)",L"(\u9ED8\u8BA4)",L"(\u65E2\u5B9A)",L"(\uAE30\uBCF8\uAC12)",L"(Varsay\u0131lan)",L"(V\u00FDchoz\u00ED)",L"(Alap\u00E9rtelmezett)",L"(Implicit)",L"(Standard)",L"(\u0417\u0430 \u0437\u0430\u043C\u043E\u0432\u0447\u0443\u0432\u0430\u043D\u043D\u044F\u043C)",L"(\u03A0\u03C1\u03BF\u03B5\u03C0\u03B9\u03BB\u03BF\u03B3\u03AE)",L"(\u0627\u0641\u062A\u0631\u0627\u0636\u064A)"}},
};

const wchar_t* InputText(UINT id) {
    if (g_lang < LangEN || g_lang >= LangCount) return nullptr;
    for (const auto& item : kInputText) if (item.id == id) return item.text[g_lang];
    return nullptr;
}

// ------- Text Services dialog-template phrases (47, shared by all pages) -------
static const wchar_t* const kInpDlgTr_IT[47] = {
    L"OK",
    L"Annulla",
    L"Aggiungi lingua di input",
    L"Selezionare la lingua da aggiungere mediante le caselle di controllo seguenti.",
    L"&Anteprima...",
    L"Barra della lingua",
    L"&Mobile sul desktop",
    L"Ancorata alla &barra delle applicazioni",
    L"&Nascosta",
    L"Mostra la barra della lingua come trasparente quando ina&ttiva",
    L"Mostra icone &aggiuntive della barra della lingua nella barra delle applicazioni",
    L"Mostra &etichette di testo sulla barra della lingua",
    L"Impostazioni avanzate tasti",
    L"Per disattivare Bloc Maiusc",
    L"Premere il tasto B&LOC MAIUSC",
    L"Premere il tasto MAI&USC",
    L"Tasti di scelta rapida per le lingue di input",
    L"Azione",
    L"&Sequenza di tasti",
    L"&Cambia sequenza di tasti...",
    L"Cambia sequenza di tasti",
    L"Cambia lingua di input",
    L"&Non assegnata",
    L"&Ctrl + Maiusc",
    L"&Alt sinistro + Maiusc",
    L"Accento &grave (`)",
    L"Cambia layout di tastiera",
    L"N&on assegnata",
    L"C&trl + Maiusc",
    L"Alt sinist&ro + Maiusc",
    L"Accento grav&e (`)",
    L"&Attiva sequenza di tasti",
    L"&Tasto:",
    L"Generale",
    L"&Lingua di input predefinita",
    L"Selezionare una delle lingue di input installate da usare come predefinita per tutti i campi di input.",
    L"&Servizi installati",
    L"Selezionare i servizi desiderati per ogni lingua di input visualizzata nell'elenco. Usare i pulsanti Aggiungi e Rimuovi per modificare l'elenco.",
    L"A&ggiungi...",
    L"&Rimuovi",
    L"&Propriet\u00E0...",
    L"Sposta s&u",
    L"Sposta g&i\u00F9",
    L"Anteprima layout di tastiera",
    L"Nome layout:",
    L"&Cambia icona...",
    L"&Non visualizzare pi\u00F9 questo messaggio.",
};

static const wchar_t* const kInpDlgTr_DE[47] = {
    L"OK",
    L"Abbrechen",
    L"Eingabesprache hinzuf\u00FCgen",
    L"W\u00E4hlen Sie die hinzuzuf\u00FCgende Sprache anhand der unten angezeigten Kontrollk\u00E4stchen aus.",
    L"&Vorschau...",
    L"Sprachleiste",
    L"&Frei auf dem Desktop",
    L"In der &Taskleiste angedockt",
    L"&Ausgeblendet",
    L"Sprachleiste bei I&naktivit\u00E4t als transparent anzeigen",
    L"&Zus\u00E4tzliche Sprachleisten-Symbole in der Taskleiste anzeigen",
    L"Te&xtbezeichnungen auf der Sprachleiste anzeigen",
    L"Erweiterte Tasteneinstellungen",
    L"Zum Deaktivieren der Feststelltaste",
    L"Feststelltaste &dr\u00FCcken",
    L"Umschalttaste d&r\u00FCcken",
    L"Tastenkombinationen f\u00FCr Eingabesprachen",
    L"Aktion",
    L"&Tastenfolge",
    L"Tastenfolge &\u00E4ndern...",
    L"Tastenfolge \u00E4ndern",
    L"Eingabesprache wechseln",
    L"&Nicht zugewiesen",
    L"&STRG + UMSCHALT",
    L"&Linke ALT + UMSCHALT",
    L"&Gravis (`)",
    L"Tastaturlayout wechseln",
    L"N&icht zugewiesen",
    L"STRG + U&MSCHALT",
    L"Linke AL&T + UMSCHALT",
    L"Gr&avis (`)",
    L"Tastenfolge &aktivieren",
    L"&Taste:",
    L"Allgemein",
    L"&Standardeingabesprache",
    L"W\u00E4hlen Sie eine der installierten Eingabesprachen als Standardsprache f\u00FCr alle Eingabefelder aus.",
    L"&Installierte Dienste",
    L"W\u00E4hlen Sie die gew\u00FCnschten Dienste f\u00FCr jede in der Liste angezeigte Eingabesprache aus. Verwenden Sie zum \u00C4ndern der Liste die Schaltfl\u00E4chen Hinzuf\u00FCgen und Entfernen.",
    L"&Hinzuf\u00FCgen...",
    L"Entfe&rnen",
    L"&Eigenschaften...",
    L"Nach &oben",
    L"Nach &unten",
    L"Vorschau des Tastaturlayouts",
    L"Layoutname:",
    L"S&ymbol \u00E4ndern...",
    L"Diese Meldung &nicht mehr anzeigen.",
};

static const wchar_t* const kInpDlgTr_FR[47] = {
    L"OK",
    L"Annuler",
    L"Ajouter une langue d'entr\u00E9e",
    L"S\u00E9lectionnez la langue \u00E0 ajouter \u00E0 l'aide des cases \u00E0 cocher ci-dessous.",
    L"&Aper\u00E7u...",
    L"Barre de langue",
    L"&Flottante sur le Bureau",
    L"A&ncr\u00E9e dans la barre des t\u00E2ches",
    L"&Masqu\u00E9e",
    L"Afficher la barre de langue comme transparente lorsqu'elle est inacti&ve",
    L"Afficher les &ic\u00F4nes suppl\u00E9mentaires de la barre de langue dans la barre des t\u00E2ches",
    L"Afficher les &\u00E9tiquettes de texte sur la barre de langue",
    L"Param\u00E8tres de touches avanc\u00E9s",
    L"Pour d\u00E9sactiver Verr. Maj",
    L"Appuyer sur la touche VERR. &MAJ",
    L"Appuyer sur la touche MA&J",
    L"Touches d'acc\u00E8s rapide pour les langues d'entr\u00E9e",
    L"Action",
    L"&S\u00E9quence de touches",
    L"Mod&ifier la s\u00E9quence de touches...",
    L"Modifier la s\u00E9quence de touches",
    L"Changer de langue d'entr\u00E9e",
    L"&Non affect\u00E9e",
    L"&Ctrl + Maj",
    L"&Alt gauche + Maj",
    L"Accent &grave (`)",
    L"Changer de disposition de clavier",
    L"N&on affect\u00E9e",
    L"C&trl + Maj",
    L"Alt gauche + Ma&j",
    L"Accent grav&e (`)",
    L"&Activer la s\u00E9quence de touches",
    L"&Touche :",
    L"G\u00E9n\u00E9ral",
    L"Langue d'entr\u00E9e par &d\u00E9faut",
    L"S\u00E9lectionnez l'une des langues d'entr\u00E9e install\u00E9es \u00E0 utiliser par d\u00E9faut pour tous les champs de saisie.",
    L"&Services install\u00E9s",
    L"S\u00E9lectionnez les services souhait\u00E9s pour chaque langue d'entr\u00E9e affich\u00E9e dans la liste. Utilisez les boutons Ajouter et Supprimer pour modifier cette liste.",
    L"A&jouter...",
    L"S&upprimer",
    L"&Propri\u00E9t\u00E9s...",
    L"&Monter",
    L"D&escendre",
    L"Aper\u00E7u de la disposition du clavier",
    L"Nom de la disposition :",
    L"&Modifier l'ic\u00F4ne...",
    L"Ne plus afficher ce &message.",
};

static const wchar_t* const kInpDlgTr_ES[47] = {
    L"Aceptar",
    L"Cancelar",
    L"Agregar idioma de entrada",
    L"Seleccione el idioma que desee agregar mediante las casillas siguientes.",
    L"Vista &previa...",
    L"Barra de idioma",
    L"&Flotante en el escritorio",
    L"Acoplada en la &barra de tareas",
    L"&Oculta",
    L"Mostrar la barra de idioma como transparente cuando est\u00E9 i&nactiva",
    L"Mostrar iconos &adicionales de la barra de idioma en la barra de tareas",
    L"Mostrar e&tiquetas de texto en la barra de idioma",
    L"Configuraci\u00F3n avanzada de teclas",
    L"Para desactivar Bloq May\u00FAs",
    L"Presionar la tecla BLOQ &MAY\u00DAS",
    L"Presionar la tecla MA&Y\u00DAS",
    L"Teclas de m\u00E9todo abreviado para los idiomas de entrada",
    L"Acci\u00F3n",
    L"&Secuencia de teclas",
    L"&Cambiar secuencia de teclas...",
    L"Cambiar secuencia de teclas",
    L"Cambiar idioma de entrada",
    L"&Sin asignar",
    L"&Ctrl + May\u00FAs",
    L"&Alt izquierdo + May\u00FAs",
    L"Acento g&rave (`)",
    L"Cambiar dise\u00F1o de teclado",
    L"S&in asignar",
    L"C&trl + May\u00FAs",
    L"Alt izquierd&o + May\u00FAs",
    L"Acento &grave (`)",
    L"&Habilitar secuencia de teclas",
    L"&Tecla:",
    L"General",
    L"Idioma de entrada pre&determinado",
    L"Seleccione uno de los idiomas de entrada instalados que se usar\u00E1 como predeterminado para todos los campos de entrada.",
    L"&Servicios instalados",
    L"Seleccione los servicios que desee para cada idioma de entrada mostrado en la lista. Use los botones Agregar y Quitar para modificar esta lista.",
    L"&Agregar...",
    L"&Quitar",
    L"&Propiedades...",
    L"Su&bir",
    L"Ba&jar",
    L"Vista previa del dise\u00F1o de teclado",
    L"Nombre del dise\u00F1o:",
    L"&Cambiar icono...",
    L"&No volver a mostrar este mensaje.",
};

static const wchar_t* const kInpDlgTr_PT[47] = {
    L"OK",
    L"Cancelar",
    L"Adicionar idioma de entrada",
    L"Selecione o idioma a ser adicionado usando as caixas de sele\u00E7\u00E3o abaixo.",
    L"&Visualizar...",
    L"Barra de idiomas",
    L"&Flutuante na \u00E1rea de trabalho",
    L"Encaixada na &barra de tarefas",
    L"&Oculta",
    L"Mostrar a barra de idiomas como transparente quando estiver i&nativa",
    L"Mostrar \u00EDcones &adicionais da barra de idiomas na barra de tarefas",
    L"Mostrar r\u00F3&tulos de texto na barra de idiomas",
    L"Defini\u00E7\u00F5es avan\u00E7adas de teclas",
    L"Para desativar Caps Lock",
    L"Pressionar a tecla CAPS &LOCK",
    L"Pressionar a tecla SHI&FT",
    L"Teclas de atalho para idiomas de entrada",
    L"A\u00E7\u00E3o",
    L"&Sequ\u00EAncia de teclas",
    L"Al&terar sequ\u00EAncia de teclas...",
    L"Alterar sequ\u00EAncia de teclas",
    L"Alternar idioma de entrada",
    L"&N\u00E3o atribu\u00EDdo",
    L"&Ctrl + Shift",
    L"&Alt esquerdo + Shift",
    L"Acento &grave (`)",
    L"Alternar layout de teclado",
    L"N&\u00E3o atribu\u00EDdo",
    L"C&trl + Shift",
    L"Alt esquerd&o + Shift",
    L"Acento gra&ve (`)",
    L"&Habilitar sequ\u00EAncia de teclas",
    L"&Tecla:",
    L"Geral",
    L"&Idioma de entrada padr\u00E3o",
    L"Selecione um dos idiomas de entrada instalados para ser usado como padr\u00E3o para todos os campos de entrada.",
    L"&Servi\u00E7os instalados",
    L"Selecione os servi\u00E7os desejados para cada idioma de entrada mostrado na lista. Use os bot\u00F5es Adicionar e Remover para modificar essa lista.",
    L"A&dicionar...",
    L"&Remover",
    L"&Propriedades...",
    L"Mover para &cima",
    L"Mover para &baixo",
    L"Visualiza\u00E7\u00E3o do layout do teclado",
    L"Nome do layout:",
    L"Alterar &\u00EDcone...",
    L"&N\u00E3o mostrar esta mensagem novamente.",
};

static const wchar_t* const kInpDlgTr_NL[47] = {
    L"OK",
    L"Annuleren",
    L"Invoertaal toevoegen",
    L"Selecteer de taal die u wilt toevoegen met behulp van de onderstaande selectievakjes.",
    L"&Voorbeeld...",
    L"Taalbalk",
    L"&Zwevend op het bureaublad",
    L"Vastgemaakt aan de &taakbalk",
    L"&Verborgen",
    L"De taalbalk als transparant weergeven wanneer i&nactief",
    L"Aanvu&llende taalbalkpictogrammen in de taakbalk weergeven",
    L"Te&kstlabels op de taalbalk weergeven",
    L"Geavanceerde toetsinstellingen",
    L"Voor het uitschakelen van Caps Lock",
    L"De CAPS &LOCK-toets indrukken",
    L"De SHI&FT-toets indrukken",
    L"Sneltoetsen voor invoertalen",
    L"Actie",
    L"&Toetsvolgorde",
    L"Toetsvolgorde &wijzigen...",
    L"Toetsvolgorde wijzigen",
    L"Invoertaal wijzigen",
    L"&Niet toegewezen",
    L"&Ctrl + Shift",
    L"&Linker Alt + Shift",
    L"&Accent grave (`)",
    L"Toetsenbordindeling wijzigen",
    L"N&iet toegewezen",
    L"C&trl + Shift",
    L"Lin&ker Alt + Shift",
    L"Accent &grave (`)",
    L"Toetsvolgorde &inschakelen",
    L"&Toets:",
    L"Algemeen",
    L"Stan&daard invoertaal",
    L"Selecteer een van de ge\u00EFnstalleerde invoertalen om als standaard voor alle invoervelden te gebruiken.",
    L"&Ge\u00EFnstalleerde services",
    L"Selecteer de services die u wilt voor elke invoertaal in de lijst. Gebruik de knoppen Toevoegen en Verwijderen om deze lijst te wijzigen.",
    L"&Toevoegen...",
    L"&Verwijderen",
    L"&Eigenschappen...",
    L"Naar b&oven",
    L"Naar be&neden",
    L"Voorbeeld van toetsenbordindeling",
    L"Naam indeling:",
    L"Pictogram &wijzigen...",
    L"Dit bericht &niet meer tonen.",
};

static const wchar_t* const kInpDlgTr_PL[47] = {
    L"OK",
    L"Anuluj",
    L"Dodaj j\u0119zyk wprowadzania",
    L"Wybierz j\u0119zyk, kt\u00F3ry ma zosta\u0107 dodany, za pomoc\u0105 poni\u017Cszych p\u00F3l wyboru.",
    L"&Podgl\u0105d...",
    L"Pasek j\u0119zyka",
    L"&P\u0142ywaj\u0105cy na pulpicie",
    L"Zadokowany na pasku &zada\u0144",
    L"&Ukryty",
    L"Poka\u017C pasek j\u0119zyka jako przezroczysty, gdy jest &nieaktywny",
    L"Poka\u017C dod&atkowe ikony paska j\u0119zyka na pasku zada\u0144",
    L"Poka\u017C e&tykiety tekstowe na pasku j\u0119zyka",
    L"Zaawansowane ustawienia klawiszy",
    L"Aby wy\u0142\u0105czy\u0107 klawisz Caps Lock",
    L"Naci\u015Bnij klawisz CAPS &LOCK",
    L"Naci\u015Bnij klawisz SHI&FT",
    L"Klawisze skr\u00F3tu dla j\u0119zyk\u00F3w wprowadzania",
    L"Akcja",
    L"&Sekwencja klawiszy",
    L"&Zmie\u0144 sekwencj\u0119 klawiszy...",
    L"Zmie\u0144 sekwencj\u0119 klawiszy",
    L"Prze\u0142\u0105cz j\u0119zyk wprowadzania",
    L"&Nie przypisano",
    L"&Ctrl + Shift",
    L"&Lewy Alt + Shift",
    L"&Akcent gravis (`)",
    L"Prze\u0142\u0105cz uk\u0142ad klawiatury",
    L"N&ie przypisano",
    L"C&trl + Shift",
    L"Le&wy Alt + Shift",
    L"Akcent &gravis (`)",
    L"&W\u0142\u0105cz sekwencj\u0119 klawiszy",
    L"&Klawisz:",
    L"Og\u00F3lne",
    L"Domy\u015Blny &j\u0119zyk wprowadzania",
    L"Wybierz jeden z zainstalowanych j\u0119zyk\u00F3w wprowadzania, kt\u00F3ry ma by\u0107 u\u017Cywany jako domy\u015Blny dla wszystkich p\u00F3l wprowadzania.",
    L"&Zainstalowane us\u0142ugi",
    L"Wybierz us\u0142ugi dla ka\u017Cdego j\u0119zyka wprowadzania widocznego na li\u015Bcie. U\u017Cyj przycisk\u00F3w Dodaj i Usu\u0144, aby zmieni\u0107 t\u0119 list\u0119.",
    L"D&odaj...",
    L"&Usu\u0144",
    L"&W\u0142a\u015Bciwo\u015Bci...",
    L"Przenie\u015B w &g\u00F3r\u0119",
    L"Przenie\u015B w &d\u00F3\u0142",
    L"Podgl\u0105d uk\u0142adu klawiatury",
    L"Nazwa uk\u0142adu:",
    L"Zmie\u0144 &ikon\u0119...",
    L"&Nie pokazuj wi\u0119cej tej wiadomo\u015Bci.",
};

static const wchar_t* const kInpDlgTr_RU[47] = {
    L"OK",
    L"\u041E\u0442\u043C\u0435\u043D\u0430",
    L"\u0414\u043E\u0431\u0430\u0432\u0438\u0442\u044C \u044F\u0437\u044B\u043A \u0432\u0432\u043E\u0434\u0430",
    L"\u0412\u044B\u0431\u0435\u0440\u0438\u0442\u0435 \u0434\u043E\u0431\u0430\u0432\u043B\u044F\u0435\u043C\u044B\u0439 \u044F\u0437\u044B\u043A \u0441 \u043F\u043E\u043C\u043E\u0449\u044C\u044E \u0444\u043B\u0430\u0436\u043A\u043E\u0432 \u043D\u0438\u0436\u0435.",
    L"&\u041F\u0440\u0435\u0434\u0432\u0430\u0440\u0438\u0442\u0435\u043B\u044C\u043D\u044B\u0439 \u043F\u0440\u043E\u0441\u043C\u043E\u0442\u0440...",
    L"\u042F\u0437\u044B\u043A\u043E\u0432\u0430\u044F \u043F\u0430\u043D\u0435\u043B\u044C",
    L"&\u041F\u043B\u0430\u0432\u0430\u044E\u0449\u0430\u044F \u043D\u0430 \u0440\u0430\u0431\u043E\u0447\u0435\u043C \u0441\u0442\u043E\u043B\u0435",
    L"\u0417\u0430\u043A\u0440\u0435\u043F\u043B\u0435\u043D\u0430 \u043D\u0430 \u043F\u0430\u043D\u0435\u043B\u0438 &\u0437\u0430\u0434\u0430\u0447",
    L"&\u0421\u043A\u0440\u044B\u0442\u0430\u044F",
    L"\u041E\u0442\u043E\u0431\u0440\u0430\u0436\u0430\u0442\u044C \u044F\u0437\u044B\u043A\u043E\u0432\u0443\u044E \u043F\u0430\u043D\u0435\u043B\u044C \u043A\u0430\u043A \u043F\u0440\u043E\u0437\u0440\u0430\u0447\u043D\u0443\u044E, \u043A\u043E\u0433\u0434\u0430 \u043E\u043D\u0430 &\u043D\u0435\u0430\u043A\u0442\u0438\u0432\u043D\u0430",
    L"\u041E\u0442\u043E\u0431\u0440\u0430\u0436\u0430\u0442\u044C \u0434\u043E\u043F\u043E&\u043B\u043D\u0438\u0442\u0435\u043B\u044C\u043D\u044B\u0435 \u0437\u043D\u0430\u0447\u043A\u0438 \u044F\u0437\u044B\u043A\u043E\u0432\u043E\u0439 \u043F\u0430\u043D\u0435\u043B\u0438 \u043D\u0430 \u043F\u0430\u043D\u0435\u043B\u0438 \u0437\u0430\u0434\u0430\u0447",
    L"\u041E\u0442\u043E\u0431\u0440\u0430\u0436\u0430\u0442\u044C \u0442\u0435&\u043A\u0441\u0442\u043E\u0432\u044B\u0435 \u043F\u043E\u0434\u043F\u0438\u0441\u0438 \u043D\u0430 \u044F\u0437\u044B\u043A\u043E\u0432\u043E\u0439 \u043F\u0430\u043D\u0435\u043B\u0438",
    L"\u0414\u043E\u043F\u043E\u043B\u043D\u0438\u0442\u0435\u043B\u044C\u043D\u044B\u0435 \u043F\u0430\u0440\u0430\u043C\u0435\u0442\u0440\u044B \u043A\u043B\u0430\u0432\u0438\u0448",
    L"\u0414\u043B\u044F \u0432\u044B\u043A\u043B\u044E\u0447\u0435\u043D\u0438\u044F \u043A\u043B\u0430\u0432\u0438\u0448\u0438 CAPS LOCK",
    L"\u041D\u0430\u0436\u0438\u043C\u0430\u0442\u044C \u043A\u043B\u0430\u0432\u0438\u0448\u0443 CAPS &LOCK",
    L"\u041D\u0430\u0436\u0438\u043C\u0430\u0442\u044C \u043A\u043B\u0430\u0432\u0438\u0448\u0443 SHI&FT",
    L"\u0421\u043E\u0447\u0435\u0442\u0430\u043D\u0438\u044F \u043A\u043B\u0430\u0432\u0438\u0448 \u0434\u043B\u044F \u044F\u0437\u044B\u043A\u043E\u0432 \u0432\u0432\u043E\u0434\u0430",
    L"\u0414\u0435\u0439\u0441\u0442\u0432\u0438\u0435",
    L"&\u0421\u043E\u0447\u0435\u0442\u0430\u043D\u0438\u0435 \u043A\u043B\u0430\u0432\u0438\u0448",
    L"&\u0418\u0437\u043C\u0435\u043D\u0438\u0442\u044C \u0441\u043E\u0447\u0435\u0442\u0430\u043D\u0438\u0435 \u043A\u043B\u0430\u0432\u0438\u0448...",
    L"\u0418\u0437\u043C\u0435\u043D\u0438\u0442\u044C \u0441\u043E\u0447\u0435\u0442\u0430\u043D\u0438\u0435 \u043A\u043B\u0430\u0432\u0438\u0448",
    L"\u0421\u043C\u0435\u043D\u0438\u0442\u044C \u044F\u0437\u044B\u043A \u0432\u0432\u043E\u0434\u0430",
    L"&\u041D\u0435 \u043D\u0430\u0437\u043D\u0430\u0447\u0435\u043D\u043E",
    L"&CTRL + SHIFT",
    L"&\u041B\u0435\u0432\u044B\u0439 ALT + SHIFT",
    L"&\u0421\u0438\u043C\u0432\u043E\u043B \u0443\u0434\u0430\u0440\u0435\u043D\u0438\u044F (`)",
    L"\u0421\u043C\u0435\u043D\u0438\u0442\u044C \u0440\u0430\u0441\u043A\u043B\u0430\u0434\u043A\u0443 \u043A\u043B\u0430\u0432\u0438\u0430\u0442\u0443\u0440\u044B",
    L"\u041D&\u0435 \u043D\u0430\u0437\u043D\u0430\u0447\u0435\u043D\u043E",
    L"CTRL + SHIF&T",
    L"\u041B\u0435\u0432\u044B&\u0439 ALT + SHIFT",
    L"\u0421\u0438\u043C\u0432\u043E\u043B &\u0443\u0434\u0430\u0440\u0435\u043D\u0438\u044F (`)",
    L"&\u0412\u043A\u043B\u044E\u0447\u0438\u0442\u044C \u0441\u043E\u0447\u0435\u0442\u0430\u043D\u0438\u0435 \u043A\u043B\u0430\u0432\u0438\u0448",
    L"&\u041A\u043B\u0430\u0432\u0438\u0448\u0430:",
    L"\u041E\u0431\u0449\u0438\u0435",
    L"\u042F\u0437\u044B\u043A \u0432\u0432\u043E\u0434\u0430 \u043F\u043E \u0443\u043C\u043E\u043B&\u0447\u0430\u043D\u0438\u044E",
    L"\u0412\u044B\u0431\u0435\u0440\u0438\u0442\u0435 \u043E\u0434\u0438\u043D \u0438\u0437 \u0443\u0441\u0442\u0430\u043D\u043E\u0432\u043B\u0435\u043D\u043D\u044B\u0445 \u044F\u0437\u044B\u043A\u043E\u0432 \u0432\u0432\u043E\u0434\u0430, \u043A\u043E\u0442\u043E\u0440\u044B\u0439 \u0431\u0443\u0434\u0435\u0442 \u0438\u0441\u043F\u043E\u043B\u044C\u0437\u043E\u0432\u0430\u0442\u044C\u0441\u044F \u043F\u043E \u0443\u043C\u043E\u043B\u0447\u0430\u043D\u0438\u044E \u0432\u043E \u0432\u0441\u0435\u0445 \u043F\u043E\u043B\u044F\u0445 \u0432\u0432\u043E\u0434\u0430.",
    L"&\u0423\u0441\u0442\u0430\u043D\u043E\u0432\u043B\u0435\u043D\u043D\u044B\u0435 \u0441\u043B\u0443\u0436\u0431\u044B",
    L"\u0412\u044B\u0431\u0435\u0440\u0438\u0442\u0435 \u0441\u043B\u0443\u0436\u0431\u044B \u0434\u043B\u044F \u043A\u0430\u0436\u0434\u043E\u0433\u043E \u044F\u0437\u044B\u043A\u0430 \u0432\u0432\u043E\u0434\u0430 \u0432 \u0441\u043F\u0438\u0441\u043A\u0435. \u0414\u043B\u044F \u0438\u0437\u043C\u0435\u043D\u0435\u043D\u0438\u044F \u0441\u043F\u0438\u0441\u043A\u0430 \u0438\u0441\u043F\u043E\u043B\u044C\u0437\u0443\u0439\u0442\u0435 \u043A\u043D\u043E\u043F\u043A\u0438 \u00AB\u0414\u043E\u0431\u0430\u0432\u0438\u0442\u044C\u00BB \u0438 \u00AB\u0423\u0434\u0430\u043B\u0438\u0442\u044C\u00BB.",
    L"&\u0414\u043E\u0431\u0430\u0432\u0438\u0442\u044C...",
    L"\u0423\u0434\u0430&\u043B\u0438\u0442\u044C",
    L"&\u0421\u0432\u043E\u0439\u0441\u0442\u0432\u0430...",
    L"\u0412&\u0432\u0435\u0440\u0445",
    L"\u0412&\u043D\u0438\u0437",
    L"\u041F\u0440\u043E\u0441\u043C\u043E\u0442\u0440 \u0440\u0430\u0441\u043A\u043B\u0430\u0434\u043A\u0438 \u043A\u043B\u0430\u0432\u0438\u0430\u0442\u0443\u0440\u044B",
    L"\u041D\u0430\u0437\u0432\u0430\u043D\u0438\u0435 \u0440\u0430\u0441\u043A\u043B\u0430\u0434\u043A\u0438:",
    L"\u0421\u043C\u0435\u043D\u0438\u0442\u044C &\u0437\u043D\u0430\u0447\u043E\u043A...",
    L"&\u0411\u043E\u043B\u044C\u0448\u0435 \u043D\u0435 \u043F\u043E\u043A\u0430\u0437\u044B\u0432\u0430\u0442\u044C \u044D\u0442\u043E \u0441\u043E\u043E\u0431\u0449\u0435\u043D\u0438\u0435.",
};

static const wchar_t* const kInpDlgTr_ZH[47] = {
    L"\u786E\u5B9A",
    L"\u53D6\u6D88",
    L"\u6DFB\u52A0\u8F93\u5165\u8BED\u8A00",
    L"\u4F7F\u7528\u4E0B\u9762\u7684\u590D\u9009\u6846\u9009\u62E9\u8981\u6DFB\u52A0\u7684\u8BED\u8A00\u3002",
    L"\u9884\u89C8(&P)...",
    L"\u8BED\u8A00\u680F",
    L"\u60AC\u6D6E\u4E8E\u684C\u9762(&F)",
    L"\u505C\u9760\u4E8E\u4EFB\u52A1\u680F(&D)",
    L"\u9690\u85CF(&H)",
    L"\u8BED\u8A00\u680F\u5728\u975E\u6D3B\u52A8\u65F6\u663E\u793A\u4E3A\u900F\u660E(&N)",
    L"\u5728\u4EFB\u52A1\u680F\u4E2D\u663E\u793A\u5176\u4ED6\u8BED\u8A00\u680F\u56FE\u6807(&I)",
    L"\u5728\u8BED\u8A00\u680F\u4E0A\u663E\u793A\u6587\u672C\u6807\u7B7E(&E)",
    L"\u9AD8\u7EA7\u952E\u8BBE\u7F6E",
    L"\u82E5\u8981\u5173\u95ED Caps Lock",
    L"\u6309 CAPS &LOCK \u952E",
    L"\u6309 SHI&FT \u952E",
    L"\u8F93\u5165\u8BED\u8A00\u7684\u70ED\u952E",
    L"\u64CD\u4F5C",
    L"\u6309\u952E\u987A\u5E8F(&K)",
    L"\u66F4\u6539\u6309\u952E\u987A\u5E8F(&C)...",
    L"\u66F4\u6539\u6309\u952E\u987A\u5E8F",
    L"\u5207\u6362\u8F93\u5165\u8BED\u8A00",
    L"\u672A\u5206\u914D",
    L"Ctrl + Shift",
    L"\u5DE6 Alt + Shift",
    L"\u91CD\u97F3\u7B26(`)",
    L"\u5207\u6362\u952E\u76D8\u5E03\u5C40",
    L"\u672A\u5206\u914D",
    L"Ctrl + Shift",
    L"\u5DE6 Alt + Shift",
    L"\u91CD\u97F3\u7B26(`)",
    L"\u542F\u7528\u6309\u952E\u987A\u5E8F(&E)",
    L"\u952E(&K):",
    L"\u5E38\u89C4",
    L"\u9ED8\u8BA4\u8F93\u5165\u8BED\u8A00(&L)",
    L"\u9009\u62E9\u4E00\u79CD\u5DF2\u5B89\u88C5\u7684\u8F93\u5165\u8BED\u8A00\u4F5C\u4E3A\u6240\u6709\u8F93\u5165\u5B57\u6BB5\u7684\u9ED8\u8BA4\u8BED\u8A00\u3002",
    L"\u5DF2\u5B89\u88C5\u7684\u670D\u52A1(&I)",
    L"\u4E3A\u5217\u8868\u4E2D\u663E\u793A\u7684\u6BCF\u79CD\u8F93\u5165\u8BED\u8A00\u9009\u62E9\u6240\u9700\u7684\u670D\u52A1\u3002\u4F7F\u7528\u201C\u6DFB\u52A0\u201D\u548C\u201C\u5220\u9664\u201D\u6309\u94AE\u4FEE\u6539\u6B64\u5217\u8868\u3002",
    L"\u6DFB\u52A0(&D)...",
    L"\u5220\u9664(&R)",
    L"\u5C5E\u6027(&P)...",
    L"\u4E0A\u79FB(&U)",
    L"\u4E0B\u79FB(&O)",
    L"\u952E\u76D8\u5E03\u5C40\u9884\u89C8",
    L"\u5E03\u5C40\u540D\u79F0:",
    L"\u66F4\u6539\u56FE\u6807(&C)...",
    L"\u4E0D\u518D\u663E\u793A\u6B64\u6D88\u606F(&D)\u3002",
};

static const wchar_t* const kInpDlgTr_JA[47] = {
    L"OK",
    L"\u30AD\u30E3\u30F3\u30BB\u30EB",
    L"\u5165\u529B\u8A00\u8A9E\u306E\u8FFD\u52A0",
    L"\u4E0B\u306E\u30C1\u30A7\u30C3\u30AF \u30DC\u30C3\u30AF\u30B9\u3092\u4F7F\u7528\u3057\u3066\u3001\u8FFD\u52A0\u3059\u308B\u8A00\u8A9E\u3092\u9078\u629E\u3057\u3066\u304F\u3060\u3055\u3044\u3002",
    L"\u30D7\u30EC\u30D3\u30E5\u30FC(&P)...",
    L"\u8A00\u8A9E\u30D0\u30FC",
    L"\u30C7\u30B9\u30AF\u30C8\u30C3\u30D7\u306B\u30D5\u30ED\u30FC\u30C8\u8868\u793A(&F)",
    L"\u30BF\u30B9\u30AF \u30D0\u30FC\u306B\u30C9\u30C3\u30AD\u30F3\u30B0(&D)",
    L"\u975E\u8868\u793A(&H)",
    L"\u30A2\u30AF\u30C6\u30A3\u30D6\u3067\u306A\u3044\u3068\u304D\u306B\u8A00\u8A9E\u30D0\u30FC\u3092\u534A\u900F\u660E\u3067\u8868\u793A(&N)",
    L"\u30BF\u30B9\u30AF \u30D0\u30FC\u306B\u8FFD\u52A0\u306E\u8A00\u8A9E\u30D0\u30FC \u30A2\u30A4\u30B3\u30F3\u3092\u8868\u793A(&I)",
    L"\u8A00\u8A9E\u30D0\u30FC\u306B\u30C6\u30AD\u30B9\u30C8 \u30E9\u30D9\u30EB\u3092\u8868\u793A(&E)",
    L"\u30AD\u30FC\u306E\u8A73\u7D30\u8A2D\u5B9A",
    L"Caps Lock \u3092\u30AA\u30D5\u306B\u3059\u308B\u306B\u306F",
    L"CAPS &LOCK \u30AD\u30FC\u3092\u62BC\u3059",
    L"SHI&FT \u30AD\u30FC\u3092\u62BC\u3059",
    L"\u5165\u529B\u8A00\u8A9E\u306E\u30DB\u30C3\u30C8 \u30AD\u30FC",
    L"\u64CD\u4F5C",
    L"\u30AD\u30FC \u30B7\u30FC\u30B1\u30F3\u30B9(&K)",
    L"\u30AD\u30FC \u30B7\u30FC\u30B1\u30F3\u30B9\u306E\u5909\u66F4(&C)...",
    L"\u30AD\u30FC \u30B7\u30FC\u30B1\u30F3\u30B9\u306E\u5909\u66F4",
    L"\u5165\u529B\u8A00\u8A9E\u306E\u5207\u308A\u66FF\u3048",
    L"\u5272\u308A\u5F53\u3066\u306A\u3057",
    L"Ctrl + Shift",
    L"\u5DE6 Alt + Shift",
    L"\u30B0\u30EC\u30FC\u30D6 \u30A2\u30AF\u30BB\u30F3\u30C8 (`)",
    L"\u30AD\u30FC\u30DC\u30FC\u30C9 \u30EC\u30A4\u30A2\u30A6\u30C8\u306E\u5207\u308A\u66FF\u3048",
    L"\u5272\u308A\u5F53\u3066\u306A\u3057",
    L"Ctrl + Shift",
    L"\u5DE6 Alt + Shift",
    L"\u30B0\u30EC\u30FC\u30D6 \u30A2\u30AF\u30BB\u30F3\u30C8 (`)",
    L"\u30AD\u30FC \u30B7\u30FC\u30B1\u30F3\u30B9\u3092\u6709\u52B9\u306B\u3059\u308B(&E)",
    L"\u30AD\u30FC(&K):",
    L"\u5168\u822C",
    L"\u65E2\u5B9A\u306E\u5165\u529B\u8A00\u8A9E(&L)",
    L"\u3059\u3079\u3066\u306E\u5165\u529B\u30D5\u30A3\u30FC\u30EB\u30C9\u3067\u65E2\u5B9A\u3068\u3057\u3066\u4F7F\u7528\u3059\u308B\u3001\u30A4\u30F3\u30B9\u30C8\u30FC\u30EB\u6E08\u307F\u306E\u5165\u529B\u8A00\u8A9E\u3092\u9078\u629E\u3057\u307E\u3059\u3002",
    L"\u30A4\u30F3\u30B9\u30C8\u30FC\u30EB\u3055\u308C\u3066\u3044\u308B\u30B5\u30FC\u30D3\u30B9(&I)",
    L"\u4E00\u89A7\u306B\u8868\u793A\u3055\u308C\u308B\u5404\u5165\u529B\u8A00\u8A9E\u3067\u4F7F\u7528\u3059\u308B\u30B5\u30FC\u30D3\u30B9\u3092\u9078\u629E\u3057\u307E\u3059\u3002\u8FFD\u52A0\u30DC\u30BF\u30F3\u3068\u524A\u9664\u30DC\u30BF\u30F3\u3067\u4E00\u89A7\u3092\u5909\u66F4\u3057\u307E\u3059\u3002",
    L"\u8FFD\u52A0(&D)...",
    L"\u524A\u9664(&R)",
    L"\u30D7\u30ED\u30D1\u30C6\u30A3(&P)...",
    L"\u4E0A\u3078(&U)",
    L"\u4E0B\u3078(&O)",
    L"\u30AD\u30FC\u30DC\u30FC\u30C9 \u30EC\u30A4\u30A2\u30A6\u30C8\u306E\u30D7\u30EC\u30D3\u30E5\u30FC",
    L"\u30EC\u30A4\u30A2\u30A6\u30C8\u540D:",
    L"\u30A2\u30A4\u30B3\u30F3\u306E\u5909\u66F4(&C)...",
    L"\u4ECA\u5F8C\u3053\u306E\u30E1\u30C3\u30BB\u30FC\u30B8\u3092\u8868\u793A\u3057\u306A\u3044(&D)\u3002",
};

static const wchar_t* const kInpDlgTr_KO[47] = {
    L"\uD655\uC778",
    L"\uCDE8\uC18C",
    L"\uC785\uB825 \uC5B8\uC5B4 \uCD94\uAC00",
    L"\uC544\uB798 \uD655\uC778\uB780\uC744 \uC0AC\uC6A9\uD558\uC5EC \uCD94\uAC00\uD560 \uC5B8\uC5B4\uB97C \uC120\uD0DD\uD558\uC138\uC694.",
    L"\uBBF8\uB9AC \uBCF4\uAE30(&P)...",
    L"\uC5B8\uC5B4 \uD45C\uC2DC\uC904",
    L"\uBC14\uD0D5 \uD654\uBA74\uC5D0 \uD45C\uC2DC(&F)",
    L"\uC791\uC5C5 \uD45C\uC2DC\uC904\uC5D0 \uD45C\uC2DC(&D)",
    L"\uC228\uAE30\uAE30(&H)",
    L"\uBE44\uD65C\uC131 \uC2DC \uC5B8\uC5B4 \uD45C\uC2DC\uC904\uC744 \uD22C\uBA85\uD558\uAC8C \uD45C\uC2DC(&N)",
    L"\uC791\uC5C5 \uD45C\uC2DC\uC904\uC5D0 \uCD94\uAC00 \uC5B8\uC5B4 \uD45C\uC2DC\uC904 \uC544\uC774\uCF58 \uD45C\uC2DC(&I)",
    L"\uC5B8\uC5B4 \uD45C\uC2DC\uC904\uC5D0 \uD14D\uC2A4\uD2B8 \uB808\uC774\uBE14 \uD45C\uC2DC(&E)",
    L"\uACE0\uAE09 \uD0A4 \uC124\uC815",
    L"Caps Lock \uB044\uAE30",
    L"CAPS &LOCK \uD0A4 \uB204\uB974\uAE30",
    L"SHI&FT \uD0A4 \uB204\uB974\uAE30",
    L"\uC785\uB825 \uC5B8\uC5B4\uC758 \uBC14\uB85C \uAC00\uAE30 \uD0A4",
    L"\uB3D9\uC791",
    L"\uD0A4 \uC21C\uC11C(&K)",
    L"\uD0A4 \uC21C\uC11C \uBCC0\uACBD(&C)...",
    L"\uD0A4 \uC21C\uC11C \uBCC0\uACBD",
    L"\uC785\uB825 \uC5B8\uC5B4 \uC804\uD658",
    L"\uD560\uB2F9 \uC548 \uB428",
    L"Ctrl + Shift",
    L"\uC67C\uCABD Alt + Shift",
    L"\uC5B5\uC74C \uC545\uC13C\uD2B8(`)",
    L"\uD0A4\uBCF4\uB4DC \uB808\uC774\uC544\uC6C3 \uC804\uD658",
    L"\uD560\uB2F9 \uC548 \uB428",
    L"Ctrl + Shift",
    L"\uC67C\uCABD Alt + Shift",
    L"\uC5B5\uC74C \uC545\uC13C\uD2B8(`)",
    L"\uD0A4 \uC21C\uC11C \uC124\uC815(&E)",
    L"\uD0A4(&K):",
    L"\uC77C\uBC18",
    L"\uAE30\uBCF8 \uC785\uB825 \uC5B8\uC5B4(&L)",
    L"\uBAA8\uB4E0 \uC785\uB825 \uD544\uB4DC\uC5D0 \uAE30\uBCF8\uAC12\uC73C\uB85C \uC0AC\uC6A9\uD560 \uC124\uCE58\uB41C \uC785\uB825 \uC5B8\uC5B4\uB97C \uC120\uD0DD\uD558\uC138\uC694.",
    L"\uC124\uCE58\uB41C \uC11C\uBE44\uC2A4(&I)",
    L"\uBAA9\uB85D\uC5D0 \uD45C\uC2DC\uB41C \uAC01 \uC785\uB825 \uC5B8\uC5B4\uC5D0 \uC0AC\uC6A9\uD560 \uC11C\uBE44\uC2A4\uB97C \uC120\uD0DD\uD558\uC138\uC694. \uCD94\uAC00 \uBC0F \uC81C\uAC70 \uB2E8\uCD94\uB97C \uC0AC\uC6A9\uD558\uC5EC \uC774 \uBAA9\uB85D\uC744 \uC218\uC815\uD569\uB2C8\uB2E4.",
    L"\uCD94\uAC00(&D)...",
    L"\uC81C\uAC70(&R)",
    L"\uC18D\uC131(&P)...",
    L"\uC704\uB85C \uC774\uB3D9(&U)",
    L"\uC544\uB798\uB85C \uC774\uB3D9(&O)",
    L"\uD0A4\uBCF4\uB4DC \uB808\uC774\uC544\uC6C3 \uBBF8\uB9AC \uBCF4\uAE30",
    L"\uB808\uC774\uC544\uC6C3 \uC774\uB984:",
    L"\uC544\uC774\uCF58 \uBCC0\uACBD(&C)...",
    L"\uC774 \uBA54\uC2DC\uC9C0\uB97C \uB2E4\uC2DC \uD45C\uC2DC \uC548 \uD568(&D).",
};

static const wchar_t* const kInpDlgTr_TR[47] = {
    L"Tamam",
    L"\u0130ptal",
    L"Giri\u015F dili ekle",
    L"Eklemek istedi\u011Finiz dili a\u015Fa\u011F\u0131daki onay kutular\u0131n\u0131 kullanarak se\u00E7in.",
    L"\u00D6&nizleme...",
    L"Dil \u00E7ubu\u011Fu",
    L"Masa\u00FCst\u00FCnde &serbest",
    L"G\u00F6rev \u00E7ubu\u011Funa &yerle\u015Fik",
    L"&Gizli",
    L"&Etkin olmad\u0131\u011F\u0131nda dil \u00E7ubu\u011Funu saydam g\u00F6ster",
    L"G\u00F6rev \u00E7ubu\u011Funda ek dil \u00E7ubu\u011Fu s&imgelerini g\u00F6ster",
    L"Dil \u00E7ubu\u011Funda meti&n etiketlerini g\u00F6ster",
    L"Geli\u015Fmi\u015F tu\u015F ayarlar\u0131",
    L"Caps Lock'u kapatmak i\u00E7in",
    L"CAPS &LOCK tu\u015Funa bas",
    L"SHI&FT tu\u015Funa bas",
    L"Giri\u015F dilleri i\u00E7in k\u0131sayol tu\u015Flar\u0131",
    L"Eylem",
    L"&Tu\u015F dizisi",
    L"Tu\u015F &dizisini de\u011Fi\u015Ftir...",
    L"Tu\u015F dizisini de\u011Fi\u015Ftir",
    L"Giri\u015F dilini de\u011Fi\u015Ftir",
    L"&Atanmad\u0131",
    L"&Ctrl + Shift",
    L"&Sol Alt + Shift",
    L"&Vurgu i\u015Fareti (`)",
    L"Klavye d\u00FCzenini de\u011Fi\u015Ftir",
    L"Atanma&d\u0131",
    L"C&trl + Shift",
    L"Sol A&lt + Shift",
    L"Vurgu &i\u015Fareti (`)",
    L"Tu\u015F dizisini &etkinle\u015Ftir",
    L"&Tu\u015F:",
    L"Genel",
    L"Varsay\u0131lan giri\u015F &dili",
    L"T\u00FCm giri\u015F alanlar\u0131nda varsay\u0131lan olarak kullan\u0131lacak y\u00FCkl\u00FC giri\u015F dillerinden birini se\u00E7in.",
    L"&Y\u00FCkl\u00FC hizmetler",
    L"Listede g\u00F6sterilen her giri\u015F dili i\u00E7in istedi\u011Finiz hizmetleri se\u00E7in. Listeyi de\u011Fi\u015Ftirmek i\u00E7in Ekle ve Kald\u0131r d\u00FC\u011Fmelerini kullan\u0131n.",
    L"&Ekle...",
    L"&Kald\u0131r",
    L"\u00D6&zellikler...",
    L"Y&ukar\u0131 Ta\u015F\u0131",
    L"A\u015Fa\u011F\u0131 T&a\u015F\u0131",
    L"Klavye D\u00FCzeni \u00D6nizlemesi",
    L"D\u00FCzen Ad\u0131:",
    L"S&imge De\u011Fi\u015Ftir...",
    L"Bu iletiyi bir daha g\u00F6ster&me.",
};

static const wchar_t* const kInpDlgTr_CS[47] = {
    L"OK",
    L"Storno",
    L"P\u0159idat vstupn\u00ED jazyk",
    L"Vyberte jazyk, kter\u00FD chcete p\u0159idat, pomoc\u00ED za\u0161krt\u00E1vac\u00EDch pol\u00ED\u010Dek n\u00ED\u017Ee.",
    L"&N\u00E1hled...",
    L"Panel jazyk\u016F",
    L"&Plovouc\u00ED na plo\u0161e",
    L"Upevn\u011Bno na panelu &\u00FAloh",
    L"&Skryt\u00FD",
    L"Zobrazit panel jazyk\u016F jako pr\u016Fhledn\u00FD, pokud nen\u00ED akti&vn\u00ED",
    L"Zobrazit dop&l\u0148kov\u00E9 ikony panelu jazyk\u016F na panelu \u00FAloh",
    L"Zobrazit t&extov\u00E9 popisky na panelu jazyk\u016F",
    L"Up\u0159esnit nastaven\u00ED kl\u00E1ves",
    L"Chcete-li vypnout kl\u00E1vesu Caps Lock",
    L"Stisknout kl\u00E1vesu CAPS &LOCK",
    L"Stisknout kl\u00E1vesu SHI&FT",
    L"Kl\u00E1vesov\u00E9 zkratky pro vstupn\u00ED jazyky",
    L"Akce",
    L"&Kombinace kl\u00E1ves",
    L"&Zm\u011Bnit kombinaci kl\u00E1ves...",
    L"Zm\u011Bnit kombinaci kl\u00E1ves",
    L"P\u0159epnout vstupn\u00ED jazyk",
    L"&Nep\u0159i\u0159azeno",
    L"&Ctrl + Shift",
    L"&Lev\u00FD Alt + Shift",
    L"&P\u0159\u00EDzvuk (`)",
    L"P\u0159epnout rozlo\u017Een\u00ED kl\u00E1vesnice",
    L"N&ep\u0159i\u0159azeno",
    L"C&trl + Shift",
    L"Le&v\u00FD Alt + Shift",
    L"P\u0159\u00ED&zvuk (`)",
    L"&Povolit kombinaci kl\u00E1ves",
    L"&Kl\u00E1vesa:",
    L"Obecn\u00E9",
    L"V\u00FDchoz\u00ED vstupn\u00ED &jazyk",
    L"Vyberte jeden z nainstalovan\u00FDch vstupn\u00EDch jazyk\u016F, kter\u00FD se pou\u017Eije jako v\u00FDchoz\u00ED pro v\u0161echna vstupn\u00ED pole.",
    L"&Nainstalovan\u00E9 slu\u017Eby",
    L"Vyberte slu\u017Eby pro jednotliv\u00E9 vstupn\u00ED jazyky zobrazen\u00E9 v seznamu. Seznam upravte pomoc\u00ED tla\u010D\u00EDtek P\u0159idat a Odebrat.",
    L"&P\u0159idat...",
    L"&Odebrat",
    L"&Vlastnosti...",
    L"Posunout v&\u00FD\u0161",
    L"Posunout n\u00ED&\u017E",
    L"N\u00E1hled rozlo\u017Een\u00ED kl\u00E1vesnice",
    L"N\u00E1zev rozlo\u017Een\u00ED:",
    L"Zm\u011Bnit &ikonu...",
    L"&Tuto zpr\u00E1vu u\u017E p\u0159\u00ED\u0161t\u011B nezobrazovat.",
};

static const wchar_t* const kInpDlgTr_HU[47] = {
    L"OK",
    L"M\u00E9gse",
    L"Beviteli nyelv hozz\u00E1ad\u00E1sa",
    L"V\u00E1lassza ki a hozz\u00E1adand\u00F3 nyelvet az al\u00E1bbi jel\u00F6l\u0151n\u00E9gyzetekkel.",
    L"&El\u0151n\u00E9zet...",
    L"Nyelvi eszk\u00F6zt\u00E1r",
    L"&Lebeg\u0151 az asztalon",
    L"R\u00F6gz\u00EDtve a &t\u00E1lc\u00E1n",
    L"&Rejtett",
    L"A nyelvi eszk\u00F6zt\u00E1r megjelen\u00EDt\u00E9se \u00E1ttetsz\u0151k\u00E9nt, ha i&nakt\u00EDv",
    L"K&ieg\u00E9sz\u00EDt\u0151 nyelvi eszk\u00F6zt\u00E1r-ikonok megjelen\u00EDt\u00E9se a t\u00E1lc\u00E1n",
    L"Sz\u00F6veges c\u00ED&mk\u00E9k megjelen\u00EDt\u00E9se a nyelvi eszk\u00F6zt\u00E1ron",
    L"Speci\u00E1lis billenty\u0171zet-be\u00E1ll\u00EDt\u00E1sok",
    L"A Caps Lock kikapcsol\u00E1s\u00E1hoz",
    L"A CAPS &LOCK billenty\u0171 lenyom\u00E1sa",
    L"A SHI&FT billenty\u0171 lenyom\u00E1sa",
    L"A beviteli nyelvek gyorsbillenty\u0171i",
    L"M\u0171velet",
    L"&Billenty\u0171sorrend",
    L"Billenty\u0171sorrend &m\u00F3dos\u00EDt\u00E1sa...",
    L"Billenty\u0171sorrend m\u00F3dos\u00EDt\u00E1sa",
    L"Beviteli nyelv v\u00E1lt\u00E1sa",
    L"&Nincs hozz\u00E1rendelve",
    L"&Ctrl + Shift",
    L"&Bal Alt + Shift",
    L"&Visszav\u00E1g\u00F3jel (`)",
    L"Billenty\u0171zetkioszt\u00E1s v\u00E1lt\u00E1sa",
    L"N&incs hozz\u00E1rendelve",
    L"C&trl + Shift",
    L"B&al Alt + Shift",
    L"Visszav\u00E1g\u00F3&jel (`)",
    L"&Billenty\u0171sorrend enged\u00E9lyez\u00E9se",
    L"Billent&y\u0171:",
    L"\u00C1ltal\u00E1nos",
    L"Alap\u00E9rtelmezett beviteli &nyelv",
    L"V\u00E1lasszon egy telep\u00EDtett beviteli nyelvet, amely alap\u00E9rtelmezettk\u00E9nt haszn\u00E1lhat\u00F3 minden beviteli mez\u0151ben.",
    L"&Telep\u00EDtett szolg\u00E1ltat\u00E1sok",
    L"V\u00E1lassza ki a list\u00E1ban szerepl\u0151 beviteli nyelvekhez k\u00EDv\u00E1nt szolg\u00E1ltat\u00E1sokat. A lista m\u00F3dos\u00EDt\u00E1s\u00E1hoz haszn\u00E1lja a Hozz\u00E1ad\u00E1s \u00E9s Elt\u00E1vol\u00EDt\u00E1s gombot.",
    L"&Hozz\u00E1ad\u00E1s...",
    L"&Elt\u00E1vol\u00EDt\u00E1s",
    L"Tulajdons\u00E1&gok...",
    L"&Feljebb",
    L"&Lejjebb",
    L"Billenty\u0171zetkioszt\u00E1s el\u0151n\u00E9zete",
    L"Kioszt\u00E1s neve:",
    L"&Ikon m\u00F3dos\u00EDt\u00E1sa...",
    L"Ne &jelen\u00EDtse meg \u00FAjra ezt az \u00FCzenetet.",
};

static const wchar_t* const kInpDlgTr_RO[47] = {
    L"OK",
    L"Anulare",
    L"Ad\u0103ugare limb\u0103 de intrare",
    L"Selecta\u021Bi limba de ad\u0103ugat utiliz\u00E2nd casetele de selectare de mai jos.",
    L"&Previzualizare...",
    L"Bara de limbi",
    L"&Plutind pe desktop",
    L"Andocat\u0103 \u00EEn &bara de sarcini",
    L"&Ascuns\u0103",
    L"Afi\u0219a\u021Bi bara de limbi ca transparent\u0103 c\u00E2nd este i&nactiv\u0103",
    L"Afi\u0219a\u021Bi icoane s&uplimentare pentru bara de limbi \u00EEn bara de sarcini",
    L"Afi\u0219a\u021Bi &etichete text pe bara de limbi",
    L"Set\u0103ri avansate ale tastelor",
    L"Pentru a dezactiva Caps Lock",
    L"Ap\u0103sa\u021Bi tasta CAPS &LOCK",
    L"Ap\u0103sa\u021Bi tasta SHI&FT",
    L"Taste rapide pentru limbile de intrare",
    L"Ac\u021Biune",
    L"&Secven\u021B\u0103 de taste",
    L"Schim&ba\u021Bi secven\u021Ba de taste...",
    L"Schimba\u021Bi secven\u021Ba de taste",
    L"Comutare limb\u0103 de intrare",
    L"&Neatribuit",
    L"&Ctrl + Shift",
    L"&Alt st\u00E2nga + Shift",
    L"Accent &grav (`)",
    L"Comutare aspect de tastatur\u0103",
    L"N&eatribuit",
    L"C&trl + Shift",
    L"Alt st\u00E2nga + Shi&ft",
    L"Accent g&rav (`)",
    L"&Activa\u021Bi secven\u021Ba de taste",
    L"&Tasta:",
    L"General",
    L"Limba de intrare &implicit\u0103",
    L"Selecta\u021Bi una dintre limbile de intrare instalate pentru a o utiliza ca implicit\u0103 pentru toate c\u00E2mpurile de intrare.",
    L"&Servicii instalate",
    L"Selecta\u021Bi serviciile dorite pentru fiecare limb\u0103 de intrare afi\u0219at\u0103 \u00EEn list\u0103. Folosi\u021Bi butoanele Ad\u0103ugare \u0219i Eliminare pentru a modifica lista.",
    L"&Ad\u0103ugare...",
    L"&Eliminare",
    L"&Propriet\u0103\u021Bi...",
    L"Mutare \u00EEn s&us",
    L"Mutare \u00EEn &jos",
    L"Previzualizare aspect tastatur\u0103",
    L"Nume aspect:",
    L"Schimbare i&con\u0103...",
    L"&Nu mai afi\u0219a\u021Bi acest mesaj.",
};

static const wchar_t* const kInpDlgTr_SV[47] = {
    L"OK",
    L"Avbryt",
    L"L\u00E4gg till inmatningsspr\u00E5k",
    L"V\u00E4lj spr\u00E5ket som ska l\u00E4ggas till med kryssrutorna nedan.",
    L"&F\u00F6rhandsgranska...",
    L"Spr\u00E5kf\u00E4lt",
    L"&Flytande p\u00E5 skrivbordet",
    L"Dockad i &aktivitetsf\u00E4ltet",
    L"&Dolt",
    L"Visa spr\u00E5kf\u00E4ltet som genomskinligt n\u00E4r det \u00E4r i&naktivt",
    L"Visa ytterl&igare spr\u00E5kf\u00E4ltsikoner i aktivitetsf\u00E4ltet",
    L"Visa t&extetiketter p\u00E5 spr\u00E5kf\u00E4ltet",
    L"Avancerade tangentinst\u00E4llningar",
    L"Om du vill st\u00E4nga av Caps Lock",
    L"Tryck p\u00E5 CAPS &LOCK-tangenten",
    L"Tryck p\u00E5 SHI&FT-tangenten",
    L"Snabbtangenter f\u00F6r inmatningsspr\u00E5k",
    L"\u00C5tg\u00E4rd",
    L"&Tangentsekvens",
    L"\u00C4&ndra tangentsekvens...",
    L"\u00C4ndra tangentsekvens",
    L"V\u00E4xla inmatningsspr\u00E5k",
    L"&Inte tilldelad",
    L"&Ctrl + Skift",
    L"&V\u00E4nster Alt + Skift",
    L"&Grav accent (`)",
    L"V\u00E4xla tangentbordslayout",
    L"Inte tilld&elad",
    L"C&trl + Skift",
    L"V\u00E4&nster Alt + Skift",
    L"Grav &accent (`)",
    L"&Aktivera tangentsekvens",
    L"&Tangent:",
    L"Allm\u00E4nt",
    L"Standardinmatnings&spr\u00E5k",
    L"V\u00E4lj ett av de installerade inmatningsspr\u00E5ken som ska anv\u00E4ndas som standard f\u00F6r alla inmatningsf\u00E4lt.",
    L"&Installerade tj\u00E4nster",
    L"V\u00E4lj de tj\u00E4nster du vill anv\u00E4nda f\u00F6r varje inmatningsspr\u00E5k i listan. Anv\u00E4nd knapparna L\u00E4gg till och Ta bort om du vill \u00E4ndra listan.",
    L"&L\u00E4gg till...",
    L"&Ta bort",
    L"&Egenskaper...",
    L"Flytta &upp",
    L"Flytta &ned",
    L"F\u00F6rhandsgranskning av tangentbordslayout",
    L"Layoutnamn:",
    L"\u00C4ndra &ikon...",
    L"&Visa inte detta meddelande igen.",
};

static const wchar_t* const kInpDlgTr_UK[47] = {
    L"OK",
    L"\u0421\u043A\u0430\u0441\u0443\u0432\u0430\u0442\u0438",
    L"\u0414\u043E\u0434\u0430\u0442\u0438 \u043C\u043E\u0432\u0443 \u0432\u0432\u0435\u0434\u0435\u043D\u043D\u044F",
    L"\u0412\u0438\u0431\u0435\u0440\u0456\u0442\u044C \u043C\u043E\u0432\u0443 \u0434\u043B\u044F \u0434\u043E\u0434\u0430\u0432\u0430\u043D\u043D\u044F \u0437\u0430 \u0434\u043E\u043F\u043E\u043C\u043E\u0433\u043E\u044E \u043D\u0430\u0432\u0435\u0434\u0435\u043D\u0438\u0445 \u043D\u0438\u0436\u0447\u0435 \u043F\u0440\u0430\u043F\u043E\u0440\u0446\u0456\u0432.",
    L"&\u041F\u043E\u043F\u0435\u0440\u0435\u0434\u043D\u0456\u0439 \u043F\u0435\u0440\u0435\u0433\u043B\u044F\u0434...",
    L"\u041C\u043E\u0432\u043D\u0430 \u043F\u0430\u043D\u0435\u043B\u044C",
    L"&\u041F\u043B\u0430\u0432\u0430\u044E\u0447\u0430 \u043D\u0430 \u0440\u043E\u0431\u043E\u0447\u043E\u043C\u0443 \u0441\u0442\u043E\u043B\u0456",
    L"\u0417\u0430\u043A\u0440\u0456\u043F\u043B\u0435\u043D\u0430 \u043D\u0430 \u043F\u0430\u043D\u0435\u043B\u0456 &\u0437\u0430\u0432\u0434\u0430\u043D\u044C",
    L"\u041F\u0440\u0438&\u0445\u043E\u0432\u0430\u043D\u0430",
    L"\u041F\u043E\u043A\u0430\u0437\u0443\u0432\u0430\u0442\u0438 \u043C\u043E\u0432\u043D\u0443 \u043F\u0430\u043D\u0435\u043B\u044C \u043F\u0440\u043E\u0437\u043E\u0440\u043E\u044E, \u044F\u043A\u0449\u043E \u0432\u043E\u043D\u0430 &\u043D\u0435\u0430\u043A\u0442\u0438\u0432\u043D\u0430",
    L"\u041F\u043E\u043A\u0430\u0437\u0443\u0432\u0430\u0442\u0438 \u0434\u043E\u043F\u043E&\u043B\u043D\u0456 \u0437\u043D\u0430\u0447\u043A\u0438 \u043C\u043E\u0432\u043D\u043E\u0457 \u043F\u0430\u043D\u0435\u043B\u0456 \u043D\u0430 \u043F\u0430\u043D\u0435\u043B\u0456 \u0437\u0430\u0432\u0434\u0430\u043D\u044C",
    L"\u041F\u043E\u043A\u0430\u0437\u0443\u0432\u0430\u0442\u0438 \u0442\u0435&\u043A\u0441\u0442\u043E\u0432\u0456 \u043F\u0456\u0434\u043F\u0438\u0441\u0438 \u043D\u0430 \u043C\u043E\u0432\u043D\u0456\u0439 \u043F\u0430\u043D\u0435\u043B\u0456",
    L"\u0420\u043E\u0437\u0448\u0438\u0440\u0435\u043D\u0456 \u043F\u0430\u0440\u0430\u043C\u0435\u0442\u0440\u0438 \u043A\u043B\u0430\u0432\u0456\u0448",
    L"\u0429\u043E\u0431 \u0432\u0438\u043C\u043A\u043D\u0443\u0442\u0438 \u043A\u043B\u0430\u0432\u0456\u0448\u0443 Caps Lock",
    L"\u041D\u0430\u0442\u0438\u0441\u043A\u0430\u0442\u0438 \u043A\u043B\u0430\u0432\u0456\u0448\u0443 CAPS &LOCK",
    L"\u041D\u0430\u0442\u0438\u0441\u043A\u0430\u0442\u0438 \u043A\u043B\u0430\u0432\u0456\u0448\u0443 SHI&FT",
    L"\u0421\u043F\u043E\u043B\u0443\u0447\u0435\u043D\u043D\u044F \u043A\u043B\u0430\u0432\u0456\u0448 \u0434\u043B\u044F \u043C\u043E\u0432 \u0432\u0432\u0435\u0434\u0435\u043D\u043D\u044F",
    L"\u0414\u0456\u044F",
    L"&\u041F\u043E\u0441\u043B\u0456\u0434\u043E\u0432\u043D\u0456\u0441\u0442\u044C \u043A\u043B\u0430\u0432\u0456\u0448",
    L"&\u0417\u043C\u0456\u043D\u0438\u0442\u0438 \u043F\u043E\u0441\u043B\u0456\u0434\u043E\u0432\u043D\u0456\u0441\u0442\u044C \u043A\u043B\u0430\u0432\u0456\u0448...",
    L"\u0417\u043C\u0456\u043D\u0438\u0442\u0438 \u043F\u043E\u0441\u043B\u0456\u0434\u043E\u0432\u043D\u0456\u0441\u0442\u044C \u043A\u043B\u0430\u0432\u0456\u0448",
    L"\u0417\u043C\u0456\u043D\u0438\u0442\u0438 \u043C\u043E\u0432\u0443 \u0432\u0432\u0435\u0434\u0435\u043D\u043D\u044F",
    L"&\u041D\u0435 \u043F\u0440\u0438\u0437\u043D\u0430\u0447\u0435\u043D\u043E",
    L"&CTRL + SHIFT",
    L"&\u041B\u0456\u0432\u0438\u0439 ALT + SHIFT",
    L"&\u0417\u043D\u0430\u043A \u043D\u0430\u0433\u043E\u043B\u043E\u0441\u0443 (`)",
    L"\u0417\u043C\u0456\u043D\u0438\u0442\u0438 \u0440\u043E\u0437\u043A\u043B\u0430\u0434\u043A\u0443 \u043A\u043B\u0430\u0432\u0456\u0430\u0442\u0443\u0440\u0438",
    L"\u041D&\u0435 \u043F\u0440\u0438\u0437\u043D\u0430\u0447\u0435\u043D\u043E",
    L"CTR&L + SHIFT",
    L"\u041B\u0456\u0432\u0438&\u0439 ALT + SHIFT",
    L"\u0417\u043D\u0430&\u043A \u043D\u0430\u0433\u043E\u043B\u043E\u0441\u0443 (`)",
    L"&\u0423\u0432\u0456\u043C\u043A\u043D\u0443\u0442\u0438 \u043F\u043E\u0441\u043B\u0456\u0434\u043E\u0432\u043D\u0456\u0441\u0442\u044C \u043A\u043B\u0430\u0432\u0456\u0448",
    L"&\u041A\u043B\u0430\u0432\u0456\u0448\u0430:",
    L"\u0417\u0430\u0433\u0430\u043B\u044C\u043D\u0456",
    L"\u041C\u043E\u0432\u0430 \u0432\u0432\u0435\u0434\u0435\u043D\u043D\u044F \u0437\u0430 &\u0437\u0430\u043C\u043E\u0432\u0447\u0443\u0432\u0430\u043D\u043D\u044F\u043C",
    L"\u0412\u0438\u0431\u0435\u0440\u0456\u0442\u044C \u043E\u0434\u043D\u0443 \u0437 \u0432\u0441\u0442\u0430\u043D\u043E\u0432\u043B\u0435\u043D\u0438\u0445 \u043C\u043E\u0432 \u0432\u0432\u0435\u0434\u0435\u043D\u043D\u044F, \u044F\u043A\u0430 \u0432\u0438\u043A\u043E\u0440\u0438\u0441\u0442\u043E\u0432\u0443\u0432\u0430\u0442\u0438\u043C\u0435\u0442\u044C\u0441\u044F \u0437\u0430 \u0437\u0430\u043C\u043E\u0432\u0447\u0443\u0432\u0430\u043D\u043D\u044F\u043C \u0443 \u0432\u0441\u0456\u0445 \u043F\u043E\u043B\u044F\u0445 \u0432\u0432\u0435\u0434\u0435\u043D\u043D\u044F.",
    L"&\u0412\u0441\u0442\u0430\u043D\u043E\u0432\u043B\u0435\u043D\u0456 \u0441\u043B\u0443\u0436\u0431\u0438",
    L"\u0412\u0438\u0431\u0435\u0440\u0456\u0442\u044C \u0441\u043B\u0443\u0436\u0431\u0438 \u0434\u043B\u044F \u043A\u043E\u0436\u043D\u043E\u0457 \u043C\u043E\u0432\u0438 \u0432\u0432\u0435\u0434\u0435\u043D\u043D\u044F \u0443 \u0441\u043F\u0438\u0441\u043A\u0443. \u0414\u043B\u044F \u0437\u043C\u0456\u043D\u0438 \u0441\u043F\u0438\u0441\u043A\u0443 \u0432\u0438\u043A\u043E\u0440\u0438\u0441\u0442\u043E\u0432\u0443\u0439\u0442\u0435 \u043A\u043D\u043E\u043F\u043A\u0438 \u00AB\u0414\u043E\u0434\u0430\u0442\u0438\u00BB \u0442\u0430 \u00AB\u0412\u0438\u0434\u0430\u043B\u0438\u0442\u0438\u00BB.",
    L"&\u0414\u043E\u0434\u0430\u0442\u0438...",
    L"\u0412\u0438\u0434\u0430&\u043B\u0438\u0442\u0438",
    L"\u0412\u043B\u0430\u0441\u0442\u0438\u0432\u043E\u0441&\u0442\u0456...",
    L"\u0412&\u0433\u043E\u0440\u0443",
    L"\u0412&\u043D\u0438\u0437",
    L"\u041F\u0435\u0440\u0435\u0433\u043B\u044F\u0434 \u0440\u043E\u0437\u043A\u043B\u0430\u0434\u043A\u0438 \u043A\u043B\u0430\u0432\u0456\u0430\u0442\u0443\u0440\u0438",
    L"\u041D\u0430\u0437\u0432\u0430 \u0440\u043E\u0437\u043A\u043B\u0430\u0434\u043A\u0438:",
    L"\u0417\u043C\u0456\u043D\u0438\u0442\u0438 \u0437&\u043D\u0430\u0447\u043E\u043A...",
    L"&\u0411\u0456\u043B\u044C\u0448\u0435 \u043D\u0435 \u043F\u043E\u043A\u0430\u0437\u0443\u0432\u0430\u0442\u0438 \u0446\u0435 \u043F\u043E\u0432\u0456\u0434\u043E\u043C\u043B\u0435\u043D\u043D\u044F.",
};

static const wchar_t* const kInpDlgTr_EL[47] = {
    L"OK",
    L"\u0386\u03BA\u03C5\u03C1\u03BF",
    L"\u03A0\u03C1\u03BF\u03C3\u03B8\u03AE\u03BA\u03B7 \u03B3\u03BB\u03CE\u03C3\u03C3\u03B1\u03C2 \u03B5\u03B9\u03C3\u03CC\u03B4\u03BF\u03C5",
    L"\u0395\u03C0\u03B9\u03BB\u03AD\u03BE\u03C4\u03B5 \u03C4\u03B7 \u03B3\u03BB\u03CE\u03C3\u03C3\u03B1 \u03C0\u03BF\u03C5 \u03B8\u03B1 \u03C0\u03C1\u03BF\u03C3\u03C4\u03B5\u03B8\u03B5\u03AF \u03C7\u03C1\u03B7\u03C3\u03B9\u03BC\u03BF\u03C0\u03BF\u03B9\u03CE\u03BD\u03C4\u03B1\u03C2 \u03C4\u03B1 \u03C0\u03B1\u03C1\u03B1\u03BA\u03AC\u03C4\u03C9 \u03C0\u03BB\u03B1\u03AF\u03C3\u03B9\u03B1 \u03B5\u03BB\u03AD\u03B3\u03C7\u03BF\u03C5.",
    L"&\u03A0\u03C1\u03BF\u03B5\u03C0\u03B9\u03C3\u03BA\u03CC\u03C0\u03B7\u03C3\u03B7...",
    L"\u0393\u03C1\u03B1\u03BC\u03BC\u03AE \u03B3\u03BB\u03CE\u03C3\u03C3\u03B1\u03C2",
    L"&\u0391\u03B9\u03C9\u03C1\u03BF\u03CD\u03BC\u03B5\u03BD\u03B7 \u03C3\u03C4\u03B7\u03BD \u03B5\u03C0\u03B9\u03C6\u03AC\u03BD\u03B5\u03B9\u03B1 \u03B5\u03C1\u03B3\u03B1\u03C3\u03AF\u03B1\u03C2",
    L"\u03A0\u03C1\u03BF\u03C3\u03B1\u03C1\u03C4\u03B7\u03BC\u03AD\u03BD\u03B7 \u03C3\u03C4\u03B7 &\u03B3\u03C1\u03B1\u03BC\u03BC\u03AE \u03B5\u03C1\u03B3\u03B1\u03C3\u03B9\u03CE\u03BD",
    L"&\u039A\u03C1\u03C5\u03C6\u03AE",
    L"\u0395\u03BC\u03C6\u03AC\u03BD\u03B9\u03C3\u03B7 \u03C4\u03B7\u03C2 \u03B3\u03C1\u03B1\u03BC\u03BC\u03AE\u03C2 \u03B3\u03BB\u03CE\u03C3\u03C3\u03B1\u03C2 \u03C9\u03C2 \u03B4\u03B9\u03B1\u03C6\u03B1\u03BD\u03AE\u03C2 \u03CC\u03C4\u03B1\u03BD \u03B5\u03AF\u03BD\u03B1\u03B9 \u03B1&\u03BD\u03B5\u03BD\u03B5\u03C1\u03B3\u03AE",
    L"\u0395\u03BC\u03C6\u03AC\u03BD\u03B9\u03C3\u03B7 \u03C0\u03C1\u03CC\u03C3&\u03B8\u03B5\u03C4\u03C9\u03BD \u03B5\u03B9\u03BA\u03BF\u03BD\u03B9\u03B4\u03AF\u03C9\u03BD \u03B3\u03C1\u03B1\u03BC\u03BC\u03AE\u03C2 \u03B3\u03BB\u03CE\u03C3\u03C3\u03B1\u03C2 \u03C3\u03C4\u03B7 \u03B3\u03C1\u03B1\u03BC\u03BC\u03AE \u03B5\u03C1\u03B3\u03B1\u03C3\u03B9\u03CE\u03BD",
    L"\u0395\u03BC\u03C6\u03AC\u03BD\u03B9\u03C3\u03B7 \u03B5\u03C4\u03B9\u03BA&\u03B5\u03C4\u03CE\u03BD \u03BA\u03B5\u03B9\u03BC\u03AD\u03BD\u03BF\u03C5 \u03C3\u03C4\u03B7 \u03B3\u03C1\u03B1\u03BC\u03BC\u03AE \u03B3\u03BB\u03CE\u03C3\u03C3\u03B1\u03C2",
    L"\u03A1\u03C5\u03B8\u03BC\u03AF\u03C3\u03B5\u03B9\u03C2 \u03C0\u03BB\u03AE\u03BA\u03C4\u03C1\u03C9\u03BD \u03B3\u03B9\u03B1 \u03C0\u03C1\u03BF\u03C7\u03C9\u03C1\u03B7\u03BC\u03AD\u03BD\u03BF\u03C5\u03C2",
    L"\u0393\u03B9\u03B1 \u03B1\u03C0\u03B5\u03BD\u03B5\u03C1\u03B3\u03BF\u03C0\u03BF\u03AF\u03B7\u03C3\u03B7 \u03C4\u03BF\u03C5 Caps Lock",
    L"\u03A0\u03B1\u03C4\u03AE\u03C3\u03C4\u03B5 \u03C4\u03BF \u03C0\u03BB\u03AE\u03BA\u03C4\u03C1\u03BF CAPS &LOCK",
    L"\u03A0\u03B1\u03C4\u03AE\u03C3\u03C4\u03B5 \u03C4\u03BF \u03C0\u03BB\u03AE\u03BA\u03C4\u03C1\u03BF SHI&FT",
    L"\u03A0\u03BB\u03AE\u03BA\u03C4\u03C1\u03B1 \u03C3\u03C5\u03BD\u03C4\u03CC\u03BC\u03B5\u03C5\u03C3\u03B7\u03C2 \u03B3\u03B9\u03B1 \u03B3\u03BB\u03CE\u03C3\u03C3\u03B5\u03C2 \u03B5\u03B9\u03C3\u03CC\u03B4\u03BF\u03C5",
    L"\u0395\u03BD\u03AD\u03C1\u03B3\u03B5\u03B9\u03B1",
    L"&\u0391\u03BB\u03BB\u03B7\u03BB\u03BF\u03C5\u03C7\u03AF\u03B1 \u03C0\u03BB\u03AE\u03BA\u03C4\u03C1\u03C9\u03BD",
    L"\u0391&\u03BB\u03BB\u03B1\u03B3\u03AE \u03B1\u03BB\u03BB\u03B7\u03BB\u03BF\u03C5\u03C7\u03AF\u03B1\u03C2 \u03C0\u03BB\u03AE\u03BA\u03C4\u03C1\u03C9\u03BD...",
    L"\u0391\u03BB\u03BB\u03B1\u03B3\u03AE \u03B1\u03BB\u03BB\u03B7\u03BB\u03BF\u03C5\u03C7\u03AF\u03B1\u03C2 \u03C0\u03BB\u03AE\u03BA\u03C4\u03C1\u03C9\u03BD",
    L"\u0391\u03BB\u03BB\u03B1\u03B3\u03AE \u03B3\u03BB\u03CE\u03C3\u03C3\u03B1\u03C2 \u03B5\u03B9\u03C3\u03CC\u03B4\u03BF\u03C5",
    L"&\u039C\u03B7 \u03B5\u03BA\u03C7\u03C9\u03C1\u03B7\u03BC\u03AD\u03BD\u03BF",
    L"&Ctrl + Shift",
    L"&\u0391\u03C1\u03B9\u03C3\u03C4\u03B5\u03C1\u03CC Alt + Shift",
    L"&\u0392\u03B1\u03C1\u03B5\u03AF\u03B1 (`)",
    L"\u0391\u03BB\u03BB\u03B1\u03B3\u03AE \u03B4\u03B9\u03AC\u03C4\u03B1\u03BE\u03B7\u03C2 \u03C0\u03BB\u03B7\u03BA\u03C4\u03C1\u03BF\u03BB\u03BF\u03B3\u03AF\u03BF\u03C5",
    L"\u039C&\u03B7 \u03B5\u03BA\u03C7\u03C9\u03C1\u03B7\u03BC\u03AD\u03BD\u03BF",
    L"C&trl + Shift",
    L"\u0391\u03C1\u03B9\u03C3\u03C4\u03B5\u03C1&\u03CC Alt + Shift",
    L"\u0392\u03B1\u03C1\u03B5&\u03AF\u03B1 (`)",
    L"&\u0395\u03BD\u03B5\u03C1\u03B3\u03BF\u03C0\u03BF\u03AF\u03B7\u03C3\u03B7 \u03B1\u03BB\u03BB\u03B7\u03BB\u03BF\u03C5\u03C7\u03AF\u03B1\u03C2 \u03C0\u03BB\u03AE\u03BA\u03C4\u03C1\u03C9\u03BD",
    L"&\u03A0\u03BB\u03AE\u03BA\u03C4\u03C1\u03BF:",
    L"\u0393\u03B5\u03BD\u03B9\u03BA\u03AC",
    L"\u03A0\u03C1\u03BF\u03B5\u03C0\u03B9\u03BB\u03B5\u03B3\u03BC\u03AD\u03BD\u03B7 \u03B3\u03BB\u03CE\u03C3\u03C3\u03B1 &\u03B5\u03B9\u03C3\u03CC\u03B4\u03BF\u03C5",
    L"\u0395\u03C0\u03B9\u03BB\u03AD\u03BE\u03C4\u03B5 \u03BC\u03AF\u03B1 \u03B1\u03C0\u03CC \u03C4\u03B9\u03C2 \u03B5\u03B3\u03BA\u03B1\u03C4\u03B5\u03C3\u03C4\u03B7\u03BC\u03AD\u03BD\u03B5\u03C2 \u03B3\u03BB\u03CE\u03C3\u03C3\u03B5\u03C2 \u03B5\u03B9\u03C3\u03CC\u03B4\u03BF\u03C5 \u03B3\u03B9\u03B1 \u03C7\u03C1\u03AE\u03C3\u03B7 \u03C9\u03C2 \u03C0\u03C1\u03BF\u03B5\u03C0\u03B9\u03BB\u03BF\u03B3\u03AE \u03C3\u03B5 \u03CC\u03BB\u03B1 \u03C4\u03B1 \u03C0\u03B5\u03B4\u03AF\u03B1 \u03B5\u03B9\u03C3\u03CC\u03B4\u03BF\u03C5.",
    L"\u0395\u03B3\u03BA\u03B1&\u03C4\u03B5\u03C3\u03C4\u03B7\u03BC\u03AD\u03BD\u03B5\u03C2 \u03C5\u03C0\u03B7\u03C1\u03B5\u03C3\u03AF\u03B5\u03C2",
    L"\u0395\u03C0\u03B9\u03BB\u03AD\u03BE\u03C4\u03B5 \u03C4\u03B9\u03C2 \u03C5\u03C0\u03B7\u03C1\u03B5\u03C3\u03AF\u03B5\u03C2 \u03C0\u03BF\u03C5 \u03B8\u03AD\u03BB\u03B5\u03C4\u03B5 \u03B3\u03B9\u03B1 \u03BA\u03AC\u03B8\u03B5 \u03B3\u03BB\u03CE\u03C3\u03C3\u03B1 \u03B5\u03B9\u03C3\u03CC\u03B4\u03BF\u03C5 \u03C3\u03C4\u03B7 \u03BB\u03AF\u03C3\u03C4\u03B1. \u03A7\u03C1\u03B7\u03C3\u03B9\u03BC\u03BF\u03C0\u03BF\u03B9\u03AE\u03C3\u03C4\u03B5 \u03C4\u03B1 \u03BA\u03BF\u03C5\u03BC\u03C0\u03B9\u03AC \u03A0\u03C1\u03BF\u03C3\u03B8\u03AE\u03BA\u03B7 \u03BA\u03B1\u03B9 \u039A\u03B1\u03C4\u03AC\u03C1\u03B3\u03B7\u03C3\u03B7 \u03B3\u03B9\u03B1 \u03BD\u03B1 \u03C4\u03C1\u03BF\u03C0\u03BF\u03C0\u03BF\u03B9\u03AE\u03C3\u03B5\u03C4\u03B5 \u03C4\u03B7 \u03BB\u03AF\u03C3\u03C4\u03B1.",
    L"\u03A0\u03C1\u03BF\u03C3&\u03B8\u03AE\u03BA\u03B7...",
    L"&\u039A\u03B1\u03C4\u03AC\u03C1\u03B3\u03B7\u03C3\u03B7",
    L"&\u0399\u03B4\u03B9\u03CC\u03C4\u03B7\u03C4\u03B5\u03C2...",
    L"\u039C\u03B5\u03C4\u03B1\u03BA\u03AF\u03BD\u03B7\u03C3\u03B7 \u03B5&\u03C0\u03AC\u03BD\u03C9",
    L"\u039C\u03B5\u03C4\u03B1\u03BA\u03AF\u03BD&\u03B7\u03C3\u03B7 \u03BA\u03AC\u03C4\u03C9",
    L"\u03A0\u03C1\u03BF\u03B5\u03C0\u03B9\u03C3\u03BA\u03CC\u03C0\u03B7\u03C3\u03B7 \u03B4\u03B9\u03AC\u03C4\u03B1\u03BE\u03B7\u03C2 \u03C0\u03BB\u03B7\u03BA\u03C4\u03C1\u03BF\u03BB\u03BF\u03B3\u03AF\u03BF\u03C5",
    L"\u038C\u03BD\u03BF\u03BC\u03B1 \u03B4\u03B9\u03AC\u03C4\u03B1\u03BE\u03B7\u03C2:",
    L"\u0391&\u03BB\u03BB\u03B1\u03B3\u03AE \u03B5\u03B9\u03BA\u03BF\u03BD\u03B9\u03B4\u03AF\u03BF\u03C5...",
    L"\u039D\u03B1 \u03BC\u03B7\u03BD \u03B5\u03BC\u03C6\u03B1\u03BD\u03B9\u03C3\u03C4\u03B5\u03AF \u03BE\u03B1\u03BD\u03AC \u03B1\u03C5\u03C4\u03CC \u03C4\u03BF &\u03BC\u03AE\u03BD\u03C5\u03BC\u03B1.",
};

static const wchar_t* const kInpDlgTr_AR[47] = {
    L"\u0645\u0648\u0627\u0641\u0642",
    L"\u0625\u0644\u063A\u0627\u0621 \u0627\u0644\u0623\u0645\u0631",
    L"\u0625\u0636\u0627\u0641\u0629 \u0644\u063A\u0629 \u0625\u062F\u062E\u0627\u0644",
    L"\u062D\u062F\u062F \u0627\u0644\u0644\u063A\u0629 \u0627\u0644\u062A\u064A \u062A\u0631\u064A\u062F \u0625\u0636\u0627\u0641\u062A\u0647\u0627 \u0628\u0627\u0633\u062A\u062E\u062F\u0627\u0645 \u062E\u0627\u0646\u0627\u062A \u0627\u0644\u0627\u062E\u062A\u064A\u0627\u0631 \u0623\u062F\u0646\u0627\u0647.",
    L"&\u0645\u0639\u0627\u064A\u0646\u0629...",
    L"\u0634\u0631\u064A\u0637 \u0627\u0644\u0644\u063A\u0629",
    L"&\u0639\u0627\u0626\u0645 \u0639\u0644\u0649 \u0633\u0637\u062D \u0627\u0644\u0645\u0643\u062A\u0628",
    L"\u0645\u062B\u0628\u062A \u0641\u064A &\u0634\u0631\u064A\u0637 \u0627\u0644\u0645\u0647\u0627\u0645",
    L"&\u0645\u062E\u0641\u064A",
    L"\u0625\u0638\u0647\u0627\u0631 \u0634\u0631\u064A\u0637 \u0627\u0644\u0644\u063A\u0629 \u0634\u0641\u0627\u0641\u064B\u0627 \u0639\u0646\u062F\u0645\u0627 \u064A\u0643\u0648\u0646 \u063A\u064A\u0631 &\u0646\u0634\u0637",
    L"\u0625\u0638\u0647\u0627\u0631 \u0623\u064A\u0642\u0648\u0646\u0627\u062A \u0634\u0631\u064A\u0637 \u0627\u0644\u0644\u063A\u0629 \u0627\u0644\u0625&\u0636\u0627\u0641\u064A\u0629 \u0641\u064A \u0634\u0631\u064A\u0637 \u0627\u0644\u0645\u0647\u0627\u0645",
    L"&\u062A\u0633\u0645\u064A\u0627\u062A \u0627\u0644\u0646\u0635 \u0639\u0644\u0649 \u0634\u0631\u064A\u0637 \u0627\u0644\u0644\u063A\u0629",
    L"\u0625\u0639\u062F\u0627\u062F\u0627\u062A \u0627\u0644\u0645\u0641\u0627\u062A\u064A\u062D \u0627\u0644\u0645\u062A\u0642\u062F\u0645\u0629",
    L"\u0644\u0625\u064A\u0642\u0627\u0641 \u062A\u0634\u063A\u064A\u0644 Caps Lock",
    L"\u0627\u0636\u063A\u0637 \u0639\u0644\u0649 \u0645\u0641\u062A\u0627\u062D CAPS &LOCK",
    L"\u0627\u0636\u063A\u0637 \u0639\u0644\u0649 \u0645\u0641\u062A\u0627\u062D SHI&FT",
    L"\u0645\u0641\u0627\u062A\u064A\u062D \u0627\u0644\u062A\u0634\u063A\u064A\u0644 \u0627\u0644\u0633\u0631\u064A\u0639 \u0644\u0644\u063A\u0627\u062A \u0627\u0644\u0625\u062F\u062E\u0627\u0644",
    L"\u0627\u0644\u0625\u062C\u0631\u0627\u0621",
    L"&\u062A\u0633\u0644\u0633\u0644 \u0627\u0644\u0645\u0641\u0627\u062A\u064A\u062D",
    L"\u062A\u063A&\u064A\u064A\u0631 \u062A\u0633\u0644\u0633\u0644 \u0627\u0644\u0645\u0641\u0627\u062A\u064A\u062D...",
    L"\u062A\u063A\u064A\u064A\u0631 \u062A\u0633\u0644\u0633\u0644 \u0627\u0644\u0645\u0641\u0627\u062A\u064A\u062D",
    L"\u062A\u0628\u062F\u064A\u0644 \u0644\u063A\u0629 \u0627\u0644\u0625\u062F\u062E\u0627\u0644",
    L"&\u063A\u064A\u0631 \u0645\u0639\u064A\u0651\u0646",
    L"&Ctrl + Shift",
    L"&\u200EAlt \u0627\u0644\u0623\u064A\u0633\u0631 + Shift",
    L"\u0639\u0644\u0627\u0645\u0629 \u0627\u0644\u0646\u0628\u0631 (`)",
    L"\u062A\u0628\u062F\u064A\u0644 \u062A\u062E\u0637\u064A\u0637 \u0644\u0648\u062D\u0629 \u0627\u0644\u0645\u0641\u0627\u062A\u064A\u062D",
    L"\u063A&\u064A\u0631 \u0645\u0639\u064A\u0651\u0646",
    L"C&trl + Shift",
    L"Alt \u0627\u0644\u0623\u064A\u0633\u0631 + Shift",
    L"\u0639\u0644\u0627\u0645\u0629 \u0627\u0644\u0646&\u0628\u0631 (`)",
    L"&\u062A\u0645\u0643\u064A\u0646 \u062A\u0633\u0644\u0633\u0644 \u0627\u0644\u0645\u0641\u0627\u062A\u064A\u062D",
    L"&\u0627\u0644\u0645\u0641\u062A\u0627\u062D:",
    L"\u0639\u0627\u0645",
    L"&\u0644\u063A\u0629 \u0627\u0644\u0625\u062F\u062E\u0627\u0644 \u0627\u0644\u0627\u0641\u062A\u0631\u0627\u0636\u064A\u0629",
    L"\u062D\u062F\u062F \u0625\u062D\u062F\u0649 \u0644\u063A\u0627\u062A \u0627\u0644\u0625\u062F\u062E\u0627\u0644 \u0627\u0644\u0645\u062B\u0628\u062A\u0629 \u0644\u0627\u0633\u062A\u062E\u062F\u0627\u0645\u0647\u0627 \u0643\u0644\u063A\u0629 \u0627\u0641\u062A\u0631\u0627\u0636\u064A\u0629 \u0641\u064A \u062C\u0645\u064A\u0639 \u062D\u0642\u0648\u0644 \u0627\u0644\u0625\u062F\u062E\u0627\u0644.",
    L"&\u0627\u0644\u062E\u062F\u0645\u0627\u062A \u0627\u0644\u0645\u062B\u0628\u062A\u0629",
    L"\u062D\u062F\u062F \u0627\u0644\u062E\u062F\u0645\u0627\u062A \u0627\u0644\u0645\u0637\u0644\u0648\u0628\u0629 \u0644\u0643\u0644 \u0644\u063A\u0629 \u0625\u062F\u062E\u0627\u0644 \u0645\u0639\u0631\u0648\u0636\u0629 \u0641\u064A \u0627\u0644\u0642\u0627\u0626\u0645\u0629. \u0627\u0633\u062A\u062E\u062F\u0645 \u0632\u0631\u064A \u0625\u0636\u0627\u0641\u0629 \u0648\u0625\u0632\u0627\u0644\u0629 \u0644\u062A\u0639\u062F\u064A\u0644 \u0647\u0630\u0647 \u0627\u0644\u0642\u0627\u0626\u0645\u0629.",
    L"&\u0625\u0636\u0627\u0641\u0629...",
    L"\u0625&\u0632\u0627\u0644\u0629",
    L"&\u062E\u0635\u0627\u0626\u0635...",
    L"\u062A\u062D\u0631\u064A\u0643 \u0644\u0623&\u0639\u0644\u0649",
    L"\u062A\u062D\u0631\u064A\u0643 \u0644\u0623\u0633&\u0641\u0644",
    L"\u0645\u0639\u0627\u064A\u0646\u0629 \u062A\u062E\u0637\u064A\u0637 \u0644\u0648\u062D\u0629 \u0627\u0644\u0645\u0641\u0627\u062A\u064A\u062D",
    L"\u0627\u0633\u0645 \u0627\u0644\u062A\u062E\u0637\u064A\u0637:",
    L"&\u062A\u063A\u064A\u064A\u0631 \u0627\u0644\u0623\u064A\u0642\u0648\u0646\u0629...",
    L"&\u0639\u062F\u0645 \u0625\u0638\u0647\u0627\u0631 \u0647\u0630\u0647 \u0627\u0644\u0631\u0633\u0627\u0644\u0629 \u0645\u0631\u0629 \u0623\u062E\u0631\u0649.",
};

static const wchar_t* const* const kInpDlgMasters[20] = {
    nullptr,
    kInpDlgTr_IT,
    kInpDlgTr_DE,
    kInpDlgTr_FR,
    kInpDlgTr_ES,
    kInpDlgTr_PT,
    kInpDlgTr_NL,
    kInpDlgTr_PL,
    kInpDlgTr_RU,
    kInpDlgTr_ZH,
    kInpDlgTr_JA,
    kInpDlgTr_KO,
    kInpDlgTr_TR,
    kInpDlgTr_CS,
    kInpDlgTr_HU,
    kInpDlgTr_RO,
    kInpDlgTr_SV,
    kInpDlgTr_UK,
    kInpDlgTr_EL,
    kInpDlgTr_AR,
};


// ===== Embedded-resource builders 
void PutU16(std::vector<BYTE>& out, WORD v) {
    out.push_back(static_cast<BYTE>(v)); out.push_back(static_cast<BYTE>(v >> 8));
}
void PutU32(std::vector<BYTE>& out, DWORD v) {
    out.push_back(static_cast<BYTE>(v)); out.push_back(static_cast<BYTE>(v >> 8));
    out.push_back(static_cast<BYTE>(v >> 16)); out.push_back(static_cast<BYTE>(v >> 24));
}
void PutSzW(std::vector<BYTE>& out, const wchar_t* s) {
    for (const wchar_t* p = s ? s : L""; ; ++p) { PutU16(out, static_cast<WORD>(*p)); if (!*p) break; }
}
void PutField(std::vector<BYTE>& out, WORD ord, const wchar_t* text) {
    if (ord == 0xFFFF) PutSzW(out, text);
    else if (ord == 0xFFFE) PutU16(out, 0);
    else { PutU16(out, 0xFFFF); PutU16(out, ord); }
}
bool BuildDlgTemplate(const EmbDlg& d, std::vector<BYTE>& out) {
    out.clear(); out.reserve(d.expectSize);
    // Dialog index into kDlgPhrMaps/kTitleMasters (kEmbDialogs order 101, 102,
    // 104-109, 600, 700, 800); -1 disables substitution (genuine English).
    int dlgIdx = static_cast<int>(&d - kEmbDialogs);
    if (dlgIdx < 0 || dlgIdx >= 11 || &kEmbDialogs[dlgIdx] != &d) dlgIdx = -1;
    const bool tr = g_lang > LangEN && g_lang < LangCount && dlgIdx >= 0;
    PutU16(out, 1); PutU16(out, 0xFFFF);
    DWORD exStyle = d.exStyle;
    if (tr && LangRtl(g_lang)) exStyle |= WS_EX_LAYOUTRTL; // Arabic mirroring
    PutU32(out, d.helpId); PutU32(out, exStyle); PutU32(out, d.style);
    PutU16(out, d.count);
    PutU16(out, static_cast<WORD>(d.x)); PutU16(out, static_cast<WORD>(d.y));
    PutU16(out, static_cast<WORD>(d.cx)); PutU16(out, static_cast<WORD>(d.cy));
    PutU16(out, 0); // menu: none (verified for all 11 dialogs)
    PutU16(out, 0); // class: none (verified for all 11 dialogs)
    const wchar_t* title = d.title;
    if (tr) {
        const wchar_t* tt = kTitleMasters[g_lang][dlgIdx];
        if (d.id == 102) tt = kInputLanguagesTitle[g_lang];
        if (tt) title = tt;
    } else if (d.id == 102) {
        title = kInputLanguagesTitle[LangEN];
    }
    PutSzW(out, title);
    if (d.style & DS_SETFONT) {
        PutU16(out, d.fontPt); PutU16(out, d.fontWeight);
        out.push_back(d.fontItalic); out.push_back(d.fontCharset);
        PutSzW(out, d.typeface);
    }
    for (WORD i = 0; i < d.count; ++i) {
        const EmbCtl& c = d.ctls[i];
        while (out.size() % 4) out.push_back(0);
        PutU32(out, c.helpId); PutU32(out, c.exStyle); PutU32(out, c.style);
        PutU16(out, static_cast<WORD>(c.x)); PutU16(out, static_cast<WORD>(c.y));
        PutU16(out, static_cast<WORD>(c.cx)); PutU16(out, static_cast<WORD>(c.cy));
        PutU32(out, c.id);
        PutField(out, c.clsOrd, c.clsText);
        const wchar_t* text = c.text;
        if (tr && c.textOrd == 0xFFFF) {
            const short p = kDlgPhrMaps[dlgIdx][i];
            if (p >= 0 && p < 92) {
                const wchar_t* t2 = kDlgMasters[g_lang][p];
                if (t2) text = t2;
            }
        }
        PutField(out, c.textOrd, text);
        PutU16(out, 0); // no creation data (verified for all 177 controls)
    }
    return true;
}
const wchar_t* EmbStringText(UINT id) {
    int slot = 0;
    for (const EmbString* s = kEmbStrings; s->text; ++s, ++slot) {
        if (s->id != id) continue;
        // Slot order parallels kStrTr_* (kMasterStrIds-verified); a nullptr
        // pack entry keeps the genuine English original.
        if (g_lang > LangEN && g_lang < LangCount && slot < 66) {
            const wchar_t* tr = kStrMasters[g_lang][slot];
            if (tr) return tr;
        }
        return s->text;
    }
    return nullptr;
}
bool BuildStringBlock(UINT block, std::vector<BYTE>& out) {
    out.clear();
    for (UINT i = 0; i < 16; ++i) {
        const wchar_t* text = EmbStringText((block - 1) * 16 + i);
        size_t n = text ? wcslen(text) : 0;
        if (n > 0xFFFF) return false;
        PutU16(out, static_cast<WORD>(n));
        for (size_t k = 0; k < n; ++k) PutU16(out, static_cast<WORD>(text[k]));
    }
    return true;
}
DWORD Fnv1a(const std::vector<BYTE>& data) {
    DWORD h = 0x811C9DC5;
    for (BYTE b : data) { h ^= b; h *= 0x01000193; }
    return h;
}
// Embedded blobs are addressed by small-int pseudo-HRSRCs that can never
// collide with real image pointers. LoadResource returns a pointer to the
// static Blob so LockResource/FreeResource can verify membership exactly.
HRSRC EncodeBlob(DWORD i) { return reinterpret_cast<HRSRC>(static_cast<ULONG_PTR>(0x1000 + i)); }
bool DecodeBlob(HRSRC h, DWORD& i) {
    ULONG_PTR v = reinterpret_cast<ULONG_PTR>(h);
    if (v >= 0x1000 && v < 0x1000 + g_blobCount) { i = static_cast<DWORD>(v - 0x1000); return true; }
    return false;
}
bool BuildEmbeddedResources() {
    for (auto& v : g_blobForDialog) v = -1;
    for (auto& v : g_blobForStrBlock) v = -1;
    g_blobCount = 0;
    for (const EmbDlg* d = kEmbDialogs; d->id; ++d) {
        if (g_blobCount >= ARRAYSIZE(g_blobs)) return Fail(L"Embedded blob table overflow", ERROR_INSUFFICIENT_BUFFER);
        if (!BuildDlgTemplate(*d, g_blobStore[g_blobCount])) return Fail(L"Build dialog template", ERROR_BAD_FORMAT);
        if (g_lang == LangEN) {
            if (g_blobStore[g_blobCount].size() != d->expectSize ||
                Fnv1a(g_blobStore[g_blobCount]) != d->expectFnv) {
                Wh_Log(L"Dialog %u rebuild mismatch: not serving a corrupt template", d->id);
                return Fail(L"Dialog template verification failed", ERROR_CRC);
            }
        } else if (g_blobStore[g_blobCount].empty()) {
            return Fail(L"Translated dialog template is empty", ERROR_BAD_FORMAT);
        }
        g_blobs[g_blobCount] = {g_blobStore[g_blobCount].data(), static_cast<DWORD>(g_blobStore[g_blobCount].size())};
        if (d->id < ARRAYSIZE(g_blobForDialog)) g_blobForDialog[d->id] = static_cast<int>(g_blobCount);
        ++g_blobCount;
    }
    for (const EmbStrBlock* b = kEmbStrBlocks; b->block; ++b) {
        if (g_blobCount >= ARRAYSIZE(g_blobs)) return Fail(L"Embedded blob table overflow", ERROR_INSUFFICIENT_BUFFER);
        if (!BuildStringBlock(b->block, g_blobStore[g_blobCount])) return Fail(L"Build string block", ERROR_BAD_FORMAT);
        if (g_lang == LangEN) {
            if (g_blobStore[g_blobCount].size() != b->size) {
                Wh_Log(L"String block %u size mismatch", b->block);
                return Fail(L"String block verification failed", ERROR_CRC);
            }
        } else if (g_blobStore[g_blobCount].empty()) {
            return Fail(L"Translated string block is empty", ERROR_BAD_FORMAT);
        }
        g_blobs[g_blobCount] = {g_blobStore[g_blobCount].data(), static_cast<DWORD>(g_blobStore[g_blobCount].size())};
        if (b->block < ARRAYSIZE(g_blobForStrBlock)) g_blobForStrBlock[b->block] = static_cast<int>(g_blobCount);
        ++g_blobCount;
    }
    Wh_Log(L"Embedded resources ready: 11 dialogs + string blocks (%lu blobs) language=%s", g_blobCount, kLangTags[g_lang]);
    return true;
}
const BYTE* EmbeddedDlgTemplate(WORD id, DWORD& size) {
    if (id < ARRAYSIZE(g_blobForDialog) && g_blobForDialog[id] >= 0) {
        size = g_blobs[g_blobForDialog[id]].size;
        return g_blobs[g_blobForDialog[id]].data;
    }
    return nullptr;
}

// ===== Embedded input.dll resource builders (Text Services translation) =====
// Mirrors the intl.cpl builders above: templates are rebuilt from the
// verified kInpDialogs tables and localized with the kInpDlgMasters packs.
// The seven EX dialogs rebuild byte-identical to the pinned file's own
// resources (expectSize/expectFnv prove it at English); 112/113/114 are
// classic templates canonically rebuilt in EX form, which the dialog
// manager detects by signature.
bool BuildInputDlgTemplate(const EmbDlg& d, std::vector<BYTE>& out) {
    out.clear(); out.reserve(d.expectSize);
    const int dlgIdx = static_cast<int>(&d - kInpDialogs);
    if (dlgIdx < 0 || dlgIdx >= 10 || &kInpDialogs[dlgIdx] != &d) return false;
    const bool tr = g_lang > LangEN && g_lang < LangCount;
    PutU16(out, 1); PutU16(out, 0xFFFF);
    DWORD exStyle = d.exStyle;
    if (tr && LangRtl(g_lang)) exStyle |= WS_EX_LAYOUTRTL; // Arabic mirroring
    PutU32(out, d.helpId); PutU32(out, exStyle); PutU32(out, d.style);
    PutU16(out, d.count);
    PutU16(out, static_cast<WORD>(d.x)); PutU16(out, static_cast<WORD>(d.y));
    PutU16(out, static_cast<WORD>(d.cx)); PutU16(out, static_cast<WORD>(d.cy));
    PutU16(out, 0); // menu: none (verified for all 10 input dialogs)
    PutU16(out, 0); // class: none (verified for all 10 input dialogs)
    const wchar_t* title = d.title;
    if (dlgIdx >= 7) {
        // 112/113/114 carry the sheet title; reuse the localized title pack.
        if (tr) title = kInputLanguagesTitle[g_lang];
    } else if (tr) {
        const wchar_t* tt = kInpDlgMasters[g_lang][kInpTitlePhrase[dlgIdx]];
        if (tt) title = tt;
    }
    PutSzW(out, title);
    if (d.style & DS_SETFONT) {
        PutU16(out, d.fontPt); PutU16(out, d.fontWeight);
        out.push_back(d.fontItalic); out.push_back(d.fontCharset);
        PutSzW(out, d.typeface);
    }
    for (WORD i = 0; i < d.count; ++i) {
        const EmbCtl& c = d.ctls[i];
        while (out.size() % 4) out.push_back(0);
        PutU32(out, c.helpId); PutU32(out, c.exStyle); PutU32(out, c.style);
        PutU16(out, static_cast<WORD>(c.x)); PutU16(out, static_cast<WORD>(c.y));
        PutU16(out, static_cast<WORD>(c.cx)); PutU16(out, static_cast<WORD>(c.cy));
        PutU32(out, c.id);
        PutField(out, c.clsOrd, c.clsText);
        const wchar_t* text = c.text;
        if (tr && c.textOrd == 0xFFFF) {
            const short p = kInpPhrMaps[dlgIdx][i];
            if (p >= 0 && p < 47) {
                const wchar_t* t2 = kInpDlgMasters[g_lang][p];
                if (t2) text = t2;
            }
        }
        PutField(out, c.textOrd, text);
        PutU16(out, 0); // no creation data (verified for all input controls)
    }
    return true;
}
bool BuildEmbeddedInputResources() {
    for (auto& v : g_inpBlobForDialog) v = -1;
    g_inpBlobCount = 0;
    for (const EmbDlg* d = kInpDialogs; d->id; ++d) {
        if (g_inpBlobCount >= ARRAYSIZE(g_inpBlobs)) return Fail(L"Input blob table overflow", ERROR_INSUFFICIENT_BUFFER);
        if (!BuildInputDlgTemplate(*d, g_inpBlobStore[g_inpBlobCount])) return Fail(L"Build input dialog template", ERROR_BAD_FORMAT);
        if (g_lang == LangEN) {
            // English rebuilds must be byte-identical to the pinned input.dll
            // resources (112/113/114: identical to the canonical EX rebuild).
            if (g_inpBlobStore[g_inpBlobCount].size() != d->expectSize ||
                Fnv1a(g_inpBlobStore[g_inpBlobCount]) != d->expectFnv) {
                Wh_Log(L"Input dialog %u rebuild mismatch: not serving a corrupt template", d->id);
                return Fail(L"Input dialog template verification failed", ERROR_CRC);
            }
        } else if (g_inpBlobStore[g_inpBlobCount].empty()) {
            return Fail(L"Translated input dialog template is empty", ERROR_BAD_FORMAT);
        }
        g_inpBlobs[g_inpBlobCount] = {g_inpBlobStore[g_inpBlobCount].data(), static_cast<DWORD>(g_inpBlobStore[g_inpBlobCount].size())};
        if (d->id < ARRAYSIZE(g_inpBlobForDialog)) g_inpBlobForDialog[d->id] = static_cast<int>(g_inpBlobCount);
        ++g_inpBlobCount;
    }
    Wh_Log(L"Embedded input resources ready: 10 Text Services dialogs (%lu blobs) language=%s", g_inpBlobCount, kLangTags[g_lang]);
    return true;
}
const BYTE* EmbeddedInpDlgTemplate(WORD id, DWORD& size) {
    if (id < ARRAYSIZE(g_inpBlobForDialog) && g_inpBlobForDialog[id] >= 0) {
        size = g_inpBlobs[g_inpBlobForDialog[id]].size;
        return g_inpBlobs[g_inpBlobForDialog[id]].data;
    }
    return nullptr;
}

// ===== Private IAT overrides (only the private intl.cpl's IAT reaches these) =====
HRSRC WINAPI PrivateFindResourceW(HMODULE module, LPCWSTR name, LPCWSTR type) {
    try {
        // Evidence: FindResourceW(SCHEMAS, IDS_UNATTEND) at 0xBBD8 uses the global
        // hInstance; dialogs/strings are served from embedded genuine data, every
        // other type from the neutral bytes in the manual map (FindResource parses
        // any valid mapped image: ICON/VERSION/MANIFEST/SCHEMAS/WEVT all present).
        if (!IsMain(module)) return FindResourceW(module, name, type);
        if (IS_INTRESOURCE(type) && IS_INTRESOURCE(name)) {
            if (type == RT_DIALOG) {
                WORD id = LOWORD(name);
                if (id < ARRAYSIZE(g_blobForDialog) && g_blobForDialog[id] >= 0)
                    return EncodeBlob(static_cast<DWORD>(g_blobForDialog[id]));
            } else if (type == RT_STRING) {
                WORD block = LOWORD(name);
                if (block < ARRAYSIZE(g_blobForStrBlock) && g_blobForStrBlock[block] >= 0)
                    return EncodeBlob(static_cast<DWORD>(g_blobForStrBlock[block]));
            } else {
                HRSRC real = FindResourceW(reinterpret_cast<HMODULE>(g_image.base), name, type);
                if (real) return real;
            }
        } else {
            HRSRC real = FindResourceW(reinterpret_cast<HMODULE>(g_image.base), name, type);
            if (real) return real;
        }
        SetLastError(ERROR_RESOURCE_NAME_NOT_FOUND); return nullptr;

    } catch (...) {
        SetLastError(ERROR_NOT_ENOUGH_MEMORY); return nullptr;
    }
}
HRSRC WINAPI PrivateFindResourceExW(HMODULE module, LPCWSTR type, LPCWSTR name, WORD language) {
    try {
        // The private binary imports FindResourceExW but only ever needs the one
        // embedded build (genuine en-US, or the selected translation pack); it is
        // served for any requested language instead of failing LANG_NOT_FOUND.
        if (!IsMain(module)) return FindResourceExW(module, type, name, language);
        (void)language;
        return PrivateFindResourceW(module, name, type);

    } catch (...) {
        SetLastError(ERROR_NOT_ENOUGH_MEMORY); return nullptr;
    }
}
HGLOBAL WINAPI PrivateLoadResource(HMODULE module, HRSRC resource) {
    try {
        if (!IsMain(module)) return LoadResource(module, resource);
        DWORD i = 0;
        if (DecodeBlob(resource, i)) return reinterpret_cast<HGLOBAL>(&g_blobs[i]);
        return LoadResource(reinterpret_cast<HMODULE>(g_image.base), resource);

    } catch (...) {
        SetLastError(ERROR_NOT_ENOUGH_MEMORY); return nullptr;
    }
}
LPVOID WINAPI PrivateLockResource(HGLOBAL data) {
    try {
        // LockResource takes no module: disambiguate by exact pointer membership.
        for (DWORD i = 0; i < g_blobCount; ++i)
            if (data == reinterpret_cast<HGLOBAL>(&g_blobs[i]))
                return const_cast<LPVOID>(reinterpret_cast<LPCVOID>(g_blobs[i].data));
        return LockResource(data);

    } catch (...) {
        SetLastError(ERROR_NOT_ENOUGH_MEMORY); return nullptr;
    }
}
BOOL WINAPI PrivateFreeResource(HGLOBAL data) {
    try {
        for (DWORD i = 0; i < g_blobCount; ++i)
            if (data == reinterpret_cast<HGLOBAL>(&g_blobs[i])) return TRUE;
        return FreeResource(data);

    } catch (...) {
        return FALSE;
    }
}
int WINAPI PrivateLoadStringW(HINSTANCE module, UINT id, LPWSTR output, int size) {
    try {
        // Evidence: 35 static LoadStringW sites, all IDs covered by the embedded
        // table except genuinely-missing ones (e.g. 85), which return 0 exactly
        // like the real MUI-backed lookup does.
        if (!IsMain(module)) return LoadStringW(module, id, output, size);
        const wchar_t* text = EmbStringText(id);
        if (!text) return 0;
        size_t n = wcslen(text);
        if (size == 0) { // Pointer form: static lifetime, valid until unload.
            if (output) *reinterpret_cast<const wchar_t**>(output) = text;
            return static_cast<int>(n);
        }
        if (!output || size < 0) { SetLastError(ERROR_INVALID_PARAMETER); return 0; }
        size_t copy = std::min(n, static_cast<size_t>(size - 1));
        memcpy(output, text, copy * sizeof(wchar_t)); output[copy] = 0;
        return static_cast<int>(copy);

    } catch (...) {
        return 0;
    }
}
DWORD WINAPI PrivateGetModuleFileNameW(HMODULE module, LPWSTR buffer, DWORD chars) {
    try {
        if (!IsMain(module)) return GetModuleFileNameW(module, buffer, chars);
        if (!buffer || !chars) { SetLastError(ERROR_INSUFFICIENT_BUFFER); return 0; }
        size_t copy = std::min(g_intlPath.size(), static_cast<size_t>(chars - 1));
        memcpy(buffer, g_intlPath.data(), copy * sizeof(wchar_t)); buffer[copy] = 0;
        if (g_intlPath.size() >= chars) { SetLastError(ERROR_INSUFFICIENT_BUFFER); return chars; }
        return static_cast<DWORD>(copy);

    } catch (...) {
        return GetModuleFileNameW(module, buffer, chars);
    }
}
HMODULE WINAPI PrivateGetModuleHandleW(LPCWSTR name) {
    try {
    if (name && (Equal(name, L"intl.cpl") || Equal(Slashes(name), g_intlPath)))
        return reinterpret_cast<HMODULE>(g_image.base);
    if (name && (Equal(name, L"input.dll") || Equal(Slashes(name), g_inputPath))) return g_inputModule;
    return GetModuleHandleW(name);
    } catch (...) { return GetModuleHandleW(name); } // our matching failed; real lookup stays valid
}
BOOL WINAPI PrivateFreeLibrary(HMODULE module) {
    try {
        // The private executable image is owned by this mod, never by the shell.
        if (IsMain(module)) { SetLastError(ERROR_INVALID_HANDLE); return FALSE; }
        if (module == g_inputModule) {
            // Balance a reference PrivateLoadLibraryW handed out (finding 6):
            // let the legacy code's own refcounting drop the extra reference
            // we took on its behalf, but keep the module itself resident
            // (Cleanup() owns the base reference and frees it once).
            LONG prior = g_inputExtraRefs.load(std::memory_order_acquire);
            while (prior > 0 && !g_inputExtraRefs.compare_exchange_weak(
                       prior, prior - 1, std::memory_order_acq_rel)) {}
            if (prior > 0) { FreeLibrary(module); return TRUE; }
        }
        return FreeLibrary(module);

    } catch (...) {
        return FreeLibrary(module);
    }
}
HANDLE WINAPI PrivateCreateActCtxW(PCACTCTXW input) {
    try {
        if (!input || !IsMain(input->hModule)) return CreateActCtxW(input);
        // Decompile ground truth (sub_8070C): legacy DllMain passes cbSize=56
        // with hModule=<own base>. Win7 honored cbSize (hModule sits at offset
        // 56, outside it) and read lpSource; Win10 rejects the unbacked handle
        // instead, so hActCtx[0] stayed -1 and 80CDC stamped PSP_USEFUSIONCONTEXT
        // with an invalid context on every page -> CreatePropertySheetPage NULL.
        // Rebuild the call in the exact shape Prepare proves good (g_act): full
        // struct, module cleared, genuine manifest 123 from the pinned file.
        ACTCTXW fixed{};
        memcpy(&fixed, input, std::min<size_t>(input->cbSize, sizeof(fixed)));
        fixed.cbSize = sizeof(fixed);
        fixed.dwFlags &= ~ACTCTX_FLAG_HMODULE_VALID;
        fixed.lpSource = g_intlPath.c_str(); fixed.hModule = nullptr;
        HANDLE ctx = CreateActCtxW(&fixed);
        if (ctx && ctx != INVALID_HANDLE_VALUE)
            Wh_Log(L"Private Win7 fusion context active: hActCtx=%p (manifest 123, pinned file)", ctx);
        else Wh_Log(L"Private fusion context failed: Win32=%lu; pages fall back to host comctl32 v6", GetLastError());
        return ctx;

    } catch (...) {
        return CreateActCtxW(input);
    }
}
HMODULE WINAPI PrivateLoadLibraryW(LPCWSTR name) {
    try {
    // Evidence: LoadLibraryW(L"input.dll") at 0x1C05C (string at RVA 0x3EA0).
    // The Win10 system input.dll lacks ordinals 104-114, so this MUST resolve
    // to the private Win7 provider. Every other name loads from System32 only.
    if (!name) { SetLastError(ERROR_INVALID_PARAMETER); return nullptr; }
    const std::wstring requested = Slashes(name);
    const size_t slash = requested.rfind(L'\\');
    const auto leaf = requested.substr(slash == std::wstring::npos ? 0 : slash + 1);
    if (Equal(leaf, L"input.dll")) {
        if (g_stopping.load()) { SetLastError(ERROR_SHUTDOWN_IN_PROGRESS); return nullptr; }
        HMODULE input = LoadLibraryExW(g_inputPath.c_str(), nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
        if (input != g_inputModule) {
            if (input) FreeLibrary(input);
            Wh_Log(L"Private input.dll identity mismatch");
            SetLastError(ERROR_INVALID_DLL); return nullptr;
        }
        // This reference is handed to the legacy code; track it so Cleanup()
        // can release it if the legacy code never balances it (finding 6).
        g_inputExtraRefs.fetch_add(1, std::memory_order_acq_rel);
        Wh_Log(L"Legacy intl.cpl requested input.dll -> private Windows 7 module %p", input);
        return input;
    }
    // Only calls made through the private CPL's IAT use this search restriction.
    return LoadLibraryExW(name, nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    } catch (...) { SetLastError(ERROR_NOT_ENOUGH_MEMORY); return nullptr; }
}

void Own(HWND window, bool add, bool sheet = false) {
    if (!window) return;
    AcquireSRWLockExclusive(&g_windowsLock);
    if (add) {
        bool exists = false;
        for (const auto& entry : g_owned) if (entry.hwnd == window) exists = true;
        if (!exists) {
            bool placed = false;
            for (auto& entry : g_owned) {
                if (!entry.hwnd) { entry.hwnd = window; entry.sheet = sheet; placed = true; break; }
            }
            if (!placed && g_owned.size() < kMaxOwnedWindows) g_owned.push_back({window, sheet});
        }
    } else {
        for (auto& entry : g_owned) if (entry.hwnd == window) { entry.hwnd = nullptr; entry.sheet = false; }
    }
    ReleaseSRWLockExclusive(&g_windowsLock);
}
// True while any window this mod serves is still alive. Used as the quiet-point
// test before rebuilding the embedded resource tables at runtime.
bool AnyOwnedWindowAlive() {
    AcquireSRWLockShared(&g_windowsLock);
    bool alive = false;
    for (const auto& entry : g_owned) if (entry.hwnd) { alive = true; break; }
    ReleaseSRWLockShared(&g_windowsLock);
    return alive;
}
struct SheetContext { PFNPROPSHEETCALLBACK original; HWND window; SheetContext* previous; bool isInput; };
thread_local SheetContext* g_sheet = nullptr;
int CALLBACK SheetCallback(HWND window, UINT message, LPARAM parameter) {
    try {
        SheetContext* context = g_sheet;
        // Register before the provider callback: at PSCB_INITIALIZED a window
        // already exists, even if the provider's callback subsequently fails.
if (context && message == PSCB_INITIALIZED) {
            context->window = window;
            if (context->isInput) TranslateInputWindow(window);
            Own(window, true, true); ++g_uiCreated;
            Wh_Log(L"Original Windows 7 property sheet initialized: HWND=%p", window);
            if (g_stopping.load()) PostMessageW(window, WM_CLOSE, 0, 0);
        }
        return context && context->original ? context->original(window, message, parameter) : 0;
    } catch (...) {
        // An OS-invoked callback must never propagate: the sheet proceeds
        // without our bookkeeping for this message.
        g_uiFailed = true;
        return 0;
    }
}
// Substitute int-resources the private module cannot resolve through the OS
// loader (manual map has no MUI redirection): template -> indirect bytes,
// title/header strings -> embedded text pointers. Icons stay on the manual
// map (neutral file genuinely contains them).
bool IndirectPage(PROPSHEETPAGEW& page) {
    try {
        if (!(page.dwFlags & PSP_DLGINDIRECT)) {
            if (!IS_INTRESOURCE(page.pszTemplate)) { SetLastError(ERROR_RESOURCE_NAME_NOT_FOUND); return false; }
            DWORD size = 0;
            const BYTE* tpl = EmbeddedDlgTemplate(LOWORD(page.pszTemplate), size);
            if (!tpl || !size) {
                Wh_Log(L"Unknown private dialog template %u", LOWORD(page.pszTemplate));
                SetLastError(ERROR_RESOURCE_NAME_NOT_FOUND); return false;
            }
            page.dwFlags |= PSP_DLGINDIRECT;
            page.pResource = reinterpret_cast<LPCDLGTEMPLATE>(tpl);
        }
        // IS_INTRESOURCE(NULL) is true, but a NULL title/subtitle is legal (none).
        if (page.pszTitle && IS_INTRESOURCE(page.pszTitle)) {
            const wchar_t* text = EmbStringText(LOWORD(page.pszTitle));
            if (!text) { SetLastError(ERROR_RESOURCE_NAME_NOT_FOUND); return false; }
            page.pszTitle = text;
        }
        if ((page.dwFlags & PSP_USEHEADERTITLE) && page.pszHeaderTitle && IS_INTRESOURCE(page.pszHeaderTitle)) {
            const wchar_t* text = EmbStringText(LOWORD(page.pszHeaderTitle));
            if (!text) { SetLastError(ERROR_RESOURCE_NAME_NOT_FOUND); return false; }
            page.pszHeaderTitle = text;
        }
        if ((page.dwFlags & PSP_USEHEADERSUBTITLE) && page.pszHeaderSubTitle && IS_INTRESOURCE(page.pszHeaderSubTitle)) {
            const wchar_t* text = EmbStringText(LOWORD(page.pszHeaderSubTitle));
            if (!text) { SetLastError(ERROR_RESOURCE_NAME_NOT_FOUND); return false; }
            page.pszHeaderSubTitle = text;
        }
        return true;

    } catch (...) {
        return false;
    }
}
INT_PTR ShowSheet(PSProc original, LPCPROPSHEETHEADERW header, bool privateMain) {
    try {
    if (!original || !header || header->dwSize < offsetof(PROPSHEETHEADERW, pfnCallback) + sizeof(header->pfnCallback) ||
        header->dwSize > 512 || (header->dwFlags & PSH_MODELESS)) {
        Wh_Log(L"ShowSheet: REJECTED dwSize=%lu flags=0x%lX sizeofHdr=%u",
               header ? header->dwSize : 0, header ? header->dwFlags : 0, (unsigned)sizeof(PROPSHEETHEADERW));
        g_uiFailed = true; SetLastError(ERROR_NOT_SUPPORTED); return -1;
    }
    PROPSHEETHEADERW copy{};
    const size_t hwire = std::min<size_t>(header->dwSize, sizeof(copy));
    memcpy(&copy, header, hwire); copy.dwSize = static_cast<DWORD>(hwire);
    Wh_Log(L"%s sheet: nPages=%u flags=0x%lX", privateMain ? L"Win7 Region" : L"Win7 Input", copy.nPages, copy.dwFlags);
    std::vector<PROPSHEETPAGEW> converted; // Outlives the modal call below.
    if (privateMain && IsMain(copy.hInstance)) {
        if (copy.pszCaption && IS_INTRESOURCE(copy.pszCaption)) {
            const wchar_t* text = EmbStringText(LOWORD(copy.pszCaption));
            if (!text) { g_uiFailed = true; SetLastError(ERROR_RESOURCE_NAME_NOT_FOUND); return -1; }
            copy.pszCaption = text;
        }
        if (copy.dwFlags & PSH_PROPSHEETPAGE) {
            if (!copy.ppsp || copy.nPages == 0 || copy.nPages > 64) {
                g_uiFailed = true; SetLastError(ERROR_INVALID_PARAMETER); return -1;
            }
            converted.reserve(copy.nPages);
            for (UINT i = 0; i < copy.nPages; ++i) {
                PROPSHEETPAGEW pg{}; memcpy(&pg, &copy.ppsp[i], std::min<size_t>(copy.ppsp[i].dwSize, sizeof(pg)));
                pg.dwSize = static_cast<DWORD>(std::min<size_t>(copy.ppsp[i].dwSize, sizeof(pg)));
                if (!IndirectPage(pg)) { g_uiFailed = true; return -1; }
                converted.push_back(pg);
            }
            copy.ppsp = converted.data();
        }
    } else if (g_lang != LangEN && copy.hInstance == g_inputModule && copy.pszCaption && IS_INTRESOURCE(copy.pszCaption)) {
        // Evidence: the Win7 input.dll sets the sheet caption as the int
        // resource 1 (disasm 0x11CAB stores r14=1 at header+0x20). Win10
        // comctl32 resolves it with its own LoadStringW, which the private
        // input.dll IAT hook cannot reach, so the caption is replaced here
        // before the original call.
        const wchar_t* text = InputText(LOWORD(copy.pszCaption));
        if (text) copy.pszCaption = text;
    }
    SheetContext context{(copy.dwFlags & PSH_USECALLBACK) ? copy.pfnCallback : nullptr, nullptr, g_sheet, !privateMain || copy.hInstance == g_inputModule};
    copy.dwFlags |= PSH_USECALLBACK; copy.pfnCallback = SheetCallback;
    g_sheet = &context;
    INT_PTR result = original(&copy);
    DWORD error = GetLastError();
    g_sheet = context.previous; Own(context.window, false);
    if (result == -1) { g_uiFailed = true; Wh_Log(L"PropertySheetW failed: Win32=%lu", error); }
    SetLastError(error); return result;
    } catch (...) { g_uiFailed = true; SetLastError(ERROR_NOT_ENOUGH_MEMORY); return -1; }
}
INT_PTR WINAPI MainPropertySheet(LPCPROPSHEETHEADERW header) { return ShowSheet(g_mainPS, header, true); }
INT_PTR WINAPI InputPropertySheet(LPCPROPSHEETHEADERW header) { return ShowSheet(g_inputPS, header, false); }
HPROPSHEETPAGE WINAPI MainCreatePage(LPCPROPSHEETPAGEW page) {
    try {
    // v0.3.3 log proved this function exits silently: entry data + the build
    // SDK's struct size are logged so the wire contract, not an SDK guess,
    // decides. A 96-byte SDK struct rejects legacy's 104-byte pages.
    Wh_Log(L"MainCreatePage: entry page=%p dwSize=%lu flags=0x%lX hInstance=%p isMain=%d tmpl=%p sizeofPage=%u",
           page, page ? page->dwSize : 0, page ? page->dwFlags : 0,
           page ? page->hInstance : nullptr, page ? (int)IsMain(page->hInstance) : 0,
           page ? page->pszTemplate : nullptr, (unsigned)sizeof(PROPSHEETPAGEW));
    if (!page || page->dwSize < offsetof(PROPSHEETPAGEW, lParam) + sizeof(page->lParam) || page->dwSize > 512) {
        Wh_Log(L"MainCreatePage: REJECTED dwSize=%lu (wire contract [56,512])", page ? page->dwSize : 0);
        SetLastError(ERROR_INVALID_PARAMETER); g_uiFailed = true; return nullptr;
    }
    PROPSHEETPAGEW copy{};
    const size_t wire = std::min<size_t>(page->dwSize, sizeof(copy));
    memcpy(&copy, page, wire); copy.dwSize = static_cast<DWORD>(wire);
    if (IsMain(copy.hInstance) && !IndirectPage(copy)) {
        Wh_Log(L"MainCreatePage: IndirectPage FAILED tmpl=%p", page->pszTemplate);
        g_uiFailed = true; SetLastError(ERROR_RESOURCE_NAME_NOT_FOUND); return nullptr;
    }
    // Decompile 80CDC: legacy sets PSP_USEFUSIONCONTEXT (0x4000) plus
    // hActCtx[0] at page+88 whenever dwSize > 0x58. The genuine context
    // survives PrivateCreateActCtxW above; only a poisoned marker (NULL or
    // -1, which Win7 tolerated and Win10 rejects) is cleared, so the host's
    // comctl32 v6 activation styles the page instead.
    constexpr DWORD kFusion = 0x4000u; // PSP_USEFUSIONCONTEXT (prsht.h)
    constexpr size_t kActCtxOff = 88;  // hActCtx offset in PROPSHEETPAGEW
    if (copy.dwSize > 0x58 && (copy.dwFlags & kFusion)) {
        HANDLE ctx = *reinterpret_cast<HANDLE*>(reinterpret_cast<BYTE*>(&copy) + kActCtxOff);
        if (!ctx || ctx == INVALID_HANDLE_VALUE) {
            copy.dwFlags &= ~kFusion;
            *reinterpret_cast<HANDLE*>(reinterpret_cast<BYTE*>(&copy) + kActCtxOff) = nullptr;
            Wh_Log(L"Cleared invalid Win7 fusion marker on a page (host comctl32 v6 stays active)");
        }
    }
    HPROPSHEETPAGE result = g_mainPage(&copy);
    if (!result) { g_uiFailed = true; Wh_Log(L"CreatePropertySheetPageW failed, error=%lu", GetLastError()); }
    else Wh_Log(L"Windows 7 page created: %p", result);
    return result;
    } catch (...) { Wh_Log(L"MainCreatePage: EXCEPTION swallowed"); g_uiFailed = true; SetLastError(ERROR_NOT_ENOUGH_MEMORY); return nullptr; }
}
// Text Services property-sheet pages: input.dll creates them through its
// own CreatePropertySheetPageW import with pszTemplate = MAKEINTRESOURCE
// (500/106/107) and hInstance = its own module handle (disasm 0x11DCC).
// The wrapper serves the translated indirect template exactly like
// IndirectPage does for the private intl.cpl; English and unexpected
// shapes pass the genuine call through untouched.
HPROPSHEETPAGE WINAPI InputCreatePage(LPCPROPSHEETPAGEW page) {
    try {
        if (!g_inputPage) return nullptr;
        if (!page || page->dwSize < offsetof(PROPSHEETPAGEW, lParam) + sizeof(page->lParam) || page->dwSize > 512)
            return g_inputPage(page);
        if (page->hInstance != g_inputModule || (page->dwFlags & PSP_DLGINDIRECT) || !IS_INTRESOURCE(page->pszTemplate))
            return g_inputPage(page);
        if (g_lang == LangEN) return g_inputPage(page); // genuine en-US resources
        DWORD size = 0;
        const BYTE* tpl = EmbeddedInpDlgTemplate(LOWORD(page->pszTemplate), size);
        if (!tpl || !size) return g_inputPage(page);    // unknown template: genuine
        PROPSHEETPAGEW copy{};
        const size_t wire = std::min<size_t>(page->dwSize, sizeof(copy));
        memcpy(&copy, page, wire); copy.dwSize = static_cast<DWORD>(wire);
        copy.dwFlags |= PSP_DLGINDIRECT;
        copy.pResource = reinterpret_cast<LPCDLGTEMPLATE>(tpl);
        HPROPSHEETPAGE result = g_inputPage(&copy);
        if (!result) { g_uiFailed = true; Wh_Log(L"Translated input page %u failed: Win32=%lu", LOWORD(page->pszTemplate), GetLastError()); }
        else Wh_Log(L"Windows 7 input page %u served translated: %p", LOWORD(page->pszTemplate), result);
        return result;
    } catch (...) {
        Wh_Log(L"InputCreatePage: EXCEPTION swallowed");
        g_uiFailed = true; return nullptr;
    }
}

struct DialogContext { DLGPROC original; LPARAM parameter; HWND window; bool isInput; };
constexpr PCWSTR kDialogProperty = L"Windhawk.IntlRestore.Dialog.89B30F6D";
INT_PTR CALLBACK DialogCallback(HWND window, UINT message, WPARAM wparam, LPARAM lparam) {
    try {
        auto* context = message == WM_INITDIALOG ? reinterpret_cast<DialogContext*>(lparam)
            : reinterpret_cast<DialogContext*>(GetPropW(window, kDialogProperty));
        if (!context) return FALSE;
        if (message == WM_INITDIALOG) {
            context->window = window;
            if (context->isInput) TranslateInputWindow(window);
            if (!SetPropW(window, kDialogProperty, context)) {
                EndDialog(window, -1); g_uiFailed = true; return FALSE;
            }
            Own(window, true); ++g_uiCreated; lparam = context->parameter;
            if (g_stopping.load()) PostMessageW(window, WM_CLOSE, 0, 0);
        }
        // Unload escalation: the mod asked this dialog to close and its own
        // DLGPROC did not (or it is a nested modal child that ignores it). End
        // it from here - the dialog's own thread, which is the only place
        // EndDialog may be called - so the unload wait can finish instead of
        // holding the host open. Only ever reached with g_stopping set.
        if (message == WM_CLOSE && g_forceCloseUi.load(std::memory_order_acquire)) {
            // EndDialog only: the dialog's own teardown (WM_DESTROY and the
            // WM_NCDESTROY that untracks it below) must still run through the
            // original DLGPROC, so the property and the tracking entry are not
            // dropped early here.
            EndDialog(window, 0);
            return TRUE;
        }
        INT_PTR result = context->original(window, message, wparam, lparam);
        if (message == WM_NCDESTROY) { RemovePropW(window, kDialogProperty); Own(window, false); }
        return result;
    } catch (...) {
        // An OS-invoked dialog callback must never propagate: report the
        // message unhandled and flag the UI flow for fallback diagnosis.
        g_uiFailed = true;
        return FALSE;
    }
}
INT_PTR ShowDialog(DialogProc original, HINSTANCE instance, LPCWSTR name, HWND parent, DLGPROC proc, LPARAM param) {
    try {
        if (!original || !proc) { SetLastError(ERROR_INVALID_PARAMETER); return -1; }
        if (IsMain(instance)) {
            // Indirect creation: the template bytes come from the embedded genuine
            // tables, so no MUI file or loader MUI redirection is needed.
            if (!IS_INTRESOURCE(name)) { g_uiFailed = true; SetLastError(ERROR_RESOURCE_NAME_NOT_FOUND); return -1; }
            DWORD size = 0;
            const BYTE* tpl = EmbeddedDlgTemplate(LOWORD(name), size);
            if (!tpl || !size) {
                Wh_Log(L"Unknown private dialog template %u", LOWORD(name));
                g_uiFailed = true; SetLastError(ERROR_RESOURCE_NAME_NOT_FOUND); return -1;
            }
            DialogContext context{proc, param, nullptr, false};
            INT_PTR result = DialogBoxIndirectParamW(instance, reinterpret_cast<LPCDLGTEMPLATE>(tpl),
                                                     parent, DialogCallback, reinterpret_cast<LPARAM>(&context));
            DWORD error = GetLastError();
            Own(context.window, false);
            if (result == -1) {
                g_uiFailed = true;
                Wh_Log(L"DialogBoxIndirectParamW failed: %lu", error);
            }
            SetLastError(error); return result;
        }
        if (instance == g_inputModule && g_lang != LangEN && IS_INTRESOURCE(name)) {
            // Text Services modal dialogs (Add Input Language 101, Change
            // Key Sequence 108/111, Keyboard Layout Preview 900 and the
            // confirmation dialogs 112-114): serve the translated indirect
            // template exactly like the private intl.cpl dialogs above.
            DWORD size = 0;
            const BYTE* tpl = EmbeddedInpDlgTemplate(LOWORD(name), size);
            if (tpl && size) {
                DialogContext context{proc, param, nullptr, true};
                INT_PTR result = DialogBoxIndirectParamW(instance, reinterpret_cast<LPCDLGTEMPLATE>(tpl),
                                                          parent, DialogCallback, reinterpret_cast<LPARAM>(&context));
                DWORD error = GetLastError();
                Own(context.window, false);
                if (result == -1) {
                    g_uiFailed = true;
                    Wh_Log(L"Translated input dialog %u failed: %lu", LOWORD(name), error);
                }
                SetLastError(error); return result;
            }
        }
        DialogContext context{proc, param, nullptr, (instance == g_inputModule)};
        INT_PTR result = original(instance, name, parent, DialogCallback, reinterpret_cast<LPARAM>(&context));
        DWORD error = GetLastError();
        Own(context.window, false);
        if (result == -1) { g_uiFailed = true; Wh_Log(L"DialogBoxParamW failed: %lu", error); }
        SetLastError(error); return result;

    } catch (...) {
        g_uiFailed = true; SetLastError(ERROR_NOT_ENOUGH_MEMORY); return -1;
    }
}
INT_PTR WINAPI MainDialogBox(HINSTANCE h, LPCWSTR n, HWND w, DLGPROC p, LPARAM l) {
    return ShowDialog(DialogBoxParamW, h, n, w, p, l);
}
INT_PTR WINAPI InputDialogBox(HINSTANCE h, LPCWSTR n, HWND w, DLGPROC p, LPARAM l) {
    return ShowDialog(g_inputDialog, h, n, w, p, l);
}

FARPROC WINAPI PrivateGetProcAddress(HMODULE module, LPCSTR name) {
    try {
        if (IsMain(module)) return PrivateExport(name);
        // Dynamic version queries (any module) see Windows 7 SP1, like ACT shims
        // report to legacy code. Ordinals can never be version names.
        if (reinterpret_cast<ULONG_PTR>(name) > 0xffff && IsVersionQuery(name))
            return VersionSpoof(name);
        FARPROC proc = GetProcAddress(module, name);
        // Evidence: dynamic comctl32 resolution (InitCommonControlsEx,
        // TaskDialogIndirect, TaskDialog, CreatePropertySheetPageW, PropertySheetW,
        // DSA ords 320/321/323/324/327). Remap by resolved address so both named
        // and ordinal queries are covered.
        if (proc) {
            if (proc == reinterpret_cast<FARPROC>(reinterpret_cast<void*>(g_mainPS))) {
                Wh_Log(L"Dynamic PropertySheetW routed to the Win7 adapter");
                return reinterpret_cast<FARPROC>(reinterpret_cast<void*>(MainPropertySheet));
            }
            if (proc == reinterpret_cast<FARPROC>(reinterpret_cast<void*>(g_mainPage))) {
                Wh_Log(L"Dynamic CreatePropertySheetPageW routed to the Win7 adapter");
                return reinterpret_cast<FARPROC>(reinterpret_cast<void*>(MainCreatePage));
            }
        }
        if (!proc) {
            DWORD error = GetLastError();
            if (reinterpret_cast<ULONG_PTR>(name) <= 0xffff)
                Wh_Log(L"Dynamic lookup missing: module=%p ordinal=%u Win32=%lu", module, LOWORD(name), error);
            else Wh_Log(L"Dynamic lookup missing: module=%p name=%S Win32=%lu", module, name, error);
            SetLastError(error);
        }
        return proc;

    } catch (...) {
        return GetProcAddress(module, name);
    }
}

// Only the private Win7 IAT reaches these adapters. SHCreateThread's
// CTF_FREELIBANDEXIT cannot retain a manually mapped image via the OS module
// list, so the equivalent lifetime is supplied by the mod's explicit rundown.
struct ThreadJob { LPTHREAD_START_ROUTINE routine; void* argument; };
bool BeginJob() {
    AcquireSRWLockShared(&g_gate);
    const bool allowed = !g_stopping.load() || g_active.load() != 0 || g_jobs.load() != 0;
    if (allowed && g_jobs.fetch_add(1, std::memory_order_acq_rel) == 0) ResetEvent(g_jobsIdle);
    ReleaseSRWLockShared(&g_gate);
    return allowed;
}
void EndJob() {
    AcquireSRWLockExclusive(&g_gate);
    if (g_jobs.fetch_sub(1, std::memory_order_acq_rel) == 1) SetEvent(g_jobsIdle);
    ReleaseSRWLockExclusive(&g_gate);
}
DWORD WINAPI ThreadBridge(void* parameter) {
    ThreadJob job = *static_cast<ThreadJob*>(parameter);
    HeapFree(GetProcessHeap(), 0, parameter);
    DWORD result = 0;
    try {
        ActScope act(g_act);
        result = job.routine(job.argument);
    } catch (...) {
        // A throwing worker must still release its job slot, or teardown
        // would wait for it forever. The notification itself is best-effort.
        LastErrorScope keep;
        Wh_Log(L"Private worker routine threw; job released");
        result = ERROR_NOT_ENOUGH_MEMORY;
    }
    // Both verified Win7 routines return normally. After this point there are
    // no references/return addresses into the private image on this worker.
    EndJob();
    return result;
}
HANDLE WINAPI PrivateCreateThread(LPSECURITY_ATTRIBUTES attributes, SIZE_T stack,
    LPTHREAD_START_ROUTINE routine, LPVOID parameter, DWORD flags, LPDWORD id) {
    try {
        if (!routine || (flags & CREATE_SUSPENDED)) {
            // No pinned call site requests a suspended worker. Do not introduce an
            // unresumable work item which could hang teardown indefinitely.
            SetLastError(ERROR_NOT_SUPPORTED); return nullptr;
        }
        auto* job = static_cast<ThreadJob*>(HeapAlloc(GetProcessHeap(), 0, sizeof(ThreadJob)));
        if (!job) { SetLastError(ERROR_NOT_ENOUGH_MEMORY); return nullptr; }
        *job = {routine, parameter};
        if (!BeginJob()) {
            HeapFree(GetProcessHeap(), 0, job); SetLastError(ERROR_SHUTDOWN_IN_PROGRESS); return nullptr;
        }
        HANDLE thread = CreateThread(attributes, stack, ThreadBridge, job, flags, id);
        if (!thread) {
            DWORD error = GetLastError(); HeapFree(GetProcessHeap(), 0, job); EndJob(); SetLastError(error);
        } else {
            RegisterThread(thread); // teardown joins this before freeing the image
        }
        return thread; // Caller owns this real handle, just as with CreateThread.

    } catch (...) {
        SetLastError(ERROR_NOT_ENOUGH_MEMORY); return nullptr;
    }
}
BOOL WINAPI PrivateSHCreateThread(LPTHREAD_START_ROUTINE routine, void* parameter,
                                  DWORD flags, LPTHREAD_START_ROUTINE callback) {
    try {
        // Evidence: all 7 shlwapi-ord-16 sites pass routine + NULL param +
        // flags 0x11 (CTF_INSIST|CTF_FREELIBANDEXIT) + NULL callback.
        constexpr DWORD insistAndLibraryLifetime = 0x11;
        if (!routine || callback || flags != insistAndLibraryLifetime) {
            Wh_Log(L"Unexpected private SHCreateThread contract: flags=0x%lX", flags);
            SetLastError(ERROR_NOT_SUPPORTED); return FALSE;
        }
        if (HANDLE thread = PrivateCreateThread(nullptr, 0, routine, parameter, 0, nullptr)) {
            CloseHandle(thread); return TRUE;
        }
        // CTF_INSIST's documented fallback runs the same real notification routine
        // on the current thread if a worker could not be created.
        if (!BeginJob()) return FALSE;
        { ActScope act(g_act); routine(parameter); }
        EndJob();
        return TRUE;
    } catch (...) {
        SetLastError(ERROR_NOT_ENOUGH_MEMORY); return FALSE;
    }
}
BOOL WINAPI PrivateDisableThreadLibraryCalls(HMODULE module) {
    try {
        if (!IsMain(module)) return DisableThreadLibraryCalls(module);
        // This exact mapped DLL has no TLS directory and receives no automatic
        // thread notifications. The requested state is already guaranteed by the
        // private loader; no system module's notification state is changed.
        return TRUE;

    } catch (...) {
        return DisableThreadLibraryCalls(module);
    }
}
// Returns false only if a caller supplied a budget and it ran out. The unload
// path passes INFINITE: a job that has not called EndJob means a worker still
// inside ThreadBridge, i.e. mod code on a live stack.
bool WaitForJobs(DWORD timeoutMs) {
    if (!g_jobsIdle || g_jobs.load(std::memory_order_acquire) == 0) return true;
    const bool unbounded = timeoutMs == INFINITE;
    const ULONGLONG deadline = unbounded ? 0 : GetTickCount64() + timeoutMs;
    ULONGLONG lastDiagnostic = GetTickCount64();
    while (g_jobs.load(std::memory_order_acquire) != 0) {
        const ULONGLONG now = GetTickCount64();
        if (!unbounded && now >= deadline) {
            Wh_Log(L"Private worker jobs still running after %lu ms; this wait is over, the caller escalates",
                   static_cast<unsigned long>(timeoutMs));
            return false;
        }
        if (unbounded && now - lastDiagnostic > 5000) {
            Wh_Log(L"Still waiting for %ld private worker job(s) to finish (%llu ms elapsed)",
                   g_jobs.load(std::memory_order_acquire), now - lastDiagnostic);
            lastDiagnostic = now;
        }
        // Plain WaitForSingleObject does not service cross-thread
        // SendMessage calls, so a worker that sends to this (UI) thread
        // while it waits here would deadlock or stall it (finding 2).
        // Wake for inbound sends and let PeekMessage's internal dispatch
        // answer them, without pumping posted messages (which would
        // re-enter arbitrary window procs).
        const DWORD slice = unbounded ? 100
            : static_cast<DWORD>(std::min<ULONGLONG>(100, deadline - now));
        DWORD wait = MsgWaitForMultipleObjectsEx(1, &g_jobsIdle, slice, QS_SENDMESSAGE, MWMO_INPUTAVAILABLE);
        if (wait == WAIT_OBJECT_0 + 1) {
            MSG msg;
            PeekMessageW(&msg, nullptr, 0, 0, PM_NOREMOVE);
        }
    }
    // Pair with EndJob's final signal before teardown closes the event.
    AcquireSRWLockExclusive(&g_gate); ReleaseSRWLockExclusive(&g_gate);
    return true;
}
// Same shape as WaitForJobs, for the in-flight legacy CPL calls instead of the
// worker jobs: an unfinished legacy call is mod code and provider code on a
// live stack, so the unload path passes INFINITE here too. The bounded variant
// exists so Cleanup() can escalate (close the UI this mod created) between two
// waits rather than sitting in a wait nothing can end.
bool WaitForLegacyIdle(DWORD timeoutMs) {
    if (!g_idle || g_active.load(std::memory_order_acquire) == 0) return true;
    const bool unbounded = timeoutMs == INFINITE;
    const ULONGLONG deadline = unbounded ? 0 : GetTickCount64() + timeoutMs;
    ULONGLONG lastDiagnostic = GetTickCount64();
    while (g_active.load(std::memory_order_acquire) != 0) {
        const ULONGLONG now = GetTickCount64();
        if (!unbounded && now >= deadline) return false;
        if (unbounded && now - lastDiagnostic > 5000) {
            Wh_Log(L"Still waiting for %ld in-flight private legacy call(s) to finish (%llu ms elapsed)",
                   g_active.load(std::memory_order_acquire), now - lastDiagnostic);
            lastDiagnostic = now;
        }
        // See WaitForJobs: wake for inbound sends so a legacy thread messaging
        // this one is answered instead of deadlocking against the wait.
        const DWORD slice = unbounded ? 100
            : static_cast<DWORD>(std::min<ULONGLONG>(100, deadline - now));
        DWORD wait = MsgWaitForMultipleObjectsEx(1, &g_idle, slice, QS_SENDMESSAGE, MWMO_INPUTAVAILABLE);
        if (wait == WAIT_OBJECT_0 + 1) {
            MSG msg;
            PeekMessageW(&msg, nullptr, 0, 0, PM_NOREMOVE);
        }
    }
    return true;
}
BOOL WINAPI PrivateShellExecuteExW(SHELLEXECUTEINFOW* info) {
    try {
    if (!info || info->cbSize < sizeof(*info) || !info->lpFile || !info->lpParameters)
        return ShellExecuteExW(info);
    // Evidence: RVA 0x4B60 holds "shell32.dll,Control_RunDLL input.dll", used
    // by Intl_LaunchCiceroDlg at 0x1BCBF. Only that private launch is rewritten
    // to the verified Win7 input.dll path. Inside the private CPL's IAT only.
    if (!SystemFileToken(info->lpFile, L"rundll32.exe", g_system) ||
        !Equal(info->lpParameters, L"shell32.dll,Control_RunDLL input.dll"))
        return ShellExecuteExW(info);
    if (g_stopping.load()) { SetLastError(ERROR_CANCELLED); return FALSE; }
    std::wstring args = L"shell32.dll,Control_RunDLL \"" + g_inputPath + L"\"";
    SHELLEXECUTEINFOW copy = *info; copy.lpParameters = args.c_str();
    Wh_Log(L"Original Change keyboards action -> built-in rundll32, private Windows 7 input.dll: %s", g_inputPath.c_str());
    BOOL result = ShellExecuteExW(&copy);
    DWORD error = GetLastError();
    info->hInstApp = copy.hInstApp;
    if (info->fMask & SEE_MASK_NOCLOSEPROCESS) info->hProcess = copy.hProcess;
    if (!result) Wh_Log(L"Private Input CPL launch failed: Win32=%lu", error);
    SetLastError(error); return result;
    } catch (...) { return ShellExecuteExW(info); } // our rewrite failed; real call, unmodified args
}

// The genuine Location page (dialog 600) opens its "Default location" link
// (control 1045, NM_CLICK/NM_RETURN) through COpenControlPanel::Open, first
// with "Microsoft.LocationAndOtherSensors"+"pageMain", then with
// "Microsoft.DefaultLocation" (IDA evidence cited in the header notes). Both
// names are gone on Windows 10/11: the fallthrough opens the Settings app
// with a spurious error. Only this one coclass is substituted, and only for
// the private image; every other CoCreateInstance passes through untouched.
constexpr GUID kClsidOpenControlPanel = { 0x06622D85, 0x6856, 0x4460,
    { 0x8D, 0xE1, 0xA8, 0x19, 0x21, 0xB4, 0x1C, 0x4B } };
constexpr GUID kIidOpenControlPanel = { 0xD11AD862, 0x66DE, 0x4DF4,
    { 0xBF, 0x6C, 0x1F, 0x56, 0x21, 0x99, 0x6A, 0xF1 } };
constexpr GUID kIidUnknown = { 0x00000000, 0x0000, 0x0000,
    { 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46 } };
struct IOpenPanel : public IUnknown {
    virtual HRESULT STDMETHODCALLTYPE Open(PCWSTR name, PCWSTR page, IUnknown* site) = 0;
    // Real IOpenControlPanel also declares GetPath/GetCurrentView. QueryInterface
    // hands this object out for IID_IOpenControlPanel, so the vtable must be as
    // long as the real interface or a call to either of these two jumps past the
    // end of the emitted vtable into whatever memory follows it (finding 6).
    virtual HRESULT STDMETHODCALLTYPE GetPath(PCWSTR name, PWSTR path, UINT count, PVOID reserved) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetCurrentView(DWORD flags, REFIID riid, void** out) = 0;
};
class FakeControlPanel : public IOpenPanel {
    std::atomic<LONG> refs_{ 1 };
public:
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** out) override {
        try {
            if (!out) return E_POINTER;
            if (!memcmp(&riid, &kIidUnknown, sizeof(GUID)) ||
                !memcmp(&riid, &kIidOpenControlPanel, sizeof(GUID))) {
                *out = static_cast<IOpenPanel*>(this);
                AddRef();
                return S_OK;
            }
            *out = nullptr;
            return E_NOINTERFACE;
        } catch (...) { return E_UNEXPECTED; }
    }
    ULONG STDMETHODCALLTYPE AddRef() override {
        return static_cast<ULONG>(refs_.fetch_add(1, std::memory_order_relaxed) + 1);
    }
    ULONG STDMETHODCALLTYPE Release() override {
        LONG left = refs_.fetch_sub(1, std::memory_order_acq_rel) - 1;
        return static_cast<ULONG>(left < 0 ? 0 : left); // static lifetime; never freed
    }
    HRESULT STDMETHODCALLTYPE GetPath(PCWSTR /*name*/, PWSTR /*path*/, UINT /*count*/, PVOID /*reserved*/) override {
        return E_NOTIMPL;
    }
    HRESULT STDMETHODCALLTYPE GetCurrentView(DWORD /*flags*/, REFIID /*riid*/, void** out) override {
        if (out) *out = nullptr;
        return E_NOTIMPL;
    }
    HRESULT STDMETHODCALLTYPE Open(PCWSTR name, PCWSTR page, IUnknown* site) override {
        (void)name; (void)page; (void)site;
        // The Win7 sensors panel no longer exists: open the equivalent modern
        // Location page (generic Settings home if that ever fails) and report
        // success, exactly like a working panel open. The genuine caller shows
        // no UI on failure, so a launch failure stays silent here as well.
        try {
            SHELLEXECUTEINFOW exec{};
            exec.cbSize = sizeof(exec);
            exec.lpFile = L"ms-settings:privacy-location";
            exec.nShow = SW_SHOWNORMAL;
            LastErrorScope keep;
            if (!ShellExecuteExW(&exec)) {
                exec.lpFile = L"ms-settings:";
                ShellExecuteExW(&exec);
            }
            Wh_Log(L"Default-location link -> modern Location settings (Win7 sensors panel is gone)");
        } catch (...) {}
        return S_OK;
    }
};
FakeControlPanel g_fakePanel;
HRESULT WINAPI PrivateCoCreateInstance(REFCLSID rclsid, LPUNKNOWN outer, DWORD clsctx,
                                       REFIID riid, LPVOID* out) {
    try {
        if (out && !outer && !memcmp(&rclsid, &kClsidOpenControlPanel, sizeof(GUID)))
            return g_fakePanel.QueryInterface(riid, out);
        return CoCreateInstance(rclsid, outer, clsctx, riid, out);
    } catch (...) {
        return CoCreateInstance(rclsid, outer, clsctx, riid, out);
    }
}

FARPROC ResolvePrivate(HMODULE module, const char* dll, LPCSTR proc) {
    const auto library = LowerDll(dll);
    if (reinterpret_cast<ULONG_PTR>(proc) <= 0xffff) {
        if (library == "shlwapi.dll" && LOWORD(proc) == 16)
            return reinterpret_cast<FARPROC>(reinterpret_cast<void*>(PrivateSHCreateThread));
        // shlwapi ord437 (IsOS): the single callsite passes OS_ANYSERVER and
        // branches on server-ness; the real client-SKU answer is correct.
        return GetProcAddress(module, proc);
    }
    auto asProc = [](auto* p) { return reinterpret_cast<FARPROC>(reinterpret_cast<void*>(p)); };
    if (library == "kernel32.dll" || library == "kernelbase.dll") {
        // Win7 NLS setters/policy: real kernelbase implementation or fail
        // closed (BindImports aborts -> controlled fallback to native CPL).
        if (!strcmp(proc, "NlsCheckPolicy") || !strcmp(proc, "NlsUpdateLocale") ||
            !strcmp(proc, "NlsUpdateSystemLocale")) {
            FARPROC target = ForwardKernelBase(proc);
            if (target) return target;
            Wh_Log(L"Refusing to fake NLS setter %S", proc);
            return nullptr;
        }
        if (!strcmp(proc, "NlsEventDataDescCreate")) {
            FARPROC target = ForwardKernelBase(proc);
            return target ? target : asProc(FallbackNlsEventDataDescCreate);
        }
        if (!strcmp(proc, "NlsWriteEtwEvent")) {
            FARPROC target = ForwardKernelBase(proc);
            return target ? target : asProc(FallbackNlsWriteEtwEvent);
        }
        if (!strcmp(proc, "GetUILanguageInfo")) {
            FARPROC target = ForwardKernelBase(proc);
            return target ? target : asProc(ShimGetUILanguageInfo);
        }
        if (!strcmp(proc, "NotifyUILanguageChange")) return asProc(ShimNotifyUILanguageChange);
        if (!strcmp(proc, "CheckElevationEnabled")) return asProc(ShimCheckElevationEnabled);
        if (!strcmp(proc, "CreateThread")) return asProc(PrivateCreateThread);
        if (!strcmp(proc, "DisableThreadLibraryCalls")) return asProc(PrivateDisableThreadLibraryCalls);
        if (!strcmp(proc, "GetModuleFileNameW")) return asProc(PrivateGetModuleFileNameW);
        if (!strcmp(proc, "GetModuleHandleW")) return asProc(PrivateGetModuleHandleW);
        if (!strcmp(proc, "LoadLibraryW")) return asProc(PrivateLoadLibraryW);
        if (!strcmp(proc, "GetProcAddress")) return asProc(PrivateGetProcAddress);
        if (!strcmp(proc, "FreeLibrary")) return asProc(PrivateFreeLibrary);
        if (!strcmp(proc, "CreateActCtxW")) return asProc(PrivateCreateActCtxW);
        if (!strcmp(proc, "FindResourceW")) return asProc(PrivateFindResourceW);
        if (!strcmp(proc, "FindResourceExW")) return asProc(PrivateFindResourceExW);
        if (!strcmp(proc, "LoadResource")) return asProc(PrivateLoadResource);
        if (!strcmp(proc, "LockResource")) return asProc(PrivateLockResource);
        if (!strcmp(proc, "FreeResource")) return asProc(PrivateFreeResource);
        if (!strcmp(proc, "GetVersionExW")) return asProc(PrivateGetVersionExW);
        if (!strcmp(proc, "GetVersionExA")) return asProc(PrivateGetVersionExA);
        if (!strcmp(proc, "GetVersion")) return asProc(PrivateGetVersion);
        if (!strcmp(proc, "VerifyVersionInfoW")) return asProc(PrivateVerifyVersionInfoW);
        if (!strcmp(proc, "VerifyVersionInfoA")) return asProc(PrivateVerifyVersionInfoA);
        if (!strcmp(proc, "VerSetConditionMask")) return asProc(PrivateVerSetConditionMask);
    }
    if (library == "ntdll.dll") {
        if (!strcmp(proc, "RtlGetVersion")) return asProc(PrivateRtlGetVersion);
        if (!strcmp(proc, "WinSqmAddToStream")) return asProc(ShimWinSqmAddToStream);
        if (!strcmp(proc, "WinSqmSetString")) return asProc(ShimWinSqmSetString);
        if (!strcmp(proc, "RtlGetUILanguageInfo")) return asProc(ShimRtlGetUILanguageInfo);
        if (!strcmp(proc, "RtlpSetPreferredUILanguages")) return asProc(ShimRtlpSetPreferredUILanguages);
    }
    if (library == "shell32.dll" && !strcmp(proc, "ShellExecuteExW"))
        return asProc(PrivateShellExecuteExW);
    if (library == "ole32.dll" && !strcmp(proc, "CoCreateInstance"))
        return asProc(PrivateCoCreateInstance);
    if (library == "shlwapi.dll" && !strcmp(proc, "SHCreateThread"))
        return asProc(PrivateSHCreateThread);
    if (library == "user32.dll") {
        if (!strcmp(proc, "LoadStringW")) return asProc(PrivateLoadStringW);
        if (!strcmp(proc, "DialogBoxParamW")) return asProc(MainDialogBox);
    }
    if (library == "comctl32.dll") {
        if (!strcmp(proc, "PropertySheetW")) return asProc(MainPropertySheet);
        if (!strcmp(proc, "CreatePropertySheetPageW")) return asProc(MainCreatePage);
    }
    if (library == "msvcrt.dll" &&
        (!strcmp(proc, "malloc") || !strcmp(proc, "free") || !strcmp(proc, "calloc") ||
         !strcmp(proc, "realloc") || !strcmp(proc, "??2@YAPEAX_K@Z") || !strcmp(proc, "??3@YAXPEAX@Z"))) {
        // Real-first CRT heap routing (see PrivateMalloc): the genuine attach
        // initializer fails closed when msvcrt!malloc returns NULL.
        FARPROC real = GetProcAddress(module, proc);
        if (!real) return nullptr;
        if (!strcmp(proc, "malloc")) {
            if (!g_realMalloc) g_realMalloc = reinterpret_cast<MallocFn>(reinterpret_cast<void*>(real));
            return asProc(PrivateMalloc);
        }
        if (!strcmp(proc, "free")) {
            if (!g_realFree) g_realFree = reinterpret_cast<FreeFn>(reinterpret_cast<void*>(real));
            return asProc(PrivateFree);
        }
        if (!strcmp(proc, "calloc")) {
            if (!g_realCalloc) g_realCalloc = reinterpret_cast<CallocFn>(reinterpret_cast<void*>(real));
            return asProc(PrivateCalloc);
        }
        if (!strcmp(proc, "realloc")) {
            if (!g_realRealloc) g_realRealloc = reinterpret_cast<ReallocFn>(reinterpret_cast<void*>(real));
            return asProc(PrivateRealloc);
        }
        if (!strcmp(proc, "??2@YAPEAX_K@Z")) {
            if (!g_realNew) g_realNew = reinterpret_cast<MallocFn>(reinterpret_cast<void*>(real));
            return asProc(PrivateNew);
        }
        if (!g_realDelete) g_realDelete = reinterpret_cast<FreeFn>(reinterpret_cast<void*>(real));
        return asProc(PrivateDelete);
    }
    // Native NLS queries, registry, COM, TSF and every other API keep their
    // real implementation. Unexpected missing imports abort the setup.
    return GetProcAddress(module, proc);
}

bool PatchInputSlot(ULONG_PTR* slot, ULONG_PTR replacement) {
    if (!slot || !replacement) return false;
    DWORD old = 0;
    if (!VirtualProtect(slot, sizeof(*slot), PAGE_READWRITE, &old)) return false;
    ULONG_PTR original = *slot;
    InterlockedExchangePointer(reinterpret_cast<PVOID volatile*>(slot), reinterpret_cast<void*>(replacement));
    DWORD ignored = 0;
    if (!VirtualProtect(slot, sizeof(*slot), old, &ignored)) {
        InterlockedExchangePointer(reinterpret_cast<PVOID volatile*>(slot), reinterpret_cast<void*>(original));
        VirtualProtect(slot, sizeof(*slot), old, &ignored);
        return false;
    }
    g_inputPatches.push_back({slot, original, replacement});
    return true;
}
using InputLoadStringFn = int (WINAPI*)(HINSTANCE, UINT, LPWSTR, int);
InputLoadStringFn g_inputLoadString = nullptr;

// input.dll keeps its own resource set; the intl.cpl tables never translate
// it. Its user-facing text is now translated three ways: dialog templates
// are rebuilt from the verified kInpDialogs data (property-sheet pages
// through the patched CreatePropertySheetPageW slot, modal dialogs through
// ShowDialog below), runtime strings come from the id-verified kInputText
// pack (defined with the language packs above the builders), and
// kInputUiLabels below stays as a window-text fallback for anything the
// templates and the string hook do not cover.
struct InputUiLabel { const wchar_t* source; const wchar_t* text[LangCount]; };
static const InputUiLabel kInputUiLabels[] = {
 {L"General",{L"General",L"Generale",L"Allgemein",L"Général",L"General",L"Geral",L"Algemeen",L"Ogólne",L"Общие",L"常规",L"全般",L"일반",L"Genel",L"Obecné",L"Általános",L"General",L"Allmänt",L"Загальні",L"Γενικά",L"عام"}},
 {L"Language Bar",{L"Language Bar",L"Barra della lingua",L"Sprachleiste",L"Barre de langue",L"Barra de idioma",L"Barra de idiomas",L"Taalbalk",L"Pasek języka",L"Языковая панель",L"语言栏",L"言語バー",L"언어 표시줄",L"Dil çubuğu",L"Panel jazyků",L"Nyelvi eszköztár",L"Bara de limbi",L"Språkfält",L"Мовна панель",L"Γραμμή γλώσσας",L"شريط اللغة"}},
 {L"Advanced Key Settings",{L"Advanced Key Settings",L"Impostazioni avanzate tasti",L"Erweiterte Tasteneinstellungen",L"Paramètres de touches avancés",L"Configuración avanzada de teclas",L"Definições avançadas de teclas",L"Geavanceerde toetsinstellingen",L"Zaawansowane ustawienia klawiszy",L"Дополнительные параметры клавиш",L"高级键设置",L"キーの詳細設定",L"고급 키 설정",L"Gelişmiş tuş ayarları",L"Upřesnit nastavení kláves",L"Speciális billentyűzet-beállítások",L"Setări avansate ale tastelor",L"Avancerade tangentinställningar",L"Розширені параметри клавіш",L"Ρυθμίσεις πλήκτρων για προχωρημένους",L"إعدادات المفاتيح المتقدمة"}},
 {L"Add...",{L"Add...",L"Aggiungi...",L"Hinzufügen...",L"Ajouter...",L"Agregar...",L"Adicionar...",L"Toevoegen...",L"Dodaj...",L"Добавить...",L"添加...",L"追加...",L"추가...",L"Ekle...",L"Přidat...",L"Hozzáadás...",L"Adăugare...",L"Lägg till...",L"Додати...",L"Προσθήκη...",L"إضافة..."}},
 {L"Remove",{L"Remove",L"Rimuovi",L"Entfernen",L"Supprimer",L"Quitar",L"Remover",L"Verwijderen",L"Usuń",L"Удалить",L"删除",L"削除",L"제거",L"Kaldır",L"Odebrat",L"Eltávolítás",L"Eliminare",L"Ta bort",L"Видалити",L"Κατάργηση",L"إزالة"}},
 {L"Properties...",{L"Properties...",L"Proprietà...",L"Eigenschaften...",L"Propriétés...",L"Propiedades...",L"Propriedades...",L"Eigenschappen...",L"Właściwości...",L"Свойства...",L"属性...",L"プロパティ...",L"속성...",L"Özellikler...",L"Vlastnosti...",L"Tulajdonságok...",L"Proprietăți...",L"Egenskaper...",L"Властивості...",L"Ιδιότητες...",L"خصائص..."}},
 {L"Move Up",{L"Move Up",L"Sposta su",L"Nach oben",L"Monter",L"Subir",L"Mover para cima",L"Omhoog",L"Przenieś w górę",L"Вверх",L"上移",L"上へ",L"위로 이동",L"Yukarı Taşı",L"Nahoru",L"Feljebb",L"Mutare în sus",L"Flytta upp",L"Вгору",L"Μετακίνηση επάνω",L"نقل لأعلى"}},
 {L"Move Down",{L"Move Down",L"Sposta giù",L"Nach unten",L"Descendre",L"Bajar",L"Mover para baixo",L"Omlaag",L"Przenieś w dół",L"Вниз",L"下移",L"下へ",L"아래로 이동",L"Aşağı Taşı",L"Dolů",L"Lejjebb",L"Mutare în jos",L"Flytta ned",L"Вниз",L"Μετακίνηση κάτω",L"نقل لأسفل"}},
 {L"Select one of the installed input languages to use as the default for all input fields.",{L"Select one of the installed input languages to use as the default for all input fields.",L"Selezionare una delle lingue di input installate da usare come predefinita per tutti i campi di input.",L"Wählen Sie eine der installierten Eingabesprachen als Standardsprache für alle Eingabefelder aus.",L"Sélectionnez l’une des langues d’entrée installées comme langue par défaut pour tous les champs de saisie.",L"Seleccione uno de los idiomas de entrada instalados para usarlo como predeterminado en todos los campos de entrada.",L"Selecione um dos idiomas de entrada instalados para usar como padrão em todos os campos de entrada.",L"Selecteer een van de geïnstalleerde invoertalen als standaard voor alle invoervelden.",L"Wybierz jeden z zainstalowanych języków wprowadzania jako domyślny dla wszystkich pól wprowadzania.",L"Выберите один из установленных языков ввода, который будет использоваться по умолчанию во всех полях ввода.",L"选择一种已安装的输入语言作为所有输入字段的默认语言。",L"すべての入力フィールドで既定として使用する、インストール済みの入力言語を選択します。",L"모든 입력 필드에 기본값으로 사용할 설치된 입력 언어를 선택하세요.",L"Tüm giriş alanlarında varsayılan olarak kullanılacak yüklü giriş dillerinden birini seçin.",L"Vyberte jeden z nainstalovaných vstupních jazyků, který se použije jako výchozí pro všechna vstupní pole.",L"Válasszon egy telepített beviteli nyelvet, amely alapértelmezettként használható minden beviteli mezőben.",L"Selectați una dintre limbile de intrare instalate pentru a o utiliza implicit în toate câmpurile de introducere.",L"Välj ett av de installerade inmatningsspråken som ska användas som standard i alla inmatningsfält.",L"Виберіть одну з установлених мов введення, яка використовуватиметься за замовчуванням у всіх полях введення.",L"Επιλέξτε μία από τις εγκατεστημένες γλώσσες εισόδου για χρήση ως προεπιλογή σε όλα τα πεδία εισόδου.",L"حدد إحدى لغات الإدخال المثبتة لاستخدامها كلغة افتراضية في جميع حقول الإدخال."}},
 {L"Installed services",{L"Installed services",L"Servizi installati",L"Installierte Dienste",L"Services installés",L"Servicios instalados",L"Serviços instalados",L"Geïnstalleerde services",L"Zainstalowane usługi",L"Установленные службы",L"已安装的服务",L"インストールされているサービス",L"설치된 서비스",L"Yüklü hizmetler",L"Nainstalované služby",L"Telepített szolgáltatások",L"Servicii instalate",L"Installerade tjänster",L"Установлені служби",L"Υπηρεσίες εγκατεστημένες",L"الخدمات المثبتة"}},
 {L"Select the services that you want for each input language shown in the list. Use the Add and Remove buttons to modify this list.",{L"Select the services that you want for each input language shown in the list. Use the Add and Remove buttons to modify this list.",L"Selezionare i servizi desiderati per ogni lingua di input visualizzata nell'elenco. Usare i pulsanti Aggiungi e Rimuovi per modificare l'elenco.",L"Wählen Sie die gewünschten Dienste für jede in der Liste angezeigte Eingabesprache aus. Verwenden Sie zum Ändern der Liste die Schaltflächen Hinzufügen und Entfernen.",L"Sélectionnez les services souhaités pour chaque langue d’entrée affichée dans la liste. Utilisez les boutons Ajouter et Supprimer pour modifier cette liste.",L"Seleccione los servicios que desea para cada idioma de entrada mostrado en la lista. Use los botones Agregar y Quitar para modificarla.",L"Selecione os serviços desejados para cada idioma de entrada mostrado na lista. Use os botões Adicionar e Remover para alterar esta lista.",L"Selecteer de gewenste services voor elke invoertaal in de lijst. Gebruik de knoppen Toevoegen en Verwijderen om deze lijst te wijzigen.",L"Wybierz usługi dla każdego języka wprowadzania widocznego na liście. Użyj przycisków Dodaj i Usuń, aby zmienić tę listę.",L"Выберите службы для каждого языка ввода в списке. Для изменения списка используйте кнопки «Добавить» и «Удалить».",L"为列表中显示的每种输入语言选择所需的服务。使用“添加”和“删除”按钮修改此列表。",L"一覧に表示される各入力言語で使用するサービスを選択します。追加ボタンと削除ボタンで一覧を変更します。",L"목록에 표시된 각 입력 언어에 사용할 서비스를 선택하세요. 추가 및 제거 단추를 사용하여 이 목록을 수정합니다.",L"Listede gösterilen her giriş dili için istediğiniz hizmetleri seçin. Listeyi değiştirmek için Ekle ve Kaldır düğmelerini kullanın.",L"Vyberte služby pro jednotlivé vstupní jazyky zobrazené v seznamu. Seznam upravte pomocí tlačítek Přidat a Odebrat.",L"Válassza ki a listában szereplő beviteli nyelvekhez kívánt szolgáltatásokat. A lista módosításához használja a Hozzáadás és Eltávolítás gombot.",L"Selectați serviciile dorite pentru fiecare limbă de intrare afișată în listă. Folosiți butoanele Adăugare și Eliminare pentru a modifica lista.",L"Välj de tjänster du vill använda för varje inmatningsspråk i listan. Använd knapparna Lägg till och Ta bort för att ändra listan.",L"Виберіть служби для кожної мови введення у списку. Для зміни списку використовуйте кнопки «Додати» та «Видалити».",L"Επιλέξτε τις υπηρεσίες που θέλετε για κάθε γλώσσα εισόδου στη λίστα. Χρησιμοποιήστε τα κουμπιά Προσθήκη και Κατάργηση για να τροποποιήσετε τη λίστα.",L"حدد الخدمات المطلوبة لكل لغة إدخال معروضة في القائمة. استخدم زري إضافة وإزالة لتعديل هذه القائمة."}}
};
bool SameInputText(const wchar_t* a, const wchar_t* b) {
    // Whitespace- and accelerator-insensitive compare: dialog templates
    // carry '&' markers that localized texts place at different letters.
    while (*a && *b) {
        while (*a && (iswspace(*a) || *a == L'&')) ++a;
        while (*b && (iswspace(*b) || *b == L'&')) ++b;
        if (*a != *b) return false;
        if (*a) { ++a; ++b; }
    }
    while (*a && (iswspace(*a) || *a == L'&')) ++a;
    while (*b && (iswspace(*b) || *b == L'&')) ++b;
    return !*a && !*b;
}
void TranslateInputWindow(HWND window) {
    // Fallback pass for windows the rebuilt templates do not cover.
    // Runs at WM_INITDIALOG (all controls exist) and PSCB_INITIALIZED.
    if (g_lang <= LangEN || g_lang >= LangCount) return;
    wchar_t title[256]{}; GetWindowTextW(window, title, ARRAYSIZE(title));
    if (SameInputText(title, kInputLanguagesTitle[LangEN]))
        SetWindowTextW(window, kInputLanguagesTitle[g_lang]);
    EnumChildWindows(window, [](HWND child, LPARAM) -> BOOL {
        wchar_t text[256]{}; GetWindowTextW(child, text, ARRAYSIZE(text));
        for (const auto& label : kInputUiLabels) {
            if (SameInputText(text, label.source)) {
                SetWindowTextW(child, label.text[g_lang]); break;
            }
        }
        return TRUE;
    }, 0);
}

int WINAPI InputLoadStringW(HINSTANCE module, UINT id, LPWSTR output, int size) {
    if (!g_inputLoadString || module != g_inputModule) return g_inputLoadString ? g_inputLoadString(module,id,output,size) : 0;
    const wchar_t* text = InputText(id);
    if (!text) return g_inputLoadString(module, id, output, size);
    size_t n = wcslen(text);
    if (size == 0) { if (output) *reinterpret_cast<const wchar_t**>(output) = text; return (int)n; }
    if (!output || size < 0) { SetLastError(ERROR_INVALID_PARAMETER); return 0; }
    size_t copy = std::min(n, (size_t)(size - 1));
    memcpy(output, text, copy * sizeof(wchar_t)); output[copy] = 0;
    return (int)copy;
}

bool AdaptInputIat() {
    auto* base = reinterpret_cast<BYTE*>(g_inputModule);
    auto* nt = ImageHeaders(base);
    const DWORD imageSize = nt->OptionalHeader.SizeOfImage;
    auto d = nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT];
    if (!Range(d.VirtualAddress, d.Size, imageSize)) return false;
    bool sheet = false, dialog = false, version = false, loadString = false, page = false;
    for (size_t off = 0; off + sizeof(IMAGE_IMPORT_DESCRIPTOR) <= d.Size; off += sizeof(IMAGE_IMPORT_DESCRIPTOR)) {
        auto* desc = reinterpret_cast<IMAGE_IMPORT_DESCRIPTOR*>(base + d.VirtualAddress + off);
        if (!desc->Name) break;
        const char* dll = ImageString(base, imageSize, desc->Name);
        if (!dll || !desc->OriginalFirstThunk) return false;
        const auto lib = LowerDll(dll);
        for (size_t i = 0; ; ++i) {
            size_t read = desc->OriginalFirstThunk + i * 8, write = desc->FirstThunk + i * 8;
            if (!Range(read, 8, imageSize) || !Range(write, 8, imageSize)) return false;
            ULONGLONG token = *reinterpret_cast<ULONGLONG*>(base + read);
            if (!token) break;
            if (IMAGE_SNAP_BY_ORDINAL64(token)) continue;
            if (token > MAXDWORD - 2) return false;
            const char* name = ImageString(base, imageSize, static_cast<DWORD>(token) + 2);
            if (!name) return false;
            auto* slot = reinterpret_cast<ULONG_PTR*>(base + write);
            if (lib == "comctl32.dll" && !strcmp(name, "PropertySheetW")) {
                g_inputPS = reinterpret_cast<PSProc>(*slot);
                if (!PatchInputSlot(slot, reinterpret_cast<ULONG_PTR>(InputPropertySheet))) return false;
                sheet = true;
            } else if (lib == "user32.dll" && !strcmp(name, "DialogBoxParamW")) {
                g_inputDialog = reinterpret_cast<DialogProc>(*slot);
                if (!PatchInputSlot(slot, reinterpret_cast<ULONG_PTR>(InputDialogBox))) return false;
                dialog = true;
            } else if (lib == "kernel32.dll" && !strcmp(name, "GetVersionExW")) {
                // input.dll asks the OS version; same Win7 SP1 story as intl.cpl.
                if (!PatchInputSlot(slot, reinterpret_cast<ULONG_PTR>(PrivateGetVersionExW))) return false;
                version = true;
            } else if (lib == "user32.dll" && !strcmp(name, "LoadStringW")) {
                g_inputLoadString = reinterpret_cast<InputLoadStringFn>(*slot);
                if (!PatchInputSlot(slot, reinterpret_cast<ULONG_PTR>(InputLoadStringW))) return false;
                loadString = true;
            } else if (lib == "comctl32.dll" && !strcmp(name, "CreatePropertySheetPageW")) {
                // Evidence: the three Text Services pages are created through
                // this import (disasm 0x11E12/0x11E98/0x11F22); serving the
                // translated indirect template here localizes the pages and
                // their tab captions before comctl32 ever reads them.
                g_inputPage = reinterpret_cast<PageProc>(*slot);
                if (!PatchInputSlot(slot, reinterpret_cast<ULONG_PTR>(InputCreatePage))) return false;
                page = true;
            }
        }
    }
    return sheet && dialog && version && loadString && page;
}
bool SameMappedFile(HMODULE module, HANDLE pin) {
    // Check the file actually backing the image, not merely a user-supplied path.
    std::vector<wchar_t> device(MAX_PATH);
    DWORD n = GetMappedFileNameW(GetCurrentProcess(), reinterpret_cast<void*>(
        reinterpret_cast<ULONG_PTR>(module) & ~static_cast<ULONG_PTR>(3)), device.data(), static_cast<DWORD>(device.size()));
    if (n >= device.size()) {
        device.resize(32768);
        n = GetMappedFileNameW(GetCurrentProcess(), reinterpret_cast<void*>(
            reinterpret_cast<ULONG_PTR>(module) & ~static_cast<ULONG_PTR>(3)), device.data(), static_cast<DWORD>(device.size()));
    }
    if (!n || n >= device.size()) return false;
    std::wstring path = L"\\\\?\\GLOBALROOT" + std::wstring(device.data(), n);
    Handle opened(CreateFileW(path.c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                              nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr));
    if (!opened) return false;
    FILE_ID_INFO first{}, second{};
    return GetFileInformationByHandleEx(pin, FileIdInfo, &first, sizeof(first)) &&
        GetFileInformationByHandleEx(opened.value, FileIdInfo, &second, sizeof(second)) &&
        first.VolumeSerialNumber == second.VolumeSerialNumber &&
        !memcmp(first.FileId.Identifier, second.FileId.Identifier, sizeof(first.FileId.Identifier));
}
bool CheckMitigations() {
    PROCESS_MITIGATION_DYNAMIC_CODE_POLICY dynamic{};
    PROCESS_MITIGATION_BINARY_SIGNATURE_POLICY signature{};
    if (GetProcessMitigationPolicy(GetCurrentProcess(), ProcessDynamicCodePolicy, &dynamic, sizeof(dynamic)) &&
        dynamic.ProhibitDynamicCode) return Fail(L"Dynamic-code policy forbids private mapping; not bypassing policy", ERROR_ACCESS_DISABLED_BY_POLICY);
    if (GetProcessMitigationPolicy(GetCurrentProcess(), ProcessSignaturePolicy, &signature, sizeof(signature)) &&
        (signature.MicrosoftSignedOnly || signature.StoreSignedOnly))
        return Fail(L"Signature policy forbids this private mapper; not bypassing policy", ERROR_ACCESS_DISABLED_BY_POLICY);
    return true;
}
// Resolves (once) the private storage directory the pinned payloads live in.
// Shared by Prepare() and the background prefetch worker so both compute the
// same path; g_cache/g_intlPath/g_inputPath are only ever written here, under
// g_cacheLock.
SRWLOCK g_cacheLock = SRWLOCK_INIT;
std::atomic<bool> g_cachePathReady{false};
bool InitPayloadCachePath() {
    AcquireSRWLockExclusive(&g_cacheLock);
    if (g_cachePathReady.load(std::memory_order_acquire)) { ReleaseSRWLockExclusive(&g_cacheLock); return true; }
    wchar_t storage[MAX_PATH]{};
    if (!Wh_GetModStoragePath(storage, ARRAYSIZE(storage))) {
        ReleaseSRWLockExclusive(&g_cacheLock);
        return Fail(L"Windhawk private storage unavailable");
    }
    const std::wstring cache = FullPath(std::wstring(storage) + L"\\win7-x64");
    if (!LocalAbsolute(cache) || cache.size() >= 32000) {
        ReleaseSRWLockExclusive(&g_cacheLock);
        return Fail(L"Invalid private storage path", ERROR_INVALID_NAME);
    }
    // Refuse even a manually misconfigured Windhawk storage under Windows.
    if (cache.size() > g_windows.size() && Equal(cache.substr(0, g_windows.size() + 1), g_windows + L"\\")) {
        ReleaseSRWLockExclusive(&g_cacheLock);
        return Fail(L"Refusing to write under Windows directory", ERROR_ACCESS_DENIED);
    }
    if (!Directory(cache)) { ReleaseSRWLockExclusive(&g_cacheLock); return false; }
    g_cache = cache;
    g_intlPath = g_cache + L"\\intl.cpl";
    g_inputPath = g_cache + L"\\input.dll";
    g_cachePathReady.store(true, std::memory_order_release);
    ReleaseSRWLockExclusive(&g_cacheLock);
    Wh_Log(L"Private payload storage: %s", g_cache.c_str());
    return true;
}
// True when both pinned payloads are on disk with the exact expected size and
// digest. Never touches the network. Callers must have been through a
// successful InitPayloadCachePath() first: that is what publishes g_cache and
// the two payload paths to this thread (they are written under g_cacheLock).
bool PayloadsCached() {
    if (!g_cachePathReady.load(std::memory_order_acquire)) return false;
    ReadFile intl, input;
    return OpenRead(g_intlPath, intl) && intl.bytes.size() == kIntlSize && Hash(intl.bytes) == kIntlSha &&
           OpenRead(g_inputPath, input) && input.bytes.size() == kInputSize && Hash(input.bytes) == kInputSha;
}
// ===== Background payload prefetch (finding 5) =====
// The two pinned payloads are ~620 KB from msdl.microsoft.com. Downloading
// them on the thread that handles the Control Panel activation froze that
// thread - in Explorer, Explorer's own UI thread - for as long as the network
// took, with no feedback. The fetch now runs on a worker thread, and the
// activation path waits for it only up to kPrepareGraceMs. If it is not done
// by then, that one activation shows the native (modern) Region page, which is
// a working page, and the classic one appears on the next activation.
// Prepare() itself never touches the network, so it cannot reintroduce the
// freeze.
// Total time this process may spend waiting for the background fetch, summed
// over every activation. Only a real activation waits at all (CplHook routes
// CPL_INIT/CPL_GETCOUNT/CPL_INQUIRE - the messages the shell sends when it
// merely enumerates the applet - to StartPrefetch instead), but an activation
// can come through more than once before the provider is up, so without a
// shared budget a slow download would freeze the caller once per attempt.
constexpr DWORD kPrepareGraceMs = 5000;
std::atomic<ULONGLONG> g_prepareWaitUsed{0};
std::atomic<bool> g_prefetchRunning{false};
std::atomic<bool> g_prefetchSucceeded{false};
HANDLE g_prefetchDone = nullptr; // manual-reset, created in Wh_ModInit
bool PrefetchPayloads() {
    if (!InitPayloadCachePath()) return false;
    if (PayloadsCached()) return true;
    if (!EnsurePinned(L"intl.cpl", kIntlSha, kIntlSize, kIntlUrl)) return false;
    if (g_stopping.load(std::memory_order_acquire)) { SetLastError(ERROR_SHUTDOWN_IN_PROGRESS); return false; }
    if (!EnsurePinned(L"input.dll", kInputSha, kInputSize, kInputUrl)) return false;
    return PayloadsCached();
}
DWORD WINAPI PrefetchThread(void*) {
    bool ok = false;
    try {
        ok = PrefetchPayloads();
    } catch (...) {
        LastErrorScope keep;
        Wh_Log(L"Payload prefetch threw; staying on the native intl.cpl for now");
        ok = false;
    }
    g_prefetchSucceeded.store(ok, std::memory_order_release);
    g_prefetchRunning.store(false, std::memory_order_release);
    HANDLE done = g_prefetchDone;
    if (done) SetEvent(done);
    return 0;
}
// Idempotent: at most one prefetch attempt is in flight, and no file or network
// I/O happens on the caller's thread - the warm-cache check is the worker's job
// too, so hooking intl.cpl in Explorer costs a CreateThread and nothing else.
// The worker is tracked so the unload path joins it before the mod image goes
// away.
void StartPrefetch() {
    if (g_prefetchSucceeded.load(std::memory_order_acquire)) return;
    if (g_prefetchRunning.exchange(true)) return; // one attempt in flight
    // Same fence BeginJob() uses, for the same reason: Wh_ModBeforeUninit sets
    // g_stopping under g_gate held exclusively and Cleanup() joins the tracked
    // threads afterwards, so reading g_stopping outside the lock could let this
    // create a worker after the join already happened - and PrefetchThread's
    // code lives in the image Windhawk is about to unmap. Creating the thread
    // and registering it must both happen inside the shared hold.
    HANDLE thread = nullptr;
    bool allowed = false;
    AcquireSRWLockShared(&g_gate);
    allowed = !g_stopping.load(std::memory_order_acquire);
    if (allowed) {
        if (g_prefetchDone) ResetEvent(g_prefetchDone);
        thread = CreateThread(nullptr, 0, PrefetchThread, nullptr, 0, nullptr);
        if (thread) RegisterThread(thread); // teardown joins this before unmapping
    }
    ReleaseSRWLockShared(&g_gate);
    if (!thread) {
        g_prefetchRunning.store(false, std::memory_order_release);
        if (allowed) Wh_Log(L"Could not start the payload prefetch worker");
        return;
    }
    CloseHandle(thread);
    Wh_Log(L"Fetching the original Microsoft payloads in the background");
}
bool Prepare() {
    Wh_Log(L"Target provider: authentic Windows 7 x64");
    g_deps.reserve(64); g_inputPatches.reserve(4);
    if (!InitPayloadCachePath()) return false;
    // No network here, on purpose: this runs on the thread handling the
    // activation. The prefetch worker owns the download and EnsurePrepared()
    // only gets here once both payloads are verified in the private cache.
    if (!PayloadsCached()) return Fail(L"Verified payloads missing from the private cache", ERROR_FILE_NOT_FOUND);
    if (!OpenRead(g_intlPath, g_intlFile) || g_intlFile.bytes.size() != kIntlSize || Hash(g_intlFile.bytes) != kIntlSha ||
        !OpenRead(g_inputPath, g_inputFile) || g_inputFile.bytes.size() != kInputSize || Hash(g_inputFile.bytes) != kInputSha)
        return Fail(L"Pin and verify executable payloads", ERROR_CRC);
    PEView intl{g_intlFile.bytes}, input{g_inputFile.bytes};
    if (!intl.Validate(true) || !input.Validate(false)) return Fail(L"Unexpected provider PE contract", ERROR_BAD_EXE_FORMAT);
    std::wstring intlVersion, inputVersion;
    if (!VerifyVersion(g_intlPath, 0x00060001, 0x1DB1446A, intlVersion) ||
        !VerifyVersion(g_inputPath, 0x00060001, 0x1DB04001, inputVersion))
        return Fail(L"Provider version check failed", ERROR_BAD_EXE_FORMAT);
    if (!CheckMitigations()) return false;
    Wh_Log(L"Verified provider: intl.cpl %s AMD64; input.dll %s AMD64", intlVersion.c_str(), inputVersion.c_str());
    Wh_Log(L"PE analysis: intl ImageBase=%p Size=0x%lX Entry=0x%lX Sections=%u TLS=0 LoadConfig=0 DelayImports=0",
        reinterpret_cast<void*>(intl.nt->OptionalHeader.ImageBase), intl.nt->OptionalHeader.SizeOfImage,
        intl.nt->OptionalHeader.AddressOfEntryPoint, intl.nt->FileHeader.NumberOfSections);
    HMODULE ntdll = GetModuleHandleW(L"ntdll.dll");
    g_rtlAddTable = ntdll ? reinterpret_cast<RtlAddTableProc>(reinterpret_cast<void*>(GetProcAddress(ntdll, "RtlAddFunctionTable"))) : nullptr;
    g_rtlDeleteTable = ntdll ? reinterpret_cast<RtlDeleteTableProc>(reinterpret_cast<void*>(GetProcAddress(ntdll, "RtlDeleteFunctionTable"))) : nullptr;
    if (!g_rtlAddTable || !g_rtlDeleteTable) return Fail(L"RtlAddFunctionTable unavailable", ERROR_PROC_NOT_FOUND);
    ACTCTXW act{}; act.cbSize = sizeof(act); act.dwFlags = ACTCTX_FLAG_RESOURCE_NAME_VALID;
    act.lpSource = g_intlPath.c_str(); act.lpResourceName = MAKEINTRESOURCEW(123);
    g_act = CreateActCtxW(&act);
    if (g_act == INVALID_HANDLE_VALUE) return Fail(L"Activate original Win7 common-controls manifest");
    ActScope active(g_act);
    if (!active.active) return Fail(L"ActivateActCtx");
    g_lang = ResolveSelectedLanguage();
    if (g_lang < LangEN || g_lang >= LangCount) g_lang = LangEN;
    // Prepare() just resolved the live setting, so a change queued while the
    // provider was not ready yet is already reflected in the tables below.
    g_pendingLang.store(-1, std::memory_order_release);
    Wh_Log(L"Building embedded resources for language %s (%s)", kLangTags[g_lang], kLangNames[g_lang]);
    if (!BuildEmbeddedResources()) return false;
    if (!BuildEmbeddedInputResources()) return false;
    HMODULE common = SystemDependency("comctl32.dll");
    if (!common) return false;
    g_mainPS = reinterpret_cast<PSProc>(reinterpret_cast<void*>(GetProcAddress(common, "PropertySheetW")));
    g_mainPage = reinterpret_cast<PageProc>(reinterpret_cast<void*>(GetProcAddress(common, "CreatePropertySheetPageW")));
    if (!g_mainPS || !g_mainPage) return Fail(L"Common-controls property sheet exports missing", ERROR_PROC_NOT_FOUND);
    // Preflight Input's static imports, but let Windows do its actual loading.
    Image inputCheck;
    if (!CopyImage(input, inputCheck)) return Fail(L"Allocate Input import preflight");
    bool inputImports = BindImports(inputCheck, false);
    VirtualFree(inputCheck.base, 0, MEM_RELEASE);
    if (!inputImports) return Fail(L"Private input.dll import preflight failed", ERROR_PROC_NOT_FOUND);
    g_inputModule = LoadLibraryExW(g_inputPath.c_str(), nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!g_inputModule) return Fail(L"Windows loader could not load private Win7 input.dll");
    if (!SameMappedFile(g_inputModule, g_inputFile.pin.value)) return Fail(L"Loaded Input is not the pinned file", ERROR_INVALID_DLL);
    for (WORD ordinal : {100, 101, 104, 105, 106, 107, 113, 114}) {
        if (!GetProcAddress(g_inputModule, MAKEINTRESOURCEA(ordinal))) {
            Wh_Log(L"Win7 input.dll is missing required ordinal %u", ordinal); return false;
        }
    }
    if (!AdaptInputIat()) return Fail(L"Private Input modal-lifetime IAT adaptation failed");
    Wh_Log(L"Private Windows 7 input.dll loaded; original Text Services dialogs available");
    // Why the two payloads are loaded differently (deliberate, not oversight):
    //
    // input.dll goes through the real Windows loader above and is only
    // IAT-patched afterwards (AdaptInputIat), which buys real unwind info, TLS
    // callbacks, loader lock semantics and a proper module list entry for free.
    // That works because nothing in its attach path needs a patched import.
    //
    // intl.cpl cannot: its compatibility IAT (BindImports(..., true) below,
    // which routes msvcrt!malloc/free/calloc/realloc/new/delete and the absent
    // Win7 NLS/ETW imports to the private shims) has to be fully in place
    // BEFORE the entry point runs, because DllMain -> _CRT_INIT calls
    // msvcrt!malloc through that very IAT and the genuine attach chain fails
    // closed when it returns NULL (see "Private CRT heap overrides"). The
    // Windows loader binds imports and runs _CRT_INIT inside LoadLibrary with
    // no point in between where the overrides could be installed, so the
    // mapping has to be done here: CopyImage/Relocate/BindImports/ProtectImage
    // + RtlAddFunctionTable + an explicit DllMain call. That is the whole
    // reason this hand-written mapper exists.
    if (!CopyImage(intl, g_image) || !Relocate(g_image) || !BindImports(g_image, true) || !ProtectImage(g_image, intl))
        return Fail(L"Private intl.cpl mapping or import resolution failed");
    auto unwind = intl.nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXCEPTION];
    if (!unwind.Size || unwind.Size % sizeof(RUNTIME_FUNCTION) ||
        !Range(unwind.VirtualAddress, unwind.Size, g_image.size)) return Fail(L"Invalid AMD64 exception directory", ERROR_BAD_EXE_FORMAT);
    g_image.functions = reinterpret_cast<PRUNTIME_FUNCTION>(g_image.base + unwind.VirtualAddress);
    if (!g_rtlAddTable(g_image.functions, static_cast<DWORD>(unwind.Size / sizeof(RUNTIME_FUNCTION)), reinterpret_cast<DWORD64>(g_image.base)))
        return Fail(L"RtlAddFunctionTable failed", ERROR_INVALID_FUNCTION);
    g_image.functionTable = true;
    g_image.entry = reinterpret_cast<EntryProc>(g_image.base + intl.nt->OptionalHeader.AddressOfEntryPoint);
    BOOL attached = FALSE; DWORD exception = 0;
    if (!CallEntry(g_image.entry, reinterpret_cast<HINSTANCE>(g_image.base), DLL_PROCESS_ATTACH, attached, exception) || !attached) {
        DWORD attachError = GetLastError(); // whatever DllMain left behind, if anything
        Wh_Log(L"Legacy DllMain failed: exception=0x%08lX result=%d lastError=%lu", exception, attached, attachError);
        return Fail(L"Legacy entry-point initialization failed", ERROR_DLL_INIT_FAILED);
    }
    g_image.attached = true;
    g_image.cpl = reinterpret_cast<CplProc>(reinterpret_cast<void*>(PrivateExport("CPlApplet")));
    if (!g_image.cpl) return Fail(L"Private CPlApplet export missing", ERROR_PROC_NOT_FOUND);
    Wh_Log(L"Legacy provider mapped at %p; CPlApplet=%p; compatibility layer active (UI not yet opened)",
        g_image.base, reinterpret_cast<void*>(g_image.cpl));
    return true;
}

std::atomic<bool> g_prepareAttempted{false};
SRWLOCK g_prepareLock = SRWLOCK_INIT;
// Prepare() does the verify/map/DllMain work and is expensive on a cold cache;
// the download itself is the prefetch worker's job. Wh_ModInit only installs
// the cheap CPlApplet hook, so this runs once, on the first real activation,
// off the Explorer-startup path (finding 5). The CPL hook already has a clean
// native-fallback path if the provider isn't ready by the time this returns
// false, and every wait here is bounded.
bool EnsurePrepared() {
    if (g_legacyInitialized.load(std::memory_order_acquire)) return true;
    if (g_prepareAttempted.load(std::memory_order_acquire))
        return g_image.cpl != nullptr && g_act != INVALID_HANDLE_VALUE;
    AcquireSRWLockExclusive(&g_prepareLock);
    bool ready;
    if (g_prepareAttempted.load(std::memory_order_acquire)) {
        ready = g_image.cpl != nullptr && g_act != INVALID_HANDLE_VALUE;
    } else {
        StartPrefetch();
        // Spend at most kPrepareGraceMs of this process's total budget here;
        // once it is used up every later activation only polls the event, so a
        // slow download can never freeze the caller again.
        const ULONGLONG used = g_prepareWaitUsed.load(std::memory_order_acquire);
        const DWORD grace = used >= kPrepareGraceMs ? 0
            : static_cast<DWORD>(kPrepareGraceMs - used);
        const ULONGLONG waitStarted = GetTickCount64();
        const bool fetched = g_prefetchDone &&
            WaitForSingleObject(g_prefetchDone, grace) == WAIT_OBJECT_0;
        g_prepareWaitUsed.fetch_add(GetTickCount64() - waitStarted, std::memory_order_acq_rel);
        if (!fetched) {
            // Not latched: the fetch keeps running in the background and the
            // next activation retries. This one gets the native page instead
            // of freezing the caller for the length of a download.
            Wh_Log(L"Payloads not ready after %lu ms of waiting; using the native intl.cpl for this activation",
                   static_cast<unsigned long>(used + (GetTickCount64() - waitStarted)));
            ready = false;
        } else if (!g_prefetchSucceeded.load(std::memory_order_acquire)) {
            // The attempt finished and failed (offline, digest mismatch, ...).
            // Also not latched, so a later activation retries the download.
            Wh_Log(L"Payload download did not produce verified files; using the native intl.cpl for now");
            ready = false;
        } else {
            Wh_Log(L"First activation: preparing Windows 7 private provider now");
            ready = Prepare();
            g_prepareAttempted.store(true, std::memory_order_release);
            if (!ready) Wh_Log(L"Lazy provider preparation failed; falling back to native intl.cpl");
        }
    }
    ReleaseSRWLockExclusive(&g_prepareLock);
    return ready;
}

// ===== Runtime language switch =====
// Wh_ModSettingsChanged no longer reloads the mod, so a language change is
// applied here. The embedded resource tables are handed to comctl32 as raw
// pointers (g_blobs[i].data() becomes a dialog template), so they may only be
// rebuilt at a quiet point: no live CPL session, no window of ours alive, and no
// other legacy call in flight. CplHook calls this on every message and it is a
// single atomic load until all of that holds.
bool ApplyPendingLanguageChange() {
    if (g_pendingLang.load(std::memory_order_acquire) < 0) return true;
    if (g_legacyInitialized.load(std::memory_order_acquire)) return false;
    if (g_active.load(std::memory_order_acquire) > 1) return false; // another legacy call in flight
    if (AnyOwnedWindowAlive()) return false;                        // a dialog of ours is up
    // Provider not mapped yet: Prepare() resolves the live setting itself.
    if (!g_image.cpl || !g_blobCount) return false;
    AcquireSRWLockExclusive(&g_langLock);
    const int pending = g_pendingLang.load(std::memory_order_acquire);
    if (pending < 0) { ReleaseSRWLockExclusive(&g_langLock); return true; }
    const int previous = g_lang.load(std::memory_order_acquire);
    g_lang.store(pending, std::memory_order_release);
    g_pendingLang.store(-1, std::memory_order_release);
    bool ok = BuildEmbeddedResources() && BuildEmbeddedInputResources();
    if (ok) {
        Wh_Log(L"UI language switched at runtime: %s -> %s", kLangTags[previous], kLangTags[pending]);
    } else {
        // Never leave half-built tables behind: restore the previous language.
        g_lang.store(previous, std::memory_order_release);
        ok = BuildEmbeddedResources() && BuildEmbeddedInputResources();
        Wh_Log(L"Runtime language switch to %s failed; restored %s", kLangTags[pending], kLangTags[previous]);
    }
    ReleaseSRWLockExclusive(&g_langLock);
    return ok;
}

// Threads currently inside a legacy CPL call, paired with the host window
// CplHook was invoked with. Both halves are used by CloseOwnedWindows(): the
// thread id narrows the enumeration to threads that are genuinely running
// legacy UI code, and the host HWND is the root the Win32 owner chain of a
// provider-raised window (a MessageBoxW, a modal child) has to trace back to
// before that window may be asked to close. A thread id on its own is not a
// safe filter - the host's own top-level windows live on the same thread, and
// broadcasting WM_CLOSE to all of them closed Shell_TrayWnd, Progman and the
// user's own Control Panel window (finding 3).
// Only CplHook enters this list now; RedirectShellExecuteExW no longer takes
// the rundown gate at all (see that function).
std::vector<std::pair<DWORD, HWND>> g_legacyThreads;
bool EnterLegacy(HWND host = nullptr) {
    AcquireSRWLockShared(&g_gate);
    bool enter = !g_stopping.load(std::memory_order_acquire);
    if (enter && g_active.fetch_add(1, std::memory_order_acq_rel) == 0) ResetEvent(g_idle);
    ReleaseSRWLockShared(&g_gate);
    if (enter) {
        AcquireSRWLockExclusive(&g_windowsLock);
        g_legacyThreads.emplace_back(GetCurrentThreadId(), host);
        ReleaseSRWLockExclusive(&g_windowsLock);
    }
    return enter;
}
void LeaveLegacy() {
    AcquireSRWLockExclusive(&g_windowsLock);
    DWORD tid = GetCurrentThreadId();
    for (auto it = g_legacyThreads.begin(); it != g_legacyThreads.end(); ++it) {
        if (it->first == tid) { g_legacyThreads.erase(it); break; }
    }
    ReleaseSRWLockExclusive(&g_windowsLock);
    AcquireSRWLockExclusive(&g_gate);
    if (g_active.fetch_sub(1, std::memory_order_acq_rel) == 1) SetEvent(g_idle);
    ReleaseSRWLockExclusive(&g_gate);
}
struct LegacyCall { ~LegacyCall() { LeaveLegacy(); } };
// Brings the private provider up when the shell already sent CPL_INIT before
// the payloads were verified. Enumerating the applet must not wait on a cold
// download - that is the shell's own UI thread - so in that case CPL_INIT is
// answered by the native provider and the legacy side is initialized here
// instead, on the activation that actually needs it, replaying the prologue a
// host would have sent. Without this the activation below would find
// g_legacyInitialized false and hand the whole thing to the modern page.
bool InitializeLegacyProvider(HWND window) {
    if (!g_useLegacy.load(std::memory_order_acquire) || !g_image.cpl) return false;
    LONG result = 0; DWORD exception = 0;
    if (!CallCpl(g_image.cpl, window, CPL_INIT, 0, 0, result, exception) || !result) {
        Wh_Log(L"Deferred CPlApplet initialization failed: exception=0x%08lX result=%ld; "
               L"fallback to native intl.cpl", exception, result);
        g_useLegacy.store(false);
        return false;
    }
    // CPL_GETCOUNT is what a host sends next and is where the provider sizes
    // its per-applet state. The shell already enumerated, so the count is not
    // used here - only logged. A failure is not fatal: the provider is up, and
    // marking it initialized keeps the CPL_STOP/CPL_EXIT pairing intact.
    LONG count = 0;
    const bool counted = CallCpl(g_image.cpl, window, CPL_GETCOUNT, 0, 0, count, exception) && count > 0;
    Wh_Log(L"CPlApplet initialized on activation: authentic Windows 7 provider (CPL count = %ld%s)",
           count, counted ? L"" : L", unexpected");
    g_legacyInitialized.store(true);
    g_cplInitThreadId.store(GetCurrentThreadId(), std::memory_order_release);
    return true;
}
LONG CALLBACK CplHook(HWND window, UINT message, LPARAM first, LPARAM second) {
    DWORD entryError = GetLastError();
    // Every path below calls g_nativeCpl. It cannot be null once this hook is
    // live - the detour and the original-function pointer are written by the
    // same Wh_ApplyHookOperations step, and both registration sites require it
    // to have succeeded - but the alternative to checking is a null call inside
    // explorer.exe, so the applet is refused instead.
    if (!g_nativeCpl) {
        Wh_Log(L"ERROR: CPlApplet hook is live without an original function; refusing CPL=%u", message);
        SetLastError(entryError);
        return 0;
    }
    const bool activation = message == CPL_DBLCLK || message == CPL_STARTWPARMSA || message == CPL_STARTWPARMSW;
    // The complement of "the applet is being asked to show something": what the
    // shell sends when it merely lists the applet or tears it down. Deliberately
    // broader than `activation` above (which the native-fallback paths below
    // have always used): anything that is not enumeration or lifetime may need
    // the provider, CPL_SELECT included, and must never be routed to the modern
    // page just because it is not one of the three messages listed there.
    const bool enumeration = message == CPL_INIT || message == CPL_GETCOUNT ||
                             message == CPL_INQUIRE || message == CPL_NEWINQUIRE ||
                             message == CPL_STOP || message == CPL_EXIT;
    // `window` is the host window the applet was invoked with; the unload path
    // uses it as the root that a provider-raised window's owner chain has to
    // trace back to before it may be asked to close (finding 3).
    if (!EnterLegacy(window)) {
        SetLastError(entryError);
        return g_nativeCpl(window, message, first, second);
    }
    try {
        DpiScope dpi;
        LegacyCall call;
        SetLastError(entryError);
        if (g_useLegacy.load() && !g_legacyInitialized.load()) {
            // A real activation may always wait out the grace budget for the
            // payloads. Enumeration (CPL_INIT/CPL_GETCOUNT/CPL_INQUIRE, sent
            // when the shell merely lists the applet - opening the Control
            // Panel folder - which is also the moment LoadLibraryExW_hook
            // installs this hook) must not wait on a *cold* download: that
            // would freeze the shell's UI thread and burn the whole grace
            // budget before the user ever double-clicks Region.
            //
            // It does still prepare here when the verified payloads are already
            // on disk, which is the normal case after the first use. That
            // matters: g_legacyInitialized is set by the CPL_INIT branch below,
            // and a provider that missed CPL_INIT would leave every later
            // message - including the activation - dispatched to the native
            // modern page. When the payloads are not ready yet the provider
            // comes up on the activation instead (InitializeLegacyProvider).
            if (!enumeration || g_prefetchSucceeded.load(std::memory_order_acquire)) EnsurePrepared();
            else StartPrefetch();
        }
        ApplyPendingLanguageChange(); // cheap no-op unless the language changed
        ActScope act(g_act);
        g_uiCreated = 0; g_uiFailed = false;
        if (message == CPL_INIT) {
            // Keep native lifetime valid even if the mod is disabled while the host
            // is holding CPL metadata. The native provider shows no UI on CPL_INIT.
            LONG native = g_nativeCpl(window, message, first, second);
            g_nativeInitialized.store(native != 0);
            // act.active/g_image.cpl are empty until Prepare() has run, which a
            // plain enumeration deliberately does not wait for.
            if (!native || !g_useLegacy.load() || !act.active || !g_image.cpl) return native;
            LONG legacy = 0; DWORD exception = 0;
            if (!CallCpl(g_image.cpl, window, message, first, second, legacy, exception) || !legacy) {
                Wh_Log(L"CPlApplet initialization failed: exception=0x%08lX result=%ld; fallback to native intl.cpl", exception, legacy);
                g_useLegacy.store(false); return native;
            }
            g_legacyInitialized.store(true);
            g_cplInitThreadId.store(GetCurrentThreadId(), std::memory_order_release);
            Wh_Log(L"CPlApplet initialized: authentic Windows 7 provider");
            return legacy;
        }
        if (!g_useLegacy.load() || !act.active) {
            if (message == CPL_EXIT) g_nativeInitialized.store(false);
            return g_nativeCpl(window, message, first, second);
        }
        if (!g_legacyInitialized.load()) {
            // Only a message that wants UI may bring the provider up late:
            // CPL_STOP/CPL_EXIT must never map a provider that was not used,
            // and the enumeration messages have nothing to dispatch to it.
            if (enumeration || !InitializeLegacyProvider(window)) {
                if (message == CPL_EXIT) g_nativeInitialized.store(false);
                return g_nativeCpl(window, message, first, second);
            }
        }
        // The original DLL starts notification/launch workers. Do not let its
        // globals disappear while those workers still use the private image.
        if (message == CPL_EXIT) WaitForJobs(kUiJobWaitMs);
        LONG result = 0; DWORD exception = 0;
        const bool returned = CallCpl(g_image.cpl, window, message, first, second, result, exception);
        DWORD error = GetLastError();
        if (message == CPL_GETCOUNT && returned) Wh_Log(L"CPL count = %ld", result);
        if (message == CPL_EXIT) {
            g_legacyInitialized.store(false);
            g_cplInitThreadId.store(0, std::memory_order_release);
            g_nativeInitialized.store(false);
            g_nativeCpl(window, message, first, second);
        } else if (message == CPL_STOP) {
            g_nativeCpl(window, message, first, second);
        }
        if (!returned || (activation && g_uiCreated == 0 && g_uiFailed)) {
            Wh_Log(L"Legacy failure: CPL=%u exception=0x%08lX Win32=%lu; fallback to native intl.cpl",
                   message, exception, error);
            g_useLegacy.store(false);
            if (activation) return g_nativeCpl(window, message, first, second);
            if (!returned && message != CPL_EXIT && message != CPL_STOP)
                return g_nativeCpl(window, message, first, second);
        }
        SetLastError(error); return result;

    } catch (...) {
        Wh_Log(L"CplHook C++ exception on CPL=%u; legacy disabled, native fallback", message);
        g_useLegacy.store(false);
        SetLastError(entryError);
        if (activation) return g_nativeCpl(window, message, first, second);
        return 0;
    }
}
// Candidate windows for the unload close request (finding 3).
struct CloseScan {
    DWORD pid = 0;
    std::vector<HWND> owned;  // windows the mod created and tracked itself
    std::vector<HWND> hosts;  // host windows a legacy CPL call was invoked with
};
// True when `candidate`'s Win32 owner chain reaches one of the roots: a window
// the mod tracked in g_owned, or a host window a legacy call was invoked with.
// Those are the only untracked windows the unload path may ask to close - a
// MessageBoxW, a modal child, a helper dialog the provider raised itself.
bool OwnedByTrackedRoot(HWND candidate, const CloseScan& scan) {
    // Depth-limited and owner-verified: an owner cycle or a bogus chain must
    // not loop, and a window owned by another process is never a candidate.
    for (int depth = 0; depth < 8 && candidate; ++depth) {
        const HWND owner = GetWindow(candidate, GW_OWNER);
        if (!owner) return false;
        DWORD ownerPid = 0;
        if (!GetWindowThreadProcessId(owner, &ownerPid) || ownerPid != scan.pid) return false;
        for (HWND root : scan.owned) if (root && root == owner) return true;
        for (HWND root : scan.hosts) if (root && root == owner) return true;
        candidate = owner;
    }
    return false;
}
BOOL CALLBACK CloseScanProc(HWND window, LPARAM lparam) {
    const auto* scan = reinterpret_cast<const CloseScan*>(lparam);
    try {
        DWORD pid = 0;
        if (!scan || !window || !GetWindowThreadProcessId(window, &pid) || pid != scan->pid) return TRUE;
        // The host window itself is never closed: in the plain CPL case that is
        // the user's own Control Panel window, in Explorer it is the shell's
        // Control Panel container. Only windows *owned by* it are candidates.
        for (HWND host : scan->hosts) if (host == window) return TRUE;
        for (HWND own : scan->owned) if (own == window) return TRUE; // handled by pass 1
        if (!OwnedByTrackedRoot(window, *scan)) return TRUE;
        PostMessageW(window, WM_CLOSE, 0, 0);
    } catch (...) {
        // Best-effort: keep enumerating, the outer unload loop retries anyway.
    }
    return TRUE;
}
// `force` escalates from "please close" to "this mod is ending the UI it
// created itself". It exists because the unload wait is unbounded on purpose
// (see the block comment at "Unload rundown"): a WM_CLOSE that a Win7 DLGPROC
// ignores, or a nested modal child that never sees it, would otherwise hold the
// unload open forever. Everything it force-closes is a window this mod created,
// served and is still tracking, so it can legitimately end them.
void CloseOwnedWindows(bool force) {
    // Called in a loop from Wh_ModBeforeUninit's unload wait; an exception
    // here must not abort that loop early and skip requesting the close.
    try {
        CloseScan scan;
        scan.pid = GetCurrentProcessId();
        std::vector<DWORD> threads;
        std::vector<HWND> sheets, dialogs;
        AcquireSRWLockShared(&g_windowsLock);
        scan.owned.reserve(g_owned.size());
        for (const auto& entry : g_owned) {
            if (!entry.hwnd) continue;
            scan.owned.push_back(entry.hwnd);
            if (entry.sheet) sheets.push_back(entry.hwnd);
            else dialogs.push_back(entry.hwnd);
        }
        scan.hosts.reserve(g_legacyThreads.size());
        threads.reserve(g_legacyThreads.size());
        for (const auto& entry : g_legacyThreads) {
            if (!entry.second) continue;
            scan.hosts.push_back(entry.second);
            bool seen = false;
            for (DWORD tid : threads) if (tid == entry.first) seen = true;
            if (!seen) threads.push_back(entry.first);
        }
        ReleaseSRWLockShared(&g_windowsLock);

        // 1. Windows the mod itself created and tracked (property sheets and
        //    dialogs registered through Own()).
        for (HWND window : scan.owned) {
            DWORD process = 0;
            if (window && GetWindowThreadProcessId(window, &process) && process == scan.pid) {
                PostMessageW(window, WM_CLOSE, 0, 0);
            }
        }
        // 2. Windows the legacy provider raised on its own that never went
        //    through the mod's tracking. Only threads that are actually inside
        //    a legacy CPL call are scanned, and within those only windows whose
        //    owner chain traces back to a tracked or host window are asked to
        //    close - never a whole thread (finding 3).
        for (DWORD tid : threads) EnumThreadWindows(tid, CloseScanProc, reinterpret_cast<LPARAM>(&scan));

        if (!force) return;
        // 3. Escalation, children first: a nested modal dialog is what keeps
        //    its parent sheet's loop running.
        g_forceCloseUi.store(true, std::memory_order_release);
        //    Modal dialogs: DialogCallback sees WM_CLOSE with the flag set and
        //    calls EndDialog from the dialog's own thread, which is the only
        //    thread allowed to end it. The unload loop re-posts every 100 ms, so
        //    a dialog that is pumping will take it.
        for (HWND window : dialogs) {
            DWORD process = 0;
            if (GetWindowThreadProcessId(window, &process) && process == scan.pid)
                PostMessageW(window, WM_CLOSE, 0, 0);
        }
        //    Property sheets: press Cancel. PSM_PRESSBUTTON is the documented
        //    programmatic close and is safe to send cross-thread; ABORTIFHUNG
        //    keeps a wedged sheet from blocking the unload thread instead.
        for (HWND window : sheets) {
            DWORD process = 0;
            if (!GetWindowThreadProcessId(window, &process) || process != scan.pid) continue;
            DWORD_PTR ignored = 0;
            SendMessageTimeoutW(window, PSM_PRESSBUTTON, PSBTN_CANCEL, 0,
                                SMTO_ABORTIFHUNG | SMTO_NORMAL, 1000, &ignored);
        }
    } catch (...) {
        // Best-effort: the outer unload loop keeps retrying regardless.
    }
}
HHOOK g_shutdownHook = nullptr;
// Set by ShutdownHookProc as its last action on the init thread. The unload
// thread never treats this as "the hook proc has returned": at the moment it is
// signalled, that proc's epilogue and return address are still live inside this
// mod's image (finding 1).
HANDLE g_shutdownWorked = nullptr;
// Joinable completion point. It does nothing but wait for g_shutdownWorked and
// exit, so the unload thread can wait on a real thread handle - the only signal
// on this path whose completion can be genuinely joined.
HANDLE g_shutdownSentinel = nullptr;
std::atomic<bool> g_shutdownRan{false};
std::atomic<bool> g_detachDone{false};
constexpr DWORD kShutdownMarshalMs = 5000;
// The actual legacy teardown; must run on the thread that ran CPL_INIT.
void ShutdownLegacyOnCurrentThread() {
    if (g_legacyInitialized.exchange(false) && g_image.cpl) {
        LONG ignored = 0; DWORD exception = 0;
        CallCpl(g_image.cpl, nullptr, CPL_EXIT, 0, 0, ignored, exception);
        if (exception) Wh_Log(L"Legacy CPL_EXIT exception=0x%08lX", exception);
    }
    g_cplInitThreadId.store(0, std::memory_order_release);
    // exchange() because the marshal timeout path can end up running this both
    // inline and on the init thread: DLL_PROCESS_DETACH must happen exactly once.
    if (g_image.attached && g_image.entry && !g_detachDone.exchange(true)) {
        BOOL ignored = FALSE; DWORD exception = 0;
        CallEntry(g_image.entry, reinterpret_cast<HINSTANCE>(g_image.base), DLL_PROCESS_DETACH, ignored, exception);
        g_image.attached = false;
    }
}
LRESULT CALLBACK ShutdownHookProc(int code, WPARAM wParam, LPARAM lParam) {
    // A hook callback must never let a C++ exception cross back into the
    // system hook-chain dispatcher; the waiting unload thread is released
    // even if ShutdownLegacyOnCurrentThread() throws.
    //
    // The chain call runs BEFORE the signal on purpose. What is left of this
    // proc after SetEvent is a load of an already-computed result plus the
    // return, so no mod code runs on this thread after the unload thread is
    // released and no mod return address stays on this thread's stack past
    // the signal. Relying on the compiler to emit a tail call for the
    // CallNextHookEx below would be optimizer-dependent - this function has
    // a try block - and memory safety must not rest on that (finding 1).
    const bool ran = code == HC_ACTION && !g_shutdownRan.exchange(true);
    if (ran) {
        try {
            ShutdownLegacyOnCurrentThread();
        } catch (...) {
            LastErrorScope keep;
            Wh_Log(L"Exception during marshaled legacy shutdown; continuing teardown");
        }
    }
    // The unhook is the unload thread's job (ShutdownMarshalRelease), so
    // g_shutdownHook is only ever touched from one thread.
    //
    // The unload thread does not resume on this thread's epilogue either - it
    // joins the sentinel thread instead, which gives this epilogue the whole
    // wake/schedule/exit latency of another thread to retire before
    // Wh_ModUninit returns and Windhawk unmaps the image.
    const LRESULT result = CallNextHookEx(nullptr, code, wParam, lParam);
    if (ran && g_shutdownWorked) SetEvent(g_shutdownWorked);
    return result;
}
DWORD WINAPI ShutdownSentinel(void* parameter) {
    if (parameter) WaitForSingleObject(static_cast<HANDLE>(parameter), kShutdownMarshalMs);
    return 0;
}
void ShutdownMarshalRelease() {
    // Unhook first: after this no new invocation of ShutdownHookProc can start.
    if (g_shutdownHook) { HHOOK h = g_shutdownHook; g_shutdownHook = nullptr; UnhookWindowsHookEx(h); }
    HANDLE sentinel = g_shutdownSentinel; g_shutdownSentinel = nullptr;
    HANDLE worked = g_shutdownWorked; g_shutdownWorked = nullptr;
    if (sentinel) {
        // Never close the event out from under the sentinel's own wait: a
        // recycled handle value would leave it waiting on somebody else's
        // object. Join it (bounded) and leak rather than risk it.
        if (WaitForSingleObject(sentinel, 2000) == WAIT_OBJECT_0) CloseHandle(sentinel);
        else Wh_Log(L"Shutdown sentinel did not exit; leaking its handle");
    }
    // Same rule for the event the hook proc signals: close it only once it is
    // actually signalled, which proves SetEvent already returned. Otherwise an
    // invocation that is still in flight would signal a closed handle.
    if (worked) {
        if (WaitForSingleObject(worked, 0) == WAIT_OBJECT_0) CloseHandle(worked);
        else Wh_Log(L"Shutdown marshal event never signalled; leaking its handle");
    }
}
// Marshals the legacy CPL_EXIT/DLL_PROCESS_DETACH teardown to the thread
// that ran CPL_INIT: that thread's Cicero/TSF and COM objects have thread
// affinity there, while Wh_ModUninit runs on an arbitrary Windhawk thread
// with no COM apartment initialized (finding 5).
//
// This one keeps its own fixed budget (kShutdownMarshalMs plus slack for the
// sentinel) rather than joining the drain: by the time Cleanup() gets here the
// drain has already proved g_active == 0, so no legacy call is in flight on any
// thread and a timeout can only mean the init thread is gone or not pumping -
// in which case running the teardown inline is the correct fallback, not a
// race. The budget is what keeps a dead thread from hanging the unload.
void ShutdownLegacyOnInitThread() {
    DWORD tid = g_cplInitThreadId.load(std::memory_order_acquire);
    if (!tid || tid == GetCurrentThreadId()) { ShutdownLegacyOnCurrentThread(); return; }
    try {
        g_shutdownRan.store(false, std::memory_order_release);
        g_shutdownWorked = CreateEventW(nullptr, TRUE, FALSE, nullptr);
        if (!g_shutdownWorked) { ShutdownLegacyOnCurrentThread(); return; }
        // The wait below targets this thread's handle, not the hook proc's
        // event: a thread handle is the one completion signal here that the
        // unload thread can genuinely join (finding 1).
        g_shutdownSentinel = CreateThread(nullptr, 0, ShutdownSentinel, g_shutdownWorked, 0, nullptr);
        HMODULE self = nullptr;
        // UNCHANGED_REFCOUNT is required. Without it GetModuleHandleExW takes a
        // reference on this mod's own module that nothing ever releases, so the
        // single FreeLibrary Windhawk issues right after Wh_ModUninit returns is
        // a no-op: the mod image, its globals and the mapped Win7 provider stay
        // in the host forever, and the next enable/update loads a second copy.
        // The handle is only needed for the duration of the SetWindowsHookExW
        // call, and the module cannot go away while we are running inside it.
        if (GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                                   GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                reinterpret_cast<LPCWSTR>(&ShutdownHookProc), &self) && self)
            g_shutdownHook = SetWindowsHookExW(WH_GETMESSAGE, ShutdownHookProc, self, tid);
        if (!g_shutdownHook) {
            Wh_Log(L"Could not marshal legacy shutdown to init thread %lu; running inline", tid);
            ShutdownMarshalRelease();
            ShutdownLegacyOnCurrentThread();
            return;
        }
        PostThreadMessageW(tid, WM_NULL, 0, 0);
        // The sentinel's own wait is kShutdownMarshalMs; give it slack on top.
        const DWORD budget = g_shutdownSentinel ? kShutdownMarshalMs + 2000 : kShutdownMarshalMs;
        const HANDLE target = g_shutdownSentinel ? g_shutdownSentinel : g_shutdownWorked;
        if (WaitForSingleObject(target, budget) != WAIT_OBJECT_0) {
            Wh_Log(L"Legacy shutdown marshal to init thread %lu timed out; running inline", tid);
            ShutdownMarshalRelease();
            // Safe to run again here: ShutdownLegacyOnCurrentThread() is idempotent
            // (g_legacyInitialized and g_detachDone are exchanged, not tested).
            ShutdownLegacyOnCurrentThread();
            return;
        }
        ShutdownMarshalRelease();
    } catch (...) {
        // Make sure the handle/hook never leak and the teardown still runs.
        ShutdownMarshalRelease();
        Wh_Log(L"Exception marshaling legacy shutdown; running inline");
        ShutdownLegacyOnCurrentThread();
    }
}
// Restores the private input.dll IAT slots this mod patched. Restore only
// slots we own; normally called after legacy modal calls have left.
void RestoreInputIat() {
    for (auto it = g_inputPatches.rbegin(); it != g_inputPatches.rend(); ++it) {
        DWORD old = 0, ignored = 0;
        if (VirtualProtect(it->slot, sizeof(*it->slot), PAGE_READWRITE, &old)) {
            InterlockedCompareExchangePointer(reinterpret_cast<PVOID volatile*>(it->slot),
                reinterpret_cast<void*>(it->before), reinterpret_cast<void*>(it->after));
            VirtualProtect(it->slot, sizeof(*it->slot), old, &ignored);
        }
    }
    g_inputPatches.clear();
}
void Cleanup() {
    // Unbounded, and it has to be - see "Unload rundown" at the top of the file
    // for the four ways the mapped provider still points into this mod's image.
    // What is bounded is each step leading up to it: ask the UI to close, and
    // after kUnloadEscalateMs force-close the dialogs and sheets this mod
    // created itself. That keeps this path correct even when Cleanup() is
    // reached without Wh_ModBeforeUninit's loop (initialisation failure), and
    // makes the unconditional wait below something nobody can be held open by.
    bool drained = WaitForLegacyIdle(kUnloadEscalateMs) && WaitForJobs(kUnloadEscalateMs);
    if (!drained) {
        CloseOwnedWindows(false);
        drained = WaitForLegacyIdle(kUnloadEscalateMs) && WaitForJobs(kUnloadEscalateMs);
    }
    if (!drained) {
        Wh_Log(L"Private-provider UI still open at teardown; force-closing the dialogs and "
               L"sheets this mod created");
        CloseOwnedWindows(true);
        drained = WaitForLegacyIdle(INFINITE) && WaitForJobs(INFINITE);
    }
    // EndJob() (the job-count signal) fires before ThreadBridge actually
    // returns, so wait for the real thread handles too before anything below
    // unmaps the mod image their epilogues still execute in (finding 4).
    const bool joined = JoinTrackedThreads(INFINITE);
    if (!drained || !joined) {
        // Only reachable if an INFINITE wait returned something other than
        // WAIT_OBJECT_0, which cannot happen with valid handles. Continuing is
        // the lesser evil versus a Cleanup() that never returns, but it must be
        // loud: it means the image below is about to be freed with something
        // still running in it.
        Wh_Log(L"ERROR: unload rundown reported failure (drained=%d joined=%d); continuing teardown",
               drained ? 1 : 0, joined ? 1 : 0);
    }
    // Last check before the image goes away: everything this mod served is
    // modal, so nothing of ours should still be alive here. A hit means a
    // DLGPROC/callback in this image is still reachable from a live window.
    AcquireSRWLockShared(&g_windowsLock);
    for (const auto& entry : g_owned)
        if (entry.hwnd) Wh_Log(L"WARNING: window %p served by this mod is still alive at teardown", entry.hwnd);
    ReleaseSRWLockShared(&g_windowsLock);
    ActScope act(g_act);
    ShutdownLegacyOnInitThread();
    // Restore only slots we own, after legacy modal calls have left. They point
    // into this mod's image, so an in-flight legacy call finishing through the
    // original function is survivable; leaving them pointing at an unmapped
    // image is not - and on a reload the next instance would record our dead
    // hook as "the original".
    RestoreInputIat();
    if (g_image.functionTable && g_rtlDeleteTable) { g_rtlDeleteTable(g_image.functions); g_image.functionTable = false; }
    if (g_image.base) VirtualFree(g_image.base, 0, MEM_RELEASE);
    g_image = {};
    if (g_inputModule) {
        // Release any extra references handed to the legacy code that it
        // never balanced itself (finding 6), then the base reference.
        LONG extra = g_inputExtraRefs.exchange(0, std::memory_order_acq_rel);
        for (LONG i = 0; i < extra; ++i) FreeLibrary(g_inputModule);
        FreeLibrary(g_inputModule);
        g_inputModule = nullptr;
    }
    for (auto it = g_deps.rbegin(); it != g_deps.rend(); ++it) FreeLibrary(it->module);
    g_deps.clear();
    for (auto& store : g_blobStore) { std::vector<BYTE> empty; store.swap(empty); }
    g_blobCount = 0;
    g_intlFile = {}; g_inputFile = {};
    // ActScope must deactivate before the final context reference is released.
}
// ===== Optional ms-settings redirect (interoperability, Explorer only) ====
// When the redirectSettings option is on, modern Settings Region/Language
// links opened from Explorer (tray, Start, help links) land on the restored
// classic Region dialog instead of the Settings app. Only the two documented
// Region/Language pages are matched (ms-settings:regionlanguage* and
// ms-settings:regionformatting*, any subpage); Date & time, Speech, Typing
// and every other page pass through untouched. The rewrite targets
// control.exe intl.cpl, which this mod already serves in full mode.
Host g_host = Host::Other;
bool g_envViable = false;
// Read by the three redirect hooks on entry. Wh_ModSettingsChanged flips it, so
// turning the redirect on or off takes effect immediately and does not need a
// mod reload (which would run the whole unload rundown).
std::atomic<bool> g_redirectWanted{false};
using ShellExecuteExWFn = BOOL (WINAPI*)(SHELLEXECUTEINFOW*);
using ShellExecuteWFn = HINSTANCE (WINAPI*)(HWND, LPCWSTR, LPCWSTR, LPCWSTR, LPCWSTR, INT);
using CreateProcessWFn = BOOL (WINAPI*)(LPCWSTR, LPWSTR, LPSECURITY_ATTRIBUTES, LPSECURITY_ATTRIBUTES, BOOL, DWORD, LPVOID, LPCWSTR, LPSTARTUPINFOW, LPPROCESS_INFORMATION);
ShellExecuteExWFn g_origShellExecuteExW = nullptr;
ShellExecuteWFn g_origShellExecuteW = nullptr;
CreateProcessWFn g_origCreateProcessW = nullptr;
std::wstring g_redirectExe;
bool SettingsUrlPrefix(const wchar_t* s, const wchar_t* prefix) {
    if (!s || !prefix) return false;
    while (*s == L' ' || *s == L'\t' || *s == L'"') ++s; // tolerate quoting
    for (; *prefix; ++s, ++prefix) if (Fold(*s) != *prefix) return false;
    return *s == 0 || *s == L'?' || *s == L'/' || *s == L'#' || *s == L'&' || *s == L'"';
}
bool IsRegionSettingsUrl(const wchar_t* s) {
    return SettingsUrlPrefix(s, L"ms-settings:regionlanguage") ||
           SettingsUrlPrefix(s, L"ms-settings:regionformatting");
}
bool ContainsRegionSettingsUrl(const wchar_t* s) {
    // Launchers that carry the URL inside parameters (quote-tolerant scan).
    if (!s) return false;
    for (const wchar_t* p = s; *p; ++p)
        if (IsRegionSettingsUrl(p)) return true;
    return false;
}
bool RedirectTarget(const SHELLEXECUTEINFOW* info) {
    return IsRegionSettingsUrl(info->lpFile) || ContainsRegionSettingsUrl(info->lpParameters);
}
BOOL WINAPI RedirectShellExecuteExW(SHELLEXECUTEINFOW* info) {
    auto orig = g_origShellExecuteExW;
    if (!orig) { SetLastError(ERROR_INVALID_FUNCTION); return FALSE; }
    if (!g_redirectWanted.load(std::memory_order_acquire)) return orig(info);
    // Deliberately no EnterLegacy()/LeaveLegacy() here (finding 3). This hook
    // only rewrites a ms-settings: URL into "control.exe intl.cpl" and hands
    // the call straight to the original ShellExecuteExW: it never touches the
    // mapped Win7 image, the provider's globals, or any state with a lifetime
    // the unload path has to wait for. Taking the rundown gate made every
    // ShellExecuteExW in Explorer - the taskbar, the Start menu, the desktop
    // launching programs - count as an in-flight legacy call, which
    //   (a) kept g_active non-zero for as long as a UAC consent prompt was on
    //       screen, feeding the unload wait, and
    //   (b) put those threads in g_legacyThreads, where the unload path used
    //       to broadcast WM_CLOSE to all of their top-level windows.
    // g_active means "inside a call into the mapped Win7 image", which a URL
    // rewrite is not. The two sibling hooks below (RedirectShellExecuteW,
    // RedirectCreateProcessW) never took the gate either, so this makes the
    // three consistent. The generic "a thread may still be inside a hook frame
    // when Windhawk unhooks and unmaps" window is the same for all three and is
    // not what the rundown gate was for; using it here only ever bought a way
    // to hang the unload on somebody else's UAC prompt.
    try {
        if (info && info->cbSize >= sizeof(*info) && !g_redirectExe.empty() && RedirectTarget(info)) {
            SHELLEXECUTEINFOW copy = *info;
            copy.lpFile = g_redirectExe.c_str();
            copy.lpParameters = L"intl.cpl";
            copy.lpDirectory = nullptr;
            BOOL result = orig(&copy);
            {
                LastErrorScope keep; // the log below must keep the real error
                Wh_Log(L"Settings redirect: Region/Language page -> classic Region dialog");
            }
            info->hInstApp = copy.hInstApp;
            if (info->fMask & SEE_MASK_NOCLOSEPROCESS) info->hProcess = copy.hProcess;
            return result;
        }
    } catch (...) {
        LastErrorScope keep;
        Wh_Log(L"Settings redirect failed internally; passing through");
    }
    return orig(info);
}
HINSTANCE WINAPI RedirectShellExecuteW(HWND hwnd, LPCWSTR verb, LPCWSTR file,
                                       LPCWSTR parameters, LPCWSTR directory, INT show) {
    auto orig = g_origShellExecuteW;
    if (!orig) return FALSE;
    if (!g_redirectWanted.load(std::memory_order_acquire))
        return orig(hwnd, verb, file, parameters, directory, show);
    try {
        if (IsRegionSettingsUrl(file) || ContainsRegionSettingsUrl(parameters)) {
            return orig(hwnd, verb, g_redirectExe.c_str(), L"intl.cpl", directory, show);
        }
    } catch (...) {}
    return orig(hwnd, verb, file, parameters, directory, show);
}

BOOL WINAPI RedirectCreateProcessW(LPCWSTR applicationName, LPWSTR commandLine,
    LPSECURITY_ATTRIBUTES processAttributes, LPSECURITY_ATTRIBUTES threadAttributes,
    BOOL inheritHandles, DWORD creationFlags, LPVOID environment,
    LPCWSTR currentDirectory, LPSTARTUPINFOW startupInfo,
    LPPROCESS_INFORMATION processInformation) {
    auto orig = g_origCreateProcessW;
    if (!orig) return FALSE;
    if (!g_redirectWanted.load(std::memory_order_acquire))
        return orig(applicationName, commandLine, processAttributes, threadAttributes,
                    inheritHandles, creationFlags, environment, currentDirectory,
                    startupInfo, processInformation);
    try {
        if (commandLine && ContainsRegionSettingsUrl(commandLine)) {
            std::wstring replacement = L"\"" + g_redirectExe + L"\" intl.cpl";
            std::vector<wchar_t> mutableCommand(replacement.begin(), replacement.end());
            mutableCommand.push_back(L'\0');
            return orig(nullptr, mutableCommand.data(), processAttributes,
                        threadAttributes, inheritHandles, creationFlags, environment,
                        currentDirectory, startupInfo, processInformation);
        }
    } catch (...) {}
    return orig(applicationName, commandLine, processAttributes, threadAttributes,
                inheritHandles, creationFlags, environment, currentDirectory,
                startupInfo, processInformation);
}

bool InstallRedirectHook() {
    if (g_origShellExecuteExW) return true;
    g_redirectExe = g_system + L"\\control.exe";
    HMODULE shell32 = GetModuleHandleW(L"shell32.dll");
    auto target = shell32 ? reinterpret_cast<ShellExecuteExWFn>(reinterpret_cast<void*>(
                      GetProcAddress(shell32, "ShellExecuteExW")))
                          : nullptr;
    if (!target) return false;
    bool ok = WindhawkUtils::SetFunctionHook(target, RedirectShellExecuteExW,
                                             &g_origShellExecuteExW);
    HMODULE kernel32 = GetModuleHandleW(L"kernel32.dll");
    auto createProcess = kernel32 ? reinterpret_cast<CreateProcessWFn>(
        reinterpret_cast<void*>(GetProcAddress(kernel32, "CreateProcessW"))) : nullptr;
    auto shellExecute = shell32 ? reinterpret_cast<ShellExecuteWFn>(
        reinterpret_cast<void*>(GetProcAddress(shell32, "ShellExecuteW"))) : nullptr;
    if (shellExecute)
        ok = WindhawkUtils::SetFunctionHook(shellExecute, RedirectShellExecuteW,
                                            &g_origShellExecuteW) && ok;
    if (createProcess)
        ok = WindhawkUtils::SetFunctionHook(createProcess, RedirectCreateProcessW,
                                            &g_origCreateProcessW) && ok;
    return ok;
}
// ===== Explorer in-process Control Panel hook (finding 1) =====
// In Explorer, intl.cpl is not force-loaded at startup (that would make the
// mod resident for nothing when the Region page is never opened). Instead,
// hook the loader itself so the CPlApplet hook is installed the moment
// Explorer loads intl.cpl for its own in-process
// shell32!Control_RunDLLW path — the classic "Control Panel > Region" route,
// which is the mod's headline scenario.
using LoadLibraryExWProc = HMODULE(WINAPI*)(LPCWSTR, HANDLE, DWORD);
LoadLibraryExWProc g_origLoadLibraryExW = nullptr;
// "Settled", not "installed": set once the Explorer in-process route is either
// hooked or has failed to hook, so the loader hook stops trying. Read only
// there. The name predates the failure latch.
std::atomic<bool> g_explorerCplHooked{false};
// Shared with the eager Control/Rundll32 path in Wh_ModInit.
bool HookNativeCpl(HMODULE module) {
    try {
        auto native = reinterpret_cast<CplProc>(reinterpret_cast<void*>(GetProcAddress(module, "CPlApplet")));
        if (!native) {
            Wh_Log(L"Fallback to native intl.cpl: CPlApplet export missing; no CPL hook installed");
            return false;
        }
        if (!WindhawkUtils::SetFunctionHook(native, CplHook, &g_nativeCpl)) {
            Wh_Log(L"CPlApplet hook registration failed; fallback to native");
            return false;
        }
        g_nativeModule = module;
        return true;
    } catch (...) {
        return false;
    }
}
HMODULE WINAPI LoadLibraryExW_hook(LPCWSTR name, HANDLE file, DWORD flags) {
    HMODULE result = g_origLoadLibraryExW ? g_origLoadLibraryExW(name, file, flags)
                                           : LoadLibraryExW(name, file, flags);
    try {
        if (result && name && !g_explorerCplHooked.load(std::memory_order_acquire)) {
            std::wstring requested = Slashes(name);
            size_t slash = requested.rfind(L'\\');
            std::wstring leaf = requested.substr(slash == std::wstring::npos ? 0 : slash + 1);
            if (Equal(leaf, L"intl.cpl")) {
                // Take our own reference so the module stays resident once
                // hooked: Explorer releases its own reference on CPL_EXIT,
                // and the hooked CPlApplet export must stay valid for later
                // activations in the same Explorer process.
                HMODULE pinned = g_origLoadLibraryExW
                    ? g_origLoadLibraryExW(name, nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32)
                    : LoadLibraryExW(name, nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
                // Wh_SetFunctionHook only *registers* an operation: Windhawk
                // applies the registrations automatically exactly once, right
                // after Wh_ModInit returns, and that is also when the original-
                // function pointer gets written. This registration happens long
                // after init, so without Wh_ApplyHookOperations() the CPlApplet
                // detour never exists - the bookkeeping below would all succeed
                // while CplHook was never entered and g_nativeCpl stayed null,
                // leaving Explorer's in-process Control Panel > Region route on
                // the modern page.
                const bool registered = pinned && HookNativeCpl(pinned);
                const bool hooked = registered && Wh_ApplyHookOperations() && g_nativeCpl;
                if (hooked) {
                    // An activation is about to follow on this thread; start the
                    // payload fetch now so the download runs in the background
                    // instead of on the activation thread (finding 5).
                    StartPrefetch();
                    Wh_Log(L"intl.cpl loaded in Explorer; CPlApplet hook installed");
                } else if (!registered) {
                    // Nothing was queued and nothing was adopted, so the extra
                    // reference is simply ours to drop.
                    if (pinned) FreeLibrary(pinned);
                    Wh_Log(L"WARNING: CPlApplet hook could not be registered in Explorer; "
                           L"the in-process Control Panel > Region route stays on the modern page");
                } else {
                    // Registered but not applied. Deliberately keeps our module
                    // reference (HookNativeCpl adopted it into g_nativeModule):
                    // the operation is still queued inside Windhawk, and any
                    // later Wh_ApplyHookOperations() - a settings edit, for
                    // instance - would apply it. Releasing the pin here could
                    // then leave a live detour pointing into a module Explorer
                    // is free to unmap. Cleanup() drops the reference on unload.
                    Wh_Log(L"ERROR: CPlApplet hook registered but Wh_ApplyHookOperations "
                           L"failed in Explorer; the in-process Control Panel > Region route "
                           L"stays on the modern page");
                }
                // Settled either way. Retrying on the next load would register
                // the same target again and re-apply everything from inside a
                // loader hook, and an apply that failed once is unlikely to
                // fail differently next time.
                g_explorerCplHooked.store(true, std::memory_order_release);
            }
        }
    } catch (...) {
        // Never let a bookkeeping failure here affect the real load.
    }
    return result;
}
bool InstallExplorerCplHook() {
    // Already loaded before we got here (e.g. a Region page opened earlier
    // this session, before the mod initialized): hook it directly rather
    // than waiting for a future load that may never come.
    HMODULE already = GetModuleHandleW(L"intl.cpl");
    if (already) {
        HMODULE pinned = LoadLibraryExW((g_system + L"\\intl.cpl").c_str(), nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
        if (pinned && HookNativeCpl(pinned)) {
            g_explorerCplHooked.store(true, std::memory_order_release);
            Wh_Log(L"intl.cpl already loaded in Explorer; CPlApplet hook installed");
            return true;
        }
        if (pinned) FreeLibrary(pinned);
    }
    // Resolve from kernelbase.dll, not the kernel32 import: internal loader
    // callers (including the ones that bring in intl.cpl) go straight to
    // kernelbase.
    HMODULE kernelBase = GetModuleHandleW(L"kernelbase.dll");
    if (!kernelBase) return false;
    auto real = reinterpret_cast<LoadLibraryExWProc>(
        reinterpret_cast<void*>(GetProcAddress(kernelBase, "LoadLibraryExW")));
    if (!real) return false;
    return WindhawkUtils::SetFunctionHook(real, LoadLibraryExW_hook, &g_origLoadLibraryExW);
}
bool Environment() {
    SYSTEM_INFO native{}; GetNativeSystemInfo(&native);
    if (native.wProcessorArchitecture != PROCESSOR_ARCHITECTURE_AMD64) return false;
    auto rtl = reinterpret_cast<RtlVersionProc>(reinterpret_cast<void*>(
        GetProcAddress(GetModuleHandleW(L"ntdll.dll"), "RtlGetVersion")));
    OSVERSIONINFOEXW version{}; version.dwOSVersionInfoSize = sizeof(version);
    if (!rtl || rtl(&version) < 0 || version.dwMajorVersion != 10 || version.dwMinorVersion != 0 ||
        version.dwBuildNumber < 10240 || version.wProductType != VER_NT_WORKSTATION) return false;
    g_build = version.dwBuildNumber;
    wchar_t system[MAX_PATH], windows[MAX_PATH], exe[MAX_PATH];
    UINT n = GetSystemDirectoryW(system, ARRAYSIZE(system)); if (!n || n >= ARRAYSIZE(system)) return false;
    n = GetWindowsDirectoryW(windows, ARRAYSIZE(windows)); if (!n || n >= ARRAYSIZE(windows)) return false;
    DWORD e = GetModuleFileNameW(nullptr, exe, ARRAYSIZE(exe)); if (!e || e >= ARRAYSIZE(exe)) return false;
    g_system = Slashes(system); g_windows = Slashes(windows);
    Host host = Host::Other;
    if (Equal(Slashes(exe), g_system + L"\\control.exe")) host = Host::Control;
    else if (Equal(Slashes(exe), g_system + L"\\rundll32.exe")) host = Host::Rundll32;
    else if (Equal(Slashes(exe), g_windows + L"\\explorer.exe")) host = Host::Explorer;
    g_host = host;
    if (host == Host::Other) return false;
    g_envViable = true; // arch + OS + known host proven; only the session filter remains
    int count = 0;
    LocalMem argv(CommandLineToArgvW(GetCommandLineW(), &count));
    if (!argv) return false;
    LPWSTR* raw = static_cast<LPWSTR*>(argv.value);
    std::vector<std::wstring> args;
    for (int i = 0; i < count; ++i) args.emplace_back(raw[i]);
    if (!SelectedLaunch(host, args, g_system)) {
        Wh_Log(L"Not a selected Region session (Data and Time/other CPL unchanged)");
        return false;
    }
    Wh_Log(L"Host detected: %s; Windows build=%lu AMD64", exe, g_build);
    Wh_Log(L"Requested applet: intl.cpl (Region / Area geografica)");
    return true;
}
} // namespace IntlRestore

BOOL Wh_ModInit() {
    using namespace IntlRestore;
    try {
        const bool redirectWanted = Wh_GetIntSetting(L"redirectSettings") != 0;
        g_redirectWanted.store(redirectWanted, std::memory_order_release);
        if (!Environment()) {
            return FALSE;
        }
        Wh_Log(L"Initializing Windows 7 private-provider restoration");
        g_idle = CreateEventW(nullptr, TRUE, TRUE, nullptr);
        g_jobsIdle = CreateEventW(nullptr, TRUE, TRUE, nullptr);
        g_prefetchDone = CreateEventW(nullptr, TRUE, FALSE, nullptr); // signalled: payloads verified
        // g_prefetchDone is load-bearing: EnsurePrepared() waits on it, so
        // without it the classic page could never appear. Treat its failure
        // like the other two.
        if (!g_idle || !g_jobsIdle || !g_prefetchDone) {
            if (g_idle) { CloseHandle(g_idle); g_idle = nullptr; }
            if (g_jobsIdle) { CloseHandle(g_jobsIdle); g_jobsIdle = nullptr; }
            if (g_prefetchDone) { CloseHandle(g_prefetchDone); g_prefetchDone = nullptr; }
            return FALSE;
        }

        // In Control and Rundll32 hosts, intl.cpl is directly loaded and hooked during init.
        // In Explorer, we do not force-load intl.cpl at startup: instead we hook the
        // loader so the in-process shell32!Control_RunDLLW path gets caught too (finding 1).
        if (g_host != Host::Explorer) {
            g_nativeModule = LoadLibraryExW((g_system + L"\\intl.cpl").c_str(), nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
            if (!g_nativeModule) throw std::runtime_error("native intl.cpl unavailable");
            if (!HookNativeCpl(g_nativeModule)) {
                FreeLibrary(g_nativeModule); g_nativeModule = nullptr;
                CloseHandle(g_idle); g_idle = nullptr;
                CloseHandle(g_jobsIdle); g_jobsIdle = nullptr;
                if (g_prefetchDone) { CloseHandle(g_prefetchDone); g_prefetchDone = nullptr; }
                return FALSE;
            }
            // No mapping and no DllMain here: the private Windows 7 provider is
            // prepared lazily by EnsurePrepared() on the first real
            // CPL_INIT/activation, so a session that never opens the Region page
            // pays none of that cost (finding 5).
            //
            // This host (control.exe/rundll32.exe launched for intl.cpl) exists
            // only to show the Region page, so the payload fetch is started now,
            // in the background, in parallel with the applet's own startup -
            // which is what keeps the activation thread off the network
            // (finding 5). Nothing is downloaded in explorer.exe at startup;
            // there the prefetch starts when intl.cpl is first loaded.
            StartPrefetch();
            Wh_Log(L"Region-only CPlApplet dispatch queued; private provider will prepare on first activation");
        } else {
            if (!InstallExplorerCplHook()) {
                Wh_Log(L"WARNING: could not install the Explorer intl.cpl loader hook; "
                       L"in-process Control Panel > Region will show the modern page");
            } else {
                Wh_Log(L"Explorer intl.cpl loader hook active; CPlApplet will be hooked once intl.cpl loads");
            }
        }

        if (redirectWanted && g_host == Host::Explorer) {
            if (!InstallRedirectHook())
                Wh_Log(L"WARNING: Settings-redirect hook failed; classic UI still active");
            else
                Wh_Log(L"Settings redirect active alongside classic UI");
        }
        return TRUE;
    } catch (...) {
        Wh_Log(L"Initialization exception; fallback to native intl.cpl");
        Cleanup();
        if (g_act != INVALID_HANDLE_VALUE) { ReleaseActCtx(g_act); g_act = INVALID_HANDLE_VALUE; }
        if (g_nativeModule) { FreeLibrary(g_nativeModule); g_nativeModule = nullptr; }
        if (g_idle) { CloseHandle(g_idle); g_idle = nullptr; }
        if (g_jobsIdle) { CloseHandle(g_jobsIdle); g_jobsIdle = nullptr; }
        if (g_prefetchDone) { CloseHandle(g_prefetchDone); g_prefetchDone = nullptr; }
        return FALSE;
    }
}
// Settings are applied at runtime; nothing here needs the mod torn down and
// reloaded. That is not just a convenience: a reload runs the whole unload
// rundown, so editing a setting while a Region dialog is open would make the
// host wait for that dialog.
BOOL Wh_ModSettingsChanged(BOOL* reload) {
    using namespace IntlRestore;
    *reload = FALSE;
    try {
        // redirectSettings: the hooks stay installed once they are up and check
        // the flag on entry. They are only installed on demand, so an Explorer
        // that never uses the feature carries no extra hooks; turning the
        // feature off just clears the flag, because unhooking here would be a
        // reload in all but name.
        const bool wanted = Wh_GetIntSetting(L"redirectSettings") != 0;
        const bool before = g_redirectWanted.exchange(wanted, std::memory_order_acq_rel);
        if (wanted && g_host == Host::Explorer && !g_origShellExecuteExW) {
            // Same rule as the loader hook above: registrations made after
            // Wh_ModInit are inert until they are applied, and the trampolines
            // stay null until then. Applying is also what makes the
            // !g_origShellExecuteExW guard above work - without it every later
            // settings change would re-register the same three targets.
            // On failure the trampolines stay null, so this branch is taken
            // again by the next settings edit - which is right, since a later
            // edit is also a later chance for the apply to succeed. It is not a
            // re-registration loop in the harmful sense: InstallRedirectHook()
            // guards on g_origShellExecuteExW, and the registration itself is
            // idempotent for the same target and hook.
            if (InstallRedirectHook() && Wh_ApplyHookOperations() && g_origShellExecuteExW)
                Wh_Log(L"Settings redirect installed at runtime");
            else
                Wh_Log(L"WARNING: Settings-redirect hook failed at runtime; classic UI still active");
        } else if (wanted != before) {
            Wh_Log(L"Settings redirect %s", wanted ? L"enabled" : L"disabled");
        }
        // language: queue the switch; ApplyPendingLanguageChange() rebuilds the
        // embedded tables at the next quiet point inside CplHook.
        const int lang = ResolveSelectedLanguage();
        if (lang != g_lang.load(std::memory_order_acquire)) {
            g_pendingLang.store(lang, std::memory_order_release);
            Wh_Log(L"UI language change queued: %s -> %s", kLangTags[g_lang.load(std::memory_order_acquire)],
                   kLangTags[lang]);
        }
    } catch (...) {
        Wh_Log(L"Exception while applying settings at runtime; keeping the current state");
    }
    return TRUE;
}
void Wh_ModBeforeUninit() {
    using namespace IntlRestore;
    try {
        AcquireSRWLockExclusive(&g_gate);
        g_stopping.store(true, std::memory_order_release);
        ReleaseSRWLockExclusive(&g_gate);

        // A first-use setup blocked in WinHTTP (connect/send/receive, or
        // between the two pinned payloads) must not sit out its timeouts
        // during unload (finding 3).
        CancelInFlightDownload();

        Wh_Log(L"Unloading: requesting normal close of private-provider dialogs");

        // No deadline here, deliberately: the mod image cannot be unmapped while
        // anything still points into it, and giving up would crash the host
        // instead of waiting (see "Unload rundown"). What this loop does instead
        // is remove the reasons the wait can be long - it asks every window it
        // may touch to close, and after kUnloadEscalateMs it force-closes the UI
        // this mod created itself (EndDialog through its own DLGPROC wrapper,
        // PSM_PRESSBUTTON/Cancel for its property sheets). What is left after
        // that is only a wait on the user: a legacy launch blocked behind an
        // out-of-process UAC consent prompt, which nothing in this process can
        // end and which resolves as soon as it is answered.
        const ULONGLONG start = GetTickCount64();
        ULONGLONG lastDiagnostic = start;
        const ULONGLONG DIAGNOSTIC_INTERVAL_MS = 5000;
        bool escalated = false;

        while (g_active.load(std::memory_order_acquire) != 0 ||
               g_jobs.load(std::memory_order_acquire) != 0) {
            const ULONGLONG now = GetTickCount64();
            if (!escalated && now - start >= kUnloadEscalateMs) {
                escalated = true;
                Wh_Log(L"Private-provider UI still open after %llu ms; force-closing the dialogs and sheets "
                       L"this mod created", now - start);
            }

            CloseOwnedWindows(escalated);

            if (now - lastDiagnostic > DIAGNOSTIC_INTERVAL_MS) {
                Wh_Log(L"Still waiting for private-provider dialogs/jobs to close (%llu ms elapsed, "
                       L"active=%ld jobs=%ld)", now - start, g_active.load(std::memory_order_acquire),
                       g_jobs.load(std::memory_order_acquire));
                lastDiagnostic = now;
            }

            HANDLE event = g_active.load() != 0 ? g_idle : g_jobsIdle;
            if (event) WaitForSingleObject(event, 100);
        }

        AcquireSRWLockExclusive(&g_gate);
        ReleaseSRWLockExclusive(&g_gate);
        
    } catch (...) {
        Wh_Log(L"Exception during pre-unload wait; continuing teardown");
    }
}
void Wh_ModUninit() {
    using namespace IntlRestore;
    try {
    Cleanup();
    // Cleanup() drained every legacy call and joined every tracked worker, so
    // nothing can still SetEvent these handles or run inside the mod image.
    if (g_act != INVALID_HANDLE_VALUE) { ReleaseActCtx(g_act); g_act = INVALID_HANDLE_VALUE; }
    if (g_nativeModule) { FreeLibrary(g_nativeModule); g_nativeModule = nullptr; }
    if (g_idle) { CloseHandle(g_idle); g_idle = nullptr; }
    if (g_jobsIdle) { CloseHandle(g_jobsIdle); g_jobsIdle = nullptr; }
    if (g_prefetchDone) { CloseHandle(g_prefetchDone); g_prefetchDone = nullptr; }
    Wh_Log(L"Unloaded. Native Region behavior restored for new calls; system files and registration unchanged.");
    } catch (...) {
        // Unload must never propagate; the host survives with native behavior.
    }
}
