// ==WindhawkMod==
// @id              withered-foxy-jumpscare
// @name            Withered Foxy Jumpscare
// @description     Randomly shows the FNAF 2 Withered Foxy jumpscare fullscreen on top of all apps
// @version         1.0
// @author          Kismeria
// @github          https://github.com/Kismeria
// @include         windhawk.exe
// @compilerOptions -lgdiplus -lwinmm -lgdi32 -luser32 -lshell32 -lurlmon -lole32
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Withered Foxy Jumpscare

Every check interval (1 second by default) the mod rolls a die. With the
default chance of **1 in 100000**, the jumpscare happens about once every 28
hours of uptime.

The jumpscare is drawn in a transparent fullscreen window on top of all other
windows, on the monitor of the active window. Only Foxy is visible, with no
background.

## Files

On start, the mod downloads missing files to `%USERPROFILE%\foxy\`:

- `foxy.gif` - Withered Foxy jumpscare on a green screen. The green
  background is removed automatically.
- `scream.mp3` - FNAF 2 scream.

You can replace them with your own:

- **Image** - animated `.gif`, static `.png`, or a folder of `.png` frames
  (sorted by name). Turn off "Green screen" for images that already have
  transparency.
- **Sound** - `.wav` or `.mp3`.

To download the files again, delete them and save the settings.

## Fullscreen apps

- Windowed and borderless games: the jumpscare is drawn on top.
- Exclusive fullscreen games don't allow other windows on top. For them, keep
  "Steal focus" on: the game is minimized during the jumpscare and gets focus
  back afterwards.

## Testing

- Press the test hotkey (`Ctrl+Alt+F` by default).
- Or turn on "Test on save" and save the settings.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- chance: 100000
  $name: Chance (1 in N)
- checkIntervalMs: 1000
  $name: Check interval (ms)
- imagePath: '%USERPROFILE%\foxy\foxy.gif'
  $name: Image
  $description: .gif, .png or folder of .png frames
- soundPath: '%USERPROFILE%\foxy\scream.mp3'
  $name: Sound
  $description: .wav or .mp3
- chromaKey: true
  $name: Green screen
- autoDownload: true
  $name: Auto download
- imageUrl: 'https://media1.tenor.com/m/pC-4acFw1PAAAAAC/fnaf-foxy.gif'
  $name: Image URL
- soundUrl: 'https://archive.org/download/fnaf1n2_XScreams_1mar2022/freddy-fazbears-pizza-fandom-wikia_fnaf-2_Xscream2.mp3'
  $name: Sound URL
- volume: 100
  $name: Volume
- durationMs: 1500
  $name: Duration (ms)
  $description: 0 = animation length
- fps: 30
  $name: Frame folder FPS
- fitMode: stretch
  $name: Fit
  $options:
  - stretch: Stretch
  - cover: Fill
  - contain: Fit
- scalePercent: 100
  $name: Scale (%)
- zoomIn: false
  $name: Zoom in
- shake: true
  $name: Shake
- forceForeground: true
  $name: Steal focus
- testOnApply: false
  $name: Test on save
- testHotkey: 'Ctrl+Alt+F'
  $name: Test hotkey
*/
// ==/WindhawkModSettings==

#include <windows.h>

#include <gdiplus.h>
#include <mmsystem.h>
#include <shlobj.h>
#include <urlmon.h>

#include <algorithm>
#include <cmath>
#include <memory>
#include <random>
#include <string>
#include <vector>

namespace {

enum class FitMode {
    stretch,
    cover,
    contain,
};

struct Settings {
    FitMode fitMode;
    int chance;
    int checkIntervalMs;
    std::wstring imagePath;
    std::wstring soundPath;
    bool chromaKey;
    bool autoDownload;
    std::wstring imageUrl;
    std::wstring soundUrl;
    int volume;
    int durationMs;
    int fps;
    int scalePercent;
    bool zoomIn;
    bool shake;
    bool forceForeground;
    bool testOnApply;
    std::wstring testHotkey;
};

struct Frame {
    std::unique_ptr<Gdiplus::Bitmap> bitmap;
    int delayMs;
};

const wchar_t kWindowClass[] = L"WhWitheredFoxyJumpscare";
const wchar_t kMciAlias[] = L"whfoxyscream";
const PROPID kPropertyTagFrameDelay = 0x5100;
const int kTestHotkeyId = 1;
const GUID kFrameDimensionTime = {
    0x6aedbd6d,
    0x3fb5,
    0x418a,
    {0x83, 0xa6, 0x7f, 0x45, 0x22, 0x9d, 0xc8, 0x72}};

Settings g_settings;
HANDLE g_thread;
HANDLE g_stopEvent;

std::wstring ReadStringSetting(PCWSTR name, bool expand = true) {
    PCWSTR value = Wh_GetStringSetting(name);
    std::wstring raw = value ? value : L"";
    Wh_FreeStringSetting(value);

    if (!expand || raw.empty()) {
        return raw;
    }

    DWORD size = ExpandEnvironmentStringsW(raw.c_str(), nullptr, 0);
    if (!size) {
        return raw;
    }

    std::wstring expanded(size, L'\0');
    ExpandEnvironmentStringsW(raw.c_str(), expanded.data(), size);
    expanded.resize(size - 1);
    return expanded;
}

void LoadSettings() {
    g_settings.chance = std::max(1, Wh_GetIntSetting(L"chance"));
    g_settings.checkIntervalMs =
        std::max(50, Wh_GetIntSetting(L"checkIntervalMs"));
    g_settings.imagePath = ReadStringSetting(L"imagePath");
    g_settings.soundPath = ReadStringSetting(L"soundPath");
    g_settings.chromaKey = Wh_GetIntSetting(L"chromaKey");
    g_settings.autoDownload = Wh_GetIntSetting(L"autoDownload");
    g_settings.imageUrl = ReadStringSetting(L"imageUrl", false);
    g_settings.soundUrl = ReadStringSetting(L"soundUrl", false);
    g_settings.volume = std::clamp(Wh_GetIntSetting(L"volume"), 0, 100);
    g_settings.durationMs = std::max(0, Wh_GetIntSetting(L"durationMs"));
    g_settings.fps = std::clamp(Wh_GetIntSetting(L"fps"), 1, 240);
    g_settings.scalePercent =
        std::clamp(Wh_GetIntSetting(L"scalePercent"), 10, 500);

    std::wstring fitMode = ReadStringSetting(L"fitMode", false);
    if (fitMode == L"cover") {
        g_settings.fitMode = FitMode::cover;
    } else if (fitMode == L"contain") {
        g_settings.fitMode = FitMode::contain;
    } else {
        g_settings.fitMode = FitMode::stretch;
    }
    g_settings.zoomIn = Wh_GetIntSetting(L"zoomIn");
    g_settings.shake = Wh_GetIntSetting(L"shake");
    g_settings.forceForeground = Wh_GetIntSetting(L"forceForeground");
    g_settings.testOnApply = Wh_GetIntSetting(L"testOnApply");
    g_settings.testHotkey = ReadStringSetting(L"testHotkey");
}

bool ParseHotkey(const std::wstring& text, UINT* modifiers, UINT* virtualKey) {
    *modifiers = 0;
    *virtualKey = 0;

    size_t start = 0;
    while (start <= text.size()) {
        size_t end = text.find(L'+', start);
        if (end == std::wstring::npos) {
            end = text.size();
        }

        std::wstring token = text.substr(start, end - start);
        token.erase(0, token.find_first_not_of(L" \t"));
        token.erase(token.find_last_not_of(L" \t") + 1);
        CharUpperBuffW(token.data(), (DWORD)token.size());
        start = end + 1;

        if (token.empty()) {
            continue;
        }

        if (token == L"CTRL" || token == L"CONTROL") {
            *modifiers |= MOD_CONTROL;
        } else if (token == L"ALT") {
            *modifiers |= MOD_ALT;
        } else if (token == L"SHIFT") {
            *modifiers |= MOD_SHIFT;
        } else if (token == L"WIN") {
            *modifiers |= MOD_WIN;
        } else if (token.size() == 1 &&
                   ((token[0] >= L'A' && token[0] <= L'Z') ||
                    (token[0] >= L'0' && token[0] <= L'9'))) {
            *virtualKey = token[0];
        } else if (token.size() >= 2 && token[0] == L'F' &&
                   iswdigit(token[1])) {
            int number = _wtoi(token.c_str() + 1);
            if (number < 1 || number > 24) {
                return false;
            }
            *virtualKey = VK_F1 + number - 1;
        } else if (token == L"SPACE") {
            *virtualKey = VK_SPACE;
        } else if (token == L"ENTER") {
            *virtualKey = VK_RETURN;
        } else if (token == L"INSERT") {
            *virtualKey = VK_INSERT;
        } else if (token == L"DELETE") {
            *virtualKey = VK_DELETE;
        } else if (token == L"HOME") {
            *virtualKey = VK_HOME;
        } else if (token == L"END") {
            *virtualKey = VK_END;
        } else if (token == L"PAUSE") {
            *virtualKey = VK_PAUSE;
        } else {
            return false;
        }
    }

    return *virtualKey != 0;
}

bool IsStopRequested() {
    return WaitForSingleObject(g_stopEvent, 0) == WAIT_OBJECT_0;
}

std::unique_ptr<Gdiplus::Bitmap> RemoveGreenScreen(Gdiplus::Image* source) {
    int width = source->GetWidth();
    int height = source->GetHeight();
    auto bitmap =
        std::make_unique<Gdiplus::Bitmap>(width, height, PixelFormat32bppARGB);
    if (bitmap->GetLastStatus() != Gdiplus::Ok) {
        return nullptr;
    }

    {
        Gdiplus::Graphics graphics(bitmap.get());
        graphics.Clear(Gdiplus::Color(0, 0, 0, 0));
        graphics.DrawImage(source, Gdiplus::Rect(0, 0, width, height));
    }

    Gdiplus::Rect rect(0, 0, width, height);
    Gdiplus::BitmapData data;
    if (bitmap->LockBits(&rect, Gdiplus::ImageLockModeRead |
                                    Gdiplus::ImageLockModeWrite,
                         PixelFormat32bppARGB, &data) != Gdiplus::Ok) {
        return nullptr;
    }

    const int kOpaqueBelow = 25;
    const int kTransparentAbove = 70;

    for (int y = 0; y < height; y++) {
        BYTE* row = (BYTE*)data.Scan0 + y * data.Stride;
        for (int x = 0; x < width; x++) {
            BYTE* pixel = row + x * 4;
            int b = pixel[0];
            int g = pixel[1];
            int r = pixel[2];
            int a = pixel[3];
            int other = std::max(r, b);
            int dominance = g - other;

            if (dominance >= kTransparentAbove) {
                a = 0;
            } else if (dominance > kOpaqueBelow) {
                a = a * (kTransparentAbove - dominance) /
                    (kTransparentAbove - kOpaqueBelow);
            }

            if (g > other) {
                pixel[1] = (BYTE)other;
            }
            pixel[3] = (BYTE)a;
        }
    }

    bitmap->UnlockBits(&data);
    return bitmap;
}

std::unique_ptr<Gdiplus::Bitmap> CopyScaled(Gdiplus::Image* source,
                                            int width,
                                            int height) {
    std::unique_ptr<Gdiplus::Bitmap> keyed;
    if (g_settings.chromaKey) {
        keyed = RemoveGreenScreen(source);
        if (keyed) {
            source = keyed.get();
        }
    }

    auto bitmap = std::make_unique<Gdiplus::Bitmap>(
        width, height, PixelFormat32bppPARGB);
    if (bitmap->GetLastStatus() != Gdiplus::Ok) {
        return nullptr;
    }

    Gdiplus::Graphics graphics(bitmap.get());
    graphics.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);
    graphics.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHalf);
    graphics.Clear(Gdiplus::Color(0, 0, 0, 0));
    graphics.DrawImage(source, Gdiplus::Rect(0, 0, width, height));
    return bitmap;
}

bool FileExists(const std::wstring& path) {
    return GetFileAttributesW(path.c_str()) != INVALID_FILE_ATTRIBUTES;
}

void DownloadIfMissing(const std::wstring& path, const std::wstring& url) {
    if (path.empty() || url.empty() || FileExists(path)) {
        return;
    }

    size_t slash = path.find_last_of(L"\\/");
    if (slash != std::wstring::npos) {
        int result = SHCreateDirectoryExW(nullptr,
                                          path.substr(0, slash).c_str(),
                                          nullptr);
        if (result != ERROR_SUCCESS && result != ERROR_ALREADY_EXISTS &&
            result != ERROR_FILE_EXISTS) {
            Wh_Log(L"Failed to create folder (%d): %s", result, path.c_str());
            return;
        }
    }

    Wh_Log(L"Downloading %s", url.c_str());

    std::wstring partPath = path + L".part";
    HRESULT hr = URLDownloadToFileW(nullptr, url.c_str(), partPath.c_str(), 0,
                                    nullptr);
    if (FAILED(hr)) {
        Wh_Log(L"Download failed (0x%08X): %s", (unsigned)hr, url.c_str());
        DeleteFileW(partPath.c_str());
        return;
    }

    if (!MoveFileExW(partPath.c_str(), path.c_str(),
                     MOVEFILE_REPLACE_EXISTING)) {
        Wh_Log(L"Failed to save download (%u): %s", GetLastError(),
               path.c_str());
        DeleteFileW(partPath.c_str());
        return;
    }

    Wh_Log(L"Saved %s", path.c_str());
}

void FitSize(UINT srcWidth,
             UINT srcHeight,
             int screenWidth,
             int screenHeight,
             int* width,
             int* height) {
    double percent = g_settings.scalePercent / 100.0;
    double scaleX = (double)screenWidth / srcWidth;
    double scaleY = (double)screenHeight / srcHeight;

    switch (g_settings.fitMode) {
        case FitMode::stretch:
            break;
        case FitMode::cover:
            scaleX = scaleY = std::max(scaleX, scaleY);
            break;
        case FitMode::contain:
            scaleX = scaleY = std::min(scaleX, scaleY);
            break;
    }

    *width = std::max(1, (int)std::lround(srcWidth * scaleX * percent));
    *height = std::max(1, (int)std::lround(srcHeight * scaleY * percent));
}

std::vector<Frame> LoadFrames(int screenWidth, int screenHeight) {
    std::vector<Frame> frames;
    const std::wstring& path = g_settings.imagePath;
    int defaultDelayMs = std::max(1, 1000 / g_settings.fps);

    DWORD attributes = GetFileAttributesW(path.c_str());
    if (attributes == INVALID_FILE_ATTRIBUTES) {
        Wh_Log(L"Image not found: %s", path.c_str());
        return frames;
    }

    if (attributes & FILE_ATTRIBUTE_DIRECTORY) {
        std::vector<std::wstring> files;
        WIN32_FIND_DATAW findData;
        HANDLE find =
            FindFirstFileW((path + L"\\*.png").c_str(), &findData);
        if (find != INVALID_HANDLE_VALUE) {
            do {
                if (!(findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
                    files.push_back(path + L"\\" + findData.cFileName);
                }
            } while (FindNextFileW(find, &findData));
            FindClose(find);
        }

        std::sort(files.begin(), files.end(),
                  [](const std::wstring& a, const std::wstring& b) {
                      return CompareStringOrdinal(a.c_str(), -1, b.c_str(), -1,
                                                  TRUE) == CSTR_LESS_THAN;
                  });

        int width = 0;
        int height = 0;
        for (const auto& file : files) {
            Gdiplus::Bitmap source(file.c_str());
            if (source.GetLastStatus() != Gdiplus::Ok) {
                Wh_Log(L"Failed to load frame: %s", file.c_str());
                continue;
            }

            if (frames.empty()) {
                FitSize(source.GetWidth(), source.GetHeight(), screenWidth,
                        screenHeight, &width, &height);
            }

            auto bitmap = CopyScaled(&source, width, height);
            if (bitmap) {
                frames.push_back({std::move(bitmap), defaultDelayMs});
            }
        }

        return frames;
    }

    Gdiplus::Bitmap source(path.c_str());
    if (source.GetLastStatus() != Gdiplus::Ok) {
        Wh_Log(L"Failed to load image: %s", path.c_str());
        return frames;
    }

    int width;
    int height;
    FitSize(source.GetWidth(), source.GetHeight(), screenWidth, screenHeight,
            &width, &height);

    UINT frameCount = source.GetFrameCount(&kFrameDimensionTime);
    if (frameCount == 0) {
        frameCount = 1;
    }

    std::vector<UINT> delays;
    UINT propertySize = source.GetPropertyItemSize(kPropertyTagFrameDelay);
    if (propertySize > 0) {
        std::vector<BYTE> buffer(propertySize);
        auto* item = (Gdiplus::PropertyItem*)buffer.data();
        if (source.GetPropertyItem(kPropertyTagFrameDelay, propertySize,
                                   item) == Gdiplus::Ok) {
            UINT count = item->length / sizeof(UINT);
            auto* values = (UINT*)item->value;
            delays.assign(values, values + count);
        }
    }

    for (UINT i = 0; i < frameCount; i++) {
        if (frameCount > 1) {
            source.SelectActiveFrame(&kFrameDimensionTime, i);
        }

        auto bitmap = CopyScaled(&source, width, height);
        if (!bitmap) {
            continue;
        }

        int delayMs = defaultDelayMs;
        if (i < delays.size() && delays[i] > 0) {
            delayMs = std::max(20, (int)delays[i] * 10);
        }

        frames.push_back({std::move(bitmap), delayMs});
    }

    return frames;
}

bool OpenSound() {
    if (g_settings.soundPath.empty()) {
        return false;
    }

    std::wstring command = L"open \"" + g_settings.soundPath +
                           L"\" type mpegvideo alias " + kMciAlias;
    MCIERROR error = mciSendStringW(command.c_str(), nullptr, 0, nullptr);
    if (error) {
        Wh_Log(L"Failed to open sound (%u): %s", error,
               g_settings.soundPath.c_str());
        return false;
    }

    command = std::wstring(L"setaudio ") + kMciAlias + L" volume to " +
              std::to_wstring(g_settings.volume * 10);
    mciSendStringW(command.c_str(), nullptr, 0, nullptr);
    return true;
}

void StartSound() {
    std::wstring command = std::wstring(L"play ") + kMciAlias;
    mciSendStringW(command.c_str(), nullptr, 0, nullptr);
}

void CloseSound() {
    std::wstring command = std::wstring(L"close ") + kMciAlias;
    mciSendStringW(command.c_str(), nullptr, 0, nullptr);
}

LRESULT CALLBACK WindowProc(HWND hwnd,
                            UINT message,
                            WPARAM wParam,
                            LPARAM lParam) {
    switch (message) {
        case WM_NCHITTEST:
            return HTTRANSPARENT;
        case WM_MOUSEACTIVATE:
            return MA_NOACTIVATE;
    }

    return DefWindowProcW(hwnd, message, wParam, lParam);
}

bool RegisterWindowClass() {
    WNDCLASSEXW windowClass{
        .cbSize = sizeof(WNDCLASSEXW),
        .lpfnWndProc = WindowProc,
        .hInstance = GetModuleHandleW(nullptr),
        .hCursor = LoadCursorW(nullptr, IDC_ARROW),
        .lpszClassName = kWindowClass,
    };

    return RegisterClassExW(&windowClass) ||
           GetLastError() == ERROR_CLASS_ALREADY_EXISTS;
}

void ForceForeground(HWND hwnd) {
    HWND foreground = GetForegroundWindow();
    DWORD foregroundThread =
        foreground ? GetWindowThreadProcessId(foreground, nullptr) : 0;
    DWORD currentThread = GetCurrentThreadId();

    bool attached = foregroundThread && foregroundThread != currentThread &&
                    AttachThreadInput(currentThread, foregroundThread, TRUE);

    SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0,
                 SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
    BringWindowToTop(hwnd);
    SetForegroundWindow(hwnd);

    if (attached) {
        AttachThreadInput(currentThread, foregroundThread, FALSE);
    }
}

void PumpMessages() {
    MSG msg;
    while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
}

void RunJumpscare(std::mt19937_64& rng) {
    Wh_Log(L"Jumpscare!");

    HWND previousForeground = GetForegroundWindow();
    HMONITOR monitor =
        MonitorFromWindow(previousForeground, MONITOR_DEFAULTTOPRIMARY);
    MONITORINFO monitorInfo{.cbSize = sizeof(MONITORINFO)};
    if (!GetMonitorInfoW(monitor, &monitorInfo)) {
        return;
    }

    RECT rc = monitorInfo.rcMonitor;
    int screenWidth = rc.right - rc.left;
    int screenHeight = rc.bottom - rc.top;

    std::vector<Frame> frames = LoadFrames(screenWidth, screenHeight);
    if (frames.empty()) {
        return;
    }

    int animationMs = 0;
    for (const auto& frame : frames) {
        animationMs += frame.delayMs;
    }

    int totalMs = g_settings.durationMs;
    if (totalMs == 0) {
        totalMs = std::max(animationMs, 800);
    }

    if (!RegisterWindowClass()) {
        Wh_Log(L"RegisterClassEx failed");
        return;
    }

    DWORD exStyle = WS_EX_TOPMOST | WS_EX_LAYERED | WS_EX_TRANSPARENT |
                    WS_EX_TOOLWINDOW;
    if (!g_settings.forceForeground) {
        exStyle |= WS_EX_NOACTIVATE;
    }

    HWND hwnd = CreateWindowExW(exStyle, kWindowClass, L"", WS_POPUP, rc.left,
                                rc.top, screenWidth, screenHeight, nullptr,
                                nullptr, GetModuleHandleW(nullptr), nullptr);
    if (!hwnd) {
        Wh_Log(L"CreateWindowEx failed");
        return;
    }

    HDC screenDc = GetDC(nullptr);
    HDC memoryDc = CreateCompatibleDC(screenDc);

    BITMAPINFO bitmapInfo{};
    bitmapInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bitmapInfo.bmiHeader.biWidth = screenWidth;
    bitmapInfo.bmiHeader.biHeight = -screenHeight;
    bitmapInfo.bmiHeader.biPlanes = 1;
    bitmapInfo.bmiHeader.biBitCount = 32;
    bitmapInfo.bmiHeader.biCompression = BI_RGB;

    void* bits = nullptr;
    HBITMAP dib = CreateDIBSection(screenDc, &bitmapInfo, DIB_RGB_COLORS,
                                   &bits, nullptr, 0);
    if (!dib) {
        Wh_Log(L"CreateDIBSection failed");
        DeleteDC(memoryDc);
        ReleaseDC(nullptr, screenDc);
        DestroyWindow(hwnd);
        return;
    }

    HGDIOBJ oldBitmap = SelectObject(memoryDc, dib);

    bool soundOpened = OpenSound();

    {
        Gdiplus::Bitmap canvas(screenWidth, screenHeight, screenWidth * 4,
                               PixelFormat32bppPARGB, (BYTE*)bits);
        Gdiplus::Graphics graphics(&canvas);
        graphics.SetInterpolationMode(Gdiplus::InterpolationModeBilinear);
        graphics.SetCompositingQuality(Gdiplus::CompositingQualityHighSpeed);

        BLENDFUNCTION blend{
            .BlendOp = AC_SRC_OVER,
            .SourceConstantAlpha = 255,
            .AlphaFormat = AC_SRC_ALPHA,
        };
        POINT windowPos{rc.left, rc.top};
        POINT sourcePos{0, 0};
        SIZE windowSize{screenWidth, screenHeight};

        std::uniform_real_distribution<double> shakeDistribution(-1.0, 1.0);
        const int zoomMs = 140;
        const double maxShake = screenHeight * 0.025;

        LARGE_INTEGER frequency;
        LARGE_INTEGER startCounter;
        QueryPerformanceFrequency(&frequency);
        QueryPerformanceCounter(&startCounter);

        bool shown = false;
        while (!IsStopRequested()) {
            LARGE_INTEGER now;
            QueryPerformanceCounter(&now);
            int elapsedMs = (int)((now.QuadPart - startCounter.QuadPart) *
                                  1000 / frequency.QuadPart);
            if (elapsedMs >= totalMs) {
                break;
            }

            size_t frameIndex = frames.size() - 1;
            int frameTime = elapsedMs;
            for (size_t i = 0; i < frames.size(); i++) {
                if (frameTime < frames[i].delayMs) {
                    frameIndex = i;
                    break;
                }
                frameTime -= frames[i].delayMs;
            }

            Gdiplus::Bitmap* frameBitmap = frames[frameIndex].bitmap.get();
            double scale = 1.0;
            if (g_settings.zoomIn && elapsedMs < zoomMs) {
                double t = (double)elapsedMs / zoomMs;
                scale = 0.45 + 0.55 * (1.0 - (1.0 - t) * (1.0 - t));
            }

            int width = (int)std::lround(frameBitmap->GetWidth() * scale);
            int height = (int)std::lround(frameBitmap->GetHeight() * scale);
            int x = (screenWidth - width) / 2;
            int y = (screenHeight - height) / 2;

            if (g_settings.shake) {
                double decay = 1.0 - 0.6 * elapsedMs / totalMs;
                x += (int)std::lround(shakeDistribution(rng) * maxShake * decay);
                y += (int)std::lround(shakeDistribution(rng) * maxShake * decay);
            }

            graphics.Clear(Gdiplus::Color(0, 0, 0, 0));
            graphics.DrawImage(frameBitmap, Gdiplus::Rect(x, y, width, height));
            graphics.Flush(Gdiplus::FlushIntentionSync);

            UpdateLayeredWindow(hwnd, screenDc, &windowPos, &windowSize,
                                memoryDc, &sourcePos, 0, &blend, ULW_ALPHA);

            if (!shown) {
                if (g_settings.forceForeground) {
                    ForceForeground(hwnd);
                } else {
                    ShowWindow(hwnd, SW_SHOWNOACTIVATE);
                }

                if (soundOpened) {
                    StartSound();
                }

                shown = true;
            } else {
                SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0,
                             SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
            }

            PumpMessages();
            MsgWaitForMultipleObjects(1, &g_stopEvent, FALSE, 15, QS_ALLINPUT);
        }
    }

    if (soundOpened) {
        CloseSound();
    }

    SelectObject(memoryDc, oldBitmap);
    DeleteObject(dib);
    DeleteDC(memoryDc);
    ReleaseDC(nullptr, screenDc);
    DestroyWindow(hwnd);
    PumpMessages();

    if (g_settings.forceForeground && previousForeground &&
        IsWindow(previousForeground)) {
        SetForegroundWindow(previousForeground);
    }
}

DWORD WINAPI WorkerThread(LPVOID) {
    SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

    Gdiplus::GdiplusStartupInput startupInput;
    ULONG_PTR gdiplusToken;
    if (Gdiplus::GdiplusStartup(&gdiplusToken, &startupInput, nullptr) !=
        Gdiplus::Ok) {
        Wh_Log(L"GdiplusStartup failed");
        return 1;
    }

    HRESULT comResult = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);

    if (g_settings.autoDownload) {
        DownloadIfMissing(g_settings.imagePath, g_settings.imageUrl);
        DownloadIfMissing(g_settings.soundPath, g_settings.soundUrl);
    }

    std::random_device randomDevice;
    LARGE_INTEGER counter;
    QueryPerformanceCounter(&counter);
    std::seed_seq seed{randomDevice(), randomDevice(),
                       (unsigned)counter.LowPart, (unsigned)counter.HighPart};
    std::mt19937_64 rng(seed);
    std::uniform_int_distribution<int> roll(1, g_settings.chance);

    bool hotkeyRegistered = false;
    UINT modifiers;
    UINT virtualKey;
    if (ParseHotkey(g_settings.testHotkey, &modifiers, &virtualKey)) {
        hotkeyRegistered = RegisterHotKey(nullptr, kTestHotkeyId,
                                          modifiers | MOD_NOREPEAT, virtualKey);
        if (!hotkeyRegistered) {
            Wh_Log(L"RegisterHotKey failed (%u): %s", GetLastError(),
                   g_settings.testHotkey.c_str());
        }
    } else if (!g_settings.testHotkey.empty()) {
        Wh_Log(L"Invalid hotkey: %s", g_settings.testHotkey.c_str());
    }

    if (g_settings.testOnApply) {
        RunJumpscare(rng);
    }

    ULONGLONG nextCheck = GetTickCount64() + g_settings.checkIntervalMs;
    while (true) {
        ULONGLONG now = GetTickCount64();
        if (now >= nextCheck) {
            if (roll(rng) == 1) {
                RunJumpscare(rng);
            }
            nextCheck = GetTickCount64() + g_settings.checkIntervalMs;
            continue;
        }

        DWORD result = MsgWaitForMultipleObjects(
            1, &g_stopEvent, FALSE, (DWORD)(nextCheck - now), QS_ALLINPUT);
        if (result == WAIT_OBJECT_0) {
            break;
        }

        bool testRequested = false;
        MSG msg;
        while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_HOTKEY && msg.wParam == kTestHotkeyId) {
                testRequested = true;
                continue;
            }
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }

        if (testRequested) {
            RunJumpscare(rng);
        }
    }

    if (hotkeyRegistered) {
        UnregisterHotKey(nullptr, kTestHotkeyId);
    }

    UnregisterClassW(kWindowClass, GetModuleHandleW(nullptr));
    if (SUCCEEDED(comResult)) {
        CoUninitialize();
    }
    Gdiplus::GdiplusShutdown(gdiplusToken);
    return 0;
}

void StartWorker() {
    ResetEvent(g_stopEvent);
    g_thread = CreateThread(nullptr, 0, WorkerThread, nullptr, 0, nullptr);
    if (!g_thread) {
        Wh_Log(L"CreateThread failed");
    }
}

void StopWorker() {
    if (!g_thread) {
        return;
    }

    SetEvent(g_stopEvent);
    WaitForSingleObject(g_thread, INFINITE);
    CloseHandle(g_thread);
    g_thread = nullptr;
}

}  // namespace

BOOL WhTool_ModInit() {
    LoadSettings();

    g_stopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_stopEvent) {
        Wh_Log(L"CreateEvent failed");
        return FALSE;
    }

    StartWorker();
    return TRUE;
}

void WhTool_ModSettingsChanged() {
    StopWorker();
    LoadSettings();
    StartWorker();
}

void WhTool_ModUninit() {
    StopWorker();
    CloseHandle(g_stopEvent);
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
