// ==WindhawkMod==
// @id              win10-dark-search-box
// @name            Dark Taskbar Search Box (Windows 10)
// @description     Paints the Windows 10 taskbar search box dark (1507-style) while Windows stays in Light mode
// @version         0.3
// @author          winVisturn
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lgdi32 -lcomctl32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*...*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*...*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>
#include <commctrl.h>
#include <algorithm>
#include <mutex>
#include <string>
#include <vector>

static const wchar_t kSearchClass[] = L"TrayDummySearchControl";
static const wchar_t kDefaultText[] = L"Type here to search";

struct Settings {
    COLORREF bg = RGB(0x2B, 0x2B, 0x2B);
    COLORREF text = RGB(0xBF, 0xBF, 0xBF);
    std::wstring placeholder;
    bool showIcon = false;
};

static Settings g_settings;
static std::mutex g_mutex;
static std::vector<HWND> g_subclassed;
static bool g_foundAtInit = false;

// ---------- helpers ----------

static COLORREF ParseColor(PCWSTR s, COLORREF fallback) {
    if (!s) return fallback;
    if (*s == L'#') s++;
    if (wcslen(s) != 6) return fallback;
    wchar_t* end = nullptr;
    unsigned long v = wcstoul(s, &end, 16);
    if (!end || *end) return fallback;
    return RGB((v >> 16) & 0xFF, (v >> 8) & 0xFF, v & 0xFF);
}

static void LoadSettings() {
    Settings s;
    PCWSTR v = Wh_GetStringSetting(L"backgroundColor");
    s.bg = ParseColor(v, s.bg);
    Wh_FreeStringSetting(v);

    v = Wh_GetStringSetting(L"textColor");
    s.text = ParseColor(v, s.text);
    Wh_FreeStringSetting(v);

    v = Wh_GetStringSetting(L"placeholderText");
    s.placeholder = v ? v : L"";
    Wh_FreeStringSetting(v);

    s.showIcon = Wh_GetIntSetting(L"showIcon") != 0;

    std::lock_guard<std::mutex> lock(g_mutex);
    g_settings = s;
}

static Settings GetSettings() {
    std::lock_guard<std::mutex> lock(g_mutex);
    return g_settings;
}

static bool IsClass(HWND hwnd, PCWSTR cls) {
    wchar_t buf[256];
    return GetClassNameW(hwnd, buf, 256) && _wcsicmp(buf, cls) == 0;
}

static HWND FindSearchRoot(HWND hwnd) {
    HWND desktop = GetDesktopWindow();
    for (HWND h = hwnd; h && h != desktop; h = GetAncestor(h, GA_PARENT)) {
        if (IsClass(h, kSearchClass)) return h;
    }
    return nullptr;
}

static HFONT MakeFont(int px, PCWSTR face) {
    return CreateFontW(-px, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                       DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                       CLEARTYPE_QUALITY, DEFAULT_PITCH, face);
}

// ---------- painting ----------
// The whole search box is drawn as one picture at the size of the root
// window. Each window (root or child) then copies only the part of that
// picture it covers, so the text flows across child windows seamlessly.

static void PaintDark(HWND hwnd) {
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hwnd, &ps);
    if (!hdc) return;

    HWND root = FindSearchRoot(hwnd);
    if (!root) root = hwnd;

    RECT rc, rr;
    GetClientRect(hwnd, &rc);
    GetClientRect(root, &rr);
    int w = rc.right - rc.left;
    int h = rc.bottom - rc.top;
    int rw = rr.right - rr.left;
    int rh = rr.bottom - rr.top;
    if (w <= 0 || h <= 0 || rw <= 0 || rh <= 0) {
        EndPaint(hwnd, &ps);
        return;
    }

    // Where this window sits inside the root.
    POINT origin = {0, 0};
    if (hwnd != root) MapWindowPoints(hwnd, root, &origin, 1);

    Settings s = GetSettings();
    int dpi = GetDeviceCaps(hdc, LOGPIXELSY);
    if (dpi <= 0) dpi = 96;

    HDC mem = CreateCompatibleDC(hdc);
    // 32-bit DIB so we can force every pixel fully opaque. GDI leaves the
    // alpha byte at 0, which lets the taskbar's translucent (often bluish)
    // background bleed through and tint the colors.
    BITMAPINFO bi = {};
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = rw;
    bi.bmiHeader.biHeight = -rh;  // top-down
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;
    void* bits = nullptr;
    HBITMAP bmp = CreateDIBSection(hdc, &bi, DIB_RGB_COLORS, &bits, nullptr, 0);
    if (!bmp) {
        DeleteDC(mem);
        EndPaint(hwnd, &ps);
        return;
    }
    HGDIOBJ oldBmp = SelectObject(mem, bmp);

    HBRUSH br = CreateSolidBrush(s.bg);
    FillRect(mem, &rr, br);
    DeleteObject(br);

    SetBkMode(mem, TRANSPARENT);
    SetTextColor(mem, s.text);

    int pad = MulDiv(10, dpi, 96);
    RECT textRect = rr;
    textRect.left = pad;
    textRect.right -= pad;

    if (s.showIcon) {
        HFONT iconFont = MakeFont(MulDiv(14, dpi, 96), L"Segoe MDL2 Assets");
        HGDIOBJ old = SelectObject(mem, iconFont);
        RECT ir = rr;
        ir.left = pad;
        ir.right = pad + MulDiv(16, dpi, 96);
        DrawTextW(mem, L"", 1, &ir,
                  DT_SINGLELINE | DT_VCENTER | DT_CENTER | DT_NOPREFIX);
        SelectObject(mem, old);
        DeleteObject(iconFont);
        textRect.left = ir.right + MulDiv(8, dpi, 96);
    }

    std::wstring text =
        s.placeholder.empty() ? std::wstring(kDefaultText) : s.placeholder;
    HFONT textFont = MakeFont(MulDiv(12, dpi, 96), L"Segoe UI");
    HGDIOBJ oldFont = SelectObject(mem, textFont);
    DrawTextW(mem, text.c_str(), (int)text.size(), &textRect,
              DT_SINGLELINE | DT_VCENTER | DT_LEFT | DT_END_ELLIPSIS |
                  DT_NOPREFIX);
    SelectObject(mem, oldFont);
    DeleteObject(textFont);

    GdiFlush();
    DWORD* px = (DWORD*)bits;
    for (int i = 0, n = rw * rh; i < n; i++) px[i] |= 0xFF000000;

    BitBlt(hdc, 0, 0, w, h, mem, origin.x, origin.y, SRCCOPY);
    SelectObject(mem, oldBmp);
    DeleteObject(bmp);
    DeleteDC(mem);
    EndPaint(hwnd, &ps);
}

static LRESULT CALLBACK SubclassProc(HWND hwnd, UINT msg, WPARAM wParam,
                                     LPARAM lParam, DWORD_PTR) {
    switch (msg) {
        case WM_ERASEBKGND:
            return 1;
        case WM_PAINT:
            PaintDark(hwnd);
            return 0;
        case WM_NCDESTROY: {
            std::lock_guard<std::mutex> lock(g_mutex);
            g_subclassed.erase(
                std::remove(g_subclassed.begin(), g_subclassed.end(), hwnd),
                g_subclassed.end());
            break;
        }
    }
    return DefSubclassProc(hwnd, msg, wParam, lParam);
}

// ---------- attaching ----------

static void SubclassOne(HWND hwnd) {
    {
        std::lock_guard<std::mutex> lock(g_mutex);
        if (std::find(g_subclassed.begin(), g_subclassed.end(), hwnd) !=
            g_subclassed.end())
            return;
        g_subclassed.push_back(hwnd);
    }
    if (WindhawkUtils::SetWindowSubclassFromAnyThread(hwnd, SubclassProc, 0)) {
        wchar_t cls[128] = {};
        GetClassNameW(hwnd, cls, 128);
        Wh_Log(L"Subclassed %p (%s)", hwnd, cls);
    } else {
        Wh_Log(L"Failed to subclass %p", hwnd);
    }
    InvalidateRect(hwnd, nullptr, TRUE);
}

static BOOL CALLBACK SubclassChildProc(HWND hwnd, LPARAM) {
    SubclassOne(hwnd);
    return TRUE;
}

static void HandleRoot(HWND root) {
    SubclassOne(root);
    EnumChildWindows(root, SubclassChildProc, 0);
}

static void LogTree(HWND parent, int depth) {
    for (HWND c = GetWindow(parent, GW_CHILD); c; c = GetWindow(c, GW_HWNDNEXT)) {
        wchar_t cls[128] = {}, txt[128] = {};
        GetClassNameW(c, cls, 128);
        GetWindowTextW(c, txt, 128);
        RECT r;
        GetWindowRect(c, &r);
        Wh_Log(L"%*s%s \"%s\" %ldx%ld visible=%d", depth * 2, L"", cls, txt,
               r.right - r.left, r.bottom - r.top, IsWindowVisible(c) ? 1 : 0);
        if (depth < 4) LogTree(c, depth + 1);
    }
}

static BOOL CALLBACK FindSearchInTray(HWND hwnd, LPARAM) {
    if (IsClass(hwnd, kSearchClass)) {
        g_foundAtInit = true;
        HandleRoot(hwnd);
    }
    return TRUE;
}

static BOOL CALLBACK EnumTopLevelProc(HWND hwnd, LPARAM) {
    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    if (pid != GetCurrentProcessId()) return TRUE;
    if (IsClass(hwnd, L"Shell_TrayWnd")) {
        Wh_Log(L"Taskbar tree:");
        LogTree(hwnd, 1);
        EnumChildWindows(hwnd, FindSearchInTray, 0);
    }
    return TRUE;
}

// ---------- CreateWindowExW hook (catches the box after Explorer restarts) ----------

using CreateWindowExW_t = decltype(&CreateWindowExW);
static CreateWindowExW_t CreateWindowExW_Original;

static HWND WINAPI CreateWindowExW_Hook(DWORD exStyle, LPCWSTR className,
                                        LPCWSTR windowName, DWORD style, int x,
                                        int y, int width, int height,
                                        HWND parent, HMENU menu,
                                        HINSTANCE instance, LPVOID param) {
    HWND hwnd = CreateWindowExW_Original(exStyle, className, windowName, style,
                                         x, y, width, height, parent, menu,
                                         instance, param);
    if (hwnd && parent) {
        HWND root = FindSearchRoot(hwnd);
        if (root == hwnd)
            HandleRoot(hwnd);
        else if (root)
            SubclassOne(hwnd);
    }
    return hwnd;
}

// ---------- Windhawk entry points ----------

BOOL Wh_ModInit() {
    Wh_Log(L"Init");
    LoadSettings();
    Wh_SetFunctionHook((void*)CreateWindowExW, (void*)CreateWindowExW_Hook,
                       (void**)&CreateWindowExW_Original);
    return TRUE;
}

void Wh_ModAfterInit() {
    EnumWindows(EnumTopLevelProc, 0);
    if (!g_foundAtInit)
        Wh_Log(L"No %s window found in the taskbar", kSearchClass);
}

static void RedrawAll(const std::vector<HWND>& windows) {
    for (HWND h : windows) {
        if (IsWindow(h))
            RedrawWindow(h, nullptr, nullptr,
                         RDW_INVALIDATE | RDW_ERASE | RDW_ALLCHILDREN | RDW_FRAME);
    }
}

void Wh_ModSettingsChanged() {
    LoadSettings();
    std::vector<HWND> copy;
    {
        std::lock_guard<std::mutex> lock(g_mutex);
        copy = g_subclassed;
    }
    RedrawAll(copy);
}

void Wh_ModUninit() {
    std::vector<HWND> copy;
    {
        std::lock_guard<std::mutex> lock(g_mutex);
        copy = g_subclassed;
        g_subclassed.clear();
    }
    for (HWND h : copy) {
        if (IsWindow(h))
            WindhawkUtils::RemoveWindowSubclassFromAnyThread(h, SubclassProc);
    }
    RedrawAll(copy);
    Wh_Log(L"Uninit");
}
