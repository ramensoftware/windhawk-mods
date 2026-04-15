// ==WindhawkMod==
// @id              translucent-flyouts-controller
// @name            Translucent Flyouts Controller
// @description     Controls TranslucentFlyouts settings through Windhawk (registry bridge)
// @version         1.1.0
// @author          GID0317
// @github          https://github.com/GID0317
// @include         windhawk.exe
// @compilerOptions -lwtsapi32
// ==/WindhawkMod==

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

- controller:
  - resetAction: none
    $name: Reset To Defaults
    $description: Choose a category to reset to defaults. Set back to None before triggering another reset
    $options:
      - none: None
      - global: Reset Global
      - dropdown: Reset DropDown
      - menu: Reset Menu
      - tooltip: Reset Tooltip
      - all: Reset All
  - confirmReset: true
    $name: Confirm Reset
    $description: Show a confirmation before applying a reset-to-defaults action
  - hardReloadOnApply: false
    $name: Hard Reload On Apply
    $description: Also send TranslucentFlyouts detach and attach messages after apply. Slower, but can fix stale visuals
  - configAppCompatibilityMode: false
    $name: Translucent Flyouts Config Compatibility Mode
    $description: Delete known zero-color values instead of writing 0. Keeps Translucent Flyouts Config from crashing on those values
  $name: Controller

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
# Translucent Flyouts Controller for Windhawk

![Preview](https://i.imgur.com/F64iYmG.gif)

This mod provides a Windhawk settings interface for [TranslucentFlyouts](https://github.com/ALTaleX531/TranslucentFlyouts), a Windows utility that applies visual effects to system menus and tooltips. The controller bridges Windhawk's GUI to the registry keys that TranslucentFlyouts monitors.

TranslucentFlyouts handles the actual rendering of transparency and effects. This controller simply lets you configure those effects without manual registry editing.

## Requirements

You must have [TranslucentFlyouts](https://github.com/ALTaleX531/TranslucentFlyouts) already installed and running. If you don't have it, install it first. This controller won't work standalone.

Minimum OS: Windows 10/11 (depends on selected effect)
Recommended: Windows 11 22H2 or newer

## How Settings Are Applied

When you adjust a setting and click Apply:

1. The mod reads your Windhawk configuration
2. Writes the corresponding values to TranslucentFlyouts registry keys (under HKCU\Software\TranslucentFlyouts)
3. Sends a refresh signal so changes take effect immediately

By default, this refresh is fast. It pokes the running TranslucentFlyouts process. If you enable "Hard Reload On Apply", it fully restarts the process instead, which is slower but sometimes necessary if visuals appear corrupted or stale.

## The Category System

Settings are organized into four main categories:

- **Global**: Default settings that apply across all menu types
- **DropDown**: Behavior for standard dropdown menus
- **Menu**: Styling for context menus and application menus
- **Tooltip**: Appearance of system tooltips

Each category can either use its own settings or inherit from Global via the "use global" option. This lets you have one consistent style throughout Windows, or customize specific menu types.

## Resetting to Defaults

Use the "Controller / Reset To Defaults" dropdown to restore a category to its built-in defaults:

1. Select which category to reset: Global, DropDown, Menu, Tooltip, or All
2. Click Apply
3. The dropdown automatically returns to None

You can trigger another reset anytime by picking a category again. This pattern exists because Windhawk doesn't support custom buttons in settings. Only dropdowns and toggles are available.

## Key Details

- The "use global" setting (when available) makes a category inherit the corresponding Global setting value
- Color values use ARGB hex format: `0xAARRGGBB`
  - Example: `0xFF0000FF` is fully opaque blue
  - Example: `0x801F75FF` is semi-transparent blue
- Some settings interact with each other. For example, "Enable Immersive Style" on menu applies Windows 11's modern visual language
- Effect types include options like Acrylic, Mica, Blur, and Transparent, each with different visual characteristics

## Advanced Functions Warning

This section is only meant for advanced users.

Configuring the Block List can require restarting Translucent Flyouts or Windows and may prevent TranslucentFlyouts from loading for those applications.

If used incorrectly, these options can cause serious problems, including high CPU usage.

## References

This controller is built on top of two excellent projects:

- [TranslucentFlyouts](https://github.com/ALTaleX531/TranslucentFlyouts) by ALTaleX531 provides the original runtime and handles all the actual menu rendering that makes Windows menus truly customizable.

- [Translucent Flyouts Config](https://github.com/Satanarious/TranslucentFlyoutsConfig) by Satanarious created a polished configuration application that became the standard way to manage these settings. The setting names, organization, and defaults used in this controller are based on that work.

Both projects are well maintained and worth exploring if you want to understand how it all works together.
*/
// ==/WindhawkModReadme==

#include <windhawk_utils.h>
#include <windows.h>
#include <CommCtrl.h>
#include <wtsapi32.h>
#include <cstdint>
#include <cstdlib>
#include <cwctype>
#include <string>
#include <vector>

struct Settings {
    int globalEffectType = 5;
    int globalCornerType = 3;
    int globalEnableDropShadow = 0;
    int globalNoBorderColor = 0;
    int globalEnableThemeColorization = 0;
    int globalDarkModeThemeColorizationType = 1;
    int globalLightModeThemeColorizationType = 1;
    DWORD globalDarkModeBorderColor = 0xFF2B2B2B;
    DWORD globalLightModeBorderColor = 0xFFDDDDDD;
    DWORD globalDarkModeGradientColor = 0x412B2B2B;
    DWORD globalLightModeGradientColor = 0x9EDDDDDD;
    int globalEnableMiniDump = 1;
    int globalDisabled = 0;

    int dropDownEffectType = 9;
    int dropDownCornerType = 4;
    int dropDownEnableDropShadow = 2;
    int dropDownEnableFluentAnimation = 0;
    int dropDownNoBorderColor = 2;
    int dropDownEnableThemeColorization = 2;
    int dropDownDarkModeThemeColorizationType = 1;
    int dropDownLightModeThemeColorizationType = 1;
    DWORD dropDownDarkModeBorderColor = 0xFF2B2B2B;
    DWORD dropDownLightModeBorderColor = 0xFFDDDDDD;
    DWORD dropDownDarkModeGradientColor = 0x412B2B2B;
    DWORD dropDownLightModeGradientColor = 0x9EDDDDDD;
    int dropDownDisabled = 2;
    int dropDownFadeOutTime = 350;
    int dropDownPopInTime = 250;
    int dropDownFadeInTime = 87;
    int dropDownPopInStyle = 0;
    int dropDownStartRatio = 50;
    int dropDownEnableImmediateInterupting = 0;

    int menuNoSystemDropShadow = 0;
    int menuEnableImmersiveStyle = 1;
    int menuEnableCustomRendering = 0;
    int menuEnableFluentAnimation = 0;
    int menuEnableCompatibilityMode = 0;
    int menuNoModernAppBackgroundColor = 1;
    int menuColorTreatAsTransparentEnabled = 0;
    DWORD menuColorTreatAsTransparent = 0x00000000;
    int menuColorTreatAsTransparentThreshold = 50;
    int menuEffectType = 9;
    int menuCornerType = 4;
    int menuEnableDropShadow = 2;
    int menuNoBorderColor = 2;
    int menuEnableThemeColorization = 2;
    int menuDarkModeThemeColorizationType = 1;
    int menuLightModeThemeColorizationType = 1;
    DWORD menuDarkModeBorderColor = 0xFF2B2B2B;
    DWORD menuLightModeBorderColor = 0xFFDDDDDD;
    DWORD menuDarkModeGradientColor = 0x412B2B2B;
    DWORD menuLightModeGradientColor = 0x9EDDDDDD;
    int menuDisabled = 2;
    int menuFadeOutTime = 350;
    int menuPopInTime = 250;
    int menuFadeInTime = 87;
    int menuPopInStyle = 0;
    int menuStartRatio = 50;
    int menuEnableImmediateInterupting = 0;

    int menuSeparatorDisabled = 0;
    int menuSeparatorWidth = 1000;
    DWORD menuSeparatorDarkModeColor = 0x30D9D9D9;
    DWORD menuSeparatorLightModeColor = 0x30262626;
    int menuSeparatorEnableThemeColorization = 0;
    int menuSeparatorDarkThemeColorizationType = 1;
    int menuSeparatorLightThemeColorizationType = 1;

    int menuFocusingDisabled = 0;
    int menuFocusingCornerRadius = 8;
    int menuFocusingWidth = 1000;
    DWORD menuFocusingDarkModeColor = 0xFFFFFFFF;
    DWORD menuFocusingLightModeColor = 0xFF000000;
    int menuFocusingEnableThemeColorization = 0;
    int menuFocusingDarkThemeColorizationType = 1;
    int menuFocusingLightThemeColorizationType = 1;

    int menuDisabledHotDisabled = 0;
    int menuDisabledHotCornerRadius = 8;
    DWORD menuDisabledHotDarkModeColor = 0x00000000;
    DWORD menuDisabledHotLightModeColor = 0x00000000;
    int menuDisabledHotEnableThemeColorization = 0;
    int menuDisabledHotDarkThemeColorizationType = 1;
    int menuDisabledHotLightThemeColorizationType = 1;

    int menuHotDisabled = 0;
    int menuHotCornerRadius = 8;
    DWORD menuHotDarkModeColor = 0x41808080;
    DWORD menuHotLightModeColor = 0x30000000;
    int menuHotEnableThemeColorization = 0;
    int menuHotDarkThemeColorizationType = 1;
    int menuHotLightThemeColorizationType = 1;

    int tooltipNoSystemDropShadow = 0;
    int tooltipEffectType = 9;
    int tooltipCornerType = 4;
    int tooltipEnableDropShadow = 2;
    int tooltipNoBorderColor = 2;
    int tooltipEnableThemeColorization = 2;
    int tooltipDarkModeThemeColorizationType = 1;
    int tooltipLightModeThemeColorizationType = 1;
    DWORD tooltipDarkModeBorderColor = 0xFF2B2B2B;
    DWORD tooltipLightModeBorderColor = 0xFFDDDDDD;
    DWORD tooltipDarkModeGradientColor = 0x412B2B2B;
    DWORD tooltipLightModeGradientColor = 0x9EDDDDDD;
    DWORD tooltipDarkModeColor = 0xFFFFFFFF;
    DWORD tooltipLightModeColor = 0xFF1A1A1A;
    int tooltipMarginsType = 0;
    int tooltipMarginLeft = 6;
    int tooltipMarginRight = 6;
    int tooltipMarginTop = 6;
    int tooltipMarginBottom = 6;
    int tooltipDisabled = 2;
    std::vector<std::wstring> processBlockList;
    std::vector<std::wstring> processDisabledList;
    std::vector<std::wstring> menuProcessDisabledList;
    std::vector<std::wstring> tooltipProcessDisabledList;
    std::vector<std::wstring> dropDownProcessDisabledList;
    int resetAction = 0;

    bool confirmReset = true;

    bool hardReloadOnApply = false;

    bool configAppCompatibilityMode = false;
};

static Settings g_settings;

static DWORD ClampDword(int v, int lo, int hi)
{
    if (v < lo) {
        return static_cast<DWORD>(lo);
    }
    if (v > hi) {
        return static_cast<DWORD>(hi);
    }
    return static_cast<DWORD>(v);
}

static bool WriteDwordHKCU(const wchar_t* subKey, const wchar_t* valueName, DWORD value)
{
    HKEY hKey = nullptr;
    LONG rc = RegCreateKeyExW(
        HKEY_CURRENT_USER,
        subKey,
        0,
        nullptr,
        REG_OPTION_NON_VOLATILE,
        KEY_SET_VALUE,
        nullptr,
        &hKey,
        nullptr);

    if (rc != ERROR_SUCCESS) {
        Wh_Log(L"RegCreateKeyExW failed for %s (rc=%ld)", subKey, rc);
        return false;
    }

    rc = RegSetValueExW(
        hKey,
        valueName,
        0,
        REG_DWORD,
        reinterpret_cast<const BYTE*>(&value),
        sizeof(value));

    RegCloseKey(hKey);

    if (rc != ERROR_SUCCESS) {
        Wh_Log(L"RegSetValueExW failed for %s\\%s (rc=%ld)", subKey, valueName, rc);
        return false;
    }

    return true;
}

static bool DeleteValueHKCU(const wchar_t* subKey, const wchar_t* valueName)
{
    HKEY hKey = nullptr;
    LONG rc = RegOpenKeyExW(HKEY_CURRENT_USER, subKey, 0, KEY_SET_VALUE, &hKey);
    if (rc != ERROR_SUCCESS) {
        // Missing key is fine for fallback behavior.
        return true;
    }

    rc = RegDeleteValueW(hKey, valueName);
    RegCloseKey(hKey);

    // Missing value is expected when already inherited from global.
    return (rc == ERROR_SUCCESS || rc == ERROR_FILE_NOT_FOUND);
}

static bool WriteStringHKCU(const wchar_t* subKey, const wchar_t* valueName, const wchar_t* value)
{
    HKEY hKey = nullptr;
    LONG rc = RegCreateKeyExW(
        HKEY_CURRENT_USER,
        subKey,
        0,
        nullptr,
        REG_OPTION_NON_VOLATILE,
        KEY_SET_VALUE,
        nullptr,
        &hKey,
        nullptr);

    if (rc != ERROR_SUCCESS) {
        return false;
    }

    const DWORD bytes = static_cast<DWORD>((wcslen(value) + 1) * sizeof(wchar_t));
    rc = RegSetValueExW(hKey, valueName, 0, REG_SZ, reinterpret_cast<const BYTE*>(value), bytes);
    RegCloseKey(hKey);
    return rc == ERROR_SUCCESS;
}

  static bool SyncProcessListHKCU(const wchar_t* subKey, const std::vector<std::wstring>& processNames)
  {
    HKEY hKey = nullptr;
    LONG rc = RegCreateKeyExW(
      HKEY_CURRENT_USER,
      subKey,
      0,
      nullptr,
      REG_OPTION_NON_VOLATILE,
      KEY_QUERY_VALUE | KEY_SET_VALUE,
      nullptr,
      &hKey,
      nullptr);

    if (rc != ERROR_SUCCESS) {
      Wh_Log(L"RegCreateKeyExW failed for %s (rc=%ld)", subKey, rc);
      return false;
    }

    bool success = true;
    std::vector<std::wstring> existingValueNames;
    DWORD index = 0;
    while (true) {
      wchar_t valueName[512] = {};
      DWORD valueNameLen = _countof(valueName);
      const LONG enumRc = RegEnumValueW(
        hKey,
        index,
        valueName,
        &valueNameLen,
        nullptr,
        nullptr,
        nullptr,
        nullptr);

      if (enumRc == ERROR_NO_MORE_ITEMS) {
        break;
      }

      if (enumRc == ERROR_MORE_DATA) {
        Wh_Log(L"RegEnumValueW value name too long under %s (index=%lu)", subKey, index);
        success = false;
        ++index;
        continue;
      }

      if (enumRc != ERROR_SUCCESS) {
        Wh_Log(L"RegEnumValueW failed under %s (index=%lu, rc=%ld)", subKey, index, enumRc);
        success = false;
        break;
      }

      // Keep default value untouched (empty value name).
      if (valueNameLen > 0) {
        existingValueNames.emplace_back(valueName, valueNameLen);
      }

      ++index;
    }

    for (const auto& valueName : existingValueNames) {
      const LONG deleteRc = RegDeleteValueW(hKey, valueName.c_str());
      if (deleteRc != ERROR_SUCCESS && deleteRc != ERROR_FILE_NOT_FOUND) {
        Wh_Log(L"RegDeleteValueW failed for %s\\%s (rc=%ld)", subKey, valueName.c_str(), deleteRc);
        success = false;
      }
    }

    const DWORD enabled = 1;
    for (const auto& processName : processNames) {
      const LONG setRc = RegSetValueExW(
        hKey,
        processName.c_str(),
        0,
        REG_DWORD,
        reinterpret_cast<const BYTE*>(&enabled),
        sizeof(enabled));
      if (setRc != ERROR_SUCCESS) {
        Wh_Log(L"RegSetValueExW failed for %s\\%s (rc=%ld)", subKey, processName.c_str(), setRc);
        success = false;
      }
    }

    RegCloseKey(hKey);
    return success;
  }

static const wchar_t* ThemeColorizationTypeToName(int index)
{
    static const wchar_t* kTypes[] = {
        L"ImmersiveStartBackground",
        L"ImmersiveStartHoverBackground",
        L"ImmersiveSystemAccent",
        L"ImmersiveSystemAccentDark1",
        L"ImmersiveSystemAccentDark2",
        L"ImmersiveSystemAccentDark3",
        L"ImmersiveSystemAccentLight1",
        L"ImmersiveSystemAccentLight2",
        L"ImmersiveSystemAccentLight3"
    };

    if (index < 0 || index > 8) {
        return kTypes[1];
    }
    return kTypes[index];
}

static void SetOrDeleteDword(const wchar_t* subKey, const wchar_t* valueName, int value, int useGlobalSentinel)
{
    if (value == useGlobalSentinel) {
        DeleteValueHKCU(subKey, valueName);
    } else {
        WriteDwordHKCU(subKey, valueName, static_cast<DWORD>(value));
    }
}

static void SetOrDeleteColorDword(const wchar_t* subKey, const wchar_t* valueName, DWORD value, int mode, int useGlobalSentinel)
{
    if (mode == useGlobalSentinel) {
        DeleteValueHKCU(subKey, valueName);
    } else {
        WriteDwordHKCU(subKey, valueName, value);
    }
}

static void SetOrDeleteZeroColorDword(const wchar_t* subKey, const wchar_t* valueName, DWORD value)
{
  // Translucent Flyouts Config stores colors as unpadded hex strings.
  // A DWORD value of 0 becomes "0", which that parser can't safely handle.
  if (value == 0) {
    DeleteValueHKCU(subKey, valueName);
  } else {
    WriteDwordHKCU(subKey, valueName, value);
  }
}

static void SetOrDeleteThemeTypes(const wchar_t* subKey, int enableThemeColorizationMode, int useGlobalSentinel, int darkType, int lightType)
{
    if (enableThemeColorizationMode == useGlobalSentinel) {
        DeleteValueHKCU(subKey, L"DarkMode_ThemeColorizationType");
        DeleteValueHKCU(subKey, L"LightMode_ThemeColorizationType");
    } else {
        WriteStringHKCU(subKey, L"DarkMode_ThemeColorizationType", ThemeColorizationTypeToName(darkType));
        WriteStringHKCU(subKey, L"LightMode_ThemeColorizationType", ThemeColorizationTypeToName(lightType));
    }
}

static void NotifyTranslucentFlyoutsReload()
{
    // Fast path: notify listeners without forcing full TF detach/attach cycle.
    PostMessageW(
        HWND_BROADCAST,
        WM_SETTINGCHANGE,
        0,
        reinterpret_cast<LPARAM>(L"Software\\TranslucentFlyouts"));

    if (g_settings.hardReloadOnApply) {
        UINT msgDetach = RegisterWindowMessageW(L"TranslucentFlyouts.Detach");
        UINT msgAttach = RegisterWindowMessageW(L"TranslucentFlyouts.Attach");
        PostMessageW(HWND_BROADCAST, msgDetach, 0, 0);
        PostMessageW(HWND_BROADCAST, msgAttach, 0, 0);
    }
}

  // Forward declarations (RunControllerOnce is defined before these helpers).
  static void LoadSettings();
  static void ApplyResetAction(int resetAction);
  static void ApplySettingsToOriginalTranslucentFlyouts();

  static const wchar_t* ResetActionToLabel(int resetAction)
  {
    switch (resetAction) {
      case 1: return L"Global";
      case 2: return L"DropDown";
      case 3: return L"Menu";
      case 4: return L"Tooltip";
      case 5: return L"All";
      default: return L"None";
    }
  }

static constexpr const wchar_t* kLastResetActionValueName = L"last_reset_action";
static constexpr const wchar_t* kRegistryWriteConfirmedValueName = L"registry_write_confirmed";
static constexpr const wchar_t* kControllerStateSubKey = L"Software\\TranslucentFlyouts\\WindhawkController";

static int GetControllerStateInt(const wchar_t* valueName, int fallback)
{
  HKEY hKey = nullptr;
  LONG rc = RegOpenKeyExW(
    HKEY_CURRENT_USER,
    kControllerStateSubKey,
    0,
    KEY_QUERY_VALUE,
    &hKey);

  if (rc == ERROR_SUCCESS) {
    DWORD type = 0;
    DWORD data = 0;
    DWORD dataSize = sizeof(data);
    rc = RegQueryValueExW(
      hKey,
      valueName,
      nullptr,
      &type,
      reinterpret_cast<LPBYTE>(&data),
      &dataSize);
    RegCloseKey(hKey);

    if (rc == ERROR_SUCCESS && type == REG_DWORD && dataSize == sizeof(DWORD)) {
      return static_cast<int>(data);
    }
  }

  return fallback;
}

static void SetControllerStateInt(const wchar_t* valueName, int value)
{
  WriteDwordHKCU(kControllerStateSubKey, valueName, static_cast<DWORD>(value));
}
static HRESULT CALLBACK TopmostTaskDialogCallback(
  HWND hwnd,
  UINT msg,
  WPARAM,
  LPARAM,
  LONG_PTR)
{
  if (msg == TDN_CREATED) {
    SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
  }

  return S_OK;
}
static bool ConfirmWithDontShowAgain(
  bool allowUi,
  PCWSTR title,
  PCWSTR mainInstruction,
  PCWSTR message,
  PCWSTR actionButtonText,
  PCWSTR mainIcon,
  PCWSTR verificationText,
  const wchar_t* dontShowAgainIntValueName)
{
  if (!allowUi) {
    return true;
  }

  if (dontShowAgainIntValueName) {
    const int suppressed = GetControllerStateInt(dontShowAgainIntValueName, 0);
    if (suppressed != 0) {
      return true;
    }
  }

  HMODULE comctl32 = LoadLibraryW(L"comctl32.dll");
  const auto pTaskDialogIndirect = reinterpret_cast<decltype(&TaskDialogIndirect)>(
    comctl32 ? GetProcAddress(comctl32, "TaskDialogIndirect") : nullptr);

  if (pTaskDialogIndirect) {
    TASKDIALOG_BUTTON actionButton{};
    actionButton.nButtonID = IDOK;
    actionButton.pszButtonText = actionButtonText ? actionButtonText : L"Continue";

    TASKDIALOGCONFIG config{};
    config.cbSize = sizeof(config);
    config.hwndParent = nullptr;
    config.pfCallback = TopmostTaskDialogCallback;
    config.dwFlags = TDF_ALLOW_DIALOG_CANCELLATION;
    config.dwCommonButtons = TDCBF_CANCEL_BUTTON;
    config.cButtons = 1;
    config.pButtons = &actionButton;
    config.nDefaultButton = IDCANCEL;
    config.pszWindowTitle = title;
    config.pszMainIcon = mainIcon ? mainIcon : TD_INFORMATION_ICON;
    config.pszMainInstruction = mainInstruction ? mainInstruction : title;
    config.pszContent = message;
    config.pszVerificationText = verificationText;

    int button = 0;
    BOOL verificationChecked = FALSE;
    HRESULT hr = pTaskDialogIndirect(
      &config,
      &button,
      nullptr,
      verificationText ? &verificationChecked : nullptr);

    if (comctl32) {
      FreeLibrary(comctl32);
    }

    if (SUCCEEDED(hr)) {
      if (button == IDOK) {
        if (dontShowAgainIntValueName) {
          if (!verificationText || verificationChecked) {
            SetControllerStateInt(dontShowAgainIntValueName, 1);
          }
        }
        return true;
      }

      return false;
    }
  }

  if (comctl32) {
    FreeLibrary(comctl32);
  }

  UINT fallbackIcon = MB_ICONINFORMATION;
  if (mainIcon == TD_WARNING_ICON) {
    fallbackIcon = MB_ICONWARNING;
  }

  const int response = MessageBoxW(
    nullptr,
    message ? message : (mainInstruction ? mainInstruction : title),
    title,
    fallbackIcon | MB_OKCANCEL | MB_DEFBUTTON2);

  return response == IDOK;
}
static bool ShouldProceedWithRegistryWritePrompt(bool allowUi)
{
  if (!allowUi) {
    return true;
  }

  const bool confirmed = ConfirmWithDontShowAgain(
    allowUi,
    L"Translucent Flyouts Controller",
    L"Apply settings to TranslucentFlyouts?",
    L"This will write settings to HKCU\\Software\\TranslucentFlyouts and trigger a reload.",
    L"Apply",
    TD_INFORMATION_ICON,
    nullptr,
    kRegistryWriteConfirmedValueName);

  return confirmed;
}
static bool MaybeApplyResetActionEdgeTriggered(bool allowUi)
{
  const int lastResetAction = GetControllerStateInt(kLastResetActionValueName, 0);

  // Reset action is intentionally edge-triggered to avoid repeated resets.
  if (g_settings.resetAction == 0) {
    if (lastResetAction != 0) {
      SetControllerStateInt(kLastResetActionValueName, 0);
    }
    return false;
  }

  if (g_settings.resetAction == lastResetAction) {
    return false;
  }

  if (allowUi && g_settings.confirmReset) {
    const bool confirmed = ConfirmWithDontShowAgain(
      allowUi,
      L"Translucent Flyouts Controller",
      L"Are you sure you want to reset to defaults?",
      L"This will restore the selected category to its default values.",
      L"Reset",
      TD_WARNING_ICON,
      nullptr,
      nullptr);

    if (!confirmed) {
      // Consume this edge so a canceled reset doesn't keep prompting until the
      // user explicitly changes the reset selector again.
      SetControllerStateInt(kLastResetActionValueName, g_settings.resetAction);
      return false;
    }
  }

  ApplyResetAction(g_settings.resetAction);
  SetControllerStateInt(kLastResetActionValueName, g_settings.resetAction);
  Wh_Log(L"Tool process: applied reset action=%d (%s)", g_settings.resetAction, ResetActionToLabel(g_settings.resetAction));

  if (g_settings.resetAction == 5) {
    // "Reset All" is treated as a full reset, including one-time prompt state.
    SetControllerStateInt(kRegistryWriteConfirmedValueName, 0);
  }

  return true;
}

static bool ApplySettingsOnceInToolProcess(bool allowUi)
{
    LoadSettings();

  if (!ShouldProceedWithRegistryWritePrompt(allowUi)) {
    return false;
  }

  const bool didReset = MaybeApplyResetActionEdgeTriggered(allowUi);

  // If the user chose a reset action, we still want to write the registry so
  // TranslucentFlyouts picks up the default values.

    ApplySettingsToOriginalTranslucentFlyouts();

    (void)didReset;
    return true;
}

struct SettingChoice {
    const wchar_t* key;
    int value;
};

static int GetMappedIntSetting(const wchar_t* name, const SettingChoice* choices, size_t count, int fallback)
{
    PCWSTR value = Wh_GetStringSetting(name);
    if (!value) {
        return fallback;
    }

    for (size_t i = 0; i < count; ++i) {
        if (wcscmp(value, choices[i].key) == 0) {
            Wh_FreeStringSetting(value);
            return choices[i].value;
        }
    }

    Wh_FreeStringSetting(value);
    return fallback;
}

  static int GetBoolSettingCompat(const wchar_t* name, int fallback)
  {
    PCWSTR value = Wh_GetStringSetting(name);
    if (value) {
      if (*value) {
        if (_wcsicmp(value, L"yes") == 0 || _wcsicmp(value, L"true") == 0 || wcscmp(value, L"1") == 0) {
          Wh_FreeStringSetting(value);
          return 1;
        }

        if (_wcsicmp(value, L"no") == 0 || _wcsicmp(value, L"false") == 0 || wcscmp(value, L"0") == 0) {
          Wh_FreeStringSetting(value);
          return 0;
        }
      }

      Wh_FreeStringSetting(value);
    }

    int v = Wh_GetIntSetting(name);
    if (v == 0 || v == 1) {
      return v;
    }

    return fallback;
  }

static void TrimWhitespaceInPlace(std::wstring* text)
{
  if (!text) {
    return;
  }

  size_t first = 0;
  while (first < text->size() && iswspace((*text)[first])) {
    ++first;
  }

  size_t last = text->size();
  while (last > first && iswspace((*text)[last - 1])) {
    --last;
  }

  if (first == 0 && last == text->size()) {
    return;
  }

  *text = text->substr(first, last - first);
}

static void AddProcessListEntry(std::vector<std::wstring>* list, std::wstring token)
{
  if (!list) {
    return;
  }

  TrimWhitespaceInPlace(&token);
  if (token.empty()) {
    return;
  }

  for (wchar_t& ch : token) {
    ch = static_cast<wchar_t>(towlower(ch));
  }

  if (token.find(L'.') == std::wstring::npos) {
    token += L".exe";
  }

  for (const auto& existing : *list) {
    if (_wcsicmp(existing.c_str(), token.c_str()) == 0) {
      return;
    }
  }

  list->push_back(std::move(token));
}

static std::vector<std::wstring> ParseProcessListSetting(const wchar_t* name)
{
  std::vector<std::wstring> processList;

  PCWSTR value = Wh_GetStringSetting(name);
  if (!value) {
    return processList;
  }

  std::wstring token;
  for (const wchar_t* p = value; *p; ++p) {
    const wchar_t ch = *p;
    if (ch == L',' || ch == L';' || ch == L'\n' || ch == L'\r' || ch == L'\t' || ch == L'|') {
      AddProcessListEntry(&processList, token);
      token.clear();
      continue;
    }

    token.push_back(ch);
  }

  AddProcessListEntry(&processList, token);
  Wh_FreeStringSetting(value);
  return processList;
}

static std::vector<std::wstring> ParseProcessListSettingWithLegacy(const wchar_t* primaryName, const wchar_t* legacyName)
{
  std::vector<std::wstring> processList = ParseProcessListSetting(primaryName);
  if (processList.empty() && legacyName) {
    processList = ParseProcessListSetting(legacyName);
  }

  return processList;
}

static bool TryParseColorValue(const wchar_t* value, DWORD* out)
{
    if (!value || !out) {
        return false;
    }

    const wchar_t* p = value;
    while (*p && iswspace(*p)) {
        ++p;
    }

    int base = 10;
    if (p[0] == L'0' && (p[1] == L'x' || p[1] == L'X')) {
        p += 2;
        base = 16;
    }

    wchar_t* end = nullptr;
    unsigned long parsed = wcstoul(p, &end, base);
    if (end == p) {
        return false;
    }

    while (*end && iswspace(*end)) {
        ++end;
    }

    if (*end != L'\0') {
        return false;
    }

    *out = static_cast<DWORD>(parsed);
    return true;
}

static DWORD GetColorSetting(const wchar_t* name)
{
    PCWSTR value = Wh_GetStringSetting(name);
    if (value) {
        DWORD parsed = 0;
        bool ok = TryParseColorValue(value, &parsed);
        Wh_FreeStringSetting(value);
        if (ok) {
            return parsed;
        }
    }

    return static_cast<DWORD>(Wh_GetIntSetting(name));
}

static constexpr SettingChoice kTriState[] = {
    {L"no", 0},
    {L"yes", 1},
    {L"use_global", 2},
};

static constexpr SettingChoice kGlobalEffect[] = {
    {L"none", 0},
    {L"transparent", 1},
    {L"solid", 2},
    {L"blurred", 3},
    {L"acrylic", 4},
    {L"modern_acrylic", 5},
    {L"acrylic_bg", 6},
    {L"mica_bg", 7},
    {L"mica_variant", 8},
};

static constexpr SettingChoice kPartEffect[] = {
    {L"none", 0},
    {L"transparent", 1},
    {L"solid", 2},
    {L"blurred", 3},
    {L"acrylic", 4},
    {L"modern_acrylic", 5},
    {L"acrylic_bg", 6},
    {L"mica_bg", 7},
    {L"mica_variant", 8},
    {L"use_global", 9},
};

static constexpr SettingChoice kGlobalCorner[] = {
    {L"dont_change", 0},
    {L"sharp", 1},
    {L"large_round", 2},
    {L"small_round", 3},
};

static constexpr SettingChoice kPartCorner[] = {
    {L"dont_change", 0},
    {L"sharp", 1},
    {L"large_round", 2},
    {L"small_round", 3},
    {L"use_global", 4},
};

static constexpr SettingChoice kThemeColorizationType[] = {
    {L"start_background", 0},
    {L"start_hover", 1},
    {L"system_accent", 2},
    {L"accent_dark1", 3},
    {L"accent_dark2", 4},
    {L"accent_dark3", 5},
    {L"accent_light1", 6},
    {L"accent_light2", 7},
    {L"accent_light3", 8},
};

static constexpr SettingChoice kPopInStyle[] = {
    {L"slide_down", 0},
    {L"ripple", 1},
    {L"smooth_scroll", 2},
    {L"smooth_zoom", 3},
};

static constexpr SettingChoice kMarginsType[] = {
    {L"add_to_existing", 0},
    {L"replace_existing", 1},
};

static constexpr SettingChoice kResetAction[] = {
  {L"none", 0},
  {L"global", 1},
  {L"dropdown", 2},
  {L"menu", 3},
  {L"tooltip", 4},
  {L"all", 5},
};

static void LoadSettings()
{
    g_settings.globalEffectType = GetMappedIntSetting(L"global.effectType", kGlobalEffect, _countof(kGlobalEffect), 5);
    g_settings.globalCornerType = GetMappedIntSetting(L"global.cornerType", kGlobalCorner, _countof(kGlobalCorner), 3);
  g_settings.globalEnableDropShadow = GetBoolSettingCompat(L"global.enableDropShadow", 0);
  g_settings.globalNoBorderColor = GetBoolSettingCompat(L"global.noBorderColor", 0);
  g_settings.globalEnableThemeColorization = GetBoolSettingCompat(L"global.enableThemeColorization", 0);
    g_settings.globalDarkModeThemeColorizationType = GetMappedIntSetting(L"global.darkModeThemeColorizationType", kThemeColorizationType, _countof(kThemeColorizationType), 1);
    g_settings.globalLightModeThemeColorizationType = GetMappedIntSetting(L"global.lightModeThemeColorizationType", kThemeColorizationType, _countof(kThemeColorizationType), 1);
    g_settings.globalDarkModeBorderColor = GetColorSetting(L"global.darkModeBorderColor");
    g_settings.globalLightModeBorderColor = GetColorSetting(L"global.lightModeBorderColor");
    g_settings.globalDarkModeGradientColor = GetColorSetting(L"global.darkModeGradientColor");
    g_settings.globalLightModeGradientColor = GetColorSetting(L"global.lightModeGradientColor");
    g_settings.globalEnableMiniDump = GetBoolSettingCompat(L"global.enableMiniDump", 1);
    g_settings.globalDisabled = GetBoolSettingCompat(L"global.disabled", 0);

    g_settings.dropDownEffectType = GetMappedIntSetting(L"dropDown.effectType", kPartEffect, _countof(kPartEffect), 9);
    g_settings.dropDownCornerType = GetMappedIntSetting(L"dropDown.cornerType", kPartCorner, _countof(kPartCorner), 4);
    g_settings.dropDownEnableDropShadow = GetMappedIntSetting(L"dropDown.enableDropShadow", kTriState, _countof(kTriState), 2);
    g_settings.dropDownEnableFluentAnimation = GetBoolSettingCompat(L"dropDown.enableFluentAnimation", 0);
    g_settings.dropDownNoBorderColor = GetMappedIntSetting(L"dropDown.noBorderColor", kTriState, _countof(kTriState), 2);
    g_settings.dropDownEnableThemeColorization = GetMappedIntSetting(L"dropDown.enableThemeColorization", kTriState, _countof(kTriState), 2);
    g_settings.dropDownDarkModeThemeColorizationType = Wh_GetIntSetting(L"dropDown.darkModeThemeColorizationType");
    g_settings.dropDownLightModeThemeColorizationType = Wh_GetIntSetting(L"dropDown.lightModeThemeColorizationType");
    g_settings.dropDownDarkModeBorderColor = GetColorSetting(L"dropDown.darkModeBorderColor");
    g_settings.dropDownLightModeBorderColor = GetColorSetting(L"dropDown.lightModeBorderColor");
    g_settings.dropDownDarkModeGradientColor = GetColorSetting(L"dropDown.darkModeGradientColor");
    g_settings.dropDownLightModeGradientColor = GetColorSetting(L"dropDown.lightModeGradientColor");
    g_settings.dropDownDisabled = GetMappedIntSetting(L"dropDown.disabled", kTriState, _countof(kTriState), 2);
    g_settings.dropDownFadeOutTime = Wh_GetIntSetting(L"dropDown.animation_fadeOutTime");
    g_settings.dropDownPopInTime = Wh_GetIntSetting(L"dropDown.animation_popInTime");
    g_settings.dropDownFadeInTime = Wh_GetIntSetting(L"dropDown.animation_fadeInTime");
    g_settings.dropDownPopInStyle = GetMappedIntSetting(L"dropDown.animation_popInStyle", kPopInStyle, _countof(kPopInStyle), 0);
    g_settings.dropDownStartRatio = Wh_GetIntSetting(L"dropDown.animation_startRatio");
    g_settings.dropDownEnableImmediateInterupting = GetBoolSettingCompat(L"dropDown.animation_enableImmediateInterupting", 0);

    g_settings.menuNoSystemDropShadow = GetBoolSettingCompat(L"menu.noSystemDropShadow", 0);
    g_settings.menuEnableImmersiveStyle = GetBoolSettingCompat(L"menu.enableImmersiveStyle", 1);
    g_settings.menuEnableCustomRendering = GetBoolSettingCompat(L"menu.enableCustomRendering", 0);
    g_settings.menuEnableFluentAnimation = GetBoolSettingCompat(L"menu.enableFluentAnimation", 0);
    g_settings.menuEnableCompatibilityMode = GetBoolSettingCompat(L"menu.enableCompatibilityMode", 0);
    g_settings.menuNoModernAppBackgroundColor = GetBoolSettingCompat(L"menu.noModernAppBackgroundColor", 1);
    g_settings.menuColorTreatAsTransparentEnabled = Wh_GetIntSetting(L"menu.colorTreatAsTransparentEnabled");
    g_settings.menuColorTreatAsTransparent = GetColorSetting(L"menu.colorTreatAsTransparent");
    g_settings.menuColorTreatAsTransparentThreshold = Wh_GetIntSetting(L"menu.colorTreatAsTransparentThreshold");
    g_settings.menuEffectType = GetMappedIntSetting(L"menu.effectType", kPartEffect, _countof(kPartEffect), 9);
    g_settings.menuCornerType = GetMappedIntSetting(L"menu.cornerType", kPartCorner, _countof(kPartCorner), 4);
    g_settings.menuEnableDropShadow = GetMappedIntSetting(L"menu.enableDropShadow", kTriState, _countof(kTriState), 2);
    g_settings.menuNoBorderColor = GetMappedIntSetting(L"menu.noBorderColor", kTriState, _countof(kTriState), 2);
    g_settings.menuEnableThemeColorization = GetMappedIntSetting(L"menu.enableThemeColorization", kTriState, _countof(kTriState), 2);
    g_settings.menuDarkModeThemeColorizationType = Wh_GetIntSetting(L"menu.darkModeThemeColorizationType");
    g_settings.menuLightModeThemeColorizationType = Wh_GetIntSetting(L"menu.lightModeThemeColorizationType");
    g_settings.menuDarkModeBorderColor = GetColorSetting(L"menu.darkModeBorderColor");
    g_settings.menuLightModeBorderColor = GetColorSetting(L"menu.lightModeBorderColor");
    g_settings.menuDarkModeGradientColor = GetColorSetting(L"menu.darkModeGradientColor");
    g_settings.menuLightModeGradientColor = GetColorSetting(L"menu.lightModeGradientColor");
    g_settings.menuDisabled = GetMappedIntSetting(L"menu.disabled", kTriState, _countof(kTriState), 2);
    g_settings.menuFadeOutTime = Wh_GetIntSetting(L"menu.animation_fadeOutTime");
    g_settings.menuPopInTime = Wh_GetIntSetting(L"menu.animation_popInTime");
    g_settings.menuFadeInTime = Wh_GetIntSetting(L"menu.animation_fadeInTime");
    g_settings.menuPopInStyle = GetMappedIntSetting(L"menu.animation_popInStyle", kPopInStyle, _countof(kPopInStyle), 0);
    g_settings.menuStartRatio = Wh_GetIntSetting(L"menu.animation_startRatio");
    g_settings.menuEnableImmediateInterupting = GetBoolSettingCompat(L"menu.animation_enableImmediateInterupting", 0);

    g_settings.menuSeparatorDisabled = Wh_GetIntSetting(L"menu.separator_disabled");
    g_settings.menuSeparatorWidth = Wh_GetIntSetting(L"menu.separator_width");
    g_settings.menuSeparatorDarkModeColor = GetColorSetting(L"menu.separator_darkModeColor");
    g_settings.menuSeparatorLightModeColor = GetColorSetting(L"menu.separator_lightModeColor");
    g_settings.menuSeparatorEnableThemeColorization = Wh_GetIntSetting(L"menu.separator_enableThemeColorization");
    g_settings.menuSeparatorDarkThemeColorizationType = Wh_GetIntSetting(L"menu.separator_darkThemeColorizationType");
    g_settings.menuSeparatorLightThemeColorizationType = Wh_GetIntSetting(L"menu.separator_lightThemeColorizationType");

    g_settings.menuFocusingDisabled = Wh_GetIntSetting(L"menu.focusing_disabled");
    g_settings.menuFocusingCornerRadius = Wh_GetIntSetting(L"menu.focusing_cornerRadius");
    g_settings.menuFocusingWidth = Wh_GetIntSetting(L"menu.focusing_width");
    g_settings.menuFocusingDarkModeColor = GetColorSetting(L"menu.focusing_darkModeColor");
    g_settings.menuFocusingLightModeColor = GetColorSetting(L"menu.focusing_lightModeColor");
    g_settings.menuFocusingEnableThemeColorization = Wh_GetIntSetting(L"menu.focusing_enableThemeColorization");
    g_settings.menuFocusingDarkThemeColorizationType = Wh_GetIntSetting(L"menu.focusing_darkThemeColorizationType");
    g_settings.menuFocusingLightThemeColorizationType = Wh_GetIntSetting(L"menu.focusing_lightThemeColorizationType");

    g_settings.menuDisabledHotDisabled = Wh_GetIntSetting(L"menu.disabledHot_disabled");
    g_settings.menuDisabledHotCornerRadius = Wh_GetIntSetting(L"menu.disabledHot_cornerRadius");
    g_settings.menuDisabledHotDarkModeColor = GetColorSetting(L"menu.disabledHot_darkModeColor");
    g_settings.menuDisabledHotLightModeColor = GetColorSetting(L"menu.disabledHot_lightModeColor");
    g_settings.menuDisabledHotEnableThemeColorization = Wh_GetIntSetting(L"menu.disabledHot_enableThemeColorization");
    g_settings.menuDisabledHotDarkThemeColorizationType = Wh_GetIntSetting(L"menu.disabledHot_darkThemeColorizationType");
    g_settings.menuDisabledHotLightThemeColorizationType = Wh_GetIntSetting(L"menu.disabledHot_lightThemeColorizationType");

    g_settings.menuHotDisabled = Wh_GetIntSetting(L"menu.hot_disabled");
    g_settings.menuHotCornerRadius = Wh_GetIntSetting(L"menu.hot_cornerRadius");
    g_settings.menuHotDarkModeColor = GetColorSetting(L"menu.hot_darkModeColor");
    g_settings.menuHotLightModeColor = GetColorSetting(L"menu.hot_lightModeColor");
    g_settings.menuHotEnableThemeColorization = Wh_GetIntSetting(L"menu.hot_enableThemeColorization");
    g_settings.menuHotDarkThemeColorizationType = Wh_GetIntSetting(L"menu.hot_darkThemeColorizationType");
    g_settings.menuHotLightThemeColorizationType = Wh_GetIntSetting(L"menu.hot_lightThemeColorizationType");

    g_settings.tooltipNoSystemDropShadow = GetBoolSettingCompat(L"tooltip.noSystemDropShadow", 0);
    g_settings.tooltipEffectType = GetMappedIntSetting(L"tooltip.effectType", kPartEffect, _countof(kPartEffect), 9);
    g_settings.tooltipCornerType = GetMappedIntSetting(L"tooltip.cornerType", kPartCorner, _countof(kPartCorner), 4);
    g_settings.tooltipEnableDropShadow = GetMappedIntSetting(L"tooltip.enableDropShadow", kTriState, _countof(kTriState), 2);
    g_settings.tooltipNoBorderColor = GetMappedIntSetting(L"tooltip.noBorderColor", kTriState, _countof(kTriState), 2);
    g_settings.tooltipEnableThemeColorization = GetMappedIntSetting(L"tooltip.enableThemeColorization", kTriState, _countof(kTriState), 2);
    g_settings.tooltipDarkModeThemeColorizationType = Wh_GetIntSetting(L"tooltip.darkModeThemeColorizationType");
    g_settings.tooltipLightModeThemeColorizationType = Wh_GetIntSetting(L"tooltip.lightModeThemeColorizationType");
    g_settings.tooltipDarkModeBorderColor = GetColorSetting(L"tooltip.darkModeBorderColor");
    g_settings.tooltipLightModeBorderColor = GetColorSetting(L"tooltip.lightModeBorderColor");
    g_settings.tooltipDarkModeGradientColor = GetColorSetting(L"tooltip.darkModeGradientColor");
    g_settings.tooltipLightModeGradientColor = GetColorSetting(L"tooltip.lightModeGradientColor");
    g_settings.tooltipDarkModeColor = GetColorSetting(L"tooltip.darkModeColor");
    g_settings.tooltipLightModeColor = GetColorSetting(L"tooltip.lightModeColor");
    g_settings.tooltipMarginsType = GetMappedIntSetting(L"tooltip.marginsType", kMarginsType, _countof(kMarginsType), 0);
    g_settings.tooltipMarginLeft = Wh_GetIntSetting(L"tooltip.marginLeft");
    g_settings.tooltipMarginRight = Wh_GetIntSetting(L"tooltip.marginRight");
    g_settings.tooltipMarginTop = Wh_GetIntSetting(L"tooltip.marginTop");
    g_settings.tooltipMarginBottom = Wh_GetIntSetting(L"tooltip.marginBottom");
    g_settings.tooltipDisabled = GetMappedIntSetting(L"tooltip.disabled", kTriState, _countof(kTriState), 2);
    g_settings.processBlockList = ParseProcessListSettingWithLegacy(L"advancedFunctions.processBlockList", L"controller.processBlockList");
    g_settings.processDisabledList = ParseProcessListSettingWithLegacy(L"advancedFunctions.processDisabledList", L"controller.processDisabledList");
    g_settings.menuProcessDisabledList = ParseProcessListSettingWithLegacy(L"advancedFunctions.menuProcessDisabledList", L"controller.menuProcessDisabledList");
    g_settings.tooltipProcessDisabledList = ParseProcessListSettingWithLegacy(L"advancedFunctions.tooltipProcessDisabledList", L"controller.tooltipProcessDisabledList");
    g_settings.dropDownProcessDisabledList = ParseProcessListSettingWithLegacy(L"advancedFunctions.dropDownProcessDisabledList", L"controller.dropDownProcessDisabledList");
    g_settings.resetAction = GetMappedIntSetting(L"controller.resetAction", kResetAction, _countof(kResetAction), 0);

    g_settings.confirmReset = (Wh_GetIntSetting(L"controller.confirmReset") != 0);

    g_settings.hardReloadOnApply = (Wh_GetIntSetting(L"controller.hardReloadOnApply") != 0);

    g_settings.configAppCompatibilityMode = (Wh_GetIntSetting(L"controller.configAppCompatibilityMode") != 0);

    g_settings.globalEffectType = static_cast<int>(ClampDword(g_settings.globalEffectType, 0, 8));
    g_settings.globalCornerType = static_cast<int>(ClampDword(g_settings.globalCornerType, 0, 3));
    g_settings.dropDownPopInStyle = static_cast<int>(ClampDword(g_settings.dropDownPopInStyle, 0, 3));
    g_settings.menuPopInStyle = static_cast<int>(ClampDword(g_settings.menuPopInStyle, 0, 3));
}

  static void ApplyResetAction(int resetAction)
  {
    Settings defaults{};

    if (resetAction == 1 || resetAction == 5) {
      g_settings.globalEffectType = defaults.globalEffectType;
      g_settings.globalCornerType = defaults.globalCornerType;
      g_settings.globalEnableDropShadow = defaults.globalEnableDropShadow;
      g_settings.globalNoBorderColor = defaults.globalNoBorderColor;
      g_settings.globalEnableThemeColorization = defaults.globalEnableThemeColorization;
      g_settings.globalDarkModeThemeColorizationType = defaults.globalDarkModeThemeColorizationType;
      g_settings.globalLightModeThemeColorizationType = defaults.globalLightModeThemeColorizationType;
      g_settings.globalDarkModeBorderColor = defaults.globalDarkModeBorderColor;
      g_settings.globalLightModeBorderColor = defaults.globalLightModeBorderColor;
      g_settings.globalDarkModeGradientColor = defaults.globalDarkModeGradientColor;
      g_settings.globalLightModeGradientColor = defaults.globalLightModeGradientColor;
      g_settings.globalEnableMiniDump = defaults.globalEnableMiniDump;
      g_settings.globalDisabled = defaults.globalDisabled;
    }

    if (resetAction == 2 || resetAction == 5) {
      g_settings.dropDownEffectType = defaults.dropDownEffectType;
      g_settings.dropDownCornerType = defaults.dropDownCornerType;
      g_settings.dropDownEnableDropShadow = defaults.dropDownEnableDropShadow;
      g_settings.dropDownEnableFluentAnimation = defaults.dropDownEnableFluentAnimation;
      g_settings.dropDownNoBorderColor = defaults.dropDownNoBorderColor;
      g_settings.dropDownEnableThemeColorization = defaults.dropDownEnableThemeColorization;
      g_settings.dropDownDarkModeThemeColorizationType = defaults.dropDownDarkModeThemeColorizationType;
      g_settings.dropDownLightModeThemeColorizationType = defaults.dropDownLightModeThemeColorizationType;
      g_settings.dropDownDarkModeBorderColor = defaults.dropDownDarkModeBorderColor;
      g_settings.dropDownLightModeBorderColor = defaults.dropDownLightModeBorderColor;
      g_settings.dropDownDarkModeGradientColor = defaults.dropDownDarkModeGradientColor;
      g_settings.dropDownLightModeGradientColor = defaults.dropDownLightModeGradientColor;
      g_settings.dropDownDisabled = defaults.dropDownDisabled;
      g_settings.dropDownFadeOutTime = defaults.dropDownFadeOutTime;
      g_settings.dropDownPopInTime = defaults.dropDownPopInTime;
      g_settings.dropDownFadeInTime = defaults.dropDownFadeInTime;
      g_settings.dropDownPopInStyle = defaults.dropDownPopInStyle;
      g_settings.dropDownStartRatio = defaults.dropDownStartRatio;
      g_settings.dropDownEnableImmediateInterupting = defaults.dropDownEnableImmediateInterupting;
    }

    if (resetAction == 3 || resetAction == 5) {
      g_settings.menuNoSystemDropShadow = defaults.menuNoSystemDropShadow;
      g_settings.menuEnableImmersiveStyle = defaults.menuEnableImmersiveStyle;
      g_settings.menuEnableCustomRendering = defaults.menuEnableCustomRendering;
      g_settings.menuEnableFluentAnimation = defaults.menuEnableFluentAnimation;
      g_settings.menuEnableCompatibilityMode = defaults.menuEnableCompatibilityMode;
      g_settings.menuNoModernAppBackgroundColor = defaults.menuNoModernAppBackgroundColor;
      g_settings.menuColorTreatAsTransparentEnabled = defaults.menuColorTreatAsTransparentEnabled;
      g_settings.menuColorTreatAsTransparent = defaults.menuColorTreatAsTransparent;
      g_settings.menuColorTreatAsTransparentThreshold = defaults.menuColorTreatAsTransparentThreshold;
      g_settings.menuEffectType = defaults.menuEffectType;
      g_settings.menuCornerType = defaults.menuCornerType;
      g_settings.menuEnableDropShadow = defaults.menuEnableDropShadow;
      g_settings.menuNoBorderColor = defaults.menuNoBorderColor;
      g_settings.menuEnableThemeColorization = defaults.menuEnableThemeColorization;
      g_settings.menuDarkModeThemeColorizationType = defaults.menuDarkModeThemeColorizationType;
      g_settings.menuLightModeThemeColorizationType = defaults.menuLightModeThemeColorizationType;
      g_settings.menuDarkModeBorderColor = defaults.menuDarkModeBorderColor;
      g_settings.menuLightModeBorderColor = defaults.menuLightModeBorderColor;
      g_settings.menuDarkModeGradientColor = defaults.menuDarkModeGradientColor;
      g_settings.menuLightModeGradientColor = defaults.menuLightModeGradientColor;
      g_settings.menuDisabled = defaults.menuDisabled;
      g_settings.menuFadeOutTime = defaults.menuFadeOutTime;
      g_settings.menuPopInTime = defaults.menuPopInTime;
      g_settings.menuFadeInTime = defaults.menuFadeInTime;
      g_settings.menuPopInStyle = defaults.menuPopInStyle;
      g_settings.menuStartRatio = defaults.menuStartRatio;
      g_settings.menuEnableImmediateInterupting = defaults.menuEnableImmediateInterupting;
      g_settings.menuSeparatorDisabled = defaults.menuSeparatorDisabled;
      g_settings.menuSeparatorWidth = defaults.menuSeparatorWidth;
      g_settings.menuSeparatorDarkModeColor = defaults.menuSeparatorDarkModeColor;
      g_settings.menuSeparatorLightModeColor = defaults.menuSeparatorLightModeColor;
      g_settings.menuSeparatorEnableThemeColorization = defaults.menuSeparatorEnableThemeColorization;
      g_settings.menuSeparatorDarkThemeColorizationType = defaults.menuSeparatorDarkThemeColorizationType;
      g_settings.menuSeparatorLightThemeColorizationType = defaults.menuSeparatorLightThemeColorizationType;
      g_settings.menuFocusingDisabled = defaults.menuFocusingDisabled;
      g_settings.menuFocusingCornerRadius = defaults.menuFocusingCornerRadius;
      g_settings.menuFocusingWidth = defaults.menuFocusingWidth;
      g_settings.menuFocusingDarkModeColor = defaults.menuFocusingDarkModeColor;
      g_settings.menuFocusingLightModeColor = defaults.menuFocusingLightModeColor;
      g_settings.menuFocusingEnableThemeColorization = defaults.menuFocusingEnableThemeColorization;
      g_settings.menuFocusingDarkThemeColorizationType = defaults.menuFocusingDarkThemeColorizationType;
      g_settings.menuFocusingLightThemeColorizationType = defaults.menuFocusingLightThemeColorizationType;
      g_settings.menuDisabledHotDisabled = defaults.menuDisabledHotDisabled;
      g_settings.menuDisabledHotCornerRadius = defaults.menuDisabledHotCornerRadius;
      g_settings.menuDisabledHotDarkModeColor = defaults.menuDisabledHotDarkModeColor;
      g_settings.menuDisabledHotLightModeColor = defaults.menuDisabledHotLightModeColor;
      g_settings.menuDisabledHotEnableThemeColorization = defaults.menuDisabledHotEnableThemeColorization;
      g_settings.menuDisabledHotDarkThemeColorizationType = defaults.menuDisabledHotDarkThemeColorizationType;
      g_settings.menuDisabledHotLightThemeColorizationType = defaults.menuDisabledHotLightThemeColorizationType;
      g_settings.menuHotDisabled = defaults.menuHotDisabled;
      g_settings.menuHotCornerRadius = defaults.menuHotCornerRadius;
      g_settings.menuHotDarkModeColor = defaults.menuHotDarkModeColor;
      g_settings.menuHotLightModeColor = defaults.menuHotLightModeColor;
      g_settings.menuHotEnableThemeColorization = defaults.menuHotEnableThemeColorization;
      g_settings.menuHotDarkThemeColorizationType = defaults.menuHotDarkThemeColorizationType;
      g_settings.menuHotLightThemeColorizationType = defaults.menuHotLightThemeColorizationType;
    }

    if (resetAction == 4 || resetAction == 5) {
      g_settings.tooltipNoSystemDropShadow = defaults.tooltipNoSystemDropShadow;
      g_settings.tooltipEffectType = defaults.tooltipEffectType;
      g_settings.tooltipCornerType = defaults.tooltipCornerType;
      g_settings.tooltipEnableDropShadow = defaults.tooltipEnableDropShadow;
      g_settings.tooltipNoBorderColor = defaults.tooltipNoBorderColor;
      g_settings.tooltipEnableThemeColorization = defaults.tooltipEnableThemeColorization;
      g_settings.tooltipDarkModeThemeColorizationType = defaults.tooltipDarkModeThemeColorizationType;
      g_settings.tooltipLightModeThemeColorizationType = defaults.tooltipLightModeThemeColorizationType;
      g_settings.tooltipDarkModeBorderColor = defaults.tooltipDarkModeBorderColor;
      g_settings.tooltipLightModeBorderColor = defaults.tooltipLightModeBorderColor;
      g_settings.tooltipDarkModeGradientColor = defaults.tooltipDarkModeGradientColor;
      g_settings.tooltipLightModeGradientColor = defaults.tooltipLightModeGradientColor;
      g_settings.tooltipDarkModeColor = defaults.tooltipDarkModeColor;
      g_settings.tooltipLightModeColor = defaults.tooltipLightModeColor;
      g_settings.tooltipMarginsType = defaults.tooltipMarginsType;
      g_settings.tooltipMarginLeft = defaults.tooltipMarginLeft;
      g_settings.tooltipMarginRight = defaults.tooltipMarginRight;
      g_settings.tooltipMarginTop = defaults.tooltipMarginTop;
      g_settings.tooltipMarginBottom = defaults.tooltipMarginBottom;
      g_settings.tooltipDisabled = defaults.tooltipDisabled;
    }
  }

static void ApplySettingsToOriginalTranslucentFlyouts()
{
    const wchar_t* kRoot = L"Software\\TranslucentFlyouts";
    const wchar_t* kDropDown = L"Software\\TranslucentFlyouts\\DropDown";
    const wchar_t* kDropDownAnimation = L"Software\\TranslucentFlyouts\\DropDown\\Animation";
    const wchar_t* kMenu = L"Software\\TranslucentFlyouts\\Menu";
    const wchar_t* kMenuAnimation = L"Software\\TranslucentFlyouts\\Menu\\Animation";
    const wchar_t* kMenuSeparator = L"Software\\TranslucentFlyouts\\Menu\\Separator";
    const wchar_t* kMenuFocusing = L"Software\\TranslucentFlyouts\\Menu\\Focusing";
    const wchar_t* kMenuDisabledHot = L"Software\\TranslucentFlyouts\\Menu\\DisabledHot";
    const wchar_t* kMenuHot = L"Software\\TranslucentFlyouts\\Menu\\Hot";
    const wchar_t* kTooltip = L"Software\\TranslucentFlyouts\\Tooltip";
    const wchar_t* kBlockList = L"Software\\TranslucentFlyouts\\BlockList";
    const wchar_t* kDisabledList = L"Software\\TranslucentFlyouts\\DisabledList";
    const wchar_t* kMenuDisabledList = L"Software\\TranslucentFlyouts\\Menu\\DisabledList";
    const wchar_t* kTooltipDisabledList = L"Software\\TranslucentFlyouts\\Tooltip\\DisabledList";
    const wchar_t* kDropDownDisabledList = L"Software\\TranslucentFlyouts\\DropDown\\DisabledList";

    // 1) Global
    WriteDwordHKCU(kRoot, L"EffectType", static_cast<DWORD>(g_settings.globalEffectType));
    WriteDwordHKCU(kRoot, L"CornerType", static_cast<DWORD>(g_settings.globalCornerType));
    WriteDwordHKCU(kRoot, L"EnableDropShadow", static_cast<DWORD>(g_settings.globalEnableDropShadow));
    WriteDwordHKCU(kRoot, L"NoBorderColor", static_cast<DWORD>(g_settings.globalNoBorderColor));
    WriteDwordHKCU(kRoot, L"EnableThemeColorization", static_cast<DWORD>(g_settings.globalEnableThemeColorization));
    WriteStringHKCU(kRoot, L"DarkMode_ThemeColorizationType", ThemeColorizationTypeToName(g_settings.globalDarkModeThemeColorizationType));
    WriteStringHKCU(kRoot, L"LightMode_ThemeColorizationType", ThemeColorizationTypeToName(g_settings.globalLightModeThemeColorizationType));
    WriteDwordHKCU(kRoot, L"DarkMode_BorderColor", g_settings.globalDarkModeBorderColor);
    WriteDwordHKCU(kRoot, L"LightMode_BorderColor", g_settings.globalLightModeBorderColor);
    WriteDwordHKCU(kRoot, L"DarkMode_GradientColor", g_settings.globalDarkModeGradientColor);
    WriteDwordHKCU(kRoot, L"LightMode_GradientColor", g_settings.globalLightModeGradientColor);
    WriteDwordHKCU(kRoot, L"EnableMiniDump", static_cast<DWORD>(g_settings.globalEnableMiniDump));
    WriteDwordHKCU(kRoot, L"Disabled", static_cast<DWORD>(g_settings.globalDisabled));

    // 2) DropDown
    SetOrDeleteDword(kDropDown, L"EffectType", g_settings.dropDownEffectType, 9);
    SetOrDeleteDword(kDropDown, L"CornerType", g_settings.dropDownCornerType, 4);
    SetOrDeleteDword(kDropDown, L"EnableDropShadow", g_settings.dropDownEnableDropShadow, 2);
    WriteDwordHKCU(kDropDown, L"EnableFluentAnimation", static_cast<DWORD>(g_settings.dropDownEnableFluentAnimation));
    SetOrDeleteDword(kDropDown, L"NoBorderColor", g_settings.dropDownNoBorderColor, 2);
    SetOrDeleteDword(kDropDown, L"EnableThemeColorization", g_settings.dropDownEnableThemeColorization, 2);
    SetOrDeleteThemeTypes(
        kDropDown,
        g_settings.dropDownEnableThemeColorization,
        2,
        g_settings.dropDownDarkModeThemeColorizationType,
        g_settings.dropDownLightModeThemeColorizationType);
    SetOrDeleteColorDword(kDropDown, L"DarkMode_BorderColor", g_settings.dropDownDarkModeBorderColor, g_settings.dropDownEnableDropShadow, 2);
    SetOrDeleteColorDword(kDropDown, L"LightMode_BorderColor", g_settings.dropDownLightModeBorderColor, g_settings.dropDownEnableDropShadow, 2);
    SetOrDeleteColorDword(kDropDown, L"DarkMode_GradientColor", g_settings.dropDownDarkModeGradientColor, g_settings.dropDownEffectType, 9);
    SetOrDeleteColorDword(kDropDown, L"LightMode_GradientColor", g_settings.dropDownLightModeGradientColor, g_settings.dropDownEffectType, 9);
    SetOrDeleteDword(kDropDown, L"Disabled", g_settings.dropDownDisabled, 2);
    WriteDwordHKCU(kDropDownAnimation, L"FadeOutTime", static_cast<DWORD>(g_settings.dropDownFadeOutTime));
    WriteDwordHKCU(kDropDownAnimation, L"PopInTime", static_cast<DWORD>(g_settings.dropDownPopInTime));
    WriteDwordHKCU(kDropDownAnimation, L"FadeInTime", static_cast<DWORD>(g_settings.dropDownFadeInTime));
    WriteDwordHKCU(kDropDownAnimation, L"PopInStyle", static_cast<DWORD>(g_settings.dropDownPopInStyle));
    WriteDwordHKCU(kDropDownAnimation, L"StartRatio", static_cast<DWORD>(g_settings.dropDownStartRatio));
    WriteDwordHKCU(kDropDownAnimation, L"EnableImmediateInterupting", static_cast<DWORD>(g_settings.dropDownEnableImmediateInterupting));

    // 3) Menu
    WriteDwordHKCU(kMenu, L"NoSystemDropShadow", static_cast<DWORD>(g_settings.menuNoSystemDropShadow));
    WriteDwordHKCU(kMenu, L"EnableImmersiveStyle", static_cast<DWORD>(g_settings.menuEnableImmersiveStyle));
    WriteDwordHKCU(kMenu, L"EnableCustomRendering", static_cast<DWORD>(g_settings.menuEnableCustomRendering));
    WriteDwordHKCU(kMenu, L"EnableFluentAnimation", static_cast<DWORD>(g_settings.menuEnableFluentAnimation));
    WriteDwordHKCU(kMenu, L"EnableCompatibilityMode", static_cast<DWORD>(g_settings.menuEnableCompatibilityMode));
    WriteDwordHKCU(kMenu, L"NoModernAppBackgroundColor", static_cast<DWORD>(g_settings.menuNoModernAppBackgroundColor));
    if (g_settings.menuColorTreatAsTransparentEnabled) {
      if (g_settings.configAppCompatibilityMode && g_settings.menuColorTreatAsTransparent == 0) {
        DeleteValueHKCU(kMenu, L"ColorTreatAsTransparent");
      } else {
        WriteDwordHKCU(kMenu, L"ColorTreatAsTransparent", g_settings.menuColorTreatAsTransparent);
      }
    } else {
      DeleteValueHKCU(kMenu, L"ColorTreatAsTransparent");
    }
    WriteDwordHKCU(kMenu, L"ColorTreatAsTransparentThreshold", static_cast<DWORD>(g_settings.menuColorTreatAsTransparentThreshold));

    SetOrDeleteDword(kMenu, L"EffectType", g_settings.menuEffectType, 9);
    SetOrDeleteDword(kMenu, L"CornerType", g_settings.menuCornerType, 4);
    SetOrDeleteDword(kMenu, L"EnableDropShadow", g_settings.menuEnableDropShadow, 2);
    SetOrDeleteDword(kMenu, L"NoBorderColor", g_settings.menuNoBorderColor, 2);
    SetOrDeleteDword(kMenu, L"EnableThemeColorization", g_settings.menuEnableThemeColorization, 2);
    SetOrDeleteThemeTypes(kMenu, g_settings.menuEnableThemeColorization, 2, g_settings.menuDarkModeThemeColorizationType, g_settings.menuLightModeThemeColorizationType);
    SetOrDeleteColorDword(kMenu, L"DarkMode_BorderColor", g_settings.menuDarkModeBorderColor, g_settings.menuEnableDropShadow, 2);
    SetOrDeleteColorDword(kMenu, L"LightMode_BorderColor", g_settings.menuLightModeBorderColor, g_settings.menuEnableDropShadow, 2);
    SetOrDeleteColorDword(kMenu, L"DarkMode_GradientColor", g_settings.menuDarkModeGradientColor, g_settings.menuEffectType, 9);
    SetOrDeleteColorDword(kMenu, L"LightMode_GradientColor", g_settings.menuLightModeGradientColor, g_settings.menuEffectType, 9);
    SetOrDeleteDword(kMenu, L"Disabled", g_settings.menuDisabled, 2);

    WriteDwordHKCU(kMenuAnimation, L"FadeOutTime", static_cast<DWORD>(g_settings.menuFadeOutTime));
    WriteDwordHKCU(kMenuAnimation, L"PopInTime", static_cast<DWORD>(g_settings.menuPopInTime));
    WriteDwordHKCU(kMenuAnimation, L"FadeInTime", static_cast<DWORD>(g_settings.menuFadeInTime));
    WriteDwordHKCU(kMenuAnimation, L"PopInStyle", static_cast<DWORD>(g_settings.menuPopInStyle));
    WriteDwordHKCU(kMenuAnimation, L"StartRatio", static_cast<DWORD>(g_settings.menuStartRatio));
    WriteDwordHKCU(kMenuAnimation, L"EnableImmediateInterupting", static_cast<DWORD>(g_settings.menuEnableImmediateInterupting));

    WriteDwordHKCU(kMenuSeparator, L"Disabled", static_cast<DWORD>(g_settings.menuSeparatorDisabled));
    WriteDwordHKCU(kMenuSeparator, L"Width", static_cast<DWORD>(g_settings.menuSeparatorWidth));
    WriteDwordHKCU(kMenuSeparator, L"DarkMode_Color", g_settings.menuSeparatorDarkModeColor);
    WriteDwordHKCU(kMenuSeparator, L"LightMode_Color", g_settings.menuSeparatorLightModeColor);
    WriteDwordHKCU(kMenuSeparator, L"EnableThemeColorization", static_cast<DWORD>(g_settings.menuSeparatorEnableThemeColorization));
    WriteStringHKCU(kMenuSeparator, L"DarkMode_ThemeColorizationType", ThemeColorizationTypeToName(g_settings.menuSeparatorDarkThemeColorizationType));
    WriteStringHKCU(kMenuSeparator, L"LightMode_ThemeColorizationType", ThemeColorizationTypeToName(g_settings.menuSeparatorLightThemeColorizationType));

    WriteDwordHKCU(kMenuFocusing, L"Disabled", static_cast<DWORD>(g_settings.menuFocusingDisabled));
    WriteDwordHKCU(kMenuFocusing, L"CornerRadius", static_cast<DWORD>(g_settings.menuFocusingCornerRadius));
    WriteDwordHKCU(kMenuFocusing, L"Width", static_cast<DWORD>(g_settings.menuFocusingWidth));
    WriteDwordHKCU(kMenuFocusing, L"DarkMode_Color", g_settings.menuFocusingDarkModeColor);
    WriteDwordHKCU(kMenuFocusing, L"LightMode_Color", g_settings.menuFocusingLightModeColor);
    WriteDwordHKCU(kMenuFocusing, L"EnableThemeColorization", static_cast<DWORD>(g_settings.menuFocusingEnableThemeColorization));
    WriteStringHKCU(kMenuFocusing, L"DarkMode_ThemeColorizationType", ThemeColorizationTypeToName(g_settings.menuFocusingDarkThemeColorizationType));
    WriteStringHKCU(kMenuFocusing, L"LightMode_ThemeColorizationType", ThemeColorizationTypeToName(g_settings.menuFocusingLightThemeColorizationType));

    WriteDwordHKCU(kMenuDisabledHot, L"Disabled", static_cast<DWORD>(g_settings.menuDisabledHotDisabled));
    WriteDwordHKCU(kMenuDisabledHot, L"CornerRadius", static_cast<DWORD>(g_settings.menuDisabledHotCornerRadius));
    if (g_settings.configAppCompatibilityMode) {
      SetOrDeleteZeroColorDword(kMenuDisabledHot, L"DarkMode_Color", g_settings.menuDisabledHotDarkModeColor);
      SetOrDeleteZeroColorDword(kMenuDisabledHot, L"LightMode_Color", g_settings.menuDisabledHotLightModeColor);
    } else {
      WriteDwordHKCU(kMenuDisabledHot, L"DarkMode_Color", g_settings.menuDisabledHotDarkModeColor);
      WriteDwordHKCU(kMenuDisabledHot, L"LightMode_Color", g_settings.menuDisabledHotLightModeColor);
    }
    WriteDwordHKCU(kMenuDisabledHot, L"EnableThemeColorization", static_cast<DWORD>(g_settings.menuDisabledHotEnableThemeColorization));
    WriteStringHKCU(kMenuDisabledHot, L"DarkMode_ThemeColorizationType", ThemeColorizationTypeToName(g_settings.menuDisabledHotDarkThemeColorizationType));
    WriteStringHKCU(kMenuDisabledHot, L"LightMode_ThemeColorizationType", ThemeColorizationTypeToName(g_settings.menuDisabledHotLightThemeColorizationType));

    WriteDwordHKCU(kMenuHot, L"Disabled", static_cast<DWORD>(g_settings.menuHotDisabled));
    WriteDwordHKCU(kMenuHot, L"CornerRadius", static_cast<DWORD>(g_settings.menuHotCornerRadius));
    WriteDwordHKCU(kMenuHot, L"DarkMode_Color", g_settings.menuHotDarkModeColor);
    WriteDwordHKCU(kMenuHot, L"LightMode_Color", g_settings.menuHotLightModeColor);
    WriteDwordHKCU(kMenuHot, L"EnableThemeColorization", static_cast<DWORD>(g_settings.menuHotEnableThemeColorization));
    WriteStringHKCU(kMenuHot, L"DarkMode_ThemeColorizationType", ThemeColorizationTypeToName(g_settings.menuHotDarkThemeColorizationType));
    WriteStringHKCU(kMenuHot, L"LightMode_ThemeColorizationType", ThemeColorizationTypeToName(g_settings.menuHotLightThemeColorizationType));

    // 4) Tooltip
    WriteDwordHKCU(kTooltip, L"NoSystemDropShadow", static_cast<DWORD>(g_settings.tooltipNoSystemDropShadow));
    SetOrDeleteDword(kTooltip, L"EffectType", g_settings.tooltipEffectType, 9);
    SetOrDeleteDword(kTooltip, L"CornerType", g_settings.tooltipCornerType, 4);
    SetOrDeleteDword(kTooltip, L"EnableDropShadow", g_settings.tooltipEnableDropShadow, 2);
    SetOrDeleteDword(kTooltip, L"NoBorderColor", g_settings.tooltipNoBorderColor, 2);
    SetOrDeleteDword(kTooltip, L"EnableThemeColorization", g_settings.tooltipEnableThemeColorization, 2);
    SetOrDeleteThemeTypes(kTooltip, g_settings.tooltipEnableThemeColorization, 2, g_settings.tooltipDarkModeThemeColorizationType, g_settings.tooltipLightModeThemeColorizationType);
    SetOrDeleteColorDword(kTooltip, L"DarkMode_BorderColor", g_settings.tooltipDarkModeBorderColor, g_settings.tooltipEnableDropShadow, 2);
    SetOrDeleteColorDword(kTooltip, L"LightMode_BorderColor", g_settings.tooltipLightModeBorderColor, g_settings.tooltipEnableDropShadow, 2);
    SetOrDeleteColorDword(kTooltip, L"DarkMode_GradientColor", g_settings.tooltipDarkModeGradientColor, g_settings.tooltipEffectType, 9);
    SetOrDeleteColorDword(kTooltip, L"LightMode_GradientColor", g_settings.tooltipLightModeGradientColor, g_settings.tooltipEffectType, 9);
    WriteDwordHKCU(kTooltip, L"DarkMode_Color", g_settings.tooltipDarkModeColor);
    WriteDwordHKCU(kTooltip, L"LightMode_Color", g_settings.tooltipLightModeColor);
    WriteDwordHKCU(kTooltip, L"MarginsType", static_cast<DWORD>(g_settings.tooltipMarginsType));
    WriteDwordHKCU(kTooltip, L"Margins_cxLeftWidth", static_cast<DWORD>(g_settings.tooltipMarginLeft));
    WriteDwordHKCU(kTooltip, L"Margins_cxRightWidth", static_cast<DWORD>(g_settings.tooltipMarginRight));
    WriteDwordHKCU(kTooltip, L"Margins_cyTopHeight", static_cast<DWORD>(g_settings.tooltipMarginTop));
    WriteDwordHKCU(kTooltip, L"Margins_cyBottomHeight", static_cast<DWORD>(g_settings.tooltipMarginBottom));
    SetOrDeleteDword(kTooltip, L"Disabled", g_settings.tooltipDisabled, 2);

    SyncProcessListHKCU(kBlockList, g_settings.processBlockList);
    SyncProcessListHKCU(kDisabledList, g_settings.processDisabledList);
    SyncProcessListHKCU(kMenuDisabledList, g_settings.menuProcessDisabledList);
    SyncProcessListHKCU(kTooltipDisabledList, g_settings.tooltipProcessDisabledList);
    SyncProcessListHKCU(kDropDownDisabledList, g_settings.dropDownProcessDisabledList);

    Wh_Log(L"Controller applied ordered full-option profile (Global/DropDown/Menu/Tooltip), hardReload=%d", g_settings.hardReloadOnApply);

    NotifyTranslucentFlyoutsReload();
}

////////////////////////////////////////////////////////////////////////////////
// Windhawk tool mod implementation for mods which don't need to inject to other
// processes or hook other functions. Context:
// https://github.com/ramensoftware/windhawk/wiki/Mods-as-tools:-Running-mods-in-a-dedicated-process
//
// The mod will load and run in a dedicated windhawk.exe process.
//
// Paste the code below as part of the mod code, and use these callbacks:
// * WhTool_ModInit
// * WhTool_ModSettingsChanged
// * WhTool_ModUninit
//
// Currently, other callbacks are not supported.

BOOL WhTool_ModInit()
{
    WTS_CONNECTSTATE_CLASS* pConnectState = nullptr;
    DWORD bytesReturned;
    bool isActive = false;
    if (WTSQuerySessionInformationW(WTS_CURRENT_SERVER_HANDLE,
                                    WTS_CURRENT_SESSION, WTSConnectState,
                                    (LPWSTR*)&pConnectState, &bytesReturned) &&
        pConnectState) {
        isActive = (*pConnectState == WTSActive);
        WTSFreeMemory(pConnectState);
    }
    if (!isActive) {
        Wh_Log(L"Tool process: session is not active, skipping");
        return FALSE;
    }

    Wh_Log(L"Tool process: applying TranslucentFlyouts settings");
    ApplySettingsOnceInToolProcess(true);

    // Unload the mod and exit the tool process.
    return FALSE;
}

void WhTool_ModUninit()
{
}

bool g_isToolModProcessLauncher;
HANDLE g_toolModProcessMutex;

void WINAPI EntryPoint_Hook()
{
    Wh_Log(L">");
    ExitThread(0);
}

BOOL Wh_ModInit()
{
    DWORD sessionId;
    if (ProcessIdToSessionId(GetCurrentProcessId(), &sessionId) &&
        sessionId == 0) {
        return FALSE;
    }

    bool isExcluded = false;
    bool isToolModProcess = false;
    bool isCurrentToolModProcess = false;
    int argc;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLine(), &argc);
    if (!argv) {
        Wh_Log(L"CommandLineToArgvW failed");
        return FALSE;
    }

    for (int i = 1; i < argc; i++) {
        if (wcscmp(argv[i], L"-service") == 0 || wcscmp(argv[i], L"-service-start") == 0 ||
            wcscmp(argv[i], L"-service-stop") == 0) {
            isExcluded = true;
            break;
        }
    }

    for (int i = 1; i < argc - 1; i++) {
        if (wcscmp(argv[i], L"-tool-mod") == 0) {
            isToolModProcess = true;
            if (wcscmp(argv[i + 1], WH_MOD_ID) == 0) {
                isCurrentToolModProcess = true;
            }
            break;
        }
    }

    LocalFree(argv);

    if (isExcluded) {
        return FALSE;
    }

    if (isCurrentToolModProcess) {
        Wh_Log(L"Tool process instance detected (%s)", WH_MOD_ID);
        g_toolModProcessMutex =
            CreateMutex(nullptr, TRUE, L"windhawk-tool-mod_" WH_MOD_ID);
        if (!g_toolModProcessMutex) {
            Wh_Log(L"CreateMutex failed");
            ExitProcess(1);
        }

        if (GetLastError() == ERROR_ALREADY_EXISTS) {
            Wh_Log(L"Tool mod already running (%s)", WH_MOD_ID);
            ExitProcess(1);
        }

        if (!WhTool_ModInit()) {
            ExitProcess(1);
        }

        IMAGE_DOS_HEADER* dosHeader =
            (IMAGE_DOS_HEADER*)GetModuleHandle(nullptr);
        IMAGE_NT_HEADERS* ntHeaders =
            (IMAGE_NT_HEADERS*)((BYTE*)dosHeader + dosHeader->e_lfanew);

        DWORD entryPointRVA =
            ntHeaders->OptionalHeader.AddressOfEntryPoint;
        void* entryPoint = (BYTE*)dosHeader + entryPointRVA;

        Wh_SetFunctionHook(entryPoint, (void*)EntryPoint_Hook, nullptr);
        return TRUE;
    }

    if (isToolModProcess) {
        return FALSE;
    }

    g_isToolModProcessLauncher = true;
    return TRUE;
}

void Wh_ModAfterInit()
{
    if (!g_isToolModProcessLauncher) {
        return;
    }

    WCHAR currentProcessPath[MAX_PATH];
    switch (GetModuleFileName(nullptr, currentProcessPath,
                              ARRAYSIZE(currentProcessPath))) {
        case 0:
        case ARRAYSIZE(currentProcessPath):
            Wh_Log(L"GetModuleFileName failed");
            return;
    }

    WCHAR commandLine[MAX_PATH + 2 +
                      (sizeof(L" -tool-mod \"" WH_MOD_ID "\"") / sizeof(WCHAR)) - 1];
    swprintf_s(commandLine, L"\"%s\" -tool-mod \"%s\"", currentProcessPath,
               WH_MOD_ID);

    HMODULE kernelModule = GetModuleHandle(L"kernelbase.dll");
    if (!kernelModule) {
        kernelModule = GetModuleHandle(L"kernel32.dll");
        if (!kernelModule) {
            Wh_Log(L"No kernelbase.dll/kernel32.dll");
            return;
        }
    }

    using CreateProcessInternalW_t = BOOL(WINAPI*)(
        HANDLE hUserToken, LPCWSTR lpApplicationName, LPWSTR lpCommandLine,
        LPSECURITY_ATTRIBUTES lpProcessAttributes,
        LPSECURITY_ATTRIBUTES lpThreadAttributes, WINBOOL bInheritHandles,
        DWORD dwCreationFlags, LPVOID lpEnvironment,
        LPCWSTR lpCurrentDirectory, LPSTARTUPINFOW lpStartupInfo,
        LPPROCESS_INFORMATION lpProcessInformation,
        PHANDLE hRestrictedUserToken);
    CreateProcessInternalW_t pCreateProcessInternalW =
        (CreateProcessInternalW_t)GetProcAddress(kernelModule,
                                                 "CreateProcessInternalW");
    if (!pCreateProcessInternalW) {
        Wh_Log(L"No CreateProcessInternalW");
        return;
    }

    STARTUPINFO si{
        .cb = sizeof(STARTUPINFO),
        .dwFlags = STARTF_FORCEOFFFEEDBACK,
    };
    PROCESS_INFORMATION pi;
    if (!pCreateProcessInternalW(nullptr, currentProcessPath, commandLine,
                                 nullptr, nullptr, FALSE, NORMAL_PRIORITY_CLASS,
                                 nullptr, nullptr, &si, &pi, nullptr)) {
        Wh_Log(L"CreateProcess failed");
        return;
    }

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}

BOOL Wh_ModSettingsChanged(BOOL* bReload)
{
    *bReload = TRUE;
  return TRUE;
}

void Wh_ModUninit()
{
    if (g_isToolModProcessLauncher) {
        return;
    }

    WhTool_ModUninit();
    ExitProcess(0);
}
