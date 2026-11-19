// ==WindhawkMod==
// @id              taskbar-timer-alarm
// @name            Taskbar Timer & Alarm
// @description     A countdown timer and alarm clock integrated into the Windows 11 taskbar (Windows 11 only)
// @version         1.0
// @author          yazanhani900-blip
// @github          https://github.com/yazanhani900-blip
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lruntimeobject -lversion -ldwmapi -lgdi32 -luxtheme -lwinmm -lcomdlg32 -lshlwapi -lwindowscodecs -luuid -lshell32
// @license         GPL-3.0
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Taskbar Timer & Alarm

Adds a compact countdown timer **and alarm clock** directly to the Windows 11 taskbar.

## Features

- **Two modes in one flyout: Timer and Alarm**
  - Timer: set a countdown in **hours, minutes and seconds**
  - Alarm: a Samsung-style **wheel picker** with **hour (1-12), minute and AM / PM**.
    Roll the **mouse wheel** over a column (or click its arrows) to change it.
    It rings at the next occurrence of that time (today if it is still ahead,
    otherwise tomorrow)
- Add a custom reminder message
- Live countdown display in the taskbar (compact button, adjustable font size).
  A timer shows a stopwatch icon, an alarm shows an alarm-clock icon.
- Cancel an active timer / alarm
- **"Time's up" / "Alarm" alert in two sizes**: a large full-screen screen, or a small
  notification in the bottom-right corner
- **Settings page** (the gear button in the flyout): sound, background,
  Dark / Light / Auto look and alert size
- **Frosted-glass card**: the card is translucent and blurs the background behind it
- **Glass style**: the flyout follows Dark / Light / Auto, shows your background image and can be solid or frosted glass; with a video background the alert card is blurred too
- **Custom background for the alert: an image or a looping video**
  (chosen from the flyout, with a Preview button)
- Choose any sound file for the completion alert
- Timer / alarm state survives Explorer restarts and mod reloads
- Missed reminders older than 6 hours are discarded on restore
- Completion card is frosted glass with a **Dark / Light / Auto** look (set it on the settings page; Auto follows the Windows app theme)

## How to use

1. Click the button on the taskbar.
2. Choose **Timer** or **Alarm** at the top of the flyout.
3. Enter a reminder and a duration (Timer) or roll the wheels to a time (Alarm).
4. Optionally open the gear (settings) page and click **Choose sound…** and/or **Choose background…**
   (images: jpg, png, bmp, gif, tif - videos: mp4, mov, wmv, avi, mkv, webm).
5. Open the gear (settings) page and use **Preview alert** to see it right away.
6. Click **Start** (Timer) or **Set alarm** (Alarm). When it is time, the alert appears.
   Press **Esc** or click **Dismiss** to close it. **Snooze** starts a new timer.

Click **Default background** to go back to the built-in gradient.
Videos are muted by default (see settings) because the completion sound plays too.

## Notes

- Only one timer / alarm can run at a time.
- Alarm wheels: scroll down = next value, scroll up = previous value.
- Alert size (large / small) and look are set on the settings page of the flyout; the glass tint is in the mod settings.
- With a video background in the large alert the card stays solid (a live video
  can't be blurred). The small alert always uses an image or the default gradient.

## Credits and license

Based on the "Taskbar Countdown Timer" mod by K3M. This version re-organises the internals (classes with
clear ownership instead of loose globals) while keeping the same interface and
behaviour. It is a derivative work and stays under **GPL-3.0**.
The taskbar / XAML integration technique is adapted from Windhawk's
taskbar-multirow and taskbar-notification-icon-spacing mods by m417z (GPL-3.0).
*/
// ==/WindhawkModReadme==


// ==WindhawkModSettings==
/*
- defaultMinutes: 20
  $name: Default timer length (minutes)
- defaultSnoozeMinutes: 5
  $name: Default snooze length (minutes)
- maximumMinutes: 1440
  $name: Maximum timer length (minutes)
- backgroundDim: 40
  $name: Background dimming for image backgrounds (0-80 %)
- glassTint: 35
  $name: Frosted glass tint strength (0-90 %)
- buttonFontSize: 11
  $name: Taskbar button font size (8-20)
- buttonLabel: "⏱"
  $name: Taskbar button label (idle)
- completionSound: true
  $name: Play completion sound
- videoSound: false
  $name: Play video sound
  $description: Play the audio track of a video background. Off by default.
*/
// ==/WindhawkModSettings==


// Portions of the taskbar/XAML integration are adapted from Windhawk's
// taskbar-multirow and taskbar-notification-icon-spacing mods by m417z,
// both licensed under GPL-3.0:
// https://github.com/ramensoftware/windhawk-mods/blob/main/mods/taskbar-multirow.wh.cpp
// https://github.com/ramensoftware/windhawk-mods/blob/main/mods/taskbar-notification-icon-spacing.wh.cpp
//
// This file is a restructured derivative of the "Taskbar Countdown
// Timer" mod by K3M and is distributed under the same GPL-3.0 license.
//
// ARCHITECTURE OVERVIEW
//   util / gfx      stateless helpers (time, paths, bitmaps, blur)
//   OptionStore     mod settings + media overrides      Preferences  UI prefs
//   SoundPlayer     completion sound (WAV / MediaPlayer / MCI / beep)
//   Countdown       deadline, persistence, worker thread, expiry events
//   FilePicker      off-thread open-file dialog
//   AlertWindow     one Win32 alert (large or small) - owns all its resources
//   AlertHost       alert thread lifecycle (open / refresh / replace / close)
//   host            taskbar.dll / SystemTray hooks, XAML root, thread marshalling
//   ui              XAML construction helpers
//   WheelPicker     alarm wheels         FlyoutView   the flyout (2 pages)
//   TaskbarButton   the taskbar button + 1 s display ticker
//   Mod wiring      lifecycle and callbacks between the components

#ifdef GetCurrentTime
#undef GetCurrentTime
#endif

#include <algorithm>
#include <array>
#include <atomic>
#include <chrono>
#include <climits>
#include <cmath>
#include <dwmapi.h>
#include <uxtheme.h>
#include <cstdint>
#include <cstdlib>
#include <functional>
#include <initializer_list>
#include <list>
#include <memory>
#include <new>
#include <optional>
#include <cwchar>
#include <string>
#include <vector>
#include <windhawk_utils.h>
#include <mmsystem.h>
#include <commdlg.h>
#include <shlwapi.h>
#include <shobjidl.h>
#include <propidl.h>
#include <wincodec.h>
#include <mfplay.h>

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Controls.Primitives.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.Media.Imaging.h>
#include <winrt/Windows.UI.Xaml.Interop.h>
#include <winrt/Windows.UI.Xaml.Input.h>
#include <winrt/Windows.UI.Input.h>
#include <winrt/Windows.UI.h>
#include <winrt/Windows.UI.Text.h>
#include <winrt/Windows.Media.Core.h>
#include <winrt/Windows.Media.Playback.h>

using namespace winrt::Windows::UI::Xaml;
using namespace winrt::Windows::UI::Xaml::Controls;
using namespace winrt::Windows::UI::Xaml::Media;
namespace Controls = winrt::Windows::UI::Xaml::Controls;

namespace tct {

// Set as soon as the mod starts unloading; every long-running piece polls it.
inline std::atomic_bool g_unloading{false};
inline std::atomic_bool g_xamlTornDown{false};

// Implemented at the end of the file (called from hooks).
void OnTaskbarStarted();
void OnTrayRebuilt();


// =============================================================================
// util: small stateless helpers
// =============================================================================

namespace util {

inline ULONGLONG NowUtc100ns()
{
    FILETIME ft{};
    GetSystemTimeAsFileTime(&ft);

    ULARGE_INTEGER value{};
    value.LowPart = ft.dwLowDateTime;
    value.HighPart = ft.dwHighDateTime;
    return value.QuadPart;
}

// Whole seconds left until the deadline (rounded up), 0 when passed / unset.
inline int SecondsLeft(ULONGLONG deadline)
{
    if (!deadline) {
        return 0;
    }

    const ULONGLONG now = NowUtc100ns();

    if (now >= deadline) {
        return 0;
    }

    const ULONGLONG seconds = (deadline - now + 9999999ULL) / 10000000ULL;

    return static_cast<int>(
        std::min<ULONGLONG>(seconds, static_cast<ULONGLONG>(INT_MAX)));
}

inline std::wstring FormatCountdown(int totalSeconds, bool alarm)
{
    if (totalSeconds < 0) {
        totalSeconds = 0;
    }

    // Stopwatch icon for a timer, alarm-clock icon for an alarm.
    const wchar_t* prefix = alarm ? L"⏰ " : L"⏱ ";

    wchar_t text[64]{};

    if (totalSeconds >= 3600) {
        swprintf_s(text, L"%s%d:%02d:%02d", prefix,
                   totalSeconds / 3600, (totalSeconds % 3600) / 60,
                   totalSeconds % 60);
    }
    else {
        swprintf_s(text, L"%s%02d:%02d", prefix,
                   totalSeconds / 60, totalSeconds % 60);
    }

    return text;
}

// Seconds until the local clock next shows hour:minute:00 (hour is 0-23).
// A time that already passed (or is right now) means tomorrow.
inline long long SecondsUntilLocalTime(int hour, int minute)
{
    SYSTEMTIME now{};
    GetLocalTime(&now);

    const long long nowSec =
        now.wHour * 3600LL + now.wMinute * 60LL + now.wSecond;
    const long long target = hour * 3600LL + minute * 60LL;

    long long diff = target - nowSec;

    if (diff <= 0) {
        diff += 86400LL;
    }

    return diff;
}

inline std::wstring BaseName(const std::wstring& path)
{
    const size_t slash = path.find_last_of(L"\\/");
    return slash == std::wstring::npos ? path : path.substr(slash + 1);
}

inline bool HasAnyExtension(
    const std::wstring& path,
    std::initializer_list<const wchar_t*> extensions)
{
    const size_t dot = path.find_last_of(L'.');

    if (dot == std::wstring::npos) {
        return false;
    }

    for (const wchar_t* ext : extensions) {
        if (_wcsicmp(path.c_str() + dot, ext) == 0) {
            return true;
        }
    }

    return false;
}

inline bool IsVideoFile(const std::wstring& path)
{
    return HasAnyExtension(path, {L".mp4", L".m4v", L".mov", L".wmv",
                                  L".avi", L".mkv", L".webm", L".mpg",
                                  L".mpeg"});
}

inline bool IsImageFile(const std::wstring& path)
{
    return HasAnyExtension(path, {L".jpg", L".jpeg", L".png", L".bmp",
                                  L".gif", L".tif", L".tiff", L".webp"});
}

inline bool AppsUseLightTheme()
{
    DWORD value = 0;
    DWORD size = sizeof(value);

    const LONG result = RegGetValueW(
        HKEY_CURRENT_USER,
        L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
        L"AppsUseLightTheme",
        RRF_RT_REG_DWORD, nullptr, &value, &size);

    return result == ERROR_SUCCESS && value != 0;
}

inline int Scale(int value, UINT dpi)
{
    return MulDiv(value, dpi ? dpi : 96, 96);
}

// Empty string on failure.
inline std::wstring ToFileUrl(const std::wstring& path)
{
    wchar_t url[2084]{};
    DWORD length = ARRAYSIZE(url);

    if (FAILED(UrlCreateFromPathW(path.c_str(), url, &length, 0))) {
        return L"";
    }

    return url;
}

// Waits for a thread while still answering sent messages (no cross-thread
// deadlocks). Returns WAIT_OBJECT_0 when it ended, WAIT_TIMEOUT otherwise.
inline DWORD PumpWaitForThread(HANDLE thread, DWORD timeoutMs = INFINITE)
{
    if (!thread) {
        return WAIT_OBJECT_0;
    }

    DWORD result;

    do {
        result = MsgWaitForMultipleObjects(1, &thread, FALSE, timeoutMs,
                                           QS_SENDMESSAGE);

        if (result == WAIT_OBJECT_0 + 1) {
            MSG message;
            PeekMessageW(&message, nullptr, 0, 0, PM_NOREMOVE);
        }
    } while (result == WAIT_OBJECT_0 + 1);

    return result;
}

}  // namespace util


// =============================================================================
// Options (mod settings + media overrides) and UI preferences
// =============================================================================

struct Options {
    int defaultMinutes = 20;
    int defaultSnoozeMinutes = 5;
    int maximumMinutes = 1440;
    int backgroundDim = 40;
    int glassTint = 35;
    int buttonFontSize = 11;
    bool completionSound = true;
    bool videoSound = false;
    std::wstring buttonLabel = L"⏱";
    std::wstring customSoundPath;        // chosen in the flyout
    std::wstring customBackgroundPath;   // chosen in the flyout (image/video)
};

class OptionStore {
public:
    static void LoadModSettings()
    {
        Options next;

        next.maximumMinutes =
            std::clamp(Wh_GetIntSetting(L"maximumMinutes"), 1, 10080);
        next.defaultMinutes =
            std::clamp(Wh_GetIntSetting(L"defaultMinutes"), 1, next.maximumMinutes);
        next.defaultSnoozeMinutes =
            std::clamp(Wh_GetIntSetting(L"defaultSnoozeMinutes"), 1, next.maximumMinutes);
        next.backgroundDim =
            std::clamp(Wh_GetIntSetting(L"backgroundDim"), 0, 80);
        next.glassTint =
            std::clamp(Wh_GetIntSetting(L"glassTint"), 0, 90);
        next.buttonFontSize =
            std::clamp(Wh_GetIntSetting(L"buttonFontSize"), 8, 20);
        next.completionSound = Wh_GetIntSetting(L"completionSound") != 0;
        next.videoSound = Wh_GetIntSetting(L"videoSound") != 0;

        auto label = WindhawkUtils::StringSetting::make(L"buttonLabel");

        if (*label) {
            next.buttonLabel = label.get();
        }

        AcquireSRWLockExclusive(&lock_);
        current_ = next;
        ReleaseSRWLockExclusive(&lock_);
    }

    static void LoadMediaOverrides()
    {
        wchar_t buffer[520]{};

        if (Wh_GetStringValue(L"soundPath", buffer, ARRAYSIZE(buffer)) &&
            buffer[0])
        {
            AcquireSRWLockExclusive(&lock_);
            sound_ = buffer;
            ReleaseSRWLockExclusive(&lock_);
        }

        buffer[0] = 0;

        if (Wh_GetStringValue(L"backgroundPath", buffer, ARRAYSIZE(buffer)) &&
            buffer[0])
        {
            AcquireSRWLockExclusive(&lock_);
            background_ = buffer;
            ReleaseSRWLockExclusive(&lock_);
        }
    }

    static Options Snapshot()
    {
        AcquireSRWLockShared(&lock_);
        Options copy = current_;
        copy.customSoundPath = sound_;
        copy.customBackgroundPath = background_;
        ReleaseSRWLockShared(&lock_);
        return copy;
    }

    // nullptr / empty = back to the default.
    static void SetSound(const wchar_t* path)
    {
        Store(sound_, L"soundPath", path);
    }

    static void SetBackground(const wchar_t* path)
    {
        Store(background_, L"backgroundPath", path);
    }

    static std::wstring SoundDisplayName()
    {
        const Options o = Snapshot();
        return o.customSoundPath.empty() ? L"Default Windows sound"
                                         : util::BaseName(o.customSoundPath);
    }

    static std::wstring BackgroundDisplayName()
    {
        const Options o = Snapshot();
        return o.customBackgroundPath.empty()
                   ? L"Default (dark gradient)"
                   : util::BaseName(o.customBackgroundPath);
    }

private:
    static void Store(std::wstring& slot, const wchar_t* key, const wchar_t* path)
    {
        AcquireSRWLockExclusive(&lock_);
        slot = path ? path : L"";
        ReleaseSRWLockExclusive(&lock_);

        if (path && path[0]) {
            Wh_SetStringValue(key, path);
        }
        else {
            Wh_DeleteValue(key);
        }
    }

    static inline SRWLOCK lock_ = SRWLOCK_INIT;
    static inline Options current_;
    static inline std::wstring sound_;
    static inline std::wstring background_;
};


// Choices made inside the flyout (persisted individually).
struct Preferences {
    std::atomic_bool alertSmall{false};     // false = large full screen
    std::atomic<int> alertTheme{0};         // 0 dark, 1 light, 2 follow Windows
    std::atomic_bool flyoutGlass{false};    // false = solid, true = frosted
    std::atomic_bool alarmMode{false};      // flyout shows Alarm instead of Timer

    // Alarm wheel value (12-hour clock).
    std::atomic<int> alarmHour12{7};
    std::atomic<int> alarmMinute{0};
    std::atomic<int> alarmPM{0};

    void Load()
    {
        alertSmall.store(Wh_GetIntValue(L"alertSmall", 0) != 0);
        alertTheme.store(std::clamp(Wh_GetIntValue(L"alertTheme", 0), 0, 2));
        flyoutGlass.store(Wh_GetIntValue(L"flyoutGlass", 0) != 0);
        alarmMode.store(Wh_GetIntValue(L"alarmMode", 0) != 0);
    }
};

inline Preferences g_prefs;


// =============================================================================
// SoundPlayer: .wav via PlaySound, everything else via MediaPlayer -> MCI ->
// system beep.
// =============================================================================

class SoundPlayer {
public:
    // force = true is used by the "Test" button and ignores the on/off setting.
    void Play(bool force = false)
    {
        const Options options = OptionStore::Snapshot();

        if (!force && !options.completionSound) {
            return;
        }

        Stop();

        const std::wstring& path = options.customSoundPath;

        if (!path.empty()) {
            if (GetFileAttributesW(path.c_str()) == INVALID_FILE_ATTRIBUTES) {
                Wh_Log(L"Custom sound file not found: %s", path.c_str());
            }
            else {
                if (util::HasAnyExtension(path, {L".wav"})) {
                    if (PlaySoundW(path.c_str(), nullptr,
                                   SND_FILENAME | SND_ASYNC | SND_NODEFAULT))
                    {
                        return;
                    }

                    Wh_Log(L"PlaySound failed for: %s", path.c_str());
                }

                if (StartJob(path)) {
                    return;
                }

                Wh_Log(L"Custom sound failed, falling back to default beep");
            }
        }

        FallbackBeep();
    }

    void Stop()
    {
        PlaySoundW(nullptr, nullptr, 0);

        std::shared_ptr<std::atomic_bool> flag;

        AcquireSRWLockExclusive(&lock_);
        flag = std::move(stopFlag_);
        stopFlag_.reset();
        ReleaseSRWLockExclusive(&lock_);

        if (flag) {
            flag->store(true);
        }
    }

    // Callers must have raised g_unloading / called Stop() so threads end fast.
    void WaitForThreads(DWORD timeoutMs)
    {
        const ULONGLONG start = GetTickCount64();

        while (active_.load() > 0 && GetTickCount64() - start < timeoutMs) {
            const DWORD result = MsgWaitForMultipleObjects(
                0, nullptr, FALSE, 20, QS_SENDMESSAGE);

            if (result == WAIT_OBJECT_0) {
                MSG message;
                PeekMessageW(&message, nullptr, 0, 0, PM_NOREMOVE);
            }
        }

        if (active_.load() > 0) {
            Wh_Log(L"WARNING: MCI sound thread did not exit in time");
        }
    }

private:
    struct Job {
        SoundPlayer* owner = nullptr;
        std::wstring path;
        std::shared_ptr<std::atomic_bool> stopFlag;
        unsigned id = 0;
    };

    static bool Mci(const std::wstring& command)
    {
        const MCIERROR error =
            mciSendStringW(command.c_str(), nullptr, 0, nullptr);

        if (error) {
            wchar_t text[256]{};
            mciGetErrorStringW(error, text, ARRAYSIZE(text));
            Wh_Log(L"MCI '%s' failed: %s", command.c_str(), text);
        }

        return error == 0;
    }

    static DWORD WINAPI BeepThread(LPVOID param)
    {
        auto* self = static_cast<SoundPlayer*>(param);

        if (!g_unloading.load()) {
            Beep(880, 180);
        }

        if (!g_unloading.load()) {
            Beep(1175, 260);
        }

        self->active_.fetch_sub(1);
        return 0;
    }

    void FallbackBeep()
    {
        if (PlaySoundW(L"SystemExclamation", nullptr,
                       SND_ALIAS | SND_ASYNC | SND_NODEFAULT))
        {
            return;
        }

        active_.fetch_add(1);

        HANDLE thread =
            CreateThread(nullptr, 0, &SoundPlayer::BeepThread, this, 0, nullptr);

        if (thread) {
            CloseHandle(thread);
        }
        else {
            active_.fetch_sub(1);
            MessageBeep(MB_ICONEXCLAMATION);
        }
    }

    static bool RunMediaPlayer(const Job& job)
    {
        using namespace winrt::Windows::Media::Playback;
        using namespace winrt::Windows::Media::Core;

        const std::wstring url = util::ToFileUrl(job.path);

        if (url.empty()) {
            Wh_Log(L"UrlCreateFromPath failed for: %s", job.path.c_str());
            return false;
        }

        HANDLE done = CreateEventW(nullptr, TRUE, FALSE, nullptr);

        if (!done) {
            return false;
        }

        std::atomic_bool failed{false};
        std::atomic_bool started{false};
        bool ok = false;

        try {
            MediaPlayer player;
            player.AutoPlay(false);
            player.Volume(1.0);

            auto endedToken = player.MediaEnded(
                [&](auto const&, auto const&) { SetEvent(done); });

            auto failedToken = player.MediaFailed(
                [&](auto const&, MediaPlayerFailedEventArgs const& args)
                {
                    failed.store(true);

                    Wh_Log(L"MediaPlayer failed: 0x%08X %s",
                           static_cast<unsigned>(
                               static_cast<int32_t>(args.ExtendedErrorCode())),
                           args.ErrorMessage().c_str());

                    SetEvent(done);
                });

            player.Source(MediaSource::CreateFromUri(
                winrt::Windows::Foundation::Uri(url)));

            player.Play();
            started.store(true);

            const ULONGLONG start = GetTickCount64();

            while (!job.stopFlag->load() && !g_unloading.load() &&
                   !failed.load())
            {
                if (WaitForSingleObject(done, 100) == WAIT_OBJECT_0) {
                    break;
                }

                if (GetTickCount64() - start > 5ULL * 60ULL * 1000ULL) {
                    break;
                }
            }

            player.MediaEnded(endedToken);
            player.MediaFailed(failedToken);

            try { player.Pause(); } catch (...) {}
            try { player.Close(); } catch (...) {}

            ok = started.load() && !failed.load();
        }
        catch (...) {
            Wh_Log(L"MediaPlayer exception: 0x%08X",
                   static_cast<unsigned>(winrt::to_hresult().value));
            ok = false;
        }

        CloseHandle(done);
        return ok;
    }

    static bool RunMci(const Job& job)
    {
        wchar_t aliasBuffer[32]{};
        swprintf_s(aliasBuffer, L"tctsound%u", job.id);

        const std::wstring alias = aliasBuffer;
        const std::wstring quoted = L"\"" + job.path + L"\"";

        const bool opened =
            Mci(L"open " + quoted + L" alias " + alias) ||
            Mci(L"open " + quoted + L" type mpegvideo alias " + alias);

        if (!opened) {
            return false;
        }

        bool played = false;

        if (Mci(L"play " + alias)) {
            played = true;

            const ULONGLONG start = GetTickCount64();
            const std::wstring statusCommand = L"status " + alias + L" mode";

            while (!job.stopFlag->load() && !g_unloading.load()) {
                wchar_t mode[64]{};

                if (mciSendStringW(statusCommand.c_str(), mode,
                                   ARRAYSIZE(mode), nullptr) != 0)
                {
                    break;
                }

                const ULONGLONG elapsed = GetTickCount64() - start;

                if (_wcsicmp(mode, L"playing") != 0 && elapsed > 500) {
                    break;
                }

                if (elapsed > 5ULL * 60ULL * 1000ULL) {
                    break;
                }

                Sleep(100);
            }
        }

        mciSendStringW((L"stop " + alias).c_str(), nullptr, 0, nullptr);
        mciSendStringW((L"close " + alias).c_str(), nullptr, 0, nullptr);

        return played;
    }

    static DWORD WINAPI JobThread(LPVOID param)
    {
        std::unique_ptr<Job> job(static_cast<Job*>(param));
        SoundPlayer* owner = job->owner;

        const auto keepGoing = [&]
        {
            return !job->stopFlag->load() && !g_unloading.load();
        };

        bool apartment = false;

        try {
            winrt::init_apartment(winrt::apartment_type::multi_threaded);
            apartment = true;
        }
        catch (...) {
            Wh_Log(L"init_apartment failed on sound thread");
        }

        bool played = false;

        if (apartment && keepGoing()) {
            played = RunMediaPlayer(*job);
        }

        if (!played && keepGoing()) {
            played = RunMci(*job);
        }

        if (!played && keepGoing()) {
            Wh_Log(L"Custom sound failed, falling back to default beep");
            owner->FallbackBeep();
        }

        if (apartment) {
            winrt::uninit_apartment();
        }

        owner->active_.fetch_sub(1);
        return 0;
    }

    bool StartJob(const std::wstring& path)
    {
        auto job = std::make_unique<Job>();
        job->owner = this;
        job->path = path;
        job->stopFlag = std::make_shared<std::atomic_bool>(false);
        job->id = counter_.fetch_add(1) + 1;

        auto stopFlag = job->stopFlag;

        active_.fetch_add(1);

        HANDLE thread = CreateThread(nullptr, 0, &SoundPlayer::JobThread,
                                     job.get(), 0, nullptr);

        if (!thread) {
            active_.fetch_sub(1);
            Wh_Log(L"ERROR: Could not create MCI sound thread");
            return false;
        }

        job.release();
        CloseHandle(thread);

        AcquireSRWLockExclusive(&lock_);
        stopFlag_ = stopFlag;
        ReleaseSRWLockExclusive(&lock_);

        return true;
    }

    SRWLOCK lock_ = SRWLOCK_INIT;
    std::shared_ptr<std::atomic_bool> stopFlag_;   // guarded by lock_
    std::atomic<int> active_{0};
    std::atomic<unsigned> counter_{0};
};

inline SoundPlayer g_sound;


// =============================================================================
// Countdown: the single running timer / alarm
// =============================================================================
//
// Owns the deadline (UTC, 100 ns units), its persisted copy, and the worker
// thread that waits on a waitable timer. It knows nothing about XAML or
// windows: the rest of the mod reacts through the three callbacks.

class Countdown {
public:
    std::function<void()> onArmed;              // a timer/alarm was started
    std::function<void()> onDisarmed;           // cancelled by the user
    std::function<void(bool restored)> onFinished;   // time is up

    bool Initialize()
    {
        stopEvent_ = CreateEventW(nullptr, TRUE, FALSE, nullptr);
        rearmEvent_ = CreateEventW(nullptr, FALSE, FALSE, nullptr);
        waitable_ = CreateWaitableTimerW(nullptr, FALSE, nullptr);

        if (!stopEvent_ || !rearmEvent_ || !waitable_) {
            Wh_Log(L"ERROR: Could not create timer synchronization objects");
            Dispose();
            return false;
        }

        thread_ = CreateThread(nullptr, 0, &Countdown::WorkerThread, this, 0,
                               nullptr);

        if (!thread_) {
            Wh_Log(L"ERROR: Could not create timer worker thread");
            Dispose();
            return false;
        }

        return true;
    }

    void RequestStop()
    {
        if (stopEvent_) {
            SetEvent(stopEvent_);
        }
    }

    void Join()
    {
        if (thread_) {
            util::PumpWaitForThread(thread_);
            CloseHandle(thread_);
            thread_ = nullptr;
        }
    }

    void Dispose()
    {
        if (waitable_) { CloseHandle(waitable_); waitable_ = nullptr; }
        if (rearmEvent_) { CloseHandle(rearmEvent_); rearmEvent_ = nullptr; }
        if (stopEvent_) { CloseHandle(stopEvent_); stopEvent_ = nullptr; }
    }

    // alarm = true marks the countdown as an alarm (other icon / alert title).
    // Alarms may be up to 24 hours regardless of the maximum setting.
    bool Arm(int totalSeconds, const wchar_t* reminder, bool alarm = false)
    {
        const Options options = OptionStore::Snapshot();

        const long long limit =
            alarm ? 86400LL
                  : static_cast<long long>(options.maximumMinutes) * 60LL;

        if (totalSeconds <= 0 ||
            static_cast<long long>(totalSeconds) > limit ||
            g_unloading.load())
        {
            return false;
        }

        const ULONGLONG deadline =
            util::NowUtc100ns() +
            static_cast<ULONGLONG>(totalSeconds) * 10000000ULL;

        const wchar_t* text =
            reminder && reminder[0] ? reminder
                                    : (alarm ? L"Alarm" : L"Timer finished");

        SetReminder(text);
        alarm_.store(alarm);
        deadline_.store(deadline);
        Persist(deadline, text);
        Wake();

        if (onArmed) {
            onArmed();
        }

        Wh_Log(L"%s armed: '%s' - %d second(s)",
               alarm ? L"Alarm" : L"Timer", text, totalSeconds);

        return true;
    }

    void Cancel()
    {
        deadline_.store(0);
        ClearPersisted();
        Wake();

        if (onDisarmed) {
            onDisarmed();
        }

        Wh_Log(L"Timer cancelled");
    }

    int Remaining() const { return util::SecondsLeft(deadline_.load()); }
    bool Running() const { return Remaining() > 0; }
    bool IsAlarm() const { return alarm_.load(); }

    std::wstring Reminder() const
    {
        AcquireSRWLockShared(&reminderLock_);
        std::wstring copy = reminder_;
        ReleaseSRWLockShared(&reminderLock_);
        return copy;
    }

    // Reminder text, or the default title for the current kind.
    std::wstring ReminderOrDefault() const
    {
        std::wstring text = Reminder();

        if (text.empty()) {
            text = alarm_.load() ? L"Alarm" : L"Timer finished";
        }

        return text;
    }

    // Brings back a timer/alarm that was running before Explorer restarted.
    void RestorePersisted()
    {
        wchar_t deadlineText[32]{};

        if (!Wh_GetStringValue(L"deadlineUtc100ns", deadlineText,
                               ARRAYSIZE(deadlineText)))
        {
            return;
        }

        wchar_t* end = nullptr;
        const unsigned long long parsed = wcstoull(deadlineText, &end, 10);

        if (!end || end == deadlineText || *end != L'\0' || !parsed) {
            ClearPersisted();
            return;
        }

        wchar_t reminder[256]{};
        Wh_GetStringValue(L"reminder", reminder, ARRAYSIZE(reminder));

        alarm_.store(Wh_GetIntValue(L"isAlarm", 0) != 0);

        SetReminder(reminder[0] ? reminder
                                : (alarm_.load() ? L"Alarm" : L"Timer finished"));

        const ULONGLONG deadline = static_cast<ULONGLONG>(parsed);
        const int remaining = util::SecondsLeft(deadline);

        if (remaining > 0) {
            deadline_.store(deadline);
            Wake();

            Wh_Log(L"Restored running timer/alarm with %d second(s) remaining",
                   remaining);
            return;
        }

        deadline_.store(0);

        const ULONGLONG now = util::NowUtc100ns();
        constexpr ULONGLONG kStaleCutoff = 6ULL * 60ULL * 60ULL * 10000000ULL;

        if (now > deadline && now - deadline > kStaleCutoff) {
            ClearPersisted();
            Wh_Log(L"Ignoring stale expired timer from more than 6 hours ago");
            return;
        }

        ClearPersisted();
        Wh_Log(L"Restored expired timer/alarm");

        if (onFinished) {
            onFinished(true);
        }
    }

private:
    void Wake()
    {
        if (rearmEvent_) {
            SetEvent(rearmEvent_);
        }
    }

    void SetReminder(const wchar_t* text)
    {
        AcquireSRWLockExclusive(&reminderLock_);
        wcsncpy_s(reminder_, text ? text : L"", _TRUNCATE);
        ReleaseSRWLockExclusive(&reminderLock_);
    }

    void Persist(ULONGLONG deadline, const wchar_t* reminder)
    {
        wchar_t deadlineText[32]{};
        swprintf_s(deadlineText, L"%llu",
                   static_cast<unsigned long long>(deadline));

        Wh_SetStringValue(L"deadlineUtc100ns", deadlineText);
        Wh_SetStringValue(L"reminder", reminder ? reminder : L"");
        Wh_SetIntValue(L"isAlarm", alarm_.load() ? 1 : 0);
    }

    static void ClearPersisted()
    {
        Wh_DeleteValue(L"deadlineUtc100ns");
        Wh_DeleteValue(L"reminder");
        Wh_DeleteValue(L"isAlarm");
    }

    void HandleExpired()
    {
        if (g_unloading.load()) {
            return;
        }

        const ULONGLONG expected = deadline_.load();

        if (!expected) {
            return;
        }

        if (util::NowUtc100ns() < expected) {
            // Woke slightly early: wait a moment and re-arm.
            if (WaitForSingleObject(stopEvent_, 250) == WAIT_TIMEOUT) {
                Wake();
            }

            return;
        }

        deadline_.store(0);
        ClearPersisted();

        if (onFinished) {
            onFinished(false);
        }
    }

    void WorkerLoop()
    {
        HANDLE handles[] = {stopEvent_, rearmEvent_, waitable_};

        while (!g_unloading.load()) {
            const DWORD wait = WaitForMultipleObjects(
                ARRAYSIZE(handles), handles, FALSE, INFINITE);

            if (wait == WAIT_OBJECT_0) {
                break;
            }

            if (wait == WAIT_OBJECT_0 + 1) {
                const ULONGLONG deadline = deadline_.load();

                if (!deadline) {
                    CancelWaitableTimer(waitable_);
                    continue;
                }

                LARGE_INTEGER due{};
                due.QuadPart = static_cast<LONGLONG>(deadline);

                if (!SetWaitableTimer(waitable_, &due, 0, nullptr, nullptr,
                                      FALSE))
                {
                    Wh_Log(L"ERROR: SetWaitableTimer failed: %u",
                           GetLastError());
                }

                continue;
            }

            if (wait == WAIT_OBJECT_0 + 2) {
                HandleExpired();
                continue;
            }

            break;
        }

        CancelWaitableTimer(waitable_);
    }

    static DWORD WINAPI WorkerThread(LPVOID param)
    {
        static_cast<Countdown*>(param)->WorkerLoop();
        return 0;
    }

    std::atomic<ULONGLONG> deadline_{0};
    std::atomic_bool alarm_{false};

    mutable SRWLOCK reminderLock_ = SRWLOCK_INIT;
    wchar_t reminder_[256]{};

    HANDLE stopEvent_ = nullptr;
    HANDLE rearmEvent_ = nullptr;
    HANDLE waitable_ = nullptr;
    HANDLE thread_ = nullptr;
};

inline Countdown g_countdown;


// =============================================================================
// FilePicker: open-file dialog on its own thread (never blocks the taskbar)
// =============================================================================

enum class PickKind { Sound = 0, Background = 1 };

class FilePicker {
public:
    std::function<void()> onClosed;   // called on the picker thread when done

    void Start(PickKind kind)
    {
        if (g_unloading.load()) {
            return;
        }

        HANDLE existing = thread_.load();

        if (existing) {
            if (WaitForSingleObject(existing, 0) != WAIT_OBJECT_0) {
                return;   // dialog is still open
            }

            CloseHandle(existing);
            thread_.store(nullptr);
        }

        kind_.store(static_cast<int>(kind));

        HANDLE thread = CreateThread(nullptr, 0, &FilePicker::ThreadProc, this,
                                     0, nullptr);

        if (thread) {
            thread_.store(thread);
        }
        else {
            Wh_Log(L"ERROR: Could not create file picker thread");
        }
    }

    // Closes an open dialog and joins the thread (mod unload).
    void Shutdown()
    {
        HANDLE thread = thread_.exchange(nullptr);

        if (!thread) {
            return;
        }

        for (int i = 0; i < 50; i++) {
            CloseDialog(thread);

            if (util::PumpWaitForThread(thread, 100) != WAIT_TIMEOUT) {
                break;
            }
        }

        CloseHandle(thread);
    }

private:
    static void CloseDialog(HANDLE thread)
    {
        const DWORD threadId = GetThreadId(thread);

        if (!threadId) {
            return;
        }

        EnumThreadWindows(
            threadId,
            [](HWND hWnd, LPARAM) -> BOOL
            {
                PostMessageW(hWnd, WM_CLOSE, 0, 0);
                return TRUE;
            },
            0);
    }

    static DWORD WINAPI ThreadProc(LPVOID param)
    {
        auto* self = static_cast<FilePicker*>(param);

        CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);

        const PickKind kind = static_cast<PickKind>(self->kind_.load());

        wchar_t file[520]{};

        OPENFILENAMEW ofn{};
        ofn.lStructSize = sizeof(ofn);

        if (kind == PickKind::Background) {
            ofn.lpstrFilter =
                L"Images and videos\0"
                L"*.jpg;*.jpeg;*.png;*.bmp;*.gif;*.tif;*.tiff;"
                L"*.mp4;*.m4v;*.mov;*.wmv;*.avi;*.mkv;*.webm\0"
                L"All files (*.*)\0*.*\0";
            ofn.lpstrTitle = L"Choose full-screen background (image or video)";
        }
        else {
            ofn.lpstrFilter =
                L"Audio files (*.wav;*.mp3;*.wma;*.m4a;*.aac)\0"
                L"*.wav;*.mp3;*.wma;*.m4a;*.aac\0"
                L"All files (*.*)\0*.*\0";
            ofn.lpstrTitle = L"Choose timer sound";
        }

        ofn.lpstrFile = file;
        ofn.nMaxFile = ARRAYSIZE(file);
        ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST |
                    OFN_HIDEREADONLY | OFN_NOCHANGEDIR;

        const BOOL ok = GetOpenFileNameW(&ofn);

        CoUninitialize();

        if (g_unloading.load()) {
            return 0;
        }

        if (ok && file[0]) {
            if (kind == PickKind::Background) {
                OptionStore::SetBackground(file);
            }
            else {
                OptionStore::SetSound(file);
            }
        }

        if (self->onClosed) {
            self->onClosed();
        }

        return 0;
    }

    std::atomic<HANDLE> thread_{nullptr};
    std::atomic<int> kind_{0};
};

inline FilePicker g_picker;


// =============================================================================
// gfx: bitmap helpers for the alert window
// =============================================================================

namespace gfx {

inline HBITMAP CreateDib(int w, int h, uint32_t** bits)
{
    BITMAPINFO bi{};
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = w;
    bi.bmiHeader.biHeight = -h;   // top-down
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;

    void* data = nullptr;
    HBITMAP bitmap =
        CreateDIBSection(nullptr, &bi, DIB_RGB_COLORS, &data, nullptr, 0);

    *bits = static_cast<uint32_t*>(data);
    return bitmap;
}

inline void Dim(uint32_t* px, size_t count, int dimPercent)
{
    if (dimPercent <= 0) {
        return;
    }

    const uint32_t scale =
        static_cast<uint32_t>((100 - std::min(dimPercent, 95)) * 256 / 100);

    for (size_t i = 0; i < count; i++) {
        const uint32_t p = px[i];
        const uint32_t r = (((p >> 16) & 0xFF) * scale) >> 8;
        const uint32_t g = (((p >> 8) & 0xFF) * scale) >> 8;
        const uint32_t b = ((p & 0xFF) * scale) >> 8;
        px[i] = (r << 16) | (g << 8) | b;
    }
}

// Dark blue/purple gradient with a soft glow behind the card.
inline HBITMAP CreateDefaultBackground(int w, int h)
{
    uint32_t* px = nullptr;
    HBITMAP bitmap = CreateDib(w, h, &px);

    if (!bitmap || !px) {
        return bitmap;
    }

    const double cx = w * 0.5;
    const double cy = h * 0.42;
    const double radius = std::max(w, h) * 0.75;

    for (int y = 0; y < h; y++) {
        const double t = static_cast<double>(y) / h;
        const double baseR = 10 + 22 * t;
        const double baseG = 14 + 8 * t;
        const double baseB = 30 + 40 * t;

        for (int x = 0; x < w; x++) {
            const double dx = x - cx;
            const double dy = y - cy;
            const double d = std::sqrt(dx * dx + dy * dy) / radius;
            const double glow = d < 1.0 ? (1.0 - d) * (1.0 - d) : 0.0;

            const int r = std::min(255, static_cast<int>(baseR + glow * 40));
            const int g = std::min(255, static_cast<int>(baseG + glow * 70));
            const int b = std::min(255, static_cast<int>(baseB + glow * 150));

            px[static_cast<size_t>(y) * w + x] =
                (static_cast<uint32_t>(r) << 16) |
                (static_cast<uint32_t>(g) << 8) | static_cast<uint32_t>(b);
        }
    }

    return bitmap;
}

// Decodes an image with WIC and scales it to cover w x h (centered crop).
inline HBITMAP LoadCoverBitmap(const std::wstring& path, int w, int h,
                               int dimPercent)
{
    HBITMAP bitmap = nullptr;

    try {
        auto factory = winrt::create_instance<IWICImagingFactory>(
            CLSID_WICImagingFactory, CLSCTX_INPROC_SERVER);

        winrt::com_ptr<IWICBitmapDecoder> decoder;
        winrt::check_hresult(factory->CreateDecoderFromFilename(
            path.c_str(), nullptr, GENERIC_READ,
            WICDecodeMetadataCacheOnDemand, decoder.put()));

        winrt::com_ptr<IWICBitmapFrameDecode> frame;
        winrt::check_hresult(decoder->GetFrame(0, frame.put()));

        UINT sw = 0;
        UINT sh = 0;
        winrt::check_hresult(frame->GetSize(&sw, &sh));

        if (!sw || !sh) {
            return nullptr;
        }

        const double scale = std::max(static_cast<double>(w) / sw,
                                      static_cast<double>(h) / sh);

        const UINT nw =
            std::max<UINT>(w, static_cast<UINT>(std::ceil(sw * scale)));
        const UINT nh =
            std::max<UINT>(h, static_cast<UINT>(std::ceil(sh * scale)));

        winrt::com_ptr<IWICBitmapScaler> scaler;
        winrt::check_hresult(factory->CreateBitmapScaler(scaler.put()));
        winrt::check_hresult(scaler->Initialize(
            frame.get(), nw, nh, WICBitmapInterpolationModeFant));

        winrt::com_ptr<IWICFormatConverter> converter;
        winrt::check_hresult(factory->CreateFormatConverter(converter.put()));
        winrt::check_hresult(converter->Initialize(
            scaler.get(), GUID_WICPixelFormat32bppBGR, WICBitmapDitherTypeNone,
            nullptr, 0.0, WICBitmapPaletteTypeCustom));

        uint32_t* px = nullptr;
        bitmap = CreateDib(w, h, &px);

        if (!bitmap || !px) {
            return nullptr;
        }

        WICRect rect{static_cast<INT>((nw - w) / 2),
                     static_cast<INT>((nh - h) / 2), w, h};

        winrt::check_hresult(converter->CopyPixels(
            &rect, static_cast<UINT>(w) * 4, static_cast<UINT>(w) * h * 4,
            reinterpret_cast<BYTE*>(px)));

        Dim(px, static_cast<size_t>(w) * h, dimPercent);

        return bitmap;
    }
    catch (...) {
        Wh_Log(L"Loading background image failed: 0x%08X",
               static_cast<unsigned>(winrt::to_hresult().value));

        if (bitmap) {
            DeleteObject(bitmap);
        }

        return nullptr;
    }
}

// Shell thumbnail of a video, cropped to cover w x h. Only used as the source
// of the blurred frosted card; the live video itself stays unchanged.
inline HBITMAP LoadVideoPoster(const std::wstring& path, int w, int h)
{
    IShellItemImageFactory* factory = nullptr;

    if (FAILED(SHCreateItemFromParsingName(path.c_str(), nullptr,
                                           IID_PPV_ARGS(&factory))) ||
        !factory)
    {
        return nullptr;
    }

    HBITMAP thumb = nullptr;
    SIZE size{std::max(w / 2, 256), std::max(h / 2, 144)};

    const HRESULT hr = factory->GetImage(
        size, SIIGBF_BIGGERSIZEOK | SIIGBF_THUMBNAILONLY, &thumb);

    factory->Release();

    if (FAILED(hr) || !thumb) {
        Wh_Log(L"Video thumbnail not available: 0x%08X",
               static_cast<unsigned>(hr));
        return nullptr;
    }

    BITMAP bm{};
    GetObjectW(thumb, sizeof(bm), &bm);

    const int sw = bm.bmWidth;
    const int sh = bm.bmHeight;

    if (sw <= 0 || sh <= 0) {
        DeleteObject(thumb);
        return nullptr;
    }

    std::vector<uint32_t> src(static_cast<size_t>(sw) * sh);

    BITMAPINFO bi{};
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = sw;
    bi.bmiHeader.biHeight = -sh;
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;

    HDC dc = GetDC(nullptr);
    const int lines = GetDIBits(dc, thumb, 0, sh, src.data(), &bi, DIB_RGB_COLORS);
    ReleaseDC(nullptr, dc);
    DeleteObject(thumb);

    if (!lines) {
        return nullptr;
    }

    uint32_t* px = nullptr;
    HBITMAP out = CreateDib(w, h, &px);

    if (!out || !px) {
        if (out) {
            DeleteObject(out);
        }

        return nullptr;
    }

    const double scale =
        std::max(static_cast<double>(w) / sw, static_cast<double>(h) / sh);
    const double offX = (sw * scale - w) / 2.0;
    const double offY = (sh * scale - h) / 2.0;

    for (int y = 0; y < h; y++) {
        const int sy =
            std::clamp(static_cast<int>((y + offY) / scale), 0, sh - 1);

        for (int x = 0; x < w; x++) {
            const int sx =
                std::clamp(static_cast<int>((x + offX) / scale), 0, sw - 1);

            px[static_cast<size_t>(y) * w + x] =
                src[static_cast<size_t>(sy) * sw + sx] & 0x00FFFFFF;
        }
    }

    return out;
}

// One running-sum box blur pass along rows (horizontal) or columns.
inline void BoxBlurPass(const uint32_t* src, uint32_t* dst, int w, int h,
                        int r, bool horizontal)
{
    const int len = horizontal ? w : h;
    const int lines = horizontal ? h : w;
    const size_t stride = horizontal ? 1 : static_cast<size_t>(w);
    const size_t lineStep = horizontal ? static_cast<size_t>(w) : 1;
    const int win = 2 * r + 1;

    for (int l = 0; l < lines; l++) {
        const uint32_t* s = src + static_cast<size_t>(l) * lineStep;
        uint32_t* d = dst + static_cast<size_t>(l) * lineStep;

        auto at = [&](int i) -> uint32_t
        {
            i = std::clamp(i, 0, len - 1);
            return s[static_cast<size_t>(i) * stride];
        };

        int sr = 0, sg = 0, sb = 0;

        for (int i = -r; i <= r; i++) {
            const uint32_t p = at(i);
            sr += (p >> 16) & 0xFF;
            sg += (p >> 8) & 0xFF;
            sb += p & 0xFF;
        }

        for (int i = 0; i < len; i++) {
            d[static_cast<size_t>(i) * stride] =
                (static_cast<uint32_t>(sr / win) << 16) |
                (static_cast<uint32_t>(sg / win) << 8) |
                static_cast<uint32_t>(sb / win);

            const uint32_t out = at(i - r);
            const uint32_t in = at(i + r + 1);

            sr += static_cast<int>((in >> 16) & 0xFF) -
                  static_cast<int>((out >> 16) & 0xFF);
            sg += static_cast<int>((in >> 8) & 0xFF) -
                  static_cast<int>((out >> 8) & 0xFF);
            sb += static_cast<int>(in & 0xFF) - static_cast<int>(out & 0xFF);
        }
    }
}

// Loops the video and keeps it muted unless the setting says otherwise.
class VideoCallback final : public IMFPMediaPlayerCallback {
public:
    explicit VideoCallback(bool mute) : m_mute(mute) {}

    STDMETHODIMP QueryInterface(REFIID riid, void** ppv) override
    {
        if (!ppv) {
            return E_POINTER;
        }

        if (IsEqualIID(riid, IID_IUnknown) ||
            IsEqualIID(riid, __uuidof(IMFPMediaPlayerCallback)))
        {
            *ppv = static_cast<IMFPMediaPlayerCallback*>(this);
            AddRef();
            return S_OK;
        }

        *ppv = nullptr;
        return E_NOINTERFACE;
    }

    STDMETHODIMP_(ULONG) AddRef() override
    {
        return static_cast<ULONG>(++m_ref);
    }

    STDMETHODIMP_(ULONG) Release() override
    {
        const ULONG r = static_cast<ULONG>(--m_ref);

        if (!r) {
            delete this;
        }

        return r;
    }

    void STDMETHODCALLTYPE OnMediaPlayerEvent(MFP_EVENT_HEADER* header) override
    {
        if (!header || !header->pMediaPlayer || FAILED(header->hrEvent)) {
            return;
        }

        switch (header->eEventType) {
        case MFP_EVENT_TYPE_MEDIAITEM_SET:
            header->pMediaPlayer->SetMute(m_mute ? TRUE : FALSE);
            break;

        case MFP_EVENT_TYPE_PLAYBACK_ENDED:
        {
            PROPVARIANT position;
            PropVariantInit(&position);
            position.vt = VT_I8;
            position.hVal.QuadPart = 0;

            header->pMediaPlayer->SetPosition(GUID_NULL, &position);
            header->pMediaPlayer->Play();
            break;
        }

        default:
            break;
        }
    }

private:
    std::atomic<LONG> m_ref{1};
    bool m_mute;
};

using MFPCreateMediaPlayer_t = HRESULT(WINAPI*)(
    LPCWSTR, BOOL, MFP_CREATION_OPTIONS, IMFPMediaPlayerCallback*, HWND,
    IMFPMediaPlayer**);

}  // namespace gfx


// =============================================================================
// AlertWindow: one completion alert (large full screen or small notification)
// =============================================================================
//
// Large: borderless topmost window over the taskbar's monitor.
// Small: compact topmost window in the bottom-right corner of the work area.
// Background = generated gradient, image (WIC, "cover") or looping video
// (MFPlay, large mode only). The centered card is frosted glass. Every GDI /
// COM resource belongs to the instance and is released with it.

namespace alert_ids {
constexpr int Reminder = 2100;
constexpr int SnoozeLabel = 2101;
constexpr int SnoozeHours = 2102;
constexpr int Snooze = 2103;
constexpr int Dismiss = 2104;
constexpr int SnoozeMinutes = 2105;
constexpr int SnoozeSeconds = 2106;
constexpr int UnitH = 2107;
constexpr int UnitM = 2108;
constexpr int UnitS = 2109;
constexpr int Title = 2110;
constexpr int Subtitle = 2111;
}  // namespace alert_ids

constexpr UINT WM_APP_ALERT_REFRESH = WM_APP + 20;

struct AlertData {
    wchar_t reminder[256]{};
    int defaultSnoozeSeconds = 300;
    int maximumSeconds = 86400;
    bool preview = false;
    bool compactAlert = false;
    bool alarm = false;      // the alert is for an alarm (not a timer)
};

inline AlertData MakeAlertData(const wchar_t* reminder, bool preview)
{
    AlertData data;

    wcsncpy_s(data.reminder,
              reminder && reminder[0] ? reminder : L"Timer finished",
              _TRUNCATE);

    const Options options = OptionStore::Snapshot();
    data.defaultSnoozeSeconds = options.defaultSnoozeMinutes * 60;
    data.maximumSeconds = options.maximumMinutes * 60;
    data.preview = preview;
    data.compactAlert = g_prefs.alertSmall.load();
    data.alarm = !preview && g_countdown.IsAlarm();

    return data;
}

class AlertWindow {
public:
    std::function<void()> onDestroyed;

    explicit AlertWindow(const AlertData& data) : data_(data) {}

    ~AlertWindow() { FreeStyle(gdi_); }

    AlertWindow(const AlertWindow&) = delete;
    AlertWindow& operator=(const AlertWindow&) = delete;

    static bool RegisterClasses()
    {
        if (registered_) {
            return true;
        }

        HMODULE module = nullptr;

        if (!GetModuleHandleExW(
                GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                    GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                reinterpret_cast<LPCWSTR>(&AlertWindow::WndProc), &module))
        {
            Wh_Log(L"ERROR: Could not get mod module handle for alert class");
            return false;
        }

        WNDCLASSEXW wc{sizeof(wc)};
        wc.lpfnWndProc = &AlertWindow::WndProc;
        wc.hInstance = module;
        wc.lpszClassName = kClass;
        wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
        wc.hbrBackground = nullptr;   // painted in WM_ERASEBKGND

        if (!RegisterClassExW(&wc)) {
            Wh_Log(L"ERROR: Could not register completion alert class: %u",
                   GetLastError());
            return false;
        }

        WNDCLASSEXW card{sizeof(card)};
        card.lpfnWndProc = &AlertWindow::CardProc;
        card.hInstance = module;
        card.lpszClassName = kCardClass;
        card.hCursor = LoadCursorW(nullptr, IDC_ARROW);
        card.hbrBackground = nullptr;

        if (!RegisterClassExW(&card)) {
            Wh_Log(L"ERROR: Could not register alert card class: %u",
                   GetLastError());
            UnregisterClassW(kClass, module);
            return false;
        }

        module_ = module;
        registered_ = true;
        return true;
    }

    static void UnregisterClasses()
    {
        if (!registered_ || !module_) {
            return;
        }

        if (!UnregisterClassW(kCardClass, module_)) {
            Wh_Log(L"ERROR: Could not unregister alert card class: %u",
                   GetLastError());
        }

        if (!UnregisterClassW(kClass, module_)) {
            Wh_Log(L"ERROR: Could not unregister completion alert class: %u",
                   GetLastError());
        }

        registered_ = false;
        module_ = nullptr;
    }

    static bool IsRegistered() { return registered_; }

    HWND Create(int x, int y, int w, int h)
    {
        return CreateWindowExW(
            WS_EX_TOOLWINDOW | WS_EX_TOPMOST, kClass, L"Timer finished",
            WS_POPUP | WS_CLIPCHILDREN, x, y, w, h, nullptr, nullptr, module_,
            this);
    }

    HWND Handle() const { return hwnd_; }

private:
    static constexpr wchar_t kClass[] = L"TaskbarTimerAlarmFinishedAlert";
    static constexpr wchar_t kCardClass[] = L"TaskbarTimerAlarmAlertCard";

    struct Particle {
        float x, y, vx, vy, radius, phase;
        int r, g, b;
    };

    struct Style {
        HFONT body = nullptr;
        HFONT title = nullptr;
        HFONT reminder = nullptr;
        HFONT sub = nullptr;
        HBRUSH background = nullptr;
        HBRUSH edit = nullptr;
    };

    static inline bool registered_ = false;
    static inline HINSTANCE module_ = nullptr;

    // ---- state ---------------------------------------------------------------
    AlertData data_;
    HWND hwnd_ = nullptr;
    HWND card_ = nullptr;
    HWND videoWnd_ = nullptr;
    IMFPMediaPlayer* player_ = nullptr;
    bool small_ = false;

    HBITMAP bg_ = nullptr;
    HBITMAP glass_ = nullptr;     // blurred + tinted card background
    HBITMAP poster_ = nullptr;    // video frame used as blur source
    HBITMAP frameBmp_ = nullptr;
    uint32_t* framePx_ = nullptr;
    const uint32_t* bgPx_ = nullptr;
    int animW_ = 0;
    int animH_ = 0;
    int fade_ = 0;
    unsigned tick_ = 0;
    std::array<Particle, 40> particles_{};
    int particleCount_ = 0;

    Style gdi_;
    bool dark_ = true;
    COLORREF bgColor_ = RGB(32, 32, 32);
    COLORREF editColor_ = RGB(45, 45, 45);
    COLORREF textColor_ = RGB(245, 245, 245);

    // ---- window procedures ---------------------------------------------------
    static AlertWindow* From(HWND hWnd)
    {
        return reinterpret_cast<AlertWindow*>(
            GetWindowLongPtrW(hWnd, GWLP_USERDATA));
    }

    static LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam,
                                    LPARAM lParam)
    {
        if (msg == WM_NCCREATE) {
            auto* create = reinterpret_cast<CREATESTRUCTW*>(lParam);
            SetWindowLongPtrW(hWnd, GWLP_USERDATA,
                              reinterpret_cast<LONG_PTR>(create->lpCreateParams));
            return DefWindowProcW(hWnd, msg, wParam, lParam);
        }

        AlertWindow* self = From(hWnd);

        if (!self) {
            return DefWindowProcW(hWnd, msg, wParam, lParam);
        }

        return self->Handle(hWnd, msg, wParam, lParam);
    }

    static LRESULT CALLBACK CardProc(HWND hWnd, UINT msg, WPARAM wParam,
                                     LPARAM lParam)
    {
        if (msg == WM_NCCREATE) {
            auto* create = reinterpret_cast<CREATESTRUCTW*>(lParam);
            SetWindowLongPtrW(hWnd, GWLP_USERDATA,
                              reinterpret_cast<LONG_PTR>(create->lpCreateParams));
            return DefWindowProcW(hWnd, msg, wParam, lParam);
        }

        AlertWindow* self = From(hWnd);

        if (self) {
            switch (msg) {
            case WM_ERASEBKGND:
                return self->PaintCard(hWnd, reinterpret_cast<HDC>(wParam));

            case WM_CTLCOLORSTATIC:
            case WM_CTLCOLOREDIT:
                return self->CtlColor(msg, wParam, lParam);

            case WM_COMMAND:
                return SendMessageW(GetParent(hWnd), msg, wParam, lParam);
            }
        }

        return DefWindowProcW(hWnd, msg, wParam, lParam);
    }

    LRESULT Handle(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
    {
        switch (msg) {
        case WM_CREATE:
            return OnCreate(hWnd, reinterpret_cast<CREATESTRUCTW*>(lParam));

        case WM_TIMER:
            if (wParam == 1) {
                if (fade_ < 255) {
                    fade_ = std::min(255, fade_ + 28);
                    SetLayeredWindowAttributes(hWnd, 0,
                                               static_cast<BYTE>(fade_),
                                               LWA_ALPHA);
                }

                if (framePx_) {
                    RenderFrame();
                    InvalidateRect(hWnd, nullptr, TRUE);
                }

                return 0;
            }
            break;

        case WM_DPICHANGED:
        {
            const UINT dpi = HIWORD(wParam);
            ApplyTheme(dpi);
            Layout(dpi);
            return 0;
        }

        case WM_ERASEBKGND:
            return PaintBackground(hWnd, reinterpret_cast<HDC>(wParam));

        case WM_THEMECHANGED:
            ApplyTheme(GetDpiForWindow(hWnd));
            Layout(GetDpiForWindow(hWnd));
            return 0;

        case WM_SETTINGCHANGE:
            if (lParam &&
                _wcsicmp(reinterpret_cast<LPCWSTR>(lParam),
                         L"ImmersiveColorSet") == 0)
            {
                ApplyTheme(GetDpiForWindow(hWnd));
                Layout(GetDpiForWindow(hWnd));
            }
            return 0;

        case WM_APP_ALERT_REFRESH:
        {
            std::unique_ptr<AlertData> incoming(
                reinterpret_cast<AlertData*>(lParam));

            if (incoming) {
                // The size of an open alert doesn't change mid-way.
                const bool keepSmall = data_.compactAlert;
                data_ = *incoming;
                data_.compactAlert = keepSmall;

                SetTexts();
                SetSnoozeFields(data_.defaultSnoozeSeconds);
                Layout(GetDpiForWindow(hWnd));
            }

            if (!small_) {
                SetForegroundWindow(hWnd);
            }

            return 0;
        }

        case WM_COMMAND:
            if (OnCommand(hWnd, LOWORD(wParam))) {
                return 0;
            }
            break;

        case WM_CLOSE:
            g_sound.Stop();
            DestroyWindow(hWnd);
            return 0;

        case WM_DESTROY:
            KillTimer(hWnd, 1);
            ReleaseBackground();
            card_ = nullptr;

            if (onDestroyed) {
                onDestroyed();
            }

            PostQuitMessage(0);
            return 0;
        }

        return DefWindowProcW(hWnd, msg, wParam, lParam);
    }

    // ---- creation ------------------------------------------------------------
    LRESULT OnCreate(HWND hWnd, CREATESTRUCTW* create)
    {
        hwnd_ = hWnd;

        UINT dpi = GetDpiForWindow(hWnd);

        if (!dpi) {
            dpi = 96;
        }

        small_ = data_.compactAlert;

        // Background first so the card is created above the video window.
        CreateBackground(create->cx, create->cy);

        card_ = CreateWindowExW(
            WS_EX_CONTROLPARENT, kCardClass, nullptr,
            WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS, 0, 0, 10, 10, hWnd,
            nullptr, module_, this);

        if (!card_) {
            return -1;
        }

        using namespace alert_ids;

        MakeStatic(card_, Title, L"", SS_CENTER);
        MakeStatic(card_, Subtitle, L"", SS_CENTER);
        MakeStatic(card_, Reminder, L"", SS_CENTER);

        MakeStatic(card_, SnoozeLabel, L"Snooze for:");
        MakeEdit(card_, SnoozeHours);
        MakeStatic(card_, UnitH, L"h");
        MakeEdit(card_, SnoozeMinutes);
        MakeStatic(card_, UnitM, L"m");
        MakeEdit(card_, SnoozeSeconds);
        MakeStatic(card_, UnitS, L"s");

        CreateWindowExW(0, L"BUTTON", L"Snooze",
                        WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_DEFPUSHBUTTON,
                        0, 0, 0, 0, card_,
                        reinterpret_cast<HMENU>(static_cast<INT_PTR>(Snooze)),
                        nullptr, nullptr);

        CreateWindowExW(0, L"BUTTON", L"Dismiss",
                        WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
                        0, 0, 0, 0, card_,
                        reinterpret_cast<HMENU>(static_cast<INT_PTR>(Dismiss)),
                        nullptr, nullptr);

        SetTexts();
        SetSnoozeFields(data_.defaultSnoozeSeconds);

        ApplyTheme(dpi);
        Layout(dpi);

        // Fade-in (and animated particles in large mode). Not used with
        // video, where a layered window could break the video renderer.
        if (!videoWnd_) {
            if (!small_) {
                InitAnimation(create->cx, create->cy);
            }

            SetWindowLongPtrW(hWnd, GWL_EXSTYLE,
                              GetWindowLongPtrW(hWnd, GWL_EXSTYLE) |
                                  WS_EX_LAYERED);

            fade_ = 0;
            SetLayeredWindowAttributes(hWnd, 0, 0, LWA_ALPHA);
            SetTimer(hWnd, 1, 33, nullptr);
        }

        return 0;
    }

    static HWND MakeEdit(HWND parent, int id)
    {
        HWND edit = CreateWindowExW(
            0, L"EDIT", L"0",
            WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_NUMBER | ES_CENTER, 0, 0,
            0, 0, parent, reinterpret_cast<HMENU>(static_cast<INT_PTR>(id)),
            nullptr, nullptr);

        if (edit) {
            SendMessageW(edit, EM_LIMITTEXT, 5, 0);
        }

        return edit;
    }

    static HWND MakeStatic(HWND parent, int id, const wchar_t* text,
                           DWORD extraStyle = 0)
    {
        return CreateWindowExW(
            0, L"STATIC", text, WS_CHILD | WS_VISIBLE | SS_NOPREFIX | extraStyle,
            0, 0, 0, 0, parent, reinterpret_cast<HMENU>(static_cast<INT_PTR>(id)),
            nullptr, nullptr);
    }

    // ---- commands ------------------------------------------------------------
    bool OnCommand(HWND hWnd, int id)
    {
        using namespace alert_ids;

        if (id == IDOK) {
            id = Snooze;
        }
        else if (id == IDCANCEL) {
            id = Dismiss;
        }

        if (id == Snooze) {
            if (data_.preview) {
                DestroyWindow(hWnd);
                return true;
            }

            const long long total = ReadField(SnoozeHours) * 3600LL +
                                    ReadField(SnoozeMinutes) * 60LL +
                                    ReadField(SnoozeSeconds);

            if (total <= 0 || total > data_.maximumSeconds) {
                SetText(SnoozeLabel, L"Invalid time:");

                HWND edit = Item(SnoozeMinutes);
                SetFocus(edit);
                SendMessageW(edit, EM_SETSEL, 0, -1);
                MessageBeep(MB_ICONWARNING);
                return true;
            }

            SetText(SnoozeLabel, L"Snooze for:");

            const wchar_t* reminder =
                data_.reminder[0] ? data_.reminder : L"Timer finished";

            // A snooze is a plain countdown timer, even after an alarm.
            if (!g_countdown.Arm(static_cast<int>(total), reminder)) {
                MessageBeep(MB_ICONERROR);
                return true;
            }

            g_sound.Stop();
            DestroyWindow(hWnd);
            return true;
        }

        if (id == Dismiss) {
            g_sound.Stop();
            DestroyWindow(hWnd);
            return true;
        }

        return false;
    }

    // ---- theme / resources ---------------------------------------------------
    static void FreeStyle(Style& s)
    {
        if (s.body) DeleteObject(s.body);
        if (s.title) DeleteObject(s.title);
        if (s.reminder) DeleteObject(s.reminder);
        if (s.sub) DeleteObject(s.sub);
        if (s.background) DeleteObject(s.background);
        if (s.edit) DeleteObject(s.edit);
        s = Style{};
    }

    static HFONT MakeFont(int points, UINT dpi, int weight)
    {
        return CreateFontW(-MulDiv(points, dpi ? dpi : 96, 72), 0, 0, 0, weight,
                           FALSE, FALSE, FALSE, DEFAULT_CHARSET,
                           OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                           CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE,
                           L"Segoe UI");
    }

    Style BuildStyle(UINT dpi)
    {
        const int pref = g_prefs.alertTheme.load();

        dark_ = pref == 1 ? false
                          : pref == 2 ? !util::AppsUseLightTheme() : true;

        bgColor_ = dark_ ? RGB(32, 32, 32) : RGB(249, 249, 249);
        editColor_ = dark_ ? RGB(45, 45, 45) : RGB(255, 255, 255);
        textColor_ = dark_ ? RGB(245, 245, 245) : RGB(25, 25, 25);

        Style s;
        s.background = CreateSolidBrush(bgColor_);
        s.edit = CreateSolidBrush(editColor_);
        s.body = MakeFont(11, dpi, FW_NORMAL);
        s.title = MakeFont(small_ ? 18 : 36, dpi, FW_SEMIBOLD);
        s.reminder = MakeFont(small_ ? 12 : 18, dpi, FW_NORMAL);
        s.sub = MakeFont(small_ ? 10 : 11, dpi, FW_NORMAL);
        return s;
    }

    void ThemeChild(HWND child)
    {
        if (!child) {
            return;
        }

        if (gdi_.body) {
            SendMessageW(child, WM_SETFONT,
                         reinterpret_cast<WPARAM>(gdi_.body), TRUE);
        }

        wchar_t className[32]{};

        if (!GetClassNameW(child, className, ARRAYSIZE(className))) {
            return;
        }

        if (_wcsicmp(className, L"Button") == 0) {
            SetWindowTheme(child, dark_ ? L"DarkMode_Explorer" : L"Explorer",
                           nullptr);
        }
        else if (_wcsicmp(className, L"Edit") == 0) {
            SetWindowTheme(child, dark_ ? L"DarkMode_CFD" : L"Explorer",
                           nullptr);
        }
    }

    void ApplyTheme(UINT dpi)
    {
        Style old = gdi_;
        gdi_ = BuildStyle(dpi);

        // Point every child at the new font/theme before deleting the old GDI
        // resources they may still reference.
        EnumChildWindows(
            hwnd_,
            [](HWND child, LPARAM self) -> BOOL
            {
                reinterpret_cast<AlertWindow*>(self)->ThemeChild(child);
                return TRUE;
            },
            reinterpret_cast<LPARAM>(this));

        const auto setFont = [this](int id, HFONT font)
        {
            if (HWND item = Item(id)) {
                SendMessageW(item, WM_SETFONT, reinterpret_cast<WPARAM>(font),
                             TRUE);
            }
        };

        setFont(alert_ids::Title, gdi_.title);
        setFont(alert_ids::Reminder, gdi_.reminder);
        setFont(alert_ids::Subtitle, gdi_.sub);

        InvalidateRect(hwnd_, nullptr, TRUE);

        if (card_) {
            InvalidateRect(card_, nullptr, TRUE);
        }

        FreeStyle(old);
    }

    // ---- painting ------------------------------------------------------------
    LRESULT PaintBackground(HWND hWnd, HDC hdc)
    {
        RECT rect{};
        GetClientRect(hWnd, &rect);

        HBITMAP source = frameBmp_ ? frameBmp_ : bg_;

        if (source) {
            HDC memory = CreateCompatibleDC(hdc);
            HGDIOBJ old = SelectObject(memory, source);

            BITMAP info{};
            GetObjectW(source, sizeof(info), &info);

            BitBlt(hdc, 0, 0, std::min<int>(rect.right, info.bmWidth),
                   std::min<int>(rect.bottom, info.bmHeight), memory, 0, 0,
                   SRCCOPY);

            SelectObject(memory, old);
            DeleteDC(memory);
        }
        else {
            FillRect(hdc, &rect,
                     static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH)));
        }

        return 1;
    }

    LRESULT PaintCard(HWND hWnd, HDC hdc)
    {
        RECT rect{};
        GetClientRect(hWnd, &rect);

        HBRUSH background =
            gdi_.background ? gdi_.background : GetSysColorBrush(COLOR_WINDOW);

        if (glass_) {
            HDC memory = CreateCompatibleDC(hdc);
            HGDIOBJ old = SelectObject(memory, glass_);

            BitBlt(hdc, 0, 0, rect.right, rect.bottom, memory, 0, 0, SRCCOPY);

            SelectObject(memory, old);
            DeleteDC(memory);
        }
        else {
            FillRect(hdc, &rect, background);
        }

        const UINT dpi = GetDpiForWindow(hWnd);

        // Blue -> violet gradient accent bar.
        const int barHeight = util::Scale(6, dpi);
        const int width = std::max<int>(1, rect.right);

        for (int x = 0; x < rect.right; x++) {
            const float t = static_cast<float>(x) / width;

            HBRUSH accent = CreateSolidBrush(
                RGB(static_cast<int>(0 + 150 * t),
                    static_cast<int>(120 - 30 * t),
                    static_cast<int>(215 + 40 * t)));

            RECT column{x, 0, x + 1, barHeight};
            FillRect(hdc, &column, accent);
            DeleteObject(accent);
        }

        return 1;
    }

    LRESULT CtlColor(UINT msg, WPARAM wParam, LPARAM lParam)
    {
        HDC hdc = reinterpret_cast<HDC>(wParam);

        const bool edit = msg == WM_CTLCOLOREDIT;

        const bool isTitle =
            !edit && GetDlgCtrlID(reinterpret_cast<HWND>(lParam)) ==
                         alert_ids::Title;

        SetTextColor(hdc, isTitle ? (dark_ ? RGB(96, 180, 255) : RGB(0, 95, 184))
                                  : textColor_);

        // Over the frosted glass the labels are transparent so the blur shows.
        if (!edit && glass_) {
            SetBkMode(hdc, TRANSPARENT);
            return reinterpret_cast<LRESULT>(GetStockObject(NULL_BRUSH));
        }

        SetBkColor(hdc, edit ? editColor_ : bgColor_);

        HBRUSH brush = edit ? gdi_.edit : gdi_.background;

        if (!brush) {
            brush = GetSysColorBrush(COLOR_WINDOW);
        }

        return reinterpret_cast<LRESULT>(brush);
    }

    // ---- items / text ----------------------------------------------------------
    HWND Item(int id) const { return card_ ? GetDlgItem(card_, id) : nullptr; }

    // Static text is transparent over the glass, so after changing a label the
    // area behind it has to be repainted or the old text stays visible.
    void SetText(int id, const wchar_t* text)
    {
        HWND item = Item(id);

        if (!item) {
            return;
        }

        SetWindowTextW(item, text);

        if (glass_ && card_) {
            RECT rc{};
            GetWindowRect(item, &rc);
            MapWindowPoints(nullptr, card_, reinterpret_cast<LPPOINT>(&rc), 2);

            RedrawWindow(card_, &rc, nullptr,
                         RDW_INVALIDATE | RDW_ERASE | RDW_ALLCHILDREN |
                             RDW_UPDATENOW);
        }
    }

    void SetSnoozeFields(int totalSeconds)
    {
        if (totalSeconds < 0) {
            totalSeconds = 0;
        }

        wchar_t text[16]{};

        swprintf_s(text, L"%d", totalSeconds / 3600);
        SetWindowTextW(Item(alert_ids::SnoozeHours), text);

        swprintf_s(text, L"%d", (totalSeconds % 3600) / 60);
        SetWindowTextW(Item(alert_ids::SnoozeMinutes), text);

        swprintf_s(text, L"%d", totalSeconds % 60);
        SetWindowTextW(Item(alert_ids::SnoozeSeconds), text);
    }

    int ReadField(int id) const
    {
        wchar_t text[16]{};
        GetWindowTextW(Item(id), text, ARRAYSIZE(text));
        return _wtoi(text);
    }

    void SetTexts()
    {
        using namespace alert_ids;

        const bool preview = data_.preview;
        const bool alarm = data_.alarm && !preview;

        SetText(Title, preview ? L"Preview" : (alarm ? L"Alarm" : L"Time's up"));

        if (preview) {
            SetText(Subtitle, L"Your timer alert");
        }
        else {
            SYSTEMTIME now{};
            GetLocalTime(&now);

            wchar_t text[64]{};

            // 12-hour clock with AM / PM.
            const int hour12 = now.wHour % 12 == 0 ? 12 : now.wHour % 12;
            const wchar_t* suffix = now.wHour >= 12 ? L"PM" : L"AM";

            if (alarm) {
                swprintf_s(text, L"Alarm at %d:%02d %s", hour12, now.wMinute,
                           suffix);
            }
            else {
                swprintf_s(text, L"Finished at %d:%02d %s", hour12,
                           now.wMinute, suffix);
            }

            SetText(Subtitle, text);
        }

        SetText(Reminder, data_.reminder[0]
                              ? data_.reminder
                              : (alarm ? L"Alarm" : L"Timer finished"));

        SetWindowTextW(Item(Dismiss), preview ? L"Close preview" : L"Dismiss");
    }

    // ---- layout ----------------------------------------------------------------
    void Layout(UINT dpi)
    {
        using namespace alert_ids;

        if (!card_) {
            return;
        }

        RECT client{};
        GetClientRect(hwnd_, &client);

        const int W = client.right;
        const int H = client.bottom;

        if (videoWnd_) {
            SetWindowPos(videoWnd_, HWND_BOTTOM, 0, 0, W, H, SWP_NOACTIVATE);

            if (player_) {
                player_->UpdateVideo();
            }
        }

        const bool preview = data_.preview;

        const int cardW = small_ ? W : std::min(util::Scale(580, dpi), W);
        const int cardH = small_ ? H : std::min(util::Scale(400, dpi), H);
        const int cardX = (W - cardW) / 2;
        const int cardY = (H - cardH) / 2;
        const int radius = util::Scale(small_ ? 20 : 28, dpi);

        SetWindowPos(card_, nullptr, cardX, cardY, cardW, cardH,
                     SWP_NOZORDER | SWP_NOACTIVATE);

        SetWindowRgn(card_,
                     CreateRoundRectRgn(0, 0, cardW + 1, cardH + 1, radius,
                                        radius),
                     TRUE);

        if (small_) {
            // The small alert is the card itself, so round the window too.
            SetWindowRgn(hwnd_,
                         CreateRoundRectRgn(0, 0, W + 1, H + 1, radius, radius),
                         TRUE);
        }

        struct Entry {
            int id, x, y, w, h;
        };

        static const Entry kLarge[] = {
            {Title,          30,  38, 520, 64},
            {Subtitle,       30, 104, 520, 24},
            {Reminder,       40, 148, 500, 72},
            {SnoozeLabel,   123, 252, 100, 22},
            {SnoozeHours,   229, 246,  52, 32},
            {UnitH,         285, 252,  16, 22},
            {SnoozeMinutes, 307, 246,  52, 32},
            {UnitM,         363, 252,  18, 22},
            {SnoozeSeconds, 385, 246,  52, 32},
            {UnitS,         441, 252,  16, 22},
            {Snooze,         40, 322, 240, 48},
            {Dismiss,       300, 322, 240, 48},
        };

        // Small notification: card is 400 x 216 (at 96 dpi).
        static const Entry kSmall[] = {
            {Title,          20,  14, 360, 36},
            {Subtitle,       20,  50, 360, 20},
            {Reminder,       24,  76, 352, 44},
            {SnoozeLabel,    59, 130,  90, 22},
            {SnoozeHours,   153, 125,  44, 30},
            {UnitH,         199, 130,  14, 22},
            {SnoozeMinutes, 217, 125,  44, 30},
            {UnitM,         263, 130,  16, 22},
            {SnoozeSeconds, 281, 125,  44, 30},
            {UnitS,         327, 130,  14, 22},
            {Snooze,         20, 168, 176, 36},
            {Dismiss,       204, 168, 176, 36},
        };

        const Entry* entries = small_ ? kSmall : kLarge;
        const size_t count = small_ ? ARRAYSIZE(kSmall) : ARRAYSIZE(kLarge);

        for (size_t i = 0; i < count; i++) {
            const Entry& e = entries[i];
            HWND child = Item(e.id);

            if (!child) {
                continue;
            }

            int x = e.x;
            int w = e.w;

            const bool snoozeRelated =
                e.id == SnoozeLabel || e.id == SnoozeHours ||
                e.id == SnoozeMinutes || e.id == SnoozeSeconds ||
                e.id == UnitH || e.id == UnitM || e.id == UnitS ||
                e.id == Snooze;

            if (preview && e.id == Dismiss) {
                x = small_ ? 20 : 150;
                w = small_ ? 360 : 280;
            }

            ShowWindow(child, preview && snoozeRelated ? SW_HIDE : SW_SHOWNA);

            SetWindowPos(child, nullptr, util::Scale(x, dpi),
                         util::Scale(e.y, dpi), util::Scale(w, dpi),
                         util::Scale(e.h, dpi), SWP_NOZORDER | SWP_NOACTIVATE);
        }

        BuildGlass(cardX, cardY, cardW, cardH, dpi);

        RedrawWindow(card_, nullptr, nullptr,
                     RDW_INVALIDATE | RDW_ERASE | RDW_ALLCHILDREN);
    }

    // ---- background media --------------------------------------------------------
    bool StartVideo(const Options& options, int w, int h)
    {
        HMODULE mfplay = LoadLibraryW(L"mfplay.dll");

        auto create =
            mfplay ? reinterpret_cast<gfx::MFPCreateMediaPlayer_t>(
                         GetProcAddress(mfplay, "MFPCreateMediaPlayer"))
                   : nullptr;

        if (!create) {
            Wh_Log(L"mfplay.dll / MFPCreateMediaPlayer not available");
            return false;
        }

        HWND videoWnd = CreateWindowExW(
            0, L"STATIC", nullptr,
            WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | SS_BLACKRECT, 0, 0, w, h,
            hwnd_, nullptr, nullptr, nullptr);

        if (!videoWnd) {
            return false;
        }

        auto* callback =
            new (std::nothrow) gfx::VideoCallback(!options.videoSound);

        if (!callback) {
            DestroyWindow(videoWnd);
            return false;
        }

        IMFPMediaPlayer* player = nullptr;

        const HRESULT hr =
            create(options.customBackgroundPath.c_str(), TRUE, MFP_OPTION_NONE,
                   callback, videoWnd, &player);

        callback->Release();   // the player holds its own reference

        if (FAILED(hr) || !player) {
            Wh_Log(L"MFPCreateMediaPlayer failed: 0x%08X",
                   static_cast<unsigned>(hr));
            DestroyWindow(videoWnd);
            return false;
        }

        videoWnd_ = videoWnd;
        player_ = player;
        return true;
    }

    void CreateBackground(int w, int h)
    {
        const Options options = OptionStore::Snapshot();
        const std::wstring& path = options.customBackgroundPath;

        bool videoStarted = false;

        if (!path.empty()) {
            if (GetFileAttributesW(path.c_str()) == INVALID_FILE_ATTRIBUTES) {
                Wh_Log(L"Background file not found: %s", path.c_str());
            }
            else if (util::IsVideoFile(path)) {
                // The small notification is covered by its card, so a video
                // would never be visible there: use the gradient instead.
                if (!small_) {
                    videoStarted = StartVideo(options, w, h);

                    // Frosted glass option: blur a video frame behind the card.
                    if (videoStarted && g_prefs.flyoutGlass.load()) {
                        poster_ = gfx::LoadVideoPoster(path, w, h);
                    }
                }
            }
            else if (util::IsImageFile(path)) {
                bg_ = gfx::LoadCoverBitmap(path, w, h, options.backgroundDim);
            }
        }

        if (!videoStarted && !bg_) {
            bg_ = gfx::CreateDefaultBackground(w, h);
        }
    }

    void ReleaseBackground()
    {
        if (player_) {
            player_->Shutdown();
            player_->Release();
            player_ = nullptr;
        }

        videoWnd_ = nullptr;   // destroyed together with the parent

        const auto drop = [](HBITMAP& bmp)
        {
            if (bmp) {
                DeleteObject(bmp);
                bmp = nullptr;
            }
        };

        drop(frameBmp_);
        framePx_ = nullptr;
        bgPx_ = nullptr;
        particleCount_ = 0;

        drop(glass_);
        drop(bg_);
        drop(poster_);
    }

    // ---- animated orbs (gradient / image backgrounds, large mode) ---------------
    void InitAnimation(int w, int h)
    {
        if (!bg_) {
            return;
        }

        DIBSECTION section{};

        if (!GetObjectW(bg_, sizeof(section), &section) || !section.dsBm.bmBits) {
            return;
        }

        bgPx_ = static_cast<const uint32_t*>(section.dsBm.bmBits);

        frameBmp_ = gfx::CreateDib(w, h, &framePx_);

        if (!frameBmp_ || !framePx_) {
            framePx_ = nullptr;
            bgPx_ = nullptr;
            return;
        }

        animW_ = w;
        animH_ = h;

        static const int palette[3][3] = {
            {70, 140, 255}, {160, 90, 255}, {50, 210, 230}};

        const float unit = h / 1080.0f;
        particleCount_ = static_cast<int>(particles_.size());

        for (int i = 0; i < particleCount_; i++) {
            Particle& p = particles_[i];
            const int* c = palette[i % 3];

            p.x = static_cast<float>(rand() % std::max(1, w));
            p.y = static_cast<float>(rand() % std::max(1, h));
            p.vx = ((rand() % 100) / 100.0f - 0.5f) * 0.6f * unit;
            p.vy = -(0.25f + (rand() % 100) / 100.0f * 0.9f) * unit;
            p.radius = (28 + rand() % 70) * unit;
            p.phase = (rand() % 628) / 100.0f;
            p.r = c[0];
            p.g = c[1];
            p.b = c[2];
        }
    }

    void RenderFrame()
    {
        if (!framePx_ || !bgPx_) {
            return;
        }

        const int w = animW_;
        const int h = animH_;

        memcpy(framePx_, bgPx_, static_cast<size_t>(w) * h * 4);

        tick_++;
        const float t = tick_ * 0.05f;

        for (int i = 0; i < particleCount_; i++) {
            Particle& p = particles_[i];

            p.x += p.vx + std::sin(t + p.phase) * 0.35f;
            p.y += p.vy;

            if (p.y < -p.radius) {
                p.y = h + p.radius;
                p.x = static_cast<float>(rand() % std::max(1, w));
            }

            if (p.x < -p.radius) p.x = w + p.radius;
            if (p.x > w + p.radius) p.x = -p.radius;

            const float pulse = 0.55f + 0.45f * std::sin(t * 1.3f + p.phase);
            const float intensity = 0.42f * pulse;
            const int rad = static_cast<int>(p.radius);
            const float r2 = p.radius * p.radius;

            const int x0 = std::max(0, static_cast<int>(p.x) - rad);
            const int x1 = std::min(w - 1, static_cast<int>(p.x) + rad);
            const int y0 = std::max(0, static_cast<int>(p.y) - rad);
            const int y1 = std::min(h - 1, static_cast<int>(p.y) + rad);

            for (int y = y0; y <= y1; y++) {
                const float dy = y - p.y;
                uint32_t* row = framePx_ + static_cast<size_t>(y) * w;

                for (int x = x0; x <= x1; x++) {
                    const float dx = x - p.x;
                    const float d2 = dx * dx + dy * dy;

                    if (d2 >= r2) {
                        continue;
                    }

                    float f = 1.0f - d2 / r2;
                    f = f * f * intensity;

                    const uint32_t px = row[x];
                    const int r = static_cast<int>((px >> 16) & 0xFF) +
                                  static_cast<int>(p.r * f);
                    const int g = static_cast<int>((px >> 8) & 0xFF) +
                                  static_cast<int>(p.g * f);
                    const int b = static_cast<int>(px & 0xFF) +
                                  static_cast<int>(p.b * f);

                    row[x] = (static_cast<uint32_t>(std::min(r, 255)) << 16) |
                             (static_cast<uint32_t>(std::min(g, 255)) << 8) |
                             static_cast<uint32_t>(std::min(b, 255));
                }
            }
        }
    }

    // ---- frosted glass ------------------------------------------------------------
    // Blurs the part of the background under the card and tints it.
    void BuildGlass(int cx, int cy, int cw, int ch, UINT dpi)
    {
        if (glass_) {
            DeleteObject(glass_);
            glass_ = nullptr;
        }

        // Image / gradient, or a video frame when the frosted option is on.
        // Without either the card stays solid.
        HBITMAP sourceBmp = bg_ ? bg_ : poster_;

        if (!sourceBmp || cw <= 0 || ch <= 0) {
            return;
        }

        DIBSECTION section{};

        if (!GetObjectW(sourceBmp, sizeof(section), &section) ||
            !section.dsBm.bmBits)
        {
            return;
        }

        const uint32_t* bg = static_cast<const uint32_t*>(section.dsBm.bmBits);
        const int bw = section.dsBm.bmWidth;
        const int bh = section.dsBm.bmHeight;

        std::vector<uint32_t> a(static_cast<size_t>(cw) * ch);
        std::vector<uint32_t> b(a.size());

        for (int y = 0; y < ch; y++) {
            const int sy = std::clamp(cy + y, 0, bh - 1);

            for (int x = 0; x < cw; x++) {
                const int sx = std::clamp(cx + x, 0, bw - 1);

                a[static_cast<size_t>(y) * cw + x] =
                    bg[static_cast<size_t>(sy) * bw + sx];
            }
        }

        const int r = std::max(2, util::Scale(14, dpi));

        gfx::BoxBlurPass(a.data(), b.data(), cw, ch, r, true);
        gfx::BoxBlurPass(b.data(), a.data(), cw, ch, r, false);
        gfx::BoxBlurPass(a.data(), b.data(), cw, ch, r, true);
        gfx::BoxBlurPass(b.data(), a.data(), cw, ch, r, false);

        const Options options = OptionStore::Snapshot();

        // The light look lifts the blurred pixels toward white first and then
        // uses a moderate tint, so the card stays bright but the blur remains
        // visible (a heavy white tint would hide it completely).
        const int alpha = dark_ ? std::clamp(options.glassTint, 0, 90)
                                : std::clamp(options.glassTint + 10, 30, 70);

        const int tr = dark_ ? 22 : 250;
        const int tg = dark_ ? 24 : 250;
        const int tb = dark_ ? 32 : 252;

        uint32_t* out = nullptr;
        HBITMAP glass = gfx::CreateDib(cw, ch, &out);

        if (!glass || !out) {
            if (glass) {
                DeleteObject(glass);
            }

            return;
        }

        for (size_t i = 0; i < a.size(); i++) {
            const uint32_t p = a[i];
            int r0 = (p >> 16) & 0xFF;
            int g0 = (p >> 8) & 0xFF;
            int b0 = p & 0xFF;

            if (!dark_) {
                // Lift toward white so the frosted look stays bright.
                r0 += (255 - r0) * 35 / 100;
                g0 += (255 - g0) * 35 / 100;
                b0 += (255 - b0) * 35 / 100;
            }

            out[i] =
                (static_cast<uint32_t>((r0 * (100 - alpha) + tr * alpha) / 100) << 16) |
                (static_cast<uint32_t>((g0 * (100 - alpha) + tg * alpha) / 100) << 8) |
                static_cast<uint32_t>((b0 * (100 - alpha) + tb * alpha) / 100);
        }

        glass_ = glass;
    }
};


// =============================================================================
// AlertHost: owns the alert thread (open / refresh / replace / close)
// =============================================================================

[[clang::no_destroy]] inline std::atomic<HWND> g_taskbarWnd{nullptr};

class AlertHost {
public:
    bool Initialize()
    {
        stopEvent_ = CreateEventW(nullptr, TRUE, FALSE, nullptr);
        return stopEvent_ != nullptr;
    }

    void Dispose()
    {
        if (stopEvent_) {
            CloseHandle(stopEvent_);
            stopEvent_ = nullptr;
        }
    }

    bool Show(const wchar_t* reminder, bool preview = false)
    {
        AcquireSRWLockExclusive(&lock_);

        bool result = false;

        if (!g_unloading.load()) {
            result = ShowLocked(reminder, preview);
        }

        ReleaseSRWLockExclusive(&lock_);
        return result;
    }

    // Closes the alert (if any) and joins its thread.
    void Shutdown()
    {
        AcquireSRWLockExclusive(&lock_);

        if (stopEvent_) {
            SetEvent(stopEvent_);
        }

        if (HWND alert = hwnd_.load()) {
            PostMessageW(alert, WM_CLOSE, 0, 0);
        }

        HANDLE thread = thread_.exchange(nullptr);

        ReleaseSRWLockExclusive(&lock_);

        if (thread) {
            util::PumpWaitForThread(thread);
            CloseHandle(thread);
        }
    }

private:
    struct Launch {
        AlertHost* host;
        AlertData data;
    };

    void ReapExitedThread()
    {
        HANDLE thread = thread_.load();

        if (!thread || WaitForSingleObject(thread, 0) != WAIT_OBJECT_0) {
            return;
        }

        if (thread_.compare_exchange_strong(thread, nullptr)) {
            CloseHandle(thread);
        }
    }

    bool ShowLocked(const wchar_t* reminder, bool preview)
    {
        if (!AlertWindow::IsRegistered() && !AlertWindow::RegisterClasses()) {
            Wh_Log(L"ERROR: Completion alert class isn't available");
            return false;
        }

        ReapExitedThread();

        if (HWND existing = hwnd_.load()) {
            if (IsWindow(existing) &&
                (preview || openSmall_.load() != g_prefs.alertSmall.load() ||
                 openTheme_.load() != g_prefs.alertTheme.load()))
            {
                // Look or size changed (or a preview was requested): close the
                // open alert and build a new one so the change is applied.
                PostMessageW(existing, WM_CLOSE, 0, 0);

                HANDLE oldThread = thread_.exchange(nullptr);

                if (oldThread) {
                    WaitForSingleObject(oldThread, 3000);
                    CloseHandle(oldThread);
                }
            }
            else if (IsWindow(existing)) {
                auto* refresh = new (std::nothrow)
                    AlertData(MakeAlertData(reminder, preview));

                if (refresh &&
                    !PostMessageW(existing, WM_APP_ALERT_REFRESH, 0,
                                  reinterpret_cast<LPARAM>(refresh)))
                {
                    delete refresh;
                }

                return true;
            }
        }

        HANDLE existingThread = thread_.load();

        if (existingThread &&
            WaitForSingleObject(existingThread, 0) != WAIT_OBJECT_0)
        {
            return true;
        }

        if (!stopEvent_) {
            Wh_Log(L"ERROR: Completion alert stop event isn't available");
            return false;
        }

        ResetEvent(stopEvent_);

        auto launch = std::make_unique<Launch>(
            Launch{this, MakeAlertData(reminder, preview)});

        HANDLE thread = CreateThread(nullptr, 0, &AlertHost::ThreadMain,
                                     launch.get(), 0, nullptr);

        if (!thread) {
            Wh_Log(L"ERROR: Could not create completion alert thread");
            return false;
        }

        launch.release();
        thread_.store(thread);
        return true;
    }

    static DWORD WINAPI ThreadMain(LPVOID param)
    {
        std::unique_ptr<Launch> launch(static_cast<Launch*>(param));
        AlertHost* host = launch->host;

        if (!AlertWindow::IsRegistered()) {
            Wh_Log(L"ERROR: Completion alert class isn't registered");
            return 0;
        }

        // WIC and Media Foundation need COM on this thread.
        const bool com =
            SUCCEEDED(CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED));

        HWND taskbar = g_taskbarWnd.load();

        HMONITOR monitor = MonitorFromWindow(
            taskbar ? taskbar : GetDesktopWindow(), MONITOR_DEFAULTTOPRIMARY);

        MONITORINFO info{sizeof(info)};

        RECT area{0, 0, GetSystemMetrics(SM_CXSCREEN),
                  GetSystemMetrics(SM_CYSCREEN)};
        RECT work = area;

        if (GetMonitorInfoW(monitor, &info)) {
            area = info.rcMonitor;
            work = info.rcWork;
        }

        const bool compact = launch->data.compactAlert;

        int winX = area.left;
        int winY = area.top;
        int winW = area.right - area.left;
        int winH = area.bottom - area.top;

        if (compact) {
            // Compact notification in the bottom-right corner of the work area.
            const UINT sysDpi = GetDpiForSystem();

            winW = util::Scale(400, sysDpi);
            winH = util::Scale(216, sysDpi);

            const int margin = util::Scale(16, sysDpi);

            winX = work.right - winW - margin;
            winY = work.bottom - winH - margin;
        }

        {
            AlertWindow window(launch->data);
            window.onDestroyed = [host] { host->hwnd_.store(nullptr); };

            HWND hWnd = window.Create(winX, winY, winW, winH);

            if (!hWnd) {
                Wh_Log(L"ERROR: Could not create completion alert: %u",
                       GetLastError());

                if (com) {
                    CoUninitialize();
                }

                return 0;
            }

            host->hwnd_.store(hWnd);
            host->openSmall_.store(compact);
            host->openTheme_.store(g_prefs.alertTheme.load());

            SetWindowPos(hWnd, HWND_TOPMOST, winX, winY, winW, winH,
                         SWP_SHOWWINDOW | (compact ? SWP_NOACTIVATE : 0));

            // The small notification must not steal focus from typing.
            if (!compact) {
                SetForegroundWindow(hWnd);
            }

            MSG msg{};
            bool quit = false;

            while (!quit && !g_unloading.load()) {
                const DWORD wait = MsgWaitForMultipleObjects(
                    1, &host->stopEvent_, FALSE, INFINITE, QS_ALLINPUT);

                if (wait != WAIT_OBJECT_0 + 1) {
                    break;   // stop event (or error)
                }

                while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
                    if (msg.message == WM_QUIT) {
                        quit = true;
                        break;
                    }

                    if (!IsDialogMessageW(hWnd, &msg)) {
                        TranslateMessage(&msg);
                        DispatchMessageW(&msg);
                    }
                }
            }

            MSG pending{};

            while (PeekMessageW(&pending, hWnd, WM_APP_ALERT_REFRESH,
                                WM_APP_ALERT_REFRESH, PM_REMOVE))
            {
                delete reinterpret_cast<AlertData*>(pending.lParam);
            }

            if (IsWindow(hWnd)) {
                DestroyWindow(hWnd);
            }

            host->hwnd_.store(nullptr);
        }   // AlertWindow released here (fonts, brushes, bitmaps)

        if (com) {
            CoUninitialize();
        }

        return 0;
    }

    SRWLOCK lock_ = SRWLOCK_INIT;
    HANDLE stopEvent_ = nullptr;
    std::atomic<HWND> hwnd_{nullptr};
    std::atomic<HANDLE> thread_{nullptr};
    std::atomic_bool openSmall_{false};
    std::atomic<int> openTheme_{0};
};

inline AlertHost g_alerts;


// =============================================================================
// host: taskbar.dll / SystemTray hooks, XAML root, UI-thread marshalling
// =============================================================================

namespace host {

inline std::atomic<DWORD> g_taskbarThreadId{0};
inline std::atomic_bool g_systemTrayHooked{false};
inline std::atomic<HMODULE> g_systemTrayAttempted{nullptr};
inline std::atomic_bool g_taskbarDllHooked{false};
inline std::atomic<HMODULE> g_taskbarDllAttempted{nullptr};

// Loaded-event revokers for IconView instances seen while the mod is active.
[[clang::no_destroy]] inline std::optional<
    std::list<FrameworkElement::Loaded_revoker>>
    g_loadedRevokers{std::in_place};

inline HWND FindCurrentProcessTaskbarWnd()
{
    HWND result = nullptr;

    EnumWindows(
        [](HWND hWnd, LPARAM lParam) -> BOOL
        {
            DWORD pid = 0;
            WCHAR className[32];

            if (GetWindowThreadProcessId(hWnd, &pid) &&
                pid == GetCurrentProcessId() &&
                GetClassNameW(hWnd, className, ARRAYSIZE(className)) &&
                _wcsicmp(className, L"Shell_TrayWnd") == 0)
            {
                *reinterpret_cast<HWND*>(lParam) = hWnd;
                return FALSE;
            }

            return TRUE;
        },
        reinterpret_cast<LPARAM>(&result));

    if (result) {
        const DWORD threadId = GetWindowThreadProcessId(result, nullptr);

        if (threadId) {
            g_taskbarThreadId.store(threadId);
        }
    }

    return result;
}

inline HWND FindAnyWindowOnTaskbarThread()
{
    const DWORD threadId = g_taskbarThreadId.load();

    if (!threadId) {
        return nullptr;
    }

    HWND result = nullptr;

    EnumThreadWindows(
        threadId,
        [](HWND hWnd, LPARAM lParam) -> BOOL
        {
            *reinterpret_cast<HWND*>(lParam) = hWnd;
            return FALSE;
        },
        reinterpret_cast<LPARAM>(&result));

    return result;
}

// Cached taskbar window, re-discovered when it went away.
inline HWND ResolveTaskbar()
{
    HWND taskbar = g_taskbarWnd.load();

    if (!taskbar || !IsWindow(taskbar)) {
        taskbar = FindCurrentProcessTaskbarWnd();

        if (taskbar) {
            g_taskbarWnd.store(taskbar);
        }
    }

    return taskbar;
}

// ---- taskbar.dll symbols --------------------------------------------------------

using CTaskBand_GetTaskbarHost_t = void* (WINAPI*)(void* pThis, void* result);
inline CTaskBand_GetTaskbarHost_t CTaskBand_GetTaskbarHost_Original = nullptr;

using TaskbarHost_FrameHeight_t = int (WINAPI*)(void* pThis);
inline TaskbarHost_FrameHeight_t TaskbarHost_FrameHeight_Original = nullptr;

using std__Ref_count_base__Decref_t = void (WINAPI*)(void* pThis);
inline std__Ref_count_base__Decref_t std__Ref_count_base__Decref_Original =
    nullptr;

inline void* CTaskBand_ITaskListWndSite_vftable = nullptr;

using TrayUI_StartTaskbar_t = void(WINAPI*)(void* pThis);
inline TrayUI_StartTaskbar_t TrayUI_StartTaskbar_Original = nullptr;

inline void WINAPI TrayUI_StartTaskbar_Hook(void* pThis)
{
    if (TrayUI_StartTaskbar_Original) {
        TrayUI_StartTaskbar_Original(pThis);
    }

    if (!g_unloading.load()) {
        OnTaskbarStarted();
    }
}

inline bool HookTaskbarDll(HMODULE module)
{
    if (!module) {
        return false;
    }

    WindhawkUtils::SYMBOL_HOOK taskbarDllHooks[] = {
        {
            {LR"(public: virtual void __cdecl TrayUI::StartTaskbar(void))"},
            &TrayUI_StartTaskbar_Original,
            TrayUI_StartTaskbar_Hook,
        },
        {
            {LR"(const CTaskBand::`vftable'{for `ITaskListWndSite'})"},
            &CTaskBand_ITaskListWndSite_vftable,
        },
        {
            {LR"(public: virtual class std::shared_ptr<class TaskbarHost> __cdecl CTaskBand::GetTaskbarHost(void)const )"},
            &CTaskBand_GetTaskbarHost_Original,
        },
        {
            {LR"(public: int __cdecl TaskbarHost::FrameHeight(void)const )"},
            &TaskbarHost_FrameHeight_Original,
        },
        {
            {LR"(public: void __cdecl std::_Ref_count_base::_Decref(void))"},
            &std__Ref_count_base__Decref_Original,
        },
    };

    return WindhawkUtils::HookSymbols(module, taskbarDllHooks, ARRAYSIZE(taskbarDllHooks));
}

// ---- taskbar XAML root -------------------------------------------------------------

inline XamlRoot GetTaskbarXamlRoot(HWND hTaskbarWnd)
{
    if (!CTaskBand_GetTaskbarHost_Original ||
        !TaskbarHost_FrameHeight_Original ||
        !std__Ref_count_base__Decref_Original ||
        !CTaskBand_ITaskListWndSite_vftable)
    {
        Wh_Log(L"ERROR: Required symbols are missing");
        return nullptr;
    }

    HWND hTaskSwWnd =
        reinterpret_cast<HWND>(GetPropW(hTaskbarWnd, L"TaskbandHWND"));

    if (!hTaskSwWnd) {
        Wh_Log(L"ERROR: TaskbandHWND not found");
        return nullptr;
    }

    void* taskBand =
        reinterpret_cast<void*>(GetWindowLongPtrW(hTaskSwWnd, 0));

    if (!taskBand) {
        Wh_Log(L"ERROR: CTaskBand object not found");
        return nullptr;
    }

    void* taskBandForSite = taskBand;

    for (int i = 0; *reinterpret_cast<void**>(taskBandForSite) !=
                    CTaskBand_ITaskListWndSite_vftable;
         i++)
    {
        if (i == 20) {
            Wh_Log(L"ERROR: ITaskListWndSite vftable not found");
            return nullptr;
        }

        taskBandForSite = reinterpret_cast<void**>(taskBandForSite) + 1;
    }

    void* taskbarHostSharedPtr[2]{};

    CTaskBand_GetTaskbarHost_Original(taskBandForSite, taskbarHostSharedPtr);

    if (!taskbarHostSharedPtr[0] || !taskbarHostSharedPtr[1]) {
        if (taskbarHostSharedPtr[1]) {
            std__Ref_count_base__Decref_Original(taskbarHostSharedPtr[1]);
        }

        Wh_Log(L"ERROR: TaskbarHost not obtained");
        return nullptr;
    }

    size_t offset;

#if defined(_M_X64)
    const BYTE* code =
        reinterpret_cast<const BYTE*>(TaskbarHost_FrameHeight_Original);

    if (code[0] == 0x48 && code[1] == 0x83 && code[2] == 0xEC &&
        code[4] == 0x48 && code[5] == 0x83 && code[6] == 0xC1 &&
        code[7] <= 0x7F)
    {
        offset = code[7];
    }
    else {
        Wh_Log(L"Unsupported TaskbarHost::FrameHeight");
        std__Ref_count_base__Decref_Original(taskbarHostSharedPtr[1]);
        return nullptr;
    }
#elif defined(_M_ARM64)
    const DWORD* p =
        reinterpret_cast<const DWORD*>(TaskbarHost_FrameHeight_Original);

    if (p[0] == 0xD503237F && (p[1] & 0xFFC07FFF) == 0xA9807BFD &&
        p[2] == 0x910003FD && (p[3] & 0xFFF00FE0) == 0xF8400C00)
    {
        offset = (p[3] >> 12) & 0xFF;
    }
    else {
        Wh_Log(L"Unsupported TaskbarHost::FrameHeight");
        std__Ref_count_base__Decref_Original(taskbarHostSharedPtr[1]);
        return nullptr;
    }
#else
#error "Unsupported architecture"
#endif

    auto* unknown = *reinterpret_cast<IUnknown**>(
        reinterpret_cast<BYTE*>(taskbarHostSharedPtr[0]) + offset);

    if (!unknown) {
        Wh_Log(L"ERROR: Taskbar XAML object not found");
        std__Ref_count_base__Decref_Original(taskbarHostSharedPtr[1]);
        return nullptr;
    }

    FrameworkElement taskbarElement = nullptr;

    unknown->QueryInterface(winrt::guid_of<FrameworkElement>(),
                            winrt::put_abi(taskbarElement));

    auto result = taskbarElement ? taskbarElement.XamlRoot() : nullptr;

    std__Ref_count_base__Decref_Original(taskbarHostSharedPtr[1]);

    return result;
}

inline FrameworkElement FindChildRecursive(
    FrameworkElement element,
    const std::function<bool(FrameworkElement)>& callback,
    int maxDepth = 20)
{
    if (!element || maxDepth <= 0) {
        return nullptr;
    }

    const int count = VisualTreeHelper::GetChildrenCount(element);

    for (int i = 0; i < count; i++) {
        auto child =
            VisualTreeHelper::GetChild(element, i).try_as<FrameworkElement>();

        if (!child) {
            continue;
        }

        if (callback(child)) {
            return child;
        }

        if (auto found = FindChildRecursive(child, callback, maxDepth - 1)) {
            return found;
        }
    }

    return nullptr;
}

// ---- run code on the taskbar's XAML thread ------------------------------------------

using ThreadProc_t = void (*)(void*);

inline bool RunOnWindowThread(HWND hWnd, ThreadProc_t proc, void* procParam)
{
    static const UINT message =
        RegisterWindowMessageW(L"Windhawk_RunFromWindowThread_" WH_MOD_ID);

    struct Param {
        ThreadProc_t proc;
        void* procParam;
        bool invoked;
    };

    const DWORD threadId = GetWindowThreadProcessId(hWnd, nullptr);

    if (!threadId) {
        return false;
    }

    if (threadId == GetCurrentThreadId()) {
        proc(procParam);
        return true;
    }

    HHOOK hook = SetWindowsHookExW(
        WH_CALLWNDPROC,
        [](int nCode, WPARAM wParam, LPARAM lParam) -> LRESULT
        {
            if (nCode == HC_ACTION) {
                const CWPSTRUCT* cwp = reinterpret_cast<const CWPSTRUCT*>(lParam);

                if (cwp->message == message) {
                    auto* param = reinterpret_cast<Param*>(cwp->lParam);

                    if (!param->invoked) {
                        param->proc(param->procParam);
                        param->invoked = true;
                    }
                }
            }

            return CallNextHookEx(nullptr, nCode, wParam, lParam);
        },
        nullptr, threadId);

    if (!hook) {
        return false;
    }

    Param param{proc, procParam, false};

    SendMessageW(hWnd, message, 0, reinterpret_cast<LPARAM>(&param));

    UnhookWindowsHookEx(hook);

    return param.invoked;
}

// ---- system tray rebuild detection ---------------------------------------------------

using IconView_IconView_t = void* (WINAPI*)(void* pThis);
inline IconView_IconView_t IconView_IconView_Original = nullptr;

using LoadLibraryExW_t = HMODULE(WINAPI*)(LPCWSTR, HANDLE, DWORD);
inline LoadLibraryExW_t LoadLibraryExW_Original = nullptr;

inline VS_FIXEDFILEINFO* GetModuleVersionInfo(HMODULE module)
{
    HRSRC resource =
        FindResourceW(module, MAKEINTRESOURCEW(VS_VERSION_INFO), RT_VERSION);

    if (!resource) {
        return nullptr;
    }

    HGLOBAL loaded = LoadResource(module, resource);

    if (!loaded) {
        return nullptr;
    }

    void* data = LockResource(loaded);

    if (!data) {
        return nullptr;
    }

    void* fixedInfo = nullptr;
    UINT fixedInfoSize = 0;

    if (!VerQueryValueW(data, L"\\", &fixedInfo, &fixedInfoSize) ||
        !fixedInfoSize)
    {
        return nullptr;
    }

    return reinterpret_cast<VS_FIXEDFILEINFO*>(fixedInfo);
}

inline HMODULE GetSystemTrayModuleHandle()
{
    if (HMODULE module = GetModuleHandleW(L"SystemTray.dll")) {
        return module;
    }

    if (HMODULE module = GetModuleHandleW(L"Taskbar.View.dll")) {
        // First known Taskbar.View.dll version without SystemTray symbols:
        // 2604.8002.200.6000. Newer builds use SystemTray.dll instead.
        VS_FIXEDFILEINFO* fixedInfo = GetModuleVersionInfo(module);

        const WORD moduleMajor =
            fixedInfo ? HIWORD(fixedInfo->dwFileVersionMS) : 0;

        if (moduleMajor && moduleMajor < 2604) {
            return module;
        }
    }

    return GetModuleHandleW(L"ExplorerExtensions.dll");
}

inline void* WINAPI IconView_IconView_Hook(void* pThis)
{
    auto result = IconView_IconView_Original(pThis);

    if (g_unloading.load() || !g_loadedRevokers) {
        return result;
    }

    try {
        FrameworkElement iconView = nullptr;

        reinterpret_cast<IUnknown**>(pThis)[1]->QueryInterface(
            winrt::guid_of<FrameworkElement>(), winrt::put_abi(iconView));

        if (!iconView || !g_loadedRevokers) {
            return result;
        }

        g_loadedRevokers->emplace_back();
        auto it = std::prev(g_loadedRevokers->end());

        *it = iconView.Loaded(
            winrt::auto_revoke_t{},
            [it](winrt::Windows::Foundation::IInspectable const&,
                 RoutedEventArgs const&)
            {
                if (!g_loadedRevokers) {
                    return;
                }

                g_loadedRevokers->erase(it);

                if (g_unloading.load() || !g_loadedRevokers) {
                    return;
                }

                // A new IconView means the tray may have rebuilt its XAML tree.
                OnTrayRebuilt();
            });
    }
    catch (...) {
        Wh_Log(L"IconView hook XAML work failed: %08X",
               static_cast<unsigned>(winrt::to_hresult()));
    }

    return result;
}

inline bool HookSystemTray(HMODULE module)
{
    // SystemTray.dll, Taskbar.View.dll, ExplorerExtensions.dll
    WindhawkUtils::SYMBOL_HOOK hooks[] = {{
        {LR"(public: __cdecl winrt::SystemTray::implementation::IconView::IconView(void))"},
        &IconView_IconView_Original,
        IconView_IconView_Hook,
    }};

    return WindhawkUtils::HookSymbols(module, hooks, ARRAYSIZE(hooks));
}

inline void OnModuleLoadedTaskbar(HMODULE module)
{
    if (g_unloading.load() || g_taskbarDllHooked.load()) {
        return;
    }

    HMODULE current = GetModuleHandleW(L"taskbar.dll");

    if (!current || current != module) {
        return;
    }

    if (g_taskbarDllAttempted.exchange(module) == module) {
        return;
    }

    if (HookTaskbarDll(module)) {
        g_taskbarDllHooked.store(true);
        Wh_ApplyHookOperations();
    }
    else {
        Wh_Log(L"ERROR: Failed to hook taskbar.dll symbols");
    }
}

inline void OnModuleLoadedSystemTray(HMODULE module)
{
    if (g_unloading.load() || g_systemTrayHooked.load()) {
        return;
    }

    if (GetSystemTrayModuleHandle() != module) {
        return;
    }

    if (g_systemTrayAttempted.exchange(module) == module) {
        return;
    }

    if (HookSystemTray(module)) {
        g_systemTrayHooked.store(true);
        Wh_ApplyHookOperations();
    }
    else {
        Wh_Log(L"ERROR: Failed to hook system tray symbols");
    }
}

inline HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR fileName, HANDLE file,
                                          DWORD flags)
{
    HMODULE module = LoadLibraryExW_Original(fileName, file, flags);

    if (module) {
        OnModuleLoadedTaskbar(module);
        OnModuleLoadedSystemTray(module);
    }

    return module;
}

}  // namespace host


// =============================================================================
// ui: XAML construction helpers (visual constants are unchanged)
// =============================================================================

namespace ui {

inline Style AccentButtonStyle()
{
    try {
        if (auto app = Application::Current()) {
            auto resources = app.Resources();
            auto key = winrt::box_value(L"AccentButtonStyle");

            if (resources.HasKey(key)) {
                return resources.Lookup(key).as<Style>();
            }
        }
    }
    catch (...) {
    }

    return Style{nullptr};
}

inline StackPanel ButtonRow()
{
    StackPanel row;
    row.Orientation(Orientation::Horizontal);
    return row;
}

inline Button RowButton(const wchar_t* text, double minWidth)
{
    Button button;
    button.Content(winrt::box_value(text));
    button.MinWidth(minWidth);
    button.Margin(Thickness{0, 0, 8, 0});
    return button;
}

inline TextBlock SectionTitle(const wchar_t* text)
{
    TextBlock title;
    title.Text(text);
    title.FontSize(12);
    title.Opacity(0.65);
    title.Margin(Thickness{0, 16, 0, 6});
    return title;
}

inline TextBlock ValueText()
{
    TextBlock text;
    text.TextTrimming(TextTrimming::CharacterEllipsis);
    text.Opacity(0.9);
    text.Margin(Thickness{0, 0, 0, 8});
    return text;
}

inline TextBlock HintText(const wchar_t* text)
{
    TextBlock hint;
    hint.Text(text);
    hint.FontSize(12);
    hint.Opacity(0.6);
    hint.TextWrapping(TextWrapping::Wrap);
    hint.Margin(Thickness{0, 2, 0, 0});
    return hint;
}

inline Button IconButton(const wchar_t* glyph)
{
    Button button;
    button.Content(winrt::box_value(glyph));
    button.FontFamily(FontFamily(L"Segoe Fluent Icons, Segoe MDL2 Assets"));
    button.Width(36);
    button.Height(32);
    button.Padding(Thickness{0, 0, 0, 0});
    return button;
}

inline Grid Header(const wchar_t* title, Button const& button, bool buttonOnLeft)
{
    Grid grid;

    ColumnDefinition titleColumn;
    titleColumn.Width(GridLengthHelper::FromValueAndType(1.0, GridUnitType::Star));

    ColumnDefinition buttonColumn;
    buttonColumn.Width(GridLengthHelper::Auto());

    TextBlock titleText;
    titleText.Text(title);
    titleText.FontSize(20);
    titleText.FontWeight(winrt::Windows::UI::Text::FontWeights::SemiBold());
    titleText.VerticalAlignment(VerticalAlignment::Center);

    if (buttonOnLeft) {
        titleText.Margin(Thickness{10, 0, 0, 0});

        grid.ColumnDefinitions().Append(buttonColumn);
        grid.ColumnDefinitions().Append(titleColumn);

        Grid::SetColumn(button, 0);
        Grid::SetColumn(titleText, 1);
    }
    else {
        grid.ColumnDefinitions().Append(titleColumn);
        grid.ColumnDefinitions().Append(buttonColumn);

        Grid::SetColumn(titleText, 0);
        Grid::SetColumn(button, 1);
    }

    grid.Children().Append(titleText);
    grid.Children().Append(button);

    return grid;
}

inline TextBox DurationBox(const wchar_t* header, bool last)
{
    TextBox box;
    box.Header(winrt::box_value(header));
    box.Width(112);
    box.MaxLength(5);
    box.Margin(Thickness{0, 0, last ? 0.0 : 8.0, 0});
    return box;
}

// Selected choice = check mark + accent style.
inline void MarkChoice(Button const& button, const wchar_t* name, bool selected)
{
    const std::wstring text = selected ? std::wstring(L"\u2713  ") + name : name;

    button.Content(winrt::box_value(winrt::hstring{text}));
    button.Style(selected ? AccentButtonStyle() : Style{nullptr});
}

// Empty field counts as 0. Returns false for anything that isn't a number.
inline bool ParseDuration(TextBox const& box, long& value)
{
    value = 0;

    if (!box) {
        return false;
    }

    const std::wstring text = box.Text().c_str();

    if (text.empty()) {
        return true;
    }

    wchar_t* end = nullptr;
    const long parsed = wcstol(text.c_str(), &end, 10);

    if (!end || end == text.c_str() || *end != L'\0' || parsed < 0 ||
        parsed > 99999)
    {
        return false;
    }

    value = parsed;
    return true;
}

}  // namespace ui


// =============================================================================
// WheelPicker: Samsung-style alarm wheels (hour, minute, AM / PM)
// =============================================================================
//
// Three columns of five rows; the selected value sits in the middle, big and
// highlighted. Mouse wheel over a column or the small arrows change the value.
// The value itself lives in g_prefs.alarm*.

class WheelPicker {
public:
    // Builds the whole alarm row (hour : minute  AM/PM).
    StackPanel Build()
    {
        StackPanel row = ui::ButtonRow();
        row.Visibility(Visibility::Collapsed);
        row.HorizontalAlignment(HorizontalAlignment::Center);

        row.Children().Append(MakeColumn(0));

        TextBlock colon;
        colon.Text(L":");
        colon.FontSize(30);
        colon.FontWeight(winrt::Windows::UI::Text::FontWeights::SemiBold());
        colon.VerticalAlignment(VerticalAlignment::Center);
        colon.Margin(Thickness{4, 0, 4, 0});
        row.Children().Append(colon);

        row.Children().Append(MakeColumn(1));

        Border spacer;
        spacer.Width(14);
        row.Children().Append(spacer);

        row.Children().Append(MakeColumn(2));

        return row;
    }

    void Release()
    {
        for (auto& entry : clicks_) {
            if (entry.first) {
                entry.first.Click(entry.second);
            }
        }

        clicks_.clear();

        for (auto& entry : wheels_) {
            if (entry.first) {
                entry.first.PointerWheelChanged(entry.second);
            }
        }

        wheels_.clear();

        for (auto& rows : rows_) {
            rows.clear();
        }
    }

    // Writes the current value into the text rows of all three columns.
    void Refresh()
    {
        const int h12 = std::clamp(g_prefs.alarmHour12.load(), 1, 12);
        const int minute = std::clamp(g_prefs.alarmMinute.load(), 0, 59);
        const int pm = g_prefs.alarmPM.load() ? 1 : 0;

        wchar_t text[16]{};

        if (rows_[0].size() == 5) {
            for (int i = 0; i < 5; i++) {
                const int value = (((h12 - 1 + (i - 2)) % 12) + 12) % 12 + 1;
                swprintf_s(text, L"%d", value);
                rows_[0][i].Text(text);
            }
        }

        if (rows_[1].size() == 5) {
            for (int i = 0; i < 5; i++) {
                const int value = (((minute + (i - 2)) % 60) + 60) % 60;
                swprintf_s(text, L"%02d", value);
                rows_[1][i].Text(text);
            }
        }

        // AM / PM: only two values, no wrap-around.
        if (rows_[2].size() == 5) {
            static const wchar_t* const kNames[] = {L"AM", L"PM"};

            for (int i = 0; i < 5; i++) {
                const int idx = pm + (i - 2);

                if (idx < 0 || idx > 1 || i == 0 || i == 4) {
                    rows_[2][i].Text(L"");
                }
                else {
                    rows_[2][i].Text(kNames[idx]);
                }
            }
        }
    }

    // Default alarm time = now + the given number of minutes.
    void SetFromNow(int addMinutes)
    {
        SYSTEMTIME now{};
        GetLocalTime(&now);

        int total = now.wHour * 60 + now.wMinute + addMinutes;
        total = ((total % 1440) + 1440) % 1440;

        const int hour24 = total / 60;

        g_prefs.alarmPM.store(hour24 >= 12 ? 1 : 0);
        g_prefs.alarmHour12.store(hour24 % 12 == 0 ? 12 : hour24 % 12);
        g_prefs.alarmMinute.store(total % 60);

        Refresh();
    }

private:
    // delta: +1 = next value, -1 = previous.
    void Step(int column, int delta)
    {
        if (column == 0) {
            int h = g_prefs.alarmHour12.load();
            h = (((h - 1 + delta) % 12) + 12) % 12 + 1;
            g_prefs.alarmHour12.store(h);
        }
        else if (column == 1) {
            int m = g_prefs.alarmMinute.load();
            m = (((m + delta) % 60) + 60) % 60;
            g_prefs.alarmMinute.store(m);
        }
        else {
            int p = g_prefs.alarmPM.load() ? 1 : 0;
            p = (((p + delta) % 2) + 2) % 2;
            g_prefs.alarmPM.store(p);
        }

        Refresh();
    }

    void AddClick(Button const& button, std::function<void()> action)
    {
        auto token = button.Click(
            [action](auto const&, RoutedEventArgs const&)
            {
                if (!g_unloading.load()) {
                    action();
                }
            });

        clicks_.emplace_back(button, token);
    }

    StackPanel MakeColumn(int column)
    {
        StackPanel panel;
        panel.Width(84);
        panel.Background(SolidColorBrush(winrt::Windows::UI::Colors::Transparent()));

        Button up = ui::IconButton(L"\uE70E");
        up.Width(84);
        up.Height(28);
        up.HorizontalAlignment(HorizontalAlignment::Center);
        panel.Children().Append(up);

        AddClick(up, [this, column] { Step(column, -1); });

        static const double kFontSizes[5] = {16, 20, 30, 20, 16};
        static const double kOpacities[5] = {0.22, 0.5, 1.0, 0.5, 0.22};

        rows_[column].clear();

        for (int i = 0; i < 5; i++) {
            Border cell;
            cell.Width(84);
            cell.Height(40);
            cell.CornerRadius(CornerRadius{8, 8, 8, 8});

            if (i == 2) {
                // Highlight of the selected value.
                cell.Background(SolidColorBrush(
                    winrt::Windows::UI::ColorHelper::FromArgb(48, 128, 128, 128)));
            }

            TextBlock row;
            row.FontSize(kFontSizes[i]);
            row.Opacity(kOpacities[i]);
            row.HorizontalAlignment(HorizontalAlignment::Center);
            row.VerticalAlignment(VerticalAlignment::Center);
            row.TextAlignment(TextAlignment::Center);

            if (i == 2) {
                row.FontWeight(winrt::Windows::UI::Text::FontWeights::SemiBold());
            }

            cell.Child(row);
            panel.Children().Append(cell);

            rows_[column].push_back(row);
        }

        Button down = ui::IconButton(L"\uE70D");
        down.Width(84);
        down.Height(28);
        down.HorizontalAlignment(HorizontalAlignment::Center);
        panel.Children().Append(down);

        AddClick(down, [this, column] { Step(column, 1); });

        // Mouse wheel: scroll down = next value, scroll up = previous value.
        auto token = panel.PointerWheelChanged(
            [this, column](
                winrt::Windows::Foundation::IInspectable const&,
                winrt::Windows::UI::Xaml::Input::PointerRoutedEventArgs const& args)
            {
                if (g_unloading.load()) {
                    return;
                }

                const int wheel = args.GetCurrentPoint(UIElement{nullptr})
                                      .Properties()
                                      .MouseWheelDelta();

                if (wheel == 0) {
                    return;
                }

                Step(column, wheel > 0 ? -1 : 1);
                args.Handled(true);
            });

        wheels_.emplace_back(panel, token);

        return panel;
    }

    std::vector<TextBlock> rows_[3];
    std::vector<std::pair<Button, winrt::event_token>> clicks_;
    std::vector<std::pair<UIElement, winrt::event_token>> wheels_;
};


// =============================================================================
// FlyoutView: the flyout with its main page and settings page
// =============================================================================

class FlyoutView {
public:
    void Show(FrameworkElement const& anchor)
    {
        if (g_unloading.load()) {
            return;
        }

        Build();

        if (!flyout_ || !main_ || !settings_ || !reminder_ || !hours_ ||
            !minutes_ || !seconds_ || !soundName_ || !bgName_ || !start_ ||
            !cancel_)
        {
            return;
        }

        const Options options = OptionStore::Snapshot();

        soundName_.Text(L"Sound: " + OptionStore::SoundDisplayName());
        bgName_.Text(L"Background: " + OptionStore::BackgroundDisplayName());

        RefreshChoices();

        const bool keepFields = keepFields_.exchange(false);
        const bool openSettings = openSettings_.exchange(false);

        ShowPage(openSettings);

        const int remaining = g_countdown.Remaining();

        if (remaining > 0) {
            reminder_.Text(g_countdown.Reminder());

            SetDurationBoxes(remaining);

            reminder_.IsEnabled(false);
            hours_.IsEnabled(false);
            minutes_.IsEnabled(false);
            seconds_.IsEnabled(false);

            start_.IsEnabled(false);
            cancel_.IsEnabled(true);
        }
        else {
            if (!keepFields) {
                reminder_.Text(L"");
                SetDurationBoxes(options.defaultMinutes * 60);
                picker_.SetFromNow(options.defaultMinutes);
            }

            reminder_.IsEnabled(true);
            hours_.IsEnabled(true);
            minutes_.IsEnabled(true);
            seconds_.IsEnabled(true);

            start_.IsEnabled(true);
            cancel_.IsEnabled(false);
        }

        // Right fields for the chosen mode. While something runs, the remaining
        // time is shown (always h / m / s) and the mode can't be switched.
        ApplyMode();

        const bool running = remaining > 0;

        if (running && durationRow_ && alarmRow_) {
            durationRow_.Visibility(Visibility::Visible);
            alarmRow_.Visibility(Visibility::Collapsed);
        }

        for (auto& choice : modeChoices_) {
            choice.IsEnabled(!running);
        }

        ApplyLook();

        flyout_.ShowAt(anchor);
    }

    // Revokes delegates, then hides and drops the whole flyout tree.
    void Release()
    {
        for (auto& entry : clicks_) {
            if (entry.first) {
                entry.first.Click(entry.second);
            }
        }

        clicks_.clear();

        picker_.Release();

        if (flyout_) {
            flyout_.Hide();
        }

        modeChoices_.clear();
        themeChoices_.clear();
        sizeChoices_.clear();
        glassChoices_.clear();

        frame_ = nullptr;
        overlay_ = nullptr;
        root_ = nullptr;

        main_ = nullptr;
        settings_ = nullptr;
        durationRow_ = nullptr;
        alarmRow_ = nullptr;
        reminder_ = nullptr;
        hours_ = nullptr;
        minutes_ = nullptr;
        seconds_ = nullptr;
        soundName_ = nullptr;
        bgName_ = nullptr;
        start_ = nullptr;
        cancel_ = nullptr;
        flyout_ = nullptr;
    }

    void Hide()
    {
        if (flyout_) {
            flyout_.Hide();
        }
    }

private:
    void AddClick(Button const& button, std::function<void()> action)
    {
        auto token = button.Click(
            [action](auto const&, RoutedEventArgs const&)
            {
                if (!g_unloading.load()) {
                    action();
                }
            });

        clicks_.emplace_back(button, token);
    }

    // ---- state refresh ---------------------------------------------------------------
    void RefreshChoices()
    {
        static const wchar_t* const kThemeNames[] = {L"Dark", L"Light", L"Auto"};
        static const wchar_t* const kSizeNames[] = {L"Large", L"Small"};
        static const wchar_t* const kModeNames[] = {L"Timer", L"Alarm"};
        static const wchar_t* const kGlassNames[] = {L"Solid", L"Frosted glass"};

        const int theme = g_prefs.alertTheme.load();

        for (size_t i = 0; i < themeChoices_.size() && i < 3; i++) {
            ui::MarkChoice(themeChoices_[i], kThemeNames[i],
                           static_cast<int>(i) == theme);
        }

        const int size = g_prefs.alertSmall.load() ? 1 : 0;

        for (size_t i = 0; i < sizeChoices_.size() && i < 2; i++) {
            ui::MarkChoice(sizeChoices_[i], kSizeNames[i],
                           static_cast<int>(i) == size);
        }

        const int mode = g_prefs.alarmMode.load() ? 1 : 0;

        for (size_t i = 0; i < modeChoices_.size() && i < 2; i++) {
            ui::MarkChoice(modeChoices_[i], kModeNames[i],
                           static_cast<int>(i) == mode);
        }

        const int glass = g_prefs.flyoutGlass.load() ? 1 : 0;

        for (size_t i = 0; i < glassChoices_.size() && i < 2; i++) {
            ui::MarkChoice(glassChoices_[i], kGlassNames[i],
                           static_cast<int>(i) == glass);
        }
    }

    // Duration fields (Timer) or wheel picker (Alarm); renames the Start button.
    void ApplyMode()
    {
        if (!durationRow_ || !alarmRow_) {
            return;
        }

        const bool alarm = g_prefs.alarmMode.load();

        durationRow_.Visibility(alarm ? Visibility::Collapsed : Visibility::Visible);
        alarmRow_.Visibility(alarm ? Visibility::Visible : Visibility::Collapsed);

        if (start_) {
            start_.Content(winrt::box_value(alarm ? L"Set alarm" : L"Start"));
        }
    }

    void ShowPage(bool settings)
    {
        if (!main_ || !settings_) {
            return;
        }

        main_.Visibility(settings ? Visibility::Collapsed : Visibility::Visible);
        settings_.Visibility(settings ? Visibility::Visible : Visibility::Collapsed);
    }

    void SetDurationBoxes(int totalSeconds)
    {
        if (totalSeconds < 0) {
            totalSeconds = 0;
        }

        wchar_t text[16]{};

        swprintf_s(text, L"%d", totalSeconds / 3600);
        hours_.Text(text);

        swprintf_s(text, L"%d", (totalSeconds % 3600) / 60);
        minutes_.Text(text);

        swprintf_s(text, L"%d", totalSeconds % 60);
        seconds_.Text(text);
    }

    // Dark / Light / Auto, chosen background image, optional acrylic glass.
    void ApplyLook()
    {
        if (!flyout_ || !frame_ || !overlay_) {
            return;
        }

        using namespace winrt::Windows::UI::Xaml::Media::Imaging;
        using winrt::Windows::UI::ColorHelper;

        const int pref = g_prefs.alertTheme.load();
        const bool light = pref == 1 || (pref == 2 && util::AppsUseLightTheme());
        const bool glass = g_prefs.flyoutGlass.load();

        if (root_) {
            root_.RequestedTheme(light ? ElementTheme::Light : ElementTheme::Dark);
        }

        const Options options = OptionStore::Snapshot();
        const std::wstring& path = options.customBackgroundPath;

        bool hasImage = false;

        if (!path.empty() && util::IsImageFile(path) &&
            GetFileAttributesW(path.c_str()) != INVALID_FILE_ATTRIBUTES)
        {
            try {
                const std::wstring url = util::ToFileUrl(path);

                if (!url.empty()) {
                    BitmapImage bitmap;
                    bitmap.UriSource(winrt::Windows::Foundation::Uri(url));

                    ImageBrush brush;
                    brush.ImageSource(bitmap);
                    brush.Stretch(Stretch::UniformToFill);

                    // In glass mode the picture itself becomes translucent.
                    brush.Opacity(glass ? 0.45 : 1.0);

                    frame_.Background(brush);
                    hasImage = true;
                }
            }
            catch (...) {
                Wh_Log(L"Flyout background image failed");
            }
        }

        if (!hasImage) {
            const auto solid = light ? ColorHelper::FromArgb(255, 249, 249, 249)
                                     : ColorHelper::FromArgb(255, 32, 32, 32);

            if (glass) {
                // Blurs the desktop behind the flyout; glassTint controls the
                // tint strength (same setting as the alert card).
                AcrylicBrush acrylic;
                acrylic.BackgroundSource(AcrylicBackgroundSource::HostBackdrop);
                acrylic.TintColor(light ? ColorHelper::FromArgb(255, 252, 252, 255)
                                        : ColorHelper::FromArgb(255, 24, 26, 34));
                acrylic.TintOpacity(
                    light ? std::clamp(options.glassTint / 100.0 + 0.30, 0.45, 0.85)
                          : std::clamp(options.glassTint / 100.0 + 0.15, 0.0, 1.0));

                try {
                    acrylic.TintLuminosityOpacity(
                        winrt::box_value(light ? 0.9 : 0.85)
                            .as<winrt::Windows::Foundation::IReference<double>>());
                }
                catch (...) {
                }

                acrylic.FallbackColor(solid);

                frame_.Background(acrylic);
            }
            else {
                frame_.Background(SolidColorBrush(solid));
            }
        }

        // Tint over the image so the text stays readable.
        uint8_t alpha = 0;

        if (hasImage) {
            alpha = glass ? (light ? 120 : 110) : (light ? 165 : 150);
        }

        overlay_.Background(SolidColorBrush(
            light ? ColorHelper::FromArgb(alpha, 255, 255, 255)
                  : ColorHelper::FromArgb(alpha, 0, 0, 0)));
    }

    // ---- actions ---------------------------------------------------------------------
    void OnStart()
    {
        if (!reminder_ || !hours_ || !minutes_ || !seconds_) {
            return;
        }

        // ---- Alarm mode: ring at a time of day (12-hour wheel) ----------------
        if (g_prefs.alarmMode.load()) {
            const int h12 = std::clamp(g_prefs.alarmHour12.load(), 1, 12);
            const int minute = std::clamp(g_prefs.alarmMinute.load(), 0, 59);

            // 12 AM = 0, 1-11 AM = 1-11, 12 PM = 12, 1-11 PM = 13-23.
            const int hour24 = (h12 % 12) + (g_prefs.alarmPM.load() ? 12 : 0);

            const long long until = util::SecondsUntilLocalTime(hour24, minute);

            const std::wstring text = reminder_.Text().c_str();

            if (!g_countdown.Arm(static_cast<int>(until), text.c_str(), true)) {
                return;
            }

            g_sound.Stop();
            Hide();
            return;
        }

        // ---- Timer mode: countdown ----------------------------------------------
        const Options options = OptionStore::Snapshot();

        long hours = 0;
        long minutes = 0;
        long seconds = 0;

        const auto focusBad = [](TextBox const& box)
        {
            box.SelectAll();
            box.Focus(FocusState::Programmatic);
        };

        if (!ui::ParseDuration(hours_, hours)) {
            focusBad(hours_);
            return;
        }

        if (!ui::ParseDuration(minutes_, minutes)) {
            focusBad(minutes_);
            return;
        }

        if (!ui::ParseDuration(seconds_, seconds)) {
            focusBad(seconds_);
            return;
        }

        const long long total = hours * 3600LL + minutes * 60LL + seconds;
        const long long maximum =
            static_cast<long long>(options.maximumMinutes) * 60LL;

        if (total <= 0 || total > maximum) {
            focusBad(minutes_);
            return;
        }

        const std::wstring text = reminder_.Text().c_str();

        if (!g_countdown.Arm(static_cast<int>(total), text.c_str())) {
            return;
        }

        g_sound.Stop();
        Hide();
    }

    // ---- construction -----------------------------------------------------------------
    void Build()
    {
        if (flyout_) {
            return;
        }

        Flyout flyout;
        flyout.Placement(Controls::Primitives::FlyoutPlacementMode::Top);
        flyout.ShouldConstrainToRootBounds(false);

        StackPanel root;
        root.Width(360);
        root.Margin(Thickness{16, 14, 16, 14});

        // ---- main page: mode, reminder, duration / alarm time, start / cancel ----
        StackPanel mainPage;

        Button gearButton = ui::IconButton(L"\uE713");
        mainPage.Children().Append(ui::Header(L"Timer & Alarm", gearButton, false));

        StackPanel modeRow = ui::ButtonRow();
        modeRow.Margin(Thickness{0, 12, 0, 0});

        for (int i = 0; i < 2; i++) {
            Button choice = ui::RowButton(L"", 100);

            modeRow.Children().Append(choice);
            modeChoices_.push_back(choice);

            AddClick(choice, [this, i]
            {
                g_prefs.alarmMode.store(i == 1);
                Wh_SetIntValue(L"alarmMode", i);
                RefreshChoices();
                ApplyMode();
            });
        }

        mainPage.Children().Append(modeRow);

        TextBox reminderBox;
        reminderBox.Header(winrt::box_value(L"Reminder"));
        reminderBox.PlaceholderText(L"What should I remind you about?");
        reminderBox.MaxLength(255);
        reminderBox.Margin(Thickness{0, 12, 0, 12});
        mainPage.Children().Append(reminderBox);

        // Timer mode: hours / minutes / seconds.
        StackPanel durationRow = ui::ButtonRow();

        TextBox hoursBox = ui::DurationBox(L"Hours", false);
        TextBox minutesBox = ui::DurationBox(L"Minutes", false);
        TextBox secondsBox = ui::DurationBox(L"Seconds", true);

        durationRow.Children().Append(hoursBox);
        durationRow.Children().Append(minutesBox);
        durationRow.Children().Append(secondsBox);

        mainPage.Children().Append(durationRow);

        // Alarm mode: wheels.
        StackPanel alarmRow = picker_.Build();
        mainPage.Children().Append(alarmRow);

        StackPanel actions = ui::ButtonRow();
        actions.Margin(Thickness{0, 18, 0, 0});

        Button startButton = ui::RowButton(L"Start", 130);
        Button cancelButton = ui::RowButton(L"Cancel", 130);

        if (auto accent = ui::AccentButtonStyle()) {
            startButton.Style(accent);
        }

        actions.Children().Append(startButton);
        actions.Children().Append(cancelButton);

        mainPage.Children().Append(actions);

        // ---- settings page --------------------------------------------------------
        StackPanel settingsPage;
        settingsPage.Visibility(Visibility::Collapsed);

        Button backButton = ui::IconButton(L"\uE72B");
        settingsPage.Children().Append(ui::Header(L"Settings", backButton, true));

        // Sound
        settingsPage.Children().Append(ui::SectionTitle(L"COMPLETION SOUND"));

        TextBlock soundName = ui::ValueText();
        settingsPage.Children().Append(soundName);

        StackPanel soundRow = ui::ButtonRow();
        Button soundChooseButton = ui::RowButton(L"Choose sound…", 120);
        Button soundTestButton = ui::RowButton(L"Test", 70);
        Button soundDefaultButton = ui::RowButton(L"Default", 80);

        soundRow.Children().Append(soundChooseButton);
        soundRow.Children().Append(soundTestButton);
        soundRow.Children().Append(soundDefaultButton);
        settingsPage.Children().Append(soundRow);

        // Background
        settingsPage.Children().Append(
            ui::SectionTitle(L"ALERT BACKGROUND (IMAGE OR VIDEO)"));

        TextBlock bgName = ui::ValueText();
        settingsPage.Children().Append(bgName);

        StackPanel bgRow = ui::ButtonRow();
        Button bgChooseButton = ui::RowButton(L"Choose background…", 170);
        Button bgDefaultButton = ui::RowButton(L"Default", 80);

        bgRow.Children().Append(bgChooseButton);
        bgRow.Children().Append(bgDefaultButton);
        settingsPage.Children().Append(bgRow);

        // Look: dark / light / auto
        settingsPage.Children().Append(ui::SectionTitle(L"ALERT LOOK"));

        StackPanel themeRow = ui::ButtonRow();

        for (int i = 0; i < 3; i++) {
            Button choice = ui::RowButton(L"", 100);

            themeRow.Children().Append(choice);
            themeChoices_.push_back(choice);

            AddClick(choice, [this, i]
            {
                g_prefs.alertTheme.store(i);
                Wh_SetIntValue(L"alertTheme", i);
                RefreshChoices();
                ApplyLook();
            });
        }

        settingsPage.Children().Append(themeRow);

        // Glass: solid / frosted
        settingsPage.Children().Append(
            ui::SectionTitle(L"GLASS STYLE (FLYOUT + ALERT)"));

        StackPanel glassRow = ui::ButtonRow();

        for (int i = 0; i < 2; i++) {
            Button choice = ui::RowButton(L"", 150);

            glassRow.Children().Append(choice);
            glassChoices_.push_back(choice);

            AddClick(choice, [this, i]
            {
                g_prefs.flyoutGlass.store(i == 1);
                Wh_SetIntValue(L"flyoutGlass", i);
                RefreshChoices();
                ApplyLook();
            });
        }

        settingsPage.Children().Append(glassRow);
        settingsPage.Children().Append(ui::HintText(
            L"Frosted glass also blurs the alert card over a video background."));

        // Size: large / small
        settingsPage.Children().Append(ui::SectionTitle(L"ALERT SIZE"));

        StackPanel sizeRow = ui::ButtonRow();

        for (int i = 0; i < 2; i++) {
            Button choice = ui::RowButton(L"", 150);

            sizeRow.Children().Append(choice);
            sizeChoices_.push_back(choice);

            AddClick(choice, [this, i]
            {
                g_prefs.alertSmall.store(i == 1);
                Wh_SetIntValue(L"alertSmall", i);
                RefreshChoices();
            });
        }

        settingsPage.Children().Append(sizeRow);
        settingsPage.Children().Append(ui::HintText(
            L"Large: full screen. Small: notification in the bottom-right corner."));

        // Preview
        Button previewButton = ui::RowButton(L"Preview alert", 160);
        previewButton.Margin(Thickness{0, 18, 0, 0});

        if (auto accent = ui::AccentButtonStyle()) {
            previewButton.Style(accent);
        }

        settingsPage.Children().Append(previewButton);

        // ---- handlers -----------------------------------------------------------------
        AddClick(gearButton, [this]
        {
            if (soundName_) {
                soundName_.Text(L"Sound: " + OptionStore::SoundDisplayName());
            }

            if (bgName_) {
                bgName_.Text(L"Background: " + OptionStore::BackgroundDisplayName());
            }

            RefreshChoices();
            ShowPage(true);
        });

        AddClick(backButton, [this] { ShowPage(false); });

        AddClick(startButton, [this] { OnStart(); });

        AddClick(cancelButton, [this]
        {
            g_countdown.Cancel();
            g_sound.Stop();
            Hide();
        });

        AddClick(soundChooseButton, [this]
        {
            // The flyout closes when the file dialog takes focus; keep what the
            // user typed and reopen on the settings page afterwards.
            keepFields_.store(true);
            openSettings_.store(true);
            g_picker.Start(PickKind::Sound);
        });

        AddClick(soundTestButton, [] { g_sound.Play(true); });

        AddClick(soundDefaultButton, [this]
        {
            g_sound.Stop();
            OptionStore::SetSound(nullptr);

            if (soundName_) {
                soundName_.Text(L"Sound: " + OptionStore::SoundDisplayName());
            }
        });

        AddClick(bgChooseButton, [this]
        {
            keepFields_.store(true);
            openSettings_.store(true);
            g_picker.Start(PickKind::Background);
        });

        AddClick(bgDefaultButton, [this]
        {
            OptionStore::SetBackground(nullptr);

            if (bgName_) {
                bgName_.Text(L"Background: " + OptionStore::BackgroundDisplayName());
            }

            ApplyLook();
        });

        AddClick(previewButton, [this]
        {
            Hide();
            g_alerts.Show(L"This is how your timer alert will look.", true);
        });

        root.Children().Append(mainPage);
        root.Children().Append(settingsPage);

        Grid frame;
        frame.CornerRadius(CornerRadius{8, 8, 8, 8});

        Border overlay;
        overlay.IsHitTestVisible(false);

        frame.Children().Append(overlay);   // tint over the background
        frame.Children().Append(root);      // content

        // Remove the default gray background / padding of the flyout presenter.
        Style presenterStyle{winrt::Windows::UI::Xaml::Interop::TypeName{
            L"Windows.UI.Xaml.Controls.FlyoutPresenter",
            winrt::Windows::UI::Xaml::Interop::TypeKind::Metadata}};

        presenterStyle.Setters().Append(Setter(
            Control::PaddingProperty(), winrt::box_value(Thickness{0, 0, 0, 0})));

        presenterStyle.Setters().Append(Setter(
            Control::BackgroundProperty(),
            SolidColorBrush(winrt::Windows::UI::Colors::Transparent())));

        flyout.FlyoutPresenterStyle(presenterStyle);
        flyout.Content(frame);

        frame_ = frame;
        overlay_ = overlay;
        root_ = root;

        main_ = mainPage;
        settings_ = settingsPage;
        durationRow_ = durationRow;
        alarmRow_ = alarmRow;
        reminder_ = reminderBox;
        hours_ = hoursBox;
        minutes_ = minutesBox;
        seconds_ = secondsBox;
        soundName_ = soundName;
        bgName_ = bgName;
        start_ = startButton;
        cancel_ = cancelButton;
        flyout_ = flyout;

        RefreshChoices();
        picker_.Refresh();
        ApplyMode();
        ApplyLook();
    }

    Flyout flyout_{nullptr};
    Grid frame_{nullptr};
    Border overlay_{nullptr};
    FrameworkElement root_{nullptr};

    StackPanel main_{nullptr};
    StackPanel settings_{nullptr};
    StackPanel durationRow_{nullptr};
    StackPanel alarmRow_{nullptr};

    TextBox reminder_{nullptr};
    TextBox hours_{nullptr};
    TextBox minutes_{nullptr};
    TextBox seconds_{nullptr};
    TextBlock soundName_{nullptr};
    TextBlock bgName_{nullptr};
    Button start_{nullptr};
    Button cancel_{nullptr};

    std::vector<Button> modeChoices_;
    std::vector<Button> themeChoices_;
    std::vector<Button> sizeChoices_;
    std::vector<Button> glassChoices_;

    WheelPicker picker_;
    std::vector<std::pair<Button, winrt::event_token>> clicks_;

    std::atomic_bool keepFields_{false};
    std::atomic_bool openSettings_{false};
};

[[clang::no_destroy]] inline FlyoutView g_flyout;


// =============================================================================
// TaskbarButton: the button in the system tray area + its 1 s display ticker
// =============================================================================

template <typename T1, typename T2>
inline bool SameWinrtObject(T1 const& a, T2 const& b)
{
    if (!a || !b) {
        return false;
    }

    auto aUnknown = a.template as<winrt::Windows::Foundation::IUnknown>();
    auto bUnknown = b.template as<winrt::Windows::Foundation::IUnknown>();

    return winrt::get_abi(aUnknown) == winrt::get_abi(bUnknown);
}

class TaskbarButton {
public:
    // Adds the button to the tray grid (or keeps the existing one).
    void Attach(HWND taskbar)
    {
        if (g_unloading.load()) {
            return;
        }

        auto xamlRoot = host::GetTaskbarXamlRoot(taskbar);

        if (!xamlRoot) {
            return;
        }

        auto content = xamlRoot.Content().try_as<FrameworkElement>();

        if (!content) {
            return;
        }

        auto tray = host::FindChildRecursive(
            content,
            [](FrameworkElement element)
            {
                return element.Name() == L"SystemTrayFrameGrid";
            });

        if (!tray) {
            return;
        }

        if (button_) {
            auto currentParent =
                VisualTreeHelper::GetParent(button_).try_as<FrameworkElement>();

            if (currentParent && SameWinrtObject(currentParent, tray)) {
                g_taskbarWnd.store(taskbar);
                return;
            }
        }

        auto panel = tray.try_as<Panel>();

        if (!panel) {
            return;
        }

        auto children = panel.Children();

        FrameworkElement existing{nullptr};

        for (uint32_t i = 0; i < children.Size(); i++) {
            auto child = children.GetAt(i).try_as<FrameworkElement>();

            if (child && child.Name() == L"TaskbarTimerAlarmButton") {
                existing = child;
                break;
            }
        }

        if (existing && button_ && SameWinrtObject(existing, button_)) {
            g_taskbarWnd.store(taskbar);
            return;
        }

        if (existing) {
            Wh_Log(L"WARNING: A foreign TaskbarTimerAlarmButton already "
                   L"exists; leaving it untouched for safety");
            return;
        }

        ReleaseForRebuild();

        button_ = Button();
        text_ = TextBlock();

        button_.Name(L"TaskbarTimerAlarmButton");
        button_.MinWidth(0);
        button_.MinHeight(0);
        button_.BorderThickness(Thickness{0, 0, 0, 0});
        button_.Padding(Thickness{4, 0, 4, 0});
        button_.VerticalAlignment(VerticalAlignment::Stretch);

        text_.VerticalAlignment(VerticalAlignment::Center);

        button_.Content(text_);

        ApplyMetrics();
        Refresh();

        clickToken_ = button_.Click(
            [this](auto const&, RoutedEventArgs const&)
            {
                if (!g_unloading.load()) {
                    OpenFlyout();
                }
            });

        ticker_ = DispatcherTimer();
        ticker_.Interval(std::chrono::seconds(1));

        tickToken_ = ticker_.Tick(
            [this](auto const&, auto const&)
            {
                if (g_unloading.load()) {
                    return;
                }

                const int remaining = g_countdown.Remaining();

                if (remaining > 0) {
                    if (text_) {
                        text_.Text(util::FormatCountdown(remaining,
                                                         g_countdown.IsAlarm()));
                    }
                }
                else {
                    StopTicker();
                    SetIdle();
                }
            });

        const auto trayClass = winrt::get_class_name(tray);

        if (trayClass == L"Windows.UI.Xaml.Controls.StackPanel") {
            children.InsertAt(0, button_);
        }
        else {
            auto trayGrid = tray.try_as<Grid>();

            if (!trayGrid) {
                Wh_Log(L"ERROR: Unsupported SystemTrayFrameGrid layout class: %s",
                       trayClass.c_str());

                ReleaseForRebuild();
                return;
            }

            auto columns = trayGrid.ColumnDefinitions();

            column_ = ColumnDefinition();
            column_.Width(GridLengthHelper::Auto());

            columns.InsertAt(0, column_);

            for (uint32_t i = 0; i < children.Size(); i++) {
                auto child = children.GetAt(i).try_as<FrameworkElement>();

                if (child) {
                    Grid::SetColumn(child, Grid::GetColumn(child) + 1);
                }
            }

            Grid::SetColumn(button_, 0);

            children.InsertAt(0, button_);
        }

        g_taskbarWnd.store(taskbar);

        if (g_countdown.Remaining() > 0 && ticker_) {
            Refresh();
            ticker_.Start();
        }
        else {
            SetIdle();
        }

        Wh_Log(L"Timer button added to taskbar");
    }

    // Full teardown (mod unload). Revokes every callback into the mod first.
    void Teardown(bool& callbacksRevoked)
    {
        if (host::g_loadedRevokers) {
            host::g_loadedRevokers.reset();
        }

        g_flyout.Release();

        if (ticker_) {
            ticker_.Stop();
            ticker_.Tick(tickToken_);
            ticker_ = nullptr;
        }

        if (button_) {
            button_.Click(clickToken_);
        }

        callbacksRevoked = true;

        // Best-effort visual-tree detachment after all callbacks are gone.
        DetachVisuals();

        column_ = nullptr;
        text_ = nullptr;
        button_ = nullptr;
    }

    void OpenFlyout()
    {
        if (!button_ || g_unloading.load()) {
            return;
        }

        g_flyout.Show(button_);
    }

    void StartTicker()
    {
        if (ticker_) {
            ticker_.Start();
        }
    }

    void StopTicker()
    {
        if (ticker_) {
            ticker_.Stop();
        }
    }

    void SetIdle()
    {
        if (!text_) {
            return;
        }

        text_.Text(OptionStore::Snapshot().buttonLabel);
    }

    // Shows the countdown when something is running, the idle label otherwise.
    void Refresh()
    {
        if (!text_) {
            return;
        }

        const int remaining = g_countdown.Remaining();

        if (remaining > 0) {
            text_.Text(util::FormatCountdown(remaining, g_countdown.IsAlarm()));
        }
        else {
            SetIdle();
        }
    }

    // Keeps the taskbar button small: smaller font and tight padding.
    void ApplyMetrics()
    {
        if (!button_ || !text_) {
            return;
        }

        text_.FontSize(OptionStore::Snapshot().buttonFontSize);
        button_.Padding(Thickness{4, 0, 4, 0});
    }

private:
    void DetachVisuals()
    {
        if (!button_) {
            return;
        }

        auto parentElement =
            VisualTreeHelper::GetParent(button_).try_as<FrameworkElement>();

        auto parent = parentElement.try_as<Panel>();

        if (!parent) {
            return;
        }

        auto children = parent.Children();

        uint32_t buttonIndex = 0;

        if (children.IndexOf(button_, buttonIndex)) {
            children.RemoveAt(buttonIndex);
        }

        auto parentGrid = parentElement.try_as<Grid>();

        if (!parentGrid || !column_) {
            return;
        }

        auto columns = parentGrid.ColumnDefinitions();

        uint32_t columnIndex = 0;

        if (!columns.IndexOf(column_, columnIndex)) {
            return;
        }

        for (uint32_t i = 0; i < children.Size(); i++) {
            auto child = children.GetAt(i).try_as<FrameworkElement>();

            if (!child) {
                continue;
            }

            const int currentColumn = Grid::GetColumn(child);

            if (currentColumn > static_cast<int>(columnIndex)) {
                Grid::SetColumn(child, currentColumn - 1);
            }
        }

        columns.RemoveAt(columnIndex);
    }

    // Drops our previous button before a fresh one is created.
    void ReleaseForRebuild()
    {
        g_flyout.Release();

        if (ticker_) {
            ticker_.Stop();
            ticker_.Tick(tickToken_);
            ticker_ = nullptr;
        }

        if (button_) {
            button_.Click(clickToken_);
        }

        DetachVisuals();

        button_ = nullptr;
        text_ = nullptr;
        column_ = nullptr;
    }

    Button button_{nullptr};
    TextBlock text_{nullptr};
    DispatcherTimer ticker_{nullptr};
    ColumnDefinition column_{nullptr};

    winrt::event_token clickToken_{};
    winrt::event_token tickToken_{};
};

[[clang::no_destroy]] inline TaskbarButton g_button;


// =============================================================================
// Mod wiring: runtime start, callbacks, lifecycle
// =============================================================================

inline SRWLOCK g_runtimeLock = SRWLOCK_INIT;
inline bool g_runtimeStarted = false;

// Connects the independent components to each other and to the taskbar UI.
inline void WireComponents()
{
    // A timer/alarm was armed (possibly from the alert thread): start the
    // taskbar ticker on the XAML thread.
    g_countdown.onArmed = []
    {
        if (HWND taskbar = host::ResolveTaskbar()) {
            host::RunOnWindowThread(
                taskbar,
                [](void*)
                {
                    g_button.StartTicker();
                    g_button.Refresh();
                },
                nullptr);
        }
    };

    g_countdown.onDisarmed = []
    {
        g_button.StopTicker();
        g_button.SetIdle();
    };

    g_countdown.onFinished = [](bool restored)
    {
        g_sound.Play();

        if (!restored) {
            if (HWND taskbar = host::ResolveTaskbar()) {
                host::RunOnWindowThread(
                    taskbar,
                    [](void*)
                    {
                        if (g_unloading.load()) {
                            return;
                        }

                        g_button.StopTicker();
                        g_button.SetIdle();
                    },
                    nullptr);
            }
        }

        const std::wstring reminder = g_countdown.ReminderOrDefault();

        if (!g_alerts.Show(reminder.c_str())) {
            Wh_Log(L"ERROR: Completion alert couldn't be shown");
        }
    };

    // After the file dialog closes, reopen the flyout on the XAML thread.
    g_picker.onClosed = []
    {
        HWND taskbar = g_taskbarWnd.load();

        if (taskbar && IsWindow(taskbar)) {
            host::RunOnWindowThread(
                taskbar,
                [](void*)
                {
                    if (g_unloading.load()) {
                        return;
                    }

                    try {
                        g_button.OpenFlyout();
                    }
                    catch (...) {
                        Wh_Log(L"Reopening timer flyout failed");
                    }
                },
                nullptr);
        }
    };
}

bool EnsureShellRuntimeStarted()
{
    AcquireSRWLockExclusive(&g_runtimeLock);

    if (g_unloading.load()) {
        ReleaseSRWLockExclusive(&g_runtimeLock);
        return false;
    }

    if (g_runtimeStarted) {
        ReleaseSRWLockExclusive(&g_runtimeLock);
        return true;
    }

    if (!AlertWindow::IsRegistered() && !AlertWindow::RegisterClasses()) {
        Wh_Log(L"WARNING: Completion alert class could not be registered");
    }

    if (!g_alerts.Initialize() || !g_countdown.Initialize()) {
        Wh_Log(L"ERROR: Could not start timer runtime");
        g_countdown.Dispose();
        g_alerts.Dispose();
        ReleaseSRWLockExclusive(&g_runtimeLock);
        return false;
    }

    g_runtimeStarted = true;

    g_countdown.RestorePersisted();

    ReleaseSRWLockExclusive(&g_runtimeLock);
    return true;
}

void ApplyTimerButtonIfAvailable()
{
    if (g_unloading.load()) {
        return;
    }

    HWND taskbar = host::ResolveTaskbar();

    if (!taskbar) {
        return;
    }

    const bool ran = host::RunOnWindowThread(
        taskbar,
        [](void* param)
        {
            try {
                g_button.Attach(reinterpret_cast<HWND>(param));
            }
            catch (...) {
                Wh_Log(L"Applying timer button failed: %08X",
                       static_cast<unsigned>(winrt::to_hresult()));
            }
        },
        taskbar);

    if (!ran) {
        Wh_Log(L"Could not access taskbar UI thread");
    }
}

void OnTaskbarStarted()
{
    EnsureShellRuntimeStarted();
    ApplyTimerButtonIfAvailable();
}

void OnTrayRebuilt()
{
    ApplyTimerButtonIfAvailable();
}

inline bool TryRemoveTimerXaml()
{
    HWND target = g_taskbarWnd.load();

    if (!target || !IsWindow(target)) {
        target = host::FindCurrentProcessTaskbarWnd();
    }

    if (!target) {
        target = host::FindAnyWindowOnTaskbarThread();
    }

    if (!target) {
        return false;
    }

    struct Request {
        bool callbacksRevoked = false;
    } request;

    const bool ran = host::RunOnWindowThread(
        target,
        [](void* param)
        {
            auto* req = static_cast<Request*>(param);

            try {
                g_button.Teardown(req->callbacksRevoked);
            }
            catch (...) {
                Wh_Log(L"Removing timer button failed: %08X",
                       static_cast<unsigned>(winrt::to_hresult()));
            }
        },
        &request);

    return ran && request.callbacksRevoked;
}

inline void UpdateIdleLabelOnTaskbarThread(void*)
{
    g_button.ApplyMetrics();

    if (!g_countdown.Running()) {
        g_button.SetIdle();
    }
}

}  // namespace tct


// =============================================================================
// Windhawk entry points
// =============================================================================

BOOL Wh_ModInit()
{
    using namespace tct;

    Wh_Log(L"Taskbar Timer & Alarm loading");

    g_unloading.store(false);
    g_xamlTornDown.store(false);

    OptionStore::LoadModSettings();
    OptionStore::LoadMediaOverrides();
    g_prefs.Load();

    WireComponents();

    bool taskbarHookedAtInit = false;

    if (HMODULE taskbar = GetModuleHandleW(L"taskbar.dll")) {
        host::g_taskbarDllAttempted.store(taskbar);

        taskbarHookedAtInit = host::HookTaskbarDll(taskbar);

        if (taskbarHookedAtInit) {
            host::g_taskbarDllHooked.store(true);
        }
        else {
            Wh_Log(L"ERROR: Failed to resolve taskbar.dll symbols");
            return FALSE;
        }
    }

    bool systemTrayHookedAtInit = false;

    if (HMODULE systemTray = host::GetSystemTrayModuleHandle()) {
        host::g_systemTrayAttempted.store(systemTray);

        systemTrayHookedAtInit = host::HookSystemTray(systemTray);

        if (systemTrayHookedAtInit) {
            host::g_systemTrayHooked.store(true);
        }
        else {
            Wh_Log(L"ERROR: Failed to hook system tray symbols");
        }
    }

    if (!taskbarHookedAtInit || !systemTrayHookedAtInit) {
        HMODULE kernelbase = GetModuleHandleW(L"kernelbase.dll");

        auto loadLibraryExW =
            kernelbase ? reinterpret_cast<host::LoadLibraryExW_t>(
                             GetProcAddress(kernelbase, "LoadLibraryExW"))
                       : nullptr;

        if (loadLibraryExW) {
            WindhawkUtils::SetFunctionHook(loadLibraryExW,
                                           host::LoadLibraryExW_Hook,
                                           &host::LoadLibraryExW_Original);
        }
        else {
            Wh_Log(L"ERROR: Could not resolve LoadLibraryExW");
        }
    }

    return TRUE;
}

void Wh_ModAfterInit()
{
    using namespace tct;

    if (HMODULE taskbar = GetModuleHandleW(L"taskbar.dll")) {
        host::OnModuleLoadedTaskbar(taskbar);
    }

    if (HMODULE systemTray = host::GetSystemTrayModuleHandle()) {
        host::OnModuleLoadedSystemTray(systemTray);
    }

    if (host::FindCurrentProcessTaskbarWnd()) {
        EnsureShellRuntimeStarted();
        ApplyTimerButtonIfAvailable();
    }
}

void Wh_ModBeforeUninit()
{
    using namespace tct;

    AcquireSRWLockExclusive(&g_runtimeLock);
    g_unloading.store(true);
    ReleaseSRWLockExclusive(&g_runtimeLock);

    g_countdown.RequestStop();

    // Primary teardown attempt while the taskbar UI thread is still available.
    const bool tornDown = TryRemoveTimerXaml();

    g_xamlTornDown.store(tornDown);

    if (!tornDown) {
        Wh_Log(L"Initial timer XAML teardown failed; will retry in Wh_ModUninit");
    }
}

void Wh_ModUninit()
{
    using namespace tct;

    g_unloading.store(true);

    g_countdown.RequestStop();

    g_sound.Stop();

    // Sound threads poll g_unloading and the stop flag, so they exit within
    // ~100 ms. They must be gone before this DLL can be unloaded.
    g_sound.WaitForThreads(3000);

    // Close the file dialog (if open) and wait for its thread.
    g_picker.Shutdown();

    // Stop and join every thread that can still create/use alert UI.
    g_countdown.Join();

    // Only after the timer worker is gone can no new alert be requested by it.
    g_alerts.Shutdown();

    // The alert's WM_CLOSE handler calls Stop(), and a late sound could in
    // theory have been started in the meantime.
    g_sound.Stop();
    g_sound.WaitForThreads(1000);

    // Retry whenever the first teardown didn't complete. Loaded revokers can
    // exist even when no timer button was ever created.
    if (!g_xamlTornDown.load()) {
        const bool tornDown = TryRemoveTimerXaml();

        g_xamlTornDown.store(tornDown);

        if (!tornDown) {
            Wh_Log(L"ERROR: Could not complete final timer XAML teardown");
        }
    }

    g_countdown.Dispose();
    g_alerts.Dispose();

    AlertWindow::UnregisterClasses();

    AcquireSRWLockExclusive(&g_runtimeLock);
    g_runtimeStarted = false;
    ReleaseSRWLockExclusive(&g_runtimeLock);

    Wh_Log(L"Taskbar Timer & Alarm unloaded");
}

void Wh_ModSettingsChanged()
{
    using namespace tct;

    OptionStore::LoadModSettings();

    if (HWND taskbar = host::ResolveTaskbar()) {
        host::RunOnWindowThread(taskbar, UpdateIdleLabelOnTaskbarThread, nullptr);
    }
}
