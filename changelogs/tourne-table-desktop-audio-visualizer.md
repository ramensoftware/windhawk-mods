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
