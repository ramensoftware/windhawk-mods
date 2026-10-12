// ==WindhawkMod==
// @id              screenshot-path-to-clipboard
// @name            Screenshot Path to Clipboard
// @description     Monitors the Screenshots folder and copies the full path of each new .png screenshot to the clipboard the moment it appears.
// @version         1.0.0
// @author          Sondre Myrmel
// @github          https://github.com/Sondre234
// @include         windhawk.exe
// @compilerOptions -lshell32 -lole32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Screenshot Path to Clipboard

Watches the Windows Screenshots folder and copies the full path of every new
screenshot to the clipboard, so you can paste it straight into any app
without digging through File Explorer.

## Usage
- Take a screenshot (Win+PrintScreen, Snipping Tool, etc.).
- The full path to the new `.png` is copied to the clipboard automatically.
- Paste (Ctrl+V) anywhere the path is needed.

## Notes
- Runs as a dedicated background process (see the
  [mods-as-tools](https://github.com/ramensoftware/windhawk/wiki/Mods-as-tools:-Running-mods-in-a-dedicated-process)
  pattern) and watches the Screenshots known folder (`FOLDERID_Screenshots`)
  with `ReadDirectoryChangesW` — no polling.
- Only `.png` files trigger a clipboard update.
- No configuration is needed; the folder is resolved via the shell so it
  still works if the Screenshots folder has been relocated, e.g. by OneDrive.
*/
// ==/WindhawkModReadme==

#include <shlobj.h>
#include <windows.h>

#include <string>
#include <thread>

// {B7BEDE81-DF94-4682-A7D8-57A52620B86F} — FOLDERID_Screenshots. Defined
// inline rather than referencing the named constant from <knownfolders.h>:
// that header gates newer known-folder IDs behind NTDDI_VERSION checks that
// aren't reliably satisfied across toolchains, which can leave the name
// undeclared even though the folder itself exists on any Windows 10+ system.
constexpr GUID kFolderIdScreenshots = {
    0xb7bede81, 0xdf94, 0x4682, {0xa7, 0xd8, 0x57, 0xa5, 0x26, 0x20, 0xb8, 0x6f}};

// ---------------------------------------------------------------------------
// Global state shared between WhTool_ModInit / WhTool_ModUninit and the
// worker thread.
// ---------------------------------------------------------------------------

static HANDLE g_hStopEvent = nullptr;  // manual-reset event: set → thread exits

// Heap-allocated and intentionally leaked if the process is torn down
// abruptly (e.g. killed) without WhTool_ModUninit running: a plain
// std::thread global would call std::terminate() from its destructor if it
// is still joinable, aborting the process. WhTool_ModUninit performs the
// real join() + delete on the normal shutdown path.
static std::thread* g_monitorThread = nullptr;

// How long to wait before retrying after a transient failure (folder
// temporarily missing, or a watch error), so a momentary hiccup doesn't
// permanently stop monitoring.
constexpr DWORD kRetryDelayMs = 2000;

// ---------------------------------------------------------------------------
// CopyPathToClipboard
//
// Allocates a GMEM_MOVEABLE block containing the null-terminated wide string,
// then hands ownership to the clipboard via SetClipboardData.
// Per MSDN: after a successful SetClipboardData the system owns hMem — never
// free it yourself; on failure, free it to avoid a leak.
// ---------------------------------------------------------------------------
static void CopyPathToClipboard(const std::wstring& path)
{
    // Size in bytes including the null terminator
    const SIZE_T byteCount = (path.size() + 1) * sizeof(wchar_t);

    HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, byteCount);
    if (!hMem)
    {
        Wh_Log(L"GlobalAlloc failed (error %u)", GetLastError());
        return;
    }

    void* pMem = GlobalLock(hMem);
    if (!pMem)
    {
        GlobalFree(hMem);
        return;
    }
    memcpy(pMem, path.c_str(), byteCount);
    GlobalUnlock(hMem);

    // nullptr → no specific owner window; fine for programmatic clipboard use
    if (!OpenClipboard(nullptr))
    {
        Wh_Log(L"OpenClipboard failed (error %u)", GetLastError());
        GlobalFree(hMem);
        return;
    }

    EmptyClipboard();  // discard whatever was on the clipboard before

    // CF_UNICODETEXT expects a null-terminated WCHAR string in a global block
    if (!SetClipboardData(CF_UNICODETEXT, hMem))
    {
        Wh_Log(L"SetClipboardData failed (error %u)", GetLastError());
        GlobalFree(hMem);  // we still own it on failure
    }
    // On success the system owns hMem — do NOT free it

    CloseClipboard();
}

// ---------------------------------------------------------------------------
// MonitorThread
//
// Runs on a dedicated background thread. Uses ReadDirectoryChangesW with an
// OVERLAPPED structure so the wait can be interrupted by g_hStopEvent without
// polling. Handles both FILE_ACTION_ADDED (direct creation) and
// FILE_ACTION_RENAMED_NEW_NAME (temp-file-then-rename pattern) so every tool
// that produces screenshots is covered.
//
// The outer loop re-opens the directory and retries after a short backoff if
// the folder is momentarily unavailable or a watch error occurs, so a
// transient failure doesn't permanently stop monitoring.
// ---------------------------------------------------------------------------
static void MonitorThread(std::wstring folderPath)
{
    while (true)
    {
        // Open a directory handle — FILE_FLAG_BACKUP_SEMANTICS is mandatory
        // for directories, FILE_FLAG_OVERLAPPED enables async I/O.
        HANDLE hDir = CreateFileW(
            folderPath.c_str(),
            FILE_LIST_DIRECTORY,
            FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
            nullptr,
            OPEN_EXISTING,
            FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED,
            nullptr);

        if (hDir == INVALID_HANDLE_VALUE)
        {
            Wh_Log(L"Cannot open Screenshots folder (error %u): %s, retrying",
                   GetLastError(), folderPath.c_str());
            if (WaitForSingleObject(g_hStopEvent, kRetryDelayMs) != WAIT_TIMEOUT)
            {
                break;  // stop event signalled during backoff
            }
            continue;
        }

        // Notification buffer — sized to absorb a burst of simultaneous
        // events. Must be DWORD-aligned; the alignas ensures that even on
        // stack allocations.
        alignas(DWORD) BYTE buffer[8192];

        // The event in the OVERLAPPED struct is signalled when a change is
        // ready.
        OVERLAPPED ov = {};
        ov.hEvent = CreateEventW(nullptr, /*manualReset=*/TRUE, /*initial=*/FALSE, nullptr);
        if (!ov.hEvent)
        {
            CloseHandle(hDir);
            break;
        }

        // We wait on two handles:
        //   [0] ov.hEvent    – ReadDirectoryChangesW result ready
        //   [1] g_hStopEvent – WhTool_ModUninit told us to quit
        HANDLE waitHandles[2] = { ov.hEvent, g_hStopEvent };

        Wh_Log(L"Screenshot monitor started: %s", folderPath.c_str());

        bool stopRequested = false;
        bool transientError = false;

        while (true)
        {
            ResetEvent(ov.hEvent);

            // Issue an async watch request. FILE_NOTIFY_CHANGE_FILE_NAME
            // fires on create, delete, and rename — exactly what we need.
            // bWatchSubtree = FALSE: we only care about the top-level folder.
            DWORD bytesReturned = 0;
            BOOL queued = ReadDirectoryChangesW(
                hDir,
                buffer,
                sizeof(buffer),
                /*bWatchSubtree=*/FALSE,
                FILE_NOTIFY_CHANGE_FILE_NAME,
                &bytesReturned,
                &ov,
                /*lpCompletionRoutine=*/nullptr);

            if (!queued && GetLastError() != ERROR_IO_PENDING)
            {
                Wh_Log(L"ReadDirectoryChangesW failed (error %u)", GetLastError());
                transientError = true;
                break;
            }

            // Block until either a change arrives or we are asked to stop.
            DWORD waitResult = WaitForMultipleObjects(2, waitHandles, FALSE, INFINITE);

            if (waitResult == WAIT_OBJECT_0 + 1)
            {
                stopRequested = true;
                break;
            }

            if (waitResult != WAIT_OBJECT_0)
            {
                Wh_Log(L"WaitForMultipleObjects returned %u (error %u)",
                       waitResult, GetLastError());
                transientError = true;
                break;
            }

            // Retrieve the number of bytes actually written to the buffer.
            if (!GetOverlappedResult(hDir, &ov, &bytesReturned, /*bWait=*/FALSE))
            {
                Wh_Log(L"GetOverlappedResult failed (error %u)", GetLastError());
                transientError = true;
                break;
            }

            if (bytesReturned == 0)
            {
                // Buffer overflow: too many changes arrived at once; records
                // lost. Simply loop back and continue watching.
                Wh_Log(L"Change buffer overflow — some events may have been lost");
                continue;
            }

            // Walk the singly-linked list of FILE_NOTIFY_INFORMATION records.
            const BYTE* p = buffer;
            for (;;)
            {
                const auto* info = reinterpret_cast<const FILE_NOTIFY_INFORMATION*>(p);

                // Catch both direct creation and the rename-into-place
                // pattern.
                if (info->Action == FILE_ACTION_ADDED ||
                    info->Action == FILE_ACTION_RENAMED_NEW_NAME)
                {
                    // FileName is NOT null-terminated; use FileNameLength
                    // (bytes).
                    std::wstring fileName(
                        info->FileName,
                        info->FileNameLength / sizeof(wchar_t));

                    // Filter: only process files ending in .png
                    // (case-insensitive).
                    if (fileName.size() > 4)
                    {
                        std::wstring ext = fileName.substr(fileName.size() - 4);
                        for (wchar_t& c : ext) c = towlower(c);

                        if (ext == L".png")
                        {
                            std::wstring fullPath = folderPath + L'\\' + fileName;
                            Wh_Log(L"New screenshot → clipboard: %s", fullPath.c_str());
                            CopyPathToClipboard(fullPath);
                        }
                    }
                }

                if (info->NextEntryOffset == 0) break;  // last record in batch
                p += info->NextEntryOffset;
            }
        }

        // Whether we're stopping cleanly or bailing out after a transient
        // error, there may still be an async ReadDirectoryChangesW in
        // flight against `buffer` — cancel and drain it before the buffer
        // goes out of scope and the handle is closed.
        CancelIo(hDir);
        DWORD dummy;
        GetOverlappedResult(hDir, &ov, &dummy, /*bWait=*/TRUE);

        CloseHandle(ov.hEvent);
        CloseHandle(hDir);

        if (stopRequested)
        {
            break;
        }

        if (transientError)
        {
            Wh_Log(L"Retrying after transient error");
            if (WaitForSingleObject(g_hStopEvent, kRetryDelayMs) != WAIT_TIMEOUT)
            {
                break;  // stop event signalled during backoff
            }
        }
    }

    Wh_Log(L"Screenshot monitor stopped.");
}

// ---------------------------------------------------------------------------
// WhTool_ModInit — called by Windhawk when the dedicated tool-mod process
// starts.
// ---------------------------------------------------------------------------
BOOL WhTool_ModInit()
{
    // Resolve the Screenshots known folder via the shell rather than
    // hardcoding %USERPROFILE%\Pictures\Screenshots: that location is
    // frequently relocated, most commonly by OneDrive redirecting Pictures
    // (and Screenshots) under %USERPROFILE%\OneDrive\Pictures\Screenshots,
    // but a user can also move it manually. The known-folder path always
    // points at wherever it currently lives, and it's guaranteed to already
    // exist, so there's no need to create it ourselves.
    PWSTR path = nullptr;
    HRESULT hr = SHGetKnownFolderPath(kFolderIdScreenshots, 0, nullptr, &path);
    if (FAILED(hr))
    {
        Wh_Log(L"SHGetKnownFolderPath failed (hr %08x)", hr);
        return FALSE;
    }

    std::wstring folderPath = path;
    CoTaskMemFree(path);

    // Manual-reset event: set once to stop the thread; never auto-reset.
    g_hStopEvent = CreateEventW(nullptr, /*manualReset=*/TRUE, /*initial=*/FALSE, nullptr);
    if (!g_hStopEvent)
    {
        Wh_Log(L"CreateEvent failed (error %u)", GetLastError());
        return FALSE;
    }

    // Start the background monitoring thread.
    g_monitorThread = new std::thread(MonitorThread, folderPath);

    Wh_Log(L"Mod initialised — watching %s", folderPath.c_str());
    return TRUE;
}

void WhTool_ModSettingsChanged()
{
    // No settings.
}

// ---------------------------------------------------------------------------
// WhTool_ModUninit — called by Windhawk just before the tool-mod process
// exits.
// ---------------------------------------------------------------------------
void WhTool_ModUninit()
{
    if (g_hStopEvent)
    {
        // Signal the thread; it will cancel its I/O and exit the loop.
        SetEvent(g_hStopEvent);
    }

    if (g_monitorThread)
    {
        g_monitorThread->join();  // wait for the thread to finish cleanly
        delete g_monitorThread;
        g_monitorThread = nullptr;
    }

    if (g_hStopEvent)
    {
        CloseHandle(g_hStopEvent);
        g_hStopEvent = nullptr;
    }

    Wh_Log(L"Mod unloaded.");
}

////////////////////////////////////////////////////////////////////////////////
// Windhawk tool mod implementation for mods which don't need to inject to other
// processes or hook other functions. Context:
// https://github.com/ramensoftware/windhawk/wiki/Mods-as-tools:-Running-mods-in-a-dedicated-process
//
// The mod will load and run in a dedicated windhawk.exe process.
//
// Paste the code below as part of the mod code, and use these callbacks:
// * WhTool_ModInit
// * WhTool_ModSettingsChanged
// * WhTool_ModUninit
//
// Currently, other callbacks are not supported.

bool g_isToolModProcessLauncher;
HANDLE g_toolModProcessMutex;

void WINAPI EntryPoint_Hook() {
    Wh_Log(L">");
    ExitThread(0);
}

BOOL Wh_ModInit() {
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

    WCHAR
    commandLine[MAX_PATH + 2 +
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
