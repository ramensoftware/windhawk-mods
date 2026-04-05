## 1.2.0 ([Apr 5, 2026](https://github.com/ramensoftware/windhawk-mods/blob/33e8963bbd0dd420be7812ec2e6a22f68c75db8d/mods/explorer-breadcrumb-mclick-newtab.wh.cpp))

[Added] Dropdown Menu Support: Middle-clicking subfolders within the > (chevron) dropdown menus in the address bar now directly opens the selected folder in a new background tab.

[Improved] Smart Menu Detection: Expanded the AnalyzeElement function to dynamically distinguish between a standard breadcrumb click and a dropdown menu item click (UIA List, Text, or Menu items).

[Improved] Expanded State Detection: Introduced the FindExpandedBreadcrumbElement function to accurately identify which breadcrumb menu is currently open in the background. The system flawlessly combines the clicked subfolder's name with the expanded parent path.

[Stability] The 100% clipboard-free architecture—which preserves your copied data and prevents Explorer crashes—has been maintained and fully integrated with the new dropdown feature.

## 1.1.0 ([Mar 23, 2026](https://github.com/ramensoftware/windhawk-mods/blob/3b18c32d213fdf0e1047f5e9615ba1e334fbbdcd/mods/explorer-breadcrumb-mclick-newtab.wh.cpp))

🚀 Release v1.1.0: The Clipboard-Free Update
This major update completely redesigns how the mod reads and writes folder paths, moving away from the clipboard-dependent logic of v1.0.0 to a much safer and less intrusive architecture.

✨ Key Improvements & Fixes
Preserves Copied Files (Non-Destructive): In v1.0.0, the mod temporarily hijacked the Windows Clipboard to read and paste paths, which inadvertently destroyed any copied files or complex data sitting in your clipboard. Version 1.1.0 is 100% Clipboard-Free. You can now copy a file, middle-click a folder to open a new tab, and safely paste your file without interruption.

Increased Reliability: Removed the reliance on simulated Ctrl+C and Ctrl+V commands, which could sometimes misfire depending on system lag or clipboard history monitors.

⚙️ Under the Hood (v1.0.0 vs v1.1.0)
Native UIA Probing: Replaced ProbeAddressBarWithClipboard with ProbeAddressBarWithUIA. Instead of forcing a copy command, the mod now securely reads the ValuePattern of the active XAML address bar directly through UIAutomation.

Unicode Injection: Replaced the clipboard-paste navigation sequence. Target folder paths are now instantly typed into the new tab's address bar using native KEYEVENTF_UNICODE input vectors, bypassing the clipboard entirely.

Code Cleanup: Completely removed the old BackupClipboard and RestoreClipboard structures, resulting in a cleaner and more efficient execution flow.

## 1.0.0 ([Jan 30, 2026](https://github.com/ramensoftware/windhawk-mods/blob/161fe6155b4e2de349f68456abe63b55027bc78c/mods/explorer-breadcrumb-mclick-newtab.wh.cpp))

Initial release.
