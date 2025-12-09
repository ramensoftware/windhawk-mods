// ==WindhawkMod==
// @id              windbg-no-ole-flush-clipboard
// @name            WinDbg No OleFlushClipboard
// @description     Prevents the UI from getting stuck on Ctrl-C
// @version         0.1
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
One example is when running SPICE’s vdagent (Windows VM on Linux).

This mod patches OleFlushClipboard to do nothing in WinDbg's UI process.
(Will update with a full writeup once it's published.)
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
