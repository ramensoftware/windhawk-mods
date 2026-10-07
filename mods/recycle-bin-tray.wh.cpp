// ==WindhawkMod==
// @id              recycle-bin-tray-explorer
// @name            Recycle Bin Tray Icon (Explorer)
// @description     Adds a working Recycle Bin icon to the notification area (tray) next to the clock
// @version         1.0.0
// @author          you
// @include         explorer.exe
// @compilerOptions -ld2d1 -ld3d11 -ldxgi -lole32 -lshell32 -lshlwapi -luser32 -lgdi32 -ladvapi32 -luuid
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Recycle Bin Tray Icon

Adds a Recycle Bin icon to the notification area next to the clock.

* The icon changes automatically between *empty* and *full* (checked every 2 s).
* The tooltip shows the number of items and their total size.
* Left click opens the Recycle Bin (single or double click, see settings).
* Right click shows a menu: **Open** / **Empty Recycle Bin**.
* Custom `.ico` or `.svg` icons for empty / full state, with separate
  icons for a light taskbar (fallback chain: light icon -> normal icon -> system icon).
* Environment variables such as `%USERPROFILE%` are allowed in paths.

On Windows 11 the new icon may land in the overflow area (^) - drag it
next to the clock once, Windows remembers the position.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- confirmBeforeEmptying: true
  $name: Confirm before emptying
  $description: Show the standard Windows confirmation dialog before emptying the Recycle Bin.
- openOnSingleClick: true
  $name: Open on single click
  $description: Open the Recycle Bin with a single left click instead of a double-click.
- customEmptyIcon: ""
  $name: Custom empty icon
  $description: Full path to a custom .ico or .svg file shown when the Recycle Bin is empty. Leave empty to use the system icon.
- customFullIcon: ""
  $name: Custom full icon
  $description: Full path to a custom .ico or .svg file shown when the Recycle Bin is not empty. Leave empty to use the system icon.
- customEmptyIconLight: ""
  $name: Custom empty icon (light taskbar)
  $description: Full path to a dark .ico or .svg file shown when the Recycle Bin is empty and Windows uses a light taskbar. Leave empty to fall back to the icon above.
- customFullIconLight: ""
  $name: Custom full icon (light taskbar)
  $description: Full path to a dark .ico or .svg file shown when the Recycle Bin is not empty and Windows uses a light taskbar. Leave empty to fall back to the icon above.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <shellapi.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <d3d11.h>
#include <dxgi.h>
#include <d2d1_3.h>

#include <algorithm>
#include <atomic>
#include <cstdarg>
#include <cstdio>
#include <string>
#include <vector>

// ---------------------------------------------------------------------------
// Minimal COM smart pointer
// ---------------------------------------------------------------------------
template <class T>
class Com {
    T* p = nullptr;

   public:
    Com() = default;
    ~Com() { reset(); }
    Com(const Com&) = delete;
    Com& operator=(const Com&) = delete;
    void reset() {
        if (p) {
            p->Release();
            p = nullptr;
        }
    }
    T** put() {
        reset();
        return &p;
    }
    void attach(T* x) {
        reset();
        p = x;
    }
    T* get() const { return p; }
    T* operator->() const { return p; }
    explicit operator bool() const { return p != nullptr; }
};

// ---------------------------------------------------------------------------
// Globals
// ---------------------------------------------------------------------------
constexpr UINT WM_TRAY = WM_APP + 1;
constexpr UINT WM_EMPTY_DONE = WM_APP + 2;
constexpr UINT WM_SETTINGS = WM_APP + 3;
constexpr UINT_PTR TIMER_POLL = 1;
constexpr UINT TRAY_ID = 1;

struct Settings {
    bool confirm = true;
    bool singleClick = true;
    std::wstring emptyIcon, fullIcon, emptyIconLight, fullIconLight;
};

struct Texts {
    const wchar_t* open;
    const wchar_t* empty;
    const wchar_t* tipEmpty;
    const wchar_t* tipItems;
};

Settings g_s;
Texts g_t;
HWND g_hwnd = nullptr;
UINT g_taskbarCreated = 0;
HICON g_icons[4] = {};  // 0 empty, 1 full, 2 empty(light), 3 full(light)
std::wstring g_binName = L"Recycle Bin";

bool g_added = false;
bool g_lastFull = false;
bool g_lastLight = false;
std::wstring g_lastTip;
long long g_items = 0;
std::atomic<bool> g_emptying{false};
DWORD g_lastOpenTick = 0;
int g_tick = 0;

// Diagnostic log: %TEMP%\recycle-bin-tray.log (Windhawk's own log is not visible during logon).
static void FileLog(PCWSTR fmt, ...) {
    WCHAR dir[MAX_PATH];
    DWORD n = GetTempPathW(MAX_PATH, dir);
    if (!n || n >= MAX_PATH) return;
    std::wstring p = std::wstring(dir) + L"recycle-bin-tray.log";

    WCHAR msg[1024];
    va_list ap;
    va_start(ap, fmt);
    _vsnwprintf(msg, 1023, fmt, ap);
    va_end(ap);
    msg[1023] = 0;

    SYSTEMTIME st;
    GetLocalTime(&st);
    WCHAR line[1200];
    _snwprintf(line, 1199, L"%02d:%02d:%02d.%03d [pid %lu] %ls\r\n", st.wHour, st.wMinute, st.wSecond,
               st.wMilliseconds, GetCurrentProcessId(), msg);
    line[1199] = 0;

    char utf8[4000];
    int len = WideCharToMultiByte(CP_UTF8, 0, line, -1, utf8, sizeof(utf8), nullptr, nullptr);
    if (len <= 1) return;
    HANDLE f = CreateFileW(p.c_str(), FILE_APPEND_DATA, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_ALWAYS,
                           FILE_ATTRIBUTE_NORMAL, nullptr);
    if (f == INVALID_HANDLE_VALUE) return;
    DWORD wr = 0;
    WriteFile(f, utf8, (DWORD)(len - 1), &wr, nullptr);
    CloseHandle(f);
}

#define WIDEN2(x) L##x
#define WIDEN(x) WIDEN2(x)
#define SVG_CHECK(expr)                                                                          \
    do {                                                                                         \
        HRESULT _h = (expr);                                                                     \
        if (FAILED(_h)) {                                                                        \
            FileLog(L"SVG step failed: %ls hr=0x%08lX", WIDEN(#expr), (unsigned long)_h);        \
            return nullptr;                                                                      \
        }                                                                                        \
    } while (0)

bool g_loadFailed = false;  // a configured custom icon could not be loaded yet -> retry later
int g_retryTicks = 0;

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------
static std::wstring Trim(std::wstring s) {
    const wchar_t* ws = L" \t\r\n\"";
    size_t a = s.find_first_not_of(ws);
    if (a == std::wstring::npos) return L"";
    size_t b = s.find_last_not_of(ws);
    return s.substr(a, b - a + 1);
}

static std::wstring GetStr(PCWSTR name) {
    PCWSTR s = Wh_GetStringSetting(name);
    std::wstring r = s ? s : L"";
    Wh_FreeStringSetting(s);
    return Trim(r);
}

static std::wstring ExpandPath(const std::wstring& in) {
    DWORD n = ExpandEnvironmentStringsW(in.c_str(), nullptr, 0);
    if (!n) return in;
    std::wstring out(n, L'\0');
    n = ExpandEnvironmentStringsW(in.c_str(), &out[0], n);
    if (!n) return in;
    out.resize(n - 1);
    return out;
}

static void LoadTexts() {
    switch (PRIMARYLANGID(GetUserDefaultUILanguage())) {
        case LANG_RUSSIAN:
            g_t = {L"Открыть", L"Очистить корзину", L"Пусто", L"Элементов"};
            break;
        case LANG_UKRAINIAN:
            g_t = {L"Відкрити", L"Очистити кошик", L"Порожньо", L"Елементів"};
            break;
        default:
            g_t = {L"Open", L"Empty Recycle Bin", L"Empty", L"Items"};
    }
}

static void LoadSettings() {
    g_s.confirm = Wh_GetIntSetting(L"confirmBeforeEmptying") != 0;
    g_s.singleClick = Wh_GetIntSetting(L"openOnSingleClick") != 0;
    g_s.emptyIcon = GetStr(L"customEmptyIcon");
    g_s.fullIcon = GetStr(L"customFullIcon");
    g_s.emptyIconLight = GetStr(L"customEmptyIconLight");
    g_s.fullIconLight = GetStr(L"customFullIconLight");
    FileLog(L"Settings: empty=[%ls] full=[%ls] emptyLight=[%ls] fullLight=[%ls]", g_s.emptyIcon.c_str(),
            g_s.fullIcon.c_str(), g_s.emptyIconLight.c_str(), g_s.fullIconLight.c_str());
}

static bool IsLightTaskbar() {
    DWORD v = 0, sz = sizeof(v);
    if (RegGetValueW(HKEY_CURRENT_USER,
                     L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
                     L"SystemUsesLightTheme", RRF_RT_REG_DWORD, nullptr, &v, &sz) == ERROR_SUCCESS)
        return v != 0;
    return false;
}

static int GetTrayIconSize() {
    HMODULE u = GetModuleHandleW(L"user32.dll");
    typedef UINT(WINAPI * GetDpiForSystem_t)();
    typedef int(WINAPI * GetSystemMetricsForDpi_t)(int, UINT);
    auto pDpi = reinterpret_cast<GetDpiForSystem_t>((void*)GetProcAddress(u, "GetDpiForSystem"));
    auto pMet = reinterpret_cast<GetSystemMetricsForDpi_t>((void*)GetProcAddress(u, "GetSystemMetricsForDpi"));
    if (pDpi && pMet) return pMet(SM_CXSMICON, pDpi());
    return GetSystemMetrics(SM_CXSMICON);
}

static std::wstring GetBinDisplayName() {
    std::wstring r = L"Recycle Bin";
    LPITEMIDLIST pidl = nullptr;
    if (SUCCEEDED(SHGetSpecialFolderLocation(nullptr, CSIDL_BITBUCKET, &pidl)) && pidl) {
        PWSTR name = nullptr;
        if (SUCCEEDED(SHGetNameFromIDList(pidl, SIGDN_NORMALDISPLAY, &name)) && name) {
            r = name;
            CoTaskMemFree(name);
        }
        CoTaskMemFree(pidl);
    }
    return r;
}

// ---------------------------------------------------------------------------
// SVG -> HICON (Direct2D 1.1 + SVG support, Windows 10 1703+)
// ---------------------------------------------------------------------------
static HICON LoadSvgIcon(const std::wstring& path, int size) {
    HANDLE f = CreateFileW(path.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, 0, nullptr);
    if (f == INVALID_HANDLE_VALUE) {
        FileLog(L"SVG: CreateFile failed, err=%lu", GetLastError());
        return nullptr;
    }
    LARGE_INTEGER fs{};
    std::vector<BYTE> data;
    if (GetFileSizeEx(f, &fs) && fs.QuadPart > 0 && fs.QuadPart < 16 * 1024 * 1024) {
        data.resize((size_t)fs.QuadPart);
        DWORD rd = 0;
        if (!ReadFile(f, data.data(), (DWORD)data.size(), &rd, nullptr) || rd != data.size()) data.clear();
    }
    CloseHandle(f);
    if (data.empty()) {
        FileLog(L"SVG: file empty/too large/unreadable");
        return nullptr;
    }

    Com<IStream> stream;
    stream.attach(SHCreateMemStream(data.data(), (UINT)data.size()));
    if (!stream) {
        FileLog(L"SVG: SHCreateMemStream failed");
        return nullptr;
    }

    Com<ID3D11Device> d3d;
    HRESULT hr = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, D3D11_CREATE_DEVICE_BGRA_SUPPORT,
                                   nullptr, 0, D3D11_SDK_VERSION, d3d.put(), nullptr, nullptr);
    if (FAILED(hr))
        hr = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_WARP, nullptr, D3D11_CREATE_DEVICE_BGRA_SUPPORT, nullptr, 0,
                               D3D11_SDK_VERSION, d3d.put(), nullptr, nullptr);
    SVG_CHECK(hr);

    Com<IDXGIDevice> dxgi;
    SVG_CHECK(d3d->QueryInterface(__uuidof(IDXGIDevice), (void**)dxgi.put()));

    Com<ID2D1Factory1> factory;
    SVG_CHECK(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, __uuidof(ID2D1Factory1), nullptr,
                                 (void**)factory.put()));

    Com<ID2D1Device> dev;
    SVG_CHECK(factory->CreateDevice(dxgi.get(), dev.put()));

    Com<ID2D1DeviceContext> ctx;
    SVG_CHECK(dev->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, ctx.put()));

    Com<ID2D1DeviceContext5> ctx5;
    SVG_CHECK(ctx->QueryInterface(__uuidof(ID2D1DeviceContext5), (void**)ctx5.put()));

    D2D1_BITMAP_PROPERTIES1 bp{};
    bp.pixelFormat.format = DXGI_FORMAT_B8G8R8A8_UNORM;
    bp.pixelFormat.alphaMode = D2D1_ALPHA_MODE_PREMULTIPLIED;
    bp.dpiX = bp.dpiY = 96.0f;
    bp.bitmapOptions = D2D1_BITMAP_OPTIONS_TARGET;
    D2D1_SIZE_U szu = {(UINT32)size, (UINT32)size};

    Com<ID2D1Bitmap1> target;
    SVG_CHECK(ctx->CreateBitmap(szu, nullptr, 0, &bp, target.put()));
    ctx->SetTarget(target.get());

    Com<ID2D1SvgDocument> svg;
    SVG_CHECK(ctx5->CreateSvgDocument(stream.get(), D2D1_SIZE_F{(float)size, (float)size}, svg.put()));

    // Fit the SVG into size x size with a device-context transform. (Setting width/height through
    // SetAttributeValue had no effect: a 16 px target showed only the empty top-left corner of the
    // full-size drawing.) If the root has no explicit width/height, Direct2D already fits the viewBox
    // to the viewport we passed to CreateSvgDocument, so the base size is simply `size` (scale 1).
    float baseW = 0, baseH = 0;
    Com<ID2D1SvgElement> root;
    svg->GetRoot(root.put());
    if (root) {
        D2D1_SVG_LENGTH lw{}, lh{};
        if (root->IsAttributeSpecified(L"width") &&
            SUCCEEDED(root->GetAttributeValue(L"width", D2D1_SVG_ATTRIBUTE_POD_TYPE_LENGTH, &lw, sizeof(lw))) &&
            lw.units == D2D1_SVG_LENGTH_UNITS_NUMBER)
            baseW = lw.value;
        if (root->IsAttributeSpecified(L"height") &&
            SUCCEEDED(root->GetAttributeValue(L"height", D2D1_SVG_ATTRIBUTE_POD_TYPE_LENGTH, &lh, sizeof(lh))) &&
            lh.units == D2D1_SVG_LENGTH_UNITS_NUMBER)
            baseH = lh.value;
    }
    if (baseW <= 0 || baseH <= 0) baseW = baseH = (float)size;
    float scale = std::min((float)size / baseW, (float)size / baseH);
    float tx = ((float)size - baseW * scale) / 2.0f;
    float ty = ((float)size - baseH * scale) / 2.0f;
    FileLog(L"SVG fit: base=%.1fx%.1f scale=%.4f offset=%.2f,%.2f", baseW, baseH, scale, tx, ty);

    ctx->BeginDraw();
    {
        D2D1::Matrix3x2F m = D2D1::Matrix3x2F::Scale(scale, scale) * D2D1::Matrix3x2F::Translation(tx, ty);
        ctx->SetTransform(&m);
    }
    D2D1_COLOR_F clear{0, 0, 0, 0};
    ctx->Clear(&clear);
    ctx5->DrawSvgDocument(svg.get());
    SVG_CHECK(ctx->EndDraw());

    // Read the pixels back through a CPU-readable staging bitmap.
    D2D1_BITMAP_PROPERTIES1 sp = bp;
    sp.bitmapOptions = D2D1_BITMAP_OPTIONS_CPU_READ | D2D1_BITMAP_OPTIONS_CANNOT_DRAW;
    Com<ID2D1Bitmap1> staging;
    SVG_CHECK(ctx->CreateBitmap(szu, nullptr, 0, &sp, staging.put()));
    SVG_CHECK(staging->CopyFromBitmap(nullptr, target.get(), nullptr));

    D2D1_MAPPED_RECT mapped{};
    SVG_CHECK(staging->Map(D2D1_MAP_OPTIONS_READ, &mapped));

    {   // Diagnostics: what did Direct2D actually render?
        static int dumps = 0;
        long long cntA = 0, cntFull = 0, sr = 0, sg = 0, sb = 0;
        for (int y = 0; y < size; y++) {
            const BYTE* p = (const BYTE*)mapped.bits + (size_t)y * mapped.pitch;
            for (int x = 0; x < size; x++, p += 4) {
                if (p[3]) {
                    cntA++;
                    if (p[3] == 255) cntFull++;
                    sb += p[0];
                    sg += p[1];
                    sr += p[2];
                }
            }
        }
        FileLog(L"SVG pixels: size=%d alpha>0=%lld alpha==255=%lld premultSumRGB=%lld,%lld,%lld", size, cntA, cntFull,
                sr, sg, sb);
        if (dumps < 2) {
            dumps++;
            for (int y = 0; y < size; y++) {
                const BYTE* p = (const BYTE*)mapped.bits + (size_t)y * mapped.pitch;
                WCHAR row[16 * 9 + 8] = L"";
                int pos = 0;
                for (int x = 0; x < size && pos < 16 * 9; x++, p += 4)
                    pos += _snwprintf(row + pos, 10, L"%02X%02X%02X%02X ", p[3], p[2], p[1], p[0]);
                FileLog(L"  row %02d: %ls", y, row);
            }
        }
    }

    BITMAPINFO bi{};
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = size;
    bi.bmiHeader.biHeight = -size;  // top-down
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;
    void* bits = nullptr;
    HBITMAP color = CreateDIBSection(nullptr, &bi, DIB_RGB_COLORS, &bits, nullptr, 0);
    HICON icon = nullptr;
    if (color && bits) {
        for (int y = 0; y < size; y++) {
            const BYTE* s = (const BYTE*)mapped.bits + (size_t)y * mapped.pitch;
            BYTE* d = (BYTE*)bits + (size_t)y * size * 4;
            for (int x = 0; x < size; x++, s += 4, d += 4) {
                int b = s[0], g = s[1], r = s[2], a = s[3];
                if (a > 0 && a < 255) {  // premultiplied -> straight alpha
                    b = std::min(255, b * 255 / a);
                    g = std::min(255, g * 255 / a);
                    r = std::min(255, r * 255 / a);
                }
                d[0] = (BYTE)b;
                d[1] = (BYTE)g;
                d[2] = (BYTE)r;
                d[3] = (BYTE)a;
            }
        }
        // Monochrome AND-mask must be explicitly zeroed (nullptr = uninitialised memory -> black square).
        std::vector<BYTE> maskBits((size_t)((size + 15) / 16) * 2 * size, 0);
        HBITMAP mask = CreateBitmap(size, size, 1, 1, maskBits.data());
        ICONINFO ii{};
        ii.fIcon = TRUE;
        ii.hbmMask = mask;
        ii.hbmColor = color;
        icon = CreateIconIndirect(&ii);
        {
            BITMAP bm{};
            GetObjectW(color, sizeof(bm), &bm);
            FileLog(L"CreateIconIndirect -> %p (color bmBitsPixel=%d, %dx%d)", (void*)icon, (int)bm.bmBitsPixel,
                    (int)bm.bmWidth, (int)bm.bmHeight);
        }
        if (mask) DeleteObject(mask);
    }
    if (color) DeleteObject(color);
    staging->Unmap();
    return icon;
}

// ---------------------------------------------------------------------------
// Icon loading
// ---------------------------------------------------------------------------
static HICON LoadIconFromFile(const std::wstring& raw, int size) {
    if (raw.empty()) return nullptr;
    std::wstring p = ExpandPath(raw);
    if (GetFileAttributesW(p.c_str()) == INVALID_FILE_ATTRIBUTES) {
        Wh_Log(L"Icon file not found: %s", p.c_str());
        FileLog(L"Icon file not found: %ls", p.c_str());
        g_loadFailed = true;
        return nullptr;
    }
    PCWSTR ext = PathFindExtensionW(p.c_str());
    HICON h = nullptr;
    if (ext && _wcsicmp(ext, L".svg") == 0)
        h = LoadSvgIcon(p, size);
    else
        h = (HICON)LoadImageW(nullptr, p.c_str(), IMAGE_ICON, size, size, LR_LOADFROMFILE);
    if (!h) {
        g_loadFailed = true;
        Wh_Log(L"Failed to load icon: %s", p.c_str());
        FileLog(L"Failed to load icon: %ls (%d px)", p.c_str(), size);
    } else {
        FileLog(L"Loaded icon: %ls (%d px)", p.c_str(), size);
    }
    return h;
}

static HICON LoadStockIcon(bool full, int size) {
    SHSTOCKICONINFO sii{};
    sii.cbSize = sizeof(sii);
    SHSTOCKICONID id = full ? SIID_RECYCLERFULL : SIID_RECYCLER;
    if (SUCCEEDED(SHGetStockIconInfo(id, SHGSI_ICONLOCATION, &sii))) {
        HICON h = nullptr;
        UINT n = PrivateExtractIconsW(sii.szPath, sii.iIcon, size, size, &h, nullptr, 1, 0);
        if (n == 1 && h) return h;
    }
    SHSTOCKICONINFO s2{};
    s2.cbSize = sizeof(s2);
    if (SUCCEEDED(SHGetStockIconInfo(id, SHGSI_ICON | SHGSI_SMALLICON, &s2))) return s2.hIcon;
    return nullptr;
}

static HICON Resolve(const std::wstring& a, const std::wstring& b, bool full, int size) {
    HICON h = LoadIconFromFile(a, size);
    if (!h) h = LoadIconFromFile(b, size);
    if (!h) h = LoadStockIcon(full, size);
    return h;
}

// ---------------------------------------------------------------------------
// Tray handling
// ---------------------------------------------------------------------------
static std::wstring BuildTip(long long items, long long bytes) {
    std::wstring t = g_binName;
    t += L"\n";
    if (items <= 0) {
        t += g_t.tipEmpty;
    } else {
        wchar_t sz[64] = L"";
        StrFormatByteSizeW(bytes, sz, ARRAYSIZE(sz));
        t += g_t.tipItems;
        t += L": ";
        t += std::to_wstring(items);
        t += L" (";
        t += sz;
        t += L")";
    }
    return t;
}

static void UpdateTray(bool force) {
    SHQUERYRBINFO qi{};
    qi.cbSize = sizeof(qi);
    long long items = 0, bytes = 0;
    if (SUCCEEDED(SHQueryRecycleBinW(nullptr, &qi))) {
        items = qi.i64NumItems;
        bytes = qi.i64Size;
    }
    g_items = items;
    bool full = items > 0;
    bool light = IsLightTaskbar();
    std::wstring tip = BuildTip(items, bytes);

    if (!force && g_added && full == g_lastFull && light == g_lastLight && tip == g_lastTip) return;

    NOTIFYICONDATAW nid{};
    nid.cbSize = sizeof(nid);
    nid.hWnd = g_hwnd;
    nid.uID = TRAY_ID;
    nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    nid.uCallbackMessage = WM_TRAY;
    nid.hIcon = g_icons[(light ? 2 : 0) + (full ? 1 : 0)];
    lstrcpynW(nid.szTip, tip.c_str(), ARRAYSIZE(nid.szTip));

    if (!g_added) {
        g_added = Shell_NotifyIconW(NIM_ADD, &nid) != FALSE;
        FileLog(L"NIM_ADD -> %d (err %lu), hIcon=%p full=%d light=%d", (int)g_added, GetLastError(),
                (void*)nid.hIcon, (int)full, (int)light);
    } else if (!Shell_NotifyIconW(NIM_MODIFY, &nid)) {
        g_added = Shell_NotifyIconW(NIM_ADD, &nid) != FALSE;
        FileLog(L"NIM_MODIFY failed, re-add -> %d", (int)g_added);
    }
    if (g_added) {
        g_lastFull = full;
        g_lastLight = light;
        g_lastTip = tip;
    }
}

static void ReloadIcons() {
    g_loadFailed = false;
    int size = GetTrayIconSize();
    HICON n[4];
    n[0] = Resolve(g_s.emptyIcon, L"", false, size);
    n[1] = Resolve(g_s.fullIcon, L"", true, size);
    n[2] = Resolve(g_s.emptyIconLight, g_s.emptyIcon, false, size);
    n[3] = Resolve(g_s.fullIconLight, g_s.fullIcon, true, size);

    HICON old[4];
    for (int i = 0; i < 4; i++) {
        old[i] = g_icons[i];
        g_icons[i] = n[i];
    }
    UpdateTray(true);
    FileLog(L"ReloadIcons: size=%d loadFailed=%d icons=%p %p %p %p", size, (int)g_loadFailed, (void*)g_icons[0],
            (void*)g_icons[1], (void*)g_icons[2], (void*)g_icons[3]);
    for (int i = 0; i < 4; i++)
        if (old[i]) DestroyIcon(old[i]);
}

// ---------------------------------------------------------------------------
// Actions
// ---------------------------------------------------------------------------
static void OpenBin() {
    DWORD now = GetTickCount();
    if (g_lastOpenTick && now - g_lastOpenTick < GetDoubleClickTime()) return;  // avoid double open
    g_lastOpenTick = now;
    ShellExecuteW(nullptr, L"open", L"shell:RecycleBinFolder", nullptr, nullptr, SW_SHOWNORMAL);
}

static DWORD WINAPI EmptyThread(LPVOID param) {
    DWORD flags = (DWORD)(ULONG_PTR)param;
    CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    SHEmptyRecycleBinW(nullptr, nullptr, flags);
    CoUninitialize();
    g_emptying = false;
    if (g_hwnd) PostMessageW(g_hwnd, WM_EMPTY_DONE, 0, 0);
    return 0;
}

static void EmptyBin() {
    if (g_emptying.exchange(true)) return;
    DWORD flags = g_s.confirm ? 0 : SHERB_NOCONFIRMATION;
    HANDLE t = CreateThread(nullptr, 0, EmptyThread, (LPVOID)(ULONG_PTR)flags, 0, nullptr);
    if (t)
        CloseHandle(t);
    else
        g_emptying = false;
}

// Make Win32 popup menus follow the system light/dark theme (undocumented uxtheme ordinals,
// the same approach used by many Windhawk mods and Explorer tools).
static void EnableThemedMenus() {
    static HMODULE ux = LoadLibraryW(L"uxtheme.dll");
    if (!ux) return;
    typedef int(WINAPI * SetPreferredAppMode_t)(int);
    typedef BOOL(WINAPI * AllowDarkModeForWindow_t)(HWND, BOOL);
    typedef void(WINAPI * FlushMenuThemes_t)();
    auto setMode = reinterpret_cast<SetPreferredAppMode_t>((void*)GetProcAddress(ux, MAKEINTRESOURCEA(135)));
    auto allowWnd = reinterpret_cast<AllowDarkModeForWindow_t>((void*)GetProcAddress(ux, MAKEINTRESOURCEA(133)));
    auto flush = reinterpret_cast<FlushMenuThemes_t>((void*)GetProcAddress(ux, MAKEINTRESOURCEA(136)));
    if (setMode) setMode(1);  // AllowDark: follow the system setting
    if (allowWnd && g_hwnd) allowWnd(g_hwnd, TRUE);
    if (flush) flush();
}

// Small stock icon -> 32-bit ARGB bitmap usable as a menu item image.
static HBITMAP StockIconBitmap(SHSTOCKICONID id) {
    SHSTOCKICONINFO sii{};
    sii.cbSize = sizeof(sii);
    if (FAILED(SHGetStockIconInfo(id, SHGSI_ICON | SHGSI_SMALLICON, &sii)) || !sii.hIcon) return nullptr;
    int sz = GetSystemMetrics(SM_CXSMICON);
    BITMAPINFO bi{};
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = sz;
    bi.bmiHeader.biHeight = -sz;
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;
    void* bits = nullptr;
    HDC dc = CreateCompatibleDC(nullptr);
    HBITMAP bmp = CreateDIBSection(dc, &bi, DIB_RGB_COLORS, &bits, nullptr, 0);
    if (bmp) {
        HGDIOBJ old = SelectObject(dc, bmp);
        DrawIconEx(dc, 0, 0, sii.hIcon, sz, sz, 0, nullptr, DI_NORMAL);
        SelectObject(dc, old);
    }
    DeleteDC(dc);
    DestroyIcon(sii.hIcon);
    return bmp;
}

static void ShowMenu() {
    HMENU m = CreatePopupMenu();
    if (!m) return;

    HBITMAP bmpOpen = StockIconBitmap(SIID_FOLDEROPEN);
    HBITMAP bmpEmpty = StockIconBitmap(SIID_RECYCLER);

    MENUINFO mi{};
    mi.cbSize = sizeof(mi);
    mi.fMask = MIM_STYLE;
    mi.dwStyle = MNS_CHECKORBMP;
    SetMenuInfo(m, &mi);

    MENUITEMINFOW it{};
    it.cbSize = sizeof(it);
    it.fMask = MIIM_ID | MIIM_STRING | MIIM_STATE | MIIM_BITMAP;

    it.wID = 1;
    it.dwTypeData = const_cast<LPWSTR>(g_t.open);
    it.fState = MFS_ENABLED;
    it.hbmpItem = bmpOpen;
    InsertMenuItemW(m, 0, TRUE, &it);

    it.wID = 2;
    it.dwTypeData = const_cast<LPWSTR>(g_t.empty);
    it.fState = (g_items > 0 && !g_emptying) ? MFS_ENABLED : MFS_GRAYED;
    it.hbmpItem = bmpEmpty;
    InsertMenuItemW(m, 1, TRUE, &it);

    SetMenuDefaultItem(m, 1, FALSE);

    POINT pt;
    GetCursorPos(&pt);
    SetForegroundWindow(g_hwnd);
    int cmd = TrackPopupMenu(m, TPM_RETURNCMD | TPM_RIGHTBUTTON | TPM_BOTTOMALIGN | TPM_LEFTALIGN, pt.x, pt.y, 0,
                             g_hwnd, nullptr);
    PostMessageW(g_hwnd, WM_NULL, 0, 0);
    DestroyMenu(m);
    if (bmpOpen) DeleteObject(bmpOpen);
    if (bmpEmpty) DeleteObject(bmpEmpty);

    if (cmd == 1) {
        g_lastOpenTick = 0;
        OpenBin();
    } else if (cmd == 2) {
        EmptyBin();
    }
}

// ---------------------------------------------------------------------------
// Window procedure
// ---------------------------------------------------------------------------
static LRESULT CALLBACK WndProc(HWND h, UINT m, WPARAM w, LPARAM l) {
    if (g_taskbarCreated && m == g_taskbarCreated) {  // Explorer restarted
        FileLog(L"TaskbarCreated");
        g_added = false;
        UpdateTray(true);
        return 0;
    }
    switch (m) {
        case WM_TRAY:
            switch (l) {
                case WM_LBUTTONUP:
                    if (g_s.singleClick) OpenBin();
                    break;
                case WM_LBUTTONDBLCLK:
                    if (!g_s.singleClick) OpenBin();
                    break;
                case WM_RBUTTONUP:
                    ShowMenu();
                    break;
            }
            return 0;
        case WM_TIMER:
            if (w == TIMER_POLL) {
                // Custom icon not loadable yet (drive/D2D not ready right after logon): retry for ~5 min.
                g_tick++;
                if (g_loadFailed && g_retryTicks < 150) {
                    g_retryTicks++;
                    ReloadIcons();
                } else if (g_tick == 5 || g_tick == 30) {
                    FileLog(L"Forced icon reload (tick %d)", g_tick);
                    ReloadIcons();
                } else {
                    UpdateTray(false);
                }
            }
            return 0;
        case WM_EMPTY_DONE:
            UpdateTray(false);
            return 0;
        case WM_SETTINGS:
            LoadSettings();
            g_retryTicks = 0;
            ReloadIcons();
            return 0;
        case WM_SETTINGCHANGE:
            if (l && lstrcmpiW((PCWSTR)l, L"ImmersiveColorSet") == 0) UpdateTray(true);
            return 0;
        case WM_DPICHANGED:
        case WM_DISPLAYCHANGE:
            ReloadIcons();
            return 0;
        case WM_CLOSE:
            DestroyWindow(h);
            return 0;
        case WM_DESTROY: {
            KillTimer(h, TIMER_POLL);
            NOTIFYICONDATAW nid{};
            nid.cbSize = sizeof(nid);
            nid.hWnd = h;
            nid.uID = TRAY_ID;
            Shell_NotifyIconW(NIM_DELETE, &nid);
            g_added = false;
            PostQuitMessage(0);
            return 0;
        }
    }
    return DefWindowProcW(h, m, w, l);
}

// ---------------------------------------------------------------------------
// Windhawk entry points (runs inside explorer.exe; one instance per session)
// ---------------------------------------------------------------------------
static HANDLE g_thread = nullptr;
static HANDLE g_ready = nullptr;

static DWORD WINAPI UiThread(LPVOID) {
    FileLog(L"UI thread started");
    CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);

    // Per-monitor DPI awareness for correct icon size (ignored on old Windows).
    typedef HANDLE(WINAPI * SetCtx_t)(HANDLE);
    auto pSetCtx = reinterpret_cast<SetCtx_t>(
        (void*)GetProcAddress(GetModuleHandleW(L"user32.dll"), "SetThreadDpiAwarenessContext"));
    if (pSetCtx) pSetCtx((HANDLE)-4);  // DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2

    LoadTexts();
    LoadSettings();
    g_binName = GetBinDisplayName();

    WNDCLASSW wc{};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = GetModuleHandleW(nullptr);
    wc.lpszClassName = L"WindhawkRecycleBinTrayWnd";
    RegisterClassW(&wc);

    g_taskbarCreated = RegisterWindowMessageW(L"TaskbarCreated");

    // Normal hidden top-level window (message-only windows do not get broadcasts).
    g_hwnd = CreateWindowExW(WS_EX_TOOLWINDOW, wc.lpszClassName, L"", WS_POPUP, 0, 0, 0, 0, nullptr, nullptr,
                             wc.hInstance, nullptr);
    if (!g_hwnd) {
        Wh_Log(L"CreateWindowEx failed: %lu", GetLastError());
        SetEvent(g_ready);
        CoUninitialize();
        return 1;
    }

    ReloadIcons();
    SetTimer(g_hwnd, TIMER_POLL, 2000, nullptr);
    SetEvent(g_ready);

    MSG msg;
    while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    for (auto& h : g_icons) {
        if (h) DestroyIcon(h);
        h = nullptr;
    }
    g_hwnd = nullptr;
    UnregisterClassW(wc.lpszClassName, wc.hInstance);
    CoUninitialize();
    return 0;
}

static HANDLE g_instanceMutex = nullptr;

BOOL Wh_ModInit() {
    Wh_Log(L"Recycle Bin Tray Icon (Explorer): init");
    // Several explorer.exe processes can exist; only one may own the tray icon.
    g_instanceMutex = CreateMutexW(nullptr, FALSE, L"Local\\WindhawkRecycleBinTrayExplorer");
    if (!g_instanceMutex) return FALSE;
    if (GetLastError() == ERROR_ALREADY_EXISTS) {
        CloseHandle(g_instanceMutex);
        g_instanceMutex = nullptr;
        return FALSE;
    }
    g_ready = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_thread = CreateThread(nullptr, 0, UiThread, nullptr, 0, nullptr);
    if (!g_thread) return FALSE;
    WaitForSingleObject(g_ready, 5000);
    return TRUE;
}

void Wh_ModUninit() {
    if (g_hwnd) PostMessageW(g_hwnd, WM_CLOSE, 0, 0);
    if (g_thread) {
        WaitForSingleObject(g_thread, 5000);
        CloseHandle(g_thread);
        g_thread = nullptr;
    }
    if (g_ready) {
        CloseHandle(g_ready);
        g_ready = nullptr;
    }
    if (g_instanceMutex) {
        CloseHandle(g_instanceMutex);
        g_instanceMutex = nullptr;
    }
}

void Wh_ModSettingsChanged() {
    if (g_hwnd) PostMessageW(g_hwnd, WM_SETTINGS, 0, 0);
}
