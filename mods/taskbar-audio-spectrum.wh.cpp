// ==WindhawkMod==
// @id              taskbar-audio-spectrum
// @name            Taskbar Audio Spectrum
// @description     Render the system audio spectrum in the native Windows 10/11 taskbar search box
// @version         1.1.0
// @author          shanght
// @github          https://github.com/ShangHTao
// @homepage        https://github.com/ShangHTao/TaskbarAudioSpectrum
// @license         MIT
// @include         windhawk.exe
// @architecture    x86
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -luuid -lgdi32 -ladvapi32 -lshell32 -ldwmapi
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Taskbar Audio Spectrum

![Taskbar Audio Spectrum preview](https://raw.githubusercontent.com/ShangHTao/TaskbarAudioSpectrum/main/assets/taskbar-spectrum-preview.png)

Displays a click-through real-time spectrum over the native Windows 10 or
Windows 11 taskbar search box. System playback is captured with WASAPI
loopback, so microphone permission isn't required and the search box remains
clickable.

## Requirements

Set the Windows taskbar Search option to the full **Search box** before enabling
the mod. Do not run the standalone Taskbar Audio Spectrum application at the
same time because both versions render the same overlay.

The `thirdOctave` scale derives its number of bars from the minimum and maximum
center frequencies. `nonOctaveBarCount` applies only to Bark, logarithmic, Mel,
and linear scales. Rapid taskbar layout tracking temporarily increases geometry
checks after the taskbar changes, which helps the overlay follow animations and
layout updates.

The default 16384-point FFT prioritizes low-frequency resolution. Select 4096
for faster-looking motion when resolving the lowest third-octave bands is less
important.

Unlike **Taskbar Fluent Media Player**, this mod doesn't add a media-player
widget. Unlike **Desktop Audio Visualizer**, it renders inside the taskbar search
box. It runs in a dedicated `windhawk.exe` tool process instead of injecting the
audio and rendering runtime into Explorer.

Source, standalone downloads, and detailed documentation are available on the
[project page](https://github.com/ShangHTao/TaskbarAudioSpectrum).
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- frequencyScale: "thirdOctave"
  $name: Frequency scale
  $description: Third-octave uses the center-frequency range; other scales use the non-octave bar count.
  $options:
  - "thirdOctave": Third-octave
  - "bark": Bark
  - "log": Logarithmic
  - "mel": Mel
  - "linear": Linear
- nonOctaveBarCount: 28
  $name: Non-octave bar count
  $description: Number of bars for Bark, logarithmic, Mel, and linear scales; ignored by third-octave.
- minimumFrequency: "31.5"
  $name: Minimum frequency (Hz)
  $description: Lower analysis edge for non-octave scales; ignored by third-octave.
- maximumFrequency: "16000"
  $name: Maximum frequency (Hz)
  $description: Upper analysis edge for non-octave scales; ignored by third-octave.
- minimumCenterFrequency: "31.5"
  $name: Minimum third-octave center (Hz)
  $description: Lowest nominal center for third-octave; ignored by other scales.
- maximumCenterFrequency: "16000"
  $name: Maximum third-octave center (Hz)
  $description: Highest nominal center for third-octave; ignored by other scales.
- referenceFrequency: "1000"
  $name: Third-octave reference frequency (Hz)
  $description: Reference used to calculate nominal third-octave centers; ignored by other scales.
- bandAggregation: "energy"
  $name: Band amplitude calculation
  $description: Method used to combine FFT bins. Slaney normalization is specific to Mel; on other scales it matches energy.
  $options:
  - "peak": Peak
  - "energy": Energy
  - "mean": Mean
  - "slaney": Slaney-normalized
- frequencyWeighting: "none"
  $name: Frequency weighting
  $description: Optional perceptual weighting applied before band aggregation.
  $options:
  - "none": None
  - "A": A-weighting
  - "C": C-weighting
- foldBelowMinimum: false
  $name: Fold lower frequencies into first bar
  $description: Adds energy below the configured minimum to the first visible band on third-octave, Bark, logarithmic, and linear scales; ignored by Mel.
- fftSize: 16384
  $name: FFT size
  $description: Analysis window size. 4096 reacts faster; 16384 resolves the lowest third-octave bands more accurately but adds latency. Supported values are 4096, 8192, and 16384.
- overlapPercent: 75
  $name: FFT overlap (percent)
  $description: Overlap between consecutive analysis windows. Higher values update the analysis more often and use more CPU.
- windowFunction: "hann"
  $name: FFT window
  $description: Window function applied before each FFT.
  $options:
  - "hann": Hann
  - "hamming": Hamming
  - "blackmanHarris": Blackman-Harris
- framesPerSecond: 30
  $name: Frames per second
  $description: Maximum visual refresh rate.
- barColor: "#FF78D4"
  $name: Gradient start color
  $description: RGB color for the first spectrum bar.
- secondColor: "#00C2FF"
  $name: Gradient end color
  $description: RGB color for the last spectrum bar.
- opacity: 180
  $name: Opacity (0-255)
  $description: Alpha value used to draw bars and peaks.
- sensitivity: 100
  $name: Sensitivity (percent)
  $description: Gain applied to the analyzed spectrum before display mapping.
- minimumDecibels: "-72"
  $name: Minimum display level (dB)
  $description: Signal level mapped to an empty bar.
- maximumDecibels: "-6"
  $name: Maximum display level (dB)
  $description: Signal level mapped to a full-height bar.
- attackMs: 20
  $name: Bar attack time (ms)
  $description: Smoothing time while a bar rises.
- releaseMs: 220
  $name: Bar release time (ms)
  $description: Smoothing time while a bar falls.
- peakEnabled: true
  $name: Show peak blocks
  $description: Draws a falling peak marker above each bar.
- peakShowWhenSilent: true
  $name: Keep peak blocks visible during silence
  $description: Leaves peak markers at the baseline when their levels reach zero.
- peakHoldMs: 160
  $name: Peak hold time (ms)
  $description: Delay before a peak marker starts falling.
- peakGravity: "3.2"
  $name: Peak fall gravity
  $description: Acceleration applied to falling peak markers.
- peakHeight: 2
  $name: Peak block height (pixels)
  $description: Peak marker height at 96 DPI.
- peakGap: 1
  $name: Peak block gap (pixels)
  $description: Gap between each bar and its peak marker at 96 DPI.
- hideWhenSilent: false
  $name: Hide when silent
  $description: Hides the overlay after the signal remains below the silence threshold.
- silenceThreshold: "0.015"
  $name: Silence threshold
  $description: Normalized display level considered audible.
- silenceHideDelayMs: 500
  $name: Silence hide delay (ms)
  $description: Time below the threshold before the overlay is hidden.
- bottomPadding: 5
  $name: Bottom edge offset (pixels)
  $description: Empty space below the spectrum at 96 DPI.
- topPadding: 5
  $name: Top edge offset (pixels)
  $description: Empty space above the spectrum at 96 DPI.
- spectrumLeftOffset: 66
  $name: Left edge offset (pixels)
  $description: Space reserved for the search icon at 96 DPI.
- rightPadding: 10
  $name: Right edge offset (pixels)
  $description: Empty space at the right edge at 96 DPI.
- barWidthPercent: 48
  $name: Bar width (percent of slot)
  $description: Width of each bar relative to its allocated horizontal slot.
- autoPosition: true
  $name: Rapid taskbar layout tracking
  $description: Temporarily checks geometry more frequently after taskbar layout changes.
*/
// ==/WindhawkModSettings==

#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <windows.h>
#include <windhawk_utils.h>
#include <audioclient.h>
#include <dwmapi.h>
#include <mmdeviceapi.h>
#include <mmreg.h>
#include <shellapi.h>
#include <uiautomation.h>

#include <algorithm>
#include <array>
#include <atomic>
#include <cmath>
#include <complex>
#include <cstdint>
#include <cstring>
#include <cwchar>
#include <memory>
#include <mutex>
#include <new>
#include <optional>
#include <string>
#include <utility>
#include <vector>





namespace tas {

constexpr int kMaxBars = 64;
constexpr int kDefaultFftSamples = 16384;
constexpr int kDefaultFftOverlapPercent = 75;
constexpr DWORD kUiAutomationTimeoutMs = 2000;
constexpr wchar_t kOverlayWindowClass[] = L"TaskbarAudioSpectrumOverlay";
constexpr double kPi = 3.14159265358979323846;

class ScopedHandle {
public:
    ScopedHandle() = default;
    explicit ScopedHandle(HANDLE handle) : handle_(handle) {}
    ~ScopedHandle() { reset(); }
    ScopedHandle(const ScopedHandle&) = delete;
    ScopedHandle& operator=(const ScopedHandle&) = delete;
    ScopedHandle(ScopedHandle&& other) noexcept : handle_(other.release()) {}
    ScopedHandle& operator=(ScopedHandle&& other) noexcept {
        if (this != &other) reset(other.release());
        return *this;
    }
    HANDLE get() const { return handle_; }
    explicit operator bool() const { return handle_ != nullptr; }
    HANDLE release() {
        HANDLE handle = handle_;
        handle_ = nullptr;
        return handle;
    }
    void reset(HANDLE handle = nullptr) {
        if (handle_) CloseHandle(handle_);
        handle_ = handle;
    }

private:
    HANDLE handle_ = nullptr;
};

template <typename T>
void SafeRelease(T*& value) {
    if (value) {
        value->Release();
        value = nullptr;
    }
}

inline void EnablePerMonitorDpiAwarenessForThread() {
    using SetThreadDpiAwarenessContextFunction = HANDLE(WINAPI*)(HANDLE);
    static const auto setThreadDpiAwarenessContext = [] {
        const HMODULE user32 = GetModuleHandleW(L"user32.dll");
        return user32 ? reinterpret_cast<SetThreadDpiAwarenessContextFunction>(
                            GetProcAddress(user32,
                                           "SetThreadDpiAwarenessContext"))
                      : nullptr;
    }();
    if (setThreadDpiAwarenessContext) {
        setThreadDpiAwarenessContext(
            reinterpret_cast<HANDLE>(static_cast<LONG_PTR>(-4)));
    }
}

inline UINT GetWindowDpiOrDefault(HWND window) {
    using GetDpiForWindowFunction = UINT(WINAPI*)(HWND);
    static const auto getDpiForWindow = [] {
        const HMODULE user32 = GetModuleHandleW(L"user32.dll");
        return user32 ? reinterpret_cast<GetDpiForWindowFunction>(
                           GetProcAddress(user32, "GetDpiForWindow"))
                      : nullptr;
    }();
    if (getDpiForWindow && window) {
        const UINT dpi = getDpiForWindow(window);
        if (dpi) return dpi;
    }
    return 96;
}

}  // namespace tas



namespace tas {

enum class FrequencyScale { ThirdOctaveNominal, Bark, Logarithmic, HtkMel, Linear };
enum class BandAggregation { Peak, Energy, Mean, Slaney };
enum class FrequencyWeighting { None, A, C };
enum class WindowFunction { Hann, Hamming, BlackmanHarris };

struct Settings {
    int fftSize = kDefaultFftSamples;
    int fftOverlapPercent = kDefaultFftOverlapPercent;
    WindowFunction windowFunction = WindowFunction::Hann;
    int barCount = 28;
    int fps = 30;
    COLORREF color = RGB(255, 120, 212);
    COLORREF secondColor = RGB(0, 194, 255);
    BYTE opacity = 180;
    float sensitivity = 1.0f;
    float minimumDecibels = -72.0f;
    float maximumDecibels = -6.0f;
    int attackMs = 20;
    int releaseMs = 220;
    float minimumFrequency = 31.5f;
    float maximumFrequency = 16000.0f;
    float minimumCenterFrequency = 31.5f;
    float maximumCenterFrequency = 16000.0f;
    float referenceFrequency = 1000.0f;
    FrequencyScale frequencyScale = FrequencyScale::ThirdOctaveNominal;
    BandAggregation bandAggregation = BandAggregation::Energy;
    FrequencyWeighting frequencyWeighting = FrequencyWeighting::None;
    bool foldBelowMinimum = false;
    bool hideWhenSilent = false;
    float silenceThreshold = 0.015f;
    int silenceHideDelayMs = 500;
    bool peakEnabled = true;
    bool peakShowWhenSilent = true;
    int peakHoldMs = 160;
    float peakGravity = 3.2f;
    int peakHeight = 2;
    int peakGap = 1;
    int bottomPadding = 5;
    int topPadding = 5;
    int leftPadding = 66;
    int rightPadding = 10;
    int barWidthPercent = 48;
    bool autoPosition = true;
};

bool TokenEquals(PCWSTR text, PCWSTR expected);
bool TryParseClampedFloat(PCWSTR text, float minimum, float maximum, float* value);
bool TryParseFrequencyScale(PCWSTR text, FrequencyScale* scale);
PCWSTR FrequencyScaleName(FrequencyScale scale);
bool TryParseBandAggregation(PCWSTR text, BandAggregation* aggregation);
PCWSTR BandAggregationName(BandAggregation aggregation);
bool TryParseFrequencyWeighting(PCWSTR text, FrequencyWeighting* weighting);
PCWSTR FrequencyWeightingName(FrequencyWeighting weighting);
bool TryParseWindowFunction(PCWSTR text, WindowFunction* function);
PCWSTR WindowFunctionName(WindowFunction function);
bool IsSupportedFftSize(int fftSize);
int CalculateFftHopSamples(int fftSize, int overlapPercent);
bool TryParseColor(PCWSTR text, COLORREF* color);
Settings LoadSettings();

}  // namespace tas




namespace tas {

struct AnalysisPlan {
    int bars = 0;
    int fftSize = 0;
    int fftBins = 0;
    int hopSamples = 0;
    float sensitivity = 1.0f;
    float binWidth = 0.0f;
    double windowSumSquares = 0.0;
    float minimumDecibels = -72.0f;
    float maximumDecibels = -6.0f;
    BandAggregation bandAggregation = BandAggregation::Energy;
    bool foldBelowMinimum = false;
    std::vector<float> window;
    std::vector<float> binPowerGain;
    std::array<float, kMaxBars> centerFrequency{};
    std::array<float, kMaxBars> lowerFrequency{};
    std::array<float, kMaxBars> upperFrequency{};
    std::vector<std::vector<std::pair<int, float>>> binWeights;
};

struct PeakState { float level = 0.0f; float velocity = 0.0f; float holdSeconds = 0.0f; };
struct LevelPixelCoverage { int fullPixels = 0; float partialPixel = 0.0f; };
using BandLevels = std::array<float, kMaxBars>;

struct AnalysisScratch {
    std::vector<std::complex<float>> transformed;
    std::vector<double> spectrumPower;
};

float ReadSample(const BYTE* data, UINT32 frame, int channel,
                 const WAVEFORMATEX* format);
AnalysisPlan MakeAnalysisPlan(int sampleRate, const Settings& settings);
void AnalyzeChannels(const std::vector<std::vector<float>>& channels,
                     size_t oldestSample, const AnalysisPlan& plan,
                     AnalysisScratch* scratch, BandLevels* levels);
void ClearBands(BandLevels* levels);
float SmoothDisplayLevel(float current, float target, float deltaSeconds,
                         int attackMs, int releaseMs);
int CalculateRenderedLevelHeight(float level, int maximumHeight, int minimumHeight);
LevelPixelCoverage CalculateLevelPixelCoverage(float level, int maximumHeight);
int CalculatePeakBlockBottom(int peakLevelHeight, int spectrumTop,
                             int spectrumBottom, int peakHeight, int peakGap,
                             bool keepVisibleAtZero);
void UpdatePeak(PeakState* peak, float level, float deltaSeconds,
                float holdSeconds, float gravity);

}  // namespace tas



namespace tas {
int HostGetIntSetting(PCWSTR key, int fallback);
std::wstring HostGetStringSetting(PCWSTR key, PCWSTR fallback);
}  // namespace tas




namespace tas {

class SignalWindowTracker {
public:
    explicit SignalWindowTracker(size_t windowFrames)
        : windowFrames_(std::max<size_t>(1, windowFrames)),
          framesSinceSignal_(windowFrames_) {}

    bool ContainsSignal() const {
        return framesSinceSignal_ < windowFrames_;
    }

    void PushFrame(bool containsSignal) {
        if (containsSignal) {
            framesSinceSignal_ = 0;
        } else if (framesSinceSignal_ < windowFrames_) {
            ++framesSinceSignal_;
        }
    }

    void ResetToSilence() { framesSinceSignal_ = windowFrames_; }

private:
    size_t windowFrames_;
    size_t framesSinceSignal_;
};

class EndpointNotificationClient final : public IMMNotificationClient {
public:
    explicit EndpointNotificationClient(HANDLE changedEvent);
    ULONG STDMETHODCALLTYPE AddRef() override;
    ULONG STDMETHODCALLTYPE Release() override;
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid, void** object) override;
    HRESULT STDMETHODCALLTYPE OnDefaultDeviceChanged(EDataFlow, ERole, LPCWSTR) override;
    HRESULT STDMETHODCALLTYPE OnDeviceAdded(LPCWSTR) override;
    HRESULT STDMETHODCALLTYPE OnDeviceRemoved(LPCWSTR) override;
    HRESULT STDMETHODCALLTYPE OnDeviceStateChanged(LPCWSTR, DWORD) override;
    HRESULT STDMETHODCALLTYPE OnPropertyValueChanged(LPCWSTR, const PROPERTYKEY) override;
private:
    std::atomic<ULONG> references_{1};
    HANDLE changedEvent_;
};
DWORD WINAPI AudioThreadProc(void*);
}  // namespace tas



namespace tas {
enum class SearchHostKind { Unknown, Windows10, Windows11 };
struct SearchLayout {
    HWND taskbar = nullptr;
    DWORD explorerProcessId = 0;
    RECT rect{};
    UINT dpi = 96;
    uint64_t generation = 0;
    bool valid = false;
};
DWORD GetSearchMode();
bool IsSearchBoxMode(DWORD mode, SearchHostKind hostKind);
bool IsSearchBoxMode(DWORD mode);
int ScaleForDpi(int value, UINT dpi);
RECT CalculateSpectrumBounds(const SearchLayout& layout,
                             const Settings& settings);
bool RectEquals(const RECT& first, const RECT& second);
bool IsSearchExecutableName(PCWSTR executablePath);
SearchHostKind DetectSearchHostKind(PCWSTR executablePath);
bool IsSearchProcessWindow(HWND window);
bool IsSearchInterfaceOpen();
bool IsFullscreenForeground(const SearchLayout& layout);
bool ShouldShowOverlay(const SearchLayout& layout, DWORD searchMode,
                       bool searchInterfaceOpen,
                       bool fullscreenForeground);
DWORD WINAPI SearchLocatorThreadProc(void*);
}  // namespace tas



namespace tas {
std::wstring MakeOverlayWindowClassName(unsigned serial);
DWORD WINAPI OverlayThreadProc(void*);
}  // namespace tas




namespace tas {
struct ApplicationContext;

class Runtime {
public:
    Runtime();
    Runtime(const Runtime&) = delete;
    Runtime& operator=(const Runtime&) = delete;
    ~Runtime();

    bool Start(const Settings& settings);
    bool Stop();
    bool running() const;

private:
    ScopedHandle audioThread_;
    ScopedHandle locatorThread_;
    ScopedHandle overlayThread_;
    std::unique_ptr<ApplicationContext> context_;
};

Runtime& AppRuntime();
}  // namespace tas



namespace tas {

struct SearchLayoutState {
    std::mutex mutex;
    SearchLayout latest;
    uint64_t generation = 0;
    RECT templateTaskbarRect{};
    RECT templateSearchRect{};
    UINT templateDpi = 96;
    bool templateValid = false;
};

struct ApplicationContext {
    Settings settings;
    std::array<std::atomic<float>, kMaxBars> bands{};
    std::atomic<bool> publishedSignalActive{false};
    std::atomic<bool> visualizerActive{false};
    std::atomic<bool> locatorShutdownIncomplete{false};
    ScopedHandle stopEvent;
    ScopedHandle activityChangedEvent;
    ScopedHandle bandActivityEvent;
    ScopedHandle layoutChangedEvent;
    SearchLayoutState searchLayout;
};

void SetVisualizerActive(ApplicationContext& context, bool active);
void ClearPublishedBands(ApplicationContext& context);
void PublishBandLevels(ApplicationContext& context, const BandLevels& levels);
bool ReadLatestSearchLayout(ApplicationContext& context, SearchLayout* layout);

}  // namespace tas



namespace tas {

constexpr PCWSTR kExplorerAdvancedRegistryPath =
    L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Advanced";
constexpr PCWSTR kSearchSettingsRegistryPath =
    L"Software\\Microsoft\\Windows\\CurrentVersion\\Search";

class RegistryChangeWatcher {
public:
    RegistryChangeWatcher() = default;
    RegistryChangeWatcher(const RegistryChangeWatcher&) = delete;
    RegistryChangeWatcher& operator=(const RegistryChangeWatcher&) = delete;
    ~RegistryChangeWatcher() { Stop(); }

    bool Start(PCWSTR subKey) {
        Stop();
        changedEvent_.reset(CreateEventW(nullptr, FALSE, FALSE, nullptr));
        if (!changedEvent_) return false;
        const LSTATUS result = RegOpenKeyExW(
            HKEY_CURRENT_USER, subKey, 0, KEY_NOTIFY, &key_);
        if (result == ERROR_SUCCESS && Arm()) return true;
        Stop();
        return false;
    }

    bool Arm() const {
        return key_ && changedEvent_ &&
               RegNotifyChangeKeyValue(
                   key_, FALSE, REG_NOTIFY_CHANGE_LAST_SET,
                   changedEvent_.get(), TRUE) == ERROR_SUCCESS;
    }

    void Stop() {
        if (key_) RegCloseKey(key_);
        key_ = nullptr;
        changedEvent_.reset();
    }

    HANDLE changedEvent() const { return changedEvent_.get(); }

private:
    HKEY key_ = nullptr;
    ScopedHandle changedEvent_;
};

}  // namespace tas



namespace tas {

namespace {

int HexValue(wchar_t character) {
    if (character >= L'0' && character <= L'9') return character - L'0';
    if (character >= L'a' && character <= L'f') return character - L'a' + 10;
    if (character >= L'A' && character <= L'F') return character - L'A' + 10;
    return -1;
}

int ThirdOctaveBandCount(float minimumCenter, float maximumCenter,
                         float referenceFrequency) {
    if (minimumCenter <= 0.0f || maximumCenter < minimumCenter ||
        referenceFrequency <= 0.0f) return 0;
    const int first = static_cast<int>(std::lround(
        3.0 * std::log2(minimumCenter / referenceFrequency)));
    const int last = static_cast<int>(std::lround(
        3.0 * std::log2(maximumCenter / referenceFrequency)));
    return last - first + 1;
}

}  // namespace

bool TokenEquals(PCWSTR text, PCWSTR expected) {
    if (!text || !expected) return false;
    while (*text == L' ' || *text == L'\t') ++text;
    const wchar_t* end = text + wcslen(text);
    while (end > text && (end[-1] == L' ' || end[-1] == L'\t')) --end;
    const size_t length = static_cast<size_t>(end - text);
    return length == wcslen(expected) &&
           _wcsnicmp(text, expected, length) == 0;
}



bool TryParseClampedFloat(PCWSTR text, float minimum, float maximum,
                          float* value) {
    if (!text || !value || !std::isfinite(minimum) ||
        !std::isfinite(maximum) || minimum > maximum) {
        return false;
    }
    while (*text == L' ' || *text == L'\t') ++text;
    if (!*text) return false;
    wchar_t* numberEnd = nullptr;
    const double number = wcstod(text, &numberEnd);
    if (numberEnd == text || !std::isfinite(number)) return false;
    while (*numberEnd == L' ' || *numberEnd == L'\t') ++numberEnd;
    if (*numberEnd) return false;
    *value = std::clamp(static_cast<float>(number), minimum, maximum);
    return true;
}

bool TryParseFrequencyScale(PCWSTR text, FrequencyScale* scale) {
    if (!scale) return false;
    if (TokenEquals(text, L"thirdOctave") ||
        TokenEquals(text, L"octave") ||
        TokenEquals(text, L"third-octave") ||
        TokenEquals(text, L"third_octave") ||
        TokenEquals(text, L"thirdOctaveNominal") ||
        TokenEquals(text, L"1/3 octave")) {
        *scale = FrequencyScale::ThirdOctaveNominal;
        return true;
    }
    if (TokenEquals(text, L"bark")) {
        *scale = FrequencyScale::Bark;
        return true;
    }
    if (TokenEquals(text, L"log") ||
        TokenEquals(text, L"logarithmic")) {
        *scale = FrequencyScale::Logarithmic;
        return true;
    }
    if (TokenEquals(text, L"mel") || TokenEquals(text, L"htk-mel") ||
        TokenEquals(text, L"htk_mel") || TokenEquals(text, L"htkmel")) {
        *scale = FrequencyScale::HtkMel;
        return true;
    }
    if (TokenEquals(text, L"linear")) {
        *scale = FrequencyScale::Linear;
        return true;
    }
    return false;
}

PCWSTR FrequencyScaleName(FrequencyScale scale) {
    switch (scale) {
        case FrequencyScale::ThirdOctaveNominal:
            return L"thirdOctave";
        case FrequencyScale::Bark:
            return L"bark";
        case FrequencyScale::HtkMel:
            return L"mel";
        case FrequencyScale::Linear:
            return L"linear";
        default:
            return L"log";
    }
}

bool TryParseBandAggregation(PCWSTR text, BandAggregation* aggregation) {
    if (!aggregation) return false;
    if (TokenEquals(text, L"peak") || TokenEquals(text, L"peakDb") ||
        TokenEquals(text, L"max")) {
        *aggregation = BandAggregation::Peak;
        return true;
    }
    if (TokenEquals(text, L"energy") || TokenEquals(text, L"sum")) {
        *aggregation = BandAggregation::Energy;
        return true;
    }
    if (TokenEquals(text, L"mean") || TokenEquals(text, L"average") ||
        TokenEquals(text, L"density")) {
        *aggregation = BandAggregation::Mean;
        return true;
    }
    if (TokenEquals(text, L"slaney") ||
        TokenEquals(text, L"slaneyPower")) {
        *aggregation = BandAggregation::Slaney;
        return true;
    }
    return false;
}

PCWSTR BandAggregationName(BandAggregation aggregation) {
    switch (aggregation) {
        case BandAggregation::Energy:
            return L"energy";
        case BandAggregation::Mean:
            return L"mean";
        case BandAggregation::Slaney:
            return L"slaney";
        default:
            return L"peak";
    }
}

bool TryParseFrequencyWeighting(PCWSTR text, FrequencyWeighting* weighting) {
    if (!weighting) return false;
    if (TokenEquals(text, L"none") || TokenEquals(text, L"off") ||
        TokenEquals(text, L"flat")) {
        *weighting = FrequencyWeighting::None;
        return true;
    }
    if (TokenEquals(text, L"a") || TokenEquals(text, L"aWeighting") ||
        TokenEquals(text, L"a-weighting")) {
        *weighting = FrequencyWeighting::A;
        return true;
    }
    if (TokenEquals(text, L"c") || TokenEquals(text, L"cWeighting") ||
        TokenEquals(text, L"c-weighting")) {
        *weighting = FrequencyWeighting::C;
        return true;
    }
    return false;
}

PCWSTR FrequencyWeightingName(FrequencyWeighting weighting) {
    switch (weighting) {
        case FrequencyWeighting::A:
            return L"A";
        case FrequencyWeighting::C:
            return L"C";
        default:
            return L"none";
    }
}

bool TryParseWindowFunction(PCWSTR text, WindowFunction* function) {
    if (!function) return false;
    if (TokenEquals(text, L"hann") || TokenEquals(text, L"hanning")) {
        *function = WindowFunction::Hann;
        return true;
    }
    if (TokenEquals(text, L"hamming")) {
        *function = WindowFunction::Hamming;
        return true;
    }
    if (TokenEquals(text, L"blackmanHarris") ||
        TokenEquals(text, L"blackman-harris") ||
        TokenEquals(text, L"blackman_harris")) {
        *function = WindowFunction::BlackmanHarris;
        return true;
    }
    return false;
}

PCWSTR WindowFunctionName(WindowFunction function) {
    switch (function) {
        case WindowFunction::Hamming:
            return L"hamming";
        case WindowFunction::BlackmanHarris:
            return L"blackmanHarris";
        default:
            return L"hann";
    }
}

bool IsSupportedFftSize(int fftSize) {
    return fftSize == 4096 || fftSize == 8192 || fftSize == 16384;
}

int CalculateFftHopSamples(int fftSize, int overlapPercent) {
    if (!IsSupportedFftSize(fftSize)) fftSize = kDefaultFftSamples;
    overlapPercent = std::clamp(overlapPercent, 0, 95);
    return std::max(
        1, (fftSize * (100 - overlapPercent) + 50) / 100);
}


bool TryParseColor(PCWSTR text, COLORREF* color) {
    if (!color || !text || text[0] != L'#' || wcslen(text) != 7) {
        return false;
    }
    int digits[6];
    for (int index = 0; index < 6; ++index) {
        digits[index] = HexValue(text[index + 1]);
        if (digits[index] < 0) return false;
    }
    *color = RGB(digits[0] * 16 + digits[1],
                 digits[2] * 16 + digits[3],
                 digits[4] * 16 + digits[5]);
    return true;
}


Settings LoadSettings() {
    Settings loaded;
    const int configuredFftSize =
        HostGetIntSetting(L"fftSize", kDefaultFftSamples);
    if (!IsSupportedFftSize(configuredFftSize)) {
        Wh_Log(L"Unsupported fftSize=%d; using %d",
            configuredFftSize, kDefaultFftSamples);
        loaded.fftSize = kDefaultFftSamples;
    } else {
        loaded.fftSize = configuredFftSize;
    }
    loaded.fftOverlapPercent = std::clamp(
        HostGetIntSetting(L"overlapPercent", kDefaultFftOverlapPercent),
        0, 95);
    const std::wstring windowFunction =
        HostGetStringSetting(L"windowFunction", L"hann");
    if (!TryParseWindowFunction(windowFunction.c_str(),
                                &loaded.windowFunction)) {
        loaded.windowFunction = WindowFunction::Hann;
        Wh_Log(L"Invalid windowFunction '%ls'; using hann",
            windowFunction.c_str());
    }
    loaded.barCount = std::clamp(
        HostGetIntSetting(L"nonOctaveBarCount", 28), 8, kMaxBars);
    loaded.fps = std::clamp(
        HostGetIntSetting(L"framesPerSecond", 30), 1, 100);
    loaded.opacity = static_cast<BYTE>(std::clamp(
        HostGetIntSetting(L"opacity", 180), 0, 255));
    loaded.sensitivity = std::clamp(
        HostGetIntSetting(L"sensitivity", 100), 25, 500) / 100.0f;
    const std::wstring minimumDecibels =
        HostGetStringSetting(L"minimumDecibels", L"-72");
    const std::wstring maximumDecibels =
        HostGetStringSetting(L"maximumDecibels", L"-6");
    const bool minimumDecibelsValid = TryParseClampedFloat(
        minimumDecibels.c_str(), -160.0f, -1.0f,
        &loaded.minimumDecibels);
    const bool maximumDecibelsValid = TryParseClampedFloat(
        maximumDecibels.c_str(), -120.0f, 0.0f,
        &loaded.maximumDecibels);
    if (!minimumDecibelsValid || !maximumDecibelsValid ||
        loaded.maximumDecibels <= loaded.minimumDecibels) {
        Wh_Log(L"Invalid spectrum dB range '%ls' to '%ls'; using -72 to -6",
            minimumDecibels.c_str(), maximumDecibels.c_str());
        loaded.minimumDecibels = -72.0f;
        loaded.maximumDecibels = -6.0f;
    }
    loaded.attackMs = std::clamp(
        HostGetIntSetting(L"attackMs", 20), 0, 2000);
    loaded.releaseMs = std::clamp(
        HostGetIntSetting(L"releaseMs", 220), 0, 5000);
    const std::wstring minimumFrequency =
        HostGetStringSetting(L"minimumFrequency", L"31.5");
    if (!TryParseClampedFloat(minimumFrequency.c_str(), 1.0f, 20000.0f,
                              &loaded.minimumFrequency)) {
        loaded.minimumFrequency = 31.5f;
        Wh_Log(L"Invalid minimumFrequency '%ls'; using 31.5",
            minimumFrequency.c_str());
    }
    const std::wstring maximumFrequency =
        HostGetStringSetting(L"maximumFrequency", L"16000");
    if (!TryParseClampedFloat(maximumFrequency.c_str(), 20.0f, 24000.0f,
                              &loaded.maximumFrequency)) {
        loaded.maximumFrequency = 16000.0f;
        Wh_Log(L"Invalid maximumFrequency '%ls'; using 16000",
            maximumFrequency.c_str());
    }
    if (loaded.maximumFrequency <= loaded.minimumFrequency) {
        Wh_Log(L"Invalid frequency range %.2f-%.2f Hz; using 31.5-16000 Hz",
            loaded.minimumFrequency, loaded.maximumFrequency);
        loaded.minimumFrequency = 31.5f;
        loaded.maximumFrequency = 16000.0f;
    }
    const std::wstring minimumCenterFrequency =
        HostGetStringSetting(L"minimumCenterFrequency", L"31.5");
    if (!TryParseClampedFloat(minimumCenterFrequency.c_str(), 1.0f, 20000.0f,
                              &loaded.minimumCenterFrequency)) {
        loaded.minimumCenterFrequency = 31.5f;
        Wh_Log(L"Invalid minCenterFrequency '%ls'; using 31.5",
            minimumCenterFrequency.c_str());
    }
    const std::wstring maximumCenterFrequency =
        HostGetStringSetting(L"maximumCenterFrequency", L"16000");
    if (!TryParseClampedFloat(maximumCenterFrequency.c_str(), 20.0f, 24000.0f,
                              &loaded.maximumCenterFrequency)) {
        loaded.maximumCenterFrequency = 16000.0f;
        Wh_Log(L"Invalid maxCenterFrequency '%ls'; using 16000",
            maximumCenterFrequency.c_str());
    }
    const std::wstring referenceFrequency =
        HostGetStringSetting(L"referenceFrequency", L"1000");
    if (!TryParseClampedFloat(referenceFrequency.c_str(), 20.0f, 20000.0f,
                              &loaded.referenceFrequency)) {
        loaded.referenceFrequency = 1000.0f;
        Wh_Log(L"Invalid referenceFrequency '%ls'; using 1000",
            referenceFrequency.c_str());
    }
    if (loaded.maximumCenterFrequency <= loaded.minimumCenterFrequency) {
        Wh_Log(L"Invalid nominal center range %.2f-%.2f Hz; using 31.5-16000 Hz",
            loaded.minimumCenterFrequency, loaded.maximumCenterFrequency);
        loaded.minimumCenterFrequency = 31.5f;
        loaded.maximumCenterFrequency = 16000.0f;
    }
    const std::wstring frequencyScale =
        HostGetStringSetting(L"frequencyScale", L"thirdOctave");
    if (!TryParseFrequencyScale(frequencyScale.c_str(),
                                &loaded.frequencyScale)) {
        loaded.frequencyScale = FrequencyScale::ThirdOctaveNominal;
        Wh_Log(L"Invalid frequencyScale '%ls'; using thirdOctave",
            frequencyScale.c_str());
    }
    if (loaded.frequencyScale == FrequencyScale::ThirdOctaveNominal) {
        const int nominalBandCount = ThirdOctaveBandCount(
            loaded.minimumCenterFrequency, loaded.maximumCenterFrequency,
            loaded.referenceFrequency);
        if (nominalBandCount < 8 || nominalBandCount > kMaxBars) {
            Wh_Log(L"Invalid third-octave center range; using 31.5-16000 Hz");
            loaded.minimumCenterFrequency = 31.5f;
            loaded.maximumCenterFrequency = 16000.0f;
            loaded.referenceFrequency = 1000.0f;
            loaded.barCount = 28;
        } else {
            loaded.barCount = nominalBandCount;
        }
    }
    const std::wstring bandAggregation =
        HostGetStringSetting(L"bandAggregation", L"energy");
    if (!TryParseBandAggregation(bandAggregation.c_str(),
                                 &loaded.bandAggregation)) {
        loaded.bandAggregation = BandAggregation::Energy;
        Wh_Log(L"Invalid bandAggregation '%ls'; using energy",
            bandAggregation.c_str());
    }
    const std::wstring frequencyWeighting =
        HostGetStringSetting(L"frequencyWeighting", L"none");
    if (!TryParseFrequencyWeighting(frequencyWeighting.c_str(),
                                    &loaded.frequencyWeighting)) {
        loaded.frequencyWeighting = FrequencyWeighting::None;
        Wh_Log(L"Invalid frequencyWeighting '%ls'; using none",
            frequencyWeighting.c_str());
    }
    loaded.foldBelowMinimum =
        HostGetIntSetting(L"foldBelowMinimum", 0) != 0;
    loaded.hideWhenSilent = HostGetIntSetting(L"hideWhenSilent", 0) != 0;
    const std::wstring silenceThreshold =
        HostGetStringSetting(L"silenceThreshold", L"0.015");
    if (!TryParseClampedFloat(silenceThreshold.c_str(), 0.0f, 1.0f,
                              &loaded.silenceThreshold)) {
        loaded.silenceThreshold = 0.015f;
        Wh_Log(L"Invalid silenceThreshold '%ls'; using 0.015",
            silenceThreshold.c_str());
    }
    loaded.silenceHideDelayMs = std::clamp(
        HostGetIntSetting(L"silenceHideDelayMs", 500), 0, 10000);
    loaded.peakEnabled = HostGetIntSetting(L"peakEnabled", 1) != 0;
    loaded.peakShowWhenSilent =
        HostGetIntSetting(L"peakShowWhenSilent", 1) != 0;
    loaded.peakHoldMs = std::clamp(
        HostGetIntSetting(L"peakHoldMs", 160), 0, 5000);
    const std::wstring peakGravity =
        HostGetStringSetting(L"peakGravity", L"3.2");
    if (!TryParseClampedFloat(peakGravity.c_str(), 0.1f, 50.0f,
                              &loaded.peakGravity)) {
        loaded.peakGravity = 3.2f;
        Wh_Log(L"Invalid peakGravity '%ls'; using 3.2", peakGravity.c_str());
    }
    loaded.peakHeight = std::clamp(
        HostGetIntSetting(L"peakHeight", 2), 1, 8);
    loaded.peakGap = std::clamp(
        HostGetIntSetting(L"peakGap", 1), 0, 8);
    loaded.bottomPadding = std::clamp(
        HostGetIntSetting(L"bottomPadding", 5), 0, 12);
    loaded.topPadding = std::clamp(
        HostGetIntSetting(L"topPadding", 5), 0, 12);
    loaded.rightPadding = std::clamp(
        HostGetIntSetting(L"rightPadding", 10), 0, 40);
    loaded.barWidthPercent = std::clamp(
        HostGetIntSetting(L"barWidthPercent", 48), 15, 95);
    const std::wstring barColor =
        HostGetStringSetting(L"barColor", L"#FF78D4");
    if (!TryParseColor(barColor.c_str(), &loaded.color)) {
        loaded.color = RGB(255, 120, 212);
        Wh_Log(L"Invalid barColor '%ls'; using #FF78D4", barColor.c_str());
    }
    const std::wstring secondColor =
        HostGetStringSetting(L"secondColor", L"#00C2FF");
    if (!TryParseColor(secondColor.c_str(), &loaded.secondColor)) {
        loaded.secondColor = RGB(0, 194, 255);
        Wh_Log(L"Invalid secondColor '%ls'; using #00C2FF",
            secondColor.c_str());
    }

    loaded.leftPadding = std::clamp(
        HostGetIntSetting(L"spectrumLeftOffset", 66), 0, 4096);
    loaded.autoPosition = HostGetIntSetting(L"autoPosition", 1) != 0;


    Wh_Log(L"Settings loaded: fft=%d window=%ls "
        L"overlap=%d%% hop=%d bars=%d fps=%d opacity=%u "
        L"sensitivity=%.2f dB=%.1f..%.1f attack=%dms release=%dms "
        L"range=%.2f-%.2f centers=%.2f-%.2f/ref=%.1f "
        L"scale=%ls aggregation=%ls weighting=%ls foldLow=%d "
        L"auto=%d insets=%d/%d/%d/%d "
        L"peak=%d/%dms/%.2f/%d/%d showSilent=%d "
        L"silence=%d/%.3f/%dms "
        L"colors=#%02X%02X%02X/#%02X%02X%02X",
        loaded.fftSize,
        WindowFunctionName(loaded.windowFunction),
        loaded.fftOverlapPercent,
        CalculateFftHopSamples(loaded.fftSize, loaded.fftOverlapPercent),
        loaded.barCount, loaded.fps,
        loaded.opacity, loaded.sensitivity,
        loaded.minimumDecibels, loaded.maximumDecibels,
        loaded.attackMs, loaded.releaseMs,
        loaded.minimumFrequency, loaded.maximumFrequency,
        loaded.minimumCenterFrequency, loaded.maximumCenterFrequency,
        loaded.referenceFrequency,
        FrequencyScaleName(loaded.frequencyScale),
        BandAggregationName(loaded.bandAggregation),
        FrequencyWeightingName(loaded.frequencyWeighting),
        loaded.foldBelowMinimum,
        loaded.autoPosition, loaded.leftPadding, loaded.rightPadding,
        loaded.topPadding, loaded.bottomPadding,
        loaded.peakEnabled, loaded.peakHoldMs, loaded.peakGravity,
        loaded.peakHeight, loaded.peakGap, loaded.peakShowWhenSilent,
        loaded.hideWhenSilent, loaded.silenceThreshold,
        loaded.silenceHideDelayMs,
        GetRValue(loaded.color), GetGValue(loaded.color), GetBValue(loaded.color),
        GetRValue(loaded.secondColor), GetGValue(loaded.secondColor),
        GetBValue(loaded.secondColor));
    return loaded;
}

}  // namespace tas


namespace tas {

namespace {

constexpr GUID kIeeeFloatSubformat = {
    WAVE_FORMAT_IEEE_FLOAT, 0x0000, 0x0010,
    {0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71}};

}  // namespace

void ClearBands(BandLevels* levels) {
    if (levels) levels->fill(0.0f);
}

float ReadSample(const BYTE* data, UINT32 frame, int channel,
                 const WAVEFORMATEX* format) {
    if (!data || !format) return 0.0f;
    const int channels = format->nChannels;
    const int bits = format->wBitsPerSample;
    const int bytesPerSample = channels > 0 ? format->nBlockAlign / channels : 0;
    if (channel < 0 || channel >= channels || bytesPerSample <= 0) return 0.0f;
    const BYTE* sample = data + static_cast<size_t>(frame) * format->nBlockAlign +
                         static_cast<size_t>(channel) * bytesPerSample;

    bool isFloat = format->wFormatTag == WAVE_FORMAT_IEEE_FLOAT;
    if (format->wFormatTag == WAVE_FORMAT_EXTENSIBLE &&
        format->cbSize >= sizeof(WAVEFORMATEXTENSIBLE) - sizeof(WAVEFORMATEX)) {
        const auto* extensible =
            reinterpret_cast<const WAVEFORMATEXTENSIBLE*>(format);
        isFloat = IsEqualGUID(extensible->SubFormat, kIeeeFloatSubformat);
    }
    if (isFloat && bits == 32) {
        float value;
        memcpy(&value, sample, sizeof(value));
        return std::isfinite(value) ? value : 0.0f;
    }
    if (isFloat && bits == 64) {
        double value;
        memcpy(&value, sample, sizeof(value));
        return std::isfinite(value) ? static_cast<float>(value) : 0.0f;
    }
    if (bits == 8) return (static_cast<int>(sample[0]) - 128) / 128.0f;
    if (bits == 16) {
        int16_t value;
        memcpy(&value, sample, sizeof(value));
        return value / 32768.0f;
    }
    if (bits == 24) {
        int32_t value = sample[0] | (sample[1] << 8) | (sample[2] << 16);
        if (value & 0x800000) value |= ~0xFFFFFF;
        return value / 8388608.0f;
    }
    if (bits == 32) {
        int32_t value;
        memcpy(&value, sample, sizeof(value));
        return value / 2147483648.0f;
    }
    return 0.0f;
}

namespace {

double FrequencyToScale(double frequency, FrequencyScale scale) {
    switch (scale) {
        case FrequencyScale::Bark:
            return 26.81 * frequency / (1960.0 + frequency) - 0.53;
        case FrequencyScale::HtkMel:
            return 2595.0 * std::log10(1.0 + frequency / 700.0);
        case FrequencyScale::Linear:
            return frequency;
        default:
            return std::log(frequency);
    }
}

double ScaleToFrequency(double value, FrequencyScale scale) {
    switch (scale) {
        case FrequencyScale::Bark:
            return 1960.0 / (26.81 / (value + 0.53) - 1.0);
        case FrequencyScale::HtkMel:
            return 700.0 * (std::pow(10.0, value / 2595.0) - 1.0);
        case FrequencyScale::Linear:
            return value;
        default:
            return std::exp(value);
    }
}

float FrequencyWeightingDecibels(float frequency,
                                 FrequencyWeighting weighting) {
    if (weighting == FrequencyWeighting::None || frequency <= 0.0f) {
        return 0.0f;
    }
    const double frequencySquared =
        static_cast<double>(frequency) * frequency;
    constexpr double kReferenceSquared = 12194.0 * 12194.0;
    if (weighting == FrequencyWeighting::A) {
        const double response =
            kReferenceSquared * frequencySquared * frequencySquared /
            ((frequencySquared + 20.6 * 20.6) *
             std::sqrt((frequencySquared + 107.7 * 107.7) *
                       (frequencySquared + 737.9 * 737.9)) *
             (frequencySquared + kReferenceSquared));
        return static_cast<float>(2.0 + 20.0 * std::log10(
            std::max(response, 1.0e-30)));
    }
    const double response =
        kReferenceSquared * frequencySquared /
        ((frequencySquared + 20.6 * 20.6) *
         (frequencySquared + kReferenceSquared));
    return static_cast<float>(0.06 + 20.0 * std::log10(
        std::max(response, 1.0e-30)));
}

float WindowValue(int index, int size, WindowFunction function) {
    const double phase = 2.0 * kPi * index / std::max(1, size - 1);
    switch (function) {
        case WindowFunction::Hamming:
            return static_cast<float>(0.54 - 0.46 * std::cos(phase));
        case WindowFunction::BlackmanHarris:
            return static_cast<float>(
                0.35875 - 0.48829 * std::cos(phase) +
                0.14128 * std::cos(2.0 * phase) -
                0.01168 * std::cos(3.0 * phase));
        default:
            return static_cast<float>(0.5 - 0.5 * std::cos(phase));
    }
}

void FourierTransform(std::vector<std::complex<float>>* values) {
    if (!values || values->empty()) return;
    const size_t size = values->size();
    for (size_t index = 1, reversed = 0; index < size; ++index) {
        size_t bit = size >> 1;
        while (reversed & bit) {
            reversed ^= bit;
            bit >>= 1;
        }
        reversed ^= bit;
        if (index < reversed) std::swap((*values)[index], (*values)[reversed]);
    }
    for (size_t length = 2; length <= size; length <<= 1) {
        const float angle = static_cast<float>(-2.0 * kPi / length);
        const std::complex<float> step(std::cos(angle), std::sin(angle));
        for (size_t offset = 0; offset < size; offset += length) {
            std::complex<float> rotation(1.0f, 0.0f);
            const size_t half = length >> 1;
            for (size_t index = 0; index < half; ++index) {
                const std::complex<float> even = (*values)[offset + index];
                const std::complex<float> odd =
                    (*values)[offset + index + half] * rotation;
                (*values)[offset + index] = even + odd;
                (*values)[offset + index + half] = even - odd;
                rotation *= step;
            }
        }
    }
}

void AccumulateChannelPower(const std::vector<float>& samples,
                            size_t oldestSample,
                            std::vector<double>* accumulatedPower,
                            std::vector<std::complex<float>>* transformed,
                            const AnalysisPlan& plan) {
    if (!accumulatedPower || !transformed || samples.size() !=
            static_cast<size_t>(plan.fftSize)) return;
    transformed->resize(plan.fftSize);
    double mean = 0.0;
    for (int index = 0; index < plan.fftSize; ++index) {
        mean += samples[(oldestSample + index) % plan.fftSize];
    }
    mean /= plan.fftSize;
    for (int index = 0; index < plan.fftSize; ++index) {
        const size_t source = (oldestSample + index) % plan.fftSize;
        (*transformed)[index] =
            static_cast<float>(samples[source] - mean) * plan.window[index];
    }
    FourierTransform(transformed);
    for (int bin = 1; bin < plan.fftBins; ++bin) {
        (*accumulatedPower)[bin] += std::norm((*transformed)[bin]);
    }
}

double InterpolateSpectrumPower(const std::vector<double>& spectrumPower,
                                float frequency,
                                const AnalysisPlan& plan) {
    if (spectrumPower.size() < 2 || plan.binWidth <= 0.0f) return 0.0;
    const float coordinate = std::clamp(
        frequency / plan.binWidth, 1.0f,
        static_cast<float>(plan.fftBins - 1));
    const int lowerBin = static_cast<int>(std::floor(coordinate));
    const int upperBin = std::min(lowerBin + 1, plan.fftBins - 1);
    const float fraction = coordinate - lowerBin;
    const double lowerPower =
        spectrumPower[lowerBin] * plan.binPowerGain[lowerBin];
    const double upperPower =
        spectrumPower[upperBin] * plan.binPowerGain[upperBin];
    return lowerPower * (1.0f - fraction) + upperPower * fraction;
}

double PeakBandPower(const std::vector<double>& spectrumPower, int band,
                      const AnalysisPlan& plan) {
    const float lower = band == 0 && plan.foldBelowMinimum
        ? plan.binWidth : plan.lowerFrequency[band];
    const float upper = plan.upperFrequency[band];
    double peak = std::max(
        InterpolateSpectrumPower(spectrumPower, lower, plan),
        InterpolateSpectrumPower(spectrumPower, upper, plan));
    const int firstBin = std::max(
        1, static_cast<int>(std::ceil(lower / plan.binWidth)));
    const int lastBin = std::min(
        plan.fftBins - 1,
        static_cast<int>(std::floor(upper / plan.binWidth)));
    for (int bin = firstBin; bin <= lastBin; ++bin) {
        peak = std::max(
            peak, spectrumPower[bin] * plan.binPowerGain[bin]);
    }
    return peak;
}

void PublishBandPower(const std::vector<double>& spectrumPower,
                      size_t channels, const AnalysisPlan& plan,
                      BandLevels* levels) {
    if (!levels || !channels || plan.bars <= 0 ||
        plan.windowSumSquares <= 0.0) return;
    const double normalization =
        2.0 / (plan.fftSize * plan.windowSumSquares * channels);
    const float gainDecibels =
        20.0f * std::log10(std::max(0.01f, plan.sensitivity));
    const float decibelSpan =
        plan.maximumDecibels - plan.minimumDecibels;

    for (int band = 0; band < plan.bars; ++band) {
        double power = 0.0;
        if (plan.bandAggregation == BandAggregation::Peak) {
            power = PeakBandPower(spectrumPower, band, plan);
        } else {
            double weightSum = 0.0;
            for (const auto& [bin, weight] : plan.binWeights[band]) {
                power += spectrumPower[bin] * plan.binPowerGain[bin] * weight;
                weightSum += weight;
            }
            if (plan.bandAggregation == BandAggregation::Mean &&
                weightSum > 0.0) {
                power /= weightSum;
            }
        }
        power *= normalization;
        const float decibels = static_cast<float>(
            10.0 * std::log10(std::max(power, 1.0e-16))) + gainDecibels;
        const float level = std::clamp(
            (decibels - plan.minimumDecibels) / decibelSpan,
            0.0f, 1.0f);
        (*levels)[band] = level;
    }
    for (int band = plan.bars; band < kMaxBars; ++band) {
        (*levels)[band] = 0.0f;
    }
}

}  // namespace

AnalysisPlan MakeAnalysisPlan(int sampleRate, const Settings& settings) {
    AnalysisPlan plan;
    if (sampleRate <= 0) return plan;
    plan.bars = std::clamp(settings.barCount, 1, kMaxBars);
    plan.binWeights.resize(kMaxBars);
    plan.fftSize = IsSupportedFftSize(settings.fftSize)
        ? settings.fftSize : kDefaultFftSamples;
    plan.fftBins = plan.fftSize / 2 + 1;
    plan.hopSamples = CalculateFftHopSamples(
        plan.fftSize, settings.fftOverlapPercent);
    plan.sensitivity = settings.sensitivity;
    plan.minimumDecibels = settings.minimumDecibels;
    plan.maximumDecibels = settings.maximumDecibels;
    plan.bandAggregation = settings.bandAggregation;
    plan.foldBelowMinimum = settings.foldBelowMinimum &&
        settings.frequencyScale != FrequencyScale::HtkMel;
    plan.binWidth = sampleRate / static_cast<float>(plan.fftSize);
    plan.binPowerGain.resize(plan.fftBins, 1.0f);
    for (int bin = 1; bin < plan.fftBins; ++bin) {
        const float weightingDecibels = FrequencyWeightingDecibels(
            bin * plan.binWidth, settings.frequencyWeighting);
        plan.binPowerGain[bin] = std::pow(10.0f, weightingDecibels / 10.0f);
    }
    plan.window.resize(plan.fftSize);
    for (int index = 0; index < plan.fftSize; ++index) {
        const float value = WindowValue(
            index, plan.fftSize, settings.windowFunction);
        plan.window[index] = value;
        plan.windowSumSquares += static_cast<double>(value) * value;
    }
    if (settings.frequencyScale == FrequencyScale::ThirdOctaveNominal) {
        const int firstIndex = static_cast<int>(std::lround(
            3.0 * std::log2(settings.minimumCenterFrequency /
                            settings.referenceFrequency)));
        const int lastIndex = static_cast<int>(std::lround(
            3.0 * std::log2(settings.maximumCenterFrequency /
                            settings.referenceFrequency)));
        plan.bars = std::clamp(lastIndex - firstIndex + 1, 0, kMaxBars);
        const double halfBandRatio = std::pow(2.0, 1.0 / 6.0);
        for (int band = 0; band < plan.bars; ++band) {
            const float center = static_cast<float>(
                settings.referenceFrequency *
                std::pow(2.0, (firstIndex + band) / 3.0));
            plan.centerFrequency[band] = center;
            plan.lowerFrequency[band] = static_cast<float>(
                center / halfBandRatio);
            plan.upperFrequency[band] = std::min(
                static_cast<float>(center * halfBandRatio),
                sampleRate * 0.5f);
        }
    } else if (settings.frequencyScale == FrequencyScale::HtkMel) {
        const float minimumFrequency = settings.minimumFrequency;
        const float maximumFrequency =
            std::min(settings.maximumFrequency, sampleRate * 0.5f);
        if (minimumFrequency <= 0.0f || maximumFrequency <= minimumFrequency) {
            plan.bars = 0;
            return plan;
        }
        const double scaledMinimum = FrequencyToScale(
            minimumFrequency, FrequencyScale::HtkMel);
        const double scaledMaximum = FrequencyToScale(
            maximumFrequency, FrequencyScale::HtkMel);
        std::vector<float> melPoints(plan.bars + 2);
        for (int point = 0; point < plan.bars + 2; ++point) {
            melPoints[point] = static_cast<float>(ScaleToFrequency(
                scaledMinimum + (scaledMaximum - scaledMinimum) * point /
                    (plan.bars + 1),
                FrequencyScale::HtkMel));
        }
        for (int band = 0; band < plan.bars; ++band) {
            plan.lowerFrequency[band] = melPoints[band];
            plan.centerFrequency[band] = melPoints[band + 1];
            plan.upperFrequency[band] = melPoints[band + 2];
        }
    } else {
        const float minimumFrequency = settings.minimumFrequency;
        const float maximumFrequency =
            std::min(settings.maximumFrequency, sampleRate * 0.5f);
        if (minimumFrequency <= 0.0f || maximumFrequency <= minimumFrequency) {
            plan.bars = 0;
            return plan;
        }
        const double scaledMinimum = FrequencyToScale(
            minimumFrequency, settings.frequencyScale);
        const double scaledMaximum = FrequencyToScale(
            maximumFrequency, settings.frequencyScale);
        for (int band = 0; band < plan.bars; ++band) {
            const float lower = static_cast<float>(ScaleToFrequency(
                scaledMinimum + (scaledMaximum - scaledMinimum) * band /
                    plan.bars,
                settings.frequencyScale));
            const float upper = band + 1 == plan.bars ? maximumFrequency :
                static_cast<float>(ScaleToFrequency(
                    scaledMinimum + (scaledMaximum - scaledMinimum) *
                        (band + 1) / plan.bars,
                    settings.frequencyScale));
            plan.lowerFrequency[band] = lower;
            plan.upperFrequency[band] = upper;
            plan.centerFrequency[band] = static_cast<float>(ScaleToFrequency(
                scaledMinimum + (scaledMaximum - scaledMinimum) *
                    (band + 0.5) / plan.bars,
                settings.frequencyScale));
        }
    }

    for (int band = 0; band < plan.bars; ++band) {
        const float lower = plan.lowerFrequency[band];
        const float upper = plan.upperFrequency[band];
        if (settings.frequencyScale == FrequencyScale::HtkMel) {
            const float center = plan.centerFrequency[band];
            const float slaneyNormalization =
                settings.bandAggregation == BandAggregation::Slaney
                ? 2.0f * plan.binWidth / (upper - lower) : 1.0f;
            for (int bin = 1; bin < plan.fftBins; ++bin) {
                const float frequency = bin * plan.binWidth;
                const float lowerSlope = (frequency - lower) /
                                         (center - lower);
                const float upperSlope = (upper - frequency) /
                                         (upper - center);
                const float triangle = std::max(
                    0.0f, std::min(lowerSlope, upperSlope));
                if (triangle > 0.0f) {
                    plan.binWeights[band].emplace_back(
                        bin, triangle * slaneyNormalization);
                }
            }
            continue;
        }
        const float analysisLower =
            band == 0 && plan.foldBelowMinimum ? 0.0f : lower;
        for (int bin = 1; bin < plan.fftBins; ++bin) {
            const float binLower = std::max(0.0f, (bin - 0.5f) * plan.binWidth);
            const float binUpper = (bin + 0.5f) * plan.binWidth;
            const float overlap = std::min(upper, binUpper) -
                                  std::max(analysisLower, binLower);
            if (overlap > 0.0f) {
                plan.binWeights[band].emplace_back(
                    bin, overlap / plan.binWidth);
            }
        }
    }
    return plan;
}


void AnalyzeChannels(const std::vector<std::vector<float>>& channels,
                     size_t oldestSample, const AnalysisPlan& plan,
                     AnalysisScratch* scratch, BandLevels* levels) {
    if (!scratch || !levels || channels.empty()) return;
    scratch->spectrumPower.assign(plan.fftBins, 0.0);
    for (const auto& samples : channels) {
        AccumulateChannelPower(samples, oldestSample,
                               &scratch->spectrumPower,
                               &scratch->transformed, plan);
    }
    PublishBandPower(scratch->spectrumPower, channels.size(), plan, levels);
}

float SmoothDisplayLevel(float current, float target, float deltaSeconds,
                         int attackMs, int releaseMs) {
    current = std::clamp(current, 0.0f, 1.0f);
    target = std::clamp(target, 0.0f, 1.0f);
    deltaSeconds = std::clamp(deltaSeconds, 0.0f, 0.25f);
    const int smoothingMs = target > current ? attackMs : releaseMs;
    if (smoothingMs <= 0) return target;
    const float retention = std::exp(
        -deltaSeconds / (smoothingMs / 1000.0f));
    const float smoothed =
        current * retention + target * (1.0f - retention);
    return target <= 0.0f && smoothed < 0.001f ? 0.0f : smoothed;
}

int CalculateRenderedLevelHeight(float level, int maximumHeight,
                                 int minimumHeight) {
    if (level <= 0.0f || maximumHeight <= 0) return 0;
    level = std::clamp(level, 0.0f, 1.0f);
    minimumHeight = std::clamp(minimumHeight, 0, maximumHeight);
    return std::clamp(
        static_cast<int>(level * maximumHeight), minimumHeight,
        maximumHeight);
}

LevelPixelCoverage CalculateLevelPixelCoverage(float level,
                                               int maximumHeight) {
    LevelPixelCoverage coverage;
    if (level <= 0.0f || maximumHeight <= 0) return coverage;
    const float pixelHeight =
        std::clamp(level, 0.0f, 1.0f) * maximumHeight;
    coverage.fullPixels = std::clamp(
        static_cast<int>(std::floor(pixelHeight)), 0, maximumHeight);
    if (coverage.fullPixels < maximumHeight) {
        coverage.partialPixel = std::clamp(
            pixelHeight - coverage.fullPixels, 0.0f, 1.0f);
    }
    return coverage;
}

int CalculatePeakBlockBottom(int peakLevelHeight, int spectrumTop,
                             int spectrumBottom, int peakHeight, int peakGap,
                             bool keepVisibleAtZero) {
    if (spectrumBottom <= spectrumTop || peakHeight <= 0 ||
        peakHeight > spectrumBottom - spectrumTop) return -1;
    peakLevelHeight = std::max(0, peakLevelHeight);
    if (peakLevelHeight == 0 && !keepVisibleAtZero) return -1;
    const int offset = peakLevelHeight > 0
        ? peakLevelHeight + std::max(0, peakGap)
        : 0;
    return std::clamp(
        spectrumBottom - offset,
        spectrumTop + peakHeight, spectrumBottom);
}

void UpdatePeak(PeakState* peak, float level, float deltaSeconds,
                float holdSeconds, float gravity) {
    if (!peak) return;
    level = std::clamp(level, 0.0f, 1.0f);
    deltaSeconds = std::clamp(deltaSeconds, 0.0f, 0.25f);
    holdSeconds = std::max(0.0f, holdSeconds);
    gravity = std::max(0.0f, gravity);
    if (level >= peak->level) {
        peak->level = level;
        peak->velocity = 0.0f;
        peak->holdSeconds = level > 0.0f ? holdSeconds : 0.0f;
        return;
    }
    float fallingTime = deltaSeconds;
    if (peak->holdSeconds > 0.0f) {
        const float held = std::min(peak->holdSeconds, fallingTime);
        peak->holdSeconds -= held;
        fallingTime -= held;
    }
    if (fallingTime <= 0.0f) return;
    peak->velocity += gravity * fallingTime;
    peak->level -= peak->velocity * fallingTime;
    if (peak->level <= level) {
        peak->level = level;
        peak->velocity = 0.0f;
        peak->holdSeconds = 0.0f;
    }
}

}  // namespace tas




namespace tas {

EndpointNotificationClient::EndpointNotificationClient(HANDLE changedEvent)
    : changedEvent_(changedEvent) {}

ULONG STDMETHODCALLTYPE EndpointNotificationClient::AddRef() {
    return references_.fetch_add(1, std::memory_order_relaxed) + 1;
}

ULONG STDMETHODCALLTYPE EndpointNotificationClient::Release() {
    const ULONG remaining =
        references_.fetch_sub(1, std::memory_order_acq_rel) - 1;
    if (!remaining) delete this;
    return remaining;
}

HRESULT STDMETHODCALLTYPE EndpointNotificationClient::QueryInterface(
    REFIID iid, void** object) {
    if (!object) return E_POINTER;
    *object = nullptr;
    if (iid == IID_IUnknown || iid == __uuidof(IMMNotificationClient)) {
        *object = static_cast<IMMNotificationClient*>(this);
        AddRef();
        return S_OK;
    }
    return E_NOINTERFACE;
}

HRESULT STDMETHODCALLTYPE EndpointNotificationClient::OnDefaultDeviceChanged(
    EDataFlow flow, ERole role, LPCWSTR) {
    if (flow == eRender && role == eConsole) SetEvent(changedEvent_);
    return S_OK;
}

HRESULT STDMETHODCALLTYPE EndpointNotificationClient::OnDeviceAdded(LPCWSTR) {
    return S_OK;
}

HRESULT STDMETHODCALLTYPE EndpointNotificationClient::OnDeviceRemoved(LPCWSTR) {
    return S_OK;
}

HRESULT STDMETHODCALLTYPE EndpointNotificationClient::OnDeviceStateChanged(
    LPCWSTR, DWORD) {
    return S_OK;
}

HRESULT STDMETHODCALLTYPE EndpointNotificationClient::OnPropertyValueChanged(
    LPCWSTR, const PROPERTYKEY) {
    return S_OK;
}

DWORD WINAPI AudioThreadProc(void* parameter) {
    auto& context = *static_cast<ApplicationContext*>(parameter);
    const Settings& settings = context.settings;
    HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    if (FAILED(hr)) {
        Wh_Log(L"Audio COM initialization failed: 0x%08X", hr);
        return 0;
    }

    while (WaitForSingleObject(context.stopEvent.get(), 0) != WAIT_OBJECT_0) {
        while (!context.visualizerActive.load(std::memory_order_acquire)) {
            HANDLE inactiveWaits[] = {context.stopEvent.get(),
                                      context.activityChangedEvent.get()};
            const DWORD waitResult = WaitForMultipleObjects(
                ARRAYSIZE(inactiveWaits), inactiveWaits, FALSE, INFINITE);
            if (waitResult == WAIT_OBJECT_0) goto finished;
            if (waitResult != WAIT_OBJECT_0 + 1) {
                Wh_Log(L"Audio activity wait failed: %u", GetLastError());
                goto finished;
            }
        }

        IMMDeviceEnumerator* enumerator = nullptr;
        IMMDevice* device = nullptr;
        IAudioClient* audioClient = nullptr;
        IAudioCaptureClient* captureClient = nullptr;
        WAVEFORMATEX* format = nullptr;
        HANDLE audioEvent = nullptr;
        HANDLE deviceChangedEvent = nullptr;
        EndpointNotificationClient* notification = nullptr;
        bool notificationRegistered = false;
        bool audioStarted = false;
        AnalysisPlan plan{};

        hr = S_OK;
        do {
            deviceChangedEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
            audioEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
            if (!deviceChangedEvent || !audioEvent) {
                hr = HRESULT_FROM_WIN32(GetLastError());
                break;
            }

            hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr,
                                  CLSCTX_ALL, __uuidof(IMMDeviceEnumerator),
                                  reinterpret_cast<void**>(&enumerator));
            if (FAILED(hr)) break;

            notification = new (std::nothrow)
                EndpointNotificationClient(deviceChangedEvent);
            if (notification) {
                const HRESULT notificationResult =
                    enumerator->RegisterEndpointNotificationCallback(notification);
                if (SUCCEEDED(notificationResult)) {
                    notificationRegistered = true;
                } else {
                    Wh_Log(L"Audio endpoint notification unavailable: 0x%08X",
                        notificationResult);
                }
            }

            hr = enumerator->GetDefaultAudioEndpoint(eRender, eConsole, &device);
            if (FAILED(hr)) break;
            hr = device->Activate(__uuidof(IAudioClient), CLSCTX_ALL, nullptr,
                                  reinterpret_cast<void**>(&audioClient));
            if (FAILED(hr)) break;
            hr = audioClient->GetMixFormat(&format);
            if (FAILED(hr)) break;
            hr = audioClient->Initialize(
                AUDCLNT_SHAREMODE_SHARED,
                AUDCLNT_STREAMFLAGS_LOOPBACK | AUDCLNT_STREAMFLAGS_EVENTCALLBACK,
                0, 0, format, nullptr);
            if (FAILED(hr)) break;
            hr = audioClient->SetEventHandle(audioEvent);
            if (FAILED(hr)) break;
            hr = audioClient->GetService(
                __uuidof(IAudioCaptureClient),
                reinterpret_cast<void**>(&captureClient));
            if (FAILED(hr)) break;
            hr = audioClient->Start();
            if (FAILED(hr)) break;
            audioStarted = true;

            plan = MakeAnalysisPlan(format->nSamplesPerSec, settings);
            const size_t fftSamples = static_cast<size_t>(plan.fftSize);
            const size_t hopSamples = static_cast<size_t>(plan.hopSamples);
            BandLevels levels{};
            AnalysisScratch analysisScratch;
            std::vector<BYTE> packetData;
            ClearPublishedBands(context);
            Wh_Log(L"WASAPI loopback started: %u Hz, %u channels, %u bit; "
                L"fft=%d overlap=%d%% hop=%d window=%ls",
                format->nSamplesPerSec, format->nChannels,
                format->wBitsPerSample, plan.fftSize,
                settings.fftOverlapPercent, plan.hopSamples,
                WindowFunctionName(settings.windowFunction));

            const int analysisChannels =
                std::max(1, static_cast<int>(format->nChannels));
            std::vector<std::vector<float>> samples(
                analysisChannels, std::vector<float>(fftSamples));
            size_t writeIndex = 0;
            size_t bufferedFrames = 0;
            size_t framesSinceAnalysis = 0;
            SignalWindowTracker signalWindow(fftSamples);
            const DWORD idleTimeoutMs = std::max<DWORD>(
                100, static_cast<DWORD>(
                    (static_cast<uint64_t>(plan.hopSamples) * 2000 +
                     format->nSamplesPerSec - 1) /
                    format->nSamplesPerSec));
            bool streamIdle = false;
            bool signalSeen = false;
            bool silencePublished = false;
            ULONGLONG lastPacketTick = GetTickCount64();
            const auto enterStreamIdle = [&] {
                if (streamIdle) return;
                for (auto& channel : samples) {
                    std::fill(channel.begin(), channel.end(), 0.0f);
                }
                writeIndex = 0;
                bufferedFrames = fftSamples;
                framesSinceAnalysis = 0;
                signalWindow.ResetToSilence();
                ClearBands(&levels);
                ClearPublishedBands(context);
                signalSeen = false;
                silencePublished = true;
                streamIdle = true;
                Wh_Log(L"Audio stream idle for %u ms; spectrum cleared",
                    idleTimeoutMs);
            };
            HANDLE waits[] = {context.stopEvent.get(),
                              context.activityChangedEvent.get(),
                              deviceChangedEvent, audioEvent};
            while (true) {
                const DWORD waitResult = WaitForMultipleObjects(
                    ARRAYSIZE(waits), waits, FALSE, idleTimeoutMs);
                if (waitResult == WAIT_OBJECT_0) {
                    hr = S_OK;
                    break;
                }
                if (waitResult == WAIT_OBJECT_0 + 1) {
                    if (!context.visualizerActive.load(
                            std::memory_order_acquire)) {
                        hr = S_FALSE;
                        break;
                    }
                    continue;
                }
                if (waitResult == WAIT_OBJECT_0 + 2) {
                    hr = AUDCLNT_E_DEVICE_INVALIDATED;
                    Wh_Log(L"Default playback device changed; reconnecting");
                    break;
                }
                if (waitResult == WAIT_TIMEOUT) {
                    enterStreamIdle();
                    continue;
                }
                if (waitResult != WAIT_OBJECT_0 + 3) {
                    hr = HRESULT_FROM_WIN32(GetLastError());
                    break;
                }

                UINT32 packetFrames = 0;
                bool receivedFrames = false;
                while (true) {
                    hr = captureClient->GetNextPacketSize(&packetFrames);
                    if (FAILED(hr) || packetFrames == 0) break;
                    BYTE* data = nullptr;
                    DWORD flags = 0;
                    UINT32 frames = 0;
                    hr = captureClient->GetBuffer(
                        &data, &frames, &flags, nullptr, nullptr);
                    if (FAILED(hr)) break;
                    if (frames > 0) receivedFrames = true;

                    const bool silent =
                        (flags & AUDCLNT_BUFFERFLAGS_SILENT) != 0;
                    if (!silent && frames > 0) {
                        const size_t packetBytes =
                            static_cast<size_t>(frames) * format->nBlockAlign;
                        packetData.resize(packetBytes);
                        memcpy(packetData.data(), data, packetBytes);
                    }
                    const HRESULT releaseResult =
                        captureClient->ReleaseBuffer(frames);
                    if (FAILED(releaseResult)) {
                        hr = releaseResult;
                        break;
                    }

                    if (flags & AUDCLNT_BUFFERFLAGS_DATA_DISCONTINUITY) {
                        writeIndex = 0;
                        bufferedFrames = 0;
                        framesSinceAnalysis = 0;
                        signalWindow.ResetToSilence();
                    }
                    const BYTE* copiedData = silent ? nullptr
                                                    : packetData.data();
                    for (UINT32 frame = 0; frame < frames; ++frame) {
                        bool frameContainsSignal = false;
                        for (int channel = 0; channel < analysisChannels;
                             ++channel) {
                            const float sample = silent
                                ? 0.0f
                                : ReadSample(copiedData, frame, channel,
                                             format);
                            samples[channel][writeIndex] = sample;
                            frameContainsSignal =
                                frameContainsSignal || sample != 0.0f;
                        }
                        const bool signalResumed = frameContainsSignal &&
                            !signalWindow.ContainsSignal();
                        signalWindow.PushFrame(frameContainsSignal);
                        writeIndex = (writeIndex + 1) % fftSamples;
                        bufferedFrames = std::min(
                            bufferedFrames + 1, fftSamples);
                        ++framesSinceAnalysis;
                        if (bufferedFrames == fftSamples &&
                            !signalWindow.ContainsSignal()) {
                            if (!silencePublished) {
                                ClearBands(&levels);
                                PublishBandLevels(context, levels);
                                signalSeen = false;
                                silencePublished = true;
                            }
                            framesSinceAnalysis = 0;
                            continue;
                        }
                        if (signalResumed) {
                            framesSinceAnalysis = std::max(
                                framesSinceAnalysis, hopSamples);
                            silencePublished = false;
                        }
                        if (bufferedFrames == fftSamples &&
                            framesSinceAnalysis >= hopSamples) {
                            AnalyzeChannels(samples, writeIndex, plan,
                                            &analysisScratch, &levels);
                            PublishBandLevels(context, levels);
                            if (plan.bars > 0) {
                                const float strongest = *std::max_element(
                                    levels.begin(),
                                    levels.begin() + plan.bars);
                                if (strongest > 0.03f && !signalSeen) {
                                    Wh_Log(L"Audio signal detected, peak band level %.3f",
                                        strongest);
                                    signalSeen = true;
                                }
                            }
                            framesSinceAnalysis = 0;
                        }
                    }
                }
                if (FAILED(hr)) break;
                if (receivedFrames) {
                    lastPacketTick = GetTickCount64();
                    if (streamIdle) {
                        streamIdle = false;
                        Wh_Log(L"Audio stream resumed");
                    }
                } else if (GetTickCount64() - lastPacketTick >=
                           idleTimeoutMs) {
                    enterStreamIdle();
                }
            }
        } while (false);

        ClearPublishedBands(context);
        if (audioStarted) audioClient->Stop();
        if (notificationRegistered) {
            enumerator->UnregisterEndpointNotificationCallback(notification);
        }
        if (notification) notification->Release();
        if (audioEvent) CloseHandle(audioEvent);
        if (deviceChangedEvent) CloseHandle(deviceChangedEvent);
        if (format) CoTaskMemFree(format);
        SafeRelease(captureClient);
        SafeRelease(audioClient);
        SafeRelease(device);
        SafeRelease(enumerator);

        if (WaitForSingleObject(context.stopEvent.get(), 0) == WAIT_OBJECT_0) {
            break;
        }
        if (!context.visualizerActive.load(std::memory_order_acquire)) continue;
        Wh_Log(L"WASAPI session ended (0x%08X); reconnecting", hr);
        HANDLE retryWaits[] = {context.stopEvent.get(),
                               context.activityChangedEvent.get()};
        if (WaitForMultipleObjects(ARRAYSIZE(retryWaits), retryWaits, FALSE,
                                   1000) == WAIT_OBJECT_0) {
            break;
        }
    }

finished:
    ClearPublishedBands(context);
    CoUninitialize();
    return 0;
}

}  // namespace tas





namespace tas {

namespace {

constexpr DWORD kNormalLayoutRefreshMs = 30000;
constexpr DWORD kRapidLayoutRefreshMs = 100;
constexpr ULONGLONG kRapidLayoutRefreshDurationMs = 2500;

struct LocatorTarget {
    ApplicationContext* context = nullptr;
    HWND taskbar = nullptr;
    DWORD explorerProcessId = 0;
};

class LayoutPropertyChangedHandler final
    : public IUIAutomationPropertyChangedEventHandler {
public:
    LayoutPropertyChangedHandler()
        : changedEvent_(CreateEventW(nullptr, FALSE, FALSE, nullptr)) {}

    ~LayoutPropertyChangedHandler() {
        if (changedEvent_) CloseHandle(changedEvent_);
    }

    HANDLE changedEvent() const { return changedEvent_; }

    ULONG STDMETHODCALLTYPE AddRef() override {
        return references_.fetch_add(1, std::memory_order_relaxed) + 1;
    }

    ULONG STDMETHODCALLTYPE Release() override {
        const ULONG remaining =
            references_.fetch_sub(1, std::memory_order_acq_rel) - 1;
        if (!remaining) delete this;
        return remaining;
    }

    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid,
                                             void** object) override {
        if (!object) return E_POINTER;
        *object = nullptr;
        if (iid == IID_IUnknown ||
            iid == __uuidof(IUIAutomationPropertyChangedEventHandler)) {
            *object = static_cast<IUIAutomationPropertyChangedEventHandler*>(
                this);
            AddRef();
            return S_OK;
        }
        return E_NOINTERFACE;
    }

    HRESULT STDMETHODCALLTYPE HandlePropertyChangedEvent(
        IUIAutomationElement*, PROPERTYID propertyId, VARIANT) override {
        if (propertyId == UIA_BoundingRectanglePropertyId && changedEvent_) {
            SetEvent(changedEvent_);
        }
        return S_OK;
    }

private:
    std::atomic<ULONG> references_{1};
    HANDLE changedEvent_ = nullptr;
};

HRESULT FindDescendantByAutomationId(IUIAutomation* automation,
                                     IUIAutomationElement* root,
                                     PCWSTR automationId,
                                     IUIAutomationElement** result) {
    if (!automation || !root || !result) return E_INVALIDARG;
    *result = nullptr;
    IUIAutomationCondition* condition = nullptr;
    VARIANT value;
    VariantInit(&value);
    value.vt = VT_BSTR;
    value.bstrVal = SysAllocString(automationId);
    if (!value.bstrVal) return E_OUTOFMEMORY;
    HRESULT hr = automation->CreatePropertyCondition(
        UIA_AutomationIdPropertyId, value, &condition);
    if (SUCCEEDED(hr)) {
        hr = root->FindFirst(TreeScope_Descendants, condition, result);
    }
    VariantClear(&value);
    SafeRelease(condition);
    return hr;
}

bool QuerySearchLayout(IUIAutomation* automation, HWND taskbar,
                       IUIAutomationElement** cachedSearch,
                       SearchLayout* result) {
    if (!automation || !taskbar || !cachedSearch || !result) return false;
    IUIAutomationElement* root = nullptr;
    SearchLayout layout;
    layout.taskbar = taskbar;
    GetWindowThreadProcessId(taskbar, &layout.explorerProcessId);
    layout.dpi = GetWindowDpiOrDefault(taskbar);

    HRESULT hr = E_FAIL;
    if (*cachedSearch) {
        hr = (*cachedSearch)->get_CurrentBoundingRectangle(&layout.rect);
        if (FAILED(hr) || IsRectEmpty(&layout.rect)) {
            SafeRelease(*cachedSearch);
        }
    }
    if (!*cachedSearch) {
        hr = automation->ElementFromHandle(taskbar, &root);
        if (SUCCEEDED(hr)) {
            hr = FindDescendantByAutomationId(
                automation, root, L"SearchButton", cachedSearch);
        }
        if (SUCCEEDED(hr) && !*cachedSearch) {
            hr = HRESULT_FROM_WIN32(ERROR_NOT_FOUND);
        }
        if (SUCCEEDED(hr)) {
            hr = (*cachedSearch)->get_CurrentBoundingRectangle(&layout.rect);
        }
    }
    if (SUCCEEDED(hr) &&
        (layout.rect.right <= layout.rect.left ||
         layout.rect.bottom <= layout.rect.top)) {
        hr = E_FAIL;
    }

    SafeRelease(root);
    if (FAILED(hr)) return false;
    layout.valid = true;
    *result = layout;
    return true;
}

bool LayoutContentEquals(const SearchLayout& first,
                         const SearchLayout& second) {
    return first.valid == second.valid && first.taskbar == second.taskbar &&
           first.explorerProcessId == second.explorerProcessId &&
           RectEquals(first.rect, second.rect) &&
           first.dpi == second.dpi;
}

void PublishLayout(ApplicationContext& context, SearchLayout layout) {
    auto& state = context.searchLayout;
    bool changed = false;
    {
        std::lock_guard<std::mutex> lock(state.mutex);
        changed = !LayoutContentEquals(state.latest, layout);
        if (changed) ++state.generation;
        layout.generation = state.generation;
        state.latest = layout;
    }
    if (changed && context.layoutChangedEvent) {
        SetEvent(context.layoutChangedEvent.get());
    }
}

void RememberLayout(ApplicationContext& context, const SearchLayout& layout) {
    RECT taskbarRect{};
    if (!layout.valid || !layout.taskbar ||
        !GetWindowRect(layout.taskbar, &taskbarRect) ||
        IsRectEmpty(&taskbarRect)) {
        return;
    }
    auto& state = context.searchLayout;
    std::lock_guard<std::mutex> lock(state.mutex);
    state.templateTaskbarRect = taskbarRect;
    state.templateSearchRect = layout.rect;
    state.templateDpi = layout.dpi ? layout.dpi : 96;
    state.templateValid = true;
}

bool BuildLayoutFromTemplate(ApplicationContext& context, HWND taskbar,
                              DWORD explorerProcessId,
                              SearchLayout* result) {
    if (!taskbar || !explorerProcessId || !result ||
        !IsWindow(taskbar) || !IsWindowVisible(taskbar)) {
        return false;
    }
    RECT taskbarRect{};
    if (!GetWindowRect(taskbar, &taskbarRect) || IsRectEmpty(&taskbarRect)) {
        return false;
    }
    RECT templateTaskbarRect{};
    RECT templateSearchRect{};
    UINT templateDpi = 96;
    {
        auto& state = context.searchLayout;
        std::lock_guard<std::mutex> lock(state.mutex);
        if (!state.templateValid) return false;
        templateTaskbarRect = state.templateTaskbarRect;
        templateSearchRect = state.templateSearchRect;
        templateDpi = state.templateDpi;
    }

    const UINT dpi = GetWindowDpiOrDefault(taskbar);
    const auto scale = [&](LONG value) {
        return MulDiv(value, static_cast<int>(dpi),
                      static_cast<int>(templateDpi));
    };
    const LONG relativeLeft =
        templateSearchRect.left - templateTaskbarRect.left;
    const LONG relativeTop =
        templateSearchRect.top - templateTaskbarRect.top;
    const LONG width = templateSearchRect.right - templateSearchRect.left;
    const LONG height = templateSearchRect.bottom - templateSearchRect.top;
    SearchLayout layout;
    layout.taskbar = taskbar;
    layout.explorerProcessId = explorerProcessId;
    layout.dpi = dpi;
    layout.rect.left = taskbarRect.left + scale(relativeLeft);
    layout.rect.top = taskbarRect.top + scale(relativeTop);
    layout.rect.right = layout.rect.left + scale(width);
    layout.rect.bottom = layout.rect.top + scale(height);
    if (layout.rect.left < taskbarRect.left ||
        layout.rect.top < taskbarRect.top ||
        layout.rect.right > taskbarRect.right ||
        layout.rect.bottom > taskbarRect.bottom ||
        IsRectEmpty(&layout.rect)) {
        return false;
    }
    layout.valid = true;
    *result = layout;
    return true;
}

HRESULT CreateAutomationClient(IUIAutomation2** automation) {
    if (!automation) return E_POINTER;
    *automation = nullptr;
    HRESULT hr = CoCreateInstance(
        CLSID_CUIAutomation8, nullptr, CLSCTX_INPROC_SERVER,
        IID_PPV_ARGS(automation));
    if (SUCCEEDED(hr)) {
        hr = (*automation)->put_ConnectionTimeout(kUiAutomationTimeoutMs);
    }
    if (SUCCEEDED(hr)) {
        hr = (*automation)->put_TransactionTimeout(kUiAutomationTimeoutMs);
    }
    if (FAILED(hr)) SafeRelease(*automation);
    return hr;
}

DWORD WINAPI SearchLocatorWorkerProc(void* parameter) {
    const std::unique_ptr<LocatorTarget> targetParameter(
        static_cast<LocatorTarget*>(parameter));
    const LocatorTarget target = *targetParameter;
    ApplicationContext& context = *target.context;
    EnablePerMonitorDpiAwarenessForThread();
    const HRESULT initializeResult =
        CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    if (FAILED(initializeResult)) {
        Wh_Log(L"Locator COM initialization failed: 0x%08X", initializeResult);
        return 0;
    }

    const bool cancellationEnabled =
        SUCCEEDED(CoEnableCallCancellation(nullptr));
    IUIAutomation2* automation = nullptr;
    const HRESULT createResult = CreateAutomationClient(&automation);
    if (FAILED(createResult)) {
        Wh_Log(L"UI Automation client creation failed: 0x%08X", createResult);
        if (cancellationEnabled) {
            const HRESULT disableResult = CoDisableCallCancellation(nullptr);
            if (FAILED(disableResult)) {
                Wh_Log(L"Failed to disable COM call cancellation: 0x%08X",
                    disableResult);
            }
        }
        CoUninitialize();
        return 0;
    }
    Wh_Log(L"UI Automation worker started: explorer=%lu timeout=%u ms",
        target.explorerProcessId, kUiAutomationTimeoutMs);

    auto* propertyHandler = new (std::nothrow) LayoutPropertyChangedHandler;
    bool propertyHandlerRegistered = false;

    RegistryChangeWatcher taskbarSettingsWatcher;
    if (context.settings.autoPosition && taskbarSettingsWatcher.Start(
            kExplorerAdvancedRegistryPath)) {
        Wh_Log(L"Taskbar settings change watcher started");
    }
    RegistryChangeWatcher searchModeWatcher;
    if (searchModeWatcher.Start(kSearchSettingsRegistryPath)) {
        Wh_Log(L"Search mode change watcher started");
    }
    ScopedHandle explorerProcess(OpenProcess(
        SYNCHRONIZE, FALSE, target.explorerProcessId));
    DWORD searchMode = GetSearchMode();
    ULONGLONG rapidRefreshUntil = 0;
    SearchLayout previousLayout;
    bool hasPreviousLayout = false;
    IUIAutomationElement* cachedSearch = nullptr;
    int consecutiveFailures = 0;
    bool fallbackLogged = false;
    while (WaitForSingleObject(context.stopEvent.get(), 0) != WAIT_OBJECT_0) {
        const bool searchBoxMode = IsSearchBoxMode(searchMode);
        if (!searchBoxMode) {
            PublishLayout(context, {});
            consecutiveFailures = 0;
        } else {
            const HWND taskbar = FindWindowW(L"Shell_TrayWnd", nullptr);
            DWORD explorerProcessId = 0;
            if (taskbar) {
                GetWindowThreadProcessId(taskbar, &explorerProcessId);
            }
            if (taskbar != target.taskbar ||
                explorerProcessId != target.explorerProcessId) {
                Wh_Log(L"UI Automation worker retiring after shell change: old=%lu new=%lu",
                    target.explorerProcessId, explorerProcessId);
                break;
            }

            SearchLayout layout;
            IUIAutomationElement* previousSearch = cachedSearch;
            if (previousSearch) previousSearch->AddRef();
            const bool queried = QuerySearchLayout(
                automation, taskbar, &cachedSearch, &layout);
            if (previousSearch != cachedSearch) {
                if (propertyHandlerRegistered && previousSearch) {
                    automation->RemovePropertyChangedEventHandler(
                        previousSearch, propertyHandler);
                    propertyHandlerRegistered = false;
                }
                if (cachedSearch && propertyHandler &&
                    propertyHandler->changedEvent()) {
                    PROPERTYID properties[] = {
                        UIA_BoundingRectanglePropertyId};
                    propertyHandlerRegistered = SUCCEEDED(
                        automation->AddPropertyChangedEventHandlerNativeArray(
                            cachedSearch, TreeScope_Element, nullptr,
                            propertyHandler, properties,
                            static_cast<int>(ARRAYSIZE(properties))));
                }
            }
            SafeRelease(previousSearch);
            if (queried) {
                const bool layoutChanged = hasPreviousLayout &&
                    !LayoutContentEquals(previousLayout, layout);
                previousLayout = layout;
                hasPreviousLayout = true;
                if (context.settings.autoPosition && layoutChanged) {
                    rapidRefreshUntil = std::max(
                        rapidRefreshUntil,
                        GetTickCount64() + kRapidLayoutRefreshDurationMs);
                }
                consecutiveFailures = 0;
                fallbackLogged = false;
                RememberLayout(context, layout);
                PublishLayout(context, layout);
            } else if (BuildLayoutFromTemplate(
                           context, taskbar, explorerProcessId, &layout)) {
                consecutiveFailures = 0;
                PublishLayout(context, layout);
                if (!fallbackLogged) {
                    Wh_Log(L"Using cached search-box geometry for explorer=%lu",
                        explorerProcessId);
                    fallbackLogged = true;
                }
            } else if (++consecutiveFailures >= 3) {
                Wh_Log(L"UI Automation worker retiring after repeated query failures");
                break;
            }
        }

        const bool rapidRefresh = searchBoxMode &&
            context.settings.autoPosition &&
            GetTickCount64() < rapidRefreshUntil;
        const DWORD refreshDelay = !searchBoxMode && searchModeWatcher.changedEvent()
            ? INFINITE
            : (rapidRefresh ? kRapidLayoutRefreshMs
                            : kNormalLayoutRefreshMs);
        HANDLE waits[5] = {context.stopEvent.get()};
        DWORD waitCount = 1;
        DWORD explorerWaitIndex = MAXDWORD;
        DWORD taskbarSettingsWaitIndex = MAXDWORD;
        DWORD searchModeWaitIndex = MAXDWORD;
        DWORD propertyWaitIndex = MAXDWORD;
        if (explorerProcess) {
            explorerWaitIndex = waitCount;
            waits[waitCount++] = explorerProcess.get();
        }
        if (taskbarSettingsWatcher.changedEvent()) {
            taskbarSettingsWaitIndex = waitCount;
            waits[waitCount++] = taskbarSettingsWatcher.changedEvent();
        }
        if (searchModeWatcher.changedEvent()) {
            searchModeWaitIndex = waitCount;
            waits[waitCount++] = searchModeWatcher.changedEvent();
        }
        if (propertyHandler && propertyHandler->changedEvent()) {
            propertyWaitIndex = waitCount;
            waits[waitCount++] = propertyHandler->changedEvent();
        }
        const DWORD waitResult = WaitForMultipleObjects(
            waitCount, waits, FALSE, refreshDelay);
        if (waitResult == WAIT_OBJECT_0) break;
        if (explorerWaitIndex != MAXDWORD &&
            waitResult == WAIT_OBJECT_0 + explorerWaitIndex) {
            Wh_Log(L"UI Automation worker retiring after Explorer exited");
            break;
        }
        if (taskbarSettingsWaitIndex != MAXDWORD &&
            waitResult == WAIT_OBJECT_0 + taskbarSettingsWaitIndex) {
            rapidRefreshUntil =
                GetTickCount64() + kRapidLayoutRefreshDurationMs;
            if (!taskbarSettingsWatcher.Arm()) {
                Wh_Log(L"Taskbar settings change watcher could not be rearmed");
                taskbarSettingsWatcher.Stop();
            } else {
                Wh_Log(L"Taskbar settings changed; refreshing search-box geometry");
            }
        } else if (searchModeWaitIndex != MAXDWORD &&
                   waitResult == WAIT_OBJECT_0 + searchModeWaitIndex) {
            if (!searchModeWatcher.Arm()) {
                Wh_Log(L"Search mode change watcher could not be rearmed");
                searchModeWatcher.Stop();
            }
            searchMode = GetSearchMode();
            rapidRefreshUntil =
                GetTickCount64() + kRapidLayoutRefreshDurationMs;
            Wh_Log(L"Search mode changed: %lu", searchMode);
        } else if (propertyWaitIndex != MAXDWORD &&
                   waitResult == WAIT_OBJECT_0 + propertyWaitIndex) {
            rapidRefreshUntil =
                GetTickCount64() + kRapidLayoutRefreshDurationMs;
        } else if (waitResult == WAIT_TIMEOUT) {
            if (!searchModeWatcher.changedEvent()) {
                searchMode = GetSearchMode();
            }
        } else {
            Wh_Log(L"UI Automation refresh wait failed: %u", GetLastError());
            break;
        }
    }

    PublishLayout(context, {});
    if (propertyHandlerRegistered && cachedSearch) {
        automation->RemovePropertyChangedEventHandler(
            cachedSearch, propertyHandler);
    }
    SafeRelease(cachedSearch);
    if (propertyHandler) propertyHandler->Release();
    SafeRelease(automation);
    if (cancellationEnabled) {
        const HRESULT disableResult = CoDisableCallCancellation(nullptr);
        if (FAILED(disableResult)) {
            Wh_Log(L"Failed to disable COM call cancellation: 0x%08X",
                disableResult);
        }
    }
    CoUninitialize();
    Wh_Log(L"UI Automation worker stopped: explorer=%lu",
        target.explorerProcessId);
    return 0;
}

}  // namespace

SearchHostKind DetectSearchHostKind(PCWSTR executablePath) {
    if (!executablePath || !*executablePath) return SearchHostKind::Unknown;
    const PCWSTR slash = wcsrchr(executablePath, L'\\');
    const PCWSTR forwardSlash = wcsrchr(executablePath, L'/');
    PCWSTR name = executablePath;
    if (slash && slash + 1 > name) name = slash + 1;
    if (forwardSlash && forwardSlash + 1 > name) name = forwardSlash + 1;
    if (_wcsicmp(name, L"SearchHost.exe") == 0) {
        return SearchHostKind::Windows11;
    }
    if (_wcsicmp(name, L"SearchApp.exe") == 0 ||
        _wcsicmp(name, L"SearchUI.exe") == 0) {
        return SearchHostKind::Windows10;
    }
    return SearchHostKind::Unknown;
}

namespace {

SearchHostKind DetectOperatingSystemSearchHostKind() {
    using RtlGetVersionFunction = LONG(WINAPI*)(OSVERSIONINFOW*);
    static const SearchHostKind hostKind = [] {
        const HMODULE ntdll = GetModuleHandleW(L"ntdll.dll");
        const auto rtlGetVersion = ntdll
            ? reinterpret_cast<RtlGetVersionFunction>(
                  GetProcAddress(ntdll, "RtlGetVersion"))
            : nullptr;
        OSVERSIONINFOW version{};
        version.dwOSVersionInfoSize = sizeof(version);
        if (!rtlGetVersion || rtlGetVersion(&version) < 0) {
            return SearchHostKind::Unknown;
        }
        if (version.dwMajorVersion > 10 ||
            (version.dwMajorVersion == 10 && version.dwBuildNumber >= 22000)) {
            return SearchHostKind::Windows11;
        }
        return version.dwMajorVersion == 10
            ? SearchHostKind::Windows10
            : SearchHostKind::Unknown;
    }();
    return hostKind;
}

}  // namespace

bool IsSearchExecutableName(PCWSTR executablePath) {
    return DetectSearchHostKind(executablePath) != SearchHostKind::Unknown;
}

bool IsSearchProcessWindow(HWND window) {
    if (!window) return false;
    window = GetAncestor(window, GA_ROOT);
    DWORD processId = 0;
    GetWindowThreadProcessId(window, &processId);
    if (!processId) return false;
    ScopedHandle process(OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE,
                                     processId));
    if (!process) return false;
    std::array<wchar_t, MAX_PATH> executablePath{};
    DWORD pathLength = static_cast<DWORD>(executablePath.size());
    if (QueryFullProcessImageNameW(
            process.get(), 0, executablePath.data(), &pathLength)) {
        return IsSearchExecutableName(executablePath.data());
    }
    if (GetLastError() != ERROR_INSUFFICIENT_BUFFER) return false;

    std::vector<wchar_t> longExecutablePath(32768);
    pathLength = static_cast<DWORD>(longExecutablePath.size());
    return QueryFullProcessImageNameW(
               process.get(), 0, longExecutablePath.data(), &pathLength) &&
           IsSearchExecutableName(longExecutablePath.data());
}

bool IsSearchInterfaceOpen() {
    HWND foreground = GetForegroundWindow();
    if (!foreground) return false;
    foreground = GetAncestor(foreground, GA_ROOT);
    if (!foreground || !IsWindowVisible(foreground) || IsIconic(foreground)) {
        return false;
    }

    DWORD cloaked = 0;
    if (SUCCEEDED(DwmGetWindowAttribute(
            foreground, DWMWA_CLOAKED, &cloaked, sizeof(cloaked))) &&
        cloaked) {
        return false;
    }

    RECT rectangle{};
    if (!GetWindowRect(foreground, &rectangle) || IsRectEmpty(&rectangle)) {
        return false;
    }
    return IsSearchProcessWindow(foreground);
}

DWORD GetSearchMode() {
    DWORD mode = 0;
    DWORD size = sizeof(mode);
    const LSTATUS status = RegGetValueW(
        HKEY_CURRENT_USER,
        L"Software\\Microsoft\\Windows\\CurrentVersion\\Search",
        L"SearchboxTaskbarMode", RRF_RT_REG_DWORD, nullptr, &mode, &size);
    return status == ERROR_SUCCESS ? mode : 0;
}

bool IsSearchBoxMode(DWORD mode, SearchHostKind hostKind) {
    if (hostKind == SearchHostKind::Windows10) return mode == 2;
    if (hostKind == SearchHostKind::Windows11) return mode == 3;
    // Mode 3 only represents a full search box on current Windows versions.
    return mode == 3;
}

bool IsSearchBoxMode(DWORD mode) {
    return IsSearchBoxMode(mode, DetectOperatingSystemSearchHostKind());
}

int ScaleForDpi(int value, UINT dpi) {
    return MulDiv(value, static_cast<int>(dpi ? dpi : 96), 96);
}

RECT CalculateSpectrumBounds(const SearchLayout& layout,
                             const Settings& settings) {
    const int width = layout.rect.right - layout.rect.left;
    const int height = layout.rect.bottom - layout.rect.top;
    const int clampedWidth = std::max(0, width);
    const int clampedHeight = std::max(0, height);
    const int left = std::clamp(
        ScaleForDpi(settings.leftPadding, layout.dpi), 0,
        clampedWidth);
    const int right = std::clamp(
        width - ScaleForDpi(settings.rightPadding, layout.dpi),
        left, clampedWidth);
    const int top = std::clamp(
        ScaleForDpi(settings.topPadding, layout.dpi), 0,
        clampedHeight);
    const int bottom = std::clamp(
        height - ScaleForDpi(settings.bottomPadding, layout.dpi),
        top, clampedHeight);
    return {left, top, right, bottom};
}

bool RectEquals(const RECT& first, const RECT& second) {
    return first.left == second.left && first.top == second.top &&
           first.right == second.right && first.bottom == second.bottom;
}

bool ReadLatestSearchLayout(ApplicationContext& context,
                            SearchLayout* layout) {
    if (!layout) return false;
    auto& state = context.searchLayout;
    std::lock_guard<std::mutex> lock(state.mutex);
    *layout = state.latest;
    return layout->valid;
}

bool IsFullscreenForeground(const SearchLayout& layout) {
    HWND foreground = GetForegroundWindow();
    if (!foreground || IsIconic(foreground)) return false;
    foreground = GetAncestor(foreground, GA_ROOT);
    DWORD processId = 0;
    GetWindowThreadProcessId(foreground, &processId);
    if (!processId || processId == layout.explorerProcessId) return false;

    RECT foregroundRect{};
    if (!GetWindowRect(foreground, &foregroundRect)) return false;
    MONITORINFO monitorInfo{};
    monitorInfo.cbSize = sizeof(monitorInfo);
    const HMONITOR monitor = MonitorFromRect(
        &layout.rect, MONITOR_DEFAULTTONEAREST);
    if (!GetMonitorInfoW(monitor, &monitorInfo)) return false;
    constexpr LONG tolerance = 2;
    return foregroundRect.left <= monitorInfo.rcMonitor.left + tolerance &&
           foregroundRect.top <= monitorInfo.rcMonitor.top + tolerance &&
           foregroundRect.right >= monitorInfo.rcMonitor.right - tolerance &&
           foregroundRect.bottom >= monitorInfo.rcMonitor.bottom - tolerance;
}

bool ShouldShowOverlay(const SearchLayout& layout, DWORD searchMode,
                       bool searchInterfaceOpen,
                       bool fullscreenForeground) {
    if (!layout.valid || !layout.taskbar || !IsWindow(layout.taskbar) ||
        !IsWindowVisible(layout.taskbar) ||
        !IsSearchBoxMode(searchMode) || IsRectEmpty(&layout.rect) ||
        searchInterfaceOpen || fullscreenForeground) {
        return false;
    }
    DWORD currentProcessId = 0;
    GetWindowThreadProcessId(layout.taskbar, &currentProcessId);
    return currentProcessId != 0 &&
           currentProcessId == layout.explorerProcessId;
}

DWORD WINAPI SearchLocatorThreadProc(void* parameter) {
    auto& context = *static_cast<ApplicationContext*>(parameter);
    const HRESULT initializeResult =
        CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    if (FAILED(initializeResult)) {
        Wh_Log(L"Locator supervisor COM initialization failed: 0x%08X",
            initializeResult);
        PublishLayout(context, {});
        return 0;
    }
    Wh_Log(L"UI Automation locator supervisor started");
    while (WaitForSingleObject(context.stopEvent.get(), 0) != WAIT_OBJECT_0) {
        const HWND taskbar = FindWindowW(L"Shell_TrayWnd", nullptr);
        if (!taskbar) {
            PublishLayout(context, {});
            if (WaitForSingleObject(context.stopEvent.get(), 500) ==
                WAIT_OBJECT_0) break;
            continue;
        }
        DWORD explorerProcessId = 0;
        GetWindowThreadProcessId(taskbar, &explorerProcessId);
        if (!explorerProcessId) {
            if (WaitForSingleObject(context.stopEvent.get(), 500) ==
                WAIT_OBJECT_0) break;
            continue;
        }

        auto target =
            std::unique_ptr<LocatorTarget>(new (std::nothrow) LocatorTarget);
        if (!target) {
            Wh_Log(L"Failed to allocate UI Automation worker target");
            break;
        }
        target->context = &context;
        target->taskbar = taskbar;
        target->explorerProcessId = explorerProcessId;
        DWORD workerThreadId = 0;
        HANDLE worker = CreateThread(
            nullptr, 0, SearchLocatorWorkerProc, target.get(), 0,
            &workerThreadId);
        if (!worker) {
            Wh_Log(L"Failed to create UI Automation worker: %u", GetLastError());
            if (WaitForSingleObject(context.stopEvent.get(), 750) ==
                WAIT_OBJECT_0) break;
            continue;
        }
        target.release();

        HANDLE waits[] = {context.stopEvent.get(), worker};
        const DWORD waitResult = WaitForMultipleObjects(
            ARRAYSIZE(waits), waits, FALSE, INFINITE);
        if (waitResult == WAIT_OBJECT_0) {
            if (WaitForSingleObject(worker, 2500) == WAIT_TIMEOUT) {
                const HRESULT cancelResult = CoCancelCall(workerThreadId, 1);
                if (SUCCEEDED(cancelResult)) {
                    Wh_Log(L"Cancellation requested for UI Automation worker");
                } else {
                    Wh_Log(L"Failed to cancel UI Automation worker: 0x%08X",
                        cancelResult);
                }
                if (WaitForSingleObject(worker, 2500) == WAIT_TIMEOUT) {
                    context.locatorShutdownIncomplete.store(
                        true, std::memory_order_release);
                    Wh_Log(L"UI Automation worker did not stop within 5000 ms");
                }
            }
            CloseHandle(worker);
            break;
        }
        if (waitResult != WAIT_OBJECT_0 + 1) {
            Wh_Log(L"UI Automation supervisor wait failed: %u", GetLastError());
            context.locatorShutdownIncomplete.store(
                true, std::memory_order_release);
            CloseHandle(worker);
            break;
        }
        CloseHandle(worker);
        if (WaitForSingleObject(context.stopEvent.get(), 100) ==
            WAIT_OBJECT_0) break;
    }

    PublishLayout(context, {});
    Wh_Log(L"UI Automation locator supervisor stopped");
    CoUninitialize();
    return 0;
}

}  // namespace tas



namespace tas {

std::wstring MakeOverlayWindowClassName(unsigned serial) {
    return std::wstring(kOverlayWindowClass) + L"_" +
           std::to_wstring(serial);
}

namespace {

constexpr UINT_PTR kFrameTimer = 1;
constexpr UINT_PTR kStateSafetyTimer = 2;
constexpr UINT_PTR kShellStateDebounceTimer = 3;
constexpr UINT kShellStateChangedMessage = WM_APP + 1;
constexpr UINT kStateSafetyRefreshMs = 30000;
constexpr UINT kShellStateDebounceMs = 200;
constexpr UINT kPendingForegroundChange = 1 << 0;
constexpr UINT kPendingLocationChange = 1 << 1;
constexpr DWORD kEventHookFlags =
    WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS;

struct WindowState {
    ApplicationContext* context = nullptr;
    SearchLayout layout{};
    std::array<float, kMaxBars> displayed{};
    std::array<PeakState, kMaxBars> peaks{};
    HDC memoryDc = nullptr;
    HBITMAP bitmap = nullptr;
    HGDIOBJ originalBitmap = nullptr;
    void* pixels = nullptr;
    std::vector<uint32_t> framePixels;
    std::vector<uint32_t> presentedPixels;
    int surfaceWidth = 0;
    int surfaceHeight = 0;
    ULONGLONG lastFrameTick = 0;
    ULONGLONG lastAudibleTick = 0;
    UINT frameInterval = 0;
    HWND ownerTaskbar = nullptr;
    HWINEVENTHOOK locationHook = nullptr;
    DWORD searchMode = 0;
    bool searchInterfaceOpen = false;
    bool searchInterfaceWasOpen = false;
    ULONGLONG lastZOrderLog = 0;
    bool positioned = false;
    bool eligible = false;
    bool visible = false;
    bool frameTimerActive = false;
    bool frameTimerErrorLogged = false;
    bool locationHookErrorLogged = false;
    bool updateErrorLogged = false;
    bool zOrderErrorLogged = false;
};

thread_local HWND g_shellStateTargetWindow = nullptr;
thread_local HWND g_foregroundRootWindow = nullptr;
thread_local UINT g_pendingShellStateChanges = 0;
std::atomic<unsigned> g_overlayClassSerial{0};

void CALLBACK ShellStateWinEventProc(HWINEVENTHOOK, DWORD event,
                                     HWND eventWindow, LONG objectId,
                                     LONG childId, DWORD, DWORD) {
    if (!g_shellStateTargetWindow) return;
    UINT pendingChange = 0;
    if (event == EVENT_SYSTEM_FOREGROUND) {
        g_foregroundRootWindow = eventWindow
            ? GetAncestor(eventWindow, GA_ROOT)
            : nullptr;
        pendingChange = kPendingForegroundChange;
    } else if (event == EVENT_OBJECT_LOCATIONCHANGE) {
        if (!eventWindow || objectId != OBJID_WINDOW ||
            childId != CHILDID_SELF) {
            return;
        }
        if (!g_foregroundRootWindow ||
            GetAncestor(eventWindow, GA_ROOT) != g_foregroundRootWindow) {
            return;
        }
        pendingChange = kPendingLocationChange;
    } else {
        return;
    }

    const UINT previousChanges = g_pendingShellStateChanges;
    g_pendingShellStateChanges |= pendingChange;
    if (!previousChanges &&
        !PostMessageW(g_shellStateTargetWindow,
                      kShellStateChangedMessage, 0, 0)) {
        g_pendingShellStateChanges = 0;
    }
}

void UpdateForegroundLocationHook(WindowState* state) {
    if (!state) return;
    if (state->locationHook) {
        UnhookWinEvent(state->locationHook);
        state->locationHook = nullptr;
    }

    DWORD foregroundProcessId = 0;
    if (g_foregroundRootWindow && IsWindow(g_foregroundRootWindow)) {
        GetWindowThreadProcessId(g_foregroundRootWindow,
                                 &foregroundProcessId);
    }
    if (!foregroundProcessId) {
        state->locationHookErrorLogged = false;
        return;
    }

    state->locationHook = SetWinEventHook(
        EVENT_OBJECT_LOCATIONCHANGE, EVENT_OBJECT_LOCATIONCHANGE,
        nullptr, ShellStateWinEventProc, foregroundProcessId, 0,
        kEventHookFlags);
    if (!state->locationHook) {
        if (!state->locationHookErrorLogged) {
            Wh_Log(L"Failed to watch foreground-window geometry: %u",
                GetLastError());
            state->locationHookErrorLogged = true;
        }
        return;
    }
    state->locationHookErrorLogged = false;
}

bool StartFrameTimer(HWND window, WindowState* state) {
    if (!state) return false;
    if (state->frameTimerActive) return true;
    if (!SetTimer(window, kFrameTimer, state->frameInterval, nullptr)) {
        if (!state->frameTimerErrorLogged) {
            Wh_Log(L"Failed to start spectrum frame timer: %u", GetLastError());
            state->frameTimerErrorLogged = true;
        }
        return false;
    }
    state->frameTimerActive = true;
    state->frameTimerErrorLogged = false;
    state->lastFrameTick = 0;
    return true;
}

void StopFrameTimer(HWND window, WindowState* state) {
    if (!state || !state->frameTimerActive) return;
    KillTimer(window, kFrameTimer);
    state->frameTimerActive = false;
    state->lastFrameTick = 0;
}

bool IsWindowAbove(HWND first, HWND second) {
    if (!first || !second) return false;
    for (HWND window = GetWindow(second, GW_HWNDPREV); window;
         window = GetWindow(window, GW_HWNDPREV)) {
        if (window == first) return true;
    }
    return false;
}

bool EnsureAboveTaskbar(HWND window, WindowState* state) {
    if (!state || !state->visible || !state->layout.taskbar ||
        !IsWindow(state->layout.taskbar)) {
        return true;
    }
    if (!IsWindowAbove(state->layout.taskbar, window)) return true;
    if (!SetWindowPos(window, HWND_TOPMOST, 0, 0, 0, 0,
                      SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE |
                          SWP_NOOWNERZORDER)) {
        if (!state->zOrderErrorLogged) {
            Wh_Log(L"Failed to restore overlay above the taskbar: %u",
                GetLastError());
            state->zOrderErrorLogged = true;
        }
        return false;
    }

    state->zOrderErrorLogged = false;
    const ULONGLONG now = GetTickCount64();
    if (!state->lastZOrderLog || now - state->lastZOrderLog >= 60000) {
        Wh_Log(L"Overlay z-order restored above the taskbar");
        state->lastZOrderLog = now;
    }
    return true;
}

bool EnsureTaskbarOwner(HWND window, WindowState* state,
                        const SearchLayout& layout) {
    if (!window || !state || !layout.taskbar ||
        !IsWindow(layout.taskbar)) {
        return false;
    }
    if (state->ownerTaskbar == layout.taskbar &&
        reinterpret_cast<HWND>(GetWindowLongPtrW(window, GWLP_HWNDPARENT)) ==
            layout.taskbar) {
        return true;
    }

    SetLastError(ERROR_SUCCESS);
    const LONG_PTR previous = SetWindowLongPtrW(
        window, GWLP_HWNDPARENT,
        reinterpret_cast<LONG_PTR>(layout.taskbar));
    if (!previous && GetLastError() != ERROR_SUCCESS) {
        Wh_Log(L"Failed to assign taskbar as overlay owner: %u", GetLastError());
        return false;
    }
    state->ownerTaskbar = layout.taskbar;
    if (!SetWindowPos(window, HWND_TOPMOST, 0, 0, 0, 0,
                      SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE |
                          SWP_NOOWNERZORDER | SWP_FRAMECHANGED)) {
        Wh_Log(L"Failed to apply taskbar overlay ownership: %u", GetLastError());
        return false;
    }
    Wh_Log(L"Overlay attached as an owned top-level window: explorer=%lu",
        layout.explorerProcessId);
    return true;
}

void HideOverlay(HWND window, WindowState* state, bool deactivate = true) {
    if (!state) return;
    if (deactivate) {
        StopFrameTimer(window, state);
        SetVisualizerActive(*state->context, false);
    }
    state->peaks = {};
    if (deactivate) state->displayed = {};
    state->lastFrameTick = 0;
    if (!state->visible) return;
    ShowWindow(window, SW_HIDE);
    SetWindowPos(window, HWND_NOTOPMOST, 0, 0, 0, 0,
                 SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE |
                     SWP_NOOWNERZORDER);
    state->visible = false;
}

bool ApplyWindowLayout(HWND window, WindowState* state,
                       const SearchLayout& layout) {
    if (!state || !layout.valid) return false;
    const int width = layout.rect.right - layout.rect.left;
    const int height = layout.rect.bottom - layout.rect.top;
    if (width <= 0 || height <= 0) return false;
    if (!EnsureTaskbarOwner(window, state, layout)) return false;
    const bool changed = state->layout.generation != layout.generation ||
                         !RectEquals(state->layout.rect, layout.rect);
    state->layout = layout;
    if (!changed && state->positioned) return true;
    const BOOL positioned = SetWindowPos(
        window, HWND_TOPMOST, layout.rect.left, layout.rect.top, width, height,
        SWP_NOACTIVATE | SWP_NOOWNERZORDER |
            (state->visible ? SWP_SHOWWINDOW : 0));
    state->positioned = positioned != FALSE;
    return state->positioned;
}

void DestroySurface(WindowState* state) {
    if (!state) return;
    if (state->memoryDc && state->originalBitmap) {
        SelectObject(state->memoryDc, state->originalBitmap);
    }
    if (state->bitmap) DeleteObject(state->bitmap);
    if (state->memoryDc) DeleteDC(state->memoryDc);
    state->memoryDc = nullptr;
    state->bitmap = nullptr;
    state->originalBitmap = nullptr;
    state->pixels = nullptr;
    state->surfaceWidth = 0;
    state->surfaceHeight = 0;
    state->framePixels.clear();
    state->presentedPixels.clear();
}

bool EnsureSurface(WindowState* state, int width, int height) {
    if (state->memoryDc && state->bitmap && state->pixels &&
        state->surfaceWidth == width && state->surfaceHeight == height) {
        return true;
    }
    if (!state->memoryDc) {
        state->memoryDc = CreateCompatibleDC(nullptr);
        if (!state->memoryDc) return false;
    }
    BITMAPINFO bitmapInfo{};
    bitmapInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bitmapInfo.bmiHeader.biWidth = width;
    bitmapInfo.bmiHeader.biHeight = -height;
    bitmapInfo.bmiHeader.biPlanes = 1;
    bitmapInfo.bmiHeader.biBitCount = 32;
    bitmapInfo.bmiHeader.biCompression = BI_RGB;
    void* pixels = nullptr;
    HBITMAP bitmap = CreateDIBSection(
        nullptr, &bitmapInfo, DIB_RGB_COLORS, &pixels, nullptr, 0);
    if (!bitmap || !pixels) {
        if (bitmap) DeleteObject(bitmap);
        return false;
    }
    HGDIOBJ previous = SelectObject(state->memoryDc, bitmap);
    if (!previous || previous == HGDI_ERROR) {
        DeleteObject(bitmap);
        return false;
    }
    if (state->bitmap) {
        DeleteObject(state->bitmap);
    } else {
        state->originalBitmap = previous;
    }
    state->bitmap = bitmap;
    state->pixels = pixels;
    state->surfaceWidth = width;
    state->surfaceHeight = height;
    state->presentedPixels.clear();
    return true;
}

bool NeedsFrameUpdates(const WindowState* state) {
    if (!state) return false;
    if (state->context->publishedSignalActive.load(
            std::memory_order_acquire)) {
        return true;
    }
    for (int index = 0; index < state->context->settings.barCount; ++index) {
        const PeakState& peak = state->peaks[index];
        if (state->displayed[index] != 0.0f || peak.level != 0.0f ||
            peak.velocity != 0.0f) {
            return true;
        }
    }
    return false;
}

bool DrawSpectrum(HWND window, WindowState* state) {
    ApplicationContext& context = *state->context;
    const Settings& settings = context.settings;
    const int width = state->layout.rect.right - state->layout.rect.left;
    const int height = state->layout.rect.bottom - state->layout.rect.top;
    if (width <= 0 || height <= 0) return false;

    const int bars = settings.barCount;
    const ULONGLONG frameTick = GetTickCount64();
    const float frameSeconds = state->lastFrameTick
        ? std::clamp((frameTick - state->lastFrameTick) / 1000.0f,
                     0.0f, 0.25f)
        : 1.0f / settings.fps;
    state->lastFrameTick = frameTick;
    bool audible = false;
    std::array<float, kMaxBars> shaped{};
    for (int index = 0; index < bars; ++index) {
        const float target =
            context.bands[index].load(std::memory_order_relaxed);
        state->displayed[index] = SmoothDisplayLevel(
            state->displayed[index], target, frameSeconds,
            settings.attackMs, settings.releaseMs);
        if (state->displayed[index] < 0.0005f) state->displayed[index] = 0.0f;
        if (state->displayed[index] > settings.silenceThreshold) {
            audible = true;
        }
        shaped[index] = std::pow(state->displayed[index], 0.72f);
        if (settings.peakEnabled) {
            UpdatePeak(&state->peaks[index], shaped[index], frameSeconds,
                       settings.peakHoldMs / 1000.0f,
                       settings.peakGravity);
        } else {
            state->peaks[index] = {};
        }
    }
    if (audible || !state->lastAudibleTick) {
        state->lastAudibleTick = frameTick;
    }
    const bool silenceDelayElapsed = !audible &&
        frameTick - state->lastAudibleTick >=
            static_cast<ULONGLONG>(settings.silenceHideDelayMs);
    if (settings.hideWhenSilent && silenceDelayElapsed) {
        HideOverlay(window, state, false);
        return NeedsFrameUpdates(state);
    }

    const RECT spectrumBounds =
        CalculateSpectrumBounds(state->layout, settings);
    const int spectrumLeft = static_cast<int>(spectrumBounds.left);
    const int spectrumRight = static_cast<int>(spectrumBounds.right);
    const int spectrumTop = static_cast<int>(spectrumBounds.top);
    const int spectrumBottom = static_cast<int>(spectrumBounds.bottom);
    const int usableWidth = spectrumRight - spectrumLeft;
    const int maximumHeight = spectrumBottom - spectrumTop;
    if (usableWidth <= 0 || maximumHeight <= 0) {
        HideOverlay(window, state, false);
        return false;
    }
    if (!EnsureSurface(state, width, height)) {
        if (!state->updateErrorLogged) {
            Wh_Log(L"Failed to allocate spectrum surface: %u", GetLastError());
            state->updateErrorLogged = true;
        }
        HideOverlay(window, state);
        return false;
    }
    const size_t pixelCount = static_cast<size_t>(width) * height;
    state->framePixels.assign(pixelCount, 0);
    auto* buffer = state->framePixels.data();
    const auto makePixel = [](COLORREF color, BYTE alpha) -> uint32_t {
        return (static_cast<uint32_t>(alpha) << 24) |
               (static_cast<uint32_t>(GetRValue(color) * alpha / 255) << 16) |
               (static_cast<uint32_t>(GetGValue(color) * alpha / 255) << 8) |
               (GetBValue(color) * alpha / 255);
    };
    const auto fillRect = [&](int x0, int y0, int x1, int y1,
                              uint32_t pixel) {
        x0 = std::clamp(x0, 0, width);
        x1 = std::clamp(x1, 0, width);
        y0 = std::clamp(y0, 0, height);
        y1 = std::clamp(y1, 0, height);
        for (int y = y0; y < y1; ++y) {
            for (int x = x0; x < x1; ++x) {
                uint32_t& destination = buffer[y * width + x];
                destination = pixel;
            }
        }
    };

    const float slot = usableWidth / static_cast<float>(bars);
    const int barWidth = std::max(
        1, static_cast<int>(slot * settings.barWidthPercent / 100.0f));
    const int peakHeight = std::clamp(
        ScaleForDpi(settings.peakHeight, state->layout.dpi),
        1, maximumHeight);
    const int peakGap =
        std::max(0, ScaleForDpi(settings.peakGap, state->layout.dpi));
    for (int index = 0; index < bars; ++index) {
        const LevelPixelCoverage barCoverage =
            CalculateLevelPixelCoverage(shaped[index], maximumHeight);
        const int barHeight = barCoverage.fullPixels;
        const int x0 = spectrumLeft +
                       static_cast<int>(index * slot +
                                        (slot - barWidth) / 2);
        const int x1 = std::min(spectrumRight, x0 + barWidth);
        const int y1 = spectrumBottom;
        const int y0 = std::max(spectrumTop, y1 - barHeight);
        const float mix = index /
            static_cast<float>(std::max(1, bars - 1));
        const BYTE red = static_cast<BYTE>(
            GetRValue(settings.color) * (1 - mix) +
            GetRValue(settings.secondColor) * mix);
        const BYTE green = static_cast<BYTE>(
            GetGValue(settings.color) * (1 - mix) +
            GetGValue(settings.secondColor) * mix);
        const BYTE blue = static_cast<BYTE>(
            GetBValue(settings.color) * (1 - mix) +
            GetBValue(settings.secondColor) * mix);
        const COLORREF barColor = RGB(red, green, blue);
        if (barHeight > 0) {
            fillRect(x0, y0, x1, y1,
                     makePixel(barColor, settings.opacity));
        }
        if (barCoverage.partialPixel > 0.0f && y0 > spectrumTop) {
            const BYTE partialOpacity = static_cast<BYTE>(std::clamp(
                static_cast<int>(std::lround(
                    settings.opacity * barCoverage.partialPixel)),
                0, 255));
            if (partialOpacity > 0) {
                fillRect(x0, y0 - 1, x1, y0,
                         makePixel(barColor, partialOpacity));
            }
        }

        if (settings.peakEnabled) {
            const int peakBarHeight = CalculateRenderedLevelHeight(
                state->peaks[index].level, maximumHeight, 0);
            const int peakBottom = CalculatePeakBlockBottom(
                peakBarHeight, spectrumTop, y1, peakHeight, peakGap,
                settings.peakShowWhenSilent);
            if (peakBottom < 0) continue;
            const int peakTop = peakBottom - peakHeight;
            const BYTE peakRed = static_cast<BYTE>((red * 3 + 255) / 4);
            const BYTE peakGreen = static_cast<BYTE>((green * 3 + 255) / 4);
            const BYTE peakBlue = static_cast<BYTE>((blue * 3 + 255) / 4);
            const BYTE peakOpacity = static_cast<BYTE>(
                std::min(255, static_cast<int>(settings.opacity) + 55));
            fillRect(x0, peakTop, x1, peakBottom,
                     makePixel(RGB(peakRed, peakGreen, peakBlue),
                               peakOpacity));
        }
    }

    SIZE size{width, height};
    POINT source{};
    BLENDFUNCTION blend{AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
    if (state->visible && state->presentedPixels == state->framePixels) {
        return NeedsFrameUpdates(state);
    }
    memcpy(state->pixels, state->framePixels.data(),
           pixelCount * sizeof(uint32_t));
    if (!UpdateLayeredWindow(window, nullptr, nullptr, &size, state->memoryDc,
                             &source, 0, &blend, ULW_ALPHA)) {
        if (!state->updateErrorLogged) {
            Wh_Log(L"UpdateLayeredWindow failed: %u", GetLastError());
            state->updateErrorLogged = true;
        }
        return NeedsFrameUpdates(state);
    }
    state->updateErrorLogged = false;
    state->presentedPixels = state->framePixels;
    if (!state->visible) {
        SetWindowPos(window, HWND_TOPMOST, 0, 0, 0, 0,
                     SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE |
                         SWP_NOOWNERZORDER | SWP_SHOWWINDOW);
        state->visible = true;
        SetVisualizerActive(context, true);
        EnsureAboveTaskbar(window, state);
    }
    return NeedsFrameUpdates(state);
}

void RefreshOverlayState(HWND window, WindowState* state,
                         bool refreshSearchMode,
                         bool refreshSearchInterface) {
    if (!state) return;
    if (state->context->settings.opacity == 0) {
        state->eligible = false;
        HideOverlay(window, state);
        return;
    }
    if (refreshSearchMode) {
        state->searchMode = GetSearchMode();
    }
    if (refreshSearchInterface) {
        state->searchInterfaceOpen = IsSearchInterfaceOpen();
        if (state->searchInterfaceOpen) {
            if (!state->searchInterfaceWasOpen) {
                Wh_Log(L"Search interface open detected");
            }
            state->searchInterfaceWasOpen = true;
        } else if (state->searchInterfaceWasOpen) {
            state->searchInterfaceWasOpen = false;
            state->positioned = false;
            Wh_Log(L"Search interface dismissed; restoring owned overlay");
        }
    }

    SearchLayout latest;
    if (!ReadLatestSearchLayout(*state->context, &latest)) {
        state->eligible = false;
        HideOverlay(window, state);
        return;
    }
    const bool fullscreenForeground = IsFullscreenForeground(latest);
    if (!ShouldShowOverlay(latest, state->searchMode,
                           state->searchInterfaceOpen,
                           fullscreenForeground)) {
        state->eligible = false;
        HideOverlay(window, state);
        return;
    }
    state->eligible = true;
    SetVisualizerActive(*state->context, true);
    const bool changed =
        state->layout.generation != latest.generation;
    const DWORD previousExplorer = state->layout.explorerProcessId;
    if (!ApplyWindowLayout(window, state, latest)) {
        Wh_Log(L"Failed to position overlay: %u", GetLastError());
        state->eligible = false;
        HideOverlay(window, state);
        return;
    }
    EnsureAboveTaskbar(window, state);
    if (changed) {
        const RECT spectrumBounds = CalculateSpectrumBounds(
            latest, state->context->settings);
        if (previousExplorer != latest.explorerProcessId) {
            Wh_Log(L"Search box attached: explorer=%lu left=%ld top=%ld width=%ld height=%ld insets=%d/%d/%d/%d dpi=%u",
                latest.explorerProcessId, latest.rect.left,
                latest.rect.top, latest.rect.right - latest.rect.left,
                latest.rect.bottom - latest.rect.top,
                spectrumBounds.left,
                latest.rect.right - latest.rect.left - spectrumBounds.right,
                spectrumBounds.top,
                latest.rect.bottom - latest.rect.top - spectrumBounds.bottom,
                latest.dpi);
        } else {
            Wh_Log(L"Spectrum layout updated: explorer=%lu left=%ld top=%ld width=%ld height=%ld insets=%d/%d/%d/%d dpi=%u",
                latest.explorerProcessId, latest.rect.left,
                latest.rect.top, latest.rect.right - latest.rect.left,
                latest.rect.bottom - latest.rect.top,
                spectrumBounds.left,
                latest.rect.right - latest.rect.left - spectrumBounds.right,
                spectrumBounds.top,
                latest.rect.bottom - latest.rect.top - spectrumBounds.bottom,
                latest.dpi);
        }
    }
    StartFrameTimer(window, state);
}

LRESULT CALLBACK OverlayWindowProc(HWND window, UINT message,
                                   WPARAM wParam, LPARAM lParam) {
    auto* state = reinterpret_cast<WindowState*>(
        GetWindowLongPtrW(window, GWLP_USERDATA));
    switch (message) {
        case WM_NCCREATE: {
            auto* create = reinterpret_cast<CREATESTRUCTW*>(lParam);
            SetWindowLongPtrW(
                window, GWLP_USERDATA,
                reinterpret_cast<LONG_PTR>(create->lpCreateParams));
            return TRUE;
        }
        case kShellStateChangedMessage: {
            if (!state) return 0;
            const UINT pendingChanges =
                std::exchange(g_pendingShellStateChanges, 0u);
            if (pendingChanges & kPendingForegroundChange) {
                KillTimer(window, kShellStateDebounceTimer);
                UpdateForegroundLocationHook(state);
                RefreshOverlayState(window, state, false, true);
            } else if (pendingChanges & kPendingLocationChange) {
                if (!SetTimer(window, kShellStateDebounceTimer,
                              kShellStateDebounceMs, nullptr)) {
                    RefreshOverlayState(window, state, false, false);
                }
            }
            return 0;
        }
        case WM_TIMER:
            if (!state) return 0;
            if (wParam == kStateSafetyTimer) {
                RefreshOverlayState(window, state, true, true);
            } else if (wParam == kShellStateDebounceTimer) {
                KillTimer(window, kShellStateDebounceTimer);
                RefreshOverlayState(window, state, false, false);
            } else if (wParam == kFrameTimer && state->positioned &&
                       state->eligible) {
                if (!DrawSpectrum(window, state)) {
                    StopFrameTimer(window, state);
                }
            }
            return 0;
        case WM_MOUSEACTIVATE:
            return MA_NOACTIVATE;
        case WM_NCHITTEST:
            return HTTRANSPARENT;
        case WM_DESTROY:
            if (state) SetVisualizerActive(*state->context, false);
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProcW(window, message, wParam, lParam);
}

}  // namespace

DWORD WINAPI OverlayThreadProc(void* parameter) {
    auto& context = *static_cast<ApplicationContext*>(parameter);
    EnablePerMonitorDpiAwarenessForThread();
    const std::wstring overlayClassName = MakeOverlayWindowClassName(
        g_overlayClassSerial.fetch_add(1, std::memory_order_relaxed) + 1);
    WNDCLASSEXW windowClass{};
    windowClass.cbSize = sizeof(windowClass);
    windowClass.lpfnWndProc = OverlayWindowProc;
    HMODULE currentModule = nullptr;
    if (!GetModuleHandleExW(
            GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
            reinterpret_cast<PCWSTR>(&OverlayThreadProc), &currentModule)) {
        Wh_Log(L"Failed to resolve overlay module: %u", GetLastError());
        return 0;
    }
    windowClass.hInstance = currentModule;
    windowClass.lpszClassName = overlayClassName.c_str();
    windowClass.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    const bool registeredWindowClass = RegisterClassExW(&windowClass) != 0;
    if (!registeredWindowClass) {
        Wh_Log(L"Failed to register overlay class: %u", GetLastError());
        return 0;
    }

    const UINT frameInterval = std::clamp<UINT>(
        static_cast<UINT>((1000 + context.settings.fps / 2) /
                          context.settings.fps),
        USER_TIMER_MINIMUM, 1000);
    RegistryChangeWatcher searchModeWatcher;
    if (searchModeWatcher.Start(kSearchSettingsRegistryPath)) {
        Wh_Log(L"Overlay search mode watcher started");
    } else {
        Wh_Log(L"Overlay search mode watcher unavailable; using safety refresh");
    }
    bool rendererStarted = false;
    while (WaitForSingleObject(context.stopEvent.get(), 0) != WAIT_OBJECT_0) {
        WindowState state;
        state.context = &context;
        state.frameInterval = frameInterval;
        HWND window = CreateWindowExW(
            WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW |
                WS_EX_NOACTIVATE,
            windowClass.lpszClassName, L"", WS_POPUP, 0, 0, 0, 0, nullptr,
            nullptr, windowClass.hInstance, &state);
        if (!window) {
            Wh_Log(L"Failed to create overlay window: %u", GetLastError());
            break;
        }
        if (!SetTimer(window, kStateSafetyTimer,
                      kStateSafetyRefreshMs, nullptr)) {
            Wh_Log(L"Failed to create overlay timer: %u", GetLastError());
            DestroyWindow(window);
            DestroySurface(&state);
            break;
        }
        g_shellStateTargetWindow = window;
        const HWND foreground = GetForegroundWindow();
        g_foregroundRootWindow = foreground
            ? GetAncestor(foreground, GA_ROOT)
            : nullptr;
        g_pendingShellStateChanges = 0;
        const HWINEVENTHOOK foregroundHook = SetWinEventHook(
            EVENT_SYSTEM_FOREGROUND, EVENT_SYSTEM_FOREGROUND, nullptr,
            ShellStateWinEventProc, 0, 0, kEventHookFlags);
        const DWORD foregroundHookError =
            foregroundHook ? ERROR_SUCCESS : GetLastError();
        UpdateForegroundLocationHook(&state);
        if (!foregroundHook) {
            Wh_Log(L"Foreground event watcher unavailable: %u",
                foregroundHookError);
        }
        if (rendererStarted) {
            Wh_Log(L"Spectrum renderer recreated after taskbar replacement");
        } else {
            Wh_Log(L"Spectrum renderer started as an owned top-level overlay");
        }
        rendererStarted = true;

        RefreshOverlayState(window, &state, true, true);
        bool messageLoopFailed = false;
        bool ownerReplaced = false;
        while (!ownerReplaced) {
            HANDLE waits[4] = {context.stopEvent.get(),
                               context.layoutChangedEvent.get(),
                               context.bandActivityEvent.get()};
            DWORD waitCount = 3;
            DWORD searchModeWaitIndex = MAXDWORD;
            if (searchModeWatcher.changedEvent()) {
                searchModeWaitIndex = waitCount;
                waits[waitCount++] = searchModeWatcher.changedEvent();
            }
            const DWORD waitResult = MsgWaitForMultipleObjectsEx(
                waitCount, waits, INFINITE, QS_ALLINPUT,
                MWMO_INPUTAVAILABLE);
            if (waitResult == WAIT_FAILED) {
                Wh_Log(L"Overlay message wait failed: %u", GetLastError());
                messageLoopFailed = true;
                break;
            }
            if (waitResult == WAIT_OBJECT_0) break;
            if (waitResult == WAIT_OBJECT_0 + 1) {
                RefreshOverlayState(window, &state, false, false);
                continue;
            }
            if (waitResult == WAIT_OBJECT_0 + 2) {
                if (state.positioned && state.eligible) {
                    StartFrameTimer(window, &state);
                }
                continue;
            }
            if (searchModeWaitIndex != MAXDWORD &&
                waitResult == WAIT_OBJECT_0 + searchModeWaitIndex) {
                if (!searchModeWatcher.Arm()) {
                    Wh_Log(L"Overlay search mode watcher could not be rearmed");
                    searchModeWatcher.Stop();
                }
                RefreshOverlayState(window, &state, true, false);
                continue;
            }
            if (waitResult != WAIT_OBJECT_0 + waitCount) {
                Wh_Log(L"Overlay message wait returned unexpected status: %u",
                    waitResult);
                messageLoopFailed = true;
                break;
            }

            MSG message;
            while (PeekMessageW(&message, nullptr, 0, 0, PM_REMOVE)) {
                if (message.message == WM_QUIT) {
                    ownerReplaced = true;
                    break;
                }
                TranslateMessage(&message);
                DispatchMessageW(&message);
            }
        }
        g_shellStateTargetWindow = nullptr;
        g_foregroundRootWindow = nullptr;
        g_pendingShellStateChanges = 0;
        if (foregroundHook) UnhookWinEvent(foregroundHook);
        if (state.locationHook) UnhookWinEvent(state.locationHook);
        StopFrameTimer(window, &state);
        KillTimer(window, kStateSafetyTimer);
        KillTimer(window, kShellStateDebounceTimer);
        if (IsWindow(window)) DestroyWindow(window);
        DestroySurface(&state);
        if (messageLoopFailed ||
            WaitForSingleObject(context.stopEvent.get(), 0) == WAIT_OBJECT_0) {
            break;
        }
        Wh_Log(L"Overlay owner was replaced; recreating renderer");
    }
    if (registeredWindowClass) {
        UnregisterClassW(windowClass.lpszClassName, windowClass.hInstance);
    }
    Wh_Log(L"Spectrum renderer stopped");
    return 0;
}

}  // namespace tas



namespace tas {

void ClearPublishedBands(ApplicationContext& context) {
    for (auto& band : context.bands) {
        band.store(0.0f, std::memory_order_relaxed);
    }
    context.publishedSignalActive.store(false, std::memory_order_release);
}

void PublishBandLevels(ApplicationContext& context, const BandLevels& levels) {
    bool signalActive = false;
    for (size_t index = 0; index < levels.size(); ++index) {
        context.bands[index].store(levels[index], std::memory_order_relaxed);
        signalActive = signalActive || levels[index] > 0.0f;
    }
    const bool wasActive = context.publishedSignalActive.exchange(
        signalActive, std::memory_order_acq_rel);
    if (signalActive && !wasActive && context.bandActivityEvent) {
        SetEvent(context.bandActivityEvent.get());
    }
}

namespace {

constexpr DWORD kRuntimeShutdownTimeoutMs = 5500;

bool WaitForThreadUntil(HANDLE thread, ULONGLONG deadline, PCWSTR name) {
    if (!thread) return true;
    const ULONGLONG now = GetTickCount64();
    const DWORD remaining = now >= deadline
        ? 0
        : static_cast<DWORD>(std::min<ULONGLONG>(
              deadline - now, MAXDWORD));
    const DWORD result = WaitForSingleObject(thread, remaining);
    if (result == WAIT_OBJECT_0) return true;
    if (result == WAIT_TIMEOUT) {
        Wh_Log(L"%ls thread did not stop before the shared shutdown deadline",
            name);
    } else {
        Wh_Log(L"%ls thread wait failed: %u", name, GetLastError());
    }
    return false;
}

}  // namespace

Runtime::Runtime() = default;

Runtime::~Runtime() {
    Stop();
}

void SetVisualizerActive(ApplicationContext& context, bool active) {
    if (context.visualizerActive.exchange(active, std::memory_order_acq_rel) !=
            active &&
        context.activityChangedEvent) {
        SetEvent(context.activityChangedEvent.get());
    }
}

bool Runtime::running() const {
    return context_ && static_cast<bool>(context_->stopEvent);
}

bool Runtime::Start(const Settings& settings) {
    if (running()) return false;
    context_ = std::make_unique<ApplicationContext>();
    context_->settings = settings;
    ClearPublishedBands(*context_);
    context_->visualizerActive.store(false, std::memory_order_release);
    context_->locatorShutdownIncomplete.store(false, std::memory_order_release);
    context_->stopEvent.reset(CreateEventW(nullptr, TRUE, FALSE, nullptr));
    context_->activityChangedEvent.reset(
        CreateEventW(nullptr, FALSE, FALSE, nullptr));
    context_->bandActivityEvent.reset(
        CreateEventW(nullptr, FALSE, FALSE, nullptr));
    context_->layoutChangedEvent.reset(
        CreateEventW(nullptr, FALSE, FALSE, nullptr));
    if (!context_->stopEvent || !context_->activityChangedEvent ||
        !context_->bandActivityEvent || !context_->layoutChangedEvent) {
        context_.reset();
        return false;
    }

    locatorThread_.reset(CreateThread(
        nullptr, 0, SearchLocatorThreadProc, context_.get(), 0, nullptr));
    overlayThread_.reset(CreateThread(
        nullptr, 0, OverlayThreadProc, context_.get(), 0, nullptr));
    audioThread_.reset(CreateThread(
        nullptr, 0, AudioThreadProc, context_.get(), 0, nullptr));
    if (!locatorThread_ || !overlayThread_ || !audioThread_) {
        Wh_Log(L"Failed to create runtime threads: %u", GetLastError());
        Stop();
        return false;
    }
    Wh_Log(L"Runtime started");
    return true;
}

bool Runtime::Stop() {
    if (!running()) return true;
    SetVisualizerActive(*context_, false);
    SetEvent(context_->stopEvent.get());
    if (context_->activityChangedEvent) {
        SetEvent(context_->activityChangedEvent.get());
    }

    const ULONGLONG shutdownDeadline =
        GetTickCount64() + kRuntimeShutdownTimeoutMs;
    const bool locatorStopped =
        WaitForThreadUntil(locatorThread_.get(), shutdownDeadline, L"Locator");
    const bool overlayStopped =
        WaitForThreadUntil(overlayThread_.get(), shutdownDeadline, L"Overlay");
    const bool audioStopped =
        WaitForThreadUntil(audioThread_.get(), shutdownDeadline, L"Audio");
    if (!locatorStopped ||
        context_->locatorShutdownIncomplete.load(std::memory_order_acquire) ||
        !overlayStopped || !audioStopped) {
        Wh_Log(L"Runtime shutdown incomplete; abandoning stale runtime state");
        locatorThread_.reset();
        overlayThread_.reset();
        audioThread_.reset();
        context_.release();
        return false;
    }

    locatorThread_.reset();
    overlayThread_.reset();
    audioThread_.reset();
    ClearPublishedBands(*context_);
    context_.reset();
    Wh_Log(L"Runtime stopped cleanly");
    return true;
}

}  // namespace tas

namespace {

[[clang::no_destroy]] std::optional<tas::Runtime> g_appRuntime;

}  // namespace

namespace tas {

Runtime& AppRuntime() {
    if (!g_appRuntime) g_appRuntime.emplace();
    return *g_appRuntime;
}

int HostGetIntSetting(PCWSTR key, int) {
    return Wh_GetIntSetting(key);
}

std::wstring HostGetStringSetting(PCWSTR key, PCWSTR) {
    const auto value = WindhawkUtils::StringSetting::make(key);
    return value.get();
}

}  // namespace tas

BOOL WhTool_ModInit() {
    Wh_Log(L"Windhawk host starting");
    return tas::AppRuntime().Start(tas::LoadSettings()) ? TRUE : FALSE;
}

void WhTool_ModSettingsChanged() {
    if (!tas::AppRuntime().Stop()) {
        Wh_Log(L"Stale runtime state was abandoned after shutdown timed out");
    }
    if (!tas::AppRuntime().Start(tas::LoadSettings())) {
        Wh_Log(L"Runtime restart failed after settings changed");
    }
}

void WhTool_ModUninit() {
    g_appRuntime.reset();
    Wh_Log(L"Windhawk host stopped");
}

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
