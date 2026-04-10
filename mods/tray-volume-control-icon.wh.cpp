// ==WindhawkMod==
// @id              tray-volume-control-icon
// @name            Tray Volume Controller Icon 
// @description     Tray-Symbol to mute and change System Volume. Hovering shows Application-Volume-Popup.
// @version         1.1.0
// @author          Rene Mayer
// @github          https://github.com/renemayer-hb
// @include         explorer.exe
// @architecture    x86-64
// @license         MIT
// @compilerOptions -lshell32 -luser32 -lole32 -luuid -lgdi32 -lshlwapi
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Taskbar Mute Toggle

Tray icon for quick access to audio.

## How to Use
- Left-click: Mute / unmute system sound
- Scroll over the icon: Adjust system volume +/- with each click
- Hover (configurable): A popup with all active programs appears
- Left-click on a program in the popup: Mute / unmute program
- Scroll over a program in the popup: Adjust program volume
- Right-click: Context menu (Mute / Open mixer)
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- volumeStep: 2
  $name: Volume step (%)
  $description: “What percentage per mouse wheel click (1–10)”
- hoverDelay: 500
  $name: Hover delay (ms)
  $description: "How long to hover until Application Popups appear. (100-5000)"
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <windowsx.h>
#include <shellapi.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <mmdeviceapi.h>
#include <endpointvolume.h>
#include <audiopolicy.h>
#include <string>
#include <vector>
#include <thread>

// ---------------------------------------------------------------------------
// Konstanten
// ---------------------------------------------------------------------------
#define WM_TRAYICON        (WM_USER + 101)
#define WM_AUDIO_CHANGED   (WM_USER + 102)
#define WM_POPUP_SCROLL    (WM_APP  + 1)
#define WM_POPUP_HOVER     (WM_APP  + 2)

#define IDM_TOGGLE_MUTE    9100
#define IDM_OPEN_MIXER     9101
#define TRAY_UID           43
#define TIMER_HOVER        1

#define TRAY_CLASS         L"Wh_MuteToggle_v2"
#define POPUP_CLASS        L"Wh_MutePopup_v2"

#define POPUP_W            300
#define POPUP_ITEM_H       46
#define POPUP_PAD          8
#define POPUP_ICON_SZ      22
#define POPUP_BAR_H        3

#define CLR_BG             RGB(28,  28,  28)
#define CLR_ITEM_HOVER     RGB(50,  50,  50)
#define CLR_BORDER         RGB(65,  65,  65)
#define CLR_TEXT           RGB(255, 255, 255)
#define CLR_SUBTEXT        RGB(150, 150, 150)
#define CLR_ACCENT         RGB(0,   120, 212)
#define CLR_MUTED_BAR      RGB(196, 43,  28)
#define CLR_BAR_TRACK      RGB(65,  65,  65)

// ---------------------------------------------------------------------------
// Datenstrukturen
// ---------------------------------------------------------------------------
struct AudioSession {
    std::wstring        name;
    HICON               icon   = nullptr;
    ISimpleAudioVolume* pVol   = nullptr;
    float               volume = 1.f;
    bool                muted  = false;
    DWORD               pid    = 0;
};

// ---------------------------------------------------------------------------
// Globals
// ---------------------------------------------------------------------------
static HWND                  g_msgWnd    = nullptr;
static HWND                  g_popup     = nullptr;
static NOTIFYICONDATAW       g_nid       = {};
static bool                  g_nidAdded  = false;
static std::thread           g_thread;
static HHOOK                 g_mouseHook = nullptr;
static int                   g_volStep   = 2;
static int                   g_hoverDelay = 1000;

static IMMDeviceEnumerator*  g_pEnum     = nullptr;
static IAudioEndpointVolume* g_pVolume   = nullptr;

static std::vector<AudioSession> g_sessions;
static int                   g_hoveredItem = -1;

// ---------------------------------------------------------------------------
// Audio-Callback
// ---------------------------------------------------------------------------
class AudioCallback : public IAudioEndpointVolumeCallback {
    LONG m_ref = 1;
public:
    STDMETHODIMP QueryInterface(REFIID riid, void** ppv) override {
        if (riid == IID_IUnknown ||
            riid == __uuidof(IAudioEndpointVolumeCallback)) {
            *ppv = static_cast<IAudioEndpointVolumeCallback*>(this);
            AddRef(); return S_OK;
        }
        *ppv = nullptr; return E_NOINTERFACE;
    }
    STDMETHODIMP_(ULONG) AddRef()  override { return InterlockedIncrement(&m_ref); }
    STDMETHODIMP_(ULONG) Release() override {
        LONG r = InterlockedDecrement(&m_ref);
        if (r == 0) delete this;
        return r;
    }
    STDMETHODIMP OnNotify(PAUDIO_VOLUME_NOTIFICATION_DATA) override {
        if (g_msgWnd) PostMessageW(g_msgWnd, WM_AUDIO_CHANGED, 0, 0);
        return S_OK;
    }
};
static AudioCallback* g_pCallback = nullptr;

// ---------------------------------------------------------------------------
// Audio-Hilfsfunktionen
// ---------------------------------------------------------------------------
static bool GetMuteState(bool& muted) {
    if (!g_pVolume) return false;
    BOOL b = FALSE;
    if (FAILED(g_pVolume->GetMute(&b))) return false;
    muted = (b == TRUE); return true;
}

static bool GetVolumePercent(int& pct) {
    if (!g_pVolume) return false;
    float v = 0.f;
    if (FAILED(g_pVolume->GetMasterVolumeLevelScalar(&v))) return false;
    pct = (int)(v * 100.f + 0.5f); return true;
}

static void ToggleMute() {
    if (!g_pVolume) return;
    BOOL b = FALSE;
    g_pVolume->GetMute(&b);
    g_pVolume->SetMute(!b, nullptr);
}

static void AdjustSystemVolume(int wheelDelta) {
    if (!g_pVolume) return;
    float step = g_volStep / 100.f;
    float cur = 0.f;
    g_pVolume->GetMasterVolumeLevelScalar(&cur);
    float next = cur + (wheelDelta > 0 ? step : -step);
    if (next < 0.f) next = 0.f;
    if (next > 1.f) next = 1.f;
    g_pVolume->SetMasterVolumeLevelScalar(next, nullptr);
}

static void AdjustSessionVolume(ISimpleAudioVolume* pVol, int wheelDelta) {
    if (!pVol) return;
    float step = g_volStep / 100.f;
    float cur = 0.f;
    pVol->GetMasterVolume(&cur);
    float next = cur + (wheelDelta > 0 ? step : -step);
    if (next < 0.f) next = 0.f;
    if (next > 1.f) next = 1.f;
    pVol->SetMasterVolume(next, nullptr);
}

static bool InitAudio() {
    if (FAILED(CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED))) return false;
    if (FAILED(CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr,
        CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), (void**)&g_pEnum)))
        return false;
    IMMDevice* pDev = nullptr;
    if (FAILED(g_pEnum->GetDefaultAudioEndpoint(eRender, eConsole, &pDev)))
        return false;
    HRESULT hr = pDev->Activate(__uuidof(IAudioEndpointVolume),
        CLSCTX_ALL, nullptr, (void**)&g_pVolume);
    pDev->Release();
    if (FAILED(hr)) return false;
    g_pCallback = new AudioCallback();
    g_pVolume->RegisterControlChangeNotify(g_pCallback);
    return true;
}

static void UninitAudio() {
    if (g_pVolume && g_pCallback)
        g_pVolume->UnregisterControlChangeNotify(g_pCallback);
    if (g_pCallback) { g_pCallback->Release(); g_pCallback = nullptr; }
    if (g_pVolume)   { g_pVolume->Release();   g_pVolume   = nullptr; }
    if (g_pEnum)     { g_pEnum->Release();      g_pEnum     = nullptr; }
    CoUninitialize();
}

// ---------------------------------------------------------------------------
// Sessions
// ---------------------------------------------------------------------------
static void FreeSessions() {
    for (auto& s : g_sessions) {
        if (s.pVol) { s.pVol->Release(); s.pVol = nullptr; }
        if (s.icon) { DestroyIcon(s.icon); s.icon = nullptr; }
    }
    g_sessions.clear();
}

static void EnumerateSessions() {
    FreeSessions();
    if (!g_pEnum) return;

    IMMDevice* pDev = nullptr;
    if (FAILED(g_pEnum->GetDefaultAudioEndpoint(eRender, eConsole, &pDev))) return;

    IAudioSessionManager2* pMgr = nullptr;
    pDev->Activate(__uuidof(IAudioSessionManager2), CLSCTX_ALL, nullptr, (void**)&pMgr);
    pDev->Release();
    if (!pMgr) return;

    IAudioSessionEnumerator* pSE = nullptr;
    pMgr->GetSessionEnumerator(&pSE);
    pMgr->Release();
    if (!pSE) return;

    int count = 0;
    pSE->GetCount(&count);

    for (int i = 0; i < count; i++) {
        IAudioSessionControl* pC = nullptr;
        pSE->GetSession(i, &pC);
        if (!pC) continue;

        IAudioSessionControl2* pC2 = nullptr;
        pC->QueryInterface(__uuidof(IAudioSessionControl2), (void**)&pC2);
        pC->Release();
        if (!pC2) continue;

        if (pC2->IsSystemSoundsSession() == S_OK) { pC2->Release(); continue; }

        DWORD pid = 0;
        pC2->GetProcessId(&pid);

        std::wstring name;
        LPWSTR pwsz = nullptr;
        if (SUCCEEDED(pC2->GetDisplayName(&pwsz)) && pwsz && *pwsz) {
            name = pwsz; CoTaskMemFree(pwsz);
        }
        if (name.empty() && pid) {
            HANDLE hP = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
            if (hP) {
                wchar_t path[MAX_PATH] = {};
                DWORD sz = MAX_PATH;
                if (QueryFullProcessImageNameW(hP, 0, path, &sz)) {
                    wchar_t* fn  = PathFindFileNameW(path);
                    wchar_t* ext = PathFindExtensionW(fn);
                    if (ext) *ext = L'\0';
                    name = fn;
                }
                CloseHandle(hP);
            }
        }
        if (name.empty()) { pC2->Release(); continue; }

        ISimpleAudioVolume* pVol = nullptr;
        pC2->QueryInterface(__uuidof(ISimpleAudioVolume), (void**)&pVol);
        pC2->Release();
        if (!pVol) continue;

        float vol = 1.f; BOOL mut = FALSE;
        pVol->GetMasterVolume(&vol);
        pVol->GetMute(&mut);

        HICON icon = nullptr;
        if (pid) {
            HANDLE hP = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
            if (hP) {
                wchar_t path[MAX_PATH] = {};
                DWORD sz = MAX_PATH;
                if (QueryFullProcessImageNameW(hP, 0, path, &sz)) {
                    SHFILEINFOW sfi = {};
                    SHGetFileInfoW(path, 0, &sfi, sizeof(sfi),
                        SHGFI_ICON | SHGFI_SMALLICON);
                    icon = sfi.hIcon;
                }
                CloseHandle(hP);
            }
        }

        AudioSession s;
        s.name = name; s.icon = icon; s.pVol = pVol;
        s.volume = vol; s.muted = (mut == TRUE); s.pid = pid;
        g_sessions.push_back(std::move(s));
    }
    pSE->Release();
}

// ---------------------------------------------------------------------------
// Tray-Icon zeichnen
// ---------------------------------------------------------------------------
static HICON CreateMuteIcon(bool muted, int size = 16) {
    HDC hdcScr = GetDC(nullptr);
    HDC hdc    = CreateCompatibleDC(hdcScr);

    BITMAPV5HEADER bi = {};
    bi.bV5Size = sizeof(bi); bi.bV5Width = size; bi.bV5Height = -size;
    bi.bV5Planes = 1; bi.bV5BitCount = 32; bi.bV5Compression = BI_BITFIELDS;
    bi.bV5RedMask = 0x00FF0000; bi.bV5GreenMask = 0x0000FF00;
    bi.bV5BlueMask = 0x000000FF; bi.bV5AlphaMask = 0xFF000000;

    void* pv = nullptr;
    HBITMAP hBmp = CreateDIBSection(hdc, (BITMAPINFO*)&bi,
        DIB_RGB_COLORS, &pv, nullptr, 0);
    HGDIOBJ ob = SelectObject(hdc, hBmp);

    RECT rc = {0, 0, size, size};
    HBRUSH hbg = CreateSolidBrush(RGB(0,0,0));
    FillRect(hdc, &rc, hbg); DeleteObject(hbg);

    LOGFONTW lf = {}; lf.lfHeight = -(size-2); lf.lfCharSet = DEFAULT_CHARSET;
    lf.lfQuality = CLEARTYPE_QUALITY;
    wcscpy_s(lf.lfFaceName, L"Segoe UI Emoji");
    HFONT hf = CreateFontIndirectW(&lf);
    HGDIOBJ of = SelectObject(hdc, hf);
    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, muted ? RGB(220,60,60) : RGB(255,255,255));
    // 🔇 U+1F507 / 🔊 U+1F50A als Surrogate-Pair
    const wchar_t* sym = muted ? L"\xD83D\xDD07" : L"\xD83D\xDD0A";
    DrawTextW(hdc, sym, -1, &rc,
              DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOCLIP);
    SelectObject(hdc, of); DeleteObject(hf);
    SelectObject(hdc, ob);

    HBITMAP hMask = CreateBitmap(size, size, 1, 1, nullptr);
    ICONINFO ii = {}; ii.fIcon = TRUE; ii.hbmColor = hBmp; ii.hbmMask = hMask;
    HICON hIcon = CreateIconIndirect(&ii);
    DeleteObject(hMask); DeleteObject(hBmp);
    DeleteDC(hdc); ReleaseDC(nullptr, hdcScr);
    return hIcon;
}

static void UpdateTrayIcon(bool add = false) {
    bool muted = false; int vol = 100;
    GetMuteState(muted); GetVolumePercent(vol);
    HICON hIcon = CreateMuteIcon(muted);

    g_nid.cbSize = sizeof(g_nid); g_nid.hWnd = g_msgWnd; g_nid.uID = TRAY_UID;
    g_nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP | NIF_SHOWTIP;
    g_nid.uCallbackMessage = WM_TRAYICON; g_nid.hIcon = hIcon;
    g_nid.uVersion = NOTIFYICON_VERSION_4;

    if (muted) swprintf_s(g_nid.szTip, L"Stumm (%d%%)", vol);
    else       swprintf_s(g_nid.szTip, L"Lautstaerke: %d%%", vol);

    if (add) {
        Shell_NotifyIconW(NIM_ADD, &g_nid);
        Shell_NotifyIconW(NIM_SETVERSION, &g_nid);
        g_nidAdded = true;
    } else if (g_nidAdded) {
        Shell_NotifyIconW(NIM_MODIFY, &g_nid);
    }
    DestroyIcon(hIcon);
}

// ---------------------------------------------------------------------------
// Popup-Fenster zeichnen
// ---------------------------------------------------------------------------
static void DrawPopup(HWND hwnd, HDC hdc) {
    RECT rcC; GetClientRect(hwnd, &rcC);

    HBRUSH hbg = CreateSolidBrush(CLR_BG);
    FillRect(hdc, &rcC, hbg); DeleteObject(hbg);

    // Rahmen
    HPEN hpen = CreatePen(PS_SOLID, 1, CLR_BORDER);
    HGDIOBJ op = SelectObject(hdc, hpen);
    HGDIOBJ obr = SelectObject(hdc, GetStockObject(NULL_BRUSH));
    Rectangle(hdc, rcC.left, rcC.top, rcC.right, rcC.bottom);
    SelectObject(hdc, op); SelectObject(hdc, obr); DeleteObject(hpen);

    SetBkMode(hdc, TRANSPARENT);
    int n = (int)g_sessions.size();

    if (n == 0) {
        LOGFONTW lf = {}; lf.lfHeight = -13; lf.lfCharSet = DEFAULT_CHARSET;
        wcscpy_s(lf.lfFaceName, L"Segoe UI");
        HFONT hf = CreateFontIndirectW(&lf);
        HGDIOBJ of = SelectObject(hdc, hf);
        SetTextColor(hdc, CLR_SUBTEXT);
        RECT rc = {POPUP_PAD, POPUP_PAD, POPUP_W - POPUP_PAD, POPUP_ITEM_H};
        DrawTextW(hdc, L"Keine aktiven Programme", -1, &rc,
                  DT_CENTER | DT_VCENTER | DT_SINGLELINE);
        SelectObject(hdc, of); DeleteObject(hf);
        return;
    }

    LOGFONTW lfN = {}; lfN.lfHeight = -13; lfN.lfCharSet = DEFAULT_CHARSET;
    wcscpy_s(lfN.lfFaceName, L"Segoe UI");
    HFONT hfN = CreateFontIndirectW(&lfN);

    LOGFONTW lfV = {}; lfV.lfHeight = -11; lfV.lfCharSet = DEFAULT_CHARSET;
    wcscpy_s(lfV.lfFaceName, L"Segoe UI");
    HFONT hfV = CreateFontIndirectW(&lfV);

    for (int i = 0; i < n; i++) {
        const AudioSession& s = g_sessions[i];
        int y0 = POPUP_PAD + i * POPUP_ITEM_H;
        int y1 = y0 + POPUP_ITEM_H;

        if (i == g_hoveredItem) {
            RECT rh = {1, y0, POPUP_W - 1, y1};
            HBRUSH hh = CreateSolidBrush(CLR_ITEM_HOVER);
            FillRect(hdc, &rh, hh); DeleteObject(hh);
        }

        if (i < n - 1) {
            HPEN hsp = CreatePen(PS_SOLID, 1, CLR_BORDER);
            HGDIOBJ osp = SelectObject(hdc, hsp);
            MoveToEx(hdc, POPUP_PAD, y1, nullptr);
            LineTo(hdc, POPUP_W - POPUP_PAD, y1);
            SelectObject(hdc, osp); DeleteObject(hsp);
        }

        // Icon
        int ix = POPUP_PAD + 4;
        int iy = y0 + (POPUP_ITEM_H - POPUP_ICON_SZ) / 2;
        if (s.icon)
            DrawIconEx(hdc, ix, iy, s.icon,
                       POPUP_ICON_SZ, POPUP_ICON_SZ, 0, nullptr, DI_NORMAL);

        int tx = ix + POPUP_ICON_SZ + 8;
        int tw = POPUP_W - tx - 42;

        // Name (ausgegraut wenn stummgeschaltet)
        SelectObject(hdc, hfN);
        SetTextColor(hdc, s.muted ? CLR_SUBTEXT : CLR_TEXT);
        RECT rn = {tx, y0 + 6, tx + tw, y0 + 22};
        DrawTextW(hdc, s.name.c_str(), -1, &rn,
                  DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS);

        // Prozent
        wchar_t pct[8];
        swprintf_s(pct, L"%d%%", (int)(s.volume * 100.f + 0.5f));
        SelectObject(hdc, hfV);
        SetTextColor(hdc, CLR_SUBTEXT);
        RECT rp = {POPUP_W - 40, y0 + 6, POPUP_W - 4, y0 + 22};
        DrawTextW(hdc, pct, -1, &rp,
                  DT_RIGHT | DT_VCENTER | DT_SINGLELINE);

        // Lautstärke-Balken
        int bx = tx;
        int by = y0 + POPUP_ITEM_H - POPUP_BAR_H - 8;
        int bw = POPUP_W - tx - 4;
        int bfw = (int)(s.volume * bw);

        RECT rt = {bx, by, bx + bw, by + POPUP_BAR_H};
        HBRUSH hbt = CreateSolidBrush(CLR_BAR_TRACK);
        FillRect(hdc, &rt, hbt); DeleteObject(hbt);

        if (bfw > 0) {
            RECT rf = {bx, by, bx + bfw, by + POPUP_BAR_H};
            HBRUSH hbf = CreateSolidBrush(s.muted ? CLR_MUTED_BAR : CLR_ACCENT);
            FillRect(hdc, &rf, hbf); DeleteObject(hbf);
        }
    }

    DeleteObject(hfN); DeleteObject(hfV);
}

static LRESULT CALLBACK PopupWndProc(HWND hwnd, UINT msg,
                                      WPARAM wParam, LPARAM lParam)
{
    switch (msg) {
    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        DrawPopup(hwnd, hdc);
        EndPaint(hwnd, &ps);
        return 0;
    }
    case WM_LBUTTONUP: {
        int y   = GET_Y_LPARAM(lParam);
        int idx = (y - POPUP_PAD) / POPUP_ITEM_H;
        if (idx >= 0 && idx < (int)g_sessions.size()) {
            AudioSession& s = g_sessions[idx];
            BOOL cur = FALSE;
            s.pVol->GetMute(&cur);
            s.pVol->SetMute(!cur, nullptr);
            s.muted = (cur == FALSE); // nach Toggle: war FALSE -> jetzt TRUE, usw.
            InvalidateRect(hwnd, nullptr, FALSE);
        }
        return 0;
    }
    case WM_MOUSEMOVE: {
        int y   = GET_Y_LPARAM(lParam);
        int idx = (y - POPUP_PAD) / POPUP_ITEM_H;
        if (idx < 0 || idx >= (int)g_sessions.size()) idx = -1;
        if (idx != g_hoveredItem) {
            g_hoveredItem = idx;
            InvalidateRect(hwnd, nullptr, FALSE);
        }
        return 0;
    }
    case WM_POPUP_SCROLL: {
        int item  = (int)(INT_PTR)wParam;
        int delta = (int)(short)LOWORD(lParam);
        if (item >= 0 && item < (int)g_sessions.size()) {
            AdjustSessionVolume(g_sessions[item].pVol, delta);
            g_sessions[item].pVol->GetMasterVolume(&g_sessions[item].volume);
            BOOL m = FALSE;
            g_sessions[item].pVol->GetMute(&m);
            g_sessions[item].muted = (m == TRUE);
        } else {
            AdjustSystemVolume(delta);
        }
        InvalidateRect(hwnd, nullptr, FALSE);
        return 0;
    }
    case WM_POPUP_HOVER: {
        POINT pt = {(LONG)(INT_PTR)wParam, (LONG)(INT_PTR)lParam};
        ScreenToClient(hwnd, &pt);
        int idx = (pt.y - POPUP_PAD) / POPUP_ITEM_H;
        if (idx < 0 || idx >= (int)g_sessions.size()) idx = -1;
        if (idx != g_hoveredItem) {
            g_hoveredItem = idx;
            InvalidateRect(hwnd, nullptr, FALSE);
        }
        return 0;
    }
    case WM_DESTROY:
        g_popup = nullptr;
        g_hoveredItem = -1;
        FreeSessions();
        return 0;
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

// ---------------------------------------------------------------------------
// Low-Level Mouse Hook
// ---------------------------------------------------------------------------
static LRESULT CALLBACK MouseHookProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode != HC_ACTION)
        return CallNextHookEx(g_mouseHook, nCode, wParam, lParam);

    MSLLHOOKSTRUCT* ms = (MSLLHOOKSTRUCT*)lParam;
    POINT pt = ms->pt;

    // Scroll über Popup
    if (wParam == WM_MOUSEWHEEL && g_popup) {
        RECT rp; GetWindowRect(g_popup, &rp);
        if (PtInRect(&rp, pt)) {
            POINT ptC = pt; ScreenToClient(g_popup, &ptC);
            int idx = (ptC.y - POPUP_PAD) / POPUP_ITEM_H;
            if (idx < 0 || idx >= (int)g_sessions.size()) idx = -1;
            short delta = (short)HIWORD(ms->mouseData);
            PostMessageW(g_popup, WM_POPUP_SCROLL,
                         (WPARAM)(INT_PTR)idx,
                         (LPARAM)(WORD)(short)delta);
            return 1;
        }
    }

    // Scroll über Tray-Icon (kein Popup offen)
    if (wParam == WM_MOUSEWHEEL && !g_popup && g_msgWnd) {
        NOTIFYICONIDENTIFIER nii = {};
        nii.cbSize = sizeof(nii); nii.hWnd = g_msgWnd; nii.uID = TRAY_UID;
        RECT rcI = {};
        if (SUCCEEDED(Shell_NotifyIconGetRect(&nii, &rcI)) && PtInRect(&rcI, pt)) {
            short delta = (short)HIWORD(ms->mouseData);
            AdjustSystemVolume(delta);
            return 1;
        }
    }

    // Mausbewegung: Popup schließen wenn Maus raus, sonst Hover updaten
    if (wParam == WM_MOUSEMOVE && g_popup) {
        RECT rp; GetWindowRect(g_popup, &rp);

        NOTIFYICONIDENTIFIER nii = {};
        nii.cbSize = sizeof(nii); nii.hWnd = g_msgWnd; nii.uID = TRAY_UID;
        RECT rcI = {};
        Shell_NotifyIconGetRect(&nii, &rcI);

        if (!PtInRect(&rp, pt) && !PtInRect(&rcI, pt)) {
            DestroyWindow(g_popup);
        } else if (PtInRect(&rp, pt)) {
            PostMessageW(g_popup, WM_POPUP_HOVER,
                         (WPARAM)(INT_PTR)pt.x,
                         (LPARAM)(INT_PTR)pt.y);
        }
    }

    return CallNextHookEx(g_mouseHook, nCode, wParam, lParam);
}

// ---------------------------------------------------------------------------
// Popup anzeigen
// ---------------------------------------------------------------------------
static void ShowPopup() {
    if (g_popup) return;
    EnumerateSessions();

    int n = g_sessions.empty() ? 1 : (int)g_sessions.size();
    int w = POPUP_W;
    int h = POPUP_PAD * 2 + n * POPUP_ITEM_H;

    NOTIFYICONIDENTIFIER nii = {};
    nii.cbSize = sizeof(nii); nii.hWnd = g_msgWnd; nii.uID = TRAY_UID;
    RECT rcI = {};
    Shell_NotifyIconGetRect(&nii, &rcI);

    int x = rcI.right - w;
    int y = rcI.top   - h - 4;

    MONITORINFO mi = {}; mi.cbSize = sizeof(mi);
    GetMonitorInfoW(MonitorFromRect(&rcI, MONITOR_DEFAULTTONEAREST), &mi);
    if (x < mi.rcWork.left) x = mi.rcWork.left;
    if (y < mi.rcWork.top)  y = mi.rcWork.top;

    g_hoveredItem = -1;
    g_popup = CreateWindowExW(
        WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE,
        POPUP_CLASS, nullptr, WS_POPUP,
        x, y, w, h,
        nullptr, nullptr, GetModuleHandleW(nullptr), nullptr);

    if (!g_popup) { FreeSessions(); return; }
    ShowWindow(g_popup, SW_SHOWNOACTIVATE);
    UpdateWindow(g_popup);
}

// ---------------------------------------------------------------------------
// Kontext-Menü
// ---------------------------------------------------------------------------
static void ShowContextMenu(HWND hwnd) {
    bool muted = false; GetMuteState(muted);
    HMENU hMenu = CreatePopupMenu();
    AppendMenuW(hMenu, MF_STRING, IDM_TOGGLE_MUTE,
                muted ? L"Ton einschalten" : L"Stummschalten");
    AppendMenuW(hMenu, MF_SEPARATOR, 0, nullptr);
    AppendMenuW(hMenu, MF_STRING, IDM_OPEN_MIXER, L"Lautstaerke-Mixer oeffnen");
    POINT pt; GetCursorPos(&pt);
    SetForegroundWindow(hwnd);
    int cmd = TrackPopupMenuEx(hMenu,
        TPM_RETURNCMD | TPM_RIGHTBUTTON | TPM_BOTTOMALIGN | TPM_RIGHTALIGN,
        pt.x, pt.y, hwnd, nullptr);
    DestroyMenu(hMenu);
    if (cmd == IDM_TOGGLE_MUTE) ToggleMute();
    else if (cmd == IDM_OPEN_MIXER)
        ShellExecuteW(nullptr, L"open", L"sndvol.exe",
                      nullptr, nullptr, SW_SHOWNORMAL);
}

// ---------------------------------------------------------------------------
// Haupt-Fensterprozedur
// ---------------------------------------------------------------------------
static LRESULT CALLBACK MsgWndProc(HWND hwnd, UINT msg,
                                    WPARAM wParam, LPARAM lParam)
{
    switch (msg) {
    case WM_TRAYICON: {
        UINT ev = LOWORD(lParam);
        if (ev == WM_LBUTTONUP) {
            KillTimer(hwnd, TIMER_HOVER);
            if (g_popup) DestroyWindow(g_popup);
            else ToggleMute();
        } else if (ev == WM_RBUTTONUP) {
            KillTimer(hwnd, TIMER_HOVER);
            if (g_popup) DestroyWindow(g_popup);
            ShowContextMenu(hwnd);
        } else if (ev == WM_MOUSEMOVE) {
            if (!g_popup)
                SetTimer(hwnd, TIMER_HOVER, g_hoverDelay, nullptr);
        }
        return 0;
    }
    case WM_TIMER:
        if (wParam == TIMER_HOVER) {
            KillTimer(hwnd, TIMER_HOVER);
            ShowPopup();
        }
        return 0;
    case WM_AUDIO_CHANGED:
        UpdateTrayIcon();
        return 0;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

// ---------------------------------------------------------------------------
// Message-Loop
// ---------------------------------------------------------------------------
static void MessageLoop() {
    HINSTANCE hInst = GetModuleHandleW(nullptr);

    UnregisterClassW(TRAY_CLASS,  hInst);
    UnregisterClassW(POPUP_CLASS, hInst);

    WNDCLASSEXW wcT  = {};
    wcT.cbSize       = sizeof(wcT);
    wcT.lpfnWndProc  = MsgWndProc;
    wcT.hInstance    = hInst;
    wcT.lpszClassName = TRAY_CLASS;
    if (!RegisterClassExW(&wcT)) {
        Wh_Log(L"MuteToggle: Tray-Klasse Fehler %u", GetLastError()); return;
    }

    WNDCLASSEXW wcP  = {};
    wcP.cbSize       = sizeof(wcP);
    wcP.lpfnWndProc  = PopupWndProc;
    wcP.hInstance    = hInst;
    wcP.hCursor      = LoadCursorW(nullptr, IDC_ARROW);
    wcP.lpszClassName = POPUP_CLASS;
    if (!RegisterClassExW(&wcP)) {
        Wh_Log(L"MuteToggle: Popup-Klasse Fehler %u", GetLastError());
        UnregisterClassW(TRAY_CLASS, hInst); return;
    }

    g_msgWnd = CreateWindowExW(0, TRAY_CLASS, nullptr, 0,
        0, 0, 0, 0, HWND_MESSAGE, nullptr, hInst, nullptr);
    if (!g_msgWnd) {
        Wh_Log(L"MuteToggle: Fenster Fehler %u", GetLastError());
        UnregisterClassW(TRAY_CLASS, hInst);
        UnregisterClassW(POPUP_CLASS, hInst); return;
    }

    if (!InitAudio()) {
        Wh_Log(L"MuteToggle: Audio-Init fehlgeschlagen");
        DestroyWindow(g_msgWnd); g_msgWnd = nullptr;
        UnregisterClassW(TRAY_CLASS, hInst);
        UnregisterClassW(POPUP_CLASS, hInst); return;
    }

    g_mouseHook = SetWindowsHookExW(WH_MOUSE_LL, MouseHookProc, hInst, 0);

    g_nidAdded = false;
    UpdateTrayIcon(true);

    MSG m;
    while (GetMessageW(&m, nullptr, 0, 0) > 0) {
        TranslateMessage(&m);
        DispatchMessageW(&m);
    }

    if (g_mouseHook) { UnhookWindowsHookEx(g_mouseHook); g_mouseHook = nullptr; }
    if (g_popup)     { DestroyWindow(g_popup); g_popup = nullptr; }
    UninitAudio();
    FreeSessions();
    if (g_nidAdded) { Shell_NotifyIconW(NIM_DELETE, &g_nid); g_nidAdded = false; }
    DestroyWindow(g_msgWnd); g_msgWnd = nullptr;
    UnregisterClassW(POPUP_CLASS, hInst);
    UnregisterClassW(TRAY_CLASS,  hInst);
}

// ---------------------------------------------------------------------------
// Einstellungen & Einstiegspunkte
// ---------------------------------------------------------------------------
static void LoadSettings() {
    int s = (int)Wh_GetIntSetting(L"volumeStep");
    g_volStep = (s >= 1 && s <= 10) ? s : 2;

    int d = (int)Wh_GetIntSetting(L"hoverDelay");
    g_hoverDelay = (d >= 100 && d <= 5000) ? d : 1000;
}

BOOL Wh_ModInit() {
    Wh_Log(L"Taskbar Mute Toggle v2.1: Init");
    LoadSettings();
    g_thread = std::thread(MessageLoop);
    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L"Taskbar Mute Toggle v2.1: Uninit");
    if (g_msgWnd) PostMessageW(g_msgWnd, WM_QUIT, 0, 0);
    if (g_thread.joinable()) g_thread.join();
}

void Wh_ModSettingsChanged() {
    LoadSettings();
    if (g_msgWnd && g_nidAdded) UpdateTrayIcon();
}
