// ==WindhawkMod==
// @id              translucent-flyouts
// @name            Translucent Flyouts
// @description     Acrylic, Mica and blur backgrounds for context menus, dropdowns and tooltips in every app
// @version         0.6.6
// @author          DavidHiFi
// @github          https://github.com/DavidHiFi
// @homepage        https://github.com/DavidHiFi/davids-windhawk-mods
// @exclude         discord*.exe
// @exclude         Voicemeeter*.exe
// @exclude         Matrix*.exe
// @exclude         Stream Deck*.exe
// @exclude         dwm.exe
// @architecture    x86-64
// @architecture    x86
// @license         LGPL-3.0
// @compilerOptions -ld2d1 -ldwrite -ldwmapi -luxtheme -lcomctl32 -lgdi32 -luser32 -lole32 -ladvapi32 -lshlwapi -lversion -lmsimg32 -loleaut32 -luuid -lwindowscodecs
// ==/WindhawkMod==

// This mod is a port of TranslucentFlyouts (TFMain) by ALTaleX531 to the
// Windhawk mod platform. Upstream: https://github.com/ALTaleX531/TranslucentFlyouts
// (LGPL-3.0). The settings surface follows the TranslucentFlyouts configuration
// schema (as exposed by GID0317's translucent-flyouts-controller). No helper
// application is required: this mod implements the engine itself.

// ==WindhawkModSettings==
/*
- global:
  - effectType: modern_acrylic
    $name: Effect Type
    $description: Choose the background effect used for this pop-up type. Use use_global to inherit the Global value
    $options:
      - none: None / Disabled
      - transparent: Fully Transparent
      - solid: Solid Color
      - blurred: Blurred
      - acrylic: Acrylic
      - modern_acrylic: Modern Acrylic (Recommended)
      - acrylic_bg: Acrylic Background Layer
      - mica_bg: Mica Background Layer
      - mica_variant: Mica Variant Background Layer
  - cornerType: small_round
    $name: Corner Style
    $description: Choose the corner shape and roundness for this pop-up type. Use use_global to inherit the Global value
    $options:
      - dont_change: Don't Change
      - sharp: Sharp Corners
      - large_round: Large Round Corners
      - small_round: Small Round Corners
  - enableDropShadow: false
    $name: Enable Drop Shadow
    $description: Enable or disable drop shadow behind this pop-up type. Use use_global to inherit the Global value
  - noBorderColor: false
    $name: Disable Border Color
    $description: Choose whether to render system borders for this pop-up type. Use use_global to inherit the Global value
  - enableThemeColorization: false
    $name: Enable Theme Colorization
    $description: Use current theme accent color for borders instead of fixed custom border colors. Use use_global to inherit the Global value
  - darkModeThemeColorizationType: start_hover
    $name: Dark Accent Source
    $description: Choose which immersive color slot is used in dark mode when theme colorization is enabled
    $options:
      - start_background: ImmersiveStartBackground
      - start_hover: ImmersiveStartHoverBackground
      - system_accent: ImmersiveSystemAccent
      - accent_dark1: ImmersiveSystemAccentDark1
      - accent_dark2: ImmersiveSystemAccentDark2
      - accent_dark3: ImmersiveSystemAccentDark3
      - accent_light1: ImmersiveSystemAccentLight1
      - accent_light2: ImmersiveSystemAccentLight2
      - accent_light3: ImmersiveSystemAccentLight3
  - lightModeThemeColorizationType: start_hover
    $name: Light Accent Source
    $description: Choose which immersive color slot is used in light mode when theme colorization is enabled
    $options:
      - start_background: ImmersiveStartBackground
      - start_hover: ImmersiveStartHoverBackground
      - system_accent: ImmersiveSystemAccent
      - accent_dark1: ImmersiveSystemAccentDark1
      - accent_dark2: ImmersiveSystemAccentDark2
      - accent_dark3: ImmersiveSystemAccentDark3
      - accent_light1: ImmersiveSystemAccentLight1
      - accent_light2: ImmersiveSystemAccentLight2
      - accent_light3: ImmersiveSystemAccentLight3
  - darkModeBorderColor: "0xFF2B2B2B"
    $name: Dark Border Color
    $description: Dark mode border color in ARGB hex format 0xAARRGGBB
  - lightModeBorderColor: "0xFFDDDDDD"
    $name: Light Border Color
    $description: Light mode border color in ARGB hex format 0xAARRGGBB
  - darkModeGradientColor: "0x412B2B2B"
    $name: Dark Gradient Color
    $description: Dark mode gradient or acrylic tint in ARGB hex format 0xAARRGGBB
  - lightModeGradientColor: "0x9EDDDDDD"
    $name: Light Gradient Color
    $description: Light mode gradient or acrylic tint in ARGB hex format 0xAARRGGBB
  - enableMiniDump: true
    $name: Enable MiniDump
    $description: Write crash minidump files for troubleshooting when TranslucentFlyouts fails
  - disabled: false
    $name: Disabled
    $description: Disable all effects for this pop-up type. Use use_global to inherit the Global value where available
  $name: Global

- dropDown:
  - effectType: use_global
    $name: Effect Type
    $description: Choose the background effect used for this pop-up type. Use use_global to inherit the Global value
    $options:
      - none: None
      - transparent: Fully Transparent
      - solid: Solid Color
      - blurred: Blurred
      - acrylic: Acrylic
      - modern_acrylic: Modern Acrylic
      - acrylic_bg: Acrylic Background Layer
      - mica_bg: Mica Background Layer
      - mica_variant: Mica Variant Background Layer
      - use_global: Use Global Setting
  - cornerType: use_global
    $name: Corner Style
    $description: Choose the corner shape and roundness for this pop-up type. Use use_global to inherit the Global value
    $options:
      - dont_change: Don't Change
      - sharp: Sharp
      - large_round: Large Round
      - small_round: Small Round
      - use_global: Use Global Setting
  - enableDropShadow: use_global
    $name: Enable Drop Shadow
    $description: Enable or disable drop shadow behind this pop-up type. Use use_global to inherit the Global value
    $options:
      - no: No
      - yes: Yes
      - use_global: Use Global Setting
  - enableFluentAnimation: false
    $name: Enable Fluent Animation
    $description: Enable fluent pop-up animations for this category
  - noBorderColor: use_global
    $name: Disable Border Color
    $description: Choose whether to render system borders for this pop-up type. Use use_global to inherit the Global value
    $options:
      - no: No
      - yes: Yes
      - use_global: Use Global Setting
  - enableThemeColorization: use_global
    $name: Enable Theme Colorization
    $description: Use current theme accent color for borders instead of fixed custom border colors. Use use_global to inherit the Global value
    $options:
      - no: No
      - yes: Yes
      - use_global: Use Global Setting
  - darkModeThemeColorizationType: 1
    $name: Dark Accent Source
    $description: Choose which immersive color slot is used in dark mode when theme colorization is enabled
    $options:
      - 0: ImmersiveStartBackground
      - 1: ImmersiveStartHoverBackground
      - 2: ImmersiveSystemAccent
      - 3: ImmersiveSystemAccentDark1
      - 4: ImmersiveSystemAccentDark2
      - 5: ImmersiveSystemAccentDark3
      - 6: ImmersiveSystemAccentLight1
      - 7: ImmersiveSystemAccentLight2
      - 8: ImmersiveSystemAccentLight3
  - lightModeThemeColorizationType: 1
    $name: Light Accent Source
    $description: Choose which immersive color slot is used in light mode when theme colorization is enabled
    $options:
      - 0: ImmersiveStartBackground
      - 1: ImmersiveStartHoverBackground
      - 2: ImmersiveSystemAccent
      - 3: ImmersiveSystemAccentDark1
      - 4: ImmersiveSystemAccentDark2
      - 5: ImmersiveSystemAccentDark3
      - 6: ImmersiveSystemAccentLight1
      - 7: ImmersiveSystemAccentLight2
      - 8: ImmersiveSystemAccentLight3
  - darkModeBorderColor: "0xFF2B2B2B"
    $name: Dark Border Color
    $description: Dark mode border color in ARGB hex format 0xAARRGGBB
  - lightModeBorderColor: "0xFFDDDDDD"
    $name: Light Border Color
    $description: Light mode border color in ARGB hex format 0xAARRGGBB
  - darkModeGradientColor: "0x412B2B2B"
    $name: Dark Gradient Color
    $description: Dark mode gradient or acrylic tint in ARGB hex format 0xAARRGGBB
  - lightModeGradientColor: "0x9EDDDDDD"
    $name: Light Gradient Color
    $description: Light mode gradient or acrylic tint in ARGB hex format 0xAARRGGBB
  - disabled: use_global
    $name: Disabled
    $description: Disable all effects for this pop-up type. Use use_global to inherit the Global value where available
    $options:
      - no: No
      - yes: Yes
      - use_global: Use Global Setting
  - animation_fadeOutTime: 350
    $name: Fade Out Time
    $description: Duration of fade-out animation in milliseconds
  - animation_popInTime: 250
    $name: Pop In Time
    $description: Duration of pop-in animation in milliseconds
  - animation_fadeInTime: 87
    $name: Fade In Time
    $description: Duration of fade-in animation in milliseconds
  - animation_popInStyle: slide_down
    $name: Pop In Style
    $description: Style of the pop-in animation when the element appears
    $options:
      - slide_down: Slide Down
      - ripple: Ripple
      - smooth_scroll: Smooth Scroll
      - smooth_zoom: Smooth Zoom
  - animation_startRatio: 50
    $name: Start Ratio
    $description: Start ratio for pop-in animation. Higher values start closer to final state
  - animation_enableImmediateInterupting: false
    $name: Enable Immediate Interrupting
    $description: Allow running animations to be interrupted immediately by a new state change
  $name: DropDown

- menu:
  - noSystemDropShadow: false
    $name: Disable System Drop Shadow
    $description: Disable system-provided menu shadow
  - enableImmersiveStyle: true
    $name: Enable Immersive Style
    $description: Use modern uniformly styled pop-up menus. Recommended on Windows 11
  - enableCustomRendering: false
    $name: Enable Custom Rendering
    $description: Fully render pop-up menus using custom rendering. Required for several advanced Menu visual options
  - enableFluentAnimation: false
    $name: Enable Fluent Animation
    $description: Enable fluent menu animations
  - enableCompatibilityMode: false
    $name: Enable Compatibility Mode
    $description: Use compatibility mode for apps that misbehave with normal menu rendering
  - noModernAppBackgroundColor: true
    $name: Disable Modern App Background Color
    $description: Ignore modern app provided background color and use configured TranslucentFlyouts appearance instead
  - colorTreatAsTransparentEnabled: false
    $name: Color Treat As Transparent(Enable)
    $description: Treat a specific menu color as transparent during rendering
  - colorTreatAsTransparent: "0x00000000"
    $name: Color Treat As Transparent(ARGB)
    $description: ARGB key color treated as transparent in menu rendering, format 0xAARRGGBB
  - colorTreatAsTransparentThreshold: 50
    $name: Color Treat As Transparent Threshold
    $description: Tolerance for transparent color matching. Higher values match a wider color range
  - effectType: use_global
    $name: Effect Type
    $description: Choose the background effect used for this pop-up type. Use use_global to inherit the Global value
    $options:
      - none: None
      - transparent: Fully Transparent
      - solid: Solid Color
      - blurred: Blurred
      - acrylic: Acrylic
      - modern_acrylic: Modern Acrylic
      - acrylic_bg: Acrylic Background Layer
      - mica_bg: Mica Background Layer
      - mica_variant: Mica Variant Background Layer
      - use_global: Use Global Setting
  - cornerType: use_global
    $name: Corner Style
    $description: Choose the corner shape and roundness for this pop-up type. Use use_global to inherit the Global value
    $options:
      - dont_change: Don't Change
      - sharp: Sharp
      - large_round: Large Round
      - small_round: Small Round
      - use_global: Use Global Setting
  - enableDropShadow: use_global
    $name: Enable Drop Shadow
    $description: Enable or disable drop shadow behind this pop-up type. Use use_global to inherit the Global value
    $options:
      - no: No
      - yes: Yes
      - use_global: Use Global Setting
  - noBorderColor: use_global
    $name: Disable Border Color
    $description: Choose whether to render system borders for this pop-up type. Use use_global to inherit the Global value
    $options:
      - no: No
      - yes: Yes
      - use_global: Use Global Setting
  - enableThemeColorization: use_global
    $name: Enable Theme Colorization
    $description: Use current theme accent color for borders instead of fixed custom border colors. Use use_global to inherit the Global value
    $options:
      - no: No
      - yes: Yes
      - use_global: Use Global Setting
  - darkModeThemeColorizationType: 1
    $name: Dark Accent Source
    $description: Choose which immersive color slot is used in dark mode when theme colorization is enabled
    $options:
      - 0: ImmersiveStartBackground
      - 1: ImmersiveStartHoverBackground
      - 2: ImmersiveSystemAccent
      - 3: ImmersiveSystemAccentDark1
      - 4: ImmersiveSystemAccentDark2
      - 5: ImmersiveSystemAccentDark3
      - 6: ImmersiveSystemAccentLight1
      - 7: ImmersiveSystemAccentLight2
      - 8: ImmersiveSystemAccentLight3
  - lightModeThemeColorizationType: 1
    $name: Light Accent Source
    $description: Choose which immersive color slot is used in light mode when theme colorization is enabled
    $options:
      - 0: ImmersiveStartBackground
      - 1: ImmersiveStartHoverBackground
      - 2: ImmersiveSystemAccent
      - 3: ImmersiveSystemAccentDark1
      - 4: ImmersiveSystemAccentDark2
      - 5: ImmersiveSystemAccentDark3
      - 6: ImmersiveSystemAccentLight1
      - 7: ImmersiveSystemAccentLight2
      - 8: ImmersiveSystemAccentLight3
  - darkModeBorderColor: "0xFF2B2B2B"
    $name: Dark Border Color
    $description: Dark mode border color in ARGB hex format 0xAARRGGBB
  - lightModeBorderColor: "0xFFDDDDDD"
    $name: Light Border Color
    $description: Light mode border color in ARGB hex format 0xAARRGGBB
  - darkModeGradientColor: "0x412B2B2B"
    $name: Dark Gradient Color
    $description: Dark mode gradient or acrylic tint in ARGB hex format 0xAARRGGBB
  - lightModeGradientColor: "0x9EDDDDDD"
    $name: Light Gradient Color
    $description: Light mode gradient or acrylic tint in ARGB hex format 0xAARRGGBB
  - disabled: use_global
    $name: Disabled
    $description: Disable all effects for this pop-up type. Use use_global to inherit the Global value where available
    $options:
      - no: No
      - yes: Yes
      - use_global: Use Global Setting
  - animation_fadeOutTime: 350
    $name: Fade Out Time
    $description: Duration of fade-out animation in milliseconds
  - animation_popInTime: 250
    $name: Pop In Time
    $description: Duration of pop-in animation in milliseconds
  - animation_fadeInTime: 87
    $name: Fade In Time
    $description: Duration of fade-in animation in milliseconds
  - animation_popInStyle: slide_down
    $name: Pop In Style
    $description: Style of the pop-in animation when the element appears
    $options:
      - slide_down: Slide Down
      - ripple: Ripple
      - smooth_scroll: Smooth Scroll
      - smooth_zoom: Smooth Zoom
  - animation_startRatio: 50
    $name: Start Ratio
    $description: Start ratio for pop-in animation. Higher values start closer to final state
  - animation_enableImmediateInterupting: false
    $name: Enable Immediate Interrupting
    $description: Allow running animations to be interrupted immediately by a new state change
  - separator_disabled: 0
    $name: Separator / Disabled
    $description: Disable all effects for this pop-up type. Use use_global to inherit the Global value where available
  - separator_width: 1000
    $name: Separator / Width
    $description: Separator line thickness control. 1000 equals full default thickness
  - separator_darkModeColor: "0x30D9D9D9"
    $name: Separator / Dark Color
    $description: Dark mode color in ARGB hex format 0xAARRGGBB
  - separator_lightModeColor: "0x30262626"
    $name: Separator / Light Color
    $description: Light mode color in ARGB hex format 0xAARRGGBB
  - separator_enableThemeColorization: 0
    $name: Separator / Enable Theme Colorization
    $description: Use current theme accent color for borders instead of fixed custom border colors. Use use_global to inherit the Global value
  - separator_darkThemeColorizationType: 1
    $name: Separator / Dark Accent Source
    $description: Choose which immersive color slot is used in dark mode for this sub-part
    $options:
      - 0: ImmersiveStartBackground
      - 1: ImmersiveStartHoverBackground
      - 2: ImmersiveSystemAccent
      - 3: ImmersiveSystemAccentDark1
      - 4: ImmersiveSystemAccentDark2
      - 5: ImmersiveSystemAccentDark3
      - 6: ImmersiveSystemAccentLight1
      - 7: ImmersiveSystemAccentLight2
      - 8: ImmersiveSystemAccentLight3
  - separator_lightThemeColorizationType: 1
    $name: Separator / Light Accent Source
    $description: Choose which immersive color slot is used in light mode for this sub-part
    $options:
      - 0: ImmersiveStartBackground
      - 1: ImmersiveStartHoverBackground
      - 2: ImmersiveSystemAccent
      - 3: ImmersiveSystemAccentDark1
      - 4: ImmersiveSystemAccentDark2
      - 5: ImmersiveSystemAccentDark3
      - 6: ImmersiveSystemAccentLight1
      - 7: ImmersiveSystemAccentLight2
      - 8: ImmersiveSystemAccentLight3
  - focusing_disabled: 0
    $name: Focusing / Disabled
    $description: Disable all effects for this pop-up type. Use use_global to inherit the Global value where available
  - focusing_cornerRadius: 8
    $name: Focusing / Corner Radius
    $description: Corner radius for focused item highlight
  - focusing_width: 1000
    $name: Focusing / Width
    $description: Width control for focused item highlight. 1000 equals full item width
  - focusing_darkModeColor: "0xFFFFFFFF"
    $name: Focusing / Dark Color
    $description: Dark mode color in ARGB hex format 0xAARRGGBB
  - focusing_lightModeColor: "0xFF000000"
    $name: Focusing / Light Color
    $description: Light mode color in ARGB hex format 0xAARRGGBB
  - focusing_enableThemeColorization: 0
    $name: Focusing / Enable Theme Colorization
    $description: Use current theme accent color for borders instead of fixed custom border colors. Use use_global to inherit the Global value
  - focusing_darkThemeColorizationType: 1
    $name: Focusing / Dark Accent Source
    $description: Choose which immersive color slot is used in dark mode for this sub-part
    $options:
      - 0: ImmersiveStartBackground
      - 1: ImmersiveStartHoverBackground
      - 2: ImmersiveSystemAccent
      - 3: ImmersiveSystemAccentDark1
      - 4: ImmersiveSystemAccentDark2
      - 5: ImmersiveSystemAccentDark3
      - 6: ImmersiveSystemAccentLight1
      - 7: ImmersiveSystemAccentLight2
      - 8: ImmersiveSystemAccentLight3
  - focusing_lightThemeColorizationType: 1
    $name: Focusing / Light Accent Source
    $description: Choose which immersive color slot is used in light mode for this sub-part
    $options:
      - 0: ImmersiveStartBackground
      - 1: ImmersiveStartHoverBackground
      - 2: ImmersiveSystemAccent
      - 3: ImmersiveSystemAccentDark1
      - 4: ImmersiveSystemAccentDark2
      - 5: ImmersiveSystemAccentDark3
      - 6: ImmersiveSystemAccentLight1
      - 7: ImmersiveSystemAccentLight2
      - 8: ImmersiveSystemAccentLight3
  - disabledHot_disabled: 0
    $name: Disabled Hot / Disabled
    $description: Disable all effects for this pop-up type. Use use_global to inherit the Global value where available
  - disabledHot_cornerRadius: 8
    $name: Disabled Hot / Corner Radius
    $description: Corner radius for disabled hot item highlight
  - disabledHot_darkModeColor: "0x00000000"
    $name: Disabled Hot / Dark Color
    $description: Dark mode color in ARGB hex format 0xAARRGGBB
  - disabledHot_lightModeColor: "0x00000000"
    $name: Disabled Hot / Light Color
    $description: Light mode color in ARGB hex format 0xAARRGGBB
  - disabledHot_enableThemeColorization: 0
    $name: Disabled Hot / Enable Theme Colorization
    $description: Use current theme accent color for borders instead of fixed custom border colors. Use use_global to inherit the Global value
  - disabledHot_darkThemeColorizationType: 1
    $name: Disabled Hot / Dark Accent Source
    $description: Choose which immersive color slot is used in dark mode for this sub-part
    $options:
      - 0: ImmersiveStartBackground
      - 1: ImmersiveStartHoverBackground
      - 2: ImmersiveSystemAccent
      - 3: ImmersiveSystemAccentDark1
      - 4: ImmersiveSystemAccentDark2
      - 5: ImmersiveSystemAccentDark3
      - 6: ImmersiveSystemAccentLight1
      - 7: ImmersiveSystemAccentLight2
      - 8: ImmersiveSystemAccentLight3
  - disabledHot_lightThemeColorizationType: 1
    $name: Disabled Hot / Light Accent Source
    $description: Choose which immersive color slot is used in light mode for this sub-part
    $options:
      - 0: ImmersiveStartBackground
      - 1: ImmersiveStartHoverBackground
      - 2: ImmersiveSystemAccent
      - 3: ImmersiveSystemAccentDark1
      - 4: ImmersiveSystemAccentDark2
      - 5: ImmersiveSystemAccentDark3
      - 6: ImmersiveSystemAccentLight1
      - 7: ImmersiveSystemAccentLight2
      - 8: ImmersiveSystemAccentLight3
  - hot_disabled: 0
    $name: Hot / Disabled
    $description: Disable all effects for this pop-up type. Use use_global to inherit the Global value where available
  - hot_cornerRadius: 8
    $name: Hot / Corner Radius
    $description: Corner radius for hot or hover item highlight
  - hot_darkModeColor: "0x41808080"
    $name: Hot / Dark Color
    $description: Dark mode color in ARGB hex format 0xAARRGGBB
  - hot_lightModeColor: "0x30000000"
    $name: Hot / Light Color
    $description: Light mode color in ARGB hex format 0xAARRGGBB
  - hot_enableThemeColorization: 0
    $name: Hot / Enable Theme Colorization
    $description: Use current theme accent color for borders instead of fixed custom border colors. Use use_global to inherit the Global value
  - hot_darkThemeColorizationType: 1
    $name: Hot / Dark Accent Source
    $description: Choose which immersive color slot is used in dark mode for this sub-part
    $options:
      - 0: ImmersiveStartBackground
      - 1: ImmersiveStartHoverBackground
      - 2: ImmersiveSystemAccent
      - 3: ImmersiveSystemAccentDark1
      - 4: ImmersiveSystemAccentDark2
      - 5: ImmersiveSystemAccentDark3
      - 6: ImmersiveSystemAccentLight1
      - 7: ImmersiveSystemAccentLight2
      - 8: ImmersiveSystemAccentLight3
  - hot_lightThemeColorizationType: 1
    $name: Hot / Light Accent Source
    $description: Choose which immersive color slot is used in light mode for this sub-part
    $options:
      - 0: ImmersiveStartBackground
      - 1: ImmersiveStartHoverBackground
      - 2: ImmersiveSystemAccent
      - 3: ImmersiveSystemAccentDark1
      - 4: ImmersiveSystemAccentDark2
      - 5: ImmersiveSystemAccentDark3
      - 6: ImmersiveSystemAccentLight1
      - 7: ImmersiveSystemAccentLight2
      - 8: ImmersiveSystemAccentLight3
  $name: Menu

- tooltip:
  - noSystemDropShadow: false
    $name: Disable System Drop Shadow
    $description: Disable system-provided tooltip shadow
  - effectType: use_global
    $name: Effect Type
    $description: Choose the background effect used for this pop-up type. Use use_global to inherit the Global value
    $options:
      - none: None
      - transparent: Fully Transparent
      - solid: Solid Color
      - blurred: Blurred
      - acrylic: Acrylic
      - modern_acrylic: Modern Acrylic
      - acrylic_bg: Acrylic Background Layer
      - mica_bg: Mica Background Layer
      - mica_variant: Mica Variant Background Layer
      - use_global: Use Global Setting
  - cornerType: use_global
    $name: Corner Style
    $description: Choose the corner shape and roundness for this pop-up type. Use use_global to inherit the Global value
    $options:
      - dont_change: Don't Change
      - sharp: Sharp
      - large_round: Large Round
      - small_round: Small Round
      - use_global: Use Global Setting
  - enableDropShadow: use_global
    $name: Enable Drop Shadow
    $description: Enable or disable drop shadow behind this pop-up type. Use use_global to inherit the Global value
    $options:
      - no: No
      - yes: Yes
      - use_global: Use Global Setting
  - noBorderColor: use_global
    $name: Disable Border Color
    $description: Choose whether to render system borders for this pop-up type. Use use_global to inherit the Global value
    $options:
      - no: No
      - yes: Yes
      - use_global: Use Global Setting
  - enableThemeColorization: use_global
    $name: Enable Theme Colorization
    $description: Use current theme accent color for borders instead of fixed custom border colors. Use use_global to inherit the Global value
    $options:
      - no: No
      - yes: Yes
      - use_global: Use Global Setting
  - darkModeThemeColorizationType: 1
    $name: Dark Accent Source
    $description: Choose which immersive color slot is used in dark mode when theme colorization is enabled
    $options:
      - 0: ImmersiveStartBackground
      - 1: ImmersiveStartHoverBackground
      - 2: ImmersiveSystemAccent
      - 3: ImmersiveSystemAccentDark1
      - 4: ImmersiveSystemAccentDark2
      - 5: ImmersiveSystemAccentDark3
      - 6: ImmersiveSystemAccentLight1
      - 7: ImmersiveSystemAccentLight2
      - 8: ImmersiveSystemAccentLight3
  - lightModeThemeColorizationType: 1
    $name: Light Accent Source
    $description: Choose which immersive color slot is used in light mode when theme colorization is enabled
    $options:
      - 0: ImmersiveStartBackground
      - 1: ImmersiveStartHoverBackground
      - 2: ImmersiveSystemAccent
      - 3: ImmersiveSystemAccentDark1
      - 4: ImmersiveSystemAccentDark2
      - 5: ImmersiveSystemAccentDark3
      - 6: ImmersiveSystemAccentLight1
      - 7: ImmersiveSystemAccentLight2
      - 8: ImmersiveSystemAccentLight3
  - darkModeBorderColor: "0xFF2B2B2B"
    $name: Dark Border Color
    $description: Dark mode border color in ARGB hex format 0xAARRGGBB
  - lightModeBorderColor: "0xFFDDDDDD"
    $name: Light Border Color
    $description: Light mode border color in ARGB hex format 0xAARRGGBB
  - darkModeGradientColor: "0x412B2B2B"
    $name: Dark Gradient Color
    $description: Dark mode gradient or acrylic tint in ARGB hex format 0xAARRGGBB
  - lightModeGradientColor: "0x9EDDDDDD"
    $name: Light Gradient Color
    $description: Light mode gradient or acrylic tint in ARGB hex format 0xAARRGGBB
  - darkModeColor: "0xFFFFFFFF"
    $name: Dark Fill Color
    $description: Dark mode color in ARGB hex format 0xAARRGGBB
  - lightModeColor: "0xFF1A1A1A"
    $name: Light Fill Color
    $description: Light mode color in ARGB hex format 0xAARRGGBB
  - marginsType: add_to_existing
    $name: Margins Mode
    $description: Define how tooltip margins are applied, add to existing or replace existing values
    $options:
      - add_to_existing: AddToExisting
      - replace_existing: ReplaceExisting
  - marginLeft: 6
    $name: Margin Left
    $description: Tooltip left margin in pixels
  - marginRight: 6
    $name: Margin Right
    $description: Tooltip right margin in pixels
  - marginTop: 6
    $name: Margin Top
    $description: Tooltip top margin in pixels
  - marginBottom: 6
    $name: Margin Bottom
    $description: Tooltip bottom margin in pixels
  - disabled: use_global
    $name: Disabled
    $description: Disable all effects for this pop-up type. Use use_global to inherit the Global value where available
    $options:
      - no: No
      - yes: Yes
      - use_global: Use Global Setting
  $name: Tooltip


- advancedFunctions:
  - processBlockList: ""
    $name: Process Block List
    $description: Advanced users only. Comma, semicolon, or newline separated process names to block TranslucentFlyouts loading (for example explorer.exe). This can require restarting Translucent Flyouts or Windows and may cause high CPU usage if misconfigured
  - processDisabledList: ""
    $name: Process Disabled List (Global)
    $description: Advanced users only. Comma, semicolon, or newline separated process names to disable all effects globally (for example app.exe)
  - menuProcessDisabledList: ""
    $name: Process Disabled List (Menu)
    $description: Advanced users only. Comma, semicolon, or newline separated process names to disable only Menu effects
  - tooltipProcessDisabledList: ""
    $name: Process Disabled List (Tooltip)
    $description: Advanced users only. Comma, semicolon, or newline separated process names to disable only Tooltip effects
  - dropDownProcessDisabledList: ""
    $name: Process Disabled List (DropDown)
    $description: Advanced users only. Comma, semicolon, or newline separated process names to disable only DropDown effects
  $name: Advanced Functions
*/
// ==/WindhawkModSettings==

// ==WindhawkModReadme==
/*
# Translucent Flyouts

Give the classic menus, dropdowns and tooltips in every app a modern acrylic,
Mica or blur background. The TranslucentFlyouts app, rebuilt as one mod with
no helper program.

![Translucent Flyouts preview](https://raw.githubusercontent.com/DavidHiFi/davids-windhawk-mods/main/media/previews/translucent-flyouts.png)

## Features

- **Context menus, menu bars, dropdown lists and tooltips** in 64-bit and
  32-bit apps.
- **Modern Acrylic, Acrylic, Mica, blur, transparent or solid** backgrounds.
- **Rounded or square corners**, drop shadows and accent-colored borders.
- **Separate styles** for menus, dropdowns and tooltips, or one global style.
- **Light and dark mode colors**, set independently.
- **Fluent animations** for menus and dropdowns.

## Tips

- Pair it with [Dark Menus](https://windhawk.net/mods/dark-menus) and turn on
  its TranslucentFlyouts option for dark menus in every app.
- Programs listed in this mod's exclusions are left untouched. Add any app
  that should keep its original menus.

## Credits

A port of [TranslucentFlyouts](https://github.com/ALTaleX531/TranslucentFlyouts)
by ALTaleX531. The settings follow its configuration schema. LGPL-3.0.
*/
// ==/WindhawkModReadme==

#include <windhawk_api.h>
#include <windhawk_utils.h>

#include <windows.h>
#include <windowsx.h>
#include <dwmapi.h>
#include <uxtheme.h>
#include <vsstyle.h>
#include <vssym32.h>
#include <commctrl.h>
#include <shellapi.h>
#include <shlwapi.h>
#include <d2d1.h>

#include <algorithm>
#include <array>
#include <atomic>
#include <cmath>
#include <cstdint>
#include <cwchar>
#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <vector>

// The MinGW vsstyle.h predates these parts (Windows Insider Preview 22621+);
// the 26100 SDK renamed the second one without the inner underscore.
#ifndef MENU_POPUPITEMKBFOCUS
#define MENU_POPUPITEMKBFOCUS 26
#endif
#ifndef MENU_POPUPITEMFOCUSABLE
#define MENU_POPUPITEMFOCUSABLE 27
#endif

namespace tf {

// ===========================================================================
// Minimal WIL-style helpers (the upstream uses WIL; this port keeps only the
// pieces the engine needs)
// ===========================================================================

#define TF_RETURN_IF_FAILED(hr)     \
    do {                            \
        HRESULT _hr = (hr);         \
        if (FAILED(_hr)) {          \
            return _hr;             \
        }                           \
    } while (0)

#define TF_RETURN_LAST_ERROR_IF(cond)        \
    do {                                     \
        if (cond) {                          \
            return HRESULT_FROM_WIN32(GetLastError()); \
        }                                    \
    } while (0)

#define TF_THROW_IF_FAILED(hr)  \
    do {                        \
        HRESULT _hr = (hr);     \
        if (FAILED(_hr)) {      \
            throw _hr;          \
        }                       \
    } while (0)

#define TF_THROW_HR_IF(hr, cond)   \
    do {                           \
        if (cond) {                \
            throw (hr);            \
        }                          \
    } while (0)

#define TF_THROW_LAST_ERROR_IF(cond)             \
    do {                                         \
        if (cond) {                              \
            throw HRESULT_FROM_WIN32(GetLastError()); \
        }                                        \
    } while (0)

struct HandleCloser {
    using pointer = HANDLE;
    void operator()(HANDLE h) const {
        if (h) {
            DeleteObject(h);
        }
    }
};

template <typename T, typename Closer = HandleCloser>
class unique_handle {
   public:
    unique_handle() = default;
    explicit unique_handle(T value) : m_value(value) {}
    ~unique_handle() { reset(); }
    unique_handle(const unique_handle&) = delete;
    unique_handle& operator=(const unique_handle&) = delete;
    unique_handle(unique_handle&& other) noexcept : m_value(other.release()) {}
    unique_handle& operator=(unique_handle&& other) noexcept {
        reset(other.release());
        return *this;
    }
    T get() const { return m_value; }
    T release() {
        T v = m_value;
        m_value = T{};
        return v;
    }
    void reset(T value = T{}) {
        if (m_value) {
            Closer{}(m_value);
        }
        m_value = value;
    }
    explicit operator bool() const { return m_value != nullptr; }

   private:
    T m_value{};
};

struct DcCloser {
    using pointer = HDC;
    void operator()(HDC dc) const {
        if (dc) {
            ReleaseDC(nullptr, dc);
        }
    }
};

struct WindowDcCloser {
    using pointer = HDC;
    void operator()(HDC dc) const {
        if (dc) {
            ReleaseDC(nullptr, dc);
        }
    }
};

class unique_dc {
   public:
    unique_dc() = default;
    explicit unique_dc(HDC dc, HWND fromWindow = nullptr) : m_dc(dc), m_hwnd(fromWindow) {}
    ~unique_dc() { reset(); }
    unique_dc(const unique_dc&) = delete;
    unique_dc& operator=(const unique_dc&) = delete;
    HDC get() const { return m_dc; }
    operator HDC() const { return m_dc; }
    bool is_valid() const { return m_dc != nullptr; }
    HDC release() {
        HDC dc = m_dc;
        m_dc = nullptr;
        return dc;
    }
    void reset(HDC dc = nullptr, HWND fromWindow = nullptr) {
        if (m_dc) {
            if (m_hwnd) {
                ReleaseDC(m_hwnd, m_dc);
            } else {
                ReleaseDC(nullptr, m_dc);
            }
        }
        m_dc = dc;
        m_hwnd = fromWindow;
    }

   private:
    HDC m_dc{};
    HWND m_hwnd{};
};

// Saves and restores a DC on scope exit.
class DcStateGuard {
   public:
    explicit DcStateGuard(HDC dc, bool saveState = true) : m_dc(dc), m_saved(saveState ? SaveDC(dc) : -1) {}
    ~DcStateGuard() {
        if (m_saved != -1) {
            RestoreDC(m_dc, m_saved);
        }
    }
    DcStateGuard(const DcStateGuard&) = delete;
    DcStateGuard& operator=(const DcStateGuard&) = delete;

   private:
    HDC m_dc{};
    int m_saved{-1};
};

// A selected-object guard for memory DCs.
class SelectObjectGuard {
   public:
    SelectObjectGuard(HDC dc, HGDIOBJ obj) : m_dc(dc) {
        if (dc && obj) {
            m_old = SelectObject(dc, obj);
        }
    }
    ~SelectObjectGuard() {
        if (m_dc && m_old) {
            SelectObject(m_dc, m_old);
        }
    }
    SelectObjectGuard(const SelectObjectGuard&) = delete;
    SelectObjectGuard& operator=(const SelectObjectGuard&) = delete;

   private:
    HDC m_dc{};
    HGDIOBJ m_old{};
};

// A tiny C++ exception carrying an HRESULT, so upstream-style throw/catch code
// keeps compiling.
class ResultException {
   public:
    explicit ResultException(HRESULT hr) : m_hr(hr) {}
    HRESULT code() const { return m_hr; }

   private:
    HRESULT m_hr;
};

[[noreturn]] inline void Throw(HRESULT hr) { throw ResultException(hr); }

inline int RectWidth(const RECT& r) { return r.right - r.left; }
inline int RectHeight(const RECT& r) { return r.bottom - r.top; }

// ===========================================================================
// Logging
// ===========================================================================

namespace Log {
// Writes to <mod storage>\translucent-flyouts-<process>-<pid>.log. Disabled in
// release unless TF_DEBUG_LOG is defined; kept because it is the only way to
// see what the mod does inside other processes.
#ifdef TF_DEBUG_LOG
constexpr bool kEnabled = true;
#else
constexpr bool kEnabled = false;
#endif
constexpr size_t kMaxLines = 4000;

inline bool IsTargetProcess() {
    static const bool value = []() {
        wchar_t path[MAX_PATH]{};
        GetModuleFileNameW(nullptr, path, MAX_PATH);
        const wchar_t* base = wcsrchr(path, L'\\');
        base = base ? base + 1 : path;
        return !_wcsicmp(base, L"explorer.exe") || !_wcsicmp(base, L"FlyoutTestWindow.exe") ||
               !_wcsicmp(base, L"FlyoutTestWindow32.exe") || !_wcsicmp(base, L"notepad++.exe") ||
               !_wcsicmp(base, L"regedit.exe") || !_wcsicmp(base, L"Ableton Live 12 Suite.exe");
    }();
    return value;
}

inline void Write(const wchar_t* fmt, ...) {
    if (!kEnabled || !IsTargetProcess()) {
        return;
    }
    static std::mutex lock;
    static HANDLE file = INVALID_HANDLE_VALUE;
    static int lines = 0;

    std::lock_guard<std::mutex> guard(lock);
    if (lines >= (int)kMaxLines) {
        return;
    }
    if (file == INVALID_HANDLE_VALUE) {
        wchar_t dir[MAX_PATH]{};
        if (!Wh_GetModStoragePath(dir, MAX_PATH)) {
            return;
        }
        wchar_t path[MAX_PATH]{};
        GetModuleFileNameW(nullptr, path, MAX_PATH);
        const wchar_t* base = wcsrchr(path, L'\\');
        base = base ? base + 1 : path;
        wchar_t filePath[MAX_PATH]{};
        swprintf_s(filePath, L"%s\\tf-%s-%lu.log", dir, base, GetCurrentProcessId());
        file = CreateFileW(filePath, FILE_APPEND_DATA, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr,
                           CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
        if (file == INVALID_HANDLE_VALUE) {
            return;
        }
    }
    wchar_t line[1024]{};
    va_list args;
    va_start(args, fmt);
    _vsnwprintf_s(line, _TRUNCATE, fmt, args);
    va_end(args);
    DWORD written = 0;
    WriteFile(file, line, (DWORD)(wcslen(line) * sizeof(wchar_t)), &written, nullptr);
    WriteFile(file, L"\r\n", 4, &written, nullptr);
    lines++;
}
}  // namespace Log

// ===========================================================================
// Utils (subset of Common/Utils.hpp)
// ===========================================================================

namespace Utils {

inline std::wstring GetProcessName() {
    static const std::wstring value = []() {
        wchar_t path[MAX_PATH]{};
        GetModuleFileNameW(nullptr, path, MAX_PATH);
        const wchar_t* base = wcsrchr(path, L'\\');
        return std::wstring(base ? base + 1 : path);
    }();
    return value;
}

inline bool IsWindowClass(HWND hWnd, std::wstring_view className) {
    if (!hWnd) {
        return false;
    }
    wchar_t cls[MAX_PATH + 1]{};
    if (!GetClassNameW(hWnd, cls, MAX_PATH)) {
        return false;
    }
    return !_wcsicmp(cls, std::wstring(className).c_str());
}

// Some frameworks (WinForms) register their own window classes that embed the
// system class name, e.g. "WindowsForms10.tooltips_class32.app.0.1_2_ad1".
inline bool IsWindowClassContains(HWND hWnd, std::wstring_view part) {
    if (!hWnd) {
        return false;
    }
    wchar_t cls[MAX_PATH + 1]{};
    if (!GetClassNameW(hWnd, cls, MAX_PATH)) {
        return false;
    }
    std::wstring haystack(cls);
    std::wstring needle(part);
    for (wchar_t& ch : haystack) {
        ch = static_cast<wchar_t>(towlower(ch));
    }
    for (wchar_t& ch : needle) {
        ch = static_cast<wchar_t>(towlower(ch));
    }
    return haystack.find(needle) != std::wstring::npos;
}

inline bool IsPopupMenu(HWND hWnd) {
    return hWnd && GetClassLongPtrW(hWnd, GCW_ATOM) == 32768;
}

inline HWND GetCurrentMenuOwner() {
    GUITHREADINFO guiThreadInfo{sizeof(GUITHREADINFO)};
    GetGUIThreadInfo(GetCurrentThreadId(), &guiThreadInfo);
    return guiThreadInfo.hwndMenuOwner;
}

inline bool IsRectEmptySafe(const RECT* rect) { return !rect || IsRectEmpty(rect); }

inline UCHAR PremultiplyColor(UCHAR color, UCHAR alpha = 255) {
    return static_cast<UCHAR>(color * (alpha + 1) >> 8);
}

inline COLORREF MakeCOLORREF(DWORD argb) {
    return RGB(argb >> 16 & 0xff, argb >> 8 & 0xff, argb & 0xff);
}

inline DWORD MakeArgb(UCHAR a, UCHAR r, UCHAR g, UCHAR b) {
    return (static_cast<DWORD>(a) << 24) | (static_cast<DWORD>(r) << 16) |
           (static_cast<DWORD>(g) << 8) | b;
}

inline UCHAR GetAlphaFromARGB(DWORD argb) { return static_cast<UCHAR>(argb >> 24); }

inline HBITMAP CreateDIB(LONG width = 1, LONG height = -1, UCHAR** bits = nullptr) {
    BITMAPINFO bitmapInfo{{sizeof(bitmapInfo.bmiHeader), width, height, 1, 32, BI_RGB}};
    return CreateDIBSection(nullptr, &bitmapInfo, DIB_RGB_COLORS, reinterpret_cast<PVOID*>(bits),
                            nullptr, 0);
}

// A solid brush that honors alpha channel: a 1x1 pattern brush from a 32bpp
// premultiplied DIB.
inline HBRUSH CreateSolidColorBrushWithAlpha(COLORREF color, UCHAR alpha) {
    UCHAR* bits{nullptr};
    unique_handle<HBITMAP> bitmap{CreateDIB(1, -1, &bits)};
    if (!bitmap) {
        return nullptr;
    }
    bits[0] = PremultiplyColor(GetBValue(color), alpha);
    bits[1] = PremultiplyColor(GetGValue(color), alpha);
    bits[2] = PremultiplyColor(GetRValue(color), alpha);
    bits[3] = alpha;
    return CreatePatternBrush(bitmap.get());
}

inline bool IsBadReadPtr(const void* ptr) {
    MEMORY_BASIC_INFORMATION mbi{};
    if (!VirtualQuery(ptr, &mbi, sizeof(mbi))) {
        return true;
    }
    constexpr DWORD mask{PAGE_READONLY | PAGE_READWRITE | PAGE_WRITECOPY | PAGE_EXECUTE_READ |
                         PAGE_EXECUTE_READWRITE | PAGE_EXECUTE_WRITECOPY};
    if (!(mbi.Protect & mask) || (mbi.Protect & (PAGE_GUARD | PAGE_NOACCESS))) {
        return true;
    }
    return false;
}

// Converts a bitmap to a 32bpp premultiplied-alpha DIB (upstream
// ConvertTo32BPP/Promise32BPP, used to fix menu item icons).
inline bool IsBitmapSupportAlpha(HBITMAP bitmap) {
    bool hasAlpha{false};
    BITMAPINFO bitmapInfo{sizeof(bitmapInfo.bmiHeader)};
    HDC hdc = GetDC(nullptr);
    if (!hdc) {
        return false;
    }
    unique_dc dcGuard(hdc);
    if (!bitmap || GetObjectType(bitmap) != OBJ_BITMAP ||
        GetDIBits(hdc, bitmap, 0, 0, nullptr, &bitmapInfo, DIB_RGB_COLORS) == 0) {
        return false;
    }
    bitmapInfo.bmiHeader.biCompression = BI_RGB;
    if (bitmapInfo.bmiHeader.biBitCount != 32) {
        return false;
    }
    auto pixelBits = std::make_unique<UCHAR[]>(bitmapInfo.bmiHeader.biSizeImage);
    if (GetDIBits(hdc, bitmap, 0, bitmapInfo.bmiHeader.biHeight, pixelBits.get(), &bitmapInfo,
                  DIB_RGB_COLORS) == 0) {
        return false;
    }
    for (size_t i = 0; i < bitmapInfo.bmiHeader.biSizeImage; i += 4) {
        if (pixelBits[i + 3] != 0) {
            hasAlpha = true;
            break;
        }
    }
    return hasAlpha;
}

inline HBITMAP ConvertTo32BPP(HBITMAP bitmap) {
    if (!bitmap || GetObjectType(bitmap) != OBJ_BITMAP) {
        return nullptr;
    }
    BITMAPINFO bitmapInfo{sizeof(bitmapInfo.bmiHeader)};
    HDC hdc = GetDC(nullptr);
    if (!hdc) {
        return nullptr;
    }
    unique_dc dcGuard(hdc);
    if (GetDIBits(hdc, bitmap, 0, 0, nullptr, &bitmapInfo, DIB_RGB_COLORS) == 0) {
        return nullptr;
    }
    bitmapInfo.bmiHeader.biCompression = BI_RGB;
    auto pixelBits = std::make_unique<UCHAR[]>(bitmapInfo.bmiHeader.biSizeImage);
    if (GetDIBits(hdc, bitmap, 0, bitmapInfo.bmiHeader.biHeight, pixelBits.get(), &bitmapInfo,
                  DIB_RGB_COLORS) == 0) {
        return nullptr;
    }

    UCHAR* pixelBitsWithAlpha{nullptr};
    BITMAPINFO bitmapWithAlphaInfo{{sizeof(bitmapInfo.bmiHeader), bitmapInfo.bmiHeader.biWidth,
                                    -abs(bitmapInfo.bmiHeader.biHeight), 1, 32, BI_RGB}};
    unique_handle<HBITMAP> bitmapWithAlpha{
        CreateDIBSection(nullptr, &bitmapWithAlphaInfo, DIB_RGB_COLORS,
                         reinterpret_cast<PVOID*>(&pixelBitsWithAlpha), nullptr, 0)};
    if (!bitmapWithAlpha) {
        return nullptr;
    }
    HDC memoryDC = CreateCompatibleDC(hdc);
    if (!memoryDC) {
        return nullptr;
    }
    unique_dc memoryDcGuard(memoryDC);

    size_t bitmapSize = static_cast<size_t>(bitmapInfo.bmiHeader.biWidth) *
                        static_cast<size_t>(abs(bitmapInfo.bmiHeader.biHeight)) * 4ull;
    for (size_t i = 0; i < bitmapSize; i += 4) {
        pixelBitsWithAlpha[i + 3] = 255;
    }
    {
        SelectObjectGuard select(memoryDC, bitmapWithAlpha.get());
        StretchDIBits(memoryDC, 0, 0, bitmapInfo.bmiHeader.biWidth, bitmapInfo.bmiHeader.biHeight,
                      0, 0, bitmapInfo.bmiHeader.biWidth, bitmapInfo.bmiHeader.biHeight,
                      pixelBits.get(), &bitmapInfo, DIB_RGB_COLORS, SRCPAINT);
    }
    return bitmapWithAlpha.release();
}

inline HBITMAP Promise32BPP(HBITMAP bitmap) {
    if (IsBitmapSupportAlpha(bitmap)) {
        return reinterpret_cast<HBITMAP>(
            CopyImage(bitmap, IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION | LR_DEFAULTSIZE));
    }
    return ConvertTo32BPP(bitmap);
}

// Removes a specific background color from a 32bpp bitmap (upstream
// SpriteEffect: flood fill from the corners, distance threshold).
inline HRESULT BitmapRemoveColor(HBITMAP bitmap, DWORD color, DWORD distance) {
    if (!bitmap || GetObjectType(bitmap) != OBJ_BITMAP) {
        return E_INVALIDARG;
    }
    BITMAPINFO bitmapInfo{sizeof(bitmapInfo.bmiHeader)};
    HDC hdc = GetDC(nullptr);
    if (!hdc) {
        return E_FAIL;
    }
    unique_dc dcGuard(hdc);
    if (GetDIBits(hdc, bitmap, 0, 0, nullptr, &bitmapInfo, DIB_RGB_COLORS) == 0) {
        return E_FAIL;
    }
    bitmapInfo.bmiHeader.biCompression = BI_RGB;
    if (bitmapInfo.bmiHeader.biBitCount != 32) {
        return E_NOTIMPL;
    }
    const LONG width = abs(bitmapInfo.bmiHeader.biWidth);
    const LONG height = abs(bitmapInfo.bmiHeader.biHeight);
    auto pixelBits = std::make_unique<UCHAR[]>(bitmapInfo.bmiHeader.biSizeImage);
    if (GetDIBits(hdc, bitmap, 0, bitmapInfo.bmiHeader.biHeight, pixelBits.get(), &bitmapInfo,
                  DIB_RGB_COLORS) == 0) {
        return E_FAIL;
    }

    auto map = std::make_unique<bool[]>(static_cast<size_t>(width) * height);
    auto matches = [&](LONG x, LONG y) -> bool {
        size_t index = (static_cast<size_t>(width) * y + x) * 4;
        const UCHAR* px = &pixelBits[index];
        long long db = static_cast<long long>(px[0]) - GetBValue(MakeCOLORREF(color));
        long long dg = static_cast<long long>(px[1]) - GetGValue(MakeCOLORREF(color));
        long long dr = static_cast<long long>(px[2]) - GetRValue(MakeCOLORREF(color));
        long long da = static_cast<long long>(px[3]) - GetAlphaFromARGB(color);
        return (db * db + dg * dg + dr * dr + da * da) <= static_cast<long long>(distance) * distance;
    };

    struct StackItem {
        LONG x, y;
    };
    std::vector<StackItem> stack;
    auto push = [&](LONG x, LONG y) {
        if (x < 0 || y < 0 || x >= width || y >= height) {
            return;
        }
        if (map[static_cast<size_t>(width) * y + x]) {
            return;
        }
        if (!matches(x, y)) {
            return;
        }
        map[static_cast<size_t>(width) * y + x] = true;
        stack.push_back({x, y});
    };
    push(0, 0);
    push(width - 1, 0);
    push(0, height - 1);
    push(width - 1, height - 1);

    while (!stack.empty()) {
        StackItem item = stack.back();
        stack.pop_back();
        size_t index = (static_cast<size_t>(width) * item.y + item.x) * 4;
        pixelBits[index] = 0;
        pixelBits[index + 1] = 0;
        pixelBits[index + 2] = 0;
        pixelBits[index + 3] = 0;
        push(item.x + 1, item.y);
        push(item.x - 1, item.y);
        push(item.x, item.y + 1);
        push(item.x, item.y - 1);
    }

    if (SetDIBits(hdc, bitmap, 0, bitmapInfo.bmiHeader.biHeight, pixelBits.get(), &bitmapInfo,
                  DIB_RGB_COLORS) == 0) {
        return E_FAIL;
    }
    return S_OK;
}

inline bool IsOemBitmap(HBITMAP bitmap) {
    switch (reinterpret_cast<DWORD64>(bitmap)) {
        case (DWORD64)-1:
        case 1:
        case 2:
        case 3:
        case 5:
        case 6:
        case 7:
        case 8:
        case 9:
        case 10:
        case 11:
            return true;
        default:
            return false;
    }
}

inline bool IsHighContrast() {
    HIGHCONTRASTW hc{sizeof(hc)};
    SystemParametersInfoW(SPI_GETHIGHCONTRAST, sizeof(HIGHCONTRAST), &hc, 0);
    return (hc.dwFlags & HCF_HIGHCONTRASTON) != 0;
}

}  // namespace Utils

// ===========================================================================
// SystemHelper (build number)
// ===========================================================================

namespace SystemHelper {
inline DWORD GetBuildNumber() {
    static const DWORD value = []() -> DWORD {
        HKEY hKey{};
        DWORD build = 0;
        DWORD size = sizeof(build);
        if (RegOpenKeyExW(HKEY_LOCAL_MACHINE,
                          L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", 0, KEY_QUERY_VALUE,
                          &hKey) == ERROR_SUCCESS) {
            RegQueryValueExW(hKey, L"CurrentBuildNumber", nullptr, nullptr,
                             reinterpret_cast<LPBYTE>(&build), &size);
            RegCloseKey(hKey);
        }
        return build;
    }();
    return value;
}
inline bool IsWindows11() { return GetBuildNumber() >= 22000; }
inline bool SupportsDwmBackdrop() { return GetBuildNumber() >= 22621; }
}  // namespace SystemHelper

// ===========================================================================
// ThemeHelper (subset)
// ===========================================================================

namespace ThemeHelper {
__forceinline bool ShouldAppsUseDarkMode() {
    static const auto actual = reinterpret_cast<bool(WINAPI*)()>(
        GetProcAddress(GetModuleHandleW(L"UxTheme.dll"), MAKEINTRESOURCEA(132)));
    return actual ? actual() : false;
}

__forceinline bool IsDarkModeAllowedForApp() {
    static const auto actual = reinterpret_cast<bool(WINAPI*)()>(
        GetProcAddress(GetModuleHandleW(L"UxTheme.dll"), MAKEINTRESOURCEA(139)));
    return actual ? actual() : false;
}

__forceinline HRESULT GetThemeClass(HTHEME hTheme, LPCWSTR pszClassIdList, int cchClass) {
    static const auto actual = reinterpret_cast<HRESULT(WINAPI*)(HTHEME, LPCWSTR, int)>(
        GetProcAddress(GetModuleHandleW(L"UxTheme"), MAKEINTRESOURCEA(74)));
    return actual ? actual(hTheme, pszClassIdList, cchClass) : E_FAIL;
}

inline DWORD GetThemeColorizationColor(std::wstring_view type) {
    static const auto GetImmersiveColorFromColorSetEx =
        reinterpret_cast<DWORD(WINAPI*)(DWORD, DWORD, bool, DWORD)>(
            GetProcAddress(GetModuleHandleW(L"UxTheme.dll"), MAKEINTRESOURCEA(95)));
    static const auto GetImmersiveColorTypeFromName = reinterpret_cast<DWORD(WINAPI*)(LPCWSTR)>(
        GetProcAddress(GetModuleHandleW(L"UxTheme.dll"), MAKEINTRESOURCEA(96)));
    static const auto GetImmersiveUserColorSetPreference = reinterpret_cast<DWORD(WINAPI*)(bool, bool)>(
        GetProcAddress(GetModuleHandleW(L"UxTheme.dll"), MAKEINTRESOURCEA(98)));

    DWORD argb{0};
    if (GetImmersiveColorFromColorSetEx && GetImmersiveColorTypeFromName &&
        GetImmersiveUserColorSetPreference) {
        std::wstring typeName(type);
        DWORD abgr = GetImmersiveColorFromColorSetEx(GetImmersiveUserColorSetPreference(0, 0),
                                                     GetImmersiveColorTypeFromName(typeName.c_str()),
                                                     true, 0);
        argb = Utils::MakeArgb(abgr >> 24, abgr & 0xff, (abgr >> 8) & 0xff, (abgr >> 16) & 0xff);
    } else {
        BOOL opaque{FALSE};
        DwmGetColorizationColor(&argb, &opaque);
    }
    return argb;
}

// Renders content with alpha through a buffered paint DC (upstream
// DrawThemeContent).
inline HRESULT DrawThemeContent(HDC hdc,
                                const RECT& paintRect,
                                LPCRECT clipRect,
                                LPCRECT excludeRect,
                                DWORD additionalFlags,
                                void (*callback)(HDC memoryDC, HPAINTBUFFER, RGBQUAD*, int, LPARAM),
                                LPARAM lParam) try {
    BOOL updateTarget{FALSE};
    HDC memoryDC{nullptr};
    HPAINTBUFFER bufferedPaint{nullptr};
    BLENDFUNCTION blendFunction{AC_SRC_OVER, 0, 0xFF, AC_SRC_ALPHA};
    BP_PAINTPARAMS paintParams{sizeof(BP_PAINTPARAMS), BPPF_ERASE | additionalFlags, excludeRect,
                               &blendFunction};

    DcStateGuard dcState(hdc);
    if (clipRect) {
        IntersectClipRect(hdc, clipRect->left, clipRect->top, clipRect->right, clipRect->bottom);
    }
    if (excludeRect) {
        ExcludeClipRect(hdc, excludeRect->left, excludeRect->top, excludeRect->right,
                        excludeRect->bottom);
    }

    bufferedPaint = BeginBufferedPaint(hdc, &paintRect, BPBF_TOPDOWNDIB, &paintParams, &memoryDC);
    if (!bufferedPaint) {
        return E_FAIL;
    }
    {
        SelectObjectGuard fontGuard(memoryDC, GetCurrentObject(hdc, OBJ_FONT));
        SelectObjectGuard brushGuard(memoryDC, GetCurrentObject(hdc, OBJ_BRUSH));
        SelectObjectGuard penGuard(memoryDC, GetCurrentObject(hdc, OBJ_PEN));
        int cxRow{0};
        RGBQUAD* buffer{nullptr};
        if (FAILED(GetBufferedPaintBits(bufferedPaint, &buffer, &cxRow))) {
            EndBufferedPaint(bufferedPaint, FALSE);
            return E_FAIL;
        }
        callback(memoryDC, bufferedPaint, buffer, cxRow, lParam);
    }
    updateTarget = TRUE;
    EndBufferedPaint(bufferedPaint, updateTarget);
    return S_OK;
} catch (...) {
    return E_FAIL;
}

__forceinline HRESULT DrawTextWithGlow(HDC hdc,
                                       LPCWSTR pszText,
                                       int cchText,
                                       LPRECT prc,
                                       UINT dwFlags,
                                       UINT crText,
                                       UINT crGlow,
                                       UINT nGlowRadius,
                                       UINT nGlowIntensity,
                                       BOOL bPreMultiply,
                                       DTT_CALLBACK_PROC actualDrawTextCallback,
                                       LPARAM lParam) {
    static const auto actual = reinterpret_cast<HRESULT(WINAPI*)(
        HDC, LPCWSTR, int, LPRECT, UINT, UINT, UINT, UINT, UINT, BOOL, DTT_CALLBACK_PROC, LPARAM)>(
        GetProcAddress(GetModuleHandleW(L"UxTheme"), MAKEINTRESOURCEA(126)));
    if (actual) {
        return actual(hdc, pszText, cchText, prc, dwFlags, crText, crGlow, nGlowRadius,
                      nGlowIntensity, bPreMultiply, actualDrawTextCallback, lParam);
    }
    return E_FAIL;
}
}  // namespace ThemeHelper

// ===========================================================================
// EffectHelper (backdrop effects, upstream EffectHelper.hpp)
// ===========================================================================

namespace EffectHelper {
enum class EffectType {
    None,
    Transparent,
    Solid,
    Blur,
    AcrylicBlur,
    // Windows 11
    ModernAcrylicBlur,
    Acrylic,
    Mica,
    MicaAlt,
    Max,
};

enum class WINDOWCOMPOSITIONATTRIBUTE : DWORD {
    WCA_ACCENT_POLICY = 19,
};

enum class ACCENT_STATE : DWORD {
    ACCENT_DISABLED,
    ACCENT_ENABLE_GRADIENT,
    ACCENT_ENABLE_TRANSPARENTGRADIENT,
    ACCENT_ENABLE_BLURBEHIND,
    ACCENT_ENABLE_ACRYLICBLURBEHIND,
    ACCENT_ENABLE_HOSTBACKDROP,
    ACCENT_INVALID_STATE,
};

enum ACCENT_FLAG : DWORD {
    ACCENT_NONE = 0,
    ACCENT_ENABLE_MODERN_ACRYLIC_RECIPE = 1 << 1,
    ACCENT_ENABLE_GRADIENT_COLOR = 1 << 1,
    ACCENT_ENABLE_BORDER_LEFT = 1 << 5,
    ACCENT_ENABLE_BORDER_TOP = 1 << 6,
    ACCENT_ENABLE_BORDER_RIGHT = 1 << 7,
    ACCENT_ENABLE_BORDER_BOTTOM = 1 << 8,
    ACCENT_ENABLE_BORDER = ACCENT_ENABLE_BORDER_LEFT | ACCENT_ENABLE_BORDER_TOP |
                          ACCENT_ENABLE_BORDER_RIGHT | ACCENT_ENABLE_BORDER_BOTTOM,
};

struct ACCENT_POLICY {
    DWORD AccentState;
    DWORD AccentFlags;
    DWORD dwGradientColor;
    DWORD dwAnimationId;
};

struct WINDOWCOMPOSITIONATTRIBUTEDATA {
    DWORD dwAttribute;
    PVOID pvData;
    SIZE_T cbData;
};

inline BOOL WINAPI SetWindowCompositionAttribute(HWND hWnd,
                                                 WINDOWCOMPOSITIONATTRIBUTEDATA* data) {
    static const auto actual = reinterpret_cast<BOOL(WINAPI*)(HWND, WINDOWCOMPOSITIONATTRIBUTEDATA*)>(
        GetProcAddress(GetModuleHandleW(L"user32.dll"), "SetWindowCompositionAttribute"));
    return actual ? actual(hWnd, data) : FALSE;
}

inline void DwmMakeWindowTransparent(HWND hwnd, BOOL enable) {
    DWM_BLURBEHIND bb{DWM_BB_ENABLE | static_cast<DWORD>(enable ? (DWM_BB_BLURREGION | DWM_BB_TRANSITIONONMAXIMIZED) : 0),
                      enable, CreateRectRgn(0, 0, -1, -1), TRUE};
    DwmEnableBlurBehindWindow(hwnd, &bb);
    if (bb.hRgnBlur) {
        DeleteObject(bb.hRgnBlur);
    }
}

__forceinline void EnableWindowDarkMode(HWND hwnd, BOOL darkMode) {
    if (!SystemHelper::IsWindows11()) {
        constexpr DWORD kDwmwaUseImmersiveDarkModeWin10{19};
        DwmSetWindowAttribute(hwnd, kDwmwaUseImmersiveDarkModeWin10, &darkMode, sizeof(darkMode));
    } else {
        DwmSetWindowAttribute(hwnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &darkMode, sizeof(darkMode));
    }
}

inline void TriggerWindowNCRendering(HWND hwnd) {
    DefWindowProcW(hwnd, WM_NCACTIVATE, TRUE, 0);
    SetWindowPos(hwnd, nullptr, 0, 0, 0, 0,
                 SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_DRAWFRAME |
                     SWP_NOACTIVATE);
}

// Applies a backdrop effect to a flyout window (port of
// EffectHelper::SetWindowBackdrop).
inline void SetWindowBackdrop(HWND hwnd, BOOL dropShadow, DWORD tintColor, DWORD effectType) {
    ACCENT_POLICY accentPolicy{static_cast<DWORD>(ACCENT_STATE::ACCENT_DISABLED),
                               dropShadow ? static_cast<DWORD>(ACCENT_ENABLE_BORDER)
                                          : static_cast<DWORD>(ACCENT_NONE),
                               tintColor, 0};
    WINDOWCOMPOSITIONATTRIBUTEDATA data{static_cast<DWORD>(WINDOWCOMPOSITIONATTRIBUTE::WCA_ACCENT_POLICY),
                                        &accentPolicy, sizeof(ACCENT_POLICY)};
    DWM_SYSTEMBACKDROP_TYPE backdropType{DWMSBT_NONE};

    BOOL mica{FALSE};
    BOOL ncRendering{FALSE};
    BOOL windowTransparent{FALSE};

    switch (static_cast<EffectType>(effectType)) {
        case EffectType::None:
            break;
        case EffectType::Solid:
            accentPolicy.AccentState = static_cast<DWORD>(ACCENT_STATE::ACCENT_ENABLE_GRADIENT);
            accentPolicy.AccentFlags |= ACCENT_ENABLE_GRADIENT_COLOR;
            break;
        case EffectType::Transparent:
            accentPolicy.AccentState =
                static_cast<DWORD>(ACCENT_STATE::ACCENT_ENABLE_TRANSPARENTGRADIENT);
            accentPolicy.AccentFlags |= ACCENT_ENABLE_GRADIENT_COLOR;
            break;
        case EffectType::Blur:
            accentPolicy.AccentState =
                static_cast<DWORD>(ACCENT_STATE::ACCENT_ENABLE_BLURBEHIND);
            accentPolicy.AccentFlags |= ACCENT_ENABLE_GRADIENT_COLOR;
            break;
        case EffectType::AcrylicBlur:
            accentPolicy.AccentState =
                static_cast<DWORD>(ACCENT_STATE::ACCENT_ENABLE_ACRYLICBLURBEHIND);
            break;
        case EffectType::ModernAcrylicBlur:
            accentPolicy.AccentState =
                static_cast<DWORD>(ACCENT_STATE::ACCENT_ENABLE_ACRYLICBLURBEHIND);
            accentPolicy.AccentFlags |= ACCENT_ENABLE_MODERN_ACRYLIC_RECIPE;
            break;
        case EffectType::Acrylic:
            backdropType = DWMSBT_TRANSIENTWINDOW;
            ncRendering = TRUE;
            windowTransparent = TRUE;
            break;
        case EffectType::Mica:
            mica = TRUE;
            backdropType = DWMSBT_MAINWINDOW;
            ncRendering = TRUE;
            windowTransparent = TRUE;
            break;
        case EffectType::MicaAlt:
            backdropType = DWMSBT_TABBEDWINDOW;
            ncRendering = TRUE;
            windowTransparent = TRUE;
            break;
        default:
            break;
    }

    if (SystemHelper::GetBuildNumber() > 22000 && SystemHelper::GetBuildNumber() < 22621) {
        DwmSetWindowAttribute(hwnd, 1029, &mica, sizeof(mica));
    }
    if (SystemHelper::GetBuildNumber() >= 22621) {
        DwmSetWindowAttribute(hwnd, DWMWA_SYSTEMBACKDROP_TYPE, &backdropType,
                              sizeof(DWM_SYSTEMBACKDROP_TYPE));
    }
    // DwmEnableBlurBehindWindow can deadlock when called synchronously from
    // inside a flyout's paint (seen on tooltips). Only call it when the effect
    // actually needs the window made transparent; there is nothing to clear on
    // a freshly seen window otherwise.
    if (windowTransparent) {
        DwmMakeWindowTransparent(hwnd, TRUE);
    }
    SetWindowCompositionAttribute(hwnd, &data);
    if (ncRendering) {
        TriggerWindowNCRendering(hwnd);
    }
}
}  // namespace EffectHelper

}  // namespace tf

// ===========================================================================
// Configuration: Windhawk settings -> engine values
//
// The settings schema and semantics follow the TranslucentFlyouts controller
// mod (GID0317): four categories (global / dropDown / menu / tooltip) where a
// category value of "use_global" (a sentinel index) inherits the global value.
// ===========================================================================

namespace tf {

namespace Cfg {

constexpr int kUseGlobalEffect = 9;   // dropDown/menu/tooltip effectType
constexpr int kUseGlobalCorner = 4;   // dropDown/menu/tooltip cornerType
constexpr int kUseGlobalBool = 2;     // no/yes/use_global dropdowns

struct Category {
    int disabledMode{0};
    int effectType{5};
    int cornerType{3};
    int enableDropShadowMode{0};
    int noBorderColorMode{0};
    int enableThemeColorizationMode{0};
    int darkThemeColorizationType{1};
    int lightThemeColorizationType{1};
    DWORD darkBorderColor{0xFF2B2B2B};
    DWORD lightBorderColor{0xFFDDDDDD};
    DWORD darkGradientColor{0x412B2B2B};
    DWORD lightGradientColor{0x9EDDDDDD};
};

struct MenuExtras {
    bool noSystemDropShadow{false};
    bool enableImmersiveStyle{true};
    bool enableCustomRendering{false};
    bool enableCompatibilityMode{false};
    bool noModernAppBackgroundColor{true};
    bool colorTreatAsTransparentEnabled{false};
    DWORD colorTreatAsTransparent{0};
    DWORD colorTreatAsTransparentThreshold{50};

    bool separator_disabled{false};
    DWORD separator_width{1000};
    DWORD separator_darkModeColor{0x30D9D9D9};
    DWORD separator_lightModeColor{0x30262626};
    bool separator_enableThemeColorization{false};
    int separator_darkThemeColorizationType{1};
    int separator_lightThemeColorizationType{1};

    bool focusing_disabled{false};
    DWORD focusing_cornerRadius{8};
    DWORD focusing_width{1000};
    DWORD focusing_darkModeColor{0xFFFFFFFF};
    DWORD focusing_lightModeColor{0xFF000000};
    bool focusing_enableThemeColorization{false};
    int focusing_darkThemeColorizationType{1};
    int focusing_lightThemeColorizationType{1};

    bool disabledHot_disabled{false};
    DWORD disabledHot_cornerRadius{8};
    DWORD disabledHot_darkModeColor{0x00000000};
    DWORD disabledHot_lightModeColor{0x00000000};
    bool disabledHot_enableThemeColorization{false};
    int disabledHot_darkThemeColorizationType{1};
    int disabledHot_lightThemeColorizationType{1};

    bool hot_disabled{false};
    DWORD hot_cornerRadius{8};
    DWORD hot_darkModeColor{0x41808080};
    DWORD hot_lightModeColor{0x30000000};
    bool hot_enableThemeColorization{false};
    int hot_darkThemeColorizationType{1};
    int hot_lightThemeColorizationType{1};

    bool enableFluentAnimation{false};
    DWORD fadeOutTime{350};
    DWORD popInTime{250};
    DWORD fadeInTime{87};
    DWORD popInStyle{0};
    DWORD startRatio{50};
    bool immediateInterupting{false};
};

struct TooltipExtras {
    bool noSystemDropShadow{false};
    DWORD darkModeColor{0xFFFFFFFF};
    DWORD lightModeColor{0xFF1A1A1A};
    DWORD marginsType{0};
    MARGINS margins{6, 6, 6, 6};
};

struct State {
    bool loaded{false};
    Category global;
    Category dropDown;
    Category menu;
    Category tooltip;
    MenuExtras menuExtras;
    TooltipExtras tooltipExtras;
    std::vector<std::wstring> processBlockList;
    std::vector<std::wstring> processDisabledList;
    std::vector<std::wstring> menuProcessDisabledList;
    std::vector<std::wstring> tooltipProcessDisabledList;
    std::vector<std::wstring> dropDownProcessDisabledList;
};

inline State& Get() {
    static State state;
    return state;
}

// ------------------------------------------------------------- raw values ---
//
// Settings are read straight from the engine's settings registry key.
// Windhawk stores every setting as a value under
// HKLM\SOFTWARE\Windhawk\Engine\Mods\<mod-id>\Settings, with dotted names for
// nested settings ("menu.effectType"). Values are REG_DWORD for numbers and
// checkboxes, and REG_SZ for option settings (the option key, e.g.
// "modern_acrylic"). Wh_GetIntSetting cannot be used for this: it returns 0
// both for "the user chose 0" and "the value is not set", so code defaults
// would be indistinguishable from a real zero. Reading the registry directly
// keeps "not set -> code default" exact.

inline std::wstring ToLower(std::wstring text) {
    for (wchar_t& ch : text) {
        ch = static_cast<wchar_t>(towlower(ch));
    }
    return text;
}

inline std::unordered_map<std::wstring, std::wstring>& Values() {
    static std::unordered_map<std::wstring, std::wstring> values;
    return values;
}

inline void ReloadValues() {
    auto& values = Values();
    values.clear();

    HKEY hKey{};
    if (RegOpenKeyExW(HKEY_LOCAL_MACHINE,
                      L"SOFTWARE\\Windhawk\\Engine\\Mods\\" WH_MOD_ID L"\\Settings", 0,
                      KEY_QUERY_VALUE | KEY_WOW64_64KEY, &hKey) != ERROR_SUCCESS) {
        return;
    }
    DWORD index = 0;
    for (;;) {
        wchar_t name[512]{};
        DWORD nameSize = _countof(name);
        BYTE data[1024]{};
        DWORD dataSize = sizeof(data);
        DWORD type = 0;
        LONG rc = RegEnumValueW(hKey, index++, name, &nameSize, nullptr, &type, data, &dataSize);
        if (rc == ERROR_NO_MORE_ITEMS) {
            break;
        }
        if (rc != ERROR_SUCCESS) {
            continue;
        }
        if (type == REG_DWORD && dataSize >= sizeof(DWORD)) {
            wchar_t text[32]{};
            swprintf_s(text, L"%u", *reinterpret_cast<DWORD*>(data));
            values[ToLower(name)] = text;
        } else if (type == REG_SZ && dataSize >= sizeof(wchar_t)) {
            values[ToLower(name)] =
                std::wstring(reinterpret_cast<wchar_t*>(data),
                             wcsnlen(reinterpret_cast<wchar_t*>(data),
                                     dataSize / sizeof(wchar_t)));
        }
    }
    RegCloseKey(hKey);
}

// Registry value names are matched case-insensitively (the map is keyed by
// lowercase name).
inline bool TryGetValue(PCWSTR name, std::wstring& result) {
    auto& values = Values();
    auto it = values.find(ToLower(name));
    if (it == values.end()) {
        return false;
    }
    result = it->second;
    return true;
}

inline bool ParseIntText(const std::wstring& text, int& result) {
    if (text.empty()) {
        return false;
    }
    wchar_t* end{nullptr};
    long value = wcstol(text.c_str(), &end, 10);
    if (end == text.c_str() || *end != 0) {
        return false;
    }
    result = static_cast<int>(value);
    return true;
}

inline int GetInt(const wchar_t* name, int fallback) {
    std::wstring value;
    int result{};
    if (!TryGetValue(name, value) || !ParseIntText(value, result)) {
        return fallback;
    }
    return result;
}

// ---------------------------------------------------------------- parsing ---

inline DWORD ParseColorSetting(PCWSTR value, DWORD fallback) {
    if (!value || !*value) {
        return fallback;
    }
    PCWSTR p = value;
    while (*p == L' ' || *p == L'\t') {
        p++;
    }
    if ((p[0] == L'0') && (p[1] == L'x' || p[1] == L'X')) {
        p += 2;
    }
    wchar_t* end{nullptr};
    unsigned long long parsed = wcstoull(p, &end, 16);
    if (end == p) {
        return fallback;
    }
    return static_cast<DWORD>(parsed);
}

inline std::vector<std::wstring> ParseProcessList(PCWSTR value) {
    std::vector<std::wstring> result;
    if (!value) {
        return result;
    }
    std::wstring current;
    for (PCWSTR p = value; *p; p++) {
        if (*p == L',' || *p == L';' || *p == L'\n' || *p == L'\r') {
            if (!current.empty()) {
                result.push_back(current);
                current.clear();
            }
        } else if (*p != L' ' && *p != L'\t') {
            current.push_back(*p);
        }
    }
    if (!current.empty()) {
        result.push_back(current);
    }
    return result;
}

inline bool WildcardMatch(PCWSTR pattern, PCWSTR text) {
    if (!pattern || !text) {
        return false;
    }
    PCWSTR p = pattern;
    PCWSTR t = text;
    PCWSTR starP{nullptr};
    PCWSTR starT{nullptr};
    while (*t) {
        if (*p == L'*') {
            starP = p++;
            starT = t;
        } else if (towlower(*p) == towlower(*t)) {
            p++;
            t++;
        } else if (starP) {
            p = starP + 1;
            t = ++starT;
        } else {
            return false;
        }
    }
    while (*p == L'*') {
        p++;
    }
    return *p == 0;
}

inline bool MatchesProcessList(const std::vector<std::wstring>& list, const std::wstring& name) {
    for (const auto& entry : list) {
        if (WildcardMatch(entry.c_str(), name.c_str())) {
            return true;
        }
    }
    return false;
}

// --------------------------------------------------------------- loading ---

// Option key -> internal index tables (identical to the controller's mapping).
struct SettingChoice {
    const wchar_t* key;
    int value;
};

inline constexpr SettingChoice kEffectGlobal[] = {
    {L"none", 0},        {L"transparent", 1},  {L"solid", 2},       {L"blurred", 3},
    {L"acrylic", 4},     {L"modern_acrylic", 5}, {L"acrylic_bg", 6}, {L"mica_bg", 7},
    {L"mica_variant", 8},
};

inline constexpr SettingChoice kEffectPart[] = {
    {L"none", 0},        {L"transparent", 1},  {L"solid", 2},        {L"blurred", 3},
    {L"acrylic", 4},     {L"modern_acrylic", 5}, {L"acrylic_bg", 6}, {L"mica_bg", 7},
    {L"mica_variant", 8}, {L"use_global", 9},
};

inline constexpr SettingChoice kCornerGlobal[] = {
    {L"dont_change", 0}, {L"sharp", 1}, {L"large_round", 2}, {L"small_round", 3},
};

inline constexpr SettingChoice kCornerPart[] = {
    {L"dont_change", 0}, {L"sharp", 1},          {L"large_round", 2},
    {L"small_round", 3}, {L"use_global", 4},
};

inline constexpr SettingChoice kTriState[] = {
    {L"no", 0}, {L"yes", 1}, {L"use_global", 2},
};

// Options are textual in the controller schema, but older exports stored the
// numeric index; both are accepted.
inline constexpr SettingChoice kColorizationType[] = {
    {L"start_background", 0}, {L"start_hover", 1},   {L"system_accent", 2},
    {L"accent_dark1", 3},     {L"accent_dark2", 4},  {L"accent_dark3", 5},
    {L"accent_light1", 6},    {L"accent_light2", 7}, {L"accent_light3", 8},
};

inline constexpr SettingChoice kPopInStyle[] = {
    {L"slide_down", 0}, {L"ripple", 1}, {L"smooth_scroll", 2}, {L"smooth_zoom", 3},
};

inline constexpr SettingChoice kMarginsType[] = {
    {L"add_to_existing", 0}, {L"replace_existing", 1},
};

inline int GetBool(const wchar_t* name, int fallback) {
    std::wstring value;
    if (!TryGetValue(name, value) || value.empty()) {
        return fallback;
    }
    if (!_wcsicmp(value.c_str(), L"yes") || !_wcsicmp(value.c_str(), L"true") ||
        !_wcsicmp(value.c_str(), L"on") || value == L"1") {
        return 1;
    }
    if (!_wcsicmp(value.c_str(), L"no") || !_wcsicmp(value.c_str(), L"false") ||
        !_wcsicmp(value.c_str(), L"off") || value == L"0") {
        return 0;
    }
    return fallback;
}

inline int GetMappedInt(const wchar_t* name, const SettingChoice* choices, size_t count,
                        int fallback) {
    std::wstring value;
    if (!TryGetValue(name, value) || value.empty()) {
        return fallback;
    }
    for (size_t i = 0; i < count; i++) {
        if (!_wcsicmp(value.c_str(), choices[i].key)) {
            return choices[i].value;
        }
    }
    int numeric{};
    return ParseIntText(value, numeric) ? numeric : fallback;
}

inline DWORD GetColor(const wchar_t* name, DWORD fallback) {
    std::wstring value;
    if (!TryGetValue(name, value) || value.empty()) {
        return fallback;
    }
    return ParseColorSetting(value.c_str(), fallback);
}

inline std::wstring GetString(const wchar_t* name) {
    std::wstring value;
    TryGetValue(name, value);
    return value;
}

inline int GetColorizationType(const wchar_t* name, int fallback) {
    return GetMappedInt(name, kColorizationType, _countof(kColorizationType), fallback);
}

inline void ReadCategory(PCWSTR prefix, Category& category, bool isGlobal) {
    wchar_t key[160]{};
    auto make = [&](const wchar_t* name) -> const wchar_t* {
        swprintf_s(key, L"%s.%s", prefix, name);
        return key;
    };

    category.effectType =
        GetMappedInt(make(L"effectType"), isGlobal ? kEffectGlobal : kEffectPart,
                     isGlobal ? _countof(kEffectGlobal) : _countof(kEffectPart),
                     isGlobal ? 5 : kUseGlobalEffect);
    category.cornerType =
        GetMappedInt(make(L"cornerType"), isGlobal ? kCornerGlobal : kCornerPart,
                     isGlobal ? _countof(kCornerGlobal) : _countof(kCornerPart),
                     isGlobal ? 3 : kUseGlobalCorner);
    category.enableDropShadowMode =
        GetBool(make(L"enableDropShadow"), isGlobal ? 0 : kUseGlobalBool);
    category.noBorderColorMode = GetBool(make(L"noBorderColor"), isGlobal ? 0 : kUseGlobalBool);
    category.enableThemeColorizationMode =
        GetBool(make(L"enableThemeColorization"), isGlobal ? 0 : kUseGlobalBool);
    category.darkThemeColorizationType = GetColorizationType(make(L"darkModeThemeColorizationType"), 1);
    category.lightThemeColorizationType = GetColorizationType(make(L"lightModeThemeColorizationType"), 1);
    category.darkBorderColor = GetColor(make(L"darkModeBorderColor"), 0xFF2B2B2B);
    category.lightBorderColor = GetColor(make(L"lightModeBorderColor"), 0xFFDDDDDD);
    category.darkGradientColor = GetColor(make(L"darkModeGradientColor"), 0x412B2B2B);
    category.lightGradientColor = GetColor(make(L"lightModeGradientColor"), 0x9EDDDDDD);
    category.disabledMode = GetBool(make(L"disabled"), isGlobal ? 0 : kUseGlobalBool);
}

inline void Load() {
    static std::mutex loadLock;
    std::lock_guard<std::mutex> guard(loadLock);

    State& state = Get();
    ReloadValues();

    ReadCategory(L"global", state.global, true);
    ReadCategory(L"dropDown", state.dropDown, false);
    ReadCategory(L"menu", state.menu, false);
    ReadCategory(L"tooltip", state.tooltip, false);

    MenuExtras& ex = state.menuExtras;
    ex.noSystemDropShadow = GetBool(L"menu.noSystemDropShadow", 0) != 0;
    ex.enableImmersiveStyle = GetBool(L"menu.enableImmersiveStyle", 1) != 0;
    ex.enableCustomRendering = GetBool(L"menu.enableCustomRendering", 0) != 0;
    ex.enableCompatibilityMode = GetBool(L"menu.enableCompatibilityMode", 0) != 0;
    ex.noModernAppBackgroundColor = GetBool(L"menu.noModernAppBackgroundColor", 1) != 0;
    ex.colorTreatAsTransparentEnabled =
        GetBool(L"menu.colorTreatAsTransparentEnabled", 0) != 0;
    ex.colorTreatAsTransparent = GetColor(L"menu.colorTreatAsTransparent", 0x00000000);
    ex.colorTreatAsTransparentThreshold =
        (DWORD)GetInt(L"menu.colorTreatAsTransparentThreshold", 50);

    ex.separator_disabled = GetBool(L"menu.separator_disabled", 0) != 0;
    ex.separator_width = (DWORD)GetInt(L"menu.separator_width", 1000);
    ex.separator_darkModeColor = GetColor(L"menu.separator_darkModeColor", 0x30D9D9D9);
    ex.separator_lightModeColor = GetColor(L"menu.separator_lightModeColor", 0x30262626);
    ex.separator_enableThemeColorization =
        GetBool(L"menu.separator_enableThemeColorization", 0) != 0;
    ex.separator_darkThemeColorizationType =
        GetColorizationType(L"menu.separator_darkThemeColorizationType", 1);
    ex.separator_lightThemeColorizationType =
        GetColorizationType(L"menu.separator_lightThemeColorizationType", 1);

    ex.focusing_disabled = GetBool(L"menu.focusing_disabled", 0) != 0;
    ex.focusing_cornerRadius = (DWORD)GetInt(L"menu.focusing_cornerRadius", 8);
    ex.focusing_width = (DWORD)GetInt(L"menu.focusing_width", 1000);
    ex.focusing_darkModeColor = GetColor(L"menu.focusing_darkModeColor", 0xFFFFFFFF);
    ex.focusing_lightModeColor = GetColor(L"menu.focusing_lightModeColor", 0xFF000000);
    ex.focusing_enableThemeColorization =
        GetBool(L"menu.focusing_enableThemeColorization", 0) != 0;
    ex.focusing_darkThemeColorizationType =
        GetColorizationType(L"menu.focusing_darkThemeColorizationType", 1);
    ex.focusing_lightThemeColorizationType =
        GetColorizationType(L"menu.focusing_lightThemeColorizationType", 1);

    ex.disabledHot_disabled = GetBool(L"menu.disabledHot_disabled", 0) != 0;
    ex.disabledHot_cornerRadius = (DWORD)GetInt(L"menu.disabledHot_cornerRadius", 8);
    ex.disabledHot_darkModeColor = GetColor(L"menu.disabledHot_darkModeColor", 0x00000000);
    ex.disabledHot_lightModeColor = GetColor(L"menu.disabledHot_lightModeColor", 0x00000000);
    ex.disabledHot_enableThemeColorization =
        GetBool(L"menu.disabledHot_enableThemeColorization", 0) != 0;
    ex.disabledHot_darkThemeColorizationType =
        GetColorizationType(L"menu.disabledHot_darkThemeColorizationType", 1);
    ex.disabledHot_lightThemeColorizationType =
        GetColorizationType(L"menu.disabledHot_lightThemeColorizationType", 1);

    ex.hot_disabled = GetBool(L"menu.hot_disabled", 0) != 0;
    ex.hot_cornerRadius = (DWORD)GetInt(L"menu.hot_cornerRadius", 8);
    ex.hot_darkModeColor = GetColor(L"menu.hot_darkModeColor", 0x41808080);
    ex.hot_lightModeColor = GetColor(L"menu.hot_lightModeColor", 0x30000000);
    ex.hot_enableThemeColorization =
        GetBool(L"menu.hot_enableThemeColorization", 0) != 0;
    ex.hot_darkThemeColorizationType =
        GetColorizationType(L"menu.hot_darkThemeColorizationType", 1);
    ex.hot_lightThemeColorizationType =
        GetColorizationType(L"menu.hot_lightThemeColorizationType", 1);

    ex.enableFluentAnimation = GetBool(L"menu.enableFluentAnimation", 0) != 0;
    ex.fadeOutTime = (DWORD)GetInt(L"menu.animation_fadeOutTime", 350);
    ex.popInTime = (DWORD)GetInt(L"menu.animation_popInTime", 250);
    ex.fadeInTime = (DWORD)GetInt(L"menu.animation_fadeInTime", 87);
    ex.popInStyle = (DWORD)GetMappedInt(L"menu.animation_popInStyle", kPopInStyle,
                                        _countof(kPopInStyle), 0);
    ex.startRatio = (DWORD)GetInt(L"menu.animation_startRatio", 50);
    ex.immediateInterupting =
        GetBool(L"menu.animation_enableImmediateInterupting", 0) != 0;

    TooltipExtras& tip = state.tooltipExtras;
    tip.noSystemDropShadow = GetBool(L"tooltip.noSystemDropShadow", 0) != 0;
    tip.darkModeColor = GetColor(L"tooltip.darkModeColor", 0xFFFFFFFF);
    tip.lightModeColor = GetColor(L"tooltip.lightModeColor", 0xFF1A1A1A);
    tip.marginsType = (DWORD)GetMappedInt(L"tooltip.marginsType", kMarginsType,
                                          _countof(kMarginsType), 0);
    tip.margins.cxLeftWidth = (LONG)GetInt(L"tooltip.marginLeft", 6);
    tip.margins.cxRightWidth = (LONG)GetInt(L"tooltip.marginRight", 6);
    tip.margins.cyTopHeight = (LONG)GetInt(L"tooltip.marginTop", 6);
    tip.margins.cyBottomHeight = (LONG)GetInt(L"tooltip.marginBottom", 6);

    auto readList = [](const wchar_t* name) { return ParseProcessList(GetString(name).c_str()); };
    state.processBlockList = readList(L"advancedFunctions.processBlockList");
    state.processDisabledList = readList(L"advancedFunctions.processDisabledList");
    state.menuProcessDisabledList = readList(L"advancedFunctions.menuProcessDisabledList");
    state.tooltipProcessDisabledList = readList(L"advancedFunctions.tooltipProcessDisabledList");
    state.dropDownProcessDisabledList = readList(L"advancedFunctions.dropDownProcessDisabledList");

    state.loaded = true;
}

}  // namespace Cfg

// ===========================================================================
// Api: the queries the engine uses (port of TFMain/ApiEx.cpp + Api.cpp)
// ===========================================================================

namespace Api {

struct WindowBackdropEffectContext {
    DWORD effectType;
    DWORD enableDropShadow;
    DWORD gradientColor;
};

struct BorderContext {
    bool colorUseNone;
    bool colorUseDefault;
    DWORD color;
    DWM_WINDOW_CORNER_PREFERENCE cornerType;
};

struct MenuCustomRenderingContext {
    bool enable;

    bool separator_disabled;
    DWORD separator_cornerRadius;
    DWORD separator_color;
    DWORD separator_width;

    bool focusing_disabled;
    DWORD focusing_cornerRadius;
    DWORD focusing_color;
    DWORD focusing_width;

    bool disabledHot_disabled;
    DWORD disabledHot_cornerRadius;
    DWORD disabledHot_color;

    bool hot_disabled;
    DWORD hot_cornerRadius;
    DWORD hot_color;
};

struct MenuIconBackgroundColorRemovalContext {
    bool enable;
    DWORD colorTreatAsTransparent;
    DWORD colorTreatAsTransparentThreshold;
};

struct FlyoutAnimationContext {
    bool enable;
    DWORD fadeOutTime;
    bool immediateInterupting;
    DWORD startRatio;
    DWORD popInTime;
    DWORD fadeInTime;
    DWORD popInStyle;
};

struct TooltipRenderingContext {
    COLORREF color;
    DWORD marginsType;
    MARGINS margins;
};

inline bool IsGlobalPart(std::wstring_view part) { return part.empty(); }

inline const Cfg::Category& GlobalCategory() { return Cfg::Get().global; }

// Maps a part name ("Menu" / "DropDown" / "Tooltip" / "") to its category.
inline Cfg::Category& CategoryForPart(std::wstring_view part) {
    Cfg::State& state = Cfg::Get();
    if (!_wcsicmp(std::wstring(part).c_str(), L"Menu")) {
        return state.menu;
    }
    if (!_wcsicmp(std::wstring(part).c_str(), L"DropDown")) {
        return state.dropDown;
    }
    if (!_wcsicmp(std::wstring(part).c_str(), L"Tooltip")) {
        return state.tooltip;
    }
    return state.global;
}

inline int ResolveInt(std::wstring_view part, int categoryValue, int sentinel, int globalValue) {
    if (IsGlobalPart(part) || categoryValue == sentinel) {
        return globalValue;
    }
    return categoryValue;
}

inline bool ResolveBoolMode(std::wstring_view part, int categoryMode, bool globalValue) {
    if (IsGlobalPart(part) || categoryMode == Cfg::kUseGlobalBool) {
        return globalValue;
    }
    return categoryMode != 0;
}

inline const wchar_t* ThemeColorizationTypeToName(int index) {
    static const wchar_t* kTypes[] = {
        L"ImmersiveStartBackground",       L"ImmersiveStartHoverBackground",
        L"ImmersiveSystemAccent",          L"ImmersiveSystemAccentDark1",
        L"ImmersiveSystemAccentDark2",     L"ImmersiveSystemAccentDark3",
        L"ImmersiveSystemAccentLight1",    L"ImmersiveSystemAccentLight2",
        L"ImmersiveSystemAccentLight3",
    };
    if (index < 0 || index > 8) {
        return kTypes[1];
    }
    return kTypes[index];
}

inline bool IsPartDisabled(std::wstring_view part) {
    Cfg::State& state = Cfg::Get();
    const Cfg::Category& global = state.global;
    const std::wstring processName = Utils::GetProcessName();

    bool globalDisabled = global.disabledMode != 0;
    globalDisabled = globalDisabled || Cfg::MatchesProcessList(state.processDisabledList, processName);
    if (globalDisabled) {
        return true;
    }

    const Cfg::Category& category = CategoryForPart(part);
    bool partDisabled = ResolveBoolMode(part, category.disabledMode, false);
    if (!IsGlobalPart(part)) {
        if (!_wcsicmp(std::wstring(part).c_str(), L"Menu")) {
            partDisabled =
                partDisabled || Cfg::MatchesProcessList(state.menuProcessDisabledList, processName);
        } else if (!_wcsicmp(std::wstring(part).c_str(), L"Tooltip")) {
            partDisabled = partDisabled ||
                           Cfg::MatchesProcessList(state.tooltipProcessDisabledList, processName);
        } else if (!_wcsicmp(std::wstring(part).c_str(), L"DropDown")) {
            partDisabled = partDisabled ||
                           Cfg::MatchesProcessList(state.dropDownProcessDisabledList, processName);
        }
    }
    return partDisabled;
}

inline bool IsCurrentProcessInBlockList() {
    Cfg::State& state = Cfg::Get();
    return Cfg::MatchesProcessList(state.processBlockList, Utils::GetProcessName());
}

// ------------------------------------------------------------------ query ---

inline void QueryBackdropEffectContext(std::wstring_view part, bool darkMode,
                                       WindowBackdropEffectContext& context) {
    RtlSecureZeroMemory(&context, sizeof(context));
    Cfg::State& state = Cfg::Get();
    Cfg::Category& category = CategoryForPart(part);
    const Cfg::Category& global = state.global;

    context.effectType = static_cast<DWORD>(
        ResolveInt(part, category.effectType, Cfg::kUseGlobalEffect, global.effectType));
    context.enableDropShadow =
        ResolveBoolMode(part, category.enableDropShadowMode, global.enableDropShadowMode != 0)
            ? 1
            : 0;
    context.gradientColor = darkMode ? category.darkGradientColor : category.lightGradientColor;
}

inline void QueryBorderContext(std::wstring_view part, bool darkMode, BorderContext& context) {
    RtlSecureZeroMemory(&context, sizeof(context));
    Cfg::State& state = Cfg::Get();
    Cfg::Category& category = CategoryForPart(part);
    const Cfg::Category& global = state.global;

    context.colorUseDefault = true;
    context.colorUseNone =
        ResolveBoolMode(part, category.noBorderColorMode, global.noBorderColorMode != 0);
    if (!context.colorUseNone) {
        bool enableColorization = ResolveBoolMode(
            part, category.enableThemeColorizationMode, global.enableThemeColorizationMode != 0);
        if (enableColorization) {
            int typeIndex = darkMode ? category.darkThemeColorizationType
                                     : category.lightThemeColorizationType;
            context.color = ThemeHelper::GetThemeColorizationColor(
                ThemeColorizationTypeToName(typeIndex));
            context.colorUseDefault = false;
        } else {
            context.color = darkMode ? category.darkBorderColor : category.lightBorderColor;
            context.colorUseDefault = false;
        }
    } else {
        context.color = DWMWA_COLOR_NONE;
        context.colorUseDefault = false;
    }

    int cornerIndex = ResolveInt(part, category.cornerType, Cfg::kUseGlobalCorner,
                                 global.cornerType);
    switch (cornerIndex) {
        case 1:
            context.cornerType = DWM_WINDOW_CORNER_PREFERENCE::DWMWCP_DONOTROUND;
            break;
        case 2:
            context.cornerType = DWM_WINDOW_CORNER_PREFERENCE::DWMWCP_ROUND;
            break;
        case 3:
            context.cornerType = DWM_WINDOW_CORNER_PREFERENCE::DWMWCP_ROUNDSMALL;
            break;
        default:
            context.cornerType = DWM_WINDOW_CORNER_PREFERENCE::DWMWCP_DEFAULT;
            break;
    }
    if (!SystemHelper::IsWindows11()) {
        context.cornerType = DWM_WINDOW_CORNER_PREFERENCE::DWMWCP_DONOTROUND;
    }
}

inline void QueryMenuCustomRenderingContext(bool darkMode, MenuCustomRenderingContext& context) {
    RtlSecureZeroMemory(&context, sizeof(context));
    Cfg::MenuExtras& ex = Cfg::Get().menuExtras;

    context.enable = ex.enableCustomRendering;
    if (!context.enable) {
        return;
    }

    context.separator_disabled = ex.separator_disabled;
    if (!context.separator_disabled) {
        context.separator_width = ex.separator_width;
        context.separator_color =
            darkMode ? ex.separator_darkModeColor : ex.separator_lightModeColor;
        if (ex.separator_enableThemeColorization) {
            context.separator_color = ThemeHelper::GetThemeColorizationColor(
                ThemeColorizationTypeToName(darkMode ? ex.separator_darkThemeColorizationType
                                                     : ex.separator_lightThemeColorizationType));
        }
    }

    context.focusing_disabled = ex.focusing_disabled;
    if (!context.focusing_disabled) {
        context.focusing_cornerRadius = ex.focusing_cornerRadius;
        context.focusing_width = ex.focusing_width;
        context.focusing_color =
            darkMode ? ex.focusing_darkModeColor : ex.focusing_lightModeColor;
        if (ex.focusing_enableThemeColorization) {
            context.focusing_color = ThemeHelper::GetThemeColorizationColor(
                ThemeColorizationTypeToName(darkMode ? ex.focusing_darkThemeColorizationType
                                                     : ex.focusing_lightThemeColorizationType));
        }
    }

    context.disabledHot_disabled = ex.disabledHot_disabled;
    if (!context.disabledHot_disabled) {
        context.disabledHot_cornerRadius = ex.disabledHot_cornerRadius;
        context.disabledHot_color =
            darkMode ? ex.disabledHot_darkModeColor : ex.disabledHot_lightModeColor;
        if (ex.disabledHot_enableThemeColorization) {
            context.disabledHot_color = ThemeHelper::GetThemeColorizationColor(
                ThemeColorizationTypeToName(darkMode ? ex.disabledHot_darkThemeColorizationType
                                                     : ex.disabledHot_lightThemeColorizationType));
        }
    }

    context.hot_disabled = ex.hot_disabled;
    if (!context.hot_disabled) {
        context.hot_cornerRadius = ex.hot_cornerRadius;
        context.hot_color = darkMode ? ex.hot_darkModeColor : ex.hot_lightModeColor;
        if (ex.hot_enableThemeColorization) {
            context.hot_color = ThemeHelper::GetThemeColorizationColor(
                ThemeColorizationTypeToName(darkMode ? ex.hot_darkThemeColorizationType
                                                     : ex.hot_lightThemeColorizationType));
        }
    }
}

inline void QueryMenuIconBackgroundColorRemovalContext(
    MenuIconBackgroundColorRemovalContext& context) {
    RtlSecureZeroMemory(&context, sizeof(context));
    Cfg::MenuExtras& ex = Cfg::Get().menuExtras;
    context.enable = ex.colorTreatAsTransparentEnabled;
    if (context.enable) {
        context.colorTreatAsTransparent = ex.colorTreatAsTransparent;
        context.colorTreatAsTransparentThreshold = ex.colorTreatAsTransparentThreshold;
    }
}

inline void QueryFlyoutAnimationContext(std::wstring_view part, FlyoutAnimationContext& context) {
    RtlSecureZeroMemory(&context, sizeof(context));
    if (_wcsicmp(std::wstring(part).c_str(), L"Menu") != 0) {
        // Dropdown animation settings exist but are not ported yet; keep off.
        return;
    }
    Cfg::MenuExtras& ex = Cfg::Get().menuExtras;
    context.enable = ex.enableFluentAnimation;
    context.fadeOutTime = ex.fadeOutTime;
    if (context.enable) {
        context.startRatio = ex.startRatio;
        context.popInTime = ex.popInTime;
        context.fadeInTime = ex.fadeInTime;
        context.popInStyle = ex.popInStyle;
        context.immediateInterupting = ex.immediateInterupting;
    }
}

inline void QueryTooltipRenderingContext(TooltipRenderingContext& context, bool darkMode) {
    RtlSecureZeroMemory(&context, sizeof(context));
    Cfg::TooltipExtras& tip = Cfg::Get().tooltipExtras;
    context.color = Utils::MakeCOLORREF(darkMode ? tip.darkModeColor : tip.lightModeColor);
    context.marginsType = tip.marginsType;
    context.margins = tip.margins;
}

// ------------------------------------------------------------------ apply ---

inline void ApplyBorderEffect(HWND hWnd, bool darkMode, const BorderContext& border) {
    EffectHelper::EnableWindowDarkMode(hWnd, darkMode);
    if (SystemHelper::IsWindows11()) {
        if (border.cornerType != DWM_WINDOW_CORNER_PREFERENCE::DWMWCP_DEFAULT) {
            DwmSetWindowAttribute(hWnd, DWMWA_WINDOW_CORNER_PREFERENCE, &border.cornerType,
                                  sizeof(border.cornerType));
        }
        COLORREF color{Utils::MakeCOLORREF(border.color)};
        if (border.colorUseNone) {
            color = border.color;
        }
        if (!border.colorUseDefault || border.colorUseNone) {
            DwmSetWindowAttribute(hWnd, DWMWA_BORDER_COLOR, &color, sizeof(color));
        }
    }
    DwmTransitionOwnedWindow(hWnd, DWMTRANSITION_OWNEDWINDOW_REPOSITION);
}

inline void ApplyBackdropEffect(HWND hWnd, bool darkMode,
                                const WindowBackdropEffectContext& backdropContext) {
    EffectHelper::EnableWindowDarkMode(hWnd, darkMode);
    EffectHelper::SetWindowBackdrop(
        hWnd, backdropContext.enableDropShadow,
        Utils::MakeCOLORREF(backdropContext.gradientColor) |
            (static_cast<DWORD>(Utils::GetAlphaFromARGB(backdropContext.gradientColor)) << 24),
        backdropContext.effectType);
    DwmTransitionOwnedWindow(hWnd, DWMTRANSITION_OWNEDWINDOW_REPOSITION);
}

inline void ApplyEffect(HWND hWnd, bool darkMode, const WindowBackdropEffectContext& backdropContext,
                        const BorderContext& border) {
    ApplyBackdropEffect(hWnd, darkMode, backdropContext);
    ApplyBorderEffect(hWnd, darkMode, border);
}

inline void DropEffect(std::wstring_view part, HWND hWnd) {
    EffectHelper::SetWindowBackdrop(hWnd, FALSE, 0,
                                    static_cast<DWORD>(EffectHelper::EffectType::None));
    if (SystemHelper::IsWindows11()) {
        auto cornerType{DWM_WINDOW_CORNER_PREFERENCE::DWMWCP_DEFAULT};
        if (!_wcsicmp(std::wstring(part).c_str(), L"Menu")) {
            cornerType = DWM_WINDOW_CORNER_PREFERENCE::DWMWCP_ROUNDSMALL;
        }
        DwmSetWindowAttribute(hWnd, DWMWA_WINDOW_CORNER_PREFERENCE, &cornerType,
                              sizeof(cornerType));
        COLORREF color{DWMWA_COLOR_NONE};
        DwmSetWindowAttribute(hWnd, DWMWA_BORDER_COLOR, &color, sizeof(color));
    }
    InvalidateRect(hWnd, nullptr, TRUE);
}

}  // namespace Api

}  // namespace tf


// ===========================================================================
// Flyout handling: menus, tooltips and dropdown lists.
//
// The engine intercepts uxtheme drawing in the target process (the same
// functions the upstream TranslucentFlyouts hooks) and identifies the owning
// flyout window during paint. uxtheme paints flyouts into buffered/memory DCs,
// so WindowFromDC is not enough; the flyout window is found by class among the
// painting thread's windows.
// ===========================================================================

namespace tf {

namespace Handler {

enum class FlyoutType {
    Unknown,
    Menu,
    Tooltip,
    DropDown,
    // A menu drawn by an app with its own drawing code: leave it alone.
    ThirdParty,
};

struct WindowState {
    FlyoutType type{FlyoutType::Unknown};
    bool darkMode{false};
    bool effectsApplied{false};
    DWORD tick{0};
    Api::WindowBackdropEffectContext backdrop{};
    Api::BorderContext border{};
    Api::MenuCustomRenderingContext customRendering{};
    Api::MenuIconBackgroundColorRemovalContext iconRemoval{};
    Api::FlyoutAnimationContext animation{};
    bool noSystemDropShadow{false};
    bool immersiveStyle{true};
};

std::mutex g_windowsLock;
std::unordered_map<HWND, WindowState> g_windows;

// The window state of the flyout being painted on this thread (set by the
// hooks while a flyout paint is in progress).
thread_local WindowState* g_paintingState = nullptr;

constexpr size_t kMaxWindows = 64;

WindowState* FindWindowState(HWND hWnd) {
    if (!hWnd) {
        return nullptr;
    }
    std::lock_guard<std::mutex> lock(g_windowsLock);
    auto it = g_windows.find(hWnd);
    return it == g_windows.end() ? nullptr : &it->second;
}

WindowState& GetOrCreateWindowState(HWND hWnd) {
    std::lock_guard<std::mutex> lock(g_windowsLock);
    auto it = g_windows.find(hWnd);
    if (it != g_windows.end()) {
        return it->second;
    }
    if (g_windows.size() >= kMaxWindows) {
        // Drop the oldest entry.
        auto oldest = g_windows.begin();
        for (auto iter = g_windows.begin(); iter != g_windows.end(); ++iter) {
            if (iter->second.tick < oldest->second.tick) {
                oldest = iter;
            }
        }
        g_windows.erase(oldest);
    }
    WindowState& state = g_windows[hWnd];
    state.tick = GetTickCount();
    return state;
}

void RemoveWindowState(HWND hWnd) {
    std::lock_guard<std::mutex> lock(g_windowsLock);
    g_windows.erase(hWnd);
}

// ---------------------------------------------------------------- helpers ---

inline bool IsBalloonTooltip(HWND hWnd) {
    return (GetWindowLongPtrW(hWnd, GWL_STYLE) & TTS_BALLOON) == TTS_BALLOON;
}

inline bool IsTooltipWindow(HWND hWnd) {
    // An unthemed tooltip window (e.g. the WinForms ToolTip control, which
    // calls SetWindowTheme(hwnd, L"", L"") internally) paints its own classic
    // background and cannot show the backdrop effect; the original app skips
    // these too.
    return hWnd && !IsBalloonTooltip(hWnd) && GetWindowTheme(hWnd) != nullptr &&
           Utils::IsWindowClassContains(hWnd, L"tooltips_class32");
}

inline bool IsMenuWindow(HWND hWnd) { return Utils::IsPopupMenu(hWnd); }

inline bool IsDropDownWindow(HWND hWnd) {
    return Utils::IsWindowClassContains(hWnd, L"listviewpopup") ||
           Utils::IsWindowClassContains(hWnd, L"combolbox") ||
           Utils::IsWindowClassContains(hWnd, L"dropdown");
}

struct EnumWindowParam {
    FlyoutType type;
    HWND result;
};

BOOL CALLBACK EnumFlyoutProc(HWND hWnd, LPARAM lParam) {
    auto* param = reinterpret_cast<EnumWindowParam*>(lParam);
    bool match = false;
    switch (param->type) {
        case FlyoutType::Menu:
            match = IsMenuWindow(hWnd);
            break;
        case FlyoutType::Tooltip:
            match = IsTooltipWindow(hWnd);
            break;
        case FlyoutType::DropDown:
            match = IsDropDownWindow(hWnd);
            break;
        default:
            break;
    }
    if (match) {
        param->result = hWnd;
        return FALSE;
    }
    return TRUE;
}

HWND FindFlyoutWindowOnCurrentThread(FlyoutType type) {
    EnumWindowParam param{type, nullptr};
    EnumThreadWindows(GetCurrentThreadId(), EnumFlyoutProc, reinterpret_cast<LPARAM>(&param));
    return param.result;
}

// The flyout window for the DC being painted. Prefers WindowFromDC; falls back
// to the thread's window of the given type (buffered paint DCs have no window).
HWND ResolveFlyoutWindow(HDC hdc, FlyoutType type) {
    HWND w = WindowFromDC(hdc);
    if (w && (IsMenuWindow(w) || IsTooltipWindow(w) || IsDropDownWindow(w))) {
        return w;
    }
    return FindFlyoutWindowOnCurrentThread(type);
}

FlyoutType TypeForThemeClass(PCWSTR themeClass) {
    if (!_wcsicmp(themeClass, L"Menu")) {
        return FlyoutType::Menu;
    }
    if (!_wcsicmp(themeClass, L"Tooltip")) {
        return FlyoutType::Tooltip;
    }
    // Only the dropped-down list draws with the ListviewPopup theme class; the
    // closed combo box uses "Combobox" and must not be treated as a flyout.
    if (!_wcsicmp(themeClass, L"ListviewPopup")) {
        return FlyoutType::DropDown;
    }
    return FlyoutType::Unknown;
}

// Dark mode for menus: compare the menu's text color with the color of the
// dark menu theme (upstream method).
bool MenuShouldUseDarkMode(HTHEME hTheme) {
    COLORREF color{DWMWA_COLOR_NONE};
    COLORREF expectedColor{0};
    HTHEME darkTheme = OpenThemeData(nullptr, L"DarkMode::Menu");
    if (darkTheme) {
        HRESULT hr1 = GetThemeColor(hTheme, MENU_POPUPITEM, 0, TMT_TEXTCOLOR, &color);
        HRESULT hr2 = GetThemeColor(darkTheme, MENU_POPUPITEM, 0, TMT_TEXTCOLOR, &expectedColor);
        CloseThemeData(darkTheme);
        if (SUCCEEDED(hr1) && SUCCEEDED(hr2) && color == expectedColor) {
            return true;
        }
        return false;
    }
    return ThemeHelper::ShouldAppsUseDarkMode() && ThemeHelper::IsDarkModeAllowedForApp();
}

bool TooltipShouldUseDarkMode(HWND hWnd, HWND owner) {
    HWND tray = FindWindowW(L"Shell_TrayWnd", nullptr);
    if (tray && GetWindowThreadProcessId(tray, nullptr) == GetWindowThreadProcessId(hWnd, nullptr)) {
        static const auto ShouldSystemUseDarkMode = reinterpret_cast<bool(WINAPI*)()>(
            GetProcAddress(GetModuleHandleW(L"UxTheme.dll"), MAKEINTRESOURCEA(138)));
        return ShouldSystemUseDarkMode ? ShouldSystemUseDarkMode() : false;
    }
    (void)owner;
    return ThemeHelper::ShouldAppsUseDarkMode() && ThemeHelper::IsDarkModeAllowedForApp();
}

// Refresh the cached window state from the current settings.
void RefreshWindowState(HWND hWnd, WindowState& state, bool darkMode) {
    state.darkMode = darkMode;
    switch (state.type) {
        case FlyoutType::Menu: {
            state.noSystemDropShadow = Cfg::Get().menuExtras.noSystemDropShadow;
            state.immersiveStyle = Cfg::Get().menuExtras.enableImmersiveStyle;
            Api::QueryBackdropEffectContext(L"Menu", darkMode, state.backdrop);
            Api::QueryBorderContext(L"Menu", darkMode, state.border);
            Api::QueryMenuCustomRenderingContext(darkMode, state.customRendering);
            Api::QueryMenuIconBackgroundColorRemovalContext(state.iconRemoval);
            Api::QueryFlyoutAnimationContext(L"Menu", state.animation);
            break;
        }
        case FlyoutType::Tooltip: {
            state.noSystemDropShadow = Cfg::Get().tooltipExtras.noSystemDropShadow;
            Api::QueryBackdropEffectContext(L"Tooltip", darkMode, state.backdrop);
            Api::QueryBorderContext(L"Tooltip", darkMode, state.border);
            break;
        }
        case FlyoutType::DropDown: {
            Api::QueryBackdropEffectContext(L"DropDown", darkMode, state.backdrop);
            Api::QueryBorderContext(L"DropDown", darkMode, state.border);
            break;
        }
        default:
            break;
    }
}

void ApplyWindowEffects(HWND hWnd, WindowState& state) {
    Api::ApplyEffect(hWnd, state.darkMode, state.backdrop, state.border);
    state.effectsApplied = true;

    // Hide the system drop shadow window that follows flyouts (the "SysShadow"
    // class sibling), when the settings ask for no system shadow.
    if (state.noSystemDropShadow) {
        HWND backdropWindow = GetWindow(hWnd, GW_HWNDNEXT);
        if (Utils::IsWindowClass(backdropWindow, L"SysShadow")) {
            ShowWindow(backdropWindow, SW_HIDE);
        }
    }
}

// The flyout's first paint triggers the effect application, so the accent
// policy is in place before DWM composites the frame.
WindowState* EnsureWindowState(HWND hWnd, FlyoutType type, bool darkMode) {
    WindowState& state = GetOrCreateWindowState(hWnd);
    bool applyNow = false;
    if (state.type != type) {
        state.type = type;
        state.effectsApplied = false;
    }
    if (!state.effectsApplied) {
        RefreshWindowState(hWnd, state, darkMode);
        applyNow = true;
    }
    if (applyNow && !Api::IsPartDisabled(type == FlyoutType::Menu     ? L"Menu"
                                         : type == FlyoutType::Tooltip ? L"Tooltip"
                                                                       : L"DropDown")) {
        ApplyWindowEffects(hWnd, state);
        Log::Write(L"[fx] applied type=%d hwnd=%p dark=%d effect=%u", (int)type, hWnd,
                   (int)darkMode, state.backdrop.effectType);
    }
    return &state;
}

}  // namespace Handler

// ===========================================================================
// Rendering (port of TFMain/MenuRendering.cpp, GDI variant)
// ===========================================================================

namespace Rendering {

bool HandlePopupMenuNonClientBorderColors(HDC hdc, const RECT& paintRect) {
    Handler::WindowState* state = Handler::g_paintingState;
    if (!state) {
        return false;
    }

    if (!state->border.colorUseNone) {
        if (state->border.colorUseDefault) {
            return false;
        }
        unique_handle<HBRUSH> brush{Utils::CreateSolidColorBrushWithAlpha(
            Utils::MakeCOLORREF(state->border.color),
            Utils::GetAlphaFromARGB(state->border.color))};
        if (brush) {
            FrameRect(hdc, &paintRect, brush.get());
        }
    } else {
        FrameRect(hdc, &paintRect, GetStockBrush(BLACK_BRUSH));
    }
    return true;
}

bool HandleCustomRendering(HDC hdc, int partId, int stateId, const RECT& clipRect,
                           const RECT& paintRect) {
    Handler::WindowState* state = Handler::g_paintingState;
    if (!state) {
        return false;
    }
    const Api::MenuCustomRenderingContext& custom = state->customRendering;

    DcStateGuard dcState(hdc);
    IntersectClipRect(hdc, clipRect.left, clipRect.top, clipRect.right, clipRect.bottom);
    PatBlt(hdc, paintRect.left, paintRect.top, paintRect.right - paintRect.left,
           paintRect.bottom - paintRect.top, BLACKNESS);

    auto drawRoundedFill = [&](DWORD color, DWORD radius, bool stroke, DWORD width) {
        RECT rc = paintRect;
        int w = rc.right - rc.left;
        int h = rc.bottom - rc.top;
        if (w <= 0 || h <= 0) {
            return false;
        }
        unique_handle<HBRUSH> brush{Utils::CreateSolidColorBrushWithAlpha(
            Utils::MakeCOLORREF(color), Utils::GetAlphaFromARGB(color))};
        if (!brush) {
            return false;
        }
        HGDIOBJ oldBrush = SelectObject(hdc, brush.get());
        HGDIOBJ oldPen = SelectObject(hdc, GetStockObject(NULL_PEN));
        RoundRect(hdc, rc.left, rc.top, rc.right, rc.bottom, (int)radius * 2, (int)radius * 2);
        SelectObject(hdc, oldPen);
        SelectObject(hdc, oldBrush);
        (void)stroke;
        (void)width;
        return true;
    };

    if (partId == MENU_POPUPSEPARATOR) {
        if (custom.separator_disabled) {
            return false;
        }
        int height = paintRect.bottom - paintRect.top;
        int thickness = std::max(1, (int)(custom.separator_width / 1000));
        RECT line{paintRect.left, paintRect.top + height / 2, paintRect.right,
                  paintRect.top + height / 2 + thickness};
        unique_handle<HBRUSH> brush{Utils::CreateSolidColorBrushWithAlpha(
            Utils::MakeCOLORREF(custom.separator_color),
            Utils::GetAlphaFromARGB(custom.separator_color))};
        if (!brush) {
            return false;
        }
        FillRect(hdc, &line, brush.get());
        return true;
    }

    if (partId == MENU_POPUPITEMKBFOCUS) {
        if (custom.focusing_disabled) {
            return false;
        }
        return drawRoundedFill(custom.focusing_color, custom.focusing_cornerRadius, true,
                               custom.focusing_width);
    }

    if (partId == MENU_POPUPITEM || partId == MENU_POPUPITEMFOCUSABLE) {
        if (stateId == MPI_DISABLEDHOT) {
            if (custom.disabledHot_disabled) {
                return false;
            }
            return drawRoundedFill(custom.disabledHot_color, custom.disabledHot_cornerRadius, false,
                                   0);
        }
        if (stateId == MPI_HOT) {
            if (custom.hot_disabled) {
                return false;
            }
            return drawRoundedFill(custom.hot_color, custom.hot_cornerRadius, false, 0);
        }
    }

    return false;
}

// Port of MenuRendering::HandleDrawThemeBackground: decides what to draw for
// "Menu"-themed parts. Returns true when the call was handled.
bool HandleMenuDrawThemeBackground(HTHEME hTheme, HDC hdc, int iPartId, int iStateId,
                                   LPCRECT pRect, LPCRECT pClipRect,
                                   decltype(&DrawThemeBackground) actualDrawThemeBackground) {
    Handler::WindowState* state = Handler::g_paintingState;
    if (!state || IsRectEmpty(pRect)) {
        return false;
    }

    RECT clipRect{*pRect};
    if (pClipRect) {
        IntersectRect(&clipRect, &clipRect, pClipRect);
    }

    if (iPartId == MENU_POPUPSEPARATOR || iPartId == MENU_POPUPITEMKBFOCUS) {
        if (state->customRendering.enable &&
            HandleCustomRendering(hdc, iPartId, iStateId, clipRect, *pRect)) {
            return true;
        }
        return false;
    }

    if (iPartId == MENU_POPUPITEM || iPartId == MENU_POPUPITEMFOCUSABLE) {
        if (iStateId == MPI_DISABLEDHOT && state->customRendering.enable &&
            HandleCustomRendering(hdc, iPartId, iStateId, clipRect, *pRect)) {
            return true;
        }
        if (iStateId == MPI_HOT) {
            if (state->customRendering.enable &&
                HandleCustomRendering(hdc, iPartId, iStateId, clipRect, *pRect)) {
                return true;
            }
            if (actualDrawThemeBackground) {
                actualDrawThemeBackground(hTheme, hdc, iPartId, iStateId, pRect, pClipRect);
            }
            return true;
        }
    }

    if (iPartId != MENU_POPUPBACKGROUND && iPartId != MENU_POPUPBORDERS &&
        iPartId != MENU_POPUPGUTTER && iPartId != MENU_POPUPITEM &&
        iPartId != MENU_POPUPITEMFOCUSABLE) {
        return false;
    }

    PatBlt(hdc, clipRect.left, clipRect.top, clipRect.right - clipRect.left,
           clipRect.bottom - clipRect.top, BLACKNESS);
    return true;
}

bool HandleMenuBitmap(HBITMAP& source, unique_handle<HBITMAP>& target) {
    Handler::WindowState* state = Handler::g_paintingState;
    if (source && !Utils::IsOemBitmap(source)) {
        target.reset(Utils::Promise32BPP(source));
        if (!target) {
            return false;
        }
        if (state && state->iconRemoval.enable) {
            Utils::BitmapRemoveColor(target.get(), state->iconRemoval.colorTreatAsTransparent,
                                     state->iconRemoval.colorTreatAsTransparentThreshold);
        }
        source = target.get();
        return true;
    }
    return false;
}

}  // namespace Rendering

// ===========================================================================
// uxtheme / user32 hooks
// ===========================================================================

namespace Hooks {

thread_local Handler::FlyoutType g_paintingType = Handler::FlyoutType::Unknown;

using DrawThemeBackgroundFn = decltype(&DrawThemeBackground);
using DrawThemeBackgroundExFn = decltype(&DrawThemeBackgroundEx);
using DrawThemeTextFn = decltype(&DrawThemeText);
using DrawThemeTextExFn = decltype(&DrawThemeTextEx);
using GetThemeMarginsFn = decltype(&GetThemeMargins);
using DrawTextWFn = decltype(&DrawTextW);

DrawThemeBackgroundFn g_drawThemeBackgroundOrig = nullptr;
DrawThemeBackgroundExFn g_drawThemeBackgroundExOrig = nullptr;
DrawThemeTextFn g_drawThemeTextOrig = nullptr;
DrawThemeTextExFn g_drawThemeTextExOrig = nullptr;
GetThemeMarginsFn g_getThemeMarginsOrig = nullptr;
DrawTextWFn g_drawTextWOrig = nullptr;

void* g_hookTargets[8]{};
int g_hookTargetCount = 0;

// Sets the per-thread paint context for the current call.
Handler::WindowState* BeginPaint(HTHEME hTheme, HDC hdc, Handler::FlyoutType& typeOut) {
    typeOut = Handler::FlyoutType::Unknown;
    wchar_t themeClass[MAX_PATH + 1]{};
    if (FAILED(ThemeHelper::GetThemeClass(hTheme, themeClass, MAX_PATH))) {
        return nullptr;
    }
    Handler::FlyoutType type = Handler::TypeForThemeClass(themeClass);
    if (type == Handler::FlyoutType::Unknown) {
        return nullptr;
    }
    const wchar_t* part = type == Handler::FlyoutType::Menu     ? L"Menu"
                          : type == Handler::FlyoutType::Tooltip ? L"Tooltip"
                                                                 : L"DropDown";
    if (Api::IsPartDisabled(part)) {
        return nullptr;
    }

    HWND hWnd = Handler::ResolveFlyoutWindow(hdc, type);
    if (!hWnd) {
        return nullptr;
    }

    bool darkMode = false;
    if (type == Handler::FlyoutType::Menu) {
        darkMode = Handler::MenuShouldUseDarkMode(hTheme);
    } else if (type == Handler::FlyoutType::Tooltip) {
        HWND owner = reinterpret_cast<HWND>(GetWindowLongPtrW(hWnd, GWLP_HWNDPARENT));
        darkMode = Handler::TooltipShouldUseDarkMode(hWnd, owner);
    } else {
        darkMode = ThemeHelper::ShouldAppsUseDarkMode() && ThemeHelper::IsDarkModeAllowedForApp();
    }

    typeOut = type;
    return Handler::EnsureWindowState(hWnd, type, darkMode);
}

HRESULT WINAPI DrawThemeBackgroundHook(HTHEME hTheme, HDC hdc, int iPartId, int iStateId,
                                       LPCRECT pRect, LPCRECT pClipRect) {
    Handler::FlyoutType type{};
    Handler::WindowState* state = BeginPaint(hTheme, hdc, type);
    if (!state) {
        return g_drawThemeBackgroundOrig(hTheme, hdc, iPartId, iStateId, pRect, pClipRect);
    }

    auto previous = std::exchange(Handler::g_paintingState, state);
    g_paintingType = type;
    struct Restore {
        Handler::WindowState* previous;
        ~Restore() { Handler::g_paintingState = previous; }
    } restore{previous};

    if (type == Handler::FlyoutType::Menu) {
        if (iPartId == MENU_POPUPBORDERS) {
            RECT clipRect{*pRect};
            if (pClipRect) {
                IntersectRect(&clipRect, &clipRect, pClipRect);
            }
            PatBlt(hdc, clipRect.left, clipRect.top, clipRect.right - clipRect.left,
                   clipRect.bottom - clipRect.top, BLACKNESS);
            if (!Rendering::HandlePopupMenuNonClientBorderColors(hdc, clipRect)) {
                DcStateGuard dcState(hdc);
                ExcludeClipRect(hdc, clipRect.left + 1, clipRect.top + 1, clipRect.right - 1,
                                clipRect.bottom - 1);
                g_drawThemeBackgroundOrig(hTheme, hdc, iPartId, iStateId, pRect, pClipRect);
            }
            return S_OK;
        }
        if (Rendering::HandleMenuDrawThemeBackground(hTheme, hdc, iPartId, iStateId, pRect, pClipRect,
                                                     g_drawThemeBackgroundOrig)) {
            return S_OK;
        }
    } else if (type == Handler::FlyoutType::Tooltip) {
        if (iPartId == TTP_STANDARD && !IsRectEmpty(pRect)) {
            RECT paintRect{*pRect};
            if (pClipRect) {
                IntersectRect(&paintRect, &paintRect, pClipRect);
            }
            PatBlt(hdc, paintRect.left, paintRect.top, paintRect.right - paintRect.left,
                   paintRect.bottom - paintRect.top, BLACKNESS);
            return S_OK;
        }
    } else if (type == Handler::FlyoutType::DropDown) {
        if (IsRectEmpty(pRect)) {
            return g_drawThemeBackgroundOrig(hTheme, hdc, iPartId, iStateId, pRect, pClipRect);
        }
        RECT clipRect{*pRect};
        if (pClipRect) {
            IntersectRect(&clipRect, &clipRect, pClipRect);
        }
        // Gut the list background; item paintings go through unchanged.
        if (iPartId != LVP_LISTITEM && iPartId != LVP_LISTGROUP && iPartId != LVP_LISTDETAIL) {
            PatBlt(hdc, clipRect.left, clipRect.top, clipRect.right - clipRect.left,
                   clipRect.bottom - clipRect.top, BLACKNESS);
            return S_OK;
        }
    }

    return g_drawThemeBackgroundOrig(hTheme, hdc, iPartId, iStateId, pRect, pClipRect);
}

HRESULT WINAPI DrawThemeBackgroundExHook(HTHEME hTheme, HDC hdc, int iPartId, int iStateId,
                                         LPCRECT pRect, const DTBGOPTS* pOptions) {
    LPCRECT pClipRect = pOptions ? &pOptions->rcClip : nullptr;
    return DrawThemeBackgroundHook(hTheme, hdc, iPartId, iStateId, pRect, pClipRect);
}

HRESULT WINAPI DrawThemeTextHook(HTHEME hTheme, HDC hdc, int iPartId, int iStateId, LPCWSTR pszText,
                                 int cchText, DWORD dwTextFlags, DWORD dwTextFlags2, LPCRECT pRect) {
    Handler::FlyoutType type{};
    Handler::WindowState* state = BeginPaint(hTheme, hdc, type);
    if (!state) {
        return g_drawThemeTextOrig(hTheme, hdc, iPartId, iStateId, pszText, cchText, dwTextFlags,
                                   dwTextFlags2, pRect);
    }
    if (type == Handler::FlyoutType::Tooltip && !IsRectEmpty(pRect)) {
        DTTOPTS options{sizeof(DTTOPTS), DTT_TEXTCOLOR | DTT_COMPOSITED};
        const auto& tip = Cfg::Get().tooltipExtras;
        options.crText = Utils::MakeCOLORREF(state->darkMode ? tip.darkModeColor : tip.lightModeColor);
        return g_drawThemeTextExOrig(hTheme, hdc, iPartId, iStateId, pszText, cchText,
                                    dwTextFlags, const_cast<LPRECT>(pRect), &options);
    }
    if (type != Handler::FlyoutType::Menu || IsRectEmpty(pRect)) {
        return g_drawThemeTextOrig(hTheme, hdc, iPartId, iStateId, pszText, cchText, dwTextFlags,
                                   dwTextFlags2, pRect);
    }

    // Composite the text with alpha so it renders correctly over the translucent
    // background (upstream MyDrawThemeText).
    auto previous = std::exchange(Handler::g_paintingState, state);
    struct Restore {
        Handler::WindowState* previous;
        ~Restore() { Handler::g_paintingState = previous; }
    } restore{previous};

    struct TextParams {
        HTHEME hTheme;
        int iPartId;
        int iStateId;
        LPCWSTR pszText;
        int cchText;
        DWORD dwTextFlags;
        LPRECT pRect;
    } params{hTheme, iPartId, iStateId, pszText, cchText, dwTextFlags, const_cast<LPRECT>(pRect)};

    HRESULT hr = ThemeHelper::DrawThemeContent(
        hdc, *pRect, nullptr, nullptr, 0,
        [](HDC memoryDC, HPAINTBUFFER, RGBQUAD*, int, LPARAM lParam) {
            auto* p = reinterpret_cast<TextParams*>(lParam);
            DTTOPTS options{sizeof(DTTOPTS), DTT_COMPOSITED};
            DrawThemeTextEx(p->hTheme, memoryDC, p->iPartId, p->iStateId, p->pszText, p->cchText,
                            p->dwTextFlags, p->pRect, &options);
        },
        reinterpret_cast<LPARAM>(&params));
    if (FAILED(hr)) {
        return g_drawThemeTextOrig(hTheme, hdc, iPartId, iStateId, pszText, cchText, dwTextFlags,
                                   dwTextFlags2, pRect);
    }
    return S_OK;
}

HRESULT WINAPI DrawThemeTextExHook(HTHEME hTheme, HDC hdc, int iPartId, int iStateId,
                                   LPCWSTR pszText, int cchText, DWORD dwTextFlags, LPRECT pRect,
                                   const DTTOPTS* pOptions) {
    Handler::FlyoutType type{};
    Handler::WindowState* state = BeginPaint(hTheme, hdc, type);
    if (!state) {
        return g_drawThemeTextExOrig(hTheme, hdc, iPartId, iStateId, pszText, cchText, dwTextFlags,
                                     pRect, pOptions);
    }
    if (type == Handler::FlyoutType::Tooltip) {
        auto previous = std::exchange(Handler::g_paintingState, state);
        struct Restore {
            Handler::WindowState* previous;
            ~Restore() { Handler::g_paintingState = previous; }
        } restore{previous};
        Cfg::TooltipExtras& tip = Cfg::Get().tooltipExtras;
        DTTOPTS options = pOptions ? *pOptions : DTTOPTS{};
        options.dwSize = sizeof(options);
        options.dwFlags |= DTT_TEXTCOLOR;
        options.crText = Utils::MakeCOLORREF(state->darkMode ? tip.darkModeColor : tip.lightModeColor);
        return g_drawThemeTextExOrig(hTheme, hdc, iPartId, iStateId, pszText, cchText, dwTextFlags,
                                     pRect, &options);
    }
    return g_drawThemeTextExOrig(hTheme, hdc, iPartId, iStateId, pszText, cchText, dwTextFlags,
                                 pRect, pOptions);
}

HRESULT WINAPI GetThemeMarginsHook(HTHEME hTheme, HDC hdc, int iPartId, int iStateId, int iPropId,
                                   RECT* prc, MARGINS* pMargins) {
    if (pMargins) {
        wchar_t themeClass[MAX_PATH + 1]{};
        if (SUCCEEDED(ThemeHelper::GetThemeClass(hTheme, themeClass, MAX_PATH)) &&
            !_wcsicmp(themeClass, L"Tooltip")) {
            Cfg::TooltipExtras& tip = Cfg::Get().tooltipExtras;
            if (tip.marginsType == 1) {
                *pMargins = tip.margins;
                return S_OK;
            }
        }
    }
    return g_getThemeMarginsOrig(hTheme, hdc, iPartId, iStateId, iPropId, prc, pMargins);
}

namespace {
struct TooltipDrawTextParams {
    LPCWSTR text;
    int cch;
    LPRECT rect;
    UINT format;
    int result;
    COLORREF color;
};

void TooltipDrawTextCallback(HDC memoryDC, HPAINTBUFFER, RGBQUAD*, int, LPARAM lParam) {
    auto* p = reinterpret_cast<TooltipDrawTextParams*>(lParam);
    SetTextColor(memoryDC, p->color);
    SetBkMode(memoryDC, TRANSPARENT);
    p->result = g_drawTextWOrig(memoryDC, p->text, p->cch, p->rect, p->format);
}
}  // namespace

int WINAPI DrawTextWHook(HDC hdc, LPCWSTR lpchText, int cchText, LPRECT lprc, UINT format) {
    // Tooltip text with alpha (upstream TooltipHooks::MyDrawTextW).
    HWND hWnd = WindowFromDC(hdc);
    // Common controls paint tooltip text into a buffered DC with no HWND.
    if (!hWnd && GetObjectType(hdc) == OBJ_MEMDC) {
        HWND tip = Handler::FindFlyoutWindowOnCurrentThread(Handler::FlyoutType::Tooltip);
        if (tip && IsWindowVisible(tip)) {
            hWnd = tip;
        }
    }
    if (hWnd && Handler::IsTooltipWindow(hWnd) &&
        !(format & (DT_CALCRECT | DT_INTERNAL))) {
        Handler::WindowState& state = Handler::GetOrCreateWindowState(hWnd);
        if (state.type == Handler::FlyoutType::Tooltip ||
            state.type == Handler::FlyoutType::Unknown) {
            Cfg::TooltipExtras& tip = Cfg::Get().tooltipExtras;
            COLORREF color =
                Utils::MakeCOLORREF(state.darkMode ? tip.darkModeColor : tip.lightModeColor);
            COLORREF oldColor = SetTextColor(hdc, color);
            TooltipDrawTextParams params{lpchText, cchText, lprc, format, 0, color};
            HRESULT hr = ThemeHelper::DrawThemeContent(
                hdc, *lprc, nullptr, nullptr, 0, TooltipDrawTextCallback,
                reinterpret_cast<LPARAM>(&params));
            SetTextColor(hdc, oldColor);
            if (SUCCEEDED(hr)) {
                return params.result;
            }
        }
    }
    return g_drawTextWOrig(hdc, lpchText, cchText, lprc, format);
}

// ------------------------------------------------------------- lifecycle ---

void InstallHook(HMODULE module, const char* name, void* hook, void** original) {
    if (!module) {
        return;
    }
    FARPROC target = GetProcAddress(module, name);
    if (!target) {
        return;
    }
    if (WindhawkUtils::SetFunctionHook(reinterpret_cast<void*>(target), hook,
                                       reinterpret_cast<void**>(original))) {
        if (g_hookTargetCount < (int)std::size(g_hookTargets)) {
            g_hookTargets[g_hookTargetCount++] = reinterpret_cast<void*>(target);
        }
        Log::Write(L"[init] hooked %S (%p)", name, target);
    } else {
        Log::Write(L"[init] hook FAILED %S", name);
    }
}

void Install() {
    HMODULE uxtheme = GetModuleHandleW(L"uxtheme.dll");
    HMODULE user32 = GetModuleHandleW(L"user32.dll");
    if (!uxtheme) {
        uxtheme = LoadLibraryW(L"uxtheme.dll");
    }
    if (!uxtheme) {
        Log::Write(L"[init] uxtheme.dll missing");
        return;
    }

    InstallHook(uxtheme, "DrawThemeBackground",
                reinterpret_cast<void*>(DrawThemeBackgroundHook),
                reinterpret_cast<void**>(&g_drawThemeBackgroundOrig));
    InstallHook(uxtheme, "DrawThemeBackgroundEx",
                reinterpret_cast<void*>(DrawThemeBackgroundExHook),
                reinterpret_cast<void**>(&g_drawThemeBackgroundExOrig));
    InstallHook(uxtheme, "DrawThemeText", reinterpret_cast<void*>(DrawThemeTextHook),
                reinterpret_cast<void**>(&g_drawThemeTextOrig));
    InstallHook(uxtheme, "DrawThemeTextEx", reinterpret_cast<void*>(DrawThemeTextExHook),
                reinterpret_cast<void**>(&g_drawThemeTextExOrig));
    InstallHook(uxtheme, "GetThemeMargins", reinterpret_cast<void*>(GetThemeMarginsHook),
                reinterpret_cast<void**>(&g_getThemeMarginsOrig));
    InstallHook(user32, "DrawTextW", reinterpret_cast<void*>(DrawTextWHook),
                reinterpret_cast<void**>(&g_drawTextWOrig));
}

void Uninstall() {
    for (int i = 0; i < g_hookTargetCount; i++) {
        Wh_RemoveFunctionHook(g_hookTargets[i]);
    }
    g_hookTargetCount = 0;
    Wh_ApplyHookOperations();
}

}  // namespace Hooks

}  // namespace tf

// ===========================================================================
// Mod entry points
// ===========================================================================

BOOL Wh_ModInit() {
    tf::Cfg::Load();
    if (tf::Api::IsCurrentProcessInBlockList()) {
        tf::Log::Write(L"[init] process is block-listed");
        return FALSE;
    }
    tf::Log::Write(L"[init] process=%s", tf::Utils::GetProcessName().c_str());
    tf::Hooks::Install();
    return TRUE;
}

void Wh_ModSettingsChanged() {
    tf::Cfg::Load();
    // Existing flyout windows keep their applied effect until they close; new
    // ones pick up the new settings.
}

void Wh_ModBeforeUninit() {
    tf::Hooks::Uninstall();
}






