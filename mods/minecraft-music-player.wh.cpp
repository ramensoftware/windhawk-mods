// ==WindhawkMod==
// @id              minecraft-music-player
// @name            Minecraft Music Player
// @description     A mod for Windhawk that adds the "now playing" overlay
// @version         1.0.0
// @author          MaxURhino
// @github          https://github.com/MaxURhino
// @homepage        https://maxurhino.github.io
// @include         explorer.exe
// @compilerOptions -lgdi32 -lgdiplus -lwindowsapp -lshell32 -luser32 -lwinhttp -lshlwapi
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
Adds a Minecraft styled overlay for the current thing playing.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- scale: 3
  $name: Overlay scale
  $description: >-
    Multiplier applied to the overlay's native size (3 = original, 6 = double, etc.)
- default-font: false
  $name: Use Default Font
  $description: >-
    Use default/system font instead of the custom one
- show-toast: true
  $name: Show Toast
  $description: >-
    Makes the "Now Playing" toast appear.
    This option needs to be selected to make all other options work.
- show-animation: true
  $name: Show Animation
  $description: >-
    Adds a show/hide animation to the "Now Playing" toast
- custom-font: ""
  $name: Custom Font (Optional)
  $description: >-
    If you want to use a custom font, you can input the path in here.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <winhttp.h>

// INITGUID makes FOLDERID_RoamingAppData resolve inline instead of
// needing an extra import lib to link against.
#define INITGUID
#include <shlobj.h>
#include <knownfolders.h>

#include <string>

#include <algorithm>
#include <gdiplus.h>

#include <winrt/base.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Media.Control.h>

using namespace winrt::Windows::Media::Control;
using namespace Gdiplus;

HINSTANCE g_hInstance = nullptr;

std::vector<BYTE> DownloadUrlToMemory(const wchar_t* url) {
    std::vector<BYTE> result;

    URL_COMPONENTS urlComp = { sizeof(urlComp) };
    wchar_t hostName[256] = {};
    wchar_t urlPath[2048] = {};
    urlComp.lpszHostName = hostName;
    urlComp.dwHostNameLength = ARRAYSIZE(hostName);
    urlComp.lpszUrlPath = urlPath;
    urlComp.dwUrlPathLength = ARRAYSIZE(urlPath);

    if (!WinHttpCrackUrl(url, 0, 0, &urlComp)) return result;

    HINTERNET hSession = WinHttpOpen(L"WindhawkModAgent/1.0",
                                  WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
                                  WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
    if (!hSession) return result;

    WinHttpSetTimeouts(hSession, 5000, 5000, 5000, 5000); // resolve, connect, send, receive (ms)

    HINTERNET hConnect = WinHttpConnect(hSession, hostName, urlComp.nPort, 0);
    if (!hConnect) { WinHttpCloseHandle(hSession); return result; }

    DWORD flags = (urlComp.nScheme == INTERNET_SCHEME_HTTPS) ? WINHTTP_FLAG_SECURE : 0;
    HINTERNET hRequest = WinHttpOpenRequest(hConnect, L"GET", urlPath, nullptr,
                                             WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, flags);
    if (!hRequest) { WinHttpCloseHandle(hConnect); WinHttpCloseHandle(hSession); return result; }

    bool ok = WinHttpSendRequest(hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0,
                                  WINHTTP_NO_REQUEST_DATA, 0, 0, 0)
              && WinHttpReceiveResponse(hRequest, nullptr);

    if (ok) {
        DWORD bytesAvailable = 0;
        while (WinHttpQueryDataAvailable(hRequest, &bytesAvailable) && bytesAvailable > 0) {
            size_t oldSize = result.size();
            result.resize(oldSize + bytesAvailable);

            DWORD bytesRead = 0;
            WinHttpReadData(hRequest, result.data() + oldSize, bytesAvailable, &bytesRead);
            result.resize(oldSize + bytesRead);
        }
    }

    WinHttpCloseHandle(hRequest);
    WinHttpCloseHandle(hConnect);
    WinHttpCloseHandle(hSession);
    return result;
}

std::vector<BYTE> DecodeBase64(const std::string& input) {
    static const std::string chars =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

    std::vector<BYTE> result;
    std::vector<int> lookup(256, -1);
    for (size_t i = 0; i < chars.size(); i++) lookup[(unsigned char)chars[i]] = (int)i;

    int val = 0, bits = -8;
    for (unsigned char c : input) {
        if (lookup[c] == -1) {
            if (c == '=') break;
            continue; // skip whitespace/newlines
        }
        val = (val << 6) + lookup[c];
        bits += 6;
        if (bits >= 0) {
            result.push_back((BYTE)((val >> bits) & 0xFF));
            bits -= 8;
        }
    }
    return result;
}

std::vector<BYTE> DecodeDataUri(const std::string& dataUri) {
    size_t commaPos = dataUri.find(',');
    if (commaPos == std::string::npos) return {};
    return DecodeBase64(dataUri.substr(commaPos + 1));
}

#include <shlwapi.h> // needed for SHCreateMemStream

IStream* g_toastStream = nullptr;
IStream* g_notesStream = nullptr;

Gdiplus::Image* LoadImageFromMemory(const std::vector<BYTE>& data, IStream** outStream) {
    if (data.empty()) return nullptr;

    IStream* stream = SHCreateMemStream(data.data(), (UINT)data.size());
    if (!stream) return nullptr;

    Gdiplus::Image* image = new Gdiplus::Image(stream);
    *outStream = stream; // keep alive — see note below
    return image;
}

std::vector<BYTE> g_fontBytes; // must outlive the PrivateFontCollection — see note

HINSTANCE GetModHInstance() {
    HMODULE hMod = nullptr;
    GetModuleHandleExW(
        GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
            GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
        (LPCWSTR)&GetModHInstance,
        &hMod);
    return (HINSTANCE)hMod;
}

HWND g_hwnd;
HANDLE g_thread;
volatile bool g_stop = false;
ULONG_PTR g_gdiplusToken;
Image* g_image = nullptr;

Image* g_musicNotes = nullptr;
int g_musicNotes_frameCount = 8;      // set this to however many frames your sheet has
int g_musicNotes_frameWidth = 16;
int g_musicNotes_frameHeight = 16;
int g_musicNotes_currentFrame = 0;

int g_musicNotes_currentWidth = 0, g_musicNotes_currentHeight = 0; // cache so ticking doesn't need SetWindowPos

const UINT_PTR musicNotes_kAnimTimerId = 1;
const UINT musicNotes_kAnimIntervalMs = 100; // 2 ticks * 50ms/tick (Minecraft runs at 20 tps)

int g_scale = 3;
bool g_defaultFont = false;
int g_baseWidth = 160, g_baseHeight = 30; // native image size, set once at load

int g_moveFrame = 0;
int g_moveDirection = -1;

int g_overlayY = -30;

const UINT_PTR kRenderTimerId = 1;
const UINT kRenderIntervalMs = 16; // ~60fps, was 100ms

const UINT kSpriteFrameIntervalMs = 100; // sprite still advances every 100ms
const UINT kNowPlayingPollIntervalMs = 500; // SMTC poll, doesn't need to be per-frame at all

bool g_showAnimation = true;
bool g_showToast = true;

ULONGLONG g_lastSpriteAdvance = 0;
ULONGLONG g_lastNowPlayingPoll = 0;

Gdiplus::FontFamily* g_systemFontFamily = nullptr;
Gdiplus::PrivateFontCollection* g_privateFonts = nullptr;
Gdiplus::FontFamily* g_customFontFamily = nullptr;

bool InitCustomFontFromMemory(const std::vector<BYTE>& fontData) {
    if (fontData.empty()) return false;

    g_fontBytes = fontData; // copy into a long-lived global
    g_privateFonts = new Gdiplus::PrivateFontCollection();
    Gdiplus::Status status = g_privateFonts->AddMemoryFont(g_fontBytes.data(), (INT)g_fontBytes.size());
    if (status != Gdiplus::Ok) return false;

    int count = g_privateFonts->GetFamilyCount();
    if (count == 0) return false;

    g_customFontFamily = new Gdiplus::FontFamily();
    int found = 0;
    g_privateFonts->GetFamilies(1, g_customFontFamily, &found);
    return found > 0;
}

std::string songName, lastSongName;

bool InitCustomFont(const wchar_t* fontFilePath) {
    g_privateFonts = new Gdiplus::PrivateFontCollection();
    Gdiplus::Status status = g_privateFonts->AddFontFile(fontFilePath);
    if (status != Gdiplus::Ok) {
        delete g_privateFonts;
        g_privateFonts = nullptr;
        return false;
    }

    int count = g_privateFonts->GetFamilyCount();
    if (count == 0) return false;

    g_customFontFamily = new Gdiplus::FontFamily();
    int found = 0;
    g_privateFonts->GetFamilies(1, g_customFontFamily, &found);

    return found > 0;
}

Gdiplus::Font* titleFont = nullptr;

void InitSystemFont(HDC hdc) {
    NONCLIENTMETRICSW ncm = { sizeof(ncm) };
    SystemParametersInfoW(SPI_GETNONCLIENTMETRICS, sizeof(ncm), &ncm, 0);

    Gdiplus::Font tempFont(hdc, &ncm.lfMessageFont);
    g_systemFontFamily = new Gdiplus::FontFamily();
    tempFont.GetFamily(g_systemFontFamily);
}

Gdiplus::Font* MakeSystemFont(float sizeInPoints, Gdiplus::FontStyle style = Gdiplus::FontStyleRegular) {
    if (!g_systemFontFamily) return nullptr;
    return new Gdiplus::Font(g_systemFontFamily, sizeInPoints, style, Gdiplus::UnitPoint);
}

Gdiplus::Font* MakeCustomFont(float sizeInPoints, Gdiplus::FontStyle style = Gdiplus::FontStyleRegular) {
    if (!g_customFontFamily) return MakeSystemFont(sizeInPoints, style);
    return new Gdiplus::Font(g_customFontFamily, sizeInPoints, style, Gdiplus::UnitPoint);
}

Gdiplus::RectF MeasureText(Gdiplus::Graphics& graphics, const std::wstring& text, Gdiplus::Font* font) {
    Gdiplus::RectF boundingBox;
    Gdiplus::PointF origin(0.0f, 0.0f);

    graphics.MeasureString(text.c_str(), -1, font, origin, &boundingBox);
    return boundingBox;
}

std::wstring StringToWString(const std::string& str) {
    if (str.empty()) return std::wstring();

    int sizeNeeded = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), (int)str.size(), nullptr, 0);
    std::wstring result(sizeNeeded, 0);
    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), (int)str.size(), &result[0], sizeNeeded);
    return result;
}

std::string WStringToString(const std::wstring& wstr) {
    if (wstr.empty()) return std::string();

    int sizeNeeded = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), (int)wstr.size(),
                                          nullptr, 0, nullptr, nullptr);
    std::string result(sizeNeeded, 0);
    WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), (int)wstr.size(),
                         &result[0], sizeNeeded, nullptr, nullptr);
    return result;
}

struct NinePatchMargins {
    int left, top, right, bottom;
};

NinePatchMargins createMargins(int size) {
    return { size, size, size, size };
}

struct NowPlayingInfo {
    std::wstring title;
    std::wstring subtitle; // artist
    bool hasSession = false;
};

NowPlayingInfo g_nowPlaying;
std::string g_customFont;

NowPlayingInfo GetNowPlayingInfo() {
    NowPlayingInfo info;
    try {
        auto sessionManager = GlobalSystemMediaTransportControlsSessionManager::RequestAsync().get();
        auto session = sessionManager.GetCurrentSession();
        if (session) {
            auto props = session.TryGetMediaPropertiesAsync().get();
            info.title = props.Title().c_str();
            info.subtitle = props.Artist().c_str();
            info.hasSession = true;
        }
    } catch (winrt::hresult_error const&) {
        // No active session, API unavailable, or permission denied — leave info empty
    }
    return info;
}

enum class SlideState { SlidingIn, Holding, SlidingOut, Hidden };
SlideState g_slideState = SlideState::Hidden;

const int kSlideDurationFrames = 50;
const int kHoldDurationFrames = 250;
const int kHiddenY = -30;
const int kVisibleY = 0;

std::wstring GetCacheDir() {
    PWSTR path = nullptr;
    std::wstring result;
    if (SUCCEEDED(SHGetKnownFolderPath(FOLDERID_LocalAppData, 0, nullptr, &path))) {
        result = path;
        result += L"\\WindhawkMinecraftMusicPlayer\\";
        CreateDirectoryW(result.c_str(), nullptr); // no-op if it already exists
        CoTaskMemFree(path);
    }
    return result;
}

std::vector<BYTE> ReadEntireFile(const wchar_t* path) {
    std::vector<BYTE> result;
    HANDLE hFile = CreateFileW(path, GENERIC_READ, FILE_SHARE_READ, nullptr,
                                OPEN_EXISTING, 0, nullptr);
    if (hFile == INVALID_HANDLE_VALUE) return result;

    LARGE_INTEGER size;
    GetFileSizeEx(hFile, &size);
    result.resize((size_t)size.QuadPart);

    DWORD bytesRead = 0;
    ReadFile(hFile, result.data(), (DWORD)size.QuadPart, &bytesRead, nullptr);
    CloseHandle(hFile);
    return result;
}

bool WriteEntireFile(const wchar_t* path, const std::vector<BYTE>& data) {
    HANDLE hFile = CreateFileW(path, GENERIC_WRITE, 0, nullptr,
                                CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (hFile == INVALID_HANDLE_VALUE) return false;

    DWORD written = 0;
    bool ok = WriteFile(hFile, data.data(), (DWORD)data.size(), &written, nullptr) != 0;
    CloseHandle(hFile);
    return ok;
}

std::vector<BYTE> LoadWithCache(const wchar_t* url, const wchar_t* cacheFileName) {
    std::wstring cachePath = GetCacheDir() + cacheFileName;

    std::vector<BYTE> cached = ReadEntireFile(cachePath.c_str());
    if (!cached.empty()) {
        return cached; // cache hit — no network call at all
    }

    std::vector<BYTE> downloaded = DownloadUrlToMemory(url);
    if (!downloaded.empty()) {
        WriteEntireFile(cachePath.c_str(), downloaded);
    }
    return downloaded;
}

std::string StripSurroundingQuotes(const std::string& str) {
    if (str.size() >= 2 && str.front() == '"' && str.back() == '"') {
        return str.substr(1, str.size() - 2);
    }
    return str;
}

DWORD WINAPI FontDownloadThread(LPVOID) {
    if (g_customFont != "") {
        std::wstring wsCustomFont = StringToWString(g_customFont);
        Wh_Log(L"Initializing custom font with path \"%s\"", wsCustomFont.c_str());

        bool notFailed = InitCustomFont(wsCustomFont.c_str());

        Wh_Log(L"Operation %s.", notFailed ? L"completed successfully" : L"failed");
        return notFailed;
    }

    auto fontBytes = LoadWithCache(
        L"https://github.com/tryashtar/minecraft-ttf/releases/download/v1.6/MinecraftDefaultUniformBMP-Regular.ttf",
        L"minecraft-font.ttf"
    );
    if (!fontBytes.empty()) {
        InitCustomFontFromMemory(fontBytes);
    }
    return 0;
}

void AdvanceAnimation() {
    g_moveFrame++;

    if (!g_showToast) {
        g_overlayY = kHiddenY;
        return;
    }

    if (!g_showAnimation) {
        g_overlayY = kVisibleY;
        if (g_moveFrame >= kHoldDurationFrames) {
            g_slideState = SlideState::SlidingOut;
            g_moveFrame = 0;
        }
        return;
    }

    switch (g_slideState) {
        case SlideState::SlidingIn:
            g_overlayY = kHiddenY + (int)((float)g_moveFrame / kSlideDurationFrames * (kVisibleY - kHiddenY));
            if (g_moveFrame >= kSlideDurationFrames) {
                g_overlayY = kVisibleY;
                g_slideState = SlideState::Holding;
                g_moveFrame = 0;
            }
            break;
        case SlideState::Holding:
            g_overlayY = kVisibleY;
            if (g_moveFrame >= kHoldDurationFrames) {
                g_slideState = SlideState::SlidingOut;
                g_moveFrame = 0;
            }
            break;
        case SlideState::SlidingOut:
            g_overlayY = kVisibleY - (int)((float)g_moveFrame / kSlideDurationFrames * (kVisibleY - kHiddenY));
            if (g_moveFrame >= kSlideDurationFrames) {
                g_overlayY = kHiddenY;
                g_slideState = SlideState::Hidden;
                g_moveFrame = 0;
            }
            break;
        case SlideState::Hidden:
            g_overlayY = kHiddenY;
            break; // stays here until a song change restarts it
    }
}

const NinePatchMargins kMargins = createMargins(6); // adjust to match your texture's border thickness

void DrawNinePatch(Graphics& graphics, Image* image,
                    int destWidth, int destHeight,
                    const NinePatchMargins& srcMargins,
                    const NinePatchMargins& dstMargins) {
    int srcW = (int)image->GetWidth();
    int srcH = (int)image->GetHeight();

    int srcMidW = srcW - srcMargins.left - srcMargins.right;
    int srcMidH = srcH - srcMargins.top  - srcMargins.bottom;
    int dstMidW = destWidth  - dstMargins.left - dstMargins.right;
    int dstMidH = destHeight - dstMargins.top  - dstMargins.bottom;

    auto draw = [&](int sx, int sy, int sw, int sh,
                    int dx, int dy, int dw, int dh) {
        if (sw <= 0 || sh <= 0 || dw <= 0 || dh <= 0) return;
        Rect destRect(dx, dy, dw, dh);
        graphics.DrawImage(image, destRect, sx, sy, sw, sh, UnitPixel);
    };

    // 4 corners — now scaled to dstMargins instead of staying source-sized
    draw(0, 0, srcMargins.left, srcMargins.top,
         0, 0, dstMargins.left, dstMargins.top);
    draw(srcW - srcMargins.right, 0, srcMargins.right, srcMargins.top,
         destWidth - dstMargins.right, 0, dstMargins.right, dstMargins.top);
    draw(0, srcH - srcMargins.bottom, srcMargins.left, srcMargins.bottom,
         0, destHeight - dstMargins.bottom, dstMargins.left, dstMargins.bottom);
    draw(srcW - srcMargins.right, srcH - srcMargins.bottom, srcMargins.right, srcMargins.bottom,
         destWidth - dstMargins.right, destHeight - dstMargins.bottom, dstMargins.right, dstMargins.bottom);

    // 4 edges
    draw(srcMargins.left, 0, srcMidW, srcMargins.top,
         dstMargins.left, 0, dstMidW, dstMargins.top);
    draw(srcMargins.left, srcH - srcMargins.bottom, srcMidW, srcMargins.bottom,
         dstMargins.left, destHeight - dstMargins.bottom, dstMidW, dstMargins.bottom);
    draw(0, srcMargins.top, srcMargins.left, srcMidH,
         0, dstMargins.top, dstMargins.left, dstMidH);
    draw(srcW - srcMargins.right, srcMargins.top, srcMargins.right, srcMidH,
         destWidth - dstMargins.right, dstMargins.top, dstMargins.right, dstMidH);

    // center
    draw(srcMargins.left, srcMargins.top, srcMidW, srcMidH,
         dstMargins.left, dstMargins.top, dstMidW, dstMidH);
}

std::string createSongName() {
    return WStringToString(g_nowPlaying.subtitle) + " - " + WStringToString(g_nowPlaying.title);
}

void RenderAndUpdateWindow(HWND hwnd, int height) {
    float sizeInPoints = 7.0f * g_scale;
    if (g_systemFontFamily || g_customFontFamily) {
        if (g_defaultFont) {
            titleFont = MakeSystemFont(sizeInPoints);
        } else {
            titleFont = MakeCustomFont(sizeInPoints);
        }
    }

    songName = createSongName();
    if (songName != lastSongName) {
        g_slideState = SlideState::SlidingIn;
        g_moveFrame = 0;
    }
    lastSongName = songName;
    std::wstring songNameWstring = StringToWString(songName);

    AdvanceAnimation();

    // Measure BEFORE deciding the canvas size, so nothing gets clipped.
    int width = g_baseWidth * g_scale; // floor: never smaller than the base toast
    if (titleFont) {
        HDC hdcMeasure = GetDC(nullptr);
        Graphics measureGraphics(hdcMeasure);
        RectF bbox;
        PointF origin(0.0f, 0.0f);
        measureGraphics.MeasureString(songNameWstring.c_str(), -1, titleFont, origin, &bbox);
        ReleaseDC(nullptr, hdcMeasure);

        width = ((int)(bbox.Width / g_scale) + 37) * g_scale;
    }

    // Keep the real window in sync with whatever width this frame needs.
    SetWindowPos(hwnd, nullptr, 0, 0 + g_overlayY, width, height,
                SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
    g_musicNotes_currentWidth  = width;
    g_musicNotes_currentHeight = height;

    HDC hdcScreen = GetDC(nullptr);
    HDC hdcMem = CreateCompatibleDC(hdcScreen);

    BITMAPINFO bmi = {};
    bmi.bmiHeader.biSize        = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth       = width;
    bmi.bmiHeader.biHeight      = -height;
    bmi.bmiHeader.biPlanes      = 1;
    bmi.bmiHeader.biBitCount    = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    void* bits = nullptr;
    HBITMAP hBitmap = CreateDIBSection(hdcMem, &bmi, DIB_RGB_COLORS, &bits, nullptr, 0);
    HBITMAP hOldBitmap = (HBITMAP)SelectObject(hdcMem, hBitmap);

    Bitmap target(width, height, width * 4, PixelFormat32bppPARGB, (BYTE*)bits);
    Graphics graphics(&target);
    graphics.SetInterpolationMode(InterpolationModeNearestNeighbor);
    graphics.SetPixelOffsetMode(PixelOffsetModeHalf);
    graphics.SetSmoothingMode(SmoothingModeNone);

    if (g_image && g_image->GetLastStatus() == Ok) {
        graphics.SetCompositingMode(CompositingModeSourceCopy);
        NinePatchMargins dstMargins = createMargins(kMargins.left * g_scale);
        DrawNinePatch(graphics, g_image, width, height, kMargins, dstMargins);
    }

    if (g_musicNotes && g_musicNotes->GetLastStatus() == Ok && g_musicNotes_frameHeight > 0) {
        graphics.SetCompositingMode(CompositingModeSourceOver);
        int srcY = g_musicNotes_currentFrame * g_musicNotes_frameHeight;
        int destW = g_musicNotes_frameWidth  * g_scale;
        int destH = g_musicNotes_frameHeight * g_scale;
        int destX = 7 * g_scale;
        int destY = 7 * g_scale;

        Rect destRect(destX, destY, destW, destH);
        graphics.DrawImage(g_musicNotes, destRect,
                            0, srcY, g_musicNotes_frameWidth, g_musicNotes_frameHeight, UnitPixel);
    }

    if (titleFont) {
        graphics.SetCompositingMode(CompositingModeSourceOver);
        graphics.SetTextRenderingHint(TextRenderingHintAntiAlias);

        Gdiplus::Color textColor = Gdiplus::Color(255, 255, 255, 255);

        Gdiplus::SolidBrush textBrush(textColor);
        Gdiplus::PointF textPos(30.0f * g_scale, 10.5f * g_scale);

        Gdiplus::SolidBrush shadowBrush(Gdiplus::Color(
            textColor.GetA(),
            textColor.GetR() / 4,
            textColor.GetG() / 4,
            textColor.GetB() / 4
        ));
        Gdiplus::PointF shadowPos(textPos.X + g_scale, textPos.Y + g_scale);
        
        const WCHAR* realSongName = songNameWstring.c_str();

        graphics.DrawString(realSongName, -1, titleFont, shadowPos, &shadowBrush);
        graphics.DrawString(realSongName, -1, titleFont, textPos,   &textBrush  );
    }

    POINT ptSrc = {0, 0};
    POINT ptDst = {0, g_overlayY * g_scale};
    SIZE size = {width, height};
    BLENDFUNCTION blend = {};
    blend.BlendOp = AC_SRC_OVER;
    blend.SourceConstantAlpha = 255;
    blend.AlphaFormat = AC_SRC_ALPHA;

    UpdateLayeredWindow(hwnd, hdcScreen, &ptDst, &size, hdcMem, &ptSrc, 0, &blend, ULW_ALPHA);

    SelectObject(hdcMem, hOldBitmap);
    DeleteObject(hBitmap);
    DeleteDC(hdcMem);
    ReleaseDC(nullptr, hdcScreen);

    delete titleFont; // was leaking every frame before
}

// ApplyScaleAndRedraw
void ApplyScaleAndRedraw() {
    if (!g_hwnd) return;
    int height = g_baseHeight * g_scale;
    RenderAndUpdateWindow(g_hwnd, height);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
        case WM_PAINT: {
            // Actual pixels come from UpdateLayeredWindow, not WM_PAINT.
            PAINTSTRUCT ps;
            BeginPaint(hwnd, &ps);
            EndPaint(hwnd, &ps);
            return 0;
        }
        case WM_APP: {
            // Posted from another thread (e.g. Wh_ModSettingsChanged) so the
            // actual resize/redraw happens on this window's own thread.
            ApplyScaleAndRedraw();
            return 0;
        }
        case WM_TIMER: {
            if (wp == kRenderTimerId) {
                ULONGLONG now = GetTickCount64();

                if (now - g_lastSpriteAdvance >= kSpriteFrameIntervalMs) {
                    g_musicNotes_currentFrame = (g_musicNotes_currentFrame + 1) % g_musicNotes_frameCount;
                    g_lastSpriteAdvance = now;
                }

                if (now - g_lastNowPlayingPoll >= kNowPlayingPollIntervalMs) {
                    g_nowPlaying = GetNowPlayingInfo();
                    g_lastNowPlayingPoll = now;
                }

                AdvanceAnimation(); // now needs to be time-based too, see below
                RenderAndUpdateWindow(hwnd, g_musicNotes_currentHeight);
            }
            return 0;
        }
        case WM_DESTROY:
            PostQuitMessage(0);
            winrt::uninit_apartment();
            return 0;
    }
    return DefWindowProc(hwnd, msg, wp, lp);
}

void parseModSettings() {
    g_showToast     = Wh_GetIntSetting(L"show-toast"    ) != 0;
    if (!g_showToast) return;

    g_scale = Wh_GetIntSetting(L"scale");
    if (g_scale < 1) g_scale = 1;

    g_defaultFont   = Wh_GetIntSetting(L"default-font"  ) != 0;
    g_showAnimation = Wh_GetIntSetting(L"show-animation") != 0;

    g_customFont = StripSurroundingQuotes(WStringToString(Wh_GetStringSetting(L"custom-font")));

    CreateThread(nullptr, 0, FontDownloadThread, nullptr, 0, nullptr);
}

DWORD WINAPI OverlayThread(LPVOID) {
    winrt::init_apartment(winrt::apartment_type::multi_threaded);

    g_hInstance = GetModHInstance();

    std::string nowPlayingDataUri = "data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAKAAAAAgAgMAAADoEZtmAAAADFBMVEUAAAAAAABVVVUhISGc8ziTAAAAAnRSTlP/AOW3MEoAAAA/SURBVHjaYwhgIAqIMnitIgq0MmgRp3Ahg9Z/osAKBm3iFL4YVTiqcFThkFFIdAFAbNmzgiGI2LInhIEowAoASfJc+vuz7jYAAAAASUVORK5CYII=";
    auto nowPlayingBytes = DecodeDataUri(nowPlayingDataUri);
    g_image = LoadImageFromMemory(nowPlayingBytes, &g_toastStream);

    int height = g_baseHeight * g_scale;
    
    std::string musicNotesDataUri = "data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABAAAACACAMAAAA1SxkQAAAAElBMVEUAAACSkpJoaGjz8/NRUVGzs7NULgk1AAAAAXRSTlMAQObYZgAAAMNJREFUeNrFlUEKBCEQAxPX/v+X97JrqAFBYaA9NTFzSE1odXTKT2FYFDRpc2wRYDNtnxq0jTlpk0SbJNr0tAm2d0+Zc9L+59IkIEu7+BHWd4Dym5M2kRriX5eknL+/LhM/gBIfUMIDQngkdKaEvojfX5K4Ex/EDHdyOyUhoKua9JeEl+blDQ8ywCUAXfFoAAS9NFMeIz6JeakRtoD2u7UfCuM7WDJluWB/nPHg/jD2R0NJTt9bHJSE8cuIf/XeUni/JF9QzgRRI8k/JAAAAABJRU5ErkJggg==";
    auto musicNotesBytes = DecodeDataUri(musicNotesDataUri);
    g_musicNotes = LoadImageFromMemory(musicNotesBytes, &g_notesStream);
    if (g_musicNotes->GetLastStatus() == Ok) {
        g_musicNotes_frameWidth  = (int)g_musicNotes->GetWidth();
        g_musicNotes_frameHeight = (int)g_musicNotes->GetHeight() / g_musicNotes_frameCount;
    }

    WNDCLASS wc = {};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = g_hInstance;
    wc.lpszClassName = L"WhCornerOverlayClass";

    if (!RegisterClass(&wc)) {
        // Only tolerate "already registered" (e.g. a previous unload didn't
        // run cleanly). Anything else means something's actually wrong.
        if (GetLastError() != ERROR_CLASS_ALREADY_EXISTS) {
            return 1;
        }
    }

    g_hwnd = CreateWindowEx(
        WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW |
            WS_EX_NOACTIVATE | WS_EX_TOPMOST,
        wc.lpszClassName, L"", WS_POPUP,
        0, 0, g_baseWidth * g_scale, height,
        nullptr, nullptr, g_hInstance, nullptr
    );

    HDC hdcTemp = GetDC(g_hwnd);
    InitSystemFont(hdcTemp);
    ReleaseDC(g_hwnd, hdcTemp);

    CreateThread(nullptr, 0, FontDownloadThread, nullptr, 0, nullptr);

    ShowWindow(g_hwnd, SW_SHOWNOACTIVATE);
    RenderAndUpdateWindow(g_hwnd, height);
    SetTimer(g_hwnd, kRenderTimerId, kRenderIntervalMs, nullptr);

    MSG msg;
    while (!g_stop && GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return 0;
}

BOOL Wh_ModInit() {
    GdiplusStartupInput gdiplusStartupInput;
    GdiplusStartup(&g_gdiplusToken, &gdiplusStartupInput, nullptr);

    parseModSettings();

    g_thread = CreateThread(nullptr, 0, OverlayThread, nullptr, 0, nullptr);
    return TRUE;
}

void Wh_ModUninit() {
    g_stop = true;
    if (g_hwnd) PostMessage(g_hwnd, WM_CLOSE, 0, 0);
    if (g_thread) {
        WaitForSingleObject(g_thread, 2000);
        CloseHandle(g_thread);
    }

    if (g_hwnd) KillTimer(g_hwnd, musicNotes_kAnimTimerId);
    g_hwnd = nullptr;

    UnregisterClass(L"WhCornerOverlayClass", g_hInstance); // new — cleans up so reload doesn't collide

    delete g_image;
    delete g_musicNotes;
    delete g_systemFontFamily;
    delete g_customFontFamily;
    delete g_privateFonts;
    GdiplusShutdown(g_gdiplusToken);
}

void Wh_ModSettingsChanged() {
    parseModSettings();

    if (g_hwnd) PostMessage(g_hwnd, WM_APP, 0, 0);
}
