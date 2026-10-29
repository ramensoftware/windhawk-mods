## 1.0.4 ([Oct 29, 2026](https://github.com/ramensoftware/windhawk-mods/blob/03aa5d1759a850ce6b8df6f9a68dc63b99c12cf9/mods/magic-pointer.wh.cpp))

* Added ChatGPT and Claude next to Gemini. The one API key setting accepts a key from any of the three and the mod recognizes which service it is for. That service then answers questions and follow-ups and powers Smart suggestions and Translate. Without a key, Gemini in the browser works as before.
* The Model setting is now a level (Fast, Balanced, Smartest) that maps to each service's own models, and the separate suggestions model setting is gone. Gemini model names saved by 1.0.3 are still honored.
* When the chosen model is busy, slow to start or not available on the key's plan, a faster model answers instead and the card says so.
* New Start typing to ask setting (on by default): as soon as the pointer appears, start typing and the card opens at the pointer with your text and no image, and Enter asks. Turn it off to keep typing in the app you were using.
* New Custom pointer file setting to use your own .cur or .ani file (animated cursors included) instead of the pointer arrow. It only replaces the pointer while the Magic Pointer is on screen, scales with Pointer size and display scaling, and gets the same glow. The hint moves aside for large cursors.
* More reliable hand-off to Gemini in the browser: the image and question are retried when Gemini redraws its message box after loading, so an attachment is no longer sometimes left out.
* Translated the new texts and settings and updated the documentation. The new translations were written with AI assistance, so corrections are welcome on GitHub.

## 1.0.3 ([Oct 18, 2026](https://github.com/ramensoftware/windhawk-mods/blob/2cb665613f187b4f2160998718b76a5600c22011/mods/magic-pointer.wh.cpp))

* Added translations. The hint, the card with its buttons and messages, and the Google Lens helper page now follow your Windows display language. Available in Arabic, Bengali, Chinese (Simplified and Traditional), Croatian, Czech, Danish, Dutch, Finnish, French, German, Greek, Hebrew, Hindi, Hungarian, Indonesian, Italian, Japanese, Korean, Norwegian, Persian, Polish, Portuguese (Brazil), Romanian, Russian, Spanish, Swedish, Tamil, Thai, Turkish, Ukrainian and Vietnamese, including right-to-left layouts. To make things easier the translations were written with AI assistance, so corrections are welcome on GitHub.
* New **Language** setting to use a different language than Windows. The settings page is translated too, in the languages Windhawk offers, and follows Windhawk's own language.
* The Translate button now uses your chosen language when **Translate to** is set to Auto. The language list shows native names and gained Croatian, Persian and Tamil, and Smart suggestions are written in your language.
* The ripple animation is much smoother on slower PCs and high-resolution screens. It lowers its detail when a frame takes too long, and it now follows your monitor's refresh rate, up to 144 fps.
* Fixed a hitch when summoning the pointer on large or high-resolution screens.

## 1.0.2 ([Oct 14, 2026](https://github.com/ramensoftware/windhawk-mods/blob/9ef5c6d286cc4961cf85aa8d1b691862c900e82f/mods/magic-pointer.wh.cpp))

* Add a cursor-sampling fallback for shake activation while the Windhawk UI is focused.
* Use standard foreground activation for Gemini browser handoff without attaching input queues.
* Add an animated preview, Discord contact, and problem-reporting instructions.
* Clarify Google AI Studio API key setup, follow-up questions, and optional Smart suggestions.

## 1.0.1 ([Oct 12, 2026](https://github.com/ramensoftware/windhawk-mods/blob/bc7a9d84ea4986925be09f770a2cee39ebbe2eb8/mods/magic-pointer.wh.cpp))

Initial release.
