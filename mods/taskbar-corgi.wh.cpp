// ==WindhawkMod==
// @id              taskbar-corgi
// @name            Taskbar Corgi
// @description     A little pixel corgi lives on top of your taskbar: it walks around, sits, naps and loves being petted
// @version         1.0
// @author          Dimitris Katsimpris
// @github          https://github.com/dimitriskatsimpris
// @include         explorer.exe
// @compilerOptions -lgdi32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Taskbar Corgi

A small pixel-art corgi that lives on top of your taskbar.

- It **walks** back and forth, **sits** down for a while, and sometimes takes
  a **nap** (with little floating z's).
- Optionally, it **chases the cursor** when your mouse comes close to the
  taskbar (off by default, turn it on in the settings).
- Hover the mouse over the corgi to **pet** it and get a heart. Petting a
  sleeping corgi wakes it up.

The corgi is click-through, so it never blocks anything, and it hides itself
while a fullscreen app or game is in front.

## Settings
- **Size**: pixel size of the corgi (1–8), multiplied by your display scaling.
- **Chase the mouse**: run toward the cursor when it comes near the taskbar.
- **Hide in fullscreen apps**: hide while a fullscreen window is active.
- **Walking speed**: in percent.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- size: 3
  $name: Size
  $description: Pixel size of the corgi (1-8), multiplied by your display scaling
- chaseMouse: false
  $name: Chase the mouse
  $description: Run toward the cursor when it comes close to the taskbar
- hideInFullscreen: true
  $name: Hide in fullscreen apps
- speed: 100
  $name: Walking speed (%)
  $description: 25 to 300
*/
// ==/WindhawkModSettings==

#include <windows.h>

#include <cmath>
#include <cstdint>
#include <cstring>
#include <cwchar>
#include <initializer_list>

namespace {

struct Sprite {
    int w;
    int h;
    const char* const* rows;
};

const char* const kWalk1Rows[] = {
    "................K...K...",
    "...............KPK.KPK..",
    "...............KPOKKPOK.",
    "...............KOOOOOOK.",
    "..............KOOOOOEOK.",
    "..............KOOOOWWWEK",
    "..KKKKKKKKKKKKKOOOWWWWK.",
    ".KOOOOOOOOOOOOOOOOWWKK..",
    "KDOOOOOOOOOOOOOOOWWWK...",
    "KDOOOOOOOOOOOOOOWWWWK...",
    "KWDOOOOOOOOOOOOOWWWWK...",
    ".KWWDDOOOOOOOOOWWWWK....",
    "..KWWKKKKKKKKKKWWKWK....",
    "..KWK.........KWKKWK....",
    "..KWK.........KWK.KWK...",
    "..KK..........KK...KK...",
};
const Sprite kWalk1 = {24, 16, kWalk1Rows};

const char* const kWalk2Rows[] = {
    "................K...K...",
    "...............KPK.KPK..",
    "...............KPOKKPOK.",
    "...............KOOOOOOK.",
    "..............KOOOOOEOK.",
    "..............KOOOOWWWEK",
    "..KKKKKKKKKKKKKOOOWWWWK.",
    ".KOOOOOOOOOOOOOOOOWWKK..",
    "KDOOOOOOOOOOOOOOOWWWK...",
    "KDOOOOOOOOOOOOOOWWWWK...",
    "KWDOOOOOOOOOOOOOWWWWK...",
    ".KWWDDOOOOOOOOOWWWWK....",
    "..KWWKKKKKKKKKKKWWKK....",
    "...KWK.........KWWK.....",
    "...KWK.........KWWK.....",
    "...KK..........KKK......",
};
const Sprite kWalk2 = {24, 16, kWalk2Rows};

const char* const kSitRows[] = {
    "................K...K...",
    "...............KPK.KPK..",
    "...............KPOKKPOK.",
    "...............KOOOOOOK.",
    "..............KOOOOOEOK.",
    "..............KOOOOWWWEK",
    ".........KKKKKKOOOWWWWK.",
    ".......KKOOOOOOOOOWWKK..",
    "......KDOOOOOOOOOWWWK...",
    ".....KDOOOOOOOOOWWWWK...",
    "....KDOOOOOOOOOOWWWWK...",
    "....KDOOOOOOOOOWWWWK....",
    "...KWDDOOOOOOOKWWKWK....",
    "...KWWWDDDOOOOKWKKWK....",
    "...KWWWWWWKKKKKWK.KWK...",
    "....KKKKKKK...KK...KK...",
};
const Sprite kSit = {24, 16, kSitRows};

const char* const kSleep1Rows[] = {
    "........................",
    "........................",
    "........................",
    "........................",
    "........................",
    "........................",
    "................K...K...",
    "...............KPK.KPK..",
    "..KKKKKKKKKKKKKKPOKKPOK.",
    ".KOOOOOOOOOOOOOKOOOOOOK.",
    "KDOOOOOOOOOOOOOOOOOOKKOK",
    "KDOOOOOOOOOOOOOOOOOWWWEK",
    "KWDOOOOOOOOOOOOOOOWWWWK.",
    "KWWDDOOOOOOOOOOWWWWWWK..",
    ".KWWWWKKKKKKKKKWWWWWK...",
    "..KKKK.........KKKKK....",
};
const Sprite kSleep1 = {24, 16, kSleep1Rows};

const char* const kSleep2Rows[] = {
    "........................",
    "........................",
    "........................",
    "........................",
    "........................",
    "........................",
    "........................",
    "................K...K...",
    "..KKKKKKKKKKKKK.KPK.KPK.",
    ".KOOOOOOOOOOOOOKPOKKPOK.",
    "KDOOOOOOOOOOOOOOOOOOKKOK",
    "KDOOOOOOOOOOOOOOOOOWWWEK",
    "KWDOOOOOOOOOOOOOOOWWWWK.",
    "KWWDDOOOOOOOOOOWWWWWWK..",
    ".KWWWWKKKKKKKKKWWWWWK...",
    "..KKKK.........KKKKK....",
};
const Sprite kSleep2 = {24, 16, kSleep2Rows};

const char* const kZRows[] = {
    "LLLL",
    "..L.",
    ".L..",
    "LLLL",
};
const Sprite kZ = {4, 4, kZRows};

const char* const kHeartRows[] = {
    ".KK.KK.",
    "KPPKPPK",
    "KPPPPPK",
    ".KPPPK.",
    "..KPK..",
    "...K...",
};
const Sprite kHeart = {7, 6, kHeartRows};

uint32_t PaletteColor(char c) {
    switch (c) {
        case 'K': return 0xFF3B2418;
        case 'O': return 0xFFF0913A;
        case 'D': return 0xFFC96A24;
        case 'W': return 0xFFF8F4EE;
        case 'P': return 0xFFF5A6B8;
        case 'E': return 0xFF11111B;
        case 'L': return 0xFFB4BEFE;
    }
    return 0;
}

constexpr int kCanvasW = 30;
constexpr int kCanvasH = 26;
constexpr int kDogX = 3;   // where the 24x16 dog sits inside the canvas
constexpr int kDogY = 10;
constexpr int kDogW = 24;
constexpr int kDogH = 16;

constexpr UINT kMsgReloadSettings = WM_APP + 1;
constexpr UINT_PTR kTimerId = 1;
constexpr PCWSTR kClassName = L"WindhawkTaskbarCorgi";

struct Settings {
    int size = 3;
    bool chase = false;
    bool hideInFullscreen = true;
    double speed = 1.0;
};

enum class State { Walk, Sit, Sleep, Chase, Watch };

struct Pet {
    bool placed = false;
    State state = State::Sit;
    double x = 0;  // screen x of the canvas' left edge
    int dir = 1;   // 1 = facing right, -1 = facing left
    double targetX = 0;
    double stateLeft = 2.0;
    double anim = 0;
    double hover = 0;
    double heart = 0;
};

// Everything below is only touched on the worker thread.
Settings g_settings;
Pet g_pet;
HINSTANCE g_hInstance = nullptr;
HWND g_hwnd = nullptr;
bool g_shown = false;
HDC g_memDC = nullptr;
HBITMAP g_bitmap = nullptr;
HGDIOBJ g_oldBitmap = nullptr;
uint32_t* g_bits = nullptr;
int g_bitmapW = 0;
int g_bitmapH = 0;
uint32_t g_canvas[kCanvasH][kCanvasW];
uint32_t g_rng = 1;
ULONGLONG g_lastTick = 0;
double g_topmostTimer = 0;

HANDLE g_thread = nullptr;
DWORD g_threadId = 0;
HANDLE g_stopEvent = nullptr;

double Rand() {
    g_rng ^= g_rng << 13;
    g_rng ^= g_rng >> 17;
    g_rng ^= g_rng << 5;
    return (g_rng & 0xFFFFFF) / 16777216.0;
}

void LoadSettings() {
    int size = Wh_GetIntSetting(L"size");
    g_settings.size = size < 1 ? 1 : (size > 8 ? 8 : size);
    g_settings.chase = Wh_GetIntSetting(L"chaseMouse") != 0;
    g_settings.hideInFullscreen = Wh_GetIntSetting(L"hideInFullscreen") != 0;
    int speed = Wh_GetIntSetting(L"speed");
    speed = speed < 25 ? 25 : (speed > 300 ? 300 : speed);
    g_settings.speed = speed / 100.0;
}

void DrawSprite(const Sprite& s, int ox, int oy) {
    for (int y = 0; y < s.h; y++) {
        int cy = oy + y;
        if (cy < 0 || cy >= kCanvasH) {
            continue;
        }
        for (int x = 0; x < s.w; x++) {
            int cx = ox + x;
            if (cx < 0 || cx >= kCanvasW) {
                continue;
            }
            uint32_t color = PaletteColor(s.rows[y][x]);
            if (color) {
                g_canvas[cy][cx] = color;
            }
        }
    }
}

bool EnsureBitmap(int w, int h) {
    if (g_bits && g_bitmapW == w && g_bitmapH == h) {
        return true;
    }
    if (g_bitmap) {
        SelectObject(g_memDC, g_oldBitmap);
        DeleteObject(g_bitmap);
        g_bitmap = nullptr;
        g_bits = nullptr;
    }

    BITMAPINFO bi = {};
    bi.bmiHeader.biSize = sizeof(bi.bmiHeader);
    bi.bmiHeader.biWidth = w;
    bi.bmiHeader.biHeight = -h;  // top-down
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;

    void* bits = nullptr;
    g_bitmap = CreateDIBSection(g_memDC, &bi, DIB_RGB_COLORS, &bits, nullptr, 0);
    if (!g_bitmap) {
        return false;
    }
    g_oldBitmap = SelectObject(g_memDC, g_bitmap);
    g_bits = static_cast<uint32_t*>(bits);
    g_bitmapW = w;
    g_bitmapH = h;
    return true;
}

void Hide() {
    if (g_shown) {
        ShowWindow(g_hwnd, SW_HIDE);
        g_shown = false;
    }
}

bool IsFullscreenAppActive(const RECT& monitor) {
    HWND fg = GetForegroundWindow();
    if (!fg) {
        return false;
    }

    WCHAR cls[64];
    if (GetClassNameW(fg, cls, ARRAYSIZE(cls))) {
        for (PCWSTR skip : {L"Progman", L"WorkerW", L"Shell_TrayWnd"}) {
            if (wcscmp(cls, skip) == 0) {
                return false;
            }
        }
    }

    RECT r;
    if (!GetWindowRect(fg, &r)) {
        return false;
    }
    return r.left <= monitor.left && r.top <= monitor.top &&
           r.right >= monitor.right && r.bottom >= monitor.bottom;
}

void StartWalk(double minX, double maxX, int scale) {
    double target = g_pet.x;
    for (int i = 0; i < 5; i++) {
        target = minX + Rand() * (maxX - minX);
        if (std::fabs(target - g_pet.x) > 40.0 * scale) {
            break;
        }
    }
    g_pet.targetX = target;
    g_pet.state = State::Walk;
}

void PickNextActivity(double minX, double maxX, int scale) {
    double r = Rand();
    if (r < 0.5) {
        g_pet.state = State::Sit;
        g_pet.stateLeft = 3.0 + Rand() * 5.0;
    } else if (r < 0.8) {
        StartWalk(minX, maxX, scale);
    } else {
        g_pet.state = State::Sleep;
        g_pet.stateLeft = 20.0 + Rand() * 30.0;
    }
}

void Update() {
    ULONGLONG now = GetTickCount64();
    double dt = g_lastTick ? (now - g_lastTick) / 1000.0 : 0.0;
    g_lastTick = now;
    if (dt > 0.1) {
        dt = 0.1;
    }

    HWND taskbar = FindWindowW(L"Shell_TrayWnd", nullptr);
    if (!taskbar) {
        Hide();
        return;
    }

    RECT tbRect;
    MONITORINFO mi = {sizeof(mi)};
    if (!GetWindowRect(taskbar, &tbRect) ||
        !GetMonitorInfoW(MonitorFromWindow(taskbar, MONITOR_DEFAULTTOPRIMARY),
                         &mi)) {
        Hide();
        return;
    }

    if (g_settings.hideInFullscreen && IsFullscreenAppActive(mi.rcMonitor)) {
        Hide();
        return;
    }

    UINT dpi = GetDpiForWindow(taskbar);
    if (!dpi) {
        dpi = 96;
    }
    int scale = (int)std::lround(g_settings.size * dpi / 96.0);
    if (scale < 1) {
        scale = 1;
    }

    int w = kCanvasW * scale;
    int h = kCanvasH * scale;
    int ground = tbRect.top < mi.rcMonitor.bottom ? tbRect.top
                                                  : mi.rcMonitor.bottom;
    int winY = ground - h;
    double minX = mi.rcMonitor.left - kDogX * scale;
    double maxX = mi.rcMonitor.right - (kDogX + kDogW) * scale;

    if (!g_pet.placed) {
        g_pet.placed = true;
        g_pet.x = minX + (maxX - minX) * (0.3 + Rand() * 0.4);
        g_pet.state = State::Sit;
        g_pet.stateLeft = 2.0;
    }

    // Mouse interaction.
    POINT pt;
    GetCursorPos(&pt);
    double center = g_pet.x + (kDogX + kDogW / 2.0) * scale;
    double dist = pt.x - center;
    RECT dogRect = {(LONG)(g_pet.x + kDogX * scale), ground - kDogH * scale,
                    (LONG)(g_pet.x + (kDogX + kDogW) * scale), ground};
    bool overDog = PtInRect(&dogRect, pt);
    bool nearby = g_settings.chase && pt.y >= winY - 30 * scale &&
                  pt.y <= tbRect.bottom && std::fabs(dist) < 90.0 * scale;

    if (overDog) {
        g_pet.hover += dt;
        if (g_pet.hover > 0.4 && g_pet.heart <= 0) {
            g_pet.heart = 1.6;
            g_pet.hover = 0;
            if (g_pet.state == State::Sleep) {
                g_pet.state = State::Watch;
            }
        }
    } else {
        g_pet.hover = 0;
    }
    if (g_pet.heart > 0) {
        g_pet.heart -= dt;
    }

    // Behaviour.
    double walkSpeed = 14.0 * scale * g_settings.speed;
    double runSpeed = 48.0 * scale * g_settings.speed;

    if (overDog && g_pet.state != State::Sleep) {
        g_pet.state = State::Watch;
    } else if (nearby && g_pet.state != State::Sleep) {
        if (std::fabs(dist) > 6.0 * scale) {
            g_pet.state = State::Chase;
            g_pet.dir = dist > 0 ? 1 : -1;
            g_pet.x += g_pet.dir * runSpeed * dt;
        } else {
            g_pet.state = State::Watch;
        }
    } else if (g_pet.state == State::Chase || g_pet.state == State::Watch) {
        g_pet.state = State::Sit;
        g_pet.stateLeft = 1.5 + Rand() * 2.5;
    } else if (g_pet.state == State::Walk) {
        double d = g_pet.targetX - g_pet.x;
        if (std::fabs(d) <= walkSpeed * dt) {
            g_pet.x = g_pet.targetX;
            PickNextActivity(minX, maxX, scale);
        } else {
            g_pet.dir = d > 0 ? 1 : -1;
            g_pet.x += g_pet.dir * walkSpeed * dt;
        }
    } else {
        g_pet.stateLeft -= dt;
        if (g_pet.stateLeft <= 0) {
            if (g_pet.state == State::Sleep) {
                g_pet.state = State::Sit;
                g_pet.stateLeft = 1.5;
            } else {
                StartWalk(minX, maxX, scale);
            }
        }
    }

    if (g_pet.x < minX) {
        g_pet.x = minX;
    }
    if (g_pet.x > maxX) {
        g_pet.x = maxX;
    }
    g_pet.anim += dt;

    // Pick the frame.
    const Sprite* dog = &kSit;
    switch (g_pet.state) {
        case State::Walk:
            dog = std::fmod(g_pet.anim, 0.36) < 0.18 ? &kWalk1 : &kWalk2;
            break;
        case State::Chase:
            dog = std::fmod(g_pet.anim, 0.18) < 0.09 ? &kWalk1 : &kWalk2;
            break;
        case State::Sleep:
            dog = std::fmod(g_pet.anim, 2.4) < 1.2 ? &kSleep1 : &kSleep2;
            break;
        case State::Sit:
        case State::Watch:
            dog = &kSit;
            break;
    }

    // Draw the canvas (facing right), then mirror if needed.
    memset(g_canvas, 0, sizeof(g_canvas));
    DrawSprite(*dog, kDogX, kDogY);
    if (g_pet.state == State::Sleep) {
        for (int i = 0; i < 2; i++) {
            double p = std::fmod(g_pet.anim + i * 1.2, 2.4) / 2.4;
            if (p < 0.9) {
                DrawSprite(kZ, kDogX + 20 + (int)(p * 2),
                           kDogY + 4 - (int)(p * 12));
            }
        }
    }
    if (g_pet.heart > 0) {
        DrawSprite(kHeart, kDogX + 15, 1 + (int)(g_pet.heart / 1.6 * 3));
    }

    if (!EnsureBitmap(w, h)) {
        return;
    }
    for (int cy = 0; cy < kCanvasH; cy++) {
        for (int cx = 0; cx < kCanvasW; cx++) {
            uint32_t color =
                g_canvas[cy][g_pet.dir < 0 ? kCanvasW - 1 - cx : cx];
            for (int sy = 0; sy < scale; sy++) {
                uint32_t* row = g_bits + (size_t)(cy * scale + sy) * w;
                for (int sx = 0; sx < scale; sx++) {
                    row[cx * scale + sx] = color;
                }
            }
        }
    }

    POINT dst = {(LONG)std::lround(g_pet.x), winY};
    SIZE size = {w, h};
    POINT src = {0, 0};
    BLENDFUNCTION blend = {AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
    UpdateLayeredWindow(g_hwnd, nullptr, &dst, &size, g_memDC, &src, 0, &blend,
                        ULW_ALPHA);

    if (!g_shown) {
        ShowWindow(g_hwnd, SW_SHOWNOACTIVATE);
        g_shown = true;
    }

    g_topmostTimer -= dt;
    if (g_topmostTimer <= 0) {
        g_topmostTimer = 2.0;
        SetWindowPos(g_hwnd, HWND_TOPMOST, 0, 0, 0, 0,
                     SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
    }
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_TIMER:
            if (wParam == kTimerId) {
                Update();
            }
            return 0;
        case WM_MOUSEACTIVATE:
            return MA_NOACTIVATE;
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

DWORD WINAPI WorkerThread(LPVOID) {
    MSG msg;
    PeekMessageW(&msg, nullptr, 0, 0, PM_NOREMOVE);  // create the queue

    // Only the explorer.exe instance that owns the taskbar gets a corgi.
    for (;;) {
        HWND taskbar = FindWindowW(L"Shell_TrayWnd", nullptr);
        if (taskbar) {
            DWORD pid = 0;
            GetWindowThreadProcessId(taskbar, &pid);
            if (pid != GetCurrentProcessId()) {
                return 0;
            }
            break;
        }
        if (WaitForSingleObject(g_stopEvent, 500) == WAIT_OBJECT_0) {
            return 0;
        }
    }

    LoadSettings();
    g_rng = (uint32_t)GetTickCount64() | 1;

    WNDCLASSEXW wc = {sizeof(wc)};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = g_hInstance;
    wc.lpszClassName = kClassName;
    RegisterClassExW(&wc);

    g_hwnd = CreateWindowExW(WS_EX_LAYERED | WS_EX_TRANSPARENT |
                                 WS_EX_TOOLWINDOW | WS_EX_TOPMOST |
                                 WS_EX_NOACTIVATE,
                             kClassName, L"Taskbar Corgi", WS_POPUP, 0, 0, 1, 1,
                             nullptr, nullptr, g_hInstance, nullptr);
    if (g_hwnd) {
        g_memDC = CreateCompatibleDC(nullptr);
        SetTimer(g_hwnd, kTimerId, 33, nullptr);

        while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
            if (!msg.hwnd && msg.message == kMsgReloadSettings) {
                LoadSettings();
                continue;
            }
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }

        KillTimer(g_hwnd, kTimerId);
        DestroyWindow(g_hwnd);
        g_hwnd = nullptr;
        if (g_bitmap) {
            SelectObject(g_memDC, g_oldBitmap);
            DeleteObject(g_bitmap);
            g_bitmap = nullptr;
            g_bits = nullptr;
        }
        DeleteDC(g_memDC);
        g_memDC = nullptr;
    }

    UnregisterClassW(kClassName, g_hInstance);
    return 0;
}

}  // namespace

BOOL Wh_ModInit() {
    GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                           GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                       reinterpret_cast<LPCWSTR>(&WndProc), &g_hInstance);

    g_stopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_stopEvent) {
        return FALSE;
    }
    g_thread = CreateThread(nullptr, 0, WorkerThread, nullptr, 0, &g_threadId);
    if (!g_thread) {
        CloseHandle(g_stopEvent);
        g_stopEvent = nullptr;
        return FALSE;
    }
    return TRUE;
}

void Wh_ModSettingsChanged() {
    if (g_threadId) {
        PostThreadMessageW(g_threadId, kMsgReloadSettings, 0, 0);
    }
}

void Wh_ModUninit() {
    if (!g_thread) {
        return;
    }
    SetEvent(g_stopEvent);
    while (!PostThreadMessageW(g_threadId, WM_QUIT, 0, 0)) {
        if (WaitForSingleObject(g_thread, 10) == WAIT_OBJECT_0) {
            break;
        }
    }
    WaitForSingleObject(g_thread, INFINITE);
    CloseHandle(g_thread);
    CloseHandle(g_stopEvent);
    g_thread = nullptr;
    g_stopEvent = nullptr;
    g_threadId = 0;
}
