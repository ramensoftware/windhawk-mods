## 1.5 ([Jan 31, 2026](https://github.com/ramensoftware/windhawk-mods/blob/cecaca7b6506e55b663a4a7ee391cb5763a7285c/mods/explorer-details-better-file-sizes.wh.cpp))

* When showing folder sizes, the sizes are now shown in Tiles and Content folder view, not only in Details view. In addition, folder sizes are shown in the status bar, and in the details pane (enabled via OldNewExplorer). Note: An explorer restart might be required to apply these changes.
* Fixed the "Use IEC terms" option for locales, such as Korean, which don't have a space, e.g. "50MB" and not "50 MB".
* Fixed explorer freezing in some rare cases, possibly related to some network drives.

## 1.4.11 ([Jun 29, 2025](https://github.com/ramensoftware/windhawk-mods/blob/5698ca7310087986276b6cb408fb83ed73d155d6/mods/explorer-details-better-file-sizes.wh.cpp))

* Undid the Everything v1.4 fix from the previous version which was incorrect.

## 1.4.10 ([Jun 28, 2025](https://github.com/ramensoftware/windhawk-mods/blob/f0a4fe87b2b24a0e9f9a685f7dfd90968a2cf132/mods/explorer-details-better-file-sizes.wh.cpp))

* Fixed querying Everything v1.4 when custom macros named "folder" or "wfn" are configured.
* Fixed querying Everything v1.5 when the **alpha_instance** option is set to false.
* Libraries are now loaded with the `LOAD_LIBRARY_SEARCH_SYSTEM32` flag to prevent malicious or accidental dll hijacking.
* Excluded the search menu process due to a reported incompatibility which is still being investigated. Several other processes, such as the start menu and the lock screen, were excluded as well. These processes never use the explorer details control, so this mod isn't useful for them anyway.

## 1.4.9 ([Apr 15, 2025](https://github.com/ramensoftware/windhawk-mods/blob/7ae6bca49f773eef9966ceedc8e9e2495f896463/mods/explorer-details-better-file-sizes.wh.cpp))

* Added support for long paths.
* Reduced footprint for processes which never use a file list control.
* Reduced footprint if "Everything" 1.4 isn't used. That's yet another reason to switch to ["Everything" 1.5 Alpha](https://www.voidtools.com/forum/viewtopic.php?t=9787), which is also about 20x faster at size query operations. Despite the "Alpha" label, it's stable for daily use according to users' feedback and my own experience.
* Fixed a crash in 32-bit programs when the "Use IEC terms" option is enabled and the "Use MB/GB for large files" option isn't enabled.
* Fixed Everything hanging in some cases if its executable name is Everything64.exe and not Everything.exe.
* Fixed slow file list loading when browsing slow network share folders.
* Fixed Explorer showing incorrect operation sizes in some cases, such as when extracting files from a 7-Zip archive.

## 1.4.8 ([Feb 17, 2025](https://github.com/ramensoftware/windhawk-mods/blob/712276e60d0edae88611fcebec9f9119298e7ba0/mods/explorer-details-better-file-sizes.wh.cpp))

* Fixed "Everything" 1.5.0.1384a or newer hanging when browsing for files in "Everything" itself using the system dialog.
* Fixed the "Use IEC terms" option not working if "Use MB/GB for large files" is disabled.
* Fixed size not shown for empty OneDrive folders (and possibly other non-standard folders).

## 1.4.7 ([Dec 18, 2024](https://github.com/ramensoftware/windhawk-mods/blob/28f831fb2eb390551fac6fa88ca6bf6c590fac01/mods/explorer-details-better-file-sizes.wh.cpp))

* "Everything" integration for folder sizes: Added support for folder junctions.

## 1.4.6 ([Dec 16, 2024](https://github.com/ramensoftware/windhawk-mods/blob/4daa0db637b47f3da820ad7d1cdcc64236068d12/mods/explorer-details-better-file-sizes.wh.cpp))

* "Everything" integration for folder sizes: Added support for the new Everything SDK3. With "Everything" version 1.5.0.1384a or newer, the folder size query is much faster now (can be around 20x faster).
* "Everything" integration for folder sizes: Folder junctions no longer have a size (zero size was shown before).
* Fixed sorting folders by size when the "Mix files and folders when sorting by size" option is disabled.
* Folder sizes are now displayed in more places, including "Library" folders and the details pane.
* Fixed a rare mod hang on unload.

## 1.4.5 ([Dec 4, 2024](https://github.com/ramensoftware/windhawk-mods/blob/1c07c82111a8cfb4c2a1757f702e598172570193/mods/explorer-details-better-file-sizes.wh.cpp))

* Fixed Explorer hanging when Open Path is used in Everything.
* Excluded Plex Media Server which is incompatible, a proper fix will be implemented later.
* Added support for older Windows 10 versions.

## 1.4.4 ([Nov 10, 2024](https://github.com/ramensoftware/windhawk-mods/blob/f396f78d645dc57089bf4805a63b0bd82f50d745/mods/explorer-details-better-file-sizes.wh.cpp))

* Fixed the "Mix files and folders when sorting by size" option that stopped working in version 1.4.2.

## 1.4.3 ([Nov 10, 2024](https://github.com/ramensoftware/windhawk-mods/blob/49e105f33c5fef9363a0dae4f6e12a9b5ec31746/mods/explorer-details-better-file-sizes.wh.cpp))

* Excluded Plex which is incompatible, a proper fix will be implemented later.
* Excluded conhost to reduce possible conflicts, the mod isn't relevant for it anyway.
* Improved the workaround to the error message that says "Runtime Error - Not Enough space for thread Data" to cover more (hopefully all) cases.

## 1.4.2 ([Nov 9, 2024](https://github.com/ramensoftware/windhawk-mods/blob/319700c1152b5e0d664b6832eff4bb003d92dfee/mods/explorer-details-better-file-sizes.wh.cpp))

* Fixed the mod interfering with file size calculation in operations such as copying folders.
* Added a workaround to the error message that says "Runtime Error - Not Enough space for thread Data". The error could occur when a process is shutting down due to a somewhat specific incompatibility between the Microsoft C++ runtime library and Windhawk's C++ library. It's not really harmful, as the process is shutting down anyway, but it can be annoying.

## 1.4.1 ([Nov 9, 2024](https://github.com/ramensoftware/windhawk-mods/blob/a338ea9f84db30284a1e95afd8475423e389ef58/mods/explorer-details-better-file-sizes.wh.cpp))

* Fixed elevated processes being unable to receive folder sizes from Everything, causing a timeout and missing folder sizes.
* Made the "Use MB/GB for large files" option work for older file dialogs too, for example Regedit's export dialog.

## 1.4 ([Nov 8, 2024](https://github.com/ramensoftware/windhawk-mods/blob/8a97216088069321df239b51174247ec6342138c/mods/explorer-details-better-file-sizes.wh.cpp))

* Added an option to mix files and folders when sorting by size.

## 1.3 ([Nov 8, 2024](https://github.com/ramensoftware/windhawk-mods/blob/404f046ed97d3f4f50c777b774419268c318de7a/mods/explorer-details-better-file-sizes.wh.cpp))

* The integration with Everything was rewritten and is now faster (about x6 speedup), more stable (occasional hangs were fixed), and more robust overall.

## 1.2.1 ([Nov 7, 2024](https://github.com/ramensoftware/windhawk-mods/blob/2d7a490c8540c53e5c97f7b8f96789c2a96d5fd4/mods/explorer-details-better-file-sizes.wh.cpp))

* Added support for Everything 1.5 alpha.

## 1.2 ([Nov 6, 2024](https://github.com/ramensoftware/windhawk-mods/blob/b789eaa75dec0f1005f0c08b8657a9901bbb910d/mods/explorer-details-better-file-sizes.wh.cpp))

* Added an option for getting folder sizes via "Everything" integration, which results in immediate folder sizes without having to calculate them manually.
* Improved the performance of sorting folders by size.

## 1.1 ([Oct 31, 2024](https://github.com/ramensoftware/windhawk-mods/blob/0948b033c97db8abadfab78beffec4628a3924e0/mods/explorer-details-better-file-sizes.wh.cpp))

* Added an option to show folder sizes too.

## 1.0 ([Oct 29, 2024](https://github.com/ramensoftware/windhawk-mods/blob/244c09ae0f58a664e6590bcce385b7f99cc2305a/mods/explorer-details-better-file-sizes.wh.cpp))

Initial release.
