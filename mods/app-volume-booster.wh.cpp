// ==WindhawkMod==
// @id              app-volume-booster
// @name            App Volume Booster
// @description     Boost the volume of chosen apps beyond 100%, with optional compressor mode and per-app overrides
// @version         1.0
// @author          shubho
// @github          https://github.com/shubho193
// @compilerOptions -lole32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# App Volume Booster
Changes the loudness of the audio an app sends to Windows, so it can go past the
100% limit of the volume mixer.

This mod doesn't target any process by default.

**Which apps:** open this mod's *Advanced* tab -> *Custom process inclusion list* and
list the exe names you want affected (one per line). Only those processes get the mod.
Restart an app after adding it to the list.

*Note for games:* Games are excluded by default in Windhawk. To work with games, they need
to be specifically added to the inclusion list from Windhawk's Advanced settings:
*Settings > Advanced settings > More advanced settings > Process Inclusion List*.

**How loud:** every included app uses the default settings. To give one app its own
settings, click **+** under *Per-app overrides*, type its exe name and choose its volume
and mode. An override only applies to apps that are also in the inclusion list.
Overrides with an empty process name are ignored.

**Modes**
- *Gain* (default): multiplies the audio by the volume %. 100 = unchanged, 200 = twice as
  loud. Anything pushed past full scale is clipped.
- *Compressor*: turns down the loud parts, then applies the volume % as makeup gain, so
  quiet material gets louder with less distortion on loud peaks. Tune it with threshold,
  ratio, attack and release (these settings are shared by all apps).

Changes apply live. Lower your system volume before starting, boosted audio can be very loud.
Exclusive-mode streams, spatial audio streams (e.g. Windows Sonic / Dolby Atmos, which bypass
IAudioRenderClient), and apps that bypass WASAPI entirely may not be affected.

**Troubleshooting:** start the app *after* enabling the mod (streams created earlier are not seen), then check Windhawk's log for this mod. It reports each stream's format and whether audio is being processed.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- defaultGain: 200
  $name: Default volume (%)
  $description: Applies to every included app without an override (range 10 to 1000). 100 = unchanged, 200 = twice as loud. In compressor mode this is the makeup gain.
- defaultCompressor: false
  $name: Compressor mode (default)
  $description: Compress loud peaks before applying the volume %.
- thresholdDb: -25
  $name: Compressor threshold (dBFS)
  $description: Level above which compression starts. Range -60 to 0.
- ratio: 2
  $name: Compressor ratio (x:1)
  $description: How strongly levels above the threshold are reduced. Range 1 to 20.
- attackMs: 10
  $name: Compressor attack (ms)
  $description: How fast the compressor reacts to loud sounds. Range 1 to 500.
- releaseMs: 150
  $name: Compressor release (ms)
  $description: How fast it lets go afterwards. Range 10 to 2000.
- slots:
  - - process: ""
      $name: Process name
      $description: Exe name of the app, e.g. app.exe. Empty entries are ignored.
    - gain: 200
      $name: Volume (%)
      $description: Range 10 to 1000.
    - compressor: false
      $name: Compressor mode
  $name: Per-app overrides
  $description: Click + to add another app.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <mmdeviceapi.h>
#include <audioclient.h>
#include <mmreg.h>
#include <ksmedia.h>
#include <windhawk_utils.h>

#include <atomic>
#include <cmath>
#include <memory>
#include <mutex>
#include <unordered_map>

// MinGW's ksmedia.h declares these subformats as extern without defining them,
// which would otherwise require linking libksguid.a. Defining them here with
// DECLSPEC_SELECTANY allows clean linking with just -lole32.
const CDECL GUID DECLSPEC_SELECTANY KSDATAFORMAT_SUBTYPE_PCM = {
    0x00000001, 0x0000, 0x0010, {0x80, 0x00, 0x00, 0xAA, 0x00, 0x38, 0x9B, 0x71}
};
const CDECL GUID DECLSPEC_SELECTANY KSDATAFORMAT_SUBTYPE_IEEE_FLOAT = {
    0x00000003, 0x0000, 0x0010, {0x80, 0x00, 0x00, 0xAA, 0x00, 0x38, 0x9B, 0x71}
};

static const int kMaxSlots = 64;
static const UINT32 kMaxChannels = 32;


// State

enum class SampleKind { Unsupported, Float32, Int16, Int24, Int32 };

struct FmtInfo {
    SampleKind kind = SampleKind::Unsupported;
    UINT32 blockAlign = 0;
    UINT32 channels = 0;
    UINT32 sampleRate = 48000;
    UINT32 bytesPerSample = 0;
};

struct RenderInfo {
    FmtInfo fmt;
    BYTE* lastBuffer = nullptr;
    UINT32 lastBufferFrames = 0;
    float env = 0.0f;  // compressor envelope, persists between buffers
    bool loggedFirst = false;
};

static std::atomic<bool> g_loggedUnknownClient{false};

// Live settings for this process
static std::atomic<float> g_gain{1.0f};
static std::atomic<bool> g_compressor{false};
static std::atomic<float> g_thresholdDb{-18.0f};
static std::atomic<float> g_ratio{4.0f};
static std::atomic<float> g_attackMs{10.0f};
static std::atomic<float> g_releaseMs{150.0f};

static std::mutex g_mutex;
static std::unordered_map<void*, FmtInfo> g_clientFormats;                      // IAudioClient*       -> format
static std::unordered_map<void*, std::shared_ptr<RenderInfo>> g_renderClients; // IAudioRenderClient* -> state


// Settings

static bool GetOwnExeName(WCHAR* out, DWORD cap) {
    WCHAR path[MAX_PATH];
    DWORD n = GetModuleFileNameW(nullptr, path, MAX_PATH);
    if (n == 0 || n >= MAX_PATH) return false;
    const WCHAR* name = wcsrchr(path, L'\\');
    name = name ? name + 1 : path;
    wcsncpy(out, name, cap - 1);
    out[cap - 1] = 0;
    return true;
}

static int Clamp(int v, int lo, int hi) {
    return v < lo ? lo : (v > hi ? hi : v);
}

// A matching override wins, otherwise the defaults are used.
static void LoadSettings() {
    int pct = Clamp(Wh_GetIntSetting(L"defaultGain"), 10, 1000);
    bool comp = Wh_GetIntSetting(L"defaultCompressor") != 0;

    WCHAR exe[MAX_PATH];
    if (GetOwnExeName(exe, MAX_PATH)) {
        for (int i = 0; i < kMaxSlots; i++) {
            auto name = WindhawkUtils::StringSetting::make(L"slots[%d].process", i);
            PCWSTR psz = name.get();
            if (!psz || !psz[0] || _wcsicmp(psz, exe) != 0) continue;

            pct = Clamp(Wh_GetIntSetting(L"slots[%d].gain", i), 10, 1000);
            comp = Wh_GetIntSetting(L"slots[%d].compressor", i) != 0;
            break;
        }
    }

    g_gain = pct / 100.0f;
    g_compressor = comp;
    g_thresholdDb = (float)Clamp(Wh_GetIntSetting(L"thresholdDb"), -60, 0);
    g_ratio = (float)Clamp(Wh_GetIntSetting(L"ratio"), 1, 20);
    g_attackMs = (float)Clamp(Wh_GetIntSetting(L"attackMs"), 1, 500);
    g_releaseMs = (float)Clamp(Wh_GetIntSetting(L"releaseMs"), 10, 2000);
}


// DSP

static inline float Clip(float x) {
    if (x > 1.0f) return 1.0f;
    if (x < -1.0f) return -1.0f;
    return x;
}

static inline float LoadSample(const BYTE* p, SampleKind k) {
    switch (k) {
    case SampleKind::Float32:
        return *(const float*)p;
    case SampleKind::Int16:
        return *(const int16_t*)p / 32768.0f;
    case SampleKind::Int24: {
        int32_t v = (int32_t)((p[0] << 8) | (p[1] << 16) | ((uint32_t)p[2] << 24)) >> 8;
        return v / 8388608.0f;
    }
    case SampleKind::Int32:
        return (float)((double)*(const int32_t*)p / 2147483648.0);
    default:
        return 0.0f;
    }
}

static inline void StoreSample(BYTE* p, SampleKind k, float x) {
    switch (k) {
    case SampleKind::Float32:
        *(float*)p = x;
        break;
    case SampleKind::Int16:
        *(int16_t*)p = (int16_t)lrintf(x * 32767.0f);
        break;
    case SampleKind::Int24: {
        int32_t o = (int32_t)lrintf(x * 8388607.0f);
        p[0] = (BYTE)(o & 0xFF);
        p[1] = (BYTE)((o >> 8) & 0xFF);
        p[2] = (BYTE)((o >> 16) & 0xFF);
        break;
    }
    case SampleKind::Int32:
        *(int32_t*)p = (int32_t)((double)x * 2147483647.0);
        break;
    default:
        break;
    }
}

// Applies gain (and optionally compression) to one buffer of interleaved samples.
static void ProcessBuffer(BYTE* data, UINT32 frames, RenderInfo& ri) {
    const FmtInfo& f = ri.fmt;
    const UINT32 ch = f.channels;
    const UINT32 bps = f.bytesPerSample;
    if (ch == 0 || ch > kMaxChannels || bps == 0 || f.blockAlign < ch * bps) return;

    const bool comp = g_compressor.load();
    const float makeup = g_gain.load();

    // Compressor parameters
    const float thresholdDb = g_thresholdDb.load();
    const float slope = 1.0f - 1.0f / g_ratio.load();
    const float sr = (float)f.sampleRate;
    const float atkCoef = expf(-1.0f / (sr * g_attackMs.load() * 0.001f));
    const float relCoef = expf(-1.0f / (sr * g_releaseMs.load() * 0.001f));
    float env = ri.env;

    float s[kMaxChannels];
    BYTE* p = data;

    for (UINT32 n = 0; n < frames; n++, p += f.blockAlign) {
        float level = 0.0f;
        for (UINT32 c = 0; c < ch; c++) {
            s[c] = LoadSample(p + c * bps, f.kind);
            float a = fabsf(s[c]);
            if (a > level) level = a;
        }

        float g = makeup;
        if (comp) {
            // Peak envelope follower, linked across channels
            float coef = level > env ? atkCoef : relCoef;
            env = coef * env + (1.0f - coef) * level;

            if (env > 1e-6f) {
                float over = 20.0f * log10f(env) - thresholdDb;
                if (over > 0.0f) g *= powf(10.0f, -over * slope / 20.0f);
            }
        }

        for (UINT32 c = 0; c < ch; c++) {
            StoreSample(p + c * bps, f.kind, Clip(s[c] * g));
        }
    }

    ri.env = env;
}

static FmtInfo ParseFormat(const WAVEFORMATEX* wf) {
    FmtInfo f;
    if (!wf) return f;

    f.blockAlign = wf->nBlockAlign;
    f.channels = wf->nChannels;
    f.sampleRate = wf->nSamplesPerSec ? wf->nSamplesPerSec : 48000;

    bool isPcm = false;
    bool isFloat = false;

    if (wf->wFormatTag == WAVE_FORMAT_EXTENSIBLE && wf->cbSize >= 22) {
        const auto* ext = reinterpret_cast<const WAVEFORMATEXTENSIBLE*>(wf);
        if (IsEqualGUID(ext->SubFormat, KSDATAFORMAT_SUBTYPE_PCM)) {
            isPcm = true;
        } else if (IsEqualGUID(ext->SubFormat, KSDATAFORMAT_SUBTYPE_IEEE_FLOAT)) {
            isFloat = true;
        }
    } else if (wf->wFormatTag == WAVE_FORMAT_IEEE_FLOAT) {
        isFloat = true;
    } else if (wf->wFormatTag == WAVE_FORMAT_PCM) {
        isPcm = true;
    }

    if (isFloat && wf->wBitsPerSample == 32) {
        f.kind = SampleKind::Float32;
        f.bytesPerSample = 4;
    } else if (isPcm) {
        switch (wf->wBitsPerSample) {
        case 16: f.kind = SampleKind::Int16; f.bytesPerSample = 2; break;
        case 24: f.kind = SampleKind::Int24; f.bytesPerSample = 3; break;
        case 32: f.kind = SampleKind::Int32; f.bytesPerSample = 4; break;
        }
    }

    return f;
}


// Hooks

using Release_t = ULONG(STDMETHODCALLTYPE*)(IUnknown*);
using Initialize_t = HRESULT(STDMETHODCALLTYPE*)(IAudioClient*, AUDCLNT_SHAREMODE, DWORD,
                                                 REFERENCE_TIME, REFERENCE_TIME,
                                                 const WAVEFORMATEX*, LPCGUID);
using GetService_t = HRESULT(STDMETHODCALLTYPE*)(IAudioClient*, REFIID, void**);
using GetBuffer_t = HRESULT(STDMETHODCALLTYPE*)(IAudioRenderClient*, UINT32, BYTE**);
using ReleaseBuffer_t = HRESULT(STDMETHODCALLTYPE*)(IAudioRenderClient*, UINT32, DWORD);

// IAudioClient3::InitializeSharedAudioStream (low-latency path, used by many games/engines)
using InitShared_t = HRESULT(STDMETHODCALLTYPE*)(IAudioClient*, DWORD, UINT32, const WAVEFORMATEX*, LPCGUID);

static Release_t Orig_ClientRelease;
static Release_t Orig_RenderRelease;
static Initialize_t Orig_Initialize;
static InitShared_t Orig_InitShared;
static GetService_t Orig_GetService;
static GetBuffer_t Orig_GetBuffer;
static ReleaseBuffer_t Orig_ReleaseBuffer;

static ULONG STDMETHODCALLTYPE Hook_ClientRelease(IUnknown* self) {
    ULONG ref = Orig_ClientRelease(self);
    if (ref == 0) {
        std::lock_guard<std::mutex> lock(g_mutex);
        g_clientFormats.erase(self);
        g_renderClients.erase(self);
    }
    return ref;
}

static ULONG STDMETHODCALLTYPE Hook_RenderRelease(IUnknown* self) {
    ULONG ref = Orig_RenderRelease(self);
    if (ref == 0) {
        std::lock_guard<std::mutex> lock(g_mutex);
        g_clientFormats.erase(self);
        g_renderClients.erase(self);
    }
    return ref;
}

static void RememberClientFormat(const wchar_t* via, IAudioClient* self, const WAVEFORMATEX* fmt) {
    if (!fmt) return;
    FmtInfo info = ParseFormat(fmt);
    Wh_Log(L"%s: tag=%u ch=%u bits=%u rate=%u -> %s", via, (unsigned)fmt->wFormatTag,
           (unsigned)fmt->nChannels, (unsigned)fmt->wBitsPerSample, (unsigned)fmt->nSamplesPerSec,
           info.kind == SampleKind::Unsupported ? L"UNSUPPORTED format" : L"supported");
    std::lock_guard<std::mutex> lock(g_mutex);
    g_clientFormats[self] = info;
}

static HRESULT STDMETHODCALLTYPE Hook_Initialize(IAudioClient* self, AUDCLNT_SHAREMODE mode, DWORD flags,
                                                 REFERENCE_TIME duration, REFERENCE_TIME period,
                                                 const WAVEFORMATEX* fmt, LPCGUID session) {
    HRESULT hr = Orig_Initialize(self, mode, flags, duration, period, fmt, session);
    if (SUCCEEDED(hr)) {
        RememberClientFormat(L"IAudioClient::Initialize", self, fmt);
    } else {
        std::lock_guard<std::mutex> lock(g_mutex);
        g_clientFormats.erase(self);
    }
    return hr;
}

static HRESULT STDMETHODCALLTYPE Hook_InitShared(IAudioClient* self, DWORD flags, UINT32 periodFrames,
                                                 const WAVEFORMATEX* fmt, LPCGUID session) {
    HRESULT hr = Orig_InitShared(self, flags, periodFrames, fmt, session);
    if (SUCCEEDED(hr)) {
        RememberClientFormat(L"IAudioClient3::InitializeSharedAudioStream", self, fmt);
    } else {
        std::lock_guard<std::mutex> lock(g_mutex);
        g_clientFormats.erase(self);
    }
    return hr;
}

static HRESULT STDMETHODCALLTYPE Hook_GetService(IAudioClient* self, REFIID riid, void** ppv) {
    HRESULT hr = Orig_GetService(self, riid, ppv);
    if (SUCCEEDED(hr) && ppv && *ppv && IsEqualIID(riid, __uuidof(IAudioRenderClient))) {
        std::lock_guard<std::mutex> lock(g_mutex);
        auto it = g_clientFormats.find(self);
        if (it != g_clientFormats.end()) {
            auto ri = std::make_shared<RenderInfo>();
            ri->fmt = it->second;
            g_renderClients[*ppv] = ri;
            Wh_Log(L"Render client attached");
        } else {
            g_renderClients.erase(*ppv);
            Wh_Log(L"Render client created but its format is unknown (stream was initialized before the mod loaded?)");
        }
    }
    return hr;
}

static HRESULT STDMETHODCALLTYPE Hook_GetBuffer(IAudioRenderClient* self, UINT32 frames, BYTE** ppData) {
    HRESULT hr = Orig_GetBuffer(self, frames, ppData);
    if (SUCCEEDED(hr) && ppData) {
        std::lock_guard<std::mutex> lock(g_mutex);
        auto it = g_renderClients.find(self);
        if (it != g_renderClients.end()) {
            it->second->lastBuffer = *ppData;
            it->second->lastBufferFrames = frames;
        }
    }
    return hr;
}

static HRESULT STDMETHODCALLTYPE Hook_ReleaseBuffer(IAudioRenderClient* self, UINT32 framesWritten, DWORD flags) {
    bool active = g_compressor.load() || g_gain.load() != 1.0f;
    if (active && framesWritten > 0 && !(flags & AUDCLNT_BUFFERFLAGS_SILENT)) {
        std::shared_ptr<RenderInfo> ri;
        {
            std::lock_guard<std::mutex> lock(g_mutex);
            auto it = g_renderClients.find(self);
            if (it != g_renderClients.end()) ri = it->second;
        }
        if (!ri) {
            if (!g_loggedUnknownClient.exchange(true)) {
                Wh_Log(L"Audio is being written through a render client the mod doesn't know about, not processing it");
            }
        } else if (ri->lastBuffer && ri->fmt.kind != SampleKind::Unsupported) {
            BYTE* buf = ri->lastBuffer;
            UINT32 maxFrames = ri->lastBufferFrames;
            ri->lastBuffer = nullptr;
            ri->lastBufferFrames = 0;
            UINT32 framesToProcess = framesWritten < maxFrames ? framesWritten : maxFrames;
            if (framesToProcess > 0) {
                if (!ri->loggedFirst) {
                    ri->loggedFirst = true;
                    Wh_Log(L"Processing audio: gain %.2fx, compressor %d", g_gain.load(), (int)g_compressor.load());
                }
                ProcessBuffer(buf, framesToProcess, *ri);
            }
        }
    }
    return Orig_ReleaseBuffer(self, framesWritten, flags);
}


// Locate the COM method addresses by creating a throwaway audio client

static bool InstallHooks() {
    HRESULT hrInit = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    bool needUninit = SUCCEEDED(hrInit);

    IMMDeviceEnumerator* en = nullptr;
    IMMDevice* dev = nullptr;
    IAudioClient* ac = nullptr;
    IAudioRenderClient* rc = nullptr;
    WAVEFORMATEX* mix = nullptr;
    bool ok = false;

    do {
        if (FAILED(CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL,
                                    __uuidof(IMMDeviceEnumerator), (void**)&en))) break;
        if (FAILED(en->GetDefaultAudioEndpoint(eRender, eConsole, &dev))) break;
        if (FAILED(dev->Activate(__uuidof(IAudioClient), CLSCTX_ALL, nullptr, (void**)&ac))) break;
        if (FAILED(ac->GetMixFormat(&mix))) break;
        if (FAILED(ac->Initialize(AUDCLNT_SHAREMODE_SHARED, 0, 10000000, 0, mix, nullptr))) break;
        if (FAILED(ac->GetService(__uuidof(IAudioRenderClient), (void**)&rc))) break;

        void** acVtbl = *(void***)ac;
        void** rcVtbl = *(void***)rc;

        // IAudioClient:       2 = Release, 3 = Initialize, 14 = GetService
        // IAudioRenderClient: 2 = Release, 3 = GetBuffer,   4 = ReleaseBuffer
        ok = WindhawkUtils::SetFunctionHook((Release_t)acVtbl[2], Hook_ClientRelease, &Orig_ClientRelease) &&
             WindhawkUtils::SetFunctionHook((Initialize_t)acVtbl[3], Hook_Initialize, &Orig_Initialize) &&
             WindhawkUtils::SetFunctionHook((GetService_t)acVtbl[14], Hook_GetService, &Orig_GetService) &&
             WindhawkUtils::SetFunctionHook((GetBuffer_t)rcVtbl[3], Hook_GetBuffer, &Orig_GetBuffer) &&
             WindhawkUtils::SetFunctionHook((ReleaseBuffer_t)rcVtbl[4], Hook_ReleaseBuffer, &Orig_ReleaseBuffer);
        if (!ok) break;

        if (rcVtbl[2] != acVtbl[2]) {
            if (!WindhawkUtils::SetFunctionHook((Release_t)rcVtbl[2], Hook_RenderRelease, &Orig_RenderRelease)) {
                Wh_Log(L"Could not hook IAudioRenderClient::Release");
            }
        }

        // IAudioClient2: 15-17
        // IAudioClient3: 18 GetSharedModeEnginePeriod, 19 GetCurrentSharedModeEnginePeriod, 20 InitializeSharedAudioStream.
        // Streams created through InitializeSharedAudioStream never call IAudioClient::Initialize.
        void* ac3 = nullptr;
        if (SUCCEEDED(ac->QueryInterface(__uuidof(IAudioClient3), &ac3)) && ac3) {
            void** ac3Vtbl = *(void***)ac3;
            if (!WindhawkUtils::SetFunctionHook((InitShared_t)ac3Vtbl[20], Hook_InitShared, &Orig_InitShared)) {
                Wh_Log(L"Could not hook InitializeSharedAudioStream");
            }
            ((IUnknown*)ac3)->Release();
        }
    } while (false);

    if (mix) CoTaskMemFree(mix);
    if (rc) rc->Release();
    if (ac) ac->Release();
    if (dev) dev->Release();
    if (en) en->Release();
    if (needUninit) CoUninitialize();

    return ok;
}


// Windhawk entry points

BOOL Wh_ModInit() {
    LoadSettings();

    if (!InstallHooks()) {
        Wh_Log(L"Failed to hook WASAPI (no output device?)");
        return FALSE;
    }

    Wh_Log(L"App Volume Booster active, gain %.2fx, compressor %d",
           g_gain.load(), (int)g_compressor.load());
    return TRUE;
}

void Wh_ModSettingsChanged() {
    LoadSettings();
}
