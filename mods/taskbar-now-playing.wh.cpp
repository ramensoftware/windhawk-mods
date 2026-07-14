// ==WindhawkMod==
// @id              taskbar-now-playing
// @name            Taskbar Now Playing
// @description     Shows the currently playing track (Spotify, browsers, Yandex Music) on the left side of the Windows 11 taskbar
// @version         1.0.0
// @author          stillmvd
// @github          https://github.com/stillmvd
// @include         explorer.exe
// @license         MIT
// @compilerOptions -lole32 -lgdi32 -luser32 -lwindowsapp -lshcore -lgdiplus -lshell32 -ld2d1 -ldwrite
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Taskbar Now Playing

Shows the currently playing track in the left corner of the Windows 11 taskbar:
album art, title and artist. Works with any app that publishes a Windows media
session (SMTC): Spotify, Yandex Music, browsers (Chrome, Edge, Firefox — VK
Music, YouTube, etc.).

- Left click — play/pause
- Right click — next track
- Middle click — previous track
- Mouse wheel — volume

The widget hides when nothing is playing and appears on playback.

Requirement: disable taskbar Widgets so the left corner is free.

Tool-mod architecture: https://github.com/ramensoftware/windhawk-mods/pull/1916
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- PanelWidth: 300
  $name: Panel width (px)
- PanelHeight: 44
  $name: Panel height (px)
- OffsetX: 12
  $name: Left offset (px)
- FontSize: 13
  $name: Title font size (px)
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <d2d1.h>
#include <dwrite.h>
#include <gdiplus.h>
#include <shcore.h>
#include <shellapi.h>
#include <string>
#include <mutex>
#include <thread>

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Media.Control.h>
#include <winrt/Windows.Storage.Streams.h>

using namespace Gdiplus;
using namespace winrt;
using namespace Windows::Media::Control;
using namespace Windows::Storage::Streams;

enum ZBID {
    ZBID_DEFAULT = 0,
    ZBID_IMMERSIVE_NOTIFICATION = 4,
};

typedef HWND(WINAPI* pCreateWindowInBand)(
    DWORD dwExStyle, LPCWSTR lpClassName, LPCWSTR lpWindowName, DWORD dwStyle,
    int x, int y, int nWidth, int nHeight,
    HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam, DWORD dwBand);

struct Settings {
    int width = 300;
    int height = 44;
    int offsetX = 12;
    int fontSize = 13;
} g_settings;

struct MediaState {
    std::mutex lock;
    std::wstring title;
    std::wstring artist;
    bool isPlaying = false;
    bool hasMedia = false;
    Bitmap* albumArt = nullptr;
    UINT64 artStreamSize = 0;
    UINT64 suspectArtSize = 0;
    int artDelayTicks = 0;
} g_media;

HWND g_hwnd = nullptr;
HHOOK g_mouseHook = nullptr;
HWINEVENTHOOK g_taskbarHook = nullptr;
UINT g_taskbarCreatedMsg = RegisterWindowMessage(L"TaskbarCreated");
bool g_hover = false;
GlobalSystemMediaTransportControlsSessionManager g_manager{nullptr};
winrt::hstring g_lastPlayingApp;
ID2D1Factory* g_d2dFactory = nullptr;
IDWriteFactory* g_dwFactory = nullptr;
ID2D1DCRenderTarget* g_dcRT = nullptr;

#define IDT_POLL 1001
#define WM_REPOSITION (WM_APP + 10)
#define WM_WHEEL_VOLUME (WM_APP + 11)
#define WM_APP_CLOSE WM_APP

void LoadSettings() {
    g_settings.width = Wh_GetIntSetting(L"PanelWidth");
    g_settings.height = Wh_GetIntSetting(L"PanelHeight");
    g_settings.offsetX = Wh_GetIntSetting(L"OffsetX");
    g_settings.fontSize = Wh_GetIntSetting(L"FontSize");
    if (g_settings.width < 120) g_settings.width = 300;
    if (g_settings.height < 24) g_settings.height = 44;
    if (g_settings.fontSize < 8) g_settings.fontSize = 13;
}

GlobalSystemMediaTransportControlsSession PickSession() {
    if (!g_manager) return nullptr;
    auto sessions = g_manager.GetSessions();
    for (auto const& s : sessions) {
        auto pb = s.GetPlaybackInfo();
        if (pb && pb.PlaybackStatus() ==
                      GlobalSystemMediaTransportControlsSessionPlaybackStatus::Playing) {
            g_lastPlayingApp = s.SourceAppUserModelId();
            return s;
        }
    }
    if (!g_lastPlayingApp.empty()) {
        for (auto const& s : sessions) {
            if (s.SourceAppUserModelId() == g_lastPlayingApp) {
                return s;
            }
        }
        return nullptr;
    }
    return g_manager.GetCurrentSession();
}

bool IsBrowserApp(winrt::hstring const& app) {
    std::wstring s = app.c_str();
    for (auto& c : s) c = towlower(c);
    for (auto part : {L"chrome", L"edge", L"firefox", L"opera", L"brave",
                      L"vivaldi", L"browser"}) {
        if (s.find(part) != std::wstring::npos) return true;
    }
    return false;
}

Bitmap* StreamToBitmap(IRandomAccessStreamWithContentType const& stream) {
    if (!stream) return nullptr;
    IStream* native = nullptr;
    if (SUCCEEDED(CreateStreamOverRandomAccessStream(
            reinterpret_cast<IUnknown*>(winrt::get_abi(stream)), IID_PPV_ARGS(&native)))) {
        Bitmap* bmp = Bitmap::FromStream(native);
        native->Release();
        if (bmp && bmp->GetLastStatus() == Ok) return bmp;
        delete bmp;
    }
    return nullptr;
}

void UpdateMediaInfo() {
    try {
        if (!g_manager) {
            g_manager = GlobalSystemMediaTransportControlsSessionManager::RequestAsync().get();
        }
        auto session = PickSession();
        if (session) {
            auto props = session.TryGetMediaPropertiesAsync().get();
            auto info = session.GetPlaybackInfo();
            bool browser = IsBrowserApp(session.SourceAppUserModelId());

            std::wstring newTitle = props.Title().c_str();
            std::wstring newArtist = props.Artist().c_str();

            std::lock_guard<std::mutex> guard(g_media.lock);
            if (!newTitle.empty()) {
                if (newTitle != g_media.title || newArtist != g_media.artist) {
                    g_media.suspectArtSize = browser ? g_media.artStreamSize : 0;
                    delete g_media.albumArt;
                    g_media.albumArt = nullptr;
                    g_media.artStreamSize = 0;
                    g_media.artDelayTicks = 1;
                }
                auto thumbRef = props.Thumbnail();
                if (!thumbRef) {
                    delete g_media.albumArt;
                    g_media.albumArt = nullptr;
                    g_media.artStreamSize = 0;
                } else if (g_media.artDelayTicks > 0) {
                    g_media.artDelayTicks--;
                } else {
                    auto stream = thumbRef.OpenReadAsync().get();
                    UINT64 size = stream ? stream.Size() : 0;
                    if (size != g_media.artStreamSize) {
                        delete g_media.albumArt;
                        g_media.albumArt = nullptr;
                        if (size == 0 ||
                            (browser && size == g_media.suspectArtSize)) {
                            g_media.artStreamSize = size;
                        } else {
                            g_media.albumArt = StreamToBitmap(stream);
                            g_media.artStreamSize = size;
                        }
                    }
                }
                g_media.title = newTitle;
                g_media.artist = newArtist;
                g_media.hasMedia = true;
            } else {
                g_media.hasMedia = false;
            }
            g_media.isPlaying =
                info.PlaybackStatus() ==
                GlobalSystemMediaTransportControlsSessionPlaybackStatus::Playing;
        } else {
            std::lock_guard<std::mutex> guard(g_media.lock);
            g_media.hasMedia = false;
        }
    } catch (...) {
        std::lock_guard<std::mutex> guard(g_media.lock);
        g_media.hasMedia = false;
    }
}

void SendMediaCommand(int cmd) {
    try {
        auto session = PickSession();
        if (!session) return;
        if (cmd == 1) session.TryTogglePlayPauseAsync();
        else if (cmd == 2) session.TrySkipNextAsync();
        else if (cmd == 3) session.TrySkipPreviousAsync();
    } catch (...) {}
}

bool IsLightTheme() {
    DWORD value = 0, size = sizeof(value);
    if (RegGetValueW(HKEY_CURRENT_USER,
                     L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
                     L"SystemUsesLightTheme", RRF_RT_DWORD, nullptr, &value,
                     &size) == ERROR_SUCCESS) {
        return value != 0;
    }
    return false;
}

void AddRoundedRect(GraphicsPath& path, REAL x, REAL y, REAL w, REAL h, REAL r) {
    REAL d = r * 2;
    path.AddArc(x, y, d, d, 180, 90);
    path.AddArc(x + w - d, y, d, d, 270, 90);
    path.AddArc(x + w - d, y + h - d, d, d, 0, 90);
    path.AddArc(x, y + h - d, d, d, 90, 90);
    path.CloseFigure();
}

void DrawPanel(Graphics& g, int w, int h) {
    g.SetSmoothingMode(SmoothingModeAntiAlias);
    g.SetInterpolationMode(InterpolationModeHighQualityBicubic);
    g.Clear(Color(1, 0, 0, 0));

    bool isPlaying = false, hasMedia = false;
    Bitmap* art = nullptr;
    {
        std::lock_guard<std::mutex> guard(g_media.lock);
        isPlaying = g_media.isPlaying;
        hasMedia = g_media.hasMedia;
        art = g_media.albumArt ? g_media.albumArt->Clone(
                  0, 0, g_media.albumArt->GetWidth(), g_media.albumArt->GetHeight(),
                  PixelFormat32bppARGB)
                               : nullptr;
    }

    Color textColor{IsLightTheme() ? 0xFF000000 : 0xFFFFFFFF};

    if (g_hover) {
        GraphicsPath bg;
        AddRoundedRect(bg, 0.0f, 0.0f, (REAL)w, (REAL)h, 10.0f);
        SolidBrush hoverBrush{Color(25, textColor.GetRed(), textColor.GetGreen(),
                                    textColor.GetBlue())};
        g.FillPath(&hoverBrush, &bg);
    }

    REAL artSize = (REAL)h;
    GraphicsPath artPath;
    AddRoundedRect(artPath, 0.0f, 0.0f, artSize, artSize, 8.0f);

    if (art) {
        g.SetClip(&artPath);
        g.DrawImage(art, 0.0f, 0.0f, artSize, artSize);
        g.ResetClip();
        delete art;
    } else {
        SolidBrush placeholder{Color(55, textColor.GetRed(), textColor.GetGreen(),
                                     textColor.GetBlue())};
        g.FillPath(&placeholder, &artPath);
    }

    if (hasMedia && !isPlaying) {
        SolidBrush dim{Color(110, 0, 0, 0)};
        g.FillPath(&dim, &artPath);
        REAL cx = artSize / 2, cy = artSize / 2;
        REAL tw = artSize * 0.28f, th = artSize * 0.36f;
        PointF tri[3] = {{cx - tw / 2, cy - th / 2},
                         {cx - tw / 2, cy + th / 2},
                         {cx + tw / 2, cy}};
        SolidBrush white{Color(230, 255, 255, 255)};
        g.FillPolygon(&white, tri, 3);
    }

}

bool EnsureD2D() {
    if (!g_d2dFactory &&
        FAILED(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &g_d2dFactory))) {
        return false;
    }
    if (!g_dwFactory &&
        FAILED(DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED,
                                   __uuidof(IDWriteFactory),
                                   (IUnknown**)&g_dwFactory))) {
        return false;
    }
    if (!g_dcRT) {
        D2D1_RENDER_TARGET_PROPERTIES props = D2D1::RenderTargetProperties(
            D2D1_RENDER_TARGET_TYPE_DEFAULT,
            D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM,
                              D2D1_ALPHA_MODE_PREMULTIPLIED),
            96.0f, 96.0f);
        if (FAILED(g_d2dFactory->CreateDCRenderTarget(&props, &g_dcRT))) {
            return false;
        }
    }
    return true;
}

void DrawPanelText(HDC dc, int w, int h) {
    if (!EnsureD2D()) return;

    std::wstring title, artist;
    bool hasArt, isPlaying;
    {
        std::lock_guard<std::mutex> guard(g_media.lock);
        title = g_media.title;
        artist = g_media.artist;
        hasArt = g_media.albumArt != nullptr;
        isPlaying = g_media.isPlaying;
    }
    if (title.empty()) return;

    float textX = (float)h + 10;
    float textW = (float)w - textX - 6;
    if (textW <= 0) return;

    RECT rc = {0, 0, w, h};
    if (FAILED(g_dcRT->BindDC(dc, &rc))) return;

    g_dcRT->BeginDraw();
    g_dcRT->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE);

    bool light = IsLightTheme();
    ID2D1SolidColorBrush* brush = nullptr;
    g_dcRT->CreateSolidColorBrush(
        light ? D2D1::ColorF(0, 0, 0) : D2D1::ColorF(1, 1, 1), &brush);

    auto drawLine = [&](const std::wstring& text, float top, float bottom,
                        float size, DWRITE_FONT_WEIGHT weight, float opacity) {
        IDWriteTextFormat* fmt = nullptr;
        if (FAILED(g_dwFactory->CreateTextFormat(
                L"Segoe UI Variable Display", nullptr, weight,
                DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, size, L"",
                &fmt))) {
            return;
        }
        fmt->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
        fmt->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
        IDWriteInlineObject* ellipsis = nullptr;
        g_dwFactory->CreateEllipsisTrimmingSign(fmt, &ellipsis);
        DWRITE_TRIMMING trim = {DWRITE_TRIMMING_GRANULARITY_CHARACTER, 0, 0};
        fmt->SetTrimming(&trim, ellipsis);
        brush->SetOpacity(opacity);
        g_dcRT->DrawText(text.c_str(), (UINT32)text.size(), fmt,
                         D2D1::RectF(textX, top, textX + textW, bottom), brush,
                         D2D1_DRAW_TEXT_OPTIONS_CLIP);
        if (ellipsis) ellipsis->Release();
        fmt->Release();
    };

    if (brush) {
        float fontSize = (float)g_settings.fontSize;
        if (artist.empty()) {
            drawLine(title, 0, (float)h, fontSize, DWRITE_FONT_WEIGHT_SEMI_BOLD,
                     1.0f);
        } else {
            float half = (float)h / 2;
            drawLine(title, 1, half, fontSize, DWRITE_FONT_WEIGHT_SEMI_BOLD, 1.0f);
            drawLine(artist, half + 1, (float)h - 1, fontSize - 1,
                     DWRITE_FONT_WEIGHT_NORMAL, 0.8f);
        }

        if (!hasArt && isPlaying) {
            IDWriteTextFormat* iconFmt = nullptr;
            if (SUCCEEDED(g_dwFactory->CreateTextFormat(
                    L"Segoe Fluent Icons", nullptr, DWRITE_FONT_WEIGHT_NORMAL,
                    DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL,
                    (float)h * 0.45f, L"", &iconFmt))) {
                iconFmt->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
                iconFmt->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
                brush->SetOpacity(0.6f);
                g_dcRT->DrawText(L"\uE8D6", 1, iconFmt,
                                 D2D1::RectF(0, 0, (float)h, (float)h), brush);
                iconFmt->Release();
            }
        }
        brush->Release();
    }

    if (g_dcRT->EndDraw() == (HRESULT)D2DERR_RECREATE_TARGET) {
        g_dcRT->Release();
        g_dcRT = nullptr;
    }
}

void RenderWindow(HWND hwnd) {
    int w = g_settings.width, h = g_settings.height;
    RECT rc;
    GetWindowRect(hwnd, &rc);

    HDC screen = GetDC(nullptr);
    HDC memDC = CreateCompatibleDC(screen);
    BITMAPINFO bi = {};
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = w;
    bi.bmiHeader.biHeight = -h;
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;
    void* bits = nullptr;
    HBITMAP dib = CreateDIBSection(screen, &bi, DIB_RGB_COLORS, &bits, nullptr, 0);
    if (!dib) {
        DeleteDC(memDC);
        ReleaseDC(nullptr, screen);
        return;
    }
    HBITMAP old = (HBITMAP)SelectObject(memDC, dib);
    {
        Graphics g(memDC);
        DrawPanel(g, w, h);
        g.Flush();
    }
    DrawPanelText(memDC, w, h);
    POINT src = {0, 0};
    SIZE size = {w, h};
    POINT dst = {rc.left, rc.top};
    BLENDFUNCTION bf = {AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
    UpdateLayeredWindow(hwnd, screen, &dst, &size, memDC, &src, 0, &bf, ULW_ALPHA);
    SelectObject(memDC, old);
    DeleteObject(dib);
    DeleteDC(memDC);
    ReleaseDC(nullptr, screen);
}

LRESULT CALLBACK MouseHookProc(int code, WPARAM wParam, LPARAM lParam) {
    if (code == HC_ACTION && wParam == WM_MOUSEWHEEL && g_hwnd &&
        IsWindowVisible(g_hwnd)) {
        auto* info = (MSLLHOOKSTRUCT*)lParam;
        RECT rc;
        if (GetWindowRect(g_hwnd, &rc) && PtInRect(&rc, info->pt)) {
            short delta = (short)HIWORD(info->mouseData);
            PostMessage(g_hwnd, WM_WHEEL_VOLUME, delta > 0, 0);
            return 1;
        }
    }
    return CallNextHookEx(nullptr, code, wParam, lParam);
}

bool IsTaskbarWindow(HWND hwnd) {
    WCHAR cls[64];
    if (!hwnd) return false;
    GetClassNameW(hwnd, cls, ARRAYSIZE(cls));
    return wcscmp(cls, L"Shell_TrayWnd") == 0;
}

void CALLBACK TaskbarEventProc(HWINEVENTHOOK, DWORD, HWND hwnd, LONG, LONG, DWORD, DWORD) {
    if (!IsTaskbarWindow(hwnd) || !g_hwnd) return;
    PostMessage(g_hwnd, WM_REPOSITION, 0, 0);
}

void RegisterTaskbarHook(HWND hwnd) {
    HWND hTaskbar = FindWindow(L"Shell_TrayWnd", nullptr);
    if (hTaskbar) {
        DWORD pid = 0;
        DWORD tid = GetWindowThreadProcessId(hTaskbar, &pid);
        if (tid) {
            g_taskbarHook = SetWinEventHook(
                EVENT_OBJECT_LOCATIONCHANGE, EVENT_OBJECT_LOCATIONCHANGE, nullptr,
                TaskbarEventProc, pid, tid,
                WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS);
        }
    }
    PostMessage(hwnd, WM_REPOSITION, 0, 0);
}

LRESULT CALLBACK MediaWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_CREATE:
            SetTimer(hwnd, IDT_POLL, 1000, nullptr);
            RegisterTaskbarHook(hwnd);
            PostMessage(hwnd, WM_TIMER, IDT_POLL, 0);
            return 0;

        case WM_CLOSE:
            return 0;

        case WM_APP_CLOSE:
            DestroyWindow(hwnd);
            return 0;

        case WM_DESTROY:
            if (g_taskbarHook) {
                UnhookWinEvent(g_taskbarHook);
                g_taskbarHook = nullptr;
            }
            g_manager = nullptr;
            PostQuitMessage(0);
            return 0;

        case WM_SETTINGCHANGE:
            RenderWindow(hwnd);
            return 0;

        case WM_TIMER:
            if (wParam == IDT_POLL) {
                UpdateMediaInfo();
                static int noMediaTicks = 0;
                bool hasMedia;
                {
                    std::lock_guard<std::mutex> guard(g_media.lock);
                    hasMedia = g_media.hasMedia;
                }
                noMediaTicks = hasMedia ? 0 : noMediaTicks + 1;
                bool show = hasMedia || noMediaTicks < 3;
                HWND hTaskbar = FindWindow(L"Shell_TrayWnd", nullptr);
                show = show && hTaskbar && IsWindowVisible(hTaskbar);
                if (show && !IsWindowVisible(hwnd)) {
                    ShowWindow(hwnd, SW_SHOWNOACTIVATE);
                    PostMessage(hwnd, WM_REPOSITION, 0, 0);
                } else if (!show && IsWindowVisible(hwnd)) {
                    ShowWindow(hwnd, SW_HIDE);
                }
                RenderWindow(hwnd);
            }
            return 0;

        case WM_REPOSITION: {
            HWND hTaskbar = FindWindow(L"Shell_TrayWnd", nullptr);
            if (!hTaskbar) break;
            if (!IsWindowVisible(hTaskbar)) {
                if (IsWindowVisible(hwnd)) ShowWindow(hwnd, SW_HIDE);
                return 0;
            }
            RECT rc;
            GetWindowRect(hTaskbar, &rc);
            int x = rc.left + g_settings.offsetX;
            int y = rc.top + ((rc.bottom - rc.top) - g_settings.height) / 2;
            RECT my;
            GetWindowRect(hwnd, &my);
            if (my.left != x || my.top != y ||
                my.right - my.left != g_settings.width ||
                my.bottom - my.top != g_settings.height) {
                SetWindowPos(hwnd, HWND_TOPMOST, x, y, g_settings.width,
                             g_settings.height, SWP_NOACTIVATE);
                RenderWindow(hwnd);
            }
            return 0;
        }

        case WM_MOUSEMOVE: {
            if (!g_hover) {
                g_hover = true;
                RenderWindow(hwnd);
            }
            TRACKMOUSEEVENT tme = {sizeof(TRACKMOUSEEVENT), TME_LEAVE, hwnd, 0};
            TrackMouseEvent(&tme);
            return 0;
        }

        case WM_MOUSELEAVE:
            g_hover = false;
            RenderWindow(hwnd);
            return 0;

        case WM_LBUTTONUP:
            SendMediaCommand(1);
            return 0;

        case WM_RBUTTONUP:
            SendMediaCommand(2);
            return 0;

        case WM_MBUTTONUP:
            SendMediaCommand(3);
            return 0;

        case WM_MOUSEWHEEL:
            PostMessage(hwnd, WM_WHEEL_VOLUME,
                        GET_WHEEL_DELTA_WPARAM(wParam) > 0, 0);
            return 0;

        case WM_WHEEL_VOLUME: {
            BYTE vk = wParam ? VK_VOLUME_UP : VK_VOLUME_DOWN;
            keybd_event(vk, 0, 0, 0);
            keybd_event(vk, 0, KEYEVENTF_KEYUP, 0);
            return 0;
        }

        default:
            if (msg == g_taskbarCreatedMsg) {
                if (g_taskbarHook) {
                    UnhookWinEvent(g_taskbarHook);
                    g_taskbarHook = nullptr;
                }
                RegisterTaskbarHook(hwnd);
                return 0;
            }
            break;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

void MediaThread() {
    winrt::init_apartment();

    GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;
    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, nullptr);

    WNDCLASS wc = {};
    wc.lpfnWndProc = MediaWndProc;
    wc.hInstance = GetModuleHandle(nullptr);
    wc.lpszClassName = L"TaskbarNowPlayingWnd";
    wc.hCursor = LoadCursor(nullptr, IDC_HAND);
    RegisterClass(&wc);

    HMODULE hUser32 = GetModuleHandle(L"user32.dll");
    pCreateWindowInBand CreateWindowInBand =
        hUser32 ? (pCreateWindowInBand)GetProcAddress(hUser32, "CreateWindowInBand")
                : nullptr;

    DWORD exStyle = WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_TOPMOST | WS_EX_NOACTIVATE;
    if (CreateWindowInBand) {
        g_hwnd = CreateWindowInBand(
            exStyle, wc.lpszClassName, L"NowPlaying", WS_POPUP, 0, 0,
            g_settings.width, g_settings.height, nullptr, nullptr, wc.hInstance,
            nullptr, ZBID_IMMERSIVE_NOTIFICATION);
    }
    if (!g_hwnd) {
        g_hwnd = CreateWindowEx(exStyle, wc.lpszClassName, L"NowPlaying", WS_POPUP,
                                0, 0, g_settings.width, g_settings.height, nullptr,
                                nullptr, wc.hInstance, nullptr);
    }
    RenderWindow(g_hwnd);
    g_mouseHook = SetWindowsHookEx(WH_MOUSE_LL, MouseHookProc,
                                   GetModuleHandle(nullptr), 0);

    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    if (g_mouseHook) {
        UnhookWindowsHookEx(g_mouseHook);
        g_mouseHook = nullptr;
    }
    UnregisterClass(wc.lpszClassName, wc.hInstance);
    if (g_dcRT) {
        g_dcRT->Release();
        g_dcRT = nullptr;
    }
    if (g_dwFactory) {
        g_dwFactory->Release();
        g_dwFactory = nullptr;
    }
    if (g_d2dFactory) {
        g_d2dFactory->Release();
        g_d2dFactory = nullptr;
    }
    GdiplusShutdown(gdiplusToken);
    winrt::uninit_apartment();
}

std::thread* g_thread = nullptr;

BOOL WhTool_ModInit() {
    LoadSettings();
    g_thread = new std::thread(MediaThread);
    return TRUE;
}

void WhTool_ModUninit() {
    if (g_hwnd) SendMessage(g_hwnd, WM_APP_CLOSE, 0, 0);
    if (g_thread) {
        if (g_thread->joinable()) g_thread->join();
        delete g_thread;
        g_thread = nullptr;
    }
}

void WhTool_ModSettingsChanged() {
    LoadSettings();
    if (g_hwnd) {
        PostMessage(g_hwnd, WM_REPOSITION, 0, 0);
        PostMessage(g_hwnd, WM_TIMER, IDT_POLL, 0);
    }
}

bool g_isToolModProcessLauncher;
HANDLE g_toolModProcessMutex;

void WINAPI EntryPoint_Hook() {
    Wh_Log(L">");
    ExitThread(0);
}

BOOL Wh_ModInit() {
    bool isService = false;
    bool isToolModProcess = false;
    bool isCurrentToolModProcess = false;
    int argc;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLine(), &argc);
    if (!argv) {
        Wh_Log(L"CommandLineToArgvW failed");
        return FALSE;
    }

    for (int i = 1; i < argc; i++) {
        if (wcscmp(argv[i], L"-service") == 0) {
            isService = true;
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

    if (isService) {
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

        IMAGE_DOS_HEADER* dosHeader = (IMAGE_DOS_HEADER*)GetModuleHandle(nullptr);
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
