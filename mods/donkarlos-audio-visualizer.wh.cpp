// ==WindhawkMod==
// @id donkarlos-audio-visualizer
// @name DonKarlos Audio Visualizer
// @description Turn system audio into animated bars or a waveform on the desktop, taskbar, or both
// @description:de-DE Zeigt Systemaudio als animierte Balken oder Wellenform auf Desktop, Taskleiste oder beiden
// @version 1.1.2
// @author DonKarlos
// @github https://github.com/Karlos5000
// @license MIT
// @include windhawk.exe
// @compilerOptions -lole32 -luuid -lgdi32 -luser32 -lshell32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# DonKarlos Audio Visualizer

Both panels share one capture stream and stay in sync. The four controls for style, color, sensitivity and bar count apply to both panels.

See your system audio on the Windows desktop, on the primary taskbar, or in both places at once. Choose frequency bars, mirrored bars, a waveform, or segmented LEDs. Each pattern works with RGB, green, blue, or white.

![Four patterns and the resting bars during silence](https://raw.githubusercontent.com/Karlos5000/windhawk-mods/48a79046d3e5157417686fca6b667740ebbf8ab0/previews/donkarlos-audio-visualizer.png)

*Renderer captures with a test signal. The last panel shows silence.*

Set 8–64 bars, sensitivity from 10–400%, and transparency from 0–100%. In silence the bars settle to a short row and the waveform becomes a flat line. Capture follows the default Windows multimedia output via WASAPI loopback. Boom 3D works when its audio reaches that endpoint.

## Deutsch

Vier Muster zeigen die laufende Audioausgabe auf Desktop und Taskleiste. Bei Stille bleiben kurze Balken stehen. Die Wellenform ruht auf der Mittellinie. Farbe, Empfindlichkeit und Transparenz lassen sich in den Mod-Einstellungen ändern.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- display: both
  $name: Show on
  $name:de-DE: Anzeige
  $options:
    - both: Desktop and taskbar
    - desktop: Desktop
    - taskbar: Taskbar
  $options:de-DE:
    - both: Beide
    - desktop: Desktop
    - taskbar: Taskleiste
- pattern: bars
  $name: Pattern
  $name:de-DE: Muster
  $options:
    - bars: Frequency bars
    - mirror: Mirrored
    - wave: Waveform
    - led: LED
  $options:de-DE:
    - bars: Frequenzbalken
    - mirror: Gespiegelt
    - wave: Wellenform
    - led: LED
- palette: rgb
  $name: Palette
  $name:de-DE: Farbschema
  $options:
    - rgb: RGB
    - green: Green
    - blue: Blue
    - white: White
  $options:de-DE:
    - rgb: RGB
    - green: Grün
    - blue: Blau
    - white: Weiß
- bars: 32
  $name: Bar count (8–64)
  $name:de-DE: Balkenanzahl (8–64)
- sensitivity: 150
  $name: Sensitivity (10–400 percent)
  $name:de-DE: Empfindlichkeit (10–400 Prozent)
- transparency: 20
  $name: Transparency (0–100 percent)
  $name:de-DE: Transparenz (0–100 Prozent)
*/
// ==/WindhawkModSettings==

#define NOMINMAX
#include <windows.h>
#include <complex>
#include <shellapi.h>
#include <windhawk_api.h>
#include <audioclient.h>
#include <mmdeviceapi.h>
#include <wrl/client.h>
#include <algorithm>
#include <array>
#include <atomic>
#include <cmath>

#include <string>

using Microsoft::WRL::ComPtr;
using std::clamp;
constexpr int N = 4096;
constexpr double pi = 3.141592653589793;
static std::atomic_bool changed{true}, stopping{false};
static std::wstring pattern, palette, display;
static int count, gain, transparency;
static std::array<float, N> samples{};
static std::array<float, 64> levels{};
static int head;
static unsigned rate;
static HWND panels[2]{};
static constexpr wchar_t className[] = L"WindhawkAudioVisualizerPanel";
static std::wstring setting(PCWSTR name) {
    PCWSTR value = Wh_GetStringSetting(name);
    std::wstring result(value);
    Wh_FreeStringSetting(value);
    return result;
}
static void settings() {
    pattern = setting(L"pattern");
    palette = setting(L"palette");
    display = setting(L"display");
    count = clamp(Wh_GetIntSetting(L"bars"), 8, 64);
    gain = clamp(Wh_GetIntSetting(L"sensitivity"), 10, 400);
    transparency = clamp(Wh_GetIntSetting(L"transparency"), 0, 100);
}

struct Capture {
    std::array<float,N> samples{};
    int head{};
    unsigned rate{};
    ComPtr<IMMDeviceEnumerator> devices;
    ComPtr<IAudioClient> client;
    ComPtr<IAudioCaptureClient> reader;
    std::wstring id;
    WORD channels{};
    HRESULT connect() {
        if (!devices) {
            HRESULT hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr,
                                          CLSCTX_ALL, IID_PPV_ARGS(&devices));
            if (FAILED(hr))
                return hr;
        }
        ComPtr<IMMDevice> endpoint;
        HRESULT hr =
            devices->GetDefaultAudioEndpoint(eRender, eMultimedia, &endpoint);
        if (FAILED(hr))
            return hr;
        LPWSTR name{};
        if (FAILED(hr = endpoint->GetId(&name)))
            return hr;
        bool same = id == name;
        id = name;
        CoTaskMemFree(name);
        if (reader && same)
            return S_OK;
        close();
        if (FAILED(hr = endpoint->Activate(
                       __uuidof(IAudioClient), CLSCTX_ALL, nullptr,
                       reinterpret_cast<void**>(client.GetAddressOf()))))
            return hr;
        WAVEFORMATEX* mix{};
        if (FAILED(hr = client->GetMixFormat(&mix)))
            return hr;
        rate = mix->nSamplesPerSec;
        channels = mix->nChannels;
        WAVEFORMATEX fmt{WAVE_FORMAT_IEEE_FLOAT,
                         channels,
                         rate,
                         rate * channels * 4,
                         static_cast<WORD>(channels * 4),
                         32,
                         0};
        CoTaskMemFree(mix);
        if (FAILED(
                hr = client->Initialize(AUDCLNT_SHAREMODE_SHARED,
                                        AUDCLNT_STREAMFLAGS_LOOPBACK |
                                            AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                                        1000000, 0, &fmt, nullptr)))
            return hr;
        if (FAILED(hr = client->GetService(IID_PPV_ARGS(&reader))))
            return hr;
        return client->Start();
    }
    void close() {
        if (client)
            client->Stop();
        reader.Reset();
        client.Reset();
        samples.fill(0);
        head = 0;
    }
    HRESULT drain(bool& fresh) {
        UINT32 size{};
        HRESULT hr;
        while (SUCCEEDED(hr = reader->GetNextPacketSize(&size)) && size) {
            BYTE* bytes{};
            DWORD flags{};
            if (FAILED(hr = reader->GetBuffer(&bytes, &size, &flags, nullptr,
                                              nullptr)))
                break;
            const float* data = reinterpret_cast<float*>(bytes);
            for (UINT32 i = 0; i < size; ++i) {
                float sum = 0;
                if (!(flags & AUDCLNT_BUFFERFLAGS_SILENT))
                    for (unsigned ch = 0; ch < channels; ++ch)
                        sum += data[i * channels + ch];
                samples[head] = sum / channels;
                head = (head + 1) % N;
            }
            if (FAILED(hr = reader->ReleaseBuffer(size)))
                break;
            fresh = true;
        }
        return hr;
    }
};

static void analyze(bool active) {
    std::array<std::complex<float>, N> bins{};
    if (active) {
        static const auto window = [] {
            std::array<float, N> w{};
            for (int i = 0; i < N; ++i)
                w[i] = 0.5f - 0.5f * std::cos(2 * pi * i / (N - 1));
            return w;
        }();
        for (int i = 0, j = 0; i < N; ++i) {
            bins[j] = samples[(head + i) % N] * window[i];
            int bit = N / 2;
            while (bit && (j & bit)) {
                j ^= bit;
                bit >>= 1;
            }
            j ^= bit;
        }
        for (int len = 2; len <= N; len *= 2) {
            auto step = std::polar(1.f, float(-2 * pi / len));
            for (int begin = 0; begin < N; begin += len) {
                std::complex<float> phase = 1.f;
                for (int j = 0; j < len / 2; ++j, phase *= step) {
                    auto a = bins[begin + j],
                         b = phase * bins[begin + j + len / 2];
                    bins[begin + j] = a + b;
                    bins[begin + j + len / 2] = a - b;
                }
            }
        }
    }
    int first = 1;
    for (int i = 0; i < count; ++i) {
        float peak = 0;
        if (active) {
            double upper = std::min(18000., rate * 0.5);
            int end =
                clamp(int(40 * std::pow(upper / 40., double(i + 1) / count) *
                          N / rate),
                      first + 1, N / 2 + 1);
            for (int k = first; k < end; ++k)
                peak = std::max(peak, std::abs(bins[k]));
            first = end;
        }
        float target = clamp(
            float(
                (20 * std::log10(std::max(
                          1e-8f, peak * 4 / N * std::max(1.f, gain / 100.f))) +
                 60) /
                60) *
                std::min(gain, 100) / 65,
            0.f, 1.f);
        levels[i] +=
            (target - levels[i]) * (target > levels[i] ? 0.58f : 0.13f);
    }
}
static COLORREF color(int i, int total) {
    if (palette == L"white")
        return RGB(255, 255, 255);
    if (palette == L"blue")
        return RGB(50, 160, 255);
    if (palette == L"green")
        return RGB(32, 245, 115);
    auto component = [&](int offset) {
        return int(
            255 *
            clamp(std::abs(std::fmod(6. * i / total + offset, 6.) - 3.) - 1.,
                  0., 1.));
    };
    return RGB(component(0), component(4), component(2));
}
static LRESULT CALLBACK paint(HWND w, UINT msg, WPARAM wp, LPARAM lp) {
    if (msg == WM_NCHITTEST)
        return HTTRANSPARENT;
    if (msg == WM_ERASEBKGND)
        return 1;
    if (msg != WM_PAINT)
        return DefWindowProcW(w, msg, wp, lp);
    PAINTSTRUCT ps;
    HDC out = BeginPaint(w, &ps);
    RECT r;
    GetClientRect(w, &r);
    int width = r.right, height = r.bottom;
    HDC dc = CreateCompatibleDC(out);
    HBITMAP bmp = CreateCompatibleBitmap(out, width, height);
    auto old = SelectObject(dc, bmp);
    FillRect(dc, &r, (HBRUSH)GetStockObject(BLACK_BRUSH));
    if (pattern == L"wave") {
        SelectObject(dc, GetStockObject(DC_PEN));
        for (int x = 0; x < width; ++x) {
            int pos = (head + N - 512 + 512 * x / width) % N;
            int y = int((1 - clamp(samples[pos] * gain / 50, -1.f, 1.f)) *
                        (height - 1) / 2);
            SetDCPenColor(dc, color(x, width));
            if (x == 0)
                MoveToEx(dc, x, y, nullptr);
            else
                LineTo(dc, x, y);
        }
    } else
        for (int i = 0; i < count; ++i) {
            const int rest = std::min(height, std::max(2, height / 18));
            int h = rest + int(levels[i] * (height - rest));
            RECT b{i * width / count, height - h,
                   std::max(i * width / count + 1, (i + 1) * width / count - 1),
                   height};
            SetDCBrushColor(dc, color(i, count));
            auto brush = (HBRUSH)GetStockObject(DC_BRUSH);
            if (pattern == L"mirror") {
                b.top = (height - h) / 2;
                b.bottom = b.top + h;
            }
            if (pattern == L"led") {
                for (int y = height; y > b.top; y -= 5) {
                    RECT cell{b.left, std::max<LONG>(b.top, y - 3), b.right, y};
                    FillRect(dc, &cell, brush);
                }
            } else
                FillRect(dc, &b, brush);
        }
    BitBlt(out, 0, 0, width, height, dc, 0, 0, SRCCOPY);
    SelectObject(dc, old);
    DeleteObject(bmp);
    DeleteDC(dc);
    EndPaint(w, &ps);
    return 0;
}

static HWND desktopHost() {
    HWND shell = GetShellWindow();
    if (!shell)
        return nullptr;
    DWORD shellPid{};
    GetWindowThreadProcessId(shell, &shellPid);
    auto host = [&](HWND window) -> HWND {
        DWORD pid{};
        GetWindowThreadProcessId(window, &pid);
        if (pid != shellPid || !IsWindowVisible(window))
            return nullptr;
        HWND view =
            FindWindowExW(window, nullptr, L"SHELLDLL_DefView", nullptr);
        if (!view)
            return nullptr;
        HWND icons = FindWindowExW(view, nullptr, L"SysListView32", nullptr);
        if (IsWindowVisible(icons))
            return icons;
        return IsWindowVisible(view) ? view : window;
    };
    if (HWND h = host(shell))
        return h;
    for (HWND w = nullptr;
         (w = FindWindowExW(nullptr, w, L"WorkerW", nullptr));)
        if (HWND h = host(w))
            return h;
    return nullptr;
}
static void layout(int index) {
    bool task = index == 1;
    HWND host = task ? FindWindowW(L"Shell_TrayWnd", nullptr) : desktopHost();
    HWND& panel = panels[index];
    bool enabled =
        host && (task ? display != L"desktop" : display != L"taskbar");
    if (panel &&
        (!enabled || !IsWindow(panel) || (GetParent(panel) != host))) {
        if (IsWindow(panel))
            DestroyWindow(panel);
        panel = nullptr;
    }
    if (!enabled)
        return;
    MONITORINFO mi{sizeof(mi)};
    if (!GetMonitorInfoW(MonitorFromPoint({0, 0}, MONITOR_DEFAULTTOPRIMARY),
                         &mi))
        return;
    RECT area = mi.rcWork;
    if (task && !GetWindowRect(host, &area))
        return;
    UINT dpi = GetDpiForWindow(host);
    auto scale = [&](int n) { return MulDiv(n, dpi ? dpi : 96, 96); };
    int w = std::min<int>(scale(task ? 224 : 480), area.right - area.left);
    int h = std::min<int>(scale(task ? 28 : 100), area.bottom - area.top);
    POINT pos{area.left +
                  (task ? std::max(0, int((area.right - area.left) / 4 - w / 2))
                        : scale(24)),
              task ? area.top + (area.bottom - area.top - h) / 2
                   : area.bottom - h - scale(90)};
    ScreenToClient(host, &pos);
    if (!panel)
        panel = CreateWindowExW(
            WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_NOACTIVATE |
                WS_EX_NOPARENTNOTIFY,
            className, L"", WS_CHILD, pos.x, pos.y, w, h,
            host, nullptr, GetModuleHandleW(nullptr), nullptr);
    if (!panel)
        return;
    BYTE alpha = BYTE(255 * (100 - transparency) / 100), oldAlpha{};
    DWORD flags{};
    if (!GetLayeredWindowAttributes(panel, nullptr, &oldAlpha, &flags) || flags != (LWA_COLORKEY | LWA_ALPHA) ||
        oldAlpha != alpha)
        SetLayeredWindowAttributes(panel, 0, alpha, LWA_COLORKEY | LWA_ALPHA);
    RECT current{};
    GetWindowRect(panel, &current);
    POINT screen = pos;
    if (!task)
        ClientToScreen(host, &screen);
    if ((task && GetTopWindow(host) != panel) || !IsWindowVisible(panel) || current.left != screen.x ||
        current.top != screen.y || current.right - current.left != w ||
        current.bottom - current.top != h)
        SetWindowPos(panel, HWND_TOP, pos.x, pos.y, w, h,
                     SWP_NOACTIVATE | SWP_SHOWWINDOW);
}

static HANDLE stopEvent, audioEvent, uiThread, audioThread;
static SRWLOCK sampleLock = SRWLOCK_INIT;
static std::array<float, N> published{};
static int publishedHead;
static unsigned publishedRate;
static ULONGLONG publishedAt;

static DWORD WINAPI captureLoop(void*) {
    if (FAILED(CoInitializeEx(nullptr, COINIT_MULTITHREADED)))
        return 0;
    {
        Capture capture;
        ULONGLONG nextConnect = 0;
        bool wasActive = false;
        while (!stopping) {
            auto now = GetTickCount64();
            if (now >= nextConnect) {
                if (FAILED(capture.connect()))
                    capture.close();
                nextConnect = now + 2000;
            }
            bool fresh = false;
            if (capture.reader && FAILED(capture.drain(fresh)))
                capture.close();
            if (fresh) {
                bool active = std::any_of(
                    capture.samples.begin(), capture.samples.end(),
                    [](float v) { return std::abs(v) > 0.000001f; });
                if (active || wasActive) {
                    AcquireSRWLockExclusive(&sampleLock);
                    published = capture.samples;
                    publishedHead = capture.head;
                    publishedRate = capture.rate;
                    publishedAt = active ? now : 0;
                    ReleaseSRWLockExclusive(&sampleLock);
                    SetEvent(audioEvent);
                }
                wasActive = active;
            }
            if (WaitForSingleObject(stopEvent, fresh ? 16 : 50) ==
                WAIT_OBJECT_0)
                break;
        }
        capture.close();
    }
    CoUninitialize();
    return 0;
}

static DWORD WINAPI visualizerLoop(void*) {

    WNDCLASSW wc{};
    wc.hInstance = GetModuleHandleW(nullptr);
    wc.lpfnWndProc = paint;
    wc.lpszClassName = className;
    if (!RegisterClassW(&wc))
        return 0;
    ULONGLONG nextLayout = 0, nextFrame = 0;
    bool moving = true;
    HANDLE events[] = {stopEvent, audioEvent};
    while (!stopping) {
        MSG msg;
        while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE))
            DispatchMessageW(&msg);
        auto now = GetTickCount64();
        bool dirty = changed.exchange(false);
        if (dirty) {
            settings();
            nextLayout = 0;
        }
        if (now >= nextLayout) {
            layout(0);
            layout(1);
            nextLayout = now + 1000;
        }
        if (now >= nextFrame || dirty) {
            AcquireSRWLockShared(&sampleLock);
            bool active = publishedAt && now < publishedAt + 120;
            if (active) {
                samples = published;
                head = publishedHead;
                rate = publishedRate;
            }
            ReleaseSRWLockShared(&sampleLock);
            if (!active)
                samples.fill(0);
            if (active || moving || dirty) {
                if (pattern != L"wave")
                    analyze(active);
                moving = active ||
                         (pattern != L"wave" &&
                          *std::max_element(levels.begin(),
                                            levels.begin() + count) > 0.001f);
                if (!moving)
                    levels.fill(0);
                for (HWND panel : panels)
                    if (panel)
                        InvalidateRect(panel, nullptr, FALSE);
            }
            nextFrame = now + 16;
        }
        DWORD wait = moving ? DWORD(std::min<ULONGLONG>(
                                  16, nextFrame > now ? nextFrame - now : 0))
                            : 200;
        MsgWaitForMultipleObjects(2, events, FALSE, wait, QS_ALLINPUT);
    }
    for (HWND panel : panels)
        if (panel)
            DestroyWindow(panel);
    UnregisterClassW(className, wc.hInstance);
    return 0;
}
void WhTool_ModSettingsChanged() {
    changed = true;
    if (audioEvent)
        SetEvent(audioEvent);
}
void WhTool_ModUninit() {
    stopping = true;
    if (stopEvent)
        SetEvent(stopEvent);
    for (HANDLE thread : {uiThread, audioThread})
        if (thread) {
            WaitForSingleObject(thread, INFINITE);
            CloseHandle(thread);
        }
    if (stopEvent)
        CloseHandle(stopEvent);
    if (audioEvent)
        CloseHandle(audioEvent);
}
BOOL WhTool_ModInit() {
    stopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    audioEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    if (stopEvent && audioEvent) {
        uiThread =
            CreateThread(nullptr, 0, visualizerLoop, nullptr, 0, nullptr);
        audioThread =
            CreateThread(nullptr, 0, captureLoop, nullptr, 0, nullptr);
    }
    if (uiThread && audioThread)
        return TRUE;
    WhTool_ModUninit();
    return FALSE;
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
