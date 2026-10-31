## 2.0 ([Oct 31, 2026](https://github.com/ramensoftware/windhawk-mods/blob/5685777d56f943dad16b7817f4ada0d71b0a1380/mods/taskbar-open-window-underline.wh.cpp))

### Added
- **Icon underline (experimental)** — a settings group that draws a line under each button's icon with the same open/minimized rule as the label underline, so the mod also works with "Combine taskbar buttons and hide labels".
  - A composition `SpriteVisual` attached to `Image#Icon` via `ElementCompositionPreview::SetElementChildVisual`, sized and positioned relative to the element (`RelativeSizeAdjustment` / `RelativeOffsetAdjustment`): it follows the icon through resizes and DPI changes with no layout code and never enters the taskbar's layout.
  - Options: thickness, width (% of the icon), distance from the icon (negative moves it over the icon), corner radius, opacity, color.
  - Color: `#RRGGBB` / `#AARRGGBB`; `light|dark` variants; comma-separated horizontal gradients; both combinable. Empty uses the system accent color. The theme is read from `SystemUsesLightTheme`, since the taskbar follows the system theme.
  - Corner radius: a `CompositionGeometricClip` with a rounded-rectangle geometry whose size is bound to the icon element's visual by an expression animation, so it tracks the line's relative width; 0 disables the clip.
  - Colors are cached and refreshed through UISettings::ColorValuesChanged, which covers accent changes and system theme switches; subscribed whenever the icon underline is on; the brush and clip are rebuilt only when their parameters change, identified by their `Comment`.
  - Visuals are tagged with a `Comment`, tracked only as weak element references, and hidden and detached when the option is turned off and on unload. An element that already carries another mod's child visual is left alone.
- **Hide the running indicator** — hides the taskbar's own indicator under running buttons so the underline is the only cue. The same element marks the active window and, on some builds, attention requests, so those markers are hidden too (intended). Independent of the icon underline. Implemented by setting `IsVisible(false)` on `Rectangle#RunningIndicator`'s hand-off composition visual rather than its XAML properties, which the button's visual states would overwrite; restored when the option is turned off and on unload, handing control back to XAML.

### Changed
- A button without a label is evaluated when the icon underline or the indicator option is on; the view-model resolver works from the `ItemsRepeater` item, not the text. The title fallback still requires a label.
- README: icon-underline screenshot, the hidden-labels dependency on the view-model symbols, and the new options.

## 1.0 ([Oct 19, 2026](https://github.com/ramensoftware/windhawk-mods/blob/ed9b52479fb527279540b2fb7afcd367afac640e/mods/taskbar-open-window-underline.wh.cpp))

Initial release.
