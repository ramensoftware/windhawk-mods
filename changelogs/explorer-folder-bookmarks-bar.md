## 0.8.35 ([Oct 11, 2026](https://github.com/ramensoftware/windhawk-mods/blob/c6bcbda4c69d98e2ad36b0ae913b6228579b3c04/mods/explorer-folder-bookmarks-bar.wh.cpp))

* New: **Ctrl+B** shortcut (off by default) bookmarks the current folder or removes its bookmark. With folders selected in the file list, it bookmarks them all, or removes them if all are already bookmarked.
* New: drag folders onto the bar to bookmark them where you drop them, up to 20 at a time.
* New: right-click a bookmark to open it in a new window.
* New: a short notice when the bar is full or a folder is already bookmarked.
* New: optional recent folders (off by default): buttons at the right end of the bar and an **RC** menu with the other folders you opened in File Explorer. The list stays in File Explorer's memory only and starts empty after File Explorer restarts.
* New: optional parent labels for bookmarks with the same folder name, for example `projA\src` and `projB\src`.
* Each Windows account has its own bookmarks. After the update, the existing list moves to the first account that opens File Explorer.
* Settings, bookmark and recent-folder changes now apply to all open windows right away.
* Hovering a bookmark refreshes its icon and missing-folder mark without redrawing the bar.
* Desktop, Documents and Downloads in the FX menu are hidden only when missing from a local disk or when their drive is unplugged.
* Fixed: a new window could stall when Desktop, Documents or Downloads pointed to an unreachable network share.
* Fixed: backups saved with a UTF-8 byte order mark now load, and a backup path with a hidden NUL character is rejected.
* Fixed: paths that differ only in the case of non-Latin letters count as the same folder.
* Errors in the bar's click, menu and drag handlers are caught and logged.
* Opening a missing bookmark in a new tab or window does nothing, like a plain click, instead of showing Windows' error box.
* Disabling or updating the mod waits for a bar click that is still being handled, or for a new window's bar that is still being built.

## 0.7.8 ([Sep 29, 2026](https://github.com/ramensoftware/windhawk-mods/blob/0c9eded5a005f4645d1e834b11a5fd94fc3159c2/mods/explorer-folder-bookmarks-bar.wh.cpp))

Initial release.
