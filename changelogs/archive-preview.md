## 1.1.0 ([Nov 19, 2026](https://github.com/ramensoftware/windhawk-mods/blob/973ad733fe13aa51435894e6801ecef9a77bc24e/mods/archive-preview.wh.cpp))

### Extraction and drag and drop

- Added optional extraction with **7-Zip, NanaZip, or WinRAR**. Windows remains the default. The mod can find an installed program automatically, or you can set its executable path.
- The selected program handles full archives and selected files. External extraction keeps the paths stored in the archive and uses the program's own progress, password, and overwrite dialogs where supported.
- **Extract to…** and **Extract selected to…** now start in the folder that contains the archive.
- Added separate shortcuts for all four extraction actions. **Ctrl + Shift + E** still runs **Extract selected here** while the file list has focus.
- Added optional external extraction when dragging files out of the preview. The mod stages the files in its own storage before Explorer copies them. It keeps the folder while the transfer uses it and retries cleanup when the broker starts again if it can confirm the folder is free. Clipboard copy and paste still use Windows.
- Added support for long paths when opening and cleaning up temporary files used by external drag and drop.
- Improved native extraction status and logging. Windows can still show **0%** until extraction finishes for some formats. External programs show their own progress.

### Sorting, columns, and appearance

- Click a column header to sort and click again to reverse the order. The list starts sorted by name, keeps folders first, and sorts dates and sizes by their values.
- Drag the header separators to resize columns. Right-click the headers to show or hide **Date modified**, **Type**, and **Size**. The mod saves these choices and gives the **Name** column priority in narrow panes.
- Added an option to hide the horizontal scrollbar. The preview temporarily shows fewer columns when there is too little space.
- Added separate options for reading dates and sizes in archives of **256 MiB or more**. Both are off by default for large archives. ZIP folder dates can use dates from the files they contain when the archive has no date for the folder.
- Limited the ZIP date fallback to direct subfolders of the current folder. It caps stored dates and stops when the preview is canceled.
- Leave the **Date modified** and **Size** cells blank when metadata is unavailable.
- Improved selection highlighting with the Windows accent color and made the main and recovery views more consistent in dark mode.

### Preview fixes

- Fixed icons disappearing after switching away from an archive and selecting it again.
- Fixed cases where another preview could cover the archive pane and block clicks, folder navigation, and extraction buttons. This includes the reported case when switching from an FBX file to an archive.
- Fixed preview sessions taking over another preview pane and improved checks before reusing cached data from an archive that may have changed.
- The mod can rebuild a broken file list from data it already loaded and restore the saved selection and scroll position when available. If that fails, it can switch to the Windows recovery view.

### Language, settings, and updates

- Added a preview language setting with **Automatic** and **105 language options and variants**. Automatic follows the Windows display language when available. Some detailed messages use English.
- Reorganized Windhawk settings, with **Files and security** and **Extraction** first. If you are updating from a version with ungrouped settings, copy your custom values before updating so you can reapply them.
- Added **Updates and disabling**. If you enable it, Explorer closes windows that still contain an Archive Preview when the mod is updated, recompiled, or disabled. This closes all tabs in each affected window. The option is off by default.
- Fixed broker shutdown when updating or disabling the mod during an external drag or while the broker is still starting. Pending requests and results are cleared before the broker's window closes.
- Added **Diagnostics > Detailed logs** to help investigate preview, extraction, and drag issues. It is off by default. Updated the mod README with setup, settings, and known limitations.

### Thanks

Big thanks to [Dizse](https://github.com/Dizse) for [his pull request](https://github.com/luizbizzio/archive-preview/pull/2). His work on preview stability, columns, localization, and external extraction was a big part of getting 1.1.0 ready.

## 1.0.0 ([Nov 13, 2026](https://github.com/ramensoftware/windhawk-mods/blob/9a7af07ba878775030838c526bba40862a9bd9a4/mods/archive-preview.wh.cpp))

Initial release.
