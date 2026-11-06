// ==WindhawkMod==
// @id                lock-screen-live-wallpaper
// @name              Lock Screen Live Wallpaper
// @description       Play a video as the lock screen and password screen background
// @description:id-ID Putar video sebagai latar belakang lock screen dan layar password
// @version           1.0.0
// @author            grn15RenS
// @github            https://github.com/grn15RenS
// @donateUrl         https://ko-fi.com/grn15rens
// @license           GPL-3.0
// @include           LogonUI.exe
// @architecture      x86-64
// @compilerOptions   -lole32 -loleaut32 -lruntimeobject -lmfplat -lmfreadwrite -lmfuuid -lwinmm -ladvapi32 -luuid
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Lock Screen Live Wallpaper

Plays a video as the background of the Windows 11 **lock screen** and the **password / sign-in screen**.

![preview](https://i.imgur.com/cCjmOzL.gif)

*Preview video: "Stelle Cosmic Cheerleader" by u/iTaiizor, [source](https://www.reddit.com/r/iSucrose/comments/1xirh99/stelle_cosmic_cheerleader/). Character © HoYoverse. The video is not included in this mod.*

The mod runs inside `LogonUI.exe`, decodes your video with Media Foundation, and feeds the frames into the XAML image elements that Windows normally uses for the lock screen wallpaper. If something looks wrong, a built-in fail-safe restores the original wallpaper.

## ⚠️ Before you start ⚠️

`LogonUI.exe` is a protected system process, so Windhawk does not inject into it by default. For this mod to work you must add it to Windhawk's process inclusion list:

1. Open Windhawk and go to **Settings** → **Advanced settings** → **More advanced settings**.
2. Add `LogonUI.exe` to the **Process inclusion list** and save.

![Windhawk advanced settings: process inclusion list](https://i.imgur.com/9Biip4G.png)

## 📋 Requirements

- Windows 11
- `LogonUI.exe` added to Windhawk's process inclusion list (see above)
- A video file that the **SYSTEM** account can read. `LogonUI.exe` runs as SYSTEM, so network locations, encrypted folders, and cloud-only (not downloaded) files may not work.

## ⚙️ Features

| Feature | Details |
| --- | --- |
| Video source | One video file, or a folder (a random video is picked on every lock) |
| Formats | `mp4`, `m4v`, `mov`, `wmv`, `avi`, `mkv` (anything Media Foundation can decode on your system; H.264 MP4 is recommended) |
| Fit modes | Cover, fill, fit (letterbox), center |
| Playback speed | 25% to 400% |
| Color adjustments | Hue, saturation, brightness, contrast, gamma |
| Audio | Plays the video's audio track when the volume is above 0 |
| Power saving | Still-frame mode, pause when idle, and optional pause on the password screen |
| Boot behavior | Optionally show the last frame right after boot instead of decoding |
| Shutdown screen | Optionally keep the original background on the "Shutting down" / "Restarting" screen |
| Fail-safe | Restores the original wallpaper if the lock screen UI changes or the mod misbehaves |
| Debug log | Optional CPU and decode statistics and a UI tree dump |

## 📝 Notes on some settings

- **Render width**: internal canvas width (320 to 1920). The height follows your screen's aspect ratio. This is the biggest CPU lever.
- **Behavior right after boot**: with `still`, the first 3 minutes after boot show the last saved frame without decoding. Needs at least one earlier session where the video was playing.
- **Audio volume**: 0 means muted. Above 0, audio only plays at 100% speed while the video is playing (not paused, idle-paused, or showing the boot still frame).
- **Pause when idle**: freezes the video and mutes the audio after the chosen time without input, and resumes on the next input.
- **Play video on the password screen**: on (default) keeps the video playing while you type your password. Off freezes it on the password screen, which can help if playback stutters there, and it plays again on the lock screen.
- **Debug log**: also writes `debug.log` in the mod's storage folder, with CPU and decode statistics every 5 seconds and one UI tree dump. Basic events always go to Windhawk's log (enable logging for the mod in Windhawk to see them).
- **Long playback**: the video loops without any time limit. By default it freezes after 5 minutes without input (see **Pause when idle**); set that to 0 if you want it to keep playing while nobody is at the PC.

## 📈 Performance tips

- Lower **Render width** (the default is 1920; for example try 1280 or 960).
- Videos at 4K or 60 fps are decoded on the CPU and can use several cores. For lower CPU use, prefer 1080p at 30 fps.
- Use **Pause** if you only want a still image.
- Keep **Pause when idle** enabled so nothing is decoded while nobody is at the PC.
- If the video stutters while you type your password, turn off **Play video on the password screen**.
- The password screen blurs the wallpaper behind it. A constantly changing background makes Windows redo that blur every frame, which costs some GPU time.

## 🔧 Troubleshooting

- **Normal wallpaper is shown**: make sure `LogonUI.exe` is in the process inclusion list and the video path is correct, then turn on **Debug log** and read `debug.log` in the mod's storage folder (see "Files written by the mod"). Look for `Failed to open video`, `Unsupported video format`, or `FAIL-SAFE TRIPPED`. Enable **Debug log** for more detail.
- **Video will not open**: try an H.264 MP4 stored somewhere the SYSTEM account can read.
- **No sound**: set **Audio volume** above 0 and see the notes above.
- **Stuttering**: lower the render width and enable **Debug log**, then check the `CPU` and `UI stalls` lines in the log.

## 🤔 How it works

1. A worker thread finds the `LogonUI Logon Window` and installs a `WH_GETMESSAGE` hook on its UI thread.
2. A decoder thread reads the video as RGB32 through Media Foundation, scales each frame according to the fit mode, applies the color settings, and publishes it to a shared buffer.
3. On the UI thread, the newest frame is copied into a `WriteableBitmap`, which is set as the source of the `LockLogonImage` and `SwapLockLogonImage` elements.
4. When the mod is unloaded, the original image sources are restored.

## 🛡️ Fail-safe

The mod stops itself and restores the original wallpaper for the rest of that `LogonUI.exe` process when:

- the video decoder fails before producing any frame,
- no video frame is produced within 30 seconds,
- the lock screen elements (`LockLogonImage` / `SwapLockLogonImage`) are never found, which usually means a Windows update changed the UI, or
- the UI throws repeated errors (3 within 30 seconds).

Separately, a crash guard protects the sign-in screen. If two `LogonUI.exe` sessions in a row end before the decoder shows its first frame (for example because a codec crashes on your video), the mod turns playback off and the normal wallpaper is shown. To turn it back on, change **Video file or folder**, **Render width** or **Fit mode** to a different value and keep it. Very short sessions (for example two Ctrl+Alt+Del screens in a row) can also trip it; the same fix applies.

## 📁 Files written by the mod

Everything is stored in the mod's own storage folder inside Windhawk's data folder:

`C:\ProgramData\Windhawk\Engine\ModsWritable\mod-storage\lock-screen-live-wallpaper`

`ProgramData` is a hidden folder, so paste the path above into the File Explorer address bar to open it. The files are:

- `debug.log`: log file, trimmed automatically after 2 MB
- `frame_*.bgra`: cached first frame, so the lock screen can show something immediately
- `last.bgra`: last shown frame, used by the `still` boot mode

You can delete these files at any time; the mod recreates what it needs. The folder is removed automatically when the mod is uninstalled.

## ☕ Support

If you enjoy this mod, you can support its development:

- [Ko-fi](https://ko-fi.com/grn15rens)
- [Saweria](https://saweria.co/grn15RenS) (for Indonesia)

## ℹ️ Disclaimer

Only tested on Windows 11 23H2 and 25H2. This mod runs inside a system UI process (`LogonUI.exe`), so it includes a fail-safe that restores the original wallpaper if anything goes wrong. As a precaution, keep another way to sign in available while trying it out.

---

# Bahasa Indonesia

Memutar video sebagai latar belakang **lock screen** dan **layar password / sign-in** Windows 11.

![preview](https://i.imgur.com/cCjmOzL.gif)

*Pratinjau video: "Stelle Cosmic Cheerleader" oleh u/iTaiizor, [sumber](https://www.reddit.com/r/iSucrose/comments/1xirh99/stelle_cosmic_cheerleader/). Karakter © HoYoverse. Videonya tidak disertakan dalam mod ini.*

Mod ini berjalan di dalam `LogonUI.exe`, men-decode videomu dengan Media Foundation, lalu memasukkan frame-nya ke elemen gambar XAML yang biasanya dipakai Windows untuk wallpaper lock screen. Jika ada yang tidak beres, fail-safe bawaan akan mengembalikan wallpaper asli.

## ⚠️ Sebelum mulai ⚠️

`LogonUI.exe` adalah proses sistem yang dilindungi, jadi Windhawk tidak meng-inject ke dalamnya secara default. Agar mod ini berfungsi, kamu harus menambahkannya ke daftar inklusi proses Windhawk:

1. Buka Windhawk lalu pergi ke **Settings** → **Advanced settings** → **More advanced settings**.
2. Tambahkan `LogonUI.exe` ke **Process inclusion list** lalu simpan.

![pengaturan lanjutan](https://i.imgur.com/9Biip4G.png)

## 📋 Persyaratan

- Windows 11
- `LogonUI.exe` sudah ditambahkan ke daftar inklusi proses Windhawk (lihat di atas)
- File video yang bisa dibaca akun **SYSTEM**. `LogonUI.exe` berjalan sebagai SYSTEM, sehingga lokasi jaringan, folder terenkripsi, dan file cloud-only (belum diunduh) mungkin tidak berfungsi.

## ⚙️ Fitur

| Fitur | Keterangan |
| --- | --- |
| Sumber video | Satu file video, atau satu folder (video dipilih acak setiap kali dikunci) |
| Format | `mp4`, `m4v`, `mov`, `wmv`, `avi`, `mkv` (apa pun yang bisa di-decode Media Foundation di sistemmu; MP4 H.264 disarankan) |
| Mode fit | Cover, fill, fit (letterbox), center |
| Kecepatan putar | 25% sampai 400% |
| Penyesuaian warna | Hue, saturasi, kecerahan, kontras, gamma |
| Audio | Memutar audio video jika volume di atas 0 |
| Hemat daya | Mode gambar diam, jeda saat idle, dan opsi jeda di layar password |
| Perilaku saat boot | Opsional menampilkan frame terakhir tepat setelah boot tanpa decoding |
| Layar shutdown | Opsional mempertahankan latar asli di layar "Shutting down" / "Restarting" |
| Fail-safe | Mengembalikan wallpaper asli jika UI lock screen berubah atau mod bermasalah |
| Debug log | Opsional: statistik CPU dan decode serta dump UI tree |

## 📝 Catatan beberapa pengaturan

- **Render width**: lebar kanvas internal (320 sampai 1920). Tinggi mengikuti rasio aspek layarmu. Ini pengaturan yang paling berpengaruh pada CPU.
- **Behavior right after boot**: dengan `still`, 3 menit pertama setelah boot menampilkan frame terakhir yang tersimpan tanpa decoding. Butuh minimal satu sesi sebelumnya di mana video sempat diputar.
- **Audio volume**: 0 berarti senyap. Di atas 0, audio hanya diputar pada kecepatan 100% dan hanya saat video benar-benar berjalan (bukan saat dijeda, dijeda karena idle, atau menampilkan frame diam setelah boot).
- **Pause when idle**: membekukan video dan membisukan audio setelah waktu tertentu tanpa input, lalu lanjut saat ada input berikutnya.
- **Play video on the password screen**: aktif (default) berarti video tetap diputar saat kamu mengetik password. Jika dimatikan, video dibekukan di layar password (berguna jika video patah-patah di sana), dan diputar lagi di lock screen.
- **Debug log**: juga menulis `debug.log` di folder penyimpanan mod, berisi statistik CPU dan decode setiap 5 detik dan satu kali dump UI tree. Peristiwa dasar selalu masuk ke log Windhawk (aktifkan logging untuk mod ini di Windhawk untuk melihatnya).
- **Pemutaran lama**: video diulang tanpa batas waktu. Secara default video dibekukan setelah 5 menit tanpa input (lihat **Pause when idle**); setel ke 0 kalau kamu ingin video terus berjalan saat tidak ada orang di depan PC.

## 📈 Tips performa

- Turunkan **Render width** (defaultnya 1920; misalnya coba 1280 atau 960).
- Video 4K atau 60 fps di-decode oleh CPU dan bisa memakai beberapa core. Untuk pemakaian CPU lebih rendah, pilih 1080p dengan 30 fps.
- Gunakan **Pause** jika kamu hanya ingin gambar diam.
- Biarkan **Pause when idle** aktif agar tidak ada decoding saat tidak ada orang di depan PC.
- Jika video patah-patah saat mengetik password, matikan **Play video on the password screen**.
- Layar password mem-blur wallpaper di belakangnya. Latar yang terus berubah membuat Windows mengulang blur itu di setiap frame, yang memakan sebagian GPU.

## 🔧 Pemecahan masalah

- **Wallpaper normal yang tampil**: pastikan `LogonUI.exe` ada di daftar inklusi proses dan path video benar, lalu aktifkan **Debug log** dan baca `debug.log` di folder penyimpanan mod (lihat "File yang ditulis oleh mod"). Cari `Failed to open video`, `Unsupported video format`, atau `FAIL-SAFE TRIPPED`. Aktifkan **Debug log** untuk detail lebih banyak.
- **Video tidak mau terbuka**: coba MP4 H.264 yang disimpan di tempat yang bisa dibaca akun SYSTEM.
- **Tidak ada suara**: setel **Audio volume** di atas 0 dan baca catatan di atas.
- **Patah-patah**: turunkan render width dan aktifkan **Debug log**, lalu cek baris `CPU` dan `UI stalls` di log.

## 🤔 Cara kerja

1. Worker thread mencari `LogonUI Logon Window` dan memasang hook `WH_GETMESSAGE` pada UI thread-nya.
2. Decoder thread membaca video sebagai RGB32 lewat Media Foundation, mengubah ukuran tiap frame sesuai mode fit, menerapkan pengaturan warna, lalu menaruhnya di buffer bersama.
3. Di UI thread, frame terbaru disalin ke `WriteableBitmap`, yang dipasang sebagai source elemen `LockLogonImage` dan `SwapLockLogonImage`.
4. Saat mod di-unload, source gambar asli dikembalikan.

## 🛡️ Fail-safe

Mod berhenti sendiri dan mengembalikan wallpaper asli sampai proses `LogonUI.exe` itu berakhir, jika:

- decoder video gagal sebelum menghasilkan frame apa pun,
- tidak ada frame video yang dihasilkan dalam 30 detik,
- elemen lock screen (`LockLogonImage` / `SwapLockLogonImage`) tidak pernah ditemukan, yang biasanya berarti update Windows mengubah UI, atau
- UI melempar error berulang (3 kali dalam 30 detik).

Selain itu, ada pelindung crash untuk layar sign-in. Jika dua sesi `LogonUI.exe` berturut-turut berakhir sebelum decoder menampilkan frame pertama (misalnya karena codec crash pada videomu), mod mematikan pemutaran dan wallpaper normal yang tampil. Untuk menyalakannya lagi, ubah **Video file or folder**, **Render width** atau **Fit mode** ke nilai yang berbeda dan biarkan begitu. Sesi yang sangat singkat (misalnya dua kali layar Ctrl+Alt+Del berturut-turut) juga bisa memicunya; solusinya sama.

## 📁 File yang ditulis oleh mod

Semuanya disimpan di folder penyimpanan milik mod sendiri, di dalam folder data Windhawk:

`C:\ProgramData\Windhawk\Engine\ModsWritable\mod-storage\lock-screen-live-wallpaper`

`ProgramData` adalah folder tersembunyi, jadi tempel path di atas ke address bar File Explorer untuk membukanya. Isinya:

- `debug.log`: file log, dipangkas otomatis setelah 2 MB
- `frame_*.bgra`: cache frame pertama, agar lock screen bisa langsung menampilkan sesuatu
- `last.bgra`: frame terakhir yang ditampilkan, dipakai oleh mode boot `still`

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
- videoPath: ""
  $name: Video file or folder
  $description: Full path to a video file (mp4 recommended), or a folder with videos (one is picked at random each lock).
- width: 1920
  $name: Render width
  $description: Internal width in pixels (the height follows your screen's aspect ratio). Lower = less CPU, higher = sharper.
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
  $description: 0 to 100. 0 = muted (default). Above 0 plays the audio track of the video, only at 100% video speed while the video is playing (not paused, no boot still frame). Scales the mod's own audio before it reaches Windows, so it is separate from other apps. The system master volume still applies on top.
- speed: 100
  $name: Video speed (%)
  $description: 25 to 400. 100 = normal speed.
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
  $name: Color adjustments
- power:
  - paused: false
    $name: Pause (show a still frame)
    $description: Show the first frame of the video as a still image instead of playing it. Almost no CPU usage.
  - idlePauseSeconds: 300
    $name: Pause when idle (seconds)
    $description: Freeze the video and mute the audio after this many seconds without keyboard or mouse input. It resumes on the next input. 0 = never.
  - playOnPasswordScreen: true
    $name: Play video on the password screen
    $description: On = the video keeps playing while you type your password. Off = the video freezes on the password screen, which can help if playback stutters there. It plays again on the lock screen.
  - bootMode: still
    $name: Behavior right after boot
    $description: "still = for the first 3 minutes after boot, show the last frame without decoding. play = play immediately."
    $options:
    - still: Show last frame
    - play: Play video
  $name: Pause and power saving
- hideOnShutdown: false
  $name: Hide video on shutdown / restart screen
  $description: Do not show the video on the "Shutting down" / "Restarting" status screen. The original Windows background is shown instead.
- debugLog: false
  $name: Debug log
  $description: Also write debug.log in the mod's storage folder, with CPU and decode statistics every 5 seconds and a one-time dump of the lock screen UI tree.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#undef GetCurrentTime
#include <timeapi.h>

#include <mfapi.h>
#include <mfidl.h>
#include <mfreadwrite.h>
#include <mmreg.h>
#include <mmdeviceapi.h>
#include <audioclient.h>

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
static std::atomic<bool> g_attached{false};
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
static bool g_pwScene = false;                 // UI thread only
static DWORD g_pwGoneTick = 0;                 // UI thread only: when the password screen last closed
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

int ClampInt(int v, int lo, int hi) { return v < lo ? lo : (v > hi ? hi : v); }

void LoadColorSettings() {
    g_speed = ClampInt(Wh_GetIntSetting(L"speed"), 25, 400);
    g_hue = ClampInt(Wh_GetIntSetting(L"colorAdjustments.hue"), -180, 180);    
    g_sat = ClampInt(Wh_GetIntSetting(L"colorAdjustments.saturation"), 0, 300);
    g_bri = ClampInt(Wh_GetIntSetting(L"colorAdjustments.brightness"), -100, 100);
    g_con = ClampInt(Wh_GetIntSetting(L"colorAdjustments.contrast"), 25, 300);
    g_gam = ClampInt(Wh_GetIntSetting(L"colorAdjustments.gamma"), 20, 300);

    g_identity = (g_hue == 0 && g_sat == 100 && g_bri == 0 && g_con == 100 &&
                  g_gam == 100);

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
           e == L"avi" || e == L"mkv";
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
    std::wstring key = g_videoFile + L"|" + std::to_wstring(kW) + L"|" +
        std::to_wstring(g_hue) + L"|" + std::to_wstring(g_sat) + L"|" +
        std::to_wstring(g_bri) + L"|" + std::to_wstring(g_con) + L"|" +
        std::to_wstring(g_gam) + L"|" +
        std::to_wstring(kH) + L"|" + std::to_wstring(g_fitMode);
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

void PruneCache() {
    std::wstring pat = std::wstring(kCacheDir) + L"\\frame_*.bgra";
    WIN32_FIND_DATAW fd;
    HANDLE h = FindFirstFileW(pat.c_str(), &fd);
    if (h == INVALID_HANDLE_VALUE) return;
    std::vector<std::wstring> v;
    do { v.push_back(std::wstring(kCacheDir) + L"\\" + fd.cFileName); }
    while (FindNextFileW(h, &fd));
    FindClose(h);
    if (v.size() <= 6) return;
    for (auto& f : v) if (f != g_cacheFile) DeleteFileW(f.c_str());
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
        if (g_identity) {
            for (int x = 0; x < kW; x++)
                dst[x] = xmap[x] < 0 ? kBlack : (src[xmap[x]] | kBlack);
        } else {
            for (int x = 0; x < kW; x++)
                dst[x] = xmap[x] < 0 ? kBlack : Fx(src[xmap[x]]);
        }
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
static std::atomic<bool> g_frameDone{false}, g_audioDone{true}, g_pendingCleared{false};
static void TryClearPending() {
    if (g_frameDone && g_audioDone && !g_pendingCleared.exchange(true))
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
        while (!g_stop && !g_decoderDone) {
            // Playback hold (idle or hidden): stop the stream and wait.
            if (g_idlePaused.load()) {
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
                            b->Unlock();
                        }
                        b->Release();
                    }
                    s->Release();
                }
            }
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
                for (UINT32 i = 0; i < n * ch; i++) o[i] = in[i] * vol;
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
// ---------- Decoder (separate thread) ----------
static DWORD DecodeThreadImpl() {
    // Whatever the reason the decoder exits (file won't open, unsupported codec, etc.),
    // if no frame was ever produced, tell the UI thread so the original wallpaper
    // is not replaced by a black bitmap and the fail-safe can trip.
    struct ExitGuard {
        ~ExitGuard() { if (g_seq == 0) g_decodeDead = true; g_decoderDone = true; Wh_SetIntValue(L"pendingStarts", 0); }
    } exitGuard;

    HRESULT hrCo = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    struct CoGuard {
        bool on;
        ~CoGuard() { if (on) CoUninitialize(); }
    } coGuard{ SUCCEEDED(hrCo) };
    timeBeginPeriod(1);
    if (FAILED(MFStartup(MF_VERSION))) {
        FileLog(L"MFStartup failed");
        timeEndPeriod(1);
        return 1;
    }
    IMFAttributes* at = nullptr;
    MFCreateAttributes(&at, 1);
    if (at) at->SetUINT32(MF_SOURCE_READER_ENABLE_ADVANCED_VIDEO_PROCESSING, TRUE);

    IMFSourceReader* rd = nullptr;
    HRESULT hr = MFCreateSourceReaderFromURL(g_videoFile.c_str(), at, &rd);
    if (at) at->Release();
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
    mt->SetGUID(MF_MT_SUBTYPE, MFVideoFormat_RGB32);
    {
        // Let Media Foundation scale big videos down: converting and copying
        // a 4K frame as RGB32 on the CPU costs far more than at canvas size.
        IMFMediaType* nat = nullptr;
        UINT32 nw = 0, nh = 0;
        if (SUCCEEDED(rd->GetNativeMediaType(VS, 0, &nat)) && nat) {
            MFGetAttributeSize(nat, MF_MT_FRAME_SIZE, &nw, &nh);
            nat->Release();
        }
        if (nw > 0 && nh > 0) {
            float sc;
            switch (g_fitMode) {
            case kFitFit:    sc = std::min((float)kW / nw, (float)kH / nh); break;
            case kFitCenter: sc = (float)kW / (float)g_screenW; break;
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
            cur->Release();
        }
    };
    readType();
    FileLog(L"Video: %ux%u stride=%ld", sw, sh, stride);
    visH = sh;    
    if (!sw || !sh) { rd->Release(); MFShutdown(); timeEndPeriod(1); return 1; }

    std::vector<uint8_t> tmp(g_frameBytes);
    std::vector<int> xmap(kW);

    ULONGLONG startTick = GetTickCount64();
    ULONGLONG t0 = startTick;
    LONG seqAtRewind = -1;
    bool firstPublished = false, savedOnce = false;    
    while (!g_stop) {
        // Playback hold (idle or hidden): park without decoding. On resume the
        // pacing code below re-syncs t0 by itself because the first frame is late.
        if (g_idlePaused.load()) { Sleep(100); continue; }        
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
        // 1) Convert into a temporary buffer first
        bool have = false;
        IMFMediaBuffer* buf = nullptr;
        if (SUCCEEDED(sample->ConvertToContiguousBuffer(&buf)) && buf) {
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
        if (!have) continue;

        double dec = NowMs() - tA;
        g_decMsSum += dec;
        g_readMsSum += tRead;        
        g_decN++;
        if (dec > g_decMsMax) g_decMsMax = dec;

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
        if (!firstPublished) { firstPublished = true; g_frameDone = true; TryClearPending(); }

        if (g_needSave) {
            g_needSave = false;
            SaveCache(tmp.data());
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

DWORD WINAPI DecodeThread(LPVOID) {
    try { return DecodeThreadImpl(); }
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
        if (g_pwScene && !pw) g_pwGoneTick = GetTickCount();
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

void DumpTree(DependencyObject const& o, int d, int& c) {
    if (c > 300 || d > 40) return;
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
        DWORD interval = both ? (light ? 250u : 1000u) : (now - g_initTick < 5000 ? 30u : 250u);
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
        bool hold = idle || hideNow || (!g_playOnPassword && g_pwScene);
        if (hold != g_idlePaused.load()) {
            g_idlePaused = hold;
            FileLog(L"Playback hold %s (idle=%d hidden=%d)", hold ? L"ON" : L"OFF",
                   idle ? 1 : 0, hideNow ? 1 : 0);
        }

        bool ok = true;
        if (g_imgA && !hideNow) ok = Attach(g_imgA, true) && ok;
        if (g_imgB && !hideNow) ok = Attach(g_imgB, false) && ok;
        if (!ok) OnStepError(L"Attach failed: Source did not change");

        g_attached = (g_imgA && g_imgB) ? true : false;
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
        Sleep(200);
    }
    // Exiting because of fail-safe / uninit / window gone: remove the hook so
    // the UI thread has no more overhead.
    UnhookOnce();
    g_wnd = nullptr;
    FileLog(L"Worker finished");
    return 0;
}

BOOL Wh_ModInit() {
    if (Wh_GetModStoragePath(kCacheDir, MAX_PATH) == 0) {
        FileLog(L"Cannot get the mod storage folder, mod stopped");
        return FALSE;
    }
    size_t dirLen = wcslen(kCacheDir);
    while (dirLen > 0 && kCacheDir[dirLen - 1] == L'\\') kCacheDir[--dirLen] = 0;
    wsprintfW(g_logPath, L"%s\\debug.log", kCacheDir);
    wsprintfW(kLastFile, L"%s\\last.bgra", kCacheDir);
    wsprintfW(kLastTmp, L"%s\\last.tmp", kCacheDir);
    g_debug = Wh_GetIntSetting(L"debugLog") != 0;    
    FileLog(L"Storage: %s", kCacheDir);    
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
    PCWSTR p = Wh_GetStringSetting(L"videoPath");
    std::wstring path = Clean(p ? p : L"");
    Wh_FreeStringSetting(p);
    int w = Wh_GetIntSetting(L"width");
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
        PCWSTR fm = Wh_GetStringSetting(L"fitMode");
        g_fitMode = kFitCover;
        if (fm) {
            if (wcscmp(fm, L"fill") == 0) g_fitMode = kFitFill;
            else if (wcscmp(fm, L"fit") == 0) g_fitMode = kFitFit;
            else if (wcscmp(fm, L"center") == 0) g_fitMode = kFitCenter;
            Wh_FreeStringSetting(fm);
        }
    }
    g_audioVol = ClampInt(Wh_GetIntSetting(L"audioVolume"), 0, 100);
    g_audioWanted = g_audioVol > 0;    
    g_idleMs = (DWORD)ClampInt(Wh_GetIntSetting(L"power.idlePauseSeconds"), 0, 86400) * 1000u;
    g_hideOnShutdown = Wh_GetIntSetting(L"hideOnShutdown") != 0;
    g_playOnPassword = Wh_GetIntSetting(L"power.playOnPasswordScreen") != 0;
    FileLog(L"Fit=%d audio=%d vol=%d idle=%lu ms hideOnShutdown=%d", g_fitMode,
           g_audioWanted ? 1 : 0, g_audioVol, g_idleMs, g_hideOnShutdown ? 1 : 0);    

    g_initTick = GetTickCount();
    FileLog(L"Fail-safe: %s, timeout %lu ms", g_failSafe ? L"ON" : L"OFF", g_failSafeMs);

    g_paused = Wh_GetIntSetting(L"power.paused") != 0;
    g_debug = Wh_GetIntSetting(L"debugLog") != 0;
    {
        PCWSTR bm = Wh_GetStringSetting(L"power.bootMode");
        bool still = !bm || wcscmp(bm, L"play") != 0;
        Wh_FreeStringSetting(bm);
        g_bootStill = still && GetTickCount64() < 180000ULL;
        g_saveLast = still;        
        FileLog(L"Uptime %llu s, bootMode=%s", GetTickCount64() / 1000,
               still ? L"still" : L"play");
    }

    if (path.empty()) {
        FileLog(L"videoPath is empty. Set it in the mod settings.");
        return TRUE;
    }
    g_videoFile = PickVideo(path);
    if (g_videoFile.empty()) return TRUE;
    FileLog(L"Video: %s (render %dx%d)%s", g_videoFile.c_str(), kW, kH,
           g_paused ? L" [PAUSE]" : L"");

    InitializeCriticalSection(&g_cs);
    g_shared.assign(g_frameBytes, 0);
    BuildCachePath();
    g_cacheHit = LoadCache();
    g_needSave = !g_cacheHit;
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
    if (!(g_paused && g_cacheHit) && !g_bootStill) {
        uint64_t fp = Fnv(path + L"|" + std::to_wstring(kW) + L"|" +
                          std::to_wstring(g_fitMode));
        int key = (int)(fp ^ (fp >> 32));
        if (Wh_GetIntValue(L"pendingKey", 0) != key) {
            Wh_SetIntValue(L"pendingKey", key);
            Wh_SetIntValue(L"pendingStarts", 0);
        }        
        int pending = Wh_GetIntValue(L"pendingStarts", 0);
        if (pending >= 2) {
            FileLog(L"Two LogonUI sessions in a row ended before the decoder produced a frame; playback disabled. Change the video path, render width or fit mode to try again.");
            return TRUE;
        }
        Wh_SetIntValue(L"pendingStarts", pending + 1);
        g_audioDone = !(g_audioWanted && !g_paused && g_speed == 100);        
    }
    // Pause mode with a cache hit: no decoder needed at all
    if (!(g_paused && g_cacheHit) && !g_bootStill)
        g_decThread = CreateThread(nullptr, 0, DecodeThread, nullptr, 0, nullptr);
    // Audio only while the video really plays at normal speed.
    if (g_audioWanted && g_decThread && !g_paused && g_speed == 100)
        g_audioThread = CreateThread(nullptr, 0, AudioThread, nullptr, 0, nullptr);
    else if (g_audioWanted)
        FileLog(L"Audio not started (needs 100%% speed, no pause / boot still frame)");
    if (!g_audioThread) { g_audioDone = true; TryClearPending(); }
    g_thread = CreateThread(nullptr, 0, Worker, nullptr, 0, nullptr);
    return TRUE;
}

void Wh_ModUninit() {
    FileLog(L"=== MOD UNINIT ===");
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
    return TRUE;
}
