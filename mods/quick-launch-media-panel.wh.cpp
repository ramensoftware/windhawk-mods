Submission review
Note: This review was done by Claude. Due to the amount of submissions, doing a fully manual review for each pull request is no longer feasible. Thank you for understanding.

Remember: The AI reviewer can be wrong - it may misread code, flag correct code as broken, or suggest changes that make things worse. Treat its findings as suggestions to verify, not instructions to follow blindly. You're responsible for the code you submit, so if a finding doesn't hold up, say so instead of changing working code to satisfy it.

Please address the following issues. The items in the collapsed sections are optional, so it's your call whether to address them.

Nice work overall — the tool-mod structure is correct, the settings block matches the code one-to-one (every key is read, every read key is declared, and $options is only used on string settings), teardown joins both threads and unregisters the window classes, and the README has a screenshot. The items below are mostly about the recurring background cost and a few lifetime/side-effect details.

1. The panel does a full window sweep and a full re-render once per second, forever.

MediaThreadProc posts WM_APP_MEDIA every 1000 ms unconditionally, and the handler runs:

case WM_APP_MEDIA: {
    RefreshRunningWindows();
    if(g_settings.autoThemeSource==AutoThemeSource::Media)ApplyAutomaticTheme();
    Render();return 0;
}
RefreshRunningWindows() is an EnumWindows over every top-level window on the desktop, with an OpenProcess + QueryFullProcessImageNameW (into a 64 KB stack-adjacent buffer) for each one, and Render() redraws the whole panel with GDI+ and calls UpdateLayeredWindow — even when nothing changed. Since the panel is owned by the desktop host and sits at the bottom of the Z-order, it is covered by other windows most of the time, so nearly all of this work is invisible to the user. This is the most common performance objection on submissions (constant polling / process-wide enumeration).

Three cheap fixes, ideally all three:

Only Render() when the drawn state actually changed — keep the last MediaState you rendered and compare title/artist/playing/position-bucket/artwork before redrawing.
Don't re-enumerate every second. Running-window detection changes on a much slower scale; run it on a longer interval, or drive it from SetWinEventHook(EVENT_SYSTEM_FOREGROUND, ...) / window create-destroy events instead of polling.
Gate the whole loop on the desktop actually being visible. explorer-folder-hover-menu.wh.cpp does exactly this — SyncActiveState() plus an EVENT_SYSTEM_FOREGROUND hook — so its work only runs while the shell/desktop is in front.
2. winrt::uninit_apartment() runs while the WinRT session manager is still alive.

DWORD WINAPI MediaThreadProc(void*) {
    winrt::init_apartment(winrt::apartment_type::multi_threaded);
    MediaManager manager{nullptr};
    ...
    winrt::uninit_apartment();
    return 0;   // <-- ~manager() runs HERE, after CoUninitialize
}
manager has function scope, so its destructor (an IUnknown::Release) runs after uninit_apartment() has already called CoUninitialize. Releasing a WinRT/COM proxy on an uninitialized apartment is undefined — it can crash or leak the object, and it happens on every unload. Fix:

    manager = nullptr;
    winrt::uninit_apartment();
    return 0;
(or wrap everything from MediaManager manager{nullptr}; to the end of the loop in a nested scope).

3. FindDesktopHost() sends the undocumented Progman 0x052C message, which has a side effect that outlives the mod.

SendMessageTimeoutW(progman, 0x052C, 0, 0, SMTO_ABORTIFHUNG | SMTO_BLOCK, 1000, &ignored);
That message makes Explorer split the desktop into an extra WorkerW layer. The change is process-wide, sticks around after the mod is disabled (until Explorer restarts), and is known to interfere with wallpaper tools and slideshow transitions — which runs against Windhawk's "a mod's effects disappear when it's disabled" principle.

As far as I can tell the mod doesn't need it: the panel only uses the result as an owner window, and without 0x052C your EnumWindows finds Progman itself (it hosts SHELLDLL_DefView), which works identically as an owner — the panel still ends up above the desktop icons and below normal windows. If that's right, just drop the SendMessageTimeoutW call. If the WorkerW really is required for something, please say why in a comment.

4. Please state how this differs from the existing media mods.

The media half of the panel overlaps several merged mods — Dynamic Island for Windows (also a windhawk.exe tool mod drawing a desktop overlay driven by GlobalSystemMediaTransportControls), Island Media Controls, Taskbar Fluent Media Player and Taskbar Music Lounge. The launcher half looks genuinely new, so this probably isn't a duplicate — but the maintainer routinely asks this, so it's worth answering up front in the PR description (and in the README): what this does that those don't, and why the media strip belongs in the same mod rather than being left to one of them.

5. Automatic/wallpaper theming decodes the whole wallpaper on every WM_SETTINGCHANGE.

ApplyAutomaticTheme() does Bitmap wallpaper(path); accent = AverageBitmap(wallpaper, accent); — a full image decode of a possibly-4K JPEG — and it's reachable from WM_SETTINGCHANGE, which Windows broadcasts for all sorts of unrelated changes. Worse, both WindowProc and SinkWindowProc handle WM_SETTINGCHANGE, so each broadcast decodes it twice on the UI thread. Cache the computed accent keyed on the wallpaper path plus its last-write time (you already do this for artwork via g_lastArtworkHash), and only recompute when the wallpaper actually changed.

Optional improvements

Minor polish — none of this affects users, so it's your call.

Use WindhawkUtils::StringSetting. GetStringSetting() re-implements it, and the value ? value : L"" guard is dead code — Wh_GetStringSetting never returns NULL, it returns L"" on error or when unset. WindhawkUtils::StringSetting::make(L"theme") gives you the same thing with RAII.
french defaults to French on an empty value. next.french = _wcsicmp(GetStringSetting(L"language").c_str(), L"en-US") != 0; — anything that isn't exactly en-US, including an empty string, selects French. Invert it (== L"fr") so English stays the fallback.
Keep the tool-mod boilerplate byte-for-byte identical to the wiki snippet. The only difference is in Wh_ModAfterInit; the wiki uses designated initializers:
    STARTUPINFO si{
        .cb = sizeof(STARTUPINFO),
        .dwFlags = STARTF_FORCEOFFFEEDBACK,
    };
    PROCESS_INFORMATION pi;
Functionally identical, but keeping it unmodified makes it obvious at a glance that the launcher is the stock one.
Nested modal loops let the 1 s desktop timer run underneath them. While PromptText, ChooseFile, ChooseFolder, ChooseShortcutColor or Import/ExportLayout are up, their message loops dispatch the sink's WM_TIMER → EnsurePanelWindow(). If Explorer restarts at that moment, DestroyPanelWindow() destroys g_hwnd while its WindowProc is still on the stack, and the code that resumes afterwards (EnableWindow(g_hwnd, TRUE), SaveShortcut, Render) operates on a different window than the one the user interacted with. Setting a g_modalActive flag that the timer handler checks (or KillTimer/SetTimer around modal calls) avoids it.
LoadLibraryW(L"shcore.dll") in DpiForMonitor uses the default search order. shcore.dll isn't a KnownDLL, so this is technically a DLL-planting vector. The scope here is windhawk.exe in a protected directory, so it's just hardening, but LoadLibraryExW(L"shcore.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32) costs nothing. You're also already calling GetDpiForWindow unconditionally (Win10 1607+), so you could equally just add -lshcore and call GetDpiForMonitor directly.
g_iconBitmaps has a destructor that calls into GDI+ at process exit. std::array<std::unique_ptr<Gdiplus::Bitmap>, kMaxShortcuts> destroys its elements via GdipDisposeImage under the loader lock at process shutdown, potentially after GdiplusShutdown or after the owning UI thread was terminated. In the normal path DestroyIcons() has already emptied it, but not if the UI thread hit the 5 s ExitProcess(1) path or the process was killed. Since every element is a nullable smart pointer and DestroyIcons() already fully releases them, the bare attribute is enough here: [[clang::no_destroy]] std::array<std::unique_ptr<Bitmap>, kMaxShortcuts> g_iconBitmaps;. Background and the case-by-case rules: https://github.com/ramensoftware/windhawk/wiki/Global-objects-and-process-shutdown (see §7 RAII wrappers and §8 smart pointers). The other globals here (std::wstring, std::vector, std::mutex, raw HANDLEs) are heap-only or no-op and need nothing.
g_hwnd is read from the media thread without synchronization. HWND panel = g_hwnd; in MediaThreadProc races with the UI thread's writes. It's benign in practice (PostMessage on a stale handle just fails), but std::atomic<HWND> makes it well-defined.
Dead code. Nothing ever posts WM_APP_RELOAD to the panel window, so WindowProc's case WM_APP_RELOAD: block is unreachable (the sink handles it). SinkWindowProc's Render() on WM_DISPLAYCHANGE/WM_SETTINGCHANGE also duplicates what WindowProc already does for the same broadcasts.
Missing includes. <cstdint> (uint8_t, int64_t) and <cstring> (memcpy, memset) are used but only reach you transitively.
Formatting. Large stretches (e.g. MonitorStorageSuffix, PanelDropTarget, AverageBitmap, ShowTileContextMenu) are collapsed onto single lines with no spaces around operators, while the rest of the file is normally formatted. The repo ships a .clang-format (Chromium, 4-space indent) — running it over the file would make the mod much easier to review and maintain.
Functionality notes

Non-critical observations and ideas about the feature behavior itself.

Seeking only works on the right-hand third of the progress bar. WM_LBUTTONDOWN decides whether to start a window drag with
bool onMediaControl = p.y >= mt+6 && p.y <= mt+72 && p.x >= cx-58 && p.x <= cx+66;
where cx = g_width - 102. The timeline is drawn across x ∈ [32, g_width-32], so a click on its left portion sets g_dragging = true; WM_LBUTTONUP then returns from the if (g_dragging) branch and the MediaSeek(...) line below is never reached — the panel moves instead of seeking. Add the timeline strip (p.y >= mt+59 && p.y <= mt+72 && p.x >= 32 && p.x <= g_width-32) to the "don't start a drag" test.
Popup menus may not dismiss on an outside click. The panel is WS_EX_NOACTIVATE, so the tool process never becomes foreground; TrackPopupMenu with a non-foreground owner is the classic case where the menu stays on screen after the user clicks elsewhere. Worth testing all three menus (tile, media-session, panel). The usual SetForegroundWindow(hwnd) before / PostMessage(hwnd, WM_NULL, 0, 0) after workaround conflicts with WS_EX_NOACTIVATE, so you may need a hidden activatable helper window as the menu owner.
Verify drag-and-drop when Windhawk and Explorer run at different integrity levels. OLE drag-and-drop is driven by messages from the source thread, and UIPI blocks a lower-integrity source from reaching a higher-integrity target — if the tool process ends up above Explorer, dropping a file on a tile will silently do nothing. explorer-folder-hover-menu.wh.cpp hits the same class of problem and works around it with ChangeWindowMessageFilterEx. Since drag-and-drop is a headline feature here, it's worth confirming it works in that configuration (and allowing WM_DROPFILES, WM_COPYDATA and 0x0049 on the panel window if it doesn't).
Releasing the button over a tile launches it even if the press started elsewhere. WM_LBUTTONDOWN only captures when it hits a tile; with lockPosition on, a press on empty space captures nothing, so a WM_LBUTTONUP that lands on a tile falls through to if (tile >= 0) { LaunchSlot(tile); ... }. Probably not what the user intended.
Running-app detection matches on the bare file name. RefreshRunningWindowsProc accepts _wcsicmp(PathFindFileNameW(processPath), PathFindFileNameW(target)), so an unrelated process with the same exe name (there are a lot of launcher.exe / client.exe around) marks the tile as running, and clicking it focuses that window instead of launching. Consider matching on the full path only, and keeping the file-name fallback just for .lnk targets whose resolved path differs.
Icon extraction blocks the UI thread. LoadAssignments() calls SHGetFileInfoW(..., SHGFI_ICON | SHGFI_LARGEICON) for all 8 slots synchronously. For a target on a disconnected network share or a removed drive, the shell can take seconds to time out, freezing the panel (and, since LoadAssignments() also runs from PanelDropTarget::Drop, the drag source too). Extracting icons on a worker thread and posting the results back would keep the panel responsive.
README wording on URL shortcuts. "Drag an .exe, .lnk, document, folder, or URL shortcut onto a tile" — dragging a link out of a browser delivers CFSTR_INETURL/CF_UNICODETEXT, not CF_HDROP, so only .url/.website files actually work today. Either handle the text formats in PanelDropTarget::Drop or reword.
