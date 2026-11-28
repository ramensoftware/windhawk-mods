// ==WindhawkMod==
// @id monochrome-shell-icons
// @name Monochrome Shell Icons
// @description Adaptive monochrome icons for Windows 11.
// @version 1.0.0
// @author Piot
// @github https://github.com/Piotxd
// @include         explorer.exe
// @include         StartMenuExperienceHost.exe
// @include         SearchHost.exe
// @include         SearchApp.exe
// @include         ShellExperienceHost.exe
// @include         ShellHost.exe
// @architecture    x86-64
// @compilerOptions -lole32 -lwindowscodecs -luuid -lshell32 -lgdi32 -luser32
// ==/WindhawkMod==

// ==WindhawkModSettings==
/*
- MinimumBrightness: 65
  $name: Dark detail brightness (0-255)
  $description: Lower values give dark internal details more contrast.
- MaximumBrightness: 245
  $name: Highlight brightness (0-255)
- AdaptiveContrast: true
  $name: Adaptive contrast
- AutoPolarity: true
  $name: Automatically keep the dominant icon tone bright
- Invert: true
  $name: Invert manually when automatic polarity is disabled
- EnableShellImageLists: true
  $name: Recolour shell image-list icons (experimental)
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <objbase.h>
#include <wincodec.h>
#include <commctrl.h>
#include <commoncontrols.h>
#include <shellapi.h>
#include <windhawk_api.h>
#include <atomic>
#include <cwchar>
#include <cstring>

namespace {

std::atomic<int> g_minimumBrightness{65};
std::atomic<int> g_maximumBrightness{245};
std::atomic<bool> g_adaptive{true}, g_autoPolarity{true};
std::atomic<bool> g_invert{true};
std::atomic<bool> g_listsEnabled{true};
std::atomic<unsigned> g_wicHits{0}, g_listHits{0};
thread_local unsigned g_depth = 0;

struct DepthGuard {
    DepthGuard() { ++g_depth; }
    ~DepthGuard() { --g_depth; }
};

template<class T> struct Com {
    T* p = nullptr;
    ~Com() { if (p) p->Release(); }
    Com() = default;
    Com(const Com&) = delete;
    Com& operator=(const Com&) = delete;
    T* operator->() const { return p; }
    T** put() { return &p; }
    T* detach() { T* out = p; p = nullptr; return out; }
};

void ReadSettings() {
    int v = Wh_GetIntSetting(L"MinimumBrightness");
    g_minimumBrightness = v < 0 ? 0 : (v > 255 ? 255 : v);
    v = Wh_GetIntSetting(L"MaximumBrightness");
    g_maximumBrightness = v < 0 ? 0 : (v > 255 ? 255 : v);
    g_adaptive = Wh_GetIntSetting(L"AdaptiveContrast") != 0;
    g_autoPolarity = Wh_GetIntSetting(L"AutoPolarity") != 0;
    g_invert = Wh_GetIntSetting(L"Invert") != 0;
    g_listsEnabled = Wh_GetIntSetting(L"EnableShellImageLists") != 0;
}

// Straight BGRA; use alpha-weighted luminance statistics, excluding very
// faint edge pixels from the statistics but preserving all original alpha.
int Luminance(const BYTE* p) {
    return (2126 * int(p[2]) + 7152 * int(p[1]) +
            722 * int(p[0]) + 5000) / 10000;
}

bool Transform(BYTE* data, UINT w, UINT h, UINT stride) {
    int minimum = g_minimumBrightness.load();
    int maximum = g_maximumBrightness.load();
    if (maximum < minimum) maximum = minimum;
    const bool adaptive = g_adaptive.load();
    const bool autoPolarity = g_autoPolarity.load();
    bool invert = g_invert.load();
    unsigned long long hist[256]{};
    unsigned long long weight = 0;
    // If there are no reasonably opaque pixels, use all nonzero alpha.
    for (int pass = 0; pass < 2 && weight == 0; ++pass) {
        const int minAlpha = pass == 0 ? 64 : 1;
        for (UINT y = 0; y < h; ++y) {
            const BYTE* p = data + size_t(y) * stride;
            for (UINT x = 0; x < w; ++x, p += 4) {
                if (p[3] < minAlpha) continue;
                hist[Luminance(p)] += p[3];
                weight += p[3];
            }
        }
    }
    if (!weight) return false; // Nothing visible: return the original bitmap.
    // Robust 2nd and 98th percentiles reject isolated noisy pixels.
    int low = 0, high = 255;
    unsigned long long sum = 0;
    for (int i = 0; i < 256; ++i) {
        sum += hist[i];
        if (sum > weight / 50) { low = i; break; }
    }
    sum = 0;
    for (int i = 255; i >= 0; --i) {
        sum += hist[i];
        if (sum > weight / 50) { high = i; break; }
    }
    if (high < low) high = low;
    if (autoPolarity) {
        unsigned long long dark = 0, light = 0;
        const int mid = (low + high) / 2;
        for (int i = 0; i < 256; ++i) {
            if (i <= mid) dark += hist[i]; else light += hist[i];
        }
        invert = dark > light;
    }
    const int range = high - low;
    for (UINT y = 0; y < h; ++y) {
        BYTE* p = data + size_t(y) * stride;
        for (UINT x = 0; x < w; ++x, p += 4) {
            if (!p[3]) { p[0] = p[1] = p[2] = 0; continue; }
            int lum = Luminance(p);
            int tone = invert ? 255 - lum : lum;
            if (adaptive) {
                if (range < 12) {
                    // Flat silhouettes should be visible on a dark taskbar.
                    tone = 255;
                } else {
                    int span = range < 48 ? 48 : range;
                    int start = (low + high - span) / 2;
                    int t = (lum - start) * 255 / span;
                    t = t < 0 ? 0 : (t > 255 ? 255 : t);
                    if (invert) t = 255 - t;
                    // Mild S curve: maintain details without hard thresholding.
                    int smooth = (t * t * (765 - 2 * t) + 32512) / 65025;
                    tone = (t + smooth + 1) / 2;
                }
            }
            int gray = minimum + ((maximum - minimum) * tone + 127) / 255;
            p[0] = p[1] = p[2] = static_cast<BYTE>(gray);
        }
    }
    return true;
}

// Make a separate bitmap; do not edit the shell's original bitmap in place.
HRESULT MakeMonoBitmap(IWICImagingFactory* factory, IWICBitmapSource* src,
                       IWICBitmap** out) {
    if (!factory || !src || !out) return E_INVALIDARG;
    *out = nullptr;
    UINT w = 0, h = 0;
    if (FAILED(src->GetSize(&w, &h)) || !w || !h || w > 512 || h > 512)
        return E_FAIL;
    Com<IWICFormatConverter> converter;
    HRESULT hr = factory->CreateFormatConverter(converter.put());
    if (FAILED(hr)) return hr;
    hr = converter->Initialize(src, GUID_WICPixelFormat32bppBGRA,
        WICBitmapDitherTypeNone, nullptr, 0.0, WICBitmapPaletteTypeCustom);
    if (FAILED(hr)) return hr;
    Com<IWICBitmap> result;
    hr = factory->CreateBitmap(w, h, GUID_WICPixelFormat32bppBGRA,
                              WICBitmapCacheOnLoad, result.put());
    if (FAILED(hr)) return hr;
    {
        WICRect rect{0, 0, static_cast<INT>(w), static_cast<INT>(h)};
        Com<IWICBitmapLock> lock;
        hr = result->Lock(&rect, WICBitmapLockWrite, lock.put());
        if (FAILED(hr)) return hr;
        UINT stride = 0, size = 0;
        BYTE* data = nullptr;
        if (FAILED(lock->GetStride(&stride)) ||
            FAILED(lock->GetDataPointer(&size, &data)) || !data ||
            stride < w * 4 || size < size_t(h - 1) * stride + w * 4)
            return E_FAIL;
        hr = converter->CopyPixels(nullptr, stride, size, data);
        if (FAILED(hr)) return hr;
        if (!Transform(data, w, h, stride)) return E_FAIL;
    }
    *out = result.detach();
    return S_OK;
}

using WicIconFn = HRESULT (STDMETHODCALLTYPE*)(IWICImagingFactory*, HICON, IWICBitmap**);
WicIconFn g_wicOriginal[2]{};
void* g_wicAddress[2]{};

template<int N>
HRESULT STDMETHODCALLTYPE WicIconHook(IWICImagingFactory* self, HICON icon,
                                      IWICBitmap** out) {
    if (g_depth) return g_wicOriginal[N](self, icon, out);
    DepthGuard guard;
    HRESULT hr = g_wicOriginal[N](self, icon, out);
    if (FAILED(hr) || !out || !*out || !icon) return hr;
    Com<IWICBitmap> mono;
    if (SUCCEEDED(MakeMonoBitmap(self, *out, mono.put())) && mono.p) {
        // Match the original pixel format when it is the premultiplied variant.
        WICPixelFormatGUID originalFormat{};
        if (FAILED((*out)->GetPixelFormat(&originalFormat))) return hr;
        if (IsEqualGUID(originalFormat, GUID_WICPixelFormat32bppPBGRA)) {
            Com<IWICFormatConverter> conv;
            Com<IWICBitmap> converted;
            if (FAILED(self->CreateFormatConverter(conv.put())) ||
                FAILED(conv->Initialize(mono.p, GUID_WICPixelFormat32bppPBGRA,
                      WICBitmapDitherTypeNone, nullptr, 0.0, WICBitmapPaletteTypeCustom)) ||
                FAILED(self->CreateBitmapFromSource(conv.p, WICBitmapCacheOnLoad,
                                                     converted.put()))) return hr;
            mono.p->Release();
            mono.p = converted.detach();
        } else if (!IsEqualGUID(originalFormat, GUID_WICPixelFormat32bppBGRA)) {
            // Do not surprise a caller expecting an unfamiliar pixel format.
            return hr;
        }
        (*out)->Release();
        *out = mono.detach();
        unsigned n = ++g_wicHits;
        if (n <= 8 || n % 250 == 0) Wh_Log(L"V3 WIC recoloured icon #%u", n);
    }
    return hr;
}

// Discover the documented CreateBitmapFromHICON method from the live COM
// interface (IUnknown slots 0-2, method slot 22). No private shell offsets.
bool HookWic(REFCLSID clsid) {
    Com<IWICImagingFactory> factory;
    if (FAILED(CoCreateInstance(clsid, nullptr, CLSCTX_INPROC_SERVER,
            __uuidof(IWICImagingFactory), reinterpret_cast<void**>(factory.put()))))
        return false;
    void* address = (*reinterpret_cast<void***>(factory.p))[22];
    if (!address) return false;
    if (address == g_wicAddress[0] || address == g_wicAddress[1]) return true;
    int n = !g_wicAddress[0] ? 0 : 1;
    if (g_wicAddress[n]) return false;
    void* hook = n == 0 ? reinterpret_cast<void*>(WicIconHook<0>) :
                          reinterpret_cast<void*>(WicIconHook<1>);
    if (!Wh_SetFunctionHook(address, hook, reinterpret_cast<void**>(&g_wicOriginal[n])))
        return false;
    g_wicAddress[n] = address;
    Wh_Log(L"V3 WIC hook registered (implementation %d)", n);
    return true;
}

// Construct a temporary alpha icon for standard system image-list drawing.
// The original HICON, its mask and its resource files are never modified.
HICON MakeMonoIcon(HICON icon) {
    Com<IWICImagingFactory> factory;
    if (FAILED(CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER,
        __uuidof(IWICImagingFactory), reinterpret_cast<void**>(factory.put())))) return nullptr;
    Com<IWICBitmap> src, mono;
    if (FAILED(factory->CreateBitmapFromHICON(icon, src.put())) ||
        FAILED(MakeMonoBitmap(factory.p, src.p, mono.put()))) return nullptr;
    UINT w = 0, h = 0;
    if (FAILED(mono->GetSize(&w, &h)) || !w || !h || w > 512 || h > 512) return nullptr;
    Com<IWICFormatConverter> conv;
    if (FAILED(factory->CreateFormatConverter(conv.put())) ||
        FAILED(conv->Initialize(mono.p, GUID_WICPixelFormat32bppPBGRA,
            WICBitmapDitherTypeNone, nullptr, 0.0, WICBitmapPaletteTypeCustom))) return nullptr;
    BITMAPINFO bmi{};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = static_cast<LONG>(w);
    bmi.bmiHeader.biHeight = -static_cast<LONG>(h);
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;
    void* bits = nullptr;
    HBITMAP color = CreateDIBSection(nullptr, &bmi, DIB_RGB_COLORS, &bits, nullptr, 0);
    if (!color || !bits) { if (color) DeleteObject(color); return nullptr; }
    HICON result = nullptr;
    if (SUCCEEDED(conv->CopyPixels(nullptr, w * 4, w * h * 4, static_cast<BYTE*>(bits)))) {
        ICONINFO ii{};
        if (GetIconInfo(icon, &ii)) {
            if (ii.fIcon && ii.hbmMask && ii.hbmColor) {
                ICONINFO replacement = ii;
                replacement.hbmColor = color;
                result = CreateIconIndirect(&replacement);
            }
            if (ii.hbmMask) DeleteObject(ii.hbmMask);
            if (ii.hbmColor) DeleteObject(ii.hbmColor);
        }
    }
    DeleteObject(color);
    return result;
}

// Restrict drawing hooks to the standard system icon lists returned by the
// shell, not arbitrary image lists or thumbnail providers.
IImageList* g_systemLists[4]{};
using ListDrawFn = HRESULT (STDMETHODCALLTYPE*)(IImageList*, IMAGELISTDRAWPARAMS*);
ListDrawFn g_listOriginal[4]{};
void* g_listAddress[4]{};

bool IsSystemList(IImageList* self) {
    for (auto* list : g_systemLists) if (list && self == list) return true;
    return false;
}

bool TryDraw(IImageList* self, IMAGELISTDRAWPARAMS* p) {
    if (!g_listsEnabled || g_depth || !self || !p || !IsSystemList(self) ||
        p->cbSize < sizeof(IMAGELISTDRAWPARAMS) || !p->hdcDst || p->i < 0 ||
        p->xBitmap || p->yBitmap || p->fState != ILS_NORMAL) return false;
    // Leave selected, blended, masked, raster-op and otherwise specialised
    // draws to Windows. Preserve overlay badges on ordinary icon draws.
    const UINT allowed = ILD_TRANSPARENT | ILD_OVERLAYMASK;
    if ((p->fStyle & ~allowed) != 0) return false;
    if (!(p->fStyle & ILD_TRANSPARENT) && p->rgbBk != CLR_NONE) {
        COLORREF background = CLR_DEFAULT;
        if (p->rgbBk != CLR_DEFAULT || FAILED(self->GetBkColor(&background)) ||
            background != CLR_NONE) return false;
    }
    int w = 0, h = 0;
    if (FAILED(self->GetIconSize(&w, &h)) || w <= 0 || h <= 0 ||
        (p->cx && p->cx != w) || (p->cy && p->cy != h)) return false;
    DepthGuard guard;
    HICON original = nullptr;
    if (FAILED(self->GetIcon(p->i, p->fStyle, &original)) || !original) return false;
    HICON mono = MakeMonoIcon(original);
    DestroyIcon(original);
    if (!mono) return false;
    BOOL ok = DrawIconEx(p->hdcDst, p->x, p->y, mono, w, h, 0, nullptr, DI_NORMAL);
    DestroyIcon(mono);
    if (ok) {
        unsigned n = ++g_listHits;
        if (n <= 8 || n % 250 == 0) Wh_Log(L"V3 shell image-list recoloured icon #%u", n);
    }
    return ok != FALSE;
}

template<int N>
HRESULT STDMETHODCALLTYPE ListDrawHook(IImageList* self, IMAGELISTDRAWPARAMS* p) {
    if (TryDraw(self, p)) return S_OK;
    return g_listOriginal[N](self, p);
}

bool HookSystemLists() {
    const int sizes[] = {SHIL_SMALL, SHIL_LARGE, SHIL_EXTRALARGE, SHIL_JUMBO};
    void* hooks[] = {reinterpret_cast<void*>(ListDrawHook<0>),
                     reinterpret_cast<void*>(ListDrawHook<1>),
                     reinterpret_cast<void*>(ListDrawHook<2>),
                     reinterpret_cast<void*>(ListDrawHook<3>)};
    int count = 0;
    for (int i = 0; i < 4; ++i) {
        IImageList* list = nullptr;
        if (FAILED(SHGetImageList(sizes[i], __uuidof(IImageList),
                                  reinterpret_cast<void**>(&list))) || !list) continue;
        // IImageList::Draw is slot 8 in the documented COM interface.
        void* address = (*reinterpret_cast<void***>(list))[8];
        bool covered = false;
        for (int j = 0; j < count; ++j) if (g_listAddress[j] == address) covered = true;
        if (!covered && address && count < 4 &&
            Wh_SetFunctionHook(address, hooks[count],
                               reinterpret_cast<void**>(&g_listOriginal[count]))) {
            g_listAddress[count++] = address;
            covered = true;
        }
        if (covered) g_systemLists[i] = list;
        else list->Release();
    }
    Wh_Log(L"V3 shell image-list hook implementations: %d", count);
    return count > 0;
}

bool IsExplorer() {
    wchar_t name[MAX_PATH]{};
    if (!GetModuleFileNameW(nullptr, name, MAX_PATH)) return false;
    const wchar_t* base = wcsrchr(name, L'\\');
    return _wcsicmp(base ? base + 1 : name, L"explorer.exe") == 0;
}

} // namespace

BOOL Wh_ModInit() {
    ReadSettings();
    Wh_Log(L"V3 starting (minimum brightness=%d, inverted=%d)", g_minimumBrightness.load(), int(g_invert.load()));
    HRESULT com = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    if (FAILED(com) && com != RPC_E_CHANGED_MODE) {
        Wh_Log(L"V3 COM initialization failed: 0x%08X", unsigned(com));
        return FALSE;
    }
    bool wic1 = HookWic(CLSID_WICImagingFactory);
    bool wic2 = HookWic(CLSID_WICImagingFactory2);
    bool lists = IsExplorer() && HookSystemLists();
    if (SUCCEEDED(com)) CoUninitialize();
    Wh_Log(L"V3 ready: WIC=%d WIC2=%d shellLists=%d", int(wic1), int(wic2), int(lists));
    return wic1 || wic2 || lists;
}

void Wh_ModSettingsChanged() { ReadSettings(); }

void Wh_ModUninit() {
    // Release references without clearing or rewriting shell image lists.
    for (auto*& list : g_systemLists) {
        if (list) { list->Release(); list = nullptr; }
    }
    Wh_Log(L"V3 stopped: WIC hits=%u, shell list hits=%u", g_wicHits.load(), g_listHits.load());
}
