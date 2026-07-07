// ==WindhawkMod==
// @id              screenshot-path-to-clipboard
// @name            Screenshot Path to Clipboard
// @description     Monitors %USERPROFILE%\Pictures\Screenshots and copies the full path of each new .png screenshot to the clipboard the moment it appears.
// @version         1.0.0
// @author          Sondre234
// @github          https://github.com/Sondre234
// @include         %SystemRoot%\\explorer.exe
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Screenshot Path to Clipboard

Watches the default Windows Screenshots folder and copies the full path of
every new screenshot to the clipboard, so you can paste it straight into any
app without digging through File Explorer.

## Usage
- Take a screenshot (Win+PrintScreen, Snipping Tool, Xbox Game Bar, etc.).
- The full path to the new `.png` is copied to the clipboard automatically.
- Paste (Ctrl+V) anywhere the path is needed.

## Notes
- Runs inside `explorer.exe` and watches `%USERPROFILE%\Pictures\Screenshots`
  with `ReadDirectoryChangesW` — no polling.
- Creates the Screenshots folder if it doesn't exist yet.
- Only `.png` files trigger a clipboard update.
- No configuration is needed; the folder path is fixed to the standard location.
*/
// ==/WindhawkModReadme==

#include <windows.h>
#include <string>
#include <thread>

// ---------------------------------------------------------------------------
// Global state shared between Wh_ModInit / Wh_ModUninit and the worker thread
// ---------------------------------------------------------------------------

static HANDLE      g_hStopEvent   = nullptr;  // manual-reset event: set → thread exits
static std::thread g_monitorThread;

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
// Runs on a dedicated background thread.  Uses ReadDirectoryChangesW with an
// OVERLAPPED structure so the wait can be interrupted by g_hStopEvent without
// polling.  Handles both FILE_ACTION_ADDED (direct creation) and
// FILE_ACTION_RENAMED_NEW_NAME (temp-file-then-rename pattern) so every tool
// that produces screenshots is covered.
// ---------------------------------------------------------------------------
static void MonitorThread(std::wstring folderPath)
{
    // Open a directory handle — FILE_FLAG_BACKUP_SEMANTICS is mandatory for
    // directories, FILE_FLAG_OVERLAPPED enables async I/O.
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
        Wh_Log(L"Cannot open Screenshots folder (error %u): %s",
               GetLastError(), folderPath.c_str());
        return;
    }

    // Notification buffer — sized to absorb a burst of simultaneous events.
    // Must be DWORD-aligned; the alignas ensures that even on stack allocations.
    alignas(DWORD) BYTE buffer[8192];

    // The event in the OVERLAPPED struct is signalled when a change is ready.
    OVERLAPPED ov  = {};
    ov.hEvent      = CreateEventW(nullptr, /*manualReset=*/TRUE, /*initial=*/FALSE, nullptr);
    if (!ov.hEvent)
    {
        CloseHandle(hDir);
        return;
    }

    // We wait on two handles:
    //   [0] ov.hEvent    – ReadDirectoryChangesW result ready
    //   [1] g_hStopEvent – Wh_ModUninit told us to quit
    HANDLE waitHandles[2] = { ov.hEvent, g_hStopEvent };

    Wh_Log(L"Screenshot monitor started: %s", folderPath.c_str());

    while (true)
    {
        ResetEvent(ov.hEvent);

        // Issue an async watch request.  FILE_NOTIFY_CHANGE_FILE_NAME fires on
        // create, delete, and rename — exactly what we need.
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
            break;
        }

        // Block until either a change arrives or we are asked to stop.
        DWORD waitResult = WaitForMultipleObjects(2, waitHandles, FALSE, INFINITE);

        if (waitResult == WAIT_OBJECT_0 + 1)
        {
            // Stop event: cancel the pending I/O, drain it, then exit.
            CancelIo(hDir);
            DWORD dummy;
            GetOverlappedResult(hDir, &ov, &dummy, /*bWait=*/TRUE);
            break;
        }

        if (waitResult != WAIT_OBJECT_0)
        {
            Wh_Log(L"WaitForMultipleObjects returned %u (error %u)",
                   waitResult, GetLastError());
            break;
        }

        // Retrieve the number of bytes actually written to the buffer.
        if (!GetOverlappedResult(hDir, &ov, &bytesReturned, /*bWait=*/FALSE))
        {
            Wh_Log(L"GetOverlappedResult failed (error %u)", GetLastError());
            break;
        }

        if (bytesReturned == 0)
        {
            // Buffer overflow: too many changes arrived at once; records lost.
            // Simply loop back and continue watching.
            Wh_Log(L"Change buffer overflow — some events may have been lost");
            continue;
        }

        // Walk the singly-linked list of FILE_NOTIFY_INFORMATION records.
        const BYTE* p = buffer;
        for (;;)
        {
            const auto* info = reinterpret_cast<const FILE_NOTIFY_INFORMATION*>(p);

            // Catch both direct creation and the rename-into-place pattern.
            if (info->Action == FILE_ACTION_ADDED ||
                info->Action == FILE_ACTION_RENAMED_NEW_NAME)
            {
                // FileName is NOT null-terminated; use FileNameLength (bytes).
                std::wstring fileName(
                    info->FileName,
                    info->FileNameLength / sizeof(wchar_t));

                // Filter: only process files ending in .png (case-insensitive).
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

            if (info->NextEntryOffset == 0) break;     // last record in batch
            p += info->NextEntryOffset;
        }
    }

    CloseHandle(ov.hEvent);
    CloseHandle(hDir);
    Wh_Log(L"Screenshot monitor stopped.");
}

// ---------------------------------------------------------------------------
// Wh_ModInit — called by Windhawk when the mod is injected into explorer.exe
// ---------------------------------------------------------------------------
BOOL Wh_ModInit()
{
    // Resolve %USERPROFILE%\Pictures\Screenshots at runtime so the path is
    // always correct regardless of which user account is running explorer.
    wchar_t userProfile[MAX_PATH] = {};
    if (!GetEnvironmentVariableW(L"USERPROFILE", userProfile, MAX_PATH))
    {
        Wh_Log(L"Cannot read USERPROFILE (error %u)", GetLastError());
        return FALSE;
    }

    std::wstring folderPath =
        std::wstring(userProfile) + L"\\Pictures\\Screenshots";

    // Create the folder if it has never been used — CreateDirectoryW is a
    // no-op when the directory already exists (returns FALSE / ERROR_ALREADY_EXISTS).
    CreateDirectoryW(folderPath.c_str(), nullptr);

    // Manual-reset event: set once to stop the thread; never auto-reset.
    g_hStopEvent = CreateEventW(nullptr, /*manualReset=*/TRUE, /*initial=*/FALSE, nullptr);
    if (!g_hStopEvent)
    {
        Wh_Log(L"CreateEvent failed (error %u)", GetLastError());
        return FALSE;
    }

    // Start the background monitoring thread.
    g_monitorThread = std::thread(MonitorThread, folderPath);

    Wh_Log(L"Mod initialised — watching %s", folderPath.c_str());
    return TRUE;
}

// ---------------------------------------------------------------------------
// Wh_ModUninit — called by Windhawk just before unloading the mod
// ---------------------------------------------------------------------------
void Wh_ModUninit()
{
    if (g_hStopEvent)
    {
        // Signal the thread; it will cancel its I/O and exit the loop.
        SetEvent(g_hStopEvent);
    }

    if (g_monitorThread.joinable())
    {
        g_monitorThread.join();   // wait for the thread to finish cleanly
    }

    if (g_hStopEvent)
    {
        CloseHandle(g_hStopEvent);
        g_hStopEvent = nullptr;
    }

    Wh_Log(L"Mod unloaded.");
}
