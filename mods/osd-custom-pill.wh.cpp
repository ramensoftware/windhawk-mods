// ==WindhawkMod==
// @id              osd-custom-pill
// @name            OSD Custom Pill
// @description     Replaces the Windows volume, brightness, and virtual desktop OSD with an animated spring pill
// @description:fr-FR Remplace les indicateurs Windows de volume, de luminosité et de bureau virtuel par une pilule animée à ressort
// @version         4.6
// @author          yelloxygene
// @github          https://github.com/yelloxygene
// @include         explorer.exe
// @compilerOptions -lole32 -lgdi32 -luser32 -ldxva2 -luuid -ldwmapi -ladvapi32
// ==/WindhawkMod==

// ==WindhawkModSettings==
/*
- startDelayMs: 90
  $name: "Delay before the pill appears (ms)"
  $name:fr-FR: "Délai avant affichage de la pilule (ms)"
- decideMs: 300
  $name: "Time to distinguish volume/brightness from virtual desktop changes (ms)"
  $name:fr-FR: "Attente pour savoir si c'est volume/luminosité ou un changement de bureau (ms)"
- holdMs: 120
  $name: "Delay before expanding while the circle rises (ms)"
  $name:fr-FR: "Délai avant le dépliage, pendant que le rond monte (ms)"
- closeHoldMs: 140
  $name: "Delay before sliding down while collapsing (ms)"
  $name:fr-FR: "Délai avant la sortie par le bas, pendant que ça se replie (ms)"
- responseMs: 450
  $name: "Spring response time (ms; lower = snappier)"
  $name:fr-FR: "Vitesse du ressort (ms, plus petit = plus nerveux)"
- dampingPct: 62
  $name: "Spring damping (%; lower = more bouncing, 100 = none)"
  $name:fr-FR: "Amortissement (%, plus petit = plus de rebonds, 100 = aucun)"
- widthPx: 220
  $name: "Pill width (px at 100% scaling)"
  $name:fr-FR: "Largeur de la pilule (px à 100%)"
- heightPx: 44
  $name: "Pill height (px at 100% scaling)"
  $name:fr-FR: "Hauteur de la pilule (px à 100%)"
- bottomOffsetPx: 70
  $name: "Distance from pill bottom to screen bottom (px at 100% scaling)"
  $name:fr-FR: "Distance entre le bas de la pilule et le bas de l'écran (px à 100%)"
- shakePx: 18
  $name: "Directional motion: horizontal displacement (px at 100%, 0 = disabled)"
  $name:fr-FR: "\"Impulsion directionnelle : déplacement horizontal (px à 100%, 0 = désactivé)\""
- shakeMs: 750
  $name: "Directional motion: spring duration (ms; higher = slower)"
  $name:fr-FR: "\"Impulsion directionnelle : durée du ressort (ms, plus grand = plus lent)\""
- shakeDampingPct: 32
  $name: "Directional motion: damping (%; lower = more bouncing)"
  $name:fr-FR: "\"Impulsion directionnelle : amortissement (%, plus petit = plus de rebonds)\""
- valueShakePct: 70
  $name: "Motion per value change (% intensity, 0 = disabled)"
  $name:fr-FR: "\"Impulsion à chaque changement de valeur (% de l'amplitude, 0 = désactivée)\""
- shakeWidthPct: 32
  $name: "Directional motion: maximum horizontal stretch (%)"
  $name:fr-FR: "\"Impulsion directionnelle : élargissement horizontal maximal (%)\""
- returnPowerPct: 155
  $name: "Elastic return: rebound strength (%, 100 = v4.4, 155 = stronger, 0 = soft)"
  $name:fr-FR: "\"Retour élastique : puissance du rebond (%, 100 = comme la v4.4, 155 = plus marqué, 0 = retour doux)\""
- compactEnabled: true
  $name: "Compact mode: enable compact mode"
  $name:fr-FR: "\"Mode compact : activer le mode compact\""
- compactTriggerCount: 2
  $name: "Compact mode: number of presses before activation"
  $name:fr-FR: "\"Mode compact : nombre de pressions avant activation\""
- compactReturnToNormal: true
  $name: "Compact mode: return to normal after a pause (off = compact until hidden)"
  $name:fr-FR: "\"Mode compact : revenir au mode normal après une pause (décoché = reste compact jusqu'à sa disparition)\""
- compactReleaseMs: 450
  $name: "Compact mode: max gap between presses and return delay (ms; 0 = no limit/automatic return)"
  $name:fr-FR: "\"Mode compact : fenêtre entre les pressions et délai de retour (ms, 0 = aucune limite / aucun retour automatique)\""
- deskLabel: Bureau
  $name: "Label before virtual desktop number when no name is set"
  $name:fr-FR: "Mot affiché devant le numéro quand le bureau n'a pas de nom"
- deskWidthPx: 200
  $name: "Virtual desktop: pill width (px at 100% scaling)"
  $name:fr-FR: "\"Bureau : largeur de la pilule (px à 100%)\""
- deskHeightPx: 44
  $name: "Virtual desktop: pill height (px at 100% scaling)"
  $name:fr-FR: "\"Bureau : hauteur de la pilule (px à 100%)\""
- deskBottomOffsetPx: 70
  $name: "Virtual desktop: distance from screen bottom (px at 100% scaling)"
  $name:fr-FR: "\"Bureau : distance au bas de l'écran (px à 100%)\""
- deskHoldMs: 120
  $name: "Virtual desktop: delay before expansion (ms)"
  $name:fr-FR: "\"Bureau : délai avant le dépliage (ms)\""
- deskCloseHoldMs: 140
  $name: "Virtual desktop: delay before sliding down (ms)"
  $name:fr-FR: "\"Bureau : délai avant la sortie par le bas (ms)\""
- deskResponseMs: 450
  $name: "Virtual desktop: spring response time (ms)"
  $name:fr-FR: "\"Bureau : vitesse du ressort (ms)\""
- deskDampingPct: 62
  $name: "Virtual desktop: damping (%; lower = more bouncing)"
  $name:fr-FR: "\"Bureau : amortissement (%, plus petit = plus de rebonds)\""
- deskDurationMs: 0
  $name: "Virtual desktop: display duration (ms; 0 = follow Windows)"
  $name:fr-FR: "\"Bureau : durée d'affichage (ms, 0 = suit Windows)\""
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <dwmapi.h>
#include <mmdeviceapi.h>
#include <endpointvolume.h>
#include <physicalmonitorenumerationapi.h>
#include <highlevelmonitorconfigurationapi.h>
#include <cmath>
#include <cwchar>
#include <cstdlib>
#include <cstring>

// ---------------------------------------------------------------- état global
struct {
    int startDelayMs, decideMs, holdMs, closeHoldMs, responseMs, dampingPct, widthPx, heightPx, bottomOffset,
        shakePx, shakeMs, shakeDampPct, valueShakePct, shakeWidthPct, returnPowerPct, compactReleaseMs,
        compactEnabled, compactTriggerCount, compactReturnToNormal;
} g;
struct {
    int widthPx, heightPx, bottomOffset, holdMs, closeHoldMs, responseMs, dampingPct, durationMs;
} dk;
static wchar_t g_deskLabel[32] = L"Bureau";

#define MSG_SHOW   (WM_APP + 1)
#define MSG_HIDE   (WM_APP + 2)
#define MSG_VALUES (WM_APP + 3)

static volatile LONG g_run = 1;
static volatile LONG g_vol = -1, g_mute = 0, g_bri = -1;
static volatile DWORD g_volTick = 0, g_briTick = 0;
static volatile DWORD g_deskTick = 0;   // dernier changement de bureau détecté
static volatile DWORD g_wantDdcUntil = 0;
static int g_limitDir = 0;              // luminosité en butée : +1 max, -1 min
static int g_bumpDir = 0;               // sens de la secousse à jouer à l'ouverture
static HWND g_ui = nullptr;
static HWND g_osd = nullptr;
static HANDLE g_uiThread = nullptr, g_pollThread = nullptr;

static inline float Clamp(float v, float a, float b) { return v < a ? a : (v > b ? b : v); }
static inline float Lerp(float a, float b, float t) { return a + (b - a) * t; }

static bool IsOsd(HWND h) {
    wchar_t b[128];
    return GetClassNameW(h, b, 128) && wcscmp(b, L"XamlExplorerHostIslandWindow") == 0;
}

// ---------------------------------------------------------------- ressorts
struct Spring { float x = 0, v = 0, t = 0; };
static float g_k = 100, g_c = 10;
static float g_kv = 400, g_cv = 30;
static float g_kc = 200, g_cc = 20;     // ressort du mode compact

static void MakeSpringParams(float responseSec, float zeta, float* k, float* c) {
    float w = 6.2831853f / responseSec;
    *k = w * w;
    *c = 2.0f * zeta * w;
}

static void Step(Spring& s, float dt, float k, float c) {
    int n = (int)ceilf(dt / 0.004f);
    if (n < 1) n = 1;
    float h = dt / n;
    for (int i = 0; i < n; i++) {
        float a = k * (s.t - s.x) - c * s.v;
        s.v += a * h;
        s.x += s.v * h;
    }
}

static void ComputeSprings(int responseMs, int dampingPct) {
    float zeta = Clamp(dampingPct / 100.0f, 0.2f, 1.2f);
    MakeSpringParams(responseMs / 1000.0f, zeta, &g_k, &g_c);
    MakeSpringParams(0.22f, 0.75f, &g_kv, &g_cv);
    // Ressort du mode compact : amortissement plus faible pour un rebond visible.
    // La période un peu plus longue rend le mouvement souple, sans être trop lent.
    MakeSpringParams(0.36f, 0.48f, &g_kc, &g_cc);
}

// ---------------------------------------------------------------- nom du bureau virtuel
static bool RegBin(const wchar_t* path, const wchar_t* val, BYTE* buf, DWORD* sz) {
    return RegGetValueW(HKEY_CURRENT_USER, path, val, RRF_RT_REG_BINARY, nullptr, buf, sz) == ERROR_SUCCESS;
}

static void ReadDesktopName(wchar_t* out, int n) {
    const wchar_t* base = L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\VirtualDesktops";
    wchar_t sessPath[300];
    DWORD sess = 0;
    ProcessIdToSessionId(GetCurrentProcessId(), &sess);
    _snwprintf(sessPath, 300,
        L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\SessionInfo\\%lu\\VirtualDesktops", (unsigned long)sess);

    BYTE cur[16]; DWORD cs = 16;
    bool ok = RegBin(sessPath, L"CurrentVirtualDesktop", cur, &cs);
    if (!ok) { cs = 16; ok = RegBin(base, L"CurrentVirtualDesktop", cur, &cs); }

    BYTE ids[16 * 64]; DWORD is = sizeof(ids);
    bool okIds = RegBin(base, L"VirtualDesktopIDs", ids, &is);

    int index = 0;
    if (ok && okIds) {
        for (DWORD i = 0; i + 16 <= is; i += 16)
            if (memcmp(ids + i, cur, 16) == 0) { index = (int)(i / 16) + 1; break; }
    }

    if (ok) {
        GUID gd; memcpy(&gd, cur, 16);
        wchar_t gs[64];
        if (StringFromGUID2(gd, gs, 64)) {
            wchar_t p[400];
            _snwprintf(p, 400, L"%s\\Desktops\\%s", base, gs);
            wchar_t nm[64]; DWORD ns = sizeof(nm);
            if (RegGetValueW(HKEY_CURRENT_USER, p, L"Name", RRF_RT_REG_SZ, nullptr, nm, &ns) == ERROR_SUCCESS && nm[0]) {
                wcsncpy(out, nm, n - 1); out[n - 1] = 0;
                return;
            }
        }
    }
    if (index > 0) _snwprintf(out, n, L"%s %d", g_deskLabel, index);
    else _snwprintf(out, n, L"%s", g_deskLabel);
    out[n - 1] = 0;
}

// ---------------------------------------------------------------- lecture des valeurs
static int ReadVolume(IMMDeviceEnumerator* en, bool* mute) {
    IMMDevice* dev = nullptr; IAudioEndpointVolume* ep = nullptr; int res = -1;
    if (SUCCEEDED(en->GetDefaultAudioEndpoint(eRender, eMultimedia, &dev)) && dev) {
        if (SUCCEEDED(dev->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_ALL, nullptr, (void**)&ep)) && ep) {
            float lv = 0; BOOL m = FALSE;
            if (SUCCEEDED(ep->GetMasterVolumeLevelScalar(&lv))) {
                res = (int)(lv * 100.0f + 0.5f);
                ep->GetMute(&m);
                *mute = m != FALSE;
            }
            ep->Release();
        }
        dev->Release();
    }
    return res;
}

struct DispBrightness { UCHAR policy, ac, dc; };
#define IOCTL_QUERY_BRIGHTNESS CTL_CODE(0x23, 0x126, METHOD_BUFFERED, FILE_ANY_ACCESS)

static int ReadLcd(HANDLE h) {
    if (h == INVALID_HANDLE_VALUE) return -1;
    DispBrightness d{}; DWORD ret = 0;
    if (!DeviceIoControl(h, IOCTL_QUERY_BRIGHTNESS, nullptr, 0, &d, sizeof(d), &ret, nullptr)) return -1;
    SYSTEM_POWER_STATUS ps{};
    bool ac = GetSystemPowerStatus(&ps) ? (ps.ACLineStatus == 1) : true;
    return ac ? d.ac : d.dc;
}

static int ReadDdc() {
    HMONITOR hm = MonitorFromPoint(POINT{0, 0}, MONITOR_DEFAULTTOPRIMARY);
    DWORD n = 0;
    if (!GetNumberOfPhysicalMonitorsFromHMONITOR(hm, &n) || n == 0) return -1;
    PHYSICAL_MONITOR* pm = (PHYSICAL_MONITOR*)calloc(n, sizeof(PHYSICAL_MONITOR));
    if (!pm) return -1;
    int res = -1;
    if (GetPhysicalMonitorsFromHMONITOR(hm, n, pm)) {
        DWORD mn = 0, cur = 0, mx = 0;
        if (GetMonitorBrightness(pm[0].hPhysicalMonitor, &mn, &cur, &mx) && mx > mn)
            res = (int)((cur - mn) * 100 / (mx - mn));
        DestroyPhysicalMonitors(n, pm);
    }
    free(pm);
    return res;
}

static void PostValue(int kind, int v, bool mute) {
    if (g_ui) PostMessageW(g_ui, MSG_VALUES, (WPARAM)kind, (LPARAM)(v | (mute ? 0x100 : 0)));
}

static DWORD WINAPI PollThread(LPVOID) {
    bool comOk = SUCCEEDED(CoInitializeEx(nullptr, COINIT_MULTITHREADED));
    IMMDeviceEnumerator* en = nullptr;
    CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), (void**)&en);

    HANDLE hLcd = CreateFileW(L"\\\\.\\LCD", GENERIC_READ | GENERIC_WRITE,
                              FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, 0, nullptr);
    bool lcdOk = ReadLcd(hLcd) >= 0;

    int pv = -1, pm = 0, pb = -1;
    DWORD lastDdc = GetTickCount();
    if (lcdOk) pb = ReadLcd(hLcd); else pb = ReadDdc();
    g_bri = pb;

    wchar_t ld[64] = L""; DWORD lastDeskPoll = 0;
    ReadDesktopName(ld, 64);

    while (g_run) {
        DWORD now = GetTickCount();

        if (en) {
            bool m = false;
            int v = ReadVolume(en, &m);
            if (v >= 0 && (v != pv || (int)m != pm)) {
                bool first = pv < 0;
                pv = v; pm = m ? 1 : 0;
                g_vol = v; g_mute = pm;
                if (!first) { g_volTick = GetTickCount(); PostValue(0, v, m); }
            }
        }

        int b = -1;
        if (lcdOk) b = ReadLcd(hLcd);
        else if ((LONG)(g_wantDdcUntil - now) > 0 && now - lastDdc > 250) { b = ReadDdc(); lastDdc = now; }
        if (b >= 0 && b != pb) {
            pb = b; g_bri = b; g_briTick = GetTickCount();
            PostValue(1, b, false);
        }

        if (now - lastDeskPoll > 150) {
            wchar_t nn[64];
            ReadDesktopName(nn, 64);
            if (wcscmp(nn, ld) != 0) {
                wcscpy(ld, nn);
                g_deskTick = GetTickCount();
                if (!g_deskTick) g_deskTick = 1;
            }
            lastDeskPoll = now;
        }
        Sleep(50);
    }

    if (hLcd != INVALID_HANDLE_VALUE) CloseHandle(hLcd);
    if (en) en->Release();
    if (comOk) CoUninitialize();
    return 0;
}

// ---------------------------------------------------------------- rendu
enum { S_HIDDEN, S_PENDING, S_OPEN, S_CLOSING };
static int st = S_HIDDEN;
static DWORD stStart = 0, lastTick = 0, lastDeskRead = 0;
static int g_kind = 0;                   // 0 volume, 1 luminosité, 2 bureau
static int g_pendKind = 0;               // 0 incertain, 1 touche volume, 2 Win+Ctrl
static DWORD g_showTick = 0;
static bool g_muted = false;
static bool g_unfolded = false, g_phase2 = false;
static wchar_t g_deskName[64] = L"";

static Spring sp_p, sp_y, sp_v;
static float g_curV = 0, g_p = 0, g_ga = 0, g_yOff = 0, g_startOff = 0;

static float g_s = 1.0f, g_W = 220, g_H = 44;
static int g_pad = 16, g_padY = 16, g_cw = 0, g_ch = 0;
static POINT g_dst{0, 0};
static LONG g_baseY = 0;

static HDC g_scrDC = nullptr, g_dc = nullptr, g_maskDC = nullptr;
static HBITMAP g_bmp = nullptr, g_maskBmp = nullptr, g_oldBmp = nullptr, g_oldMask = nullptr;
static DWORD* g_bits = nullptr; static BYTE* g_maskBits = nullptr;
static HFONT g_fIcon = nullptr, g_fText = nullptr;

// valeurs actives (volume/luminosité ou bureau)
static int a_w = 220, a_h = 44, a_bottom = 70, a_hold = 120, a_closeHold = 140, g_fontH = 0;
static DWORD g_deskT0 = 0;
static bool g_hidePending = false;

// Impulsion directionnelle : déplacement horizontal + compression verticale.
static Spring sp_sh;
static Spring sp_shape;                 // enveloppe de déformation indépendante du déplacement
static float g_shakeK = 100, g_shakeC = 5, g_shakeW = 18;
static float g_shapeK = 100, g_shapeC = 20; // ressort d'attaque (compression progressive)
static float g_shapeReleaseK = 100, g_shapeReleaseC = 10; // ressort de retour sous-amorti
static float g_squash = 0, g_stretch = 0;
static volatile LONG g_keyDir = 0;       // +1 volume +, -1 volume -, 0 autre
static bool g_pendBump = false;          // secousse à jouer une fois la pilule dépliée
static DWORD g_lastBump = 0;
static bool g_volumeKeyWasHeld = false; // permet un retour au relâchement physique de la touche
static bool g_shapeReturning = false; // garantit un vrai retour, sans temporisation répétée

// mode compact (enchaînement de changements)
static Spring sp_c;                      // 0 = pilule normale, 1 = compacte autour de la barre
static DWORD g_lastChg = 0;
static int g_chain = 0;                  // nombre de changements rapprochés

static HBITMAP MakeDib(HDC dc, int w, int h, void** bits) {
    BITMAPINFO bi{};
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = w; bi.bmiHeader.biHeight = -h;
    bi.bmiHeader.biPlanes = 1; bi.bmiHeader.biBitCount = 32; bi.bmiHeader.biCompression = BI_RGB;
    return CreateDIBSection(dc, &bi, DIB_RGB_COLORS, bits, nullptr, 0);
}

static void FreeCanvas() {
    if (g_dc && g_oldBmp) SelectObject(g_dc, g_oldBmp);
    if (g_maskDC && g_oldMask) SelectObject(g_maskDC, g_oldMask);
    if (g_bmp) DeleteObject(g_bmp);
    if (g_maskBmp) DeleteObject(g_maskBmp);
    g_bmp = g_maskBmp = nullptr; g_bits = nullptr; g_maskBits = nullptr;
    g_oldBmp = g_oldMask = nullptr;
}

static void EnsureCanvas() {
    g_W = a_w * g_s; g_H = a_h * g_s;

    // La pilule peut devenir beaucoup plus large qu'au repos. Le bitmap doit
    // pouvoir contenir l'etirement maximal ET le depassement du ressort
    // d'ouverture (g_p jusqu'a 1.12), sinon son contour est coupe aux bords.
    const float stretchMax = (g_kind == 2) ? 0.0f :
        1.65f * Clamp(g.shakeWidthPct / 100.0f, 0.0f, 1.0f);
    const float maxBodyW = g_W * (1.0f + stretchMax) * 1.12f;
    const int neededPad = (int)ceilf((maxBodyW - g_W) * 0.5f + 20.0f * g_s);
    g_pad = (int)ceilf(Clamp((float)neededPad, 24.0f * g_s, 1000.0f * g_s));

    // Une marge verticale independante evite de dessiner inutilement un bitmap
    // gigantesque lorsque seul l'etirement horizontal augmente.
    g_padY = (int)ceilf(fmaxf(20.0f * g_s, g_H * 0.25f));
    int cw = (int)ceilf(g_W) + 2 * g_pad;
    int ch = (int)ceilf(g_H) + 2 * g_padY;
    if (g_bmp && cw == g_cw && ch == g_ch) return;
    FreeCanvas();
    g_cw = cw; g_ch = ch;
    g_bmp = MakeDib(g_scrDC, cw, ch, (void**)&g_bits);
    g_maskBmp = MakeDib(g_scrDC, cw, ch, (void**)&g_maskBits);
    g_oldBmp = (HBITMAP)SelectObject(g_dc, g_bmp);
    g_oldMask = (HBITMAP)SelectObject(g_maskDC, g_maskBmp);
}

static float CapDist(float px, float py, float xa, float xb, float cy, float r) {
    if (xb < xa) xa = xb = (xa + xb) * 0.5f;
    float qx = Clamp(px, xa, xb), dx = px - qx, dy = py - cy;
    return sqrtf(dx * dx + dy * dy) - r;
}

static wchar_t IconGlyph() {
    if (g_kind == 2) return 0xE7F4;                 // écran / bureau
    if (g_kind == 1) return 0xE706;                 // luminosité
    if (g_muted) return 0xE74F;                     // muet
    int v = (int)(g_curV + 0.5f);
    if (v <= 0) return 0xE992;
    if (v < 34) return 0xE993;
    if (v < 67) return 0xE994;
    return 0xE995;
}

static void Render() {
    if (!g_bits || !g_maskBits || !g_ui) return;
    const int CW = g_cw, CH = g_ch;
    const bool desk = (g_kind == 2);

    // Progression du ressort compact. On conserve un peu de dépassement
    // au-dessus et en dessous de la cible pour rendre le rebond visible.
    const float cm = desk ? 0.0f : Clamp(sp_c.x, -0.18f, 1.22f);

    // Le contour rétrécit progressivement de 5 % en mode compact.
    // L'impulsion comprime aussi brièvement la pilule sur son axe vertical.
    const float compactScale = Lerp(1.0f, 0.95f, cm);
    const float s = g_s;
    // Chaque impulsion comprime la hauteur et élargit légèrement la pilule.
    // L'intensité suit la vitesse du ressort : les impulsions rapprochées
    // s'additionnent et produisent un déplacement/une déformation plus marqués.
    const float Hf = g_H * compactScale * (1.0f - g_squash);
    const float Wf = g_W * compactScale * (1.0f + g_stretch);
    const float vShown = Clamp(g_curV, 0, 100);
    float cx = CW * 0.5f, cy = CH * 0.5f, r = Hf * 0.5f;
    float curW = Hf + (Wf - Hf) * g_p;
    float x0 = cx - curW * 0.5f, x1 = cx + curW * 0.5f;
    float ca = Clamp((g_p - 0.55f) / 0.45f, 0, 1);
    ca = ca * ca * (3 - 2 * ca);
    float iconCx = x0 + r;
    float textR = x1 - 12 * s, textL = textR - 36 * s;
    float bx0 = x0 + Hf - 4 * s, bx1 = textL - 8 * s;
    if (bx1 < bx0 + 2) bx1 = bx0 + 2;
    float tr = 3 * s * (1.0f - g_squash);

    // ---- mode compact : le contour se resserre autour de la barre, qui s'étire
    const float trC = 4 * s * (1.0f - g_squash), cRad = trC + 4 * s * (1.0f - g_squash); // barre et rayon compact
    const float Lc = Wf * 0.78f;                         // longueur de la barre en mode compact
    float rr0 = Lerp(r, cRad, cm);
    float hw = Lerp(curW * 0.5f, Lc * 0.5f + 4 * s, cm);
    float sa = cx - hw + rr0, sb = cx + hw - rr0;        // centres des extrémités du contour
    bx0 = Lerp(bx0, cx - Lc * 0.5f, cm);
    bx1 = Lerp(bx1, cx + Lc * 0.5f, cm);
    tr  = Lerp(tr, trC, cm);
    const float fadeC = 1.0f - Clamp(cm * 2.2f, 0, 1);   // icône et valeur disparaissent

    memset(g_maskBits, 0, (size_t)CW * CH * 4);
    SetBkMode(g_maskDC, TRANSPARENT);
    wchar_t glyph[2] = { IconGlyph(), 0 };
    SelectObject(g_maskDC, g_fIcon);
    SetTextColor(g_maskDC, RGB(255, 0, 0));
    RECT ir = { (LONG)(iconCx - r), (LONG)(cy - r), (LONG)(iconCx + r), (LONG)(cy + r) };
    DrawTextW(g_maskDC, glyph, 1, &ir, DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOCLIP);
    if (ca > 0.01f) {
        SelectObject(g_maskDC, g_fText);
        SetTextColor(g_maskDC, RGB(0, 255, 0));
        if (desk) {
            RECT tr2 = { (LONG)(x0 + Hf - 2 * s), (LONG)(cy - r), (LONG)textR, (LONG)(cy + r) };
            DrawTextW(g_maskDC, g_deskName, -1, &tr2,
                      DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS);
        } else {
            wchar_t num[8];
            _snwprintf(num, 8, L"%d", (int)(vShown + 0.5f));
            RECT tr2 = { (LONG)textL, (LONG)(cy - r), (LONG)textR, (LONG)(cy + r) };
            DrawTextW(g_maskDC, num, -1, &tr2, DT_RIGHT | DT_VCENTER | DT_SINGLELINE | DT_NOCLIP);
        }
    }
    GdiFlush();

    float accR = g_muted ? 150 : 96, accG = g_muted ? 150 : 205, accB = g_muted ? 150 : 255;
    float fillW = (bx1 - bx0) * vShown / 100.0f;
    float fa = bx0 + tr, fb = bx0 + (fillW > 2 * tr ? fillW : 2 * tr) - tr;

    for (int y = 0; y < CH; y++) {
        for (int x = 0; x < CW; x++) {
            float px = x + 0.5f, py = y + 0.5f;
            float d = CapDist(px, py, sa, sb, cy, rr0);
            float cov = Clamp(0.5f - d, 0, 1);
            if (cov <= 0) { g_bits[y * CW + x] = 0; continue; }

            float R = 32, G = 32, B = 32;
            float bf = Clamp((d + 1.5f) / 1.5f, 0, 1) * 0.8f;
            R = Lerp(R, 95, bf); G = Lerp(G, 95, bf); B = Lerp(B, 95, bf);

            if (!desk && ca > 0.001f) {
                float td = CapDist(px, py, bx0 + tr, bx1 - tr, cy, tr);
                float tc = Clamp(0.5f - td, 0, 1) * ca * 0.28f;
                R = Lerp(R, 255, tc); G = Lerp(G, 255, tc); B = Lerp(B, 255, tc);
                if (vShown > 0.2f) {
                    float fd = CapDist(px, py, fa, fb, cy, tr);
                    float fc = Clamp(0.5f - fd, 0, 1) * ca;
                    R = Lerp(R, accR, fc); G = Lerp(G, accG, fc); B = Lerp(B, accB, fc);
                }
            }
            BYTE* m = (BYTE*)&g_maskBits[(y * CW + x) * 4];     // B G R A
            float ic = m[2] / 255.0f * fadeC, tx = m[1] / 255.0f * ca * fadeC;
            R = Lerp(R, 255, ic); G = Lerp(G, 255, ic); B = Lerp(B, 255, ic);
            R = Lerp(R, 255, tx); G = Lerp(G, 255, tx); B = Lerp(B, 255, tx);

            float A = 0.94f * cov * g_ga;
            DWORD a = (DWORD)(A * 255.0f + 0.5f);
            DWORD rr = (DWORD)(R * A + 0.5f), gg = (DWORD)(G * A + 0.5f), bb = (DWORD)(B * A + 0.5f);
            g_bits[y * CW + x] = (a << 24) | (rr << 16) | (gg << 8) | bb;
        }
    }

    BLENDFUNCTION bf{ AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };
    POINT src{0, 0}; SIZE sz{ CW, CH };
    // Le ressort pousse la pilule horizontalement dans le sens du changement.
    // Valeur positive = droite ; valeur négative = gauche.
    POINT dst{
        g_dst.x + (LONG)lroundf(sp_sh.x),
        g_baseY + (LONG)lroundf(g_yOff) + (LONG)lroundf(8.0f * g_s * cm)
    };
    UpdateLayeredWindow(g_ui, g_scrDC, &dst, &sz, g_dc, &src, 0, &bf, ULW_ALPHA);
}

// ---------------------------------------------------------------- profils
static void MakeFonts(int h) {
    if (g_fIcon && g_fText && h == g_fontH) return;
    if (g_maskDC) SelectObject(g_maskDC, GetStockObject(SYSTEM_FONT));   // libère les anciennes polices
    if (g_fIcon) DeleteObject(g_fIcon);
    if (g_fText) DeleteObject(g_fText);
    g_fIcon = CreateFontW(-(int)(h * 0.33f * g_s), 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET,
                          OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY,
                          DEFAULT_PITCH, L"Segoe Fluent Icons");
    g_fText = CreateFontW(-(int)(h * 0.28f * g_s), 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET,
                          OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY,
                          DEFAULT_PITCH, L"Segoe UI");
    g_fontH = h;
}

static void UseProfile(bool desk) {
    if (desk) {
        a_w = dk.widthPx; a_h = dk.heightPx; a_bottom = dk.bottomOffset;
        a_hold = dk.holdMs; a_closeHold = dk.closeHoldMs;
        ComputeSprings(dk.responseMs, dk.dampingPct);
    } else {
        a_w = g.widthPx; a_h = g.heightPx; a_bottom = g.bottomOffset;
        a_hold = g.holdMs; a_closeHold = g.closeHoldMs;
        ComputeSprings(g.responseMs, g.dampingPct);
    }
    MakeFonts(a_h);
}

static void StartClose() {
    st = S_CLOSING; stStart = GetTickCount();
    g_phase2 = false; g_hidePending = false;
    sp_p.t = 0;
    // Si le retour automatique est désactivé, conserver la forme compacte
    // pendant toute l'animation de disparition.
    if (g.compactReturnToNormal) sp_c.t = 0;
}

// ---------------------------------------------------------------- secousse
static void ComputeShake() {
    float period = g.shakeMs / 1000.0f;
    g_shakeW = 6.2831853f / period;
    MakeSpringParams(period, Clamp(g.shakeDampPct / 100.0f, 0.1f, 1.0f), &g_shakeK, &g_shakeC);

    // Deux réponses indépendantes : montée rapide et sans à-coups pendant
    // le maintien, retour lent et élastique au relâchement.
    MakeSpringParams(0.32f, 0.86f, &g_shapeK, &g_shapeC);
    // 100 % conserve exactement les reglages du ressort de la v4.4.
    // Une puissance plus grande laisse davantage de rebond (moins d'amortissement).
    const float power = Clamp(g.returnPowerPct / 100.0f, 0.0f, 3.0f);
    const float releaseDamping = Clamp(0.32f / sqrtf(fmaxf(power, 0.35f)), 0.18f, 0.80f);
    MakeSpringParams(0.70f, releaseDamping, &g_shapeReleaseK, &g_shapeReleaseC);
}

static void Bump(int dir, float scale) {
    if (g.shakePx <= 0 || scale <= 0) return;

    // Chaque changement décale doucement la cible dans la direction correspondante.
    // Les changements successifs s'accumulent, sans ajouter de coups de vitesse brusques.
    const float maxOffset = g.shakePx * g_s * 1.8f;
    sp_sh.t = Clamp(sp_sh.t + dir * scale * g.shakePx * g_s * 0.35f,
                    -maxOffset, maxOffset);

    // La compression augmente progressivement tant que les valeurs s'enchaînent.
    // La cible est plafonnée pour éviter que la pilule ne disparaisse presque.
    sp_shape.t = Clamp(sp_shape.t + scale * 0.22f, 0.0f, 0.95f);
    g_shapeReturning = false;
    g_lastBump = GetTickCount();
}

// Appui sur volume +/- alors que le niveau est déjà au max / min
static void CheckLimitKeys(DWORD now) {
    if (g_kind != 0 || g_muted) return;
    bool up = (GetAsyncKeyState(VK_VOLUME_UP) & 0x8000) != 0;
    bool dn = (GetAsyncKeyState(VK_VOLUME_DOWN) & 0x8000) != 0;
    bool atMax = up && g_vol >= 100;
    bool atMin = dn && g_vol <= 0;
    if ((atMax || atMin) && now - g_lastBump > 260)      // se répète si la touche reste enfoncée
        Bump(atMax ? 1 : -1, 1.0f);
}

// ---------------------------------------------------------------- machine d'états
static void LoadValues() {
    g_muted = false;
    float v = 0;
    if (g_kind == 2) { ReadDesktopName(g_deskName, 64); lastDeskRead = GetTickCount(); }
    else if (g_kind == 0) { v = (float)(g_vol < 0 ? 0 : g_vol); g_muted = g_mute != 0; }
    else { v = (float)(g_bri < 0 ? 50 : g_bri); }
    sp_v.x = sp_v.t = v; sp_v.v = 0;
    g_curV = v;
}

static void BeginShow() {
    UseProfile(g_kind == 2);
    EnsureCanvas();
    MONITORINFO mi{}; mi.cbSize = sizeof(mi);
    GetMonitorInfoW(MonitorFromPoint(POINT{0, 0}, MONITOR_DEFAULTTOPRIMARY), &mi);
    g_dst.x = (mi.rcMonitor.left + mi.rcMonitor.right) / 2 - g_cw / 2;
    g_dst.y = mi.rcMonitor.bottom - (LONG)(a_bottom * g_s) - (LONG)g_H - g_padY;
    g_baseY = g_dst.y;
    g_startOff = (float)(mi.rcMonitor.bottom - g_baseY) + 4;

    sp_y.x = g_startOff; sp_y.v = 0; sp_y.t = 0;
    sp_p.x = 0; sp_p.v = 0; sp_p.t = 0;
    g_unfolded = false; g_phase2 = false;

    LoadValues();
    g_p = 0; g_ga = 0; g_yOff = g_startOff;
    sp_sh.x = 0; sp_sh.v = 0; sp_sh.t = 0;
    sp_shape.x = 0; sp_shape.v = 0; sp_shape.t = 0;
    g_lastBump = GetTickCount();
    g_volumeKeyWasHeld = false;
    g_shapeReturning = false;
    g_squash = 0; g_stretch = 0;

    g_bumpDir = 0;
    if (g_kind == 0 && !g_muted) {
        if (g_keyDir > 0 && g_vol >= 100) g_bumpDir = 1;
        else if (g_keyDir < 0 && g_vol <= 0) g_bumpDir = -1;
    } else if (g_kind == 1 && g_limitDir) g_bumpDir = g_limitDir;
    g_pendBump = g_bumpDir != 0;

    sp_c.x = 0; sp_c.v = 0; sp_c.t = 0;
    g_chain = g.compactEnabled ? 1 : 0;
    g_lastChg = GetTickCount();
    // Le premier appui est celui qui a déclenché l'affichage.
    if (g.compactEnabled && g.compactTriggerCount <= 1) sp_c.t = 1;

    Render();
    g_deskT0 = GetTickCount(); g_hidePending = false;
    st = S_OPEN; stStart = GetTickCount(); lastTick = stStart;
    SetWindowPos(g_ui, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_SHOWWINDOW);
}

static bool Recent(DWORD tick) {
    return tick != 0 && (LONG)(tick - (g_showTick - 250)) >= 0;
}

static void Tick() {
    DWORD now = GetTickCount();
    float dt = Clamp((now - lastTick) / 1000.0f, 0.001f, 0.05f);
    lastTick = now;

    if (st == S_PENDING) {
        DWORD el = now - stStart;
        if (g_pendKind == 2 || (g_pendKind == 0 && Recent(g_deskTick))) {
            if ((int)el >= g.startDelayMs) { g_kind = 2; BeginShow(); }
        } else {
            bool rv = Recent(g_volTick), rb = Recent(g_briTick);
            if (g_pendKind == 1 || rv || rb) {
                if ((int)el >= g.startDelayMs) {
                    if (g_pendKind == 1) g_kind = 0;
                    else if (rv && rb) g_kind = (LONG)(g_briTick - g_volTick) > 0 ? 1 : 0;
                    else g_kind = rb ? 1 : 0;
                    BeginShow();
                }
            } else if ((int)el >= g.decideMs) {
                int bri = g_bri;
                if (bri >= 100 || (bri >= 0 && bri <= 1)) {    // luminosité en butée
                    g_kind = 1; g_limitDir = bri >= 100 ? 1 : -1;
                } else {
                    g_kind = 2;                                // changement de bureau
                }
                BeginShow();
            }
        }
        return;
    }
    if (st == S_OPEN) {
        if (!g_unfolded && (int)(now - stStart) >= a_hold) {
            g_unfolded = true; sp_p.t = 1;
            if (g_pendBump) { Bump(g_bumpDir, 1.0f); g_pendBump = false; }
        }
        CheckLimitKeys(now);
        if (!g.compactEnabled) {
            // Désactivation immédiate du mode compact si l'option est décochée.
            sp_c.t = 0;
            g_chain = 0;
        } else if (g.compactReturnToNormal && sp_c.t > 0.5f) {
            // Le retour au mode normal est optionnel.
            bool held = g_kind == 0 &&
                (((GetAsyncKeyState(VK_VOLUME_UP) | GetAsyncKeyState(VK_VOLUME_DOWN)) & 0x8000) != 0);
            if (held) g_lastChg = now;
            else if (g.compactReleaseMs > 0 &&
                     (int)(now - g_lastChg) > g.compactReleaseMs) {
                sp_c.t = 0;
                g_chain = 0;
            }
        }
        g_ga = Clamp(g_ga + dt / 0.09f, 0, 1);
        g_wantDdcUntil = now + 1500;
        if (g_kind == 2) {
            if (now - lastDeskRead > 120) {
                wchar_t nn[64];
                ReadDesktopName(nn, 64);
                if (wcscmp(nn, g_deskName) != 0) {        // nouveau changement de bureau
                    wcscpy(g_deskName, nn);
                    g_deskT0 = now; g_hidePending = false;
                }
                lastDeskRead = now;
            }
            bool closeNow = dk.durationMs > 0 ? (int)(now - g_deskT0) >= dk.durationMs
                                              : g_hidePending;
            if (closeNow) StartClose();
        }
    } else if (st == S_CLOSING) {
        g_ga = 1;
        if (!g_phase2 && (int)(now - stStart) >= a_closeHold) { g_phase2 = true; sp_y.t = g_startOff; }
        if ((g_phase2 && sp_y.x >= g_startOff - 2) || (int)(now - stStart) > 3000) {
            ShowWindow(g_ui, SW_HIDE);
            st = S_HIDDEN;
            return;
        }
    } else return;

    Step(sp_p, dt, g_k, g_c);
    Step(sp_y, dt, g_k, g_c);
    Step(sp_v, dt, g_kv, g_cv);
    // Les événements audio Windows n'arrivent pas nécessairement à intervalles
    // réguliers. Ne JAMAIS relâcher le ressort entre deux événements tant que
    // la touche volume est physiquement maintenue.
    const bool volumeHeld = g_kind == 0 &&
        (((GetAsyncKeyState(VK_VOLUME_UP) | GetAsyncKeyState(VK_VOLUME_DOWN)) & 0x8000) != 0);
    const bool releasedNow = g_volumeKeyWasHeld && !volumeHeld;
    g_volumeKeyWasHeld = volumeHeld;

    // Pour les touches volume visibles via GetAsyncKeyState : retour immédiat
    // dès le relâchement. Pour la luminosité, les touches Fn non détectables
    // et les contrôles logiciels : petit délai de regroupement des événements,
    // pour préserver l'accumulation durant un maintien.
    const bool eventsFinished = g_lastBump && (now - g_lastBump >= 170);
    if (!g_shapeReturning && (releasedNow || (!volumeHeld && eventsFinished))) {
        sp_sh.t = 0;
        sp_shape.t = 0;
        g_shapeReturning = true;

        // Petite impulsion supplementaire au relachement : elle accelere le
        // retour vers le repos et genere une surcompensation plus visible.
        // 100 % = comportement v4.4 ; moins de 100 % adoucit le ressort.
        const float extra = Clamp(g.returnPowerPct / 100.0f - 1.0f, -1.0f, 2.0f);
        if (extra > 0.0f) {
            sp_shape.v -= extra * 0.72f * sqrtf(g_shapeReleaseK) * fmaxf(sp_shape.x, 0.0f);
            sp_sh.v -= extra * 0.24f * g_shakeW * sp_sh.x;
        }
    }
    Step(sp_sh, dt, g_shakeK, g_shakeC);
    Step(sp_shape, dt,
         g_shapeReturning ? g_shapeReleaseK : g_shapeK,
         g_shapeReturning ? g_shapeReleaseC : g_shapeC);
    Step(sp_c, dt, g_kc, g_cc);

    g_p = Clamp(sp_p.x, 0, 1.12f);
    g_yOff = sp_y.x;
    g_curV = sp_v.x;
    {
        // L'enveloppe de forme dure plus longtemps que la simple impulsion de vitesse.
        // Elle permet une compression/extension visible, puis une légère surcompensation
        // avant le retour au repos. Les impulsions rapprochées se cumulent dans ce ressort.
        // Autorise une surcompensation légèrement plus ample au retour,
        // mais garde la même compression maximale que la version appréciée.
        const float intensity = Clamp(sp_shape.x / 0.58f, -0.65f, 1.65f);
        // Valeur positive : compression + élargissement. Valeur négative :
        // légère surcompensation (la pilule grandit et se resserre) au rebond.
        g_squash = intensity * 0.34f;
        g_stretch = intensity * (g.shakeWidthPct / 100.0f);
    }
    Render();
}

static LRESULT CALLBACK UiProc(HWND h, UINT msg, WPARAM w, LPARAM l) {
    switch (msg) {
    case MSG_SHOW: {
        DWORD now = GetTickCount();
        g_wantDdcUntil = now + 2000;
        if (st == S_HIDDEN) {
            g_limitDir = 0;
            g_pendKind = (int)w; g_showTick = now;
            st = S_PENDING; stStart = now; lastTick = now;
        } else if (st == S_CLOSING) {
            st = S_OPEN; stStart = now;
            g_unfolded = true; g_phase2 = false;
            sp_p.t = 1; sp_y.t = 0;
            g_deskT0 = now; g_hidePending = false;
            SetWindowPos(h, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_SHOWWINDOW);
        } else if (st == S_OPEN) {
            g_deskT0 = now; g_hidePending = false;
        }
        return 0; }
    case MSG_HIDE:
        if (st == S_PENDING) { st = S_HIDDEN; }
        else if (st == S_OPEN) {
            if (g_kind == 2) g_hidePending = true;   // fermeture gérée dans Tick (durée)
            else StartClose();
        }
        return 0;
    case MSG_VALUES:
        if (st == S_OPEN) {
            if (g_kind == 2) return 0;               // ne pas écraser la pilule bureau
            int kind = (int)w, v = (int)(l & 0xFF);
            bool mute = (l & 0x100) != 0;
            if (kind != g_kind) {
                g_kind = kind; sp_v.x = (float)v; sp_v.v = 0;
                sp_c.t = 0; g_chain = 0;
            } else if ((float)v != sp_v.t) {         // la valeur a changé
                Bump(v > sp_v.t ? 1 : -1, g.valueShakePct / 100.0f);
                DWORD n = GetTickCount();
                if (g.compactEnabled) {
                    // compactReleaseMs sert aussi de fenêtre pour compter une série
                    // de pressions. À 0, on compte pendant toute la durée d'affichage.
                    bool sameSequence = g.compactReleaseMs <= 0 ||
                        (int)(n - g_lastChg) <= g.compactReleaseMs;
                    g_chain = sameSequence ? g_chain + 1 : 1;
                    g_lastChg = n;
                    if (g_chain >= g.compactTriggerCount) sp_c.t = 1;
                } else {
                    g_chain = 0;
                    sp_c.t = 0;
                }
            }
            sp_v.t = (float)v; g_muted = mute;
        }
        return 0;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProcW(h, msg, w, l);
}

static DWORD WINAPI UiThread(LPVOID) {
    HINSTANCE hi = GetModuleHandleW(nullptr);
    WNDCLASSW wc{};
    wc.lpfnWndProc = UiProc; wc.hInstance = hi; wc.lpszClassName = L"WhOsdCustomPill";
    wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    RegisterClassW(&wc);

    g_scrDC = GetDC(nullptr);
    g_dc = CreateCompatibleDC(g_scrDC);
    g_maskDC = CreateCompatibleDC(g_scrDC);
    UINT dpi = GetDpiForSystem();
    g_s = (dpi ? dpi : 96) / 96.0f;
    g_pad = (int)(16 * g_s);
    UseProfile(false);
    EnsureCanvas();

    HWND hwnd = CreateWindowExW(WS_EX_LAYERED | WS_EX_TOPMOST | WS_EX_TOOLWINDOW |
                                WS_EX_NOACTIVATE | WS_EX_TRANSPARENT,
                                wc.lpszClassName, L"", WS_POPUP, 0, 0, 1, 1,
                                nullptr, nullptr, hi, nullptr);
    g_ui = hwnd;

    bool quit = false;
    while (!quit) {
        MSG m;
        while (PeekMessageW(&m, nullptr, 0, 0, PM_REMOVE)) {
            if (m.message == WM_QUIT) { quit = true; break; }
            TranslateMessage(&m); DispatchMessageW(&m);
        }
        if (quit) break;
        if (st != S_HIDDEN) {
            DWORD t = GetTickCount();
            DwmFlush();
            if (GetTickCount() - t < 4) Sleep(8);
            Tick();
        } else {
            WaitMessage();
        }
    }

    g_ui = nullptr;
    FreeCanvas();
    if (g_maskDC) SelectObject(g_maskDC, GetStockObject(SYSTEM_FONT));
    if (g_fIcon) DeleteObject(g_fIcon);
    if (g_fText) DeleteObject(g_fText);
    g_fIcon = g_fText = nullptr;
    if (g_dc) DeleteDC(g_dc);
    if (g_maskDC) DeleteDC(g_maskDC);
    if (g_scrDC) ReleaseDC(nullptr, g_scrDC);
    UnregisterClassW(wc.lpszClassName, hi);
    return 0;
}

// ---------------------------------------------------------------- hook ShowWindow
using ShowWindow_t = decltype(&ShowWindow);
ShowWindow_t ShowWindow_Orig;

BOOL WINAPI ShowWindow_Hook(HWND hwnd, int cmd) {
    if (!g_ui || !IsOsd(hwnd)) return ShowWindow_Orig(hwnd, cmd);

    if (cmd == SW_HIDE) {
        BOOL r = ShowWindow_Orig(hwnd, cmd);
        if (hwnd == g_osd) {
            SetWindowRgn(hwnd, nullptr, FALSE);
            PostMessageW(g_ui, MSG_HIDE, 0, 0);
        }
        return r;
    }
    if (IsWindowVisible(hwnd)) return ShowWindow_Orig(hwnd, cmd);

    RECT rc; GetWindowRect(hwnd, &rc);
    int h = rc.bottom - rc.top;
    if (h < 80 || h > 110) return ShowWindow_Orig(hwnd, cmd);   // autre flyout

    int pend = 0;
    if ((GetAsyncKeyState(VK_VOLUME_UP) | GetAsyncKeyState(VK_VOLUME_DOWN) |
         GetAsyncKeyState(VK_VOLUME_MUTE)) & 0x8000) pend = 1;
    else if (((GetAsyncKeyState(VK_LWIN) | GetAsyncKeyState(VK_RWIN)) & 0x8000) &&
             (GetAsyncKeyState(VK_CONTROL) & 0x8000)) pend = 2;

    g_keyDir = 0;
    if (pend == 1) {
        if (GetAsyncKeyState(VK_VOLUME_UP) & 0x8000) g_keyDir = 1;
        else if (GetAsyncKeyState(VK_VOLUME_DOWN) & 0x8000) g_keyDir = -1;
    }

    g_osd = hwnd;
    SetWindowRgn(hwnd, CreateRectRgn(0, 0, 0, 0), FALSE);       // original invisible
    BOOL r = ShowWindow_Orig(hwnd, cmd);
    PostMessageW(g_ui, MSG_SHOW, (WPARAM)pend, 0);
    return r;
}

// ---------------------------------------------------------------- init
static void LoadSettings() {
    g.startDelayMs  = Wh_GetIntSetting(L"startDelayMs");
    g.decideMs      = Wh_GetIntSetting(L"decideMs");
    g.holdMs        = Wh_GetIntSetting(L"holdMs");
    g.closeHoldMs   = Wh_GetIntSetting(L"closeHoldMs");
    g.responseMs    = Wh_GetIntSetting(L"responseMs");
    g.dampingPct    = Wh_GetIntSetting(L"dampingPct");
    g.widthPx       = Wh_GetIntSetting(L"widthPx");
    g.heightPx      = Wh_GetIntSetting(L"heightPx");
    g.bottomOffset  = Wh_GetIntSetting(L"bottomOffsetPx");
    g.shakePx       = Wh_GetIntSetting(L"shakePx");
    g.shakeMs       = Wh_GetIntSetting(L"shakeMs");
    g.shakeDampPct  = Wh_GetIntSetting(L"shakeDampingPct");
    g.valueShakePct = Wh_GetIntSetting(L"valueShakePct");
    g.shakeWidthPct = Wh_GetIntSetting(L"shakeWidthPct");
    g.returnPowerPct = Wh_GetIntSetting(L"returnPowerPct");
    g.compactEnabled = Wh_GetIntSetting(L"compactEnabled");
    g.compactTriggerCount = Wh_GetIntSetting(L"compactTriggerCount");
    g.compactReturnToNormal = Wh_GetIntSetting(L"compactReturnToNormal");
    g.compactReleaseMs = Wh_GetIntSetting(L"compactReleaseMs");

    dk.widthPx      = Wh_GetIntSetting(L"deskWidthPx");
    dk.heightPx     = Wh_GetIntSetting(L"deskHeightPx");
    dk.bottomOffset = Wh_GetIntSetting(L"deskBottomOffsetPx");
    dk.holdMs       = Wh_GetIntSetting(L"deskHoldMs");
    dk.closeHoldMs  = Wh_GetIntSetting(L"deskCloseHoldMs");
    dk.responseMs   = Wh_GetIntSetting(L"deskResponseMs");
    dk.dampingPct   = Wh_GetIntSetting(L"deskDampingPct");
    dk.durationMs   = Wh_GetIntSetting(L"deskDurationMs");

    PCWSTR lbl = Wh_GetStringSetting(L"deskLabel");
    if (lbl && lbl[0]) { wcsncpy(g_deskLabel, lbl, 31); g_deskLabel[31] = 0; }
    Wh_FreeStringSetting(lbl);

    if (g.startDelayMs < 0) g.startDelayMs = 0;
    if (g.decideMs < 100) g.decideMs = 100;
    if (g.holdMs < 0) g.holdMs = 0;
    if (g.closeHoldMs < 0) g.closeHoldMs = 0;
    if (g.responseMs < 100) g.responseMs = 100;
    if (g.heightPx < 30) g.heightPx = 30;
    if (g.widthPx < g.heightPx + 120) g.widthPx = g.heightPx + 120;
    if (g.shakePx < 0) g.shakePx = 0;
    if (g.shakeMs < 100) g.shakeMs = 100;
    if (g.shakeDampPct < 10) g.shakeDampPct = 10;
    if (g.valueShakePct < 0) g.valueShakePct = 0;
    if (g.shakeWidthPct < 0) g.shakeWidthPct = 0;
    if (g.shakeWidthPct > 45) g.shakeWidthPct = 45;
    if (g.returnPowerPct < 0) g.returnPowerPct = 0;
    if (g.returnPowerPct > 300) g.returnPowerPct = 300;
    if (g.compactTriggerCount < 1) g.compactTriggerCount = 1;
    if (g.compactTriggerCount > 100) g.compactTriggerCount = 100;
    if (g.compactReleaseMs < 0) g.compactReleaseMs = 0;

    if (dk.heightPx < 30) dk.heightPx = 30;
    if (dk.widthPx < dk.heightPx + 80) dk.widthPx = dk.heightPx + 80;
    if (dk.holdMs < 0) dk.holdMs = 0;
    if (dk.closeHoldMs < 0) dk.closeHoldMs = 0;
    if (dk.responseMs < 100) dk.responseMs = 100;
    if (dk.durationMs < 0) dk.durationMs = 0;

    ComputeShake();
}

BOOL Wh_ModInit() {
    LoadSettings();
    g_uiThread = CreateThread(nullptr, 0, UiThread, nullptr, 0, nullptr);
    g_pollThread = CreateThread(nullptr, 0, PollThread, nullptr, 0, nullptr);
    Wh_SetFunctionHook((void*)ShowWindow, (void*)ShowWindow_Hook, (void**)&ShowWindow_Orig);
    return TRUE;
}

void Wh_ModBeforeUninit() {
    g_run = 0;
    HWND ui = g_ui;
    if (ui) PostMessageW(ui, WM_CLOSE, 0, 0);
    if (g_uiThread)   { WaitForSingleObject(g_uiThread, 1000);   CloseHandle(g_uiThread); }
    if (g_pollThread) { WaitForSingleObject(g_pollThread, 1000); CloseHandle(g_pollThread); }
    if (g_osd && IsWindow(g_osd)) SetWindowRgn(g_osd, nullptr, TRUE);
}

void Wh_ModSettingsChanged() { LoadSettings(); }