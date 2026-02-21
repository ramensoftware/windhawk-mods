// ==WindhawkMod==
// @id              windbg-no-ole-flush-clipboard
// @name            WinDbg No OleFlushClipboard
// @description     Prevents the UI from getting stuck on Ctrl-C
// @version         0.2
// @author          Nikita
// @github          https://github.com/nikital
// @homepage        https://www.leshenko.net/
// @include         DbgX.Shell.exe
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# WinDbg No OleFlushClipboard
Prevents the UI from getting stuck on Ctrl-C…
This happens when a clipboard monitor tries to read the clipboard as soon as WinDbg copies something.

In the mod author's case, it happens when running a Windows VM on Linux. The
participants are WinDbg, WPF, and SPICE's vdagent, where:

- WinDbg calls WPF's Clipboard.SetDataObject() with copy = true. This call is
  done on WinDbg's UI thread.
- WPF puts delay-rendered content on the clipboard and repeatedly calls
  OleFlushClipboard(), on the aforementioned UI thread.
- Meanwhile, SPICE's vdagent tries to synchronize the clipboard to Linux, which
  triggers delayed rendering, but it can't complete since WinDbg UI's thread is
  blocked.
- WinDbg can't complete the clipboard flush since vdagent holds the clipboard
  open. This creates a deadlock.

This mod patches OleFlushClipboard to do nothing in WinDbg's UI process.

A full technical writeup is available at:
https://www.island.io/blog/debugging-windbg-with-windbg-fixing-a-ctrl-c-ui-freeze

*/
// ==/WindhawkModReadme==

using OleFlushClipboard_t = decltype(&OleFlushClipboard);
OleFlushClipboard_t OleFlushClipboard_Original;
HRESULT WINAPI OleFlushClipboard_Hook() {
    return 0;
}

BOOL Wh_ModInit() {
    HMODULE ole32 = LoadLibrary(L"ole32.dll");
    OleFlushClipboard_t OleFlushClipboardExport =
        (OleFlushClipboard_t)GetProcAddress(ole32, "OleFlushClipboard");

    Wh_SetFunctionHook((void*)OleFlushClipboardExport,
                       (void*)OleFlushClipboard_Hook,
                       (void**)&OleFlushClipboard_Original);

    return TRUE;
}
