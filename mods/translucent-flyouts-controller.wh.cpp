// ==WindhawkMod==
// @id              translucent-flyouts-controller
// @name            Translucent Flyouts Controller
// @description     Controls TranslucentFlyouts settings through Windhawk (registry bridge)
// @version         1.0.0
// @author          GID0317
// @github          https://github.com/GID0317
// @include         windhawk.exe
// ==/WindhawkMod==

// ==WindhawkModSettings==
/*
- global_effectType: modern_acrylic
  $name: Global / Effect Type
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

- global_cornerType: small_round
  $name: Global / Corner Style
  $description: Choose the corner shape and roundness for this pop-up type. Use use_global to inherit the Global value
  $options:
    - dont_change: Don't Change
    - sharp: Sharp Corners
    - large_round: Large Round Corners
    - small_round: Small Round Corners

- global_enableDropShadow: false
  $name: Global / Enable Drop Shadow
  $description: Enable or disable drop shadow behind this pop-up type. Use use_global to inherit the Global value


- global_noBorderColor: false
  $name: Global / Disable Border Color
  $description: Choose whether to render system borders for this pop-up type. Use use_global to inherit the Global value


- global_enableThemeColorization: false
  $name: Global / Enable Theme Colorization
  $description: Use current theme accent color for borders instead of fixed custom border colors. Use use_global to inherit the Global value


- global_darkModeThemeColorizationType: start_hover
  $name: Global / Dark Accent Source
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

- global_lightModeThemeColorizationType: start_hover
  $name: Global / Light Accent Source
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

- global_darkModeBorderColor: "0xFF2B2B2B"
  $name: Global / Dark Border Color
  $description: Dark mode border color in ARGB hex format 0xAARRGGBB
- global_lightModeBorderColor: "0xFFDDDDDD"
  $name: Global / Light Border Color
  $description: Light mode border color in ARGB hex format 0xAARRGGBB
- global_darkModeGradientColor: "0x412B2B2B"
  $name: Global / Dark Gradient Color
  $description: Dark mode gradient or acrylic tint in ARGB hex format 0xAARRGGBB
- global_lightModeGradientColor: "0x9EDDDDDD"
  $name: Global / Light Gradient Color
  $description: Light mode gradient or acrylic tint in ARGB hex format 0xAARRGGBB
- global_enableMiniDump: true
  $name: Global / Enable MiniDump
  $description: Write crash minidump files for troubleshooting when TranslucentFlyouts fails


- global_disabled: false
  $name: Global / Disabled
  $description: Disable all effects for this pop-up type. Use use_global to inherit the Global value where available


- dropDown_effectType: use_global
  $name: DropDown / Effect Type
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

- dropDown_cornerType: use_global
  $name: DropDown / Corner Style
  $description: Choose the corner shape and roundness for this pop-up type. Use use_global to inherit the Global value
  $options:
    - dont_change: Don't Change
    - sharp: Sharp
    - large_round: Large Round
    - small_round: Small Round
    - use_global: Use Global Setting

- dropDown_enableDropShadow: use_global
  $name: DropDown / Enable Drop Shadow
  $description: Enable or disable drop shadow behind this pop-up type. Use use_global to inherit the Global value
  $options:
    - no: No
    - yes: Yes
    - use_global: Use Global Setting

- dropDown_enableFluentAnimation: false
  $name: DropDown / Enable Fluent Animation
  $description: Enable fluent pop-up animations for this category


- dropDown_noBorderColor: use_global
  $name: DropDown / Disable Border Color
  $description: Choose whether to render system borders for this pop-up type. Use use_global to inherit the Global value
  $options:
    - no: No
    - yes: Yes
    - use_global: Use Global Setting

- dropDown_enableThemeColorization: use_global
  $name: DropDown / Enable Theme Colorization
  $description: Use current theme accent color for borders instead of fixed custom border colors. Use use_global to inherit the Global value
  $options:
    - no: No
    - yes: Yes
    - use_global: Use Global Setting

- dropDown_darkModeThemeColorizationType: 1
  $name: DropDown / Dark Accent Source
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
- dropDown_lightModeThemeColorizationType: 1
  $name: DropDown / Light Accent Source
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
- dropDown_darkModeBorderColor: "0xFF2B2B2B"
  $name: DropDown / Dark Border Color
  $description: Dark mode border color in ARGB hex format 0xAARRGGBB
- dropDown_lightModeBorderColor: "0xFFDDDDDD"
  $name: DropDown / Light Border Color
  $description: Light mode border color in ARGB hex format 0xAARRGGBB
- dropDown_darkModeGradientColor: "0x412B2B2B"
  $name: DropDown / Dark Gradient Color
  $description: Dark mode gradient or acrylic tint in ARGB hex format 0xAARRGGBB
- dropDown_lightModeGradientColor: "0x9EDDDDDD"
  $name: DropDown / Light Gradient Color
  $description: Light mode gradient or acrylic tint in ARGB hex format 0xAARRGGBB
- dropDown_disabled: use_global
  $name: DropDown / Disabled
  $description: Disable all effects for this pop-up type. Use use_global to inherit the Global value where available
  $options:
    - no: No
    - yes: Yes
    - use_global: Use Global Setting

- dropDown_animation_fadeOutTime: 350
  $name: DropDown / Fade Out Time
  $description: Duration of fade-out animation in milliseconds
- dropDown_animation_popInTime: 250
  $name: DropDown / Pop In Time
  $description: Duration of pop-in animation in milliseconds
- dropDown_animation_fadeInTime: 87
  $name: DropDown / Fade In Time
  $description: Duration of fade-in animation in milliseconds
- dropDown_animation_popInStyle: slide_down
  $name: DropDown / Pop In Style
  $description: Style of the pop-in animation when the element appears
  $options:
    - slide_down: Slide Down
    - ripple: Ripple
    - smooth_scroll: Smooth Scroll
    - smooth_zoom: Smooth Zoom

- dropDown_animation_startRatio: 50
  $name: DropDown / Start Ratio
  $description: Start ratio for pop-in animation. Higher values start closer to final state
- dropDown_animation_enableImmediateInterupting: false
  $name: DropDown / Enable Immediate Interrupting
  $description: Allow running animations to be interrupted immediately by a new state change


- menu_noSystemDropShadow: false
  $name: Menu / Disable System Drop Shadow
  $description: Disable system-provided menu shadow


- menu_enableImmersiveStyle: true
  $name: Menu / Enable Immersive Style
  $description: Use modern uniformly styled pop-up menus. Recommended on Windows 11


- menu_enableCustomRendering: false
  $name: Menu / Enable Custom Rendering
  $description: Fully render pop-up menus using custom rendering. Required for several advanced Menu visual options


- menu_enableFluentAnimation: false
  $name: Menu / Enable Fluent Animation
  $description: Enable fluent menu animations


- menu_enableCompatibilityMode: false
  $name: Menu / Enable Compatibility Mode
  $description: Use compatibility mode for apps that misbehave with normal menu rendering


- menu_noModernAppBackgroundColor: true
  $name: Menu / Disable Modern App Background Color
  $description: Ignore modern app provided background color and use configured TranslucentFlyouts appearance instead


- menu_colorTreatAsTransparentEnabled: false
  $name: Menu / Color Treat As Transparent(Enable)
  $description: Treat a specific menu color as transparent during rendering
- menu_colorTreatAsTransparent: "0x00000000"
  $name: Menu / Color Treat As Transparent(ARGB)
  $description: ARGB key color treated as transparent in menu rendering, format 0xAARRGGBB
- menu_colorTreatAsTransparentThreshold: 50
  $name: Menu / Color Treat As Transparent Threshold
  $description: Tolerance for transparent color matching. Higher values match a wider color range
- menu_effectType: use_global
  $name: Menu / Effect Type
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

- menu_cornerType: use_global
  $name: Menu / Corner Style
  $description: Choose the corner shape and roundness for this pop-up type. Use use_global to inherit the Global value
  $options:
    - dont_change: Don't Change
    - sharp: Sharp
    - large_round: Large Round
    - small_round: Small Round
    - use_global: Use Global Setting

- menu_enableDropShadow: use_global
  $name: Menu / Enable Drop Shadow
  $description: Enable or disable drop shadow behind this pop-up type. Use use_global to inherit the Global value
  $options:
    - no: No
    - yes: Yes
    - use_global: Use Global Setting

- menu_noBorderColor: use_global
  $name: Menu / Disable Border Color
  $description: Choose whether to render system borders for this pop-up type. Use use_global to inherit the Global value
  $options:
    - no: No
    - yes: Yes
    - use_global: Use Global Setting

- menu_enableThemeColorization: use_global
  $name: Menu / Enable Theme Colorization
  $description: Use current theme accent color for borders instead of fixed custom border colors. Use use_global to inherit the Global value
  $options:
    - no: No
    - yes: Yes
    - use_global: Use Global Setting

- menu_darkModeThemeColorizationType: 1
  $name: Menu / Dark Accent Source
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
- menu_lightModeThemeColorizationType: 1
  $name: Menu / Light Accent Source
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
- menu_darkModeBorderColor: "0xFF2B2B2B"
  $name: Menu / Dark Border Color
  $description: Dark mode border color in ARGB hex format 0xAARRGGBB
- menu_lightModeBorderColor: "0xFFDDDDDD"
  $name: Menu / Light Border Color
  $description: Light mode border color in ARGB hex format 0xAARRGGBB
- menu_darkModeGradientColor: "0x412B2B2B"
  $name: Menu / Dark Gradient Color
  $description: Dark mode gradient or acrylic tint in ARGB hex format 0xAARRGGBB
- menu_lightModeGradientColor: "0x9EDDDDDD"
  $name: Menu / Light Gradient Color
  $description: Light mode gradient or acrylic tint in ARGB hex format 0xAARRGGBB
- menu_disabled: use_global
  $name: Menu / Disabled
  $description: Disable all effects for this pop-up type. Use use_global to inherit the Global value where available
  $options:
    - no: No
    - yes: Yes
    - use_global: Use Global Setting

- menu_animation_fadeOutTime: 350
  $name: Menu / Fade Out Time
  $description: Duration of fade-out animation in milliseconds
- menu_animation_popInTime: 250
  $name: Menu / Pop In Time
  $description: Duration of pop-in animation in milliseconds
- menu_animation_fadeInTime: 87
  $name: Menu / Fade In Time
  $description: Duration of fade-in animation in milliseconds
- menu_animation_popInStyle: slide_down
  $name: Menu / Pop In Style
  $description: Style of the pop-in animation when the element appears
  $options:
    - slide_down: Slide Down
    - ripple: Ripple
    - smooth_scroll: Smooth Scroll
    - smooth_zoom: Smooth Zoom
- menu_animation_startRatio: 50
  $name: Menu / Start Ratio
  $description: Start ratio for pop-in animation. Higher values start closer to final state
- menu_animation_enableImmediateInterupting: false
  $name: Menu / Enable Immediate Interrupting
  $description: Allow running animations to be interrupted immediately by a new state change


- menu_separator_disabled: 0
  $name: Menu / Separator / Disabled
  $description: Disable all effects for this pop-up type. Use use_global to inherit the Global value where available
- menu_separator_width: 1000
  $name: Menu / Separator / Width
  $description: Separator line thickness control. 1000 equals full default thickness
- menu_separator_darkModeColor: "0x30D9D9D9"
  $name: Menu / Separator / Dark Color
  $description: Dark mode color in ARGB hex format 0xAARRGGBB
- menu_separator_lightModeColor: "0x30262626"
  $name: Menu / Separator / Light Color
  $description: Light mode color in ARGB hex format 0xAARRGGBB
- menu_separator_enableThemeColorization: 0
  $name: Menu / Separator / Enable Theme Colorization
  $description: Use current theme accent color for borders instead of fixed custom border colors. Use use_global to inherit the Global value
- menu_separator_darkThemeColorizationType: 1
  $name: Menu / Separator / Dark Accent Source
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
- menu_separator_lightThemeColorizationType: 1
  $name: Menu / Separator / Light Accent Source
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
- menu_focusing_disabled: 0
  $name: Menu / Focusing / Disabled
  $description: Disable all effects for this pop-up type. Use use_global to inherit the Global value where available
- menu_focusing_cornerRadius: 8
  $name: Menu / Focusing / Corner Radius
  $description: Corner radius for focused item highlight
- menu_focusing_width: 1000
  $name: Menu / Focusing / Width
  $description: Width control for focused item highlight. 1000 equals full item width
- menu_focusing_darkModeColor: "0xFFFFFFFF"
  $name: Menu / Focusing / Dark Color
  $description: Dark mode color in ARGB hex format 0xAARRGGBB
- menu_focusing_lightModeColor: "0xFF000000"
  $name: Menu / Focusing / Light Color
  $description: Light mode color in ARGB hex format 0xAARRGGBB
- menu_focusing_enableThemeColorization: 0
  $name: Menu / Focusing / Enable Theme Colorization
  $description: Use current theme accent color for borders instead of fixed custom border colors. Use use_global to inherit the Global value
- menu_focusing_darkThemeColorizationType: 1
  $name: Menu / Focusing / Dark Accent Source
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
- menu_focusing_lightThemeColorizationType: 1
  $name: Menu / Focusing / Light Accent Source
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
- menu_disabledHot_disabled: 0
  $name: Menu / Disabled Hot / Disabled
  $description: Disable all effects for this pop-up type. Use use_global to inherit the Global value where available
- menu_disabledHot_cornerRadius: 8
  $name: Menu / Disabled Hot / Corner Radius
  $description: Corner radius for disabled hot item highlight
- menu_disabledHot_darkModeColor: "0x00000000"
  $name: Menu / Disabled Hot / Dark Color
  $description: Dark mode color in ARGB hex format 0xAARRGGBB
- menu_disabledHot_lightModeColor: "0x00000000"
  $name: Menu / Disabled Hot / Light Color
  $description: Light mode color in ARGB hex format 0xAARRGGBB
- menu_disabledHot_enableThemeColorization: 0
  $name: Menu / Disabled Hot / Enable Theme Colorization
  $description: Use current theme accent color for borders instead of fixed custom border colors. Use use_global to inherit the Global value
- menu_disabledHot_darkThemeColorizationType: 1
  $name: Menu / Disabled Hot / Dark Accent Source
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
- menu_disabledHot_lightThemeColorizationType: 1
  $name: Menu / Disabled Hot / Light Accent Source
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
- menu_hot_disabled: 0
  $name: Menu / Hot / Disabled
  $description: Disable all effects for this pop-up type. Use use_global to inherit the Global value where available
- menu_hot_cornerRadius: 8
  $name: Menu / Hot / Corner Radius
  $description: Corner radius for hot or hover item highlight
- menu_hot_darkModeColor: "0x41808080"
  $name: Menu / Hot / Dark Color
  $description: Dark mode color in ARGB hex format 0xAARRGGBB
- menu_hot_lightModeColor: "0x30000000"
  $name: Menu / Hot / Light Color
  $description: Light mode color in ARGB hex format 0xAARRGGBB
- menu_hot_enableThemeColorization: 0
  $name: Menu / Hot / Enable Theme Colorization
  $description: Use current theme accent color for borders instead of fixed custom border colors. Use use_global to inherit the Global value
- menu_hot_darkThemeColorizationType: 1
  $name: Menu / Hot / Dark Accent Source
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
- menu_hot_lightThemeColorizationType: 1
  $name: Menu / Hot / Light Accent Source
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

- tooltip_noSystemDropShadow: false
  $name: Tooltip / Disable System Drop Shadow
  $description: Disable system-provided tooltip shadow


- tooltip_effectType: use_global
  $name: Tooltip / Effect Type
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

- tooltip_cornerType: use_global
  $name: Tooltip / Corner Style
  $description: Choose the corner shape and roundness for this pop-up type. Use use_global to inherit the Global value
  $options:
    - dont_change: Don't Change
    - sharp: Sharp
    - large_round: Large Round
    - small_round: Small Round
    - use_global: Use Global Setting

- tooltip_enableDropShadow: use_global
  $name: Tooltip / Enable Drop Shadow
  $description: Enable or disable drop shadow behind this pop-up type. Use use_global to inherit the Global value
  $options:
    - no: No
    - yes: Yes
    - use_global: Use Global Setting

- tooltip_noBorderColor: use_global
  $name: Tooltip / Disable Border Color
  $description: Choose whether to render system borders for this pop-up type. Use use_global to inherit the Global value
  $options:
    - no: No
    - yes: Yes
    - use_global: Use Global Setting

- tooltip_enableThemeColorization: use_global
  $name: Tooltip / Enable Theme Colorization
  $description: Use current theme accent color for borders instead of fixed custom border colors. Use use_global to inherit the Global value
  $options:
    - no: No
    - yes: Yes
    - use_global: Use Global Setting

- tooltip_darkModeThemeColorizationType: 1
  $name: Tooltip / Dark Accent Source
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
- tooltip_lightModeThemeColorizationType: 1
  $name: Tooltip / Light Accent Source
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
- tooltip_darkModeBorderColor: "0xFF2B2B2B"
  $name: Tooltip / Dark Border Color
  $description: Dark mode border color in ARGB hex format 0xAARRGGBB
- tooltip_lightModeBorderColor: "0xFFDDDDDD"
  $name: Tooltip / Light Border Color
  $description: Light mode border color in ARGB hex format 0xAARRGGBB
- tooltip_darkModeGradientColor: "0x412B2B2B"
  $name: Tooltip / Dark Gradient Color
  $description: Dark mode gradient or acrylic tint in ARGB hex format 0xAARRGGBB
- tooltip_lightModeGradientColor: "0x9EDDDDDD"
  $name: Tooltip / Light Gradient Color
  $description: Light mode gradient or acrylic tint in ARGB hex format 0xAARRGGBB
- tooltip_darkModeColor: "0xFFFFFFFF"
  $name: Tooltip / Dark Fill Color
  $description: Dark mode color in ARGB hex format 0xAARRGGBB
- tooltip_lightModeColor: "0xFF1A1A1A"
  $name: Tooltip / Light Fill Color
  $description: Light mode color in ARGB hex format 0xAARRGGBB
- tooltip_marginsType: add_to_existing
  $name: Tooltip / Margins Mode
  $description: Define how tooltip margins are applied, add to existing or replace existing values
  $options:
    - add_to_existing: AddToExisting
    - replace_existing: ReplaceExisting
- tooltip_marginLeft: 6
  $name: Tooltip / Margin Left
  $description: Tooltip left margin in pixels
- tooltip_marginRight: 6
  $name: Tooltip / Margin Right
  $description: Tooltip right margin in pixels
- tooltip_marginTop: 6
  $name: Tooltip / Margin Top
  $description: Tooltip top margin in pixels
- tooltip_marginBottom: 6
  $name: Tooltip / Margin Bottom
  $description: Tooltip bottom margin in pixels
- tooltip_disabled: use_global
  $name: Tooltip / Disabled
  $description: Disable all effects for this pop-up type. Use use_global to inherit the Global value where available
  $options:
    - no: No
    - yes: Yes
    - use_global: Use Global Setting

- controller_resetAction: none
  $name: Controller / Reset To Defaults
  $description: Choose a category to reset to defaults. Set back to None before triggering another reset
  $options:
    - none: None
    - global: Reset Global
    - dropdown: Reset DropDown
    - menu: Reset Menu
    - tooltip: Reset Tooltip
    - all: Reset All

- controller_confirmRegistryWrite: true
  $name: Controller / Confirm Registry Write
  $description: Show a confirmation the first time this controller writes settings to HKCU\Software\TranslucentFlyouts

- controller_confirmReset: true
  $name: Controller / Confirm Reset
  $description: Show a confirmation before applying a reset-to-defaults action

- controller_hardReloadOnApply: false
  $name: Controller / Hard Reload On Apply
  $description: Also send TranslucentFlyouts detach and attach messages after apply. Slower, but can fix stale visuals
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

## References

This controller is built on top of two excellent projects:

- [TranslucentFlyouts](https://github.com/ALTaleX531/TranslucentFlyouts) by ALTaleX531 provides the original runtime and handles all the actual menu rendering that makes Windows menus truly customizable.

- [TranslucentFlyoutsConfig](https://github.com/Satanarious/TranslucentFlyoutsConfig) by Satanarious created a polished configuration application that became the standard way to manage these settings. The setting names, organization, and defaults used in this controller are based on that work.

Both projects are well maintained and worth exploring if you want to understand how it all works together.
*/
// ==/WindhawkModReadme==

#include <windhawk_utils.h>
#include <windows.h>
#include <CommCtrl.h>
#include <cstdint>
#include <cstdlib>
#include <cwctype>

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
    int resetAction = 0;

    bool confirmRegistryWrite = true;
    bool confirmReset = true;

    bool hardReloadOnApply = false;
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

static bool ConfirmWithDontShowAgain(
  bool allowUi,
  PCWSTR title,
  PCWSTR message,
  PCWSTR verificationText,
  const wchar_t* dontShowAgainIntValueName,
  const wchar_t* disableSettingName)
{
  if (!allowUi) {
    return true;
  }

  if (dontShowAgainIntValueName) {
    const int suppressed = Wh_GetIntValue(dontShowAgainIntValueName, 0);
    if (suppressed != 0) {
      return true;
    }
  }

  HMODULE comctl32 = LoadLibraryW(L"comctl32.dll");
  const auto pTaskDialogIndirect = reinterpret_cast<decltype(&TaskDialogIndirect)>(
    comctl32 ? GetProcAddress(comctl32, "TaskDialogIndirect") : nullptr);

  if (pTaskDialogIndirect) {
    TASKDIALOGCONFIG config{};
    config.cbSize = sizeof(config);
    config.hwndParent = nullptr;
    config.dwFlags = TDF_ALLOW_DIALOG_CANCELLATION;
    config.dwCommonButtons = TDCBF_YES_BUTTON | TDCBF_NO_BUTTON;
    config.pszWindowTitle = title;
    config.pszMainIcon = TD_WARNING_ICON;
    config.pszMainInstruction = title;
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
      if (button == IDYES) {
        if (verificationText && verificationChecked) {
          if (dontShowAgainIntValueName) {
            Wh_SetIntValue(dontShowAgainIntValueName, 1);
          }
          if (disableSettingName) {
            Wh_SetIntValue(disableSettingName, 0);
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

  const int response = MessageBoxW(
    nullptr,
    message,
    title,
    MB_ICONWARNING | MB_YESNO | MB_DEFBUTTON2);

  return response == IDYES;
}

static bool ShouldProceedWithRegistryWritePrompt(bool allowUi)
{
  if (!allowUi || !g_settings.confirmRegistryWrite) {
    return true;
  }

  const int alreadyConfirmed = Wh_GetIntValue(kRegistryWriteConfirmedValueName, 0);
  if (alreadyConfirmed != 0) {
    return true;
  }

  const bool confirmed = ConfirmWithDontShowAgain(
    allowUi,
    L"Translucent Flyouts Controller",
    L"This will write settings to HKCU\\Software\\TranslucentFlyouts and trigger a reload so TranslucentFlyouts can apply them.\n\nContinue?",
    L"Don't show this again",
    kRegistryWriteConfirmedValueName,
    L"controller_confirmRegistryWrite");

  return confirmed;
}

static bool MaybeApplyResetActionEdgeTriggered(bool allowUi)
{
  const int lastResetAction = Wh_GetIntValue(kLastResetActionValueName, 0);

  // Reset action is intentionally edge-triggered to avoid repeated resets.
  if (g_settings.resetAction == 0) {
    if (lastResetAction != 0) {
      Wh_SetIntValue(kLastResetActionValueName, 0);
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
      L"This will reset the selected category to its default values.\n\nContinue?",
      L"Don't show this again",
      nullptr,
      L"controller_confirmReset");

    if (!confirmed) {
      return false;
    }
  }

  ApplyResetAction(g_settings.resetAction);
  Wh_SetIntValue(kLastResetActionValueName, g_settings.resetAction);
  Wh_Log(L"Tool process: applied reset action=%d (%s)", g_settings.resetAction, ResetActionToLabel(g_settings.resetAction));

  if (g_settings.resetAction == 5) {
    // "Reset All" is treated as a full reset, including safety prompts.
    Wh_SetIntValue(L"controller_confirmRegistryWrite", 1);
    Wh_SetIntValue(L"controller_confirmReset", 1);
    Wh_SetIntValue(kRegistryWriteConfirmedValueName, 0);
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
    g_settings.globalEffectType = GetMappedIntSetting(L"global_effectType", kGlobalEffect, _countof(kGlobalEffect), 5);
    g_settings.globalCornerType = GetMappedIntSetting(L"global_cornerType", kGlobalCorner, _countof(kGlobalCorner), 3);
  g_settings.globalEnableDropShadow = GetBoolSettingCompat(L"global_enableDropShadow", 0);
  g_settings.globalNoBorderColor = GetBoolSettingCompat(L"global_noBorderColor", 0);
  g_settings.globalEnableThemeColorization = GetBoolSettingCompat(L"global_enableThemeColorization", 0);
    g_settings.globalDarkModeThemeColorizationType = GetMappedIntSetting(L"global_darkModeThemeColorizationType", kThemeColorizationType, _countof(kThemeColorizationType), 1);
    g_settings.globalLightModeThemeColorizationType = GetMappedIntSetting(L"global_lightModeThemeColorizationType", kThemeColorizationType, _countof(kThemeColorizationType), 1);
    g_settings.globalDarkModeBorderColor = GetColorSetting(L"global_darkModeBorderColor");
    g_settings.globalLightModeBorderColor = GetColorSetting(L"global_lightModeBorderColor");
    g_settings.globalDarkModeGradientColor = GetColorSetting(L"global_darkModeGradientColor");
    g_settings.globalLightModeGradientColor = GetColorSetting(L"global_lightModeGradientColor");
    g_settings.globalEnableMiniDump = GetBoolSettingCompat(L"global_enableMiniDump", 1);
    g_settings.globalDisabled = GetBoolSettingCompat(L"global_disabled", 0);

    g_settings.dropDownEffectType = GetMappedIntSetting(L"dropDown_effectType", kPartEffect, _countof(kPartEffect), 9);
    g_settings.dropDownCornerType = GetMappedIntSetting(L"dropDown_cornerType", kPartCorner, _countof(kPartCorner), 4);
    g_settings.dropDownEnableDropShadow = GetMappedIntSetting(L"dropDown_enableDropShadow", kTriState, _countof(kTriState), 2);
    g_settings.dropDownEnableFluentAnimation = GetBoolSettingCompat(L"dropDown_enableFluentAnimation", 0);
    g_settings.dropDownNoBorderColor = GetMappedIntSetting(L"dropDown_noBorderColor", kTriState, _countof(kTriState), 2);
    g_settings.dropDownEnableThemeColorization = GetMappedIntSetting(L"dropDown_enableThemeColorization", kTriState, _countof(kTriState), 2);
    g_settings.dropDownDarkModeThemeColorizationType = Wh_GetIntSetting(L"dropDown_darkModeThemeColorizationType");
    g_settings.dropDownLightModeThemeColorizationType = Wh_GetIntSetting(L"dropDown_lightModeThemeColorizationType");
    g_settings.dropDownDarkModeBorderColor = GetColorSetting(L"dropDown_darkModeBorderColor");
    g_settings.dropDownLightModeBorderColor = GetColorSetting(L"dropDown_lightModeBorderColor");
    g_settings.dropDownDarkModeGradientColor = GetColorSetting(L"dropDown_darkModeGradientColor");
    g_settings.dropDownLightModeGradientColor = GetColorSetting(L"dropDown_lightModeGradientColor");
    g_settings.dropDownDisabled = GetMappedIntSetting(L"dropDown_disabled", kTriState, _countof(kTriState), 2);
    g_settings.dropDownFadeOutTime = Wh_GetIntSetting(L"dropDown_animation_fadeOutTime");
    g_settings.dropDownPopInTime = Wh_GetIntSetting(L"dropDown_animation_popInTime");
    g_settings.dropDownFadeInTime = Wh_GetIntSetting(L"dropDown_animation_fadeInTime");
    g_settings.dropDownPopInStyle = GetMappedIntSetting(L"dropDown_animation_popInStyle", kPopInStyle, _countof(kPopInStyle), 0);
    g_settings.dropDownStartRatio = Wh_GetIntSetting(L"dropDown_animation_startRatio");
    g_settings.dropDownEnableImmediateInterupting = GetBoolSettingCompat(L"dropDown_animation_enableImmediateInterupting", 0);

    g_settings.menuNoSystemDropShadow = GetBoolSettingCompat(L"menu_noSystemDropShadow", 0);
    g_settings.menuEnableImmersiveStyle = GetBoolSettingCompat(L"menu_enableImmersiveStyle", 1);
    g_settings.menuEnableCustomRendering = GetBoolSettingCompat(L"menu_enableCustomRendering", 0);
    g_settings.menuEnableFluentAnimation = GetBoolSettingCompat(L"menu_enableFluentAnimation", 0);
    g_settings.menuEnableCompatibilityMode = GetBoolSettingCompat(L"menu_enableCompatibilityMode", 0);
    g_settings.menuNoModernAppBackgroundColor = GetBoolSettingCompat(L"menu_noModernAppBackgroundColor", 1);
    g_settings.menuColorTreatAsTransparentEnabled = Wh_GetIntSetting(L"menu_colorTreatAsTransparentEnabled");
    g_settings.menuColorTreatAsTransparent = GetColorSetting(L"menu_colorTreatAsTransparent");
    g_settings.menuColorTreatAsTransparentThreshold = Wh_GetIntSetting(L"menu_colorTreatAsTransparentThreshold");
    g_settings.menuEffectType = GetMappedIntSetting(L"menu_effectType", kPartEffect, _countof(kPartEffect), 9);
    g_settings.menuCornerType = GetMappedIntSetting(L"menu_cornerType", kPartCorner, _countof(kPartCorner), 4);
    g_settings.menuEnableDropShadow = GetMappedIntSetting(L"menu_enableDropShadow", kTriState, _countof(kTriState), 2);
    g_settings.menuNoBorderColor = GetMappedIntSetting(L"menu_noBorderColor", kTriState, _countof(kTriState), 2);
    g_settings.menuEnableThemeColorization = GetMappedIntSetting(L"menu_enableThemeColorization", kTriState, _countof(kTriState), 2);
    g_settings.menuDarkModeThemeColorizationType = Wh_GetIntSetting(L"menu_darkModeThemeColorizationType");
    g_settings.menuLightModeThemeColorizationType = Wh_GetIntSetting(L"menu_lightModeThemeColorizationType");
    g_settings.menuDarkModeBorderColor = GetColorSetting(L"menu_darkModeBorderColor");
    g_settings.menuLightModeBorderColor = GetColorSetting(L"menu_lightModeBorderColor");
    g_settings.menuDarkModeGradientColor = GetColorSetting(L"menu_darkModeGradientColor");
    g_settings.menuLightModeGradientColor = GetColorSetting(L"menu_lightModeGradientColor");
    g_settings.menuDisabled = GetMappedIntSetting(L"menu_disabled", kTriState, _countof(kTriState), 2);
    g_settings.menuFadeOutTime = Wh_GetIntSetting(L"menu_animation_fadeOutTime");
    g_settings.menuPopInTime = Wh_GetIntSetting(L"menu_animation_popInTime");
    g_settings.menuFadeInTime = Wh_GetIntSetting(L"menu_animation_fadeInTime");
    g_settings.menuPopInStyle = GetMappedIntSetting(L"menu_animation_popInStyle", kPopInStyle, _countof(kPopInStyle), 0);
    g_settings.menuStartRatio = Wh_GetIntSetting(L"menu_animation_startRatio");
    g_settings.menuEnableImmediateInterupting = GetBoolSettingCompat(L"menu_animation_enableImmediateInterupting", 0);

    g_settings.menuSeparatorDisabled = Wh_GetIntSetting(L"menu_separator_disabled");
    g_settings.menuSeparatorWidth = Wh_GetIntSetting(L"menu_separator_width");
    g_settings.menuSeparatorDarkModeColor = GetColorSetting(L"menu_separator_darkModeColor");
    g_settings.menuSeparatorLightModeColor = GetColorSetting(L"menu_separator_lightModeColor");
    g_settings.menuSeparatorEnableThemeColorization = Wh_GetIntSetting(L"menu_separator_enableThemeColorization");
    g_settings.menuSeparatorDarkThemeColorizationType = Wh_GetIntSetting(L"menu_separator_darkThemeColorizationType");
    g_settings.menuSeparatorLightThemeColorizationType = Wh_GetIntSetting(L"menu_separator_lightThemeColorizationType");

    g_settings.menuFocusingDisabled = Wh_GetIntSetting(L"menu_focusing_disabled");
    g_settings.menuFocusingCornerRadius = Wh_GetIntSetting(L"menu_focusing_cornerRadius");
    g_settings.menuFocusingWidth = Wh_GetIntSetting(L"menu_focusing_width");
    g_settings.menuFocusingDarkModeColor = GetColorSetting(L"menu_focusing_darkModeColor");
    g_settings.menuFocusingLightModeColor = GetColorSetting(L"menu_focusing_lightModeColor");
    g_settings.menuFocusingEnableThemeColorization = Wh_GetIntSetting(L"menu_focusing_enableThemeColorization");
    g_settings.menuFocusingDarkThemeColorizationType = Wh_GetIntSetting(L"menu_focusing_darkThemeColorizationType");
    g_settings.menuFocusingLightThemeColorizationType = Wh_GetIntSetting(L"menu_focusing_lightThemeColorizationType");

    g_settings.menuDisabledHotDisabled = Wh_GetIntSetting(L"menu_disabledHot_disabled");
    g_settings.menuDisabledHotCornerRadius = Wh_GetIntSetting(L"menu_disabledHot_cornerRadius");
    g_settings.menuDisabledHotDarkModeColor = GetColorSetting(L"menu_disabledHot_darkModeColor");
    g_settings.menuDisabledHotLightModeColor = GetColorSetting(L"menu_disabledHot_lightModeColor");
    g_settings.menuDisabledHotEnableThemeColorization = Wh_GetIntSetting(L"menu_disabledHot_enableThemeColorization");
    g_settings.menuDisabledHotDarkThemeColorizationType = Wh_GetIntSetting(L"menu_disabledHot_darkThemeColorizationType");
    g_settings.menuDisabledHotLightThemeColorizationType = Wh_GetIntSetting(L"menu_disabledHot_lightThemeColorizationType");

    g_settings.menuHotDisabled = Wh_GetIntSetting(L"menu_hot_disabled");
    g_settings.menuHotCornerRadius = Wh_GetIntSetting(L"menu_hot_cornerRadius");
    g_settings.menuHotDarkModeColor = GetColorSetting(L"menu_hot_darkModeColor");
    g_settings.menuHotLightModeColor = GetColorSetting(L"menu_hot_lightModeColor");
    g_settings.menuHotEnableThemeColorization = Wh_GetIntSetting(L"menu_hot_enableThemeColorization");
    g_settings.menuHotDarkThemeColorizationType = Wh_GetIntSetting(L"menu_hot_darkThemeColorizationType");
    g_settings.menuHotLightThemeColorizationType = Wh_GetIntSetting(L"menu_hot_lightThemeColorizationType");

    g_settings.tooltipNoSystemDropShadow = GetBoolSettingCompat(L"tooltip_noSystemDropShadow", 0);
    g_settings.tooltipEffectType = GetMappedIntSetting(L"tooltip_effectType", kPartEffect, _countof(kPartEffect), 9);
    g_settings.tooltipCornerType = GetMappedIntSetting(L"tooltip_cornerType", kPartCorner, _countof(kPartCorner), 4);
    g_settings.tooltipEnableDropShadow = GetMappedIntSetting(L"tooltip_enableDropShadow", kTriState, _countof(kTriState), 2);
    g_settings.tooltipNoBorderColor = GetMappedIntSetting(L"tooltip_noBorderColor", kTriState, _countof(kTriState), 2);
    g_settings.tooltipEnableThemeColorization = GetMappedIntSetting(L"tooltip_enableThemeColorization", kTriState, _countof(kTriState), 2);
    g_settings.tooltipDarkModeThemeColorizationType = Wh_GetIntSetting(L"tooltip_darkModeThemeColorizationType");
    g_settings.tooltipLightModeThemeColorizationType = Wh_GetIntSetting(L"tooltip_lightModeThemeColorizationType");
    g_settings.tooltipDarkModeBorderColor = GetColorSetting(L"tooltip_darkModeBorderColor");
    g_settings.tooltipLightModeBorderColor = GetColorSetting(L"tooltip_lightModeBorderColor");
    g_settings.tooltipDarkModeGradientColor = GetColorSetting(L"tooltip_darkModeGradientColor");
    g_settings.tooltipLightModeGradientColor = GetColorSetting(L"tooltip_lightModeGradientColor");
    g_settings.tooltipDarkModeColor = GetColorSetting(L"tooltip_darkModeColor");
    g_settings.tooltipLightModeColor = GetColorSetting(L"tooltip_lightModeColor");
    g_settings.tooltipMarginsType = GetMappedIntSetting(L"tooltip_marginsType", kMarginsType, _countof(kMarginsType), 0);
    g_settings.tooltipMarginLeft = Wh_GetIntSetting(L"tooltip_marginLeft");
    g_settings.tooltipMarginRight = Wh_GetIntSetting(L"tooltip_marginRight");
    g_settings.tooltipMarginTop = Wh_GetIntSetting(L"tooltip_marginTop");
    g_settings.tooltipMarginBottom = Wh_GetIntSetting(L"tooltip_marginBottom");
    g_settings.tooltipDisabled = GetMappedIntSetting(L"tooltip_disabled", kTriState, _countof(kTriState), 2);
    g_settings.resetAction = GetMappedIntSetting(L"controller_resetAction", kResetAction, _countof(kResetAction), 0);

    g_settings.confirmRegistryWrite = (Wh_GetIntSetting(L"controller_confirmRegistryWrite") != 0);
    g_settings.confirmReset = (Wh_GetIntSetting(L"controller_confirmReset") != 0);

    g_settings.hardReloadOnApply = (Wh_GetIntSetting(L"controller_hardReloadOnApply") != 0);

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
        WriteDwordHKCU(kMenu, L"ColorTreatAsTransparent", g_settings.menuColorTreatAsTransparent);
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
    WriteDwordHKCU(kMenuDisabledHot, L"DarkMode_Color", g_settings.menuDisabledHotDarkModeColor);
    WriteDwordHKCU(kMenuDisabledHot, L"LightMode_Color", g_settings.menuDisabledHotLightModeColor);
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

bool WhTool_ModInit()
{
    Wh_Log(L"Tool process: applying TranslucentFlyouts settings");
  ApplySettingsOnceInToolProcess(false);

  // Return true to signal success. The tool-mod process exits because the
  // snippet hooks windhawk.exe's entry point and EntryPoint_Hook() calls
  // ExitThread(0) (see EntryPoint_Hook below).
    return true;
}

void WhTool_ModSettingsChanged()
{
  ApplySettingsOnceInToolProcess(false);
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

void Wh_ModSettingsChanged()
{
    if (g_isToolModProcessLauncher) {
    // The wiki snippet only spawns the tool process once (in Wh_ModAfterInit).
    // To make "Save" re-apply the updated settings, we spawn it again here.
  // Show any confirmations in the launcher (interactive) context.
  if (!ApplySettingsOnceInToolProcess(true)) {
    return;
  }

  Wh_ModAfterInit();
        return;
    }

    WhTool_ModSettingsChanged();
}

void Wh_ModUninit()
{
    if (g_isToolModProcessLauncher) {
        return;
    }

    WhTool_ModUninit();
    ExitProcess(0);
}
