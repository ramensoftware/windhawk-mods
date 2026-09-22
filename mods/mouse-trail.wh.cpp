// ==WindhawkMod==
// @id              mouse-trail
// @name            Mouse Trail
// @name:zh-CN      鼠标拖尾
// @description     Highly customizable cursor trail with D3D11 rendering, 23 color modes, 10 trail shapes, 2.5D particles, click effects, centripetal vortex, particle physics, music reactive. DirectComposition acceleration, low idle CPU.
// @description:zh-CN 高度可定制的鼠标拖尾，原生 D3D11 渲染，23种颜色模式，10种拖尾形状，2.5D 立体效果，粒子系统，点击特效，向心力漩涡，粒子物理，音乐响应框架。DirectComposition 硬件加速，闲置低 CPU。
// @version         3.4.2
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
* **Zero CPU Idle:** Window hidden when cursor is stationary and no effects are active
* **Background Sampling:** Desktop color sampling runs on a dedicated low-priority thread

### Function Trail Variables

Available in custom function formulas: `t` (normalized 0=head, 1=tail), `d` (distance from head in pixels), `time` (seconds). Functions: sin cos exp sqrt abs. Operators: + - * / ^.

Examples: `sin(d * 0.15) * 8`, `sin(d * 0.25) * exp(0 - t * 2.5) * 10`.

### Color Format

Hex RGB, e.g. `FF0000`=red, `00FF00`=green, `0000FF`=blue, `FFD700`=gold.

### Credits

Developed by [MCheng404](https://github.com/MCheng404).
Original overlay/smear architecture inspired by [TheatriChris](https://github.com/TheatriChris)'s cursor-motion-blur mod (MIT licensed).

---

# 鼠标拖尾

Windhawk 高度可定制鼠标拖尾特效模组。基于原生 D3D11 + DirectComposition 硬件加速，包含 23 种颜色模式、10 种拖尾渲染风格、9 种粒子形状、完整牛顿粒子物理系统（质量、引力、碰撞、电磁力、湍流、流体耦合）、向心力漩涡轨道捕获、音乐响应音频物理、2.5D 深度效果、点击特效和文字/Emoji 粒子。独立 Tool Mod 进程运行——闲置零 CPU，激活时全硬件加速。

---

### 渲染架构

* **原生 D3D11 管线：** 自定义 HLSL 顶点/像素着色器，粒子实例化渲染。核心拖尾、粒子、形状渲染不依赖 D2D1。
* **DirectComposition 硬件覆盖层：** 预乘 alpha DXGI 翻转交换链，per-pixel alpha，与桌面合成器无撕裂合成。
* **2.5D 深度效果：** 每个粒子带 z 深度 + 透视投影 + 简单光照 + 深度缩放（近大远小近亮远暗）。
* **双线程设计：** UI 线程跑窗口消息泵，渲染线程独占 D3D11/DComp。鼠标输入永不被渲染阻塞。
* **HDR 自动检测：** 自动识别 HDR 显示器并使用 R16G16B16A16_FLOAT，SDR 自动回退 BGRA8。
* **设备丢失恢复：** 主动 `GetDeviceRemovedReason()` 轮询 + `WM_POWERBROADCAST` 唤醒处理。GPU TDR、驱动更新、显卡切换或睡眠唤醒时全自动重建 D3D/DComp 栈——无黑帧。
* **显示变化处理：** 显示器切换或分辨率变化时自动调整覆盖层。
* **加法混合发光：** 拖尾、粒子、波纹发光层使用 SrcAlpha+One 加法混合，光晕更通透。
* **快速移动插值：** 帧间隔 ≤10ms 时沿路径插值补粒子，消除快速移动缝隙。

### 拖尾渲染风格（10种）

* **锥形飘带：** 经典渐隐飘带，带发光、阴影和头部高光
* **圆点链：** 沿路径排列的圆点，密度和大小可调
* **函数曲线：** 自定义数学函数变形轨迹（正弦、阻尼、心跳、漩涡或自定义公式）
* **波浪曲线：** 动态正弦波变形
* **形状拖尾：** 沿路径生成 9 种可选形状（爱心、五角星、六边形、圆形、菱形、三角形、花朵、五边形、六芒星或随机），带随机速度、旋转、重力，间隔/大小/数量/存活时间可调
* **双线拖尾：** 两条平行拖尾带
* **虚线拖尾：** 常量宽度分段虚线
* **螺旋拖尾：** 沿路径螺旋变形
* **闪电拖尾：** 细亮锯齿主线 + 35% 概率随机分支
* **羽毛拖尾：** 细主轴 + 两侧斜向羽枝，自然羽毛弧度

### 颜色模式（23种）

单色 / 流动渐变 / 彩虹流动 / 暖色调流动 / 冷色调流动 / 霓虹脉冲 / 速度变色 / 流动条纹 / 火焰 / 极光 / 光标取色 / 光标混色 / 金属金 / 赛博朋克 / 粉彩 / 色相旋转 / 双色脉冲 / 星光闪烁 / 热力图 / 波纹干涉 / 色谱分裂 / 颗粒抖动 / 渐变扭曲

### 渐变系统

* **任意数量颜色：** 添加任意数量渐变颜色（最多 16 种）
* **OKLab 感知均匀插值：** 无灰暗中点，感知均匀的色彩过渡
* **256色 LUT：** 预计算查找表，零帧分配
* **流动渐变：** 渐变沿拖尾随时间流动

### 取色偏移（6种模式）

关闭 / 互补色(180°) / 类似色(30°) / 三角色(120°) / 分裂互补(150°) / 自定义角度

### 视觉特效

* **贝塞尔平滑：** Catmull-Rom 样条插值，曲线如丝般顺滑
* **运动模糊：** 历史帧叠加，透明度递减（1–5 强度）
* **增强发光：** 双层光晕（外晕+内辉），独立开关
* **头部高光 + 拖尾阴影：** 高级质感深度提示
* **速度响应宽度：** 快速移动时拖尾变宽

### 粒子物理系统

模组内置完整牛顿粒子物理引擎，每个力都有独立开关：

* **粒子质量：** 每个粒子有随机质量（Box-Muller 正态分布）。影响惯性（重=阻力小，速度保持久）、大小（∝质量^(1/3)）、生命周期和加速度（a=F/m）
* **粒子万有引力：** 牛顿万有引力定律 F=G·m₁·m₂/r² + Plummer 软化。支持双星（双星系统）或 N 体系统（2–10 个主导天体）
* **向心力漩涡：** 鼠标做曲线运动时粒子被捕获到轨道上。角动量守恒 + 开普勒速度梯度 + 轨道进动 + 3D 轨道倾角。停止运动后粒子离心甩出。两种物理模型：Rankine 漩涡或自定义
* **弹性碰撞：** 动量 + 动能守恒，按质量反比位置修正
* **洛伦兹力：** 带电粒子在磁场中做圆周运动（F=q·v×B）。正电荷逆时针，负电荷顺时针
* **库仑力：** 同号电荷相斥，异号电荷相吸（F=k·q₁·q₂/r²）。与洛伦兹力联动开启
* **布朗运动/湍流：** Perlin-like 空间连贯噪声场（非纯随机抖动）。空间相邻粒子受到相似力，产生流动感湍流
* **粘性耦合：** 邻近粒子（30px）互相拖拽速度，产生流体般的团簇行为
* **空气阻力：** 低速线性阻力 + 高速二次阻力。质量惯性 + 大小空气阻力（大粒子阻力更大）
* **自旋物理：** 粒子自旋带旋转空气阻尼（自旋速度随时间衰减）
* **弹簧（布料）：** 邻近粒子间胡克定律弹簧 + 沿连线阻尼 + 软化截止
* **环境重力：** 方向恒定加速度（角度+强度可调）
* **环境风：** 水平风 + 阵风 + 方向摆动 + 高频湍流
* **光标吸引+排斥：** 光标周围力场，半径和衰减可调

### 音乐响应框架

* **WASAPI 回环捕获：** 实时系统音频捕获（48kHz 立体声）
* **FFT 频率分析：** Cooley-Tukey FFT（256/512/1024/2048 点）+ Hann 窗
* **节拍检测：** 三种方式——能量阈值、频谱通量、多频段检测。低频权重最高
* **频段分析：** 低频/中频/高频能量级别
* **BPM 估计：** 实时节拍速度估计
* **音乐物理联动：** 节拍→速度脉冲、低频→粒子大小、音量→引力强度、节拍→漩涡能量、多频段→引力/磁场/热噪声
* **多频段独立联动：** 低频→引力，中频→磁场，高频→热噪声——全部独立可调

### 文字与 Emoji 粒子

* 自定义文字作为粒子释放（逗号分隔词组整体渲染不拆字）
* Emoji 支持（Segoe UI Emoji 字体）
* 字号可调
* 颜色跟随当前颜色模式

### 点击特效

* 点击时星爆粒子迸发（数量、半径、持续时间可调）
* 点击时扩散波纹环
* 两者可独立开关

### 本地化

* 设置界面完整支持英文、简体中文、繁体中文、日语
* 每个设置项的名称、描述和下拉选项均已翻译

### 性能

* **超级性能模式：** 解除所有粒子/形状上限和快速路径降级
* **自适应退避：** 渲染线程活跃时等待 1ms（约 1000fps 余量），闲置时 16ms（约 60fps 响应）
* **游戏检测：** 全屏 DirectX 游戏时自动隐藏
* **零 CPU 闲置：** 鼠标静止且无特效时窗口隐藏
* **后台采样：** 桌面颜色采样在独立低优先级线程运行

### 函数轨迹变量

自定义公式中可使用：`t`（归一化位置 0=头 1=尾）、`d`（距头部像素距离）、`time`（秒）。函数：sin cos exp sqrt abs。运算符：+ - * / ^。

示例：`sin(d * 0.15) * 8`，`sin(d * 0.25) * exp(0 - t * 2.5) * 10`。

### 颜色格式

自定义颜色使用十六进制 RGB，例如：`FF0000`=红，`00FF00`=绿，`0000FF`=蓝，`FFD700`=金。

### 致谢

开发者 [MCheng404](https://github.com/MCheng404)。
原始覆盖层/拖尾架构灵感来自 [TheatriChris](https://github.com/TheatriChris) 的 cursor-motion-blur mod（MIT 许可证）。
*/
// ==/WindhawkModReadme==
// ==WindhawkModSettings==
/*

# ===== 基础设置 =====
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
- tail_length: 10
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
  $description: How much the trail head lags behind cursor (0-10, 0=off).
  $description:zh-CN: 拖尾头部滞后光标的程度（0-10，0=关闭）。
  $description:zh-TW: 拖尾頭部落後游標的程度（0-10，0=關閉）。
  $description:ja-JP: トレイル先端がカーソルから遅れる度合い（0-10、0=オフ）。

# ===== 拖尾外观 =====
- trail_shape: tapered
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

# ===== 颜色设置 =====
- color_mode: single
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
- custom_color: "00BFFF"
  $name: Custom Color
  $name:zh-CN: 自定义颜色
  $name:zh-TW: 自訂顏色
  $name:ja-JP: カスタムカラー
  $description: Primary color for single/neon/hue-rotate modes. Hex RGB.
  $description:zh-CN: 单色/霓虹/色相旋转模式的主色，十六进制 RGB。
  $description:zh-TW: 單色/霓虹/色相旋轉模式的主色，十六進位 RGB。
  $description:ja-JP: 単色/ネオン/色相回転モードの基本色。Hex RGB。
- gradient_colors: "FF6B35,00BFFF,FFD700"
  $name: Gradient Colors
  $name:zh-CN: 渐变颜色
  $name:zh-TW: 漸層顏色
  $name:ja-JP: グラデーション色
  $description: Any number of colors (max 16) for gradient mode, comma-separated hex RGB. Uses OKLab perceptual interpolation with 256-color LUT. Gradient flows along the trail over time.
  $description:zh-CN: 渐变模式的颜色，任意数量（最多16种），逗号分隔十六进制RGB。使用OKLab感知均匀插值+256色LUT。渐变沿拖尾随时间流动。
  $description:zh-TW: 漸層模式的顏色，任意數量（最多16種），逗號分隔十六進位RGB。使用OKLab感知均勻插值+256色LUT。漸層沿拖尾隨時間流動。
  $description:ja-JP: グラデーションモードの色（最大16色）、カンマ区切りHex RGB。OKLab知覚補間+256色LUT使用。
- enable_cursor_color_shift: true
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

# ===== 发光与质感 =====
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
- edge_softness: 50
  $name: Edge Softness
  $name:zh-CN: 边缘柔和度
  $name:zh-TW: 邊緣柔和度
  $name:ja-JP: エッジ柔軟度
  $description: Trail edge anti-aliasing width (0=hard edge, 100=very soft).
  $description:zh-CN: 拖尾边缘抗锯齿宽度（0=硬边，100=极柔和）。
  $description:zh-TW: 拖尾邊緣反鋸齒寬度（0=硬邊，100=極柔和）。
  $description:ja-JP: トレイル端のアンチエイリアス幅（0=硬い端、100=非常に柔らかい）。
- aa_mode: smooth
  $name: Anti-Aliasing
  $name:zh-CN: 抗锯齿
  $name:zh-TW: 反鋸齒
  $name:ja-JP: アンチエイリアス
  $description: Edge anti-aliasing algorithm.
  $description:zh-CN: 边缘抗锯齿算法。
  $description:zh-TW: 邊緣抗鋸齒算法。
  $description:ja-JP: エッジのアンチエイリアス。
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
- enable_msaa: false
  $name: Enable MSAA
  $name:zh-CN: 启用 MSAA
  $name:zh-TW: 啟用 MSAA
  $name:ja-JP: MSAAを有効化
  $description: Multi-Sample Anti-Aliasing. Hardware edge smoothing.
  $description:zh-CN: 硬件多重采样抗锯齿，平滑三角形边缘。
  $description:zh-TW: 硬體多重採樣抗鋸齒。
  $description:ja-JP: ハードウェアMSAA。
- msaa_level: 4x
  $name: MSAA Level
  $name:zh-CN: MSAA 级别
  $name:zh-TW: MSAA 等級
  $name:ja-JP: MSAAレベル
  $description: MSAA sample count.
  $description:zh-CN: MSAA 采样数。
  $description:zh-TW: MSAA 取樣倍數。
  $description:ja-JP: MSAAサンプル数。
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
  $description:ja-JP: スーパーサンプリングAA。最高画質。
- ssaa_level: 2x
  $name: SSAA Scale
  $name:zh-CN: SSAA 缩放
  $name:zh-TW: SSAA 倍率
  $name:ja-JP: SSAAスケール
  $description: Render resolution scale factor.
  $description:zh-CN: 渲染分辨率缩放倍数。
  $description:zh-TW: 渲染解析度倍率。
  $description:ja-JP: レンダリング解像度倍率。
  $options:
  - 2x: 2x
  - 4x: 4x
  $options:zh-CN:
  - 2x: 2x
  - 4x: 4x
  $options:zh-TW:
  - 2x: 2x
  - 4x: 4x
  $options:ja-JP:
  - 2x: 2x
  - 4x: 4x
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
  $description:ja-JP: 背景輝度をサンプリングしてトレイル色の明るさを自動調整します。

# ===== 粒子系统 =====
- particle_mode: fadeout
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
- particle_origin: tail
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
- particle_density: 3
  $name: Particle Density
  $name:zh-CN: 粒子密度
  $name:zh-TW: 粒子密度
  $name:ja-JP: パーティクル密度
  $description: Number of particles per release (1-10).
  $description:zh-CN: 每次释放的粒子数量（1-10）。
  $description:zh-TW: 每次釋放的粒子數量（1-10）。
  $description:ja-JP: 放出ごとのパーティクル数（1-10）。
- particle_size_multiplier: 100
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
- particle_interval: 50
  $name: Particle Interval
  $name:zh-CN: 粒子释放间隔
  $name:zh-TW: 粒子釋放間隔
  $name:ja-JP: 放出間隔
  $description: Minimum interval between releases (ms, 0-2000). 0 = every frame. When <=10ms, fast-move interpolation fills gaps along the path.
  $description:zh-CN: 粒子释放的最小时间间隔（毫秒，0-2000），0=每帧生成。间隔≤10ms时启用快速移动插值，沿路径补粒子消除缝隙。
  $description:zh-TW: 粒子釋放的最小時間間隔（毫秒，0-2000），0=每影格生成。間隔≤10ms時啟用快速移動插值，沿路徑補粒子消除縫隙。
  $description:ja-JP: 放出間隔の最小値（ms、0-2000）。0=毎フレーム。
- particle_acceleration: true
  $name: Acceleration Effect
  $name:zh-CN: 加速度影响
  $name:zh-TW: 加速度影響
  $name:ja-JP: 加速度エフェクト
  $description: Particle initial velocity affected by mouse acceleration.
  $description:zh-CN: 粒子初速度受鼠标加速度影响，速度变化越大飞散越快。
  $description:zh-TW: 粒子初速度受滑鼠加速度影響，速度變化越大飛散越快。
  $description:ja-JP: パーティクル初速度がマウス加速度の影響を受けます。
- particle_shape: random
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
  $description:ja-JP: 基本回転速度（0-100）。各パーティクルで±50%のランダム差。
- enable_particle_interaction: true
  $name: Particle Interaction
  $name:zh-CN: 粒子间相互作用
  $name:zh-TW: 粒子間相互作用
  $name:ja-JP: パーティクル相互作用
  $description: Enable repulsion force between particles. Force scales with mass product, acceleration scales with 1/m. O(n²), auto-skipped when >500 particles.
  $description:zh-CN: 启用粒子之间的排斥力。力与质量乘积成正比，加速度与质量成反比。O(n²)复杂度，粒子>500时自动跳过。
  $description:zh-TW: 啟用粒子之間的排斥力。力與質量乘積成正比，加速度與質量成反比。O(n²)複雜度，粒子>500時自動跳過。
  $description:ja-JP: パーティクル間の反発力を有効にします。O(n²)、500個超で自動スキップ。
- particle_repel_distance: 25
  $name: Repel Distance
  $name:zh-CN: 排斥距离
  $name:zh-TW: 排斥距離
  $name:ja-JP: 反発距離
  $description: Distance (pixels) within which particles repel each other.
  $description:zh-CN: 粒子之间产生排斥力的距离（像素）。
  $description:zh-TW: 粒子之間產生排斥力的距離（像素）。
  $description:ja-JP: パーティクルが反発し合う距離（ピクセル）。
- particle_inter_repel_force: 15
  $name: Inter Repel Force
  $name:zh-CN: 粒子间排斥强度
  $name:zh-TW: 粒子間排斥強度
  $name:ja-JP: 反発力
  $description: Strength of inter-particle repulsion force. Multiplied by mass factor when Particle Mass is enabled.
  $description:zh-CN: 粒子之间排斥力的强度。启用粒子质量时乘以质量系数。
  $description:zh-TW: 粒子之間排斥力的強度。啟用粒子質量時乘以質量係數。
  $description:ja-JP: パーティクル間反発力の強さ。質量有効時は質量係数を乗算。
- particle_attraction: 40
  $name: Particle Attraction
  $name:zh-CN: 粒子吸附强度
  $name:zh-TW: 粒子吸附強度
  $name:ja-JP: 吸引力
  $description: How strongly particles are attracted to cursor (0-100).
  $description:zh-CN: 粒子被吸向光标的强度（0-100）。
  $description:zh-TW: 粒子被吸向游標的強度（0-100）。
  $description:ja-JP: パーティクルがカーソルに引き寄せられる強さ（0-100）。
- enable_particle_repel: true
  $name: Cursor Repulsion
  $name:zh-CN: 光标排斥力
  $name:zh-TW: 游標排斥力
  $name:ja-JP: カーソル反発
  $description: Particles near cursor are repelled, creating orbiting motion.
  $description:zh-CN: 粒子靠近光标时被排斥弹开，形成绕飞效果。
  $description:zh-TW: 粒子靠近游標時被排斥彈開，形成繞飛效果。
  $description:ja-JP: カーソル付近のパーティクルが反発され周回運動を生みます。
- particle_repel_radius: 25
  $name: Repulsion Radius
  $name:zh-CN: 排斥范围
  $name:zh-TW: 排斥範圍
  $name:ja-JP: 反発半径
  $description: Repulsion radius around cursor (pixels, 5-100).
  $description:zh-CN: 光标周围的排斥半径（像素，5-100）。
  $description:zh-TW: 游標周圍的排斥半徑（像素，5-100）。
  $description:ja-JP: カーソル周囲の反発半径（ピクセル、5-100）。
- particle_repel_force: 30
  $name: Repulsion Force
  $name:zh-CN: 排斥强度
  $name:zh-TW: 排斥強度
  $name:ja-JP: 反発力の強さ
  $description: Repulsion and perturbation strength (0-100).
  $description:zh-CN: 排斥力和随机扰动的强度（0-100）。
  $description:zh-TW: 排斥力和隨機擾動的強度（0-100）。
  $description:ja-JP: 反発力とランダム摂動の強さ（0-100）。

# ===== 文字粒子 =====
- text_content: Hi
  $name: Text Content
  $name:zh-CN: 文字内容
  $name:zh-TW: 文字內容
  $name:ja-JP: テキスト内容
  $description: Characters used when particle shape is set to Text. Supports letters, numbers, and Chinese characters. Comma-separated.
  $description:zh-CN: 粒子形状设为文字字符时使用的字符。支持字母、数字和汉字。逗号分隔。
  $description:zh-TW: 粒子形狀設為文字字元時使用的字元。支援字母、數字和漢字。逗號分隔。
  $description:ja-JP: パーティクル形状をテキストに設定時に使用する文字。英数字、漢字対応。カンマ区切り。
- text_font_size: 24
  $name: Font Size
  $name:zh-CN: 字号
  $name:zh-TW: 字級
  $name:ja-JP: フォントサイズ
  $description: Character on-screen size in pixels (10-200) when particle shape is Text.
  $description:zh-CN: 粒子形状设为文字字符时的字符屏幕大小（像素，10-200）。
  $description:zh-TW: 粒子形狀設為文字字元時的螢幕大小（像素，10-200）。
  $description:ja-JP: パーティクル形状をテキストに設定時の文字の画面サイズ（ピクセル、10-200）。
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

# ===== 形状拖尾 =====
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
  $description: Base size of shapes in pixels (5-30).
  $description:zh-CN: 形状的基础大小（像素，5-30）。
  $description:zh-TW: 形狀的基礎大小（像素，5-30）。
  $description:ja-JP: 形状の基本サイズ（ピクセル、5-30）。
- shape_lifetime: 800
  $name: Shape Lifetime
  $name:zh-CN: 形状存活时间
  $name:zh-TW: 形狀存活時間
  $name:ja-JP: 形状寿命
  $description: How long each shape lasts (ms, 200-2000).
  $description:zh-CN: 每个形状的存活时间（毫秒，200-2000）。
  $description:zh-TW: 每個形狀的存活時間（毫秒，200-2000）。
  $description:ja-JP: 各形状の持続時間（ms、200-2000）。

# ===== 特殊形状参数 =====
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
- custom_function: "sin(d * 0.15) * 8"
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

# ===== 向心力漩涡 =====
- enable_centripetal: false
  $name: Centripetal Vortex
  $name:zh-CN: 向心力漩涡
  $name:zh-TW: 向心力漩渦
  $name:ja-JP: 求心ボルテックス
  $description: Curved mouse motion captures particles into orbiting vortex. Includes angular momentum conservation, Kepler velocity gradient, orbital precession, 3D inclination.
  $description:zh-CN: 鼠标做曲线运动时粒子被捕获形成旋转漩涡。物理模型：角动量守恒、开普勒速度梯度、轨道进动、3D轨道倾角。
  $description:zh-TW: 滑鼠做曲線運動時粒子被捕獲形成旋轉漩渦。物理模型：角動量守恆、開普勒速度梯度、軌道進動、3D軌道傾角。
  $description:ja-JP: 曲線マウス運動がパーティクルを捕捉し周回ボルテックスを形成します。
- centripetal_force: 50
  $name: Vortex Force
  $name:zh-CN: 漩涡强度
  $name:zh-TW: 漩渦強度
  $name:ja-JP: ボルテックス力
  $description: Centripetal force strength, orbit speed and spring constraint (0-100).
  $description:zh-CN: 向心力强度、轨道速度和弹簧约束（0-100）。
  $description:zh-TW: 向心力強度、軌道速度和彈簧約束（0-100）。
  $description:ja-JP: 求心力の強さ、軌道速度、バネ拘束（0-100）。
- centripetal_sensitivity: 50
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
- vortex_max_count: 2
  $name: Max Vortex Count
  $name:zh-CN: 最大漩涡数量
  $name:zh-TW: 最大漩渦數量
  $name:ja-JP: 最大ボルテックス数
  $description: Maximum simultaneous vortices (1-8). New vortex only spawns when an existing one fully decays.
  $description:zh-CN: 同时存在的最大漩涡数量（1-8）。只有现有漩涡完全衰减后才能生成新漩涡。
  $description:zh-TW: 同時存在的最大漩渦數量（1-8）。只有現有漩渦完全衰減後才能生成新漩渦。
  $description:ja-JP: 同時ボルテックス最大数（1-8）。
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
  $description: Vortex velocity field model.
  $description:zh-CN: 漩涡速度场模型。
  $description:zh-TW: 漩渦速度場模型。
  $description:ja-JP: ボルテックス速度場モデル。
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

# ===== 粒子物理 =====
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
- particle_mass_max: 20
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
- enable_particle_collision: false
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
- enable_brownian_motion: false
  $name: Brownian Motion
  $name:zh-CN: 布朗运动
  $name:zh-TW: 布朗運動
  $name:ja-JP: ブラウン運動
  $description: Random thermal noise force on particles, simulating molecular heat motion.
  $description:zh-CN: 粒子受随机热噪声力，模拟分子热运动。
  $description:zh-TW: 粒子受隨機熱噪聲力，模擬分子熱運動。
  $description:ja-JP: 分子の熱運動を模擬するランダムな熱ノイズ力。
- brownian_strength: 20
  $name: Thermal Noise
  $name:zh-CN: 热噪声强度
  $name:zh-TW: 熱噪聲強度
  $name:ja-JP: 熱ノイズ強度
  $description: Brownian motion force magnitude (0-100).
  $description:zh-CN: 布朗运动力的大小（0-100）。
  $description:zh-TW: 布朗運動力的大小（0-100）。
  $description:ja-JP: ブラウン運動力の大きさ（0-100）。
- enable_multi_band_link: false
  $name: Multi-Band Link
  $name:zh-CN: 多频段联动
  $name:zh-TW: 多頻段聯動
  $name:ja-JP: マルチバンド連動
  $description: Link audio frequency bands to physics. Bass→Gravity, Mid→Magnetic Field, Treble→Thermal Noise.
  $description:zh-CN: 将音频频段独立联动到物理：低频→引力，中频→磁场，高频→热噪声。
  $description:zh-TW: 將音訊頻段獨立聯動到物理：低頻→引力，中頻→磁場，高頻→熱噪聲。
  $description:ja-JP: オーディオ帯域を物理に連動：低音→重力、中音→磁場、高音→熱ノイズ。

# ===== 环境力场 =====
- enable_env_gravity: false
  $name: Environmental Gravity
  $name:zh-CN: 环境重力
  $name:zh-TW: 環境重力
  $name:ja-JP: 環境重力
  $description: Constant global acceleration field like gravity, mass-independent.
  $description:zh-CN: 恒定全局加速度场，模拟重力，与质量无关。
  $description:zh-TW: 恆定全域加速度場，模擬重力，與質量無關。
  $description:ja-JP: 重力のような一定の全地球加速度場。質量非依存。
- env_gravity_strength: 30
  $name: Gravity Strength
  $name:zh-CN: 重力强度
  $name:zh-TW: 重力強度
  $name:ja-JP: 重力強度
  $description: Gravity acceleration strength (0-100).
  $description:zh-CN: 重力加速度强度（0-100）。
  $description:zh-TW: 重力加速度強度（0-100）。
  $description:ja-JP: 重力加速度の強さ（0-100）。
- env_gravity_angle: 90
  $name: Gravity Direction
  $name:zh-CN: 重力方向
  $name:zh-TW: 重力方向
  $name:ja-JP: 重力方向
  $description: Gravity direction in degrees. 0=right, 90=down, 180=left, -90=up.
  $description:zh-CN: 重力方向角度。0=向右，90=向下，180=向左，-90=向上。
  $description:zh-TW: 重力方向角度。0=向右，90=向下，180=向左，-90=向上。
  $description:ja-JP: 重力方向（度）。0=右、90=下、180=左、-90=上。
- enable_env_wind: false
  $name: Wind Field
  $name:zh-CN: 风场
  $name:zh-TW: 風場
  $name:ja-JP: 風場
  $description: Global wind pushes particles. Light particles blow further.
  $description:zh-CN: 全局风推动粒子，轻粒子被吹得更远。
  $description:zh-TW: 全域風推動粒子，輕粒子被吹得更遠。
  $description:ja-JP: 全体の風がパーティクルを押します。軽い粒子ほど遠くへ。
- wind_strength: 30
  $name: Wind Strength
  $name:zh-CN: 风力强度
  $name:zh-TW: 風力強度
  $name:ja-JP: 風力強度
  $description: Base wind force (0-100).
  $description:zh-CN: 基础风力（0-100）。
  $description:zh-TW: 基礎風力（0-100）。
  $description:ja-JP: 基本風力（0-100）。
- wind_turbulence: 50
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

# ===== 音乐响应 =====
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

# ===== 音乐物理联动 =====
- enable_music_physics: false
  $name: Music Physics Link
  $name:zh-CN: 音乐物理联动
  $name:zh-TW: 音樂物理聯動
  $name:ja-JP: 音楽物理連動
  $description: Link music analysis to particle physics with beat detection and pulses.
  $description:zh-CN: 将音乐分析联动到粒子物理，节拍检测和脉冲效果。
  $description:zh-TW: 將音樂分析聯動到粒子物理，節拍偵測和脈衝效果。
  $description:ja-JP: 音楽分析を粒子物理に連動させます。
- music_gravity_link: 50
  $name: Volume→Gravity
  $name:zh-CN: 音量→引力
  $name:zh-TW: 音量→引力
  $name:ja-JP: 音量→重力
  $description: Audio volume boosts particle gravity strength (0-100).
  $description:zh-CN: 总音量增强粒子引力强度（0-100）。
  $description:zh-TW: 總音量增強粒子引力強度（0-100）。
  $description:ja-JP: 音量が粒子重力強度を増幅（0-100）。
- music_beat_pulse: 50
  $name: Beat→Velocity
  $name:zh-CN: 节拍→速度
  $name:zh-TW: 節拍→速度
  $name:ja-JP: ビート→速度
  $description: Beat triggers particle velocity burst (0-100).
  $description:zh-CN: 节拍触发粒子速度爆发（0-100）。
  $description:zh-TW: 節拍觸發粒子速度爆發（0-100）。
  $description:ja-JP: ビートが粒子速度バーストを発生（0-100）。
- music_bass_size: 50
  $name: Bass→Size
  $name:zh-CN: 低频→大小
  $name:zh-TW: 低頻→大小
  $name:ja-JP: 低音→サイズ
  $description: Bass energy increases particle size (0-100).
  $description:zh-CN: 低频能量增大粒子大小（0-100）。
  $description:zh-TW: 低頻能量增大粒子大小（0-100）。
  $description:ja-JP: 低音エネルギーが粒子サイズを増加（0-100）。
- music_vortex_link: 50
  $name: Beat→Vortex
  $name:zh-CN: 节拍→漩涡
  $name:zh-TW: 節拍→漩渦
  $name:ja-JP: ビート→ボルテックス
  $description: Beat energy injects into vortex strength (0-100).
  $description:zh-CN: 节拍能量注入漩涡强度（0-100）。
  $description:zh-TW: 節拍能量注入漩渦強度（0-100）。
  $description:ja-JP: ビートエネルギーをボルテックス強度に注入（0-100）。

# ===== 点击效果 =====
- enable_click_starburst: true
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

# ===== 高级效果 =====
- enable_bezier_smooth: true
  $name: Bezier Smoothing
  $name:zh-CN: 贝塞尔曲线平滑
  $name:zh-TW: 貝茲曲線平滑
  $name:ja-JP: ベジェ平滑
  $description: Catmull-Rom spline interpolation for smoother curves.
  $description:zh-CN: 使用 Catmull-Rom 样条插值，拖尾曲线更顺滑。
  $description:zh-TW: 使用 Catmull-Rom 樣條插值，拖尾曲線更順滑。
  $description:ja-JP: Catmull-Romスプライン補間で滑らかな曲線に。
- enable_motion_blur: false
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

# ===== 物理调试 =====
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

# ===== 性能设置 =====
- super_performance_mode: false
  $name: Super Performance Mode
  $name:zh-CN: 超级性能模式
  $name:zh-TW: 超級效能模式
  $name:ja-JP: スーパーパフォーマンス
  $description: Remove all performance limits. High-end PCs only.
  $description:zh-CN: 解除所有性能上限。仅在高性能电脑上启用。
  $description:zh-TW: 解除所有效能上限。僅在高效能電腦上啟用。
  $description:ja-JP: すべてのパフォーマンス制限を解除。ハイエンドPC専用。
*/
// ==/WindhawkModSettings==
#include <windows.h>
#include <d2d1_1.h>
#include <dwrite.h>
#include <d3d11.h>
#include <dxgi1_2.h>
#include <dcomp.h>
#include <dwmapi.h>
#include <d3dcompiler.h>
#include <math.h>
#include <shellapi.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <deque>
#include <vector>
#include <unordered_map>
#include <atomic>
#include <algorithm>
// WASAPI 音频捕获 / WASAPI audio capture
#include <mmdeviceapi.h>
#include <audioclient.h>
#include <avrt.h>
#include <functiondiscoverykeys_devpkey.h>

#define GRAD_STOPS 12

// ===================== 颜色工具 =====================
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
static D2D1_COLOR_F LighterColor(D2D1_COLOR_F c, float amount = 0.55f) {
    return D2D1::ColorF(c.r + (1.0f - c.r) * amount, c.g + (1.0f - c.g) * amount, c.b + (1.0f - c.b) * amount, 1.0f);
}

// 渐变颜色系统全局变量 / Gradient color system globals (must be declared before OKLab functions)
D2D1_COLOR_F g_gradColors[16] = {{1.0f, 0.42f, 0.21f, 1.0f}, {0.0f, 0.75f, 1.0f, 1.0f}, {1.0f, 0.84f, 0.0f, 1.0f}};
int g_gradColorCount = 3;
D2D1_COLOR_F g_gradientLUT[256];  // 预计算256色查找表 / Precomputed 256-entry lookup table
bool g_gradientLUTDirty = true;   // LUT需要重建标记 / LUT rebuild flag
float g_gradientFlowSpeed = 0.15f;  // 流动速度（相位/秒）/ Flow speed (phase per second)

// ===== OKLab 感知均匀颜色空间转换 / OKLab perceptually-uniform color space conversion =====
// OKLab是W3C推荐的CSS默认渐变插值空间，感知均匀、无灰暗中点 / W3C recommended default CSS gradient interpolation space
struct OKLab { float L, a, b; };
static OKLab RGBtoOKLab(float r, float g, float b) {
    // sRGB → linear / sRGB转线性空间
    auto srgb2lin = [](float c) { return c <= 0.04045f ? c / 12.92f : powf((c + 0.055f) / 1.055f, 2.4f); };
    float lr = srgb2lin(r), lg = srgb2lin(g), lb = srgb2lin(b);
    // linear RGB → LMS / 线性RGB转LMS锥细胞响应空间
    float l = 0.4122214708f * lr + 0.5363325363f * lg + 0.0514459929f * lb;
    float m = 0.2119034982f * lr + 0.6806995451f * lg + 0.1073969566f * lb;
    float s = 0.0883024619f * lr + 0.2817188376f * lg + 0.6299787005f * lb;
    // LMS → OKLab（立方根）
    l = cbrtf(l); m = cbrtf(m); s = cbrtf(s);
    return { 0.2104542553f * l + 0.7936177850f * m - 0.0040720468f * s,
             1.9779984951f * l - 2.4285922050f * m + 0.4505937099f * s,
             0.0259040371f * l + 0.7827717662f * m - 0.8086757660f * s };
}
static D2D1_COLOR_F OKLabtoRGB(float L, float a, float b) {
    // OKLab → LMS / OKLab转回LMS空间
    float l = L + 0.3963377774f * a + 0.2158037573f * b;
    float m = L - 0.1055613458f * a - 0.0638541728f * b;
    float s = L - 0.0894841775f * a - 1.2914855480f * b;
    l = l * l * l; m = m * m * m; s = s * s * s;
    // LMS → linear RGB / LMS转回线性RGB
    float lr =  4.0767416621f * l - 3.3077115913f * m + 0.2309699292f * s;
    float lg = -1.2684380046f * l + 2.6097574011f * m - 0.3413193965f * s;
    float lb = -0.0041960863f * l - 0.7034186147f * m + 1.7076147010f * s;
    // linear → sRGB / 线性空间转回sRGB
    auto lin2srgb = [](float c) { return c <= 0.0031308f ? 12.92f * c : 1.055f * powf(c, 1.0f / 2.4f) - 0.055f; };
    lr = fminf(fmaxf(lin2srgb(lr), 0), 1);
    lg = fminf(fmaxf(lin2srgb(lg), 0), 1);
    lb = fminf(fmaxf(lin2srgb(lb), 0), 1);
    return D2D1::ColorF(lr, lg, lb, 1.0f);
}
// OKLab 空间插值（感知均匀，无灰暗中点）/ OKLab-space interpolation (perceptually uniform, no muddy midpoint)
static D2D1_COLOR_F LerpColorOKLab(D2D1_COLOR_F a, D2D1_COLOR_F b, float t) {
    OKLab ca = RGBtoOKLab(a.r, a.g, a.b), cb = RGBtoOKLab(b.r, b.g, b.b);
    return OKLabtoRGB(ca.L + (cb.L - ca.L) * t, ca.a + (cb.a - ca.a) * t, ca.b + (cb.b - ca.b) * t);
}
// 预计算256色渐变LUT（OKLab空间，多色点均匀分布）/ Precompute 256-entry gradient LUT (OKLab, evenly distributed color stops)
static void RebuildGradientLUT() {
    if (g_gradColorCount < 1) { g_gradientLUT[0] = D2D1::ColorF(1,1,1,1); return; }
    if (g_gradColorCount == 1) { for (int i = 0; i < 256; i++) g_gradientLUT[i] = g_gradColors[0]; return; }
    for (int i = 0; i < 256; i++) {
        float pos = (float)i / 255.0f * (g_gradColorCount - 1);
        int idx = (int)pos;
        float frac = pos - idx;
        if (idx >= g_gradColorCount - 1) { g_gradientLUT[i] = g_gradColors[g_gradColorCount - 1]; continue; }
        g_gradientLUT[i] = LerpColorOKLab(g_gradColors[idx], g_gradColors[idx + 1], frac);
    }
    g_gradientLUTDirty = false;
}
// 从LUT采样渐变颜色（支持流动相位）/ Sample gradient from LUT (supports flowing phase)
static D2D1_COLOR_F SampleGradientLUT(float ratio, float flowPhase) {
    if (g_gradientLUTDirty) RebuildGradientLUT();
    float pos = fmodf(ratio + flowPhase, 1.0f);
    if (pos < 0) pos += 1.0f;
    int idx = (int)(pos * 255.0f + 0.5f);
    if (idx < 0) idx = 0; if (idx > 255) idx = 255;
    return g_gradientLUT[idx];
}
// 安全获取渐变颜色（循环索引）/ Safe gradient color access (wrapping index)
static D2D1_COLOR_F GetGradColor(int idx) {
    if (g_gradColorCount <= 0) return D2D1::ColorF(1,1,1,1);
    return g_gradColors[idx % g_gradColorCount];
}
static D2D1_COLOR_F ParseHexColor(PCWSTR hex, D2D1_COLOR_F fallback) {
    if (!hex || !*hex)
        return fallback;
    // Validate: must be exactly 6 hex digits / 校验：必须恰好6位十六进制
    int len = 0;
    while (hex[len] && len < 8) {
        WCHAR c = hex[len];
        if (!((c >= L'0' && c <= L'9') || (c >= L'a' && c <= L'f') || (c >= L'A' && c <= L'F')))
            return fallback;
        len++;
    }
    if (len != 6)
        return fallback;
    DWORD val = wcstoul(hex, nullptr, 16);
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
    while (tok && idx < 16) {
        while (*tok == L' ' || *tok == L'\t')
            tok++;
        D2D1_COLOR_F c = ParseHexColor(tok, g_gradColors[idx]);
        g_gradColors[idx++] = c;
        tok = wcstok_s(nullptr, L",", &ctx);
    }
    if (idx > 0) g_gradColorCount = idx;
    g_gradientLUTDirty = true;
}

// ===================== 数学表达式解析器 =====================
enum ExprTokType { ET_NUM, ET_VAR, ET_FUNC, ET_OP, ET_LPAREN, ET_RPAREN };
struct ExprToken {
    ExprTokType type;
    float num;
    char name[16];
};

static std::vector<ExprToken> g_exprRPN;
static bool g_exprValid = false;

static int OpPrec(char op) {
    if (op == '+' || op == '-')
        return 1;
    if (op == '*' || op == '/')
        return 2;
    if (op == '^')
        return 4;
    return 0;
}

static void CompileExpression(const char *expr) {
    g_exprValid = false;
    g_exprRPN.clear();
    if (!expr || !*expr)
        return;
    char buf[512];
    int bi = 0;
    bool expectVal = true;
    for (int i = 0; expr[i] && bi < 510; i++) {
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
        expectVal = (c == '(' || c == '+' || c == '-' || c == '*' || c == '/' || c == '^');
    }
    buf[bi] = 0;
    std::vector<ExprToken> tokens;
    int i = 0;
    while (buf[i]) {
        if (isdigit((unsigned char)buf[i]) || buf[i] == '.') {
            float val = 0;
            int dec = 0;
            float div = 1;
            while (isdigit((unsigned char)buf[i]) || buf[i] == '.') {
                if (buf[i] == '.')
                    dec = 1;
                else if (dec) {
                    div *= 10;
                    val += (buf[i] - '0') / div;
                } else
                    val = val * 10 + (buf[i] - '0');
                i++;
            }
            tokens.push_back({ET_NUM, val, ""});
        } else if (isalpha((unsigned char)buf[i])) {
            char name[16] = {0};
            int j = 0;
            while (isalnum((unsigned char)buf[i]) && j < 15)
                name[j++] = buf[i++];
            if (buf[i] == '(')
                tokens.push_back({ET_FUNC, 0, ""}), strcpy_s(tokens.back().name, name);
            else
                tokens.push_back({ET_VAR, 0, ""}), strcpy_s(tokens.back().name, name);
        } else if (buf[i] == '(') {
            tokens.push_back({ET_LPAREN, 0, ""});
            i++;
        } else if (buf[i] == ')') {
            tokens.push_back({ET_RPAREN, 0, ""});
            i++;
        } else if (strchr("+-*/^", buf[i])) {
            tokens.push_back({ET_OP, 0, ""});
            tokens.back().name[0] = buf[i];
            tokens.back().name[1] = 0;
            i++;
        } else
            i++;
    }
    std::vector<ExprToken> output, stack;
    for (auto &tok : tokens) {
        if (tok.type == ET_NUM || tok.type == ET_VAR)
            output.push_back(tok);
        else if (tok.type == ET_FUNC)
            stack.push_back(tok);
        else if (tok.type == ET_OP) {
            while (!stack.empty() && stack.back().type != ET_LPAREN && stack.back().type != ET_FUNC &&
                   OpPrec(stack.back().name[0]) >= OpPrec(tok.name[0])) {
                output.push_back(stack.back());
                stack.pop_back();
            }
            stack.push_back(tok);
        } else if (tok.type == ET_LPAREN)
            stack.push_back(tok);
        else if (tok.type == ET_RPAREN) {
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
    g_exprRPN = output;
    g_exprValid = !output.empty();
}

static float EvalExpression(float t, float d, float time) {
    if (!g_exprValid || g_exprRPN.empty())
        return 0;
    float stk[64];
    int sp = 0;
    for (auto &tok : g_exprRPN) {
        if (sp >= 63)
            break;
        if (tok.type == ET_NUM)
            stk[sp++] = tok.num;
        else if (tok.type == ET_VAR) {
            if (strcmp(tok.name, "t") == 0)
                stk[sp++] = t;
            else if (strcmp(tok.name, "d") == 0)
                stk[sp++] = d;
            else if (strcmp(tok.name, "time") == 0)
                stk[sp++] = time;
            else if (strcmp(tok.name, "pi") == 0)
                stk[sp++] = 3.14159265f;
            else if (strcmp(tok.name, "e") == 0)
                stk[sp++] = 2.7182818f;
            else
                stk[sp++] = 0;
        } else if (tok.type == ET_OP) {
            if (sp < 2) {
                sp = 0;
                break;
            }
            float b = stk[--sp], a = stk[--sp], r = 0;
            switch (tok.name[0]) {
                case '+':
                    r = a + b;
                    break;
                case '-':
                    r = a - b;
                    break;
                case '*':
                    r = a * b;
                    break;
                case '/':
                    r = (b != 0) ? a / b : 0;
                    break;
                case '^':
                    r = powf(fabsf(a) + 0.0001f, b);
                    break;
            }
            stk[sp++] = r;
        } else if (tok.type == ET_FUNC) {
            if (sp < 1) {
                sp = 0;
                break;
            }
            float a = stk[--sp], r = 0;
            if (strcmp(tok.name, "sin") == 0)
                r = sinf(a);
            else if (strcmp(tok.name, "cos") == 0)
                r = cosf(a);
            else if (strcmp(tok.name, "tan") == 0)
                r = tanf(a);
            else if (strcmp(tok.name, "exp") == 0)
                r = expf(a);
            else if (strcmp(tok.name, "sqrt") == 0)
                r = sqrtf(fabsf(a));
            else if (strcmp(tok.name, "abs") == 0)
                r = fabsf(a);
            else if (strcmp(tok.name, "log") == 0)
                r = logf(fabsf(a) + 0.0001f);
            stk[sp++] = r;
        }
    }
    return sp > 0 ? stk[sp - 1] : 0;
}

// ===================== 全局状态 =====================
HWND g_overlayHwnd = NULL;
// 自定义消息：在 UI 线程显示/隐藏覆盖窗口（渲染线程通过 PostMessage 发送，避免跨线程窗口操作）
// Custom messages: show/hide overlay on UI thread (render thread posts via PostMessage to avoid cross-thread window ops)
constexpr UINT WM_APP_SHOW_OVERLAY = WM_USER + 101;
constexpr UINT WM_APP_HIDE_OVERLAY = WM_USER + 102;
HANDLE g_threadHandle = NULL;       // UI 线程句柄
HANDLE g_readyEvent = NULL;         // 窗口创建完成事件
HANDLE g_renderExitEvent = NULL;    // 渲染线程退出事件
HANDLE g_renderThread = NULL;       // 渲染线程句柄
std::deque<POINT> g_history;
POINT g_lastPos = {0, 0};

int g_trailDelay = 0;
POINT g_lagPos = {0, 0};
bool g_lagInited = false;

float g_fadeAlpha = 1.0f;

D2D1_COLOR_F g_cursorExtractedColor = {0.5f, 0.5f, 0.5f, 1.0f};
bool g_cursorColorShift = true;
int g_colorShiftMode = 1;       // 0=off, 1=complementary(180), 2=analogous(30), 3=triadic(120), 4=split(150), 5=custom
int g_colorShiftAngle = 180;    // 自定义偏移角度
float g_colorShiftSatBoost = 0.55f;  // 饱和度保底
float g_colorShiftValBoost = 0.72f;  // 亮度保底
static DWORD s_lastColorExtract = 0;

// ===================== 后台采样线程（GDI 回读剥离，避免阻塞渲染线程 vsync）=====================
// GetDC(NULL)/BitBlt 在 DWM 下触发全屏 GPU→CPU 回读，放在独立低优先级线程执行
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

// ===================== 渲染设备（D3D11 + D2D1 Device + DirectComposition）=====================
ID3D11Device *g_pD3DDevice = nullptr;
ID3D11DeviceContext *g_pD3DContext = nullptr;
IDXGIDevice *g_pDXGIDevice = nullptr;
ID2D1Factory1 *g_pD2DFactory = nullptr;
// DirectWrite 文字粒子云 / DirectWrite text particle cloud
IDWriteFactory *g_dwFactory = nullptr;
IDWriteTextFormat *g_pTextFormat = nullptr;     // 文字粒子格式 / text particle format
std::unordered_map<wchar_t, ID2D1Bitmap*> g_charBitmaps;  // 字符预渲染缓存 / pre-rendered char bitmap cache
int g_cachedFontSize = 0;                        // 缓存的字号 / cached font size
std::vector<D2D1_POINT_2F> g_textPoints;       // 文字轮廓采样点云（相对中心）/ sampled outline points (relative to center)
bool g_textPointsDirty = true;                  // 文字或字号改变后需重建 / mark rebuild needed
static void BuildTextPoints();
wchar_t g_textContent[256] = L"Hi";             // 文字内容 / text content
int g_textFontSize = 120;                       // 字号 / font size
bool g_enableTextChunk = true;                  // 长句分段开关 / long-text chunking toggle
int  g_textChunkMode = 0;                       // 0=逐粒子 particle, 1=全局同步 global
int  g_textChunkMax = 3;                        // 每段最多码点数 / max codepoints per chunk
int  g_textChunkDelay = 600;                    // 分段切换延迟(ms) / chunk switch delay (ms)
int  g_textOffsetX = 0;                         // 文字相对光标 X 偏移 / text spawn X offset
int  g_textOffsetY = 0;                         // 文字相对光标 Y 偏移 / text spawn Y offset
int g_textSpacing = 4;                          // 采样点间距（像素）/ sample spacing (px)
bool g_enableTextParticles = false;             // 文字粒子开关 / text particles toggle
ID2D1Device *g_pD2DDevice = nullptr;
ID2D1DeviceContext *g_pD2DDC = nullptr;
IDXGISwapChain1 *g_pSwapChain = nullptr;
bool g_isHDRMode = false;  // 当前是否HDR模式
DXGI_FORMAT g_swapChainFormat = DXGI_FORMAT_B8G8R8A8_UNORM;  // 当前交换链格式
ID2D1Bitmap1 *g_pD2DTargetBitmap = nullptr;
ID3D11RenderTargetView *g_pCachedRTV = nullptr;  // 缓存的后台缓冲 RTV（随交换链重建，避免每帧创建）
int g_msaaSamples = 1;  // MSAA 采样数 1=off / 2 / 4 / 8
ID3D11Texture2D *g_pMSAATexture = nullptr;  // MSAA 离屏渲染纹理 / MSAA offscreen texture
ID3D11RenderTargetView *g_pMSAARTV = nullptr;  // MSAA RTV / MSAA render target view
int g_ssaaScale = 1;  // SSAA 缩放倍数 1=off / 2 / 4
int g_createdMsaaSamples = 1;  // 离屏纹理实际创建时的 MSAA 配置 / MSAA config the offscreen resources were created with
int g_createdSsaaScale = 1;    // 离屏纹理实际创建时的 SSAA 配置 / SSAA config the offscreen resources were created with
int g_renderW = 0, g_renderH = 0;  // 实际渲染分辨率（SSAA 时放大）/ Actual render resolution (scaled for SSAA)
ID3D11Texture2D *g_pSSAATexture = nullptr;  // SSAA 离屏渲染纹理 / SSAA offscreen RT
ID3D11RenderTargetView *g_pSSAARTV = nullptr;  // SSAA RTV
ID3D11ShaderResourceView *g_pSSAASRV = nullptr;  // SSAA SRV (for blit)
ID3D11Texture2D *g_pSSAAResolveTexture = nullptr;  // MSAA resolve 目标（仅MSAA+SSAA时）/ MSAA resolve target (only when MSAA+SSAA)
ID3D11ShaderResourceView *g_pSSAAResolveSRV = nullptr;  // resolve 纹理 SRV / resolve texture SRV
ID3D11VertexShader *g_pBlitVS = nullptr;
ID3D11PixelShader *g_pBlitPS = nullptr;
ID3D11Buffer *g_pBlitVB = nullptr;  // 全屏四边形 VB / fullscreen quad VB
ID3D11InputLayout *g_pBlitLayout = nullptr;  // blit 输入布局 / blit input layout
ID3D11SamplerState *g_pBlitSampler = nullptr;
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
// 每帧动态创建的 mesh（ID2D1Mesh 只写一次，不能复用 Open）
ID2D1Mesh *g_pShadowMesh = nullptr;
ID2D1Mesh *g_pGlow2Mesh = nullptr;
ID2D1Mesh *g_pGlowMesh = nullptr;
ID2D1Mesh *g_pOuterMesh = nullptr;
ID2D1Mesh *g_pInnerMesh = nullptr;

int g_cachedVW = 0, g_cachedVH = 0;
int g_virtX = 0, g_virtY = 0, g_virtW = 0, g_virtH = 0;  // 虚拟屏幕尺寸缓存，WM_DISPLAYCHANGE 时更新

// ===================== 设置缓存 =====================
float g_triggerVelocity = 25.0f, g_stopVelocity = 10.0f;
int g_tailOffsetX = 6, g_tailOffsetY = 10, g_tailLength = 10;
bool g_enableSmoothGradient = true;
int g_fadeoutMode = 2;  // 0=hard 1=accelerate 2=soft
bool g_enableSpeedResponse = true;
bool g_enhancedGlow = true;
bool g_enableHeadHighlight = true;
bool g_adaptiveContrast = false;   // 自适应对比度
float g_bgLuminance = 0.5f;        // 背景亮度缓存（0=暗，1=亮）
DWORD g_lastBgSample = 0;          // 上次背景采样时间
bool g_enableTrailShadow = true;
int g_trailShape = 0;
int g_dotsMultiplier = 2;
int g_dotChainSize = 100;         // 圆点链大小倍数（百分比）
int g_particleSizeMultiplier = 100; // 粒子大小倍数（百分比）
bool g_enableParticleGlow = true;   // 粒子发光开关
int g_particleGlowIntensity = 40;   // 粒子发光强度（0-100）
int g_functionPreset = 0;
char g_customFunction[256] = "sin(d * 0.15) * 8";
int g_waveAmplitude = 8, g_waveFrequency = 15;
bool g_enableGlow = true;
int g_glowIntensity = 40;
int g_edgeSoftness = 50;  // 拖尾边缘柔和度（0=硬边，100=极柔和）
int g_aaMode = 1;  // 抗锯齿模式 0=off 1=smooth 2=crisp 3=extra / AA mode: 0=off 1=smooth 2=crisp 3=extra
int g_colorMode = 0;
float g_gradientWarp = 0.5f;  // 渐变扭曲模式的中间色位置（0~1）
D2D1_COLOR_F g_customColor = {0.0f, 0.75f, 1.0f, 1.0f};
int g_particleMode = 1;
int g_particleOrigin = 2;  // 0=head 1=middle 2=tail 3=custom
int g_particleOriginRatio = 80;
float g_particleAttraction = 0.032f;
bool g_enableParticleRepel = true;
int g_particleRepelRadius = 25;
float g_particleRepelForce = 0.9f;
int g_particleDensity = 3;
int g_particleInterval = 50;
bool g_particleAccel = true;
int g_particleShape = 9;  // 0=circle,1=star,2=hexagram,3=heart,4=diamond,5=triangle,6=flower,7=pentagon,8=hexagon,9=random / 形状编码与HLSL一致
bool g_enableParticleSpin = true;  // 粒子自旋转
int g_particleSpinSpeed = 30;  // 粒子自旋速度（0-100）
bool g_enableParticleInteraction = true;  // 粒子间相互作用
int g_interParticleRepelDistance = 25;  // 粒子间排斥距离（像素）
float g_interParticleRepelForce = 0.15f;  // 粒子间排斥力强度
// 粒子质量系统 / Particle mass system
bool g_enableParticleMass = true;
float g_particleMassMin = 0.5f;
float g_particleMassMax = 2.0f;
// 粒子万有引力系统 / Particle universal gravitation
bool g_enableParticleGravity = false;
float g_gravityStrength = 0.5f;  // 引力常数G（0-1）/ Gravitational constant G (0-1)
int g_gravitySystem = 0;  // 0=双星, 1=N体 / 0=binary star, 1=N-body
int g_gravityBodyCount = 3;  // N体数量 / N-body count
// 引力天体（双星/N体的大质量粒子索引）/ Gravity bodies (high-mass particle indices for binary/N-body)
std::vector<int> g_gravityBodies;
// 引力计算复用工作缓冲（避免每帧堆分配）/ Reusable scratch buffers for gravity
std::vector<std::pair<float, int>> g_gravityMassIdx;
std::vector<bool> g_gravityBodyFlag;

// ===================== 音乐响应系统（v3.4 基础设施）===================== / Music reactive system (v3.4 infrastructure, visual effects TBD)
struct MusicState {
    bool initialized = false;
    bool capturing = false;
    // WASAPI 接口 / WASAPI interfaces
    IMMDeviceEnumerator *pEnumerator = nullptr;
    IMMDevice *pDevice = nullptr;
    IAudioClient *pAudioClient = nullptr;
    IAudioCaptureClient *pCaptureClient = nullptr;
    HANDLE hCaptureThread = nullptr;
    HANDLE hStopEvent = nullptr;
    std::atomic<bool> captureRunning{false};
    // 音频格式 / Audio format
    WAVEFORMATEX *pWaveFormat = nullptr;
    UINT32 bufferFrames = 0;
    // FFT 数据 / FFT data
    int fftSize = 512;
    std::vector<float> fftInput;       // 时域输入（环形缓冲）
    std::vector<float> fftOutput;      // 频域输出（幅度）
    std::vector<float> fftSmoothed;    // 平滑后的频域
    std::vector<float> windowFunction; // Hann 窗函数
    std::vector<float> fftWorkReal;    // FFT工作缓冲（实部，复用避免每帧分配）
    std::vector<float> fftWorkImag;    // FFT工作缓冲（虚部，复用避免每帧分配）
    int fftWritePos = 0;               // 环形缓冲写入位置
    // 节拍检测 / Beat detection
    float beatEnergy = 0;              // 当前帧能量 / Current frame energy
    float beatAvgEnergy = 0;           // 平均能量（历史）/ Average energy (history)
    float beatThreshold = 0;           // 节拍阈值 / Beat threshold
    bool beatDetected = false;         // 当前帧是否检测到节拍 / Beat detected this frame
    float beatIntensity = 0;           // 节拍强度（0-1）/ Beat intensity (0-1)
    DWORD lastBeatTime = 0;            // 上次节拍时间 / Last beat time
    float bpmEstimate = 0;             // BPM估计 / BPM estimate
    std::deque<DWORD> beatHistory;     // 节拍时间历史 / Beat time history
    // 频谱通量检测 / Spectral flux detection
    std::vector<float> prevSpectrum;   // 上一帧频谱
    float spectralFlux = 0;            // 当前频谱通量
    float avgSpectralFlux = 0;         // 平均频谱通量
    // 多频段节拍 / Multi-band beat detection
    float bassBeatEnergy = 0;          // 低频节拍能量
    float bassBeatAvg = 0;             // 低频平均能量
    bool bassBeat = false;             // 低频节拍
    float midBeatEnergy = 0;           // 中频节拍能量
    float midBeatAvg = 0;              // 中频平均能量
    bool midBeat = false;              // 中频节拍
    float trebleBeatEnergy = 0;        // 高频节拍能量
    float trebleBeatAvg = 0;           // 高频平均能量
    bool trebleBeat = false;           // 高频节拍
    // 频段数据（低/中/高）/ Frequency band data (bass/mid/treble)
    float bassLevel = 0;    // 低频 (20-250Hz) / Bass (20-250Hz)
    float midLevel = 0;     // 中频 (250-2000Hz) / Mid (250-2000Hz)
    float trebleLevel = 0;  // 高频 (2000-20000Hz) / Treble (2000-20000Hz)
    float overallLevel = 0; // 总音量 / Overall volume
};
MusicState g_music;
bool g_enableMusicReactive = false;
int g_musicFftSize = 512;
float g_musicSensitivity = 0.5f;
float g_musicSmoothing = 0.5f;
// 音乐物理联动 / Music-physics link
bool g_enableMusicPhysics = false;
float g_musicGravityLink = 0.5f;   // 音量→引力
float g_musicBeatPulse = 0.5f;     // 节拍→速度
float g_musicBassSize = 0.5f;      // 低频→大小
float g_musicVortexLink = 0.5f;    // 节拍→漩涡
float g_musicBeatPulseAmount = 0;  // 当前节拍脉冲量（衰减）
float g_musicBassSizeBoost = 0;    // 当前低频大小增益（平滑）
// 高级物理系统 / Advanced physics
bool g_enableParticleCollision = false;
bool g_enableLorentzForce = false;
float g_lorentzStrength = 0.3f;    // 磁场强度
bool g_enableBrownianMotion = false;
float g_brownianStrength = 0.2f;   // 热噪声强度
bool g_enableMultiBandLink = false;
// 环境力场：全局重力 + 风场 / Environmental forces: global gravity + wind field
bool g_enableEnvGravity = false;
float g_envGravityStrength = 0.3f;  // 重力加速度（像素/帧2）/ gravity accel (px/frame^2)
float g_envGravityAngle = 1.5708f;  // 重力方向弧度，默认90度向下 / gravity angle rad, default 90 deg downward
bool g_enableEnvWind = false;
float g_windStrength = 0.2f;        // 风力基准强度 / baseline wind strength
float g_windTurbulence = 0.5f;      // 湍流程度 0-1 / turbulence amount 0-1
float g_windX = 0, g_windY = 0;     // 当前风向量（每帧更新）/ current wind vector (updated per frame)
// 布料弹簧网：邻近粒子间软弹簧，形成果冻/布料抖动 / Cloth spring net: soft springs between nearby particles, jelly/cloth wobble
bool g_enableParticleSpring = false;
float g_springK = 0.02f;            // 弹簧刚度 / spring stiffness
float g_springMaxDist = 50.0f;      // 建立弹簧的最大距离（像素）/ max distance to form spring (px)
std::vector<int> g_springLinkCount;  // 每粒子当前帧弹簧连接数（防中心过压）/ per-particle spring link count (anti center over-constraint)
// 物理可视化调试（独立开关）/ Physics debug visualization (individual toggles)
bool g_debugVelocity = false;   // 速度向量
bool g_debugForce = false;      // 受力向量
bool g_debugVortex = false;     // 漩涡中心/轨道
bool g_debugGravity = false;    // 引力源
bool g_debugCollision = false;  // 碰撞半径
bool g_debugSpring = false;     // 弹簧连线
// 多频段联动状态 / Multi-band link state
float g_bassGravityBoost = 0;      // 低频→引力
float g_midRepelBoost = 0;         // 中频→排斥力
float g_trebleNoiseBoost = 0;      // 高频→噪声

// 向心力漩涡系统 / Centripetal vortex system
bool g_enableCentripetal = false;
float g_centripetalForce = 0.5f;  // 向心力强度（0-1）
float g_centripetalSensitivity = 0.5f;  // 检测灵敏度（0-1，越低越灵敏）
int g_centripetalDuration = 1500;  // 漩涡持续时间（ms）
int g_vortexMaxCount = 2;      // 最大同时存在的漩涡数量（1-8）
float g_vortexMinDistance = 200.0f;  // 漩涡之间最小间距（像素）
float g_vortexDrift = 0.3f;     // 漩涡中心漂移程度（0-1）
float g_vortexDurationSpeed = 0.2f;  // 持续时间速度增益（0-1）
int g_vortexPhysModel = 0;      // 漩涡物理模型：0=Rankine, 1=自由涡, 2=刚体旋转, 3=Lamb-Oseen, 4=开普勒

#define MAX_VORTICES 8
// 单个漩涡实例 / Single vortex instance
struct Vortex {
    bool active;           // 是否活跃（包括衰减阶段）
    float centerX, centerY; // 漩涡中心
    float driftX, driftY;   // 中心漂移速度
    float radius;          // 轨道半径（涡核外边界）
    float coreRadius;      // 涡核半径（Rankine涡内边界，内部刚体旋转）
    float angularVel;      // 角速度（涡核内刚体旋转角速度）
    float circulation;     // 环量Γ（自由涡强度，v_θ = Γ/(2πr)）
    float strength;        // 强度（0-1，平滑过渡）
    float pressureGradient; // 径向压力梯度强度（中心低压吸力）
    bool isCircling;       // 是否在维持（鼠标还在圆周运动）
    DWORD startTime;       // 创建时间
    int actualDuration;    // 实际持续时间（受速度影响）
    float orbitTilt;       // 轨道倾角
    float orbitTiltVel;    // 倾角变化速度
    float orbitPhase;      // 轨道相位
    float stretchRate;     // 涡管拉伸率（鼠标加速度导致，增强涡量）
};
Vortex g_vortices[MAX_VORTICES] = {};

// 圆周运动检测状态（位置历史共享，检测后分配到漩涡槽位）/ Circular motion detection state (position history shared, assigned to vortex slot after detection)
struct CircleDetect {
    float historyX[20];  // 位置历史
    float historyY[20];
    int historyCount;
    float angularVel;  // 角速度（弧度/帧）
    float circleCenterX, circleCenterY;  // 估算圆心
    float circleRadius;  // 估算半径
    bool isCircling;  // 是否在做圆周运动
    DWORD lastCircleTime;  // 上次检测到圆周运动的时间
} g_circleDetect = {};
ID2D1PathGeometry *g_pStarGeom = nullptr;
ID2D1PathGeometry *g_pHexagramGeom = nullptr;
ID2D1PathGeometry *g_pHeartGeom = nullptr;

// ===== 形状拖尾 =====
struct TrailShape {
    float x, y;
    float vx, vy;
    float size;
    float rotation;      // 当前旋转角度
    float rotSpeed;      // 旋转速度
    int shapeType;       // 0=heart 1=star 2=hexagon 3=circle 4=diamond 5=triangle 6=flower 7=pentagon 8=hexagram
    DWORD startTime;
    float lifetime;
    D2D1_COLOR_F color;
};
std::vector<TrailShape> g_trailShapes;
float g_lastShapeX = 0, g_lastShapeY = 0;
bool g_hasLastShapePos = false;
int g_shapeType = 0;          // 0=heart 1=star 2=hexagon 3=circle 4=diamond 5=triangle 6=flower 7=pentagon 8=hexagram 9=random
int g_shapeInterval = 30;     // 生成间隔（像素）
bool g_shapeRandomOffset = true;
int g_shapeCount = 1;         // 每次生成数量
float g_shapeSize = 12.0f;
int g_shapeLifetime = 800;    // 存活时间 ms
bool g_superPerformanceMode = false;  // 超级性能模式：无视性能上限
bool g_enableBezierSmooth = true;     // 贝塞尔曲线平滑
bool g_enableMotionBlur = false;      // 运动模糊
int g_motionBlurStrength = 3;         // 运动模糊强度（叠加帧数）
bool g_enable25DEffect = true;        // 2.5D 立体效果
int g_perspectiveStrength = 15;       // 透视强度（0-50）
// 运动模糊历史帧缓冲区 / Motion blur history frame buffer
struct TrailFrame {
    std::vector<D2D1_POINT_2F> path;
    DWORD time;
};
std::vector<TrailFrame> g_trailHistory;
std::atomic<bool> g_settingsDirty{false};  // 设置变更标记，渲染线程中消费
std::atomic<bool> g_deviceLost{false};      // 设备丢失标记，渲染循环中重建
DWORD g_lastParticleTime = 0;
float g_lastParticleX = 0, g_lastParticleY = 0;
bool g_hasLastParticlePos = false;
float g_prevVelocity = 0;
bool g_enableClickStarburst = true;
int g_starburstCount = 8;
bool g_enableClickEffect = true;
int g_clickMaxRadius = 40, g_clickDuration = 300;

// ===================== 粒子系统 =====================
struct Particle {
    float x, y, vx, vy, size;
    float z;            // 3D 深度（生成时随机，避免每帧闪烁）
    float mass;         // 质量（影响惯性、大小、生命周期、引力）
    float charge;       // 电荷（洛伦兹力用，正/负）
    float rotation;     // 当前旋转角度（弧度）
    float spinSpeed;    // 自旋转速度（弧度/帧）
    DWORD startTime;
    int lifetime;
    D2D1_COLOR_F color;
    D2D1_COLOR_F endColor;
    int shapeType;
    wchar_t textChar; // 文字形状时的字符 / character for text shape
    float charIndex;  // 字符图集索引 / char atlas index
    int chunkStart;   // 所属词组的起始图集格 / first atlas cell of owning phrase
    int chunkCount;   // 所属词组占用的格数（>1 时启用轮播）/ atlas cells of owning phrase (>1 enables cycling)
    float colorOffset[3]; // 随机偏色（RGB，约 ±20/255），生成后不变
    float vortexBrightness[3]; // 漩涡亮度调制（向心力/3D效果），与色差分离
    float debugForceX, debugForceY; // 调试用：本帧总受力（物理可视化）
};
std::vector<Particle> g_particles;
struct Ripple {
    POINT pos;
    DWORD startTime;
};
std::vector<Ripple> g_ripples;
bool g_prevLButton = false, g_prevRButton = false;

// ===================== 颜色缓存 =====================
struct GradData {
    D2D1_GRADIENT_STOP outer[GRAD_STOPS];
    D2D1_GRADIENT_STOP inner[GRAD_STOPS];
    D2D1_COLOR_F solidOuter, solidInner;
};
static GradData s_cachedGrad = {};
static bool s_gradValid = false;

static bool ColorApproxEq(D2D1_COLOR_F a, D2D1_COLOR_F b) {
    return fabsf(a.r - b.r) < 0.02f && fabsf(a.g - b.g) < 0.02f && fabsf(a.b - b.b) < 0.02f;
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

static void ComputeColors(int mode, DWORD time, float velocity, GradData &out) {
    float t = time / 1000.0f;
    D2D1_COLOR_F headOuter, headInner, tailOuter, tailInner;
    switch (mode) {
        case 0:
            headOuter = tailOuter = g_customColor;
            headInner = tailInner = LighterColor(g_customColor);
            break;
        case 1: { // 流动渐变：任意数量颜色，OKLab插值，颜色沿拖尾流动 / Flowing gradient: any color count, OKLab interpolation, colors flow along trail
            float phase = fmodf(t * g_gradientFlowSpeed, 1.0f);
            headOuter = SampleGradientLUT(0.0f, phase);
            tailOuter = SampleGradientLUT(1.0f, phase);
            headInner = LighterColor(headOuter, 0.5f);
            tailInner = LighterColor(tailOuter, 0.5f);
            break;
        }
        case 2: { // 彩虹流动：多频率色相旋转 + 速度驱动饱和度 / Rainbow flow: multi-frequency hue rotation + velocity-driven saturation
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
        case 3: { // 暖色调流动：红橙黄范围 + 火焰式亮度脉动 / Warm flow: red-orange-yellow + flame-like brightness pulse
            float h = fmodf(t * 25, 60) - 15;  // -15~45: 红-橙-黄更广范围
            float flicker = 0.85f + 0.15f * sinf(t * 8.0f) + 0.08f * sinf(t * 17.0f);
            headOuter = HSVtoRGB(h, 0.95f, flicker);
            tailOuter = HSVtoRGB(fmodf(h + 25, 360), 0.85f, flicker * 0.85f);
            headInner = HSVtoRGB(h + 10, 0.5f, 1.0f);
            tailInner = HSVtoRGB(fmodf(h + 35, 360), 0.4f, 0.95f);
            break;
        }
        case 4: { // 冷色调流动：青蓝紫 + 冰晶式亮度闪烁 / Cool flow: cyan-blue-purple + icy shimmer
            float h = 160 + fmodf(t * 20, 130);  // 160~290: 青-蓝-紫更广
            float shimmer = 0.9f + 0.1f * sinf(t * 6.0f) + 0.05f * sinf(t * 13.0f);
            headOuter = HSVtoRGB(h, 0.75f, shimmer);
            tailOuter = HSVtoRGB(fmodf(h + 50, 360), 0.7f, shimmer * 0.9f);
            headInner = HSVtoRGB(h, 0.3f, 1.0f);
            tailInner = HSVtoRGB(fmodf(h + 50, 360), 0.25f, 0.98f);
            break;
        }
        case 5: { // 霓虹脉冲：强对比亮度脉冲 + 色相微偏移 + 速度增强 / Neon pulse: high-contrast brightness pulse + hue shift + velocity boost
            float speedBoost = fminf(velocity / 60.0f, 1.0f);
            float p = 0.5f + 0.5f * sinf(t * (3.5f + speedBoost * 2.5f));
            float sharpPulse = p * p * (3 - 2 * p);  // smoothstep 更锐利
            D2D1_COLOR_F c = g_customColor;
            float hueShift = sharpPulse * 20.0f;
            float h, s, v; RGBtoHSV(c, h, s, v);
            headOuter = HSVtoRGB(fmodf(h + hueShift, 360), s, 0.3f + sharpPulse * 0.7f);
            tailOuter = HSVtoRGB(fmodf(h - hueShift, 360), s * 0.8f, sharpPulse * 0.4f);
            headInner = LighterColor(headOuter, 0.5f);
            tailInner = LighterColor(tailOuter, 0.5f);
            break;
        }
        case 6: { // 速度变色：非线性映射蓝→青→绿→黄→橙→红，速度越快越鲜艳 / Velocity color: non-linear mapping blue→cyan→green→yellow→orange→red
            float sn = fminf(powf(velocity / 70.0f, 0.55f), 1.0f);
            float h = 240 - sn * 240;  // 蓝240→红0
            float sat = 0.7f + sn * 0.25f;  // 速度越快越饱和
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
        case 8: { // 火焰：多频率湍流 + 速度增强火焰高度 / Fire: multi-frequency turbulence + velocity-driven flame height
            float speedBoost = fminf(velocity / 50.0f, 1.0f);
            float turbulence = 0.75f + 0.25f * sinf(t * (10 + speedBoost * 8)) * sinf(t * 5.5f) + 0.12f * sinf(t * (20 + speedBoost * 10));
            float core = 0.9f + 0.1f * sinf(t * 15);
            headOuter = D2D1::ColorF(1.0f * turbulence, (0.65f + speedBoost * 0.15f) * turbulence, 0.1f * core, 1);
            tailOuter = D2D1::ColorF(0.85f, 0.12f, 0.0f, 1);
            headInner = D2D1::ColorF(1.0f, 0.92f + speedBoost * 0.08f, 0.55f + speedBoost * 0.2f, 1);
            tailInner = D2D1::ColorF(0.95f, 0.35f, 0.05f, 1);
            break;
        }
        case 9: { // 极光：三层色相波动 + 柔和饱和度过渡 + 速度增强 / Aurora: triple-layer hue wave + soft saturation transition + velocity boost
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
            D2D1_COLOR_F ec = g_cursorExtractedColor;
            headOuter = tailOuter = ec;
            headInner = tailInner = LighterColor(ec, 0.6f);
            break;
        }
        case 11: {
            D2D1_COLOR_F mixed = LerpColor(g_cursorExtractedColor, g_customColor, 0.5f);
            D2D1_COLOR_F mixedTail = LerpColor(g_cursorExtractedColor, GetGradColor(2), 0.5f);
            headOuter = mixed;
            tailOuter = mixedTail;
            headInner = LighterColor(mixed, 0.55f);
            tailInner = LighterColor(mixedTail, 0.55f);
            break;
        }
        case 12: { // 金属金：高光扫过效果 + 速度增强光泽 / Metallic gold: highlight sweep effect + velocity-enhanced shine
            float speedBoost = fminf(velocity / 50.0f, 1.0f);
            float shine = 0.6f + 0.4f * sinf(t * (2.0f + speedBoost * 1.5f)) + 0.12f * sinf(t * (6.0f + speedBoost * 3.0f));
            float sweep = 0.5f + 0.5f * sinf(t * 1.2f);  // 高光扫过 / highlight sweep
            headOuter = D2D1::ColorF(1.0f * shine, 0.8f * shine, 0.08f * shine, 1);
            tailOuter = D2D1::ColorF(0.5f, 0.35f, 0.04f, 1);
            headInner = D2D1::ColorF(1.0f, 0.9f + sweep * 0.1f, 0.5f + sweep * 0.3f, 1);
            tailInner = D2D1::ColorF(0.8f, 0.6f, 0.2f, 1);
            break;
        }
        case 13: { // 赛博朋克：紫青强对比 + glitch式快速脉冲 + 速度增强 / Cyberpunk: purple-cyan high contrast + glitch-style fast pulse + velocity boost
            float speedBoost = fminf(velocity / 50.0f, 1.0f);
            float pulse = 0.5f + 0.5f * sinf(t * (4.0f + speedBoost * 3.0f));
            float pulse2 = 0.5f + 0.5f * sinf(t * (4.0f + speedBoost * 3.0f) + 1.5f);
            float glitch = 0.9f + 0.1f * sinf(t * 30.0f) * (speedBoost > 0.5f ? 1.0f : 0.0f);  // 高速时glitch闪烁
            headOuter = HSVtoRGB(285 + pulse * 35, 0.9f, glitch);
            tailOuter = HSVtoRGB(170 - pulse2 * 20, 0.9f, glitch * 0.95f);
            headInner = HSVtoRGB(310, 0.5f, 1.0f);
            tailInner = HSVtoRGB(160, 0.5f, 1.0f);
            break;
        }
        case 14: { // 粉彩：低饱和高亮度 + 柔和色相流动 + 速度微增饱和 / Pastel: low-sat high-brightness + soft hue flow + velocity micro-sat boost
            float speedBoost = fminf(velocity / 80.0f, 1.0f);
            float h = fmodf(t * 15, 360);
            float sat = 0.25f + speedBoost * 0.15f;  // 速度越快越鲜艳
            headOuter = HSVtoRGB(h, sat, 1.0f);
            tailOuter = HSVtoRGB(fmodf(h + 70, 360), sat, 0.98f);
            headInner = HSVtoRGB(h, sat * 0.5f, 1.0f);
            tailInner = HSVtoRGB(fmodf(h + 70, 360), sat * 0.5f, 1.0f);
            break;
        }
        case 15: { // 色相旋转：基于自定义颜色 + 速度驱动旋转速度 + 头尾大色差 / Hue rotate: custom color base + velocity-driven rotation speed + large head-tail gap
            float speedBoost = fminf(velocity / 50.0f, 1.0f);
            float h, s, v;
            RGBtoHSV(g_customColor, h, s, v);
            float rotSpeed = 30 + speedBoost * 60;  // 速度越快旋转越快
            float h1 = fmodf(h + t * rotSpeed, 360);
            float h2 = fmodf(h1 + 160 + speedBoost * 40, 360);  // 高速时色差更大
            headOuter = HSVtoRGB(h1, s, v);
            tailOuter = HSVtoRGB(h2, s * 0.9f, v * 0.95f);
            headInner = HSVtoRGB(h1, s * 0.4f, v);
            tailInner = HSVtoRGB(h2, s * 0.35f, v * 0.95f);
            break;
        }
        case 16: { // 双色脉冲：smoothstep脉冲 + 速度增强频率 + 中间色过渡 / Dual pulse: smoothstep pulse + velocity-enhanced freq + mid-color transition
            float speedBoost = fminf(velocity / 60.0f, 1.0f);
            float pulse = 0.5f + 0.5f * sinf(t * (2.5f + speedBoost * 2.0f));
            float smoothPulse = pulse * pulse * (3 - 2 * pulse);
            headOuter = LerpColor(GetGradColor(0), GetGradColor(2), smoothPulse);
            tailOuter = LerpColor(GetGradColor(2), GetGradColor(0), smoothPulse);
            headInner = LighterColor(headOuter, 0.45f);
            tailInner = LighterColor(tailOuter, 0.45f);
            break;
        }
        case 17: { // 星光闪烁：随机亮度闪烁 + 色相微变 + 速度增强闪烁频率 / Sparkle: random brightness flicker + hue micro-shift + velocity-enhanced flicker
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
        case 18: { // 热力图：速度映射黑体辐射色温 + 余温延迟 + 强对比 / Thermal heatmap: velocity maps to blackbody temp + thermal lag + strong contrast
            static float heatLag = 0.0f;  // 余温延迟 / thermal lag
            float targetHeat = fminf(powf(velocity / 80.0f, 0.5f), 1.0f);
            heatLag += (targetHeat - heatLag) * 0.15f;  // 缓慢冷却 / slow cooling
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
        case 19: { // 相位干涉：三波叠加干涉 + 速度增强频率 + 饱和度脉动 / Phase interference: triple-wave interference + velocity freq boost + saturation pulse
            float speedBoost = fminf(velocity / 50.0f, 1.0f);
            float wave1 = sinf(t * (2.0f + speedBoost * 2.0f)) * 0.5f + 0.5f;
            float wave2 = sinf(t * (3.3f + speedBoost * 2.5f) + 1.2f) * 0.5f + 0.5f;
            float wave3 = sinf(t * (5.0f + speedBoost * 3.0f) + 2.5f) * 0.5f + 0.5f;
            float interfere = wave1 * wave2 + (1 - wave1) * (1 - wave2);
            interfere = interfere * 0.7f + wave3 * 0.3f;  // 三波混合 / triple mix
            float h1 = fmodf(interfere * 360 + t * (15 + speedBoost * 25), 360);
            float h2 = fmodf(h1 + 130 + sinf(t * 1.5f) * 50, 360);
            float sat = 0.55f + interfere * 0.4f;
            headOuter = HSVtoRGB(h1, sat, 1.0f);
            tailOuter = HSVtoRGB(h2, sat, 0.9f);
            headInner = HSVtoRGB(h1, sat * 0.35f, 1.0f);
            tailInner = HSVtoRGB(h2, sat * 0.3f, 0.95f);
            break;
        }
        case 20: { // 色谱分裂：速度驱动头尾色相分裂 + 分裂时饱和度增强 + 摆动 / Spectrum split: velocity-driven head-tail hue split + saturation boost on split + wobble
            float speedBoost = fminf(velocity / 40.0f, 1.0f);
            float h, s, v;
            RGBtoHSV(g_customColor, h, s, v);
            float split = speedBoost * 200.0f;  // 0~200度分裂，更明显
            float wobble = sinf(t * (1.5f + speedBoost)) * (20.0f + speedBoost * 20.0f);
            float satBoost = 1.0f + speedBoost * 0.3f;  // 分裂时更饱和
            headOuter = HSVtoRGB(fmodf(h + split * 0.5f + wobble + 360, 360), fminf(s * satBoost, 1.0f), v);
            tailOuter = HSVtoRGB(fmodf(h - split * 0.5f - wobble + 360, 360), fminf(s * satBoost * 0.9f, 1.0f), v * 0.9f);
            headInner = HSVtoRGB(fmodf(h + split * 0.5f + wobble + 360, 360), s * 0.35f, v);
            tailInner = HSVtoRGB(fmodf(h - split * 0.5f - wobble + 360, 360), s * 0.3f, v * 0.9f);
            break;
        }
        case 21: { // 颗粒抖动：时间流动噪点 + 速度增强幅度 + 有序抖动图案 / Grain dither: temporal flowing noise + velocity amplitude boost + ordered dither pattern
            float speedBoost = fminf(velocity / 40.0f, 1.0f);
            float jitterAmp = (0.2f + speedBoost * 0.4f);  // 幅度更大
            float baseH, baseS, baseV;
            RGBtoHSV(g_customColor, baseH, baseS, baseV);
            // 时间流动噪点：不同帧有不同噪点，形成流动感 / Temporal flowing noise: different noise per frame creates flowing feel
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
        case 22: { // 渐变扭曲：双正弦相位扭曲 + 速度增强扭曲幅度 + LUT流动 / Gradient warp: dual-sine phase distortion + velocity amplitude boost + LUT flow
            float speedBoost = fminf(velocity / 50.0f, 1.0f);
            float warp = 0.5f + (0.3f + speedBoost * 0.2f) * sinf(t * 1.5f) + (0.15f + speedBoost * 0.1f) * sinf(t * 3.7f + 0.8f);
            headOuter = GetGradColor(0);
            tailOuter = GetGradColor(g_gradColorCount - 1);
            headInner = LighterColor(GetGradColor(0));
            tailInner = LighterColor(GetGradColor(g_gradColorCount - 1));
            g_gradientWarp = warp;
            break;
        }
        default:
            headOuter = tailOuter = D2D1::ColorF(0, 0, 0, 1);
            headInner = tailInner = D2D1::ColorF(1, 1, 1, 1);
            break;
    }
    out.solidOuter = headOuter;
    out.solidInner = headInner;
    for (int i = 0; i < GRAD_STOPS; i++) {
        float ratio = (float)i / (GRAD_STOPS - 1);
        float alpha = 0.86f * powf(1.0f - ratio, 1.4f);
        if (mode == 7) {
            float phase = fmodf(ratio * 4.0f + t * 2.0f, 1.0f);
            bool stripe = phase < 0.5f;
            D2D1_COLOR_F co = stripe ? headOuter : tailOuter, ci = stripe ? headInner : tailInner;
            out.outer[i] = {ratio, D2D1::ColorF(co.r, co.g, co.b, alpha)};
            out.inner[i] = {ratio, D2D1::ColorF(ci.r, ci.g, ci.b, alpha)};
        } else if (mode == 8) {
            float fr = ratio * ratio;
            D2D1_COLOR_F co = LerpColor(headOuter, tailOuter, fr), ci = LerpColor(headInner, tailInner, fr);
            out.outer[i] = {ratio, D2D1::ColorF(co.r, co.g, co.b, alpha)};
            out.inner[i] = {ratio, D2D1::ColorF(ci.r, ci.g, ci.b, alpha)};
        } else if (mode == 1) {
            // 流动渐变：OKLab LUT采样，颜色沿拖尾流动，速度更快 / Flowing gradient: OKLab LUT, colors flow along trail, faster speed
            float phase = fmodf(t * g_gradientFlowSpeed * 1.5f, 1.0f);
            D2D1_COLOR_F co = SampleGradientLUT(ratio, phase);
            D2D1_COLOR_F ci = LighterColor(co, 0.45f);
            out.outer[i] = {ratio, D2D1::ColorF(co.r, co.g, co.b, alpha)};
            out.inner[i] = {ratio, D2D1::ColorF(ci.r, ci.g, ci.b, alpha)};
        } else if (mode == 22) {
            // 渐变扭曲：LUT采样 + 双正弦相位扭曲，扭曲幅度更大 / Gradient warp: LUT + dual-sine phase distortion, larger amplitude
            float warp = 0.5f + 0.4f * sinf(t * 1.5f + ratio * 4.0f) + 0.2f * sinf(t * 3.7f + ratio * 7.0f + 0.8f);
            float phase = fmodf(t * g_gradientFlowSpeed + warp * 0.5f, 1.0f);
            D2D1_COLOR_F co = SampleGradientLUT(ratio, phase);
            D2D1_COLOR_F ci = LighterColor(co, 0.45f);
            out.outer[i] = {ratio, D2D1::ColorF(co.r, co.g, co.b, alpha)};
            out.inner[i] = {ratio, D2D1::ColorF(ci.r, ci.g, ci.b, alpha)};
        } else {
            D2D1_COLOR_F co = LerpColor(headOuter, tailOuter, ratio), ci = LerpColor(headInner, tailInner, ratio);
            out.outer[i] = {ratio, D2D1::ColorF(co.r, co.g, co.b, alpha)};
            out.inner[i] = {ratio, D2D1::ColorF(ci.r, ci.g, ci.b, alpha)};
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
    if (g_pGradOuterBrush) {
        g_pGradOuterBrush->SetStartPoint(headPt);
        g_pGradOuterBrush->SetEndPoint(tailPt);
    }
    if (g_pGradInnerBrush) {
        g_pGradInnerBrush->SetStartPoint(headPt);
        g_pGradInnerBrush->SetEndPoint(tailPt);
    }
}

// ===================== D3D11 原生渲染（v3）=====================

// ---- HLSL 着色器（v3.4.1 调优：心形SDF修复 + 圆形快速路径 + 预乘alpha + 光照合并 + 自适应软边缘）----
// ---- HLSL shaders (v3.4.1 tuning: heart SDF fix + circle fast path + premultiplied alpha + merged lighting + adaptive soft edge) ----
static const char* g_vsShader = R"(
cbuffer ConstantBuffer : register(b0) {
    float2 screenSize;
    float perspective;   // 透视强度（0=纯2D，0.5=中等）/ Perspective strength (0=pure 2D, 0.5=medium)
    float _pad0;
    float4 gradient[16]; // 渐变停止点（rgba）/ Gradient stops (rgba)
    int gradientCount;
    float bgLuminance;   // 背景亮度 / Background luminance
    float adaptiveFlag;  // 自适应对比度开关 / Adaptive contrast toggle
    float smoothFlag;    // 平滑渐变开关（1=插值平滑，0=阶梯色）/ Smooth gradient toggle
    float edgeSoftness;  // 边缘柔和度（0=硬边，1=极柔和）/ Edge softness (0=hard, 1=very soft)
    float _pad1;
    float _pad2;
    float aaMode;  // 抗锯齿模式 / AA mode: 0=off 1=smooth 2=crisp 3=extra
    float textAspect; // 文字宽高比 / text aspect ratio
};

struct VS_INPUT {
    float3 pos : POSITION;   // x, y, z(深度) / x, y, z(depth)
    float4 color : COLOR;
    float u : TEXCOORD0;     // 沿路径比例 0~1 / Along-path ratio 0~1
    float v : TEXCOORD1;     // 垂直路径方向 -1~1 / Perpendicular direction -1~1
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
    // 2.5D 透视：根据深度 z 相对于屏幕中心缩放 / 2.5D perspective: scale by depth z relative to screen center
    float scale = 1.0 + input.pos.z * perspective;
    float2 center = screenSize * 0.5;
    float2 screenPos = center + (input.pos.xy - center) * scale;
    // 屏幕坐标 → 裁剪空间 / Screen coords → clip space (fused multiply-add)
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

static const char* g_psShader = R"(
cbuffer ConstantBuffer : register(b0) {
    float2 screenSize;
    float perspective;
    float _pad0;
    float4 gradient[16];
    int gradientCount;
    float bgLuminance;
    float adaptiveFlag;
    float smoothFlag;
    float edgeSoftness;
    float _pad1;
    float _pad2;
    float aaMode;
    float textAspect;
};

struct PS_INPUT {
    float4 pos : SV_POSITION;
    float4 color : COLOR;
    float u : TEXCOORD0;
    float v : TEXCOORD1;
    float depth : TEXCOORD2;
};

float4 PSMain(PS_INPUT input) : SV_TARGET {
    // 抗锯齿边缘：根据aaMode调整边缘过渡宽度 / AA edge: adjust transition width based on aaMode
    float absV = abs(input.v);
    float edgeFade;
    if (aaMode < 0.5) {
        // off：硬边 / off: hard edge
        edgeFade = (absV < 1.0) ? 1.0 : 0.0;
    } else if (aaMode < 1.5) {
        // smooth：默认平滑 / smooth: default
        float inner = 1.0 - edgeSoftness * 0.5;
        edgeFade = smoothstep(1.0, inner, absV);
    } else if (aaMode < 2.5) {
        // crisp：锐利窄边 / crisp: narrow sharp edge
        float inner = 1.0 - edgeSoftness * 0.15;
        edgeFade = smoothstep(1.0, inner, absV);
    } else {
        // extra：超宽柔和边 / extra: wide soft edge
        float inner = 1.0 - edgeSoftness * 0.8;
        edgeFade = smoothstep(1.0, inner, absV);
    }
    // Early discard: 完全在边缘外的像素直接丢弃 / Early discard: skip pixels fully outside edge
    if (edgeFade <= 0.001) discard;
    // 渐变采样：GPU硬件插值，无断层 / Gradient sampling: GPU hardware interpolation, no banding
    float4 gradColor = input.color;
    float3 tint = float3(1, 1, 1);  // 单色模式：顶点颜色即最终颜色 / Solid mode: vertex color is final
    if (gradientCount > 1) {
        float t = input.u * (gradientCount - 1);
        int idx = (int)t;
        float frac = t - idx;
        if (idx >= gradientCount - 1) {
            gradColor = gradient[gradientCount - 1];
        } else if (smoothFlag > 0.5) {
            // smoothstep 过渡比线性lerp更自然 / smoothstep transition feels more natural than linear lerp
            float s = frac * frac * (3.0 - 2.0 * frac);
            gradColor = lerp(gradient[idx], gradient[idx + 1], s);
        } else {
            gradColor = gradient[idx];  // 阶梯色 / Stepped color
        }
        gradColor.a *= input.color.a;
        tint = input.color.rgb;  // 渐变模式：顶点RGB作为亮度调制 / Gradient mode: vertex RGB modulates brightness
    }
    // 合并圆柱光照 + 2.5D深度光照：中心(v=0)最亮，深度越大越亮 / Merged cylindrical + 2.5D depth lighting
    float tubeLight = (0.88 + 0.12 * (1.0 - absV)) * (1.0 + input.depth * 0.3);
    float3 rgb = gradColor.rgb * tint * tubeLight;
    // 自适应对比度：分支扁平化 / Adaptive contrast: branchless
    if (adaptiveFlag > 0.5) {
        float bgDelta = bgLuminance - 0.5;
        float isBright = step(0.0, bgDelta);
        float darken = bgDelta * bgDelta * 2.5 + bgDelta * 0.3;
        float3 brightResult = rgb * (1.0 - min(darken, 0.7));
        float3 darkResult = lerp(rgb, float3(1,1,1), -bgDelta * 0.5);
        rgb = lerp(darkResult, brightResult, isBright);
    }
    // 输出非预乘alpha：混合状态SRC_ALPHA/INV_SRC_ALPHA会自动完成预乘 / Output non-premultiplied alpha: SRC_ALPHA blend auto-premultiplies
    float alpha = gradColor.a * edgeFade;
    return float4(rgb, alpha);
}
)";

// 粒子实例着色器（v3.4.1 调优：圆形快速路径 + 心形SDF修复 + early discard + 分支扁平化 + 自适应软边缘）
// Particle instanced shader (v3.4.1 tuning: circle fast path + heart SDF fix + early discard + branchless + adaptive soft edge)
static const char* g_particleVS = R"(
cbuffer ConstantBuffer : register(b0) {
    float2 screenSize;
    float perspective;
    float _pad0;
    float4 gradient[16];
    int gradientCount;
    float bgLuminance;
    float adaptiveFlag;
    float smoothFlag;
    float edgeSoftness;
    float atlasRows;
    float atlasCols;
    float aaMode;
    float textAspect;
};

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
    float size : TEXCOORD3;  // 传递大小给PS用于自适应软边缘 / Pass size to PS for adaptive soft edge
    float charIdx : TEXCOORD4;
};

VS_OUTPUT VSMain(VS_INPUT input) {
    VS_OUTPUT output;
    // 2.5D 透视缩放 / 2.5D perspective scale
    float scale = 1.0 + input.instancePos.z * perspective;
    float finalSize = input.instanceSize * scale;  // 预计算最终大小 / Precompute final size
    // 文字粒子：按宽高比拉伸 X，避免长句被压缩成正方形 / Text particles: stretch X by aspect ratio to avoid squishing
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
    // 屏幕坐标 → NDC（fused multiply-add）/ Screen → NDC (fused multiply-add)
    float2 ndc = float2(
        worldPos.x / screenSize.x * 2.0 - 1.0,
        1.0 - worldPos.y / screenSize.y * 2.0
    );
    output.pos = float4(ndc, 0.0, 1.0);
    output.uv = input.quadPos * 0.5 + 0.5;  // uv不旋转，形状遮罩随四边形旋转 / uv unrotated so shape mask appears rotated
    output.color = input.instanceColor;
    output.depth = input.instancePos.z;
    output.shape = input.instanceShape;
    output.size = finalSize;  // 透视后的实际大小 / Actual size after perspective
    output.charIdx = input.instanceCharIdx;
    return output;
}
)";

static const char* g_particlePS = R"(
cbuffer ConstantBuffer : register(b0) {
    float2 screenSize;
    float perspective;
    float _pad0;
    float4 gradient[16];
    int gradientCount;
    float bgLuminance;
    float adaptiveFlag;
    float smoothFlag;
    float edgeSoftness;
    float atlasRows;
    float atlasCols;
    float aaMode;
    float textAspect;
};

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

// 形状SDF：接收预计算的p和r，避免重复计算 / Shape SDF: takes precomputed p and r to avoid recomputation
float shapeSDF(float2 p, float r, float shapeType) {
    // 圆形快速路径：最常用形状，跳过atan2和分支 / Circle fast path: most common shape, skip atan2 and branches
    if (shapeType < 0.5) {
        return 0.5 - r;
    }
    float a = atan2(p.y, p.x);
    if (shapeType < 1.5) {
        return 0.5 * (0.4 + 0.6 * abs(cos(a * 2.5))) - r;          // star / 星形
    } else if (shapeType < 2.5) {
        return 0.5 * (0.5 + 0.5 * abs(cos(a * 3.0))) - r;          // hexagram / 六角星
    } else if (shapeType < 3.5) {
        // 心形：复用p*2，用x*x*x替代pow避免负数底数UB / Heart: reuse p*2, use x*x*x instead of pow
        float2 hp = p * 2.0;
        hp.y = -hp.y;
        float t = hp.x*hp.x + hp.y*hp.y - 1.0;
        float heart = t * t * t - hp.x*hp.x * hp.y*hp.y*hp.y;
        return -heart * 0.12;
    } else if (shapeType < 4.5) {
        return 0.5 - (abs(p.x) + abs(p.y));                        // diamond / 菱形
    } else if (shapeType < 5.5) {
        // 等边三角形（顶点在上）/ Equilateral triangle (point up)
        float2 tp = p * 2.0;
        tp.y = -tp.y;
        return min(tp.y + 0.5, min(1.0 - tp.y - 1.732 * tp.x, 1.0 - tp.y + 1.732 * tp.x)) * 0.35;
    } else if (shapeType < 6.5) {
        return 0.5 * (0.6 + 0.4 * cos(a * 5.0)) - r;               // flower / 花形
    } else if (shapeType < 7.5) {
        // 五边形：预计算PI常量 / Pentagon: precomputed PI constants
        float pentR = 0.425 / cos(fmod(a + 3.14159, 1.25664) - 0.62832);
        return pentR - r;                                          // pentagon / 五边形
    } else {
        // 六边形：预计算PI常量 / Hexagon: precomputed PI constants
        float hexR = 0.435 / cos(fmod(a + 3.14159, 1.04720) - 0.52360);
        return hexR - r;                                           // hexagon / 六边形
    }
}

float4 PSMain(PS_INPUT input) : SV_TARGET {
    // 文字字符形状：采样字符图集纹理 / Text char shape: sample char atlas texture
    if (input.shape > 9.5) {
        int idx = (int)input.charIdx;
        int cols = (int)atlasCols;
        int row = idx / cols;
        int col = idx % cols;
        float2 cellUV = input.uv;
        float2 atlasUV = float2((col + cellUV.x) / atlasCols, (row + cellUV.y) / atlasRows);
        float4 tex = charAtlasTex.Sample(charAtlasSampler, atlasUV);
        if (tex.a <= 0.01) discard;
        float3 rgb = input.color.rgb;
        float alpha = input.color.a * tex.a;
        return float4(rgb, alpha);
    }
    float2 p = input.uv - 0.5;
    float r = length(p);
    // Early discard: 最大形状半径约0.5+边缘余量，超出直接跳过SDF计算 / Early discard: skip SDF for pixels outside max shape radius + margin
    if (r > 0.62) discard;
    // SDF软边缘抗锯齿，边缘宽度随aaMode和粒子大小自适应 / SDF soft-edge AA, width adapts to aaMode and particle size
    float sdf = shapeSDF(p, r, input.shape);
    float edge;
    if (aaMode < 0.5) {
        // off：硬边 / off: hard edge
        edge = 0.003;
    } else if (aaMode < 1.5) {
        // smooth：默认 / smooth: default
        edge = clamp(0.025 * (input.size / 12.0), 0.01, 0.08);
    } else if (aaMode < 2.5) {
        // crisp：窄边 / crisp: narrow
        edge = clamp(0.012 * (input.size / 12.0), 0.005, 0.04);
    } else {
        // extra：宽边 / extra: wide
        edge = clamp(0.05 * (input.size / 12.0), 0.02, 0.12);
    }
    float mask = smoothstep(-edge, edge, sdf);
    if (mask <= 0.001) discard;
    // 合并径向光照 + 2.5D深度光照 / Merged radial + 2.5D depth lighting
    float radialLight = max(1.0 - r * 0.3, 0.6) * (1.0 + input.depth * 0.25);
    float3 rgb = input.color.rgb * radialLight;
    // 自适应对比度：分支扁平化，用lerp+step替代if-else / Adaptive contrast: branchless with lerp+step
    if (adaptiveFlag > 0.5) {
        float bgDelta = bgLuminance - 0.5;
        float isBright = step(0.0, bgDelta);  // 1=亮背景, 0=暗背景 / 1=bright, 0=dark
        float darken = bgDelta * bgDelta * 2.5 + bgDelta * 0.3;
        float3 brightResult = rgb * (1.0 - min(darken, 0.7));
        float3 darkResult = lerp(rgb, float3(1,1,1), -bgDelta * 0.5);
        rgb = lerp(darkResult, brightResult, isBright);
    }
    // 输出非预乘alpha：混合状态SRC_ALPHA/INV_SRC_ALPHA自动完成预乘 / Output non-premultiplied alpha: SRC_ALPHA blend auto-premultiplies
    float alpha = input.color.a * mask;
    return float4(rgb, alpha);
}
)";

// SSAA 降采样着色器：全屏四边形采样离屏纹理 / SSAA downsample shader: fullscreen quad sampling offscreen
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
Texture2D offscreenTex : register(t0);
SamplerState offscreenSampler : register(s0);
struct VS_OUT { float4 pos : SV_POSITION; float2 uv : TEXCOORD0; };
float4 PSMain(VS_OUT input) : SV_TARGET {
    return offscreenTex.Sample(offscreenSampler, input.uv);
}
)";

// ---- 顶点结构（v3.3：添加 v 垂直坐标用于软边缘和管光）----
struct VertexPosColor {
    float x, y, z;     // 位置 + 深度（2.5D）
    float r, g, b, a;  // 颜色
    float u;           // 沿路径比例 0~1（用于渐变采样）
    float v;           // 垂直路径方向 -1~1（左边缘=-1，右边缘=1，用于软边缘和圆柱光照）
};

struct ParticleInstance {
    float x, y, z;     // 位置 + 深度 / Position + depth
    float r, g, b, a; // 颜色 / Color
    float size;       // 大小（半径）/ Size (radius)
    float shapeType;  // 0=circle,1=star,2=hexagram,3=heart,4=diamond,5=triangle,6=flower,7=pentagon,8=hexagon (与HLSL shapeSDF一致 / matches HLSL shapeSDF)
    float rotation;   // 旋转角度（弧度）/ Rotation (radians)
    float charIndex;  // 字符图集索引 / char atlas index
};

// ---- 原生渲染资源 ----
ID3D11VertexShader* g_pNativeVS = nullptr;
ID3D11PixelShader* g_pNativePS = nullptr;
ID3D11VertexShader* g_pParticleVS = nullptr;
ID3D11PixelShader* g_pParticlePS = nullptr;
ID3D11InputLayout* g_pNativeLayout = nullptr;
ID3D11InputLayout* g_pParticleLayout = nullptr;
ID3D11Buffer* g_pConstantBuffer = nullptr;
ID3D11Buffer* g_pTrailVB = nullptr;       // 拖尾带顶点缓冲（动态）
ID3D11Buffer* g_pParticleQuadVB = nullptr; // 粒子四边形顶点缓冲
ID3D11Buffer* g_pParticleInstanceBuf = nullptr; // 粒子实例缓冲（动态）
ID3D11BlendState* g_pAlphaBlend = nullptr;
ID3D11BlendState* g_pAdditiveBlend = nullptr;  // 加法混合状态（发光用）/ Additive blend state (for glow effects)
ID3D11RasterizerState* g_pRasterState = nullptr;
ID3D11Texture2D* g_pCharAtlasTex = nullptr;       // 字符图集纹理 / char atlas texture
ID3D11ShaderResourceView* g_pCharAtlasSRV = nullptr; // 字符图集 SRV / char atlas SRV
ID3D11SamplerState* g_pCharAtlasSampler = nullptr;     // 字符图集采样器 / char atlas sampler
int g_charAtlasCols = 0;                           // 图集列数 / atlas columns
int g_charAtlasRows = 0;                          // 图集行数 / atlas rows
int g_charAtlasCellW = 0;                          // 单元格宽 / cell width
int g_charAtlasCellH = 0;                         // 单元格高 / cell height
float g_textAspectRatio = 1.0f;                      // 文字宽高比 / text cell aspect ratio
bool g_charAtlasDirty = true;                            // 图集脏标记（设置变更时重建）/ atlas dirty flag (rebuild on settings change)
std::vector<std::wstring> g_atlasChars;                // 图集格子列表（分段后为各片段）/ atlas cell list (chunks after splitting)
std::vector<int> g_phraseChunkStart;                    // 每个原始词组在图集中的起始格 / first atlas cell of each original phrase
std::vector<int> g_phraseChunkCount;                    // 每个原始词组占用的格数 / atlas cells per original phrase
DWORD g_globalChunkClock = 0;                           // 全局同步轮播时钟起点(ms) / global sync cycle clock origin (ms)
// 构建字符图集：把所有字符渲染到一张 D3D11 纹理 / Build char atlas: render all chars to one D3D11 texture
static void BuildCharAtlas() {
    Wh_Log(L"[CharAtlas] BuildCharAtlas called, dev=%p factory=%p fmt=%p", g_pD3DDevice, g_pD2DFactory, g_pTextFormat);
    // 重建文字格式（字号可能已变化）/ Recreate text format (font size may have changed)
    if (g_pTextFormat) { g_pTextFormat->Release(); g_pTextFormat = nullptr; }
    if (g_dwFactory) {
        g_dwFactory->CreateTextFormat(L"Segoe UI Emoji", nullptr, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL,
            (float)(g_textFontSize * 2.0f), L"en-us", &g_pTextFormat);
        if (!g_pTextFormat) g_dwFactory->CreateTextFormat(L"Segoe UI", nullptr, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL,
            (float)(g_textFontSize * 2.0f), L"en-us", &g_pTextFormat);
        if (g_pTextFormat) {
            g_pTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
            g_pTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
        }
    }
    if (!g_pD3DDevice || !g_pD2DFactory || !g_pTextFormat) { Wh_Log(L"[CharAtlas] early return null deps"); return; }
    // 解析 text_content，按逗号/竖线分隔成多个原始词组 / Parse text_content into original phrases by comma/pipe
    std::vector<std::wstring> phrases;
    {
        wchar_t buffer[256];
        int bi = 0;
        for (int i = 0; g_textContent[i] != 0 && i < 255; i++) {
            wchar_t ch = g_textContent[i];
            if (ch == L',' || ch == L'，' || ch == L'|' || ch == L'｜') {
                if (bi > 0) { buffer[bi] = 0; phrases.push_back(buffer); bi = 0; }
                continue;
            }
            if (bi < 255) buffer[bi++] = ch;
        }
        if (bi > 0) { buffer[bi] = 0; phrases.push_back(buffer); }
    }
    if (phrases.empty()) phrases.push_back(L" ");

    // 码点计数：代理对(Emoji)算一个码点 / Count codepoints: a surrogate pair (emoji) counts as one
    auto CodePointLen = [](const std::wstring &s) -> int {
        int n = 0;
        for (size_t k = 0; k < s.size();) {
            wchar_t c = s[k];
            k += (c >= 0xD800 && c <= 0xDBFF && k + 1 < s.size()) ? 2 : 1;
            n++;
        }
        return n;
    };
    // 按码点边界切分长词组，绝不在代理对(Emoji)中间切开 / Split a long phrase on codepoint boundaries, never mid-surrogate-pair
    auto SplitChunks = [](const std::wstring &s, int maxCP) {
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
    };

    // 展开为图集格子：超长词组切成多格，并记录词组→格子区间 / Expand into atlas cells: split overlong phrases, record phrase->cell range
    g_atlasChars.clear();
    g_phraseChunkStart.clear();
    g_phraseChunkCount.clear();
    for (auto &ph : phrases) {
        int cellStart = (int)g_atlasChars.size();
        g_phraseChunkStart.push_back(cellStart);
        if (g_enableTextChunk && CodePointLen(ph) > g_textChunkMax) {
            std::vector<std::wstring> chunks = SplitChunks(ph, g_textChunkMax);
            for (auto &ck : chunks) g_atlasChars.push_back(ck);
        } else {
            g_atlasChars.push_back(ph);
        }
        g_phraseChunkCount.push_back((int)g_atlasChars.size() - cellStart);
    }
    // 重置全局同步轮播时钟 / Reset global-sync cycle clock
    g_globalChunkClock = GetTickCount();

    // 图集布局：16列，行数按格子数算 / Atlas layout: 16 cols, rows by cell count
    int phraseCount = (int)g_atlasChars.size();
    g_charAtlasCols = 16;
    // 格子少时减少列数，避免图集过宽 / Reduce columns when few cells to avoid oversized atlas
    if (phraseCount < g_charAtlasCols) g_charAtlasCols = phraseCount;
    g_charAtlasRows = ((int)g_atlasChars.size() + g_charAtlasCols - 1) / g_charAtlasCols;
    if (g_charAtlasRows < 1) g_charAtlasRows = 1;
    int maxPhraseLen = 1;
    for (auto &s : g_atlasChars) { int l = CodePointLen(s); if (l > maxPhraseLen) maxPhraseLen = l; }
    float SS = 2.0f; // 超采样倍数 / supersampling factor
    g_charAtlasCellW = (int)(g_textFontSize * maxPhraseLen * SS + 16);
    g_charAtlasCellH = (int)(g_textFontSize * SS + 8);
    g_textAspectRatio = (g_charAtlasCellH > 0) ? (float)g_charAtlasCellW / (float)g_charAtlasCellH : 1.0f;
    UINT atlasW = (UINT)(g_charAtlasCols * g_charAtlasCellW);
    UINT atlasH = (UINT)(g_charAtlasRows * g_charAtlasCellH);
    // 钳制到 D3D11 纹理上限，必要时降低超采样 / Clamp to D3D11 texture limit, reduce SS if needed
    const UINT maxTexDim = D3D11_REQ_TEXTURE2D_U_OR_V_DIMENSION;
    while ((atlasW > maxTexDim || atlasH > maxTexDim) && SS > 1.0f) {
        SS -= 0.5f;
        g_charAtlasCellW = (int)(g_textFontSize * maxPhraseLen * SS + 16);
        g_charAtlasCellH = (int)(g_textFontSize * SS + 8);
        g_textAspectRatio = (g_charAtlasCellH > 0) ? (float)g_charAtlasCellW / (float)g_charAtlasCellH : 1.0f;
        atlasW = (UINT)(g_charAtlasCols * g_charAtlasCellW);
        atlasH = (UINT)(g_charAtlasRows * g_charAtlasCellH);
    }

    // 创建 D3D11 纹理 / Create D3D11 texture
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

    // 用 D2D DC 渲染到纹理 / Use D2D DC to render to texture
    if (!g_pD2DDC) { Wh_Log(L"[CharAtlas] g_pD2DDC is null, cannot render"); return; }
    IDXGISurface *pSurface = nullptr;
    if (FAILED(g_pCharAtlasTex->QueryInterface(__uuidof(IDXGISurface), (void**)&pSurface))) { Wh_Log(L"[CharAtlas] QueryInterface(IDXGISurface) FAILED"); return; }

    ID2D1Bitmap1 *pBmp = nullptr;
    D2D1_BITMAP_PROPERTIES1 bmpProps = D2D1::BitmapProperties1(
        D2D1_BITMAP_OPTIONS_TARGET,
        D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED));
    if (FAILED(g_pD2DDC->CreateBitmapFromDxgiSurface(pSurface, &bmpProps, &pBmp))) { Wh_Log(L"[CharAtlas] CreateBitmapFromDxgiSurface FAILED");
        pSurface->Release(); return;
    }

    ID2D1Image *pOldTarget = nullptr;
    g_pD2DDC->GetTarget(&pOldTarget);
    g_pD2DDC->SetTarget(pBmp);
    g_pD2DDC->BeginDraw();
    g_pD2DDC->Clear(D2D1::ColorF(0, 0, 0, 0));

    if (!g_pSolidOuterBrush) { Wh_Log(L"[CharAtlas] g_pSolidOuterBrush null, abort"); pBmp->Release(); pSurface->Release(); return; }
    g_pSolidOuterBrush->SetColor(D2D1::ColorF(1, 1, 1, 1));
    g_pSolidOuterBrush->SetOpacity(1.0f);

    for (int i = 0; i < (int)g_atlasChars.size(); i++) {
        int row = i / g_charAtlasCols;
        int col = i % g_charAtlasCols;
        float cellX = (float)(col * g_charAtlasCellW);
        float cellY = (float)(row * g_charAtlasCellH);
        D2D1_RECT_F rc = { cellX, cellY, cellX + (float)g_charAtlasCellW, cellY + (float)g_charAtlasCellH };
        // DrawText + ENABLE_COLOR_FONT 渲染彩色 emoji / DrawText with ENABLE_COLOR_FONT for color emoji
        g_pD2DDC->DrawText(g_atlasChars[i].c_str(), (UINT32)g_atlasChars[i].length(),
            g_pTextFormat, &rc, g_pSolidOuterBrush,
            D2D1_DRAW_TEXT_OPTIONS_ENABLE_COLOR_FONT);
    }

    g_pD2DDC->EndDraw();
    g_pD2DDC->SetTarget(pOldTarget);
    if (pOldTarget) pOldTarget->Release();
    pBmp->Release();
    pSurface->Release();
    Wh_Log(L"[CharAtlas] built OK, atlasW=%d atlasH=%d phrases=%d cols=%d rows=%d cellW=%d", atlasW, atlasH, (int)g_atlasChars.size(), g_charAtlasCols, g_charAtlasRows, g_charAtlasCellW);
}
static void ReleaseNativeRendering();  // 前向声明，供 InitNativeRendering 失败清理调用

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

static bool InitNativeRendering() {
    if (!g_pD3DDevice) return false;
    ReleaseNativeRendering();  // 幂等：清理可能的残留对象

    // 编译着色器 / Compile shaders
    ID3DBlob *vsBlob = nullptr, *psBlob = nullptr;
    ID3DBlob *pvsBlob = nullptr, *ppsBlob = nullptr;
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

        // 编译 SSAA blit 着色器 / Compile SSAA blit shaders
        ID3DBlob *bvsBlob = nullptr, *bpsBlob = nullptr;
        if (!CompileShader(g_blitVS, "VSMain", "vs_4_0", &bvsBlob)) break;
        if (!CompileShader(g_blitPS, "PSMain", "ps_4_0", &bpsBlob)) break;
        if (FAILED(g_pD3DDevice->CreateVertexShader(bvsBlob->GetBufferPointer(), bvsBlob->GetBufferSize(), nullptr, &g_pBlitVS))) break;
        if (FAILED(g_pD3DDevice->CreatePixelShader(bpsBlob->GetBufferPointer(), bpsBlob->GetBufferSize(), nullptr, &g_pBlitPS))) break;

        // blit 输入布局（POSITION float2 + TEXCOORD0 float2）/ blit input layout (POSITION float2 + TEXCOORD0 float2)
        {
            D3D11_INPUT_ELEMENT_DESC blitLayoutDesc[] = {
                {"POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
                {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 8, D3D11_INPUT_PER_VERTEX_DATA, 0},
            };
            g_pD3DDevice->CreateInputLayout(blitLayoutDesc, 2, bvsBlob->GetBufferPointer(),
                                            bvsBlob->GetBufferSize(), &g_pBlitLayout);
        }
        bvsBlob->Release();
        bpsBlob->Release();

        // 创建全屏四边形 VB（NDC 坐标 + UV）/ Create fullscreen quad VB (NDC coords + UV)
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
        // 创建双线性采样器 / Create bilinear sampler
        {
            D3D11_SAMPLER_DESC sampDesc = {};
            sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
            sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
            sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
            sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
            sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
            g_pD3DDevice->CreateSamplerState(&sampDesc, &g_pBlitSampler);
        }

        // 输入布局：通用顶点（位置+深度+颜色+u坐标+v坐标）/ Input layout: generic vertex (pos+depth+color+u+v)
        D3D11_INPUT_ELEMENT_DESC layoutDesc[] = {
            {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
            {"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
            {"TEXCOORD", 0, DXGI_FORMAT_R32_FLOAT, 0, 28, D3D11_INPUT_PER_VERTEX_DATA, 0},
            {"TEXCOORD", 1, DXGI_FORMAT_R32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0},
        };
        if (FAILED(g_pD3DDevice->CreateInputLayout(layoutDesc, 4, vsBlob->GetBufferPointer(),
                                                   vsBlob->GetBufferSize(), &g_pNativeLayout)))
            break;

        // 输入布局：粒子实例（位置+深度+颜色+大小+形状+旋转）/ Input layout: particle instance (pos+depth+color+size+shape+rot)
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

        // 常量缓冲（包含屏幕尺寸、透视、渐变停止点）/ Constant buffer (screen size, perspective, gradient stops)
        D3D11_BUFFER_DESC cbDesc = {};
        cbDesc.ByteWidth = 320;  // 对齐到 16 字节 / Aligned to 16 bytes
        cbDesc.Usage = D3D11_USAGE_DYNAMIC;
        cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        if (FAILED(g_pD3DDevice->CreateBuffer(&cbDesc, nullptr, &g_pConstantBuffer))) break;

        // 拖尾带顶点缓冲（动态，最大 4096 顶点）/ Trail strip vertex buffer (dynamic, max 4096 verts)
        D3D11_BUFFER_DESC vbDesc = {};
        vbDesc.ByteWidth = sizeof(VertexPosColor) * 4096;
        vbDesc.Usage = D3D11_USAGE_DYNAMIC;
        vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        vbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        if (FAILED(g_pD3DDevice->CreateBuffer(&vbDesc, nullptr, &g_pTrailVB))) break;

        // 粒子四边形（两个三角形组成的正方形）/ Particle quad (square made of two triangles)
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

        // 粒子实例缓冲（动态，最大 2000 实例）/ Particle instance buffer (dynamic, max 2000 instances per layer)
        D3D11_BUFFER_DESC instDesc = {};
        instDesc.ByteWidth = sizeof(ParticleInstance) * 4000;  // 发光层+正常层各最多2000 / glow+normal layers, 2000 each
        instDesc.Usage = D3D11_USAGE_DYNAMIC;
        instDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        instDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        if (FAILED(g_pD3DDevice->CreateBuffer(&instDesc, nullptr, &g_pParticleInstanceBuf))) break;

        // Alpha 混合状态 / Alpha blend state
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

        // 加法混合状态（发光用）/ Additive blend state (for glow effects)
        D3D11_BLEND_DESC addBlendDesc = {};
        addBlendDesc.RenderTarget[0].BlendEnable = TRUE;
        addBlendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
        addBlendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;  // 加法混合 / Additive blending
        addBlendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
        addBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
        addBlendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
        addBlendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
        addBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
        if (FAILED(g_pD3DDevice->CreateBlendState(&addBlendDesc, &g_pAdditiveBlend))) break;

        // 光栅化状态 / Rasterizer state
        D3D11_RASTERIZER_DESC rastDesc = {};
        rastDesc.FillMode = D3D11_FILL_SOLID;
        rastDesc.CullMode = D3D11_CULL_NONE;
        rastDesc.DepthClipEnable = FALSE;
        rastDesc.MultisampleEnable = TRUE;
        if (FAILED(g_pD3DDevice->CreateRasterizerState(&rastDesc, &g_pRasterState))) break;

        // 字符图集采样器（线性过滤，边缘钳制）/ Char atlas sampler (linear filter, clamp)
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

    // 编译 blob 无论成败都释放 / Release compile blobs regardless of success
    if (vsBlob) vsBlob->Release();
    if (psBlob) psBlob->Release();
    if (pvsBlob) pvsBlob->Release();
    if (ppsBlob) ppsBlob->Release();

    if (!ok) {
        Wh_Log(L"NativeD3D: init failed, releasing partial resources");
        ReleaseNativeRendering();
        return false;
    }
    Wh_Log(L"NativeD3D: initialized successfully");
    BuildCharAtlas();  // 构建字符图集 / build char atlas
    return true;
}

static void ReleaseNativeRendering() {
    if (g_pNativeVS) { g_pNativeVS->Release(); g_pNativeVS = nullptr; }
    if (g_pNativePS) { g_pNativePS->Release(); g_pNativePS = nullptr; }
    if (g_pParticleVS) { g_pParticleVS->Release(); g_pParticleVS = nullptr; }
    if (g_pCharAtlasSRV) { g_pCharAtlasSRV->Release(); g_pCharAtlasSRV = nullptr; }
    if (g_pCharAtlasTex) { g_pCharAtlasTex->Release(); g_pCharAtlasTex = nullptr; }
    if (g_pParticlePS) { g_pParticlePS->Release(); g_pParticlePS = nullptr; }
    if (g_pNativeLayout) { g_pNativeLayout->Release(); g_pNativeLayout = nullptr; }
    if (g_pParticleLayout) { g_pParticleLayout->Release(); g_pParticleLayout = nullptr; }
    if (g_pConstantBuffer) { g_pConstantBuffer->Release(); g_pConstantBuffer = nullptr; }
    if (g_pTrailVB) { g_pTrailVB->Release(); g_pTrailVB = nullptr; }
    if (g_pParticleQuadVB) { g_pParticleQuadVB->Release(); g_pParticleQuadVB = nullptr; }
    if (g_pParticleInstanceBuf) { g_pParticleInstanceBuf->Release(); g_pParticleInstanceBuf = nullptr; }
    if (g_pAlphaBlend) { g_pAlphaBlend->Release(); g_pAlphaBlend = nullptr; }
    if (g_pAdditiveBlend) { g_pAdditiveBlend->Release(); g_pAdditiveBlend = nullptr; }
    if (g_pRasterState) { g_pRasterState->Release(); g_pRasterState = nullptr; }
    if (g_pCharAtlasSampler) { g_pCharAtlasSampler->Release(); g_pCharAtlasSampler = nullptr; }
}

static void UpdateConstantBuffer(int width, int height, const GradData* cols = nullptr) {
    if (!g_pConstantBuffer || !g_pD3DContext) return;
    D3D11_MAPPED_SUBRESOURCE mapped;
    if (SUCCEEDED(g_pD3DContext->Map(g_pConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped))) {
        float* data = (float*)mapped.pData;
        // screenSize (offset 0, bytes 0-7) / screenSize
        data[0] = (float)width;
        data[1] = (float)height;
        // perspective (offset 2, bytes 8-11) / perspective
        data[2] = g_enable25DEffect ? (float)g_perspectiveStrength / 100.0f : 0.0f;
        // _pad0 (offset 3, bytes 12-15) — 16字节对齐填充 / alignment padding
        data[3] = 0.0f;
        // gradient[16] (offset 4, bytes 16-271) — 移除lightDir后gradient前移到offset 4 / gradient moved to offset 4 after removing lightDir
        int gradCount = 0;
        if (cols) {
            for (int i = 0; i < GRAD_STOPS; i++) {
                data[4 + i * 4 + 0] = cols->outer[i].color.r;
                data[4 + i * 4 + 1] = cols->outer[i].color.g;
                data[4 + i * 4 + 2] = cols->outer[i].color.b;
                data[4 + i * 4 + 3] = cols->outer[i].color.a;
            }
            gradCount = GRAD_STOPS;
        }
        // gradientCount (offset 68, bytes 272-275) — int类型 / gradientCount (int)
        ((int*)data)[68] = gradCount;
        // bgLuminance (offset 69) / bgLuminance
        data[69] = g_bgLuminance;
        // adaptiveFlag (offset 70) / adaptiveFlag
        data[70] = g_adaptiveContrast ? 1.0f : 0.0f;
        // smoothFlag (offset 71) / smoothFlag
        data[71] = g_enableSmoothGradient ? 1.0f : 0.0f;
        // edgeSoftness (offset 72)：0=硬边，1=极柔和 / edgeSoftness (0=hard, 1=soft)
        data[72] = g_edgeSoftness / 100.0f;
        // _pad1, _pad2 (offset 73-74) / padding
        // atlasRows/atlasCols (offset 73-74) / char atlas dimensions
        data[73] = (float)(g_charAtlasRows > 0 ? g_charAtlasRows : 16);
        data[74] = (float)(g_charAtlasCols > 0 ? g_charAtlasCols : 16);
        data[75] = (float)g_aaMode;
        data[76] = g_textAspectRatio;
        g_pD3DContext->Unmap(g_pConstantBuffer, 0);
    }
}

// 粒子 Instanced Rendering（v3 原生渲染，含发光层；一次 Map 写两层）
static void NativeRenderParticles(int screenW, int screenH) {
    if (!g_pParticleVS || !g_pParticlePS || !g_pParticleInstanceBuf || g_particles.empty()) return;
    static bool loggedNative = false;
    if (!loggedNative) { Wh_Log(L"[CharAtlas] NativeRenderParticles called, g_particleShape=%d, particles=%d", g_particleShape, (int)g_particles.size()); loggedNative = true; }


    // 3D深度排序：z大的（后面）先渲染，z小的（前面）后渲染覆盖，实现前后遮挡 / 3D depth sort: far (large z) drawn first, near (small z) overlaps for occlusion
    // 仅在漩涡活跃时排序，避免普通模式下的性能开销 / Only sort when vortices active to avoid overhead in normal mode
    bool anyVortexActive = false;
    for (int i = 0; i < g_vortexMaxCount; i++) {
        if (g_vortices[i].active && g_vortices[i].strength > 0.1f) { anyVortexActive = true; break; }
    }
    if (anyVortexActive && g_particles.size() < 800) {
        std::sort(g_particles.begin(), g_particles.end(), [](const Particle &a, const Particle &b) {
            return a.z > b.z;
        });
    }

    DWORD now = GetTickCount();
    const int MAX_PER_LAYER = 2000;

    // 一次 Map，依次写入发光层（offset 0）和正常层（offset MAX_PER_LAYER）
    D3D11_MAPPED_SUBRESOURCE mapped;
    if (FAILED(g_pD3DContext->Map(g_pParticleInstanceBuf, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped))) return;
    ParticleInstance* instances = (ParticleInstance*)mapped.pData;

    int glowCount = 0, normalCount = 0;
    float gi = g_particleGlowIntensity / 100.0f;
    float glowSize = 1.6f + gi * 0.8f;
    float glowAlpha = 0.2f + gi * 0.2f;
    float glowColorMul = 1.2f + gi * 0.3f;

    // 单次遍历同时填充两层，消除重复遍历和重复 Map
    float sizeMul = g_particleSizeMultiplier / 100.0f;  // 预计算
    for (auto &p : g_particles) {
        float progress = (float)(now - p.startTime) / p.lifetime;
        if (progress < 0 || progress >= 1) continue;
        float lifeAlpha = (1.0f - progress);
        // 颜色二次衰减：前期保持亮度，后期快速变暗，避免粒子中期就发黑 / Quadratic color fade: bright early, dim late, avoids mid-life darkness
        float colorFade = progress * progress;
        // 预计算颜色偏移（colorOffset + vortexBrightness合并）/ Precompute color offset (colorOffset + vortexBrightness merged)
        float rOff = p.colorOffset[0] + p.vortexBrightness[0];
        float gOff = p.colorOffset[1] + p.vortexBrightness[1];
        float bOff = p.colorOffset[2] + p.vortexBrightness[2];
        D2D1_COLOR_F pc = D2D1::ColorF(
            p.color.r + (p.endColor.r - p.color.r) * colorFade + rOff,
            p.color.g + (p.endColor.g - p.color.g) * colorFade + gOff,
            p.color.b + (p.endColor.b - p.color.b) * colorFade + bOff, 1.0f);
        float sizeScale = sinf(progress * 3.14159f) * 0.7f + 0.3f;
        // 音乐联动：低频增强粒子大小 / Music link: bass boosts particle size
        float musicSizeBoost = g_enableMusicPhysics ? (1.0f + g_musicBassSizeBoost * 0.5f) : 1.0f;
        float baseSize;
        if (p.shapeType == 10) {
            // 文字粒子：字号直接决定屏幕大小 / Text particles: font size directly determines on-screen size
            baseSize = (float)g_textFontSize * 0.5f * sizeMul * sizeScale * musicSizeBoost;
        } else {
            baseSize = p.size * 2.0f * sizeMul * sizeScale * musicSizeBoost;
        }

        // 发光层 / Glow layer
        if (g_enableParticleGlow && glowCount < MAX_PER_LAYER) {
            ParticleInstance& gl = instances[glowCount++];
            gl.x = p.x; gl.y = p.y; gl.z = p.z;
            gl.r = pc.r * glowColorMul; gl.g = pc.g * glowColorMul; gl.b = pc.b * glowColorMul;
            gl.a = lifeAlpha * 0.8f * glowAlpha;
            gl.size = baseSize * glowSize;
            gl.shapeType = (float)p.shapeType;
            gl.rotation = p.rotation;
            gl.charIndex = p.charIndex;
        }
        // 正常层 / Normal layer
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

    // 设置渲染状态 / Set render state
    UpdateConstantBuffer(screenW, screenH);
    g_pD3DContext->IASetInputLayout(g_pParticleLayout);
    g_pD3DContext->VSSetShader(g_pParticleVS, nullptr, 0);
    g_pD3DContext->PSSetShader(g_pParticlePS, nullptr, 0);
    g_pD3DContext->VSSetConstantBuffers(0, 1, &g_pConstantBuffer);
    g_pD3DContext->PSSetConstantBuffers(0, 1, &g_pConstantBuffer);
    // 绑定字符图集纹理（文字形状用）/ Bind char atlas texture (for text shape)
    if (g_particleShape == 10 && g_pCharAtlasSRV) {
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

    // 发光层（实例起始 0），加法混合 / Glow layer (instances from 0), additive blending
    if (glowCount > 0) {
        g_pD3DContext->OMSetBlendState(g_pAdditiveBlend, nullptr, 0xFFFFFFFF);
        g_pD3DContext->DrawInstanced(6, glowCount, 0, 0);
    }
    // 正常层（实例起始 MAX_PER_LAYER），Alpha混合
    if (normalCount > 0) {
        g_pD3DContext->OMSetBlendState(g_pAlphaBlend, nullptr, 0xFFFFFFFF);
        g_pD3DContext->DrawInstanced(6, normalCount, 0, MAX_PER_LAYER);
    }
    // 解绑纹理和采样器，避免影响其他渲染 / Unbind texture and sampler
    { ID3D11ShaderResourceView *nullSRV = nullptr; g_pD3DContext->PSSetShaderResources(0, 1, &nullSRV); }
    { ID3D11SamplerState *nullSampler = nullptr; g_pD3DContext->PSSetSamplers(0, 1, &nullSampler); }
}

// 拖尾带顶点缓冲渲染（v3 原生渲染）/ Trail strip vertex buffer rendering (v3 native)
static void NativeRenderTrail(const std::vector<D2D1_POINT_2F>& smoothed, float widthMul,
                              const GradData& cols, float fadeAlpha, int screenW, int screenH, DWORD dwTime = 0) {
    if (!g_pNativeVS || !g_pNativePS || !g_pTrailVB || smoothed.size() < 2) return;

    size_t sl = smoothed.size();

    // 预计算每个点的法线、宽度和ratio / Precompute normals, widths and ratios per point
    std::vector<float> nx(sl), ny(sl), widths(sl), ratios(sl);
    float invSlMinus1 = 1.0f / (float)(sl - 1);
    for (size_t i = 0; i < sl; ++i) {
        float ddx, ddy;
        if (i == 0) { ddx = smoothed[1].x - smoothed[0].x; ddy = smoothed[1].y - smoothed[0].y; }
        else if (i == sl - 1) { ddx = smoothed[i].x - smoothed[i-1].x; ddy = smoothed[i].y - smoothed[i-1].y; }
        else { ddx = smoothed[i+1].x - smoothed[i-1].x; ddy = smoothed[i+1].y - smoothed[i-1].y; }
        float ln = sqrtf(ddx*ddx + ddy*ddy);
        if (ln > 0) { ddx /= ln; ddy /= ln; } else { ddx = 1; ddy = 0; }
        nx[i] = -ddy; ny[i] = ddx;
        float ratio = (float)i * invSlMinus1;
        ratios[i] = ratio;
        float taper = powf(1.0f - ratio, 1.3f);
        widths[i] = (i == sl - 1) ? 0.5f : 10.0f * taper * widthMul;
    }

    UpdateConstantBuffer(screenW, screenH, &cols);
    g_pD3DContext->IASetInputLayout(g_pNativeLayout);
    g_pD3DContext->VSSetShader(g_pNativeVS, nullptr, 0);
    g_pD3DContext->PSSetShader(g_pNativePS, nullptr, 0);
    g_pD3DContext->VSSetConstantBuffers(0, 1, &g_pConstantBuffer);
    g_pD3DContext->PSSetConstantBuffers(0, 1, &g_pConstantBuffer);
    g_pD3DContext->RSSetState(g_pRasterState);
    g_pD3DContext->OMSetBlendState(g_pAlphaBlend, nullptr, 0xFFFFFFFF);

    UINT stride = sizeof(VertexPosColor);
    UINT vbOffset = 0;
    g_pD3DContext->IASetVertexBuffers(0, 1, &g_pTrailVB, &stride, &vbOffset);
    g_pD3DContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

    // 一次 Map 批量写入所有层（消除多次 vector 分配 + Map/Unmap）/ Single Map batch-writes all layers
    D3D11_MAPPED_SUBRESOURCE mapped;
    if (FAILED(g_pD3DContext->Map(g_pTrailVB, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped))) return;
    VertexPosColor* dst = (VertexPosColor*)mapped.pData;
    int vtxOffset = 0;
    const int MAX_VTX = 4096;

    struct Layer { int offset, count; bool additive; };
    Layer layers[8];
    int layerCount = 0;

    // 写入一层带，直接写入 mapped 内存，返回顶点数（0=缓冲溢出）/ Write one band layer directly to mapped memory, returns vertex count (0=overflow)
    auto writeBand = [&](float widthScale, float r, float g, float b, float aMul,
                         bool headOnly, float offX, float offY, bool additive) -> int {
        int count = (int)sl * 2;
        if (vtxOffset + count > MAX_VTX) return 0;
        VertexPosColor* p = dst + vtxOffset;
        for (size_t i = 0; i < sl; ++i) {
            float ratio = ratios[i];
            float ow = widths[i] * widthScale;
            float pointAlpha = fadeAlpha * aMul;
            if (headOnly) {
                pointAlpha *= (ratio < 0.25f) ? 0.0f : ((ratio > 0.85f) ? 1.0f : (ratio - 0.25f) / 0.6f);
            }
            p[0] = {smoothed[i].x + offX + nx[i]*ow, smoothed[i].y + offY + ny[i]*ow, 0.0f,
                    r, g, b, pointAlpha, ratio, 1.0f};
            p[1] = {smoothed[i].x + offX - nx[i]*ow, smoothed[i].y + offY - ny[i]*ow, 0.0f,
                    r, g, b, pointAlpha, ratio, -1.0f};
            p += 2;
        }
        vtxOffset += count;
        return count;
    };

    auto pushLayer = [&](int c, bool add) { if (c && layerCount < 8) layers[layerCount++] = {vtxOffset - c, c, add}; };

    // 1. 拖尾阴影（右下偏移深色投影）/ 1. Trail shadow (bottom-right offset dark projection)
    if (g_enableTrailShadow)
        pushLayer(writeBand(1.0f, 0,0,0, 0.18f, false, 1.5f, 2.0f, false), false);
    // 2. 自适应柔和边缘 / 2. Adaptive soft edge
    if (g_adaptiveContrast) {
        // 亮背景用深色边缘增强对比，暗背景用浅色边缘 / Bright bg uses dark edge for contrast, dark bg uses light edge
        float edgeV = g_bgLuminance > 0.5f ? (0.02f + (1.0f - g_bgLuminance) * 0.06f) : (1.2f + g_bgLuminance * 0.8f);
        pushLayer(writeBand(1.18f, edgeV,edgeV,edgeV, 0.22f, false, 0,0, false), false);
    }
    // 3. 外发光三层（宽淡→中→窄亮），加法混合 / Outer glow 3 layers (wide-fade → medium → narrow-bright), additive blending
    if (g_enableGlow) {
        float gi = g_glowIntensity / 100.0f;
        pushLayer(writeBand(2.6f + gi*1.4f, 1,1,1, 0.04f+gi*0.05f, false, 0,0, true), true);
        pushLayer(writeBand(1.7f + gi*0.7f, 1,1,1, 0.09f+gi*0.09f, false, 0,0, true), true);
        if (g_enhancedGlow)
            pushLayer(writeBand(1.25f + gi*0.35f, 1.15f,1.15f,1.15f, 0.14f+gi*0.10f, false, 0,0, true), true);
    }
    // 4. 主体带 / Main body band
    pushLayer(writeBand(1.0f, 1,1,1, 1.0f, false, 0,0, false), false);
    // 5. 头部高亮（只在头部显示）/ Head highlight (only at head)
    if (g_enableHeadHighlight)
        pushLayer(writeBand(0.35f, 1.25f,1.25f,1.25f, 0.75f, true, 0,0, false), false);

    g_pD3DContext->Unmap(g_pTrailVB, 0);

    // 绘制所有层，发光层用加法混合，其他用Alpha混合 / Draw all layers: glow uses additive, rest uses alpha blending
    for (int i = 0; i < layerCount; ++i) {
        if (layers[i].additive)
            g_pD3DContext->OMSetBlendState(g_pAdditiveBlend, nullptr, 0xFFFFFFFF);
        else
            g_pD3DContext->OMSetBlendState(g_pAlphaBlend, nullptr, 0xFFFFFFFF);
        g_pD3DContext->Draw(layers[i].count, layers[i].offset);
    }
    g_pD3DContext->OMSetBlendState(g_pAlphaBlend, nullptr, 0xFFFFFFFF);
}


// 常量宽度线渲染（虚线/闪电/羽毛用）/ Constant-width line rendering (for dashed/lightning/feather)
static void NativeRenderLineTrail(const std::vector<D2D1_POINT_2F>& pts, float width,
                                  const GradData& cols, float fadeAlpha, int screenW, int screenH) {
    if (!g_pNativeVS || !g_pNativePS || !g_pTrailVB || pts.size() < 2) return;
    size_t sl = pts.size();
    std::vector<float> nx(sl), ny(sl);
    for (size_t i = 0; i < sl; ++i) {
        float ddx, ddy;
        if (i == 0) { ddx = pts[1].x - pts[0].x; ddy = pts[1].y - pts[0].y; }
        else if (i == sl - 1) { ddx = pts[i].x - pts[i-1].x; ddy = pts[i].y - pts[i-1].y; }
        else { ddx = pts[i+1].x - pts[i-1].x; ddy = pts[i+1].y - pts[i-1].y; }
        float ln = sqrtf(ddx*ddx + ddy*ddy);
        if (ln > 0) { ddx /= ln; ddy /= ln; } else { ddx = 1; ddy = 0; }
        nx[i] = -ddy; ny[i] = ddx;
    }
    UpdateConstantBuffer(screenW, screenH, &cols);
    g_pD3DContext->IASetInputLayout(g_pNativeLayout);
    g_pD3DContext->VSSetShader(g_pNativeVS, nullptr, 0);
    g_pD3DContext->PSSetShader(g_pNativePS, nullptr, 0);
    g_pD3DContext->VSSetConstantBuffers(0, 1, &g_pConstantBuffer);
    g_pD3DContext->PSSetConstantBuffers(0, 1, &g_pConstantBuffer);
    g_pD3DContext->RSSetState(g_pRasterState);
    g_pD3DContext->OMSetBlendState(g_pAlphaBlend, nullptr, 0xFFFFFFFF);
    UINT stride = sizeof(VertexPosColor);
    UINT vbOffset = 0;
    g_pD3DContext->IASetVertexBuffers(0, 1, &g_pTrailVB, &stride, &vbOffset);
    g_pD3DContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
    D3D11_MAPPED_SUBRESOURCE mapped;
    if (FAILED(g_pD3DContext->Map(g_pTrailVB, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped))) return;
    VertexPosColor* p = (VertexPosColor*)mapped.pData;
    int count = (int)sl * 2;
    if (count > 4096) count = 4096;
    float hw = width * 0.5f;
    for (int i = 0; i < count; ++i) {
        size_t idx = i / 2;
        float ratio = (float)idx / (float)(sl - 1);
        float alpha = fadeAlpha * (1.0f - ratio * 0.3f);
        if (i % 2 == 0) {
            p[0] = {pts[idx].x + nx[idx]*hw, pts[idx].y + ny[idx]*hw, 0, cols.solidOuter.r, cols.solidOuter.g, cols.solidOuter.b, alpha, ratio, 1};
        } else {
            p[0] = {pts[idx].x - nx[idx]*hw, pts[idx].y - ny[idx]*hw, 0, cols.solidOuter.r, cols.solidOuter.g, cols.solidOuter.b, alpha, ratio, -1};
        }
        p++;
    }
    g_pD3DContext->Unmap(g_pTrailVB, 0);
    g_pD3DContext->Draw(count, 0);
}

// 预计算形状局部顶点（消除每帧 cos/sin/pow 重复计算）
struct ShapeVertsCache { const float* verts; int count; };
static ShapeVertsCache GetShapeVerts(int shapeType) {
    // 形状编号：0=heart 1=star 2=hexagon 3=circle 4=diamond 5=triangle 6=flower 7=pentagon 8=hexagram
    // Shape IDs: 0=heart 1=star 2=hexagon 3=circle 4=diamond 5=triangle 6=flower 7=pentagon 8=hexagram
    static float circleVerts[72 * 2];   // 24段 × 3顶点
    static float starVerts[30 * 2];     // 5角 × 2三角 × 3
    static float hexVerts[18 * 2];      // 6边 × 3三角（扇形填充）
    static float heartVerts[72 * 2];    // 24段 × 3
    static float diamondVerts[12 * 2];  // 4边 × 3三角
    static float triVerts[3 * 2];       // 1三角
    static float flowerVerts[30 * 2];   // 5瓣 × 2三角 × 3
    static float pentVerts[15 * 2];     // 5边 × 3三角
    static float hexagramVerts[60 * 2]; // 6角 × 2三角 × 3 + 6个内部三角
    static bool inited = false;
    if (!inited) {
        inited = true;
        const int seg = 24;
        // 圆形 / Circle
        for (int i = 0; i < seg; i++) {
            float a1 = (i / (float)seg) * 6.28318f;
            float a2 = ((i + 1) / (float)seg) * 6.28318f;
            int idx = i * 6;
            circleVerts[idx]=0; circleVerts[idx+1]=0;
            circleVerts[idx+2]=cosf(a1); circleVerts[idx+3]=sinf(a1);
            circleVerts[idx+4]=cosf(a2); circleVerts[idx+5]=sinf(a2);
        }
        // 五角星 / 5-pointed star
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
        // 六边形（扇形填充）/ Hexagon (triangle fan)
        for (int i = 0; i < 6; i++) {
            float a1 = (i / 6.0f) * 6.28318f - 1.5708f;
            float a2 = ((i + 1) / 6.0f) * 6.28318f - 1.5708f;
            int idx = i * 6;
            hexVerts[idx]=0; hexVerts[idx+1]=0;
            hexVerts[idx+2]=cosf(a1); hexVerts[idx+3]=sinf(a1);
            hexVerts[idx+4]=cosf(a2); hexVerts[idx+5]=sinf(a2);
        }
        // 心形 / Heart
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
        // 菱形 / Diamond
        for (int i = 0; i < 4; i++) {
            float a1 = (i / 4.0f) * 6.28318f - 1.5708f;
            float a2 = ((i + 1) / 4.0f) * 6.28318f - 1.5708f;
            int idx = i * 6;
            diamondVerts[idx]=0; diamondVerts[idx+1]=0;
            diamondVerts[idx+2]=cosf(a1); diamondVerts[idx+3]=sinf(a1);
            diamondVerts[idx+4]=cosf(a2); diamondVerts[idx+5]=sinf(a2);
        }
        // 三角形（顶点在上）/ Triangle (point up)
        triVerts[0]=0; triVerts[1]=-1;
        triVerts[2]=-0.866f; triVerts[3]=0.5f;
        triVerts[4]=0.866f; triVerts[5]=0.5f;
        // 花朵（5瓣）/ Flower (5 petals)
        for (int i = 0; i < 5; i++) {
            float a1 = (i / 5.0f) * 6.28318f;
            float a2 = a1 + 0.35f;
            float a3 = a1 - 0.35f;
            int idx = i * 12;
            flowerVerts[idx]=0; flowerVerts[idx+1]=0;
            flowerVerts[idx+2]=0.7f*cosf(a2); flowerVerts[idx+3]=0.7f*sinf(a2);
            flowerVerts[idx+6]=0; flowerVerts[idx+7]=0;
            flowerVerts[idx+8]=0.7f*cosf(a3); flowerVerts[idx+9]=0.7f*sinf(a3);
            flowerVerts[idx+10]=cosf(a1); flowerVerts[idx+11]=sinf(a1);
        }
        // 五边形（扇形填充）/ Pentagon (triangle fan)
        for (int i = 0; i < 5; i++) {
            float a1 = (i / 5.0f) * 6.28318f - 1.5708f;
            float a2 = ((i + 1) / 5.0f) * 6.28318f - 1.5708f;
            int idx = i * 6;
            pentVerts[idx]=0; pentVerts[idx+1]=0;
            pentVerts[idx+2]=cosf(a1); pentVerts[idx+3]=sinf(a1);
            pentVerts[idx+4]=cosf(a2); pentVerts[idx+5]=sinf(a2);
        }
        // 六芒星（六角星）/ Hexagram (6-pointed star)
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
    }
    switch (shapeType) {
        case 0: return {heartVerts, 72};
        case 1: return {starVerts, 30};
        case 2: return {hexVerts, 18};
        case 3: return {circleVerts, 72};
        case 4: return {diamondVerts, 12};
        case 5: return {triVerts, 3};
        case 6: return {flowerVerts, 30};
        case 7: return {pentVerts, 15};
        case 8: return {hexagramVerts, 60};
        default: return {circleVerts, 72};
    }
}

// 形状拖尾原生渲染（v3）：直接生成世界坐标顶点，一次绘制 / Shape trail native rendering (v3): generate world-space verts, single draw
// 公共D3D11渲染状态设置 / Common D3D11 render state setup
static void SetNativeRenderState(int screenW, int screenH, D3D11_PRIMITIVE_TOPOLOGY topology) {
    UpdateConstantBuffer(screenW, screenH);
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

static void NativeRenderTrailShapes(int screenW, int screenH, DWORD dwTime) {
    if (g_trailShapes.empty() || !g_pTrailVB) return;

    std::vector<VertexPosColor> verts;
    verts.reserve(g_trailShapes.size() * 32);

    for (auto &s : g_trailShapes) {
        float progress = (float)(dwTime - s.startTime) / s.lifetime;
        if (progress < 0 || progress >= 1) continue;
        // 生命周期动画：与D2D1渲染对齐 / Lifecycle animation: aligned with D2D1 rendering
        float scale;
        if (progress < 0.2f) {
            scale = progress * 5.0f;
        } else if (progress > 0.7f) {
            scale = 1.0f - (progress - 0.7f) * 2.33f;
            scale = fmaxf(scale, 0.1f);
        } else {
            scale = 1.0f;
        }
        scale *= s.size;
        // 透明度：与D2D1对齐 / Alpha: aligned with D2D1
        float lifeAlpha;
        if (progress < 0.1f) {
            lifeAlpha = progress * 10.0f;
        } else if (progress > 0.7f) {
            lifeAlpha = (1.0f - progress) / 0.3f;
        } else {
            lifeAlpha = 1.0f;
        }
        lifeAlpha *= 0.7f;
        float cr = s.color.r, cg = s.color.g, cb = s.color.b;
        float cosR = cosf(s.rotation), sinR = sinf(s.rotation);

        ShapeVertsCache sv = GetShapeVerts(s.shapeType);
        for (int i = 0; i < sv.count; i++) {
            float lx = sv.verts[i * 2] * scale;
            float ly = sv.verts[i * 2 + 1] * scale;
            float rx = lx * cosR - ly * sinR;
            float ry = lx * sinR + ly * cosR;
            verts.push_back({s.x + rx, s.y + ry, 0.0f, cr, cg, cb, lifeAlpha, 0.5f});
        }
    }

    if (verts.empty()) return;
    if (verts.size() > 4096) verts.resize(4096); // 限制最大顶点数 / Cap max vertices

    D3D11_MAPPED_SUBRESOURCE mapped;
    if (FAILED(g_pD3DContext->Map(g_pTrailVB, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped))) return;
    memcpy(mapped.pData, verts.data(), verts.size() * sizeof(VertexPosColor));
    g_pD3DContext->Unmap(g_pTrailVB, 0);

    SetNativeRenderState(screenW, screenH, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    g_pD3DContext->Draw((UINT)verts.size(), 0);
}

// 波纹原生渲染 / Ripple native rendering
static void NativeRenderRipples(int screenW, int screenH, DWORD dwTime, const GradData& cols, int vX, int vY) {
    if (g_ripples.empty() || !g_pTrailVB) return;

    // 波纹用纯色渲染（不上传渐变，gradientCount=0，PS 直接用顶点色）/ Ripples use solid color (no gradient, PS uses vertex color directly)
    SetNativeRenderState(screenW, screenH, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

    const int segments = 48;
    const int ringVerts = (segments + 1) * 2;
    // 预计算cos/sin表（所有波纹共用，避免重复计算）
    static float cosTable[49], sinTable[49];
    static bool tableInit = false;
    if (!tableInit) {
        for (int i = 0; i <= segments; i++) {
            float a = (i / (float)segments) * 6.28318f;
            cosTable[i] = cosf(a);
            sinTable[i] = sinf(a);
        }
        tableInit = true;
    }

    // 一次 Map 写所有波纹的所有层（消除多次 Map/Unmap）
    D3D11_MAPPED_SUBRESOURCE mapped;
    if (FAILED(g_pD3DContext->Map(g_pTrailVB, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped))) return;
    VertexPosColor* dst = (VertexPosColor*)mapped.pData;
    int vtxOffset = 0;
    const int MAX_VTX = 4096;

    struct RingLayer { int offset, count; bool additive; };
    RingLayer layers[48];
    int layerCount = 0;

    // 写一个环形带：外圈 v=1，内圈 v=-1，PS 软边缘衰减 / Write a ring band: outer v=1, inner v=-1, PS soft-edge fade
    auto writeRing = [&](float px, float py, float radius, float width,
                         float r, float g, float b, float alpha, bool additive) -> int {
        int count = ringVerts;
        if (vtxOffset + count > MAX_VTX) return 0;
        VertexPosColor* p = dst + vtxOffset;
        for (int i = 0; i <= segments; i++) {
            float cosA = cosTable[i], sinA = sinTable[i];
            p[0] = {px + cosA * (radius + width), py + sinA * (radius + width), 0,
                    r, g, b, alpha, 0.5f, 1.0f};
            p[1] = {px + cosA * radius, py + sinA * radius, 0,
                    r, g, b, alpha, 0.5f, -1.0f};
            p += 2;
        }
        vtxOffset += count;
        return count;
    };

    auto pushLayer = [&](int c, bool add) { if (c && layerCount < 48) layers[layerCount++] = {vtxOffset - c, c, add}; };

    for (auto &r : g_ripples) {
        float progress = (float)(dwTime - r.startTime) / g_clickDuration;
        if (progress < 0 || progress >= 1) continue;
        // ease-out 半径：开始扩张快，结束慢，更自然 / Ease-out radius: fast expansion start, slow end, more natural
        float eased = 1.0f - powf(1.0f - progress, 2.0f);
        float radius = g_clickMaxRadius * eased;
        float alpha = (1.0f - progress) * 0.9f;
        float ringWidth = 5.0f + progress * 4.0f;
        // 用渐变头部色（最亮的色）作为波纹纯色 / Use gradient head color (brightest) as ripple solid color
        D2D1_COLOR_F rc = cols.outer[0].color;

        float px = r.pos.x - vX;
        float py = r.pos.y - vY;

        // 1. 外发光层（宽、低透明度，柔和光晕），加法混合 / Outer glow layer (wide, low alpha, soft halo), additive
        pushLayer(writeRing(px, py, radius, ringWidth * 2.5f,
                            rc.r, rc.g, rc.b, alpha * 0.35f, true), true);
        // 2. 主环（不透明主体）/ Main ring (opaque body)
        pushLayer(writeRing(px, py, radius, ringWidth,
                            rc.r, rc.g, rc.b, alpha, false), false);
        // 3. 内圈高亮（颜色提亮 >1.0，PS 中增亮）/ Inner highlight (brightened >1.0, PS boosts)
        pushLayer(writeRing(px, py, radius - ringWidth * 0.3f, ringWidth * 0.5f,
                            rc.r * 1.4f, rc.g * 1.4f, rc.b * 1.4f, alpha * 0.6f, false), false);
    }

    g_pD3DContext->Unmap(g_pTrailVB, 0);

    for (int i = 0; i < layerCount; i++) {
        if (layers[i].additive)
            g_pD3DContext->OMSetBlendState(g_pAdditiveBlend, nullptr, 0xFFFFFFFF);
        else
            g_pD3DContext->OMSetBlendState(g_pAlphaBlend, nullptr, 0xFFFFFFFF);
        g_pD3DContext->Draw(layers[i].count, layers[i].offset);
    }
    g_pD3DContext->OMSetBlendState(g_pAlphaBlend, nullptr, 0xFFFFFFFF);
}

static D2D1_POINT_2F GetPointOnPath(const std::vector<D2D1_POINT_2F> &path, float ratio);

// 圆点链渲染：离散圆点，每个点是一个独立的四边形 / Dot chain rendering: discrete dots, each an independent quad
static void NativeRenderDotChain(const std::vector<D2D1_POINT_2F>& path, float dotSize,
                                 const GradData& cols, float fadeAlpha, int screenW, int screenH) {
    if (!g_pNativeVS || !g_pNativePS || !g_pTrailVB || path.size() < 2) return;

    std::vector<VertexPosColor> verts;
    std::vector<VertexPosColor> shadowVerts;
    verts.reserve(path.size() * 6);  // 每个圆点 6 个顶点（2 个三角形）/ 6 verts per dot (2 triangles)
    shadowVerts.reserve(path.size() * 6);
    const float SH_DX = 1.5f, SH_DY = 2.0f;

    for (size_t i = 0; i < path.size(); i++) {
        float ratio = (float)i / (path.size() - 1);
        float taper = powf(1.0f - ratio, 1.3f);
        float size = dotSize * taper;
        if (size < 0.5f) continue;

        float x = path[i].x, y = path[i].y;
        // 从渐变采样颜色 / Sample color from gradient
        D2D1_GRADIENT_STOP gs = cols.outer[(int)(ratio * (GRAD_STOPS - 1))];
        float r = gs.color.r, g = gs.color.g, b = gs.color.b;

        // 阴影圆点（向右下偏移的深色投影，受 enable_trail_shadow 控制）/ Shadow dot (offset dark projection, controlled by enable_trail_shadow)
        if (g_enableTrailShadow) {
            float sx = x + SH_DX, sy = y + SH_DY, sa = fadeAlpha * 0.18f;
            shadowVerts.push_back({sx - size, sy - size, 0, 0, 0, 0, sa, ratio});
            shadowVerts.push_back({sx + size, sy - size, 0, 0, 0, 0, sa, ratio});
            shadowVerts.push_back({sx - size, sy + size, 0, 0, 0, 0, sa, ratio});
            shadowVerts.push_back({sx + size, sy - size, 0, 0, 0, 0, sa, ratio});
            shadowVerts.push_back({sx + size, sy + size, 0, 0, 0, 0, sa, ratio});
            shadowVerts.push_back({sx - size, sy + size, 0, 0, 0, 0, sa, ratio});
        }

        verts.push_back({x - size, y - size, 0, r, g, b, fadeAlpha, ratio});
        verts.push_back({x + size, y - size, 0, r, g, b, fadeAlpha, ratio});
        verts.push_back({x - size, y + size, 0, r, g, b, fadeAlpha, ratio});
        verts.push_back({x + size, y - size, 0, r, g, b, fadeAlpha, ratio});
        verts.push_back({x + size, y + size, 0, r, g, b, fadeAlpha, ratio});
        verts.push_back({x - size, y + size, 0, r, g, b, fadeAlpha, ratio});
    }

    if (verts.empty() && shadowVerts.empty()) return;
    if (verts.size() > 4096) verts.resize(4096);
    if (shadowVerts.size() > 4096) shadowVerts.resize(4096);

    UpdateConstantBuffer(screenW, screenH, &cols);
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
    g_pD3DContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // 先画阴影，再画主体 / Draw shadow first, then main body
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

// 整数钳制辅助函数 / Integer clamp helper
static inline int ClampInt(int v, int lo, int hi) {
    return v < lo ? lo : (v > hi ? hi : v);
}

// ===== 物理可视化调试（D2D1叠加绘制）===== / Physics debug visualization: each element has independent toggle
static void RenderPhysicsDebugD2D(DWORD dwTime) {
    if (!g_pD2DDC) return;
    // 所有调试都关闭则直接返回 / Return early if all debug toggles off
    if (!g_debugVelocity && !g_debugForce && !g_debugVortex && !g_debugGravity && !g_debugCollision && !g_debugSpring) return;

    // 创建画刷（调试功能，每帧创建可接受）/ Create brushes (debug feature, per-frame creation acceptable)
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

    const float VEC_SCALE = 4.0f;   // 速度向量缩放 / Velocity vector scale
    const float FORCE_SCALE = 10.0f; // 受力向量缩放（增大更明显）/ Force vector scale (larger for visibility)

    // 1. 粒子级调试：速度向量、受力向量、碰撞半径 / 1. Particle-level debug: velocity, force, collision radius
    if (g_debugVelocity || g_debugForce || g_debugCollision) {
        for (auto &p : g_particles) {
            float progress = (float)(dwTime - p.startTime) / p.lifetime;
            if (progress < 0 || progress >= 1) continue;

            D2D1_POINT_2F pos = D2D1::Point2F(p.x, p.y);

            // 碰撞半径：半透明填充 + 高亮轮廓（青色）/ Collision radius: semi-transparent fill + bright outline (cyan)
            if (g_debugCollision) {
                float colRadius = p.size * 1.5f;  // 略大于实际碰撞范围，更易观察 / Slightly larger than actual for visibility
                g_pD2DDC->FillEllipse(D2D1::Ellipse(pos, colRadius, colRadius), brushCyanFill);
                g_pD2DDC->DrawEllipse(D2D1::Ellipse(pos, colRadius, colRadius), brushCyan, 2.0f);
            }

            // 速度向量：绿色线段 + 箭头 / Velocity vector: green line + arrowhead
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
                }
            }

            // 受力向量：红色线段 + 箭头 / Force vector: red line + arrowhead
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
                }
            }
        }
    }

    // 1.5 弹簧连线调试：遍历粒子对，按张力着色 / 1.5 Spring links debug: iterate pairs, color by tension
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
                float tension = (dist - springRestDbg) / g_springMaxDist;  // -0.5..~0.5
                float t = fminf(fmaxf(tension * 2.0f + 0.5f, 0.0f), 1.0f);  // 0=松弛蓝, 1=绷紧红
                D2D1_POINT_2F a = D2D1::Point2F(g_particles[i].x, g_particles[i].y);
                D2D1_POINT_2F b = D2D1::Point2F(g_particles[j].x, g_particles[j].y);
                if (t < 0.5f)
                    g_pD2DDC->DrawLine(a, b, brushSpringLoose, 1.2f);
                else
                    g_pD2DDC->DrawLine(a, b, brushSpringTaut, 1.6f);
            }
        }
    }

    // 2. 漩涡中心和轨道（蓝色）：遍历所有活跃漩涡 / 2. Vortex centers and orbits (blue): iterate all active vortices
    if (g_debugVortex && g_enableCentripetal) {
        for (int vi = 0; vi < g_vortexMaxCount; vi++) {
            Vortex &v = g_vortices[vi];
            if (!v.active || v.strength <= 0.02f) continue;

            D2D1_POINT_2F center = D2D1::Point2F(v.centerX, v.centerY);

            // 中心十字 / Center cross
            g_pD2DDC->DrawLine(D2D1::Point2F(center.x - 10, center.y),
                               D2D1::Point2F(center.x + 10, center.y), brushBlue, 2.5f);
            g_pD2DDC->DrawLine(D2D1::Point2F(center.x, center.y - 10),
                               D2D1::Point2F(center.x, center.y + 10), brushBlue, 2.5f);

            // 轨道圆（多个半径，表示粒子轨道）/ Orbit circles (multiple radii, particle orbits)
            for (int r = 30; r <= 180; r += 30) {
                float radius = r * (0.85f + v.strength * 0.3f);
                g_pD2DDC->DrawEllipse(D2D1::Ellipse(center, radius, radius), brushBlue, 1.2f);
            }

            // 涡核圆（Rankine涡内边界）/ Vortex core circle (Rankine inner boundary)
            g_pD2DDC->DrawEllipse(D2D1::Ellipse(center, v.coreRadius, v.coreRadius), brushBlue, 1.5f);

            // 漩涡强度：实心圆大小表示 / Vortex strength: solid circle size indicates
            float strengthRadius = 8 + v.strength * 25;
            g_pD2DDC->FillEllipse(D2D1::Ellipse(center, strengthRadius, strengthRadius), brushBlue);
        }
    }

    // 3. 引力源（黄色双圆）/ 3. Gravity sources (yellow double circles)
    if (g_debugGravity && g_enableParticleGravity) {
        int n = (int)g_particles.size();
        int bodyCount = (g_gravitySystem == 0) ? 2 : ClampInt(g_gravityBodyCount, 2, 10);
        int drawCount = bodyCount < n ? bodyCount : n;
        // 找出质量最大的bodyCount个粒子作为引力源
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
}

static bool NativeRenderFrame(int screenW, int screenH, const std::vector<D2D1_POINT_2F>& smoothed,
                              bool tailVisible, const GradData& cols, float widthMul, float fadeAlpha,
                              DWORD dwTime, int vX, int vY) {
    if (!g_pNativeVS || !g_pD3DContext || !g_pCachedRTV) return false;

    // 图集脏标记重建（在任何状态绑定前）/ Rebuild atlas on dirty flag (before any state binding)
    if (g_charAtlasDirty && g_particleShape == 10) { BuildCharAtlas(); g_charAtlasDirty = false; }

    // 清空渲染目标（透明黑），RTV 随交换链缓存，避免每帧创建 COM 对象
    float clearColor[4] = {0, 0, 0, 0};
    g_pD3DContext->ClearRenderTargetView(g_pCachedRTV, clearColor);
    g_pD3DContext->OMSetRenderTargets(1, &g_pCachedRTV, nullptr);

    // 设置视口（SSAA 时使用放大后的渲染分辨率）/ Set viewport (use scaled resolution for SSAA)
    int renderW = screenW, renderH = screenH;
    if (g_ssaaScale > 1) {
        renderW = screenW * g_ssaaScale;
        renderH = screenH * g_ssaaScale;
    }
    g_renderW = renderW;
    g_renderH = renderH;
    D3D11_VIEWPORT vp = {0, 0, (float)renderW, (float)renderH, 0, 1};
    g_pD3DContext->RSSetViewports(1, &vp);

    // 1. 粒子渲染（Instanced）/ 1. Particle rendering (instanced)
    // 文字形状(10)走D2D路径，跳过GPU instanced渲染 / Text shape(10) uses D2D path, skip GPU instanced
    if (!g_particles.empty()) {
        NativeRenderParticles(screenW, screenH);
    }

    // 2. 拖尾带渲染（顶点缓冲）/ 2. Trail strip rendering (vertex buffer)
    // 锥形(0)、函数曲线(2)、波形曲线(3)、螺旋(7)、闪电(8)、羽毛(9) 直接渲染 / cone(0), func(2), wave(3), spiral(7), lightning(8), feather(9) direct
    // 点链(1) 用小锥形段近似 / dot chain(1) approximated by small cone segments
    // 双线(5) 渲染两条偏移的带 / double line(5) renders two offset bands
    // 虚线(6) 分段渲染 / dashed(6) segmented rendering
    // 形状拖尾(4) 不渲染带 / shape trail(4) no band
    if (tailVisible && !smoothed.empty() && g_trailShape != 4 && g_trailShape != 10) {
        if (g_trailShape == 1) {
            // 点链：沿路径生成离散圆点 / Dot chain: generate discrete dots along path
            std::vector<D2D1_POINT_2F> dots;
            int dotCount = (int)smoothed.size() * g_dotsMultiplier;
            if (dotCount > 200) dotCount = 200;
            for (int i = 0; i < dotCount; i++) {
                float t = i / (float)(dotCount - 1);
                D2D1_POINT_2F p = GetPointOnPath(smoothed, t);
                dots.push_back(p);
            }
            float dotSize = 4.0f * widthMul * (g_dotChainSize / 100.0f);
            NativeRenderDotChain(dots, dotSize, cols, fadeAlpha, screenW, screenH);
        } else if (g_trailShape == 5) {
            // 双线拖尾：渲染两条偏移的带 / Double line trail: render two offset bands
            std::vector<D2D1_POINT_2F> line1, line2;
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
            NativeRenderTrail(line1, widthMul * 0.5f, cols, fadeAlpha, screenW, screenH, dwTime);
            NativeRenderTrail(line2, widthMul * 0.5f, cols, fadeAlpha, screenW, screenH, dwTime);
        } else if (g_trailShape == 6) {
            // 虚线拖尾：常量宽度分段线，避免锥形段变成三角形 / Dashed: constant-width segments, avoid tapered wedges
            std::vector<D2D1_POINT_2F> segment;
            int segLen = 6, gapLen = 4;
            for (size_t i = 0; i < smoothed.size(); i++) {
                segment.push_back(smoothed[i]);
                if ((int)(i + 1) % (segLen + gapLen) == 0) {
                    if (segment.size() >= 2)
                        NativeRenderLineTrail(segment, 6.0f * widthMul, cols, fadeAlpha, screenW, screenH);
                    segment.clear();
                    i += gapLen;
                }
            }
            if (segment.size() >= 2)
                NativeRenderLineTrail(segment, 6.0f * widthMul, cols, fadeAlpha, screenW, screenH);
        } else if (g_trailShape == 8) {
            // 闪电拖尾：细亮锯齿线 + 分支 / Lightning: thin bright jagged line + branches
            NativeRenderLineTrail(smoothed, 3.0f * widthMul, cols, fadeAlpha, screenW, screenH);
            // 随机分支：在路径中间点向外发射短线 / Random branches: short lines from midpoints
            for (size_t i = 2; i + 2 < smoothed.size(); i += 3) {
                if ((rand() / (float)RAND_MAX) > 0.35f) continue;
                float dx = smoothed[i+1].x - smoothed[i-1].x;
                float dy = smoothed[i+1].y - smoothed[i-1].y;
                float ln = sqrtf(dx*dx + dy*dy);
                if (ln < 0.001f) continue;
                float nx = -dy / ln, ny = dx / ln;
                float blen = 15.0f + (rand() / (float)RAND_MAX) * 25.0f;
                float side = ((rand() / (float)RAND_MAX) > 0.5f) ? 1.0f : -1.0f;
                std::vector<D2D1_POINT_2F> branch = {
                    smoothed[i],
                    {smoothed[i].x + nx * side * blen * 0.5f + dx * 0.3f,
                     smoothed[i].y + ny * side * blen * 0.5f + dy * 0.3f},
                    {smoothed[i].x + nx * side * blen,
                     smoothed[i].y + ny * side * blen}
                };
                NativeRenderLineTrail(branch, 1.5f * widthMul, cols, fadeAlpha * 0.6f, screenW, screenH);
            }
        } else if (g_trailShape == 9) {
            // 羽毛拖尾：细主轴 + 两侧斜向羽枝 / Feather: thin shaft + angled side barbs
            NativeRenderLineTrail(smoothed, 2.5f * widthMul, cols, fadeAlpha, screenW, screenH);
            // 羽枝：每隔几个点向两侧画斜短线 / Barbs: angled short lines on both sides every few points
            for (size_t i = 1; i + 1 < smoothed.size(); i += 2) {
                float dx = smoothed[i+1].x - smoothed[i-1].x;
                float dy = smoothed[i+1].y - smoothed[i-1].y;
                float ln = sqrtf(dx*dx + dy*dy);
                if (ln < 0.001f) continue;
                float nx = -dy / ln, ny = dx / ln;
                float barbLen = 10.0f * (1.0f - (float)i / smoothed.size()) + 3.0f;
                // 羽枝向后倾斜（沿路径反方向）/ Barbs angle backward along path
                float bx = -dx / ln, by = -dy / ln;
                for (int side = -1; side <= 1; side += 2) {
                    std::vector<D2D1_POINT_2F> barb = {
                        smoothed[i],
                        {smoothed[i].x + nx * side * barbLen + bx * barbLen * 0.4f,
                         smoothed[i].y + ny * side * barbLen + by * barbLen * 0.4f}
                    };
                    NativeRenderLineTrail(barb, 1.0f * widthMul, cols, fadeAlpha * 0.5f, screenW, screenH);
                }
            }
        } else {
            // 锥形/函数/波形/螺旋 / Cone/func/wave/spiral
            NativeRenderTrail(smoothed, widthMul, cols, fadeAlpha, screenW, screenH, dwTime);
        }
    }

    // 3. 点击波纹环渲染（顶点缓冲）/ 3. Click ripple ring rendering (vertex buffer)
    if (g_enableClickEffect && !g_ripples.empty()) {
        NativeRenderRipples(screenW, screenH, dwTime, cols, vX, vY);
    }

    // 4. 运动模糊：绘制历史路径（透明度递减）/ 4. Motion blur: draw history paths (decreasing alpha)
    if (g_enableMotionBlur && tailVisible && !g_trailHistory.empty() && g_trailShape != 10) {
        for (size_t h = 0; h < g_trailHistory.size(); h++) {
            float histAlpha = fadeAlpha * (0.15f + 0.1f * (float)h / g_trailHistory.size());
            if (histAlpha < 0.02f) continue;
            const std::vector<D2D1_POINT_2F>& histPath = g_trailHistory[h].path;
            if (histPath.size() < 2) continue;
            NativeRenderTrail(histPath, widthMul, cols, histAlpha, screenW, screenH, dwTime);
        }
    }

    // 5. 形状拖尾渲染（顶点缓冲）/ 5. Shape trail rendering (vertex buffer)
    if (!g_trailShapes.empty()) {
        NativeRenderTrailShapes(screenW, screenH, dwTime);
    }

    // 恢复 D2D1 状态（如果后续还有 D2D1 渲染）/ Restore D2D1 state (if more D2D1 rendering follows)
    g_pD2DDC->SetTarget(g_pD2DTargetBitmap);
    return true;
}

static inline float Rand01() {
    return rand() / (float)RAND_MAX;
}
// 获取粒子数量上限 / Get particle count cap
static inline int GetParticleCap() {
    return g_superPerformanceMode ? 1200 : 500;
}
static inline float Hash01(int n) {
    uint32_t u = (uint32_t)n;
    u = (u << 13) ^ u;
    return (float)(((u * (u * u * 15731u + 789221u) + 1376312589u) & 0x7fffffffu) / 2147483647.0);
}
static void SpawnParticles(float x, float y, int count, float speedMin, float speedMax, float sizeMin, float sizeMax,
                           int lifeMin, int lifeMax, D2D1_COLOR_F color, DWORD time, bool radial = false,
                           int shapeType = -1) {
    // 容量保护：剩余可生成数量不足时截断，避免瞬时超过粒子上限造成内存/性能尖峰 / Capacity guard: truncate when room insufficient to avoid transient over-cap spikes
    int cap = GetParticleCap();
    int room = cap - (int)g_particles.size();
    if (room <= 0) return;
    if (count > room) count = room;
    D2D1_COLOR_F endCol = D2D1::ColorF(color.r * 0.35f, color.g * 0.35f, color.b * 0.35f, 1.0f);  // 结束色不要太暗 / End color not too dark
    // 预计算公共因子，避免循环内重复计算 / Precompute common factors to avoid repeated calculation in loop
    float massMean = (g_particleMassMin + g_particleMassMax) * 0.5f;
    float massStddev = (g_particleMassMax - g_particleMassMin) * 0.25f;
    float baseSpin = (float)g_particleSpinSpeed / 100.0f * 0.3f;
    float twoPi = 6.2831853f;
    // radial放射模式加随机起始相位，避免粒子数过少时方向总从0°（右侧）开始、下半圈无粒子 / Radial mode: random start phase so few particles cover all directions
    float radialPhase = radial ? (Rand01() * twoPi) : 0.0f;
    for (int i = 0; i < count; i++) {
        float angle = radial ? (radialPhase + i / (float)count * twoPi) : (Rand01() * twoPi);
        float speed = speedMin + Rand01() * (speedMax - speedMin);
        int st = shapeType;
        if (st < 0) {
            st = g_particleShape;
            if (st == 9)  // random: 0-8 shapes / 随机：0-8种形状
                st = (int)(Rand01() * 9.0f);
        }
        // 生成位置随机偏移：在生成点周围分布，避免所有粒子从同一点发射 / Spawn position jitter: distribute around spawn point to avoid all particles from same origin
        float spawnAngle = Rand01() * twoPi;
        float spawnRadius = Rand01() * 8.0f;  // 0-8 像素的随机偏移 / 0-8px random offset
        float px = x + cosf(spawnAngle) * spawnRadius;
        float py = y + sinf(spawnAngle) * spawnRadius;
        if (st == 10) { px += (float)g_textOffsetX; py += (float)g_textOffsetY; }  // text particle spawn offset vs cursor
        Particle p;
        p.x = px; p.y = py;
        p.vx = cosf(angle) * speed; p.vy = sinf(angle) * speed;
        // 随机质量：正态分布（Box-Muller变换）/ Random mass: normal distribution (Box-Muller transform)
        float mass = 1.0f;
        if (g_enableParticleMass) {
            float u1 = fmaxf(Rand01(), 0.001f);
            float u2 = Rand01();
            float z = sqrtf(-2.0f * logf(u1)) * cosf(twoPi * u2);
            mass = massMean + z * massStddev;
            mass = fmaxf(g_particleMassMin, fminf(g_particleMassMax, mass));
        }
        p.mass = mass;
        // 随机电荷：50%正，50%负（洛伦兹力用）/ Random charge: 50% positive, 50% negative (for Lorentz force)
        p.charge = (Rand01() > 0.5f) ? 1.0f : -1.0f;
        // 体积∝质量，半径∝质量^(1/3)，用快速近似替代powf / Volume ∝ mass, radius ∝ mass^(1/3), use fast approx instead of powf
        float massSizeFactor = 1.0f;
        if (g_enableParticleMass) {
            float m = mass * 0.1f;  // 归一化到0.5-2.0范围 / Normalize to 0.5-2.0 range
            massSizeFactor = 0.7f + m * 0.3f;  // 线性近似cube root，足够接近且更快 / Linear cube root approx, close enough and faster
        }
        p.size = (sizeMin + Rand01() * (sizeMax - sizeMin)) * massSizeFactor;
        p.startTime = time;
        // 质量大的粒子生命周期更长（惯性大，消散慢）/ Heavier particles live longer (more inertia, slower dissipation)
        int baseLife = lifeMin + (int)(Rand01() * (lifeMax - lifeMin));
        p.lifetime = g_enableParticleMass ? (int)(baseLife * (0.7f + mass * 0.3f)) : baseLife;
        p.color = color;
        p.endColor = endCol;
        p.shapeType = st;
        // 文字形状：从 text_content 取一个字符 / Text shape: pick a char from text_content
        p.textChar = 0;
        p.charIndex = 0;
        p.chunkStart = 0;
        p.chunkCount = 1;
        if (st == 10 && !g_phraseChunkStart.empty()) {
            // 随机选一个原始词组并记录其格子区间，初始显示第一段 / Pick a random original phrase, record its cell range, start on first chunk
            int phraseCount = (int)g_phraseChunkStart.size();
            int pi = (int)(Rand01() * (float)phraseCount);
            if (pi >= phraseCount) pi = phraseCount - 1;
            p.chunkStart = g_phraseChunkStart[pi];
            p.chunkCount = g_phraseChunkCount[pi];
            if (p.chunkCount < 1) p.chunkCount = 1;
            p.charIndex = (float)p.chunkStart;
        }
        p.z = (Rand01() - 0.5f) * 0.8f;
        p.rotation = Rand01() * twoPi;
        // 随机自旋转速度，每个粒子有 ±50% 差异 / Random spin speed, ±50% variation per particle
        float variation = (Rand01() - 0.5f) * baseSpin;
        p.spinSpeed = baseSpin + variation;
        // 用一次随机数生成三个颜色偏移（减少Rand01调用）/ Generate three color offsets from one random call (reduce Rand01 calls)
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

// ===================== 音乐响应系统（v3.4 基础设施）===================== / Music reactive system (v3.4 infrastructure, visual effects TBD)

// Cooley-Tukey radix-2 FFT（原地，输入输出交替存储）/ Cooley-Tukey radix-2 FFT (in-place)
static void FFT(std::vector<float> &real, std::vector<float> &imag, bool inverse = false) {
    int n = (int)real.size();
    if (n <= 1) return;
    // 位反转排序 / Bit-reversal permutation
    for (int i = 1, j = 0; i < n; i++) {
        int bit = n >> 1;
        for (; j & bit; bit >>= 1) j ^= bit;
        j ^= bit;
        if (i < j) {
            std::swap(real[i], real[j]);
            std::swap(imag[i], imag[j]);
        }
    }
    // 蝶形运算 / Butterfly operations
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

// 初始化Hann窗函数 / Initialize Hann window
static void InitWindowFunction(std::vector<float> &window, int size) {
    window.resize(size);
    for (int i = 0; i < size; i++) {
        window[i] = 0.5f * (1.0f - cosf(2.0f * 3.14159265358979f * i / (size - 1)));
    }
}

// WASAPI 音频捕获线程 / WASAPI audio capture thread
static DWORD WINAPI MusicCaptureThread(LPVOID param) {
    MusicState *m = &g_music;
    // 设置MMCSS优先级 / Set MMCSS priority
    DWORD taskIndex = 0;
    HANDLE hAvrt = AvSetMmThreadCharacteristicsW(L"Audio", &taskIndex);
    while (m->captureRunning.load()) {
        // 等待捕获缓冲区就绪 / Wait for capture buffer
        UINT32 packetLength = 0;
        HRESULT hr = m->pCaptureClient->GetNextPacketSize(&packetLength);
        if (FAILED(hr) || packetLength == 0) {
            WaitForSingleObject(m->hStopEvent, 5);
            continue;
        }
        // 读取捕获数据 / Read captured data
        BYTE *pData = nullptr;
        UINT32 numFrames = 0;
        DWORD flags = 0;
        hr = m->pCaptureClient->GetBuffer(&pData, &numFrames, &flags, nullptr, nullptr);
        if (SUCCEEDED(hr) && pData && numFrames > 0) {
            int channels = m->pWaveFormat->nChannels;
            if (channels < 1) channels = 1;  // 声道除零保护
            int bytesPerSample = m->pWaveFormat->wBitsPerSample / 8;
            bool isSilent = (flags & AUDCLNT_BUFFERFLAGS_SILENT) != 0;
            // 转换为单声道浮点样本并写入环形缓冲 / Convert to mono float and write to ring buffer
            for (UINT32 i = 0; i < numFrames; i++) {
                float sample = 0;
                if (!isSilent) {
                    if (bytesPerSample == 4) {
                        // 32-bit float / 32位浮点
                        float *pFloat = (float*)(pData + i * channels * bytesPerSample);
                        for (int c = 0; c < channels; c++) sample += pFloat[c];
                        sample /= channels;
                    } else if (bytesPerSample == 2) {
                        // 16-bit integer / 16位整数
                        short *pShort = (short*)(pData + i * channels * bytesPerSample);
                        for (int c = 0; c < channels; c++) sample += pShort[c];
                        sample /= (channels * 32768.0f);
                    }
                    // 钳制到[-1,1]，防止异常样本污染FFT / Clamp to [-1,1]
                    if (sample > 1.0f) sample = 1.0f; else if (sample < -1.0f) sample = -1.0f;
                }
                m->fftInput[m->fftWritePos] = sample;
                m->fftWritePos = (m->fftWritePos + 1) % m->fftSize;
            }
            m->pCaptureClient->ReleaseBuffer(numFrames);
        }
    }
    if (hAvrt) AvRevertMmThreadCharacteristics(hAvrt);
    return 0;
}

// 启动音乐捕获 / Start music capture
static bool MusicStartCapture() {
    MusicState *m = &g_music;
    if (m->capturing) return true;
    // 确保COM已初始化 / Ensure COM is initialized（与 MusicStopCapture 中的 CoUninitialize 配对）
    CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
    REFERENCE_TIME hnsRequestedDuration = 10000000; // 1秒缓冲（提前声明，避免 goto 跨越初始化）
    // 创建设备枚举器 / Create device enumerator
    HRESULT hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL,
                                  __uuidof(IMMDeviceEnumerator), (void**)&m->pEnumerator);
    if (FAILED(hr)) { Wh_Log(L"Music: CoCreateInstance failed 0x%08X", hr); CoUninitialize(); return false; }
    // 获取默认渲染设备（用于loopback）/ Get default render device for loopback
    hr = m->pEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &m->pDevice);
    if (FAILED(hr)) { Wh_Log(L"Music: GetDefaultAudioEndpoint failed 0x%08X", hr); goto fail; }
    // 激活音频客户端 / Activate audio client
    hr = m->pDevice->Activate(__uuidof(IAudioClient), CLSCTX_ALL, nullptr, (void**)&m->pAudioClient);
    if (FAILED(hr)) { Wh_Log(L"Music: Activate failed 0x%08X", hr); goto fail; }
    // 获取混合格式 / Get mix format
    hr = m->pAudioClient->GetMixFormat(&m->pWaveFormat);
    if (FAILED(hr)) { Wh_Log(L"Music: GetMixFormat failed 0x%08X", hr); goto fail; }
    // 初始化loopback捕获 / Initialize loopback capture
    hr = m->pAudioClient->Initialize(AUDCLNT_SHAREMODE_SHARED,
                                     AUDCLNT_STREAMFLAGS_LOOPBACK,
                                     hnsRequestedDuration, 0, m->pWaveFormat, nullptr);
    if (FAILED(hr)) { Wh_Log(L"Music: Initialize loopback failed 0x%08X", hr); goto fail; }
    // 获取缓冲区大小 / Get buffer size
    hr = m->pAudioClient->GetBufferSize(&m->bufferFrames);
    if (FAILED(hr)) { Wh_Log(L"Music: GetBufferSize failed 0x%08X", hr); goto fail; }
    // 获取捕获客户端 / Get capture client
    hr = m->pAudioClient->GetService(__uuidof(IAudioCaptureClient), (void**)&m->pCaptureClient);
    if (FAILED(hr)) { Wh_Log(L"Music: GetService capture failed 0x%08X", hr); goto fail; }
    // 初始化FFT缓冲 / Initialize FFT buffers
    m->fftSize = g_musicFftSize;
    m->fftInput.assign(m->fftSize, 0.0f);
    m->fftOutput.assign(m->fftSize / 2, 0.0f);
    m->fftSmoothed.assign(m->fftSize / 2, 0.0f);
    m->fftWorkReal.assign(m->fftSize, 0.0f);
    m->fftWorkImag.assign(m->fftSize, 0.0f);
    InitWindowFunction(m->windowFunction, m->fftSize);
    m->fftWritePos = 0;
    // 启动捕获 / Start capture
    hr = m->pAudioClient->Start();
    if (FAILED(hr)) { Wh_Log(L"Music: Start failed 0x%08X", hr); goto fail; }
    // 创建捕获线程 / Create capture thread
    m->hStopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    m->captureRunning.store(true);
    m->hCaptureThread = CreateThread(nullptr, 0, MusicCaptureThread, nullptr, 0, nullptr);
    if (!m->hCaptureThread || !m->hStopEvent) { Wh_Log(L"Music: capture thread/event creation failed"); goto fail; }
    m->capturing = true;
    m->initialized = true;
    Wh_Log(L"Music: capture started, %d Hz, %d channels, FFT %d",
           m->pWaveFormat->nSamplesPerSec, m->pWaveFormat->nChannels, m->fftSize);
    return true;

fail:
    // 统一失败清理：释放所有已分配资源，避免COM泄漏 / Unified failure cleanup
    if (m->hCaptureThread) {
        m->captureRunning.store(false);
        if (m->hStopEvent) SetEvent(m->hStopEvent);
        WaitForSingleObject(m->hCaptureThread, 1000);
        CloseHandle(m->hCaptureThread);
        m->hCaptureThread = nullptr;
    }
    if (m->hStopEvent) { CloseHandle(m->hStopEvent); m->hStopEvent = nullptr; }
    if (m->pAudioClient) { m->pAudioClient->Release(); m->pAudioClient = nullptr; }
    if (m->pCaptureClient) { m->pCaptureClient->Release(); m->pCaptureClient = nullptr; }
    if (m->pDevice) { m->pDevice->Release(); m->pDevice = nullptr; }
    if (m->pEnumerator) { m->pEnumerator->Release(); m->pEnumerator = nullptr; }
    if (m->pWaveFormat) { CoTaskMemFree(m->pWaveFormat); m->pWaveFormat = nullptr; }
    CoUninitialize();
    return false;
}

// 停止音乐捕获 / Stop music capture
static void MusicStopCapture() {
    MusicState *m = &g_music;
    if (!m->capturing) return;
    m->captureRunning.store(false);
    if (m->hStopEvent) { SetEvent(m->hStopEvent); }
    if (m->hCaptureThread) {
        WaitForSingleObject(m->hCaptureThread, 2000);
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
    CoUninitialize();  // 与 MusicStartCapture 中的 CoInitializeEx 配对
    Wh_Log(L"Music: capture stopped");
}

// 每帧更新音乐分析（FFT + 节拍检测 + 频段分析）/ Per-frame music analysis (FFT + beat detection + frequency bands)
static void MusicUpdate(DWORD dwTime) {
    MusicState *m = &g_music;
    if (!m->capturing || !m->initialized) return;
    int n = m->fftSize;
    // 复用预分配工作缓冲，避免每帧堆分配（FFT大小变化时重新分配）/ Reuse preallocated work buffers, reallocate only when FFT size changes
    if ((int)m->fftWorkReal.size() != n) {
        m->fftWorkReal.assign(n, 0.0f);
        m->fftWorkImag.assign(n, 0.0f);
    }
    // 从环形缓冲复制数据并加窗，同时计算maxSample（合并两次遍历）/ Copy from ring buffer and apply window, compute maxSample in same pass
    float maxSample = 0;
    for (int i = 0; i < n; i++) {
        int idx = (m->fftWritePos + i) % n;
        float sample = m->fftInput[idx];
        m->fftWorkReal[i] = sample * m->windowFunction[i];
        m->fftWorkImag[i] = 0.0f;
        float absSample = fabsf(sample);
        if (absSample > maxSample) maxSample = absSample;
    }
    // 执行FFT / Execute FFT
    FFT(m->fftWorkReal, m->fftWorkImag, false);
    // 计算幅度谱 / Compute magnitude spectrum
    // 平滑系数上限0.95，避免100%时新数据权重为0导致频谱冻结 / Smoothing factor capped at 0.95 to prevent spectrum freeze at 100%
    float smoothFactor = fminf(g_musicSmoothing * 0.01f, 0.95f); // 0~0.95
    float totalEnergy = 0;
    for (int i = 0; i < n / 2; i++) {
        float mag = sqrtf(m->fftWorkReal[i] * m->fftWorkReal[i] + m->fftWorkImag[i] * m->fftWorkImag[i]) / n;
        m->fftOutput[i] = mag;
        // 平滑 / Smoothing
        m->fftSmoothed[i] = m->fftSmoothed[i] * smoothFactor + mag * (1.0f - smoothFactor);
        totalEnergy += mag * mag;
    }
    // 频段分析 / Frequency band analysis（预计算频段边界索引，避免每帧算freq）
    if (m->pWaveFormat) {
        float sampleRate = (float)m->pWaveFormat->nSamplesPerSec;
        float freqPerBin = sampleRate / n;
        // 预计算频段边界：bass<250Hz, mid<2000Hz, treble<20000Hz / Precompute band boundaries: bass<250Hz, mid<2000Hz, treble<20000Hz
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

    // ===== 多方式节拍检测 / Multi-method beat detection =====
    // 设置语义：值越低越灵敏。g=0(最灵敏)阈值1.15，g=1(最不灵敏)阈值2.2 / Semantics: lower = more sensitive. g=0 threshold 1.15, g=1 threshold 2.2
    float sensitivity = 1.15f + g_musicSensitivity * 1.05f;
    // 额外触发门限同样随灵敏度提高，避免三个检测器 OR 叠加导致高设置仍频繁误触发 / Extra gates scale with sensitivity to prevent OR-combined false triggers at high settings
    float energyGate = 1.05f + g_musicSensitivity * 0.45f;
    float fluxGate = 1.10f + g_musicSensitivity * 0.50f;
    float bassGate = 1.08f + g_musicSensitivity * 0.50f;

    // 方式1：总能量阈值法 / Method 1: Total energy threshold
    m->beatEnergy = totalEnergy;
    m->beatAvgEnergy = m->beatAvgEnergy * 0.92f + totalEnergy * 0.08f;
    m->beatThreshold = m->beatAvgEnergy * sensitivity;
    bool energyBeat = (totalEnergy > m->beatThreshold) && (totalEnergy > m->beatAvgEnergy * energyGate);

    // 方式2：频谱通量法（相邻帧频谱变化量）/ Method 2: Spectral flux
    if (m->prevSpectrum.size() != (size_t)(n / 2)) {
        m->prevSpectrum.assign(n / 2, 0.0f);
    }
    float flux = 0;
    for (int i = 0; i < n / 2; i++) {
        float diff = m->fftSmoothed[i] - m->prevSpectrum[i];
        if (diff > 0) flux += diff;  // 只计正变化（能量增加）/ Count only positive changes (energy increase)
        m->prevSpectrum[i] = m->fftSmoothed[i];
    }
    m->spectralFlux = flux;
    m->avgSpectralFlux = m->avgSpectralFlux * 0.9f + flux * 0.1f;
    bool fluxBeat = (flux > m->avgSpectralFlux * sensitivity) && (flux > m->avgSpectralFlux * fluxGate);

    // 方式3：多频段节拍检测 / Method 3: Multi-band beat detection
    m->bassBeatEnergy = m->bassLevel;
    m->bassBeatAvg = m->bassBeatAvg * 0.9f + m->bassLevel * 0.1f;
    m->bassBeat = (m->bassLevel > m->bassBeatAvg * sensitivity) && (m->bassLevel > m->bassBeatAvg * bassGate);

    m->midBeatEnergy = m->midLevel;
    m->midBeatAvg = m->midBeatAvg * 0.9f + m->midLevel * 0.1f;
    m->midBeat = (m->midLevel > m->midBeatAvg * sensitivity * 1.1f);

    m->trebleBeatEnergy = m->trebleLevel;
    m->trebleBeatAvg = m->trebleBeatAvg * 0.9f + m->trebleLevel * 0.1f;
    m->trebleBeat = (m->trebleLevel > m->trebleBeatAvg * sensitivity * 1.2f);

    // 综合判断：任一方式触发即算节拍，低频节拍权重最高 / Combined: any method triggers, bass has highest weight
    bool newBeat = energyBeat || fluxBeat || m->bassBeat;
    // 节拍强度：综合多种触发方式 / Beat intensity: combined from multiple methods
    float intensity = 0;
    if (energyBeat) intensity += 0.4f * fminf(totalEnergy / fmaxf(m->beatAvgEnergy, 0.001f), 2.0f);
    if (fluxBeat) intensity += 0.3f * fminf(flux / fmaxf(m->avgSpectralFlux, 0.001f), 2.0f);
    if (m->bassBeat) intensity += 0.5f * fminf(m->bassLevel / fmaxf(m->bassBeatAvg, 0.001f), 2.5f);
    if (m->midBeat) intensity += 0.2f;
    if (m->trebleBeat) intensity += 0.15f;
    m->beatIntensity = fminf(intensity, 1.0f);

    // 节拍去抖（最小间隔120ms = 500BPM上限）/ Beat debounce
    if (newBeat && (dwTime - m->lastBeatTime) > 120) {
        m->beatDetected = true;
        m->lastBeatTime = dwTime;
        m->beatHistory.push_back(dwTime);
        if (m->beatHistory.size() > 16) m->beatHistory.pop_front();
        // BPM估计 / BPM estimation
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

    // ===== 音乐物理联动状态更新（优化持续能力）===== / Music-physics linkage state update (optimized persistence)
    if (g_enableMusicPhysics) {
        // 节拍脉冲：触发时叠加（而非替换），缓慢衰减，延长作用时间 / Beat pulse: accumulate (not replace) on trigger, slow decay for longer effect
        if (m->beatDetected) {
            float pulseAdd = m->beatIntensity * g_musicBeatPulse * 1.5f;
            g_musicBeatPulseAmount = fminf(g_musicBeatPulseAmount + pulseAdd, 1.5f);  // 叠加，上限1.5 / Accumulate, cap 1.5
        } else {
            g_musicBeatPulseAmount *= 0.94f;  // 缓慢衰减（约0.5秒作用时间）/ Slow decay (~0.5s effect duration)
        }
        // 低频大小增益：平滑跟随bassLevel，攻击快释放慢 / Bass size boost: smooth follow bassLevel, fast attack slow release
        float bassTarget = fminf(m->bassLevel * 15.0f, 1.0f) * g_musicBassSize;
        float bassLerp = (bassTarget > g_musicBassSizeBoost) ? 0.3f : 0.08f;
        g_musicBassSizeBoost += (bassTarget - g_musicBassSizeBoost) * bassLerp;
        // 多频段联动：低频→引力，中频→排斥力，高频→噪声 / Multi-band: bass→gravity, mid→repulsion, treble→noise
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

// 沿路径获取指定比例（0=头，1=尾）的坐标 / Get coordinate at specified ratio along path (0=head, 1=tail)
// Catmull-Rom 样条插值：生成更平滑的曲线 / Catmull-Rom spline interpolation: generates smoother curves
static void CatmullRomSmooth(const std::vector<D2D1_POINT_2F> &input, std::vector<D2D1_POINT_2F> &output, int segments = 4) {
    if (input.size() < 3) {
        output = input;
        return;
    }
    output.clear();
    output.push_back(input.front());
    for (size_t i = 0; i < input.size() - 1; i++) {
        D2D1_POINT_2F p0 = (i > 0) ? input[i - 1] : input[i];
        D2D1_POINT_2F p1 = input[i];
        D2D1_POINT_2F p2 = input[i + 1];
        D2D1_POINT_2F p3 = (i + 2 < input.size()) ? input[i + 2] : input[i + 1];
        for (int s = 1; s <= segments; s++) {
            float t = (float)s / segments;
            float t2 = t * t, t3 = t2 * t;
            float x = 0.5f * ((2 * p1.x) + (-p0.x + p2.x) * t + (2 * p0.x - 5 * p1.x + 4 * p2.x - p3.x) * t2 + (-p0.x + 3 * p1.x - 3 * p2.x + p3.x) * t3);
            float y = 0.5f * ((2 * p1.y) + (-p0.y + p2.y) * t + (2 * p0.y - 5 * p1.y + 4 * p2.y - p3.y) * t2 + (-p0.y + 3 * p1.y - 3 * p2.y + p3.y) * t3);
            output.push_back(D2D1::Point2F(x, y));
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

// ===================== 设置加载 =====================
void LoadSettings() {
    g_triggerVelocity = (float)Wh_GetIntSetting(L"trigger_velocity");
    g_stopVelocity = (float)Wh_GetIntSetting(L"stop_velocity");
    g_tailOffsetX = Wh_GetIntSetting(L"tail_offset_x");
    g_tailOffsetY = Wh_GetIntSetting(L"tail_offset_y");
    g_tailLength = Wh_GetIntSetting(L"tail_length");
    g_trailDelay = Wh_GetIntSetting(L"trail_delay");
    {
        PCWSTR fstr = Wh_GetStringSetting(L"fadeout_mode");
        if (fstr) {
            if (wcscmp(fstr, L"hard") == 0)
                g_fadeoutMode = 0;
            else if (wcscmp(fstr, L"accelerate") == 0)
                g_fadeoutMode = 1;
            else
                g_fadeoutMode = 2;
            Wh_FreeStringSetting(fstr);
        }
    }
    g_enableSpeedResponse = Wh_GetIntSetting(L"enable_speed_response") != 0;
    g_enhancedGlow = Wh_GetIntSetting(L"enhanced_glow") != 0;
    g_enableHeadHighlight = Wh_GetIntSetting(L"enable_head_highlight") != 0;
    g_adaptiveContrast = Wh_GetIntSetting(L"enable_adaptive_contrast") != 0;
    g_dotsMultiplier = Wh_GetIntSetting(L"dots_multiplier");
    g_waveAmplitude = Wh_GetIntSetting(L"wave_amplitude");
    g_waveFrequency = Wh_GetIntSetting(L"wave_frequency");
    g_enableGlow = Wh_GetIntSetting(L"enable_glow") != 0;
    g_glowIntensity = Wh_GetIntSetting(L"glow_intensity");
    g_edgeSoftness = Wh_GetIntSetting(L"edge_softness");
    // 抗锯齿模式 / AA mode
    {
        PCWSTR aaStr = Wh_GetStringSetting(L"aa_mode");
        if (aaStr) {
            if (wcscmp(aaStr, L"off") == 0) g_aaMode = 0;
            else if (wcscmp(aaStr, L"crisp") == 0) g_aaMode = 2;
            else if (wcscmp(aaStr, L"extra") == 0) g_aaMode = 3;
            else g_aaMode = 1;  // smooth default
            Wh_FreeStringSetting(aaStr);
        }
    }
    // MSAA 级别 / MSAA level
    bool enableMsaa = Wh_GetIntSetting(L"enable_msaa") != 0;
    if (enableMsaa) {
        PCWSTR msaaStr = Wh_GetStringSetting(L"msaa_level");
        if (msaaStr) {
            if (wcscmp(msaaStr, L"2x") == 0) g_msaaSamples = 2;
            else if (wcscmp(msaaStr, L"4x") == 0) g_msaaSamples = 4;
            else if (wcscmp(msaaStr, L"8x") == 0) g_msaaSamples = 8;
            else g_msaaSamples = 4;
            Wh_FreeStringSetting(msaaStr);
        }
    } else {
        g_msaaSamples = 1;
    }
    // SSAA 缩放 / SSAA scale
    bool enableSsaa = Wh_GetIntSetting(L"enable_ssaa") != 0;
    if (enableSsaa) {
        PCWSTR ssaaStr = Wh_GetStringSetting(L"ssaa_level");
        if (ssaaStr) {
            if (wcscmp(ssaaStr, L"4x") == 0) g_ssaaScale = 4;
            else g_ssaaScale = 2;
            Wh_FreeStringSetting(ssaaStr);
        }
    } else {
        g_ssaaScale = 1;
    }
    g_enableSmoothGradient = Wh_GetIntSetting(L"enable_smooth_gradient") != 0;
    g_enableTrailShadow = Wh_GetIntSetting(L"enable_trail_shadow") != 0;
    g_particleDensity = Wh_GetIntSetting(L"particle_density");
    g_particleInterval = Wh_GetIntSetting(L"particle_interval");
    g_particleAccel = Wh_GetIntSetting(L"particle_acceleration") != 0;
    g_particleOriginRatio = Wh_GetIntSetting(L"particle_origin_ratio");
    int attrVal = Wh_GetIntSetting(L"particle_attraction");
    if (attrVal < 0)
        attrVal = 0;
    if (attrVal > 100)
        attrVal = 100;
    // 非线性映射：低区间精细区分，高区间压缩。value=1→0.0008（用户舒适值），value=5→0.0065，value=40+→上限0.08
    g_particleAttraction = attrVal > 0 ? fminf(0.0008f * powf((float)attrVal, 1.3f), 0.08f) : 0.0f;
    g_enableParticleRepel = Wh_GetIntSetting(L"enable_particle_repel") != 0;
    g_particleRepelRadius = Wh_GetIntSetting(L"particle_repel_radius");
    if (g_particleRepelRadius < 5)
        g_particleRepelRadius = 5;
    if (g_particleRepelRadius > 100)
        g_particleRepelRadius = 100;
    int repelVal = Wh_GetIntSetting(L"particle_repel_force");
    if (repelVal < 0)
        repelVal = 0;
    if (repelVal > 100)
        repelVal = 100;
    g_particleRepelForce = (repelVal / 100.0f) * 3.0f;  // 0-100 → 0-3.0 像素/帧 / 0-100 → 0-3.0 px/frame
    // 向心力漩涡设置 / Centripetal vortex settings
    g_enableCentripetal = Wh_GetIntSetting(L"enable_centripetal") != 0;
    int cfVal = Wh_GetIntSetting(L"centripetal_force");
    cfVal = ClampInt(cfVal, 0, 100);
    g_centripetalForce = cfVal / 100.0f;
    int csVal = Wh_GetIntSetting(L"centripetal_sensitivity");
    csVal = ClampInt(csVal, 0, 100);
    g_centripetalSensitivity = csVal / 100.0f;
    int cdVal = Wh_GetIntSetting(L"centripetal_duration");
    if (cdVal < 500) cdVal = 500; if (cdVal > 5000) cdVal = 5000;
    g_centripetalDuration = cdVal;
    int vmcVal = Wh_GetIntSetting(L"vortex_max_count");
    vmcVal = ClampInt(vmcVal, 1, MAX_VORTICES);
    g_vortexMaxCount = vmcVal;
    int vmdVal = Wh_GetIntSetting(L"vortex_min_distance");
    vmdVal = ClampInt(vmdVal, 50, 500);
    g_vortexMinDistance = (float)vmdVal;
    int vdVal = Wh_GetIntSetting(L"vortex_drift");
    vdVal = ClampInt(vdVal, 0, 100);
    g_vortexDrift = vdVal / 100.0f;
    int vdsVal = Wh_GetIntSetting(L"vortex_duration_speed");
    vdsVal = ClampInt(vdsVal, 0, 100);
    g_vortexDurationSpeed = vdsVal / 100.0f;
    g_vortexPhysModel = 0;
    {
        PCWSTR modelStr = Wh_GetStringSetting(L"vortex_phys_model");
        if (modelStr) {
            if (wcscmp(modelStr, L"rankine") == 0) g_vortexPhysModel = 0;
            else if (wcscmp(modelStr, L"free") == 0) g_vortexPhysModel = 1;
            else if (wcscmp(modelStr, L"solid") == 0) g_vortexPhysModel = 2;
            else if (wcscmp(modelStr, L"lamb") == 0) g_vortexPhysModel = 3;
            else if (wcscmp(modelStr, L"kepler") == 0) g_vortexPhysModel = 4;
            Wh_FreeStringSetting(modelStr);
        }
    }
    // 粒子质量系统设置 / Particle mass system settings
    g_enableParticleMass = Wh_GetIntSetting(L"enable_particle_mass") != 0;
    g_particleMassMin = (float)Wh_GetIntSetting(L"particle_mass_min") / 10.0f;
    if (g_particleMassMin < 0.1f) g_particleMassMin = 0.1f;
    g_particleMassMax = (float)Wh_GetIntSetting(L"particle_mass_max") / 10.0f;
    if (g_particleMassMax < 0.1f) g_particleMassMax = 0.1f;
    if (g_particleMassMax < g_particleMassMin) g_particleMassMax = g_particleMassMin;
    // 粒子万有引力设置 / Particle gravity settings
    g_enableParticleGravity = Wh_GetIntSetting(L"enable_particle_gravity") != 0;
    int gsVal = Wh_GetIntSetting(L"gravity_strength");
    gsVal = ClampInt(gsVal, 0, 100);
    g_gravityStrength = gsVal / 100.0f;
    PCWSTR gstr = Wh_GetStringSetting(L"gravity_system");
    if (gstr) {
        if (wcscmp(gstr, L"nbody") == 0) g_gravitySystem = 1;
        else g_gravitySystem = 0;
        Wh_FreeStringSetting(gstr);
    }
    g_gravityBodyCount = Wh_GetIntSetting(L"gravity_body_count");
    if (g_gravityBodyCount < 2) g_gravityBodyCount = 2;
    if (g_gravityBodyCount > 10) g_gravityBodyCount = 10;
    // 音乐响应设置 / Music reactive settings
    g_enableMusicReactive = Wh_GetIntSetting(L"enable_music_reactive") != 0;
    PCWSTR fftStr = Wh_GetStringSetting(L"music_fft_size");
    if (fftStr) {
        if (wcscmp(fftStr, L"fft256") == 0) g_musicFftSize = 256;
        else if (wcscmp(fftStr, L"fft512") == 0) g_musicFftSize = 512;
        else if (wcscmp(fftStr, L"fft1024") == 0) g_musicFftSize = 1024;
        else if (wcscmp(fftStr, L"fft2048") == 0) g_musicFftSize = 2048;
        else g_musicFftSize = 512;
        Wh_FreeStringSetting(fftStr);
    } else {
        g_musicFftSize = 512;
    }
    int msVal = Wh_GetIntSetting(L"music_sensitivity");
    msVal = ClampInt(msVal, 0, 100);
    g_musicSensitivity = msVal / 100.0f;
    int msmVal = Wh_GetIntSetting(L"music_smoothing");
    msmVal = ClampInt(msmVal, 0, 100);
    g_musicSmoothing = msmVal / 100.0f;
    // 音乐物理联动设置 / Music-physics linkage settings
    g_enableMusicPhysics = Wh_GetIntSetting(L"enable_music_physics") != 0;
    int mglVal = Wh_GetIntSetting(L"music_gravity_link");
    mglVal = ClampInt(mglVal, 0, 100);
    g_musicGravityLink = mglVal / 100.0f;
    int mbpVal = Wh_GetIntSetting(L"music_beat_pulse");
    mbpVal = ClampInt(mbpVal, 0, 100);
    g_musicBeatPulse = mbpVal / 100.0f;
    int mbsVal = Wh_GetIntSetting(L"music_bass_size");
    mbsVal = ClampInt(mbsVal, 0, 100);
    g_musicBassSize = mbsVal / 100.0f;
    int mvlVal = Wh_GetIntSetting(L"music_vortex_link");
    mvlVal = ClampInt(mvlVal, 0, 100);
    g_musicVortexLink = mvlVal / 100.0f;
    // 高级物理系统设置 / Advanced physics system settings
    g_enableParticleCollision = Wh_GetIntSetting(L"enable_particle_collision") != 0;
    g_enableLorentzForce = Wh_GetIntSetting(L"enable_lorentz_force") != 0;
    int lsVal = Wh_GetIntSetting(L"lorentz_strength");
    lsVal = ClampInt(lsVal, 0, 100);
    g_lorentzStrength = lsVal / 100.0f;
    g_enableBrownianMotion = Wh_GetIntSetting(L"enable_brownian_motion") != 0;
    int bsVal = Wh_GetIntSetting(L"brownian_strength");
    bsVal = ClampInt(bsVal, 0, 100);
    g_brownianStrength = bsVal / 100.0f;
    g_enableMultiBandLink = Wh_GetIntSetting(L"enable_multi_band_link") != 0;
    // 环境力场设置 / Environmental forces settings
    g_enableEnvGravity = Wh_GetIntSetting(L"enable_env_gravity") != 0;
    int egsVal = Wh_GetIntSetting(L"env_gravity_strength");
    if (egsVal < 0) egsVal = 0; if (egsVal > 100) egsVal = 100;
    g_envGravityStrength = egsVal / 100.0f * 0.8f;  // 0-100 -> 0-0.8 px/frame^2
    int egaVal = Wh_GetIntSetting(L"env_gravity_angle");
    if (egaVal < -180) egaVal = -180; if (egaVal > 180) egaVal = 180;
    g_envGravityAngle = egaVal * 3.14159f / 180.0f;  // deg -> rad
    g_enableEnvWind = Wh_GetIntSetting(L"enable_env_wind") != 0;
    int wsVal = Wh_GetIntSetting(L"wind_strength");
    if (wsVal < 0) wsVal = 0; if (wsVal > 100) wsVal = 100;
    g_windStrength = wsVal / 100.0f * 0.6f;
    int wtVal = Wh_GetIntSetting(L"wind_turbulence");
    if (wtVal < 0) wtVal = 0; if (wtVal > 100) wtVal = 100;
    g_windTurbulence = wtVal / 100.0f;
    // 布料弹簧设置 / Cloth spring settings
    g_enableParticleSpring = Wh_GetIntSetting(L"enable_particle_spring") != 0;
    int skVal = Wh_GetIntSetting(L"spring_strength");
    if (skVal < 0) skVal = 0; if (skVal > 100) skVal = 100;
    g_springK = skVal / 100.0f * 0.05f;  // 0-100 -> 0-0.05 (damping handles stability)
    int sdVal = Wh_GetIntSetting(L"spring_distance");
    if (sdVal < 10) sdVal = 10; if (sdVal > 150) sdVal = 150;
    g_springMaxDist = (float)sdVal;
    // 文字粒子形状设置 / Text particle shape settings
    PCWSTR pText = Wh_GetStringSetting(L"text_content");
    if (pText) { wcsncpy_s(g_textContent, pText, _TRUNCATE); Wh_FreeStringSetting(pText); }
    int tfsVal = Wh_GetIntSetting(L"text_font_size");
    if (tfsVal < 10) tfsVal = 10; if (tfsVal > 200) tfsVal = 200;
    g_textFontSize = tfsVal;
    // 长句分段轮播设置 / Long-text chunk cycling settings
    g_enableTextChunk = Wh_GetIntSetting(L"enable_text_chunk") != 0;
    int tcmVal = 3;
    PCWSTR pChunkMode = Wh_GetStringSetting(L"text_chunk_mode");
    if (pChunkMode) {
        if (wcscmp(pChunkMode, L"global") == 0) tcmVal = 1; else tcmVal = 0;
        Wh_FreeStringSetting(pChunkMode);
    }
    g_textChunkMode = tcmVal;
    int tcmMax = Wh_GetIntSetting(L"text_chunk_max");
    if (tcmMax < 1) tcmMax = 1; if (tcmMax > 8) tcmMax = 8;
    g_textChunkMax = tcmMax;
    int tcdVal = Wh_GetIntSetting(L"text_chunk_delay");
    if (tcdVal < 100) tcdVal = 100; if (tcdVal > 3000) tcdVal = 3000;
    g_textChunkDelay = tcdVal;
    int toxVal = Wh_GetIntSetting(L"text_offset_x");
    if (toxVal < -100) toxVal = -100; if (toxVal > 100) toxVal = 100;
    g_textOffsetX = toxVal;
    int toyVal = Wh_GetIntSetting(L"text_offset_y");
    if (toyVal < -100) toyVal = -100; if (toyVal > 100) toyVal = 100;
    g_textOffsetY = toyVal;
    g_charAtlasDirty = true;  // 文字设置变更，标记图集需重建 / text settings changed, mark atlas dirty
    // 物理可视化调试（独立开关）/ Physics visualization debug (independent toggles)
    g_debugVelocity = Wh_GetIntSetting(L"enable_debug_velocity") != 0;
    g_debugForce = Wh_GetIntSetting(L"enable_debug_force") != 0;
    g_debugVortex = Wh_GetIntSetting(L"enable_debug_vortex") != 0;
    g_debugGravity = Wh_GetIntSetting(L"enable_debug_gravity") != 0;
    g_debugCollision = Wh_GetIntSetting(L"enable_debug_collision") != 0;
    g_debugSpring = Wh_GetIntSetting(L"enable_debug_spring") != 0;
    // 音乐捕获：音乐响应或音乐物理联动任一开启即需要捕获音频，全部关闭才停止 / Music capture: start audio if either reactive or physics link is on, stop only when both off
    bool musicWanted = g_enableMusicReactive || g_enableMusicPhysics;
    if (musicWanted && !g_music.capturing) {
        MusicStartCapture();
    } else if (!musicWanted && g_music.capturing) {
        MusicStopCapture();
    }
    PCWSTR pstr = Wh_GetStringSetting(L"particle_mode");
    if (pstr) {
        if (wcscmp(pstr, L"off") == 0)
            g_particleMode = 0;
        else if (wcscmp(pstr, L"always") == 0)
            g_particleMode = 2;
        else
            g_particleMode = 1;
        Wh_FreeStringSetting(pstr);
    }
    PCWSTR pshape = Wh_GetStringSetting(L"particle_shape");
    if (pshape) {
        // 形状编码与HLSL shapeSDF一致：0=circle,1=star,2=hexagram,3=heart,4=diamond,5=triangle,6=flower,7=pentagon,8=hexagon,9=random
        // Shape encoding matches HLSL shapeSDF: 0=circle,1=star,2=hexagram,3=heart,4=diamond,5=triangle,6=flower,7=pentagon,8=hexagon,9=random
        if (wcscmp(pshape, L"circle") == 0)
            g_particleShape = 0;
        else if (wcscmp(pshape, L"star") == 0)
            g_particleShape = 1;
        else if (wcscmp(pshape, L"hexagram") == 0)
            g_particleShape = 2;
        else if (wcscmp(pshape, L"heart") == 0)
            g_particleShape = 3;
        else if (wcscmp(pshape, L"diamond") == 0)
            g_particleShape = 4;
        else if (wcscmp(pshape, L"triangle") == 0)
            g_particleShape = 5;
        else if (wcscmp(pshape, L"flower") == 0)
            g_particleShape = 6;
        else if (wcscmp(pshape, L"pentagon") == 0)
            g_particleShape = 7;
        else if (wcscmp(pshape, L"hexagon") == 0)
            g_particleShape = 8;
        else if (wcscmp(pshape, L"text") == 0)
            g_particleShape = 10;  // text / 文字字符
        else
            g_particleShape = 9;  // random / 随机混合
        Wh_FreeStringSetting(pshape);
        Wh_Log(L"[CharAtlas] particle_shape parsed, g_particleShape=%d", g_particleShape);
    }
    g_enableParticleSpin = Wh_GetIntSetting(L"enable_particle_spin") != 0;
    g_particleSpinSpeed = Wh_GetIntSetting(L"particle_spin_speed");
    if (g_particleSpinSpeed < 0) g_particleSpinSpeed = 0;
    if (g_particleSpinSpeed > 100) g_particleSpinSpeed = 100;
    g_enableParticleInteraction = Wh_GetIntSetting(L"enable_particle_interaction") != 0;
    g_interParticleRepelDistance = Wh_GetIntSetting(L"particle_repel_distance");
    if (g_interParticleRepelDistance < 5) g_interParticleRepelDistance = 5;
    if (g_interParticleRepelDistance > 100) g_interParticleRepelDistance = 100;
    int interRepelVal = Wh_GetIntSetting(L"particle_inter_repel_force");
    if (interRepelVal < 1) interRepelVal = 1;
    if (interRepelVal > 50) interRepelVal = 50;
    g_interParticleRepelForce = interRepelVal / 100.0f;
    g_enableClickStarburst = Wh_GetIntSetting(L"enable_click_starburst") != 0;
    g_starburstCount = Wh_GetIntSetting(L"starburst_count");
    g_enableClickEffect = Wh_GetIntSetting(L"enable_click_effect") != 0;
    g_clickMaxRadius = Wh_GetIntSetting(L"click_max_radius");
    g_clickDuration = Wh_GetIntSetting(L"click_duration");

    PCWSTR str = Wh_GetStringSetting(L"trail_shape");
    if (str) {
        if (wcscmp(str, L"dots") == 0)
            g_trailShape = 1;
        else if (wcscmp(str, L"function") == 0)
            g_trailShape = 2;
        else if (wcscmp(str, L"wave") == 0)
            g_trailShape = 3;
        else if (wcscmp(str, L"shapes") == 0)
            g_trailShape = 4;
        else if (wcscmp(str, L"double") == 0)
            g_trailShape = 5;
        else if (wcscmp(str, L"dashed") == 0)
            g_trailShape = 6;
        else if (wcscmp(str, L"spiral") == 0)
            g_trailShape = 7;
        else if (wcscmp(str, L"lightning") == 0)
            g_trailShape = 8;
        else if (wcscmp(str, L"feather") == 0)
            g_trailShape = 9;
        else if (wcscmp(str, L"none") == 0)
            g_trailShape = 10;
        else
            g_trailShape = 0;
        Wh_FreeStringSetting(str);
    }
    // 形状拖尾设置 / Shape trail settings
    str = Wh_GetStringSetting(L"shape_type");
    if (str) {
        if (wcscmp(str, L"star") == 0)
            g_shapeType = 1;
        else if (wcscmp(str, L"hexagon") == 0)
            g_shapeType = 2;
        else if (wcscmp(str, L"circle") == 0)
            g_shapeType = 3;
        else if (wcscmp(str, L"diamond") == 0)
            g_shapeType = 4;
        else if (wcscmp(str, L"triangle") == 0)
            g_shapeType = 5;
        else if (wcscmp(str, L"flower") == 0)
            g_shapeType = 6;
        else if (wcscmp(str, L"pentagon") == 0)
            g_shapeType = 7;
        else if (wcscmp(str, L"hexagram") == 0)
            g_shapeType = 8;
        else if (wcscmp(str, L"random") == 0)
            g_shapeType = 9;
        else
            g_shapeType = 0;  // heart
        Wh_FreeStringSetting(str);
    }
    g_shapeInterval = Wh_GetIntSetting(L"shape_interval");
    if (g_shapeInterval < 10) g_shapeInterval = 10;
    if (g_shapeInterval > 100) g_shapeInterval = 100;
    g_shapeRandomOffset = Wh_GetIntSetting(L"shape_random_offset") != 0;
    g_shapeCount = Wh_GetIntSetting(L"shape_count");
    if (g_shapeCount < 1) g_shapeCount = 1;
    if (g_shapeCount > 5) g_shapeCount = 5;
    g_shapeSize = (float)Wh_GetIntSetting(L"shape_size");
    if (g_shapeSize < 5) g_shapeSize = 5;
    if (g_shapeSize > 30) g_shapeSize = 30;
    g_shapeLifetime = Wh_GetIntSetting(L"shape_lifetime");
    if (g_shapeLifetime < 200) g_shapeLifetime = 200;
    if (g_shapeLifetime > 2000) g_shapeLifetime = 2000;
    g_superPerformanceMode = Wh_GetIntSetting(L"super_performance_mode") != 0;
    g_enableBezierSmooth = Wh_GetIntSetting(L"enable_bezier_smooth") != 0;
    g_enableMotionBlur = Wh_GetIntSetting(L"enable_motion_blur") != 0;
    g_motionBlurStrength = Wh_GetIntSetting(L"motion_blur_strength");
    if (g_motionBlurStrength < 1) g_motionBlurStrength = 1;
    if (g_motionBlurStrength > 5) g_motionBlurStrength = 5;
    g_enable25DEffect = Wh_GetIntSetting(L"enable_25d_effect") != 0;
    g_perspectiveStrength = Wh_GetIntSetting(L"perspective_strength");
    if (g_perspectiveStrength < 0) g_perspectiveStrength = 0;
    if (g_perspectiveStrength > 50) g_perspectiveStrength = 50;
    str = Wh_GetStringSetting(L"function_preset");
    if (str) {
        if (wcscmp(str, L"damped") == 0)
            g_functionPreset = 1;
        else if (wcscmp(str, L"beat") == 0)
            g_functionPreset = 2;
        else if (wcscmp(str, L"swirl") == 0)
            g_functionPreset = 3;
        else if (wcscmp(str, L"custom") == 0)
            g_functionPreset = 4;
        else
            g_functionPreset = 0;
        Wh_FreeStringSetting(str);
    }
    str = Wh_GetStringSetting(L"custom_function");
    if (str) {
        WStrToUTF8(str, g_customFunction, 256);
        Wh_FreeStringSetting(str);
    }
    str = Wh_GetStringSetting(L"color_mode");
    if (str) {
        if (wcscmp(str, L"single") == 0)
            g_colorMode = 0;
        else if (wcscmp(str, L"gradient") == 0)
            g_colorMode = 1;
        else if (wcscmp(str, L"rainbow") == 0)
            g_colorMode = 2;
        else if (wcscmp(str, L"warm") == 0)
            g_colorMode = 3;
        else if (wcscmp(str, L"cool") == 0)
            g_colorMode = 4;
        else if (wcscmp(str, L"neon") == 0)
            g_colorMode = 5;
        else if (wcscmp(str, L"velocity") == 0)
            g_colorMode = 6;
        else if (wcscmp(str, L"stripes") == 0)
            g_colorMode = 7;
        else if (wcscmp(str, L"fire") == 0)
            g_colorMode = 8;
        else if (wcscmp(str, L"aurora") == 0)
            g_colorMode = 9;
        else if (wcscmp(str, L"cursor_extract") == 0)
            g_colorMode = 10;
        else if (wcscmp(str, L"cursor_mix") == 0)
            g_colorMode = 11;
        else if (wcscmp(str, L"metallic") == 0)
            g_colorMode = 12;
        else if (wcscmp(str, L"cyberpunk") == 0)
            g_colorMode = 13;
        else if (wcscmp(str, L"pastel") == 0)
            g_colorMode = 14;
        else if (wcscmp(str, L"hue_rotate") == 0)
            g_colorMode = 15;
        else if (wcscmp(str, L"dual_pulse") == 0)
            g_colorMode = 16;
        else if (wcscmp(str, L"sparkle") == 0)
            g_colorMode = 17;
        else if (wcscmp(str, L"thermal") == 0)
            g_colorMode = 18;
        else if (wcscmp(str, L"interference") == 0)
            g_colorMode = 19;
        else if (wcscmp(str, L"spectrum") == 0)
            g_colorMode = 20;
        else if (wcscmp(str, L"dither") == 0)
            g_colorMode = 21;
        else if (wcscmp(str, L"warp") == 0)
            g_colorMode = 22;
        else
            g_colorMode = 0;
        Wh_FreeStringSetting(str);
    }
    str = Wh_GetStringSetting(L"particle_origin");
    if (str) {
        if (wcscmp(str, L"head") == 0)
            g_particleOrigin = 0;
        else if (wcscmp(str, L"middle") == 0)
            g_particleOrigin = 1;
        else if (wcscmp(str, L"custom") == 0)
            g_particleOrigin = 3;
        else if (wcscmp(str, L"random") == 0)
            g_particleOrigin = 4;
        else
            g_particleOrigin = 2;  // tail
        Wh_FreeStringSetting(str);
    }
    g_cursorColorShift = Wh_GetIntSetting(L"enable_cursor_color_shift") != 0;
    str = Wh_GetStringSetting(L"color_shift_mode");
    if (str) {
        if (wcscmp(str, L"off") == 0) g_colorShiftMode = 0;
        else if (wcscmp(str, L"complementary") == 0) g_colorShiftMode = 1;
        else if (wcscmp(str, L"analogous") == 0) g_colorShiftMode = 2;
        else if (wcscmp(str, L"triadic") == 0) g_colorShiftMode = 3;
        else if (wcscmp(str, L"split") == 0) g_colorShiftMode = 4;
        else if (wcscmp(str, L"custom") == 0) g_colorShiftMode = 5;
        else g_colorShiftMode = 1;
        Wh_FreeStringSetting(str);
    }
    g_colorShiftAngle = Wh_GetIntSetting(L"color_shift_angle");
    if (g_colorShiftAngle < 0) g_colorShiftAngle = 0;
    if (g_colorShiftAngle > 360) g_colorShiftAngle = 360;
    str = Wh_GetStringSetting(L"custom_color");
    if (str) {
        g_customColor = ParseHexColor(str, D2D1::ColorF(0, .75f, 1));
        Wh_FreeStringSetting(str);
    }
    str = Wh_GetStringSetting(L"gradient_colors");
    if (str) {
        ParseGradientColors(str);
        Wh_FreeStringSetting(str);
    }

    if (g_triggerVelocity <= 0)
        g_triggerVelocity = 25;
    if (g_stopVelocity <= 0)
        g_stopVelocity = 10;
    // 迟滞区间要求停止阈值低于触发阈值，否则激活后会立即停止；钳制为触发阈值的80% / Hysteresis: stop must be below trigger, else immediate stop; clamp to 80% of trigger
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
    g_dotChainSize = Wh_GetIntSetting(L"dot_chain_size");
    if (g_dotChainSize < 50) g_dotChainSize = 50;
    if (g_dotChainSize > 300) g_dotChainSize = 300;
    g_particleSizeMultiplier = Wh_GetIntSetting(L"particle_size_multiplier");
    g_enableParticleGlow = Wh_GetIntSetting(L"enable_particle_glow") != 0;
    g_particleGlowIntensity = Wh_GetIntSetting(L"particle_glow_intensity");
    if (g_particleSizeMultiplier < 50) g_particleSizeMultiplier = 50;
    if (g_particleSizeMultiplier > 300) g_particleSizeMultiplier = 300;
    if (g_particleGlowIntensity < 0) g_particleGlowIntensity = 0;
    if (g_particleGlowIntensity > 100) g_particleGlowIntensity = 100;
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
    if (g_edgeSoftness < 0) g_edgeSoftness = 0;
    if (g_edgeSoftness > 100) g_edgeSoftness = 100;
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

    // 根据颜色模式动态设置屏幕捕获排除（仅光标取色模式需要排除自己）/ Dynamically set capture exclusion based on color mode (only cursor-extract needs self-exclusion)
    if (g_overlayHwnd) {
        if (g_colorMode == 10 || g_colorMode == 11)  // cursor_extract or cursor_mix
            SetWindowDisplayAffinity(g_overlayHwnd, WDA_EXCLUDEFROMCAPTURE);
        else
            SetWindowDisplayAffinity(g_overlayHwnd, WDA_NONE);
    }
}

// ===================== 游戏检测 / Game Detection =====================
// 检测前台窗口是否为全屏应用（游戏/视频等）/ Detect if foreground window is fullscreen (game/video)
// 实现方式：窗口尺寸匹配 + 光标裁剪 + 光标隐藏 + D3D 全屏状态 / Methods: window size match + cursor clip + cursor hide + D3D fullscreen state
bool CheckForegroundFullscreen() {
    HWND fg = GetForegroundWindow();
    if (!fg || fg == GetDesktopWindow())
        return false;

    // 缓存桌面窗口句柄，每 500ms 刷新（Explorer 重启后失效）/ Cache desktop window handles, refresh every 500ms (invalid after Explorer restart)
    static DWORD s_cacheTime = 0;
    static HWND s_progman = nullptr, s_workerw = nullptr;
    DWORD tick = GetTickCount();
    if (tick - s_cacheTime > 500 || !s_progman) {
        s_progman = FindWindowW(L"Progman", NULL);
        s_workerw = FindWindowW(L"WorkerW", NULL);
        s_cacheTime = tick;
    }
    if (fg == s_progman || fg == s_workerw)
        return false;

    // 1. 系统 D3D 全屏状态检测 / 1. System D3D fullscreen state detection
    QUERY_USER_NOTIFICATION_STATE quns;
    if (SUCCEEDED(SHQueryUserNotificationState(&quns)) && quns == QUNS_RUNNING_D3D_FULL_SCREEN)
        return true;

    // 2. 窗口尺寸与显示器匹配 / 2. Window size matches monitor
    RECT rcWnd;
    GetWindowRect(fg, &rcWnd);
    HMONITOR hMon = MonitorFromWindow(fg, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi = {sizeof(mi)};
    if (!GetMonitorInfo(hMon, &mi))
        return false;
    bool sizeMatch = rcWnd.left <= mi.rcMonitor.left && rcWnd.top <= mi.rcMonitor.top &&
                     rcWnd.right >= mi.rcMonitor.right && rcWnd.bottom >= mi.rcMonitor.bottom;
    if (!sizeMatch)
        return false;

    // 3. 光标裁剪（游戏通常会限制光标在窗口内）/ 3. Cursor clip (games usually confine cursor to window)
    RECT rcClip;
    if (GetClipCursor(&rcClip)) {
        int vW = GetSystemMetrics(SM_CXVIRTUALSCREEN);
        int vH = GetSystemMetrics(SM_CYVIRTUALSCREEN);
        if ((rcClip.right - rcClip.left) < vW || (rcClip.bottom - rcClip.top) < vH)
            return true;
    }

    // 4. 光标隐藏（全屏游戏通常隐藏光标）/ 4. Cursor hidden (fullscreen games usually hide cursor)
    CURSORINFO ci = {sizeof(CURSORINFO)};
    if (GetCursorInfo(&ci) && ci.flags == 0)
        return true;

    return false;
}

// 取色互补色偏移：色相+180°，增强饱和度和亮度，确保拖尾在任何背景上醒目 / Complementary color shift: hue+180°, boost saturation and brightness for visibility on any background
static D2D1_COLOR_F ShiftToComplementary(D2D1_COLOR_F c) {
    float mx = fmaxf(fmaxf(c.r, c.g), c.b);
    float mn = fminf(fminf(c.r, c.g), c.b);
    float h, s, v = mx;
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
    if (h < 0.0f)
        h += 360.0f;

    // 根据偏移模式计算偏移角度 / Compute shift angle based on shift mode
    float shiftAngle = 0.0f;
    switch (g_colorShiftMode) {
        case 1: shiftAngle = 180.0f; break;  // 互补色
        case 2: shiftAngle = 30.0f; break;   // 类似色
        case 3: shiftAngle = 120.0f; break;  // 三角色
        case 4: shiftAngle = 150.0f; break;  // 分裂互补
        case 5: shiftAngle = (float)g_colorShiftAngle; break;  // 自定义
        default: shiftAngle = 0.0f; break;   // 关闭
    }
    h = fmodf(h + shiftAngle, 360.0f);
    s = fmaxf(s, g_colorShiftSatBoost);  // 饱和度保底
    v = fmaxf(v, g_colorShiftValBoost);  // 亮度保底
    return HSVtoRGB(h, s, v);
}

static void ExtractCursorColor(POINT pt, DWORD dwTime) {
    // 100ms 一次：GetDC(NULL) 在 DWM 下触发全屏回读，过于频繁会与渲染线程冲突掉帧
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
            g_cursorExtractedColor = LerpColor(g_cursorExtractedColor, newColor, 0.22f);
        }
    }
}

// ===================== 自适应对比度：背景亮度采样 / Adaptive Contrast: Background Luminance Sampling =====================
// 沿拖尾路径、在路径法线两侧偏移采样屏幕像素（避开拖尾自身），计算平均亮度（0=暗，1=亮）/ Sample screen pixels offset along path normals (avoid self), compute avg luminance (0=dark, 1=bright)
// 限制采样频率避免性能开销，采样结果用于自适应明度 / Throttle sampling to avoid perf cost, result used for adaptive brightness
static void SampleBackgroundLuminance(const std::vector<D2D1_POINT_2F>& path, DWORD dwTime, int vX, int vY) {
    if (!g_adaptiveContrast || path.size() < 2) return;
    // 500ms 一次：GetDC(NULL) 在 DWM 下会触发全屏 GPU→CPU 回读，过于频繁会与渲染线程的 DwmFlush 互斥阻塞导致掉帧 / Every 500ms: GetDC(NULL) triggers full-screen GPU→CPU readback under DWM, too frequent causes mutex contention with render thread DwmFlush
    if (dwTime - g_lastBgSample < 500) return;
    g_lastBgSample = dwTime;

    const float OFFSET = 10.0f;  // 沿法线偏移像素，需大于拖尾最大宽度以避开自身 / Normal offset in px, must exceed max trail width to avoid self
    // 收集采样点：沿路径均匀 3 处，每处取法线两侧各 1 点（共最多 6 点）/ Collect sample points: 3 evenly along path, 1 point each side of normal (max 6 total)
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
        float nx = -dy / ln, ny = dx / ln;  // 单位法线 / Unit normal
        float bx = path[i].x + vX, by = path[i].y + vY;
        pts[nPts++] = {(int)(bx + nx * OFFSET), (int)(by + ny * OFFSET)};
        pts[nPts++] = {(int)(bx - nx * OFFSET), (int)(by - ny * OFFSET)};
    }
    if (nPts == 0) return;

    // 计算采样点包围盒 / Compute sample point bounding box
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
    // 限制 DIB 尺寸，避免极端情况下分配过大 / Limit DIB size to avoid excessive allocation in edge cases
    if (w > 128) w = 128;
    if (h > 128) h = 128;

    HDC hdcScreen = GetDC(NULL);
    if (!hdcScreen) return;

    // 创建 top-down 32bpp DIB（biHeight 为负表示自上而下，避免坐标翻转）/ Create top-down 32bpp DIB (negative biHeight = top-down, avoids coordinate flip)
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
    HGDIOBJ oldBmp = SelectObject(hdcMem, hBmp);

    // 一次 BitBlt 批量回读包围盒区域，替代多次 GetPixel（每次 GetPixel 都会单独触发 DWM 往返）/ Single BitBlt batch readback of bbox, replaces multiple GetPixel (each triggers separate DWM roundtrip)
    BitBlt(hdcMem, 0, 0, w, h, hdcScreen, minX, minY, SRCCOPY);

    float totalLum = 0.0f;
    int samples = 0;
    const BYTE* pSrc = (const BYTE*)pBits;
    for (int i = 0; i < nPts; ++i) {
        int dx = pts[i].x - minX, dy = pts[i].y - minY;
        if (dx < 0 || dx >= w || dy < 0 || dy >= h) continue;  // 超出 DIB 范围（被尺寸限制裁剪）则跳过 / Out of DIB range (clipped by size limit), skip
        const BYTE* px = pSrc + (dy * w + dx) * 4;  // 32bpp BGRA，top-down / 32bpp BGRA, top-down
        // 感知亮度：0.299R + 0.587G + 0.114B / Perceptual luminance: 0.299R + 0.587G + 0.114B
        float lum = (0.299f * px[2] + 0.587f * px[1] + 0.114f * px[0]) / 255.0f;
        totalLum += lum;
        samples++;
    }

    SelectObject(hdcMem, oldBmp);
    DeleteObject(hBmp);
    DeleteDC(hdcMem);
    ReleaseDC(NULL, hdcScreen);

    if (samples > 0) {
        float avgLum = totalLum / samples;
        // 平滑过渡，避免亮度跳变 / Smooth transition to avoid luminance jumps
        g_bgLuminance = g_bgLuminance * 0.7f + avgLum * 0.3f;
    }
}

// 后台采样线程：把 GDI 回读（GetDC/BitBlt）从渲染线程剥离，避免阻塞 vsync / Background sampler thread: offload GDI readback (GetDC/BitBlt) from render thread to avoid vsync blocking
// 渲染线程只负责写入最新输入，本线程定期读取并执行慢操作，结果写回全局变量供渲染线程读取 / Render thread writes latest input, this thread periodically reads and runs slow ops, writes results back to globals for render thread
static DWORD WINAPI BgSamplerThreadProc(LPVOID) {
    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_BELOW_NORMAL);
    while (WaitForSingleObject(g_bgSampleExitEvent, 50) != WAIT_OBJECT_0) {
        // 复制输入（锁内，vector 复制很快）
        std::vector<D2D1_POINT_2F> path;
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

        // 慢操作全部在锁外执行，不阻塞渲染线程 / Slow ops all executed outside lock, don't block render thread
        if (adaptive) {
            SampleBackgroundLuminance(path, dwTime, vX, vY);
        }
        if (colorMode == 10 || colorMode == 11) {
            ExtractCursorColor(cursor, dwTime);
        }
    }
    return 0;
}

// ===================== 轨迹变形 / Trail Deformation =====================
// 螺旋变形：沿路径法线方向应用螺旋偏移 / Spiral deformation: apply spiral offset along path normal
// 计算路径上某点的法线单位向量 / Compute normal unit vector at path point
static inline void GetPathNormal(const std::vector<D2D1_POINT_2F> &pts, size_t i, float &nx, float &ny) {
    float dx, dy;
    if (i == 0) { dx = pts[1].x - pts[0].x; dy = pts[1].y - pts[0].y; }
    else if (i == pts.size() - 1) { dx = pts[i].x - pts[i-1].x; dy = pts[i].y - pts[i-1].y; }
    else { dx = pts[i+1].x - pts[i-1].x; dy = pts[i+1].y - pts[i-1].y; }
    float len = sqrtf(dx*dx + dy*dy);
    if (len > 0.001f) { nx = -dy / len; ny = dx / len; }
    else { nx = 0; ny = 1; }
}

// 螺旋变形：沿路径法线方向应用正弦螺旋偏移，尾部衰减 / Spiral deformation: apply sinusoidal spiral offset along path normal, tail decay
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

// 闪电变形：沿路径应用随机锯齿，使用统一随机数生成器 / Lightning deformation: apply random jitter along path using uniform RNG
static void ApplyLightningDeformation(std::vector<D2D1_POINT_2F> &pts, DWORD dwTime) {
    if (pts.size() < 3) return;
    std::vector<D2D1_POINT_2F> result;
    result.reserve(pts.size() * 2);
    for (size_t i = 0; i < pts.size() - 1; ++i) {
        result.push_back(pts[i]);
        // 在每两点之间插入一个随机偏移的中点 / Insert a randomly offset midpoint between each pair
        float mx = (pts[i].x + pts[i+1].x) * 0.5f;
        float my = (pts[i].y + pts[i+1].y) * 0.5f;
        float dx = pts[i+1].x - pts[i].x, dy = pts[i+1].y - pts[i].y;
        float len = sqrtf(dx*dx + dy*dy);
        if (len > 0.001f) {
            float nx = -dy / len, ny = dx / len;
            float offset = (Rand01() - 0.5f) * 16.0f;  // 统一随机数，范围-8~8 / Uniform random, range -8~8
            result.push_back({mx + nx * offset, my + ny * offset});
        }
    }
    result.push_back(pts.back());
    pts = result;
}

// 羽毛变形：主轴轻微弯曲，模拟羽毛弧度 / Feather deformation: slight curve on shaft
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
        // 钳制偏移范围，避免表达式异常导致飞点 / Clamp offset range to prevent flyaway points from expression anomalies
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

// ===================== 交换链/目标位图重建 / Swap Chain/Target Bitmap Reconstruction =====================
// 检测当前是否HDR模式（通过尝试创建HDR格式交换链来自动探测）/ Detect HDR mode (auto-probe by attempting HDR format swap chain)
static bool DetectHDRMode() {
    // 旧版dxgi.h不支持IDXGIOutput6，改用注册表检测HDR状态 / Old dxgi.h lacks IDXGIOutput6, use registry to detect HDR state
    HKEY hKey = nullptr;
    bool hdr = false;
    if (RegOpenKeyExW(HKEY_LOCAL_MACHINE,
        L"SYSTEM\\CurrentControlSet\\Control\\GraphicsDrivers",
        0, KEY_READ, &hKey) == ERROR_SUCCESS) {
        DWORD value = 0, size = sizeof(DWORD);
        if (RegQueryValueExW(hKey, L"ATIDXGISupport", nullptr, nullptr, (LPBYTE)&value, &size) == ERROR_SUCCESS) {
            // ATI显卡HDR支持标志 / ATI GPU HDR support flag
        }
        // 检查Windows HDR高级颜色设置 / Check Windows HDR advanced color settings
        HKEY hSubKey = nullptr;
        if (RegOpenKeyExW(hKey, L"Configuration", 0, KEY_READ, &hSubKey) == ERROR_SUCCESS) {
            // 遍历所有显示配置，检查HDR启用状态 / Iterate all display configs, check HDR enabled state
            DWORD index = 0;
            wchar_t subName[256];
            DWORD subNameLen = 256;
            while (RegEnumKeyExW(hSubKey, index++, subName, &subNameLen, nullptr, nullptr, nullptr, nullptr) == ERROR_SUCCESS) {
                HKEY hMonitor = nullptr;
                if (RegOpenKeyExW(hSubKey, subName, 0, KEY_READ, &hMonitor) == ERROR_SUCCESS) {
                    DWORD advancedColor = 0;
                    DWORD advSize = sizeof(DWORD);
                    // 0=SDR, 1=HDR
                    if (RegQueryValueExW(hMonitor, L"AdvancedColorEnabled", nullptr, nullptr,
                                         (LPBYTE)&advancedColor, &advSize) == ERROR_SUCCESS) {
                        if (advancedColor == 1) hdr = true;
                    }
                    RegCloseKey(hMonitor);
                }
                subNameLen = 256;
            }
            RegCloseKey(hSubKey);
        }
        RegCloseKey(hKey);
    }
    return hdr;
}

static void RecreateSwapChain(int vW, int vH) {
    // 尺寸合法性保护：显示器切换/分辨率变化瞬间可能取到0或负值，避免创建无效交换链 / Size validity guard: monitor switch/resolution change may yield 0 or negative, avoid invalid swap chain
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

    // 检测HDR模式，选择合适的交换链格式
    g_isHDRMode = DetectHDRMode();
    if (g_isHDRMode) {
        // HDR模式使用R16G16B16A16_FLOAT（scRGB色彩空间）
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
    if (FAILED(hr) || !g_pSwapChain) {
        // HDR格式创建失败，回退到SDR格式
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

    // HDR模式下使用浮点格式，色彩空间由DWM自动处理
    // (旧版dxgi.h不支持SetColorSpace1，依赖DWM自动识别格式)

    IDXGISurface *pSurface = nullptr;
    if (FAILED(g_pSwapChain->GetBuffer(0, __uuidof(IDXGISurface), (void **)&pSurface)))
        return;

    D2D1_BITMAP_PROPERTIES1 bmpProps = {};
    bmpProps.pixelFormat = D2D1::PixelFormat(g_swapChainFormat, D2D1_ALPHA_MODE_PREMULTIPLIED);
    bmpProps.dpiX = 96.0f;
    bmpProps.dpiY = 96.0f;
    bmpProps.bitmapOptions = D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW;

    hr = g_pD2DDC->CreateBitmapFromDxgiSurface(pSurface, &bmpProps, &g_pD2DTargetBitmap);
    pSurface->Release();
    if (FAILED(hr)) {
        Wh_Log(L"RecreateSwapChain: CreateBitmapFromDxgiSurface failed: 0x%08X", hr);
        return;
    }
    Wh_Log(L"RecreateSwapChain: swap chain ready (%dx%d), HDR=%d, format=%d", vW, vH, g_isHDRMode, (int)g_swapChainFormat);

    // 从后台缓冲 bitmap 取底层 texture，创建缓存的渲染目标视图
    // （RTV 自持底层资源引用，临时 surface/texture 用完即可释放）
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

    // MSAA + SSAA 离屏渲染纹理创建 / MSAA + SSAA offscreen render target creation
    if (g_pMSAATexture) { g_pMSAATexture->Release(); g_pMSAATexture = nullptr; }
    if (g_pMSAARTV) { g_pMSAARTV->Release(); g_pMSAARTV = nullptr; }
    if (g_pSSAATexture) { g_pSSAATexture->Release(); g_pSSAATexture = nullptr; }
    if (g_pSSAARTV) { g_pSSAARTV->Release(); g_pSSAARTV = nullptr; }
    if (g_pSSAASRV) { g_pSSAASRV->Release(); g_pSSAASRV = nullptr; }
    if (g_pSSAAResolveTexture) { g_pSSAAResolveTexture->Release(); g_pSSAAResolveTexture = nullptr; }
    if (g_pSSAAResolveSRV) { g_pSSAAResolveSRV->Release(); g_pSSAAResolveSRV = nullptr; }

    int renderW = vW, renderH = vH;
    bool useSSAA = (g_ssaaScale > 1);
    bool useMSAA = (g_msaaSamples > 1);

    if (useSSAA) {
        renderW = vW * g_ssaaScale;
        renderH = vH * g_ssaaScale;
        // 钳制到 D3D11 纹理上限 / Clamp to D3D11 texture limit
        const UINT maxDim = D3D11_REQ_TEXTURE2D_U_OR_V_DIMENSION;
        if ((UINT)renderW > maxDim || (UINT)renderH > maxDim) {
            Wh_Log(L"SSAA: %dx exceeds texture limit, disabling SSAA", g_ssaaScale);
            useSSAA = false;
            g_ssaaScale = 1;
            renderW = vW; renderH = vH;
        }
    }

    // 创建离屏渲染纹理（可能带 MSAA）/ Create offscreen RT (possibly with MSAA)
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
        // MSAA 离屏纹理 / MSAA offscreen texture
        if (SUCCEEDED(g_pD3DDevice->CreateTexture2D(&rtDesc, nullptr, &g_pMSAATexture))) {
            g_pD3DDevice->CreateRenderTargetView(g_pMSAATexture, nullptr, &g_pMSAARTV);
        }
        // MSAA resolve 目标（非 MSAA 同尺寸纹理）/ MSAA resolve target (non-MSAA same size)
        rtDesc.SampleDesc.Count = 1;
        rtDesc.SampleDesc.Quality = 0;
        rtDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
        if (SUCCEEDED(g_pD3DDevice->CreateTexture2D(&rtDesc, nullptr, &g_pSSAAResolveTexture))) {
            g_pD3DDevice->CreateShaderResourceView(g_pSSAAResolveTexture, nullptr, &g_pSSAAResolveSRV);
        }
        if (g_pMSAARTV && g_pSSAAResolveSRV) {
            g_pCachedRTV->Release(); g_pCachedRTV = g_pMSAARTV;
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
        // 纯 SSAA 离屏纹理（带 SRV 供 blit）/ Pure SSAA offscreen (with SRV for blit)
        rtDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
        if (SUCCEEDED(g_pD3DDevice->CreateTexture2D(&rtDesc, nullptr, &g_pSSAATexture))) {
            g_pD3DDevice->CreateRenderTargetView(g_pSSAATexture, nullptr, &g_pSSAARTV);
            g_pD3DDevice->CreateShaderResourceView(g_pSSAATexture, nullptr, &g_pSSAASRV);
            if (g_pSSAARTV && g_pSSAASRV) {
                g_pCachedRTV->Release(); g_pCachedRTV = g_pSSAARTV;
                g_pSSAARTV->AddRef();
                Wh_Log(L"SSAA: %dx enabled (%dx%d)", g_ssaaScale, renderW, renderH);
            }
        }
        if (!g_pSSAARTV || !g_pSSAASRV) {
            Wh_Log(L"SSAA: offscreen creation failed, disabling SSAA");
            useSSAA = false;
            g_ssaaScale = 1;
            if (g_pSSAATexture) { g_pSSAATexture->Release(); g_pSSAATexture = nullptr; }
            if (g_pSSAARTV) { g_pSSAARTV->Release(); g_pSSAARTV = nullptr; }
            if (g_pSSAASRV) { g_pSSAASRV->Release(); g_pSSAASRV = nullptr; }
        }
    }
    // 记录离屏资源实际使用的 AA 配置，供渲染循环检测运行时切换 / Record effective AA config for runtime change detection
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
}

// ===================== Mesh tessellation 辅助函数 =====================
// ID2D1Mesh 只写一次（第二次 Open 会失败），所以每帧创建新 mesh，旧的在更新时释放
static void CreateMeshFromQuadStrip(ID2D1DeviceContext *dc, ID2D1Mesh **mesh,
                                    const std::vector<D2D1_POINT_2F> &left,
                                    const std::vector<D2D1_POINT_2F> &right) {
    if (*mesh) {
        (*mesh)->Release();
        *mesh = nullptr;
    }
    if (!dc || left.size() < 2 || right.size() < 2)
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
    size_t n = (std::min)(left.size(), right.size());
    std::vector<D2D1_TRIANGLE> tris;
    tris.reserve((n - 1) * 2);
    for (size_t i = 0; i < n - 1; i++) {
        tris.push_back({left[i], right[i], left[i + 1]});
        tris.push_back({right[i], right[i + 1], left[i + 1]});
    }
    pSink->AddTriangles(tris.data(), (UINT32)tris.size());
    pSink->Close();
    pSink->Release();
}

// 四边形带 + 头部椭圆帽，合并创建一个 mesh
static void CreateMeshFromQuadStripWithCap(ID2D1DeviceContext *dc, ID2D1Mesh **mesh,
                                           const std::vector<D2D1_POINT_2F> &left,
                                           const std::vector<D2D1_POINT_2F> &right,
                                           D2D1_POINT_2F capCenter, float capRadius) {
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
    std::vector<D2D1_TRIANGLE> tris;
    // 四边形带 / Quad strip
    if (left.size() >= 2 && right.size() >= 2) {
        size_t n = (std::min)(left.size(), right.size());
        tris.reserve((n - 1) * 2 + 24);
        for (size_t i = 0; i < n - 1; i++) {
            tris.push_back({left[i], right[i], left[i + 1]});
            tris.push_back({right[i], right[i + 1], left[i + 1]});
        }
    }
    // 头部椭圆（三角形扇，24 段）/ Head cap ellipse (triangle fan, 24 segments)
    if (capRadius > 0.1f) {
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

// ===================== 渲染子函数 / Render Sub-functions =====================

// 预渲染字符到 bitmap（比每帧 DrawText 快很多）/ Pre-render chars to bitmaps (much faster than per-frame DrawText)
static void BuildCharBitmaps() {
    if (!g_pD2DDC || !g_pTextFormat) return;
    if (g_cachedFontSize == g_textFontSize && !g_charBitmaps.empty()) return;

    for (auto &kv : g_charBitmaps) { if (kv.second) kv.second->Release(); }
    g_charBitmaps.clear();

    float fs = (float)g_textFontSize;
    UINT bmpW = (UINT)(fs + 4), bmpH = (UINT)(fs + 4);

    wchar_t seen[256]; int seenCount = 0;
    for (int i = 0; g_textContent[i] != 0 && i < 255; i++) {
        wchar_t ch = g_textContent[i];
        bool found = false;
        for (int j = 0; j < seenCount; j++) if (seen[j] == ch) { found = true; break; }
        if (found) continue;
        seen[seenCount++] = ch;

        ID2D1Bitmap1 *bmp = nullptr;
        D2D1_BITMAP_PROPERTIES1 bmpProps = D2D1::BitmapProperties1(
            D2D1_BITMAP_OPTIONS_TARGET,
            D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED));
        g_pD2DDC->CreateBitmap(D2D1::SizeU(bmpW, bmpH), nullptr, 0, &bmpProps, (ID2D1Bitmap1**)&bmp);

        ID2D1Image *oldTarget = nullptr;
        g_pD2DDC->GetTarget(&oldTarget);
        g_pD2DDC->SetTarget(bmp);
        g_pD2DDC->BeginDraw();
        g_pD2DDC->Clear(D2D1::ColorF(0, 0, 0, 0));
        wchar_t chStr[2] = { ch, 0 };
        D2D1_RECT_F rc = { 0, 0, (float)bmpW, (float)bmpH };
        g_pSolidOuterBrush->SetColor(D2D1::ColorF(1, 1, 1, 1));
        g_pSolidOuterBrush->SetOpacity(1.0f);
        g_pD2DDC->DrawText(chStr, 1, g_pTextFormat, &rc, g_pSolidOuterBrush);
        g_pD2DDC->EndDraw();
        g_pD2DDC->SetTarget(oldTarget);
        if (oldTarget) oldTarget->Release();

        g_charBitmaps[ch] = bmp;  // AddRef 已在 CreateBitmap / already ref'd by CreateBitmap
    }
    g_cachedFontSize = g_textFontSize;
}
// 粒子渲染（主体含发光模拟 + 多形状 + 颜色渐变）/ Particle rendering (body with glow simulation + multi-shape + color gradient)
static void RenderParticles(DWORD dwTime) {
    if (g_particles.empty()) return;
    bool particleFastPath = (g_particleShape != 10) && (g_particles.size() > (g_superPerformanceMode ? 300 : 100));
    float sizeMul = g_particleSizeMultiplier / 100.0f;  // 与原生渲染一致 / consistent with native render
    float musicSizeBoost = g_enableMusicPhysics ? (1.0f + g_musicBassSizeBoost * 0.5f) : 1.0f;
    for (auto &p : g_particles) {
        float progress = (float)(dwTime - p.startTime) / p.lifetime;
        if (progress < 0 || progress >= 1) continue;
        float lifeAlpha = (1.0f - progress);
        // 颜色二次衰减，与原生渲染一致 / Quadratic color fade, consistent with native render
        float colorFade = progress * progress;
        // 色差 + 漩涡亮度，与原生渲染一致 / Color variation + vortex brightness, consistent with native render
        float rOff = p.colorOffset[0] + p.vortexBrightness[0];
        float gOff = p.colorOffset[1] + p.vortexBrightness[1];
        float bOff = p.colorOffset[2] + p.vortexBrightness[2];
        D2D1_COLOR_F pc = D2D1::ColorF(p.color.r + (p.endColor.r - p.color.r) * colorFade + rOff,
                                       p.color.g + (p.endColor.g - p.color.g) * colorFade + gOff,
                                       p.color.b + (p.endColor.b - p.color.b) * colorFade + bOff, 1.0f);
        g_pSolidOuterBrush->SetColor(pc);
        g_pSolidOuterBrush->SetOpacity(lifeAlpha * 0.65f);
        // 生命周期大小曲线 + 粒子大小倍数 + 低频增益，与原生渲染baseSize公式一致 / Lifecycle size curve + size multiplier + bass boost, matches native baseSize
        float sizeScale = sinf(progress * 3.14159f) * 0.7f + 0.3f;
        float bodySize = p.size * 2.0f * sizeMul * sizeScale * musicSizeBoost;
        // D2D1回退路径仅支持circle(0)/star(1)/hexagram(2)，其他形状降级为圆形 / D2D1 fallback only supports circle(0)/star(1)/hexagram(2), others degrade to circle
        // 星形/六角形应用粒子自旋角度，与原生渲染rotation一致 / Star/hexagram apply particle spin rotation, consistent with native render
        if (!particleFastPath && p.shapeType == 1 && g_pStarGeom) {
            D2D1_MATRIX_3X2_F oldT;
            g_pD2DDC->GetTransform(&oldT);
            g_pD2DDC->SetTransform(D2D1::Matrix3x2F::Rotation(p.rotation * 57.2958f) *
                                   D2D1::Matrix3x2F::Scale(bodySize, bodySize) *
                                   D2D1::Matrix3x2F::Translation(p.x, p.y));
            g_pD2DDC->FillGeometry(g_pStarGeom, g_pSolidOuterBrush);
            g_pD2DDC->SetTransform(oldT);
        } else if (!particleFastPath && p.shapeType == 2 && g_pHexagramGeom) {
            D2D1_MATRIX_3X2_F oldT;
            g_pD2DDC->GetTransform(&oldT);
            g_pD2DDC->SetTransform(D2D1::Matrix3x2F::Rotation(p.rotation * 57.2958f) *
                                   D2D1::Matrix3x2F::Scale(bodySize, bodySize) *
                                   D2D1::Matrix3x2F::Translation(p.x, p.y));
            g_pD2DDC->FillGeometry(g_pHexagramGeom, g_pSolidOuterBrush);
            g_pD2DDC->SetTransform(oldT);
        } else if (p.shapeType == 10 && p.textChar != 0) {
            // 文字形状：用预渲染 bitmap 画字符，支持旋转 / Text shape: draw pre-rendered bitmap, supports rotation
            auto it = g_charBitmaps.find(p.textChar);
            if (it != g_charBitmaps.end() && it->second) {
                ID2D1Bitmap *bmp = it->second;
                D2D1_SIZE_F bmpSize = bmp->GetSize();
                float halfW = bmpSize.width * 0.5f;
                float halfH = bmpSize.height * 0.5f;
                float scale = bodySize / halfW;
                if (scale < 0.1f) scale = 0.1f;
                D2D1_MATRIX_3X2_F oldT;
                g_pD2DDC->GetTransform(&oldT);
                g_pD2DDC->SetTransform(D2D1::Matrix3x2F::Rotation(p.rotation * 57.2958f) *
                                       D2D1::Matrix3x2F::Scale(scale, scale) *
                                       D2D1::Matrix3x2F::Translation(p.x, p.y));
                D2D1_RECT_F rc = { -halfW, -halfH, halfW, halfH };
                g_pD2DDC->DrawBitmap(bmp, &rc, lifeAlpha * 0.65f, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, nullptr);
                g_pD2DDC->SetTransform(oldT);
            }
        } else {
            g_pD2DDC->FillEllipse(D2D1::Ellipse(D2D1::Point2F(p.x, p.y), bodySize, bodySize), g_pSolidOuterBrush);
        }
    }
    g_pSolidOuterBrush->SetOpacity(1.0f);
}

// 形状拖尾渲染（爱心/五角星/六角形/圆形，带出生动画）/ Shape trail rendering (heart/star/hexagram/circle, with birth animation)
static void RenderTrailShapes(DWORD dwTime) {
    if (g_trailShapes.empty()) return;
    for (auto &s : g_trailShapes) {
        float progress = (float)(dwTime - s.startTime) / s.lifetime;
        if (progress < 0 || progress >= 1) continue;
        // 生命周期动画：前20%放大（0→1），后30%缩小（1→0.3），中间保持 / Lifecycle animation: first 20% scale up (0→1), last 30% scale down (1→0.3), middle hold
        float scale;
        if (progress < 0.2f) {
            scale = progress * 5.0f;  // 出生放大 / Birth scale up
        } else if (progress > 0.7f) {
            scale = 1.0f - (progress - 0.7f) * 2.33f;  // 死亡缩小 / Death scale down
            scale = fmaxf(scale, 0.1f);
        } else {
            scale = 1.0f;
        }
        scale *= s.size;
        // 透明度：前10%淡入，后30%淡出 / Opacity: first 10% fade in, last 30% fade out
        float lifeAlpha;
        if (progress < 0.1f) {
            lifeAlpha = progress * 10.0f;
        } else if (progress > 0.7f) {
            lifeAlpha = (1.0f - progress) / 0.3f;
        } else {
            lifeAlpha = 1.0f;
        }
        lifeAlpha *= 0.85f;
        g_pSolidOuterBrush->SetColor(s.color);
        g_pSolidOuterBrush->SetOpacity(lifeAlpha);
        ID2D1PathGeometry *geom = nullptr;
        if (s.shapeType == 0) geom = g_pHeartGeom;
        else if (s.shapeType == 1) geom = g_pStarGeom;
        else if (s.shapeType == 2) geom = g_pHexagramGeom;
        // 变换：旋转 + 缩放 + 平移（形状中心在原点）/ Transform: rotation + scale + translation (shape center at origin)
        D2D1_MATRIX_3X2_F transform =
            D2D1::Matrix3x2F::Rotation(s.rotation * 57.2958f) *  // 弧度转角度 / Rad to deg
            D2D1::Matrix3x2F::Scale(scale, scale) *
            D2D1::Matrix3x2F::Translation(s.x, s.y);
        if (geom) {
            D2D1_MATRIX_3X2_F oldT;
            g_pD2DDC->GetTransform(&oldT);
            g_pD2DDC->SetTransform(transform);
            g_pD2DDC->FillGeometry(geom, g_pSolidOuterBrush);
            g_pD2DDC->SetTransform(oldT);
        } else {
            // 圆形用Ellipse，也应用旋转（圆形旋转无视觉变化，但保持一致）/ Circle uses Ellipse, also apply rotation (no visual change for circle, but consistent)
            D2D1_MATRIX_3X2_F oldT;
            g_pD2DDC->GetTransform(&oldT);
            g_pD2DDC->SetTransform(transform);
            g_pD2DDC->FillEllipse(D2D1::Ellipse(D2D1::Point2F(0, 0), 1.0f, 1.0f), g_pSolidOuterBrush);
            g_pD2DDC->SetTransform(oldT);
        }
    }
    g_pSolidOuterBrush->SetOpacity(1.0f);
}

// 运动模糊历史帧渲染（仅锥形带，简化外带，透明度递减）/ Motion blur history frame rendering (cone strip only, simplified outer band, decreasing alpha)
static void RenderMotionBlur(float widthMul, const GradData &cols, float fadeAlpha) {
    if (!g_enableMotionBlur || g_trailShape != 0 || g_trailHistory.size() <= 1) return;
    g_pD2DDC->SetAntialiasMode(D2D1_ANTIALIAS_MODE_ALIASED);
    for (size_t h = 0; h < g_trailHistory.size() - 1; h++) {
        auto &histPath = g_trailHistory[h].path;
        if (histPath.size() < 2) continue;
        float histAlpha = 0.22f * (1.0f - (float)h / g_trailHistory.size()) * fadeAlpha;
        std::vector<D2D1_POINT_2F> hlo, hro;
        for (size_t i = 0; i < histPath.size(); i++) {
            float ddx, ddy;
            if (i == 0) { ddx = histPath[0].x - histPath[1].x; ddy = histPath[0].y - histPath[1].y; }
            else if (i == histPath.size() - 1) { ddx = histPath[i-1].x - histPath[i].x; ddy = histPath[i-1].y - histPath[i].y; }
            else { ddx = histPath[i-1].x - histPath[i+1].x; ddy = histPath[i-1].y - histPath[i+1].y; }
            float ln = sqrtf(ddx*ddx + ddy*ddy);
            if (ln > 0) { ddx /= ln; ddy /= ln; } else { ddx = 1; ddy = 0; }
            float nx = -ddy, ny = ddx;
            float ratio = (float)i / (histPath.size() - 1);
            float ow = 10.0f * powf(1.0f - ratio, 1.3f) * widthMul;
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

// 点击波纹渲染（外圈填充+描边，内圈描边）/ Click ripple rendering (outer ring fill+stroke, inner ring stroke)
static void RenderClickRipples(DWORD dwTime, const GradData &cols, int vX, int vY) {
    if (!g_enableClickEffect || g_ripples.empty()) return;
    for (auto &ripple : g_ripples) {
        float elapsed = (float)(dwTime - ripple.startTime), progress = elapsed / g_clickDuration;
        if (progress < 0 || progress >= 1) continue;
        float radius = progress * g_clickMaxRadius, alpha = (1 - progress) * .7f;
        D2D1_POINT_2F c = D2D1::Point2F((float)(ripple.pos.x - vX), (float)(ripple.pos.y - vY));
        g_pSolidOuterBrush->SetColor(cols.solidOuter);
        g_pSolidOuterBrush->SetOpacity(alpha * 0.12f);
        g_pD2DDC->FillEllipse(D2D1::Ellipse(c, radius, radius), g_pSolidOuterBrush);
        g_pSolidOuterBrush->SetOpacity(alpha);
        g_pD2DDC->DrawEllipse(D2D1::Ellipse(c, radius, radius), g_pSolidOuterBrush, 2.5f);
        if (radius > 4) {
            g_pSolidInnerBrush->SetColor(cols.solidInner);
            g_pSolidInnerBrush->SetOpacity(alpha * .8f);
            g_pD2DDC->DrawEllipse(D2D1::Ellipse(c, radius * .7f, radius * .7f), g_pSolidInnerBrush, 1.5f);
        }
    }
    g_pSolidOuterBrush->SetOpacity(1);
    g_pSolidInnerBrush->SetOpacity(1);
}

// ===================== 主绘制循环 / Main Render Loop =====================
// 简易Perlin噪声：空间连贯湍流场 / Simple Perlin-like coherent turbulence
static float HashNoise(int x, int y, int seed) {
    int n = x * 374761393 + y * 668265263 + seed * 2246822519;
    n = (n ^ (n >> 13)) * 1274126177;
    return ((n & 1023) / 1023.0f) * 2.0f - 1.0f;
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

static void RenderFrame() {
    // 设置变更时在渲染线程中重载（避免 UI 线程与渲染线程竞争全局变量）/ Reload settings on render thread when changed (avoid UI/render thread race on globals)
    if (g_settingsDirty.exchange(false)) {
        LoadSettings();
    }
    DWORD dwTime = GetTickCount();
    POINT pt;
    GetCursorPos(&pt);
    int dx = pt.x - g_lastPos.x, dy = pt.y - g_lastPos.y;
    float velocity = sqrtf((float)(dx * dx + dy * dy));
    g_lastPos = pt;

    // 光标取色和背景采样已移至后台线程（BgSamplerThreadProc），避免 GDI 回读阻塞渲染线程 / Cursor color extraction and bg sampling moved to background thread (BgSamplerThreadProc), avoid GDI readback blocking render thread
    // 这里只把最新输入写入共享结构，后台线程定期读取执行 / Here only write latest input to shared struct, background thread reads periodically

    POINT renderPos = pt;
    if (g_trailDelay > 0) {
        if (!g_lagInited) {
            g_lagPos = pt;
            g_lagInited = true;
        }
        float ease = 1.0f - (g_trailDelay / 14.0f);
        if (ease < 0.12f)
            ease = 0.12f;
        g_lagPos.x += (int)((pt.x - g_lagPos.x) * ease);
        g_lagPos.y += (int)((pt.y - g_lagPos.y) * ease);
        renderPos = g_lagPos;
    } else
        g_lagInited = false;

    GradData cols;
    ComputeColors(g_colorMode, dwTime, velocity, cols);

    float widthMul = 1.0f;
    if (g_enableSpeedResponse) {
        float sf = fminf(velocity / 80.0f, 1.0f);
        float accel = fabsf(velocity - g_prevVelocity);
        float af = fminf(accel / 30.0f, 1.0f);
        widthMul = 1.0f + sf * 0.3f + af * 0.15f;
    }

    int vX = g_virtX, vY = g_virtY;

    static DWORD lastFsCheck = 0;
    static bool isGameCached = false, trailActive = false;
    static int idleFrameCount = 0, fadeoutFrame = 0;
    static bool surfaceDirty = false;
    static bool gameHidden = false;
    static bool isWindowVisible = false;  // 窗口初始隐藏，首次有内容绘制时才显示
    static int hideDelayCounter = 0;
    if (dwTime - lastFsCheck > 500) {
        isGameCached = CheckForegroundFullscreen();
        lastFsCheck = dwTime;
    }

    // 点击检测（游戏中跳过，避免全屏游戏内生成不必要的效果）/ Click detection (skip in game to avoid unnecessary effects in fullscreen)
    bool lDown = (GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0;
    bool rDown = (GetAsyncKeyState(VK_RBUTTON) & 0x8000) != 0;
    if (!isGameCached) {
        if (g_enableClickEffect) {
            if (lDown && !g_prevLButton) {
                g_ripples.push_back({pt, dwTime});
                g_ripples.push_back({pt, dwTime + 120});  // 延迟副波纹，双环效果 / Delayed secondary ripple, double-ring effect
            }
            if (rDown && !g_prevRButton) {
                g_ripples.push_back({pt, dwTime});
                g_ripples.push_back({pt, dwTime + 120});
            }
        }
        if (g_enableClickStarburst) {
            if ((lDown && !g_prevLButton) || (rDown && !g_prevRButton)) {
                SpawnParticles((float)(pt.x - vX), (float)(pt.y - vY), g_starburstCount, 2.5f, 5.5f, 2.5f, 5.0f, 250, 450,
                               cols.solidOuter, dwTime, true);
            }
        }
    }
    g_prevLButton = lDown;
    g_prevRButton = rDown;

    if (!g_ripples.empty())
        g_ripples.erase(std::remove_if(g_ripples.begin(), g_ripples.end(),
                                       [&](const Ripple &r) { return dwTime - r.startTime > (DWORD)g_clickDuration; }),
                        g_ripples.end());

    int vW = g_virtW, vH = g_virtH;

    if (isGameCached) {
        // 游戏中隐藏覆盖层窗口，避免全屏顶层窗口阻挡游戏的独立翻转/MPO / Hide overlay in game to avoid topmost window blocking independent flip/MPO
        if (!gameHidden) {
            PostMessageW(g_overlayHwnd, WM_APP_HIDE_OVERLAY, 0, 0);
            gameHidden = true;
            isWindowVisible = false;  // 同步窗口可见状态，避免退出游戏后窗口不显示 / Sync window visibility, avoid window not showing after exiting game
        }
        if (trailActive || !g_history.empty() || !g_ripples.empty() || !g_particles.empty() || surfaceDirty) {
            trailActive = false;
            g_history.clear();
            g_ripples.clear();
            g_particles.clear();
            g_fadeAlpha = 1.0f;
            fadeoutFrame = 0;
        } else
            return;
    } else {
        gameHidden = false;  // 退出游戏后重置，允许窗口重新显示 / Reset after exiting game, allow window to show again
        // 拖尾激活状态机：基于速度阈值 + 低速持续时间 + 实际移动距离 / Trail activation state machine: velocity threshold + low-speed duration + actual movement distance
        static float accumulatedDist = 0.0f;
        static float lastVel = 0.0f;
        static int triggerConfirm = 0;  // 触发确认帧数 / Trigger confirmation frame count
        static float lastTriggerPosX = 0, lastTriggerPosY = 0;
        float accel = velocity - lastVel;
        lastVel = velocity;

        if (!trailActive) {
            // 未激活：速度超过阈值 或 加速度突增 时触发 / Inactive: trigger when velocity exceeds threshold or acceleration spikes
            bool speedOK = (velocity > g_triggerVelocity);
            bool accelOK = (accel > 8.0f && velocity > g_triggerVelocity * 0.6f);  // 加速度触发更严格 / Stricter accel trigger
            if (speedOK || accelOK) {
                triggerConfirm++;
                if (triggerConfirm >= 2) {  // 连续2帧确认，避免单帧抖动误触 / Require 2 consecutive frames to avoid jitter false-trigger
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
            // 已激活：速度低于停止阈值时计数 / Active: count frames when velocity below stop threshold
            if (velocity < g_stopVelocity) {
                idleFrameCount++;
                // 计算实际移动距离（而非速度累积）/ Track actual movement distance (not velocity accumulation)
                float movedDist = sqrtf((renderPos.x - lastTriggerPosX) * (renderPos.x - lastTriggerPosX) +
                                        (renderPos.y - lastTriggerPosY) * (renderPos.y - lastTriggerPosY));
                lastTriggerPosX = renderPos.x;
                lastTriggerPosY = renderPos.y;
                accumulatedDist += movedDist;
                // 低速超过4帧 或 低速2帧且几乎没移动 时停止 / Stop after 4 low-speed frames, or 2 frames with negligible movement
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
            // 移动时 alpha 快速恢复到 1.0 / Alpha quickly recovers to 1.0 during movement
            g_fadeAlpha += (1.0f - g_fadeAlpha) * 0.4f;
            if (g_fadeAlpha > 1.0f)
                g_fadeAlpha = 1.0f;
        } else {
            // ===== 淡出模式：硬截断 / 加速收缩 / 软截断 / Fadeout modes: hard cut / accelerated shrink / soft cut =====
            switch (g_fadeoutMode) {
                case 0:  // 硬截断：立即清除所有状态，避免下次绘制残留 / Hard cut: clear all state immediately to prevent residual rendering
                    g_history.clear();
                    g_particles.clear();
                    g_trailShapes.clear();
                    g_trailHistory.clear();
                    g_hasLastParticlePos = false;
                    g_fadeAlpha = 1.0f;
                    fadeoutFrame = 0;
                    break;
                case 1: {  // 加速收缩：前慢后快，每帧 pop 数量平缓递增
                    fadeoutFrame++;
                    int popCount = 1 + fadeoutFrame / 5;  // 帧1-5:1, 6-10:2, 11-15:3...
                    if (popCount > 4)
                        popCount = 4;  // 上限 4，避免后期飞太快
                    for (int i = 0; i < popCount && !g_history.empty(); i++)
                        g_history.pop_back();
                    if (g_history.size() <= 1)
                        g_history.clear();
                    g_fadeAlpha = 1.0f;
                    break;
                }
                case 2:  // 软截断：基于剩余长度的淡出曲线 + 拖尾收缩同步，杜绝末端硬切
                    fadeoutFrame++;
                    if (!g_history.empty())
                        g_history.pop_back();
                    // alpha 由剩余点数比例决定：点数越少越透明，pow 曲线让头部保持饱满、尾部加速消失
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

    // ===== 提前计算 smoothed 路径（粒子释放和渲染共用）=====
    std::vector<D2D1_POINT_2F> smoothed;
    smoothed.reserve(g_tailLength * 4);  // 预分配，避免多次扩容
    bool havePath = (g_history.size() >= 2);
    if (havePath) {
        for (auto &p : g_history)
            smoothed.push_back(D2D1::Point2F((float)p.x + g_tailOffsetX, (float)p.y + g_tailOffsetY));
        if (g_enableBezierSmooth) {
            // Catmull-Rom 样条平滑（更顺滑的曲线）
            std::vector<D2D1_POINT_2F> bezierOut;
            bezierOut.reserve(smoothed.size() * 4);
            CatmullRomSmooth(smoothed, bezierOut, 4);
            smoothed.swap(bezierOut);  // swap替代拷贝，O(1) / swap instead of copy, O(1)
        } else {
            // 原线性插值平滑 / Original linear interpolation smoothing
            std::vector<D2D1_POINT_2F> ns;
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
                smoothed.swap(ns);  // swap替代拷贝 / swap instead of copy
            }
        }
        if (g_trailShape == 2)
            ApplyFunctionDeformation(smoothed, dwTime);
        else if (g_trailShape == 3)
            ApplyWaveDeformation(smoothed, dwTime);
        else if (g_trailShape == 7)
            ApplySpiralDeformation(smoothed, dwTime);
        else if (g_trailShape == 8)
            ApplyLightningDeformation(smoothed, dwTime);
        else if (g_trailShape == 9)
            ApplyFeatherDeformation(smoothed, dwTime);
    }

    // ===== 把最新输入交给后台采样线程（GDI 回读在后台线程执行，不阻塞渲染）===== / Pass latest input to background sampler thread (GDI readback runs in background, doesn't block render)
    EnterCriticalSection(&g_sampleCS);
    g_samplePath = smoothed;
    g_sampleVX = vX; g_sampleVY = vY;
    g_sampleCursor = pt; g_sampleTime = dwTime;
    g_sampleHasPath = havePath;
    g_sampleColorMode = g_colorMode;
    g_sampleAdaptive = g_adaptiveContrast ? true : false;
    LeaveCriticalSection(&g_sampleCS);

    // ===== 运动模糊：保存当前路径到历史缓冲区（仅锥形带模式，避免切换形状后残留旧帧）===== / Motion blur: save current path to history buffer (cone strip only, avoid stale frames after shape switch)
    if (g_enableMotionBlur && havePath && g_trailShape == 0) {
        TrailFrame frame;
        frame.path = smoothed;
        frame.time = dwTime;
        g_trailHistory.push_back(frame);
        while ((int)g_trailHistory.size() > g_motionBlurStrength)
            g_trailHistory.erase(g_trailHistory.begin());
    } else if ((!g_enableMotionBlur || g_trailShape != 0) && !g_trailHistory.empty()) {
        g_trailHistory.clear();
    }


    // ===== 粒子释放（基于 smoothed 路径的指定位置）===== / Particle spawn (based on specified position on smoothed path)
    if (g_particleMode > 0 && havePath && dwTime - g_lastParticleTime >= (DWORD)g_particleInterval) {
        bool spawnOK = (g_particleMode == 1) ? trailActive : true;
        if (spawnOK) {
            float ratio;
            switch (g_particleOrigin) {
                case 0:
                    ratio = 0.0f;
                    break;  // head
                case 1:
                    ratio = 0.5f;
                    break;  // middle
                case 3:
                    ratio = g_particleOriginRatio / 100.0f;
                    break;  // custom
                case 4:
                    ratio = Rand01();
                    break;  // random
                default:
                    ratio = 1.0f;
                    break;  // tail
            }
            D2D1_POINT_2F origin = GetPointOnPath(smoothed, ratio);
            float speedMul = 1.0f;
            if (g_particleAccel) {
                float accel = velocity - g_prevVelocity;
                speedMul = 1.0f + fabsf(accel) * 0.07f;
                if (speedMul > 3.5f)
                    speedMul = 3.5f;
            }
            // 快速移动时插值补粒子，消除缝隙 / Interpolate particles during fast movement to eliminate gaps
            float avgParticleSize = 3.25f * (g_particleSizeMultiplier / 100.0f);
            float spawnStep = avgParticleSize * 2.0f;  // 每间隔2倍粒子大小插值一批 / Interpolate every 2x particle size
            int interpCount = g_particleDensity;  // 插值点粒子数和正常一致 / Same count as normal spawn
            if (interpCount < 1) interpCount = 1;
            // 仅间隔<=10ms时插值，避免曲线路径直线插值出现粒子连光标 / Only interpolate when interval<=10ms to avoid straight-line artifacts on curved paths
            bool canInterp = (g_particleInterval <= 10) && (g_particles.size() < (size_t)(GetParticleCap() * 0.8f));
            if (canInterp && g_hasLastParticlePos) {
                float dx = origin.x - g_lastParticleX;
                float dy = origin.y - g_lastParticleY;
                float dist = sqrtf(dx * dx + dy * dy);
                DWORD timeSinceLast = dwTime - g_lastParticleTime;
                // 时间间隔短（<800ms）说明在正常移动，即使距离大也插值；时间长说明停止过，重置 / Short interval (<800ms) means continuous movement; long interval means stopped, reset
                if (timeSinceLast > 800) {
                    g_hasLastParticlePos = false;
                } else if (dist > spawnStep) {
                    int steps = (int)(dist / spawnStep);
                    if (steps > 25) steps = 25;
                    for (int i = 1; i <= steps; i++) {
                        float t = (float)i / (steps + 1);  // 0=上一位置(远), 1=当前位置(近) / 0=prev pos(far), 1=current pos(near)
                        // 位置加随机偏移，避免粒子排成直线 / Add random jitter to avoid straight particle lines
                        float jitter = avgParticleSize * 0.8f;
                        float ix = g_lastParticleX + dx * t + (Rand01() - 0.5f) * jitter;
                        float iy = g_lastParticleY + dy * t + (Rand01() - 0.5f) * jitter;
                        // 渐变：远小近大、远少近多、远短近长 / Gradient: far=smaller/fewer/shorter, near=larger/more/longer
                        float sizeFactor = 0.5f + t * 0.5f;  // 远处0.5倍，近处1.0倍 / far=0.5x, near=1.0x
                        int countFactor = (int)(interpCount * (0.4f + t * 0.6f));  // 远处40%，近处100% / far=40%, near=100%
                        if (countFactor < 1) countFactor = 1;
                        int lifeFactor = (int)(200 + t * 300);  // 远处200ms，近处500ms / far=200ms, near=500ms
                        // 插值点粒子速度稍大，沿移动方向有拖尾感 / Interpolated particles slightly faster for trailing feel
                        float spdMul = 1.0f + (1.0f - t) * 0.5f;  // 远处速度更大 / far particles faster
                        SpawnParticles(ix, iy, countFactor, 0.4f * speedMul * spdMul, 2.5f * speedMul * spdMul,
                                       2.0f * sizeFactor, 4.5f * sizeFactor, lifeFactor,
                                       lifeFactor + 200, cols.solidOuter, dwTime);
                    }
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
    // ===== 形状拖尾生成（沿路径按间隔生成爱心/五角星等，带随机方向加速度）=====
    if (g_trailShape == 4 && havePath) {
        float curX = smoothed[0].x, curY = smoothed[0].y;
        if (!g_hasLastShapePos) {
            g_lastShapeX = curX;
            g_lastShapeY = curY;
            g_hasLastShapePos = true;
        }
        float dx = curX - g_lastShapeX, dy = curY - g_lastShapeY;
        float dist = sqrtf(dx * dx + dy * dy);
        if (dist >= (float)g_shapeInterval) {
            for (int i = 0; i < g_shapeCount; i++) {
                float sx = curX, sy = curY;
                if (g_shapeRandomOffset) {
                    sx += (Rand01() - 0.5f) * g_shapeSize * 1.5f;
                    sy += (Rand01() - 0.5f) * g_shapeSize * 1.5f;
                }
                int st = g_shapeType;
                if (st == 9) st = (int)(Rand01() * 9);  // random: 0-8 shapes / 随机：0-8种形状
                // 随机方向初速度（0.5-2.0 像素/帧）+ 随机旋转速度 / Random direction initial velocity (0.5-2.0 px/frame) + random spin speed
                float angle = Rand01() * 6.2831853f;
                float speed = 0.5f + Rand01() * 1.5f;
                float rotSpeed = (Rand01() - 0.5f) * 0.1f;  // 随机旋转速度 / Random spin speed
                g_trailShapes.push_back({sx, sy, cosf(angle) * speed, sinf(angle) * speed,
                                         g_shapeSize, 0.0f, rotSpeed, st, dwTime,
                                         (float)g_shapeLifetime, cols.solidOuter});
            }
            g_lastShapeX = curX;
            g_lastShapeY = curY;
        }
    } else {
        g_hasLastShapePos = false;
    }
    // 形状总数上限（超级性能模式下提高上限）/ Shape total cap (higher in super performance mode)
    int shapeCap = g_superPerformanceMode ? 400 : 150;
    if (g_trailShapes.size() > (size_t)shapeCap) {
        g_trailShapes.erase(g_trailShapes.begin(), g_trailShapes.begin() + (g_trailShapes.size() - shapeCap));
    }
    // 粒子总数上限，防止参数拉满时性能崩溃（超级性能模式下提高上限）/ Particle total cap, prevent perf collapse when params maxed (higher in super performance mode)
    int particleCap = GetParticleCap();
    if (g_particles.size() > (size_t)particleCap) {
        g_particles.erase(g_particles.begin(), g_particles.begin() + (g_particles.size() - particleCap));
    }
    g_prevVelocity = velocity;

    // ===== 音乐响应：FFT分析 + 节拍检测 + 物理联动状态更新 ===== / Music reactive: FFT analysis + beat detection + physics-link state update
    // 音乐响应或物理联动任一开启都需要运行分析（物理联动依赖节拍/频段数据）/ Run analysis if either reactive or physics link is on (physics link depends on beat/band data)
    if (g_enableMusicReactive || g_enableMusicPhysics) {
        MusicUpdate(dwTime);
    }

    // ===== 向心力漩涡：多漩涡系统 + 圆周运动检测 + 粒子轨道捕获 ===== / Centripetal vortex: multi-vortex system + circular motion detection + particle orbit capture
    // 力施加前保存速度快照到debugForce字段，供帧末计算总受力 F=mΔv（涵盖漩涡/排斥/引力/阻力/光标等全部力）/ Save velocity snapshot before any force, for total-force debug F=mΔv at frame end (covers vortex/repulsion/gravity/drag/cursor etc.)
    if (g_debugForce) {
        for (auto &p : g_particles) { p.debugForceX = p.vx; p.debugForceY = p.vy; }
    }
    if (g_enableCentripetal) {
        CircleDetect &cd = g_circleDetect;
        // 计算加速度（当前速度 - 上帧速度）/ Compute acceleration
        float accel = velocity - g_prevVelocity;
        float accelAbs = fabsf(accel);
        // 记录位置历史（环形缓冲），使用粒子坐标空间（与粒子p.x/p.y、音乐创建的漩涡中心一致）/ Record position history (ring buffer), in particle coordinate space (consistent with particles and music-created vortices)
        float histX = (float)(renderPos.x - vX) + (float)g_tailOffsetX;
        float histY = (float)(renderPos.y - vY) + (float)g_tailOffsetY;
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

        // 统计当前活跃漩涡数量 / Count currently active vortices
        int activeVortexCount = 0;
        for (int i = 0; i < g_vortexMaxCount; i++) {
            if (g_vortices[i].active) activeVortexCount++;
        }

        // 曲线运动检测：任何曲线运动都是圆周运动的一段，曲率中心即漩涡中心 / Curved motion detection: any curve is a segment of circular motion, curvature center = vortex center
        float accelBoost = fminf(accelAbs / 10.0f, 1.0f);
        float minVel = 3.0f - accelBoost * 2.0f;
        bool detected = false;
        float detCX = 0, detCY = 0, detRadius = 0, detAngVel = 0;
        if (cd.historyCount >= 5 && velocity > minVel) {
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
                float angVel = (velocity / radius) * (cross > 0 ? 1.0f : -1.0f);
                sumCX += cx; sumCY += cy; sumRadius += radius; sumAngVel += angVel;
                validCount++;
            }
            if (validCount >= 2) {
                float avgCX = sumCX / validCount;
                float avgCY = sumCY / validCount;
                float avgRadius = sumRadius / validCount;
                float avgAngVel = sumAngVel / validCount;
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
                    cd.lastCircleTime = dwTime;
                }
            }
        }

        // 检测到曲线运动：尝试更新现有漩涡或创建新漩涡 / Curved motion detected: try to update existing vortex or create new one
        if (detected) {
            float curvatureThreshold = (1.0f - g_centripetalSensitivity) * 200.0f + 50.0f;
            float curvatureBoost = fminf((curvatureThreshold - detRadius) / curvatureThreshold, 1.0f);
            float boostRate = 0.06f + curvatureBoost * 0.1f + accelBoost * 0.08f;

            // 先检查是否有现有活跃漩涡接近检测位置（在minDistance范围内），有则更新它 / Check if an existing active vortex is near detection position (within minDistance), update it if so
            bool updatedExisting = false;
            for (int i = 0; i < g_vortexMaxCount; i++) {
                Vortex &v = g_vortices[i];
                if (!v.active) continue;
                float dx = v.centerX - detCX, dy = v.centerY - detCY;
                float distSq = dx * dx + dy * dy;
                if (distSq < g_vortexMinDistance * g_vortexMinDistance) {
                    // 更新现有漩涡：平滑移动中心，增强强度 / Update existing vortex: smoothly move center, boost strength
                    v.centerX += (detCX - v.centerX) * 0.1f;
                    v.centerY += (detCY - v.centerY) * 0.1f;
                    // 给漂移速度一个推动力（沿曲率中心到光标的方向）/ Give drift velocity a push (along direction from curvature center to cursor)
                    if (g_vortexDrift > 0.01f && velocity > 1.0f) {
                        float pullDx = detCX - v.centerX;
                        float pullDy = detCY - v.centerY;
                        float pullDist = sqrtf(pullDx * pullDx + pullDy * pullDy);
                        if (pullDist > 0.1f) {
                            v.driftX += (pullDx / pullDist) * velocity * g_vortexDrift * 0.03f;
                            v.driftY += (pullDy / pullDist) * velocity * g_vortexDrift * 0.03f;
                        }
                    }
                    v.radius = detRadius;
                    v.coreRadius = detRadius * 0.3f;
                    v.isCircling = true;
                    v.strength += (1.0f - v.strength) * boostRate;
                    v.angularVel = detAngVel * (0.7f + g_centripetalForce * 0.5f) * (1.0f + accelBoost * 0.4f);
                    v.circulation = v.angularVel * v.coreRadius * v.coreRadius * 6.28318f;
                    v.pressureGradient = 0.3f + v.strength * 0.5f;
                    // 涡管拉伸：鼠标加速度拉伸涡管，增强涡量（流体力学涡度方程）/ Vortex tube stretching: mouse acceleration stretches vortex tube, boosts vorticity (fluid mechanics vorticity equation)
                    v.stretchRate = accelAbs * 0.01f;
                    v.orbitPhase += v.angularVel;
                    // 持续运动延长实际持续时间（补充能量）/ Continuous motion extends actual duration (energy replenishment)
                    float speedFactor = fminf(velocity / 15.0f, 1.0f);
                    v.actualDuration += (int)(speedFactor * g_vortexDurationSpeed * 10);
                    if (v.actualDuration > g_centripetalDuration * 3) v.actualDuration = g_centripetalDuration * 3;
                    updatedExisting = true;
                    break;
                }
            }

            // 没有可更新的现有漩涡：尝试创建新漩涡 / No existing vortex to update: try to create new one
            if (!updatedExisting && activeVortexCount < g_vortexMaxCount) {
                // 检查与所有活跃漩涡的距离 / Check distance to all active vortices
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
                    // 找到第一个空槽位 / Find first empty slot
                    for (int i = 0; i < g_vortexMaxCount; i++) {
                        if (!g_vortices[i].active) {
                            Vortex &v = g_vortices[i];
                            v.active = true;
                            v.centerX = detCX;
                            v.centerY = detCY;
                            v.driftX = 0; v.driftY = 0;
                            v.radius = detRadius;
                            v.coreRadius = detRadius * 0.3f;  // 涡核半径=轨道半径的30% / Core radius = 30% of orbit radius
                            v.angularVel = detAngVel * (0.7f + g_centripetalForce * 0.5f) * (1.0f + accelBoost * 0.4f);
                            v.circulation = v.angularVel * v.coreRadius * v.coreRadius * 6.28318f;  // Γ=2πωR²
                            v.strength = 0.1f;
                            v.strength += (1.0f - v.strength) * boostRate;
                            v.pressureGradient = 0.5f;
                            v.isCircling = true;
                            v.startTime = dwTime;
                            v.stretchRate = 0;
                            // 实际持续时间受速度影响：速度越快持续越久 / Actual duration affected by speed: faster = longer
                            float speedFactor = fminf(velocity / 15.0f, 1.0f);
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

        // 音乐联动：节拍给漩涡注入能量（给最近的活跃漩涡，或创建新漩涡）/ Music linkage: beat injects energy into vortex (nearest active, or create new)
        if (g_enableMusicPhysics && g_musicBeatPulseAmount > 0.1f) {
            float beatVortexBoost = g_musicBeatPulseAmount * g_musicVortexLink * 0.3f;
            // 找到最近的活跃漩涡并增强 / Find nearest active vortex and boost
            int nearestIdx = -1;
            float nearestDist = 1e9f;
            float cursorX = (float)(renderPos.x - vX + g_tailOffsetX);
            float cursorY = (float)(renderPos.y - vY + g_tailOffsetY);
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
                // 没有活跃漩涡，在光标位置创建一个 / No active vortex, create one at cursor position
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
                        v.startTime = dwTime;
                        v.stretchRate = 0;
                        // 音乐触发的漩涡持续时间受低频能量影响 / Music-triggered vortex duration affected by bass energy
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

        // 停止运动后：所有漩涡的isCircling设为false，开始衰减
        if (cd.isCircling && velocity < 2.0f) {
            cd.isCircling = false;
            cd.lastCircleTime = dwTime;
            for (int i = 0; i < g_vortexMaxCount; i++) {
                if (g_vortices[i].active) g_vortices[i].isCircling = false;
            }
        }

        // 更新所有漩涡：中心漂移 + 涡旋诱导速度 + 合并/湮灭 + 衰减 + 持续时间 + 轨道倾角 / Update all vortices: center drift + induced velocity + merge/annihilate + decay + duration + orbit tilt
        float cursorX = (float)(renderPos.x - vX + g_tailOffsetX);
        float cursorY = (float)(renderPos.y - vY + g_tailOffsetY);

        // 第一步：计算所有漩涡的诱导速度（二维涡旋动力学）/ Step 1: compute induced velocity for all vortices (2D vortex dynamics)
        // 同号涡：互相诱导绕共同中心旋转；异号涡：成对平移 / Same-sign: mutually induce rotation around common center; opposite-sign: pair translation
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
                // 点涡诱导速度：v = Γ/(2πd)，方向垂直于连线 / Point vortex induced velocity: v = Γ/(2πd), perpendicular to connecting line
                // Γ的符号由angularVel决定 / Γ sign determined by angularVel
                float inducedSpeed = fabsf(vj.circulation) / (6.28318f * dist) * vj.strength * 0.3f;
                // 垂直于连线方向（逆时针为正）/ Perpendicular to connecting line (counterclockwise positive)
                float perpX = -dy / dist, perpY = dx / dist;
                // 同号涡：诱导速度使它们绕转；异号涡：诱导速度使它们平移 / Same-sign: induced velocity causes rotation; opposite-sign: causes translation
                // angularVel同号=同方向旋转，诱导速度方向相同（绕转）/ Same angularVel sign = same rotation direction, induced velocity same direction (rotation)
                // angularVel异号=反方向旋转，诱导速度方向相反（平移）/ Opposite angularVel sign = opposite rotation, induced velocity opposite (translation)
                int sameDir = (vi.angularVel >= 0) == (vj.angularVel >= 0) ? 1 : -1;
                inducedX[i] += perpX * inducedSpeed * sameDir;
                inducedY[i] += perpY * inducedSpeed * sameDir;
            }
        }

        // 第二步：检测同号涡合并和异号涡湮灭 / Step 2: detect same-sign merge and opposite-sign annihilation
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

                if (sameDir && dist < (vi.coreRadius + vj.coreRadius) * 1.5f) {
                    // 同号涡合并：总环量守恒，涡核变大，强度加权平均 / Same-sign merge: total circulation conserved, core grows, strength weighted avg
                    float wi = vi.strength, wj = vj.strength;  // 保存原始权重，避免被后续修改污染 / Save original weights to avoid pollution from later modifications
                    float totalStrength = fmaxf(wi + wj, 0.001f);  // 除零保护 / Division-by-zero guard
                    float newCirculation = vi.circulation + vj.circulation;  // 同号相加 / Same-sign addition
                    float newCore = fmaxf(sqrtf(vi.coreRadius * vi.coreRadius + vj.coreRadius * vj.coreRadius) * 1.2f, 1.0f);
                    float newRadius = (vi.radius * wi + vj.radius * wj) / totalStrength;
                    // 合并到较强的漩涡（或第一个）/ Merge into stronger vortex (or first)
                    vi.centerX = (vi.centerX * wi + vj.centerX * wj) / totalStrength;
                    vi.centerY = (vi.centerY * wi + vj.centerY * wj) / totalStrength;
                    vi.circulation = newCirculation;
                    vi.coreRadius = newCore;
                    vi.radius = newRadius;
                    vi.angularVel = newCirculation / (newCore * newCore * 6.28318f);
                    vi.driftX = (vi.driftX * wi + vj.driftX * wj) / totalStrength;
                    vi.driftY = (vi.driftY * wi + vj.driftY * wj) / totalStrength;
                    vi.strength = fminf(totalStrength * 0.7f, 1.0f);
                    vi.pressureGradient = (vi.pressureGradient + vj.pressureGradient) * 0.5f;
                    vi.isCircling = vi.isCircling || vj.isCircling;
                    vi.actualDuration = (vi.actualDuration + vj.actualDuration) / 2;
                    merged[j] = true;  // 标记j为已合并（删除）/ Mark j as merged (delete)
                } else if (!sameDir && dist < (vi.coreRadius + vj.coreRadius) * 1.2f) {
                    // 异号涡湮灭：强度抵消，较弱的消失 / Opposite-sign annihilation: strengths cancel, weaker disappears
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
        // 应用合并/湮灭删除 / Apply merge/annihilation deletion
        for (int i = 0; i < g_vortexMaxCount; i++) {
            if (merged[i]) {
                g_vortices[i].active = false;
                g_vortices[i].strength = 0;
            }
        }

        // 第三步：应用漂移、诱导速度和其他更新 / Step 3: apply drift, induced velocity and other updates
        for (int i = 0; i < g_vortexMaxCount; i++) {
            Vortex &v = g_vortices[i];
            if (!v.active) continue;

            // 1. 漩涡中心漂移：受鼠标速度和位置影响 / 1. Vortex center drift: affected by mouse velocity and position
            if (g_vortexDrift > 0.01f) {
                float dx = cursorX - v.centerX;
                float dy = cursorY - v.centerY;
                float dist = sqrtf(dx * dx + dy * dy);
                if (dist > 1.0f && dist < 500.0f) {
                    float pullStrength = g_vortexDrift * 0.02f * (1.0f - dist / 500.0f);
                    v.driftX += (dx / dist) * pullStrength * velocity * 0.1f;
                    v.driftY += (dy / dist) * pullStrength * velocity * 0.1f;
                }
                v.driftX *= 0.95f;
                v.driftY *= 0.95f;
                v.centerX += v.driftX;
                v.centerY += v.driftY;
            }

            // 2. 应用涡旋诱导速度（同号绕转，异号平移）/ 2. Apply vortex induced velocity (same-sign rotation, opposite-sign translation)
            v.centerX += inducedX[i];
            v.centerY += inducedY[i];

            // 3. 持续时间结束：强制开始衰减 / 3. Duration ended: force decay start
            if (dwTime - v.startTime > (DWORD)v.actualDuration) {
                v.isCircling = false;
            }

            // 3.5 涡管拉伸增强 / 3.5 Vortex tube stretching boost
            if (v.isCircling && v.stretchRate > 0.001f) {
                v.strength += v.stretchRate * 0.5f;
                if (v.strength > 1.0f) v.strength = 1.0f;
                v.coreRadius *= (1.0f - v.stretchRate * 0.1f);
                if (v.coreRadius < v.radius * 0.1f) v.coreRadius = v.radius * 0.1f;
                v.angularVel = v.circulation / (v.coreRadius * v.coreRadius * 6.28318f);
                v.stretchRate *= 0.9f;
            }

            // 4. 衰减 / 4. Decay
            if (!v.isCircling && v.strength > 0) {
                v.strength *= 0.92f;
                v.coreRadius += (v.radius * 0.3f - v.coreRadius) * 0.05f;
                if (v.strength < 0.02f) {
                    v.active = false;
                    v.strength = 0;
                }
            }

            // 5. 轨道倾角 / 5. Orbit tilt
            if (v.strength > 0.1f) {
                v.orbitTilt += v.orbitTiltVel;
                if (v.orbitTilt < 0.2f || v.orbitTilt > 1.2f) v.orbitTiltVel = -v.orbitTiltVel;
                v.orbitTilt = fmaxf(0.2f, fminf(1.2f, v.orbitTilt));
            }
        }

        // 清除完全没有活跃漩涡时的粒子漩涡亮度 / Clear particle vortex brightness when no active vortices at all
        bool anyActive = false;
        for (int i = 0; i < g_vortexMaxCount; i++) {
            if (g_vortices[i].active && g_vortices[i].strength > 0.05f) { anyActive = true; break; }
        }
        if (!anyActive) {
            // 不重置historyCount，让位置历史自然积累，否则检测永远触发不了 / Don't reset historyCount, let position history accumulate naturally, else detection never triggers
            for (auto &p : g_particles) {
                p.vortexBrightness[0] = 0;
                p.vortexBrightness[1] = 0;
                p.vortexBrightness[2] = 0;
            }
        }

        // 向心力作用于粒子：Rankine涡模型 + 角动量守恒 + 径向压力梯度 / Centripetal force on particles: Rankine vortex model + angular momentum conservation + radial pressure gradient
        // Rankine vortex: core (r≤R) rigid rotation vθ=ωr, outer (r>R) free vortex vθ=Γ/(2πr)
        // 牛顿第三定律：粒子对漩涡中心的反作用力累积 / Newton's third law: particle reaction force on vortex center accumulates
        if (!g_particles.empty()) {
            for (int vi = 0; vi < g_vortexMaxCount; vi++) {
                Vortex &v = g_vortices[vi];
                if (!v.active || v.strength <= 0.05f) continue;

                float force = g_centripetalForce * v.strength;
                float influenceRadius = v.radius * 4.0f;
                float decaying = v.isCircling ? 0.0f : 1.0f;
                float invCore = 1.0f / fmaxf(v.coreRadius, 1.0f);
                float circOver2Pi = v.circulation / 6.28318f;  // Γ/(2π)
                int rotDir = v.angularVel >= 0 ? 1 : -1;

                // 粒子反作用力累积（牛顿第三定律）/ Particle reaction force accumulation (Newton's third law)
                float reactionX = 0, reactionY = 0;
                int affectedCount = 0;

                for (auto &p : g_particles) {
                    float dx = v.centerX - p.x;
                    float dy = v.centerY - p.y;
                    float dist = sqrtf(dx * dx + dy * dy);
                    if (dist > 2.0f && dist < influenceRadius) {
                        float nx = dx / dist, ny = dy / dist;  // 径向（指向中心）
                        float tx = -ny * rotDir, ty = nx * rotDir;  // 切向（旋转方向）
                        float radialVel = p.vx * nx + p.vy * ny;
                        float tangentVel = p.vx * tx + p.vy * ty;

                        // 记录施加给粒子的总力，用于反作用力计算 / Record total force on particle for reaction force calculation
                        float forceOnParticleX = 0, forceOnParticleY = 0;

                        // 1. 根据物理模型计算目标切向速度 / 1. Compute target tangential velocity based on physics model
                        float targetTangentVel;
                        switch (g_vortexPhysModel) {
                            case 1: {  // 自由涡：纯角动量守恒，vθ=Γ/(2πr) / Free vortex: pure angular momentum conservation
                                targetTangentVel = circOver2Pi / dist;
                                break;
                            }
                            case 2: {  // 刚体旋转：整体匀速旋转，vθ=ω·r / Solid rotation: uniform rotation
                                targetTangentVel = v.angularVel * dist;
                                break;
                            }
                            case 3: {  // Lamb-Oseen涡：粘性高斯涡核 / Lamb-Oseen vortex: viscous Gaussian core
                                float gauss = 1.0f - expf(-(dist * dist) / (2.0f * v.coreRadius * v.coreRadius));
                                targetTangentVel = (circOver2Pi / dist) * gauss;
                                break;
                            }
                            case 4: {  // 开普勒轨道：行星模型 v=sqrt(GM/r) / Kepler orbit: planetary model
                                float GM = v.angularVel * v.angularVel * v.radius * v.radius * v.radius;
                                targetTangentVel = sqrtf(fmaxf(GM / fmaxf(dist, 5.0f), 0.5f));
                                break;
                            }
                            case 0:  // Rankine涡（默认）/ Rankine vortex (default)
                            default: {
                                if (dist <= v.coreRadius) {
                                    targetTangentVel = v.angularVel * dist;
                                } else {
                                    targetTangentVel = circOver2Pi / dist;
                                }
                                break;
                            }
                        }

                        // 2. 径向压力梯度力（中心低压吸力，根据物理模型变化）/ 2. Radial pressure gradient force (center low-pressure suction, varies by model)
                        float pressureForce;
                        switch (g_vortexPhysModel) {
                            case 1:  // 自由涡：伯努利方程，压力∝1/r² / Free vortex: Bernoulli, pressure ∝ 1/r²
                                pressureForce = force * v.pressureGradient * (v.coreRadius * v.coreRadius) / (dist * dist);
                                break;
                            case 2:  // 刚体旋转：离心力平衡，压力∝r / Solid rotation: centrifugal balance, pressure ∝ r
                                pressureForce = force * v.pressureGradient * (dist / v.radius);
                                break;
                            case 4:  // 开普勒：引力∝1/r² / Kepler: gravity ∝ 1/r²
                                pressureForce = force * v.pressureGradient * (v.radius * v.radius) / (dist * dist);
                                break;
                            case 3:  // Lamb-Oseen：高斯过渡 / Lamb-Oseen: Gaussian transition
                            case 0:  // Rankine（默认）/ Rankine (default)
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
                        p.vx += nx * effectivePressure;
                        p.vy += ny * effectivePressure;

                        // 3. 衰减时离心甩出 / 3. Centrifugal ejection during decay
                        if (decaying > 0.5f) {
                            float centrifugal = force * 3.5f * (1.0f - dist / influenceRadius * 0.5f);
                            forceOnParticleX -= nx * centrifugal;
                            forceOnParticleY -= ny * centrifugal;
                            p.vx -= nx * centrifugal;
                            p.vy -= ny * centrifugal;
                        }

                        // 4. 活跃阶段：角动量守恒驱动 + 粘性阻尼 / 4. Active phase: angular momentum conservation drive + viscous damping
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
                            p.vx += tx * tangentForce;
                            p.vy += ty * tangentForce;

                            float radialDamping = -radialVel * 0.04f * force;
                            forceOnParticleX += nx * radialDamping;
                            forceOnParticleY += ny * radialDamping;
                            p.vx += nx * radialDamping;
                            p.vy += ny * radialDamping;

                            // 软弹簧约束 / Soft spring constraint
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
                                float springForce = -radiusError * 0.006f * force;
                                springForce -= radialVel * 0.015f * force;
                                forceOnParticleX += nx * springForce;
                                forceOnParticleY += ny * springForce;
                                p.vx += nx * springForce;
                                p.vy += ny * springForce;
                            }

                            float orbitAngle = atan2f(p.y - v.centerY, p.x - v.centerX);
                            float precession = 0.0015f * force * sinf(orbitAngle * 2.0f);
                            forceOnParticleX += tx * precession;
                            forceOnParticleY += ty * precession;
                            p.vx += tx * precession;
                            p.vy += ty * precession;
                        }

                        // 5. 3D轨道倾角 + 亮度 / 5. 3D orbit tilt + brightness
                        if (v.isCircling) {
                            float orbitAngle = atan2f(p.y - v.centerY, p.x - v.centerX);
                            float depthFactor = sinf(orbitAngle) * sinf(v.orbitTilt);
                            // z以0为中心振荡（与普通粒子z∈[-0.4,0.4]协调），轨道倾角产生前后景深，而非整体推到前景 / z oscillates around 0 (matches normal particle z range), tilt creates depth instead of pushing everything forward
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
                            // 漩涡衰减：z回归0（普通粒子深度中心），避免永久偏大偏亮 / Decay: z returns to 0 (normal particle depth center), avoid permanent size/brightness offset
                            p.z += (0.0f - p.z) * 0.1f;
                            p.vortexBrightness[0] *= 0.9f;
                            p.vortexBrightness[1] *= 0.9f;
                            p.vortexBrightness[2] *= 0.9f;
                        }

                        // 累积反作用力（牛顿第三定律：粒子对漩涡的力 = -漩涡对粒子的力）/ Accumulate reaction force (Newton's third law: force on vortex = -force on particle)
                        // 粒子质量越大，反作用力越强 / Heavier particles exert stronger reaction force
                        float particleMass = g_enableParticleMass ? p.mass : 1.0f;
                        reactionX -= forceOnParticleX * particleMass * 0.02f;
                        reactionY -= forceOnParticleY * particleMass * 0.02f;
                        // 额外：粒子动量对漩涡中心的推动（粒子分布不对称时产生净推力）/ Extra: particle momentum pushes vortex center (net thrust when particle distribution asymmetric)
                        reactionX += p.vx * particleMass * 0.0005f;
                        reactionY += p.vy * particleMass * 0.0005f;
                        affectedCount++;
                    }
                }

                // 应用粒子反作用力到漩涡中心漂移 / Apply particle reaction force to vortex center drift
                if (affectedCount > 0 && g_vortexDrift > 0.01f) {
                    // 反作用力系数：粒子越多影响越大（sqrt归一化，避免过度）/ Reaction coefficient: more particles = stronger effect (sqrt normalized to avoid excess)
                    float countFactor = sqrtf((float)affectedCount) * 0.3f;
                    float reactionScale = g_vortexDrift * countFactor;
                    v.driftX += reactionX * reactionScale;
                    v.driftY += reactionY * reactionScale;
                    // 限制漂移速度，避免飞出去 / Limit drift speed to prevent flying away
                    float driftSpeed = sqrtf(v.driftX * v.driftX + v.driftY * v.driftY);
                    float maxDrift = 5.0f * g_vortexDrift;
                    if (driftSpeed > maxDrift) {
                        v.driftX = v.driftX / driftSpeed * maxDrift;
                        v.driftY = v.driftY / driftSpeed * maxDrift;
                    }
                }
            }
        }
    } else {
        // 漩涡功能关闭：衰减残留的漩涡亮度和z深度，避免关闭后效果残留 / Vortex disabled: decay residual brightness and z to avoid lingering effects after toggle-off
        for (auto &p : g_particles) {
            p.vortexBrightness[0] *= 0.9f;
            p.vortexBrightness[1] *= 0.9f;
            p.vortexBrightness[2] *= 0.9f;
            p.z += (0.0f - p.z) * 0.1f;
        }
    }

    // ===== 粒子间排斥力 + 弹性碰撞（与质量相关）=====
    if ((g_enableParticleInteraction || g_enableParticleCollision || g_enableParticleSpring) && !g_particles.empty()) {
        int pcount = (int)g_particles.size();
        int maxCalc = GetParticleCap();
        if (pcount <= maxCalc) {
            float repelDist = (float)g_interParticleRepelDistance;
            // 弹簧可能在更大距离生效，扩大双循环距离范围 / Spring may act over larger range, widen pair loop
            float pairRange = g_enableParticleSpring ? fmaxf(repelDist, g_springMaxDist) : repelDist;
            float pairRangeSq = pairRange * pairRange;
            float springRest = g_springMaxDist * 0.5f;  // 弹簧自然长度 / spring rest length
            const int MAX_SPRING_LINKS = 5;  // 每粒子最多弹簧连接数，防中心过压 / max links per particle, anti center over-constraint
            float softEndDist = g_springMaxDist * 0.9f;  // 软化区起点 / softening zone start
            float softRange = g_springMaxDist - softEndDist;
            if (g_enableParticleSpring) {
                g_springLinkCount.assign(pcount, 0);
            }
            for (int i = 0; i < pcount; i++) {
                for (int j = i + 1; j < pcount; j++) {
                    float dx = g_particles[i].x - g_particles[j].x;
                    float dy = g_particles[i].y - g_particles[j].y;
                    float distSq = dx * dx + dy * dy;
                    if (distSq < pairRangeSq && distSq > 0.01f) {
                        float dist = sqrtf(distSq);
                        float nx = dx / dist, ny = dy / dist;
                        // 排斥力 / Repulsion force
                        if (g_enableParticleInteraction && dist < repelDist) {
                            float falloff = 1.0f - dist / repelDist;
                            float massFactor = g_enableParticleMass ? sqrtf(g_particles[i].mass * g_particles[j].mass) : 1.0f;
                            float force = falloff * g_interParticleRepelForce / dist * massFactor;
                            float invMassI = g_enableParticleMass ? (1.0f / g_particles[i].mass) : 1.0f;
                            float invMassJ = g_enableParticleMass ? (1.0f / g_particles[j].mass) : 1.0f;
                            g_particles[i].vx += nx * force * invMassI;
                            g_particles[i].vy += ny * force * invMassI;
                            g_particles[j].vx -= nx * force * invMassJ;
                            g_particles[j].vy -= ny * force * invMassJ;
                        }
                        // 弹性碰撞：距离小于两粒子半径之和时 / Elastic collision: when distance < sum of radii
                        if (g_enableParticleCollision) {
                            // 文字粒子使用实际渲染尺寸的包围圆半径 / Text particles use actual rendered bounding circle radius
                            float sizeMulCol = g_particleSizeMultiplier / 100.0f;
                            float radiusI = (g_particles[i].shapeType == 10)
                                ? (g_textFontSize * 0.5f * sizeMulCol * 0.5f) * sqrtf(1.0f + g_textAspectRatio * g_textAspectRatio)
                                : g_particles[i].size * 0.5f;
                            float radiusJ = (g_particles[j].shapeType == 10)
                                ? (g_textFontSize * 0.5f * sizeMulCol * 0.5f) * sqrtf(1.0f + g_textAspectRatio * g_textAspectRatio)
                                : g_particles[j].size * 0.5f;
                            float minDist = radiusI + radiusJ;
                            if (dist < minDist) {
                                // 位置修正：按质量反比分配位移，重粒子移动少、轻粒子移动多 / Position correction: inverse-mass distribution, heavy moves less, light moves more
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
                                // 弹性碰撞（动量守恒+动能守恒）/ Elastic collision (momentum + kinetic energy conservation)
                                // 相对速度在法向上的分量 / Relative velocity component along normal
                                float dvx = g_particles[i].vx - g_particles[j].vx;
                                float dvy = g_particles[i].vy - g_particles[j].vy;
                                float vn = dvx * nx + dvy * ny;
                                if (vn > 0) {  // 正在接近才碰撞 / Only collide if approaching
                                    float impulse = (2.0f * vn) / totalMass;
                                    g_particles[i].vx -= impulse * mj * nx;
                                    g_particles[i].vy -= impulse * mj * ny;
                                    g_particles[j].vx += impulse * mi * nx;
                                    g_particles[j].vy += impulse * mi * ny;
                                }
                            }
                        }
                        // 粘性耦合：邻近粒子间的速度拖拽（流体效应）/ Viscous coupling: velocity drag between nearby particles (fluid effect)
                        if (g_enableBrownianMotion && dist < 30.0f) {
                            float viscosity = 0.003f * (1.0f - dist / 30.0f);
                            float dvx = (g_particles[i].vx - g_particles[j].vx) * viscosity;
                            float dvy = (g_particles[i].vy - g_particles[j].vy) * viscosity;
                            g_particles[i].vx -= dvx;
                            g_particles[i].vy -= dvy;
                            g_particles[j].vx += dvx;
                            g_particles[j].vy += dvy;
                        }
                        // 布料弹簧力：胡克 + 沿连线阻尼 + 邻居上限 + 软化截止 / Cloth spring: Hooke + axial damping + link cap + soft cutoff
                        if (g_enableParticleSpring && dist < g_springMaxDist && dist > 0.5f
                            && g_springLinkCount[i] < MAX_SPRING_LINKS && g_springLinkCount[j] < MAX_SPRING_LINKS) {
                            float displacement = dist - springRest;
                            // 软化截止：接近最大连接距离时力平滑归零，防硬截止弹射 / soft cutoff: force smoothly dies near max dist, no hard pop
                            float fade = 1.0f;
                            if (dist > softEndDist) fade = (g_springMaxDist - dist) / softRange;
                            float springForce = g_springK * displacement * fade;
                            // 沿连线的相对速度（i 相对 j 远离 j 的速度分量）/ relative velocity along axis (i receding from j)
                            float dvx = g_particles[i].vx - g_particles[j].vx;
                            float dvy = g_particles[i].vy - g_particles[j].vy;
                            float vn = dvx * nx + dvy * ny;
                            // 阻尼系数与刚度成比例，快速耗散沿连线振荡 / damping proportional to stiffness, kills axial oscillation
                            float damper = g_springK * 14.0f * fade;
                            float fmag = springForce + damper * vn;
                            g_springLinkCount[i]++;
                            g_springLinkCount[j]++;
                            // 限幅，防止极端情况力爆炸 / clamp to avoid force blowup
                            if (fmag > 1.5f) fmag = 1.5f;
                            if (fmag < -1.5f) fmag = -1.5f;
                            float imi = g_enableParticleMass ? (1.0f / g_particles[i].mass) : 1.0f;
                            float imj = g_enableParticleMass ? (1.0f / g_particles[j].mass) : 1.0f;
                            g_particles[i].vx -= nx * fmag * imi;
                            g_particles[i].vy -= ny * fmag * imi;
                            g_particles[j].vx += nx * fmag * imj;
                            g_particles[j].vy += ny * fmag * imj;
                        }
                    }
                }
            }
        }
    }

    // ===== 粒子万有引力系统（牛顿万有引力定律 + 牛顿第二定律 + Plummer软化）===== / Particle gravity system (Newton's law of gravitation + 2nd law + Plummer softening)
    if (g_enableParticleGravity && g_particles.size() >= 2) {
        int pcount = (int)g_particles.size();
        int bodyCount = g_gravitySystem == 0 ? 2 : (int)fminf((float)g_gravityBodyCount, (float)pcount);
        if (bodyCount > pcount) bodyCount = pcount;
        if (bodyCount < 1) bodyCount = 1;
        g_gravityBodies.clear();
        // 复用工作缓冲，避免每帧堆分配 / Reuse scratch buffer to avoid per-frame heap allocation
        g_gravityMassIdx.resize(pcount);
        for (int i = 0; i < pcount; i++) {
            g_gravityMassIdx[i] = {g_particles[i].mass, i};
        }
        std::partial_sort(g_gravityMassIdx.begin(), g_gravityMassIdx.begin() + bodyCount, g_gravityMassIdx.end(),
                          [](const std::pair<float,int> &a, const std::pair<float,int> &b) { return a.first > b.first; });
        for (int i = 0; i < bodyCount; i++) {
            g_gravityBodies.push_back(g_gravityMassIdx[i].second);
        }
        float G = g_gravityStrength * 50.0f;
        if (g_enableMusicPhysics && g_musicGravityLink > 0) {
            float volumeBoost = 1.0f + fminf(g_music.overallLevel * 20.0f, 2.0f) * g_musicGravityLink;
            G *= volumeBoost;
        }
        // 多频段联动：低频增强引力 / Multi-band linkage: bass enhances gravity
        if (g_enableMultiBandLink) {
            G *= (1.0f + g_bassGravityBoost * 1.5f);
        }
        // Plummer软化半径：避免近距离引力奇点 / Plummer softening radius: avoid near-distance gravity singularity
        float softening = 8.0f;
        float softeningSq = softening * softening;
        float maxDist = 400.0f;  // 引力截断距离 / Gravity cutoff distance
        float maxDistSq = maxDist * maxDist;
        // 先计算所有引力源之间的相互作用（引力源也运动）/ First compute interactions between all gravity sources (sources also move)
        for (int bi = 0; bi < (int)g_gravityBodies.size(); bi++) {
            int i = g_gravityBodies[bi];
            float ax = 0, ay = 0;
            for (int bj = 0; bj < (int)g_gravityBodies.size(); bj++) {
                if (bi == bj) continue;
                int j = g_gravityBodies[bj];
                float dx = g_particles[j].x - g_particles[i].x;
                float dy = g_particles[j].y - g_particles[i].y;
                float distSq = dx * dx + dy * dy + softeningSq;
                if (distSq > maxDistSq) continue;
                float dist = sqrtf(distSq);
                float accel = G * g_particles[j].mass / distSq;
                ax += (dx / dist) * accel;
                ay += (dy / dist) * accel;
            }
            g_particles[i].vx += ax * 0.5f;  // 引力源运动减半（避免太剧烈）/ Gravity source movement halved (avoid too violent)
            g_particles[i].vy += ay * 0.5f;
        }
        // 用bool数组标记引力源，O(1)查找替代线性查找（复用缓冲避免每帧分配）/ Mark gravity sources with bool array, O(1) lookup replaces linear search (reuse buffer)
        g_gravityBodyFlag.assign(pcount, false);
        for (int bi = 0; bi < (int)g_gravityBodies.size(); bi++) {
            g_gravityBodyFlag[g_gravityBodies[bi]] = true;
        }
        // 计算所有粒子受到引力源的引力 / Compute gravity from sources on all particles
        for (int i = 0; i < pcount; i++) {
            if (g_gravityBodyFlag[i]) continue;  // 引力源已计算 / Gravity source already computed
            float ax = 0, ay = 0;
            for (int bi = 0; bi < (int)g_gravityBodies.size(); bi++) {
                int j = g_gravityBodies[bi];
                float dx = g_particles[j].x - g_particles[i].x;
                float dy = g_particles[j].y - g_particles[i].y;
                float distSq = dx * dx + dy * dy + softeningSq;
                if (distSq > maxDistSq) continue;
                float dist = sqrtf(distSq);
                float accel = G * g_particles[j].mass / distSq;
                ax += (dx / dist) * accel;
                ay += (dy / dist) * accel;
            }
            g_particles[i].vx += ax;
            g_particles[i].vy += ay;
        }
    }

    // ===== 粒子物理：空气阻力 + 速度上限 + 光标排斥/吸附 + 音乐脉冲 =====
    // 风场随时间演化：基准水平方向 + 阵风 + 方向摆动 / Wind evolves over time: baseline horizontal + gusts + direction sway
    if (g_enableEnvWind) {
        float wt = dwTime * 0.001f;
        float gust = g_windStrength * (0.5f + 0.5f * sinf(wt * 0.9f));           // 阵风包络
        float sway = g_windTurbulence * 0.7f * sinf(wt * 0.4f + 1.3f);          // 风向摆动（弧度）
        float jitter = g_windTurbulence * 0.25f * sinf(wt * 2.1f);              // 高频小抖动
        g_windX = cosf(sway) * gust + jitter;
        g_windY = sinf(sway) * gust * 0.25f + g_windTurbulence * 0.15f * sinf(wt * 1.3f + 4.0f);
    } else {
        g_windX = 0; g_windY = 0;
    }
    float attractTargetX = (float)(renderPos.x - vX + g_tailOffsetX);
    float attractTargetY = (float)(renderPos.y - vY + g_tailOffsetY);
    const float MAX_SPEED = 25.0f;  // 速度上限（像素/帧）/ Speed cap (pixels/frame)
    const float MAX_SPEED_SQ = MAX_SPEED * MAX_SPEED;
    for (auto &p : g_particles) {
        // 质量影响惯性：质量大的空气阻力小，速度保持更久 / Mass affects inertia: heavier particles have less drag, retain velocity longer
        float invMass = g_enableParticleMass ? (1.0f / p.mass) : 1.0f;
        // 预计算到光标的距离（音乐脉冲和光标排斥共用）/ Precompute distance to cursor (shared by music pulse and cursor repulsion)
        float toCursorDx = p.x - attractTargetX;
        float toCursorDy = p.y - attractTargetY;
        float toCursorDistSq = toCursorDx * toCursorDx + toCursorDy * toCursorDy;
        // 空气阻力模型：F_drag = -k * v（低速线性阻力）+ 高速二次阻力 / Air drag model: F_drag = -k*v (low-speed linear) + high-speed quadratic
        float speedSq = p.vx * p.vx + p.vy * p.vy;
        float speed = sqrtf(speedSq);
        float dragCoeff = g_enableParticleMass ? (0.04f / fmaxf(p.mass, 0.2f)) : 0.05f;
        float linearDrag = 1.0f - dragCoeff;
        float quadraticDrag = speed > 10.0f ? (1.0f - dragCoeff * 0.5f * (speed - 10.0f) / 15.0f) : 1.0f;
        float totalDrag = fmaxf(fminf(linearDrag * quadraticDrag, 0.98f), 0.80f);
        p.vx *= totalDrag;
        p.vy *= totalDrag;
        // 环境重力：等效原理，加速度与质量无关（不乘invMass）/ Environmental gravity: equivalence principle, accel mass-independent (no invMass)
        if (g_enableEnvGravity) {
            p.vx += cosf(g_envGravityAngle) * g_envGravityStrength;
            p.vy += sinf(g_envGravityAngle) * g_envGravityStrength;
        }
        // 环境风：表面力，轻粒子被吹得更远（乘invMass，像羽毛vs石头）/ Environmental wind: surface force, light particles blow further (x invMass, feather vs stone)
        if (g_enableEnvWind) {
            p.vx += g_windX * invMass;
            p.vy += g_windY * invMass;
        }
        // 音乐节拍脉冲：每拍给粒子一个径向速度爆发（从光标向外）/ Music beat pulse: radial velocity burst per beat (outward from cursor)
        if (g_enableMusicPhysics && g_musicBeatPulseAmount > 0.01f && toCursorDistSq > 1.0f) {
            float pdist = sqrtf(toCursorDistSq);
            float pulseForce = g_musicBeatPulseAmount * 8.0f * invMass;
            p.vx += (toCursorDx / pdist) * pulseForce;
            p.vy += (toCursorDy / pdist) * pulseForce;
        }
        // 光标周围排斥力：加速度=F/m，质量大的加速度小 / Cursor repulsion: a=F/m, heavier particles accelerate less
        if (g_enableParticleRepel && g_particleRepelForce > 0 && toCursorDistSq < (float)g_particleRepelRadius * g_particleRepelRadius && toCursorDistSq > 0.25f) {
            float rdist = sqrtf(toCursorDistSq);
            float nx = toCursorDx / rdist, ny = toCursorDy / rdist;
            float falloff = 1.0f - rdist / (float)g_particleRepelRadius;
            float force = falloff * g_particleRepelForce;
            float accel = force * invMass;
            p.vx += nx * accel;
            p.vy += ny * accel;
            float perturb = force * 0.65f * invMass;
            float angle = Rand01() * 6.28318f;
            p.vx += cosf(angle) * perturb;
            p.vy += sinf(angle) * perturb;
        }
        // 洛伦兹力：带电粒子在磁场中做圆周运动 F = q(v × B) / Lorentz force: charged particles circle in magnetic field F = q(v × B)
        // 2D中：F = q * B * (-vy, vx)，正电荷逆时针，负电荷顺时针 / In 2D: F = q*B*(-vy, vx), positive CCW, negative CW
        if (g_enableLorentzForce) {
            float B = g_lorentzStrength * 0.5f;
            // 多频段联动：中频增强磁场 / Multi-band linkage: mid enhances magnetic field
            if (g_enableMultiBandLink) B *= (1.0f + g_midRepelBoost * 0.5f);
            float lx = -p.vy * B * p.charge;
            float ly = p.vx * B * p.charge;
            p.vx += lx * invMass;
            p.vy += ly * invMass;
        }
        // 布朗运动：空间连贯湍流场（Perlin-like），比纯随机更自然 / Brownian motion: coherent turbulence field, more natural than pure random
        if (g_enableBrownianMotion) {
            float noise = g_brownianStrength * 0.5f;
            if (g_enableMultiBandLink) noise *= (1.0f + g_trebleNoiseBoost * 2.0f);
            float nx = CoherentNoise(p.x * 0.02f, p.y * 0.02f, dwTime * 0.001f, 1);
            float ny = CoherentNoise(p.x * 0.02f + 100.0f, p.y * 0.02f + 100.0f, dwTime * 0.001f, 2);
            p.vx += nx * noise * invMass;
            p.vy += ny * noise * invMass;
        }
        // 库仑力：同号电荷相斥，异号电荷相吸（粒子间）/ Coulomb force: like charges repel, opposite attract (between particles)
        if (g_enableLorentzForce && fabsf(p.charge) > 0.01f) {
            for (auto &q : g_particles) {
                if (&q == &p) continue;
                float dx = q.x - p.x, dy = q.y - p.y;
                float dSq = dx*dx + dy*dy;
                if (dSq < 4.0f || dSq > 40000.0f) continue;
                float d = sqrtf(dSq);
                float force = 800.0f * p.charge * q.charge / dSq;
                p.vx += (dx / d) * force * invMass;
                p.vy += (dy / d) * force * invMass;
            }
        }
        // 空气阻力受粒子大小影响：大粒子迎风面积大，阻力更大 / Size-based drag: larger particles have more air resistance
        if (g_enableParticleMass && p.size > 2.0f) {
            float sizeDrag = 1.0f - fminf(p.size * 0.002f, 0.02f);
            p.vx *= sizeDrag;
            p.vy *= sizeDrag;
        }
        // 速度上限：避免粒子飞太快（必须在所有力施加完毕后重新计算，不能用阻力前的旧速度）/ Speed cap: recompute after ALL forces applied, not pre-drag old speed
        float finalSpeedSq = p.vx * p.vx + p.vy * p.vy;
        if (finalSpeedSq > MAX_SPEED_SQ) {
            float finalSpeed = sqrtf(finalSpeedSq);
            float scale = MAX_SPEED / finalSpeed;
            p.vx *= scale;
            p.vy *= scale;
        }
        p.x += p.vx;
        p.y += p.vy;
        // 调试：记录本帧总受力 F = m * Δv（快照在所有力施加前保存，涵盖全部力）/ Debug: total force F=m*Δv (snapshot taken before all forces, covers everything)
        if (g_debugForce) {
            float snapVx = p.debugForceX, snapVy = p.debugForceY;
            p.debugForceX = p.mass * (p.vx - snapVx);
            p.debugForceY = p.mass * (p.vy - snapVy);
        }
        if (g_enableParticleSpin) {
            p.rotation += p.spinSpeed;
            p.spinSpeed *= 0.995f;  // 旋转空气阻尼 / Spin air damping
        }
        // 文字分段轮播：逐粒子模式按自身存活时间，全局模式按统一时钟 / Text chunk cycling: per-particle uses own age, global mode uses a shared clock
        if (p.shapeType == 10 && p.chunkCount > 1) {
            DWORD chunkBase = (g_textChunkMode == 1) ? g_globalChunkClock : p.startTime;
            DWORD chunkElapsed = dwTime - chunkBase;
            int chunkSlot = (int)(chunkElapsed / (DWORD)g_textChunkDelay) % p.chunkCount;
            if (chunkSlot < 0) chunkSlot = 0;
            p.charIndex = (float)(p.chunkStart + chunkSlot);
        }
        // 光标吸附：质量大的吸附加速度小 / Cursor attraction: heavier particles attract less
        if (g_particleAttraction > 0) {
            float attractAccel = g_particleAttraction * invMass;
            p.x += (attractTargetX - p.x) * attractAccel;
            p.y += (attractTargetY - p.y) * attractAccel;
        }
        // 质量影响亮度：大质量粒子更亮（通过vortexBrightness叠加）/ Mass affects brightness: heavier particles brighter (via vortexBrightness)
        if (g_enableParticleMass && p.mass > 1.2f) {
            float massBrightness = (p.mass - 1.0f) * 0.08f;
            p.vortexBrightness[0] = fmaxf(p.vortexBrightness[0], massBrightness);
            p.vortexBrightness[1] = fmaxf(p.vortexBrightness[1], massBrightness);
            p.vortexBrightness[2] = fmaxf(p.vortexBrightness[2], massBrightness);
        }
    }
    if (!g_particles.empty())
        g_particles.erase(std::remove_if(g_particles.begin(), g_particles.end(),
                                         [&](const Particle &p) { return dwTime - p.startTime > (DWORD)p.lifetime; }),
                          g_particles.end());
    // 形状拖尾物理更新（速度衰减 + 旋转 + 微重力）/ Shape trail physics update (velocity decay + rotation + micro-gravity)
    for (auto &s : g_trailShapes) {
        s.x += s.vx;
        s.y += s.vy;
        s.vx *= 0.96f;
        s.vy *= 0.96f;
        s.vy += 0.02f;  // 微重力，形状缓慢下落 / Micro-gravity, shapes slowly fall
        s.rotation += s.rotSpeed;  // 旋转 / Rotation
        s.rotSpeed *= 0.98f;  // 旋转阻尼 / Rotation damping
    }
    // 形状拖尾过期清理 / Shape trail expiration cleanup
    if (!g_trailShapes.empty())
        g_trailShapes.erase(std::remove_if(g_trailShapes.begin(), g_trailShapes.end(),
                                            [&](const TrailShape &s) { return dwTime - s.startTime > (DWORD)s.lifetime; }),
                             g_trailShapes.end());

    bool tailVisible = (trailActive || g_history.size() >= 2) && g_fadeAlpha > 0.02f;
    bool isDrawing = tailVisible || !g_ripples.empty() || !g_particles.empty() || !g_trailShapes.empty();

    if (isDrawing) {
        hideDelayCounter = 0;
        if (!isWindowVisible) {
            // 通过 PostMessage 在 UI 线程显示窗口（不激活），点击穿透由 WM_NCHITTEST 返回 HTTRANSPARENT 实现
            // Post to UI thread to show window (no activate); click-through via WM_NCHITTEST HTTRANSPARENT
            PostMessageW(g_overlayHwnd, WM_APP_SHOW_OVERLAY, 0, 0);
            isWindowVisible = true;
        }
    } else if (!surfaceDirty) {
        hideDelayCounter++;
        if (hideDelayCounter >= 3 && isWindowVisible) {
            PostMessageW(g_overlayHwnd, WM_APP_HIDE_OVERLAY, 0, 0);
            isWindowVisible = false;
        }
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

    // v3：原生 D3D11 渲染路径（锥形拖尾 + 粒子）/ v3: native D3D11 render path (conical trail + particles)
    bool useNative = (g_pNativeVS != nullptr);  // 文字形状(10)也走GPU字符图集 / text shape(10) uses GPU char atlas too  // v3：文字形状(10)走D2D路径，跳过GPU原生 / v3: text shape(10) uses D2D, skip GPU native
    if (useNative) {
        bool nativeOK = NativeRenderFrame(vW, vH, smoothed, tailVisible, cols, widthMul, g_fadeAlpha, dwTime, vX, vY);
        if (nativeOK) {
            // 原生渲染成功：所有效果（拖尾、粒子、形状、点击、运动模糊）均用 D3D11 原生渲染 / Native render success: all effects via D3D11 native
            // MSAA Resolve + SSAA Blit / MSAA resolve + SSAA downsample blit
            {
                ID3D11Texture2D *pBackBuffer = nullptr;
                ID3D11RenderTargetView *pSwapRTV = nullptr;
                HRESULT hrBuf = g_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&pBackBuffer);

                if (SUCCEEDED(hrBuf) && g_msaaSamples > 1 && g_pMSAATexture && g_pSSAAResolveTexture) {
                    // MSAA resolve 到非 MSAA 纹理 / MSAA resolve to non-MSAA texture
                    g_pD3DContext->ResolveSubresource(g_pSSAAResolveTexture, 0, g_pMSAATexture, 0, g_swapChainFormat);
                }

                if ((g_msaaSamples > 1 && g_pSSAAResolveSRV) || (g_ssaaScale > 1 && g_pSSAASRV)) {
                    // SSAA/MSAA 降采样 blit 到交换链后台缓冲 / Downsample blit to swap chain back buffer
                    g_pD3DDevice->CreateRenderTargetView(pBackBuffer, nullptr, &pSwapRTV);
                    if (!pSwapRTV) { if (pBackBuffer) pBackBuffer->Release(); pBackBuffer = nullptr; goto present_skip_blit; }
                    ID3D11ShaderResourceView *pBlitSRV = (g_msaaSamples > 1) ? g_pSSAAResolveSRV : g_pSSAASRV;

                    // 保存当前状态 / Save current state
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

                    // 清除交换链后台缓冲为透明黑，避免旧帧残留 / Clear swap chain back buffer to transparent black to prevent frame persistence
                    float clearBlack[4] = {0, 0, 0, 0};
                    g_pD3DContext->ClearRenderTargetView(pSwapRTV, clearBlack);

                    // 保存混合状态 / Save blend state
                    ID3D11BlendState *pOldBlend = nullptr;
                    FLOAT oldBlendFactor[4] = {1,1,1,1};
                    UINT oldSampleMask = 0;
                    g_pD3DContext->OMGetBlendState(&pOldBlend, oldBlendFactor, &oldSampleMask);

                    // 设置 blit 状态（不透明混合，完全覆盖后台缓冲）/ Set blit state (opaque blend, fully overwrite back buffer)
                    D3D11_VIEWPORT blitVP = {0, 0, (float)g_cachedVW, (float)g_cachedVH, 0, 1};
                    g_pD3DContext->RSSetViewports(1, &blitVP);
                    g_pD3DContext->OMSetRenderTargets(1, &pSwapRTV, nullptr);
                    g_pD3DContext->OMSetBlendState(nullptr, nullptr, 0xFFFFFFFF);  // 不透明覆盖 / opaque overwrite
                    g_pD3DContext->IASetInputLayout(g_pBlitLayout);
                    g_pD3DContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
                    UINT stride = 16, offset = 0;
                    g_pD3DContext->IASetVertexBuffers(0, 1, &g_pBlitVB, &stride, &offset);
                    g_pD3DContext->VSSetShader(g_pBlitVS, nullptr, 0);
                    g_pD3DContext->PSSetShader(g_pBlitPS, nullptr, 0);
                    g_pD3DContext->PSSetShaderResources(0, 1, &pBlitSRV);
                    g_pD3DContext->PSSetSamplers(0, 1, &g_pBlitSampler);
                    g_pD3DContext->Draw(4, 0);

                    // 恢复混合状态 / Restore blend state
                    g_pD3DContext->OMSetBlendState(pOldBlend, oldBlendFactor, oldSampleMask);
                    if (pOldBlend) pOldBlend->Release();

                    // 恢复状态 / Restore state
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
                    if (pSwapRTV) pSwapRTV->Release();
                }
                if (pBackBuffer) pBackBuffer->Release();
            }
            present_skip_blit:
            // 物理可视化调试：用 D2D1 叠加绘制速度/受力向量、漩涡、引力源等（在 blit 之后，避免被清掉）/ Physics debug overlay AFTER blit
            if (g_debugVelocity || g_debugForce || g_debugVortex || g_debugGravity || g_debugCollision || g_debugSpring) {
                g_pD2DDC->SetTarget(g_pD2DTargetBitmap);
                g_pD2DDC->BeginDraw();
                RenderPhysicsDebugD2D(dwTime);
                g_pD2DDC->EndDraw();
            }

            if (g_pSwapChain) {
                HRESULT presHr = g_pSwapChain->Present(1, 0);
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
            // 原生渲染失败但有内容要渲染，可能是设备丢失，触发恢复 / Native render failed but content exists, likely device lost, trigger recovery
            g_deviceLost.store(true);
            return;
        }
        // 原生渲染失败且无内容，回退到 D2D1（通常是空闲帧）/ Native render failed and no content, fallback to D2D1 (usually idle frame)
    }

    g_pD2DDC->SetTarget(g_pD2DTargetBitmap);
    g_pD2DDC->BeginDraw();
    g_pD2DDC->Clear(D2D1::ColorF(0, 0, 0, 0));

    // ===== 粒子渲染 =====
    if (!g_particles.empty()) {
        RenderParticles(dwTime);
        surfaceDirty = true;
    }

    // ===== 形状拖尾渲染 =====
    if (!g_trailShapes.empty()) {
        RenderTrailShapes(dwTime);
        surfaceDirty = true;
    }

    // ===== 运动模糊历史帧渲染 =====
    if (g_enableMotionBlur && tailVisible && havePath && g_trailHistory.size() > 1 && g_trailShape != 10) {
        RenderMotionBlur(widthMul, cols, g_fadeAlpha);
        surfaceDirty = true;
    }

    // ===== 拖尾（复用已计算的 smoothed 路径）=====
    if (tailVisible && havePath && g_trailShape != 4 && g_trailShape != 10) {
        UpdateColorBrushes(cols, smoothed[0], smoothed.back());
        float glowR = g_enableGlow ? (g_glowIntensity / 100.0f) * 7.0f : 0;
        float glowO = g_enableGlow ? (g_glowIntensity / 100.0f) * 0.28f : 0;
        bool useEnhancedGlow = g_enhancedGlow && glowR > 0.1f;
        float fa = g_fadeAlpha;
        const float SHADOW_DX = 1.5f, SHADOW_DY = 2.0f;

        if (g_trailShape == 1) {
            // ===== 类锥形圆链 =====
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
            std::vector<DotInfo> dots;
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
            // ===== 多边形带状 =====
            size_t sl = smoothed.size();
            std::vector<D2D1_POINT_2F> lo, ro, lc, rc, gl, gr, gl2, gr2;
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
                float taper = powf(1.0f - ratio, 1.3f);
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
            // FillMesh 不支持抗锯齿，临时切换到别名模式
            g_pD2DDC->SetAntialiasMode(D2D1_ANTIALIAS_MODE_ALIASED);
            // ===== 阴影层（mesh）=====
            if (g_enableTrailShadow && lo.size() >= 2) {
                std::vector<D2D1_POINT_2F> slo, sro;
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
            // ===== 增强发光层（mesh）=====
            if (useEnhancedGlow && gl2.size() >= 2) {
                CreateMeshFromQuadStrip(g_pD2DDC, &g_pGlow2Mesh, gl2, gr2);
                if (g_pGlow2Mesh) {
                    g_pSolidOuterBrush->SetColor(cols.solidOuter);
                    g_pSolidOuterBrush->SetOpacity(glowO * 0.3f * fa);
                    g_pD2DDC->FillMesh(g_pGlow2Mesh, g_pSolidOuterBrush);
                }
            }
            // ===== 外发光层（mesh）=====
            if (glowR > 0.1f && gl.size() >= 2) {
                CreateMeshFromQuadStrip(g_pD2DDC, &g_pGlowMesh, gl, gr);
                if (g_pGlowMesh) {
                    g_pSolidOuterBrush->SetColor(cols.solidOuter);
                    g_pSolidOuterBrush->SetOpacity(useEnhancedGlow ? glowO * 0.65f * fa : glowO * fa);
                    g_pD2DDC->FillMesh(g_pGlowMesh, g_pSolidOuterBrush);
                }
            }
            // ===== 外带 + 内带（mesh，含头部椭圆帽）=====
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
            // 恢复抗锯齿模式（头部高光用 FillEllipse，支持抗锯齿）
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

    // ===== 点击波纹渲染 =====
    if (g_enableClickEffect && !g_ripples.empty()) {
        RenderClickRipples(dwTime, cols, vX, vY);
        surfaceDirty = true;
    }

    HRESULT hr = g_pD2DDC->EndDraw();
    if (hr == D2DERR_RECREATE_TARGET || hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET) {
        Wh_Log(L"RenderFrame: device lost (0x%08X), scheduling recovery", hr);
        g_deviceLost.store(true);
        return;
    }
    if (g_pSwapChain) {
        HRESULT presHr = g_pSwapChain->Present(1, 0);  // 1 = 等待 vsync，避免帧率不稳定
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


// ===================== 粒子形状几何 =====================
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

// ===================== 设备丢失恢复 =====================
static void ReleaseAllRenderResources() {
    // 清理字符 bitmap 缓存 / Release char bitmap cache
    for (auto &kv : g_charBitmaps) { if (kv.second) kv.second->Release(); }
    g_charBitmaps.clear();
    g_cachedFontSize = 0;
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
    if (g_pBlitSampler) { g_pBlitSampler->Release(); g_pBlitSampler = nullptr; }
    if (g_pBlitVB) { g_pBlitVB->Release(); g_pBlitVB = nullptr; }
    if (g_pBlitLayout) { g_pBlitLayout->Release(); g_pBlitLayout = nullptr; }
    if (g_pBlitPS) { g_pBlitPS->Release(); g_pBlitPS = nullptr; }
    if (g_pBlitVS) { g_pBlitVS->Release(); g_pBlitVS = nullptr; }
    if (g_pCachedRTV) { g_pCachedRTV->Release(); g_pCachedRTV = nullptr; }
    if (g_pD2DTargetBitmap) { g_pD2DTargetBitmap->Release(); g_pD2DTargetBitmap = nullptr; }
    if (g_pSwapChain) { g_pSwapChain->Release(); g_pSwapChain = nullptr; }
    if (g_pDCompTarget && g_pDCompVisual) {
        g_pDCompTarget->SetRoot(nullptr);  // 清除根视觉对象，避免旧内容残留
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
    if (g_pD2DFactory) { g_pD2DFactory->Release(); g_pD2DFactory = nullptr; }
    if (g_dwFactory) { g_dwFactory->Release(); g_dwFactory = nullptr; }
    if (g_pDXGIDevice) { g_pDXGIDevice->Release(); g_pDXGIDevice = nullptr; }
    if (g_pD3DContext) { g_pD3DContext->Release(); g_pD3DContext = nullptr; }
    if (g_pD3DDevice) { g_pD3DDevice->Release(); g_pD3DDevice = nullptr; }
    g_cachedVW = 0;
    g_cachedVH = 0;
    g_trailHistory.clear();  // 清除运动模糊历史帧，避免恢复后渲染旧坐标
    g_history.clear();       // 清除拖尾历史
    g_particles.clear();     // 清除粒子
    g_ripples.clear();       // 清除点击波纹
    g_trailShapes.clear();   // 清除形状拖尾
    ReleaseNativeRendering();  // 释放 D3D11 原生渲染资源
}

static bool InitAllRenderResources() {
    // ---- D3D11 设备（硬件优先，WARP 回退）----
    D3D_FEATURE_LEVEL fl;
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
        Wh_Log(L"Recover: D3D11 device creation FAILED");
        return false;
    }
    Wh_Log(L"Recover: D3D11 device created (feature level %d)", fl);
    g_pD3DDevice->QueryInterface(__uuidof(IDXGIDevice), (void **)&g_pDXGIDevice);

    // ---- D2D1 设备 + 设备上下文 ----
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
        Wh_Log(L"Recover: D2D1 DC creation FAILED");
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

    // ---- DirectComposition ----
    if (g_pDXGIDevice) {
        DCompositionCreateDevice(g_pDXGIDevice, __uuidof(IDCompositionDevice), (void **)&g_pDCompDevice);
    }
    if (g_pDCompDevice) {
        g_pDCompDevice->CreateTargetForHwnd(g_overlayHwnd, TRUE, &g_pDCompTarget);
        g_pDCompDevice->CreateVisual(&g_pDCompVisual);
        if (g_pDCompTarget && g_pDCompVisual) g_pDCompTarget->SetRoot(g_pDCompVisual);
        Wh_Log(L"Recover: DComp device ready");
    }

    // ---- 交换链 ----
    RecreateSwapChain(g_virtW, g_virtH);

    // ---- D3D11 原生渲染初始化（v3）----
    InitNativeRendering();

    Wh_Log(L"Recover: resources reinitialized");
    return true;
}

// ===================== 覆盖层线程 =====================
static void UpdateVirtualScreenCache() {
    g_virtX = GetSystemMetrics(SM_XVIRTUALSCREEN);
    g_virtY = GetSystemMetrics(SM_YVIRTUALSCREEN);
    g_virtW = GetSystemMetrics(SM_CXVIRTUALSCREEN);
    g_virtH = GetSystemMetrics(SM_CYVIRTUALSCREEN);
    // 高度减 1 避免与 DWM 合成边界完全重合导致的渲染问题
    if (g_virtH > 1) g_virtH -= 1;
}
static LRESULT CALLBACK OverlayWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (msg == WM_POWERBROADCAST && wParam == PBT_APMRESUMEAUTOMATIC) {
        // 从睡眠/休眠唤醒后，GPU设备状态可能失效，主动触发资源重建
        // After resume from sleep/hibernate, GPU device state may be invalid, proactively trigger resource recreation
        Wh_Log(L"OverlayWndProc: wake from sleep, forcing device recovery");
        g_deviceLost.store(true);
        return TRUE;
    }
    if (msg == WM_DISPLAYCHANGE) {
        // 分辨率/显示器变化时更新缓存并调整窗口大小和位置 / Update cache and adjust window size/position on resolution/display change
        UpdateVirtualScreenCache();
        // 不用 SWP_SHOWWINDOW，避免空闲隐藏状态下被意外显示 / Don't use SWP_SHOWWINDOW to avoid accidental show in idle-hidden state
        SetWindowPos(hwnd, HWND_TOPMOST, g_virtX, g_virtY, g_virtW, g_virtH, SWP_NOACTIVATE | SWP_NOZORDER);
        return 0;
    }
    if (msg == WM_APP_SHOW_OVERLAY) {
        ShowWindow(hwnd, SW_SHOWNOACTIVATE);
        return 0;
    }
    if (msg == WM_APP_HIDE_OVERLAY) {
        ShowWindow(hwnd, SW_HIDE);
        return 0;
    }
    if (msg == WM_MOUSEACTIVATE) {
        // 阻止窗口被鼠标点击激活 / Prevent window activation on mouse click
        return MA_NOACTIVATEANDEAT;
    }
    if (msg == WM_NCHITTEST) {
        // 双保险：WS_EX_LAYERED|WS_EX_TRANSPARENT 已由内核做色键命中测试，这里再返回 HTTRANSPARENT
        // Belt-and-suspenders: layered color key handles kernel hit-testing, also return HTTRANSPARENT
        return HTTRANSPARENT;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}
DWORD WINAPI RenderThreadProc(LPVOID);  // 前向声明
DWORD WINAPI OverlayThreadProc(LPVOID) {
    CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
    SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

    // ---- 窗口（UI 线程创建，DComp 在渲染线程创建）----
    HINSTANCE hi = GetModuleHandle(NULL);
    const wchar_t CN[] = L"MouseTrailClass";
    WNDCLASS wc = {};
    wc.lpfnWndProc = OverlayWndProc;
    wc.hInstance = hi;
    wc.lpszClassName = CN;
    wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);  // 黑色背景用于 LWA_COLORKEY 色键透明 / black bg for color-key transparency
    RegisterClass(&wc);
    UpdateVirtualScreenCache();
    int sx = g_virtX, sy = g_virtY;
    int sw = g_virtW, sh = g_virtH;
    g_overlayHwnd = CreateWindowEx(
        WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE, CN,
        L"MouseTrailOverlay", WS_POPUP, sx, sy, sw, sh, NULL, NULL, hi, NULL);
    if (!g_overlayHwnd) {
        Wh_Log(L"OverlayThread: CreateWindowEx failed: %d", GetLastError());
        if (g_readyEvent) SetEvent(g_readyEvent);
        CoUninitialize();
        return 0;
    }
    // 黑色色键透明：重定向表面为黑色 -> 全部抠除透明且点击穿透（WS_EX_TRANSPARENT 需要 WS_EX_LAYERED 才生效）
    // DComp 视觉树由 DWM 独立合成，彩色拖尾不受色键影响；睡眠唤醒 DComp 丢失时回退表面也是透明的，不会黑屏
    // Black color key: redirection surface is black -> fully keyed transparent + click-through (WS_EX_TRANSPARENT requires WS_EX_LAYERED)
    // DComp visual tree is composited independently by DWM, colored trail unaffected; after sleep DComp loss, fallback surface is also transparent (no black screen)
    SetLayeredWindowAttributes(g_overlayHwnd, RGB(0, 0, 0), 255, LWA_COLORKEY);
    // 屏幕捕获排除在 LoadSettings 中根据颜色模式动态设置 / Screen capture exclusion dynamically set in LoadSettings based on color mode
    Wh_Log(L"OverlayThread: window created (%dx%d at %d,%d), initially hidden", sw, sh, sx, sy);
    // 不立即 ShowWindow，等渲染线程首次有内容绘制时再显示，避免渲染失败时全屏透明窗口残留 / Don't ShowWindow immediately, wait until render thread first draws content, avoids fullscreen transparent window residue on render failure

    // 通知渲染线程窗口已就绪 / Notify render thread window is ready
    if (g_readyEvent) SetEvent(g_readyEvent);

    // 启动渲染线程 / Start render thread
    g_renderExitEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    g_renderThread = CreateThread(NULL, 0, RenderThreadProc, NULL, 0, NULL);
    if (!g_renderThread) {
        Wh_Log(L"OverlayThread: CreateRenderThread failed: %d", GetLastError());
    }

    // ---- 消息循环（纯 UI 线程，不做渲染）----
    Wh_Log(L"OverlayThread: entering message loop");
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    // ---- 通知渲染线程退出并等待 ----
    Wh_Log(L"OverlayThread: message loop ended, stopping render thread");
    if (g_renderExitEvent) SetEvent(g_renderExitEvent);
    if (g_renderThread) {
        DWORD wr = WaitForSingleObject(g_renderThread, 5000);
        if (wr == WAIT_TIMEOUT) {
            // 渲染线程 5s 未退出（可能卡在 GPU 调用），再宽限 2s；仍不退出则强制终止，避免卸载永久挂起
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

    DestroyWindow(g_overlayHwnd);
    g_overlayHwnd = nullptr;
    UnregisterClass(CN, hi);
    CoUninitialize();
    return 0;
}

// ===================== 渲染线程（D3D11 + D2D + DComp，独立于 UI 线程）=====================
DWORD WINAPI RenderThreadProc(LPVOID) {
    CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
    SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    srand((unsigned)GetTickCount());

    // 等待 UI 线程创建窗口 / Wait for UI thread to create window
    if (g_readyEvent) WaitForSingleObject(g_readyEvent, INFINITE);
    if (!g_overlayHwnd) {
        Wh_Log(L"RenderThread: window not ready, exiting");
        CoUninitialize();
        return 0;
    }

    // ---- D3D11 硬件设备 ----
    UINT createFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT | D3D11_CREATE_DEVICE_SINGLETHREADED;
    D3D_FEATURE_LEVEL featureLevels[] = {
        D3D_FEATURE_LEVEL_11_1, D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1, D3D_FEATURE_LEVEL_10_0,
    };
    D3D_FEATURE_LEVEL chosenLevel;
    if (FAILED(D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, createFlags, featureLevels,
                                 ARRAYSIZE(featureLevels), D3D11_SDK_VERSION, &g_pD3DDevice, &chosenLevel,
                                 &g_pD3DContext))) {
        D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_WARP, nullptr, createFlags, featureLevels, ARRAYSIZE(featureLevels),
                          D3D11_SDK_VERSION, &g_pD3DDevice, &chosenLevel, &g_pD3DContext);
    }
    if (!g_pD3DDevice) {
        Wh_Log(L"RenderThread: D3D11 device creation FAILED");
        CoUninitialize();
        return 0;
    }
    Wh_Log(L"RenderThread: D3D11 device created (feature level %d)", chosenLevel);
    g_pD3DDevice->QueryInterface(__uuidof(IDXGIDevice), (void **)&g_pDXGIDevice);

    // ---- D2D1 设备 + 设备上下文 ----
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
        Wh_Log(L"RenderThread: D2D1 DC creation FAILED");
        CoUninitialize();
        return 0;
    }
    Wh_Log(L"RenderThread: D2D1 DC created");
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
    // mesh 每帧动态创建（ID2D1Mesh 只写一次），此处不预创建

    // ---- D3D11 原生渲染初始化（v3）----
    if (!InitNativeRendering()) {
        Wh_Log(L"RenderThread: Native D3D11 rendering init FAILED, falling back to D2D1");
    }

    // ---- DirectComposition（DComp API 线程安全，可在渲染线程创建）----
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

    // ---- 交换链 ----
    RecreateSwapChain(g_virtW, g_virtH);
    GetCursorPos(&g_lastPos);

    // ---- 后台采样线程（把 GDI 回读从渲染线程剥离，避免阻塞 vsync）----
    InitializeCriticalSection(&g_sampleCS);
    g_bgSampleExitEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    g_bgSampleThread = CreateThread(NULL, 0, BgSamplerThreadProc, NULL, 0, NULL);
    if (g_bgSampleThread)
        Wh_Log(L"RenderThread: background sampler thread started");
    else
        Wh_Log(L"RenderThread: failed to create background sampler thread");

    Wh_Log(L"RenderThread: entering render loop");
    // ---- 渲染循环（设备丢失恢复 + 空闲退避）----
    DWORD waitMs = 1;
    int frameCount = 0;
    while (WaitForSingleObject(g_renderExitEvent, waitMs) != WAIT_OBJECT_0) {
        // 主动检测设备丢失（每120帧检查一次，避免 Present 黑帧）
        // Proactive device lost check (every 120 frames, avoids black frames before Present fails)
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
        // 设备丢失恢复 / Device loss recovery
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
        // 显式 vsync 同步：Present(1,0) 已等待 vsync，DwmFlush 确保与 DWM 合成器同步
        bool isActive = !g_history.empty() || !g_particles.empty() || !g_ripples.empty() || !g_trailShapes.empty();
        if (isActive) {
            DwmFlush();
        }
        // 空闲退避：有拖尾/粒子/效果时 1ms，空闲时 16ms（~60fps 响应）
        waitMs = isActive ? 1 : 16;
    }

    // ---- 清理渲染资源（复用统一清理，保证与设备丢失恢复路径一致，含 SetRoot(nullptr)）---- / Cleanup render resources (reuse unified cleanup, consistent with device loss recovery path, including SetRoot(nullptr))
    Wh_Log(L"RenderThread: exiting, cleaning up");
    // 先停止后台采样线程，再释放渲染资源 / Stop background sampler thread first, then release render resources
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
    // 在创建COM的同一渲染线程停止音乐捕获，保证 CoInitializeEx/CoUninitialize 同线程配对
    MusicStopCapture();
    ReleaseAllRenderResources();
    CoUninitialize();
    return 0;
}

BOOL WhTool_ModInit() {
    Wh_Log(L"WhTool_ModInit: starting");
    LoadSettings();
    g_readyEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    g_threadHandle = CreateThread(NULL, 0, OverlayThreadProc, NULL, 0, NULL);
    if (!g_threadHandle) {
        Wh_Log(L"WhTool_ModInit: CreateThread failed: %d", GetLastError());
        if (g_readyEvent) { CloseHandle(g_readyEvent); g_readyEvent = NULL; }
        return FALSE;
    }
    Wh_Log(L"WhTool_ModInit: thread created");
    return TRUE;
}
void WhTool_ModUninit() {
    // 音乐捕获不在此（控制线程）停止：其COM对象由渲染线程创建，统一在 RenderThreadProc 退出时
    // 于同一渲染线程调用 MusicStopCapture 释放，避免跨线程释放 STA 对象。
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
    // 设置变更标记，由渲染线程消费（避免 UI 线程与渲染线程竞争）/ Settings change flag, consumed by render thread (avoids UI/render thread race)
    g_settingsDirty.store(true);
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
bool g_isToolModProcessLauncher;
HANDLE g_toolModProcessMutex;
void WINAPI EntryPoint_Hook() {
    Wh_Log(L">");
    ExitThread(0);
}
BOOL Wh_ModInit() {
    DWORD sessionId;
    if (ProcessIdToSessionId(GetCurrentProcessId(), &sessionId) && sessionId == 0) {
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
            Wh_Log(L"Tool mod already running (%s)", WH_MOD_ID);
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
            Wh_Log(L"GetModuleFileName failed");
            return;
    }
    Wh_Log(L"Tool mod host: %s", currentProcessPath);
    WCHAR
    commandLine[MAX_PATH + 2 + (sizeof(L" -tool-mod \"" WH_MOD_ID "\"") / sizeof(WCHAR)) - 1];
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
    STARTUPINFO si{
        .cb = sizeof(STARTUPINFO),
        .dwFlags = STARTF_FORCEOFFFEEDBACK,
    };
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
