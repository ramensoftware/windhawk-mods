// ==WindhawkMod==
// @id              mouse-trail
// @name            Mouse Trail
// @name:zh-CN      鼠标拖尾
// @description     Highly customizable cursor trail with native D3D11 rendering, 18 color modes, 10 trail shapes, 2.5D particle effects, particle system, click effects, and cursor color extraction. DirectComposition hardware acceleration, low idle CPU.
// @description:zh-CN 高度可定制的鼠标拖尾，原生 D3D11 渲染，18种颜色模式，10种拖尾形状，2.5D 立体效果，粒子系统，点击特效，光标取色。DirectComposition 硬件加速，闲置低 CPU。
// @version         3.3
// @author          MCheng404
// @github          https://github.com/MCheng404
// @license         MIT
// @include         windhawk.exe
// @compilerOptions -ld2d1 -ld3d11 -ldxgi -ldcomp -ldwmapi -lole32 -lgdi32 -lshell32 -ld3dcompiler
// ==/WindhawkMod==
// ==WindhawkModReadme==
/*
# Mouse Trail

A highly customizable mouse cursor trail with native D3D11 rendering, 18 color modes, 10 trail shapes, 2.5D particle effects, particle system, click effects, and cursor color extraction. DirectComposition hardware accelerated, runs as a dedicated process with low CPU usage when idle.

🎬 Demo

**Tapered Trail**
![Tapered Trail](https://i.imgur.com/6pJjvT7.gif)

**Shape Trail**
![Shape Trail](https://i.imgur.com/vtTCnuw.gif)

**Particle Effects**
![Particle Effects](https://i.imgur.com/vieAvyG.gif)

**Click Effects**
![Click Effects](https://i.imgur.com/bK1UC7g.gif)

---

### Rendering Architecture

* **Native D3D11 Rendering:** Custom HLSL vertex/pixel shaders with instanced particle rendering. No D2D1 dependency for core trail/particle/shape rendering.
* **2.5D Particle Effects:** Particles have per-instance z-depth with perspective projection and simple lighting. Particles scale with depth (near = larger, far = smaller) for a 3D feel.
* **DXGI Flip Swap Chain:** Premultiplied alpha for tear-free composition with DirectComposition.
* **Dual-Thread Design:** UI thread handles window/message pump, render thread handles all D3D11/DComp work — mouse input never blocks.
* **Device Loss Recovery:** Auto-rebuilds entire D3D/DComp stack on GPU TDR, driver update, or GPU switch.
* **Display Change Handling:** Auto-resizes and repositions overlay on `WM_DISPLAYCHANGE`.

### Trail Shapes (10 modes)

* **Tapered Ribbon:** Classic fading ribbon with glow, shadow, and head highlight
* **Dot Chain:** Beads along the path with configurable density
* **Function Curve:** Custom mathematical function deforms the trail (sine, damped, heartbeat, swirl, or custom formula)
* **Wave Curve:** Animated wave deformation
* **Shape Trail:** Spawns hearts, stars, hexagrams, or circles along the path with random velocity, configurable interval/size/count/lifetime
* **Double Line:** Two parallel trail ribbons
* **Dashed:** Segmented dashed trail
* **Spiral:** Spiral deformation along the path
* **Lightning:** Random jagged lightning effect
* **Feather:** Random spiky depth texture

### Color Modes (18 modes)

Single / 3-Color Gradient / Rainbow Flow / Warm Flow / Cool Flow / Neon Pulse / Velocity Color / Stripes / Fire / Aurora / Cursor Extract / Cursor Mix / Metallic Gold / Cyberpunk / Pastel / Hue Rotate / Dual Pulse / Sparkle

### Cursor Color Shift (6 modes)

Off / Complementary (180°) / Analogous (30°) / Triadic (120°) / Split Complement (150°) / Custom Angle

### Visual Effects

* **Bezier Smoothing:** Catmull-Rom spline interpolation for buttery-smooth curves
* **Motion Blur:** History frame overlay with decreasing opacity (1-5 strength)
* **Enhanced Glow:** Dual-layer halo (outer + inner) with independent toggles
* **Head Highlight + Trail Shadow:** Premium depth cues
* **Speed-Reactive Width:** Trail widens when moving fast

### Particle System

* Released from trail head/middle/tail/custom/random position
* Cursor attraction + repulsion force creating orbiting motion
* Shapes: circle, star, hexagram, or random mix
* Colors fade over lifetime; configurable density, interval (0 = per-frame), acceleration
* Click starburst particle burst

### Click Effects

* Starburst particle burst on click
* Expanding ripple ring on click
* Both toggleable independently

### Performance

* **Super Performance Mode:** Removes all caps (particle/shape limits, fast-path downgrade)
* **Adaptive Backoff:** Render thread waits 1ms when active, 16ms when idle
* **Game Detection:** Auto-hide in fullscreen DirectX games

### Function Trail Variables

Available variables: `t` (normalized 0=head 1=tail), `d` (distance from head in px), `time` (seconds). Functions: sin cos exp sqrt abs. Operators: + - * / ^.

Examples: `sin(d * 0.15) * 8`, `sin(d * 0.25) * exp(0 - t * 2.5) * 10`.

### Color Format

Hex RGB, e.g. `FF0000`=red, `00FF00`=green, `0000FF`=blue, `FFD700`=gold.

### Credits

Developed by [MCheng404](https://github.com/MCheng404).
Original overlay/smear architecture inspired by [TheatriChris](https://github.com/TheatriChris)'s cursor-motion-blur mod (MIT licensed).

---

# 鼠标拖尾

高度可定制的鼠标拖尾特效，原生 D3D11 渲染，18种颜色模式，10种拖尾形状，2.5D 立体效果，粒子系统，点击特效，光标取色。DirectComposition 硬件加速，独立进程运行，闲置时低 CPU 占用。

---

### 渲染架构

* **原生 D3D11 渲染：** 自定义 HLSL 顶点/像素着色器，粒子实例化渲染。核心拖尾/粒子/形状渲染不依赖 D2D1。
* **2.5D 粒子效果：** 粒子具有实例级 z 深度 + 透视投影 + 简单光照。粒子随深度缩放（近大远小），营造 3D 空间感。
* **DXGI 翻转交换链：** 预乘 alpha，与 DirectComposition 无撕裂合成。
* **双线程设计：** UI 线程处理窗口/消息泵，渲染线程处理所有 D3D11/DComp 工作——鼠标输入永不阻塞。
* **设备丢失恢复：** GPU TDR、驱动更新或显卡切换时自动重建整个 D3D/DComp 栈。
* **显示变化处理：** `WM_DISPLAYCHANGE` 时自动调整覆盖层大小和位置。

### 拖尾形状（10种）

* **锥形飘带：** 经典渐隐飘带，带发光、阴影和头部高光
* **圆点链：** 沿路径排列的圆点，密度可调
* **函数曲线：** 自定义数学函数变形轨迹（正弦、阻尼、心跳、漩涡或自定义公式）
* **波浪曲线：** 动态波浪变形
* **形状拖尾：** 沿路径生成爱心、五角星、六芒星或圆形，随机速度，间隔/大小/数量/存活时间可调
* **双线拖尾：** 两条平行拖尾带
* **虚线拖尾：** 分段虚线效果
* **螺旋拖尾：** 沿路径螺旋变形
* **闪电拖尾：** 随机锯齿闪电效果
* **羽毛拖尾：** 随机毛刺深度纹理

### 颜色模式（18种）

单色 / 三色渐变 / 彩虹流动 / 暖色调流动 / 冷色调流动 / 霓虹脉冲 / 速度变色 / 流动条纹 / 火焰 / 极光 / 光标取色 / 光标混色 / 金属金 / 赛博朋克 / 粉彩 / 色相旋转 / 双色脉冲 / 星光闪烁

### 取色偏移（6种模式）

关闭 / 互补色(180°) / 类似色(30°) / 三角色(120°) / 分裂互补(150°) / 自定义角度

### 视觉特效

* **贝塞尔平滑：** Catmull-Rom 样条插值，曲线如丝般顺滑
* **运动模糊：** 历史帧叠加，透明度递减（1-5强度）
* **增强发光：** 双层光晕（外晕+内辉），独立开关
* **头部高光 + 拖尾阴影：** 高级质感深度提示
* **速度响应宽度：** 快速移动时拖尾变宽

### 粒子系统

* 从拖尾开头/中间/结尾/自定义/随机位置释放
* 光标吸引 + 排斥力，形成绕飞运动
* 形状：圆形、五角星、六芒星或随机混合
* 颜色随生命周期渐变；密度、间隔（0=每帧）、加速度可调
* 点击星爆粒子迸发

### 点击特效

* 点击时星爆粒子迸发
* 点击时扩散波纹环
* 两者可独立开关

### 性能

* **超级性能模式：** 解除所有上限（粒子/形状限制、快速路径降级）
* **自适应退避：** 渲染线程活跃时等待1ms，闲置时16ms
* **游戏检测：** 全屏 DirectX 游戏时自动隐藏

### 函数轨迹变量

自定义公式中可使用：`t`（归一化位置 0=头 1=尾）、`d`（距头部像素距离）、`time`（秒）。支持函数：sin cos exp sqrt abs，运算符：+ - * / ^。

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
  $description: Minimum mouse speed to activate the trail (pixels/frame).
  $description:zh-CN: 激活拖尾所需的最低鼠标移动速度（像素/帧）。
- stop_velocity: 10
  $name: Stop Velocity
  $name:zh-CN: 停止速度
  $description: Speed below which the trail fades out. Must be lower than Trigger Velocity.
  $description:zh-CN: 低于此速度时拖尾开始淡出。必须低于触发速度。
- tail_length: 10
  $name: Trail Length
  $name:zh-CN: 拖尾长度
  $description: Number of history points in the trail. Range 2-200.
  $description:zh-CN: 拖尾保留的历史点数。范围 2-200。
- tail_offset_x: 6
  $name: Tail Offset X
  $name:zh-CN: 拖尾 X 偏移
  $description: Horizontal offset of trail origin from cursor (pixels).
  $description:zh-CN: 拖尾起点相对光标的水平偏移（像素）。
- tail_offset_y: 10
  $name: Tail Offset Y
  $name:zh-CN: 拖尾 Y 偏移
  $description: Vertical offset of trail origin from cursor (pixels).
  $description:zh-CN: 拖尾起点相对光标的垂直偏移（像素）。
- trail_delay: 0
  $name: Trail Delay
  $name:zh-CN: 拖尾延迟
  $description: How much the trail head lags behind cursor (0-10, 0=off).
  $description:zh-CN: 拖尾头部滞后光标的程度（0-10，0=关闭）。

# ===== 拖尾外观 =====
- trail_shape: tapered
  $name: Trail Shape
  $name:zh-CN: 拖尾形状
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
- fadeout_mode: soft
  $name: Fadeout Mode
  $name:zh-CN: 淡出模式
  $description: How the trail disappears when mouse stops.
  $description:zh-CN: 鼠标停止后拖尾的消失方式。
  $options:
  - hard: Hard Cut
  - accelerate: Accelerated Shrink
  - soft: Soft Fade
  $options:zh-CN:
  - hard: 硬截断
  - accelerate: 加速收缩
  - soft: 柔和淡出
- enable_smooth_gradient: true
  $name: Smooth Gradient
  $name:zh-CN: 平滑渐变
  $description: Smoothly interpolate between gradient colors. Turn off for stepped color bands.
  $description:zh-CN: 在渐变色之间平滑插值过渡，关闭则为分段阶梯色带。
- enable_speed_response: true
  $name: Dynamic Width
  $name:zh-CN: 动态宽度
  $description: Trail width increases with speed and acceleration.
  $description:zh-CN: 移动速度和加速度影响拖尾宽度，快速移动时更宽。

# ===== 颜色设置 =====
- color_mode: single
  $name: Color Mode
  $name:zh-CN: 颜色模式
  $options:
  - single: Single Color
  - gradient: 3-Color Gradient
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
  $options:zh-CN:
  - single: 单色
  - gradient: 三色渐变
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
- custom_color: "00BFFF"
  $name: Custom Color
  $name:zh-CN: 自定义颜色
  $description: Primary color for single/neon/hue-rotate modes. Hex RGB.
  $description:zh-CN: 单色/霓虹/色相旋转模式的主色，十六进制 RGB。
- gradient_colors: "FF6B35,00BFFF,FFD700"
  $name: Gradient Colors
  $name:zh-CN: 渐变颜色
  $description: 3 colors for gradient/stripes/dual-pulse modes, comma-separated hex RGB.
  $description:zh-CN: 渐变/条纹/双色脉冲模式的3个颜色，逗号分隔的十六进制RGB。
- enable_cursor_color_shift: true
  $name: Auto Color Shift
  $name:zh-CN: 取色自动偏移
  $description: Auto hue shift for cursor extraction modes to ensure visibility.
  $description:zh-CN: 光标取色模式下自动偏移色相，确保拖尾在任何背景上都醒目。
- color_shift_mode: complementary
  $name: Color Shift Mode
  $name:zh-CN: 取色偏移模式
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
- color_shift_angle: 180
  $name: Custom Shift Angle
  $name:zh-CN: 自定义偏移角度
  $description: Hue shift angle for custom mode. 0-360 degrees.
  $description:zh-CN: 自定义模式下的色相偏移角度，0-360度。

# ===== 发光效果 =====
- enable_glow: true
  $name: Micro Glow
  $name:zh-CN: 微发光
  $description: Soft outer glow around the trail.
  $description:zh-CN: 拖尾外圈柔和发光效果。
- glow_intensity: 40
  $name: Glow Intensity
  $name:zh-CN: 发光强度
  $description: Glow radius and brightness (0-100).
  $description:zh-CN: 发光范围和亮度（0-100）。
- enhanced_glow: true
  $name: Enhanced Glow
  $name:zh-CN: 增强发光
  $description: Dual-layer halo (outer + inner) for softer glow.
  $description:zh-CN: 双层光晕（外晕+内辉），发光更柔和自然。
- edge_softness: 50
  $name: Edge Softness
  $name:zh-CN: 边缘柔和度
  $description: Trail edge anti-aliasing width (0=hard edge, 100=very soft).
  $description:zh-CN: 拖尾边缘抗锯齿宽度（0=硬边，100=极柔和）。
- enable_head_highlight: true
  $name: Head Highlight
  $name:zh-CN: 头部高光
  $description: Bright center dot at trail head for premium look.
  $description:zh-CN: 拖尾头部添加明亮中心点，提升质感。
- enable_trail_shadow: true
  $name: Trail Shadow
  $name:zh-CN: 拖尾阴影
  $description: Dark underlay shadow for depth perception.
  $description:zh-CN: 拖尾底层绘制暗色阴影，增加立体感。
- enable_adaptive_contrast: false
  $name: Adaptive Contrast
  $name:zh-CN: 自适应对比度
  $description: Sample background brightness and adapt trail color luminance automatically, with a subtle soft edge for visibility on any background.
  $description:zh-CN: 采样背景亮度自动微调拖尾颜色明暗，亮背景下压暗、暗背景提亮，并加一圈极细柔和边缘，确保任何背景下都清晰可见。

# ===== 形状拖尾设置 =====
- shape_type: heart
  $name: Shape Type
  $name:zh-CN: 拖尾形状类型
  $description: Shape used for Shape Trail mode.
  $description:zh-CN: 形状拖尾模式下使用的形状。
  $options:
  - heart: Heart
  - star: Star
  - hexagram: Hexagram
  - circle: Circle
  - random: Random Mix
  $options:zh-CN:
  - heart: 爱心
  - star: 五角星
  - hexagram: 六芒星
  - circle: 圆形
  - random: 随机混合
- shape_interval: 30
  $name: Shape Interval
  $name:zh-CN: 形状生成间隔
  $description: Distance between shapes in pixels (10-100).
  $description:zh-CN: 形状之间的间隔距离（像素，10-100）。
- shape_random_offset: true
  $name: Random Position Offset
  $name:zh-CN: 随机位置微调
  $description: Add small random offset to each shape position.
  $description:zh-CN: 为每个形状位置添加小范围随机偏移，更自然。
- shape_count: 1
  $name: Shape Count
  $name:zh-CN: 每次生成数量
  $description: Number of shapes per spawn (1-5).
  $description:zh-CN: 每次生成的形状数量（1-5）。
- shape_size: 12
  $name: Shape Size
  $name:zh-CN: 形状大小
  $description: Base size of shapes in pixels (5-30).
  $description:zh-CN: 形状的基础大小（像素，5-30）。
- shape_lifetime: 800
  $name: Shape Lifetime
  $name:zh-CN: 形状存活时间
  $description: How long each shape lasts (ms, 200-2000).
  $description:zh-CN: 每个形状的存活时间（毫秒，200-2000）。

# ===== 特殊形状参数 =====
- dots_multiplier: 2
  $name: Dot Chain Density
  $name:zh-CN: 圆点链密度
  $description: Dot count multiplier (1-5), higher = more smaller dots.
  $description:zh-CN: 圆点链的小球数量倍率（1-5），越大小球越多越密。
- dot_chain_size: 100
  $name: Dot Chain Size
  $name:zh-CN: 圆点大小
  $description: Size multiplier for dot chain trail (50-300).
  $description:zh-CN: 圆点链拖尾的大小倍数（50-300）。
- function_preset: sine
  $name: Function Preset
  $name:zh-CN: 函数预设
  $description: Preset formula for Function Curve mode.
  $description:zh-CN: 函数曲线模式的预设公式。
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
- custom_function: "sin(d * 0.15) * 8"
  $name: Custom Function
  $name:zh-CN: 自定义函数公式
  $description: "Variables: t(0-1) d(distance) time(sec); Functions: sin cos exp sqrt abs."
  $description:zh-CN: "变量 t(0-1) d(距离) time(秒)；函数 sin cos exp sqrt abs。"
- wave_amplitude: 8
  $name: Wave Amplitude
  $name:zh-CN: 波浪幅度
  $description: Wave amplitude in pixels.
  $description:zh-CN: 波浪曲线的振幅（像素）。
- wave_frequency: 15
  $name: Wave Frequency
  $name:zh-CN: 波浪频率
  $description: Wave frequency (3-60, higher = denser waves).
  $description:zh-CN: 波浪曲线的频率（3-60，越大波浪越密）。

# ===== 粒子系统 =====
- particle_mode: fadeout
  $name: Particle Mode
  $name:zh-CN: 粒子模式
  $options:
  - off: Off
  - fadeout: On Fadeout
  - always: Always (except idle)
  $options:zh-CN:
  - off: 关闭
  - fadeout: 淡出时
  - always: 始终（静止除外）
- particle_origin: tail
  $name: Particle Origin
  $name:zh-CN: 粒子释放位置
  $description: Where on the trail particles are released.
  $description:zh-CN: 粒子从拖尾的哪个位置释放。
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
- particle_origin_ratio: 80
  $name: Custom Origin Ratio
  $name:zh-CN: 自定义释放比例
  $description: Position along trail (0=head, 100=tail). Custom origin only.
  $description:zh-CN: 沿拖尾的位置比例（0=开头，100=结尾）。仅自定义位置生效。
- particle_density: 3
  $name: Particle Density
  $name:zh-CN: 粒子密度
  $description: Number of particles per release (1-10).
  $description:zh-CN: 每次释放的粒子数量（1-10）。
- particle_size_multiplier: 100
  $name: Particle Size
  $name:zh-CN: 粒子大小
  $description: Size multiplier for particles (50-300).
  $description:zh-CN: 粒子大小倍数（50-300）。
- enable_particle_glow: true
  $name: Particle Glow
  $name:zh-CN: 粒子发光
  $description: Soft outer halo around each particle.
  $description:zh-CN: 每个粒子外圈绘制柔和光晕。
- particle_glow_intensity: 40
  $name: Particle Glow Intensity
  $name:zh-CN: 粒子发光强度
  $description: Particle glow size and brightness (0-100).
  $description:zh-CN: 粒子发光范围和亮度（0-100）。
- particle_interval: 50
  $name: Particle Interval
  $name:zh-CN: 粒子释放间隔
  $description: Minimum interval between releases (ms, 0-2000). 0 = every frame.
  $description:zh-CN: 粒子释放的最小时间间隔（毫秒，0-2000），0=每帧生成。
- particle_acceleration: true
  $name: Acceleration Effect
  $name:zh-CN: 加速度影响
  $description: Particle initial velocity affected by mouse acceleration.
  $description:zh-CN: 粒子初速度受鼠标加速度影响，速度变化越大飞散越快。
- particle_shape: random
  $name: Particle Shape
  $name:zh-CN: 粒子形状
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
- enable_particle_spin: true
  $name: Particle Spin
  $name:zh-CN: 粒子自旋转
  $description: Enable random self-rotation for particles.
  $description:zh-CN: 启用粒子随机自旋转效果。
- particle_spin_speed: 30
  $name: Spin Speed
  $name:zh-CN: 自旋速度
  $description: Base rotation speed for particles (0-100). Each particle gets random variation ±50%. Very high speed can make asymmetric shapes appear round due to motion blur.
  $description:zh-CN: 粒子基础自旋转速度（0-100），每个粒子有 ±50% 的随机差异。速度过高会导致三角形/星形等非对称形状因运动模糊看起来像圆形。
- enable_particle_interaction: true
  $name: Particle Interaction
  $name:zh-CN: 粒子间相互作用
  $description: Enable repulsion force between particles.
  $description:zh-CN: 启用粒子之间的排斥力，使粒子不会重叠。
- particle_repel_distance: 25
  $name: Repel Distance
  $name:zh-CN: 排斥距离
  $description: Distance (pixels) within which particles repel each other.
  $description:zh-CN: 粒子之间产生排斥力的距离（像素）。
- particle_inter_repel_force: 15
  $name: Inter Repel Force
  $name:zh-CN: 粒子间排斥强度
  $description: Strength of inter-particle repulsion force.
  $description:zh-CN: 粒子之间排斥力的强度。
- particle_attraction: 40
  $name: Particle Attraction
  $name:zh-CN: 粒子吸附强度
  $description: How strongly particles are attracted to cursor (0-100).
  $description:zh-CN: 粒子被吸向光标的强度（0-100）。
- enable_particle_repel: true
  $name: Cursor Repulsion
  $name:zh-CN: 光标排斥力
  $description: Particles near cursor are repelled, creating orbiting motion.
  $description:zh-CN: 粒子靠近光标时被排斥弹开，形成绕飞效果。
- particle_repel_radius: 25
  $name: Repulsion Radius
  $name:zh-CN: 排斥范围
  $description: Repulsion radius around cursor (pixels, 5-100).
  $description:zh-CN: 光标周围的排斥半径（像素，5-100）。
- particle_repel_force: 30
  $name: Repulsion Force
  $name:zh-CN: 排斥强度
  $description: Repulsion and perturbation strength (0-100).
  $description:zh-CN: 排斥力和随机扰动的强度（0-100）。

# ===== 点击效果 =====
- enable_click_starburst: true
  $name: Click Starburst
  $name:zh-CN: 点击星爆
  $description: Particle burst from cursor on mouse click.
  $description:zh-CN: 点击时从光标位置迸发粒子。
- starburst_count: 8
  $name: Starburst Count
  $name:zh-CN: 星爆粒子数
  $description: Number of particles per click burst (4-20).
  $description:zh-CN: 每次点击迸发的粒子数量（4-20）。
- enable_click_effect: true
  $name: Click Ripple
  $name:zh-CN: 点击波纹
  $description: Expanding ripple ring on mouse click.
  $description:zh-CN: 点击时产生扩散波纹环。
- click_max_radius: 40
  $name: Ripple Max Radius
  $name:zh-CN: 波纹最大半径
  $description: Maximum ripple radius (pixels, 1-500).
  $description:zh-CN: 点击波纹扩散的最大半径（像素，1-500）。
- click_duration: 300
  $name: Ripple Duration
  $name:zh-CN: 波纹持续时间
  $description: Ripple duration (ms, 1-3000).
  $description:zh-CN: 点击波纹从出现到消失的时长（毫秒，1-3000）。

# ===== 高级效果 =====
- enable_bezier_smooth: true
  $name: Bezier Smoothing
  $name:zh-CN: 贝塞尔曲线平滑
  $description: Catmull-Rom spline interpolation for smoother curves.
  $description:zh-CN: 使用 Catmull-Rom 样条插值，拖尾曲线更顺滑。
- enable_motion_blur: false
  $name: Motion Blur
  $name:zh-CN: 运动模糊
  $description: Overlay previous frames with decreasing opacity for motion blur.
  $description:zh-CN: 以递减透明度叠加历史帧，制造运动模糊效果。
- motion_blur_strength: 3
  $name: Motion Blur Strength
  $name:zh-CN: 运动模糊强度
  $description: Number of history frames to overlay (1-5).
  $description:zh-CN: 叠加的历史帧数（1-5）。
- enable_25d_effect: true
  $name: 2.5D Depth Effect
  $name:zh-CN: 2.5D 立体效果
  $description: Add per-vertex depth with perspective projection for a 2.5D look.
  $description:zh-CN: 为顶点添加深度坐标和透视投影，营造 2.5D 立体感。
- perspective_strength: 15
  $name: Perspective Strength
  $name:zh-CN: 透视强度
  $description: Strength of 2.5D perspective scaling (0-50). Higher = more depth.
  $description:zh-CN: 2.5D 透视缩放强度（0-50），数值越大立体感越强。

# ===== 性能设置 =====
- super_performance_mode: false
  $name: Super Performance Mode
  $name:zh-CN: 超级性能模式
  $description: Remove all performance limits. High-end PCs only.
  $description:zh-CN: 解除所有性能上限。仅在高性能电脑上启用。
*/
// ==/WindhawkModSettings==
#include <windows.h>
#include <d2d1_1.h>
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
#include <atomic>
#include <algorithm>

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
static D2D1_COLOR_F ParseHexColor(PCWSTR hex, D2D1_COLOR_F fallback) {
    if (!hex || !*hex)
        return fallback;
    // Validate: must be exactly 6 hex digits
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
static void ParseGradientColors(PCWSTR input, D2D1_COLOR_F out[3]) {
    if (!input || !*input)
        return;
    WCHAR buf[256];
    wcsncpy_s(buf, input, 255);
    buf[255] = 0;
    int idx = 0;
    WCHAR *ctx = nullptr;
    WCHAR *tok = wcstok_s(buf, L",", &ctx);
    while (tok && idx < 3) {
        while (*tok == L' ' || *tok == L'\t')
            tok++;
        D2D1_COLOR_F c = ParseHexColor(tok, out[idx]);
        out[idx++] = c;
        tok = wcstok_s(nullptr, L",", &ctx);
    }
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
ID2D1Device *g_pD2DDevice = nullptr;
ID2D1DeviceContext *g_pD2DDC = nullptr;
IDXGISwapChain1 *g_pSwapChain = nullptr;
ID2D1Bitmap1 *g_pD2DTargetBitmap = nullptr;
ID3D11RenderTargetView *g_pCachedRTV = nullptr;  // 缓存的后台缓冲 RTV（随交换链重建，避免每帧创建）
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
int g_colorMode = 0;
D2D1_COLOR_F g_customColor = {0.0f, 0.75f, 1.0f, 1.0f};
D2D1_COLOR_F g_gradColors[3] = {{1.0f, 0.42f, 0.21f, 1.0f}, {0.0f, 0.75f, 1.0f, 1.0f}, {1.0f, 0.84f, 0.0f, 1.0f}};
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
int g_particleShape = 0;  // 0=random, 1=circle, 2=star, 3=hexagram
bool g_enableParticleSpin = true;  // 粒子自旋转
int g_particleSpinSpeed = 30;  // 粒子自旋速度（0-100）
bool g_enableParticleInteraction = true;  // 粒子间相互作用
int g_interParticleRepelDistance = 25;  // 粒子间排斥距离（像素）
float g_interParticleRepelForce = 0.15f;  // 粒子间排斥力强度
ID2D1PathGeometry *g_pStarGeom = nullptr;
ID2D1PathGeometry *g_pHexagramGeom = nullptr;
ID2D1PathGeometry *g_pHeartGeom = nullptr;

// ===== 形状拖尾 =====
struct TrailShape {
    float x, y;
    float vx, vy;
    float size;
    int shapeType;  // 0=heart 1=star 2=hexagram 3=circle
    DWORD startTime;
    float lifetime;
    D2D1_COLOR_F color;
};
std::vector<TrailShape> g_trailShapes;
float g_lastShapeX = 0, g_lastShapeY = 0;
bool g_hasLastShapePos = false;
int g_shapeType = 0;          // 0=heart 1=star 2=hexagram 3=circle 4=random
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
// 运动模糊历史帧缓冲区
struct TrailFrame {
    std::vector<D2D1_POINT_2F> path;
    DWORD time;
};
std::vector<TrailFrame> g_trailHistory;
std::atomic<bool> g_settingsDirty{false};  // 设置变更标记，渲染线程中消费
std::atomic<bool> g_deviceLost{false};      // 设备丢失标记，渲染循环中重建
DWORD g_lastParticleTime = 0;
float g_prevVelocity = 0;
bool g_enableClickStarburst = true;
int g_starburstCount = 8;
bool g_enableClickEffect = true;
int g_clickMaxRadius = 40, g_clickDuration = 300;

// ===================== 粒子系统 =====================
struct Particle {
    float x, y, vx, vy, size;
    float z;            // 3D 深度（生成时随机，避免每帧闪烁）
    float rotation;     // 当前旋转角度（弧度）
    float spinSpeed;    // 自旋转速度（弧度/帧）
    DWORD startTime;
    int lifetime;
    D2D1_COLOR_F color;
    D2D1_COLOR_F endColor;
    int shapeType;
    float colorOffset[3]; // 随机偏色（RGB，约 ±20/255）
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
        case 1:
            headOuter = g_gradColors[0];
            tailOuter = g_gradColors[2];
            headInner = LighterColor(g_gradColors[0]);
            tailInner = LighterColor(g_gradColors[2]);
            break;
        case 2: {
            float h = fmodf(t * 50, 360);
            headOuter = HSVtoRGB(h, .85f, 1);
            tailOuter = HSVtoRGB(fmodf(h + 180, 360), .85f, 1);
            headInner = HSVtoRGB(h, .4f, 1);
            tailInner = HSVtoRGB(fmodf(h + 180, 360), .4f, 1);
            break;
        }
        case 3: {
            float h = fmodf(t * 30, 50) - 10;  // -10~40: 红-橙-黄
            headOuter = HSVtoRGB(h, .9f, 1);
            tailOuter = HSVtoRGB(fmodf(h + 30, 360), .85f, .95f);
            headInner = HSVtoRGB(h, .45f, 1);
            tailInner = HSVtoRGB(fmodf(h + 30, 360), .4f, .95f);
            break;
        }
        case 4: {
            float h = 170 + fmodf(t * 25, 110);  // 170~280: 青-蓝-紫
            headOuter = HSVtoRGB(h, .8f, 1);
            tailOuter = HSVtoRGB(fmodf(h + 60, 360), .75f, .95f);
            headInner = HSVtoRGB(h, .4f, 1);
            tailInner = HSVtoRGB(fmodf(h + 60, 360), .35f, .95f);
            break;
        }
        case 5: {
            float p = .5f + .5f * sinf(t * 3.5f);
            D2D1_COLOR_F c = g_customColor;
            headOuter = D2D1::ColorF(c.r * (0.4f + p * 0.6f), c.g * (0.4f + p * 0.6f), c.b * (0.4f + p * 0.6f), 1);
            tailOuter = D2D1::ColorF(c.r * p * 0.3f, c.g * p * 0.3f, c.b * p * 0.3f, 1);
            headInner = LighterColor(headOuter, .5f);
            tailInner = LighterColor(tailOuter, .5f);
            break;
        }
        case 6: {
            float sn = fminf(powf(velocity / 80.0f, 0.7f), 1.0f);
            float h = 220 - sn * 220;  // 蓝→绿→黄→红
            headOuter = HSVtoRGB(h, .85f, 1);
            tailOuter = HSVtoRGB(fmodf(h + 50, 360), .7f, .85f);
            headInner = HSVtoRGB(h, .4f, 1);
            tailInner = HSVtoRGB(fmodf(h + 50, 360), .3f, .9f);
            break;
        }
        case 7:
            headOuter = g_gradColors[0];
            tailOuter = g_gradColors[1];
            headInner = LighterColor(g_gradColors[0]);
            tailInner = LighterColor(g_gradColors[1]);
            break;
        case 8: {
            float f = .8f + .2f * sinf(t * 12) * sinf(t * 6.7f) + .1f * sinf(t * 23);
            headOuter = D2D1::ColorF(1.0f * f, 0.75f * f, 0.15f, 1);
            tailOuter = D2D1::ColorF(0.8f, 0.15f, 0.0f, 1);
            headInner = D2D1::ColorF(1.0f, 0.95f, 0.6f, 1);
            tailInner = D2D1::ColorF(0.95f, 0.4f, 0.05f, 1);
            break;
        }
        case 9: {
            float h1 = 130 + 40 * sinf(t * 0.6f) + 20 * sinf(t * 1.3f);
            float h2 = 260 + 50 * sinf(t * 0.5f + 1.5f) + 15 * sinf(t * 1.1f);
            headOuter = HSVtoRGB(h1, .7f, .95f);
            tailOuter = HSVtoRGB(h2, .7f, .9f);
            headInner = HSVtoRGB(180, .35f, 1);
            tailInner = HSVtoRGB(fmodf(h2 + 40, 360), .3f, .95f);
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
            D2D1_COLOR_F mixedTail = LerpColor(g_cursorExtractedColor, g_gradColors[2], 0.5f);
            headOuter = mixed;
            tailOuter = mixedTail;
            headInner = LighterColor(mixed, 0.55f);
            tailInner = LighterColor(mixedTail, 0.55f);
            break;
        }
        case 12: { // 金属金色
            float shine = 0.65f + 0.35f * sinf(t * 2.5f) + 0.1f * sinf(t * 7.0f);
            headOuter = D2D1::ColorF(1.0f * shine, 0.82f * shine, 0.1f * shine, 1);
            tailOuter = D2D1::ColorF(0.55f, 0.38f, 0.05f, 1);
            headInner = D2D1::ColorF(1.0f, 0.92f, 0.55f, 1);
            tailInner = D2D1::ColorF(0.85f, 0.65f, 0.25f, 1);
            break;
        }
        case 13: { // 赛博朋克（紫青渐变）
            float pulse = 0.5f + 0.5f * sinf(t * 4.0f);
            float pulse2 = 0.5f + 0.5f * sinf(t * 4.0f + 1.5f);
            headOuter = HSVtoRGB(290 + pulse * 30, 0.85f, 1.0f);
            tailOuter = HSVtoRGB(175 - pulse2 * 15, 0.85f, 1.0f);
            headInner = HSVtoRGB(310, 0.45f, 1.0f);
            tailInner = HSVtoRGB(165, 0.45f, 1.0f);
            break;
        }
        case 14: { // 粉彩
            float h = fmodf(t * 20, 360);
            headOuter = HSVtoRGB(h, 0.3f, 1.0f);
            tailOuter = HSVtoRGB(fmodf(h + 80, 360), 0.3f, 1.0f);
            headInner = HSVtoRGB(h, 0.15f, 1.0f);
            tailInner = HSVtoRGB(fmodf(h + 80, 360), 0.15f, 1.0f);
            break;
        }
        case 15: { // 色相旋转（基于自定义颜色）
            float h, s, v;
            RGBtoHSV(g_customColor, h, s, v);
            float h1 = fmodf(h + t * 45, 360);
            float h2 = fmodf(h1 + 140, 360);
            headOuter = HSVtoRGB(h1, s, v);
            tailOuter = HSVtoRGB(h2, s, v);
            headInner = HSVtoRGB(h1, s * 0.45f, v);
            tailInner = HSVtoRGB(h2, s * 0.45f, v);
            break;
        }
        case 16: { // 双色脉冲
            float pulse = 0.5f + 0.5f * sinf(t * 3.0f);
            float smoothPulse = pulse * pulse * (3 - 2 * pulse);  // smoothstep
            headOuter = LerpColor(g_gradColors[0], g_gradColors[2], smoothPulse);
            tailOuter = LerpColor(g_gradColors[2], g_gradColors[0], smoothPulse);
            headInner = LighterColor(headOuter, 0.45f);
            tailInner = LighterColor(tailOuter, 0.45f);
            break;
        }
        case 17: { // 随机闪烁
            float sparkle = (rand() % 100) / 100.0f;
            float sparkle2 = (rand() % 100) / 100.0f;
            float baseH = fmodf(t * 40, 360);
            headOuter = HSVtoRGB(baseH, 0.75f, 0.5f + sparkle * 0.5f);
            tailOuter = HSVtoRGB(fmodf(baseH + 100, 360), 0.75f, 0.4f + sparkle2 * 0.4f);
            headInner = HSVtoRGB(baseH, 0.35f, 1.0f);
            tailInner = HSVtoRGB(fmodf(baseH + 100, 360), 0.35f, 1.0f);
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
            // 三色渐变：color1 → color2 → color3
            float t2 = ratio * 2.0f;
            D2D1_COLOR_F co, ci;
            if (t2 < 1.0f) {
                co = LerpColor(g_gradColors[0], g_gradColors[1], t2);
                ci = LerpColor(LighterColor(g_gradColors[0]), LighterColor(g_gradColors[1]), t2);
            } else {
                co = LerpColor(g_gradColors[1], g_gradColors[2], t2 - 1.0f);
                ci = LerpColor(LighterColor(g_gradColors[1]), LighterColor(g_gradColors[2]), t2 - 1.0f);
            }
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

// ---- HLSL 着色器（v3.1：渐变采样 + 2.5D 透视）----
static const char* g_vsShader = R"(
cbuffer ConstantBuffer : register(b0) {
    float2 screenSize;
    float perspective;   // 透视强度（0=纯2D，0.5=中等）
    float2 lightDir;     // 光照方向
    float4 gradient[16]; // 渐变停止点（rgba）
    int gradientCount;
    float bgLuminance;   // 背景亮度
    float adaptiveFlag;  // 自适应对比度开关
    float smoothFlag;  // 平滑渐变开关（1=插值平滑，0=阶梯色）
    float edgeSoftness;  // 边缘柔和度（0=硬边，1=极柔和）
};

struct VS_INPUT {
    float3 pos : POSITION;   // x, y, z(深度)
    float4 color : COLOR;
    float u : TEXCOORD0;     // 沿路径比例 0~1
    float v : TEXCOORD1;     // 垂直路径方向 -1~1
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
    // 2.5D 透视：根据深度 z 相对于屏幕中心缩放
    float scale = 1.0 + input.pos.z * perspective;
    float2 center = screenSize * 0.5;
    float2 screenPos = center + (input.pos.xy - center) * scale;
    // 屏幕坐标 → 裁剪空间
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
    float2 lightDir;
    float4 gradient[16];
    int gradientCount;
    float bgLuminance;
    float adaptiveFlag;
    float smoothFlag;  // 平滑渐变开关（1=插值平滑，0=阶梯色）
    float edgeSoftness;  // 边缘柔和度（0=硬边，1=极柔和）
};

struct PS_INPUT {
    float4 pos : SV_POSITION;
    float4 color : COLOR;
    float u : TEXCOORD0;
    float v : TEXCOORD1;
    float depth : TEXCOORD2;
};

float4 PSMain(PS_INPUT input) : SV_TARGET {
    // 从渐变中采样颜色（GPU 硬件插值，无断层）
    float4 gradColor = input.color;
    float3 tint = float3(1, 1, 1);  // 单色模式下顶点颜色即最终颜色，不做调制
    if (gradientCount > 1) {
        float t = input.u * (gradientCount - 1);
        int idx = (int)t;
        float frac = t - idx;
        if (idx >= gradientCount - 1) {
            gradColor = gradient[gradientCount - 1];
        } else if (smoothFlag > 0.5) {
            gradColor = lerp(gradient[idx], gradient[idx + 1], frac);  // 平滑插值
        } else {
            gradColor = gradient[idx];  // 阶梯色（关闭平滑渐变时分段切换）
        }
        gradColor.a *= input.color.a;
        tint = input.color.rgb;  // 渐变模式下顶点RGB作为亮度/色调调制：阴影(0,0,0)变黑、高亮(>1)变亮、主体(1,1,1)不变
    }
    // 软边缘：|v| 接近 1 时 alpha 衰减，消除三角形带硬边锯齿；edgeSoftness 控制衰减宽度
    float inner = 1.0 - edgeSoftness * 0.5;  // 0=硬边(inner=1.0)，1=极柔和(inner=0.5)
    float edgeFade = smoothstep(1.0, inner, abs(input.v));
    // 圆柱光照：中心(v=0)最亮，向边缘逐渐变暗，模拟管状体积感
    float tube = 0.88 + 0.12 * (1.0 - abs(input.v));
    // 2.5D 光照：深度越大越亮（模拟高光）
    float light = 1.0 + input.depth * 0.3;
    float3 rgb = gradColor.rgb * tint * tube * light;
    if (adaptiveFlag > 0.5) {
        float adapt = (bgLuminance - 0.5) * 0.35;  // 压暗/提亮强度减半，避免过度
        if (adapt > 0.0) { rgb *= (1.0 - adapt); }
        else { rgb = lerp(rgb, float3(1,1,1), -adapt); }
    }
    return float4(rgb, gradColor.a * edgeFade);
}
)";

// 粒子实例着色器（带 2.5D 透视）
static const char* g_particleVS = R"(
cbuffer ConstantBuffer : register(b0) {
    float2 screenSize;
    float perspective;
    float2 lightDir;
    float4 gradient[16];
    int gradientCount;
    float bgLuminance;
    float adaptiveFlag;
    float smoothFlag;  // 平滑渐变开关（1=插值平滑，0=阶梯色）
    float edgeSoftness;  // 边缘柔和度（0=硬边，1=极柔和）
};

struct VS_INPUT {
    float2 quadPos : POSITION;
    float3 instancePos : TEXCOORD0; // x, y, z
    float4 instanceColor : TEXCOORD1;
    float instanceSize : TEXCOORD2;
    float instanceShape : TEXCOORD3;
    float instanceRot : TEXCOORD4;
};

struct VS_OUTPUT {
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
    float4 color : COLOR;
    float depth : TEXCOORD1;
    float shape : TEXCOORD2;
};

VS_OUTPUT VSMain(VS_INPUT input) {
    VS_OUTPUT output;
    // 2.5D 透视
    float scale = 1.0 + input.instancePos.z * perspective;
    // 自旋转：只旋转顶点位置，不旋转 uv
    // 如果同时旋转顶点位置和 uv，旋转效果会相互抵消，屏幕上看起来没转
    float rotAngle = input.instanceRot;
    float cosR = cos(rotAngle);
    float sinR = sin(rotAngle);
    float2 rotatedPos = float2(
        input.quadPos.x * cosR - input.quadPos.y * sinR,
        input.quadPos.x * sinR + input.quadPos.y * cosR
    );
    float2 worldPos = input.instancePos.xy + rotatedPos * input.instanceSize * scale;
    float2 ndc = float2(
        (worldPos.x / screenSize.x) * 2.0 - 1.0,
        1.0 - (worldPos.y / screenSize.y) * 2.0
    );
    output.pos = float4(ndc, 0.0, 1.0);
    // uv 不旋转，使用原始 quadPos，这样形状遮罩会随着四边形的旋转而在屏幕上显示为旋转
    output.uv = input.quadPos * 0.5 + 0.5;
    output.color = input.instanceColor;
    output.depth = input.instancePos.z;
    output.shape = input.instanceShape;
    return output;
}
)";

static const char* g_particlePS = R"(
cbuffer ConstantBuffer : register(b0) {
    float2 screenSize;
    float perspective;
    float2 lightDir;
    float4 gradient[16];
    int gradientCount;
    float bgLuminance;
    float adaptiveFlag;
    float smoothFlag;  // 平滑渐变开关（1=插值平滑，0=阶梯色）
    float edgeSoftness;  // 边缘柔和度（0=硬边，1=极柔和）
};

struct PS_INPUT {
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
    float4 color : COLOR;
    float depth : TEXCOORD1;
    float shape : TEXCOORD2;
};

// 形状 signed distance：返回值 >0 在形状内部，<0 在外部，绝对值近似到边界的距离（UV 空间）
float shapeSDF(float2 uv, float shapeType) {
    float2 p = uv - 0.5;
    float r = length(p);
    float a = atan2(p.y, p.x);
    if (shapeType < 1.5) {
        return 0.5 - r;                                    // circle
    } else if (shapeType < 2.5) {
        float starR = 0.5 * (0.4 + 0.6 * abs(cos(a * 2.5)));
        return starR - r;                                  // star
    } else if (shapeType < 3.5) {
        float hexR = 0.5 * (0.5 + 0.5 * abs(cos(a * 3.0)));
        return hexR - r;                                   // hexagram
    } else if (shapeType < 4.5) {
        // heart: (x²+y²-1)³ - x²y³ < 0 在内部
        float2 hp = (uv - 0.5) * 2.0;
        hp.y = -hp.y;
        float heart = pow(hp.x*hp.x + hp.y*hp.y - 1.0, 3.0) - hp.x*hp.x * hp.y*hp.y*hp.y;
        return -heart * 0.12;                              // heart（缩放使边缘过渡合适）
    } else if (shapeType < 5.5) {
        return 0.5 - (abs(p.x) + abs(p.y));                // diamond
    } else if (shapeType < 6.5) {
        // triangle（等边，顶点在上）
        float2 tp = (uv - 0.5) * 2.0;
        tp.y = -tp.y;
        float d1 = tp.y + 0.5;
        float d2 = 1.0 - tp.y - 1.732 * tp.x;
        float d3 = 1.0 - tp.y + 1.732 * tp.x;
        return min(d1, min(d2, d3)) * 0.35;                // triangle
    } else if (shapeType < 7.5) {
        float flowerR = 0.5 * (0.6 + 0.4 * cos(a * 5.0));
        return flowerR - r;                                // flower
    } else if (shapeType < 8.5) {
        float pentR = 0.5 * 0.85 / cos(fmod(a + 3.14159, 2.0 * 3.14159 / 5.0) - 3.14159 / 5.0);
        return pentR - r;                                  // pentagon
    } else {
        float hexR = 0.5 * 0.87 / cos(fmod(a + 3.14159, 3.14159 / 3.0) - 3.14159 / 6.0);
        return hexR - r;                                   // hexagon
    }
}

float4 PSMain(PS_INPUT input) : SV_TARGET {
    float2 center = input.uv - 0.5;
    float dist = length(center);
    // signed distance 软边缘：所有形状统一平滑抗锯齿
    float sdf = shapeSDF(input.uv, input.shape);
    float edge = 0.025;  // 软边缘宽度（UV 空间）
    float mask = smoothstep(-edge, edge, sdf);
    if (mask <= 0.001) discard;
    // 径向光照：中心亮、边缘暗，模拟球体/体积感
    float radial = 1.0 - dist * 0.3;
    radial = max(radial, 0.6);
    // 2.5D 深度光照
    float light = 1.0 + input.depth * 0.25;
    float3 rgb = input.color.rgb * light * radial;
    // 自适应对比度：根据背景亮度微调粒子明度
    if (adaptiveFlag > 0.5) {
        float adapt = (bgLuminance - 0.5) * 0.35;
        if (adapt > 0.0) { rgb *= (1.0 - adapt); }
        else { rgb = lerp(rgb, float3(1,1,1), -adapt); }
    }
    float alpha = input.color.a * mask;
    return float4(rgb, alpha);
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
    float x, y, z;     // 位置 + 深度
    float r, g, b, a; // 颜色
    float size;       // 大小（半径）
    float shapeType;  // 0=circle, 1=star, 2=hexagram
    float rotation;   // 旋转角度（弧度）
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
ID3D11RasterizerState* g_pRasterState = nullptr;

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

    // 编译着色器
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

        // 输入布局：通用顶点（位置+深度+颜色+u坐标+v坐标）
        D3D11_INPUT_ELEMENT_DESC layoutDesc[] = {
            {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
            {"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
            {"TEXCOORD", 0, DXGI_FORMAT_R32_FLOAT, 0, 28, D3D11_INPUT_PER_VERTEX_DATA, 0},
            {"TEXCOORD", 1, DXGI_FORMAT_R32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0},
        };
        if (FAILED(g_pD3DDevice->CreateInputLayout(layoutDesc, 4, vsBlob->GetBufferPointer(),
                                                   vsBlob->GetBufferSize(), &g_pNativeLayout)))
            break;

        // 输入布局：粒子实例（位置+深度+颜色+大小+形状+旋转）
        D3D11_INPUT_ELEMENT_DESC particleLayout[] = {
            {"POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
            {"TEXCOORD", 0, DXGI_FORMAT_R32G32B32_FLOAT, 1, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1},
            {"TEXCOORD", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},
            {"TEXCOORD", 2, DXGI_FORMAT_R32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},
            {"TEXCOORD", 3, DXGI_FORMAT_R32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},
            {"TEXCOORD", 4, DXGI_FORMAT_R32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},
        };
        if (FAILED(g_pD3DDevice->CreateInputLayout(particleLayout, 6, pvsBlob->GetBufferPointer(),
                                                   pvsBlob->GetBufferSize(), &g_pParticleLayout)))
            break;

        // 常量缓冲（包含屏幕尺寸、透视、渐变停止点）
        D3D11_BUFFER_DESC cbDesc = {};
        cbDesc.ByteWidth = 320;  // 对齐到 16 字节
        cbDesc.Usage = D3D11_USAGE_DYNAMIC;
        cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        if (FAILED(g_pD3DDevice->CreateBuffer(&cbDesc, nullptr, &g_pConstantBuffer))) break;

        // 拖尾带顶点缓冲（动态，最大 4096 顶点）
        D3D11_BUFFER_DESC vbDesc = {};
        vbDesc.ByteWidth = sizeof(VertexPosColor) * 4096;
        vbDesc.Usage = D3D11_USAGE_DYNAMIC;
        vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        vbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        if (FAILED(g_pD3DDevice->CreateBuffer(&vbDesc, nullptr, &g_pTrailVB))) break;

        // 粒子四边形（两个三角形组成的正方形）
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

        // 粒子实例缓冲（动态，最大 2000 实例）
        D3D11_BUFFER_DESC instDesc = {};
        instDesc.ByteWidth = sizeof(ParticleInstance) * 4000;  // 发光层+正常层各最多2000
        instDesc.Usage = D3D11_USAGE_DYNAMIC;
        instDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        instDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        if (FAILED(g_pD3DDevice->CreateBuffer(&instDesc, nullptr, &g_pParticleInstanceBuf))) break;

        // Alpha 混合状态
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

        // 光栅化状态
        D3D11_RASTERIZER_DESC rastDesc = {};
        rastDesc.FillMode = D3D11_FILL_SOLID;
        rastDesc.CullMode = D3D11_CULL_NONE;
        rastDesc.DepthClipEnable = FALSE;
        if (FAILED(g_pD3DDevice->CreateRasterizerState(&rastDesc, &g_pRasterState))) break;

        ok = true;
    } while (false);

    // 编译 blob 无论成败都释放
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
    return true;
}

static void ReleaseNativeRendering() {
    if (g_pNativeVS) { g_pNativeVS->Release(); g_pNativeVS = nullptr; }
    if (g_pNativePS) { g_pNativePS->Release(); g_pNativePS = nullptr; }
    if (g_pParticleVS) { g_pParticleVS->Release(); g_pParticleVS = nullptr; }
    if (g_pParticlePS) { g_pParticlePS->Release(); g_pParticlePS = nullptr; }
    if (g_pNativeLayout) { g_pNativeLayout->Release(); g_pNativeLayout = nullptr; }
    if (g_pParticleLayout) { g_pParticleLayout->Release(); g_pParticleLayout = nullptr; }
    if (g_pConstantBuffer) { g_pConstantBuffer->Release(); g_pConstantBuffer = nullptr; }
    if (g_pTrailVB) { g_pTrailVB->Release(); g_pTrailVB = nullptr; }
    if (g_pParticleQuadVB) { g_pParticleQuadVB->Release(); g_pParticleQuadVB = nullptr; }
    if (g_pParticleInstanceBuf) { g_pParticleInstanceBuf->Release(); g_pParticleInstanceBuf = nullptr; }
    if (g_pAlphaBlend) { g_pAlphaBlend->Release(); g_pAlphaBlend = nullptr; }
    if (g_pRasterState) { g_pRasterState->Release(); g_pRasterState = nullptr; }
}

static void UpdateConstantBuffer(int width, int height, const GradData* cols = nullptr) {
    if (!g_pConstantBuffer || !g_pD3DContext) return;
    D3D11_MAPPED_SUBRESOURCE mapped;
    if (SUCCEEDED(g_pD3DContext->Map(g_pConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped))) {
        float* data = (float*)mapped.pData;
        // screenSize (offset 0, bytes 0-7)
        data[0] = (float)width;
        data[1] = (float)height;
        // perspective (offset 2, bytes 8-11)
        data[2] = g_enable25DEffect ? (float)g_perspectiveStrength / 100.0f : 0.0f;
        // lightDir (offset 4, bytes 16-23) — float2 不能跨越 16 字节边界
        data[4] = 0.5f;
        data[5] = -0.5f;
        // gradient[16] (offset 8, bytes 32-287)
        int gradCount = 0;
        if (cols) {
            for (int i = 0; i < GRAD_STOPS; i++) {
                data[8 + i * 4 + 0] = cols->outer[i].color.r;
                data[8 + i * 4 + 1] = cols->outer[i].color.g;
                data[8 + i * 4 + 2] = cols->outer[i].color.b;
                data[8 + i * 4 + 3] = cols->outer[i].color.a;
            }
            gradCount = GRAD_STOPS;
        }
        // gradientCount (offset 72, bytes 288-291) — int 类型，用整数写入
        ((int*)data)[72] = gradCount;
        // bgLuminance (offset 73) 和 adaptiveFlag (offset 74)
        data[73] = g_bgLuminance;
        data[74] = g_adaptiveContrast ? 1.0f : 0.0f;
        data[75] = g_enableSmoothGradient ? 1.0f : 0.0f;
        // edgeSoftness (offset 76)：0=硬边，1=极柔和
        data[76] = g_edgeSoftness / 100.0f;
        g_pD3DContext->Unmap(g_pConstantBuffer, 0);
    }
}

// 粒子 Instanced Rendering（v3 原生渲染，含发光层；一次 Map 写两层）
static void NativeRenderParticles(int screenW, int screenH) {
    if (!g_pParticleVS || !g_pParticlePS || !g_pParticleInstanceBuf || g_particles.empty()) return;

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
    for (auto &p : g_particles) {
        float progress = (float)(now - p.startTime) / p.lifetime;
        if (progress < 0 || progress >= 1) continue;
        float lifeAlpha = (1.0f - progress);
        D2D1_COLOR_F pc = D2D1::ColorF(
            p.color.r + (p.endColor.r - p.color.r) * progress + p.colorOffset[0],
            p.color.g + (p.endColor.g - p.color.g) * progress + p.colorOffset[1],
            p.color.b + (p.endColor.b - p.color.b) * progress + p.colorOffset[2], 1.0f);
        float sizeScale = sinf(progress * 3.14159f) * 0.7f + 0.3f;
        float baseSize = p.size * 2.0f * (g_particleSizeMultiplier / 100.0f) * sizeScale;

        // 发光层
        if (g_enableParticleGlow && glowCount < MAX_PER_LAYER) {
            ParticleInstance& gl = instances[glowCount++];
            gl.x = p.x; gl.y = p.y; gl.z = p.z;
            gl.r = pc.r * glowColorMul; gl.g = pc.g * glowColorMul; gl.b = pc.b * glowColorMul;
            gl.a = lifeAlpha * 0.8f * glowAlpha;
            gl.size = baseSize * glowSize;
            gl.shapeType = (float)p.shapeType;
            gl.rotation = p.rotation;
        }
        // 正常层
        if (normalCount < MAX_PER_LAYER) {
            ParticleInstance& nm = instances[MAX_PER_LAYER + normalCount++];
            nm.x = p.x; nm.y = p.y; nm.z = p.z;
            nm.r = pc.r; nm.g = pc.g; nm.b = pc.b;
            nm.a = lifeAlpha * 0.8f;
            nm.size = baseSize;
            nm.shapeType = (float)p.shapeType;
            nm.rotation = p.rotation;
        }
    }
    g_pD3DContext->Unmap(g_pParticleInstanceBuf, 0);
    if (glowCount == 0 && normalCount == 0) return;

    // 设置渲染状态
    UpdateConstantBuffer(screenW, screenH);
    g_pD3DContext->IASetInputLayout(g_pParticleLayout);
    g_pD3DContext->VSSetShader(g_pParticleVS, nullptr, 0);
    g_pD3DContext->PSSetShader(g_pParticlePS, nullptr, 0);
    g_pD3DContext->VSSetConstantBuffers(0, 1, &g_pConstantBuffer);
    g_pD3DContext->PSSetConstantBuffers(0, 1, &g_pConstantBuffer);
    g_pD3DContext->RSSetState(g_pRasterState);
    g_pD3DContext->OMSetBlendState(g_pAlphaBlend, nullptr, 0xFFFFFFFF);

    UINT stride = 2 * sizeof(float);
    UINT vbOffset = 0;
    g_pD3DContext->IASetVertexBuffers(0, 1, &g_pParticleQuadVB, &stride, &vbOffset);
    stride = sizeof(ParticleInstance);
    g_pD3DContext->IASetVertexBuffers(1, 1, &g_pParticleInstanceBuf, &stride, &vbOffset);
    g_pD3DContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // 发光层（实例起始 0）
    if (glowCount > 0) g_pD3DContext->DrawInstanced(6, glowCount, 0, 0);
    // 正常层（实例起始 MAX_PER_LAYER）
    if (normalCount > 0) g_pD3DContext->DrawInstanced(6, normalCount, 0, MAX_PER_LAYER);
}

// 拖尾带顶点缓冲渲染（v3 原生渲染）
static void NativeRenderTrail(const std::vector<D2D1_POINT_2F>& smoothed, float widthMul,
                              const GradData& cols, float fadeAlpha, int screenW, int screenH, DWORD dwTime = 0) {
    if (!g_pNativeVS || !g_pNativePS || !g_pTrailVB || smoothed.size() < 2) return;

    size_t sl = smoothed.size();

    // 预计算每个点的法线和宽度
    std::vector<float> nx(sl), ny(sl), widths(sl);
    for (size_t i = 0; i < sl; ++i) {
        float ddx, ddy;
        if (i == 0) { ddx = smoothed[1].x - smoothed[0].x; ddy = smoothed[1].y - smoothed[0].y; }
        else if (i == sl - 1) { ddx = smoothed[i].x - smoothed[i-1].x; ddy = smoothed[i].y - smoothed[i-1].y; }
        else { ddx = smoothed[i+1].x - smoothed[i-1].x; ddy = smoothed[i+1].y - smoothed[i-1].y; }
        float ln = sqrtf(ddx*ddx + ddy*ddy);
        if (ln > 0) { ddx /= ln; ddy /= ln; } else { ddx = 1; ddy = 0; }
        nx[i] = -ddy; ny[i] = ddx;
        float ratio = (float)i / (sl - 1);
        float taper = powf(1.0f - ratio, 1.3f);
        widths[i] = (i == sl - 1) ? 0.5f : 10.0f * taper * widthMul;
    }

    // 设置渲染状态
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

    // 一次 Map 批量写入所有层（消除多次 vector 分配 + Map/Unmap）
    D3D11_MAPPED_SUBRESOURCE mapped;
    if (FAILED(g_pD3DContext->Map(g_pTrailVB, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped))) return;
    VertexPosColor* dst = (VertexPosColor*)mapped.pData;
    int vtxOffset = 0;
    const int MAX_VTX = 4096;

    struct Layer { int offset, count; };
    Layer layers[8];
    int layerCount = 0;

    // 写入一层带，直接写入 mapped 内存，返回顶点数（0=缓冲溢出）
    auto writeBand = [&](float widthScale, float r, float g, float b, float aMul,
                         bool headOnly, float offX, float offY) -> int {
        int count = (int)sl * 2;
        if (vtxOffset + count > MAX_VTX) return 0;
        VertexPosColor* p = dst + vtxOffset;
        for (size_t i = 0; i < sl; ++i) {
            float ratio = (float)i / (sl - 1);
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

    auto pushLayer = [&](int c) { if (c && layerCount < 8) layers[layerCount++] = {vtxOffset - c, c}; };

    // 1. 拖尾阴影（右下偏移深色投影）
    if (g_enableTrailShadow)
        pushLayer(writeBand(1.0f, 0,0,0, 0.18f, false, 1.5f, 2.0f));
    // 2. 自适应柔和边缘
    if (g_adaptiveContrast) {
        float edgeV = g_bgLuminance > 0.5f ? 0.08f : 1.6f;
        pushLayer(writeBand(1.18f, edgeV,edgeV,edgeV, 0.18f, false, 0,0));
    }
    // 3. 外发光三层（宽淡→中→窄亮）
    if (g_enableGlow) {
        float gi = g_glowIntensity / 100.0f;
        pushLayer(writeBand(2.6f + gi*1.4f, 1,1,1, 0.04f+gi*0.05f, false, 0,0));
        pushLayer(writeBand(1.7f + gi*0.7f, 1,1,1, 0.09f+gi*0.09f, false, 0,0));
        if (g_enhancedGlow)
            pushLayer(writeBand(1.25f + gi*0.35f, 1.15f,1.15f,1.15f, 0.14f+gi*0.10f, false, 0,0));
    }
    // 4. 主体带
    pushLayer(writeBand(1.0f, 1,1,1, 1.0f, false, 0,0));
    // 5. 头部高亮（只在头部显示）
    if (g_enableHeadHighlight)
        pushLayer(writeBand(0.35f, 1.25f,1.25f,1.25f, 0.75f, true, 0,0));

    g_pD3DContext->Unmap(g_pTrailVB, 0);

    // 绘制所有层（每层独立 Draw，三角形带不跨层连接）
    for (int i = 0; i < layerCount; ++i)
        g_pD3DContext->Draw(layers[i].count, layers[i].offset);
}

// 预计算形状局部顶点（消除每帧 cos/sin/pow 重复计算）
struct ShapeVertsCache { const float* verts; int count; };
static ShapeVertsCache GetShapeVerts(int shapeType) {
    static float circleVerts[72 * 2];   // 24段 × 3顶点 × 2坐标
    static float starVerts[30 * 2];     // 5 × 6顶点 × 2
    static float hexVerts[6 * 2];       // 6顶点 × 2
    static float heartVerts[72 * 2];    // 24段 × 3顶点 × 2
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
        hexVerts[0]=0; hexVerts[1]=-1;
        hexVerts[2]=-0.866f; hexVerts[3]=0.5f;
        hexVerts[4]=0.866f; hexVerts[5]=0.5f;
        hexVerts[6]=0; hexVerts[7]=1;
        hexVerts[8]=-0.866f; hexVerts[9]=-0.5f;
        hexVerts[10]=0.866f; hexVerts[11]=-0.5f;
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
    }
    switch (shapeType) {
        case 0: return {heartVerts, 72};
        case 1: return {starVerts, 30};
        case 2: return {hexVerts, 6};
        case 3: return {circleVerts, 72};
        default: return {circleVerts, 72};
    }
}

// 形状拖尾原生渲染（v3）：直接生成世界坐标顶点，一次绘制
static void NativeRenderTrailShapes(int screenW, int screenH, DWORD dwTime) {
    if (g_trailShapes.empty() || !g_pTrailVB) return;

    std::vector<VertexPosColor> verts;
    verts.reserve(g_trailShapes.size() * 32);

    for (auto &s : g_trailShapes) {
        float progress = (float)(dwTime - s.startTime) / s.lifetime;
        if (progress < 0 || progress >= 1) continue;
        float lifeAlpha = (1.0f - progress) * 0.7f;
        float size = s.size * (1.0f + progress * 0.3f);
        float cr = s.color.r, cg = s.color.g, cb = s.color.b;

        ShapeVertsCache sv = GetShapeVerts(s.shapeType);
        for (int i = 0; i < sv.count; i++) {
            float lx = sv.verts[i * 2] * size;
            float ly = sv.verts[i * 2 + 1] * size;
            verts.push_back({s.x + lx, s.y + ly, 0.0f, cr, cg, cb, lifeAlpha, 0.5f});
        }
    }

    if (verts.empty()) return;
    if (verts.size() > 4096) verts.resize(4096); // 限制最大顶点数

    // 更新顶点缓冲
    D3D11_MAPPED_SUBRESOURCE mapped;
    if (FAILED(g_pD3DContext->Map(g_pTrailVB, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped))) return;
    memcpy(mapped.pData, verts.data(), verts.size() * sizeof(VertexPosColor));
    g_pD3DContext->Unmap(g_pTrailVB, 0);

    // 设置渲染状态
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
    g_pD3DContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    g_pD3DContext->Draw((UINT)verts.size(), 0);
}

// 原生渲染主函数（v3）：使用 D3D11 直接渲染，替代 D2D1
static void NativeRenderRipples(int screenW, int screenH, DWORD dwTime, const GradData& cols, int vX, int vY) {
    if (g_ripples.empty() || !g_pTrailVB) return;

    // 设置渲染状态
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
    g_pD3DContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

    const int segments = 48;
    const int maxVerts = 4096;
    int totalVerts = 0;

    for (auto &r : g_ripples) {
        float progress = (float)(dwTime - r.startTime) / g_clickDuration;
        if (progress < 0 || progress >= 1) continue;
        float radius = g_clickMaxRadius * progress;
        float alpha = (1.0f - progress) * 0.6f;
        float ringWidth = 3.0f + progress * 2.0f;
        D2D1_COLOR_F rc = cols.outer[GRAD_STOPS / 2].color;

        // 检查顶点缓冲是否足够
        int ringVerts = (segments + 1) * 2;
        if (totalVerts + ringVerts > maxVerts) break;

        // 减去虚拟屏幕原点偏移
        float px = r.pos.x - vX;
        float py = r.pos.y - vY;

        // 1. 外发光层（宽、半透明）
        {
            VertexPosColor glowVerts[100];
            int idx = 0;
            float glowWidth = ringWidth * 2.5f;
            for (int i = 0; i <= segments; i++) {
                float a = (i / (float)segments) * 6.28318f;
                float cosA = cosf(a), sinA = sinf(a);
                glowVerts[idx++] = {px + cosA * (radius + glowWidth), py + sinA * (radius + glowWidth), 0,
                                    rc.r, rc.g, rc.b, alpha * 0.2f, 0.5f};
                glowVerts[idx++] = {px + cosA * radius, py + sinA * radius, 0,
                                    rc.r, rc.g, rc.b, 0, 0.5f};
            }
            D3D11_MAPPED_SUBRESOURCE mapped;
            if (SUCCEEDED(g_pD3DContext->Map(g_pTrailVB, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped))) {
                memcpy(mapped.pData, glowVerts, ringVerts * sizeof(VertexPosColor));
                g_pD3DContext->Unmap(g_pTrailVB, 0);
                g_pD3DContext->Draw(ringVerts, 0);
            }
        }

        // 2. 主环（外圈亮，内圈半透明填充）
        {
            VertexPosColor ringVertsArr[100];
            int idx = 0;
            for (int i = 0; i <= segments; i++) {
                float a = (i / (float)segments) * 6.28318f;
                float cosA = cosf(a), sinA = sinf(a);
                ringVertsArr[idx++] = {px + cosA * (radius + ringWidth), py + sinA * (radius + ringWidth), 0,
                                       rc.r, rc.g, rc.b, alpha, 0.5f};
                ringVertsArr[idx++] = {px + cosA * radius, py + sinA * radius, 0,
                                       rc.r, rc.g, rc.b, alpha * 0.3f, 0.5f};
            }
            D3D11_MAPPED_SUBRESOURCE mapped;
            if (SUCCEEDED(g_pD3DContext->Map(g_pTrailVB, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped))) {
                memcpy(mapped.pData, ringVertsArr, ringVerts * sizeof(VertexPosColor));
                g_pD3DContext->Unmap(g_pTrailVB, 0);
                g_pD3DContext->Draw(ringVerts, 0);
            }
        }

        // 3. 内圈高亮（更亮更窄）
        {
            VertexPosColor innerVerts[100];
            int idx = 0;
            float innerWidth = ringWidth * 0.4f;
            for (int i = 0; i <= segments; i++) {
                float a = (i / (float)segments) * 6.28318f;
                float cosA = cosf(a), sinA = sinf(a);
                innerVerts[idx++] = {px + cosA * (radius + innerWidth), py + sinA * (radius + innerWidth), 0,
                                     1.0f, 1.0f, 1.0f, alpha * 0.5f, 0.5f};
                innerVerts[idx++] = {px + cosA * (radius - innerWidth * 0.5f), py + sinA * (radius - innerWidth * 0.5f), 0,
                                     1.0f, 1.0f, 1.0f, 0, 0.5f};
            }
            D3D11_MAPPED_SUBRESOURCE mapped;
            if (SUCCEEDED(g_pD3DContext->Map(g_pTrailVB, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped))) {
                memcpy(mapped.pData, innerVerts, ringVerts * sizeof(VertexPosColor));
                g_pD3DContext->Unmap(g_pTrailVB, 0);
                g_pD3DContext->Draw(ringVerts, 0);
            }
        }
        totalVerts += ringVerts * 3;
    }
}

static D2D1_POINT_2F GetPointOnPath(const std::vector<D2D1_POINT_2F> &path, float ratio);

// 圆点链渲染：离散圆点，每个点是一个独立的四边形
static void NativeRenderDotChain(const std::vector<D2D1_POINT_2F>& path, float dotSize,
                                 const GradData& cols, float fadeAlpha, int screenW, int screenH) {
    if (!g_pNativeVS || !g_pNativePS || !g_pTrailVB || path.size() < 2) return;

    std::vector<VertexPosColor> verts;
    std::vector<VertexPosColor> shadowVerts;
    verts.reserve(path.size() * 6);  // 每个圆点 6 个顶点（2 个三角形）
    shadowVerts.reserve(path.size() * 6);
    const float SH_DX = 1.5f, SH_DY = 2.0f;

    for (size_t i = 0; i < path.size(); i++) {
        float ratio = (float)i / (path.size() - 1);
        float taper = powf(1.0f - ratio, 1.3f);
        float size = dotSize * taper;
        if (size < 0.5f) continue;

        float x = path[i].x, y = path[i].y;
        // 从渐变采样颜色
        D2D1_GRADIENT_STOP gs = cols.outer[(int)(ratio * (GRAD_STOPS - 1))];
        float r = gs.color.r, g = gs.color.g, b = gs.color.b;

        // 阴影圆点（向右下偏移的深色投影，受 enable_trail_shadow 控制）
        if (g_enableTrailShadow) {
            float sx = x + SH_DX, sy = y + SH_DY, sa = fadeAlpha * 0.18f;
            shadowVerts.push_back({sx - size, sy - size, 0, 0, 0, 0, sa, ratio});
            shadowVerts.push_back({sx + size, sy - size, 0, 0, 0, 0, sa, ratio});
            shadowVerts.push_back({sx - size, sy + size, 0, 0, 0, 0, sa, ratio});
            shadowVerts.push_back({sx + size, sy - size, 0, 0, 0, 0, sa, ratio});
            shadowVerts.push_back({sx + size, sy + size, 0, 0, 0, 0, sa, ratio});
            shadowVerts.push_back({sx - size, sy + size, 0, 0, 0, 0, sa, ratio});
        }

        // 四边形的四个角（两个三角形）
        // 三角形 1: 左上, 右上, 左下
        verts.push_back({x - size, y - size, 0, r, g, b, fadeAlpha, ratio});
        verts.push_back({x + size, y - size, 0, r, g, b, fadeAlpha, ratio});
        verts.push_back({x - size, y + size, 0, r, g, b, fadeAlpha, ratio});
        // 三角形 2: 右上, 右下, 左下
        verts.push_back({x + size, y - size, 0, r, g, b, fadeAlpha, ratio});
        verts.push_back({x + size, y + size, 0, r, g, b, fadeAlpha, ratio});
        verts.push_back({x - size, y + size, 0, r, g, b, fadeAlpha, ratio});
    }

    if (verts.empty() && shadowVerts.empty()) return;
    if (verts.size() > 4096) verts.resize(4096);
    if (shadowVerts.size() > 4096) shadowVerts.resize(4096);

    // 设置渲染状态
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

    // 先画阴影，再画主体
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

static bool NativeRenderFrame(int screenW, int screenH, const std::vector<D2D1_POINT_2F>& smoothed,
                              bool tailVisible, const GradData& cols, float widthMul, float fadeAlpha,
                              DWORD dwTime, int vX, int vY) {
    if (!g_pNativeVS || !g_pD3DContext || !g_pCachedRTV) return false;

    // 清空渲染目标（透明黑），RTV 随交换链缓存，避免每帧创建 COM 对象
    float clearColor[4] = {0, 0, 0, 0};
    g_pD3DContext->ClearRenderTargetView(g_pCachedRTV, clearColor);
    g_pD3DContext->OMSetRenderTargets(1, &g_pCachedRTV, nullptr);

    // 设置视口
    D3D11_VIEWPORT vp = {0, 0, (float)screenW, (float)screenH, 0, 1};
    g_pD3DContext->RSSetViewports(1, &vp);

    // 1. 粒子渲染（Instanced）
    if (!g_particles.empty()) {
        NativeRenderParticles(screenW, screenH);
    }

    // 2. 拖尾带渲染（顶点缓冲）
    // 锥形(0)、函数曲线(2)、波形曲线(3)、螺旋(7)、闪电(8)、羽毛(9) 直接渲染
    // 点链(1) 用小锥形段近似
    // 双线(5) 渲染两条偏移的带
    // 虚线(6) 分段渲染
    // 形状拖尾(4) 不渲染带
    if (tailVisible && !smoothed.empty() && g_trailShape != 4) {
        if (g_trailShape == 1) {
            // 点链：沿路径生成离散圆点
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
            // 双线拖尾：渲染两条偏移的带
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
            // 虚线拖尾：分段渲染（每隔一段跳过一段）
            std::vector<D2D1_POINT_2F> segment;
            int segLen = 8;
            for (size_t i = 0; i < smoothed.size(); i++) {
                segment.push_back(smoothed[i]);
                if ((i + 1) % segLen == 0) {
                    if (segment.size() >= 2)
                        NativeRenderTrail(segment, widthMul, cols, fadeAlpha, screenW, screenH, dwTime);
                    segment.clear();
                    i += segLen / 2;
                }
            }
            if (segment.size() >= 2)
                NativeRenderTrail(segment, widthMul, cols, fadeAlpha, screenW, screenH, dwTime);
        } else {
            // 锥形/函数/波形/螺旋/闪电/羽毛
            NativeRenderTrail(smoothed, widthMul, cols, fadeAlpha, screenW, screenH, dwTime);
        }
    }

    // 3. 点击波纹环渲染（顶点缓冲）
    if (g_enableClickEffect && !g_ripples.empty()) {
        NativeRenderRipples(screenW, screenH, dwTime, cols, vX, vY);
    }

    // 4. 运动模糊：绘制历史路径（透明度递减）
    if (g_enableMotionBlur && tailVisible && !g_trailHistory.empty()) {
        for (size_t h = 0; h < g_trailHistory.size(); h++) {
            float histAlpha = fadeAlpha * (0.15f + 0.1f * (float)h / g_trailHistory.size());
            if (histAlpha < 0.02f) continue;
            const std::vector<D2D1_POINT_2F>& histPath = g_trailHistory[h].path;
            if (histPath.size() < 2) continue;
            NativeRenderTrail(histPath, widthMul, cols, histAlpha, screenW, screenH, dwTime);
        }
    }

    // 5. 形状拖尾渲染（顶点缓冲）
    if (!g_trailShapes.empty()) {
        NativeRenderTrailShapes(screenW, screenH, dwTime);
    }

    // 恢复 D2D1 状态（如果后续还有 D2D1 渲染）
    g_pD2DDC->SetTarget(g_pD2DTargetBitmap);
    return true;
}

static inline float Rand01() {
    return rand() / (float)RAND_MAX;
}
static inline float Hash01(int n) {
    uint32_t u = (uint32_t)n;
    u = (u << 13) ^ u;
    return (float)(((u * (u * u * 15731u + 789221u) + 1376312589u) & 0x7fffffffu) / 2147483647.0);
}
static void SpawnParticles(float x, float y, int count, float speedMin, float speedMax, float sizeMin, float sizeMax,
                           int lifeMin, int lifeMax, D2D1_COLOR_F color, DWORD time, bool radial = false,
                           int shapeType = -1) {
    D2D1_COLOR_F endCol = D2D1::ColorF(color.r * 0.25f, color.g * 0.25f, color.b * 0.25f, 1.0f);
    for (int i = 0; i < count; i++) {
        float angle = radial ? (i / (float)count * 6.28318f) : (Rand01() * 6.28318f);
        float speed = speedMin + Rand01() * (speedMax - speedMin);
        int st = shapeType;
        if (st < 0) {
            st = g_particleShape;
            if (st == 0)
                st = (int)(Rand01() * 9.0f) + 1;  // random: 1-9 shapes
        }
        // 生成位置随机偏移：在生成点周围分布，避免所有粒子从同一点发射
        float spawnAngle = Rand01() * 6.28318f;
        float spawnRadius = Rand01() * 8.0f;  // 0-8 像素的随机偏移
        float px = x + cosf(spawnAngle) * spawnRadius;
        float py = y + sinf(spawnAngle) * spawnRadius;
        Particle p;
        p.x = px; p.y = py;
        p.vx = cosf(angle) * speed; p.vy = sinf(angle) * speed;
        p.size = sizeMin + Rand01() * (sizeMax - sizeMin);
        p.startTime = time;
        p.lifetime = lifeMin + (int)(Rand01() * (lifeMax - lifeMin));
        p.color = color;
        p.endColor = endCol;
        p.shapeType = st;
        p.z = (Rand01() - 0.5f) * 0.8f;  // 生成时随机深度，避免每帧闪烁
        p.rotation = Rand01() * 6.28318f;  // 随机初始旋转角度
        // 基础速度由设置控制，每个粒子有 ±50% 的随机差异，确保旋转速度存在差异
        // 注意：速度过快会导致非对称形状（三角形/星形等）因运动模糊看起来像圆形
        float baseSpin = (float)g_particleSpinSpeed / 100.0f * 0.3f;  // 最大 ±0.15 rad/帧
        float variation = (Rand01() - 0.5f) * baseSpin;  // ±50% 随机差异
        p.spinSpeed = baseSpin + variation;  // 随机自旋转速度，每个粒子不同
        p.colorOffset[0] = (Rand01() - 0.5f) * 0.16f;
        p.colorOffset[1] = (Rand01() - 0.5f) * 0.16f;
        p.colorOffset[2] = (Rand01() - 0.5f) * 0.16f;
        g_particles.push_back(p);
    }
}

// 沿路径获取指定比例（0=头，1=尾）的坐标
// Catmull-Rom 样条插值：生成更平滑的曲线
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
    g_particleRepelForce = (repelVal / 100.0f) * 3.0f;  // 0-100 → 0-3.0 像素/帧
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
        if (wcscmp(pshape, L"circle") == 0)
            g_particleShape = 1;
        else if (wcscmp(pshape, L"star") == 0)
            g_particleShape = 2;
        else if (wcscmp(pshape, L"hexagram") == 0)
            g_particleShape = 3;
        else if (wcscmp(pshape, L"heart") == 0)
            g_particleShape = 4;
        else if (wcscmp(pshape, L"diamond") == 0)
            g_particleShape = 5;
        else if (wcscmp(pshape, L"triangle") == 0)
            g_particleShape = 6;
        else if (wcscmp(pshape, L"flower") == 0)
            g_particleShape = 7;
        else if (wcscmp(pshape, L"pentagon") == 0)
            g_particleShape = 8;
        else if (wcscmp(pshape, L"hexagon") == 0)
            g_particleShape = 9;
        else
            g_particleShape = 0;  // random
        Wh_FreeStringSetting(pshape);
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
        else
            g_trailShape = 0;
        Wh_FreeStringSetting(str);
    }
    // 形状拖尾设置
    str = Wh_GetStringSetting(L"shape_type");
    if (str) {
        if (wcscmp(str, L"star") == 0)
            g_shapeType = 1;
        else if (wcscmp(str, L"hexagram") == 0)
            g_shapeType = 2;
        else if (wcscmp(str, L"circle") == 0)
            g_shapeType = 3;
        else if (wcscmp(str, L"random") == 0)
            g_shapeType = 4;
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
        ParseGradientColors(str, g_gradColors);
        Wh_FreeStringSetting(str);
    }

    if (g_triggerVelocity <= 0)
        g_triggerVelocity = 25;
    if (g_stopVelocity <= 0)
        g_stopVelocity = 10;
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

    // 根据颜色模式动态设置屏幕捕获排除（仅光标取色模式需要排除自己）
    if (g_overlayHwnd) {
        if (g_colorMode == 10 || g_colorMode == 11)  // cursor_extract or cursor_mix
            SetWindowDisplayAffinity(g_overlayHwnd, WDA_EXCLUDEFROMCAPTURE);
        else
            SetWindowDisplayAffinity(g_overlayHwnd, WDA_NONE);
    }
}

// ===================== 游戏检测 =====================
// 检测前台窗口是否为全屏应用（游戏/视频等）
// 实现方式：窗口尺寸匹配 + 光标裁剪 + 光标隐藏 + D3D 全屏状态
bool CheckForegroundFullscreen() {
    HWND fg = GetForegroundWindow();
    if (!fg || fg == GetDesktopWindow())
        return false;

    // 缓存桌面窗口句柄，每 500ms 刷新（Explorer 重启后失效）
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

    // 1. 系统 D3D 全屏状态检测
    QUERY_USER_NOTIFICATION_STATE quns;
    if (SUCCEEDED(SHQueryUserNotificationState(&quns)) && quns == QUNS_RUNNING_D3D_FULL_SCREEN)
        return true;

    // 2. 窗口尺寸与显示器匹配
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

    // 3. 光标裁剪（游戏通常会限制光标在窗口内）
    RECT rcClip;
    if (GetClipCursor(&rcClip)) {
        int vW = GetSystemMetrics(SM_CXVIRTUALSCREEN);
        int vH = GetSystemMetrics(SM_CYVIRTUALSCREEN);
        if ((rcClip.right - rcClip.left) < vW || (rcClip.bottom - rcClip.top) < vH)
            return true;
    }

    // 4. 光标隐藏（全屏游戏通常隐藏光标）
    CURSORINFO ci = {sizeof(CURSORINFO)};
    if (GetCursorInfo(&ci) && ci.flags == 0)
        return true;

    return false;
}

// 取色互补色偏移：色相+180°，增强饱和度和亮度，确保拖尾在任何背景上醒目
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

    // 根据偏移模式计算偏移角度
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

// ===================== 自适应对比度：背景亮度采样 =====================
// 沿拖尾路径、在路径法线两侧偏移采样屏幕像素（避开拖尾自身），计算平均亮度（0=暗，1=亮）
// 限制采样频率避免性能开销，采样结果用于自适应明度
static void SampleBackgroundLuminance(const std::vector<D2D1_POINT_2F>& path, DWORD dwTime, int vX, int vY) {
    if (!g_adaptiveContrast || path.size() < 2) return;
    // 500ms 一次：GetDC(NULL) 在 DWM 下会触发全屏 GPU→CPU 回读，过于频繁会与渲染线程的 DwmFlush 互斥阻塞导致掉帧
    if (dwTime - g_lastBgSample < 500) return;
    g_lastBgSample = dwTime;

    const float OFFSET = 10.0f;  // 沿法线偏移像素，需大于拖尾最大宽度以避开自身
    // 收集采样点：沿路径均匀 3 处，每处取法线两侧各 1 点（共最多 6 点）
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
        float nx = -dy / ln, ny = dx / ln;  // 单位法线
        float bx = path[i].x + vX, by = path[i].y + vY;
        pts[nPts++] = {(int)(bx + nx * OFFSET), (int)(by + ny * OFFSET)};
        pts[nPts++] = {(int)(bx - nx * OFFSET), (int)(by - ny * OFFSET)};
    }
    if (nPts == 0) return;

    // 计算采样点包围盒
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
    // 限制 DIB 尺寸，避免极端情况下分配过大
    if (w > 128) w = 128;
    if (h > 128) h = 128;

    HDC hdcScreen = GetDC(NULL);
    if (!hdcScreen) return;

    // 创建 top-down 32bpp DIB（biHeight 为负表示自上而下，避免坐标翻转）
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

    // 一次 BitBlt 批量回读包围盒区域，替代多次 GetPixel（每次 GetPixel 都会单独触发 DWM 往返）
    BitBlt(hdcMem, 0, 0, w, h, hdcScreen, minX, minY, SRCCOPY);

    float totalLum = 0.0f;
    int samples = 0;
    const BYTE* pSrc = (const BYTE*)pBits;
    for (int i = 0; i < nPts; ++i) {
        int dx = pts[i].x - minX, dy = pts[i].y - minY;
        if (dx < 0 || dx >= w || dy < 0 || dy >= h) continue;  // 超出 DIB 范围（被尺寸限制裁剪）则跳过
        const BYTE* px = pSrc + (dy * w + dx) * 4;  // 32bpp BGRA，top-down
        // 感知亮度：0.299R + 0.587G + 0.114B
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
        // 平滑过渡，避免亮度跳变
        g_bgLuminance = g_bgLuminance * 0.7f + avgLum * 0.3f;
    }
}

// 后台采样线程：把 GDI 回读（GetDC/BitBlt）从渲染线程剥离，避免阻塞 vsync
// 渲染线程只负责写入最新输入，本线程定期读取并执行慢操作，结果写回全局变量供渲染线程读取
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

        // 慢操作全部在锁外执行，不阻塞渲染线程
        if (adaptive) {
            SampleBackgroundLuminance(path, dwTime, vX, vY);
        }
        if (colorMode == 10 || colorMode == 11) {
            ExtractCursorColor(cursor, dwTime);
        }
    }
    return 0;
}

// ===================== 轨迹变形 =====================
// 螺旋变形：沿路径法线方向应用螺旋偏移
static void ApplySpiralDeformation(std::vector<D2D1_POINT_2F> &pts, DWORD dwTime) {
    if (pts.size() < 3) return;
    float phase = dwTime * 0.005f;
    std::vector<D2D1_POINT_2F> result;
    result.reserve(pts.size());
    for (size_t i = 0; i < pts.size(); ++i) {
        float ratio = (float)i / (pts.size() - 1);
        float spiral = sinf(ratio * 12.0f + phase) * (1.0f - ratio) * 15.0f;
        // 计算法线
        float dx, dy;
        if (i == 0) { dx = pts[1].x - pts[0].x; dy = pts[1].y - pts[0].y; }
        else if (i == pts.size() - 1) { dx = pts[i].x - pts[i-1].x; dy = pts[i].y - pts[i-1].y; }
        else { dx = pts[i+1].x - pts[i-1].x; dy = pts[i+1].y - pts[i-1].y; }
        float len = sqrtf(dx*dx + dy*dy);
        if (len > 0) { dx /= len; dy /= len; }
        float nx = -dy, ny = dx;
        result.push_back({pts[i].x + nx * spiral, pts[i].y + ny * spiral});
    }
    pts = result;
}

// 闪电变形：沿路径应用随机锯齿
static void ApplyLightningDeformation(std::vector<D2D1_POINT_2F> &pts, DWORD dwTime) {
    if (pts.size() < 3) return;
    std::vector<D2D1_POINT_2F> result;
    result.reserve(pts.size() * 2);
    for (size_t i = 0; i < pts.size() - 1; ++i) {
        result.push_back(pts[i]);
        // 在每两点之间插入一个随机偏移的中点
        float mx = (pts[i].x + pts[i+1].x) / 2.0f;
        float my = (pts[i].y + pts[i+1].y) / 2.0f;
        float dx = pts[i+1].x - pts[i].x, dy = pts[i+1].y - pts[i].y;
        float len = sqrtf(dx*dx + dy*dy);
        if (len > 0) {
            float nx = -dy / len, ny = dx / len;
            float offset = (rand() % 100 - 50) / 100.0f * 8.0f;
            result.push_back({mx + nx * offset, my + ny * offset});
        }
    }
    result.push_back(pts.back());
    pts = result;
}

static void ApplyWaveDeformation(std::vector<D2D1_POINT_2F> &pts, DWORD dwTime) {
    if (pts.size() < 3)
        return;
    float freq = g_waveFrequency / 100.0f, amp = (float)g_waveAmplitude;
    float phase = dwTime * 0.004f, dist = 0;
    std::vector<D2D1_POINT_2F> result;
    result.reserve(pts.size());
    for (size_t i = 0; i < pts.size(); ++i) {
        if (i > 0) {
            float ddx = pts[i].x - pts[i - 1].x, ddy = pts[i].y - pts[i - 1].y;
            dist += sqrtf(ddx * ddx + ddy * ddy);
        }
        float tdx, tdy;
        if (i == 0) {
            tdx = pts[1].x - pts[0].x;
            tdy = pts[1].y - pts[0].y;
        } else if (i == pts.size() - 1) {
            tdx = pts[i].x - pts[i - 1].x;
            tdy = pts[i].y - pts[i - 1].y;
        } else {
            tdx = pts[i + 1].x - pts[i - 1].x;
            tdy = pts[i + 1].y - pts[i - 1].y;
        }
        float tl = sqrtf(tdx * tdx + tdy * tdy);
        if (tl > 0.001f) {
            tdx /= tl;
            tdy /= tl;
        } else {
            tdx = 1;
            tdy = 0;
        }
        float nx = -tdy, ny = tdx;
        float taper = 1.0f - (float)i / (pts.size() - 1) * 0.65f;
        float wave = sinf(dist * freq + phase) * amp * taper;
        result.push_back(D2D1::Point2F(pts[i].x + nx * wave, pts[i].y + ny * wave));
    }
    pts = result;
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
    std::vector<D2D1_POINT_2F> result;
    result.reserve(pts.size());
    for (size_t i = 0; i < pts.size(); ++i) {
        if (i > 0) {
            float ddx = pts[i].x - pts[i - 1].x, ddy = pts[i].y - pts[i - 1].y;
            dist += sqrtf(ddx * ddx + ddy * ddy);
        }
        float tdx, tdy;
        if (i == 0) {
            tdx = pts[1].x - pts[0].x;
            tdy = pts[1].y - pts[0].y;
        } else if (i == pts.size() - 1) {
            tdx = pts[i].x - pts[i - 1].x;
            tdy = pts[i].y - pts[i - 1].y;
        } else {
            tdx = pts[i + 1].x - pts[i - 1].x;
            tdy = pts[i + 1].y - pts[i - 1].y;
        }
        float tl = sqrtf(tdx * tdx + tdy * tdy);
        if (tl > 0.001f) {
            tdx /= tl;
            tdy /= tl;
        } else {
            tdx = 1;
            tdy = 0;
        }
        float nx = -tdy, ny = tdx;
        float t = totalDist > 0 ? dist / totalDist : 0;
        float offset = EvalExpression(t, dist, time);
        if (offset > 60)
            offset = 60;
        if (offset < -60)
            offset = -60;
        result.push_back(D2D1::Point2F(pts[i].x + nx * offset, pts[i].y + ny * offset));
    }
    pts = result;
}

struct DotInfo {
    D2D1_POINT_2F pos;
    float radius;
    D2D1_COLOR_F outer;
    D2D1_COLOR_F inner;
    float alpha;
};

// ===================== 交换链/目标位图重建 =====================
static void RecreateSwapChain(int vW, int vH) {
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

    IDXGIFactory2 *pFactory = nullptr;
    if (FAILED(CreateDXGIFactory1(__uuidof(IDXGIFactory2), (void **)&pFactory)))
        return;

    DXGI_SWAP_CHAIN_DESC1 desc = {};
    desc.Width = vW;
    desc.Height = vH;
    desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
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
        Wh_Log(L"RecreateSwapChain: CreateSwapChainForComposition failed: 0x%08X (%dx%d)", hr, vW, vH);
        return;
    }

    IDXGISurface *pSurface = nullptr;
    if (FAILED(g_pSwapChain->GetBuffer(0, __uuidof(IDXGISurface), (void **)&pSurface)))
        return;

    D2D1_BITMAP_PROPERTIES1 bmpProps = {};
    bmpProps.pixelFormat = D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED);
    bmpProps.dpiX = 96.0f;
    bmpProps.dpiY = 96.0f;
    bmpProps.bitmapOptions = D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW;

    hr = g_pD2DDC->CreateBitmapFromDxgiSurface(pSurface, &bmpProps, &g_pD2DTargetBitmap);
    pSurface->Release();
    if (FAILED(hr)) {
        Wh_Log(L"RecreateSwapChain: CreateBitmapFromDxgiSurface failed: 0x%08X", hr);
        return;
    }
    Wh_Log(L"RecreateSwapChain: swap chain ready (%dx%d)", vW, vH);

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
    // 四边形带
    if (left.size() >= 2 && right.size() >= 2) {
        size_t n = (std::min)(left.size(), right.size());
        tris.reserve((n - 1) * 2 + 24);
        for (size_t i = 0; i < n - 1; i++) {
            tris.push_back({left[i], right[i], left[i + 1]});
            tris.push_back({right[i], right[i + 1], left[i + 1]});
        }
    }
    // 头部椭圆（三角形扇，24 段）
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

// ===================== 渲染子函数 =====================

// 粒子渲染（主体含发光模拟 + 多形状 + 颜色渐变）
static void RenderParticles(DWORD dwTime) {
    if (g_particles.empty()) return;
    bool particleFastPath = g_particles.size() > (g_superPerformanceMode ? 300 : 100);
    for (auto &p : g_particles) {
        float progress = (float)(dwTime - p.startTime) / p.lifetime;
        if (progress < 0 || progress >= 1) continue;
        float lifeAlpha = (1.0f - progress);
        D2D1_COLOR_F pc = D2D1::ColorF(p.color.r + (p.endColor.r - p.color.r) * progress,
                                       p.color.g + (p.endColor.g - p.color.g) * progress,
                                       p.color.b + (p.endColor.b - p.color.b) * progress, 1.0f);
        g_pSolidOuterBrush->SetColor(pc);
        g_pSolidOuterBrush->SetOpacity(lifeAlpha * 0.65f);
        float bodySize = p.size * 1.6f;
        if (!particleFastPath && p.shapeType == 2 && g_pStarGeom) {
            D2D1_MATRIX_3X2_F oldT;
            g_pD2DDC->GetTransform(&oldT);
            g_pD2DDC->SetTransform(D2D1::Matrix3x2F::Scale(bodySize, bodySize) *
                                   D2D1::Matrix3x2F::Translation(p.x, p.y));
            g_pD2DDC->FillGeometry(g_pStarGeom, g_pSolidOuterBrush);
            g_pD2DDC->SetTransform(oldT);
        } else if (!particleFastPath && p.shapeType == 3 && g_pHexagramGeom) {
            D2D1_MATRIX_3X2_F oldT;
            g_pD2DDC->GetTransform(&oldT);
            g_pD2DDC->SetTransform(D2D1::Matrix3x2F::Scale(bodySize, bodySize) *
                                   D2D1::Matrix3x2F::Translation(p.x, p.y));
            g_pD2DDC->FillGeometry(g_pHexagramGeom, g_pSolidOuterBrush);
            g_pD2DDC->SetTransform(oldT);
        } else {
            g_pD2DDC->FillEllipse(D2D1::Ellipse(D2D1::Point2F(p.x, p.y), bodySize, bodySize), g_pSolidOuterBrush);
        }
    }
    g_pSolidOuterBrush->SetOpacity(1.0f);
}

// 形状拖尾渲染（爱心/五角星/六角形/圆形，带出生动画）
static void RenderTrailShapes(DWORD dwTime) {
    if (g_trailShapes.empty()) return;
    for (auto &s : g_trailShapes) {
        float progress = (float)(dwTime - s.startTime) / s.lifetime;
        if (progress < 0 || progress >= 1) continue;
        float lifeAlpha = (1.0f - progress);
        float scale = s.size * (progress < 0.2f ? progress * 5.0f : 1.0f);
        g_pSolidOuterBrush->SetColor(s.color);
        g_pSolidOuterBrush->SetOpacity(lifeAlpha * 0.85f);
        ID2D1PathGeometry *geom = nullptr;
        if (s.shapeType == 0) geom = g_pHeartGeom;
        else if (s.shapeType == 1) geom = g_pStarGeom;
        else if (s.shapeType == 2) geom = g_pHexagramGeom;
        if (geom) {
            D2D1_MATRIX_3X2_F oldT;
            g_pD2DDC->GetTransform(&oldT);
            g_pD2DDC->SetTransform(D2D1::Matrix3x2F::Scale(scale, scale) *
                                   D2D1::Matrix3x2F::Translation(s.x, s.y));
            g_pD2DDC->FillGeometry(geom, g_pSolidOuterBrush);
            g_pD2DDC->SetTransform(oldT);
        } else {
            g_pD2DDC->FillEllipse(D2D1::Ellipse(D2D1::Point2F(s.x, s.y), scale, scale), g_pSolidOuterBrush);
        }
    }
    g_pSolidOuterBrush->SetOpacity(1.0f);
}

// 运动模糊历史帧渲染（仅锥形带，简化外带，透明度递减）
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

// 点击波纹渲染（外圈填充+描边，内圈描边）
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

// ===================== 主绘制循环 =====================
static void RenderFrame() {
    // 设置变更时在渲染线程中重载（避免 UI 线程与渲染线程竞争全局变量）
    if (g_settingsDirty.exchange(false)) {
        LoadSettings();
    }
    DWORD dwTime = GetTickCount();
    POINT pt;
    GetCursorPos(&pt);
    int dx = pt.x - g_lastPos.x, dy = pt.y - g_lastPos.y;
    float velocity = sqrtf((float)(dx * dx + dy * dy));
    g_lastPos = pt;

    // 光标取色和背景采样已移至后台线程（BgSamplerThreadProc），避免 GDI 回读阻塞渲染线程
    // 这里只把最新输入写入共享结构，后台线程定期读取执行

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

    // 点击检测（游戏中跳过，避免全屏游戏内生成不必要的效果）
    bool lDown = (GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0;
    bool rDown = (GetAsyncKeyState(VK_RBUTTON) & 0x8000) != 0;
    if (!isGameCached) {
        if (g_enableClickEffect) {
            if (lDown && !g_prevLButton)
                g_ripples.push_back({pt, dwTime});
            if (rDown && !g_prevRButton)
                g_ripples.push_back({pt, dwTime});
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
        // 游戏中隐藏覆盖层窗口，避免全屏顶层窗口阻挡游戏的独立翻转/MPO
        if (!gameHidden) {
            ShowWindowAsync(g_overlayHwnd, SW_HIDE);
            gameHidden = true;
            isWindowVisible = false;  // 同步窗口可见状态，避免退出游戏后窗口不显示
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
        gameHidden = false;  // 退出游戏后重置，允许窗口重新显示
        // 拖尾激活状态机：基于速度阈值 + 低速持续时间
        // 与 cursor-motion-blur 的实现不同：使用速度积分和加速度辅助判断
        static float accumulatedDist = 0.0f;
        static float lastVel = 0.0f;
        float accel = velocity - lastVel;
        lastVel = velocity;

        if (!trailActive) {
            // 未激活：速度超过阈值 或 加速度突增 时激活
            if (velocity > g_triggerVelocity || (accel > 5.0f && velocity > g_triggerVelocity * 0.5f)) {
                trailActive = true;
                idleFrameCount = 0;
                accumulatedDist = 0.0f;
            }
        } else {
            // 已激活：速度低于停止阈值时计数
            if (velocity < g_stopVelocity) {
                idleFrameCount++;
                accumulatedDist += velocity;
                // 低速超过 3 帧 或 累积距离过小 时停止
                if (idleFrameCount > 3 || (idleFrameCount > 1 && accumulatedDist < 2.0f)) {
                    trailActive = false;
                    g_hasLastShapePos = false;  // 重置形状拖尾位置
                    accumulatedDist = 0.0f;
                }
            } else {
                idleFrameCount = 0;
                accumulatedDist = 0.0f;
            }
        }
        if (trailActive) {
            POINT np = {renderPos.x - vX, renderPos.y - vY};
            g_history.push_front(np);
            while (g_history.size() > (size_t)g_tailLength)
                g_history.pop_back();
            fadeoutFrame = 0;
            // 移动时 alpha 快速恢复到 1.0
            g_fadeAlpha += (1.0f - g_fadeAlpha) * 0.4f;
            if (g_fadeAlpha > 1.0f)
                g_fadeAlpha = 1.0f;
        } else {
            // ===== 淡出模式：硬截断 / 加速收缩 / 软截断 =====
            switch (g_fadeoutMode) {
                case 0:  // 硬截断：立即清除
                    g_history.clear();
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
    bool havePath = (g_history.size() >= 2);
    if (havePath) {
        for (auto &p : g_history)
            smoothed.push_back(D2D1::Point2F((float)p.x + g_tailOffsetX, (float)p.y + g_tailOffsetY));
        if (g_enableBezierSmooth) {
            // Catmull-Rom 样条平滑（更顺滑的曲线）
            std::vector<D2D1_POINT_2F> bezierOut;
            CatmullRomSmooth(smoothed, bezierOut, 4);
            smoothed = bezierOut;
        } else {
            // 原线性插值平滑
            for (int iter = 0; iter < 2; ++iter) {
                if (smoothed.size() < 3)
                    break;
                std::vector<D2D1_POINT_2F> ns;
                ns.push_back(smoothed.front());
                for (size_t i = 0; i < smoothed.size() - 1; ++i) {
                    D2D1_POINT_2F p0 = smoothed[i], p1 = smoothed[i + 1];
                    ns.push_back(D2D1::Point2F(.75f * p0.x + .25f * p1.x, .75f * p0.y + .25f * p1.y));
                    ns.push_back(D2D1::Point2F(.25f * p0.x + .75f * p1.x, .25f * p0.y + .75f * p1.y));
                }
                ns.push_back(smoothed.back());
                smoothed = ns;
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
    }

    // ===== 把最新输入交给后台采样线程（GDI 回读在后台线程执行，不阻塞渲染）=====
    EnterCriticalSection(&g_sampleCS);
    g_samplePath = smoothed;
    g_sampleVX = vX; g_sampleVY = vY;
    g_sampleCursor = pt; g_sampleTime = dwTime;
    g_sampleHasPath = havePath;
    g_sampleColorMode = g_colorMode;
    g_sampleAdaptive = g_adaptiveContrast ? true : false;
    LeaveCriticalSection(&g_sampleCS);

    // ===== 运动模糊：保存当前路径到历史缓冲区（仅锥形带模式，避免切换形状后残留旧帧）=====
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

    // ===== 粒子释放（基于 smoothed 路径的指定位置）=====
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
            SpawnParticles(origin.x, origin.y, g_particleDensity, 0.3f * speedMul, 2.0f * speedMul, 2.0f, 4.5f, 300,
                           700, cols.solidOuter, dwTime);
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
                if (st == 4) st = (int)(Rand01() * 4);  // random
                // 随机方向初速度（0.5-2.0 像素/帧）
                float angle = Rand01() * 6.2831853f;
                float speed = 0.5f + Rand01() * 1.5f;
                g_trailShapes.push_back({sx, sy, cosf(angle) * speed, sinf(angle) * speed,
                                         g_shapeSize, st, dwTime, (float)g_shapeLifetime, cols.solidOuter});
            }
            g_lastShapeX = curX;
            g_lastShapeY = curY;
        }
    } else {
        g_hasLastShapePos = false;
    }
    // 形状总数上限（超级性能模式下提高上限）
    int shapeCap = g_superPerformanceMode ? 400 : 150;
    if (g_trailShapes.size() > (size_t)shapeCap) {
        g_trailShapes.erase(g_trailShapes.begin(), g_trailShapes.begin() + (g_trailShapes.size() - shapeCap));
    }
    // 粒子总数上限，防止参数拉满时性能崩溃（超级性能模式下提高上限）
    int particleCap = g_superPerformanceMode ? 500 : 200;
    if (g_particles.size() > (size_t)particleCap) {
        g_particles.erase(g_particles.begin(), g_particles.begin() + (g_particles.size() - particleCap));
    }
    g_prevVelocity = velocity;

    // ===== 粒子间排斥力计算（O(n²)，粒子过多时跳过以保证性能）=====
    if (g_enableParticleInteraction && !g_particles.empty()) {
        int pcount = (int)g_particles.size();
        int maxCalc = g_superPerformanceMode ? 500 : 250;
        if (pcount <= maxCalc) {
            float repelDist = (float)g_interParticleRepelDistance;
            float repelDistSq = repelDist * repelDist;
            for (int i = 0; i < pcount; i++) {
                for (int j = i + 1; j < pcount; j++) {
                    float dx = g_particles[i].x - g_particles[j].x;
                    float dy = g_particles[i].y - g_particles[j].y;
                    float distSq = dx * dx + dy * dy;
                    if (distSq < repelDistSq && distSq > 0.01f) {
                        float dist = sqrtf(distSq);
                        float falloff = 1.0f - dist / repelDist;
                        float force = falloff * g_interParticleRepelForce / dist;
                        g_particles[i].vx += dx * force;
                        g_particles[i].vy += dy * force;
                        g_particles[j].vx -= dx * force;
                        g_particles[j].vy -= dy * force;
                    }
                }
            }
        }
    }

    // ===== 粒子物理：摩擦 + 光标排斥力 + 随机扰动 + 全程吸附光标 =====
    float attractTargetX = (float)(renderPos.x - vX + g_tailOffsetX);
    float attractTargetY = (float)(renderPos.y - vY + g_tailOffsetY);
    for (auto &p : g_particles) {
        p.vx *= 0.93f;
        p.vy *= 0.93f;
        // 光标周围排斥力：粒子进入范围后被径向弹开 + 随机方向扰乱
        if (g_enableParticleRepel && g_particleRepelForce > 0) {
            float rdx = p.x - attractTargetX;
            float rdy = p.y - attractTargetY;
            float rdist = sqrtf(rdx * rdx + rdy * rdy);
            if (rdist < (float)g_particleRepelRadius && rdist > 0.5f) {
                float nx = rdx / rdist, ny = rdy / rdist;
                float falloff = 1.0f - rdist / (float)g_particleRepelRadius;  // 越靠近光标排斥越强
                float force = falloff * g_particleRepelForce;
                // 径向排斥
                p.vx += nx * force;
                p.vy += ny * force;
                // 随机方向扰动（扰乱吸附轨迹，制造绕飞感）
                float perturb = force * 0.65f;
                float angle = Rand01() * 6.28318f;
                p.vx += cosf(angle) * perturb;
                p.vy += sinf(angle) * perturb;
            }
        }
        p.x += p.vx;
        p.y += p.vy;
        if (g_enableParticleSpin) p.rotation += p.spinSpeed;  // 自旋转
        if (g_particleAttraction > 0) {
            p.x += (attractTargetX - p.x) * g_particleAttraction;
            p.y += (attractTargetY - p.y) * g_particleAttraction;
        }
    }
    if (!g_particles.empty())
        g_particles.erase(std::remove_if(g_particles.begin(), g_particles.end(),
                                         [&](const Particle &p) { return dwTime - p.startTime > (DWORD)p.lifetime; }),
                          g_particles.end());
    // 形状拖尾物理更新（随机方向速度 + 摩擦衰减）
    for (auto &s : g_trailShapes) {
        s.x += s.vx;
        s.y += s.vy;
        s.vx *= 0.96f;
        s.vy *= 0.96f;
    }
    // 形状拖尾过期清理
    if (!g_trailShapes.empty())
        g_trailShapes.erase(std::remove_if(g_trailShapes.begin(), g_trailShapes.end(),
                                            [&](const TrailShape &s) { return dwTime - s.startTime > (DWORD)s.lifetime; }),
                             g_trailShapes.end());

    bool tailVisible = (trailActive || g_history.size() >= 2) && g_fadeAlpha > 0.02f;
    bool isDrawing = tailVisible || !g_ripples.empty() || !g_particles.empty() || !g_trailShapes.empty();

    if (isDrawing) {
        hideDelayCounter = 0;
        if (!isWindowVisible) {
            ShowWindowAsync(g_overlayHwnd, SW_SHOWNA);
            isWindowVisible = true;
        }
    } else if (!surfaceDirty) {
        hideDelayCounter++;
        if (hideDelayCounter >= 3 && isWindowVisible) {
            ShowWindowAsync(g_overlayHwnd, SW_HIDE);
            isWindowVisible = false;
        }
    } else {
        hideDelayCounter = 0;
    }
    if (!isDrawing && !surfaceDirty)
        return;

    if (!g_pSwapChain || !g_pD2DTargetBitmap || g_cachedVW != vW || g_cachedVH != vH) {
        RecreateSwapChain(vW, vH);
    }
    if (!g_pD2DDC || !g_pD2DTargetBitmap)
        return;

    // v3：原生 D3D11 渲染路径（锥形拖尾 + 粒子）
    bool useNative = g_pNativeVS != nullptr;  // v3：所有拖尾形状都支持原生渲染
    if (useNative) {
        bool nativeOK = NativeRenderFrame(vW, vH, smoothed, tailVisible, cols, widthMul, g_fadeAlpha, dwTime, vX, vY);
        if (nativeOK) {
            // 原生渲染成功：所有效果（拖尾、粒子、形状、点击、运动模糊）均用 D3D11 原生渲染
            // 无需 D2D1 后处理

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
            // 原生渲染失败但有内容要渲染，可能是设备丢失，触发恢复
            g_deviceLost.store(true);
            return;
        }
        // 原生渲染失败且无内容，回退到 D2D1（通常是空闲帧）
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
    if (g_enableMotionBlur && tailVisible && havePath && g_trailHistory.size() > 1) {
        RenderMotionBlur(widthMul, cols, g_fadeAlpha);
        surfaceDirty = true;
    }

    // ===== 拖尾（复用已计算的 smoothed 路径）=====
    if (tailVisible && havePath && g_trailShape != 4) {
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
    // 三角形1（尖角朝上）
    sink->BeginFigure(D2D1::Point2F(0, -1), D2D1_FIGURE_BEGIN_FILLED);
    sink->AddLine(D2D1::Point2F(0.866f, 0.5f));
    sink->AddLine(D2D1::Point2F(-0.866f, 0.5f));
    sink->EndFigure(D2D1_FIGURE_END_CLOSED);
    // 三角形2（尖角朝下）
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
    ReleaseGradientBrushes();
    if (g_pShadowBrush) { g_pShadowBrush->Release(); g_pShadowBrush = nullptr; }
    if (g_pSolidInnerBrush) { g_pSolidInnerBrush->Release(); g_pSolidInnerBrush = nullptr; }
    if (g_pSolidOuterBrush) { g_pSolidOuterBrush->Release(); g_pSolidOuterBrush = nullptr; }
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
    if (msg == WM_DISPLAYCHANGE) {
        // 分辨率/显示器变化时更新缓存并调整窗口大小和位置
        UpdateVirtualScreenCache();
        // 不用 SWP_SHOWWINDOW，避免空闲隐藏状态下被意外显示
        SetWindowPos(hwnd, HWND_TOPMOST, g_virtX, g_virtY, g_virtW, g_virtH, SWP_NOACTIVATE | SWP_NOZORDER);
        return 0;
    }
    if (msg == WM_NCHITTEST) {
        // DirectComposition 窗口需要显式返回 HTTRANSPARENT 才能鼠标穿透
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
    wc.hbrBackground = nullptr;
    RegisterClass(&wc);
    UpdateVirtualScreenCache();
    int sx = g_virtX, sy = g_virtY;
    int sw = g_virtW, sh = g_virtH;
    g_overlayHwnd = CreateWindowEx(
        WS_EX_TRANSPARENT | WS_EX_LAYERED | WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE, CN,
        L"MouseTrailOverlay", WS_POPUP, sx, sy, sw, sh, NULL, NULL, hi, NULL);
    if (!g_overlayHwnd) {
        Wh_Log(L"OverlayThread: CreateWindowEx failed: %d", GetLastError());
        if (g_readyEvent) SetEvent(g_readyEvent);
        CoUninitialize();
        return 0;
    }
    // 分层窗口整体不透明（per-pixel alpha 由 DirectComposition 处理）
    SetLayeredWindowAttributes(g_overlayHwnd, 0, 255, LWA_ALPHA);
    // 屏幕捕获排除在 LoadSettings 中根据颜色模式动态设置
    Wh_Log(L"OverlayThread: window created (%dx%d at %d,%d), initially hidden", sw, sh, sx, sy);
    // 不立即 ShowWindow，等渲染线程首次有内容绘制时再显示，避免渲染失败时全屏透明窗口残留

    // 通知渲染线程窗口已就绪
    if (g_readyEvent) SetEvent(g_readyEvent);

    // 启动渲染线程
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

    // 等待 UI 线程创建窗口
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
    while (WaitForSingleObject(g_renderExitEvent, waitMs) != WAIT_OBJECT_0) {
        // 设备丢失恢复
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

    // ---- 清理渲染资源（复用统一清理，保证与设备丢失恢复路径一致，含 SetRoot(nullptr)）----
    Wh_Log(L"RenderThread: exiting, cleaning up");
    // 先停止后台采样线程，再释放渲染资源
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
    // 设置变更标记，由渲染线程消费（避免 UI 线程与渲染线程竞争）
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
