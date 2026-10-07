## 2.17 ([Oct 7, 2026](https://github.com/ramensoftware/windhawk-mods/blob/7d87bb3f48d7cfc2bd2150fc583a4e481fb84a29/mods/classic-webview-pane.wh.cpp))

* Add individually switchable file thumbnails, an image viewer for Pictures/imgview folders, metadata, attributes, multiple-selection details, special-folder templates/actions and background printer updates.
* Add zoom, panning, actual size, best fit, printing and a separate image window. The Separate Window button can instead launch a user-selected viewer with configurable arguments and a quoted image path.
* Add Windows 2000/98/Me profiles, automatic English/Russian captions based on the Windows display language, language overrides and editable text captions. Built-in clouds use Picture width 0; squares use 127, matching the reference DLL, without overwriting the saved manual width.
* Preserve the 2.16 disk-chart and startup-layout fixes. Limit attachment to Explorer folder frames, preserve Desktop interaction and remove obsolete preview controls when the active folder changes.
* Retain a complete pane during Shell view replacement, preserve the shared sunken frame and corner artwork, and use the same NT5 pane width in Pictures and ordinary folders.
* Correct preview-toolbar painting, resizing and window recreation. Remove the extra white toolbar gap in both embedded and separate viewers; scroll overflowing details independently of the preview, with optional compact headers.
* Restore special-folder description plaques with system tooltip colours/font and tight, balanced text padding; correct profile colours and background blending.
* Keep divider colours in their original positions when a scrollbar appears: clip the full-width divider beneath the scrollbar. Avoid spurious horizontal scrolling caused by DPI rounding.
* Cancel stale preview results, join workers before unloading and preserve complete settings snapshots during nested Shell callbacks. Independently implement the identified Windows-source fragments and clarify setting scope/defaults.

## 2.16 ([Oct 4, 2026](https://github.com/ramensoftware/windhawk-mods/blob/f79c82018ed58eb880450f0b94f74ecfc5083c15/mods/classic-webview-pane.wh.cpp))

* Fix the disk usage pie chart's side wall and outlines.
* Correct the colours of fully empty drives and prevent tiny sectors from rounding into a full pie.
* Keep the WebView pane hidden until its spacer and folder layout are ready, preventing a brief initial appearance in small windows.

## 2.15 ([Sep 30, 2026](https://github.com/ramensoftware/windhawk-mods/blob/c871a2fda4c89a8f1d224abb4f02eda91cb1a98b/mods/classic-webview-pane.wh.cpp))

Initial release.
