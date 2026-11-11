// ==WindhawkMod==
// @id                lock-screen-live-wallpaper
// @name              Lock Screen Live Wallpaper
// @description       Play videos and animated GIFs on the lock screen and sign-in screen
// @description:id-ID Putar video dan GIF animasi di lock screen dan layar sign-in
// @version           1.1.0
// @author            grn15RenS
// @github            https://github.com/grn15RenS
// @donateUrl         https://ko-fi.com/grn15rens
// @license           GPL-3.0
// @include           LogonUI.exe
// @include           LockApp.exe
// @include           explorer.exe
// @architecture      x86-64
// @compilerOptions   -lole32 -loleaut32 -lruntimeobject -lmfplat -lmfreadwrite -lmfuuid -lwinmm -ladvapi32 -luuid -ld3d11 -lgdi32 -luser32 -lshell32 -luxtheme
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
![preview](https://i.imgur.com/QFNE1hY.gif)

# Lock Screen Live Wallpaper

**Windows 11 finally has a live wallpaper on the lock screen.** For years people have wanted a video playing behind the lock screen clock, and the popular wallpaper apps never got past the desktop. This mod brings it to the lock screen and the sign-in screen: it runs inside `LogonUI.exe`, decodes your video or animated GIF, and feeds the frames into the XAML image elements that Windows normally uses for the lock screen wallpaper. If something looks wrong, a built-in fail-safe restores the original wallpaper.

## ⚠️ Before you start ⚠️

`LogonUI.exe` is a protected system process, so Windhawk does not inject into it by default. For this mod to work you must add it to Windhawk's process inclusion list:

1. Open Windhawk and go to **Settings** → **Advanced settings** → **More advanced settings**.
2. Add `LogonUI.exe` to the **Process inclusion list** and save.

![Windhawk advanced settings: process inclusion list](https://i.imgur.com/9Biip4G.png)

## 📋 Requirements

- Windows 11
- `LogonUI.exe` added to Windhawk's process inclusion list (see above)
- A video file that the **SYSTEM** account can read. `LogonUI.exe` runs as SYSTEM, so network locations, encrypted folders, and cloud-only (not downloaded) files may not work. A normal local folder is the safest choice.

## 📝 Changelog

**1.1.0**

- **Preview window**: check fit, colors and speed without locking the PC. Open it from the settings, a hotkey (Ctrl+Alt+P by default, and you can type your own) or a tray icon. It can also draw the real clock, date and status icons over the video.
- **Animated GIF support**: `gif` files work as a source, alone or inside a folder. GIFs with more than 3840x2160 pixels in total are rejected, because they could use hundreds of MB of memory inside `LogonUI.exe`.
- **Low battery saver**: lower the frame rate or the render width, or freeze on a still frame, when the battery is low.
- **Dim**: darken the video so the clock is easier to read.
- **Folder order**: pick a random, sequential or fixed video from a folder.
- **Audio fade**: the audio fades in and out instead of starting and stopping abruptly.
- **Still frame duration after boot**: the time right after boot that shows the last saved frame is now a setting.
- **Cache housekeeping**: cached first frames older than 30 days are deleted, and only the 5 newest others are kept.
- **Faster color processing**: when only brightness, contrast, gamma or dim are changed, the hue and saturation step is skipped.
- **Fixes**: videos whose decoder reports a padded frame height (for example 1088 for 1080) are cropped to the real picture height, and the audio restarts together with the video on every loop.
- **Crash guard**: a session in which the password screen stayed open for at least 1 second no longer counts as a failed start, so several quick unlocks in a row no longer switch the video off.

**1.0.0**

- First release.

## ⚙️ Features

| Feature | Details |
| --- | --- |
| Video source | One video file, one animated GIF, or a folder (random, sequential or fixed order) |
| Formats | `mp4`, `m4v`, `mov`, `wmv`, `avi`, `mkv` (anything Media Foundation can decode on your system; H.264 MP4 is recommended), and animated `gif` (no audio, at most 3840x2160 pixels in total) |
| Fit modes | Cover, fill, fit (letterbox), center |
| Audio | Plays the video's audio track when the volume is above 0, with a short fade in and fade out |
| Playback speed | 25% to 400% |
| Hardware decoding | Decodes on the GPU (D3D11/DXVA) when possible, with automatic fallback to software decoding |
| Color adjustments | Hue, saturation, brightness, contrast, gamma, dim |
| Power saving | Still-frame mode, pause when idle, optional pause on the password screen, and a low battery saver (light, balanced, maximum) |
| Boot behavior | Optionally show the last frame for a chosen time right after boot instead of decoding |
| Shutdown screen | Optionally show the video on the "Shutting down" / "Restarting" screen (off by default, so Windows' own black screen is shown) |
| Preview window | Optional window that plays your video with the current fit, color and speed settings, opened from the settings, a hotkey or a tray icon. It can draw a copy of the real clock, date and status icons over the video. It runs inside `explorer.exe`, so the mod is also loaded there. It is an approximation of the wallpaper layer, not the Windows lock screen UI itself |
| Fail-safe | Restores the original wallpaper if the lock screen UI changes or the mod misbehaves |
| Debug log | Optional CPU and decode statistics and a UI tree dump |

| ![preview window](https://i.imgur.com/KFW2K91.gif) |
| :---: |
| *The preview window playing the video, with the clock, date and status icons drawn over it.* |

## 📝 Notes on some settings

| Setting or topic | Details |
| --- | --- |
| **Folder order** | Used when the path is a folder. *Random* picks a video on every lock, *Sequential* plays the next one (alphabetical) on every lock and starts over after the last one, *Fixed* always uses the first one (alphabetical). GIF files in the folder count as videos. |
| **Audio volume** | 0 means muted. Above 0, audio only plays at 100% speed while the video is playing (not paused, idle-paused, or showing the boot still frame). The audio fades in when playback starts or resumes and fades out when it is held, in about 0.6 seconds. |
| **Dim** | 0 to 90. Darkens the video so the clock and the sign-in screen are easier to read. 0 is off. |
| **Render width** | Internal canvas width (320 to 1920). The height follows your screen's aspect ratio. This is the biggest CPU lever. |
| **Hardware video decoding** | On (default) decodes with the GPU when possible, which greatly lowers CPU use for 4K or 60 fps videos. Turn it off if playback glitches or the sign-in screen has problems. If the GPU path cannot produce a frame, the mod falls back to software decoding by itself. Changing this setting also re-enables playback if the crash guard turned it off. |
| **Pause when idle** | Freezes the video and mutes the audio after the chosen time without input, and resumes on the next input. |
| **Battery saver starts at** and **Battery saver mode** | Set the threshold above 0 to turn the battery saver on. When the battery is at or below it (and the PC is unplugged, unless **Also apply while charging** is on), the chosen mode is applied. *Light* caps playback at about 30 fps (only matters for 60 fps videos), *Balanced* renders 960 px wide and caps at about 15 fps, *Maximum* freezes on a still frame. The battery is checked every 5 seconds. The render width is chosen when the lock screen starts, so it applies from the next lock; the frame rate cap and the freeze apply live. |
| **Play video on the password screen** | On (default) keeps the video playing while you type your password. Off freezes it on the password screen, which can help if playback stutters there, and it plays again on the lock screen. |
| **Behavior right after boot** and **Still frame duration after boot** | With **Show last frame**, the first minutes after boot show the last saved frame without decoding. How long is set by **Still frame duration after boot** (10 to 1800 seconds, 180 by default). Needs at least one earlier session where the video was playing. |
| **Show preview window** | Opens the preview window and keeps it open. Saving the settings refreshes it without closing it, unless a preview option itself changed. You can resize the window (the aspect ratio stays) and it remembers its size and position. `Esc` or the close button hides it, and a hidden or minimized preview does not decode anything. |
| **Preview hotkey** | A global shortcut that shows or hides the preview window. It is a text field with `Ctrl+Alt+P` as the default: delete the text to turn the hotkey off, or type your own combination, for example `Ctrl+Alt+Shift+P` or `Ctrl+Shift+F12`. Write the modifiers (`Ctrl`, `Alt`, `Shift`, `Win`) and one key joined with `+`; at least one modifier is required. The key can be a letter, a digit, `F1` to `F24`, `Space`, `Tab`, `Enter`, `Esc`, `Insert`, `Delete`, `Home`, `End`, `PgUp`, `PgDn` or an arrow key (`Up`, `Down`, `Left`, `Right`). Many combinations with `Win` are already used by Windows itself. Text that cannot be read is ignored and the log shows `is not valid`. If another program already uses the combination, Windows refuses it and the log shows `RegisterHotKey failed`. While the hotkey is on, the mod keeps a small hidden window inside `explorer.exe` to listen for it. |
| **Tray icon** | Left click shows or hides the preview window, right click opens a small menu. |
| **Preview stays on top** | Keeps the preview window above other windows. When it is off, other windows can cover the preview. |
| **Clock, date and status icons in the preview** | The clock, date and status icons are drawn by `LockApp.exe`, not by `LogonUI.exe`. The mod captures them from the real lock screen every time you lock the PC (wait about 6 seconds without touching anything), and the preview uses the newest copy, so the clock in the preview shows the time of the last lock. While it renders them, the real clock and date move down for a split second; turn this option off if that bothers you, and the mod stops capturing. Until a copy exists, the preview says so at the bottom. |
| **Display scaling and layout** | The clock, date and icons in the preview were only tested at 100%, 125% and 150% display scaling and with the standard Windows 11 lock screen layout; with other layouts they may be placed approximately. On the PC this mod was developed on, Windows itself did not show the battery and network icons on the lock screen above 100% scaling, and the preview does the same. |
| **Debug log** | Also writes `debug.log` in the mod's storage folder, with CPU and decode statistics every 5 seconds (video only, GIFs show 0 ms) and UI tree dumps of the lock screen windows (`LogonUI.exe` and `LockApp.exe`). The dumps can make the video stutter for a moment at the start of a lock, so keep it off unless you are diagnosing a problem. Basic events always go to Windhawk's log (enable logging for the mod in Windhawk to see them). |
| **Long playback** | The video loops without any time limit. By default it freezes after 5 minutes without input (see **Pause when idle**); set that to 0 if you want it to keep playing while nobody is at the PC. |
| **Startup delay** | LogonUI starts fresh on every lock, so a video needs about 2 to 4 seconds before it starts moving; the cached first frame is shown until then (GIFs start almost immediately). The first lock after changing the video, **Render width** or **Fit mode** has no cached frame yet and can briefly show the original wallpaper. In folder mode each video gets its own cached frame the first time it is picked. Changing only the color settings does not need a new cached frame. |

## 📈 Performance tips

- Lower **Render width** (the default is 1920; for example try 1280 or 960).
- Videos at 4K or 60 fps are decoded on the GPU when **Hardware video decoding** is on (the default). With it off they are decoded on the CPU and can use several cores; prefer 1080p at 30 fps then.
- Use **Pause** if you only want a still image.
- Keep **Pause when idle** enabled so nothing is decoded while nobody is at the PC.
- Use the low battery saver on laptops.
- If the video stutters while you type your password, turn off **Play video on the password screen**.
- The password screen blurs the wallpaper behind it. A constantly changing background makes Windows redo that blur every frame, which costs some GPU time.

## 🔧 Troubleshooting

- **Normal wallpaper is shown**: make sure `LogonUI.exe` is in the process inclusion list and the video path is correct, then turn on **Debug log** and read `debug.log` in the mod's storage folder (see "Files written by the mod"). Look for `Failed to open video`, `Unsupported video format`, or `FAIL-SAFE TRIPPED`.
- **Video will not open**: try an H.264 MP4 stored somewhere the SYSTEM account can read.
- **No sound**: set **Audio volume** above 0 and see the notes above.
- **Stuttering**: lower the render width and enable **Debug log**, then check the `CPU` and `UI stalls` lines in the log.
- **The preview hotkey does nothing**: make sure the **Preview hotkey** field is not empty and is written like `Ctrl+Alt+P` (look for `is not valid` in Windhawk's log), then look for `RegisterHotKey failed`. Another program may already use that combination; type a different one.
- **The preview says there is no overlay yet**: lock the PC once (Win+L) and wait about 6 seconds without touching anything.
- **The clock, date or icons in the preview look misplaced**: turn on **Debug log**, lock the PC, wait about 6 seconds, and look at the lines starting with `Overlay:` in the log.
- **Still stuck?**: see **Report a bug** below.

## 🐞 Report a bug

If something does not work or you have a feature suggestion, please open an issue on [the mod's repository](https://github.com/grn15RenS/lock-screen-live-wallpaper/issues/) or on [the Windhawk mods repository](https://github.com/ramensoftware/windhawk-mods/issues). To help find the cause faster, please include:

- your Windows 11 version (for example 25H2)
- `debug.log` (turn on **Debug log** first, see "Files written by the mod" below)
- Windhawk's log (enable logging for the mod in Windhawk)

The logs contain the video path, so remove anything personal first if you prefer.

## 🤔 How it works

1. A worker thread finds the `LogonUI Logon Window` and installs a `WH_GETMESSAGE` hook on its UI thread.
2. A decoder thread reads the video as RGB32 through Media Foundation (or the GIF through WIC), scales each frame according to the fit mode, applies the color settings, and publishes it to a shared buffer.
3. On the UI thread, the newest frame is copied into a `WriteableBitmap`, which is set as the source of the `LockLogonImage` and `SwapLockLogonImage` elements.
4. When the mod is unloaded, the original image sources are restored.
5. The preview window lives in `explorer.exe` and runs its own decoder with the same settings.
6. In `LockApp.exe` the mod reads the lock screen UI. A few seconds after the lock screen appears, it renders the clock, date and status icons to transparent bitmaps at the scale XAML uses (adding temporary padding around the clock and date so large glyphs are not clipped, which makes them move for a split second), places them on a transparent canvas the size of the screen, and saves it as `overlay.bgra` for the preview to draw over the video.

## 🛡️ Fail-safe

The mod stops itself and restores the original wallpaper for the rest of that `LogonUI.exe` process when:

- the video decoder fails before producing any frame,
- no video frame is produced within 30 seconds,
- the lock screen elements (`LockLogonImage` / `SwapLockLogonImage`) are never found, which usually means a Windows update changed the UI, or
- the UI throws repeated errors (3 within 30 seconds).

Separately, a crash guard protects the sign-in screen. If two `LogonUI.exe` sessions in a row end before the decoder shows its first frame and before the password screen has been open for at least 1 second (for example because a codec crashes on your video), the mod turns playback off and the normal wallpaper is shown. To turn it back on, change **Video file or folder**, **Render width**, **Fit mode** or **Hardware video decoding** to a different value and keep it. Very short sessions (for example two Ctrl+Alt+Del screens in a row) can still trip it, and so can very quick unlocks with Windows Hello or a fingerprint reader (not tested); the same fix applies.

## 📁 Files written by the mod

Everything is stored in the mod's own storage folder inside Windhawk's data folder (the path below is for a regular install; a portable Windhawk keeps it elsewhere):

`C:\ProgramData\Windhawk\Engine\ModsWritable\mod-storage\lock-screen-live-wallpaper`

`ProgramData` is a hidden folder, so paste the path above into the File Explorer address bar to open it. The files are:

- `debug.log`: log file, trimmed automatically after 2 MB
- `frame_*.bgra`: cached first frame, so the lock screen can show something immediately. Files older than 30 days are deleted, and only the 5 newest others are kept
- `last.bgra`: last shown frame, used by the **Show last frame** boot mode
- `overlay.bgra`: the captured clock, date and status icons, used by the preview window

You can delete these files at any time; the mod recreates what it needs. The folder is removed automatically when the mod is uninstalled.

## ☕ Support

If you enjoy this mod, you can support its development:

- [Ko-fi](https://ko-fi.com/grn15rens)
- [Saweria](https://saweria.co/grn15RenS) (for Indonesia)

## ℹ️ Disclaimer

Only tested on Windows 11 23H2 and 25H2. This mod runs inside a system UI process (`LogonUI.exe`), so it includes a fail-safe that restores the original wallpaper if anything goes wrong. As a precaution, keep another way to sign in available while trying it out.

---

# Bahasa Indonesia

![preview](https://i.imgur.com/QFNE1hY.gif)

# Lock Screen Live Wallpaper

**Akhirnya, Windows 11 punya live wallpaper di lock screen.** Sudah bertahun-tahun banyak orang ingin ada video yang bergerak di belakang jam lock screen, tapi aplikasi wallpaper populer tidak pernah sampai ke sana. Mod ini membawanya ke lock screen dan layar sign-in: berjalan di dalam `LogonUI.exe`, men-decode video atau GIF animasimu, lalu memasukkan frame-nya ke elemen gambar XAML yang biasanya dipakai Windows untuk wallpaper lock screen. Jika ada yang tidak beres, fail-safe bawaan akan mengembalikan wallpaper asli.

## ⚠️ Sebelum mulai ⚠️

`LogonUI.exe` adalah proses sistem yang dilindungi, jadi Windhawk tidak meng-inject ke dalamnya secara default. Agar mod ini berfungsi, kamu harus menambahkannya ke daftar inklusi proses Windhawk:

1. Buka Windhawk lalu pergi ke **Settings** → **Advanced settings** → **More advanced settings**.
2. Tambahkan `LogonUI.exe` ke **Process inclusion list** lalu simpan.

![pengaturan lanjutan](https://i.imgur.com/9Biip4G.png)

## 📋 Persyaratan

- Windows 11
- `LogonUI.exe` sudah ditambahkan ke daftar inklusi proses Windhawk (lihat di atas)
- File video yang bisa dibaca akun **SYSTEM**. `LogonUI.exe` berjalan sebagai SYSTEM, sehingga lokasi jaringan, folder terenkripsi, dan file cloud-only (belum diunduh) mungkin tidak berfungsi. Folder lokal biasa paling aman.

## 📝 Catatan perubahan

**1.1.0**

- **Jendela preview**: cek fit, warna, dan kecepatan tanpa harus mengunci PC. Bisa dibuka dari pengaturan, hotkey (default Ctrl+Alt+P, bisa kamu ganti sendiri), atau tray icon. Jendela ini juga bisa menggambar jam, tanggal, dan ikon status asli di atas videonya.
- **Dukungan GIF animasi**: file `gif` bisa dipakai sebagai sumber, sendirian atau di dalam folder. GIF dengan lebih dari 3840x2160 piksel secara total ditolak, karena bisa memakai ratusan MB memori di dalam `LogonUI.exe`.
- **Penghemat baterai**: turunkan frame rate atau lebar render, atau bekukan di satu frame, saat baterai rendah.
- **Dim**: gelapkan video supaya jam lebih mudah dibaca.
- **Urutan folder**: pilih video acak, berurutan, atau tetap dari sebuah folder.
- **Fade audio**: audio masuk dan keluar dengan halus, tidak langsung mulai atau berhenti.
- **Durasi frame diam setelah boot**: lama waktu setelah boot yang menampilkan frame terakhir yang tersimpan sekarang bisa diatur.
- **Perawatan cache**: frame pertama di cache yang lebih tua dari 30 hari dihapus, dan hanya 5 yang terbaru yang disimpan.
- **Pemrosesan warna lebih cepat**: kalau hanya kecerahan, kontras, gamma, atau dim yang diubah, langkah hue dan saturasi dilewati.
- **Perbaikan**: video yang decoder-nya melaporkan tinggi frame berpadding (misalnya 1088 untuk 1080) dipotong sesuai tinggi gambar sebenarnya, dan audio dimulai ulang bersama video di setiap putaran.
- **Crash guard**: sesi dengan layar password tampil minimal 1 detik tidak dihitung sebagai start gagal, jadi unlock cepat berturut-turut tidak lagi mematikan video.

**1.0.0**

- Rilis pertama.

## ⚙️ Fitur

| Fitur | Keterangan |
| --- | --- |
| Sumber video | Satu file video, satu GIF animasi, atau satu folder (urutan acak, berurutan, atau tetap) |
| Format | `mp4`, `m4v`, `mov`, `wmv`, `avi`, `mkv` (apa pun yang bisa di-decode Media Foundation di sistemmu; MP4 H.264 disarankan), dan `gif` animasi (tanpa audio, maksimal 3840x2160 piksel secara total) |
| Mode fit | Cover, fill, fit (letterbox), center |
| Audio | Memutar audio video jika volume di atas 0, dengan fade masuk dan fade keluar singkat |
| Kecepatan putar | 25% sampai 400% |
| Decoding hardware | Men-decode di GPU (D3D11/DXVA) jika memungkinkan, dengan fallback otomatis ke decoding software |
| Penyesuaian warna | Hue, saturasi, kecerahan, kontras, gamma, dim |
| Hemat daya | Mode gambar diam, jeda saat idle, opsi jeda di layar password, dan penghemat baterai rendah (ringan, seimbang, maksimum) |
| Perilaku saat boot | Opsional menampilkan frame terakhir selama waktu yang dipilih tepat setelah boot tanpa decoding |
| Layar shutdown | Opsional menampilkan video di layar "Shutting down" / "Restarting" (mati secara default, sehingga layar hitam bawaan Windows yang tampil) |
| Jendela preview | Jendela opsional yang memutar videomu dengan pengaturan fit, warna, dan kecepatan saat ini, dibuka lewat pengaturan, hotkey, atau tray icon. Bisa menggambar salinan jam, tanggal, dan ikon status asli di atas videonya. Jalan di dalam `explorer.exe`, jadi mod ini juga dimuat di sana. Tampilannya perkiraan dari lapisan wallpaper, bukan UI lock screen Windows itu sendiri |
| Fail-safe | Mengembalikan wallpaper asli jika UI lock screen berubah atau mod bermasalah |
| Debug log | Opsional: statistik CPU dan decode serta dump UI tree |

| ![preview window](https://i.imgur.com/KFW2K91.gif) |
| :---: |
| *The preview window playing the video, with the clock, date and status icons drawn over it.* |

## 📝 Catatan beberapa pengaturan

| Pengaturan atau topik | Keterangan |
| --- | --- |
| **Folder order** | Dipakai kalau path-nya folder. *Random* memilih video di setiap lock, *Sequential* memutar video berikutnya (urut abjad) di setiap lock lalu mengulang dari awal setelah yang terakhir, *Fixed* selalu memakai yang pertama (urut abjad). File GIF di folder dianggap video. |
| **Audio volume** | 0 berarti senyap. Di atas 0, audio hanya diputar pada kecepatan 100% dan hanya saat video benar-benar berjalan (bukan saat dijeda, dijeda karena idle, atau menampilkan frame diam setelah boot). Audio masuk perlahan saat pemutaran mulai atau lanjut dan keluar perlahan saat ditahan, sekitar 0,6 detik. |
| **Dim** | 0 sampai 90. Menggelapkan video supaya jam dan layar sign-in lebih mudah dibaca. 0 berarti mati. |
| **Render width** | Lebar kanvas internal (320 sampai 1920). Tinggi mengikuti rasio aspek layarmu. Ini pengaturan yang paling berpengaruh pada CPU. |
| **Hardware video decoding** | Aktif (default) berarti video di-decode oleh GPU jika memungkinkan, sehingga pemakaian CPU turun jauh untuk video 4K atau 60 fps. Matikan jika pemutaran bermasalah atau layar sign-in bermasalah. Jika jalur GPU gagal menghasilkan frame, mod otomatis beralih ke decoding software. Mengubah setting ini juga menyalakan kembali pemutaran jika crash guard sempat mematikannya. |
| **Pause when idle** | Membekukan video dan membisukan audio setelah waktu tertentu tanpa input, lalu lanjut saat ada input berikutnya. |
| **Battery saver starts at** dan **Battery saver mode** | Isi ambang di atas 0 untuk menyalakan penghemat baterai. Saat baterai berada di ambang itu atau di bawahnya (dan PC tidak dicolok, kecuali **Also apply while charging** aktif), mode yang dipilih diterapkan. *Light* membatasi pemutaran sekitar 30 fps (hanya berpengaruh untuk video 60 fps), *Balanced* merender selebar 960 px dan membatasi sekitar 15 fps, *Maximum* membekukan di satu frame. Baterai dicek setiap 5 detik. Lebar render dipilih saat lock screen mulai, jadi baru berlaku di lock berikutnya; batas frame rate dan pembekuan berlaku langsung. |
| **Play video on the password screen** | Aktif (default) berarti video tetap diputar saat kamu mengetik password. Jika dimatikan, video dibekukan di layar password (berguna jika video patah-patah di sana), dan diputar lagi di lock screen. |
| **Behavior right after boot** dan **Still frame duration after boot** | Dengan **Show last frame**, beberapa menit pertama setelah boot menampilkan frame terakhir yang tersimpan tanpa decoding. Lamanya diatur oleh **Still frame duration after boot** (10 sampai 1800 detik, default 180). Butuh minimal satu sesi sebelumnya di mana video sempat diputar. |
| **Show preview window** | Membuka jendela preview dan membiarkannya tetap terbuka. Menyimpan pengaturan me-refresh jendelanya tanpa menutupnya, kecuali opsi preview itu sendiri yang berubah. Ukuran jendela bisa diubah (rasio aspek tetap), dan ukuran serta posisinya diingat. `Esc` atau tombol tutup menyembunyikannya, dan preview yang tersembunyi atau di-minimize tidak men-decode apa pun. |
| **Preview hotkey** | Pintasan global untuk menampilkan atau menyembunyikan jendela preview. Berupa kolom teks dengan isi default `Ctrl+Alt+P`: hapus teksnya untuk mematikan hotkey, atau ketik kombinasimu sendiri, misalnya `Ctrl+Alt+Shift+P` atau `Ctrl+Shift+F12`. Tulis modifier (`Ctrl`, `Alt`, `Shift`, `Win`) dan satu tombol yang digabung dengan `+`; minimal satu modifier wajib ada. Tombolnya bisa huruf, angka, `F1` sampai `F24`, `Space`, `Tab`, `Enter`, `Esc`, `Insert`, `Delete`, `Home`, `End`, `PgUp`, `PgDn`, atau tombol panah (`Up`, `Down`, `Left`, `Right`). Banyak kombinasi dengan `Win` sudah dipakai Windows sendiri. Teks yang tidak terbaca diabaikan dan log menampilkan `is not valid`. Kalau kombinasinya sudah dipakai program lain, Windows menolaknya dan log menampilkan `RegisterHotKey failed`. Selama hotkey aktif, mod menjaga satu jendela tersembunyi kecil di dalam `explorer.exe` untuk mendengarkannya. |
| **Tray icon** | Klik kiri menampilkan atau menyembunyikan jendela preview, klik kanan membuka menu kecil. |
| **Preview stays on top** | Menjaga jendela preview tetap di atas jendela lain. Kalau dimatikan, jendela lain bisa menutupinya. |
| **Clock, date and status icons in the preview** | Jam, tanggal, dan ikon status digambar oleh `LockApp.exe`, bukan oleh `LogonUI.exe`. Mod menangkapnya dari lock screen asli setiap kali kamu mengunci PC (tunggu sekitar 6 detik tanpa menyentuh apa pun), dan preview memakai salinan terbaru, jadi jam di preview menunjukkan waktu lock terakhir. Saat merendernya, jam dan tanggal asli bergeser turun sepersekian detik; matikan opsi ini kalau mengganggu, dan mod berhenti menangkap. Sebelum salinannya ada, preview menuliskannya di bagian bawah. |
| **Skala tampilan dan layout** | Jam, tanggal, dan ikon di preview hanya diuji pada skala tampilan 100%, 125%, dan 150% dengan layout lock screen standar Windows 11; di layout lain posisinya bisa kurang tepat. Di PC tempat mod ini dikembangkan, Windows sendiri tidak menampilkan ikon baterai dan jaringan di lock screen pada skala di atas 100%, dan preview ikut tidak menampilkannya. |
| **Debug log** | Juga menulis `debug.log` di folder penyimpanan mod, berisi statistik CPU dan decode setiap 5 detik (hanya untuk video, GIF menampilkan 0 ms) dan dump UI tree jendela lock screen (`LogonUI.exe` dan `LockApp.exe`). Dump itu bisa membuat video tersendat sesaat di awal lock, jadi biarkan mati kecuali kamu sedang mendiagnosis masalah. Peristiwa dasar selalu masuk ke log Windhawk (aktifkan logging untuk mod ini di Windhawk untuk melihatnya). |
| **Pemutaran lama** | Video diulang tanpa batas waktu. Secara default video dibekukan setelah 5 menit tanpa input (lihat **Pause when idle**); setel ke 0 kalau kamu ingin video terus berjalan saat tidak ada orang di depan PC. |
| **Jeda saat mulai** | LogonUI dimulai dari awal di setiap lock, jadi video butuh sekitar 2 sampai 4 detik sebelum mulai bergerak; selama itu frame pertama dari cache ditampilkan (GIF mulai hampir langsung). Lock pertama setelah mengganti video, **Render width**, atau **Fit mode** belum punya frame cache dan bisa menampilkan wallpaper asli sebentar. Di mode folder, tiap video punya frame cache sendiri saat pertama kali terpilih. Mengubah pengaturan warna saja tidak membutuhkan frame cache baru. |

## 📈 Tips performa

- Turunkan **Render width** (defaultnya 1920; misalnya coba 1280 atau 960).
- Video 4K atau 60 fps di-decode oleh GPU saat **Hardware video decoding** aktif (default). Jika dimatikan, video di-decode oleh CPU dan bisa memakai beberapa core; pilih 1080p dengan 30 fps.
- Gunakan **Pause** jika kamu hanya ingin gambar diam.
- Biarkan **Pause when idle** aktif agar tidak ada decoding saat tidak ada orang di depan PC.
- Pakai penghemat baterai di laptop.
- Jika video patah-patah saat mengetik password, matikan **Play video on the password screen**.
- Layar password mem-blur wallpaper di belakangnya. Latar yang terus berubah membuat Windows mengulang blur itu di setiap frame, yang memakan sebagian GPU.

## 🔧 Pemecahan masalah

- **Wallpaper normal yang tampil**: pastikan `LogonUI.exe` ada di daftar inklusi proses dan path video benar, lalu aktifkan **Debug log** dan baca `debug.log` di folder penyimpanan mod (lihat "File yang ditulis oleh mod"). Cari `Failed to open video`, `Unsupported video format`, atau `FAIL-SAFE TRIPPED`.
- **Video tidak mau terbuka**: coba MP4 H.264 yang disimpan di tempat yang bisa dibaca akun SYSTEM.
- **Tidak ada suara**: setel **Audio volume** di atas 0 dan baca catatan di atas.
- **Patah-patah**: turunkan render width dan aktifkan **Debug log**, lalu cek baris `CPU` dan `UI stalls` di log.
- **Hotkey preview tidak berfungsi**: pastikan kolom **Preview hotkey** tidak kosong dan ditulis seperti `Ctrl+Alt+P` (cari `is not valid` di log Windhawk), lalu cari `RegisterHotKey failed`. Bisa jadi program lain sudah memakai kombinasi itu; ketik kombinasi lain.
- **Preview bilang belum ada overlay**: kunci PC sekali (Win+L) dan tunggu sekitar 6 detik tanpa menyentuh apa pun.
- **Jam, tanggal, atau ikon di preview posisinya meleset**: aktifkan **Debug log**, kunci PC, tunggu sekitar 6 detik, lalu lihat baris yang diawali `Overlay:` di log.
- **Masih bermasalah?**: lihat bagian **Laporkan bug** di bawah.

## 🐞 Laporkan bug

Kalau ada yang tidak berfungsi atau kamu punya saran fitur, silakan buka issue di [repo mod ini](https://github.com/grn15RenS/lock-screen-live-wallpaper/issues/) atau di [repo mod Windhawk](https://github.com/ramensoftware/windhawk-mods/issues). Supaya penyebabnya lebih cepat ketemu, sertakan:

- versi Windows 11 kamu (misalnya 25H2)
- `debug.log` (aktifkan **Debug log** dulu, lihat "File yang ditulis oleh mod" di bawah)
- log Windhawk (aktifkan logging untuk mod ini di Windhawk)

Log berisi path videomu, jadi hapus bagian pribadi dulu kalau perlu.

## 🤔 Cara kerja

1. Worker thread mencari `LogonUI Logon Window` dan memasang hook `WH_GETMESSAGE` pada UI thread-nya.
2. Decoder thread membaca video sebagai RGB32 lewat Media Foundation (atau GIF lewat WIC), mengubah ukuran tiap frame sesuai mode fit, menerapkan pengaturan warna, lalu menaruhnya di buffer bersama.
3. Di UI thread, frame terbaru disalin ke `WriteableBitmap`, yang dipasang sebagai source elemen `LockLogonImage` dan `SwapLockLogonImage`.
4. Saat mod di-unload, source gambar asli dikembalikan.
5. Jendela preview berada di `explorer.exe` dan menjalankan decoder sendiri dengan pengaturan yang sama.
6. Di `LockApp.exe` mod membaca UI lock screen. Beberapa detik setelah lock screen muncul, mod merender jam, tanggal, dan ikon status ke bitmap transparan pada skala yang dipakai XAML (dengan padding sementara di sekitar jam dan tanggal supaya glyph besar tidak terpotong, sehingga keduanya bergeser sepersekian detik), menaruhnya di kanvas transparan seukuran layar, lalu menyimpannya sebagai `overlay.bgra` agar preview menggambarnya di atas video.

## 🛡️ Fail-safe

Mod berhenti sendiri dan mengembalikan wallpaper asli sampai proses `LogonUI.exe` itu berakhir, jika:

- decoder video gagal sebelum menghasilkan frame apa pun,
- tidak ada frame video yang dihasilkan dalam 30 detik,
- elemen lock screen (`LockLogonImage` / `SwapLockLogonImage`) tidak pernah ditemukan, yang biasanya berarti update Windows mengubah UI, atau
- UI melempar error berulang (3 kali dalam 30 detik).

Selain itu, ada pelindung crash untuk layar sign-in. Jika dua sesi `LogonUI.exe` berturut-turut berakhir sebelum decoder menampilkan frame pertama dan sebelum layar password terbuka minimal 1 detik (misalnya karena codec crash pada videomu), mod mematikan pemutaran dan wallpaper normal yang tampil. Untuk menyalakannya lagi, ubah **Video file or folder**, **Render width**, **Fit mode** atau **Hardware video decoding** ke nilai yang berbeda dan biarkan begitu. Sesi yang sangat singkat (misalnya dua kali layar Ctrl+Alt+Del berturut-turut) masih bisa memicunya, begitu juga unlock yang sangat cepat lewat Windows Hello atau pembaca sidik jari (belum diuji); solusinya sama.

## 📁 File yang ditulis oleh mod

Semuanya disimpan di folder penyimpanan mod ini, di dalam folder data Windhawk (path di bawah untuk instalasi biasa; Windhawk portabel menyimpannya di tempat lain):

`C:\ProgramData\Windhawk\Engine\ModsWritable\mod-storage\lock-screen-live-wallpaper`

`ProgramData` adalah folder tersembunyi, jadi tempel path di atas ke address bar File Explorer untuk membukanya. Isinya:

- `debug.log`: file log, dipangkas otomatis setelah 2 MB
- `frame_*.bgra`: cache frame pertama, agar lock screen bisa langsung menampilkan sesuatu. File lebih tua dari 30 hari dihapus, dan hanya 5 file terbaru lainnya disimpan
- `last.bgra`: frame terakhir yang ditampilkan, dipakai oleh mode boot **Show last frame**
- `overlay.bgra`: jam, tanggal, dan ikon status yang sudah ditangkap, dipakai oleh jendela preview

Kamu boleh menghapus file-file ini kapan saja; mod akan membuat ulang yang dibutuhkan. Folder ini dihapus otomatis saat mod di-uninstall.

## ☕ Dukungan

Kalau kamu suka mod ini, kamu bisa mendukung pengembangannya lewat:

- [Saweria](https://saweria.co/grn15RenS) (untuk Indonesia)
- [Ko-fi](https://ko-fi.com/grn15rens) (internasional)

## ℹ️ Peringatan

Hanya diuji di Windows 11 23H2 dan 25H2. Mod ini berjalan di dalam proses UI sistem (`LogonUI.exe`), jadi sudah dilengkapi fail-safe yang mengembalikan wallpaper asli jika ada yang tidak beres. Sebagai langkah berjaga-jaga, siapkan cara lain untuk masuk ke Windows saat mencobanya.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- general:
  - videoPath: ""
    $name: Video file or folder
    $description: Full path to a video file (mp4 recommended), or a folder with videos (one is picked at random each lock).
  - folderOrder: random
    $name: Folder order
    $description: When the path is a folder, how the video is chosen on each lock.
    $options:
    - random: Random
    - sequential: Sequential (next video on every lock, alphabetical)
    - fixed: Fixed (always the first video, alphabetical)
  - fitMode: cover
    $name: Fit mode
    $description: How the video is scaled to the screen.
    $options:
    - cover: Cover (fill the screen, crop the edges)
    - fill: Fill (stretch, may distort)
    - fit: Fit (letterbox, keep aspect ratio)
    - center: Center (original pixel size, no scaling)
  - audioVolume: 0
    $name: Audio volume (%)
    $description: 0 to 100. 0 = muted (default). Above 0 plays the audio track of the video, only at 100% video speed while the video is playing (not paused, and not while the last frame is shown after boot). Scales the mod's own audio before it reaches Windows, so it is separate from other apps. The system master volume still applies on top.
  - speed: 100
    $name: Video speed (%)
    $description: 25 to 400. 100 = normal speed.
  $name: General
- colorAdjustments:
  - hue: 0
    $name: Hue shift (degrees)
    $description: -180 to 180. 0 = original colors.
  - saturation: 100
    $name: Saturation (%)
    $description: 0 to 300. 0 = black and white, 100 = original.
  - brightness: 0
    $name: Brightness
    $description: -100 to 100. 0 = original.
  - contrast: 100
    $name: Contrast (%)
    $description: 25 to 300. 100 = original.
  - gamma: 100
    $name: Gamma
    $description: 20 to 300 (100 = 1.0).
  - dim: 0
    $name: Dim (%)
    $description: 0 to 90. Darkens the video. 0 = off.  
  $name: Color adjustments
- performance:
  - width: 1920
    $name: Render width
    $description: Internal width in pixels, 320 to 1920 (the height follows your screen's aspect ratio). Lower = less CPU, higher = sharper. Common values are 960, 1280 and 1920.
  - hwDecode: true
    $name: Hardware video decoding
    $description: Decode the video on the GPU (D3D11/DXVA) when possible. This greatly lowers CPU use for 4K or 60 fps videos. Turn it off if playback glitches or the sign-in screen has problems. Falls back to software decoding automatically if the GPU cannot decode the video.
  $name: Performance
- power:
  - paused: false
    $name: Pause (show a still frame)
    $description: Show the first frame of the video as a still image instead of playing it. Almost no CPU usage.
  - idlePauseSeconds: 300
    $name: Pause when idle (seconds)
    $description: Freeze the video and mute the audio after this many seconds without keyboard or mouse input. It resumes on the next input. 0 = never.
  - lowBatteryPercent: 0
    $name: Battery saver starts at (%)
    $description: 0 to 100. When the PC is unplugged (or always, if "Also apply while charging" is on) and the battery is at or below this level, the battery saver mode below is applied. 0 = never.
  - lowBatteryLevel: balanced
    $name: Battery saver mode
    $description: How much the mod saves power once the threshold is reached. The render width is decided when the lock screen starts, so it applies from the next lock. The frame rate cap and the pause apply live.
    $options:
    - off: Off (no change)
    - light: Light (cap at 30 fps, only matters for 60 fps videos)
    - balanced: Balanced (960 px wide, cap at 15 fps)
    - maximum: Maximum (freeze on a still frame)
  - lowBatteryOnAC: false
    $name: Also apply while charging
    $description: Off = the battery saver only applies when the PC is unplugged. On = it also applies while the charger is connected (the threshold still decides when).
  $name: Pause and power saving
- screens:
  - playOnPasswordScreen: true
    $name: Play video on the password screen
    $description: On = the video keeps playing while you type your password. Off = the video freezes on the password screen, which can help if playback stutters there. It plays again on the lock screen.
  - showOnShutdown: false
    $name: Show video on shutdown / restart screen
    $description: Show the video on the "Shutting down" / "Restarting" status screen. Off (default) = Windows' own background for that screen (usually plain black) is shown instead.
  - bootMode: still
    $name: Behavior right after boot
    $description: "Show last frame = for the first minutes after boot (the duration is set below), the last saved frame is shown without decoding. Play video = the video plays immediately."
    $options:
    - still: Show last frame
    - play: Play video
  - bootStillSeconds: 180
    $name: Still frame duration after boot (seconds)
    $description: 10 to 1800. How long after boot the last saved frame is shown without decoding (only when "Behavior right after boot" is "Show last frame").  
  $name: Screens and startup
- preview:
  - enabled: false
    $name: Show preview window
    $description: Opens a window that plays your video with the current fit, color and speed settings, so you can check them without locking the PC. It runs inside explorer.exe and refreshes every time you save the settings. It shows the wallpaper only, not the Windows lock screen UI.
  - hotkey: Ctrl+Alt+P
    $name: Preview hotkey
    $description: Global shortcut that shows or hides the preview window. Write the modifiers (Ctrl, Alt, Shift, Win) and one key joined with +, for example Ctrl+Alt+Shift+P or Ctrl+Shift+F12. At least one modifier is needed. Clear the text to turn the hotkey off.
  - trayIcon: false
    $name: Tray icon
    $description: Adds a tray icon. Left click shows or hides the preview window, right click opens a small menu.
  - alwaysOnTop: true
    $name: Preview stays on top
    $description: Keeps the preview window above other windows.
  - showOverlay: true
    $name: Show clock, date and status icons in the preview
    $description: Draws the clock, date, battery and network icons over the preview video. They are captured from the real lock screen every time you lock the PC (wait about 6 seconds without touching anything) and the newest copy is used. While capturing, the real clock and date move down for a split second; turn this off to stop capturing. Needs the mod to be loaded in LockApp.exe.    
  $name: Preview
- debugLog: false
  $name: Debug log
  $description: Also write debug.log with CPU and decode statistics every 5 seconds and a one-time dump of the lock screen UI tree. In a regular Windhawk install it is saved in C:\ProgramData\Windhawk\Engine\ModsWritable\mod-storage\lock-screen-live-wallpaper (ProgramData is hidden, so paste this path into the File Explorer address bar). A portable Windhawk keeps it inside its own folder.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <shellapi.h>
#include <uxtheme.h>
#undef GetCurrentTime
#include <timeapi.h>

#include <mfapi.h>
#include <mfidl.h>
#include <mfreadwrite.h>
#include <mmreg.h>
#include <mmdeviceapi.h>
#include <audioclient.h>
#include <wincodec.h>
#include <d3d11_4.h>

#include <algorithm>
#include <atomic>
#include <cstdarg>
#include <cmath>
#include <string>
#include <vector>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Storage.Streams.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.Media.Imaging.h>

using namespace winrt::Windows::UI::Xaml;

struct IBufferByteAccess : ::IUnknown {
    virtual HRESULT __stdcall Buffer(uint8_t** value) = 0;
};
static const GUID IID_IBufferByteAccess_ =
    { 0x905a0fef, 0xbc53, 0x11df,
      { 0x8c, 0x49, 0x00, 0x1e, 0x4f, 0xc6, 0x86, 0xda } };
static const GUID kGuidNull = { 0, 0, 0, { 0, 0, 0, 0, 0, 0, 0, 0 } };
static bool g_debug = false;
static bool g_previewRole = false;   // true inside explorer.exe: hosts the preview window, never touches the lock screen
static SRWLOCK g_logLock = SRWLOCK_INIT;
static wchar_t kCacheDir[MAX_PATH] = L"";  // mod storage folder, set in Wh_ModInit
static wchar_t g_logPath[MAX_PATH] = L"";
static void FileLog(const wchar_t* fmt, ...) {
    wchar_t msg[1024];
    va_list ap;
    va_start(ap, fmt);
    _vsnwprintf(msg, 1023, fmt, ap);
    va_end(ap);
    msg[1023] = 0;
    Wh_Log(L"%s", msg);
    if (!g_debug) return;
    SYSTEMTIME st;
    GetLocalTime(&st);
    wchar_t line[1200];
    _snwprintf(line, 1199, L"%02d:%02d:%02d.%03d up=%llus pid=%lu %s\r\n",
               st.wHour, st.wMinute, st.wSecond, st.wMilliseconds,
               GetTickCount64() / 1000, GetCurrentProcessId(), msg);
    line[1199] = 0;
    char u8[3000];
    int n = WideCharToMultiByte(CP_UTF8, 0, line, -1, u8, sizeof(u8), nullptr, nullptr);
    if (n <= 1) return;
    if (!g_logPath[0]) return;
    AcquireSRWLockExclusive(&g_logLock);
    CreateDirectoryW(kCacheDir, nullptr);
    WIN32_FILE_ATTRIBUTE_DATA fa;
    if (GetFileAttributesExW(g_logPath,
                             GetFileExInfoStandard, &fa) &&
        (fa.nFileSizeHigh || fa.nFileSizeLow > 2 * 1024 * 1024))
        DeleteFileW(g_logPath);
    HANDLE f = CreateFileW(g_logPath,
                           FILE_APPEND_DATA, FILE_SHARE_READ | FILE_SHARE_WRITE,
                           nullptr, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (f != INVALID_HANDLE_VALUE) {
        DWORD w = 0;
        WriteFile(f, u8, (DWORD)(n - 1), &w, nullptr);
        CloseHandle(f);
    }
    ReleaseSRWLockExclusive(&g_logLock);
}

static int kW = 960, kH = 540;
static size_t g_frameBytes = 0;
static std::wstring g_videoFile;

static HWND g_wnd = nullptr;
static std::atomic<HHOOK> g_hook{nullptr};
static std::atomic<bool> g_stop{false};
static HANDLE g_thread = nullptr;
static HANDLE g_decThread = nullptr;

static CRITICAL_SECTION g_cs;
static std::vector<uint8_t> g_shared;
static volatile LONG g_seq = 0;
static LONG g_lastSeq = 0;

[[clang::no_destroy]] static Media::Imaging::WriteableBitmap g_wb{nullptr};
[[clang::no_destroy]] static Controls::Image g_imgA{nullptr};
[[clang::no_destroy]] static Controls::Image g_imgB{nullptr};
static uint8_t* g_pixels = nullptr;
static std::atomic<bool> g_teardown{false}, g_tornDown{false};
static std::atomic<DWORD> g_lightUntil{0};
static bool g_bootStill = false, g_saveLast = false;
static DWORD g_lastLog = 0, g_lastFind = 0;
static ULONGLONG g_cpuPrev = 0, g_wallPrev = 0;
// Fit mode, audio, playback hold (idle / hidden), shutdown screen
enum FitMode { kFitCover = 0, kFitFill, kFitFit, kFitCenter };
static int g_fitMode = kFitCover;
static int g_screenW = 1920, g_screenH = 1080;
static bool g_audioWanted = false;
static int g_audioVol = 50;
static HANDLE g_audioThread = nullptr;
static std::atomic<bool> g_decoderDone{false};
static std::atomic<LONG> g_loopSerial{0};      // incremented by the decoder on every video loop
static std::atomic<bool> g_idlePaused{false};  // playback hold: idle or hidden (written by the UI thread)
static DWORD g_idleMs = 0;
static DWORD g_lastInput = 0;                  // UI thread only
static bool g_hideOnShutdown = false;
static bool g_playOnPassword = true;
static int g_dim = 0;
static int g_folderOrder = 0;                  // 0 random, 1 sequential, 2 fixed
static int g_pauseBatPct = 0;
static int g_lowBatLevel = 2;                  // 0 off, 1 light, 2 balanced, 3 maximum
static bool g_lowBatOnAC = false;
static std::atomic<bool> g_lowBat{false};      // unplugged and at/below the threshold
static std::atomic<bool> g_batHold{false};     // battery hold (written by the UI thread)
static DWORD g_bootStillMs = 180000;
static bool g_pwScene = false;                 // UI thread only
static DWORD g_pwGoneTick = 0;                 // UI thread only: when the password screen last closed
static DWORD g_pwShownTick = 0;                // UI thread only: when the password screen last opened
static bool g_msgSceneUp = false;              // UI thread only

// ---------- Fail-safe ----------
// All state below is only touched from the UI thread (GetMsgProc -> Step),
// except g_failed / g_decodeDead, which are atomic.
static std::atomic<bool> g_failed{false};      // fail-safe tripped, mod is off for good
static std::atomic<bool> g_decodeDead{false};  // decoder exited without ever producing a frame
static const bool g_failSafe = true;           // always on
static const DWORD g_failSafeMs = 30000;       // give up after 30 seconds
static DWORD g_initTick = 0;                   // tick count at ModInit
static bool g_everFound = false;               // the UI elements were found at least once
static DWORD g_missSince = 0;                  // since when the elements are missing
static int g_missTries = 0;                    // consecutive failed lookups
static int g_errCount = 0;                     // exceptions inside the time window
static DWORD g_errWindowStart = 0;
static DWORD g_lastErrTick = 0;
static const int kMinMissTries = 20;           // minimum lookup attempts before giving up
static const int kMaxErrors = 3;               // max exceptions within 30 seconds
static const DWORD kErrWindowMs = 30000;
static const DWORD kErrThrottleMs = 500;       // errors closer than this count as one

// Diagnostics
static double g_decMsSum = 0, g_decMsMax = 0, g_readMsSum = 0;
static int g_decN = 0;
static double g_lastStepMs = 0, g_stallMax = 0;
static int g_stalls = 0;

// Pause and cache
static bool g_paused = false;
static bool g_cacheHit = false;
static std::atomic<bool> g_needSave{false};
static std::wstring g_cacheFile;

double NowMs() {
    static LARGE_INTEGER f = {};
    if (!f.QuadPart) QueryPerformanceFrequency(&f);
    LARGE_INTEGER c;
    QueryPerformanceCounter(&c);
    return (double)c.QuadPart * 1000.0 / (double)f.QuadPart;
}

static void UnhookOnce() {
    HHOOK h = g_hook.exchange(nullptr);
    if (h) UnhookWindowsHookEx(h);
}

// ---------- Color and speed settings ----------
static int g_speed = 100;
static int g_hue = 0, g_sat = 100, g_bri = 0, g_con = 100, g_gam = 100;
static int g_m[9];
static uint8_t g_lut[256];
static bool g_identity = true;
static bool g_lutOnly = false;   // hue=0 and sat=100: the LUT alone is enough, skip the matrix

int ClampInt(int v, int lo, int hi) { return v < lo ? lo : (v > hi ? hi : v); }

void LoadColorSettings() {
    g_speed = ClampInt(Wh_GetIntSetting(L"general.speed"), 25, 400);
    g_hue = ClampInt(Wh_GetIntSetting(L"colorAdjustments.hue"), -180, 180);
    g_sat = ClampInt(Wh_GetIntSetting(L"colorAdjustments.saturation"), 0, 300);
    g_bri = ClampInt(Wh_GetIntSetting(L"colorAdjustments.brightness"), -100, 100);
    g_con = ClampInt(Wh_GetIntSetting(L"colorAdjustments.contrast"), 25, 300);
    g_gam = ClampInt(Wh_GetIntSetting(L"colorAdjustments.gamma"), 20, 300);
    g_dim = ClampInt(Wh_GetIntSetting(L"colorAdjustments.dim"), 0, 90);    

    g_lutOnly = (g_hue == 0 && g_sat == 100);
    g_identity = (g_hue == 0 && g_sat == 100 && g_bri == 0 && g_con == 100 &&
                  g_gam == 100 && g_dim == 0);

    double a = g_hue * 3.14159265358979 / 180.0;
    double c = cos(a), s = sin(a);
    double H[3][3] = {
        { 0.213 + c * 0.787 - s * 0.213, 0.715 - c * 0.715 - s * 0.715,
          0.072 - c * 0.072 + s * 0.928 },
        { 0.213 - c * 0.213 + s * 0.143, 0.715 + c * 0.285 + s * 0.140,
          0.072 - c * 0.072 - s * 0.283 },
        { 0.213 - c * 0.213 - s * 0.787, 0.715 - c * 0.715 + s * 0.715,
          0.072 + c * 0.928 + s * 0.072 }
    };
    double lum[3] = { 0.213, 0.715, 0.072 };
    double sv = g_sat / 100.0;
    double S[3][3];
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            S[i][j] = (1.0 - sv) * lum[j] + (i == j ? sv : 0.0);
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++) {
            double v = 0;
            for (int k = 0; k < 3; k++) v += S[i][k] * H[k][j];
            g_m[i * 3 + j] = (int)lround(v * 256.0);
        }

    double contrast = g_con / 100.0, bright = g_bri / 100.0,
           gamma = g_gam / 100.0;
    for (int i = 0; i < 256; i++) {
        double x = i / 255.0;
        x = (x - 0.5) * contrast + 0.5 + bright;
        if (x < 0) x = 0;
        if (x > 1) x = 1;
        x = pow(x, 1.0 / gamma);
        x *= 1.0 - g_dim / 100.0;        
        g_lut[i] = (uint8_t)lround(x * 255.0);
    }
    FileLog(L"Color: speed=%d hue=%d sat=%d bri=%d con=%d gamma=%d%s", g_speed,
           g_hue, g_sat, g_bri, g_con, g_gam,
           g_identity ? L" (no effect)" : L"");
}

static inline uint32_t Fx(uint32_t p) {
    int r = (p >> 16) & 255, g = (p >> 8) & 255, b = p & 255;
    int nr = (g_m[0] * r + g_m[1] * g + g_m[2] * b) >> 8;
    int ng = (g_m[3] * r + g_m[4] * g + g_m[5] * b) >> 8;
    int nb = (g_m[6] * r + g_m[7] * g + g_m[8] * b) >> 8;
    nr = nr < 0 ? 0 : (nr > 255 ? 255 : nr);
    ng = ng < 0 ? 0 : (ng > 255 ? 255 : ng);
    nb = nb < 0 ? 0 : (nb > 255 ? 255 : nb);
    return 0xFF000000u | ((uint32_t)g_lut[nr] << 16) |
           ((uint32_t)g_lut[ng] << 8) | g_lut[nb];
}

static inline uint32_t FxLut(uint32_t p) {
    return 0xFF000000u | ((uint32_t)g_lut[(p >> 16) & 255] << 16) |
           ((uint32_t)g_lut[(p >> 8) & 255] << 8) | g_lut[p & 255];
}

// Applies the color settings to a finished canvas-size frame (in place).
// Letterbox bars (fit / center modes) stay pure black.
static void ApplyColor(uint32_t* px, size_t n) {
    if (g_identity) return;
    const bool keepBlack = (g_fitMode == kFitFit || g_fitMode == kFitCenter);
    for (size_t i = 0; i < n; i++) {
        uint32_t p = px[i];
        if (keepBlack && p == 0xFF000000u) continue;
        px[i] = g_lutOnly ? FxLut(p) : Fx(p);
    }
}

// ---------- Path utilities ----------
std::wstring Clean(std::wstring s) {
    auto isJunk = [](wchar_t c) { return c == L'"' || c == L' ' || c == L'\t'; };
    while (!s.empty() && isJunk(s.back())) s.pop_back();
    while (!s.empty() && isJunk(s.front())) s.erase(s.begin());
    return s;
}

bool IsVideoExt(const std::wstring& n) {
    size_t p = n.find_last_of(L'.');
    if (p == std::wstring::npos) return false;
    std::wstring e = n.substr(p + 1);
    for (auto& c : e) c = (wchar_t)towlower(c);
    return e == L"mp4" || e == L"m4v" || e == L"mov" || e == L"wmv" ||
           e == L"avi" || e == L"mkv" || e == L"gif";
}

std::wstring PickVideo(const std::wstring& path) {
    DWORD attr = GetFileAttributesW(path.c_str());
    if (attr == INVALID_FILE_ATTRIBUTES) {
        FileLog(L"Path not found: %s", path.c_str());
        return L"";
    }
    if (!(attr & FILE_ATTRIBUTE_DIRECTORY)) return path;

    std::vector<std::wstring> files;
    WIN32_FIND_DATAW fd;
    HANDLE h = FindFirstFileW((path + L"\\*").c_str(), &fd);
    if (h != INVALID_HANDLE_VALUE) {
        do {
            if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) &&
                IsVideoExt(fd.cFileName))
                files.push_back(path + L"\\" + fd.cFileName);
        } while (FindNextFileW(h, &fd));
        FindClose(h);
    }
    if (files.empty()) {
        FileLog(L"No video files in folder: %s", path.c_str());
        return L"";
    }
    std::sort(files.begin(), files.end(), [](const std::wstring& a, const std::wstring& b) {
        return _wcsicmp(a.c_str(), b.c_str()) < 0;
    });
    if (g_folderOrder == 2) return files[0];   // fixed
    if (g_folderOrder == 1) {                  // sequential
        int idx = Wh_GetIntValue(L"folderIndex", 0);
        if (idx < 0 || idx >= (int)files.size()) idx = 0;
        if (!g_previewRole) Wh_SetIntValue(L"folderIndex", idx + 1);   // the preview must not advance the lock screen's sequence
        return files[idx];
    }
    return files[GetTickCount() % files.size()];
}

// ---------- First-frame cache and last-frame persistence ----------
struct CacheHdr { uint32_t magic, w, h, pad; };
static const uint32_t kMagic = 0x4C565734;  // "LVW4"

uint64_t Fnv(const std::wstring& s) {
    uint64_t h = 1469598103934665603ULL;
    for (wchar_t c : s) { h ^= (uint64_t)c; h *= 1099511628211ULL; }
    return h;
}

void BuildCachePath() {
    // The cache holds the raw (uncolored) first frame, so color settings are not part of the key.
    std::wstring key = g_videoFile + L"|" + std::to_wstring(kW) + L"|" + std::to_wstring(kH) + L"|" +
          std::to_wstring(g_fitMode) + L"|raw";        
    uint64_t h = Fnv(key);
    wchar_t name[64];
    wsprintfW(name, L"frame_%08X%08X.bgra", (unsigned)(h >> 32), (unsigned)h);
    g_cacheFile = std::wstring(kCacheDir) + L"\\" + name;
}

bool LoadCache() {
    HANDLE f = CreateFileW(g_cacheFile.c_str(), GENERIC_READ, FILE_SHARE_READ,
                           nullptr, OPEN_EXISTING, 0, nullptr);
    if (f == INVALID_HANDLE_VALUE) return false;
    CacheHdr hd = {};
    DWORD got = 0;
    bool ok = ReadFile(f, &hd, sizeof(hd), &got, nullptr) && got == sizeof(hd) &&
              hd.magic == kMagic && hd.w == (uint32_t)kW && hd.h == (uint32_t)kH;
    if (ok) {
        ok = ReadFile(f, g_shared.data(), (DWORD)g_frameBytes, &got, nullptr) &&
             got == g_frameBytes;
    }
    CloseHandle(f);
    if (ok) g_seq = 1;
    return ok;
}

// Deletes frame_*.bgra older than 30 days, then keeps only the 5 newest others.
// The file for the current settings (g_cacheFile) is never touched.
void PruneCache() {
    std::wstring pat = std::wstring(kCacheDir) + L"\\frame_*.bgra";
    WIN32_FIND_DATAW fd;
    HANDLE h = FindFirstFileW(pat.c_str(), &fd);
    if (h == INVALID_HANDLE_VALUE) return;
    struct Entry { std::wstring path; ULONGLONG t; };
    std::vector<Entry> v;
    FILETIME nowFt;
    GetSystemTimeAsFileTime(&nowFt);
    ULONGLONG now = ((ULONGLONG)nowFt.dwHighDateTime << 32) | nowFt.dwLowDateTime;
    const ULONGLONG kMaxAge = 30ULL * 24 * 3600 * 10000000ULL;  // 100 ns units
    do {
        std::wstring p = std::wstring(kCacheDir) + L"\\" + fd.cFileName;
        if (p == g_cacheFile) continue;
        ULONGLONG t = ((ULONGLONG)fd.ftLastWriteTime.dwHighDateTime << 32) |
                      fd.ftLastWriteTime.dwLowDateTime;
        if (now > t && now - t > kMaxAge) DeleteFileW(p.c_str());
        else v.push_back({ p, t });
    } while (FindNextFileW(h, &fd));
    FindClose(h);
    if (v.size() <= 5) return;
    std::sort(v.begin(), v.end(), [](const Entry& a, const Entry& b) { return a.t > b.t; });
    for (size_t i = 5; i < v.size(); i++) DeleteFileW(v[i].path.c_str());
}

void SaveCache(const uint8_t* px) {
    CreateDirectoryW(kCacheDir, nullptr);
    HANDLE f = CreateFileW(g_cacheFile.c_str(), GENERIC_WRITE, 0, nullptr,
                           CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (f == INVALID_HANDLE_VALUE) {
        FileLog(L"Failed to save cache (error %u)", GetLastError());
        return;
    }
    CacheHdr hd = { kMagic, (uint32_t)kW, (uint32_t)kH, 0 };
    DWORD w = 0;
    WriteFile(f, &hd, sizeof(hd), &w, nullptr);
    WriteFile(f, px, (DWORD)g_frameBytes, &w, nullptr);
    CloseHandle(f);
    FileLog(L"First frame cached: %s", g_cacheFile.c_str());
    PruneCache();
}

static wchar_t kLastFile[MAX_PATH] = L"";
static wchar_t kLastTmp[MAX_PATH] = L"";
static wchar_t kOverlayFile[MAX_PATH] = L"";
static wchar_t kOverlayTmp[MAX_PATH] = L"";
static const uint32_t kOvMagic = 0x4C4F5631;  // "LOV1"

void SaveLast() {
    std::vector<uint8_t> copy(g_frameBytes);
    EnterCriticalSection(&g_cs);
    memcpy(copy.data(), g_shared.data(), g_frameBytes);
    LeaveCriticalSection(&g_cs);
    CreateDirectoryW(kCacheDir, nullptr);
    HANDLE f = CreateFileW(kLastTmp, GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS,
                           FILE_ATTRIBUTE_NORMAL, nullptr);
    if (f == INVALID_HANDLE_VALUE) return;
    CacheHdr hd = { kMagic, (uint32_t)kW, (uint32_t)kH, 0 };
    DWORD w = 0;
    bool ok = WriteFile(f, &hd, sizeof(hd), &w, nullptr) &&
              WriteFile(f, copy.data(), (DWORD)g_frameBytes, &w, nullptr);
    CloseHandle(f);
    if (ok) MoveFileExW(kLastTmp, kLastFile, MOVEFILE_REPLACE_EXISTING);
}

bool LoadLast() {
    HANDLE f = CreateFileW(kLastFile, GENERIC_READ, FILE_SHARE_READ, nullptr,
                           OPEN_EXISTING, 0, nullptr);
    if (f == INVALID_HANDLE_VALUE) return false;
    CacheHdr hd = {};
    DWORD got = 0;
    bool ok = ReadFile(f, &hd, sizeof(hd), &got, nullptr) && got == sizeof(hd) &&
              hd.magic == kMagic && hd.w == (uint32_t)kW && hd.h == (uint32_t)kH;
    if (ok) ok = ReadFile(f, g_shared.data(), (DWORD)g_frameBytes, &got, nullptr) &&
                 got == g_frameBytes;
    CloseHandle(f);
    if (ok) g_seq = 1;
    return ok;
}

// ---------- Frame scaling (fit modes) ----------
// Scales one RGB32 frame into the kW x kH canvas according to g_fitMode.
// Pixels outside the video area stay pure black (letterbox bars).
static bool RenderFrame(const BYTE* data, size_t stride, bool flip, int sw, int sh,
                        uint8_t* out, std::vector<int>& xmap) {
    if (sw <= 0 || sh <= 0 || stride < (size_t)sw * 4) return false;
    float scX, scY;  // canvas pixels per source pixel
    switch (g_fitMode) {
    case kFitFill:
        scX = (float)kW / sw;
        scY = (float)kH / sh;
        break;
    case kFitFit:
        scX = scY = std::min((float)kW / sw, (float)kH / sh);
        break;
    case kFitCenter:  // 1:1 with the real screen pixels
        scX = scY = (float)kW / (float)g_screenW;
        break;
    default:          // cover
        scX = scY = std::max((float)kW / sw, (float)kH / sh);
        break;
    }
    const uint32_t kBlack = 0xFF000000u;
    for (int x = 0; x < kW; x++) {
        int sx = (int)std::floor(((float)x + 0.5f - kW * 0.5f) / scX + sw * 0.5f);
        xmap[x] = (sx >= 0 && sx < sw) ? sx : -1;
    }
    for (int y = 0; y < kH; y++) {
        uint32_t* dst = (uint32_t*)(out + (size_t)y * kW * 4);
        int sy = (int)std::floor(((float)y + 0.5f - kH * 0.5f) / scY + sh * 0.5f);
        if (sy < 0 || sy >= sh) {
            for (int x = 0; x < kW; x++) dst[x] = kBlack;
            continue;
        }
        int row = flip ? (sh - 1 - sy) : sy;
        const uint32_t* src = (const uint32_t*)(data + (size_t)row * stride);
        // scaling only: color is applied afterwards by ApplyColor
        for (int x = 0; x < kW; x++)
            dst[x] = xmap[x] < 0 ? kBlack : (src[xmap[x]] | kBlack);
    }
    return true;
}

// ---------- Audio (separate thread, WASAPI shared mode) ----------
// Plays the audio track of the video. If anything fails the mod simply stays
// silent. The video is paced by the wall clock, so the audio restarts together
// with every video loop (g_loopSerial) to keep both in sync.
#ifndef AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM
#define AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM 0x80000000
#endif
#ifndef AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY
#define AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY 0x08000000
#endif
static const CLSID kClsidMMDeviceEnumerator =
    { 0xBCDE0395, 0xE52F, 0x467C, { 0x8E, 0x3D, 0xC4, 0x57, 0x92, 0x91, 0x69, 0x2E } };

// Crash guard: the pendingStarts counter is cleared only after the first video frame
// AND (if audio is used) the first audio buffer were produced.
static std::atomic<bool> g_frameDone{false}, g_audioDone{true}, g_pendingCleared{false},
                         g_decPublished{false}, g_gpuActive{false};
static bool g_hwDecode = true;
static bool g_isGif = false;
static UINT32 g_natW = 0, g_natH = 0;   // native video size (used to detect padded frame heights)
static void TryClearPending() {
    if (!g_previewRole && g_frameDone && g_audioDone && !g_pendingCleared.exchange(true))
        Wh_SetIntValue(L"pendingStarts", 0);
}

template <class T> static void SafeRelease(T*& p) {
    if (p) { p->Release(); p = nullptr; }
}

DWORD WINAPI AudioThread(LPVOID) {
    bool coOk = SUCCEEDED(CoInitializeEx(nullptr, COINIT_MULTITHREADED));
    bool mfOk = SUCCEEDED(MFStartup(MF_VERSION));
    IMFSourceReader* rd = nullptr;
    IMMDeviceEnumerator* en = nullptr;
    IMMDevice* dev = nullptr;
    IAudioClient* ac = nullptr;
    IAudioRenderClient* rc = nullptr;
    do {
        if (!mfOk) break;
        HRESULT hr = S_OK;
        const DWORD AS = (DWORD)MF_SOURCE_READER_FIRST_AUDIO_STREAM;
        if (FAILED(MFCreateSourceReaderFromURL(g_videoFile.c_str(), nullptr, &rd)) || !rd) {
            FileLog(L"Audio: cannot open the file");
            break;
        }
        rd->SetStreamSelection((DWORD)MF_SOURCE_READER_ALL_STREAMS, FALSE);
        IMFMediaType* nat = nullptr;
        if (FAILED(rd->SetStreamSelection(AS, TRUE)) ||
            FAILED(rd->GetNativeMediaType(AS, 0, &nat)) || !nat) {
            FileLog(L"Audio: the video has no audio track");
            break;
        }
        UINT32 ch = MFGetAttributeUINT32(nat, MF_MT_AUDIO_NUM_CHANNELS, 2);
        UINT32 rate = MFGetAttributeUINT32(nat, MF_MT_AUDIO_SAMPLES_PER_SECOND, 48000);
        nat->Release();
        if (ch > 2) ch = 2;
        if (ch < 1) ch = 1;

        IMFMediaType* mt = nullptr;
        if (FAILED(MFCreateMediaType(&mt)) || !mt) break;
        mt->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Audio);
        mt->SetGUID(MF_MT_SUBTYPE, MFAudioFormat_Float);
        mt->SetUINT32(MF_MT_AUDIO_NUM_CHANNELS, ch);
        mt->SetUINT32(MF_MT_AUDIO_SAMPLES_PER_SECOND, rate);
        mt->SetUINT32(MF_MT_AUDIO_BITS_PER_SAMPLE, 32);
        mt->SetUINT32(MF_MT_AUDIO_BLOCK_ALIGNMENT, ch * 4);
        mt->SetUINT32(MF_MT_AUDIO_AVG_BYTES_PER_SECOND, rate * ch * 4);
        hr = rd->SetCurrentMediaType(AS, nullptr, mt);
        mt->Release();
        if (FAILED(hr)) {
            FileLog(L"Audio: PCM float output not supported (hr=0x%08X)", (unsigned)hr);
            break;
        }

        if (FAILED(CoCreateInstance(kClsidMMDeviceEnumerator, nullptr, CLSCTX_ALL,
                                    __uuidof(IMMDeviceEnumerator), (void**)&en)) || !en) break;
        if (FAILED(en->GetDefaultAudioEndpoint(eRender, eConsole, &dev)) || !dev) {
            FileLog(L"Audio: no output device");
            break;
        }
        if (FAILED(dev->Activate(__uuidof(IAudioClient), CLSCTX_ALL, nullptr,
                                 (void**)&ac)) || !ac) break;
        WAVEFORMATEX wf = {};
        wf.wFormatTag = WAVE_FORMAT_IEEE_FLOAT;
        wf.nChannels = (WORD)ch;
        wf.nSamplesPerSec = rate;
        wf.wBitsPerSample = 32;
        wf.nBlockAlign = (WORD)(ch * 4);
        wf.nAvgBytesPerSec = rate * wf.nBlockAlign;
        hr = ac->Initialize(AUDCLNT_SHAREMODE_SHARED,
                            AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                                AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                            2000000, 0, &wf, nullptr);
        if (FAILED(hr)) {
            FileLog(L"Audio: IAudioClient::Initialize failed (hr=0x%08X)", (unsigned)hr);
            break;
        }
        UINT32 bufFrames = 0;
        ac->GetBufferSize(&bufFrames);
        if (FAILED(ac->GetService(__uuidof(IAudioRenderClient), (void**)&rc)) || !rc) break;
        FileLog(L"Audio: started (%u ch, %u Hz)", ch, rate);

        std::vector<uint8_t> pend;
        size_t pos = 0;
        bool ended = false, running = false;
        LONG seen = g_loopSerial.load();
        const UINT32 frameBytes = ch * 4;
        float gain = 0.0f;                                   // fade gain, 0..1
        const float fadeStep = 1.0f / (float)(rate * 0.6);   // 0.6 second fade        
        while (!g_stop && !g_decoderDone) {
            // Playback hold (idle or hidden): stop the stream and wait.
            if (g_idlePaused.load() && g_audioDone && (!running || gain <= 0.001f)) {
                gain = 0.0f;
                if (running) { ac->Stop(); running = false; }
                Sleep(100);
                continue;
            }
            // The video looped: restart the audio from the beginning, in sync.
            if (seen != g_loopSerial.load()) {
                seen = g_loopSerial.load();
                PROPVARIANT pv;
                PropVariantInit(&pv);
                pv.vt = VT_I8;
                pv.hVal.QuadPart = 0;
                rd->SetCurrentPosition(kGuidNull, pv);
                pend.clear();
                pos = 0;
                ended = false;
                ac->Stop();
                ac->Reset();
                gain = 0.0f;                
                running = false;
            }
            // Refill the pending PCM bytes.
            if (pos >= pend.size() && !ended) {
                pend.clear();
                pos = 0;
                DWORD flags = 0;
                IMFSample* s = nullptr;
                hr = rd->ReadSample(AS, 0, nullptr, &flags, nullptr, &s);
                if (FAILED(hr) || (flags & MF_SOURCE_READERF_ERROR)) {
                    if (s) s->Release();
                    break;
                }
                if (flags & MF_SOURCE_READERF_ENDOFSTREAM) ended = true;
                if (s) {
                    IMFMediaBuffer* b = nullptr;
                    if (SUCCEEDED(s->ConvertToContiguousBuffer(&b)) && b) {
                        BYTE* d = nullptr;
                        DWORD cur = 0;
                        if (SUCCEEDED(b->Lock(&d, nullptr, &cur)) && d) {
                            pend.assign(d, d + cur);
                            if (!g_audioDone) { g_audioDone = true; TryClearPending(); }
                            b->Unlock();
                        }
                        b->Release();
                    }
                    s->Release();
                }
            }
            if (g_idlePaused.load() && gain <= 0.001f) { Sleep(10); continue; }  // decoded, but wait until the fade-out is done
            // Write what fits into the WASAPI buffer, scaled by our own volume.
            UINT32 padding = 0;
            if (FAILED(ac->GetCurrentPadding(&padding))) break;
            UINT32 freeFrames = bufFrames > padding ? bufFrames - padding : 0;
            UINT32 haveFrames = (UINT32)((pend.size() - pos) / frameBytes);
            if (haveFrames == 0) pos = pend.size();  // drop a trailing partial frame
            UINT32 n = std::min(freeFrames, haveFrames);
            if (n > 0) {
                BYTE* outBuf = nullptr;
                if (FAILED(rc->GetBuffer(n, &outBuf)) || !outBuf) break;
                float vol = (float)g_audioVol / 100.0f;
                vol *= vol;  // gentle curve: finer control at low volume
                const float* in = (const float*)(pend.data() + pos);
                float* o = (float*)outBuf;
                const float tgt = g_idlePaused.load() ? 0.0f : 1.0f;
                for (UINT32 i = 0; i < n; i++) {
                    if (gain < tgt) gain = std::min(tgt, gain + fadeStep);
                    else if (gain > tgt) gain = std::max(tgt, gain - fadeStep);
                    for (UINT32 c = 0; c < ch; c++)
                        o[i * ch + c] = in[i * ch + c] * vol * gain;
                }
                rc->ReleaseBuffer(n, 0);
                if (!g_audioDone) { g_audioDone = true; TryClearPending(); }
                pos += (size_t)n * frameBytes;
                if (!running) { ac->Start(); running = true; }
            }
            Sleep(10);
        }
    } while (false);
    if (ac) ac->Stop();
    SafeRelease(rc);
    SafeRelease(ac);
    SafeRelease(dev);
    SafeRelease(en);
    SafeRelease(rd);
    if (mfOk) MFShutdown();
    if (coOk) CoUninitialize();
    g_audioDone = true;
    TryClearPending();
    FileLog(L"Audio: stopped");
    return 0;
}
// ---------- Hardware decoding (D3D11 device shared with Media Foundation) ----------
// Used only by the decoder thread. The destructor releases everything.
struct GpuState {
    ID3D11Device* dev = nullptr;
    ID3D11DeviceContext* ctx = nullptr;
    IMFDXGIDeviceManager* mgr = nullptr;
    ID3D11Texture2D* staging = nullptr;
    UINT stW = 0, stH = 0;
    DXGI_FORMAT stFmt = DXGI_FORMAT_UNKNOWN;

    bool Init() {
        D3D_FEATURE_LEVEL fl;
        UINT flags = D3D11_CREATE_DEVICE_VIDEO_SUPPORT | D3D11_CREATE_DEVICE_BGRA_SUPPORT;
        HRESULT hr = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, flags,
                                       nullptr, 0, D3D11_SDK_VERSION, &dev, &fl, &ctx);
        if (FAILED(hr) || !dev || !ctx) {
            FileLog(L"GPU: D3D11CreateDevice failed (hr=0x%08X)", (unsigned)hr);
            return false;
        }
        ID3D11Multithread* mtp = nullptr;
        if (SUCCEEDED(dev->QueryInterface(__uuidof(ID3D11Multithread), (void**)&mtp)) && mtp) {
            mtp->SetMultithreadProtected(TRUE);
            mtp->Release();
        }
        UINT token = 0;
        hr = MFCreateDXGIDeviceManager(&token, &mgr);
        if (FAILED(hr) || !mgr) {
            FileLog(L"GPU: MFCreateDXGIDeviceManager failed (hr=0x%08X)", (unsigned)hr);
            return false;
        }
        hr = mgr->ResetDevice(dev, token);
        if (FAILED(hr)) {
            FileLog(L"GPU: ResetDevice failed (hr=0x%08X)", (unsigned)hr);
            return false;
        }
        return true;
    }

    // Copies the GPU frame of a sample into a CPU-readable staging texture and
    // maps it. On success call Unmap() when done with m.pData.
    bool Map(IMFSample* s, D3D11_MAPPED_SUBRESOURCE& m, UINT& w, UINT& h) {
        IMFMediaBuffer* b = nullptr;
        if (FAILED(s->GetBufferByIndex(0, &b)) || !b) return false;
        bool ok = false;
        IMFDXGIBuffer* db = nullptr;
        if (SUCCEEDED(b->QueryInterface(__uuidof(IMFDXGIBuffer), (void**)&db)) && db) {
            ID3D11Texture2D* tex = nullptr;
            if (SUCCEEDED(db->GetResource(__uuidof(ID3D11Texture2D), (void**)&tex)) && tex) {
                UINT sub = 0;
                db->GetSubresourceIndex(&sub);
                D3D11_TEXTURE2D_DESC d;
                tex->GetDesc(&d);
                if (!staging || stW != d.Width || stH != d.Height || stFmt != d.Format) {
                    SafeRelease(staging);
                    D3D11_TEXTURE2D_DESC sd = d;
                    sd.Usage = D3D11_USAGE_STAGING;
                    sd.BindFlags = 0;
                    sd.MiscFlags = 0;
                    sd.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
                    sd.ArraySize = 1;
                    sd.MipLevels = 1;
                    if (SUCCEEDED(dev->CreateTexture2D(&sd, nullptr, &staging))) {
                        stW = d.Width; stH = d.Height; stFmt = d.Format;
                    }
                }
                if (staging) {
                    ctx->CopySubresourceRegion(staging, 0, 0, 0, 0, tex, sub, nullptr);
                    if (SUCCEEDED(ctx->Map(staging, 0, D3D11_MAP_READ, 0, &m))) {
                        w = d.Width;
                        h = d.Height;
                        ok = true;
                    }
                }
                tex->Release();
            }
            db->Release();
        }
        b->Release();
        return ok;
    }

    void Unmap() { if (ctx && staging) ctx->Unmap(staging, 0); }

    ~GpuState() {
        SafeRelease(staging);
        SafeRelease(mgr);
        SafeRelease(ctx);
        SafeRelease(dev);
    }
};
// ---------- Decoder (separate thread) ----------
static DWORD DecodeRun(bool gpu) {
    // Whatever the reason the decoder exits (file won't open, unsupported codec, etc.),
    // if no frame was ever produced, tell the UI thread so the original wallpaper
    // is not replaced by a black bitmap and the fail-safe can trip.
    struct ExitGuard {
        ~ExitGuard() {
            if (g_gpuActive && !g_decPublished && !g_stop) return;  // software retry follows
            if (g_seq == 0) g_decodeDead = true;
            g_decoderDone = true;
            if (!g_previewRole) Wh_SetIntValue(L"pendingStarts", 0);
        }
    } exitGuard;

    HRESULT hrCo = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    struct CoGuard {
        bool on;
        ~CoGuard() { if (on) CoUninitialize(); }
    } coGuard{ SUCCEEDED(hrCo) };
    timeBeginPeriod(1);
    const double tInit0 = NowMs();    
    if (FAILED(MFStartup(MF_VERSION))) {
        FileLog(L"MFStartup failed");
        timeEndPeriod(1);
        return 1;
    }
    IMFAttributes* at = nullptr;
    MFCreateAttributes(&at, 1);
    if (at) at->SetUINT32(MF_SOURCE_READER_ENABLE_ADVANCED_VIDEO_PROCESSING, TRUE);
    GpuState gs;
    g_gpuActive = false;
    if (gpu) {
        if (at && gs.Init()) {
            at->SetUnknown(MF_SOURCE_READER_D3D_MANAGER, gs.mgr);
            at->SetUINT32(MF_READWRITE_ENABLE_HARDWARE_TRANSFORMS, TRUE);
            g_gpuActive = true;
        } else {
            FileLog(L"Hardware decoding unavailable, using software decoding");
            gpu = false;
        }
    }

    IMFSourceReader* rd = nullptr;
    FileLog(L"Startup: MF + GPU init took %.0f ms", NowMs() - tInit0);
    HRESULT hr = MFCreateSourceReaderFromURL(g_videoFile.c_str(), at, &rd);
    if (at) at->Release();
    FileLog(L"Startup: source reader opened at +%.0f ms", NowMs() - tInit0);    
    if (FAILED(hr) || !rd) {
        FileLog(L"Failed to open video (hr=0x%08X): %s", (unsigned)hr,
               g_videoFile.c_str());
        MFShutdown();
        timeEndPeriod(1);
        return 1;
    }
    const DWORD VS = (DWORD)MF_SOURCE_READER_FIRST_VIDEO_STREAM;
    rd->SetStreamSelection((DWORD)MF_SOURCE_READER_ALL_STREAMS, FALSE);
    rd->SetStreamSelection(VS, TRUE);

    IMFMediaType* mt = nullptr;
    if (FAILED(MFCreateMediaType(&mt)) || !mt) {
        FileLog(L"MFCreateMediaType failed");
        rd->Release();
        MFShutdown();
        timeEndPeriod(1);
        return 1;
    }
    mt->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Video);
    mt->SetGUID(MF_MT_SUBTYPE, gpu ? MFVideoFormat_ARGB32 : MFVideoFormat_RGB32);
    {
        // Let Media Foundation scale big videos down: converting and copying
        // a 4K frame as RGB32 on the CPU costs far more than at canvas size.
        IMFMediaType* nat = nullptr;
        UINT32 nw = 0, nh = 0;
        if (SUCCEEDED(rd->GetNativeMediaType(VS, 0, &nat)) && nat) {
            MFGetAttributeSize(nat, MF_MT_FRAME_SIZE, &nw, &nh);
            g_natW = nw;
            g_natH = nh;            
            nat->Release();
        }
        if (nw > 0 && nh > 0) {
            float sc;
            switch (g_fitMode) {
            case kFitFit:    sc = std::min((float)kW / nw, (float)kH / nh); break;
            case kFitCenter: sc = 1.0f; break;  // RenderFrame does the 1:1 scaling itself
            default:         sc = std::max((float)kW / nw, (float)kH / nh); break;
            }
            if (sc < 0.98f) {
                UINT32 tw = ((UINT32)(nw * sc + 0.5f) + 1) & ~1u;
                UINT32 th = ((UINT32)(nh * sc + 0.5f) + 1) & ~1u;
                if (tw >= 2 && th >= 2) {
                    MFSetAttributeSize(mt, MF_MT_FRAME_SIZE, tw, th);
                    FileLog(L"Native %ux%u -> MF output %ux%u", nw, nh, tw, th);
                }
            }
        }
    }
    hr = rd->SetCurrentMediaType(VS, nullptr, mt);
    if (FAILED(hr)) {
        FileLog(L"Scaled output type rejected (hr=0x%08X), using native size", (unsigned)hr);
        mt->DeleteItem(MF_MT_FRAME_SIZE);
        hr = rd->SetCurrentMediaType(VS, nullptr, mt);
    }
    mt->Release();
    if (FAILED(hr)) {
        FileLog(L"Unsupported video format (hr=0x%08X)", (unsigned)hr);
        rd->Release();
        MFShutdown();
        timeEndPeriod(1);
        return 1;
    }

    UINT32 sw = 0, sh = 0;
    UINT32 visH = 0;  // real video height (frames can be reported as 1088 because of padding)
    LONG stride = 0;
    auto readType = [&]() {
        IMFMediaType* cur = nullptr;
        if (SUCCEEDED(rd->GetCurrentMediaType(VS, &cur)) && cur) {
            MFGetAttributeSize(cur, MF_MT_FRAME_SIZE, &sw, &sh);
            stride = (LONG)MFGetAttributeUINT32(cur, MF_MT_DEFAULT_STRIDE,
                                                (UINT32)(sw * 4));
            visH = sh;
            MFVideoArea ap = {};
            if (SUCCEEDED(cur->GetBlob(MF_MT_MINIMUM_DISPLAY_APERTURE, (UINT8*)&ap,
                                       sizeof(ap), nullptr)) &&
                ap.Area.cy > 0 && (UINT32)ap.Area.cy <= sh && ap.OffsetY.value == 0)
                visH = (UINT32)ap.Area.cy;
            // Hardware decoders often report a padded height (1088 for 1080) without an aperture.
            if (g_natW == sw && g_natH > 0 && g_natH < visH) visH = g_natH;                
            cur->Release();
        }
    };
    readType();
    FileLog(L"Video: %ux%u stride=%ld", sw, sh, stride);
    FileLog(L"Startup: media type set at +%.0f ms", NowMs() - tInit0);    
    FileLog(L"Visible height=%u (frame height=%u)", visH, sh);
    if (!sw || !sh) { rd->Release(); MFShutdown(); timeEndPeriod(1); return 1; }

    std::vector<uint8_t> tmp(g_frameBytes);
    std::vector<int> xmap(kW);

    ULONGLONG startTick = GetTickCount64();
    ULONGLONG t0 = startTick;
    LONG seqAtRewind = -1;
    bool firstPublished = false, savedOnce = false;
    LONGLONG lastKeptTs = -1;  // timestamp of the last kept frame (low-battery fps cap)    
    int gpuMiss = 0;
    double rewindStartMs = 0; 
    std::vector<uint8_t> rawFirst;  // uncolored first frame (for the cache)       
    while (!g_stop) {
        // Playback hold (idle or hidden): park without decoding. On resume the
        // pacing code below re-syncs t0 by itself because the first frame is late.
        if (g_idlePaused.load() && firstPublished) { Sleep(100); continue; }
        double tA = NowMs();
        DWORD actual = 0, flags = 0;
        LONGLONG ts = 0;
        IMFSample* sample = nullptr;
        hr = rd->ReadSample(VS, 0, &actual, &flags, &ts, &sample);
        double tRead = NowMs() - tA;
        if (FAILED(hr) || (flags & MF_SOURCE_READERF_ERROR)) {
            FileLog(L"ReadSample error (hr=0x%08X)", (unsigned)hr);
            if (sample) sample->Release();
            break;
        }
        if (flags & MF_SOURCE_READERF_ENDOFSTREAM) {
            rewindStartMs = NowMs();            
            PROPVARIANT pv;
            PropVariantInit(&pv);
            pv.vt = VT_I8;
            pv.hVal.QuadPart = 0;
            rd->SetCurrentPosition(kGuidNull, pv);
            t0 = GetTickCount64();
            if (g_seq == seqAtRewind) Sleep(500);  // no frame produced: avoid a busy loop
            seqAtRewind = g_seq;
            g_loopSerial++;         
            if (sample) sample->Release();
            continue;
        }
        if (flags & MF_SOURCE_READERF_CURRENTMEDIATYPECHANGED) readType();
        if (!sample) continue;
        // Low battery, Light / Balanced: drop frames that come too soon after the last kept one.
        if (g_lowBat.load() && (g_lowBatLevel == 1 || g_lowBatLevel == 2) && firstPublished) {
            const LONGLONG minGap = (g_lowBatLevel == 1) ? 300000LL : 600000LL;  // 30 ms / 60 ms (100 ns units)
            if (lastKeptTs >= 0 && ts >= lastKeptTs && ts - lastKeptTs < minGap) {
                sample->Release();
                continue;
            }
        }
        lastKeptTs = ts;
        // 1) Convert into a temporary buffer first
        bool have = false;
        if (gpu) {
            D3D11_MAPPED_SUBRESOURCE mp = {};
            UINT tw = 0, th = 0;
            if (gs.Map(sample, mp, tw, th)) {
                if (mp.pData && mp.RowPitch >= sw * 4 && th > 0) {
                    have = RenderFrame((const BYTE*)mp.pData, (size_t)mp.RowPitch, false,
                                       (int)sw, (int)std::min<UINT>(visH ? visH : sh, th),
                                       tmp.data(), xmap);
                }
                gs.Unmap();
            }
        }
        IMFMediaBuffer* buf = nullptr;
        if (!gpu && SUCCEEDED(sample->ConvertToContiguousBuffer(&buf)) && buf) {
            BYTE* data = nullptr;
            DWORD maxLen = 0, curLen = 0;
            if (SUCCEEDED(buf->Lock(&data, &maxLen, &curLen)) && data) {
                bool flip = stride < 0;
                size_t absStride = stride < 0 ? (size_t)(-(long long)stride) : (size_t)stride;
                if (absStride >= (size_t)sw * 4 && absStride * sh <= curLen) {
                    have = RenderFrame(data, (size_t)absStride, flip, (int)sw, (int)(visH && visH < sh ? visH : sh),
                                       tmp.data(), xmap);
                }
                buf->Unlock();
            }
            buf->Release();
        }
        sample->Release();
        if (!have) {
            if (gpu && !firstPublished && ++gpuMiss >= 10) {
                FileLog(L"GPU: no readable frame after 10 samples, giving up");
                break;
            }
            continue;
        }

        double dec = NowMs() - tA;
        g_decMsSum += dec;
        g_readMsSum += tRead;
        g_decN++;
        if (dec > g_decMsMax) g_decMsMax = dec;

        // 1b) Keep an uncolored copy of the very first frame for the cache, then apply the colors
        if (g_needSave && rawFirst.empty()) rawFirst = tmp;
        ApplyColor((uint32_t*)tmp.data(), (size_t)kW * kH);

        // 2) Wait until the presentation time (not needed while paused)
        if (!g_paused) {
            ULONGLONG mediaMs = (ULONGLONG)(ts / 10000);
            ULONGLONG due = t0 + mediaMs * 100ULL / (ULONGLONG)g_speed;
            for (;;) {
                ULONGLONG now = GetTickCount64();
                if (g_stop || now >= due) {
                    if (now > due + 250) t0 += (now - due);
                    break;
                }
                Sleep((DWORD)std::min<ULONGLONG>(due - now, 4));
            }
            if (g_stop) break;
        }

        // 3) Publish the frame, then wake the UI thread right away
        EnterCriticalSection(&g_cs);
        memcpy(g_shared.data(), tmp.data(), g_frameBytes);
        InterlockedIncrement(&g_seq);
        LeaveCriticalSection(&g_cs);
        HWND w = g_wnd;
        if (w) PostMessageW(w, WM_NULL, 0, 0);
        if (rewindStartMs > 0) {
            FileLog(L"Loop rewind: %.0f ms until the next frame", NowMs() - rewindStartMs);
            rewindStartMs = 0;
        }        
        if (!firstPublished) {
            firstPublished = true;
            g_decPublished = true;
            g_frameDone = true;
            TryClearPending();
            FileLog(gpu ? L"Decoder: hardware (D3D11) path active" : L"Decoder: software path active");
            FileLog(L"Startup: first frame published at +%.0f ms", NowMs() - tInit0);            
        }

        if (g_needSave) {
            g_needSave = false;
            SaveCache(rawFirst.empty() ? tmp.data() : rawFirst.data());
        }
        if (g_saveLast && !savedOnce && GetTickCount64() - startTick > 10000) { savedOnce = true; SaveLast(); }
        if (g_paused) {
            FileLog(L"Pause: still frame shown");
            break;
        }
    }
    rd->Release();
    MFShutdown();
    timeEndPeriod(1);
    if (g_saveLast && g_seq > 0) SaveLast();
    FileLog(L"Decoder stopped");
    return 0;
}

// ---------- Animated GIF decoder (WIC) ----------
// Same contract as DecodeRun: publishes finished frames into g_shared.
static const GUID kGuidWicPBGRA =
    { 0x6fddc324, 0x4e03, 0x4bfe, { 0xb1, 0x85, 0x3d, 0x77, 0x76, 0x8d, 0xc9, 0x10 } };
static const CLSID kClsidWicFactory =
    { 0xcacaf262, 0x9370, 0x4615, { 0xa1, 0x3b, 0x9f, 0x55, 0x39, 0xda, 0x4c, 0x0a } };

static UINT WicU16(IWICMetadataQueryReader* q, const wchar_t* name, UINT def) {
    UINT v = def;
    if (!q) return v;
    PROPVARIANT pv;
    PropVariantInit(&pv);
    if (SUCCEEDED(q->GetMetadataByName(name, &pv))) {
        if (pv.vt == VT_UI2) v = pv.uiVal;
        else if (pv.vt == VT_UI1) v = pv.bVal;
    }
    PropVariantClear(&pv);
    return v;
}

static DWORD GifRun() {
    struct ExitGuard {
        ~ExitGuard() {
            if (g_seq == 0) g_decodeDead = true;
            g_decoderDone = true;
            if (!g_previewRole) Wh_SetIntValue(L"pendingStarts", 0);
        }
    } exitGuard;
    HRESULT hrCo = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    struct CoGuard {
        bool on;
        ~CoGuard() { if (on) CoUninitialize(); }
    } coGuard{ SUCCEEDED(hrCo) };
    timeBeginPeriod(1);
    struct TimerGuard {
        ~TimerGuard() { timeEndPeriod(1); }
    } timerGuard;

    IWICImagingFactory* fac = nullptr;
    IWICBitmapDecoder* dec = nullptr;
    HRESULT hr = CoCreateInstance(kClsidWicFactory, nullptr, CLSCTX_INPROC_SERVER,
                                  __uuidof(IWICImagingFactory), (void**)&fac);
    if (SUCCEEDED(hr) && fac)
        hr = fac->CreateDecoderFromFilename(g_videoFile.c_str(), nullptr, GENERIC_READ,
                                            WICDecodeMetadataCacheOnDemand, &dec);
    UINT count = 0;
    if (SUCCEEDED(hr) && dec) dec->GetFrameCount(&count);
    if (FAILED(hr) || !dec || count == 0) {
        FileLog(L"GIF: cannot open or no frames (hr=0x%08X): %s", (unsigned)hr,
               g_videoFile.c_str());
        SafeRelease(dec);
        SafeRelease(fac);
        return 1;
    }

    // Logical screen size (the canvas every frame is composited onto)
    UINT lw = 0, lh = 0;
    {
        IWICMetadataQueryReader* q = nullptr;
        if (SUCCEEDED(dec->GetMetadataQueryReader(&q)) && q) {
            lw = WicU16(q, L"/logscrdesc/Width", 0);
            lh = WicU16(q, L"/logscrdesc/Height", 0);
        }
        SafeRelease(q);
    }
    if (!lw || !lh) {
        IWICBitmapFrameDecode* f0 = nullptr;
        if (SUCCEEDED(dec->GetFrame(0, &f0)) && f0) f0->GetSize(&lw, &lh);
        SafeRelease(f0);
    }
    if (!lw || !lh || lw > 8192 || lh > 8192 ||
        (unsigned long long)lw * lh > 3840ULL * 2160ULL) {
        FileLog(L"GIF: invalid canvas size %ux%u", lw, lh);
        SafeRelease(dec);
        SafeRelease(fac);
        return 1;
    }
    FileLog(L"GIF: %u frames, %ux%u", count, lw, lh);

    std::vector<uint32_t> canvas((size_t)lw * lh, 0), prev, frame;
    std::vector<uint8_t> tmp(g_frameBytes), rawFirst;
    std::vector<int> xmap(kW);
    ULONGLONG startTick = GetTickCount64();
    ULONGLONG nextDue = startTick;
    bool firstPublished = false, savedOnce = false;
    UINT idx = 0;
    while (!g_stop) {
        // Playback hold (idle / hidden): park without decoding.
        if (g_idlePaused.load() && firstPublished) {
            Sleep(100);
            nextDue = GetTickCount64();
            continue;
        }
        IWICBitmapFrameDecode* fr = nullptr;
        if (FAILED(dec->GetFrame(idx, &fr)) || !fr) break;
        IWICMetadataQueryReader* q = nullptr;
        fr->GetMetadataQueryReader(&q);
        UINT left = WicU16(q, L"/imgdesc/Left", 0);
        UINT top = WicU16(q, L"/imgdesc/Top", 0);
        UINT delay = WicU16(q, L"/grctlext/Delay", 10);   // 1/100 s
        UINT disp = WicU16(q, L"/grctlext/Disposal", 0);
        SafeRelease(q);

        UINT fw = 0, fh = 0;
        bool ok = false;
        IWICFormatConverter* cv = nullptr;
        if (SUCCEEDED(fac->CreateFormatConverter(&cv)) && cv &&
            SUCCEEDED(cv->Initialize(fr, kGuidWicPBGRA, WICBitmapDitherTypeNone, nullptr,
                                     0.0, WICBitmapPaletteTypeCustom)) &&
            SUCCEEDED(cv->GetSize(&fw, &fh)) && fw && fh && fw <= 8192 && fh <= 8192) {
            frame.resize((size_t)fw * fh);
            ok = SUCCEEDED(cv->CopyPixels(nullptr, fw * 4, (UINT)(frame.size() * 4),
                                          (BYTE*)frame.data()));
        }
        SafeRelease(cv);
        SafeRelease(fr);
        if (!ok) {
            FileLog(L"GIF: failed to decode frame %u", idx);
            break;
        }

        // Compose this frame onto the canvas (transparent pixels keep what is below).
        if (disp == 3) prev = canvas;
        for (UINT y = 0; y < fh && top + y < lh; y++) {
            for (UINT x = 0; x < fw && left + x < lw; x++) {
                uint32_t p = frame[(size_t)y * fw + x];
                if (p >> 24) canvas[(size_t)(top + y) * lw + (left + x)] = p;
            }
        }

        if (!RenderFrame((const BYTE*)canvas.data(), (size_t)lw * 4, false, (int)lw,
                         (int)lh, tmp.data(), xmap))
            break;
        if (g_needSave && rawFirst.empty()) rawFirst = tmp;
        ApplyColor((uint32_t*)tmp.data(), (size_t)kW * kH);

        // Wait until this frame is due.
        for (;;) {
            ULONGLONG now = GetTickCount64();
            if (g_stop || now >= nextDue) {
                if (now > nextDue + 250) nextDue = now;
                break;
            }
            Sleep((DWORD)std::min<ULONGLONG>(nextDue - now, 4));
        }
        if (g_stop) break;

        EnterCriticalSection(&g_cs);
        memcpy(g_shared.data(), tmp.data(), g_frameBytes);
        InterlockedIncrement(&g_seq);
        LeaveCriticalSection(&g_cs);
        HWND w = g_wnd;
        if (w) PostMessageW(w, WM_NULL, 0, 0);
        if (!firstPublished) {
            firstPublished = true;
            g_decPublished = true;
            g_frameDone = true;
            TryClearPending();
            FileLog(L"Decoder: GIF path active");
        }
        if (g_needSave) {
            g_needSave = false;
            SaveCache(rawFirst.empty() ? tmp.data() : rawFirst.data());
        }
        if (g_saveLast && !savedOnce && GetTickCount64() - startTick > 10000) {
            savedOnce = true;
            SaveLast();
        }
        if (g_paused || count <= 1) {
            FileLog(L"GIF: still frame shown");
            break;
        }

        UINT d = delay <= 1 ? 10 : delay;   // browsers treat 0 / 1 as 100 ms
        nextDue += (ULONGLONG)d * 10ULL * 100ULL / (ULONGLONG)g_speed;

        // Disposal for the next frame.
        if (disp == 2) {
            for (UINT y = 0; y < fh && top + y < lh; y++)
                for (UINT x = 0; x < fw && left + x < lw; x++)
                    canvas[(size_t)(top + y) * lw + (left + x)] = 0;
        } else if (disp == 3 && !prev.empty()) {
            canvas = prev;
        }
        if (++idx >= count) {
            idx = 0;
            std::fill(canvas.begin(), canvas.end(), 0u);
            g_loopSerial++;
        }
    }
    SafeRelease(dec);
    SafeRelease(fac);
    if (g_saveLast && g_seq > 0) SaveLast();
    FileLog(L"GIF decoder stopped");
    return 0;
}

DWORD WINAPI DecodeThread(LPVOID) {
    try {
        if (g_isGif) return GifRun();        
        DWORD r = DecodeRun(g_hwDecode);
        if (g_hwDecode && g_gpuActive && !g_decPublished && !g_stop) {
            FileLog(L"Hardware decoding produced no frame, retrying with software decoding");
            g_gpuActive = false;
            r = DecodeRun(false);
        }
        return r;
    }
    catch (...) { FileLog(L"Decoder exception"); g_decodeDead = true; return 1; }
}

// ---------- XAML side ----------
DependencyObject FindByName(DependencyObject const& o,
                            winrt::hstring const& target, int depth) {
    if (depth > 60) return nullptr;
    if (auto fe = o.try_as<FrameworkElement>()) {
        if (fe.Name() == target) return o;
    }
    int n = Media::VisualTreeHelper::GetChildrenCount(o);
    for (int i = 0; i < n; i++) {
        auto r = FindByName(Media::VisualTreeHelper::GetChild(o, i),
                            target, depth + 1);
        if (r) return r;
    }
    return nullptr;
}

struct SceneNodes {
    DependencyObject msg{ nullptr }, lock{ nullptr }, cred{ nullptr },
                     imgA{ nullptr }, imgB{ nullptr };
};

void CollectNodes(DependencyObject const& o, int depth, SceneNodes& s) {
    if (depth > 60) return;
    if (auto fe = o.try_as<FrameworkElement>()) {
        auto n = fe.Name();
        if (!n.empty()) {
            if (!s.msg && n == L"DisplayMessageUserControl") s.msg = o;
            else if (!s.lock && n == L"LockScreenUserControl") s.lock = o;
            else if (!s.cred && n == L"RequestCredentialUserControl") s.cred = o;
            else if (!s.imgA && n == L"LockLogonImage") s.imgA = o;
            else if (!s.imgB && n == L"SwapLockLogonImage") s.imgB = o;
        }
    }
    int cnt = Media::VisualTreeHelper::GetChildrenCount(o);
    for (int i = 0; i < cnt; i++)
        CollectNodes(Media::VisualTreeHelper::GetChild(o, i), depth + 1, s);
}

void EnsureBitmap() {
    if (g_wb) return;
    Media::Imaging::WriteableBitmap wb(kW, kH);
    winrt::com_ptr<IBufferByteAccess> bba;
    winrt::check_hresult(winrt::get_unknown(wb.PixelBuffer())->QueryInterface(
        IID_IBufferByteAccess_, bba.put_void()));
    uint8_t* p = nullptr;
    winrt::check_hresult(bba->Buffer(&p));
    uint32_t* px = (uint32_t*)p;
    for (int i = 0; i < kW * kH; i++) px[i] = 0xFF000000u;
    g_pixels = p;
    g_wb = wb;
    g_wb.Invalidate();
    FileLog(L"WriteableBitmap %dx%d created", kW, kH);
}

void LogStats() {
    FILETIME c, e, k, u;
    GetProcessTimes(GetCurrentProcess(), &c, &e, &k, &u);
    ULONGLONG cpu = (((ULONGLONG)k.dwHighDateTime << 32) | k.dwLowDateTime) +
                    (((ULONGLONG)u.dwHighDateTime << 32) | u.dwLowDateTime);
    ULONGLONG wall = GetTickCount64() * 10000ULL;
    if (g_wallPrev) {
        int n = g_decN;
        double sum = g_decMsSum, mx = g_decMsMax;
        double rsum = g_readMsSum;
        g_decN = 0; g_decMsSum = 0; g_decMsMax = 0; g_readMsSum = 0;
        int st = g_stalls;
        double sm = g_stallMax;
        g_stalls = 0; g_stallMax = 0;
        FileLog(L"ReadSample avg=%.1f ms (decode + convert + MF scaling)", n ? rsum / n : 0.0);
        FileLog(L"CPU %.1f%% | frame=%ld | decode avg=%.1f ms max=%.1f ms | UI stalls>50ms: %d (worst %.0f ms)",
               100.0 * (double)(cpu - g_cpuPrev) / (double)(wall - g_wallPrev),
               g_seq, n ? sum / n : 0.0, mx, st, sm);
    }
    g_cpuPrev = cpu;
    g_wallPrev = wall;
}

[[clang::no_destroy]] static Media::ImageSource g_origSrcA{nullptr};
[[clang::no_destroy]] static Media::ImageSource g_origSrcB{nullptr};
static Media::Stretch g_origStrA = Media::Stretch::Uniform;
static Media::Stretch g_origStrB = Media::Stretch::Uniform;
static bool g_savedA = false, g_savedB = false;

[[clang::no_destroy]] static Controls::Grid g_rootGrid{nullptr};
[[clang::no_destroy]] static Media::Brush g_origBrush{nullptr};
static bool g_brushOn = false;

void UpdateBrush(DependencyObject const& content, DependencyObject const& o) {
    auto fe = o ? o.try_as<FrameworkElement>() : nullptr;
    bool msg = fe && fe.Visibility() == Visibility::Visible && fe.ActualWidth() > 0;
    g_msgSceneUp = msg;
    {
        static bool s_prevMsg = false;
        if (msg != s_prevMsg) {
            s_prevMsg = msg;
            FileLog(L"Message scene %s (SM_SHUTTINGDOWN=%d, hideOnShutdown=%d)",
                   msg ? L"shown" : L"gone", GetSystemMetrics(SM_SHUTTINGDOWN),
                   g_hideOnShutdown ? 1 : 0);
        }
    }
    if (msg && g_hideOnShutdown) {
        // Shutdown / restart style status screen: keep the original background.
        if (g_brushOn && g_rootGrid) {
            g_rootGrid.Background(g_origBrush);
            g_brushOn = false;
        }
        return;
    }
    if (msg && !g_brushOn) {
        if (Media::VisualTreeHelper::GetChildrenCount(content) < 1) return;
        auto g = Media::VisualTreeHelper::GetChild(content, 0).try_as<Controls::Grid>();
        if (!g) return;
        g_origBrush = g.Background();
        Media::ImageBrush b;
        b.ImageSource(g_wb);
        b.Stretch(Media::Stretch::UniformToFill);
        g.Background(b);
        g_rootGrid = g;
        g_brushOn = true;
        FileLog(L"Message scene: video brush applied");
    } else if (!msg && g_brushOn) {
        if (g_rootGrid) g_rootGrid.Background(g_origBrush);
        g_brushOn = false;
        FileLog(L"Message scene gone: brush restored");
    }
}

// Detects the password / sign-in screen (UI thread only).
void UpdatePasswordScene(DependencyObject const& o, DependencyObject const& l) {
    auto fe = o ? o.try_as<FrameworkElement>() : nullptr;
    bool credVisible = fe && fe.Visibility() == Visibility::Visible && fe.ActualWidth() > 0;
    auto lfe = l ? l.try_as<FrameworkElement>() : nullptr;
    // Password screen = credential UI visible while the lock screen UI is collapsed.
    bool pw = credVisible && lfe && lfe.Visibility() != Visibility::Visible;
    if (pw != g_pwScene) {
        if (!g_pwScene && pw) g_pwShownTick = GetTickCount();        
        if (g_pwScene && !pw) {
            g_pwGoneTick = GetTickCount();
            // Only a sign-in screen that stayed up for a while counts as "got through"; startup flickers last only 0.2-0.3 s.
            if (GetTickCount() - g_pwShownTick >= 1000)
            if (!g_previewRole) Wh_SetIntValue(L"pendingStarts", 0);
        }
        g_pwScene = pw;
        FileLog(L"Password screen %s", pw ? L"shown" : L"gone");
    }
}
// Returns true if the image Source really points to our bitmap.
// false = the element exists but rejected/ignored the change (UI structure changed).
bool Attach(Controls::Image const& im, bool isA) {
    auto cur = im.Source();
    if (cur && cur == g_wb) return true;
    if (isA) { g_origSrcA = cur; g_origStrA = im.Stretch(); g_savedA = true; }
    else     { g_origSrcB = cur; g_origStrB = im.Stretch(); g_savedB = true; }
    im.Source(g_wb);
    im.Stretch(Media::Stretch::UniformToFill);
    auto after = im.Source();
    if (!(after && after == g_wb)) return false;
    FileLog(L"Attached to %s (original source: %s)",
           isA ? L"LockLogonImage" : L"SwapLockLogonImage",
           cur ? winrt::get_class_name(cur).c_str() : L"(none)");
    return true;
}

// Puts the original images back WITHOUT tearing the mod down (used on the
// shutdown screen). Called only from the UI thread.
static void RestoreImages() {
    try {
        if (g_imgA && g_savedA && g_imgA.Source() == g_wb) {
            g_imgA.Source(g_origSrcA);
            g_imgA.Stretch(g_origStrA);
        }
        if (g_imgB && g_savedB && g_imgB.Source() == g_wb) {
            g_imgB.Source(g_origSrcB);
            g_imgB.Stretch(g_origStrB);
        }
    } catch (...) {}
}

// Called ONLY from the UI thread (inside GetMsgProc)
void Teardown() {
    FileLog(L"Teardown");
    try {
        if (g_imgA && g_savedA && g_imgA.Source() == g_wb) {
            g_imgA.Source(g_origSrcA);
            g_imgA.Stretch(g_origStrA);
        }
        if (g_imgB && g_savedB && g_imgB.Source() == g_wb) {
            g_imgB.Source(g_origSrcB);
            g_imgB.Stretch(g_origStrB);
        }
    } catch (...) {}
    try { if (g_brushOn && g_rootGrid) g_rootGrid.Background(g_origBrush); } catch (...) {}
    g_rootGrid = nullptr;
    g_origBrush = nullptr;
    g_brushOn = false;
    g_imgA = nullptr;
    g_imgB = nullptr;
    g_origSrcA = nullptr;
    g_origSrcB = nullptr;
    g_wb = nullptr;
    g_pixels = nullptr;
    g_tornDown = true;
}

// Fail-safe: undo every modification and stop all threads, permanently until
// this LogonUI.exe process ends. Called ONLY from the UI thread (Step).
//  - Teardown() restores the original wallpaper (safe, on the UI thread).
//  - g_stop makes the Worker and Decoder exit on their own. We do NOT wait for them
//    here, so the UI thread is never blocked.
//  - The Worker removes the hook when it exits; after g_stop, GetMsgProc
//    is a pass-through.
void TripFailSafe(const wchar_t* reason) {
    if (g_failed.exchange(true)) return;
    FileLog(L"FAIL-SAFE TRIPPED: %s -> original wallpaper restored, mod stopped", reason);
    if (!g_tornDown) Teardown();
    g_stop = true;
}

// Recorded on every exception / attach failure. Errors closer than 500 ms
// count as one because Step() runs on every UI message. kMaxErrors errors within
// 30 seconds -> fail-safe.
void OnStepError(const wchar_t* what) {
    DWORD now = GetTickCount();
    // Right after the password screen closes (sign-in), the lock UI is being torn
    // down, so errors from it are expected and must not trip the fail-safe.
    if (g_pwGoneTick && now - g_pwGoneTick < 10000) return;
    if (g_lastErrTick && now - g_lastErrTick < kErrThrottleMs) return;
    g_lastErrTick = now;
    if (g_errCount == 0 || now - g_errWindowStart > kErrWindowMs) {
        g_errWindowStart = now;
        g_errCount = 0;
    }
    g_errCount++;
    FileLog(L"ERROR #%d: %s", g_errCount, what);
    if (g_failSafe && g_errCount >= kMaxErrors)
        TripFailSafe(L"too many errors while manipulating the UI");
}

static std::wstring g_lastScene;
static bool g_dumpedMsg = false;
static DWORD g_lockDumpTick = 0;   // when the lock screen tree was last dumped
static int g_lockDumps = 0;

void DumpTree(DependencyObject const& o, int d, int& c) {
    if (c > 1500 || d > 40) return;
    c++;
    std::wstring ind(d * 2, L' ');
    auto fe = o.try_as<FrameworkElement>();
    FileLog(L"  %s%s [%s] %dx%d", ind.c_str(), winrt::get_class_name(o).c_str(),
           fe ? fe.Name().c_str() : L"", fe ? (int)fe.ActualWidth() : 0,
           fe ? (int)fe.ActualHeight() : 0);
    if (auto img = o.try_as<Controls::Image>()) {
        auto src = img.Source();
        FileLog(L"  %s  -> Image source: %s", ind.c_str(),
               src ? winrt::get_class_name(src).c_str() : L"(none)");
    }
    int n = Media::VisualTreeHelper::GetChildrenCount(o);
    for (int i = 0; i < n; i++)
        DumpTree(Media::VisualTreeHelper::GetChild(o, i), d + 1, c);
}

void LogScene() {
    auto window = Window::Current();
    auto content = window ? window.Content() : nullptr;
    if (!content) return;
    const wchar_t* names[] = { L"DisplayMessageUserControl", L"LockScreenUserControl",
        L"PicturePasswordLogonView", L"RequestCredentialUserControl",
        L"LockLogonImageGrid" };
    std::wstring s;
    bool msgShown = false;
    for (auto n : names) {
        auto o = FindByName(content, n, 0);
        auto fe = o ? o.try_as<FrameworkElement>() : nullptr;
        s += n;
        s += L"=";
        if (fe) {
            s += std::to_wstring((int)fe.Visibility()) + L"/" +
                 std::to_wstring((int)fe.ActualWidth());
            if (wcscmp(n, L"DisplayMessageUserControl") == 0 && fe.ActualWidth() > 0)
                msgShown = true;
        } else s += L"-";
        s += L" ";
    }
    if (g_debug && g_lockDumps < 2 && (!g_lockDumpTick || GetTickCount() - g_lockDumpTick > 3000)) {
        auto lockObj = FindByName(content, L"LockScreenUserControl", 0);
        auto lockFe = lockObj ? lockObj.try_as<FrameworkElement>() : nullptr;
        if (lockFe && lockFe.Visibility() == Visibility::Visible && lockFe.ActualWidth() > 0) {
            g_lockDumpTick = GetTickCount() | 1;
            g_lockDumps++;
            FileLog(L"Lock screen is visible, dumping the whole window tree (dump #%d):", g_lockDumps);
            int c = 0;
            DumpTree(content, 0, c);
        }
    }    
    if (s != g_lastScene) {
        g_lastScene = s;
        FileLog(L"SCENE (visibility/width, 0=Visible 1=Collapsed): %s", s.c_str());
    }
    if (msgShown && !g_dumpedMsg) {
        g_dumpedMsg = true;
        FileLog(L"DisplayMessage scene is visible, dumping tree:");
        int c = 0;
        DumpTree(content, 0, c);
    }
}

void Step() {
    if (g_failed) return;
    try {
        DWORD now = GetTickCount();
        bool light = now < g_lightUntil.load();

        double t = NowMs();
        if (g_lastStepMs > 0) {
            double gap = t - g_lastStepMs;
            if (gap > 50) {
                g_stalls++;
                if (gap > g_stallMax) g_stallMax = gap;
            }
        }
        g_lastStepMs = t;

        EnsureBitmap();

        // No frame yet: do NOT attach the (still black) bitmap to the UI.
        // If the decoder is dead or the timeout has passed, trip the fail-safe.
        if (g_seq == 0) {
            if (g_failSafe && (g_decodeDead || now - g_initTick >= g_failSafeMs))
                TripFailSafe(g_decodeDead ? L"decoder failed, no video frame produced"
                                          : L"no video frame before timeout");
            return;
        }

        if (g_seq != g_lastSeq && TryEnterCriticalSection(&g_cs)) {
            memcpy(g_pixels, g_shared.data(), g_frameBytes);
            g_lastSeq = g_seq;
            LeaveCriticalSection(&g_cs);
            g_wb.Invalidate();
        }

        bool both = g_imgA && g_imgB;
        DWORD interval = both ? (light ? 250u : 150u) : (now - g_initTick < 5000 ? 30u : 100u);
        if (now - g_lastFind >= interval) {
            g_lastFind = now;
            auto window = Window::Current();
            auto content = window ? window.Content() : nullptr;
            if (content) {
                SceneNodes sn;
                CollectNodes(content, 0, sn);
                UpdateBrush(content, sn.msg);
                UpdatePasswordScene(sn.cred, sn.lock);
                auto a = sn.imgA;
                auto b = sn.imgB;
                g_imgA = a ? a.try_as<Controls::Image>() : nullptr;
                g_imgB = b ? b.try_as<Controls::Image>() : nullptr;
            } else {
                g_imgA = nullptr;
                g_imgB = nullptr;
            }

            if (g_imgA || g_imgB || g_brushOn || g_msgSceneUp) {
                g_everFound = true;
                g_missSince = 0;
                g_missTries = 0;
            } else {
                // Elements not found. During normal scene changes the elements
                // can disappear temporarily, so the fail-safe only trips if
                // the elements were NEVER found since the mod started (UI structure
                // changed), after enough attempts AND the timeout has passed.
                if (!g_missSince) g_missSince = now ? now : 1;
                g_missTries++;
                if (g_failSafe && !g_everFound && g_missTries >= kMinMissTries &&
                    now - g_missSince >= g_failSafeMs) {
                    TripFailSafe(L"LockLogonImage/SwapLockLogonImage not found (UI structure changed)");
                    return;
                }
            }
        }

        // Shutdown / restart status screen: keep the original background.
        const bool hideNow = g_hideOnShutdown && g_msgSceneUp;
        if (hideNow) RestoreImages();

        // Playback hold: no input for a while, or the video is hidden.
        bool idle = false;
        if (g_idleMs) {
            if (!g_lastInput) g_lastInput = now;
            idle = (DWORD)(now - g_lastInput) >= g_idleMs;
        }
        // Battery saver / low battery check, polled every 5 s
        {
            static DWORD s_lastBat = 0;
            if (!s_lastBat || now - s_lastBat >= 5000) {
                s_lastBat = now ? now : 1;
                SYSTEM_POWER_STATUS sps;
                bool bh = false, lowNow = false;
                if (GetSystemPowerStatus(&sps)) {
                    bool onBattery = g_lowBatOnAC || sps.ACLineStatus == 0;  // also true on AC if the toggle is on
                    static unsigned s_lastSig = 0xFFFFFFFFu;
                    unsigned sig = (unsigned)sps.ACLineStatus | ((unsigned)sps.BatteryFlag << 8) |
                                   ((unsigned)sps.SystemStatusFlag << 16) |
                                   ((unsigned)sps.BatteryLifePercent << 24);
                    if (sig != s_lastSig) {
                        s_lastSig = sig;
                        FileLog(L"Power: AC=%u batteryFlag=0x%02X sysStatusFlag=%u pct=%u (threshold=%d level=%d)",
                               (unsigned)sps.ACLineStatus, (unsigned)sps.BatteryFlag,
                               (unsigned)sps.SystemStatusFlag, (unsigned)sps.BatteryLifePercent,
                               g_pauseBatPct, g_lowBatLevel);
                    }                    
                    if (g_pauseBatPct > 0 && onBattery && sps.BatteryLifePercent != 255 &&
                        sps.BatteryLifePercent <= g_pauseBatPct) {
                        if (g_lowBatLevel == 3) bh = true;
                        else if (g_lowBatLevel >= 1) lowNow = true;
                    }
                }
                if (bh != g_batHold.load())
                    FileLog(L"Battery hold %s", bh ? L"ON" : L"OFF");
                g_batHold = bh;
                if (lowNow != g_lowBat.load())
                    FileLog(L"Low battery mode %s (level=%d)", lowNow ? L"ON" : L"OFF", g_lowBatLevel);
                g_lowBat = lowNow;
            }
        }
        bool hold = idle || hideNow || g_batHold.load() || (!g_playOnPassword && g_pwScene);
        if (hold != g_idlePaused.load()) {
            g_idlePaused = hold;
            FileLog(L"Playback hold %s (idle=%d hidden=%d)", hold ? L"ON" : L"OFF",
                   idle ? 1 : 0, hideNow ? 1 : 0);
        }

        bool ok = true;
        if (g_imgA && !hideNow) ok = Attach(g_imgA, true) && ok;
        if (g_imgB && !hideNow) ok = Attach(g_imgB, false) && ok;
        if (!ok) OnStepError(L"Attach failed: Source did not change");

        if (g_debug) {
            static DWORD lastScene = 0;
            if (now - lastScene >= 250) { lastScene = now; LogScene(); }
        }

        if (g_debug && now - g_lastLog > 5000) { g_lastLog = now; LogStats(); }
    } catch (winrt::hresult_error const& e) {
        g_imgA = nullptr;
        g_imgB = nullptr;
        OnStepError(e.message().c_str());
    } catch (...) {
        g_imgA = nullptr;
        g_imgB = nullptr;
        OnStepError(L"other exception (non-WinRT)");
    }
}

LRESULT CALLBACK GetMsgProc(int code, WPARAM w, LPARAM l) {
    if (code >= 0) {
        if (g_teardown) {
            if (!g_tornDown) Teardown();
        } else if (!g_stop && !g_failed) {
            MSG* m = (MSG*)l;
            if (m && (m->message == WM_KEYDOWN || m->message == WM_MOUSEMOVE ||
                      m->message == WM_LBUTTONDOWN ||
                      m->message == 0x0246 /* WM_POINTERDOWN */ ||
                      m->message == 0x0245 /* WM_POINTERUPDATE */))
                g_lastInput = GetTickCount();
            if (m && (m->message == WM_KEYDOWN || m->message == WM_LBUTTONDOWN ||
                      m->message == 0x0246 /* WM_POINTERDOWN */))
                g_lightUntil = GetTickCount() + 600000;
            Step();
        }
    }
    return CallNextHookEx(g_hook.load(), code, w, l);
}

BOOL CALLBACK EnumProc(HWND hwnd, LPARAM) {
    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    if (pid != GetCurrentProcessId()) return TRUE;
    wchar_t cls[128] = {};
    GetClassNameW(hwnd, cls, 128);
    if (wcscmp(cls, L"LogonUI Logon Window") == 0) {
        g_wnd = hwnd;
        return FALSE;
    }
    return TRUE;
}

DWORD WINAPI Worker(LPVOID) {
    for (int i = 0; i < 3000 && !g_stop; i++) {
        g_wnd = nullptr;
        EnumWindows(EnumProc, 0);
        if (g_wnd) break;
        Sleep(10);
    }
    if (!g_wnd) { FileLog(L"LogonUI window not found"); return 0; }
    DWORD tid = GetWindowThreadProcessId(g_wnd, nullptr);
    HHOOK hk = SetWindowsHookExW(WH_GETMESSAGE, GetMsgProc, nullptr, tid);
    g_hook = hk;
    if (!hk) {
        FileLog(L"SetWindowsHookExW failed (error %lu), mod stopped", GetLastError());
        g_stop = true;
        g_wnd = nullptr;
        return 0;
    }
    FileLog(L"Window=%p thread=%u hook=%p", g_wnd, tid, hk);
    while (!g_stop && IsWindow(g_wnd)) {
        PostMessageW(g_wnd, WM_NULL, 0, 0);
        Sleep(16);
    }
    // Exiting because of fail-safe / uninit / window gone: remove the hook so
    // the UI thread has no more overhead.
    UnhookOnce();
    g_wnd = nullptr;
    FileLog(L"Worker finished");
    return 0;
}

// ---------- Preview window (runs inside explorer.exe) ----------
// The same mod file also loads into explorer.exe. There it does not touch the lock
// screen: it plays the video with the same fit / color / speed settings in a normal window.
static HANDLE g_prevThread = nullptr;
static DWORD g_prevTid = 0;
static HWND g_prevCtl = nullptr;   // hidden window: receives the hotkey and tray messages
static HWND g_prevWnd = nullptr;   // the visible preview window
static bool g_prevAutoOpen = false, g_prevTray = false, g_prevTopmost = true;
static int g_prevHotkey = 0;       // 0 = no hotkey, 1 = a valid hotkey text was read
static UINT g_prevHkMods = 0, g_prevHkVk = 0;   // parsed hotkey: MOD_* flags and virtual-key code
static std::vector<uint8_t> g_prevPx;
static bool g_prevShowOverlay = true;
static std::vector<uint32_t> g_ovPx;     // overlay scaled to the canvas, premultiplied BGRA
static FILETIME g_ovStamp = {};
static bool g_ovDirty = false;
static LONG g_prevLastSeq = 0;
static UINT g_msgTaskbar = 0;
static const wchar_t kPrevClass[] = L"LockScreenLiveWallpaperPreview";
static const wchar_t kPrevTitle[] = L"Lock Screen Live Wallpaper - Preview";
static const UINT kTrayMsg = WM_APP + 1;

// Turns text such as "Ctrl+Alt+P" into RegisterHotKey modifiers and a virtual-key code.
// Returns false (outputs untouched) when the text is empty or invalid.
// At least one modifier is required, otherwise a plain letter would be blocked in every program.
static bool ParseHotkey(const wchar_t* text, UINT& outMods, UINT& outVk) {
    if (!text) return false;
    std::wstring s;
    for (const wchar_t* c = text; *c; c++)
        if (*c != L' ' && *c != L'\t') s += (wchar_t)towupper(*c);
    if (s.empty()) return false;
    struct NamedKey { const wchar_t* name; UINT vk; };
    static const NamedKey kNamed[] = {
        { L"SPACE", VK_SPACE }, { L"TAB", VK_TAB }, { L"ENTER", VK_RETURN },
        { L"ESC", VK_ESCAPE }, { L"INSERT", VK_INSERT }, { L"DELETE", VK_DELETE },
        { L"HOME", VK_HOME }, { L"END", VK_END }, { L"PGUP", VK_PRIOR },
        { L"PGDN", VK_NEXT }, { L"UP", VK_UP }, { L"DOWN", VK_DOWN },
        { L"LEFT", VK_LEFT }, { L"RIGHT", VK_RIGHT } };
    UINT mods = 0, vk = 0;
    size_t pos = 0;
    while (pos <= s.size()) {
        size_t plus = s.find(L'+', pos);
        std::wstring tok = (plus == std::wstring::npos) ? s.substr(pos) : s.substr(pos, plus - pos);
        pos = (plus == std::wstring::npos) ? s.size() + 1 : plus + 1;
        if (tok.empty()) return false;   // "Ctrl+", "++" and similar
        if (tok == L"CTRL" || tok == L"CONTROL") mods |= MOD_CONTROL;
        else if (tok == L"ALT") mods |= MOD_ALT;
        else if (tok == L"SHIFT") mods |= MOD_SHIFT;
        else if (tok == L"WIN" || tok == L"WINDOWS") mods |= MOD_WIN;
        else {
            if (vk) return false;        // two main keys
            if (tok.size() == 1 && ((tok[0] >= L'A' && tok[0] <= L'Z') ||
                                    (tok[0] >= L'0' && tok[0] <= L'9'))) {
                vk = (UINT)tok[0];
            } else if (tok[0] == L'F' && tok.size() >= 2 && tok.size() <= 3) {
                int n = 0;
                for (size_t i = 1; i < tok.size(); i++) {
                    if (tok[i] < L'0' || tok[i] > L'9') return false;
                    n = n * 10 + (tok[i] - L'0');
                }
                if (n < 1 || n > 24) return false;
                vk = (UINT)(VK_F1 + n - 1);
            } else {
                for (const auto& k : kNamed)
                    if (tok == k.name) vk = k.vk;
            }
            if (!vk) return false;
        }
    }
    if (!vk || !mods) return false;
    outMods = mods;
    outVk = vk;
    return true;
}

// Loads overlay.bgra (written by the LockApp.exe side) and scales it to the canvas.
static bool PreviewLoadOverlay(bool force) {
    if (!g_prevShowOverlay) {
        if (!g_ovPx.empty()) { g_ovPx.clear(); g_ovDirty = true; }
        return false;
    }
    WIN32_FILE_ATTRIBUTE_DATA fa;
    if (!kOverlayFile[0] ||
        !GetFileAttributesExW(kOverlayFile, GetFileExInfoStandard, &fa)) {
        if (!g_ovPx.empty()) { g_ovPx.clear(); g_ovDirty = true; }
        return false;
    }
    if (!force && g_ovPx.size() == (size_t)kW * kH &&
        CompareFileTime(&fa.ftLastWriteTime, &g_ovStamp) == 0)
        return true;
    HANDLE f = CreateFileW(kOverlayFile, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE,
                           nullptr, OPEN_EXISTING, 0, nullptr);
    if (f == INVALID_HANDLE_VALUE) return false;
    CacheHdr hd = {};
    DWORD got = 0;
    bool ok = ReadFile(f, &hd, sizeof(hd), &got, nullptr) && got == sizeof(hd) &&
              hd.magic == kOvMagic && hd.w > 0 && hd.h > 0 && hd.w <= 4096 && hd.h <= 4096;
    std::vector<uint32_t> src;
    int w = (int)hd.w, h = (int)hd.h;
    if (ok) {
        src.resize((size_t)w * h);
        ok = ReadFile(f, src.data(), (DWORD)(src.size() * 4), &got, nullptr) &&
             got == src.size() * 4;
    }
    CloseHandle(f);
    if (!ok) { FileLog(L"Preview: overlay file unreadable"); return false; }

    // Box filter on premultiplied pixels (averaging them stays correct).
    std::vector<uint32_t> out((size_t)kW * kH);
    for (int y = 0; y < kH; y++) {
        int y0 = (int)((long long)y * h / kH);
        int y1 = (int)((long long)(y + 1) * h / kH);
        if (y1 <= y0) y1 = y0 + 1;
        if (y1 > h) y1 = h;
        for (int x = 0; x < kW; x++) {
            int x0 = (int)((long long)x * w / kW);
            int x1 = (int)((long long)(x + 1) * w / kW);
            if (x1 <= x0) x1 = x0 + 1;
            if (x1 > w) x1 = w;
            uint32_t sa = 0, sr = 0, sg = 0, sb = 0, cnt = 0;
            for (int yy = y0; yy < y1; yy++) {
                const uint32_t* row = src.data() + (size_t)yy * w;
                for (int xx = x0; xx < x1; xx++) {
                    uint32_t p = row[xx];
                    sa += p >> 24; sr += (p >> 16) & 255; sg += (p >> 8) & 255; sb += p & 255;
                    cnt++;
                }
            }
            out[(size_t)y * kW + x] = cnt
                ? (((sa / cnt) << 24) | ((sr / cnt) << 16) | ((sg / cnt) << 8) | (sb / cnt))
                : 0u;
        }
    }
    g_ovPx.swap(out);
    g_ovStamp = fa.ftLastWriteTime;
    g_ovDirty = true;
    FileLog(L"Preview: overlay loaded %dx%d -> %dx%d", w, h, kW, kH);
    return true;
}

// Draws the overlay on top of g_prevPx (premultiplied alpha over opaque video).
static void PreviewBlendOverlay() {
    size_t n = (size_t)kW * kH;
    if (g_ovPx.size() != n || g_prevPx.size() != g_frameBytes) return;
    uint32_t* d = (uint32_t*)g_prevPx.data();
    const uint32_t* o = g_ovPx.data();
    for (size_t i = 0; i < n; i++) {
        uint32_t s = o[i];
        uint32_t a = s >> 24;
        if (!a) continue;
        if (a == 255) { d[i] = s; continue; }
        uint32_t p = d[i], ia = 255 - a;
        uint32_t r = ((s >> 16) & 255) + ((((p >> 16) & 255) * ia + 127) / 255);
        uint32_t g = ((s >> 8) & 255) + ((((p >> 8) & 255) * ia + 127) / 255);
        uint32_t b = (s & 255) + (((p & 255) * ia + 127) / 255);
        if (r > 255) r = 255;
        if (g > 255) g = 255;
        if (b > 255) b = 255;
        d[i] = 0xFF000000u | (r << 16) | (g << 8) | b;
    }
}
static void PreviewSavePlacement() {
    if (!g_prevWnd || IsIconic(g_prevWnd)) return;
    RECT wr, cr;
    if (!GetWindowRect(g_prevWnd, &wr) || !GetClientRect(g_prevWnd, &cr)) return;
    Wh_SetIntValue(L"previewX", wr.left);
    Wh_SetIntValue(L"previewY", wr.top);
    Wh_SetIntValue(L"previewCW", cr.right);
}

static void PreviewCreateWindow() {
    int cw = Wh_GetIntValue(L"previewCW", 640);
    if (cw < 240) cw = 240;
    if (cw > 3840) cw = 3840;
    int ch = (int)((long long)cw * kH / kW);
    DWORD style = WS_OVERLAPPEDWINDOW;
    RECT rc = { 0, 0, cw, ch };
    AdjustWindowRect(&rc, style, FALSE);
    int x = Wh_GetIntValue(L"previewX", (int)CW_USEDEFAULT);
    int y = Wh_GetIntValue(L"previewY", (int)CW_USEDEFAULT);
    if (x != (int)CW_USEDEFAULT) {
        POINT pt = { x + 40, y + 40 };
        if (!MonitorFromPoint(pt, MONITOR_DEFAULTTONULL)) x = y = (int)CW_USEDEFAULT;
    }
    g_prevWnd = CreateWindowExW(g_prevTopmost ? WS_EX_TOPMOST : 0, kPrevClass, kPrevTitle,
                                style, x, y, rc.right - rc.left, rc.bottom - rc.top,
                                nullptr, nullptr, GetModuleHandleW(nullptr), nullptr);
    if (!g_prevWnd) {
        FileLog(L"Preview: CreateWindow failed (error %lu)", GetLastError());
        return;
    }
    {
        WTA_OPTIONS wo = { WTNCA_NODRAWICON, WTNCA_NODRAWICON };
        SetWindowThemeAttribute(g_prevWnd, WTA_NONCLIENT, &wo, sizeof(wo));
    }
    SetTimer(g_prevWnd, 1, 33, nullptr);
    FileLog(L"Preview: window created (client %dx%d, canvas %dx%d)", cw, ch, kW, kH);
}

static void PreviewShow() {
    if (!g_prevWnd) PreviewCreateWindow();
    if (!g_prevWnd) return;
    g_idlePaused = false;
    PreviewLoadOverlay(false);
    if (!g_decThread && !(g_paused && g_cacheHit))
        g_decThread = CreateThread(nullptr, 0, DecodeThread, nullptr, 0, nullptr);
    ShowWindow(g_prevWnd, IsIconic(g_prevWnd) ? SW_RESTORE : SW_SHOW);
    SetForegroundWindow(g_prevWnd);
}

static void PreviewHide() {
    if (!g_prevWnd) return;
    PreviewSavePlacement();
    ShowWindow(g_prevWnd, SW_HIDE);
    g_idlePaused = true;   // the decoder parks itself while the window is hidden
}

static void PreviewToggle() {
    if (g_prevWnd && IsWindowVisible(g_prevWnd) && !IsIconic(g_prevWnd)) PreviewHide();
    else PreviewShow();
}

static void PreviewTrayAdd() {
    NOTIFYICONDATAW nid = {};
    nid.cbSize = sizeof(nid);
    nid.hWnd = g_prevCtl;
    nid.uID = 1;
    nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    nid.uCallbackMessage = kTrayMsg;
    nid.hIcon = LoadIconW(nullptr, IDI_APPLICATION);
    lstrcpynW(nid.szTip, L"Lock Screen Live Wallpaper preview", ARRAYSIZE(nid.szTip));
    Shell_NotifyIconW(NIM_ADD, &nid);
}

static void PreviewTrayRemove() {
    NOTIFYICONDATAW nid = {};
    nid.cbSize = sizeof(nid);
    nid.hWnd = g_prevCtl;
    nid.uID = 1;
    Shell_NotifyIconW(NIM_DELETE, &nid);
}

static const UINT kReloadMsg = WM_APP + 2;
static BOOL (*g_modInitFn)() = nullptr;   // set at the top of Wh_ModInit so the preview can re-run it

// Applies changed settings without closing the preview window.
// Runs on the preview thread (called from PreviewProc), so it cannot race with the paint timer.
static bool PreviewReloadInPlace() {
    if (!g_modInitFn) return false;
    FileLog(L"Preview: applying settings in place");
    g_stop = true;
    if (g_decThread) {
        WaitForSingleObject(g_decThread, INFINITE);
        CloseHandle(g_decThread);
        g_decThread = nullptr;
    }
    g_stop = false;
    g_decoderDone = false;
    g_decodeDead = false;
    g_decPublished = false;
    g_frameDone = false;
    g_gpuActive = false;
    g_lowBat = false;
    g_seq = 0;
    g_videoFile.clear();
    g_isGif = false;
    g_modInitFn();   // re-reads every setting; the preview role never starts a second thread
    if (g_prevPx.size() != g_frameBytes) g_prevPx.assign(g_frameBytes, 0);
    if (g_videoFile.empty()) std::fill(g_prevPx.begin(), g_prevPx.end(), (uint8_t)0);
    g_prevLastSeq = 0;
    PreviewLoadOverlay(true);   // canvas size or the toggle may have changed
    bool visible = g_prevWnd && IsWindowVisible(g_prevWnd) && !IsIconic(g_prevWnd);
    if (visible && !g_videoFile.empty() && !(g_paused && g_cacheHit)) {
        g_idlePaused = false;
        g_decThread = CreateThread(nullptr, 0, DecodeThread, nullptr, 0, nullptr);
    }
    if (g_prevWnd) InvalidateRect(g_prevWnd, nullptr, FALSE);
    return true;
}

static LRESULT CALLBACK PreviewProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    if (hwnd == g_prevCtl) {
        if (msg == WM_HOTKEY) { PreviewToggle(); return 0; }
        if (msg == kReloadMsg) return PreviewReloadInPlace() ? 1 : 0;        
        if (msg == kTrayMsg) {
            if (lp == WM_LBUTTONUP) {
                PreviewToggle();
            } else if (lp == WM_RBUTTONUP) {
                HMENU m = CreatePopupMenu();
                AppendMenuW(m, MF_STRING, 1, L"Show / hide preview");
                AppendMenuW(m, MF_STRING, 2, L"Remove this tray icon");
                POINT pt;
                GetCursorPos(&pt);
                SetForegroundWindow(hwnd);
                int cmd = (int)TrackPopupMenu(m, TPM_RETURNCMD | TPM_RIGHTBUTTON, pt.x, pt.y,
                                              0, hwnd, nullptr);
                PostMessageW(hwnd, WM_NULL, 0, 0);
                DestroyMenu(m);
                if (cmd == 1) PreviewToggle();
                else if (cmd == 2) PreviewTrayRemove();
            }
            return 0;
        }
        if (g_msgTaskbar && msg == g_msgTaskbar && g_prevTray) {
            PreviewTrayAdd();   // explorer restarted: the tray icon has to be added again
            return 0;
        }
        return DefWindowProcW(hwnd, msg, wp, lp);
    }
    switch (msg) {
    case WM_TIMER:
        { static int s_ovTick = 0; if (wp == 1 && ++s_ovTick % 60 == 0) PreviewLoadOverlay(false); }
        if (wp == 1 && (g_seq != g_prevLastSeq || g_ovDirty) && TryEnterCriticalSection(&g_cs)) {
            if (g_prevPx.size() == g_frameBytes)
                memcpy(g_prevPx.data(), g_shared.data(), g_frameBytes);
            g_prevLastSeq = g_seq;
            PreviewBlendOverlay();
            g_ovDirty = false;            
            LeaveCriticalSection(&g_cs);
            InvalidateRect(hwnd, nullptr, FALSE);
        }
        return 0;
    case WM_ERASEBKGND:
        return 1;
    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC realDc = BeginPaint(hwnd, &ps);
        HDC dc = realDc;
        RECT cr0;
        GetClientRect(hwnd, &cr0);
        BP_PAINTPARAMS bpp = { sizeof(bpp) };
        HDC bufDc = nullptr;
        HPAINTBUFFER hbuf = BeginBufferedPaint(realDc, &cr0, BPBF_TOPDOWNDIB, &bpp, &bufDc);
        if (hbuf && bufDc) dc = bufDc;   // draw into an ARGB buffer so it can be made opaque
        RECT cr;
        GetClientRect(hwnd, &cr);
        int cw = cr.right, ch = cr.bottom;
        if (cw > 0 && ch > 0 && g_prevPx.size() == g_frameBytes) {
            double sc = std::min((double)cw / kW, (double)ch / kH);
            int dw = (int)(kW * sc), dh = (int)(kH * sc);
            int dx = (cw - dw) / 2, dy = (ch - dh) / 2;
            BITMAPINFO bmi = {};
            bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
            bmi.bmiHeader.biWidth = kW;
            bmi.bmiHeader.biHeight = -kH;   // top-down
            bmi.bmiHeader.biPlanes = 1;
            bmi.bmiHeader.biBitCount = 32;
            bmi.bmiHeader.biCompression = BI_RGB;
            SetStretchBltMode(dc, HALFTONE);
            SetBrushOrgEx(dc, 0, 0, nullptr);
            StretchDIBits(dc, dx, dy, dw, dh, 0, 0, kW, kH, g_prevPx.data(), &bmi,
                          DIB_RGB_COLORS, SRCCOPY);
            HBRUSH blk = (HBRUSH)GetStockObject(BLACK_BRUSH);
            if (dx > 0) {
                RECT a = { 0, 0, dx, ch }, b = { dx + dw, 0, cw, ch };
                FillRect(dc, &a, blk);
                FillRect(dc, &b, blk);
            }
            if (dy > 0) {
                RECT a = { 0, 0, cw, dy }, b = { 0, dy + dh, cw, ch };
                FillRect(dc, &a, blk);
                FillRect(dc, &b, blk);
            }
        }
        if (g_prevShowOverlay && g_ovPx.empty() && g_prevPx.size() == g_frameBytes) {
            SetBkMode(dc, TRANSPARENT);
            SetTextColor(dc, RGB(255, 255, 255));
            RECT tr = { 8, ch - 28, cw - 8, ch - 4 };
            DrawTextW(dc, L"No overlay yet: lock the PC once (Win+L) and wait 6 s to capture it", -1, &tr,
                      DT_LEFT | DT_SINGLELINE | DT_BOTTOM | DT_END_ELLIPSIS);
        }
        if (hbuf) {
            // Fully opaque, even when a translucency mod extends the glass into the client area.
            BufferedPaintSetAlpha(hbuf, &cr0, 255);
            EndBufferedPaint(hbuf, TRUE);
        }
        EndPaint(hwnd, &ps);
        return 0;
    }
    case WM_SIZING: {
        // keep the aspect ratio of the lock screen canvas
        RECT* r = (RECT*)lp;
        RECT nc = { 0, 0, 0, 0 };
        AdjustWindowRectEx(&nc, (DWORD)GetWindowLongW(hwnd, GWL_STYLE), FALSE,
                           (DWORD)GetWindowLongW(hwnd, GWL_EXSTYLE));
        int ncw = nc.right - nc.left, nch = nc.bottom - nc.top;
        int cw = (r->right - r->left) - ncw;
        int ch = (int)((long long)cw * kH / kW);
        if (wp == WMSZ_TOP || wp == WMSZ_TOPLEFT || wp == WMSZ_TOPRIGHT)
            r->top = r->bottom - (ch + nch);
        else
            r->bottom = r->top + ch + nch;
        return TRUE;
    }
    case WM_SIZE:
        g_idlePaused = (wp == SIZE_MINIMIZED);
        break;
    case WM_EXITSIZEMOVE:
        PreviewSavePlacement();
        break;
    case WM_KEYDOWN:
        if (wp == VK_ESCAPE) { PreviewHide(); return 0; }
        break;
    case WM_CLOSE:
        PreviewHide();   // hide instead of destroy: the hotkey / tray can bring it back
        return 0;
    }
    return DefWindowProcW(hwnd, msg, wp, lp);
}

static DWORD WINAPI PreviewThread(LPVOID) {
    g_prevPx.assign(g_frameBytes, 0);
    HINSTANCE hi = GetModuleHandleW(nullptr);
    BufferedPaintInit();    
    WNDCLASSW wc = {};
    wc.lpfnWndProc = PreviewProc;
    wc.hInstance = hi;
    wc.lpszClassName = kPrevClass;
    wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    if (!RegisterClassW(&wc)) {
        FileLog(L"Preview: RegisterClass failed (error %lu)", GetLastError());
        return 1;
    }
    g_prevCtl = CreateWindowExW(WS_EX_TOOLWINDOW, kPrevClass, L"", WS_POPUP, 0, 0, 0, 0,
                                nullptr, nullptr, hi, nullptr);
    g_msgTaskbar = RegisterWindowMessageW(L"TaskbarCreated");
    if (g_prevCtl) {
        if (g_prevHotkey) {
            UINT mods = g_prevHkMods | MOD_NOREPEAT, vk = g_prevHkVk;
            if (RegisterHotKey(g_prevCtl, 1, mods, vk)) FileLog(L"Preview: hotkey registered");
            else FileLog(L"Preview: RegisterHotKey failed (error %lu), maybe already in use", GetLastError());
        }
        if (g_prevTray) PreviewTrayAdd();
    }
    if (g_prevAutoOpen) PreviewShow();
    MSG m;
    while (GetMessageW(&m, nullptr, 0, 0) > 0) {
        TranslateMessage(&m);
        DispatchMessageW(&m);
    }
    // cleanup (window handles must be destroyed on the thread that created them)
    if (g_prevCtl) {
        UnregisterHotKey(g_prevCtl, 1);
        if (g_prevTray) PreviewTrayRemove();
    }
    if (g_prevWnd) {
        PreviewSavePlacement();
        DestroyWindow(g_prevWnd);
        g_prevWnd = nullptr;
    }
    if (g_prevCtl) {
        DestroyWindow(g_prevCtl);
        g_prevCtl = nullptr;
    }
    BufferedPaintUnInit();
    UnregisterClassW(kPrevClass, hi);
    FileLog(L"Preview: stopped");
    return 0;
}

// ---------- LockApp.exe overlay capture ----------
// Renders LockScreenTextContent (clock, date, status icons, media controls) to a
// transparent bitmap and saves it to overlay.bgra for the preview window.
static bool g_ovWanted = false;
static int g_ovState = 0;          // 0 = not yet, 1 = capture running, 2 = done (UI thread only)
static DWORD g_ovLastTry = 0;
static DWORD g_ovSession = 0;      // when the current lock session was first seen
static DWORD g_ovLastMsg = 0;      // last time the hook ran (to detect a suspended LockApp)
static void SaveOverlay(const uint8_t* px, int w, int h) {
    CreateDirectoryW(kCacheDir, nullptr);
    HANDLE f = CreateFileW(kOverlayTmp, GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS,
                           FILE_ATTRIBUTE_NORMAL, nullptr);
    if (f == INVALID_HANDLE_VALUE) {
        FileLog(L"Overlay: cannot write %s (error %lu)", kOverlayTmp, GetLastError());
        return;
    }
    CacheHdr hd = { kOvMagic, (uint32_t)w, (uint32_t)h, 0 };
    DWORD wr = 0;
    bool ok = WriteFile(f, &hd, sizeof(hd), &wr, nullptr) &&
              WriteFile(f, px, (DWORD)((size_t)w * h * 4), &wr, nullptr);
    CloseHandle(f);
    if (ok && MoveFileExW(kOverlayTmp, kOverlayFile, MOVEFILE_REPLACE_EXISTING))
        FileLog(L"Overlay: saved %dx%d to %s", w, h, kOverlayFile);
    else
        FileLog(L"Overlay: save failed (error %lu)", GetLastError());
}

// Renders the visible lock screen pieces one by one and puts each one at its real
// screen position (RenderTargetBitmap re-lays a big Grid out into a narrow column).
static winrt::fire_and_forget CaptureOverlayParts(UIElement root) {
    bool ok = false;
    try {
        auto rootFe = root.try_as<FrameworkElement>();      
        if (rootFe && rootFe.ActualWidth() > 0 && rootFe.ActualHeight() > 0) {
            struct Part { std::vector<uint32_t> px; int w = 0, h = 0, x = 0, y = 0; };
            std::vector<Part> parts;
            double scale = 0;
            const wchar_t* names[] = { L"Time", L"Date", L"BackplateBorder", L"MediaControlsContainer" };
            for (const wchar_t* name : names) {
                auto o = FindByName(root, name, 0);
                auto fe = o ? o.try_as<FrameworkElement>() : nullptr;
                if (!fe || fe.ActualWidth() <= 0 || fe.ActualHeight() <= 0 ||
                    fe.Visibility() != Visibility::Visible) continue;
                auto pt = fe.TransformToVisual(rootFe).TransformPoint(
                    winrt::Windows::Foundation::Point{ 0.0f, 0.0f });
                Media::Imaging::RenderTargetBitmap rtb;
                // The big clock glyphs overflow the TextBlock box and RenderTargetBitmap clips
                // to the element bounds, so give the text temporary room above and below.
                double padTop = 0, padBottom = 0;
                auto tbk = fe.try_as<Controls::TextBlock>();
                Thickness oldPad{};
                double oldH = fe.Height();
                bool heightChanged = false;
                if (tbk) {
                    oldPad = tbk.Padding();
                    FileLog(L"Overlay: %s fontSize=%.1f lineHeight=%.1f stacking=%d padding=(%.1f,%.1f,%.1f,%.1f) height=%.1f explicitHeight=%.1f",
                            name, tbk.FontSize(), tbk.LineHeight(), (int)tbk.LineStackingStrategy(),
                            oldPad.Left, oldPad.Top, oldPad.Right, oldPad.Bottom,
                            fe.ActualHeight(), oldH);
                    if (wcscmp(name, L"Time") == 0) { padTop = 90; padBottom = 40; }
                    else if (wcscmp(name, L"Date") == 0) { padTop = 8; padBottom = 8; }
                    if (padTop + padBottom > 0) {
                        tbk.Padding(Thickness{ oldPad.Left, oldPad.Top + padTop,
                                               oldPad.Right, oldPad.Bottom + padBottom });
                        if (!std::isnan(oldH) && oldH > 0) {
                            fe.Height(oldH + padTop + padBottom);
                            heightChanged = true;
                        }
                    }
                }
                bool renderedPart = false;
                try {
                    co_await rtb.RenderAsync(fe);
                    renderedPart = true;
                } catch (...) {}
                if (tbk) {
                    tbk.Padding(oldPad);          // always put the real layout back
                    if (heightChanged) fe.Height(oldH);
                }
                if (!renderedPart) continue;
                int pw = rtb.PixelWidth(), ph = rtb.PixelHeight();
                if (pw <= 0 || ph <= 0 || pw > 4096 || ph > 4096) continue;
                auto buf = co_await rtb.GetPixelsAsync();
                winrt::com_ptr<IBufferByteAccess> bba;
                winrt::check_hresult(winrt::get_unknown(buf)->QueryInterface(
                    IID_IBufferByteAccess_, bba.put_void()));
                uint8_t* p = nullptr;
                winrt::check_hresult(bba->Buffer(&p));
                if (!p || buf.Length() < (uint32_t)pw * (uint32_t)ph * 4) continue;
                if (scale <= 0) {
                    scale = 0;
                    try {
                        auto xr = rootFe.XamlRoot();
                        if (xr) scale = xr.RasterizationScale();
                    } catch (...) {}
                    FileLog(L"Overlay: rasterScale=%.3f systemDpiScale=%.3f", scale, (double)GetDpiForSystem() / 96.0);
                    if (scale < 0.5 || scale > 4.0) scale = (double)GetDpiForSystem() / 96.0;
                    if (scale < 0.5 || scale > 4.0) scale = 1.0;
                }
                FileLog(L"Overlay: %s layout=(%.1f,%.1f %.1fx%.1f) bitmap=%dx%d px, dpiScale=%.2f", name, (double)pt.X, (double)pt.Y, fe.ActualWidth(), fe.ActualHeight(), pw, ph, scale);
                Part pr;
                pr.w = pw; pr.h = ph;
                pr.x = (wcscmp(name, L"MediaControlsContainer") == 0)
                           ? (int)lround((pt.X + fe.ActualWidth() * 0.5) * scale - pw * 0.5)
                           : (int)lround(rootFe.ActualWidth() * 0.5 * scale - pw * 0.5);
                pr.y = tbk ? (int)lround((pt.Y - padTop) * scale)
                           : (int)lround((pt.Y + fe.ActualHeight() * 0.5) * scale - ph * 0.5);
                pr.px.assign((const uint32_t*)p, (const uint32_t*)p + (size_t)pw * ph);
                FileLog(L"Overlay: part %s at (%d,%d) size %dx%d (element %dx%d)", name,
                        pr.x, pr.y, pw, ph, (int)fe.ActualWidth(), (int)fe.ActualHeight());
                parts.push_back(std::move(pr));
            }
            if (!parts.empty()) {
                int W = (int)lround(rootFe.ActualWidth() * scale);
                int H = (int)lround(rootFe.ActualHeight() * scale);
                if (W > 0 && H > 0 && W <= 4096 && H <= 4096) {
                    std::vector<uint32_t> canvas((size_t)W * H, 0u);
                    for (auto& pr : parts) {
                        for (int yy = 0; yy < pr.h; yy++) {
                            int cy = pr.y + yy;
                            if (cy < 0 || cy >= H) continue;
                            for (int xx = 0; xx < pr.w; xx++) {
                                int cx = pr.x + xx;
                                if (cx < 0 || cx >= W) continue;
                                uint32_t s = pr.px[(size_t)yy * pr.w + xx];
                                uint32_t a = s >> 24;
                                if (!a) continue;
                                uint32_t& d = canvas[(size_t)cy * W + cx];
                                if (a == 255) { d = s; continue; }
                                uint32_t ia = 255 - a;
                                uint32_t ra = a + ((d >> 24) * ia + 127) / 255;
                                uint32_t rr = ((s >> 16) & 255) + (((d >> 16) & 255) * ia + 127) / 255;
                                uint32_t rg = ((s >> 8) & 255) + (((d >> 8) & 255) * ia + 127) / 255;
                                uint32_t rb = (s & 255) + ((d & 255) * ia + 127) / 255;
                                if (ra > 255) ra = 255;
                                if (rr > 255) rr = 255;
                                if (rg > 255) rg = 255;
                                if (rb > 255) rb = 255;
                                d = (ra << 24) | (rr << 16) | (rg << 8) | rb;
                            }
                        }
                    }
                    size_t vis = 0;
                    for (size_t i = 0; i < canvas.size(); i++) if (canvas[i] >> 24) vis++;
                    FileLog(L"Overlay: composed %dx%d from %d parts, %zu visible pixels", W, H,
                            (int)parts.size(), vis);
                    if (vis > 0) {
                        SaveOverlay((const uint8_t*)canvas.data(), W, H);
                        ok = true;
                    }
                }
            } else {
                FileLog(L"Overlay: none of the parts could be rendered");
            }
        }
    } catch (winrt::hresult_error const& e) {
        FileLog(L"Overlay: capture failed: %s", e.message().c_str());
    } catch (...) {
        FileLog(L"Overlay: capture failed (other exception)");
    }
    g_ovState = ok ? 2 : 0;
}

static void OvWaitLog(int code, const wchar_t* why) {
    static int s_last = -1;
    if (code == s_last) return;   // log each reason only when it changes
    s_last = code;
    FileLog(L"Overlay: waiting (%s)", why);
}

static void TryStartOverlayCapture() {
    try {
        auto window = Window::Current();
        auto content = window ? window.Content() : nullptr;
        if (!content) { OvWaitLog(1, L"no XAML content"); return; }
        auto o = FindByName(content, L"LockScreenTextContent", 0);
        auto fe = o ? o.try_as<FrameworkElement>() : nullptr;
        if (!fe) { OvWaitLog(2, L"LockScreenTextContent not found"); return; }
        if (fe.ActualWidth() < 100) { OvWaitLog(3, L"LockScreenTextContent has no size"); return; }
        auto t = FindByName(content, L"Time", 0);
        auto tb = t ? t.try_as<Controls::TextBlock>() : nullptr;
        if (!tb) { OvWaitLog(4, L"Time TextBlock not found"); return; }
        if (tb.Text().empty()) { OvWaitLog(5, L"Time text is empty"); return; }
        if (tb.ActualWidth() <= 0) { OvWaitLog(6, L"Time has no width"); return; }
        g_ovState = 1;
        FileLog(L"Overlay: starting capture (%dx%d, visibility=%d, opacity=%.2f, time opacity=%.2f)",
                (int)fe.ActualWidth(), (int)fe.ActualHeight(), (int)fe.Visibility(),
                fe.Opacity(), tb.Opacity());
        CaptureOverlayParts(content);
    } catch (...) {
        FileLog(L"Overlay: exception while preparing the capture");
    }
}

// ---------- LockApp.exe probe (read-only: only logs its XAML tree) ----------
// The clock, date and status icons of the lock screen are drawn by LockApp.exe, not by LogonUI.
static bool g_lockAppRole = false;
static HANDLE g_probeThread = nullptr;
static std::atomic<HHOOK> g_probeHook{nullptr};
static DWORD g_probeStart = 0;
static int g_probeDumps = 0;

static LRESULT CALLBACK ProbeMsgProc(int code, WPARAM w, LPARAM l) {
    if (code >= 0 && !g_stop) {
        DWORD age = GetTickCount() - g_probeStart;
        DWORD nowT = GetTickCount();
        {
            // LockApp.exe stays alive between locks. The window becoming visible again,
            // or a long gap between hook calls (process was suspended), starts a new session.
            static bool s_vis = false;
            bool vis = false;
            try { auto w2 = Window::Current(); vis = w2 && w2.Visible(); } catch (...) {}
            if ((vis && !s_vis) || (g_ovLastMsg && nowT - g_ovLastMsg > 5000)) {
                g_ovSession = nowT;
                g_ovState = 0;
                FileLog(L"Overlay: new lock session (visible=%d)", vis ? 1 : 0);
            }
            s_vis = vis;
            g_ovLastMsg = nowT;
        }
        if (g_ovWanted && g_ovState == 0 && nowT - g_ovSession > 3500 && nowT - g_ovSession < 20000 &&
            GetTickCount() - g_ovLastTry > 1000) {
            g_ovLastTry = GetTickCount();
            TryStartOverlayCapture();
        }        
        // dump once about 2 s and once about 6 s after the hook was installed
        if (g_debug && ((g_probeDumps == 0 && age > 2000) || (g_probeDumps == 1 && age > 6000))) {
            g_probeDumps++;
            try {
                auto window = Window::Current();
                auto content = window ? window.Content() : nullptr;
                if (content) {
                    FileLog(L"LockApp: dumping the XAML tree (dump #%d)", g_probeDumps);
                    int c = 0;
                    DumpTree(content, 0, c);
                    FileLog(L"LockApp: dump #%d done (%d nodes)", g_probeDumps, c);
                } else {
                    FileLog(L"LockApp: no XAML content yet (dump #%d)", g_probeDumps);
                }
            } catch (...) {
                FileLog(L"LockApp: exception while dumping (dump #%d)", g_probeDumps);
            }
        }
    }
    return CallNextHookEx(g_probeHook.load(), code, w, l);
}

static BOOL CALLBACK ProbeEnum(HWND hwnd, LPARAM lp) {
    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    if (pid != GetCurrentProcessId()) return TRUE;
    wchar_t cls[128] = {};
    GetClassNameW(hwnd, cls, 128);
    if (wcscmp(cls, L"Windows.UI.Core.CoreWindow") == 0) {
        *(HWND*)lp = hwnd;
        return FALSE;
    }
    return TRUE;
}

static DWORD WINAPI ProbeThread(LPVOID) {
    HWND w = nullptr;
    for (int i = 0; i < 2000 && !g_stop && !w; i++) {   // wait up to about 20 s for the window
        EnumWindows(ProbeEnum, (LPARAM)&w);
        if (!w) Sleep(10);
    }
    if (!w) {
        FileLog(L"LockApp: CoreWindow not found");
        return 0;
    }
    DWORD tid = GetWindowThreadProcessId(w, nullptr);
    g_probeStart = GetTickCount();
    g_ovSession = g_probeStart;    
    HHOOK hk = SetWindowsHookExW(WH_GETMESSAGE, ProbeMsgProc, nullptr, tid);
    g_probeHook = hk;
    if (!hk) {
        FileLog(L"LockApp: SetWindowsHookEx failed (error %lu)", GetLastError());
        return 0;
    }
    FileLog(L"LockApp: CoreWindow=%p thread=%lu, hook installed", w, tid);
    while (!g_stop && IsWindow(w)) {
        PostMessageW(w, WM_NULL, 0, 0);   // wake the UI thread so the hook runs
        Sleep(200);
    }
    HHOOK h = g_probeHook.exchange(nullptr);
    if (h) UnhookWindowsHookEx(h);
    FileLog(L"LockApp: probe finished");
    return 0;
}

static BOOL LockAppInit() {
    FileLog(L"LockApp: probe mode (reads the XAML tree, changes nothing)");
    g_probeThread = CreateThread(nullptr, 0, ProbeThread, nullptr, 0, nullptr);
    return TRUE;
}

BOOL Wh_ModInit() {
    g_modInitFn = &Wh_ModInit;    
    {
        wchar_t exe0[MAX_PATH] = L"";
        GetModuleFileNameW(nullptr, exe0, MAX_PATH);
        const wchar_t* b0 = wcsrchr(exe0, L'\\');
        g_lockAppRole = _wcsicmp(b0 ? b0 + 1 : exe0, L"LockApp.exe") == 0;
    }
    if (g_lockAppRole) {
        g_ovWanted = Wh_GetIntSetting(L"preview.showOverlay") != 0;
        if (Wh_GetModStoragePath(kCacheDir, MAX_PATH) != 0) {
            size_t n = wcslen(kCacheDir);
            while (n > 0 && kCacheDir[n - 1] == L'\\') kCacheDir[--n] = 0;
            wsprintfW(g_logPath, L"%s\\debug.log", kCacheDir);
            wsprintfW(kOverlayFile, L"%s\\overlay.bgra", kCacheDir);
            wsprintfW(kOverlayTmp, L"%s\\overlay.tmp", kCacheDir);
            g_debug = Wh_GetIntSetting(L"debugLog") != 0;
        } else {
            g_ovWanted = false;
        }
        FileLog(L"Overlay: LockApp side wanted=%d, file=%s", g_ovWanted ? 1 : 0, kOverlayFile);
        return LockAppInit();
    }
    if (Wh_GetModStoragePath(kCacheDir, MAX_PATH) == 0) {
        FileLog(L"Cannot get the mod storage folder, mod stopped");
        return FALSE;
    }
    size_t dirLen = wcslen(kCacheDir);
    while (dirLen > 0 && kCacheDir[dirLen - 1] == L'\\') kCacheDir[--dirLen] = 0;
    wsprintfW(g_logPath, L"%s\\debug.log", kCacheDir);
    wsprintfW(kLastFile, L"%s\\last.bgra", kCacheDir);
    wsprintfW(kLastTmp, L"%s\\last.tmp", kCacheDir);
    wsprintfW(kOverlayFile, L"%s\\overlay.bgra", kCacheDir);
    wsprintfW(kOverlayTmp, L"%s\\overlay.tmp", kCacheDir);    
    g_debug = Wh_GetIntSetting(L"debugLog") != 0;
    FileLog(L"Storage: %s", kCacheDir);
    {
        wchar_t exe[MAX_PATH] = L"";
        GetModuleFileNameW(nullptr, exe, MAX_PATH);
        const wchar_t* base = wcsrchr(exe, L'\\');
        g_previewRole = _wcsicmp(base ? base + 1 : exe, L"explorer.exe") == 0;
    }
    if (g_previewRole) {
        g_prevAutoOpen = Wh_GetIntSetting(L"preview.enabled") != 0;
        g_prevTray = Wh_GetIntSetting(L"preview.trayIcon") != 0;
        g_prevTopmost = Wh_GetIntSetting(L"preview.alwaysOnTop") != 0;
        g_prevShowOverlay = Wh_GetIntSetting(L"preview.showOverlay") != 0;        
        g_prevHotkey = 0; g_prevHkMods = 0; g_prevHkVk = 0;
        PCWSTR hk = Wh_GetStringSetting(L"preview.hotkey");
        if (hk) {
            if (ParseHotkey(hk, g_prevHkMods, g_prevHkVk)) g_prevHotkey = 1;
            else if (hk[0]) FileLog(L"Preview: hotkey \"%s\" is not valid (use for example Ctrl+Alt+P), hotkey disabled", hk);
            Wh_FreeStringSetting(hk);
        }
        // Nothing requested: stay completely idle inside explorer.exe.
        if (!g_prevAutoOpen && !g_prevTray && g_prevHotkey == 0) return TRUE;
        FileLog(L"Preview: role active (open=%d tray=%d hotkey=%d)", g_prevAutoOpen ? 1 : 0,
               g_prevTray ? 1 : 0, g_prevHotkey);
    }    
    {
        FILETIME cr, ex, kr, us, nw;
        GetProcessTimes(GetCurrentProcess(), &cr, &ex, &kr, &us);
        GetSystemTimeAsFileTime(&nw);
        ULONGLONG a = (((ULONGLONG)nw.dwHighDateTime << 32) | nw.dwLowDateTime) -
                      (((ULONGLONG)cr.dwHighDateTime << 32) | cr.dwLowDateTime);
        DWORD sid = 0;
        ProcessIdToSessionId(GetCurrentProcessId(), &sid);
        FileLog(L"=== MOD INIT === LogonUI age=%llu ms, session=%lu", a / 10000, sid);
    }
    PCWSTR p = Wh_GetStringSetting(L"general.videoPath");
    std::wstring path = Clean(p ? p : L"");
    Wh_FreeStringSetting(p);
    int w = Wh_GetIntSetting(L"performance.width");
    g_pauseBatPct = ClampInt(Wh_GetIntSetting(L"power.lowBatteryPercent"), 0, 100);
    {
        g_lowBatOnAC = Wh_GetIntSetting(L"power.lowBatteryOnAC") != 0;
        PCWSTR ll = Wh_GetStringSetting(L"power.lowBatteryLevel");
        g_lowBatLevel = 2;
        if (ll) {
            if (wcscmp(ll, L"off") == 0) g_lowBatLevel = 0;
            else if (wcscmp(ll, L"light") == 0) g_lowBatLevel = 1;
            else if (wcscmp(ll, L"maximum") == 0) g_lowBatLevel = 3;
            Wh_FreeStringSetting(ll);
        }
        // A new LogonUI starts on every lock, so check the battery right now.
        SYSTEM_POWER_STATUS sps0;
        if (g_pauseBatPct > 0 && (g_lowBatLevel == 1 || g_lowBatLevel == 2) &&
            GetSystemPowerStatus(&sps0) && (g_lowBatOnAC || sps0.ACLineStatus == 0) &&
            sps0.BatteryLifePercent != 255 && sps0.BatteryLifePercent <= g_pauseBatPct)
            g_lowBat = true;
    }
    if (g_lowBat && g_lowBatLevel == 2 && w > 960) w = 960;
    FileLog(L"Low battery: level=%d (0 off,1 light,2 balanced,3 maximum) threshold=%d%% startupLow=%d",
           g_lowBatLevel, g_pauseBatPct, g_lowBat.load() ? 1 : 0);
    if (g_previewRole && w > 960) w = 960;   // the preview window is small, no need for a big canvas
    if (w < 320) w = 320;
    if (w > 1920) w = 1920;
    kW = (w / 2) * 2;
    g_screenW = GetSystemMetrics(SM_CXSCREEN);
    g_screenH = GetSystemMetrics(SM_CYSCREEN);
    if (g_screenW < 320 || g_screenH < 200) { g_screenW = 1920; g_screenH = 1080; }
    kH = (int)((long long)kW * g_screenH / g_screenW) / 2 * 2;
    if (kH < 180) kH = 180;
    if (kH > 4096) kH = 4096;
    g_frameBytes = (size_t)kW * kH * 4;
    LoadColorSettings();
    {
        PCWSTR fm = Wh_GetStringSetting(L"general.fitMode");
        g_fitMode = kFitCover;
        if (fm) {
            if (wcscmp(fm, L"fill") == 0) g_fitMode = kFitFill;
            else if (wcscmp(fm, L"fit") == 0) g_fitMode = kFitFit;
            else if (wcscmp(fm, L"center") == 0) g_fitMode = kFitCenter;
            Wh_FreeStringSetting(fm);
        }
    }
    g_audioVol = ClampInt(Wh_GetIntSetting(L"general.audioVolume"), 0, 100);
    g_audioWanted = g_audioVol > 0;
    g_idleMs = (DWORD)ClampInt(Wh_GetIntSetting(L"power.idlePauseSeconds"), 0, 86400) * 1000u;
    g_hideOnShutdown = Wh_GetIntSetting(L"screens.showOnShutdown") == 0;
    g_playOnPassword = Wh_GetIntSetting(L"screens.playOnPasswordScreen") != 0;
    g_hwDecode = Wh_GetIntSetting(L"performance.hwDecode") != 0;
    // low battery settings are read near the top of this function (before the render width)
    {
        PCWSTR fo = Wh_GetStringSetting(L"general.folderOrder");
        g_folderOrder = 0;
        if (fo) {
            if (wcscmp(fo, L"sequential") == 0) g_folderOrder = 1;
            else if (wcscmp(fo, L"fixed") == 0) g_folderOrder = 2;
            Wh_FreeStringSetting(fo);
        }
    }    
    FileLog(L"Fit=%d audio=%d vol=%d idle=%lu ms hideOnShutdown=%d", g_fitMode,
           g_audioWanted ? 1 : 0, g_audioVol, g_idleMs, g_hideOnShutdown ? 1 : 0);

    g_initTick = GetTickCount();
    FileLog(L"Fail-safe: %s, timeout %lu ms", g_failSafe ? L"ON" : L"OFF", g_failSafeMs);

    g_paused = Wh_GetIntSetting(L"power.paused") != 0;
    g_debug = Wh_GetIntSetting(L"debugLog") != 0;
    {
        PCWSTR bm = Wh_GetStringSetting(L"screens.bootMode");
        bool still = !bm || wcscmp(bm, L"play") != 0;
        Wh_FreeStringSetting(bm);
        g_bootStillMs = (DWORD)ClampInt(Wh_GetIntSetting(L"screens.bootStillSeconds"), 10, 1800) * 1000u;
        g_bootStill = still && GetTickCount64() < (ULONGLONG)g_bootStillMs;
        g_saveLast = still;
        FileLog(L"Uptime %llu s, bootMode=%s", GetTickCount64() / 1000,
               still ? L"still" : L"play");
    }

    if (g_previewRole) { g_bootStill = false; g_saveLast = false; }
    if (path.empty()) {
        FileLog(L"videoPath is empty. Set it in the mod settings.");
        return TRUE;
    }
    g_videoFile = PickVideo(path);
    if (g_videoFile.empty()) return TRUE;
    {
        size_t dot = g_videoFile.find_last_of(L'.');
        std::wstring ext = (dot == std::wstring::npos) ? L"" : g_videoFile.substr(dot + 1);
        g_isGif = _wcsicmp(ext.c_str(), L"gif") == 0;
    }    
    FileLog(L"Video: %s (render %dx%d)%s", g_videoFile.c_str(), kW, kH,
           g_paused ? L" [PAUSE]" : L"");

    static bool s_csReady = false;   // ModInit can run again for the in-place preview reload
    if (!s_csReady) { InitializeCriticalSection(&g_cs); s_csReady = true; }
    g_shared.assign(g_frameBytes, 0);
    BuildCachePath();
    g_cacheHit = LoadCache();
    if (g_cacheHit) ApplyColor((uint32_t*)g_shared.data(), (size_t)kW * kH);    
    g_needSave = !g_cacheHit;
    if (g_previewRole) g_needSave = false;    
    FileLog(L"First-frame cache: %s", g_cacheHit ? L"HIT (used directly)"
                                                   : L"none yet");
    if (g_bootStill) {
        if (LoadLast()) {
            g_cacheHit = true;
            g_needSave = false;
            FileLog(L"Boot: showing last frame, decoder not started");
        } else {
            g_bootStill = false;
            FileLog(L"Boot: no saved last frame, playing normally");
        }
    }
    if (!(g_paused && g_cacheHit) && !g_bootStill && !g_previewRole) {
        uint64_t fp = Fnv(path + L"|" + std::to_wstring(kW) + L"|" +
                          std::to_wstring(g_fitMode) + L"|" +
                          std::to_wstring(g_hwDecode ? 1 : 0));
        int key = (int)(fp ^ (fp >> 32));
        if (Wh_GetIntValue(L"pendingKey", 0) != key) {
            Wh_SetIntValue(L"pendingKey", key);
            Wh_SetIntValue(L"pendingStarts", 0);
        }
        int pending = Wh_GetIntValue(L"pendingStarts", 0);
        if (pending >= 2) {
            FileLog(L"Two LogonUI sessions in a row ended before the decoder produced a frame; playback disabled. Change the video path, render width, fit mode or hardware decoding to try again.");
            return TRUE;
        }
        Wh_SetIntValue(L"pendingStarts", pending + 1);
        g_audioDone = !(g_audioWanted && !g_paused && g_speed == 100);
    }
    if (g_previewRole) {
        // The decoder starts lazily the first time the preview window is shown.
        if (!g_prevThread) g_prevThread = CreateThread(nullptr, 0, PreviewThread, nullptr, 0, &g_prevTid);
        return TRUE;
    }
    // Pause mode with a cache hit: no decoder needed at all
    if (!(g_paused && g_cacheHit) && !g_bootStill)
        g_decThread = CreateThread(nullptr, 0, DecodeThread, nullptr, 0, nullptr);
    // Audio only while the video really plays at normal speed.
    if (g_audioWanted && g_decThread && !g_paused && g_speed == 100 && !g_isGif)
        g_audioThread = CreateThread(nullptr, 0, AudioThread, nullptr, 0, nullptr);
    else if (g_audioWanted)
        FileLog(L"Audio not started (needs 100%% speed, no pause / boot still frame)");
    if (!g_audioThread) { g_audioDone = true; TryClearPending(); }
    g_thread = CreateThread(nullptr, 0, Worker, nullptr, 0, nullptr);
    return TRUE;
}

void Wh_ModUninit() {
    FileLog(L"=== MOD UNINIT ===");
    if (g_lockAppRole) {
        g_stop = true;
        if (g_probeThread) {
            WaitForSingleObject(g_probeThread, INFINITE);
            CloseHandle(g_probeThread);
            g_probeThread = nullptr;
        }
        Sleep(100);   // let a hook call that is still running on the UI thread finish
    }    
    if (g_previewRole && g_prevThread) {
        g_stop = true;
        for (int i = 0; i < 50 && !PostThreadMessageW(g_prevTid, WM_QUIT, 0, 0); i++) Sleep(10);
        WaitForSingleObject(g_prevThread, INFINITE);
        CloseHandle(g_prevThread);
        g_prevThread = nullptr;
    }    
    // 1) Ask the UI thread to restore the original images and release the XAML objects
    //    (if the fail-safe already ran, g_tornDown is true and this loop is skipped)
    g_teardown = true;
    HWND w = g_wnd;
    for (int i = 0; i < 60 && !g_tornDown && w && IsWindow(w); i++) {
        PostMessageW(w, WM_NULL, 0, 0);
        Sleep(50);
    }
    // 2) Stop the threads
    g_stop = true;
    if (g_thread) {
        if (WaitForSingleObject(g_thread, INFINITE) != WAIT_OBJECT_0) {
            FileLog(L"Worker did not stop in time");
        }
        CloseHandle(g_thread);
        g_thread = nullptr;
    }
    if (g_audioThread) {
        if (WaitForSingleObject(g_audioThread, INFINITE) != WAIT_OBJECT_0) {
            FileLog(L"Audio thread did not stop in time");
        }
        CloseHandle(g_audioThread);
        g_audioThread = nullptr;
    }
    if (g_decThread) {
        if (WaitForSingleObject(g_decThread, INFINITE) != WAIT_OBJECT_0) {
            FileLog(L"Decoder did not stop in time");
        }
        CloseHandle(g_decThread);
        g_decThread = nullptr;
    }
    // 3) Remove the hook (safe to call twice; the Worker may already have removed it)
    UnhookOnce();
    if (w && IsWindow(w)) SendMessageW(w, WM_NULL, 0, 0);  // when this returns, no hook call is still running
}

BOOL Wh_ModSettingsChanged(BOOL* bReload) {
    *bReload = TRUE;
    if (g_previewRole && g_prevThread && g_prevCtl) {
        // If the preview options themselves did not change, apply the rest in place and keep the window.
        bool open = Wh_GetIntSetting(L"preview.enabled") != 0;
        bool tray = Wh_GetIntSetting(L"preview.trayIcon") != 0;
        bool top = Wh_GetIntSetting(L"preview.alwaysOnTop") != 0;
        int hk = 0;
        UINT hkMods = 0, hkVk = 0;
        PCWSTR s = Wh_GetStringSetting(L"preview.hotkey");
        if (s) {
            if (ParseHotkey(s, hkMods, hkVk)) hk = 1;
            Wh_FreeStringSetting(s);
        }
        if (open == g_prevAutoOpen && tray == g_prevTray && top == g_prevTopmost && hk == g_prevHotkey &&
            hkMods == g_prevHkMods && hkVk == g_prevHkVk) {
            DWORD_PTR res = 0;
            if (SendMessageTimeoutW(g_prevCtl, kReloadMsg, 0, 0, SMTO_ABORTIFHUNG, 15000, &res) && res == 1)
                *bReload = FALSE;   // handled in place, the window stays
        }
    }    
    return TRUE;
}
