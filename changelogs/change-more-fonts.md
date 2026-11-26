## 1.1.0 ([Nov 26, 2026](https://github.com/ramensoftware/windhawk-mods/blob/9a71122b19fc848f4eaa6acf412ddbc396fa8f55/mods/change-more-fonts.wh.cpp))

* Set a font or weight for each executable, or turn off replacement just for that app. If the override font is unavailable, the app uses the global font.
* Give a source font its own replacement, including a specific monospace font. List-only mode limits replacements to the families and rules you enter.
* Keep the full DirectWrite fallback list if any entry is in **Keep these fonts**.
* Fix Arial Bold showing up as Arial Black in classic controls. GDI and DirectWrite preserve the weight implied by family names like Segoe UI Semibold.
* Keep the replacement family when apps create bold or underlined variants of it.
* Skip font lookup and hook setup for renderers that start disabled.
* Refresh DirectWrite's font list when you save settings. The mod readme now explains what to check when a font doesn't change.

## 1.0.0 ([Nov 19, 2026](https://github.com/ramensoftware/windhawk-mods/blob/6c8d723b7e8dfef68d6b706de29dcf9a933cea70/mods/change-more-fonts.wh.cpp))

Initial release.
