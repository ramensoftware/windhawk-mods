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