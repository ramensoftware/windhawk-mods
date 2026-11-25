## 1.2.1 ([Nov 25, 2026](https://github.com/ramensoftware/windhawk-mods/blob/579149e78863cacf0bd0d4922d9beac306d8507c/mods/touchpad-relaxed-hand-tracking.wh.cpp))

* **Steadier speed multiplier (improved "flicks"):** the higher multiplier no longer drops at the start and end of each movement, when fingers don't move in unison (most noticeable with five fingers down). The lower multiplier now only takes over after a confirmed movement with fewer fingers.    
* **No more accidental clicks with five fingers resting:** touchpads usually report up to five contacts, and a thumb split in two at the pad's edge made the pad take turns among them; those swaps no longer click.   
* 5-3 fingers moving default speed Increased to 2.0 after this update's precision gain.   
*  Readme and settings review.

## 1.2.0 ([Nov 21, 2026](https://github.com/ramensoftware/windhawk-mods/blob/bccd6babe55b52d78e9caecec8b027cf2ad0b31f/mods/touchpad-relaxed-hand-tracking.wh.cpp))

* **Improved click stability:** more assertive tap-hold drag action and pointer position while clicking.
* **Scroll output live switch hotkey and memory (optional):** press Win, or Win + modifiers, during a scroll to use the other output method (wheel or touch) for that action. The mod will remember the last choice **for each program**. 
* **Scroll inertia stops on any key press**, as Windows' own does (Ctrl no longer zooms a page that is still scrolling).
* **Thumb recognition with fewer fingers:** in any tracking mode, a thumb resting well under the other fingers is recognized and left out, so it no longer moves the pointer with them.
* **Speed multiplier moving fingers correction:** a resting thumb was counting as a moving finger. Not anymore, only when it also moves (but still do not steer the pointer). 
* **Resizable diagnostics overlay**: its window dragging and resizing respond only to native taps to avoid lag.

## 1.1.0 ([Nov 16, 2026](https://github.com/ramensoftware/windhawk-mods/blob/ce1a605b853f4a015dffaef8ea6c6447bf6fcbfc/mods/touchpad-relaxed-hand-tracking.wh.cpp))

* **Improved movement precision:** slow movements no longer lose distance, resting fingers drop out of the movement, and a single finger moving on its own leads, for precise positioning.
* **Speed reset:** the multipliers are back to 1.5 for 5-3 fingers and 1.0 for 2-1. If you changed them, check your values.
* **Fewer accidental clicks:** touches at the pad's edge, a finger flickering back where it just left, and the thumb no longer click.
* **Steadier near the pad's edge:** a finger at the edge, which the touchpad reads poorly, rests while fingers further in steer.
* **Thumb recognition:** with the new Hand setting (right or left), the thumb is the touch farthest inside the hand on the lower part of the pad, or, with the whole hand down, anywhere. The pinky is never taken for it, and recognition holds when the index crosses over the thumb, a finger slips off the pad and comes back, or the pad reports a flat thumb as two touches.
* **The thumb is left out of the more-fingers glide for better movement precision:** it only rests, helps start the glide and counts as a finger for the speed multiplier. It never moves the pointer or clicks while other fingers touch the pad, even when the pad reports it as two touches.
* **Diagnostics overlay:** a configurable hotkey shows how the mod reads each finger: which ones steer, rest, may tap, scroll or hold, and which one is the thumb. It can be dragged and closed, and records nothing while hidden.
* **Mouse wheel is now the default scroll output.** More precise vertical and horizontal scrolling. Touch scrolling is still available in the settings.
* **Tap and hold feedback (optional):** a small dot shows beside the pointer on each tap, and stays while a hold drags or a scroll runs, Windows' own taps and scroll included. The pointer itself doesn't change.

## 1.0.0 ([Nov 7, 2026](https://github.com/ramensoftware/windhawk-mods/blob/e1b866d715515ef71cec20eb25ef4eb437c26cfe/mods/touchpad-relaxed-hand-tracking.wh.cpp))

Initial release.
