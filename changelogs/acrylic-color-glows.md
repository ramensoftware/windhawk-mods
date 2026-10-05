## 0.5.0 ([Oct 5, 2026](https://github.com/ramensoftware/windhawk-mods/blob/318c99c6364375c88fbfd29739e0208da2bc2268/mods/acrylic-color-glows.wh.cpp))

## What's new

### Audio reactivity (optional, off by default)
Every effect can now speed up while sound is playing.

- **React to audio**: turns the feature on.
- **Cycle duration at full volume**: the cycle duration at the loudest level (default 3 s). The regular "Cycle duration" is used in silence, with smooth transitions in between.
- **Sensitivity**: a percentage from 25 to 400 (100 = normal), for listening at low volume.

The effect speeds up quickly and calms down gently when the sound stops. The level follows loudness as heard (square root of the peak), so it reacts at normal and low volume too.

**Privacy:** only the peak level of the default output device is read through `IAudioMeterInformation`. It's the same value that drives the bar in the Windows volume mixer. No audio is recorded, analyzed or sent anywhere, and the microphone is never used.

**Resources:**
- The level is read only while an effect is moving. Frozen effects, hidden windows and the automatic power-saving pauses all stop it.
- The mod follows changes of the default output device, for example when headphones are plugged in.
- Without an output device it simply stays at normal speed, without logging errors.

The new settings sit right below "Cycle duration", since they work together.

## Fixes
- **Reduced smoothness rate:** it ran at about 21 updates per second instead of the intended 30. USER timers fire on the system tick (15.6 ms), so a 33 ms interval rounded up to about 47 ms. The interval is now 30 ms (about 32 updates per second). The audio mode uses 15 ms (about 64 updates per second).

## Technical notes
- **How speed changes:** with audio reactivity on, the shared clock is advanced by the worker. It adds the elapsed time multiplied by the current speed, because the compositor can't change the speed of a running animation. Speed changes therefore never cause a jump.
- **Reentrancy:** audio COM calls are covered by the same reentrancy guard as the refresh.
- **Cleanup:** the audio objects are released before the COM apartment is torn down.
- **Headers:** `IAudioMeterInformation` is declared locally, because the compiler's `endpointvolume.h` only forward-declares it.
- **Metadata:** added the `@github` field.

## 0.4.2 ([Oct 3, 2026](https://github.com/ramensoftware/windhawk-mods/blob/72b4d1d86c1ec6e0fefba8f2b54b33dec2021738/mods/acrylic-color-glows.wh.cpp))

Initial release.
