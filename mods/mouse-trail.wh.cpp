// ==WindhawkMod==
// @id              mouse-trail
// @name            Mouse Trail
// @name:zh-CN      鼠标拖尾
// @description     A highly customizable mouse cursor trail with particle effects, 12 color modes, custom function trails, cursor color extraction, and click effects. Direct2D hardware accelerated.
// @description:zh-CN 高度可定制的鼠标拖尾特效，支持粒子系统、12种颜色模式、自定义函数轨迹、光标取色和点击特效，Direct2D 硬件加速。
// @version         1.0
// @author          MCheng404
// @github          https://github.com/MCheng404
// @license         MIT
// @include         windhawk.exe
// @compilerOptions -ld2d1 -lole32 -lgdi32 -lshell32
// ==/WindhawkMod==
// ==WindhawkModReadme==
/*
# Mouse Trail / 鼠标拖尾

**EN:** A highly customizable mouse cursor trail with particle effects, multiple color modes, custom function trails, cursor color extraction, and click effects. Direct2D hardware accelerated. Runs as a dedicated process with zero CPU usage when idle.

**中文：** 高度可定制的鼠标拖尾特效，支持粒子系统、多种颜色模式、自定义函数轨迹、光标取色和点击特效。Direct2D 硬件加速，独立进程运行，静止时零 CPU 占用。

![Trail Effect](https://raw.githubusercontent.com/MCheng404/cursor-motion-blur-enhanced/main/assets/demo_trail.gif)

### More Demos / 更多展示

![Demo 2](https://raw.githubusercontent.com/MCheng404/cursor-motion-blur-enhanced/main/assets/demo_trail_2.gif)

![Demo 3](https://raw.githubusercontent.com/MCheng404/cursor-motion-blur-enhanced/main/assets/demo_trail_3.gif)

![Demo 4](https://raw.githubusercontent.com/MCheng404/cursor-motion-blur-enhanced/main/assets/demo_trail_4.gif)

![Demo 5](https://raw.githubusercontent.com/MCheng404/cursor-motion-blur-enhanced/main/assets/demo_trail_5.gif)

![Trail Screenshot](https://raw.githubusercontent.com/MCheng404/cursor-motion-blur-enhanced/main/assets/screenshot_trail.png)

---

### Features / 功能特性

* **4 Trail Shapes / 4 种拖尾形状：** Tapered ribbon / Tapered dot chain / Function curve / Sine wave. / 锥形带 / 类锥形圆链 / 函数曲线 / 正弦波浪。
* **12 Color Modes / 12 种颜色模式：** Single / Gradient (3-color) / Rainbow / Warm / Cool / Neon / Velocity / Stripes / Fire / Aurora / Cursor Extract / Cursor Mix.
* **Particle System / 粒子系统：** Mini particles released from the trail, attracted back to cursor with configurable origin (head/middle/tail/custom), attraction strength, and cursor repulsion force. Shapes: circle / star / hexagram / random mix. Colors fade over lifetime. / 拖尾释放迷你粒子，全程吸附回光标。释放位置、吸附强度、光标排斥力均可调。支持圆形/五角星/六芒星/随机混合形状，颜色随生命周期渐变。
* **Click Effects / 点击特效：** Starburst particle burst + expanding ripple on left/right click (both toggleable). / 点击时迸发星爆粒子 + 扩散波纹（均可开关）。
* **Cursor Color Extraction / 光标取色：** Real-time pixel color sampling under the cursor (2 modes), with auto complementary-color shift for visibility. / 实时提取光标下方像素颜色（2种模式），支持自动互补色偏移确保醒目。
* **Function Trails / 函数轨迹：** Custom math expressions generate trail curves, 4 built-in presets. / 自定义数学公式生成轨迹曲线，内置4组预设。
* **Delay Rendering / 延迟渲染：** Trail head eases toward the cursor (0-10 adjustable). / 拖尾头部缓动跟随光标（0-10可调）。
* **Fadeout Modes / 淡出模式：** Hard cut / Accelerated shrink / Soft fade (alpha + length synchronized). / 硬截断 / 加速收缩 / 软截断（透明度+长度同步）。
* **Dynamic Width / 动态宽度：** Trail widens with speed and acceleration. / 移动越快、急转时拖尾越宽。
* **Enhanced Glow / 增强发光：** Dual-layer halo (outer glow + inner bloom), toggleable. / 双层光晕（外晕+内辉），可开关。
* **Head Highlight / 头部高光：** Bright center dot at the trail head. / 拖尾头部明亮中心点。
* **Trail Shadow / 拖尾阴影：** Dark underlay adds depth. / 底层暗色阴影增加立体感。
* **Game Detection / 游戏检测：** Auto-disable in fullscreen DirectX games. / 全屏 DirectX 游戏时自动禁用。
* **Idle at 0% CPU / 零 CPU 待机：** Window hidden when cursor is stationary and no effects active. / 鼠标静止且无特效时窗口隐藏，CPU 占用为 0%。

### Function Trail Variables / 函数轨迹变量
**EN:** Available variables: `t` (normalized 0=head 1=tail), `d` (distance from head in px), `time` (seconds). Functions: sin cos tan exp sqrt abs log. Operators: + - * / ^. Constants: pi e.
**中文：** 自定义公式中可使用：`t`（归一化位置 0=头 1=尾）、`d`（距头部像素距离）、`time`（秒）。支持函数：sin cos tan exp sqrt abs log，运算符：+ - * / ^，常量：pi e。
Examples / 示例：`sin(d * 0.15) * 8`, `sin(d * 0.25) * exp(0 - t * 2.5) * 10`.

### Color Format / 颜色格式
**EN:** Hex RGB, e.g. `FF0000`=red, `00FF00`=green, `0000FF`=blue, `FFD700`=gold.
**中文：** 自定义颜色使用十六进制 RGB，例如：`FF0000`=红，`00FF00`=绿，`0000FF`=蓝，`FFD700`=金。

### Author / 作者
Developed by [MCheng404](https://github.com/MCheng404).
开发者 [MCheng404](https://github.com/MCheng404)。
*/
// ==/WindhawkModReadme==
// ==WindhawkModSettings==
/*
- trigger_velocity: 25
  $name: Trigger Velocity
  $name:zh-CN: 触发速度
  $description: How fast the mouse must move to trigger the blur (pixels per frame).
  $description:zh-CN: 鼠标移动多快时触发拖影（像素/帧）。
- stop_velocity: 10
  $name: Stop Velocity
  $name:zh-CN: 停止速度
  $description: Velocity threshold to stop the blur. Must be lower than Trigger Velocity.
  $description:zh-CN: 停止拖影的速度阈值（像素/帧）。必须低于触发速度。
- tail_offset_x: 6
  $name: Tail Offset X
  $name:zh-CN: 拖尾 X 偏移
  $description: X-axis offset where the tail connects to the cursor.
  $description:zh-CN: 拖尾连接到光标的 X 轴偏移量（像素）。
- tail_offset_y: 10
  $name: Tail Offset Y
  $name:zh-CN: 拖尾 Y 偏移
  $description: Y-axis offset where the tail connects to the cursor.
  $description:zh-CN: 拖尾连接到光标的 Y 轴偏移量（像素）。
- tail_length: 10
  $name: Tail Length
  $name:zh-CN: 拖尾长度
  $description: How many frames the blur trails behind you. Minimum 2.
  $description:zh-CN: 拖影跟随的帧数。最低为 2。
- trail_delay: 0
  $name: Trail Delay
  $name:zh-CN: 拖尾延迟
  $description: How much the trail head lags behind the cursor (0-10, 0=off).
  $description:zh-CN: 拖尾头部滞后于光标的程度（0-10，0=关闭）。
- enable_smooth_gradient: true
  $name: Smooth Gradient
  $name:zh-CN: 平滑渐变
  $description: Head-to-tail opacity gradient.
  $description:zh-CN: 拖尾透明度渐变淡出。
- fadeout_mode: soft
  $name: Fadeout Mode
  $name:zh-CN: 淡出模式
  $description: How the trail disappears when the mouse stops.
  $description:zh-CN: 鼠标停止后拖尾的消失方式。
  $options:
  - hard: Hard Cut
  - accelerate: Accelerated Shrink
  - soft: Soft Fade
  $options:zh-CN:
  - hard: 硬截断
  - accelerate: 加速收缩
  - soft: 软截断
- enable_speed_response: true
  $name: Dynamic Width
  $name:zh-CN: 动态宽度
  $description: Trail width responds to speed and acceleration.
  $description:zh-CN: 移动速度和加速度影响拖尾宽度，急转时更宽。
- enhanced_glow: true
  $name: Enhanced Glow
  $name:zh-CN: 增强发光
  $description: Dual-layer halo for softer glow. Requires Micro Glow to be enabled.
  $description:zh-CN: 双层光晕（外晕+内辉），发光更柔和自然。需先开启微发光效果。
- enable_head_highlight: true
  $name: Head Highlight
  $name:zh-CN: 头部高光
  $description: Bright center dot at trail head. Tapered/function/wave shapes only.
  $description:zh-CN: 拖尾头部添加明亮中心点，提升质感。仅锥形/函数/波浪形状生效。
- enable_trail_shadow: true
  $name: Trail Shadow
  $name:zh-CN: 拖尾阴影
  $description: Dark underlay shadow for depth.
  $description:zh-CN: 拖尾底层绘制暗色阴影，增加立体感。
- trail_shape: tapered
  $name: Trail Shape
  $name:zh-CN: 拖尾形状
  $options:
  - tapered: Tapered
  - dots: Dot Chain
  - function: Function Curve
  - wave: Wave Curve
  $options:zh-CN:
  - tapered: 锥形
  - dots: 类锥形圆链
  - function: 函数曲线
  - wave: 波浪曲线
- dots_multiplier: 2
  $name: Dot Chain Density
  $name:zh-CN: 圆链密度倍率
  $description: Dot count multiplier (1-5), higher = more smaller dots. Dot Chain shape only.
  $description:zh-CN: 类锥形圆链的小球数量倍率（1-5），越大小球越多且越小。仅圆链形状生效。
- function_preset: sine
  $name: Function Preset
  $name:zh-CN: 函数预设
  $description: Preset formula for function curve shape. Choose custom to use your own formula.
  $description:zh-CN: 函数曲线形状的预设公式，选择 custom 时使用下方自定义公式。
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
  $description: "Variables: t(0-1) d(distance) time(sec); Functions: sin cos exp sqrt abs; Operators: + - * / ^."
  $description:zh-CN: "变量 t(0-1) d(距离) time(秒)；函数 sin cos exp sqrt abs；运算符 + - * / ^。"
- wave_amplitude: 8
  $name: Wave Amplitude
  $name:zh-CN: 波浪幅度
  $description: Wave amplitude in pixels. Wave shape only.
  $description:zh-CN: 波浪曲线的振幅（像素）。仅波浪形状生效。
- wave_frequency: 15
  $name: Wave Frequency
  $name:zh-CN: 波浪频率
  $description: Wave frequency (3-60, higher = denser waves). Wave shape only.
  $description:zh-CN: 波浪曲线的频率（3-60，越大波浪越密）。仅波浪形状生效。
- enable_glow: true
  $name: Micro Glow
  $name:zh-CN: 微发光效果
  $description: Soft outer glow around the trail.
  $description:zh-CN: 拖尾外圈柔和发光。
- glow_intensity: 40
  $name: Glow Intensity
  $name:zh-CN: 发光强度
  $description: Glow radius and brightness (0-100).
  $description:zh-CN: 发光范围和亮度（0-100）。
- color_mode: single
  $name: Color Mode
  $name:zh-CN: 颜色模式
  $options:
  - single: Single Color
  - gradient: Gradient
  - rainbow: Rainbow
  - warm: Warm Flow
  - cool: Cool Flow
  - neon: Neon Pulse
  - velocity: Velocity Color
  - stripes: Stripes
  - fire: Fire
  - aurora: Aurora
  - cursor_extract: Cursor Extract
  - cursor_mix: Cursor Mix
  $options:zh-CN:
  - single: 单色
  - gradient: 多色渐变
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
- enable_cursor_color_shift: true
  $name: Auto Color Shift
  $name:zh-CN: 取色自动偏移
  $description: Auto complementary-color shift (+180° hue) for cursor extraction modes, ensuring visibility on any background.
  $description:zh-CN: 光标取色模式下自动将提取的颜色转为互补色（色相+180°）并增强饱和度和亮度，确保拖尾在任何背景上都醒目可见。
- custom_color: "00BFFF"
  $name: Custom Color
  $name:zh-CN: 自定义颜色
  $description: Primary color for single/neon/cursor-mix modes. Hex RGB.
  $description:zh-CN: 单色/霓虹/光标混色模式的主色，十六进制 RGB。
- gradient_colors: "FF6B35,00BFFF,FFD700"
  $name: Gradient Colors 1,2,3
  $name:zh-CN: 渐变颜色1,2,3
  $description: 3 colors for gradient/stripes modes, comma-separated hex RGB (e.g. FF6B35,00BFFF,FFD700).
  $description:zh-CN: 多色渐变/条纹模式的颜色，用英文逗号分隔3个十六进制RGB（如 FF6B35,00BFFF,FFD700）。
- particle_mode: fadeout
  $name: Particle Mode
  $name:zh-CN: 粒子消散模式
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
  $options:zh-CN:
  - head: 开头（光标处）
  - middle: 中间
  - tail: 结尾
  - custom: 自定义比例
- particle_origin_ratio: 80
  $name: Custom Origin Ratio
  $name:zh-CN: 自定义释放比例
  $description: Position along trail (0=head/cursor, 100=tail). Custom origin only.
  $description:zh-CN: 沿拖尾的位置比例（0=开头光标，100=结尾）。仅释放位置为自定义时生效。
- particle_attraction: 40
  $name: Particle Attraction
  $name:zh-CN: 粒子吸附强度
  $description: How strongly particles are attracted to cursor (0=off, 1-100, non-linear curve).
  $description:zh-CN: 粒子被吸向光标的强度（0=关闭，1-100，非线性曲线：低数值区分度高）。
- enable_particle_repel: true
  $name: Cursor Repulsion
  $name:zh-CN: 光标排斥力
  $description: Particles near cursor are repelled with random perturbation, creating orbiting motion.
  $description:zh-CN: 粒子飞到光标附近时被排斥力弹开，并随机扰乱轨迹，形成振荡绕飞效果。
- particle_repel_radius: 25
  $name: Repulsion Radius
  $name:zh-CN: 排斥范围
  $description: Repulsion radius around cursor in pixels (5-100).
  $description:zh-CN: 光标周围的排斥半径（像素，5-100）。粒子进入此范围会受到排斥力。
- particle_repel_force: 30
  $name: Repulsion Force
  $name:zh-CN: 排斥强度
  $description: Repulsion and random perturbation strength (0-100).
  $description:zh-CN: 排斥力和随机扰动的强度（0-100）。数值越大粒子被弹开越远、扰乱越剧烈。
- particle_density: 3
  $name: Particle Density
  $name:zh-CN: 粒子密度
  $description: Number of particles per release (1-10).
  $description:zh-CN: 每次释放的粒子数量（1-10）。数值越大消散越明显。
- particle_interval: 50
  $name: Particle Interval
  $name:zh-CN: 粒子释放间隔
  $description: Minimum interval between particle releases in ms (10-2000).
  $description:zh-CN: 粒子释放的最小时间间隔（毫秒，10-2000），越小越密集。
- particle_acceleration: true
  $name: Acceleration Effect
  $name:zh-CN: 加速度影响
  $description: Particle initial velocity affected by mouse acceleration.
  $description:zh-CN: 粒子初速度受鼠标相对加速度影响（速度变化越大粒子飞散越快）。
- particle_shape: random
  $name: Particle Shape
  $name:zh-CN: 粒子形状
  $description: Particle shape. Random mix includes circle, star, and hexagram.
  $description:zh-CN: 粒子消散时的形状。随机混合会同时出现圆形、五角星、六芒星。
  $options:
  - random: Random Mix
  - circle: Circle
  - star: Star
  - hexagram: Hexagram
  $options:zh-CN:
  - random: 随机混合
  - circle: 仅圆形
  - star: 仅五角星
  - hexagram: 仅六芒星
- enable_click_starburst: true
  $name: Click Starburst
  $name:zh-CN: 点击星爆
  $description: Particle burst on mouse click.
  $description:zh-CN: 点击时从光标位置迸发粒子。
- starburst_count: 8
  $name: Starburst Count
  $name:zh-CN: 星爆粒子数
  $description: Number of particles per click burst (4-20).
  $description:zh-CN: 每次点击迸发的粒子数量（4-20）。
- enable_click_effect: true
  $name: Click Ripple
  $name:zh-CN: 点击波纹
  $description: Expanding ripple on mouse click.
  $description:zh-CN: 点击时产生扩散波纹。
- click_max_radius: 40
  $name: Ripple Max Radius
  $name:zh-CN: 波纹最大半径
  $description: Maximum ripple radius in pixels.
  $description:zh-CN: 点击波纹扩散的最大半径（像素）。
- click_duration: 300
  $name: Ripple Duration
  $name:zh-CN: 波纹持续时间
  $description: Ripple duration in milliseconds.
  $description:zh-CN: 点击波纹从出现到消失的时长（毫秒）。
*/
// ==/WindhawkModSettings==
#include <windows.h>
#include <d2d1.h>
#include <math.h>
#include <shellapi.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <deque>
#include <vector>
#include <algorithm>

#define GRAD_STOPS 12

// ===================== 颜色工具 =====================
static D2D1_COLOR_F HSVtoRGB(float h, float s, float v) {
    h = fmodf(h, 360.0f); if (h < 0) h += 360.0f;
    float c = v * s, x = c * (1.0f - fabsf(fmodf(h / 60.0f, 2.0f) - 1.0f)), m = v - c;
    float r, g, b;
    if (h < 60)      { r = c; g = x; b = 0; }
    else if (h < 120) { r = x; g = c; b = 0; }
    else if (h < 180) { r = 0; g = c; b = x; }
    else if (h < 240) { r = 0; g = x; b = c; }
    else if (h < 300) { r = x; g = 0; b = c; }
    else              { r = c; g = 0; b = x; }
    return D2D1::ColorF(r + m, g + m, b + m, 1.0f);
}
static D2D1_COLOR_F LerpColor(D2D1_COLOR_F a, D2D1_COLOR_F b, float t) {
    return D2D1::ColorF(a.r + (b.r - a.r) * t, a.g + (b.g - a.g) * t, a.b + (b.b - a.b) * t, 1.0f);
}
static D2D1_COLOR_F LighterColor(D2D1_COLOR_F c, float amount = 0.55f) {
    return D2D1::ColorF(c.r + (1.0f - c.r) * amount, c.g + (1.0f - c.g) * amount, c.b + (1.0f - c.b) * amount, 1.0f);
}
static D2D1_COLOR_F ParseHexColor(PCWSTR hex, D2D1_COLOR_F fallback) {
    if (!hex || !*hex) return fallback;
    // Validate: must be exactly 6 hex digits
    int len = 0;
    while (hex[len] && len < 8) {
        WCHAR c = hex[len];
        if (!((c >= L'0' && c <= L'9') || (c >= L'a' && c <= L'f') || (c >= L'A' && c <= L'F'))) return fallback;
        len++;
    }
    if (len != 6) return fallback;
    DWORD val = wcstoul(hex, nullptr, 16);
    return D2D1::ColorF(((val >> 16) & 0xFF) / 255.0f, ((val >> 8) & 0xFF) / 255.0f, (val & 0xFF) / 255.0f, 1.0f);
}
static void ParseGradientColors(PCWSTR input, D2D1_COLOR_F out[3]) {
    if (!input || !*input) return;
    WCHAR buf[256];
    wcsncpy_s(buf, input, 255); buf[255] = 0;
    int idx = 0;
    WCHAR* ctx = nullptr;
    WCHAR* tok = wcstok_s(buf, L",", &ctx);
    while (tok && idx < 3) {
        while (*tok == L' ' || *tok == L'\t') tok++;
        D2D1_COLOR_F c = ParseHexColor(tok, out[idx]);
        out[idx++] = c;
        tok = wcstok_s(nullptr, L",", &ctx);
    }
}

// ===================== 数学表达式解析器 =====================
enum ExprTokType { ET_NUM, ET_VAR, ET_FUNC, ET_OP, ET_LPAREN, ET_RPAREN };
struct ExprToken { ExprTokType type; float num; char name[16]; };

static std::vector<ExprToken> g_exprRPN;
static bool g_exprValid = false;

static int OpPrec(char op) {
    if (op == '+' || op == '-') return 1;
    if (op == '*' || op == '/') return 2;
    if (op == '^') return 4;
    return 0;
}

static void CompileExpression(const char* expr) {
    g_exprValid = false;
    g_exprRPN.clear();
    if (!expr || !*expr) return;
    char buf[512]; int bi = 0;
    bool expectVal = true;
    for (int i = 0; expr[i] && bi < 510; i++) {
        char c = expr[i];
        if (c == '-' && expectVal) { buf[bi++] = '0'; buf[bi++] = '-'; expectVal = false; continue; }
        if (c == ' ' || c == '\t') continue;
        buf[bi++] = c;
        expectVal = (c == '(' || c == '+' || c == '-' || c == '*' || c == '/' || c == '^');
    }
    buf[bi] = 0;
    std::vector<ExprToken> tokens;
    int i = 0;
    while (buf[i]) {
        if (isdigit((unsigned char)buf[i]) || buf[i] == '.') {
            float val = 0; int dec = 0; float div = 1;
            while (isdigit((unsigned char)buf[i]) || buf[i] == '.') {
                if (buf[i] == '.') dec = 1;
                else if (dec) { div *= 10; val += (buf[i] - '0') / div; }
                else val = val * 10 + (buf[i] - '0');
                i++;
            }
            tokens.push_back({ ET_NUM, val, "" });
        } else if (isalpha((unsigned char)buf[i])) {
            char name[16] = { 0 }; int j = 0;
            while (isalnum((unsigned char)buf[i]) && j < 15) name[j++] = buf[i++];
            if (buf[i] == '(') tokens.push_back({ ET_FUNC, 0, "" }), strcpy_s(tokens.back().name, name);
            else tokens.push_back({ ET_VAR, 0, "" }), strcpy_s(tokens.back().name, name);
        } else if (buf[i] == '(') { tokens.push_back({ ET_LPAREN, 0, "" }); i++; }
        else if (buf[i] == ')') { tokens.push_back({ ET_RPAREN, 0, "" }); i++; }
        else if (strchr("+-*/^", buf[i])) {
            tokens.push_back({ ET_OP, 0, "" }); tokens.back().name[0] = buf[i]; tokens.back().name[1] = 0; i++;
        } else i++;
    }
    std::vector<ExprToken> output, stack;
    for (auto& tok : tokens) {
        if (tok.type == ET_NUM || tok.type == ET_VAR) output.push_back(tok);
        else if (tok.type == ET_FUNC) stack.push_back(tok);
        else if (tok.type == ET_OP) {
            while (!stack.empty() && stack.back().type != ET_LPAREN &&
                   stack.back().type != ET_FUNC && OpPrec(stack.back().name[0]) >= OpPrec(tok.name[0])) {
                output.push_back(stack.back()); stack.pop_back();
            }
            stack.push_back(tok);
        } else if (tok.type == ET_LPAREN) stack.push_back(tok);
        else if (tok.type == ET_RPAREN) {
            while (!stack.empty() && stack.back().type != ET_LPAREN) { output.push_back(stack.back()); stack.pop_back(); }
            if (!stack.empty()) stack.pop_back();
            if (!stack.empty() && stack.back().type == ET_FUNC) { output.push_back(stack.back()); stack.pop_back(); }
        }
    }
    while (!stack.empty()) { output.push_back(stack.back()); stack.pop_back(); }
    g_exprRPN = output;
    g_exprValid = !output.empty();
}

static float EvalExpression(float t, float d, float time) {
    if (!g_exprValid || g_exprRPN.empty()) return 0;
    float stk[64]; int sp = 0;
    for (auto& tok : g_exprRPN) {
        if (sp >= 63) break;
        if (tok.type == ET_NUM) stk[sp++] = tok.num;
        else if (tok.type == ET_VAR) {
            if (strcmp(tok.name, "t") == 0) stk[sp++] = t;
            else if (strcmp(tok.name, "d") == 0) stk[sp++] = d;
            else if (strcmp(tok.name, "time") == 0) stk[sp++] = time;
            else if (strcmp(tok.name, "pi") == 0) stk[sp++] = 3.14159265f;
            else if (strcmp(tok.name, "e") == 0) stk[sp++] = 2.7182818f;
            else stk[sp++] = 0;
        } else if (tok.type == ET_OP) {
            if (sp < 2) { sp = 0; break; }
            float b = stk[--sp], a = stk[--sp], r = 0;
            switch (tok.name[0]) {
                case '+': r = a + b; break;
                case '-': r = a - b; break;
                case '*': r = a * b; break;
                case '/': r = (b != 0) ? a / b : 0; break;
                case '^': r = powf(fabsf(a) + 0.0001f, b); break;
            }
            stk[sp++] = r;
        } else if (tok.type == ET_FUNC) {
            if (sp < 1) { sp = 0; break; }
            float a = stk[--sp], r = 0;
            if (strcmp(tok.name, "sin") == 0) r = sinf(a);
            else if (strcmp(tok.name, "cos") == 0) r = cosf(a);
            else if (strcmp(tok.name, "tan") == 0) r = tanf(a);
            else if (strcmp(tok.name, "exp") == 0) r = expf(a);
            else if (strcmp(tok.name, "sqrt") == 0) r = sqrtf(fabsf(a));
            else if (strcmp(tok.name, "abs") == 0) r = fabsf(a);
            else if (strcmp(tok.name, "log") == 0) r = logf(fabsf(a) + 0.0001f);
            stk[sp++] = r;
        }
    }
    return sp > 0 ? stk[sp - 1] : 0;
}

// ===================== 全局状态 =====================
HWND g_overlayHwnd = NULL;
HANDLE g_threadHandle = NULL;
HANDLE g_readyEvent = NULL;
std::deque<POINT> g_history;
POINT g_lastPos = { 0, 0 };

int g_trailDelay = 0;
POINT g_lagPos = { 0, 0 };
bool g_lagInited = false;

float g_fadeAlpha = 1.0f;

D2D1_COLOR_F g_cursorExtractedColor = { 0.5f, 0.5f, 0.5f, 1.0f };
bool g_cursorColorShift = true;
static DWORD s_lastColorExtract = 0;

ID2D1Factory* g_pD2DFactory = nullptr;
ID2D1DCRenderTarget* g_pDCRenderTarget = nullptr;
ID2D1SolidColorBrush* g_pSolidOuterBrush = nullptr;
ID2D1SolidColorBrush* g_pSolidInnerBrush = nullptr;
ID2D1SolidColorBrush* g_pShadowBrush = nullptr;
ID2D1LinearGradientBrush* g_pGradOuterBrush = nullptr;
ID2D1LinearGradientBrush* g_pGradInnerBrush = nullptr;
ID2D1GradientStopCollection* g_pGradOuterStops = nullptr;
ID2D1GradientStopCollection* g_pGradInnerStops = nullptr;

HDC g_hdcMem = NULL;
HBITMAP g_hBitmap = NULL;
int g_cachedVW = 0, g_cachedVH = 0;

// ===================== 设置缓存 =====================
float g_triggerVelocity = 25.0f, g_stopVelocity = 10.0f;
int g_tailOffsetX = 6, g_tailOffsetY = 10, g_tailLength = 10;
bool g_enableSmoothGradient = true;
int g_fadeoutMode = 2; // 0=hard 1=accelerate 2=soft
bool g_enableSpeedResponse = true;
bool g_enhancedGlow = true;
bool g_enableHeadHighlight = true;
bool g_enableTrailShadow = true;
int g_trailShape = 0;
int g_dotsMultiplier = 2;
int g_functionPreset = 0;
char g_customFunction[256] = "sin(d * 0.15) * 8";
int g_waveAmplitude = 8, g_waveFrequency = 15;
bool g_enableGlow = true;
int g_glowIntensity = 40;
int g_colorMode = 0;
D2D1_COLOR_F g_customColor = { 0.0f, 0.75f, 1.0f, 1.0f };
D2D1_COLOR_F g_gradColors[3] = { { 1.0f, 0.42f, 0.21f, 1.0f }, { 0.0f, 0.75f, 1.0f, 1.0f }, { 1.0f, 0.84f, 0.0f, 1.0f } };
int g_particleMode = 1;
int g_particleOrigin = 2; // 0=head 1=middle 2=tail 3=custom
int g_particleOriginRatio = 80;
float g_particleAttraction = 0.032f;
bool g_enableParticleRepel = true;
int g_particleRepelRadius = 25;
float g_particleRepelForce = 0.9f;
int g_particleDensity = 3;
int g_particleInterval = 50;
bool g_particleAccel = true;
int g_particleShape = 0; // 0=random, 1=circle, 2=star, 3=hexagram
ID2D1PathGeometry* g_pStarGeom = nullptr;
ID2D1PathGeometry* g_pHexagramGeom = nullptr;
DWORD g_lastParticleTime = 0;
float g_prevVelocity = 0;
bool g_enableClickStarburst = true;
int g_starburstCount = 8;
bool g_enableClickEffect = true;
int g_clickMaxRadius = 40, g_clickDuration = 300;

// ===================== 粒子系统 =====================
struct Particle { float x, y, vx, vy, size; DWORD startTime; int lifetime; D2D1_COLOR_F color; D2D1_COLOR_F endColor; int shapeType; };
std::vector<Particle> g_particles;
struct Ripple { POINT pos; DWORD startTime; };
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
static bool GradApproxEq(const GradData& a, const GradData& b) {
    for (int i = 0; i < GRAD_STOPS; i++) {
        if (!ColorApproxEq(a.outer[i].color, b.outer[i].color)) return false;
        if (!ColorApproxEq(a.inner[i].color, b.inner[i].color)) return false;
    }
    return true;
}

static void ComputeColors(int mode, DWORD time, float velocity, GradData& out) {
    float t = time / 1000.0f;
    D2D1_COLOR_F headOuter, headInner, tailOuter, tailInner;
    switch (mode) {
        case 0: headOuter = tailOuter = g_customColor; headInner = tailInner = LighterColor(g_customColor); break;
        case 1: headOuter = g_gradColors[0]; tailOuter = g_gradColors[2]; headInner = LighterColor(g_gradColors[0]); tailInner = LighterColor(g_gradColors[2]); break;
        case 2: { float h = fmodf(t*80,360); headOuter=HSVtoRGB(h,.9f,1); tailOuter=HSVtoRGB(fmodf(h+140,360),.9f,1); headInner=HSVtoRGB(h,.45f,1); tailInner=HSVtoRGB(fmodf(h+140,360),.45f,1); break; }
        case 3: { float h = fmodf(t*40,60); headOuter=HSVtoRGB(h,.95f,1); tailOuter=HSVtoRGB(fmodf(h+35,60),.95f,1); headInner=HSVtoRGB(h,.5f,1); tailInner=HSVtoRGB(fmodf(h+35,60),.5f,1); break; }
        case 4: { float h = 180+fmodf(t*40,120); headOuter=HSVtoRGB(h,.9f,1); tailOuter=HSVtoRGB(fmodf(h+70,360),.9f,1); headInner=HSVtoRGB(h,.45f,1); tailInner=HSVtoRGB(fmodf(h+70,360),.45f,1); break; }
        case 5: { float p = .6f+.4f*sinf(t*4); D2D1_COLOR_F c=g_customColor; headOuter=D2D1::ColorF(c.r*p,c.g*p,c.b*p,1); tailOuter=D2D1::ColorF(c.r*p*.4f,c.g*p*.4f,c.b*p*.4f,1); headInner=LighterColor(headOuter,.6f); tailInner=LighterColor(tailOuter,.6f); break; }
        case 6: { float sn=fminf(velocity/60.0f,1.0f); float h=240-sn*240; headOuter=HSVtoRGB(h,.9f,1); tailOuter=HSVtoRGB(fmodf(h+60,360),.7f,.8f); headInner=HSVtoRGB(h,.4f,1); tailInner=HSVtoRGB(fmodf(h+60,360),.3f,.9f); break; }
        case 7: headOuter=g_gradColors[0]; tailOuter=g_gradColors[1]; headInner=LighterColor(g_gradColors[0]); tailInner=LighterColor(g_gradColors[1]); break;
        case 8: { float f=.85f+.15f*sinf(t*15)*sinf(t*7.3f); headOuter=D2D1::ColorF(1*f,.9f*f,.2f,1); tailOuter=D2D1::ColorF(.7f,.1f,0,1); headInner=D2D1::ColorF(1,1,.7f,1); tailInner=D2D1::ColorF(.9f,.3f,0,1); break; }
        case 9: { float h1=140+30*sinf(t*.8f); float h2=280+40*sinf(t*.6f+1); headOuter=HSVtoRGB(h1,.8f,.9f); tailOuter=HSVtoRGB(h2,.8f,.9f); headInner=HSVtoRGB(190,.5f,1); tailInner=HSVtoRGB(fmodf(h2+30,360),.4f,1); break; }
        case 10: { D2D1_COLOR_F ec = g_cursorExtractedColor; headOuter = tailOuter = ec; headInner = tailInner = LighterColor(ec, 0.6f); break; }
        case 11: { D2D1_COLOR_F mixed = LerpColor(g_cursorExtractedColor, g_customColor, 0.5f); D2D1_COLOR_F mixedTail = LerpColor(g_cursorExtractedColor, g_gradColors[2], 0.5f); headOuter = mixed; tailOuter = mixedTail; headInner = LighterColor(mixed, 0.55f); tailInner = LighterColor(mixedTail, 0.55f); break; }
        default: headOuter = tailOuter = g_customColor; headInner = tailInner = LighterColor(g_customColor); break;
    }
    out.solidOuter = headOuter; out.solidInner = headInner;
    for (int i = 0; i < GRAD_STOPS; i++) {
        float ratio = (float)i / (GRAD_STOPS - 1);
        float alpha = 0.86f * powf(1.0f - ratio, 1.4f);
        if (mode == 7) {
            float phase = fmodf(ratio * 4.0f + t * 2.0f, 1.0f);
            bool stripe = phase < 0.5f;
            D2D1_COLOR_F co = stripe ? headOuter : tailOuter, ci = stripe ? headInner : tailInner;
            out.outer[i] = { ratio, D2D1::ColorF(co.r, co.g, co.b, alpha) };
            out.inner[i] = { ratio, D2D1::ColorF(ci.r, ci.g, ci.b, alpha) };
        } else if (mode == 8) {
            float fr = ratio * ratio;
            D2D1_COLOR_F co = LerpColor(headOuter, tailOuter, fr), ci = LerpColor(headInner, tailInner, fr);
            out.outer[i] = { ratio, D2D1::ColorF(co.r, co.g, co.b, alpha) };
            out.inner[i] = { ratio, D2D1::ColorF(ci.r, ci.g, ci.b, alpha) };
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
            out.outer[i] = { ratio, D2D1::ColorF(co.r, co.g, co.b, alpha) };
            out.inner[i] = { ratio, D2D1::ColorF(ci.r, ci.g, ci.b, alpha) };
        } else {
            D2D1_COLOR_F co = LerpColor(headOuter, tailOuter, ratio), ci = LerpColor(headInner, tailInner, ratio);
            out.outer[i] = { ratio, D2D1::ColorF(co.r, co.g, co.b, alpha) };
            out.inner[i] = { ratio, D2D1::ColorF(ci.r, ci.g, ci.b, alpha) };
        }
    }
}

static void ReleaseGradientBrushes() {
    if (g_pGradInnerBrush) { g_pGradInnerBrush->Release(); g_pGradInnerBrush = nullptr; }
    if (g_pGradOuterBrush) { g_pGradOuterBrush->Release(); g_pGradOuterBrush = nullptr; }
    if (g_pGradInnerStops) { g_pGradInnerStops->Release(); g_pGradInnerStops = nullptr; }
    if (g_pGradOuterStops) { g_pGradOuterStops->Release(); g_pGradOuterStops = nullptr; }
    s_gradValid = false;
}

static void UpdateColorBrushes(const GradData& data, D2D1_POINT_2F headPt, D2D1_POINT_2F tailPt) {
    if (g_pSolidOuterBrush) g_pSolidOuterBrush->SetColor(data.solidOuter);
    if (g_pSolidInnerBrush) g_pSolidInnerBrush->SetColor(data.solidInner);
    bool needRebuild = !s_gradValid || !GradApproxEq(s_cachedGrad, data);
    if (needRebuild && g_pDCRenderTarget) {
        ReleaseGradientBrushes();
        g_pDCRenderTarget->CreateGradientStopCollection(data.outer, GRAD_STOPS, &g_pGradOuterStops);
        g_pDCRenderTarget->CreateGradientStopCollection(data.inner, GRAD_STOPS, &g_pGradInnerStops);
        g_pDCRenderTarget->CreateLinearGradientBrush(D2D1::LinearGradientBrushProperties(headPt, tailPt), g_pGradOuterStops, &g_pGradOuterBrush);
        g_pDCRenderTarget->CreateLinearGradientBrush(D2D1::LinearGradientBrushProperties(headPt, tailPt), g_pGradInnerStops, &g_pGradInnerBrush);
        s_cachedGrad = data; s_gradValid = true;
    }
    if (g_pGradOuterBrush) { g_pGradOuterBrush->SetStartPoint(headPt); g_pGradOuterBrush->SetEndPoint(tailPt); }
    if (g_pGradInnerBrush) { g_pGradInnerBrush->SetStartPoint(headPt); g_pGradInnerBrush->SetEndPoint(tailPt); }
}

static inline float Rand01() { return rand() / (float)RAND_MAX; }
static inline float Hash01(int n) {
    uint32_t u = (uint32_t)n;
    u = (u << 13) ^ u;
    return (float)(((u * (u * u * 15731u + 789221u) + 1376312589u) & 0x7fffffffu) / 2147483647.0);
}
static void SpawnParticles(float x, float y, int count, float speedMin, float speedMax,
                           float sizeMin, float sizeMax, int lifeMin, int lifeMax,
                           D2D1_COLOR_F color, DWORD time, bool radial = false, int shapeType = -1) {
    D2D1_COLOR_F endCol = D2D1::ColorF(color.r * 0.25f, color.g * 0.25f, color.b * 0.25f, 1.0f);
    for (int i = 0; i < count; i++) {
        float angle = radial ? (i / (float)count * 6.28318f) : (Rand01() * 6.28318f);
        float speed = speedMin + Rand01() * (speedMax - speedMin);
        int st = shapeType;
        if (st < 0) {
            st = g_particleShape;
            if (st == 0) st = (int)(Rand01() * 3.0f) + 1; // random: 1=circle,2=star,3=hexagram
        }
        g_particles.push_back({ x, y, cosf(angle) * speed, sinf(angle) * speed,
            sizeMin + Rand01() * (sizeMax - sizeMin), time,
            lifeMin + (int)(Rand01() * (lifeMax - lifeMin)), color, endCol, st });
    }
}

// 沿路径获取指定比例（0=头，1=尾）的坐标
static D2D1_POINT_2F GetPointOnPath(const std::vector<D2D1_POINT_2F>& path, float ratio) {
    if (path.empty()) return D2D1::Point2F(0, 0);
    if (path.size() == 1) return path[0];
    if (ratio <= 0) return path[0];
    if (ratio >= 1) return path.back();
    float totalLen = 0;
    for (size_t i = 1; i < path.size(); i++) {
        float dx = path[i].x - path[i-1].x, dy = path[i].y - path[i-1].y;
        totalLen += sqrtf(dx*dx + dy*dy);
    }
    float targetDist = ratio * totalLen, acc = 0;
    for (size_t i = 1; i < path.size(); i++) {
        float dx = path[i].x - path[i-1].x, dy = path[i].y - path[i-1].y;
        float segLen = sqrtf(dx*dx + dy*dy);
        if (acc + segLen >= targetDist) {
            float t = segLen > 0 ? (targetDist - acc) / segLen : 0;
            return D2D1::Point2F(path[i-1].x + dx*t, path[i-1].y + dy*t);
        }
        acc += segLen;
    }
    return path.back();
}

static void WStrToUTF8(PCWSTR wstr, char* out, int outSize) {
    if (!wstr || outSize <= 0) { if (outSize > 0) out[0] = 0; return; }
    out[0] = 0;
    int written = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, out, outSize, NULL, NULL);
    if (written <= 0) out[0] = 0;
}

// ===================== 设置加载 =====================
void LoadSettings() {
    g_triggerVelocity = (float)Wh_GetIntSetting(L"trigger_velocity");
    g_stopVelocity = (float)Wh_GetIntSetting(L"stop_velocity");
    g_tailOffsetX = Wh_GetIntSetting(L"tail_offset_x");
    g_tailOffsetY = Wh_GetIntSetting(L"tail_offset_y");
    g_tailLength = Wh_GetIntSetting(L"tail_length");
    g_trailDelay = Wh_GetIntSetting(L"trail_delay");
    g_enableSmoothGradient = Wh_GetIntSetting(L"enable_smooth_gradient") != 0;
    {
        PCWSTR fstr = Wh_GetStringSetting(L"fadeout_mode");
        if (fstr) {
            if (wcscmp(fstr, L"hard") == 0) g_fadeoutMode = 0;
            else if (wcscmp(fstr, L"accelerate") == 0) g_fadeoutMode = 1;
            else g_fadeoutMode = 2;
            Wh_FreeStringSetting(fstr);
        }
    }
    g_enableSpeedResponse = Wh_GetIntSetting(L"enable_speed_response") != 0;
    g_enhancedGlow = Wh_GetIntSetting(L"enhanced_glow") != 0;
    g_enableHeadHighlight = Wh_GetIntSetting(L"enable_head_highlight") != 0;
    g_enableTrailShadow = Wh_GetIntSetting(L"enable_trail_shadow") != 0;
    g_dotsMultiplier = Wh_GetIntSetting(L"dots_multiplier");
    g_waveAmplitude = Wh_GetIntSetting(L"wave_amplitude");
    g_waveFrequency = Wh_GetIntSetting(L"wave_frequency");
    g_enableGlow = Wh_GetIntSetting(L"enable_glow") != 0;
    g_glowIntensity = Wh_GetIntSetting(L"glow_intensity");
    g_particleDensity = Wh_GetIntSetting(L"particle_density");
    g_particleInterval = Wh_GetIntSetting(L"particle_interval");
    g_particleAccel = Wh_GetIntSetting(L"particle_acceleration") != 0;
    g_particleOriginRatio = Wh_GetIntSetting(L"particle_origin_ratio");
    int attrVal = Wh_GetIntSetting(L"particle_attraction");
    if (attrVal < 0) attrVal = 0; if (attrVal > 100) attrVal = 100;
    // 非线性映射：低区间精细区分，高区间压缩。value=1→0.0008（用户舒适值），value=5→0.0065，value=40+→上限0.08
    g_particleAttraction = attrVal > 0 ? fminf(0.0008f * powf((float)attrVal, 1.3f), 0.08f) : 0.0f;
    g_enableParticleRepel = Wh_GetIntSetting(L"enable_particle_repel") != 0;
    g_particleRepelRadius = Wh_GetIntSetting(L"particle_repel_radius");
    if (g_particleRepelRadius < 5) g_particleRepelRadius = 5; if (g_particleRepelRadius > 100) g_particleRepelRadius = 100;
    int repelVal = Wh_GetIntSetting(L"particle_repel_force");
    if (repelVal < 0) repelVal = 0; if (repelVal > 100) repelVal = 100;
    g_particleRepelForce = (repelVal / 100.0f) * 3.0f; // 0-100 → 0-3.0 像素/帧
    PCWSTR pstr = Wh_GetStringSetting(L"particle_mode");
    if (pstr) {
        if (wcscmp(pstr, L"off") == 0) g_particleMode = 0;
        else if (wcscmp(pstr, L"always") == 0) g_particleMode = 2;
        else g_particleMode = 1;
        Wh_FreeStringSetting(pstr);
    }
    PCWSTR pshape = Wh_GetStringSetting(L"particle_shape");
    if (pshape) {
        if (wcscmp(pshape, L"circle") == 0) g_particleShape = 1;
        else if (wcscmp(pshape, L"star") == 0) g_particleShape = 2;
        else if (wcscmp(pshape, L"hexagram") == 0) g_particleShape = 3;
        else g_particleShape = 0; // random
        Wh_FreeStringSetting(pshape);
    }
    g_enableClickStarburst = Wh_GetIntSetting(L"enable_click_starburst") != 0;
    g_starburstCount = Wh_GetIntSetting(L"starburst_count");
    g_enableClickEffect = Wh_GetIntSetting(L"enable_click_effect") != 0;
    g_clickMaxRadius = Wh_GetIntSetting(L"click_max_radius");
    g_clickDuration = Wh_GetIntSetting(L"click_duration");

    PCWSTR str = Wh_GetStringSetting(L"trail_shape");
    if (str) {
        if (wcscmp(str, L"dots") == 0) g_trailShape = 1;
        else if (wcscmp(str, L"function") == 0) g_trailShape = 2;
        else if (wcscmp(str, L"wave") == 0) g_trailShape = 3;
        else g_trailShape = 0;
        Wh_FreeStringSetting(str);
    }
    str = Wh_GetStringSetting(L"function_preset");
    if (str) {
        if (wcscmp(str, L"damped") == 0) g_functionPreset = 1;
        else if (wcscmp(str, L"beat") == 0) g_functionPreset = 2;
        else if (wcscmp(str, L"swirl") == 0) g_functionPreset = 3;
        else if (wcscmp(str, L"custom") == 0) g_functionPreset = 4;
        else g_functionPreset = 0;
        Wh_FreeStringSetting(str);
    }
    str = Wh_GetStringSetting(L"custom_function");
    if (str) { WStrToUTF8(str, g_customFunction, 256); Wh_FreeStringSetting(str); }
    str = Wh_GetStringSetting(L"color_mode");
    if (str) {
        if (wcscmp(str, L"single") == 0) g_colorMode = 0;
        else if (wcscmp(str, L"gradient") == 0) g_colorMode = 1;
        else if (wcscmp(str, L"rainbow") == 0) g_colorMode = 2;
        else if (wcscmp(str, L"warm") == 0) g_colorMode = 3;
        else if (wcscmp(str, L"cool") == 0) g_colorMode = 4;
        else if (wcscmp(str, L"neon") == 0) g_colorMode = 5;
        else if (wcscmp(str, L"velocity") == 0) g_colorMode = 6;
        else if (wcscmp(str, L"stripes") == 0) g_colorMode = 7;
        else if (wcscmp(str, L"fire") == 0) g_colorMode = 8;
        else if (wcscmp(str, L"aurora") == 0) g_colorMode = 9;
        else if (wcscmp(str, L"cursor_extract") == 0) g_colorMode = 10;
        else if (wcscmp(str, L"cursor_mix") == 0) g_colorMode = 11;
        else g_colorMode = 0;
        Wh_FreeStringSetting(str);
    }
    str = Wh_GetStringSetting(L"particle_origin");
    if (str) {
        if (wcscmp(str, L"head") == 0) g_particleOrigin = 0;
        else if (wcscmp(str, L"middle") == 0) g_particleOrigin = 1;
        else if (wcscmp(str, L"custom") == 0) g_particleOrigin = 3;
        else g_particleOrigin = 2; // tail
        Wh_FreeStringSetting(str);
    }
    g_cursorColorShift = Wh_GetIntSetting(L"enable_cursor_color_shift") != 0;
    str = Wh_GetStringSetting(L"custom_color");
    if (str) { g_customColor = ParseHexColor(str, D2D1::ColorF(0, .75f, 1)); Wh_FreeStringSetting(str); }
    str = Wh_GetStringSetting(L"gradient_colors");
    if (str) { ParseGradientColors(str, g_gradColors); Wh_FreeStringSetting(str); }

    if (g_triggerVelocity <= 0) g_triggerVelocity = 25;
    if (g_stopVelocity <= 0) g_stopVelocity = 10;
    if (g_tailLength < 2) g_tailLength = 10;
    if (g_trailDelay < 0) g_trailDelay = 0; if (g_trailDelay > 10) g_trailDelay = 10;
    if (g_dotsMultiplier < 1) g_dotsMultiplier = 1; if (g_dotsMultiplier > 5) g_dotsMultiplier = 5;
    if (g_waveAmplitude < 1) g_waveAmplitude = 1; if (g_waveAmplitude > 40) g_waveAmplitude = 40;
    if (g_waveFrequency < 3) g_waveFrequency = 3; if (g_waveFrequency > 60) g_waveFrequency = 60;
    if (g_glowIntensity < 0) g_glowIntensity = 0; if (g_glowIntensity > 100) g_glowIntensity = 100;
    if (g_particleDensity < 1) g_particleDensity = 1; if (g_particleDensity > 10) g_particleDensity = 10;
    if (g_particleInterval < 10) g_particleInterval = 10; if (g_particleInterval > 2000) g_particleInterval = 2000;
    if (g_particleOriginRatio < 0) g_particleOriginRatio = 0; if (g_particleOriginRatio > 100) g_particleOriginRatio = 100;
    if (g_starburstCount < 4) g_starburstCount = 4; if (g_starburstCount > 20) g_starburstCount = 20;
    if (g_clickMaxRadius <= 0) g_clickMaxRadius = 40;
    if (g_clickDuration <= 0) g_clickDuration = 300;

    const char* presetExprs[] = {
        "sin(d * 0.15) * 8",
        "sin(d * 0.25) * exp(0 - t * 2.5) * 10",
        "abs(sin(d * 0.22)) ^ 3 * 12",
        "sin(d * 0.1 + time * 2) * cos(d * 0.05) * 9",
    };
    if (g_functionPreset == 4) CompileExpression(g_customFunction);
    else CompileExpression(presetExprs[g_functionPreset]);

    s_gradValid = false;
    g_lagInited = false;
    g_fadeAlpha = 1.0f;
}

// ===================== 游戏检测 =====================
bool IsGameRunning() {
    HWND hwnd = GetForegroundWindow();
    if (!hwnd || hwnd == GetDesktopWindow()) return false;
    static HWND s_pm = FindWindowW(L"Progman", NULL), s_ww = FindWindowW(L"WorkerW", NULL);
    if (hwnd == s_pm || hwnd == s_ww) return false;
    QUERY_USER_NOTIFICATION_STATE state;
    if (SUCCEEDED(SHQueryUserNotificationState(&state)) && state == QUNS_RUNNING_D3D_FULL_SCREEN) return true;
    RECT rcApp; GetWindowRect(hwnd, &rcApp);
    HMONITOR hMon = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi = { sizeof(mi) };
    if (GetMonitorInfo(hMon, &mi)) {
        bool fs = rcApp.left <= mi.rcMonitor.left && rcApp.top <= mi.rcMonitor.top && rcApp.right >= mi.rcMonitor.right && rcApp.bottom >= mi.rcMonitor.bottom;
        if (fs) {
            RECT rcClip;
            if (GetClipCursor(&rcClip)) {
                int vW = GetSystemMetrics(SM_CXVIRTUALSCREEN), vH = GetSystemMetrics(SM_CYVIRTUALSCREEN);
                if ((rcClip.right - rcClip.left) < vW || (rcClip.bottom - rcClip.top) < vH) return true;
            }
            CURSORINFO ci = { sizeof(CURSORINFO) };
            if (GetCursorInfo(&ci) && ci.flags == 0) return true;
        }
    }
    return false;
}

// 取色互补色偏移：色相+180°，增强饱和度和亮度，确保拖尾在任何背景上醒目
static D2D1_COLOR_F ShiftToComplementary(D2D1_COLOR_F c) {
    float mx = fmaxf(fmaxf(c.r, c.g), c.b);
    float mn = fminf(fminf(c.r, c.g), c.b);
    float h, s, v = mx;
    float d = mx - mn;
    s = (mx == 0.0f) ? 0.0f : d / mx;
    if (d == 0.0f) h = 0.0f;
    else if (mx == c.r) h = fmodf((c.g - c.b) / d, 6.0f);
    else if (mx == c.g) h = (c.b - c.r) / d + 2.0f;
    else h = (c.r - c.g) / d + 4.0f;
    h *= 60.0f; if (h < 0.0f) h += 360.0f;
    h = fmodf(h + 180.0f, 360.0f);  // 互补色
    s = fmaxf(s, 0.55f);             // 饱和度保底
    v = fmaxf(v, 0.72f);             // 亮度保底
    return HSVtoRGB(h, s, v);
}

static void ExtractCursorColor(POINT pt, DWORD dwTime) {
    if (dwTime - s_lastColorExtract < 40) return;
    s_lastColorExtract = dwTime;
    HDC hdcScreen = GetDC(NULL);
    if (hdcScreen) {
        COLORREF col = GetPixel(hdcScreen, pt.x + 2, pt.y + 4);
        ReleaseDC(NULL, hdcScreen);
        if (col != CLR_INVALID) {
            D2D1_COLOR_F newColor = D2D1::ColorF(GetRValue(col) / 255.0f, GetGValue(col) / 255.0f, GetBValue(col) / 255.0f, 1.0f);
            if (g_cursorColorShift) newColor = ShiftToComplementary(newColor);
            g_cursorExtractedColor = LerpColor(g_cursorExtractedColor, newColor, 0.22f);
        }
    }
}

// ===================== 轨迹变形 =====================
static void ApplyWaveDeformation(std::vector<D2D1_POINT_2F>& pts, DWORD dwTime) {
    if (pts.size() < 3) return;
    float freq = g_waveFrequency / 100.0f, amp = (float)g_waveAmplitude;
    float phase = dwTime * 0.004f, dist = 0;
    std::vector<D2D1_POINT_2F> result; result.reserve(pts.size());
    for (size_t i = 0; i < pts.size(); ++i) {
        if (i > 0) { float ddx = pts[i].x - pts[i-1].x, ddy = pts[i].y - pts[i-1].y; dist += sqrtf(ddx*ddx + ddy*ddy); }
        float tdx, tdy;
        if (i == 0) { tdx = pts[1].x - pts[0].x; tdy = pts[1].y - pts[0].y; }
        else if (i == pts.size()-1) { tdx = pts[i].x - pts[i-1].x; tdy = pts[i].y - pts[i-1].y; }
        else { tdx = pts[i+1].x - pts[i-1].x; tdy = pts[i+1].y - pts[i-1].y; }
        float tl = sqrtf(tdx*tdx + tdy*tdy);
        if (tl > 0.001f) { tdx /= tl; tdy /= tl; } else { tdx = 1; tdy = 0; }
        float nx = -tdy, ny = tdx;
        float taper = 1.0f - (float)i / (pts.size() - 1) * 0.65f;
        float wave = sinf(dist * freq + phase) * amp * taper;
        result.push_back(D2D1::Point2F(pts[i].x + nx * wave, pts[i].y + ny * wave));
    }
    pts = result;
}

static void ApplyFunctionDeformation(std::vector<D2D1_POINT_2F>& pts, DWORD dwTime) {
    if (pts.size() < 3 || !g_exprValid) return;
    float time = dwTime / 1000.0f, dist = 0;
    float totalDist = 0;
    for (size_t i = 1; i < pts.size(); i++) {
        float ddx = pts[i].x - pts[i-1].x, ddy = pts[i].y - pts[i-1].y;
        totalDist += sqrtf(ddx*ddx + ddy*ddy);
    }
    std::vector<D2D1_POINT_2F> result; result.reserve(pts.size());
    for (size_t i = 0; i < pts.size(); ++i) {
        if (i > 0) { float ddx = pts[i].x - pts[i-1].x, ddy = pts[i].y - pts[i-1].y; dist += sqrtf(ddx*ddx + ddy*ddy); }
        float tdx, tdy;
        if (i == 0) { tdx = pts[1].x - pts[0].x; tdy = pts[1].y - pts[0].y; }
        else if (i == pts.size()-1) { tdx = pts[i].x - pts[i-1].x; tdy = pts[i].y - pts[i-1].y; }
        else { tdx = pts[i+1].x - pts[i-1].x; tdy = pts[i+1].y - pts[i-1].y; }
        float tl = sqrtf(tdx*tdx + tdy*tdy);
        if (tl > 0.001f) { tdx /= tl; tdy /= tl; } else { tdx = 1; tdy = 0; }
        float nx = -tdy, ny = tdx;
        float t = totalDist > 0 ? dist / totalDist : 0;
        float offset = EvalExpression(t, dist, time);
        if (offset > 60) offset = 60; if (offset < -60) offset = -60;
        result.push_back(D2D1::Point2F(pts[i].x + nx * offset, pts[i].y + ny * offset));
    }
    pts = result;
}

struct DotInfo { D2D1_POINT_2F pos; float radius; D2D1_COLOR_F outer; D2D1_COLOR_F inner; float alpha; };

// ===================== 主绘制循环 =====================
VOID CALLBACK SmearTimerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime) {
    POINT pt; GetCursorPos(&pt);
    int dx = pt.x - g_lastPos.x, dy = pt.y - g_lastPos.y;
    float velocity = sqrtf((float)(dx*dx + dy*dy));
    g_lastPos = pt;

    if (g_colorMode == 10 || g_colorMode == 11) ExtractCursorColor(pt, dwTime);

    POINT renderPos = pt;
    if (g_trailDelay > 0) {
        if (!g_lagInited) { g_lagPos = pt; g_lagInited = true; }
        float ease = 1.0f - (g_trailDelay / 14.0f);
        if (ease < 0.12f) ease = 0.12f;
        g_lagPos.x += (int)((pt.x - g_lagPos.x) * ease);
        g_lagPos.y += (int)((pt.y - g_lagPos.y) * ease);
        renderPos = g_lagPos;
    } else g_lagInited = false;

    GradData cols;
    ComputeColors(g_colorMode, dwTime, velocity, cols);

    float widthMul = 1.0f;
    if (g_enableSpeedResponse) {
        float sf = fminf(velocity / 80.0f, 1.0f);
        float accel = fabsf(velocity - g_prevVelocity);
        float af = fminf(accel / 30.0f, 1.0f);
        widthMul = 1.0f + sf * 0.3f + af * 0.15f;
    }

    int vX = GetSystemMetrics(SM_XVIRTUALSCREEN), vY = GetSystemMetrics(SM_YVIRTUALSCREEN);

    bool lDown = (GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0;
    bool rDown = (GetAsyncKeyState(VK_RBUTTON) & 0x8000) != 0;
    if (g_enableClickEffect) {
        if (lDown && !g_prevLButton) g_ripples.push_back({ pt, dwTime });
        if (rDown && !g_prevRButton) g_ripples.push_back({ pt, dwTime });
    }
    if (g_enableClickStarburst) {
        if ((lDown && !g_prevLButton) || (rDown && !g_prevRButton)) {
            SpawnParticles((float)(pt.x - vX), (float)(pt.y - vY), g_starburstCount, 2.5f, 5.5f, 1.5f, 3.0f, 250, 450, cols.solidOuter, dwTime, true);
        }
    }
    g_prevLButton = lDown; g_prevRButton = rDown;

    if (!g_ripples.empty())
        g_ripples.erase(std::remove_if(g_ripples.begin(), g_ripples.end(),
            [&](const Ripple& r) { return dwTime - r.startTime > (DWORD)g_clickDuration; }), g_ripples.end());

    static DWORD lastFsCheck = 0;
    static bool isGameCached = false, isSmearing = false;
    static int lowVelFrames = 0, needsClear = false, fadeoutFrame = 0;
    if (dwTime - lastFsCheck > 500) { isGameCached = IsGameRunning(); lastFsCheck = dwTime; }

    int vW = GetSystemMetrics(SM_CXVIRTUALSCREEN), vH = GetSystemMetrics(SM_CYVIRTUALSCREEN) - 1;

    if (isGameCached) {
        if (isSmearing || !g_history.empty() || !g_ripples.empty() || !g_particles.empty() || needsClear) {
            isSmearing = false; g_history.clear(); g_ripples.clear(); g_particles.clear();
            g_fadeAlpha = 1.0f; fadeoutFrame = 0;
        } else return;
    } else {
        if (velocity > g_triggerVelocity && !isSmearing) { isSmearing = true; lowVelFrames = 0; }
        else if (velocity < g_stopVelocity && isSmearing) { lowVelFrames++; if (lowVelFrames > 2) isSmearing = false; }
        else if (velocity >= g_stopVelocity && isSmearing) lowVelFrames = 0;
        if (isSmearing) {
            POINT np = { renderPos.x - vX, renderPos.y - vY };
            g_history.push_front(np);
            while (g_history.size() > (size_t)g_tailLength) g_history.pop_back();
            fadeoutFrame = 0;
            // 移动时 alpha 快速恢复到 1.0
            g_fadeAlpha += (1.0f - g_fadeAlpha) * 0.4f;
            if (g_fadeAlpha > 1.0f) g_fadeAlpha = 1.0f;
        } else {
            // ===== 淡出模式：硬截断 / 加速收缩 / 软截断 =====
            switch (g_fadeoutMode) {
                case 0: // 硬截断：立即清除
                    g_history.clear();
                    g_fadeAlpha = 1.0f;
                    fadeoutFrame = 0;
                    break;
                case 1: { // 加速收缩：前慢后快，每帧 pop 数量平缓递增
                    fadeoutFrame++;
                    int popCount = 1 + fadeoutFrame / 5; // 帧1-5:1, 6-10:2, 11-15:3...
                    if (popCount > 4) popCount = 4; // 上限 4，避免后期飞太快
                    for (int i = 0; i < popCount && !g_history.empty(); i++)
                        g_history.pop_back();
                    if (g_history.size() <= 1) g_history.clear();
                    g_fadeAlpha = 1.0f;
                    break;
                }
                case 2: // 软截断：基于剩余长度的淡出曲线 + 拖尾收缩同步，杜绝末端硬切
                    fadeoutFrame++;
                    if (!g_history.empty()) g_history.pop_back();
                    // alpha 由剩余点数比例决定：点数越少越透明，pow 曲线让头部保持饱满、尾部加速消失
                    if (g_history.size() >= 2) {
                        float lenRatio = (float)g_history.size() / (float)g_tailLength;
                        g_fadeAlpha = powf(lenRatio, 1.4f);
                    } else {
                        g_fadeAlpha = 0; g_history.clear();
                    }
                    break;
            }
        }
    }

    // ===== 提前计算 smoothed 路径（粒子释放和渲染共用）=====
    std::vector<D2D1_POINT_2F> smoothed;
    bool havePath = (g_history.size() >= 2);
    if (havePath) {
        for (auto& p : g_history) smoothed.push_back(D2D1::Point2F((float)p.x + g_tailOffsetX, (float)p.y + g_tailOffsetY));
        for (int iter = 0; iter < 2; ++iter) {
            if (smoothed.size() < 3) break;
            std::vector<D2D1_POINT_2F> ns;
            ns.push_back(smoothed.front());
            for (size_t i = 0; i < smoothed.size() - 1; ++i) {
                D2D1_POINT_2F p0 = smoothed[i], p1 = smoothed[i+1];
                ns.push_back(D2D1::Point2F(.75f*p0.x+.25f*p1.x, .75f*p0.y+.25f*p1.y));
                ns.push_back(D2D1::Point2F(.25f*p0.x+.75f*p1.x, .25f*p0.y+.75f*p1.y));
            }
            ns.push_back(smoothed.back());
            smoothed = ns;
        }
        if (g_trailShape == 2) ApplyFunctionDeformation(smoothed, dwTime);
        else if (g_trailShape == 3) ApplyWaveDeformation(smoothed, dwTime);
    }

    // ===== 粒子释放（基于 smoothed 路径的指定位置）=====
    if (g_particleMode > 0 && havePath &&
        dwTime - g_lastParticleTime >= (DWORD)g_particleInterval) {
        bool spawnOK = (g_particleMode == 1) ? !isSmearing : true;
        if (spawnOK) {
            float ratio;
            switch (g_particleOrigin) {
                case 0: ratio = 0.0f; break;  // head
                case 1: ratio = 0.5f; break;  // middle
                case 3: ratio = g_particleOriginRatio / 100.0f; break; // custom
                default: ratio = 1.0f; break; // tail
            }
            D2D1_POINT_2F origin = GetPointOnPath(smoothed, ratio);
            float speedMul = 1.0f;
            if (g_particleAccel) {
                float accel = velocity - g_prevVelocity;
                speedMul = 1.0f + fabsf(accel) * 0.07f;
                if (speedMul > 3.5f) speedMul = 3.5f;
            }
            SpawnParticles(origin.x, origin.y,
                g_particleDensity, 0.3f * speedMul, 2.0f * speedMul, 0.8f, 2.2f, 300, 700, cols.solidOuter, dwTime);
            g_lastParticleTime = dwTime;
        }
    }
    // 粒子总数上限，防止参数拉满时性能崩溃
    if (g_particles.size() > 200) {
        g_particles.erase(g_particles.begin(), g_particles.begin() + (g_particles.size() - 200));
    }
    g_prevVelocity = velocity;

    // ===== 粒子物理：摩擦 + 光标排斥力 + 随机扰动 + 全程吸附光标 =====
    float attractTargetX = (float)(pt.x - vX + g_tailOffsetX);
    float attractTargetY = (float)(pt.y - vY + g_tailOffsetY);
    for (auto& p : g_particles) {
        p.vx *= 0.93f; p.vy *= 0.93f;
        // 光标周围排斥力：粒子进入范围后被径向弹开 + 随机方向扰乱
        if (g_enableParticleRepel && g_particleRepelForce > 0) {
            float rdx = p.x - attractTargetX;
            float rdy = p.y - attractTargetY;
            float rdist = sqrtf(rdx * rdx + rdy * rdy);
            if (rdist < (float)g_particleRepelRadius && rdist > 0.5f) {
                float nx = rdx / rdist, ny = rdy / rdist;
                float falloff = 1.0f - rdist / (float)g_particleRepelRadius; // 越靠近光标排斥越强
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
        p.x += p.vx; p.y += p.vy;
        if (g_particleAttraction > 0) {
            p.x += (attractTargetX - p.x) * g_particleAttraction;
            p.y += (attractTargetY - p.y) * g_particleAttraction;
        }
    }
    if (!g_particles.empty())
        g_particles.erase(std::remove_if(g_particles.begin(), g_particles.end(),
            [&](const Particle& p) { return dwTime - p.startTime > (DWORD)p.lifetime; }), g_particles.end());

    bool tailVisible = (isSmearing || g_history.size() >= 2) && g_fadeAlpha > 0.02f;
    bool isDrawing = tailVisible || !g_ripples.empty() || !g_particles.empty();

    static bool isWindowVisible = true;
    static int hideDelayCounter = 0;
    if (isDrawing) {
        hideDelayCounter = 0;
        if (!isWindowVisible) { ShowWindow(hwnd, SW_SHOWNA); isWindowVisible = true; }
    } else if (!needsClear) {
        hideDelayCounter++;
        if (hideDelayCounter >= 3 && isWindowVisible) { ShowWindow(hwnd, SW_HIDE); isWindowVisible = false; }
    } else {
        hideDelayCounter = 0;
    }
    if (!isDrawing && !needsClear) return;

    HDC hdcScreen = GetDC(NULL);
    if (!g_hBitmap || g_cachedVW != vW || g_cachedVH != vH) {
        if (g_hBitmap) DeleteObject(g_hBitmap);
        if (g_hdcMem) DeleteDC(g_hdcMem);
        g_hdcMem = CreateCompatibleDC(hdcScreen);
        g_hBitmap = CreateCompatibleBitmap(hdcScreen, vW, vH);
        SelectObject(g_hdcMem, g_hBitmap);
        g_cachedVW = vW; g_cachedVH = vH;
        if (g_pDCRenderTarget) { g_pDCRenderTarget->Release(); g_pDCRenderTarget = nullptr; ReleaseGradientBrushes(); }
    }
    if (!g_pDCRenderTarget && g_pD2DFactory) {
        if (g_pSolidOuterBrush) { g_pSolidOuterBrush->Release(); g_pSolidOuterBrush = nullptr; }
        if (g_pSolidInnerBrush) { g_pSolidInnerBrush->Release(); g_pSolidInnerBrush = nullptr; }
        if (g_pShadowBrush) { g_pShadowBrush->Release(); g_pShadowBrush = nullptr; }
        ReleaseGradientBrushes();
        D2D1_RENDER_TARGET_PROPERTIES props = D2D1::RenderTargetProperties(
            D2D1_RENDER_TARGET_TYPE_DEFAULT,
            D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED),
            0, 0, D2D1_RENDER_TARGET_USAGE_NONE, D2D1_FEATURE_LEVEL_DEFAULT);
        g_pD2DFactory->CreateDCRenderTarget(&props, &g_pDCRenderTarget);
        if (g_pDCRenderTarget) {
            g_pDCRenderTarget->SetAntialiasMode(D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
            g_pDCRenderTarget->CreateSolidColorBrush(D2D1::ColorF(0,0,0,1), &g_pSolidOuterBrush);
            g_pDCRenderTarget->CreateSolidColorBrush(D2D1::ColorF(1,1,1,1), &g_pSolidInnerBrush);
            g_pDCRenderTarget->CreateSolidColorBrush(D2D1::ColorF(0,0,0,0.18f), &g_pShadowBrush);
        }
    }
    if (!g_pDCRenderTarget) { ReleaseDC(NULL, hdcScreen); return; }

    RECT rc = { 0, 0, vW, vH };
    g_pDCRenderTarget->BindDC(g_hdcMem, &rc);
    g_pDCRenderTarget->BeginDraw();
    g_pDCRenderTarget->Clear(D2D1::ColorF(0, 0, 0, 0));

    // ===== 粒子（带微发光 + 多形状 + 颜色渐变）=====
    if (!g_particles.empty()) {
        bool particleFastPath = g_particles.size() > 100; // 粒子过多时降级为圆形，提升性能
        for (auto& p : g_particles) {
            float progress = (float)(dwTime - p.startTime) / p.lifetime;
            if (progress < 0 || progress >= 1) continue;
            float lifeAlpha = (1.0f - progress);
            // 颜色随生命周期从起始色渐变到结束色（参考 Mouse-Trail 项目）
            D2D1_COLOR_F pc = D2D1::ColorF(
                p.color.r + (p.endColor.r - p.color.r) * progress,
                p.color.g + (p.endColor.g - p.color.g) * progress,
                p.color.b + (p.endColor.b - p.color.b) * progress,
                1.0f);
            D2D1_POINT_2F pp = D2D1::Point2F(p.x, p.y);
            // 微发光光晕（始终圆形）
            g_pSolidOuterBrush->SetColor(pc);
            g_pSolidOuterBrush->SetOpacity(lifeAlpha * 0.18f);
            g_pDCRenderTarget->FillEllipse(D2D1::Ellipse(pp, p.size * 2.5f, p.size * 2.5f), g_pSolidOuterBrush);
            // 主体：按形状绘制（粒子过多时自动降级为圆形）
            g_pSolidOuterBrush->SetOpacity(lifeAlpha * 0.72f);
            if (!particleFastPath && p.shapeType == 2 && g_pStarGeom) {
                D2D1_MATRIX_3X2_F oldT;
                g_pDCRenderTarget->GetTransform(&oldT);
                g_pDCRenderTarget->SetTransform(D2D1::Matrix3x2F::Scale(p.size, p.size) * D2D1::Matrix3x2F::Translation(p.x, p.y));
                g_pDCRenderTarget->FillGeometry(g_pStarGeom, g_pSolidOuterBrush);
                g_pDCRenderTarget->SetTransform(oldT);
            } else if (!particleFastPath && p.shapeType == 3 && g_pHexagramGeom) {
                D2D1_MATRIX_3X2_F oldT;
                g_pDCRenderTarget->GetTransform(&oldT);
                g_pDCRenderTarget->SetTransform(D2D1::Matrix3x2F::Scale(p.size, p.size) * D2D1::Matrix3x2F::Translation(p.x, p.y));
                g_pDCRenderTarget->FillGeometry(g_pHexagramGeom, g_pSolidOuterBrush);
                g_pDCRenderTarget->SetTransform(oldT);
            } else {
                g_pDCRenderTarget->FillEllipse(D2D1::Ellipse(pp, p.size, p.size), g_pSolidOuterBrush);
            }
        }
        g_pSolidOuterBrush->SetOpacity(1.0f);
        needsClear = true;
    }

    // ===== 拖尾（复用已计算的 smoothed 路径）=====
    if (tailVisible && havePath) {
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
                float ddx = smoothed[i].x - smoothed[i-1].x, ddy = smoothed[i].y - smoothed[i-1].y;
                totalLen += sqrtf(ddx*ddx + ddy*ddy);
            }
            float mult = (float)g_dotsMultiplier;
            float spacing = 2.4f / mult;
            float maxR = 9.0f / sqrtf(mult) * widthMul;
            int dotCount = (int)(totalLen / spacing); if (dotCount < 3) dotCount = 3; if (dotCount > 150) dotCount = 150;
            std::vector<DotInfo> dots; dots.reserve(dotCount + 1);
            for (int di = 0; di <= dotCount; di++) {
                float frac = (float)di / dotCount;
                float targetDist = frac * totalLen, acc = 0;
                D2D1_POINT_2F pos = smoothed[0];
                float nx = 0, ny = 1;
                for (size_t i = 1; i < smoothed.size(); i++) {
                    float ddx = smoothed[i].x - smoothed[i-1].x, ddy = smoothed[i].y - smoothed[i-1].y;
                    float segLen = sqrtf(ddx*ddx + ddy*ddy);
                    if (acc + segLen >= targetDist) {
                        float t = segLen > 0 ? (targetDist - acc) / segLen : 0;
                        pos = D2D1::Point2F(smoothed[i-1].x + ddx*t, smoothed[i-1].y + ddy*t);
                        if (segLen > 0.001f) { nx = -ddy/segLen; ny = ddx/segLen; }
                        break;
                    }
                    acc += segLen; pos = smoothed[i];
                    if (segLen > 0.001f) { nx = -ddy/segLen; ny = ddx/segLen; }
                }
                float sizeJit = 0.78f + Hash01(di * 7 + 1) * 0.44f;
                float opJit = 0.65f + Hash01(di * 13 + 5) * 0.55f;
                float posJit = (Hash01(di * 3 + 9) - 0.5f) * 2.5f;
                float dr = maxR * powf(1.0f - frac, 1.6f) * sizeJit;
                if (dr < 0.3f) continue;
                pos.x += nx * posJit; pos.y += ny * posJit;
                int idx = (int)(frac * (GRAD_STOPS-1) + 0.5f); if (idx >= GRAD_STOPS) idx = GRAD_STOPS-1;
                D2D1_COLOR_F dco = cols.outer[idx].color, dci = cols.inner[idx].color;
                float da = 0.86f * powf(1.0f - frac, 1.4f) * opJit * fa;
                if (g_enableSmoothGradient) da *= (1.0f - frac * 0.3f);
                dots.push_back({ pos, dr, dco, dci, da });
            }
            if (g_enableTrailShadow && !dots.empty()) {
                g_pShadowBrush->SetOpacity(0.18f * fa);
                for (auto& d : dots) {
                    D2D1_POINT_2F sp = D2D1::Point2F(d.pos.x + SHADOW_DX, d.pos.y + SHADOW_DY);
                    g_pDCRenderTarget->FillEllipse(D2D1::Ellipse(sp, d.radius * 1.1f, d.radius * 1.1f), g_pShadowBrush);
                }
            }
            if (dots.size() >= 2) {
                for (size_t i = 0; i < dots.size() - 1; i++) {
                    float lineW = (dots[i].radius + dots[i+1].radius) * 0.65f;
                    if (lineW < 0.5f) continue;
                    D2D1_COLOR_F midColor = LerpColor(dots[i].outer, dots[i+1].outer, 0.5f);
                    g_pSolidOuterBrush->SetColor(midColor);
                    g_pSolidOuterBrush->SetOpacity((dots[i].alpha + dots[i+1].alpha) * 0.45f);
                    g_pDCRenderTarget->DrawLine(dots[i].pos, dots[i+1].pos, g_pSolidOuterBrush, lineW);
                }
                g_pSolidOuterBrush->SetOpacity(1.0f);
            }
            int glowCutoff = (int)(dots.size() * 0.6f);
            for (size_t di = 0; di < dots.size(); di++) {
                auto& d = dots[di];
                if (di < (size_t)glowCutoff) {
                if (useEnhancedGlow) {
                    g_pSolidOuterBrush->SetColor(d.outer);
                    g_pSolidOuterBrush->SetOpacity(glowO * 0.35f * d.alpha / fa);
                    g_pDCRenderTarget->FillEllipse(D2D1::Ellipse(d.pos, d.radius + glowR * 1.6f, d.radius + glowR * 1.6f), g_pSolidOuterBrush);
                    g_pSolidOuterBrush->SetOpacity(glowO * 0.7f * d.alpha / fa);
                    g_pDCRenderTarget->FillEllipse(D2D1::Ellipse(d.pos, d.radius + glowR * 0.7f, d.radius + glowR * 0.7f), g_pSolidOuterBrush);
                } else if (glowR > 0.1f) {
                    g_pSolidOuterBrush->SetColor(d.outer);
                    g_pSolidOuterBrush->SetOpacity(glowO * d.alpha / fa);
                    g_pDCRenderTarget->FillEllipse(D2D1::Ellipse(d.pos, d.radius + glowR * 0.7f, d.radius + glowR * 0.7f), g_pSolidOuterBrush);
                }
                }
                g_pSolidOuterBrush->SetColor(d.outer); g_pSolidOuterBrush->SetOpacity(d.alpha);
                g_pDCRenderTarget->FillEllipse(D2D1::Ellipse(d.pos, d.radius, d.radius), g_pSolidOuterBrush);
                g_pSolidInnerBrush->SetColor(d.inner); g_pSolidInnerBrush->SetOpacity(d.alpha * 0.9f);
                g_pDCRenderTarget->FillEllipse(D2D1::Ellipse(d.pos, d.radius * 0.58f, d.radius * 0.58f), g_pSolidInnerBrush);
            }
            g_pSolidOuterBrush->SetOpacity(1); g_pSolidInnerBrush->SetOpacity(1);
            needsClear = true;
        } else {
            // ===== 多边形带状 =====
            size_t sl = smoothed.size();
            std::vector<D2D1_POINT_2F> lo, ro, lc, rc, gl, gr, gl2, gr2;
            for (size_t i = 0; i < sl; ++i) {
                float ddx, ddy;
                if (i == 0) { ddx = smoothed[0].x - smoothed[1].x; ddy = smoothed[0].y - smoothed[1].y; }
                else if (i == sl-1) { ddx = smoothed[i-1].x - smoothed[i].x; ddy = smoothed[i-1].y - smoothed[i].y; }
                else { ddx = smoothed[i-1].x - smoothed[i+1].x; ddy = smoothed[i-1].y - smoothed[i+1].y; }
                float ln = sqrtf(ddx*ddx + ddy*ddy);
                if (ln > 0) { ddx /= ln; ddy /= ln; } else { ddx = 1; ddy = 0; }
                float nx = -ddy, ny = ddx, ratio = (float)i / (sl-1);
                float taper = powf(1.0f - ratio, 1.3f);
                float ow = 10.0f * taper * widthMul;
                float cw = 6.0f * taper * widthMul;
                if (i == sl-1) { ow = 0; cw = 0; }
                lo.push_back(D2D1::Point2F(smoothed[i].x + nx*ow, smoothed[i].y + ny*ow));
                ro.push_back(D2D1::Point2F(smoothed[i].x - nx*ow, smoothed[i].y - ny*ow));
                lc.push_back(D2D1::Point2F(smoothed[i].x + nx*cw, smoothed[i].y + ny*cw));
                rc.push_back(D2D1::Point2F(smoothed[i].x - nx*cw, smoothed[i].y - ny*cw));
                if (glowR > 0.1f) {
                    float gw = ow + glowR * (1.0f - ratio * 0.3f);
                    gl.push_back(D2D1::Point2F(smoothed[i].x + nx*gw, smoothed[i].y + ny*gw));
                    gr.push_back(D2D1::Point2F(smoothed[i].x - nx*gw, smoothed[i].y - ny*gw));
                    if (useEnhancedGlow) {
                        float gw2 = ow + glowR * 1.8f * (1.0f - ratio * 0.2f);
                        gl2.push_back(D2D1::Point2F(smoothed[i].x + nx*gw2, smoothed[i].y + ny*gw2));
                        gr2.push_back(D2D1::Point2F(smoothed[i].x - nx*gw2, smoothed[i].y - ny*gw2));
                    }
                }
            }
            if (g_enableTrailShadow && lo.size() >= 2) {
                ID2D1PathGeometry* pg = nullptr; ID2D1GeometrySink* ps = nullptr;
                g_pD2DFactory->CreatePathGeometry(&pg); pg->Open(&ps);
                ps->SetFillMode(D2D1_FILL_MODE_WINDING);
                ps->BeginFigure(D2D1::Point2F(lo[0].x + SHADOW_DX, lo[0].y + SHADOW_DY), D2D1_FIGURE_BEGIN_FILLED);
                for (size_t i = 1; i < lo.size(); ++i) ps->AddLine(D2D1::Point2F(lo[i].x + SHADOW_DX, lo[i].y + SHADOW_DY));
                for (int i = (int)ro.size()-1; i >= 0; --i) ps->AddLine(D2D1::Point2F(ro[i].x + SHADOW_DX, ro[i].y + SHADOW_DY));
                ps->EndFigure(D2D1_FIGURE_END_CLOSED); ps->Close(); ps->Release();
                g_pShadowBrush->SetOpacity(0.18f * fa);
                g_pDCRenderTarget->FillGeometry(pg, g_pShadowBrush); pg->Release();
            }
            if (useEnhancedGlow && gl2.size() >= 2) {
                ID2D1PathGeometry* pg = nullptr; ID2D1GeometrySink* ps = nullptr;
                g_pD2DFactory->CreatePathGeometry(&pg); pg->Open(&ps);
                ps->SetFillMode(D2D1_FILL_MODE_WINDING); ps->BeginFigure(gl2[0], D2D1_FIGURE_BEGIN_FILLED);
                for (size_t i = 1; i < gl2.size(); ++i) ps->AddLine(gl2[i]);
                for (int i = (int)gr2.size()-1; i >= 0; --i) ps->AddLine(gr2[i]);
                ps->EndFigure(D2D1_FIGURE_END_CLOSED); ps->Close(); ps->Release();
                g_pSolidOuterBrush->SetColor(cols.solidOuter); g_pSolidOuterBrush->SetOpacity(glowO * 0.3f * fa);
                g_pDCRenderTarget->FillGeometry(pg, g_pSolidOuterBrush); pg->Release();
            }
            if (glowR > 0.1f && gl.size() >= 2) {
                ID2D1PathGeometry* pg = nullptr; ID2D1GeometrySink* ps = nullptr;
                g_pD2DFactory->CreatePathGeometry(&pg); pg->Open(&ps);
                ps->SetFillMode(D2D1_FILL_MODE_WINDING); ps->BeginFigure(gl[0], D2D1_FIGURE_BEGIN_FILLED);
                for (size_t i = 1; i < gl.size(); ++i) ps->AddLine(gl[i]);
                for (int i = (int)gr.size()-1; i >= 0; --i) ps->AddLine(gr[i]);
                ps->EndFigure(D2D1_FIGURE_END_CLOSED); ps->Close(); ps->Release();
                g_pSolidOuterBrush->SetColor(cols.solidOuter);
                g_pSolidOuterBrush->SetOpacity(useEnhancedGlow ? glowO * 0.65f * fa : glowO * fa);
                g_pDCRenderTarget->FillGeometry(pg, g_pSolidOuterBrush); pg->Release();
            }
            ID2D1PathGeometry *pog = nullptr, *pcg = nullptr;
            ID2D1GeometrySink* ps = nullptr;
            g_pD2DFactory->CreatePathGeometry(&pog); pog->Open(&ps);
            ps->SetFillMode(D2D1_FILL_MODE_WINDING); ps->BeginFigure(lo[0], D2D1_FIGURE_BEGIN_FILLED);
            for (size_t i = 1; i < lo.size(); ++i) ps->AddLine(lo[i]);
            for (int i = (int)ro.size()-1; i >= 0; --i) ps->AddLine(ro[i]);
            ps->EndFigure(D2D1_FIGURE_END_CLOSED); ps->Close(); ps->Release();
            g_pD2DFactory->CreatePathGeometry(&pcg); pcg->Open(&ps);
            ps->SetFillMode(D2D1_FILL_MODE_WINDING); ps->BeginFigure(lc[0], D2D1_FIGURE_BEGIN_FILLED);
            for (size_t i = 1; i < lc.size(); ++i) ps->AddLine(lc[i]);
            for (int i = (int)rc.size()-1; i >= 0; --i) ps->AddLine(rc[i]);
            ps->EndFigure(D2D1_FIGURE_END_CLOSED); ps->Close(); ps->Release();
            float headR = 10.0f * widthMul, innerHeadR = 6.0f * widthMul;
            ID2D1EllipseGeometry *poe = nullptr, *pie = nullptr;
            g_pD2DFactory->CreateEllipseGeometry(D2D1::Ellipse(smoothed[0], headR, headR), &poe);
            g_pD2DFactory->CreateEllipseGeometry(D2D1::Ellipse(smoothed[0], innerHeadR, innerHeadR), &pie);
            ID2D1Brush *ob = g_pSolidOuterBrush, *ib = g_pSolidInnerBrush;
            if (g_enableSmoothGradient && g_pGradOuterBrush && g_pGradInnerBrush) { ob = g_pGradOuterBrush; ib = g_pGradInnerBrush; }
            else { g_pSolidOuterBrush->SetOpacity(.86f * fa); g_pSolidInnerBrush->SetOpacity(.86f * fa); }
            ID2D1Geometry* og[2] = { pog, poe }, *ig[2] = { pcg, pie };
            ID2D1GeometryGroup *pog_g = nullptr, *pig_g = nullptr;
            g_pD2DFactory->CreateGeometryGroup(D2D1_FILL_MODE_WINDING, og, 2, &pog_g);
            g_pD2DFactory->CreateGeometryGroup(D2D1_FILL_MODE_WINDING, ig, 2, &pig_g);
            g_pDCRenderTarget->FillGeometry(pog_g, ob);
            g_pDCRenderTarget->FillGeometry(pig_g, ib);
            g_pSolidOuterBrush->SetOpacity(1); g_pSolidInnerBrush->SetOpacity(1);
            if (g_enableHeadHighlight) {
                g_pSolidInnerBrush->SetColor(D2D1::ColorF(1, 1, 1, 1));
                g_pSolidInnerBrush->SetOpacity(0.85f * fa);
                g_pDCRenderTarget->FillEllipse(D2D1::Ellipse(smoothed[0], 2.8f, 2.8f), g_pSolidInnerBrush);
                g_pSolidInnerBrush->SetOpacity(1.0f);
            }
            pog_g->Release(); pig_g->Release(); poe->Release(); pie->Release(); pog->Release(); pcg->Release();
            needsClear = true;
        }
    }

    // ===== 点击波纹 =====
    if (g_enableClickEffect && !g_ripples.empty()) {
        for (auto& ripple : g_ripples) {
            float elapsed = (float)(dwTime - ripple.startTime), progress = elapsed / g_clickDuration;
            if (progress < 0 || progress >= 1) continue;
            float radius = progress * g_clickMaxRadius, alpha = (1 - progress) * .7f;
            D2D1_POINT_2F c = D2D1::Point2F((float)(ripple.pos.x - vX), (float)(ripple.pos.y - vY));
            g_pSolidOuterBrush->SetColor(cols.solidOuter);
            g_pSolidOuterBrush->SetOpacity(alpha * 0.12f);
            g_pDCRenderTarget->FillEllipse(D2D1::Ellipse(c, radius, radius), g_pSolidOuterBrush);
            g_pSolidOuterBrush->SetOpacity(alpha);
            g_pDCRenderTarget->DrawEllipse(D2D1::Ellipse(c, radius, radius), g_pSolidOuterBrush, 2.5f);
            if (radius > 4) {
                g_pSolidInnerBrush->SetColor(cols.solidInner); g_pSolidInnerBrush->SetOpacity(alpha * .8f);
                g_pDCRenderTarget->DrawEllipse(D2D1::Ellipse(c, radius * .7f, radius * .7f), g_pSolidInnerBrush, 1.5f);
            }
            needsClear = true;
        }
        g_pSolidOuterBrush->SetOpacity(1); g_pSolidInnerBrush->SetOpacity(1);
    }

    HRESULT hr = g_pDCRenderTarget->EndDraw();
    if (hr == D2DERR_RECREATE_TARGET) { g_pDCRenderTarget->Release(); g_pDCRenderTarget = nullptr; ReleaseGradientBrushes(); }

    BLENDFUNCTION blend = { 0 }; blend.BlendOp = AC_SRC_OVER; blend.SourceConstantAlpha = 255; blend.AlphaFormat = AC_SRC_ALPHA;
    POINT ptPos = { vX, vY }; SIZE sz = { vW, vH }; POINT ptSrc = { 0, 0 };
    UpdateLayeredWindow(hwnd, hdcScreen, &ptPos, &sz, g_hdcMem, &ptSrc, 0, &blend, ULW_ALPHA);
    ReleaseDC(NULL, hdcScreen);
    if (!isSmearing && g_history.empty() && g_ripples.empty() && g_particles.empty()) needsClear = false;
}

// ===================== 粒子形状几何 =====================
static void CreateStarGeometry(ID2D1Factory* factory, ID2D1PathGeometry** geom) {
    factory->CreatePathGeometry(geom);
    ID2D1GeometrySink* sink = nullptr;
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

static void CreateHexagramGeometry(ID2D1Factory* factory, ID2D1PathGeometry** geom) {
    factory->CreatePathGeometry(geom);
    ID2D1GeometrySink* sink = nullptr;
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

// ===================== 覆盖层线程 =====================
static UINT g_reloadMsg = 0;
static LRESULT CALLBACK OverlayWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (msg == g_reloadMsg) { LoadSettings(); return 0; }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}
DWORD WINAPI OverlayThreadProc(LPVOID) {
    CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
    SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    srand((unsigned)GetTickCount());
    D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &g_pD2DFactory);
    if (g_pD2DFactory) {
        CreateStarGeometry(g_pD2DFactory, &g_pStarGeom);
        CreateHexagramGeometry(g_pD2DFactory, &g_pHexagramGeom);
    }
    HINSTANCE hi = GetModuleHandle(NULL);
    const wchar_t CN[] = L"MouseTrailClass";
    WNDCLASS wc = { }; wc.lpfnWndProc = OverlayWndProc; wc.hInstance = hi; wc.lpszClassName = CN; RegisterClass(&wc);
    int sx = GetSystemMetrics(SM_XVIRTUALSCREEN), sy = GetSystemMetrics(SM_YVIRTUALSCREEN);
    int sw = GetSystemMetrics(SM_CXVIRTUALSCREEN), sh = GetSystemMetrics(SM_CYVIRTUALSCREEN) - 1;
    g_overlayHwnd = CreateWindowEx(
        WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE,
        CN, L"MouseTrailOverlay", WS_POPUP, sx, sy, sw, sh, NULL, NULL, hi, NULL);
    if (g_readyEvent) SetEvent(g_readyEvent);
    if (!g_overlayHwnd) return 0;
    ShowWindow(g_overlayHwnd, SW_SHOWNA);
    GetCursorPos(&g_lastPos);
    SetTimer(g_overlayHwnd, 1, USER_TIMER_MINIMUM, SmearTimerProc);
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) { TranslateMessage(&msg); DispatchMessage(&msg); }
    ReleaseGradientBrushes();
    if (g_pShadowBrush) { g_pShadowBrush->Release(); g_pShadowBrush = nullptr; }
    if (g_pSolidInnerBrush) { g_pSolidInnerBrush->Release(); g_pSolidInnerBrush = nullptr; }
    if (g_pSolidOuterBrush) { g_pSolidOuterBrush->Release(); g_pSolidOuterBrush = nullptr; }
    if (g_pDCRenderTarget) { g_pDCRenderTarget->Release(); g_pDCRenderTarget = nullptr; }
    if (g_pStarGeom) { g_pStarGeom->Release(); g_pStarGeom = nullptr; }
    if (g_pHexagramGeom) { g_pHexagramGeom->Release(); g_pHexagramGeom = nullptr; }
    if (g_pD2DFactory) { g_pD2DFactory->Release(); g_pD2DFactory = nullptr; }
    if (g_hBitmap) DeleteObject(g_hBitmap);
    if (g_hdcMem) DeleteDC(g_hdcMem);
    DestroyWindow(g_overlayHwnd); UnregisterClass(CN, hi); CoUninitialize();
    return 0;
}

BOOL WhTool_ModInit() {
    LoadSettings();
    g_readyEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    g_threadHandle = CreateThread(NULL, 0, OverlayThreadProc, NULL, 0, NULL);
    return TRUE;
}

void WhTool_ModUninit() {
    if (g_threadHandle) {
        if (g_readyEvent) WaitForSingleObject(g_readyEvent, INFINITE);
        if (g_overlayHwnd) PostMessage(g_overlayHwnd, WM_QUIT, 0, 0);
        WaitForSingleObject(g_threadHandle, INFINITE);
        CloseHandle(g_threadHandle);
        g_threadHandle = NULL;
    }
    if (g_readyEvent) {
        CloseHandle(g_readyEvent);
        g_readyEvent = NULL;
    }
}
void WhTool_ModSettingsChanged() { if (!g_reloadMsg) g_reloadMsg = RegisterWindowMessageW(L"MouseTrail_Reload"); if (g_overlayHwnd) PostMessage(g_overlayHwnd, g_reloadMsg, 0, 0); }

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
        if (wcscmp(argv[i], L"-service") == 0 ||
            wcscmp(argv[i], L"-service-start") == 0 ||
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
        DWORD entryPointRVA = ntHeaders->OptionalHeader.AddressOfEntryPoint;
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
void Wh_ModAfterInit() {
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
    WCHAR
    commandLine[MAX_PATH + 2 +
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
        DWORD dwCreationFlags, LPVOID lpEnvironment, LPCWSTR lpCurrentDirectory,
        LPSTARTUPINFOW lpStartupInfo,
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
