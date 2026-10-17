## 2.1.0 ([Oct 17, 2026](https://github.com/ramensoftware/windhawk-mods/blob/03313a1c1b7fd3526f5a6fb8f79f3a727d6ae74c/mods/tourne-table-desktop-audio-visualizer.wh.cpp))

* **Precision analysis engine:** every bar is its own band (real-input FFT, three resolution tiers; log, mel, bark, ERB, fractional-octave or note layouts; A/C/Z weighting; time-based ballistics). The 1.x analysis stays as Analysis Engine = Classic.
* **Direct3D 11 renderer:** instanced bars drawn from a buffer. It skips frames that didn't change. Direct2D stays as an option and as the fallback.
* **Idle:** one wake per frame while playing. In silence it wakes only on audio. Deep Idle then stops the loopback stream and reads the peak meter instead.
* **Right-click menu** for live quick settings, stored in mod values and laid over the settings page.
* **Any audio device** as the source, outputs or inputs.
* **New shapes and styles:** Terminal, Goniometer, LED Meter, Spectrogram, VU Needles and more. Reflection, Glow and Bloom, and bar modifiers.
* **Media Card** and **My Styles** with a Style Editor.
* **Placement:** Taskbar, Panel and Dock To App.
* **Setup sharing:** codes and `.ttstyle` files. A Community button reads a list of shared styles from GitHub, but only when it's pressed.
* **32-bit:** builds for 32-bit as well as 64-bit.
* **Readme:** the images now load from the Tourne-Style repo.

## 1.3.0 ([Sep 20, 2026](https://github.com/ramensoftware/windhawk-mods/blob/e4bfa7249dca8ae600280c33614715815211823b/mods/tourne-table-desktop-audio-visualizer.wh.cpp))

* **New: Input Gain (dB)**, a fixed level trim on the captured audio, range -24 to +24. Loopback capture sees each app's own volume slider but not the Windows master slider, so a music app held at a low volume reads as quiet no matter how loud the system is. This is the control for that, and it is also the only setting that scales the Oscilloscope waveform directly.

* **New: Auto Gain**, off by default, with **Auto Gain Max Boost (dB)**, range 0 to 24. Adapts the level so quiet sources still fill the bars without retuning Sensitivity per app or per track. Boost only, held steady through silence, and idle shutdown behaves exactly as it did before.

* **Fixed:** Pause When Covered never triggered for a visualizer positioned near a screen edge. Coverage was measured against the whole visualizer including any part hanging off-screen, which can never be covered by a window, so the reading could never reach the default 100% threshold.

* **Fixed:** the Now Playing Font check could warn about a font that is genuinely installed, after a cold boot. Mods start before Windows has finished registering fonts, so the check now retries for ten seconds before reporting anything.

* **Fixed:** the Oscilloscope trace could be drawn past the edge of its panel on a hot signal. It now clips flat at full scale.

* **Fixed:** clearing a saved position wrote to storage synchronously from inside a low-level input hook, where everything blocks all system input until it returns. It now uses the same deferred save the keyboard nudges already did.

* **Changed:** removed the one-shot import of the position file that pre-catalog builds wrote. The mod now performs no file I/O at all.

## 1.2.0 ([Sep 18, 2026](https://github.com/ramensoftware/windhawk-mods/blob/9485e30106c141ddc8472442a43c1c740d7ce4b3/mods/tourne-table-desktop-audio-visualizer.wh.cpp))

Initial release.
