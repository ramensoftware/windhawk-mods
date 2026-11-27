// ==WindhawkMod==
// @id              tray-eq
// @name            Tray EQ
// @description     System-wide equalizer with tray icon, monitor-correction file and a Windows 11 acrylic flyout
// @description:pt-PT Equalizador para todo o sistema com ícone na bandeja, ficheiro de correção de monitores e flyout acrílico no Windows 11
// @description:pt-BR Equalizador para todo o sistema com ícone na bandeja, arquivo de correção de monitores e flyout acrílico no Windows 11
// @description:es-ES Ecualizador para todo el sistema con icono en la bandeja, archivo de corrección de monitores y flyout acrílico en Windows 11
// @description:fr-FR Égaliseur système avec icône de zone de notification, fichier de correction de moniteur et flyout acrylique sous Windows 11
// @description:de-DE Systemweiter Equalizer mit Tray-Symbol, Monitor-Korrekturdatei und Acryl-Flyout unter Windows 11
// @description:it-IT Equalizzatore di sistema con icona nell'area di notifica, file di correzione del monitor e flyout acrilico su Windows 11
// @description:ru-RU Системный эквалайзер со значком в трее, файлом коррекции монитора и акриловым всплывающим окном в Windows 11
// @description:ja-JP システム全体のイコライザー。トレイアイコン、モニター補正ファイル、Windows 11 のアクリル風フライアウトに対応
// @description:ko-KR 트레이 아이콘, 모니터 보정 파일, Windows 11 아크릴 플라이아웃을 갖춘 시스템 전체 이퀄라이저
// @description:zh-CN 系统级均衡器，带托盘图标、显示器校正文件和 Windows 11 亚克力弹出窗口
// @version         1.0.0
// @author          bacoinz
// @github          https://github.com/bacoinz
// @homepage        https://github.com/bacoinz/Windhawk-Tray-EQ
// @license         MIT
// @include         *
// @architecture    x86-64
// @exclude         audiodg.exe
// @exclude         windhawk.exe
// @compilerOptions -lole32 -lcomdlg32 -lshell32 -luuid -ldwmapi -ld2d1 -ldwrite -ladvapi32 -luser32 -lgdi32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Tray EQ
A lightweight, free alternative to room/monitor-correction software.

Equalizer applied inside every process that plays audio through WASAPI shared mode
(audiodg.exe cannot be injected, so the EQ runs per app, before the Windows mixer).

* Tray icon (shown by the tray host process, default `explorer.exe`):
  left click = EQ flyout, right click = menu, **middle click = turn the EQ on/off**.
* Manual EQ: 10-band graphic **or** parametric (drag nodes; wheel = Q; double click = new node; right click = remove).
* Monitor correction file (Equalizer APO / AutoEQ / REW text: `Preamp: -6 dB`,
  `Filter 1: ON PK Fc 100 Hz Gain -3 dB Q 1.41`), toggled independently from the manual EQ.

Notes: apps must (re)open their audio stream after the mod loads. Exclusive-mode streams are never touched.
Only 64-bit apps are processed. `@include *` is needed because any app can play audio; to limit injection, narrow the process list in the mod's *Advanced* tab.
The shared settings block is read-only for other users; only the current user's processes can change the EQ.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- curveFile: ""
  $name: Monitor correction file
  $name:pt-PT: Ficheiro de correção dos monitores
  $name:pt-BR: Arquivo de correção dos monitores
  $name:es-ES: Archivo de corrección del monitor
  $name:fr-FR: Fichier de correction du moniteur
  $name:de-DE: Monitor-Korrekturdatei
  $name:it-IT: File di correzione del monitor
  $name:ru-RU: Файл коррекции монитора
  $name:ja-JP: モニター補正ファイル
  $name:ko-KR: 모니터 보정 파일
  $name:zh-CN: 显示器校正文件
  $description: Full path. Optional, you can also load it from the tray flyout.
  $description:pt-PT: Caminho completo. Opcional, também pode ser carregado a partir do flyout da bandeja.
  $description:pt-BR: Caminho completo. Opcional, também pode ser carregado pelo flyout da bandeja.
  $description:es-ES: Ruta completa. Opcional, también se puede cargar desde el flyout de la bandeja.
  $description:fr-FR: Chemin complet. Facultatif, peut aussi être chargé depuis le flyout de la zone de notification.
  $description:de-DE: Vollständiger Pfad. Optional, kann auch über das Tray-Flyout geladen werden.
  $description:it-IT: Percorso completo. Facoltativo, può essere caricato anche dal flyout dell'area di notifica.
  $description:ru-RU: Полный путь. Необязательно, файл можно загрузить и из всплывающего окна в трее.
  $description:ja-JP: フルパス。省略可能で、トレイのフライアウトから読み込むこともできます。
  $description:ko-KR: 전체 경로입니다. 선택 사항이며 트레이 플라이아웃에서도 불러올 수 있습니다.
  $description:zh-CN: 完整路径。可选，也可以从托盘弹出窗口加载。
- trayHost: explorer.exe
  $name: Process that shows the tray icon
  $description: Executable name, e.g. explorer.exe. The tray icon and flyout run inside this process.
  $name:pt-PT: Processo que mostra o ícone da bandeja
  $name:pt-BR: Processo que exibe o ícone da bandeja
  $name:es-ES: Proceso que muestra el icono de la bandeja
  $name:fr-FR: Processus qui affiche l'icône de notification
  $name:de-DE: Prozess, der das Tray-Symbol anzeigt
  $name:it-IT: Processo che mostra l'icona nell'area di notifica
  $name:ru-RU: Процесс, показывающий значок в трее
  $name:ja-JP: トレイアイコンを表示するプロセス
  $name:ko-KR: 트레이 아이콘을 표시하는 프로세스
  $name:zh-CN: 显示托盘图标的进程
- backdrop: acrylic
  $name: Flyout backdrop
  $name:pt-PT: Fundo do flyout
  $name:pt-BR: Fundo do flyout
  $name:es-ES: Fondo del flyout
  $name:fr-FR: Arrière-plan du flyout
  $name:de-DE: Flyout-Hintergrund
  $name:it-IT: Sfondo del flyout
  $name:ru-RU: Фон всплывающего окна
  $name:ja-JP: フライアウトの背景
  $name:ko-KR: 플라이아웃 배경
  $name:zh-CN: 弹出窗口背景
  $options:
  - acrylic: Acrylic
  - mica: Mica
  $options:pt-PT:
  - acrylic: Acrílico
  - mica: Mica
  $options:pt-BR:
  - acrylic: Acrílico
  - mica: Mica
  $options:es-ES:
  - acrylic: Acrílico
  - mica: Mica
  $options:fr-FR:
  - acrylic: Acrylique
  - mica: Mica
  $options:de-DE:
  - acrylic: Acryl
  - mica: Mica
  $options:it-IT:
  - acrylic: Acrilico
  - mica: Mica
  $options:ru-RU:
  - acrylic: Акрил
  - mica: Mica
  $options:ja-JP:
  - acrylic: アクリル
  - mica: Mica
  $options:ko-KR:
  - acrylic: 아크릴
  - mica: Mica
  $options:zh-CN:
  - acrylic: 亚克力
  - mica: Mica
*/
// ==/WindhawkModSettings==

#include <audioclient.h>
#include <commdlg.h>
#include <d2d1.h>
#include <dwmapi.h>
#include <dwrite.h>
#include <mmdeviceapi.h>
#include <sddl.h>
#include <shellapi.h>
#include <windhawk_utils.h>
#include <windows.h>
#include <windowsx.h>
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <cwchar>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

// ---------------------------------------------------------------- shared state
struct Band {
    int type;
    float fc, gain, q;
    int on = 1;
};
enum { T_PK, T_LS, T_HS, T_LP, T_HP };
const int MAXP = 8, MAXF = 30, NG = 10, kMagic = 0x51455702;
static const float kGfx[NG] = {31.25f, 62.5f, 125,  250,  500,
                               1000,   2000,  4000, 8000, 16000};
static const double kPi = 3.14159265358979323846;

struct Shared {
    volatile LONG version;  // odd while the tray is writing
    int magic, eqEnabled, curveEnabled, mode,
        invert;  // mode 0 graphic, 1 parametric
    float preamp, filePreamp;
    float graphic[NG];
    int nParam;
    Band param[MAXP];
    int nFile;
    Band file[MAXF];
};

static Shared* g_sh;
static HANDLE g_map;

static bool OpenShared(bool rw) {
    const wchar_t* name = L"Local\\WindhawkTrayEQ_v2";
    if (!rw)
        g_map = OpenFileMappingW(FILE_MAP_READ, FALSE, name);
    if (!g_map) {
        // Everyone: read only. Full access only for the current user (works for
        // elevated and normal tokens alike).
        std::wstring sddl = L"D:(A;;GR;;;WD)";
        HANDLE tok = nullptr;
        BYTE ub[256];
        DWORD len = 0;
        LPWSTR sid = nullptr;
        if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &tok)) {
            if (GetTokenInformation(tok, TokenUser, ub, sizeof ub, &len) &&
                ConvertSidToStringSidW(((TOKEN_USER*)ub)->User.Sid, &sid)) {
                sddl += std::wstring(L"(A;;GA;;;") + sid + L")";
                LocalFree(sid);
            }
            CloseHandle(tok);
        }
        sddl += L"S:(ML;;NW;;;LW)";
        PSECURITY_DESCRIPTOR psd = nullptr;
        ConvertStringSecurityDescriptorToSecurityDescriptorW(
            sddl.c_str(), SDDL_REVISION_1, &psd, nullptr);
        SECURITY_ATTRIBUTES sa = {sizeof sa, psd, FALSE};
        g_map = CreateFileMappingW(INVALID_HANDLE_VALUE, &sa, PAGE_READWRITE, 0,
                                   sizeof(Shared), name);
        if (psd)
            LocalFree(psd);
    }
    if (!g_map)
        return false;
    g_sh = (Shared*)MapViewOfFile(
        g_map, rw ? FILE_MAP_ALL_ACCESS : FILE_MAP_READ, 0, 0, sizeof(Shared));
    return g_sh != nullptr;
}

// Flat list of filters + total preamp (dB) for a config snapshot.
static int BuildChain(const Shared& s, Band* out, float* preDb) {
    int n = 0;
    float pre = 0;
    if (!s.eqEnabled) {
        *preDb = 0;
        return 0;
    }
    if (s.curveEnabled) {
        pre += s.filePreamp;
        for (int i = 0; i < s.nFile && i < MAXF; i++)
            out[n++] = s.file[i];
    }
    pre += s.preamp;
    if (s.mode == 0) {
        for (int i = 0; i < NG; i++)
            if (s.graphic[i] != 0)
                out[n++] = {T_PK, kGfx[i], s.graphic[i], 1.41f};
    } else
        for (int i = 0; i < s.nParam && i < MAXP; i++) {
            const Band& b = s.param[i];
            if (b.on && (b.type >= T_LP || b.gain != 0))
                out[n++] = b;
        }
    *preDb = pre;
    return n;
}

// ---------------------------------------------------------------- biquad (RBJ)
struct Bq {
    double b0, b1, b2, a1, a2;
};

static bool Coef(const Band& b, double fs, Bq& o) {
    if (!(b.fc > 0) || b.fc >= fs * 0.49)
        return false;
    double w = 2 * kPi * b.fc / fs, c = cos(w), s = sin(w),
           q = b.q > 0.05 ? b.q : 0.05;
    double al = s / (2 * q), A = pow(10.0, b.gain / 40), sA = 2 * sqrt(A) * al;
    double b0, b1, b2, a0, a1, a2;
    switch (b.type) {
        case T_LS:
            b0 = A * ((A + 1) - (A - 1) * c + sA);
            b1 = 2 * A * ((A - 1) - (A + 1) * c);
            b2 = A * ((A + 1) - (A - 1) * c - sA);
            a0 = (A + 1) + (A - 1) * c + sA;
            a1 = -2 * ((A - 1) + (A + 1) * c);
            a2 = (A + 1) + (A - 1) * c - sA;
            break;
        case T_HS:
            b0 = A * ((A + 1) + (A - 1) * c + sA);
            b1 = -2 * A * ((A - 1) + (A + 1) * c);
            b2 = A * ((A + 1) + (A - 1) * c - sA);
            a0 = (A + 1) - (A - 1) * c + sA;
            a1 = 2 * ((A - 1) - (A + 1) * c);
            a2 = (A + 1) - (A - 1) * c - sA;
            break;
        case T_LP:
            b0 = (1 - c) / 2;
            b1 = 1 - c;
            b2 = b0;
            a0 = 1 + al;
            a1 = -2 * c;
            a2 = 1 - al;
            break;
        case T_HP:
            b0 = (1 + c) / 2;
            b1 = -(1 + c);
            b2 = b0;
            a0 = 1 + al;
            a1 = -2 * c;
            a2 = 1 - al;
            break;
        default:
            b0 = 1 + al * A;
            b1 = -2 * c;
            b2 = 1 - al * A;
            a0 = 1 + al / A;
            a1 = -2 * c;
            a2 = 1 - al / A;
            break;
    }
    o = {b0 / a0, b1 / a0, b2 / a0, a1 / a0, a2 / a0};
    return true;
}

static double MagDb(const Bq& q, double f, double fs) {
    double w = 2 * kPi * f / fs, c1 = cos(w), c2 = cos(2 * w);
    double n = q.b0 * q.b0 + q.b1 * q.b1 + q.b2 * q.b2 +
               2 * (q.b0 * q.b1 + q.b1 * q.b2) * c1 + 2 * q.b0 * q.b2 * c2;
    double d = 1 + q.a1 * q.a1 + q.a2 * q.a2 + 2 * (q.a1 + q.a1 * q.a2) * c1 +
               2 * q.a2 * c2;
    return 10 * log10((n > 1e-12 ? n : 1e-12) / (d > 1e-12 ? d : 1e-12));
}

// ---------------------------------------------------------------- WASAPI hooks
// (DSP)
struct Fmt {
    int ch, rate, kind, align;
};  // kind: 0 bypass, 1 f32, 2 i16, 3 i24, 4 i32
struct Stream {
    Fmt f;
    BYTE* buf;
    LONG ver;
    int n;
    float pre;
    Bq bq[64];
    double z[64][8][2];
    Stream(const Fmt& ff) {
        memset((void*)this, 0, sizeof *this);
        f = ff;
        ver = -1;
        pre = 1;
    }
};

static SRWLOCK g_lk = SRWLOCK_INIT;
static std::unordered_map<void*, Fmt> g_clients;
static std::unordered_map<void*, std::unique_ptr<Stream>> g_streams;

static Fmt ParseFmt(const WAVEFORMATEX* w) {
    Fmt f = {w->nChannels, (int)w->nSamplesPerSec, 0, w->nBlockAlign};
    WORD tag = w->wFormatTag;
    int bits = w->wBitsPerSample;
    if (tag == WAVE_FORMAT_EXTENSIBLE && w->cbSize >= 22) {
        DWORD d1 = ((const WAVEFORMATEXTENSIBLE*)w)->SubFormat.Data1;
        tag = d1 == 3 ? 3 : d1 == 1 ? 1 : 0;
    }
    if (f.ch < 1 || f.ch > 8 || f.align != f.ch * bits / 8)
        return f;
    if (tag == 3 && bits == 32)
        f.kind = 1;
    else if (tag == 1)
        f.kind = bits == 16 ? 2 : bits == 24 ? 3 : bits == 32 ? 4 : 0;
    return f;
}

static void Refresh(Stream& st) {
    if (!g_sh)
        return;
    LONG v1 = g_sh->version;
    if (v1 == st.ver || (v1 & 1))
        return;
    Shared snap;
    memcpy((void*)&snap, (const void*)g_sh, sizeof snap);
    MemoryBarrier();
    if (g_sh->version != v1 || snap.magic != kMagic)
        return;
    Band ch[64];
    float pre;
    int n = BuildChain(snap, ch, &pre), m = 0;
    for (int i = 0; i < n; i++)
        if (Coef(ch[i], st.f.rate, st.bq[m]))
            m++;
    if (m != st.n)
        memset(st.z, 0, sizeof st.z);
    st.n = m;
    st.pre = powf(10.f, pre / 20);
    st.ver = v1;
}

static inline double Clip1(double x) {
    return x > 1 ? 1 : x < -1 ? -1 : x;
}

static void Process(Stream& st, UINT32 frames) {
    Refresh(st);
    if (st.n == 0 && st.pre == 1.0f)
        return;
    const int ch = st.f.ch, bps = st.f.align / ch;
    for (UINT32 i = 0; i < frames; i++)
        for (int c = 0; c < ch; c++) {
            BYTE* p = st.buf + (size_t)i * st.f.align + (size_t)c * bps;
            double x;
            switch (st.f.kind) {
                case 1:
                    x = *(float*)p;
                    break;
                case 2:
                    x = *(int16_t*)p / 32768.0;
                    break;
                case 3:
                    x = ((int)((unsigned)p[0] << 8 | (unsigned)p[1] << 16 |
                               (unsigned)p[2] << 24) >>
                         8) /
                        8388608.0;
                    break;
                default:
                    x = *(int32_t*)p / 2147483648.0;
                    break;
            }
            x *= st.pre;
            for (int k = 0; k < st.n; k++) {
                const Bq& q = st.bq[k];
                double* z = st.z[k][c];
                double y = q.b0 * x + z[0];
                z[0] = q.b1 * x - q.a1 * y + z[1];
                z[1] = q.b2 * x - q.a2 * y;
                x = y;
            }
            switch (st.f.kind) {
                case 1:
                    *(float*)p = (float)x;
                    break;
                case 2:
                    *(int16_t*)p = (int16_t)lrint(Clip1(x) * 32767);
                    break;
                case 3: {
                    int v = (int)lrint(Clip1(x) * 8388607);
                    p[0] = v & 255;
                    p[1] = (v >> 8) & 255;
                    p[2] = (v >> 16) & 255;
                    break;
                }
                default: {
                    double v = Clip1(x) * 2147483647.0;
                    *(int32_t*)p = (int32_t)lrint(v);
                    break;
                }
            }
        }
}

typedef HRESULT(STDMETHODCALLTYPE* Init_t)(IAudioClient*,
                                           AUDCLNT_SHAREMODE,
                                           DWORD,
                                           REFERENCE_TIME,
                                           REFERENCE_TIME,
                                           const WAVEFORMATEX*,
                                           LPCGUID);
typedef HRESULT(STDMETHODCALLTYPE* GetService_t)(IAudioClient*, REFIID, void**);
typedef HRESULT(STDMETHODCALLTYPE* GetBuffer_t)(IAudioRenderClient*,
                                                UINT32,
                                                BYTE**);
typedef HRESULT(STDMETHODCALLTYPE* Release_t)(IAudioRenderClient*,
                                              UINT32,
                                              DWORD);
static Init_t Init_o;
static GetService_t GS_o;
static GetBuffer_t GB_o;
static Release_t RB_o;

static HRESULT STDMETHODCALLTYPE Init_h(IAudioClient* t,
                                        AUDCLNT_SHAREMODE m,
                                        DWORD fl,
                                        REFERENCE_TIME a,
                                        REFERENCE_TIME b,
                                        const WAVEFORMATEX* f,
                                        LPCGUID g) {
    HRESULT hr = Init_o(t, m, fl, a, b, f, g);
    if (SUCCEEDED(hr)) {
        Fmt ff = {};
        if (m == AUDCLNT_SHAREMODE_SHARED && f)
            ff = ParseFmt(f);
        AcquireSRWLockExclusive(&g_lk);
        g_clients[t] = ff;
        ReleaseSRWLockExclusive(&g_lk);
    }
    return hr;
}

static HRESULT STDMETHODCALLTYPE GS_h(IAudioClient* t, REFIID riid, void** pp) {
    HRESULT hr = GS_o(t, riid, pp);
    if (SUCCEEDED(hr) && pp && *pp &&
        IsEqualIID(riid, __uuidof(IAudioRenderClient))) {
        AcquireSRWLockExclusive(&g_lk);
        g_streams.erase(*pp);
        auto it = g_clients.find(t);
        if (it != g_clients.end() && it->second.kind)
            g_streams[*pp] = std::make_unique<Stream>(it->second);
        ReleaseSRWLockExclusive(&g_lk);
    }
    return hr;
}

static HRESULT STDMETHODCALLTYPE GB_h(IAudioRenderClient* t,
                                      UINT32 n,
                                      BYTE** pd) {
    HRESULT hr = GB_o(t, n, pd);
    if (SUCCEEDED(hr) && pd) {
        AcquireSRWLockShared(&g_lk);
        auto it = g_streams.find(t);
        if (it != g_streams.end())
            it->second->buf = *pd;
        ReleaseSRWLockShared(&g_lk);
    }
    return hr;
}

static HRESULT STDMETHODCALLTYPE RB_h(IAudioRenderClient* t,
                                      UINT32 n,
                                      DWORD flags) {
    if (n && !(flags & AUDCLNT_BUFFERFLAGS_SILENT)) {
        AcquireSRWLockShared(&g_lk);
        auto it = g_streams.find(t);
        if (it != g_streams.end() && it->second->buf) {
            Process(*it->second, n);
            it->second->buf = nullptr;
        }
        ReleaseSRWLockShared(&g_lk);
    }
    return RB_o(t, n, flags);
}

// Drop per-object state when the COM object dies. The destructors receive the
// start of the object while the app holds interface pointers inside it, so
// forget every key that falls inside the object.
static void ForgetObject(void* obj) {
    char* lo = (char*)obj;
    AcquireSRWLockExclusive(&g_lk);
    for (auto it = g_clients.begin(); it != g_clients.end();)
        it = (char*)it->first >= lo && (char*)it->first < lo + 0x40
                 ? g_clients.erase(it)
                 : std::next(it);
    for (auto it = g_streams.begin(); it != g_streams.end();)
        it = (char*)it->first >= lo && (char*)it->first < lo + 0x40
                 ? g_streams.erase(it)
                 : std::next(it);
    ReleaseSRWLockExclusive(&g_lk);
}

typedef void(STDMETHODCALLTYPE* Dtor_t)(void*);
static Dtor_t ACDtor_o, RCDtor_o;

static void STDMETHODCALLTYPE ACDtor_h(void* t) {
    ForgetObject(t);
    ACDtor_o(t);
}

static void STDMETHODCALLTYPE RCDtor_h(void* t) {
    ForgetObject(t);
    RCDtor_o(t);
}

// The WASAPI implementation classes of AudioSes.dll, resolved by symbol (x64).
static bool HookAudioSes(HMODULE audioSes) {
    WindhawkUtils::SYMBOL_HOOK audioSesDllHooks[] = {
        {{L"public: virtual long __cdecl CAudioClient::Initialize(enum "
          L"_AUDCLNT_SHAREMODE,unsigned long,__int64,__int64,struct "
          L"tWAVEFORMATEX const *,struct _GUID const *)"},
         (void**)&Init_o,
         (void*)Init_h},
        {{L"public: virtual long __cdecl CAudioClient::GetService(struct "
          L"_GUID const &,void * *)"},
         (void**)&GS_o,
         (void*)GS_h},
        {{L"public: virtual long __cdecl "
          L"CAudioRenderClient::GetBuffer(unsigned int,unsigned char * *)"},
         (void**)&GB_o,
         (void*)GB_h},
        {{L"public: virtual long __cdecl "
          L"CAudioRenderClient::ReleaseBuffer(unsigned int,unsigned long)"},
         (void**)&RB_o,
         (void*)RB_h},
        {{L"protected: virtual __cdecl CAudioClient::~CAudioClient(void)"},
         (void**)&ACDtor_o,
         (void*)ACDtor_h,
         true},
        {{L"public: virtual __cdecl "
          L"CAudioRenderClient::~CAudioRenderClient(void)"},
         (void**)&RCDtor_o,
         (void*)RCDtor_h,
         true},
    };
    return WindhawkUtils::HookSymbols(audioSes, audioSesDllHooks,
                                      ARRAYSIZE(audioSesDllHooks));
}

// Hooks must be in place before the app's own Initialize runs, so they are
// installed synchronously, right after AudioSes.dll is loaded. Hook calls are
// only allowed between Wh_ModAfterInit and Wh_ModBeforeUninit.
static SRWLOCK g_hookLock = SRWLOCK_INIT;
static bool g_hookAllowed, g_audioHooked;

static void HookAudioSesIfLoaded(bool apply) {
    AcquireSRWLockExclusive(&g_hookLock);
    if (g_hookAllowed && !g_audioHooked) {
        if (HMODULE m = GetModuleHandleW(L"AudioSes.dll")) {
            g_audioHooked = true;
            if (!HookAudioSes(m))
                Wh_Log(L"AudioSes.dll symbols not resolved");
            else if (apply)
                Wh_ApplyHookOperations();
        }
    }
    ReleaseSRWLockExclusive(&g_hookLock);
}

typedef HMODULE(WINAPI* LoadLibraryExW_t)(LPCWSTR, HANDLE, DWORD);
static LoadLibraryExW_t LoadLibraryExW_o;

static HMODULE WINAPI LoadLibraryExW_h(LPCWSTR name, HANDLE f, DWORD fl) {
    HMODULE m = LoadLibraryExW_o(name, f, fl);
    if (m && !g_audioHooked &&
        !(fl & (LOAD_LIBRARY_AS_DATAFILE | LOAD_LIBRARY_AS_IMAGE_RESOURCE)))
        HookAudioSesIfLoaded(true);
    return m;
}

// ---------------------------------------------------------------- tray-side
// config
static Shared g_cfg;
static std::wstring g_curvePath, g_curveName, g_curveMsg,
    g_trayHost = L"explorer.exe", g_lastSettingFile;
static int g_backdrop;  // 0 acrylic, 1 mica

// ---------------------------------------------------------------- localization
// (follows the Windows UI language)
enum {
    S_EQ,
    S_CURVE,
    S_LOADFILE,
    S_OPEN,
    S_ERR_OPEN,
    S_ERR_BIG,
    S_ERR_NONE,
    S_NAMEHINT,
    S_PRESETS,
    S_SAVE,
    S_CURVEPFX,
    S_LOAD,
    S_INVERT,
    S_PARAM,
    S_GFX,
    S_FREQ,
    S_GAIN,
    S_PREAMP,
    S_PRE,
    S_NOPRE,
    S_DELPRE,
    S_PRESETN,
    S_COUNT
};
enum {
    L_EN,
    L_PT,
    L_PTBR,
    L_ES,
    L_FR,
    L_DE,
    L_IT,
    L_RU,
    L_JA,
    L_KO,
    L_ZH,
    L_COUNT
};
static const wchar_t* const kStr[L_COUNT][S_COUNT] = {
    {L"Equalizer",
     L"Monitor curve",
     L"Load curve file…",
     L"Open EQ",
     L"Could not open file",
     L"Invalid or too large file",
     L"No data recognized",
     L"Enter save · Esc cancel",
     L"Presets",
     L"Save",
     L"Curve: ",
     L"Load…",
     L"Invert curve",
     L"Parametric",
     L"Graphic",
     L"Freq",
     L"Gain",
     L"Preamp",
     L"Pre",
     L"(no presets)",
     L"Delete preset",
     L"Preset %d"},
    {L"Equalizador",
     L"Curva dos monitores",
     L"Carregar ficheiro de curva…",
     L"Abrir EQ",
     L"Não foi possível abrir o ficheiro",
     L"Ficheiro inválido ou demasiado grande",
     L"Nenhum dado reconhecido",
     L"Enter guardar · Esc cancelar",
     L"Presets",
     L"Guardar",
     L"Curva: ",
     L"Carregar…",
     L"Inverter curva",
     L"Paramétrica",
     L"Gráfica",
     L"Freq",
     L"Ganho",
     L"Pré-amp",
     L"Pré",
     L"(sem presets)",
     L"Apagar preset",
     L"Preset %d"},
    {L"Equalizador",
     L"Curva dos monitores",
     L"Carregar arquivo de curva…",
     L"Abrir EQ",
     L"Não foi possível abrir o arquivo",
     L"Arquivo inválido ou grande demais",
     L"Nenhum dado reconhecido",
     L"Enter salvar · Esc cancelar",
     L"Presets",
     L"Salvar",
     L"Curva: ",
     L"Carregar…",
     L"Inverter curva",
     L"Paramétrica",
     L"Gráfica",
     L"Freq",
     L"Ganho",
     L"Pré-amp",
     L"Pré",
     L"(sem presets)",
     L"Excluir preset",
     L"Preset %d"},
    {L"Ecualizador",
     L"Curva del monitor",
     L"Cargar archivo de curva…",
     L"Abrir EQ",
     L"No se pudo abrir el archivo",
     L"Archivo no válido o demasiado grande",
     L"No se reconocieron datos",
     L"Intro guardar · Esc cancelar",
     L"Preajustes",
     L"Guardar",
     L"Curva: ",
     L"Cargar…",
     L"Invertir curva",
     L"Paramétrico",
     L"Gráfico",
     L"Frec",
     L"Gan",
     L"Preamp",
     L"Pre",
     L"(sin preajustes)",
     L"Eliminar preajuste",
     L"Preajuste %d"},
    {L"Égaliseur",
     L"Courbe du moniteur",
     L"Charger un fichier de courbe…",
     L"Ouvrir l'EQ",
     L"Impossible d'ouvrir le fichier",
     L"Fichier invalide ou trop volumineux",
     L"Aucune donnée reconnue",
     L"Entrée enregistrer · Échap annuler",
     L"Préréglages",
     L"Enreg.",
     L"Courbe : ",
     L"Charger…",
     L"Inverser la courbe",
     L"Paramétrique",
     L"Graphique",
     L"Fréq",
     L"Gain",
     L"Préampli",
     L"Pré",
     L"(aucun préréglage)",
     L"Supprimer le préréglage",
     L"Préréglage %d"},
    {L"Equalizer",
     L"Monitorkurve",
     L"Kurvendatei laden…",
     L"EQ öffnen",
     L"Datei konnte nicht geöffnet werden",
     L"Ungültige oder zu große Datei",
     L"Keine Daten erkannt",
     L"Enter speichern · Esc abbrechen",
     L"Presets",
     L"Speichern",
     L"Kurve: ",
     L"Laden…",
     L"Kurve invertieren",
     L"Parametrisch",
     L"Grafisch",
     L"Freq",
     L"Gain",
     L"Preamp",
     L"Pre",
     L"(keine Presets)",
     L"Preset löschen",
     L"Preset %d"},
    {L"Equalizzatore",
     L"Curva del monitor",
     L"Carica file curva…",
     L"Apri EQ",
     L"Impossibile aprire il file",
     L"File non valido o troppo grande",
     L"Nessun dato riconosciuto",
     L"Invio salva · Esc annulla",
     L"Preset",
     L"Salva",
     L"Curva: ",
     L"Carica…",
     L"Inverti curva",
     L"Parametrico",
     L"Grafico",
     L"Freq",
     L"Gain",
     L"Preamp",
     L"Pre",
     L"(nessun preset)",
     L"Elimina preset",
     L"Preset %d"},
    {L"Эквалайзер",
     L"Кривая монитора",
     L"Загрузить файл кривой…",
     L"Открыть эквалайзер",
     L"Не удалось открыть файл",
     L"Недопустимый или слишком большой файл",
     L"Данные не распознаны",
     L"Enter — сохранить · Esc — отмена",
     L"Пресеты",
     L"Сохранить",
     L"Кривая: ",
     L"Загрузить…",
     L"Инвертировать кривую",
     L"Параметрический",
     L"Графический",
     L"Част.",
     L"Усил.",
     L"Предусил.",
     L"Пред",
     L"(нет пресетов)",
     L"Удалить пресет",
     L"Пресет %d"},
    {L"イコライザー",
     L"モニター補正カーブ",
     L"カーブファイルを読み込む…",
     L"EQ を開く",
     L"ファイルを開けませんでした",
     L"無効、またはサイズが大きすぎるファイルです",
     L"認識できるデータがありません",
     L"Enter 保存 · Esc キャンセル",
     L"プリセット",
     L"保存",
     L"カーブ: ",
     L"読み込む…",
     L"カーブを反転",
     L"パラメトリック",
     L"グラフィック",
     L"周波数",
     L"ゲイン",
     L"プリアンプ",
     L"プリ",
     L"(プリセットなし)",
     L"プリセットを削除",
     L"プリセット %d"},
    {L"이퀄라이저",
     L"모니터 보정 곡선",
     L"곡선 파일 불러오기…",
     L"EQ 열기",
     L"파일을 열 수 없습니다",
     L"잘못되었거나 너무 큰 파일",
     L"인식된 데이터 없음",
     L"Enter 저장 · Esc 취소",
     L"프리셋",
     L"저장",
     L"곡선: ",
     L"불러오기…",
     L"곡선 반전",
     L"파라메트릭",
     L"그래픽",
     L"주파수",
     L"게인",
     L"프리앰프",
     L"프리",
     L"(프리셋 없음)",
     L"프리셋 삭제",
     L"프리셋 %d"},
    {L"均衡器",        L"显示器校正曲线",
     L"加载曲线文件…", L"打开 EQ",
     L"无法打开文件",  L"文件无效或过大",
     L"未识别到数据",  L"Enter 保存 · Esc 取消",
     L"预设",          L"保存",
     L"曲线：",        L"加载…",
     L"反转曲线",      L"参量",
     L"图形",          L"频率",
     L"增益",          L"前级增益",
     L"前级",          L"(无预设)",
     L"删除预设",      L"预设 %d"},
};

static const wchar_t* T(int id) {
    static int lang = -1;
    if (lang < 0) {
        LANGID li = GetUserDefaultUILanguage();
        switch (PRIMARYLANGID(li)) {
            case LANG_PORTUGUESE:
                lang = SUBLANGID(li) == SUBLANG_PORTUGUESE_BRAZILIAN ? L_PTBR
                                                                     : L_PT;
                break;
            case LANG_SPANISH:
                lang = L_ES;
                break;
            case LANG_FRENCH:
                lang = L_FR;
                break;
            case LANG_GERMAN:
                lang = L_DE;
                break;
            case LANG_ITALIAN:
                lang = L_IT;
                break;
            case LANG_RUSSIAN:
                lang = L_RU;
                break;
            case LANG_JAPANESE:
                lang = L_JA;
                break;
            case LANG_KOREAN:
                lang = L_KO;
                break;
            case LANG_CHINESE:
                lang = L_ZH;
                break;  // shortcut: Traditional Chinese shows Simplified, add a
                        // column if needed
            default:
                lang = L_EN;
        }
    }
    return kStr[lang][id];
}

static float Cf(float x, float lo, float hi) {
    if (x != x)
        x = 0;
    return x < lo ? lo : x > hi ? hi : x;
}

static void DefaultBands() {
    static const int ty[MAXP] = {T_HP, T_LS, T_PK, T_PK,
                                 T_PK, T_PK, T_HS, T_LP};
    static const float f[MAXP] = {30, 100, 250, 1000, 2500, 5000, 10000, 18000};
    static const int on[MAXP] = {0, 1, 1, 1, 1, 0, 1, 0};
    g_cfg.nParam = MAXP;
    for (int i = 0; i < MAXP; i++)
        g_cfg.param[i] = {ty[i], f[i], 0, ty[i] == T_PK ? 1.0f : 0.71f, on[i]};
}

static void Defaults() {
    memset((void*)&g_cfg, 0, sizeof g_cfg);
    g_cfg.magic = kMagic;
    g_cfg.eqEnabled = 1;
    g_cfg.curveEnabled = 1;
    g_cfg.invert = 1;
    DefaultBands();
}

static void Sanitize(Shared& s) {
    s.mode = s.mode == 1;
    s.invert = s.invert != 0;
    s.eqEnabled = s.eqEnabled != 0;
    s.curveEnabled = s.curveEnabled != 0;
    s.preamp = Cf(s.preamp, -12, 12);
    s.filePreamp = Cf(s.filePreamp, -30, 30);
    for (float& g : s.graphic)
        g = Cf(g, -12, 12);
    s.nParam = MAXP;
    s.nFile = s.nFile < 0 ? 0 : s.nFile > MAXF ? MAXF : s.nFile;
    for (int i = 0; i < MAXP + MAXF; i++) {
        Band& b = i < MAXP ? s.param[i] : s.file[i - MAXP];
        b.type = b.type < 0 || b.type > T_HP ? T_PK : b.type;
        b.on = b.on != 0;
        b.fc = Cf(b.fc, 10, 24000);
        b.gain = Cf(b.gain, -30, 30);
        b.q = Cf(b.q, 0.05f, 20);
    }
}

static void Publish() {
    if (!g_sh)
        return;
    const size_t off = offsetof(Shared, magic);
    InterlockedIncrement(&g_sh->version);
    memcpy((char*)g_sh + off, (char*)&g_cfg + off, sizeof(Shared) - off);
    MemoryBarrier();
    InterlockedIncrement(&g_sh->version);
}

// Equalizer APO / AutoEQ / REW text.
static void ParseLine(std::string l) {
    for (char& c : l) {
        c = (char)tolower((unsigned char)c);
        if (c == ':' || c == ',' || c == '\t' || c == '\r' || c == '=')
            c = ' ';
    }
    std::vector<std::string> t;
    size_t i = 0;
    while (i < l.size()) {
        while (i < l.size() && l[i] == ' ')
            i++;
        size_t j = i;
        while (j < l.size() && l[j] != ' ')
            j++;
        if (j > i)
            t.push_back(l.substr(i, j - i));
        i = j;
    }
    if (t.size() < 2)
        return;
    if (t[0] == "preamp") {
        g_cfg.filePreamp = Cf((float)atof(t[1].c_str()), -30, 30);
        return;
    }
    if (t[0] != "filter" || g_cfg.nFile >= MAXF || t.size() < 5 || t[2] != "on")
        return;
    const std::string& ty = t[3];
    int type;
    if (ty == "pk" || ty == "peak" || ty == "peaking")
        type = T_PK;
    else if (ty == "lsc" || ty == "ls" || ty == "lsq" || ty == "lowshelf")
        type = T_LS;
    else if (ty == "hsc" || ty == "hs" || ty == "hsq" || ty == "highshelf")
        type = T_HS;
    else if (ty == "lp" || ty == "lpq")
        type = T_LP;
    else if (ty == "hp" || ty == "hpq")
        type = T_HP;
    else
        return;
    float fc = 0, gain = 0, q = 0, bw = 0;
    for (i = 4; i + 1 < t.size(); i++) {
        float v = (float)atof(t[i + 1].c_str());
        if (t[i] == "fc")
            fc = v;
        else if (t[i] == "gain")
            gain = v;
        else if (t[i] == "q")
            q = v;
        else if (t[i] == "bw")
            bw = v;
    }
    if (q <= 0)
        q = bw > 0 ? (float)(1 / (2 * sinh(0.34657359 * bw)))
                   : (type == T_PK ? 1.f : 0.707f);
    if (!(fc >= 10 && fc <= 24000))
        return;
    g_cfg.file[g_cfg.nFile++] = {type, fc, Cf(gain, -30, 30), Cf(q, 0.05f, 20)};
}

// Raw measurement points (freq, dB), e.g. REW export or CSV. Fitted to a
// cascade of peaking filters.
static std::vector<std::pair<double, double>> g_pts;
static bool g_curveRaw;

static double Cd(double x, double lo, double hi) {
    if (x != x)
        x = 0;
    return x < lo ? lo : x > hi ? hi : x;
}

static bool ParseRaw(const std::string& l) {
    const char* p = l.c_str();
    double v[2];
    int n = 0;
    while (n < 2) {
        while (*p == ' ' || *p == ',' || *p == ';' || *p == '\t' || *p == '\r')
            p++;
        if (!*p)
            break;
        char* e;
        v[n] = strtod(p, &e);
        if (e == p)
            return false;
        p = e;
        n++;
    }
    if (n < 2 || !(v[0] > 0) || v[0] > 100000 || v[1] != v[1])
        return false;
    g_pts.push_back({v[0], v[1]});
    return true;
}

// Resample the raw curve (1/3 octave smoothing), normalise at 0.5-2 kHz,
// optionally invert, then iterate 30 peaking filters (1/3 oct spacing, Q 3)
// until they match. Preamp = -peak boost.
static void FitCurve() {
    g_cfg.nFile = 0;
    g_cfg.filePreamp = 0;
    const size_t n = g_pts.size();
    if (n < 4)
        return;
    std::vector<double> fr(n), ps(n + 1, 0.0);
    for (size_t i = 0; i < n; i++) {
        fr[i] = g_pts[i].first;
        ps[i + 1] = ps[i] + g_pts[i].second;
    }
    auto tgt = [&](double fq) {
        double r = pow(2.0, 1.0 / 6);
        size_t a = std::lower_bound(fr.begin(), fr.end(), fq / r) - fr.begin();
        size_t b = std::upper_bound(fr.begin(), fr.end(), fq * r) - fr.begin();
        if (b >= a + 2)
            return (ps[b] - ps[a]) / (double)(b - a);
        size_t k = std::lower_bound(fr.begin(), fr.end(), fq) - fr.begin();
        if (k == 0)
            return g_pts[0].second;
        if (k >= n)
            return g_pts[n - 1].second;
        double t = log(fq / fr[k - 1]) / log(fr[k] / fr[k - 1]);
        return g_pts[k - 1].second +
               t * (g_pts[k].second - g_pts[k - 1].second);
    };
    double ref = 0;
    for (int k = 0; k < 16; k++)
        ref += tgt(500 * pow(4.0, k / 15.0));
    ref /= 16;
    const int N = MAXF;
    const double Q = 3, fs = 48000, sign = g_cfg.invert ? -1 : 1;
    double fc[N], tg[N], g[N];
    Bq q[N];
    for (int i = 0; i < N; i++) {
        fc[i] = 20 * pow(2.0, i / 3.0);
        tg[i] = Cd(sign * (tgt(fc[i]) - ref), -24, 24);
        g[i] = tg[i];
    }
    for (int it = 0; it < 41; it++) {
        for (int i = 0; i < N; i++)
            Coef({T_PK, (float)fc[i], (float)g[i], (float)Q}, fs, q[i]);
        if (it == 40)
            break;
        for (int i = 0; i < N; i++) {
            double r = 0;
            for (int j = 0; j < N; j++)
                r += MagDb(q[j], fc[i], fs);
            g[i] = Cd(g[i] + 0.8 * (tg[i] - r), -24, 24);
        }
    }
    double peak = -99;
    for (int k = 0; k < 300; k++) {
        double f = 20 * pow(1000.0, k / 299.0), r = 0;
        for (int j = 0; j < N; j++)
            r += MagDb(q[j], f, fs);
        if (r > peak)
            peak = r;
    }
    for (int i = 0; i < N; i++)
        g_cfg.file[g_cfg.nFile++] = {T_PK, (float)fc[i], (float)g[i], (float)Q};
    g_cfg.filePreamp = peak > 0 ? (float)Cd(-ceil(peak * 10) / 10, -30, 0) : 0;
}

// Accepts Equalizer APO / AutoEQ text, or raw "freq, dB[, phase]" data (CSV /
// REW .txt).
static void LoadCurve(const wchar_t* path) {
    g_cfg.nFile = 0;
    g_cfg.filePreamp = 0;
    g_curveMsg.clear();
    g_curvePath = path;
    g_pts.clear();
    g_curveRaw = false;
    const wchar_t* fn = wcsrchr(path, L'\\');
    g_curveName = fn ? fn + 1 : path;
    HANDLE h = CreateFileW(path, GENERIC_READ, FILE_SHARE_READ, nullptr,
                           OPEN_EXISTING, 0, nullptr);
    if (h == INVALID_HANDLE_VALUE) {
        g_curveMsg = T(S_ERR_OPEN);
        return;
    }
    DWORD sz = GetFileSize(h, nullptr), rd = 0;
    if (sz == INVALID_FILE_SIZE || sz > 16u * 1024 * 1024) {
        CloseHandle(h);
        g_curveMsg = T(S_ERR_BIG);
        return;
    }
    std::string d(sz, '\0');
    ReadFile(h, &d[0], sz, &rd, nullptr);
    CloseHandle(h);
    d.resize(rd);
    for (size_t pos = 0; pos < d.size();) {
        size_t e = d.find('\n', pos);
        if (e == std::string::npos)
            e = d.size();
        std::string line = d.substr(pos, e - pos);
        pos = e + 1;
        if (line.compare(0, 3, "\xEF\xBB\xBF") == 0)
            line.erase(0, 3);
        size_t k = line.find_first_not_of(" \t");
        if (k == std::string::npos)
            continue;
        char c = line[k];
        if (isdigit((unsigned char)c) || c == '-' || c == '+' || c == '.')
            ParseRaw(line.substr(k));
        else
            ParseLine(line);
    }
    if (!g_cfg.nFile && g_pts.size() >= 4) {
        std::sort(g_pts.begin(), g_pts.end());
        g_curveRaw = true;
        g_cfg.invert = 1;  // a raw measurement must be inverted to correct it
        FitCurve();
        g_pts.clear();
        g_pts.shrink_to_fit();
    } else if (g_cfg.nFile)
        g_cfg.invert = 0;  // APO/AutoEQ files are already corrections
    else if (g_cfg.filePreamp == 0)
        g_curveMsg = T(S_ERR_NONE);
}

// ---------------------------------------------------------------- tray +
// flyout UI
static const UINT WM_TRAY = WM_APP + 1, WM_SETCH = WM_APP + 2,
                  WM_PICK = WM_APP + 3;
static const float W = 400, H = 530, PX = 20, PW = 360;

static volatile HWND g_hTray;
static HWND g_hFly;
static HANDLE g_trayThread, g_trayMutex;
static DWORD g_trayTid;
static ID2D1Factory* g_d2d;
static IDWriteFactory* g_dw;
static ID2D1HwndRenderTarget* g_rt;
static ID2D1SolidColorBrush* g_br;
static IDWriteTextFormat *g_tfL, *g_tfC, *g_tfS, *g_tfT, *g_tfI;
static bool g_dark = true;
static float g_acc[3] = {0, 0.47f, 0.83f};
static float g_s = 1;
static HICON g_icon;
static UINT g_msgTaskbar;
static DWORD g_lastDeact;
static int g_preCur = -1;
static float g_dim;  // 0 enabled .. 1 locked/greyed
static DWORD g_dimT;
static int g_sel, g_drag, g_dragCol;  // g_drag: 0 none, 1 slider, 2 node

static DWORD RegDw(const wchar_t* name, DWORD def) {
    DWORD v = def, sz = sizeof v;
    RegGetValueW(
        HKEY_CURRENT_USER,
        L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
        name, RRF_RT_REG_DWORD, nullptr, &v, &sz);
    return v;
}
static bool ThemeLight(const wchar_t* name) {
    return RegDw(name, 1) != 0;
}

static float g_tint[4] = {.08f, .08f, .09f, .55f};

// Follows the taskbar/flyout theme (SystemUsesLightTheme), accent colour and
// "accent on taskbar" setting.
static void LoadTheme() {
    g_dark = !ThemeLight(L"SystemUsesLightTheme");
    DWORD v = 0, sz = sizeof v;
    float r = 0, g = 0.47f, b = 0.83f;
    if (RegGetValueW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\DWM",
                     L"AccentColor", RRF_RT_REG_DWORD, nullptr, &v,
                     &sz) == ERROR_SUCCESS) {
        r = (v & 255) / 255.f;
        g = ((v >> 8) & 255) / 255.f;
        b = ((v >> 16) & 255) / 255.f;
    }
    if (RegDw(L"ColorPrevalence", 0)) {
        if (g_dark) {
            g_tint[0] = r * .28f;
            g_tint[1] = g * .28f;
            g_tint[2] = b * .28f;
        } else {
            g_tint[0] = .82f + r * .18f;
            g_tint[1] = .82f + g * .18f;
            g_tint[2] = .82f + b * .18f;
        }
        g_tint[3] = .75f;
    } else if (g_dark) {
        g_tint[0] = .08f;
        g_tint[1] = .08f;
        g_tint[2] = .09f;
        g_tint[3] = .55f;
    } else {
        g_tint[0] = .97f;
        g_tint[1] = .97f;
        g_tint[2] = .98f;
        g_tint[3] = .55f;
    }
    if (g_dark && 0.3f * r + 0.6f * g + 0.1f * b < 0.5f) {
        r += (1 - r) * .35f;
        g += (1 - g) * .35f;
        b += (1 - b) * .35f;
    }
    g_acc[0] = r;
    g_acc[1] = g;
    g_acc[2] = b;
}

static HICON MakeIcon() {
    int n = GetSystemMetrics(SM_CXSMICON);
    BITMAPINFO bi = {};
    bi.bmiHeader = {sizeof(BITMAPINFOHEADER), n, -n, 1, 32, BI_RGB};
    void* bits = nullptr;
    HBITMAP col =
        CreateDIBSection(nullptr, &bi, DIB_RGB_COLORS, &bits, nullptr, 0);
    HBITMAP mask = CreateBitmap(n, n, 1, 1, nullptr);
    if (!col || !mask) {
        if (col)
            DeleteObject(col);
        if (mask)
            DeleteObject(mask);
        return nullptr;
    }
    DWORD* px = (DWORD*)bits;
    memset(px, 0, (size_t)n * n * 4);
    BYTE a = g_cfg.eqEnabled ? 255 : 110,
         v = ThemeLight(L"SystemUsesLightTheme") ? 0 : 255;
    DWORD c = ((DWORD)a << 24) | ((DWORD)(v * a / 255) << 16) |
              ((DWORD)(v * a / 255) << 8) | (DWORD)(v * a / 255);
    static const float hh[5] = {.45f, .75f, .55f, 1.f, .35f};
    int bw = n / 8 > 2 ? n / 8 : 2, x0 = n / 10,
        gap = (n - 2 * x0 - 5 * bw) / 4;
    if (gap < 1)
        gap = 1;
    for (int k = 0; k < 5; k++) {
        int top = n - 1 - (int)(hh[k] * (n - 2)), x = x0 + k * (bw + gap);
        for (int y = top; y < n; y++)
            for (int xx = x; xx < x + bw && xx < n; xx++)
                px[y * n + xx] = c;
    }
    ICONINFO ii = {TRUE, 0, 0, mask, col};
    HICON ic = CreateIconIndirect(&ii);
    DeleteObject(col);
    DeleteObject(mask);
    return ic;
}

static void TrayIcon(DWORD msg) {
    NOTIFYICONDATAW nd = {sizeof nd};
    nd.hWnd = g_hTray;
    nd.uID = 1;
    nd.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
    nd.uCallbackMessage = WM_TRAY;
    HICON ic = MakeIcon();
    nd.hIcon = ic;
    wcscpy(nd.szTip, L"Tray EQ");
    Shell_NotifyIconW(msg, &nd);
    if (g_icon)
        DestroyIcon(g_icon);
    g_icon = ic;
}

static void Commit() {
    Publish();
    TrayIcon(NIM_MODIFY);
    if (g_hFly)
        InvalidateRect(g_hFly, nullptr, FALSE);
    SetTimer(g_hTray, 1, 800, nullptr);
    if (g_hFly && g_dim != (g_cfg.eqEnabled ? 0.f : 1.f)) {
        g_dimT = GetTickCount();
        SetTimer(g_hFly, 3, 15, nullptr);
    }
}

static void ApplyDwm() {
    BOOL d = g_dark;
    int bd = g_backdrop ? 2 : 3, cr = 2;
    DwmSetWindowAttribute(g_hFly, 20, &d, sizeof d);
    DwmSetWindowAttribute(g_hFly, 38, &bd, sizeof bd);
    DwmSetWindowAttribute(g_hFly, 33, &cr, sizeof cr);
    MARGINS m = {-1, -1, -1, -1};
    DwmExtendFrameIntoClientArea(g_hFly, &m);
}

static void ReleaseRT() {
    if (g_br) {
        g_br->Release();
        g_br = nullptr;
    }
    if (g_rt) {
        g_rt->Release();
        g_rt = nullptr;
    }
}

static void SetCurve(const wchar_t* p) {
    LoadCurve(p);
    if (g_cfg.nFile)
        g_cfg.curveEnabled = 1;
    g_preCur = -1;
    Wh_SetStringValue(L"curveName", g_curveName.c_str());
    Commit();
}

static bool g_modal, g_out, g_fromBottom, g_naming;
static float g_p;  // 0 hidden .. 1 shown
static DWORD g_animT;
static int g_rx, g_ry, g_rw, g_rh, g_mg, g_edge;

static void PickFile() {
    wchar_t buf[MAX_PATH] = L"";
    OPENFILENAMEW o = {sizeof o};
    bool vis = g_hFly && IsWindowVisible(g_hFly) && !g_out;
    o.hwndOwner = vis ? g_hFly : nullptr;
    o.lpstrFilter = L"EQ (*.txt;*.cfg;*.csv)\0*.txt;*.cfg;*.csv\0All\0*.*\0";
    o.lpstrFile = buf;
    o.nMaxFile = MAX_PATH;
    o.Flags =
        OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_NOCHANGEDIR | OFN_EXPLORER;
    g_modal = true;
    BOOL ok = GetOpenFileNameW(&o);
    g_modal = false;
    if (ok)
        SetCurve(buf);
    if (vis && IsWindowVisible(g_hFly))
        SetForegroundWindow(g_hFly);
}

// Slide in from the taskbar edge: window moves while a region clips it to the
// work area.
static void AnimApply() {
    float e = 1 - powf(1 - g_p, 3);
    int off = (int)((1 - e) * (g_rh + g_mg));
    int y = g_fromBottom ? g_ry + off : g_ry - off;
    SetWindowPos(
        g_hFly, HWND_TOPMOST, g_rx, y, g_rw, g_rh,
        SWP_NOACTIVATE | (IsWindowVisible(g_hFly) ? 0 : SWP_SHOWWINDOW));
    if (g_p >= 1) {
        SetWindowRgn(g_hFly, nullptr, TRUE);
        InvalidateRect(g_hFly, nullptr, FALSE);
        return;
    }
    int v0 = 0, v1 = g_rh;
    if (g_fromBottom)
        v1 = g_edge - y;
    else
        v0 = g_edge - y;
    v0 = v0 < 0 ? 0 : v0 > g_rh ? g_rh : v0;
    v1 = v1 < v0 ? v0 : v1 > g_rh ? g_rh : v1;
    SetWindowRgn(g_hFly, CreateRectRgn(0, v0, g_rw, v1), FALSE);
    InvalidateRect(g_hFly, nullptr,
                   FALSE);  // region changes do not invalidate; without this
                            // nothing is ever painted
}

// ---------------------------------------------------------------- live
// spectrum (only while the flyout is open) Loopback capture of the default
// output (post-EQ, post-mixer), FFT in the tray process.
static const int kFft = 4096, kSpec = 200;
static float g_ring[8192], g_spec[kSpec];
static volatile LONG g_spWr, g_spStop;
static unsigned g_spLast;
static int g_spFs = 48000;
static HANDLE g_spThread;

static DWORD WINAPI SpecThread(LPVOID) {
    HRESULT hc = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    IMMDeviceEnumerator* en = nullptr;
    IMMDevice* dev = nullptr;
    IAudioClient* ac = nullptr;
    IAudioCaptureClient* cc = nullptr;
    WAVEFORMATEX* wf = nullptr;
    if (SUCCEEDED(CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr,
                                   CLSCTX_ALL, IID_PPV_ARGS(&en))) &&
        SUCCEEDED(en->GetDefaultAudioEndpoint(eRender, eConsole, &dev)) &&
        SUCCEEDED(dev->Activate(__uuidof(IAudioClient), CLSCTX_ALL, nullptr,
                                (void**)&ac)) &&
        SUCCEEDED(ac->GetMixFormat(&wf)) &&
        ParseFmt(wf).kind ==
            1 &&  // shortcut: float mix format only (the Windows default)
        SUCCEEDED(ac->Initialize(AUDCLNT_SHAREMODE_SHARED,
                                 AUDCLNT_STREAMFLAGS_LOOPBACK, 2000000, 0, wf,
                                 nullptr)) &&
        SUCCEEDED(ac->GetService(__uuidof(IAudioCaptureClient), (void**)&cc)) &&
        SUCCEEDED(ac->Start())) {
        g_spFs = (int)wf->nSamplesPerSec;
        int ch = wf->nChannels;
        unsigned pos = (unsigned)g_spWr;
        while (!g_spStop) {
            Sleep(10);
            UINT32 np;
            while (SUCCEEDED(cc->GetNextPacketSize(&np)) && np) {
                BYTE* d;
                UINT32 nf;
                DWORD fl;
                if (FAILED(cc->GetBuffer(&d, &nf, &fl, nullptr, nullptr)))
                    break;
                for (UINT32 i = 0; i < nf; i++) {
                    float m = 0;
                    if (!(fl & AUDCLNT_BUFFERFLAGS_SILENT)) {
                        for (int c = 0; c < ch; c++)
                            m += ((float*)d)[i * ch + c];
                        m /= ch;
                    }
                    g_ring[pos++ & 8191] = m;
                }
                cc->ReleaseBuffer(nf);
                InterlockedExchange(&g_spWr, (LONG)pos);
            }
        }
        ac->Stop();
    }
    if (wf)
        CoTaskMemFree(wf);
    if (cc)
        cc->Release();
    if (ac)
        ac->Release();
    if (dev)
        dev->Release();
    if (en)
        en->Release();
    if (SUCCEEDED(hc))
        CoUninitialize();
    return 0;
}

static void SpecStop();
static void SpecStart() {
    if (g_spThread && !g_spStop)
        return;
    SpecStop();  // join a previous capture thread that was still shutting down
    g_spStop = 0;
    g_spThread = CreateThread(nullptr, 0, SpecThread, nullptr, 0, nullptr);
    for (int i = 0; i < kSpec; i++)
        g_spec[i] = -120;
}
// Non-blocking: the flyout hides without waiting for the audio setup.
static void SpecRequestStop() {
    g_spStop = 1;
}
static void SpecStop() {
    if (!g_spThread)
        return;
    g_spStop = 1;
    WaitForSingleObject(g_spThread, INFINITE);
    CloseHandle(g_spThread);
    g_spThread = nullptr;
}

static void Fft(float* re, float* im, int n) {
    for (int i = 1, j = 0; i < n; i++) {
        int b = n >> 1;
        for (; j & b; b >>= 1)
            j ^= b;
        j ^= b;
        if (i < j) {
            std::swap(re[i], re[j]);
            std::swap(im[i], im[j]);
        }
    }
    for (int len = 2; len <= n; len <<= 1) {
        double a = -2 * kPi / len, wr = cos(a), wi = sin(a);
        for (int i = 0; i < n; i += len) {
            double cr = 1, ci = 0;
            for (int k = 0; k < len / 2; k++) {
                int u = i + k, v = u + len / 2;
                float tr = (float)(re[v] * cr - im[v] * ci),
                      ti = (float)(re[v] * ci + im[v] * cr);
                re[v] = re[u] - tr;
                im[v] = im[u] - ti;
                re[u] += tr;
                im[u] += ti;
                double nr = cr * wr - ci * wi;
                ci = cr * wi + ci * wr;
                cr = nr;
            }
        }
    }
}

static void SpecUpdate() {
    static float re[kFft], im[kFft], win[kFft];
    static bool init;
    if (!init) {
        for (int i = 0; i < kFft; i++)
            win[i] = 0.5f - 0.5f * (float)cos(2 * kPi * i / kFft);
        init = true;
    }
    unsigned w = (unsigned)g_spWr;
    bool fresh = w != g_spLast;
    g_spLast = w;
    if (fresh) {
        for (int i = 0; i < kFft; i++) {
            re[i] = g_ring[(w - kFft + i) & 8191] * win[i];
            im[i] = 0;
        }
        Fft(re, im, kFft);
    }
    for (int j = 0; j < kSpec; j++) {
        float db = -120;
        if (fresh) {
            double f = 20 * pow(1000.0, j / (kSpec - 1.0));
            int b0 = (int)(f / 1.018 * kFft / g_spFs),
                b1 = (int)ceil(f * 1.018 * kFft / g_spFs);
            if (b1 <= b0)
                b1 = b0 + 1;
            if (b1 > kFft / 2)
                b1 = kFft / 2;
            float mx = 0;
            for (int b = b0; b <= b1; b++) {
                float m = re[b] * re[b] + im[b] * im[b];
                if (m > mx)
                    mx = m;
            }
            db = 20 * log10f(sqrtf(mx) / (kFft / 4) + 1e-9f);
        }
        g_spec[j] = db > g_spec[j] - 1.4f
                        ? db
                        : g_spec[j] - 1.4f;  // fast attack, slow fall
    }
}

static void HideFlyout() {
    if (!IsWindowVisible(g_hFly) || g_out)
        return;
    g_out = true;
    g_naming = false;
    g_lastDeact = g_animT = GetTickCount();
    SetTimer(g_hFly, 2, 15, nullptr);
}

static void ShowFlyout() {
    bool vis = IsWindowVisible(g_hFly) != 0;
    RECT ir;
    NOTIFYICONIDENTIFIER id = {sizeof id};
    id.hWnd = g_hTray;
    id.uID = 1;
    POINT c;
    GetCursorPos(&c);
    if (SUCCEEDED(Shell_NotifyIconGetRect(&id, &ir)))
        c = {(ir.left + ir.right) / 2, (ir.top + ir.bottom) / 2};
    MONITORINFO mi = {sizeof mi};
    GetMonitorInfoW(MonitorFromPoint(c, MONITOR_DEFAULTTONEAREST), &mi);
    if (!vis) {
        SetWindowPos(g_hFly, HWND_TOPMOST, c.x, c.y, 1, 1, SWP_NOACTIVATE);
        g_s = GetDpiForWindow(g_hFly) / 96.f;
        SpecStart();
        SetTimer(g_hFly, 4, 33, nullptr);
    }
    g_rw = (int)(W * g_s);
    g_rh = (int)(H * g_s);
    g_mg = (int)(12 * g_s);
    int work = mi.rcWork.bottom - mi.rcWork.top - 2 * g_mg;
    if (g_rh > work)
        g_rh = work;
    g_fromBottom = c.y >= (mi.rcWork.top + mi.rcWork.bottom) / 2;
    g_rx = c.x - g_rw / 2;
    if (g_rx > mi.rcWork.right - g_rw - g_mg)
        g_rx = mi.rcWork.right - g_rw - g_mg;
    if (g_rx < mi.rcWork.left + g_mg)
        g_rx = mi.rcWork.left + g_mg;
    g_ry = g_fromBottom ? mi.rcWork.bottom - g_rh - g_mg : mi.rcWork.top + g_mg;
    g_edge = g_fromBottom ? mi.rcWork.bottom : mi.rcWork.top;
    ReleaseRT();
    LoadTheme();
    ApplyDwm();
    g_out = false;
    if (!vis) {
        g_p = 0;
        g_dim = g_cfg.eqEnabled ? 0.f : 1.f;
    }
    g_animT = GetTickCount();
    SetTimer(g_hFly, 2, 15, nullptr);
    AnimApply();
    SetForegroundWindow(g_hFly);
    InvalidateRect(g_hFly, nullptr, FALSE);
}

static void ShowMenu() {
    HMENU m = CreatePopupMenu();
    AppendMenuW(m, MF_STRING | (g_cfg.eqEnabled ? MF_CHECKED : 0), 1, T(S_EQ));
    AppendMenuW(m, MF_STRING | (g_cfg.curveEnabled ? MF_CHECKED : 0), 2,
                T(S_CURVE));
    AppendMenuW(m, MF_SEPARATOR, 0, nullptr);
    AppendMenuW(m, MF_STRING, 3, T(S_LOADFILE));
    AppendMenuW(m, MF_STRING, 4, T(S_OPEN));
    POINT pt;
    GetCursorPos(&pt);
    SetForegroundWindow(g_hTray);
    int id =
        TrackPopupMenu(m, TPM_RETURNCMD | TPM_RIGHTBUTTON | TPM_BOTTOMALIGN,
                       pt.x, pt.y, 0, g_hTray, nullptr);
    PostMessageW(g_hTray, WM_NULL, 0, 0);
    DestroyMenu(m);
    if (id == 1) {
        g_cfg.eqEnabled ^= 1;
        Commit();
    } else if (id == 2) {
        g_cfg.curveEnabled ^= 1;
        Commit();
    } else if (id == 3)
        PostMessageW(g_hTray, WM_PICK, 0, 0);
    else if (id == 4)
        ShowFlyout();
}

// ---- presets (whole manual EQ + the fitted monitor curve; one binary blob)
struct Preset {
    wchar_t name[32], curve[40];
    int mode, curveOn, invert, nFile;
    float preamp, filePre;
    float graphic[NG];
    Band param[MAXP];
    Band file[MAXF];
};
const int MAXPRE = 24;
static std::vector<Preset> g_pre;
static int g_nameLen;
static wchar_t g_nameBuf[32];

static void SanBand(Band& b) {
    b.type = b.type < 0 || b.type > T_HP ? T_PK : b.type;
    b.fc = Cf(b.fc, 10, 24000);
    b.gain = Cf(b.gain, -30, 30);
    b.q = Cf(b.q, 0.05f, 20);
    b.on = b.on != 0;
}

// One value per preset: portable Windhawk caps a stored binary value at
// about 16 KB.
static int g_presetsStored;
static void SavePresets() {
    int n = (int)g_pre.size();
    wchar_t key[24];
    for (int i = 0; i < n; i++) {
        swprintf(key, 24, L"preset%d", i);
        Wh_SetBinaryValue(key, &g_pre[i], sizeof(Preset));
    }
    for (int i = n; i < g_presetsStored; i++) {
        swprintf(key, 24, L"preset%d", i);
        Wh_DeleteValue(key);
    }
    g_presetsStored = n;
    Wh_SetIntValue(L"presetCount", n);
}
static void LoadPresets() {
    int n = Wh_GetIntValue(L"presetCount", 0);
    n = n < 0 ? 0 : n > MAXPRE ? MAXPRE : n;
    g_pre.clear();
    wchar_t key[24];
    for (int i = 0; i < n; i++) {
        Preset p;
        swprintf(key, 24, L"preset%d", i);
        if (Wh_GetBinaryValue(key, &p, sizeof p) != sizeof p)
            break;
        g_pre.push_back(p);
    }
    g_presetsStored = (int)g_pre.size();
    for (Preset& p : g_pre) {
        p.name[31] = 0;
        p.curve[39] = 0;
        p.mode = p.mode == 1;
        p.curveOn = p.curveOn != 0;
        p.invert = p.invert != 0;
        p.nFile = p.nFile < 0 ? 0 : p.nFile > MAXF ? MAXF : p.nFile;
        p.preamp = Cf(p.preamp, -12, 12);
        p.filePre = Cf(p.filePre, -30, 30);
        for (float& g : p.graphic)
            g = Cf(g, -12, 12);
        for (Band& b : p.param)
            SanBand(b);
        for (Band& b : p.file)
            SanBand(b);
    }
}
static void ApplyPreset(int i) {
    const Preset& p = g_pre[i];
    g_cfg.mode = p.mode;
    g_cfg.preamp = p.preamp;
    memcpy(g_cfg.graphic, p.graphic, sizeof p.graphic);
    memcpy(g_cfg.param, p.param, sizeof p.param);
    memcpy(g_cfg.file, p.file, sizeof p.file);
    g_cfg.nFile = p.nFile;
    g_cfg.filePreamp = p.filePre;
    g_cfg.curveEnabled = p.curveOn;
    g_cfg.invert = p.invert;
    g_curveName = p.curve;
    g_curveMsg.clear();
    Wh_SetStringValue(L"curveName", p.curve);
    g_preCur = i;
    Commit();
}
static void SavePresetAs(const wchar_t* name) {
    Preset p = {};
    wcsncpy(p.name, name, 31);
    wcsncpy(p.curve, g_curveName.c_str(), 39);
    p.mode = g_cfg.mode;
    p.preamp = g_cfg.preamp;
    p.curveOn = g_cfg.curveEnabled;
    p.invert = g_cfg.invert;
    p.nFile = g_cfg.nFile;
    p.filePre = g_cfg.filePreamp;
    memcpy(p.graphic, g_cfg.graphic, sizeof p.graphic);
    memcpy(p.param, g_cfg.param, sizeof p.param);
    memcpy(p.file, g_cfg.file, sizeof p.file);
    int at = -1;
    for (int i = 0; i < (int)g_pre.size(); i++)
        if (!_wcsicmp(g_pre[i].name, name))
            at = i;
    if (at >= 0)
        g_pre[at] = p;
    else if ((int)g_pre.size() < MAXPRE) {
        g_pre.push_back(p);
        at = (int)g_pre.size() - 1;
    } else
        return;
    g_preCur = at;
    SavePresets();
}
static void DeletePresetAt(int i) {
    if (i < 0 || i >= (int)g_pre.size())
        return;
    g_pre.erase(g_pre.begin() + i);
    if (g_preCur == i)
        g_preCur = -1;
    else if (g_preCur > i)
        g_preCur--;
    SavePresets();
}
static void ResetEq() {
    for (float& g : g_cfg.graphic)
        g = 0;
    DefaultBands();
    g_sel = 0;
    g_cfg.preamp = 0;
    g_preCur = -1;
    Commit();
}
static void AutoPre() {  // headroom so the curve never boosts above 0 dBFS
    Bq q[MAXF];
    int m = 0;
    for (int i = 0; i < g_cfg.nFile; i++)
        if (Coef(g_cfg.file[i], 48000, q[m]))
            m++;
    double peak = -99;
    for (int k = 0; k < 300; k++) {
        double f = 20 * pow(1000.0, k / 299.0), r = 0;
        for (int j = 0; j < m; j++)
            r += MagDb(q[j], f, 48000);
        if (r > peak)
            peak = r;
    }
    g_cfg.filePreamp = peak > 0 ? (float)Cd(-ceil(peak * 10) / 10, -30, 0) : 0;
}

// Keep graphic and parametric in step: re-fit the target mode's gains to the
// other mode's response.
static double RespDb(const Band* b, int n, double f) {
    double r = 0;
    Bq q;
    for (int i = 0; i < n; i++)
        if (b[i].on && Coef(b[i], 48000, q))
            r += MagDb(q, f, 48000);
    return r;
}
static void SolveGains(Band* b, int n, const double* tg, double lim) {
    double r[MAXP + NG];
    for (int it = 0; it < 40; it++) {
        for (int i = 0; i < n; i++)
            r[i] = RespDb(b, n, b[i].fc);
        for (int i = 0; i < n; i++)
            b[i].gain = (float)Cd(b[i].gain + 0.8 * (tg[i] - r[i]), -lim, lim);
    }
}
static void SyncModes(int to) {
    Band g[NG];
    for (int i = 0; i < NG; i++)
        g[i] = {T_PK, kGfx[i], g_cfg.graphic[i], 1.41f};
    if (to == 1) {  // graphic -> parametric
        Band w[MAXP];
        int idx[MAXP], n = 0;
        double tg[MAXP];
        for (int i = 0; i < MAXP; i++)
            if (g_cfg.param[i].type < T_LP) {
                w[n] = g_cfg.param[i];
                w[n].on = 1;
                w[n].gain = 0;
                tg[n] = RespDb(g, NG, w[n].fc);
                idx[n++] = i;
            }
        SolveGains(w, n, tg, 15);
        for (int k = 0; k < n; k++) {
            Band& p = g_cfg.param[idx[k]];
            p.gain = roundf(w[k].gain * 10) / 10;
            if (fabsf(p.gain) >= 0.1f)
                p.on = 1;
        }
    } else {  // parametric -> graphic
        double tg[NG];
        for (int i = 0; i < NG; i++) {
            tg[i] = RespDb(g_cfg.param, MAXP, kGfx[i]);
            g[i].gain = 0;
        }
        SolveGains(g, NG, tg, 12);
        for (int i = 0; i < NG; i++)
            g_cfg.graphic[i] = roundf(g[i].gain * 2) / 2;
    }
}

// ---- layout (DIP). Plot rect depends on mode; knob row only in parametric
// mode.
static float g_px, g_py, g_pw, g_ph;
static const float GT = 348, GB = 436, GCW = PW / 11;
static const float KB = 334, KWD = PW / 4, CELLY = 410, BARY = 490;
static double g_dragY0, g_dragT0;
static void Layout() {
    g_px = PX;
    g_py = 176;
    g_pw = PW;
    g_ph = 150;
}

// ---- drawing helpers (the render target carries the DPI)
static void Col(float r, float g, float b, float a = 1) {
    g_br->SetColor(D2D1::ColorF(r, g, b, a));
}
static void Fg(float a) {
    if (g_dark)
        Col(1, 1, 1, a);
    else
        Col(0, 0, 0, a);
}
static void Acc(float a = 1) {
    Col(g_acc[0], g_acc[1], g_acc[2], a);
}
static void FillR(float x, float y, float w, float h, float r) {
    g_rt->FillRoundedRectangle(
        D2D1::RoundedRect(D2D1::RectF(x, y, x + w, y + h), r, r), g_br);
}
static void StrokeR(float x, float y, float w, float h, float r) {
    g_rt->DrawRoundedRectangle(
        D2D1::RoundedRect(
            D2D1::RectF(x + .5f, y + .5f, x + w - .5f, y + h - .5f), r, r),
        g_br, 1);
}
static void Text(const wchar_t* s,
                 float x,
                 float y,
                 float w,
                 float h,
                 IDWriteTextFormat* f) {
    g_rt->DrawText(s, (UINT32)wcslen(s), f, D2D1::RectF(x, y, x + w, y + h),
                   g_br, D2D1_DRAW_TEXT_OPTIONS_CLIP);
}
static void Line(float x0, float y0, float x1, float y1, float w = 1) {
    g_rt->DrawLine(D2D1::Point2F(x0, y0), D2D1::Point2F(x1, y1), g_br, w);
}
static void Dot(float x, float y, float r) {
    g_rt->FillEllipse(D2D1::Ellipse(D2D1::Point2F(x, y), r, r), g_br);
}
static void Ring(float x, float y, float r, float w) {
    g_rt->DrawEllipse(D2D1::Ellipse(D2D1::Point2F(x, y), r, r), g_br, w);
}

static void Toggle(float x, float y, float w, const wchar_t* label, bool on) {
    Fg(.95f);
    Text(label, x, y, w - 50, 36, g_tfL);
    float sx = x + w - 44, ty = y + 7;
    if (on) {
        Acc();
        FillR(sx, ty, 44, 22, 11);
        Col(1, 1, 1);
        Dot(sx + 33, ty + 11, 6.5f);
    } else {
        Fg(.55f);
        StrokeR(sx, ty, 44, 22, 11);
        Dot(sx + 11, ty + 11, 5.5f);
    }
}
static void IconBtn(float x,
                    float y,
                    float s,
                    const wchar_t* glyph,
                    bool on,
                    bool en = true) {
    Fg(.08f);
    FillR(x, y, s, s, 6);
    if (on)
        Acc();
    else
        Fg(en ? .8f : .35f);
    Text(glyph, x, y, s, s, g_tfI);
}
static void Button(float x,
                   float y,
                   float w,
                   float h,
                   const wchar_t* s,
                   bool en = true) {
    Fg(.08f);
    FillR(x, y, w, h, 5);
    Fg(.14f);
    StrokeR(x, y, w, h, 5);
    Fg(en ? .95f : .35f);
    Text(s, x, y, w, h, g_tfC);
}
static float PlotX(float f) {
    return g_px + log10f(f / 20) / 3 * g_pw;
}
static float PlotY(float db) {
    return g_py + g_ph / 2 - Cf(db, -15, 15) / 15 * g_ph / 2;
}
static float NodeY(const Band& b) {
    return b.type >= T_LP ? g_py + g_ph / 2 : PlotY(b.gain);
}

static void Icon(int type, float x, float y, float w, float h) {
    static const float P[5][5][2] = {
        {{0, .75f}, {.3f, .75f}, {.5f, .1f}, {.7f, .75f}, {1, .75f}},
        {{0, .2f}, {.35f, .2f}, {.65f, .8f}, {1, .8f}, {1, .8f}},
        {{0, .8f}, {.35f, .8f}, {.65f, .2f}, {1, .2f}, {1, .2f}},
        {{0, .95f}, {.35f, .8f}, {.55f, .2f}, {1, .2f}, {1, .2f}},
        {{0, .2f}, {.45f, .2f}, {.65f, .8f}, {1, .95f}, {1, .95f}}};
    for (int i = 0; i < 4; i++)
        Line(x + P[type][i][0] * w, y + P[type][i][1] * h,
             x + P[type][i + 1][0] * w, y + P[type][i + 1][1] * h, 1.6f);
}

static double KnobT(int k) {
    const Band& b = g_cfg.param[g_sel];
    switch (k) {
        case 0:
            return log10(b.fc / 20.0) / 3;
        case 1:
            return (b.gain + 15) / 30;
        case 2:
            return log(b.q / 0.1) / log(200.0);
        default:
            return (g_cfg.preamp + 12) / 24;
    }
}
static void KnobSet(int k, double t) {
    t = t < 0 ? 0 : t > 1 ? 1 : t;
    Band& b = g_cfg.param[g_sel];
    switch (k) {
        case 0:
            b.fc = (float)(20 * pow(1000.0, t));
            break;
        case 1:
            b.gain = roundf((float)(t * 30 - 15) * 10) / 10;
            break;
        case 2:
            b.q = (float)(0.1 * pow(200.0, t));
            break;
        default:
            g_cfg.preamp = roundf((float)(t * 24 - 12) * 2) / 2;
            break;
    }
    if (k < 3)
        b.on = 1;
    Commit();
}
static void DrawKnob(int k, const wchar_t* label, const wchar_t* val, bool en) {
    float bx = PX + k * KWD, cx = bx + KWD / 2, cy = KB + 30, r = 13;
    double a0 = kPi * 0.75, sw = kPi * 1.5, t = KnobT(k);
    auto pt = [&](double u, float rr) {
        return D2D1::Point2F(cx + rr * (float)cos(a0 + sw * u),
                             cy + rr * (float)sin(a0 + sw * u));
    };
    Fg(en ? .8f : .35f);
    Text(label, bx, KB, KWD, 14, g_tfS);
    Fg(.22f);
    for (int i = 0; i < 40; i++)
        g_rt->DrawLine(pt(i / 40.0, r), pt((i + 1) / 40.0, r), g_br, 3);
    double ts = (k == 1 || k == 3) ? 0.5 : 0, lo = t < ts ? t : ts,
           hi = t < ts ? ts : t;
    Acc(en ? 1.f : .3f);
    for (int i = 0; i < 40; i++)
        g_rt->DrawLine(pt(lo + (hi - lo) * i / 40, r),
                       pt(lo + (hi - lo) * (i + 1) / 40, r), g_br, 3);
    Fg(en ? .9f : .35f);
    g_rt->DrawLine(pt(t, 4), pt(t, 11), g_br, 2);
    Fg(en ? .9f : .4f);
    Text(val, bx, KB + 47, KWD, 16, g_tfS);
}

static void DrawSliderCol(int col, float val, const wchar_t* name) {
    float cx = PX + (col + .5f) * GCW;
    Fg(.25f);
    FillR(cx - 2, GT, 4, GB - GT, 2);
    float yv = GB - (val + 12) / 24 * (GB - GT), y0 = GB - 0.5f * (GB - GT);
    Acc();
    FillR(cx - 2, yv < y0 ? yv : y0, 4, fabsf(yv - y0), 2);
    Fg(.95f);
    Dot(cx, yv, 7);
    Acc();
    Ring(cx, yv, 7, 2);
    wchar_t b[16];
    swprintf(b, 16, L"%+.1f", val);
    Fg(.7f);
    Text(b, cx - GCW / 2, GB + 10, GCW, 14, g_tfS);
    Text(name, cx - GCW / 2, GB + 26, GCW, 14, g_tfS);
}

static void DrawBar() {
    float y = BARY;
    if (g_naming) {
        Fg(.1f);
        FillR(PX, y, PW, 30, 5);
        Acc();
        StrokeR(PX, y, PW, 30, 5);
        std::wstring s = std::wstring(g_nameBuf) + L"|";
        Fg(.95f);
        Text(s.c_str(), PX + 8, y, 190, 30, g_tfL);
        Fg(.5f);
        Text(T(S_NAMEHINT), PX + 195, y, 160, 30, g_tfS);
        return;
    }
    std::wstring pn = (g_preCur >= 0 && g_preCur < (int)g_pre.size()
                           ? std::wstring(g_pre[g_preCur].name)
                           : std::wstring(T(S_PRESETS))) +
                      L" \u25be";
    Button(PX, y, 190, 30, pn.c_str());
    Button(216, y, 110, 30, T(S_SAVE));
    IconBtn(350, y, 30, L"\uE72C", false);  // reset (circular arrow)
}

static void Paint() {
    PAINTSTRUCT ps;
    BeginPaint(g_hFly, &ps);
    if (!g_rt) {
        RECT rc;
        GetClientRect(g_hFly, &rc);
        float dpi = (float)GetDpiForWindow(g_hFly);
        g_d2d->CreateHwndRenderTarget(
            D2D1::RenderTargetProperties(
                D2D1_RENDER_TARGET_TYPE_DEFAULT,
                D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM,
                                  D2D1_ALPHA_MODE_PREMULTIPLIED),
                dpi, dpi),
            D2D1::HwndRenderTargetProperties(g_hFly,
                                             D2D1::SizeU(rc.right, rc.bottom)),
            &g_rt);
        if (g_rt)
            g_rt->CreateSolidColorBrush(D2D1::ColorF(1, 1, 1), &g_br);
    }
    if (g_rt && g_br) {
        Layout();
        g_rt->BeginDraw();
        g_rt->Clear(D2D1::ColorF(0, 0, 0, 0));
        Col(g_tint[0], g_tint[1], g_tint[2], g_tint[3]);
        g_rt->FillRectangle(D2D1::RectF(0, 0, W, H), g_br);
        Fg(.98f);
        Text(L"Tray EQ", PX, 10, 200, 34, g_tfT);
        IconBtn(PX + PW - 34, 11, 32, L"\uE7E8", g_cfg.eqEnabled);  // power I/O
        IconBtn(PX + PW - 74, 11, 32, L"\uE713", false,
                g_cfg.eqEnabled);  // settings
        {
            std::wstring cl = !g_curveMsg.empty() ? g_curveMsg
                              : g_curveName.empty()
                                  ? std::wstring(T(S_CURVE))
                                  : std::wstring(T(S_CURVEPFX)) + g_curveName;
            Toggle(PX, 50, PW, L"", g_cfg.curveEnabled);
            Fg(.95f);
            Text(cl.c_str(), PX, 50, 195, 36, g_tfL);
        }
        Button(222, 53, 100, 30, T(S_LOAD));
        Toggle(PX, 86, PW, T(S_INVERT), g_cfg.invert);
        // segmented control
        Fg(.08f);
        FillR(PX, 132, PW, 32, 6);
        Acc(.9f);
        FillR(PX + 3 + g_cfg.mode * (PW / 2 - 3), 135, PW / 2 - 3, 26, 5);
        Fg(.85f);
        Text(g_cfg.mode == 0 ? T(S_PARAM) : T(S_GFX),
             g_cfg.mode == 0 ? PX + PW / 2 : PX, 132, PW / 2, 32, g_tfC);
        Col(1, 1, 1);
        Text(g_cfg.mode == 0 ? T(S_GFX) : T(S_PARAM),
             g_cfg.mode == 0 ? PX : PX + PW / 2, 132, PW / 2, 32, g_tfC);
        // plot
        Fg(.07f);
        FillR(g_px, g_py, g_pw, g_ph, 8);
        g_rt->PushAxisAlignedClip(
            D2D1::RectF(g_px, g_py, g_px + g_pw, g_py + g_ph),
            D2D1_ANTIALIAS_MODE_ALIASED);
        for (int d = 10; d <= 10000; d *= 10)
            for (int m = 1; m < 10; m++) {
                float f = (float)(m * d);
                if (f < 20 || f > 20000)
                    continue;
                Fg(m == 1 ? .12f : .05f);
                Line(PlotX(f), g_py, PlotX(f), g_py + g_ph);
            }
        for (int db = -12; db <= 12; db += 6) {
            Fg(db ? .08f : .25f);
            Line(g_px, PlotY((float)db), g_px + g_pw, PlotY((float)db));
            wchar_t b[8];
            swprintf(b, 8, L"%d", db);
            Fg(.5f);
            Text(b, g_px + 4, PlotY((float)db) - 8, 24, 16, g_tfS);
        }
        static const float gf[3] = {100, 1000, 10000};
        static const wchar_t* gl[3] = {L"100", L"1k", L"10k"};
        for (int i = 0; i < 3; i++) {
            Fg(.5f);
            Text(gl[i], PlotX(gf[i]) + 2, g_py + g_ph - 16, 30, 14, g_tfS);
        }
        {  // smooth area: 5-tap blur across bins, then Catmull-Rom curve as
           // bezier segments
            float sm[kSpec], pt[kSpec][2];
            for (int j = 0; j < kSpec; j++) {
                float a = 0, wsum = 0;
                for (int d = -2; d <= 2; d++) {
                    int k = j + d;
                    if (k < 0 || k >= kSpec)
                        continue;
                    float wt = 3.f - (d < 0 ? -d : d);
                    a += g_spec[k] * wt;
                    wsum += wt;
                }
                sm[j] = a / wsum;
                pt[j][0] = g_px + g_pw * j / (kSpec - 1.f);
                pt[j][1] = g_py + g_ph * (1 - Cf((sm[j] + 80) / 80, 0, 1));
            }
            ID2D1PathGeometry* geo = nullptr;
            ID2D1GeometrySink* sink = nullptr;
            if (SUCCEEDED(g_d2d->CreatePathGeometry(&geo)) &&
                SUCCEEDED(geo->Open(&sink))) {
                sink->BeginFigure(D2D1::Point2F(pt[0][0], g_py + g_ph),
                                  D2D1_FIGURE_BEGIN_FILLED);
                sink->AddLine(D2D1::Point2F(pt[0][0], pt[0][1]));
                for (int j = 0; j < kSpec - 1; j++) {
                    const float *p0 = pt[j ? j - 1 : 0], *p1 = pt[j],
                                *p2 = pt[j + 1],
                                *p3 = pt[j + 2 < kSpec ? j + 2 : kSpec - 1];
                    sink->AddBezier(D2D1::BezierSegment(
                        D2D1::Point2F(p1[0] + (p2[0] - p0[0]) / 6,
                                      p1[1] + (p2[1] - p0[1]) / 6),
                        D2D1::Point2F(p2[0] - (p3[0] - p1[0]) / 6,
                                      p2[1] - (p3[1] - p1[1]) / 6),
                        D2D1::Point2F(p2[0], p2[1])));
                }
                sink->AddLine(D2D1::Point2F(pt[kSpec - 1][0], g_py + g_ph));
                sink->EndFigure(D2D1_FIGURE_END_CLOSED);
                sink->Close();
                if (g_dark)
                    Col(1, 1, 1, .14f);
                else
                    Col(0, 0, 0, .14f);
                g_rt->FillGeometry(geo, g_br);
            }
            if (sink)
                sink->Release();
            if (geo)
                geo->Release();
        }
        g_rt->PopAxisAlignedClip();
        {
            Band ch[64];
            float pre;
            int n = BuildChain(g_cfg, ch, &pre), m = 0;
            Bq q[64];
            for (int i = 0; i < n; i++)
                if (Coef(ch[i], 48000, q[m]))
                    m++;
            Acc();
            D2D1_POINT_2F prev = {};
            for (int i = 0; i < 200; i++) {
                double f = 20 * pow(1000.0, i / 199.0), db = pre;
                for (int k = 0; k < m; k++)
                    db += MagDb(q[k], f, 48000);
                D2D1_POINT_2F p =
                    D2D1::Point2F(g_px + g_pw * i / 199.f, PlotY((float)db));
                if (i)
                    g_rt->DrawLine(prev, p, g_br, 2);
                prev = p;
            }
        }
        if (g_cfg.mode == 1) {
            for (int i = 0; i < MAXP; i++) {
                const Band& b = g_cfg.param[i];
                float x = PlotX(b.fc), y = NodeY(b);
                Acc(b.on ? 1.f : .35f);
                Dot(x, y, 9);
                if (i == g_sel) {
                    Col(1, 1, 1);
                    Ring(x, y, 9, 1.5f);
                }
                wchar_t n[4];
                swprintf(n, 4, L"%d", i + 1);
                Col(0, 0, 0, b.on ? .85f : .6f);
                Text(n, x - 9, y - 9, 18, 18, g_tfS);
            }
            const Band& s = g_cfg.param[g_sel];
            wchar_t b[32];
            if (s.fc >= 1000)
                swprintf(b, 32, L"%.2f kHz", s.fc / 1000);
            else
                swprintf(b, 32, L"%.0f Hz", s.fc);
            DrawKnob(0, T(S_FREQ), b, true);
            swprintf(b, 32, L"%.2f dB", s.gain);
            DrawKnob(1, T(S_GAIN), b, s.type < T_LP);
            swprintf(b, 32, L"%.2f", s.q);
            DrawKnob(2, L"Q", b, true);
            swprintf(b, 32, L"%.1f dB", g_cfg.preamp);
            DrawKnob(3, T(S_PREAMP), b, true);
            for (int i = 0; i < MAXP; i++) {
                const Band& bd = g_cfg.param[i];
                float x = PX + i * 45.f;
                Fg(i == g_sel ? .16f : .07f);
                FillR(x + 1.5f, CELLY, 42, 28, 5);
                if (i == g_sel) {
                    Acc();
                    StrokeR(x + 1.5f, CELLY, 42, 28, 5);
                }
                Fg(bd.on ? .95f : .45f);
                Icon(bd.type, x + 10, CELLY + 7, 24, 14);
                if (bd.on) {
                    Acc();
                    FillR(x + 4, CELLY + 34, 14, 14, 3);
                } else {
                    Fg(.5f);
                    StrokeR(x + 4, CELLY + 34, 14, 14, 3);
                }
                wchar_t n[4];
                swprintf(n, 4, L"%d", i + 1);
                Fg(.85f);
                Text(n, x + 21, CELLY + 30, 20, 22, g_tfL);
            }
        } else {
            static const wchar_t* fn[NG] = {L"31",  L"62", L"125", L"250",
                                            L"500", L"1k", L"2k",  L"4k",
                                            L"8k",  L"16k"};
            for (int i = 0; i < NG; i++)
                DrawSliderCol(i, g_cfg.graphic[i], fn[i]);
            DrawSliderCol(10, g_cfg.preamp, T(S_PRE));
        }
        DrawBar();
        if (g_dim > 0) {
            Col(0, 0, 0, (g_dark ? .5f : .32f) * g_dim);
            g_rt->FillRectangle(D2D1::RectF(0, 48, W, H), g_br);
        }
        if (g_rt->EndDraw() == D2DERR_RECREATE_TARGET)
            ReleaseRT();
    }
    EndPaint(g_hFly, &ps);
}

static bool In(float x, float y, float rx, float ry, float rw, float rh) {
    return x >= rx && x < rx + rw && y >= ry && y < ry + rh;
}
static int SliderAt(float x, float y) {
    if (g_cfg.mode != 0 || !In(x, y, PX, GT - 12, PW, GB - GT + 24))
        return -1;
    int c = (int)((x - PX) / GCW);
    return c > 10 ? 10 : c;
}
static int KnobAt(float x, float y) {
    if (g_cfg.mode != 1)
        return -1;
    for (int k = 0; k < 4; k++)
        if (In(x, y, PX + k * KWD, KB + 12, KWD, 56))
            return k;
    return -1;
}
static int NodeAt(float x, float y) {
    if (g_cfg.mode != 1)
        return -1;
    int best = -1;
    float bd = 14 * 14;
    for (int i = 0; i < MAXP; i++) {
        const Band& b = g_cfg.param[i];
        float dx = PlotX(b.fc) - x, dy = NodeY(b) - y;
        if (dx * dx + dy * dy < bd) {
            bd = dx * dx + dy * dy;
            best = i;
        }
    }
    return best;
}
static void SetSlider(int col, float y) {
    float v = Cf((GB - y) / (GB - GT) * 24 - 12, -12, 12);
    v = roundf(v * 2) / 2;
    if (fabsf(v) < 0.26f)
        v = 0;
    (col == 10 ? g_cfg.preamp : g_cfg.graphic[col]) = v;
    Commit();
}
static void DragNode(float x, float y) {
    Band& b = g_cfg.param[g_sel];
    b.fc = Cf(20 * powf(1000.f, (x - g_px) / g_pw), 20, 20000);
    if (b.type < T_LP)
        b.gain = Cf(roundf((g_py + g_ph / 2 - y) / (g_ph / 2) * 15 * 10) / 10,
                    -15, 15);
    Commit();
}
static void
ToggleInvert() {  // RBJ peak/shelf with -gain is the exact inverse filter
    g_cfg.invert ^= 1;
    for (int i = 0; i < g_cfg.nFile; i++)
        g_cfg.file[i].gain = -g_cfg.file[i].gain;
    AutoPre();
    Commit();
}

static void ShowPresetMenu() {
    HMENU m = CreatePopupMenu();
    if (g_pre.empty())
        AppendMenuW(m, MF_GRAYED | MF_STRING, 0, T(S_NOPRE));
    for (int i = 0; i < (int)g_pre.size(); i++)
        AppendMenuW(m, MF_STRING | (i == g_preCur ? MF_CHECKED : 0), 100 + i,
                    g_pre[i].name);
    POINT pt = {(int)(PX * g_s), (int)(BARY * g_s)};
    ClientToScreen(g_hFly, &pt);
    g_modal = true;
    int id = TrackPopupMenu(m, TPM_RETURNCMD | TPM_BOTTOMALIGN | TPM_LEFTALIGN,
                            pt.x, pt.y, 0, g_hFly, nullptr);
    g_modal = false;
    DestroyMenu(m);
    if (id >= 100 && id - 100 < (int)g_pre.size())
        ApplyPreset(id - 100);
    SetForegroundWindow(g_hFly);
}

static void ShowSettingsMenu() {
    HMENU m = CreatePopupMenu(), sub = CreatePopupMenu();
    for (int i = 0; i < (int)g_pre.size(); i++)
        AppendMenuW(sub, MF_STRING, 200 + i, g_pre[i].name);
    AppendMenuW(m, MF_POPUP | (g_pre.empty() ? MF_GRAYED : 0), (UINT_PTR)sub,
                T(S_DELPRE));
    POINT pt = {(int)((PX + PW - 74) * g_s), (int)(44 * g_s)};
    ClientToScreen(g_hFly, &pt);
    g_modal = true;
    int id = TrackPopupMenu(m, TPM_RETURNCMD | TPM_LEFTALIGN | TPM_TOPALIGN,
                            pt.x, pt.y, 0, g_hFly, nullptr);
    g_modal = false;
    DestroyMenu(m);
    if (id >= 200 && id < 300)
        DeletePresetAt(id - 200);
    SetForegroundWindow(g_hFly);
    InvalidateRect(g_hFly, nullptr, FALSE);
}

static void OnDown(float x, float y) {
    if (g_naming) {
        if (In(x, y, PX, BARY, PW, 30))
            return;
        g_naming = false;
        InvalidateRect(g_hFly, nullptr, FALSE);
    }
    if (In(x, y, PX + PW - 34, 11, 32, 32)) {
        g_cfg.eqEnabled ^= 1;
        Commit();
        return;
    }
    if (!g_cfg.eqEnabled)
        return;
    if (In(x, y, PX + PW - 74, 11, 32, 32)) {
        ShowSettingsMenu();
        return;
    }
    if (In(x, y, 222, 53, 100, 30)) {
        PostMessageW(g_hTray, WM_PICK, 0, 0);
        return;
    }
    if (In(x, y, PX, 50, 195, 36) || In(x, y, PX + PW - 44, 50, 44, 36)) {
        g_cfg.curveEnabled ^= 1;
        Commit();
        return;
    }
    if (In(x, y, PX, 86, PW, 36)) {
        ToggleInvert();
        return;
    }
    if (In(x, y, PX, 132, PW, 32)) {
        int nm = x > PX + PW / 2;
        if (nm != g_cfg.mode)
            SyncModes(nm);
        g_cfg.mode = nm;
        Layout();
        Commit();
        return;
    }
    if (In(x, y, PX, BARY, PW, 30)) {
        if (x < 214)
            ShowPresetMenu();
        else if (x < 330) {
            g_naming = true;
            g_nameBuf[0] = 0;
            g_nameLen = 0;
            if (g_preCur >= 0 && g_preCur < (int)g_pre.size())
                wcscpy(g_nameBuf, g_pre[g_preCur].name);
            else
                swprintf(g_nameBuf, 32, T(S_PRESETN), (int)g_pre.size() + 1);
            g_nameLen = (int)wcslen(g_nameBuf);
            InvalidateRect(g_hFly, nullptr, FALSE);
        } else if (x >= 340)
            ResetEq();
        return;
    }
    int c = SliderAt(x, y);
    if (c >= 0) {
        g_drag = 1;
        g_dragCol = c;
        SetCapture(g_hFly);
        SetSlider(c, y);
        return;
    }
    if (g_cfg.mode != 1)
        return;
    int n = NodeAt(x, y);
    if (n >= 0) {
        g_sel = n;
        g_cfg.param[n].on = 1;
        g_drag = 2;
        SetCapture(g_hFly);
        Commit();
        return;
    }
    int k = KnobAt(x, y);
    if (k >= 0) {
        if (k == 1 && g_cfg.param[g_sel].type >= T_LP)
            return;
        g_drag = 3;
        g_dragCol = k;
        g_dragY0 = y;
        g_dragT0 = KnobT(k);
        SetCapture(g_hFly);
        return;
    }
    for (int i = 0; i < MAXP; i++) {
        float bx = PX + i * 45.f;
        Band& b = g_cfg.param[i];
        if (In(x, y, bx + 1.5f, CELLY, 42, 28)) {
            if (i == g_sel) {
                b.type = (b.type + 1) % 5;
                if (b.type >= T_LP && b.q < 0.3f)
                    b.q = 0.707f;
            } else
                g_sel = i;
            Commit();
            return;
        }
        if (In(x, y, bx, CELLY + 30, 44, 22)) {
            g_sel = i;
            b.on ^= 1;
            Commit();
            return;
        }
    }
}

static LRESULT CALLBACK FlyProc(HWND h, UINT m, WPARAM w, LPARAM l) {
    float x = GET_X_LPARAM(l) / g_s, y = GET_Y_LPARAM(l) / g_s;
    Layout();
    switch (m) {
        case WM_ERASEBKGND:
            return 1;
        case WM_PAINT:
            Paint();
            return 0;
        case WM_SIZE:
            if (g_rt)
                g_rt->Resize(D2D1::SizeU(LOWORD(l), HIWORD(l)));
            return 0;
        case WM_ACTIVATE:
            if (LOWORD(w) == WA_INACTIVE && !g_modal)
                HideFlyout();
            return 0;
        case WM_TIMER:
            if (w == 3) {
                DWORD n = GetTickCount();
                float dt = (n - g_dimT) / 1000.f,
                      tg = g_cfg.eqEnabled ? 0.f : 1.f;
                g_dimT = n;
                if (g_dim < tg) {
                    g_dim += dt / 0.18f;
                    if (g_dim >= tg)
                        g_dim = tg;
                } else if (g_dim > tg) {
                    g_dim -= dt / 0.18f;
                    if (g_dim <= tg)
                        g_dim = tg;
                }
                if (g_dim == tg)
                    KillTimer(h, 3);
                InvalidateRect(h, nullptr, FALSE);
                return 0;
            }
            if (w == 4) {
                SpecUpdate();
                InvalidateRect(h, nullptr, FALSE);
                return 0;
            }
            if (w == 2) {
                DWORD n = GetTickCount();
                float dt = (n - g_animT) / 1000.f;
                g_animT = n;
                g_p += (g_out ? -dt : dt) / 0.2f;
                if (g_p >= 1) {
                    g_p = 1;
                    KillTimer(h, 2);
                }
                if (g_p <= 0) {
                    g_p = 0;
                    KillTimer(h, 2);
                    KillTimer(h, 4);
                    SpecRequestStop();
                    ShowWindow(h, SW_HIDE);
                    SetWindowRgn(h, nullptr, FALSE);
                    return 0;
                }
                AnimApply();
            }
            return 0;
        case WM_KEYDOWN:
            if (w == VK_ESCAPE) {
                if (g_naming) {
                    g_naming = false;
                    InvalidateRect(h, nullptr, FALSE);
                } else
                    HideFlyout();
            }
            return 0;
        case WM_CHAR:
            if (g_naming) {
                if (w == 13) {
                    if (g_nameLen)
                        SavePresetAs(g_nameBuf);
                    g_naming = false;
                } else if (w == 8) {
                    if (g_nameLen)
                        g_nameBuf[--g_nameLen] = 0;
                } else if (w >= 32 && g_nameLen < 31) {
                    g_nameBuf[g_nameLen++] = (wchar_t)w;
                    g_nameBuf[g_nameLen] = 0;
                }
                InvalidateRect(h, nullptr, FALSE);
            }
            return 0;
        case WM_LBUTTONDOWN:
            OnDown(x, y);
            return 0;
        case WM_LBUTTONDBLCLK: {
            if (!g_cfg.eqEnabled) {
                OnDown(x, y);
                return 0;
            }
            int c = SliderAt(x, y), n = NodeAt(x, y), k = KnobAt(x, y);
            if (c >= 0) {
                (c == 10 ? g_cfg.preamp : g_cfg.graphic[c]) = 0;
                Commit();
            } else if (n >= 0) {
                g_cfg.param[n].gain = 0;
                Commit();
            } else if (k == 1 || k == 2 || k == 3)
                KnobSet(k, k == 2 ? log(0.71 / 0.1) / log(200.0) : 0.5);
            else
                OnDown(x, y);
            return 0;
        }
        case WM_MOUSEMOVE:
            if (g_drag == 1)
                SetSlider(g_dragCol, y);
            else if (g_drag == 2)
                DragNode(x, y);
            else if (g_drag == 3)
                KnobSet(g_dragCol, g_dragT0 - (y - g_dragY0) / 150.0);
            return 0;
        case WM_LBUTTONUP:
            if (g_drag) {
                g_drag = 0;
                ReleaseCapture();
            }
            return 0;
        case WM_RBUTTONDOWN: {
            if (!g_cfg.eqEnabled)
                return 0;
            int n = NodeAt(x, y);
            if (n >= 0) {
                g_sel = n;
                g_cfg.param[n].on ^= 1;
                Commit();
            }
            return 0;
        }
        case WM_MOUSEWHEEL: {
            POINT p = {GET_X_LPARAM(l), GET_Y_LPARAM(l)};
            ScreenToClient(h, &p);
            x = p.x / g_s;
            y = p.y / g_s;
            if (!g_cfg.eqEnabled)
                return 0;
            float d = GET_WHEEL_DELTA_WPARAM(w) / 120.f;
            int c = SliderAt(x, y), n = NodeAt(x, y), k = KnobAt(x, y);
            if (c >= 0) {
                float& v = c == 10 ? g_cfg.preamp : g_cfg.graphic[c];
                v = Cf(v + d * 0.5f, -12, 12);
                Commit();
            } else if (n >= 0) {
                g_sel = n;
                Band& b = g_cfg.param[n];
                b.q = Cf(b.q * powf(1.12f, d), 0.1f, 20);
                b.on = 1;
                Commit();
            } else if (k >= 0 && !(k == 1 && g_cfg.param[g_sel].type >= T_LP))
                KnobSet(k, KnobT(k) + d * 0.02);
            return 0;
        }
    }
    return DefWindowProcW(h, m, w, l);
}

static LRESULT CALLBACK TrayProc(HWND h, UINT m, WPARAM w, LPARAM l) {
    if (m == g_msgTaskbar) {
        TrayIcon(NIM_ADD);
        return 0;
    }
    switch (m) {
        case WM_TRAY:
            if (LOWORD(l) == WM_LBUTTONUP) {
                if (GetTickCount() - g_lastDeact > 250)
                    ShowFlyout();
            } else if (LOWORD(l) == WM_RBUTTONUP)
                ShowMenu();
            else if (LOWORD(l) == WM_MBUTTONUP) {
                g_cfg.eqEnabled ^= 1;
                Commit();
            }
            return 0;
        case WM_PICK:
            PickFile();
            return 0;
        case WM_SETCH: {
            PCWSTR f = Wh_GetStringSetting(L"curveFile");
            std::wstring s = f ? f : L"";
            Wh_FreeStringSetting(f);
            if (!s.empty() && s != g_lastSettingFile)
                SetCurve(s.c_str());
            g_lastSettingFile = s;
            if (g_hFly) {
                ApplyDwm();
                InvalidateRect(g_hFly, nullptr, FALSE);
            }
            return 0;
        }
        case WM_TIMER:
            KillTimer(h, 1);
            Wh_SetBinaryValue(L"cfg", &g_cfg, sizeof g_cfg);
            return 0;
        case WM_SETTINGCHANGE:
            LoadTheme();
            TrayIcon(NIM_MODIFY);
            if (g_hFly) {
                ApplyDwm();
                InvalidateRect(g_hFly, nullptr, FALSE);
            }
            return 0;
        case WM_CLOSE:
            DestroyWindow(h);
            return 0;
        case WM_DESTROY: {
            NOTIFYICONDATAW nd = {sizeof nd};
            nd.hWnd = h;
            nd.uID = 1;
            Shell_NotifyIconW(NIM_DELETE, &nd);
            if (g_hFly)
                DestroyWindow(g_hFly);
            PostQuitMessage(0);
            return 0;
        }
    }
    return DefWindowProcW(h, m, w, l);
}

static IDWriteTextFormat* MakeTF(
    float size,
    DWRITE_FONT_WEIGHT wt,
    DWRITE_TEXT_ALIGNMENT al,
    const wchar_t* fam = L"Segoe UI Variable Text") {
    IDWriteTextFormat* f = nullptr;
    g_dw->CreateTextFormat(fam, nullptr, wt, DWRITE_FONT_STYLE_NORMAL,
                           DWRITE_FONT_STRETCH_NORMAL, size, L"en-us", &f);
    if (f) {
        f->SetTextAlignment(al);
        f->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
        f->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
    }
    return f;
}

static DWORD WINAPI TrayThread(LPVOID) {
    g_trayMutex = CreateMutexW(nullptr, TRUE, L"Local\\WindhawkTrayEQ_Tray");
    if (!g_trayMutex || GetLastError() == ERROR_ALREADY_EXISTS) {
        if (g_trayMutex)
            CloseHandle(g_trayMutex);
        g_trayMutex = nullptr;
        return 0;
    }
    CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &g_d2d);
    DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory),
                        (IUnknown**)&g_dw);
    if (!g_d2d || !g_dw) {
        Wh_Log(L"D2D/DWrite init failed");
        return 0;
    }
    g_tfL =
        MakeTF(13, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_TEXT_ALIGNMENT_LEADING);
    g_tfC = MakeTF(13, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_TEXT_ALIGNMENT_CENTER);
    g_tfS = MakeTF(11, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_TEXT_ALIGNMENT_CENTER);
    g_tfT =
        MakeTF(20, DWRITE_FONT_WEIGHT_SEMI_BOLD, DWRITE_TEXT_ALIGNMENT_LEADING);
    g_tfI = MakeTF(15, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_TEXT_ALIGNMENT_CENTER,
                   L"Segoe Fluent Icons");

    Defaults();
    LoadPresets();
    Shared tmp;
    bool stored = false;
    if (Wh_GetBinaryValue(L"cfg", &tmp, sizeof tmp) == sizeof tmp &&
        tmp.magic == kMagic) {
        g_cfg = tmp;
        Sanitize(g_cfg);
        stored = true;
    }
    g_cfg.version = 0;
    wchar_t nm[MAX_PATH] = L"";
    Wh_GetStringValue(L"curveName", nm, MAX_PATH);
    g_curveName = nm;
    PCWSTR sf = Wh_GetStringSetting(L"curveFile");
    g_lastSettingFile = sf ? sf : L"";
    Wh_FreeStringSetting(sf);
    if (!stored && !g_lastSettingFile.empty())
        LoadCurve(
            g_lastSettingFile.c_str());  // first run only: the curve lives in
                                         // the preset/state afterwards
    if (g_sh && (g_sh->version & 1))
        InterlockedIncrement(&g_sh->version);
    LoadTheme();

    HINSTANCE hi = GetModuleHandleW(nullptr);
    WNDCLASSW wc = {};
    wc.hInstance = hi;
    wc.lpfnWndProc = TrayProc;
    wc.lpszClassName = L"WhTrayEqHost";
    RegisterClassW(&wc);
    WNDCLASSW fc = {};
    fc.hInstance = hi;
    fc.lpfnWndProc = FlyProc;
    fc.lpszClassName = L"WhTrayEqFlyout";
    fc.style = CS_DBLCLKS | CS_DROPSHADOW;
    fc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    RegisterClassW(&fc);
    g_hTray =
        CreateWindowExW(WS_EX_TOOLWINDOW, wc.lpszClassName, L"TrayEQ", WS_POPUP,
                        0, 0, 0, 0, nullptr, nullptr, hi, nullptr);
    g_hFly = CreateWindowExW(WS_EX_TOOLWINDOW | WS_EX_TOPMOST, fc.lpszClassName,
                             L"TrayEQFlyout", WS_POPUP, 0, 0, 100, 100, nullptr,
                             nullptr, hi, nullptr);
    g_msgTaskbar = RegisterWindowMessageW(L"TaskbarCreated");
    ApplyDwm();
    Publish();
    TrayIcon(NIM_ADD);

    MSG msg;
    while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    Wh_SetBinaryValue(L"cfg", &g_cfg, sizeof g_cfg);
    SpecStop();
    ReleaseRT();
    if (g_icon)
        DestroyIcon(g_icon);
    g_hTray = g_hFly = nullptr;
    UnregisterClassW(L"WhTrayEqHost", hi);
    UnregisterClassW(L"WhTrayEqFlyout", hi);
    for (IDWriteTextFormat* f : {g_tfL, g_tfC, g_tfS, g_tfT, g_tfI})
        if (f)
            f->Release();
    g_dw->Release();
    g_d2d->Release();
    CoUninitialize();
    ReleaseMutex(g_trayMutex);
    CloseHandle(g_trayMutex);
    g_trayMutex = nullptr;
    return 0;
}

// ---------------------------------------------------------------- mod entry
// points
static void LoadSettings() {
    PCWSTR h = Wh_GetStringSetting(L"trayHost");
    g_trayHost = h && *h ? h : L"explorer.exe";
    Wh_FreeStringSetting(h);
    PCWSTR b = Wh_GetStringSetting(L"backdrop");
    g_backdrop = b && !wcscmp(b, L"mica");
    Wh_FreeStringSetting(b);
}

static bool IsTrayHost() {
    wchar_t p[MAX_PATH];
    GetModuleFileNameW(nullptr, p, MAX_PATH);
    const wchar_t* n = wcsrchr(p, L'\\');
    n = n ? n + 1 : p;
    return _wcsicmp(n, g_trayHost.c_str()) == 0;
}

BOOL Wh_ModInit() {
    LoadSettings();
    bool tray = IsTrayHost();
    if (!OpenShared(tray))
        Wh_Log(L"shared memory failed");

    // Only processes that use audio pay for the symbol lookup.
    if (HMODULE m = GetModuleHandleW(L"AudioSes.dll")) {
        g_audioHooked = true;
        if (!HookAudioSes(m))
            Wh_Log(L"AudioSes.dll symbols not resolved");
    } else if (HMODULE kb = GetModuleHandleW(L"kernelbase.dll")) {
        void* ll = (void*)GetProcAddress(kb, "LoadLibraryExW");
        if (ll)
            Wh_SetFunctionHook(ll, (void*)LoadLibraryExW_h,
                               (void**)&LoadLibraryExW_o);
    }
    if (tray)
        g_trayThread =
            CreateThread(nullptr, 0, TrayThread, nullptr, 0, &g_trayTid);
    return TRUE;
}

void Wh_ModAfterInit() {
    AcquireSRWLockExclusive(&g_hookLock);
    g_hookAllowed = true;
    ReleaseSRWLockExclusive(&g_hookLock);
    HookAudioSesIfLoaded(true);  // AudioSes.dll may have loaded during init
}

void Wh_ModBeforeUninit() {
    AcquireSRWLockExclusive(&g_hookLock);
    g_hookAllowed = false;
    ReleaseSRWLockExclusive(&g_hookLock);
}

BOOL Wh_ModSettingsChanged(BOOL* bReload) {
    std::wstring oldHost = g_trayHost;
    LoadSettings();
    if (_wcsicmp(oldHost.c_str(), g_trayHost.c_str()) != 0) {
        *bReload = TRUE;  // the tray host is decided at load time
        return TRUE;
    }
    if (g_hTray)
        PostMessageW(g_hTray, WM_SETCH, 0, 0);
    return TRUE;
}

static BOOL CALLBACK CloseThreadWnd(HWND h, LPARAM) {
    PostMessageW(h, WM_CLOSE, 0, 0);
    return TRUE;
}

void Wh_ModUninit() {
    if (g_trayThread) {
        // No mod code may run after this returns: keep asking (the window may
        // not exist yet, or a modal dialog may be open) until the thread ends.
        while (WaitForSingleObject(g_trayThread, 100) == WAIT_TIMEOUT)
            EnumThreadWindows(g_trayTid, CloseThreadWnd, 0);
        CloseHandle(g_trayThread);
        g_trayThread = nullptr;
    }
    SpecStop();
    AcquireSRWLockExclusive(&g_lk);
    g_streams.clear();
    g_clients.clear();
    ReleaseSRWLockExclusive(&g_lk);
    if (g_sh)
        UnmapViewOfFile(g_sh);
    if (g_map)
        CloseHandle(g_map);
}
