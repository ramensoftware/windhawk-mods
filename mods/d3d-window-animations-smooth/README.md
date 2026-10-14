# D3D Window Animations (Smooth & Hz-Aware)

A high-performance, GPU-composited window animation mod for Windows, powered by **Direct3D 11** and **DirectComposition**. It brings smooth, fluid, and monitor-Hz-aware animations to window minimizing, restoring, opening, and closing.

---

## ⚠️ Beta Version Notice & First Launch Note

> **Important Note on First Launch / Update:**
> When you install or update the mod for the first time, **Windows Explorer / Desktop will briefly restart** to inject the hook into system processes. Additionally, running applications that are animated for the very first time may close or restart once to apply the new D3D11 rendering hook. **This is completely normal system behavior** during initial hook injection — simply reopen your apps, and all subsequent animations will work smoothly without any restarts!

> **Beta Status:** This mod is currently in **Beta**. While it is designed with stability in mind and tested for smooth everyday performance, unexpected bugs or edge-case behavior with specific applications may still occur.

If you encounter any issues, glitches, or crashes, please report them! Your feedback helps make this mod more stable and compatible for everyone:
- **Bug Reports & Issues:** Submit an issue directly on [GitHub Issues](https://github.com/Booge2) (or via the Windhawk mod discussion panel if available).
- **Details to Include:** Please share your Windows version, display setup (multi-monitor/Hz), and the name of the app (`.exe`) where the issue occurred.

---

## 🚀 Why D3D11 GPU Acceleration vs. CPU-Based Animations?

Most traditional window animation mods perform frame calculation and rendering directly on the **CPU (GDI/Software rendering)**. This mod takes a completely different approach by leveraging **Direct3D 11 + DirectComposition** for full hardware acceleration.

### Key Advantages:

1. **Zero CPU Bottlenecks & No Lag:**
   - **CPU-based:** Drawing complex transitions frame-by-frame on the CPU causes micro-stutters, high CPU usage spikes, and frame drops when the system is under load (e.g., during gaming or heavy multitasking).
   - **D3D11 GPU-composited:** The CPU only manages window states, while 100% of the pixel transformations, scaling, and easing are offloaded to your Graphics Card. This ensures stable 60/120/144/240+ FPS without lagging your system.

2. **Visual Differences & Shader Capabilities:**
   - **CPU-based:** Limited to simple geometry moves, crude opacity changes, or slow pixel math that looks jittery on modern high-refresh-rate displays.
   - **D3D11 Pixel Shaders:** Uses dedicated HLSL pixel shaders. This enables complex visual effects like organic cubic easing curve acceleration, smooth scale/overshoot bounds, sub-pixel blending, and subtle start-up lighting pulses that are impossible to render on the CPU efficiently.

3. **High Refresh Rate (Hz-Aware) Synchronization:**
   - **CPU-based:** Tied to fixed timer intervals (e.g., 60Hz default), resulting in uneven frame pacing or screen tearing on 120Hz/144Hz/240Hz monitors.
   - **D3D11 + DComp:** Dynamically queries your monitor’s refresh rate and uses high-resolution waitable timers and swapchain frame latency management for pixel-perfect synchronization with your monitor's exact refresh rate.

4. **Seamless Isolation (Ghost Canvas Technique):**
   - The mod renders animation frames onto a lightweight, top-most D3D11 "ghost" overlay surface while hardware-cloaking the real window via DWM. This prevents window flickering, artifacting, or race conditions with app surfaces (like Chromium or Electron renderers).

---

## ✨ Features

- **GPU Acceleration (D3D11):** Renders animations smoothly on the graphics card, completely avoiding heavy CPU overhead.
- **Monitor-Hz Aware Pacing:** Automatically adapts to your display’s refresh rate (supports high-Hz monitors like 120Hz, 144Hz, 240Hz+).
- **Full Animation Suite:** Custom animations for:
  - **Minimize** (smooth scaling & fade-out)
  - **Restore** (fluid expansion back to place)
  - **Open / Launch** (expressive starting scale & subtle lighting pulse)
  - **Close** (snappy closing transition)
- **Multi-Monitor Support:** Correctly calculates window targets and positioning across multiple displays.
- **Robust Safety & Stability:** Includes built-in smart filters, multi-process warm-up handling, and crash protection for web engines (Chrome, Edge, Electron apps) and system components.

---

## ⚙️ Settings / Configuration

You can customize the mod behavior directly via the Windhawk settings panel:

| Setting Name | Default | Description |
| :--- | :---: | :---|
| **Minimize Duration (ms)** | `500` | Duration of the window minimize animation. |
| **Restore Duration (ms)** | `500` | Duration of the window restore animation. |
| **Open Duration (ms)** | `500` | Duration of the window open/launch animation. |
| **Close Duration (ms)** | `500` | Duration of the window close animation. |
| **Frame Rate** | `0` | Target FPS (`0` = auto-sync with Monitor Hz). |
| **Enable Minimize Animation** | `true` | Toggle minimize effect. |
| **Enable Restore Animation** | `true` | Toggle restore effect. |
| **Enable Open / Launch Animation** | `true` | Toggle application launch effect. |
| **Enable Close Animation** | `true` | Toggle close effect. |
| **Multi-Monitor Support** | `true` | Enable multi-display coordinate mapping. |
| **Excluded Programs** | *(None)* | List of executables where animations should be disabled. |

---

## 🛡️ Compatibility, Exclusions & Special Apps

- **Web Browsers & Electron Apps:** Fully compatible with main browser windows (after smart warm-up delays), while safely skipping internal helper/renderer processes.
- **Special Software (Steam, Wallpaper Engine, etc.):** Heavy desktop utilities, launchers, and wallpaper engines (such as **Steam**, **Wallpaper Engine**, etc.) are automatically detected and **excluded from animations** to prevent rendering conflicts, overlay glitches, or performance drops.
- **Custom Exclusions:** If any other application behaves unexpectedly, you can easily add its `.exe` name to the `Excluded Programs` list in the mod settings.

---

## 📝 License

Distributed under the **MIT License**. See `LICENSE` for more information.
