// ==WindhawkMod==
// @id              mouse-trail
// @name            Mouse Trail
// @name:zh-CN      鼠标拖尾
// @name:zh-TW      滑鼠拖尾
// @name:ja-JP      マウストレイル
// @name:ko-KR      마우스 트레일
// @name:de-DE      Maus-Spur
// @name:fr-FR      Traînée de souris
// @name:es-ES      Estela del ratón
// @name:ru-RU      След мыши
// @name:pt-BR      Rastro do mouse
// @name:it-IT      Scia del mouse
// @description     A highly customizable mouse trail effect.
// @description:zh-CN 高度可自定义的鼠标拖尾效果。
// @description:zh-TW 高度可自訂的滑鼠拖尾效果。
// @description:ja-JP 高度にカスタマイズ可能なマウストレイル効果。
// @description:ko-KR 고도로 커스터마이즈 가능한 마우스 트레일 효과.
// @description:de-DE Hochgradig anpassbarer Maus-Spur-Effekt.
// @description:fr-FR Effet de traînée de souris hautement personnalisable.
// @description:es-ES Efecto de estela de ratón altamente personalizable.
// @description:ru-RU Высоконастраиваемый эффект следа мыши.
// @description:pt-BR Efeito de rastro do mouse altamente personalizável.
// @description:it-IT Effetto scia del mouse altamente personalizzabile.
// @version         3.4.5.9
// @author          MCheng404
// @github          https://github.com/MCheng404
// @license         MIT
// @include         windhawk.exe
// @compilerOptions -ld2d1 -ld3d11 -ldxgi -ldcomp -ldwmapi -lole32 -lgdi32 -lshell32 -ld3dcompiler -lavrt -lksuser -ldwrite
// ==/WindhawkMod==
// ==WindhawkModReadme==
/*
# Mouse Trail

A highly customizable mouse cursor trail mod for Windhawk. Built on native D3D11 + DirectComposition hardware acceleration, featuring 23 color modes, 10 trail render styles, 9 particle shapes, a full Newtonian particle physics system (mass, gravity, collisions, electromagnetic forces, turbulence, fluid coupling), centripetal vortex orbital capture, music-reactive audio physics, 2.5D depth effects, click effects, and text/emoji particles. Runs as an isolated Tool Mod process — zero CPU when idle, full hardware acceleration when active.

## Demos

![Trail & Particles](https://raw.githubusercontent.com/MCheng404/windhawk-mods/mouse-trail-assets/mods/mouse-trail-assets/demo1.gif)

![Centripetal Vortex](https://raw.githubusercontent.com/MCheng404/windhawk-mods/mouse-trail-assets/mods/mouse-trail-assets/demo2.gif)

![Particle Physics](https://raw.githubusercontent.com/MCheng404/windhawk-mods/mouse-trail-assets/mods/mouse-trail-assets/demo3.gif)

---

### Rendering Architecture

* **Native D3D11 Pipeline:** Custom HLSL vertex/pixel shaders with instanced particle rendering. Core trail, particle, and shape rendering does not depend on D2D1.
* **DirectComposition Hardware Overlay:** Per-pixel alpha via premultiplied DXGI flip swap chain. Tear-free composition with the desktop compositor.
* **2.5D Depth Effect:** Every particle carries a z-depth with perspective projection, simple lighting, and depth-based scaling (near = larger/brighter, far = smaller/dimmer).
* **Dual-Thread Design:** UI thread runs the window message pump; render thread owns all D3D11/DComp work. Mouse input is never blocked by rendering.
* **HDR Auto-Detection:** Detects HDR displays and uses R16G16B16A16_FLOAT automatically; falls back to SDR BGRA8.
* **Device Loss Recovery:** Proactive `GetDeviceRemovedReason()` polling + `WM_POWERBROADCAST` wake handling. Full D3D/DComp stack rebuilds on GPU TDR, driver updates, GPU switches, or sleep/resume — no black frames.
* **Display Change Handling:** Auto-resizes and repositions the overlay on monitor changes or resolution switches.
* **Additive Blend Glow:** Trail, particle, and ripple glow layers use SrcAlpha + One additive blending for translucent halos.
* **Fast-Move Interpolation:** When frame interval is ≤10ms, particles are interpolated along the cursor path to eliminate gaps during fast movement.
* **Shader Quality Tiers:** Fast / Balanced / High. Balanced and High switch trail and particle edges to screen-space derivative anti-aliasing (edges are always ~1–2 px wide, independent of trail thickness or particle size).
* **Configurable Downsampling:** SSAA supports 2x / 3x / 4x with four reconstruction filters — Bilinear (1 tap), Box (16 tap), Tent (9-tap equivalent), Lanczos (16 tap) — plus an adjustable unsharp pass.
* **SDF Edge Sampling:** Particle outlines can be resolved with 2x2 or 4x rotated-grid sub-pixel samples of the shape SDF.
* **Output Dither:** Ordered 4x4 / 8x8 Bayer or hash noise, added at sub-LSB amplitude to remove 8-bit gradient banding (auto-disabled in HDR).
* **Text Atlas Supersampling:** Glyph atlas resolution multiplier (1x–4x) plus 4-tap rotated-grid atlas sampling for crisp text and emoji.

### Trail Render Styles (10 modes)

* **Tapered Ribbon:** Classic fading ribbon with glow, shadow, and head highlight
* **Dot Chain:** Beads along the path with configurable density and size
* **Function Curve:** Custom mathematical function deforms the trail (sine, damped, heartbeat, swirl, or user-defined formula)
* **Wave Curve:** Animated sinusoidal wave deformation
* **Shape Trail:** Spawns 9 selectable shapes (heart, star, hexagon, circle, diamond, triangle, flower, pentagon, hexagram, or random) along the path with random velocity, rotation, gravity, and configurable interval/size/count/lifetime
* **Double Line:** Two parallel trail ribbons
* **Dashed:** Constant-width segmented dashed line
* **Spiral:** Spiral deformation along the cursor path
* **Lightning:** Thin bright jagged main line with 35% probability random branch forks
* **Feather:** Thin central shaft with angled side barbs and natural feather curvature

### Color Modes (23 modes)

Single / Flowing Gradient / Rainbow Flow / Warm Flow / Cool Flow / Neon Pulse / Velocity Color / Stripes / Fire / Aurora / Cursor Extract / Cursor Mix / Metallic Gold / Cyberpunk / Pastel / Hue Rotate / Dual Pulse / Sparkle / Heatmap / Phase Interference / Spectrum Split / Grain Jitter / Gradient Warp

### Color Science

* **WCAG 2.1 Accessibility:** `Accessibility Contrast` guarantees 3.0:1 (AA Large), 4.5:1 (AA) or 7.0:1 (AAA) against the sampled background, adjusting luminance without shifting hue. Independent of Trail Adaptive Contrast.
* **Chroma-Preserving Adjustments:** Lighten/darken scale Rec.709 luma instead of lerping toward white, so saturated colors stay saturated; only a gamut-limited residual falls back to mixing.
* **Single Source of Truth:** The luma convention and the shared color helpers exist once and are mirrored by a shared HLSL prelude, eliminating the previously duplicated (comment-synced) shader copies.

### Gradient System

* **Unlimited Colors:** Add any number of gradient colors (up to 16)
* **OKLab Perceptual Interpolation:** No gray midpoints, perceptually uniform color transitions
* **256-Color LUT:** Precomputed lookup table for zero-per-frame allocation
* **Flowing Gradient:** Gradient animates along the trail over time

### Cursor Color Shift (6 modes)

Off / Complementary (180°) / Analogous (30°) / Triadic (120°) / Split Complement (150°) / Custom Angle

### Visual Effects

* **Bezier Smoothing:** Catmull-Rom spline interpolation for buttery-smooth curves
* **Motion Blur:** History frame overlay with decreasing opacity (1–5 strength)
* **Enhanced Glow:** Dual-layer halo (outer glow + inner bloom) with independent toggles
* **Head Highlight + Trail Shadow:** Premium depth cues
* **Speed-Reactive Width:** Trail widens when moving fast

### Particle Physics System

The mod ships a full Newtonian particle physics engine with independent toggles for every force:

* **Particle Mass:** Each particle gets a random mass (Box-Muller normal distribution). Mass affects inertia (heavy = less drag, retains velocity), size (∝ mass^(1/3)), lifetime, and acceleration (a = F/m)
* **Particle Gravity:** Newton's law of universal gravitation F = G·m₁·m₂/r² with Plummer softening. Supports 2-body (binary star) or N-body systems (2–10 dominant bodies)
* **Centripetal Vortex:** Curved mouse motion captures particles into orbiting tracks. Angular momentum conservation, Kepler velocity gradient, orbital precession, and 3D orbital inclination. Particles fly outward when motion stops. Two physical models: Rankine vortex or custom
* **Elastic Collisions:** Momentum + kinetic energy conservation with inverse-mass position correction
* **Lorentz Force:** Charged particles circle in a magnetic field (F = q·v×B). Positive charge = counterclockwise, negative = clockwise
* **Coulomb Force:** Like charges repel, opposite charges attract (F = k·q₁·q₂/r²). Activated alongside Lorentz force
* **Brownian Motion / Turbulence:** Perlin-like spatially coherent noise field (not pure random jitter). Particles in nearby space receive similar forces, creating flowing turbulence
* **Viscous Coupling:** Nearby particles (30px) drag each other's velocity, creating fluid-like cluster behavior
* **Air Drag:** Linear low-speed drag + quadratic high-speed drag. Mass-based inertia and size-based air resistance (larger particles have more drag)
* **Spin Physics:** Particles spin with rotational air damping (spin speed decays over time)
* **Springs (Cloth):** Hooke's law springs between nearby particles with axial damping and soft cutoff
* **Environmental Gravity:** Directional constant acceleration (angle + strength configurable)
* **Environmental Wind:** Horizontal wind with gusts, direction sway, and high-frequency turbulence
* **Cursor Attraction + Repulsion:** Force field around the cursor with configurable radius and falloff

### Music Reactive Framework

* **WASAPI Loopback Capture:** Real-time system audio capture (48kHz stereo)
* **FFT Frequency Analysis:** Cooley-Tukey FFT (256/512/1024/2048 points) with Hann window
* **Beat Detection:** Three methods — energy threshold, spectral flux, multi-band detection. Low-frequency weighted
* **Frequency Band Analysis:** Bass / Mid / Treble energy levels
* **BPM Estimation:** Real-time tempo estimation
* **Music-Physics Linking:** Beat → velocity pulse, bass → particle size, volume → gravity strength, beat → vortex energy, multi-band → gravity / magnetic field / thermal noise
* **Multi-Band Independent Linkage:** Bass links to gravity, mid links to magnetic field, treble links to thermal noise — all independently configurable

### Text & Emoji Particles

* Custom text released as particles (comma-separated phrases render as whole units)
* Emoji support via Segoe UI Emoji font
* Configurable font size
* Color follows the active color mode

### Click Effects

* Starburst particle burst on click (count, radius, duration configurable)
* Expanding ripple ring on click
* Both toggleable independently

### Localization

* Settings UI fully localized in English, Simplified Chinese, Traditional Chinese, and Japanese
* Every setting name, description, and dropdown option is translated

### Performance

* **Super Performance Mode:** Removes all particle/shape caps and fast-path downgrades
* **Adaptive Backoff:** Render thread waits 1ms when active (~1000fps headroom), 16ms when idle (~60fps responsive)
* **Game Detection:** Auto-hides in fullscreen DirectX games
* **Desktop Only Mode:** Optional gate that draws only while the desktop has focus. Foreground changes arrive through a WinEvent hook (`EVENT_SYSTEM_FOREGROUND`) on the overlay thread, so switching back to the desktop is picked up on the next frame instead of the next poll tick; a 1 s resync keeps the cached state honest, and the module falls back to 40 ms polling if the hook cannot be installed.
* **Desktop Only Layering:** Three modes for what happens while a window has focus. `hide` (default) stops the frame end to end, clears every queue and hides the overlay, so nothing costs anything. `cover` leaves the overlay topmost and keeps generating the trail over windows. `occlude` keeps rendering but drops the overlay to the bottom of the Z order, which puts it above the wallpaper and below every ordinary window (verified on a live desktop: LIFO index 662 against the shell window at 663), so windows cover the trail instead of the trail covering windows.
* **Zero CPU Idle:** Window hidden when cursor is stationary and no effects are active
* **Background Sampling:** Desktop color sampling runs on a dedicated low-priority thread
* **Frame Sync:** Syncs presentation to the compositor for tear-free output (adds one frame of latency). Turn it off for the lowest latency, accepting possible screen tearing.
* **Hotkeys:** Optional runtime hotkeys using a modifier combo plus a key (e.g. `Alt` + `T`). Laptop Fn combinations work by binding the media key they report. Three toggles: master show/hide, trail on/off, particles on/off.
* **Mixed-DPI Aware:** Detects the DPI of the monitor under the cursor so the overlay and text stay crisp and correctly sized across mixed-DPI setups.

### Function Trail Variables

Available in custom function formulas: `t` (normalized 0=head, 1=tail), `d` (distance from head in pixels), `time` (seconds). Functions: sin cos exp sqrt abs. Operators: + - * / ^.

Examples: `sin(d * 0.15) * 8`, `sin(d * 0.25) * exp(0 - t * 2.5) * 10`.

### Color Format

Hex RGB, e.g. `FF0000`=red, `00FF00`=green, `0000FF`=blue, `FFD700`=gold.

### Reporting Issues & Feedback

Found a bug, have a feature request, or want to share a preset? Please open an issue on the project repository:

**[github.com/MCheng404/mouse-trail](https://github.com/MCheng404/mouse-trail)**

When reporting an issue, please include:
* Windows build and display setup (single/mixed DPI, HDR on/off)
* The mod version and any non-default settings that reproduce it
* Whether the issue only appears in fullscreen games or on the desktop

### Quick Start

1. Install the mod from Windhawk and enable it.
2. Move the mouse — a trail appears once it exceeds **General → Trigger Velocity**.
3. Pick a style in **Trail → Trail Shape** and a palette in **Colors → Color Mode**.
4. Enable **Particles → Particle Mode** for emitted particles; tune physics under **Physics & Forces**.
5. Use **Hotkeys** to bind quick show/hide, trail and particle toggles (e.g. `Alt`+`T`).
6. Open **Rendering & Quality** if you need a sharper look (AA, bloom, HDR) or want to trade quality for FPS.

All settings are organized into collapsible sections in the settings dialog. Hover any control for its description; ranges and units are listed inline.

*/
// ==/WindhawkModReadme==
// ==WindhawkModSettings==
/*

- General:
  - desktop_only: false
    $name: Desktop Only
    $name:zh-CN: 仅在桌面绘制
    $name:zh-TW: 僅在桌面繪製
    $name:ja-JP: デスクトップのみ
    $description: "Draw only while the desktop has focus. How the overlay reacts to an active window is set by Desktop Only Layering."
    $description:zh-CN: 仅当桌面处于前台时绘制。窗口处于前台时叠加层如何反应由「仅在桌面时的层级」决定。
    $description:zh-TW: 僅當桌面處於前景時繪製。視窗處於前景時疊加層如何反應由「僅在桌面時的分層」決定。
    $description:ja-JP: デスクトップが前面のときだけ描画します。ウィンドウが前面のときのオーバーレイの動作は「デスクトップのみ時の階層」で決まります。
  - desktop_only_layer: hide
    $name: Desktop Only Layering
    $name:zh-CN: 仅在桌面时的层级
    $name:zh-TW: 僅在桌面時的分層
    $name:ja-JP: デスクトップのみ時の階層
    $description: How the overlay behaves while another window has focus — hide and clear the trail, keep drawing above the windows, or drop behind the windows so they occlude it.
    $description:zh-CN: 其他窗口处于前台时叠加层如何处理 —— 隐藏并清空拖尾、继续绘制并压在窗口之上、沉到窗口之下被它们遮挡。
    $description:zh-TW: 其他視窗處於前景時疊加層如何處理 —— 隱藏並清空拖尾、繼續繪製並壓在視窗之上、沉到視窗之下被它們遮擋。
    $description:ja-JP: 他ウィンドウが前面のときのオーバーレイの扱い —— 非表示にしてトレイルを消去、ウィンドウの上に描画し続ける、ウィンドウの下に回り込み隠れる。
    $options:
    - hide: Hide and Clear
    - cover: Cover Windows
    - occlude: Occluded by Windows
    $options:zh-CN:
    - hide: 隐藏并清空
    - cover: 覆盖上层窗口
    - occlude: 被窗口遮挡
    $options:zh-TW:
    - hide: 隱藏並清空
    - cover: 覆蓋上層視窗
    - occlude: 被視窗遮擋
    $options:ja-JP:
    - hide: 非表示にして消去
    - cover: ウィンドウの上に描画
    - occlude: ウィンドウに隠れる
  - foreground_hook: true
    $name: Foreground Event Hook
    $name:zh-CN: 前台事件钩子
    $name:zh-TW: 前景事件鉤子
    $name:ja-JP: 前面イベントフック
    $description: Track the foreground window with a WinEvent hook instead of polling, so desktop-only reacts instantly. Falls back to 40 ms polling when the hook cannot be installed.
    $description:zh-CN: 用 WinEvent 钩子跟踪前台窗口，替代轮询，让「仅在桌面绘制」即时响应。钩子无法安装时自动回退到 40ms 轮询。
    $description:zh-TW: 用 WinEvent 鉤子追蹤前景視窗，替代輪詢，讓「僅在桌面繪製」即時回應。鉤子無法安裝時自動回退到 40ms 輪詢。
    $description:ja-JP: WinEvent フックで前面ウィンドウを追跡し、ポーリングの代わりに即座に反応します。フックを設置できない場合は 40ms のポーリングに戻ります。
  - trigger_velocity: 25
    $name: Trigger Velocity
    $name:zh-CN: 触发速度
    $name:zh-TW: 觸發速度
    $name:ja-JP: 発動速度
    $description: Minimum mouse speed to activate the trail (pixels/frame).
    $description:zh-CN: 激活拖尾所需的最低鼠标移动速度（像素/帧）。
    $description:zh-TW: 啟動拖尾所需的最低滑鼠移動速度（像素/影格）。
    $description:ja-JP: トレイルを発動する最低マウス速度（ピクセル/フレーム）。
  - stop_velocity: 10
    $name: Stop Velocity
    $name:zh-CN: 停止速度
    $name:zh-TW: 停止速度
    $name:ja-JP: 停止速度
    $description: Speed below which the trail fades out. Must be lower than Trigger Velocity.
    $description:zh-CN: 低于此速度时拖尾开始淡出。必须低于触发速度。
    $description:zh-TW: 低於此速度時拖尾開始淡出。必須低於觸發速度。
    $description:ja-JP: この速度を下回るとトレイルがフェードアウトします。発動速度より低く設定してください。
  - tail_length: 18
    $name: Trail Length
    $name:zh-CN: 拖尾长度
    $name:zh-TW: 拖尾長度
    $name:ja-JP: トレイル長
    $description: Number of history points in the trail. Range 2-200.
    $description:zh-CN: 拖尾保留的历史点数。范围 2-200。
    $description:zh-TW: 拖尾保留的歷史點數。範圍 2-200。
    $description:ja-JP: トレイルの履歴ポイント数。範囲 2-200。
  - tail_offset_x: 6
    $name: Tail Offset X
    $name:zh-CN: 拖尾 X 偏移
    $name:zh-TW: 拖尾 X 偏移
    $name:ja-JP: Xオフセット
    $description: Horizontal offset of trail origin from cursor (pixels).
    $description:zh-CN: 拖尾起点相对光标的水平偏移（像素）。
    $description:zh-TW: 拖尾起點相對游標的水平偏移（像素）。
    $description:ja-JP: トレイル起点のカーソルからの水平オフセット（ピクセル）。
  - tail_offset_y: 10
    $name: Tail Offset Y
    $name:zh-CN: 拖尾 Y 偏移
    $name:zh-TW: 拖尾 Y 偏移
    $name:ja-JP: Yオフセット
    $description: Vertical offset of trail origin from cursor (pixels).
    $description:zh-CN: 拖尾起点相对光标的垂直偏移（像素）。
    $description:zh-TW: 拖尾起點相對游標的垂直偏移（像素）。
    $description:ja-JP: トレイル起点のカーソルからの垂直オフセット（ピクセル）。
  - trail_delay: 0
    $name: Trail Delay
    $name:zh-CN: 拖尾延迟
    $name:zh-TW: 拖尾延遲
    $name:ja-JP: トレイル遅延
    $description: How much the trail head lags behind the cursor, as a time constant of roughly
      25ms per step (0-10, 0=off). The lag is computed from real elapsed time, so it looks the same
      at 60Hz and 144Hz.
    $description:zh-CN: 拖尾头部滞后光标的程度，每档约 25 毫秒时间常数（0-10，0=关闭）。
      滞后量按真实经过时间计算，因此 60Hz 与 144Hz 屏幕下观感一致。
    $description:zh-TW: 拖尾頭部落後游標的程度，每檔約 25 毫秒時間常數（0-10，0=關閉）。
      滯後量按真實經過時間計算，因此 60Hz 與 144Hz 螢幕下觀感一致。
    $description:ja-JP: トレイル先端がカーソルから遅れる度合い。1 段階あたり約 25ms の時定数
      （0-10、0=オフ）。実際の経過時間から計算するため、60Hz と 144Hz で見え方が同じです。
  $name: General
  $name:zh-CN: 基础设置
  $name:zh-TW: 基礎設定
  $name:ja-JP: 基本設定

- Trail:
  - trail_shape: double
    $name: Trail Shape
    $name:zh-CN: 拖尾形状
    $name:zh-TW: 拖尾形狀
    $name:ja-JP: トレイル形状
    $options:
    - tapered: Tapered Ribbon
    - dots: Dot Chain
    - function: Function Curve
    - wave: Wave Curve
    - shapes: Shape Trail
    - double: Double Line
    - dashed: Dashed
    - spiral: Spiral
    - lightning: Lightning
    - feather: Feather
    - none: None
    $options:zh-CN:
    - tapered: 锥形飘带
    - dots: 圆点链
    - function: 函数曲线
    - wave: 波浪曲线
    - shapes: 形状拖尾
    - double: 双线拖尾
    - dashed: 虚线拖尾
    - spiral: 螺旋拖尾
    - lightning: 闪电拖尾
    - feather: 羽毛拖尾
    - none: 无
    $options:zh-TW:
    - tapered: 錐形飄帶
    - dots: 圓點鏈
    - function: 函數曲線
    - wave: 波浪曲線
    - shapes: 形狀拖尾
    - double: 雙線拖尾
    - dashed: 虛線拖尾
    - spiral: 螺旋拖尾
    - lightning: 閃電拖尾
    - feather: 羽毛拖尾
    - none: 無
    $options:ja-JP:
    - tapered: テーパーリボン
    - dots: ドットチェーン
    - function: 関数カーブ
    - wave: 波曲線
    - shapes: シェイプトレイル
    - double: ダブルライン
    - dashed: ダッシュ
    - spiral: スパイラル
    - lightning: ライトニング
    - feather: フェザー
    - none: なし
  - fadeout_mode: soft
    $name: Fadeout Mode
    $name:zh-CN: 淡出模式
    $name:zh-TW: 淡出模式
    $name:ja-JP: フェードアウト
    $description: How the trail disappears when mouse stops.
    $description:zh-CN: 鼠标停止后拖尾的消失方式。
    $description:zh-TW: 滑鼠停止後拖尾的消失方式。
    $description:ja-JP: マウス停止時のトレイルの消え方。
    $options:
    - hard: Hard Cut
    - accelerate: Accelerated Shrink
    - soft: Soft Fade
    $options:zh-CN:
    - hard: 硬截断
    - accelerate: 加速收缩
    - soft: 柔和淡出
    $options:zh-TW:
    - hard: 硬截斷
    - accelerate: 加速收縮
    - soft: 柔和淡出
    $options:ja-JP:
    - hard: ハードカット
    - accelerate: 加速収縮
    - soft: ソフトフェード
  - enable_smooth_gradient: true
    $name: Smooth Gradient
    $name:zh-CN: 平滑渐变
    $name:zh-TW: 平滑漸層
    $name:ja-JP: スムーズグラデーション
    $description: Smoothly interpolate between gradient colors. Turn off for stepped color bands.
    $description:zh-CN: 在渐变色之间平滑插值过渡，关闭则为分段阶梯色带。
    $description:zh-TW: 在漸層色之間平滑插值過渡，關閉則為分段階梯色帶。
    $description:ja-JP: グラデーション色間を滑らかに補間します。オフで段階的な色帯になります。
  - enable_speed_response: true
    $name: Dynamic Width
    $name:zh-CN: 动态宽度
    $name:zh-TW: 動態寬度
    $name:ja-JP: ダイナミック幅
    $description: Trail width increases with speed and acceleration.
    $description:zh-CN: 移动速度和加速度影响拖尾宽度，快速移动时更宽。
    $description:zh-TW: 移動速度和加速度影響拖尾寬度，快速移動時更寬。
    $description:ja-JP: 速度と加速度に応じてトレイル幅が変化します。
  - enable_frame_sync: true
    $name: Frame Sync (vsync)
    $name:zh-CN: 帧同步 (vsync)
    $name:zh-TW: 幀同步 (vsync)
    $name:ja-JP: フレーム同期 (vsync)
    $description: Sync presentation to the compositor for tear-free output (adds one frame of latency). Turn OFF for the lowest latency, accepting possible screen tearing.
    $description:zh-CN: 与合成器同步呈现，保证无撕裂（多一帧延迟）。关闭可获得最低延迟，但可能出现画面撕裂。
    $description:zh-TW: 與合成器同步呈現，保證無撕裂（多一幀延遲）。關閉可獲得最低延遲，但可能出現畫面撕裂。
    $description:ja-JP: コンポジターと同期してティアリングのない表示を行います（1フレームの遅延が加わります）。オフにすると遅延が最小になりますが、ティアリングの可能性があります。
  - enable_head_highlight: true
    $name: Head Highlight
    $name:zh-CN: 头部高光
    $name:zh-TW: 頭部高光
    $name:ja-JP: ヘッドハイライト
    $description: Bright center dot at trail head for premium look.
    $description:zh-CN: 拖尾头部添加明亮中心点，提升质感。
    $description:zh-TW: 拖尾頭部添加明亮中心點，提升質感。
    $description:ja-JP: トレイル先端に明るい中心点を追加。
  - enable_trail_shadow: true
    $name: Trail Shadow
    $name:zh-CN: 拖尾阴影
    $name:zh-TW: 拖尾陰影
    $name:ja-JP: トレイルシャドウ
    $description: Dark underlay shadow for depth perception.
    $description:zh-CN: 拖尾底层绘制暗色阴影，增加立体感。
    $description:zh-TW: 拖尾底層繪製暗色陰影，增加立體感。
    $description:ja-JP: 奥行き感のための暗い下地シャドウ。
  - enable_adaptive_contrast: false
    $name: Adaptive Contrast
    $name:zh-CN: 自适应对比度
    $name:zh-TW: 自適應對比度
    $name:ja-JP: 適応コントラスト
    $description: Sample background brightness and adapt trail color luminance automatically, with a subtle soft edge for visibility on any background.
    $description:zh-CN: 采样背景亮度自动微调拖尾颜色明暗，亮背景下压暗、暗背景提亮，并加一圈极细柔和边缘，确保任何背景下都清晰可见。
    $description:zh-TW: 取樣背景亮度自動微調拖尾顏色明暗，亮背景下壓暗、暗背景提亮，並加一圈極細柔和邊緣，確保任何背景下都清晰可見。
    $description:ja-JP: 背景の明るさをサンプリングして軌跡の色の輝度を自動調整します。明るい背景では暗く、暗い背景では明るく補正し、視認性のために控えめなソフトエッジも加えます。
  - enable_bezier_smooth: true
    $name: Bezier Smoothing
    $name:zh-CN: 贝塞尔曲线平滑
    $name:zh-TW: 貝茲曲線平滑
    $name:ja-JP: ベジェ平滑
    $description: Catmull-Rom spline interpolation for smoother curves.
    $description:zh-CN: 使用 Catmull-Rom 样条插值，拖尾曲线更顺滑。
    $description:zh-TW: 使用 Catmull-Rom 樣條插值，拖尾曲線更順滑。
    $description:ja-JP: Catmull-Romスプライン補間で滑らかな曲線に。
  - enable_motion_blur: true
    $name: Motion Blur
    $name:zh-CN: 运动模糊
    $name:zh-TW: 運動模糊
    $name:ja-JP: モーションブラー
    $description: Overlay previous frames with decreasing opacity for motion blur.
    $description:zh-CN: 以递减透明度叠加历史帧，制造运动模糊效果。
    $description:zh-TW: 以遞減透明度疊加歷史影格，製造運動模糊效果。
    $description:ja-JP: 過去フレームを減衰透明度で重畳しモーションブラー。
  - motion_blur_strength: 3
    $name: Motion Blur Strength
    $name:zh-CN: 运动模糊强度
    $name:zh-TW: 運動模糊強度
    $name:ja-JP: ブラー強度
    $description: Number of history frames to overlay (1-5).
    $description:zh-CN: 叠加的历史帧数（1-5）。
    $description:zh-TW: 疊加的歷史影格數（1-5）。
    $description:ja-JP: 重畳する過去フレーム数（1-5）。
  - enable_25d_effect: true
    $name: 2.5D Depth Effect
    $name:zh-CN: 2.5D 立体效果
    $name:zh-TW: 2.5D 立體效果
    $name:ja-JP: 2.5D効果
    $description: Add per-vertex depth with perspective projection for a 2.5D look.
    $description:zh-CN: 为顶点添加深度坐标和透视投影，营造 2.5D 立体感。
    $description:zh-TW: 為頂點添加深度座標和透視投影，營造 2.5D 立體感。
    $description:ja-JP: 頂点ごとに深度と透視投影を追加し2.5D表現。
  - perspective_strength: 15
    $name: Perspective Strength
    $name:zh-CN: 透视强度
    $name:zh-TW: 透視強度
    $name:ja-JP: 遠近強度
    $description: Strength of 2.5D perspective scaling (0-50).
    $description:zh-CN: 2.5D 透视缩放强度（0-50）。
    $description:zh-TW: 2.5D 透視縮放強度（0-50）。
    $description:ja-JP: 2.5D遠近スケールの強さ（0-50）。
  $name: Trail
  $name:zh-CN: 拖尾外观
  $name:zh-TW: 拖尾外觀
  $name:ja-JP: トレイル外観

- Colors:
  - color_mode: pastel
    $name: Color Mode
    $name:zh-CN: 颜色模式
    $name:zh-TW: 顏色模式
    $name:ja-JP: カラーモード
    $options:
    - single: Single Color
    - gradient: Flowing Gradient
    - rainbow: Rainbow Flow
    - warm: Warm Flow
    - cool: Cool Flow
    - neon: Neon Pulse
    - velocity: Velocity Color
    - stripes: Stripes
    - fire: Fire
    - aurora: Aurora
    - cursor_extract: Cursor Extract
    - cursor_mix: Cursor Mix
    - metallic: Metallic Gold
    - cyberpunk: Cyberpunk
    - pastel: Pastel
    - hue_rotate: Hue Rotate
    - dual_pulse: Dual Pulse
    - sparkle: Sparkle
    - thermal: Thermal Heatmap
    - interference: Wave Interference
    - spectrum: Spectrum Split
    - dither: Grain Dither
    - warp: Gradient Warp
    - oil_slick: Iridescent Oil Film
    - vaporwave: Vaporwave
    - bioluminescence: Bioluminescence
    - clock_sky: Time-of-Day Sky
    $options:zh-CN:
    - single: 单色
    - gradient: 流动渐变
    - rainbow: 彩虹流动
    - warm: 暖色调流动
    - cool: 冷色调流动
    - neon: 霓虹脉冲
    - velocity: 速度变色
    - stripes: 流动条纹
    - fire: 火焰
    - aurora: 极光
    - cursor_extract: 光标取色
    - cursor_mix: 光标混色
    - metallic: 金属金
    - cyberpunk: 赛博朋克
    - pastel: 粉彩
    - hue_rotate: 色相旋转
    - dual_pulse: 双色脉冲
    - sparkle: 星光闪烁
    - thermal: 热力图
    - interference: 波纹干涉
    - spectrum: 色谱分裂
    - dither: 颗粒抖动
    - warp: 渐变扭曲
    - oil_slick: 虹彩油膜
    - vaporwave: 蒸汽波
    - bioluminescence: 生物荧光
    - clock_sky: 时令天色
    $options:zh-TW:
    - single: 單色
    - gradient: 流動漸層
    - rainbow: 彩虹流動
    - warm: 暖色調流動
    - cool: 冷色調流動
    - neon: 霓虹脈衝
    - velocity: 速度變色
    - stripes: 流動條紋
    - fire: 火焰
    - aurora: 極光
    - cursor_extract: 游標取色
    - cursor_mix: 游標混色
    - metallic: 金屬金
    - cyberpunk: 賽博朋克
    - pastel: 粉彩
    - hue_rotate: 色相旋轉
    - dual_pulse: 雙色脈衝
    - sparkle: 星光閃爍
    - thermal: 熱力圖
    - interference: 波紋干涉
    - spectrum: 色譜分裂
    - dither: 顆粒抖動
    - warp: 漸層扭曲
    - oil_slick: 虹彩油膜
    - vaporwave: 蒸氣波
    - bioluminescence: 生物螢光
    - clock_sky: 時令天色
    $options:ja-JP:
    - single: 単色
    - gradient: 流動グラデ
    - rainbow: レインボー
    - warm: ウォーム
    - cool: クール
    - neon: ネオン
    - velocity: 速度連動
    - stripes: ストライプ
    - fire: ファイア
    - aurora: オーロラ
    - cursor_extract: カーソル抽出
    - cursor_mix: カーソル混合
    - metallic: メタリックゴールド
    - cyberpunk: サイバーパンク
    - pastel: パステル
    - hue_rotate: 色相回転
    - dual_pulse: デュアルパルス
    - sparkle: スパークル
    - thermal: サーマル
    - interference: 波干渉
    - spectrum: スペクトル
    - dither: ディザ
    - warp: ワープ
    - oil_slick: 虹彩油膜
    - vaporwave: ヴェイパーウェイヴ
    - bioluminescence: 生物発光
    - clock_sky: 一日の空色
  - custom_color: 00BFFF
    $name: Custom Color
    $name:zh-CN: 自定义颜色
    $name:zh-TW: 自訂顏色
    $name:ja-JP: カスタムカラー
    $description: Primary color for single/neon/hue-rotate modes. Hex RGB.
    $description:zh-CN: 单色/霓虹/色相旋转模式的主色，十六进制 RGB。
    $description:zh-TW: 單色/霓虹/色相旋轉模式的主色，十六進位 RGB。
    $description:ja-JP: 単色/ネオン/色相回転モードの基本色。Hex RGB。
  - gradient_colors: FF6B35,00BFFF,FFD700
    $name: Gradient Colors
    $name:zh-CN: 渐变颜色
    $name:zh-TW: 漸層顏色
    $name:ja-JP: グラデーション色
    $description: Any number of colors (max 16) for gradient mode, comma-separated hex RGB. Uses OKLab perceptual interpolation with 256-color LUT. Gradient flows along the trail over time.
    $description:zh-CN: 渐变模式的颜色，任意数量（最多16种），逗号分隔十六进制RGB。使用OKLab感知均匀插值+256色LUT。渐变沿拖尾随时间流动。
    $description:zh-TW: 漸層模式的顏色，任意數量（最多16種），逗號分隔十六進位RGB。使用OKLab感知均勻插值+256色LUT。漸層沿拖尾隨時間流動。
    $description:ja-JP: グラデーションモードの色（最大16色）、カンマ区切りHex RGB。OKLab知覚補間+256色LUT使用。
  - enable_cursor_color_shift: false
    $name: Auto Color Shift
    $name:zh-CN: 取色自动偏移
    $name:zh-TW: 取色自動偏移
    $name:ja-JP: 自動カラーシフト
    $description: Auto hue shift for cursor extraction modes to ensure visibility.
    $description:zh-CN: 光标取色模式下自动偏移色相，确保拖尾在任何背景上都醒目。
    $description:zh-TW: 游標取色模式下自動偏移色相，確保拖尾在任何背景上都醒目。
    $description:ja-JP: カーソル抽出モードで視認性を確保するため自動的に色相をシフトします。
  - color_shift_mode: complementary
    $name: Color Shift Mode
    $name:zh-CN: 取色偏移模式
    $name:zh-TW: 取色偏移模式
    $name:ja-JP: カラーシフトモード
    $options:
    - off: Off
    - complementary: Complementary (180°)
    - analogous: Analogous (30°)
    - triadic: Triadic (120°)
    - split: Split Complement (150°)
    - custom: Custom Angle
    $options:zh-CN:
    - off: 关闭
    - complementary: 互补色(180°)
    - analogous: 类似色(30°)
    - triadic: 三角色(120°)
    - split: 分裂互补(150°)
    - custom: 自定义角度
    $options:zh-TW:
    - off: 關閉
    - complementary: 互補色(180°)
    - analogous: 類似色(30°)
    - triadic: 三角色(120°)
    - split: 分裂互補(150°)
    - custom: 自訂角度
    $options:ja-JP:
    - off: オフ
    - complementary: 補色(180°)
    - analogous: 類似色(30°)
    - triadic: 三色(120°)
    - split: スプリット補色(150°)
    - custom: カスタム
  - color_shift_angle: 180
    $name: Custom Shift Angle
    $name:zh-CN: 自定义偏移角度
    $name:zh-TW: 自訂偏移角度
    $name:ja-JP: シフト角度
    $description: Hue shift angle for custom mode. 0-360 degrees.
    $description:zh-CN: 自定义模式下的色相偏移角度，0-360度。
    $description:zh-TW: 自訂模式下的色相偏移角度，0-360度。
    $description:ja-JP: カスタムモードの色相シフト角度。0-360度。
  - contrast_target: off
    $name: Accessibility Contrast
    $name:zh-CN: 无障碍对比度
    $name:zh-TW: 無障礙對比度
    $name:ja-JP: アクセシビリティ コントラスト
    $options:
    - off: Off
    - aa_large: WCAG AA Large (3.0)
    - aa: WCAG AA (4.5)
    - aaa: WCAG AAA (7.0)
    $options:zh-CN:
    - off: 关闭
    - aa_large: WCAG AA 大字 (3.0)
    - aa: WCAG AA 正文 (4.5)
    - aaa: WCAG AAA (7.0)
    $options:zh-TW:
    - off: 關閉
    - aa_large: WCAG AA 大字 (3.0)
    - aa: WCAG AA 正文 (4.5)
    - aaa: WCAG AAA (7.0)
    $options:ja-JP:
    - off: オフ
    - aa_large: WCAG AA 大文字 (3.0)
    - aa: WCAG AA 本文 (4.5)
    - aaa: WCAG AAA (7.0)
    $description: Guarantee a minimum contrast between trail colours and the sampled background brightness, without shifting hue. Independent of Trail Adaptive Contrast.
    $description:zh-CN: 保证拖尾颜色与采样到的背景亮度之间不低于所选对比度比，且不改变色相。与「拖尾外观 → 自适应对比度」相互独立。
    $description:zh-TW: 保證拖尾顏色與取樣到的背景亮度之間不低於所選對比度比，且不改變色相。與「拖尾外觀 → 自適應對比度」相互獨立。
    $description:ja-JP: 軌跡の色とサンプリングした背景輝度のコントラスト比を下回らないように保証します（色相は変化しません）。「トレイル外観 → 適応コントラスト」とは独立しています。
  $name: Colors
  $name:zh-CN: 颜色
  $name:zh-TW: 顏色
  $name:ja-JP: カラー

- Quality:
  - enable_glow: true
    $name: Micro Glow
    $name:zh-CN: 微发光
    $name:zh-TW: 微發光
    $name:ja-JP: マイクログロー
    $description: Soft outer glow around the trail.
    $description:zh-CN: 拖尾外圈柔和发光效果。
    $description:zh-TW: 拖尾外圈柔和發光效果。
    $description:ja-JP: トレイル周囲のソフトなグロー。
  - glow_intensity: 40
    $name: Glow Intensity
    $name:zh-CN: 发光强度
    $name:zh-TW: 發光強度
    $name:ja-JP: グロー強度
    $description: Glow radius and brightness (0-100).
    $description:zh-CN: 发光范围和亮度（0-100）。
    $description:zh-TW: 發光範圍和亮度（0-100）。
    $description:ja-JP: グローの半径と明るさ（0-100）。
  - enhanced_glow: true
    $name: Enhanced Glow
    $name:zh-CN: 增强发光
    $name:zh-TW: 增強發光
    $name:ja-JP: 拡張グロー
    $description: Dual-layer halo (outer + inner) for softer glow.
    $description:zh-CN: 双层光晕（外晕+内辉），发光更柔和自然。
    $description:zh-TW: 雙層光暈（外暈+內輝），發光更柔和自然。
    $description:ja-JP: 二重ハロー（外側+内側）でより柔らかいグロー。
  - enable_trail_stroke: true
    $name: Trail Stroke
    $name:zh-CN: 拖尾描边
    $name:zh-TW: 拖尾描邊
    $name:ja-JP: トレイルストローク
    $description: Outline along the trail edges. Shares stroke color/alpha with particle stroke.
    $description:zh-CN: 沿拖尾边缘的描边轮廓。描边颜色/透明度与粒子描边共用。
    $description:zh-TW: 沿拖尾邊緣的描邊輪廓。描邊顏色/透明度與粒子描邊共用。
    $description:ja-JP: トレイル縁のストローク。色/透明度は粒子ストロークと共用。
  - trail_stroke_width: 20
    $name: Trail Stroke Width
    $name:zh-CN: 拖尾描边宽度
    $name:zh-TW: 拖尾描邊寬度
    $name:ja-JP: トレイルストローク幅
    $description: Stroke thickness as percentage of trail half-width (0-50).
    $description:zh-CN: 描边粗细，占拖尾半宽百分比（0-50）。
    $description:zh-TW: 描邊粗細，佔拖尾半寬百分比（0-50）。
    $description:ja-JP: ストローク太さ（トレイル半幅に対する%、0-50）。
  - edge_softness: 50
    $name: Edge Softness
    $name:zh-CN: 边缘柔和度
    $name:zh-TW: 邊緣柔和度
    $name:ja-JP: エッジ柔軟度
    $description: Trail edge anti-aliasing width (0=hard edge, 100=very soft).
    $description:zh-CN: 拖尾边缘抗锯齿宽度（0=硬边，100=极柔和）。
    $description:zh-TW: 拖尾邊緣反鋸齒寬度（0=硬邊，100=極柔和）。
    $description:ja-JP: トレイル端のアンチエイリアス幅（0=硬い端、100=非常に柔らかい）。
  - aa_mode: extra
    $name: Anti-Aliasing
    $name:zh-CN: 抗锯齿
    $name:zh-TW: 反鋸齒
    $name:ja-JP: アンチエイリアス
    $description: Width of the soft band drawn along trail edges, widest (= softest) first. Extra = ~4px, can look blurry on small particles. Smooth = ~2px, the default. Crisp = ~1px, keeps shapes sharp. Off = hard edges with visible jaggies.
    $description:zh-CN: 拖尾边缘柔化过渡带的宽度，由柔到硬依次为：强化≈4像素（小粒子上可能显得模糊）、平滑≈2像素（默认）、清晰≈1像素（保持形状锐利）、关闭=硬边（可见锯齿）。
    $description:zh-TW: 拖尾邊緣柔化過渡帶的寬度，由柔到硬依次為：強化≈4像素（小粒子上可能顯得模糊）、平滑≈2像素（預設）、清晰≈1像素（保持形狀銳利）、關閉=硬邊（可見鋸齒）。
    $description:ja-JP: 軌跡エッジに描くソフト遷移帯の幅。柔らかい順に エクストラ=約4px（小さいパーティクルではぼやけて見える）/ スムーズ=約2px（既定）/ クリスプ=約1px（形状がシャープ）/ オフ=ハードエッジ（ジャギーが目立つ）。
    $options:
    - off: Off
    - smooth: Smooth
    - crisp: Crisp
    - extra: Extra Smooth
    $options:zh-CN:
    - off: 关闭
    - smooth: 平滑
    - crisp: 锐利
    - extra: 超平滑
    $options:zh-TW:
    - off: 關閉
    - smooth: 平滑
    - crisp: 銳利
    - extra: 超平滑
    $options:ja-JP:
    - off: オフ
    - smooth: スムーズ
    - crisp: シャープ
    - extra: ウルトラスムーズ
  - enable_msaa: true
    $name: Enable MSAA
    $name:zh-CN: 启用 MSAA
    $name:zh-TW: 啟用 MSAA
    $name:ja-JP: MSAAを有効化
    $description: Multi-Sample Anti-Aliasing. Hardware edge smoothing.
    $description:zh-CN: 硬件多重采样抗锯齿，平滑三角形边缘。
    $description:zh-TW: 硬體多重採樣抗鋸齒，平滑三角形邊緣。
    $description:ja-JP: ハードウェアMSAA。
  - msaa_level: 4x
    $name: MSAA Level
    $name:zh-CN: MSAA 级别
    $name:zh-TW: MSAA 等級
    $name:ja-JP: MSAAレベル
    $description: Hardware sample count per pixel, effective only when Enable MSAA is on. Higher values smooth the offscreen trail edges more but cost linear extra memory bandwidth and resolve time; 4x is usually enough.
    $description:zh-CN: 每像素的硬件采样数，仅在启用 MSAA 时生效。数值越高，离屏拖尾边缘越平滑，但显存带宽与解析开销同比上升；通常 4x 已足够。
    $description:zh-TW: 每像素的硬體取樣數，僅在啟用 MSAA 時生效。數值越高，離屏拖尾邊緣越平滑，但顯存頻寬與解析開銷同比上升；通常 4x 已足夠。
    $description:ja-JP: 1ピクセルあたりのハードウェアサンプル数。MSAA有効時のみ適用されます。値を上げるほど離屏トレイルのエッジが滑らかになりますが、メモリ帯域と解決時間のコストも比例して増加します。通常は4xで十分です。
    $options:
    - 2x: 2x
    - 4x: 4x
    - 8x: 8x
    $options:zh-CN:
    - 2x: 2x
    - 4x: 4x
    - 8x: 8x
    $options:zh-TW:
    - 2x: 2x
    - 4x: 4x
    - 8x: 8x
    $options:ja-JP:
    - 2x: 2x
    - 4x: 4x
    - 8x: 8x
  - enable_ssaa: false
    $name: Enable SSAA
    $name:zh-CN: 启用 SSAA
    $name:zh-TW: 啟用 SSAA
    $name:ja-JP: SSAAを有効化
    $description: Supersample Anti-Aliasing. Render at higher resolution then downsample. Best quality but GPU intensive.
    $description:zh-CN: 超采样抗锯齿，更高分辨率渲染再降采样。质量最高但耗 GPU。
    $description:zh-TW: 超取樣反鋸齒。以高解析度渲染後縮小。畫質最佳但GPU負擔大。
    $description:ja-JP: スーパーサンプリングAA。高解像度で描画してから縮小するため画質は最高ですが、GPU負荷も最大になります。
  - ssaa_level: 2x
    $name: SSAA Scale
    $name:zh-CN: SSAA 缩放
    $name:zh-TW: SSAA 倍率
    $name:ja-JP: SSAAスケール
    $description: Render resolution scale factor, effective only when Enable SSAA is on. 2x renders at twice the width and height, i.e. four times the pixels, so cost grows with the square of the value; pick the lowest level that still looks clean.
    $description:zh-CN: 渲染分辨率缩放倍数，仅在启用 SSAA 时生效。2x 表示宽高各放大 2 倍，即像素量变为 4 倍，因此开销随倍数的平方增长；选择看起来干净的最低档即可。
    $description:zh-TW: 渲染解析度縮放倍數，僅在啟用 SSAA 時生效。2x 表示寬高各放大 2 倍，即像素量變為 4 倍，因此開銷隨倍數的平方增長；選擇看起來乾淨的最低檔即可。
    $description:ja-JP: レンダリング解像度の倍率。SSAA有効時のみ適用されます。2x は縦横それぞれ2倍、つまり画素数は4倍になるため、コストは倍率の二乗で増加します。見た目が綺麗な最小値を選んでください。
    $options:
    - 2x: 2x
    - 3x: 3x
    - 4x: 4x
    $options:zh-CN:
    - 2x: 2x
    - 3x: 3x
    - 4x: 4x
    $options:zh-TW:
    - 2x: 2x
    - 3x: 3x
    - 4x: 4x
    $options:ja-JP:
    - 2x: 2x
    - 3x: 3x
    - 4x: 4x
  - ssaa_filter: box
    $name: SSAA Downsample Filter
    $name:zh-CN: SSAA 降采样滤波器
    $name:zh-TW: SSAA 降採樣濾波器
    $name:ja-JP: SSAAダウンサンプルフィルタ
    $description: Reconstruction filter used when downsampling the supersampled buffer. Bilinear = 1 tap (fastest), Box = 16 taps (cleanest), Tent = 9-tap equivalent (smoothest), Lanczos = 16 taps (sharpest, slight ringing).
    $description:zh-CN: 超采样缓冲降采样时使用的重建滤波器。Bilinear=1次采样（最快），Box=16次采样（最干净），Tent=等效9次采样（最平滑），Lanczos=16次采样（最锐利，轻微振铃）。
    $description:zh-TW: 超取樣緩衝降採樣時使用的重建濾波器。Bilinear=1次取樣（最快），Box=16次取樣（最乾淨），Tent=等效9次取樣（最平滑），Lanczos=16次取樣（最銳利，輕微振鈴）。
    $description:ja-JP: ダウンサンプル時の再構成フィルタ。Bilinear=1タップ（最速）、Box=16タップ、Tent=9タップ相当、Lanczos=16タップ（最シャープ）。
    $options:
    - bilinear: Bilinear (1 tap)
    - box: Box (16 tap)
    - tent: Tent (9 tap)
    - lanczos: Lanczos (16 tap)
    $options:zh-CN:
    - bilinear: 双线性（1次采样）
    - box: 方形（16次采样）
    - tent: 帐篷（等效9次采样）
    - lanczos: Lanczos（16次采样）
    $options:zh-TW:
    - bilinear: 雙線性（1次取樣）
    - box: 方形（16次取樣）
    - tent: 帳篷（等效9次取樣）
    - lanczos: Lanczos（16次取樣）
    $options:ja-JP:
    - bilinear: バイリニア（1タップ）
    - box: ボックス（16タップ）
    - tent: テント（9タップ相当）
    - lanczos: Lanczos（16タップ）
  - ssaa_sharpness: 0
    $name: SSAA Sharpen
    $name:zh-CN: SSAA 降采样锐化
    $name:zh-TW: SSAA 降採樣銳化
    $name:ja-JP: SSAAシャープネス
    $description: Unsharp amount applied after downsampling. 0 = neutral, higher values restore edge crispness lost by filtering.
    $description:zh-CN: 降采样后叠加的非锐化掩模强度。0=中性，数值越高越能找回被滤波抹掉的边缘锐度。
    $description:zh-TW: 降採樣後疊加的非銳化遮罩強度。0=中性，數值越高越能找回被濾波抹掉的邊緣銳度。
    $description:ja-JP: ダウンサンプル後のアンシャープ量。0=ニュートラル、高いほどエッジが締まる。
  - shader_quality: high
    $name: Shader Quality
    $name:zh-CN: 着色器质量
    $name:zh-TW: 著色器品質
    $name:ja-JP: シェーダー品質
    $description: Fast = fixed-width edges (cheapest). Balanced = screen-space derivative AA (edges are always ~1px wide) + 4-tap text atlas sampling. High = derivative AA + 4-tap rotated SDF edge sampling for the smoothest particle outlines.
    $description:zh-CN: 快速=固定宽度边缘（最省）。均衡=屏幕空间导数抗锯齿（边缘恒为约1像素宽）+ 文字图集4次采样。高=导数抗锯齿 + 粒子SDF 4次旋转网格边缘采样，轮廓最平滑。
    $description:zh-TW: 快速=固定寬度邊緣（最省）。均衡=螢幕空間導數抗鋸齒（邊緣恆為約1像素寬）+ 文字圖集4次取樣。高=導數抗鋸齒 + 粒子SDF 4次旋轉網格邊緣取樣，輪廓最平滑。
    $description:ja-JP: 高速=固定幅エッジ（最も軽量）。バランス=画面空間微分のAA（エッジは常に約1px幅）＋文字アトラス4タップ。高品質=微分AA＋SDFの回転グリッド4タップエッジサンプリングで、最も滑らかなパーティクル輪郭になります。
    $options:
    - fast: Fast
    - balanced: Balanced
    - high: High
    $options:zh-CN:
    - fast: 快速
    - balanced: 均衡
    - high: 高
    $options:zh-TW:
    - fast: 快速
    - balanced: 均衡
    - high: 高
    $options:ja-JP:
    - fast: 高速
    - balanced: バランス
    - high: 高品質
  - sdf_sample_quality: 2x2
    $name: Particle Edge Sampling
    $name:zh-CN: 粒子边缘采样
    $name:zh-TW: 粒子邊緣取樣
    $name:ja-JP: パーティクルエッジサンプル
    $description: Samples taken per pixel when evaluating the particle shape SDF. 2x2 and 4x add rotated-grid samples for smoother outlines (costs more GPU on large particles).
    $description:zh-CN: 计算粒子形状SDF时每像素的采样次数。2x2 与 4x 采用旋转网格多次采样，轮廓更平滑（大粒子时GPU开销更大）。
    $description:zh-TW: 計算粒子形狀SDF時每像素的取樣次數。2x2 與 4x 採用旋轉網格多次取樣，輪廓更平滑（大粒子時GPU開銷更大）。
    $description:ja-JP: パーティクル形状のSDFを評価する際の1ピクセルあたりサンプル数。2x2 と 4x は回転グリッドサンプルを追加して輪郭を滑らかにします（大きなパーティクルではGPUコストが増加）。
    $options:
    - 1x: 1x
    - 2x2: 2x2 Rotated
    - 4x: 4x Rotated
    $options:zh-CN:
    - 1x: 1x
    - 2x2: 2x2 旋转网格
    - 4x: 4x 旋转网格
    $options:zh-TW:
    - 1x: 1x
    - 2x2: 2x2 旋轉網格
    - 4x: 4x 旋轉網格
    $options:ja-JP:
    - 1x: 1x
    - 2x2: 2x2 回転グリッド
    - 4x: 4x 回転グリッド
  - dither_mode: ordered4
    $name: Output Dither
    $name:zh-CN: 输出抖动
    $name:zh-TW: 輸出抖動
    $name:ja-JP: 出力ディザ
    $description: Breaks up gradient banding on 8-bit displays by adding a sub-LSB noise pattern. Auto-disabled in HDR mode.
    $description:zh-CN: 叠加亚色阶噪声图案，消除 8bit 显示下的渐变色带。HDR 模式自动关闭。
    $description:zh-TW: 疊加亞色階雜訊圖案，消除 8bit 顯示下的漸層色帶。HDR 模式自動關閉。
    $description:ja-JP: 8bit表示のグラデーションバンディングをサブLSBノイズで低減します。HDRでは自動無効。
    $options:
    - off: Off
    - ordered4: Ordered 4x4
    - ordered8: Ordered 8x8
    - hash: Hash Noise
    $options:zh-CN:
    - off: 关闭
    - ordered4: 有序 4x4
    - ordered8: 有序 8x8
    - hash: 哈希噪声
    $options:zh-TW:
    - off: 關閉
    - ordered4: 有序 4x4
    - ordered8: 有序 8x8
    - hash: 雜訊雜湊
    $options:ja-JP:
    - off: オフ
    - ordered4: 有序 4x4
    - ordered8: 有序 8x8
    - hash: ハッシュノイズ
  - dither_strength: 50
    $name: Dither Strength
    $name:zh-CN: 抖动强度
    $name:zh-TW: 抖動強度
    $name:ja-JP: ディザ強度
    $description: Dither amplitude in 1/100 of a color step. 50 = half a color step (invisible, removes most banding).
    $description:zh-CN: 抖动幅度，单位为 1/100 色阶。50=半个色阶（肉眼不可见，可消除大部分色带）。
    $description:zh-TW: 抖動幅度，單位為 1/100 色階。50=半個色階（肉眼不可見，可消除大部分色帶）。
    $description:ja-JP: ディザ振幅（1色階の1/100単位）。50=半色階で、目視できない程度ながらバンディングの大半を除去します。
  - enable_temporal_blur: true
    $name: Temporal Blur (Motion Blur)
    $name:zh-CN: 时序模糊（运动模糊）
    $name:zh-TW: 時序模糊（運動模糊）
    $name:ja-JP: 時間ブラー（モーションブラー）
    $description: Real temporal accumulation. Keeps the previous frames and blends them into the current frame, producing directional afterimages instead of stacked translucent outlines. Adds one fullscreen render target.
    $description:zh-CN: 真正的时序累积。保留前几帧并与当前帧混合，形成带方向性的拖影残像，而非多层半透明描边叠加。需额外一张全屏渲染目标。
    $description:zh-TW: 真正的時序累積。保留前幾幀並與當前幀混合，形成帶方向性的拖影殘像，而非多層半透明描邊疊加。需額外一張全屏渲染目標。
    $description:ja-JP: 真の時間的蓄積。前フレームを保持して合成し、方向性のある残像を作ります（全画面RTを1枚追加）。
  - temporal_blur_amount: 35
    $name: Temporal Blur Amount
    $name:zh-CN: 时序模糊强度
    $name:zh-TW: 時序模糊強度
    $name:ja-JP: 時間ブラー量
    $description: How much of the previous frame is retained. Higher = longer trails. 95 keeps almost everything (very smeary).
    $description:zh-CN: 上一帧被保留的比例。数值越高残像越长。95 几乎全部保留（拖影很重）。
    $description:zh-TW: 上一幀被保留的比例。數值越高殘像越長。95 幾乎全部保留（拖影很重）。
    $description:ja-JP: 前フレームの保持量。高いほど残像が長くなります。95はほぼ全保持。
  - post_aa: 'off'
    $name: Post-process Anti-aliasing
    $name:zh-CN: 后处理抗锯齿
    $name:zh-TW: 後處理反鋸齒
    $name:ja-JP: 後処理アンチエイリアス
    $description: Fullscreen edge anti-aliasing applied before presentation. Cheaper and more uniform than MSAA/SSAA and needs no multisampled offscreen target. FXAA Extreme finds longer edges at a slightly higher cost.
    $description:zh-CN: 呈现前对整屏做边缘抗锯齿。比 MSAA/SSAA 更省、更均匀，且不需要多重采样离屏目标。FXAA 强化版会追踪更长的边缘，开销略高。
    $description:zh-TW: 呈現前對整屏做邊緣反鋸齒。比 MSAA/SSAA 更省、更均勻，且不需要多重採樣離屏目標。FXAA 強化版會追蹤更長的邊緣，開銷略高。
    $description:ja-JP: 提示前に全画面のエッジAAを適用します。MSAA/SSAAより軽量で均一、多重サンプリングのオフスクリーンターゲットも不要です。FXAA Extreme はより長いエッジを検出できますが、コストはやや高くなります。
    $options:
    - 'off': Off
    - fxaa: FXAA
    - fxaa_extreme: FXAA Extreme
    $options:zh-CN:
    - 'off': 关闭
    - fxaa: FXAA
    - fxaa_extreme: FXAA 强化
    $options:zh-TW:
    - 'off': 關閉
    - fxaa: FXAA
    - fxaa_extreme: FXAA 強化
    $options:ja-JP:
    - 'off': オフ
    - fxaa: FXAA
    - fxaa_extreme: FXAA 強化
  - enable_bloom: true
    $name: Bloom
    $name:zh-CN: 泛光
    $name:zh-TW: 泛光
    $name:ja-JP: ブルーム
    $description: Real bloom. Bright areas are extracted and blurred through a downsample/upsample chain, then added back. Produces soft halos and natural overexposure instead of simply widening the glow bands.
    $description:zh-CN: 真正的泛光：提取亮部经降采样/上采样模糊链后加性叠加。得到柔和光晕与自然过曝，而非单纯加宽发光带。
    $description:zh-TW: 真正的泛光：提取亮部經降採樣/上採樣模糊鏈後加性疊加。得到柔和光暈與自然過曝，而非單純加寬發光帶。
    $description:ja-JP: 本物のブルーム。高輝度領域を抽出してダウンサンプル/アップサンプルのぼかし鎖で処理し、加算して戻します。単にグロー帯を広げるのではなく、柔らかいハローと自然な露出オーバーを得られます。
  - bloom_threshold: 60
    $name: Bloom Threshold
    $name:zh-CN: 泛光阈值
    $name:zh-TW: 泛光閾值
    $name:ja-JP: ブルーム閾値
    $description: Luminance above which pixels start contributing to the bloom. Lower = more of the trail blooms.
    $description:zh-CN: 亮度超过该值才参与泛光。越低越多拖尾内容参与发光。
    $description:zh-TW: 亮度超過該值才參與泛光。越低越多拖尾內容參與發光。
    $description:ja-JP: この輝度を超えた画素がブルームに寄与します。値を下げるほど、軌跡のより広い範囲が発光します。
  - bloom_intensity: 50
    $name: Bloom Intensity
    $name:zh-CN: 泛光强度
    $name:zh-TW: 泛光強度
    $name:ja-JP: ブルーム強度
    $description: Strength of the bloom that is added back on top of the scene.
    $description:zh-CN: 叠加回场景的泛光强度。
    $description:zh-TW: 疊加回場景的泛光強度。
    $description:ja-JP: シーンに加算されるブルームの強さ。
  - hdr_mode: auto
    $name: HDR Output
    $name:zh-CN: HDR 输出
    $name:zh-TW: HDR 輸出
    $name:ja-JP: HDR出力
    $description: How the FP16 swap chain is interpreted. Auto = use scRGB when the display reports HDR (correct colour space + linearisation, highlights may exceed 1.0). Off = legacy behaviour (gamma-encoded 0-1 only). HDR10 = 10-bit PQ output.
    $description:zh-CN: FP16 交换链的解读方式。自动=检测到 HDR 显示器时使用 scRGB（正确色彩空间 + 线性化，高光可超过 1.0）。关闭=旧行为（仅 gamma 编码 0-1）。HDR10=10bit PQ 输出。
    $description:zh-TW: FP16 交換鏈的解讀方式。自動=偵測到 HDR 顯示器時使用 scRGB（正確色彩空間 + 線性化，高光可超過 1.0）。關閉=舊行為（僅 gamma 編碼 0-1）。HDR10=10bit PQ 輸出。
    $description:ja-JP: FP16スワップチェーンの解釈方法。自動=HDR時にscRGB（正しい色空間＋線形化）。オフ=従来動作。HDR10=10bit PQ出力。
    $options:
    - auto: Auto
    - 'off': 'Off (Legacy)'
    - scrgb: scRGB
    - hdr10: HDR10 (PQ)
    $options:zh-CN:
    - auto: 自动
    - 'off': 关闭（旧行为）
    - scrgb: scRGB
    - hdr10: HDR10 (PQ)
    $options:zh-TW:
    - auto: 自動
    - 'off': 關閉（舊行為）
    - scrgb: scRGB
    - hdr10: HDR10 (PQ)
    $options:ja-JP:
    - auto: 自動
    - 'off': オフ（従来動作）
    - scrgb: scRGB
    - hdr10: HDR10 (PQ)
  - hdr_highlight_boost: 0
    $name: HDR Highlight Boost
    $name:zh-CN: HDR 高光增益
    $name:zh-TW: HDR 高光增益
    $name:ja-JP: HDRハイライトブースト
    $description: Pushes bright pixels above 1.0 so they render as real HDR highlights (only has an effect on an HDR display with HDR Output enabled). 0 = unchanged.
    $description:zh-CN: 把亮部像素推高到 1.0 以上，使其成为真正的 HDR 高光（仅在 HDR 显示器且开启 HDR 输出时生效）。0=不变。
    $description:zh-TW: 把亮部像素推高到 1.0 以上，使其成為真正的 HDR 高光（僅在 HDR 顯示器且開啟 HDR 輸出時生效）。0=不變。
    $description:ja-JP: 明るい画素を1.0超へ押し上げ、真のHDRハイライトにします（HDR表示＋HDR出力時のみ）。0=変化なし。
  - hdr_tonemap: 'off'
    $name: HDR Tone Mapping
    $name:zh-CN: HDR 色调映射
    $name:zh-TW: HDR 色調映射
    $name:ja-JP: HDRトーンマッピング
    $description: Compresses the boosted range back into display range so highlights roll off instead of clipping to white. ACES is filmic and slightly desaturates highlights; Reinhard is neutral.
    $description:zh-CN: 把增益后的范围压回显示范围，使高光平缓滚降而非硬切到纯白。ACES 偏电影感、高光略去饱和；Reinhard 更中性。
    $description:zh-TW: 把增益後的範圍壓回顯示範圍，使高光平緩滾降而非硬切到純白。ACES 偏電影感、高光略去飽和；Reinhard 更中性。
    $description:ja-JP: ブースト後の範囲を表示範囲へ圧縮し、白飛びの代わりに緩やかに落とします。ACES=映画的、Reinhard=ニュートラル。
    $options:
    - 'off': Off
    - reinhard: Reinhard
    - aces: ACES (Filmic)
    $options:zh-CN:
    - 'off': 关闭
    - reinhard: Reinhard
    - aces: ACES（电影感）
    $options:zh-TW:
    - 'off': 關閉
    - reinhard: Reinhard
    - aces: ACES（電影感）
    $options:ja-JP:
    - 'off': オフ
    - reinhard: Reinhard
    - aces: ACES（映画的）
  $name: Rendering & Quality
  $name:zh-CN: 画质与发光
  $name:zh-TW: 畫質與發光
  $name:ja-JP: 画質とグロー

- Particles:
  - enable_collision_spark: false
    $name: Collision Spark
    $name:zh-CN: 碰撞火花
    $name:zh-TW: 碰撞火花
    $name:ja-JP: 衝突スパーク
    $description: Spawn a brief burst of sparks where particles collide, making the physical
      impact visible. Sparks are ordinary particles, so drag, gravity and vortices affect them
      like anything else. Capped per frame, so dense collisions cannot flood the screen.
    $description:zh-CN: 粒子相撞时在碰撞点迸出一小簇火花，让物理碰撞看得见。火花是普通粒子，
      同样受阻力、重力与漩涡影响。每帧有生成上限，密集碰撞也不会刷屏。
    $description:zh-TW: 粒子相撞時在碰撞點迸出一小簇火花，讓物理碰撞看得見。火花是普通粒子，
      同樣受阻力、重力與漩渦影響。每幀有生成上限，密集碰撞也不會刷屏。
    $description:ja-JP: 粒子が衝突する衝突点に短い火花を飛ばし、物理的な衝突を可視化します。
      火花は通常の粒子なので、空気抵抗・重力・渦の影響もそのまま受けます。毎フレーム上限あり。
  - spark_chance: 40
    $name: Spark Chance
    $name:zh-CN: 火花概率
    $name:zh-TW: 火花機率
    $name:ja-JP: 火花確率
    $description: Percentage chance that a collision produces sparks. Raise it for a busy, sparkly
      look; lower it for occasional accents. 0 disables sparks even with Collision Spark on.
    $description:zh-CN: 每次碰撞产生火花的概率。调高会显得热闹闪烁，调低则只是偶尔点缀。
      设为 0 时即使开启碰撞火花也不会出现。
    $description:zh-TW: 每次碰撞產生火花的機率。調高會顯得熱鬧閃爍，調低則只是偶爾點綴。
      設為 0 時即使開啟碰撞火花也不會出現。
    $description:ja-JP: 衝突のたびに火花が出る確率。上げると賑やかに、下げると時折アクセントに。
      0 にすると衝突スパークを有効にしても出ません。
  - spark_intensity: 100
    $name: Spark Intensity
    $name:zh-CN: 火花强度
    $name:zh-TW: 火花強度
    $name:ja-JP: 火花強度
    $description: How many sparks a single collision produces and how fast they fly. 100 is the
      baseline; 0 means collisions are silent even with Collision Spark enabled.
    $description:zh-CN: 单次碰撞产生多少火花、以及飞得多快。100 为基准；设为 0 时即使开启碰撞火花也无声无息。
    $description:zh-TW: 單次碰撞產生多少火花、以及飛得多快。100 為基準；設為 0 時即使開啟碰撞火花也無聲無息。
    $description:ja-JP: 1 回の衝突で何個の火花が飛び、どの速さで飛ぶか。100 が基準。0 にすると
      衝突スパークを有効にしても何も起きません。
  - particle_mode: always
    $name: Particle Mode
    $name:zh-CN: 粒子模式
    $name:zh-TW: 粒子模式
    $name:ja-JP: パーティクルモード
    $options:
    - off: Off
    - fadeout: On Fadeout
    - always: Always (except idle)
    $options:zh-CN:
    - off: 关闭
    - fadeout: 淡出时
    - always: 始终（静止除外）
    $options:zh-TW:
    - off: 關閉
    - fadeout: 淡出時
    - always: 始終（靜止除外）
    $options:ja-JP:
    - off: オフ
    - fadeout: フェードアウト時
    - always: 常時（静止時除く）
  - particle_origin: head
    $name: Particle Origin
    $name:zh-CN: 粒子释放位置
    $name:zh-TW: 粒子釋放位置
    $name:ja-JP: パーティクル起点
    $description: Where on the trail particles are released.
    $description:zh-CN: 粒子从拖尾的哪个位置释放。
    $description:zh-TW: 粒子從拖尾的哪個位置釋放。
    $description:ja-JP: トレイルのどこからパーティクルを放出するか。
    $options:
    - head: Head (cursor)
    - middle: Middle
    - tail: Tail
    - custom: Custom Ratio
    - random: Random Ratio
    $options:zh-CN:
    - head: 开头（光标处）
    - middle: 中间
    - tail: 结尾
    - custom: 自定义比例
    - random: 随机比例
    $options:zh-TW:
    - head: 開頭（游標處）
    - middle: 中間
    - tail: 結尾
    - custom: 自訂比例
    - random: 隨機比例
    $options:ja-JP:
    - head: 先端（カーソル位置）
    - middle: 中間
    - tail: 末端
    - custom: カスタム比率
    - random: ランダム比率
  - particle_origin_ratio: 80
    $name: Custom Origin Ratio
    $name:zh-CN: 自定义释放比例
    $name:zh-TW: 自訂釋放比例
    $name:ja-JP: カスタム比率
    $description: Position along trail (0=head, 100=tail). Custom origin only.
    $description:zh-CN: 沿拖尾的位置比例（0=开头，100=结尾）。仅自定义位置生效。
    $description:zh-TW: 沿拖尾的位置比例（0=開頭，100=結尾）。僅自訂位置生效。
    $description:ja-JP: トレイル上の位置比率（0=先端、100=末端）。カスタム起点時のみ有効。
  - particle_density: 5
    $name: Particle Density
    $name:zh-CN: 粒子密度
    $name:zh-TW: 粒子密度
    $name:ja-JP: パーティクル密度
    $description: Number of particles per release (1-10).
    $description:zh-CN: 每次释放的粒子数量（1-10）。
    $description:zh-TW: 每次釋放的粒子數量（1-10）。
    $description:ja-JP: 放出ごとのパーティクル数（1-10）。
  - particle_size_multiplier: 150
    $name: Particle Size
    $name:zh-CN: 粒子大小
    $name:zh-TW: 粒子大小
    $name:ja-JP: パーティクルサイズ
    $description: Size multiplier for particles (50-300).
    $description:zh-CN: 粒子大小倍数（50-300）。
    $description:zh-TW: 粒子大小倍數（50-300）。
    $description:ja-JP: パーティクルサイズ倍率（50-300）。
  - enable_particle_glow: true
    $name: Particle Glow
    $name:zh-CN: 粒子发光
    $name:zh-TW: 粒子發光
    $name:ja-JP: パーティクルグロー
    $description: Soft outer halo around each particle.
    $description:zh-CN: 每个粒子外圈绘制柔和光晕。
    $description:zh-TW: 每個粒子外圈繪製柔和光暈。
    $description:ja-JP: 各パーティクル周囲のソフトなハロー。
  - particle_glow_intensity: 40
    $name: Particle Glow Intensity
    $name:zh-CN: 粒子发光强度
    $name:zh-TW: 粒子發光強度
    $name:ja-JP: グロー強度
    $description: Particle glow size and brightness (0-100).
    $description:zh-CN: 粒子发光范围和亮度（0-100）。
    $description:zh-TW: 粒子發光範圍和亮度（0-100）。
    $description:ja-JP: パーティクルグローの大きさと明るさ（0-100）。
  - enable_particle_stroke: false
    $name: Particle Stroke
    $name:zh-CN: 粒子描边
    $name:zh-TW: 粒子描邊
    $name:ja-JP: パーティクルストローク
    $description: Outline around text particles, layered with glow for clarity on light backgrounds.
    $description:zh-CN: 文字粒子外圈描边，与发光叠加，浅色背景下保持清晰轮廓。
    $description:zh-TW: 文字粒子外圈描邊，與發光疊加，淺色背景下保持清晰輪廓。
    $description:ja-JP: テキスト粒子の外枠ストローク。グローと重ねて明るい背景でも輪郭を明瞭に。
  - particle_stroke_width: 6
    $name: Stroke Width
    $name:zh-CN: 描边宽度
    $name:zh-TW: 描邊寬度
    $name:ja-JP: ストローク幅
    $description: Stroke thickness in atlas pixels (0-10).
    $description:zh-CN: 描边粗细，图集像素单位（0-10）。
    $description:zh-TW: 描邊粗細，圖集像素單位（0-10）。
    $description:ja-JP: ストローク太さ（アトラスピクセル、0-10）。
  - particle_stroke_alpha: 80
    $name: Stroke Alpha
    $name:zh-CN: 描边透明度
    $name:zh-TW: 描邊透明度
    $name:ja-JP: ストローク透明度
    $description: Stroke opacity (0-100).
    $description:zh-CN: 描边不透明度（0-100）。
    $description:zh-TW: 描邊不透明度（0-100）。
    $description:ja-JP: ストローク不透明度（0-100）。
  - particle_stroke_color: auto
    $name: Stroke Color
    $name:zh-CN: 描边颜色
    $name:zh-TW: 描邊顏色
    $name:ja-JP: ストローク色
    $description: auto = darkened particle color, or a hex RGB like 101010.
    $description:zh-CN: auto 表示粒子主色变暗；或填写十六进制 RGB，如 101010。
    $description:zh-TW: auto 表示粒子主色變暗；或填寫十六進位 RGB，如 101010。
    $description:ja-JP: auto は粒子色を暗くした色、または 101010 のような Hex RGB。
  - particle_interval: 0
    $name: Particle Interval
    $name:zh-CN: 粒子释放间隔
    $name:zh-TW: 粒子釋放間隔
    $name:ja-JP: 放出間隔
    $description: Minimum interval between releases (ms, 0-2000). 0 = every frame. Fast-move interpolation is enabled only at 10 ms or below and only on near-straight segments; above that no filler particles are inserted at all, so a sparse trail follows the real cursor path instead of drawing a straight line to it.
    $description:zh-CN: 粒子释放的最小时间间隔（毫秒，0-2000），0=每帧生成。快速移动插值仅在间隔≤10ms 且这一段轨迹接近直线时启用；超过 10ms 完全不插入补间粒子，因此稀疏的拖尾会贴合光标的真实路径，而不是拉出一条直线。
    $description:zh-TW: 粒子釋放的最小時間間隔（毫秒，0-2000），0=每影格生成。快速移動插值僅在間隔≤10ms 且這一段軌跡接近直線時啟用；超過 10ms 完全不插入補間粒子，因此稀疏的拖尾會貼合游標的真實路徑，而不是拉出一條直線。
    $description:ja-JP: 放出間隔の最小値（ms、0-2000）。0=毎フレーム。高速移動補間は間隔が10ms以下かつ軌跡がほぼ直線の区間のみ有効。10msを超えると補間粒子を一切挿入しないため、まばらな軌跡でもカーソルへ直線を引かず実際の経路に沿います。
  - particle_spawn_spread: 14
    $name: Spawn Position Spread
    $name:zh-CN: 生成位置扩散
    $name:zh-TW: 生成位置擴散
    $name:ja-JP: 生成位置拡散
    $description: Random spread radius around each spawn point (px, 0-100). 0 = all particles launch from the exact same point; higher values distribute them over a wider area, reducing clustering.
    $description:zh-CN: 每个释放点周围的随机扩散半径（像素，0-100）。0=所有粒子从同一点发射；数值越大分布越广，减少扎堆。
    $description:zh-TW: 每個釋放點周圍的隨機擴散半徑（像素，0-100）。0=所有粒子從同一點發射；數值越大分布越廣，減少扎堆。
    $description:ja-JP: 各放出点を中心としたランダム拡散半径（px、0-100）。0=全粒子が同一点から発生。値が大きいほど広く分布し、密集を抑えます。
  - particle_acceleration: true
    $name: Acceleration Effect
    $name:zh-CN: 加速度影响
    $name:zh-TW: 加速度影響
    $name:ja-JP: 加速度エフェクト
    $description: Particle initial velocity affected by mouse acceleration.
    $description:zh-CN: 粒子初速度受鼠标加速度影响，速度变化越大飞散越快。
    $description:zh-TW: 粒子初速度受滑鼠加速度影響，速度變化越大飛散越快。
    $description:ja-JP: パーティクル初速度がマウス加速度の影響を受けます。
  - particle_shape: flower
    $name: Particle Shape
    $name:zh-CN: 粒子形状
    $name:zh-TW: 粒子形狀
    $name:ja-JP: パーティクル形状
    $options:
    - random: Random Mix
    - circle: Circle
    - star: Star
    - hexagram: Hexagram
    - heart: Heart
    - diamond: Diamond
    - triangle: Triangle
    - flower: Flower
    - pentagon: Pentagon
    - hexagon: Hexagon
    - text: Text Character
    $options:zh-CN:
    - random: 随机混合
    - circle: 仅圆形
    - star: 仅五角星
    - hexagram: 仅六芒星
    - heart: 仅心形
    - diamond: 仅菱形
    - triangle: 仅三角形
    - flower: 仅花朵
    - pentagon: 仅五边形
    - hexagon: 仅六边形
    - text: 文字字符
    $options:zh-TW:
    - random: 隨機混合
    - circle: 僅圓形
    - star: 僅五角星
    - hexagram: 僅六芒星
    - heart: 僅心形
    - diamond: 僅菱形
    - triangle: 僅三角形
    - flower: 僅花朵
    - pentagon: 僅五邊形
    - hexagon: 僅六邊形
    - text: 文字字元
    $options:ja-JP:
    - random: ランダム
    - circle: サークル
    - star: スター
    - hexagram: 六芒星
    - heart: ハート
    - diamond: ダイヤ
    - triangle: トライアングル
    - flower: フラワー
    - pentagon: ペンタゴン
    - hexagon: ヘキサゴン
    - text: テキスト
  - enable_particle_spin: true
    $name: Particle Spin
    $name:zh-CN: 粒子自旋转
    $name:zh-TW: 粒子自旋轉
    $name:ja-JP: スピン
    $description: Enable random self-rotation for particles.
    $description:zh-CN: 启用粒子随机自旋转效果。
    $description:zh-TW: 啟用粒子隨機自旋轉效果。
    $description:ja-JP: パーティクルのランダム自転を有効にします。
  - particle_spin_speed: 30
    $name: Spin Speed
    $name:zh-CN: 自旋速度
    $name:zh-TW: 自旋速度
    $name:ja-JP: スピン速度
    $description: Base rotation speed for particles (0-100). Each particle gets random variation ±50%. Very high speed can make asymmetric shapes appear round due to motion blur.
    $description:zh-CN: 粒子基础自旋转速度（0-100），每个粒子有 ±50% 的随机差异。速度过高会导致三角形/星形等非对称形状因运动模糊看起来像圆形。
    $description:zh-TW: 粒子基礎自旋轉速度（0-100），每個粒子有 ±50% 的隨機差異。速度過高會導致三角形/星形等非對稱形狀因運動模糊看起來像圓形。
    $description:ja-JP: パーティクルの基本回転速度（0-100）。各パーティクルに±50%のランダム差があります。速度が高すぎると、三角形や星形などの非対称な形状もモーションブラーで円形に見えます。
  - enable_particle_interaction: true
    $name: Particle Interaction
    $name:zh-CN: 粒子间相互作用
    $name:zh-TW: 粒子間相互作用
    $name:ja-JP: パーティクル相互作用
    $description: Enable repulsion force between particles. Force scales with mass product, acceleration scales with 1/m. O(n²), auto-skipped when >500 particles.
    $description:zh-CN: 启用粒子之间的排斥力。力与质量乘积成正比，加速度与质量成反比。O(n²)复杂度，粒子>500时自动跳过。
    $description:zh-TW: 啟用粒子之間的排斥力。力與質量乘積成正比，加速度與質量成反比。O(n²)複雜度，粒子>500時自動跳過。
    $description:ja-JP: パーティクル間の反発力を有効にします。力は質量の積に比例し、加速度は 1/m に比例します。計算量はO(n²)で、パーティクルが500個を超えると自動的にスキップされます。
  - particle_repel_distance: 20
    $name: Inter-Particle Repel Distance
    $name:zh-CN: 粒子间排斥距离
    $name:zh-TW: 粒子間排斥距離
    $name:ja-JP: 粒子間反発距離
    $description: Distance (px, 5-100) within which particles repel each other. Separate from Cursor Repulsion Radius. Force uses quadratic falloff — strongest at contact, zero at this boundary.
    $description:zh-CN: 粒子之间产生排斥力的距离（像素，5-100）。与"光标排斥半径"是两套独立设置。力采用二次衰减：接触处最强，到达此距离时降为零。
    $description:zh-TW: 粒子之間產生排斥力的距離（像素，5-100）。與"游標排斥半徑"是兩套獨立設置。力採用二次衰減：接觸處最強，到此距離時降為零。
    $description:ja-JP: パーティクル同士が反発し合う距離（px、5-100）。「カーソル反発半径」とは別の設定です。力は二次減衰：接触点で最強、境界でゼロ。
  - particle_inter_repel_force: 20
    $name: Inter-Particle Repel Strength
    $name:zh-CN: 粒子间排斥强度
    $name:zh-TW: 粒子間排斥強度
    $name:ja-JP: 粒子間反発強度
    $description: Strength of inter-particle repulsion (0-100). Internal scale — value/12, so 60 ≈ 5.0 base force. Mass uses fourth-root scaling (1.5-2.7x for masses 5-50), force clamped at 20. Low (5-20) subtle separation, mid (20-60) visible pushing, high (60-100) strong scattering. Enable Debug Interaction to visualize repulsion links.
    $description:zh-CN: 粒子之间排斥力的强度（0-100）。内部缩放为 值/12，即 60≈5.0 基础力。质量采用四次方根缩放（质量5-50时仅1.5-2.7倍），力上限20。低值（5-20）轻微分离，中值（20-60）明显推开，高值（60-100）强力散射。可开启"调试-粒子相互作用"可视化排斥连线。
    $description:zh-TW: 粒子之間排斥力的強度（0-100）。內部縮放為 值/12，即 60≈5.0 基礎力。質量採用四次方根縮放（質量5-50時僅1.5-2.7倍），力上限20。低值（5-20）輕同分離，中值（20-60）明顯推開，高值（60-100）強力散射。可開啟"除錯-粒子相互作用"可視化排斥連線。
    $description:ja-JP: パーティクル間反発力の強さ（0-100）。内部スケールは値/12、60で約5.0。質量は四乗根スケーリング（質量5-50で1.5-2.7倍）、力の上限は20。低値（5-20）は軽い分離、中値（20-60）ははっきりした押し合い、高値（60-100）は強い散乱。「デバッグ-粒子相互作用」で反発リンクを可視化できます。
  - particle_repel_alignment: 0
    $name: Alignment Damping
    $name:zh-CN: 同向运动衰减
    $name:zh-TW: 同向運動衰減
    $name:ja-JP: 同向減衰
    $description: When two particles move in roughly the same direction, reduce both repulsion strength and range (0-100). 0 = no reduction, 100 = repulsion fully canceled at perfect alignment. Helps particles form coherent streams instead of scattering apart.
    $description:zh-CN: 当两个粒子运动方向接近一致时，同时降低排斥强度和排斥范围（0-100）。0=不衰减，100=完全同向时排斥归零。有助于粒子形成连贯的流束而非互相散开。
    $description:zh-TW: 當兩個粒子運動方向接近一致時，同時降低排斥強度和排斥範圍（0-100）。0=不衰減，100=完全同向時排斥歸零。有助於粒子形成連貫的流束而非互相散開。
    $description:ja-JP: 2つのパーティクルの進行方向がほぼ一致するとき、反発力と範囲の両方を低減（0-100）。0=低減なし、100=完全一致時に反発ゼロ。パーティクルがバラけずにまとまった流れを作るのに役立ちます。
  - particle_attraction: 15
    $name: Particle Attraction
    $name:zh-CN: 粒子吸附强度
    $name:zh-TW: 粒子吸附強度
    $name:ja-JP: 吸引力
    $description: How strongly particles are pulled toward the cursor (0-100). Linear scale, mass-independent. Only particles within 500px are affected, with strength fading to zero at the edge. Low (5-20) subtle following, mid (20-50) visible pull, high (50-100) strong magnetic snap.
    $description:zh-CN: 粒子被吸向光标的强度（0-100）。线性缩放，与质量无关。仅 500px 范围内的粒子受影响，强度随距离衰减至零。低值（5-20）轻微跟随，中值（20-50）明显吸引，高值（50-100）强力磁吸。
    $description:zh-TW: 粒子被吸向游標的強度（0-100）。線性縮放，與質量無關。僅 500px 範圍內的粒子受影響，強度隨距離衰減至零。低值（5-20）輕微跟隨，中值（20-50）明顯吸引，高值（50-100）強力磁吸。
    $description:ja-JP: パーティクルがカーソルに引き寄せられる強さ（0-100）。線形スケール、質量に依存しません。500px以内のパーティクルのみ影響を受け、端部で強度がゼロまで減衰。低値（5-20）は軽い追従、中値（20-50）ははっきりした引き寄せ、高値（50-100）は強い磁気吸着。
  - enable_particle_repel: true
    $name: Cursor Repulsion
    $name:zh-CN: 光标排斥力
    $name:zh-TW: 游標排斥力
    $name:ja-JP: カーソル反発
    $description: Particles near cursor are repelled, creating orbiting motion.
    $description:zh-CN: 粒子靠近光标时被排斥弹开，形成绕飞效果。
    $description:zh-TW: 粒子靠近游標時被排斥彈開，形成繞飛效果。
    $description:ja-JP: カーソル付近のパーティクルが反発され周回運動を生みます。
  - particle_repel_radius: 50
    $name: Cursor Repulsion Radius
    $name:zh-CN: 光标排斥半径
    $name:zh-TW: 游標排斥半徑
    $name:ja-JP: カーソル反発半径
    $description: Radius (px, 5-100) around cursor within which particles are pushed away. Separate from Inter-Particle Repel Distance. Creates orbiting motion around the cursor.
    $description:zh-CN: 光标周围的排斥半径（像素，5-100），粒子进入此范围被弹开形成绕飞。与"粒子间排斥距离"是两套独立设置。
    $description:zh-TW: 游標周圍的排斥半徑（像素，5-100），粒子進入此範圍被彈開形成繞飛。與"粒子間排斥距離"是兩套獨立設置。
    $description:ja-JP: カーソル周囲の反発半径（px、5-100）。この範囲内のパーティクルが押し出され周回運動を生みます。「粒子間反発距離」とは別の設定です。
  - particle_repel_force: 30
    $name: Cursor Repulsion Strength
    $name:zh-CN: 光标排斥强度
    $name:zh-TW: 游標排斥強度
    $name:ja-JP: カーソル反発強度
    $description: Strength of cursor repulsion and random perturbation (0-100). Controls how hard particles are pushed away from the cursor and the amount of directional jitter.
    $description:zh-CN: 光标排斥力和随机扰动的强度（0-100）。控制粒子被光标牌开的力度和方向抖动量。
    $description:zh-TW: 游標排斥力和隨機擾動的強度（0-100）。控制粒子被游標彈開的力度和方向抖動量。
    $description:ja-JP: カーソル反発力とランダム摂動の強さ（0-100）。パーティクルがカーソルから押し出される強さと方向ジッタ量を制御。
  $name: Particles
  $name:zh-CN: 粒子
  $name:zh-TW: 粒子
  $name:ja-JP: パーティクル

- ShapesText:
  - text_content: Hi
    $name: Text Content
    $name:zh-CN: 文字内容
    $name:zh-TW: 文字內容
    $name:ja-JP: テキスト内容
    $description: Characters to scatter along the trail when particle shape is set to Text. Letters, digits,
      CJK characters and emoji all work. Use ',' '，' '|' or '｜' to separate multiple entries; each entry
      counts as one phrase and the phrases take turns appearing, and [tag] placeholders work too. Built-in
      ones are [time], [day], [datetime] and [weekday]. An inline format such as [time:%H:%M] wins over
      everything else. Escape with \[time] to get a literal "[time]"; an unknown id is kept verbatim so
      typos stay visible.
    $description:zh-CN: 粒子形状设为文字字符时沿拖尾散落的字符，支持字母、数字、汉字和 Emoji。多个词组之间用分隔符 ','、'，'、'|'
      或 '｜' 隔开，每一段算一个词组，词组轮流出现。同时支持 [tag] 占位符：[time]、[day]、[datetime]、[weekday]。
      内联格式优先级最高，例如 [time:%H:%M]。写 \[time] 会输出字面 "[time]"；未知标签原样保留，方便发现拼写错误。
    $description:zh-TW: 粒子形狀設為文字字元時沿拖尾散落的字元，支援字母、數字、漢字和 Emoji。多個詞組之間用分隔符 ','、'，'、'|'
      或 '｜' 隔開，每一段算一個詞組，詞組輪流出現。同時支援 [tag] 佔位符：[time]、[day]、[datetime]、[weekday]。
      內聯格式優先級最高，例如 [time:%H:%M]。寫 \[time] 會輸出字面 "[time]"；未知標籤原樣保留，方便發現拼寫錯誤。
    $description:ja-JP: パーティクル形状をテキストに設定したときに軌跡に散らばる文字。英数字、漢字、絵文字に対応します。
      複数の語句は ',' '，' '|' '｜' で区切ります。区切った各エントリが1つのフレーズとなり、フレーズは順番に切り替わります。
      タグ形式のプレースホルダにも対応（[time] [day] [datetime] [weekday]）。[time:%H:%M] のようなインライン形式が
      最優先。\[time] と書くとリテラルの "[time]" になります。未知のタグはそのまま表示されるため誤字に気づけます。
  - text_tag_mode: dynamic
    $name: Tag Update Mode
    $name:zh-CN: 标签更新模式
    $name:zh-TW: 標籤更新模式
    $name:ja-JP: タグ更新モード
    $options:
    - static: Static (freeze at read)
    - dynamic: Dynamic (follow changes)
    $options:zh-CN:
    - static: 静态（读到即冻结）
    - dynamic: 动态（跟随变化）
    $options:zh-TW:
    - static: 靜態（讀到即凍結）
    - dynamic: 動態（跟隨變化）
    $options:ja-JP:
    - static: 静的（読み取り時に固定）
    - dynamic: 動的（変化に追従）
    $description: Whether [tag] values keep updating or are frozen at the value first read. Format priority
      (highest first) is inline > this mode's per-tag setting > the ISO 8601 default. Switching back to
      static re-freezes, since changing either mode or format invalidates the frozen values.
    $description:zh-CN: 标签值的更新方式：是持续更新，还是冻结在首次读取到的值。格式优先级（高→低）：内联 > 本节的每标签
      设置 > ISO 8601 默认。改回静态会重新冻结——修改模式或格式都会让已冻结的值失效。
    $description:zh-TW: 標籤值的更新方式：是持續更新，還是凍結在首次讀取到的值。格式優先級（高→低）：內聯 > 本節的每標籤
      設定 > ISO 8601 預設。改回靜態會重新凍結——修改模式或格式都會讓已凍結的值失效。
    $description:ja-JP: タグ値の扱い：更新し続けるか、最初に読み取った値で固定するか。書式の優先度（高→低）はインライン >
      このモードのタグ別設定 > ISO 8601 既定値。モードや書式を変更すると固定値は破棄される。
  - text_tag_time_format: ''
    $name: '[time] Format'
    $name:zh-CN: '[time] 格式'
    $name:zh-TW: '[time] 格式'
    $name:ja-JP: '[time] 形式'
    $description: strftime format for [time]. Empty uses ISO 8601 (HH:MM:SS). Supported specifiers are %H %I
      %M %S %p %Y %y %m %b %B %d %a %A %j %U %W %w %c %x %X %z %Z %%, plus aliases %T(=HH:MM:SS) %R(=HH:MM)
      %F(=YYYY-MM-DD) %D(=MM/DD/YY). Anything else renders empty and is logged. %a/%b use the C locale, so
      they are always English.
    $description:zh-CN: 时间格式：strftime 格式串，留空使用 ISO 8601（时:分:秒）。支持 %H %I %M %S %p %Y %y %m %b %B
      %d %a %A %j %U %W %w %c %x %X %z %Z %%，以及别名 %T(=时:分:秒) %R(=时:分) %F(=年-月-日) %D(=月/日/年)。
      其余说明符输出为空并写入日志。%a/%b 固定使用 C locale，输出恒为英文。
    $description:zh-TW: 時間格式：strftime 格式串，留空使用 ISO 8601（時:分:秒）。支援 %H %I %M %S %p %Y %y %m %b %B
      %d %a %A %j %U %W %w %c %x %X %z %Z %%，以及別名 %T(=時:分:秒) %R(=時:分) %F(=年-月-日) %D(=月/日/年)。
      其餘說明符輸出為空並寫入日誌。%a/%b 固定使用 C locale，輸出恆為英文。
    $description:ja-JP: 時刻書式：strftime 書式。空欄なら ISO 8601（時:分:秒）。%H %I %M %S %p %Y %y %m %b %B %d %a %A
      %j %U %W %w %c %x %X %z %Z %% と別名 %T(=時:分:秒) %R(=時:分) %F(=年-月-日) %D(=月/日/年) に対応。
      その他は空文字になりログに記録される。%a/%b は C ロケール固定。
  - text_tag_day_format: ''
    $name: '[day] Format'
    $name:zh-CN: '[day] 格式'
    $name:zh-TW: '[day] 格式'
    $name:ja-JP: '[day] 形式'
    $description: strftime format for [day]. Empty uses ISO 8601 (YYYY-MM-DD). Same supported set as [time];
      %Y %m %d %b %B %a %A %j %U %W %w are the useful ones. Anything else renders empty and is logged.
    $description:zh-CN: 日期格式：strftime 格式串，留空使用 ISO 8601（年-月-日）。支持集与时间格式相同，常用 %Y %m %d
      %b %B %a %A %j %U %W %w。其余说明符输出为空并写入日志。
    $description:zh-TW: 日期格式：strftime 格式串，留空使用 ISO 8601（年-月-日）。支援集與時間格式相同，常用 %Y %m %d
      %b %B %a %A %j %U %W %w。其餘說明符輸出為空並寫入日誌。
    $description:ja-JP: 日付書式：strftime 書式。空欄なら ISO 8601（年-月-日）。対応書式は時刻と共通、よく使うのは
      %Y %m %d %b %B %a %A %j %U %W %w。その他は空文字になりログに記録される。
  - text_font_size: 24
    $name: Font Size
    $name:zh-CN: 字号
    $name:zh-TW: 字級
    $name:ja-JP: フォントサイズ
    $description: Character on-screen size in pixels (10-200) when particle shape is Text.
    $description:zh-CN: 粒子形状设为文字字符时的字符屏幕大小（像素，10-200）。
    $description:zh-TW: 粒子形狀設為文字字元時的螢幕大小（像素，10-200）。
    $description:ja-JP: パーティクル形状をテキストに設定時の文字の画面サイズ（ピクセル、10-200）。
  - text_atlas_ss: 2x
    $name: Text Atlas Supersampling
    $name:zh-CN: 文字图集超采样
    $name:zh-TW: 文字圖集超取樣
    $name:ja-JP: テキストアトラス超解像
    $description: Resolution multiplier for the offscreen glyph atlas. Higher values keep text/emoji crisp when particles are large, at the cost of VRAM and rebuild time.
    $description:zh-CN: 离屏字形图集的分辨率倍数。数值越高，大尺寸文字/Emoji 越清晰，代价是显存占用与重建耗时。
    $description:zh-TW: 離屏字形圖集的解析度倍數。數值越高，大尺寸文字/Emoji 越清晰，代價是顯存佔用與重建耗時。
    $description:ja-JP: グリフアトラスの解像度倍率。大きいほど大きな文字/絵文字が鮮明になります（VRAMと再構築時間が増加）。
    $options:
    - 1x: 1x
    - 2x: 2x
    - 3x: 3x
    - 4x: 4x
    $options:zh-CN:
    - 1x: 1x
    - 2x: 2x
    - 3x: 3x
    - 4x: 4x
    $options:zh-TW:
    - 1x: 1x
    - 2x: 2x
    - 3x: 3x
    - 4x: 4x
    $options:ja-JP:
    - 1x: 1x
    - 2x: 2x
    - 3x: 3x
    - 4x: 4x
  - enable_text_chunk: true
    $name: Long Text Chunking
    $name:zh-CN: 长句分段
    $name:zh-TW: 長句分段
    $name:ja-JP: 長文分割
    $description: Split long text into short chunks and cycle through them, instead of rendering one long straight string. Emoji are kept intact.
    $description:zh-CN: 将长句切分成短段并循环切换显示，避免一整句又直又长。Emoji 表情不会被切断。
    $description:zh-TW: 將長句切分成短段並循環切換顯示，避免一整句又直又長。Emoji 表情不會被切斷。
    $description:ja-JP: 長い文を短いチャンクに分割して循環表示し、一直手に長く表示されるのを防ぎます。絵文字は分断されません。
  - text_chunk_mode: particle
    $name: Chunk Cycle Mode
    $name:zh-CN: 分段轮播模式
    $name:zh-TW: 分段輪播模式
    $name:ja-JP: チャンク切替モード
    $description: "Per Particle: each particle cycles chunks on its own age (scattered fragments). Global Sync: all particles show and switch the same chunk together."
    $description:zh-CN: 逐粒子：每个粒子按自身存活时间独立切换（散落碎片感）。全局同步：所有粒子同一时刻一起显示并切换同一段。
    $description:zh-TW: 逐粒子：每個粒子按自身存活時間獨立切換（散落碎片感）。全域同步：所有粒子同一時刻一起顯示並切換同一段。
    $description:ja-JP: パーティクル毎：各パーティクルが独自の経過時間で切替（散らばる破片風）。全体同期：全パーティクルが同時に同じチャンクを表示・切替。
    $options:
    - particle: Per Particle
    - global: Global Sync
    $options:zh-CN:
    - particle: 逐粒子
    - global: 全局同步
    $options:zh-TW:
    - particle: 逐粒子
    - global: 全域同步
    $options:ja-JP:
    - particle: パーティクル毎
    - global: 全体同期
  - text_chunk_max: 3
    $name: Max Chars Per Chunk
    $name:zh-CN: 每段最多字数
    $name:zh-TW: 每段最多字數
    $name:ja-JP: 1チャンクの最大文字数
    $description: Maximum number of characters per chunk (1-8). Longer text is split at this width.
    $description:zh-CN: 每段最多显示几个字（1-8），超过此长度的文字按此宽度切分。
    $description:zh-TW: 每段最多顯示幾個字（1-8），超過此長度的文字按此寬度切分。
    $description:ja-JP: 1チャンクあたりの最大文字数（1-8）。この幅で長文を分割します。
  - text_chunk_delay: 600
    $name: Chunk Switch Delay (ms)
    $name:zh-CN: 分段切换延迟（毫秒）
    $name:zh-TW: 分段切換延遲（毫秒）
    $name:ja-JP: チャンク切替間隔（ミリ秒）
    $description: Time each chunk stays before switching to the next (100-3000 ms).
    $description:zh-CN: 每段停留多久后切换到下一段（100-3000 毫秒）。
    $description:zh-TW: 每段停留多久後切換到下一段（100-3000 毫秒）。
    $description:ja-JP: 次のチャンクへ切り替わるまでの表示時間（100-3000 ミリ秒）。
  - text_offset_x: 0
    $name: Text Offset X
    $name:zh-CN: 文字水平偏移
    $name:zh-TW: 文字水平偏移
    $name:ja-JP: テキスト左右オフセット
    $description: Horizontal spawn offset of text particles relative to the cursor (-100 to 100 px).
    $description:zh-CN: 文字粒子相对光标的水平生成位置偏移（-100 到 100 像素）。
    $description:zh-TW: 文字粒子相對游標的水平生成位置偏移（-100 到 100 像素）。
    $description:ja-JP: テキストパーティクルのカーソルに対する左右の生成オフセット（-100〜100 px）。
  - text_offset_y: 0
    $name: Text Offset Y
    $name:zh-CN: 文字垂直偏移
    $name:zh-TW: 文字垂直偏移
    $name:ja-JP: テキスト上下オフセット
    $description: Vertical spawn offset of text particles relative to the cursor (-100 to 100 px).
    $description:zh-CN: 文字粒子相对光标的垂直生成位置偏移（-100 到 100 像素）。
    $description:zh-TW: 文字粒子相對游標的垂直生成位置偏移（-100 到 100 像素）。
    $description:ja-JP: テキストパーティクルのカーソルに対する上下の生成オフセット（-100〜100 px）。
  - missile_idle_explode: false
    $name: Craft Explode When Idle
    $name:zh-CN: 静止时引爆飞行器
    $name:zh-TW: 靜止時引爆飛行器
    $name:ja-JP: 静止時に機体を爆発
    $description: Rocket / missile shapes detonate when the pointer has been still for the delay below.
      Only applies to the craft shapes, not to hearts or stars.
    $description:zh-CN: 鼠标静止达到下方延迟后，火箭/导弹/光纤导弹会引爆。仅作用于飞行器类形状，爱心等不受影响。
    $description:zh-TW: 滑鼠靜止達到下方延遲後，火箭/飛彈/光纖飛彈會引爆。僅作用於飛行器類形狀，愛心等不受影響。
    $description:ja-JP: ポインタが下の時間だけ静止すると、ロケット/ミサイル/光ファイバーミサイルが爆発します。
      対象は機体シェイプのみで、ハートなどには影響しません。
  - missile_idle_delay: 400
    $name: Idle Detonation Delay
    $name:zh-CN: 引爆延迟
    $name:zh-TW: 引爆延遲
    $name:ja-JP: 爆発までの時間
    $description: How long the pointer must stay still before craft shapes detonate (ms, 100-5000).
    $description:zh-CN: 鼠标需静止多久才引爆飞行器（毫秒，100-5000）。
    $description:zh-TW: 滑鼠需靜止多久才引爆飛行器（毫秒，100-5000）。
    $description:ja-JP: 機体が爆発するまでにポインタが静止している必要がある時間（ミリ秒、100-5000）。
  - missile_explode_particles: 12
    $name: Explosion Particles
    $name:zh-CN: 爆炸粒子数
    $name:zh-TW: 爆炸粒子數
    $name:ja-JP: 爆発パーティクル数
    $description: Particles thrown out by each detonation (0-40). Set 0 to release no particles.
    $description:zh-CN: 每次爆炸抛出的粒子数量（0-40），设为 0 则不释放粒子。
    $description:zh-TW: 每次爆炸拋出的粒子數量（0-40），設為 0 則不釋放粒子。
    $description:ja-JP: 爆発ごとに放出されるパーティクル数（0-40）。0 にすると粒子を出しません。
  - missile_explode_fx: both
    $name: Explosion Effect
    $name:zh-CN: 爆炸特效
    $name:zh-TW: 爆炸特效
    $name:ja-JP: 爆発エフェクト
    $options:
    - particles: Particles
    - shockwave: Shockwave Ring
    - both: Particles + Shockwave
    $options:zh-CN:
    - particles: 仅粒子
    - shockwave: 仅冲击波
    - both: 粒子 + 冲击波
    $options:zh-TW:
    - particles: 僅粒子
    - shockwave: 僅衝擊波
    - both: 粒子 + 衝擊波
    $options:ja-JP:
    - particles: パーティクルのみ
    - shockwave: 衝撃波のみ
    - both: パーティクル + 衝撃波
    $description: What a detonation emits. The shockwave reuses the click ripple style, so it follows
      Click Effect Style if you also enabled click ripples.
    $description:zh-CN: 爆炸时释放什么。冲击波复用点击波纹的绘制，因此会跟随点击效果样式的设置。
    $description:zh-TW: 爆炸時釋放什麼。衝擊波複用點擊波紋的繪製，因此會跟隨點擊效果樣式的設定。
    $description:ja-JP: 爆発時に何を放出するか。衝撃波はクリック波紋の描画を再利用するため、
      クリック効果スタイルの設定に従います。
  - shape_type: heart
    $name: Shape Type
    $name:zh-CN: 拖尾形状类型
    $name:zh-TW: 拖尾形狀類型
    $name:ja-JP: シェイプタイプ
    $description: Shape used for Shape Trail mode.
    $description:zh-CN: 形状拖尾模式下使用的形状。
    $description:zh-TW: 形狀拖尾模式下使用的形狀。
    $description:ja-JP: シェイプトレイルモードで使用する形状。
    $options:
    - heart: Heart
    - star: Star
    - hexagon: Hexagon
    - circle: Circle
    - diamond: Diamond
    - triangle: Triangle
    - flower: Flower
    - pentagon: Pentagon
    - hexagram: Hexagram
    - rocket: Space Rocket
    - missile: Missile
    - fiber_missile: Fiber Missile
    - random: Random Mix
    $options:zh-CN:
    - heart: 爱心
    - star: 五角星
    - hexagon: 六边形
    - circle: 圆形
    - diamond: 菱形
    - triangle: 三角形
    - flower: 花朵
    - pentagon: 五边形
    - hexagram: 六芒星
    - rocket: 太空火箭
    - missile: 导弹
    - fiber_missile: 光纤导弹
    - random: 随机混合
    $options:zh-TW:
    - heart: 愛心
    - star: 五角星
    - hexagon: 六邊形
    - circle: 圓形
    - diamond: 菱形
    - triangle: 三角形
    - flower: 花朵
    - pentagon: 五邊形
    - hexagram: 六芒星
    - rocket: 太空火箭
    - missile: 導彈
    - fiber_missile: 光纖導彈
    - random: 隨機混合
    $options:ja-JP:
    - heart: ハート
    - star: スター
    - hexagon: 六角形
    - circle: サークル
    - diamond: ダイヤ
    - triangle: トライアングル
    - flower: フラワー
    - pentagon: 五角形
    - hexagram: 六芒星
    - rocket: ロケット
    - missile: ミサイル
    - fiber_missile: 光ファイバーミサイル
    - random: ランダム
  - shape_interval: 30
    $name: Shape Interval
    $name:zh-CN: 形状生成间隔
    $name:zh-TW: 形狀生成間隔
    $name:ja-JP: シェイプ間隔
    $description: Distance between shapes in pixels (10-100).
    $description:zh-CN: 形状之间的间隔距离（像素，10-100）。
    $description:zh-TW: 形狀之間的間隔距離（像素，10-100）。
    $description:ja-JP: 形状間の距離（ピクセル、10-100）。
  - shape_random_offset: true
    $name: Random Position Offset
    $name:zh-CN: 随机位置微调
    $name:zh-TW: 隨機位置微調
    $name:ja-JP: ランダムオフセット
    $description: Add small random offset to each shape position.
    $description:zh-CN: 为每个形状位置添加小范围随机偏移，更自然。
    $description:zh-TW: 為每個形狀位置添加小範圍隨機偏移，更自然。
    $description:ja-JP: 各形状位置に小さなランダムオフセットを追加。
  - shape_count: 1
    $name: Shape Count
    $name:zh-CN: 每次生成数量
    $name:zh-TW: 每次生成數量
    $name:ja-JP: 生成数
    $description: Number of shapes per spawn (1-5).
    $description:zh-CN: 每次生成的形状数量（1-5）。
    $description:zh-TW: 每次生成的形狀數量（1-5）。
    $description:ja-JP: 生成ごとの形状数（1-5）。
  - shape_size: 12
    $name: Shape Size
    $name:zh-CN: 形状大小
    $name:zh-TW: 形狀大小
    $name:ja-JP: 形状サイズ
    $description: Base size of shapes in pixels (5-60). Larger values help the slender rocket/missile shapes.
    $description:zh-CN: 形状的基础大小（像素，5-60）。火箭/导弹这类细长形状建议调大。
    $description:zh-TW: 形狀的基礎大小（像素，5-60）。火箭/飛彈這類細長形狀建議調大。
    $description:ja-JP: 形状の基本サイズ（ピクセル、5-60）。ロケット/ミサイルのような細長い形は大きめがおすすめ。
  - shape_lifetime: 800
    $name: Shape Lifetime
    $name:zh-CN: 形状存活时间
    $name:zh-TW: 形狀存活時間
    $name:ja-JP: 形状寿命
    $description: How long each shape lasts (ms, 200-2000).
    $description:zh-CN: 每个形状的存活时间（毫秒，200-2000）。
    $description:zh-TW: 每個形狀的存活時間（毫秒，200-2000）。
    $description:ja-JP: 各形状の持続時間（ms、200-2000）。
  - dots_multiplier: 2
    $name: Dot Chain Density
    $name:zh-CN: 圆点链密度
    $name:zh-TW: 圓點鏈密度
    $name:ja-JP: ドット密度
    $description: Dot count multiplier (1-5), higher = more smaller dots.
    $description:zh-CN: 圆点链的小球数量倍率（1-5），越大小球越多越密。
    $description:zh-TW: 圓點鏈的小球數量倍率（1-5），越大小球越多越密。
    $description:ja-JP: ドット数の倍率（1-5）。大きいほど小さなドットが密に。
  - dot_chain_size: 100
    $name: Dot Chain Size
    $name:zh-CN: 圆点大小
    $name:zh-TW: 圓點大小
    $name:ja-JP: ドットサイズ
    $description: Size multiplier for dot chain trail (50-300).
    $description:zh-CN: 圆点链拖尾的大小倍数（50-300）。
    $description:zh-TW: 圓點鏈拖尾的大小倍數（50-300）。
    $description:ja-JP: ドットチェーントレイルのサイズ倍率（50-300）。
  - function_preset: sine
    $name: Function Preset
    $name:zh-CN: 函数预设
    $name:zh-TW: 函數預設
    $name:ja-JP: 関数プリセット
    $description: Preset formula for Function Curve mode.
    $description:zh-CN: 函数曲线模式的预设公式。
    $description:zh-TW: 函數曲線模式的預設公式。
    $description:ja-JP: 関数カーブモードのプリセット公式。
    $options:
    - sine: Sine Wave
    - damped: Damped
    - beat: Heartbeat
    - swirl: Swirl
    - custom: Custom
    $options:zh-CN:
    - sine: 标准正弦
    - damped: 阻尼衰减
    - beat: 心跳脉冲
    - swirl: 双频漩涡
    - custom: 自定义公式
    $options:zh-TW:
    - sine: 標準正弦
    - damped: 阻尼衰減
    - beat: 心跳脈衝
    - swirl: 雙頻漩渦
    - custom: 自訂公式
    $options:ja-JP:
    - sine: サイン波
    - damped: 減衰
    - beat: ハートビート
    - swirl: スワール
    - custom: カスタム
  - custom_function: sin(d * 0.15) * 8
    $name: Custom Function
    $name:zh-CN: 自定义函数公式
    $name:zh-TW: 自訂函數公式
    $name:ja-JP: カスタム関数
    $description: "Variables: t(0-1) d(distance) time(sec); Functions: sin cos exp sqrt abs."
    $description:zh-CN: "变量 t(0-1) d(距离) time(秒)；函数 sin cos exp sqrt abs。"
    $description:zh-TW: "變數 t(0-1) d(距離) time(秒)；函數 sin cos exp sqrt abs。"
    $description:ja-JP: "変数: t(0-1) d(距離) time(秒); 関数: sin cos exp sqrt abs。"
  - wave_amplitude: 8
    $name: Wave Amplitude
    $name:zh-CN: 波浪幅度
    $name:zh-TW: 波浪幅度
    $name:ja-JP: 波の振幅
    $description: Wave amplitude in pixels.
    $description:zh-CN: 波浪曲线的振幅（像素）。
    $description:zh-TW: 波浪曲線的振幅（像素）。
    $description:ja-JP: 波の振幅（ピクセル）。
  - wave_frequency: 15
    $name: Wave Frequency
    $name:zh-CN: 波浪频率
    $name:zh-TW: 波浪頻率
    $name:ja-JP: 波の周波数
    $description: Wave frequency (3-60, higher = denser waves).
    $description:zh-CN: 波浪曲线的频率（3-60，越大波浪越密）。
    $description:zh-TW: 波浪曲線的頻率（3-60，越大波浪越密）。
    $description:ja-JP: 波の周波数（3-60、大きいほど密）。
  $name: Shapes & Text
  $name:zh-CN: 形状与文字
  $name:zh-TW: 形狀與文字
  $name:ja-JP: 図形とテキスト

- Physics:
  - enable_centripetal: true
    $name: Centripetal Vortex
    $name:zh-CN: 向心力漩涡
    $name:zh-TW: 向心力漩渦
    $name:ja-JP: 求心ボルテックス
    $description: Curved mouse motion captures particles into orbiting vortex. Includes angular momentum conservation, Kepler velocity gradient, orbital precession, 3D inclination.
    $description:zh-CN: 鼠标做曲线运动时粒子被捕获形成旋转漩涡。物理模型：角动量守恒、开普勒速度梯度、轨道进动、3D轨道倾角。
    $description:zh-TW: 滑鼠做曲線運動時粒子被捕獲形成旋轉漩渦。物理模型：角動量守恆、開普勒速度梯度、軌道進動、3D軌道傾角。
    $description:ja-JP: 曲線的なマウス運動がパーティクルを捕捉し、周回するボルテックスを形成します。角運動量保存、ケプラー速度勾配、軌道の歳差運動、3D軌道傾斜を含む物理モデルです。
  - centripetal_force: 20
    $name: Vortex Force
    $name:zh-CN: 漩涡强度
    $name:zh-TW: 漩渦強度
    $name:ja-JP: ボルテックス力
    $description: Centripetal force strength, orbit speed and spring constraint (0-100).
    $description:zh-CN: 向心力强度、轨道速度和弹簧约束（0-100）。
    $description:zh-TW: 向心力強度、軌道速度和彈簧約束（0-100）。
    $description:ja-JP: 求心力の強さ、軌道速度、バネ拘束（0-100）。
  - centripetal_sensitivity: 40
    $name: Detection Sensitivity
    $name:zh-CN: 检测灵敏度
    $name:zh-TW: 偵測靈敏度
    $name:ja-JP: 検出感度
    $description: How easily curved motion is detected (0-100, lower = more sensitive).
    $description:zh-CN: 曲线运动检测灵敏度（0-100，越低越灵敏）。
    $description:zh-TW: 曲線運動偵測靈敏度（0-100，越低越靈敏）。
    $description:ja-JP: 曲線運動の検出感度（0-100、低いほど高感度）。
  - centripetal_duration: 1500
    $name: Vortex Duration
    $name:zh-CN: 漩涡持续时间
    $name:zh-TW: 漩渦持續時間
    $name:ja-JP: ボルテックス持続時間
    $description: How long vortex persists after curved motion stops (ms, 500-5000).
    $description:zh-CN: 停止曲线运动后漩涡持续时间（毫秒，500-5000）。
    $description:zh-TW: 停止曲線運動後漩渦持續時間（毫秒，500-5000）。
    $description:ja-JP: 曲線運動停止後の持続時間（ms、500-5000）。
  - vortex_max_count: 4
    $name: Max Vortex Count
    $name:zh-CN: 最大漩涡数量
    $name:zh-TW: 最大漩渦數量
    $name:ja-JP: 最大ボルテックス数
    $description: Maximum simultaneous vortices (1-8). New vortex only spawns when an existing one fully decays.
    $description:zh-CN: 同时存在的最大漩涡数量（1-8）。只有现有漩涡完全衰减后才能生成新漩涡。
    $description:zh-TW: 同時存在的最大漩渦數量（1-8）。只有現有漩渦完全衰減後才能生成新漩渦。
    $description:ja-JP: 同時に存在できるボルテックスの最大数（1-8）。既存のボルテックスが完全に減衰するまで、新しいボルテックスは生成されません。
  - vortex_min_distance: 200
    $name: Vortex Min Distance
    $name:zh-CN: 漩涡最小间距
    $name:zh-TW: 漩渦最小間距
    $name:ja-JP: 最小距離
    $description: Minimum distance between vortex centers (px, 50-500).
    $description:zh-CN: 漩涡中心之间的最小距离（像素，50-500）。
    $description:zh-TW: 漩渦中心之間的最小距離（像素，50-500）。
    $description:ja-JP: ボルテックス中心間の最小距離（px、50-500）。
  - vortex_drift: 30
    $name: Vortex Center Drift
    $name:zh-CN: 漩涡中心漂移
    $name:zh-TW: 漩渦中心漂移
    $name:ja-JP: 中心ドリフト
    $description: How much vortex center follows mouse motion and forces (0-100).
    $description:zh-CN: 漩涡中心受鼠标运动和力影响的漂移程度（0-100）。
    $description:zh-TW: 漩渦中心受滑鼠運動和力影響的漂移程度（0-100）。
    $description:ja-JP: ボルテックス中心がマウス運動と力に追従する度合い（0-100）。
  - vortex_duration_speed: 20
    $name: Duration Speed Boost
    $name:zh-CN: 持续时间速度增益
    $name:zh-TW: 持續時間速度增益
    $name:ja-JP: 速度ブースト
    $description: Extend vortex duration based on mouse speed (0-100).
    $description:zh-CN: 鼠标速度对漩涡持续时间的延长比例（0-100）。
    $description:zh-TW: 滑鼠速度對漩渦持續時間的延長比例（0-100）。
    $description:ja-JP: マウス速度による持続時間延長（0-100）。
  - vortex_phys_model: rankine
    $name: Vortex Physics Model
    $name:zh-CN: 漩涡物理模型
    $name:zh-TW: 漩渦物理模型
    $name:ja-JP: 物理モデル
    $description: Velocity field used by each centripetal vortex, i.e. how tangential speed falls off with distance r. Rankine (default) = solid-body rotation inside the core and 1/r outside it, the classic engineering model. Lamb-Oseen = the same falloff smoothed by a viscous Gaussian core, the most physically realistic. Free = pure 1/r everywhere, fastest near the center. Solid = omega*r like a spinning disc. Kepler = sqrt(GM/r), so particles orbit like planets instead of swirling.
    $description:zh-CN: 每个向心力漩涡使用的速度场模型，即切向速度随半径 r 如何衰减。Rankine（默认）= 涡核内刚体旋转、核外降为 1/r，最经典的工程模型。Lamb-Oseen = 用粘性高斯涡核把同一衰减曲线平滑过渡，物理上最真实。自由涡 = 全程 1/r，越靠近中心越快。刚体旋转 = ω·r，像旋转的圆盘。开普勒轨道 = 速度按 sqrt(GM/r) 衰减，粒子像行星绕行而非原地盘旋。
    $description:zh-TW: 每個向心力漩渦使用的速度場模型，即切線速度隨半徑 r 如何衰減。Rankine（預設）= 渦核內剛體旋轉、核外降為 1/r，最經典的工程模型。Lamb-Oseen = 以粘性高斯渦核將同一衰減曲線平滑過渡，物理上最真實。自由渦 = 全程 1/r，越靠近中心越快。剛體旋轉 = ω·r，像旋轉的圓盤。克卜勒軌道 = 速度按 sqrt(GM/r) 衰減，粒子像行星繞行而非原地盤旋。
    $description:ja-JP: 各求心ボルテックスが使う速度場モデル、すなわち接線速度が半径 r に対してどう減衰するか。Rankine（既定）= 渦核内は剛体回転、外側は 1/r という古典的な工学的モデル。Lamb-Oseen = 粘性ガウス渦核で同じ減衰を滑らかに繋いだ、物理的に最も現実的なモデル。自由渦 = 全区間で 1/r、中心に近いほど高速。剛体回転 = ω·r で円盤のように回転。ケプラー軌道 = 速度は sqrt(GM/r) で減衰し、パーティクルはその場で渦巻くのではなく惑星のように周回します。
    $options:
    - rankine: Rankine Vortex
    - free: Free Vortex
    - solid: Solid Body
    - lamb: Lamb-Oseen
    - kepler: Kepler Orbit
    $options:zh-CN:
    - rankine: Rankine涡
    - free: 自由涡
    - solid: 刚体旋转
    - lamb: Lamb-Oseen涡
    - kepler: 开普勒轨道
    $options:zh-TW:
    - rankine: Rankine渦
    - free: 自由渦
    - solid: 剛體旋轉
    - lamb: Lamb-Oseen渦
    - kepler: 開普勒軌道
    $options:ja-JP:
    - rankine: ランキン渦
    - free: 自由渦
    - solid: 剛体回転
    - lamb: ラムオゼーン渦
    - kepler: ケプラー軌道
  - enable_particle_mass: true
    $name: Particle Mass
    $name:zh-CN: 粒子质量
    $name:zh-TW: 粒子質量
    $name:ja-JP: 粒子質量
    $description: Enable random particle mass (normal distribution). Mass affects inertia, size, lifetime, gravity and repulsion.
    $description:zh-CN: 启用粒子随机质量（正态分布）。质量影响惯性、大小、生命周期、引力和排斥力。
    $description:zh-TW: 啟用粒子隨機質量（常態分佈）。質量影響慣性、大小、生命週期、引力和排斥力。
    $description:ja-JP: ランダム粒子質量を有効化。質量は慣性、サイズ、寿命、重力、反発に影響。
  - particle_mass_min: 5
    $name: Min Mass
    $name:zh-CN: 最小质量
    $name:zh-TW: 最小質量
    $name:ja-JP: 最小質量
    $description: Minimum particle mass (1-50, value/10 = actual mass).
    $description:zh-CN: 粒子最小质量（1-50，实际质量=数值/10）。
    $description:zh-TW: 粒子最小質量（1-50，實際質量=數值/10）。
    $description:ja-JP: 最小粒子質量（1-50、実際の質量=値/10）。
  - particle_mass_max: 50
    $name: Max Mass
    $name:zh-CN: 最大质量
    $name:zh-TW: 最大質量
    $name:ja-JP: 最大質量
    $description: Maximum particle mass (1-100, value/10 = actual mass).
    $description:zh-CN: 粒子最大质量（1-100，实际质量=数值/10）。
    $description:zh-TW: 粒子最大質量（1-100，實際質量=數值/10）。
    $description:ja-JP: 最大粒子質量（1-100、実際の質量=値/10）。
  - enable_particle_gravity: false
    $name: Particle Gravity
    $name:zh-CN: 粒子万有引力
    $name:zh-TW: 粒子萬有引力
    $name:ja-JP: 粒子重力
    $description: Newtonian gravity between particles with Plummer softening.
    $description:zh-CN: 粒子之间的牛顿万有引力，带Plummer软化。
    $description:zh-TW: 粒子之間的牛頓萬有引力，帶Plummer軟化。
    $description:ja-JP: プラマー軟化を伴うニュートン重力。
  - gravity_strength: 50
    $name: Gravity Strength
    $name:zh-CN: 引力强度
    $name:zh-TW: 引力強度
    $name:ja-JP: 重力強度
    $description: Gravitational constant G multiplier (0-100).
    $description:zh-CN: 引力常数G倍率（0-100）。
    $description:zh-TW: 引力常數G倍率（0-100）。
    $description:ja-JP: 重力定数Gの倍率（0-100）。
  - gravity_system: binary
    $name: Gravity System
    $name:zh-CN: 引力系统
    $name:zh-TW: 引力系統
    $name:ja-JP: 重力システム
    $options:
    - binary: Binary Star
    - nbody: N-Body
    $options:zh-CN:
    - binary: 双星系统
    - nbody: N体系统
    $options:zh-TW:
    - binary: 雙星系統
    - nbody: N體系統
    $options:ja-JP:
    - binary: 連星
    - nbody: N体
    $description: Binary = two heaviest as attractors; N-Body = all attract each other (O(n²)).
    $description:zh-CN: 双星=质量最大的两个粒子作为引力源；N体=所有粒子互相吸引。
    $description:zh-TW: 雙星=質量最大的兩個粒子作為引力源；N體=所有粒子互相吸引。
    $description:ja-JP: 連星=最も重い2つを引力源に; N体=全てが互いに引力。
  - gravity_body_count: 3
    $name: N-Body Count
    $name:zh-CN: N体数量
    $name:zh-TW: N體數量
    $name:ja-JP: N体数
    $description: Number of massive attractor bodies in N-Body mode (2-10).
    $description:zh-CN: N体模式中的大质量引力源数量（2-10）。
    $description:zh-TW: N體模式中的大質量引力源數量（2-10）。
    $description:ja-JP: N体モードの大質量引力源の数（2-10）。
  - enable_particle_collision: true
    $name: Particle Collision
    $name:zh-CN: 粒子碰撞
    $name:zh-TW: 粒子碰撞
    $name:ja-JP: 粒子衝突
    $description: Elastic collisions between particles with momentum and energy conservation.
    $description:zh-CN: 粒子之间的弹性碰撞，动量守恒+动能守恒。
    $description:zh-TW: 粒子之間的彈性碰撞，動量守恆+動能守恆。
    $description:ja-JP: 運動量とエネルギー保存を伴う弾性衝突。
  - enable_lorentz_force: false
    $name: Lorentz Force
    $name:zh-CN: 洛伦兹力
    $name:zh-TW: 洛倫茲力
    $name:ja-JP: ローレンツ力
    $description: Particles carry random charge. Magnetic field causes circular motion per F=q(v×B).
    $description:zh-CN: 粒子带随机电荷。磁场使粒子做圆周运动：F=q(v×B)。
    $description:zh-TW: 粒子帶隨機電荷。磁場使粒子做圓周運動：F=q(v×B)。
    $description:ja-JP: 粒子がランダムな電荷を帯び、磁場で円運動します。
  - lorentz_strength: 30
    $name: Magnetic Field
    $name:zh-CN: 磁场强度
    $name:zh-TW: 磁場強度
    $name:ja-JP: 磁場強度
    $description: Magnetic field B strength for Lorentz force (0-100).
    $description:zh-CN: 洛伦兹力的磁场B强度（0-100）。
    $description:zh-TW: 洛倫茲力的磁場B強度（0-100）。
    $description:ja-JP: ローレンツ力の磁場B強度（0-100）。
  - enable_brownian_motion: true
    $name: Brownian Motion
    $name:zh-CN: 布朗运动
    $name:zh-TW: 布朗運動
    $name:ja-JP: ブラウン運動
    $description: Random thermal noise force on particles, simulating molecular heat motion.
    $description:zh-CN: 粒子受随机热噪声力，模拟分子热运动。
    $description:zh-TW: 粒子受隨機熱噪聲力，模擬分子熱運動。
    $description:ja-JP: 分子の熱運動を模擬するランダムな熱ノイズ力。
  - brownian_strength: 50
    $name: Thermal Noise
    $name:zh-CN: 热噪声强度
    $name:zh-TW: 熱噪聲強度
    $name:ja-JP: 熱ノイズ強度
    $description: Brownian motion force magnitude (0-100).
    $description:zh-CN: 布朗运动力的大小（0-100）。
    $description:zh-TW: 布朗運動力的大小（0-100）。
    $description:ja-JP: ブラウン運動力の大きさ（0-100）。
  - enable_env_gravity: true
    $name: Environmental Gravity
    $name:zh-CN: 环境重力
    $name:zh-TW: 環境重力
    $name:ja-JP: 環境重力
    $description: Constant global acceleration field like gravity, mass-independent.
    $description:zh-CN: 恒定全局加速度场，模拟重力，与质量无关。
    $description:zh-TW: 恆定全域加速度場，模擬重力，與質量無關。
    $description:ja-JP: 重力のような一定の全地球加速度場。質量非依存。
  - env_gravity_strength: 25
    $name: Gravity Strength
    $name:zh-CN: 重力强度
    $name:zh-TW: 重力強度
    $name:ja-JP: 重力強度
    $description: Gravity acceleration strength (0-100).
    $description:zh-CN: 重力加速度强度（0-100）。
    $description:zh-TW: 重力加速度強度（0-100）。
    $description:ja-JP: 重力加速度の強さ（0-100）。
  - env_gravity_angle: 145
    $name: Gravity Direction
    $name:zh-CN: 重力方向
    $name:zh-TW: 重力方向
    $name:ja-JP: 重力方向
    $description: Gravity direction in degrees. 0=right, 90=down, 180=left, -90=up.
    $description:zh-CN: 重力方向角度。0=向右，90=向下，180=向左，-90=向上。
    $description:zh-TW: 重力方向角度。0=向右，90=向下，180=向左，-90=向上。
    $description:ja-JP: 重力方向（度）。0=右、90=下、180=左、-90=上。
  - enable_env_wind: true
    $name: Wind Field
    $name:zh-CN: 风场
    $name:zh-TW: 風場
    $name:ja-JP: 風場
    $description: Global wind pushes particles. Light particles blow further.
    $description:zh-CN: 全局风推动粒子，轻粒子被吹得更远。
    $description:zh-TW: 全域風推動粒子，輕粒子被吹得更遠。
    $description:ja-JP: 全体の風がパーティクルを押します。軽い粒子ほど遠くへ。
  - wind_strength: 40
    $name: Wind Strength
    $name:zh-CN: 风力强度
    $name:zh-TW: 風力強度
    $name:ja-JP: 風力強度
    $description: Base wind force (0-100).
    $description:zh-CN: 基础风力（0-100）。
    $description:zh-TW: 基礎風力（0-100）。
    $description:ja-JP: 基本風力（0-100）。
  - wind_turbulence: 60
    $name: Wind Turbulence
    $name:zh-CN: 风场湍流
    $name:zh-TW: 風場湍流
    $name:ja-JP: 乱流
    $description: How much wind direction and speed fluctuate (0-100).
    $description:zh-CN: 风向和风速的波动程度（0-100）。
    $description:zh-TW: 風向和風速的波動程度（0-100）。
    $description:ja-JP: 風向と風速の変動度合い（0-100）。
  - enable_particle_spring: false
    $name: Cloth Spring Net
    $name:zh-CN: 布料弹簧网
    $name:zh-TW: 布料彈簧網
    $name:ja-JP: クロスバネ
    $description: Soft springs between nearby particles. The cluster wobbles like cloth.
    $description:zh-CN: 邻近粒子之间用软弹簧连接，快速甩鼠标时整团粒子像布料一样抖动。
    $description:zh-TW: 鄰近粒子之間用軟彈簧連接，快速甩滑鼠時整團粒子像布料一樣抖動。
    $description:ja-JP: 近隣粒子間の柔らかいバネ。クラスタが布のように揺れます。
  - spring_strength: 40
    $name: Spring Stiffness
    $name:zh-CN: 弹簧刚度
    $name:zh-TW: 彈簧剛度
    $name:ja-JP: バネ剛性
    $description: How strongly springs pull/push particles (0-100).
    $description:zh-CN: 弹簧拉/推粒子的力度（0-100）。
    $description:zh-TW: 彈簧拉/推粒子的力度（0-100）。
    $description:ja-JP: バネが粒子を引く/押す強さ（0-100）。
  - spring_distance: 50
    $name: Spring Link Distance
    $name:zh-CN: 弹簧连接距离
    $name:zh-TW: 彈簧連接距離
    $name:ja-JP: 接続距離
    $description: Maximum distance at which nearby particles connect with a spring.
    $description:zh-CN: 邻近粒子建立弹簧连接的最大距离（像素）。
    $description:zh-TW: 鄰近粒子建立彈簧連接的最大距離（像素）。
    $description:ja-JP: 近隣粒子がバネで接続する最大距離（px）。
  $name: Physics & Forces
  $name:zh-CN: 物理与力场
  $name:zh-TW: 物理與力場
  $name:ja-JP: 物理と力場

- Music:
  - enable_multi_band_link: true
    $name: Multi-Band Link
    $name:zh-CN: 多频段联动
    $name:zh-TW: 多頻段聯動
    $name:ja-JP: マルチバンド連動
    $description: Link audio frequency bands to physics. Bass→Gravity, Mid→Magnetic Field, Treble→Thermal Noise.
    $description:zh-CN: 将音频频段独立联动到物理：低频→引力，中频→磁场，高频→热噪声。
    $description:zh-TW: 將音訊頻段獨立聯動到物理：低頻→引力，中頻→磁場，高頻→熱噪聲。
    $description:ja-JP: オーディオ帯域を物理に連動：低音→重力、中音→磁場、高音→熱ノイズ。
  - enable_music_reactive: false
    $name: Music Reactive
    $name:zh-CN: 音乐响应
    $name:zh-TW: 音樂回應
    $name:ja-JP: 音楽連動
    $description: Enable WASAPI loopback audio capture, FFT analysis, beat detection.
    $description:zh-CN: 启用WASAPI回环音频捕获、FFT频率分析、节拍检测。
    $description:zh-TW: 啟用WASAPI迴環音訊擷取、FFT頻率分析、節拍偵測。
    $description:ja-JP: WASAPIループバック、FFT分析、ビート検出を有効化。
  - music_fft_size: fft512
    $name: FFT Size
    $name:zh-CN: FFT大小
    $name:zh-TW: FFT大小
    $name:ja-JP: FFTサイズ
    $options:
    - fft256: "256"
    - fft512: "512"
    - fft1024: "1024"
    - fft2048: "2048"
    $options:zh-CN:
    - fft256: "256点"
    - fft512: "512点"
    - fft1024: "1024点"
    - fft2048: "2048点"
    $options:zh-TW:
    - fft256: "256點"
    - fft512: "512點"
    - fft1024: "1024點"
    - fft2048: "2048點"
    $options:ja-JP:
    - fft256: "256"
    - fft512: "512"
    - fft1024: "1024"
    - fft2048: "2048"
    $description: FFT window size. Larger = better frequency resolution, higher latency.
    $description:zh-CN: FFT窗口大小。越大频率分辨率越好，延迟越高。
    $description:zh-TW: FFT視窗大小。越大頻率解析度越好，延遲越高。
    $description:ja-JP: FFTウィンドウサイズ。大きいほど周波数分解能が向上、遅延増加。
  - music_sensitivity: 50
    $name: Beat Sensitivity
    $name:zh-CN: 节拍灵敏度
    $name:zh-TW: 節拍靈敏度
    $name:ja-JP: ビート感度
    $description: Beat detection sensitivity (0-100). Lower = more sensitive.
    $description:zh-CN: 节拍检测灵敏度（0-100）。越低越灵敏。
    $description:zh-TW: 節拍偵測靈敏度（0-100）。越低越靈敏。
    $description:ja-JP: ビート検出感度（0-100）。低いほど高感度。
  - music_smoothing: 50
    $name: Frequency Smoothing
    $name:zh-CN: 频率平滑
    $name:zh-TW: 頻率平滑
    $name:ja-JP: 周波数スムージング
    $description: Frequency band smoothing factor (0-100). Reduces jitter.
    $description:zh-CN: 频段平滑系数（0-100）。减少抖动。
    $description:zh-TW: 頻段平滑係數（0-100）。減少抖動。
    $description:ja-JP: 周波数帯域の平滑化係数（0-100）。ジッタを低減。
  - enable_music_physics: false
    $name: Music Physics Link
    $name:zh-CN: 音乐物理联动
    $name:zh-TW: 音樂物理聯動
    $name:ja-JP: 音楽物理連動
    $description: Link music analysis to particle physics with beat detection and pulses.
    $description:zh-CN: 将音乐分析联动到粒子物理，节拍检测和脉冲效果。
    $description:zh-TW: 將音樂分析聯動到粒子物理，節拍偵測和脈衝效果。
    $description:ja-JP: 音楽分析をビート検出とパルスによってパーティクル物理に連動させます。
  - music_gravity_link: 15
    $name: Volume→Gravity
    $name:zh-CN: 音量→引力
    $name:zh-TW: 音量→引力
    $name:ja-JP: 音量→重力
    $description: Audio volume boosts particle gravity strength (0-100).
    $description:zh-CN: 总音量增强粒子引力强度（0-100）。
    $description:zh-TW: 總音量增強粒子引力強度（0-100）。
    $description:ja-JP: 音量が粒子重力強度を増幅（0-100）。
  - music_beat_pulse: 5
    $name: Beat→Velocity
    $name:zh-CN: 节拍→速度
    $name:zh-TW: 節拍→速度
    $name:ja-JP: ビート→速度
    $description: Beat triggers particle velocity burst (0-100).
    $description:zh-CN: 节拍触发粒子速度爆发（0-100）。
    $description:zh-TW: 節拍觸發粒子速度爆發（0-100）。
    $description:ja-JP: ビートが粒子速度バーストを発生（0-100）。
  - music_bass_size: 60
    $name: Bass→Size
    $name:zh-CN: 低频→大小
    $name:zh-TW: 低頻→大小
    $name:ja-JP: 低音→サイズ
    $description: Bass energy increases particle size (0-100).
    $description:zh-CN: 低频能量增大粒子大小（0-100）。
    $description:zh-TW: 低頻能量增大粒子大小（0-100）。
    $description:ja-JP: 低音エネルギーが粒子サイズを増加（0-100）。
  - music_vortex_link: 20
    $name: Beat→Vortex
    $name:zh-CN: 节拍→漩涡
    $name:zh-TW: 節拍→漩渦
    $name:ja-JP: ビート→ボルテックス
    $description: Beat energy injects into vortex strength (0-100).
    $description:zh-CN: 节拍能量注入漩涡强度（0-100）。
    $description:zh-TW: 節拍能量注入漩渦強度（0-100）。
    $description:ja-JP: ビートエネルギーをボルテックス強度に注入（0-100）。
  $name: Music Reactive
  $name:zh-CN: 音乐联动
  $name:zh-TW: 音樂連動
  $name:ja-JP: 音楽連動

- Click:
  - enable_click_starburst: false
    $name: Click Starburst
    $name:zh-CN: 点击星爆
    $name:zh-TW: 點擊星爆
    $name:ja-JP: クリックスターバースト
    $description: Particle burst from cursor on mouse click.
    $description:zh-CN: 点击时从光标位置迸发粒子。
    $description:zh-TW: 點擊時從游標位置迸發粒子。
    $description:ja-JP: クリック時にカーソル位置からパーティクルが噴出。
  - starburst_count: 8
    $name: Starburst Count
    $name:zh-CN: 星爆粒子数
    $name:zh-TW: 星爆粒子數
    $name:ja-JP: 星爆数
    $description: Number of particles per click burst (4-20).
    $description:zh-CN: 每次点击迸发的粒子数量（4-20）。
    $description:zh-TW: 每次點擊迸發的粒子數量（4-20）。
    $description:ja-JP: クリックごとのパーティクル数（4-20）。
  - enable_click_effect: true
    $name: Click Ripple
    $name:zh-CN: 点击波纹
    $name:zh-TW: 點擊波紋
    $name:ja-JP: クリック波紋
    $description: Expanding ripple ring on mouse click.
    $description:zh-CN: 点击时产生扩散波纹环。
    $description:zh-TW: 點擊時產生擴散波紋環。
    $description:ja-JP: クリック時に波紋リングが拡大。
  - click_style: ripple
    $name: Click Effect Style
    $name:zh-CN: 点击效果样式
    $name:zh-TW: 點擊效果樣式
    $name:ja-JP: クリック効果スタイル
    $options:
    - ripple: Ripple
    - shockwave: Shockwave
    - ink: Ink Bloom
    - spark: Spark Star
    - bloom: Petal Bloom
    $options:zh-CN:
    - ripple: 波纹
    - shockwave: 冲击波
    - ink: 墨晕
    - spark: 星芒
    - bloom: 花瓣绽放
    $options:zh-TW:
    - ripple: 波紋
    - shockwave: 衝擊波
    - ink: 墨暈
    - spark: 星芒
    - bloom: 花瓣綻放
    $options:ja-JP:
    - ripple: 波紋
    - shockwave: 衝撃波
    - ink: 墨のにじみ
    - spark: 星芒
    - bloom: 花びら
    $description: Shape of the click feedback. Ripple is a clean expanding ring. Shockwave adds an
      opening flash, a fast thick ring and radial sparks. Ink Bloom spreads an irregular ink blot with
      splatter. Spark Star fires four main beams plus eight short ones. Petal Bloom opens six rotating
      petals. All styles reuse Max Radius and Duration below.
    $description:zh-CN: 点击反馈的形状。波纹是干净的扩散圆环；冲击波带开场闪光、快速粗环与径向火花；
      墨晕扩散出不规则墨迹并带溅墨；星芒射出四道主芒与八道副芒；花瓣绽放展开六枚旋转花瓣。
      所有样式共用下方的最大半径与持续时间。
    $description:zh-TW: 點擊回饋的形狀。波紋是乾淨的擴散圓環；衝擊波帶開場閃光、快速粗環與徑向火花；
      墨暈擴散出不規則墨跡並帶濺墨；星芒射出四道主芒與八道副芒；花瓣綻放展開六枚旋轉花瓣。
      所有樣式共用下方的最大半徑與持續時間。
    $description:ja-JP: クリック反馈のかたち。波紋はきれいな広がる円環、衝撃波は開始フラッシュと
      速い太リング＋放射火花、墨のにじみは不規則な墨だまりと飛び散り、星芒は4本の主光条と8本の副光条、
      花びらは6枚の回転する花びらを開きます。どのスタイルも下の最大半径と持続時間を共用します。
  - click_max_radius: 40
    $name: Ripple Max Radius
    $name:zh-CN: 波纹最大半径
    $name:zh-TW: 波紋最大半徑
    $name:ja-JP: 最大半径
    $description: Maximum ripple radius (pixels, 1-500).
    $description:zh-CN: 点击波纹扩散的最大半径（像素，1-500）。
    $description:zh-TW: 點擊波紋擴散的最大半徑（像素，1-500）。
    $description:ja-JP: 波紋の最大半径（ピクセル、1-500）。
  - click_duration: 300
    $name: Ripple Duration
    $name:zh-CN: 波纹持续时间
    $name:zh-TW: 波紋持續時間
    $name:ja-JP: 持続時間
    $description: Ripple duration (ms, 1-3000).
    $description:zh-CN: 点击波纹从出现到消失的时长（毫秒，1-3000）。
    $description:zh-TW: 點擊波紋從出現到消失的時長（毫秒，1-3000）。
    $description:ja-JP: 波紋の持続時間（ms、1-3000）。
  $name: Click Effects
  $name:zh-CN: 点击效果
  $name:zh-TW: 點擊效果
  $name:ja-JP: クリック効果

- AnimFX:
  - fx_bloom_pulse: false
    $name: Bloom Breathing
    $name:zh-CN: 泛光呼吸
    $name:zh-TW: 泛光呼吸
    $name:ja-JP: ブルーム呼吸
    $description: Modulates Bloom Intensity above. The base value still sets the average;
      this only makes it rise and fall.
    $description:zh-CN: 调制上方的「泛光强度」。基础值决定平均值，本项只让它在平均值上下起伏。
    $description:zh-TW: 調制上方的「泛光強度」。基礎值決定平均值，本項只讓它在平均值上下起伏。
    $description:ja-JP: 上の「ブルーム強度」を変調します。基本値が平均値となり、本項目はその上下動だけを担当します。
  - fx_glow_breath: false
    $name: Glow Breathing
    $name:zh-CN: 外发光呼吸
    $name:zh-TW: 外發光呼吸
    $name:ja-JP: グロー呼吸
    $description: Modulates Glow Intensity above (requires Glow to be enabled).
    $description:zh-CN: 调制上方的「发光强度」（需先开启发光）。
    $description:zh-TW: 調制上方的「發光強度」（需先開啟發光）。
    $description:ja-JP: 上の「グロー強度」を変調します（グローの有効化が必要）。
  - fx_edge_flow: false
    $name: Edge Softness Flow
    $name:zh-CN: 边缘柔和流动
    $name:zh-TW: 邊緣柔和流動
    $name:ja-JP: エッジの柔らかさ
    $description: Modulates Edge Softness above.
    $description:zh-CN: 调制上方的「边缘柔和度」。
    $description:zh-TW: 調制上方的「邊緣柔和度」。
    $description:ja-JP: 上の「エッジの柔らかさ」を変調します。
  - fx_dither_shimmer: false
    $name: Dither Shimmer
    $name:zh-CN: 颗粒抖动闪烁
    $name:zh-TW: 顆粒抖動閃爍
    $name:ja-JP: ディザのきらめき
    $description: Modulates Dither Strength above. Works best with the Flicker wave.
    $description:zh-CN: 调制上方的「抖动强度」。搭配「闪烁」波形效果最好。
    $description:zh-TW: 調制上方的「抖動強度」。搭配「閃爍」波形效果最好。
    $description:ja-JP: 上の「ディザ強度」を変調します。「ちらつき」波形との併用が最適です。
  - fx_anim_speed: 100
    $name: Animation Speed
    $name:zh-CN: 动画速度
    $name:zh-TW: 動畫速度
    $name:ja-JP: アニメ速度
    $description: Speed of every animation in this group (10-400, 100 is about half a cycle per second).
    $description:zh-CN: 本组所有动画的速度（10-400，100 约为每秒半个周期）。
    $description:zh-TW: 本組所有動畫的速度（10-400，100 約為每秒半個週期）。
    $description:ja-JP: このグループの全アニメの速度（10-400、100 は約 0.5Hz）。
  - fx_anim_amount: 50
    $name: Animation Amount
    $name:zh-CN: 动画幅度
    $name:zh-TW: 動畫幅度
    $name:ja-JP: アニメ幅度
    $description: How far the animations in this group swing (0-100). Set 0 and they all stay static.
    $description:zh-CN: 本组动画的摆动幅度（0-100）。设为 0 则全部保持静态。
    $description:zh-TW: 本組動畫的擺動幅度（0-100）。設為 0 則全部保持靜態。
    $description:ja-JP: このグループのアニメの振れ幅（0-100）。0 にするとすべて静止します。
  - fx_stroke_flow: false
    $name: Stroke Width Flow
    $name:zh-CN: 描边宽度流动
    $name:zh-TW: 描邊寬度流動
    $name:ja-JP: ストローク幅の流れ
    $description: The trail outline width breathes, which pairs well with Glow Breathing.
    $description:zh-CN: 拖尾描边宽度随时间呼吸，与外发光呼吸搭配观感最好。
    $description:zh-TW: 拖尾描邊寬度隨時間呼吸，與外發光呼吸搭配觀感最好。
    $description:ja-JP: トレイルの輪郭幅が呼吸します。グロー呼吸と組み合わせると効果的です。
  - fx_bloom_thr_wave: false
    $name: Bloom Threshold Wave
    $name:zh-CN: 泛光阈值波动
    $name:zh-TW: 泛光閾值波動
    $name:ja-JP: ブルーム閾値の波
    $description: The bloom cutoff rises and falls, so bright areas swell and dim instead of pulsing evenly.
    $description:zh-CN: 泛光阈值上下起伏，明亮区域会忽胀忽收，而不是整体均匀明暗。
    $description:zh-TW: 泛光閾值上下起伏，明亮區域會忽脹忽收，而不是整體均勻明暗。
    $description:ja-JP: ブルームの閾値が上下し、明るい部分が膨らんだり縮んだりします。
  - fx_blur_pulse: false
    $name: Temporal Blur Pulse
    $name:zh-CN: 时域模糊脉动
    $name:zh-TW: 時域模糊脈動
    $name:ja-JP: 時間ぼかしの脈動
    $description: How much of the previous frame is kept pulses, so ghosting thickens and thins.
    $description:zh-CN: 上一帧的保留比例随时间起伏，拖影会时浓时淡。
    $description:zh-TW: 上一幀的保留比例隨時間起伏，拖影會時濃時淡。
    $description:ja-JP: 前フレームの保持率が変動し、残像が濃くなったり薄くなったりします。
  - fx_hdr_shimmer: false
    $name: HDR Highlight Shimmer
    $name:zh-CN: HDR 高光闪烁
    $name:zh-TW: HDR 高光閃爍
    $name:ja-JP: HDR ハイライトのきらめき
    $description: The highlight boost shimmers, so the brightest parts sparkle.
    $description:zh-CN: 高光增益随时间闪烁，最亮的部分会闪动。
    $description:zh-TW: 高光增益隨時間閃爍，最亮的部分會閃動。
    $description:ja-JP: ハイライトのゲインがきらめき、最も明るい部分がまたたきます。
  - fx_anim_wave: sine
    $name: Animation Wave
    $name:zh-CN: 动画波形
    $name:zh-TW: 動畫波形
    $name:ja-JP: アニメ波形
    $options:
    - sine: Sine
    - pulse: Pulse
    - triangle: Triangle
    - flicker: Flicker
    $options:zh-CN:
    - sine: 正弦
    - pulse: 脉冲
    - triangle: 三角
    - flicker: 闪烁
    $options:zh-TW:
    - sine: 正弦
    - pulse: 脈衝
    - triangle: 三角
    - flicker: 閃爍
    $options:ja-JP:
    - sine: サイン
    - pulse: パルス
    - triangle: 三角
    - flicker: ちらつき
    $description: Shape of the animation curve for everything in this group. Sine is the smoothest,
      pulse has a fast attack and slow decay, triangle moves at a constant rate, flicker jumps
      randomly (best for Dither Shimmer).
    $description:zh-CN: 本组所有动画的曲线形状。正弦最平滑，脉冲快起慢落，三角匀速往返，
      闪烁为随机跳变（最适合颗粒抖动闪烁）。
    $description:zh-TW: 本組所有動畫的曲線形狀。正弦最平滑，脈衝快起慢落，三角勻速往返，
      閃爍為隨機跳變（最適合顆粒抖動閃爍）。
    $description:ja-JP: このグループの全アニメの曲線形状。サインが最も滑らか、パルスは立ち上がりが速く、
      三角は等速、ちらつきはランダムに跳ねます（ディザに最適）。
  $name: Shader Animations
  $name:zh-CN: 着色器动画
  $name:zh-TW: 著色器動畫
  $name:ja-JP: シェーダーアニメーション
- System:
  - super_performance_mode: false
    $name: Super Performance Mode
    $name:zh-CN: 超级性能模式
    $name:zh-TW: 超級效能模式
    $name:ja-JP: スーパーパフォーマンス
    $description: Lifts the caps and turns on a faster physics path, for high-end PCs only. Caps raised
      are particles 500 to 1200 and trail shapes 150 to 400, and the polygon-to-circle render fallback
      is delayed from 100 to 300 particles so detailed shapes survive longer. It also enables a uniform
      grid that cuts the O(n²) pair loop to O(n·k), worth about 1.67ms to 0.3ms per frame at 1200
      particles. No quality setting is weakened - text and emoji particles still take the full path.
    $description:zh-CN: 解除上限并启用更快的物理路径，仅供高性能电脑。上限放宽为：粒子 500→1200、拖尾形状 150→400；
      多边形降级为圆形的渲染回退阈值由 100 推迟到 300，细节形状能保持更久。同时启用均匀网格，
      把 O(n²) 配对循环降到 O(n·k)，1200 粒子时每帧约 1.67ms 降到 0.3ms 量级。不削弱任何画质参数，
      文字与 Emoji 粒子仍走完整路径。
    $description:zh-TW: 解除上限並啟用更快的物理路徑，僅供高效能電腦。上限放寬為：粒子 500→1200、拖尾形狀 150→400；
      多邊形降級為圓形的渲染回退閾值由 100 推遲到 300，細節形狀能維持更久。同時啟用均勻網格，
      把 O(n²) 配對迴圈降到 O(n·k)，1200 粒子時每幀約 1.67ms 降到 0.3ms 量級。不削弱任何畫質參數，
      文字與 Emoji 粒子仍走完整路徑。
    $description:ja-JP: 上限を解除し、より高速な物理経路を有効化します。ハイエンドPC専用。上限は
      パーティクル 500→1200、トレイル形状 150→400 に緩和され、多角形から円への描画フォールバックも
      100→300 個に繰り下がるため、細かい形状がより長く保たれます。さらに均一グリッドで O(n²) の
      ペア探索を O(n·k) に短縮し、1200 個でも 1フレーム約 1.67ms が 0.3ms 程度になります。
      画質パラメータは一切弱めません。文字・絵文字パーティクルは引き続き完全な経路を使います。
  - dpi_scale_mode: auto
    $name: DPI Awareness
    $name:zh-CN: DPI 感知
    $name:zh-TW: DPI 感知
    $name:ja-JP: DPI対応
    $description: Auto detects the DPI of the monitor the cursor is on and keeps the overlay, offscreen bitmaps and swap chain consistent across mixed-DPI setups. Fixed assumes 100% (legacy behaviour).
    $description:zh-CN: 自动检测光标所在显示器的 DPI，使覆盖层、离屏位图与交换链在混合 DPI 环境下保持一致。固定=按 100% 处理（旧行为）。
    $description:zh-TW: 自動偵測游標所在顯示器的 DPI，使覆蓋層、離屏位圖與交換鏈在混合 DPI 環境下保持一致。固定=按 100% 處理（舊行為）。
    $description:ja-JP: カーソルがあるモニターのDPIを自動検出し、オーバーレイ・オフスクリーンビットマップ・スワップチェーンを整合させます。固定=100%扱い（従来動作）。
    $options:
    - auto: Auto
    - fixed: Fixed (100%)
    $options:zh-CN:
    - auto: 自动
    - fixed: 固定（100%）
    $options:zh-TW:
    - auto: 自動
    - fixed: 固定（100%）
    $options:ja-JP:
    - auto: 自動
    - fixed: 固定（100%）
  - dpi_scale_text: true
    $name: DPI Scale Text
    $name:zh-CN: 文字随 DPI 缩放
    $name:zh-TW: 文字隨 DPI 縮放
    $name:ja-JP: 文字のDPIスケーリング
    $description: Scale text/emoji atlas size by the monitor DPI so text keeps the same physical size when it moves between a 100% and a 150%/200% monitor. Off keeps the pixel size constant.
    $description:zh-CN: 按显示器 DPI 缩放文字/Emoji 图集尺寸，使文字在 100% 与 150%/200% 显示器之间移动时保持相同的物理大小。关闭则保持像素尺寸不变。
    $description:zh-TW: 按顯示器 DPI 縮放文字/Emoji 圖集尺寸，使文字在 100% 與 150%/200% 顯示器之間移動時保持相同的物理大小。關閉則保持像素尺寸不變。
    $description:ja-JP: モニターDPIに応じて文字/絵文字アトラスを拡縮し、100%と150%/200%間で物理サイズを保ちます。オフ=ピクセルサイズ固定。
  $name: System & Performance
  $name:zh-CN: 系统与性能
  $name:zh-TW: 系統與效能
  $name:ja-JP: システムと性能

- Hotkeys:
  - hotkey_modifier: alt_shift
    $name: Hotkey Modifier Combo
    $name:zh-CN: 热键组合键
    $name:zh-TW: 熱鍵組合鍵
    $name:ja-JP: ホットキー修飾キー
    $description: Modifier combo that must be held together with the key below. None means no modifier is required, so the key fires on its own. Exactly this combo must be held - the other modifiers must be released. Leave the key fields empty to disable a hotkey.
    $description:zh-CN: 必须与下方按键同时按住的组合键。选择「无」表示不需要组合键，单按该键即可触发。必须恰好按住该组合——其它修饰键需处于松开状态。把按键留空即可关闭对应热键。
    $description:zh-TW: 必須與下方按鍵同時按住的組合鍵。選擇「無」表示不需要組合鍵，單按該鍵即可觸發。必須恰好按住該組合——其它修飾鍵需處於鬆開狀態。把按鍵留空即可關閉對應熱鍵。
    $description:ja-JP: 下のキーと同時に押す修飾キーの組み合わせ。「なし」は修飾キー不要（単独キーで発動）。ちょうどこの組み合わせのときだけ反応し、他の修飾キーは離しておく必要があります。キー欄を空にするとそのホットキーは無効。
    $options:
    - none: None (Bare key)
    - alt: Alt
    - alt_ctrl: Alt + Ctrl
    - alt_shift: Alt + Shift
    - alt_ctrl_shift: Alt + Ctrl + Shift
    $options:zh-CN:
    - none: 无（不需要组合键，单键触发）
    - alt: Alt
    - alt_ctrl: Alt + Ctrl
    - alt_shift: Alt + Shift
    - alt_ctrl_shift: Alt + Ctrl + Shift
    $options:zh-TW:
    - none: 無（不需要組合鍵，單鍵觸發）
    - alt: Alt
    - alt_ctrl: Alt + Ctrl
    - alt_shift: Alt + Shift
    - alt_ctrl_shift: Alt + Ctrl + Shift
    $options:ja-JP:
    - none: なし（修飾キー不要・単独キー）
    - alt: Alt
    - alt_ctrl: Alt + Ctrl
    - alt_shift: Alt + Shift
    - alt_ctrl_shift: Alt + Ctrl + Shift
  - hotkey_toggle_key: ""
    $name: Show / Hide Hotkey Key
    $name:zh-CN: 显示/消失热键按键
    $name:zh-TW: 顯示/消失熱鍵按鍵
    $name:ja-JP: 表示/非表示キー
    $description: "Key pressed together with the combo above to show/hide the whole effect. Accepts a single character (A-Z, 0-9), F1-F24, or a name: space enter tab esc up down left right home end pgup pgdn ins del media_play media_next media_prev media_stop vol_up vol_down vol_mute browser_back browser_forward. Fn combinations work too - laptops emit those as media keys, so bind them by name. Empty = off."
    $description:zh-CN: 与上方组合键同时按下以显示/隐藏整个特效的按键。可填：单个字符（A-Z、0-9）、F1-F24，或名称：space enter tab esc up down left right home end pgup pgdn ins del media_play media_next media_prev media_stop vol_up vol_down vol_mute browser_back browser_forward。Fn 组合键同样可用——笔记本会把 Fn 组合上报为多媒体键，用上面的名称绑定即可。留空=关闭。
    $description:zh-TW: 與上方組合鍵同時按下以顯示/隱藏整個特效的按鍵。可填：單一字元（A-Z、0-9）、F1-F24，或名稱：space enter tab esc up down left right home end pgup pgdn ins del media_play media_next media_prev media_stop vol_up vol_down vol_mute browser_back browser_forward。Fn 組合鍵同樣可用——筆電會把 Fn 組合上報為多媒體鍵，用上面的名稱綁定即可。留空=關閉。
    $description:ja-JP: 上の修飾キーと同時押しでエフェクト全体を表示/非表示にするキー。1文字(A-Z,0-9)、F1-F24、または名前(space enter tab esc up down left right home end pgup pgdn ins del media_play media_next media_prev media_stop vol_up vol_down vol_mute)を指定。Fn組み合わせも可能（メディアキーとして報告されます）。空=無効。
  - hotkey_trail_key: ""
    $name: Hide Trail Hotkey Key
    $name:zh-CN: 隐藏拖尾热键按键
    $name:zh-TW: 隱藏拖尾熱鍵按鍵
    $name:ja-JP: トレイル非表示キー
    $description: Key pressed with the same combo to toggle the trail ribbon on/off. Same key names as above. Empty = off.
    $description:zh-CN: 与同一组合键同时按下以开关拖尾飘带。按键名称同上。留空=关闭。
    $description:zh-TW: 與同一組合鍵同時按下以開關拖尾飄帶。按鍵名稱同上。留空=關閉。
    $description:ja-JP: 同じ修飾キーと同時押しでトレイルを切り替えます。キー名は上と同じ。空=無効。
  - hotkey_particles_key: ""
    $name: Hide Particles Hotkey Key
    $name:zh-CN: 隐藏粒子热键按键
    $name:zh-TW: 隱藏粒子熱鍵按鍵
    $name:ja-JP: パーティクル非表示キー
    $description: Key pressed with the same combo to toggle particles on/off. Same key names as above. Empty = off.
    $description:zh-CN: 与同一组合键同时按下以开关粒子。按键名称同上。留空=关闭。
    $description:zh-TW: 與同一組合鍵同時按下以開關粒子。按鍵名稱同上。留空=關閉。
    $description:ja-JP: 同じ修飾キーと同時押しでパーティクルを切り替えます。キー名は上と同じ。空=無効。
  $name: Hotkeys
  $name:zh-CN: 热键
  $name:zh-TW: 熱鍵
  $name:ja-JP: ホットキー

- Debug:
  - enable_debug_velocity: false
    $name: Debug Velocity Vectors
    $name:zh-CN: 调试-速度向量
    $name:zh-TW: 除錯-速度向量
    $name:ja-JP: デバッグ速度ベクトル
    $description: Draw green velocity arrows on each particle.
    $description:zh-CN: 在每个粒子上绘制绿色速度箭头。
    $description:zh-TW: 在每個粒子上繪製綠色速度箭頭。
    $description:ja-JP: 各パーティクルに緑の速度矢印を描画。
  - enable_debug_force: false
    $name: Debug Force Vectors
    $name:zh-CN: 调试-受力向量
    $name:zh-TW: 除錯-受力向量
    $name:ja-JP: デバッグ力ベクトル
    $description: Draw red force vectors on each particle.
    $description:zh-CN: 在每个粒子上绘制红色受力向量。
    $description:zh-TW: 在每個粒子上繪製紅色受力向量。
    $description:ja-JP: 各パーティクルに赤の力ベクトルを描画。
  - enable_debug_vortex: false
    $name: Debug Vortex Center
    $name:zh-CN: 调试-漩涡中心
    $name:zh-TW: 除錯-漩渦中心
    $name:ja-JP: デバッグ渦中心
    $description: Draw blue vortex center, orbit rings and strength indicator.
    $description:zh-CN: 绘制蓝色漩涡中心、轨道环和强度指示。
    $description:zh-TW: 繪製藍色漩渦中心、軌道環和強度指示。
    $description:ja-JP: 青いボルテックス中心、軌道リング、強度インジケータを描画。
  - enable_debug_gravity: false
    $name: Debug Gravity Sources
    $name:zh-CN: 调试-引力源
    $name:zh-TW: 除錯-引力源
    $name:ja-JP: デバッグ重力源
    $description: Draw yellow gravity source markers and influence radius.
    $description:zh-CN: 绘制黄色引力源标记和影响范围。
    $description:zh-TW: 繪製黃色引力源標記和影響範圍。
    $description:ja-JP: 黄色い重力源マーカーと影響半径を描画。
  - enable_debug_collision: false
    $name: Debug Collision Radius
    $name:zh-CN: 调试-碰撞半径
    $name:zh-TW: 除錯-碰撞半徑
    $name:ja-JP: デバッグ衝突半径
    $description: Draw collision radius on each particle.
    $description:zh-CN: 绘制每个粒子的碰撞半径。
    $description:zh-TW: 繪製每個粒子的碰撞半徑。
    $description:ja-JP: 各パーティクルに衝突半径を描画。
  - enable_debug_spring: false
    $name: Debug Spring Links
    $name:zh-CN: 调试-弹簧连线
    $name:zh-TW: 除錯-彈簧連線
    $name:ja-JP: デバッグバネ線
    $description: Draw cloth spring connections between particles.
    $description:zh-CN: 绘制粒子间的布料弹簧连接。
    $description:zh-TW: 繪製粒子間的布料彈簧連接。
    $description:ja-JP: パーティクル間のバネ接続を描画。
  - enable_debug_interaction: false
    $name: Debug Particle Interaction
    $name:zh-CN: 调试-粒子相互作用
    $name:zh-TW: 除錯-粒子相互作用
    $name:ja-JP: デバッグ粒子相互作用
    $description: Draw magenta repulsion links between interacting particle pairs. Line width and opacity scale with force strength.
    $description:zh-CN: 绘制粒子间相互排斥的品红色连线。线宽和透明度随力的强度变化。
    $description:zh-TW: 繪製粒子間相互排斥的品紅色連線。線寬和透明度隨力的強度變化。
    $description:ja-JP: 相互作用するパーティクル間にマゼンタの反発リンクを描画。線幅と透明度が力の強さに応じて変化。
  - enable_debug_spawn: false
    $name: Debug Spawn Points
    $name:zh-CN: 调试-生成点
    $name:zh-TW: 除錯-生成點
    $name:ja-JP: デバッグ生成点
    $description: Draw recent particle spawn positions as orange dots, plus the spawn spread radius circle at the cursor.
    $description:zh-CN: 用橙色点绘制最近的粒子生成位置，并在光标处显示生成扩散半径圆。
    $description:zh-TW: 用橙色點繪製最近的粒子生成位置，並在游標處顯示生成擴散半徑圓。
    $description:ja-JP: 最近のパーティクル生成位置をオレンジのドットで描画し、カーソル位置に生成拡散半径サークルを表示。
  - enable_debug_path: false
    $name: Debug Trail Path
    $name:zh-CN: 调试-拖尾路径
    $name:zh-TW: 除錯-拖尾路徑
    $name:ja-JP: デバッグ軌跡パス
    $description: Draw the smoothed cursor path (white dots) and raw history points (gray) to inspect trail geometry and interpolation.
    $description:zh-CN: 绘制平滑后的光标路径（白点）和原始历史点（灰点），用于检查拖尾几何和补插。
    $description:zh-TW: 繪製平滑後的游標路徑（白點）和原始歷史點（灰點），用於檢查拖尾幾何和補插。
    $description:ja-JP: 平滑化されたカーソルパス（白ドット）と生の履歴点（グレー）を描画し、軌跡の形状と補間を確認。
  $name: Debug Overlays
  $name:zh-CN: 调试
  $name:zh-TW: 除錯
  $name:ja-JP: デバッグ

*/
// ==/WindhawkModSettings==
#include <windows.h>
#include <d2d1_1.h>
#include <dwrite.h>
#include <d3d11.h>
#include <dxgi1_2.h>
#include <dxgi1_4.h>
#include <dcomp.h>
#include <dwmapi.h>
#include <d3dcompiler.h>
#include <math.h>
#include <shellapi.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <deque>
#include <string>
#include <vector>
#include <unordered_map>
#include <atomic>
#include <algorithm>
#include <ctime>
#include <clocale>
#include <locale>
#include <mmdeviceapi.h>
#include <audioclient.h>
#include <avrt.h>
#include <functiondiscoverykeys_devpkey.h>
#define GRAD_STOPS 12
static float g_dtScale = 1.0f;
static DWORD g_lastPhysicsTime = 0;
constexpr int kGradLUTSize = 256;
constexpr int kMaxGradColors = 16;
constexpr float kColorEpsilon = 0.02f;
static const D2D1_COLOR_F kColorWhite = {1.0f, 1.0f, 1.0f, 1.0f};
static const D2D1_COLOR_F kColorBlack = {0.0f, 0.0f, 0.0f, 1.0f};
static D2D1_COLOR_F HSVtoRGB(float h, float s, float v) {
    h = fmodf(h, 360.0f);
    if (h < 0)
        h += 360.0f;
    float c = v * s, x = c * (1.0f - fabsf(fmodf(h / 60.0f, 2.0f) - 1.0f)), m = v - c;
    float r, g, b;
    if (h < 60) {
        r = c;
        g = x;
        b = 0;
    } else if (h < 120) {
        r = x;
        g = c;
        b = 0;
    } else if (h < 180) {
        r = 0;
        g = c;
        b = x;
    } else if (h < 240) {
        r = 0;
        g = x;
        b = c;
    } else if (h < 300) {
        r = x;
        g = 0;
        b = c;
    } else {
        r = c;
        g = 0;
        b = x;
    }
    return D2D1::ColorF(r + m, g + m, b + m, 1.0f);
}
static void RGBtoHSV(D2D1_COLOR_F c, float &h, float &s, float &v) {
    float mx = fmaxf(fmaxf(c.r, c.g), c.b);
    float mn = fminf(fminf(c.r, c.g), c.b);
    v = mx;
    float d = mx - mn;
    s = (mx == 0.0f) ? 0.0f : d / mx;
    if (d == 0.0f)
        h = 0.0f;
    else if (mx == c.r)
        h = fmodf((c.g - c.b) / d, 6.0f);
    else if (mx == c.g)
        h = (c.b - c.r) / d + 2.0f;
    else
        h = (c.r - c.g) / d + 4.0f;
    h *= 60.0f;
    if (h < 0.0f) h += 360.0f;
}
static D2D1_COLOR_F LerpColor(D2D1_COLOR_F a, D2D1_COLOR_F b, float t) {
    return D2D1::ColorF(a.r + (b.r - a.r) * t, a.g + (b.g - a.g) * t, a.b + (b.b - a.b) * t, 1.0f);
}
namespace mtcolor {
D2D1_COLOR_F Lighten(D2D1_COLOR_F c, float amount);
}
static D2D1_COLOR_F LighterColor(D2D1_COLOR_F c, float amount = 0.55f) {
    return mtcolor::Lighten(c, amount);
}
D2D1_COLOR_F g_gradColors[16] = {{1.0f, 0.42f, 0.21f, 1.0f}, {0.0f, 0.75f, 1.0f, 1.0f}, {1.0f, 0.84f, 0.0f, 1.0f}};
int g_gradColorCount = 3;
D2D1_COLOR_F g_gradientLUT[256];
bool g_gradientLUTDirty = true;
float g_gradientFlowSpeed = 0.15f;
struct OKLab { float L, a, b; };
static inline float SrgbToLinear(float c) {
    return c <= 0.04045f ? c / 12.92f : powf((c + 0.055f) / 1.055f, 2.4f);
}
static inline float LinearToSrgb(float c) {
    return c <= 0.0031308f ? 12.92f * c : 1.055f * powf(c, 1.0f / 2.4f) - 0.055f;
}
static OKLab RGBtoOKLab(float r, float g, float b) {
    float lr = SrgbToLinear(r), lg = SrgbToLinear(g), lb = SrgbToLinear(b);
    float l = 0.4122214708f * lr + 0.5363325363f * lg + 0.0514459929f * lb;
    float m = 0.2119034982f * lr + 0.6806995451f * lg + 0.1073969566f * lb;
    float s = 0.0883024619f * lr + 0.2817188376f * lg + 0.6299787005f * lb;
    l = cbrtf(l); m = cbrtf(m); s = cbrtf(s);
    return { 0.2104542553f * l + 0.7936177850f * m - 0.0040720468f * s,
             1.9779984951f * l - 2.4285922050f * m + 0.4505937099f * s,
             0.0259040371f * l + 0.7827717662f * m - 0.8086757660f * s };
}
static D2D1_COLOR_F OKLabtoRGB(float L, float a, float b) {
    float l = L + 0.3963377774f * a + 0.2158037573f * b;
    float m = L - 0.1055613458f * a - 0.0638541728f * b;
    float s = L - 0.0894841775f * a - 1.2914855480f * b;
    l = l * l * l; m = m * m * m; s = s * s * s;
    float lr =  4.0767416621f * l - 3.3077115913f * m + 0.2309699292f * s;
    float lg = -1.2684380046f * l + 2.6097574011f * m - 0.3413193965f * s;
    float lb = -0.0041960863f * l - 0.7034186147f * m + 1.7076147010f * s;
    lr = fminf(fmaxf(LinearToSrgb(lr), 0), 1);
    lg = fminf(fmaxf(LinearToSrgb(lg), 0), 1);
    lb = fminf(fmaxf(LinearToSrgb(lb), 0), 1);
    return D2D1::ColorF(lr, lg, lb, 1.0f);
}
static D2D1_COLOR_F LerpColorOKLab(D2D1_COLOR_F a, D2D1_COLOR_F b, float t) {
    OKLab ca = RGBtoOKLab(a.r, a.g, a.b), cb = RGBtoOKLab(b.r, b.g, b.b);
    return OKLabtoRGB(ca.L + (cb.L - ca.L) * t, ca.a + (cb.a - ca.a) * t, ca.b + (cb.b - ca.b) * t);
}
namespace mtcolor {
constexpr float kLuma709R = 0.2126f;
constexpr float kLuma709G = 0.7152f;
constexpr float kLuma709B = 0.0722f;
constexpr float kWcagBlackLevel = 0.05f;
constexpr float kMinContrast = 1.0f;
inline float LumaEncoded(D2D1_COLOR_F c) {
    return kLuma709R * c.r + kLuma709G * c.g + kLuma709B * c.b;
}
inline float RelativeLuminance(D2D1_COLOR_F c) {
    return kLuma709R * SrgbToLinear(c.r) + kLuma709G * SrgbToLinear(c.g) + kLuma709B * SrgbToLinear(c.b);
}
inline float ContrastRatio(D2D1_COLOR_F a, D2D1_COLOR_F b) {
    const float la = RelativeLuminance(a);
    const float lb = RelativeLuminance(b);
    const float hi = fmaxf(la, lb);
    const float lo = fminf(la, lb);
    return (hi + kWcagBlackLevel) / (lo + kWcagBlackLevel);
}
inline float MaxChannel(D2D1_COLOR_F c) {
    return fmaxf(fmaxf(c.r, c.g), c.b);
}
inline D2D1_COLOR_F SetLuma(D2D1_COLOR_F c, float target) {
    const float y = LumaEncoded(c);
    if (y < 1e-4f)
        return D2D1::ColorF(target, target, target, c.a);
    const float k = target / y;
    const D2D1_COLOR_F o = D2D1::ColorF(c.r * k, c.g * k, c.b * k, c.a);
    const float m = MaxChannel(o);
    return (m > 1.0f) ? D2D1::ColorF(o.r / m, o.g / m, o.b / m, c.a) : o;
}
inline float Clamp01(float v) {
    return (v < 0.0f) ? 0.0f : ((v > 1.0f) ? 1.0f : v);
}
inline D2D1_COLOR_F MixToward(D2D1_COLOR_F from, D2D1_COLOR_F to, float t) {
    t = Clamp01(t);
    return D2D1::ColorF(from.r + (to.r - from.r) * t, from.g + (to.g - from.g) * t,
                        from.b + (to.b - from.b) * t, from.a);
}
inline D2D1_COLOR_F LumaToward(D2D1_COLOR_F c, float target) {
    target = Clamp01(target);
    const D2D1_COLOR_F o = SetLuma(c, target);
    const float y1 = LumaEncoded(o);
    if (fabsf(y1 - target) <= 1e-4f)
        return o;
    const D2D1_COLOR_F anchor = (target > y1) ? kColorWhite : kColorBlack;
    const float anchorY = LumaEncoded(anchor);
    const float denom = (target > y1) ? (anchorY - y1) : (y1 - anchorY);
    if (denom < 1e-5f)
        return o;
    return MixToward(o, anchor, (target - y1) / denom);
}
inline D2D1_COLOR_F Lighten(D2D1_COLOR_F c, float amount) {
    const float y0 = LumaEncoded(c);
    return LumaToward(c, y0 + (1.0f - y0) * Clamp01(amount));
}
inline D2D1_COLOR_F AdjustLumaForContrast(D2D1_COLOR_F fg, D2D1_COLOR_F bg, float targetRatio) {
    const float y0 = LumaEncoded(fg);
    const bool goWhite = ContrastRatio(kColorWhite, bg) >= ContrastRatio(kColorBlack, bg);
    const float extreme = goWhite ? 1.0f : 0.0f;
    float tBest = 1.0f;
    float tLo = 0.0f, tHi = 1.0f;
    for (int i = 0; i < 16; i++) {
        const float tMid = (tLo + tHi) * 0.5f;
        const D2D1_COLOR_F cand = LumaToward(fg, y0 + (extreme - y0) * tMid);
        if (ContrastRatio(cand, bg) >= targetRatio) {
            tBest = tMid;
            tHi = tMid;
        } else {
            tLo = tMid;
        }
    }
    return LumaToward(fg, y0 + (extreme - y0) * tBest);
}
inline D2D1_COLOR_F EnsureContrast(D2D1_COLOR_F fg, D2D1_COLOR_F bg, float targetRatio) {
    if (targetRatio <= kMinContrast)
        return fg;
    if (ContrastRatio(fg, bg) >= targetRatio)
        return fg;
    return AdjustLumaForContrast(fg, bg, targetRatio);
}
enum class ContrastLevel : int { Off = 0, AaLarge = 1, Aa = 2, Aaa = 3, Count };
inline float TargetRatioOf(ContrastLevel level) {
    switch (level) {
        case ContrastLevel::AaLarge: return 3.0f;
        case ContrastLevel::Aa:      return 4.5f;
        case ContrastLevel::Aaa:     return 7.0f;
        default:                     return kMinContrast;
    }
}
}
static void RebuildGradientLUT() {
    if (g_gradColorCount < 1) {
        for (int i = 0; i < kGradLUTSize; i++) g_gradientLUT[i] = kColorWhite;
        g_gradientLUTDirty = false;
        return;
    }
    if (g_gradColorCount == 1) {
        for (int i = 0; i < kGradLUTSize; i++) g_gradientLUT[i] = g_gradColors[0];
        g_gradientLUTDirty = false;
        return;
    }
    for (int i = 0; i < kGradLUTSize; i++) {
        float pos = (float)i / (kGradLUTSize - 1) * (g_gradColorCount - 1);
        int idx = (int)pos;
        float frac = pos - idx;
        if (idx >= g_gradColorCount - 1) { g_gradientLUT[i] = g_gradColors[g_gradColorCount - 1]; continue; }
        g_gradientLUT[i] = LerpColorOKLab(g_gradColors[idx], g_gradColors[idx + 1], frac);
    }
    g_gradientLUTDirty = false;
}
static D2D1_COLOR_F SampleGradientLUT(float ratio, float flowPhase) {
    if (g_gradientLUTDirty) RebuildGradientLUT();
    float pos = fmodf(ratio + flowPhase, 1.0f);
    if (pos < 0) pos += 1.0f;
    int idx = (int)(pos * (kGradLUTSize - 1) + 0.5f);
    if (idx < 0) idx = 0;
    if (idx > kGradLUTSize - 1) idx = kGradLUTSize - 1;
    return g_gradientLUT[idx];
}
static D2D1_COLOR_F GetGradColor(int idx) {
    if (g_gradColorCount <= 0) return kColorWhite;
    int m = idx % g_gradColorCount;
    if (m < 0) m += g_gradColorCount;
    return g_gradColors[m];
}
static inline int ClampInt(int v, int lo, int hi) {
    return v < lo ? lo : (v > hi ? hi : v);
}
static inline float ClampF(float v, float lo, float hi) {
    return v < lo ? lo : (v > hi ? hi : v);
}
constexpr int HexDigitValue(WCHAR c) {
    if (c >= L'0' && c <= L'9') return c - L'0';
    if (c >= L'a' && c <= L'f') return c - L'a' + 10;
    if (c >= L'A' && c <= L'F') return c - L'A' + 10;
    return -1;
}
static D2D1_COLOR_F ParseHexColor(PCWSTR hex, D2D1_COLOR_F fallback) {
    if (!hex || !*hex)
        return fallback;
    DWORD val = 0;
    int len = 0;
    for (; hex[len]; len++) {
        int d = HexDigitValue(hex[len]);
        if (d < 0)
            return fallback;
        val = val * 16 + (DWORD)d;
    }
    if (len != 6)
        return fallback;
    return D2D1::ColorF(((val >> 16) & 0xFF) / 255.0f, ((val >> 8) & 0xFF) / 255.0f, (val & 0xFF) / 255.0f, 1.0f);
}
static void ParseGradientColors(PCWSTR input) {
    if (!input || !*input)
        return;
    WCHAR buf[512];
    wcsncpy_s(buf, input, 511);
    buf[511] = 0;
    int idx = 0;
    WCHAR *ctx = nullptr;
    WCHAR *tok = wcstok_s(buf, L",", &ctx);
    while (tok && idx < kMaxGradColors) {
        while (*tok == L' ' || *tok == L'\t')
            tok++;
        g_gradColors[idx] = ParseHexColor(tok, g_gradColors[idx]);
        idx++;
        tok = wcstok_s(nullptr, L",", &ctx);
    }
    if (idx > 0) g_gradColorCount = idx;
    g_gradientLUTDirty = true;
}
enum ExprTokType { ET_NUM, ET_VAR, ET_FUNC, ET_OP, ET_LPAREN, ET_RPAREN };
struct ExprToken {
    ExprTokType type;
    float num;
    char name[16];
};
static std::vector<ExprToken> g_exprRPN;
static bool g_exprValid = false;
constexpr int kExprStackCap = 64;
constexpr int kExprNameLen = 16;
constexpr int kExprBufSize = 512;
constexpr float kExprPi = 3.14159265f;
constexpr float kExprE = 2.7182818f;
constexpr float kExprEps = 0.0001f;
static const char *const kExprOperators = "+-*/^";
constexpr int OpPrec(char op) {
    if (op == '+' || op == '-')
        return 1;
    if (op == '*' || op == '/')
        return 2;
    if (op == '^')
        return 4;
    return 0;
}
static bool IsExprOperator(char c) {
    return c != '\0' && strchr(kExprOperators, c) != nullptr;
}
static ExprToken MakeExprToken(ExprTokType type, float num) {
    ExprToken t = {};
    t.type = type;
    t.num = num;
    return t;
}
static void TokenizeExpression(const char *buf, std::vector<ExprToken> &tokens) {
    int i = 0;
    while (buf[i]) {
        if (isdigit((unsigned char)buf[i]) || buf[i] == '.') {
            float val = 0;
            bool hasDot = false;
            float div = 1;
            while (isdigit((unsigned char)buf[i]) || buf[i] == '.') {
                if (buf[i] == '.')
                    hasDot = true;
                else if (hasDot) {
                    div *= 10;
                    val += (buf[i] - '0') / div;
                } else
                    val = val * 10 + (buf[i] - '0');
                i++;
            }
            tokens.push_back(MakeExprToken(ET_NUM, val));
        } else if (isalpha((unsigned char)buf[i])) {
            char name[kExprNameLen] = {0};
            int j = 0;
            while (isalnum((unsigned char)buf[i]) && j < kExprNameLen - 1)
                name[j++] = buf[i++];
            ExprToken tok = MakeExprToken(buf[i] == '(' ? ET_FUNC : ET_VAR, 0);
            strcpy_s(tok.name, name);
            tokens.push_back(tok);
        } else if (buf[i] == '(') {
            tokens.push_back(MakeExprToken(ET_LPAREN, 0));
            i++;
        } else if (buf[i] == ')') {
            tokens.push_back(MakeExprToken(ET_RPAREN, 0));
            i++;
        } else if (IsExprOperator(buf[i])) {
            ExprToken tok = MakeExprToken(ET_OP, 0);
            tok.name[0] = buf[i];
            tok.name[1] = 0;
            tokens.push_back(tok);
            i++;
        } else
            i++;
    }
}
static void BuildExprRPN(const std::vector<ExprToken> &tokens, std::vector<ExprToken> &output) {
    std::vector<ExprToken> stack;
    for (const ExprToken &tok : tokens) {
        if (tok.type == ET_NUM || tok.type == ET_VAR) {
            output.push_back(tok);
        } else if (tok.type == ET_FUNC) {
            stack.push_back(tok);
        } else if (tok.type == ET_OP) {
            while (!stack.empty() && stack.back().type != ET_LPAREN && stack.back().type != ET_FUNC &&
                   OpPrec(stack.back().name[0]) >= OpPrec(tok.name[0])) {
                output.push_back(stack.back());
                stack.pop_back();
            }
            stack.push_back(tok);
        } else if (tok.type == ET_LPAREN) {
            stack.push_back(tok);
        } else if (tok.type == ET_RPAREN) {
            while (!stack.empty() && stack.back().type != ET_LPAREN) {
                output.push_back(stack.back());
                stack.pop_back();
            }
            if (!stack.empty())
                stack.pop_back();
            if (!stack.empty() && stack.back().type == ET_FUNC) {
                output.push_back(stack.back());
                stack.pop_back();
            }
        }
    }
    while (!stack.empty()) {
        output.push_back(stack.back());
        stack.pop_back();
    }
}
static void CompileExpression(const char *expr) {
    g_exprValid = false;
    g_exprRPN.clear();
    if (!expr || !*expr)
        return;
    char buf[kExprBufSize];
    int bi = 0;
    bool expectVal = true;
    for (int i = 0; expr[i] && bi < kExprBufSize - 2; i++) {
        char c = expr[i];
        if (c == '-' && expectVal) {
            buf[bi++] = '0';
            buf[bi++] = '-';
            expectVal = false;
            continue;
        }
        if (c == ' ' || c == '\t')
            continue;
        buf[bi++] = c;
        expectVal = (c == '(' || IsExprOperator(c));
    }
    buf[bi] = 0;
    std::vector<ExprToken> tokens;
    TokenizeExpression(buf, tokens);
    std::vector<ExprToken> output;
    BuildExprRPN(tokens, output);
    g_exprRPN = output;
    g_exprValid = !g_exprRPN.empty();
}
static float EvalExprVariable(const char *name, float t, float d, float time) {
    if (strcmp(name, "t") == 0)    return t;
    if (strcmp(name, "d") == 0)    return d;
    if (strcmp(name, "time") == 0) return time;
    if (strcmp(name, "pi") == 0)   return kExprPi;
    if (strcmp(name, "e") == 0)    return kExprE;
    return 0;
}
static float EvalExprFunction(const char *name, float a) {
    if (strcmp(name, "sin") == 0)  return sinf(a);
    if (strcmp(name, "cos") == 0)  return cosf(a);
    if (strcmp(name, "tan") == 0)  return tanf(a);
    if (strcmp(name, "exp") == 0)  return expf(a);
    if (strcmp(name, "sqrt") == 0) return sqrtf(fabsf(a));
    if (strcmp(name, "abs") == 0)  return fabsf(a);
    if (strcmp(name, "log") == 0)  return logf(fabsf(a) + kExprEps);
    return 0;
}
static float EvalExpression(float t, float d, float time) {
    if (!g_exprValid || g_exprRPN.empty())
        return 0;
    float stk[kExprStackCap];
    int sp = 0;
    for (const ExprToken &tok : g_exprRPN) {
        if (sp >= kExprStackCap - 1)
            break;
        if (tok.type == ET_NUM)
            stk[sp++] = tok.num;
        else if (tok.type == ET_VAR)
            stk[sp++] = EvalExprVariable(tok.name, t, d, time);
        else if (tok.type == ET_OP) {
            if (sp < 2) {
                sp = 0;
                break;
            }
            float b = stk[--sp], a = stk[--sp], r = 0;
            switch (tok.name[0]) {
                case '+': r = a + b; break;
                case '-': r = a - b; break;
                case '*': r = a * b; break;
                case '/': r = (b != 0) ? a / b : 0; break;
                case '^': r = powf(fabsf(a) + kExprEps, b); break;
            }
            stk[sp++] = r;
        } else if (tok.type == ET_FUNC) {
            if (sp < 1) {
                sp = 0;
                break;
            }
            float a = stk[--sp];
            stk[sp++] = EvalExprFunction(tok.name, a);
        }
    }
    return sp > 0 ? stk[sp - 1] : 0;
}
HWND g_overlayHwnd = NULL;
constexpr UINT WM_APP_SHOW_OVERLAY = WM_USER + 101;
constexpr UINT WM_APP_HIDE_OVERLAY = WM_USER + 102;
constexpr UINT WM_APP_REHOTKEY = WM_USER + 103;
constexpr UINT WM_APP_OVERLAY_LAYER = WM_USER + 104;
HANDLE g_threadHandle = NULL;
HANDLE g_readyEvent = NULL;
HANDLE g_renderExitEvent = NULL;
HANDLE g_renderThread = NULL;
std::deque<POINT> g_history;
POINT g_lastPos = {0, 0};
int g_trailDelay = 0;
float g_lagFX = 0.0f, g_lagFY = 0.0f;
DWORD g_lagLastT = 0;
POINT g_lagPos = {0, 0};
bool g_lagInited = false;
float g_fadeAlpha = 1.0f;
D2D1_COLOR_F g_cursorExtractedColor = {0.5f, 0.5f, 0.5f, 1.0f};
bool g_cursorColorShift = true;
int g_colorShiftMode = 1;
int g_colorShiftAngle = 180;
constexpr float g_colorShiftSatBoost = 0.55f;
constexpr float g_colorShiftValBoost = 0.72f;
static DWORD s_lastColorExtract = 0;
static HANDLE g_bgSampleThread = NULL;
static HANDLE g_bgSampleExitEvent = NULL;
static CRITICAL_SECTION g_sampleCS;
static std::vector<D2D1_POINT_2F> g_samplePath;
static int g_sampleVX = 0, g_sampleVY = 0;
static POINT g_sampleCursor = {0, 0};
static DWORD g_sampleTime = 0;
static bool g_sampleHasPath = false;
static int g_sampleColorMode = 0;
static bool g_sampleAdaptive = false;
ID3D11Device *g_pD3DDevice = nullptr;
ID3D11DeviceContext *g_pD3DContext = nullptr;
IDXGIDevice *g_pDXGIDevice = nullptr;
ID2D1Factory1 *g_pD2DFactory = nullptr;
IDWriteFactory *g_dwFactory = nullptr;
IDWriteTextFormat *g_pTextFormat = nullptr;
wchar_t g_textContent[256] = L"Hi";
int g_textFontSize = 120;
bool g_enableTextChunk = true;
int  g_textChunkMode = 0;
int  g_textChunkMax = 3;
int  g_textChunkDelay = 600;
int  g_textOffsetX = 0;
int  g_textOffsetY = 0;
ID2D1Device *g_pD2DDevice = nullptr;
ID2D1DeviceContext *g_pD2DDC = nullptr;
IDXGISwapChain1 *g_pSwapChain = nullptr;
bool g_isHDRMode = false;
DXGI_FORMAT g_swapChainFormat = DXGI_FORMAT_B8G8R8A8_UNORM;
ID2D1Bitmap1 *g_pD2DTargetBitmap = nullptr;
ID3D11RenderTargetView *g_pCachedRTV = nullptr;
int g_msaaSamples = 1;
ID3D11Texture2D *g_pMSAATexture = nullptr;
ID3D11RenderTargetView *g_pMSAARTV = nullptr;
int g_ssaaScale = 1;
int g_createdMsaaSamples = 1;
int g_createdSsaaScale = 1;
int g_renderW = 0, g_renderH = 0;
ID3D11Texture2D *g_pSSAATexture = nullptr;
ID3D11RenderTargetView *g_pSSAARTV = nullptr;
ID3D11ShaderResourceView *g_pSSAASRV = nullptr;
ID3D11Texture2D *g_pSSAAResolveTexture = nullptr;
ID3D11ShaderResourceView *g_pSSAAResolveSRV = nullptr;
ID3D11VertexShader *g_pBlitVS = nullptr;
ID3D11PixelShader *g_pBlitPS = nullptr;
ID3D11Buffer *g_pBlitVB = nullptr;
ID3D11InputLayout *g_pBlitLayout = nullptr;
ID3D11SamplerState *g_pBlitSampler = nullptr;
ID3D11Buffer *g_pBlitCB = nullptr;
ID3D11PixelShader *g_pTemporalPS = nullptr;
ID3D11PixelShader *g_pFXAAPS = nullptr;
ID3D11PixelShader *g_pBloomThresholdPS = nullptr;
ID3D11PixelShader *g_pBloomBlurPS = nullptr;
ID3D11PixelShader *g_pBloomCompositePS = nullptr;
ID3D11PixelShader *g_pHDREncodePS = nullptr;
ID3D11Buffer *g_pPostCB = nullptr;
ID3D11Texture2D *g_pSceneTex = nullptr;
ID3D11RenderTargetView *g_pSceneRTV = nullptr;
ID3D11ShaderResourceView *g_pSceneSRV = nullptr;
ID3D11Texture2D *g_pPostTex = nullptr;
ID3D11RenderTargetView *g_pPostRTV = nullptr;
ID3D11ShaderResourceView *g_pPostSRV = nullptr;
ID3D11Texture2D *g_pHistoryTex = nullptr;
ID3D11RenderTargetView *g_pHistoryRTV = nullptr;
ID3D11ShaderResourceView *g_pHistorySRV = nullptr;
static const int kPostBloomLevels = 4;
ID3D11Texture2D *g_pBloomTex[kPostBloomLevels] = {nullptr, nullptr, nullptr, nullptr};
ID3D11RenderTargetView *g_pBloomRTV[kPostBloomLevels] = {nullptr, nullptr, nullptr, nullptr};
ID3D11ShaderResourceView *g_pBloomSRV[kPostBloomLevels] = {nullptr, nullptr, nullptr, nullptr};
int g_postRTW = 0, g_postRTH = 0;
int g_postRTFormat = 0;
bool g_postRTsFresh = false;
IDCompositionDevice *g_pDCompDevice = nullptr;
IDCompositionTarget *g_pDCompTarget = nullptr;
IDCompositionVisual *g_pDCompVisual = nullptr;
ID2D1SolidColorBrush *g_pSolidOuterBrush = nullptr;
ID2D1SolidColorBrush *g_pSolidInnerBrush = nullptr;
ID2D1SolidColorBrush *g_pShadowBrush = nullptr;
ID2D1LinearGradientBrush *g_pGradOuterBrush = nullptr;
ID2D1LinearGradientBrush *g_pGradInnerBrush = nullptr;
ID2D1GradientStopCollection *g_pGradOuterStops = nullptr;
ID2D1GradientStopCollection *g_pGradInnerStops = nullptr;
ID2D1Mesh *g_pShadowMesh = nullptr;
ID2D1Mesh *g_pGlow2Mesh = nullptr;
ID2D1Mesh *g_pGlowMesh = nullptr;
ID2D1Mesh *g_pOuterMesh = nullptr;
ID2D1Mesh *g_pInnerMesh = nullptr;
int g_cachedVW = 0, g_cachedVH = 0;
int g_virtX = 0, g_virtY = 0, g_virtW = 0, g_virtH = 0;
static inline void PublishVirtualScreen(int x, int y, int w, int h) {
    std::atomic_ref<int>(g_virtX).store(x, std::memory_order_release);
    std::atomic_ref<int>(g_virtY).store(y, std::memory_order_release);
    std::atomic_ref<int>(g_virtW).store(w, std::memory_order_release);
    std::atomic_ref<int>(g_virtH).store(h, std::memory_order_release);
}
static inline void LoadVirtualScreen(int &x, int &y, int &w, int &h) {
    x = std::atomic_ref<int>(g_virtX).load(std::memory_order_acquire);
    y = std::atomic_ref<int>(g_virtY).load(std::memory_order_acquire);
    w = std::atomic_ref<int>(g_virtW).load(std::memory_order_acquire);
    h = std::atomic_ref<int>(g_virtH).load(std::memory_order_acquire);
}
float g_triggerVelocity = 25.0f, g_stopVelocity = 10.0f;
bool g_desktopOnly = false;
bool g_desktopSuppressed = false;
int g_desktopLayer = 0;
bool g_foregroundHook = true;
std::atomic<bool> g_desktopActive{true};
std::atomic<bool> g_fgHookActive{false};
std::atomic<bool> g_overlayTopmost{true};
int g_tailOffsetX = 6, g_tailOffsetY = 10, g_tailLength = 10;
bool g_enableSmoothGradient = true;
int g_fadeoutMode = 2;
bool g_enableSpeedResponse = true;
bool g_enableFrameSync = true;
bool g_enhancedGlow = true;
bool g_enableHeadHighlight = true;
bool g_adaptiveContrast = false;
int g_contrastTarget = 0;
    std::atomic<float> g_bgLuminance{0.5f};
DWORD g_lastBgSample = 0;
bool g_enableTrailShadow = true;
int g_trailShape = 0;
int g_dotsMultiplier = 2;
int g_dotChainSize = 100;
int g_particleSizeMultiplier = 100;
bool g_enableParticleGlow = true;
int g_particleGlowIntensity = 40;
bool g_enableParticleStroke = true;
int g_particleStrokeWidth = 3;
int g_particleStrokeAlpha = 80;
int g_particleStrokeColorMode = 0;
D2D1_COLOR_F g_particleStrokeColor = {0.10f, 0.10f, 0.12f, 1.0f};
int g_functionPreset = 0;
char g_customFunction[256] = "sin(d * 0.15) * 8";
int g_waveAmplitude = 8, g_waveFrequency = 15;
bool g_enableGlow = true;
int g_glowIntensity = 40;
bool g_enableTrailStroke = true;
int g_trailStrokeWidth = 12;
int g_edgeSoftness = 50;
int g_aaMode = 1;
int g_shaderQuality = 1;
int g_ssaaFilter = 1;
int g_ssaaSharpness = 0;
int g_sdfSamples = 1;
int g_ditherMode = 1;
int g_ditherStrength = 50;
float g_textAtlasSS = 2.0f;
int g_colorMode = 0;
D2D1_COLOR_F g_customColor = {0.0f, 0.75f, 1.0f, 1.0f};
int g_particleMode = 1;
int g_particleOrigin = 2;
int g_particleOriginRatio = 80;
float g_particleAttraction = 0.032f;
bool g_enableParticleRepel = true;
int g_particleRepelRadius = 25;
float g_particleRepelForce = 0.9f;
int g_particleDensity = 3;
int g_particleInterval = 50;
bool g_particleAccel = true;
int g_particleShape = 9;
bool g_enableParticleSpin = true;
int g_particleSpinSpeed = 30;
bool g_enableParticleInteraction = true;
int g_interParticleRepelDistance = 25;
int g_particleSpawnSpread = 30;
float g_interParticleRepelForce = 0.15f;
int g_particleRepelAlignment = 50;
bool g_enableParticleMass = true;
float g_particleMassMin = 0.5f;
float g_particleMassMax = 2.0f;
bool g_enableParticleGravity = false;
float g_gravityStrength = 0.5f;
int g_gravitySystem = 0;
int g_gravityBodyCount = 3;
std::vector<int> g_gravityBodies;
std::vector<bool> g_gravityBodyFlag;
struct MusicState {
    bool initialized = false;
    bool capturing = false;
    IMMDeviceEnumerator *pEnumerator = nullptr;
    IMMDevice *pDevice = nullptr;
    IAudioClient *pAudioClient = nullptr;
    IAudioCaptureClient *pCaptureClient = nullptr;
    HANDLE hCaptureThread = nullptr;
    HANDLE hStopEvent = nullptr;
    std::atomic<bool> captureRunning{false};
    WAVEFORMATEX *pWaveFormat = nullptr;
    UINT32 bufferFrames = 0;
    int fftSize = 512;
    std::vector<float> fftInput;
    std::vector<float> fftOutput;
    std::vector<float> fftSmoothed;
    std::vector<float> windowFunction;
    std::vector<float> fftWorkReal;
    std::vector<float> fftWorkImag;
    int fftWritePos = 0;
    float beatAvgEnergy = 0;
    float beatThreshold = 0;
    bool beatDetected = false;
    float beatIntensity = 0;
    DWORD lastBeatTime = 0;
    float bpmEstimate = 0;
    std::deque<DWORD> beatHistory;
    std::vector<float> prevSpectrum;
    float avgSpectralFlux = 0;
    float bassBeatAvg = 0;
    bool bassBeat = false;
    float midBeatAvg = 0;
    bool midBeat = false;
    float trebleBeatAvg = 0;
    bool trebleBeat = false;
    float bassLevel = 0;
    float midLevel = 0;
    float trebleLevel = 0;
    float overallLevel = 0;
};
MusicState g_music;
bool g_enableMusicReactive = false;
int g_musicFftSize = 512;
float g_musicSensitivity = 0.5f;
float g_musicSmoothing = 0.5f;
bool g_enableMusicPhysics = false;
float g_musicGravityLink = 0.5f;
float g_musicBeatPulse = 0.5f;
float g_musicBassSize = 0.5f;
float g_musicVortexLink = 0.5f;
float g_musicBeatPulseAmount = 0;
float g_musicBassSizeBoost = 0;
bool g_enableParticleCollision = false;
bool g_enableLorentzForce = false;
float g_lorentzStrength = 0.3f;
bool g_enableBrownianMotion = false;
float g_brownianStrength = 0.2f;
bool g_enableMultiBandLink = false;
bool g_enableEnvGravity = false;
float g_envGravityStrength = 0.3f;
float g_envGravityAngle = 1.5708f;
bool g_enableEnvWind = false;
float g_windStrength = 0.2f;
float g_windTurbulence = 0.5f;
float g_windX = 0, g_windY = 0;
bool g_enableParticleSpring = false;
float g_springK = 0.02f;
float g_springMaxDist = 50.0f;
std::vector<int> g_springLinkCount;
bool g_debugVelocity = false;
bool g_debugForce = false;
bool g_debugVortex = false;
bool g_debugGravity = false;
bool g_debugCollision = false;
bool g_debugSpring = false;
bool g_debugInteraction = false;
bool g_debugSpawn = false;
bool g_debugPath = false;
struct DebugSpawnPoint { float x, y; DWORD time; };
[[clang::no_destroy]] static std::vector<DebugSpawnPoint> g_debugSpawnPoints;
float g_bassGravityBoost = 0;
float g_midRepelBoost = 0;
float g_trebleNoiseBoost = 0;
bool g_enableCentripetal = false;
float g_centripetalForce = 0.5f;
float g_centripetalSensitivity = 0.5f;
int g_centripetalDuration = 1500;
int g_vortexMaxCount = 2;
float g_vortexMinDistance = 200.0f;
float g_vortexDrift = 0.3f;
float g_vortexDurationSpeed = 0.2f;
int g_vortexPhysModel = 0;
#define MAX_VORTICES 8
struct Vortex {
    bool active;
    float centerX, centerY;
    float driftX, driftY;
    float radius;
    float coreRadius;
    float angularVel;
    float circulation;
    float strength;
    float pressureGradient;
    bool isCircling;
    DWORD startTime;
    int actualDuration;
    float orbitTilt;
    float orbitTiltVel;
    float orbitPhase;
    float stretchRate;
};
Vortex g_vortices[MAX_VORTICES] = {};
struct CircleDetect {
    float historyX[20];
    float historyY[20];
    int historyCount;
    float angularVel;
    float circleCenterX, circleCenterY;
    float circleRadius;
    bool isCircling;
    DWORD lastCircleTime;
} g_circleDetect = {};
ID2D1PathGeometry *g_pStarGeom = nullptr;
ID2D1PathGeometry *g_pHexagramGeom = nullptr;
ID2D1PathGeometry *g_pHeartGeom = nullptr;
struct TrailShape {
    float x, y;
    float vx, vy;
    float size;
    float rotation;
    float rotSpeed;
    int shapeType;
    DWORD startTime;
    float lifetime;
    D2D1_COLOR_F color;
};
std::vector<TrailShape> g_trailShapes;
float g_lastShapeX = 0, g_lastShapeY = 0;
float g_lastShapeDirX = 1.0f, g_lastShapeDirY = 0.0f;
bool g_hasLastShapePos = false;
bool g_missileIdleExplode = false;
int g_missileIdleDelay = 400;
int g_missileExplodeParticles = 12;
int g_missileExplodeFx = 2;
bool g_fxBloomPulse = false;
bool g_fxGlowBreath = false;
bool g_fxEdgeFlow = false;
bool g_enableCollisionSpark = false;
int g_sparkChance = 40;
int g_sparkIntensity = 100;
static int g_sparkBudget = 0;
bool g_fxDitherShimmer = false;
bool g_fxStrokeFlow = false;
bool g_fxBloomThrWave = false;
bool g_fxBlurPulse = false;
bool g_fxHdrShimmer = false;
int g_fxAnimSpeed = 100;
int g_fxAnimAmount = 50;
int g_fxAnimWave = 0;
static float FxShape(float phase01, int wave) {
    switch (wave) {
    case 1: {
        return (phase01 < 0.30f) ? (phase01 / 0.30f) : (1.0f - (phase01 - 0.30f) / 0.70f);
    }
    case 2:
        return (phase01 < 0.5f) ? (phase01 * 2.0f) : (2.0f - phase01 * 2.0f);
    case 3: {
        float n = sinf(phase01 * 91.17f) * 43758.5453f;
        return n - floorf(n);
    }
    default:
        return 0.5f + 0.5f * sinf(phase01 * 6.2831853f);
    }
}
static float FxWave(DWORD dwTime, float phase) {
    float hz = ((float)g_fxAnimSpeed / 100.0f) * 0.5f;
    float p = (float)dwTime * 0.001f * hz + phase;
    p -= floorf(p);
    return FxShape(p, g_fxAnimWave);
}
float g_fxBloomMul = 1.0f, g_fxGlowMul = 1.0f, g_fxEdgeMul = 1.0f, g_fxDitherMul = 1.0f;
float g_fxStrokeMul = 1.0f, g_fxBloomThrMul = 1.0f, g_fxBlurMul = 1.0f, g_fxHdrMul = 1.0f;
static void UpdateFxAnimations(DWORD dwTime) {
    g_fxBloomMul  = (!g_fxBloomPulse || g_fxAnimAmount <= 0)
        ? 1.0f : 1.0f + (FxWave(dwTime, 0.00f) - 0.5f) * 2.0f * ((float)g_fxAnimAmount / 100.0f);
    g_fxGlowMul   = (!g_fxGlowBreath || g_fxAnimAmount <= 0)
        ? 1.0f : 1.0f + (FxWave(dwTime, 0.33f) - 0.5f) * 2.0f * ((float)g_fxAnimAmount / 100.0f);
    g_fxEdgeMul   = (!g_fxEdgeFlow || g_fxAnimAmount <= 0)
        ? 1.0f : 1.0f + (FxWave(dwTime, 0.66f) - 0.5f) * 2.0f * ((float)g_fxAnimAmount / 100.0f);
    g_fxDitherMul = (!g_fxDitherShimmer || g_fxAnimAmount <= 0)
        ? 1.0f : 1.0f + (FxWave(dwTime, 0.15f) - 0.5f) * 2.0f * ((float)g_fxAnimAmount / 100.0f);
    g_fxStrokeMul = (!g_fxStrokeFlow || g_fxAnimAmount <= 0)
        ? 1.0f : 1.0f + (FxWave(dwTime, 0.42f) - 0.5f) * 2.0f * ((float)g_fxAnimAmount / 100.0f);
    g_fxBloomThrMul = (!g_fxBloomThrWave || g_fxAnimAmount <= 0)
        ? 1.0f : 1.0f + (FxWave(dwTime, 0.58f) - 0.5f) * 2.0f * ((float)g_fxAnimAmount / 100.0f);
    g_fxBlurMul = (!g_fxBlurPulse || g_fxAnimAmount <= 0)
        ? 1.0f : 1.0f + (FxWave(dwTime, 0.77f) - 0.5f) * 2.0f * ((float)g_fxAnimAmount / 100.0f);
    g_fxHdrMul = (!g_fxHdrShimmer || g_fxAnimAmount <= 0)
        ? 1.0f : 1.0f + (FxWave(dwTime, 0.88f) - 0.5f) * 2.0f * ((float)g_fxAnimAmount / 100.0f);
}
static float FxApply(float base, float mul) {
    return fminf(fmaxf(base * mul, 0.0f), 1.0f);
}
int g_shapeType = 0;
int g_shapeInterval = 30;
bool g_shapeRandomOffset = true;
int g_shapeCount = 1;
float g_shapeSize = 12.0f;
int g_shapeLifetime = 800;
bool g_superPerformanceMode = false;
bool g_enableTemporalBlur = false;
int g_temporalBlurAmount = 55;
int g_postAA = 0;
bool g_enableBloom = false;
int g_bloomThreshold = 60;
int g_bloomIntensity = 50;
int g_hdrMode = 0;
int g_hdrHighlightBoost = 0;
int g_hdrTonemap = 0;
bool g_hdrColorSpaceApplied = false;
int g_dpiScaleMode = 0;
bool g_dpiScaleText = false;
std::atomic<float> g_monitorDpiScale{1.0f};
std::atomic<bool> g_effectsVisible{true};
std::atomic<bool> g_trailVisible{true};
std::atomic<bool> g_particlesVisible{true};
int g_hotkeyModifier = 0;
wchar_t g_hotkeyToggleKey[32] = L"";
wchar_t g_hotkeyTrailKey[32] = L"";
wchar_t g_hotkeyParticlesKey[32] = L"";
int g_hotkeyToggleVK = 0;
int g_hotkeyTrailVK = 0;
int g_hotkeyParticlesVK = 0;
bool g_enableBezierSmooth = true;
bool g_enableMotionBlur = false;
int g_motionBlurStrength = 3;
bool g_enable25DEffect = true;
int g_perspectiveStrength = 15;
struct TrailFrame {
    std::vector<D2D1_POINT_2F> path;
    DWORD time;
};
std::vector<TrailFrame> g_trailHistory;
std::atomic<bool> g_settingsDirty{false};
std::atomic<bool> g_deviceLost{false};
DWORD g_lastParticleTime = 0;
float g_lastParticleX = 0, g_lastParticleY = 0;
bool g_hasLastParticlePos = false;
float g_lastParticleDirX = 0.0f, g_lastParticleDirY = 0.0f;
float g_prevVelocity = 0;
bool g_enableClickStarburst = true;
int g_starburstCount = 8;
bool g_enableClickEffect = true;
int g_clickMaxRadius = 40, g_clickDuration = 300;
int g_clickStyle = 0;
struct Particle {
    float x, y, vx, vy, size;
    float z;
    float mass;
    float charge;
    float rotation;
    float spinSpeed;
    DWORD startTime;
    int lifetime;
    D2D1_COLOR_F color;
    D2D1_COLOR_F endColor;
    int shapeType;
    float charIndex;
    int chunkStart;
    int chunkCount;
    float colorOffset[3];
    float vortexBrightness[3];
    float debugForceX, debugForceY;
};
std::vector<Particle> g_particles;
static inline int GetParticleCap();
static void SpawnCollisionSpark(float cx, float cy, float nx, float ny, float approachSpeed) {
    if (g_sparkBudget <= 0) return;
    if ((int)g_particles.size() > GetParticleCap() * 85 / 100) return;
    int want = 1 + (int)((approachSpeed * 0.03f) * (g_sparkIntensity / 100.0f));
    want = ClampInt(want, 1, 3);
    for (int k = 0; k < want; k++) {
        if (g_sparkBudget <= 0) break;
        g_sparkBudget--;
        Particle p = {};
        float base = atan2f(ny, nx);
        float ang = base + (((float)rand() / RAND_MAX) - 0.5f) * 1.9f;
        float sp = (1.1f + approachSpeed * 0.030f) * (0.55f + 0.45f * ((float)rand() / RAND_MAX));
        p.x = cx; p.y = cy;
        p.vx = cosf(ang) * sp;
        p.vy = sinf(ang) * sp;
        p.size = 0.45f + ((float)rand() / RAND_MAX) * 0.5f;
        p.z = 0.5f;
        p.mass = 1.0f;
        p.charge = 0.0f;
        p.rotation = 0.0f;
        p.spinSpeed = 0.0f;
        p.startTime = GetTickCount();
        p.lifetime = 110 + rand() % 130;
        p.color = D2D1::ColorF(1.0f, 0.92f, 0.72f, 1.0f);
        p.endColor = D2D1::ColorF(0.85f, 0.28f, 0.06f, 1.0f);
        p.shapeType = 0;
        g_particles.push_back(p);
    }
}
struct Ripple {
    POINT pos;
    DWORD startTime;
};
std::vector<Ripple> g_ripples;
bool g_prevLButton = false, g_prevRButton = false;
struct GradData {
    D2D1_GRADIENT_STOP outer[GRAD_STOPS];
    D2D1_GRADIENT_STOP inner[GRAD_STOPS];
    D2D1_COLOR_F solidOuter, solidInner;
};
static GradData s_cachedGrad = {};
static bool s_gradValid = false;
static bool ColorApproxEq(D2D1_COLOR_F a, D2D1_COLOR_F b) {
    return fabsf(a.r - b.r) < kColorEpsilon && fabsf(a.g - b.g) < kColorEpsilon && fabsf(a.b - b.b) < kColorEpsilon;
}
static bool GradApproxEq(const GradData &a, const GradData &b) {
    for (int i = 0; i < GRAD_STOPS; i++) {
        if (!ColorApproxEq(a.outer[i].color, b.outer[i].color))
            return false;
        if (!ColorApproxEq(a.inner[i].color, b.inner[i].color))
            return false;
    }
    return true;
}
static void SetGradStopPair(GradData &out, int i, float ratio, D2D1_COLOR_F co, D2D1_COLOR_F ci, float alpha) {
    out.outer[i] = {ratio, D2D1::ColorF(co.r, co.g, co.b, alpha)};
    out.inner[i] = {ratio, D2D1::ColorF(ci.r, ci.g, ci.b, alpha)};
}
static D2D1_COLOR_F SnapshotCursorExtractedColor() {
    D2D1_COLOR_F snap;
    snap.r = std::atomic_ref<float>(g_cursorExtractedColor.r).load(std::memory_order_acquire);
    snap.g = std::atomic_ref<float>(g_cursorExtractedColor.g).load(std::memory_order_acquire);
    snap.b = std::atomic_ref<float>(g_cursorExtractedColor.b).load(std::memory_order_acquire);
    snap.a = std::atomic_ref<float>(g_cursorExtractedColor.a).load(std::memory_order_acquire);
    return snap;
}
static void ComputeColors(int mode, DWORD time, float velocity, GradData &out) {
    float t = time / 1000.0f;
    D2D1_COLOR_F headOuter, headInner, tailOuter, tailInner;
    switch (mode) {
        case 0:
            headOuter = tailOuter = g_customColor;
            headInner = tailInner = LighterColor(g_customColor);
            break;
        case 1: {
            float phase = fmodf(t * g_gradientFlowSpeed, 1.0f);
            headOuter = SampleGradientLUT(0.0f, phase);
            tailOuter = SampleGradientLUT(1.0f, phase);
            headInner = LighterColor(headOuter, 0.5f);
            tailInner = LighterColor(tailOuter, 0.5f);
            break;
        }
        case 2: {
            float speedBoost = fminf(velocity / 50.0f, 1.0f);
            float h = fmodf(t * (40 + speedBoost * 60), 360);
            float h2 = fmodf(h + 200 + speedBoost * 40, 360);
            float sat = 0.75f + speedBoost * 0.2f;
            headOuter = HSVtoRGB(h, sat, 1.0f);
            tailOuter = HSVtoRGB(h2, sat * 0.9f, 0.95f);
            headInner = HSVtoRGB(h, sat * 0.35f, 1.0f);
            tailInner = HSVtoRGB(h2, sat * 0.3f, 0.95f);
            break;
        }
        case 3: {
            float h = fmodf(t * 25, 60) - 15;
            float flicker = 0.84f + 0.16f * ((0.15f * sinf(t * 8.0f) + 0.08f * sinf(t * 17.0f)) / 0.23f);
            headOuter = HSVtoRGB(h, 0.95f, flicker);
            tailOuter = HSVtoRGB(fmodf(h + 25, 360), 0.85f, flicker * 0.85f);
            headInner = HSVtoRGB(h + 10, 0.5f, 1.0f);
            tailInner = HSVtoRGB(fmodf(h + 35, 360), 0.4f, 0.95f);
            break;
        }
        case 4: {
            float h = 160 + fmodf(t * 20, 130);
            float shimmer = 0.90f + 0.10f * ((0.1f * sinf(t * 6.0f) + 0.05f * sinf(t * 13.0f)) / 0.15f);
            headOuter = HSVtoRGB(h, 0.75f, shimmer);
            tailOuter = HSVtoRGB(fmodf(h + 50, 360), 0.7f, shimmer * 0.9f);
            headInner = HSVtoRGB(h, 0.3f, 1.0f);
            tailInner = HSVtoRGB(fmodf(h + 50, 360), 0.25f, 0.98f);
            break;
        }
        case 5: {
            float speedBoost = fminf(velocity / 60.0f, 1.0f);
            float p = 0.5f + 0.5f * sinf(t * (3.5f + speedBoost * 2.5f));
            float sharpPulse = p * p * (3 - 2 * p);
            D2D1_COLOR_F c = g_customColor;
            float hueShift = sharpPulse * 20.0f;
            float h, s, v; RGBtoHSV(c, h, s, v);
            headOuter = HSVtoRGB(fmodf(h + hueShift, 360), s, 0.3f + sharpPulse * 0.7f);
            tailOuter = HSVtoRGB(fmodf(h - hueShift, 360), s * 0.8f, sharpPulse * 0.4f);
            headInner = LighterColor(headOuter, 0.5f);
            tailInner = LighterColor(tailOuter, 0.5f);
            break;
        }
        case 6: {
            float sn = fminf(powf(velocity / 70.0f, 0.55f), 1.0f);
            float h = 240 - sn * 240;
            float sat = 0.7f + sn * 0.25f;
            float val = 0.85f + sn * 0.15f;
            headOuter = HSVtoRGB(h, sat, val);
            tailOuter = HSVtoRGB(fmodf(h + 40, 360), sat * 0.8f, val * 0.85f);
            headInner = HSVtoRGB(h, sat * 0.35f, 1.0f);
            tailInner = HSVtoRGB(fmodf(h + 40, 360), sat * 0.25f, 0.95f);
            break;
        }
        case 7:
            headOuter = GetGradColor(0);
            tailOuter = GetGradColor(1);
            headInner = LighterColor(GetGradColor(0));
            tailInner = LighterColor(GetGradColor(1));
            break;
        case 8: {
            float speedBoost = fminf(velocity / 50.0f, 1.0f);
            float turbulence = 0.75f + 0.25f * sinf(t * (10 + speedBoost * 8)) * sinf(t * 5.5f) + 0.12f * sinf(t * (20 + speedBoost * 10));
            float core = 0.9f + 0.1f * sinf(t * 15);
            headOuter = D2D1::ColorF(1.0f * turbulence, (0.65f + speedBoost * 0.15f) * turbulence, 0.1f * core, 1);
            tailOuter = D2D1::ColorF(0.85f, 0.12f, 0.0f, 1);
            headInner = D2D1::ColorF(1.0f, 0.92f + speedBoost * 0.08f, 0.55f + speedBoost * 0.2f, 1);
            tailInner = D2D1::ColorF(0.95f, 0.35f, 0.05f, 1);
            break;
        }
        case 9: {
            float speedBoost = fminf(velocity / 60.0f, 1.0f);
            float h1 = 125 + 45 * sinf(t * 0.5f) + 25 * sinf(t * 1.1f + 0.7f) + 10 * sinf(t * 2.3f);
            float h2 = 255 + 55 * sinf(t * 0.4f + 1.2f) + 20 * sinf(t * 0.9f + 2.0f) + 8 * sinf(t * 1.8f);
            float sat1 = 0.55f + 0.2f * sinf(t * 0.7f) + speedBoost * 0.15f;
            float sat2 = 0.5f + 0.2f * sinf(t * 0.6f + 1.0f) + speedBoost * 0.15f;
            headOuter = HSVtoRGB(h1, sat1, 0.95f);
            tailOuter = HSVtoRGB(h2, sat2, 0.9f);
            headInner = HSVtoRGB(180, 0.25f, 1.0f);
            tailInner = HSVtoRGB(fmodf(h2 + 30, 360), 0.2f, 0.98f);
            break;
        }
        case 10: {
            D2D1_COLOR_F ec = SnapshotCursorExtractedColor();
            headOuter = tailOuter = ec;
            headInner = tailInner = LighterColor(ec, 0.6f);
            break;
        }
        case 11: {
            const D2D1_COLOR_F ec = SnapshotCursorExtractedColor();
            D2D1_COLOR_F mixed = LerpColor(ec, g_customColor, 0.5f);
            D2D1_COLOR_F mixedTail = LerpColor(ec, GetGradColor(2), 0.5f);
            headOuter = mixed;
            tailOuter = mixedTail;
            headInner = LighterColor(mixed, 0.55f);
            tailInner = LighterColor(mixedTail, 0.55f);
            break;
        }
        case 12: {
            float speedBoost = fminf(velocity / 50.0f, 1.0f);
            float shine = 0.6f + 0.4f * sinf(t * (2.0f + speedBoost * 1.5f)) + 0.12f * sinf(t * (6.0f + speedBoost * 3.0f));
            float sweep = 0.5f + 0.5f * sinf(t * 1.2f);
            headOuter = D2D1::ColorF(1.0f * shine, 0.8f * shine, 0.08f * shine, 1);
            tailOuter = D2D1::ColorF(0.5f, 0.35f, 0.04f, 1);
            headInner = D2D1::ColorF(1.0f, 0.9f + sweep * 0.1f, 0.5f + sweep * 0.3f, 1);
            tailInner = D2D1::ColorF(0.8f, 0.6f, 0.2f, 1);
            break;
        }
        case 13: {
            float speedBoost = fminf(velocity / 50.0f, 1.0f);
            float pulse = 0.5f + 0.5f * sinf(t * (4.0f + speedBoost * 3.0f));
            float pulse2 = 0.5f + 0.5f * sinf(t * (4.0f + speedBoost * 3.0f) + 1.5f);
            float glitch = 0.9f + 0.1f * sinf(t * 30.0f) * (speedBoost > 0.5f ? 1.0f : 0.0f);
            headOuter = HSVtoRGB(285 + pulse * 35, 0.9f, glitch);
            tailOuter = HSVtoRGB(170 - pulse2 * 20, 0.9f, glitch * 0.95f);
            headInner = HSVtoRGB(310, 0.5f, 1.0f);
            tailInner = HSVtoRGB(160, 0.5f, 1.0f);
            break;
        }
        case 14: {
            float speedBoost = fminf(velocity / 80.0f, 1.0f);
            float h = fmodf(t * 15, 360);
            float sat = 0.25f + speedBoost * 0.15f;
            headOuter = HSVtoRGB(h, sat, 1.0f);
            tailOuter = HSVtoRGB(fmodf(h + 70, 360), sat, 0.98f);
            headInner = HSVtoRGB(h, sat * 0.5f, 1.0f);
            tailInner = HSVtoRGB(fmodf(h + 70, 360), sat * 0.5f, 1.0f);
            break;
        }
        case 15: {
            float speedBoost = fminf(velocity / 50.0f, 1.0f);
            float h, s, v;
            RGBtoHSV(g_customColor, h, s, v);
            float rotSpeed = 30 + speedBoost * 60;
            float h1 = fmodf(h + t * rotSpeed, 360);
            float h2 = fmodf(h1 + 160 + speedBoost * 40, 360);
            headOuter = HSVtoRGB(h1, s, v);
            tailOuter = HSVtoRGB(h2, s * 0.9f, v * 0.95f);
            headInner = HSVtoRGB(h1, s * 0.4f, v);
            tailInner = HSVtoRGB(h2, s * 0.35f, v * 0.95f);
            break;
        }
        case 16: {
            float speedBoost = fminf(velocity / 60.0f, 1.0f);
            float pulse = 0.5f + 0.5f * sinf(t * (2.5f + speedBoost * 2.0f));
            float smoothPulse = pulse * pulse * (3 - 2 * pulse);
            headOuter = LerpColor(GetGradColor(0), GetGradColor(2), smoothPulse);
            tailOuter = LerpColor(GetGradColor(2), GetGradColor(0), smoothPulse);
            headInner = LighterColor(headOuter, 0.45f);
            tailInner = LighterColor(tailOuter, 0.45f);
            break;
        }
        case 17: {
            float speedBoost = fminf(velocity / 60.0f, 1.0f);
            float sparkle = (rand() % 100) / 100.0f;
            float sparkle2 = (rand() % 100) / 100.0f;
            float baseH = fmodf(t * (30 + speedBoost * 30), 360);
            float bright1 = 0.4f + sparkle * 0.6f;
            float bright2 = 0.3f + sparkle2 * 0.5f;
            headOuter = HSVtoRGB(baseH, 0.8f, bright1);
            tailOuter = HSVtoRGB(fmodf(baseH + 100, 360), 0.8f, bright2);
            headInner = HSVtoRGB(baseH, 0.3f, 1.0f);
            tailInner = HSVtoRGB(fmodf(baseH + 100, 360), 0.3f, 1.0f);
            break;
        }
        case 18: {
            static float heatLag = 0.0f;
            float targetHeat = fminf(powf(velocity / 80.0f, 0.5f), 1.0f);
            heatLag += (targetHeat - heatLag) * 0.15f;
            float heat = fmaxf(targetHeat, heatLag * 0.8f);
            float heatTail = heat * 0.35f;
            auto thermal = [](float h) -> D2D1_COLOR_F {
                if (h < 0.2f) { float k = h / 0.2f; return D2D1::ColorF(0.02f, 0.05f, 0.35f + k * 0.35f, 1); }
                else if (h < 0.45f) { float k = (h - 0.2f) / 0.25f; return D2D1::ColorF(0.05f + k * 0.55f, 0.1f, 0.7f - k * 0.45f, 1); }
                else if (h < 0.7f) { float k = (h - 0.45f) / 0.25f; return D2D1::ColorF(0.6f + k * 0.35f, 0.1f + k * 0.5f, 0.25f - k * 0.15f, 1); }
                else { float k = (h - 0.7f) / 0.3f; return D2D1::ColorF(0.95f, 0.6f + k * 0.35f, 0.1f + k * 0.85f, 1); }
            };
            headOuter = thermal(heat);
            tailOuter = thermal(heatTail);
            headInner = LighterColor(headOuter, 0.5f);
            tailInner = LighterColor(tailOuter, 0.5f);
            break;
        }
        case 19: {
            float speedBoost = fminf(velocity / 50.0f, 1.0f);
            float wave1 = sinf(t * (2.0f + speedBoost * 2.0f)) * 0.5f + 0.5f;
            float wave2 = sinf(t * (3.3f + speedBoost * 2.5f) + 1.2f) * 0.5f + 0.5f;
            float wave3 = sinf(t * (5.0f + speedBoost * 3.0f) + 2.5f) * 0.5f + 0.5f;
            float interfere = wave1 * wave2 + (1 - wave1) * (1 - wave2);
            interfere = interfere * 0.7f + wave3 * 0.3f;
            float h1 = fmodf(interfere * 360 + t * (15 + speedBoost * 25), 360);
            float h2 = fmodf(h1 + 130 + sinf(t * 1.5f) * 50, 360);
            float sat = 0.55f + interfere * 0.4f;
            headOuter = HSVtoRGB(h1, sat, 1.0f);
            tailOuter = HSVtoRGB(h2, sat, 0.9f);
            headInner = HSVtoRGB(h1, sat * 0.35f, 1.0f);
            tailInner = HSVtoRGB(h2, sat * 0.3f, 0.95f);
            break;
        }
        case 20: {
            float speedBoost = fminf(velocity / 40.0f, 1.0f);
            float h, s, v;
            RGBtoHSV(g_customColor, h, s, v);
            float split = speedBoost * 200.0f;
            float wobble = sinf(t * (1.5f + speedBoost)) * (20.0f + speedBoost * 20.0f);
            float satBoost = 1.0f + speedBoost * 0.3f;
            headOuter = HSVtoRGB(fmodf(h + split * 0.5f + wobble + 360, 360), fminf(s * satBoost, 1.0f), v);
            tailOuter = HSVtoRGB(fmodf(h - split * 0.5f - wobble + 360, 360), fminf(s * satBoost * 0.9f, 1.0f), v * 0.9f);
            headInner = HSVtoRGB(fmodf(h + split * 0.5f + wobble + 360, 360), s * 0.35f, v);
            tailInner = HSVtoRGB(fmodf(h - split * 0.5f - wobble + 360, 360), s * 0.3f, v * 0.9f);
            break;
        }
        case 21: {
            float speedBoost = fminf(velocity / 40.0f, 1.0f);
            float jitterAmp = (0.2f + speedBoost * 0.4f);
            float baseH, baseS, baseV;
            RGBtoHSV(g_customColor, baseH, baseS, baseV);
            float timeSeed = fmodf(t * 10.0f, 100.0f);
            float n1 = (sinf(timeSeed * 12.9898f + 78.233f) * 43758.5453f); n1 = n1 - floorf(n1);
            float n2 = (sinf(timeSeed * 39.3468f + 11.135f) * 24634.6345f); n2 = n2 - floorf(n2);
            float n3 = (sinf(timeSeed * 27.1453f + 45.854f) * 34634.6345f); n3 = n3 - floorf(n3);
            n1 = (n1 - 0.5f) * 2 * jitterAmp;
            n2 = (n2 - 0.5f) * 2 * jitterAmp;
            n3 = (n3 - 0.5f) * 2 * jitterAmp * 0.6f;
            headOuter = HSVtoRGB(fmodf(baseH + n1 * 80 + 360, 360), fminf(fmaxf(baseS + n2, 0), 1), fminf(fmaxf(baseV + n3, 0.15f), 1));
            tailOuter = HSVtoRGB(fmodf(baseH + 80 + n2 * 80 + 360, 360), fminf(fmaxf(baseS + n1, 0), 1), fminf(fmaxf(baseV * 0.8f + n3, 0.15f), 1));
            headInner = LighterColor(headOuter, 0.45f);
            tailInner = LighterColor(tailOuter, 0.45f);
            break;
        }
        case 22: {
            headOuter = GetGradColor(0);
            tailOuter = GetGradColor(g_gradColorCount - 1);
            headInner = LighterColor(GetGradColor(0));
            tailInner = LighterColor(GetGradColor(g_gradColorCount - 1));
            break;
        }
        case 23: {
            float speedBoost = fminf(velocity / 40.0f, 1.0f);
            float d = t * (0.35f + speedBoost * 0.55f);
            float a1 = 0.5f + 0.5f * sinf(d * 2.1f);
            float a2 = 0.5f + 0.5f * sinf(d * 3.7f + 1.7f);
            float a3 = 0.5f + 0.5f * sinf(d * 1.3f + 3.1f);
            float hHead = fmodf(178.0f + a1 * 150.0f + a2 * 62.0f, 360.0f);
            float hTail = fmodf(298.0f + a2 * 95.0f + a3 * 72.0f, 360.0f);
            float sheen = fminf(0.82f + 0.18f * a3, 1.0f);
            headOuter = HSVtoRGB(hHead, 0.88f, sheen);
            tailOuter = HSVtoRGB(hTail, 0.82f, fminf(sheen * 0.92f, 1.0f));
            headInner = HSVtoRGB(fmodf(hHead + 38.0f, 360.0f), 0.42f, 1.0f);
            tailInner = HSVtoRGB(fmodf(hTail + 38.0f, 360.0f), 0.36f, 0.98f);
            break;
        }
        case 24: {
            float speedBoost = fminf(velocity / 50.0f, 1.0f);
            float w = sinf(t * (0.5f + speedBoost * 0.6f));
            float hHead = fmodf(302.0f + w * 44.0f, 360.0f);
            float hTail = fmodf(188.0f - w * 26.0f + 360.0f, 360.0f);
            float glow = fminf(0.9f + 0.1f * sinf(t * 4.0f), 1.0f);
            headOuter = HSVtoRGB(hHead, 0.86f, glow);
            tailOuter = HSVtoRGB(hTail, 0.80f, fminf(glow * 0.95f, 1.0f));
            headInner = HSVtoRGB(fmodf(hHead + 22.0f, 360.0f), 0.45f, 1.0f);
            tailInner = HSVtoRGB(fmodf(hTail - 18.0f + 360.0f, 360.0f), 0.40f, 0.98f);
            break;
        }
        case 25: {
            float speedBoost = fminf(velocity / 45.0f, 1.0f);
            float breathe = 0.5f + 0.5f * sinf(t * 1.6f);
            float pulse = 0.5f + 0.5f * sinf(t * (2.2f + speedBoost * 3.0f));
            float lum = fminf((0.5f + 0.5f * breathe) * (0.72f + 0.28f * pulse + speedBoost * 0.18f), 1.0f);
            float hHead = 168.0f + sinf(t * 0.7f) * 22.0f;
            float hTail = 194.0f + sinf(t * 0.5f + 2.0f) * 18.0f;
            headOuter = HSVtoRGB(hHead, 0.95f, lum);
            tailOuter = HSVtoRGB(hTail, 0.90f, fminf(lum * 0.85f, 1.0f));
            headInner = HSVtoRGB(hHead, 0.34f, 1.0f);
            tailInner = HSVtoRGB(hTail, 0.30f, 0.98f);
            break;
        }
        case 26: {
            SYSTEMTIME st;
            GetLocalTime(&st);
            float hour = (float)st.wHour + (float)st.wMinute / 60.0f + (float)st.wSecond / 3600.0f;
            static const float kT[7] = {0.0f, 5.5f, 8.0f, 12.0f, 17.0f, 19.5f, 24.0f};
            static const float kH[7] = {232.0f, 22.0f, 42.0f, 205.0f, 202.0f, 18.0f, 232.0f};
            static const float kS[7] = {0.60f, 0.58f, 0.28f, 0.62f, 0.58f, 0.72f, 0.60f};
            static const float kV[7] = {0.38f, 0.82f, 0.98f, 1.00f, 0.96f, 0.84f, 0.38f};
            int segIdx = 0;
            for (int i = 0; i < 6; i++)
                if (hour >= kT[i]) segIdx = i;
            float span = kT[segIdx + 1] - kT[segIdx];
            float f = (span > 1e-3f) ? (hour - kT[segIdx]) / span : 0.0f;
            f = fminf(fmaxf(f, 0.0f), 1.0f);
            f = f * f * (3.0f - 2.0f * f);
            float dh = kH[segIdx + 1] - kH[segIdx];
            if (dh > 180.0f) dh -= 360.0f;
            else if (dh < -180.0f) dh += 360.0f;
            float h = kH[segIdx] + dh * f;
            if (h < 0.0f) h += 360.0f;
            else if (h >= 360.0f) h -= 360.0f;
            float s = kS[segIdx] + (kS[segIdx + 1] - kS[segIdx]) * f;
            float v = kV[segIdx] + (kV[segIdx + 1] - kV[segIdx]) * f;
            headOuter = HSVtoRGB(h, s, v);
            tailOuter = HSVtoRGB(fmodf(h + 26.0f, 360.0f), fminf(s * 0.92f, 1.0f), fminf(v * 0.88f, 1.0f));
            headInner = HSVtoRGB(fmodf(h + 12.0f, 360.0f), fminf(s * 0.45f, 1.0f), 1.0f);
            tailInner = HSVtoRGB(fmodf(h + 38.0f, 360.0f), fminf(s * 0.40f, 1.0f), 0.98f);
            break;
        }
        default:
            headOuter = tailOuter = kColorBlack;
            headInner = tailInner = kColorWhite;
            break;
    }
    out.solidOuter = headOuter;
    out.solidInner = headInner;
    if (g_contrastTarget > 0) {
        const float target = mtcolor::TargetRatioOf(static_cast<mtcolor::ContrastLevel>(g_contrastTarget));
        const D2D1_COLOR_F bg = mtcolor::SetLuma(kColorWhite, g_bgLuminance.load(std::memory_order_relaxed));
        headOuter = mtcolor::EnsureContrast(headOuter, bg, target);
        tailOuter = mtcolor::EnsureContrast(tailOuter, bg, target);
        headInner = mtcolor::EnsureContrast(headInner, bg, target);
        tailInner = mtcolor::EnsureContrast(tailInner, bg, target);
    }
    const float flowPhase1 = (mode == 1) ? fmodf(t * g_gradientFlowSpeed * 1.5f, 1.0f) : 0.0f;
    for (int i = 0; i < GRAD_STOPS; i++) {
        float ratio = (float)i / (GRAD_STOPS - 1);
        float alpha = 0.86f * powf(1.0f - ratio, 1.4f);
        if (mode == 7) {
            float phase = fmodf(ratio * 4.0f + t * 2.0f, 1.0f);
            bool stripe = phase < 0.5f;
            SetGradStopPair(out, i, ratio, stripe ? headOuter : tailOuter, stripe ? headInner : tailInner, alpha);
        } else if (mode == 8) {
            float fr = ratio * ratio;
            SetGradStopPair(out, i, ratio, LerpColor(headOuter, tailOuter, fr), LerpColor(headInner, tailInner, fr), alpha);
        } else if (mode == 1) {
            D2D1_COLOR_F co = SampleGradientLUT(ratio, flowPhase1);
            SetGradStopPair(out, i, ratio, co, LighterColor(co, 0.45f), alpha);
        } else if (mode == 22) {
            float warp = 0.5f + 0.4f * sinf(t * 1.5f + ratio * 4.0f) + 0.2f * sinf(t * 3.7f + ratio * 7.0f + 0.8f);
            float phase = fmodf(t * g_gradientFlowSpeed + warp * 0.5f, 1.0f);
            D2D1_COLOR_F co = SampleGradientLUT(ratio, phase);
            SetGradStopPair(out, i, ratio, co, LighterColor(co, 0.45f), alpha);
        } else {
            SetGradStopPair(out, i, ratio, LerpColor(headOuter, tailOuter, ratio), LerpColor(headInner, tailInner, ratio), alpha);
        }
    }
}
static void ReleaseGradientBrushes() {
    if (g_pGradInnerBrush) {
        g_pGradInnerBrush->Release();
        g_pGradInnerBrush = nullptr;
    }
    if (g_pGradOuterBrush) {
        g_pGradOuterBrush->Release();
        g_pGradOuterBrush = nullptr;
    }
    if (g_pGradInnerStops) {
        g_pGradInnerStops->Release();
        g_pGradInnerStops = nullptr;
    }
    if (g_pGradOuterStops) {
        g_pGradOuterStops->Release();
        g_pGradOuterStops = nullptr;
    }
    s_gradValid = false;
}
static void SetGradientBrushEndpoints(ID2D1LinearGradientBrush *brush, D2D1_POINT_2F headPt, D2D1_POINT_2F tailPt) {
    if (!brush)
        return;
    brush->SetStartPoint(headPt);
    brush->SetEndPoint(tailPt);
}
static void UpdateColorBrushes(const GradData &data, D2D1_POINT_2F headPt, D2D1_POINT_2F tailPt) {
    if (g_pSolidOuterBrush)
        g_pSolidOuterBrush->SetColor(data.solidOuter);
    if (g_pSolidInnerBrush)
        g_pSolidInnerBrush->SetColor(data.solidInner);
    bool needRebuild = !s_gradValid || !GradApproxEq(s_cachedGrad, data);
    if (needRebuild && g_pD2DDC) {
        ReleaseGradientBrushes();
        g_pD2DDC->CreateGradientStopCollection(data.outer, GRAD_STOPS, &g_pGradOuterStops);
        g_pD2DDC->CreateGradientStopCollection(data.inner, GRAD_STOPS, &g_pGradInnerStops);
        g_pD2DDC->CreateLinearGradientBrush(D2D1::LinearGradientBrushProperties(headPt, tailPt), g_pGradOuterStops,
                                            &g_pGradOuterBrush);
        g_pD2DDC->CreateLinearGradientBrush(D2D1::LinearGradientBrushProperties(headPt, tailPt), g_pGradInnerStops,
                                            &g_pGradInnerBrush);
        s_cachedGrad = data;
        s_gradValid = true;
    }
    SetGradientBrushEndpoints(g_pGradOuterBrush, headPt, tailPt);
    if (g_pGradInnerBrush) {
        g_pGradInnerBrush->SetStartPoint(headPt);
        g_pGradInnerBrush->SetEndPoint(tailPt);
    }
}
#define MT_HLSL_CONSTANT_BUFFER R"(
cbuffer ConstantBuffer : register(b0) {
    float2 screenSize;
    float perspective;
    float pad0;
    float4 gradient[16];
    int gradientCount;
    float bgLuminance;
    float adaptiveFlag;
    float smoothFlag;
    float edgeSoftness;
    // 注意：这两个槽位是 vs 的 padding，同时被 ps / 粒子着色器当作图集尺寸使用。/ Note: these two slots are vs padding, also used as atlas dimensions by the ps / particle shaders.
    // 同一批字节两种解读是有意为之（省一个寄存器），但改名必须同时考虑两套语义。/ two readings of the same bytes is intentional (saves a register), but renaming must consider both semantics.
    float atlasRows;
    float atlasCols;
    float aaMode;
    float textAspect;
    float strokeStepU;
    float strokeStepV;
    float strokeEnabled;
    float strokeAlphaMul;
    float strokeColorMode;
    float strokeColorR;
    float strokeColorG;
    float strokeColorB;
    float strokeColorMul;
    float strokeWidthPx;
    float trailStrokeEnabled;
    float trailStrokeHalf;
    float shaderQuality;
    float sdfSamples;
    float ditherMode;
    float ditherStrength;
    float pixelScale;
    float noiseSeed;
    float pad3;
    float4 padTail[4];
};
)"
#define MT_HLSL_COLOR_COMMON R"(
static const float3 LUMA709 = float3(0.2126, 0.7152, 0.0722);

float Luma709(float3 c) { return dot(c, LUMA709); }

// 保持色相与彩度地改写感知明度 / rewrite perceived luma keeping hue and chroma
// ⚠ 存在色域天花板：最大通道顶到 1.0 就无法再提亮，等比回压会把颜色原样送回。/ ⚠ gamut ceiling: once the largest channel hits 1.0 it cannot brighten further, and uniform rollback sends the colour back unchanged.
float3 SetLuma(float3 c, float target) {
    float y = Luma709(c);
    if (y < 1e-4) return float3(target, target, target);
    float3 o = c * (target / y);
    float m = max(o.r, max(o.g, o.b));
    return (m > 1.0) ? (o / m) : o;
}

// 亮度导向统一入口：先走保色路径，撞到色域天花板后按缺口朝锚点混合补齐。/ unified luma entry point: take the colour-preserving path first, then fill the gap by blending toward an anchor at the ceiling.
// 朝白/黑混合只降饱和度、不动色相，因此是给定亮度目标下损失最小的补齐方式。/ blending toward white/black only lowers saturation without touching hue, so it is the least-lossy fill under a fixed luma target.
float3 LumaToward(float3 c, float target) {
    target = saturate(target);
    float3 o = SetLuma(c, target);
    float y1 = Luma709(o);
    if (abs(y1 - target) <= 1e-4) return o;
    float3 anchor = (target > y1) ? float3(1.0, 1.0, 1.0) : float3(0.0, 0.0, 0.0);
    float anchorY = Luma709(anchor);
    float denom = (target > y1) ? (anchorY - y1) : (y1 - anchorY);
    if (denom < 1e-5) return o;
    return lerp(o, anchor, saturate((target - y1) / denom));
}

// 自适应对比度强度常数。此前是散落的字面量 0.30 / 0.42 / 0.55，看不出含义也无从调优。/ adaptive-contrast strength constants; previously scattered literals 0.30 / 0.42 / 0.55 with no meaning and no tuning handle.
// 提亮、压暗、描边加深各自的上限集中在这里命名。/ the caps for lift, dim and stroke-deepen are named and gathered here.
static const float kAdaptiveLiftOnDark   = 0.30;  // 暗背景最大提亮比例 / max lift ratio on dark backgrounds
static const float kAdaptiveDimOnBright  = 0.42;  // 亮背景最大压暗比例 / max dim ratio on bright backgrounds
static const float kAdaptiveStrokeDeepen = 0.55;  // 亮背景描边最大加深比例 / max stroke-deepen ratio on bright backgrounds

// 自适应对比度 v2：保持色相/彩度的明度调整，CPU 侧对应 mtcolor::EnsureContrast 的诉求。/ adaptive contrast v2: hue/chroma-preserving luma adjustment, matching the intent of mtcolor::EnsureContrast on the CPU side.
// strokeActive=1 表示已开描边 —— 此时主体保持原色，由描边承担轮廓对比。/ strokeActive=1 means stroking is on - the body keeps its original colour and the stroke provides outline contrast.
float3 ApplyAdaptiveContrast(float3 rgb, float bgLuminance, float strokeActive) {
    float bgDelta  = bgLuminance - 0.5;
    float isBright = step(0.0, bgDelta);
    // 强度曲线必须是凸的：sqrt 这类凹曲线会放大中间调（bg≈0.7 时反而压得比线性更狠）。/ the strength curve must be convex: concave curves like sqrt over-amplify midtones (at bg≈0.7 it dims harder than linear).
    // k² 让中间调几乎不干预，只在接近纯白/纯黑背景时才发力。/ k-squared leaves midtones nearly untouched and only acts near pure-white/black backgrounds.
    float k = abs(bgDelta) * 2.0;
    float t = k * k;
    float y = Luma709(rgb);
    // 用 LumaToward 而非 SetLuma：后者碰到 channel=1.0 的亮饱和色（如默认 #00BFFF）会原地返回，/ use LumaToward instead of SetLuma: the latter returns unchanged on bright saturated colours with a channel at 1.0 (e.g. default #00BFFF),
    // 暗背景提亮等于失效。 Has gamut fallback; plain SetLuma would silently no-op on such colours. / so lifting on dark backgrounds would silently fail; LumaToward has a gamut fallback.
    float3 lit3 = LumaToward(rgb, y + (1.0 - y) * t * kAdaptiveLiftOnDark);
    float3 dim3 = SetLuma(rgb, y * (1.0 - t * kAdaptiveDimOnBright));
    float3 dimAdj = lerp(dim3, rgb, strokeActive);
    return lerp(lit3, dimAdj, isBright);
}

// 亮背景改为加深描边来提供对比（替代 v1 对主体的整体压暗），主体颜色得以保留。/ bright backgrounds now deepen the stroke for contrast (replacing v1's whole-body dimming), keeping the body colour.
float3 ApplyAdaptiveStroke(float3 strokeRgb, float bgLuminance) {
    float ks = saturate(bgLuminance - 0.5) * 2.0;
    float ts = ks * ks;
    return SetLuma(strokeRgb, Luma709(strokeRgb) * (1.0 - ts * kAdaptiveStrokeDeepen));
}

// ---- 输出抖动：把 8bit 量化误差打散成有序/随机图案，消除渐变色带 ---- / output dithering: spreads 8-bit quantisation error into ordered/random patterns to remove gradient banding ----
// 此前 Bayer2/4/8、HashNoise2D、ApplyDither 在 g_psShader 与 g_particlePS 里各写一遍，/ Bayer2/4/8, HashNoise2D and ApplyDither used to be duplicated in g_psShader and g_particlePS,
// 与 SetLuma 属于同一类重复。/ the same class of duplication as SetLuma.
float Bayer2(float x, float y) {
    return (x < 0.5) ? ((y < 0.5) ? 0.0 : 3.0) : ((y < 0.5) ? 2.0 : 1.0);
}
float Bayer4(float2 ip) {
    float2 lo = fmod(ip, 2.0);
    float2 hi = fmod(floor(ip * 0.5), 2.0);
    return 4.0 * Bayer2(lo.x, lo.y) + Bayer2(hi.x, hi.y);
}
float Bayer8(float2 ip) {
    float2 lo = fmod(ip, 4.0);
    float2 hi = fmod(floor(ip * 0.25), 2.0);
    return 4.0 * Bayer4(lo) + Bayer2(hi.x, hi.y);
}
float HashNoise2D(float2 p, float seed) {
    float3 p3 = frac(float3(p.xyx) * 0.1031);
    p3 += dot(p3, p3.yzx + 33.33 + seed);
    return frac((p3.x + p3.y) * p3.z) * 2.0 - 1.0;
}
float3 ApplyDither(float3 rgb, float2 screenPos, float mode, float strength, float pixelScale, float seed) {
    if (mode < 0.5 || pixelScale <= 0.0) return rgb;
    float n = 0.0;
    if (mode < 1.5)      n = Bayer4(floor(screenPos)) * 0.0625;   // /16
    else if (mode < 2.5) n = Bayer8(floor(screenPos)) * 0.015625; // /64
    else                 n = HashNoise2D(floor(screenPos), seed);
    return rgb + (n - 0.5) * 2.0 * strength * pixelScale;
}
)"
static const char* g_vsShader = MT_HLSL_CONSTANT_BUFFER
R"(

struct VS_INPUT {
    float3 pos : POSITION;  // x, y, z(深度) / x, y, z (depth)
    float4 color : COLOR;
    float u : TEXCOORD0;  // 沿路径比例 0~1 / ratio along the path 0~1
    float v : TEXCOORD1;  // 垂直路径方向 -1~1 / cross-path direction -1~1
};

struct VS_OUTPUT {
    float4 pos : SV_POSITION;
    float4 color : COLOR;
    float u : TEXCOORD0;
    float v : TEXCOORD1;
    float depth : TEXCOORD2;
};

VS_OUTPUT VSMain(VS_INPUT input) {
    VS_OUTPUT output;
    // 2.5D 透视：根据深度 z 相对于屏幕中心缩放 / 2.5D perspective: scale by depth z relative to the screen centre
    float scale = 1.0 + input.pos.z * perspective;
    float2 center = screenSize * 0.5;
    float2 screenPos = center + (input.pos.xy - center) * scale;
    // 屏幕坐标 → 裁剪空间 / screen coordinates -> clip space
    float2 ndc = float2(
        (screenPos.x / screenSize.x) * 2.0 - 1.0,
        1.0 - (screenPos.y / screenSize.y) * 2.0
    );
    output.pos = float4(ndc, 0.0, 1.0);
    output.color = input.color;
    output.u = input.u;
    output.v = input.v;
    output.depth = input.pos.z;
    return output;
}
)";
static const char* g_psShader = MT_HLSL_CONSTANT_BUFFER
MT_HLSL_COLOR_COMMON
R"(

struct PS_INPUT {
    float4 pos : SV_POSITION;
    float4 color : COLOR;
    float u : TEXCOORD0;
    float v : TEXCOORD1;
    float depth : TEXCOORD2;
};

float4 PSMain(PS_INPUT input) : SV_TARGET {
    // 抗锯齿边缘：根据aaMode调整边缘过渡宽度 / anti-aliased edge: transition width driven by aaMode
    float absV = abs(input.v);
    float edgeFade;
    if (aaMode < 0.5) {
        // off：硬边 / off: hard edge
        edgeFade = (absV < 1.0) ? 1.0 : 0.0;
    } else {
        // 边缘宽度倍率：smooth=2.0 crisp=1.0 extra=4.0（单位：像素）/ width multiplier in pixels
        float k = (aaMode < 1.5) ? 2.0 : ((aaMode < 2.5) ? 1.0 : 4.0);
        float inner;
        if (shaderQuality > 0.5) {
            // 屏幕空间导数：过渡带恒为 k 像素宽，与拖尾粗细无关（细拖尾不再糊、粗拖尾不再硬）/ screen-space derivatives: the transition is a constant k pixels wide regardless of trail thickness (thin trails no longer blurry, thick ones no longer hard)
            float px = max(fwidth(input.v), 1e-5);
            inner = 1.0 - px * k * (0.35 + edgeSoftness * 0.9);
        } else {
            // 快速路径：v 空间固定宽度（v3.4.2 行为）/ Fast path: fixed width in v space
            float kv = (aaMode < 1.5) ? 0.5 : ((aaMode < 2.5) ? 0.15 : 0.8);
            inner = 1.0 - edgeSoftness * kv;
        }
        inner = min(inner, 0.995);  // 防止 smoothstep 退化 / prevents smoothstep degeneracy
        edgeFade = smoothstep(1.0, inner, absV);
    }
    // Early discard: 完全在边缘外的像素直接丢弃 / Early discard: pixels fully outside the edge are dropped outright
    if (edgeFade <= 0.001) discard;
    // 渐变采样：GPU硬件插值，无断层 / gradient sampling: GPU hardware interpolation, no stepping
    float4 gradColor = input.color;
    float3 tint = float3(1, 1, 1);  // 单色模式：顶点颜色即最终颜色 / solid mode: vertex colour is the final colour
    if (gradientCount > 1) {
        float t = input.u * (gradientCount - 1);
        int idx = (int)t;
        float frac = t - idx;
        if (idx >= gradientCount - 1) {
            gradColor = gradient[gradientCount - 1];
        } else if (smoothFlag > 0.5) {
            // smoothstep 过渡比线性lerp更自然 / smoothstep transitions look more natural than linear lerp
            float s = frac * frac * (3.0 - 2.0 * frac);
            gradColor = lerp(gradient[idx], gradient[idx + 1], s);
        } else {
            gradColor = gradient[idx];  // 阶梯色 / stepped colour
        }
        gradColor.a *= input.color.a;
        tint = input.color.rgb;  // 渐变模式：顶点RGB作为亮度调制 / gradient mode: vertex RGB modulates luma
    }
    // 合并圆柱光照 + 2.5D深度光照：中心(v=0)最亮，深度越大越亮 / combined cylindrical + 2.5D depth lighting: brightest at the centre (v=0), brighter with more depth
    float tubeLight = (0.88 + 0.12 * (1.0 - absV)) * (1.0 + input.depth * 0.3);
    float3 rgb = gradColor.rgb * tint * tubeLight;
    // 自适应对比度 v2：保持色相的明度调整 / adaptive contrast v2: hue-preserving luma adjustment
    // v1 的三个问题：① 直接线性乘 RGB，高饱和色被压脏 ② 亮背景最极端时压到原亮度的 30% / v1's three problems: 1) direct linear RGB scaling dirtied saturated colours 2) at the extreme bright background it dimmed to 30% of original luma
    // ③ 中间调（bgDelta≈0.1~0.3）就开始大幅改写颜色。/ 3) it heavily rewrote colours even at midtones (bgDelta ~0.1~0.3).
    // v2：保色；soft-knee 让中间调几乎不干预；最大压暗量 0.70 → 0.42；/ v2: colour-preserving; soft-knee leaves midtones almost untouched; max dimming 0.70 -> 0.42;
    // 并且开启描边时不再压暗主体 —— 描边已提供轮廓对比，改由描边承担（见下方描边加深）。/ and with stroking on the body is no longer dimmed - the stroke already provides outline contrast (see stroke deepening below).
    // 实现已收敛到 MT_HLSL_COLOR_COMMON 里的 ApplyAdaptiveContrast()，与粒子着色器共用同一份。/ the implementation has converged into ApplyAdaptiveContrast() in MT_HLSL_COLOR_COMMON, shared with the particle shader.
    // v1 issues: it muddied saturated hues, could dim all the way down to 30%, and reacted too strongly
    // to midtones. v2 keeps chroma, spares midtones (max darken 0.70 -> 0.42), and leaves the body alone
    // when a stroke is enabled so that the stroke supplies the contrast instead. The implementation now
    // lives in ApplyAdaptiveContrast() inside MT_HLSL_COLOR_COMMON, shared with the particle shader.
    if (adaptiveFlag > 0.5) {
        rgb = ApplyAdaptiveContrast(rgb, bgLuminance, trailStrokeEnabled);
    }
    // 拖尾描边：边缘环带（|v| 接近 1）用描边色，与主体平滑叠加 / trail stroke: the edge band (|v| near 1) uses the stroke colour, blended smoothly with the body
    if (trailStrokeEnabled > 0.5) {
        float half = trailStrokeHalf;
        float strokeEdge = 0.05;
        float strokeBand = smoothstep(1.0 - half - strokeEdge, 1.0 - half + strokeEdge, absV);
        float3 strokeRgb = (strokeColorMode > 0.5) ? float3(strokeColorR, strokeColorG, strokeColorB) : rgb * strokeColorMul;
        // 自适应对比度下，亮背景改为加深描边来提供对比（替代 v1 对主体的整体压暗）：/ under adaptive contrast, bright backgrounds deepen the stroke instead of dimming the whole body (v1's approach):
        // 主体颜色得以保留，轮廓反而更清晰。/ the body keeps its colour and the outline is sharper.
        if (adaptiveFlag > 0.5) {
            // 描边加深同样用凸曲线，只在浅色背景发力 / stroke deepening also uses a convex curve, acting only on light backgrounds
            strokeRgb = ApplyAdaptiveStroke(strokeRgb, bgLuminance);
        }
        rgb = lerp(rgb, strokeRgb, strokeBand * strokeAlphaMul);
    }
    // 输出非预乘alpha：混合状态SRC_ALPHA/INV_SRC_ALPHA会自动完成预乘 / output is non-premultiplied alpha: the SRC_ALPHA/INV_SRC_ALPHA blend state premultiplies automatically
    float alpha = gradColor.a * edgeFade;
    rgb = ApplyDither(rgb, input.pos.xy, ditherMode, ditherStrength, pixelScale, noiseSeed);
    return float4(rgb, alpha);
}
)";
static const char* g_particleVS = MT_HLSL_CONSTANT_BUFFER
R"(

struct VS_INPUT {
    float2 quadPos : POSITION;
    float3 instancePos : TEXCOORD0; // x, y, z
    float4 instanceColor : TEXCOORD1;
    float instanceSize : TEXCOORD2;
    float instanceShape : TEXCOORD3;
    float instanceRot : TEXCOORD4;
    float instanceCharIdx : TEXCOORD5;
};

struct VS_OUTPUT {
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
    float4 color : COLOR;
    float depth : TEXCOORD1;
    float shape : TEXCOORD2;
    float size : TEXCOORD3;  // 传递大小给PS用于自适应软边缘 / size passed to the PS for adaptive soft edges
    float charIdx : TEXCOORD4;
};

VS_OUTPUT VSMain(VS_INPUT input) {
    VS_OUTPUT output;
    // 2.5D 透视缩放 / 2.5D perspective scaling
    float scale = 1.0 + input.instancePos.z * perspective;
    float finalSize = input.instanceSize * scale;  // 预计算最终大小 / precomputed final size
    // 文字粒子：按宽高比拉伸 X，避免长句被压缩成正方形 / text particles: stretch X by the aspect ratio so long phrases are not squeezed into squares
    float textScaleX = (input.instanceShape > 9.5) ? textAspect : 1.0;
    float2 stretchedPos = float2(input.quadPos.x * textScaleX, input.quadPos.y);
    // 自旋转：只旋转顶点位置，不旋转uv（同时旋转会抵消）/ Self-rotation: rotate vertices only, not uv
    float cosR = cos(input.instanceRot);
    float sinR = sin(input.instanceRot);
    float2 rotatedPos = float2(
        stretchedPos.x * cosR - stretchedPos.y * sinR,
        stretchedPos.x * sinR + stretchedPos.y * cosR
    );
    float2 worldPos = input.instancePos.xy + rotatedPos * finalSize;
    // 屏幕坐标 → NDC（fused multiply-add）/ Screen -> NDC (fused multiply-add)
    float2 ndc = float2(
        worldPos.x / screenSize.x * 2.0 - 1.0,
        1.0 - worldPos.y / screenSize.y * 2.0
    );
    output.pos = float4(ndc, 0.0, 1.0);
    output.uv = input.quadPos * 0.5 + 0.5;  // uv不旋转，形状遮罩随四边形旋转 / uv stays unrotated; the shape mask rotates with the quad
    output.color = input.instanceColor;
    output.depth = input.instancePos.z;
    output.shape = input.instanceShape;
    output.size = finalSize;  // 透视后的实际大小 / actual size after perspective
    output.charIdx = input.instanceCharIdx;
    return output;
}
)";
static const char* g_particlePS = MT_HLSL_CONSTANT_BUFFER
MT_HLSL_COLOR_COMMON
R"(

Texture2D charAtlasTex : register(t0);
SamplerState charAtlasSampler : register(s0);

struct PS_INPUT {
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
    float4 color : COLOR;
    float depth : TEXCOORD1;
    float shape : TEXCOORD2;
    float size : TEXCOORD3;
    float charIdx : TEXCOORD4;
};

// 形状SDF：接收预计算的p和r，避免重复计算 / shape SDF: takes precomputed p and r to avoid recomputation
// 分支顺序按「是否需要 atan2」分组：先用一次区间测试把 心形/菱形/三角形 分流出去，/ branches are grouped by whether atan2 is needed: one range test first diverts heart/diamond/triangle,
// 让它们不落在 atan2 所在的公共块里（原来的 if-else 链会把 atan2 放在所有形状之前，/ keeping them out of the common atan2 block (the old if-else chain placed atan2 before every shape,
// 这三个形状白白每样本多算 ~23 条指令；SDF 多采样时会放大 2~4 倍）。/ wasting ~23 instructions per sample for these three; SDF multi-sampling amplifies that 2-4x).
// Branches are grouped by whether atan2 is needed: one range test routes heart/diamond/triangle
// away before the atan2 block, so they no longer pay for it per sample (the old chain computed
// atan2 for every non-circle shape, wasting ~23 instructions per sample, ×N with multi-sampling).
float shapeSDF(float2 p, float r, float shapeType) {
    // 圆形快速路径：最常用形状，跳过atan2和分支 / circle fast path: the most common shape, skips atan2 and branching
    if (shapeType < 0.5) {
        return 0.5 - r;
    }
    // 无需极坐标角度的形状组 / shapes that need no polar angle
    // 上界必须是 5.5（覆盖 3=心形 / 4=菱形 / 5=三角形）。/ the upper bound must be 5.5 (covers 3=heart / 4=diamond / 5=triangle).
    // 写成 6.5 会把 6=花形 吞进本组并落到下面的三角形兜底分支（曾造成花朵渲染成三角形）。/ writing 6.5 would swallow 6=flower into this group, landing it in the triangle fallback below (once caused flowers to render as triangles).
    if (shapeType >= 2.5 && shapeType < 5.5) {
        if (shapeType < 3.5) {
            // 心形：复用p*2，用x*x*x替代pow避免负数底数UB / heart: reuses p*2; uses x*x*x instead of pow to avoid negative-base UB
            float2 hp = p * 2.0;
            hp.y = -hp.y;
            float t = hp.x*hp.x + hp.y*hp.y - 1.0;
            float heart = t * t * t - hp.x*hp.x * hp.y*hp.y*hp.y;
            return -heart * 0.12;
        }
        if (shapeType < 4.5) {
            return 0.5 - (abs(p.x) + abs(p.y));                    // 菱形 / diamond
        }
        // 等边三角形（顶点在上）/ Equilateral triangle (point up)
        float2 tp = p * 2.0;
        tp.y = -tp.y;
        return min(tp.y + 0.5, min(1.0 - tp.y - 1.732 * tp.x, 1.0 - tp.y + 1.732 * tp.x)) * 0.35;
    }
    // 需要极坐标角度的形状组 / shapes needing a polar angle
    float a = atan2(p.y, p.x);
    if (shapeType < 1.5) {
        return 0.5 * (0.4 + 0.6 * abs(cos(a * 2.5))) - r;          // 星形 / star
    }
    if (shapeType < 2.5) {
        return 0.5 * (0.5 + 0.5 * abs(cos(a * 3.0))) - r;          // 六角星 / hexagram
    }
    if (shapeType < 6.5) {
        return 0.5 * (0.6 + 0.4 * cos(a * 5.0)) - r;               // 花形 / flower
    }
    if (shapeType < 7.5) {
        // 五边形：预计算PI常量 / pentagon: precomputed PI constant
        float pentR = 0.425 / cos(fmod(a + 3.14159, 1.25664) - 0.62832);
        return pentR - r;                                          // 五边形 / pentagon
    }
    // 六边形：预计算PI常量 / hexagon: precomputed PI constant
    float hexR = 0.435 / cos(fmod(a + 3.14159, 1.04720) - 0.52360);
    return hexR - r;                                               // 六边形 / hexagon
}

// 直接由 uv 求 SDF（多采样时复用）/ SDF from uv directly (reused by multi-sampling)
float shapeSDFAt(float2 uv, float shapeType) {
    float2 p = uv - 0.5;
    return shapeSDF(p, length(p), shapeType);
}

float4 PSMain(PS_INPUT input) : SV_TARGET {
    // 文字字符形状：采样字符图集纹理 / text glyph shape: samples the character atlas texture
    if (input.shape > 9.5) {
        int idx = (int)input.charIdx;
        int cols = (int)atlasCols;
        int row = idx / cols;
        int col = idx % cols;
        float2 cellUV = input.uv;
        float2 atlasUV = float2((col + cellUV.x) / atlasCols, (row + cellUV.y) / atlasRows);
        float4 tex;
        if (shaderQuality > 0.5) {
            // 4次旋转网格采样：缩小显示时抑制字形走样（等效于轻量三线性）/ 4-tap rotated grid: suppresses glyph aliasing when minified
            float2 a1 = fwidth(atlasUV) * float2( 0.125,  0.375);
            float2 a2 = fwidth(atlasUV) * float2( 0.375, -0.125);
            tex = 0.25 * (charAtlasTex.Sample(charAtlasSampler, atlasUV + a1)
                        + charAtlasTex.Sample(charAtlasSampler, atlasUV - a1)
                        + charAtlasTex.Sample(charAtlasSampler, atlasUV + a2)
                        + charAtlasTex.Sample(charAtlasSampler, atlasUV - a2));
        } else {
            tex = charAtlasTex.Sample(charAtlasSampler, atlasUV);
        }
        // 文字描边：采样8邻域最大alpha（膨胀字形），减去中心alpha得到轮廓环，与发光叠加 / text stroke: 8-neighbour max alpha (dilates the glyph), minus the centre alpha yields an outline ring, layered with glow
        if (strokeEnabled > 0.5) {
            float aMax = tex.a;
            aMax = max(aMax, charAtlasTex.Sample(charAtlasSampler, atlasUV + float2( strokeStepU, 0.0)).a);
            aMax = max(aMax, charAtlasTex.Sample(charAtlasSampler, atlasUV + float2(-strokeStepU, 0.0)).a);
            aMax = max(aMax, charAtlasTex.Sample(charAtlasSampler, atlasUV + float2(0.0,  strokeStepV)).a);
            aMax = max(aMax, charAtlasTex.Sample(charAtlasSampler, atlasUV + float2(0.0, -strokeStepV)).a);
            aMax = max(aMax, charAtlasTex.Sample(charAtlasSampler, atlasUV + float2( strokeStepU,  strokeStepV)).a);
            aMax = max(aMax, charAtlasTex.Sample(charAtlasSampler, atlasUV + float2(-strokeStepU,  strokeStepV)).a);
            aMax = max(aMax, charAtlasTex.Sample(charAtlasSampler, atlasUV + float2( strokeStepU, -strokeStepV)).a);
            aMax = max(aMax, charAtlasTex.Sample(charAtlasSampler, atlasUV + float2(-strokeStepU, -strokeStepV)).a);
            float outline = saturate(aMax - tex.a);  // 轮廓环，仅字形边缘附近非零 / outline ring, non-zero only near glyph edges
            // 描边颜色：auto=主色变暗，custom=自定义RGB / stroke colour: auto=darkened main colour, custom=RGB
            float3 strokeRgb = (strokeColorMode > 0.5)
                ? float3(strokeColorR, strokeColorG, strokeColorB)
                : input.color.rgb * strokeColorMul;
            float strokeA = outline * strokeAlphaMul;  // 描边环透明度 / outline ring alpha
            float total = max(tex.a, strokeA);
            if (total <= 0.001) discard;
            float fillRatio = tex.a / max(total, 0.0001);  // 填充占比，字形内部≈1，边缘环≈0 / fill ratio, ~1 inside the glyph, ~0 at the edge ring
            float3 rgb = lerp(strokeRgb, input.color.rgb, fillRatio);
            rgb = ApplyDither(rgb, input.pos.xy, ditherMode, ditherStrength, pixelScale, noiseSeed);
            return float4(rgb, input.color.a * total);
        }
        if (tex.a <= 0.01) discard;
        float3 rgb = input.color.rgb;
        rgb = ApplyDither(rgb, input.pos.xy, ditherMode, ditherStrength, pixelScale, noiseSeed);
        float alpha = input.color.a * tex.a;
        return float4(rgb, alpha);
    }
    float2 p = input.uv - 0.5;
    float r = length(p);
    // 先算 SDF 与屏幕空间导数，再做 early discard（discard 后取导数在部分硬件上未定义）/ compute SDF and screen-space derivatives BEFORE early discard (derivatives after discard are undefined on some hardware)
    float sdf = shapeSDF(p, r, input.shape);
    float sdfGrad = fwidth(sdf);
    float2 uvGrad = fwidth(input.uv);
    // Early discard：最大形状半径约0.5+边缘余量，超出直接跳过后续计算 / Early discard: max shape radius is ~0.5 plus edge margin; beyond it, skip the rest
    if (r > 0.62) discard;
    // SDF软边缘抗锯齿，边缘宽度随aaMode和粒子大小自适应 / SDF soft-edge AA; edge width adapts to aaMode and particle size
    float edge;
    if (aaMode < 0.5) {
        // off：硬边 / off: hard edge
        edge = 0.003;
    } else {
        float k = (aaMode < 1.5) ? 2.0 : ((aaMode < 2.5) ? 1.0 : 3.5);
        if (shaderQuality > 0.5) {
            // 屏幕空间导数：边缘过渡恒为约 k/2 像素，大小粒子一致 / screen-space derivatives: the edge transition stays ~k/2 px for any particle size
            edge = clamp(sdfGrad * k * 0.5, 0.002, 0.25);
        } else {
            edge = clamp(0.025 * (input.size / 12.0) * k * 0.5, 0.004, 0.12);
        }
    }
    // 主体覆盖率：解析覆盖率抗锯齿（analytic coverage AA）/ body coverage: analytic coverage anti-aliasing
    // sdf 是有符号距离（内部为正），一个像素跨多少个 SDF 单位由 sdfGrad=fwidth(sdf) 给出，/ sdf is a signed distance (positive inside); how many SDF units a pixel spans is given by sdfGrad = fwidth(sdf),
    // 因此盒式滤波的一阶精确覆盖率就是 saturate(0.5 + sdf / sdfGrad) —— 这是解析解，/ so the first-order exact coverage of a box filter is saturate(0.5 + sdf / sdfGrad) - an analytic solution,
    // 不需要像旧实现那样用 2/4 次额外的 shapeSDF() 求值去逼近。/ no need for the old approach's 2-4 extra shapeSDF() evaluations to approximate it.
    // 相比旧的 smoothstep(-edge, edge, sdf)：矩形滤波的覆盖率是线性的，smoothstep 的三次曲线 / versus the old smoothstep(-edge, edge, sdf): box-filter coverage is linear, while smoothstep's cubic curve
    // 会把边缘切得过软；解析式在数学上更正确，且指令更少（无 clamp / 无多项式）。/ softens edges too much; the analytic form is mathematically more correct and uses fewer instructions (no clamp / no polynomial).
    // Analytic coverage AA: coverage of the box filter is saturate(0.5 + sdf/fwidth(sdf)), which is
    // exact to first order — extra shapeSDF() samples are only needed to compensate for curvature,
    // not to approximate the coverage itself. Compared with the old smoothstep(-edge,edge,sdf), this is
    // mathematically correct for a box filter and cheaper (no clamp, no polynomial).
//
    // aaMode 仍生效，映射为过渡带宽（单位：像素）/ aaMode still drives the transition width in pixels:
    //   off 0.24 / smooth 1.0 / crisp 0.5 / extra 1.75 —— 保持原有的软硬次序。/   off 0.24 / smooth 1.0 / crisp 0.5 / extra 1.75 - preserving the original softness ordering.
    float covK = (aaMode < 0.5) ? 0.12 : ((aaMode < 1.5) ? 1.0 : ((aaMode < 2.5) ? 0.5 : 1.75));
    float covGrad = max(sdfGrad * covK, 1e-5);
    float mask = saturate(0.5 + sdf / covGrad);
    // 高精度开关：额外做旋转网格超采样。解析式是一阶近似，对 SDF 曲率大的地方（星形尖角、/ high-precision toggle: extra rotated-grid supersampling. The analytic form is a first-order approximation that still errs on high SDF curvature (star tips,
    // 六边形折角）仍有误差，此时可用此项补偿。默认关闭 —— 每次要多 2~4 次 shapeSDF() 求值。/ hexagon corners); this compensates. Off by default - each use costs 2-4 extra shapeSDF() evaluations.
    if (sdfSamples > 1.5 && shaderQuality > 0.5) {
        if (sdfSamples < 2.5) {
            float2 o = uvGrad * 0.25;
            mask = 0.5 * (saturate(0.5 + shapeSDFAt(input.uv + o, input.shape) / covGrad)
                        + saturate(0.5 + shapeSDFAt(input.uv - o, input.shape) / covGrad));
        } else {
            float2 o1 = uvGrad * float2( 0.125,  0.375);
            float2 o2 = uvGrad * float2( 0.375, -0.125);
            mask = 0.25 * (saturate(0.5 + shapeSDFAt(input.uv + o1, input.shape) / covGrad)
                         + saturate(0.5 + shapeSDFAt(input.uv - o1, input.shape) / covGrad)
                         + saturate(0.5 + shapeSDFAt(input.uv + o2, input.shape) / covGrad)
                         + saturate(0.5 + shapeSDFAt(input.uv - o2, input.shape) / covGrad));
        }
    }
    // 描边轮廓环（|sdf| 环带，形状边缘），非文字形状同样生效 / stroke outline ring (|sdf| band at the shape edge), applies to non-text shapes too
    float outline = 0.0;
    if (strokeEnabled > 0.5) {
        float sdfHalf = strokeWidthPx / (input.size * 2.0 + 0.001);   // 单侧半宽（SDF 单位）/ one-side half width (SDF units)
        float strokeEdge = max(edge, 0.004);
        outline = 1.0 - smoothstep(sdfHalf - strokeEdge, sdfHalf + strokeEdge, abs(sdf));
    }
    float strokeA = outline * strokeAlphaMul;
    float total = max(mask, strokeA);
    if (total <= 0.001) discard;
    // 合并径向光照 + 2.5D深度光照 / combined radial + 2.5D depth lighting
    float radialLight = max(1.0 - r * 0.3, 0.6) * (1.0 + input.depth * 0.25);
    float3 rgb = input.color.rgb * radialLight;
    // 自适应对比度 v2：保持色相的明度调整 / adaptive contrast v2: hue-preserving luma adjustment
    // v1 直接线性乘 RGB 会把高饱和色压脏，且亮背景最极端时压到原亮度的 30%；/ v1's direct linear RGB scaling dirtied saturated colours and dimmed to 30% of original luma at the extreme bright background;
    // v2 保色 + soft-knee（最大压暗 0.70→0.42），并在开启描边时保留主体原色。/ v2 preserves colour, uses a soft knee (max dim 0.70->0.42) and keeps the body colour when stroking is on.
    // 实现收敛到 MT_HLSL_COLOR_COMMON 的 ApplyAdaptiveContrast()，与拖尾着色器共用同一份 / the implementation converges into ApplyAdaptiveContrast() in MT_HLSL_COLOR_COMMON, shared with the trail shader
    // —— 不再存在「两处手写副本靠注释保持同步」的风险。/ - no more risk of two hand-written copies kept in sync by comments.
    // v1 muddied saturated hues and could dim to 30%; v2 keeps chroma, spares midtones, and preserves the body
    // colour when a stroke is enabled. Now shares ApplyAdaptiveContrast() from MT_HLSL_COLOR_COMMON with the
    // trail shader — no more two hand-maintained copies.
    if (adaptiveFlag > 0.5) {
        rgb = ApplyAdaptiveContrast(rgb, bgLuminance, strokeEnabled);
    }
    // 描边合成：边缘环带用描边色，内部用填充色 / stroke compositing: the edge band uses the stroke colour, the interior uses the fill colour
    if (strokeEnabled > 0.5) {
        float3 strokeRgb = (strokeColorMode > 0.5) ? float3(strokeColorR, strokeColorG, strokeColorB) : rgb * strokeColorMul;
        // 自适应对比度下，亮背景改为加深描边来提供对比（替代 v1 对主体的整体压暗）。/ under adaptive contrast, bright backgrounds deepen the stroke instead of dimming the whole body (v1's approach).
        if (adaptiveFlag > 0.5) {
            // 描边加深同样用凸曲线，只在浅色背景发力 / stroke deepening also uses a convex curve, acting only on light backgrounds
            strokeRgb = ApplyAdaptiveStroke(strokeRgb, bgLuminance);
        }
        float fillRatio = mask / max(total, 0.0001);
        rgb = lerp(strokeRgb, rgb, fillRatio);
    }
    // 输出非预乘alpha：混合状态SRC_ALPHA/INV_SRC_ALPHA自动完成预乘 / output is non-premultiplied alpha: the SRC_ALPHA/INV_SRC_ALPHA blend state premultiplies automatically
    float alpha = input.color.a * total;
    rgb = ApplyDither(rgb, input.pos.xy, ditherMode, ditherStrength, pixelScale, noiseSeed);
    return float4(rgb, alpha);
}
)";
static const char* g_blitVS = R"(
struct VS_IN { float2 pos : POSITION; float2 uv : TEXCOORD0; };
struct VS_OUT { float4 pos : SV_POSITION; float2 uv : TEXCOORD0; };
VS_OUT VSMain(VS_IN input) {
    VS_OUT o;
    o.pos = float4(input.pos, 0.0, 1.0);
    o.uv = input.uv;
    return o;
}
)";
static const char* g_blitPS = R"(
cbuffer BlitConstants : register(b0) {
    float2 texelSize;  // 1/源宽, 1/源高 / 1/source width, 1/source height
    float filterMode;   // 0=bilinear 1=box 2=tent 3=lanczos
    float sharpness;  // 非锐化掩模强度 / unsharp-mask strength
    float scale;        // 降采样倍率（源像素/目标像素）/ downsample factor
    float3 _padBlit;  // 补齐到 32 字节 / pads to 32 bytes
};

Texture2D offscreenTex : register(t0);
SamplerState offscreenSampler : register(s0);
struct VS_OUT { float4 pos : SV_POSITION; float2 uv : TEXCOORD0; };

// Lanczos2 核权重（编译期常量，a=2）/ Lanczos2 kernel weights (compile-time constants, a=2)
// 采样位置固定为 ±0.5 / ±1.5，因此四个权重与像素无关 —— 预计算后由 fxc 折叠，/ sample positions are fixed at ±0.5 / ±1.5, so the four weights are pixel-independent - precomputed and folded by fxc,
// 省去原实现里每像素 4 次 Lanczos2() 调用（= 8 次 sin + 分支 + 除法）。/ saving the old implementation's 4 Lanczos2() calls per pixel (= 8 sins + branches + divisions).
// The tap positions are fixed at ±0.5/±1.5, so the four weights do not depend on the pixel;
// precomputing them lets fxc fold the constants and removes 8 sin() per pixel.
//   Lanczos2(x) = 2*sin(πx)*sin(πx/2)/(πx)²
static const float LANCZOS2_W05 = 0.5731591683;   // Lanczos2(±0.5)
static const float LANCZOS2_W15 = -0.0636843520;  // Lanczos2(±1.5)
// 权重和 = (Σw)²，同样为编译期常量 / the weight sum squared (Σw)^2 is also a compile-time constant
static const float LANCZOS2_WSUM = 1.0382583535;

float4 PSMain(VS_OUT input) : SV_TARGET {
    float2 uv = input.uv;
    float S = max(scale, 1.0);
    // 1:1 拷贝（仅MSAA）或用户选择双线性：单次采样 / 1:1 copy (MSAA only) or user-chosen bilinear: single sample
    if (S <= 1.001 || filterMode < 0.5) {
        return offscreenTex.Sample(offscreenSampler, uv);
    }
    float2 t = texelSize;
    float4 c;
    if (filterMode < 1.5) {
        // Box：4x4 网格 16 次采样，均匀覆盖 1 个目标像素的 SxS 源像素 / Box: 4x4 grid, 16 samples, evenly covering the SxS source pixels of one target pixel
        float2 q = t * (S * 0.125);  // ±S/8, ±3S/8 源像素 / ±S/8, ±3S/8 source pixels
        c  = offscreenTex.Sample(offscreenSampler, uv + float2(-3.0*q.x, -3.0*q.y));
        c += offscreenTex.Sample(offscreenSampler, uv + float2(-q.x,    -3.0*q.y));
        c += offscreenTex.Sample(offscreenSampler, uv + float2( q.x,    -3.0*q.y));
        c += offscreenTex.Sample(offscreenSampler, uv + float2( 3.0*q.x,-3.0*q.y));
        c += offscreenTex.Sample(offscreenSampler, uv + float2(-3.0*q.x, -q.y));
        c += offscreenTex.Sample(offscreenSampler, uv + float2(-q.x,    -q.y));
        c += offscreenTex.Sample(offscreenSampler, uv + float2( q.x,    -q.y));
        c += offscreenTex.Sample(offscreenSampler, uv + float2( 3.0*q.x,-q.y));
        c += offscreenTex.Sample(offscreenSampler, uv + float2(-3.0*q.x, q.y));
        c += offscreenTex.Sample(offscreenSampler, uv + float2(-q.x,     q.y));
        c += offscreenTex.Sample(offscreenSampler, uv + float2( q.x,     q.y));
        c += offscreenTex.Sample(offscreenSampler, uv + float2( 3.0*q.x, q.y));
        c += offscreenTex.Sample(offscreenSampler, uv + float2(-3.0*q.x, 3.0*q.y));
        c += offscreenTex.Sample(offscreenSampler, uv + float2(-q.x,     3.0*q.y));
        c += offscreenTex.Sample(offscreenSampler, uv + float2( q.x,     3.0*q.y));
        c += offscreenTex.Sample(offscreenSampler, uv + float2( 3.0*q.x, 3.0*q.y));
        c *= 0.0625;
    } else if (filterMode < 2.5) {
        // Tent：4 次双线性采样等价 3x3 帐篷核 {1,2,1;2,4,2;1,2,1}/16（半径 S/2）/ Tent: 4 bilinear samples equal a 3x3 tent kernel {1,2,1;2,4,2;1,2,1}/16 (radius S/2)
        float2 q = t * (S * 0.25);
        c  = offscreenTex.Sample(offscreenSampler, uv + float2(-q.x, -q.y));
        c += offscreenTex.Sample(offscreenSampler, uv + float2( q.x, -q.y));
        c += offscreenTex.Sample(offscreenSampler, uv + float2(-q.x,  q.y));
        c += offscreenTex.Sample(offscreenSampler, uv + float2( q.x,  q.y));
        c *= 0.25;
    } else {
        // Lanczos2：4x4 网格，权重按 Lanczos 核计算并归一化 / Lanczos2: 4x4 grid, weights computed from the Lanczos kernel and normalised
        float2 q = t * (S * 0.5);  // ±0.5S, ±1.5S 源像素 / ±0.5S, ±1.5S source pixels
        const float w[4] = { LANCZOS2_W15, LANCZOS2_W05, LANCZOS2_W05, LANCZOS2_W15 };
        c = 0.0;
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                float wt = w[i] * w[j];
                c += offscreenTex.Sample(offscreenSampler,
                     uv + float2(q.x * (2.0 * (float)i - 3.0), q.y * (2.0 * (float)j - 3.0))) * wt;
            }
        }
        c /= LANCZOS2_WSUM;
    }
    // 非锐化掩模：找回滤波抹掉的边缘锐度（半径=1目标像素）/ Unsharp mask (radius = 1 dest pixel)
    if (sharpness > 0.001) {
        float2 q = t * S;
        float4 blur = 0.25 * (offscreenTex.Sample(offscreenSampler, uv + float2(-q.x, 0.0))
                            + offscreenTex.Sample(offscreenSampler, uv + float2( q.x, 0.0))
                            + offscreenTex.Sample(offscreenSampler, uv + float2(0.0, -q.y))
                            + offscreenTex.Sample(offscreenSampler, uv + float2(0.0,  q.y)));
        c = max(c + (c - blur) * sharpness, 0.0);
    }
    return c;
}
)";
static const char* g_postTemporalPS = R"(
cbuffer PostConstants : register(b0) {
    float2 texelSize;  // cb0[0].xy  1/源分辨率 / cb0[0].xy  1/source resolution
    float2 outTexelSize;  // cb0[0].zw  1/目标分辨率 / cb0[0].zw  1/target resolution
    float4 param0;        // cb0[1]     x = k（上一帧保留比例 0..1）/ x = k (history retained 0..1)
    float4 param1;  // cb0[2]     保留 / reserved
    float4 param2;  // cb0[3]     保留 / reserved
    float4 param3;  // cb0[4]     保留 / reserved
};

Texture2D curTex : register(t0);
Texture2D histTex : register(t1);
SamplerState postSampler : register(s0);

struct VS_OUT { float4 pos : SV_POSITION; float2 uv : TEXCOORD0; };

float4 PSMain(VS_OUT input) : SV_TARGET {
    float4 cur  = curTex.Sample(postSampler, input.uv);
    float4 hist = histTex.Sample(postSampler, input.uv);
    float k = saturate(param0.x);
    // 预乘 alpha 线性插值保持预乘不变式 / linear interpolation in premultiplied alpha preserves the premultiply invariant
    return lerp(cur, hist, k);
}
)";
static const char* g_postFXAAPS = R"(
cbuffer PostConstants : register(b0) {
    float2 texelSize;  // cb0[0].xy  1/源分辨率 / cb0[0].xy  1/source resolution
    float2 outTexelSize;  // cb0[0].zw  1/目标分辨率 / cb0[0].zw  1/target resolution
    float4 param0;        // cb0[1]     x = 质量档位（1=标准，2=强化）/ x = quality (1=normal, 2=extreme)
    float4 param1;  // cb0[2]     保留 / reserved
    float4 param2;  // cb0[3]     保留 / reserved
    float4 param3;  // cb0[4]     保留 / reserved
};

Texture2D srcTex : register(t0);
SamplerState srcSampler : register(s0);

struct VS_OUT { float4 pos : SV_POSITION; float2 uv : TEXCOORD0; };

// Rec.601 亮度 / Rec.601 luma
float Luma(float3 c) {
    return dot(c, float3(0.299, 0.587, 0.114));
}

float4 PSMain(VS_OUT input) : SV_TARGET {
    float2 uv = input.uv;
    float2 inv = outTexelSize;  // 屏幕纹素尺寸 / screen texel size

    float4 cM = srcTex.Sample(srcSampler, uv);
    float4 cNW = srcTex.Sample(srcSampler, uv + float2(-inv.x, -inv.y));
    float4 cNE = srcTex.Sample(srcSampler, uv + float2( inv.x, -inv.y));
    float4 cSW = srcTex.Sample(srcSampler, uv + float2(-inv.x,  inv.y));
    float4 cSE = srcTex.Sample(srcSampler, uv + float2( inv.x,  inv.y));
    float4 cN  = srcTex.Sample(srcSampler, uv + float2(0.0,   -inv.y));
    float4 cS  = srcTex.Sample(srcSampler, uv + float2(0.0,    inv.y));
    float4 cW  = srcTex.Sample(srcSampler, uv + float2(-inv.x, 0.0));
    float4 cE  = srcTex.Sample(srcSampler, uv + float2( inv.x, 0.0));

    float lumaM  = Luma(cM.rgb);
    float lumaNW = Luma(cNW.rgb);
    float lumaNE = Luma(cNE.rgb);
    float lumaSW = Luma(cSW.rgb);
    float lumaSE = Luma(cSE.rgb);
    float lumaN  = Luma(cN.rgb);
    float lumaS  = Luma(cS.rgb);
    float lumaW  = Luma(cW.rgb);
    float lumaE  = Luma(cE.rgb);

    float lumaMin = min(lumaM, min(min(lumaNW, lumaNE), min(lumaSW, lumaSE)));
    float lumaMax = max(lumaM, max(max(lumaNW, lumaNE), max(lumaSW, lumaSE)));
    lumaMin = min(lumaMin, min(min(lumaN, lumaS), min(lumaW, lumaE)));
    lumaMax = max(lumaMax, max(max(lumaN, lumaS), max(lumaW, lumaE)));

    // 对比度不足时直接返回原像素（大面积透明区域会走这里，开销最低）/ return the original pixel when contrast is insufficient (large transparent areas take this path at lowest cost)
    if ((lumaMax - lumaMin) < max(0.0312, lumaMax * 0.125)) return cM;

    // (1) 边缘方向：由 2x2 四角亮度差决定 / (1) edge direction: decided by the 2x2 corner luma differences
    float2 dir;
    dir.x = -((lumaNW + lumaNE) - (lumaSW + lumaSE));
    dir.y =  ((lumaNW + lumaSW) - (lumaNE + lumaSE));

    float dirReduce = max((lumaNW + lumaNE + lumaSW + lumaSE) * (0.25 * 0.125), 1.0 / 128.0);
    float rcpDirMin = 1.0 / (min(abs(dir.x), abs(dir.y)) + dirReduce);
    dir = min(float2(8.0, 8.0), max(float2(-8.0, -8.0), dir * rcpDirMin)) * inv;

    // (2) 强化档：沿边缘方向步进搜索边缘长度，用它限制混合跨度（搜索更多步）/ (2) extreme tier: steps along the edge to estimate its length and bound the blend span (more search steps)
    float2 blendDir = dir;
    if (param0.x > 1.5) {
        float2 pixDir = dir / inv;  // 像素空间方向 / direction in pixel space
        pixDir = pixDir / max(length(pixDir), 1e-6);
        float2 stepUV = pixDir * inv;  // 每步 1 屏幕纹素 / one screen texel per step
        float lenPos = 0.0;
        float lenNeg = 0.0;
        // 循环内用 SampleLevel 显式指定 mip，避免导数在变长循环中未定义 / use SampleLevel with an explicit mip inside the loop: derivatives are undefined across variable-length loops
        [loop] for (int i = 1; i <= 8; i++) {
            float l = Luma(srcTex.SampleLevel(srcSampler, uv + stepUV * (float)i, 0.0).rgb);
            if (l < lumaMin || l > lumaMax) break;
            lenPos += 1.0;
        }
        [loop] for (int j = 1; j <= 8; j++) {
            float l = Luma(srcTex.SampleLevel(srcSampler, uv - stepUV * (float)j, 0.0).rgb);
            if (l < lumaMin || l > lumaMax) break;
            lenNeg += 1.0;
        }
        float edgeLen = min(min(lenPos, lenNeg), 8.0);
        blendDir = pixDir * inv * (edgeLen + 0.5);
    }

    // (3) 沿边缘取 4 个样本做三角滤波（标准档 2+2，强化档跨度由边缘长度决定）/ (3) four samples along the edge in a tent filter (2+2 at normal tier; span set by edge length at extreme tier)
    float4 rgbA = 0.5 * (srcTex.Sample(srcSampler, uv + blendDir * (-1.0 / 6.0))
                       + srcTex.Sample(srcSampler, uv + blendDir * ( 1.0 / 6.0)));
    float4 rgbB = rgbA * 0.5 + 0.25 * (srcTex.Sample(srcSampler, uv + blendDir * (-0.5))
                                     + srcTex.Sample(srcSampler, uv + blendDir * ( 0.5)));

    float lumaB = Luma(rgbB.rgb);
    if ((lumaB < lumaMin) || (lumaB > lumaMax)) return rgbA;
    return rgbB;
}
)";
static const char* g_postBloomThresholdPS = MT_HLSL_COLOR_COMMON
R"(
cbuffer PostConstants : register(b0) {
    float2 texelSize;  // cb0[0].xy  1/源分辨率 / cb0[0].xy  1/source resolution
    float2 outTexelSize;  // cb0[0].zw  1/目标分辨率 / cb0[0].zw  1/target resolution
    float4 param0;        // cb0[1]     x = 阈值 threshold，y = 软膝宽度 knee / x = threshold, y = soft-knee width
    float4 param1;  // cb0[2]     保留 / reserved
    float4 param2;  // cb0[3]     保留 / reserved
    float4 param3;  // cb0[4]     保留 / reserved
};

Texture2D srcTex : register(t0);
SamplerState srcSampler : register(s0);

struct VS_OUT { float4 pos : SV_POSITION; float2 uv : TEXCOORD0; };

float4 PSMain(VS_OUT input) : SV_TARGET {
    float2 t = texelSize;
    // 4 抽头盒式：每个目标像素覆盖 2x2 源像素 / 4-tap box: each target pixel covers a 2x2 source area
    float4 c = 0.25 * (srcTex.Sample(srcSampler, input.uv + float2(-t.x, -t.y))
                     + srcTex.Sample(srcSampler, input.uv + float2( t.x, -t.y))
                     + srcTex.Sample(srcSampler, input.uv + float2(-t.x,  t.y))
                     + srcTex.Sample(srcSampler, input.uv + float2( t.x,  t.y)));

    // 场景已是伽马编码，直接在显示空间取亮度（Rec.709 权重）/ the scene is already gamma-encoded, so luma is taken in display space (Rec.709 weights)
    float luma = Luma709(c.rgb);
    float thr = param0.x;
    float knee = max(param0.y, 1e-4);

    // 软膝：阈值附近平滑过渡，避免亮度硬切造成的色带 / soft knee: smooth transition near the threshold avoids banding from a hard luma cut
    float soft = clamp(luma - thr + knee, 0.0, 2.0 * knee);
    soft = soft * soft / (4.0 * knee);
    float contrib = max(soft, luma - thr) / max(luma, 1e-4);

    // 预乘 alpha 一并缩放，保持 rgb <= a 的预乘关系 / scale the premultiplied alpha along with it, preserving rgb <= a
    return float4(c.rgb * contrib, c.a * contrib);
}
)";
static const char* g_postBloomBlurPS = R"(
cbuffer PostConstants : register(b0) {
    float2 texelSize;  // cb0[0].xy  1/源分辨率 / cb0[0].xy  1/source resolution
    float2 outTexelSize;  // cb0[0].zw  1/目标分辨率 / cb0[0].zw  1/target resolution
    float4 param0;        // cb0[1]     xy = 模糊方向（纹素单位）/ xy = blur direction in texels
    float4 param1;  // cb0[2]     保留 / reserved
    float4 param2;  // cb0[3]     保留 / reserved
    float4 param3;  // cb0[4]     保留 / reserved
};

Texture2D srcTex : register(t0);
SamplerState srcSampler : register(s0);

struct VS_OUT { float4 pos : SV_POSITION; float2 uv : TEXCOORD0; };

float4 PSMain(VS_OUT input) : SV_TARGET {
    float2 t = texelSize * param0.xy;  // 每步 UV 偏移 / UV offset per step
    // 双线性折半采样：把相隔 1 纹素的相邻两抽头合并成一次带权双线性取样，/ halved bilinear sampling: two adjacent taps one texel apart merge into one weighted bilinear fetch,
    // 9 抽头高斯因此只需 5 次取样，合成核与原实现逐权重一致（取样数 -44%）。/ so the 9-tap Gaussian needs only 5 fetches, with a composite kernel weight-identical to the original (-44% fetches).
    // Bilinear tap-pairing: adjacent taps 1 texel apart are folded into one weighted bilinear
    // fetch, so the 9-tap Gaussian costs only 5 fetches with an identical effective kernel.
    //   tap@1(w=0.1945946) + tap@2(w=0.1216216) → w=0.3162162162 @ 1.3846153846 texels
    //   tap@3(w=0.0540541) + tap@4(w=0.0162162) → w=0.0702702703 @ 3.2307692308 texels
    float4 c = srcTex.Sample(srcSampler, input.uv) * 0.2270270270;
    c += (srcTex.Sample(srcSampler, input.uv + t * 1.3846153846)
        + srcTex.Sample(srcSampler, input.uv - t * 1.3846153846)) * 0.3162162162;
    c += (srcTex.Sample(srcSampler, input.uv + t * 3.2307692308)
        + srcTex.Sample(srcSampler, input.uv - t * 3.2307692308)) * 0.0702702703;
    return c;
}
)";
static const char* g_postBloomCompositePS = R"(
cbuffer PostConstants : register(b0) {
    float2 texelSize;  // cb0[0].xy  1/源分辨率 / cb0[0].xy  1/source resolution
    float2 outTexelSize;  // cb0[0].zw  1/目标分辨率 / cb0[0].zw  1/target resolution
    float4 param0;        // cb0[1]     x = 泛光强度（0..1）/ x = bloom intensity (0..1)
    float4 param1;  // cb0[2]     保留 / reserved
    float4 param2;  // cb0[3]     保留 / reserved
    float4 param3;  // cb0[4]     保留 / reserved
};

Texture2D sceneTex : register(t0);  // 原始场景 / original scene
Texture2D bloomHalfTex : register(t1);  // 1/2 分辨率泛光 / half-res bloom
Texture2D bloomQuarterTex : register(t2);  // 1/4 分辨率泛光 / quarter-res bloom
SamplerState postSampler : register(s0);

struct VS_OUT { float4 pos : SV_POSITION; float2 uv : TEXCOORD0; };

float4 PSMain(VS_OUT input) : SV_TARGET {
    float4 scene = sceneTex.Sample(postSampler, input.uv);
    // 双线性上采样回到全分辨率 / bilinear upsample back to full resolution
    float4 b0 = bloomHalfTex.Sample(postSampler, input.uv);
    float4 b1 = bloomQuarterTex.Sample(postSampler, input.uv);
    // 两级权重和为 1，泛光能量与原亮部能量相当 / the two level weights sum to 1, so bloom energy matches the original bright-pass energy
    float k = param0.x;
    float3 bloom = (b0.rgb * 0.65 + b1.rgb * 0.35) * k;
    return float4(scene.rgb + bloom, scene.a);
}
)";
static const char* g_postHDREncodePS = R"(
cbuffer PostConstants : register(b0) {
    float2 texelSize;  // cb0[0].xy  1/源分辨率 / cb0[0].xy  1/source resolution
    float2 outTexelSize;  // cb0[0].zw  1/目标分辨率 / cb0[0].zw  1/target resolution
    float4 param0;        // cb0[1]     x = 色彩空间（1=scRGB 2=HDR10/PQ），y = 增益倍率，z = 色调映射（0/1/2）/ x = colour space (1=scRGB 2=HDR10/PQ), y = gain multiplier, z = tonemap (0/1/2)
                          //            x = colour space (1=scRGB 2=HDR10/PQ), y = boost multiplier, z = tonemap
    float4 param1;  // cb0[2]     保留 / reserved
    float4 param2;  // cb0[3]     保留 / reserved
    float4 param3;  // cb0[4]     保留 / reserved
};

Texture2D sceneTex : register(t0);
SamplerState srcSampler : register(s0);

struct VS_OUT { float4 pos : SV_POSITION; float2 uv : TEXCOORD0; };

// sRGB EOTF（精确分段）/ sRGB EOTF (accurate piecewise)
float3 SRGBToLinear(float3 c) {
    float3 lo = c / 12.92;
    float3 hi = pow((c + 0.055) / 1.055, 2.4);
    return lerp(lo, hi, step(0.04045, c));
}

// ST.2084 (PQ) OETF（逆 EOTF），输入为归一化亮度 N = L / ST.2084 (PQ) OETF (inverse EOTF), input is normalised luma N = L
float3 PQEncode(float3 N) {
    float m1 = 0.1593017578125;
    float m2 = 78.84375;
    float c1 = 0.8359375;
    float c2 = 18.8515625;
    float c3 = 18.6875;
    float3 Np = pow(max(N, 0.0), m1);
    return pow((c1 + c2 * Np) / (1.0 + c3 * Np), m2);
}

// ACES 电影感近似（Narkowicz）/ ACES filmic approximation (Narkowicz)
float3 ACESFilm(float3 x) {
    float a = 2.51;
    float b = 0.03;
    float c = 2.43;
    float d = 0.59;
    float e = 0.14;
    return saturate((x * (a * x + b)) / (x * (c * x + d) + e));
}

float4 PSMain(VS_OUT input) : SV_TARGET {
    float4 c = sceneTex.Sample(srcSampler, input.uv);
    // 全透明像素（整屏绝大多数，前景只是细拖尾）提前返回，跳过 EOTF/增益/色调映射/PQ。/ fully transparent pixels (the vast majority of the screen, since the foreground is just a thin trail) return early, skipping EOTF/gain/tonemap/PQ.
    // rgb 与 a 同时为 0 时所有后续运算的输出恒为 0，因此这是精确等价的早退。/ when both rgb and a are 0, every later operation outputs 0, so this early-out is exactly equivalent.
    if (dot(c, c) == 0.0) return c;
    // 伽马编码 → 线性 / gamma-encoded -> linear
    float3 lin = SRGBToLinear(max(c.rgb, 0.0));

    // 高光增益：boost = 0 时为精确空操作 / highlight gain: an exact no-op when boost = 0
    lin *= (1.0 + param0.y);

    // 色调映射：仅在启用时执行 / tonemapping: only when enabled
    if (param0.z > 0.5) {
        if (param0.z < 1.5) {
            lin = lin / (1.0 + lin);  // 扩展 Reinhard / extended Reinhard
        } else {
            lin = ACESFilm(lin);            // ACES
        }
    }

    float3 outCol;
    if (param0.x < 1.5) {
        // scRGB（RGB_FULL_G10_NONE_P709）：线性输出，1.0 = SDR 参考白（DWM 按 SDR 白电平换算）/ scRGB (RGB_FULL_G10_NONE_P709): linear output, 1.0 = SDR reference white (DWM converts by SDR white level)
        outCol = lin;
    } else {
        // HDR10（RGB_FULL_G2084_NONE_P2020）：PQ 编码，1.0 线性 = 203 nit SDR 参考白 / HDR10 (RGB_FULL_G2084_NONE_P2020): PQ-encoded, 1.0 linear = 203-nit SDR reference white
        outCol = PQEncode(lin * (203.0 / 10000.0));
    }

    // alpha 精确保留：交换链是预乘 alpha，颜色运算不触碰 alpha / exact alpha preservation: the swap chain is premultiplied alpha and colour maths never touches alpha
    return float4(outCol, c.a);
}
)";
struct VertexPosColor {
    float x, y, z;
    float r, g, b, a;
    float u;
    float v;
};
struct ParticleInstance {
    float x, y, z;
    float r, g, b, a;
    float size;
    float shapeType;
    float rotation;
    float charIndex;
};
ID3D11VertexShader* g_pNativeVS = nullptr;
ID3D11PixelShader* g_pNativePS = nullptr;
ID3D11VertexShader* g_pParticleVS = nullptr;
ID3D11PixelShader* g_pParticlePS = nullptr;
ID3D11InputLayout* g_pNativeLayout = nullptr;
ID3D11InputLayout* g_pParticleLayout = nullptr;
ID3D11Buffer* g_pConstantBuffer = nullptr;
ID3D11Buffer* g_pTrailVB = nullptr;
ID3D11Buffer* g_pParticleQuadVB = nullptr;
ID3D11Buffer* g_pParticleInstanceBuf = nullptr;
ID3D11BlendState* g_pAlphaBlend = nullptr;
ID3D11BlendState* g_pAdditiveBlend = nullptr;
ID3D11RasterizerState* g_pRasterState = nullptr;
ID3D11Texture2D* g_pCharAtlasTex = nullptr;
ID3D11ShaderResourceView* g_pCharAtlasSRV = nullptr;
ID3D11SamplerState* g_pCharAtlasSampler = nullptr;
IDXGISurface* g_pCharAtlasSurface = nullptr;
ID2D1Bitmap1* g_pCharAtlasBmp = nullptr;
static const int kMaxTrailVertices = 16384;
struct ShaderConstants {
    float screenSize[2];
    float perspective;
    float pad0;
    float gradient[16][4];
    int   gradientCount;
    float bgLuminance;
    float adaptiveFlag;
    float smoothFlag;
    float edgeSoftness;
    float atlasRows;
    float atlasCols;
    float aaMode;
    float textAspect;
    float strokeStepU;
    float strokeStepV;
    float strokeEnabled;
    float strokeAlphaMul;
    float strokeColorMode;
    float strokeColorR;
    float strokeColorG;
    float strokeColorB;
    float strokeColorMul;
    float strokeWidthPx;
    float trailStrokeEnabled;
    float trailStrokeHalf;
    float shaderQuality;
    float sdfSamples;
    float ditherMode;
    float ditherStrength;
    float pixelScale;
    float noiseSeed;
    float pad3;
    float padTail[4][4];
};
static_assert(sizeof(ShaderConstants) == 448, "ShaderConstants must be 448 bytes (112 floats)");
static_assert(offsetof(ShaderConstants, gradientCount) == 272, "gradientCount must sit at byte 272 (float #68)");
static_assert(offsetof(ShaderConstants, atlasRows) == 292, "atlasRows must sit at byte 292 (float #73)");
static_assert(offsetof(ShaderConstants, noiseSeed) == 376, "noiseSeed must sit at byte 376 (float #94)");
static constexpr int kCbSeedFloatIndex = offsetof(ShaderConstants, noiseSeed) / static_cast<int>(sizeof(float));
static_assert(kCbSeedFloatIndex == 94, "noiseSeed float index must remain 94");
static ShaderConstants g_cbCache = {};
static bool g_cbCacheValid = false;
static void InvalidateConstantBuffer() { g_cbCacheValid = false; }
int g_charAtlasCols = 0;
int g_charAtlasRows = 0;
int g_charAtlasCellW = 0;
int g_charAtlasCellH = 0;
float g_textAspectRatio = 1.0f;
std::atomic<bool> g_charAtlasDirty{true};
std::vector<std::wstring> g_atlasChars;
std::vector<int> g_phraseChunkStart;
std::vector<int> g_phraseChunkCount;
DWORD g_globalChunkClock = 0;
enum class TagFmtSlot { None, Time, Day };
typedef std::wstring (*TagResolver)(const struct tm& tmv);
struct TextTagDef {
    const wchar_t* id;
    const wchar_t* defaultFmt;
    TagFmtSlot slot;
    TagResolver resolve;
};
static std::wstring ResolveIsoWeekday(const struct tm& tmv) {
    int iso = ((tmv.tm_wday + 6) % 7) + 1;
    wchar_t b[2] = { (wchar_t)(L'0' + iso), 0 };
    return std::wstring(b);
}
static const TextTagDef g_textTagDefs[] = {
    { L"time",     L"%H:%M:%S",          TagFmtSlot::Time, nullptr },
    { L"day",      L"%Y-%m-%d",          TagFmtSlot::Day,  nullptr },
    { L"datetime", L"%Y-%m-%dT%H:%M:%S", TagFmtSlot::None, nullptr },
    { L"weekday",  L"%u",                TagFmtSlot::None, ResolveIsoWeekday },
};
wchar_t g_textTagTimeFmt[64] = L"";
wchar_t g_textTagDayFmt[64] = L"";
int g_textTagMode = 1;
static std::unordered_map<std::wstring, std::wstring> g_tagFrozenCache;
std::atomic<bool> g_tagCacheDirty{false};
static const struct { const wchar_t* from; const wchar_t* to; } kPosixAliases[] = {
    { L"%T", L"%H:%M:%S" },
    { L"%F", L"%Y-%m-%d" },
    { L"%R", L"%H:%M" },
    { L"%D", L"%m/%d/%y" },
    { L"%e", L"%d" },
};
static std::wstring NormalizeStrftimeAliases(const std::wstring& fmt) {
    std::wstring out;
    for (size_t i = 0; i < fmt.size();) {
        if (fmt[i] == L'%' && i + 1 < fmt.size() && fmt[i + 1] == L'%') {
            out += L"%%"; i += 2; continue;
        }
        bool hit = false;
        if (fmt[i] == L'%' && i + 1 < fmt.size()) {
            wchar_t pair[3] = { L'%', fmt[i + 1], 0 };
            for (const auto& a : kPosixAliases) {
                if (wcscmp(pair, a.from) == 0) { out += a.to; hit = true; break; }
            }
        }
        if (hit) { i += 2; continue; }
        out += fmt[i++];
    }
    return out;
}
static bool EvalTextTag(const std::wstring& id, bool hasInlineFmt, const std::wstring& inlineFmt,
                        std::wstring& out) {
    const TextTagDef* def = nullptr;
    for (const auto& d : g_textTagDefs) {
        if (id == d.id) { def = &d; break; }
    }
    if (!def) return false;
    std::wstring key;
    if (g_textTagMode == 0) {
        key = def->id;
        if (hasInlineFmt) { key += L':'; key += inlineFmt; }
        auto it = g_tagFrozenCache.find(key);
        if (it != g_tagFrozenCache.end()) { out = it->second; return true; }
    }
    if (def->resolve) {
        time_t nowT = time(nullptr);
        struct tm resolverTm{};
        localtime_s(&resolverTm, &nowT);
        out = def->resolve(resolverTm);
        if (g_textTagMode == 0) g_tagFrozenCache[key] = out;
        return true;
    }
    std::wstring fmt;
    if (hasInlineFmt) {
        fmt = inlineFmt;
    } else if (def->slot == TagFmtSlot::Time) {
        fmt = g_textTagTimeFmt;
    } else if (def->slot == TagFmtSlot::Day) {
        fmt = g_textTagDayFmt;
    }
    if (fmt.empty()) fmt = def->defaultFmt;
    if (fmt.empty()) { out.clear(); return true; }
    fmt = NormalizeStrftimeAliases(fmt);
    std::locale prevLocale = std::locale::global(std::locale("C"));
    time_t now = time(nullptr);
    struct tm tmv{};
    localtime_s(&tmv, &now);
    wchar_t buf[128];
    buf[0] = 0;
    size_t n = wcsftime(buf, 128, fmt.c_str(), &tmv);
    std::locale::global(prevLocale);
    if (n == 0) {
        buf[0] = 0;
        Wh_Log(L"[TextTag] unsupported strftime format for [%ls]: \"%ls\" -> empty", def->id, fmt.c_str());
    }
    out.assign(buf);
    if (g_textTagMode == 0) g_tagFrozenCache[key] = out;
    return true;
}
static std::wstring ExpandTextTags(const std::wstring& in) {
    std::wstring out;
    for (size_t i = 0; i < in.size();) {
        wchar_t c = in[i];
        if (c == L'\\' && i + 1 < in.size() && in[i + 1] == L'[') {
            out.push_back(L'[');
            i += 2;
            continue;
        }
        if (c != L'[') { out.push_back(c); ++i; continue; }
        size_t close = in.find(L']', i + 1);
        if (close == std::wstring::npos) { out.push_back(c); ++i; continue; }
        std::wstring body = in.substr(i + 1, close - i - 1);
        if (body.empty()) { out.push_back(c); ++i; continue; }
        std::wstring id = body;
        bool hasInline = false;
        std::wstring inlineFmt;
        size_t colon = body.find(L':');
        if (colon != std::wstring::npos) {
            id = body.substr(0, colon);
            inlineFmt = body.substr(colon + 1);
            hasInline = true;
        }
        std::wstring val;
        if (EvalTextTag(id, hasInline, inlineFmt, val)) {
            out += val;
        } else {
            out += L'['; out += body; out += L']';
        }
        i = close + 1;
    }
    return out;
}
static std::vector<std::wstring> g_lastExpandedPhrases;
static int TextCodePointLen(const std::wstring &s) {
    int n = 0;
    for (size_t k = 0; k < s.size();) {
        wchar_t c = s[k];
        k += (c >= 0xD800 && c <= 0xDBFF && k + 1 < s.size()) ? 2 : 1;
        n++;
    }
    return n;
}
static std::vector<std::wstring> TextSplitChunks(const std::wstring &s, int maxCP) {
    std::vector<std::wstring> out;
    size_t start = 0, k = 0; int cp = 0;
    while (k < s.size()) {
        wchar_t c = s[k];
        size_t adv = (c >= 0xD800 && c <= 0xDBFF && k + 1 < s.size()) ? 2 : 1;
        k += adv; cp++;
        if (cp >= maxCP) { out.push_back(s.substr(start, k - start)); start = k; cp = 0; }
    }
    if (start < s.size()) out.push_back(s.substr(start));
    return out;
}
static void SplitAndExpandPhrases(std::vector<std::wstring> &out) {
    out.clear();
    wchar_t buffer[256];
    int bi = 0;
    for (int i = 0; g_textContent[i] != 0 && i < 255; i++) {
        wchar_t ch = g_textContent[i];
        if (ch == L',' || ch == L'，' || ch == L'|' || ch == L'｜') {
            if (bi > 0) { buffer[bi] = 0; out.push_back(buffer); bi = 0; }
            continue;
        }
        if (bi < 255) buffer[bi++] = ch;
    }
    if (bi > 0) { buffer[bi] = 0; out.push_back(buffer); }
    if (out.empty()) out.push_back(L" ");
    for (auto &ph : out) ph = ExpandTextTags(ph);
}
static void BuildCellsFromPhrases(const std::vector<std::wstring> &phrases,
                                  std::vector<std::wstring> &cells,
                                  std::vector<int> &chunkStart, std::vector<int> &chunkCount,
                                  int &maxPhraseLen) {
    cells.clear(); chunkStart.clear(); chunkCount.clear();
    maxPhraseLen = 1;
    for (const auto &ph : phrases) {
        int cellStart = (int)cells.size();
        chunkStart.push_back(cellStart);
        if (g_enableTextChunk && TextCodePointLen(ph) > g_textChunkMax) {
            std::vector<std::wstring> chunks = TextSplitChunks(ph, g_textChunkMax);
            for (auto &ck : chunks) cells.push_back(ck);
        } else {
            cells.push_back(ph);
        }
        chunkCount.push_back((int)cells.size() - cellStart);
    }
    maxPhraseLen = 1;
    for (const auto &s : cells) { int l = TextCodePointLen(s); if (l > maxPhraseLen) maxPhraseLen = l; }
}
static void ConsumeTagCacheInvalidation() {
    if (!g_tagCacheDirty.exchange(false)) return;
    g_tagFrozenCache.clear();
    g_lastExpandedPhrases.clear();
}
enum class RedrawOutcome {
    NoChange,
    Redrawn,
    NeedRebuild,
};
static void ReleaseCharAtlasTarget() {
    if (g_pD2DDC && g_pCharAtlasBmp) {
        ID2D1Image *pCur = nullptr;
        g_pD2DDC->GetTarget(&pCur);
        if (pCur) {
            bool same = (pCur == static_cast<ID2D1Image*>(g_pCharAtlasBmp));
            pCur->Release();
            if (same) g_pD2DDC->SetTarget(nullptr);
        }
    }
    if (g_pCharAtlasBmp) { g_pCharAtlasBmp->Release(); g_pCharAtlasBmp = nullptr; }
    if (g_pCharAtlasSurface) { g_pCharAtlasSurface->Release(); g_pCharAtlasSurface = nullptr; }
}
static void RedrawCharAtlas() {
    if (!g_pD2DDC || !g_pCharAtlasBmp || !g_pTextFormat) return;
    if (!g_pSolidOuterBrush) { Wh_Log(L"[CharAtlas] g_pSolidOuterBrush null, redraw aborted"); return; }
    ID2D1Image *pOldTarget = nullptr;
    g_pD2DDC->GetTarget(&pOldTarget);
    g_pD2DDC->SetTarget(g_pCharAtlasBmp);
    g_pD2DDC->BeginDraw();
    g_pD2DDC->Clear(D2D1::ColorF(0, 0, 0, 0));
    g_pSolidOuterBrush->SetColor(D2D1::ColorF(1, 1, 1, 1));
    g_pSolidOuterBrush->SetOpacity(1.0f);
    for (int i = 0; i < (int)g_atlasChars.size(); i++) {
        int row = i / g_charAtlasCols;
        int col = i % g_charAtlasCols;
        float cellX = (float)(col * g_charAtlasCellW);
        float cellY = (float)(row * g_charAtlasCellH);
        D2D1_RECT_F rc = { cellX, cellY, cellX + (float)g_charAtlasCellW, cellY + (float)g_charAtlasCellH };
        g_pD2DDC->DrawText(g_atlasChars[i].c_str(), (UINT32)g_atlasChars[i].length(),
            g_pTextFormat, &rc, g_pSolidOuterBrush,
            D2D1_DRAW_TEXT_OPTIONS_ENABLE_COLOR_FONT);
    }
    HRESULT hrDraw = g_pD2DDC->EndDraw();
    g_pD2DDC->SetTarget(pOldTarget);
    if (pOldTarget) pOldTarget->Release();
    if (FAILED(hrDraw)) Wh_Log(L"[CharAtlas] EndDraw failed: 0x%08X", hrDraw);
}
static void BuildCharAtlas() {
    Wh_Log(L"[CharAtlas] BuildCharAtlas called, dev=%p factory=%p fmt=%p", g_pD3DDevice, g_pD2DFactory, g_pTextFormat);
    float effFontSize = g_dpiScaleText ? (float)g_textFontSize * g_monitorDpiScale : (float)g_textFontSize;
    if (effFontSize < 1.0f) effFontSize = 1.0f;
    if (g_pTextFormat) { g_pTextFormat->Release(); g_pTextFormat = nullptr; }
    if (g_dwFactory) {
        g_dwFactory->CreateTextFormat(L"Segoe UI Emoji", nullptr, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL,
            (float)(effFontSize * 2.0f), L"en-us", &g_pTextFormat);
        if (!g_pTextFormat) g_dwFactory->CreateTextFormat(L"Segoe UI", nullptr, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL,
            (float)(effFontSize * 2.0f), L"en-us", &g_pTextFormat);
        if (g_pTextFormat) {
            g_pTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
            g_pTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
        }
    }
    if (!g_pD3DDevice || !g_pD2DFactory || !g_pTextFormat) { Wh_Log(L"[CharAtlas] early return null deps"); return; }
    ConsumeTagCacheInvalidation();
    std::vector<std::wstring> phrases;
    SplitAndExpandPhrases(phrases);
    g_lastExpandedPhrases = phrases;
    int maxPhraseLen = 1;
    BuildCellsFromPhrases(phrases, g_atlasChars, g_phraseChunkStart, g_phraseChunkCount, maxPhraseLen);
    g_globalChunkClock = GetTickCount();
    int phraseCount = (int)g_atlasChars.size();
    if (phraseCount <= 0) {
        g_charAtlasCols = 0;
        g_charAtlasRows = 0;
        g_charAtlasDirty = false;
        return;
    }
    g_charAtlasCols = 16;
    if (phraseCount < g_charAtlasCols) g_charAtlasCols = phraseCount;
    g_charAtlasRows = ((int)g_atlasChars.size() + g_charAtlasCols - 1) / g_charAtlasCols;
    if (g_charAtlasRows < 1) g_charAtlasRows = 1;
    float SS = g_textAtlasSS;
    SS = ClampF(SS, 1.0f, 4.0f);
    g_charAtlasCellW = (int)(effFontSize * maxPhraseLen * SS + 16);
    g_charAtlasCellH = (int)(effFontSize * SS + 8);
    g_textAspectRatio = (g_charAtlasCellH > 0) ? (float)g_charAtlasCellW / (float)g_charAtlasCellH : 1.0f;
    UINT atlasW = (UINT)(g_charAtlasCols * g_charAtlasCellW);
    UINT atlasH = (UINT)(g_charAtlasRows * g_charAtlasCellH);
    const UINT maxTexDim = D3D11_REQ_TEXTURE2D_U_OR_V_DIMENSION;
    while ((atlasW > maxTexDim || atlasH > maxTexDim) && SS > 1.0f) {
        SS -= 0.5f;
        g_charAtlasCellW = (int)(effFontSize * maxPhraseLen * SS + 16);
        g_charAtlasCellH = (int)(effFontSize * SS + 8);
        g_textAspectRatio = (g_charAtlasCellH > 0) ? (float)g_charAtlasCellW / (float)g_charAtlasCellH : 1.0f;
        atlasW = (UINT)(g_charAtlasCols * g_charAtlasCellW);
        atlasH = (UINT)(g_charAtlasRows * g_charAtlasCellH);
    }
    ReleaseCharAtlasTarget();
    if (g_pCharAtlasTex) { g_pCharAtlasTex->Release(); g_pCharAtlasTex = nullptr; }
    if (g_pCharAtlasSRV) { g_pCharAtlasSRV->Release(); g_pCharAtlasSRV = nullptr; }
    D3D11_TEXTURE2D_DESC texDesc = {};
    texDesc.Width = atlasW;
    texDesc.Height = atlasH;
    texDesc.MipLevels = 1;
    texDesc.ArraySize = 1;
    texDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    texDesc.SampleDesc.Count = 1;
    texDesc.Usage = D3D11_USAGE_DEFAULT;
    texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
    texDesc.CPUAccessFlags = 0;
    if (FAILED(g_pD3DDevice->CreateTexture2D(&texDesc, nullptr, &g_pCharAtlasTex))) { Wh_Log(L"[CharAtlas] CreateTexture2D FAILED"); return; }
    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Format = texDesc.Format;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = 1;
    if (FAILED(g_pD3DDevice->CreateShaderResourceView(g_pCharAtlasTex, &srvDesc, &g_pCharAtlasSRV))) { Wh_Log(L"[CharAtlas] CreateSRV FAILED");
        g_pCharAtlasTex->Release(); g_pCharAtlasTex = nullptr; return;
    }
    if (FAILED(g_pCharAtlasTex->QueryInterface(__uuidof(IDXGISurface), (void**)&g_pCharAtlasSurface))) {
        if (g_pCharAtlasSRV) { g_pCharAtlasSRV->Release(); g_pCharAtlasSRV = nullptr; }
        Wh_Log(L"[CharAtlas] QueryInterface(IDXGISurface) FAILED");
        return;
    }
    D2D1_BITMAP_PROPERTIES1 bmpProps = D2D1::BitmapProperties1(
        D2D1_BITMAP_OPTIONS_TARGET,
        D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED));
    if (FAILED(g_pD2DDC->CreateBitmapFromDxgiSurface(g_pCharAtlasSurface, &bmpProps, &g_pCharAtlasBmp))) {
        Wh_Log(L"[CharAtlas] CreateBitmapFromDxgiSurface FAILED");
        ReleaseCharAtlasTarget();
        if (g_pCharAtlasSRV) { g_pCharAtlasSRV->Release(); g_pCharAtlasSRV = nullptr; }
        return;
    }
    RedrawCharAtlas();
    Wh_Log(L"[CharAtlas] built OK, atlasW=%d atlasH=%d phrases=%d cols=%d rows=%d cellW=%d", atlasW, atlasH, (int)g_atlasChars.size(), g_charAtlasCols, g_charAtlasRows, g_charAtlasCellW);
}
static RedrawOutcome UpdateCharAtlasTagsImpl() {
    if (g_textTagMode == 0) return RedrawOutcome::NoChange;
    if (!g_pD2DDC || !g_pCharAtlasBmp || !g_pSolidOuterBrush || g_atlasChars.empty())
        return RedrawOutcome::NoChange;
    std::vector<std::wstring> phrases;
    SplitAndExpandPhrases(phrases);
    if (phrases == g_lastExpandedPhrases) return RedrawOutcome::NoChange;
    int maxPhraseLen = 1;
    std::vector<std::wstring> cells;
    std::vector<int> cStart, cCount;
    BuildCellsFromPhrases(phrases, cells, cStart, cCount, maxPhraseLen);
    int curMax = 1;
    for (const auto &s : g_atlasChars) { int l = TextCodePointLen(s); if (l > curMax) curMax = l; }
    if (cells.size() != g_atlasChars.size() || maxPhraseLen != curMax)
        return RedrawOutcome::NeedRebuild;
    g_atlasChars.swap(cells);
    g_phraseChunkStart.swap(cStart);
    g_phraseChunkCount.swap(cCount);
    g_lastExpandedPhrases = phrases;
    RedrawCharAtlas();
    return RedrawOutcome::Redrawn;
}
static void UpdateCharAtlasTags() {
    if (g_particleShape != 10) return;
    ConsumeTagCacheInvalidation();
    if (g_textTagMode == 0) return;
    static DWORD s_lastTagCheckMs = 0;
    DWORD now = GetTickCount();
    if (now - s_lastTagCheckMs < 250) return;
    s_lastTagCheckMs = now;
    RedrawOutcome oc = UpdateCharAtlasTagsImpl();
    if (oc == RedrawOutcome::NeedRebuild) {
        BuildCharAtlas();
    }
}
static void ReleaseNativeRendering();
static bool CompileShader(const char* source, const char* entry, const char* target, ID3DBlob** blob) {
    ID3DBlob* error = nullptr;
    HRESULT hr = D3DCompile(source, strlen(source), nullptr, nullptr, nullptr, entry, target,
                            D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_OPTIMIZATION_LEVEL3, 0, blob, &error);
    if (FAILED(hr)) {
        if (error) {
            Wh_Log(L"NativeD3D: shader compile failed: %S", (char*)error->GetBufferPointer());
            error->Release();
        }
        return false;
    }
    if (error) error->Release();
    return true;
}
static bool NeedPostChain() {
    if (g_enableBloom) return true;
    if (g_enableTemporalBlur) return true;
    if (g_postAA != 0) return true;
    return false;
}
static bool NeedHDREncode() {
    return g_hdrColorSpaceApplied;
}
static void ReleasePostTargets() {
    if (g_pSceneSRV) { g_pSceneSRV->Release(); g_pSceneSRV = nullptr; }
    if (g_pSceneRTV) { g_pSceneRTV->Release(); g_pSceneRTV = nullptr; }
    if (g_pSceneTex) { g_pSceneTex->Release(); g_pSceneTex = nullptr; }
    if (g_pPostSRV) { g_pPostSRV->Release(); g_pPostSRV = nullptr; }
    if (g_pPostRTV) { g_pPostRTV->Release(); g_pPostRTV = nullptr; }
    if (g_pPostTex) { g_pPostTex->Release(); g_pPostTex = nullptr; }
    if (g_pHistorySRV) { g_pHistorySRV->Release(); g_pHistorySRV = nullptr; }
    if (g_pHistoryRTV) { g_pHistoryRTV->Release(); g_pHistoryRTV = nullptr; }
    if (g_pHistoryTex) { g_pHistoryTex->Release(); g_pHistoryTex = nullptr; }
    for (int i = 0; i < kPostBloomLevels; i++) {
        if (g_pBloomSRV[i]) { g_pBloomSRV[i]->Release(); g_pBloomSRV[i] = nullptr; }
        if (g_pBloomRTV[i]) { g_pBloomRTV[i]->Release(); g_pBloomRTV[i] = nullptr; }
        if (g_pBloomTex[i]) { g_pBloomTex[i]->Release(); g_pBloomTex[i] = nullptr; }
    }
    g_postRTW = 0;
    g_postRTH = 0;
    g_postRTFormat = 0;
    g_postRTsFresh = false;
}
static bool CreatePostColorTarget(int w, int h, DXGI_FORMAT fmt,
                                  ID3D11Texture2D **tex, ID3D11RenderTargetView **rtv,
                                  ID3D11ShaderResourceView **srv) {
    if (!g_pD3DDevice || w < 1 || h < 1) return false;
    D3D11_TEXTURE2D_DESC d = {};
    d.Width = (UINT)w;
    d.Height = (UINT)h;
    d.MipLevels = 1;
    d.ArraySize = 1;
    d.Format = fmt;
    d.SampleDesc.Count = 1;
    d.SampleDesc.Quality = 0;
    d.Usage = D3D11_USAGE_DEFAULT;
    d.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
    d.MiscFlags = 0;
    if (FAILED(g_pD3DDevice->CreateTexture2D(&d, nullptr, tex)) || !*tex) return false;
    if (FAILED(g_pD3DDevice->CreateRenderTargetView(*tex, nullptr, rtv)) || !*rtv) return false;
    if (FAILED(g_pD3DDevice->CreateShaderResourceView(*tex, nullptr, srv)) || !*srv) return false;
    return true;
}
static bool EnsurePostTargets(int w, int h) {
    if (!g_pD3DDevice) return false;
    bool needChain = NeedPostChain();
    bool needHDR = NeedHDREncode();
    if (!needChain && !needHDR) {
        if (g_postRTFormat != 0) ReleasePostTargets();
        return false;
    }
    if (w < 1 || h < 1) return false;
    bool needHist = g_enableTemporalBlur;
    bool needBloom = g_enableBloom;
    bool missing = (!g_pSceneTex || !g_pSceneRTV || !g_pSceneSRV)
                || (needChain && (!g_pPostTex || !g_pPostRTV || !g_pPostSRV))
                || (needHist && (!g_pHistoryTex || !g_pHistoryRTV || !g_pHistorySRV));
    if (needBloom && !missing) {
        for (int i = 0; i < kPostBloomLevels; i++) {
            if (!g_pBloomTex[i] || !g_pBloomRTV[i] || !g_pBloomSRV[i]) { missing = true; break; }
        }
    }
    bool sameTarget = (g_postRTW == w && g_postRTH == h && g_postRTFormat == (int)g_swapChainFormat);
    if (sameTarget && !missing) return true;
    ReleasePostTargets();
    if (!CreatePostColorTarget(w, h, g_swapChainFormat, &g_pSceneTex, &g_pSceneRTV, &g_pSceneSRV)) {
        Wh_Log(L"PostChain: scene target creation failed");
        ReleasePostTargets();
        return false;
    }
    if (needChain) {
        if (!CreatePostColorTarget(w, h, g_swapChainFormat, &g_pPostTex, &g_pPostRTV, &g_pPostSRV)) {
            Wh_Log(L"PostChain: ping-pong target creation failed");
            ReleasePostTargets();
            return false;
        }
    }
    if (needHist) {
        if (!CreatePostColorTarget(w, h, g_swapChainFormat, &g_pHistoryTex, &g_pHistoryRTV, &g_pHistorySRV)) {
            Wh_Log(L"PostChain: history target creation failed");
            ReleasePostTargets();
            return false;
        }
    }
    if (needBloom) {
        int hw = w / 2; if (hw < 1) hw = 1;
        int hh = h / 2; if (hh < 1) hh = 1;
        int qw = w / 4; if (qw < 1) qw = 1;
        int qh = h / 4; if (qh < 1) qh = 1;
        for (int i = 0; i < kPostBloomLevels; i++) {
            int bw = (i < 2) ? hw : qw;
            int bh = (i < 2) ? hh : qh;
            if (!CreatePostColorTarget(bw, bh, g_swapChainFormat, &g_pBloomTex[i], &g_pBloomRTV[i], &g_pBloomSRV[i])) {
                Wh_Log(L"PostChain: bloom target %d creation failed", i);
                ReleasePostTargets();
                return false;
            }
        }
    }
    g_postRTW = w;
    g_postRTH = h;
    g_postRTFormat = (int)g_swapChainFormat;
    g_postRTsFresh = true;
    Wh_Log(L"PostChain: targets ready (%dx%d, fmt=%d, bloom=%d, temporal=%d)",
           w, h, (int)g_swapChainFormat, (int)needBloom, (int)needHist);
    return true;
}
static void DrawPostPass(ID3D11RenderTargetView *rtv, ID3D11PixelShader *ps,
                         ID3D11ShaderResourceView *const *srvs, int srvCount,
                         ID3D11Buffer *cbBuf, const float *cbData, int cbFloats,
                         bool reuseState = false) {
    if (!g_pD3DContext || !rtv || !ps) return;
    if (!g_pBlitVB || !g_pBlitLayout || !g_pBlitVS || !g_pBlitSampler) return;
    g_pD3DContext->OMSetRenderTargets(1, &rtv, nullptr);
    g_pD3DContext->OMSetBlendState(nullptr, nullptr, 0xFFFFFFFF);
    if (!reuseState) {
        g_pD3DContext->IASetInputLayout(g_pBlitLayout);
        g_pD3DContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
        UINT stride = 16, offset = 0;
        g_pD3DContext->IASetVertexBuffers(0, 1, &g_pBlitVB, &stride, &offset);
        g_pD3DContext->VSSetShader(g_pBlitVS, nullptr, 0);
        g_pD3DContext->PSSetSamplers(0, 1, &g_pBlitSampler);
    }
    g_pD3DContext->PSSetShader(ps, nullptr, 0);
    ID3D11ShaderResourceView *bindSrv[3] = {nullptr, nullptr, nullptr};
    for (int i = 0; i < srvCount && i < 3 && srvs; i++) bindSrv[i] = srvs[i];
    g_pD3DContext->PSSetShaderResources(0, 3, bindSrv);
    if (cbBuf && cbData && cbFloats > 0) {
        D3D11_MAPPED_SUBRESOURCE m;
        if (SUCCEEDED(g_pD3DContext->Map(cbBuf, 0, D3D11_MAP_WRITE_DISCARD, 0, &m))) {
            memcpy(m.pData, cbData, (size_t)cbFloats * sizeof(float));
            g_pD3DContext->Unmap(cbBuf, 0);
        }
        g_pD3DContext->PSSetConstantBuffers(0, 1, &cbBuf);
    }
    g_pD3DContext->Draw(4, 0);
}
static void RunPostChain(ID3D11RenderTargetView *pBackRTV, int w, int h) {
    if (!g_pD3DContext || !pBackRTV || !g_pSceneRTV || !g_pSceneSRV || w < 1 || h < 1) return;
    float cb[16];
    D3D11_VIEWPORT vp = {0, 0, (float)w, (float)h, 0, 1};
    g_pD3DContext->RSSetViewports(1, &vp);
    g_pD3DContext->RSSetState(g_pRasterState);
    ID3D11ShaderResourceView *cur = g_pSceneSRV;
    bool curIsScene = true;
    bool postStateBound = false;
    auto PostPass = [&](ID3D11RenderTargetView *rtv, ID3D11PixelShader *ps,
                        ID3D11ShaderResourceView *const *srvs, int n,
                        ID3D11Buffer *cbBuf, const float *cbData, int cbFloats) {
        DrawPostPass(rtv, ps, srvs, n, cbBuf, cbData, cbFloats, postStateBound);
        postStateBound = true;
    };
    if (g_enableBloom && g_pBloomThresholdPS && g_pBloomBlurPS && g_pBloomCompositePS
        && g_pBloomRTV[0] && g_pBloomSRV[0] && g_pBloomRTV[1] && g_pBloomSRV[1]
        && g_pBloomRTV[2] && g_pBloomSRV[2] && g_pBloomRTV[3] && g_pBloomSRV[3]) {
        int hw = w / 2; if (hw < 1) hw = 1;
        int hh = h / 2; if (hh < 1) hh = 1;
        int qw = w / 4; if (qw < 1) qw = 1;
        int qh = h / 4; if (qh < 1) qh = 1;
        float thr = FxApply(g_bloomThreshold / 100.0f, g_fxBloomThrMul);
        float knee = fmaxf(thr * 0.5f, 0.02f);
        memset(cb, 0, sizeof(cb));
        cb[0] = 1.0f / (float)w;  cb[1] = 1.0f / (float)h;
        cb[2] = 1.0f / (float)hw; cb[3] = 1.0f / (float)hh;
        cb[4] = thr; cb[5] = knee;
        PostPass(g_pBloomRTV[0], g_pBloomThresholdPS, &cur, 1, g_pPostCB, cb, 16);
        memset(cb, 0, sizeof(cb));
        cb[0] = 1.0f / (float)hw; cb[1] = 1.0f / (float)hh;
        cb[4] = 1.0f; cb[5] = 0.0f;
        PostPass(g_pBloomRTV[1], g_pBloomBlurPS, &g_pBloomSRV[0], 1, g_pPostCB, cb, 16);
        cb[4] = 0.0f; cb[5] = 1.0f;
        PostPass(g_pBloomRTV[0], g_pBloomBlurPS, &g_pBloomSRV[1], 1, g_pPostCB, cb, 16);
        {
            float bd[8];
            bd[0] = 1.0f / (float)hw; bd[1] = 1.0f / (float)hh;
            bd[2] = 0.0f;
            bd[3] = 0.0f;
            bd[4] = 1.0f;
            bd[5] = 0.0f; bd[6] = 0.0f; bd[7] = 0.0f;
            PostPass(g_pBloomRTV[2], g_pBlitPS, &g_pBloomSRV[0], 1, g_pBlitCB, bd, 8);
        }
        memset(cb, 0, sizeof(cb));
        cb[0] = 1.0f / (float)qw; cb[1] = 1.0f / (float)qh;
        cb[4] = 1.0f; cb[5] = 0.0f;
        PostPass(g_pBloomRTV[3], g_pBloomBlurPS, &g_pBloomSRV[2], 1, g_pPostCB, cb, 16);
        cb[4] = 0.0f; cb[5] = 1.0f;
        PostPass(g_pBloomRTV[2], g_pBloomBlurPS, &g_pBloomSRV[3], 1, g_pPostCB, cb, 16);
        {
            ID3D11RenderTargetView *dstRTV = curIsScene ? g_pPostRTV : g_pSceneRTV;
            ID3D11ShaderResourceView *dstSRV = curIsScene ? g_pPostSRV : g_pSceneSRV;
            if (dstRTV) {
                ID3D11ShaderResourceView *srvs[3] = {cur, g_pBloomSRV[0], g_pBloomSRV[2]};
                memset(cb, 0, sizeof(cb));
                cb[4] = FxApply(g_bloomIntensity / 100.0f, g_fxBloomMul);
                PostPass(dstRTV, g_pBloomCompositePS, srvs, 3, g_pPostCB, cb, 16);
                cur = dstSRV;
                curIsScene = !curIsScene;
            }
        }
    }
    if (g_enableTemporalBlur && g_pTemporalPS && g_pHistoryRTV && g_pHistorySRV && g_pHistoryTex) {
        if (g_postRTsFresh || g_history.size() <= 1) {
            float histClear[4] = {0, 0, 0, 0};
            g_pD3DContext->ClearRenderTargetView(g_pHistoryRTV, histClear);
        }
        g_postRTsFresh = false;
        ID3D11RenderTargetView *dstRTV = curIsScene ? g_pPostRTV : g_pSceneRTV;
        ID3D11ShaderResourceView *dstSRV = curIsScene ? g_pPostSRV : g_pSceneSRV;
        if (dstRTV) {
            ID3D11ShaderResourceView *srvs[2] = {cur, g_pHistorySRV};
            memset(cb, 0, sizeof(cb));
            cb[4] = fminf(FxApply(g_temporalBlurAmount / 100.0f, g_fxBlurMul), 0.95f);
            PostPass(dstRTV, g_pTemporalPS, srvs, 2, g_pPostCB, cb, 16);
            cur = dstSRV;
            curIsScene = !curIsScene;
            float bd[8];
            bd[0] = 1.0f / (float)w; bd[1] = 1.0f / (float)h;
            bd[2] = 0.0f; bd[3] = 0.0f; bd[4] = 1.0f;
            bd[5] = 0.0f; bd[6] = 0.0f; bd[7] = 0.0f;
            PostPass(g_pHistoryRTV, g_pBlitPS, &cur, 1, g_pBlitCB, bd, 8);
        }
    }
    if (g_postAA != 0 && g_pFXAAPS) {
        ID3D11RenderTargetView *dstRTV = curIsScene ? g_pPostRTV : g_pSceneRTV;
        ID3D11ShaderResourceView *dstSRV = curIsScene ? g_pPostSRV : g_pSceneSRV;
        if (dstRTV) {
            memset(cb, 0, sizeof(cb));
            cb[2] = 1.0f / (float)w;
            cb[3] = 1.0f / (float)h;
            cb[4] = (g_postAA >= 2) ? 2.0f : 1.0f;
            PostPass(dstRTV, g_pFXAAPS, &cur, 1, g_pPostCB, cb, 16);
            cur = dstSRV;
        }
    }
    if (NeedHDREncode() && g_pHDREncodePS) {
        memset(cb, 0, sizeof(cb));
        cb[2] = 1.0f / (float)w;
        cb[3] = 1.0f / (float)h;
        cb[4] = (g_swapChainFormat == DXGI_FORMAT_R10G10B10A2_UNORM) ? 2.0f : 1.0f;
        cb[5] = fminf((g_hdrHighlightBoost / 100.0f) * g_fxHdrMul, 4.0f);
        cb[6] = (float)g_hdrTonemap;
        PostPass(pBackRTV, g_pHDREncodePS, &cur, 1, g_pPostCB, cb, 16);
    } else if (g_pBlitPS) {
        float bd[8];
        bd[0] = 1.0f / (float)w; bd[1] = 1.0f / (float)h;
        bd[2] = 0.0f; bd[3] = 0.0f; bd[4] = 1.0f;
        bd[5] = 0.0f; bd[6] = 0.0f; bd[7] = 0.0f;
        PostPass(pBackRTV, g_pBlitPS, &cur, 1, g_pBlitCB, bd, 8);
    }
    ID3D11ShaderResourceView *nullSrv[3] = {nullptr, nullptr, nullptr};
    g_pD3DContext->PSSetShaderResources(0, 3, nullSrv);
}
static bool InitNativeRendering() {
    if (!g_pD3DDevice) return false;
    ReleaseNativeRendering();
    ID3DBlob *vsBlob = nullptr, *psBlob = nullptr;
    ID3DBlob *pvsBlob = nullptr, *ppsBlob = nullptr;
    ID3DBlob *bvsBlob = nullptr, *bpsBlob = nullptr;
    bool ok = false;
    do {
        if (!CompileShader(g_vsShader, "VSMain", "vs_4_0", &vsBlob)) break;
        if (!CompileShader(g_psShader, "PSMain", "ps_4_0", &psBlob)) break;
        if (!CompileShader(g_particleVS, "VSMain", "vs_4_0", &pvsBlob)) break;
        if (!CompileShader(g_particlePS, "PSMain", "ps_4_0", &ppsBlob)) break;
        if (FAILED(g_pD3DDevice->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, &g_pNativeVS))) break;
        if (FAILED(g_pD3DDevice->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, &g_pNativePS))) break;
        if (FAILED(g_pD3DDevice->CreateVertexShader(pvsBlob->GetBufferPointer(), pvsBlob->GetBufferSize(), nullptr, &g_pParticleVS))) break;
        if (FAILED(g_pD3DDevice->CreatePixelShader(ppsBlob->GetBufferPointer(), ppsBlob->GetBufferSize(), nullptr, &g_pParticlePS))) break;
        if (!CompileShader(g_blitVS, "VSMain", "vs_4_0", &bvsBlob)) break;
        if (!CompileShader(g_blitPS, "PSMain", "ps_4_0", &bpsBlob)) break;
        if (FAILED(g_pD3DDevice->CreateVertexShader(bvsBlob->GetBufferPointer(), bvsBlob->GetBufferSize(), nullptr, &g_pBlitVS))) break;
        if (FAILED(g_pD3DDevice->CreatePixelShader(bpsBlob->GetBufferPointer(), bpsBlob->GetBufferSize(), nullptr, &g_pBlitPS))) break;
        {
            D3D11_INPUT_ELEMENT_DESC blitLayoutDesc[] = {
                {"POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
                {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 8, D3D11_INPUT_PER_VERTEX_DATA, 0},
            };
            g_pD3DDevice->CreateInputLayout(blitLayoutDesc, 2, bvsBlob->GetBufferPointer(),
                                            bvsBlob->GetBufferSize(), &g_pBlitLayout);
        }
        {
            float quadVerts[] = {
                -1.0f, -1.0f,  0.0f, 1.0f,
                 1.0f, -1.0f,  1.0f, 1.0f,
                -1.0f,  1.0f,  0.0f, 0.0f,
                 1.0f,  1.0f,  1.0f, 0.0f,
            };
            D3D11_BUFFER_DESC vbDesc = {};
            vbDesc.Usage = D3D11_USAGE_DEFAULT;
            vbDesc.ByteWidth = sizeof(quadVerts);
            vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
            D3D11_SUBRESOURCE_DATA vbInit = {};
            vbInit.pSysMem = quadVerts;
            g_pD3DDevice->CreateBuffer(&vbDesc, &vbInit, &g_pBlitVB);
        }
        {
            D3D11_SAMPLER_DESC sampDesc = {};
            sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
            sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
            sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
            sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
            sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
            g_pD3DDevice->CreateSamplerState(&sampDesc, &g_pBlitSampler);
        }
        {
            D3D11_BUFFER_DESC bcbDesc = {};
            bcbDesc.ByteWidth = 32;
            bcbDesc.Usage = D3D11_USAGE_DYNAMIC;
            bcbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
            bcbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
            g_pD3DDevice->CreateBuffer(&bcbDesc, nullptr, &g_pBlitCB);
        }
        {
            struct PostShaderDef { const char *src; ID3D11PixelShader **out; const char *name; };
            PostShaderDef defs[6] = {
                { g_postTemporalPS,       &g_pTemporalPS,       "temporal" },
                { g_postFXAAPS,           &g_pFXAAPS,           "fxaa" },
                { g_postBloomThresholdPS, &g_pBloomThresholdPS, "bloom_threshold" },
                { g_postBloomBlurPS,      &g_pBloomBlurPS,      "bloom_blur" },
                { g_postBloomCompositePS, &g_pBloomCompositePS, "bloom_composite" },
                { g_postHDREncodePS,      &g_pHDREncodePS,      "hdr_encode" },
            };
            for (int i = 0; i < 6; i++) {
                ID3DBlob *postBlob = nullptr;
                if (!CompileShader(defs[i].src, "PSMain", "ps_4_0", &postBlob)) {
                    Wh_Log(L"NativeD3D: post shader '%S' compile failed, pass disabled", defs[i].name);
                    continue;
                }
                if (FAILED(g_pD3DDevice->CreatePixelShader(postBlob->GetBufferPointer(),
                                                           postBlob->GetBufferSize(), nullptr, defs[i].out))) {
                    Wh_Log(L"NativeD3D: post shader '%S' creation failed, pass disabled", defs[i].name);
                }
                postBlob->Release();
            }
            D3D11_BUFFER_DESC pcbDesc = {};
            pcbDesc.ByteWidth = 64;
            pcbDesc.Usage = D3D11_USAGE_DYNAMIC;
            pcbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
            pcbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
            g_pD3DDevice->CreateBuffer(&pcbDesc, nullptr, &g_pPostCB);
        }
        D3D11_INPUT_ELEMENT_DESC layoutDesc[] = {
            {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
            {"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
            {"TEXCOORD", 0, DXGI_FORMAT_R32_FLOAT, 0, 28, D3D11_INPUT_PER_VERTEX_DATA, 0},
            {"TEXCOORD", 1, DXGI_FORMAT_R32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0},
        };
        if (FAILED(g_pD3DDevice->CreateInputLayout(layoutDesc, 4, vsBlob->GetBufferPointer(),
                                                   vsBlob->GetBufferSize(), &g_pNativeLayout)))
            break;
        D3D11_INPUT_ELEMENT_DESC particleLayout[] = {
            {"POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
            {"TEXCOORD", 0, DXGI_FORMAT_R32G32B32_FLOAT, 1, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1},
            {"TEXCOORD", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},
            {"TEXCOORD", 2, DXGI_FORMAT_R32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},
            {"TEXCOORD", 3, DXGI_FORMAT_R32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},
            {"TEXCOORD", 4, DXGI_FORMAT_R32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},
            {"TEXCOORD", 5, DXGI_FORMAT_R32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},
        };
        if (FAILED(g_pD3DDevice->CreateInputLayout(particleLayout, 7, pvsBlob->GetBufferPointer(),
                                                   pvsBlob->GetBufferSize(), &g_pParticleLayout)))
            break;
        D3D11_BUFFER_DESC cbDesc = {};
        cbDesc.ByteWidth = 448;
        cbDesc.Usage = D3D11_USAGE_DYNAMIC;
        cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        if (FAILED(g_pD3DDevice->CreateBuffer(&cbDesc, nullptr, &g_pConstantBuffer))) break;
        D3D11_BUFFER_DESC vbDesc = {};
        vbDesc.ByteWidth = sizeof(VertexPosColor) * kMaxTrailVertices;
        vbDesc.Usage = D3D11_USAGE_DYNAMIC;
        vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        vbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        if (FAILED(g_pD3DDevice->CreateBuffer(&vbDesc, nullptr, &g_pTrailVB))) break;
        float quadVerts[] = {
            -1, -1,  1, -1,  -1, 1,
             1, -1,  1,  1,  -1, 1,
        };
        D3D11_BUFFER_DESC quadDesc = {};
        quadDesc.ByteWidth = sizeof(quadVerts);
        quadDesc.Usage = D3D11_USAGE_IMMUTABLE;
        quadDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        D3D11_SUBRESOURCE_DATA quadData = {quadVerts, 0, 0};
        if (FAILED(g_pD3DDevice->CreateBuffer(&quadDesc, &quadData, &g_pParticleQuadVB))) break;
        D3D11_BUFFER_DESC instDesc = {};
        instDesc.ByteWidth = sizeof(ParticleInstance) * 4000;
        instDesc.Usage = D3D11_USAGE_DYNAMIC;
        instDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        instDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        if (FAILED(g_pD3DDevice->CreateBuffer(&instDesc, nullptr, &g_pParticleInstanceBuf))) break;
        D3D11_BLEND_DESC blendDesc = {};
        blendDesc.RenderTarget[0].BlendEnable = TRUE;
        blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
        blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
        blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
        blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
        blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
        blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
        blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
        if (FAILED(g_pD3DDevice->CreateBlendState(&blendDesc, &g_pAlphaBlend))) break;
        D3D11_BLEND_DESC addBlendDesc = {};
        addBlendDesc.RenderTarget[0].BlendEnable = TRUE;
        addBlendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
        addBlendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
        addBlendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
        addBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
        addBlendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
        addBlendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
        addBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
        if (FAILED(g_pD3DDevice->CreateBlendState(&addBlendDesc, &g_pAdditiveBlend))) break;
        D3D11_RASTERIZER_DESC rastDesc = {};
        rastDesc.FillMode = D3D11_FILL_SOLID;
        rastDesc.CullMode = D3D11_CULL_NONE;
        rastDesc.DepthClipEnable = FALSE;
        rastDesc.MultisampleEnable = TRUE;
        if (FAILED(g_pD3DDevice->CreateRasterizerState(&rastDesc, &g_pRasterState))) break;
        D3D11_SAMPLER_DESC samplerDesc = {};
        samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
        samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
        samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
        samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
        samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
        samplerDesc.MinLOD = 0;
        samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
        if (FAILED(g_pD3DDevice->CreateSamplerState(&samplerDesc, &g_pCharAtlasSampler))) break;
        ok = true;
    } while (false);
    if (vsBlob) vsBlob->Release();
    if (psBlob) psBlob->Release();
    if (pvsBlob) pvsBlob->Release();
    if (ppsBlob) ppsBlob->Release();
    if (bvsBlob) bvsBlob->Release();
    if (bpsBlob) bpsBlob->Release();
    if (!ok) {
        Wh_Log(L"NativeD3D: init failed, releasing partial resources");
        ReleaseNativeRendering();
        return false;
    }
    Wh_Log(L"NativeD3D: initialized successfully");
    BuildCharAtlas();
    if (g_cachedVW > 0 && g_cachedVH > 0) EnsurePostTargets(g_cachedVW, g_cachedVH);
    return true;
}
static void ReleaseNativeRendering() {
    if (g_pNativeVS) { g_pNativeVS->Release(); g_pNativeVS = nullptr; }
    if (g_pNativePS) { g_pNativePS->Release(); g_pNativePS = nullptr; }
    if (g_pParticleVS) { g_pParticleVS->Release(); g_pParticleVS = nullptr; }
    ReleaseCharAtlasTarget();
    if (g_pCharAtlasSRV) { g_pCharAtlasSRV->Release(); g_pCharAtlasSRV = nullptr; }
    if (g_pCharAtlasTex) { g_pCharAtlasTex->Release(); g_pCharAtlasTex = nullptr; }
    if (g_pParticlePS) { g_pParticlePS->Release(); g_pParticlePS = nullptr; }
    if (g_pNativeLayout) { g_pNativeLayout->Release(); g_pNativeLayout = nullptr; }
    if (g_pParticleLayout) { g_pParticleLayout->Release(); g_pParticleLayout = nullptr; }
    if (g_pConstantBuffer) { g_pConstantBuffer->Release(); g_pConstantBuffer = nullptr; }
    InvalidateConstantBuffer();
    if (g_pTrailVB) { g_pTrailVB->Release(); g_pTrailVB = nullptr; }
    if (g_pParticleQuadVB) { g_pParticleQuadVB->Release(); g_pParticleQuadVB = nullptr; }
    if (g_pParticleInstanceBuf) { g_pParticleInstanceBuf->Release(); g_pParticleInstanceBuf = nullptr; }
    if (g_pAlphaBlend) { g_pAlphaBlend->Release(); g_pAlphaBlend = nullptr; }
    if (g_pAdditiveBlend) { g_pAdditiveBlend->Release(); g_pAdditiveBlend = nullptr; }
    if (g_pRasterState) { g_pRasterState->Release(); g_pRasterState = nullptr; }
    if (g_pCharAtlasSampler) { g_pCharAtlasSampler->Release(); g_pCharAtlasSampler = nullptr; }
    if (g_pBlitCB) { g_pBlitCB->Release(); g_pBlitCB = nullptr; }
    if (g_pTemporalPS) { g_pTemporalPS->Release(); g_pTemporalPS = nullptr; }
    if (g_pFXAAPS) { g_pFXAAPS->Release(); g_pFXAAPS = nullptr; }
    if (g_pBloomThresholdPS) { g_pBloomThresholdPS->Release(); g_pBloomThresholdPS = nullptr; }
    if (g_pBloomBlurPS) { g_pBloomBlurPS->Release(); g_pBloomBlurPS = nullptr; }
    if (g_pBloomCompositePS) { g_pBloomCompositePS->Release(); g_pBloomCompositePS = nullptr; }
    if (g_pHDREncodePS) { g_pHDREncodePS->Release(); g_pHDREncodePS = nullptr; }
    if (g_pPostCB) { g_pPostCB->Release(); g_pPostCB = nullptr; }
    ReleasePostTargets();
}
static float SnapshotBgLuminance() {
    float snap = 0.5f;
    memcpy(&snap, (const void*)&g_bgLuminance, sizeof(snap));
    return snap;
}
static float ResolveNoiseSeed(bool contentUnchanged, float lastSeed) {
    static unsigned int s_cbFrameSeq = 0;
    if (contentUnchanged) return lastSeed;
    ++s_cbFrameSeq;
    return (float)(s_cbFrameSeq & 255u);
}
static void UpdateConstantBuffer(int width, int height, const GradData* cols = nullptr, bool force = false) {
    if (!g_pConstantBuffer || !g_pD3DContext) return;
    ShaderConstants buf = {};
    memset(&buf, 0, sizeof(buf));
    buf.screenSize[0] = (float)width;
    buf.screenSize[1] = (float)height;
    buf.perspective = g_enable25DEffect ? (float)g_perspectiveStrength / 100.0f : 0.0f;
    buf.pad0 = 0.0f;
    int gradCount = 0;
    if (cols) {
        for (int i = 0; i < GRAD_STOPS; i++) {
            buf.gradient[i][0] = cols->outer[i].color.r;
            buf.gradient[i][1] = cols->outer[i].color.g;
            buf.gradient[i][2] = cols->outer[i].color.b;
            buf.gradient[i][3] = cols->outer[i].color.a;
        }
        gradCount = GRAD_STOPS;
    }
    buf.gradientCount = gradCount;
    buf.bgLuminance = SnapshotBgLuminance();
    buf.adaptiveFlag = g_adaptiveContrast ? 1.0f : 0.0f;
    buf.smoothFlag = g_enableSmoothGradient ? 1.0f : 0.0f;
    buf.edgeSoftness = FxApply(g_edgeSoftness / 100.0f, g_fxEdgeMul);
    buf.atlasRows = (float)(g_charAtlasRows > 0 ? g_charAtlasRows : 16);
    buf.atlasCols = (float)(g_charAtlasCols > 0 ? g_charAtlasCols : 16);
    buf.aaMode = (float)g_aaMode;
    buf.textAspect = g_textAspectRatio;
    {
        float atlasWpx = (float)((g_charAtlasCols > 0 ? g_charAtlasCols : 16) * (g_charAtlasCellW > 0 ? g_charAtlasCellW : 64));
        float atlasHpx = (float)((g_charAtlasRows > 0 ? g_charAtlasRows : 16) * (g_charAtlasCellH > 0 ? g_charAtlasCellH : 64));
        float sw = (float)(g_particleStrokeWidth > 0 ? g_particleStrokeWidth : 0);
        buf.strokeStepU = sw / atlasWpx;
        buf.strokeStepV = sw / atlasHpx;
        buf.strokeEnabled = (g_enableParticleStroke && sw > 0.0f) ? 1.0f : 0.0f;
        buf.strokeAlphaMul = g_particleStrokeAlpha / 100.0f;
        buf.strokeColorMode = (float)g_particleStrokeColorMode;
        buf.strokeColorR = g_particleStrokeColor.r;
        buf.strokeColorG = g_particleStrokeColor.g;
        buf.strokeColorB = g_particleStrokeColor.b;
        buf.strokeColorMul = 0.30f;
        buf.strokeWidthPx = g_particleStrokeWidth * 0.5f;
        buf.trailStrokeEnabled = g_enableTrailStroke ? 1.0f : 0.0f;
        buf.trailStrokeHalf = FxApply((float)(g_trailStrokeWidth > 0 ? g_trailStrokeWidth : 0) / 100.0f, g_fxStrokeMul);
        buf.shaderQuality = (float)g_shaderQuality;
        buf.sdfSamples = (float)(g_shaderQuality > 0 ? g_sdfSamples : 1);
        buf.ditherMode = (float)g_ditherMode;
        buf.ditherStrength = FxApply(g_ditherStrength / 100.0f, g_fxDitherMul);
        buf.pixelScale = g_isHDRMode ? 0.0f : (1.0f / 255.0f);
        buf.pad3 = 0.0f;
    }
    {
        const size_t headBytes = (size_t)kCbSeedFloatIndex * sizeof(float);
        const size_t tailBytes = sizeof(buf) - headBytes - sizeof(float);
        const char* pNew = reinterpret_cast<const char*>(&buf);
        const char* pOld = reinterpret_cast<const char*>(&g_cbCache);
        bool contentUnchanged = g_cbCacheValid &&
            memcmp(pNew, pOld, headBytes) == 0 &&
            memcmp(pNew + headBytes + sizeof(float), pOld + headBytes + sizeof(float), tailBytes) == 0;
        buf.noiseSeed = ResolveNoiseSeed(contentUnchanged, g_cbCache.noiseSeed);
    }
    if (!force && g_cbCacheValid && memcmp(&buf, &g_cbCache, sizeof(buf)) == 0)
        return;
    D3D11_MAPPED_SUBRESOURCE mapped;
    if (SUCCEEDED(g_pD3DContext->Map(g_pConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped))) {
        memcpy(mapped.pData, &buf, sizeof(buf));
        g_pD3DContext->Unmap(g_pConstantBuffer, 0);
        memcpy(&g_cbCache, &buf, sizeof(buf));
        g_cbCacheValid = true;
    }
}
static void SetNativeRenderState(int screenW, int screenH, D3D11_PRIMITIVE_TOPOLOGY topology, const GradData* cols);
static void NativeRenderParticles(int screenW, int screenH) {
    if (!g_pParticleVS || !g_pParticlePS || !g_pParticleInstanceBuf || g_particles.empty()) return;
    bool anyVortexActive = false;
    for (int i = 0; i < g_vortexMaxCount; i++) {
        if (g_vortices[i].active && g_vortices[i].strength > 0.1f) { anyVortexActive = true; break; }
    }
    bool depthSorted = anyVortexActive && g_particles.size() < 800;
    static std::vector<int> s_drawOrder;
    if (depthSorted) {
        s_drawOrder.resize(g_particles.size());
        for (size_t i = 0; i < g_particles.size(); i++) s_drawOrder[i] = (int)i;
        std::sort(s_drawOrder.begin(), s_drawOrder.end(), [](int ia, int ib) {
            return g_particles[ia].z > g_particles[ib].z;
        });
    }
    DWORD now = GetTickCount();
    const int MAX_PER_LAYER = 2000;
    D3D11_MAPPED_SUBRESOURCE mapped;
    if (FAILED(g_pD3DContext->Map(g_pParticleInstanceBuf, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped))) return;
    ParticleInstance* instances = (ParticleInstance*)mapped.pData;
    int glowCount = 0, normalCount = 0;
    float gi = g_particleGlowIntensity / 100.0f;
    float glowSize = 1.6f + gi * 0.8f;
    float glowAlpha = 0.2f + gi * 0.2f;
    float glowColorMul = 1.2f + gi * 0.3f;
    float sizeMul = g_particleSizeMultiplier / 100.0f;
    const float musicSizeBoost = g_enableMusicPhysics ? (1.0f + g_musicBassSizeBoost * 0.5f) : 1.0f;
    const size_t nDraw = g_particles.size();
    bool anyText = false;
    for (size_t di = 0; di < nDraw; di++) {
        Particle &p = g_particles[depthSorted ? (size_t)s_drawOrder[di] : di];
        float progress = (float)(now - p.startTime) / p.lifetime;
        if (progress < 0 || progress >= 1) continue;
        float lifeAlpha = (1.0f - progress);
        float colorFade = progress * progress;
        float rOff = p.colorOffset[0] + p.vortexBrightness[0];
        float gOff = p.colorOffset[1] + p.vortexBrightness[1];
        float bOff = p.colorOffset[2] + p.vortexBrightness[2];
        D2D1_COLOR_F pc = D2D1::ColorF(
            p.color.r + (p.endColor.r - p.color.r) * colorFade + rOff,
            p.color.g + (p.endColor.g - p.color.g) * colorFade + gOff,
            p.color.b + (p.endColor.b - p.color.b) * colorFade + bOff, 1.0f);
        float sizeScale = sinf(progress * 3.14159f) * 0.7f + 0.3f;
        float baseSize;
        if (p.shapeType == 10) {
            anyText = true;
            baseSize = (float)g_textFontSize * 0.5f * sizeMul * sizeScale * musicSizeBoost;
        } else {
            baseSize = p.size * 2.0f * sizeMul * sizeScale * musicSizeBoost;
        }
        if (g_enableParticleGlow && glowCount < MAX_PER_LAYER) {
            ParticleInstance& gl = instances[glowCount++];
            gl.x = p.x; gl.y = p.y; gl.z = p.z;
            float glowSizeUse = glowSize, glowAlphaUse = glowAlpha, glowMulUse = glowColorMul;
            if (p.shapeType == 10) {
                glowSizeUse = 1.25f + gi * 0.35f;
                glowAlphaUse = 0.10f + gi * 0.12f;
                glowMulUse = 1.0f;
            }
            gl.r = pc.r * glowMulUse; gl.g = pc.g * glowMulUse; gl.b = pc.b * glowMulUse;
            gl.a = lifeAlpha * 0.8f * glowAlphaUse;
            gl.size = baseSize * glowSizeUse;
            gl.shapeType = (float)p.shapeType;
            gl.rotation = p.rotation;
            gl.charIndex = p.charIndex;
        }
        if (normalCount < MAX_PER_LAYER) {
            ParticleInstance& nm = instances[MAX_PER_LAYER + normalCount++];
            nm.x = p.x; nm.y = p.y; nm.z = p.z;
            nm.r = pc.r; nm.g = pc.g; nm.b = pc.b;
            nm.a = lifeAlpha * 0.8f;
            nm.size = baseSize;
            nm.shapeType = (float)p.shapeType;
            nm.rotation = p.rotation;
            nm.charIndex = p.charIndex;
        }
    }
    g_pD3DContext->Unmap(g_pParticleInstanceBuf, 0);
    if (glowCount == 0 && normalCount == 0) return;
    UpdateConstantBuffer(screenW, screenH);
    g_pD3DContext->IASetInputLayout(g_pParticleLayout);
    g_pD3DContext->VSSetShader(g_pParticleVS, nullptr, 0);
    g_pD3DContext->PSSetShader(g_pParticlePS, nullptr, 0);
    g_pD3DContext->VSSetConstantBuffers(0, 1, &g_pConstantBuffer);
    g_pD3DContext->PSSetConstantBuffers(0, 1, &g_pConstantBuffer);
    if (anyText && g_pCharAtlasSRV) {
        g_pD3DContext->PSSetShaderResources(0, 1, &g_pCharAtlasSRV);
        g_pD3DContext->PSSetSamplers(0, 1, &g_pCharAtlasSampler);
    }
    g_pD3DContext->RSSetState(g_pRasterState);
    g_pD3DContext->OMSetBlendState(g_pAlphaBlend, nullptr, 0xFFFFFFFF);
    UINT stride = 2 * sizeof(float);
    UINT vbOffset = 0;
    g_pD3DContext->IASetVertexBuffers(0, 1, &g_pParticleQuadVB, &stride, &vbOffset);
    stride = sizeof(ParticleInstance);
    g_pD3DContext->IASetVertexBuffers(1, 1, &g_pParticleInstanceBuf, &stride, &vbOffset);
    g_pD3DContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    if (glowCount > 0) {
        g_pD3DContext->OMSetBlendState(g_pAdditiveBlend, nullptr, 0xFFFFFFFF);
        g_pD3DContext->DrawInstanced(6, glowCount, 0, 0);
    }
    if (normalCount > 0) {
        g_pD3DContext->OMSetBlendState(g_pAlphaBlend, nullptr, 0xFFFFFFFF);
        g_pD3DContext->DrawInstanced(6, normalCount, 0, MAX_PER_LAYER);
    }
    { ID3D11ShaderResourceView *nullSRV = nullptr; g_pD3DContext->PSSetShaderResources(0, 1, &nullSRV); }
    { ID3D11SamplerState *nullSampler = nullptr; g_pD3DContext->PSSetSamplers(0, 1, &nullSampler); }
}
static int BuildNativeStrip(
    VertexPosColor* dst, int maxVtx,
    const std::vector<D2D1_POINT_2F>& pts,
    const std::vector<float>& nx, const std::vector<float>& ny,
    bool tapered,
    float widthMul,
    float widthScale,
    float constWidth,
    float r, float g, float b, float fadeAlpha, float aMul,
    float offX, float offY, bool headOnly) {
    size_t sl = pts.size();
    int count = (int)sl * 2;
    if (count > maxVtx) count = maxVtx;
    if (count < 0) count = 0;
    VertexPosColor* p = dst;
    float invSlMinus1 = (sl > 1) ? 1.0f / (float)(sl - 1) : 0.0f;
    for (int i = 0; i < count; i += 2) {
        size_t idx = (size_t)(i / 2);
        float ratio = (float)idx * invSlMinus1;
        float ow, a;
        if (tapered) {
            float taper = powf(1.0f - ratio, 1.3f);
            ow = (idx == sl - 1) ? 0.5f * widthScale : 10.0f * taper * widthMul * widthScale;
            a = fadeAlpha * aMul;
            if (headOnly)
                a *= (ratio < 0.25f) ? 0.0f : ((ratio > 0.85f) ? 1.0f : (ratio - 0.25f) / 0.6f);
        } else {
            ow = constWidth * 0.5f;
            a = fadeAlpha * (1.0f - ratio * 0.3f);
        }
        p[0] = {pts[idx].x + offX + nx[idx]*ow, pts[idx].y + offY + ny[idx]*ow, 0.0f, r, g, b, a, ratio, 1.0f};
        p[1] = {pts[idx].x + offX - nx[idx]*ow, pts[idx].y + offY - ny[idx]*ow, 0.0f, r, g, b, a, ratio, -1.0f};
        p += 2;
    }
    return count;
}
static void ComputePathNormals(const std::vector<D2D1_POINT_2F>& pts, std::vector<float>& nx, std::vector<float>& ny) {
    size_t sl = pts.size();
    nx.clear(); nx.resize(sl);
    ny.clear(); ny.resize(sl);
    for (size_t i = 0; i < sl; ++i) {
        float ddx, ddy;
        if (i == 0) { ddx = pts[1].x - pts[0].x; ddy = pts[1].y - pts[0].y; }
        else if (i == sl - 1) { ddx = pts[i].x - pts[i-1].x; ddy = pts[i].y - pts[i-1].y; }
        else { ddx = pts[i+1].x - pts[i-1].x; ddy = pts[i+1].y - pts[i-1].y; }
        float ln = sqrtf(ddx*ddx + ddy*ddy);
        if (ln > 0) { ddx /= ln; ddy /= ln; } else { ddx = 1; ddy = 0; }
        nx[i] = -ddy; ny[i] = ddx;
    }
}
static void NativeRenderLineTrail(const std::vector<D2D1_POINT_2F>& pts, float width,
                                  const GradData& cols, float fadeAlpha, int screenW, int screenH) {
    if (!g_pNativeVS || !g_pNativePS || !g_pTrailVB || pts.size() < 2) return;
    size_t sl = pts.size();
    static std::vector<float> s_nx, s_ny;
    s_nx.clear(); s_nx.resize(sl);
    s_ny.clear(); s_ny.resize(sl);
    std::vector<float>& nx = s_nx;
    std::vector<float>& ny = s_ny;
    ComputePathNormals(pts, nx, ny);
    SetNativeRenderState(screenW, screenH, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP, &cols);
    D3D11_MAPPED_SUBRESOURCE mapped;
    if (FAILED(g_pD3DContext->Map(g_pTrailVB, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped))) return;
    VertexPosColor* p = (VertexPosColor*)mapped.pData;
    int count = BuildNativeStrip(p, kMaxTrailVertices, pts, nx, ny, false, 0.0f, 0.0f, width,
                                 cols.solidOuter.r, cols.solidOuter.g, cols.solidOuter.b, fadeAlpha, 1.0f, 0, 0, false);
    g_pD3DContext->Unmap(g_pTrailVB, 0);
    g_pD3DContext->Draw(count, 0);
}
struct ShapeVertsCache { const float* verts; int count; const unsigned char* part; };
enum { kPartBody = 0, kPartFlame = 1, kPartShadow = 2, kPartHi = 3,
       kPartGlow2 = 4, kPartGlow3 = 5, kPartGlow4 = 6, kPartGlow5 = 7 };
static unsigned char g_partAllBody[72] = {0};
static int EmitPartPoly(float *v, unsigned char *p, int n, int cap,
                        const float *pts, int npts, unsigned char part) {
    if (n + (npts - 2) * 3 > cap) return n;
    for (int i = 1; i < npts - 1; i++) {
        int o = n * 2;
        v[o + 0] = pts[0];           v[o + 1] = pts[1];
        v[o + 2] = pts[i * 2];       v[o + 3] = pts[i * 2 + 1];
        v[o + 4] = pts[(i + 1) * 2]; v[o + 5] = pts[(i + 1) * 2 + 1];
        p[n] = part; p[n + 1] = part; p[n + 2] = part;
        n += 3;
    }
    return n;
}
static float ShapeLifeAlpha(float progress) {
    if (progress < 0.1f) return progress * 10.0f;
    if (progress > 0.7f) return (1.0f - progress) / 0.3f;
    return 1.0f;
}
static ShapeVertsCache GetShapeVerts(int shapeType) {
    static float circleVerts[72 * 2];
    static float starVerts[30 * 2];
    static float hexVerts[18 * 2];
    static float heartVerts[72 * 2];
    static float diamondVerts[12 * 2];
    static float triVerts[3 * 2];
    static float flowerVerts[30 * 2];
    static float pentVerts[15 * 2];
    static float hexagramVerts[36 * 2];
    static float rocketVerts[24 * 2];
    static unsigned char rocketPart[24];
    static float missileVerts[18 * 2];
    static unsigned char missilePart[18];
    static float fiberVerts[54 * 2];
    static unsigned char fiberPart[54];
    static int rocketCount = 0, missileCount = 0, fiberCount = 0;
    static bool inited = false;
    if (!inited) {
        inited = true;
        const int seg = 24;
        for (int i = 0; i < seg; i++) {
            float a1 = (i / (float)seg) * 6.28318f;
            float a2 = ((i + 1) / (float)seg) * 6.28318f;
            int idx = i * 6;
            circleVerts[idx]=0; circleVerts[idx+1]=0;
            circleVerts[idx+2]=cosf(a1); circleVerts[idx+3]=sinf(a1);
            circleVerts[idx+4]=cosf(a2); circleVerts[idx+5]=sinf(a2);
        }
        for (int i = 0; i < 5; i++) {
            float a1 = (i / 5.0f) * 6.28318f - 1.5708f;
            float a2 = ((i + 0.5f) / 5.0f) * 6.28318f - 1.5708f;
            float a3 = ((i + 1) / 5.0f) * 6.28318f - 1.5708f;
            int idx = i * 12;
            starVerts[idx]=0; starVerts[idx+1]=0;
            starVerts[idx+2]=cosf(a1); starVerts[idx+3]=sinf(a1);
            starVerts[idx+4]=0.4f*cosf(a2); starVerts[idx+5]=0.4f*sinf(a2);
            starVerts[idx+6]=0; starVerts[idx+7]=0;
            starVerts[idx+8]=0.4f*cosf(a2); starVerts[idx+9]=0.4f*sinf(a2);
            starVerts[idx+10]=cosf(a3); starVerts[idx+11]=sinf(a3);
        }
        for (int i = 0; i < 6; i++) {
            float a1 = (i / 6.0f) * 6.28318f - 1.5708f;
            float a2 = ((i + 1) / 6.0f) * 6.28318f - 1.5708f;
            int idx = i * 6;
            hexVerts[idx]=0; hexVerts[idx+1]=0;
            hexVerts[idx+2]=cosf(a1); hexVerts[idx+3]=sinf(a1);
            hexVerts[idx+4]=cosf(a2); hexVerts[idx+5]=sinf(a2);
        }
        for (int i = 0; i < seg; i++) {
            float t = (i / (float)seg) * 6.28318f;
            float t2 = ((i + 1) / (float)seg) * 6.28318f;
            float x1 = powf(sinf(t), 3);
            float y1 = -(13*cosf(t) - 5*cosf(2*t) - 2*cosf(3*t) - cosf(4*t)) / 16.0f;
            float x2 = powf(sinf(t2), 3);
            float y2 = -(13*cosf(t2) - 5*cosf(2*t2) - 2*cosf(3*t2) - cosf(4*t2)) / 16.0f;
            int idx = i * 6;
            heartVerts[idx]=0; heartVerts[idx+1]=0;
            heartVerts[idx+2]=x1; heartVerts[idx+3]=y1;
            heartVerts[idx+4]=x2; heartVerts[idx+5]=y2;
        }
        for (int i = 0; i < 4; i++) {
            float a1 = (i / 4.0f) * 6.28318f - 1.5708f;
            float a2 = ((i + 1) / 4.0f) * 6.28318f - 1.5708f;
            int idx = i * 6;
            diamondVerts[idx]=0; diamondVerts[idx+1]=0;
            diamondVerts[idx+2]=cosf(a1); diamondVerts[idx+3]=sinf(a1);
            diamondVerts[idx+4]=cosf(a2); diamondVerts[idx+5]=sinf(a2);
        }
        triVerts[0]=0; triVerts[1]=-1;
        triVerts[2]=-0.866f; triVerts[3]=0.5f;
        triVerts[4]=0.866f; triVerts[5]=0.5f;
        for (int i = 0; i < 5; i++) {
            float a1 = (i / 5.0f) * 6.28318f;
            float a2 = a1 + 0.35f;
            float a3 = a1 - 0.35f;
            int idx = i * 12;
            flowerVerts[idx]=0; flowerVerts[idx+1]=0;
            flowerVerts[idx+2]=0.7f*cosf(a2); flowerVerts[idx+3]=0.7f*sinf(a2);
            flowerVerts[idx+4]=cosf(a1); flowerVerts[idx+5]=sinf(a1);
            flowerVerts[idx+6]=0; flowerVerts[idx+7]=0;
            flowerVerts[idx+8]=0.7f*cosf(a3); flowerVerts[idx+9]=0.7f*sinf(a3);
            flowerVerts[idx+10]=cosf(a1); flowerVerts[idx+11]=sinf(a1);
        }
        for (int i = 0; i < 5; i++) {
            float a1 = (i / 5.0f) * 6.28318f - 1.5708f;
            float a2 = ((i + 1) / 5.0f) * 6.28318f - 1.5708f;
            int idx = i * 6;
            pentVerts[idx]=0; pentVerts[idx+1]=0;
            pentVerts[idx+2]=cosf(a1); pentVerts[idx+3]=sinf(a1);
            pentVerts[idx+4]=cosf(a2); pentVerts[idx+5]=sinf(a2);
        }
        for (int i = 0; i < 6; i++) {
            float a1 = (i / 6.0f) * 6.28318f - 1.5708f;
            float a2 = ((i + 0.5f) / 6.0f) * 6.28318f - 1.5708f;
            float a3 = ((i + 1) / 6.0f) * 6.28318f - 1.5708f;
            int idx = i * 12;
            hexagramVerts[idx]=0; hexagramVerts[idx+1]=0;
            hexagramVerts[idx+2]=cosf(a1); hexagramVerts[idx+3]=sinf(a1);
            hexagramVerts[idx+4]=0.5f*cosf(a2); hexagramVerts[idx+5]=0.5f*sinf(a2);
            hexagramVerts[idx+6]=0; hexagramVerts[idx+7]=0;
            hexagramVerts[idx+8]=0.5f*cosf(a2); hexagramVerts[idx+9]=0.5f*sinf(a2);
            hexagramVerts[idx+10]=cosf(a3); hexagramVerts[idx+11]=sinf(a3);
        }
        {
            const float body[] = {-0.15f,-0.30f,  0.87f,-0.30f,  0.87f,0.30f,  -0.15f,0.30f};
            const float nose[] = { 0.87f,-0.30f,  1.38f, 0.00f,  0.87f,0.30f};
            const float finU[] = {-0.15f,-0.30f,  0.21f,-0.30f, -0.30f,-0.81f};
            const float finD[] = {-0.15f, 0.30f,  0.21f, 0.30f, -0.30f, 0.81f};
            const float flame[]= {-0.21f,-0.22f, -0.21f, 0.22f, -1.05f, 0.00f};
            const float win[]  = { 0.30f, 0.00f,  0.45f,-0.15f,  0.60f, 0.00f,  0.45f,0.15f};
            int n = 0;
            n = EmitPartPoly(rocketVerts, rocketPart, n, 24, body,  4, kPartBody);
            n = EmitPartPoly(rocketVerts, rocketPart, n, 24, nose,  3, kPartBody);
            n = EmitPartPoly(rocketVerts, rocketPart, n, 24, finU,  3, kPartShadow);
            n = EmitPartPoly(rocketVerts, rocketPart, n, 24, finD,  3, kPartShadow);
            n = EmitPartPoly(rocketVerts, rocketPart, n, 24, flame, 3, kPartFlame);
            n = EmitPartPoly(rocketVerts, rocketPart, n, 24, win,   4, kPartHi);
            rocketCount = n;
        }
        {
            const float body[] = {-0.48f,-0.20f,  0.72f,-0.20f,  0.72f,0.20f, -0.48f,0.20f};
            const float nose[] = { 0.72f,-0.20f,  1.32f, 0.00f,  0.72f,0.20f};
            const float finU[] = {-0.48f,-0.20f, -0.03f,-0.20f, -0.51f,-0.66f};
            const float finD[] = {-0.48f, 0.20f, -0.03f, 0.20f, -0.51f, 0.66f};
            const float flame[]= {-0.51f,-0.17f, -0.51f, 0.17f, -1.50f, 0.00f};
            int n = 0;
            n = EmitPartPoly(missileVerts, missilePart, n, 18, body,  4, kPartBody);
            n = EmitPartPoly(missileVerts, missilePart, n, 18, nose,  3, kPartBody);
            n = EmitPartPoly(missileVerts, missilePart, n, 18, finU,  3, kPartShadow);
            n = EmitPartPoly(missileVerts, missilePart, n, 18, finD,  3, kPartShadow);
            n = EmitPartPoly(missileVerts, missilePart, n, 18, flame, 3, kPartFlame);
            missileCount = n;
        }
        {
            const float body[]  = {-0.36f,-0.18f,  0.75f,-0.18f,  0.75f,0.18f, -0.36f,0.18f};
            const float nose[]  = { 0.75f,-0.18f,  1.38f, 0.00f,  0.75f,0.18f};
            const float finU[]  = {-0.36f,-0.18f,  0.03f,-0.18f, -0.39f,-0.60f};
            const float finD[]  = {-0.36f, 0.18f,  0.03f, 0.18f, -0.39f, 0.60f};
            const float flame[] = {-0.39f,-0.15f, -0.39f, 0.15f, -1.20f, 0.00f};
            static const float kFxX[6] = {-0.45f,-1.10f,-1.75f,-2.40f,-2.85f,-3.20f};
            static const float kFxW[6] = { 0.090f, 0.075f, 0.058f, 0.040f, 0.024f, 0.012f};
            static const unsigned char kFxPart[5] = {kPartHi, kPartGlow2, kPartGlow3, kPartGlow4, kPartGlow5};
            int n = 0;
            n = EmitPartPoly(fiberVerts, fiberPart, n, 54, body,  4, kPartBody);
            n = EmitPartPoly(fiberVerts, fiberPart, n, 54, nose,  3, kPartBody);
            n = EmitPartPoly(fiberVerts, fiberPart, n, 54, finU,  3, kPartShadow);
            n = EmitPartPoly(fiberVerts, fiberPart, n, 54, finD,  3, kPartShadow);
            n = EmitPartPoly(fiberVerts, fiberPart, n, 54, flame, 3, kPartFlame);
            const float core[] = {-0.45f,-0.030f, -1.60f,-0.020f, -1.60f,0.020f, -0.45f,0.030f};
            n = EmitPartPoly(fiberVerts, fiberPart, n, 54, core, 4, kPartHi);
            for (int si = 0; si < 5; si++) {
                float quad[] = {kFxX[si],   -kFxW[si],
                                kFxX[si+1], -kFxW[si+1],
                                kFxX[si+1],  kFxW[si+1],
                                kFxX[si],    kFxW[si]};
                n = EmitPartPoly(fiberVerts, fiberPart, n, 54, quad, 4, kFxPart[si]);
            }
            fiberCount = n;
        }
    }
    switch (shapeType) {
        case 0: return {heartVerts, 72, g_partAllBody};
        case 1: return {starVerts, 30, g_partAllBody};
        case 2: return {hexVerts, 18, g_partAllBody};
        case 3: return {circleVerts, 72, g_partAllBody};
        case 4: return {diamondVerts, 12, g_partAllBody};
        case 5: return {triVerts, 3, g_partAllBody};
        case 6: return {flowerVerts, 30, g_partAllBody};
        case 7: return {pentVerts, 15, g_partAllBody};
        case 8: return {hexagramVerts, 36, g_partAllBody};
        case 11: return {rocketVerts, rocketCount, rocketPart};
        case 12: return {missileVerts, missileCount, missilePart};
        case 13: return {fiberVerts, fiberCount, fiberPart};
        default: return {circleVerts, 72, g_partAllBody};
    }
}
static void ShapeAnimAt(float progress, float rot, float size,
                        float &scale, float &rotAdd, float &offX, float &offY, float &flameMul) {
    const float kIn = 0.22f, kOut = 0.70f;
    float hx = cosf(rot), hy = sinf(rot);
    scale = 1.0f; rotAdd = 0.0f; offX = 0.0f; offY = 0.0f; flameMul = 1.0f;
    if (progress < kIn) {
        float t = progress / kIn;
        float u = t - 1.0f;
        scale = 1.0f + 2.70158f * u * u * u + 1.70158f * u * u;
        if (scale < 0.0f) scale = 0.0f;
        float back = size * 2.6f * (1.0f - t) * (1.0f - t);
        offX = -hx * back; offY = -hy * back;
        rotAdd = 0.38f * (1.0f - t) * (1.0f - t);
        flameMul = 1.0f + 1.9f * (1.0f - t);
    } else if (progress > kOut) {
        float t = (progress - kOut) / (1.0f - kOut);
        scale = fmaxf(1.0f - t * 0.75f, 0.12f);
        float fwd = size * 2.8f * t * t;
        offX = hx * fwd; offY = hy * fwd;
        flameMul = 1.0f - 0.65f * t;
    } else {
        scale = 1.0f + 0.05f * sinf(progress * 24.0f);
        rotAdd = 0.05f * sinf(progress * 17.0f);
        flameMul = 1.0f + 0.24f * sinf(progress * 29.0f);
    }
}
static void ShapePartColor(int part, float cr, float cg, float cb,
                           float &r, float &g, float &b, float &aMul) {
    aMul = 1.0f;
    switch (part) {
    case kPartFlame:
        r = fminf(cr * 0.45f + 0.78f, 1.6f);
        g = fminf(cg * 0.40f + 0.42f, 1.4f);
        b = cb * 0.22f;
        return;
    case kPartShadow:
        r = cr * 0.55f; g = cg * 0.55f; b = cb * 0.55f;
        return;
    case kPartGlow2: aMul = 0.70f; break;
    case kPartGlow3: aMul = 0.45f; break;
    case kPartGlow4: aMul = 0.25f; break;
    case kPartGlow5: aMul = 0.10f; break;
    default:
        if (part == kPartHi) {
            r = fminf(cr * 0.35f + 0.80f, 1.8f);
            g = fminf(cg * 0.35f + 0.86f, 1.8f);
            b = fminf(cb * 0.35f + 0.95f, 1.8f);
        } else {
            r = cr; g = cg; b = cb;
        }
        return;
    }
    r = fminf(cr * 0.35f + 0.80f, 1.8f);
    g = fminf(cg * 0.35f + 0.86f, 1.8f);
    b = fminf(cb * 0.35f + 0.95f, 1.8f);
}
static void ShapeTypeFx(int shapeType, float progress,
                        float &scaleMul, float &rotExtra, float &brightMul) {
    scaleMul = 1.0f; rotExtra = 0.0f; brightMul = 1.0f;
    switch (shapeType) {
    case 0:
        scaleMul  = 1.0f + 0.07f * sinf(progress * 42.0f) * (0.55f + 0.45f * sinf(progress * 12.0f));
        brightMul = 1.0f + 0.10f * sinf(progress * 42.0f);
        break;
    case 1: case 8:
        rotExtra  = progress * 4.2f;
        brightMul = 1.0f + 0.20f * sinf(progress * 49.0f);
        break;
    case 6:
        scaleMul = 1.0f + 0.09f * sinf(progress * 19.0f);
        break;
    case 3:
        scaleMul = 1.0f + 0.06f * sinf(progress * 23.0f);
        break;
    default:
        rotExtra = 0.10f * sinf(progress * 15.0f);
        break;
    }
    brightMul += 0.05f * sinf(progress * 21.0f + (float)shapeType * 0.7f);
}
float g_trailFxIntensity = 0.0f;
static void UpdateTrailFxIntensity(const std::vector<D2D1_POINT_2F> &smoothed) {
    float ratio = (g_tailLength > 0) ? ((float)smoothed.size() / (float)g_tailLength) : 0.0f;
    if (ratio > 1.0f) ratio = 1.0f;
    g_trailFxIntensity = ratio;
}
static void SetNativeRenderState(int screenW, int screenH, D3D11_PRIMITIVE_TOPOLOGY topology, const GradData* cols = nullptr) {
    UpdateConstantBuffer(screenW, screenH, cols);
    g_pD3DContext->IASetInputLayout(g_pNativeLayout);
    g_pD3DContext->VSSetShader(g_pNativeVS, nullptr, 0);
    g_pD3DContext->PSSetShader(g_pNativePS, nullptr, 0);
    g_pD3DContext->VSSetConstantBuffers(0, 1, &g_pConstantBuffer);
    g_pD3DContext->PSSetConstantBuffers(0, 1, &g_pConstantBuffer);
    g_pD3DContext->RSSetState(g_pRasterState);
    g_pD3DContext->OMSetBlendState(g_pAlphaBlend, nullptr, 0xFFFFFFFF);
    UINT stride = sizeof(VertexPosColor);
    UINT offset = 0;
    g_pD3DContext->IASetVertexBuffers(0, 1, &g_pTrailVB, &stride, &offset);
    g_pD3DContext->IASetPrimitiveTopology(topology);
}
enum class MtPrim : unsigned char {
    Poly = 0,
    Ring = 1,
    Disc = 2,
    Spoke = 3,
    Strip = 4,
};
struct MtDraw {
    MtPrim kind = MtPrim::Poly;
    const float *verts = nullptr;
    int vertCount = 0;
    float cx = 0.0f, cy = 0.0f;
    float scale = 1.0f;
    float rot = 0.0f;
    float radius = 0.0f;
    float width = 0.0f;
    float ang = 0.0f;
    float r0 = 0.0f, r1 = 0.0f;
    float r = 1.0f, g = 1.0f, b = 1.0f, a = 1.0f;
    bool additive = false;
    const float *path = nullptr;
    const float *nrm = nullptr;
    int pathCount = 0;
    bool tapered = false;
    float widthMul = 1.0f;
    float widthScale = 1.0f;
    float constWidth = 10.0f;
    float offX = 0.0f, offY = 0.0f;
    bool headOnly = false;
    int cacheKey = -1;
};
static const GradData *g_mtGrad = nullptr;
static float MtTaperFactor(float ratio) {
    static float lut[257];
    static bool inited = false;
    if (!inited) {
        for (int i = 0; i <= 256; i++) lut[i] = powf(1.0f - (float)i / 256.0f, 1.3f);
        inited = true;
    }
    float t = ratio * 256.0f;
    if (t < 0.0f) return lut[0];
    if (t >= 256.0f) return lut[256];
    int i = (int)t;
    float frac = t - (float)i;
    return lut[i] + (lut[i + 1] - lut[i]) * frac;
}
static inline float MtStripHalfW(const MtDraw &d, int i) {
    if (!d.tapered) return d.constWidth * 0.5f;
    if (i >= d.pathCount - 1) return 0.5f * d.widthScale;
    float ratio = (float)i / (float)(d.pathCount - 1);
    return 10.0f * MtTaperFactor(ratio) * d.widthMul * d.widthScale;
}
namespace mtb {
    struct Layer { int offset, count; bool additive; D3D11_PRIMITIVE_TOPOLOGY topo; };
    static std::vector<VertexPosColor> s_verts;
    static std::vector<Layer> s_layers;
    static const size_t kGeometryCacheLimit = 512;
    static std::unordered_map<int, ID2D1PathGeometry *> s_keyCache;
    static void D3DBegin() {
        s_verts.clear();
        s_layers.clear();
    }
    static void D3DDraw(const MtDraw &d) {
        {
            int need = 0;
            switch (d.kind) {
            case MtPrim::Poly:  need = (d.vertCount > 2) ? (d.vertCount - 2) * 3 : 0; break;
            case MtPrim::Ring:  need = 25 * 2; break;
            case MtPrim::Disc:  need = 24 * 3; break;
            case MtPrim::Spoke: need = 4; break;
            case MtPrim::Strip: need = d.pathCount * 2; break;
            }
            if (need <= 0 || need > kMaxTrailVertices - (int)s_verts.size()) return;
        }
        int start = (int)s_verts.size();
        float cs = cosf(d.rot), sn = sinf(d.rot);
        auto push2 = [&](float lx, float ly, float v) {
            lx *= d.scale; ly *= d.scale;
            s_verts.push_back({d.cx + lx * cs - ly * sn, d.cy + lx * sn + ly * cs,
                               0.0f, d.r, d.g, d.b, d.a, 0.5f, v});
        };
        switch (d.kind) {
        case MtPrim::Poly: {
            if (!d.verts || d.vertCount < 3) return;
            for (int i = 0; i + 2 < d.vertCount; i += 3) {
                push2(d.verts[i * 2], d.verts[i * 2 + 1], 0.0f);
                push2(d.verts[(i + 1) * 2], d.verts[(i + 1) * 2 + 1], 0.0f);
                push2(d.verts[(i + 2) * 2], d.verts[(i + 2) * 2 + 1], 0.0f);
            }
            break;
        }
        case MtPrim::Ring: {
            const int seg = 24;
            for (int i = 0; i <= seg; i++) {
                float th = (float)i / (float)seg * 6.2831853f;
                float c = cosf(th), s = sinf(th);
                push2(c * (d.radius + d.width), s * (d.radius + d.width), 1.0f);
                push2(c * d.radius, s * d.radius, -1.0f);
            }
            break;
        }
        case MtPrim::Disc: {
            const int seg = 24;
            for (int i = 0; i < seg; i++) {
                float a1 = (float)i / (float)seg * 6.2831853f;
                float a2 = (float)(i + 1) / (float)seg * 6.2831853f;
                push2(0.0f, 0.0f, 0.0f);
                push2(cosf(a1) * d.radius, sinf(a1) * d.radius, 0.0f);
                push2(cosf(a2) * d.radius, sinf(a2) * d.radius, 0.0f);
            }
            break;
        }
        case MtPrim::Spoke: {
            float ca = cosf(d.ang), sa = sinf(d.ang);
            float nx = -sa, ny = ca;
            float hw = d.width * 0.5f;
            push2(ca * d.r0 + nx * hw, sa * d.r0 + ny * hw, 1.0f);
            push2(ca * d.r0 - nx * hw, sa * d.r0 - ny * hw, -1.0f);
            push2(ca * d.r1 + nx * hw, sa * d.r1 + ny * hw, 1.0f);
            push2(ca * d.r1 - nx * hw, sa * d.r1 - ny * hw, -1.0f);
            break;
        }
        case MtPrim::Strip: {
            if (!d.path || !d.nrm || d.pathCount < 2) return;
            int pc = d.pathCount;
            if ((int)s_verts.size() + pc * 2 > kMaxTrailVertices)
                pc = (kMaxTrailVertices - (int)s_verts.size()) / 2;
            if (pc < 2) return;
            float inv = 1.0f / (float)(d.pathCount - 1);
            for (int i = 0; i < pc; i++) {
                float ratio = (float)i * inv;
                float ow = MtStripHalfW(d, i);
                float nx = d.nrm[i * 2], ny = d.nrm[i * 2 + 1];
                float px = d.path[i * 2] + d.offX;
                float py = d.path[i * 2 + 1] + d.offY;
                float a = d.a;
                if (d.headOnly)
                    a *= (ratio < 0.25f) ? 0.0f : ((ratio > 0.85f) ? 1.0f : (ratio - 0.25f) / 0.6f);
                s_verts.push_back({px + nx * ow, py + ny * ow, 0.0f, d.r, d.g, d.b, a, ratio, 1.0f});
                s_verts.push_back({px - nx * ow, py - ny * ow, 0.0f, d.r, d.g, d.b, a, ratio, -1.0f});
            }
            break;
        }
        }
        int n = (int)s_verts.size() - start;
        if (n > 0) {
            D3D11_PRIMITIVE_TOPOLOGY topo = (d.kind == MtPrim::Poly || d.kind == MtPrim::Disc)
                ? D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST
                : D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
            bool mergeable = (topo == D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
            if (mergeable
                && !s_layers.empty()
                && s_layers.back().topo == topo
                && s_layers.back().additive == d.additive
                && s_layers.back().offset + s_layers.back().count == start) {
                s_layers.back().count += n;
            } else {
                s_layers.push_back({start, n, d.additive, topo});
            }
        }
    }
    static void D3DEnd(int screenW, int screenH, const GradData *grad = nullptr) {
        if (s_verts.empty() || !g_pTrailVB) return;
        if (s_verts.size() > kMaxTrailVertices) s_verts.resize(kMaxTrailVertices);
        D3D11_MAPPED_SUBRESOURCE mapped;
        if (FAILED(g_pD3DContext->Map(g_pTrailVB, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped))) return;
        memcpy(mapped.pData, s_verts.data(), s_verts.size() * sizeof(VertexPosColor));
        g_pD3DContext->Unmap(g_pTrailVB, 0);
        for (const Layer &L : s_layers) {
            SetNativeRenderState(screenW, screenH, L.topo, grad);
            g_pD3DContext->OMSetBlendState(L.additive ? g_pAdditiveBlend : g_pAlphaBlend,
                                           nullptr, 0xFFFFFFFF);
            g_pD3DContext->Draw((UINT)L.count, (UINT)L.offset);
        }
        g_pD3DContext->OMSetBlendState(g_pAlphaBlend, nullptr, 0xFFFFFFFF);
    }
    static ID2D1PathGeometry *BuildPolyGeom(const float *verts, int count) {
        ID2D1PathGeometry *geom = nullptr;
        if (FAILED(g_pD2DFactory->CreatePathGeometry(&geom))) return nullptr;
        ID2D1GeometrySink *sink = nullptr;
        if (FAILED(geom->Open(&sink))) { geom->Release(); return nullptr; }
        sink->SetFillMode(D2D1_FILL_MODE_WINDING);
        for (int i = 0; i + 2 < count; i += 3) {
            D2D1_POINT_2F pts[3] = {
                D2D1::Point2F(verts[i * 2],       verts[i * 2 + 1]),
                D2D1::Point2F(verts[(i + 1) * 2], verts[(i + 1) * 2 + 1]),
                D2D1::Point2F(verts[(i + 2) * 2], verts[(i + 2) * 2 + 1]),
            };
            sink->BeginFigure(pts[0], D2D1_FIGURE_BEGIN_FILLED);
            sink->AddLines(pts + 1, 2);
            sink->EndFigure(D2D1_FIGURE_END_CLOSED);
        }
        sink->Close();
        sink->Release();
        return geom;
    }
    static ID2D1PathGeometry *PolyGeom(const float *verts, int count, int cacheKey = -1) {
        if (cacheKey < 0) return BuildPolyGeom(verts, count);
        auto it = s_keyCache.find(cacheKey);
        if (it != s_keyCache.end()) return it->second;
        if (s_keyCache.size() >= kGeometryCacheLimit) {
            for (auto &kv : s_keyCache) { if (kv.second) kv.second->Release(); }
            s_keyCache.clear();
        }
        ID2D1PathGeometry *g = BuildPolyGeom(verts, count);
        if (g) s_keyCache[cacheKey] = g;
        return g;
    }
    static void D2DDraw(const MtDraw &d) {
        if (!g_pD2DDC) return;
        D2D1_MATRIX_3X2_F oldT;
        g_pD2DDC->GetTransform(&oldT);
        g_pD2DDC->SetTransform(D2D1::Matrix3x2F::Rotation(d.rot * 57.2958f) *
                               D2D1::Matrix3x2F::Scale(d.scale, d.scale) *
                               D2D1::Matrix3x2F::Translation(d.cx, d.cy) * oldT);
        g_pSolidOuterBrush->SetColor(D2D1::ColorF(d.r, d.g, d.b));
        g_pSolidOuterBrush->SetOpacity(d.a);
        switch (d.kind) {
        case MtPrim::Poly: {
            ID2D1PathGeometry *geom = PolyGeom(d.verts, d.vertCount, d.cacheKey);
            if (geom) g_pD2DDC->FillGeometry(geom, g_pSolidOuterBrush);
            break;
        }
        case MtPrim::Ring:
            g_pD2DDC->DrawEllipse(D2D1::Ellipse(D2D1::Point2F(0, 0), d.radius, d.radius),
                                  g_pSolidOuterBrush, d.width);
            break;
        case MtPrim::Disc:
            g_pD2DDC->FillEllipse(D2D1::Ellipse(D2D1::Point2F(0, 0), d.radius, d.radius),
                                  g_pSolidOuterBrush);
            break;
        case MtPrim::Spoke: {
            float ca = cosf(d.ang), sa = sinf(d.ang);
            float nx = -sa * d.width * 0.5f, ny = ca * d.width * 0.5f;
            float pts[8] = {ca * d.r0 + nx, sa * d.r0 + ny,
                            ca * d.r1 + nx, sa * d.r1 + ny,
                            ca * d.r1 - nx, sa * d.r1 - ny,
                            ca * d.r0 - nx, sa * d.r0 - ny};
            ID2D1PathGeometry *geom = nullptr;
            if (SUCCEEDED(g_pD2DFactory->CreatePathGeometry(&geom))) {
                ID2D1GeometrySink *sink = nullptr;
                if (SUCCEEDED(geom->Open(&sink))) {
                    sink->SetFillMode(D2D1_FILL_MODE_WINDING);
                    D2D1_POINT_2F p[4] = {D2D1::Point2F(pts[0], pts[1]), D2D1::Point2F(pts[2], pts[3]),
                                          D2D1::Point2F(pts[4], pts[5]), D2D1::Point2F(pts[6], pts[7])};
                    sink->BeginFigure(p[0], D2D1_FIGURE_BEGIN_FILLED);
                    sink->AddLines(p + 1, 3);
                    sink->EndFigure(D2D1_FIGURE_END_CLOSED);
                    sink->Close();
                    sink->Release();
                    g_pD2DDC->FillGeometry(geom, g_pSolidOuterBrush);
                }
                geom->Release();
            }
            break;
        }
        case MtPrim::Strip: {
            if (!d.path || !d.nrm || d.pathCount < 2) break;
            int pc = d.pathCount;
            float inv = 1.0f / (float)(pc - 1);
            auto halfW = [&](int i) -> float { return MtStripHalfW(d, i); };
            std::vector<D2D1_POINT_2F> outline;
            auto fillSeg = [&](int i0, int i1) {
                outline.clear();
                outline.reserve((size_t)(i1 - i0 + 1) * 2);
                for (int i = i0; i <= i1; i++) {
                    float ow = halfW(i);
                    outline.push_back(D2D1::Point2F(d.path[i * 2] + d.offX
                                                    + d.nrm[i * 2] * ow,
                                                    d.path[i * 2 + 1] + d.offY
                                                    + d.nrm[i * 2 + 1] * ow));
                }
                for (int i = i1; i >= i0; i--) {
                    float ow = halfW(i);
                    outline.push_back(D2D1::Point2F(d.path[i * 2] + d.offX
                                                    - d.nrm[i * 2] * ow,
                                                    d.path[i * 2 + 1] + d.offY
                                                    - d.nrm[i * 2 + 1] * ow));
                }
                if (outline.size() < 3) return;
                ID2D1PathGeometry *geom = nullptr;
                if (SUCCEEDED(g_pD2DFactory->CreatePathGeometry(&geom))) {
                    ID2D1GeometrySink *sink = nullptr;
                    if (SUCCEEDED(geom->Open(&sink))) {
                        sink->SetFillMode(D2D1_FILL_MODE_WINDING);
                        sink->BeginFigure(outline[0], D2D1_FIGURE_BEGIN_FILLED);
                        sink->AddLines(outline.data() + 1, (UINT32)(outline.size() - 1));
                        sink->EndFigure(D2D1_FIGURE_END_CLOSED);
                        sink->Close();
                        sink->Release();
                        g_pD2DDC->FillGeometry(geom, g_pSolidOuterBrush);
                    }
                    geom->Release();
                }
            };
            if (g_mtGrad) {
                const int kSegs = 8;
                for (int s = 0; s < kSegs; s++) {
                    int i0 = (int)((float)s / (float)kSegs * (float)(pc - 1));
                    int i1 = (int)((float)(s + 1) / (float)kSegs * (float)(pc - 1));
                    if (i1 <= i0) i1 = i0 + 1;
                    if (i1 > pc - 1) i1 = pc - 1;
                    float ratioMid = ((float)i0 + (float)i1) * 0.5f * inv;
                    D2D1_COLOR_F base = SampleGradientLUT(ratioMid, 0.0f);
                    g_pSolidOuterBrush->SetColor(D2D1::ColorF(base.r * d.r, base.g * d.g, base.b * d.b, 1.0f));
                    float am = 1.0f;
                    if (d.headOnly)
                        am = (ratioMid < 0.25f) ? 0.0f
                                                : ((ratioMid > 0.85f) ? 1.0f : (ratioMid - 0.25f) / 0.6f);
                    g_pSolidOuterBrush->SetOpacity(d.a * am);
                    if (am > 0.001f) fillSeg(i0, i1);
                }
            } else {
                g_pSolidOuterBrush->SetColor(D2D1::ColorF(d.r, d.g, d.b));
                fillSeg(0, pc - 1);
            }
            break;
        }
        }
        g_pD2DDC->SetTransform(oldT);
        g_pSolidOuterBrush->SetOpacity(1.0f);
    }
}
static inline bool MtUsingNative() { return g_pNativeVS != nullptr; }
static inline void MtBegin(int screenW, int screenH, const GradData *grad = nullptr) {
    g_mtGrad = grad;
    if (MtUsingNative()) mtb::D3DBegin();
    (void)screenW; (void)screenH;
}
static inline void MtPrimDraw(const MtDraw &d) {
    if (MtUsingNative()) mtb::D3DDraw(d);
    else mtb::D2DDraw(d);
}
static inline void MtEnd(int screenW, int screenH) {
    if (MtUsingNative()) mtb::D3DEnd(screenW, screenH, g_mtGrad);
    g_mtGrad = nullptr;
}
static void DrawTrailShapes(int screenW, int screenH, DWORD dwTime) {
    if (g_trailShapes.empty()) return;
    static std::vector<float> s_pv;
    MtBegin(screenW, screenH);
    for (auto &s : g_trailShapes) {
        float progress = (float)(dwTime - s.startTime) / s.lifetime;
        if (progress < 0 || progress >= 1) continue;
        float aScale, aRot, aOffX, aOffY, aFlame;
        ShapeAnimAt(progress, s.rotation, s.size, aScale, aRot, aOffX, aOffY, aFlame);
        float fxScale, fxRot, fxBright;
        ShapeTypeFx(s.shapeType, progress, fxScale, fxRot, fxBright);
        float craftMul = (s.shapeType >= 11 && s.shapeType <= 13) ? 1.25f : 1.0f;
        float scale = aScale * s.size * craftMul * fxScale;
        float rot = s.rotation + aRot + fxRot;
        float bx = s.x + aOffX, by = s.y + aOffY;
        float lifeAlpha = ShapeLifeAlpha(progress) * 0.7f;
        ShapeVertsCache sv = GetShapeVerts(s.shapeType);
        float flameStretch = (1.0f + 0.28f * sinf(progress * 31.0f + s.x * 0.05f)) * aFlame;
        float fiberStretch = 1.0f + g_trailFxIntensity * 1.1f;
        for (int part = 0; part < 8; part++) {
            s_pv.clear();
            float stretch = 1.0f;
            for (int i = 0; i < sv.count; i++) {
                unsigned char p = sv.part ? sv.part[i] : kPartBody;
                if (p != part) continue;
                float lx = sv.verts[i * 2], ly = sv.verts[i * 2 + 1];
                if (part == kPartFlame) {
                    lx *= flameStretch;
                    stretch = flameStretch;
                } else if (part == kPartHi && s.shapeType == 13 && lx < -1.0f) {
                    lx *= fiberStretch;
                    stretch = fiberStretch;
                }
                s_pv.push_back(lx);
                s_pv.push_back(ly);
            }
            if (s_pv.empty()) continue;
            float pr, pg, pb, paMul;
            ShapePartColor(part, s.color.r, s.color.g, s.color.b, pr, pg, pb, paMul);
            pr = fminf(pr * fxBright, 1.0f);
            pg = fminf(pg * fxBright, 1.0f);
            pb = fminf(pb * fxBright, 1.0f);
            MtDraw d;
            d.kind = MtPrim::Poly;
            d.verts = s_pv.data();
            d.vertCount = (int)s_pv.size() / 2;
            d.cx = bx; d.cy = by;
            d.scale = scale; d.rot = rot;
            d.r = pr; d.g = pg; d.b = pb;
            d.a = lifeAlpha * paMul;
            d.additive = false;
            int q = (int)((stretch - 0.5f) * 24.0f);
            q = ClampInt(q, 0, 47);
            d.cacheKey = s.shapeType * 10000 + part * 100 + q;
            MtPrimDraw(d);
        }
    }
    MtEnd(screenW, screenH);
}
static void DrawTrailBand(const std::vector<D2D1_POINT_2F> &smoothed,
                          const GradData &cols, float widthMul, float fadeAlpha,
                          int screenW, int screenH) {
    size_t sl = smoothed.size();
    if (sl < 2) return;
    static std::vector<float> s_nrm;
    s_nrm.clear();
    s_nrm.reserve(sl * 2);
    for (size_t i = 0; i < sl; i++) {
        float ddx, ddy;
        if (i == 0) { ddx = smoothed[1].x - smoothed[0].x; ddy = smoothed[1].y - smoothed[0].y; }
        else if (i == sl - 1) { ddx = smoothed[i].x - smoothed[i - 1].x; ddy = smoothed[i].y - smoothed[i - 1].y; }
        else { ddx = smoothed[i + 1].x - smoothed[i - 1].x; ddy = smoothed[i + 1].y - smoothed[i - 1].y; }
        float ln = sqrtf(ddx * ddx + ddy * ddy);
        if (ln > 0) { ddx /= ln; ddy /= ln; }
        s_nrm.push_back(-ddy);
        s_nrm.push_back(ddx);
    }
    MtDraw d;
    d.kind = MtPrim::Strip;
    d.path = (const float *)smoothed.data();
    d.nrm = s_nrm.data();
    d.pathCount = (int)sl;
    d.tapered = true;
    d.widthMul = widthMul;
    d.widthScale = 1.0f;
    d.r = d.g = d.b = 1.0f;
    MtBegin(screenW, screenH, &cols);
    if (g_enableTrailShadow) {
        d.widthScale = 1.0f;
        d.r = d.g = d.b = 0.0f;
        d.a = fadeAlpha * 0.18f;
        d.offX = 1.5f; d.offY = 2.0f;
        d.additive = false; d.headOnly = false;
        MtPrimDraw(d);
    }
    if (g_adaptiveContrast) {
        float edgeV = g_bgLuminance > 0.5f ? (0.02f + (1.0f - g_bgLuminance) * 0.06f)
                                           : (1.2f + g_bgLuminance * 0.8f);
        d.widthScale = 1.18f;
        d.r = d.g = d.b = edgeV;
        d.a = fadeAlpha * 0.22f;
        d.offX = 0.0f; d.offY = 0.0f;
        MtPrimDraw(d);
    }
    if (g_enableGlow) {
        float gi = FxApply(g_glowIntensity / 100.0f, g_fxGlowMul);
        d.r = d.g = d.b = 1.0f;
        d.additive = true;
        d.widthScale = 2.6f + gi * 1.4f; d.a = fadeAlpha * (0.04f + gi * 0.05f); MtPrimDraw(d);
        d.widthScale = 1.7f + gi * 0.7f; d.a = fadeAlpha * (0.09f + gi * 0.09f); MtPrimDraw(d);
        if (g_enhancedGlow) {
            d.widthScale = 1.25f + gi * 0.35f;
            d.r = d.g = d.b = 1.15f;
            d.a = fadeAlpha * (0.14f + gi * 0.10f);
            MtPrimDraw(d);
        }
    }
    d.widthScale = 1.0f;
    d.r = d.g = d.b = 1.0f;
    d.a = fadeAlpha;
    d.additive = false;
    d.headOnly = false;
    MtPrimDraw(d);
    if (g_enableHeadHighlight) {
        d.widthScale = 0.35f;
        d.r = d.g = d.b = 1.25f;
        d.a = fadeAlpha * 0.75f;
        d.headOnly = true;
        MtPrimDraw(d);
    }
    MtEnd(screenW, screenH);
}
static void DrawClickEffects(int screenW, int screenH, DWORD dwTime, const GradData &cols, int vX, int vY) {
    if (!g_enableClickEffect || g_ripples.empty()) return;
    const D2D1_COLOR_F rc = cols.outer[0].color;
    float cr = rc.r, cg = rc.g, cb = rc.b;
    static std::vector<float> s_poly;
    auto polyRing = [&](float radius, float width, int seg, float wobAmp, float wobPhase) -> int {
        s_poly.clear();
        s_poly.reserve(seg * 4);
        for (int i = 0; i < seg; i++) {
            float t0 = (float)i / (float)seg * 6.2831853f;
            float t1 = (float)(i + 1) / (float)seg * 6.2831853f;
            float w0 = 1.0f + wobAmp * (0.62f * sinf(t0 * 5.0f + wobPhase) + 0.38f * sinf(t0 * 3.0f - wobPhase * 0.65f));
            float w1 = 1.0f + wobAmp * (0.62f * sinf(t1 * 5.0f + wobPhase) + 0.38f * sinf(t1 * 3.0f - wobPhase * 0.65f));
            float c0 = cosf(t0), s0 = sinf(t0), c1 = cosf(t1), s1 = sinf(t1);
            s_poly.push_back(c0 * (radius + width) * w0); s_poly.push_back(s0 * (radius + width) * w0);
            s_poly.push_back(c1 * (radius + width) * w1); s_poly.push_back(s1 * (radius + width) * w1);
            s_poly.push_back(c1 * radius * w1);           s_poly.push_back(s1 * radius * w1);
            s_poly.push_back(c0 * (radius + width) * w0); s_poly.push_back(s0 * (radius + width) * w0);
            s_poly.push_back(c1 * radius * w1);           s_poly.push_back(s1 * radius * w1);
            s_poly.push_back(c0 * radius * w0);           s_poly.push_back(s0 * radius * w0);
        }
        return (int)s_poly.size() / 2;
    };
    auto InkKey = [&](float progress, float radius) -> int {
        int qp = (int)(progress * 24.0f);
        if (qp > 23) qp = 23;
        int qr = (int)(radius * 0.25f);
        if (qr > 127) qr = 127;
        return qp * 128 + qr;
    };
    auto polyEllipse = [&](float rx, float ry, int seg) -> int {
        s_poly.clear();
        s_poly.reserve(seg * 6);
        for (int i = 0; i < seg; i++) {
            float a1 = (float)i / (float)seg * 6.2831853f;
            float a2 = (float)(i + 1) / (float)seg * 6.2831853f;
            s_poly.push_back(0); s_poly.push_back(0);
            s_poly.push_back(cosf(a1) * rx); s_poly.push_back(sinf(a1) * ry);
            s_poly.push_back(cosf(a2) * rx); s_poly.push_back(sinf(a2) * ry);
        }
        return (int)s_poly.size() / 2;
    };
    MtBegin(screenW, screenH);
    for (auto &r : g_ripples) {
        float progress = (float)(dwTime - r.startTime) / g_clickDuration;
        if (progress < 0 || progress >= 1) continue;
        float eased = 1.0f - powf(1.0f - progress, 2.0f);
        float radius = g_clickMaxRadius * eased;
        float alpha = (1.0f - progress) * 0.9f;
        MtDraw d;
        d.cx = (float)(r.pos.x - vX);
        d.cy = (float)(r.pos.y - vY);
        d.scale = 1.0f;
        d.rot = 0.0f;
        switch (g_clickStyle) {
        default:
        case 0: {
            float w = 5.0f + progress * 4.0f;
            d.kind = MtPrim::Ring; d.radius = radius; d.width = w * 2.5f;
            d.r = cr; d.g = cg; d.b = cb; d.a = alpha * 0.35f; d.additive = true;
            MtPrimDraw(d);
            d.width = w; d.a = alpha; d.additive = false;
            MtPrimDraw(d);
            d.radius = radius - w * 0.3f; d.width = w * 0.5f;
            d.r = cr * 1.4f; d.g = cg * 1.4f; d.b = cb * 1.4f; d.a = alpha * 0.6f;
            MtPrimDraw(d);
            break;
        }
        case 1: {
            float w = 6.0f + progress * 5.0f;
            if (progress < 0.3f) {
                d.kind = MtPrim::Disc; d.radius = radius * 0.42f + 2.0f;
                d.r = cr; d.g = cg; d.b = cb;
                d.a = (1.0f - progress / 0.3f) * alpha * 0.9f; d.additive = true;
                MtPrimDraw(d);
            }
            d.kind = MtPrim::Ring; d.radius = radius; d.width = w;
            d.a = alpha; d.additive = true;
            MtPrimDraw(d);
            d.radius = radius * 1.35f; d.width = 2.0f; d.a = alpha * 0.45f; d.additive = false;
            MtPrimDraw(d);
            d.kind = MtPrim::Spoke; d.width = 3.0f;
            d.r = cr * 1.3f; d.g = cg * 1.3f; d.b = cb * 1.3f;
            d.a = alpha * 0.9f; d.additive = true;
            for (int i = 0; i < 8; i++) {
                d.ang = (float)i * 0.78539816f + progress * 0.6f;
                d.r0 = radius * 0.85f; d.r1 = radius * 1.18f;
                MtPrimDraw(d);
            }
            break;
        }
        case 2: {
            float w = 4.0f + progress * 3.0f;
            d.kind = MtPrim::Poly;
            d.r = cr; d.g = cg; d.b = cb;
            d.vertCount = polyRing(radius * 0.92f, radius * 0.30f, 28, 0.28f, progress * 4.0f);
            d.verts = s_poly.data();
            d.a = alpha * 0.20f; d.additive = true;
            d.cacheKey = 200000 + InkKey(progress, radius);
            MtPrimDraw(d);
            d.vertCount = polyRing(radius, w, 28, 0.28f, progress * 4.0f);
            d.verts = s_poly.data();
            d.a = alpha; d.additive = false;
            d.cacheKey = 250000 + InkKey(progress, radius);
            MtPrimDraw(d);
            d.kind = MtPrim::Disc;
            for (int i = 0; i < 4; i++) {
                float ang = (float)i * 1.7f + progress * 1.2f;
                float dr = radius * (1.15f + 0.12f * (float)(i % 2));
                d.cx = (float)(r.pos.x - vX) + cosf(ang) * dr;
                d.cy = (float)(r.pos.y - vY) + sinf(ang) * dr;
                d.radius = fmaxf(radius * 0.08f, 1.0f);
                d.a = alpha * 0.6f;
                MtPrimDraw(d);
            }
            break;
        }
        case 3: {
            float beam = g_clickMaxRadius * (1.0f - progress) * 1.45f;
            d.kind = MtPrim::Spoke; d.additive = true;
            d.r = cr; d.g = cg; d.b = cb;
            d.a = alpha;
            for (int i = 0; i < 4; i++) {
                d.ang = (float)i * 1.5707963f;
                d.r0 = 0.0f; d.r1 = beam; d.width = 4.0f;
                MtPrimDraw(d);
            }
            d.a = alpha * 0.8f; d.width = 2.5f;
            for (int i = 0; i < 8; i++) {
                d.ang = (float)i * 0.78539816f + 0.39269908f;
                d.r0 = beam * 0.35f; d.r1 = beam * 0.58f;
                MtPrimDraw(d);
            }
            d.kind = MtPrim::Disc; d.radius = fmaxf(beam * 0.16f, 1.5f);
            d.a = alpha;
            MtPrimDraw(d);
            break;
        }
        case 4: {
            d.kind = MtPrim::Poly;
            d.r = cr; d.g = cg; d.b = cb; d.a = alpha * 0.9f; d.additive = false;
            for (int i = 0; i < 6; i++) {
                float ang = (float)i * 6.2831853f / 6.0f + progress * 0.9f;
                d.cx = (float)(r.pos.x - vX) + cosf(ang) * radius * 0.55f;
                d.cy = (float)(r.pos.y - vY) + sinf(ang) * radius * 0.55f;
                d.rot = ang;
                d.vertCount = polyEllipse(radius * 0.5f, radius * 0.19f, 16);
                d.verts = s_poly.data();
                int qr = (int)(radius * 0.25f);
                if (qr > 127) qr = 127;
                d.cacheKey = 300000 + qr;
                MtPrimDraw(d);
            }
            d.kind = MtPrim::Disc; d.rot = 0.0f;
            d.cx = (float)(r.pos.x - vX); d.cy = (float)(r.pos.y - vY);
            d.radius = radius * 0.22f; d.a = alpha * 0.55f; d.additive = true;
            MtPrimDraw(d);
            break;
        }
        }
    }
    MtEnd(screenW, screenH);
}
static D2D1_POINT_2F GetPointOnPath(const std::vector<D2D1_POINT_2F> &path, float ratio);
static void NativeRenderDotChain(const std::vector<D2D1_POINT_2F>& path, float dotSize,
                                 const GradData& cols, float fadeAlpha, int screenW, int screenH) {
    if (!g_pNativeVS || !g_pNativePS || !g_pTrailVB || path.size() < 2) return;
    static std::vector<VertexPosColor> verts;
    static std::vector<VertexPosColor> shadowVerts;
    verts.clear();
    shadowVerts.clear();
    if (verts.capacity() < path.size() * 6) verts.reserve(path.size() * 6);
    if (shadowVerts.capacity() < path.size() * 6) shadowVerts.reserve(path.size() * 6);
    const float SH_DX = 1.5f, SH_DY = 2.0f;
    for (size_t i = 0; i < path.size(); i++) {
        float ratio = (float)i / (path.size() - 1);
        float taper = MtTaperFactor(ratio);
        float size = dotSize * taper;
        if (size < 0.5f) continue;
        float x = path[i].x, y = path[i].y;
        D2D1_GRADIENT_STOP gs = cols.outer[(int)(ratio * (GRAD_STOPS - 1))];
        float r = gs.color.r, g = gs.color.g, b = gs.color.b;
        if (g_enableTrailShadow) {
            float sx = x + SH_DX, sy = y + SH_DY, sa = fadeAlpha * 0.18f;
            shadowVerts.push_back({sx - size, sy - size, 0, 0, 0, 0, sa, ratio, 0.0f});
            shadowVerts.push_back({sx + size, sy - size, 0, 0, 0, 0, sa, ratio, 0.0f});
            shadowVerts.push_back({sx - size, sy + size, 0, 0, 0, 0, sa, ratio, 0.0f});
            shadowVerts.push_back({sx + size, sy - size, 0, 0, 0, 0, sa, ratio, 0.0f});
            shadowVerts.push_back({sx + size, sy + size, 0, 0, 0, 0, sa, ratio, 0.0f});
            shadowVerts.push_back({sx - size, sy + size, 0, 0, 0, 0, sa, ratio, 0.0f});
        }
        verts.push_back({x - size, y - size, 0, r, g, b, fadeAlpha, ratio, 0.0f});
        verts.push_back({x + size, y - size, 0, r, g, b, fadeAlpha, ratio, 0.0f});
        verts.push_back({x - size, y + size, 0, r, g, b, fadeAlpha, ratio, 0.0f});
        verts.push_back({x + size, y - size, 0, r, g, b, fadeAlpha, ratio, 0.0f});
        verts.push_back({x + size, y + size, 0, r, g, b, fadeAlpha, ratio, 0.0f});
        verts.push_back({x - size, y + size, 0, r, g, b, fadeAlpha, ratio, 0.0f});
    }
    if (verts.empty() && shadowVerts.empty()) return;
    if (verts.size() > kMaxTrailVertices) verts.resize(kMaxTrailVertices);
    if (shadowVerts.size() > kMaxTrailVertices) shadowVerts.resize(kMaxTrailVertices);
    SetNativeRenderState(screenW, screenH, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST, &cols);
    auto drawList = [&](const std::vector<VertexPosColor>& v) {
        if (v.empty()) return;
        D3D11_MAPPED_SUBRESOURCE mapped;
        if (SUCCEEDED(g_pD3DContext->Map(g_pTrailVB, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped))) {
            memcpy(mapped.pData, v.data(), v.size() * sizeof(VertexPosColor));
            g_pD3DContext->Unmap(g_pTrailVB, 0);
            g_pD3DContext->Draw((UINT)v.size(), 0);
        }
    };
    drawList(shadowVerts);
    drawList(verts);
}
template <class T, class F>
static inline void PruneExpired(std::vector<T> &v, F pred) {
    v.erase(std::remove_if(v.begin(), v.end(), pred), v.end());
}
static void RenderPhysicsDebugD2D(DWORD dwTime, const std::vector<D2D1_POINT_2F>& smoothed) {
    if (!g_pD2DDC) return;
    if (!g_debugVelocity && !g_debugForce && !g_debugVortex && !g_debugGravity && !g_debugCollision && !g_debugSpring && !g_debugInteraction && !g_debugSpawn && !g_debugPath) return;
    float animT = (float)(dwTime % 10000) / 10000.0f;
    ID2D1SolidColorBrush *brushGreen = nullptr, *brushRed = nullptr,
                        *brushBlue = nullptr, *brushYellow = nullptr,
                        *brushCyan = nullptr, *brushCyanFill = nullptr,
                        *brushSpringLoose = nullptr, *brushSpringTaut = nullptr;
    if (g_debugVelocity)
        g_pD2DDC->CreateSolidColorBrush(D2D1::ColorF(0.0f, 1.0f, 0.0f, 0.95f), &brushGreen);
    if (g_debugForce)
        g_pD2DDC->CreateSolidColorBrush(D2D1::ColorF(1.0f, 0.2f, 0.2f, 0.95f), &brushRed);
    if (g_debugVortex)
        g_pD2DDC->CreateSolidColorBrush(D2D1::ColorF(0.0f, 0.6f, 1.0f, 0.9f), &brushBlue);
    if (g_debugGravity)
        g_pD2DDC->CreateSolidColorBrush(D2D1::ColorF(1.0f, 1.0f, 0.0f, 0.95f), &brushYellow);
    if (g_debugCollision) {
        g_pD2DDC->CreateSolidColorBrush(D2D1::ColorF(0.0f, 1.0f, 1.0f, 0.9f), &brushCyan);
        g_pD2DDC->CreateSolidColorBrush(D2D1::ColorF(0.0f, 1.0f, 1.0f, 0.18f), &brushCyanFill);
    }
    if (g_debugSpring) {
        g_pD2DDC->CreateSolidColorBrush(D2D1::ColorF(0.2f, 0.4f, 1.0f, 0.55f), &brushSpringLoose);
        g_pD2DDC->CreateSolidColorBrush(D2D1::ColorF(1.0f, 0.3f, 0.2f, 0.85f), &brushSpringTaut);
    }
    ID2D1SolidColorBrush *brushMagenta = nullptr, *brushOrange = nullptr,
                        *brushOrangeFill = nullptr, *brushWhite = nullptr, *brushGray = nullptr;
    if (g_debugInteraction)
        g_pD2DDC->CreateSolidColorBrush(D2D1::ColorF(1.0f, 0.0f, 1.0f, 0.85f), &brushMagenta);
    if (g_debugSpawn) {
        g_pD2DDC->CreateSolidColorBrush(D2D1::ColorF(1.0f, 0.65f, 0.0f, 0.95f), &brushOrange);
        g_pD2DDC->CreateSolidColorBrush(D2D1::ColorF(1.0f, 0.65f, 0.0f, 0.12f), &brushOrangeFill);
    }
    if (g_debugPath) {
        g_pD2DDC->CreateSolidColorBrush(D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.9f), &brushWhite);
        g_pD2DDC->CreateSolidColorBrush(D2D1::ColorF(0.5f, 0.5f, 0.5f, 0.6f), &brushGray);
    }
    const float VEC_SCALE = 4.0f;
    const float FORCE_SCALE = 10.0f;
    if (g_debugVelocity || g_debugForce || g_debugCollision) {
        for (auto &p : g_particles) {
            float progress = (float)(dwTime - p.startTime) / p.lifetime;
            if (progress < 0 || progress >= 1) continue;
            D2D1_POINT_2F pos = D2D1::Point2F(p.x, p.y);
            if (g_debugCollision) {
                float colRadius = (p.shapeType == 10)
                    ? (g_textFontSize * 0.5f * (g_particleSizeMultiplier / 100.0f) * 0.5f) * sqrtf(1.0f + g_textAspectRatio * g_textAspectRatio)
                    : p.size * (g_particleSizeMultiplier / 100.0f);
                g_pD2DDC->FillEllipse(D2D1::Ellipse(pos, colRadius, colRadius), brushCyanFill);
                float pulseW = 1.6f + 0.9f * sinf(dwTime * 0.006f + p.x * 0.08f + p.y * 0.08f);
                g_pD2DDC->DrawEllipse(D2D1::Ellipse(pos, colRadius, colRadius), brushCyan, pulseW);
            }
            if (g_debugVelocity) {
                float vLen = sqrtf(p.vx * p.vx + p.vy * p.vy);
                if (vLen > 0.1f) {
                    D2D1_POINT_2F vEnd = D2D1::Point2F(p.x + p.vx * VEC_SCALE, p.y + p.vy * VEC_SCALE);
                    g_pD2DDC->DrawLine(pos, vEnd, brushGreen, 2.0f);
                    float angle = atan2f(p.vy, p.vx);
                    float arrowLen = 5.0f;
                    D2D1_POINT_2F a1 = D2D1::Point2F(vEnd.x - cosf(angle - 0.4f) * arrowLen,
                                                      vEnd.y - sinf(angle - 0.4f) * arrowLen);
                    D2D1_POINT_2F a2 = D2D1::Point2F(vEnd.x - cosf(angle + 0.4f) * arrowLen,
                                                      vEnd.y - sinf(angle + 0.4f) * arrowLen);
                    g_pD2DDC->DrawLine(vEnd, a1, brushGreen, 2.0f);
                    g_pD2DDC->DrawLine(vEnd, a2, brushGreen, 2.0f);
                    float dotPh = fmodf(animT * 2.5f + p.x * 0.013f + p.y * 0.017f, 1.0f);
                    D2D1_POINT_2F dotP = D2D1::Point2F(pos.x + (vEnd.x - pos.x) * dotPh,
                                                        pos.y + (vEnd.y - pos.y) * dotPh);
                    g_pD2DDC->FillEllipse(D2D1::Ellipse(dotP, 2.5f, 2.5f), brushGreen);
                }
            }
            if (g_debugForce) {
                float fLen = sqrtf(p.debugForceX * p.debugForceX + p.debugForceY * p.debugForceY);
                if (fLen > 0.02f) {
                    D2D1_POINT_2F fEnd = D2D1::Point2F(p.x + p.debugForceX * FORCE_SCALE,
                                                        p.y + p.debugForceY * FORCE_SCALE);
                    g_pD2DDC->DrawLine(pos, fEnd, brushRed, 1.8f);
                    float angle = atan2f(p.debugForceY, p.debugForceX);
                    float arrowLen = 4.0f;
                    D2D1_POINT_2F a1 = D2D1::Point2F(fEnd.x - cosf(angle - 0.4f) * arrowLen,
                                                      fEnd.y - sinf(angle - 0.4f) * arrowLen);
                    D2D1_POINT_2F a2 = D2D1::Point2F(fEnd.x - cosf(angle + 0.4f) * arrowLen,
                                                      fEnd.y - sinf(angle + 0.4f) * arrowLen);
                    g_pD2DDC->DrawLine(fEnd, a1, brushRed, 1.8f);
                    g_pD2DDC->DrawLine(fEnd, a2, brushRed, 1.8f);
                    float dotPh = fmodf(animT * 3.0f + p.x * 0.019f + p.y * 0.011f, 1.0f);
                    D2D1_POINT_2F dotP = D2D1::Point2F(pos.x + (fEnd.x - pos.x) * dotPh,
                                                        pos.y + (fEnd.y - pos.y) * dotPh);
                    g_pD2DDC->FillEllipse(D2D1::Ellipse(dotP, 2.2f, 2.2f), brushRed);
                }
            }
        }
    }
    if (g_debugSpring && g_enableParticleSpring && g_particles.size() >= 2) {
        int pc = (int)g_particles.size();
        float springRestDbg = g_springMaxDist * 0.5f;
        for (int i = 0; i < pc; i++) {
            for (int j = i + 1; j < pc; j++) {
                float dx = g_particles[i].x - g_particles[j].x;
                float dy = g_particles[i].y - g_particles[j].y;
                float d2 = dx * dx + dy * dy;
                if (d2 < 0.25f || d2 > g_springMaxDist * g_springMaxDist) continue;
                float dist = sqrtf(d2);
                float tension = (dist - springRestDbg) / g_springMaxDist;
                float t = fminf(fmaxf(tension * 2.0f + 0.5f, 0.0f), 1.0f);
                D2D1_POINT_2F a = D2D1::Point2F(g_particles[i].x, g_particles[i].y);
                D2D1_POINT_2F b = D2D1::Point2F(g_particles[j].x, g_particles[j].y);
                if (t < 0.5f)
                    g_pD2DDC->DrawLine(a, b, brushSpringLoose, 1.2f);
                else {
                    g_pD2DDC->DrawLine(a, b, brushSpringTaut, 1.6f);
                    float pulsePh = fmodf(animT * 3.5f + i * 0.3f + j * 0.17f, 1.0f);
                    D2D1_POINT_2F pp = D2D1::Point2F(a.x + (b.x - a.x) * pulsePh,
                                                     a.y + (b.y - a.y) * pulsePh);
                    g_pD2DDC->FillEllipse(D2D1::Ellipse(pp, 2.8f, 2.8f), brushSpringTaut);
                }
            }
        }
    }
    if (g_debugVortex && g_enableCentripetal) {
        ID2D1SolidColorBrush *brushBlueGlow = nullptr;
        g_pD2DDC->CreateSolidColorBrush(D2D1::ColorF(0.0f, 0.6f, 1.0f, 0.10f), &brushBlueGlow);
        for (int vi = 0; vi < g_vortexMaxCount; vi++) {
            Vortex &v = g_vortices[vi];
            if (!v.active || v.strength <= 0.02f) continue;
            D2D1_POINT_2F center = D2D1::Point2F(v.centerX, v.centerY);
            int rotDir = (v.angularVel >= 0.0f) ? 1 : -1;
            float pulse = 0.85f + 0.15f * sinf(v.orbitPhase * 3.0f);
            float glowR = v.coreRadius * 3.5f + v.strength * 60.0f;
            g_pD2DDC->FillEllipse(D2D1::Ellipse(center, glowR, glowR), brushBlueGlow);
            float orbitRs[3] = {
                v.coreRadius * 1.8f + 10.0f,
                v.coreRadius * 3.0f + 22.0f,
                v.coreRadius * 4.8f + 38.0f
            };
            for (int ri = 0; ri < 3; ri++) {
                float r = orbitRs[ri] * (0.9f + v.strength * 0.25f) * pulse;
                g_pD2DDC->DrawEllipse(D2D1::Ellipse(center, r, r), brushBlue, 1.0f);
                for (int k = 0; k < 4; k++) {
                    float ang = v.orbitPhase + k * 1.5707963f;
                    float px = center.x + cosf(ang) * r;
                    float py = center.y + sinf(ang) * r;
                    float tx = -sinf(ang) * rotDir;
                    float ty = cosf(ang) * rotDir;
                    float arrowLen = 7.0f + v.strength * 4.0f;
                    D2D1_POINT_2F tip = D2D1::Point2F(px + tx * arrowLen, py + ty * arrowLen);
                    g_pD2DDC->DrawLine(D2D1::Point2F(px, py), tip, brushBlue, 1.8f);
                    float wing = 3.5f;
                    g_pD2DDC->DrawLine(tip, D2D1::Point2F(tip.x - tx * wing - ty * wing * 0.6f,
                                                           tip.y - ty * wing + tx * wing * 0.6f), brushBlue, 1.4f);
                    g_pD2DDC->DrawLine(tip, D2D1::Point2F(tip.x - tx * wing + ty * wing * 0.6f,
                                                           tip.y - ty * wing - tx * wing * 0.6f), brushBlue, 1.4f);
                }
            }
            g_pD2DDC->DrawEllipse(D2D1::Ellipse(center, v.coreRadius, v.coreRadius), brushBlue, 2.0f);
            float strR = v.coreRadius + 6.0f + v.strength * 12.0f;
            g_pD2DDC->DrawEllipse(D2D1::Ellipse(center, strR, strR), brushBlue, 1.0f + v.strength * 3.0f);
            g_pD2DDC->DrawLine(D2D1::Point2F(center.x - 12, center.y),
                               D2D1::Point2F(center.x + 12, center.y), brushBlue, 2.0f);
            g_pD2DDC->DrawLine(D2D1::Point2F(center.x, center.y - 12),
                               D2D1::Point2F(center.x, center.y + 12), brushBlue, 2.0f);
            g_pD2DDC->FillEllipse(D2D1::Ellipse(center, 3.0f, 3.0f), brushBlue);
            float dvLen = sqrtf(v.driftX * v.driftX + v.driftY * v.driftY);
            if (dvLen > 0.05f) {
                D2D1_POINT_2F dEnd = D2D1::Point2F(center.x + v.driftX * 8.0f,
                                                    center.y + v.driftY * 8.0f);
                g_pD2DDC->DrawLine(center, dEnd, brushBlue, 1.5f);
            }
        }
        for (int mi = 0; mi < g_vortexMaxCount; mi++) {
            if (!g_vortices[mi].active) continue;
            for (int mj = mi + 1; mj < g_vortexMaxCount; mj++) {
                if (!g_vortices[mj].active) continue;
                Vortex &va = g_vortices[mi], &vb = g_vortices[mj];
                float mdx = va.centerX - vb.centerX, mdy = va.centerY - vb.centerY;
                float mdist = sqrtf(mdx*mdx + mdy*mdy);
                float mergeR = (va.coreRadius + vb.coreRadius) * 1.8f;
                if (mdist < mergeR && mdist > 0.1f) {
                    D2D1_POINT_2F ma = D2D1::Point2F(va.centerX, va.centerY);
                    D2D1_POINT_2F mb = D2D1::Point2F(vb.centerX, vb.centerY);
                    float linkAlpha = 1.0f - mdist / mergeR;
                    g_pD2DDC->DrawLine(ma, mb, brushBlue, 1.0f + linkAlpha * 2.0f);
                    float lp = fmodf(animT * 2.0f + mi * 0.5f, 1.0f);
                    D2D1_POINT_2F lep = D2D1::Point2F(ma.x + (mb.x - ma.x) * lp,
                                                      ma.y + (mb.y - ma.y) * lp);
                    g_pD2DDC->FillEllipse(D2D1::Ellipse(lep, 3.0f, 3.0f), brushBlue);
                }
            }
        }
        if (brushBlueGlow) brushBlueGlow->Release();
    }
    if (g_debugInteraction && g_enableParticleInteraction) {
        int pc = (int)g_particles.size();
        float repelDist = (float)g_interParticleRepelDistance;
        float repelDistSq = repelDist * repelDist;
        for (int i = 0; i < pc; i++) {
            for (int j = i + 1; j < pc; j++) {
                float dx = g_particles[i].x - g_particles[j].x;
                float dy = g_particles[i].y - g_particles[j].y;
                float d2 = dx * dx + dy * dy;
                if (d2 >= repelDistSq || d2 < 0.01f) continue;
                float dist = sqrtf(d2);
                float falloff = 1.0f - dist / repelDist;
                float strength = falloff * falloff;
                float lw = 0.8f + strength * 2.5f;
                D2D1_POINT_2F a = D2D1::Point2F(g_particles[i].x, g_particles[i].y);
                D2D1_POINT_2F b = D2D1::Point2F(g_particles[j].x, g_particles[j].y);
                g_pD2DDC->DrawLine(a, b, brushMagenta, lw);
                float pulsePh = fmodf(animT * 4.0f + i * 0.37f + j * 0.19f, 1.0f);
                D2D1_POINT_2F mp = D2D1::Point2F(a.x + (b.x - a.x) * pulsePh,
                                                  a.y + (b.y - a.y) * pulsePh);
                g_pD2DDC->FillEllipse(D2D1::Ellipse(mp, 1.5f + strength * 2.0f, 1.5f + strength * 2.0f), brushMagenta);
            }
        }
    }
    if (g_debugSpawn) {
        while (!g_debugSpawnPoints.empty() && dwTime - g_debugSpawnPoints.front().time > 800)
            g_debugSpawnPoints.erase(g_debugSpawnPoints.begin());
        for (auto &sp : g_debugSpawnPoints) {
            float age = (float)(dwTime - sp.time) / 800.0f;
            if (age >= 1.0f) continue;
            float alpha = 1.0f - age;
            D2D1_POINT_2F spPos = D2D1::Point2F(sp.x, sp.y);
            float r = 3.0f + age * 4.0f;
            ID2D1SolidColorBrush *tmpBrush = nullptr;
            g_pD2DDC->CreateSolidColorBrush(D2D1::ColorF(1.0f, 0.65f, 0.0f, alpha * 0.9f), &tmpBrush);
            g_pD2DDC->DrawEllipse(D2D1::Ellipse(spPos, r, r), tmpBrush, 1.5f);
            g_pD2DDC->FillEllipse(D2D1::Ellipse(spPos, 2.0f, 2.0f), tmpBrush);
            if (tmpBrush) tmpBrush->Release();
        }
        if (!smoothed.empty()) {
            D2D1_POINT_2F cursorPos = smoothed[0];
            float spreadR = (float)g_particleSpawnSpread;
            if (spreadR > 0.5f) {
                g_pD2DDC->FillEllipse(D2D1::Ellipse(cursorPos, spreadR, spreadR), brushOrangeFill);
                float dashPulse = 0.7f + 0.3f * sinf(dwTime * 0.008f);
                g_pD2DDC->DrawEllipse(D2D1::Ellipse(cursorPos, spreadR, spreadR), brushOrange, 1.5f * dashPulse);
            }
        }
    }
    if (g_debugPath) {
        for (auto &hp : g_history) {
            g_pD2DDC->FillEllipse(D2D1::Ellipse(D2D1::Point2F(hp.x, hp.y), 1.8f, 1.8f), brushGray);
        }
        for (size_t i = 0; i < smoothed.size(); i++) {
            g_pD2DDC->FillEllipse(D2D1::Ellipse(smoothed[i], 2.5f, 2.5f), brushWhite);
            if (i > 0) {
                g_pD2DDC->DrawLine(smoothed[i - 1], smoothed[i], brushWhite, 1.0f);
            }
        }
        if (!smoothed.empty()) {
            g_pD2DDC->DrawEllipse(D2D1::Ellipse(smoothed[0], 6.0f, 6.0f), brushWhite, 2.0f);
        }
    }
    if (g_debugGravity && g_enableParticleGravity) {
        int n = (int)g_particles.size();
        int bodyCount = (g_gravitySystem == 0) ? 2 : ClampInt(g_gravityBodyCount, 2, 10);
        int drawCount = bodyCount < n ? bodyCount : n;
        std::vector<int> indices(n);
        for (int i = 0; i < n; i++) indices[i] = i;
        std::partial_sort(indices.begin(), indices.begin() + drawCount, indices.end(),
                          [&](int a, int b) { return g_particles[a].mass > g_particles[b].mass; });
        for (int i = 0; i < drawCount; i++) {
            Particle &p = g_particles[indices[i]];
            D2D1_POINT_2F pos = D2D1::Point2F(p.x, p.y);
            float r = 6 + p.mass * 0.5f;
            g_pD2DDC->FillEllipse(D2D1::Ellipse(pos, r * 0.6f, r * 0.6f), brushYellow);
            g_pD2DDC->DrawEllipse(D2D1::Ellipse(pos, r, r), brushYellow, 2.0f);
            g_pD2DDC->DrawEllipse(D2D1::Ellipse(pos, r * 5, r * 5), brushYellow, 0.8f);
            for (int s = 0; s < 2; s++) {
                float orbAng = animT * 6.28318f * 1.2f + i * 3.14159f + s * 3.14159f;
                D2D1_POINT_2F sp = D2D1::Point2F(pos.x + cosf(orbAng) * r * 5.0f,
                                                  pos.y + sinf(orbAng) * r * 5.0f);
                g_pD2DDC->FillEllipse(D2D1::Ellipse(sp, 2.5f, 2.5f), brushYellow);
            }
        }
    }
    if (brushGreen) brushGreen->Release();
    if (brushRed) brushRed->Release();
    if (brushBlue) brushBlue->Release();
    if (brushYellow) brushYellow->Release();
    if (brushCyan) brushCyan->Release();
    if (brushCyanFill) brushCyanFill->Release();
    if (brushSpringLoose) brushSpringLoose->Release();
    if (brushSpringTaut) brushSpringTaut->Release();
    if (brushMagenta) brushMagenta->Release();
    if (brushOrange) brushOrange->Release();
    if (brushOrangeFill) brushOrangeFill->Release();
    if (brushWhite) brushWhite->Release();
    if (brushGray) brushGray->Release();
}
static bool NativeRenderFrame(int screenW, int screenH, const std::vector<D2D1_POINT_2F>& smoothed,
                              bool tailVisible, const GradData& cols, float widthMul, float fadeAlpha,
                              DWORD dwTime, int vX, int vY) {
    if (!g_pNativeVS || !g_pD3DContext || !g_pCachedRTV) return false;
    bool trailOn = g_trailVisible.load(std::memory_order_relaxed);
    static std::vector<D2D1_POINT_2F> s_dots, s_line1, s_line2, s_segment, s_branch, s_barb;
    static std::vector<float> s_bnx, s_bny;
    if (g_charAtlasDirty && g_particleShape == 10) {
        g_charAtlasDirty.exchange(false);
        BuildCharAtlas();
    }
    UpdateCharAtlasTags();
    float clearColor[4] = {0, 0, 0, 0};
    g_pD3DContext->ClearRenderTargetView(g_pCachedRTV, clearColor);
    g_pD3DContext->OMSetRenderTargets(1, &g_pCachedRTV, nullptr);
    int renderW = screenW, renderH = screenH;
    if (g_ssaaScale > 1) {
        renderW = screenW * g_ssaaScale;
        renderH = screenH * g_ssaaScale;
    }
    g_renderW = renderW;
    g_renderH = renderH;
    D3D11_VIEWPORT vp = {0, 0, (float)renderW, (float)renderH, 0, 1};
    g_pD3DContext->RSSetViewports(1, &vp);
    if (!g_particles.empty()) {
        NativeRenderParticles(screenW, screenH);
    }
    struct LineSegBatch {
        enum { MAX_LAYERS = 1024 };
        enum { MAX_VTX = kMaxTrailVertices };
        struct Layer { int offset, count; };
        Layer layers[MAX_LAYERS];
        int layerCount = 0;
        VertexPosColor* dst = nullptr;
        int vtxOffset = 0;
        bool mapped = false;
        bool mapFailed = false;
        float r = 0, g = 0, b = 0;
        int screenW = 0, screenH = 0;
        void open(const GradData& cols, int sw, int sh) {
            r = cols.solidOuter.r; g = cols.solidOuter.g; b = cols.solidOuter.b;
            screenW = sw; screenH = sh;
            SetNativeRenderState(screenW, screenH, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP, &cols);
        }
        bool ensure() {
            if (mapped) return true;
            if (mapFailed) return false;
            D3D11_MAPPED_SUBRESOURCE ms;
            if (FAILED(g_pD3DContext->Map(g_pTrailVB, 0, D3D11_MAP_WRITE_DISCARD, 0, &ms))) {
                mapFailed = true;
                return false;
            }
            dst = (VertexPosColor*)ms.pData;
            vtxOffset = 0;
            layerCount = 0;
            mapped = true;
            return true;
        }
        void flush() {
            if (!mapped) return;
            g_pD3DContext->Unmap(g_pTrailVB, 0);
            mapped = false;
            for (int i = 0; i < layerCount; i++)
                g_pD3DContext->Draw(layers[i].count, layers[i].offset);
            layerCount = 0;
        }
        void push(const std::vector<D2D1_POINT_2F>& pts, float width, float fadeAlpha,
                  std::vector<float>& nx, std::vector<float>& ny) {
            size_t sl = pts.size();
            if (sl < 2) return;
            if (!ensure()) return;
            int need = (int)sl * 2;
            if (vtxOffset + need > (int)MAX_VTX || layerCount >= (int)MAX_LAYERS) {
                flush();
                if (!ensure()) return;
            }
            ComputePathNormals(pts, nx, ny);
            int c = BuildNativeStrip(dst + vtxOffset, (int)MAX_VTX - vtxOffset, pts, nx, ny,
                                     false, 0.0f, 0.0f, width, r, g, b, fadeAlpha, 1.0f, 0, 0, false);
            if (c > 0) { layers[layerCount++] = {vtxOffset, c}; vtxOffset += c; }
        }
        void close() { flush(); }
    };
    if (trailOn && tailVisible && !smoothed.empty() && g_trailShape != 4 && g_trailShape != 10) {
        if (g_trailShape == 1) {
            std::vector<D2D1_POINT_2F>& dots = s_dots; dots.clear();
            int dotCount = (int)smoothed.size() * g_dotsMultiplier;
            if (dotCount > 200) dotCount = 200;
            if (smoothed.size() < 2) {
                for (int i = 0; i < dotCount; i++)
                    dots.push_back(smoothed.empty() ? D2D1::Point2F(0, 0) : smoothed[0]);
            } else {
                float totalLen = 0;
                for (size_t i = 1; i < smoothed.size(); i++) {
                    float dx = smoothed[i].x - smoothed[i - 1].x, dy = smoothed[i].y - smoothed[i - 1].y;
                    totalLen += sqrtf(dx * dx + dy * dy);
                }
                size_t j = 1;
                float acc = 0;
                for (int i = 0; i < dotCount; i++) {
                    float t = dotCount > 1 ? i / (float)(dotCount - 1) : 0;
                    D2D1_POINT_2F p;
                    bool found = false;
                    if (t <= 0) {
                        p = smoothed[0]; found = true;
                    } else if (t >= 1) {
                        p = smoothed.back(); found = true;
                    } else {
                        float target = t * totalLen;
                        while (j + 1 < smoothed.size()) {
                            float dx = smoothed[j].x - smoothed[j - 1].x, dy = smoothed[j].y - smoothed[j - 1].y;
                            float segLen = sqrtf(dx * dx + dy * dy);
                            if (acc + segLen >= target) {
                                float f = segLen > 0 ? (target - acc) / segLen : 0;
                                p = D2D1::Point2F(smoothed[j - 1].x + dx * f, smoothed[j - 1].y + dy * f);
                                found = true;
                                break;
                            }
                            acc += segLen;
                            j++;
                        }
                    }
                    dots.push_back(found ? p : smoothed.back());
                }
            }
            float dotSize = 4.0f * widthMul * (g_dotChainSize / 100.0f);
            NativeRenderDotChain(dots, dotSize, cols, fadeAlpha, screenW, screenH);
        } else if (g_trailShape == 5) {
            std::vector<D2D1_POINT_2F>& line1 = s_line1; line1.clear();
            std::vector<D2D1_POINT_2F>& line2 = s_line2; line2.clear();
            float offset = 4.0f;
            for (size_t i = 0; i < smoothed.size(); i++) {
                float ddx, ddy;
                if (i == 0) { ddx = smoothed[1].x - smoothed[0].x; ddy = smoothed[1].y - smoothed[0].y; }
                else if (i == smoothed.size() - 1) { ddx = smoothed[i].x - smoothed[i-1].x; ddy = smoothed[i].y - smoothed[i-1].y; }
                else { ddx = smoothed[i+1].x - smoothed[i-1].x; ddy = smoothed[i+1].y - smoothed[i-1].y; }
                float ln = sqrtf(ddx*ddx + ddy*ddy);
                if (ln > 0) { ddx /= ln; ddy /= ln; }
                float nx = -ddy, ny = ddx;
                line1.push_back({smoothed[i].x + nx * offset, smoothed[i].y + ny * offset});
                line2.push_back({smoothed[i].x - nx * offset, smoothed[i].y - ny * offset});
            }
            DrawTrailBand(line1, cols, widthMul * 0.5f, fadeAlpha, screenW, screenH);
            DrawTrailBand(line2, cols, widthMul * 0.5f, fadeAlpha, screenW, screenH);
        } else if (g_trailShape == 6) {
            std::vector<D2D1_POINT_2F>& segment = s_segment; segment.clear();
            int segLen = 6, gapLen = 4;
            LineSegBatch batch;
            batch.open(cols, screenW, screenH);
            for (size_t i = 0; i < smoothed.size(); i++) {
                segment.push_back(smoothed[i]);
                if ((int)(i + 1) % (segLen + gapLen) == 0) {
                    if (segment.size() >= 2)
                        batch.push(segment, 6.0f * widthMul, fadeAlpha, s_bnx, s_bny);
                    segment.clear();
                    i += gapLen;
                }
            }
            if (segment.size() >= 2)
                batch.push(segment, 6.0f * widthMul, fadeAlpha, s_bnx, s_bny);
            batch.close();
        } else if (g_trailShape == 8) {
            NativeRenderLineTrail(smoothed, 3.0f * widthMul, cols, fadeAlpha, screenW, screenH);
            LineSegBatch batch;
            batch.open(cols, screenW, screenH);
            for (size_t i = 2; i + 2 < smoothed.size(); i += 3) {
                if ((rand() / (float)RAND_MAX) > 0.35f) continue;
                float dx = smoothed[i+1].x - smoothed[i-1].x;
                float dy = smoothed[i+1].y - smoothed[i-1].y;
                float ln = sqrtf(dx*dx + dy*dy);
                if (ln < 0.001f) continue;
                float nx = -dy / ln, ny = dx / ln;
                float blen = 15.0f + (rand() / (float)RAND_MAX) * 25.0f;
                float side = ((rand() / (float)RAND_MAX) > 0.5f) ? 1.0f : -1.0f;
                std::vector<D2D1_POINT_2F>& branch = s_branch; branch.clear();
                branch.push_back(smoothed[i]);
                branch.push_back({smoothed[i].x + nx * side * blen * 0.5f + dx * 0.3f,
                                  smoothed[i].y + ny * side * blen * 0.5f + dy * 0.3f});
                branch.push_back({smoothed[i].x + nx * side * blen,
                                  smoothed[i].y + ny * side * blen});
                batch.push(branch, 1.5f * widthMul, fadeAlpha * 0.6f, s_bnx, s_bny);
            }
            batch.close();
        } else if (g_trailShape == 9) {
            NativeRenderLineTrail(smoothed, 2.5f * widthMul, cols, fadeAlpha, screenW, screenH);
            LineSegBatch batch;
            batch.open(cols, screenW, screenH);
            for (size_t i = 1; i + 1 < smoothed.size(); i += 2) {
                float dx = smoothed[i+1].x - smoothed[i-1].x;
                float dy = smoothed[i+1].y - smoothed[i-1].y;
                float ln = sqrtf(dx*dx + dy*dy);
                if (ln < 0.001f) continue;
                float nx = -dy / ln, ny = dx / ln;
                float barbLen = 10.0f * (1.0f - (float)i / smoothed.size()) + 3.0f;
                float bx = -dx / ln, by = -dy / ln;
                for (int side = -1; side <= 1; side += 2) {
                    std::vector<D2D1_POINT_2F>& barb = s_barb; barb.clear();
                    barb.push_back(smoothed[i]);
                    barb.push_back({smoothed[i].x + nx * side * barbLen + bx * barbLen * 0.4f,
                                    smoothed[i].y + ny * side * barbLen + by * barbLen * 0.4f});
                    batch.push(barb, 1.0f * widthMul, fadeAlpha * 0.5f, s_bnx, s_bny);
                }
            }
            batch.close();
        } else {
            DrawTrailBand(smoothed, cols, widthMul, fadeAlpha, screenW, screenH);
        }
    }
    if (g_enableClickEffect && !g_ripples.empty()) {
        DrawClickEffects(screenW, screenH, dwTime, cols, vX, vY);
    }
    if (trailOn && g_enableMotionBlur && tailVisible && !g_trailHistory.empty() && g_trailShape != 10) {
        for (size_t h = 0; h < g_trailHistory.size(); h++) {
            float histAlpha = fadeAlpha * (0.15f + 0.1f * (float)h / g_trailHistory.size());
            if (histAlpha < 0.02f) continue;
            const std::vector<D2D1_POINT_2F>& histPath = g_trailHistory[h].path;
            if (histPath.size() < 2) continue;
            DrawTrailBand(histPath, cols, widthMul, histAlpha, screenW, screenH);
        }
    }
    if (trailOn && !g_trailShapes.empty()) {
        DrawTrailShapes(screenW, screenH, dwTime);
    }
    g_pD2DDC->SetTarget(g_pD2DTargetBitmap);
    return true;
}
static inline float Rand01() {
    return rand() / (float)RAND_MAX;
}
static inline int GetParticleCap() {
    return g_superPerformanceMode ? 1200 : 500;
}
static const DWORD kFillMaxIntervalMs = 10;
static bool CanInsertFillers(float dist, float dx, float dy, float prevDirX, float prevDirY,
                             float tolerancePx) {
    if (g_particleInterval > (int)kFillMaxIntervalMs) return false;
    float dirLen = sqrtf(prevDirX * prevDirX + prevDirY * prevDirY);
    if (dirLen < 0.5f) return false;
    if (dist < 1e-3f) return false;
    float cx = dx / dist, cy = dy / dist;
    float nx = prevDirX / dirLen, ny = prevDirY / dirLen;
    float dot = nx * cx + ny * cy;
    dot = ClampF(dot, -1.0f, 1.0f);
    float phi = acosf(dot);
    if (phi > 2.268928f) return false;
    float estErr = 0.5f * dist * phi * phi;
    return estErr <= tolerancePx * 3.5f;
}
static inline float Hash01(int n) {
    uint32_t u = (uint32_t)n;
    u = (u << 13) ^ u;
    return (float)(((u * (u * u * 15731u + 789221u) + 1376312589u) & 0x7fffffffu) / 2147483647.0);
}
static void SpawnParticles(float x, float y, int count, float speedMin, float speedMax, float sizeMin, float sizeMax,
                           int lifeMin, int lifeMax, D2D1_COLOR_F color, DWORD time, bool radial = false,
                           int shapeType = -1) {
    int cap = GetParticleCap();
    int room = cap - (int)g_particles.size();
    if (room <= 0) return;
    if (count > room) count = room;
    if (g_debugSpawn) {
        g_debugSpawnPoints.push_back({x, y, time});
        if (g_debugSpawnPoints.size() > 40) g_debugSpawnPoints.erase(g_debugSpawnPoints.begin());
    }
    D2D1_COLOR_F endCol = D2D1::ColorF(color.r * 0.35f, color.g * 0.35f, color.b * 0.35f, 1.0f);
    float massMean = (g_particleMassMin + g_particleMassMax) * 0.5f;
    float massStddev = (g_particleMassMax - g_particleMassMin) * 0.25f;
    float baseSpin = (float)g_particleSpinSpeed / 100.0f * 0.3f;
    float twoPi = 6.2831853f;
    float radialPhase = radial ? (Rand01() * twoPi) : 0.0f;
    for (int i = 0; i < count; i++) {
        float angle = radial ? (radialPhase + i / (float)count * twoPi) : (Rand01() * twoPi);
        float speed = speedMin + Rand01() * (speedMax - speedMin);
        int st = shapeType;
        if (st < 0) {
            st = g_particleShape;
            if (st == 9)
                st = (int)(Rand01() * 9.0f);
        }
        float spawnAngle = Rand01() * twoPi;
        float spawnRadius = Rand01() * (float)g_particleSpawnSpread;
        float px = x + cosf(spawnAngle) * spawnRadius;
        float py = y + sinf(spawnAngle) * spawnRadius;
        if (st == 10) { px += (float)g_textOffsetX; py += (float)g_textOffsetY; }
        Particle p;
        p.x = px; p.y = py;
        p.vx = cosf(angle) * speed; p.vy = sinf(angle) * speed;
        float mass = 1.0f;
        if (g_enableParticleMass) {
            float u1 = fmaxf(Rand01(), 0.001f);
            float u2 = Rand01();
            float z = sqrtf(-2.0f * logf(u1)) * cosf(twoPi * u2);
            mass = massMean + z * massStddev;
            mass = fmaxf(g_particleMassMin, fminf(g_particleMassMax, mass));
        }
        p.mass = mass;
        p.charge = (Rand01() > 0.5f) ? 1.0f : -1.0f;
        float massSizeFactor = 1.0f;
        if (g_enableParticleMass) {
            float m = mass * 0.1f;
            massSizeFactor = 0.7f + m * 0.3f;
        }
        p.size = (sizeMin + Rand01() * (sizeMax - sizeMin)) * massSizeFactor;
        p.startTime = time;
        int baseLife = lifeMin + (int)(Rand01() * (lifeMax - lifeMin));
        p.lifetime = g_enableParticleMass ? (int)(baseLife * (0.7f + mass * 0.3f)) : baseLife;
        p.color = color;
        p.endColor = endCol;
        p.shapeType = st;
        p.charIndex = 0;
        p.chunkStart = 0;
        p.chunkCount = 1;
        if (st == 10 && !g_phraseChunkStart.empty()) {
            int phraseCount = (int)g_phraseChunkStart.size();
            if (phraseCount > (int)g_phraseChunkCount.size()) phraseCount = (int)g_phraseChunkCount.size();
            if (phraseCount <= 0) phraseCount = 1;
            int pi = (int)(Rand01() * (float)phraseCount);
            if (pi >= phraseCount) pi = phraseCount - 1;
            p.chunkStart = g_phraseChunkStart[pi];
            p.chunkCount = g_phraseChunkCount[pi];
            if (p.chunkCount < 1) p.chunkCount = 1;
            p.charIndex = (float)p.chunkStart;
        }
        p.z = (Rand01() - 0.5f) * 0.8f;
        p.rotation = Rand01() * twoPi;
        float variation = (Rand01() - 0.5f) * baseSpin;
        p.spinSpeed = baseSpin + variation;
        float r1 = Rand01() - 0.5f;
        float r2 = Rand01() - 0.5f;
        float r3 = Rand01() - 0.5f;
        p.colorOffset[0] = r1 * 0.16f;
        p.colorOffset[1] = r2 * 0.16f;
        p.colorOffset[2] = r3 * 0.16f;
        p.vortexBrightness[0] = 0;
        p.vortexBrightness[1] = 0;
        p.vortexBrightness[2] = 0;
        p.debugForceX = 0;
        p.debugForceY = 0;
        g_particles.push_back(p);
    }
}
static void FFT(std::vector<float> &real, std::vector<float> &imag, bool inverse = false) {
    int n = (int)real.size();
    if (n <= 1) return;
    if ((n & (n - 1)) != 0) return;
    if ((int)imag.size() != n) return;
    for (int i = 1, j = 0; i < n; i++) {
        int bit = n >> 1;
        for (; j & bit; bit >>= 1) j ^= bit;
        j ^= bit;
        if (i < j) {
            std::swap(real[i], real[j]);
            std::swap(imag[i], imag[j]);
        }
    }
    for (int len = 2; len <= n; len <<= 1) {
        float ang = (inverse ? 2.0f : -2.0f) * 3.14159265358979f / len;
        float wlenR = cosf(ang), wlenI = sinf(ang);
        for (int i = 0; i < n; i += len) {
            float wR = 1, wI = 0;
            for (int j = 0; j < len / 2; j++) {
                float uR = real[i + j], uI = imag[i + j];
                float vR = real[i + j + len / 2] * wR - imag[i + j + len / 2] * wI;
                float vI = real[i + j + len / 2] * wI + imag[i + j + len / 2] * wR;
                real[i + j] = uR + vR;
                imag[i + j] = uI + vI;
                real[i + j + len / 2] = uR - vR;
                imag[i + j + len / 2] = uI - vI;
                float nextWR = wR * wlenR - wI * wlenI;
                wI = wR * wlenI + wI * wlenR;
                wR = nextWR;
            }
        }
    }
    if (inverse) {
        for (int i = 0; i < n; i++) { real[i] /= n; imag[i] /= n; }
    }
}
static void InitWindowFunction(std::vector<float> &window, int size) {
    if (size < 2) { window.clear(); return; }
    window.resize(size);
    for (int i = 0; i < size; i++) {
        window[i] = 0.5f * (1.0f - cosf(2.0f * 3.14159265358979f * i / (size - 1)));
    }
}
static DWORD WINAPI MusicCaptureThread(LPVOID) {
    MusicState *m = &g_music;
    DWORD taskIndex = 0;
    HANDLE hAvrt = AvSetMmThreadCharacteristicsW(L"Audio", &taskIndex);
    while (m->captureRunning.load()) {
        if (!m->pCaptureClient || m->pWaveFormat == nullptr || m->fftSize <= 1 ||
            m->fftInput.size() < (size_t)m->fftSize) {
            Sleep(1);
            continue;
        }
        UINT32 packetLength = 0;
        HRESULT hr = m->pCaptureClient->GetNextPacketSize(&packetLength);
        if (FAILED(hr) || packetLength == 0) {
            WaitForSingleObject(m->hStopEvent, 5);
            continue;
        }
        BYTE *pData = nullptr;
        UINT32 numFrames = 0;
        DWORD flags = 0;
        hr = m->pCaptureClient->GetBuffer(&pData, &numFrames, &flags, nullptr, nullptr);
        if (SUCCEEDED(hr) && pData && numFrames > 0) {
            int channels = m->pWaveFormat->nChannels;
            if (channels < 1) channels = 1;
            int bytesPerSample = m->pWaveFormat->wBitsPerSample / 8;
            bool isSilent = (flags & AUDCLNT_BUFFERFLAGS_SILENT) != 0;
            std::atomic_ref<int> writePos(m->fftWritePos);
            int wp = writePos.load(std::memory_order_relaxed);
            for (UINT32 i = 0; i < numFrames; i++) {
                float sample = 0;
                if (!isSilent) {
                    if (bytesPerSample == 4) {
                        float *pFloat = (float*)(pData + i * channels * bytesPerSample);
                        for (int c = 0; c < channels; c++) sample += pFloat[c];
                        sample /= channels;
                    } else if (bytesPerSample == 2) {
                        short *pShort = (short*)(pData + i * channels * bytesPerSample);
                        for (int c = 0; c < channels; c++) sample += pShort[c];
                        sample /= (channels * 32768.0f);
                    }
                    if (sample > 1.0f) sample = 1.0f; else if (sample < -1.0f) sample = -1.0f;
                }
                m->fftInput[wp] = sample;
                wp = (wp + 1) % m->fftSize;
            }
            writePos.store(wp, std::memory_order_release);
            m->pCaptureClient->ReleaseBuffer(numFrames);
        }
    }
    if (hAvrt) AvRevertMmThreadCharacteristics(hAvrt);
    return 0;
}
static bool &MusicComOwnedRef() {
    static thread_local bool owned = false;
    return owned;
}
static bool MusicComEnsureInit() {
    HRESULT hrCo = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
    if (FAILED(hrCo)) { Wh_Log(L"Music: CoInitializeEx failed 0x%08X", hrCo); return false; }
    if (hrCo == S_OK) MusicComOwnedRef() = true;
    return true;
}
static void MusicComRelease() {
    bool &owned = MusicComOwnedRef();
    if (owned) { CoUninitialize(); owned = false; }
}
static bool MusicStartCapture() {
    MusicState *m = &g_music;
    if (m->capturing) return true;
    if (!MusicComEnsureInit()) return false;
    REFERENCE_TIME hnsRequestedDuration = 10000000;
    HRESULT hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL,
                                  __uuidof(IMMDeviceEnumerator), (void**)&m->pEnumerator);
    if (FAILED(hr)) { Wh_Log(L"Music: CoCreateInstance failed 0x%08X", hr); MusicComRelease(); return false; }
    hr = m->pEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &m->pDevice);
    if (FAILED(hr)) { Wh_Log(L"Music: GetDefaultAudioEndpoint failed 0x%08X", hr); goto fail; }
    hr = m->pDevice->Activate(__uuidof(IAudioClient), CLSCTX_ALL, nullptr, (void**)&m->pAudioClient);
    if (FAILED(hr)) { Wh_Log(L"Music: Activate failed 0x%08X", hr); goto fail; }
    hr = m->pAudioClient->GetMixFormat(&m->pWaveFormat);
    if (FAILED(hr)) { Wh_Log(L"Music: GetMixFormat failed 0x%08X", hr); goto fail; }
    hr = m->pAudioClient->Initialize(AUDCLNT_SHAREMODE_SHARED,
                                     AUDCLNT_STREAMFLAGS_LOOPBACK,
                                     hnsRequestedDuration, 0, m->pWaveFormat, nullptr);
    if (FAILED(hr)) { Wh_Log(L"Music: Initialize loopback failed 0x%08X", hr); goto fail; }
    hr = m->pAudioClient->GetBufferSize(&m->bufferFrames);
    if (FAILED(hr)) { Wh_Log(L"Music: GetBufferSize failed 0x%08X", hr); goto fail; }
    hr = m->pAudioClient->GetService(__uuidof(IAudioCaptureClient), (void**)&m->pCaptureClient);
    if (FAILED(hr)) { Wh_Log(L"Music: GetService capture failed 0x%08X", hr); goto fail; }
    m->fftSize = g_musicFftSize;
    if (m->fftSize < 64) m->fftSize = 64;
    if (m->fftSize > 4096) m->fftSize = 4096;
    if ((m->fftSize & (m->fftSize - 1)) != 0) m->fftSize = 512;
    m->fftInput.assign(m->fftSize, 0.0f);
    m->fftOutput.assign(m->fftSize / 2, 0.0f);
    m->fftSmoothed.assign(m->fftSize / 2, 0.0f);
    m->fftWorkReal.assign(m->fftSize, 0.0f);
    m->fftWorkImag.assign(m->fftSize, 0.0f);
    InitWindowFunction(m->windowFunction, m->fftSize);
    std::atomic_ref<int>(m->fftWritePos).store(0, std::memory_order_relaxed);
    hr = m->pAudioClient->Start();
    if (FAILED(hr)) { Wh_Log(L"Music: Start failed 0x%08X", hr); goto fail; }
    m->hStopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!m->hStopEvent) { Wh_Log(L"Music: capture event creation failed"); goto fail; }
    m->captureRunning.store(true);
    m->hCaptureThread = CreateThread(nullptr, 0, MusicCaptureThread, nullptr, 0, nullptr);
    if (!m->hCaptureThread) { Wh_Log(L"Music: capture thread creation failed"); goto fail; }
    m->capturing = true;
    m->initialized = true;
    Wh_Log(L"Music: capture started, %d Hz, %d channels, FFT %d",
           m->pWaveFormat->nSamplesPerSec, m->pWaveFormat->nChannels, m->fftSize);
    return true;
fail:
    m->captureRunning.store(false);
    if (m->hCaptureThread) {
        if (WaitForSingleObject(m->hCaptureThread, 1000) != WAIT_OBJECT_0)
            Wh_Log(L"Music: capture thread did not stop in 1s on cleanup");
        CloseHandle(m->hCaptureThread);
        m->hCaptureThread = nullptr;
    }
    if (m->hStopEvent) { CloseHandle(m->hStopEvent); m->hStopEvent = nullptr; }
    if (m->pAudioClient) { m->pAudioClient->Release(); m->pAudioClient = nullptr; }
    if (m->pCaptureClient) { m->pCaptureClient->Release(); m->pCaptureClient = nullptr; }
    if (m->pDevice) { m->pDevice->Release(); m->pDevice = nullptr; }
    if (m->pEnumerator) { m->pEnumerator->Release(); m->pEnumerator = nullptr; }
    if (m->pWaveFormat) { CoTaskMemFree(m->pWaveFormat); m->pWaveFormat = nullptr; }
    MusicComRelease();
    return false;
}
static void MusicStopCapture() {
    MusicState *m = &g_music;
    if (!m->capturing) return;
    m->captureRunning.store(false);
    if (m->hStopEvent) { SetEvent(m->hStopEvent); }
    if (m->hCaptureThread) {
        if (WaitForSingleObject(m->hCaptureThread, 2000) != WAIT_OBJECT_0)
            Wh_Log(L"Music: capture thread did not stop in 2s");
        CloseHandle(m->hCaptureThread);
        m->hCaptureThread = nullptr;
    }
    if (m->hStopEvent) { CloseHandle(m->hStopEvent); m->hStopEvent = nullptr; }
    if (m->pAudioClient) { m->pAudioClient->Stop(); m->pAudioClient->Release(); m->pAudioClient = nullptr; }
    if (m->pCaptureClient) { m->pCaptureClient->Release(); m->pCaptureClient = nullptr; }
    if (m->pDevice) { m->pDevice->Release(); m->pDevice = nullptr; }
    if (m->pEnumerator) { m->pEnumerator->Release(); m->pEnumerator = nullptr; }
    if (m->pWaveFormat) { CoTaskMemFree(m->pWaveFormat); m->pWaveFormat = nullptr; }
    m->capturing = false;
    m->initialized = false;
    MusicComRelease();
    Wh_Log(L"Music: capture stopped");
}
static void MusicUpdate(DWORD dwTime) {
    MusicState *m = &g_music;
    if (!m->capturing || !m->initialized) return;
    int n = m->fftSize;
    if (n < 64 || (n & (n - 1)) != 0) return;
    if (m->fftInput.size() != (size_t)n) return;
    if ((int)m->fftWorkReal.size() != n) {
        m->fftWorkReal.assign(n, 0.0f);
        m->fftWorkImag.assign(n, 0.0f);
    }
    if ((int)m->windowFunction.size() != n) InitWindowFunction(m->windowFunction, n);
    if ((int)m->fftOutput.size() != n / 2) m->fftOutput.assign(n / 2, 0.0f);
    if ((int)m->fftSmoothed.size() != n / 2) m->fftSmoothed.assign(n / 2, 0.0f);
    float maxSample = 0;
    int wp = std::atomic_ref<int>(m->fftWritePos).load(std::memory_order_acquire);
    for (int i = 0; i < n; i++) {
        int idx = (wp + i) % n;
        float sample = m->fftInput[idx];
        m->fftWorkReal[i] = sample * m->windowFunction[i];
        m->fftWorkImag[i] = 0.0f;
        float absSample = fabsf(sample);
        if (absSample > maxSample) maxSample = absSample;
    }
    FFT(m->fftWorkReal, m->fftWorkImag, false);
    float smoothFactor = fminf(g_musicSmoothing * 0.01f, 0.95f);
    float totalEnergy = 0;
    for (int i = 0; i < n / 2; i++) {
        float mag = sqrtf(m->fftWorkReal[i] * m->fftWorkReal[i] + m->fftWorkImag[i] * m->fftWorkImag[i]) / n;
        m->fftOutput[i] = mag;
        m->fftSmoothed[i] = m->fftSmoothed[i] * smoothFactor + mag * (1.0f - smoothFactor);
        totalEnergy += mag * mag;
    }
    if (m->pWaveFormat) {
        float sampleRate = (float)m->pWaveFormat->nSamplesPerSec;
        float freqPerBin = (sampleRate > 0.0f) ? (sampleRate / n) : 1.0f;
        int bassEnd = (int)(250.0f / freqPerBin) + 1;
        int midEnd = (int)(2000.0f / freqPerBin) + 1;
        int trebleEnd = (int)(20000.0f / freqPerBin) + 1;
        if (bassEnd > n / 2) bassEnd = n / 2;
        if (midEnd > n / 2) midEnd = n / 2;
        if (trebleEnd > n / 2) trebleEnd = n / 2;
        m->bassLevel = 0; m->midLevel = 0; m->trebleLevel = 0;
        for (int i = 1; i < bassEnd; i++) m->bassLevel += m->fftSmoothed[i];
        for (int i = bassEnd; i < midEnd; i++) m->midLevel += m->fftSmoothed[i];
        for (int i = midEnd; i < trebleEnd; i++) m->trebleLevel += m->fftSmoothed[i];
        int bassCount = bassEnd - 1;
        int midCount = midEnd - bassEnd;
        int trebleCount = trebleEnd - midEnd;
        if (bassCount > 0) m->bassLevel /= bassCount;
        if (midCount > 0) m->midLevel /= midCount;
        if (trebleCount > 0) m->trebleLevel /= trebleCount;
    }
    m->overallLevel = sqrtf(totalEnergy / ((float)n * 0.5f));
    float sensitivity = 1.15f + g_musicSensitivity * 1.05f;
    float energyGate = 1.05f + g_musicSensitivity * 0.45f;
    float fluxGate = 1.10f + g_musicSensitivity * 0.50f;
    float bassGate = 1.08f + g_musicSensitivity * 0.50f;
    m->beatAvgEnergy = m->beatAvgEnergy * 0.92f + totalEnergy * 0.08f;
    m->beatThreshold = m->beatAvgEnergy * sensitivity;
    bool energyBeat = (totalEnergy > m->beatThreshold) && (totalEnergy > m->beatAvgEnergy * energyGate);
    if (m->prevSpectrum.size() != (size_t)(n / 2)) {
        m->prevSpectrum.assign(n / 2, 0.0f);
    }
    float flux = 0;
    for (int i = 0; i < n / 2; i++) {
        float diff = m->fftSmoothed[i] - m->prevSpectrum[i];
        if (diff > 0) flux += diff;
        m->prevSpectrum[i] = m->fftSmoothed[i];
    }
    m->avgSpectralFlux = m->avgSpectralFlux * 0.9f + flux * 0.1f;
    bool fluxBeat = (flux > m->avgSpectralFlux * sensitivity) && (flux > m->avgSpectralFlux * fluxGate);
    m->bassBeatAvg = m->bassBeatAvg * 0.9f + m->bassLevel * 0.1f;
    m->bassBeat = (m->bassLevel > m->bassBeatAvg * sensitivity) && (m->bassLevel > m->bassBeatAvg * bassGate);
    m->midBeatAvg = m->midBeatAvg * 0.9f + m->midLevel * 0.1f;
    m->midBeat = (m->midLevel > m->midBeatAvg * sensitivity * 1.1f);
    m->trebleBeatAvg = m->trebleBeatAvg * 0.9f + m->trebleLevel * 0.1f;
    m->trebleBeat = (m->trebleLevel > m->trebleBeatAvg * sensitivity * 1.2f);
    bool newBeat = energyBeat || fluxBeat || m->bassBeat;
    float intensity = 0;
    if (energyBeat) intensity += 0.4f * fminf(totalEnergy / fmaxf(m->beatAvgEnergy, 0.001f), 2.0f);
    if (fluxBeat) intensity += 0.3f * fminf(flux / fmaxf(m->avgSpectralFlux, 0.001f), 2.0f);
    if (m->bassBeat) intensity += 0.5f * fminf(m->bassLevel / fmaxf(m->bassBeatAvg, 0.001f), 2.5f);
    if (m->midBeat) intensity += 0.2f;
    if (m->trebleBeat) intensity += 0.15f;
    m->beatIntensity = fminf(intensity, 1.0f);
    if (newBeat && (dwTime - m->lastBeatTime) > 120) {
        m->beatDetected = true;
        m->lastBeatTime = dwTime;
        m->beatHistory.push_back(dwTime);
        if (m->beatHistory.size() > 16) m->beatHistory.pop_front();
        if (m->beatHistory.size() >= 4) {
            float avgInterval = 0;
            for (size_t i = 1; i < m->beatHistory.size(); i++) {
                avgInterval += (m->beatHistory[i] - m->beatHistory[i-1]);
            }
            avgInterval /= (m->beatHistory.size() - 1);
            if (avgInterval > 0) m->bpmEstimate = 60000.0f / avgInterval;
        }
    } else {
        m->beatDetected = false;
    }
    if (g_enableMusicPhysics) {
        if (m->beatDetected) {
            float pulseAdd = m->beatIntensity * g_musicBeatPulse * 1.5f;
            g_musicBeatPulseAmount = fminf(g_musicBeatPulseAmount + pulseAdd, 1.5f);
        } else {
            g_musicBeatPulseAmount *= 0.94f;
        }
        float bassTarget = fminf(m->bassLevel * 15.0f, 1.0f) * g_musicBassSize;
        float bassLerp = (bassTarget > g_musicBassSizeBoost) ? 0.3f : 0.08f;
        g_musicBassSizeBoost += (bassTarget - g_musicBassSizeBoost) * bassLerp;
        if (g_enableMultiBandLink) {
            float bgTarget = fminf(m->bassLevel * 12.0f, 1.0f);
            g_bassGravityBoost += (bgTarget - g_bassGravityBoost) * 0.15f;
            float mrTarget = fminf(m->midLevel * 20.0f, 1.0f);
            g_midRepelBoost += (mrTarget - g_midRepelBoost) * 0.15f;
            float tnTarget = fminf(m->trebleLevel * 50.0f, 1.0f);
            g_trebleNoiseBoost += (tnTarget - g_trebleNoiseBoost) * 0.2f;
        } else {
            g_bassGravityBoost *= 0.9f;
            g_midRepelBoost *= 0.9f;
            g_trebleNoiseBoost *= 0.9f;
        }
    } else {
        g_musicBeatPulseAmount *= 0.85f;
        g_musicBassSizeBoost *= 0.9f;
        g_bassGravityBoost *= 0.9f;
        g_midRepelBoost *= 0.9f;
        g_trebleNoiseBoost *= 0.9f;
    }
}
static float CrKnotSpan(const D2D1_POINT_2F &a, const D2D1_POINT_2F &b, float alpha) {
    float dx = b.x - a.x, dy = b.y - a.y;
    float d = fmaxf(sqrtf(dx * dx + dy * dy), 1e-4f);
    if (alpha == 0.5f) return sqrtf(d);
    return powf(d, alpha);
}
static D2D1_POINT_2F CrEval(const D2D1_POINT_2F &p0, const D2D1_POINT_2F &p1,
                            const D2D1_POINT_2F &p2, const D2D1_POINT_2F &p3,
                            float t0, float t1, float t2, float t3, float t) {
    auto lerpP = [](const D2D1_POINT_2F &a, const D2D1_POINT_2F &b, float u) {
        return D2D1::Point2F(a.x + (b.x - a.x) * u, a.y + (b.y - a.y) * u);
    };
    D2D1_POINT_2F A1 = lerpP(p0, p1, (t - t0) / fmaxf(t1 - t0, 1e-6f));
    D2D1_POINT_2F A2 = lerpP(p1, p2, (t - t1) / fmaxf(t2 - t1, 1e-6f));
    D2D1_POINT_2F A3 = lerpP(p2, p3, (t - t2) / fmaxf(t3 - t2, 1e-6f));
    D2D1_POINT_2F B1 = lerpP(A1, A2, (t - t0) / fmaxf(t2 - t0, 1e-6f));
    D2D1_POINT_2F B2 = lerpP(A2, A3, (t - t1) / fmaxf(t3 - t1, 1e-6f));
    return lerpP(B1, B2, (t - t1) / fmaxf(t2 - t1, 1e-6f));
}
static void CatmullRomSmooth(const std::vector<D2D1_POINT_2F> &input, std::vector<D2D1_POINT_2F> &output, int segments = 4) {
    if (input.size() < 3) {
        output = input;
        return;
    }
    const float kAlpha = 0.5f;
    const size_t n = input.size();
    output.clear();
    output.push_back(input.front());
    for (size_t i = 0; i < n - 1; i++) {
        D2D1_POINT_2F p1 = input[i], p2 = input[i + 1];
        D2D1_POINT_2F p0 = (i > 0) ? input[i - 1]
                                   : D2D1::Point2F(2.0f * p1.x - p2.x, 2.0f * p1.y - p2.y);
        D2D1_POINT_2F p3 = (i + 2 < n) ? input[i + 2]
                                       : D2D1::Point2F(2.0f * p2.x - p1.x, 2.0f * p2.y - p1.y);
        float t0 = 0.0f;
        float t1 = t0 + CrKnotSpan(p0, p1, kAlpha);
        float t2 = t1 + CrKnotSpan(p1, p2, kAlpha);
        float t3 = t2 + CrKnotSpan(p2, p3, kAlpha);
        if (t2 - t1 < 1e-5f) {
            output.push_back(p2);
            continue;
        }
        for (int s = 1; s <= segments; s++) {
            float u = (float)s / (float)segments;
            float t = t1 + (t2 - t1) * u;
            output.push_back(CrEval(p0, p1, p2, p3, t0, t1, t2, t3, t));
        }
    }
}
static D2D1_POINT_2F GetPointOnPath(const std::vector<D2D1_POINT_2F> &path, float ratio) {
    if (path.empty())
        return D2D1::Point2F(0, 0);
    if (path.size() == 1)
        return path[0];
    if (ratio <= 0)
        return path[0];
    if (ratio >= 1)
        return path.back();
    float totalLen = 0;
    for (size_t i = 1; i < path.size(); i++) {
        float dx = path[i].x - path[i - 1].x, dy = path[i].y - path[i - 1].y;
        totalLen += sqrtf(dx * dx + dy * dy);
    }
    float targetDist = ratio * totalLen, acc = 0;
    for (size_t i = 1; i < path.size(); i++) {
        float dx = path[i].x - path[i - 1].x, dy = path[i].y - path[i - 1].y;
        float segLen = sqrtf(dx * dx + dy * dy);
        if (acc + segLen >= targetDist) {
            float t = segLen > 0 ? (targetDist - acc) / segLen : 0;
            return D2D1::Point2F(path[i - 1].x + dx * t, path[i - 1].y + dy * t);
        }
        acc += segLen;
    }
    return path.back();
}
static void WStrToUTF8(PCWSTR wstr, char *out, int outSize) {
    if (!wstr || outSize <= 0) {
        if (outSize > 0)
            out[0] = 0;
        return;
    }
    out[0] = 0;
    int written = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, out, outSize, NULL, NULL);
    if (written <= 0)
        out[0] = 0;
}
static int ResolveHotkeyVK(PCWSTR raw) {
    if (!raw || !raw[0]) return 0;
    wchar_t s[32];
    int n = 0;
    for (; raw[n] && n < 31; n++) {
        wchar_t c = raw[n];
        if (c >= L'A' && c <= L'Z') c = (wchar_t)(c - L'A' + L'a');
        s[n] = c;
    }
    s[n] = 0;
    if (s[0] == L'0' && s[1] == L'x') {
        int v = (int)wcstol(s + 2, nullptr, 16);
        return (v > 0 && v < 256) ? v : 0;
    }
    if (s[0] == L'v' && s[1] == L'k' && s[2] == L':') {
        int v = (int)wcstol(s + 3, nullptr, 10);
        return (v > 0 && v < 256) ? v : 0;
    }
    if (s[0] && !s[1]) {
        if (s[0] >= L'a' && s[0] <= L'z') return 0x41 + (s[0] - L'a');
        if (s[0] >= L'0' && s[0] <= L'9') return 0x30 + (s[0] - L'0');
    }
    if (s[0] == L'f' && s[1]) {
        int v = (int)wcstol(s + 1, nullptr, 10);
        if (v >= 1 && v <= 24) return VK_F1 + (v - 1);
    }
    static const struct { const wchar_t *name; int vk; } kHotkeyNames[] = {
        { L"space", VK_SPACE }, { L"enter", VK_RETURN }, { L"return", VK_RETURN },
        { L"tab", VK_TAB }, { L"esc", VK_ESCAPE }, { L"backspace", VK_BACK },
        { L"up", VK_UP }, { L"down", VK_DOWN }, { L"left", VK_LEFT }, { L"right", VK_RIGHT },
        { L"home", VK_HOME }, { L"end", VK_END },
        { L"pgup", VK_PRIOR }, { L"pageup", VK_PRIOR }, { L"pgdn", VK_NEXT }, { L"pagedown", VK_NEXT },
        { L"ins", VK_INSERT }, { L"insert", VK_INSERT }, { L"del", VK_DELETE }, { L"delete", VK_DELETE },
        { L"media_play", VK_MEDIA_PLAY_PAUSE }, { L"media_next", VK_MEDIA_NEXT_TRACK },
        { L"media_prev", VK_MEDIA_PREV_TRACK }, { L"media_stop", VK_MEDIA_STOP },
        { L"vol_up", VK_VOLUME_UP }, { L"vol_down", VK_VOLUME_DOWN }, { L"vol_mute", VK_VOLUME_MUTE },
        { L"browser_back", VK_BROWSER_BACK }, { L"browser_forward", VK_BROWSER_FORWARD },
        { L"browser_home", VK_BROWSER_HOME }, { L"browser_refresh", VK_BROWSER_REFRESH },
        { L"browser_search", VK_BROWSER_SEARCH }, { L"browser_favorites", VK_BROWSER_FAVORITES },
        { L"sleep", VK_SLEEP }, { L"printscreen", VK_SNAPSHOT }, { L"pause", VK_PAUSE },
        { L"scrolllock", VK_SCROLL }, { L"numlock", VK_NUMLOCK }, { L"capslock", VK_CAPITAL },
        { L"apps", VK_APPS },
        { L"oem_comma", VK_OEM_COMMA }, { L"oem_period", VK_OEM_PERIOD },
        { L"oem_minus", VK_OEM_MINUS }, { L"oem_plus", VK_OEM_PLUS },
        { L"oem_1", VK_OEM_1 }, { L"oem_2", VK_OEM_2 }, { L"oem_3", VK_OEM_3 }, { L"oem_4", VK_OEM_4 },
        { L"oem_5", VK_OEM_5 }, { L"oem_6", VK_OEM_6 }, { L"oem_7", VK_OEM_7 }, { L"oem_8", VK_OEM_8 },
    };
    for (size_t i = 0; i < sizeof(kHotkeyNames) / sizeof(kHotkeyNames[0]); i++) {
        if (wcscmp(s, kHotkeyNames[i].name) == 0) return kHotkeyNames[i].vk;
    }
    return 0;
}
static const int kHotkeyIdToggle = 0xA701;
static const int kHotkeyIdTrail = 0xA702;
static const int kHotkeyIdParticles = 0xA703;
static std::atomic<bool> g_hotkeyOSOwned[3] = { {false}, {false}, {false} };
static std::atomic<DWORD> g_hotkeyLastAction[3] = { {0}, {0}, {0} };
static bool HotkeyTryAct(int idx, const wchar_t* via) {
    (void)via;
    DWORD nowT = GetTickCount();
    DWORD prev = g_hotkeyLastAction[idx].load(std::memory_order_relaxed);
    if (nowT - prev < 250) return false;
    if (!g_hotkeyLastAction[idx].compare_exchange_strong(prev, nowT, std::memory_order_relaxed)) return false;
    if (idx == 0) {
        bool cur = g_effectsVisible.load(std::memory_order_relaxed);
        g_effectsVisible.exchange(!cur, std::memory_order_relaxed);
    } else if (idx == 1) {
        bool cur = g_trailVisible.load(std::memory_order_relaxed);
        g_trailVisible.exchange(!cur, std::memory_order_relaxed);
    } else {
        bool cur = g_particlesVisible.load(std::memory_order_relaxed);
        g_particlesVisible.exchange(!cur, std::memory_order_relaxed);
    }
    return true;
}
static UINT HotkeyModFlags() {
    switch (g_hotkeyModifier) {
        case 1: return MOD_ALT;
        case 2: return MOD_ALT | MOD_CONTROL;
        case 3: return MOD_ALT | MOD_SHIFT;
        case 4: return MOD_ALT | MOD_CONTROL | MOD_SHIFT;
        default: return 0;
    }
}
static bool HotkeyVKIsRegisterable(int vk) {
    if (vk == 0) return false;
    if (vk >= 0xA6 && vk <= 0xB3) return false;
    if (vk == VK_SLEEP || vk == VK_LAUNCH_MAIL || vk == VK_LAUNCH_APP1 || vk == VK_LAUNCH_APP2 ||
        vk == VK_LAUNCH_MEDIA_SELECT)
        return false;
    return true;
}
static void HotkeyRegisterAll() {
    for (int i = 0; i < 3; i++) g_hotkeyOSOwned[i].store(false);
    if (!g_overlayHwnd) return;
    const int ids[3] = { kHotkeyIdToggle, kHotkeyIdTrail, kHotkeyIdParticles };
    for (int i = 0; i < 3; i++) {
        UnregisterHotKey(g_overlayHwnd, ids[i]);
    }
    UINT mods = HotkeyModFlags();
    int vks[3] = { g_hotkeyToggleVK, g_hotkeyTrailVK, g_hotkeyParticlesVK };
    for (int i = 0; i < 3; i++) {
        if (vks[i] == 0) continue;
        if (mods == 0) {
            continue;
        }
        if (!HotkeyVKIsRegisterable(vks[i])) {
            continue;
        }
        SetLastError(0);
        if (RegisterHotKey(g_overlayHwnd, ids[i], mods, (UINT)vks[i])) {
            g_hotkeyOSOwned[i].store(true);
        }
    }
}
static bool HotkeyHandleId(int id) {
    if (id == kHotkeyIdToggle) return HotkeyTryAct(0, L"WM_HOTKEY");
    if (id == kHotkeyIdTrail) return HotkeyTryAct(1, L"WM_HOTKEY");
    if (id == kHotkeyIdParticles) return HotkeyTryAct(2, L"WM_HOTKEY");
    return false;
}
static void HotkeyRequestReregister() {
    if (!g_overlayHwnd) return;
    if (GetWindowThreadProcessId(g_overlayHwnd, nullptr) == GetCurrentThreadId()) {
        HotkeyRegisterAll();
        return;
    }
    if (!PostMessageW(g_overlayHwnd, WM_APP_REHOTKEY, 0, 0))
        return;
}
static void HotkeyOnSettingsLoaded() {
    g_hotkeyToggleVK = ResolveHotkeyVK(g_hotkeyToggleKey);
    g_hotkeyTrailVK = ResolveHotkeyVK(g_hotkeyTrailKey);
    g_hotkeyParticlesVK = ResolveHotkeyVK(g_hotkeyParticlesKey);
    HotkeyRequestReregister();
}
struct StrEnumEntry {
    PCWSTR name;
    int value;
};
template <size_t N>
static int ReadEnumInt(PCWSTR setting, const StrEnumEntry (&map)[N], int fallback) {
    PCWSTR s = Wh_GetStringSetting(setting);
    if (!s) return fallback;
    int value = fallback;
    for (size_t i = 0; i < N; i++) {
        if (wcscmp(s, map[i].name) == 0) { value = map[i].value; break; }
    }
    Wh_FreeStringSetting(s);
    return value;
}
static const StrEnumEntry kFadeoutModeMap[] = {
    {L"hard", 0}, {L"accelerate", 1},
};
static const StrEnumEntry kMissileFxMap[] = {
    {L"particles", 0}, {L"shockwave", 1},
};
static const StrEnumEntry kAaModeMap[] = {
    {L"off", 0}, {L"crisp", 2}, {L"extra", 3},
};
static const StrEnumEntry kMsaaLevelMap[] = {
    {L"2x", 2}, {L"4x", 4}, {L"8x", 8},
};
static const StrEnumEntry kSsaaLevelMap[] = {
    {L"4x", 4}, {L"3x", 3},
};
static const StrEnumEntry kShaderQualityMap[] = {
    {L"fast", 0}, {L"high", 2},
};
static const StrEnumEntry kSsaaFilterMap[] = {
    {L"bilinear", 0}, {L"tent", 2}, {L"lanczos", 3},
};
static const StrEnumEntry kSdfQualityMap[] = {
    {L"4x", 4}, {L"2x2", 2},
};
static const StrEnumEntry kDitherModeMap[] = {
    {L"off", 0}, {L"ordered8", 2}, {L"hash", 3},
};
static const StrEnumEntry kVortexModelMap[] = {
    {L"rankine", 0}, {L"free", 1}, {L"solid", 2}, {L"lamb", 3}, {L"kepler", 4},
};
static const StrEnumEntry kFftSizeMap[] = {
    {L"fft256", 256}, {L"fft512", 512}, {L"fft1024", 1024}, {L"fft2048", 2048},
};
static const StrEnumEntry kParticleModeMap[] = {
    {L"off", 0}, {L"always", 2},
};
static const StrEnumEntry kParticleShapeMap[] = {
    {L"circle", 0}, {L"star", 1}, {L"hexagram", 2}, {L"heart", 3}, {L"diamond", 4}, {L"triangle", 5},
    {L"flower", 6}, {L"pentagon", 7}, {L"hexagon", 8}, {L"text", 10},
};
static const StrEnumEntry kClickStyleMap[] = {
    {L"shockwave", 1}, {L"ink", 2}, {L"spark", 3}, {L"bloom", 4},
};
static const StrEnumEntry kTrailShapeMap[] = {
    {L"dots", 1}, {L"function", 2}, {L"wave", 3}, {L"shapes", 4}, {L"double", 5}, {L"dashed", 6},
    {L"spiral", 7}, {L"lightning", 8}, {L"feather", 9}, {L"none", 10},
};
static const StrEnumEntry kShapeTypeMap[] = {
    {L"star", 1}, {L"hexagon", 2}, {L"circle", 3}, {L"diamond", 4}, {L"triangle", 5}, {L"flower", 6},
    {L"pentagon", 7}, {L"hexagram", 8}, {L"random", 9}, {L"rocket", 11}, {L"missile", 12},
    {L"fiber_missile", 13},
};
static const StrEnumEntry kPostAaMap[] = {
    {L"fxaa", 1}, {L"fxaa_extreme", 2},
};
static const StrEnumEntry kFxWaveMap[] = {
    {L"pulse", 1}, {L"triangle", 2}, {L"flicker", 3},
};
static const StrEnumEntry kHdrModeMap[] = {
    {L"off", 1}, {L"false", 1}, {L"scrgb", 2}, {L"hdr10", 3},
};
static const StrEnumEntry kHdrTonemapMap[] = {
    {L"reinhard", 1}, {L"aces", 2},
};
static const StrEnumEntry kHotkeyModifierMap[] = {
    {L"alt", 1}, {L"alt_ctrl", 2}, {L"alt_shift", 3}, {L"alt_ctrl_shift", 4},
};
static const StrEnumEntry kFunctionPresetMap[] = {
    {L"damped", 1}, {L"beat", 2}, {L"swirl", 3}, {L"custom", 4},
};
static const StrEnumEntry kColorModeMap[] = {
    {L"single", 0}, {L"gradient", 1}, {L"rainbow", 2}, {L"warm", 3}, {L"cool", 4}, {L"neon", 5},
    {L"velocity", 6}, {L"stripes", 7}, {L"fire", 8}, {L"aurora", 9}, {L"cursor_extract", 10},
    {L"cursor_mix", 11}, {L"metallic", 12}, {L"cyberpunk", 13}, {L"pastel", 14}, {L"oil_slick", 23},
    {L"vaporwave", 24}, {L"bioluminescence", 25}, {L"clock_sky", 26}, {L"hue_rotate", 15},
    {L"dual_pulse", 16}, {L"sparkle", 17}, {L"thermal", 18}, {L"interference", 19}, {L"spectrum", 20},
    {L"dither", 21}, {L"warp", 22},
};
static const StrEnumEntry kParticleOriginMap[] = {
    {L"head", 0}, {L"middle", 1}, {L"custom", 3}, {L"random", 4},
};
static const StrEnumEntry kColorShiftModeMap[] = {
    {L"off", 0}, {L"complementary", 1}, {L"analogous", 2}, {L"triadic", 3}, {L"split", 4}, {L"custom", 5},
};
static const StrEnumEntry kContrastTargetMap[] = {
    {L"aa_large", 1}, {L"aa", 2}, {L"aaa", 3},
};
static const StrEnumEntry kTextAtlasSSMap[] = {
    {L"1x", 1}, {L"3x", 3}, {L"4x", 4},
};
static const StrEnumEntry kTextChunkModeMap[] = {
    {L"global", 1},
};
static const StrEnumEntry kTextTagModeMap[] = {
    {L"static", 0},
};
static const StrEnumEntry kGravitySystemMap[] = {
    {L"nbody", 1},
};
static const StrEnumEntry kDpiScaleModeMap[] = {
    {L"fixed", 1},
};
static const StrEnumEntry kDesktopLayerMap[] = {
    {L"hide", 0}, {L"cover", 1}, {L"occlude", 2},
};
void LoadSettings() {
    g_triggerVelocity = (float)Wh_GetIntSetting(L"General.trigger_velocity");
    g_stopVelocity = (float)Wh_GetIntSetting(L"General.stop_velocity");
    g_desktopOnly = Wh_GetIntSetting(L"General.desktop_only") != 0;
    g_desktopLayer = ReadEnumInt(L"General.desktop_only_layer", kDesktopLayerMap, 0);
    g_foregroundHook = Wh_GetIntSetting(L"General.foreground_hook") != 0;
    g_tailOffsetX = Wh_GetIntSetting(L"General.tail_offset_x");
    g_tailOffsetY = Wh_GetIntSetting(L"General.tail_offset_y");
    g_tailLength = Wh_GetIntSetting(L"General.tail_length");
    g_trailDelay = Wh_GetIntSetting(L"General.trail_delay");
    g_fadeoutMode = ReadEnumInt(L"Trail.fadeout_mode", kFadeoutModeMap, 2);
    g_enableSpeedResponse = Wh_GetIntSetting(L"Trail.enable_speed_response") != 0;
    g_enhancedGlow = Wh_GetIntSetting(L"Quality.enhanced_glow") != 0;
    g_enableHeadHighlight = Wh_GetIntSetting(L"Trail.enable_head_highlight") != 0;
    g_adaptiveContrast = Wh_GetIntSetting(L"Trail.enable_adaptive_contrast") != 0;
    g_dotsMultiplier = Wh_GetIntSetting(L"ShapesText.dots_multiplier");
    g_waveAmplitude = Wh_GetIntSetting(L"ShapesText.wave_amplitude");
    g_waveFrequency = Wh_GetIntSetting(L"ShapesText.wave_frequency");
    g_missileIdleExplode = Wh_GetIntSetting(L"ShapesText.missile_idle_explode") != 0;
    g_missileIdleDelay = ClampInt(Wh_GetIntSetting(L"ShapesText.missile_idle_delay"), 100, 5000);
    g_missileExplodeParticles = ClampInt(Wh_GetIntSetting(L"ShapesText.missile_explode_particles"), 0, 40);
    g_missileExplodeFx = ReadEnumInt(L"ShapesText.missile_explode_fx", kMissileFxMap, 2);
    g_enableGlow = Wh_GetIntSetting(L"Quality.enable_glow") != 0;
    g_glowIntensity = Wh_GetIntSetting(L"Quality.glow_intensity");
    g_edgeSoftness = Wh_GetIntSetting(L"Quality.edge_softness");
    g_aaMode = ReadEnumInt(L"Quality.aa_mode", kAaModeMap, 1);
    bool enableMsaa = Wh_GetIntSetting(L"Quality.enable_msaa") != 0;
    if (enableMsaa) {
        g_msaaSamples = ReadEnumInt(L"Quality.msaa_level", kMsaaLevelMap, 4);
    } else {
        g_msaaSamples = 1;
    }
    bool enableSsaa = Wh_GetIntSetting(L"Quality.enable_ssaa") != 0;
    if (enableSsaa) {
        g_ssaaScale = ReadEnumInt(L"Quality.ssaa_level", kSsaaLevelMap, 2);
    } else {
        g_ssaaScale = 1;
    }
    g_shaderQuality = ReadEnumInt(L"Quality.shader_quality", kShaderQualityMap, 1);
    g_ssaaFilter = ReadEnumInt(L"Quality.ssaa_filter", kSsaaFilterMap, 1);
    {
        int shVal = Wh_GetIntSetting(L"Quality.ssaa_sharpness");
        g_ssaaSharpness = ClampInt(shVal, 0, 100);
    }
    g_sdfSamples = ReadEnumInt(L"Quality.sdf_sample_quality", kSdfQualityMap, 1);
    g_ditherMode = ReadEnumInt(L"Quality.dither_mode", kDitherModeMap, 1);
    {
        int dsVal = Wh_GetIntSetting(L"Quality.dither_strength");
        g_ditherStrength = ClampInt(dsVal, 0, 100);
    }
    g_enableSmoothGradient = Wh_GetIntSetting(L"Trail.enable_smooth_gradient") != 0;
    g_enableFrameSync = Wh_GetIntSetting(L"Trail.enable_frame_sync") != 0;
    g_enableTrailShadow = Wh_GetIntSetting(L"Trail.enable_trail_shadow") != 0;
    g_particleDensity = Wh_GetIntSetting(L"Particles.particle_density");
    g_particleInterval = Wh_GetIntSetting(L"Particles.particle_interval");
    g_particleAccel = Wh_GetIntSetting(L"Particles.particle_acceleration") != 0;
    g_particleOriginRatio = Wh_GetIntSetting(L"Particles.particle_origin_ratio");
    int attrVal = ClampInt(Wh_GetIntSetting(L"Particles.particle_attraction"), 0, 100);
    g_particleAttraction = attrVal / 800.0f;
    g_enableParticleRepel = Wh_GetIntSetting(L"Particles.enable_particle_repel") != 0;
    g_particleRepelRadius = Wh_GetIntSetting(L"Particles.particle_repel_radius");
    if (g_particleRepelRadius < 5)
        g_particleRepelRadius = 5;
    if (g_particleRepelRadius > 100)
        g_particleRepelRadius = 100;
    int repelVal = ClampInt(Wh_GetIntSetting(L"Particles.particle_repel_force"), 0, 100);
    g_particleRepelForce = (repelVal / 100.0f) * 3.0f;
    g_enableCentripetal = Wh_GetIntSetting(L"Physics.enable_centripetal") != 0;
    int cfVal = Wh_GetIntSetting(L"Physics.centripetal_force");
    cfVal = ClampInt(cfVal, 0, 100);
    g_centripetalForce = cfVal / 100.0f;
    int csVal = Wh_GetIntSetting(L"Physics.centripetal_sensitivity");
    csVal = ClampInt(csVal, 0, 100);
    g_centripetalSensitivity = csVal / 100.0f;
    int cdVal = ClampInt(Wh_GetIntSetting(L"Physics.centripetal_duration"), 500, 5000);
    g_centripetalDuration = cdVal;
    int vmcVal = Wh_GetIntSetting(L"Physics.vortex_max_count");
    vmcVal = ClampInt(vmcVal, 1, MAX_VORTICES);
    g_vortexMaxCount = vmcVal;
    int vmdVal = Wh_GetIntSetting(L"Physics.vortex_min_distance");
    vmdVal = ClampInt(vmdVal, 50, 500);
    g_vortexMinDistance = (float)vmdVal;
    int vdVal = Wh_GetIntSetting(L"Physics.vortex_drift");
    vdVal = ClampInt(vdVal, 0, 100);
    g_vortexDrift = vdVal / 100.0f;
    int vdsVal = Wh_GetIntSetting(L"Physics.vortex_duration_speed");
    vdsVal = ClampInt(vdsVal, 0, 100);
    g_vortexDurationSpeed = vdsVal / 100.0f;
    g_vortexPhysModel = 0;
    g_vortexPhysModel = ReadEnumInt(L"Physics.vortex_phys_model", kVortexModelMap, 0);
    g_enableParticleMass = Wh_GetIntSetting(L"Physics.enable_particle_mass") != 0;
    g_particleMassMin = (float)Wh_GetIntSetting(L"Physics.particle_mass_min") / 10.0f;
    if (g_particleMassMin < 0.1f) g_particleMassMin = 0.1f;
    g_particleMassMax = (float)Wh_GetIntSetting(L"Physics.particle_mass_max") / 10.0f;
    if (g_particleMassMax < 0.1f) g_particleMassMax = 0.1f;
    if (g_particleMassMax < g_particleMassMin) g_particleMassMax = g_particleMassMin;
    g_enableParticleGravity = Wh_GetIntSetting(L"Physics.enable_particle_gravity") != 0;
    int gsVal = Wh_GetIntSetting(L"Physics.gravity_strength");
    gsVal = ClampInt(gsVal, 0, 100);
    g_gravityStrength = gsVal / 100.0f;
    g_gravitySystem = ReadEnumInt(L"Physics.gravity_system", kGravitySystemMap, 0);
    g_gravityBodyCount = ClampInt(Wh_GetIntSetting(L"Physics.gravity_body_count"), 2, 10);
    g_enableMusicReactive = Wh_GetIntSetting(L"Music.enable_music_reactive") != 0;
    g_musicFftSize = ReadEnumInt(L"Music.music_fft_size", kFftSizeMap, 512);
    int msVal = Wh_GetIntSetting(L"Music.music_sensitivity");
    msVal = ClampInt(msVal, 0, 100);
    g_musicSensitivity = msVal / 100.0f;
    int msmVal = Wh_GetIntSetting(L"Music.music_smoothing");
    msmVal = ClampInt(msmVal, 0, 100);
    g_musicSmoothing = msmVal / 100.0f;
    g_enableMusicPhysics = Wh_GetIntSetting(L"Music.enable_music_physics") != 0;
    int mglVal = Wh_GetIntSetting(L"Music.music_gravity_link");
    mglVal = ClampInt(mglVal, 0, 100);
    g_musicGravityLink = mglVal / 100.0f;
    int mbpVal = Wh_GetIntSetting(L"Music.music_beat_pulse");
    mbpVal = ClampInt(mbpVal, 0, 100);
    g_musicBeatPulse = mbpVal / 100.0f;
    int mbsVal = Wh_GetIntSetting(L"Music.music_bass_size");
    mbsVal = ClampInt(mbsVal, 0, 100);
    g_musicBassSize = mbsVal / 100.0f;
    int mvlVal = Wh_GetIntSetting(L"Music.music_vortex_link");
    mvlVal = ClampInt(mvlVal, 0, 100);
    g_musicVortexLink = mvlVal / 100.0f;
    g_enableParticleCollision = Wh_GetIntSetting(L"Physics.enable_particle_collision") != 0;
    g_enableLorentzForce = Wh_GetIntSetting(L"Physics.enable_lorentz_force") != 0;
    int lsVal = Wh_GetIntSetting(L"Physics.lorentz_strength");
    lsVal = ClampInt(lsVal, 0, 100);
    g_lorentzStrength = lsVal / 100.0f;
    g_enableBrownianMotion = Wh_GetIntSetting(L"Physics.enable_brownian_motion") != 0;
    int bsVal = Wh_GetIntSetting(L"Physics.brownian_strength");
    bsVal = ClampInt(bsVal, 0, 100);
    g_brownianStrength = bsVal / 100.0f;
    g_enableMultiBandLink = Wh_GetIntSetting(L"Music.enable_multi_band_link") != 0;
    g_enableEnvGravity = Wh_GetIntSetting(L"Physics.enable_env_gravity") != 0;
    int egsVal = ClampInt(Wh_GetIntSetting(L"Physics.env_gravity_strength"), 0, 100);
    g_envGravityStrength = egsVal / 100.0f * 0.8f;
    int egaVal = ClampInt(Wh_GetIntSetting(L"Physics.env_gravity_angle"), -180, 180);
    g_envGravityAngle = egaVal * 3.14159f / 180.0f;
    g_enableEnvWind = Wh_GetIntSetting(L"Physics.enable_env_wind") != 0;
    int wsVal = ClampInt(Wh_GetIntSetting(L"Physics.wind_strength"), 0, 100);
    g_windStrength = wsVal / 100.0f * 0.6f;
    int wtVal = ClampInt(Wh_GetIntSetting(L"Physics.wind_turbulence"), 0, 100);
    g_windTurbulence = wtVal / 100.0f;
    g_enableParticleSpring = Wh_GetIntSetting(L"Physics.enable_particle_spring") != 0;
    int skVal = ClampInt(Wh_GetIntSetting(L"Physics.spring_strength"), 0, 100);
    g_springK = skVal / 100.0f * 0.05f;
    int sdVal = ClampInt(Wh_GetIntSetting(L"Physics.spring_distance"), 10, 150);
    g_springMaxDist = (float)sdVal;
    PCWSTR pText = Wh_GetStringSetting(L"ShapesText.text_content");
    wchar_t oldText[256];
    wcscpy_s(oldText, g_textContent);
    if (pText) { wcsncpy_s(g_textContent, pText, _TRUNCATE); Wh_FreeStringSetting(pText); }
    const bool tagTextChanged = wcscmp(oldText, g_textContent) != 0;
    int tfsVal = ClampInt(Wh_GetIntSetting(L"ShapesText.text_font_size"), 10, 200);
    g_textFontSize = tfsVal;
    g_textAtlasSS = (float)ReadEnumInt(L"ShapesText.text_atlas_ss", kTextAtlasSSMap, 2);
    g_enableTextChunk = Wh_GetIntSetting(L"ShapesText.enable_text_chunk") != 0;
    g_textChunkMode = ReadEnumInt(L"ShapesText.text_chunk_mode", kTextChunkModeMap, 0);
    int tcmMax = ClampInt(Wh_GetIntSetting(L"ShapesText.text_chunk_max"), 1, 8);
    g_textChunkMax = tcmMax;
    int tcdVal = ClampInt(Wh_GetIntSetting(L"ShapesText.text_chunk_delay"), 100, 3000);
    g_textChunkDelay = tcdVal;
    int toxVal = ClampInt(Wh_GetIntSetting(L"ShapesText.text_offset_x"), -100, 100);
    g_textOffsetX = toxVal;
    int toyVal = ClampInt(Wh_GetIntSetting(L"ShapesText.text_offset_y"), -100, 100);
    g_textOffsetY = toyVal;
    {
        int newMode = ReadEnumInt(L"ShapesText.text_tag_mode", kTextTagModeMap, 1);
        wchar_t newTimeFmt[64] = L"", newDayFmt[64] = L"";
        PCWSTR pTimeFmt = Wh_GetStringSetting(L"ShapesText.text_tag_time_format");
        if (pTimeFmt) { wcsncpy_s(newTimeFmt, pTimeFmt, _TRUNCATE); Wh_FreeStringSetting(pTimeFmt); }
        PCWSTR pDayFmt = Wh_GetStringSetting(L"ShapesText.text_tag_day_format");
        if (pDayFmt) { wcsncpy_s(newDayFmt, pDayFmt, _TRUNCATE); Wh_FreeStringSetting(pDayFmt); }
        bool tagCfgChanged = (newMode != g_textTagMode) ||
                             (wcscmp(newTimeFmt, g_textTagTimeFmt) != 0) ||
                             (wcscmp(newDayFmt, g_textTagDayFmt) != 0);
        g_textTagMode = newMode;
        wcsncpy_s(g_textTagTimeFmt, newTimeFmt, _TRUNCATE);
        wcsncpy_s(g_textTagDayFmt, newDayFmt, _TRUNCATE);
        if (tagCfgChanged || tagTextChanged) g_tagCacheDirty.store(true);
    }
    g_charAtlasDirty = true;
    g_debugVelocity = Wh_GetIntSetting(L"Debug.enable_debug_velocity") != 0;
    g_debugForce = Wh_GetIntSetting(L"Debug.enable_debug_force") != 0;
    g_debugVortex = Wh_GetIntSetting(L"Debug.enable_debug_vortex") != 0;
    g_debugGravity = Wh_GetIntSetting(L"Debug.enable_debug_gravity") != 0;
    g_debugCollision = Wh_GetIntSetting(L"Debug.enable_debug_collision") != 0;
    g_debugSpring = Wh_GetIntSetting(L"Debug.enable_debug_spring") != 0;
    g_debugInteraction = Wh_GetIntSetting(L"Debug.enable_debug_interaction") != 0;
    g_debugSpawn = Wh_GetIntSetting(L"Debug.enable_debug_spawn") != 0;
    g_debugPath = Wh_GetIntSetting(L"Debug.enable_debug_path") != 0;
    bool musicWanted = g_enableMusicReactive || g_enableMusicPhysics;
    if (musicWanted && !g_music.capturing) {
        MusicStartCapture();
    } else if (!musicWanted && g_music.capturing) {
        MusicStopCapture();
    }
    g_particleMode = ReadEnumInt(L"Particles.particle_mode", kParticleModeMap, 1);
    g_particleShape = ReadEnumInt(L"Particles.particle_shape", kParticleShapeMap, 9);
    g_enableParticleSpin = Wh_GetIntSetting(L"Particles.enable_particle_spin") != 0;
    g_particleSpinSpeed = ClampInt(Wh_GetIntSetting(L"Particles.particle_spin_speed"), 0, 100);
    g_enableParticleInteraction = Wh_GetIntSetting(L"Particles.enable_particle_interaction") != 0;
    g_interParticleRepelDistance = ClampInt(Wh_GetIntSetting(L"Particles.particle_repel_distance"), 5, 100);
    g_particleSpawnSpread = ClampInt(Wh_GetIntSetting(L"Particles.particle_spawn_spread"), 0, 100);
    int interRepelVal = ClampInt(Wh_GetIntSetting(L"Particles.particle_inter_repel_force"), 1, 100);
    g_interParticleRepelForce = interRepelVal / 12.0f;
    g_particleRepelAlignment = ClampInt(Wh_GetIntSetting(L"Particles.particle_repel_alignment"), 0, 100);
    g_enableClickStarburst = Wh_GetIntSetting(L"Click.enable_click_starburst") != 0;
    g_starburstCount = Wh_GetIntSetting(L"Click.starburst_count");
    g_enableClickEffect = Wh_GetIntSetting(L"Click.enable_click_effect") != 0;
    g_clickMaxRadius = Wh_GetIntSetting(L"Click.click_max_radius");
    g_clickDuration = Wh_GetIntSetting(L"Click.click_duration");
    g_clickStyle = ReadEnumInt(L"Click.click_style", kClickStyleMap, 0);
    PCWSTR str = nullptr;
    g_trailShape = ReadEnumInt(L"Trail.trail_shape", kTrailShapeMap, 0);
    g_shapeType = ReadEnumInt(L"ShapesText.shape_type", kShapeTypeMap, 0);
    g_shapeInterval = ClampInt(Wh_GetIntSetting(L"ShapesText.shape_interval"), 10, 100);
    g_shapeRandomOffset = Wh_GetIntSetting(L"ShapesText.shape_random_offset") != 0;
    g_shapeCount = ClampInt(Wh_GetIntSetting(L"ShapesText.shape_count"), 1, 5);
    g_shapeSize = (float)Wh_GetIntSetting(L"ShapesText.shape_size");
    g_shapeSize = ClampF(g_shapeSize, 5.0f, 60.0f);
    g_shapeLifetime = ClampInt(Wh_GetIntSetting(L"ShapesText.shape_lifetime"), 200, 2000);
    g_superPerformanceMode = Wh_GetIntSetting(L"System.super_performance_mode") != 0;
    g_enableTemporalBlur = Wh_GetIntSetting(L"Quality.enable_temporal_blur") != 0;
    g_temporalBlurAmount = ClampInt(Wh_GetIntSetting(L"Quality.temporal_blur_amount"), 0, 95);
    g_postAA = ReadEnumInt(L"Quality.post_aa", kPostAaMap, 0);
    g_enableBloom = Wh_GetIntSetting(L"Quality.enable_bloom") != 0;
    g_bloomThreshold = ClampInt(Wh_GetIntSetting(L"Quality.bloom_threshold"), 0, 100);
    g_bloomIntensity = ClampInt(Wh_GetIntSetting(L"Quality.bloom_intensity"), 0, 100);
    g_fxBloomPulse = Wh_GetIntSetting(L"AnimFX.fx_bloom_pulse") != 0;
    g_fxGlowBreath = Wh_GetIntSetting(L"AnimFX.fx_glow_breath") != 0;
    g_fxEdgeFlow = Wh_GetIntSetting(L"AnimFX.fx_edge_flow") != 0;
    g_fxDitherShimmer = Wh_GetIntSetting(L"AnimFX.fx_dither_shimmer") != 0;
    g_enableCollisionSpark = Wh_GetIntSetting(L"Particles.enable_collision_spark") != 0;
    g_sparkChance = ClampInt(Wh_GetIntSetting(L"Particles.spark_chance"), 0, 100);
    g_sparkIntensity = ClampInt(Wh_GetIntSetting(L"Particles.spark_intensity"), 0, 300);
    g_fxStrokeFlow = Wh_GetIntSetting(L"AnimFX.fx_stroke_flow") != 0;
    g_fxBloomThrWave = Wh_GetIntSetting(L"AnimFX.fx_bloom_thr_wave") != 0;
    g_fxBlurPulse = Wh_GetIntSetting(L"AnimFX.fx_blur_pulse") != 0;
    g_fxHdrShimmer = Wh_GetIntSetting(L"AnimFX.fx_hdr_shimmer") != 0;
    g_fxAnimWave = ReadEnumInt(L"AnimFX.fx_anim_wave", kFxWaveMap, 0);
    g_fxAnimSpeed = ClampInt(Wh_GetIntSetting(L"AnimFX.fx_anim_speed"), 10, 400);
    g_fxAnimAmount = ClampInt(Wh_GetIntSetting(L"AnimFX.fx_anim_amount"), 0, 100);
    g_hdrMode = ReadEnumInt(L"Quality.hdr_mode", kHdrModeMap, 0);
    g_hdrHighlightBoost = ClampInt(Wh_GetIntSetting(L"Quality.hdr_highlight_boost"), 0, 200);
    g_hdrTonemap = ReadEnumInt(L"Quality.hdr_tonemap", kHdrTonemapMap, 0);
    g_dpiScaleMode = ReadEnumInt(L"System.dpi_scale_mode", kDpiScaleModeMap, 0);
    g_dpiScaleText = Wh_GetIntSetting(L"System.dpi_scale_text") != 0;
    g_hotkeyModifier = ReadEnumInt(L"Hotkeys.hotkey_modifier", kHotkeyModifierMap, 0);
    {
        PCWSTR s = Wh_GetStringSetting(L"Hotkeys.hotkey_toggle_key");
        if (s) { wcsncpy_s(g_hotkeyToggleKey, s, _TRUNCATE); Wh_FreeStringSetting(s); }
        else g_hotkeyToggleKey[0] = 0;
    }
    {
        PCWSTR s = Wh_GetStringSetting(L"Hotkeys.hotkey_trail_key");
        if (s) { wcsncpy_s(g_hotkeyTrailKey, s, _TRUNCATE); Wh_FreeStringSetting(s); }
        else g_hotkeyTrailKey[0] = 0;
    }
    {
        PCWSTR s = Wh_GetStringSetting(L"Hotkeys.hotkey_particles_key");
        if (s) { wcsncpy_s(g_hotkeyParticlesKey, s, _TRUNCATE); Wh_FreeStringSetting(s); }
        else g_hotkeyParticlesKey[0] = 0;
    }
    g_enableBezierSmooth = Wh_GetIntSetting(L"Trail.enable_bezier_smooth") != 0;
    g_enableMotionBlur = Wh_GetIntSetting(L"Trail.enable_motion_blur") != 0;
    g_motionBlurStrength = ClampInt(Wh_GetIntSetting(L"Trail.motion_blur_strength"), 1, 5);
    g_enable25DEffect = Wh_GetIntSetting(L"Trail.enable_25d_effect") != 0;
    g_perspectiveStrength = ClampInt(Wh_GetIntSetting(L"Trail.perspective_strength"), 0, 50);
    g_functionPreset = ReadEnumInt(L"ShapesText.function_preset", kFunctionPresetMap, 0);
    str = Wh_GetStringSetting(L"ShapesText.custom_function");
    if (str) {
        WStrToUTF8(str, g_customFunction, 256);
        Wh_FreeStringSetting(str);
    }
    g_colorMode = ReadEnumInt(L"Colors.color_mode", kColorModeMap, 0);
    g_particleOrigin = ReadEnumInt(L"Particles.particle_origin", kParticleOriginMap, 2);
    g_cursorColorShift = Wh_GetIntSetting(L"Colors.enable_cursor_color_shift") != 0;
    g_colorShiftMode = ReadEnumInt(L"Colors.color_shift_mode", kColorShiftModeMap, 1);
    g_colorShiftAngle = ClampInt(Wh_GetIntSetting(L"Colors.color_shift_angle"), 0, 360);
    g_contrastTarget = ReadEnumInt(L"Colors.contrast_target", kContrastTargetMap, 0);
    str = Wh_GetStringSetting(L"Colors.custom_color");
    if (str) {
        g_customColor = ParseHexColor(str, D2D1::ColorF(0, .75f, 1));
        Wh_FreeStringSetting(str);
    }
    str = Wh_GetStringSetting(L"Colors.gradient_colors");
    if (str) {
        ParseGradientColors(str);
        Wh_FreeStringSetting(str);
    }
    if (g_triggerVelocity <= 0)
        g_triggerVelocity = 25;
    if (g_stopVelocity <= 0)
        g_stopVelocity = 10;
    if (g_stopVelocity >= g_triggerVelocity)
        g_stopVelocity = fmaxf(1.0f, g_triggerVelocity * 0.8f);
    if (g_tailLength < 2)
        g_tailLength = 10;
    if (g_tailLength > 200)
        g_tailLength = 200;
    if (g_trailDelay < 0)
        g_trailDelay = 0;
    if (g_trailDelay > 10)
        g_trailDelay = 10;
    if (g_dotsMultiplier < 1)
        g_dotsMultiplier = 1;
    if (g_dotsMultiplier > 5)
        g_dotsMultiplier = 5;
    g_dotChainSize = ClampInt(Wh_GetIntSetting(L"ShapesText.dot_chain_size"), 50, 300);
    g_particleSizeMultiplier = ClampInt(Wh_GetIntSetting(L"Particles.particle_size_multiplier"), 50, 300);
    g_enableParticleGlow = Wh_GetIntSetting(L"Particles.enable_particle_glow") != 0;
    g_particleGlowIntensity = ClampInt(Wh_GetIntSetting(L"Particles.particle_glow_intensity"), 0, 100);
    g_enableParticleStroke = Wh_GetIntSetting(L"Particles.enable_particle_stroke") != 0;
    g_particleStrokeWidth = ClampInt(Wh_GetIntSetting(L"Particles.particle_stroke_width"), 0, 10);
    g_particleStrokeAlpha = ClampInt(Wh_GetIntSetting(L"Particles.particle_stroke_alpha"), 0, 100);
    {
        PCWSTR psc = Wh_GetStringSetting(L"Particles.particle_stroke_color");
        if (psc) {
            g_particleStrokeColorMode = wcscmp(psc, L"auto") != 0;
            if (g_particleStrokeColorMode)
                g_particleStrokeColor = ParseHexColor(psc, D2D1::ColorF(0x808090));
            Wh_FreeStringSetting(psc);
        }
    }
    if (g_waveAmplitude < 1)
        g_waveAmplitude = 1;
    if (g_waveAmplitude > 40)
        g_waveAmplitude = 40;
    if (g_waveFrequency < 3)
        g_waveFrequency = 3;
    if (g_waveFrequency > 60)
        g_waveFrequency = 60;
    if (g_glowIntensity < 0)
        g_glowIntensity = 0;
    if (g_glowIntensity > 100)
        g_glowIntensity = 100;
    g_enableTrailStroke = Wh_GetIntSetting(L"Quality.enable_trail_stroke") != 0;
    g_trailStrokeWidth = ClampInt(Wh_GetIntSetting(L"Quality.trail_stroke_width"), 0, 50);
    g_edgeSoftness = ClampInt(g_edgeSoftness, 0, 100);
    if (g_particleDensity < 1)
        g_particleDensity = 1;
    if (g_particleDensity > 10)
        g_particleDensity = 10;
    if (g_particleInterval < 0)
        g_particleInterval = 0;
    if (g_particleInterval > 2000)
        g_particleInterval = 2000;
    if (g_particleOriginRatio < 0)
        g_particleOriginRatio = 0;
    if (g_particleOriginRatio > 100)
        g_particleOriginRatio = 100;
    if (g_starburstCount < 4)
        g_starburstCount = 4;
    if (g_starburstCount > 20)
        g_starburstCount = 20;
    if (g_clickMaxRadius <= 0)
        g_clickMaxRadius = 40;
    if (g_clickMaxRadius > 500)
        g_clickMaxRadius = 500;
    if (g_clickDuration <= 0)
        g_clickDuration = 300;
    if (g_clickDuration > 3000)
        g_clickDuration = 3000;
    const char *presetExprs[] = {
        "sin(d * 0.15) * 8",
        "sin(d * 0.25) * exp(0 - t * 2.5) * 10",
        "abs(sin(d * 0.22)) ^ 3 * 12",
        "sin(d * 0.1 + time * 2) * cos(d * 0.05) * 9",
    };
    if (g_functionPreset == 4)
        CompileExpression(g_customFunction);
    else
        CompileExpression(presetExprs[g_functionPreset]);
    s_gradValid = false;
    g_lagInited = false;
    g_fadeAlpha = 1.0f;
    if (g_overlayHwnd) {
        if (g_colorMode == 10 || g_colorMode == 11)
            SetWindowDisplayAffinity(g_overlayHwnd, WDA_EXCLUDEFROMCAPTURE);
        else
            SetWindowDisplayAffinity(g_overlayHwnd, WDA_NONE);
    }
    HotkeyOnSettingsLoaded();
}
static bool IsShellWindow(HWND w) {
    if (!w) return false;
    if (w == GetDesktopWindow() || w == GetShellWindow()) return true;
    wchar_t cls[64] = {};
    if (!GetClassNameW(w, cls, ARRAYSIZE(cls))) return false;
    return wcscmp(cls, L"Progman") == 0 || wcscmp(cls, L"WorkerW") == 0;
}
static bool IsDesktopForeground() {
    return IsShellWindow(GetForegroundWindow());
}
static HWINEVENTHOOK g_fgHook = nullptr;
static void CALLBACK ForegroundEventProc(HWINEVENTHOOK, DWORD event, HWND w, LONG, LONG, DWORD, DWORD) {
    if (event == EVENT_SYSTEM_FOREGROUND && g_foregroundHook) g_desktopActive.store(IsShellWindow(w));
}
static bool DesktopActiveNow(DWORD now) {
    static DWORD s_lastSync = 0;
    DWORD period = (g_foregroundHook && g_fgHookActive.load()) ? 1000 : 40;
    if (now - s_lastSync >= period) {
        s_lastSync = now;
        g_desktopActive.store(IsDesktopForeground());
    }
    return g_desktopActive.load();
}

bool CheckForegroundFullscreen() {
    HWND fg = GetForegroundWindow();
    if (IsShellWindow(fg))
        return false;
    QUERY_USER_NOTIFICATION_STATE quns;
    if (SUCCEEDED(SHQueryUserNotificationState(&quns)) && quns == QUNS_RUNNING_D3D_FULL_SCREEN)
        return true;
    RECT rcWnd = {};
    if (!GetWindowRect(fg, &rcWnd)) return false;
    HMONITOR hMon = MonitorFromWindow(fg, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi = {};
    mi.cbSize = sizeof(mi);
    if (!GetMonitorInfo(hMon, &mi))
        return false;
    bool sizeMatch = rcWnd.left <= mi.rcMonitor.left && rcWnd.top <= mi.rcMonitor.top &&
                     rcWnd.right >= mi.rcMonitor.right && rcWnd.bottom >= mi.rcMonitor.bottom;
    if (!sizeMatch)
        return false;
    RECT rcClip;
    if (GetClipCursor(&rcClip)) {
        int vW = GetSystemMetrics(SM_CXVIRTUALSCREEN);
        int vH = GetSystemMetrics(SM_CYVIRTUALSCREEN);
        if ((rcClip.right - rcClip.left) < vW || (rcClip.bottom - rcClip.top) < vH)
            return true;
    }
    CURSORINFO ci = {};
    ci.cbSize = sizeof(CURSORINFO);
    if (GetCursorInfo(&ci) && ci.flags == 0)
        return true;
    return false;
}
static D2D1_COLOR_F ShiftToComplementary(D2D1_COLOR_F c) {
    float h, s, v;
    RGBtoHSV(c, h, s, v);
    float shiftAngle = 0.0f;
    switch (g_colorShiftMode) {
        case 1: shiftAngle = 180.0f; break;
        case 2: shiftAngle = 30.0f; break;
        case 3: shiftAngle = 120.0f; break;
        case 4: shiftAngle = 150.0f; break;
        case 5: shiftAngle = (float)g_colorShiftAngle; break;
        default: shiftAngle = 0.0f; break;
    }
    h = fmodf(h + shiftAngle, 360.0f);
    s = fmaxf(s, g_colorShiftSatBoost);
    v = fmaxf(v, g_colorShiftValBoost);
    return HSVtoRGB(h, s, v);
}
static inline void PublishCursorColor(const D2D1_COLOR_F &src) {
    std::atomic_ref<float>(g_cursorExtractedColor.r).store(src.r, std::memory_order_release);
    std::atomic_ref<float>(g_cursorExtractedColor.g).store(src.g, std::memory_order_release);
    std::atomic_ref<float>(g_cursorExtractedColor.b).store(src.b, std::memory_order_release);
    std::atomic_ref<float>(g_cursorExtractedColor.a).store(src.a, std::memory_order_release);
}
static void ExtractCursorColor(POINT pt, DWORD dwTime) {
    if (dwTime - s_lastColorExtract < 100)
        return;
    s_lastColorExtract = dwTime;
    HDC hdcScreen = GetDC(NULL);
    if (hdcScreen) {
        COLORREF col = GetPixel(hdcScreen, pt.x + 2, pt.y + 4);
        ReleaseDC(NULL, hdcScreen);
        if (col != CLR_INVALID) {
            D2D1_COLOR_F newColor =
                D2D1::ColorF(GetRValue(col) / 255.0f, GetGValue(col) / 255.0f, GetBValue(col) / 255.0f, 1.0f);
            if (g_cursorColorShift)
                newColor = ShiftToComplementary(newColor);
            D2D1_COLOR_F published = LerpColor(g_cursorExtractedColor, newColor, 0.22f);
            PublishCursorColor(published);
        }
    }
}
static void SampleBackgroundLuminance(const std::vector<D2D1_POINT_2F>& path, DWORD dwTime, int vX, int vY) {
    if (path.size() < 2) return;
    if (dwTime - g_lastBgSample < 500) return;
    g_lastBgSample = dwTime;
    const float OFFSET = 10.0f;
    struct Pt { int x, y; };
    Pt pts[6];
    int nPts = 0;
    size_t n = path.size();
    int step = (int)n / 3;
    if (step < 1) step = 1;
    for (size_t i = 0; i < n && nPts < 6; i += step) {
        size_t i0 = (i == 0) ? 0 : i - 1;
        size_t i1 = (i == n - 1) ? n - 1 : i + 1;
        float dx = path[i1].x - path[i0].x;
        float dy = path[i1].y - path[i0].y;
        float ln = sqrtf(dx * dx + dy * dy);
        if (ln < 0.001f) continue;
        float nx = -dy / ln, ny = dx / ln;
        float bx = path[i].x + vX, by = path[i].y + vY;
        pts[nPts++] = {(int)(bx + nx * OFFSET), (int)(by + ny * OFFSET)};
        pts[nPts++] = {(int)(bx - nx * OFFSET), (int)(by - ny * OFFSET)};
    }
    if (nPts == 0) return;
    int minX = pts[0].x, maxX = pts[0].x, minY = pts[0].y, maxY = pts[0].y;
    for (int i = 1; i < nPts; ++i) {
        if (pts[i].x < minX) minX = pts[i].x;
        if (pts[i].x > maxX) maxX = pts[i].x;
        if (pts[i].y < minY) minY = pts[i].y;
        if (pts[i].y > maxY) maxY = pts[i].y;
    }
    int w = maxX - minX + 1, h = maxY - minY + 1;
    if (w < 1) w = 1;
    if (h < 1) h = 1;
    if (w > 128) w = 128;
    if (h > 128) h = 128;
    HDC hdcScreen = GetDC(NULL);
    if (!hdcScreen) return;
    BITMAPINFO bmi = {};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = w;
    bmi.bmiHeader.biHeight = -h;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;
    void* pBits = nullptr;
    HBITMAP hBmp = CreateDIBSection(hdcScreen, &bmi, DIB_RGB_COLORS, &pBits, nullptr, 0);
    HDC hdcMem = CreateCompatibleDC(hdcScreen);
    if (!hBmp || !hdcMem || !pBits) {
        if (hdcMem) DeleteDC(hdcMem);
        if (hBmp) DeleteObject(hBmp);
        ReleaseDC(NULL, hdcScreen);
        return;
    }
    HGDIOBJ oldBmp = SelectObject(hdcMem, hBmp);
    BitBlt(hdcMem, 0, 0, w, h, hdcScreen, minX, minY, SRCCOPY);
    float totalLum = 0.0f;
    int samples = 0;
    const BYTE* pSrc = (const BYTE*)pBits;
    for (int i = 0; i < nPts; ++i) {
        int dx = pts[i].x - minX, dy = pts[i].y - minY;
        if (dx < 0 || dx >= w || dy < 0 || dy >= h) continue;
        const BYTE* px = pSrc + (dy * w + dx) * 4;
        const D2D1_COLOR_F pxCol = D2D1::ColorF(px[2] / 255.0f, px[1] / 255.0f, px[0] / 255.0f, 1.0f);
        float lum = mtcolor::LumaEncoded(pxCol);
        totalLum += lum;
        samples++;
    }
    SelectObject(hdcMem, oldBmp);
    DeleteObject(hBmp);
    DeleteDC(hdcMem);
    ReleaseDC(NULL, hdcScreen);
    if (samples > 0) {
        float avgLum = totalLum / samples;
        float newLum = g_bgLuminance.load(std::memory_order_relaxed) * 0.7f + avgLum * 0.3f;
        g_bgLuminance.store(newLum, std::memory_order_release);
    }
}
static DWORD WINAPI BgSamplerThreadProc(LPVOID) {
    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_BELOW_NORMAL);
    if (!g_bgSampleExitEvent) return 0;
    std::vector<D2D1_POINT_2F> path;
    while (WaitForSingleObject(g_bgSampleExitEvent, 50) != WAIT_OBJECT_0) {
        int vX, vY; POINT cursor; DWORD dwTime; bool hasPath; int colorMode; bool adaptive;
        EnterCriticalSection(&g_sampleCS);
        path = g_samplePath;
        vX = g_sampleVX; vY = g_sampleVY;
        cursor = g_sampleCursor; dwTime = g_sampleTime;
        hasPath = g_sampleHasPath;
        colorMode = g_sampleColorMode;
        adaptive = g_sampleAdaptive;
        LeaveCriticalSection(&g_sampleCS);
        if (!hasPath) continue;
        if (adaptive) {
            SampleBackgroundLuminance(path, dwTime, vX, vY);
        }
        if (colorMode == 10 || colorMode == 11) {
            ExtractCursorColor(cursor, dwTime);
        }
    }
    return 0;
}
static inline void GetPathNormal(const std::vector<D2D1_POINT_2F> &pts, size_t i, float &nx, float &ny) {
    float dx, dy;
    if (pts.size() < 2) { nx = 0; ny = 1; return; }
    if (i == 0) { dx = pts[1].x - pts[0].x; dy = pts[1].y - pts[0].y; }
    else if (i == pts.size() - 1) { dx = pts[i].x - pts[i-1].x; dy = pts[i].y - pts[i-1].y; }
    else { dx = pts[i+1].x - pts[i-1].x; dy = pts[i+1].y - pts[i-1].y; }
    float len = sqrtf(dx*dx + dy*dy);
    if (len > 0.001f) { nx = -dy / len; ny = dx / len; }
    else { nx = 0; ny = 1; }
}
static void ApplySpiralDeformation(std::vector<D2D1_POINT_2F> &pts, DWORD dwTime) {
    if (pts.size() < 3) return;
    float phase = dwTime * 0.005f;
    for (size_t i = 0; i < pts.size(); ++i) {
        float ratio = (float)i / (pts.size() - 1);
        float spiral = sinf(ratio * 12.0f + phase) * (1.0f - ratio) * 15.0f;
        float nx, ny;
        GetPathNormal(pts, i, nx, ny);
        pts[i].x += nx * spiral;
        pts[i].y += ny * spiral;
    }
}
static void ApplyLightningDeformation(std::vector<D2D1_POINT_2F> &pts) {
    if (pts.size() < 3) return;
    static std::vector<D2D1_POINT_2F> result;
    result.clear();
    result.reserve(pts.size() * 2);
    for (size_t i = 0; i < pts.size() - 1; ++i) {
        result.push_back(pts[i]);
        float mx = (pts[i].x + pts[i+1].x) * 0.5f;
        float my = (pts[i].y + pts[i+1].y) * 0.5f;
        float dx = pts[i+1].x - pts[i].x, dy = pts[i+1].y - pts[i].y;
        float len = sqrtf(dx*dx + dy*dy);
        if (len > 0.001f) {
            float nx = -dy / len, ny = dx / len;
            float offset = (Rand01() - 0.5f) * 16.0f;
            result.push_back({mx + nx * offset, my + ny * offset});
        }
    }
    result.push_back(pts.back());
    pts = result;
}
static void ApplyFeatherDeformation(std::vector<D2D1_POINT_2F> &pts, DWORD dwTime) {
    if (pts.size() < 3) return;
    float phase = dwTime * 0.003f;
    for (size_t i = 0; i < pts.size(); ++i) {
        float ratio = (float)i / (pts.size() - 1);
        float curve = sinf(ratio * 3.0f + phase) * (1.0f - ratio) * 6.0f;
        float nx, ny;
        GetPathNormal(pts, i, nx, ny);
        pts[i].x += nx * curve;
        pts[i].y += ny * curve;
    }
}
static void ApplyWaveDeformation(std::vector<D2D1_POINT_2F> &pts, DWORD dwTime) {
    if (pts.size() < 3)
        return;
    float freq = g_waveFrequency / 100.0f, amp = (float)g_waveAmplitude;
    float phase = dwTime * 0.004f, dist = 0;
    for (size_t i = 0; i < pts.size(); ++i) {
        if (i > 0) {
            float ddx = pts[i].x - pts[i - 1].x, ddy = pts[i].y - pts[i - 1].y;
            dist += sqrtf(ddx * ddx + ddy * ddy);
        }
        float nx, ny;
        GetPathNormal(pts, i, nx, ny);
        float taper = 1.0f - (float)i / (pts.size() - 1) * 0.65f;
        float wave = sinf(dist * freq + phase) * amp * taper;
        pts[i].x += nx * wave;
        pts[i].y += ny * wave;
    }
}
static void ApplyFunctionDeformation(std::vector<D2D1_POINT_2F> &pts, DWORD dwTime) {
    if (pts.size() < 3 || !g_exprValid)
        return;
    float time = dwTime / 1000.0f, dist = 0;
    float totalDist = 0;
    for (size_t i = 1; i < pts.size(); i++) {
        float ddx = pts[i].x - pts[i - 1].x, ddy = pts[i].y - pts[i - 1].y;
        totalDist += sqrtf(ddx * ddx + ddy * ddy);
    }
    for (size_t i = 0; i < pts.size(); ++i) {
        if (i > 0) {
            float ddx = pts[i].x - pts[i - 1].x, ddy = pts[i].y - pts[i - 1].y;
            dist += sqrtf(ddx * ddx + ddy * ddy);
        }
        float nx, ny;
        GetPathNormal(pts, i, nx, ny);
        float t = totalDist > 0 ? dist / totalDist : 0;
        float offset = EvalExpression(t, dist, time);
        offset = fmaxf(-60.0f, fminf(60.0f, offset));
        pts[i].x += nx * offset;
        pts[i].y += ny * offset;
    }
}
struct DotInfo {
    D2D1_POINT_2F pos;
    float radius;
    D2D1_COLOR_F outer;
    D2D1_COLOR_F inner;
    float alpha;
};
static bool DetectHDRMode() {
    UINT32 numPaths = 0, numModes = 0;
    if (GetDisplayConfigBufferSizes(QDC_ONLY_ACTIVE_PATHS, &numPaths, &numModes) != ERROR_SUCCESS)
        return false;
    if (numPaths == 0 || numModes == 0) return false;
    std::vector<DISPLAYCONFIG_PATH_INFO> paths(numPaths);
    std::vector<DISPLAYCONFIG_MODE_INFO> modes(numModes);
    if (QueryDisplayConfig(QDC_ONLY_ACTIVE_PATHS, &numPaths, paths.data(), &numModes, modes.data(),
                           nullptr) != ERROR_SUCCESS)
        return false;
    bool hdr = false;
    for (UINT32 i = 0; i < numPaths && i < (UINT32)paths.size(); i++) {
        DISPLAYCONFIG_GET_ADVANCED_COLOR_INFO colorInfo = {};
        colorInfo.header.type = DISPLAYCONFIG_DEVICE_INFO_GET_ADVANCED_COLOR_INFO;
        colorInfo.header.size = sizeof(colorInfo);
        colorInfo.header.adapterId = paths[i].sourceInfo.adapterId;
        colorInfo.header.id = paths[i].sourceInfo.id;
        if (DisplayConfigGetDeviceInfo(&colorInfo.header) != ERROR_SUCCESS)
            continue;
        if (colorInfo.advancedColorEnabled && colorInfo.advancedColorSupported) {
            hdr = true;
            break;
        }
    }
    return hdr;
}
static UINT64 SwapFormatBytesPerPixel(DXGI_FORMAT fmt) {
    switch (fmt) {
        case DXGI_FORMAT_R16G16B16A16_FLOAT:
            return 8;
        default:
            return 4;
    }
}
static void RecreateSwapChain(int vW, int vH) {
    if (vW < 1 || vH < 1) return;
    if (g_pD2DDC)
        g_pD2DDC->SetTarget(nullptr);
    if (g_pCachedRTV) {
        g_pCachedRTV->Release();
        g_pCachedRTV = nullptr;
    }
    if (g_pD2DTargetBitmap) {
        g_pD2DTargetBitmap->Release();
        g_pD2DTargetBitmap = nullptr;
    }
    if (g_pSwapChain) {
        g_pSwapChain->Release();
        g_pSwapChain = nullptr;
    }
    if (!g_pDXGIDevice || !g_pD3DDevice || !g_pD2DDC)
        return;
    bool useScRGB = false;
    bool useHDR10 = false;
    bool hdrDetected = DetectHDRMode();
    if (g_hdrMode == 1) {
        useScRGB = false; useHDR10 = false;
    } else if (g_hdrMode == 2) {
        useScRGB = true;
    } else if (g_hdrMode == 3) {
        useHDR10 = true;
    } else {
        useScRGB = hdrDetected;
    }
    if (useHDR10 && g_pD2DDC) {
        if (!g_pD2DDC->IsDxgiFormatSupported(DXGI_FORMAT_R10G10B10A2_UNORM)) {
            Wh_Log(L"RecreateSwapChain: D2D cannot use R10G10B10A2 as a target, falling back to SDR");
            useHDR10 = false;
            useScRGB = false;
        }
    }
    g_isHDRMode = useScRGB || useHDR10;
    g_hdrColorSpaceApplied = false;
    if (useHDR10) {
        g_swapChainFormat = DXGI_FORMAT_R10G10B10A2_UNORM;
    } else if (useScRGB) {
        g_swapChainFormat = DXGI_FORMAT_R16G16B16A16_FLOAT;
    } else {
        g_swapChainFormat = DXGI_FORMAT_B8G8R8A8_UNORM;
    }
    IDXGIFactory2 *pFactory = nullptr;
    if (FAILED(CreateDXGIFactory1(__uuidof(IDXGIFactory2), (void **)&pFactory)))
        return;
    DXGI_SWAP_CHAIN_DESC1 desc = {};
    desc.Width = vW;
    desc.Height = vH;
    desc.Format = g_swapChainFormat;
    desc.SampleDesc.Count = 1;
    desc.SampleDesc.Quality = 0;
    desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    desc.BufferCount = 2;
    desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
    desc.AlphaMode = DXGI_ALPHA_MODE_PREMULTIPLIED;
    desc.Flags = 0;
    HRESULT hr = pFactory->CreateSwapChainForComposition(g_pD3DDevice, &desc, nullptr, &g_pSwapChain);
    pFactory->Release();
    if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET) {
        Wh_Log(L"RecreateSwapChain: device removed/reset (0x%08X), triggering recovery", hr);
        g_deviceLost.store(true);
        return;
    }
    if (FAILED(hr) || !g_pSwapChain) {
        if (g_isHDRMode) {
            Wh_Log(L"RecreateSwapChain: HDR format failed, falling back to SDR");
            g_isHDRMode = false;
            g_swapChainFormat = DXGI_FORMAT_B8G8R8A8_UNORM;
            desc.Format = g_swapChainFormat;
            IDXGIFactory2 *pFactory2 = nullptr;
            if (SUCCEEDED(CreateDXGIFactory1(__uuidof(IDXGIFactory2), (void **)&pFactory2))) {
                hr = pFactory2->CreateSwapChainForComposition(g_pD3DDevice, &desc, nullptr, &g_pSwapChain);
                pFactory2->Release();
            }
        }
        if (FAILED(hr) || !g_pSwapChain) {
            Wh_Log(L"RecreateSwapChain: CreateSwapChainForComposition failed: 0x%08X (%dx%d)", hr, vW, vH);
            return;
        }
    }
    if (g_isHDRMode) {
        IDXGISwapChain3 *pSwapChain3 = nullptr;
        if (SUCCEEDED(g_pSwapChain->QueryInterface(__uuidof(IDXGISwapChain3), (void **)&pSwapChain3)) && pSwapChain3) {
            DXGI_COLOR_SPACE_TYPE cs = useHDR10 ? DXGI_COLOR_SPACE_RGB_FULL_G2084_NONE_P2020
                                               : DXGI_COLOR_SPACE_RGB_FULL_G10_NONE_P709;
            HRESULT csHr = pSwapChain3->SetColorSpace1(cs);
            pSwapChain3->Release();
            if (SUCCEEDED(csHr)) {
                g_hdrColorSpaceApplied = true;
                Wh_Log(L"RecreateSwapChain: colour space set (space=%d)", (int)cs);
            } else {
                g_hdrColorSpaceApplied = false;
                Wh_Log(L"RecreateSwapChain: SetColorSpace1 failed: 0x%08X, keeping legacy behaviour", csHr);
            }
        } else {
            g_hdrColorSpaceApplied = false;
            Wh_Log(L"RecreateSwapChain: IDXGISwapChain3 unavailable, keeping legacy behaviour");
        }
    }
    IDXGISurface *pSurface = nullptr;
    if (FAILED(g_pSwapChain->GetBuffer(0, __uuidof(IDXGISurface), (void **)&pSurface))) {
        Wh_Log(L"RecreateSwapChain: GetBuffer failed, triggering recovery");
        g_deviceLost.store(true);
        return;
    }
    D2D1_BITMAP_PROPERTIES1 bmpProps = {};
    bmpProps.pixelFormat = D2D1::PixelFormat(g_swapChainFormat, D2D1_ALPHA_MODE_PREMULTIPLIED);
    bmpProps.dpiX = 96.0f;
    bmpProps.dpiY = 96.0f;
    bmpProps.bitmapOptions = D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW;
    hr = g_pD2DDC->CreateBitmapFromDxgiSurface(pSurface, &bmpProps, &g_pD2DTargetBitmap);
    pSurface->Release();
    if (FAILED(hr) && g_isHDRMode) {
        Wh_Log(L"RecreateSwapChain: D2D target failed for HDR format (0x%08X), retrying as SDR", hr);
        if (g_pSwapChain) { g_pSwapChain->Release(); g_pSwapChain = nullptr; }
        g_isHDRMode = false;
        g_hdrColorSpaceApplied = false;
        g_swapChainFormat = DXGI_FORMAT_B8G8R8A8_UNORM;
        bmpProps.pixelFormat = D2D1::PixelFormat(g_swapChainFormat, D2D1_ALPHA_MODE_PREMULTIPLIED);
        IDXGIFactory2 *pFactorySdr = nullptr;
        if (SUCCEEDED(CreateDXGIFactory1(__uuidof(IDXGIFactory2), (void **)&pFactorySdr))) {
            DXGI_SWAP_CHAIN_DESC1 descSdr = desc;
            descSdr.Format = g_swapChainFormat;
            if (FAILED(pFactorySdr->CreateSwapChainForComposition(g_pD3DDevice, &descSdr, nullptr, &g_pSwapChain)))
                g_pSwapChain = nullptr;
            pFactorySdr->Release();
        }
        hr = E_FAIL;
        if (g_pSwapChain) {
            IDXGISurface *pSurfaceSdr = nullptr;
            if (SUCCEEDED(g_pSwapChain->GetBuffer(0, __uuidof(IDXGISurface), (void **)&pSurfaceSdr))) {
                hr = g_pD2DDC->CreateBitmapFromDxgiSurface(pSurfaceSdr, &bmpProps, &g_pD2DTargetBitmap);
                pSurfaceSdr->Release();
            }
        }
    }
    if (FAILED(hr)) {
        Wh_Log(L"RecreateSwapChain: CreateBitmapFromDxgiSurface failed: 0x%08X", hr);
        if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET || hr == D2DERR_RECREATE_TARGET)
            g_deviceLost.store(true);
        return;
    }
    Wh_Log(L"RecreateSwapChain: swap chain ready (%dx%d), HDR=%d, format=%d", vW, vH, g_isHDRMode, (int)g_swapChainFormat);
    {
        IDXGISurface *pRtvSurface = nullptr;
        ID3D11Texture2D *pRtvTexture = nullptr;
        if (SUCCEEDED(g_pD2DTargetBitmap->GetSurface(&pRtvSurface)) &&
            SUCCEEDED(pRtvSurface->QueryInterface(__uuidof(ID3D11Texture2D), (void **)&pRtvTexture))) {
            g_pD3DDevice->CreateRenderTargetView(pRtvTexture, nullptr, &g_pCachedRTV);
            pRtvTexture->Release();
            pRtvSurface->Release();
        }
    }
    if (g_pMSAATexture) { g_pMSAATexture->Release(); g_pMSAATexture = nullptr; }
    if (g_pMSAARTV) { g_pMSAARTV->Release(); g_pMSAARTV = nullptr; }
    if (g_pSSAATexture) { g_pSSAATexture->Release(); g_pSSAATexture = nullptr; }
    if (g_pSSAARTV) { g_pSSAARTV->Release(); g_pSSAARTV = nullptr; }
    if (g_pSSAASRV) { g_pSSAASRV->Release(); g_pSSAASRV = nullptr; }
    if (g_pSSAAResolveTexture) { g_pSSAAResolveTexture->Release(); g_pSSAAResolveTexture = nullptr; }
    if (g_pSSAAResolveSRV) { g_pSSAAResolveSRV->Release(); g_pSSAAResolveSRV = nullptr; }
    ReleasePostTargets();
    int renderW = vW, renderH = vH;
    bool useSSAA = (g_ssaaScale > 1);
    bool useMSAA = (g_msaaSamples > 1);
    if (useSSAA) {
        renderW = vW * g_ssaaScale;
        renderH = vH * g_ssaaScale;
        const UINT maxDim = D3D11_REQ_TEXTURE2D_U_OR_V_DIMENSION;
        if ((UINT)renderW > maxDim || (UINT)renderH > maxDim) {
            Wh_Log(L"SSAA: %dx exceeds texture limit, disabling SSAA", g_ssaaScale);
            useSSAA = false;
            g_ssaaScale = 1;
            renderW = vW; renderH = vH;
        }
    }
    {
        const UINT64 bytesPerPixel = SwapFormatBytesPerPixel(g_swapChainFormat);
        const UINT64 budgetBytes = 256ull * 1024ull * 1024ull;
        bool shedSomething = false;
        while ((useMSAA || useSSAA) &&
               (UINT64)renderW * (UINT64)renderH * (UINT64)(useMSAA ? g_msaaSamples : 1) * bytesPerPixel > budgetBytes) {
            if (useMSAA && g_msaaSamples > 4) {
                g_msaaSamples = 4;
            } else if (useMSAA && g_msaaSamples > 2) {
                g_msaaSamples = 2;
            } else if (useMSAA) {
                useMSAA = false;
                g_msaaSamples = 1;
            } else if (g_ssaaScale > 3) {
                g_ssaaScale = 3;
                renderW = vW * g_ssaaScale;
                renderH = vH * g_ssaaScale;
            } else if (g_ssaaScale > 2) {
                g_ssaaScale = 2;
                renderW = vW * g_ssaaScale;
                renderH = vH * g_ssaaScale;
            } else {
                useSSAA = false;
                g_ssaaScale = 1;
                renderW = vW; renderH = vH;
            }
            shedSomething = true;
        }
        if (shedSomething) {
            Wh_Log(L"AA budget: clamped to %dx%d RT (MSAA %dx, SSAA %dx)",
                   renderW, renderH, useMSAA ? g_msaaSamples : 1, g_ssaaScale);
        }
    }
    UINT msaaQuality = 0;
    if (useMSAA) {
        HRESULT hrCheck = g_pD3DDevice->CheckMultisampleQualityLevels(
            g_swapChainFormat, g_msaaSamples, &msaaQuality);
        if (FAILED(hrCheck) || msaaQuality == 0) {
            Wh_Log(L"MSAA: %dx not supported, disabling", g_msaaSamples);
            useMSAA = false;
            g_msaaSamples = 1;
        }
    }
    D3D11_TEXTURE2D_DESC rtDesc = {};
    rtDesc.Width = renderW;
    rtDesc.Height = renderH;
    rtDesc.MipLevels = 1;
    rtDesc.ArraySize = 1;
    rtDesc.Format = g_swapChainFormat;
    rtDesc.SampleDesc.Count = useMSAA ? g_msaaSamples : 1;
    rtDesc.SampleDesc.Quality = useMSAA ? (msaaQuality - 1) : 0;
    rtDesc.Usage = D3D11_USAGE_DEFAULT;
    rtDesc.BindFlags = D3D11_BIND_RENDER_TARGET;
    rtDesc.CPUAccessFlags = 0;
    rtDesc.MiscFlags = 0;
    bool msAAOk = false;
    if (useMSAA) {
        if (SUCCEEDED(g_pD3DDevice->CreateTexture2D(&rtDesc, nullptr, &g_pMSAATexture))) {
            g_pD3DDevice->CreateRenderTargetView(g_pMSAATexture, nullptr, &g_pMSAARTV);
        }
        rtDesc.SampleDesc.Count = 1;
        rtDesc.SampleDesc.Quality = 0;
        rtDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
        if (SUCCEEDED(g_pD3DDevice->CreateTexture2D(&rtDesc, nullptr, &g_pSSAAResolveTexture))) {
            g_pD3DDevice->CreateShaderResourceView(g_pSSAAResolveTexture, nullptr, &g_pSSAAResolveSRV);
        }
        if (g_pMSAARTV && g_pSSAAResolveSRV) {
            if (g_pCachedRTV) g_pCachedRTV->Release();
            g_pCachedRTV = g_pMSAARTV;
            g_pMSAARTV->AddRef();
            msAAOk = true;
            Wh_Log(L"MSAA: %dx + SSAA: %dx enabled", g_msaaSamples, g_ssaaScale);
        } else {
            Wh_Log(L"MSAA: offscreen creation failed, disabling MSAA");
            if (g_pMSAATexture) { g_pMSAATexture->Release(); g_pMSAATexture = nullptr; }
            if (g_pMSAARTV) { g_pMSAARTV->Release(); g_pMSAARTV = nullptr; }
            g_msaaSamples = 1;
        }
    }
    if (!msAAOk && useSSAA) {
        rtDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
        if (SUCCEEDED(g_pD3DDevice->CreateTexture2D(&rtDesc, nullptr, &g_pSSAATexture))) {
            g_pD3DDevice->CreateRenderTargetView(g_pSSAATexture, nullptr, &g_pSSAARTV);
            g_pD3DDevice->CreateShaderResourceView(g_pSSAATexture, nullptr, &g_pSSAASRV);
            if (g_pSSAARTV && g_pSSAASRV) {
                if (g_pCachedRTV) g_pCachedRTV->Release();
                g_pCachedRTV = g_pSSAARTV;
                g_pSSAARTV->AddRef();
                Wh_Log(L"SSAA: %dx enabled (%dx%d)", g_ssaaScale, renderW, renderH);
            }
        }
        if (!g_pSSAARTV || !g_pSSAASRV) {
            Wh_Log(L"SSAA: offscreen creation failed, disabling SSAA");
            g_ssaaScale = 1;
            if (g_pSSAATexture) { g_pSSAATexture->Release(); g_pSSAATexture = nullptr; }
            if (g_pSSAARTV) { g_pSSAARTV->Release(); g_pSSAARTV = nullptr; }
            if (g_pSSAASRV) { g_pSSAASRV->Release(); g_pSSAASRV = nullptr; }
        }
    }
    g_createdMsaaSamples = g_msaaSamples;
    g_createdSsaaScale = g_ssaaScale;
    if (g_pDCompVisual) {
        g_pDCompVisual->SetContent(g_pSwapChain);
    }
    if (g_pDCompDevice) {
        g_pDCompDevice->Commit();
    }
    g_cachedVW = vW;
    g_cachedVH = vH;
    InvalidateConstantBuffer();
    EnsurePostTargets(vW, vH);
}
static void CreateMeshFromTriangles(ID2D1DeviceContext *dc, ID2D1Mesh **mesh,
                                   const std::vector<D2D1_POINT_2F> &left,
                                   const std::vector<D2D1_POINT_2F> &right,
                                   bool addCap, D2D1_POINT_2F capCenter, float capRadius) {
    if (*mesh) {
        (*mesh)->Release();
        *mesh = nullptr;
    }
    if (!dc)
        return;
    dc->CreateMesh(mesh);
    if (!*mesh)
        return;
    ID2D1TessellationSink *pSink = nullptr;
    (*mesh)->Open(&pSink);
    if (!pSink) {
        (*mesh)->Release();
        *mesh = nullptr;
        return;
    }
    static std::vector<D2D1_TRIANGLE> tris;
    tris.clear();
    if (left.size() >= 2 && right.size() >= 2) {
        size_t n = (std::min)(left.size(), right.size());
        tris.reserve((n - 1) * 2 + (addCap ? 24 : 0));
        for (size_t i = 0; i < n - 1; i++) {
            tris.push_back({left[i], right[i], left[i + 1]});
            tris.push_back({right[i], right[i + 1], left[i + 1]});
        }
    }
    if (addCap && capRadius > 0.1f) {
        const int SEG = 24;
        for (int i = 0; i < SEG; i++) {
            float a1 = (float)i / SEG * 6.2831853f;
            float a2 = (float)(i + 1) / SEG * 6.2831853f;
            D2D1_POINT_2F p1 = {capCenter.x + cosf(a1) * capRadius, capCenter.y + sinf(a1) * capRadius};
            D2D1_POINT_2F p2 = {capCenter.x + cosf(a2) * capRadius, capCenter.y + sinf(a2) * capRadius};
            tris.push_back({capCenter, p1, p2});
        }
    }
    if (!tris.empty()) {
        pSink->AddTriangles(tris.data(), (UINT32)tris.size());
    }
    pSink->Close();
    pSink->Release();
}
static void CreateMeshFromQuadStrip(ID2D1DeviceContext *dc, ID2D1Mesh **mesh,
                                    const std::vector<D2D1_POINT_2F> &left,
                                    const std::vector<D2D1_POINT_2F> &right) {
    if (!dc || left.size() < 2 || right.size() < 2) {
        if (mesh && *mesh) { (*mesh)->Release(); *mesh = nullptr; }
        return;
    }
    CreateMeshFromTriangles(dc, mesh, left, right, false, {0, 0}, 0.0f);
}
static void CreateMeshFromQuadStripWithCap(ID2D1DeviceContext *dc, ID2D1Mesh **mesh,
                                           const std::vector<D2D1_POINT_2F> &left,
                                           const std::vector<D2D1_POINT_2F> &right,
                                           D2D1_POINT_2F capCenter, float capRadius) {
    if (!dc) {
        if (mesh && *mesh) { (*mesh)->Release(); *mesh = nullptr; }
        return;
    }
    CreateMeshFromTriangles(dc, mesh, left, right, true, capCenter, capRadius);
}
static ID2D1PathGeometry *GetParticleShapeGeom(int pshape) {
    static const int kMap[9] = {3, 1, 8, 0, 4, 5, 6, 7, 2};
    if (pshape < 0 || pshape > 8) return nullptr;
    int tshape = kMap[pshape];
    ShapeVertsCache sv = GetShapeVerts(tshape);
    if (!sv.verts || sv.count < 3) return nullptr;
    return mtb::PolyGeom(sv.verts, sv.count, 900000 + tshape);
}
static void RenderParticles(DWORD dwTime) {
    if (g_particles.empty()) return;
    bool particleFastPath = (g_particleShape != 10) && (g_particles.size() > (g_superPerformanceMode ? 300 : 100));
    float sizeMul = g_particleSizeMultiplier / 100.0f;
    float musicSizeBoost = g_enableMusicPhysics ? (1.0f + g_musicBassSizeBoost * 0.5f) : 1.0f;
    for (auto &p : g_particles) {
        float progress = (float)(dwTime - p.startTime) / p.lifetime;
        if (progress < 0 || progress >= 1) continue;
        float lifeAlpha = (1.0f - progress);
        float colorFade = progress * progress;
        float rOff = p.colorOffset[0] + p.vortexBrightness[0];
        float gOff = p.colorOffset[1] + p.vortexBrightness[1];
        float bOff = p.colorOffset[2] + p.vortexBrightness[2];
        D2D1_COLOR_F pc = D2D1::ColorF(p.color.r + (p.endColor.r - p.color.r) * colorFade + rOff,
                                       p.color.g + (p.endColor.g - p.color.g) * colorFade + gOff,
                                       p.color.b + (p.endColor.b - p.color.b) * colorFade + bOff, 1.0f);
        g_pSolidOuterBrush->SetColor(pc);
        g_pSolidOuterBrush->SetOpacity(lifeAlpha * 0.65f);
        float sizeScale = sinf(progress * 3.14159f) * 0.7f + 0.3f;
        float bodySize = p.size * 2.0f * sizeMul * sizeScale * musicSizeBoost;
        ID2D1PathGeometry *pgeom = particleFastPath ? nullptr : GetParticleShapeGeom(p.shapeType);
        if (pgeom) {
            D2D1_MATRIX_3X2_F oldT;
            g_pD2DDC->GetTransform(&oldT);
            g_pD2DDC->SetTransform(D2D1::Matrix3x2F::Rotation(p.rotation * 57.2958f) *
                                   D2D1::Matrix3x2F::Scale(bodySize, bodySize) *
                                   D2D1::Matrix3x2F::Translation(p.x, p.y));
            g_pD2DDC->FillGeometry(pgeom, g_pSolidOuterBrush);
            g_pD2DDC->SetTransform(oldT);
        } else {
            g_pD2DDC->FillEllipse(D2D1::Ellipse(D2D1::Point2F(p.x, p.y), bodySize, bodySize), g_pSolidOuterBrush);
        }
    }
    g_pSolidOuterBrush->SetOpacity(1.0f);
}
static void RenderMotionBlur(float widthMul, const GradData &cols, float fadeAlpha) {
    if (!g_enableMotionBlur || g_trailShape != 0 || g_trailHistory.size() <= 1) return;
    g_pD2DDC->SetAntialiasMode(D2D1_ANTIALIAS_MODE_ALIASED);
    static std::vector<D2D1_POINT_2F> hloBuf, hroBuf;
    for (size_t h = 0; h < g_trailHistory.size() - 1; h++) {
        auto &histPath = g_trailHistory[h].path;
        if (histPath.size() < 2) continue;
        float histAlpha = 0.22f * (1.0f - (float)h / g_trailHistory.size()) * fadeAlpha;
        std::vector<D2D1_POINT_2F> &hlo = hloBuf, &hro = hroBuf;
        hlo.clear(); hro.clear();
        hlo.reserve(histPath.size()); hro.reserve(histPath.size());
        for (size_t i = 0; i < histPath.size(); i++) {
            float ddx, ddy;
            if (i == 0) { ddx = histPath[0].x - histPath[1].x; ddy = histPath[0].y - histPath[1].y; }
            else if (i == histPath.size() - 1) { ddx = histPath[i-1].x - histPath[i].x; ddy = histPath[i-1].y - histPath[i].y; }
            else { ddx = histPath[i-1].x - histPath[i+1].x; ddy = histPath[i-1].y - histPath[i+1].y; }
            float ln = sqrtf(ddx*ddx + ddy*ddy);
            if (ln > 0) { ddx /= ln; ddy /= ln; } else { ddx = 1; ddy = 0; }
            float nx = -ddy, ny = ddx;
            float ratio = (float)i / (histPath.size() - 1);
            float ow = 10.0f * MtTaperFactor(ratio) * widthMul;
            if (i == histPath.size() - 1) ow = 0;
            hlo.push_back(D2D1::Point2F(histPath[i].x + nx*ow, histPath[i].y + ny*ow));
            hro.push_back(D2D1::Point2F(histPath[i].x - nx*ow, histPath[i].y - ny*ow));
        }
        ID2D1Mesh *pHistMesh = nullptr;
        CreateMeshFromQuadStrip(g_pD2DDC, &pHistMesh, hlo, hro);
        if (pHistMesh) {
            g_pSolidOuterBrush->SetColor(cols.solidOuter);
            g_pSolidOuterBrush->SetOpacity(histAlpha);
            g_pD2DDC->FillMesh(pHistMesh, g_pSolidOuterBrush);
            pHistMesh->Release();
        }
    }
    g_pSolidOuterBrush->SetOpacity(1.0f);
    g_pD2DDC->SetAntialiasMode(D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
}
struct VortexFrameContext {
    DWORD dwTime;
    int vX, vY;
    float velocity;
    float accel;
    float accelAbs;
    float cursorX;
    float cursorY;
};
static void VortexDetectCurvedMotion(const VortexFrameContext &ctx,
                                     bool &detected, float &detCX, float &detCY,
                                     float &detRadius, float &detAngVel) {
    CircleDetect &cd = g_circleDetect;
    float histX = ctx.cursorX;
    float histY = ctx.cursorY;
    if (cd.historyCount < 20) {
        cd.historyX[cd.historyCount] = histX;
        cd.historyY[cd.historyCount] = histY;
        cd.historyCount++;
    } else {
        for (int i = 0; i < 19; i++) {
            cd.historyX[i] = cd.historyX[i + 1];
            cd.historyY[i] = cd.historyY[i + 1];
        }
        cd.historyX[19] = histX;
        cd.historyY[19] = histY;
    }
    detected = false;
    detCX = 0; detCY = 0; detRadius = 0; detAngVel = 0;
    float accelBoost = fminf(ctx.accelAbs / 10.0f, 1.0f);
    float minVel = 3.0f - accelBoost * 2.0f;
    if (cd.historyCount >= 5 && ctx.velocity > minVel) {
        int start = cd.historyCount - 5;
        float sumCX = 0, sumCY = 0, sumRadius = 0, sumAngVel = 0;
        int validCount = 0;
        for (int tri = 0; tri < 3; tri++) {
            float x1 = cd.historyX[start + tri], y1 = cd.historyY[start + tri];
            float x2 = cd.historyX[start + tri + 1], y2 = cd.historyY[start + tri + 1];
            float x3 = cd.historyX[start + tri + 2], y3 = cd.historyY[start + tri + 2];
            float ax = x2 - x1, ay = y2 - y1;
            float bx = x3 - x1, by = y3 - y1;
            float cross = ax * by - ay * bx;
            if (fabsf(cross) < 0.5f) continue;
            float d = 2.0f * cross;
            float aLenSq = ax * ax + ay * ay;
            float bLenSq = bx * bx + by * by;
            float cx = x1 + (by * aLenSq - ay * bLenSq) / d;
            float cy = y1 + (ax * bLenSq - bx * aLenSq) / d;
            float radius = sqrtf((x1 - cx) * (x1 - cx) + (y1 - cy) * (y1 - cy));
            if (radius < 20 || radius > 400) continue;
            float angVel = (ctx.velocity / radius) * (cross > 0 ? 1.0f : -1.0f);
            sumCX += cx; sumCY += cy; sumRadius += radius; sumAngVel += angVel;
            validCount++;
        }
        if (validCount >= 2) {
            float avgCX = sumCX / validCount;
            float avgCY = sumCY / validCount;
            float avgRadius = sumRadius / validCount;
            float avgAngVel = sumAngVel / validCount;
            if (cd.circleRadius > 0.0f && (ctx.dwTime - cd.lastCircleTime) < 600) {
                avgCX = cd.circleCenterX * 0.55f + avgCX * 0.45f;
                avgCY = cd.circleCenterY * 0.55f + avgCY * 0.45f;
                avgRadius = cd.circleRadius * 0.6f + avgRadius * 0.4f;
                avgAngVel = cd.angularVel * 0.5f + avgAngVel * 0.5f;
            }
            float curvatureThreshold = (1.0f - g_centripetalSensitivity) * 200.0f + 50.0f;
            if (avgRadius < curvatureThreshold) {
                detected = true;
                detCX = avgCX; detCY = avgCY;
                detRadius = avgRadius; detAngVel = avgAngVel;
                cd.isCircling = true;
                cd.circleCenterX = avgCX;
                cd.circleCenterY = avgCY;
                cd.circleRadius = avgRadius;
                cd.angularVel = avgAngVel;
                cd.lastCircleTime = ctx.dwTime;
            }
        }
    }
}
static void VortexUpdateOrCreate(const VortexFrameContext &ctx,
                                bool detected, float detCX, float detCY,
                                float detRadius, float detAngVel,
                                int activeVortexCount) {
    if (!detected) return;
    float accelBoost = fminf(ctx.accelAbs / 10.0f, 1.0f);
    float curvatureThreshold = (1.0f - g_centripetalSensitivity) * 200.0f + 50.0f;
    float curvatureBoost = fminf((curvatureThreshold - detRadius) / curvatureThreshold, 1.0f);
    float boostRate = 0.06f + curvatureBoost * 0.1f + accelBoost * 0.08f;
    bool updatedExisting = false;
    for (int i = 0; i < g_vortexMaxCount; i++) {
        Vortex &v = g_vortices[i];
        if (!v.active) continue;
        float dx = v.centerX - detCX, dy = v.centerY - detCY;
        float distSq = dx * dx + dy * dy;
        if (distSq < (g_vortexMinDistance * 1.8f) * (g_vortexMinDistance * 1.8f)) {
            v.centerX += (detCX - v.centerX) * 0.1f;
            v.centerY += (detCY - v.centerY) * 0.1f;
            if (g_vortexDrift > 0.01f && ctx.velocity > 1.0f) {
                float pullDx = detCX - v.centerX;
                float pullDy = detCY - v.centerY;
                float pullDist = sqrtf(pullDx * pullDx + pullDy * pullDy);
                if (pullDist > 0.1f) {
                    v.driftX += (pullDx / pullDist) * ctx.velocity * g_vortexDrift * 0.03f;
                    v.driftY += (pullDy / pullDist) * ctx.velocity * g_vortexDrift * 0.03f;
                }
            }
            v.radius = detRadius;
            v.coreRadius = detRadius * 0.3f;
            v.isCircling = true;
            v.strength += (1.0f - v.strength) * boostRate;
            v.angularVel = detAngVel * (0.7f + g_centripetalForce * 0.5f) * (1.0f + accelBoost * 0.4f);
            v.circulation = v.angularVel * v.coreRadius * v.coreRadius * 6.28318f;
            v.pressureGradient = 0.3f + v.strength * 0.5f;
            v.stretchRate = fminf(ctx.accelAbs * 0.01f, 0.25f);
            v.orbitPhase += v.angularVel;
            float speedFactor = fminf(ctx.velocity / 15.0f, 1.0f);
            v.actualDuration += (int)(speedFactor * g_vortexDurationSpeed * 10);
            if (v.actualDuration > g_centripetalDuration * 3) v.actualDuration = g_centripetalDuration * 3;
            updatedExisting = true;
            break;
        }
    }
    if (!updatedExisting && activeVortexCount < g_vortexMaxCount) {
        bool recentCreate = false;
        for (int ci = 0; ci < g_vortexMaxCount; ci++) {
            Vortex &cv = g_vortices[ci];
            if (!cv.active) continue;
            if ((int)(ctx.dwTime - cv.startTime) < 450) {
                float cdx = cv.centerX - detCX, cdy = cv.centerY - detCY;
                if (cdx*cdx + cdy*cdy < (g_vortexMinDistance * 2.5f) * (g_vortexMinDistance * 2.5f)) {
                    recentCreate = true; break;
                }
            }
        }
        if (recentCreate) return;
        bool tooClose = false;
        for (int i = 0; i < g_vortexMaxCount; i++) {
            Vortex &v = g_vortices[i];
            if (!v.active) continue;
            float dx = v.centerX - detCX, dy = v.centerY - detCY;
            float distSq = dx * dx + dy * dy;
            if (distSq < g_vortexMinDistance * g_vortexMinDistance) {
                tooClose = true;
                break;
            }
        }
        if (!tooClose) {
            for (int i = 0; i < g_vortexMaxCount; i++) {
                if (!g_vortices[i].active) {
                    Vortex &v = g_vortices[i];
                    v.active = true;
                    v.centerX = detCX;
                    v.centerY = detCY;
                    v.driftX = 0; v.driftY = 0;
                    v.radius = detRadius;
                    v.coreRadius = detRadius * 0.3f;
                    v.angularVel = detAngVel * (0.7f + g_centripetalForce * 0.5f) * (1.0f + accelBoost * 0.4f);
                    v.circulation = v.angularVel * v.coreRadius * v.coreRadius * 6.28318f;
                    v.strength = 0.1f;
                    v.strength += (1.0f - v.strength) * boostRate;
                    v.pressureGradient = 0.5f;
                    v.isCircling = true;
                    v.startTime = ctx.dwTime;
                    v.stretchRate = 0;
                    float speedFactor = fminf(ctx.velocity / 15.0f, 1.0f);
                    v.actualDuration = (int)(g_centripetalDuration * (1.0f + speedFactor * g_vortexDurationSpeed));
                    v.orbitTilt = 0.4f + Rand01() * 0.5f;
                    v.orbitTiltVel = (Rand01() - 0.5f) * 0.005f;
                    v.orbitPhase = 0;
                    break;
                }
            }
        }
    }
}
static void VortexMusicInject(const VortexFrameContext &ctx, int activeVortexCount) {
    if (!(g_enableMusicPhysics && g_musicBeatPulseAmount > 0.1f)) return;
    float beatVortexBoost = g_musicBeatPulseAmount * g_musicVortexLink * 0.3f;
    int nearestIdx = -1;
    float nearestDist = 1e9f;
    float cursorX = ctx.cursorX;
    float cursorY = ctx.cursorY;
    for (int i = 0; i < g_vortexMaxCount; i++) {
        if (!g_vortices[i].active) continue;
        float dx = g_vortices[i].centerX - cursorX;
        float dy = g_vortices[i].centerY - cursorY;
        float d = dx * dx + dy * dy;
        if (d < nearestDist) { nearestDist = d; nearestIdx = i; }
    }
    if (nearestIdx >= 0) {
        Vortex &v = g_vortices[nearestIdx];
        v.strength += (1.0f - v.strength) * beatVortexBoost;
    } else if (activeVortexCount < g_vortexMaxCount) {
        for (int i = 0; i < g_vortexMaxCount; i++) {
            if (!g_vortices[i].active) {
                Vortex &v = g_vortices[i];
                v.active = true;
                v.centerX = cursorX;
                v.centerY = cursorY;
                v.driftX = 0; v.driftY = 0;
                v.radius = 60.0f + g_music.bassLevel * 200.0f;
                v.coreRadius = v.radius * 0.3f;
                v.angularVel = (g_music.bpmEstimate > 0) ? (g_music.bpmEstimate / 60.0f * 6.28f / 60.0f) : 0.05f;
                v.circulation = v.angularVel * v.coreRadius * v.coreRadius * 6.28318f;
                v.strength = 0.2f;
                v.strength += (1.0f - v.strength) * beatVortexBoost;
                v.pressureGradient = 0.5f;
                v.isCircling = true;
                v.startTime = ctx.dwTime;
                v.stretchRate = 0;
                float bassFactor = fminf(g_music.bassLevel * 2.0f, 1.0f);
                v.actualDuration = (int)(g_centripetalDuration * (1.0f + bassFactor * g_vortexDurationSpeed));
                v.orbitTilt = 0.4f + Rand01() * 0.5f;
                v.orbitTiltVel = (Rand01() - 0.5f) * 0.005f;
                v.orbitPhase = 0;
                break;
            }
        }
    }
}
static void VortexUpdateDynamics(const VortexFrameContext &ctx) {
    CircleDetect &cd = g_circleDetect;
    if (cd.isCircling && ctx.velocity < 2.0f) {
        cd.isCircling = false;
        cd.lastCircleTime = ctx.dwTime;
        for (int i = 0; i < g_vortexMaxCount; i++) {
            if (g_vortices[i].active) g_vortices[i].isCircling = false;
        }
    }
    float cursorX = ctx.cursorX;
    float cursorY = ctx.cursorY;
    float inducedX[8] = {0}, inducedY[8] = {0};
    for (int i = 0; i < g_vortexMaxCount; i++) {
        if (!g_vortices[i].active) continue;
        for (int j = 0; j < g_vortexMaxCount; j++) {
            if (i == j || !g_vortices[j].active) continue;
            Vortex &vi = g_vortices[i];
            Vortex &vj = g_vortices[j];
            float dx = vi.centerX - vj.centerX;
            float dy = vi.centerY - vj.centerY;
            float dist = sqrtf(dx * dx + dy * dy);
            if (dist < 1.0f) continue;
            float inducedSpeed = fabsf(vj.circulation) / (6.28318f * dist) * vj.strength * 0.3f;
            float perpX = -dy / dist, perpY = dx / dist;
            int sameDir = (vi.angularVel >= 0) == (vj.angularVel >= 0) ? 1 : -1;
            inducedX[i] += perpX * inducedSpeed * sameDir;
            inducedY[i] += perpY * inducedSpeed * sameDir;
        }
    }
    bool merged[8] = {false};
    for (int i = 0; i < g_vortexMaxCount; i++) {
        if (!g_vortices[i].active || merged[i]) continue;
        for (int j = i + 1; j < g_vortexMaxCount; j++) {
            if (!g_vortices[j].active || merged[j]) continue;
            Vortex &vi = g_vortices[i];
            Vortex &vj = g_vortices[j];
            float dx = vi.centerX - vj.centerX;
            float dy = vi.centerY - vj.centerY;
            float dist = sqrtf(dx * dx + dy * dy);
            bool sameDir = (vi.angularVel >= 0) == (vj.angularVel >= 0);
            if (sameDir && dist < (vi.coreRadius + vj.coreRadius) * 1.8f) {
                float wi = vi.strength, wj = vj.strength;
                float totalStrength = fmaxf(wi + wj, 0.001f);
                float nx = (vi.centerX - vj.centerX) / fmaxf(dist, 0.1f);
                float ny = (vi.centerY - vj.centerY) / fmaxf(dist, 0.1f);
                float closeness = 1.0f - dist / ((vi.coreRadius + vj.coreRadius) * 1.8f);
                float pull = 0.10f + closeness * 0.25f;
                vj.centerX += nx * pull * 6.0f;
                vj.centerY += ny * pull * 6.0f;
                float transfer = 0.06f + closeness * 0.08f;
                float circTransfer = vj.circulation * transfer;
                vi.circulation += circTransfer;
                vj.circulation -= circTransfer;
                float strTransfer = vj.strength * transfer;
                vi.strength = fminf(vi.strength + strTransfer * 0.6f, 1.0f);
                vj.strength *= (1.0f - transfer);
                float targetCore = fmaxf(sqrtf(vi.coreRadius * vi.coreRadius + vj.coreRadius * vj.coreRadius) * 1.2f, 1.0f);
                float targetRadius = (vi.radius * wi + vj.radius * wj) / totalStrength;
                vi.coreRadius += (targetCore - vi.coreRadius) * 0.06f;
                vi.radius += (targetRadius - vi.radius) * 0.06f;
                float coreI = fmaxf(vi.coreRadius, 0.5f);
                float coreJ = fmaxf(vj.coreRadius, 0.5f);
                vi.angularVel = vi.circulation / (coreI * coreI * 6.28318f);
                vj.angularVel = vj.circulation / (coreJ * coreJ * 6.28318f);
                vi.driftX += (vj.driftX - vi.driftX) * 0.05f;
                vi.driftY += (vj.driftY - vi.driftY) * 0.05f;
                if (vj.strength < 0.06f || dist < 3.0f) {
                    vi.centerX = (vi.centerX * vi.strength + vj.centerX * vj.strength) / fmaxf(vi.strength + vj.strength, 0.001f);
                    vi.centerY = (vi.centerY * vi.strength + vj.centerY * vj.strength) / fmaxf(vi.strength + vj.strength, 0.001f);
                    vi.circulation += vj.circulation;
                    vi.coreRadius = targetCore;
                    vi.radius = targetRadius;
                    vi.angularVel = vi.circulation / (targetCore * targetCore * 6.28318f);
                    vi.strength = fminf((vi.strength + vj.strength) * 0.7f, 1.0f);
                    vi.pressureGradient = (vi.pressureGradient + vj.pressureGradient) * 0.5f;
                    vi.isCircling = vi.isCircling || vj.isCircling;
                    vi.actualDuration = (vi.actualDuration + vj.actualDuration) / 2;
                    merged[j] = true;
                }
            } else if (!sameDir && dist < (vi.coreRadius + vj.coreRadius) * 1.2f) {
                if (vi.strength > vj.strength) {
                    vi.strength -= vj.strength * 0.8f;
                    vi.circulation -= vj.circulation * 0.8f;
                    if (vi.strength < 0.05f) vi.strength = 0.05f;
                    merged[j] = true;
                } else {
                    vj.strength -= vi.strength * 0.8f;
                    vj.circulation -= vi.circulation * 0.8f;
                    if (vj.strength < 0.05f) vj.strength = 0.05f;
                    merged[i] = true;
                    break;
                }
            }
        }
    }
    for (int i = 0; i < g_vortexMaxCount; i++) {
        if (merged[i]) {
            g_vortices[i].active = false;
            g_vortices[i].strength = 0;
        }
    }
    for (int i = 0; i < g_vortexMaxCount; i++) {
        Vortex &v = g_vortices[i];
        if (!v.active) continue;
        if (g_vortexDrift > 0.01f) {
            float dx = cursorX - v.centerX;
            float dy = cursorY - v.centerY;
            float dist = sqrtf(dx * dx + dy * dy);
            if (dist > 1.0f && dist < 500.0f) {
                float pullStrength = g_vortexDrift * 0.02f * (1.0f - dist / 500.0f);
                v.driftX += (dx / dist) * pullStrength * ctx.velocity * 0.1f;
                v.driftY += (dy / dist) * pullStrength * ctx.velocity * 0.1f;
            }
            v.driftX *= 0.95f;
            v.driftY *= 0.95f;
            v.centerX += v.driftX;
            v.centerY += v.driftY;
        }
        v.centerX += inducedX[i];
        v.centerY += inducedY[i];
        if (ctx.dwTime - v.startTime > (DWORD)v.actualDuration) {
            v.isCircling = false;
        }
        if (v.isCircling && v.stretchRate > 0.001f) {
            v.strength += v.stretchRate * 0.5f;
            if (v.strength > 1.0f) v.strength = 1.0f;
            v.coreRadius *= (1.0f - v.stretchRate * 0.1f);
            if (v.coreRadius < v.radius * 0.1f) v.coreRadius = v.radius * 0.1f;
            v.angularVel = v.circulation / (v.coreRadius * v.coreRadius * 6.28318f);
            v.stretchRate *= 0.9f;
        }
        if (!v.isCircling && v.strength > 0) {
            float decayRate, angVelDecay, coreTarget, coreRate;
            if (v.strength > 0.5f) {
                decayRate = 0.965f; angVelDecay = 0.975f;
                coreTarget = v.radius * 0.35f; coreRate = 0.03f;
            } else if (v.strength > 0.15f) {
                decayRate = 0.93f; angVelDecay = 0.94f;
                coreTarget = v.radius * 0.55f; coreRate = 0.05f;
            } else {
                decayRate = 0.87f; angVelDecay = 0.86f;
                coreTarget = v.radius * 0.85f; coreRate = 0.09f;
            }
            v.strength *= decayRate;
            v.angularVel *= angVelDecay;
            v.coreRadius += (coreTarget - v.coreRadius) * coreRate;
            v.pressureGradient *= 0.96f;
            float coreD = fmaxf(v.coreRadius, 0.5f);
            v.circulation = v.angularVel * coreD * coreD * 6.28318f;
            if (v.strength < 0.012f) {
                v.active = false;
                v.strength = 0;
            }
        }
        if (v.strength > 0.1f) {
            v.orbitTilt += v.orbitTiltVel;
            if (v.orbitTilt < 0.2f || v.orbitTilt > 1.2f) v.orbitTiltVel = -v.orbitTiltVel;
            v.orbitTilt = fmaxf(0.2f, fminf(1.2f, v.orbitTilt));
        }
    }
    bool anyActive = false;
    for (int i = 0; i < g_vortexMaxCount; i++) {
        if (g_vortices[i].active && g_vortices[i].strength > 0.05f) { anyActive = true; break; }
    }
    if (!anyActive) {
        for (auto &p : g_particles) {
            p.vortexBrightness[0] = 0;
            p.vortexBrightness[1] = 0;
            p.vortexBrightness[2] = 0;
        }
    }
}
static void VortexApplyToParticles() {
    if (g_particles.empty()) return;
    for (int vi = 0; vi < g_vortexMaxCount; vi++) {
        Vortex &v = g_vortices[vi];
        if (!v.active || v.strength <= 0.05f) continue;
        float force = g_centripetalForce * v.strength;
        float influenceRadius = v.radius * 4.0f;
        float decaying = v.isCircling ? 0.0f : 1.0f;
        float invCore = 1.0f / fmaxf(v.coreRadius, 1.0f);
        float circOver2Pi = v.circulation / 6.28318f;
        int rotDir = v.angularVel >= 0 ? 1 : -1;
        float reactionX = 0, reactionY = 0;
        int affectedCount = 0;
        for (auto &p : g_particles) {
            float dx = v.centerX - p.x;
            float dy = v.centerY - p.y;
            float dist = sqrtf(dx * dx + dy * dy);
            if (dist > 2.0f && dist < influenceRadius) {
                float nx = dx / dist, ny = dy / dist;
                float tx = ny * rotDir, ty = -nx * rotDir;
                float radialVel = p.vx * nx + p.vy * ny;
                float tangentVel = p.vx * tx + p.vy * ty;
                float forceOnParticleX = 0, forceOnParticleY = 0;
                float targetTangentVel;
                switch (g_vortexPhysModel) {
                    case 1: {
                        targetTangentVel = circOver2Pi / dist;
                        break;
                    }
                    case 2: {
                        targetTangentVel = v.angularVel * dist;
                        break;
                    }
                    case 3: {
                        float gauss = 1.0f - expf(-(dist * dist) / (2.0f * v.coreRadius * v.coreRadius));
                        targetTangentVel = (circOver2Pi / dist) * gauss;
                        break;
                    }
                    case 4: {
                        float GM = v.angularVel * v.angularVel * v.radius * v.radius * v.radius;
                        targetTangentVel = sqrtf(fmaxf(GM / fmaxf(dist, 5.0f), 0.5f));
                        break;
                    }
                    case 0:
                    default: {
                        if (dist <= v.coreRadius) {
                            targetTangentVel = v.angularVel * dist;
                        } else {
                            targetTangentVel = circOver2Pi / dist;
                        }
                        break;
                    }
                }
                float pressureForce;
                switch (g_vortexPhysModel) {
                    case 1:
                        pressureForce = force * v.pressureGradient * (v.coreRadius * v.coreRadius) / (dist * dist);
                        break;
                    case 2:
                        pressureForce = force * v.pressureGradient * (dist / v.radius);
                        break;
                    case 4:
                        pressureForce = force * v.pressureGradient * (v.radius * v.radius) / (dist * dist);
                        break;
                    case 3:
                    case 0:
                    default:
                        if (dist <= v.coreRadius) {
                            pressureForce = force * v.pressureGradient * (dist * invCore);
                        } else {
                            pressureForce = force * v.pressureGradient * (v.coreRadius / dist);
                        }
                        break;
                }
                float effectivePressure = pressureForce * (1.0f - decaying * 0.85f);
                forceOnParticleX += nx * effectivePressure;
                forceOnParticleY += ny * effectivePressure;
                p.vx += (nx * effectivePressure) * g_dtScale;
                p.vy += (ny * effectivePressure) * g_dtScale;
                if (decaying > 0.5f) {
                    float centrifugal = force * 3.5f * (1.0f - dist / influenceRadius * 0.5f);
                    forceOnParticleX -= nx * centrifugal;
                    forceOnParticleY -= ny * centrifugal;
                    p.vx -= nx * centrifugal;
                    p.vy -= ny * centrifugal;
                }
                if (v.isCircling) {
                    float tangentError = targetTangentVel - tangentVel;
                    float viscosity;
                    switch (g_vortexPhysModel) {
                        case 1: viscosity = 0.02f; break;
                        case 2: viscosity = 0.2f; break;
                        case 3: viscosity = 0.04f + 0.1f * expf(-(dist * dist) / (2.0f * v.coreRadius * v.coreRadius)); break;
                        case 4: viscosity = 0.08f; break;
                        case 0: default: viscosity = dist <= v.coreRadius ? 0.15f : 0.04f; break;
                    }
                    float tangentForce = tangentError * viscosity * force;
                    forceOnParticleX += tx * tangentForce;
                    forceOnParticleY += ty * tangentForce;
                    p.vx += (tx * tangentForce) * g_dtScale;
                    p.vy += (ty * tangentForce) * g_dtScale;
                    float radialDamping = -radialVel * 0.04f * force;
                    forceOnParticleX += nx * radialDamping;
                    forceOnParticleY += ny * radialDamping;
                    p.vx += (nx * radialDamping) * g_dtScale;
                    p.vy += (ny * radialDamping) * g_dtScale;
                    float massOrbitFactor = g_enableParticleMass ? powf(p.mass, 0.333f) : 1.0f;
                    float targetRadius;
                    switch (g_vortexPhysModel) {
                        case 1: targetRadius = v.radius * 0.8f * massOrbitFactor; break;
                        case 2: targetRadius = v.radius * 0.7f * massOrbitFactor; break;
                        case 3: targetRadius = v.coreRadius * 1.5f * massOrbitFactor; break;
                        case 4: targetRadius = v.radius * 0.6f * massOrbitFactor; break;
                        case 0: default: targetRadius = v.coreRadius * 2.0f * massOrbitFactor; break;
                    }
                    float radiusError = dist - targetRadius;
                    if (fabsf(radiusError) > 2.0f) {
                        float springForce = radiusError * 0.006f * force;
                        springForce -= radialVel * 0.015f * force;
                        forceOnParticleX += nx * springForce;
                        forceOnParticleY += ny * springForce;
                        p.vx += (nx * springForce) * g_dtScale;
                        p.vy += (ny * springForce) * g_dtScale;
                    }
                    float orbitAngle = atan2f(p.y - v.centerY, p.x - v.centerX);
                    float precession = 0.0015f * force * sinf(orbitAngle * 2.0f);
                    forceOnParticleX += tx * precession;
                    forceOnParticleY += ty * precession;
                    p.vx += (tx * precession) * g_dtScale;
                    p.vy += (ty * precession) * g_dtScale;
                }
                if (v.isCircling) {
                    float orbitAngle = atan2f(p.y - v.centerY, p.x - v.centerX);
                    float depthFactor = sinf(orbitAngle) * sinf(v.orbitTilt);
                    float targetZ = depthFactor * 0.45f;
                    p.z += (targetZ - p.z) * 0.15f;
                    float speed = sqrtf(p.vx * p.vx + p.vy * p.vy);
                    float energyBoost = fminf(speed / 12.0f, 1.0f);
                    float coreBoost = dist <= v.coreRadius ? 0.15f : 0.0f;
                    float depthBrightness = (1.0f - p.z) * 0.2f;
                    p.vortexBrightness[0] = fmaxf(p.vortexBrightness[0], energyBoost * 0.1f + depthBrightness + coreBoost);
                    p.vortexBrightness[1] = fmaxf(p.vortexBrightness[1], energyBoost * 0.1f + depthBrightness + coreBoost);
                    p.vortexBrightness[2] = fmaxf(p.vortexBrightness[2], energyBoost * 0.1f + depthBrightness + coreBoost);
                } else {
                    p.z += (0.0f - p.z) * 0.1f;
                    p.vortexBrightness[0] *= 0.9f;
                    p.vortexBrightness[1] *= 0.9f;
                    p.vortexBrightness[2] *= 0.9f;
                }
                float particleMass = g_enableParticleMass ? p.mass : 1.0f;
                reactionX -= forceOnParticleX * particleMass * 0.02f;
                reactionY -= forceOnParticleY * particleMass * 0.02f;
                reactionX += p.vx * particleMass * 0.0005f;
                reactionY += p.vy * particleMass * 0.0005f;
                affectedCount++;
            }
        }
        if (affectedCount > 0 && g_vortexDrift > 0.01f) {
            float countFactor = sqrtf((float)affectedCount) * 0.3f;
            float reactionScale = g_vortexDrift * countFactor;
            v.driftX += reactionX * reactionScale;
            v.driftY += reactionY * reactionScale;
            float driftSpeed = sqrtf(v.driftX * v.driftX + v.driftY * v.driftY);
            float maxDrift = 5.0f * g_vortexDrift;
            if (driftSpeed > maxDrift) {
                v.driftX = v.driftX / driftSpeed * maxDrift;
                v.driftY = v.driftY / driftSpeed * maxDrift;
            }
        }
    }
}
struct SoGravitySource {
    float x, y, mass;
};
static inline void GravityAccumulate(const SoGravitySource &s, float px, float py,
                                     float G, float softeningSq, float maxDistSq,
                                     float &ax, float &ay) {
    float dx = s.x - px;
    float dy = s.y - py;
    float distSq = dx * dx + dy * dy + softeningSq;
    if (distSq > maxDistSq) return;
    float dist = sqrtf(distSq);
    float accel = G * s.mass / distSq;
    ax += (dx / dist) * accel;
    ay += (dy / dist) * accel;
}
static float HashNoise(int x, int y, int seed) {
    uint32_t n = (uint32_t)x * 374761393u + (uint32_t)y * 668265263u + (uint32_t)seed * 2246822519u;
    n = (n ^ (n >> 13)) * 1274126177u;
    return ((n & 1023u) / 1023.0f) * 2.0f - 1.0f;
}
static float CoherentNoise(float x, float y, float t, int seed) {
    int xi = (int)floorf(x), yi = (int)floorf(y);
    float xf = x - xi, yf = y - yi;
    float u = xf * xf * (3.0f - 2.0f * xf);
    float v = yf * yf * (3.0f - 2.0f * yf);
    int ti = (int)(t * 10.0f);
    float a = HashNoise(xi, yi, seed + ti);
    float b = HashNoise(xi + 1, yi, seed + ti);
    float c = HashNoise(xi, yi + 1, seed + ti);
    float d = HashNoise(xi + 1, yi + 1, seed + ti);
    return (a * (1 - u) + b * u) * (1 - v) + (c * (1 - u) + d * u) * v;
}
static void PollHotkeys() {
    int vks[3] = { g_hotkeyToggleVK, g_hotkeyTrailVK, g_hotkeyParticlesVK };
    if (!vks[0] && !vks[1] && !vks[2]) return;
    bool altDown = ((GetAsyncKeyState(VK_MENU) | GetAsyncKeyState(VK_LMENU) | GetAsyncKeyState(VK_RMENU)) & 0x8000) != 0;
    bool ctrlDown = ((GetAsyncKeyState(VK_CONTROL) | GetAsyncKeyState(VK_LCONTROL) | GetAsyncKeyState(VK_RCONTROL)) & 0x8000) != 0;
    bool shiftDown = ((GetAsyncKeyState(VK_SHIFT) | GetAsyncKeyState(VK_LSHIFT) | GetAsyncKeyState(VK_RSHIFT)) & 0x8000) != 0;
    bool wantAlt = (g_hotkeyModifier == 1 || g_hotkeyModifier == 2 || g_hotkeyModifier == 4);
    bool wantCtrl = (g_hotkeyModifier == 2 || g_hotkeyModifier == 4);
    bool wantShift = (g_hotkeyModifier == 3 || g_hotkeyModifier == 4);
    bool modOK = (altDown == wantAlt) && (ctrlDown == wantCtrl) && (shiftDown == wantShift);
    bool downNow[3] = { false, false, false };
    bool freshBit[3] = { false, false, false };
    for (int i = 0; i < 3; i++) {
        if (!vks[i]) continue;
        SHORT st = GetAsyncKeyState(vks[i]);
        downNow[i] = (st & 0x8000) != 0;
        freshBit[i] = (st & 0x0001) != 0;
    }
    static bool s_prevModOK = false;
    bool comboOK = modOK || s_prevModOK;
    s_prevModOK = modOK;
    static bool s_prevDown[3] = { false, false, false };
    for (int i = 0; i < 3; i++) {
        if (!vks[i]) { s_prevDown[i] = false; continue; }
        bool edge = downNow[i] && !s_prevDown[i];
        s_prevDown[i] = downNow[i];
        bool pressed = edge || freshBit[i];
        if (!pressed) continue;
        if (!comboOK) continue;
        if (g_hotkeyOSOwned[i].load()) continue;
        HotkeyTryAct(i, L"poll");
    }
}
typedef HRESULT(WINAPI *GetDpiForMonitor_t)(HMONITOR, int, UINT *, UINT *);
typedef UINT(WINAPI *GetDpiForWindow_t)(HWND);
static void UpdateMonitorDpiScale() {
    static GetDpiForMonitor_t s_pGetDpiForMonitor = nullptr;
    static GetDpiForWindow_t s_pGetDpiForWindow = nullptr;
    static HMODULE s_hShcore = nullptr;
    static bool s_resolved = false;
    if (!s_resolved) {
        s_resolved = true;
        s_hShcore = LoadLibraryW(L"shcore.dll");
        if (s_hShcore)
            s_pGetDpiForMonitor = (GetDpiForMonitor_t)GetProcAddress(s_hShcore, "GetDpiForMonitor");
        HMODULE hUser32 = GetModuleHandleW(L"user32.dll");
        if (hUser32)
            s_pGetDpiForWindow = (GetDpiForWindow_t)GetProcAddress(hUser32, "GetDpiForWindow");
        Wh_Log(L"[DPI] resolve: shcore=%d GetDpiForMonitor=%d GetDpiForWindow=%d", s_hShcore ? 1 : 0,
               s_pGetDpiForMonitor ? 1 : 0, s_pGetDpiForWindow ? 1 : 0);
    }
    UINT dpi = 0;
    POINT pt;
    GetCursorPos(&pt);
    HMONITOR hMon = MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);
    if (s_pGetDpiForMonitor && hMon) {
        UINT dx = 0, dy = 0;
        if (SUCCEEDED(s_pGetDpiForMonitor(hMon, 0, &dx, &dy)) && dx > 0) dpi = dx;
    }
    if (dpi == 0 && s_pGetDpiForWindow && g_overlayHwnd)
        dpi = s_pGetDpiForWindow(g_overlayHwnd);
    if (dpi == 0) dpi = 96;
    float scale = (g_dpiScaleMode == 1) ? 1.0f : (float)dpi / 96.0f;
    scale = ClampF(scale, 0.5f, 4.0f);
    float oldScale = g_monitorDpiScale.load();
    if (fabsf(scale - oldScale) > 0.001f) {
        g_monitorDpiScale.store(scale);
        g_charAtlasDirty.store(true);
        Wh_Log(L"[DPI] monitor dpi=%u scale=%.3f (mode=%d scaleText=%d)", dpi, scale, g_dpiScaleMode,
               g_dpiScaleText ? 1 : 0);
    }
}
static void UpdateMonitorDpiScaleIfMoved(POINT pt) {
    static HMONITOR s_lastMon = nullptr;
    HMONITOR hMon = MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);
    if (hMon != s_lastMon) {
        s_lastMon = hMon;
        UpdateMonitorDpiScale();
    }
}
static bool s_overlayShown = false;
static void SetOverlayVisible(bool shown) {
    if (s_overlayShown == shown) return;
    PostMessageW(g_overlayHwnd, shown ? WM_APP_SHOW_OVERLAY : WM_APP_HIDE_OVERLAY, 0, 0);
    s_overlayShown = shown;
}
static void RenderFrame() {
    if (g_settingsDirty.exchange(false)) {
        LoadSettings();
    }
    PollHotkeys();
    DWORD dwTime = GetTickCount();
    static DWORD lastFsCheck = 0;
    static bool isGameCached = false, trailActive = false;
    static int idleFrameCount = 0, fadeoutFrame = 0;
    static bool surfaceDirty = false;
    static bool gameHidden = false;
    static int hideDelayCounter = 0;
    if (g_desktopOnly && g_desktopLayer == 0) {
        if (!DesktopActiveNow(dwTime)) {
            if (!g_desktopSuppressed) {
                g_desktopSuppressed = true;
                SetOverlayVisible(false);
                g_history.clear();
                g_particles.clear();
                g_ripples.clear();
                g_trailShapes.clear();
                g_trailHistory.clear();
                g_fadeAlpha = 1.0f;
                trailActive = false;
                idleFrameCount = 0;
                fadeoutFrame = 0;
                surfaceDirty = false;
                g_lagInited = false;
                g_hasLastParticlePos = false;
                g_hasLastShapePos = false;
                lastFsCheck = dwTime;
                g_prevLButton = (GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0;
                g_prevRButton = (GetAsyncKeyState(VK_RBUTTON) & 0x8000) != 0;
            }
            return;
        }
    }
    if (g_desktopSuppressed) {
        g_desktopSuppressed = false;
        GetCursorPos(&g_lastPos);
        g_prevVelocity = 0.0f;
        if (g_desktopOnly && g_desktopLayer != 0) SetOverlayVisible(true);
    }
    bool wantTopmost = !(g_desktopOnly && g_desktopLayer == 2);
    if (g_overlayTopmost.load() != wantTopmost) {
        g_overlayTopmost.store(wantTopmost);
        PostMessageW(g_overlayHwnd, WM_APP_OVERLAY_LAYER, wantTopmost ? 1 : 0, 0);
    }
    {
        DWORD dtMs = dwTime - g_lastPhysicsTime;
        g_lastPhysicsTime = dwTime;
        if (dtMs == 0 || dtMs > 200) g_dtScale = 1.0f;
        else g_dtScale = fminf(fmaxf((float)dtMs / (1000.0f / 60.0f), 0.25f), 4.0f);
    }
    POINT pt;
    GetCursorPos(&pt);
    UpdateMonitorDpiScaleIfMoved(pt);
    int dx = pt.x - g_lastPos.x, dy = pt.y - g_lastPos.y;
    float velocity = sqrtf((float)(dx * dx + dy * dy));
    g_lastPos = pt;
    POINT renderPos = pt;
    if (g_trailDelay > 0) {
        if (!g_lagInited) {
            g_lagFX = (float)pt.x; g_lagFY = (float)pt.y;
            g_lagLastT = dwTime;
            g_lagInited = true;
        }
        float dtSec = (float)(dwTime - g_lagLastT) * 0.001f;
        g_lagLastT = dwTime;
        dtSec = ClampF(dtSec, 0.0f, 0.25f);
        float tau = 0.025f * (float)g_trailDelay;
        float a = 1.0f - expf(-dtSec / fmaxf(tau, 1e-4f));
        g_lagFX += ((float)pt.x - g_lagFX) * a;
        g_lagFY += ((float)pt.y - g_lagFY) * a;
        if (fabsf((float)pt.x - g_lagFX) < 0.5f) g_lagFX = (float)pt.x;
        if (fabsf((float)pt.y - g_lagFY) < 0.5f) g_lagFY = (float)pt.y;
        g_lagPos.x = (int)lroundf(g_lagFX);
        g_lagPos.y = (int)lroundf(g_lagFY);
        renderPos = g_lagPos;
    } else {
        g_lagInited = false;
    }
    GradData cols;
    ComputeColors(g_colorMode, dwTime, velocity, cols);
    float widthMul = 1.0f;
    if (g_enableSpeedResponse) {
        float sf = fminf(velocity / 80.0f, 1.0f);
        float accel = fabsf(velocity - g_prevVelocity);
        float af = fminf(accel / 30.0f, 1.0f);
        widthMul = 1.0f + sf * 0.3f + af * 0.15f;
    }
    UpdateFxAnimations(dwTime);
    int vX, vY, vW, vH;
    LoadVirtualScreen(vX, vY, vW, vH);
    bool effectsOn = g_effectsVisible.load(std::memory_order_relaxed);
    bool trailOn = effectsOn && g_trailVisible.load(std::memory_order_relaxed);
    bool particlesOn = effectsOn && g_particlesVisible.load(std::memory_order_relaxed);
    if (dwTime - lastFsCheck > 500) {
        isGameCached = CheckForegroundFullscreen();
        lastFsCheck = dwTime;
    }
    bool lDown = (GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0;
    bool rDown = (GetAsyncKeyState(VK_RBUTTON) & 0x8000) != 0;
    if (!isGameCached) {
        if (effectsOn && g_enableClickEffect) {
            if (lDown && !g_prevLButton) {
                g_ripples.push_back({pt, dwTime});
                g_ripples.push_back({pt, dwTime + 120});
            }
            if (rDown && !g_prevRButton) {
                g_ripples.push_back({pt, dwTime});
                g_ripples.push_back({pt, dwTime + 120});
            }
        }
        if (particlesOn && g_enableClickStarburst) {
            if ((lDown && !g_prevLButton) || (rDown && !g_prevRButton)) {
                SpawnParticles((float)(pt.x - vX), (float)(pt.y - vY), g_starburstCount, 2.5f, 5.5f, 2.5f, 5.0f, 250, 450,
                               cols.solidOuter, dwTime, true);
            }
        }
    }
    g_prevLButton = lDown;
    g_prevRButton = rDown;
    PruneExpired(g_ripples, [&](const Ripple &r) { return dwTime - r.startTime > (DWORD)g_clickDuration; });
    if (isGameCached) {
        if (!gameHidden) {
            gameHidden = true;
            SetOverlayVisible(false);
        }
        if (trailActive || !g_history.empty() || !g_ripples.empty() || !g_particles.empty() || surfaceDirty) {
            trailActive = false;
            g_history.clear();
            g_ripples.clear();
            g_particles.clear();
            g_fadeAlpha = 1.0f;
            fadeoutFrame = 0;
        } else {
            g_prevVelocity = velocity;
            return;
        }
    } else {
        gameHidden = false;
        static float accumulatedDist = 0.0f;
        static float lastVel = 0.0f;
        static int triggerConfirm = 0;
        static float lastTriggerPosX = 0, lastTriggerPosY = 0;
        float accel = velocity - lastVel;
        lastVel = velocity;
        if (!trailActive) {
            bool speedOK = (velocity > g_triggerVelocity);
            bool accelOK = (accel > 8.0f && velocity > g_triggerVelocity * 0.6f);
            if (speedOK || accelOK) {
                triggerConfirm++;
                if (triggerConfirm >= 2) {
                    trailActive = true;
                    idleFrameCount = 0;
                    accumulatedDist = 0.0f;
                    triggerConfirm = 0;
                    lastTriggerPosX = renderPos.x;
                    lastTriggerPosY = renderPos.y;
                }
            } else {
                triggerConfirm = 0;
            }
        } else {
            if (velocity < g_stopVelocity) {
                idleFrameCount++;
                float movedDist = sqrtf((renderPos.x - lastTriggerPosX) * (renderPos.x - lastTriggerPosX) +
                                        (renderPos.y - lastTriggerPosY) * (renderPos.y - lastTriggerPosY));
                lastTriggerPosX = renderPos.x;
                lastTriggerPosY = renderPos.y;
                accumulatedDist += movedDist;
                if (idleFrameCount > 4 || (idleFrameCount > 1 && accumulatedDist < 1.5f)) {
                    trailActive = false;
                    g_hasLastShapePos = false;
                    accumulatedDist = 0.0f;
                }
            } else {
                idleFrameCount = 0;
                accumulatedDist = 0.0f;
                lastTriggerPosX = renderPos.x;
                lastTriggerPosY = renderPos.y;
            }
        }
        if (trailActive) {
            POINT np = {renderPos.x - vX, renderPos.y - vY};
            g_history.push_front(np);
            while (g_history.size() > (size_t)g_tailLength)
                g_history.pop_back();
            fadeoutFrame = 0;
            g_fadeAlpha += (1.0f - g_fadeAlpha) * 0.4f;
            if (g_fadeAlpha > 1.0f)
                g_fadeAlpha = 1.0f;
        } else {
            switch (g_fadeoutMode) {
                case 0:
                    g_history.clear();
                    g_particles.clear();
                    g_trailShapes.clear();
                    g_trailHistory.clear();
                    g_hasLastParticlePos = false;
                    g_lastParticleDirX = 0.0f;
                    g_lastParticleDirY = 0.0f;
                    g_fadeAlpha = 1.0f;
                    fadeoutFrame = 0;
                    break;
                case 1: {
                    fadeoutFrame++;
                    int popCount = 1 + fadeoutFrame / 5;
                    if (popCount > 4)
                        popCount = 4;
                    for (int i = 0; i < popCount && !g_history.empty(); i++)
                        g_history.pop_back();
                    if (g_history.size() <= 1)
                        g_history.clear();
                    g_fadeAlpha = 1.0f;
                    break;
                }
                case 2:
                    fadeoutFrame++;
                    if (!g_history.empty())
                        g_history.pop_back();
                    if (g_history.size() >= 2) {
                        float lenRatio = (float)g_history.size() / (float)g_tailLength;
                        g_fadeAlpha = powf(lenRatio, 1.4f);
                    } else {
                        g_fadeAlpha = 0;
                        g_history.clear();
                    }
                    break;
            }
        }
    }
    static std::vector<D2D1_POINT_2F> s_smoothed, s_bezierOut, s_ns;
    std::vector<D2D1_POINT_2F> &smoothed = s_smoothed;
    smoothed.clear();
    smoothed.reserve(g_tailLength * 4);
    bool havePath = (g_history.size() >= 2);
    if (havePath) {
        for (auto &p : g_history)
            smoothed.push_back(D2D1::Point2F((float)p.x + g_tailOffsetX, (float)p.y + g_tailOffsetY));
        if (g_enableBezierSmooth) {
            std::vector<D2D1_POINT_2F> &bezierOut = s_bezierOut;
            bezierOut.clear();
            bezierOut.reserve(smoothed.size() * 4);
            CatmullRomSmooth(smoothed, bezierOut, 4);
            smoothed.swap(bezierOut);
        } else {
            std::vector<D2D1_POINT_2F> &ns = s_ns;
            ns.clear();
            ns.reserve(smoothed.size() * 3);
            for (int iter = 0; iter < 2; ++iter) {
                if (smoothed.size() < 3)
                    break;
                ns.clear();
                ns.push_back(smoothed.front());
                for (size_t i = 0; i < smoothed.size() - 1; ++i) {
                    D2D1_POINT_2F p0 = smoothed[i], p1 = smoothed[i + 1];
                    ns.push_back(D2D1::Point2F(.75f * p0.x + .25f * p1.x, .75f * p0.y + .25f * p1.y));
                    ns.push_back(D2D1::Point2F(.25f * p0.x + .75f * p1.x, .25f * p0.y + .75f * p1.y));
                }
                ns.push_back(smoothed.back());
                smoothed.swap(ns);
            }
        }
        if (g_trailShape == 2)
            ApplyFunctionDeformation(smoothed, dwTime);
        else if (g_trailShape == 3)
            ApplyWaveDeformation(smoothed, dwTime);
        else if (g_trailShape == 7)
            ApplySpiralDeformation(smoothed, dwTime);
        else if (g_trailShape == 8)
            ApplyLightningDeformation(smoothed);
        else if (g_trailShape == 9)
            ApplyFeatherDeformation(smoothed, dwTime);
    }
    static std::vector<D2D1_POINT_2F> s_sampleScratch;
    s_sampleScratch = smoothed;
    EnterCriticalSection(&g_sampleCS);
    g_samplePath.swap(s_sampleScratch);
    g_sampleVX = vX; g_sampleVY = vY;
    g_sampleCursor = pt; g_sampleTime = dwTime;
    g_sampleHasPath = havePath;
    g_sampleColorMode = g_colorMode;
    g_sampleAdaptive = g_adaptiveContrast || (g_contrastTarget > 0);
    LeaveCriticalSection(&g_sampleCS);
    static bool s_lastTrailVisible = true;
    static bool s_lastParticlesVisible = true;
    if (!trailOn && s_lastTrailVisible) {
        g_trailHistory.clear();
        g_trailShapes.clear();
    }
    s_lastTrailVisible = trailOn;
    if (!particlesOn && s_lastParticlesVisible) {
        g_particles.clear();
    }
    s_lastParticlesVisible = particlesOn;
    if (trailOn && g_enableMotionBlur && havePath && g_trailShape == 0) {
        TrailFrame frame;
        frame.path = smoothed;
        frame.time = dwTime;
        g_trailHistory.push_back(frame);
        while ((int)g_trailHistory.size() > g_motionBlurStrength)
            g_trailHistory.erase(g_trailHistory.begin());
    } else if ((!g_enableMotionBlur || g_trailShape != 0) && !g_trailHistory.empty()) {
        g_trailHistory.clear();
    }
    bool trailFadingOut = !trailActive && g_fadeAlpha > 0.02f && !g_history.empty();
    float adaptiveInterval = (float)g_particleInterval;
    if (g_particleInterval > 0 && velocity > 2.0f) {
        float speedFactor = 1.0f + fminf(velocity * 0.018f, 2.5f);
        adaptiveInterval = (float)g_particleInterval / speedFactor;
    }
    adaptiveInterval *= (0.88f + Rand01() * 0.24f);
    if (adaptiveInterval < 0.0f) adaptiveInterval = 0.0f;
    bool distanceReady = false;
    if (g_hasLastParticlePos && g_particleInterval > 0) {
        float ddx = 0, ddy = 0;
        if (!smoothed.empty()) { ddx = smoothed[0].x - g_lastParticleX; ddy = smoothed[0].y - g_lastParticleY; }
        if (ddx*ddx + ddy*ddy > 1225.0f) distanceReady = true;
    }
    if (particlesOn && g_particleMode > 0 && havePath &&
        (dwTime - g_lastParticleTime >= (DWORD)adaptiveInterval || distanceReady)) {
        bool spawnOK = (g_particleMode == 1) ? trailFadingOut : trailActive;
        if (spawnOK) {
            float ratio;
            switch (g_particleOrigin) {
                case 0:
                    ratio = 0.0f;
                    break;
                case 1:
                    ratio = 0.5f;
                    break;
                case 3:
                    ratio = g_particleOriginRatio / 100.0f;
                    break;
                case 4:
                    ratio = Rand01();
                    break;
                default:
                    ratio = 1.0f;
                    break;
            }
            D2D1_POINT_2F origin = GetPointOnPath(smoothed, ratio);
            float speedMul = 1.0f;
            if (g_particleAccel) {
                float accel = velocity - g_prevVelocity;
                speedMul = 1.0f + fabsf(accel) * 0.07f;
                if (speedMul > 3.5f)
                    speedMul = 3.5f;
            }
            float avgParticleSize = 3.25f * (g_particleSizeMultiplier / 100.0f);
            float spawnStep = avgParticleSize * 2.0f;
            if (spawnStep < 1.0f) spawnStep = 1.0f;
            int interpCount = g_particleDensity;
            if (interpCount < 1) interpCount = 1;
            if (g_hasLastParticlePos) {
                float pdx = origin.x - g_lastParticleX;
                float pdy = origin.y - g_lastParticleY;
                float dist = sqrtf(pdx * pdx + pdy * pdy);
                DWORD timeSinceLast = dwTime - g_lastParticleTime;
                if (timeSinceLast > 800) {
                    g_hasLastParticlePos = false;
                    g_lastParticleDirX = 0.0f;
                    g_lastParticleDirY = 0.0f;
                } else if (dist > spawnStep &&
                           CanInsertFillers(dist, pdx, pdy, g_lastParticleDirX, g_lastParticleDirY,
                                            avgParticleSize)) {
                    int cap = GetParticleCap();
                    size_t usedNow = g_particles.size();
                    size_t budget = (usedNow + 16 >= (size_t)cap) ? 0 : ((size_t)cap - usedNow - 16);
                    int steps = (int)(dist / spawnStep);
                    steps = ClampInt(steps, 1, 80);
                    if ((size_t)steps > budget) steps = (int)budget;
                    if (steps < 1) steps = 1;
                    float stepLen = dist / (steps + 1);
                    const size_t kInterpBudget = 72;
                    float countScale = 1.0f;
                    if ((size_t)steps * (size_t)interpCount > kInterpBudget)
                        countScale = (float)kInterpBudget / (float)((size_t)steps * (size_t)interpCount);
                    float midX = g_lastParticleX + dx * 0.5f;
                    float midY = g_lastParticleY + dy * 0.5f;
                    float cxRaw = g_lastParticleX + g_lastParticleDirX * dist * 0.5f;
                    float cyRaw = g_lastParticleY + g_lastParticleDirY * dist * 0.5f;
                    float offX = cxRaw - midX, offY = cyRaw - midY;
                    float offLen = sqrtf(offX * offX + offY * offY);
                    float maxOff = dist * 0.25f;
                    if (offLen > maxOff && offLen > 1e-6f) {
                        float s = maxOff / offLen;
                        cxRaw = midX + offX * s;
                        cyRaw = midY + offY * s;
                    }
                    for (int i = 1; i <= steps; i++) {
                        float t = (float)i / (steps + 1);
                        float mt = 1.0f - t;
                        float bx = mt * mt * g_lastParticleX + 2.0f * mt * t * cxRaw + t * t * origin.x;
                        float by = mt * mt * g_lastParticleY + 2.0f * mt * t * cyRaw + t * t * origin.y;
                        float jitter = avgParticleSize * 0.7f + stepLen * 0.10f;
                        float ix = bx + (Rand01() - 0.5f) * jitter;
                        float iy = by + (Rand01() - 0.5f) * jitter;
                        int countFactor = (int)(interpCount * (0.4f + t * 0.6f) * countScale + 0.5f);
                        if (countFactor < 1) countFactor = 1;
                        int lifeFactor = (int)(300 + t * 400);
                        float spdMul = 1.0f + (1.0f - t) * 0.5f;
                        SpawnParticles(ix, iy, countFactor, 0.4f * speedMul * spdMul, 2.5f * speedMul * spdMul,
                                       2.0f, 4.5f, lifeFactor,
                                       lifeFactor + 200, cols.solidOuter, dwTime);
                    }
                }
                if (dist > 1e-3f) {
                    g_lastParticleDirX = dx / dist;
                    g_lastParticleDirY = dy / dist;
                }
            }
            SpawnParticles(origin.x, origin.y, g_particleDensity, 0.3f * speedMul, 2.0f * speedMul, 2.0f, 4.5f, 300,
                           700, cols.solidOuter, dwTime);
            g_lastParticleX = origin.x;
            g_lastParticleY = origin.y;
            g_hasLastParticlePos = true;
            g_lastParticleTime = dwTime;
        }
    }
    UpdateTrailFxIntensity(smoothed);
    if (trailOn && g_trailShape == 4 && havePath) {
        float curX = smoothed[0].x, curY = smoothed[0].y;
        if (!g_hasLastShapePos) {
            g_lastShapeX = curX;
            g_lastShapeY = curY;
            g_hasLastShapePos = true;
        }
        float sdx = curX - g_lastShapeX, sdy = curY - g_lastShapeY;
        float dist = sqrtf(sdx * sdx + sdy * sdy);
        if (dist >= (float)g_shapeInterval) {
            for (int i = 0; i < g_shapeCount; i++) {
                float sx = curX, sy = curY;
                if (g_shapeRandomOffset) {
                    sx += (Rand01() - 0.5f) * g_shapeSize * 1.5f;
                    sy += (Rand01() - 0.5f) * g_shapeSize * 1.5f;
                }
                int st = g_shapeType;
                if (st == 9) {
                    int r = (int)(Rand01() * 12.0f);
                    st = (r < 9) ? r : (r + 2);
                }
                const bool isCraft = (st == 11 || st == 12 || st == 13);
                float dirX, dirY;
                float dlen = sqrtf(sdx * sdx + sdy * sdy);
                if (dlen > 0.5f) {
                    dirX = sdx / dlen; dirY = sdy / dlen;
                    g_lastShapeDirX = dirX; g_lastShapeDirY = dirY;
                } else {
                    dirX = g_lastShapeDirX; dirY = g_lastShapeDirY;
                }
                float heading = atan2f(dirY, dirX);
                float angle = isCraft ? heading + (Rand01() - 0.5f) * 0.12f
                                      : Rand01() * 6.2831853f;
                float speed = isCraft ? (1.2f + Rand01() * 1.6f)
                                      : (0.5f + Rand01() * 1.5f);
                float rotSpeed = isCraft ? 0.0f : (Rand01() - 0.5f) * 0.1f;
                float startRot = isCraft ? angle : 0.0f;
                g_trailShapes.push_back({sx, sy, cosf(angle) * speed, sinf(angle) * speed,
                                         g_shapeSize, startRot, rotSpeed, st, dwTime,
                                         (float)g_shapeLifetime, cols.solidOuter});
            }
            g_lastShapeX = curX;
            g_lastShapeY = curY;
        }
    } else {
        g_hasLastShapePos = false;
    }
    int shapeCap = g_superPerformanceMode ? 400 : 150;
    if (g_trailShapes.size() > (size_t)shapeCap) {
        g_trailShapes.erase(g_trailShapes.begin(), g_trailShapes.begin() + (g_trailShapes.size() - shapeCap));
    }
    int particleCap = GetParticleCap();
    if (g_particles.size() > (size_t)particleCap) {
        g_particles.erase(g_particles.begin(), g_particles.begin() + (g_particles.size() - particleCap));
    }
    float prevFrameVelocity = g_prevVelocity;
    g_prevVelocity = velocity;
    if (g_enableMusicReactive || g_enableMusicPhysics) {
        MusicUpdate(dwTime);
    }
    if (g_debugForce) {
        for (auto &p : g_particles) { p.debugForceX = p.vx; p.debugForceY = p.vy; }
    }
    if (g_enableCentripetal) {
        VortexFrameContext vctx;
        vctx.dwTime = dwTime;
        vctx.vX = vX; vctx.vY = vY;
        vctx.velocity = velocity;
        vctx.accel = velocity - prevFrameVelocity;
        vctx.accelAbs = fabsf(vctx.accel);
        vctx.cursorX = (float)(renderPos.x - vX) + (float)g_tailOffsetX;
        vctx.cursorY = (float)(renderPos.y - vY) + (float)g_tailOffsetY;
        int activeVortexCount = 0;
        for (int i = 0; i < g_vortexMaxCount; i++) {
            if (g_vortices[i].active) activeVortexCount++;
        }
        bool detected = false; float detCX = 0, detCY = 0, detRadius = 0, detAngVel = 0;
        VortexDetectCurvedMotion(vctx, detected, detCX, detCY, detRadius, detAngVel);
        VortexUpdateOrCreate(vctx, detected, detCX, detCY, detRadius, detAngVel, activeVortexCount);
        VortexMusicInject(vctx, activeVortexCount);
        VortexUpdateDynamics(vctx);
        VortexApplyToParticles();
    } else {
        for (auto &p : g_particles) {
            p.vortexBrightness[0] *= 0.9f;
            p.vortexBrightness[1] *= 0.9f;
            p.vortexBrightness[2] *= 0.9f;
            p.z += (0.0f - p.z) * 0.1f;
        }
    }
    if ((g_enableParticleInteraction || g_enableParticleCollision || g_enableParticleSpring) && !g_particles.empty()) {
        int pcount = (int)g_particles.size();
        float repelDist = (float)g_interParticleRepelDistance;
            float pairRange = g_enableParticleSpring ? fmaxf(repelDist, g_springMaxDist) : repelDist;
            float pairRangeSq = pairRange * pairRange;
            float springRest = g_springMaxDist * 0.5f;
            float colSizeMul = g_enableParticleCollision ? (g_particleSizeMultiplier / 100.0f) : 0.0f;
            float colTextRadius = g_enableParticleCollision
                ? (g_textFontSize * 0.5f * colSizeMul * 0.5f)
                  * sqrtf(1.0f + g_textAspectRatio * g_textAspectRatio)
                : 0.0f;
            const int MAX_SPRING_LINKS = 5;
            float softEndDist = g_springMaxDist * 0.9f;
            float softRange = g_springMaxDist - softEndDist;
            if (g_enableParticleSpring) {
                g_springLinkCount.assign(pcount, 0);
            }
            const int kGridMinCount = 400;
            bool useGrid = g_superPerformanceMode && pcount >= kGridMinCount && pairRange > 1.0f;
            static std::vector<int> s_cellOf, s_cellStart, s_sorted, s_fill;
            int gw = 0, gh = 0;
            if (useGrid) {
                float minX = g_particles[0].x, maxX = minX, minY = g_particles[0].y, maxY = minY;
                for (int i = 1; i < pcount; i++) {
                    float x = g_particles[i].x, y = g_particles[i].y;
                    if (x < minX) minX = x;
                    if (x > maxX) maxX = x;
                    if (y < minY) minY = y;
                    if (y > maxY) maxY = y;
                }
                float invCell = 1.0f / pairRange;
                gw = (int)((maxX - minX) * invCell) + 1;
                gh = (int)((maxY - minY) * invCell) + 1;
                if (gw > 0 && gh > 0 && (long long)gw * gh <= 65536LL) {
                    int ncell = gw * gh;
                    s_cellOf.resize((size_t)pcount);
                    s_cellStart.assign((size_t)ncell + 1, 0);
                    for (int i = 0; i < pcount; i++) {
                        int cx = (int)((g_particles[i].x - minX) * invCell);
                        int cy = (int)((g_particles[i].y - minY) * invCell);
                        if (cx < 0) cx = 0;
                        if (cx >= gw) cx = gw - 1;
                        if (cy < 0) cy = 0;
                        if (cy >= gh) cy = gh - 1;
                        int c = cy * gw + cx;
                        s_cellOf[i] = c;
                        s_cellStart[c + 1]++;
                    }
                    for (int c = 0; c < ncell; c++) s_cellStart[c + 1] += s_cellStart[c];
                    s_sorted.resize((size_t)pcount);
                    s_fill.assign(s_cellStart.begin(), s_cellStart.end());
                    for (int i = 0; i < pcount; i++) s_sorted[(size_t)s_fill[(size_t)s_cellOf[i]]++] = i;
                } else {
                    useGrid = false;
                }
            }
            auto processPair = [&](int i, int j) {
                    float dx = g_particles[i].x - g_particles[j].x;
                    float dy = g_particles[i].y - g_particles[j].y;
                    float distSq = dx * dx + dy * dy;
                    if (distSq < pairRangeSq) {
                        float dist, nx, ny;
                        if (distSq > 0.01f) {
                            dist = sqrtf(distSq);
                            nx = dx / dist; ny = dy / dist;
                        } else {
                            float ra = Rand01() * 6.2831853f;
                            nx = cosf(ra); ny = sinf(ra);
                            dist = 0.1f;
                        }
                        float alignReduce = 1.0f;
                        if (g_particleRepelAlignment > 0) {
                            float viLen2 = g_particles[i].vx * g_particles[i].vx + g_particles[i].vy * g_particles[i].vy;
                            float vjLen2 = g_particles[j].vx * g_particles[j].vx + g_particles[j].vy * g_particles[j].vy;
                            if (viLen2 > 0.01f && vjLen2 > 0.01f) {
                                float viLen = sqrtf(viLen2), vjLen = sqrtf(vjLen2);
                                float dot = (g_particles[i].vx * g_particles[j].vx + g_particles[i].vy * g_particles[j].vy) / (viLen * vjLen);
                                if (dot > 0.0f) {
                                    alignReduce = 1.0f - dot * (g_particleRepelAlignment / 100.0f);
                                    if (alignReduce < 0.0f) alignReduce = 0.0f;
                                }
                            }
                        }
                        float effRepelDist = repelDist * alignReduce;
                        if (g_enableParticleInteraction && dist < effRepelDist) {
                            float falloff = 1.0f - dist / effRepelDist;
                            float massFactor = g_enableParticleMass ? sqrtf(sqrtf(g_particles[i].mass * g_particles[j].mass)) : 1.0f;
                            float force = falloff * falloff * g_interParticleRepelForce * massFactor * alignReduce;
                            if (force > 20.0f) force = 20.0f;
                            float invMassI = g_enableParticleMass ? (1.0f / g_particles[i].mass) : 1.0f;
                            float invMassJ = g_enableParticleMass ? (1.0f / g_particles[j].mass) : 1.0f;
                            g_particles[i].vx += nx * force * invMassI;
                            g_particles[i].vy += ny * force * invMassI;
                            g_particles[j].vx -= nx * force * invMassJ;
                            g_particles[j].vy -= ny * force * invMassJ;
                        }
                        if (g_enableParticleCollision) {
                            float radiusI = (g_particles[i].shapeType == 10)
                                ? colTextRadius
                                : g_particles[i].size * colSizeMul;
                            float radiusJ = (g_particles[j].shapeType == 10)
                                ? colTextRadius
                                : g_particles[j].size * colSizeMul;
                            float minDist = radiusI + radiusJ;
                            if (dist < minDist) {
                                float overlap = minDist - dist;
                                float mi = g_enableParticleMass ? g_particles[i].mass : 1.0f;
                                float mj = g_enableParticleMass ? g_particles[j].mass : 1.0f;
                                float totalMass = mi + mj;
                                float overlapI = overlap * (mj / totalMass);
                                float overlapJ = overlap * (mi / totalMass);
                                g_particles[i].x += nx * overlapI;
                                g_particles[i].y += ny * overlapI;
                                g_particles[j].x -= nx * overlapJ;
                                g_particles[j].y -= ny * overlapJ;
                                float dvx = g_particles[i].vx - g_particles[j].vx;
                                float dvy = g_particles[i].vy - g_particles[j].vy;
                                float vn = dvx * nx + dvy * ny;
                                if (vn < 0.0f) {
                                    if (g_enableCollisionSpark && vn < -3.0f
                                        && (int)(rand() % 100) < g_sparkChance) {
                                        SpawnCollisionSpark(
                                            (g_particles[i].x + g_particles[j].x) * 0.5f,
                                            (g_particles[i].y + g_particles[j].y) * 0.5f,
                                            nx, ny, -vn);
                                    }
                                    float impulse = (2.0f * vn) / totalMass;
                                    g_particles[i].vx -= impulse * mj * nx;
                                    g_particles[i].vy -= impulse * mj * ny;
                                    g_particles[j].vx += impulse * mi * nx;
                                    g_particles[j].vy += impulse * mi * ny;
                                }
                            }
                        }
                        if (g_enableBrownianMotion && dist < 30.0f) {
                            float viscosity = 0.003f * (1.0f - dist / 30.0f);
                            float dvx = (g_particles[i].vx - g_particles[j].vx) * viscosity;
                            float dvy = (g_particles[i].vy - g_particles[j].vy) * viscosity;
                            g_particles[i].vx -= dvx;
                            g_particles[i].vy -= dvy;
                            g_particles[j].vx += dvx;
                            g_particles[j].vy += dvy;
                        }
                        if (g_enableParticleSpring && dist < g_springMaxDist && dist > 0.5f
                            && g_springLinkCount[i] < MAX_SPRING_LINKS && g_springLinkCount[j] < MAX_SPRING_LINKS) {
                            float displacement = dist - springRest;
                            float fade = 1.0f;
                            if (dist > softEndDist) fade = (g_springMaxDist - dist) / softRange;
                            float springForce = g_springK * displacement * fade;
                            float dvx = g_particles[i].vx - g_particles[j].vx;
                            float dvy = g_particles[i].vy - g_particles[j].vy;
                            float vn = dvx * nx + dvy * ny;
                            float damper = g_springK * 14.0f * fade;
                            float fmag = springForce + damper * vn;
                            g_springLinkCount[i]++;
                            g_springLinkCount[j]++;
                            fmag = ClampF(fmag, -1.5f, 1.5f);
                            float imi = g_enableParticleMass ? (1.0f / g_particles[i].mass) : 1.0f;
                            float imj = g_enableParticleMass ? (1.0f / g_particles[j].mass) : 1.0f;
                            g_particles[i].vx -= nx * fmag * imi;
                            g_particles[i].vy -= ny * fmag * imi;
                            g_particles[j].vx += nx * fmag * imj;
                            g_particles[j].vy += ny * fmag * imj;
                        }
                    }
            };
            if (useGrid) {
                for (int i = 0; i < pcount; i++) {
                    int c = s_cellOf[(size_t)i];
                    int cx = c % gw, cy = c / gw;
                    int y0 = (cy > 0) ? cy - 1 : 0, y1 = (cy + 1 < gh) ? cy + 1 : gh - 1;
                    int x0 = (cx > 0) ? cx - 1 : 0, x1 = (cx + 1 < gw) ? cx + 1 : gw - 1;
                    for (int gy = y0; gy <= y1; gy++) {
                        int rowBase = gy * gw;
                        for (int gx = x0; gx <= x1; gx++) {
                            int nc = rowBase + gx;
                            int s0 = s_cellStart[(size_t)nc], s1 = s_cellStart[(size_t)nc + 1];
                            for (int k = s0; k < s1; k++) {
                                int j = s_sorted[(size_t)k];
                                if (j > i) processPair(i, j);
                            }
                        }
                    }
                }
            } else {
                for (int i = 0; i < pcount; i++)
                    for (int j = i + 1; j < pcount; j++) processPair(i, j);
            }
    }
    if (g_enableParticleGravity && g_particles.size() >= 2) {
        int pcount = (int)g_particles.size();
        int bodyCount = g_gravitySystem == 0 ? 2 : (int)fminf((float)g_gravityBodyCount, (float)pcount);
        if (bodyCount > pcount) bodyCount = pcount;
        if (bodyCount < 1) bodyCount = 1;
        static const int MAX_GRAVITY_SRC = 10;
        if (bodyCount > MAX_GRAVITY_SRC) bodyCount = MAX_GRAVITY_SRC;
        SoGravitySource srcBuf[MAX_GRAVITY_SRC];
        struct MassIdx { float mass; int idx; };
        MassIdx topK[MAX_GRAVITY_SRC];
        int topKCount = 0;
        for (int i = 0; i < pcount; i++) {
            float m = g_particles[i].mass;
            if (topKCount < bodyCount) {
                int pos = topKCount++;
                topK[pos].mass = m; topK[pos].idx = i;
                while (pos > 0 && topK[pos].mass > topK[pos - 1].mass) {
                    MassIdx tmp = topK[pos]; topK[pos] = topK[pos - 1]; topK[pos - 1] = tmp;
                    pos--;
                }
            } else if (m > topK[bodyCount - 1].mass) {
                int pos = bodyCount - 1;
                topK[pos].mass = m; topK[pos].idx = i;
                while (pos > 0 && topK[pos].mass > topK[pos - 1].mass) {
                    MassIdx tmp = topK[pos]; topK[pos] = topK[pos - 1]; topK[pos - 1] = tmp;
                    pos--;
                }
            }
        }
        g_gravityBodies.clear();
        int srcCount = 0;
        for (int k = 0; k < bodyCount; k++) {
            int idx = topK[k].idx;
            g_gravityBodies.push_back(idx);
            srcBuf[srcCount].x = g_particles[idx].x;
            srcBuf[srcCount].y = g_particles[idx].y;
            srcBuf[srcCount].mass = g_particles[idx].mass;
            srcCount++;
        }
        float G = g_gravityStrength * 50.0f;
        if (g_enableMusicPhysics && g_musicGravityLink > 0) {
            float volumeBoost = 1.0f + fminf(g_music.overallLevel * 20.0f, 2.0f) * g_musicGravityLink;
            G *= volumeBoost;
        }
        if (g_enableMultiBandLink) {
            G *= (1.0f + g_bassGravityBoost * 1.5f);
        }
        float softening = 8.0f;
        float softeningSq = softening * softening;
        float maxDist = 400.0f;
        float maxDistSq = maxDist * maxDist;
        float srcAx[MAX_GRAVITY_SRC] = {0}, srcAy[MAX_GRAVITY_SRC] = {0};
        for (int bi = 0; bi < srcCount; bi++) {
            for (int bj = bi + 1; bj < srcCount; bj++) {
                GravityAccumulate(srcBuf[bj], srcBuf[bi].x, srcBuf[bi].y, G, softeningSq, maxDistSq, srcAx[bi], srcAy[bi]);
                GravityAccumulate(srcBuf[bi], srcBuf[bj].x, srcBuf[bj].y, G, softeningSq, maxDistSq, srcAx[bj], srcAy[bj]);
            }
        }
        for (int bi = 0; bi < srcCount; bi++) {
            g_particles[g_gravityBodies[bi]].vx += srcAx[bi] * 0.5f;
            g_particles[g_gravityBodies[bi]].vy += srcAy[bi] * 0.5f;
        }
        g_gravityBodyFlag.assign(pcount, false);
        for (int bi = 0; bi < srcCount; bi++) {
            g_gravityBodyFlag[g_gravityBodies[bi]] = true;
        }
        for (int i = 0; i < pcount; i++) {
            if (g_gravityBodyFlag[i]) continue;
            float ax = 0, ay = 0;
            for (int bi = 0; bi < srcCount; bi++) {
                GravityAccumulate(srcBuf[bi], g_particles[i].x, g_particles[i].y, G, softeningSq, maxDistSq, ax, ay);
            }
            g_particles[i].vx += ax;
            g_particles[i].vy += ay;
        }
    }
    if (g_enableEnvWind) {
        float wt = dwTime * 0.001f;
        float gust = g_windStrength * (0.5f + 0.5f * sinf(wt * 0.9f));
        float sway = g_windTurbulence * 0.7f * sinf(wt * 0.4f + 1.3f);
        float jitter = g_windTurbulence * 0.25f * sinf(wt * 2.1f);
        g_windX = cosf(sway) * gust + jitter;
        g_windY = sinf(sway) * gust * 0.25f + g_windTurbulence * 0.15f * sinf(wt * 1.3f + 4.0f);
    } else {
        g_windX = 0; g_windY = 0;
    }
    float attractTargetX = (float)(renderPos.x - vX + g_tailOffsetX);
    float attractTargetY = (float)(renderPos.y - vY + g_tailOffsetY);
    const float kAttractRadius = 500.0f;
    const float kAttractRadiusSq = kAttractRadius * kAttractRadius;
    const float kInvAttractRadius = 1.0f / kAttractRadius;
    const float kAttractDeadZoneSq = 4.0f;
    const float kAttractVelGain = 0.06f;
    const int kTrailSampleStep = 3;
    g_sparkBudget = g_enableCollisionSpark ? (120 + g_sparkIntensity) : 0;
    const float MAX_SPEED = 25.0f * g_dtScale;
    const float MAX_SPEED_SQ = MAX_SPEED * MAX_SPEED;
    if (particlesOn)
    for (auto &p : g_particles) {
        float invMass = g_enableParticleMass ? (1.0f / p.mass) : 1.0f;
        float toCursorDx = p.x - attractTargetX;
        float toCursorDy = p.y - attractTargetY;
        float toCursorDistSq = toCursorDx * toCursorDx + toCursorDy * toCursorDy;
        float speedSq = p.vx * p.vx + p.vy * p.vy;
        float speed = sqrtf(speedSq);
        float dragCoeff = g_enableParticleMass ? (0.04f / fmaxf(p.mass, 0.2f)) : 0.05f;
        float linearDrag = 1.0f - dragCoeff;
        float quadraticDrag = speed > 10.0f ? (1.0f - dragCoeff * 0.5f * (speed - 10.0f) / 15.0f) : 1.0f;
        float totalDrag = fmaxf(fminf(linearDrag * quadraticDrag, 0.98f), 0.80f);
        float dragPow = powf(totalDrag, g_dtScale);
        p.vx *= dragPow;
        p.vy *= dragPow;
        if (g_enableEnvGravity) {
            p.vx += (cosf(g_envGravityAngle) * g_envGravityStrength) * g_dtScale;
            p.vy += (sinf(g_envGravityAngle) * g_envGravityStrength) * g_dtScale;
        }
        if (g_enableEnvWind) {
            p.vx += (g_windX * invMass) * g_dtScale;
            p.vy += (g_windY * invMass) * g_dtScale;
        }
        if (g_enableMusicPhysics && g_musicBeatPulseAmount > 0.01f && toCursorDistSq > 1.0f) {
            float pdist = sqrtf(toCursorDistSq);
            float pulseForce = g_musicBeatPulseAmount * 8.0f * invMass;
            p.vx += ((toCursorDx / pdist) * pulseForce) * g_dtScale;
            p.vy += ((toCursorDy / pdist) * pulseForce) * g_dtScale;
        }
        if (g_enableParticleRepel && g_particleRepelForce > 0 && toCursorDistSq < (float)g_particleRepelRadius * g_particleRepelRadius && toCursorDistSq > 0.25f) {
            float rdist = sqrtf(toCursorDistSq);
            float nx = toCursorDx / rdist, ny = toCursorDy / rdist;
            float falloff = 1.0f - rdist / (float)g_particleRepelRadius;
            float force = falloff * g_particleRepelForce;
            float accel = force * invMass;
            p.vx += (nx * accel) * g_dtScale;
            p.vy += (ny * accel) * g_dtScale;
            float perturb = force * 0.65f * invMass;
            float angle = Rand01() * 6.28318f;
            p.vx += (cosf(angle) * perturb) * g_dtScale;
            p.vy += (sinf(angle) * perturb) * g_dtScale;
        }
        if (g_enableLorentzForce) {
            float B = g_lorentzStrength * 0.5f;
            if (g_enableMultiBandLink) B *= (1.0f + g_midRepelBoost * 0.5f);
            float lx = -p.vy * B * p.charge;
            float ly = p.vx * B * p.charge;
            p.vx += (lx * invMass) * g_dtScale;
            p.vy += (ly * invMass) * g_dtScale;
        }
        if (g_enableBrownianMotion) {
            float noise = g_brownianStrength * 0.5f;
            if (g_enableMultiBandLink) noise *= (1.0f + g_trebleNoiseBoost * 2.0f);
            float nx = CoherentNoise(p.x * 0.02f, p.y * 0.02f, dwTime * 0.001f, 1);
            float ny = CoherentNoise(p.x * 0.02f + 100.0f, p.y * 0.02f + 100.0f, dwTime * 0.001f, 2);
            p.vx += (nx * noise * invMass) * g_dtScale;
            p.vy += (ny * noise * invMass) * g_dtScale;
        }
        if (g_enableLorentzForce && fabsf(p.charge) > 0.01f) {
            const float cpx = p.x, cpy = p.y, cpq = p.charge;
            for (auto &q : g_particles) {
                if (&q == &p) continue;
                float dx = q.x - cpx, dy = q.y - cpy;
                float dSq = dx*dx + dy*dy;
                if (dSq < 4.0f || dSq > 40000.0f) continue;
                float d = sqrtf(dSq);
                float invD = 1.0f / d;
                float kq = 800.0f * cpq * q.charge * invD * invD * invD * invMass;
                p.vx -= dx * kq;
                p.vy -= dy * kq;
            }
        }
        if (g_enableParticleMass && p.size > 2.0f) {
            float sizeDrag = 1.0f - fminf(p.size * 0.002f, 0.02f);
            float sizeDragPow = powf(sizeDrag, g_dtScale);
            p.vx *= sizeDragPow;
            p.vy *= sizeDragPow;
        }
        if (g_particleAttraction > 0.0f) {
            float nearestD2 = toCursorDistSq;
            int nearestIdx = -1;
            if (smoothed.size() >= 2) {
                for (size_t si = 0; si < smoothed.size(); si += kTrailSampleStep) {
                    float sdx = smoothed[si].x - p.x, sdy = smoothed[si].y - p.y;
                    float sd2 = sdx * sdx + sdy * sdy;
                    if (sd2 < nearestD2) { nearestD2 = sd2; nearestIdx = (int)si; }
                }
            }
            if (nearestD2 < kAttractRadiusSq) {
                float tgtX = attractTargetX, tgtY = attractTargetY;
                if (nearestIdx >= 0) {
                    int look = (int)(smoothed.size() / 5);
                    if (look < 4) look = 4;
                    int tgtIdx = nearestIdx - look;
                    if (tgtIdx < 0) tgtIdx = 0;
                    tgtX = smoothed[(size_t)tgtIdx].x;
                    tgtY = smoothed[(size_t)tgtIdx].y;
                }
                float ddx = tgtX - p.x, ddy = tgtY - p.y;
                float d2 = ddx * ddx + ddy * ddy;
                if (d2 > kAttractDeadZoneSq) {
                    float d = sqrtf(d2);
                    float falloff = 1.0f - sqrtf(nearestD2) * kInvAttractRadius;
                    float accel = g_particleAttraction * falloff * d * kAttractVelGain;
                    p.vx += ((ddx / d) * accel) * g_dtScale;
                    p.vy += ((ddy / d) * accel) * g_dtScale;
                }
            }
        }
        float finalSpeedSq = p.vx * p.vx + p.vy * p.vy;
        if (finalSpeedSq > MAX_SPEED_SQ) {
            float finalSpeed = sqrtf(finalSpeedSq);
            float scale = MAX_SPEED / finalSpeed;
            p.vx *= scale;
            p.vy *= scale;
        }
        p.x += p.vx;
        p.y += p.vy;
        if (g_debugForce) {
            float snapVx = p.debugForceX, snapVy = p.debugForceY;
            p.debugForceX = p.mass * (p.vx - snapVx);
            p.debugForceY = p.mass * (p.vy - snapVy);
        }
        if (g_enableParticleSpin) {
            p.rotation += p.spinSpeed;
            p.spinSpeed *= 0.995f;
        }
        if (p.shapeType == 10 && p.chunkCount > 1) {
            DWORD chunkBase = (g_textChunkMode == 1) ? g_globalChunkClock : p.startTime;
            DWORD chunkElapsed = dwTime - chunkBase;
            int chunkSlot = (int)(chunkElapsed / (DWORD)g_textChunkDelay) % p.chunkCount;
            if (chunkSlot < 0) chunkSlot = 0;
            p.charIndex = (float)(p.chunkStart + chunkSlot);
        }
        if (g_enableParticleMass && p.mass > 1.2f) {
            float massBrightness = (p.mass - 1.0f) * 0.08f;
            p.vortexBrightness[0] = fmaxf(p.vortexBrightness[0], massBrightness);
            p.vortexBrightness[1] = fmaxf(p.vortexBrightness[1], massBrightness);
            p.vortexBrightness[2] = fmaxf(p.vortexBrightness[2], massBrightness);
        }
    }
    PruneExpired(g_particles, [&](const Particle &p) { return dwTime - p.startTime > (DWORD)p.lifetime; });
    for (auto &s : g_trailShapes) {
        s.x += s.vx;
        s.y += s.vy;
        bool isCraft = (s.shapeType == 11 || s.shapeType == 12 || s.shapeType == 13);
        float drag = isCraft ? 0.995f : 0.96f;
        s.vx *= drag;
        s.vy *= drag;
        s.vy += isCraft ? 0.004f : 0.02f;
        s.rotation += s.rotSpeed;
        s.rotSpeed *= 0.98f;
    }
    if (g_missileIdleExplode && !g_trailShapes.empty()) {
        static DWORD craftIdleStart = 0;
        static bool craftWasIdle = false;
        bool idle = (velocity < g_stopVelocity);
        if (idle) {
            if (!craftWasIdle) { craftIdleStart = dwTime; craftWasIdle = true; }
        } else {
            craftWasIdle = false;
            craftIdleStart = 0;
        }
        if (idle && craftIdleStart != 0 && (DWORD)(dwTime - craftIdleStart) >= (DWORD)g_missileIdleDelay) {
            size_t w = 0;
            for (size_t i = 0; i < g_trailShapes.size(); i++) {
                TrailShape s = g_trailShapes[i];
                if (s.shapeType != 11 && s.shapeType != 12 && s.shapeType != 13) {
                    g_trailShapes[w++] = s;
                    continue;
                }
                if (g_missileExplodeFx != 1 && g_missileExplodeParticles > 0 && particlesOn) {
                    SpawnParticles(s.x, s.y, g_missileExplodeParticles, 2.0f, 6.0f, 2.0f, 5.0f,
                                   300, 650, s.color, dwTime);
                }
                if (g_missileExplodeFx != 0) {
                    POINT rpt = { (long)(s.x + vX), (long)(s.y + vY) };
                    g_ripples.push_back({rpt, dwTime});
                }
            }
            g_trailShapes.resize(w);
            craftIdleStart = 0;
            craftWasIdle = false;
        }
    }
    PruneExpired(g_trailShapes, [&](const TrailShape &s) { return dwTime - s.startTime > (DWORD)s.lifetime; });
    bool tailVisible = trailOn && (trailActive || g_history.size() >= 2) && g_fadeAlpha > 0.02f;
    bool isDrawing = tailVisible || !g_ripples.empty() || !g_particles.empty() || !g_trailShapes.empty();
    static bool s_lastEffectsVisible = true;
    if (!effectsOn) {
        if (s_lastEffectsVisible) {
            g_history.clear();
            g_particles.clear();
            g_ripples.clear();
            g_trailShapes.clear();
            g_trailHistory.clear();
            g_fadeAlpha = 1.0f;
            fadeoutFrame = 0;
            trailActive = false;
            SetOverlayVisible(false);
            s_lastEffectsVisible = false;
        }
        isDrawing = false;
    } else {
        s_lastEffectsVisible = true;
    }
    if (isDrawing) {
        hideDelayCounter = 0;
        SetOverlayVisible(true);
    } else if (!surfaceDirty) {
        hideDelayCounter++;
        if (hideDelayCounter >= 3)
            SetOverlayVisible(false);
    } else {
        hideDelayCounter = 0;
    }
    if (!isDrawing && !surfaceDirty)
        return;
    if (!g_pSwapChain || !g_pD2DTargetBitmap || g_cachedVW != vW || g_cachedVH != vH
        || g_createdMsaaSamples != g_msaaSamples || g_createdSsaaScale != g_ssaaScale) {
        RecreateSwapChain(vW, vH);
    }
    if (!g_pD2DDC || !g_pD2DTargetBitmap)
        return;
    bool useNative = (g_pNativeVS != nullptr);
    if (useNative) {
        bool nativeOK = NativeRenderFrame(vW, vH, smoothed, tailVisible, cols, widthMul, g_fadeAlpha, dwTime, vX, vY);
        if (nativeOK) {
            {
                ID3D11Texture2D *pBackBuffer = nullptr;
                ID3D11RenderTargetView *pSwapRTV = nullptr;
                HRESULT hrBuf = g_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&pBackBuffer);
                if (SUCCEEDED(hrBuf) && g_msaaSamples > 1 && g_pMSAATexture && g_pSSAAResolveTexture) {
                    g_pD3DContext->ResolveSubresource(g_pSSAAResolveTexture, 0, g_pMSAATexture, 0, g_swapChainFormat);
                }
                bool postWanted = (NeedPostChain() || NeedHDREncode());
                bool postActive = false;
                if (postWanted) {
                    postActive = (EnsurePostTargets(vW, vH) && g_pSceneRTV != nullptr);
                } else {
                    EnsurePostTargets(vW, vH);
                }
                if ((g_msaaSamples > 1 && g_pSSAAResolveSRV) || (g_ssaaScale > 1 && g_pSSAASRV)) {
                    g_pD3DDevice->CreateRenderTargetView(pBackBuffer, nullptr, &pSwapRTV);
                    if (!pSwapRTV) { if (pBackBuffer) pBackBuffer->Release(); pBackBuffer = nullptr; goto present_skip_blit; }
                    ID3D11ShaderResourceView *pBlitSRV = (g_msaaSamples > 1) ? g_pSSAAResolveSRV : g_pSSAASRV;
                    ID3D11RenderTargetView *pBlitDst = postActive ? g_pSceneRTV : pSwapRTV;
                    ID3D11RenderTargetView *pOldRTV = nullptr;
                    ID3D11ShaderResourceView *pOldSRV = nullptr;
                    ID3D11VertexShader *pOldVS = nullptr;
                    ID3D11PixelShader *pOldPS = nullptr;
                    ID3D11Buffer *pOldVB = nullptr;
                    UINT oldStride = 0, oldOffset = 0;
                    D3D11_VIEWPORT oldVP; UINT numVP = 1;
                    g_pD3DContext->OMGetRenderTargets(1, &pOldRTV, nullptr);
                    g_pD3DContext->PSGetShaderResources(0, 1, &pOldSRV);
                    g_pD3DContext->VSGetShader(&pOldVS, nullptr, nullptr);
                    g_pD3DContext->PSGetShader(&pOldPS, nullptr, nullptr);
                    g_pD3DContext->IAGetVertexBuffers(0, 1, &pOldVB, &oldStride, &oldOffset);
                    g_pD3DContext->RSGetViewports(&numVP, &oldVP);
                    float clearBlack[4] = {0, 0, 0, 0};
                    g_pD3DContext->ClearRenderTargetView(pBlitDst, clearBlack);
                    ID3D11BlendState *pOldBlend = nullptr;
                    FLOAT oldBlendFactor[4] = {1,1,1,1};
                    UINT oldSampleMask = 0;
                    g_pD3DContext->OMGetBlendState(&pOldBlend, oldBlendFactor, &oldSampleMask);
                    D3D11_VIEWPORT blitVP = {0, 0, (float)g_cachedVW, (float)g_cachedVH, 0, 1};
                    g_pD3DContext->RSSetViewports(1, &blitVP);
                    g_pD3DContext->OMSetRenderTargets(1, &pBlitDst, nullptr);
                    g_pD3DContext->OMSetBlendState(nullptr, nullptr, 0xFFFFFFFF);
                    g_pD3DContext->IASetInputLayout(g_pBlitLayout);
                    g_pD3DContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
                    UINT stride = 16, offset = 0;
                    g_pD3DContext->IASetVertexBuffers(0, 1, &g_pBlitVB, &stride, &offset);
                    g_pD3DContext->VSSetShader(g_pBlitVS, nullptr, 0);
                    g_pD3DContext->PSSetShader(g_pBlitPS, nullptr, 0);
                    g_pD3DContext->PSSetShaderResources(0, 1, &pBlitSRV);
                    g_pD3DContext->PSSetSamplers(0, 1, &g_pBlitSampler);
                    if (g_pBlitCB) {
                        D3D11_MAPPED_SUBRESOURCE bm;
                        if (SUCCEEDED(g_pD3DContext->Map(g_pBlitCB, 0, D3D11_MAP_WRITE_DISCARD, 0, &bm))) {
                            float *bd = (float *)bm.pData;
                            float srcW = (float)((g_renderW > 0) ? g_renderW : g_cachedVW);
                            float srcH = (float)((g_renderH > 0) ? g_renderH : g_cachedVH);
                            if (srcW < 1.0f) srcW = 1.0f;
                            if (srcH < 1.0f) srcH = 1.0f;
                            float scl = (g_ssaaScale > 1) ? (float)g_ssaaScale : 1.0f;
                            bd[0] = 1.0f / srcW;
                            bd[1] = 1.0f / srcH;
                            bd[2] = (float)((g_ssaaScale > 1) ? g_ssaaFilter : 0);
                            bd[3] = (g_ssaaScale > 1) ? (g_ssaaSharpness / 100.0f) : 0.0f;
                            bd[4] = scl;
                            bd[5] = 0.0f; bd[6] = 0.0f; bd[7] = 0.0f;
                            g_pD3DContext->Unmap(g_pBlitCB, 0);
                        }
                        g_pD3DContext->PSSetConstantBuffers(0, 1, &g_pBlitCB);
                    }
                    g_pD3DContext->Draw(4, 0);
                    g_pD3DContext->OMSetBlendState(pOldBlend, oldBlendFactor, oldSampleMask);
                    if (pOldBlend) pOldBlend->Release();
                    g_pD3DContext->RSSetViewports(1, &oldVP);
                    g_pD3DContext->OMSetRenderTargets(1, &pOldRTV, nullptr);
                    g_pD3DContext->VSSetShader(pOldVS, nullptr, 0);
                    g_pD3DContext->PSSetShader(pOldPS, nullptr, 0);
                    g_pD3DContext->IASetVertexBuffers(0, 1, &pOldVB, &oldStride, &oldOffset);
                    if (pOldRTV) pOldRTV->Release();
                    if (pOldSRV) pOldSRV->Release();
                    if (pOldVS) pOldVS->Release();
                    if (pOldPS) pOldPS->Release();
                    if (pOldVB) pOldVB->Release();
                    if (postActive) {
                        RunPostChain(pSwapRTV, vW, vH);
                    }
                    if (pSwapRTV) pSwapRTV->Release();
                } else if (postWanted && postActive) {
                    g_pD3DDevice->CreateRenderTargetView(pBackBuffer, nullptr, &pSwapRTV);
                    if (pSwapRTV && g_pSceneTex) {
                        ID3D11RenderTargetView *pNullRTV = nullptr;
                        g_pD3DContext->OMSetRenderTargets(1, &pNullRTV, nullptr);
                        g_pD3DContext->CopyResource(g_pSceneTex, pBackBuffer);
                        RunPostChain(pSwapRTV, vW, vH);
                    }
                    if (pSwapRTV) { pSwapRTV->Release(); pSwapRTV = nullptr; }
                }
                if (pBackBuffer) pBackBuffer->Release();
            }
            present_skip_blit:
            if (g_debugVelocity || g_debugForce || g_debugVortex || g_debugGravity || g_debugCollision || g_debugSpring || g_debugInteraction || g_debugSpawn || g_debugPath) {
                g_pD2DDC->SetTarget(g_pD2DTargetBitmap);
                g_pD2DDC->BeginDraw();
                RenderPhysicsDebugD2D(dwTime, smoothed);
                g_pD2DDC->EndDraw();
            }
            if (g_pSwapChain) {
                HRESULT presHr = g_pSwapChain->Present(g_enableFrameSync ? 1 : 0, 0);
                if (presHr == DXGI_ERROR_DEVICE_REMOVED || presHr == DXGI_ERROR_DEVICE_RESET) {
                    g_deviceLost.store(true);
                    return;
                }
            }
            if (g_pDCompDevice) g_pDCompDevice->Commit();
            if (!trailActive && g_history.empty() && g_ripples.empty() && g_particles.empty() && g_trailShapes.empty())
                surfaceDirty = false;
            return;
        } else if (tailVisible || !g_history.empty() || !g_particles.empty() || !g_ripples.empty() || !g_trailShapes.empty()) {
            g_deviceLost.store(true);
            return;
        }
    }
    g_pD2DDC->SetTarget(g_pD2DTargetBitmap);
    g_pD2DDC->BeginDraw();
    g_pD2DDC->Clear(D2D1::ColorF(0, 0, 0, 0));
    if (!g_particles.empty()) {
        RenderParticles(dwTime);
        surfaceDirty = true;
    }
    if (trailOn && !g_trailShapes.empty()) {
    DrawTrailShapes(0, 0, dwTime);
        surfaceDirty = true;
    }
    if (trailOn && g_enableMotionBlur && tailVisible && havePath && g_trailHistory.size() > 1 && g_trailShape != 10) {
        RenderMotionBlur(widthMul, cols, g_fadeAlpha);
        surfaceDirty = true;
    }
    if (trailOn && tailVisible && havePath && g_trailShape != 4 && g_trailShape != 10) {
        UpdateColorBrushes(cols, smoothed[0], smoothed.back());
        float glowR = g_enableGlow ? (g_glowIntensity / 100.0f) * 7.0f : 0;
        float glowO = g_enableGlow ? (g_glowIntensity / 100.0f) * 0.28f : 0;
        bool useEnhancedGlow = g_enhancedGlow && glowR > 0.1f;
        float fa = g_fadeAlpha;
        const float SHADOW_DX = 1.5f, SHADOW_DY = 2.0f;
        if (g_trailShape == 1) {
            float totalLen = 0;
            for (size_t i = 1; i < smoothed.size(); i++) {
                float ddx = smoothed[i].x - smoothed[i - 1].x, ddy = smoothed[i].y - smoothed[i - 1].y;
                totalLen += sqrtf(ddx * ddx + ddy * ddy);
            }
            float mult = (float)g_dotsMultiplier;
            float spacing = 2.4f / mult;
            float maxR = 9.0f / sqrtf(mult) * widthMul;
            int dotCount = (int)(totalLen / spacing);
            if (dotCount < 3)
                dotCount = 3;
            if (dotCount > 150)
                dotCount = 150;
            static std::vector<DotInfo> dots;
            dots.clear();
            dots.reserve(dotCount + 1);
            for (int di = 0; di <= dotCount; di++) {
                float frac = (float)di / dotCount;
                float targetDist = frac * totalLen, acc = 0;
                D2D1_POINT_2F pos = smoothed[0];
                float nx = 0, ny = 1;
                for (size_t i = 1; i < smoothed.size(); i++) {
                    float ddx = smoothed[i].x - smoothed[i - 1].x, ddy = smoothed[i].y - smoothed[i - 1].y;
                    float segLen = sqrtf(ddx * ddx + ddy * ddy);
                    if (acc + segLen >= targetDist) {
                        float t = segLen > 0 ? (targetDist - acc) / segLen : 0;
                        pos = D2D1::Point2F(smoothed[i - 1].x + ddx * t, smoothed[i - 1].y + ddy * t);
                        if (segLen > 0.001f) {
                            nx = -ddy / segLen;
                            ny = ddx / segLen;
                        }
                        break;
                    }
                    acc += segLen;
                    pos = smoothed[i];
                    if (segLen > 0.001f) {
                        nx = -ddy / segLen;
                        ny = ddx / segLen;
                    }
                }
                float sizeJit = 0.78f + Hash01(di * 7 + 1) * 0.44f;
                float opJit = 0.65f + Hash01(di * 13 + 5) * 0.55f;
                float posJit = (Hash01(di * 3 + 9) - 0.5f) * 2.5f;
                float dr = maxR * powf(1.0f - frac, 1.6f) * sizeJit;
                if (dr < 0.3f)
                    continue;
                pos.x += nx * posJit;
                pos.y += ny * posJit;
                int idx = (int)(frac * (GRAD_STOPS - 1) + 0.5f);
                if (idx >= GRAD_STOPS)
                    idx = GRAD_STOPS - 1;
                D2D1_COLOR_F dco = cols.outer[idx].color, dci = cols.inner[idx].color;
                float da = 0.86f * powf(1.0f - frac, 1.4f) * opJit * fa;
                if (g_enableSmoothGradient)
                    da *= (1.0f - frac * 0.3f);
                dots.push_back({pos, dr, dco, dci, da});
            }
            if (g_enableTrailShadow && !dots.empty()) {
                g_pShadowBrush->SetOpacity(0.18f * fa);
                for (auto &d : dots) {
                    D2D1_POINT_2F sp = D2D1::Point2F(d.pos.x + SHADOW_DX, d.pos.y + SHADOW_DY);
                    g_pD2DDC->FillEllipse(D2D1::Ellipse(sp, d.radius * 1.1f, d.radius * 1.1f), g_pShadowBrush);
                }
            }
            if (dots.size() >= 2) {
                for (size_t i = 0; i < dots.size() - 1; i++) {
                    float lineW = (dots[i].radius + dots[i + 1].radius) * 0.65f;
                    if (lineW < 0.5f)
                        continue;
                    D2D1_COLOR_F midColor = LerpColor(dots[i].outer, dots[i + 1].outer, 0.5f);
                    g_pSolidOuterBrush->SetColor(midColor);
                    g_pSolidOuterBrush->SetOpacity((dots[i].alpha + dots[i + 1].alpha) * 0.45f);
                    g_pD2DDC->DrawLine(dots[i].pos, dots[i + 1].pos, g_pSolidOuterBrush, lineW);
                }
                g_pSolidOuterBrush->SetOpacity(1.0f);
            }
            int glowCutoff = (int)(dots.size() * 0.6f);
            for (size_t di = 0; di < dots.size(); di++) {
                auto &d = dots[di];
                if (di < (size_t)glowCutoff) {
                    if (useEnhancedGlow) {
                        g_pSolidOuterBrush->SetColor(d.outer);
                        g_pSolidOuterBrush->SetOpacity(glowO * 0.35f * d.alpha / fa);
                        g_pD2DDC->FillEllipse(D2D1::Ellipse(d.pos, d.radius + glowR * 1.6f, d.radius + glowR * 1.6f),
                                              g_pSolidOuterBrush);
                        g_pSolidOuterBrush->SetOpacity(glowO * 0.7f * d.alpha / fa);
                        g_pD2DDC->FillEllipse(D2D1::Ellipse(d.pos, d.radius + glowR * 0.7f, d.radius + glowR * 0.7f),
                                              g_pSolidOuterBrush);
                    } else if (glowR > 0.1f) {
                        g_pSolidOuterBrush->SetColor(d.outer);
                        g_pSolidOuterBrush->SetOpacity(glowO * d.alpha / fa);
                        g_pD2DDC->FillEllipse(D2D1::Ellipse(d.pos, d.radius + glowR * 0.7f, d.radius + glowR * 0.7f),
                                              g_pSolidOuterBrush);
                    }
                }
                g_pSolidOuterBrush->SetColor(d.outer);
                g_pSolidOuterBrush->SetOpacity(d.alpha);
                g_pD2DDC->FillEllipse(D2D1::Ellipse(d.pos, d.radius, d.radius), g_pSolidOuterBrush);
                g_pSolidInnerBrush->SetColor(d.inner);
                g_pSolidInnerBrush->SetOpacity(d.alpha * 0.9f);
                g_pD2DDC->FillEllipse(D2D1::Ellipse(d.pos, d.radius * 0.58f, d.radius * 0.58f), g_pSolidInnerBrush);
            }
            g_pSolidOuterBrush->SetOpacity(1);
            g_pSolidInnerBrush->SetOpacity(1);
            surfaceDirty = true;
        } else {
            size_t sl = smoothed.size();
            static std::vector<D2D1_POINT_2F> lo, ro, lc, rc, gl, gr, gl2, gr2;
            lo.clear(); ro.clear(); lc.clear(); rc.clear(); gl.clear(); gr.clear(); gl2.clear(); gr2.clear();
            lo.reserve(sl); ro.reserve(sl); lc.reserve(sl); rc.reserve(sl);
            if (glowR > 0.1f) { gl.reserve(sl); gr.reserve(sl); if (useEnhancedGlow) { gl2.reserve(sl); gr2.reserve(sl); } }
            for (size_t i = 0; i < sl; ++i) {
                float ddx, ddy;
                if (i == 0) {
                    ddx = smoothed[0].x - smoothed[1].x;
                    ddy = smoothed[0].y - smoothed[1].y;
                } else if (i == sl - 1) {
                    ddx = smoothed[i - 1].x - smoothed[i].x;
                    ddy = smoothed[i - 1].y - smoothed[i].y;
                } else {
                    ddx = smoothed[i - 1].x - smoothed[i + 1].x;
                    ddy = smoothed[i - 1].y - smoothed[i + 1].y;
                }
                float ln = sqrtf(ddx * ddx + ddy * ddy);
                if (ln > 0) {
                    ddx /= ln;
                    ddy /= ln;
                } else {
                    ddx = 1;
                    ddy = 0;
                }
                float nx = -ddy, ny = ddx, ratio = (float)i / (sl - 1);
                float taper = MtTaperFactor(ratio);
                float ow = 10.0f * taper * widthMul;
                float cw = 6.0f * taper * widthMul;
                if (i == sl - 1) {
                    ow = 0;
                    cw = 0;
                }
                lo.push_back(D2D1::Point2F(smoothed[i].x + nx * ow, smoothed[i].y + ny * ow));
                ro.push_back(D2D1::Point2F(smoothed[i].x - nx * ow, smoothed[i].y - ny * ow));
                lc.push_back(D2D1::Point2F(smoothed[i].x + nx * cw, smoothed[i].y + ny * cw));
                rc.push_back(D2D1::Point2F(smoothed[i].x - nx * cw, smoothed[i].y - ny * cw));
                if (glowR > 0.1f) {
                    float gw = ow + glowR * (1.0f - ratio * 0.3f);
                    gl.push_back(D2D1::Point2F(smoothed[i].x + nx * gw, smoothed[i].y + ny * gw));
                    gr.push_back(D2D1::Point2F(smoothed[i].x - nx * gw, smoothed[i].y - ny * gw));
                    if (useEnhancedGlow) {
                        float gw2 = ow + glowR * 1.8f * (1.0f - ratio * 0.2f);
                        gl2.push_back(D2D1::Point2F(smoothed[i].x + nx * gw2, smoothed[i].y + ny * gw2));
                        gr2.push_back(D2D1::Point2F(smoothed[i].x - nx * gw2, smoothed[i].y - ny * gw2));
                    }
                }
            }
            g_pD2DDC->SetAntialiasMode(D2D1_ANTIALIAS_MODE_ALIASED);
            if (g_enableTrailShadow && lo.size() >= 2) {
                static std::vector<D2D1_POINT_2F> slo, sro;
                slo.clear(); sro.clear();
                slo.reserve(lo.size());
                sro.reserve(ro.size());
                for (size_t i = 0; i < lo.size(); i++) {
                    slo.push_back(D2D1::Point2F(lo[i].x + SHADOW_DX, lo[i].y + SHADOW_DY));
                    sro.push_back(D2D1::Point2F(ro[i].x + SHADOW_DX, ro[i].y + SHADOW_DY));
                }
                CreateMeshFromQuadStrip(g_pD2DDC, &g_pShadowMesh, slo, sro);
                if (g_pShadowMesh) {
                    g_pShadowBrush->SetOpacity(0.18f * fa);
                    g_pD2DDC->FillMesh(g_pShadowMesh, g_pShadowBrush);
                }
            }
            if (useEnhancedGlow && gl2.size() >= 2) {
                CreateMeshFromQuadStrip(g_pD2DDC, &g_pGlow2Mesh, gl2, gr2);
                if (g_pGlow2Mesh) {
                    g_pSolidOuterBrush->SetColor(cols.solidOuter);
                    g_pSolidOuterBrush->SetOpacity(glowO * 0.3f * fa);
                    g_pD2DDC->FillMesh(g_pGlow2Mesh, g_pSolidOuterBrush);
                }
            }
            if (glowR > 0.1f && gl.size() >= 2) {
                CreateMeshFromQuadStrip(g_pD2DDC, &g_pGlowMesh, gl, gr);
                if (g_pGlowMesh) {
                    g_pSolidOuterBrush->SetColor(cols.solidOuter);
                    g_pSolidOuterBrush->SetOpacity(useEnhancedGlow ? glowO * 0.65f * fa : glowO * fa);
                    g_pD2DDC->FillMesh(g_pGlowMesh, g_pSolidOuterBrush);
                }
            }
            float headR = 10.0f * widthMul, innerHeadR = 6.0f * widthMul;
            CreateMeshFromQuadStripWithCap(g_pD2DDC, &g_pOuterMesh, lo, ro, smoothed[0], headR);
            CreateMeshFromQuadStripWithCap(g_pD2DDC, &g_pInnerMesh, lc, rc, smoothed[0], innerHeadR);
            ID2D1Brush *ob = g_pSolidOuterBrush, *ib = g_pSolidInnerBrush;
            if (g_enableSmoothGradient && g_pGradOuterBrush && g_pGradInnerBrush) {
                ob = g_pGradOuterBrush;
                ib = g_pGradInnerBrush;
            } else {
                g_pSolidOuterBrush->SetOpacity(.86f * fa);
                g_pSolidInnerBrush->SetOpacity(.86f * fa);
            }
            if (g_pOuterMesh) g_pD2DDC->FillMesh(g_pOuterMesh, ob);
            if (g_pInnerMesh) g_pD2DDC->FillMesh(g_pInnerMesh, ib);
            g_pSolidOuterBrush->SetOpacity(1);
            g_pSolidInnerBrush->SetOpacity(1);
            g_pD2DDC->SetAntialiasMode(D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
            if (g_enableHeadHighlight) {
                g_pSolidInnerBrush->SetColor(D2D1::ColorF(1, 1, 1, 1));
                g_pSolidInnerBrush->SetOpacity(0.85f * fa);
                g_pD2DDC->FillEllipse(D2D1::Ellipse(smoothed[0], 2.8f, 2.8f), g_pSolidInnerBrush);
                g_pSolidInnerBrush->SetOpacity(1.0f);
            }
            surfaceDirty = true;
        }
    }
    if (g_enableClickEffect && !g_ripples.empty()) {
        DrawClickEffects(0, 0, dwTime, cols, vX, vY);
        surfaceDirty = true;
    }
    HRESULT hr = g_pD2DDC->EndDraw();
    if (hr == D2DERR_RECREATE_TARGET || hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET) {
        Wh_Log(L"RenderFrame: device lost (0x%08X), scheduling recovery", hr);
        g_deviceLost.store(true);
        return;
    }
    if (g_pSwapChain) {
        HRESULT presHr = g_pSwapChain->Present(g_enableFrameSync ? 1 : 0, 0);
        if (presHr == DXGI_ERROR_DEVICE_REMOVED || presHr == DXGI_ERROR_DEVICE_RESET) {
            Wh_Log(L"RenderFrame: Present device lost (0x%08X), scheduling recovery", presHr);
            g_deviceLost.store(true);
            return;
        }
    }
    if (g_pDCompDevice)
        g_pDCompDevice->Commit();
    if (!trailActive && g_history.empty() && g_ripples.empty() && g_particles.empty() && g_trailShapes.empty())
        surfaceDirty = false;
}
static void CreateStarGeometry(ID2D1Factory *factory, ID2D1PathGeometry **geom) {
    factory->CreatePathGeometry(geom);
    ID2D1GeometrySink *sink = nullptr;
    (*geom)->Open(&sink);
    sink->SetFillMode(D2D1_FILL_MODE_WINDING);
    sink->BeginFigure(D2D1::Point2F(0, -1), D2D1_FIGURE_BEGIN_FILLED);
    for (int i = 1; i < 10; i++) {
        float angle = i * 3.14159265f / 5.0f - 1.5707963f;
        float r = (i % 2 == 0) ? 1.0f : 0.42f;
        sink->AddLine(D2D1::Point2F(cosf(angle) * r, sinf(angle) * r));
    }
    sink->EndFigure(D2D1_FIGURE_END_CLOSED);
    sink->Close();
    sink->Release();
}
static void CreateHexagramGeometry(ID2D1Factory *factory, ID2D1PathGeometry **geom) {
    factory->CreatePathGeometry(geom);
    ID2D1GeometrySink *sink = nullptr;
    (*geom)->Open(&sink);
    sink->SetFillMode(D2D1_FILL_MODE_WINDING);
    sink->BeginFigure(D2D1::Point2F(0, -1), D2D1_FIGURE_BEGIN_FILLED);
    sink->AddLine(D2D1::Point2F(0.866f, 0.5f));
    sink->AddLine(D2D1::Point2F(-0.866f, 0.5f));
    sink->EndFigure(D2D1_FIGURE_END_CLOSED);
    sink->BeginFigure(D2D1::Point2F(0, 1), D2D1_FIGURE_BEGIN_FILLED);
    sink->AddLine(D2D1::Point2F(-0.866f, -0.5f));
    sink->AddLine(D2D1::Point2F(0.866f, -0.5f));
    sink->EndFigure(D2D1_FIGURE_END_CLOSED);
    sink->Close();
    sink->Release();
}
static void CreateHeartGeometry(ID2D1Factory *factory, ID2D1PathGeometry **geom) {
    factory->CreatePathGeometry(geom);
    ID2D1GeometrySink *sink = nullptr;
    (*geom)->Open(&sink);
    sink->SetFillMode(D2D1_FILL_MODE_WINDING);
    const int N = 48;
    for (int i = 0; i < N; i++) {
        float t = (float)i / N * 6.2831853f;
        float x = 16.0f * powf(sinf(t), 3.0f);
        float y = -(13.0f * cosf(t) - 5.0f * cosf(2.0f * t) - 2.0f * cosf(3.0f * t) - cosf(4.0f * t));
        x /= 17.0f;
        y /= 17.0f;
        if (i == 0)
            sink->BeginFigure(D2D1::Point2F(x, y), D2D1_FIGURE_BEGIN_FILLED);
        else
            sink->AddLine(D2D1::Point2F(x, y));
    }
    sink->EndFigure(D2D1_FIGURE_END_CLOSED);
    sink->Close();
    sink->Release();
}
static void ReleaseAllRenderResources() {
    if (g_pTextFormat) { g_pTextFormat->Release(); g_pTextFormat = nullptr; }
    ReleaseGradientBrushes();
    if (g_pShadowBrush) { g_pShadowBrush->Release(); g_pShadowBrush = nullptr; }
    if (g_pSolidInnerBrush) { g_pSolidInnerBrush->Release(); g_pSolidInnerBrush = nullptr; }
    if (g_pSolidOuterBrush) { g_pSolidOuterBrush->Release(); g_pSolidOuterBrush = nullptr; }
    if (g_pMSAARTV) { g_pMSAARTV->Release(); g_pMSAARTV = nullptr; }
    if (g_pMSAATexture) { g_pMSAATexture->Release(); g_pMSAATexture = nullptr; }
    if (g_pSSAAResolveSRV) { g_pSSAAResolveSRV->Release(); g_pSSAAResolveSRV = nullptr; }
    if (g_pSSAAResolveTexture) { g_pSSAAResolveTexture->Release(); g_pSSAAResolveTexture = nullptr; }
    if (g_pSSAASRV) { g_pSSAASRV->Release(); g_pSSAASRV = nullptr; }
    if (g_pSSAARTV) { g_pSSAARTV->Release(); g_pSSAARTV = nullptr; }
    if (g_pSSAATexture) { g_pSSAATexture->Release(); g_pSSAATexture = nullptr; }
    ReleasePostTargets();
    if (g_pBlitSampler) { g_pBlitSampler->Release(); g_pBlitSampler = nullptr; }
    if (g_pBlitVB) { g_pBlitVB->Release(); g_pBlitVB = nullptr; }
    if (g_pBlitLayout) { g_pBlitLayout->Release(); g_pBlitLayout = nullptr; }
    if (g_pBlitPS) { g_pBlitPS->Release(); g_pBlitPS = nullptr; }
    if (g_pBlitVS) { g_pBlitVS->Release(); g_pBlitVS = nullptr; }
    if (g_pCachedRTV) { g_pCachedRTV->Release(); g_pCachedRTV = nullptr; }
    if (g_pD2DTargetBitmap) { g_pD2DTargetBitmap->Release(); g_pD2DTargetBitmap = nullptr; }
    if (g_pSwapChain) { g_pSwapChain->Release(); g_pSwapChain = nullptr; }
    if (g_pDCompTarget && g_pDCompVisual) {
        g_pDCompTarget->SetRoot(nullptr);
    }
    if (g_pDCompVisual) { g_pDCompVisual->Release(); g_pDCompVisual = nullptr; }
    if (g_pDCompTarget) { g_pDCompTarget->Release(); g_pDCompTarget = nullptr; }
    if (g_pDCompDevice) { g_pDCompDevice->Release(); g_pDCompDevice = nullptr; }
    if (g_pShadowMesh) { g_pShadowMesh->Release(); g_pShadowMesh = nullptr; }
    if (g_pGlow2Mesh) { g_pGlow2Mesh->Release(); g_pGlow2Mesh = nullptr; }
    if (g_pGlowMesh) { g_pGlowMesh->Release(); g_pGlowMesh = nullptr; }
    if (g_pOuterMesh) { g_pOuterMesh->Release(); g_pOuterMesh = nullptr; }
    if (g_pInnerMesh) { g_pInnerMesh->Release(); g_pInnerMesh = nullptr; }
    if (g_pD2DDC) { g_pD2DDC->Release(); g_pD2DDC = nullptr; }
    if (g_pD2DDevice) { g_pD2DDevice->Release(); g_pD2DDevice = nullptr; }
    if (g_pStarGeom) { g_pStarGeom->Release(); g_pStarGeom = nullptr; }
    if (g_pHexagramGeom) { g_pHexagramGeom->Release(); g_pHexagramGeom = nullptr; }
    if (g_pHeartGeom) { g_pHeartGeom->Release(); g_pHeartGeom = nullptr; }
    for (auto &kv : mtb::s_keyCache) { if (kv.second) kv.second->Release(); }
    mtb::s_keyCache.clear();
    if (g_pD2DFactory) { g_pD2DFactory->Release(); g_pD2DFactory = nullptr; }
    if (g_dwFactory) { g_dwFactory->Release(); g_dwFactory = nullptr; }
    if (g_pDXGIDevice) { g_pDXGIDevice->Release(); g_pDXGIDevice = nullptr; }
    if (g_pD3DContext) { g_pD3DContext->Release(); g_pD3DContext = nullptr; }
    if (g_pD3DDevice) { g_pD3DDevice->Release(); g_pD3DDevice = nullptr; }
    g_cachedVW = 0;
    g_cachedVH = 0;
    g_trailHistory.clear();
    g_history.clear();
    g_particles.clear();
    g_ripples.clear();
    g_trailShapes.clear();
    ReleaseNativeRendering();
}
static bool CreateD3DAndD2DDevices(const wchar_t* logTag) {
    D3D_FEATURE_LEVEL fl = D3D_FEATURE_LEVEL_11_0;
    UINT createFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT | D3D11_CREATE_DEVICE_SINGLETHREADED;
    D3D_FEATURE_LEVEL featureLevels[] = {
        D3D_FEATURE_LEVEL_11_1, D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1, D3D_FEATURE_LEVEL_10_0,
    };
    if (FAILED(D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, createFlags, featureLevels,
                                 ARRAYSIZE(featureLevels), D3D11_SDK_VERSION, &g_pD3DDevice, &fl, &g_pD3DContext))) {
        D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_WARP, nullptr, createFlags, featureLevels, ARRAYSIZE(featureLevels),
                          D3D11_SDK_VERSION, &g_pD3DDevice, &fl, &g_pD3DContext);
    }
    if (!g_pD3DDevice) {
        Wh_Log(L"%s: D3D11 device creation FAILED", logTag);
        return false;
    }
    Wh_Log(L"%s: D3D11 device created (feature level %d)", logTag, fl);
    g_pD3DDevice->QueryInterface(__uuidof(IDXGIDevice), (void **)&g_pDXGIDevice);
    D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, __uuidof(ID2D1Factory1), nullptr, (void **)&g_pD2DFactory);
    {
        HRESULT hrDW = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), (IUnknown**)&g_dwFactory);
        if (FAILED(hrDW)) { Wh_Log(L"DWriteCreateFactory FAILED: 0x%08x", hrDW); g_dwFactory = nullptr; }
    }
    if (g_dwFactory && !g_pTextFormat) {
        g_dwFactory->CreateTextFormat(L"Segoe UI Emoji", nullptr, DWRITE_FONT_WEIGHT_BOLD, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL,
            (float)g_textFontSize, L"en-us", &g_pTextFormat);
        if (!g_pTextFormat) g_dwFactory->CreateTextFormat(L"Segoe UI", nullptr, DWRITE_FONT_WEIGHT_BOLD, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL,
            (float)g_textFontSize, L"en-us", &g_pTextFormat);
        if (g_pTextFormat) {
            g_pTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
            g_pTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
        }
    }
    if (g_pD2DFactory && g_pDXGIDevice) {
        g_pD2DFactory->CreateDevice(g_pDXGIDevice, &g_pD2DDevice);
        if (g_pD2DDevice) g_pD2DDevice->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, &g_pD2DDC);
    }
    if (!g_pD2DDC) {
        Wh_Log(L"%s: D2D1 DC creation FAILED", logTag);
        return false;
    }
    g_pD2DDC->SetAntialiasMode(D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
    g_pD2DDC->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE);
    g_pD2DDC->CreateSolidColorBrush(D2D1::ColorF(0, 0, 0, 1), &g_pSolidOuterBrush);
    g_pD2DDC->CreateSolidColorBrush(D2D1::ColorF(1, 1, 1, 1), &g_pSolidInnerBrush);
    g_pD2DDC->CreateSolidColorBrush(D2D1::ColorF(0, 0, 0, 0.18f), &g_pShadowBrush);
    if (g_pD2DFactory) {
        CreateStarGeometry(g_pD2DFactory, &g_pStarGeom);
        CreateHexagramGeometry(g_pD2DFactory, &g_pHexagramGeom);
        CreateHeartGeometry(g_pD2DFactory, &g_pHeartGeom);
    }
    return true;
}
static bool InitAllRenderResources() {
    if (!CreateD3DAndD2DDevices(L"Recover"))
        return false;
    if (g_pDXGIDevice) {
        DCompositionCreateDevice(g_pDXGIDevice, __uuidof(IDCompositionDevice), (void **)&g_pDCompDevice);
    }
    if (g_pDCompDevice) {
        g_pDCompDevice->CreateTargetForHwnd(g_overlayHwnd, TRUE, &g_pDCompTarget);
        g_pDCompDevice->CreateVisual(&g_pDCompVisual);
        if (g_pDCompTarget && g_pDCompVisual) g_pDCompTarget->SetRoot(g_pDCompVisual);
        Wh_Log(L"Recover: DComp device ready");
    }
    {
        int vx, vy, vw, vh;
        LoadVirtualScreen(vx, vy, vw, vh);
        RecreateSwapChain(vw, vh);
    }
    InitNativeRendering();
    Wh_Log(L"Recover: resources reinitialized");
    return true;
}
static void UpdateVirtualScreenCache() {
    int nx = GetSystemMetrics(SM_XVIRTUALSCREEN);
    int ny = GetSystemMetrics(SM_YVIRTUALSCREEN);
    int nw = GetSystemMetrics(SM_CXVIRTUALSCREEN);
    int nh = GetSystemMetrics(SM_CYVIRTUALSCREEN);
    if (nh > 1) nh -= 1;
    PublishVirtualScreen(nx, ny, nw, nh);
}
static void ApplyOverlayLayer(HWND hwnd) {
    if (!hwnd) return;
    bool topmost = g_overlayTopmost.load();
    SetWindowPos(hwnd, topmost ? HWND_TOPMOST : HWND_BOTTOM, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
}
static void ResizeOverlayToVirtualScreen(HWND hwnd) {
    bool topmost = g_overlayTopmost.load();
    SetWindowPos(hwnd, topmost ? HWND_TOPMOST : HWND_BOTTOM, g_virtX, g_virtY, g_virtW, g_virtH, SWP_NOACTIVATE);
}
static LRESULT CALLBACK OverlayWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (msg == WM_HOTKEY) {
        int id = (int)wParam;
        HotkeyHandleId(id);
        return 0;
    }
    if (msg == WM_POWERBROADCAST && wParam == PBT_APMRESUMEAUTOMATIC) {
        Wh_Log(L"OverlayWndProc: wake from sleep, forcing device recovery");
        g_deviceLost.store(true);
        return TRUE;
    }
    if (msg == WM_DISPLAYCHANGE) {
        UpdateVirtualScreenCache();
        UpdateMonitorDpiScale();
        ResizeOverlayToVirtualScreen(hwnd);
        return 0;
    }
    if (msg == WM_DPICHANGED) {
        UpdateVirtualScreenCache();
        ResizeOverlayToVirtualScreen(hwnd);
        g_charAtlasDirty = true;
        UpdateMonitorDpiScale();
        return 0;
    }
    if (msg == WM_APP_REHOTKEY) {
        HotkeyRegisterAll();
        return 0;
    }
    if (msg == WM_APP_SHOW_OVERLAY) {
        ShowWindow(hwnd, SW_SHOWNOACTIVATE);
        ApplyOverlayLayer(hwnd);
        return 0;
    }
    if (msg == WM_APP_HIDE_OVERLAY) {
        ShowWindow(hwnd, SW_HIDE);
        return 0;
    }
    if (msg == WM_APP_OVERLAY_LAYER) {
        ApplyOverlayLayer(hwnd);
        return 0;
    }
    if (msg == WM_TIMER && wParam == 1) {
        ApplyOverlayLayer(hwnd);
        return 0;
    }
    if (msg == WM_MOUSEACTIVATE) {
        return MA_NOACTIVATEANDEAT;
    }
    if (msg == WM_SYSCOMMAND && (wParam & 0xFFF0) == SC_MINIMIZE) {
        return 0;
    }
    if (msg == WM_NCHITTEST) {
        return HTTRANSPARENT;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}
DWORD WINAPI RenderThreadProc(LPVOID);
DWORD WINAPI OverlayThreadProc(LPVOID) {
    HRESULT hrCo = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
    bool coOwned = (hrCo == S_OK);
    SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    HINSTANCE hi = GetModuleHandle(NULL);
    const wchar_t CN[] = L"MouseTrailClass";
    WNDCLASS wc = {};
    wc.lpfnWndProc = OverlayWndProc;
    wc.hInstance = hi;
    wc.lpszClassName = CN;
    wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    RegisterClass(&wc);
    UpdateVirtualScreenCache();
    int sx = g_virtX, sy = g_virtY;
    int sw = g_virtW, sh = g_virtH;
    g_overlayHwnd = CreateWindowEx(
        WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE, CN,
        L"MouseTrailOverlay", WS_POPUP, sx, sy, sw, sh, NULL, NULL, hi, NULL);
    if (!g_overlayHwnd) {
        if (g_readyEvent) SetEvent(g_readyEvent);
        if (coOwned) CoUninitialize();
        return 0;
    }
    SetLayeredWindowAttributes(g_overlayHwnd, RGB(0, 0, 0), 255, LWA_COLORKEY);
    UpdateMonitorDpiScale();
    Wh_Log(L"[DPI] overlay start: scale=%.3f mode=%d scaleText=%d", (double)g_monitorDpiScale.load(), g_dpiScaleMode,
           g_dpiScaleText ? 1 : 0);
    HotkeyRegisterAll();
    g_fgHook = SetWinEventHook(EVENT_SYSTEM_FOREGROUND, EVENT_SYSTEM_FOREGROUND, NULL, ForegroundEventProc, 0, 0,
                               WINEVENT_OUTOFCONTEXT);
    g_fgHookActive.store(g_fgHook != nullptr);
    g_desktopActive.store(IsDesktopForeground());
    SetTimer(g_overlayHwnd, 1, 2000, NULL);
    if (g_readyEvent) SetEvent(g_readyEvent);
    g_renderExitEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    g_renderThread = CreateThread(NULL, 0, RenderThreadProc, NULL, 0, NULL);
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    Wh_Log(L"OverlayThread: message loop ended, stopping render thread");
    if (g_renderExitEvent) SetEvent(g_renderExitEvent);
    if (g_renderThread) {
        DWORD wr = WaitForSingleObject(g_renderThread, 5000);
        if (wr == WAIT_TIMEOUT) {
            Wh_Log(L"OverlayThread: render thread did not exit in 5s, waiting 2s more");
            if (WaitForSingleObject(g_renderThread, 2000) == WAIT_TIMEOUT) {
                Wh_Log(L"OverlayThread: render thread stuck, terminating as last resort");
                TerminateThread(g_renderThread, 1);
                WaitForSingleObject(g_renderThread, 1000);
            }
        }
        CloseHandle(g_renderThread);
        g_renderThread = nullptr;
    }
    KillTimer(g_overlayHwnd, 1);
    if (g_fgHook) {
        UnhookWinEvent(g_fgHook);
        g_fgHook = nullptr;
        g_fgHookActive.store(false);
    }
    DestroyWindow(g_overlayHwnd);
    g_overlayHwnd = nullptr;
    UnregisterClass(CN, hi);
    if (coOwned) CoUninitialize();
    return 0;
}
DWORD WINAPI RenderThreadProc(LPVOID) {
    HRESULT hrCo = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
    bool coOwned = (hrCo == S_OK);
    SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    srand((unsigned)GetTickCount());
    if (g_readyEvent) WaitForSingleObject(g_readyEvent, INFINITE);
    if (!g_overlayHwnd) {
        Wh_Log(L"RenderThread: window not ready, exiting");
        if (coOwned) CoUninitialize();
        return 0;
    }
    if (!CreateD3DAndD2DDevices(L"RenderThread")) {
        if (coOwned) CoUninitialize();
        return 0;
    }
    Wh_Log(L"RenderThread: D2D1 DC created");
    if (!InitNativeRendering()) {
        Wh_Log(L"RenderThread: Native D3D11 rendering init FAILED, falling back to D2D1");
    }
    if (g_pDXGIDevice) {
        DCompositionCreateDevice(g_pDXGIDevice, __uuidof(IDCompositionDevice), (void **)&g_pDCompDevice);
    }
    if (g_pDCompDevice) {
        g_pDCompDevice->CreateTargetForHwnd(g_overlayHwnd, TRUE, &g_pDCompTarget);
        g_pDCompDevice->CreateVisual(&g_pDCompVisual);
        if (g_pDCompTarget && g_pDCompVisual) g_pDCompTarget->SetRoot(g_pDCompVisual);
        Wh_Log(L"RenderThread: DComp device ready");
    } else {
        Wh_Log(L"RenderThread: DCompositionCreateDevice FAILED");
    }
    {
        int vx, vy, vw, vh;
        LoadVirtualScreen(vx, vy, vw, vh);
        RecreateSwapChain(vw, vh);
    }
    GetCursorPos(&g_lastPos);
    InitializeCriticalSection(&g_sampleCS);
    g_bgSampleExitEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    g_bgSampleThread = CreateThread(NULL, 0, BgSamplerThreadProc, NULL, 0, NULL);
    if (g_bgSampleThread)
        Wh_Log(L"RenderThread: background sampler thread started");
    else
        Wh_Log(L"RenderThread: failed to create background sampler thread");
    Wh_Log(L"RenderThread: entering render loop");
    DWORD waitMs = 1;
    int frameCount = 0;
    while (WaitForSingleObject(g_renderExitEvent, waitMs) != WAIT_OBJECT_0) {
        if (!g_deviceLost.load() && g_pD3DDevice) {
            frameCount++;
            if (frameCount >= 120) {
                frameCount = 0;
                HRESULT dr = g_pD3DDevice->GetDeviceRemovedReason();
                if (dr != S_OK) {
                    Wh_Log(L"RenderThread: proactive device lost detected (0x%08X)", dr);
                    g_deviceLost.store(true);
                }
            }
        }
        if (g_deviceLost.exchange(false)) {
            Wh_Log(L"RenderThread: recovering from device loss");
            ReleaseAllRenderResources();
            if (!InitAllRenderResources()) {
                Wh_Log(L"RenderThread: recovery FAILED, retrying in 1s");
                g_deviceLost.store(true);
                Sleep(1000);
                continue;
            }
            Wh_Log(L"RenderThread: recovery complete");
        }
        RenderFrame();
        bool isActive = !g_history.empty() || !g_particles.empty() || !g_ripples.empty() || !g_trailShapes.empty();
        waitMs = (g_desktopOnly && g_desktopSuppressed) ? 40 : (isActive ? 1 : 16);
    }
    Wh_Log(L"RenderThread: exiting, cleaning up");
    if (g_bgSampleExitEvent) SetEvent(g_bgSampleExitEvent);
    if (g_bgSampleThread) {
        if (WaitForSingleObject(g_bgSampleThread, 3000) == WAIT_TIMEOUT) {
            Wh_Log(L"RenderThread: background sampler thread timed out, terminating");
            TerminateThread(g_bgSampleThread, 1);
            WaitForSingleObject(g_bgSampleThread, 1000);
        }
        CloseHandle(g_bgSampleThread);
        g_bgSampleThread = NULL;
    }
    if (g_bgSampleExitEvent) { CloseHandle(g_bgSampleExitEvent); g_bgSampleExitEvent = NULL; }
    DeleteCriticalSection(&g_sampleCS);
    MusicStopCapture();
    ReleaseAllRenderResources();
    if (coOwned) CoUninitialize();
    return 0;
}
BOOL WhTool_ModInit() {
    LoadSettings();
    g_readyEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    g_threadHandle = CreateThread(NULL, 0, OverlayThreadProc, NULL, 0, NULL);
    if (!g_threadHandle) {
        if (g_readyEvent) { CloseHandle(g_readyEvent); g_readyEvent = NULL; }
        return FALSE;
    }
    return TRUE;
}
void WhTool_ModUninit() {
    if (g_threadHandle) {
        if (g_readyEvent)
            WaitForSingleObject(g_readyEvent, INFINITE);
        if (g_overlayHwnd)
            PostMessage(g_overlayHwnd, WM_QUIT, 0, 0);
        WaitForSingleObject(g_threadHandle, INFINITE);
        CloseHandle(g_threadHandle);
        g_threadHandle = NULL;
    }
    if (g_renderExitEvent) {
        CloseHandle(g_renderExitEvent);
        g_renderExitEvent = NULL;
    }
    if (g_readyEvent) {
        CloseHandle(g_readyEvent);
        g_readyEvent = NULL;
    }
}
void WhTool_ModSettingsChanged() {
    g_settingsDirty.store(true);
}
bool g_isToolModProcessLauncher;
HANDLE g_toolModProcessMutex;
void WINAPI EntryPoint_Hook() {
    Wh_Log(L">");
    ExitThread(0);
}
BOOL Wh_ModInit() {
    DWORD sessionId = 0;
    BOOL gotSession = ProcessIdToSessionId(GetCurrentProcessId(), &sessionId);
    if (gotSession && sessionId == 0) {
        return FALSE;
    }
    bool isExcluded = false;
    bool isToolModProcess = false;
    bool isCurrentToolModProcess = false;
    int argc;
    LPWSTR *argv = CommandLineToArgvW(GetCommandLine(), &argc);
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
        g_toolModProcessMutex = CreateMutex(nullptr, TRUE, L"windhawk-tool-mod_" WH_MOD_ID);
        if (!g_toolModProcessMutex) {
            Wh_Log(L"CreateMutex failed");
            ExitProcess(1);
        }
        if (GetLastError() == ERROR_ALREADY_EXISTS) {
            Wh_Log(L"Tool mod already running");
            ExitProcess(1);
        }
        if (!WhTool_ModInit()) {
            ExitProcess(1);
        }
        IMAGE_DOS_HEADER *dosHeader = (IMAGE_DOS_HEADER *)GetModuleHandle(nullptr);
        IMAGE_NT_HEADERS *ntHeaders = (IMAGE_NT_HEADERS *)((BYTE *)dosHeader + dosHeader->e_lfanew);
        DWORD entryPointRVA = ntHeaders->OptionalHeader.AddressOfEntryPoint;
        void *entryPoint = (BYTE *)dosHeader + entryPointRVA;
        Wh_SetFunctionHook(entryPoint, (void *)EntryPoint_Hook, nullptr);
        return TRUE;
    }
    if (isToolModProcess) {
        return FALSE;
    }
    g_isToolModProcessLauncher = true;
    return TRUE;
}
void Wh_ModAfterInit() {
    if (!g_isToolModProcessLauncher) {
        return;
    }
    WCHAR currentProcessPath[MAX_PATH];
    switch (GetModuleFileName(nullptr, currentProcessPath, ARRAYSIZE(currentProcessPath))) {
        case 0:
        case ARRAYSIZE(currentProcessPath):
            return;
    }
    WCHAR commandLine[MAX_PATH + 2 + (sizeof(L" -tool-mod \"" WH_MOD_ID "\"") / sizeof(WCHAR)) - 1];
    swprintf_s(commandLine, L"\"%s\" -tool-mod \"%s\"", currentProcessPath, WH_MOD_ID);
    HMODULE kernelModule = GetModuleHandle(L"kernelbase.dll");
    if (!kernelModule) {
        kernelModule = GetModuleHandle(L"kernel32.dll");
        if (!kernelModule) {
            Wh_Log(L"No kernelbase.dll/kernel32.dll");
            return;
        }
    }
    using CreateProcessInternalW_t = BOOL(WINAPI *)(
        HANDLE hUserToken, LPCWSTR lpApplicationName, LPWSTR lpCommandLine, LPSECURITY_ATTRIBUTES lpProcessAttributes,
        LPSECURITY_ATTRIBUTES lpThreadAttributes, WINBOOL bInheritHandles, DWORD dwCreationFlags, LPVOID lpEnvironment,
        LPCWSTR lpCurrentDirectory, LPSTARTUPINFOW lpStartupInfo, LPPROCESS_INFORMATION lpProcessInformation,
        PHANDLE hRestrictedUserToken);
    CreateProcessInternalW_t pCreateProcessInternalW =
        (CreateProcessInternalW_t)GetProcAddress(kernelModule, "CreateProcessInternalW");
    if (!pCreateProcessInternalW) {
        Wh_Log(L"No CreateProcessInternalW");
        return;
    }
    STARTUPINFO si{};
    si.cb = sizeof(STARTUPINFO);
    si.dwFlags = STARTF_FORCEOFFFEEDBACK;
    PROCESS_INFORMATION pi;
    if (!pCreateProcessInternalW(nullptr, currentProcessPath, commandLine, nullptr, nullptr, FALSE, NORMAL_PRIORITY_CLASS,
                                 nullptr, nullptr, &si, &pi, nullptr)) {
        Wh_Log(L"CreateProcess failed");
        return;
    }
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}
void Wh_ModSettingsChanged() {
    if (g_isToolModProcessLauncher) {
        return;
    }
    WhTool_ModSettingsChanged();
}
void Wh_ModUninit() {
    if (g_isToolModProcessLauncher) {
        return;
    }
    WhTool_ModUninit();
    ExitProcess(0);
}
